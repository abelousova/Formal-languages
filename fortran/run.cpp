#include <iostream>
#include <cstdlib>
#include <string>

#include "fortran.h"

int lineNumber;

void printCmp(string left, string right, int op, int thenLine, int elseLine, 
	bool writeToVector, ostream& out, vector<string>* output) {
		string thenLineStr = to_string(static_cast<long long>(thenLine));
	string elseLineStr = to_string(static_cast<long long>(elseLine));
	string res;
	switch (op) {
	case '>':
		res = "cmp " + left + " " + right + " " + elseLineStr + " " + elseLineStr + " " + thenLineStr + "\n";
		break;
	case '<':
		res = "cmp " + left + " " + right + " " + thenLineStr + " " + elseLineStr + " " + elseLineStr + "\n";
		break;
	case EQ:
		res = "cmp " + left + " " + right + " " + elseLineStr + " " + thenLineStr + " " + elseLineStr + "\n";
		break;
	case LE:
		res = "cmp " + left + " " + right + " " + thenLineStr + " " + thenLineStr + " " + elseLineStr + "\n";
		break;
	case GE:
		res = "cmp " + left + " " + right + " " + elseLineStr + " " + thenLineStr + " " + thenLineStr + "\n";
		break;
	case NE:
		res = "cmp " + left + " " + right + " " + thenLineStr + " " + elseLineStr + " " + thenLineStr + "\n";
		break;
	}

	if (writeToVector) {
		output->push_back(res);
	} else {
		out << res;		
	}
	++lineNumber;
}


string ExprArith::eval(bool writeToVector, ostream& out, vector<string>* output)
{ 
	string v1, v2;
	v1 = e1->eval(writeToVector, out, output); 
	if(e2) { 
		v2 = e2->eval(writeToVector, out, output); 
	}
	string res = tempVarName(0, writeToVector, out, output);
	string outValue;
	switch(op) {
		case '+':
			outValue = "add " + v1 + " " + v2 + " " + res + "\n";	
			break;
		case '-': 
			outValue = "sub " + v1 + " " + v2 + " " + res + "\n";
			break;
		case '*': 
			outValue = "mul " + v1 + " " + v2 + " " + res + "\n";
			break;
		case '/': 
			outValue = "div " + v1 + " " + v2 + " " + res + "\n";
			break;
		case UMINUS: 
			outValue = "mul " + v1 + " -1 " + res + "\n";
			break;
		case POW:
			outValue = pow_int(v1, v2, writeToVector, out, output);
			break;
	}
	if (writeToVector) {
		output->push_back(outValue);
	} else {
		out << outValue;
	}		
	++lineNumber;
	return res;
}

string ExprNumeral::eval(bool writeToVector, ostream& out, vector<string>* output) {
	string name = tempVarName(val, writeToVector, out, output);
	return name;
}

string ExprVariable::eval(bool writeToVector, ostream& out, vector<string>* output) {
	if (get_id(name)) {
		return name;
	} else {
		cerr << "Runtime error: " << name << " not defined\n";
		exit(1);
	}		   
}

string LogExpr::eval(bool writeToVector, ostream& out, vector<string>* output)
{
	string v1, v2;
	string result = tempVarName(0, writeToVector, out, output);
	v1 = e1->eval(writeToVector, out, output); 
	v2 = e2->eval(writeToVector, out, output);

	int thenLine = lineNumber + 2;
	int elseLine = lineNumber + 3;
	printCmp(v1, v2, op, thenLine, elseLine, writeToVector, out, output);
	string resTrue = "let " + result + " 1\n"; 
	if (writeToVector) {
		output->push_back(resTrue);
	} else {
		out << resTrue;
	}
	++lineNumber;
	return result;
}

string LogStmt::eval(bool writeToVector, ostream& out, vector<string> *output) {
	if (isExpr) {
		return e->eval(writeToVector, out, output);
	}

	string v1 = e1->eval(writeToVector, out, output);
	string v2;
	if (e2) {
		v2 = e2->eval(writeToVector, out, output);
	}

	string res = tempVarName(0, writeToVector, out, output);
	string resultPrint;

	switch (op) {
		case '&':
			resultPrint = "mul " + v1 + " " + v2 + " " + res + "\n";
			++lineNumber;
			break;
		case '|': {
		    string sum = tempVarName(0, writeToVector, out, output);
			resultPrint = "add " + v1 + " " + v2 + " " + sum + "\n";
			++lineNumber;

			string mul = tempVarName(0, writeToVector, out, output);
			resultPrint += "mul " + v1 + " " + v2 + " " + mul + "\n";
			++lineNumber;

			resultPrint += "sub " + sum + " " + mul + " " + res + "\n";
			++lineNumber;
			break; }
		case NOT:
			resultPrint = "mul " + v1 + " -1 " + res + "\n";
			++lineNumber;
			break;
		case LEQ: {
			printCmp(v1, v2, '=', lineNumber + 2, lineNumber + 3, writeToVector, out, output);
			resultPrint = "let " + res + " 1\n";
			++lineNumber;
			break; }
		case LNE: {
			printCmp(v1, v2, NE, lineNumber + 2, lineNumber + 3, writeToVector, out, output);
			resultPrint = "let " + res + " 1\n";
			++lineNumber;
			break; }
	}

	if (writeToVector) {
		output->push_back(resultPrint);
	} else {
		out << resultPrint;
	}

	return res;
}

void ListStmt::run(bool writeToVector, ostream& out, vector<string>* output)
{
	vector<Stmt*>::iterator i;
	for (i = list.begin(); i < list.end(); i++)
		(*i)->run(writeToVector, out, output);
}

void StmtPrint::run(bool writeToVector, ostream& out, vector<string> *output)
{
	string res = "out " + e->eval(writeToVector, out, output) + "\n";
	if (writeToVector) {
		output->push_back(res);
	} else {
		out << res;
	}
	++lineNumber;
}

void StmtIf::run(bool writeToVector, ostream& out, vector<string>* output)
{	
	string v = e->eval(writeToVector, out, output);
	int thenLine = lineNumber + 3;
	vector<string> thenVector;
	L1->run(true, out, &thenVector);
	vector<string> elseVector;
	if (L2) {		
		L2->run(true, out, &elseVector);
	}

	int elseLine = thenLine + thenVector.size() + 1;
	int endifLine = elseLine + elseVector.size();
	
	string endifLineStr = to_string(static_cast<long long>(endifLine));

	string zero = tempVarName(0, writeToVector, out, output);
	printCmp(v, zero, EQ, thenLine, elseLine, writeToVector, out, output);
	for (int i = 0; i < thenVector.size(); i++) {
		if (writeToVector) {
			output->push_back(thenVector[i]);
		} else {			
			out << thenVector[i];			
		}
	}

	if (writeToVector) {
		output->push_back("jmp " + endifLineStr + "\n");
	} else {
		out << "jmp " << endifLineStr << endl;		
	}
	++lineNumber;

	for (int i = 0; i < elseVector.size(); i++) {
		if (writeToVector) {
			output->push_back(elseVector[i]);
		} else {
			out << elseVector[i];			
		}
	}
}

void StmtAssign::run(bool writeToVector, ostream& out, vector<string>* output)
{
	string valueName = e->eval(writeToVector, out, output);
	struct id_tab *tab;
	if ((tab = get_id(name))) {
		tab->value = get_id(valueName)->value;
		string res = "mov " + valueName + " " + name + "\n";
		if (writeToVector) {
			output->push_back(res);
		} else {
			out << res;			
		}
		++lineNumber;		
	} else {
		cerr << "variable " + name + " is not defined";
		exit(1);
	}	
}

void StmtCycleDo::run(bool writeToVector, ostream& out, vector<string>* output)
{
	int cycleBeginLine = lineNumber + 1;
	vector<string> doOutput;
	if (L) {
		L->run(true, out, &doOutput);
	}
	for (int i = 0; i < doOutput.size(); i++) {
		string res;
		if (doOutput[i] == "exit") {
			res = "jmp " + to_string(static_cast<long long>(cycleBeginLine + doOutput.size() + 1)) + "\n";
		} else {
			res = doOutput[i];
		}
		if (writeToVector) {
			output->push_back(res);
		} else {
			out << res;			
		}
		++lineNumber;
	}

	string end = "jmp " + to_string(static_cast<long long>(cycleBeginLine)) + "\n";
	if (writeToVector) {
		output->push_back(end);
	} else {
		out << end;		
	}
	++lineNumber;	
}

void StmtExit::run(bool writeToVector, ostream& out, vector<string>* output) {
	if (!writeToVector) {
		cerr << "syntax error";
	}
	output->push_back("exit");
}

void ListIdent::ident() {
	for (int i = 0; i < list.size(); i++) {
		string name(list[i]);
		if (!get_id(name)) {
			add_id(name, 0);
		} else {
			cerr << "wrong identification list" << endl;
			exit(1);
		}
	}
}

void StmtCycleLabel::run(bool writeToVector, ostream& out, vector<string>* output) {
	string init = "let " + name + " " + to_string(static_cast<long long>(begin)) + "\n";
	if (writeToVector) {
		output->push_back(init);
	} else {
		out << init;
	}
	++lineNumber;
	add_id(name, begin);
	string incName = tempVarName(increment, writeToVector, out, output);
	string endName = tempVarName(end, writeToVector, out, output);
	string temp = tempVarName(0, writeToVector, out, output);
	int cycleBeginLine = lineNumber + 1;
	vector<string> doBody;
	lineNumber++;
	if (L) {
		L->run(true, out, &doBody);
	}
	vector<string> doOutput;
	int endLine = cycleBeginLine + 1 + doBody.size() + 3;
	string condition = "cmp " + name + " " + endName + " ";
	string bodyLineStr = to_string(static_cast<long long>(cycleBeginLine + 1));
	string endLineStr = to_string(static_cast<long long>(endLine));
	if (increment > 0) {
		condition = condition + bodyLineStr + " " + bodyLineStr + " " +  endLineStr + "\n";
	} else if (increment < 0) {
		condition = condition + endLineStr + " " + bodyLineStr + " " +  bodyLineStr + "\n";
	} else {
		cerr << "syntax error: increment cannot be 0";
	}
	doOutput.push_back(condition);
	doOutput.insert(doOutput.end(), doBody.begin(), doBody.end());
	doOutput.push_back("add " + name + " " + incName + " " + temp + "\n");
	doOutput.push_back("mov " + temp + " " + name + "\n");
	lineNumber += 2;
	for (int i = 0; i < doOutput.size(); i++) {
		string res;
		if (doOutput[i] == "exit") {
			res = "jmp " + to_string(static_cast<long long>(cycleBeginLine + doOutput.size() + 1)) + "\n";
		} else {
			res = doOutput[i];
		}
		if (writeToVector) {
			output->push_back(res);
		} else {
			out << res;			
		}
	}

	string end = "jmp " + to_string(static_cast<long long>(cycleBeginLine)) + "\n";
	if (writeToVector) {
		output->push_back(end);
	} else {
		out << end;		
	}
	++lineNumber;	
	
}

void StmtCycleWhile::run(bool writeToVector, ostream& out, vector<string>* output) {
	int cycleBeginLine = lineNumber + 1;	
	
	vector<string> doBody;
	if (L) {
		L->run(true, cout, &doBody);
	}
	vector<string> conditionBody;
	string conditionRes = e->eval(true, cout, &conditionBody);

	int exitLine = lineNumber + 3;

	for (int i = 0; i < doBody.size(); i++) {
		string res;
		if (doBody[i] == "exit") {
			res = "jmp " + to_string(static_cast<long long>(exitLine)) + "\n";
		} else {
			res = doBody[i];
		}
		if (writeToVector) {
			output->push_back(res);
		} else {
			out << res;			
		}
	}
	
	for (int i = 0; i < conditionBody.size(); i++) {
		string res;
		if (conditionBody[i] == "exit") {
			cerr << "syntax error";
		} else {
			res = conditionBody[i];
		}
		if (writeToVector) {
			output->push_back(res);
		} else {
			out << res;			
		}
	}

	string zero = tempVarName(0, writeToVector, out, output);
	printCmp(conditionRes, zero, EQ, cycleBeginLine, exitLine, writeToVector, out, output);
}

string tempVarName(int value, bool writeToVector, ostream& out, vector<string> *output) {
	long long i = 0;
	while (true) {
		string name = "name" + to_string(i);
		if (!(get_id(name))) {
			add_id(name, value);
			string res = "let " + name + " " + to_string(static_cast<long long>(value)) + "\n";
			if (writeToVector) {
				output->push_back(res);
			} else {
				out << res;				
			}
			++lineNumber;			
			return name;
		}
		++i;
	}
}

string pow_int(string x, string n, bool writeToVector, ostream& out, vector<string> *output) {
	string res = tempVarName(1);
	string counter = tempVarName(0);

	string line = to_string(static_cast<long long>(lineNumber + 1));
	string line2 = to_string(static_cast<long long>(lineNumber + 3));

	string cmp = "cmp " + counter + " " + n + " " + line + " " + line + " " + line2 + "\n";
	string mul = "mul " + res + " " + x + " " + res + "\n"; 
	string add = "add " + counter + " 1" + counter + "\n";
	
	if (writeToVector) {
		output->push_back(cmp);
		output->push_back(mul);
		output->push_back(add);
	} else {
		out << cmp;
		out << mul;
		out << add;
	}
	lineNumber += 3;

	return res;
}

ListStmt *Program;
