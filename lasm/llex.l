%{
#include <stdio.h>
#include "lasm.h"
#include "handlers.h"
%}

instr load|store|mod|cond|sig|nop
macro GOTO|RAW|INT|CHA|STR|GLOBAL|PSH|POP|CALL|RET|INC|DEC|TRA
comment \/\/.*$
label \$[a-zA-Z0-9_]+
number ([0-9]+)|([0-9a-fA-F]{2})|(0x[0-9a-fA-F]+)
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

%%

{instr} {if (handle_instr(yytext)) yyterminate();}
{macro} {if (handle_macro(yytext)) yyterminate();}
{comment} {if (handle_comment(yytext)) yyterminate();}
{label} {if (handle_label(yytext)) yyterminate();}
{number} {if (handle_number(yytext)) yyterminate();}
{string} {if (handle_string(yytext)) yyterminate();}
{char} {if (handle_char(yytext)) yyterminate();}
{register} {if (handle_register(yytext)) yyterminate();}
{signal} {if (handle_signal(yytext)) yyterminate();}
{implied} {if (handle_implied(yytext)) yyterminate();}
<MOD>{operation} {if (handle_operation(yytext)) yyterminate();}
{condition} {if (handle_condition(yytext)) yyterminate();}
{addrmode} {if (handle_addrmode(yytext)) yyterminate();}
{space} {}
\n {if (handle_newline(yytext)) yyterminate();}
. {if (handle_unknown(yytext)) yyterminate();}

%%

int yywrap(void) {return 1;}

void llex_init()
{}

void llex_set_state(int sc)
{
	BEGIN sc;
}
