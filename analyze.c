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
static int gLocation = 0;
static char *gScope = "";

static int gbNowFunctionDeclaration = 0;

char * getNewScope(TreeNode * t)
{
	char *result = NULL;

	if (t->nodekind == DecK)
	{
		if ((t->detailKind.kindInDecl) == FunK)
		{
			result = (char *)malloc(sizeof(char)* (strlen(gScope) + strlen(t->name) + 3));
			sprintf(result, "%s:%s", gScope, t->name);
			gLocation++;
		}
	}
	else if (t->nodekind == StmtK)
	{
		if ((t->detailKind).kindInStmt == CmpK)
		{
			/* normal compound statement, ( if, while, just {}, etc.. )*/
			if (gbNowFunctionDeclaration == 0)
			{
				result = (char *)malloc(sizeof(char) * (strlen(gScope) + 12));
				sprintf(result, "%s:%d\0", gScope, t->lineno);
				gLocation++;
			}
			/* when you declare function, the scope of first tcompound statment is as same as function declartion's scope*/
			else
			{
				result = NULL;
				// set gbNowFunctionDeclaration to 0
				gbNowFunctionDeclaration = 0;
			}


		}
	}

	if (result == NULL)
	{
		result = (char *) malloc(sizeof(char) * (strlen(gScope) + 2));
		strcpy(result, gScope);
	}

	return result;
}


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

		preProc(t);
		// gScope = 1, gLocation = 1
		{
			int i;
			// backup the scope before getting new scope
			char *scopeBackUp = gScope;

			// get new scope and save it to gScope
			gScope = getNewScope(t);

			for (i = 0; i < MAXCHILDREN; i++)
				traverse(t->child[i], preProc, postProc);

			free(gScope);
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
	case StmtK:
		if ((t->detailKind).kindInStmt ==  CallK)
		{
			BucketListEntity *pTemp = st_lookup_at_now_scope("", t->name);

			if (pTemp == NULL)
			{
				fprintf(listing,"function didn't declared.. \n");
				exit(-2);
			}
			else
			{
				addLine("", t->name, t->lineno);
			}
		}
		break;

	case ExpK:
		if ((t->detailKind).kindInExp == IdK)
		{
			BucketListEntity *pTemp = st_lookup_atCharScope(gScope, t->name);
			if (pTemp == NULL)
			{
				fprintf(listing, "id didn't declared.. \n");
				exit(-2);
			}
			else
			{
				addLine(gScope, t->name, t->lineno);
			}
		}
		break;

	case DecK:
		switch((t->detailKind).kindInDecl)
		{
		/**/
		case VarK:
		{
			BucketListEntity *pTemp = st_lookup_at_now_scope(gScope, t->name);

			if (pTemp == NULL)	/* there is no t->name at Symbol Table of gScope*/
			{
				if (t->bWithIndex == 1)
				{
					st_insert_atCharScope(gScope, t->name, 1, gLocation, 1, t->lineno);
				}
				else
				{
					st_insert_atCharScope(gScope, t->name, 1, gLocation, 0, t->lineno);
				}
			}
			else
			{
				fprintf(listing, "Var Decl Error, Already Declared.!\n");
				exit(-2);
			}
		}
			break;
		case FunK:
		{
			// Find the symbol at global scope()
			BucketListEntity *pTemp = st_lookup_at_now_scope("", t->name);

			// If function declaration is not in the symbol table.
			if (pTemp == NULL)
			{
				gbNowFunctionDeclaration = 1;
				if (t->bDataType == Integer)
				{
					st_insert_atCharScope("",t->name, 1, gLocation,0,t->lineno);
				}
				else
				{
					st_insert_atCharScope("", t->name, 0, gLocation, 0, t->lineno);
				}
			}
			else
			{
				fprintf(listing, "Func Decl Error, Already Declared!\n");
				exit(-2);
			}
		}
			break;
		case ParamK:	// Parameter Declaration.
		{
			BucketListEntity *pTemp = st_lookup_at_now_scope(gScope, t->name);

			if (pTemp == NULL)
			{
				if (t->bWithIndex == 1)
				{
					st_insert_atCharScope(gScope, t->name, 1, gLocation, 1, t->lineno);
				}
				else
				{
					st_insert_atCharScope(gScope, t->name, 1, gLocation, 0, t->lineno);
				}
			}
			else
			{
				fprintf(listing, "Parameter Decl Error, Already Declared..\n");
			}
		}
			break;
		default:
			break;
		}
		break;
	}
}

static void insertBultinFunctions(TreeNode **_pSyntaxtree)
{
	TreeNode *inputFunctionNode = NULL;
	TreeNode *outputFunctionNode = NULL;

	inputFunctionNode = newDeclNode(FunK);
	outputFunctionNode = newDeclNode(FunK);

	inputFunctionNode->name = copyString("input");
	outputFunctionNode->name = copyString("output");

	inputFunctionNode->lineno = 0;
	outputFunctionNode->lineno = 0;

	inputFunctionNode->detailKind.kindInDecl = FunK;
	outputFunctionNode->detailKind.kindInDecl = FunK;

	inputFunctionNode->bReturnWithValue = 1;
	outputFunctionNode->bReturnWithValue = 0;

	inputFunctionNode->bDataType = Integer;
	outputFunctionNode->bDataType = Void;

	outputFunctionNode->child[0] = newDeclNode(ParamK);
	outputFunctionNode->child[0]->name = copyString("arg");
	outputFunctionNode->child[0]->lineno = 0;
	outputFunctionNode->child[0]->bDataType = Integer;
	outputFunctionNode->child[0]->sibling = NULL;

	inputFunctionNode->sibling = *_pSyntaxtree;
	*_pSyntaxtree = inputFunctionNode;

	outputFunctionNode->sibling = *_pSyntaxtree;
	*_pSyntaxtree = outputFunctionNode;
}

/* Function buildSymtab constructs the symbol 
 * table by preorder traversal of the syntax tree
 */
void buildSymtab(TreeNode * syntaxTree)
{
	// input & output already in symbol table
	insertBultinFunctions(&syntaxTree);

	traverse(syntaxTree, insertNode, nullProc);

	if (TraceAnalyze)
	{
		fprintf(listing, "\nSymbol table:\n\n");
		printSymbolTable(listing);
	}

	if (st_lookup_atCharScope("", "main") == NULL)
	{
		fprintf(listing, "\nThere should be main function !\n");
		exit(-4);
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
	switch (t->nodekind)
	{
	case ExpK:
		// +, *, -, /, <=, >=, ==, !=, >, <
		if ((t->detailKind).kindInExp == CalcK)
		{
			if ( ((t->child[0]->bDataType) == Integer) && ((t->child[1]->bDataType) == Integer))
			{
				t->bDataType = Integer;
			}
			else
			{
				typeError(t, "Calculation Error, There is at least one void type at calculation expression");
				exit(-10);
			}
		}

		break;

	case StmtK:
		/* rvalue should be Integer */
		if ((t->detailKind).kindInStmt == AssignK)
		{
			TreeNode *rightChild = t->child[1];

			if (rightChild->bDataType != Integer)
			{
				typeError(t, "Assign statement Error, rvalue should be Integer!");
				exit(-10);
			}
			else
			{
				t->bDataType = rightChild->bDataType;
			}
		}
		else if((t->detailKind).kindInStmt == CallK)
		{
			BucketListEntity *pFunctionCall = st_lookup_atCharScope("", t->name);

			int bTypeOfFunctionCall = pFunctionCall->bType;

			if (bTypeOfFunctionCall == 1)	/* if the type of return value is Integer */
			{
				t->bDataType = Integer;
			}
			else
			{
				t->bDataType = Void;
			}
		}
		break;
	default:
		break;
	}
}

/* Procedure typeCheck performs type checking 
 * by a postorder syntax tree traversal
 */
void typeCheck(TreeNode * syntaxTree)
{
	traverse(syntaxTree, nullProc, checkNode);
}
