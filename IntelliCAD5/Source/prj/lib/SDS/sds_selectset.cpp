// File: sds_selectset.cpp
// Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
//
// Selection Set Functions
//

#include "Icad.h"/*DNT*/
#include "IcadDoc.h"/*DNT*/
#include "sdsapplication.h"/*DNT*/
#include "lisp.h"/*DNT*/
#include "cmds.h"/*DNT*/
#include "commandqueue.h"/*DNT*/
#include "IcadView.h"/*DNT*/
#include "osnapLocker.h"/*DNT*/
#include "sdsthreadexception.h"/*DNT*/
#include "sdsthread.h" /*DNT*/
#include "SelectionSet.h"/*DNT*/


CString	g_validSS = ResourceString(IDC_SDS_SELECTSET_THIS_IS_A__0, "This is a valid SS");


/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Find common ents for given ss1 and ss2 and add them to ss if necessary.
 * Returns:	RTNORM when all args are valid and intersection is created if necessary,
 *			RTCAN if there are no common ents in given selsets,
 *			RTERROR otherwise.
 ********************************************************************************/
int
SDS_ssHasIntersection
(
 const sds_name	ss1,							// =>
 const sds_name	ss2,							// =>
 sds_name		ss /* = NULL*/,					// <= resulting ss: it's not cleared here, but ents are added to
 bool			bExtractCommEnts /* = false */	// => 'true' if common ents are to be added to ss
)
{
	if(!ss1 || ss1[1] != (long)(LPCTSTR)g_validSS || !ss2 || ss2[1] != (long)(LPCTSTR)g_validSS)
		return RTERROR;

	// just an optimization here: loop thru the less ss below
	long		len1, len2;
	sds_name	ssLess, ssMore;
	if(sdsengine_sslength(ss1, &len1) != RTNORM || sdsengine_sslength(ss2, &len2) != RTNORM)
		return RTERROR;

	if(len1 < len2)
	{
		sds_name_set(ss1, ssLess);
		sds_name_set(ss2, ssMore);
	}
	else
	{
		sds_name_set(ss2, ssLess);
		sds_name_set(ss1, ssMore);
	}

	int			res = RTCAN;
	long		i = 0L;
	sds_name	ename;

	while(sdsengine_ssname(ssLess, i++, ename) == RTNORM)
		if(sdsengine_ssmemb(ename, ssMore) == RTNORM)
		{
			if(bExtractCommEnts)
			{
				if(SDS_ssadd(ename, ss, ss) != RTNORM)
					return RTERROR;
				res = RTNORM;
			}
			else
				return RTNORM;
		}

	return res;
}

/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Create an array of entities from ss ordered by handle and
 *			store it in ss (for using in ssname).
 * Returns:	RTNORM or RTERROR (when ss is not a valid selection set).
 ********************************************************************************/
int
SDS_ssOrder
(
 sds_name	ss,							// <=>
 bool		bAscending /* = true */		// =>
)
{
	if(!ss || !ss[0] || ss[1] != (long)(LPCTSTR)g_validSS)
		return RTERROR;

	CSelectionSet*	pSS = (CSelectionSet*)ss[0];

	if(!pSS->m_bOrdered)
	{
		pSS->m_asVector.resize(pSS->m_asMap.size());

		long	i = bAscending ? 0 : pSS->m_asMap.size() - 1;
		SSMiter	pMapItem = pSS->m_asMap.begin();

		if(bAscending)
			for( ; pMapItem != pSS->m_asMap.end(); ++pMapItem, ++i)
				pSS->m_asVector[i] = *(pMapItem->second);
		else
			for( ; pMapItem != pSS->m_asMap.end(); ++pMapItem, --i)
				pSS->m_asVector[i] = *(pMapItem->second);

		pSS->m_bOrdered = true;
	}
	else
		if(pSS->m_bAscending != bAscending)
		{
			long	i = 0, j = pSS->m_asVector.size() - 1,
					halfSize = pSS->m_asVector.size() / 2,
					swap;
			for( ; i < halfSize; ++i, --j)
			{
				swap = pSS->m_asVector[i];
				pSS->m_asVector[i] = pSS->m_asVector[j];
				pSS->m_asVector[j] = swap;
			}
		}

	pSS->m_bAscending = bAscending;

	return RTNORM;
}

/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Make the ss looked as unordered.
 * Returns:	RTNORM or RTERROR (when ss is not a valid selection set).
 ********************************************************************************/
int
SDS_ssUnOrder
(
 sds_name	ss	// <=>
)
{
	if(!ss || !ss[0] || ss[1] != (long)(LPCTSTR)g_validSS)
		return RTERROR;

	CSelectionSet*	pSS = (CSelectionSet*)ss[0];

	if(pSS->m_bOrdered)
	{
		pSS->m_asVector.clear();
		pSS->m_bOrdered = false;
	}

	return RTNORM;
}

/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Copy enames from one ss to another.
 * Returns:	-1 in error; size of new ss in success
 ********************************************************************************/
long
SDS_sscpy
(
 sds_name	destSS,		// <=
 sds_name	sourSS		// =>
)
{
	if( !sourSS || !sourSS[0] || sourSS[1] != (long)(LPCTSTR)g_validSS)
		return -1;
	sdsengine_ssfree(destSS);
	if(SDS_ssadd(0, 0, destSS) != RTNORM)
		return -1;

	CSelectionSet	*pDest = (CSelectionSet*)destSS[0], *pSour = (CSelectionSet*)sourSS[0];

	pDest->m_asList     = pSour->m_asList;
	pDest->m_asVector   = pSour->m_asVector;
	pDest->m_drawing    = pSour->m_drawing;
	pDest->m_bOrdered   = pSour->m_bOrdered;
	pDest->m_bAscending = pSour->m_bAscending;

	/*DG - 25.3.2003*/// Send only one SDS_CB_SSMOD after the loop (instead of several SDS_CB_SSADD's as it was before).
	// This is necessary because of performance problems which can occur if SDS_CB_SSADD notifications are sent many times.

	for(SSLiter pListItem = pDest->m_asList.begin(); pListItem != pDest->m_asList.end(); ++pListItem)
		pDest->m_asMap.insert(SSmap::value_type(((db_handitem*)*pListItem)->RetHandle(), pListItem));

	if(SDS_CURDOC && SDS_CURDOC->m_bCallSSchangeCB && sds_name_equal(destSS, SDS_CURDOC->m_pGripSelection))
		SDS_CallUserCallbackFunc(SDS_CBSSCHANGE, (void*)SDS_CB_SSMOD, NULL, NULL);

	return pDest->m_asMap.size();
}

int
SDS_ssadd
(
 const sds_name	entityToAdd,	// =>
 const sds_name	oldSelSet,		// =>
 sds_name		newSelSet		// <=
)
{
	if(!newSelSet)
		return RTERROR;

	if(!oldSelSet || oldSelSet[1] != (long)(LPCTSTR)g_validSS)
	{	// Create a new empty SS
		if(!(newSelSet[0] = (long) new CSelectionSet))
			return RTERROR;

	    newSelSet[1] = (long)(LPCTSTR)g_validSS;
	}
	else
	{
		sds_name_set(oldSelSet, newSelSet);
		((CSelectionSet*)newSelSet[0])->m_asVector.clear();
		((CSelectionSet*)newSelSet[0])->m_bOrdered = false;
	}

	// Add the new entity to the SS
	if(entityToAdd && entityToAdd[0])
	{
		extern bool	SDS_InsideSSwithX;
		// Can't add some entities to a selection set.
		int	entType = IC_TYPE_FROM_ENAME(entityToAdd), vportID;
		if( entType == DB_SEQEND || entType == DB_VERTEX || entType == DB_ENDBLK || entType == DB_ATTRIB ||
			(entType == DB_VIEWPORT && ((db_handitem*)entityToAdd[0])->get_69(&vportID) && vportID == 1 && !SDS_InsideSSwithX) )
			return RTERROR;

		CSelectionSet*	pNewSS = (CSelectionSet*)newSelSet[0];
		pNewSS->m_drawing = entityToAdd[1];
		pNewSS->m_asList.push_back(entityToAdd[0]);
		if(!pNewSS->m_asMap.insert(SSmap::value_type(((db_handitem*)entityToAdd[0])->RetHandle(), --(pNewSS->m_asList.end()))).second)
			pNewSS->m_asList.pop_back();
		else
			if(SDS_CURDOC && SDS_CURDOC->m_bCallSSchangeCB && sds_name_equal(newSelSet, SDS_CURDOC->m_pGripSelection))
				SDS_CallUserCallbackFunc(SDS_CBSSCHANGE, (void*)SDS_CB_SSADD, (void*)entityToAdd, NULL);

	}

	return RTNORM;
}

// This is an SDS External API
int
sdsengine_ssadd(const sds_name nmEntToAdd, const sds_name nmSelSet, sds_name nmNewSet)
{
	return(SDS_ssadd(nmEntToAdd, nmSelSet, nmNewSet));
}

// This is an SDS External API
int
sdsengine_ssdel(const sds_name nmEntToDel, const sds_name nmSelSet)
{
	if(!nmSelSet || !nmEntToDel || nmSelSet[1] != (long)(LPCTSTR)g_validSS)
		return RTERROR;

	CSelectionSet*	pSS = (CSelectionSet*)nmSelSet[0];
	SSMiter			pMapItem = pSS->m_asMap.find(((db_handitem*)nmEntToDel[0])->RetHandle());

	if(pMapItem == pSS->m_asMap.end())
		return RTERROR;

	pSS->m_asList.erase(pMapItem->second);
	pSS->m_asMap.erase(pMapItem);
	pSS->m_asVector.clear();
	pSS->m_bOrdered = false;

	if(SDS_CURDOC && SDS_CURDOC->m_bCallSSchangeCB && sds_name_equal(nmSelSet, SDS_CURDOC->m_pGripSelection))
		SDS_CallUserCallbackFunc(SDS_CBSSCHANGE, (void*)SDS_CB_SSDEL, (void*)nmEntToDel, NULL);


	return RTNORM;
}

// This is an SDS External API
int
sdsengine_sslength(const sds_name nmSelSet, long* plNumberOfEnts)
{
	*plNumberOfEnts = 0L;
	if(!nmSelSet || !nmSelSet[0] || nmSelSet[1] != (long)(LPCTSTR)g_validSS)
		return RTERROR;

	*plNumberOfEnts = ((CSelectionSet*)nmSelSet[0])->m_asList.size();

	return RTNORM;
}

// This is an SDS External API
int
sdsengine_ssmemb(const sds_name nmEntity, const sds_name nmSelSet)
{
	if( !nmSelSet || nmSelSet[1] != (long)(LPCTSTR)g_validSS ||
		((CSelectionSet*)nmSelSet[0])->m_asMap.find(((db_handitem*)nmEntity[0])->RetHandle()) == ((CSelectionSet*)nmSelSet[0])->m_asMap.end() )
		return RTERROR;

	return RTNORM;
}

// This is an SDS External API
int
sdsengine_ssname
(
 const sds_name	nmSelSet,
 long			lSetIndex,
 sds_name		nmEntity
)
{
	nmEntity[0] = nmEntity[1] = 0L;

	if( !nmSelSet || !nmSelSet[0] || nmSelSet[1] != (long)(LPCTSTR)g_validSS ||
		lSetIndex < 0L || ((CSelectionSet*)nmSelSet[0])->m_asList.size() <= lSetIndex)
		return RTERROR;

	CSelectionSet*	pSS = (CSelectionSet*)nmSelSet[0];

	if(!pSS->m_asVector.size())
	{
		pSS->m_asVector.resize(pSS->m_asList.size());
		long	i = 0L;
		SSLiter	pListItem = pSS->m_asList.begin();
		for( ; pListItem != pSS->m_asList.end(); ++pListItem, ++i)
			pSS->m_asVector[i] = *pListItem;
	}

	nmEntity[0] = pSS->m_asVector[lSetIndex];
	nmEntity[1] = pSS->m_drawing;

	return RTNORM;
}

// This is an SDS External API
int
sdsengine_ssnamex(sds_resbuf **pprbEntName, const sds_name nmSelSet, const long iIndex)
{
    // TODO
	return RTERROR;
}

// This is an SDS External API
int
sdsengine_ssgetfirst(sds_resbuf** pprbHaveGrips, sds_resbuf** pprbAreSelected)
{
	ASSERT(OnEngineThread());

	if(!SDS_CURDOC || !pprbHaveGrips || !pprbAreSelected)
		return RTERROR;


	/*DG - 25.1.2002*/// Although we still don't support the behaviour described below,
	// we can implement evaluating of the 1st arg already.
	if(SDS_CURDOC->m_nodeList.GetNoEntities() > 0)
	{
		/*DG - 19.2.2002*/// We must return in the 1st ss only those ents which are not in the 2nd one.
		sds_name	ename, ss = {0L, 0L};
		
		SDS_CURDOC->m_nodeList.begin();

		while(SDS_CURDOC->m_nodeList.getEntityName(ename))
			if(sdsengine_ssmemb(ename, SDS_CURDOC->m_pGripSelection) != RTNORM)
				SDS_ssadd(ename, ss, ss);

		if(ss[0])
		{
			*pprbHaveGrips = sds_newrb(SDS_RTPICKS);
			sds_name_set(ss, (*pprbHaveGrips)->resval.rlname);
		}
		else
			*pprbHaveGrips = 0;
	}
	else
		*pprbHaveGrips = 0;
	/*
	// We don't (yet) support being able to select entities on the screen,
	// then hit one esc, and have the entities unhilighted, but with grips
	// left on. This is what pprbHaveGrips should be pointing at. So until
	// we do support it, this argument will ALWAYS be 0.
	*pprbHaveGrips = 0;
	*/

	// Caller will free pprbAreSelected.
	if(SDS_CURDOC->m_pGripSelection[0] && SDS_CURDOC->m_pGripSelection[1])
	{
		*pprbAreSelected = sds_newrb(SDS_RTPICKS);
        // Fix for bug# 1-51052.
        SDS_sscpy((*pprbAreSelected)->resval.rlname, SDS_CURDOC->m_pGripSelection);
		//sds_name_set(SDS_CURDOC->m_pGripSelection, (*pprbAreSelected)->resval.rlname);
	}
	else
		*pprbAreSelected = 0;

	return RTNORM;
}

// This is an SDS External API
/*DG - 25.1.2002*/// I've rewritten this function for fixing some bugs. Old code is commented out below.
// Main changes:
// - use nmGiveGrips instead of SDS_CURDOC->m_pGripSelection in sds_ssname
// - use '1' in SDS_AddGripNodes for the 1st arg
// - use both SDS_AddGripNodes and SDS_ssadd for the 2nd arg
int
sdsengine_sssetfirst(const sds_name nmGiveGrips, const sds_name nmSelectThese)
{
	ASSERT(OnEngineThread());

	if(!SDS_CURDOC)
		return RTERROR;

	resbuf	rb;
	SDS_getvar(0, DB_QHIGHLIGHT, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
	short	highlight = rb.resval.rint;

	long		i = 0L;
	sds_name	ename;

	SDS_FreeNodeList(SDS_CURDOC);

	if(nmGiveGrips && (!nmSelectThese || SDS_ssHasIntersection(nmGiveGrips, nmSelectThese) == RTCAN))
	{
		rb.resval.rint = 0;
		SDS_setvar(NULL, DB_QHIGHLIGHT, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES, 0);

		while(RTNORM == sds_ssname(nmGiveGrips, i++, ename))
			if(!((db_handitem *)ename[0])->ret_deleted())
				SDS_AddGripNodes(SDS_CURDOC, ename, 1);

		rb.resval.rint = highlight;
		SDS_setvar(NULL, DB_QHIGHLIGHT, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES, 0);

		SDS_DrawGripNodes(SDS_CURDOC);
    }

	if(!nmSelectThese)
	{
	    sds_ssfree(SDS_CURDOC->m_pGripSelection);
		return RTNORM;
	}

	/*DG - 25.3.2003*/// Temporarily disable sending callbacks for the SS change
	// and send one SDS_CB_SSMOD after the loop.
	// This is necessary because of performance problems which can occur
	// if SDS_CB_SSADD notifications are sent many times.
	bool	bCallSSchangeCB = SDS_CURDOC->m_bCallSSchangeCB;
	SDS_CURDOC->m_bCallSSchangeCB = false;
	i = 0L;
	while(RTNORM == sds_ssname(nmSelectThese, i++, ename))
	{
		if(!((db_handitem*)ename[0])->ret_deleted())
		{
			SDS_AddGripNodes(SDS_CURDOC, ename, 1);
			SDS_ssadd(ename, SDS_CURDOC->m_pGripSelection, SDS_CURDOC->m_pGripSelection);
		}
    }
	SDS_CURDOC->m_bCallSSchangeCB = bCallSSchangeCB;
	if(bCallSSchangeCB)
		SDS_CallUserCallbackFunc(SDS_CBSSCHANGE, (void*)SDS_CB_SSMOD, NULL, NULL);

	SDS_DrawGripNodes(SDS_CURDOC);

	return RTNORM;
}

/*
int
sdsengine_sssetfirst(const sds_name nmGiveGrips, const sds_name nmSelectThese)
{
	ASSERT(OnEngineThread());

	if(!SDS_CURDOC)
		return RTERROR;

	if(!nmGiveGrips)
	{
	    sds_ssfree(SDS_CURDOC->m_pGripSelection);
		SDS_sscpy(SDS_CURDOC->m_pGripSelection, (long*)nmGiveGrips);
	}

	if(nmGiveGrips)
	{
		SDS_FreeNodeList(SDS_CURDOC);
		long		i = 0L;
		sds_name	ename;
		while(RTNORM == sds_ssname(SDS_CURDOC->m_pGripSelection, i++, ename))
		{
			// Add the grip nodes. the '2' keeps it from being added
			// the the list of selected entities..
			db_handitem*	elp = (db_handitem *)ename[0];
			if(!elp->ret_deleted())
				SDS_AddGripNodes(SDS_CURDOC, ename, 2);
		}
//4M Item:28->
      SDS_DrawGripNodes(SDS_CURDOC);
//<-4M Item:28
    }

	if(!nmSelectThese)
		return RTNORM;

// AG. I commented out these lines to make it compatible with ads_sssetfirst() behaviour in AUTOCAD
//	if(nmSelectThese!=NULL)
//	{
//	    sds_ssfree(SDS_CURDOC->m_pGripSelection);
//		SDS_sscpy(SDS_CURDOC->m_pGripSelection,(long *)nmSelectThese);
//	}

	resbuf	rb;
	SDS_getvar(0, DB_QHIGHLIGHT, &rb,SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
	if(nmSelectThese && rb.resval.rint)
	{
		long		i = 0L;
		sds_name	ename;
		while(RTNORM == sds_ssname(nmSelectThese, i++, ename))
		{
			// Add this entity to the list of entities with grips
			// turned on. And add to list of selected entities.
			db_handitem*	elp = (db_handitem*)ename[0];
			if(!elp->ret_deleted())
			{
				if(!nmGiveGrips)
					SDS_AddGripNodes(SDS_CURDOC, ename, 1);   // 1-61153 fix
				else
					sds_redraw(ename, 3);
			}
		}
//4M Item:28->
      SDS_DrawGripNodes(SDS_CURDOC);
//<-4M Item:28
    }

	return RTNORM;
}
*/

// This is an SDS External API
int
sdsengine_ssfree(sds_name nmSetToFree)
{
	if(!nmSetToFree || nmSetToFree[1]!=(long)(LPCTSTR)g_validSS)
		return RTERROR;

	bool	bCallCB = SDS_CURDOC && SDS_CURDOC->m_bCallSSchangeCB && sds_name_equal(nmSetToFree, SDS_CURDOC->m_pGripSelection);

	delete (CSelectionSet*)nmSetToFree[0];
	nmSetToFree[0] = nmSetToFree[1] = 0L;

	if(bCallCB)
		SDS_CallUserCallbackFunc(SDS_CBSSCHANGE, (void*)SDS_CB_SSFREE, NULL, NULL);

	return RTNORM;
}


// This is an SDS External API
int
sdsengine_ssget
(
 const char*		szSelMethod,
 const void*		pFirstPoint,
 const void*		pSecondPoint,
 const sds_resbuf*	prbFilter,
 sds_name			nmNewSet
)
{
	ASSERT(OnEngineThread());

	int			ret;
	resbuf		rb;
	sds_name	ename;

	/*DG - 18.12.2001*/// The last param was added by EbaTech, but previous ones made called not with
	// default values. Misprint?
	/*
	//EBATECH(FUTA)
	//ret = sds_pmtssget(0, szSelMethod, (void*)pFirstPoint, pSecondPoint, prbFilter, nmNewSet, 0);
	ret = sds_pmtssget(0, szSelMethod, (void*)pFirstPoint, pSecondPoint, prbFilter, nmNewSet, 0, 0, 0, 0, true);
	*/
	ret = sds_pmtssget(0, szSelMethod, (void*)pFirstPoint, pSecondPoint, prbFilter, nmNewSet, 0, true, false, true, true);

	// This is to fix the problem wherein our version of ssget did not always unhighlight the selection set
	// as AutoCAD does.  We check to make sure that sds_pmtssget did not already unhighlight.  If not, we do.
	if(ret != RTCAN)
	{
		SDS_getvar(0, DB_QHIGHLIGHT, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
		if(rb.resval.rint)
		{
			// Unhighlight selection set of entities.
			for(long i = 0L; sds_ssname(nmNewSet, i, ename) == RTNORM; ++i)
                // Fix for 1-57622 'Image is redrawn at beginning of command' ssget was the command.
                if(((db_handitem*)ename[0])->ret_type() != DB_IMAGE)
                    sds_redraw(ename, 4);
		}
	}

	return ret;
}

extern int	SDS_AtCmdLine;
int			SDS_AtEntGet;
static CString catTwoResourceStrings(CString FirstStr, CString SecondStr);

/*-------------------------------------------------------------------------*//**
This functions is the same as sds_ssget() as long as szSelMsg is NULL.

@author	?
@param	szSelMsg				 =>
@param	szSelMethod				 => A list of special key words that are appended to a subset
									of the selection key words Window, Crossing, Last, Box
									All, Fence, WPolygon, Cpolygon.  If this parm is supplied
									this function will work in the Single mode and return
									as soon as the user selects once.  If you want the single
									mode but do not want keywords pass "".
									NOTE: If the szSelMethod variable starts with '*' as the first
										char in the string, ONLY  the passed string is used and
										NO other options are appended.  In this case, multiple mode
										is enabled
									NOTE: If the szSelMethod variable starts with '&' as the first
										char in the string, multiple mode is forced and the passed
										string is appended onto the beginning of the "BIG LIST" for
										entity selection
@param	pFirstPoint				<=> A **resbuf.  This function will alloc and point pFirstPoint
									to a single point if the user selected one point OR this
									will be a list of points if the user selected Crossing
									(for Stretch).  Box method returns pts in UCS, Crossing in RP!!
									If multiple selection methods were used, they will
									be chained together.  There are no resbuf added to the llist for
									removals.  THE CALLER MUST SET *pFirstPoint TO NULL BEFORE
									CALLING THIS FUNCTION.  If you set any key words this
									function will return RTKWORD.
@param	pSecondPoint			 =>
@param	prbFilter				 =>
@param	nmNewSet				 =>
@param	OmitFromPrevSS			 => flag to omit this selection from being the 'previous
									selection set'. if =0, selected objects are placed in prev ss.
									All calls from ssget pass 0. if !=0, omits selected objects
									from prev ss - this is mainly used where pmtssget is replacing entsel
@param	bIncludeInvisible		 =>
@param	bIncludeLockedLayers	 =>
@return ?
*//*--------------------------------------------------------------------------*/
int						sds_pmtssget
(
const char		 *szSelMsg,
const char		 *szSelMethod,
void			 *pFirstPoint,
const void		 *pSecondPoint,
const sds_resbuf *prbFilter,
sds_name		 nmNewSet,
const int		 OmitFromPrevSS,
bool			 bIncludeInvisible,
bool			 bIncludeLockedLayers,
bool			 bDirectCall,
bool			 bsetPrevSS //EBATECH(FUTA) add
)
{
	COsnapLocker protector;
	ASSERT( OnEngineThread() );
	int ret = RTNORM;

	if( !SDSApplication::GetEngineApplication()->GetMainThread()->IsCurrent() && bDirectCall && SDS_AtCmdLine )
	{
		// redirect call to the Engine posting a job there to prevent deadlock
		CIcadDoc *pDoc = SDS_CURDOC;
		if( !pDoc )
			return RTERROR;

		HRESULT hr = pDoc->AUTO_pmtssget( szSelMsg, szSelMethod, pFirstPoint, pSecondPoint, prbFilter,
											nmNewSet, OmitFromPrevSS, bIncludeInvisible, bIncludeLockedLayers );

		if( FAILED( hr ) )
			ret = RTERROR;

		return ret;
	}

	sds_point pt,pt1,pt2,tpt1,tpt2;
    sds_name ename,ss2,UndoSS,ss;
	sds_real rad;
	int firstprompt=1,singlemode=0,frompipline=0;
    long fl1 = 0L,sslen = 0L;
	double fr1;
    int  mode,automode,UndoMode,pickadd,ptct,fi1,ok;
	int  hlite,env=0,MultiMode=0,SelMode=0,OSnapMode=-1,swBigPrompt=0;
    char fs1[IC_ACADBUF],pmt[IC_ACADBUF];
	CString cp1;
    CString vmod;
    CString vmodes; // Don't set this one here, set is below.

	LOAD_COMMAND_OPTIONS_2(IDC_SDS_SELECTSET__ADD_ADD_T_1)
	CString Pmodes = LOADEDSTRING;

    //BugZilla No. 78254; 12-08-2002
	//CString Smodes=ResourceString(IDC_SDS_SELECTSET__ADD_ADD_T_2, "~Add Add_to_set|+ ~Remove Subtract_from_set|- ~ Location Select_by_Properties...|Properties ~ Selection_Methods...|Dialog ~? ~_+ ~_Remove ~_- ~_ ~_Location ~_Properties ~_ ~_Dialog ~_?" );
	db_drawing *flp;
    resbuf	rb,*rbb,*rbc,*rbt,*pickcur,*picktmp=NULL;
	CommandQueue SaveMenuQueue;

	flp=(SDS_ExternalQuery ? SDS_ExtdwgBeg : SDS_CURDWG);

	if(SDS_ExternalQuery) {
		SDS_getvar(NULL,DB_QVIEWCTR,&rb,SDS_CURDWG,NULL,NULL);
		SDS_setvar(NULL,DB_QVIEWCTR,&rb,SDS_ExtdwgBeg,NULL,NULL,0);
		SDS_getvar(NULL,DB_QVIEWSIZE,&rb,SDS_CURDWG,NULL,NULL);
		SDS_setvar(NULL,DB_QVIEWSIZE,&rb,SDS_ExtdwgBeg,NULL,NULL,0);
		SDS_getvar(NULL,DB_QVIEWMODE,&rb,SDS_CURDWG,NULL,NULL);
		SDS_setvar(NULL,DB_QVIEWMODE,&rb,SDS_ExtdwgBeg,NULL,NULL,0);
		SDS_getvar(NULL,DB_QVIEWDIR,&rb,SDS_CURDWG,NULL,NULL);
		SDS_setvar(NULL,DB_QVIEWDIR,&rb,SDS_ExtdwgBeg,NULL,NULL,0);

		gr_view *view=SDS_CURGRVW;
		if(view) gr_initview(SDS_CURDWG,&SDS_CURCFG,&view,NULL);
	}

	SDS_ExternalQuery=0;

    SDS_getvar(NULL,DB_QTILEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	int tilemode=rb.resval.rint;
	if(SDS_getvar(NULL,DB_QCVPORT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)return(RTERROR);
	int cvport=rb.resval.rint;
	SDS_getvar(NULL,DB_QHIGHLIGHT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	hlite=rb.resval.rint;

    ret=RTNORM;
    if(ss==NULL) return(RTERROR);
    UndoSS[0]=UndoSS[1]=0L;

    if(prbFilter && prbFilter->restype==RTSTR) {
        rbt=(resbuf*)prbFilter;
        rbt->restype=0;
    }

	/*DG 3.10.2001*/// Entities in the ss must be ordered by handles when bit 1 of SORTENTS is set.
	resbuf	sortentsRb;
	// 2002/11/27 SORTENTS is dictionary var
	SDS_getvar("SORTENTS"/*DNT*/, 0, &sortentsRb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
	//SDS_getvar(0, DB_QSORTENTS, &sortentsRb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
    if(szSelMsg==NULL && (szSelMethod || pFirstPoint)) {
		//EBATECH(FUTA) 2001-01-15 Fix: After (ssget ...), Select Previous could not work.
        //return(SDS_PickObjects(szSelMethod,pFirstPoint,(double *)pSecondPoint,prbFilter,nmNewSet,1,flp, bIncludeInvisible, false, bIncludeLockedLayers, false ));
		ret = SDS_PickObjects(szSelMethod,pFirstPoint,(double *)pSecondPoint,prbFilter,nmNewSet,1,flp, bIncludeInvisible, false, bIncludeLockedLayers, bsetPrevSS); // EBATECH(FUTA)
		/*DG 3.10.2001*/// See comments above.
		if(ret == RTNORM && sortentsRb.resval.rint & 1)
			SDS_ssOrder(nmNewSet);
		return ret;
    }

    if(szSelMsg && szSelMethod && *szSelMethod!='*'/*DNT*/ && *szSelMethod!='&'/*DNT*/) singlemode=1;

	if((ret=SDS_getvar(NULL,DB_QPICKADD,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM)goto out;
	pickadd=rb.resval.rint;

	if((ret=SDS_getvar(NULL,DB_QPICKFIRST,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM)goto out;
	if (rb.resval.rint && SDS_CURDOC && RTNORM==sds_sslength(SDS_CURDOC->m_pGripSelection,&fl1) && fl1>0L) {
		if (prbFilter != NULL) {
			fl1--;
			bool emptySet = true;
			while (RTNORM==sds_ssname(SDS_CURDOC->m_pGripSelection,fl1,ename)) {
				db_handitem* elp=(db_handitem *)ename[0];
				if (elp) {
					if (EntityMatchesFilterList(prbFilter,elp,flp)) {
						if (emptySet) {
							SDS_ssadd(ename, NULL, nmNewSet);
							emptySet = false;
						}
						else
							SDS_ssadd(ename, nmNewSet, nmNewSet);
					}
				}
				fl1--;
			}
		}
		else
			SDS_sscpy(nmNewSet,SDS_CURDOC->m_pGripSelection);

		sds_ssfree(SDS_CURDOC->m_pPrevSelection);
		SDS_sscpy(SDS_CURDOC->m_pPrevSelection,SDS_CURDOC->m_pGripSelection);
		sds_ssfree(SDS_CURDOC->m_pGripSelection);
		/*DG 3.10.2001*/// See comments above.
		if(sortentsRb.resval.rint & 1)
			SDS_ssOrder(nmNewSet);
		return(RTNORM);
	}
	if((ret=SDS_getvar(NULL,DB_QOSMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM)goto out;
	if(rb.resval.rint) {
		OSnapMode=rb.resval.rint;
		rb.resval.rint=0;
		if((ret=SDS_setvar(NULL,DB_QOSMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0))!=RTNORM)goto out;
		SDS_UpdateSetvar=(char *)"OSMODE"/*DNT*/;
		ExecuteUIAction( ICAD_WNDACTION_UDCHKSTAT );
		ExecuteUIAction( ICAD_WNDACTION_UDTBSTATE );
		ExecuteUIAction( ICAD_WNDACTION_STATCHANGE );
	}

	ss[1]=0L;
    if(RTNORM!=sds_ssadd(NULL,NULL,ss)) return(RTERROR);

    automode=mode=1;

    if(GetMenuQueue()->IsCommandPause())
		{
		GetMenuQueue()->AdvanceNext();
		SaveMenuQueue.Copy( GetMenuQueue() );
		GetMenuQueue()->Flush();
	    }

    for( ;; ) {
		enametop: ;
		if( GetActiveCommandQueue()->IsNotEmpty()) {
			if(singlemode && env) break;
            if(GetActiveCommandQueue()->PeekItem()->restype==RTPICKS) {
                sslen=fl1=0L;
				sds_sslength(ss,&sslen);
                while(RTNORM==sds_ssname(GetActiveCommandQueue()->PeekItem()->resval.rlname,fl1,ename)) {
                    env=1;
                    SDS_ssadd(ename, ss, ss);
					// Modified PK 31/05/2000 [
					// Reason: Fix for bug no. 45810
					if(hlite) {
						if(mode && (!pickadd || (pickadd && !SDS_CMainWindow->m_fIsShiftDown)))
							SDS_RedrawEntity(ename,IC_REDRAW_HILITE,NULL,NULL,1);
						else
							SDS_RedrawEntity(ename,IC_REDRAW_UNHILITE,NULL,NULL,1);
					}
					// Modified PK 31/05/2000 ]
					++fl1;
                }
				GetActiveCommandQueue()->AdvanceNext();
				continue;
            } else if(GetActiveCommandQueue()->PeekItem()->restype==RTENAME) {
				env=1;
				ename[0] = GetActiveCommandQueue()->PeekItem()->resval.rlname[0];
				ename[1] = GetActiveCommandQueue()->PeekItem()->resval.rlname[1];
				// Modified PK 28/06/2000
				//if(mode && (!pickadd || (pickadd && !SDS_CMainWindow->m_fIsShiftDown))) {
				if(mode) {		// Reason : Fix for bug no. 48143
					sds_ssadd(ename,ss,ss);
					UndoMode=1;
				} else {
					sds_ssdel(ename,ss);
					UndoMode=0;
				}
				if(hlite) {
					// Modified PK 28/06/2000
					//if(mode && (!pickadd || (pickadd && !SDS_CMainWindow->m_fIsShiftDown)))
					if(mode)	// Reason : Fix for bug no. 48143
						SDS_RedrawEntity(ename,IC_REDRAW_HILITE,NULL,NULL,1);
					else
						SDS_RedrawEntity(ename,IC_REDRAW_UNHILITE,NULL,NULL,1);
				}
				GetActiveCommandQueue()->AdvanceNext();
                continue;
            } else if(GetActiveCommandQueue()->PeekItem()->restype==RTLB) {
				GetActiveCommandQueue()->AdvanceNext();
                while( GetActiveCommandQueue()->IsNotEmpty())
					{
                    if(GetActiveCommandQueue()->PeekItem()->restype==RTENAME)
						{
                        env=1;
                        sds_ssadd(GetActiveCommandQueue()->PeekItem()->resval.rlname,ss,ss);
	                    }
                    if(GetActiveCommandQueue()->PeekItem()->restype==RT3DPOINT)
						{
						if(szSelMsg && pFirstPoint && mode)
							{
							picktmp=sds_buildlist(RTSTR,"S"/*DNT*/,RT3DPOINT,GetActiveCommandQueue()->PeekItem()->resval.rpoint,0);
							if(*(resbuf**)pFirstPoint==NULL)
								{
								*(resbuf**)pFirstPoint=pickcur=picktmp;
								pickcur=pickcur->rbnext;
								}
							else
								{
								pickcur->rbnext=picktmp;
								pickcur=pickcur->rbnext->rbnext;
								}
							}
						GetActiveCommandQueue()->AdvanceNext();
						}
                    if(GetActiveCommandQueue()->PeekItem()->restype==RTLE)
						{
						GetActiveCommandQueue()->AdvanceNext();
						break;
	                    }
					GetActiveCommandQueue()->AdvanceNext();
					}  // end of while

                continue;
			}
        } else {
            sds_sslength(ss,&fl1);
			if(fl1>0 && singlemode) { ret=RTNORM; goto out; }
			SDS_getvar(NULL,DB_QCMDECHO,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
            if(CMD_CMDECHO_PRINTF(rb.resval.rint) && !frompipline) {
				if(fl1) cmd_ErrorPrompt(CMD_ERR_ENTSINSET,-1,(void *)fl1);
			}
        }

		switch(SelMode) {
			case 0:
			    if(szSelMsg && szSelMethod && *szSelMethod!='&'/*DNT*/) {
					if(*szSelMethod=='*'/*DNT*/) {
						vmodes=(char *)szSelMethod+1;
					//} else if(*szSelMethod=='&') {
					//	sprintf(vmod,"%s%s",(char *)szSelMethod+1,"Select_all_entities|ALL ~_ Last_entity_in_drawing|Last ~_ Window--Inside|Window Crossing_window|Crossing "
					//								"Outside_window|Outside ~ Window_polygon|WPolygon Crossing_polygon|CPolygon Outside_polygon|OPolygon ~ "
					//								"Window_circle|WCircle Crossing_circle|CCircle Outside_circle|OCircle ~ ~Box Point|POint ~ Fence");
					//    vmodes=vmod;
					} else {
						CString tempCStr;
						LOAD_COMMAND_OPTIONS_3(IDC_SDS_SELECTSET_INITGET_74)
						if (_tcsstr(szSelMethod, "~_"/*DNT*/) != NULL) {
							// If this condition is true, then szSelMethod is a fully globalized string.
							// We will separate it into its localized and globalized parts and splice
							// the ResourceString() we are about to retrieve into the middle of it.
							tempCStr = LOADEDSTRING;
							vmodes = *szSelMethod == 0 ? tempCStr : catTwoResourceStrings(szSelMethod, tempCStr);
						} else {
							tempCStr = LOADEDSTRING;
							vmodes = *szSelMethod == 0 ? tempCStr : catTwoResourceStrings(szSelMethod, tempCStr);
						}
					}
				} else {
					sds_sslength(ss,&fl1);
					if (!fl1) {
						LOAD_COMMAND_OPTIONS_4(IDC_SDS_SELECTSET_SELECT_ALL_3)
						if (firstprompt)		// If it's the very first prompt, don't show 'Remove/-' or 'Undo'
							vmodes = LOADEDSTRING;
						else {				// if it's not the first prompt, but no entities are in the set, then don't show  'Remove/-', but leave 'Undo' in.
			                if(!UndoSS[0] && !UndoSS[1])
								vmodes = LOADEDSTRING;
							else {
								LOAD_COMMAND_OPTIONS_4(IDC_SDS_SELECTSET_SELECT_ALL_4)
								vmodes = LOADEDSTRING;
							}
						}
					} else {
						firstprompt=0;
			            if(!UndoSS[0] && !UndoSS[1]) {
							LOAD_COMMAND_OPTIONS_4(IDC_SDS_SELECTSET_SELECT_ALL_5)
							vmodes = LOADEDSTRING;
						} else {
							LOAD_COMMAND_OPTIONS_4(IDC_SDS_SELECTSET_SELECT_ALL_6)
							vmodes = LOADEDSTRING;
						}

					}
					if(szSelMsg && szSelMethod && *szSelMethod=='&'/*DNT*/ && !fl1){
						//don't append the special '&' list if there are entities in the set.  This would
						//allow the user to pick a key word and return RTKWORD, losing the selected objects!
						vmodes = *szSelMethod == 0 ? vmodes : catTwoResourceStrings(szSelMethod + 1, vmodes);
					}
				}
				sds_initget(RSG_NOLIM|SDS_RSG_NODOCCHG,vmodes);
				SDS_SetCursor(SDS_GetPickCursor());
				if(swBigPrompt) {	// This is for when the user has typed '?' for a list of options at the command line.
					if(mode) {		// User loses the "special prompting" if they've hit "?".
						cp1=ResourceString(IDC_SDS_SELECTSET__NSELECT_E_7, "\nSelect entities:  ALL/Add/Remove/Previous/Last/Window/Crossing/Outside/WPolygon/CPolygon/OPolygon/WCircle/CCircle/OCircle/Box/POint/Fence/AUto/Multiple/Single/PROperties/Dialog/Undo: ");
					} else {
						cp1=ResourceString(IDC_SDS_SELECTSET__NSUBTRACT_8, "\nSubtract entities from selection set:  ALL/Add/Remove/Previous/Last/Window/Crossing/Outside/WPolygon/CPolygon/OPolygon/WCircle/CCircle/OCircle/Box/POint/Fence/AUto/Multiple/Single/PROperties/Dialog/Undo: ");
					}
					swBigPrompt=0;	// Set back to regular prompt until user hits '?' again.
				} else {
					if(mode) {
						if(szSelMsg && *szSelMsg)
                            cp1=(char *)szSelMsg;
						else
                            cp1=ResourceString(IDC_SDS_SELECTSET__NSELECT_E_9, "\nSelect entities: " );
					} else {
						cp1=ResourceString(IDC_SDS_SELECTSET__NSUBTRAC_10, "\nSubtract entities from selection set: " );
					}
				}
				++SDS_AtEntGet;

				if( GetActiveCommandQueue()->IsNotEmpty() ||
					GetMenuQueue()->IsNotEmpty() ||
					lsp_cmdhasmore)
					{
                    frompipline=1;
					}
				else
					{
                    frompipline=0;
					}

				try
				{
					ret=internalGetpoint(NULL,cp1,pt);
				}
				catch( SDSThreadException& theException )
				{
					--SDS_AtEntGet;
					throw theException;
				}

				--SDS_AtEntGet;
				if(ret==RTENAME) { ret=RTNORM; goto enametop; }
	            if(ret==RTKWORD) sds_getinput(fs1);
				if((ret==RTKWORD) && (strsame(fs1,"?"/*DNT*/))) {	// Runs through code above, but with huge prompt showing options.
					swBigPrompt=1;
					continue;
				}
				SDS_SetCursor(IDC_ICAD_CROSS);
				break;
			case 1:
				if(swBigPrompt) {
					sprintf(pmt,ResourceString(IDC_SDS_SELECTSET__N_S_BY_P_11, "\n%s by properties: Add/Remove/Color/LAyer/LType/Name/THickness/TYpe/Value/Width/LOcation/Dialog: " ),(mode ? ResourceString(IDC_SDS_SELECTSET_SELECT_12, "Select" ) : ResourceString(IDC_SDS_SELECTSET_SUBTRACT_13, "Subtract" )));
				}else{
					sprintf(pmt,ResourceString(IDC_SDS_SELECTSET__N_S_BY_P_14, "\n%s by properties: " ),(mode ? ResourceString(IDC_SDS_SELECTSET_SELECT_12, "Select" ) : ResourceString(IDC_SDS_SELECTSET_SUBTRACT_13, "Subtract" )));
				}
				sds_initget((RSG_NOLIM|SDS_RSG_NODOCCHG),Pmodes);
		        ret=sds_getkword(pmt,fs1);
				if(ret==RTNORM) ret=RTKWORD;
				if((ret==RTKWORD) && (strsame(fs1,"?"/*DNT*/))) {	// Runs through code above, but with big prompt showing options.
					swBigPrompt=1;
					continue;
				}else{
					swBigPrompt=0;
					break;
				}
			//BugZilla No. 78254; 12-08-2002
			/*
			case 2:
				sprintf(pmt,ResourceString(IDC_SDS_SELECTSET__N_S_LOCA_15, "\n%s Location/<SQL statement>: " ),(mode ? ResourceString(IDC_SDS_SELECTSET_SELECT_12, "Select" ) : ResourceString(IDC_SDS_SELECTSET_SUBTRACT_13, "Subtract" )));
				sds_initget((RSG_NOLIM|SDS_RSG_NODOCCHG),Smodes);
		        ret=sds_getkword(pmt,fs1);
				if(ret==RTNORM) ret=RTKWORD;
				break;
			*/
		}

        if(ret==RTCAN)  goto out;

        if(ret==RTNONE) {
            if(MultiMode) {
                MultiMode=0;
            } else {
                if(env && RTNORM==sds_sslength(ss,&fl1) && fl1>0L) {
					//this is called sometimes when it shouldn't be.  See
					//	note below
                    rb.restype=RTSHORT;
                    rb.resval.rint=OL_ENAMEVALID;
					SDS_setvar(NULL,DB_QERRNO,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
                }
				ret=RTERROR;
                goto out;
            }
        }

		if(pickadd==0 && !SDS_CMainWindow->m_fIsShiftDown) {
			//*** Unhighlight selection set of entities.
			for(long ssct=0L; sds_ssname(ss,ssct,ename)==RTNORM; ssct++) sds_redraw(ename,IC_REDRAW_UNHILITE);
			sds_ssfree(ss);
			if(RTNORM!=sds_ssadd(NULL,NULL,ss)) { ret=RTERROR;  goto out; }
		}

        if(ret==RTNORM) {
            // User picked one point.
            if(RTERROR==SDS_PickObjects(NULL,pt,NULL,prbFilter,ss2,mode,flp, bIncludeInvisible, false, bIncludeLockedLayers) && automode) {
				if((ret=SDS_getvar(NULL,DB_QPICKAUTO,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM || rb.resval.rint==0) continue;
                ic_ptcpy(pt1,pt);
                goto boxmode;
            }
#define _SUPPORT_GROUPS_
#ifdef _SUPPORT_GROUPS_
			// add all returned selection set
            sslen=fl1=0L;
			sds_sslength(ss,&sslen);
            while(RTNORM==sds_ssname(ss2,fl1,ename))
			{
                env=1;
				if(hlite)
				{
					if(mode && (!pickadd || (pickadd && (!SDS_CMainWindow->m_fIsShiftDown))))
						SDS_RedrawEntity(ename,IC_REDRAW_HILITE,NULL,NULL,1);
					else
						SDS_RedrawEntity(ename,IC_REDRAW_UNHILITE,NULL,NULL,1);
				}
				if(mode && (!pickadd || (pickadd && (!SDS_CMainWindow->m_fIsShiftDown))))
				{
					SDS_ssadd(ename, ss, ss);
					UndoMode=1;
				} else {
					sds_ssdel(ename,ss);
					UndoMode=0;
				}
                ++fl1;
				if((fl1%500)==0) SDS_BitBlt2Scr(1);
            }
#else
            if(RTNORM==sds_ssname(ss2,0L,ename)) {
                env=1;	//I'm not sure we should do this. If the user successfully picked a
						//	point and got an object, setting this means we'll exit with ERRNO set to OL_ENAMEVALID,
						//	which we really should not do
                if(hlite) {
					if(mode && (!pickadd || (pickadd && !SDS_CMainWindow->m_fIsShiftDown)))
						SDS_RedrawEntity(ename,IC_REDRAW_HILITE,NULL,NULL,1);
                    else
						SDS_RedrawEntity(ename,IC_REDRAW_UNHILITE,NULL,NULL,1);
                }
                if(mode && (!pickadd || (pickadd && !SDS_CMainWindow->m_fIsShiftDown))) {
					sds_ssadd(ename,ss,ss);
		            UndoMode=1;
                } else {
					sds_ssdel(ename,ss);
					UndoMode=0;
				}
            }
#endif
			sds_ssfree(UndoSS);
            SDS_sscpy(UndoSS,ss2);
            sds_ssfree(ss2);

			if(szSelMsg && pFirstPoint && mode) {
				//if(*(resbuf**)pFirstPoint) free(*(resbuf**)pFirstPoint);
				picktmp=sds_buildlist(RTSTR,"S"/*DNT*/,RT3DPOINT,pt,0);
				if(*(resbuf**)pFirstPoint==NULL){
					*(resbuf**)pFirstPoint=pickcur=picktmp;
					pickcur=pickcur->rbnext;
				}else{
					pickcur->rbnext=picktmp;
					pickcur=pickcur->rbnext->rbnext;
				}
			}

			if(singlemode) break;
            continue;
        }

        if(ret==RTKWORD) {
			ret=RTNORM;
			if(strisame(fs1,"DIALOG"/*DNT*/)) {
				cmd_ddselect();
				extern sds_name	SDS_EditPropsSS;
				SDS_sscpy(ss2,SDS_EditPropsSS);
				sds_ssfree(SDS_EditPropsSS);
            } else if(strisame(fs1,"LOCATION"/*DNT*/)) {
                SelMode=0;
            } else if(strisame(fs1,"PROPERTIES"/*DNT*/)) {
                SelMode=1;
//            } else if(strisame(fs1,"SQL")) {
//                SelMode=2;
            }else if(strisame(fs1,"MULTIPLE"/*DNT*/)) {
                MultiMode=1;
            } else if(strisame(fs1,"WINDOW"/*DNT*/)) {
				sds_initget(RSG_NOLIM|SDS_RSG_NODOCCHG,NULL);
                if((ret=(internalGetpoint(NULL,ResourceString(IDC_SDS_SELECTSET__NFIRST_C_21, "\nFirst corner of inclusive window: " ),pt1))) != RTNORM) goto out;
                SDS_SetCursor(IDC_ICAD_WINDOW);
				sds_initget(RSG_NOLIM|SDS_RSG_NODOCCHG|SDS_RSG_NOVIEWCHG,NULL);
				ret=sds_getcorner(pt1,ResourceString(IDC_SDS_SELECTSET__NOPPOSIT_22, "\nOpposite corner: " ),pt2);
                SDS_SetCursor(IDC_ICAD_CROSS);
				if(ret!=RTNORM) goto out;
                SDS_PickObjects("WINDOW"/*DNT*/,pt1,pt2,prbFilter,ss2,mode,flp, bIncludeInvisible, false, bIncludeLockedLayers);
				if(szSelMsg && pFirstPoint && mode) {
					//if(*(resbuf**)pFirstPoint) free(*(resbuf**)pFirstPoint);
					//*(resbuf**)pFirstPoint=sds_buildlist(RTSTR,"W",RT3DPOINT,pt1,RT3DPOINT,pt2,0);
					picktmp=sds_buildlist(RTSTR,"WINDOW"/*DNT*/,RT3DPOINT,pt1,RT3DPOINT,pt2,0);
					if(*(resbuf**)pFirstPoint==NULL){
						*(resbuf**)pFirstPoint=pickcur=picktmp;
						pickcur=pickcur->rbnext->rbnext;
					}else{
						pickcur->rbnext=picktmp;
						pickcur=pickcur->rbnext->rbnext->rbnext;
					}
				}
            } else if(strisame(fs1,"OUTSIDE"/*DNT*/)) {
				sds_initget(RSG_NOLIM|SDS_RSG_NODOCCHG|SDS_RSG_NOVIEWCHG,NULL);
                if((ret=(internalGetpoint(NULL,ResourceString(IDC_SDS_SELECTSET__NFIRST_C_24, "\nFirst corner of exclusive window: " ),pt1))) != RTNORM) goto out;
                SDS_SetCursor(IDC_ICAD_OUTSIDE);
				sds_initget(RSG_NOLIM|SDS_RSG_NODOCCHG|SDS_RSG_NOVIEWCHG,NULL);
				ret=sds_getcorner(pt1,ResourceString(IDC_SDS_SELECTSET__NOPPOSIT_22, "\nOpposite corner: " ),pt2);
                SDS_SetCursor(IDC_ICAD_CROSS);
				if(ret!=RTNORM) goto out;
                SDS_PickObjects("OUTSIDE"/*DNT*/,pt1,pt2,prbFilter,ss2,mode,flp, bIncludeInvisible, false, bIncludeLockedLayers);
				if(szSelMsg && pFirstPoint && mode) {
					//if(*(resbuf**)pFirstPoint) free(*(resbuf**)pFirstPoint);
					//*(resbuf**)pFirstPoint=sds_buildlist(RTSTR,"O",RT3DPOINT,pt1,RT3DPOINT,pt2,0);
					picktmp=sds_buildlist(RTSTR,"OUTSIDE"/*DNT*/,RT3DPOINT,pt1,RT3DPOINT,pt2,0);
					if(*(resbuf**)pFirstPoint==NULL){
						*(resbuf**)pFirstPoint=pickcur=picktmp;
						pickcur=pickcur->rbnext->rbnext;
					}else{
						pickcur->rbnext=picktmp;
						pickcur=pickcur->rbnext->rbnext->rbnext;
					}
				}
            } else if(strisame(fs1,"CROSSING"/*DNT*/)) {
				sds_initget(RSG_NOLIM|SDS_RSG_NODOCCHG,NULL);
                if((ret=(internalGetpoint(NULL,ResourceString(IDC_SDS_SELECTSET__NFIRST_C_26, "\nFirst corner of crossing window: " ),pt1))) != RTNORM) goto out;
                SDS_SetCursor(IDC_ICAD_CROSSING);
				//sds_initget(RSG_NOLIM|SDS_RSG_NODOCCHG|SDS_RSG_NOVIEWCHG,NULL);
				sds_initget(RSG_NOLIM|SDS_RSG_NODOCCHG|SDS_RSG_NOVIEWCHG|SDS_RSG_DASH,NULL); // EBATECH(FUTA)
				ret=sds_getcorner(pt1,ResourceString(IDC_SDS_SELECTSET__NOPPOSIT_22, "\nOpposite corner: " ),pt2);
                SDS_SetCursor(IDC_ICAD_CROSS);
				if(ret!=RTNORM) goto out;
                SDS_PickObjects("CROSSING"/*DNT*/,pt1,pt2,prbFilter,ss2,mode,flp,bIncludeInvisible, false, bIncludeLockedLayers);
				if(szSelMsg && pFirstPoint && mode) {
					//if(*(resbuf**)pFirstPoint) free(*(resbuf**)pFirstPoint);
					//*(resbuf**)pFirstPoint=sds_buildlist(RTSTR,"C",RT3DPOINT,pt1,RT3DPOINT,pt2,0);
					picktmp=sds_buildlist(RTSTR,"CROSSING"/*DNT*/,RT3DPOINT,pt1,RT3DPOINT,pt2,0);
					if(*(resbuf**)pFirstPoint==NULL){
						*(resbuf **)pFirstPoint=pickcur=picktmp;
						pickcur=pickcur->rbnext->rbnext;
					}else{
						pickcur->rbnext=picktmp;
						pickcur=pickcur->rbnext->rbnext->rbnext;
					}
				}
            } else if(strisame(fs1,"WPOLYGON"/*DNT*/)) {
				sds_initget(RSG_NOLIM|SDS_RSG_NODOCCHG,NULL);
                if(RTNORM!=internalGetpoint(NULL,ResourceString(IDC_SDS_SELECTSET__NFIRST_V_28, "\nFirst vertex of inclusive polygon: " ),pt)) break;
                rbb=rbc=sds_buildlist(RT3DPOINT,pt,0);
				ic_ptcpy(pt1,pt);
                SDS_SetCursor(IDC_ICAD_WINDOW);
				SDS_getvar(NULL,DB_QDRAGMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				ptct=1;
				sds_initget(RSG_NOLIM|SDS_RSG_NODOCCHG,NULL);
                while(RTNORM==
					(ptct>1 ? SDS_dragobject(35,rb.resval.rint,pt1,pt,0.0,ResourceString(IDC_SDS_SELECTSET__NNEXT_VE_29, "\nNext vertex: " ),NULL,pt2,NULL)
					: internalGetpoint(pt1,"\nNext vertex: "/*DNT*/,pt2))) {
					++ptct;
                    rbc=rbc->rbnext=sds_buildlist(RT3DPOINT,pt2,0);
                    sds_grdraw(pt1,pt2,-1,0);
                    ic_ptcpy(pt1,pt2);
	                SDS_SetCursor(IDC_ICAD_WINDOW);
					sds_initget(RSG_NOLIM|SDS_RSG_NODOCCHG,NULL);
                }
                SDS_SetCursor(IDC_ICAD_CROSS);
                ic_ptcpy(pt1,rbb->resval.rpoint);
                for(rbc=rbb->rbnext; rbc!=NULL; rbc=rbc->rbnext) {
                    sds_grdraw(pt1,rbc->resval.rpoint,-1,0);
                    ic_ptcpy(pt1,rbc->resval.rpoint);
                }
                SDS_PickObjects("WPOLYGON"/*DNT*/,rbb,NULL,prbFilter,ss2,mode,flp, bIncludeInvisible, false, bIncludeLockedLayers);
				if(szSelMsg && pFirstPoint && mode) {
					//if(*(resbuf**)pFirstPoint) free(*(resbuf**)pFirstPoint);
					//*(resbuf**)pFirstPoint=sds_buildlist(RTSTR,"WP",0);
					//(*(resbuf**)pFirstPoint)->rbnext=rbb;
					picktmp=sds_buildlist(RTSTR,"WPOLYGON"/*DNT*/,0);
					if(*(resbuf**)pFirstPoint==NULL){
						*(resbuf**)pFirstPoint=pickcur=picktmp;
					}else{
						pickcur->rbnext=picktmp;
						pickcur=pickcur->rbnext;
					}
					for(pickcur->rbnext=rbb;pickcur->rbnext!=NULL;pickcur=pickcur->rbnext);
				} else {
					sds_relrb(rbb);
				}
            } else if(strisame(fs1,"CPOLYGON"/*DNT*/)) {
				sds_initget(RSG_NOLIM|SDS_RSG_NODOCCHG,NULL);
                if(RTNORM!=internalGetpoint(NULL,ResourceString(IDC_SDS_SELECTSET__NFIRST_V_31, "\nFirst vertex of crossing polygon: " ),pt)) break;
                rbb=rbc=sds_buildlist(RT3DPOINT,pt,0);
				ic_ptcpy(pt1,pt);
                SDS_SetCursor(IDC_ICAD_CROSSING);
				SDS_getvar(NULL,DB_QDRAGMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				ptct=1;
				//sds_initget(RSG_NOLIM|SDS_RSG_NODOCCHG,NULL);
				sds_initget(RSG_NOLIM|SDS_RSG_NODOCCHG|SDS_RSG_DASH,NULL); // EBATECH(FUTA)
                while(RTNORM==
					(ptct>1 ? SDS_dragobject(35,rb.resval.rint,pt1,pt,0.0,ResourceString(IDC_SDS_SELECTSET__NNEXT_VE_29, "\nNext vertex: " ),NULL,pt2,NULL)
					: internalGetpoint(pt1,ResourceString(IDC_SDS_SELECTSET__NNEXT_VE_29, "\nNext vertex: " ),pt2))) {
					++ptct;
                    rbc=rbc->rbnext=sds_buildlist(RT3DPOINT,pt2,0);
                    //sds_grdraw(pt1,pt2,-1,0);
                    sds_grdraw(pt1,pt2,-1,1); // EBATECH(FUTA)
                    ic_ptcpy(pt1,pt2);
	                SDS_SetCursor(IDC_ICAD_CROSSING);
					//sds_initget(RSG_NOLIM|SDS_RSG_NODOCCHG,NULL);
					sds_initget(RSG_NOLIM|SDS_RSG_NODOCCHG|SDS_RSG_DASH,NULL); // EBATECH(FUTA)
                }
                SDS_SetCursor(IDC_ICAD_CROSS);
                ic_ptcpy(pt1,rbb->resval.rpoint);
                for(rbc=rbb->rbnext; rbc!=NULL; rbc=rbc->rbnext) {
                    //sds_grdraw(pt1,rbc->resval.rpoint,-1,0);
                    sds_grdraw(pt1,rbc->resval.rpoint,-1,1); // EBATECH(FUTA)
                    ic_ptcpy(pt1,rbc->resval.rpoint);
                }
                SDS_PickObjects("CPOLYGON"/*DNT*/,rbb,NULL,prbFilter,ss2,mode,flp, bIncludeInvisible, false, bIncludeLockedLayers);
				if(szSelMsg && pFirstPoint && mode) {
					//if(*(resbuf**)pFirstPoint) free(*(resbuf**)pFirstPoint);
					//*(resbuf**)pFirstPoint=sds_buildlist(RTSTR,"CP",0);
					//(*(resbuf**)pFirstPoint)->rbnext=rbb;
					picktmp=sds_buildlist(RTSTR,"CPOLYGON"/*DNT*/,0);
					if(*(resbuf**)pFirstPoint==NULL){
						*(resbuf**)pFirstPoint=pickcur=picktmp;
					}else{
						pickcur->rbnext=picktmp;
						pickcur=pickcur->rbnext;
					}
					for(pickcur->rbnext=rbb;pickcur->rbnext!=NULL;pickcur=pickcur->rbnext);
				} else {
					sds_relrb(rbb);
				}
	       } else if(strisame(fs1,"OPOLYGON"/*DNT*/)) {
				sds_initget(RSG_NOLIM|SDS_RSG_NODOCCHG,NULL);
                if(RTNORM!=internalGetpoint(NULL,ResourceString(IDC_SDS_SELECTSET__NFIRST_V_33, "\nFirst vertex of outside polygon: " ),pt)) break;
                rbb=rbc=sds_buildlist(RT3DPOINT,pt,0);
				ic_ptcpy(pt1,pt);
                SDS_SetCursor(IDC_ICAD_OUTSIDE);
				SDS_getvar(NULL,DB_QDRAGMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				ptct=1;
				sds_initget(RSG_NOLIM|SDS_RSG_NODOCCHG,NULL);
                while(RTNORM==
					(ptct>1 ? SDS_dragobject(35,rb.resval.rint,pt1,pt,0.0,ResourceString(IDC_SDS_SELECTSET__NNEXT_VE_29, "\nNext vertex: " ),NULL,pt2,NULL)
					: internalGetpoint(pt1,ResourceString(IDC_SDS_SELECTSET__NNEXT_VE_29, "\nNext vertex: " ),pt2))) {
					++ptct;
                    rbc=rbc->rbnext=sds_buildlist(RT3DPOINT,pt2,0);
                    sds_grdraw(pt1,pt2,-1,0);
                    ic_ptcpy(pt1,pt2);
	                SDS_SetCursor(IDC_ICAD_OUTSIDE);
					sds_initget(RSG_NOLIM|SDS_RSG_NODOCCHG,NULL);
                }
                SDS_SetCursor(IDC_ICAD_CROSS);
                ic_ptcpy(pt1,rbb->resval.rpoint);
                for(rbc=rbb->rbnext; rbc!=NULL; rbc=rbc->rbnext) {
                    sds_grdraw(pt1,rbc->resval.rpoint,-1,0);
                    ic_ptcpy(pt1,rbc->resval.rpoint);
                }
                SDS_PickObjects("OPOLYGON"/*DNT*/,rbb,NULL,prbFilter,ss2,mode,flp, bIncludeInvisible, false, bIncludeLockedLayers);
				if(szSelMsg && pFirstPoint && mode) {
					//if(*(resbuf**)pFirstPoint) free(*(resbuf**)pFirstPoint);
					//*(resbuf**)pFirstPoint=sds_buildlist(RTSTR,"OP",0);
					//(*(resbuf**)pFirstPoint)->rbnext=rbb;
					picktmp=sds_buildlist(RTSTR,"OPOLYGON"/*DNT*/,0);
					if(*(resbuf**)pFirstPoint==NULL){
						*(resbuf**)pFirstPoint=pickcur=picktmp;
					}else{
						pickcur->rbnext=picktmp;
						pickcur=pickcur->rbnext;
					}
					for(pickcur->rbnext=rbb;pickcur->rbnext!=NULL;pickcur=pickcur->rbnext);
				} else {
					sds_relrb(rbb);
				}
	       } else if(strisame(fs1,"WCIRCLE"/*DNT*/)) {
				sds_initget(RSG_NOLIM|SDS_RSG_NODOCCHG,NULL);
                if(RTNORM!=internalGetpoint(NULL,ResourceString(IDC_SDS_SELECTSET__NCENTER__35, "\nCenter of circle: " ),pt)) break;
		        SDS_getvar(NULL,DB_QDRAGMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
                SDS_SetCursor(IDC_ICAD_WINDOW);
				sds_initget(RSG_NOLIM|SDS_RSG_NODOCCHG,NULL);
                if(RTNORM!=SDS_dragobject(1,rb.resval.rint,pt,NULL,0L,ResourceString(IDC_SDS_SELECTSET__NRADIUS__36, "\nRadius of circle: " ),&rbb,pt2,NULL)) break;
				ic_assoc(rbb,40);
				rad=ic_rbassoc->resval.rreal;
				sds_relrb(rbb);
				rbb=sds_buildlist(RT3DPOINT,pt,RTREAL,rad,0);
                SDS_SetCursor(IDC_ICAD_CROSS);
                SDS_PickObjects("WCIRCLE"/*DNT*/,rbb,NULL,prbFilter,ss2,mode,flp, bIncludeInvisible, false, bIncludeLockedLayers);
				if(szSelMsg && pFirstPoint && mode) {
					//if(*(resbuf**)pFirstPoint) free(*(resbuf**)pFirstPoint);
					//*(resbuf**)pFirstPoint=sds_buildlist(RTSTR,"WC",0);
					//(*(resbuf**)pFirstPoint)->rbnext=rbb;
					picktmp=sds_buildlist(RTSTR,"WCIRCLE"/*DNT*/,0);
					if(*(resbuf**)pFirstPoint==NULL){
						*(resbuf**)pFirstPoint=pickcur=picktmp;
					}else{
						pickcur->rbnext=picktmp;
						pickcur=pickcur->rbnext;
					}
					for(pickcur->rbnext=rbb;pickcur->rbnext!=NULL;pickcur=pickcur->rbnext);
				} else {
					sds_relrb(rbb);
				}
	       } else if(strisame(fs1,"OCIRCLE"/*DNT*/)) {
				sds_initget(RSG_NOLIM|SDS_RSG_NODOCCHG,NULL);
                if(RTNORM!=internalGetpoint(NULL,ResourceString(IDC_SDS_SELECTSET__NCENTER__35, "\nCenter of circle: " ),pt)) break;
		        SDS_getvar(NULL,DB_QDRAGMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
                SDS_SetCursor(IDC_ICAD_WINDOW);
				sds_initget(RSG_NOLIM|SDS_RSG_NODOCCHG,NULL);
                if(RTNORM!=SDS_dragobject(1,rb.resval.rint,pt,NULL,0L,ResourceString(IDC_SDS_SELECTSET__NRADIUS__36, "\nRadius of circle: " ),&rbb,pt2,NULL)) break;
				ic_assoc(rbb,40);
				rad=ic_rbassoc->resval.rreal;
				sds_relrb(rbb);
				rbb=sds_buildlist(RT3DPOINT,pt,RTREAL,rad,0);
                SDS_SetCursor(IDC_ICAD_CROSS);
                SDS_PickObjects("OCIRCLE"/*DNT*/,rbb,NULL,prbFilter,ss2,mode,flp, bIncludeInvisible, false, bIncludeLockedLayers);
				if(szSelMsg && pFirstPoint && mode) {
					//if(*(resbuf**)pFirstPoint) free(*(resbuf **)pFirstPoint);
					//*(resbuf **)pFirstPoint=sds_buildlist(RTSTR,"OC",0);
					//(*(resbuf **)pFirstPoint)->rbnext=rbb;
					picktmp=sds_buildlist(RTSTR,"OCIRCLE"/*DNT*/,0);
					if(*(resbuf **)pFirstPoint==NULL){
						*(resbuf **)pFirstPoint=pickcur=picktmp;
					}else{
						pickcur->rbnext=picktmp;
						pickcur=pickcur->rbnext;
					}
					for(pickcur->rbnext=rbb;pickcur->rbnext!=NULL;pickcur=pickcur->rbnext);
				} else {
					sds_relrb(rbb);
				}
	       } else if(strisame(fs1,"CCIRCLE"/*DNT*/)) {
				sds_initget(RSG_NOLIM|SDS_RSG_NODOCCHG,NULL);
                if(RTNORM!=internalGetpoint(NULL,ResourceString(IDC_SDS_SELECTSET__NCENTER__35, "\nCenter of circle: " ),pt)) break;
		        SDS_getvar(NULL,DB_QDRAGMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
                SDS_SetCursor(IDC_ICAD_CROSSING);
				//sds_initget(RSG_NOLIM|SDS_RSG_NODOCCHG,NULL);
				sds_initget(RSG_NOLIM|SDS_RSG_NODOCCHG|SDS_RSG_DASH,NULL); // EBATECH(FUTA)
                if(RTNORM!=SDS_dragobject(1,rb.resval.rint,pt,NULL,0L,ResourceString(IDC_SDS_SELECTSET__NRADIUS__36, "\nRadius of circle: " ),&rbb,pt2,NULL)) break;
				ic_assoc(rbb,40);
				rad=ic_rbassoc->resval.rreal;
				sds_relrb(rbb);
				rbb=sds_buildlist(RT3DPOINT,pt,RTREAL,rad,0);
                SDS_SetCursor(IDC_ICAD_CROSS);
                SDS_PickObjects("CCIRCLE"/*DNT*/,rbb,NULL,prbFilter,ss2,mode,flp, bIncludeInvisible, false, bIncludeLockedLayers);
				if(szSelMsg && pFirstPoint && mode) {
					//if(*(resbuf **)pFirstPoint) free(*(resbuf **)pFirstPoint);
					//*(resbuf **)pFirstPoint=sds_buildlist(RTSTR,"CC",0);
					//(*(resbuf **)pFirstPoint)->rbnext=rbb;
					picktmp=sds_buildlist(RTSTR,"CCIRCLE"/*DNT*/,0);
					if(*(resbuf **)pFirstPoint==NULL){
						*(resbuf **)pFirstPoint=pickcur=picktmp;
					}else{
						pickcur->rbnext=picktmp;
						pickcur=pickcur->rbnext;
					}
					for(pickcur->rbnext=rbb;pickcur->rbnext!=NULL;pickcur=pickcur->rbnext);

				} else {
					sds_relrb(rbb);
				}
	       } else if(strisame(fs1,"POINT"/*DNT*/)) {
				sds_initget(RSG_NOLIM|SDS_RSG_NODOCCHG,NULL);
                if(RTNORM!=internalGetpoint(NULL,ResourceString(IDC_SDS_SELECTSET__NPOINT_T_40, "\nPoint to locate entity around: " ),pt)) break;
                SDS_PickObjects("POINT"/*DNT*/,pt,NULL,prbFilter,ss2,mode,flp, bIncludeInvisible, false, bIncludeLockedLayers);
				if(szSelMsg && pFirstPoint && mode) {
					//if(*(resbuf **)pFirstPoint) free(*(resbuf **)pFirstPoint);
					//*(resbuf **)pFirstPoint=sds_buildlist(RTSTR,"PO",RT3DPOINT,pt,0);
					picktmp=sds_buildlist(RTSTR,"POINT"/*DNT*/,RT3DPOINT,pt,0);
					if(*(resbuf **)pFirstPoint==NULL){
						*(resbuf **)pFirstPoint=pickcur=picktmp;
						pickcur=pickcur->rbnext;
					}else{
						pickcur->rbnext=picktmp;
						pickcur=pickcur->rbnext->rbnext;
					}
				}
            } else if(strisame(fs1,"FENCE"/*DNT*/)) {    // TODO - can I change this to "Crossing line" or something?
				sds_initget(RSG_NOLIM|SDS_RSG_NODOCCHG,NULL);
                if(RTNORM!=internalGetpoint(NULL,ResourceString(IDC_SDS_SELECTSET__NFIRST_P_42, "\nFirst point of fence: " ),pt1)) break;
                rbb=rbc=sds_buildlist(RT3DPOINT,pt1,0);
                SDS_SetCursor(IDC_ICAD_CROSSING);
				//sds_initget(RSG_NOLIM|SDS_RSG_NODOCCHG,NULL);
				sds_initget(RSG_NOLIM|SDS_RSG_NODOCCHG|SDS_RSG_DASH,NULL); // EBATECH(FUTA)
                while(RTNORM==internalGetpoint(pt1,ResourceString(IDC_SDS_SELECTSET__NNEXT_PO_43, "\nNext point: " ),pt2)) {
                    rbc=rbc->rbnext=sds_buildlist(RT3DPOINT,pt2,0);
                    //sds_grdraw(pt1,pt2,-1,0);
                    sds_grdraw(pt1,pt2,-1,1); // EBATECH(FUTA)
                    ic_ptcpy(pt1,pt2);
	                SDS_SetCursor(IDC_ICAD_CROSSING);
					//sds_initget(RSG_NOLIM|SDS_RSG_NODOCCHG,NULL);
					sds_initget(RSG_NOLIM|SDS_RSG_NODOCCHG|SDS_RSG_DASH,NULL); // EBATECH(FUTA)
                }
                SDS_SetCursor(IDC_ICAD_CROSS);
                ic_ptcpy(pt1,rbb->resval.rpoint);
                for(rbc=rbb->rbnext; rbc!=NULL; rbc=rbc->rbnext) {
                    //sds_grdraw(pt1,rbc->resval.rpoint,-1,0);
                    sds_grdraw(pt1,rbc->resval.rpoint,-1,1); // EBATECH(FUTA)
                    ic_ptcpy(pt1,rbc->resval.rpoint);
                }
                SDS_PickObjects("FENCE"/*DNT*/,rbb,NULL,prbFilter,ss2,mode,flp, bIncludeInvisible, false, bIncludeLockedLayers);
				if(szSelMsg && pFirstPoint && mode) {
					//if(*(resbuf **)pFirstPoint) free(*(resbuf **)pFirstPoint);
					//*(resbuf **)pFirstPoint=sds_buildlist(RTSTR,"F",0);
					//(*(resbuf **)pFirstPoint)->rbnext=rbb;
					picktmp=sds_buildlist(RTSTR,"FENCE"/*DNT*/,0);
					if(*(resbuf **)pFirstPoint==NULL){
						*(resbuf **)pFirstPoint=pickcur=picktmp;
					}else{
						pickcur->rbnext=picktmp;
						pickcur=pickcur->rbnext;
					}
					for(pickcur->rbnext=rbb;pickcur->rbnext!=NULL;pickcur=pickcur->rbnext);
				} else {
					sds_relrb(rbb);
				}
            } else if(strisame(fs1,"BOX"/*DNT*/)) {
				sds_initget(RSG_NOLIM|SDS_RSG_NODOCCHG,NULL);
                if((ret=(internalGetpoint(NULL,ResourceString(IDC_SDS_SELECTSET__NFIRST_C_45, "\nFirst corner of selection box: " ),pt1))) != RTNORM) goto out;
                boxmode: ;
				SDS_SetCursor(IDC_ICAD_WINDOW);
				extern int SDS_BoxModeCursor;
				SDS_BoxModeCursor=1;
				sds_initget(RSG_NOLIM|SDS_RSG_NODOCCHG|SDS_RSG_NOVIEWCHG,NULL);
				ret=sds_getcorner(pt1,ResourceString(IDC_SDS_SELECTSET__NOPPOSIT_22, "\nOpposite corner: " ),pt2);
				SDS_BoxModeCursor=0;
                SDS_SetCursor(IDC_ICAD_CROSS);
				if(ret!=RTNORM) goto out;
				gr_ucs2rp(pt1,tpt1,SDS_CURGRVW);
				gr_ucs2rp(pt2,tpt2,SDS_CURGRVW);
                SDS_PickObjects((tpt2[0]>tpt1[0]) ? "WINDOW"/*DNT*/ : "CROSSING"/*DNT*/,pt1,pt2,prbFilter,ss2,mode,flp, bIncludeInvisible, false, bIncludeLockedLayers);
				if(szSelMsg && pFirstPoint && mode) {
					//if(*(resbuf **)pFirstPoint) free(*(resbuf **)pFirstPoint);
					//*(resbuf **)pFirstPoint=sds_buildlist(RTSTR,"B",RT3DPOINT,pt1,RT3DPOINT,pt2,0);
					picktmp=sds_buildlist(RTSTR, "BOX"/*DNT*/,RT3DPOINT,pt1,RT3DPOINT,pt2,0);
					if(*(resbuf **)pFirstPoint==NULL){
						*(resbuf **)pFirstPoint=pickcur=picktmp;
						pickcur=pickcur->rbnext->rbnext;
					}else{
						pickcur->rbnext=picktmp;
						pickcur=pickcur->rbnext->rbnext->rbnext;
					}
				}
            } else if(strisame(fs1,"AUTO"/*DNT*/)) {
                automode=1; continue;
            } else if(strisame(fs1,"ALL"/*DNT*/)) {
                SDS_PickObjects("ALL"/*DNT*/,NULL,NULL,prbFilter,ss2,mode,flp, bIncludeInvisible, false, bIncludeLockedLayers);
				if(szSelMsg && pFirstPoint && mode) {
					//if(*(resbuf **)pFirstPoint) free(*(resbuf **)pFirstPoint);
					//*(resbuf **)pFirstPoint=sds_buildlist(RTSTR,"A",0);
					picktmp=sds_buildlist(RTSTR,"ALL"/*DNT*/,0);
					if(*(resbuf **)pFirstPoint==NULL){
						*(resbuf **)pFirstPoint=pickcur=picktmp;
					}else{
						pickcur->rbnext=picktmp;
						pickcur=pickcur->rbnext;
					}
				}
            } else if(strisame(fs1,"LAST"/*DNT*/)) {

				if(SDS_CURDWG->entlast((tilemode==0 && cvport==1) ? 1 : 0)==NULL) continue;
				ename[1]=(long)SDS_CURDWG;
				ename[0]=(long)SDS_CURDWG->entlast((tilemode==0 && cvport==1) ? 1 : 0);

				db_handitem *layhip=((db_handitem *)ename[0])->ret_layerhip();
				if(layhip)
					{
					int flags;
					if(layhip->get_70(&flags))
						{
						if (flags&IC_LAYER_FROZEN)
							{
							continue;
							}
						if ((flags&IC_LAYER_LOCKED) &&
							!bIncludeLockedLayers)
							{
							continue;
							}
						}
					}

                if(RTNORM!=sds_ssadd(ename,NULL,ss2)) continue;
				if(szSelMsg && pFirstPoint && mode) {
					//if(*(resbuf **)pFirstPoint) free(*(resbuf **)pFirstPoint);
					//*(resbuf **)pFirstPoint=sds_buildlist(RTSTR,"L",0);
					picktmp=sds_buildlist(RTSTR,"LAST"/*DNT*/,0);
					if(*(resbuf **)pFirstPoint==NULL){
						*(resbuf **)pFirstPoint=pickcur=picktmp;
					}else{
						pickcur->rbnext=picktmp;
						pickcur=pickcur->rbnext;
					}
				}
            } else if(strisame(fs1,"PREVIOUS"/*DNT*/)) {
                SDS_PickObjects("PREVIOUS"/*DNT*/,NULL,NULL,prbFilter,ss2,mode,flp, bIncludeInvisible, false, bIncludeLockedLayers);
            } else if(strisame(fs1,"+"/*DNT*/) || strisame(fs1,"ADD"/*DNT*/)) {
                mode=1; continue;
            } else if(strisame(fs1,"SINGLE"/*DNT*/)) {
                singlemode=1; automode=0; continue;
            } else if(strisame(fs1,"-"/*DNT*/) || strisame(fs1,"REMOVE"/*DNT*/)) {
                mode=0; continue;
            } else if(strisame(fs1,"UNDO"/*DNT*/)) {
                if(!UndoSS[0] && !UndoSS[1]) {
					cmd_ErrorPrompt(CMD_ERR_NOUNDO,0);
					continue;
				}
                sslen=fl1=0L;
				sds_sslength(ss,&sslen);
                while(RTNORM==sds_ssname(UndoSS,fl1,ename)) {
                    if(hlite) {
                        if(!UndoMode)
							{
							SDS_RedrawEntity(ename,IC_REDRAW_HILITE,NULL,NULL,0);
							}
                        else
							{
							SDS_RedrawEntity(ename,IC_REDRAW_UNHILITE,NULL,NULL,0);
							}
                    }
                    if(!UndoMode) SDS_ssadd(ename, ss, ss);
                    else          sds_ssdel(ename,ss);
                    ++fl1;
					if((fl1%500)==0) SDS_BitBlt2Scr(1);
                }
				SDS_BitBlt2Scr(1);
                sds_ssfree(UndoSS);
                UndoSS[0]=UndoSS[1]=0L;
                continue;
			} else if(strisame(fs1,"AREA"/*DNT*/)) {
				cmd_ErrorPrompt(CMD_ERR_OPTIONNOTSUP,0); goto out;
			} else if(strisame(fs1,"COLOR"/*DNT*/)) {
				fi1=256;
				sds_initget(SDS_RSG_NODOCCHG|SDS_RSG_OTHER,ResourceString(IDC_SDS_SELECTSET_INITGET_B_54, "BYLAYER|BYLayer BYBLOCK|BYBlock ~ Red Yellow Green Cyan Blue Magenta White ~_BYLayer ~_BYBlock ~_ ~_Red ~_Yellow ~_Green ~_Cyan ~_Blue ~_Magenta ~_White" ));
                do {
                    ok=1;
					if ((ret=sds_getkword(ResourceString(IDC_SDS_SELECTSET__NCOLOR_T_55, "\nColor to select <BYLAYER>: " ),fs1))==RTCAN) {
						goto out;
					}else if (ret==RTNONE) {
					    strcpy(fs1,"BYLAYER"/*DNT*/);
					}
					if (!ic_isvalidcol(fs1)) {
							cmd_ErrorPrompt(CMD_ERR_COLOR0256,0);
							ok=0;
					} else fi1=ic_colornum(fs1);
                } while (!ok);

				rbb=sds_buildlist(62,fi1,0);
                SDS_PickObjects("ALL"/*DNT*/,NULL,NULL,rbb,ss2,mode,flp, bIncludeInvisible, false, bIncludeLockedLayers);
				sds_relrb(rbb);
			} else if(strisame(fs1,"EED"/*DNT*/)){
				cmd_ErrorPrompt(CMD_ERR_OPTIONNOTSUP,0); goto out;
			} else if(strisame(fs1,"LAYER"/*DNT*/)) {
				sds_initget(SDS_RSG_OTHER|SDS_RSG_NODOCCHG,NULL);
				ret=sds_getkword(ResourceString(IDC_SDS_SELECTSET__NLAYER_N_58, "\nLayer name(s) to select <*>: " ),pmt);
				if(ret!=RTNORM && ret!=RTKWORD && ret!=RTNONE) goto out;
                if(pmt[0]==0 || ret==RTNONE) rbb=NULL; else rbb=sds_buildlist(8,pmt,0);
                SDS_PickObjects("ALL"/*DNT*/,NULL,NULL,rbb,ss2,mode,flp, bIncludeInvisible, false, bIncludeLockedLayers);
				sds_relrb(rbb);
			} else if(strisame(fs1,"LENGTH"/*DNT*/)){
                sds_printf(ResourceString(IDC_SDS_SELECTSET__NNOT_DON_60, "\nNot done with %s yet" ),fs1); goto out;
			} else if(strisame(fs1,"LTYPE"/*DNT*/)){
				sds_initget(SDS_RSG_OTHER|SDS_RSG_NODOCCHG,NULL);
				ret=sds_getkword(ResourceString(IDC_SDS_SELECTSET__NLINETYP_62, "\nLinetype(s) to select <*>: " ),pmt);
				if(ret!=RTNORM && ret!=RTKWORD && ret!=RTNONE) goto out;
                if(pmt[0]==0 || ret==RTNONE) rbb=NULL; else rbb=sds_buildlist(6,pmt,0);
                SDS_PickObjects("ALL"/*DNT*/,NULL,NULL,rbb,ss2,mode,flp, bIncludeInvisible, false, bIncludeLockedLayers);
				sds_relrb(rbb);
			} else if(strisame(fs1,"NAME"/*DNT*/)){
				sds_initget(SDS_RSG_OTHER|SDS_RSG_NODOCCHG,NULL);
				ret=sds_getkword(ResourceString(IDC_SDS_SELECTSET__NENTITY__64, "\nEntity name(s) to select <*>: " ),pmt);
				if(ret!=RTNORM && ret!=RTKWORD && ret!=RTNONE) goto out;
                if(pmt[0]==0 || ret==RTNONE) rbb=NULL; else rbb=sds_buildlist(RTDXF0,"INSERT"/*DNT*/,2,pmt,0);
                SDS_PickObjects("ALL"/*DNT*/,NULL,NULL,rbb,ss2,mode,flp, bIncludeInvisible, false, bIncludeLockedLayers);
				sds_relrb(rbb);
			} else if(strisame(fs1,"THICKNESS"/*DNT*/)){
				fr1=0.0;
				sds_initget(SDS_RSG_NODOCCHG,NULL);
				ret=sds_getreal(ResourceString(IDC_SDS_SELECTSET__NTHICKNE_66, "\nThickness to select <0.0>: " ),&fr1);
				if(ret==RTCAN) goto out;
                rbb=sds_buildlist(39,fr1,0);
                SDS_PickObjects("ALL"/*DNT*/,NULL,NULL,rbb,ss2,mode,flp, bIncludeInvisible, false, bIncludeLockedLayers);
				sds_relrb(rbb);
			} else if(strisame(fs1,"TYPE"/*DNT*/)){		// Needs initget with types in a prompt box.
				LOAD_COMMAND_OPTIONS_4(IDC_SDS_SELECTSET_INITGET_P_68)
				sds_initget(SDS_RSG_OTHER|SDS_RSG_NODOCCHG, LOADEDSTRING);
				ret=sds_getkword(ResourceString(IDC_SDS_SELECTSET__NENTITY__69, "\nEntity type(s) to select <*>: " ),pmt);
				if(ret!=RTNORM && ret!=RTKWORD && ret!=RTNONE) goto out;
                if((strisame(pmt,"ALL"/*DNT*/))||(strisame(pmt,"*"/*DNT*/))||(SDS_RTNONE==ret))
					rbb=NULL;
				else rbb=sds_buildlist(RTDXF0,pmt,0);
                SDS_PickObjects("ALL"/*DNT*/,NULL,NULL,rbb,ss2,mode,flp, bIncludeInvisible, false, bIncludeLockedLayers);
				sds_relrb(rbb);
			} else if(strisame(fs1,"VALUE"/*DNT*/)){
				sds_initget(SDS_RSG_OTHER|SDS_RSG_NODOCCHG,NULL);
				ret=sds_getkword(ResourceString(IDC_SDS_SELECTSET__NVALUES__71, "\nValues(s) to select <*>: " ),pmt);
				if(ret!=RTNORM && ret!=RTKWORD) goto out;
                if(pmt[0]==0) rbb=NULL; else rbb=sds_buildlist(RTDXF0,"TEXT,MTEXT,ATTRIB,ATTDEF"/*DNT*/,1,pmt,0);
                SDS_PickObjects("ALL"/*DNT*/,NULL,NULL,rbb,ss2,mode,flp, bIncludeInvisible, false, bIncludeLockedLayers);
				sds_relrb(rbb);
			} else if(strisame(fs1,"WIDTH"/*DNT*/)){
				fr1=0.0;
				sds_initget(SDS_RSG_NODOCCHG,NULL);
				ret=sds_getreal(ResourceString(IDC_SDS_SELECTSET__NWIDTH_T_73, "\nWidth to select <0.0>: " ),&fr1);
				if(ret==RTCAN) goto out;
                rbb=sds_buildlist(RTDXF0,db_hitype2str(DB_POLYLINE),40,fr1,0);
                SDS_PickObjects("ALL"/*DNT*/,NULL,NULL,rbb,ss2,mode,flp, bIncludeInvisible, false, bIncludeLockedLayers);
				sds_relrb(rbb);
            } else {
			    if(szSelMsg && szSelMethod) {
					ret=RTKWORD;
					break;
				}
                cmd_ErrorPrompt(CMD_ERR_KWORD,0);
                continue;
            }

            sslen=fl1=0L;
			sds_sslength(ss,&sslen);
            while(RTNORM==sds_ssname(ss2,fl1,ename)) {
                env=1;
				if(hlite) {
					// Modified PK 28/06/2000
					//if(mode && (!pickadd || (pickadd && !SDS_CMainWindow->m_fIsShiftDown)))
					if(mode && (!pickadd || (pickadd && (!SDS_CMainWindow->m_fIsShiftDown || frompipline)))) // Reason : Fix for bug no. 48143
						{
						SDS_RedrawEntity(ename,IC_REDRAW_HILITE,NULL,NULL,0);
						}
					else
						{
						SDS_RedrawEntity(ename,IC_REDRAW_UNHILITE,NULL,NULL,0);
						}
				}
				// Modified PK 28/06/2000
				//if(mode && (!pickadd || (pickadd && !SDS_CMainWindow->m_fIsShiftDown))) {
				if(mode && (!pickadd || (pickadd && (!SDS_CMainWindow->m_fIsShiftDown || frompipline)))) { // Reason : Fix for bug no. 48143
					SDS_ssadd(ename, ss, ss);
					UndoMode=1;
				} else {
					sds_ssdel(ename,ss);
					UndoMode=0;
				}
                ++fl1;
				if((fl1%500)==0) SDS_BitBlt2Scr(1);
            }
			SDS_BitBlt2Scr(1);
			sds_ssfree(UndoSS);
            SDS_sscpy(UndoSS,ss2);

            sds_ssfree(ss2);

			if(singlemode) break;
        } // if KWORD
    } // for loop

    out: ;

	if(ret==RTCAN) {
		SDS_getvar(NULL,DB_QHIGHLIGHT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		if(rb.resval.rint) {
			//*** Unhighlight selection set of entities.
			for(long ssct=0L; sds_ssname(ss,ssct,ename)==RTNORM; ssct++) sds_redraw(ename,IC_REDRAW_UNHILITE);
		}
	} else if(!OmitFromPrevSS){
		if(SDS_CURDOC->m_pPrevSelection[0] || SDS_CURDOC->m_pPrevSelection[1])
			sds_ssfree(SDS_CURDOC->m_pPrevSelection);
		SDS_sscpy(SDS_CURDOC->m_pPrevSelection,ss);
	}
	if(OSnapMode>0){
		rb.restype=RTSHORT;
		rb.rbnext=NULL;
		rb.resval.rint=OSnapMode;
		if(ret==RTNORM)
			ret=SDS_setvar(NULL,DB_QOSMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
		else
			SDS_setvar(NULL,DB_QOSMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);//don't overwrite any error codes
		SDS_UpdateSetvar=(char *)"OSMODE"/*DNT*/;
		ExecuteUIAction( ICAD_WNDACTION_UDCHKSTAT );
		ExecuteUIAction( ICAD_WNDACTION_UDTBSTATE );
		ExecuteUIAction( ICAD_WNDACTION_STATCHANGE );
	}

	if( SaveMenuQueue.IsNotEmpty() )
		{
		GetMenuQueue()->Copy( &SaveMenuQueue );
		}
    if(ret==RTERROR && RTNORM==sds_sslength(ss,&fl1) && fl1>0L) ret=RTNORM;
    if(ret==RTNORM) SDS_sscpy(nmNewSet,ss);
    sds_ssfree(ss);
    sds_ssfree(UndoSS);

	/*DG 3.10.2001*/// Entities in the ss must be ordered by handles when bit 1 of SORTENTS is set.
	if(ret == RTNORM && sortentsRb.resval.rint & 1)
		SDS_ssOrder(nmNewSet);

    return ret;
}

static CString
catTwoResourceStrings(CString FirstStr, CString SecondStr)
{
	CString	localPart1, localPart2, RetStr;
	TCHAR	*globalPart1, *globalPart2;

	globalPart1 = _tcsstr(FirstStr, "~_"/*DNT*/);
	globalPart2 = _tcsstr(SecondStr,"~_"/*DNT*/);
    ASSERT(globalPart1 && globalPart2);

	localPart1 = FirstStr;
	localPart1.SetAt(globalPart1 - localPart1 - 1, '\0'/*DNT*/);

	localPart2 = SecondStr;
	localPart2.SetAt(globalPart2 - localPart2, '\0'/*DNT*/);

	RetStr.Format("%s %s %s %s"/*DNT*/, localPart1, localPart2, globalPart1, globalPart2);
	return RetStr;
}
