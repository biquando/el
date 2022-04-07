#ifndef _LASM_HANDLERS_H
#define _LASM_HANDLERS_H

extern int lasm_ret;

int handle_instr(char *token);
int handle_macro(char *token);
int handle_comment(char *token);
int handle_label(char *token);
int handle_number(char *token);
int handle_string(char *token);
int handle_char(char *token);
int handle_register(char *token);
int handle_signal(char *token);
int handle_implied(char *token);
int handle_operation(char *token);
int handle_condition(char *token);
int handle_addrmode(char *token);
int handle_space(char *token);
int handle_newline(char *token);
int handle_unknown(char *token);

#endif
