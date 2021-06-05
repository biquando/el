#include <time.h>
#include <stdio.h>
#include "memory.h"
#include "cpu.h"

void __debug(struct cpu *proc)
{
    fprintf(stderr, "ip=0x%04x:0x%04x\ta=0x%04x\tb=0x%04x\tc=0x%04x\td=0x%04x\tx=0x%04x\tar=0x%04x\tsp=0x%04x\n", 
        proc->ip.full, proc->mem->data[proc->ip.full], proc->a.full, 
        proc->b.full, proc->c.full, proc->d.full, proc->x.full, proc->ar.full, proc->sp.full);
}

int main(int argc, char *argv[])
{
    struct memory *mem;
    struct cpu *proc;
    int i;
    mem = initialize_memory(0x10000);
    proc = initialize_cpu(mem);
    proc->ip.full = 0x0000;
    //proc->__debug = __debug;

    for (i = 1; i < argc; i++) {
        load_file(mem, argv[i]);
    }
    
    start_clock(proc, 10);

    free_cpu(proc);
    free_memory(mem);
    return 0;
}
