#ifndef _EL_SIGNAL_H
#define _EL_SIGNAL_H

#include "cpu.h"
#include "types.h"

void sig_abort(struct cpu *proc, word_t status);
void sig_out(struct cpu *proc, word_t mode);

#endif
