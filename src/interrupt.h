#ifndef _EL_INTERRUPT_H
#define _EL_INTERRUPT_H

#include "types.h"
#include "cpu.h"

enum interrupt_signal {
    SIG_INPUT = 0
};

struct interrupt_pkg {
    enum interrupt_signal sig;
    word_t start_addr;
    unsigned int len;
    void (*callback)(void *proc, int sig);
    volatile int *flag;
    struct cpu *proc;
};

#endif
