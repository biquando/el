#include <time.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include "cpu.h"
#include "types.h"
#include "terminal.h"
#include "interrupt.h"
#include "memory.h"
#include "signal.h"

struct cpu *initialize_cpu(struct memory *mem)
{
    struct cpu *proc = calloc(1, sizeof *proc);
    proc->tick = __tick_cpu;
    
    /* Registers */
    proc->a.full = 0;
    proc->b.full = 0;
    proc->c.full = 0;
    proc->d.full = 0;
    proc->x.full = 0;
    proc->ar.full = 0;
    proc->sp.full = 0;
    proc->ip.full = 0;

    /* Flags */
    proc->fo = 0;
    proc->fc = 0;
    proc->fz = 0;
    proc->fs = 0;

    proc->running = 0;
    proc->mem = mem;
    proc->cycle_count = 0;
    return proc;
}

void free_cpu(struct cpu *proc)
{
    free(proc);
}

void start_clock(struct cpu *proc, unsigned int delay_ms)
{
    /* Declarations */
    struct interrupt_pkg input_pkg;
    volatile int input_interrupt_flag = 0;
    unsigned int delay_sec;
    unsigned long delay_nsec;
    struct timespec req, rem;

    /* Timing */
    delay_sec = delay_ms / 1000;
    delay_nsec = (delay_ms % 1000 * 1000000L);
    req.tv_sec = delay_sec;
    req.tv_nsec = delay_nsec;

    /* Input interrupts */
    input_pkg.sig = SIG_INPUT;
    input_pkg.start_addr = 0x100;
    input_pkg.len = 64;
    input_pkg.callback = __handle_interrupt;
    input_pkg.flag = &input_interrupt_flag;
    input_pkg.proc = proc;

    proc->running = 1;
    
	while (proc->running) {
        /* Start threads for interrupt handling */
        if (!input_interrupt_flag) {
//          fprintf(stderr, "listening for interrupt %d (input)\n", SIG_INPUT);
            input_interrupt_flag = 1;
            pthread_t input_interrupt_thread;
            pthread_create(&input_interrupt_thread, NULL,
                           terminal_input, &input_pkg);
        }

        (*(proc->tick))(proc);
        nanosleep(&req, &rem);
	}
}

void stop_clock(struct cpu *proc)
{
    proc->running = 0;
}

void __tick_cpu(struct cpu *proc)
{
    if (proc->__debug)
        (*proc->__debug)(proc);
    __execute(proc);
}

void __handle_interrupt(void *proc_void, int sig)
{
    struct cpu *proc = (struct cpu *) proc_void;
    proc->running = 0;

    switch (sig) {
      case SIG_INPUT:
        /* proc->ip.full = 0; */
        break;
    }

    printf("handling interrupt, sig=%d\n", sig);
    proc->running = 1;
}

union reg *__select_register(struct cpu *proc, byte_t r)
{
    union reg *proc_reg = NULL;
    switch (r) {
    case 0:
        proc_reg = &proc->a;
        break;
    case 1:
        proc_reg = &proc->b;
        break;
    case 2:
        proc_reg = &proc->c;
        break;
    case 3:
        proc_reg = &proc->d;
        break;
    case 4:
        proc_reg = &proc->x;
        break;
    case 5:
        proc_reg = &proc->ar;
        break;
    case 6:
        proc_reg = &proc->sp;
        break;
    case 7:
        proc_reg = &proc->ip;
        break;
    }
    return proc_reg;
}

void __execute(struct cpu *proc)
{
    byte_t opc = proc->mem->data[proc->ip.full];
    word_t opr = read_word_from_memory(proc->mem, proc->ip.full + 1);

    byte_t type = opc >> 6;
    byte_t mode;
    byte_t h;
    union reg *r;
    byte_t sig;

    /* No-op */
    if (opc == 0xFF)
        goto next_instr;

    /* <Conditional> */
    if (type == 3) {
        __instr_cond(proc, opr);
        goto next_instr;
    }

    h = (opc >> 3) & 1;
    r = __select_register(proc, opc & 7);

    /* <Modify> */
    if (type == 2) {
        __instr_mod(proc, r, h, opr);
        goto next_instr;
    }

    mode = (opc >> 4) & 3;
    /* <Store> */
    if (type == 1) {
        switch (mode) {
        case 2:  /* [absolute] */
            opr = proc->ar.full;
            break;
        case 3:  /* [indexed] */
            opr = proc->ar.full + proc->x.full;
            break;
        default:
            return;
        }
        __instr_store(proc->mem, r, h, opr);
        goto next_instr;
    }

    /* <Signal> */
    sig = opc & 15;
    if (mode == 0) {
        switch (sig) {
        case 0:
            sig_abort(proc, opr);
            break;
        case 1:
            sig_out(proc, opr);
            break;
        }
        goto next_instr;
    }

    /* <Load> */
    switch (mode) {
    case 1:  /* [immediate] */
        break;
    case 2:  /* [absolute] */
        opr = read_word_from_memory(proc->mem, proc->ar.full);
        break;
    case 3:  /* [indexed] */
        opr = read_word_from_memory(proc->mem, proc->ar.full + proc->x.full);
        break;
    }
    __instr_load(r, h, opr);

next_instr:
    proc->ip.full += 3;
}
