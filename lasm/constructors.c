#include <stdlib.h>
#include <string.h>
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
		result |= 1 << 3;

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
static int _decode_imm(struct parser *par, char *token, int size, int lineno)
{
	if (token[0] == '$') {  /* label ref */
		par_add_ref(par, token, size, lineno);
		return 0;
	}

	return strtol(token, NULL, 0);
}

static int _decode_un_op(char *token)
{
	switch (token[0]) {
	case '+': return 0x2;
	case '-': return 0x3;
	case '~': return 0x7;
	}
	return -1;
}

static int _decode_bin_op(char *token)
{
	switch (token[0]) {
	case '+': return 0x0;
	case '-': return 0x1;
	case '&': return 0x4;
	case '|': return 0x5;
	case '^': return 0x6;
	case '>': return 0x8;
	case '<': return 0x9;
	case '/': return 0xb;
	case '?': return 0xc;
	}
	return -1;
}

static int _decode_condition(char *token)
{
	if (token[0] == 'n') {
		switch (token[1]) {
		case 'o': return 1;
		case 'b': case 'c': return 3;
		case 'e': case 'z': return 5;
		case 'a': return 6;
		case 's': return 9;
		case 'l': return 13;
		case 'g': return 14;
		}
		return -1;
	}
	switch (token[0]) {
	case 'o': return 0;
	case 'b': case 'c': return 2;
	case 'e': case 'z': return 4;
	case 'a': return 7;
	case 's': return 8;
	case 'l': return 12;
	case 'g': return 15;
	}
	return -1;
}

static int _decode_signal(char *token)
{
	if (strcmp(token, "abort") == 0)
		return 0;
	else if (strcmp(token, "out") == 0)
		return 1;
	return -1;
}

static int _decode_cha(char *token)
{
	if (token[1] != '\\')
		return token[1];

	switch (token[2]) {
	case 'a':
		return '\a';
	case 'b':
		return '\b';
	case 'f':
		return '\f';
	case 'n':
		return '\n';
	case 'r':
		return '\r';
	case 't':
		return '\t';
	case 'v':
		return '\v';
	case '\\':
		return '\\';
	case '\'':
		return '\'';
	case '"':
		return '\"';
	case '?':
		return '\?';
	case '0':
		return '\0';
	default:
		return -1;
	}
}

/* bytes should be big-endian (e.g. 0x123456 -> 0x12, 0x34, 0x56) */
static int _write_triplet(struct parser *par, int bytes) {
	int failed = 0;
	failed |= !par_write_byte(par, bytes >> 16);
	failed |= !par_write_byte(par, bytes >> 8);
	failed |= !par_write_byte(par, bytes);
	return !failed;
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
		tmp = _decode_imm(par, par->statement[6].text, 2, lineno);
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
	if (!_match_structure(par, t, 5))
		return 0;

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
	enum token_type t1[] = {MOD, SPACE, REG, SPACE, UN_OP};
	enum token_type t2[] = {MOD, SPACE, REG, SPACE, BIN_OP, SPACE, REG};
	int tmp;
	int failed = 0;
	if (_match_structure(par, t1, 5)) {
		tmp = _decode_reg(par->statement[2].text);
		failed |= !par_write_byte(par, 0x80 | tmp);

		tmp = _decode_un_op(par->statement[4].text);
		failed |= !par_write_byte(par, tmp);

		failed |= !par_write_byte(par, 0);

	} else if (_match_structure(par, t2, 7)) {
		tmp = _decode_reg(par->statement[2].text);
		failed |= !par_write_byte(par, 0x80 | tmp);

		tmp = _decode_bin_op(par->statement[4].text);
		failed |= !par_write_byte(par, tmp);

		tmp = _decode_reg(par->statement[6].text);
		failed |= !par_write_byte(par, tmp);
	} else {
		failed = 1;
	}

	return !failed;
}

int construct_cond(struct parser *par, int lineno)
{
	enum token_type t[] = {COND, SPACE, CONDITION};
	int tmp;
	int failed = 0;
	if (!_match_structure(par, t, 3))
		return 0;

	failed |= !par_write_byte(par, 0xc0);
	tmp = _decode_condition(par->statement[2].text);
	failed |= !par_write_byte(par, tmp);
	failed |= !par_write_byte(par, 0);
	return !failed;
}

int construct_sig(struct parser *par, int lineno)
{
	enum token_type t1[] = {SIG, SPACE, UNVAL_SIG};
	enum token_type t2[] = {SIG, SPACE, VAL_SIG, SPACE, IMM};
	int tmp;
	int failed = 0;
	if (_match_structure(par, t1, 3)) {
		tmp = _decode_signal(par->statement[2].text);
		failed |= !par_write_byte(par, tmp);
		failed |= !par_write_byte(par, 0);
		failed |= !par_write_byte(par, 0);

	} else if (_match_structure(par, t2, 5)) {
		tmp = _decode_signal(par->statement[2].text);
		failed |= !par_write_byte(par, tmp);

		tmp = _decode_imm(par, par->statement[4].text, 2, lineno);
		failed |= !par_write_byte(par, tmp);
		failed |= !par_write_byte(par, tmp >> 8);

	} else {
		failed = 1;
	}

	return !failed;
}

int construct_nop(struct parser *par, int lineno)
{
	enum token_type t1[] = {NOP};
	enum token_type t2[] = {NOP, SPACE, IMM};
	int tmp;
	int failed = 0;
	if (_match_structure(par, t1, 1)) {
		failed |= !_write_triplet(par, 0xff0000);

	} else if (_match_structure(par, t2, 3)) {
		failed |= !par_write_byte(par, 0xff);

		tmp = _decode_imm(par, par->statement[2].text, 2, lineno);
		failed |= !par_write_byte(par, tmp);
		failed |= !par_write_byte(par, tmp >> 8);
	} else {
		failed = 1;
	}

	return 1;
}

int construct_non(struct parser *par, int lineno)
{
	enum token_type t[] = {NON};
	int failed = 0;
	if (!_match_structure(par, t, 1))
		return 0;

	if (strcmp(par->statement[0].text, "RET") == 0) {
		failed |= !_write_triplet(par, 0x150000);  /* load # rar 0  */
		failed |= !_write_triplet(par, 0x850506);  /* mod rar | rsp */
		failed |= !_write_triplet(par, 0x860200);  /* mod rsp ++    */
		failed |= !_write_triplet(par, 0x860200);  /* mod rsp ++    */
		failed |= !_write_triplet(par, 0x270000);  /* load * rip    */
	} else {
		failed = 1;
	}

	return !failed;
}

int construct_un_reg(struct parser *par, int lineno)
{
	enum token_type t[] = {UN_REG, SPACE, REG};
	int tmp;
	int failed = 0;
	const char *macro;
	if (!_match_structure(par, t, 3))
		return 0;

	macro = par->statement[0].text;
	tmp = _decode_reg(par->statement[2].text);
	if (strcmp(macro, "PSH") == 0) {
		failed |= !_write_triplet(par, 0x860300);  /* mod rsp --    */
		if (!(tmp & 0x8))
			failed |= !_write_triplet(par, 0x860300);
		failed |= !_write_triplet(par, 0x150000);  /* load # rar 0  */
		failed |= !_write_triplet(par, 0x850506);  /* mod rar | rsp */

		failed |= !par_write_byte(par, 0x60 | tmp);/* store * REG   */
		failed |= !par_write_byte(par, 0);
		failed |= !par_write_byte(par, 0);
	} else if (strcmp(macro, "POP") == 0) {
		failed |= !_write_triplet(par, 0x150000);  /* load # rar 0  */
		failed |= !_write_triplet(par, 0x850506);  /* mod rar | rsp */
		failed |= !_write_triplet(par, 0x860200);  /* mod rsp ++    */
		if (!(tmp & 0x8))
			failed |= !_write_triplet(par, 0x860200);

		failed |= !par_write_byte(par, 0x20 | tmp);/* load * REG    */
		failed |= !par_write_byte(par, 0);
		failed |= !par_write_byte(par, 0);
	} else if (strcmp(macro, "INC") == 0) {
		tmp = (0x80 | tmp) << 16;  /* mod REG */
		failed |= !_write_triplet(par, tmp | 0x0200);  /* ++ */
		failed |= !_write_triplet(par, tmp | 0x0200);  /* ++ */
	} else if (strcmp(macro, "DEC") == 0) {
		tmp = (0x80 | tmp) << 16;  /* mod REG */
		failed |= !_write_triplet(par, tmp | 0x0300);  /* -- */
		failed |= !_write_triplet(par, tmp | 0x0300);  /* -- */
	} else {
		failed = 1;
	}

	return !failed;
}

int construct_un_imm(struct parser *par, int lineno)
{
	enum token_type t[] = {UN_IMM, SPACE, IMM};
	int tmp;
	int failed = 0;
	const char *macro;

	if (!_match_structure(par, t, 3))
		return 0;

	macro = par->statement[0].text;
	if (strcmp(macro, "GOTO") == 0) {
		failed |= !par_write_byte(par, 0x17);

		tmp = _decode_imm(par, par->statement[2].text, 2, lineno) - 3;
		failed |= !par_write_byte(par, tmp);
		failed |= !par_write_byte(par, tmp >> 8);
	} else if (strcmp(macro, "CALL") == 0) {
		failed |= !_write_triplet(par, 0x141200);  /* load # rx 18  */
		failed |= !_write_triplet(par, 0x840007);  /* mod rx + rip  */

		/* PSH rx */
		failed |= !_write_triplet(par, 0x860300);  /* mod rsp --    */
		failed |= !_write_triplet(par, 0x860300);  /* mod rsp --    */
		failed |= !_write_triplet(par, 0x150000);  /* load # rar 0  */
		failed |= !_write_triplet(par, 0x850506);  /* mod rar | rsp */
		failed |= !_write_triplet(par, 0x640000);  /* store * rx    */

		/* GOTO IMM */
		failed |= !par_write_byte(par, 0x17);
		tmp = _decode_imm(par, par->statement[2].text, 2, lineno) - 3;
		failed |= !par_write_byte(par, tmp);
		failed |= !par_write_byte(par, tmp >> 8);
	} else if (strcmp(macro, "INT") == 0) {
		tmp = _decode_imm(par, par->statement[2].text, 2, lineno);
		failed |= !par_write_byte(par, tmp);
		failed |= !par_write_byte(par, tmp >> 8);
	} else {
		failed = 1;
	}

	return !failed;
}

int construct_un_cha(struct parser *par, int lineno)
{
	enum token_type t1[] = {UN_CHA, SPACE, CHA};
	enum token_type t2[] = {UN_CHA, SPACE, IMM};
	int tmp;
	int failed = 0;
	const char *chastr;

	if (_match_structure(par, t1, 3)) {
		tmp = _decode_cha(par->statement[2].text);
		if (tmp == -1)
			failed = 1;
		failed |= !par_write_byte(par, tmp);
	} else if (_match_structure(par, t2, 3)) {
		tmp = _decode_imm(par, par->statement[2].text, 1, lineno);
		failed |= !par_write_byte(par, tmp);
	}

	return !failed;
}

int construct_un_str(struct parser *par, int lineno)
{
	enum token_type t[] = {UN_STR, SPACE, STR};
	int tmp;
	int failed = 0;
	char *str;
	int len;

	if (!_match_structure(par, t, 3))
		return 0;

	len = strlen(par->statement[2].text);
	str = malloc(len + 1);
	strncpy(str, par->statement[2].text, len);

	/* Cut off double quotes */
	str[len - 1] = '\0';
	str++;

	while (*str && !failed) {
		tmp = _decode_cha(str - 1);
		if (tmp == -1)
			failed = 1;
		failed |= !par_write_byte(par, tmp);
		if (*(str++) == '\\')
			str++;
	}
	failed |= !par_write_byte(par, '\0');
	return !failed;
}

int construct_un_raw(struct parser *par, int lineno)
{
	enum token_type t[] = {UN_RAW, SPACE, RAW};
	int tmp;
	int failed = 0;
	const char *rawstr;

	if (!_match_structure(par, t, 3))
		return 0;

	rawstr = par->statement[2].text;
	while (!failed) {
		tmp = strtol(rawstr, (char **) &rawstr, 16);
		failed |= !par_write_byte(par, tmp);
		if (!*rawstr)
			break;
		rawstr++;
	}

	return !failed;
}

int construct_bin_reg_reg(struct parser *par, int lineno)
{
	enum token_type t[] = {BIN_REG_REG, SPACE, REG, SPACE, REG};
	int tmp;
	int failed = 0;
	const char *macro;

	if (!_match_structure(par, t, 5))
		return 0;

	macro = par->statement[0].text;
	if (strcmp(macro, "TRA") == 0) {
		/* load # R1 0 */
		tmp = _decode_reg(par->statement[2].text);
		tmp = (0x10 | tmp) << 16;
		failed |= !_write_triplet(par, tmp | 0x0000);

		/* mod R1 + R2 */
		tmp = 0x80;
		tmp |= _decode_reg(par->statement[2].text);
		tmp = tmp << 16;
		tmp |= _decode_reg(par->statement[4].text);
		failed |= !_write_triplet(par, tmp);
	} else {
		failed = 1;
	}

	return !failed;
}

int construct_directive(struct parser *par, int lineno)
{
	enum token_type t1[] = {DIRECTIVE, SPACE, IMM};
	enum token_type t2[] = {DIRECTIVE, SPACE, IMM, SPACE, IMM};
	int tmp;
	int failed = 0;
	const char *dir;
	struct symbol_entry *sym;

	dir = par->statement[0].text;
	if (_match_structure(par, t1, 3) && strcmp(dir, ".global") == 0) {
		/* nop IMM */
		failed |= !par_write_byte(par, 0xff);
		tmp = _decode_imm(par, par->statement[2].text, 2, lineno);
		failed |= !par_write_byte(par, tmp);
		failed |= !par_write_byte(par, tmp >> 8);

		/* set par->global */
		failed |= !par_set_global(par, par->statement[2].text);
	} else if (_match_structure(par, t2, 5) && strcmp(dir, ".const") == 0) {
		if (par->statement[2].text[0] != '$')
			return 0;

		if (par->statement[4].text[0] == '$') {
			sym = par_get_sym(par, par->statement[4].text);
			if (!sym)
				return 0;
			tmp = sym->value;
		} else {
			tmp = strtol(par->statement[4].text, NULL, 0);
		}
		failed |= !par_add_symbol(par, par->statement[2].text, tmp);
	} else {
		failed = 1;
	}

	return !failed;
}
