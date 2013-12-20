/* CalcGrammar Rules
//rules								//predict sets
program -> stmt_list $$				{ID, read, write, $$} 
 
stmt_list -> stmt stmt_list			{ID, read, write } 
stmt_list -> <epsilon>				{ $$ } 
 
stmt -> ID := expr					{ ID } 
stmt -> read ID						{ read } 
stmt -> write expr					{ write } 
 
expr -> term term_tail				{ (, ID, numconst } 
 
term_tail -> + term term_tail		{ + } 
term_tail -> - term term_tail		{ - } 
term_tail -> <epsilon>				{ ),ID,read,write,$$} 
 
term -> factor factor_tail			{ (, ID, numconst } 
 
factor_tail -> * factor factor_tail { * } 
factor_tail -> / factor factor_tail { / } 
factor_tail -> <epsilon>			{ +, -, ), ID, read, write, $$} 
 
factor -> ( expr )					{ ( } 
factor -> ID						{ ID } 
factor -> numconst					{ numconst }
*/

#include<fstream>
#include<iostream>
#include<string>
#include<iomanip>
#include<map>
using namespace std;

#include "CalcGrammar.h"
#include "Lex.h"
#include "symbolTable.h"

ifstream calcFile;		//global fstream
char tokenString[256];	//global max token length 
string enumText;		//must have this to convert back to the text of the enum; global so i don't have to pass it back

//token declarations
enum CALCTOKENS {EOFSY=0, READSY=1, WRITESY=2, ASSIGNOPP=3, LPARAN=4, RPARAN=5, ADDOPP=6, SUBOPP=7, MULTOPP=8, DIVOPP=9, NUMCONST=10, ID=11};

//current lexical token
int tok;

//stack initialization
static double stack[100];
static int top = 0;

static void stackinit()		{ top = 0; }
static int empty()			{ return top == 0; }

static double pop()			{ return stack[top--]; }
static void push(double v)		{ stack[++top] = v; }

//map initialization
map<string, symbolic *> symTable;	//initialize the map for IDs and values
symbolic *psyml;

void match(int expectedTok)
{
	if (tok == expectedTok)
	{
		if(tok != EOFSY)
			tok = getToken();
	}
	else
		cout << "Token " << expectedTok << " expected, but instead encountered " << tokenString << endl;
}

void program()
{
	stackinit();	//initialize the stack
	tok = getToken();

	stmt_list();
	match(EOFSY);
}

void stmt_list()
{
	if ((tok == ID) || (tok == READSY) || (tok == WRITESY))
	{
		stmt();
		stmt_list();
	}
	//else it's epsilon
}

void stmt()
{
	if (tok == ID)
	{
		string curID = tokenString;	//this will save the current ID before match overwrites it (via getToken)
		match(ID);
		match(ASSIGNOPP);			//if ID is there, the next one should be :=
		expr();						//when it returns from this, whatever the expression evaluated to should be on top of the stack
		symTable[curID] = new symbolic(pop());
		psyml = symTable[curID];
		cout << "ASSIGN: " << curID << " = " << psyml->val << endl;
	}
	if (tok == READSY)
	{
		double inputVal;
		match(READSY);
		cout << "Read: Enter value for " << tokenString << "> ";
		cin >> inputVal;
		symTable[tokenString] = new symbolic(inputVal);
		match(ID);
	}
	if (tok == WRITESY)
	{
		match(WRITESY);
		expr();						//when it returns from this, whatever the expression evaluated to should be on top of the stack
		cout << "WRITE: " << pop() << endl;
	}
}

void expr()
{
	term();
	term_tail();
}

void term_tail()
{
	if (tok == ADDOPP)
	{
		match(ADDOPP);		//gets the next token
		term();				//this will get the next value
		double v2 = pop();
		double v1 = pop();
		push (v1+v2);
		term_tail();
	}
	else if (tok == SUBOPP)
	{
		match(SUBOPP);		//gets the next token
		term();
		double v2 = pop();
		double v1 = pop();
		push (v1-v2);
		term_tail();
	}
	//else is epsilon
}

void term()
{
	factor();
	factor_tail();
}

void factor_tail()
{
	if (tok == MULTOPP)
	{
		match(MULTOPP);
		factor();
		double v2 = pop();
		double v1 = pop();
		push (v1*v2);
		factor_tail();
	}
	else if (tok == DIVOPP)
	{
		match(DIVOPP);
		factor();
		double v2 = pop();
		double v1 = pop();
		push (v1/v2);
		factor_tail();
	}
	//else is epsilon
}

void factor()
{
	if (tok == LPARAN)
	{
		match(LPARAN);
		expr();
		match(RPARAN);
	}
	else if (tok == ID)
	{
		if (symTable.count(tokenString) == 0)
		{
			fprintf(stderr, "Error. %s not initialized.\n");
			return;
		}
		psyml = symTable[tokenString];
		push (psyml->val);
		match(ID);
	}
	else if (tok == NUMCONST)
	{
		push (atof(tokenString));
		match(NUMCONST);
	}

}
