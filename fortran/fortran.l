%{
#include <stdlib.h> /* for atoi() */
#include <string.h> /* for strcpy() and exit() */
#include <stdio.h> /* for printf() */
#include <string>
#include "fortran.h"
%}

IDENT [A-Za-z][A-Za-z0-9]*
NUMERAL ([1-9][0-9]*)|0

%option noyywrap

%%

"print" 	{ return PRINT; }
"if"		{ return IF; }
"then"		{ return THEN; }
"else"		{ return ELSE; }
"endif"		{ return ENDIF; }
"do"		{ return DO; }
"end do"	{ return ENDDO; }
"exit"		{ return EXIT; }
"program"	{ return PROGRAM; }
"end program"	{ return ENDPROGRAM; }
"integer"	{ return INTEGER; }
"label"     { return LABEL; }
"while"		{ return WHILE; }
{NUMERAL}	{ yylval.int_val = atoi(yytext); return NUMERAL; }
{IDENT}		{ strcpy(yylval.ident_name, yytext); return IDENT; }
"=="|".EQ."		{ return EQ; }
"<="|".LE."		{ return LE; }
">="|".GE."		{ return GE; }
"/="|".NE."		{ return NE; }
"<"|".LT."		{ return '<'; }
">"|".GT."	{ return '>'; }
"="		{ return '='; }
"+"		{ return '+'; }
"-"		{ return '-'; }
"**"	{ return POW; }
"*"		{ return '*'; }
"/"		{ return '/'; }
"("		{ return '('; }
")"		{ return ')'; }
"::"	{ return ':'; }
","		{ return ','; }
".AND."		{ return '&'; }
".OR."		{ return '|'; }
".NOT."		{ return NOT; }
".EQV."		{ return LEQ; }
".NEQV."	{ return LNE; }
\n		{ return '\n'; }
" "		{ /* space - do nothing */ }
.		{ printf("Lexical error: symbol '%c'!\n", yytext[0]); exit(1); }

%%
