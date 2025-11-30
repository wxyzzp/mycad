/* F:\ICADDEV\PRJ\LIB\CMDS\UTILITIES.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */


#include "cmds.h"/*DNT*/
#include "IcadApi.h"/*DNT*/
#pragma warning(disable:4786)
#include <set> /*DNT*/
#include <map> /*DNT*/
#include <algorithm> /*DNT*/
#include <list> /*DNT*/

//helper functions for boundary and bhatch
//it will draw or erase an entity
//see sds_redraw for explaination of howToDraw.
void cmd_redrawEntity (db_handitem *hip, int howToDraw)
{
	sds_name ename;
	ename[0]=(long)hip;
	ename[1]=(long)SDS_CURDWG;

	SDS_UpdateEntDisp(ename, 3);
	sds_redraw(ename, howToDraw);
}

void cmd_updateReactors (db_handitem *entity)
{
	/*DG - 29.1.2002*/// An abnormal situation, so assert and (of course) exit.
	if(!entity)
	{
		_ASSERTE(!"WARNING: null pointer!");
		return;
	}

	//update Reactors
	ReactorSubject *reactors = entity->getReactor();
	db_handitem *observer;
	reactors->ResetIterator();

	while (reactors->GetNextHanditem(SDS_CURDWG, &observer))
	{
		cmd_redrawEntity(observer, IC_REDRAW_UNDRAW);
	}

	entity->CheckReactors(SDS_CURDWG);

	reactors->ResetIterator();

	while (reactors->GetNextHanditem(SDS_CURDWG, &observer))
	{
		cmd_redrawEntity(observer, IC_REDRAW_DRAW);
	}
}

void cmd_updateSSReactors(const sds_name selSet)
{
	sds_name	ename;

	for(long i = 0L; sds_ssname(selSet, i, ename) == RTNORM; ++i)
		cmd_updateReactors((db_handitem*)ename[0]);
}

// <alm>
// just shorter name
typedef std::pair<db_handitem*, db_handitem*> handitems_t;

// Helper structure,
// implementing comparing functionality for subtracting sets of objects
struct SCompAcceptorAndSender
{
	bool operator () (const handitems_t& as, db_handitem* s)
	{
		return as.first < s;
	}

	bool operator () (db_handitem* s, const handitems_t& as)
	{
		return s < as.first;
	}
};
//</alm>

// Some operations require certain entities to break the associations if their associated entities
// are not part of the selection set.  This gives an entity a chance to do this if it should. Also
// update any reactors after a move is complete.
void cmd_checkReactors (const sds_name selSet)
{
	//<alm: the change was propesed to avoid redundant updating
	//		of one entity over several reactors>
#if 0
	// old version
	sds_name	ename;

	for(long i = 0L; sds_ssname(selSet, i, ename) == RTNORM; ++i)
	{
		((db_handitem*)ename[0])->CheckAssociationInSet(SDS_CURDWG, selSet);
		cmd_updateReactors((db_handitem*)ename[0]);
	}
#else
	// new version
	using namespace std;
	// the senders of events
	set<db_handitem*> senders;

	// At first, check associations in selection set
	// and fill std::set of event senders
	sds_name ename;
	for (long i = 0l; sds_ssname(selSet, i, ename) == RTNORM; ++i)
	{
		db_handitem* hip = (db_handitem*)ename[0];
		hip->CheckAssociationInSet(SDS_CURDWG, selSet);
		// And put the handitem to the set
		senders.insert(hip);
	}

	// Form set of event recievers,
	// here we will optimize count of recievers, by ignoring the duplicates
	// i.e. we consider, events sending by different senders to the same reciever as one event
	// it's can be true for some type of objects, for example for HATCH
	set<handitems_t> acceptors;
	map<db_handitem*, db_handitem*> acceptorToSender;
	for (std::set<db_handitem*>::iterator is = senders.begin();
		 is != senders.end(); ++is)
	{
		ReactorSubject* reactors = (*is)->getReactor();
		reactors->ResetIterator();
		db_handitem* acceptor;
		while (reactors->GetNextHanditem(SDS_CURDWG, &acceptor))
		{
			handitems_t as;
			as.first = acceptor;
			switch (acceptor->ret_type())
			{
			case DB_HATCH:
				as.second = 0;
				break;
			default:
				as.second = *is;
			};
			if (!as.second)
			{
				acceptorToSender[acceptor] = *is;
			}

			acceptors.insert(as);
		}
	}

	// Other optimization, to eliminate recievers that are senders.
	// Subtract set of senders from set of recievers and put result into updates
	list<handitems_t> updates;
	SCompAcceptorAndSender comp;
	set_difference(acceptors.begin(), acceptors.end(),
				   senders.begin(), senders.end(),
				   back_inserter(updates),
				   comp);

	// Update objects from updates
	for (std::list<handitems_t>::iterator il = updates.begin();
		 il != updates.end(); ++il)
	{
		// Undraw entity
		cmd_redrawEntity(il->first, IC_REDRAW_UNDRAW);

		// Update acceptor
		if (il->second)
		{
			il->first->ReactorUpdate(il->second->RetHandle());
		}
		else
		{
			il->first->ReactorUpdate(acceptorToSender[il->first]->RetHandle());
		}

		// Draw entity
		cmd_redrawEntity(il->first, IC_REDRAW_DRAW);
	}
#endif
}

bool  IsInternalArrow(char* arrowHeadBlkName)
{
	// These arrow heads are internal defaults. They will never really
	// exist. They are created on the fly, so don't look them up.
	// This string shoud NOT be translated!
	char nameUpr[256];
	strncpy( nameUpr, arrowHeadBlkName, 255 );
	strupr( nameUpr );
	char* defaultArrows = "_NONE _CLOSED _DOT _CLOSEDBLANK _OBLIQUE _ARCHTICK _OPEN _ORIGIN _OPEN90 _OPEN30 _DOTBLANK _DOTSMALL _BOXFILLED _BOXBLANK _DATUMFILLED _DATUMBLANK _INTEGRAL"/*DNT*/;

	return strstr(defaultArrows, nameUpr) != NULL;
}
