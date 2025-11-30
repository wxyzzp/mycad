/* D:\ICADDEV\PRJ\LIB\DB\FIXUP1005.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#ifndef _INC_FIXUP1005
#define _INC_FIXUP1005
#ifndef	_DB_H
#include "db.h"
#endif

#ifndef	_IC_DYNARRAY_H
#include "ic_dynarray.h"
#endif



struct FixupHandleTableEntry
	{
					FixupHandleTableEntry(void);
					FixupHandleTableEntry(db_objhandle oldhandle, db_objhandle newhandle);
					db_objhandle oldHandle;
					db_objhandle 	newHandle;
	};


typedef	ic_dynarray< FixupHandleTableEntry>	FixupHandleArray;

class DB_CLASS FixupHandleTable 
	{
public:
						FixupHandleTable();
						~FixupHandleTable();
	void				Add(db_objhandle oldhandle,db_objhandle newhandle);
	bool				Get(unsigned int nItem, FixupHandleTableEntry *theentry);
	bool				GetNewHandle(db_objhandle oldHandle, db_objhandle *newHandle);
private:
	FixupHandleArray			m_table;
	};


struct DB_CLASS FixupHipTableEntry
	{
					FixupHipTableEntry(void);
					FixupHipTableEntry(db_handitem *thehip);
	db_handitem *hip;
	};


typedef	ic_dynarray< FixupHipTableEntry>	FixupHipArray;

class DB_CLASS FixupHipTable 
{
public:
	FixupHipTable();
	virtual	~FixupHipTable();

	virtual void Add(db_handitem *thehip);
	virtual bool Get(unsigned int nItem, FixupHipTableEntry *theentry);
	virtual bool ApplyFixups(db_drawing *pDrawing) {return false;}

protected:
	FixupHipArray			m_table;
};

class DB_CLASS Fixup1005HipTable : public FixupHipTable
{
public:
	Fixup1005HipTable() : FixupHipTable() {}
	~Fixup1005HipTable() {};
	
	bool ApplyFixups(db_drawing *pDrawing);
};

class FixupReactorTable : public FixupHipTable
{
public:
	FixupReactorTable() : FixupHipTable() {}

	bool ApplyFixups(db_drawing *dp, FixupHandleTable *handleTable);
};

class DB_CLASS ReactorFixups 
{
public:
	ReactorFixups() {}
	~ReactorFixups() {}

	void ApplyFixups(db_drawing *drawing);
	void AddHanditem(db_handitem *pHandItem);
	void AddHandleMap (db_objhandle oldhandle, db_objhandle newhandle);

protected:
	FixupHandleTable m_handlesMap;
	FixupReactorTable m_handitemsTable;
};

#endif

