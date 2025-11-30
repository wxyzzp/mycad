#include "Icad.h"
#include "GroupChange.h"
#include "DbGroup.h"
#include "hireflink.h"
#include <map>

bool db_GroupChange::CacheChangedData(struct db_BinChange **pBeginList, struct db_BinChange **pCurrentNode, 
											 void *oldval, void *newval, int len)
{
	db_GroupChange* pGroupChange = NULL;

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
			pGroupChange = new db_GroupChange(db_BinChange::SUBOBJECT);

			pGroupChange->m_pWhereData = newval;
			pGroupChange->m_iDataLength = 2 * sizeof(db_hireflink*);
			pGroupChange->m_pSavedData = new BYTE [pGroupChange->m_iDataLength];
			::memset(pGroupChange->m_pSavedData, 0, pGroupChange->m_iDataLength);

			pCurLink1 = pOldList[0];
			while(pCurLink1 != NULL)
			{
				pCurLink2 = new db_hireflink(*pCurLink1);
				if((((db_hireflink**)pGroupChange->m_pSavedData)[0]) == NULL)
					memcpy(&(((db_hireflink**)pGroupChange->m_pSavedData)[0]), &pCurLink2, sizeof(db_hireflink*));
				else
					(((db_hireflink**)pGroupChange->m_pSavedData)[1])->next = pCurLink2;
				memcpy(&(((db_hireflink**)pGroupChange->m_pSavedData)[1]), &pCurLink2, sizeof(db_hireflink*));
				pCurLink1 = pCurLink1->next;
			}
		}
	}

	if(pGroupChange != NULL)
		db_BinChange::AddNewLink(pBeginList, pCurrentNode, pGroupChange);

	return true;
}

db_GroupChange::db_GroupChange(ChangeType type): db_BinChange(type)
{
}

db_GroupChange::~db_GroupChange()
{
	if(m_pSavedData)
	{
		db_hireflink::delll(((db_hireflink**)m_pSavedData)[0]);		
		IC_FREE (m_pSavedData);
		// Added Cybage VSB 14/01/2002
		// Reason: Fix for Bug No: 77962 from BugZilla, Debug Assert statement
		m_pSavedData = NULL;
	}
}

bool db_GroupChange::SwitchChanges(db_drawing *pDrawing, db_handitem *pItem)
{
	std::map<db_objhandle, int> items;

	db_hireflink* pCurList[2];
	((CDbGroup*)pItem)->getEntitiesList(pCurList[0], pCurList[1]);
	
	db_hireflink* link = NULL;
	for(link = pCurList[0]; link != NULL; link = link->next)
		items[link->hand] = 1;

	for(link = ((db_hireflink**)m_pSavedData)[0]; link != NULL; link = link->next)
	{
		std::map<db_objhandle, int>::iterator it = items.find(link->hand);
		if(it == items.end())
			items[link->hand] = 2;
		else
			items[link->hand] += 2;
	}
	((CDbGroup*)pItem)->setEntitiesList(((db_hireflink**)m_pSavedData)[0], ((db_hireflink**)m_pSavedData)[1]);
	memcpy(m_pSavedData, pCurList, 2 * sizeof(db_hireflink*));

	for(std::map<db_objhandle, int>::iterator it = items.begin(); it != items.end(); ++it)
	{
		if(it->second != 3)
		{
			db_handitem* hip = pDrawing->handent(it->first);
			if(!hip)
				continue;
			if(it->second == 2)
			{
				hip->addReactorAllowDups(pItem->RetHandle(), pItem, ReactorItem::SOFT_POINTER);
			}
			else
			{
				hip->removeReactor(pItem->RetHandle());
			}
		}
	}
	return true;
}

db_LeaderChange::~db_LeaderChange()
{
	if(m_pSavedData)
	{
		delete [] m_pSavedData;
		m_pSavedData = NULL;
	}
}

bool db_LeaderChange::SwitchChanges(db_drawing *pDrawing, db_handitem *pItem)
{
	db_handitem *annothip = NULL;
	((db_leader*)pItem)->get_340(&annothip);
	
	db_handitem** ptr = (db_handitem**)m_pSavedData;
	if(*ptr)
		((db_leader*)pItem)->set_340andAssociate(*ptr);
	else
		((db_leader*)pItem)->set_340(*ptr);
	memcpy(m_pSavedData, &annothip, sizeof(annothip));
	return true;
}

bool db_LeaderChange::CacheChangedData(struct db_BinChange **pBeginList, struct db_BinChange **pCurrentNode, 
									   void *oldval, void *newval, int len)
{
	if(len >= 0)
		return db_BinChange::CacheChangedData(pBeginList, pCurrentNode, oldval, newval, len);
	
	if(oldval == newval) 
		return false;

	db_LeaderChange* pChange = new db_LeaderChange(db_BinChange::SUBOBJECT);
	pChange->m_pWhereData = newval;
	pChange->m_iDataLength = abs(len);
	pChange->m_pSavedData = new BYTE [pChange->m_iDataLength];
	::memcpy(pChange->m_pSavedData, oldval, pChange->m_iDataLength);
	db_BinChange::AddNewLink(pBeginList, pCurrentNode, pChange);
	return true;
}
