#include <stdio.h>
#include <string.h>

#define YY_HEADER_EXPORT_START_CONDITIONS
#include "llex.yy.h"
#include "handlers.h"
#include "lasm.h"

int handle_instr(char *token, struct parser *par)
{
	if (strcmp(token, "mod") == 0) {
		llex_set_state(MODSC);
	}
	return par_add_token(par, INSTR, token);
}

int handle_macro(char *token, struct parser *par)
{
	if (strcmp(token, "RAW") == 0) {
		llex_set_state(RAWSC);
	}
	return par_add_token(par, MACRO, token);
}

int handle_comment(char *token, struct parser *par)
{
	return par_add_token(par, LABEL, token);
}

int handle_label(char *token, struct parser *par)
{
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
	if (i == 0 || par->statement[0].type != CONDITION) {
		fprintf(stderr, LERRL("Condition `%s` must be used in a cond "
					"instruction.\n"), yylineno, token);
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
		return 0;

	return par_add_token(par, SPACE, token);
}

int handle_newline(char *token, struct parser *par)
{
	llex_set_state(0);
	return 1;
}

int handle_unknown(char *token, struct parser *par)
{
	fprintf(stderr, LERRL("Invalid token.\n"), yylineno);
	lasm_ret = 2;
	return 0;
}
