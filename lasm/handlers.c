#include <stdio.h>
#include <string.h>

#define YY_HEADER_EXPORT_START_CONDITIONS
#include "llex.yy.h"
#include "handlers.h"
#include "lasm.h"

int handle_instr(char *token)
{
	if (strcmp(token, "mod") == 0) {
		llex_set_state(MOD);
	}
	return 0;
}

int handle_macro(char *token)
{
	return 0;
}

int handle_comment(char *token)
{
	return 0;
}

int handle_label(char *token)
{
	return 0;
}

int handle_number(char *token)
{
	return 0;
}

int handle_string(char *token)
{
	return 0;
}

int handle_char(char *token)
{
	return 0;
}

int handle_register(char *token)
{
	return 0;
}

int handle_signal(char *token)
{
	return 0;
}

int handle_implied(char *token)
{
	return 0;
}

int handle_operation(char *token)
{
	return 0;
}

int handle_condition(char *token)
{
	return 0;
}

int handle_addrmode(char *token)
{
	return 0;
}

int handle_space(char *token)
{
	return 0;
}

int handle_newline(char *token)
{
	llex_set_state(0);
	return 0;
}


int handle_unknown(char *token)
{
	fprintf(stderr, "Error: Invalid token on line %d\n", yylineno);
	lasm_ret = 2;
	return 1;
}
