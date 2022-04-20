#include <stdlib.h>
#include "constructors.h"
#include "parser.h"

static int _match_structure(struct parser *par,
		enum token_type types[], int ntypes)
{
	if (par->token_idx != ntypes)
		return 0;

	for (int i = 0; i < ntypes; i++) {
		if (par->statement[i].type != types[i])
			return 0;
	}
	return 1;
}

static int _decode_addr_mode(char *token)
{
	switch (token[0]) {
	case '#':
		return 1;
	case '*':
		return 2;
	case '+':
		return 3;
	default:
		return -1;
	}
}

static int _decode_reg(char *token)
{
	int result = 0;
	if (token[0] == 'h')
		result |= 1 << 4;

	switch (token[1]) {
	case 'a':
		if (token[2] == '\0')
			return result | 0;  /* ra */
		if (token[2] == 'r')
			return result | 5;  /* rar */
		break;
	case 'b':
		return result | 1;  /* rb */
	case 'c':
		return result | 2;  /* rc */
	case 'd':
		return result | 3;  /* rd */
	case 'x':
		return result | 4;  /* rx */
	case 's':
		if (token[2] == 'p')
			return result | 6;  /* rsp */
		break;
	case 'i':
		if (token[2] == 'p')
			return result | 7;  /* rip */
		break;
	}
	return -1;
}

/* Needs the parser to create label references */
static int _decode_imm(struct parser *par, char *token, int lineno)
{
	if (token[0] == '$') {  /* label ref */
		par_add_ref(par, token, 2, lineno);
		return 0;
	}

	return strtol(token, NULL, 0);
}

int construct_load(struct parser *par, int lineno)
{
	enum token_type t1[] = {LOAD, SPACE, IMM_ADDR, SPACE, REG, SPACE, IMM};
	enum token_type t2[] = {LOAD, SPACE, ABS_ADDR, SPACE, REG};
	int tmp;
	int failed = 0;
	if (_match_structure(par, t1, 7)) {
		/* opcode */
		tmp = _decode_reg(par->statement[4].text);
		failed |= !par_write_byte(par, 0x10 | tmp);

		/* operand */
		tmp = _decode_imm(par, par->statement[6].text, lineno);
		failed |= !par_write_byte(par, tmp);
		failed |= !par_write_byte(par, tmp >> 8);
	} else if (_match_structure(par, t2, 5)) {
		/* opcode */
		tmp = _decode_reg(par->statement[4].text);
		tmp |= 0x20 | (0x10 * (par->statement[2].text[0] == '+'));
		failed |= !par_write_byte(par, tmp);

		/* operand */
		failed |= !par_write_byte(par, 0);
		failed |= !par_write_byte(par, 0);
	} else {
		failed = 1;
	}

	return !failed;
}

int construct_store(struct parser *par, int lineno)
{
	enum token_type t[] = {STORE, SPACE, ABS_ADDR, SPACE, REG};
	int tmp;
	int failed = 0;
	if (!_match_structure(par, t, 5)) {
		return 0;
	}

	/* opcode */
	tmp = _decode_reg(par->statement[4].text);
	tmp |= 0x60 | (0x10 * (par->statement[2].text[0] == '+'));
	failed |= !par_write_byte(par, tmp);

	/* operand */
	failed |= !par_write_byte(par, 0);
	failed |= !par_write_byte(par, 0);
	return !failed;
}

int construct_mod(struct parser *par, int lineno)
{
	return 1;
}

int construct_cond(struct parser *par, int lineno)
{
	return 1;
}

int construct_sig(struct parser *par, int lineno)
{
	return 1;
}

int construct_nop(struct parser *par, int lineno)
{
	return 1;
}

int construct_non(struct parser *par, int lineno)
{
	return 1;
}

int construct_un_reg(struct parser *par, int lineno)
{
	return 1;
}

int construct_un_imm(struct parser *par, int lineno)
{
	return 1;
}

int construct_un_cha(struct parser *par, int lineno)
{
	return 1;
}

int construct_un_str(struct parser *par, int lineno)
{
	return 1;
}

int construct_un_raw(struct parser *par, int lineno)
{
	return 1;
}

int construct_bin_reg_reg(struct parser *par, int lineno)
{
	return 1;
}
