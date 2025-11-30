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

#include "cmds.h"
#include "Icad.h"

#include "listcoll.h"

#include "Line.h"
#include "Gr.h"
#include "cmds.h"
#include "IcadView.h"

LOCAL void SdsToRpt(
	sds_point q,	// O: sds-style point
	C3Point & P)	// O: Geo-style point
	{
	P = C3Point(q[0], q[1], q[2]);
	}

//Does not destory handitems in the collection only the collection nodes
HanditemCollection::~HanditemCollection()
{
	ClearList();
}


//destroyHI indicates whether or not the handitem should be destroyed when ClearList() is
//called or when the collection is destroyed. Returns true if the item was added.
bool
HanditemCollection::AddItem(db_drawing *drawing, db_handitem* item, bool destroyHI, db_handitem *insertParent)
{
	if (!item)
		return false;

	//make sure the new handitem has a handle (in case it's temporary)
	item->assignhand(drawing);

	HanditemNode *newNode = new HanditemNode;

	newNode->item = item;
	newNode->destroyHI = destroyHI;
	newNode->insert = insertParent;
	newNode->m_pDrawing = drawing;  // to keep drawing this object belongs to
	newNode->nextNode = NULL;

	if (!m_last) {
		m_first = newNode;
	}
	else {
		m_last->nextNode = newNode;
	}

	m_last = newNode;

	m_count++;

	return true;
}

//checks the handitem list for duplicates before adding.  If the item is already in the
//list it is not added. Note this function should not be used for temporary entities which
//may not have a handle.
//Returns true if added false if not added
bool
HanditemCollection::AddItemCheckDups(db_drawing *drawing, db_handitem* item, bool destroyHI)
{
	db_handitem *currentHip = NULL;
	struct gr_view * gView = SDS_CURGRVW;

	//go through list to ensure new item is not a duplicate
	while (currentHip = GetNext(currentHip))
	{
		if (item->RetHandle() == currentHip->RetHandle())
		{
			return false;
		}
	}

	return AddItem(drawing, item, destroyHI);
}


//Removes an item from the list.  If destroyHI is true for this entity the entity will
//be destroyed.  Otherwise, it is assumed that the handitem will be destroyed by the user.
//Returns true if the item was found and removed, otherwise false.
bool
HanditemCollection::RemoveItem(db_handitem* item)
{
	if (!item)
		return false;

	HanditemNode *current = m_first;
	HanditemNode *previous = NULL;

	while (current)
	{
		if (current->item->RetHandle()==item->RetHandle()) //item found
		{
			return RemoveItem(previous, current);
		}

		previous = current;
		current = current->nextNode;
	}
	return false;
}

//Removes a single node from the list destroying it if destroyHI is true.
bool
HanditemCollection::RemoveItem(HanditemNode* previous, HanditemNode* current)
{
	if (!current)
		return false;

	if (!previous) //first link so reassign m_first to second link
		m_first = m_first->nextNode;

	else
		previous->nextNode = current->nextNode;

	if (m_last == current)
		m_last = previous;

	delete current;
	current = NULL;

	m_count--;

	return true;
}

//Returns next item in the list.  If NULL is passed in it returns the first item in the list.
//Returns null if passed in item is the last entity in the list or if the item is not in the list.
db_handitem*
HanditemCollection::GetNext(db_handitem *currentItem)
{
	db_handitem *nextItem = NULL;
	db_handitem *insert = NULL;

	GetNext(currentItem, &nextItem, &insert);

	return nextItem;
}

//Returns next item in the list.  If NULL is passed in it returns the first item in the list.
//Returns null if passed in item is the last entity in the list.
//If the entity was derived from an insert (from ExpandBlocks) the parent insert is returned
//in the 3rd parameter
void
HanditemCollection::GetNext(db_handitem *currentItem, db_handitem** nextItem, db_handitem **insert)
{
	if (!currentItem)
		if (m_first)
		{
			*nextItem =  m_first->item;
			*insert = m_first->insert;
			return;
		}

	HanditemNode *currentNode = m_first;

	while (currentNode)
	{
		if (currentNode->item->RetHandle()==currentItem->RetHandle())
		{
			if (currentNode->nextNode)
			{
				*nextItem = currentNode->nextNode->item;
				*insert = currentNode->nextNode->insert;
				return;
			}
			else
			{
				*nextItem = NULL;
				*insert = NULL;
				return;
			}
		}

		currentNode = currentNode->nextNode;
	}

	*nextItem = NULL;
	*insert = NULL;
	return;
}


//removes all entities in current list.
bool
HanditemCollection::ClearList()
{
	HanditemNode *next;

	while (m_first) {
		next = m_first->nextNode;
		delete m_first;
		m_first = next;
	}

	m_first = NULL;
	m_last = NULL;

	m_count = 0;

	return true;
}

// The following is a helper function for CopyList.
// Right now it is declared as static, but if it is to be used more generally
// we will change its status.


// The technique used here is similar to that used in EntityRenderer::OutputPolyline
// of course

// Returns true if the given handitem is at least partially in the given view
// Returns false if no part of the given handitem is in the current view
static bool IsOnScreen(db_handitem * hip,// Input:  the handitem
                       struct gr_view * gView) // Input:  the current view
{
	bool bOnScreen = false;

	int fi1;
	int fi2;
	int nxt;
	int ndims;
	int clipresult;
	int face;

	int tmp_pixels[4];
	sds_point projectedPoint, prevProjectedPoint;

	struct gr_displayobject* tdo1 = (gr_displayobject*)hip->ret_disp();

	if (tdo1 != NULL && !(tdo1->type & DISP_OBJ_SPECIAL))
	{
		// the following two lines are implemented the same as
		// in EntityRenderer::drawDisplayObjects
		ndims = ((tdo1->type & DISP_OBJ_PTS_3D) != 0) + 2;
		face  = ((tdo1->type & DISP_OBJ_PTS_CLOSED) != 0);
		if(ndims == 3)
			gr_ucs2rp(&tdo1->chain[0], projectedPoint, gView);
        fi2 = (tdo1->npts - 2 + face) * ndims;

		for (fi1 = 0; fi1 <= fi2; fi1 += ndims)
		{
			if (face && fi1 == fi2)
				nxt = 0;
			else
				nxt = fi1 + ndims;

			if(ndims == 3)
			{
				ic_ptcpy(prevProjectedPoint, projectedPoint);
				gr_ucs2rp(&tdo1->chain[nxt], projectedPoint, gView);
			}
			else
			{
				ic_ptcpy(prevProjectedPoint, &tdo1->chain[fi1]);
				ic_ptcpy(projectedPoint, &tdo1->chain[nxt]);
			}
			//clipresult = gr_vect_rp2pix(&tdo1->chain[fi1], &tdo1->chain[nxt], tmp_pixels, gView);
			clipresult = gr_vect_rp2pix(prevProjectedPoint, projectedPoint, tmp_pixels, gView);

			if (clipresult == 0 || clipresult == 2)
			{
				bOnScreen = true;
				break;
			}
		}
	}

	return bOnScreen;
}

//Replaces the current list with the pointers to items in the new list. Entities are not actually copied.
//Destroy indicates whether the handitems should be destroyed when the list or node is destroyed.
//If you want the list filtered input the filter item.
//space removes any items that are not the same space specified - if NONE no filter is used.
//deletedItems removes all deleted items from the list
//invisibleItems removes all invisible items from the list
bool
HanditemCollection::CopyList(db_drawing *drawing, db_handitem *list, bool destroyHI, SpaceFilter space,
							 bool deletedItems, bool invisibleItems)
{
	ClearList();

	db_handitem *currentHip = list;

	struct gr_view * gView = SDS_CURGRVW;

	//go through list until finding an item that passes filter.
	while (currentHip)
	{
		if (FilterItem(currentHip, space, deletedItems, invisibleItems))
		{
			if (IsOnScreen(currentHip, gView))
			{
				AddItem(drawing, currentHip, destroyHI);
			}
		}

		currentHip = currentHip->next;
	}

	return true; //copied all entities that passed filter or none if none input.
}


//checks a single item to see if it meets the filter requirements.
//space removes any items that are not the same space specified - if NONE no filter is used.
//deletedItems removes all deleted items from the list
//invisibleItems removes all invisible items from the list
//returns true if passes filtering
bool
HanditemCollection::FilterItem(db_handitem *item, SpaceFilter space, bool deletedItems, bool visibleItems)
{
	if (!item)
		return false;

	if (deletedItems)
		if (item->ret_deleted())
			return false;

	switch (space)
	{
	case MODELSPACE:
		if (item->ret_pspace())
			return false;
		break;

	case PAPERSPACE:
		//if item is not in paperspace filter fails.
		if (!item->ret_pspace())
			return false;
		break;
	//do nothing if NONE is specified.
	}

	if (visibleItems)
		if (item->ret_invisible())
			return false;


	return true;
}


//removes all entities of input type (i.e. DB_LINE) from the list.  Note - this function does not consider entities that
//may be in blocks.  A separate method can be written later if that functionality is desired.
bool
HanditemCollection::FilterEntity(int dbType)
{
	HanditemNode *currentNode = m_first;
	HanditemNode *previous = NULL;
	HanditemNode *next = NULL;

	while (currentNode)
	{
		if (dbType == currentNode->item->ret_type())
		{
			next = currentNode->nextNode;
			RemoveItem(previous, currentNode);
			currentNode = next;
		}
		else
		{
			previous = currentNode;
			currentNode = currentNode->nextNode;
		}
	}

	return NULL;
}


bool
HanditemCollection::FilterEntity(C3Point ptNormal)
	{

	HanditemNode *currentNode = m_first;
	HanditemNode *previous = NULL;
	HanditemNode *next = NULL;

	C3Point ptOther;
	sds_point p;
	BOOL bEntityWithExtrusion;
	while (currentNode)
	{
		bEntityWithExtrusion = currentNode->item->get_210(p);
		SdsToRpt(p, ptOther);
		//Use FUZZ factor to determin if the current entity has the same extrusion vector as the current ucs z-axis.
		//Comparing the result to 0.0 does not work!
		if (bEntityWithExtrusion && ((ptNormal ^ ptOther).Norm() > FUZZ_GEN))
		{
			int type = currentNode->item->ret_type();
			// <alm: if we should remove complex entity, remove all its subents>
			if (type == DB_POLYLINE)
			{
				bool bBreakLoop = false; //flag for exiting the loop
				do
				{
					// Remember next item
					next = currentNode->nextNode;
					// Remove current
					RemoveItem(previous, currentNode);
					currentNode = next;

					// Break loop if it's the end of collection or SEQEND was achieved
					if (bBreakLoop || !currentNode)
						break;

					// Break loop if current item doesn't belong to initial polyline
					type = currentNode->item->ret_type();
					if (type != DB_VERTEX || type != DB_SEQEND)
						break;
					
					bBreakLoop = currentNode->item->ret_type() == DB_SEQEND;
				}
				while (true);
			}
			else
			{
				next = currentNode->nextNode;
				RemoveItem(previous, currentNode);
				currentNode = next;
			}
		}
		else
		{
			previous = currentNode;
			currentNode = currentNode->nextNode;
		}
	}

	return true;
	}


//Resets whether you want to keep a single handitem or delete it when the list is destroyed
bool
HanditemCollection::SetDestroyHI (db_handitem* item, bool destroyHI)
{
	if (!item)
		return false;

	HanditemNode *current = m_first;

	while (current)
	{
		if (current->item->RetHandle()==item->RetHandle()) //item found
		{
			current->destroyHI = destroyHI;
			return true;
		}

		current = current->nextNode;
	}
	return false;
}


//Resets whether you want to keep or delete all handitems in the list on destruction.
bool
HanditemCollection::SetListDestroyHI (db_handitem* item, bool destroyHI)
{
	if (!item)
		return false;

	HanditemNode *current = m_first;

	while (current)
	{
		current->destroyHI = destroyHI;
		current = current->nextNode;
	}

	return true;
}


//Recursive function that removes all inserts from the list and replaces the inserts with the entities in the block.
//Expanded entities are destroyed when the list is deleted since these entities do not exist in
//the drawing.  The user may reset this value for any nodes that it would like to keep.
bool
HanditemCollection::ExpandBlocks(db_drawing *drawing)
{
	HanditemNode *current = m_first;

	//iterate through list once to expand entities.
	while (current)
	{
		if (current->item->ret_type() == DB_INSERT)
		{
			db_handitem* insert = current->item;

			//set psflag & rbinherit
			int pspace = insert->ret_pspace();

			resbuf *rbinherit = NULL;

			//find block from insert
			char *blockName;
			insert->get_2(&blockName);
			db_blocktabrec *blockTable = (db_blocktabrec*) drawing->tblsearch(DB_BLOCKTAB, blockName, NULL);

			if (!blockTable)
			{
				current = current->nextNode;
				continue;
			}

			//get first entity in block.
			sds_name firstNameInBlock;
            blockTable->get_firstentinblock(firstNameInBlock, drawing);

			if (firstNameInBlock[0] == 0)
			{
				current = current->nextNode;
				continue;
			}

			//get block insert point
			sds_point blkInsPt;
			blockTable->get_10(blkInsPt);

			//get insert values
			sds_point insPt, extrusion;
			sds_real xScale, yScale, zScale, rotAngle;

			insert->get_10(insPt);
			insert->get_41(&xScale);
			insert->get_42(&yScale);
			insert->get_43(&zScale);
			insert->get_50(&rotAngle);
			insert->get_210(extrusion);

			//handle minsert
			//including colct, rowct, colspace, rowspace

			//Explode block and add new entities to list.
			resbuf *subEntities = NULL;

			//track last entity so next pointers can be set up for polylines.
			db_handitem *last = NULL;
            do{
				int rc = cmd_explode_makesubents(firstNameInBlock, insPt, blkInsPt, xScale, yScale, zScale, rotAngle, extrusion, pspace,
									 			 rbinherit, &subEntities);

				if (rc >= 0 && subEntities)
				{

					//*** Make it.
					//THIS IS A BUG!  If the subEntities is an insert in an XRef, the "drawing" is invalid.
					//db_makehipfromrb will do an entmod on the insert and pass in the wrong drawing pointer.
					//so insert->set_2 is called, and fails because this block is not in the "drawing".
					db_handitem *nextNewEnt = db_makehipfromrb(subEntities, drawing);

					if (!nextNewEnt)
						continue;

//4M Item:138->
					db_handitem	*LayerHip = nextNewEnt->ret_layerhip();
					if(LayerHip)
					{
//Check if layer is frozen
						int flags;
                  if(LayerHip->get_70(&flags)){
							if(flags & IC_LAYER_FROZEN)
								continue;
                  }
//Check if layer is Off
                  int color;
                  if (LayerHip->get_62(&color)){
                     if (color<0)
                        continue;
                  }
					}
//<-4M Item:138
                    int retType = nextNewEnt->ret_type();
					if (((retType == DB_VERTEX) || (retType == DB_SEQEND)) && last)
					{
						last->next = nextNewEnt;
					}

					// Entities are added to the end of the list so nested inserts will also be expanded.
					// These entitites should be destroyed on release as they are not true entities in the drawing
					AddItem(drawing, nextNewEnt, true, insert);

					last = nextNewEnt;
				}

				IC_RELRB(subEntities);
			} while (RTNORM==sds_entnext_noxref(firstNameInBlock,firstNameInBlock));
		}

		current = current->nextNode;
	}

	//Filter out inserts from list
	FilterEntity(DB_INSERT);

	return true;
}


/////////////

//Recursive function that removes all inserts from the list and replaces the inserts with the entities in the block.
//Expanded entities are destroyed when the list is deleted since these entities do not exist in
//the drawing.  The user may reset this value for any nodes that it would like to keep.
bool
CHanditemCollectionWXRefs::ExpandBlocks()
{
	HanditemNode *current = m_first;

	//iterate through list once to expand entities.
	while (current)
	{
		if (current->item->ret_type() == DB_INSERT)
		{
			db_handitem* insert = current->item;
			db_drawing *insDrawing = current->m_pDrawing;

			//set psflag & rbinherit
			int pspace = insert->ret_pspace();

			resbuf *rbinherit = NULL;

			//find block from insert
			char *blockName;
			insert->get_2(&blockName);
			db_blocktabrec *blockTable = (db_blocktabrec*) insDrawing->tblsearch(DB_BLOCKTAB, blockName, NULL);

			if (!blockTable)
			{
				current = current->nextNode;
				continue;
			}

			//get first entity in block.
			sds_name firstNameInBlock;
            blockTable->get_firstentinblock(firstNameInBlock, insDrawing);

			if (firstNameInBlock[0] == 0)
			{
				current = current->nextNode;
				continue;
			}

			//get block insert point
			sds_point blkInsPt;
			blockTable->get_10(blkInsPt);

			//get insert values
			sds_point insPt, extrusion;
			sds_real xScale, yScale, zScale, rotAngle;

			insert->get_10(insPt);
			insert->get_41(&xScale);
			insert->get_42(&yScale);
			insert->get_43(&zScale);
			insert->get_50(&rotAngle);
			insert->get_210(extrusion);

			//handle minsert
			//including colct, rowct, colspace, rowspace

			//Explode block and add new entities to list.
			resbuf *subEntities = NULL;

			//track last entity so next pointers can be set up for polylines.
			db_handitem *last = NULL;
            do{
				int rc = cmd_explode_makesubents(firstNameInBlock, insPt, blkInsPt, xScale, yScale, zScale, rotAngle, extrusion, pspace,
									 			 rbinherit, &subEntities);

				if (rc >= 0 && subEntities)
				{

					//*** Make it.
					//THIS IS A BUG!  If the subEntities is an insert in an XRef, the "drawing" is invalid.
					//db_makehipfromrb will do an entmod on the insert and pass in the wrong drawing pointer.
					//so insert->set_2 is called, and fails because this block is not in the "drawing".

					db_drawing *drawing;

					if( (((db_handitem*)firstNameInBlock[0])->ret_type() == DB_INSERT) &&
						(firstNameInBlock[1] != (long)m_pDrawing) )
					{
						drawing = (db_drawing*)firstNameInBlock[1];
					}
					else
					{
						drawing = m_pDrawing;
					}

					db_handitem *nextNewEnt = db_makehipfromrb(subEntities, drawing);

					if (!nextNewEnt)
						continue;

//4M Item:138->
					db_handitem	*LayerHip = nextNewEnt->ret_layerhip();
					if(LayerHip)
					{
//Check if layer is frozen
						int flags;
                  if(LayerHip->get_70(&flags)){
							if(flags & IC_LAYER_FROZEN)
								continue;
                  }
//Check if layer is Off
                  int color;
                  if (LayerHip->get_62(&color)){
                     if (color<0)
                        continue;
                  }
					}
//<-4M Item:138
                    int retType = nextNewEnt->ret_type();
					if (((retType == DB_VERTEX) || (retType == DB_SEQEND)) && last)
					{
						last->next = nextNewEnt;
					}

					// Entities are added to the end of the list so nested inserts will also be expanded.
					// These entitites should be destroyed on release as they are not true entities in the drawing
					AddItem(drawing, nextNewEnt, true, insert);

					last = nextNewEnt;
				}

				IC_RELRB(subEntities);
			} while (RTNORM==sds_entnext_noxref(firstNameInBlock,firstNameInBlock));
		}

		current = current->nextNode;
	}

	//Filter out inserts from list
	FilterEntity(DB_INSERT);

	return true;
}
