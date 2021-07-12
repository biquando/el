#include <stdlib.h>
#include <string.h>
#include <vector/vector.h>
#include <argparse/argparse.h>

struct argparse *argp_init()
{
	const int DEFAULT_VECTOR_ALLOC = 5;
	struct argparse *ap = malloc(sizeof *ap);

	ap->args       = vec_init(sizeof(char*),  DEFAULT_VECTOR_ALLOC);
	ap->opts       = vec_init(sizeof(char*),  DEFAULT_VECTOR_ALLOC);
	ap->vals       = vec_init(sizeof(char**), DEFAULT_VECTOR_ALLOC);
	ap->exp_nvals  = vec_init(sizeof(int),    DEFAULT_VECTOR_ALLOC);
	ap->nvals      = vec_init(sizeof(int),    DEFAULT_VECTOR_ALLOC);
	ap->alias_labs = vec_init(sizeof(char*),  DEFAULT_VECTOR_ALLOC);
	ap->alias_opts = vec_init(sizeof(char*),  DEFAULT_VECTOR_ALLOC);

	return ap;
}

int _argp_opt_exists(struct argparse *ap, const char *opt)
{
	int i;

	if (!ap || !opt) {
		return 0;
	}

	for (i = 0; i < ap->opts->n_elems; i++) {
		if (!strcmp(opt, ((char**) ap->opts->base)[i])) {
			return 1;
		}
	}
	return 0;
}

int argp_opt(struct argparse *ap, const char *opt, int exp_nval)
{
	if (!ap || _argp_opt_exists(ap, opt)) {
		return 0;
	}

	vec_push_back(ap->opts, &opt);
	vec_push_back(ap->exp_nvals, &exp_nval);
	return 1;
}

int argp_alias(struct argparse *ap, const char *lab, const char *opt)
{
	if (!ap || !lab || !opt || _argp_opt_exists(ap, lab)) {
		return 0;
	}

	vec_push_back(ap->alias_labs, &lab);
	vec_push_back(ap->alias_opts, &opt);
	return 1;
}

int argp_get_opt(struct argparse *ap, const char *opt, char **val)
{
	int i, j, nval;
	char **valptr = NULL;
	int *nvalptr = NULL;

	if (!ap || !opt) {
		return 0;
	}

	/* Check opts */
	for (i = 0; i < ap->opts->n_elems; i++) {
		if (!strcmp(opt, *((char**) vec_get(ap->opts, i)))) {
			nvalptr = (int*) vec_get(ap->nvals, i);
			nval = nvalptr ? *nvalptr : 0;
			valptr = *((char***) vec_get(ap->vals, i));
			for (j = 0; j < nval && valptr && val; j++) {
				val[j] = valptr[j];
			}
			return nval;
		}
	}

	return 0;
}

int argp_get_args(struct argparse *ap, char **args)
{
	int i;

	if (!ap || !args) {
		return 0;
	}

	for (i = 0; i < ap->args->n_elems; i++) {
		args[i] = ((char**) ap->args->base)[i];
	}
	return ap->args->n_elems;
}

int argp_parse(struct argparse *ap, int argc, char **argv)
{
	int i, j;

	if (!ap || !argv) {
		return 0;
	}

	vec_resize(ap->vals, ap->opts->n_elems);
	ap->vals->n_elems = ap->opts->n_elems;
	for (i = j = 0; i < ap->opts->n_elems; i++) {
		vec_push_back(ap->nvals, &j);
	}

	for (i = 1; i < argc; i++) {
		char *arg = calloc(strlen(argv[i]) + 1, sizeof *arg);
		int found_opt = 0;
		strcpy(arg, argv[i]);

		/* Look for alias */
		for (j = 0; j < ap->alias_labs->n_elems; j++) {
			if (!strcmp(arg, *((char**) vec_get(ap->alias_labs, j)))) {
				int new_len = strlen(*((char**) vec_get(ap->alias_opts, j)));
				arg = realloc(arg, new_len + 1);
				strcpy(arg, *((char**) vec_get(ap->alias_opts, j)));
				break;
			}
		}

		/* Look for opt */
		for (j = 0; j < ap->opts->n_elems; j++) {
			/* if argument i matches option j */
			if (!strcmp(arg, *((char**) vec_get(ap->opts, j)))) {
				int exp_nval = *((int*) vec_get(ap->exp_nvals, j));
				int k;
				if (exp_nval == 0) {
					((char***) ap->vals->base)[j] = &argv[i];
					((int*) ap->nvals->base)[j] = 1;
				} else {
					k = i + 1;
					if (k == argc || _argp_opt_exists(ap, argv[k])) {
						return 0;
					}
					((char***) ap->vals->base)[j] = (char**) &argv[k];
					while (k < argc && k - i != exp_nval &&
							!_argp_opt_exists(ap, argv[k])) {
						k++;
					}
					if (exp_nval < 0) {
						k--;
					}
					((int*) ap->nvals->base)[j] = k - i;
					i = k;
				}
				found_opt = 1;
				break;
			}
		}

		/* Add to args if arg didn't match any opts */
		if (!found_opt) {
			vec_push_back(ap->args, &argv[i]);
        }

		free(arg);
	}

	return 1;
}

void argp_clear(struct argparse *ap)
{
	vec_resize(ap->args, 0);
	vec_resize(ap->vals, 0);
	vec_resize(ap->nvals, 0);
}

void argp_free(struct argparse *ap)
{
	if (!ap) {
		return;
	}

	vec_free(ap->args);
	vec_free(ap->opts);
	vec_free(ap->vals);
	vec_free(ap->exp_nvals);
	vec_free(ap->nvals);
	vec_free(ap->alias_labs);
	vec_free(ap->alias_opts);
	free(ap);
}
