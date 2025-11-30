/* D:\ICADDEV\PRJ\LIB\DB\FIXUP1005.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * NEEDS COMPLETION . . . 1005 fixup stuff
 *
 * 
 */ 

#include "db.h"
#include "fixup1005.h"
#include <stdlib.h>


	// ************************************
	//	FixupHandleTableEntry is a pair of handles -- during fixup, replace the old with the new


FixupHandleTableEntry:: FixupHandleTableEntry(void)
	{
	oldHandle.Nullify(); newHandle.Nullify();
	}

FixupHandleTableEntry:: FixupHandleTableEntry(db_objhandle theoldhandle, db_objhandle thenewhandle)
	{
	oldHandle=theoldhandle;
	newHandle=thenewhandle;
	}



	// ************************************
	// FixupHandleTable adds entries to a dynamic array
	// Set granularity to expand by about 64K
	// When searching, the table is first sorted if necessary

const int	cluster = 0x10000;	// 64K
FixupHandleTable:: FixupHandleTable(void) : m_table( cluster/sizeof( FixupHandleTableEntry))
	{
	}

FixupHandleTable::~FixupHandleTable(void)
	{
	}

void
FixupHandleTable:: Add(db_objhandle theoldhandle,db_objhandle thenewhandle)
	{
	FixupHandleTableEntry entry(theoldhandle,thenewhandle);
	m_table.Add( entry);
	}


bool
FixupHandleTable:: Get( unsigned int nItem, FixupHandleTableEntry *theentry)
	{
	return m_table.Get( nItem, theentry);
	}

bool
FixupHandleTable::GetNewHandle(
	db_objhandle oldHandle,				// in
	db_objhandle *newHandle)					// out
	{
	FixupHandleTableEntry handleTableEntry;
	unsigned int nHandleCount = m_table.Number();

		// walk down all of the handle entries and get the
		// new handle corresponding to the old handle
	for (unsigned int i = 0; i < nHandleCount; i++)
		{
		ASSERT(m_table.Get(i, &handleTableEntry));
		if (!(m_table.Get(i, &handleTableEntry)))
			continue;

		if (oldHandle==handleTableEntry.oldHandle)
			{
			*newHandle=handleTableEntry.newHandle;
			return true;
			}
		}

	return false;
	}


	// ************************************
	//	FixupHipTableEntry is hip which needs to have a hip pointer fixed up


FixupHipTableEntry:: FixupHipTableEntry(void)
	{
	hip=NULL;
	}

FixupHipTableEntry:: FixupHipTableEntry( db_handitem *thehip)
	{
	hip=thehip;
	}



	// ************************************
	// FixupHipTable adds entries to a dynamic array
	// Set granularity to expand by about 64K
	// When searching, the table is first sorted if necessary

const int	cluster2 = 0x10000;	// 64K
FixupHipTable:: FixupHipTable() : m_table( cluster2/sizeof( FixupHipTableEntry))
	{
	}

FixupHipTable::~FixupHipTable()
	{
	}

void
FixupHipTable:: Add( db_handitem *thehip)
	{
	FixupHipTableEntry entry(thehip);
	m_table.Add( entry);
	}


bool
FixupHipTable:: Get( unsigned int nItem, FixupHipTableEntry *theentry)
	{
	return m_table.Get( nItem, theentry);
	}


// *************************************************************
// Fixup1005HipTable
// *************************************************************

bool
Fixup1005HipTable:: ApplyFixups(db_drawing *dp)
	{
//	ads_name entName;
	bool bDoEntmod;
	db_objhandle handle;
	struct resbuf *rbList, *rbTmp, rbApps;
	FixupHipTableEntry pHipTableEntry;
	unsigned int nHipCount = m_table.Number();

	ASSERT(NULL != dp);

		// need to update the 1005s in all of the xdata
	rbApps.restype = RTSTR;
	rbApps.resval.rstring = "*"/*DNT*/;
	
	for (unsigned int i = 0; i < nHipCount; i++)
		{
		ASSERT(this->Get(i, &pHipTableEntry));
		if (!(this->Get(i, &pHipTableEntry)))
			continue;

		ASSERT(pHipTableEntry.hip->RetHandle()!=NULL);
		handle=pHipTableEntry.hip->RetHandle();
		if (handle==NULL) continue;

//		if (RTNORM != sdsengine_handent(szHandle, entName))
//		if (RTNORM != sds_handent(szHandle, entName))
//			continue;

		rbList = pHipTableEntry.hip->entgetx(&rbApps, dp);
		if (rbList)
			{
				// walk down the entity and replace each old handle
				// in a 1005 resbuf with the appropriate new handle
			bDoEntmod = false;
			rbTmp = rbList;
			while (rbTmp)
				{
				if (1005 == rbTmp->restype)
					{
					if (dp->GetNew1005Handle(rbTmp->resval.rstring, &handle))
						{
						delete [] rbTmp->resval.rstring;
						char asciihandle[17];
						handle.ToAscii(asciihandle);
						rbTmp->resval.rstring = new char [strlen(asciihandle) + 1];
						strcpy(rbTmp->resval.rstring, asciihandle);
						bDoEntmod = true;
						}
					}

					rbTmp = rbTmp->rbnext;
				}
		
				// do an entmod to apply the changes
			if (bDoEntmod)
				pHipTableEntry.hip->entmod(rbList, dp);

			sds_relrb(rbList);
			}
		}

	return true;
	}

// ***********************************************************************
// Reactor Fixups
// ***********************************************************************

void ReactorFixups::ApplyFixups(db_drawing *drawing)
{
	m_handitemsTable.ApplyFixups(drawing, &m_handlesMap);
}

void ReactorFixups::AddHanditem(db_handitem *pHandItem)
{
	m_handitemsTable.Add(pHandItem);
}

void ReactorFixups::AddHandleMap (db_objhandle oldhandle, db_objhandle newhandle)
{
	m_handlesMap.Add(oldhandle, newhandle);
}



// *************************************************************
// FixupReactorTable
// *************************************************************

bool
FixupReactorTable:: ApplyFixups(db_drawing *dp, FixupHandleTable *handleTable)
	{
	bool bDoEntmod;
	db_objhandle handle;
	struct resbuf *rbList, *rbTmp, rbApps;
	FixupHipTableEntry pHipTableEntry;
	unsigned int nHipCount = m_table.Number();

	ASSERT(NULL != handleTable);

	// need to update all the 330/340 & reactors
	rbApps.restype = RTSTR;
	rbApps.resval.rstring = "*"/*DNT*/;
	
	for (unsigned int i = 0; i < nHipCount; i++)
		{
		ASSERT(this->Get(i, &pHipTableEntry));
		if (!(this->Get(i, &pHipTableEntry)))
			continue;

		ASSERT(pHipTableEntry.hip->RetHandle()!=NULL);
		handle=pHipTableEntry.hip->RetHandle();
		if (handle==NULL) continue;

		// modify reactors 
		ReactorSubject *reactors = pHipTableEntry.hip->getReactor();
	
		reactors->ResetIterator();
			
		db_objhandle oldHandle, newHandle;
		while (reactors->GetNextHandle(oldHandle))
		{
			if (!handleTable->GetNewHandle(oldHandle, &newHandle))
			{
				// if the reactor does not have a match, remove it
				reactors->DetachReactor(oldHandle);
				reactors->ResetIterator();
			}
			else
			{
				reactors->SetHandle(oldHandle, newHandle, dp);
			}
		}

		// modify 330/340 codes  walk the resbuf list because different entities store handles
		// in different places (i.e. hatch/leader)
		rbList = pHipTableEntry.hip->entgetx(&rbApps, dp);
		if (rbList)
			{
			// walk down the entity and replace each 330/340 old handle
			// with the appropriate new handle
			bDoEntmod = false;
			rbTmp = rbList;
			bool bReactors = false;
			while (rbTmp)
				{
				if(102 == rbTmp->restype)
					{
					if(!strcmp(rbTmp->resval.rstring, db_handitem::db_preactmarker))
						bReactors = true;
					else if(bReactors && !strcmp(rbTmp->resval.rstring, "}"))
						bReactors = false;
					}
				else if ((!bReactors) && (330 == rbTmp->restype) || (340 == rbTmp->restype))
					{
					db_handitem *item = (db_handitem*) rbTmp->resval.rlname[0];
					if(item && item != pHipTableEntry.hip->m_parentBlock) /*D.Gavrilov*/// in some cases there is no associations with a leader
					{
						if (handleTable->GetNewHandle(item->RetHandle(), &handle))
							{
							rbTmp->resval.rlname[0] = (long) dp->handent(handle);
							bDoEntmod = true;
							}
						else {
							pHipTableEntry.hip->BreakAssociation();
							break;
							}
						}
					}
					rbTmp = rbTmp->rbnext;
				}

			// do an entmod to apply the changes
			if (bDoEntmod)
			{
				// fixing pointers so don't want to reassoc.
				pHipTableEntry.hip->SetCreateAssocFlag(false);

				pHipTableEntry.hip->entmod(rbList, dp);

				// set back to true;
				pHipTableEntry.hip->SetCreateAssocFlag(true);
			}

			sds_relrb(rbList);
			}
		}

	return true;
	}

