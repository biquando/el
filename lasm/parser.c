#include <stdlib.h>
#include <string.h>
#include "parser.h"

struct parser *par_init()
{
	struct parser *par;
	par = malloc(sizeof *par);
	if (!par)
		return NULL;

	par->token_idx = 0;

	par->symbol_table = vec_init(sizeof(struct symbol_entry),
			BUF_DEFAULT_SIZE);
	if (!par->symbol_table)
		return NULL;

	par->out_buf = vec_init(sizeof(unsigned char), BUF_DEFAULT_SIZE);
	if (!par->out_buf)
		return NULL;

	return par;
}

/* Returns 1 if success, 0 otherwise */
int par_add_token(struct parser *par, enum token_type type, char *text)
{
	if (par->token_idx >= MAX_STATEMENT_SIZE)
		return 0;

	par->statement[par->token_idx].type = type;
	par->statement[par->token_idx].text = text;
	(par->token_idx)++;
	return 1;
}

/* Returns 1 if success, 0 otherwise */
int par_add_symbol(struct parser *par, char *name, int value, int size)
{
	struct symbol_entry sym;

	/* Check if symbol already defined */
	for (int i = 0; i < par->symbol_table->n_elems; i++) {
		struct symbol_entry *s = vec_get(par->symbol_table, i);
		if (strcmp(s->name, name) == 0)
			return 0;
	}

	sym.name = name;
	sym.value = value;
	sym.size = size;

	return vec_push_back(par->symbol_table, &sym);
}

/* Returns 1 if success, 0 otherwise */
int par_write_byte(struct parser *par, unsigned char b)
{
	return vec_push_back(par->out_buf, &b);
}

void par_free(struct parser *par)
{
	vec_free(par->symbol_table);
	vec_free(par->out_buf);
	free(par);
}
