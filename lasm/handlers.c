#include <stdio.h>
#include <string.h>

#define YY_HEADER_EXPORT_START_CONDITIONS
#include "llex.yy.h"
#include "handlers.h"
#include "lasm.h"

int handle_instr(char *token, struct parser *par)
{
	if (strcmp(token, "mod") == 0) {
		llex_set_state(MOD);
	}
	return 0;
}

int handle_macro(char *token, struct parser *par)
{
	if (strcmp(token, "RAW") == 0) {
		llex_set_state(RAW);
	}
	return 0;
}

int handle_comment(char *token, struct parser *par)
{
	return 0;
}

int handle_label(char *token, struct parser *par)
{
	return 0;
}

int handle_raw(char *token, struct parser *par)
{
	return 0;
}
int handle_number(char *token, struct parser *par)
{
	return 0;
}


int handle_string(char *token, struct parser *par)
{
	return 0;
}

int handle_char(char *token, struct parser *par)
{
	return 0;
}

int handle_register(char *token, struct parser *par)
{
	return 0;
}

int handle_signal(char *token, struct parser *par)
{
	return 0;
}

int handle_implied(char *token, struct parser *par)
{
	return 0;
}

int handle_operation(char *token, struct parser *par)
{
	return 0;
}

int handle_condition(char *token, struct parser *par)
{
	return 0;
}

int handle_addrmode(char *token, struct parser *par)
{
	return 0;
}

int handle_space(char *token, struct parser *par)
{
	return 0;
}

int handle_newline(char *token, struct parser *par)
{
	llex_set_state(0);
	return 0;
}


int handle_unknown(char *token, struct parser *par)
{
	fprintf(stderr, LERR("Invalid token on line %d\n"), yylineno);
	lasm_ret = 2;
	return 1;
}
