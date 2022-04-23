#ifndef _LASM_PARSER_H
#define _LASM_PARSER_H

#include <vector/vector.h>

#define BUF_DEFAULT_SIZE 1024
#define MAX_STATEMENT_SIZE 10

enum token_type {
	NONE, SPACE,
	LOAD, STORE, MOD, COND, SIG, NOP,
	NON, UN_REG, UN_IMM, UN_CHA, UN_STR, UN_RAW, BIN_REG_REG,
	IMM_ADDR, ABS_ADDR,
	REG, IMM, CHA, STR, RAW,
	UN_OP, BIN_OP,
	CONDITION,
	UNVAL_SIG, VAL_SIG,
	DIRECTIVE
};

struct token {
	enum token_type type;
	char *text;
};

struct symbol_entry {
	char *name;
	int value;
};

struct ref_entry {
	int location;
	char *name;
	int size;
	int lineno;
};

struct parser {
	struct token statement[MAX_STATEMENT_SIZE];
	int token_idx;
	int global;
	struct vector *symbol_table;
	struct vector *ref_table;
	struct vector *out_buf;
};

struct parser *par_init();
int par_add_token(struct parser *par, enum token_type type, char *text);
int par_end_statement(struct parser *par, int lineno);
int par_set_global(struct parser *par, char *token);
int par_add_symbol(struct parser *par, char *name, int value);
int par_add_ref(struct parser *par, char *name, int size, int lineno);
int par_write_byte(struct parser *par, unsigned char b);
int par_resolve_refs(struct parser *par, struct ref_entry *err_ref);
void par_free(struct parser *par);

#endif
