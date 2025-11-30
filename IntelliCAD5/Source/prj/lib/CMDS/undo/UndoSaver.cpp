// File: UndoSaver.cpp
//
// CUndoSaver class implementation
//

#include "icad.h"
#include "IcadApi.h"
#include "UndoSaver.h"


CUndoSaver::CUndoSaver(db_handitem* pItem, db_drawing* pDrw) : m_pProcessedItem(pItem), m_pOriginalItem(0), m_pDrw(pDrw)
{
	if(!m_pProcessedItem || !m_pDrw)
		return;
	SDS_CopyEntLink(&m_pOriginalItem, m_pProcessedItem);
}

CUndoSaver::~CUndoSaver()
{
	delete m_pOriginalItem;
	m_pOriginalItem = 0;
}

void
CUndoSaver::saveToUndo()
{
	if(!m_pOriginalItem)
		return;
	db_BinChange*	pChangeList = NULL;
	if(m_pOriginalItem && db_CompEntLinks(&pChangeList, m_pOriginalItem, m_pProcessedItem) == RTNORM && pChangeList)
		SDS_SetUndo(IC_UNDO_CHG_ENT_TAB_CLS, (void*)m_pProcessedItem, (void*)pChangeList, NULL, m_pDrw);
}
