#include <stdlib.h>
#include <string.h>
#include <vector/vector.h>
#include "constructors.h"
#include "parser.h"

struct parser *par_init()
{
	struct parser *par;
	par = malloc(sizeof *par);
	if (!par)
		return NULL;

	par->token_idx = 0;
	par->global = 0;

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
	char *text_cpy;
	if (par->token_idx >= MAX_STATEMENT_SIZE)
		return 0;

	par->statement[par->token_idx].type = type;
	text_cpy = malloc(strlen(text) + 1);
	if (!text_cpy)
		return 0;
	par->statement[par->token_idx].text = text_cpy;
	strcpy(par->statement[par->token_idx].text, text);
	(par->token_idx)++;

	return 1;
}

/* Returns 1 if success, 0 otherwise */
int par_end_statement(struct parser *par, int lineno)
{
	int success;
	if (par->token_idx == 0)
		return 1;
	if (par->statement[par->token_idx - 1].type == SPACE)
		par->token_idx--;

	switch (par->statement[0].type) {
	case LOAD:
		success = construct_load(par, lineno);
		break;
	case STORE:
		success = construct_store(par, lineno);
		break;
	case MOD:
		success = construct_mod(par, lineno);
		break;
	case COND:
		success = construct_cond(par, lineno);
		break;
	case SIG:
		success = construct_sig(par, lineno);
		break;
	case NOP:
		success = construct_nop(par, lineno);
		break;
	case NON:
		success = construct_non(par, lineno);
		break;
	case UN_REG:
		success = construct_un_reg(par, lineno);
		break;
	case UN_IMM:
		success = construct_un_imm(par, lineno);
		break;
	case UN_CHA:
		success = construct_un_cha(par, lineno);
		break;
	case UN_STR:
		success = construct_un_str(par, lineno);
		break;
	case UN_RAW:
		success = construct_un_raw(par, lineno);
		break;
	case BIN_REG_REG:
		success = construct_bin_reg_reg(par, lineno);
		break;
	case DIRECTIVE:
		success = construct_directive(par, lineno);
		break;
	default:
		success = 0;
		break;
	}

	for (int i = 0; i < par->token_idx; i++) {
		free(par->statement[i].text);
	}
	par->token_idx = 0;
	return success;
}

/* Returns NULL on fail */
struct symbol_entry *par_get_sym(struct parser *par, char *token)
{
	struct symbol_entry *sym = NULL;
	for (int s = 0; s < par->symbol_table->n_elems; s++) {
		if (strncmp(
					((struct symbol_entry *)
					 vec_get(par->symbol_table, s))->name,
					token,
					strlen(token)
			   ) == 0) {
			sym = vec_get(par->symbol_table, s);
			break;
		}
	}
	return sym;
}

/* Returns 1 if success, 0 otherwise */
int par_set_global(struct parser *par, char *token)
{
	if (token[0] == '$') {
		struct symbol_entry *sym = NULL;
		sym = par_get_sym(par, token);
		if (!sym) {
			return 0;
		}

		par->global = sym->value;
		return 1;
	}

	par->global = strtol(token, NULL, 0);
	return 1;
}

/* Returns 1 if success, 0 otherwise */
int par_add_symbol(struct parser *par, char *name, int value)
{
	struct symbol_entry sym;
	char *name_cpy;

	/* Check if symbol already defined */
	for (int i = 0; i < par->symbol_table->n_elems; i++) {
		struct symbol_entry *s = vec_get(par->symbol_table, i);
		if (strcmp(s->name, name) == 0)
			return 0;
	}

	name_cpy = malloc(strlen(name) + 1);
	if (!name_cpy)
		return 0;
	strcpy(name_cpy, name);

	sym.name = name_cpy;
	sym.value = value;
	return vec_push_back(par->symbol_table, &sym);
}

/* Returns 1 if success, 0 otherwise */
int par_add_ref(struct parser *par, char *name, int size, int lineno)
{
	struct ref_entry ref;
	char *name_cpy = malloc(strlen(name) + 1);
	if (!name_cpy)
		return 0;
	strcpy(name_cpy, name);

	ref.location = par->out_buf->n_elems;
	ref.name = name_cpy;
	ref.size = size;
	ref.lineno = lineno;
	return vec_push_back(par->ref_table, &ref);
}

/* Returns 1 if success, 0 otherwise */
int par_write_byte(struct parser *par, unsigned char b)
{
	return vec_push_back(par->out_buf, &b);
}

/* Returns 1 if success, 0 otherwise */
int par_resolve_refs(struct parser *par, struct ref_entry *err_ref)
{
	for (int i = 0; i < par->ref_table->n_elems; i++) {
		struct ref_entry *r = vec_get(par->ref_table, i);

		struct symbol_entry *sym = NULL;
		int symval;
		int offset;
		/* Find symbol with matching name */
		sym = par_get_sym(par, r->name);
		if (!sym) {
			*err_ref = *r;
			return 0;
		}
		symval = sym->value;

		/* Offset symval by buffer placeholder value */
		offset = *((int *)vec_get(par->out_buf, r->location));
		//fprintf(stderr, "	offset:%x\n", offset);
		symval += offset;

		for (int b = 0; b < r->size; b++) {
			unsigned char *p = vec_get(par->out_buf,
					r->location + b);
			*p = (unsigned char) (symval >> (8 * b));
		}
	}

	return 1;
}

void par_free(struct parser *par)
{
	par_end_statement(par, 0);
	/* Free symbol strings */
	for (int i = 0; i < par->symbol_table->n_elems; i++) {
		struct symbol_entry *sym = vec_get(par->symbol_table, i);
		free(sym->name);
	}
	/* Free refs strings */
	for (int i = 0; i < par->ref_table->n_elems; i++) {
		struct ref_entry *ref = vec_get(par->ref_table, i);
		free(ref->name);
	}
	vec_free(par->symbol_table);
	vec_free(par->ref_table);
	vec_free(par->out_buf);
	free(par);
}
