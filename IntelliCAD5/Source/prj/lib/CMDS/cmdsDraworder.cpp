/* G:\ICADDEV\PRJ\LIB\CMDS\DRAWORDER.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */
// THIS FILE HAS BEEN GLOBALIZED!

#include "cmds.h"/*DNT*/
#include "cmdsDraworder.h"
#include "Icad.h"/*DNT*/
#include "IcadApi.h"
#include "IcadDoc.h"
#include "objects.h"
#include "DbSortentsTable.h"

#define _TRACE_DBSORTENTSTABLE_(_pSortentsTable) {\
	db_objhandle _dbSortHandle, _dbHandle; \
	TRACE1("\ntable size - %d", _pSortentsTable->size()); \
	for(_pSortentsTable->resetIterator(); _pSortentsTable->next(_dbSortHandle, _dbHandle);) \
	TRACE2("\n%x %x - %x %x", _dbSortHandle.ToHugeInt(), _dbHandle.ToHugeInt()); \
	}

/*-------------------------------------------------------------------------*//**
Frees list of groups (of type 5 and 331). Special function is required because 
usual function treats group 5 as string but for SORTENTSTABLE it contains name.

@author Denis Petrov
@param pList => the list of groups (of type 5 and 331)
@return 1 for success, 0 else
*//*------------------------------------------------------------------------*/
static int cmd_specialRelease(struct resbuf* pList);

/*-------------------------------------------------------------------------*//**
Goes through list of groups (of type 5 and 331) and extracts entities which 
were selected for changing of redraw order.

@author Denis Petrov
@param entitiesToMove => selection set which contains selected entities
@param pTable => the list of groups (of type 5 and 331) 
@param pToMoveBegin <= start of the list with extracted groups 
@param pToMoveEnd <= end of the list with extracted groups 
@return 1 for success, 0 else
*//*------------------------------------------------------------------------*/
static int cmd_extractEntsToMove(sds_name entitiesToMove, 
						  struct resbuf* pTable, 
						  struct resbuf*& pToMoveBegin, 
						  struct resbuf*& pToMoveEnd);

/*-------------------------------------------------------------------------*//**
Goes through list of groups (until reference entity group not found) with types 
5 and 331 and extracts entities which were selected for changing of redraw order.

@author Denis Petrov
@param entitiesToMove => selection set which contains selected entities
@param pTable => the list of groups (of type 5 and 331) 
@param refEntity => the name of reference entity
@param pRefEntity <= pointer to reference entity group
@param pToMoveBegin <= start of the list with extracted groups 
@param pToMoveEnd <= end of the list with extracted groups 
@return 1 for success, 0 else
*//*------------------------------------------------------------------------*/
static int cmd_extractEntsToMove(sds_name entitiesToMove, 
						  struct resbuf* pTable, 
						  sds_name refEntity, 
						  struct resbuf*& pRefEntity, 
						  struct resbuf*& pToMoveBegin, 
						  struct resbuf*& pToMoveEnd);

/*-------------------------------------------------------------------------*//**
Rearranges list (to form chosen draw order) as combinations of groups 5 and 331
inserting extracted groups.

@author Denis Petrov
@param pBeginPair => pointer to group from which iteration should start
@param pToMovePair => pointer to list of groups to insert
@return 1 for success, 0 else
*//*------------------------------------------------------------------------*/
static int cmd_rearrangeEnts(struct resbuf* pBeginPair, 
					  struct resbuf* pToMovePair);

/*-------------------------------------------------------------------------*//**
Rearranges list (until reference entity group not found) as combinations of 
groups 5 and 331 inserting extracted groups.

@author Denis Petrov
@param pBeginPair => pointer to group from which iteration should start
@param pToInsertPairs => pointer to list of groups to insert
@param pRefPair => pointer to reference entity group
@return 1 for success, 0 else
*//*------------------------------------------------------------------------*/
static int cmd_rearrangeEnts(struct resbuf* pBeginPair, 
					  struct resbuf* pToInsertPairs, 
					  struct resbuf* pRefPair);

short cmd_draworder()
{
	char answer[IC_ACADBUF];
	sds_name entitiesToMove, refEntity;
	sds_point pt;
	int result;
	db_drawing* pDrawing = SDS_CURDWG;
	db_objhandle block;
	resbuf tilemode, cvport;
	SDS_getvar(NULL, DB_QTILEMODE, &tilemode, pDrawing, &SDS_CURCFG, &SDS_CURSES);
	SDS_getvar(NULL, DB_QCVPORT, &cvport, pDrawing, &SDS_CURCFG, &SDS_CURSES);
	if(tilemode.resval.rint == 1 || (tilemode.resval.rint == 0 && cvport.resval.rint != 1))
		block = pDrawing->ret_stockhand(DB_SHI_BLKREC_MSPACE);
	else
		block = pDrawing->ret_stockhand(DB_SHI_BLKREC_PSPACE);

	result = sds_pmtssget(ResourceString(IDC_DRAWORDER_SELECTENTS, "\nSelect entities to change draw order: " ), NULL, NULL, NULL, NULL, entitiesToMove, 0);

	if(result != RTNORM)
		return result;

	if(sds_initget(0, ResourceString(IDC_DRAWORDER_OPTIONS, "Above Under Front Back ~_Above ~_Under ~_Front ~_Back")) != RTNORM)
	{
		sds_ssfree(entitiesToMove);
		return RTERROR;
	}

	result = sds_getkword(ResourceString(IDC_DRAWORDER_OPTIONSPROMPT, "\nEnter entity ordering option [Above/Under/Front/Back] <Back>: "), answer);
	if(result != RTNORM)
	{
		if(result == RTNONE)
			strcpy(answer, "BACK");
		else
		{
			sds_ssfree(entitiesToMove);
			return RTNORM;
		}
	}

	if(stricmp("BACK"/*DNT*/, answer) == 0)
	{
		struct resbuf* pCurPair = NULL;
		struct resbuf* pPairsToMove = NULL;
		struct resbuf* pCurToMove = NULL;

		sds_name sdsTableName;
		cmd_getSortEntsTable(sdsTableName, block, true, &pPairsToMove);
		
		struct resbuf* pSortEntsTable = sds_entget(sdsTableName);
		if(pSortEntsTable == NULL)
		{
			ASSERT(FALSE);
			cmd_specialRelease(pPairsToMove);
			return RTERROR;
		}
		pCurPair = pSortEntsTable;
		while(pCurPair->rbnext != NULL && pCurPair->rbnext->restype != 331)
			pCurPair = pCurPair->rbnext;
		if(pCurPair->rbnext == NULL)
		{
			ASSERT(pPairsToMove != NULL);
			pCurPair->rbnext = pPairsToMove;
		}

		cmd_extractEntsToMove(entitiesToMove, pCurPair, pPairsToMove, pCurToMove);
		
		pCurToMove->rbnext = pCurPair->rbnext;
		pCurPair->rbnext = pPairsToMove;
		cmd_rearrangeEnts(pCurPair, pCurToMove);
		
		result = sds_entmod(pSortEntsTable);
		if(result != RTNORM)
		{
			ASSERT(FALSE);
			cmd_specialRelease(pSortEntsTable);
			return RTERROR;
		}
		cmd_specialRelease(pSortEntsTable);
		
		sds_ssfree(entitiesToMove);
		cmd_redrawall();
		return RTNORM;
	}
	if(stricmp("FRONT"/*DNT*/, answer) == 0)
	{
		struct resbuf* pCurPair = NULL;
		struct resbuf* pPairsToMove = NULL;
		struct resbuf* pCurToMove = NULL;

		sds_name sdsTableName;
		cmd_getSortEntsTable(sdsTableName, block, true, &pPairsToMove);
		
		struct resbuf* pSortEntsTable = sds_entget(sdsTableName);
		if(pSortEntsTable == NULL)
		{
			ASSERT(FALSE);
			cmd_specialRelease(pPairsToMove);
			return RTERROR;
		}
		pCurPair = pSortEntsTable;
		while(pCurPair->rbnext != NULL && pCurPair->rbnext->restype != 331)
			pCurPair = pCurPair->rbnext;
		if(pCurPair->rbnext == NULL)
		{
			ASSERT(pPairsToMove != NULL);
			pCurPair->rbnext = pPairsToMove;
		}

		cmd_extractEntsToMove(entitiesToMove, pCurPair, pPairsToMove, pCurToMove);
		cmd_rearrangeEnts(pCurPair, pPairsToMove, NULL);

		result = sds_entmod(pSortEntsTable);
		if(result != RTNORM)
		{
			ASSERT(FALSE);
			cmd_specialRelease(pSortEntsTable);
			return RTERROR;
		}
		cmd_specialRelease(pSortEntsTable);

		sds_ssfree(entitiesToMove);
		cmd_redrawall();
		return RTNORM;
	}

	while(true)
	{
		result = sds_entsel(ResourceString(IDC_DRAWORDER_SELECTREF, "\nSelect reference entity: " ), refEntity, pt);
		if(result != RTNORM)
		{
			sds_ssfree(entitiesToMove);
			return result;
		}
		bool bFound = false;
		sds_name sdsCurName;
		for(long i = 0; sds_ssname(entitiesToMove, i, sdsCurName) == RTNORM; ++i)
			if(sds_name_equal(refEntity, sdsCurName))
			{
				bFound = true;
				break;
			}
		if(bFound)
			sds_printf(ResourceString(IDC_DRAWORDER_CANNOTSELECT, "\nThis entity is selected for reordering already"));
		else
			break;
	}
	
	struct resbuf* pCurPair = NULL;
	struct resbuf* pPairsToMove = NULL;
	struct resbuf* pCurToMove = NULL;
	
	sds_name sdsTableName;
	cmd_getSortEntsTable(sdsTableName, block, true, &pPairsToMove);
	
	struct resbuf* pSortEntsTable = sds_entget(sdsTableName);
	if(pSortEntsTable == NULL)
	{
		ASSERT(FALSE);
		cmd_specialRelease(pPairsToMove);
		return RTERROR;
	}
	pCurPair = pSortEntsTable;
	while(pCurPair->rbnext != NULL && pCurPair->rbnext->restype != 331)
		pCurPair = pCurPair->rbnext;
	if(pCurPair->rbnext == NULL)
	{
		ASSERT(pPairsToMove != NULL);
		pCurPair->rbnext = pPairsToMove;
		pPairsToMove = NULL;
	}
	
	ASSERT(pPairsToMove == NULL);
	struct resbuf* pRefPair = NULL;
	cmd_extractEntsToMove(entitiesToMove, pCurPair, refEntity, pRefPair, pPairsToMove, pCurToMove);
	
	if(stricmp("ABOVE"/*DNT*/, answer) == 0)
	{
		ASSERT(pRefPair->rbnext->rbnext != NULL && pRefPair->rbnext->rbnext->restype == 5);
		pRefPair = pRefPair->rbnext->rbnext;
		ASSERT(pRefPair->rbnext == NULL || pRefPair->rbnext->restype == 331);
		
		if(pPairsToMove != NULL)
			cmd_rearrangeEnts(pCurPair, pPairsToMove, pRefPair->rbnext ? pRefPair : NULL);
	}
	else
	{
		ASSERT(stricmp("UNDER"/*DNT*/, answer) == 0);
		if(pPairsToMove != NULL)
			cmd_rearrangeEnts(pCurPair, pPairsToMove, pRefPair);
	}
	
	pPairsToMove = pCurToMove = NULL;
	cmd_extractEntsToMove(entitiesToMove, pRefPair, pPairsToMove, pCurToMove);
	
	if(pPairsToMove != NULL)
	{
		pCurToMove->rbnext = pRefPair->rbnext;
		pRefPair->rbnext = pPairsToMove;
		cmd_rearrangeEnts(pRefPair, pCurToMove);
	}
	
	result = sds_entmod(pSortEntsTable);
	if(result != RTNORM)
	{
		ASSERT(FALSE);
		cmd_specialRelease(pSortEntsTable);
		return RTERROR;
	}
	cmd_specialRelease(pSortEntsTable);
	
	sds_ssfree(entitiesToMove);
	cmd_redrawall();
	return RTNORM;
}

int cmd_specialRelease(struct resbuf* pList)
{
	for(struct resbuf* pCurPair = pList; pCurPair; pCurPair = pCurPair->rbnext)
	{
		switch(ic_resval(pCurPair->restype))
		{
		case RTSTR:
			if(pCurPair-> restype != 5)
				IC_FREE(pCurPair->resval.rstring);
			break;
		case RTVOID:
		case RTBINARY:
			IC_FREE(pCurPair->resval.rbinary.buf);
		}
	}
	db_free_resbuf_list(pList);
	return 1;
}

int cmd_rearrangeEnts(struct resbuf* pBeginPair, struct resbuf* pToInsertPairs, struct resbuf* pRefPair)
{
	struct resbuf* pCurPair331 = NULL;
	struct resbuf* pCurToMove = NULL;
	while(pBeginPair->rbnext != (pRefPair ? pRefPair->rbnext : NULL))
	{
		if(pBeginPair->rbnext->restype == 331)
		{
			pBeginPair = pBeginPair->rbnext->rbnext;
			ASSERT(pBeginPair->restype == 5);
		}
		else
		{
			ASSERT(pBeginPair->rbnext->restype == 5);
			
			if(pCurPair331 == NULL)
				pCurPair331 = pBeginPair;
			while(pCurPair331->rbnext != pRefPair && pCurPair331->rbnext->restype != 331)	
				pCurPair331 = pCurPair331->rbnext;
			if(pCurPair331->rbnext != pRefPair)
			{
				pCurToMove = pCurPair331->rbnext;
				pCurPair331->rbnext = pCurPair331->rbnext->rbnext;
			}
			else
			{
				pCurToMove = pToInsertPairs;
				pToInsertPairs = pToInsertPairs->rbnext;
				ASSERT(pCurToMove != NULL);
			}
			pCurToMove->rbnext = pBeginPair->rbnext;
			pBeginPair->rbnext = pCurToMove;
			
			pBeginPair = pBeginPair->rbnext->rbnext;
			ASSERT(pBeginPair->restype == 5);
		}
	}
	ASSERT(pToInsertPairs == NULL);
	return 1;
}

int cmd_rearrangeEnts(struct resbuf* pBeginPair, struct resbuf* pToMovePair)
{
	struct resbuf* pCurToMove = NULL;
	while(pBeginPair->rbnext != NULL)
	{
		if(pBeginPair->rbnext->restype == 331)
		{
			if(pBeginPair->rbnext->rbnext->restype != 5)
			{
				while(pToMovePair->rbnext->restype != 5)
					pToMovePair = pToMovePair->rbnext;
				
				pCurToMove = pToMovePair->rbnext;
				pToMovePair->rbnext = pToMovePair->rbnext->rbnext;
				
				pBeginPair = pBeginPair->rbnext;
				pCurToMove->rbnext = pBeginPair->rbnext;
				pBeginPair->rbnext = pCurToMove;
				pBeginPair = pBeginPair->rbnext;
			}
			else
				break;
		}
	}
	return 1;
}

int cmd_extractEntsToMove(sds_name entitiesToMove, struct resbuf* pTable, sds_name refEntity, struct resbuf*& pRefEntity, struct resbuf*& pToMoveBegin, struct resbuf*& pToMoveEnd)
{
	struct resbuf* pCurPair = pTable;
	while(pCurPair->rbnext != NULL && pCurPair->rbnext->restype != 331)
		pCurPair = pCurPair->rbnext;
	
	pToMoveBegin = NULL;
	pToMoveEnd = NULL;
	pRefEntity = NULL;
	long nEntities, i, nFound = 0;
	sds_sslength(entitiesToMove, &nEntities);
	sds_name sdsCurName;
	while(pCurPair->rbnext != NULL && pCurPair->rbnext->restype == 331)
	{
		if(sds_name_equal(pCurPair->rbnext->resval.rlname, refEntity))
		{
			pRefEntity = pCurPair;
			break;
		}
		if(nFound != nEntities)
		{
			for(i = 0; i < nEntities; ++i)
			{
				sds_ssname(entitiesToMove, i, sdsCurName);
				if(sds_name_equal(pCurPair->rbnext->resval.rlname, sdsCurName))
				{
					++nFound;
					if(pToMoveBegin == NULL)
					{
						pToMoveBegin = pCurPair->rbnext;
						pToMoveEnd = pCurPair->rbnext;
					}
					else
					{
						pToMoveEnd->rbnext = pCurPair->rbnext;
						pToMoveEnd = pToMoveEnd->rbnext;
					}
					pCurPair->rbnext = pCurPair->rbnext->rbnext;
					break;
				}
			}
		}
		else
			if(pRefEntity != NULL)
				break;
		if(pCurPair->rbnext->restype == 331)
			pCurPair = pCurPair->rbnext;
		pCurPair = pCurPair->rbnext;
		ASSERT(pCurPair->restype == 5);
	}
	if(pToMoveEnd != NULL)
		pToMoveEnd->rbnext = NULL;
	else
		return 0;
	return 1;
}

int cmd_extractEntsToMove(sds_name entitiesToMove, struct resbuf* pTable, struct resbuf*& pToMoveBegin, struct resbuf*& pToMoveEnd)
{
	ASSERT(pTable != NULL);

	struct resbuf* pCurPair = pTable;
	while(pCurPair->rbnext != NULL && pCurPair->rbnext->restype != 331)
		pCurPair = pCurPair->rbnext;
	
	pToMoveBegin = NULL;
	pToMoveEnd = NULL;
	long nEntities, i, nFound = 0;
	sds_sslength(entitiesToMove, &nEntities);
	sds_name sdsCurName;
	while(pCurPair->rbnext != NULL && pCurPair->rbnext->restype == 331)
	{
		for(i = 0; i < nEntities; ++i)
		{
			sds_ssname(entitiesToMove, i, sdsCurName);
			if(sds_name_equal(pCurPair->rbnext->resval.rlname, sdsCurName))
			{
				++nFound;
				if(pToMoveBegin == NULL)
				{
					pToMoveBegin = pCurPair->rbnext;
					pToMoveEnd = pCurPair->rbnext;
				}
				else
				{
					pToMoveEnd->rbnext = pCurPair->rbnext;
					pToMoveEnd = pToMoveEnd->rbnext;
				}
				pCurPair->rbnext = pCurPair->rbnext->rbnext;
				break;
			}
		}
		if(nFound == nEntities)
			break;
		if(pCurPair->rbnext->restype == 331)
			pCurPair = pCurPair->rbnext;
		pCurPair = pCurPair->rbnext;
		ASSERT(pCurPair->restype == 5);
	}
	if(pToMoveEnd != NULL)
		pToMoveEnd->rbnext = NULL;
	else
		return 0;
	return 1;
}

int cmd_getSortEntsTable(sds_name sdsTableName, /*const*/db_objhandle& ownerBlock, bool bCreate, struct resbuf** pTableList)
{
	db_drawing* pDrawing = SDS_CURDWG;
	CDbSortentsTable* pSortingTable = NULL;
	sds_name sdsXDictName;
	db_dictionary* pXDict = NULL;
	db_hireflink* pXDictRef = NULL;
	if (pDrawing != NULL)
	{
		if(ownerBlock == pDrawing->ret_stockhand(DB_SHI_BLKREC_MSPACE))
			pXDictRef = pDrawing->ret_xdict(0);
		else
			pXDictRef = pDrawing->ret_xdict(1);
	}

	if(pXDictRef == NULL)
	{
		if(!bCreate)
		{
			sdsTableName[0] = 0;
			sdsTableName[1] = 0;
			return 0;
		}
		// create extension dictionary
		pXDict = new db_dictionary();
		pXDict->set_280(1);	
		pXDict->set_281(1);
		// TODO: set owner block
		// pXDict->m_parentBlock = NULL; or on drawing saving?
		pDrawing->addhanditem(pXDict);
		if(ownerBlock == pDrawing->ret_stockhand(DB_SHI_BLKREC_MSPACE))
			pDrawing->set_xdict(0, DB_SOFT_POINTER, pXDict->RetHandle(), pXDict);
		else
			pDrawing->set_xdict(1, DB_SOFT_POINTER, pXDict->RetHandle(), pXDict);
	}
	else
	{
		ASSERT(pXDictRef->hand != NULL);
		pXDict = ((db_dictionary*)(pXDictRef->ret_hip(pDrawing)));
		pSortingTable = (CDbSortentsTable*)pDrawing->dictsearch(pXDict, "ACAD_SORTENTS", 0);
	}
	if(pSortingTable != NULL)
	{
		sdsTableName[0] = (long)pSortingTable;
		sdsTableName[1] = (long)pDrawing;
		return 1;
	}
	if(!bCreate)
	{
		sdsTableName[0] = 0;
		sdsTableName[1] = 0;
		return 0;
	}
	sdsXDictName[0] = (long)pXDict;
	sdsXDictName[1] = (long)pDrawing;

	sds_name sdsCurName;
	// trick to pass block handle
	ownerBlock.ToODHandle((unsigned char*)sdsCurName);

	// create object
	struct resbuf* pSortEntsTable = sds_buildlist(RTDXF0, "SORTENTSTABLE",
			102, "{ACAD_REACTORS",
			330, sdsXDictName,
			102, "}",
			330, sdsCurName,
			RTNONE);

	struct resbuf* pCurPair = NULL;
	if(pTableList != NULL)
	{	
		bool bPSpace = ownerBlock == pDrawing->ret_stockhand(DB_SHI_BLKREC_MSPACE) ? false : true;
		db_handitem* pEntity = NULL;
		for(pEntity = pDrawing->entnext_noxref(NULL); pEntity != NULL; pEntity = pDrawing->entnext_noxref(pEntity))
			if((bPSpace && pEntity->ret_pspace()) || (!bPSpace && !pEntity->ret_pspace()))
			{
				if(*pTableList == NULL)
				{
					*pTableList = db_alloc_resbuf_list(2);
					pCurPair = *pTableList;
				}
				else
				{
					pCurPair->rbnext = db_alloc_resbuf_list(2);
					pCurPair = pCurPair->rbnext;
				}
				pCurPair->restype = 331;
				pCurPair->resval.rlname[0] = (long)pEntity;
				pCurPair->resval.rlname[1] = (long)pDrawing;
				
				pCurPair = pCurPair->rbnext;
				pCurPair->restype = 5;
				pCurPair->resval.rlname[0] = (long)pEntity;
				pCurPair->resval.rlname[1] = (long)pDrawing;
				
			}
	}
	else
		ASSERT(false);

	int result = sds_entmakex(pSortEntsTable, sdsTableName);
	if(result != RTNORM)
	{
		ASSERT(FALSE);
		sds_relrb(pSortEntsTable);
		return 0;
	}
	sds_relrb(pSortEntsTable);

	// add object to extension dictionary
	struct resbuf* pXDictList = sds_entget(sdsXDictName);
	if(pXDict == NULL)
	{
		ASSERT(FALSE);
		sds_relrb(pXDictList);
		return 0;
	}

	pSortEntsTable = sds_buildlist(3, "ACAD_SORTENTS", 350, sdsTableName, RTNONE);
	pCurPair = pXDictList;
	while(pCurPair->rbnext != NULL && pCurPair->rbnext->restype != 350)
		pCurPair = pCurPair->rbnext;

	while(pCurPair->rbnext != NULL && pCurPair->rbnext->restype == 350)
	{
		pCurPair = pCurPair->rbnext;
		if(pCurPair->rbnext != NULL)
			pCurPair = pCurPair->rbnext;
		else
			break;
	}

	if(pCurPair->rbnext == NULL)
		pCurPair->rbnext = pSortEntsTable;
	else
	{
		ASSERT(pCurPair->restype == 3);
		pSortEntsTable->rbnext->rbnext = pCurPair->rbnext->rbnext;
		pCurPair->rbnext->rbnext = pSortEntsTable;
	}
	pSortEntsTable = NULL;
	result = sds_entmod(pXDictList);
	if(result != RTNORM)
	{
		ASSERT(FALSE);
		sds_relrb(pXDictList);
		return 0;
	}
	sds_relrb(pXDictList);

	return 1;
}
