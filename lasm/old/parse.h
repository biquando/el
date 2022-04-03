#ifndef _LASM_PARSE_H
#define _LASM_PARSE_H

#include "buffer.h"

char *skip_whitespace(char *str);
unsigned int symbol_len(char *str);
char *next_symbol(char *str);

int parse_addrmode(char *arg, unsigned int len);
int parse_register(char *arg, unsigned int len);
int parse_operation(char *arg, unsigned int len);
int parse_cond(char *arg, unsigned int len);
int parse_sigid(char *arg, unsigned int len);
int parse_hexbyte(char *arg);
int parse_char(char *arg, unsigned int len);
int parse_escape(char c);
int parse_string(char *arg, unsigned int len, char *buffer);
int parse_quote_len(char **arg, char q);

void parse_line(char *str, struct buffer *buf, struct symtab *sym);
void parse_instr(char *str, struct buffer *buf, struct symtab *sym);
void parse_macro(char *str, struct buffer *buf, struct symtab *sym);
void parse_label(char *str, struct buffer *buf, struct symtab *sym);

int parse_number(char *arg, unsigned int len, struct symtab *sym,
                 struct buffer *buf);

#endif
