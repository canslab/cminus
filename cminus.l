/****************************************************/
/* File: cminus.l                                   */
/* Lex specification for C-MINUS                    */
/* Jangho Park		                                */
/****************************************************/

%{
#include "globals.h"
#include "util.h"
#include "scan.h"
/* lexeme of identifier or reserved word */
char tokenString[MAXTOKENLEN+1];
%}

digit       [0-9]
number      {digit}+
letter      [a-zA-Z]
identifier  {letter}+
newline     \n
whitespace  [ \t]+

%%

"if"            {return IF;}
"return"        {return RETURN;}
"else"          {return ELSE;}
"while"         {return WHILE;}
"void"          {return VOID;}
"int"           {return INT;}

"=="            {return EQUAL;}
"!="			{return NOTEQ;}
"="             {return ASSIGN;}
"<"             {return LT;}
"<="			{return LTEQ;}
">"				{return GT;}
">="			{return GTEQ;}
"+"             {return PLUS;}
"-"             {return MINUS;}
"*"             {return TIMES;}
"/"             {return DIVISION;}
"("             {return LPAREN;}
")"             {return RPAREN;}
"{"				{return LCURLYBRACKET;}
"}"				{return RCURLYBRACKET;}
"["				{return LSQUAREBRACKET;}
"]"				{return RSQUAREBRACKET;}
";"             {return SEMICOLON;}
","				{return COMMA;}

{number}        {return NUM;}
{identifier}    {return ID;}
{newline}       {lineno++;}
{whitespace}    {/* skip whitespace */}
"/*"            { 
					char c;
                  	for(;;)
                  	{
                		
                  		while ( (c = input()) != '*' && c != EOF )
                  		{
                  			if ( c == '\n')
                  				lineno++;
                  		}
                  		
                  		if ( c == '*' )
                  		{
                  			while ( (c = input()) == '*');
                  			
                  			if ( c == '/' )
                  			{
                  				break;
                  			}              	
                  		}
                  		if ( c == EOF )
                  		{
                  			return ERROR;	
                  		}                 
                  	}
                  	
                }
.               {return ERROR;}

%%

TokenType getToken(void)
{ 
	static int firstTime = TRUE;
  	TokenType currentToken;
  	if (firstTime)
  	{
  		/* initialization for the scanner */
  		firstTime = FALSE;
    	lineno++;
    	yyin = source;
    	yyout = listing;
 	 }
  	currentToken = yylex();
  	strncpy(tokenString,yytext,MAXTOKENLEN);
  	if (TraceScan) 	
  	{
    	fprintf(listing,"\t%d: ",lineno);
    	printToken(currentToken,tokenString);
  	}
  	return currentToken;
}