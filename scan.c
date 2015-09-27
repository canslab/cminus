/****************************************************/
/* File: scan.c                                     */
/* The scanner implementation for the TINY compiler */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "util.h"
#include "scan.h"

/* states in scanner DFA */
/* 2015-09-26 */
typedef enum STATETYPE
{
	START,			/*  Start State */
	INEQUAL,		/*  deal with ==, =  */
	INNOTEQUAL,     /*  deal with != */
	INGT,			/*  deal with >, >= */
	INLT,			/*  deal with <, <= */
	INCOMMENT,		/*  deal with C-style comment */
	INCOMMENT_INTER1,	/* deals with C-style comment after '/*' */
	INCOMMENT_INTER2,	/* deals with C-style comment before '/' */
	INNUM,
	INID,
	DONE			/*  TERMINATION STATE */
} StateType;

/* lexeme of identifier or reserved word */
char tokenString[MAXTOKENLEN+1];

/* BUFLEN = length of the input buffer for
   source code lines */
#define BUFLEN 256

static char lineBuf[BUFLEN]; /* holds the current line */
static int currnetPositionInLineBuf = 0; /* current position in LineBuf */
static int bufsize = 0; /* current size of buffer string */
static int EOF_flag = FALSE; /* corrects ungetNextChar behavior on EOF */

/* getNextChar fetches the next non-blank character
   from lineBuf, reading in a new line if lineBuf is
   exhausted */
static int getNextChar(void)
{
	if (!(currnetPositionInLineBuf < bufsize))
	{
		lineno++;
		if (fgets(lineBuf, BUFLEN - 1, source))
		{
			if (EchoSource)
				fprintf(listing, "%4d: %s", lineno, lineBuf);
			bufsize = strlen(lineBuf);
			currnetPositionInLineBuf = 0;
			return lineBuf[currnetPositionInLineBuf++];
		}
		else
		{
			EOF_flag = TRUE;
			return EOF;
		}
	}
	else
		return lineBuf[currnetPositionInLineBuf++];
}

/* ungetNextChar backtracks one character
   in lineBuf */
static void ungetNextChar(void)
{
	if (!EOF_flag)
	{
		currnetPositionInLineBuf-- ;
	}
}

/* 2015-09-26 */
/* lookup table of reserved words for C-minus Compiler*/
static struct TOKENTYPE
{
	char* str;
	TokenType tok;
} reservedWords[MAXRESERVED] = {
	{ "else",   ELSE },
	{ "if",     IF},
	{ "int",    INT },
	{ "return", RETURN },
	{ "void",   VOID },
	{ "while",  WHILE },
};

/* lookup an identifier to see if it is a reserved word */
/* uses linear search */
static TokenType reservedLookup(char * s)
{
	int i;
	for (i = 0; i < MAXRESERVED; i++)
		if (!strcmp(s, reservedWords[i].str))
			return reservedWords[i].tok;

	return ID;
}

/****************************************/
/* the primary function of the scanner  */
/****************************************/
/* function getToken returns the 
 * next token in source file
 */
TokenType getToken(void)
{
	/* index for storing into tokenString */
	int tokenStringIndex = 0;

	/* holds current token to be returned */
	TokenType currentToken;

	/* current state - always begins at START */
	StateType state = START;

	/* flag to indicate save to tokenString */
	int save;

	while (state != DONE)
	{
		int c = getNextChar();
		save = TRUE;
		switch (state)
		{
		case START:
			if (isdigit(c))
			{
				state = INNUM;
			}

			else if (isalpha(c))
			{
				state = INID;
			}

			else if (c == '=')
			{
				state = INEQUAL;
			}

			else if (c == '!')
			{
				/* it copes with != case*/
				state = INNOTEQUAL;
			}

			else if ( c == '>' )
			{
				/* transite START to INGT(greater than) state */
				state = INGT;
			}

			else if ( c == '<' )
			{
				/* transite START to INLT(less than) state */
				state = INLT;
			}

			/* if the next character is white space, just consume , don't save it */
			else if ((c == ' ') || (c == '\t') || (c == '\n'))
			{
				save = FALSE;
			}

			/* '/' indicates that we should go into INCOMMENT state */
			else if (c == '/')
			{
				save = FALSE;
				state = INCOMMENT;
			}

			else /* it copes with a single character token */
			{
				state = DONE;
				switch (c)
				{
				case EOF:
					save = FALSE;
					currentToken = ENDFILE;
					break;
				case '+':
					currentToken = PLUS;
					break;
				case '-':
					currentToken = MINUS;
					break;
				case '*':
					currentToken = TIMES;
					break;
				case '/':
					currentToken = DIVISION;
					break;
				case '(':
					currentToken = LPAREN;
					break;
				case ')':
					currentToken = RPAREN;
					break;
				case '{':
					currentToken = LCURLYBRACKET;
					break;
				case '}':
					currentToken = RCURLYBRACKET;
					break;
				case '[':
					currentToken = LSQUAREBRACKET;
					break;
				case ']':
					currentToken = RSQUAREBRACKET;
					break;
				case ',':
					currentToken = COMMA;
					break;
				case ';':
					currentToken = SEMICOLON;
					break;
				default:
					currentToken = ERROR;
					break;
				}
			}
			break;

		case INEQUAL:
			if (c == '=')					/* -> If c is also '=' then it is EQUAL(==) */
			{
				currentToken = EQUAL;
			}
			else							/* it means ASSIGN(=) */
			{
				/* backup in the input */
				ungetNextChar();
				/* if assignment operator, c doesn't have to be saved */
				save = FALSE;
				/* current token should be ASSIGN(=) */
				currentToken = ASSIGN;
			}
			state = DONE;
			break;

		case INNOTEQUAL:			/* added */
			if ( c == '=')			/* != case */
			{
				currentToken = NOTEQ;
			}
			else
			{
				ungetNextChar();
				save = FALSE;
				currentToken = ERROR;
			}
			state = DONE;
			break;

		case INGT:
			if ( c == '=')
			{
				currentToken = GTEQ;
			}
			else
			{
				ungetNextChar();
				save = FALSE;
				currentToken = GT;
			}
			state = DONE;
			break;

		case INLT:
			if (c == '=')
			{
				currentToken = LTEQ;
			}
			else
			{
				ungetNextChar();
				save = FALSE;
				currentToken = LT;
			}
			state = DONE;
			break;

		case INCOMMENT:
			save = FALSE;
			if (c == '*')
			{
				state = INCOMMENT_INTER1;
			}
			else
			{
				ungetNextChar();
				c = '/';
				save = TRUE;
				currentToken = DIVISION;
				state = DONE;
			}
			break;
		//			save = FALSE;
		//			if (c == EOF)
		//			{
		//				state = DONE;
		//				currentToken = ENDFILE;
		//			}
		//			else if (c == '}')
		//				state = START;
		case INCOMMENT_INTER1:
			save = FALSE;
			if ( c == '*' )
			{
				state = INCOMMENT_INTER2;
			}

			/* if we encounters a character except '*', just consume it! */
			/* and the next state would be the same state */
			else
			{
				state = INCOMMENT_INTER1;
			}
			break;

		case INCOMMENT_INTER2:
			save = FALSE;
			if ( c == '/')
			{
				// comment ended
				// return to first state(=START)
				state = START;
			}
			else if (c == '*')
			{
				state = INCOMMENT_INTER2;
			}
			else	/* when an input character is not ('/' or '*') */
			{
				state = INCOMMENT_INTER1;
			}
			break;

		case INNUM:
			if (!isdigit(c))
			{
				/* backup in the input */
				ungetNextChar();
				save = FALSE;
				state = DONE;
				currentToken = NUM;
			}
			break;
		case INID:
			if (!isalpha(c))
			{
				/* backup in the input */
				ungetNextChar();
				save = FALSE;
				state = DONE;
				currentToken = ID;
			}
			break;
		case DONE:
		default: /* should never happen */
			fprintf(listing, "Scanner Bug: state= %d\n", state);
			state = DONE;
			currentToken = ERROR;
			break;
		}

		if ((save) && (tokenStringIndex <= MAXTOKENLEN))
		{
			tokenString[tokenStringIndex++] = (char) c;
		}

		if (state == DONE)
		{
			tokenString[tokenStringIndex] = '\0';
			if (currentToken == ID)
			{
				currentToken = reservedLookup(tokenString);
			}
		}
	}
	if (TraceScan)
	{
		fprintf(listing, "\t%d: ", lineno);
		printToken(currentToken, tokenString);
	}
	return currentToken;
} /* end getToken */
