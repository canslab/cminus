/****************************************************/
/* File: util.h                                     */
/* Utility functions for the TINY compiler          */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#ifndef _UTIL_H_
#define _UTIL_H_

#include "globals.h"
/* Procedure printToken prints a token 
 * and its lexeme to the listing file
 */
void printToken( TokenType type, const char* tokenString);
/* Function newStmtNode creates a new statement
 * node for syntax tree construction
 */
TreeNode * newStmtNode(StmtKind kind);
/* Function newExpNode creates a new expression 
 * node for syntax tree construction
 */
TreeNode * newDeclNode(DeclKind kind);
/* Function newExpNode creates a new expression
 * node for syntax tree construction
 */
TreeNode * newExpNode(ExpKind kind);
TreeNode * newOpNode(OpKind kind);
/* Function copyString allocates and makes a new
 * copy of an existing string
 */
char * copyString( char * );

/* procedure printTree prints a syntax tree to the 
 * listing file using indentation to indicate subtrees
 */
void printTree( TreeNode * );

#endif
