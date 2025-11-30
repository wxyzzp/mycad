#include "db.h"
#include "DbSortentsTable.h"
//#include "objects.h"

/***************************** CDbSortentsTable *****************************/

CDbSortentsTable::CDbSortentsTable(): 
CDbObject(DB_SORTENTSTABLE)
{
    m_deleted = 0;
    m_block = NULL;
	m_pTable = new SORTENTSTABLE;
	m_tableIt = m_pTable->end();
}

CDbSortentsTable::CDbSortentsTable(const CDbSortentsTable& table):
CDbObject(table)
{
    m_deleted = table.m_deleted;
	m_block = table.m_block;

	m_pTable = new SORTENTSTABLE(*table.m_pTable);
	m_tableIt = m_pTable->end();
}

CDbSortentsTable::~CDbSortentsTable()
{
	delete m_pTable;
}

int CDbSortentsTable::entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing *pDrawing)
{
	CDbObject::entgetspecific(begpp, endpp, pDrawing);

    sds_name entityName;
    db_handitem* pEntity;

	// 330 Soft pointer to owner BLOCK_RECORD *Model_space or *Paper_spaceX */
	// Note : Icad cannot store block_record table record object into db_drawing
	// So, This program code always set (330 . <EntityName: 0>)
	assert(m_block != NULL);
	pEntity = pDrawing->handent(m_block);
	entityName[0] = (long)pEntity; 
	entityName[1] = (long)pDrawing; 

	*begpp = *endpp = db_newrb(330, 'N', entityName);

	for(SORTENTSTABLE::const_iterator it = m_pTable->begin(); it != m_pTable->end(); ++it)
	{
		pEntity = pDrawing->handent((*it).second);
		entityName[0] = (long)pEntity; 
		entityName[1] = (long)pDrawing; 
		(*endpp)->rbnext = db_newrb(331, 'N', entityName);
		*endpp = (*endpp)->rbnext;

		pEntity = pDrawing->handent((*it).first);
		entityName[0] = (long)pEntity; 
		entityName[1] = (long)pDrawing; 
		(*endpp)->rbnext = db_newrb(5, 'N', entityName);
		*endpp = (*endpp)->rbnext;
	}
    return 0;
}

int CDbSortentsTable::entmod(struct resbuf *modll, db_drawing *pDrawing)
{
	CDbObject::entmod(modll, pDrawing);

	// there can be several 330 groups in the object list
	// block name should be last
	struct resbuf* pCurPair = modll;
	while(ic_assoc(pCurPair->rbnext, 330) == 0)
		pCurPair = ic_rbassoc;
	if(pCurPair && pCurPair->restype == 330)
		m_block = db_objhandle((unsigned char*)pCurPair->resval.rlname);

	pCurPair = modll;
	while(pCurPair && pCurPair->restype != 331)
		pCurPair = pCurPair->rbnext;

	m_pTable->clear();
	while(pCurPair != NULL && pCurPair->restype == 331)
	{
		assert(pCurPair->rbnext->restype == 5);
		assert(m_pTable->find(((db_handitem*)pCurPair->rbnext->resval.rlname[0])->RetHandle()) == m_pTable->end());
		(*m_pTable)[((db_handitem*)pCurPair->rbnext->resval.rlname[0])->RetHandle()] = ((db_handitem*)pCurPair->resval.rlname[0])->RetHandle();
		pCurPair = pCurPair->rbnext;
		pCurPair = pCurPair->rbnext;
	}
	return 0;
}

bool CDbSortentsTable::next(db_objhandle& sortHandle, db_objhandle& handle)
{
	if(m_tableIt == m_pTable->end())
		return false;

	sortHandle = (*m_tableIt).first;
	handle = (*m_tableIt).second;
	++m_tableIt;
	return true;
}

bool CDbSortentsTable::find(const db_objhandle& sortHandle, db_objhandle& handle) const
{
	SORTENTSTABLE::const_iterator it = m_pTable->find(sortHandle);
	if(it == m_pTable->end())
	{
		handle = db_objhandle();
		return false;
	}
	handle = (*it).second;
	return true;
}

bool CDbSortentsTable::insert(const db_objhandle& sortHandle, const db_objhandle& handle)
{
	if(sortHandle == NULL || handle == NULL)
	{
		assert(false);
		return false;
	}
	if(m_pTable->find(sortHandle) != m_pTable->end())
		return false;

	(*m_pTable)[sortHandle] = handle;
	return true;
}

