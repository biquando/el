#ifndef _LASM_LASM_H
#define _LASM_LASM_H

extern int lasm_ret;

void llex_init();
void llex_set_state(int sc);
void llex_cleanup();

#endif
