%{
#include <stdio.h>
%}

string \".*\"
char \'\\?.\'
comment \/\/.*$
instr load|store|mod|cond|sig|nop
macro GOTO|RAW|INT|CHA|STR|GLOBAL|PSH|POP|CALL|RET|INC|DEC|TRA
label \$[a-zA-Z0-9_]+
labeldef ^[\ \t]*\$[a-zA-Z0-9_]+
number ([0-9]+)|([0-9a-fA-F]{2})|(0x[0-9a-fA-F]+)
register [rh](a|b|c|d|x|ar|sp|ip)
signal out|abort
implied \*{label}

%%

{string} {printf("string: %s\n", yytext);}
{instr} {printf("(%d) instruction: %s\n", yylineno, yytext);}
{macro} {printf("macro: %s\n", yytext);}
{comment} {printf("comment: %s\n", yytext);}
{label} {printf("label: %s\n", yytext);}
{labeldef} {printf("labeldef: %s\n", yytext);}
{number} {printf("number: %s\n", yytext);}
{char} {printf("char: %s\n", yytext);}
{register} {printf("register: %s\n", yytext);}
{signal} {printf("signal: %s\n", yytext);}
{implied} {printf("implied: %s\n", yytext);}
.|\n

%%

int yywrap(void) {return 1;}
