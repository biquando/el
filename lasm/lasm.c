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
	FILE *ofile = stdout;

	argp_opt(argp, "--output", 1);
	argp_alias(argp, "-o", "--output");

	if (!argp_parse(argp, argc, argv)) {
		fprintf(stderr, LERR("There was an error with parsing arguments"
					".\n"));
		lasm_ret = 1;
		goto cleanup;
	}

	/* Read ofile argument */
	nargs = argp_get_opt(argp, "--output", argvals);
	if (nargs == 1) {
		ofile = fopen(argvals[0], "wb");
		if (!ofile) {
			fprintf(stderr, LERR("Couldn't open output file: "
					FITALIC"%s"FRESET"\n"), argvals[0]);
			lasm_ret = 1;
			goto cleanup;
		}
	}
	llex_set_ofile(ofile);

	/* Read non-option arguments */
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

	/* Initialize lexer */
	if (!llex_init()) {
		fprintf(stderr, LERR("There was a problem with initializing. "
					"Try again.\n"));
		lasm_ret = 2;
		goto cleanup;
	}
	yylex();
	llex_end();
	argp_free(argp);
	free(argvals);

cleanup:
	fclose(ofile);
	fclose(yyin);
	return lasm_ret;
}
