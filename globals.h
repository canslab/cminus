/****************************************************/
/* File: globals.h                                  */
/* Yacc/Bison Version                               */
/* Global types and vars for TINY compiler          */
/* must come before other include files             */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* Yacc/Bison generates internally its own values
 * for the tokens. Other files can access these values
 * by including the tab.h file generated using the
 * Yacc/Bison option -d ("generate header")
 *
 * The YYPARSER flag prevents inclusion of the tab.h
 * into the Yacc/Bison output itself
 */

#ifndef YYPARSER

/* the name of the following file may change */
#include "y.tab.h"

/* ENDFILE is implicitly defined by Yacc/Bison,
 * and not included in the tab.h file
 */
#define ENDFILE 0

#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

/* MAXRESERVED = the number of reserved words */
#define MAXRESERVED 8

/* Yacc/Bison generates its own integer values
 * for tokens
 */
typedef int TokenType;

extern FILE* source; /* source code text file */
extern FILE* listing; /* listing output text file */
extern FILE* code; /* code text file for TM simulator */

extern int lineno; /* source line number for listing */

/**************************************************/
/***********   Syntax tree for parsing ************/
/**************************************************/

typedef enum
{
	StmtK,		// statement node
	DecK,		// declaration node
	ExpK,		// expression node
	OpK			// operator node
} NodeKind;

////////////////////////////////////////////////////

typedef enum
{
	CmpK,		// compound statement
	SelK,		// if statement
	ItK,		// while statement
	RetK,		// return statement
	CallK,		// call statement
	AssignK
} StmtKind;

typedef enum
{
	VarK,	// varaible declaration
	FunK,	// function declaration
	ParamK,	// parameter declaration
} DeclKind;

typedef enum
{
	ConstK, // Constant expression
	IdK,	// id		expression
	CalcK,	// calculation expression   eg) a + b, a > b
	TypeK,

} ExpKind;

typedef enum
{
	RelOpK,	// >=, <=, ...
	MathOpK	// +, -, *, /
} OpKind;

/* ExpType is used for type checking */
typedef enum {Void,Integer} ExpType;

#define MAXCHILDREN 3

typedef struct treeNode
{
	struct treeNode * child[MAXCHILDREN];
	struct treeNode * sibling;
	int lineno;

	NodeKind nodekind;

	union
	{
		DeclKind kindInDecl;
		StmtKind kindInStmt;
		ExpKind kindInExp;
		OpKind	kindInOp;
	} detailKind;

	// it indicates attributes
	TokenType tokType;				// it contains one of ( PLUS, MINUS, ...... )
	int value;						// value
	char *name;						// ex) name of the variable


	int bIfWithElse;				// it distinguishes "if" from "if-else"
	int bReturnWithValue;
	int bWithIndex;					// it distinguishes "normal var" from "array"

	ExpType bDataType;				 /* for type checking of exps */
} TreeNode;

/**************************************************/
/***********   Flags for tracing       ************/
/**************************************************/

/* EchoSource = TRUE causes the source program to
 * be echoed to the listing file with line numbers
 * during parsing
 */
extern int EchoSource;

/* TraceScan = TRUE causes token information to be
 * printed to the listing file as each token is
 * recognized by the scanner
 */
extern int TraceScan;

/* TraceParse = TRUE causes the syntax tree to be
 * printed to the listing file in linearized form
 * (using indents for children)
 */
extern int TraceParse;

/* TraceAnalyze = TRUE causes symbol table inserts
 * and lookups to be reported to the listing file
 */
extern int TraceAnalyze;

/* TraceCode = TRUE causes comments to be written
 * to the TM code file as code is generated
 */
extern int TraceCode;

/* Error = TRUE prevents further passes if an error occurs */
extern int Error; 
#endif
