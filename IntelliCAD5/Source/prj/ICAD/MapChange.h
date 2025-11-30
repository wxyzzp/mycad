#ifndef __MapChangeH__
#define __MapChangeH__

#include "Icad.h"
#include "binchange.h"
#include "objects.h"
#include <map>

/*---------------------------------------------------------------------------*//**
@author Denis Petrov

@description This class implements UNDO interface for STL map.
*//*--------------------------------------------------------------------------*/
template <class _Key, class _Type>
class db_MapChange: public db_BinChange
{
public:
	db_MapChange(ChangeType type): db_BinChange(type) {}
	virtual ~db_MapChange()
	{
		if(m_pSavedData)
		{
			CMapSt* pMap = (CMapSt*)m_pSavedData;
			delete pMap;
			m_pSavedData = NULL;
		}
	}
	static bool CacheChangedData(struct db_BinChange **pBeginList, struct db_BinChange **pCurrentNode, 
		void *oldval, void *newval, int len);

protected:
	typedef std::map<_Key, _Type> CMapSt;

	virtual bool SwitchChanges(db_drawing *pDrawing, db_handitem *pItem);
};

template <class _Key, class _Type> 
bool db_MapChange<_Key, _Type>::CacheChangedData(struct db_BinChange **pBeginList, struct db_BinChange **pCurrentNode,
											 void *oldval, void *newval, int len)
{
	db_MapChange<_Key, _Type>* pMapChange = NULL;

	if(oldval == NULL || newval == NULL)
		return false;

	if(len != -1)
		return db_BinChange::CacheChangedData(pBeginList, pCurrentNode, oldval, newval, len);
	else
	{
		CMapSt** pOldMap = (CMapSt**)oldval;
		CMapSt** pNewMap = (CMapSt**)newval;

		bool bEqual = true;
		for(CMapSt::iterator it = (*pOldMap)->begin(); it != (*pOldMap)->end(); ++it)
		{
			CMapSt::iterator itAtNew = (*pNewMap)->find((*it).first);
			if(itAtNew == (*pNewMap)->end())
			{
				bEqual = false;
				break;
			}
			if((*itAtNew).second != (*it).second)
			{
				bEqual = false;
				break;
			}
		}

		if(!bEqual)
		{
			pMapChange = new db_MapChange<_Key, _Type>(db_BinChange::SUBOBJECT);

			pMapChange->m_pWhereData = newval;
			pMapChange->m_iDataLength = sizeof(*pOldMap);
			pMapChange->m_pSavedData = (*pOldMap);
			(*pOldMap) = NULL;
		}
	}

	if(pMapChange != NULL)
		db_BinChange::AddNewLink(pBeginList, pCurrentNode, pMapChange);

	return true;
}

template <class _Key, class _Type>
bool db_MapChange<_Key, _Type>::SwitchChanges(db_drawing *pDrawing, db_handitem *pItem)
{
	CMapSt* pMap = *(CMapSt**)m_pWhereData;
	memcpy(m_pWhereData, &m_pSavedData, m_iDataLength);
	m_pSavedData = pMap;
	return true;
}

#endif
