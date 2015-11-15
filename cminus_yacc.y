/****************************************************/
/* File: tiny.y                                     */
/* The TINY Yacc/Bison specification file           */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/
%{
#define YYPARSER /* distinguishes Yacc output from other code files */

#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

#define YYSTYPE TreeNode*
static char * savedName; /* for use in assignments */
static int savedNumber; 	/* for use in var_declaration of array */
static int savedLineNo;  /* ditto */
static TreeNode * savedTree; /* stores syntax tree for later return */
static int yylex(void); // added 11/2/11 to ensure no conflict with lex

/* Codes that are below are related to the stack library */
#define STACK_MAX	10

int nameStackIndex = 0;
int lineStackIndex = 0;
int numberStackIndex = 0;

char *nameStack[STACK_MAX];
int lineStack[STACK_MAX];
int numberStack[STACK_MAX];

void pushToNameStack(char *str);
void pushToLineStack(int noLine);
void pushToNumberStack(int number);

char *popFromNameStack();
int popFromLineStack();
int popFromNumberStack();

%}

%token IF RETURN ELSE WHILE VOID INT
%token ID NUM
%token EQUAL NOTEQ ASSIGN LT LTEQ GT GTEQ PLUS MINUS TIMES DIVISION LPAREN RPAREN
%token LCURLYBRACKET RCURLYBRACKET LSQUAREBRACKET RSQUAREBRACKET SEMICOLON COMMA
%token ERROR 

%% /* Grammar for TINY */

/* 1 */
program				: declaration_list 
					{ 
						savedTree = $1; 
					}
					;

/* 2 */
declaration_list	: declaration_list declaration	
					{
						YYSTYPE t = $1;
						if ( t != NULL )
						{
							while ( t->sibling != NULL )
							{
								t = t->sibling;
							}
							t->sibling = $2;
							$$ = $1;	
						}
						else
						{
							$$ = $2;
						}
					}
					| declaration		/* declaration_list -> declaration */
					{
						$$ = $1;
					}
					;

/* 3 */			
declaration			: var_declaration	{ $$ = $1; }
					| fun_declaration	{ $$ = $1; }
					;

/* 4 */
var_declaration		: type_specifier ID SEMICOLON
					{
						$$ = newDeclNode(VarK);
						$$->child[0] = $1;	/* type specifier is the first child */
						$$->bWithIndex = 0;
						
						$$->name = popFromNameStack();
						$$->lineno = popFromLineStack();
				
					}
					| type_specifier ID 
					  LSQUAREBRACKET NUM RSQUAREBRACKET SEMICOLON
					{
						$$ = newDeclNode(VarK);
						$$->child[0] = $1;						/* type specifier is the first child */
						$$->name = popFromNameStack();			/* assign string of ID to Node */
						$$->lineno = popFromLineStack();		/* assign line number to Node */
						$$->bWithIndex = 1;
						
						
						TreeNode *numberNode = newExpNode(ConstK);	/* subscript index is included in the new number node */
						numberNode->value = popFromNumberStack();	/* subscript index is the contents of the number node*/
						
						$$->child[1] = numberNode;		/* subscript index is the second child*/
					}
					;
			
/* 5 */	
type_specifier  	: INT   { $$ = newExpNode(TypeK); $$->name = copyString("int"); $$->bDataType = Integer; }
					| VOID	{ $$ = newExpNode(TypeK); $$->name = copyString("void"); $$->bDataType = Void; }
					;

/* 6 */
fun_declaration		: type_specifier ID LPAREN params RPAREN compound_stmt
					{
						$$ = newDeclNode(FunK);
						$$->child[0] = $1;
						$$->name = popFromNameStack();
						$$->lineno = popFromLineStack();
						$$->child[1] = $4;
						$$->child[2] = $6;
					}
					;
					
/* 7 */
params				: param_list
					{ $$ = $1; }
					| VOID
					;

/* 8 */
param_list			: param_list COMMA param
					{ 
						TreeNode *t = $1;
						
						if ( t != NULL )
						{
							while( t->sibling != NULL)
							{
								t = t->sibling;
							}
							
							t->sibling = $3;
							$$ = $1;
						}
						else
						{
							$$ = $3;
						}
					}
					| param
					{ $$ = $1;}
					;

/* 9 */
param				: type_specifier ID
					{
						$$ = newDeclNode(ParamK);
						$$->child[0] = $1;
						$$->name = popFromNameStack();
						$$->lineno = popFromLineStack();
					}
					| type_specifier ID LSQUAREBRACKET RSQUAREBRACKET
					{
						$$ = newDeclNode(ParamK);
						$$->child[0] = $1;
						$$->name = popFromNameStack();
						$$->lineno = popFromLineStack();
						$$->bWithIndex = 1;
					}
					;

/* 10  */
compound_stmt		: LCURLYBRACKET local_declarations statement_list RCURLYBRACKET
					{
						$$ = newStmtNode(CmpK);
						$$->child[0] = $2;
						$$->child[1] = $3;
						$$->lineno = popFromLineStack();
					}
					;

/* 11 */
local_declarations	: local_declarations var_declaration
					{
						TreeNode *t = $1;
						
						if ( t != NULL)
						{
							while ( t->sibling != NULL )
							{
								t = t->sibling;
							}	
							t->sibling = $2;
							$$ = $1;
						}
						else
						{	
							$$ = $2;
						}
					}
					|	{ 	$$ = NULL;	printf(" New scope!.. \n");	}	/* when empty */
					;

/* 12 */
statement_list		: statement_list statement
					{
						TreeNode *t = $1;
						
						if ( t != NULL )
						{
							while ( t->sibling != NULL )
							{
								t = t->sibling;
							}
							t->sibling = $2;
							$$ = $1;
						}
						else
						{
							$$ = $2;
						}
					} 
					| { $$ = NULL; }		/* when empty */
					;

/* 13 */
statement			: expression_stmt { $$ = $1; }
					| compound_stmt	  { $$ = $1; }
					| selection_stmt  { $$ = $1; }
					| iteration_stmt  { $$ = $1; }
					| return_stmt     { $$ = $1; }
					;
					
/* 14 */
expression_stmt		: expression SEMICOLON
					{
						$$ = $1;
					}
					| SEMICOLON
					;

/* 15 */
selection_stmt		: IF LPAREN expression RPAREN statement 
					{
						$$ = newStmtNode(SelK);
						$$->tokType = IF;
						$$->name = "if";
						$$->bIfWithElse = 0;
						
						$$->child[0] = $3;
						$$->child[1] = $5;
					}
					| IF LPAREN expression RPAREN statement ELSE statement
					{
						$$ = newStmtNode(SelK);
						$$->tokType = IF;
						$$->name = "if else";
						$$->bIfWithElse = 1;
						
						$$->child[0] = $3;
						$$->child[1] = $5;
						$$->child[2] = $7;
					}
					;

/* 16 */
iteration_stmt		: WHILE LPAREN expression RPAREN statement 
					{ 
						$$ = newStmtNode(ItK);
						$$->tokType = WHILE;
						$$->name = "while";
						
						$$->child[0] = $3;
						$$->child[1] = $5;
					}	
					;

/* 17 */
return_stmt			: RETURN SEMICOLON
					{
						$$ = newStmtNode(RetK);
						$$->tokType = RETURN;
						$$->name = "return";
						$$->bReturnWithValue = 0;
						
						$$->child[0] = NULL;
						
					} 
					| RETURN expression SEMICOLON
					{
						$$ = newStmtNode(RetK);
						$$->tokType = RETURN;
						$$->name = "return with value";
						$$->bReturnWithValue = 1;
						
						$$->child[0] = $2;
					}
					;

/* 18 */
expression			: var ASSIGN expression
					{
						$$ = newStmtNode(AssignK);
						$$->tokType = ASSIGN;
						$$->name = "assign";
						
						$$->child[0] = $1;
						$$->child[1] = $3;
					} 
					| simple_expression
					{
						$$ = $1;
					}
					;
				
/* 19 */
var					: ID
					{
						$$ = newExpNode(IdK);
						$$->tokType = ID;
						$$->bWithIndex = 0;
						$$->bDataType = Integer;
						
						$$->name = popFromNameStack();
						$$->lineno = popFromLineStack();
					}
					| ID LSQUAREBRACKET expression RSQUAREBRACKET
					{
						$$ = newExpNode(IdK);
						$$->tokType = ID;
						$$->bWithIndex = 1;
						$$->bDataType = Integer;
						
						$$->name = popFromNameStack();
						$$->lineno = popFromLineStack();
						$$->child[0] = $3;
					}
					;

/* 20 */
simple_expression	: additive_expression relop additive_expression
					{
						$$ = newExpNode(CalcK);
						$$->name = copyString($2->name);
						$$->tokType = $2->tokType;
						free($2);
						
						$$->child[0] = $1;
						$$->child[1] = $3;
					}
					| additive_expression 
					{
						$$ = $1;
					}
					;

/* 21 */
relop				: LTEQ  { $$ = newOpNode(RelOpK); $$->tokType = LTEQ; $$->name = "<="; }
					| LT	{ $$ = newOpNode(RelOpK); $$->tokType = LT;   $$->name = "<";  } 
					| GT	{ $$ = newOpNode(RelOpK); $$->tokType = GT;   $$->name = ">"; }
					| GTEQ	{ $$ = newOpNode(RelOpK); $$->tokType = GTEQ; $$->name = ">="; }
					| EQUAL	{ $$ = newOpNode(RelOpK); $$->tokType = EQUAL; $$->name = "=="; }
					| NOTEQ	{ $$ = newOpNode(RelOpK); $$->tokType = NOTEQ; $$->name = "!=";}
					;

/* 22 */
additive_expression : additive_expression addop term 
					{
						$$ = newExpNode(CalcK); 
						$$->tokType = $2->tokType;
						$$->name = copyString($2->name);
						free($2);
						
						$$->child[0] = $1;
						$$->child[1] = $3;
					}
					| term
					{
						$$ = $1;
					}
					;

/* 23 */
addop				: PLUS { $$ = newOpNode(MathOpK); $$->tokType = PLUS; $$->name = "+"; }
					| MINUS	{ $$ = newOpNode(MathOpK); $$->tokType = MINUS; $$->name = "-"; }
					;

/* 24 */
term				: term mulop factor
					{
						$$ = newExpNode(CalcK);
						$$->tokType = $2->tokType;
						$$->name = copyString($2->name);
						free($2);
						
						$$->child[0] = $1;						
						$$->child[1] = $3;
					} 
					| factor
					{
						$$ = $1;
					}
					;
				
/* 25 */
mulop				: TIMES		{ $$ = newOpNode(MathOpK); $$->tokType = TIMES; $$->name = "*"; }
					| DIVISION	{ $$ = newOpNode(MathOpK); $$->tokType = DIVISION; $$->name = "/"; }
					;
			
/* 26 */
factor				: LPAREN expression RPAREN 
					{
						$$ = $2;
					}
					| var
					{
						$$ = $1;
					}
					| call
					{
						$$ = $1;
					}
					| NUM
					{
						TreeNode *numberNode = newExpNode(ConstK);
						numberNode->value = popFromNumberStack();
						numberNode->bDataType = Integer;
						
						$$ = numberNode;
					}
					;

/* 27 */
call				: ID LPAREN args RPAREN
					{
						$$ = newStmtNode(CallK);
						$$->name = popFromNameStack();
						$$->lineno = popFromLineStack();
						$$->child[0] = $3;
					}
					;

/* 28 */
args				: arg_list 
					{
						$$ = $1;	
					}
					| { $$ = NULL; }
					;

/* 29 */
arg_list			: arg_list COMMA expression
					{
						
						TreeNode *t = $1;
						
						if ( t != NULL )
						{
							while( t->sibling != NULL)
							{
								t = t->sibling;
							}
							t->sibling = $3;
							$$ = $1;
						}
						else
						{
							$$ = $3;
						}
					}
					| expression
					{
						$$ = $1;
					}
					;
%%

void pushToNameStack(char *str)
{
	nameStack[nameStackIndex++] = str;
}

void pushToLineStack(int noLine)
{
	lineStack[lineStackIndex++] = noLine;
}

void pushToNumberStack(int number)
{
	numberStack[numberStackIndex++] = number;
}

char *popFromNameStack()
{
	return nameStack[--nameStackIndex];	
}
int popFromLineStack()
{
	return lineStack[--lineStackIndex];
}

int popFromNumberStack()
{
	return numberStack[--numberStackIndex];
}

int yyerror(char * message)
{
	fprintf(listing,"Syntax error at line %d: %s\n",lineno,message);
  	fprintf(listing,"Current token: ");
  	printToken(yychar,tokenString);
  	Error = TRUE;
  	
  	return 0;
}

/* yylex calls getToken to make Yacc/Bison output
 * compatible with ealier versions of the TINY scanner
 */
static int yylex(void)
{
	TokenType token = getToken();
	if ( token == ID )
	{
		pushToNameStack (copyString(tokenString));
		pushToLineStack	(lineno);
	}
	else if ( token == NUM )
	{
		pushToNumberStack (atoi(tokenString));
	}
	else if (token == LCURLYBRACKET)
	{
		pushToLineStack(lineno);
	}
	return token;
}

TreeNode * parse(void)
{ 
	yyparse();
	return savedTree;
}

