#define YY_HEADER_EXPORT_START_CONDITIONS
#include "llex.yy.h"
#include "lasm.h"

#include <stdio.h>

static int lasm_ret = 0;

int main(int argc, char *argv[])
{
	if (argc > 1) {
		yyin = fopen(argv[1], "r");
		if (!yyin) {
			fprintf(stderr, "Error: File does not exist: %s\n", argv[1]);
			return 1;
		}
	}
	init_llex();
	yylex();
	fclose(yyin);
	return lasm_ret;
}

int handle_instr(char *token)
{
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

int handle_labeldef(char *token)
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

int handle_addrmode(char *token)
{
	return 0;
}

int handle_newline(char *token)
{
	return 0;
}


int handle_unknown(char *token)
{
	printf("Unknown token on line %d\n", yylineno);
	lasm_ret = 2;
	return 1;
}
