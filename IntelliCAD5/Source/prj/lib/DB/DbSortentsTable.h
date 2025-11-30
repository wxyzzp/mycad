#ifndef __DbSortentsTableH__
#define __DbSortentsTableH__

#include "handitem.h"
typedef db_handitem CDbObject;
#include <map>
typedef std::map<db_objhandle, db_objhandle> SORTENTSTABLE;

/*--------------------------------------------------------------------------------*//**
@author Denis Petrov

@description Class for representing SORTENTSTABLE object.
*//*----------------------------------------------------------------------------------*/
class DB_CLASS CDbSortentsTable: public CDbObject 
{
	friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

public:
	CDbSortentsTable();
	CDbSortentsTable(const CDbSortentsTable& table);
	virtual ~CDbSortentsTable();

	int   ret_deleted() { return m_deleted; }
	void  set_deleted(int p) { m_deleted = (char)(p!=0); }

	const db_objhandle& block()	{ return m_block; }
	void setBlock(const db_objhandle& block) { m_block = block; }

	int size() const { return m_pTable->size(); }

	void resetIterator() { m_tableIt = m_pTable->begin(); }
	bool next(db_objhandle& sortHandle, db_objhandle& handle);
	bool find(const db_objhandle& sortHandle, db_objhandle& handle) const;
	bool insert(const db_objhandle& sortHandle, const db_objhandle& handle);

	int entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing* pDrawing);
	int entmod(struct resbuf *modll, db_drawing* pDrawing);

private:
	char m_deleted;
	db_objhandle m_block;

	SORTENTSTABLE* m_pTable;
	SORTENTSTABLE::iterator m_tableIt;
};

#endif
