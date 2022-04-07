%{
#include <stdio.h>
#include "lasm.h"
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
addrmode #|\*|\+
space [ \t]

%s LINE_START

%%

<LINE_START>{instr} {handle_instr(yytext);}
<LINE_START>{macro} {handle_macro(yytext);}
{comment} {handle_comment(yytext);}
{label} {handle_label(yytext);}
<LINE_START>{label} {handle_labeldef(yytext);}
{number} {handle_number(yytext);}
{string} {handle_string(yytext);}
{char} {handle_char(yytext);}
{register} {handle_register(yytext);}
{signal} {handle_signal(yytext);}
{implied} {handle_implied(yytext);}
{addrmode} {handle_addrmode(yytext);}
{space} {}
\n {handle_newline(yytext);}
. {handle_unknown(yytext); yyterminate();}

%%

int yywrap(void) {return 1;}

void init_llex()
{
	BEGIN LINE_START;
}

void set_state(int sc)
{
	BEGIN sc;
}
