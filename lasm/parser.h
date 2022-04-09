#ifndef _LASM_PARSER_H
#define _LASM_PARSER_H

#include <vector/vector.h>

#define BUF_DEFAULT_SIZE 1024

enum token_type {
	INSTR, MACRO, COMMENT, LABEL, NUMBER, STRING, CHAR, REGISTER, SIGNAL,
	IMPLIED, OPERATION, CONDITION, ADDRMODE, SPACE, NEWLINE, UNKNOWN
};

struct token {
	enum token_type type;
	const char *text;
};

struct parser {
	struct vector *out_buf;
};

struct parser *par_init();
void par_free(struct parser *par);

#endif
