#ifndef _EL_TERMINAL_H
#define _EL_TERMINAL_H

#include "types.h"
#include "interrupt.h"

void terminal_output(byte_t *start, word_t mode);
void *terminal_input(void *pkg_ptr);

#endif
