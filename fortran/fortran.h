#ifndef _FORTRAN_H
#define _FORTRAN_H

#include <string>
#include <vector>
#include <iostream>
using namespace std;

class Expr {
    public:
	virtual string eval(bool writeToVector = false, ostream& out = cout, vector<string> *output = NULL) = 0;
};

class ExprArith : public Expr {
    public:
	ExprArith(int op_, Expr *e1_, Expr *e2_) : op(op_), e1(e1_), e2(e2_) { }
    string eval(bool writeToVector = false, ostream& out = cout, vector<string>* output = NULL);
    private:
	int op; /* '+', '-', '*', '/', UMINUS, POW */
	Expr *e1, *e2;
};

class ExprNumeral : public Expr {
    public:
	ExprNumeral(int val_) : val(val_) { }
    string eval(bool writeToVector = false, ostream& out = cout, vector<string>* output = NULL);
    private:
	int val;
};

class ExprVariable : public Expr {
    public:
       ExprVariable(const char *name_) : name(name_) { }
       string eval(bool writeToVector = false, ostream& out = cout, vector<string>* output = NULL);
    private:
       string name;
};

class LogExpr { /* A <> B */
    public:
	LogExpr(int op_, Expr *e1_, Expr *e2_) : op(op_), e1(e1_), e2(e2_) { }
    string eval(bool writeToVector = false, ostream& out = cout, vector<string> *output = NULL);
    private:
	int op; /* '<', '>', '=' */
	Expr *e1, *e2;
};

class LogStmt {
	public:
	LogStmt(int op_, LogStmt *e1_, LogStmt *e2_) : op(op_), e1(e1_), e2(e2_), isExpr(false) { }
	LogStmt(LogExpr *e_) : e(e_), isExpr(true) { }
    string eval(bool writeToVector = false, ostream& out = cout, vector<string>* output = NULL);
    private:
	int op; 
	bool isExpr;
	LogStmt *e1, *e2;
	LogExpr *e;
};


class Stmt { 
    public:
	virtual void run(bool writeToVector = false, ostream& out = cout, vector<string> *output = NULL) = 0;
};

class ListStmt {
    public:
	ListStmt() { }
	void add(Stmt *s) { list.push_back(s); }
	virtual void run(bool writeToVector = false, ostream& out = cout, vector<string> *output = NULL);
    private:
	vector<Stmt*> list;
};

class ListIdent {
	vector<string> list;
    public:
	ListIdent() {}
	void add(char* s) { list.push_back(s); }
	void ident();
};

class StmtPrint : public Stmt {
    public:
	StmtPrint(Expr *e_) : e(e_) { }
	virtual void run(bool writeToVector = false, ostream& out = cout, vector<string> *output = NULL);
    private:
	Expr *e;
};

class StmtIf : public Stmt {
    public:
	StmtIf(LogStmt *e_, ListStmt *L1_, ListStmt *L2_) : e(e_), L1(L1_), L2(L2_) { }
	virtual void run(bool writeToVector = false, ostream& out = cout, vector<string> *output = NULL);
    private:
	LogStmt *e;
	ListStmt *L1, *L2;
};

class StmtAssign : public Stmt {
    public:
	StmtAssign(const string name_, Expr *e_) : name(name_), e(e_) { }
	virtual void run(bool writeToVector = false, ostream& out = cout, vector<string> *output = NULL);
    private:
	string name;
	Expr *e;
};

class StmtCycleDo : public Stmt {
	ListStmt *L;
public:
	StmtCycleDo(ListStmt* L_) : L(L_){ }
	virtual void run(bool writeToVector = false, ostream& out = cout, vector<string> *output = NULL);
};

class StmtExit : public Stmt {
public:
	StmtExit() {};
	virtual void run(bool writeToVector = false, ostream& out = cout, vector<string> *output = NULL);
};

class StmtDefineList : public Stmt {
	ListIdent* list_ident;
public:
	StmtDefineList(ListIdent* list) { list_ident = list; }
	virtual void run(bool writeToVector = false, ostream& out = cout, vector<string> *output = NULL) { list_ident -> ident(); }
};

class StmtCycleLabel : public Stmt {
	ListStmt* L;
	string name;
	int begin;
	int end;
	int increment;
public:
	StmtCycleLabel(ListStmt* L_, const string name_, int begin_, int end_, int increment_) : 
		L(L_),
		name(name_),
		begin(begin_),
		end(end_),
		increment(increment_) { }
	virtual void run(bool writeToVector = false, ostream& out = cout, vector<string> *output = NULL);
};

class StmtCycleWhile : public Stmt {
	ListStmt* L;
	LogStmt* e;
public:
	StmtCycleWhile(ListStmt* L_, LogStmt* e_) : 
		L(L_),
		e(e_) {}
	virtual void run(bool writeToVector = false, ostream& out = cout, vector<string> *output = NULL);
};

extern ListStmt *Program;
extern ListIdent *ident_list;

#include "fortran.tab.h"

struct id_tab {
	string name;
	int value;
};

extern struct id_tab Table[];

string tempVarName(int value, bool writeToVector = false, ostream& out = cout, vector<string> *output = NULL);
struct id_tab *get_id(string name);
void add_id(string name, int value);
string pow_int(string x, string n, bool writeToVector, ostream& out, vector<string> *output);
extern int lineNumber;

int yylex();
void yyerror(string s);

#endif /* _FORTRAN_H */
