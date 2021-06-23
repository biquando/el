#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <argparse/argparse.h>
#include "memory.h"
#include "cpu.h"

void __debug(struct cpu *proc)
{
    fprintf(stderr, "ip=0x%04x:0x%04x\ta=0x%04x\tb=0x%04x\tc=0x%04x\td=0x%04x\t\
            x=0x%04x\tar=0x%04x\tsp=0x%04x\n",
        proc->ip.full, proc->mem->data[proc->ip.full], proc->a.full,
        proc->b.full, proc->c.full, proc->d.full, proc->x.full, proc->ar.full,
        proc->sp.full);
}

int parse_error()
{
    fprintf(stderr, "El: Argument error.\n");
    exit(1);
}

int main(int argc, char *argv[])
{
    struct argparse *ap = argp_init();
    char **arg_vals = malloc(argc * sizeof *arg_vals);
    struct memory *mem;
    struct cpu *proc;
    int i, nargs;
    unsigned int clock_delay = 0;

    /* Initialize components */
    mem = initialize_memory(0x10000);
    proc = initialize_cpu(mem);
    proc->ip.full = 0x0000;

    /* Parse arguments */
    argp_opt(ap, "--debug", 0);
    argp_opt(ap, "--clock", 1);
    !argp_parse(ap, argc, argv) && parse_error();
    if (argp_get_opt(ap, "--debug", arg_vals)) {
        proc->__debug = __debug;
    }
    if (argp_get_opt(ap, "--clock", arg_vals)) {
        clock_delay = strtol(arg_vals[0], NULL, 0);
    }
    nargs = argp_get_args(ap, arg_vals);
    for (i = 0; i < nargs; i++) {
        load_file(mem, arg_vals[i]);
    }
    argp_free(ap);
    free(arg_vals);

    start_clock(proc, clock_delay);

    free_cpu(proc);
    free_memory(mem);
    return 0;
}
