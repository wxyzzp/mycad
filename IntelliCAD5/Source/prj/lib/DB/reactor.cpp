/* File  : <DevDir>\source\prj\lib\db\reactor.cpp
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * A class that handles the handitems that should be notified when a handitem is modify
 */ 

#include "db.h"
#include <algorithm>

extern int	g_nClosingDrawings;

ReactorSubject::ReactorSubject()
{
	m_iterator = m_reactors.begin();
}

ReactorSubject::ReactorSubject(const ReactorSubject& reactor)
{
	m_reactors.assign(reactor.m_reactors.begin(), reactor.m_reactors.end());
	m_iterator = m_reactors.begin();
}

ReactorSubject::~ReactorSubject()
{
}

void ReactorSubject::BreakAssociation(db_handitem* pEntity, bool bDispMessage)
{
	// no need to break associations on close.
	if(g_nClosingDrawings > 0) 
		return;

	//int i = m_reactors.size();
	LIST_ITER nextIt;


	for(LIST_ITER it = m_reactors.begin(); it != m_reactors.end(); it = nextIt)
	{
		nextIt = it;
		nextIt++;
		if(!it->m_hip || it->m_hip->ret_type() != DB_GROUP)
		{
			if(it->m_hip)
				it->m_hip->BreakAssociation(pEntity, bDispMessage);
			// Changing erase to remove stops the crash,
			// but probably still isn't right. Needs research.
			m_reactors.remove((class ReactorItem &)it);
			//m_reactors.erase(it);
		}
	}
}

void ReactorSubject::ResolveNullReactors(db_drawing* pDrawing)
{
	for(LIST_ITER it = m_reactors.begin(); it != m_reactors.end(); ++it)
		if(!it->m_hip)
			it->m_hip = pDrawing->handent(it->m_handle);
}

bool ReactorSubject::FindReactor(const db_handitem* pHanditem)
{
	for(LIST_ITER it = m_reactors.begin(); it != m_reactors.end(); ++it)
		if(it->m_hip == pHanditem)
			return true;
	return false;
}

bool ReactorSubject::AttachReactor(const db_objhandle& handle, void*** fixuploc, ReactorItem::Type type)
{
	if(handle == 0)
		return false;

	ReactorItem	newItem(handle, type);

	//no duplicates allowed (since this is only used at load time we can eliminate duplicates here)
	if(std::find(m_reactors.begin(), m_reactors.end(), newItem) != m_reactors.end())		
		return false;

	m_reactors.push_back(newItem);

	*fixuploc = (void**)m_reactors.back().RetHanditem();
	return true;
}

bool ReactorSubject::AttachReactor(const db_objhandle& handle, db_handitem* pHanditem, ReactorItem::Type type)
{
	if(handle == 0)
		return false;

	ReactorItem	newItem(handle, pHanditem, type);

	// no duplicates allowed
	if(std::find(m_reactors.begin(), m_reactors.end(), newItem) != m_reactors.end())		
		return false;

	m_reactors.push_back(newItem);
	return true;
}

// need an attach method that allows duplicate entities.  Temporary entities sometimes need 
// to be created such as savelp in entmod.  This allows undo to have the proper association.
bool ReactorSubject::AttachReactorAllowDups(const db_objhandle& handle, db_handitem* pHanditem, ReactorItem::Type type)
{
	if(handle == 0)
		return false;

	if(pHanditem && pHanditem->ret_type() == DB_GROUP)
	{
		ReactorItem	item(handle);
 		LIST_ITER it = std::find(m_reactors.begin(), m_reactors.end(), item);
		if(it != m_reactors.end())
			return true;
	}
	//8/10/99 allow duplicates. Duplicates are needed for temporary entities (savelp in entmod)
	//so that undo will have the proper association.
	m_reactors.push_back(ReactorItem(handle, pHanditem, type));
	return true;
}

bool ReactorSubject::DetachReactor(const db_objhandle& handle)
{
	ReactorItem	item(handle);
 	LIST_ITER it = std::find(m_reactors.begin(), m_reactors.end(), item);

	if(it == m_reactors.end())
		return false;

	m_reactors.erase(it);
	return true;
}

bool ReactorSubject::GetNextHanditem(db_drawing* pDrawing, db_handitem** ppItem)
{
	if(m_iterator == m_reactors.end())
		return false;
	
	if(!m_iterator->m_hip)
		m_iterator->m_hip = pDrawing->handent(m_iterator->m_handle);

	*ppItem = m_iterator->m_hip;
	++m_iterator;

	if(*ppItem)
		return true;

	return false;
}

bool ReactorSubject::GetNextHandle(db_objhandle& handle)
{
	if(m_iterator == m_reactors.end())
		return false;
	
	handle = m_iterator->m_handle;
	++m_iterator;
	return true;
}

ReactorItem::Type ReactorSubject::GetCurrentType()
{
	assert(m_iterator != m_reactors.end());
	return m_iterator->m_type;
}

db_objhandle ReactorSubject::GetCurrentHandle()
{
	assert(m_iterator != m_reactors.end());
	return m_iterator->m_handle;
}

db_handitem* ReactorSubject::GetCurrentHanditem()
{
	assert(m_iterator != m_reactors.end());
	return m_iterator->m_hip;
}

// Resets the handle & hip at the current position. This function does not break the association
// with the other object, so the caller is responsible for modifying that.  This is used
// In the copy code so that the copies can be reassociated properly.
bool ReactorSubject::SetHandle(db_objhandle& oldHandle, db_objhandle& newHandle, db_drawing* pDrawing)
{
	ReactorItem	oldItem(oldHandle);
 	LIST_ITER it = std::find(m_reactors.begin(), m_reactors.end(), oldItem);

	if(it == m_reactors.end())
		return false;

	it->m_handle = newHandle;
	it->m_hip = pDrawing->handent(newHandle);
	return true;
}

void ReactorSubject::Notify(db_drawing* pDrawing, db_objhandle& subjectHandle)
{
	for(LIST_ITER it = m_reactors.begin(); it != m_reactors.end(); ++it)
	{
		if(!it->m_hip)
			it->m_hip = pDrawing->handent(it->m_handle);

		if(it->m_hip && !it->m_hip->ret_deleted())
			if(!it->m_hip->ReactorUpdate(subjectHandle))
				break;
	}
}

int ReactorSubject::GetCountNotDeleted(db_drawing* pDrawing) 
{
	int count = 0;
	for(REACTORITEMS_LIST::iterator it = m_reactors.begin(); it != m_reactors.end(); ++it)
	{
		try
		{
			it->m_hip = pDrawing->handent(it->m_handle);

			if(it->m_hip && !it->m_hip->ret_deleted())
				++count;
		}
		catch(...)
		{
			//m_reactors.remove(*it);
			// Calling remove just pushes the problem down the road.
			// The real fix is more complex, because 
			// something is corrupting these reactors.
			// This is just a band aid to preven crashes.
			// I'll add this ASSERT() for future references.
			ASSERT(!"BAD reactor");
		}
	}
	return count;
}
