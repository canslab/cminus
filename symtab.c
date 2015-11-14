/****************************************************/
/* File: symtab.c                                   */
/* Symbol table implementation for the TINY compiler*/
/* (allows only one symbol table)                   */
/* Symbol table is implemented as a chained         */
/* hash table                                       */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

/* SIZE is the size of the hash table */
#define SIZE 211

/* SHIFT is the power of two used as multiplier
 in hash function  */
#define SHIFT 4

/* the hash function */
static int hash(char * key)
{
	int temp = 0;
	int i = 0;
	while (key[i] != '\0')
	{
		temp = ((temp << SHIFT) + key[i]) % SIZE;
		++i;
	}
	return temp;
}

/* the hash table */
static BucketList hashTable[SIZE];

/* Procedure st_insert inserts line numbers and
 * memory locations into the symbol table
 * loc = memory location is inserted only the
 * first time, otherwise ignored
 */
void st_insert( char * _name, int _scope, int _location, int _type, int _isFunction, int _lineno)
{
	int h = hash(_name);
	BucketList l = hashTable[h];

	while ((l != NULL) && ((strcmp(_name, l->name) != 0) || (_scope != l->scope)))
		l = l->next;

	if (l == NULL) /* variable not yet in table */
	{
		l = (BucketList) malloc(sizeof(struct BucketListRec));
		l->lines = (LineList) malloc(sizeof(struct LineListRec));
		l->name = _name;
		l->lines->lineno = _lineno;
		l->location = _location;
		l->scope = _scope;
		l->type = _type;
		l->isFunction = _isFunction;

		l->lines->next = NULL;
		l->next = hashTable[h];
		hashTable[h] = l;
	}
	else /* found in table, so just add line number */
	{
		LineList t = l->lines;
		while (t->next != NULL)
			t = t->next;
		t->next = (LineList) malloc(sizeof(struct LineListRec));
		t->next->lineno = _lineno;
		t->next->next = NULL;
	}
} /* st_insert */

/* Function st_lookup returns the memory 
 * location of a variable or -1 if not found
 */
int st_lookup(char * _name, int _scope)
{
	int h = hash(_name);
	BucketList l = hashTable[h];

	while ((l != NULL) && ((strcmp(_name, l->name) != 0) || (_scope != l->scope)))
		l = l->next;

	if (l == NULL)
		return -1;
	else
		return l->location;
}

/* Procedure printSymTab prints a formatted 
 * listing of the symbol table contents 
 * to the listing file
 */
void printSymTab(FILE * listing)
{
	int i;
	fprintf(listing, "Variable Name  Type   Location   Scope     Line Numbers\n");
	fprintf(listing, "-------------------------------------------------------\n");

	for (i = 0; i < SIZE; ++i)
	{
		if (hashTable[i] != NULL)
		{
			BucketList l = hashTable[i];
			while (l != NULL)
			{
				LineList t = l->lines;
				fprintf(listing, "%-14s ", l->name);

				if(l->isFunction == 1)
				{
					if(l->type == 1)
					{
						fprintf(listing, "function(int)  ");
					}
					else if (l->type == 0)
					{
						fprintf(listing, "function(void)  ");
					}
				}
				else	// is not function
				{
					fprintf(listing, "variable(int)  ");
				}

				fprintf(listing, "%-8d", l->location);
				fprintf(listing, "%-5d", l->scope);

				while (t != NULL)
				{
					fprintf(listing, "%4d ", t->lineno);
					t = t->next;
				}
				fprintf(listing, "\n");
				l = l->next;
			}
		}
	}
} /* printSymTab */
