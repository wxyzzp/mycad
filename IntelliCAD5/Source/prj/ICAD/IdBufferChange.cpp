#include "Icad.h"
#include "IdBufferChange.h"
#include "objects.h"
#include "hireflink.h"

bool db_IdBufferChange::CacheChangedData(struct db_BinChange **pBeginList, struct db_BinChange **pCurrentNode,
											 void *oldval, void *newval, int len)
{
	db_IdBufferChange* pIdBufferChange = NULL;

	if(oldval == NULL || newval == NULL)
		return false;

	if(len != -1)
		return db_BinChange::CacheChangedData(pBeginList, pCurrentNode, oldval, newval, len);
	else
	{
		db_hireflink** pOldList = (db_hireflink**)oldval;
		db_hireflink** pNewList = (db_hireflink**)newval;

		db_hireflink* pCurLink1 = pOldList[0];
		db_hireflink* pCurLink2 = pNewList[0];
		bool bEqual = true;
		if(pCurLink1 == NULL && pCurLink2 != NULL)
			bEqual = false;
		else
		{
			while(pCurLink1 != NULL)
			{
				if(pCurLink2 == NULL || pCurLink1->hand != pCurLink2->hand)
					break;
				pCurLink1 = pCurLink1->next;
				pCurLink2 = pCurLink2->next;
			}
			if(pCurLink1 != NULL || pCurLink2 != NULL)
				bEqual = false;
		}

		if(!bEqual)
		{
			pIdBufferChange = new db_IdBufferChange(db_BinChange::SUBOBJECT);

			pIdBufferChange->m_pWhereData = newval;
			pIdBufferChange->m_iDataLength = 2 * sizeof(db_hireflink*);
			pIdBufferChange->m_pSavedData = new BYTE[pIdBufferChange->m_iDataLength];
			::memset(pIdBufferChange->m_pSavedData, 0, pIdBufferChange->m_iDataLength);

			pCurLink1 = pOldList[0];
			while(pCurLink1 != NULL)
			{
				pCurLink2 = new db_hireflink(*pCurLink1);
				if((((db_hireflink**)pIdBufferChange->m_pSavedData)[0]) == NULL)
					memcpy(&(((db_hireflink**)pIdBufferChange->m_pSavedData)[0]), &pCurLink2, sizeof(db_hireflink*));
				else
					(((db_hireflink**)pIdBufferChange->m_pSavedData)[1])->next = pCurLink2;
				memcpy(&(((db_hireflink**)pIdBufferChange->m_pSavedData)[1]), &pCurLink2, sizeof(db_hireflink*));
				pCurLink1 = pCurLink1->next;
			}
		}
	}

	if(pIdBufferChange != NULL)
		db_BinChange::AddNewLink(pBeginList, pCurrentNode, pIdBufferChange);

	return true;
}

db_IdBufferChange::db_IdBufferChange(ChangeType type): db_BinChange(type)
{
}

db_IdBufferChange::~db_IdBufferChange()
{
	if(m_pSavedData)
	{
		db_hireflink::delll(((db_hireflink**)m_pSavedData)[0]);
		delete [] m_pSavedData;
		// Added Cybage VSB 14/01/2002
		// Reason: Fix for Bug No: 77962 from BugZilla, Debug Assert statement
		m_pSavedData = NULL;
	}
}

bool db_IdBufferChange::SwitchChanges(db_drawing *pDrawing, db_handitem *pItem)
{
	db_hireflink* pCurList[2];
	((db_idbuffer*)pItem)->get_hirefllends(&pCurList[0], &pCurList[1]);
	((db_idbuffer*)pItem)->set_hirefllends(((db_hireflink**)m_pSavedData)[0], ((db_hireflink**)m_pSavedData)[1]);
	memcpy(m_pSavedData, pCurList, 2 * sizeof(db_hireflink*));

	return true;
}
