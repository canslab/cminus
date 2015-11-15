#include "symtab.h"

#include <stdlib.h>
#include <string.h>

#define SHIFT 4

static ScopeListEntity *scopeHashTable[SIZE];

// hash function
static int hash(char *_pszKey)
{
	int temp = 0;
	int i = 0;

	while (_pszKey[i] != '\0')
	{
		temp = ((temp << SHIFT) + _pszKey[i]) % SIZE;
		++i;
	}
	return temp;
}

// it returns the ScopeListEntity that is the parent scope list entity of _pszScope
// example :      _pszScope = "AA:BB" -> return the scopeListEntity of AA
// if _pszScope is the root scope => return NULL
ScopeListEntity* getParentScopeListEntity(char *_pszScope)
{
	int i = 0;
	char *pszTemp = NULL;
	char *pszParentScope = NULL;

	ScopeListEntity *pScopeListEntity = NULL;

	if (strcmp(_pszScope, "") == 0)
	{
		// it means there is no parent scope
		return NULL;
	}

	pszTemp = (char*)malloc(sizeof(char) * (strlen(_pszScope) + 1));
	strcpy(pszTemp, _pszScope);

	i = strlen(pszTemp);
	while (pszTemp[i] != ':')
	{
		pszTemp[i--] = '\0';

	}
	pszTemp[i] = '\0';

	pszParentScope = pszTemp;
	pScopeListEntity = scopeHashTable[hash(pszParentScope)];

	while ((pScopeListEntity != NULL) && (strcmp(pScopeListEntity->pszName, pszParentScope) != 0))
	{
		pScopeListEntity = pScopeListEntity->pNext;
	}

	if (pScopeListEntity == NULL)
	{
		pScopeListEntity = getParentScopeListEntity(pszParentScope);
	}
	free(pszTemp);

	return pScopeListEntity;
}

void st_insert_atScope(ScopeListEntity *_pScope, char *_pszName, int _bType, int _nLocation, int _bArray, int _nLineno)
{
	int nIndex = hash(_pszName);
	BucketListEntity *pBucketEntity = (_pScope->pBucketListEntities)[nIndex];


	while ((pBucketEntity != NULL) && (strcmp(_pszName, pBucketEntity->pszName) != 0))
	{
		/* loop until find the appropriate bucket entity */
		pBucketEntity = pBucketEntity->pNext;
	}

	if (pBucketEntity == NULL)	/* if there is no name is Symbol Table */
	{
		// allocate the new one(bucket entity)
		BucketListEntity *pNewBucketEntity = (BucketListEntity*)malloc(sizeof(BucketListEntity));

		// assign name
		pNewBucketEntity->pszName = (char*)malloc(sizeof(char) * (strlen(_pszName) + 1));
		strcpy(pNewBucketEntity->pszName, _pszName);
		
		// assign line
		pNewBucketEntity->pLineList = (LineListEntity*)malloc(sizeof(LineListEntity));
		pNewBucketEntity->pLineList->pNext = NULL;
		pNewBucketEntity->pLineList->nLine = _nLineno;

		// assign Type
		pNewBucketEntity->bType = _bType;
		
		// assign nLocation
		pNewBucketEntity->nMemloc = _nLocation;

		// assign bArray
		pNewBucketEntity->bArray = _bArray;

		// assign 
		pNewBucketEntity->pNext = (_pScope->pBucketListEntities)[nIndex];
		(_pScope->pBucketListEntities)[nIndex] = pNewBucketEntity;
	}
	else /* found in the symbol table*/
	{
		LineListEntity *pTemp = pBucketEntity->pLineList;
		while (pTemp->pNext != NULL)
		{
			pTemp = pTemp->pNext;
		}
		pTemp->pNext = (LineListEntity*)malloc(sizeof(LineListEntity));
		pTemp->pNext->nLine = _nLineno;
		pTemp->pNext->pNext = NULL;
	}
}
void st_insert_atCharScope(char *_pszScope, char *_pszName, int _bType, int _nLocation, int _bArray, int _nLineno)
{
	int nIndex = hash(_pszScope);
	ScopeListEntity *pScopeListEntity = scopeHashTable[nIndex];

	while ((pScopeListEntity != NULL) && (strcmp(pScopeListEntity->pszName, _pszScope) != 0))
	{
		pScopeListEntity = pScopeListEntity->pNext;
	}

	if (pScopeListEntity == NULL) /* this scope list entity doesn't exist. */
	{
		pScopeListEntity = (ScopeListEntity*)malloc(sizeof(ScopeListEntity));

		// initialize all values to 0
		memset(pScopeListEntity, 0, sizeof(ScopeListEntity));
		
		pScopeListEntity->pParent = getParentScopeListEntity(_pszScope);
		pScopeListEntity->pszName = (char*)malloc(sizeof(char) * (strlen(_pszScope) + 1));
		strcpy(pScopeListEntity->pszName, _pszScope);
		
		pScopeListEntity->pNext = scopeHashTable[nIndex];
		scopeHashTable[nIndex] = pScopeListEntity;
	}

	st_insert_atScope(pScopeListEntity, _pszName, _bType, _nLocation, _bArray, _nLineno);
}

// it is only at _pScope that we find _pszName 
BucketListEntity *st_lookup_atScope(ScopeListEntity *_pScope, char *_pszName)
{
	BucketListEntity *pBucketListEntity = NULL;

	if (_pScope == NULL)
	{
		return NULL;
	}
	
	pBucketListEntity = (_pScope->pBucketListEntities)[hash(_pszName)];
	while ((pBucketListEntity != NULL) && (strcmp(pBucketListEntity->pszName, _pszName) != 0))
	{
		pBucketListEntity = pBucketListEntity->pNext;
	}

	if (pBucketListEntity == NULL)
	{
		return NULL;
	}
	else
	{
		return pBucketListEntity;
	}
}

// it is not only at _pszScope but also at its parent's path recursively
BucketListEntity *st_lookup_atCharScope(char *_pszScope, char *_pszName)
{
	int nIndex = hash(_pszScope);

	ScopeListEntity *pScopeListEntity = scopeHashTable[nIndex];
	BucketListEntity *pRetBucketListEntity = NULL;

	while ((pScopeListEntity != NULL) && (strcmp(pScopeListEntity->pszName, _pszScope) != 0))
	{
		pScopeListEntity = pScopeListEntity->pNext;
	}

	if (pScopeListEntity == NULL)
	{
		// find the parent 
		ScopeListEntity *pParentScopeListEntity = getParentScopeListEntity(_pszScope);
		if (pParentScopeListEntity == NULL)
		{
			return NULL;
		}

		// find that _pszName in the Parent Scope List Entity.
		return st_lookup_atCharScope(pParentScopeListEntity->pszName, _pszName);
	}

	pRetBucketListEntity = st_lookup_atScope(pScopeListEntity, _pszName);
	if (pRetBucketListEntity == NULL)
	{
		ScopeListEntity *pParentScopeListEntity = getParentScopeListEntity(_pszScope);
		if (pParentScopeListEntity == NULL)
		{
			return NULL;
		}
		else
		{
			return st_lookup_atCharScope(pParentScopeListEntity->pszName, _pszName);
		}
	}
	else
	{ 
		return pRetBucketListEntity;
	}
}

// it returns memory location for _pszName at _pszScope
int st_get_location(char *_pszScope, char *_pszName)
{
	BucketListEntity *bucketListEntity = st_lookup_atCharScope(_pszScope, _pszName);


	if (bucketListEntity == NULL)
	{
		return -1;
	}
	else
	{
		return bucketListEntity->nMemloc;
	}
}

// it is only at _pszScope that we find _pszName
BucketListEntity *st_lookup_at_now_scope(char *_pszScope, char *_pszName)
{
	int nIndex = hash(_pszScope);
	ScopeListEntity *pScopeListEntity = scopeHashTable[nIndex];

	while ((pScopeListEntity != NULL) && (strcmp(pScopeListEntity->pszName, _pszScope) != 0))
	{
		pScopeListEntity = pScopeListEntity->pNext;
	}

	if (pScopeListEntity == NULL)
	{
		return NULL;
	}
	
	return st_lookup_atScope(pScopeListEntity, _pszName);
}

/* It checks whether the variable whose name is _pszName and member of the _pszScope is array or not*/
int isArray(char *_pszScope, char *_pszName)
{
	BucketListEntity *pBucketListEntity = st_lookup_atCharScope(_pszScope, _pszName);
	if (pBucketListEntity == NULL)
	{
		return -1;
	}
	
	return pBucketListEntity->bArray;
}


void addLine(char *_pszScope, char *_pszName, int _nLineno)
{
	BucketListEntity *pBucketListEntity = NULL;
	LineListEntity *pLineListEntity = NULL;
	ScopeListEntity *pScopeListEntity = scopeHashTable[hash(_pszScope)];

	// iterate until you find the appropriate ScopeListEntity
	while ((pScopeListEntity != NULL) && (strcmp(_pszScope, pScopeListEntity->pszName) != 0))
	{
		pScopeListEntity = pScopeListEntity->pNext;
	}
	
	// if there doesn't exist ScopeListEntity, get the parent scope list entity
	if (pScopeListEntity == NULL)
	{
		pScopeListEntity = getParentScopeListEntity(_pszScope);
	}

	/* find the appropriate bucket list entity */
	while (pScopeListEntity)
	{
		pBucketListEntity = (pScopeListEntity->pBucketListEntities)[hash(_pszName)];
		while ((pBucketListEntity != NULL) && (strcmp(_pszName, pBucketListEntity->pszName) != 0))
		{
			pBucketListEntity = pBucketListEntity->pNext;
		}

		if (pBucketListEntity == NULL)	/* if there isn't not an bucket */
		{
			/* go to the parent scope */
			pScopeListEntity = pScopeListEntity->pParent;
		}
		else
		{
			/*if sucess, break this loop */
			break;
		}
	}
	pLineListEntity = pBucketListEntity->pLineList;
	while (pLineListEntity->pNext != NULL)
	{
		pLineListEntity = pLineListEntity->pNext;
	}
	pLineListEntity->pNext = (LineListEntity*)malloc(sizeof(LineListEntity));
	pLineListEntity->pNext->nLine = _nLineno;
	pLineListEntity->pNext->pNext = NULL;
}

void printSymbolTable_AtScope(FILE *_pListing, ScopeListEntity *_pScopeListEntity)
{
	
	int i;
	fprintf(_pListing, "Variable Name  Variable Type   Location   LineNumbers    \n");
	fprintf(_pListing, "-------------  -------------   --------   ---------------\n");
	
	for (i = 0; i < SIZE; i++)
	{
		if ((_pScopeListEntity->pBucketListEntities)[i] != NULL)
		{
			BucketListEntity *pTempBucketListEntity = (_pScopeListEntity->pBucketListEntities[i]);
			
			while (pTempBucketListEntity != NULL)
			{
				LineListEntity *pTempLineListEntity = pTempBucketListEntity->pLineList;
				fprintf(_pListing, "%-16s", pTempBucketListEntity->pszName);
				fprintf(_pListing, "%-16s", (pTempBucketListEntity->bType == 1)?"int":"void");
				fprintf(_pListing, "%-7d", pTempBucketListEntity->nMemloc);
				while (pTempLineListEntity != NULL)
				{
					fprintf(_pListing, "%4d", pTempLineListEntity->nLine);
					pTempLineListEntity = pTempLineListEntity->pNext;
				}
				fprintf(_pListing, "\n");
				pTempBucketListEntity = pTempBucketListEntity->pNext;
			}
		}
	}
}

void printSymbolTable(FILE *_pListing)
{
	int i = 0;
	for (i = 0; i < SIZE; ++i)
	{
		if (scopeHashTable[i] != NULL)
		{
			ScopeListEntity *pTempScopeListEntity = scopeHashTable[i];

			while (pTempScopeListEntity != NULL)
			{
				fprintf(_pListing, "Scope name : %s\n", pTempScopeListEntity->pszName);
				fprintf(_pListing, "----------------------------------------------------------\n");
				printSymbolTable_AtScope(_pListing, pTempScopeListEntity);
				fprintf(_pListing, "----------------------------------------------------------\n\n\n\n");
				pTempScopeListEntity = pTempScopeListEntity->pNext;
			}
		}
	}
}
