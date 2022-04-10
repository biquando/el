#ifndef _LASM_PARSER_H
#define _LASM_PARSER_H

#include <vector/vector.h>

#define BUF_DEFAULT_SIZE 1024
#define MAX_STATEMENT_SIZE 10

enum token_type {
	INSTR, MACRO, COMMENT, LABEL, RAW, NUMBER, STRING, CHAR, REGISTER,
	SIGNAL, IMPLIED, OPERATION, CONDITION, ADDRMODE, SPACE, NEWLINE, UNKNOWN
};

struct token {
	enum token_type type;
	char *text;
};

struct symbol_entry {
	char *name;
	int value; /* values are one or two bytes long */
	int size;  /* (value & 0xFF) or (value & 0xFFFF) */
};

struct ref_entry {
	int location;
	int sym_idx;
	int size; /* we will check that this matches the corresponding symbol */
};

struct parser {
	struct token statement[MAX_STATEMENT_SIZE];
	int token_idx;
	struct vector *symbol_table;
	struct vector *ref_table;
	struct vector *out_buf;
};

struct parser *par_init();
int par_add_token(struct parser *par, enum token_type type, char *text);
int par_add_symbol(struct parser *par, char *name, int value, int size);
int par_add_ref(struct parser *par, char *name, int size);
int par_write_byte(struct parser *par, unsigned char b);
void par_resolve_refs(struct parser *par);
void par_free(struct parser *par);

#endif
