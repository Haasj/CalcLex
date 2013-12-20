#include<fstream>
#include<iostream>
#include<string>
#include<iomanip>
using namespace std;

extern ifstream calcFile;		//global fstream
extern char tokenString[256];	//global max token length 
extern string enumText;		//must have this to convert back to the text of the enum; global so i don't have to pass it back

//token declarations
extern enum CALCTOKENS {EOFSY=0, READSY=1, WRITESY=2, ASSIGNOPP=3, LPARAN=4, RPARAN=5, ADDOPP=6, SUBOPP=7, MULTOPP=8, DIVOPP=9, NUMCONST=10, ID=11};

int getToken()
{
	while (1)
	{
		memset(tokenString, 0, 255);	//clears the token buffer
		int curChar;
		while ((curChar = calcFile.get()) == ' ' || curChar == '\t' || curChar == '\n')	//ignore all whitespace chars
			continue;	//will do the while loop again. essentially a no-op


		if (curChar == '/')	//could be divide or start of a comment
		{
			if (calcFile.peek() == '*')	//start of a comment
			{
				calcFile.get();	//do this to get past the *; only relevant in casese with /*/--not a valid comment
				while (curChar = calcFile.get())	//just gets the next character
				{
					if ((curChar == '*' && calcFile.peek() == '/') || curChar == '\n')
						break;	//the only way to get out of the loop is if the sentinal is reached or newline
				}
				calcFile.get();	//gets the /. If this isn't there, the next char gotten will be the /
				continue; //essentially re-starts the getToken function
			}
			else //it is a divide operation
				return DIVOPP;
		}

		if (curChar == EOF)
			return EOFSY;

		if (curChar == ':' && calcFile.peek() == '=')
		{
			calcFile.get();	//have to get the = too
			return ASSIGNOPP;
		}
		if (curChar == '(')
			return LPARAN;
		if (curChar == ')')
			return RPARAN;
		if (curChar == '+')
			return ADDOPP;
		if (curChar == '-')
			return SUBOPP;
		if (curChar == '*')		//note: can't be a comment, since we've already checked for those
			return MULTOPP;
		if (curChar >= '0' && curChar <= '9') //start of number
		{
			int decimalCount = 0;	//to hold the number of decimal points in the number
			tokenString[0] = curChar;
			int j = 1;
			while (((curChar = calcFile.get()) >= '0' && curChar <= '9') || (curChar == '.')) //the next char is a number or .
			{
				if (curChar == '.')	//add one to the decimal count
					decimalCount++;
				if (decimalCount == 2)
					break;			//breaks as soon as the second decimal point is hit
				tokenString[j] = curChar;
				j++;
			}
			calcFile.unget();	//unget the char after the number--could be a token that needs to be processed in the next iteration
			return NUMCONST;
		}
		else //it is a letter
		{
			tokenString[0] = toupper(curChar);
			int j = 1;
			while ((curChar = calcFile.get()) != ' ' && curChar != '\t' && curChar != '\n' && curChar != EOF && curChar != ':' && curChar != '(' && curChar != ')' && curChar != '+' && curChar != '-' && curChar != '*' && curChar != '/')
			{
				tokenString[j] = toupper(curChar);
				j++;
			}
			tokenString[j] = 0; //enter null byte at the end
			calcFile.unget();	//only have to do this if the next char is EOF, but doesn't hurt to do it every time.
			if (_stricmp(tokenString, "read") == 0)
				return READSY;
			if (_stricmp(tokenString, "write") == 0)
				return WRITESY;
			//else it is actually an id
			return ID;
		}
	}
}

void decodeToken (int token)
{
	switch (token) {
		case 0:
			enumText = "EOFSY";
			tokenString[0] = 0;	//nothing for the string
			break;
		case 1:
			enumText = "READSY";
			break;	//string will have read in it
		case 2:
			enumText = "WRITESY";
			break;	//string will have write in it
		case 3:
			enumText = "ASSIGNOPP";
			tokenString[0] = ':';
			tokenString[1] = '=';
			tokenString[2] = 0;
			break;
		case 4:
			enumText = "LPARAN";
			tokenString[0] = '(';
			tokenString[1] = 0;
			break;
		case 5:
			enumText = "RPARAN";
			tokenString[0] = ')';
			tokenString[1] = 0;
			break;
		case 6:
			enumText = "ADDOPP";
			tokenString[0] = '+';
			tokenString[1] = 0;
			break;
		case 7:
			enumText = "SUBOPP";
			tokenString[0] = '-';
			tokenString[1] = 0;
			break;
		case 8:
			enumText = "MULTOPP";
			tokenString[0] = '*';
			tokenString[1] = 0;
			break;
		case 9:
			enumText = "DIVOPP";
			tokenString[0] = '/';
			tokenString[1] = 0;
			break;
		case 10:
			enumText = "NUMCONST";
			break;
		case 11:
			enumText = "ID";
			break;
		}
}
