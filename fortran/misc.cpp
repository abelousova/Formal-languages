#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include "fortran.h"

int yyparse();

extern FILE* yyin;

int main(int argc, char **argv)
{
	FILE *handle;
	if (argc < 2 || argc > 3) {
		cerr << "Usage: " << argv[0] << " program\n";
		exit(1);
	}
	if (!(handle = fopen(argv[1], "r"))) {
		cerr << "Error: cannot open file\n";
		exit(1);
	}
	lineNumber = -1;
	yyin = handle;
	yyparse();
	vector<string> results;
    if (argc == 2) {
		Program->run();
	} else {
		ofstream out(argv[2]);
		Program->run(false, out, &results);
	}
}

void yyerror(string s)
{
	cerr << "Parser error: " << s << "\n";
	exit(1);
}
