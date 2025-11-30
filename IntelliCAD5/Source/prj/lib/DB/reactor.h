/* File  : <DevDir>\source\prj\lib\db\reactor.h
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * A class that handles the handitems that should be notified when a handitem is modify
 */ 

#ifndef __reactorH__
#define __reactorH__

#include <list>

class DB_CLASS ReactorItem
{
public:
	enum Type 
	{
		SOFT_POINTER = DB_SOFT_POINTER, 
		HARD_POINTER = DB_HARD_POINTER,
		SOFT_OWNER = DB_SOFT_OWNER,
		HARD_OWNER = DB_HARD_OWNER
	};

	ReactorItem() : m_hip(NULL), m_type(SOFT_POINTER) {}
	ReactorItem(const ReactorItem& item): m_hip(item.m_hip), m_type(item.m_type), m_handle(item.m_handle) {}
	ReactorItem(const db_objhandle& handle, Type type = SOFT_POINTER): m_hip(NULL), m_type(type), m_handle(handle) {}
	ReactorItem(const db_objhandle& handle, db_handitem* pHanditem, Type type = SOFT_POINTER): m_hip(pHanditem), m_type(type), m_handle(handle) {}

	//Returns the location of handitem for fixup on load
	db_handitem** RetHanditem() { return &m_hip; }

	void operator= (const ReactorItem& item) { m_handle = item.m_handle; m_hip = item.m_hip; }
	bool operator== (const ReactorItem& item) const { return m_handle == item.m_handle; }

	db_objhandle	m_handle;
	db_handitem*	m_hip;
	Type			m_type;
};


typedef std::list<ReactorItem> REACTORITEMS_LIST;
typedef std::list<ReactorItem>::iterator LIST_ITER;
#pragma warning (disable : 4251)

class DB_CLASS ReactorSubject 
{
public:
	ReactorSubject();
	ReactorSubject(const ReactorSubject &reactor);
	~ReactorSubject();

	void BreakAssociation(db_handitem* pEntity, bool bDispMessage = true);

	// Attaching a reactory without the db_handitem resolved is only intended for when the drawing
	// is opened and all the handitems are not yet loaded.  If a NULL handitem is added need to call
	// ResolveNullReactors to ensure they are found.
	bool AttachReactor(const db_objhandle& handle, void*** fixuploc, ReactorItem::Type type = ReactorItem::SOFT_POINTER);
	bool AttachReactor(const db_objhandle& handle, db_handitem* pHanditem = NULL, ReactorItem::Type type = ReactorItem::SOFT_POINTER);

	// need an attach method that allows duplicate entities.  Temporary entities sometimes need 
	// to be created such as savelp in entmod.  This allows undo to have the proper association.
	bool AttachReactorAllowDups(const db_objhandle& handle, db_handitem* pHanditem, ReactorItem::Type type);

	bool FindReactor(const db_handitem* pHanditem);
	bool DetachReactor(const db_objhandle& handle);
	void RemoveAll() { m_reactors.clear(); }

	void ResolveNullReactors(db_drawing* pDrawing);

	int GetCount() const { return m_reactors.size(); }
	int GetCountNotDeleted(db_drawing* pDrawing) ;
	void ResetIterator() { m_iterator = m_reactors.begin(); }

	// A false return indicates the end of the list has been reached.  If the handitem cannot be found in the 
	// drawing (this should not happen) then nextItem will be NULL.
	bool GetNextHanditem(db_drawing* pDrawing, db_handitem** ppItem);
	bool GetNextHandle(db_objhandle& handle);

	//Gets the type of the item at the current position
	ReactorItem::Type GetCurrentType();
	db_objhandle GetCurrentHandle();
	db_handitem* GetCurrentHanditem();

	// Resets the handle & hip at the current position. This function does not break the association
	// with the other object, so the caller is responsible for modifying that.  This is used
	// In the copy code so that the copies can be reassociated properly.
	bool SetHandle(db_objhandle& oldHandle, db_objhandle& newHandle, db_drawing* pDrawing);

	//Notifies each of the entities in the list that the calling entity has changed
	void Notify(db_drawing* pDrawing, db_objhandle& subjectHandle);

private:
	REACTORITEMS_LIST	m_reactors;
	LIST_ITER			m_iterator;
};

#endif
