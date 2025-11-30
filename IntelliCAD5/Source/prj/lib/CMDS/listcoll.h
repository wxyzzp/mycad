/* 
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * A list collection class which holds db_handitem pointers. The list only allocates handitems when
 * blocks are being expanded as new entitites are needed to store the rotated/scaled/... values.
 * Handitems may or may not be destroyed depending on the destroyHI indicator that is passed in.
 * The class was originally designed to be used to keep track of a set of entities which 
 * are already in the drawing and the memory management is being handled by the drawing.  However,
 * it the user can indicate that particular entities should be destroyed by setting the destroyHI
 * flag.  
 * 
 */ 

#ifndef _LISTCOLL_H
#define _LISTCOLL_H

#include "db.h"

class C3Point;

struct HanditemNode
{
	HanditemNode() : destroyHI(false), insert(NULL), m_pDrawing(NULL) {}
	~HanditemNode() 
		{if (destroyHI) delete item;}


	db_handitem *item;
	bool destroyHI; //Indicates whether or not the handitem should be destroyed on destruction.
	HanditemNode *nextNode;

	//If the entity is in a block this indicates which insert this entity came from.  This is 
	//used in the HanditemCollection::ExpandBlocks().  If entity is not an expanded insert this value 
	//will be NULL
	db_handitem *insert;
	
	db_drawing *m_pDrawing;
};

class HanditemCollection 
{
public:
	enum SpaceFilter {NONE, PAPERSPACE, MODELSPACE};


	HanditemCollection() : m_first(NULL), m_last(NULL), m_count(0) {}

	//Does not destory handitems in the collection only the collection nodes
	~HanditemCollection();

	//destroyHI indicates whether or not the handitem should be destroyed when ClearList() is 
	//called or when the collection is destroyed. Returns true if the item was added.
	//InsertParent should only be set it the entity being added is in a block and then the insert can be
	//set.  InsertParent is intended to be used from ExpandBlocks
	bool AddItem(db_drawing *drawing, db_handitem* item, bool destroyHI = false, db_handitem *insertParent = NULL);

	//checks the handitem list for duplicates before adding.  If the item is already in the
	//list it is not added. Note this function should not be used for temporary entities which
	//may not have a handle.
	//Returns true if added false if not added
	bool AddItemCheckDups(db_drawing *drawing, db_handitem* item, bool destroyHI = false);


	//Removes an item from the list.  If destroyHI is true for this entity the entity will
	//be destroyed.  Otherwise, it is assumed that the handitem will be destroyed by the user.
	//Returns true if the item was found and removed, otherwise false.
	bool RemoveItem(db_handitem* item);
	
	//removes all entities in current list, and deletes if destroyHI is true
	bool ClearList();

	//Returns next item in the list.  If NULL is passed in it returns the first item in the list.
	//Returns null if passed in item is the last entity in the list.
	db_handitem* GetNext(db_handitem *currentItem);

	//Returns next item in the list.  If NULL is passed in it returns the first item in the list.
	//Returns null if passed in item is the last entity in the list. 
	//If the entity was derived from an insert (from ExpandBlocks) the parent insert is returned
	//in the 3rd parameter
	void GetNext(db_handitem *currentItem, db_handitem** nextItem, db_handitem **insert);


	//Returns true if no entities are in list
	bool IsEmpty() {return (m_count <= 0);}

	//Number of entities in list
	int Count() {return m_count;}

	//Resets whether you want to keep a single handitem or delete it when the list is destroyed
	bool SetDestroyHI (db_handitem* item, bool destroyHI = false);

	//Resets whether you want to keep or delete all handitems in the list on destruction.
	bool SetListDestroyHI (db_handitem* item, bool destroyHI = false);

	//Replaces the current list with the pointers to items in the new list. Entities are not actually copied.
	//Destroy indicates whether the handitems should be destroyed when the list or node is destroyed.
	//If you want the list filtered input the filter item.
	//space removes any items that are not the same space specified - if NONE no filter is used.
	//deletedItems removes all deleted items from the list
	//invisibleItems removes all invisible items from the list
	bool CopyList(db_drawing *drawing, db_handitem *list, bool destroyHI = false, SpaceFilter space = NONE, 
				  bool deletedItems = false, bool invisibleItems = false);

	//removes all entities of input type (i.e. DB_LINE) from the list.  Note - this function does not consider entities that
	//may be in blocks.  A separate method can be written later if that functionality is desired.
	bool FilterEntity(int dbType);

	//Removes all entities that do not have same directional vectors(extrusion vector) as ptNormal
	bool FilterEntity(C3Point ptNormal);

	//Recursive function that removes all inserts from the list and replaces the inserts with the entities in the block.
	//Expanded entities are destroyed when the list is deleted since these entities do not exist in
	//the drawing.  The user may reset this value for any nodes that it would like to keep.
	bool ExpandBlocks(db_drawing *drawing);

protected:
	HanditemNode *m_first;
	HanditemNode *m_last;
	int m_count;

	//checks a single item to see if it meets the filter requirements.
	//space removes any items that are not the same space specified - if NONE no filter is used.
	//deletedItems removes all deleted items from the list
	//invisibleItems removes all invisible items from the list
	//returns true if passes filtering
	bool FilterItem(db_handitem *item, SpaceFilter space, bool deletedItems, bool visibleItems);

	//Removes a single node from the list destroying it if destroyHI is true.
	bool RemoveItem(HanditemNode* previous, HanditemNode* current);
};


///
class CHanditemCollectionWXRefs : public HanditemCollection
{
public:
	CHanditemCollectionWXRefs() : m_pDrawing(NULL) {};
	CHanditemCollectionWXRefs( db_drawing *pDrawing ){ m_pDrawing = pDrawing; }

	void setDrawing( db_drawing *pDrawing ){ m_pDrawing = pDrawing; }
	bool ExpandBlocks();

protected:
	db_drawing *m_pDrawing;
};


#endif

