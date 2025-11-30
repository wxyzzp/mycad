/* LIB\SDS\SDS_DB.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * DWG R/W Functions
 *
 */

//** Includes
#include "Icad.h"/*DNT*/
#include "IcadDoc.h"
#include "sdsthread.h" /*DNT*/
#include "sdsapplication.h"/*DNT*/
#include "sds_engine.h" /*DNT*/
#include "cmds.h"/*DNT*/
#include <icadraster.h>
#include "commandqueue.h" /*DNT*/
#include "appid.h"
#include "vxtabrec.h"
#include "IcadView.h"
#include "TextStyleTableInfo.h"/*DNT*/
#include "ltypetabrec.h"
#include <set>
#include "Objects.h"
#include "BinChange.h" //oops

extern bool SDS_IsFromOsnapToXref;
extern int SDS_AtCmdLine;

bool	  SDS_DontUpdateDisp;
int 	  SDS_EntMakingEnt;

//Modified Cybage SBD 25/02/2002 DD/MM/YYYY
//Reason : RollBack Change for Bug No. 77758 from Bugzilla
//bool		InitializeTextRot = FALSE;

//Modified Cybage SBD 13/11/2001 DD/MM/YYYY
//Reason : Fix for Bug No. 77866 and 77867 from Bugzilla
extern BOOL bdimUpdate;


/*-------------------------------------------------------------------------*//**
Find xref drawing pointer from xref name.

@author	Martin Waardenburg (Bricsnet)
@param	pXrefDrawing	<=	xref drawing pointer
@param	pXrefName		 => xref name
@return 1 for success, 0 for error
*//*--------------------------------------------------------------------------*/
static int	xrefFind
(
db_drawing	*&pXrefDrawing,
char		*pXrefName
)
{
	if (NULL == pXrefName)
		return 0;

	pXrefDrawing = NULL;
	strupr(pXrefName);
	int blockIndex = -1;
	db_handitem *pHandItem = NULL;

	for (pHandItem = SDS_CURDWG->tblnext(DB_BLOCKTABREC, 1); NULL != pHandItem; pHandItem = pHandItem->next)
		{
		blockIndex++;

		if (pHandItem->ret_deleted())
			continue;

		// Check if this block is an xref.
		int fi1;
		pHandItem->get_70(&fi1);

		if (!(fi1 & IC_BLOCK_XREF))
			continue;

		// Check if the name of this block is the same as the xref name.
		char xrefName[IC_ACADBUF];
		pHandItem->get_2(xrefName, sizeof(xrefName) - 1);
		strupr(xrefName);

		if (RTNORM != sds_wcmatch(xrefName, pXrefName))
			continue;

		// Get the drawing pointer.
		if (NULL == (pXrefDrawing = (db_drawing *) pHandItem->ret_xrefdp()))
			continue;
	}

	return NULL != pXrefDrawing ? 1 : 0;
}

/*-------------------------------------------------------------------------*//**
*//*--------------------------------------------------------------------------*/
int			SDS_regapp
(
const char	*szApplication,
db_drawing	*argflp
)
	{
	ASSERT( OnEngineThread() );

  	if( argflp==NULL ||
		szApplication==NULL ||
		*szApplication==0 )
		{
		return(RTERROR);
		}

	if(argflp->findtabrec(DB_APPIDTAB,(char *)szApplication,1))
		{
		return(RTERROR);
		}

	db_handitem *hip = new db_appidtabrec((char *)szApplication,argflp);

	if(hip==NULL)
		{
		return(RTERROR);
		}

	argflp->addhanditem(hip);
	return(RTNORM);

	}

// *****************************************
// This is an SDS External API
//
int
sdsengine_regapp(const char *szApplication)
	{

	ASSERT( OnEngineThread() );

	return(SDS_regapp(szApplication,SDS_CURDWG));
	}

// *****************************************
// This is an SDS External API
//
// 2nd parameter is undocumented???
//
int
sdsengine_regappx(const char *szApplication, int swSaveAsR12)
	{

	return(sdsengine_regapp(szApplication));
	}

// *****************************************
// This is an SDS External API
//
// TODOTHREAD -- this will have to change enormously
//
// DO NOT CALL FROM ENGINE
//
LPTSTR
sds_getappname(void)
	{
	DWORD dwCurThreadID = GetCurrentThreadId();
	for(SDSApplication *pcurApp=SDSApplication::GetEngineApplication(); pcurApp!=NULL; pcurApp=pcurApp->GetNextLink())
		{
		if( pcurApp->GetMainThread()->IsSame( dwCurThreadID ) )
			break;
		}
	if(pcurApp==NULL)
		{
		return((char *)""/*DNT*/);
		}
	// ACK!  Override cast away const  required for API compatibility
	//
	return(char *)(pcurApp->GetAppName());
	}


/****************************************************************************/
/*********************** Table Functions ************************************/
/****************************************************************************/


// *****************************************
// This is an SDS External API
//
struct sds_resbuf *
sdsengine_tblnext(const char *szTable, int swFirst)
	{

	ASSERT( OnEngineThread() );

   	if(SDS_CURDWG==NULL)
		{
		return(NULL);
		}

	int tabidx=db_tabrec2tabfn(db_str2hitype((char*)szTable));

	if( tabidx<0 ||
		tabidx>=DB_NTABS )
		{
		return(NULL);
		}

	db_handitem *hip=SDS_CURDWG->tblnext(tabidx,swFirst);
	//Modified SAU 14/06/00 [
	//Reason:Using (tblnext) to walk through the linetype table should
	//not return the BYBLOCK and BYLAYER linetypes
	if(tabidx==DB_LTYPETAB)
		while( hip!=NULL && (!stricmp("bylayer"/*DNT*/,((db_tablerecord *)hip)->retp_name()) ||
			!stricmp("byblock"/*DNT*/,((db_tablerecord *)hip)->retp_name())))
			hip=SDS_CURDWG->tblnext(tabidx,0);
	//Modified SAU 14/06/00 ]
	// EBATECH(CNBR) 2002/10/30 Skip layout blocks for A2K compatibility.
	if(tabidx==DB_BLOCKTABREC)
		while( hip!=NULL && (!stricmp("*MODEL_SPACE"/*DNT*/,((db_tablerecord *)hip)->retp_name()) ||
			!strnicmp("*PAPER_SPACE"/*DNT*/,((db_tablerecord *)hip)->retp_name(), 12)))
			hip=SDS_CURDWG->tblnext(tabidx,0);
	// EBATECH ]

	// EBATECH(CNBR) 2002/10/30 replace BLOCK_RECORD to BLOCK for A2K compatibility.
	if ( hip && tabidx == DB_BLOCKTABREC)
		{
		hip = ((db_blocktabrec*)hip)->ret_block();
		}

	if( hip==NULL )
		{
		return(NULL);
		}

	return( hip->entgetx(NULL,SDS_CURDWG) );
	}

// *****************************************
// This is an SDS External API
//
struct sds_resbuf *
sdsengine_tblsearch(const char *szTable, const char *szFindThis, int swNextItem)
	{

	ASSERT( OnEngineThread() );

   	if(SDS_CURDWG==NULL)
		{
		return(NULL);
		}

	int tabidx=db_tabrec2tabfn(db_str2hitype((char *)szTable));

	if( tabidx<0 ||
		tabidx>=DB_NTABS )
		{
		return(NULL);
		}

	if( tabidx==DB_VPORTTABREC &&
		strisame(szFindThis,SDS_STARACTIVE) &&
		SDS_CURVIEW )
		{
		return(SDS_CURVIEW->m_pVportTabHip->entgetx(NULL,SDS_CURDWG));
		}

	// EBATECH(CNBR) 2002/10/30 Refuse layout blocks for A2K compatibility.
	if(tabidx==DB_BLOCKTABREC && (!stricmp("*MODEL_SPACE"/*DNT*/, szFindThis) ||
			!strnicmp("*PAPER_SPACE"/*DNT*/, szFindThis, 12)))
		{
		return(NULL);
		}

	db_handitem *hip=SDS_CURDWG->tblsearch(tabidx,(char *)szFindThis,swNextItem);

	// EBATECH(CNBR) 2002/10/30 replace BLOCK_RECORD to BLOCK for A2K compatibility.
	if ( hip && tabidx == DB_BLOCKTABREC )
		{
		hip = ((db_blocktabrec*)hip)->ret_block();
		}

	if(hip==NULL)
		{
		return(NULL);
		}

	return(hip->entgetx(NULL,SDS_CURDWG));

	}

/*-------------------------------------------------------------------------*//**
This is an SDS External API.
*//*--------------------------------------------------------------------------*/
int			sdsengine_tblobjname
(
const char	*szTable,
const char	*szEntInTable,
sds_name	nmEntName
)
	{
	ASSERT( OnEngineThread() );

	if( szTable==NULL ||
		*szTable==0 ||
		szEntInTable==NULL ||
		*szEntInTable==0 )
		{
		return(RTERROR);
		}

	int tabidx=db_str2hitype((char *)szTable);

	if(!db_is_tabrectype(tabidx))
		{
		return(RTERROR);
		}

	db_drawing *pDrawing = NULL;
	char *pName = (char *) szEntInTable;

	if (DB_BLOCKTABREC == tabidx)
	{
		// Take care of getting blocks in xrefs. The convention is <xref_name>|<block_name>.
		char xrefName[IC_ACADBUF];
		strcpy(xrefName, szEntInTable);
		char *pDivider = strchr(xrefName, CMD_XREF_TBLDIVIDER);

		if (NULL != pDivider)
		{
			*pDivider = '\0';
			xrefFind(pDrawing, xrefName);
			pName = pDivider + 1;
		}
	}

	if (NULL == pDrawing)
		pDrawing = SDS_CURDWG;

	/*DG - 1.7.2003*/// tblobjname must not affect table pointers for tblnext
#if 1
	db_handitem *hip = pDrawing->tblsearch(tabidx, pName, 0);
#else
	db_handitem *hip = pDrawing->tblsearch(tabidx, pName, 1);
#endif

	if (hip == NULL)
		return(RTERROR);

	if (tabidx == DB_BLOCKTABREC)
		{
		db_block* block = ((db_blocktabrec*)hip)->ret_block();

		if(block==NULL)
			{
			return(RTERROR);
			}
		nmEntName[0]=(long)block;
		}
	else
		nmEntName[0]=(long)hip;

	nmEntName[1]=(long) pDrawing;

	return(RTNORM);
	}


/****************************************************************************/
/*********************** Entity Functions ***********************************/
/****************************************************************************/


bool get_ent_entdel(int& rc, sds_name ename, const sds_name nmEntity)
{
	if(nmEntity==NULL || nmEntity[0]==0L || nmEntity[1]==0L)
	{
		rc = RTERROR;
		return false;
	}

	ic_encpy(ename,nmEntity);
	db_drawing *pDrawing = (db_drawing *)nmEntity[1];
	db_handitem *pHanditem = (db_handitem *)nmEntity[0];
	ASSERT( CHECKSTRUCTPTR( pDrawing ) );
	ASSERT( CHECKSTRUCTPTR( pHanditem ) );

	// You can't delete the 1 (paperspace) viewport
	//
	if(pHanditem->ret_type()==DB_VIEWPORT)
		{
		int iVportNumber;
		pHanditem->get_69( &iVportNumber );
		if ( iVportNumber < 2 )
			{
			// Don't think there is a Viewport 0
			//
			ASSERT( iVportNumber == 1 );
			// Don't think we should error, but we definitely shouldn't delete anything
			//
			rc = RTNORM;
			return false;
			}
		}


	if( pHanditem->ret_type() == DB_BLOCK )
	{
		db_handitem *pTmpHip = ((db_block*)pHanditem)->ret_bthip();
		if( !pTmpHip )
			{
			rc = RTERROR;
			return false;
			}
		pHanditem = pTmpHip;
		ename[0] = (long)pHanditem;
	}
	return true;
}

struct CEntData
{
	CEntData(): m_saved(NULL){}
	~CEntData() { delete m_saved; }
	db_handitem* m_saved;
	std::vector<db_handitem*> m_initiators;
	void add(db_handitem* initiator)
	{
		for(int i = 0; i < m_initiators.size() && m_initiators[i] != initiator; ++i){}
		if(i == m_initiators.size())
			m_initiators.push_back(initiator);
	}
};

class CEntdelData: public std::map<db_handitem*, CEntData*>
{
public:
	CEntdelData(){}
	~CEntdelData()
	{
		for(iterator it = begin(); it != end(); ++it)
			delete it->second;
	}
	void add(db_handitem* ent, db_handitem* initiator)
	{
		iterator it = find(ent);
		if(it == end())
		{
			CEntData* data = new CEntData;
			if(data->m_saved = ent->newcopy())
			{
				(*this)[ent] = data;
				data->add(initiator);
			}
			else
			{
				ASSERT(0);
				delete data;
			}
		}
		else
		{
			it->second->add(initiator);
		}
	}
};

void notify_entdel(const std::set<db_handitem*>& sEnts, db_drawing *pDrawing, CEntdelData& savedEnts)
{
	for(std::set<db_handitem*>::const_iterator it = sEnts.begin(); it != sEnts.end(); ++it)
	{
		db_handitem *pHanditem = (db_handitem *)(*it);
		if(pHanditem->ret_deleted())
			continue;
		if(pHanditem->notifyDeleted(pHanditem))
			savedEnts.add(pHanditem, pHanditem);
		if(pHanditem->getReactor() && pHanditem->getReactor()->GetCountNotDeleted(pDrawing))
		{
			pHanditem->getReactor()->ResetIterator();
			db_handitem *observer;
			while (pHanditem->getReactor()->GetNextHanditem(pDrawing, &observer))
			{
				if((!observer->ret_deleted()) && observer->notifyDeleted(pHanditem))
					savedEnts.add(observer, pHanditem);
			}
		}
	}
}


static void increaseProgress(int curprogress, int curidx, int maxidx)
{
	int fi1=(int)(((double)curidx/maxidx)*100);
	if(curprogress+5<=fi1) {
		curprogress=fi1;
		sds_progresspercent(curprogress);
	}
}

int deleteEnts(std::vector<db_handitem*>& aEnts, db_drawing *pDrawing, bool bCmdErase)
{
	std::set<db_handitem*> sEnts;
	for(int i = 0; i < aEnts.size(); ++i)
	{
		sds_name ename;
		sds_name nmEntity;
		nmEntity[0] = (long)aEnts[i];
		nmEntity[1] = (long)pDrawing;
		int rc;
		if(!get_ent_entdel(rc, ename, nmEntity))
			continue;

		db_handitem *pHanditem = (db_handitem *)ename[0];
		/*DG - 13.11.2002*/// If we are undeleting an entity then apps will not be able to entget it, so mark the entity temporary as undeleted.
		// [btw, the same hacks are implemented in icadundoredo.cpp, see comments before some SDS_CallUserCallbackFunc(SDS_CBENTUNDO calls].
		// Old code commented out below.
		int	delflag = pHanditem->ret_deleted();
		pHanditem->set_deleted(0);
		int	cbret = SDS_CallUserCallbackFunc(SDS_CBENTDEL, (void*)ename, (void*)delflag, NULL);
		pHanditem->set_deleted(delflag);
		
		//Autodsys 062304 {{
		if(sds_ssmemb(ename, SDS_CURDOC->m_pGripSelection) == RTNORM)	/*D.G.*/// Delete it from
		{																	// the GripSelection too.
			sds_ssdel(ename, SDS_CURDOC->m_pGripSelection);
		}
		//Autodsys 062304 }}

		if(cbret == RTERROR)
			continue;

		sEnts.insert((db_handitem*)ename[0]);
	}
	if(!sEnts.size())
		return RTERROR;

	if(bCmdErase && !SDS_CURDOC)
		bCmdErase = false;

	if(bCmdErase)
		SDS_CURDOC->m_cmdHelper.setOopsAvailable(false);

	struct resbuf rb;
	SDS_getvar(NULL,DB_QUNDOCTL,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	const bool bUndoOn = (rb.resval.rint & IC_UNDOCTL_ON);
	CEntdelData savedEnts;
	if(bUndoOn || bCmdErase)
		notify_entdel(sEnts, pDrawing, savedEnts);

	std::set<db_handitem*>::iterator itNext;
	bool bCancel = false;
	int curidx = 0;
	const int maxidx = sEnts.size();
	int curprogress = 0;
	for(std::set<db_handitem*>::iterator it = sEnts.begin(); it != sEnts.end();
		++curidx, it = itNext, increaseProgress(curprogress, curidx, maxidx))
	{
		if(sds_usrbrk())
		{
			bCancel = true;
			break;
		}
		itNext = it;
		++itNext;
		sds_name ename;
		db_handitem *pHanditem = *it;
		ename[0] = (long)pHanditem;
		ename[1] = (long)pDrawing;
		// If we have multi views of this drawing we need to unpaint all the ents in all the
		// views first, rather than let sds_entupd() do it.	 This is because of our single display list.
		SDS_getvar(NULL,DB_QTILEMODE,&rb,(db_drawing *)ename[1],&SDS_CURCFG,&SDS_CURSES);

		if(rb.resval.rint==0 && !pHanditem->ret_deleted())
			if(!SDS_DontUpdateDisp) sds_redraw(ename,IC_REDRAW_UNDRAW);

		if(SDS_CURDOC && SDS_CURDOC->m_nOpenViews>1 && !pHanditem->ret_deleted())
			if(!SDS_DontUpdateDisp) sds_redraw(ename,IC_REDRAW_UNDRAW);

		if(IC_TYPE_FROM_ENAME(ename)==DB_XLINE || IC_TYPE_FROM_ENAME(ename)==DB_RAY ||
			IC_TYPE_FROM_ENAME(ename)==DB_IMAGE)
			if(!SDS_DontUpdateDisp) sds_redraw(ename, IC_REDRAW_UNDRAW);

		if(pHanditem->ret_type()==DB_IMAGE)
			{
				//	  ** Disclaimer:
				//	  I have put this code here, because the imagedef class is incomplete.
				//	  if/when it is completed, this should be made a member of db_image.
				//	  There is not enough time to implement the db_imagedef correctly and
				//	  make sure it gets the testing it will need. mb

			// Information per Hitachi. What they need is for us, upon deletion
			// of an image is to remove the image, the imagedef_reactor, and the
			// reference to the imagedef_reactor in the imagedef. However, they
			// want us to leave the imagedef in place. This is so their ieImageManager
			// can re-attach, or detach the image even though there is no instantiation
			// of it. So here goes.

			// Need to delete the imagedef_reactor.
			db_handitem *imageDef=NULL, *imageDefReactor=NULL;
			pHanditem->get_340(&imageDef);
			pHanditem->get_360(&imageDefReactor);
			ASSERT(imageDef != NULL);
			ASSERT(imageDefReactor != NULL);

			// Now remove the imagedef_reactor from the imagedef. This is the part
			// I refered to above about db_imagedef not being complete. Other than
			// walking it's resbuf list, there is no way to get at the list of
			// reactors that may or may not be on it.
			sds_name imagedefName, imagedefReactorName;
			imagedefName[0] 		= (long)imageDef;
			imagedefReactorName[0]	= (long)imageDefReactor;
			imagedefName[1] 		= imagedefReactorName[1] = (long)ename[1];

			//DP: memory leak, IMAGEDEF was copied for unknown purpose
			//db_handitem *savehip=NULL;
			//SDS_CopyEntLink(&savehip,imageDef);

			sds_resbuf *tmp, *imagedefData = sds_entget(imagedefName);
			sds_resbuf *last;
			tmp = imagedefData;
			while(tmp != NULL)
				{
				if (tmp->restype == 330)
					{
					if (sds_name_equal(imagedefReactorName, tmp->resval.rlname))
					   {
						// Found it, now bridge across, and delete it.
						sds_resbuf *t = tmp;
						last->rbnext = tmp->rbnext;
						t->rbnext = NULL;
						sds_relrb(t);
						break;
						}
					}
				last = tmp;
				tmp = tmp->rbnext;
				}

			imageDef->entmod(imagedefData,pDrawing);
			// Next delhanditem() will remove the db_image.

			// Note there is no equivalent to delhanditem() for database objects as
			// opposed to database entities. db_imagedef and db_imagedef_reactor are
			// both objects, and therefor in the objll instead of the entll. It is
			// not enough to simply mark them as deleted. You must remove them form
			// the linked list
			db_handitem *firstpp=NULL, *lastpp=NULL, *tmpObj=NULL;
			pDrawing->get_objllends(&firstpp, &lastpp);

			tmpObj = firstpp;
			while(tmpObj != NULL)
				{
			  // One thing to note here is that for some reason, db_imagedef_reactors have
			  // a member db_hireflink* for allowing more that one 330 group (db_image) which
			  // can't happen. On the other hand, db_imagedef has no db_hireflink* to allow
			  // multiple 330 (db_imagedef_reactors) which it will have one for each copy of the image.
			  // TODO Reverse this in object.h. This means that I'll have to get a resbuf chain
			  // for the imagedef, walk it, remove this imagedef_reactor and entmod it.
				if(tmpObj->next == imageDefReactor)
					{
					// remove it.
					db_handitem *t = tmpObj->next;
					tmpObj->next = tmpObj->next->next;
					delete(t);
					break;
					}
				else
					tmpObj = tmpObj->next;
				}

			// Reset first objll and last ojbll pointers, and reassign them.
			lastpp = firstpp;
			while(lastpp != NULL && lastpp->next != NULL) lastpp = lastpp->next;
			pDrawing->set_objllends(firstpp, lastpp);
			}

		if(pDrawing->delhanditem(NULL,pHanditem,2))
		{
			sEnts.erase(it);
			continue;
		}

		if(pHanditem->ret_type()==DB_VIEWPORT)
			{

			for(db_handitem *vpe=pDrawing->tblnext(DB_VXTAB,1); vpe!=NULL; vpe=vpe->next)
				{
				db_vxtabrec *pVxTableRecord = (db_vxtabrec *)vpe;

				if(pVxTableRecord->ret_vpehip()==((db_viewport *)pHanditem))
					{
					pDrawing->delhanditem(NULL,vpe,2);
					break;
					}
				}
			}

		// Delete sub ents too.
		bool bRes = true;
		for(db_handitem *hip = pHanditem->next; hip && db_is_subentitytype(hip->ret_type()); hip = hip->next)
		{
			if(pDrawing->delhanditem(NULL,hip,2))
			{
				bRes = false;
				break;
			}
		}

		if(!bRes)
			continue;

		if(IC_TYPE_FROM_ENAME(ename)==DB_OLE2FRAME && NULL!=SDS_CMainWindow)
		{
			//*** Delete the CIcadCntrItem object associated with this entity.
			SDS_CMainWindow->m_pvWndAction=(void*)ename[0];
			ExecuteUIAction( ICAD_WNDACTION_DELOLEITEM );
			SDS_CMainWindow->m_pvWndAction=NULL;
			if(!SDS_CMainWindow->m_bWndAction) continue;
			sds_redraw(NULL,IC_REDRAW_DONTCARE);
		}

		if(!SDS_DontUpdateDisp && !(IC_TYPE_FROM_ENAME(ename)==DB_IMAGE)) {
			sds_entupd(ename);	// Build or delete the display list
		}

	}

	// Set the Undo
	bool bModified = false;
	if(bCmdErase && savedEnts.size() && !bUndoOn)
	{
		rb.resval.rint |= IC_UNDOCTL_ON;
		SDS_setvar(NULL, DB_QUNDOCTL, &rb, pDrawing, &SDS_CURCFG, &SDS_CURSES, 0);
	}
	for(CEntdelData::iterator dit = savedEnts.begin(); dit != savedEnts.end(); ++dit)
	{
		db_BinChange* change = NULL;
		if(db_CompEntLinks(&change, dit->second->m_saved, dit->first) == RTNORM && change)
		{
			if(bUndoOn)
			{
				SDS_SetUndo(IC_UNDO_CHG_ENT_TAB_CLS,(void *)dit->first, (void *)change, (void *)NULL, pDrawing);
				change = NULL;
			}
			bModified = true;
			if(bCmdErase)
			{
				if(!change)
					VERIFY(db_CompEntLinks(&change, dit->second->m_saved, dit->first) == RTNORM);
				SDS_CURDOC->m_cmdHelper.oopsAddModified(dit->first, change, dit->second->m_initiators);
			}
		}
	}
	if(bCmdErase && savedEnts.size() && !bUndoOn)
	{
		rb.resval.rint &= ~IC_UNDOCTL_ON;
		SDS_setvar(NULL, DB_QUNDOCTL, &rb, pDrawing, &SDS_CURCFG, &SDS_CURSES, 0);
	}
	for(; it != sEnts.begin(); )
	{
		--it;
		SDS_SetUndo(IC_UNDO_DEL_ENT_TAB_CLS,(void *)(*it), (void *)NULL, NULL, pDrawing);
		bModified = true;
		if(bCmdErase)
			SDS_CURDOC->m_cmdHelper.oopsAddErased(*it);
	}
	if(bModified)
		SDS_DBModified(IC_DBMOD_ENTITIES_MODIFIED);
	return bCancel ? RTCAN : (bModified ? RTNORM : RTERROR);
}

int sds_delss(const sds_name ss, bool bCmdErase)
{
	SDSAPI_Entry();
	long ssLen;
	sds_name ename;
	sds_sslength(ss, &ssLen);
	std::vector<db_handitem*> aEnts;
	db_drawing *pDrawing = NULL;
	for(long ssct=0L; sds_ssname(ss, ssct, ename) == RTNORM; ssct++)
	{
		pDrawing = (db_drawing *)ename[1];
		aEnts.push_back((db_handitem*)ename[0]);
	}
	return deleteEnts(aEnts, pDrawing, bCmdErase);
}
// *****************************************
// This is an SDS External API
//
int sdsengine_entdel(const sds_name nmEntity)
{
	ASSERT( OnEngineThread() );
	if(nmEntity==NULL || nmEntity[0]==0L || nmEntity[1]==0L)
		return RTERROR;
	db_drawing *pDrawing = (db_drawing *)nmEntity[1];
	db_handitem *pHanditem = (db_handitem *)nmEntity[0];
	ASSERT( CHECKSTRUCTPTR( pDrawing ) );
	ASSERT( CHECKSTRUCTPTR( pHanditem ) );

	std::vector<db_handitem*> aEnts;
	aEnts.push_back(pHanditem);
	return deleteEnts(aEnts, pDrawing, false);
}

// *****************************************
// This is no longer an SDS External API
//
int
sds_entnext_noxref(const sds_name nmKnownEnt, sds_name nmNextEnt)
	{

	ASSERT( OnEngineThread() );

	db_handitem *nextitem;

	if(nmNextEnt==NULL)
		{
		struct resbuf rb;
		rb.restype=RTSHORT;
		rb.resval.rint=OL_ENAMEVALID;
		SDS_setvar(NULL,DB_QERRNO,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
		return(RTERROR);
		}
	if(nmKnownEnt==NULL)
		{
	  	if(SDS_CURDWG==NULL || NULL==SDS_CURDWG->entnext_noxref(NULL))
			{
			struct resbuf rb;
			rb.restype=RTSHORT;
			rb.resval.rint=OL_EEOEF;
			SDS_setvar(NULL,DB_QERRNO,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
			return(RTERROR);
			}
		nmNextEnt[1]=(long)SDS_CURDWG;
		nmNextEnt[0]=(long)SDS_CURDWG->entnext_noxref(NULL);
		}
	else
		{
		if(nmKnownEnt[0]==0L || nmKnownEnt[1]==0L)
			{
			struct resbuf rb;
			rb.restype=RTSHORT;
			rb.resval.rint=OL_ENAMEVALID;
			SDS_setvar(NULL,DB_QERRNO,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
			return(RTERROR);
			}
		nextitem=((db_drawing *)nmKnownEnt[1])->entnext_noxref((db_handitem *)nmKnownEnt[0]);
		if (nextitem==NULL)
			{
			struct resbuf rb;
			rb.restype=RTSHORT;
			rb.resval.rint=OL_EEOEF;
			SDS_setvar(NULL,DB_QERRNO,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
			return(RTERROR);
			}
		nmNextEnt[1]=nmKnownEnt[1];
		nmNextEnt[0]=(long)nextitem;
		}
	return(RTNORM);
	}

// *****************************************
// This is an SDS External API
//
int
sdsengine_entnext_withxref(const sds_name nmKnownEnt, sds_name nmNextEnt)
	{

	ASSERT( OnEngineThread() );

	db_drawing *xrefdp;
	db_handitem *knownenthip,*xrefenthip,*hip;

	if(nmNextEnt==NULL)
		{
		struct resbuf rb;
		rb.restype=RTSHORT;
		rb.resval.rint=OL_ENAMEVALID;
		SDS_setvar(NULL,DB_QERRNO,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
		return(RTERROR);
		}

	if(nmKnownEnt==NULL)
		{
	  	if(SDS_CURDWG==NULL || NULL==SDS_CURDWG->entnext_noxref(NULL))
			{
			struct resbuf rb;
			rb.restype=RTSHORT;
			rb.resval.rint=OL_EEOEF;
			SDS_setvar(NULL,DB_QERRNO,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
			return(RTERROR);
			}
		nmNextEnt[1]=(long)SDS_CURDWG;
		nmNextEnt[0]=(long)SDS_CURDWG->entnext_noxref(NULL);
		return(RTNORM);
	}

	if(nmKnownEnt[0]==0L || nmKnownEnt[1]==0L)
		{
		struct resbuf rb;
		rb.restype=RTSHORT;
		rb.resval.rint=OL_ENAMEVALID;
		SDS_setvar(NULL,DB_QERRNO,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
		return(RTERROR);
		}

	knownenthip=(db_handitem *)nmKnownEnt[0];
	if (knownenthip->ret_type()==DB_BLOCK && (((db_block *)knownenthip)->ret_flags() & IC_BLOCK_XREF)) {
		xrefdp=((db_block *)knownenthip)->ret_xrefdp();
		if (xrefdp!=NULL) {
			xrefenthip=xrefdp->ret_firstent();
			if (xrefenthip==NULL) goto searchmaindwg;
			while (xrefenthip!=NULL && xrefenthip->ret_deleted())
				xrefenthip=xrefenthip->next;
			if (xrefenthip!=NULL) {
				nmNextEnt[0]=(long)xrefenthip;		// return entity in xref
				nmNextEnt[1]=(long)xrefdp;
				return RTNORM;
			}
		}
	}

searchmaindwg:
	for (hip=knownenthip->next; hip!=NULL && hip->ret_deleted(); hip=hip->next);
	if (hip!=NULL && hip->ret_type()==DB_ENDBLK) hip=NULL;
	nmNextEnt[1]=nmKnownEnt[1];
	nmNextEnt[0]=(long)hip;
	if (hip==NULL) return(RTERROR);
	return(RTNORM);
}

// *****************************************
// This is an SDS External API
//
int sdsengine_entlast(sds_name nmLastEnt)
	{

	ASSERT( OnEngineThread() );
   	if(SDS_CURDWG==NULL)
		{
		return(RTERROR);
		}

	if(SDS_CURDWG->entlast(2)==NULL)
		{
		return(RTERROR);
		}

	nmLastEnt[1]=(long)SDS_CURDWG;
	nmLastEnt[0]=(long)SDS_CURDWG->entlast(2);

	return(RTNORM);
	}

// *****************************************
// This is an SDS External API
//
int sdsengine_entmake(const struct sds_resbuf *prbEntList)
	{

	ASSERT( OnEngineThread() );

	if(SDS_CURDWG==NULL)
		{
		return(RTERROR);
		}

	return(SDS_entmake(prbEntList,SDS_CURDWG));
	}

// *****************************************
// This is an SDS External API
//
int	sdsengine_entmakex(const struct sds_resbuf *prbEntList, sds_name nmNewEnt)
	{

	ASSERT( OnEngineThread() );

	if( SDS_CURDWG==NULL ||
		prbEntList==NULL ||
		nmNewEnt==NULL)
		{
		return(RTERROR);
		}

	SDS_LastHandItem=NULL;
	int ret=sdsengine_entmake(prbEntList);

	if( ret!=RTNORM ||
		SDS_LastHandItem==NULL)
		{
		return(RTERROR);
		}

	nmNewEnt[0]=(long)SDS_LastHandItem;
	nmNewEnt[1]=(long)SDS_CURDWG;
	SDS_LastHandItem=NULL;

	return(RTNORM);
	}

int SDS_entmake(const struct sds_resbuf *prbEntList,db_drawing *argflp)
	{

	ASSERT( OnEngineThread() );

	if(argflp==NULL)
		{
		return(RTERROR);
		}

	/////////// This block of code is for Undo.
	int undoarea=-1,ret,makingtype = -1;
	bool stripReactor = false;
	db_drawing *flp=argflp;
	db_handitem *newhip=NULL,*blktabrec=NULL;

	if(prbEntList==NULL)
		{
		flp->entmake(NULL,NULL);
		return(RTNORM);
		}

	int wasmakingblk=flp->blockdefpending();
	int wasmakingcomplex=flp->polylinepending()|flp->complexinsertpending();

	for(struct resbuf *trbp1=(struct resbuf *)prbEntList; trbp1!=NULL; trbp1=trbp1->rbnext) {
		if(trbp1->restype == 0) {
			makingtype=db_str2hitype(trbp1->resval.rstring);
		}
		// Bug fix # 1-8180 (old# 6341)
		if (trbp1->restype == 330) { // possible group reactor. If so set flag to strip it later.
			if (trbp1->resval.rlname[0] != 0 && ((db_handitem*)trbp1->resval.rlname[0])->ret_type()==DB_GROUP)
				stripReactor = true;
			break;
		}
	}

	if(makingtype == -1) return(RTERROR);

	resbuf		rb;
	db_handitem	*ehip, *blkehip = NULL, *blkbhip = NULL;

	/*D.G.*/// Commented this out with their comments:
/*	// Make sure we don't try to mahe ACIS objects this is to fix bug 6252.
	if(makingtype==DB_3DSOLID || makingtype==DB_BODY || makingtype==DB_REGION) return(RTNORM);
*/

	if(makingtype == DB_ENDBLK)
	{
		flp->get_tabrecllends(DB_BLOCKTABREC, NULL, &ehip);
		undoarea = DB_BLOCKTABREC;	// Its in this table.
	}
	else
		if(db_is_tabrectype(makingtype))
		{
			makingtype = db_tabrec2tabfn(makingtype);
			flp->get_tabrecllends(makingtype, NULL, &ehip);
			undoarea = makingtype;	// Its in this table.
		}
		else
		{
			/*DG - 24.1.2002*/// Sould use different llist for objects (i.e. set ehip to the last object).
			if(db_is_objecttype(makingtype))
				flp->get_objllends(NULL, &ehip);
			else
				flp->get_entllends(NULL, &ehip);
			undoarea = -1;	// Its in the drawing.
		}

	// If we are making a viewport we need to make sure it has unique ID.
	// Note: 2002/11/30 Ebatech(CNBR) When VPID exceeds MAXACTVP,
	// It is better to create inactive floating viewport.
	if(makingtype==DB_VIEWPORT) {
		struct resbuf *rbt;
		// Note: Don't set the value of the second parameter to 0 because 
		// VX table can contain the deleted records(marked as deleted) after a viewport
		// creation is UNDOne, Setting the second parameter to 0 will cause those records
		// to be counted. SWH, 30/9/2004
		//
		int vpid=flp->ret_ntabrecs(DB_VXTAB,1)+1;
		SDS_getvar(NULL,DB_QMAXACTVP,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
		if(vpid>rb.resval.rint) return(RTERROR);
		for(rbt=(struct resbuf *)prbEntList;rbt;rbt=rbt->rbnext) {
			if(rbt->restype==69) rbt->resval.rint=vpid;
			if(rbt->restype==68) rbt->resval.rint=vpid;
		}
	}

	if(flp->entmake((struct resbuf *)prbEntList,&newhip) || newhip==NULL) {
		rb.restype=RTSHORT;
		rb.resval.rint=db_ret_lasterror();
		SDS_setvar(NULL,DB_QERRNO,&rb,flp,&SDS_CURCFG,&SDS_CURSES,0);
		return(RTREJ);
	} else ret=RTNORM;

	if(newhip) SDS_LastHandItem=newhip;

	if (newhip &&
		ret==RTNORM &&
		(newhip->ret_type()==DB_TEXT ||
		 newhip->ret_type()==DB_ATTRIB ||
		 newhip->ret_type()==DB_ATTDEF))
		{
			CDC *dc = GetIcadTargetDeviceCDC();
			if (NULL == dc)
				return RTERROR;

			gr_fixtextjust(newhip, flp, NULL, dc, 0);
		}
	if(stripReactor) {
		stripReactor = false;
		// Bug fix # 1-8180 (old# 6341)
		// Here we are stripping out the 102 surrounded 'GROUP' reactor.
		// I've intentionally waited untill we reach this point so the resbuf chain is
		// in perfect shape, with the exception that it still has the bogus 'Group' reactor.
		// Stripping this out will cause AutoCAD to be able to manipulate them as normal entities.
		struct sds_resbuf* entdata = NULL, *walkingPtr = NULL, *firstGroupMarker = NULL, *tmp;
		entdata = newhip->entgetx(NULL, flp);

		// Looking for first 102 reactor sentinal.
		for (walkingPtr = entdata; walkingPtr != NULL, walkingPtr->rbnext->restype != 102; walkingPtr = walkingPtr->rbnext) ; // Do nothing, looking for 'GROUP' reactor.

		// We are now on the resbuf just ahead of the 102 reactor group code.
		firstGroupMarker = walkingPtr;
		tmp = firstGroupMarker->rbnext; // We'll relrb tmp later.

		// Since walkingPtr is currently on the resbuf just befor the 102 sentinal, reset
		// it to the rbnext of the rbnext to skip this beginning sentinal.
		walkingPtr = walkingPtr->rbnext->rbnext;

		// Looking for closing 102 reactor sentinal.
		for(  ; walkingPtr->restype != 102; walkingPtr = walkingPtr->rbnext) ; // Do nothing but walk.

		// hook up the resbuf list skipping the 102 sentinal pair.
		firstGroupMarker->rbnext = walkingPtr->rbnext;

		//sds_entmod(entdata);
		newhip->entmod(entdata, flp);

		// Now reclaim the memory the sentinal was holding.
		walkingPtr->rbnext = NULL;
		sds_relrb(tmp);
		sds_relrb(entdata);	// 2004/6/30 memory leak
	}
	if(ret==RTNORM && prbEntList) {
		if(!flp->complexinsertpending() &&
			!flp->polylinepending() &&
			 !flp->blockdefpending()) {

			SDS_DBModified( IC_DBMOD_ENTITIES_MODIFIED);

			// Special case for blocks.
			if(newhip->ret_type()==DB_ENDBLK) {
				db_handitem *thip;
				flp->get_tabrecllends(DB_BLOCKTABREC,&thip,&newhip);
				thip=flp->ret_lastblockdef();
				((db_blocktabrec *)thip)->get_oldblockllends(&blkbhip,&blkehip);
				((db_blocktabrec *)thip)->set_oldblockllends(NULL,NULL);
			}

			// Set the Undo.
			if(blkehip && blkbhip) { // Check for redefining a block.
				SDS_SetUndo(IC_UNDO_REDEFINE_BLOCK,(void *)flp->ret_lastblockdef(),(void *)blkbhip,(void *)blkehip,flp);
			} else {
				SDS_SetUndo(IC_UNDO_NEW_ENT_TAB_CLS,(void *)ehip,(void *)newhip,(void *)undoarea,flp);
			}

			if(wasmakingblk) {
				// Set up the keyword for entmake of ENDBLK.
				db_handitem *hip=flp->ret_lastblockdef();
				if(hip==NULL) ret=RTERROR;
				else {
					char *fcp1;
					sds_name ename;
					hip->get_2(&fcp1);
					strcpy(SDSApplication::GetActiveApplication()->thekeyword,fcp1);
					ret=RTKWORD;

					// Send callback for created block
					ename[0] = (long)hip;
					ename[1] = (long)flp;
					SDS_CallUserCallbackFunc(SDS_CBENTMAKE,(void *)ename,NULL,NULL);
				}
			} else {
				if(db_is_tabrectype(newhip->ret_type()) || db_is_objecttype(newhip->ret_type())) {
					return(RTNORM);
				}
				db_handitem *elp;
				sds_name ename;
				if(NULL==(elp=flp->entlast(2))) return(RTERROR);

				ename[0]=(long)elp;
				ename[1]=(long)flp;
				if(elp->ret_type()==DB_DIMENSION) {
					cmd_makedimension((db_handitem *)ename[0],(db_drawing *)ename[1],0);
				}
				if(elp->ret_type()==DB_INSERT) {
					db_handitem *lastent,*firstent;
					flp->get_entllends(&lastent,&firstent);
					db_drawing *flp=(db_drawing *)ename[1];
					if(lastent && lastent->ret_type()==DB_ATTRIB)
						ename[0]=(long)lastent;
				}

				SDS_EntMakingEnt=TRUE;
				sds_entupd(ename);
				SDS_EntMakingEnt=FALSE;

				SDS_CallUserCallbackFunc(SDS_CBENTMAKE,(void *)ename,NULL,NULL);
			}
		}
	}
	return(ret);
}

// *****************************************
// This is an SDS External API
//
int sdsengine_entmod(const struct sds_resbuf *prbEntList)
{
	ASSERT(OnEngineThread());

	const struct resbuf *trbp1;
	struct resbuf rb;
	db_handitem *savelp=NULL;
	struct db_BinChange *chglst=NULL;
	int ret, tilemode;

	if (SDS_CURDWG==NULL || SDS_CURVIEW==NULL || SDS_CURDOC==NULL)
		return(RTERROR);

	for	(trbp1=(struct resbuf *)prbEntList;	trbp1!=NULL && trbp1->restype!=-1;)
		trbp1=trbp1->rbnext;

	if (trbp1==NULL)
		return RTERROR;

	db_handitem *elp=(db_handitem *)trbp1->resval.rlname[0];
	// Progesoft
	// rem this is Autocad normal function
	/*	// Modified AP 08/23/2000 [
	int flags; // Reason : Fix for bug no. 76098
	db_handitem *layhip=elp->ret_layerhip();

	  if(layhip != NULL)
	  {
	  if(layhip->get_70(&flags))
	  {
	  if (flags & IC_LAYER_LOCKED)
	  return RTERROR;
	  }
	  }
	// Modified AP 08/23/2000 ]*/
	//end progesoft
	SDS_CopyEntLink(&savelp,elp);

	SDS_getvar(NULL,DB_QTILEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	tilemode=rb.resval.rint;

	if( !SDS_IsFromOsnapToXref ) // do not redraw if this is a temporary entity created while processing insert in osnap methods
		if(tilemode==0 || SDS_CURDOC->m_nOpenViews>1 || elp->ret_type()==DB_RAY || elp->ret_type()==DB_XLINE || elp->ret_type()==DB_VIEWPORT || elp->ret_type()==DB_IMAGE)
		{
			if(elp->ret_type()!=DB_VERTEX && elp->ret_type()!=DB_SEQEND)
				sds_redraw(trbp1->resval.rlname,IC_REDRAW_UNDRAW);
		}

	ret=RTNORM;
	if(elp->entmod((struct resbuf *)prbEntList,SDS_CURDWG))
	{
		rb.restype=RTSHORT;
		rb.resval.rint=db_ret_lasterror();
		SDS_setvar(NULL,DB_QERRNO,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
		ret=RTERROR;
	}

	if(ret!=RTERROR)
	{
		SDS_DBModified( IC_DBMOD_ENTITIES_MODIFIED);

		if (elp->ret_type()==DB_TEXT ||
			elp->ret_type()==DB_ATTRIB ||
			elp->ret_type()==DB_ATTDEF)
		{
			CDC *dc = GetIcadTargetDeviceCDC();
			if (NULL == dc)
				return RTERROR;

			gr_fixtextjust(elp, SDS_CURDWG, NULL, dc, 0);
		}

		if(elp->ret_type()==DB_DIMENSION)
		{
			//Modified Cybage SBD 25/02/2002 DD/MM/YYYY
			//Reason : RollBack Change for Bug No. 77758 from Bugzilla
			//InitializeTextRot = TRUE;
			//Modified Cybage SBD 13/11/2001 DD/MM/YYYY
			//Reason : Fix for Bug No. 77866 and 77867 from Bugzilla
			bdimUpdate=TRUE;

			//Bugzilla No. 77993 ; date : 18-03-2002 [
			int SavedSDS_AtNodeDrag;
			SavedSDS_AtNodeDrag = SDS_AtNodeDrag;
			//Bugzilla No. 77993 ; date : 08-04-2002
			if ( SDS_AtNodeDrag == 0 )
				SDS_AtNodeDrag = 1;
			cmd_makedimension((db_handitem *)trbp1->resval.rlname[0],(db_drawing *)trbp1->resval.rlname[1],0);
			SDS_AtNodeDrag = SavedSDS_AtNodeDrag;
			//Bugzilla No. 77993 ; date : 18-03-2002 ]

			//Modified Cybage SBD 13/11/2001 DD/MM/YYYY
			//Reason : Fix for Bug No. 77866 and 77867 from Bugzilla
			bdimUpdate=FALSE;
		}

		// To reabuild dispobjs of entities belong to changed layer
		if(elp->ret_type()==DB_LAYERTABREC)
			cmd_regenall();

		// Bug fix #6407
		// Here we are looking for the 280 group code to tell us if we need to call
		// CVisioRaster::unloadimage() or not.
		if(elp->ret_type()==DB_IMAGEDEF)
		{
			const struct sds_resbuf* tmp = trbp1;
			sds_name entname;
			entname[0] = tmp->resval.rstring[0];
			entname[1] = tmp->resval.rstring[1];

			char* tmpImageName = NULL;
			while (tmp)
			{
				if (tmp->restype == 1)
				{
					// Save this off in case we are reloading an image.
					tmpImageName = new char [strlen(tmp->resval.rstring) + 1];
					strcpy(tmpImageName, tmp->resval.rstring);
				}
				if (tmp->restype == 280)
				{
					if (tmp->resval.rint == 0)
						Ras_Unloadpixelmapbuffer(elp);
					else
					{
						char ffname[IC_ACADBUF];
						if(RTNORM!=sds_findfile(tmpImageName,ffname))
						{
							if(NULL==strchr(tmpImageName,'\\') || RTNORM!=sds_findfile(strchr(tmpImageName,'\\')+1,ffname))
							{
								sds_printf(ResourceString(IDC_SDS_DB__N___UNABLE_TO_LO_0, "\n-- Unable to locate image file %s. --\n" ),tmpImageName);
								IC_FREE(tmpImageName); tmpImageName = NULL;
								ret = SDS_RSERR;
							}
						}

						char dllpath[IC_ACADBUF];
						if(RTNORM!=sds_findfile(SDS_PROGRAMFILE,dllpath))
						{
							IC_FREE(tmpImageName); tmpImageName = NULL;
							ret = RTERROR;
						}
						if(strrchr(dllpath,'\\')) *strrchr(dllpath,'\\')=0;

						if (ret == SDS_RSERR)
						{
							ret = RTNORM;		// Image file is not found. Not really an error,
							// but don't want to call Ras_Reloadpixelmapbuffer()
							// Intensionally leaving the 280 set to 1.
						}
						else
							Ras_Reloadpixelmapbuffer(elp, tmpImageName, dllpath);
					}
					break;
				}
				tmp = tmp->rbnext;
			}
			IC_FREE(tmpImageName); tmpImageName = NULL;
		}

		// Set the Undo.
		if(db_CompEntLinks(&chglst,savelp,elp)==RTNORM && chglst)
			SDS_SetUndo(IC_UNDO_CHG_ENT_TAB_CLS,(void *)elp,(void *)chglst,NULL,SDS_CURDWG);

		// Regenerating display object (Entity only).
		if(db_is_entitytype(elp->ret_type()))
		{
			if(tilemode==0)
			{
				if(elp->ret_type()!=DB_POLYLINE && elp->ret_type()!=DB_VERTEX && elp->ret_type()!=DB_ATTRIB)
					sds_entupd(trbp1->resval.rlname);
			}
			else
			{
				// Scary change here
				//				if(elp->ret_type()!=DB_POLYLINE && elp->ret_type()!=DB_VERTEX && elp->ret_type()!=DB_ATTRIB) {
				if(elp->ret_type()!=DB_VERTEX && elp->ret_type()!=DB_ATTRIB)
					//					sds_redraw(trbp1->resval.rlname,IC_REDRAW_DRAW);
					//					sds_redraw(trbp1->resval.rlname,IC_REDRAW_UNDRAW);
					sds_entupd(trbp1->resval.rlname);
			}
		}
		SDS_CallUserCallbackFunc(SDS_CBENTMOD,(void *)trbp1->resval.rlname,NULL,NULL);
	}

	delete savelp;
	return(ret);
}

// *****************************************
// This is an SDS External API
//
int
sdsengine_entupd(const sds_name nmEntity)
	{

	ASSERT( OnEngineThread() );

	if( nmEntity==NULL ||
		nmEntity[0]==0L ||
		nmEntity[1]==0L )
		{
		return(RTERROR);
		}

	extern bool SDS_IsFromOsnapToXref;	/*D.G.*/// We don't redraw temporary entities which are created
	if(SDS_IsFromOsnapToXref)			// from xref blocks in sds_osnap.
		return(RTNORM);

	int extmode=3;
	db_handitem *telp=(db_handitem *)nmEntity[0];
	db_drawing *flp=(db_drawing *)nmEntity[1];
	sds_name ename;

	ename[1]=nmEntity[1];

	if(flp->ret_nmainents()==1)
		{
		extmode=7;
		}

	if(!db_is_entitytype(telp->ret_type()))
	{
		return(RTNORM);
	}

	// If you pass a sub ent step back to the start.
	for(; telp!=NULL && telp->ret_type()==DB_VERTEX; telp=telp->next);

	for(; telp!=NULL && telp->ret_type()==DB_ATTRIB; telp=telp->next);

	if(telp==NULL)
		{
		return(RTERROR);
		}

	if(telp->ret_type()==DB_SEQEND)
		{
		((db_seqend *)telp)->get_mainent(&telp);
		}

	do
		{
		ename[0]=(long)telp;
		if(!SDS_EntMakingEnt)
			{
			sds_redraw(ename,-IC_REDRAW_UNDRAW);
			}

		if(RTNORM!=SDS_UpdateEntDisp(ename,extmode))
			{
			return(RTERROR);
			}

		sds_redraw(ename,-IC_REDRAW_DRAW);
		telp=telp->next;
		}
		while(telp && (telp->ret_type()==DB_ATTRIB));


	return(RTNORM);
	}

// *****************************************
// This is an SDS External API
//
int
sdsengine_handent(const db_objhandle EntHandle, sds_name nmEntity)
	{

	ASSERT( OnEngineThread() );

	if(SDS_CURDWG==NULL)
		{
		return(RTERROR);
		}
	long fl1=(long)SDS_CURDWG->handent(EntHandle);

	if(fl1==0L)
		{
		return(RTERROR);
		}

	nmEntity[1]=(long)SDS_CURDWG;
	nmEntity[0]=fl1;

	return(RTNORM);
	}

// *****************************************
// This is an SDS External API
//
struct sds_resbuf *
sdsengine_entget(const sds_name nmEntity)
	{

	ASSERT( OnEngineThread() );

	return sdsengine_entgetx( nmEntity, NULL );
	}

/*-------------------------------------------------------------------------*//**
helper function to find the db_blocktabrec that keeps pointer
	to xref drawing in which pFindEnt is kept. Function uses
	db_handitem::m_parentBlock as pointer to db_blocktabrec for xrefs
@param pFindEnt	=> it is looked entity for
@param pXrefBlk	<= reference to the pointer to found block table record
@return true if pFindEnt was found. Bisides if pXrefBlk != NULL then pFindEnt
			was found in xref drawing
*//*--------------------------------------------------------------------------*/
bool findXrefByEnt(db_handitem* pFindEnt, db_blocktabrec*& pXrefBlk)
{
	ASSERT(pFindEnt);
	db_blocktabrec* bthip = (db_blocktabrec*)pFindEnt->m_parentBlock;
	if(bthip)
	{
		ASSERT(!bthip->ret_deleted());
		if(bthip->ret_flags() & IC_BLOCK_XREF)
		{
			pXrefBlk = bthip;
			return true;
		}
		else
		{
			bthip = (db_blocktabrec*)bthip->m_parentBlock; // blockdef has pointer to xref
			if(bthip && bthip->ret_flags() & IC_BLOCK_XREF)
			{
				pXrefBlk = bthip;
				return true;
			}
		}
	}
	return false;
}

// *****************************************
// This is an SDS External API
//
struct sds_resbuf *
sdsengine_entgetx(const sds_name nmEntity, const struct sds_resbuf *prbAppList)
	{

	ASSERT( OnEngineThread() );

	if(nmEntity==NULL)
		{
		return(NULL);
		}
	if(nmEntity[0]==0L ||
		nmEntity[1]==0L)
		{
		return(NULL);
		}
	db_handitem *pItem = (db_handitem *)nmEntity[0];
	db_drawing *pDrawing = (db_drawing *)nmEntity[1];
	ASSERT( CHECKSTRUCTPTR( pItem ) );
	ASSERT( CHECKSTRUCTPTR( pDrawing ) );


	struct sds_resbuf *pRetval = NULL;
	__try
		{
		if ( pItem->ret_deleted())
			{
			pRetval = NULL;
			}
		else
			{


			// this is a BLOCK_RECORD.	entget returns the information about the BLOCK itself.
			// So, lets get the BLOCK and call its entget method
			//
			if ( pItem->ret_type() == DB_BLOCKTABREC )
				{
				db_blocktabrec *pBlockTableRecord = (db_blocktabrec *)(pItem);

				db_block *pActualBlock = pBlockTableRecord->ret_block();

				ASSERT( pActualBlock != NULL );
				ASSERT( pActualBlock->ret_type() == DB_BLOCK );

				return pActualBlock->entgetx( prbAppList, pDrawing );
				}

			pRetval = pItem->entgetx( prbAppList, pDrawing );
			}
		}
	__except( EXCEPTION_EXECUTE_HANDLER )
		{
		pRetval = NULL;
		}
	if(pRetval)
	{
		// If nmEntity[0] is kept into an xref drawing
		// then layer name and linetype name should be changed to "xref_name|layer_name"
		db_drawing* owner = pItem->ret_xrefdp();
		db_blocktabrec* pXrefBlkTblRec = NULL;
		if(findXrefByEnt(pItem, pXrefBlkTblRec)	&& pXrefBlkTblRec)
		{
			db_handitem* pTopLayerHip = pItem->ret_layerhip();
			GetTopLevelLayerHip(pDrawing, pXrefBlkTblRec, &pTopLayerHip);
			resbuf* rbLayerName = ic_ret_assoc(pRetval, 8);
			if(pTopLayerHip && rbLayerName)
			{	// replace layer name
				rbLayerName->resval.rstring = ic_realloc_char(rbLayerName->resval.rstring,
										strlen(((db_layertabrec*)pTopLayerHip)->retp_name())+1);
				strcpy(rbLayerName->resval.rstring, ((db_layertabrec*)pTopLayerHip)->retp_name());
			}

			// replace linetype name
			char toplevelname[512];
			sprintf(toplevelname,"%s|%s", pXrefBlkTblRec->retp_name(),
				((db_ltypetabrec*)(pItem->ret_ltypehip()))->retp_name());
			db_handitem* pTopLtypeHip = pDrawing->findtabrec(DB_LTYPETAB,toplevelname,0);
			resbuf* rbLtypeName = ic_ret_assoc(pRetval, 6);
			if(pTopLtypeHip && rbLtypeName)
			{	// replace linetype name
				rbLtypeName->resval.rstring = ic_realloc_char(rbLtypeName->resval.rstring,
										strlen(((db_ltypetabrec*)pTopLtypeHip)->retp_name())+1);
				strcpy(rbLtypeName->resval.rstring, ((db_ltypetabrec*)pTopLtypeHip)->retp_name());
			}
		}
	}

	return pRetval;
	}

// *****************************************
// This is an SDS External API
//
int
sdsengine_entsel(const char *szSelectMsg, sds_name nmEntity, sds_point ptSelected, bool bDirectCall )
	{

	ASSERT( OnEngineThread() );

	int ret = RTNORM, inlst = 0, breakflg = 0, hadpt = 0, haden = 0, osmode = -1;
	sds_name ss;
	struct resbuf rb;

	nmEntity[0] = nmEntity[1] = 0L;
	ptSelected[0] = ptSelected[1] = ptSelected[2] = 0.0;

	if( !SDSApplication::GetEngineApplication()->GetMainThread()->IsCurrent() && bDirectCall && SDS_AtCmdLine )
	{
		// redirect call to the Engine posting a job there to prevent deadlock
		CIcadDoc *pDoc = SDS_CURDOC;
		if( !pDoc )
			return RTERROR;

		HRESULT hr = pDoc->AUTO_entsel( szSelectMsg, nmEntity, ptSelected );
		if( FAILED( hr ) )
			ret = RTERROR;

		return ret;
	}

	SDS_getvar(NULL,DB_QOSMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	osmode=rb.resval.rint;

	enametop: ;

	ret=RTNORM;

	struct resbuf *pNextToken = GetActiveCommandQueue()->PeekItem();

	while( pNextToken )
		{

		switch( pNextToken->restype )
			{
			case RTLB:
				inlst=1;
				pNextToken = GetActiveCommandQueue()->AdvanceNext();
				break;
			case RTLE:
				inlst=0;
				pNextToken = GetActiveCommandQueue()->AdvanceNext();
				goto out;
			case RTPICKS:
				haden=1;
				sds_ssname(pNextToken->resval.rlname,0,nmEntity);
				pNextToken = GetActiveCommandQueue()->AdvanceNext();
				break;
			case RTENAME:
				haden=1;
				ic_encpy(nmEntity,pNextToken->resval.rlname);
				pNextToken = GetActiveCommandQueue()->AdvanceNext();
				// Modified CyberAge VSB 17/09/2001 [
				// Reason : Bug 77843 from Bugzilla.Solved with help from Denis Petrov.
				if(inlst == 0)
					breakflg=1;
				// Modified CyberAge 17/09/2001 ]
				break;
			case RTPOINT:
			case RT3DPOINT:
				hadpt=1;
				ic_ptcpy(ptSelected,pNextToken->resval.rpoint);
				pNextToken = GetActiveCommandQueue()->AdvanceNext();
				if(!inlst)
					{
					goto gotpt;
					}
				break;
			default:
				breakflg=1;
				break;
			}
		if(breakflg)
			{
			break;
			}
	}
	if(haden && !hadpt)
		{
		goto out;
		}

	SDS_getvar(NULL,DB_QERRNO,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

	extern int SDS_AtEntGet;
	++SDS_AtEntGet;
	SDS_SetCursor(SDS_GetPickCursor());

	if(osmode) {
		rb.restype=RTSHORT;
		rb.resval.rint=0;
		SDS_setvar(NULL,DB_QOSMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
		SDS_UpdateSetvar=(char *)"OSMODE"/*DNT*/;
		ExecuteUIAction( ICAD_WNDACTION_UDCHKSTAT );
		ExecuteUIAction( ICAD_WNDACTION_UDTBSTATE );
		ExecuteUIAction( ICAD_WNDACTION_STATCHANGE );
	}

	if(RTNORM!=(ret=SDS_AskForPoint( NULL,szSelectMsg==NULL ? ResourceString(IDC_SDS_DB__NSELECT_ENTITY___1, "\nSelect entity: " ) : szSelectMsg,ptSelected))) {
		--SDS_AtEntGet;
		if(ret==RTENAME) goto enametop;
		if(ret==RTKWORD || ret==RTCAN) goto out;
		rb.resval.rint=OL_ENTSELNULL;
		SDS_setvar(NULL,DB_QERRNO,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
		ret=RTERROR;
		goto out;
	}
	--SDS_AtEntGet;

	gotpt: ;

	if(RTNORM!=(ret=sds_pmtssget(NULL,NULL,ptSelected,NULL,NULL,ss,0)) || RTNORM!=(ret=sds_ssname(ss,0L,nmEntity))) {
		rb.resval.rint=OL_ENTSELPICK;
		SDS_setvar(NULL,DB_QERRNO,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
		ret=RTERROR;
		goto out;
	}

	out: ;

	sds_ssfree(ss);

	if(osmode) {
		rb.restype=RTSHORT;
		rb.resval.rint=osmode;
		SDS_setvar(NULL,DB_QOSMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
		SDS_UpdateSetvar=(char *)"OSMODE"/*DNT*/;
		ExecuteUIAction( ICAD_WNDACTION_UDCHKSTAT );
		ExecuteUIAction( ICAD_WNDACTION_UDTBSTATE );
		ExecuteUIAction( ICAD_WNDACTION_STATCHANGE );
	}

	SDS_SetCursor(IDC_ICAD_CROSS);
	return(ret);
}

// *****************************************
// This is an SDS External API
//
int
sdsengine_nentsel(const char		   *szNEntMsg,
				sds_name			nmEntity,
				sds_point			ptEntPoint,
				sds_point			ptECStoWCS[4],
				struct sds_resbuf **pprbNestBlkList)
	{

	ASSERT( OnEngineThread() );

	if( SDS_CURDWG==NULL ||
		nmEntity==NULL ||
		ptEntPoint==NULL ||
		ptECStoWCS==NULL)
		{
		return(RTERROR);
		}

	sds_matrix mat;

	int ret=sds_nentselp(szNEntMsg,nmEntity,ptEntPoint,0,mat,pprbNestBlkList);

	if(ret==RTERROR)
		{
		return(ret);
		}

	int fi1,fi2;

	/* Convert new 4x4 matrix to the old 4x3 matrix: */
	for (fi1=0; fi1<4; fi1++)
		{
		for (fi2=0; fi2<3; fi2++)
			{
			ptECStoWCS[fi1][fi2]=mat[fi2][fi1];
			}
		}

	return(ret);
	}

// *****************************************
// This is an SDS External API
//
int
sdsengine_nentselp(const char			*szNEntMsg,
				 sds_name			 nmEntity,
				 sds_point			 ptEntPoint,
				 int				 swUserPick,
				 sds_matrix			 mxECStoWCS,
				 struct sds_resbuf **pprbNestBlkList)
	{

	ASSERT( OnEngineThread() );

	struct resbuf rb,*trb=NULL;
	double fr1;
	sds_name ss;
	db_handitem *newhip;
	struct gr_view *view=SDS_CURGRVW;

	if( view==NULL ||
		SDS_CURDWG==NULL ||
		nmEntity==NULL ||
		ptEntPoint==NULL ||
		mxECStoWCS==NULL)
		{
		return(RTERROR);
		}

	int iRetval = RTERROR;

	if(!swUserPick)
		{
		iRetval = sds_entsel(szNEntMsg,nmEntity,ptEntPoint);
		}
	else
		{
		ss[1]=0L;  // this ssget seems to be having trouble sometimes.
		iRetval = sds_pmtssget(NULL,NULL,ptEntPoint,NULL,NULL,ss,0, true, false, true, false);
		if(iRetval == RTNORM)
			iRetval = sds_ssname(ss,0L,nmEntity);
		sds_ssfree(ss);
		}
	if ( iRetval == RTNORM )
		{


		// Calc the corners of the selector.
		if(SDS_getvar(NULL,DB_QVIEWSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
			{
			return(RTERROR);
			}
		fr1=rb.resval.rreal;

		if(SDS_getvar(NULL,DB_QSCREENSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
			{
			return(RTERROR);
			}

		fr1/=rb.resval.rpoint[1];
		if(SDS_getvar(NULL,DB_QPICKBOX,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
			{
			return(RTERROR);
			}

		fr1*=rb.resval.rint;

		sds_point p1,p3,tpt;
		gr_ucs2rp(ptEntPoint,tpt,view);
		p1[0]=tpt[0]-fr1; p1[1]=tpt[1]-fr1; p1[2]=0.0;
		p3[0]=tpt[0]+fr1; p3[1]=tpt[1]+fr1; p3[2]=0.0;

		trb=sds_buildlist(RT3DPOINT,p1,RT3DPOINT,p3,0);
		gr_selectorlink  *sl=gr_initselector('W','C',trb);
		sds_relrb(trb);

		// get the main view's target device DC in case we have any TrueType text
		CDC *dc = NULL;
		CIcadView *icadView = SDS_CMainWindow->GetIcadView();
		if (icadView)
			{
			ASSERT_KINDOF(CIcadView, icadView);
			dc = icadView->GetTargetDeviceCDC();
			}

		int result = gr_nentselphelper(0,
									   (db_handitem *)nmEntity[0],
									   sl,
									   view,
									   SDS_CURDWG,
									   dc,
									   &newhip,
									   mxECStoWCS,
									   pprbNestBlkList);

		if (result)
			{
			gr_freeselector(sl);
			return(RTERROR);
			}

		if(newhip)
			{
			nmEntity[0]=(long)newhip;
			}

		gr_freeselector(sl);
		}

	return( iRetval );

	}

/****************************************************************************/
/*********************** Extended Entity Functions **************************/
/****************************************************************************/

// *****************************************
// This is an SDS External API
//
int
sdsengine_xdroom(const sds_name nmEntity, long *plMemAvail)
	{

	ASSERT( OnEngineThread() );

	struct resbuf *elist,rb;
	long fl1=0L;

	if(plMemAvail==NULL)
		{
		return(RTERROR);
		}

	rb.restype=RTSTR;
	rb.resval.rstring="*"/*DNT*/;
	rb.rbnext=NULL;

	if((elist=sds_entgetx((long *)nmEntity,&rb))==NULL)
		{
		return(RTERROR);
		}


	if(ic_assoc(elist,-3)==0)
		{
		sds_xdsize(ic_rbassoc,&fl1);
		}

	*plMemAvail=(16383L - fl1);
	IC_FREEIT return(RTNORM);
	}

// *****************************************
// This is an SDS External API
//
int
sdsengine_xdsize(const struct sds_resbuf *prbEntData, long *plMemUsed)
	{

	struct resbuf *rbtmp=NULL;
	long fl1=0;

	if(plMemUsed==NULL)
		{
		return(RTERROR);
		}

	if( prbEntData==NULL ||
		(prbEntData->restype!=(-3) &&
		prbEntData->restype!=1001))
		{
		*plMemUsed=0; return(RTERROR);
		}

	for(rbtmp=(struct resbuf *)prbEntData; rbtmp!=NULL; rbtmp=rbtmp->rbnext)
		{
		switch(rbtmp->restype)
			{
			case (-3):
				break;

			case 1000:
				if (NULL == rbtmp->resval.rstring)
					{
					*plMemUsed = 0;
					return RTERROR;
					}

				fl1 += (long) (strlen(rbtmp->resval.rstring) + 2L);
				break;

			case 1001:
				fl1+=3L;
				break;

			case 1002:
				fl1+=2L;
				break;

			case 1003:
				fl1+=3L;
				break;

			case 1004:
				fl1+=(long)(rbtmp->resval.rbinary.clen+2L);
				break;

			case 1005:
				fl1+=9L;
				break;

			case 1010:
				fl1+=25L;
				break;

			case 1011:
				fl1+=25L;
				break;

			case 1012:
				fl1+=25L;
				break;

			case 1013:
				fl1+=25L;
				break;

			case 1040:
				fl1+=9L;
				break;

			case 1041:
				fl1+=9L;
				break;

			case 1042:
				fl1+=9L;
				break;

			case 1070:
				fl1+=3L;
				break;

			case 1071:
				fl1+=5L;
				break;

			default:
				*plMemUsed=0;
				return(RTERROR);
			}
		if(fl1>16383L)
			{
			cmd_ErrorPrompt(CMD_ERR_TOOMUCHDATA,0);
			fl1=0; break;
			}
		}

	*plMemUsed=fl1;
	return(RTNORM);

	}

/****************************************************************************/
/*********************** Dictionary Functions *******************************/
/****************************************************************************/

// *****************************************
// This is an SDS External API
//
int
sdsengine_namedobjdict(sds_name nmDict)
	{

	ASSERT( OnEngineThread() );

	if( SDS_CURDWG==NULL ||
		nmDict==NULL)
		{
		return(RTERROR);
		}

	nmDict[0]=(long)SDS_CURDWG->namedobjdict();
	nmDict[1]=(long)SDS_CURDWG;

	return(RTNORM);
	}

// *****************************************
// This is an SDS External API
//
int sdsengine_dictadd(const sds_name nmDict, const char *szAddThis, const sds_name nmNonGraph)
{
	ASSERT(OnEngineThread());

	if(nmDict==NULL || nmDict[0]==0L || nmDict[1]==0L || szAddThis==NULL || *szAddThis==0 ||
		nmNonGraph==NULL || nmNonGraph[0]==0L || nmNonGraph[1]==0L)
		return(RTERROR);

	db_handitem* elp;
	db_handitem* savelp=NULL;
	struct db_BinChange *chglst=NULL;

	elp = (db_handitem*)(nmDict[0]);
	SDS_CopyEntLink(&savelp, elp);

	int ret=((db_drawing *)nmDict[1])->dictadd(((db_handitem *)nmDict[0]),(char *)szAddThis,((db_handitem *)nmNonGraph[0]));

	if(!ret)
	{
		SDS_DBModified( IC_DBMOD_ENTITIES_MODIFIED );
		if(db_CompEntLinks(&chglst, savelp, elp ) == RTNORM && chglst)
			SDS_SetUndo(IC_UNDO_CHG_ENT_TAB_CLS, (void*)elp, (void*)chglst, NULL, SDS_CURDWG);
		SDS_CallUserCallbackFunc(SDS_CBENTMOD, (void*)nmDict, NULL, NULL);
	}
	if(savelp)
		delete savelp;
	if(!ret)
		return(RTNORM);
	return(RTERROR);
}

// *****************************************
// This is an SDS External API
//
int sdsengine_dictdel(const sds_name nmDict, const char *szDelThis)
{
	ASSERT(OnEngineThread());

	if(nmDict==NULL || nmDict[0]==0L || nmDict[1]==0L || szDelThis==NULL || *szDelThis==0)
		return(RTERROR);

	db_handitem* elp;
	db_handitem* savelp=NULL;
	struct db_BinChange *chglst=NULL;

	elp = (db_handitem*)(nmDict[0]);
	SDS_CopyEntLink(&savelp, elp);

	int ret=((db_drawing *)nmDict[1])->dictdel(((db_handitem *)nmDict[0]),(char *)szDelThis);

	if(!ret)
	{
		SDS_DBModified( IC_DBMOD_ENTITIES_MODIFIED );
		if(db_CompEntLinks(&chglst, savelp, elp ) == RTNORM && chglst)
			SDS_SetUndo(IC_UNDO_CHG_ENT_TAB_CLS, (void*)elp, (void*)chglst, NULL, SDS_CURDWG);
		SDS_CallUserCallbackFunc(SDS_CBENTMOD, (void*)nmDict, NULL, NULL);
	}
	if(savelp)
		delete savelp;
	if(!ret)
		return(RTNORM);
	return(RTERROR);
}

// *****************************************
// This is an SDS External API
//
int	sdsengine_dictrename(const sds_name nmDict, const char *szOldName, const char *szNewName)
{
	ASSERT(OnEngineThread());

	if(nmDict==NULL || nmDict[0]==0L || nmDict[1]==0L || szOldName==NULL || *szOldName==0 || szNewName==NULL || *szNewName==0)
		return(RTERROR);

	db_handitem* elp;
	db_handitem* savelp=NULL;
	struct db_BinChange *chglst=NULL;

	elp = (db_handitem*)(nmDict[0]);
	SDS_CopyEntLink(&savelp, elp);

	int ret=((db_drawing *)nmDict[1])->dictrename(((db_handitem *)nmDict[0]),(char *)szOldName,(char *)szNewName);

	if(!ret)
	{
		SDS_DBModified(IC_DBMOD_ENTITIES_MODIFIED);
		if(db_CompEntLinks(&chglst, savelp, elp ) == RTNORM && chglst)
			SDS_SetUndo(IC_UNDO_CHG_ENT_TAB_CLS, (void*)elp, (void*)chglst, NULL, SDS_CURDWG);
		SDS_CallUserCallbackFunc(SDS_CBENTMOD, (void*)nmDict, NULL, NULL);
	}
	if(savelp)
		delete savelp;
	if(!ret)
		return(RTNORM);
	return(RTERROR);
}

// *****************************************
// This is an SDS External API
//
struct sds_resbuf *
sdsengine_dictsearch(const sds_name nmDict, const char *szFindThis, int swFirst)
{
	ASSERT(OnEngineThread());

	if(nmDict==NULL || nmDict[0]==0L || nmDict[1]==0L || szFindThis==NULL || *szFindThis==0)
		return(NULL);

	db_handitem *hip=((db_drawing *)nmDict[1])->dictsearch(((db_handitem *)nmDict[0]),(char *)szFindThis,swFirst);
	if(hip==NULL)
		return(NULL);

	return(hip->entgetx(NULL,(db_drawing *)nmDict[1]));
}

// *****************************************
// This is an SDS External API
//
struct sds_resbuf *
sdsengine_dictnext(const sds_name nmDict, int swFirst)
{
	ASSERT(OnEngineThread());

	if(nmDict==NULL || nmDict[0]==0L || nmDict[1]==0L)
		return(NULL);

	db_handitem *hip=((db_drawing *)nmDict[1])->dictnext(((db_handitem *)nmDict[0]),swFirst);
	if(hip==NULL)
		return(NULL);

	return(hip->entgetx(NULL,(db_drawing *)nmDict[1]));
}

/****************************************************************************/
/*********************** MLINE specific helper Functions ********************/
/****************************************************************************/
int SDS_copyMLineStyle (char* SourceStyleName, db_drawing *SourceDWG, db_drawing *TargetDWG, sds_name *CopiedEname)
{
	struct sds_resbuf *tmprb, *stylerb = NULL;
	db_handitem *SourceMstyleDict, *TargetMstyleDict, *mstyle;
	int ret = RTERROR;

	//Find the source MLineStyle dictionary
	SourceMstyleDict = SourceDWG->dictsearch(SourceDWG->namedobjdict(), "ACAD_MLINESTYLE"/*DNT*/, 0);
	if (!SourceMstyleDict)
		goto out;

	//Find the target MLineStyle dictionary
	TargetMstyleDict = TargetDWG->dictsearch(TargetDWG->namedobjdict(), "ACAD_MLINESTYLE"/*DNT*/, 0);
	if (!TargetMstyleDict)
		goto out;

	//Check to see if the source style is in the source document
	mstyle = SourceDWG->dictsearch(SourceMstyleDict, SourceStyleName, 0);
	if (!mstyle)
		goto out; //impossible - it has to be in the source doc atleast.
	stylerb = mstyle->entgetx (NULL, SourceDWG);
	for (tmprb = stylerb; tmprb != NULL; tmprb = tmprb->rbnext)
	{
		if (tmprb->restype == 330)
		{
			//this entry must point to the source dict
			ASSERT (tmprb->resval.rlname[0] == (long)SourceMstyleDict);
			//set it to point to the target dict
			tmprb->resval.rlname[0] = (long)TargetMstyleDict;
			tmprb->resval.rlname[1] = (long)TargetDWG;
			break;
		}
	}
	//TODO
	//Copy all the linetype's in the source MLineStyle from the source dwg to the Target dwg
	//go through all the elements in the MLineStyle, get_6 on each and ensure they exist in the Target

	//now make the MLineStyle
	ret = SDS_entmake (stylerb, TargetDWG);
	*CopiedEname[0] = (long)SDS_LastHandItem;
	*CopiedEname[1] = (long)TargetDWG;

out:
	if (stylerb)
		sds_relrb(stylerb);
	return(ret);
}

#pragma optimize( "", off )
//This function is called when inserting an MLine from one file into another.
//The MLine (newename) definition in the new drawing needs to have its style
//updated to match the original Mline (ename)
int SDS_updateMLine(sds_name ename, sds_name newename)
{
	struct sds_resbuf *elist = NULL, *tmprb, *newelist = NULL;
	db_drawing *source = (db_drawing*)ename[1];
	db_drawing *target = (db_drawing*)newename[1];
	db_handitem *namedObjectDict, *mstyleDict, *mstyle;
	sds_name mstyle_name;
	int ret = RTERROR;

	if(target)
		SDS_SetUndo(IC_UNDO_GROUP_BEGIN,NULL,(void*)1,NULL,target);

	elist = sds_entget(ename);
	ASSERT(elist != NULL);
	if (elist == NULL)
		goto out;

	if(ic_assoc(elist,2)!=0)
		goto out;
	tmprb=ic_rbassoc;

	//Search the current drawing for an MLineStyle dictionary
	namedObjectDict=target->namedobjdict();
	mstyleDict = target->dictsearch(namedObjectDict, "ACAD_MLINESTYLE"/*DNT*/, 0);
	if (!mstyleDict)  //this dictionary is always there
		goto out;

	//Check to see if the source style is in the target document
	mstyle = target->dictsearch(mstyleDict, tmprb->resval.rstring, 0);
	if (!mstyle)
	{
		//Copy it from the source drawing to the current one
		ret = SDS_copyMLineStyle (tmprb->resval.rstring, source, target, &mstyle_name);
		if (ret != RTERROR)
		{
			//Add it to the dictionary
			sds_name msdict_name;
			msdict_name[0] = (long)mstyleDict;
			msdict_name[1] = newename[1];
			ret = sds_dictadd(msdict_name, tmprb->resval.rstring, mstyle_name);
		}
		mstyle_name[1] = (long) target;
	}
	else
	{
		mstyle_name[0] = (long) mstyle;
		mstyle_name[1] = (long) target;
	}

	//set the new style pointer for the new mline
	newelist = sds_entget(newename);
	ASSERT(newelist != NULL);
	if (newelist == NULL)
		goto out;

	//Go to the end of list and add the modified data
	for(tmprb=newelist; tmprb->rbnext!=NULL; tmprb=tmprb->rbnext);
	tmprb->rbnext=sds_buildlist(340,mstyle_name,0);
	sds_entmod (newelist);

out:
	if (elist)
		sds_relrb (elist);
	if (newelist)
		sds_relrb (newelist);
	if(target)
		SDS_SetUndo(IC_UNDO_GROUP_END,NULL,(void*)1,NULL,target);
	return ret;
	}

#pragma optimize( "", on )

void CCmdHelper::clearOops()
{
	m_erased.clear();
	for(MModified::iterator it = m_modified.begin(); it != m_modified.end(); ++it)
		delete it->second.second;
	m_modified.clear();
}

void CCmdHelper::oopsAddErased(db_handitem* hip)
{
	ASSERT(m_erased.find(hip) == m_erased.end());
	m_erased[hip] = 0;
	setOopsAvailable(true);
}

void CCmdHelper::oopsAddModified(db_handitem* hip, db_BinChange* change,
		const std::vector<db_handitem*>& initiators)
{
	ASSERT(m_modified.find(hip) == m_modified.end());
	CModifiedData* data = new CModifiedData();
	data->m_change = change;
	int count = 0;
	for(int i = 0; i < initiators.size(); ++i)
		if(initiators[i]->ret_deleted())
			++count;
	data->m_initiators.resize(count);
	for(i = 0; i < initiators.size(); ++i)
		if(initiators[i]->ret_deleted())
			data->m_initiators[--count] = initiators[i];

	std::pair<int, CModifiedData*>& ref = m_modified[hip];
	ref.first = 0;
	ref.second = data;
	setOopsAvailable(true);
}

void CCmdHelper::oopsProcess(CIcadDoc* pDoc)
{
	if(!m_bOopsAvailable)
		return;
	m_bOopsAvailable = false;
	db_drawing* dp = pDoc->m_dbDrawing;
	ASSERT(dp);
	setCheckCallback(false);
	sds_name ent;
	ent[1] = (long)dp;
	sds_ssfree(pDoc->m_pPrevSelection);
	SDS_ssadd(NULL, NULL, pDoc->m_pPrevSelection);
	for(MErased::iterator it = m_erased.begin(); it != m_erased.end(); ++it)
	{
		if(it->second == 0)
		{
			if(it->first->ret_deleted())
			{
				ent[0] = (long)it->first;
				sds_entdel(ent);
				if(!it->first->ret_deleted())
					sds_ssadd(ent, pDoc->m_pPrevSelection, pDoc->m_pPrevSelection);
			}
			else
				it->second = 1;
		}
	}
	for(MModified::iterator it1 = m_modified.begin(); it1 != m_modified.end(); ++it1)
	{
		if(it1->second.first)
			continue;
		bool bChanged = false;
		for(int i = 0; i < it1->second.second->m_initiators.size(); ++i)
		{
			MErased::iterator it = m_erased.find(it1->second.second->m_initiators[i]);
			if(it != m_erased.end() && it->second)
			{
				bChanged = true;
				break;
			}
		}
		if(bChanged)
			continue;

		db_handitem* hip = it1->first->newcopy();
		if(!hip)
			continue;
		db_BinChange *tcl = it1->second.second->m_change;
		hip->SetCreateAssocFlag(false);
		VERIFY(tcl->UndoChanges(dp, hip));
		hip->SetCreateAssocFlag(true);
		sds_resbuf* rb = hip->entgetx(NULL, dp);
		hip->SetCreateAssocFlag(false);
		VERIFY(tcl->RedoChanges(dp, hip));
		hip->SetCreateAssocFlag(true);
		delete hip;
		if(rb)
		{
			if(rb->restype == -1)
			{
				rb->resval.rlname[0] = (long)(it1->first);
				rb->resval.rlname[1] = (long)dp;
			}
			sds_entmod(rb);
			sds_relrb(rb);
			cmd_updateReactors(it1->first);
		}
	}
	clearOops();
}

void CCmdHelper::callbackChanged(db_handitem* hip, bool bUndo)
{
	if(!isCheckCallback())
		return;
	MErased::iterator it = m_erased.find(hip);
	if(it != m_erased.end())
		it->second += bUndo ? -1 : 1;
	MModified::iterator it1 = m_modified.find(hip);
	if(it1 != m_modified.end())
		it1->second.first += bUndo ? -1 : 1;
}

void CCmdHelper::setOopsAvailable(bool bOn)
{
	m_bOopsAvailable = bOn;
	if(bOn)
	{
		setCheckCallback(true);
	}
	else
	{
		setCheckCallback(false);
		clearOops();
	}
}

CCmdHelper::CModifiedData::~CModifiedData()
{
	delete m_change;
}
