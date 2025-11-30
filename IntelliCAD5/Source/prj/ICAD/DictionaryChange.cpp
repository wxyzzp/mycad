#include "Icad.h"
#include "DictionaryChange.h"
#include "objects.h"

bool db_DictionaryChange::CacheChangedData(struct db_BinChange **pBeginList, struct db_BinChange **pCurrentNode,
											 void *oldval, void *newval, int len)
{
	db_DictionaryChange* pGroupChange = NULL;

	if(oldval == NULL || newval == NULL)
		return false;

	if(len != -1)
		return db_BinChange::CacheChangedData(pBeginList, pCurrentNode, oldval, newval, len);
	else
	{
		db_dictionaryrec** pOldList = (db_dictionaryrec**)oldval;
		db_dictionaryrec** pNewList = (db_dictionaryrec**)newval;

		db_dictionaryrec* pCurLink1 = pOldList[0];
		db_dictionaryrec* pCurLink2 = pNewList[0];
		bool bEqual = true;
		if(pCurLink1 == NULL && pCurLink2 != NULL)
			bEqual = false;
		else
		{
			while(pCurLink1 != NULL)
			{
				if(pCurLink2 == NULL ||
					pCurLink1->hiref->hand != pCurLink2->hiref->hand ||
					db_compareNames(pCurLink1->name, pCurLink2->name) != 0)
					break;
				pCurLink1 = pCurLink1->next;
				pCurLink2 = pCurLink2->next;
			}
			if(pCurLink1 != NULL || pCurLink2 != NULL)
				bEqual = false;
		}

		if(!bEqual)
		{
			pGroupChange = new db_DictionaryChange(db_BinChange::SUBOBJECT);

			pGroupChange->m_pWhereData = newval;
			pGroupChange->m_iDataLength = 2 * sizeof(db_dictionaryrec*);
			pGroupChange->m_pSavedData = new BYTE [pGroupChange->m_iDataLength];
			::memset(pGroupChange->m_pSavedData, 0, pGroupChange->m_iDataLength);

			pCurLink1 = pOldList[0];
			while(pCurLink1 != NULL)
			{
				pCurLink2 = new db_dictionaryrec(*pCurLink1);
				if((((db_dictionaryrec**)pGroupChange->m_pSavedData)[0]) == NULL)
					memcpy(&(((db_dictionaryrec**)pGroupChange->m_pSavedData)[0]), &pCurLink2, sizeof(db_dictionaryrec*));
				else
					(((db_dictionaryrec**)pGroupChange->m_pSavedData)[1])->next = pCurLink2;
				memcpy(&(((db_dictionaryrec**)pGroupChange->m_pSavedData)[1]), &pCurLink2, sizeof(db_dictionaryrec*));
				pCurLink1 = pCurLink1->next;
			}
		}
	}

	if(pGroupChange != NULL)
		db_BinChange::AddNewLink(pBeginList, pCurrentNode, pGroupChange);

	return true;
}

db_DictionaryChange::db_DictionaryChange(ChangeType type): db_BinChange(type)
{
}

db_DictionaryChange::~db_DictionaryChange()
{
	if(m_pSavedData)
	{
		db_dictionaryrec* pCurRec = (((db_dictionaryrec**)m_pSavedData)[0]);
		db_dictionaryrec* pRecToDel;
		while(pCurRec != NULL)
		{
			pRecToDel = pCurRec;
			pCurRec = pCurRec->next;
			delete pRecToDel;
		}		
		IC_FREE(m_pSavedData);
		// Added Cybage VSB 14/01/2002
		// Reason: Fix for Bug No: 77962 from BugZilla, Debug Assert statement
		m_pSavedData = NULL;
	}
}

bool db_DictionaryChange::SwitchChanges(db_drawing *pDrawing, db_handitem *pItem)
{
	db_dictionaryrec* pCurList[2];
	if(pItem->ret_type() == DB_DICTIONARY )
	{
		((db_dictionary*)pItem)->get_recllends(&pCurList[0], &pCurList[1]);
		((db_dictionary*)pItem)->set_recllends(((db_dictionaryrec**)m_pSavedData)[0], ((db_dictionaryrec**)m_pSavedData)[1]);
	}
	else if(pItem->ret_type() == DB_DICTIONARYWDFLT )
	{
		((db_dictionarywdflt*)pItem)->get_recllends(&pCurList[0], &pCurList[1]);
		((db_dictionarywdflt*)pItem)->set_recllends(((db_dictionaryrec**)m_pSavedData)[0], ((db_dictionaryrec**)m_pSavedData)[1]);
	}
	else
	{
		return false;
	}
	memcpy(m_pSavedData, pCurList, 2 * sizeof(db_dictionaryrec*));

	return true;
}
