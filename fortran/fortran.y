%{
#include "fortran.h"
%}

%union {
	Expr *expr;
	LogExpr *log_expr;
	Stmt *stmt;
	LogStmt *log_stmt;
	ListStmt *list_stmt;
	ListIdent *list_ident;

	int int_val;
	char ident_name[256];
}; /* тип yylval */

/* Precedence */
%left '+' '-'
%left '*' '/'
%right POW
%nonassoc UMINUS

%left LEQ LNE
%left '|'
%left '&'
%nonassoc NOT

%token <int_val> NUMERAL
%token <ident_name> IDENT
%token PRINT IF THEN ELSE ENDIF EQ DO ENDDO PROGRAM ENDPROGRAM INTEGER LE GE NE EXIT LABEL WHILE NOT LEQ LNE POW

%type <expr> expr
%type <log_expr> log_expr
%type <list_stmt> list_stmt
%type <stmt> stmt
%type <log_stmt> log_stmt
%type <list_ident> list_ident

%%

program
	:PROGRAM IDENT separator list_stmt ENDPROGRAM IDENT { Program = $4; }
	;

list_stmt
	: list_stmt stmt separator { $1->add($2); $$=$1; }
	| /* epsilon */ { $$ = new ListStmt; }
	;

separator
	: '\n'
	| separator '\n'
	;

list_ident
	: IDENT { $$ = new ListIdent; $$->add($1); } 
	| list_ident ',' IDENT { $1->add($3); $$ = $1; }
	;

stmt
	: PRINT expr { $$ = new StmtPrint($2); } 
	| IDENT '=' expr { $$ = new StmtAssign($1, $3); }
	| IF '(' log_stmt ')' THEN separator list_stmt ELSE separator list_stmt ENDIF { $$ = new StmtIf($3, $7, $10); }
	| IF '(' log_stmt ')' THEN separator list_stmt ENDIF { $$ = new StmtIf($3, $7, NULL); }
	| INTEGER ':' list_ident { $$ = new StmtDefineList($3); }
	| DO separator list_stmt ENDDO { $$ = new StmtCycleDo($3); }
	| EXIT { $$ = new StmtExit(); }
	| DO LABEL IDENT '=' NUMERAL ',' NUMERAL separator list_stmt ENDDO { $$ = new StmtCycleLabel($9, $3, $5, $7, 1); }
	| DO LABEL IDENT '=' NUMERAL ',' NUMERAL ',' NUMERAL separator list_stmt ENDDO { $$ = new StmtCycleLabel($11, $3, $5, $7, $9); }
	| DO separator list_stmt WHILE '(' log_stmt ')' { $$ = new StmtCycleWhile($3, $6); }
	;

log_expr
	: expr '<' expr { $$ = new LogExpr('<', $1, $3); }
	| expr '>' expr { $$ = new LogExpr('>', $1, $3); }
	| expr EQ expr	{ $$ = new LogExpr(EQ, $1, $3); }
	| expr LE expr	{ $$ = new LogExpr(LE, $1, $3); }
	| expr GE expr	{ $$ = new LogExpr(GE, $1, $3); }
	| expr NE expr	{ $$ = new LogExpr(NE, $1, $3); }
	;

log_stmt
	: log_stmt '&' log_stmt { $$ = new LogStmt('&', $1, $3); }
	| log_stmt '|' log_stmt { $$ = new LogStmt('|', $1, $3); }
	| log_stmt LEQ log_stmt { $$ = new LogStmt(LEQ, $1, $3); }
	| log_stmt LNE log_stmt { $$ = new LogStmt(LNE, $1, $3); }
	| NOT log_stmt { $$ = new LogStmt(NOT, $2, NULL); }
	| '(' log_stmt ')' { $$ = $2; }
	| log_expr { $$ = new LogStmt($1); }
	;

expr
	: IDENT  { $$ = new ExprVariable($1); }
	| NUMERAL 		{ $$ = new ExprNumeral($1); }
	| '-' expr %prec UMINUS	{ $$ = new ExprArith(UMINUS, $2, NULL); }
	| expr '+' expr		{ $$ = new ExprArith('+', $1, $3); }
	| expr '-' expr		{ $$ = new ExprArith('-', $1, $3); }
	| expr '*' expr		{ $$ = new ExprArith('*', $1, $3); }
	| expr '/' expr		{ $$ = new ExprArith('/', $1, $3); }	
	| '(' expr ')'		{ $$ = $2; }
	| expr POW expr		{ $$ = new ExprArith(POW, $1, $3); }
	;

%%
