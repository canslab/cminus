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

#define YYSTYPE type_t
static char * savedName; /* for use in assignments */
static int savedLineNo;  /* ditto */
static TreeNode * savedTree; /* stores syntax tree for later return */
static int yylex(void); // added 11/2/11 to ensure no conflict with lex

static char *nameStack[10];
static int numberStack[10];

typedef union type_t type_t;

union type_t
{
	TreeNode *node;
	char *str;
	int op;
};

extern int yychar;

%}

%token IF RETURN ELSE WHILE VOID INT
%token ID NUM
%token EQUAL NOTEQ ASSIGN LT LTEQ GT GTEQ PLUS MINUS TIMES DIVISION LPAREN RPAREN
%token LCURLYBRACKET RCURLYBRACKET LSQUAREBRACKET RSQUAREBRACKET SEMICOLON COMMA
%token ERROR

%left TIMES DIVISION
%left PLUS MINUS

%% /* Grammar for TINY */


program : declaration_list
			{ savedTree = $1.node; }
		;
		
declaration_list : declaration_list declaration
				{
			    	TreeNode * t = $1.node;
			    	if (t != NULL)
			    	{
						while (t->sibling != NULL)
				    		t = t->sibling;
						t->sibling = $2.node;
						$$.node = $1.node;
			    	}
			    	else $$.node = $2.node;
				}
					
				| declaration
			    { $$.node = $1.node; }
				;

declaration : var_declaration	{ $$.node = $1.node; }
			| fun_declaration 	{ $$.node = $2.node; }
			;

var_declaration : type_specifier ID SEMICOLON
					{
						$$.node = newDeclNode(VarK);
						$$.node.type = $1.str;
						$$.node->name = copyString (tokenString);
					}
				| type_specifier ID 
				{ savedLineNo = lineno; $$.str = copyString (tokenString); }
				LSQUAREBRACKET NUM RSQUAREBRACKET SEMICOLON
				{
					$$.node = newDeclNode(VarK);
					$$.node.type = copyString("Array");
					$$.node->name = $3.str;
					$$.node->lineno = savedLineNo;
					$$.node->val = atoi(
					
				}
						
			
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
{
	 
	return getToken(); 
}

TreeNode * parse(void)
{ yyparse();
  return savedTree;
}

