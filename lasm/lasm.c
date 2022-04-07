#include <stdio.h>

#include "llex.yy.h"
#include "lasm.h"

int lasm_ret = 0;

int main(int argc, char *argv[])
{
	if (argc > 1) {
		yyin = fopen(argv[1], "r");
		if (!yyin) {
			fprintf(stderr, "Error: File does not exist: %s\n", argv[1]);
			return 1;
		}
	}
	llex_init();
	yylex();
	fclose(yyin);
	return lasm_ret;
}
