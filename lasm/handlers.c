#include <stdio.h>
#include <string.h>

#define YY_HEADER_EXPORT_START_CONDITIONS
#include "llex.yy.h"
#include "handlers.h"
#include "lasm.h"

#define CHECK_START(name, type) if (par->token_idx != 0) { \
	fprintf(stderr, LERRL(name " `%s` must be at the start of line.\n"), \
			yylineno, token); \
	lasm_ret = 0x10 + type; \
	return 0; \
}

#define CHECK_SPACE() if (par->token_idx > 0 \
		&& par->statement[par->token_idx - 1].type != SPACE) { \
	fprintf(stderr, LERRL("Invalid token. Did you mean `%s`?\n"), \
			yylineno, token); \
	lasm_ret = 0x10 + UNKNOWN; \
	return 0; \
}

int handle_instr(char *token, struct parser *par)
{
	if (strcmp(token, "mod") == 0) {
		llex_set_state(MODSC);
	}
	CHECK_START("Instruction", INSTR);
	return par_add_token(par, INSTR, token);
}

int handle_macro(char *token, struct parser *par)
{
	if (strcmp(token, "RAW") == 0) {
		llex_set_state(RAWSC);
	}
	CHECK_START("Macro", MACRO);
	return par_add_token(par, MACRO, token);
}

int handle_comment(char *token, struct parser *par)
{
	return par_add_token(par, LABEL, token);
}

int handle_label(char *token, struct parser *par)
{
	if (par->token_idx == 0)
		return par_add_symbol(par, token, par->out_buf->n_elems);

	return par_add_token(par, LABEL, token);
}

int handle_raw(char *token, struct parser *par)
{
	return par_add_token(par, RAW, token);
}

int handle_number(char *token, struct parser *par)
{
	return par_add_token(par, NUMBER, token);
}

int handle_string(char *token, struct parser *par)
{
	return par_add_token(par, STRING, token);
}

int handle_char(char *token, struct parser *par)
{
	return par_add_token(par, CHAR, token);
}

int handle_register(char *token, struct parser *par)
{
	return par_add_token(par, REGISTER, token);
}

int handle_signal(char *token, struct parser *par)
{
	return par_add_token(par, SIGNAL, token);
}

int handle_implied(char *token, struct parser *par)
{
	return par_add_token(par, IMPLIED, token);
}

int handle_operation(char *token, struct parser *par)
{
	return par_add_token(par, OPERATION, token);
}

int handle_condition(char *token, struct parser *par)
{
	const int i = par->token_idx;
	CHECK_SPACE();
	if (i != 2 || par->statement[0].type != INSTR
			|| strcmp(par->statement[0].text, "cond") != 0) {
		fprintf(stderr, LERRL("Invalid use of condition `%s`.\n"),
				yylineno, token);
		lasm_ret = 0x10 + CONDITION;
		return 0;
	}

	return par_add_token(par, CONDITION, token);
}

int handle_addrmode(char *token, struct parser *par)
{
	return par_add_token(par, ADDRMODE, token);
}

int handle_space(char *token, struct parser *par)
{
	const int i = par->token_idx;
	if (i == 0 || par->statement[i - 1].type == SPACE)
		return 1;

	return par_add_token(par, SPACE, token);
}

int handle_newline(char *token, struct parser *par)
{
	llex_set_state(0);
	par_end_statement(par);
	return 1;
}

int handle_unknown(char *token, struct parser *par)
{
	fprintf(stderr, LERRL("Invalid token.\n"), yylineno);
	lasm_ret = 0x10 + UNKNOWN;
	return 0;
}
