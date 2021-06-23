#ifndef ARGPARSE_H
#define ARGPARSE_H

#include <vector/vector.h>

struct argparse {
	struct vector *args;        /* (char*)  optionless arguments */
	struct vector *opts;        /* (char*)  names of options */
	struct vector *vals;        /* (char**) pointers to option values */
	struct vector *exp_nvals;   /* (int)    expected number of option values */
	struct vector *nvals;       /* (int)    actual number of option values */
	struct vector *alias_labs;  /* (char*)  alias labels */
	struct vector *alias_opts;  /* (char*)  corresponding options for aliases */
};

struct argparse *argp_init();
int argp_opt(struct argparse *ap, const char *opt, int exp_nval);
int argp_alias(struct argparse *ap, const char *lab, const char *opt);
int argp_get_opt(struct argparse *ap, const char *opt, char **val);
int argp_get_args(struct argparse *ap, char **args);
int argp_parse(struct argparse *ap, int argc, char **argv);
void argp_clear(struct argparse *ap);
void argp_free(struct argparse *ap);

#endif
