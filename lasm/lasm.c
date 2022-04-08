#include <stdio.h>
#include <stdlib.h>
#include <argparse/argparse.h>

#include "llex.yy.h"
#include "lasm.h"

#define FBOLD    "\x1b[1m"
#define FITALIC  "\x1b[3m"
#define FUNDER   "\x1b[4m"
#define FRED     "\x1b[31m"
#define FGREEN   "\x1b[32m"
#define FYELLOW  "\x1b[33m"
#define FBLUE    "\x1b[34m"
#define FMAGENTA "\x1b[35m"
#define FCYAN    "\x1b[36m"
#define FRESET   "\x1b[0m"

#define LERR(msg) FBOLD "lasm: " FRED "Error: " FRESET msg
#define LWARN(msg) FBOLD "lasm: " FYELLOW "Warning: " FRESET msg

int lasm_ret = 0;

int main(int argc, char *argv[])
{
	struct argparse *argp = argp_init();
	char **argvals = malloc(argc * sizeof *argvals);
	int nargs;
	if (!argp_parse(argp, argc, argv)) {
		fprintf(stderr, LERR("There was an error with parsing arguments"
					".\n"));
		lasm_ret = 1;
		goto cleanup;
	}
	nargs = argp_get_args(argp, argvals);

	if (nargs > 0) {
		yyin = fopen(argvals[0], "r");
		if (!yyin) {
			fprintf(stderr, LERR("File does not exist: "
					FITALIC"%s"FRESET"\n"), argvals[0]);
			lasm_ret = 1;
			goto cleanup;
		}
	}

	if (nargs > 1) {
		fprintf(stderr, LWARN("Only one FILE argument allowed. Using "
				FITALIC"%s"FRESET"\n"), argvals[0]);
	}

	llex_init();
	yylex();

cleanup:
	fclose(yyin);
	argp_free(argp);
	free(argvals);
	return lasm_ret;
}
