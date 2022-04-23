%{
#include <stdio.h>
#include "handlers.h"
#include "lasm.h"
#include "parser.h"
#include "style.h"

static struct parser *par;
static FILE *ofile;
%}

instr load|store|mod|cond|sig|nop
macro GOTO|RAW|INT|CHA|STR|PSH|POP|CALL|RET|INC|DEC|TRA
directive .(global|const)
comment \/\/.*$
label \$[a-zA-Z0-9_]+
number ([0-9]+)|(0x[0-9a-fA-F]+)
raw ([0-9a-fA-F]{2}[ ])*([0-9a-fA-F]{2})
string \"([^\\\"]|\\.)*\"
char \'([^\\\']|\\.)\'
register [rh](a|b|c|d|x|ar|sp|ip)
signal abort|out
implied \*{label}
operation \+|-|\+\+|--|&|\||\^|~|\<\<|\>\>|\/\>|\?
condition o|no|b|c|nb|nc|e|z|ne|nz|na|a|s|ns|l|nl|ng|g
addrmode #|\*|\+
space [ \t]+

%s MODSC
%s RAWSC

%%

{instr} {if (!handle_instr(yytext, par)) yyterminate();}
{macro} {if (!handle_macro(yytext, par)) yyterminate();}
{directive} {if (!handle_directive(yytext, par)) yyterminate();}
{comment} {if (!handle_comment(yytext, par)) yyterminate();}
{label} {if (!handle_label(yytext, par)) yyterminate();}
<RAWSC>{raw} {if (!handle_raw(yytext, par)) yyterminate();}
{number} {if (!handle_number(yytext, par)) yyterminate();}
{string} {if (!handle_string(yytext, par)) yyterminate();}
{char} {if (!handle_char(yytext, par)) yyterminate();}
{register} {if (!handle_register(yytext, par)) yyterminate();}
{signal} {if (!handle_signal(yytext, par)) yyterminate();}
{implied} {if (!handle_implied(yytext, par)) yyterminate();}
<MODSC>{operation} {if (!handle_operation(yytext, par)) yyterminate();}
{condition} {if (!handle_condition(yytext, par)) yyterminate();}
{addrmode} {if (!handle_addrmode(yytext, par)) yyterminate();}
{space} {if (!handle_space(yytext, par)) yyterminate();}
\n {if (!handle_newline(yytext, par)) yyterminate();}
. {if (!handle_unknown(yytext, par)) yyterminate();}

%%

int yywrap(void) {
	if (++currfile >= numfiles) {
		return 1;
	}

	yyin = fopen(ifiles[currfile], "r");
	if (!yyin) {
		fprintf(stderr, LERR("File does not exist: "
				FITALIC"%s"FRESET"\n"), ifiles[currfile]);
		lasm_ret = 1;
		return 1;
	}
	return 0;
}

int llex_init()
{
	par = par_init();
	if (!par)
		return 0;
	return 1;
}

void llex_set_state(int sc)
{
	BEGIN sc;
}

void llex_set_ofile(FILE *fd)
{
	ofile = fd;
}

void llex_end()
{
	struct ref_entry err_ref;
	if (!lasm_ret && !par_resolve_refs(par, &err_ref)) {
		fprintf(stderr, LERRL("Undefined symbol `%s`.\n"),
				err_ref.lineno, err_ref.name);
		lasm_ret = 0x2D;
	}

	if (!lasm_ret || lasm_ret == 0x2D) {
		fwrite(par->out_buf->base, 1, par->out_buf->n_elems, ofile);
	}

	par_free(par);
}
