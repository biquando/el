#ifndef _LASM_HANDLERS_H
#define _LASM_HANDLERS_H

#include "parser.h"
#include "style.h"

extern int lasm_ret;

int handle_instr(char *token, struct parser *par);
int handle_macro(char *token, struct parser *par);
int handle_comment(char *token, struct parser *par);
int handle_label(char *token, struct parser *par);
int handle_number(char *token, struct parser *par);
int handle_raw(char *token, struct parser *par);
int handle_string(char *token, struct parser *par);
int handle_char(char *token, struct parser *par);
int handle_register(char *token, struct parser *par);
int handle_signal(char *token, struct parser *par);
int handle_implied(char *token, struct parser *par);
int handle_operation(char *token, struct parser *par);
int handle_condition(char *token, struct parser *par);
int handle_addrmode(char *token, struct parser *par);
int handle_space(char *token, struct parser *par);
int handle_newline(char *token, struct parser *par);
int handle_unknown(char *token, struct parser *par);

#endif
