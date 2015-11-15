/****************************************************/
/* File: util.c                                     */
/* Utility function implementation                  */
/* for the TINY compiler                            */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "util.h"
#include "globals.h"

/* Procedure printToken prints a token 
 * and its lexeme to the listing file
 */
void printToken(TokenType token, const char* tokenString)
{
	switch (token)
	{
	case ELSE:
	case IF:
	case INT:
	case RETURN:
	case VOID:
	case WHILE:
		fprintf(listing, "reserved word: %s\n", tokenString);
		break;
	case ASSIGN:						/* = */
		fprintf(listing, "=\n");
		break;
	case LT:
		fprintf(listing, "<\n");		/* < */
		break;
	case LTEQ:
		fprintf(listing, "<=\n");		/* <= */
		break;
	case GT:
		fprintf(listing, ">\n");		/* > */
		break;
	case GTEQ:							/* >= */
		fprintf(listing, ">=\n");
		break;
	case EQUAL:							/* == */
		fprintf(listing, "==\n");
		break;
	case NOTEQ:							/* != */
		fprintf(listing, "!=\n");
		break;
	case LPAREN:						/* ( */
		fprintf(listing, "(\n");
		break;
	case RPAREN:						/* ) */
		fprintf(listing, ")\n");
		break;
	case LSQUAREBRACKET:				/* [ */
		fprintf(listing, "[\n");
		break;
	case RSQUAREBRACKET:				/* ] */
		fprintf(listing, "]\n");
		break;
	case LCURLYBRACKET:					/* { */
		fprintf(listing, "{\n");
		break;
	case RCURLYBRACKET:					/* } */
		fprintf(listing, "}\n");
		break;
	case COMMA:							/* , */
		fprintf(listing, ",\n");
		break;
	case SEMICOLON:						/* ; */
		fprintf(listing, ";\n");
		break;
	case PLUS:							/* + */
		fprintf(listing, "+\n");
		break;
	case MINUS:							/* - */
		fprintf(listing, "-\n");
		break;
	case TIMES:							/* * */
		fprintf(listing, "*\n");
		break;
	case DIVISION:						/* / */
		fprintf(listing, "/\n");
		break;
	case ENDFILE:
		fprintf(listing, "EOF\n");
		break;
	case NUM:
		fprintf(listing, "NUM, val= %s\n", tokenString);
		break;
	case ID:
		fprintf(listing, "ID, name= %s\n", tokenString);
		break;
	case ERROR:
		fprintf(listing, "ERROR: %s\n", tokenString);
		break;
	default: /* should never happen */
		fprintf(listing, "Unknown token: %d\n", token);
	}
}

/* Function newStmtNode creates a new statement
 * node for syntax tree construction
 */
TreeNode * newStmtNode(StmtKind kind)
{
	TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
	int i;
	if (t == NULL)
		fprintf(listing, "Out of memory error at line %d\n", lineno);
	else
	{
		for (i = 0; i < MAXCHILDREN; i++)
			t->child[i] = NULL;
		t->sibling = NULL;
		t->nodekind =  StmtK;
		t->detailKind.kindInStmt = kind;
		t->lineno = lineno;
		t->bIfWithElse = -1;
		t->bReturnWithValue = -1;
	}
	return t;
}

/* Function newExpNode creates a new expression 
 * node for syntax tree construction
 */
TreeNode * newDeclNode(DeclKind kind)
{
	TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
	int i;
	if (t == NULL)
		fprintf(listing, "Out of memory error at line %d\n", lineno);
	else
	{
		for (i = 0; i < MAXCHILDREN; i++)
			t->child[i] = NULL;
		t->sibling = NULL;
		t->nodekind = DecK;
		t->detailKind.kindInDecl = kind;
		t->lineno = lineno;
		t->nArgument = 0;
	}
	return t;
}

TreeNode * newExpNode(ExpKind kind)
{
	TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
		int i;
		if (t == NULL)
			fprintf(listing, "Out of memory error at line %d\n", lineno);
		else
		{
			for (i = 0; i < MAXCHILDREN; i++)
				t->child[i] = NULL;
			t->sibling = NULL;
			t->nodekind = ExpK;
			t->detailKind.kindInExp = kind;
			t->lineno = lineno;
			t->bWithIndex = -1;
		}
		return t;

}

TreeNode * newOpNode(OpKind kind)
{
	TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
		int i;
		if (t == NULL)
			fprintf(listing, "Out of memory error at line %d\n", lineno);
		else
		{
			for (i = 0; i < MAXCHILDREN; i++)
				t->child[i] = NULL;
			t->sibling = NULL;
			t->nodekind = OpK;
			t->detailKind.kindInOp = kind;
			t->lineno = lineno;
		}
		return t;

}
/* Function copyString allocates and makes a new
 * copy of an existing string
 */
char * copyString(char * s)
{
	int n;
	char * t;
	if (s == NULL)
		return NULL;
	n = strlen(s) + 1;
	t = malloc(n);
	if (t == NULL)
		fprintf(listing, "Out of memory error at line %d\n", lineno);
	else
		strcpy(t, s);
	return t;
}

/* Variable indentno is used by printTree to
 * store current number of spaces to indent
 */
static indentno = 0;

/* macros to increase/decrease indentation */
#define INDENT indentno+=2
#define UNINDENT indentno-=2

/* printSpaces indents by printing spaces */
static void printSpaces(void)
{
	int i;
	for (i = 0; i < indentno; i++)
		fprintf(listing, " ");
}

/* procedure printTree prints a syntax tree to the 
 * listing file using indentation to indicate subtrees
 */
void printTree(TreeNode * tree)
{
	int i;
	INDENT;
	while (tree != NULL)
	{
		printSpaces();

		if (tree->nodekind == StmtK)
		{
			switch (tree->detailKind.kindInStmt)
			{
			case SelK:
				if (tree->bIfWithElse == 1)
					fprintf(listing, "If with Else \n");
				else
					fprintf(listing, "If \n");
				break;
			case CmpK:
				fprintf(listing, "Compound statement \n");
				break;
			case AssignK:
				fprintf(listing, "Assignment: \n");
				break;
			case ItK:
				fprintf(listing, "While statement \n");
				break;
			case RetK:
				if (tree->bReturnWithValue == 1)
					fprintf(listing, "Return Statement with value \n");
				else
					fprintf(listing,"Return Statement with no value \n");
				break;
			case CallK:
				fprintf(listing, "Call statement => %s\n", tree->name);
				break;
			default:
				fprintf(listing, "Unknown ExpNode kind\n");
				break;
			}
		}
		else if (tree->nodekind == DecK)
		{
			switch (tree->detailKind.kindInDecl)
			{
			case VarK:
				fprintf(listing, "Var declaration => %s\n", tree->name);
				break;
			case FunK:
				fprintf(listing, "Func declaration => %s\n", tree->name);
				break;
			case ParamK:
				fprintf(listing, "Param Declaration => %s\n", tree->name);
				break;
			default:
				fprintf(listing, "Unknown Declaration kind\n");
				break;
			}
		}
		else if (tree->nodekind == ExpK)
		{
			switch (tree->detailKind.kindInExp)
			{
			case ConstK:
				fprintf(listing, "Constant Expression => %d \n", tree->value);
				break;
			case IdK:
				fprintf(listing, "Id Expression => %s \n", tree->name );
				break;
			case CalcK:
				fprintf(listing, "Calculation Expression => %s \n", tree->name );
				break;
			case TypeK:
				fprintf(listing, "Type-specific Expression => %s \n", tree->name);
				break;
			default:
				fprintf(listing, "Unknown Expression Kind..\n");
				break;
			}
		}
		else if (tree->nodekind == OpK)
		{
			switch (tree->detailKind.kindInOp)
			{
			case RelOpK:
				fprintf(listing, "Relative Comparison Opeartion\n");
				break;
			case MathOpK:
				fprintf(listing, "Math Operation \n");
				break;
			default:
				fprintf(listing, "Unknown Operation Kind \n");
				break;
			}
		}
		else
			fprintf(listing, "Unknown node kind\n");
		for (i = 0; i < MAXCHILDREN; i++)
			printTree(tree->child[i]);
		tree = tree->sibling;
	}
	UNINDENT;
}
