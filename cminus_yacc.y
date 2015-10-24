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
declaration_list	: declaration_list declaration	/* declaration_list -> declaration_list declaration */
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
var_declaration		: type_specifier ID { savedName = copyString(tokenString); } SEMICOLON
					{
						$$ = newDeclNode(VarK);
						$$->child[0] = $1;	/* type specifier is the first child */
						
						/*  NOTE I can't ensure that it would be all right.. */
						$$->name = savedName;						
				
					}
					| type_specifier ID { savedName = copyString(tokenString); }
					  LSQUAREBRACKET NUM {savedNumber = atoi(tokenString); } RSQUAREBRACKET SEMICOLON
					{
						$$ = newDeclNode(VarK);
						$$->child[0] = $1;				/* type specifier is the first child */
						$$->name = savedName;			/* assign string of ID to Node */
						
						TreeNode *numberNode = newExpNode(ConstK);	/* subscript index is included in the new number node */
						numberNode->value = savedNumber;			/* subscript index is the contents of the number node*/
						
						$$->child[1] = numberNode;		/* subscript index is the second child*/
					}
					;
			
/* 5 */	
type_specifier  	: INT   { $$ = newExpNode(TypeK); $$->tokType = INT; }
					| VOID	{ $$ = newExpNode(TypeK); $$->tokType = VOID; }
					;

/* 6 */
fun_declaration		: type_specifier ID { savedName = copyString(tokenString); }
					  LPAREN params RPAREN compound_stmt
					{
						$$ = newDeclNode(FunK);
						$$->child[0] = $1;
						$$->name = savedName;
						$$->child[1] = $5;
						$$->child[2] = $7;
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
						$$->name = copyString(tokenString);
					}
					| type_specifier ID { savedName = copyString(tokenString); }
					  LSQUAREBRACKET RSQUAREBRACKET
					{
						$$ = newDeclNode(ParamK);
						$$->child[0] = $1;
						$$->name = savedName;
					}
					;

/* 10  */
compound_stmt		: LCURLYBRACKET local_declarations statement_list RCURLYBRACKET
					{
						$$ = newStmtNode(CmpK);
						$$->child[0] = $2;
						$$->child[1] = $3;
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
						$$->child[0] = $3;
						$$->child[1] = $5;
					}
					| IF LPAREN expression RPAREN statement ELSE statement
					{
						$$ = newStmtNode(SelK);
						$$->child[0] = $3;
						$$->child[1] = $5;
						$$->child[2] = $7;
					}
					;

/* 16 */
iteration_stmt		: WHILE LPAREN expression RPAREN statement 
					{ 
						$$ = newStmtNode(ItK);
						$$->child[0] = $3;
						$$->child[1] = $5;
					}	
					;

/* 17 */
return_stmt			: RETURN SEMICOLON
					{
						$$ = newStmtNode(RetK);
						$$->child[0] = NULL;
					} 
					| RETURN expression SEMICOLON
					{
						$$ = newStmtNode(RetK);
						$$->child[0] = $2;
					}
					;

/* 18 */
expression			: var ASSIGN expression
					{
						$$ = newStmtNode(AssignK);
						$$->tokType = ASSIGN;
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
						$$->name = copyString(tokenString);
					}
					| ID { savedName = copyString(tokenString); } LSQUAREBRACKET expression RSQUAREBRACKET
					{
						$$ = newExpNode(IdK);
						$$->name = savedName;
						$$->child[0] = $4;
					}
					;

/* 20 */
simple_expression	: additive_expression relop additive_expression
					{
						$$ = newExpNode(CalcK);
						$$->tokType = $2->tokType;
						$$->child[0] = $1;
						$$->child[1] = $3;
					}
					| additive_expression 
					{
						$$ = $1;
					}
					;

/* 21 */
relop				: LTEQ  { $$ = newOpNode(RelOpK); $$->tokType = LTEQ;  }
					| LT	{ $$ = newOpNode(RelOpK); $$->tokType = LT;    } 
					| GT	{ $$ = newOpNode(RelOpK); $$->tokType = GT;    }
					| GTEQ	{ $$ = newOpNode(RelOpK); $$->tokType = GTEQ;  }
					| EQUAL	{ $$ = newOpNode(RelOpK); $$->tokType = EQUAL; }
					| NOTEQ	{ $$ = newOpNode(RelOpK); $$->tokType = NOTEQ; }
					;

/* 22 */
additive_expression : additive_expression addop term 
					{
						$$ = newExpNode(CalcK); 
						$$->tokType = $2->tokType;
						$$->child[0] = $1;
						$$->child[1] = $3;
					}
					| term
					{
						$$ = $1;
					}
					;

/* 23 */
addop				: PLUS { $$ = newOpNode(MathOpK); $$->tokType = PLUS; }
					| MINUS	{ $$ = newOpNode(MathOpK); $$->tokType = MINUS; }
					;

/* 24 */
term				: term mulop factor
					{
						$$ = newExpNode(CalcK);
						$$->child[0] = $1;
						$$->tokType = $2->tokType;
						$$->child[1] = $3;
					} 
					| factor
					{
						$$ = $1;
					}
					;
				
/* 25 */
mulop				: TIMES		{ $$ = newOpNode(MathOpK); $$->tokType = TIMES; }
					| DIVISION	{ $$ = newOpNode(MathOpK); $$->tokType = DIVISION; }
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
						numberNode->value = atoi(tokenString);
						
						$$ = numberNode;
					}
					;

/* 27 */
call				: ID {savedName = copyString(tokenString); } LPAREN args RPAREN
					{
						$$ = newStmtNode(CallK);
						$$->name = savedName;
						$$->child[0] = $4;
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

int yyerror(char * message)
{ fprintf(listing,"Syntax error at line %d: %s\n",lineno,message);
  fprintf(listing,"Current token: ");
  printToken(yychar,tokenString);
  Error = TRUE;
  return 0;
}

/* yylex calls getToken to make Yacc/Bison output
 * compatible with ealier versions of the TINY scanner
 */
static int yylex(void)
{ return getToken();
 }

TreeNode * parse(void)
{ 
	yyparse();
	return savedTree;
}

