#include <stdio.h>
#include <string.h>

#define YY_HEADER_EXPORT_START_CONDITIONS
#include "llex.yy.h"
#include "handlers.h"
#include "lasm.h"
#include "style.h"

#define TOKEN_START() if (0) {
#define TOKEN(ttext, ttype) } else if (strcmp(token, ttext) == 0) { type = ttype
#define TOKEN_DEFAULT() } else {
#define TOKEN_END() }

static int _try_add_token(struct parser *par, enum token_type type, char *text)
{
	int success = par_add_token(par, type, text);
	if (!success) {
		fprintf(stderr, LERRL("Couldn't add token `%s`. This statement "
					"may be too long.\n"),
				yylineno, text);
		lasm_ret = 0x30 + type;
		return 0;
	}
	return 1;
}

static int _try_add_symbol(struct parser *par, char *name, int value)
{
	int success = par_add_symbol(par, name, value);
	if (!success) {
		fprintf(stderr, LERRL("Couldn't add symbol `%s`. This symbol "
					"may already be defined.\n"),
				yylineno, name);
		lasm_ret = 0x2F;
		return 0;
	}
	return 1;
}

static int _try_add_ref(struct parser *par, char *name, int size, int lineno)
{
	int success = par_add_ref(par, name, size, lineno);
	if (!success) {
		fprintf(stderr, LERRL("Couldn't add reference `%s`.\n"),
				yylineno, name);
		lasm_ret = 0x2E;
		return 0;
	}
	return 1;
}

static int _try_write_byte(struct parser *par, unsigned char b)
{
	int success = par_write_byte(par, b);
	if (!success) {
		fprintf(stderr, LERRL("Couldn't add byte %x\n"),
				yylineno, b);
		lasm_ret = 0x40;
		return 0;
	}
	return 1;
}

int handle_instr(char *token, struct parser *par)
{
	enum token_type type;

	TOKEN_START();
	TOKEN("load", LOAD);
	TOKEN("store", STORE);
	TOKEN("mod", MOD);
		llex_set_state(MODSC);
	TOKEN("cond", COND);
	TOKEN("sig", SIG);
	TOKEN("nop", NOP);
	TOKEN_DEFAULT();
		fprintf(stderr, LERRL("Unexpected instruction: %s\n"),
				yylineno, token);
		lasm_ret = 0x60;
		return 0;
	TOKEN_END();

	return _try_add_token(par, type, token);
}

int handle_macro(char *token, struct parser *par)
{
	enum token_type type;

	TOKEN_START();
	TOKEN("RET", NON);
	TOKEN("PSH", UN_REG);
	TOKEN("POP", UN_REG);
	TOKEN("INC", UN_REG);
	TOKEN("DEC", UN_REG);
	TOKEN("GOTO", UN_IMM);
	TOKEN("INT", UN_IMM);
	TOKEN("CALL", UN_IMM);
	TOKEN("CHA", UN_CHA);
	TOKEN("STR", UN_STR);
	TOKEN("RAW", UN_RAW);
		llex_set_state(RAWSC);
	TOKEN("TRA", BIN_REG_REG);
	TOKEN_DEFAULT();
		fprintf(stderr, LERRL("Unexpected macro: %s\n"),
				yylineno, token);
		lasm_ret = 0x61;
		return 0;
	TOKEN_END();

	return _try_add_token(par, type, token);
}

int handle_directive(char *token, struct parser *par)
{
	return _try_add_token(par, DIRECTIVE, token);
}

int handle_comment(char *token, struct parser *par)
{
	return 1;
}

int handle_label(char *token, struct parser *par)
{
	if (par->token_idx == 0)
		return _try_add_symbol(par, token,
				par->out_buf->n_elems + par->global);

	return _try_add_token(par, IMM, token);
}

int handle_raw(char *token, struct parser *par)
{
	return _try_add_token(par, RAW, token);
}

int handle_number(char *token, struct parser *par)
{
	return _try_add_token(par, IMM, token);
}

int handle_string(char *token, struct parser *par)
{
	return _try_add_token(par, STR, token);
}

int handle_char(char *token, struct parser *par)
{
	return _try_add_token(par, CHA, token);
}

int handle_register(char *token, struct parser *par)
{
	return _try_add_token(par, REG, token);
}

int handle_signal(char *token, struct parser *par)
{
	enum token_type type;

	TOKEN_START();
	TOKEN("abort", VAL_SIG);
	TOKEN("out", VAL_SIG);
	TOKEN_DEFAULT();
		fprintf(stderr, LERRL("Unexpected signal: %s\n"),
				yylineno, token);
		lasm_ret = 0x62;
		return 0;
	TOKEN_END();

	return _try_add_token(par, type, token);
}

int handle_implied(char *token, struct parser *par)
{
	int failed = 0;
	failed |= !_try_write_byte(par, 0x15); /* Opcode for `load # rar` */
	failed |= !_try_add_ref(par, token + 1, 2, yylineno);
	failed |= !_try_write_byte(par, 0x00);
	failed |= !_try_write_byte(par, 0x00);
	return !failed;
}

int handle_operation(char *token, struct parser *par)
{
	enum token_type type;

	TOKEN_START();
	TOKEN("+", BIN_OP);
	TOKEN("-", BIN_OP);
	TOKEN("++", UN_OP);
	TOKEN("--", UN_OP);
	TOKEN("&", BIN_OP);
	TOKEN("|", BIN_OP);
	TOKEN("^", BIN_OP);
	TOKEN("~", UN_OP);
	TOKEN(">>", BIN_OP);
	TOKEN("<<", BIN_OP);
	TOKEN("/>", BIN_OP);
	TOKEN("?", BIN_OP);
	TOKEN_DEFAULT();
		fprintf(stderr, LERRL("Unexpected operation: %s\n"),
				yylineno, token);
		lasm_ret = 0x63;
		return 0;
	TOKEN_END();

	return _try_add_token(par, type, token);
}

int handle_condition(char *token, struct parser *par)
{
	return _try_add_token(par, CONDITION, token);
}

int handle_addrmode(char *token, struct parser *par)
{
	enum token_type type;

	TOKEN_START();
	TOKEN("#", IMM_ADDR);
	TOKEN("*", ABS_ADDR);
	TOKEN("+", ABS_ADDR);
	TOKEN_DEFAULT();
		fprintf(stderr, LERRL("Unexpected addressing mode: %s\n"),
				yylineno, token);
		lasm_ret = 0x64;
		return 0;
	TOKEN_END();

	return _try_add_token(par, type, token);
}


int handle_space(char *token, struct parser *par)
{
	const int i = par->token_idx;
	if (i == 0 || par->statement[i - 1].type == SPACE)
		return 1;

	return _try_add_token(par, SPACE, token);
}

int handle_newline(char *token, struct parser *par)
{
	llex_set_state(0);
	if (!par_end_statement(par, yylineno - 1)) {
		fprintf(stderr, LERRL("Invalid statement.\n"),
				yylineno - 1);
		lasm_ret = 0x7f;
		return 0;
	}
	return 1;
}

int handle_unknown(char *token, struct parser *par)
{
	fprintf(stderr, LERRL("Invalid token.\n"), yylineno);
	lasm_ret = 0x10 + NONE;
	return 0;
}
