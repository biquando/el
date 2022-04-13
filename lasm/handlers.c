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

#define CHECK_CNTXT(cntxt_type, cntxt, cntxt_pos, pos) (par->token_idx == pos \
		&& par->statement[cntxt_pos].type == cntxt_type \
		&& strcmp(par->statement[cntxt_pos].text, cntxt) == 0)

#define INVALID_USE(name, type) \
		fprintf(stderr, LERRL("Invalid use of "name" `%s`.\n"), \
				yylineno, token); \
		lasm_ret = 0x10 + type; \
		return 0

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
	if (!CHECK_CNTXT(MACRO, "RAW", 0, 2)) {
		INVALID_USE("raw", RAW);
	}
	return par_add_token(par, RAW, token);
}

int handle_number(char *token, struct parser *par)
{
	return par_add_token(par, NUMBER, token);
}

int handle_string(char *token, struct parser *par)
{
	if (!CHECK_CNTXT(MACRO, "STR", 0, 2)) {
		INVALID_USE("string", STRING);
	}
	return par_add_token(par, STRING, token);
}

int handle_char(char *token, struct parser *par)
{
	if (!CHECK_CNTXT(MACRO, "CHA", 0, 2)) {
		INVALID_USE("character", CHAR);
	}
	return par_add_token(par, CHAR, token);
}

int handle_register(char *token, struct parser *par)
{
	CHECK_SPACE();
	if (
			   !CHECK_CNTXT(INSTR, "load", 0, 4)
			&& !CHECK_CNTXT(INSTR, "store", 0, 4)
			&& !CHECK_CNTXT(INSTR, "mod", 0, 2)
			&& !(CHECK_CNTXT(INSTR, "mod", 0, 6)
			/* This context shall not be used with the following
			 * mod operations. */
				&& !(
					   CHECK_CNTXT(OPERATION, "++", 4, 6)
					|| CHECK_CNTXT(OPERATION, "--", 4, 6)
					|| CHECK_CNTXT(OPERATION, "~", 4, 6)
				))
			&& !CHECK_CNTXT(MACRO, "PSH", 0, 2)
			&& !CHECK_CNTXT(MACRO, "POP", 0, 2)
			&& !CHECK_CNTXT(MACRO, "INC", 0, 2)
			&& !CHECK_CNTXT(MACRO, "DEC", 0, 2)
			&& !CHECK_CNTXT(MACRO, "TRA", 0, 2)
			&& !CHECK_CNTXT(MACRO, "TRA", 0, 4)) {
		INVALID_USE("register", REGISTER);
	}
	return par_add_token(par, REGISTER, token);
}

int handle_signal(char *token, struct parser *par)
{
	CHECK_SPACE();
	if (!CHECK_CNTXT(INSTR, "sig", 0, 2)) {
		INVALID_USE("signal", SIGNAL);
	}
	return par_add_token(par, SIGNAL, token);
}

int handle_implied(char *token, struct parser *par)
{
	CHECK_SPACE();
	return par_add_token(par, IMPLIED, token);
}

int handle_operation(char *token, struct parser *par)
{
	CHECK_SPACE();
	if (!CHECK_CNTXT(INSTR, "mod", 0, 4)) {
		INVALID_USE("operation", OPERATION);
	}
	return par_add_token(par, OPERATION, token);
}

int handle_condition(char *token, struct parser *par)
{
	CHECK_SPACE();
	if (!CHECK_CNTXT(INSTR, "cond", 0, 2)) {
		INVALID_USE("condition", CONDITION);
	}
	return par_add_token(par, CONDITION, token);
}

int handle_addrmode(char *token, struct parser *par)
{
	CHECK_SPACE();
	if (!CHECK_CNTXT(INSTR, "load", 0, 2)
			&& !CHECK_CNTXT(INSTR, "store", 0, 2)) {
		INVALID_USE("addressing mode", ADDRMODE);
	}
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
