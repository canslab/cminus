/****************************************************/
/* File: analyze.c                                  */
/* Semantic analyzer implementation                 */
/* for the TINY compiler                            */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "symtab.h"
#include "analyze.h"
#include "globals.h"

/* counter for variable memory locations */
static int location = 0;

static int gLocation = 0;
static int gScope = 0;
static int gAccScope = 0;

/* Procedure traverse is a generic recursive 
 * syntax tree traversal routine:
 * it applies preProc in preorder and postProc 
 * in postorder to tree pointed to by t
 */
static void traverse(TreeNode * t, void (*preProc)(TreeNode *), void (*postProc)(TreeNode *))
{
	if (t != NULL)
	{
		int locationBackup = gLocation;
		int scopeBackUp = gScope;

		preProc(t);
		// gScope = 1, gLocation = 1
		{
			int i;
			for (i = 0; i < MAXCHILDREN; i++)
				traverse(t->child[i], preProc, postProc);

			gLocation = locationBackup;
			gScope = scopeBackUp;
		}

		postProc(t);
		traverse(t->sibling, preProc, postProc);
	}
}

/* nullProc is a do-nothing procedure to 
 * generate preorder-only or postorder-only
 * traversals from traverse
 */
static void nullProc(TreeNode * t)
{
	if (t == NULL)
		return;
	else
		return;
}

/* Procedure insertNode inserts 
 * identifiers stored in t into 
 * the symbol table 
 */
static void insertNode(TreeNode * t)
{
	switch (t->nodekind)
	{
	case ExpK:
		if((t->detailKind).kindInExp == IdK)
		{
			if (st_lookup(t->name, gScope) == -1)
			{
				fprintf(listing, " Id should be declared before use! \n");
				exit(-1);
			}
			else
			{
				st_insert(t->name, gScope, gLocation, 1, 0, t->lineno);
			}
		}
		break;

	case StmtK:
		switch((t->detailKind).kindInStmt)
		{
		case CmpK:
			gLocation++;
			break;

		case CallK:
			// call means function call
			// function should be already in the symbol table
			if (st_lookup(t->name, 0) == -1)
			{
				fprintf(listing, "Function should be declared before use!\n");
				exit(-3);
			}
			break;
		}
		break;

	case DecK:
		switch((t->detailKind).kindInDecl)
		{
		case VarK:
			fprintf(listing, "gScope = %d\n", gScope);
			int tempLoc;
			if((tempLoc = st_lookup(t->name,gScope)) != -1)
			{
				if(gLocation != tempLoc)
				{
					st_insert(t->name, gScope, gLocation, 1, 0, t->lineno);
				}
				else
				{
					fprintf(listing, "Var Declaration Error...\n");
					exit(-2);
				}
			}
			else
			{
				st_insert(t->name, gScope, gLocation, 1, 0, t->lineno);
			}
			break;
		case FunK:
			if (st_lookup(t->name, 0) == -1)	// cannot find
			{
				if((t->child[0])->tokType == INT )
				{
					st_insert(t->name, 0, 0, 1, 1, t->lineno );
				}
				else
				{
					st_insert(t->name, 0, 0, 0, 1, t->lineno);
				}
				gAccScope++;
				gScope = gAccScope;
				fprintf(listing, " Acc Scope = %d\n", gAccScope);
			}
			else	// find
			{
				fprintf(listing, "Function Declaration Error...\n");
				exit(-2);
			}
			break;
		case ParamK:
			if (st_lookup(t->name, gScope) == -1)	// cannot find
			{
				if( (t->child[0])->tokType == INT)
				{
					st_insert(t->name, gScope, 1, 1, 0, t->lineno);
				}
			}
			else
			{
				fprintf(listing, "Parameter Decl Error..\n");
				exit(-2);
			}
			break;
		default:
			break;
		}
		break;
	}
}

/* Function buildSymtab constructs the symbol 
 * table by preorder traversal of the syntax tree
 */
void buildSymtab(TreeNode * syntaxTree)
{
	// input & output already in symbol table.
	st_insert("input", 0, 0, 1, 1, 0);
	st_insert("output", 0, 0, 0, 1, 0);

	traverse(syntaxTree, insertNode, nullProc);

	if (TraceAnalyze)
	{
		fprintf(listing, "\nSymbol table:\n\n");
		printSymTab(listing);
	}
}

static void typeError(TreeNode * t, char * message)
{
	fprintf(listing, "Type error at line %d: %s\n", t->lineno, message);
	Error = TRUE;
}

/* Procedure checkNode performs
 * type checking at a single tree node
 */
static void checkNode(TreeNode * t)
{
//	switch (t->nodekind)
//	{
//	case ExpK:
//		switch (t->kind.exp)
//		{
//		case OpK:
//			if ((t->child[0]->type != Integer) || (t->child[1]->type != Integer))
//				typeError(t, "Op applied to non-integer");
//			if ((t->attr.op == EQ) || (t->attr.op == LT))
//				t->type = Boolean;
//			else
//				t->type = Integer;
//			break;
//		case ConstK:
//		case IdK:
//			t->type = Integer;
//			break;
//		default:
//			break;
//		}
//		break;
//	case StmtK:
//		switch (t->kind.stmt)
//		{
//		case IfK:
//			if (t->child[0]->type == Integer)
//				typeError(t->child[0], "if test is not Boolean");
//			break;
//		case AssignK:
//			if (t->child[0]->type != Integer)
//				typeError(t->child[0], "assignment of non-integer value");
//			break;
//		case WriteK:
//			if (t->child[0]->type != Integer)
//				typeError(t->child[0], "write of non-integer value");
//			break;
//		case RepeatK:
//			if (t->child[1]->type == Integer)
//				typeError(t->child[1], "repeat test is not Boolean");
//			break;
//		default:
//			break;
//		}
//		break;
//	default:
//		break;

//	}
}

/* Procedure typeCheck performs type checking 
 * by a postorder syntax tree traversal
 */
void typeCheck(TreeNode * syntaxTree)
{
	traverse(syntaxTree, nullProc, checkNode);
}
