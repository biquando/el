#include <stdlib.h>
#include <string.h>
#include <vector/vector.h>
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
	par->ref_table = vec_init(sizeof(struct ref_entry),
			BUF_DEFAULT_SIZE);
	par->out_buf = vec_init(sizeof(unsigned char),
			BUF_DEFAULT_SIZE);

	if (!par->symbol_table || !par->ref_table || !par->out_buf)
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
int par_add_symbol(struct parser *par, char *name, int value)
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

	return vec_push_back(par->symbol_table, &sym);
}

/* Returns 1 if success, 0 otherwise */
int par_add_ref(struct parser *par, char *name, int size)
{
	struct ref_entry ref;
	ref.sym_idx = -1;

	/* Look for symbol of the same name */
	for (int i = 0; i < par->symbol_table->n_elems; i++) {
		struct symbol_entry *s = vec_get(par->symbol_table, i);
		if (strcmp(s->name, name) == 0) {
			ref.sym_idx = i;
			break;
		}
	}
	if (ref.sym_idx == -1)
		return 0;

	ref.location = (int) par->out_buf->n_elems;
	ref.size = size;

	return vec_push_back(par->ref_table, &ref);
}

/* Returns 1 if success, 0 otherwise */
int par_write_byte(struct parser *par, unsigned char b)
{
	return vec_push_back(par->out_buf, &b);
}

void par_resolve_refs(struct parser *par)
{
	for (int i = 0; i < par->ref_table->n_elems; i++) {
		struct ref_entry *r = vec_get(par->ref_table, i);
		struct symbol_entry *s = vec_get(par->symbol_table, r->sym_idx);

		for (int b = 0; b < r->size; b++) {
			unsigned char *p = vec_get(par->out_buf,
					r->location + b);
			*p = (unsigned char) (s->value >> (8 * b));
		}
	}
}

void par_free(struct parser *par)
{
	vec_free(par->symbol_table);
	vec_free(par->ref_table);
	vec_free(par->out_buf);
	free(par);
}
