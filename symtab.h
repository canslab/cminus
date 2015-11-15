#ifndef _SYMTAB_H_
#define _SYMTAB_H_
#define SIZE 211

#include <stdio.h>

typedef struct LineListEntity
{
	int nLine;
	struct LineListEntity *pNext;
}LineListEntity;

typedef struct BucketListEntity
{
	char *pszName;

	struct BucketListEntity *pNext;
	LineListEntity *pLineList;

	int nMemloc;
	int bArray;
	int bType;

} BucketListEntity;

typedef struct ScopeListEntity
{
	char *pszName;
	BucketListEntity *pBucketListEntities[SIZE];
	struct ScopeListEntity *pParent;
	struct ScopeListEntity *pNext;
} ScopeListEntity;


// it returns the ScopeListEntity that is the parent scope list entity of _pszScope
// example :      _pszScope = "AA:BB" -> return the scopeListEntity of AA
ScopeListEntity* getParentScopeListEntity(char *_pszScope);

// insert functions
void st_insert_atScope(ScopeListEntity *_pScope, char *_pszName, int _bType, int _nLocation ,int _bArray,int _nLineno);
void st_insert_atCharScope(char *_pszScope, char *_pszName, int _bType, int _nLocation, int _bArray, int _nLineno);

// it is only at _pScope that we find _pszName 
BucketListEntity *st_lookup_atScope(ScopeListEntity *_pScope, char *_pszName);

// it is not only at _pszScope but also at its parent's path recursively
BucketListEntity *st_lookup_atCharScope(char *_pszScope, char *_pszName);

// it is only at _pszScope that we find _pszName
BucketListEntity *st_lookup_at_now_scope(char *_pszScope, char *_pszName);

// it returns memory location for _pszName at _pszScope
int st_get_location(char *_pszScope, char *_pszName);

/* It checks whether the variable whose name is _pszName and member of the _pszScope is array or not*/
int isArray(char *_pszScope, char *_pszName);

void addLine(char *_pszScope, char *_pszName, int _nLineno);

/* print Symbol Ttable of the given scope*/
void printSymbolTable_AtScope(FILE *_pListing, ScopeListEntity *_pScopeListEntity);

/* print Symbol Table */
void printSymbolTable(FILE *_pListing);

#endif