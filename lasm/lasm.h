#ifndef _LASM_LASM_H
#define _LASM_LASM_H

#include <stdio.h>

extern int lasm_ret;

int llex_init();
void llex_set_state(int sc);
void llex_set_ofile(FILE *fd);
void llex_end();

#endif
