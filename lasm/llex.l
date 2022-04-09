%{
#include <stdio.h>
#include "lasm.h"
#include "handlers.h"
#include "parser.h"

static struct parser *par;
%}

instr load|store|mod|cond|sig|nop
macro GOTO|RAW|INT|CHA|STR|GLOBAL|PSH|POP|CALL|RET|INC|DEC|TRA
comment \/\/.*$
label \$[a-zA-Z0-9_]+
number ([0-9]+)|(0x[0-9a-fA-F]+)
raw ([0-9a-fA-F]{2}[ ])*([0-9a-fA-F]{2})
string \"([^\\\"]|\\.)*\"
char \'([^\\\']|\\.)\'
register [rh](a|b|c|d|x|ar|sp|ip)
signal out|abort
implied \*{label}
operation \+|-|\+\+|--|&|\||\^|~|\<\<|\>\>|\/\>|\?
condition o|no|b|c|nb|nc|e|z|ne|nz|na|a|s|ns|l|nl|ng|g
addrmode #|\*|\+
space [ \t]+

%s MOD
%s RAW

%%

{instr} {if (handle_instr(yytext, par)) yyterminate();}
{macro} {if (handle_macro(yytext, par)) yyterminate();}
{comment} {if (handle_comment(yytext, par)) yyterminate();}
{label} {if (handle_label(yytext, par)) yyterminate();}
<RAW>{raw} {if (handle_raw(yytext, par)) yyterminate();}
{number} {if (handle_number(yytext, par)) yyterminate();}
{string} {if (handle_string(yytext, par)) yyterminate();}
{char} {if (handle_char(yytext, par)) yyterminate();}
{register} {if (handle_register(yytext, par)) yyterminate();}
{signal} {if (handle_signal(yytext, par)) yyterminate();}
{implied} {if (handle_implied(yytext, par)) yyterminate();}
<MOD>{operation} {if (handle_operation(yytext, par)) yyterminate();}
{condition} {if (handle_condition(yytext, par)) yyterminate();}
{addrmode} {if (handle_addrmode(yytext, par)) yyterminate();}
{space} {}
\n {if (handle_newline(yytext, par)) yyterminate();}
. {if (handle_unknown(yytext, par)) yyterminate();}

%%

int yywrap(void) {return 1;}

void llex_init()
{
	par = par_init();
}

void llex_set_state(int sc)
{
	BEGIN sc;
}

void llex_cleanup()
{
	par_free(par);
}
