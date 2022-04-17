#include <stdio.h>
#include <stdlib.h>
#include <argparse/argparse.h>

#include "llex.yy.h"
#include "lasm.h"
#include "style.h"

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

	if (!llex_init()) {
		fprintf(stderr, LERR("There was a problem with initializing. "
					"Try again.\n"));
		lasm_ret = 2;
		goto cleanup;
	}
	yylex();

cleanup:
	fclose(yyin);
	argp_free(argp);
	free(argvals);
	llex_end();
	return lasm_ret;
}
