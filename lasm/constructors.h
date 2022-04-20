#ifndef _LASM_CONSTRUCTORS_H
#define _LASM_CONSTRUCTORS_H

#include "parser.h"

int construct_load(struct parser *par, int lineno);
int construct_store(struct parser *par, int lineno);
int construct_mod(struct parser *par, int lineno);
int construct_cond(struct parser *par, int lineno);
int construct_sig(struct parser *par, int lineno);
int construct_nop(struct parser *par, int lineno);
int construct_non(struct parser *par, int lineno);
int construct_un_reg(struct parser *par, int lineno);
int construct_un_imm(struct parser *par, int lineno);
int construct_un_cha(struct parser *par, int lineno);
int construct_un_str(struct parser *par, int lineno);
int construct_un_raw(struct parser *par, int lineno);
int construct_bin_reg_reg(struct parser *par, int lineno);

#endif
