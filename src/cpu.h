#ifndef _EL_CPU_H
#define _EL_CPU_H

#include "types.h"
#include "memory.h"

union reg {
    word_t full;
    byte_t half;
};

struct cpu {
    void (*tick)(struct cpu *proc);
    void (*__debug)(struct cpu *proc);
    union reg a, b, c, d, x, ar, sp, ip;
    byte_t fo, fc, fz, fs;
    struct memory *mem;
	volatile char running;
    unsigned long cycle_count;
};

struct cpu *initialize_cpu(struct memory *mem);
void free_cpu(struct cpu *proc);

void start_clock(struct cpu *proc, unsigned int delay_ms);
void stop_clock(struct cpu *proc);

void __tick_cpu(struct cpu *proc);
void __handle_interrupt(void *proc, int sig);
union reg *__select_register(struct cpu *proc, byte_t r);
void __execute(struct cpu *proc);

void __instr_load(union reg *r, byte_t h, word_t value);
void __instr_store(struct memory *mem, union reg *r, byte_t h, word_t addr);
void __instr_mod(struct cpu *proc, union reg *r, byte_t h, word_t opr);
void __instr_cond(struct cpu *proc, word_t opr);

#endif
