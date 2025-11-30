// \PRJ\ICAD\OSNAPHELPERS.CPP

#include "stdAfx.h"

#include "osnapHelpers.h"
#include "osnapManagerImp.h"
#include "sdsapplication.h"

#include "cmds.h"/*DNT*/
#include "IcadView.h"
#include "IcadEntityRenderer.h"
#include "IcadHelpers.h"/*DNT*/
#include "TransformUtils.h"
#include "Modeler.h"
#include "gvector.h"
#include "transf.h"
#include "xstd.h"
#include "GeoData.h"
#include "elliparc.h"
#include "nurbs.h"
#include "splineutils.h"
#include "CmdQueryTools.h" //wcs2ucs
#include <vector>
using std::vector;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//** Globals
extern signed char SDS_OSNAP_TANPER_IGNORE;
extern bool SDS_IsFromOsnapToXref;		/*D.G.*/// Used only in sds_osnap & sdsengine_entupd
										// (look at comments in sds_osnap).

extern int		 SDS_AtNodeDrag;
extern sds_name  SDS_NodeDragEnt;



struct pt_bulg 
{
	sds_point pt;
	sds_real bulge;
};

struct pface_edge
{
	int vtx[2];
	struct pface_edge *next;
};

void freePFaceEdgeList( struct pface_edge *list );
sds_real sds_dist2d (sds_point p1,sds_point p2);//proto for f'n found in sds1

short
ic_ssxss2
(
 sds_name			ss0,
 sds_name			ss1,
 sds_real			fuzz,
 sds_point			_seed,
 resbuf**			ptlistp,
 int				mode3d,
 long*				nskipped0p,
 long*				nskipped1p,
 EInterExtendOption	interOption   /* = eExtendNone*/,
 bool				bExtendAnyway /* = true*/
);


//////////////////////////////////////////////////////////////////////////////////////////////
void osnap_helper_addPtRec( int osmode, sds_point snapPt, sds_point snapPtRP, sds_point inPtRP, sds_point ptAp1rp, sds_point ptAp2rp );
int osnap_helper_elist(struct resbuf *elist, int osmode, sds_point pickptrp, sds_point pickptucs, sds_point ospt/*, sds_real *dsq*/,int pl_omitflag,sds_matrix *m_ecs2ucs, sds_point ptAp1rp, sds_point ptAp2rp);
//////////////////////////////////////////////////////////////////////////////////////////////
// Helper functions for COsnapManagerImp class (should be replaced with the appropriate
// methods of COsnapManagerImp class in future implementation
// Current implementation of these helper functions (and functions they call) are
// borrowed from sds_osnap.cpp file
//////////////////////////////////////////////////////////////////////////////////////////////

int osnap_helper_preprocessSS( sds_point ptAp1rp, sds_point ptAp2rp, sds_name ss, CHipArray& hipArray )
{
	sds_name entName, newSS;
	int res = 0;
	long snapSSLength = 0;
	gr_selectorlink  *sl=NULL;
	struct resbuf *rbTmp = NULL;
	CDC *dc = NULL;
	sds_matrix blkmtx; // = NULL;
	db_handitem *subenthip = NULL;

	sds_name_clear( newSS );

	sds_sslength( ss, &snapSSLength );
	if( !snapSSLength )
		return res;

	struct gr_view *view=SDS_CURGRVW;
	if( !view )
		return res;

	CIcadView *icadView = SDS_CMainWindow->GetIcadView();
	if( icadView )
	{
		ASSERT_KINDOF(CIcadView, icadView);
		dc = icadView->GetTargetDeviceCDC();
	}
	else
		return res;
	

	rbTmp = sds_buildlist( RT3DPOINT, ptAp1rp, RT3DPOINT, ptAp2rp, 0 );
	sl = gr_initselector( 'W', 'C', rbTmp );
	sds_relrb( rbTmp );
	rbTmp = NULL;

	while( sds_ssname( ss, 0L, entName ) == RTNORM ) // while ss is not empty
	{
		// first we should check the length of output selection set;
		// if it exceeds OSNAP_MAX_NUMBER then breaks this loop to avoid valuable overhead
		// during processing osnap entities; we will process the set which includes
		// at max OSNAP_MAX_NUMBER entities
		long newSSLen = 0;
		sds_sslength( newSS, &newSSLen );
		if( newSSLen > OSNAP_MAX_NUMBER )
		{
			sds_ssfree( ss );
			break;
		}

		// move an entity from the base SS to the new one
		sds_ssdel( entName, ss );
		sds_ssadd( entName, newSS, newSS );

		db_handitem *hip = (db_handitem*)entName[0];
		int type = hip->ret_type();
		if( type != DB_INSERT && type != DB_DIMENSION )
			continue;

		int grRes = gr_nentselphelper( 0, hip, sl, view, SDS_CURDWG, dc, &subenthip, blkmtx, NULL );

		if( grRes ) // an error
			continue;
		if( subenthip && (long)subenthip == entName[0] ) // should not be...
			continue;

		// transform matrix to UCS basis
		sds_matrix uw, wu, tmpMtx;
		ic_idmat( uw );
		ic_idmat( wu );
		uc2wcMatrix( wu );
		wc2ucMatrix( uw );
		multMtx2Mtx( blkmtx, wu, tmpMtx );
		multMtx2Mtx( uw, tmpMtx, blkmtx );

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//HACK - SDS_simplexforment will trigger a call to entmod which gets logged for undo
		//However, we are working with temp entities, which should never be logged for undo.
		//The hack is to temporarily turn Undo off before xforment and turn it back on after.
		struct resbuf undoctl;
		SDS_getvar(NULL,DB_QUNDOCTL,&undoctl,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		if (undoctl.resval.rint & IC_UNDOCTL_ON) 
		{
			undoctl.resval.rint &= ~IC_UNDOCTL_ON;  // turn Undo off
			sds_setvar("UNDOCTL",&undoctl);
		}

		//Since we will be changing a global value, enclose it in a CriticalSection, so
		//that none of the other threads will see this temporary change.
		SingleThreadSection localCriticalSection;
		localCriticalSection.Enter();

		//Trick to prevent callbacks on these temporary entities
		extern bool	  SDS_SetAUserCallback;
		bool Save_SDS_SetAUserCallback = SDS_SetAUserCallback;
		SDS_SetAUserCallback = FALSE;


		SDS_IsFromOsnapToXref = true;	/*D.G.*/// We don't redraw temporary entities
		                 				// which are created from xref or insert blocks in sub-
										// sequential calls of sdsengine_entupd (from SDS_xforment).
		
		int cnt = 0;	// let's count iterations and stop at OSNAP_MAX_NUMBER
		
		// Here is the loop thru all subentites starting from the 1st one - subenthip - which falls into the aperture.
		while( subenthip && cnt <= OSNAP_MAX_NUMBER )
		{
			cnt++;
			int	EntType;
			db_handitem	*TmpHip, *LayerHip;
			sds_name TmpEname;
			gr_displayobject *DispObjBeg, *DispObjEnd;
			CHipArray polyHipArray;

			bool ToAdd = false;

			EntType = subenthip->ret_type();

			/*D.G.*/// VERY important: stop processing at the end of the block/insert.
			// Old code commented out below.
			if(EntType == DB_ENDBLK || EntType == DB_SEQEND)
				break;

			/*if( EntType == DB_ENDBLK )
			{
				subenthip = subenthip->next;
				continue;
			}*/

			TmpHip = subenthip->newcopy();	// Note, we don't copy DispObjs
			TmpEname[0] = (long)TmpHip;
			TmpEname[1] = (long)SDS_CURDWG;

			ASSERT( EntType != DB_VERTEX );	// should not be!!!

			if( EntType == DB_POLYLINE )
			{
				// process all subentities of POLYLINE at once
				for( subenthip = subenthip->next; subenthip; subenthip = subenthip->next )
				{
					db_handitem *polySubEntHip = subenthip->newcopy();
					polyHipArray.Add( polySubEntHip );
					int count = polyHipArray.GetSize();
					if( count > 1 )
						polyHipArray[count-2]->next = polySubEntHip;

					// stop processing of polyline subentities
					if( polySubEntHip->ret_type() == DB_SEQEND )
						break;
				}

				if( polyHipArray.GetSize() )
					TmpHip->next = polyHipArray[0];
				else
				{
					delete TmpHip;
					subenthip = subenthip->next;
					continue;
				}
			}

			// Check for deleted objects and end stuff.
			if( TmpHip->ret_deleted() || TmpHip->ret_invisible() )
			{
				if( EntType == DB_POLYLINE )
					polyHipArray.Clear();
				delete TmpHip;
				subenthip = subenthip->next;
				continue;
			}

			// AG: Make a "temporary" copy of TmpEname 'cause ename parameter could be changed
			// inside SDS_xforment() (it gets changed to db_seqend in while processing of db_poliline)
			{
				/*D.G.*/// Enable snapping to ents on locked layers: temporally onlock layer.
				int				layerFlags = 0;
				db_handitem*	pLayer = reinterpret_cast<db_handitem*>(TmpEname[0])->ret_layerhip();

				if(pLayer)
				{
					pLayer->get_70(&layerFlags);
					pLayer->set_70(layerFlags & ~IC_LAYER_LOCKED);
				}

				sds_name tmpEntityName;
				ic_encpy( tmpEntityName, TmpEname );
				SDS_xforment( tmpEntityName, NULL, blkmtx );

				if(pLayer)
					pLayer->set_70(layerFlags);
			}

			// special handling for text drawn with a TrueType font
			// a similar approach should also work for images.
			if( (DB_TEXT == EntType || DB_ATTDEF == EntType || DB_ATTRIB == EntType) &&
				TrueTypeTextForGDI(TmpHip, view) )
			{
				sds_point corner1, corner2, corner3, corner4, corner5;
				GetGDITrueTypeTextBox(subenthip, corner1, corner2, corner3, corner4, corner5);	// Note, using subenthip, not TmpHip because of DispObjs.

				bool success;
				CParallelogram EntityExtents(THREEPOINTS_t, corner2, corner1, corner4, success);
				CRectangle PickWindow(ptAp1rp, ptAp2rp);
				if(PickWindow.intersects(EntityExtents))
					ToAdd = true;
			}
			else
			{
				db_handitem *telp = TmpHip;

				// check a layer
				LayerHip = TmpHip->ret_layerhip();
				if(LayerHip)
				{
					int flags;
					if(LayerHip->get_70(&flags) && (flags & IC_LAYER_FROZEN))
					{
						if( EntType == DB_POLYLINE )
							polyHipArray.Clear();
						delete TmpHip;
						subenthip = subenthip->next;
						continue;
					}
				}

				if(SDS_getdispobjs(SDS_CURDWG, &TmpHip, &DispObjBeg, &DispObjEnd, NULL, SDS_CURDWG, view, 1))
				{
					if( EntType == DB_POLYLINE )
						polyHipArray.Clear();
					delete TmpHip;
					subenthip = subenthip->next;
					continue;
				}

				// restore TmpHip value
				TmpHip = telp;
				TmpHip->set_disp((void *)DispObjBeg);

				if(gr_selected(TmpHip, sl))
				{
					// Must do a special case to handle attributes.
					if(EntType == DB_ATTRIB)
					{
						//db_handitem *telp = TmpHip;
						db_handitem *telp = subenthip;
						sds_name ename;
						for(; telp != NULL && telp->ret_type() == DB_ATTRIB; telp = SDS_CURDWG->entnext_noxref(telp))
							;
						if(telp == NULL)
							goto out;
						if(telp->ret_type() == DB_SEQEND)
							((db_seqend *)telp)->get_mainent(&telp);
						// DP: INSERT in the  BLOCK was loaded from file
						// TODO: change loading of BLOCKs so SEQEND pointer to INSERT object is valid
						if(telp == NULL)
							goto out;
						ename[0] = (long)telp;
						ename[1] = (long)SDS_CURDWG;
						if(RTNORM == sds_ssmemb(ename, ss))
							goto out;
					}
					ToAdd = true;
				}
				out: ;
			} // else

			if( !ToAdd )
			{
				if( EntType == DB_POLYLINE )
					polyHipArray.Clear();
				delete TmpHip;

				/*DG - 11.3.2002*/// We shouldn't use this insert's hip in view->previnsdata
				// after this point (e.g. if we drag some attrib not belonging to this insert).
				if(EntType == DB_INSERT && view->previnsdata)
					view->previnsdata->hip = NULL;
			}
			else
			{
				hipArray.Add( TmpHip );
				int count = hipArray.GetSize();
				if( count > 1 )
					hipArray[count-2]->next = TmpHip;

				if( EntType == DB_POLYLINE )
				{
					hipArray.Append( polyHipArray );
					polyHipArray.RemoveAll();	// just for safety!!!
				}

				sds_ssadd(TmpEname,ss,ss);
				sds_sslength( ss, &snapSSLength );
				if( snapSSLength > OSNAP_MAX_NUMBER || EntType == DB_ATTRIB)
					break;
			}

			subenthip = subenthip->next;

			/*D.G.*/// VERY important: it's a nested insert and it's not selected - with its attribs,
			// so pass them (if any).
			if(!ToAdd && EntType == DB_INSERT)
			{
				for( ; subenthip && subenthip->ret_type() != DB_SEQEND; subenthip = subenthip->next)
					;
			}
		}	// while(subenthip) - loop through all interesting subentities in insert

		SDS_IsFromOsnapToXref = false; // set this flag back to false to enable redrawing of entities being transformed

		//restore the saved value to restart the callbacks if needed.
		SDS_SetAUserCallback = Save_SDS_SetAUserCallback;

		localCriticalSection.Leave();
		undoctl.resval.rint |= IC_UNDOCTL_ON;	// and now turn Undo back on
		sds_setvar("UNDOCTL",&undoctl);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	} // end of while(sds_ssname(ss)) - loop through all the entities from the initial selection set

	// Now we should get an empty ss 'cause we've already moved all elements to the newSS
	sds_sslength( ss, &snapSSLength );
	ASSERT( snapSSLength == 0 );
	sds_ssfree( ss );
	sds_name_set( newSS, ss );


	// Clean Up
	if( sl )
		gr_freeselector( sl );
	if( rbTmp )
		sds_relrb( rbTmp );

	res = 1;
	return res;
}

//<alm>
int COsnapManagerImp::processSS
(
sds_point ptAp1rp, 
sds_point ptAp2rp,
int osmode, 
sds_name ss
)
{
	sds_name entName, entTmp;
	struct resbuf rb1, rb2, *rbTmp = NULL, *elist = NULL;
	sds_point p1, p2, p3, p4, p5, p6, p7, p9, ur, ll, p210;
	sds_real fr1 = 0, fr2 = 0;
	int nct, mct, closureflg, plcentroid, pl_omitflag, fi1 = 0;
	long vtxcnt, fl1 = 0, fl2 = 0;

	struct pface_edge *pface_beg,*pface_cur,*pface_tmp;
	pface_beg=pface_cur=pface_tmp=NULL;
	struct pt_bulg *ptlst=NULL;

	sds_point saveDragPoint;
	bool bSaveDragPoint = false;

	// This is a stub now. ToDo: check if it should be always NULL.
	sds_matrix *m_ecs2ucs = NULL;

	int res = RTNORM;
	
	long ssLen = 0;
	sds_sslength( ss, &ssLen );
	if( !ssLen )
		return RTERROR;

	struct gr_view *view=SDS_CURGRVW;
	if( !view )
		return RTERROR;

	sds_point ospt, ptPoint;
	gr_rp2ucs( m_curPointRP, ospt,view );
	sds_point_set( ospt, ptPoint );

	sds_real maxDistFromPickBoxCen = (sds_distance( ptAp1rp, ptAp2rp )) / 2;

	//<alm: enabling deferred snapping>
	if (SDS_OSNAP_TANPER_IGNORE==-1)
    {
	    //If we're snapping tangent or perpendicular in the LINE command, the
		//	osnap MAY depend upon a pt not yet selected.  The -1 flags this...
		if (osmode & IC_OSMODE_PERP)
    	{
    		osmode &= ~IC_OSMODE_PERP;
    	}
		if (osmode & IC_OSMODE_TAN)
    	{
    		osmode &= ~IC_OSMODE_TAN;
    	}
#if 0
	    if (osmode==0)
    	{
    		ic_ptcpy(ptPoint,ospt);
    		dsq=0.0;
    		ret=RTNORM;
    		goto exit_point;
    	}
#endif
    }
	//</alm>


/*	if( ssLen > 25 )
	{
		sds_printf (ResourceString(IDC_SDS_OSNAP_ERROR1, "\nToo many entities selected.  No valid snap was found."));
	}
*/
	while( sds_ssname( ss, 0L,entName ) == RTNORM )
	{
		// AG: restore the initial value of point of dragged object to perform snapping to unchanged object
		if(1 == SDS_AtNodeDrag && entName[0] == SDS_NodeDragEnt[0] && entName[1] == SDS_NodeDragEnt[1] &&
			SDS_CMainWindow && SDS_CMainWindow->m_pDragVarsCur && SDS_CMainWindow->m_pDragVarsCur->elpt )
		{
			ic_ptcpy( saveDragPoint, SDS_CMainWindow->m_pDragVarsCur->elpt );
			ic_ptcpy( SDS_CMainWindow->m_pDragVarsCur->elpt, SDS_CMainWindow->m_pDragVarsCur->pt2 );
			bSaveDragPoint = true;
		}

		db_handitem *hip = (db_handitem*)entName[0];
		int type = hip->ret_type();
		elist = sds_entget( entName );
		if( !elist )
			break;

		switch( type ) // main switch
		{
		case DB_INSERT:
		case DB_ATTRIB:
			// here we check only with INS mode for these types 
			if( osmode & IC_OSMODE_INS )
			{
				int count = 0;
				p2[0] = p2[1] = 0.0;
				p2[2] = 1.0;	// should be a default normal

				// Let's pick up the data
				for( rbTmp = elist; rbTmp; rbTmp = rbTmp->rbnext )
				{
					if( rbTmp->restype == 10 )
						ic_ptcpy( p1, rbTmp->resval.rpoint );
					else if( rbTmp->restype == 66 && type == DB_INSERT )
						count = rbTmp->resval.rint;
					else if( rbTmp->restype == 210 )
						ic_ptcpy( p2, rbTmp->resval.rpoint );
				}

				rb1.rbnext = rb2.rbnext = NULL;
				rb1.restype = RT3DPOINT;
				rb2.restype = RTSHORT;
				ic_ptcpy( rb1.resval.rpoint, p2 );
				rb2.resval.rint = 1;	//UCS

				sds_trans( p1, &rb1, &rb2, 0, p2 );
				gr_ucs2rp( p2, p3, view );
				osnap_helper_addPtRec( IC_OSMODE_INS, p2, p3, m_curPointRP, ptAp1rp, ptAp2rp );

				if( count )	// let's check the ATTRIB ???
				{
					for( ic_encpy( entTmp, entName ); sds_entnext_noxref( entTmp,entTmp ) == RTNORM; )
					{
						if( ((db_handitem*)entTmp[0])->ret_type() != DB_ATTRIB )
							break;
						if( (rbTmp = sds_entget( entTmp )) == NULL )
							break;
						osnap_helper_elist( rbTmp, IC_OSMODE_INS, m_curPointRP,(double *)ptPoint,ospt/*,&dsq*/,pl_omitflag,m_ecs2ucs, ptAp1rp, ptAp2rp );
						FreeResbufIfNotNull( &rbTmp );
					}
				}
			}
			break;

		case DB_POLYLINE:
			{
				//Break the pline down into individual vertices & check
				//the boundary of each.  If within crossing box, call
				//simple osnap routine after adding 11 group to elist.
				//NOTE: if it's not a 2D pline, change etype from VERTEX
				//to 3ERTEX
				fr2=p210[0]=p210[1]=0.0;
				p210[2]=1.0;
				mct=nct=0;

				ic_assoc(elist,70);
				closureflg=ic_rbassoc->resval.rint;
				if (!(closureflg & (IC_PLINE_3DPLINE | IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH)))
				{
					if (0==ic_assoc(elist,39))
						fr2=ic_rbassoc->resval.rreal;
					if (0==ic_assoc(elist,210))
						ic_ptcpy(p210,ic_rbassoc->resval.rpoint);
				}

				if (0==(closureflg & IC_PLINE_3DMESH))
					nct=1;
				else
				{
					mct=(!ic_assoc(elist,71)) ? ic_rbassoc->resval.rint : 0;
					nct=(!ic_assoc(elist,72)) ? ic_rbassoc->resval.rint : 0;
				}

				//if pline is closed and is not a mesh or polyface mesh,
				//	enable centroid flag.  Later, we'll check that all
				//	vertices have 0.0 bulge.  If so, we can snap to center
				if ((closureflg & (IC_PLINE_CLOSED | IC_PLINE_3DMESH | IC_PLINE_CLOSEDN | IC_PLINE_POLYFACEMESH))==IC_PLINE_CLOSED)
					plcentroid=1;
				else plcentroid=0;
				//step thru to see how many points we have.
				vtxcnt=0;
				ic_encpy(entTmp,entName);
				while(sds_entnext_noxref(entTmp,entTmp)==RTNORM)
				{
					FreeResbufIfNotNull(&elist);
					if ((elist=sds_entget(entTmp))==NULL)
					{
						res=RTERROR;
						goto exit_point;
					}
					ic_assoc(elist,0);
					if (!strsame(ic_rbassoc->resval.rstring,db_hitype2str(DB_VERTEX)))
						break;
					if (closureflg & IC_PLINE_SPLINEFIT)	//if it's been splined or fitted...
					{
						ic_assoc(elist,70);
						//if it isn't a splined vertex, continue
						if (0==(ic_rbassoc->resval.rint & IC_VERTEX_SPLINEVERTEX))
							continue;
					}
					else if (closureflg & IC_PLINE_POLYFACEMESH)//pface
					{
						ic_assoc(elist,70);//don't count pface mesh def vertices
						if (0==(ic_rbassoc->resval.rint & IC_VERTEX_3DMESHVERT))
						{
							int iEdge[4];
							for (rbTmp=elist;rbTmp!=NULL;rbTmp=rbTmp->rbnext)
							{
								//walk the vertex and get the vertices to build this face
								if (rbTmp->restype==71)
									iEdge[0]=rbTmp->resval.rint;
								else if (rbTmp->restype==72)
									iEdge[1]=rbTmp->resval.rint;
								else if (rbTmp->restype==73)
									iEdge[2]=rbTmp->resval.rint;
								else if (rbTmp->restype==74)
									iEdge[3]=rbTmp->resval.rint;
							}
							//now that we have the vertices, define the visible edges
							for (fi1=0;fi1<4;fi1++)
							{
								if (iEdge[fi1]==0)
									break;
								if (iEdge[fi1]<0)
									continue;
								if (NULL==(pface_tmp= new pface_edge ))
								{
									cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
									res = RTERROR;
									goto exit_point;
								}
								memset(pface_tmp,0,sizeof(struct pface_edge));
								if (pface_cur==NULL)
									pface_cur=pface_beg=pface_tmp;
								else
								{
									pface_cur->next=pface_tmp;
									pface_cur=pface_cur->next;
								}
								pface_cur->vtx[0]=iEdge[fi1]-1;//pface counter 1-based, we'll make a 0-based array
								//if end of array or next vtx is 0, close back to beginning
								if (fi1==3 || 0==iEdge[fi1+1])
									pface_cur->vtx[1]=abs(iEdge[0])-1;
								else
									pface_cur->vtx[1]=abs(iEdge[fi1+1])-1;//else go to next vtx
							}
							continue;//don't increment vtxcnt
						}
					}
					vtxcnt++;
				}
				// If only one pline and it has 2 vertices, it can't self intersect.
				if (fl1 == 1 && vtxcnt == 2)
					osmode &= ~IC_OSMODE_INT;

				if ((closureflg & IC_PLINE_3DMESH) && vtxcnt!=mct*nct)
				{
					res = RTERROR;
					goto exit_point;
				}
				else if (closureflg & IC_PLINE_POLYFACEMESH)
					mct=vtxcnt;

				//make sure ptlst is free
           		if (ptlst!=NULL)
				{
					delete ptlst;
					ptlst=NULL;
				}

				//allocate array of points
				if ((ptlst= new struct pt_bulg [vtxcnt])==NULL)
				{
					cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
					res = RTERROR;
					goto exit_point;
				}
				memset(ptlst,0,sizeof(struct pt_bulg));
				ic_encpy(entTmp,entName);
				//populate array of points
				for (fl1=0;fl1<vtxcnt;fl1++)
				{
					if ((res = sds_entnext_noxref(entTmp,entTmp))!=RTNORM)
						goto exit_point;
					FreeResbufIfNotNull(&elist);
					if ((elist=sds_entget(entTmp))==NULL)
					{
						res = RTERROR;
						goto exit_point;
					}
					if (closureflg & IC_PLINE_SPLINEFIT)	//if it's been splined or fitted...
					{
						ic_assoc(elist,70);
						//if it isn't a splined vertex, continue
						if (0==(ic_rbassoc->resval.rint & IC_VERTEX_SPLINEVERTEX))
						{
							fl1--;
							continue;
						}
					}
					else if (closureflg & IC_PLINE_POLYFACEMESH)
					{
						ic_assoc(elist,70);
						//if its a pface definition vertex, continue
						if (0==(ic_rbassoc->resval.rint & IC_VERTEX_3DMESHVERT))
						{
							fl1--;
							continue;
						}
					}
					ic_assoc(elist,10);
					ic_ptcpy(ptlst[fl1].pt,ic_rbassoc->resval.rpoint);
					ic_assoc(elist,42);
					ptlst[fl1].bulge=ic_rbassoc->resval.rreal;
				}

				fr1=0.0;  // give it a value

				//before looping, allocate llist for call to pts checker
				FreeResbufIfNotNull(&elist);
				p7[0]=p7[1]=p7[2]=0.0;  // init this
				if (closureflg & (IC_PLINE_3DPLINE | IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH)) 
					elist=sds_buildlist(-1,entName,RTDXF0,"3ERTEX"/*DNT*/,10,p7,11,p7,42,fr1,0);
				else
					elist=sds_buildlist(-1,entName,RTDXF0,db_hitype2str(DB_VERTEX),10,p7,11,p7,42,fr1,39,fr2,210,p210,0);
				if (elist==NULL)
				{
					cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
					res= RTERROR;
					goto exit_point;
				}
				for (fi1=0;fi1<2;fi1++)
				{
					//check across vertices in M-direction, then N-direction
					//if needed
					if ((fi1==1)&&((closureflg & IC_PLINE_3DMESH)==0))//if it's not a mesh, just make one pass
						break;
					for (fl1=0;;fl1++)
					{
						pl_omitflag=0;
						//at this pt, we need to determine which vertices we need to check across.
						//	for pfaces, we need to determine the order by readint the ent
						if (closureflg & IC_PLINE_POLYFACEMESH)
						{
							//pface
							if (fl1==0)
								pface_cur=pface_beg;
							else
								pface_cur=pface_cur->next;
							if (pface_cur==NULL)
								break;
							fl1=pface_cur->vtx[0];
							fl2=pface_cur->vtx[1];
						}
						else
						{
							//non-pface - determine which vertices to use
							if (fi1==0)	//going in "standard" direction along pline
							{
								if (fl1>=vtxcnt)
									break;
								fl2=fl1+1;
								if ((closureflg & IC_PLINE_3DMESH)==0)	//Non-mesh control
								{
									if (fl1==vtxcnt-1)
									{
										if (!(closureflg&1))
											break;
										else
											fl2=0;
									}
									if (0==fl1 && 0==(closureflg & IC_PLINE_CLOSED))
										pl_omitflag=1;//don't check 10pt as "INT"
									if (fl1==(vtxcnt-2) && 0==(closureflg & IC_PLINE_CLOSED))
										pl_omitflag=2;
								}
								else				 	//Control for 3D mesh
								{
									//N direction closure or edge
									if ((fl2%nct)==0)
									{
										if (closureflg & IC_PLINE_CLOSEDN)
											fl2-=nct;
										else
											continue;
									}
								}
							}
							else		//going in M direction "across" pline
							{
								if (fl1>=vtxcnt)
									break;
								fl2=fl1+nct;
								if (fl1>=(mct*nct-nct)) //M-direction edge condition
								{
									if (closureflg & IC_PLINE_CLOSED)
										//M-direction closure
										fl2=(fl1%nct);
									else
										break;
								}
							}
						}
						if (vtxcnt>47)
						{
							//if it's a huge pline, try weeding out segments not near picked point
							if (fl1==0L)
							{
								//setup resbufs to transform pline's points from ecs/wcs into wcs
 								if (m_ecs2ucs!=NULL)
								{
									//pline nested in block
									rb1.restype=rb2.restype=RTSHORT;
									rb1.rbnext=rb2.rbnext=NULL;
									rb1.resval.rint=1;	//ucs
									rb2.resval.rint=0;	//wcs
								}
								else if (fl1==0 && (closureflg & (IC_PLINE_3DPLINE | IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH)))
								{
									rb1.restype=rb2.restype=RTSHORT;
									rb1.rbnext=rb2.rbnext=NULL;
									rb1.resval.rint=0;	//wcs
									rb2.resval.rint=0;	//wcs
								}
								else if (fl1==0)
								{
									rb1.rbnext=rb2.rbnext=NULL;
									rb1.restype=RTENAME;
									ic_encpy(rb1.resval.rlname,entName); //ecs
									rb2.restype=RTSHORT;
									rb2.resval.rint=0;	//wcs
								}
							}
							//see if this vertex might cross pickbox
							fr1=ptlst[fl1].bulge;
							if ((fi1==1)||(icadRealEqual(fr1,0.0)))//straight segment
							{
								ic_ptcpy(p3,ptlst[fl1].pt);
								ic_ptcpy(p4,ptlst[fl2].pt);
								ic_ptcpy(p5,p3);
								ic_ptcpy(p6,p4);
							}
							else
							{
								if (plcentroid)
									plcentroid=0;	//if seg is curved, cancel centroid calc
								ic_bulge2arc(ptlst[fl1].pt,ptlst[fl2].pt,fr1,p3,&p4[0],&p4[1],&p4[2]);
								//note: don't overwrite ptlst, because we check pline meshes transversely
								ic_arcextent(p3,p4[0],p4[1],p4[2],p5,p6);
								p5[2]=p6[2]=p3[2];
							}

							//p3 & p4 are points to use for checking if segment within boundary
							//trans p5 and p6 bounding box for segment into ucs coords
							if (NULL!=m_ecs2ucs)
							{
								ic_usemat(p5,p5,m_ecs2ucs,0,0,0);
								ic_usemat(p6,p6,m_ecs2ucs,0,0,0);//pts are now ucs
								sds_trans(p5,&rb1,&rb2,0,p5);
								sds_trans(p6,&rb1,&rb2,0,p6);//pts are now wcs
								if (fi1==0 && fl1==0 && !icadRealEqual(fr2,0.0))
								{
									ic_usemat(p210,p210,m_ecs2ucs,1,0,0);
									sds_trans(p210,&rb1,&rb2,1,p210);
								}
							}
							else
							{
								sds_trans(p5,&rb1,&rb2,0,p5);
								sds_trans(p6,&rb1,&rb2,0,p6);//pts are now wcs
								if (fi1==0 && fl1==0 && !icadRealEqual(fr2,0.0))
									sds_trans(p210,&rb1,&rb2,1,p210);
							}
							SDS_rpextents(p5,p6,ll,ur,NULL);
							if (!icadRealEqual(fr2,0.0))
							{
								p9[0]=p210[0]*fr2;
								p9[1]=p210[1]*fr2;
								p9[2]=p210[2]*fr2;
								//rb2 is always a wcs resbuf.  steal it & use as dcs
								rbTmp=sds_buildlist(RTSHORT,2,0);
								sds_trans(p9,&rb2,rbTmp,1,p9);
								FreeResbufIfNotNull(&rbTmp);
								p9[0]=fabs(p9[0]);p9[1]=fabs(p9[1]);
								ll[0]-=p9[0];
								ur[0]+=p9[0];
								ll[1]-=p9[1];
								ur[1]+=p9[1];
							}
							if ((ll[0]>ptAp2rp[0])||	   //if segment out of bounds for
								(ll[1]>ptAp2rp[1])||	   //orig ssget crossing box, continue
								(ur[0]<ptAp1rp[0])||
								(ur[1]<ptAp1rp[1]))
								continue;
						}
						else
						{
							//pline is small, so don't weed out vertices - just check 'em all
							fr1=ptlst[fl1].bulge;
							if ((fi1==1)||(icadRealEqual(fr1,0.0)))//straight segment
							{
								ic_ptcpy(p3,ptlst[fl1].pt);
								ic_ptcpy(p4,ptlst[fl2].pt);
							}
							else
							{
								if (plcentroid)
									plcentroid=0;	//if seg is curved, cancel centroid calc
								ic_bulge2arc(ptlst[fl1].pt,ptlst[fl2].pt,fr1,p3,&p4[0],&p4[1],&p4[2]);
								//note: don't overwrite ptlst, because we check pline meshes transversely
							}
						}
						ic_assoc(elist,10);
						ic_ptcpy(ic_rbassoc->resval.rpoint,p3);
						ic_assoc(elist,11);
						ic_ptcpy(ic_rbassoc->resval.rpoint,p4);
						ic_assoc(elist,42);
						ic_rbassoc->resval.rreal=fr1;
						osnap_helper_elist(elist,osmode,m_curPointRP,(double *)ptPoint,ospt/*,&dsq*/,pl_omitflag,m_ecs2ucs, ptAp1rp, ptAp2rp );
						// KLUGE alert!
						// If the osnap is intersection and the number of entities in the selection set passed
						// to this function is one. We have to do some special handling. What we will do is see
						// if the pickpoint and its pickbox are over a vertex of the pline. If so we will return
						// that point instead of what the call to SDS_osnap_elist() has returned. Bug 1-46576.
						// This is kind of late in the shipping game, so this compromise was reached. We still do
						// not handle self-intersecting plines. If we don't find a vertex that will work, then we
						// return RTNONE and the user is re-prompted for a point.
						if (osmode == IC_OSMODE_INT && ssLen == 1)	/*D.Gavrilov*/// We should compare with INT
                        {										// using == (not with &).
							bool found = FALSE;
							if (sds_distance(m_curPointRP,ospt) > maxDistFromPickBoxCen)
                            {
								// Note 1 and vtxcnt-1, eliminates the start and end points
								for (int i = 1; i < vtxcnt-1; i++)
                                {
									if (sds_distance(m_curPointRP,ptlst[i].pt) < maxDistFromPickBoxCen)
                                    {
										ic_ptcpy(ospt, ptlst[fl1].pt);
										found = TRUE;
										res=RTNORM;
										break;
                                    }
                                }
                            }
							else
								found = TRUE;

							if (!found)
                            {
								res=RTNONE;
								goto exit_point;
                            }
                        }
					}
				}//for loop N direction
				sds_ssname(ss,0L,entName);
				if (plcentroid && IC_PLINE_CLOSED==(closureflg&(IC_PLINE_CLOSED| IC_PLINE_3DPLINE | IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH)) && (osmode & IC_OSMODE_CENTER))
				{
					//if CEN enabled and pline of correct type
					p5[0]=p5[1]=p5[2]=0.0;
					for (fl1=0;fl1<vtxcnt;fl1++)
					{
						p5[0]+=ptlst[fl1].pt[0];
						p5[1]+=ptlst[fl1].pt[1];
						p5[2]+=ptlst[fl1].pt[2];
					}
					p5[0]/=vtxcnt;
					p5[1]/=vtxcnt;
					p5[2]/=vtxcnt;
					//convert from ECS to UCS
					if (NULL!=m_ecs2ucs)
					{
						ic_usemat(p5,p6,m_ecs2ucs,0,0,0);
					}
					else
					{
						rb1.restype=RTSHORT;
						rb1.resval.rint=1;
						rb2.restype=RTENAME;
						ic_encpy(rb2.resval.rlname,entName);
						sds_trans(p5,&rb2,&rb1,0,p6);
					}
					gr_ucs2rp(p6,p7,view);
//**					if (SDS_osnap_chkdsq(pickptrp,p7,ospt,&dsq))
//**						ic_ptcpy(ospt,p6);
					osnap_helper_addPtRec( IC_OSMODE_CENTER, p6, p7, m_curPointRP, ptAp1rp, ptAp2rp );
				}
			} // end of POLYLINE code
			break;

		case DB_LWPOLYLINE:
			{
				//Break the pline down into individual vertices & check
				//the boundary of each.  If within crossing box, call
				//simple osnap routine after adding 11 group to elist.
				//NOTE: if it's not a 2D pline, change etype from VERTEX
				//to 3ERTEX
				fi1 = 0;
				db_lwpolyline *lwpl=(db_lwpolyline *)entName[0];
				fr2=p210[0]=p210[1]=0.0;
				p210[2]=1.0;
				mct=nct=0;

				ic_assoc(elist,70);
				closureflg=ic_rbassoc->resval.rint;
				if (0==ic_assoc(elist,39))
					fr2=ic_rbassoc->resval.rreal;
				if (0==ic_assoc(elist,210))
					ic_ptcpy(p210,ic_rbassoc->resval.rpoint);

				nct=1;

				//if pline is closed and is not a mesh or polyface mesh,
				//	enable centroid flag.  Later, we'll check that all
				//	vertices have 0.0 bulge.  If so, we can snap to center
				if (closureflg & IC_PLINE_CLOSED)
					plcentroid=1;
				else
					plcentroid=0;
				//step thru to see how many points we have.
				vtxcnt=lwpl->ret_90();
				ic_encpy(entTmp,entName);
				// If only one pline and it has 2 vertices, it can't self intersect.
				if (fl1 == 1 && vtxcnt == 2)
	                osmode &= ~IC_OSMODE_INT;

				fr1=0.0;  // give it a value
			
				// build a list to send to the existing points checker
				FreeResbufIfNotNull(&elist);
				p7[0]=p7[1]=p7[2]=0.0;  // init this
				elist=sds_buildlist(-1,entName,RTDXF0,db_hitype2str(DB_VERTEX),10,p7,11,p7,42,fr1,39,fr2,210,p210,0);
				if (elist==NULL)
				{
					cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
					res=RTERROR;
					goto exit_point;
				}
				for (fl1=0;;fl1++)
				{
					pl_omitflag=0;
					if (fl1>=vtxcnt)
						break;
					fl2=fl1+1;
					if (fl1==vtxcnt-1)
					{
						if (!(closureflg&1))
							break;
						else
							fl2=0;
					}
					if (fl1==0 && !(closureflg & IC_PLINE_CLOSED))
						pl_omitflag=1;//don't check 10pt as "INT"
					if (fl1==(vtxcnt-2) && !(closureflg & IC_PLINE_CLOSED))
						pl_omitflag=2;

					if (vtxcnt>47)
					{
						//if it's a huge pline, try weeding out segments not near picked point
						if (fl1==0L)
						{
							//setup resbufs to transform pline's points from ecs/wcs into wcs
							if (m_ecs2ucs!=NULL)
							{
								//pline nested in block
								rb1.restype=rb2.restype=RTSHORT;
								rb1.rbnext=rb2.rbnext=NULL;
								rb1.resval.rint=1;	//ucs
								rb2.resval.rint=0;	//wcs
							}
							else if (fl1==0)
							{
								rb1.rbnext=rb2.rbnext=NULL;
								rb1.restype=RTENAME;
								ic_encpy(rb1.resval.rlname,entName); //ecs
								rb2.restype=RTSHORT;
								rb2.resval.rint=0;	//wcs
							}
						}
						//see if this vertex might cross pickbox
						lwpl->get_42(&fr1,fl1);
						if (fi1==1 || icadRealEqual(fr1,0.0))//straight segment
						{
							lwpl->get_10(p3,fl1);
							lwpl->get_10(p4,fl2);
							ic_ptcpy(p5,p3);
							ic_ptcpy(p6,p4);
						}
						else
						{
							if (plcentroid)
								plcentroid=0;	//if seg is curved, cancel centroid calc
							sds_point temppt1,temppt2;
							lwpl->get_10(temppt1,fl1);
							lwpl->get_10(temppt2,fl2);
							ic_bulge2arc(temppt1,temppt2,fr1,p3,&p4[0],&p4[1],&p4[2]);
							ic_arcextent(p3,p4[0],p4[1],p4[2],p5,p6);
							p5[2]=p6[2]=p3[2];
						}

						//p3 & p4 are points to use for checking if segment within boundary
						//trans p5 and p6 bounding box for segment into ucs coords
						if (NULL!=m_ecs2ucs)
						{
							ic_usemat(p5,p5,m_ecs2ucs,0,0,0);
							ic_usemat(p6,p6,m_ecs2ucs,0,0,0);//pts are now ucs
							sds_trans(p5,&rb1,&rb2,0,p5);
							sds_trans(p6,&rb1,&rb2,0,p6);//pts are now wcs
							if (fi1==0 && fl1==0 && !icadRealEqual(fr2,0.0))
							{
								ic_usemat(p210,p210,m_ecs2ucs,1,0,0);
								sds_trans(p210,&rb1,&rb2,1,p210);
							}
						}
						else
						{
							sds_trans(p5,&rb1,&rb2,0,p5);
							sds_trans(p6,&rb1,&rb2,0,p6);//pts are now wcs
							if (fi1==0 && fl1==0 && !icadRealEqual(fr2,0.0))
								sds_trans(p210,&rb1,&rb2,1,p210);
						}
						SDS_rpextents(p5,p6,ll,ur,NULL);
						if (!icadRealEqual(fr2,0.0))
						{
							p9[0]=p210[0]*fr2;
							p9[1]=p210[1]*fr2;
							p9[2]=p210[2]*fr2;
							//rb2 is always a wcs resbuf.  steal it & use as dcs
							rbTmp=sds_buildlist(RTSHORT,2,0);
							sds_trans(p9,&rb2,rbTmp,1,p9);
							FreeResbufIfNotNull(&rbTmp);
							p9[0]=fabs(p9[0]);p9[1]=fabs(p9[1]);
							ll[0]-=p9[0];
							ur[0]+=p9[0];
							ll[1]-=p9[1];
							ur[1]+=p9[1];
						}
						if ((ll[0]>ptAp2rp[0])||	   //if segment out of bounds for
							(ll[1]>ptAp2rp[1])||	   //orig ssget crossing box, continue
							(ur[0]<ptAp1rp[0])||
							(ur[1]<ptAp1rp[1])) 
							continue;
					}
					else
					{
						//pline is small, so don't weed out vertices - just check 'em all
						lwpl->get_42(&fr1,fl1);
						if (fi1==1 || icadRealEqual(fr1,0.0))//straight segment
						{
							lwpl->get_10(p3,fl1);
							lwpl->get_10(p4,fl2);
						}
						else
						{
							if (plcentroid)
								plcentroid=0;	//if seg is curved, cancel centroid calc
							sds_point temppt1,temppt2;
							lwpl->get_10(temppt1,fl1);
							lwpl->get_10(temppt2,fl2);
							ic_bulge2arc(temppt1,temppt2,fr1,p3,&p4[0],&p4[1],&p4[2]);
						}
					}
					ic_assoc(elist,10);
					ic_ptcpy(ic_rbassoc->resval.rpoint,p3);
					ic_assoc(elist,11);
					ic_ptcpy(ic_rbassoc->resval.rpoint,p4);
					ic_assoc(elist,42);
					ic_rbassoc->resval.rreal=fr1;
 					// Modified PK 28/06/2000 
 					//SDS_osnap_elist(elist,osmode,pickptrp,(double *)ptPoint,ospt,&dsq,pl_omitflag,m_ecs2ucs);
  					osnap_helper_elist(elist,osmode,m_curPointRP,(double *)ptPoint,ospt/*,&dsq*/,pl_omitflag,m_ecs2ucs, ptAp1rp, ptAp2rp ); // Reason: Fix for bug no.47837 
					// KLUGE alert!
					// If the osnap is intersection and the number of entities in the selection set passed
					// to this function is one. We have to do some special handling. What we will do is see
					// if the pickpoint and its pickbox are over a vertex of the pline. If so we will return
					// that point instead of what the call to SDS_osnap_elist() has returned. Bug 1-46576.
					// This is kind of late in the shipping game, so this compromise was reached. We still do
					// not handle self-intersecting plines. If we don't find a vertex that will work, then we
					// return RTNONE and the user is re-prompted for a point.
					if((osmode & (IC_OSMODE_INT | IC_OSMODE_PLAN)) && ssLen == 1)		/*D.G.*/// Use SSLen instead of fl1.
					{
						bool found = FALSE;						
						sds_point *points = new sds_point [vtxcnt];
						lwpl->get_10(points, vtxcnt);
                    
	                    if (sds_distance(m_curPointRP,ospt) > maxDistFromPickBoxCen)
		                {
			                // Note 1 and vtxcnt-1, eliminates the start and end points
				            for (int i = 1; i < vtxcnt-1; i++)
					        {
						        if (sds_distance(m_curPointRP,points[i]) < maxDistFromPickBoxCen)
							    {
									ic_ptcpy(ospt, points[i]);
									found = TRUE;
									res=RTNORM;
									break;
								}
							}
						}
						else
							found = TRUE;

						IC_FREE(points);	/*D.G.*/

	                    if (!found)
                        {	/*D.G.*/// Try to intersect polyline with itself.
							sds_name ss2={0,0};
							sds_ssadd(entName, NULL, ss2);
							rb1.rbnext = rb2.rbnext = NULL;
							rb1.restype = rb2.restype = RTSHORT;
							rb1.resval.rint = 0;  rb2.resval.rint = 1;

							/*DG - 31.10.2001*/// Added interOption & bExtendAnyway parameters for ic_ssxss.
							if (ic_ssxss2(ss, ss2, IC_ZRO, ptPoint, &elist, 1, NULL, NULL, eExtendBoth, false) == 0)
							{
								for(rbTmp = elist; rbTmp != NULL; rbTmp = rbTmp->rbnext)
								{
									//transform point into ucs coords
									sds_trans(rbTmp->resval.rpoint, &rb1, &rb2, 0, rbTmp->resval.rpoint);
									gr_ucs2rp(rbTmp->resval.rpoint, p5, view);
//**									if(SDS_osnap_chkdsq(pickptrp, p5, ospt, &dsq))
//**										ic_ptcpy(ospt, rbTmp->resval.rpoint);
									// The following lines is to set the only exact osmode to the record
									int tmpOsmode = osmode & IC_OSMODE_INT;
									if( !tmpOsmode )
										osmode = IC_OSMODE_PLAN;
									osnap_helper_addPtRec( tmpOsmode, rbTmp->resval.rpoint, p5, m_curPointRP, ptAp1rp, ptAp2rp );

								}
								res = RTNORM;
								break;
							}
							else
							{
//**								res = RTNONE;
//**								goto exit_point;
								break;
							}

//						ret=RTNONE;
//						goto exit_point;
                        }
                    }
				}
				sds_ssname(ss,0L,entName);
				if (plcentroid && (closureflg & IC_PLINE_CLOSED) && (osmode & IC_OSMODE_CENTER))
				{
					//if CEN enabled and pline of correct type
					p5[0]=p5[1]=p5[2]=0.0;
					for (fl1=0;fl1<vtxcnt;fl1++)
					{
						sds_point temppt;
						lwpl->get_10(temppt,fl1);
						p5[0]+=temppt[0];
						p5[1]+=temppt[1];
						p5[2]+=temppt[2];
					}
					p5[0]/=vtxcnt;
					p5[1]/=vtxcnt;
					p5[2]/=vtxcnt;
					//convert from ECS to UCS
					if (NULL!=m_ecs2ucs)
						ic_usemat(p5,p6,m_ecs2ucs,0,0,0);
					else
					{
						rb1.restype=RTSHORT;
						rb1.resval.rint=1;
						rb2.restype=RTENAME;
						ic_encpy(rb2.resval.rlname,entName);
						sds_trans(p5,&rb2,&rb1,0,p6);
					}
					gr_ucs2rp(p6,p7,view);
//**					if (SDS_osnap_chkdsq(pickptrp,p7,ospt,&dsq))
//**						ic_ptcpy(ospt,p6);
					osnap_helper_addPtRec( IC_OSMODE_CENTER, p6, p7, m_curPointRP, ptAp1rp, ptAp2rp );
				}			
			} // end of LWPOLYLINE code
			break;

		default:
			osnap_helper_elist( elist, osmode, m_curPointRP, (double*)ptPoint, ospt/*, &dsq*/, pl_omitflag, m_ecs2ucs, ptAp1rp, ptAp2rp );
			break;
		} // end of main switch

		{//<alm>
		// Process custom modes now
		sds_point ptucs, ptAp1ucs, ptAp2ucs;
		gr_rp2ucs(m_curPointRP, ptucs, view);
		gr_rp2ucs(ptAp1rp, ptAp1ucs, view);
		gr_rp2ucs(ptAp2rp, ptAp2ucs, view);

		db_handitem *hip = (db_handitem*)entName[0];
		resbuf* el = sds_entget(entName);

		for (int i = s_stdModeArraySize, n = m_modeArray.GetSize(); i < n; i++)
		{
			//<alm: check additional custom condition>
			if (!m_modeArray[i].m_pMode->customCondition() &&
				!m_modeArray[i].m_bIsActive)
				continue;

			COsnapMode *pMode = m_modeArray[i].m_pMode;
			ASSERT( pMode );

			sds_resbuf *pPointList = NULL;
			if (pMode->getOsnapPoints(el, ptucs, ptAp1ucs, ptAp2ucs, &pPointList))
			{
				// add those points into the record array
				for (sds_resbuf *tmpRb = pPointList; tmpRb; tmpRb = tmpRb->rbnext)
				{
					if (tmpRb->restype != RT3DPOINT)
						continue;

					sds_point ptRp;
					gr_ucs2rp(tmpRb->resval.rpoint, ptRp, m_pView->m_pCurDwgView);
					addOsnapRecord(pMode, tmpRb->resval.rpoint, ptRp, m_curPointRP, ptAp1rp, ptAp2rp);
				}
			}
			// free these array funally
			sds_relrb( pPointList );
		}

		FreeResbufIfNotNull(&el);
		}//</alm>

		FreeResbufIfNotNull( &elist );
		elist = NULL;
		if( ptlst != NULL )
		{
			IC_FREE( ptlst );
			ptlst = NULL;
		}
		if( pface_beg )
		{
			freePFaceEdgeList( pface_beg );
			pface_beg = NULL;
		}

		sds_ssdel( entName, ss );	// remove processed entity from ss
		sds_sslength( ss, &ssLen );

		// Now let's check for intersections
		if( ssLen > 0 ) // there're some entities left in ss
		{
			if( osmode & (IC_OSMODE_PLAN | IC_OSMODE_APP | IC_OSMODE_INT) )
			{
				sds_name ss2;
				sds_name_nil( ss2 );
				int threeDmode = ( (osmode & IC_OSMODE_INT) ? 1 : 0 );
				sds_ssadd( entName, 0, ss2 );	// add entName to ss2 to process intersections with ss
				/*DG - 31.10.2001*/// Added interOption & bExtendAnyway parameters for ic_ssxss.
				if (ic_ssxss2(ss, ss2, IC_ZRO, ptPoint, &elist, threeDmode, NULL, NULL, eExtendBoth, false) == 0)
				{
					rb1.rbnext = rb2.rbnext = NULL;
					rb1.restype = rb2.restype = RTSHORT;
					rb1.resval.rint = 0;
					rb2.resval.rint = 1;	//UCS

					for( rbTmp = elist; rbTmp; rbTmp = rbTmp->rbnext )
					{
						sds_trans( rbTmp->resval.rpoint, &rb1, &rb2, 0, p1 );
						gr_ucs2rp( p1, p2, view );

						int tmpOsmode = osmode & IC_OSMODE_INT;
						if( !tmpOsmode )
							tmpOsmode = osmode & IC_OSMODE_PLAN;
						if( !tmpOsmode )
							tmpOsmode = IC_OSMODE_APP;
						osnap_helper_addPtRec( tmpOsmode, p1, p2, m_curPointRP, ptAp1rp, ptAp2rp );
					}
				}

				FreeResbufIfNotNull( &elist );
				sds_ssfree( ss2 );
			}
		}

		// AG: restore dragpoint
		if( bSaveDragPoint )
		{
			bSaveDragPoint = false;
			ic_ptcpy( SDS_CMainWindow->m_pDragVarsCur->elpt, saveDragPoint );
		}

	} // end of while() loop

exit_point:

	if( bSaveDragPoint )
		ic_ptcpy( SDS_CMainWindow->m_pDragVarsCur->elpt, saveDragPoint );
	FreeResbufIfNotNull( &elist );
	if( ptlst != NULL )
		IC_FREE( ptlst );
	freePFaceEdgeList( pface_beg );

	return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////

int osnap_helper_elist
(
struct resbuf *elist, 
int osmode, 
sds_point pickptrp, 
sds_point pickptucs, 
sds_point ospt/*, sds_real *dsq*/,
int pl_omitflag,
sds_matrix *m_ecs2ucs, 
sds_point ptAp1rp, 
sds_point ptAp2rp
)
{
	//NOTE: pickpt2d is a 2D point in RP coordinates!
	//NOTE: 2d and 3d pline vertices have an 11 group for endpoint, and 3d vertices passed
	//			with 0 group changed to 3ERTEX
	//NOTE: for some entities (SOLID, POLYLINE, TRACE, 3DFACE), INT is checked for
	//			intersections within entity itself. If pl_omitflag==1, don't check 10-pt
	//			on vtx for INT, if pl_omitflag==2, don't check 11pt
	//for ents w/thickness, MID and NEA are valid for "edges" which connect
	//	ends of entity base and extruded level

	sds_point osptrp, ucsPt,lastpt,p1,p2,p3,p4,p5,p6,p7,p11={0.0,0.0,0.0},p12={0.0,0.0,0.0},p13,p14,p15,p16={0.0,0.0,0.0},p17,p210;
	sds_real fr1,fr2,fr3,fr4,fr5,thickness;
	struct resbuf rb, *rbtemp, rbwcs, rbucs, rbent;
	int ret=RTNORM,fi1,fi2,thickflg;
	char etype[IC_ACADNMLEN];

	struct gr_view *view=SDS_CURGRVW;
	if (view==NULL)
		return(RTERROR);
	rbucs.restype=rbwcs.restype=RTSHORT;
	rbent.restype=RTENAME;
	rbucs.rbnext=rbwcs.rbnext=rbent.rbnext=NULL;
	rbucs.resval.rint=1;rbwcs.resval.rint=0;

	int tmpOsmode = 0;

	//NOTES ON VERTEX ENTS -
		//for a VERTEX, the entity is "doctored" so it's name is 3ERTEX if
		//the vtx is from a 3D poly or mesh.  Its 10 group is the center of
		//the arc if bulge !=0.0.  if bulge==0.0, 10 group not altered
		//It also has an 11 group to designate the endpoint of the
		//straight segment or the rr, sa, &ea of the arc
		//Essentially, it's in ic_segdist format!


	gr_ucs2rp(ospt,osptrp,view);

	if (SDS_getvar(NULL,DB_QLASTPOINT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
	{
		ret=RTERROR;
		goto exit_point;
	}
	ic_ptcpy(lastpt,rb.resval.rpoint);//this is a UCS point
	ic_assoc(elist,0);
	strncpy(etype,ic_rbassoc->resval.rstring,sizeof(etype)-1);
    thickness=0.0;
	p210[0]=p210[1]=0.0;
	p210[2]=1.0;
    thickflg=0;
    if (0==ic_assoc(elist,39))
		{
        thickness=ic_rbassoc->resval.rreal;
        if (!icadRealEqual(thickness,0.0))
			thickflg=1;
		if (0==ic_assoc(elist,210))
			ic_ptcpy(p210,ic_rbassoc->resval.rpoint);
		}

	//**************************************************************************
	//*********** BEGIN CHECKING INDIVIDUAL ENTITIES for OSNAPS ****************
	//**************************************************************************
	if (strsame(etype,"3ERTEX"/*DNT*/))  //3D poly in wcs coordinates!
		{
		for (rbtemp=elist;rbtemp!=NULL;rbtemp=rbtemp->rbnext)
			{
			if (rbtemp->restype==10)
				ic_ptcpy(p1,rbtemp->resval.rpoint);
			else if (rbtemp->restype==11)
				ic_ptcpy(p2,rbtemp->resval.rpoint);
			else if (rbtemp->restype==42)
				fr1=rbtemp->resval.rreal;
			}
        
		rbtemp=NULL;
		if (!icadRealEqual(fr1,0.0))
			goto exit_point;
		//note: 3d vertex cannot have thickness!
            
		if (m_ecs2ucs)
			{
			ic_usemat(p1,p1,m_ecs2ucs,0,0,0);
			ic_usemat(p2,p2,m_ecs2ucs,0,0,0);
			}
		else
			{
			sds_trans(p1,&rbwcs,&rbucs,0,p1);
			sds_trans(p2,&rbwcs,&rbucs,0,p2);
			}
		gr_ucs2rp(p1,p3,view);
		gr_ucs2rp(p2,p4,view);

		if (osmode &(IC_OSMODE_END | IC_OSMODE_INT | IC_OSMODE_PLAN | IC_OSMODE_APP))
			{
			tmpOsmode = osmode & IC_OSMODE_END;
			if( !tmpOsmode )
				tmpOsmode = osmode & IC_OSMODE_INT;
			if( !tmpOsmode )
				tmpOsmode = osmode & IC_OSMODE_PLAN;
			if( !tmpOsmode )
				tmpOsmode = IC_OSMODE_APP;

/*			if (SDS_osnap_chkdsq(pickptrp,p3,osptrp,dsq))
				{
				ic_ptcpy(ospt,p1);
				ic_ptcpy(osptrp,p3);
				}
*/
			osnap_helper_addPtRec( tmpOsmode, p1, p3, pickptrp, ptAp1rp, ptAp2rp );

/*			if (SDS_osnap_chkdsq(pickptrp,p4,osptrp,dsq))
				{
				ic_ptcpy(ospt,p2);
				ic_ptcpy(osptrp,p4);
				}
*/
			osnap_helper_addPtRec( tmpOsmode, p2, p4, pickptrp, ptAp1rp, ptAp2rp );
			}
		if (osmode & IC_OSMODE_NEAR)//NEA
			{
			//do calcs in rp to get nearest point on screen...
			ic_segdist(pickptrp,p3,p4,0,&fr1,p5);
/*			if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
				{
				ic_ptcpy(osptrp,p5);
				fr1=sds_distance(p3,p5)/sds_distance(p3,p4);
				ospt[0]=p1[0]+fr1*(p2[0]-p1[0]);
				ospt[1]=p1[1]+fr1*(p2[1]-p1[1]);
				ospt[2]=p1[2]+fr1*(p2[2]-p1[2]);
				}
*/
			// translate to UCS now
			fr1=sds_distance(p3,p5)/sds_distance(p3,p4);
			ucsPt[0]=p1[0]+fr1*(p2[0]-p1[0]);
			ucsPt[1]=p1[1]+fr1*(p2[1]-p1[1]);
			ucsPt[2]=p1[2]+fr1*(p2[2]-p1[2]);

			osnap_helper_addPtRec( IC_OSMODE_NEAR, ucsPt, p5, pickptrp, ptAp1rp, ptAp2rp );
			}
		if (osmode & IC_OSMODE_PERP)//PER
			{
			sds_trans(lastpt,&rbucs,&rbwcs,0,p7);
			if (-1!=ic_ptlndist(p7,p1,p2,&fr3,p6))
				{
				fr3=sds_dist2d(p1,p6)/sds_dist2d(p1,p2);
				p6[2]=p1[2]+fr3*(p2[2]-p1[2]);
				gr_ucs2rp(p6,p5,view);
/*				if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
					{
					ic_ptcpy(ospt,p6);
					ic_ptcpy(osptrp,p5);
					}
*/
				osnap_helper_addPtRec( IC_OSMODE_PERP, p6, p5, pickptrp, ptAp1rp, ptAp2rp );
				}
			}
		if (osmode & IC_OSMODE_MID)//MID
			{
			p5[0]=(p1[0]+p2[0])/2.0;
			p5[1]=(p1[1]+p2[1])/2.0;
			p5[2]=(p1[2]+p2[2])/2.0;
			gr_ucs2rp(p5,p6,view);
/*			if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
				{
				ic_ptcpy(ospt,p5);
				ic_ptcpy(osptrp,p6);
				}
*/
			osnap_helper_addPtRec( IC_OSMODE_MID, p5, p6, pickptrp, ptAp1rp, ptAp2rp );
			}
		}
	else if(strsame(etype,db_hitype2str(DB_VERTEX)))  //2D poly in ecs coordinates!
		{
		//NOTE: even if ent is within a block, we'll do all line and arc
		//	math in ECS coordinates.  Arcs can't be done outside of ecs
		//Arcs are passed w/p2 as [0]=rr, [1]=sa, [2]=ea.
		for (rbtemp=elist;rbtemp!=NULL;rbtemp=rbtemp->rbnext)
			{
			if (rbtemp->restype==10)
				ic_ptcpy(p1,rbtemp->resval.rpoint);
			else if (rbtemp->restype==11)
				ic_ptcpy(p2,rbtemp->resval.rpoint);
			else if (rbtemp->restype==42)
				fr1=rbtemp->resval.rreal;
			else if (rbtemp->restype==-1)
				ic_ptcpy(rbent.resval.rlname,rbtemp->resval.rlname);
			}
        if (thickflg)
			{
            ic_ptcpy(p11,p1);
            p11[0]+=thickness*p210[0];
            p11[1]+=thickness*p210[1];
            p11[2]+=thickness*p210[2];
            ic_ptcpy(p12,p2);
			if (0.0==fr1)
				{
				p12[0]+=thickness*p210[0];
				p12[1]+=thickness*p210[1];
				p12[2]+=thickness*p210[2];
				}
			}
		rbtemp=NULL;
		if (!icadRealEqual(fr1,0.0))
			{
			ic_normang(&p2[1],&p2[2]);
			sds_polar(p1,p2[1],p2[0],p3);
			sds_polar(p1,p2[2],p2[0],p4);
			if (thickflg)
				{
                ic_normang(&p12[1],&p12[2]);
			    sds_polar(p11,p12[1],p12[0],p13);
			    sds_polar(p11,p12[2],p12[0],p14);
				}
			}
		if (osmode & (IC_OSMODE_END | IC_OSMODE_INT | IC_OSMODE_PLAN | IC_OSMODE_APP))
			{
			if ((osmode & IC_OSMODE_END) || 1!=pl_omitflag)	//if end osnap set or we're not at beginning of pline
				{
				//check the 10 point
				if (icadRealEqual(fr1,0.0))
					ic_ptcpy(p3,p1);
				if (m_ecs2ucs)
					ic_usemat(p3,p5,m_ecs2ucs,0,0,0);
				else
					sds_trans(p3,&rbent,&rbucs,0,p5);
				gr_ucs2rp(p5,p6,view);
/*				if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
					{
					ic_ptcpy(ospt,p5);
					ic_ptcpy(osptrp,p6);
					}
*/
				osnap_helper_addPtRec( IC_OSMODE_END, p5, p6, pickptrp, ptAp1rp, ptAp2rp );
				if (thickflg)
					{
					if (icadRealEqual(fr1,0.0))ic_ptcpy(p13,p11);
					if (m_ecs2ucs)
						ic_usemat(p13,p15,m_ecs2ucs,0,0,0);
					else
						sds_trans(p13,&rbent,&rbucs,0,p15);
					gr_ucs2rp(p15,p16,view);
/*					if (SDS_osnap_chkdsq(pickptrp,p16,osptrp,dsq))
						{
						ic_ptcpy(ospt,p15);
						ic_ptcpy(osptrp,p16);
						}
*/
					osnap_helper_addPtRec( IC_OSMODE_END, p15, p16, pickptrp, ptAp1rp, ptAp2rp );
					}
				}
			if ((osmode & IC_OSMODE_END) || 2!=pl_omitflag)	//if end osnap set or we're not at beginning of pline
				{
				//check the 11 point
				if (icadRealEqual(fr1,0.0))
					ic_ptcpy(p4,p2);
				if (m_ecs2ucs)
					ic_usemat(p4,p5,m_ecs2ucs,0,0,0);
				else
					sds_trans(p4,&rbent,&rbucs,0,p5);
				gr_ucs2rp(p5,p6,view);
/*				if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
					{
					ic_ptcpy(ospt,p5);
					ic_ptcpy(osptrp,p6);
					}
*/
				osnap_helper_addPtRec( IC_OSMODE_END, p5, p6, pickptrp, ptAp1rp, ptAp2rp );
				if (thickflg)
					{
					if (icadRealEqual(fr1,0.0))
						ic_ptcpy(p14,p12);
					if (m_ecs2ucs)
						ic_usemat(p14,p15,m_ecs2ucs,0,0,1);
					else
						sds_trans(p13,&rbent,&rbucs,0,p15);
					gr_ucs2rp(p15,p16,view);
/*					if (SDS_osnap_chkdsq(pickptrp,p16,osptrp,dsq))
						{
						ic_ptcpy(ospt,p15);
						ic_ptcpy(osptrp,p16);
						}
*/
					osnap_helper_addPtRec( IC_OSMODE_END, p15, p16, pickptrp, ptAp1rp, ptAp2rp );
					}
				}
			}
		if (osmode & IC_OSMODE_NEAR)//NEA
			{
			if (icadRealEqual(fr1,0.0))
				{
				sds_trans(p1,&rbent,&rbucs,0,p5);
				sds_trans(p2,&rbent,&rbucs,0,p6);
				gr_ucs2rp(p5,p5,view);
				gr_ucs2rp(p6,p6,view);
				ic_segdist(pickptrp,p5,p6,0,&fr4,p7);
/*				if (SDS_osnap_chkdsq(pickptrp,p7,osptrp,dsq))
					{
					ic_ptcpy(osptrp,p7);
					fr5=sds_dist2d(p5,p7)/sds_dist2d(p5,p6);
					p7[0]=p1[0]+(fr5*(p2[0]-p1[0]));
					p7[1]=p1[1]+(fr5*(p2[1]-p1[1]));
					p7[2]=p1[2]+(fr5*(p2[2]-p1[2]));
					sds_trans(p7,&rbent,&rbucs,0,ospt);
					}
*/
				// translate to UCS now	
				fr5=sds_dist2d(p5,p7)/sds_dist2d(p5,p6);
				p7[0]=p1[0]+(fr5*(p2[0]-p1[0]));
				p7[1]=p1[1]+(fr5*(p2[1]-p1[1]));
				p7[2]=p1[2]+(fr5*(p2[2]-p1[2]));
				sds_trans(p7,&rbent,&rbucs,0,ucsPt);
				gr_ucs2rp( ucsPt, p7, view );
				osnap_helper_addPtRec( IC_OSMODE_NEAR, ucsPt, p7, pickptrp, ptAp1rp, ptAp2rp );
				}
			else
				{
				SDS_getvar(NULL,DB_QVIEWDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				if (NULL==m_ecs2ucs)
					{
					sds_trans(pickptucs,&rbucs,&rbent,0,p16);
					sds_trans(rb.resval.rpoint,&rbucs,&rbent,1,p15);
					}
				else
					{
					ic_usemat(pickptucs,p16,m_ecs2ucs,0,0,1);//go backwards
					ic_usemat(rb.resval.rpoint,p15,m_ecs2ucs,1,0,1);//go backwards and it's vector
					}
				//adjust for elevation
				if (!icadRealEqual(p16[2],p1[2]) && !icadRealEqual(p15[2],0.0))
					{
					p7[2]=p1[2];   //p1[2] always has elev (center or start pt)
					p7[0]=p16[0]+((p1[2]-p16[2])/p15[2])*p15[0];
					p7[1]=p16[1]+((p1[2]-p16[2])/p15[2])*p15[1];
					//p7 now represents the picked point in correct system adjusted for elev!
					}
				else
					ic_ptcpy(p7,p16);
				//now take p7 & snap it to the segment
				ic_segdist(p7,p1,p2,1,&fr4,p5);
				if (m_ecs2ucs)
					ic_usemat(p5,p6,m_ecs2ucs,0,0,0);
				else
					sds_trans(p5,&rbent,&rbucs,0,p6);
				gr_ucs2rp(p6,p5,view);
/*				if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
					{
					ic_ptcpy(ospt,p6);
					ic_ptcpy(osptrp,p5);
					}
*/
				osnap_helper_addPtRec( IC_OSMODE_NEAR, p6, p5, pickptrp, ptAp1rp, ptAp2rp );
				}
			if (thickflg)
				{
				if (icadRealEqual(fr1,0.0))
					{
					gr_ucs2rp(p11,p5,view);
					gr_ucs2rp(p12,p6,view);
					ic_segdist(pickptrp,p5,p6,0,&fr4,p7);
/*					if (SDS_osnap_chkdsq(pickptrp,p7,osptrp,dsq))
						{
						ic_ptcpy(osptrp,p7);
						fr5=sds_distance(p5,p7)/sds_distance(p5,p6);
						ospt[0]=p11[0]+(fr5*(p12[0]-p11[0]));
						ospt[1]=p11[1]+(fr5*(p12[1]-p11[1]));
						ospt[2]=p11[2]+(fr5*(p12[2]-p11[2]));
						}
*/
					fr5=sds_distance(p5,p7)/sds_distance(p5,p6);
					ucsPt[0]=p11[0]+(fr5*(p12[0]-p11[0]));
					ucsPt[1]=p11[1]+(fr5*(p12[1]-p11[1]));
					ucsPt[2]=p11[2]+(fr5*(p12[2]-p11[2]));
					gr_ucs2rp( ucsPt, p7, view );
					osnap_helper_addPtRec( IC_OSMODE_NEAR, ucsPt, p7, pickptrp, ptAp1rp, ptAp2rp );
					//test along edges
					gr_ucs2rp(p11,p5,view);
					gr_ucs2rp(p1,p6,view);
					ic_segdist(pickptrp,p5,p6,0,&fr4,p7);
/*					if (SDS_osnap_chkdsq(pickptrp,p7,osptrp,dsq))
						{
						ic_ptcpy(osptrp,p7);
						fr5=sds_distance(p5,p7)/sds_distance(p5,p6);
						ospt[0]=p11[0]+(fr5*(p1[0]-p11[0]));
						ospt[1]=p11[1]+(fr5*(p1[1]-p11[1]));
						ospt[2]=p11[2]+(fr5*(p1[2]-p11[2]));
						}
*/
					fr5=sds_distance(p5,p7)/sds_distance(p5,p6);
					ucsPt[0]=p11[0]+(fr5*(p1[0]-p11[0]));
					ucsPt[1]=p11[1]+(fr5*(p1[1]-p11[1]));
					ucsPt[2]=p11[2]+(fr5*(p1[2]-p11[2]));
					gr_ucs2rp( ucsPt, p7, view );
					osnap_helper_addPtRec( IC_OSMODE_NEAR, ucsPt, p7, pickptrp, ptAp1rp, ptAp2rp );
					gr_ucs2rp(p12,p5,view);
					gr_ucs2rp(p2,p6,view);
					ic_segdist(pickptrp,p5,p6,0,&fr4,p7);
/*					if (SDS_osnap_chkdsq(pickptrp,p7,osptrp,dsq))
						{
						ic_ptcpy(osptrp,p7);
						fr5=sds_distance(p5,p7)/sds_distance(p5,p6);
						ospt[0]=p12[0]+(fr5*(p2[0]-p12[0]));
						ospt[1]=p12[1]+(fr5*(p2[1]-p12[1]));
						ospt[2]=p12[2]+(fr5*(p2[2]-p12[2]));
						}
*/
					fr5=sds_distance(p5,p7)/sds_distance(p5,p6);
					ucsPt[0]=p12[0]+(fr5*(p2[0]-p12[0]));
					ucsPt[1]=p12[1]+(fr5*(p2[1]-p12[1]));
					ucsPt[2]=p12[2]+(fr5*(p2[2]-p12[2]));
					gr_ucs2rp( ucsPt, p7, view );
					osnap_helper_addPtRec( IC_OSMODE_NEAR, ucsPt, p7, pickptrp, ptAp1rp, ptAp2rp );
					}
				else
					{
					//adjust for elevation
					if (fabs(p15[2])>IC_ZRO && p16[2]!=p11[2])
						{
						p17[2]=p11[2];   //p1[2] always has elev (center or start pt)
						p17[0]=p16[0]+((p11[2]-p15[2])/p15[2])*p15[0];
						p17[1]=p16[1]+((p11[2]-p15[2])/p15[2])*p15[1];
						//p7 now represents the picked point in correct system adjusted for elev!
						}
					else
						ic_ptcpy(p17,p16);
					//now take p17 & snap it to the segment
					ic_segdist(p17,p11,p12,1,&fr4,p5);
					if (m_ecs2ucs)
						ic_usemat(p5,p6,m_ecs2ucs,0,0,0);
					else
						sds_trans(p5,&rbent,&rbucs,0,p6);
					gr_ucs2rp(p6,p5,view);
/*					if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
						{
						ic_ptcpy(ospt,p6);
						ic_ptcpy(osptrp,p5);
						}
*/
					osnap_helper_addPtRec( IC_OSMODE_NEAR, p6, p5, pickptrp, ptAp1rp, ptAp2rp );
					//test along edges
					ic_segdist(p17,p3,p13,0,&fr4,p5);
					if (m_ecs2ucs)
						ic_usemat(p5,p6,m_ecs2ucs,0,0,0);
					else
						sds_trans(p5,&rbent,&rbucs,0,p6);
					gr_ucs2rp(p6,p5,view);
/*					if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
						{
						ic_ptcpy(ospt,p6);
						ic_ptcpy(osptrp,p5);
						}
*/
					osnap_helper_addPtRec( IC_OSMODE_NEAR, p6, p5, pickptrp, ptAp1rp, ptAp2rp );
					ic_segdist(p17,p4,p14,0,&fr4,p5);
					if (m_ecs2ucs)
						ic_usemat(p5,p6,m_ecs2ucs,0,0,0);
					else
						sds_trans(p5,&rbent,&rbucs,0,p6);
					gr_ucs2rp(p6,p5,view);
/*					if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
						{
						ic_ptcpy(ospt,p6);
						ic_ptcpy(osptrp,p5);
						}
*/
					osnap_helper_addPtRec( IC_OSMODE_NEAR, p6, p5, pickptrp, ptAp1rp, ptAp2rp );
					}
				}
			}
		if (osmode & IC_OSMODE_PERP)//PER
			{
			if (m_ecs2ucs)
				ic_usemat(lastpt,p5,m_ecs2ucs,0,0,1);
			else
				sds_trans(lastpt,&rbucs,&rbent,0,p5);
			if (!icadRealEqual(fr1,0.0))
				{
				fr4=sds_angle(p1,p5);
				ic_normang(&fr2,&fr4);
				//if ((fr4>=p2[1])&&(fr4<=p2[2])){
					sds_polar(p1,fr4,p2[0],p7);
					if (m_ecs2ucs)
						ic_usemat(p7,p6,m_ecs2ucs,0,0,0);
					else
						sds_trans(p7,&rbent,&rbucs,0,p6);
					gr_ucs2rp(p6,p7,view);
/*					if (SDS_osnap_chkdsq(pickptrp,p7,osptrp,dsq))
						{
						ic_ptcpy(ospt,p6);
						ic_ptcpy(osptrp,p7);
						}
*/
					osnap_helper_addPtRec( IC_OSMODE_PERP, p6, p7, pickptrp, ptAp1rp, ptAp2rp );
				//}
				}
			else
				{
				if (-1!=ic_ptlndist(p5,p1,p2,&fr3,p6))
					{
					if (m_ecs2ucs)
						ic_usemat(p6,p6,m_ecs2ucs,0,0,0);
					else
						sds_trans(p6,&rbent,&rbucs,0,p6);
					
					gr_ucs2rp(p6,p7,view);
/*					if (SDS_osnap_chkdsq(pickptrp,p7,osptrp,dsq))
						{
						ic_ptcpy(ospt,p6);
						ic_ptcpy(osptrp,p7);
						}
*/
					osnap_helper_addPtRec( IC_OSMODE_PERP, p6, p7, pickptrp, ptAp1rp, ptAp2rp );
					}
				}
			if (thickflg)
				{
				if (!icadRealEqual(fr1,0.0))
					{
					fr4=sds_angle(p11,p5);
					ic_normang(&fr2,&fr4);
					//if ((fr4>=p2[1])&&(fr4<=p2[2])){
						sds_polar(p11,fr4,p2[0],p7);
						if (m_ecs2ucs)
							ic_usemat(p7,p6,m_ecs2ucs,0,0,0);
						else
							sds_trans(p7,&rbent,&rbucs,0,p6);
						gr_ucs2rp(p6,p7,view);
/*						if (SDS_osnap_chkdsq(pickptrp,p7,osptrp,dsq))
							{
							ic_ptcpy(ospt,p6);
							ic_ptcpy(osptrp,p7);
							}
*/
						osnap_helper_addPtRec( IC_OSMODE_PERP, p6, p7, pickptrp, ptAp1rp, ptAp2rp );
					//}
					}
				else
					{
					if (-1!=ic_ptlndist(p5,p11,p12,&fr3,p6))
						{
						if (m_ecs2ucs)
							ic_usemat(p6,p6,m_ecs2ucs,0,0,0);
						else
							sds_trans(p6,&rbent,&rbucs,0,p6);
						gr_ucs2rp(p6,p7,view);
/*						if (SDS_osnap_chkdsq(pickptrp,p7,osptrp,dsq))
							{
							ic_ptcpy(ospt,p6);
							ic_ptcpy(osptrp,p7);
							}
*/
						osnap_helper_addPtRec( IC_OSMODE_PERP, p6, p7, pickptrp, ptAp1rp, ptAp2rp );

						}
					}
				}
			}
		if (osmode & IC_OSMODE_QUADRANT && !icadRealEqual(fr1,0.0))//QUA of bulged section
			{
			//test if entity is parallel to current ucs...
			p7[0]=p7[1]=1.0;
			p7[2]=0.0;
			sds_trans(p7,&rbent,&rbucs,1,p7);
			if (icadRealEqual(p7[2],0.0))
				{
				p7[0]=1.0;
				p7[1]=p7[2]=0.0;
				sds_trans(p7,&rbent,&rbucs,1,p7);
				fr5=atan2(p7[1],p7[0]);
				for (fi1=0;fi1<4;fi1++)
					{
					fr4=(fi1*IC_PI/2.0)-fr5;
					ic_normang(&p2[1],&fr4);
					if (fr4<=p2[2])
						{
						ic_ptcpy(p6,p1);
						p7[0]=sin(fr4);
						p7[1]=cos(fr4);
						p6[0]+=p2[0]*p7[1];
						p6[1]+=p2[0]*p7[0];
						if (m_ecs2ucs)
							ic_usemat(p6,p7,m_ecs2ucs,0,0,0);
						else
							sds_trans(p6,&rbent,&rbucs,0,p7);
						gr_ucs2rp(p7,p5,view);
/*						if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
							{
							ic_ptcpy(ospt,p7);
							ic_ptcpy(osptrp,p5);
							}
*/
						osnap_helper_addPtRec( IC_OSMODE_QUADRANT, p7, p5, pickptrp, ptAp1rp, ptAp2rp );
						if (thickflg)
							{
							p6[0]+=thickness*p210[0];
							p6[1]+=thickness*p210[1];
							p6[2]+=thickness*p210[2];
							if (m_ecs2ucs)
								ic_usemat(p6,p7,m_ecs2ucs,0,0,0);
							else
								sds_trans(p6,&rbent,&rbucs,0,p7);
							gr_ucs2rp(p7,p5,view);
/*							if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
								{
								ic_ptcpy(ospt,p7);
								ic_ptcpy(osptrp,p5);
								}
*/
							osnap_helper_addPtRec( IC_OSMODE_QUADRANT, p7, p5, pickptrp, ptAp1rp, ptAp2rp );
							}
						}//if
					}//for
				}
			}
		if (osmode & IC_OSMODE_CENTER)//CEN
			{
			if (!icadRealEqual(fr1,0.0))
				{
				if (m_ecs2ucs)
					ic_usemat(p1,p5,m_ecs2ucs,0,0,0);
				else
					sds_trans(p1,&rbent,&rbucs,0,p5);
				gr_ucs2rp(p5,p6,view);
/*				if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
					{
					ic_ptcpy(ospt,p5);
					ic_ptcpy(osptrp,p6);
					}
*/
				osnap_helper_addPtRec( IC_OSMODE_CENTER, p5, p6, pickptrp, ptAp1rp, ptAp2rp );
				if (thickflg)
					{
                    sds_trans(p11,&rbent,&rbucs,0,p15);
				    gr_ucs2rp(p15,p16,view);
/*				    if (SDS_osnap_chkdsq(pickptrp,p16,osptrp,dsq))
						{
						ic_ptcpy(ospt,p15);
						ic_ptcpy(osptrp,p16);
						}
*/
					osnap_helper_addPtRec( IC_OSMODE_CENTER, p15, p16, pickptrp, ptAp1rp, ptAp2rp );
					}
				}
			}
		if (osmode & IC_OSMODE_MID)//MID
			{
			if (icadRealEqual(fr1,0.0))
				{
				p5[0]=(p1[0]+p2[0])/2.0;
				p5[1]=(p1[1]+p2[1])/2.0;
				p5[2]=(p1[2]+p2[2])/2.0;
				}
			else
				{
				fr3=(p2[1]+p2[2])/2.0;
				sds_polar(p1,fr3,p2[0],p5);
				}
			if (m_ecs2ucs)
				ic_usemat(p5,p6,m_ecs2ucs,0,0,0);
			else
				sds_trans(p5,&rbent,&rbucs,0,p6);
			gr_ucs2rp(p6,p7,view);
/*			if (SDS_osnap_chkdsq(pickptrp,p7,osptrp,dsq))
				{
				ic_ptcpy(ospt,p6);
				ic_ptcpy(osptrp,p7);
				}
*/
			osnap_helper_addPtRec( IC_OSMODE_MID, p6, p7, pickptrp, ptAp1rp, ptAp2rp );
			if (thickflg)
				{
                if (icadRealEqual(fr1,0.0))
					{
				    p15[0]=(p11[0]+p12[0])/2.0;
				    p15[1]=(p11[1]+p12[1])/2.0;
				    p15[2]=(p11[2]+p12[2])/2.0;
					}
				else
					{
				    fr3=(p12[1]+p12[2])/2.0;
				    sds_polar(p11,fr3,p12[0],p15);
					}
				if (m_ecs2ucs)
					ic_usemat(p15,p16,m_ecs2ucs,0,0,0);
				else
					sds_trans(p15,&rbent,&rbucs,0,p16);
			    gr_ucs2rp(p16,p17,view);
/*			    if (SDS_osnap_chkdsq(pickptrp,p17,osptrp,dsq))
					{
					ic_ptcpy(ospt,p16);
					ic_ptcpy(osptrp,p17);
					}
*/
				osnap_helper_addPtRec( IC_OSMODE_MID, p16, p17, pickptrp, ptAp1rp, ptAp2rp );
				//check along the edges
                if (icadRealEqual(fr1,0.0))
					{
				    p15[0]=(p11[0]+p1[0])/2.0;
				    p15[1]=(p11[1]+p1[1])/2.0;
				    p15[2]=(p11[2]+p1[2])/2.0;
					}
				else
					{
				    p15[0]=(p13[0]+p3[0])/2.0;
				    p15[1]=(p13[1]+p3[1])/2.0;
				    p15[2]=(p13[2]+p3[2])/2.0;
					}
				if (m_ecs2ucs)
					ic_usemat(p15,p16,m_ecs2ucs,0,0,0);
				else
					sds_trans(p15,&rbent,&rbucs,0,p16);
			    gr_ucs2rp(p16,p17,view);
/*			    if (SDS_osnap_chkdsq(pickptrp,p17,osptrp,dsq))
					{
					ic_ptcpy(ospt,p16);
					ic_ptcpy(osptrp,p17);
					}
*/
				osnap_helper_addPtRec( IC_OSMODE_MID, p16, p17, pickptrp, ptAp1rp, ptAp2rp );
                if (icadRealEqual(fr1,0.0))
					{
				    p15[0]=(p12[0]+p2[0])/2.0;
				    p15[1]=(p12[1]+p2[1])/2.0;
				    p15[2]=(p12[2]+p2[2])/2.0;
					}
				else
					{
				    p15[0]=(p14[0]+p4[0])/2.0;
				    p15[1]=(p14[1]+p4[1])/2.0;
				    p15[2]=(p14[2]+p4[2])/2.0;
					}
				if (m_ecs2ucs)
					ic_usemat(p15,p16,m_ecs2ucs,0,0,0);
				else
					sds_trans(p15,&rbent,&rbucs,0,p16);
			    gr_ucs2rp(p16,p17,view);
/*			    if (SDS_osnap_chkdsq(pickptrp,p17,osptrp,dsq))
					{
					ic_ptcpy(ospt,p16);
					ic_ptcpy(osptrp,p17);
					}
*/
				osnap_helper_addPtRec( IC_OSMODE_MID, p16, p17, pickptrp, ptAp1rp, ptAp2rp );
				}
			}
		if ((osmode & IC_OSMODE_TAN) && !icadRealEqual(fr1,0.0))//TAN
			{
			if (m_ecs2ucs)
				ic_usemat(lastpt,p5,m_ecs2ucs,0,0,1);
			else
				sds_trans(lastpt,&rbucs,&rbent,0,p5);
			fr4=sds_dist2d(p1,p5);
			fr3=p2[0]; //radius - fix for bug 1-65912.  Earlier, fr1 (the bulge) was being used.  
					   //This is incorrect, we need to use the radius instead of the bulge in this algorithm.
			if (fr4<=fr3)
				{
				fr5=sds_angle(p1,p5);
				ic_normang(&fr2,&fr5);
				if (fr5>=p2[1] && fr5<=p2[2])
					{
					sds_polar(p1,fr5,fr3,p6);
					if (m_ecs2ucs)
						ic_usemat(p6,p6,m_ecs2ucs,0,0,0);
					else
						sds_trans(p6,&rbent,&rbucs,0,p6);
					gr_ucs2rp(p6,p7,view);
/*					if (SDS_osnap_chkdsq(pickptrp,p7,osptrp,dsq))
						{
						ic_ptcpy(ospt,p6);
						ic_ptcpy(osptrp,p7);
						}
*/
					osnap_helper_addPtRec( IC_OSMODE_TAN, p6, p7, pickptrp, ptAp1rp, ptAp2rp );
					}
				}
			else
				{
				//get dist from lastpt to tangent pt
				fr5=sqrt((fr4*fr4)-(fr3*fr3));
				fi1=ic_arcxarc(p1,fr3,p2[1],p2[2],p5,fr5,0.0,IC_PI,p6,p7);
				p6[2]=p7[2]=p1[2];
				if (fi1&1)
					{
					if (m_ecs2ucs)
						ic_usemat(p6,p5,m_ecs2ucs,0,0,0);
					else
						sds_trans(p6,&rbent,&rbucs,0,p5);
					gr_ucs2rp(p5,p6,view);
/*					if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
						{
						ic_ptcpy(ospt,p5);
						ic_ptcpy(osptrp,p6);
						}
*/
					osnap_helper_addPtRec( IC_OSMODE_TAN, p5, p6, pickptrp, ptAp1rp, ptAp2rp );
					}
				if (fi1&4)
					{
					if (m_ecs2ucs)
						ic_usemat(p7,p5,m_ecs2ucs,0,0,0);
					else
						sds_trans(p7,&rbent,&rbucs,0,p5);
					gr_ucs2rp(p5,p6,view);
/*					if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
						{
						ic_ptcpy(ospt,p6);
						ic_ptcpy(osptrp,p6);
						}
*/
					osnap_helper_addPtRec( IC_OSMODE_TAN, p5, p6, pickptrp, ptAp1rp, ptAp2rp );
					}
				}
			if (thickflg)
				{
				if (m_ecs2ucs)
					ic_usemat(lastpt,p15,m_ecs2ucs,0,0,1);
				else
					sds_trans(lastpt,&rbucs,&rbent,0,p15);
			    fr4=sds_dist2d(p11,p15);
			    if (fr4<=fr3)
					{
				    fr5=sds_angle(p11,p15);
				    ic_normang(&fr2,&fr5);
				    if ((fr5>=p12[1])&&(fr5<=p12[2]))
						{
					    sds_polar(p11,fr3,fr3,p16);
						if (m_ecs2ucs)
							ic_usemat(p16,p16,m_ecs2ucs,0,0,0);
						else
							sds_trans(p16,&rbent,&rbucs,0,p16);
					    gr_ucs2rp(p16,p17,view);
/*					    if (SDS_osnap_chkdsq(pickptrp,p17,osptrp,dsq))
							{
							ic_ptcpy(ospt,p16);
							ic_ptcpy(osptrp,p17);
							}
*/
						osnap_helper_addPtRec( IC_OSMODE_TAN, p16, p17, pickptrp, ptAp1rp, ptAp2rp );
						}
					}
				else
					{
				    //get dist from lastpt to tangent pt
				    fr5=sqrt((fr4*fr4)-(fr3*fr3));
				    fi1=ic_arcxarc(p11,fr3,p12[1],p12[2],p15,fr5,0.0,IC_PI,p16,p17);
					p16[2]=p17[2]=p11[2];
				    if (fi1&1)
						{
						if (m_ecs2ucs)
							ic_usemat(p16,p5,m_ecs2ucs,0,0,0);
						else
							sds_trans(p16,&rbent,&rbucs,0,p15);
					    gr_ucs2rp(p15,p16,view);
/*					    if (SDS_osnap_chkdsq(pickptrp,p16,osptrp,dsq))
							{
							ic_ptcpy(ospt,p15);
							ic_ptcpy(osptrp,p16);
							}
*/
						osnap_helper_addPtRec( IC_OSMODE_TAN, p15, p16, pickptrp, ptAp1rp, ptAp2rp );
						}
				    if (fi1&4)
						{
						if (m_ecs2ucs)
							ic_usemat(p17,p15,m_ecs2ucs,0,0,0);
						else
							sds_trans(p17,&rbent,&rbucs,0,p15);
					    gr_ucs2rp(p15,p16,view);
/*					    if (SDS_osnap_chkdsq(pickptrp,p16,osptrp,dsq))
							{
							ic_ptcpy(ospt,p16);
							ic_ptcpy(osptrp,p16);
							}
*/
						osnap_helper_addPtRec( IC_OSMODE_TAN, p15, p16, pickptrp, ptAp1rp, ptAp2rp );
						}
					}
				}
			}
		}
	else if (strsame(etype,db_hitype2str(DB_VIEWPORT)))
		{
		//NOTE: viewports can't have a thickness...
		if (SDS_getvar(NULL,DB_QTILEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) {ret=RTERROR;goto exit_point;}
		if (1==rb.resval.rint)
			goto exit_point;
		if (SDS_getvar(NULL,DB_QCVPORT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) {ret=RTERROR;goto exit_point;}
		if (rb.resval.rint!=1)
			goto exit_point;
		//okay, we're in paper space...
		for (rbtemp=elist;rbtemp!=NULL;rbtemp=rbtemp->rbnext)
			{
			if (rbtemp->restype==10)
				ic_ptcpy(p1,rbtemp->resval.rpoint);
			else if (rbtemp->restype==40)
				fr1=rbtemp->resval.rreal;
			else if (rbtemp->restype==41)
				fr2=rbtemp->resval.rreal;
			}
		fr1*=0.5;
		fr2*=0.5;
		if (osmode & IC_OSMODE_CENTER)//CEN
			{
			if (m_ecs2ucs)
				ic_usemat(p1,p7,m_ecs2ucs,0,0,0);
			else
				ic_ptcpy(p7,p1);
			gr_ucs2rp(p7,p6,view);
/*			if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
				{
				ic_ptcpy(ospt,p7);
				ic_ptcpy(osptrp,p6);
				}
*/
			osnap_helper_addPtRec( IC_OSMODE_CENTER, p7, p6, pickptrp, ptAp1rp, ptAp2rp );
			}
		//setup p1 thru p4 as the corners
		ic_ptcpy(p2,p1);
		p2[0]+=fr1;
		p2[1]-=fr2;
		ic_ptcpy(p3,p1);
		p3[0]-=fr1;
		p3[1]-=fr2;
		ic_ptcpy(p4,p1);
		p4[0]-=fr1;
		p4[1]+=fr2;
		p1[0]+=fr1;
		p1[1]+=fr2;
		if (m_ecs2ucs)
			{
			ic_usemat(p1,p1,m_ecs2ucs,0,0,0);
			ic_usemat(p2,p2,m_ecs2ucs,0,0,0);
			ic_usemat(p3,p3,m_ecs2ucs,0,0,0);
			ic_usemat(p4,p4,m_ecs2ucs,0,0,0);
			}
		else
			{
			sds_trans(p1,&rbwcs,&rbucs,0,p1);
			sds_trans(p2,&rbwcs,&rbucs,0,p2);
			sds_trans(p3,&rbwcs,&rbucs,0,p3);
			sds_trans(p4,&rbwcs,&rbucs,0,p4);
			}
		if (osmode & (IC_OSMODE_END | IC_OSMODE_INT | IC_OSMODE_PLAN | IC_OSMODE_APP))
			{
			tmpOsmode = osmode & IC_OSMODE_END;
			if( !tmpOsmode )
				tmpOsmode = osmode & IC_OSMODE_INT;
			if( !tmpOsmode )
				tmpOsmode = osmode & IC_OSMODE_PLAN;
			if( !tmpOsmode )
				tmpOsmode = IC_OSMODE_APP;

			gr_ucs2rp(p1,p6,view);
/*			if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
				{
				ic_ptcpy(ospt,p1);
				ic_ptcpy(osptrp,p6);
				}
*/
			osnap_helper_addPtRec( tmpOsmode, p1, p6, pickptrp, ptAp1rp, ptAp2rp );
			gr_ucs2rp(p2,p6,view);
/*			if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
				{
				ic_ptcpy(ospt,p2);
				ic_ptcpy(osptrp,p6);
				}
*/
			osnap_helper_addPtRec( tmpOsmode, p2, p6, pickptrp, ptAp1rp, ptAp2rp );
			gr_ucs2rp(p3,p6,view);
/*			if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
				{
				ic_ptcpy(ospt,p3);
				ic_ptcpy(osptrp,p6);
				}
*/
			osnap_helper_addPtRec( tmpOsmode, p3, p6, pickptrp, ptAp1rp, ptAp2rp );
			gr_ucs2rp(p4,p6,view);
/*			if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
				{
				ic_ptcpy(ospt,p4);
				ic_ptcpy(osptrp,p6);
				}
*/
			osnap_helper_addPtRec( tmpOsmode, p4, p6, pickptrp, ptAp1rp, ptAp2rp );
			}
		if (osmode & IC_OSMODE_NEAR)//NEA
			{
			gr_ucs2rp(p1,p5,view);
			gr_ucs2rp(p2,p6,view);
			ic_segdist(pickptrp,p5,p6,0,&fr1,p7);
/*			if (SDS_osnap_chkdsq(pickptrp,p7,osptrp,dsq))
				{
				ic_ptcpy(osptrp,p7);
				fr1=sds_distance(p5,p7)/sds_distance(p5,p6);
				ospt[0]=p1[0]+(fr1*(p2[0]-p1[0]));
				ospt[1]=p1[1]+(fr1*(p2[1]-p1[1]));
				ospt[2]=p1[2]+(fr1*(p2[2]-p1[2]));
				}
*/
			fr1=sds_distance(p5,p7)/sds_distance(p5,p6);
			ucsPt[0]=p1[0]+(fr1*(p2[0]-p1[0]));
			ucsPt[1]=p1[1]+(fr1*(p2[1]-p1[1]));
			ucsPt[2]=p1[2]+(fr1*(p2[2]-p1[2]));
			osnap_helper_addPtRec( IC_OSMODE_NEAR, ucsPt, p7, pickptrp, ptAp1rp, ptAp2rp );
			gr_ucs2rp(p3,p5,view);
			ic_segdist(pickptrp,p5,p6,0,&fr1,p7);
/*			if (SDS_osnap_chkdsq(pickptrp,p7,osptrp,dsq))
				{
				ic_ptcpy(osptrp,p7);
				fr1=sds_distance(p5,p7)/sds_distance(p5,p6);
				ospt[0]=p3[0]+(fr1*(p2[0]-p3[0]));
				ospt[1]=p3[1]+(fr1*(p2[1]-p3[1]));
				ospt[2]=p3[2]+(fr1*(p2[2]-p3[2]));
				}
*/
			fr1=sds_distance(p5,p7)/sds_distance(p5,p6);
			ucsPt[0]=p3[0]+(fr1*(p2[0]-p3[0]));
			ucsPt[1]=p3[1]+(fr1*(p2[1]-p3[1]));
			ucsPt[2]=p3[2]+(fr1*(p2[2]-p3[2]));
			osnap_helper_addPtRec( IC_OSMODE_NEAR, ucsPt, p7, pickptrp, ptAp1rp, ptAp2rp );
			gr_ucs2rp(p4,p6,view);
			ic_segdist(pickptrp,p5,p6,0,&fr1,p7);
/*			if (SDS_osnap_chkdsq(pickptrp,p7,osptrp,dsq))
				{
				ic_ptcpy(osptrp,p7);
				fr1=sds_distance(p5,p7)/sds_distance(p5,p6);
				ospt[0]=p3[0]+(fr1*(p4[0]-p3[0]));
				ospt[1]=p3[1]+(fr1*(p4[1]-p3[1]));
				ospt[2]=p3[2]+(fr1*(p4[2]-p3[2]));
				}
*/
			fr1=sds_distance(p5,p7)/sds_distance(p5,p6);
			ucsPt[0]=p3[0]+(fr1*(p4[0]-p3[0]));
			ucsPt[1]=p3[1]+(fr1*(p4[1]-p3[1]));
			ucsPt[2]=p3[2]+(fr1*(p4[2]-p3[2]));
			osnap_helper_addPtRec( IC_OSMODE_NEAR, ucsPt, p7, pickptrp, ptAp1rp, ptAp2rp );
			gr_ucs2rp(p1,p5,view);
			ic_segdist(pickptrp,p5,p6,0,&fr1,p7);
/*			if (SDS_osnap_chkdsq(pickptrp,p7,osptrp,dsq))
				{
				ic_ptcpy(osptrp,p7);
				fr1=sds_distance(p5,p7)/sds_distance(p5,p6);
				ospt[0]=p1[0]+(fr1*(p4[0]-p1[0]));
				ospt[1]=p1[1]+(fr1*(p4[1]-p1[1]));
				ospt[2]=p1[2]+(fr1*(p4[2]-p1[2]));
				}
*/
			fr1=sds_distance(p5,p7)/sds_distance(p5,p6);
			ucsPt[0]=p1[0]+(fr1*(p4[0]-p1[0]));
			ucsPt[1]=p1[1]+(fr1*(p4[1]-p1[1]));
			ucsPt[2]=p1[2]+(fr1*(p4[2]-p1[2]));
			osnap_helper_addPtRec( IC_OSMODE_NEAR, ucsPt, p7, pickptrp, ptAp1rp, ptAp2rp );
			}
		if (osmode & IC_OSMODE_PERP)//PER
			{
			//NOTE: can't do calc in RP, because per is independent of view
			//NOTE: all pts should be in UCS, including lastpt
			//lastpt already in ucs
			if (-1!=ic_ptlndist(lastpt,p1,p2,&fr1,p7))
				{
				gr_ucs2rp(p7,p6,view);
/*				if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
					{
					ic_ptcpy(ospt,p7);
					ic_ptcpy(osptrp,p6);
					}
*/
				osnap_helper_addPtRec( IC_OSMODE_PERP, p7, p6, pickptrp, ptAp1rp, ptAp2rp );
				}	
			if (-1!=ic_ptlndist(lastpt,p2,p3,&fr1,p7))
				{
				gr_ucs2rp(p7,p6,view);
/*				if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
					{
					ic_ptcpy(ospt,p7);
					ic_ptcpy(osptrp,p6);
					}
*/
				osnap_helper_addPtRec( IC_OSMODE_PERP, p7, p6, pickptrp, ptAp1rp, ptAp2rp );
				}
			if (-1!=ic_ptlndist(lastpt,p3,p4,&fr1,p7))
				{
				gr_ucs2rp(p7,p6,view);
/*				if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
					{
					ic_ptcpy(ospt,p7);
					ic_ptcpy(osptrp,p6);
					}
*/
				osnap_helper_addPtRec( IC_OSMODE_PERP, p7, p6, pickptrp, ptAp1rp, ptAp2rp );
				}
			if (-1!=ic_ptlndist(lastpt,p4,p1,&fr1,p7))
				{
				gr_ucs2rp(p7,p6,view);
/*				if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
					{
					ic_ptcpy(ospt,p7);
					ic_ptcpy(osptrp,p6);
					}
*/
				osnap_helper_addPtRec( IC_OSMODE_PERP, p7, p6, pickptrp, ptAp1rp, ptAp2rp );
				}
			}
		if (osmode & IC_OSMODE_MID)//MID
			{
			//1-2
			p5[0]=(p1[0]+p2[0])/2.0;
			p5[1]=(p1[1]+p2[1])/2.0;
			gr_ucs2rp(p5,p6,view);
/*			if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
				{
				ic_ptcpy(ospt,p5);
				ic_ptcpy(osptrp,p6);
				}			
*/
			osnap_helper_addPtRec( IC_OSMODE_MID, p5, p6, pickptrp, ptAp1rp, ptAp2rp );
			//2-3
			p5[0]=(p2[0]+p3[0])/2.0;
			p5[1]=(p2[1]+p3[1])/2.0;
			gr_ucs2rp(p5,p6,view);
/*			if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
				{
				ic_ptcpy(ospt,p5);
				ic_ptcpy(osptrp,p6);
				}
*/
			osnap_helper_addPtRec( IC_OSMODE_MID, p5, p6, pickptrp, ptAp1rp, ptAp2rp );
			//3-4
			p5[0]=(p4[0]+p3[0])/2.0;
			p5[1]=(p4[1]+p3[1])/2.0;
			p5[2]=(p4[2]+p3[2])/2.0;
			gr_ucs2rp(p5,p6,view);
/*			if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
				{
				ic_ptcpy(ospt,p5);
				ic_ptcpy(osptrp,p6);
				}
*/
			osnap_helper_addPtRec( IC_OSMODE_MID, p5, p6, pickptrp, ptAp1rp, ptAp2rp );
			//1-4
			p5[0]=(p4[0]+p1[0])/2.0;
			p5[1]=(p4[1]+p1[1])/2.0;
			p5[2]=(p4[2]+p1[2])/2.0;
			gr_ucs2rp(p5,p6,view);
/*			if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
				{
				ic_ptcpy(ospt,p5);
				ic_ptcpy(osptrp,p6);
				}
*/
			osnap_helper_addPtRec( IC_OSMODE_MID, p5, p6, pickptrp, ptAp1rp, ptAp2rp );
			}
		}
	else if (strsame(etype,db_hitype2str(DB_SPLINE)))
		{

		//NOTE: splines cannot have thickness
		C3Point ptNormal;
		CNURBS* pNurbs = NULL;
		if (GetSplineFrom_resbuf(elist, pNurbs, ptNormal) != SUCCESS) 
			goto exit_point;

        CAffine wcsXForm(C3Vector(ptNormal.X(), ptNormal.Y(), ptNormal.Z()));
        if (!icadIsZero(ptNormal.X()) || !icadIsZero(ptNormal.Y()))
            pNurbs->Transform(wcsXForm);

		RC rc;
		CNURBS nurbs(*pNurbs, rc);
		delete pNurbs;
		pNurbs = NULL;

		if (osmode & IC_OSMODE_END)	{//END
			C3Point start, end;
			nurbs.GetStartPoint(start);
			nurbs.GetEndPoint(end);

			p1[0] = start.X(); p1[1] = start.Y(); p1[2] = start.Z();
			p2[0] = end.X(); p2[1] = end.Y(); p2[2] = end.Z();

			sds_trans(p1,&rbwcs,&rbucs,0,p1);
			sds_trans(p2,&rbwcs,&rbucs,0,p2);
			
			gr_ucs2rp(p1,p3,view);
			gr_ucs2rp(p2,p4,view);

			osnap_helper_addPtRec(IC_OSMODE_END, p1, p3, pickptrp, ptAp1rp, ptAp2rp );
			osnap_helper_addPtRec(IC_OSMODE_END, p2, p4, pickptrp, ptAp1rp, ptAp2rp );
		}
		if (osmode & IC_OSMODE_NEAR)//NEA
			{
			sds_trans(pickptucs, &rbucs,&rbwcs, 0,p3);

			C3Point pickPoint(p3[0], p3[1], p3[2]);

			C3Point nearestPoint;
			double rAt, rDis;
			if (nurbs.NearestPoint(pickPoint, nearestPoint, rAt, rDis) == SUCCESS) 
				{

				p3[0] = nearestPoint.X(); p3[1] = nearestPoint.Y(); p3[2] = nearestPoint.Z();

				if (m_ecs2ucs)
					ic_usemat(p3,p4,m_ecs2ucs,0,0,0);
				else
					sds_trans(p3,&rbwcs,&rbucs,0,p4);
				gr_ucs2rp(p4,p3,view);

				osnap_helper_addPtRec(IC_OSMODE_NEAR, p4, p3, pickptrp, ptAp1rp, ptAp2rp );
				}
		    }
		if (osmode & IC_OSMODE_MID) //MID
			{
			if (!nurbs.Closed())
				{
				//check starting point
				double mid = (nurbs.Start() + nurbs.End()) / 2.0;

				C3Point midPoint, tangent;
				if (nurbs.Evaluate(mid, midPoint, tangent) == SUCCESS) 
					{
					p3[0] = midPoint.X();
					p3[1] = midPoint.Y();
					p3[2] = midPoint.Z();

					sds_trans(p3,&rbwcs,&rbucs,0,p4);
					gr_ucs2rp(p4,p3,view);
					osnap_helper_addPtRec( IC_OSMODE_MID, p4, p3, pickptrp, ptAp1rp, ptAp2rp );
					}
				}
		    }
		if ((osmode & (IC_OSMODE_PERP | IC_OSMODE_NEAR))==IC_OSMODE_PERP)//PER must be calc'd in ECS
			{
			sds_trans(lastpt,&rbucs,&rbwcs,0,p5);
			C3Point lastPoint(p5[0], p5[1], p5[2]);

			sds_trans(pickptucs, &rbucs,&rbwcs, 0,p6);
			C3Point ptPicked(p6[0], p6[1], p6[2]);

			double param;
			C3Point ptPerpendicular;
			if (nurbs.PerpendicularThru(lastPoint, ptPicked, ptPerpendicular, param) == SUCCESS)
				{
				p3[0] = ptPerpendicular.X();
				p3[1] = ptPerpendicular.Y();
				p3[2] = ptPerpendicular.Z();

				sds_trans(p3,&rbwcs,&rbucs,0,p4);
				gr_ucs2rp(p4,p3,view);

				osnap_helper_addPtRec( IC_OSMODE_PERP, p4, p3, pickptrp, ptAp1rp, ptAp2rp );
				}
			}
		if (osmode & IC_OSMODE_TAN)	// TAN
			{		
			sds_trans(lastpt,&rbucs,&rbwcs,0,p5);
			C3Point lastPoint(p5[0], p5[1], p5[2]);

			sds_trans(pickptucs, &rbucs,&rbwcs, 0,p3);
			C3Point pickPoint(p3[0], p3[1], p3[2]);

			double rAt, rDis;
			C3Point ptNearest;
			if (nurbs.NearestPoint(pickPoint, ptNearest, rAt, rDis) != SUCCESS) 
				rAt = (nurbs.Start() + nurbs.End()) / 2.0;

			C3Point ptTangent, tangent;
			if (nurbs.TangentThru(lastPoint, rAt, rAt) == SUCCESS &&
				nurbs.Evaluate(rAt, ptTangent, tangent) == SUCCESS) 
				{
				p3[0] = ptTangent.X();
				p3[1] = ptTangent.Y();
				p3[2] = ptTangent.Z();

				sds_trans(p3,&rbwcs,&rbucs,0,p4);
				gr_ucs2rp(p4,p3,view);

				osnap_helper_addPtRec(IC_OSMODE_TAN, p4, p3, pickptrp, ptAp1rp, ptAp2rp );
				}
			}
		}
	else if (strsame(etype,db_hitype2str(DB_POINT)))
		{
		if (osmode & (IC_OSMODE_NEAR | IC_OSMODE_NODE))	//NODE(a.k.a. POINT) or NEA
			{
			tmpOsmode = osmode & IC_OSMODE_NEAR;
			if( !tmpOsmode )
				tmpOsmode = IC_OSMODE_NODE;

			ic_assoc(elist,10);
			ic_ptcpy(p1,ic_rbassoc->resval.rpoint);
			if (m_ecs2ucs)
				ic_usemat(p1,p1,m_ecs2ucs,0,0,0);
			else
				sds_trans(p1,&rbwcs,&rbucs,0,p1);
			gr_ucs2rp(p1,p2,view);
/*			if (SDS_osnap_chkdsq(pickptrp,p2,osptrp,dsq))
				{
				ic_ptcpy(ospt,p1);
				ic_ptcpy(osptrp,p2);
				}
*/
			osnap_helper_addPtRec( tmpOsmode, p1, p2, pickptrp, ptAp1rp, ptAp2rp );
			if (thickflg)
				{
                ic_assoc(elist,10);
			    ic_ptcpy(p11,ic_rbassoc->resval.rpoint);
			    ic_assoc(elist,210);
                ic_rbassoc->resval.rpoint[0]*=thickness;ic_rbassoc->resval.rpoint[1]*=thickness;ic_rbassoc->resval.rpoint[1]*=thickness;
                p11[0]+=ic_rbassoc->resval.rpoint[0];p11[1]+=ic_rbassoc->resval.rpoint[1];p11[2]+=ic_rbassoc->resval.rpoint[2];
                sds_trans(p11,&rbwcs,&rbucs,0,p11);
			    gr_ucs2rp(p11,p12,view);
/*			    if (SDS_osnap_chkdsq(pickptrp,p12,osptrp,dsq))
					{
					ic_ptcpy(ospt,p11);
					ic_ptcpy(osptrp,p12);
					}
*/
				osnap_helper_addPtRec( tmpOsmode, p11, p12, pickptrp, ptAp1rp, ptAp2rp );
				}
			}
		}
	else if (strsame(etype,db_hitype2str(DB_MTEXT)))
		{
		if (osmode & (IC_OSMODE_INS | IC_OSMODE_NEAR))	//INS or NEA
			{
			tmpOsmode = osmode & IC_OSMODE_INS;
			if( !tmpOsmode )
				tmpOsmode = IC_OSMODE_NEAR;

			ic_assoc(elist,10);
			ic_ptcpy(p1,ic_rbassoc->resval.rpoint);
			if (m_ecs2ucs)
				ic_usemat(p1,p1,m_ecs2ucs,0,0,0);
			else
				sds_trans(p1,&rbwcs,&rbucs,0,p1);
			gr_ucs2rp(p1,p2,view);
/*			if (SDS_osnap_chkdsq(pickptrp,p2,osptrp,dsq))
				{
				ic_ptcpy(ospt,p1);
				ic_ptcpy(osptrp,p2);
				}
*/
			osnap_helper_addPtRec( tmpOsmode, p1, p2, pickptrp, ptAp1rp, ptAp2rp );
			if (thickflg)
				{
                ic_assoc(elist,10);
			    ic_ptcpy(p11,ic_rbassoc->resval.rpoint);
			    ic_assoc(elist,210);
                ic_rbassoc->resval.rpoint[0]*=thickness;ic_rbassoc->resval.rpoint[1]*=thickness;ic_rbassoc->resval.rpoint[1]*=thickness;
                p11[0]+=ic_rbassoc->resval.rpoint[0];p11[1]+=ic_rbassoc->resval.rpoint[1];p11[2]+=ic_rbassoc->resval.rpoint[2];
                sds_trans(p11,&rbwcs,&rbucs,0,p11);
			    gr_ucs2rp(p11,p12,view);
/*			    if (SDS_osnap_chkdsq(pickptrp,p12,osptrp,dsq))
					{
					ic_ptcpy(ospt,p11);
					ic_ptcpy(osptrp,p12);
					}
*/
				osnap_helper_addPtRec( tmpOsmode, p11, p12, pickptrp, ptAp1rp, ptAp2rp );
				}
			}
		}
	else if (strsame(etype,db_hitype2str(DB_LINE)) || strsame(etype,db_hitype2str(DB_RAY)) || strsame(etype,db_hitype2str(DB_XLINE)))
		{
		ic_assoc(elist,10);
		ic_ptcpy(p1,ic_rbassoc->resval.rpoint);
		ic_assoc(elist,11);
        if (strsame(etype,db_hitype2str(DB_LINE)))
			ic_ptcpy(p2,ic_rbassoc->resval.rpoint);
		else
			{
			p2[0]=p1[0]+ic_rbassoc->resval.rpoint[0];
			p2[1]=p1[1]+ic_rbassoc->resval.rpoint[1];
			p2[2]=p1[2]+ic_rbassoc->resval.rpoint[2];
			}
		if (thickflg)
			{
			ic_ptcpy(p12,p2);
			ic_ptcpy(p11,p1);
			p11[0]+=thickness*p210[0];
			p11[1]+=thickness*p210[1];
			p11[2]+=thickness*p210[2];
			p12[0]+=thickness*p210[0];
			p12[1]+=thickness*p210[1];
			p12[2]+=thickness*p210[2];
			if (m_ecs2ucs)
				{
				ic_usemat(p11,p11,m_ecs2ucs,0,0,0);
				ic_usemat(p12,p12,m_ecs2ucs,0,0,0);
				}
			else
				{
				sds_trans(p11,&rbwcs,&rbucs,0,p11);
				sds_trans(p12,&rbwcs,&rbucs,0,p12);
				}
			}
		if (m_ecs2ucs)
			{
			ic_usemat(p1,p1,m_ecs2ucs,0,0,0);
			ic_usemat(p2,p2,m_ecs2ucs,0,0,0);
			}
		else
			{
			sds_trans(p1,&rbwcs,&rbucs,0,p1);
			sds_trans(p2,&rbwcs,&rbucs,0,p2);
			}
		gr_ucs2rp(p1,p3,view);
		gr_ucs2rp(p2,p4,view);
		if ((osmode & IC_OSMODE_END) && !strsame(etype,db_hitype2str(DB_XLINE)))//END for line & ray
			{
/*			if (SDS_osnap_chkdsq(pickptrp,p3,osptrp,dsq))
				{
				ic_ptcpy(ospt,p1);
				ic_ptcpy(osptrp,p3);
				}
*/
			osnap_helper_addPtRec( IC_OSMODE_END, p1, p3, pickptrp, ptAp1rp, ptAp2rp );
			if ( strsame(etype,db_hitype2str(DB_LINE)) )
				osnap_helper_addPtRec( IC_OSMODE_END, p2, p4, pickptrp, ptAp1rp, ptAp2rp );
/*				SDS_osnap_chkdsq(pickptrp,p4,osptrp,dsq))
				{
				ic_ptcpy(ospt,p2);
				ic_ptcpy(osptrp,p4);
				}
*/
			if (thickflg)
				{
				gr_ucs2rp(p11,p13,view);
				gr_ucs2rp(p12,p14,view);
/*				if (SDS_osnap_chkdsq(pickptrp,p13,osptrp,dsq))
					{
					ic_ptcpy(ospt,p11);
					ic_ptcpy(osptrp,p13);
					}
*/
				osnap_helper_addPtRec( IC_OSMODE_END, p11, p13, pickptrp, ptAp1rp, ptAp2rp );
			    if ( strsame(etype,db_hitype2str(DB_LINE)) )
					osnap_helper_addPtRec( IC_OSMODE_END, p12, p14, pickptrp, ptAp1rp, ptAp2rp );
/*				    SDS_osnap_chkdsq(pickptrp,p14,osptrp,dsq))
					{
					ic_ptcpy(ospt,p12);
					ic_ptcpy(osptrp,p14);
					}
*/
				}
			}
		if (osmode & IC_OSMODE_NEAR)//NEA
			{
			//note: on nearest, do the math in rp because it's what USER SEES
			//	as nearest point
			if (strsame(etype,db_hitype2str(DB_LINE)))
				{
				ic_segdist(pickptrp,p3,p4,0,&fr1,p5);
				fi1=1;
				}
			else
				fi1=ic_ptlndist(pickptrp,p3,p4,&fr1,p5);
				//fi1 is -1,0, or 1
			if (fi1==0)	//RAY or XLINE
				{
				//set fi1 to 2 if it's along p3-p4 direction
				if (p5[0]-p3[0]>0.0)
					{
					if (p4[0]-p3[0]>0.0)
						fi1=2;
					else if (strsame(etype,db_hitype2str(DB_XLINE))) 
						fi1=3;
					}
				else if (p5[0]-p3[0]<0.0)
					{
					if (p4[0]-p3[0]<0.0)
						fi1=2;
					else if (strsame(etype,db_hitype2str(DB_XLINE))) 
						fi1=3;
					}
				else if (p5[1]-p3[1]>0.0)
					{
					if (p4[1]-p3[1]>0.0)
						fi1=2;
					else if (strsame(etype,db_hitype2str(DB_XLINE))) 
						fi1=3;
					}
				else if (p5[1]-p3[1]<0.0)
					{
					if (p4[1]-p3[1]<0.0)
						fi1=2;
					else if (strsame(etype,db_hitype2str(DB_XLINE))) 
						fi1=3;
					}
				else
					goto exit_point;
				}
			if (fi1==0 && strsame(etype,db_hitype2str(DB_RAY)))
				{
				fi1=1;
				ic_ptcpy(p5,p3);
				}
			if (fi1>0)
				{
/*				if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
					{
					ic_ptcpy(osptrp,p5);
					fr1=sds_distance(p3,p5)/sds_distance(p3,p4);
					if (fi1!=3)
						{
						ospt[0]=p1[0]+fr1*(p2[0]-p1[0]);
						ospt[1]=p1[1]+fr1*(p2[1]-p1[1]);
						ospt[2]=p1[2]+fr1*(p2[2]-p1[2]);
						}
					else
						{
						fr1+=1.0;
						ospt[0]=p2[0]+fr1*(p1[0]-p2[0]);
						ospt[1]=p2[1]+fr1*(p1[1]-p2[1]);
						ospt[2]=p2[2]+fr1*(p1[2]-p2[2]);
						}
					}
*/
				fr1=sds_distance(p3,p5)/sds_distance(p3,p4);
				if (fi1!=3)
				{
					ucsPt[0]=p1[0]+fr1*(p2[0]-p1[0]);
					ucsPt[1]=p1[1]+fr1*(p2[1]-p1[1]);
					ucsPt[2]=p1[2]+fr1*(p2[2]-p1[2]);
				}
				else
					{
					fr1+=1.0;
					ucsPt[0]=p2[0]+fr1*(p1[0]-p2[0]);
					ucsPt[1]=p2[1]+fr1*(p1[1]-p2[1]);
					ucsPt[2]=p2[2]+fr1*(p1[2]-p2[2]);
				}
				osnap_helper_addPtRec( IC_OSMODE_NEAR, ucsPt, p5, pickptrp, ptAp1rp, ptAp2rp );
				}
			if (thickflg)
				{
                if (strsame(etype,db_hitype2str(DB_LINE)))
					{
				    ic_segdist(pickptrp,p13,p14,0,&fr1,p15);
				    fi1=1;
					}
				else
				    fi1=ic_ptlndist(pickptrp,p13,p14,&fr1,p15);
				    //fi1 is -1,0, or 1
			    if (fi1==0)	//RAY or XLINE
					{
					//set fi1 to 2 if it's along p3-p4 direction
				    if (p15[0]-p13[0]>0.0)
						{
					    if (p14[0]-p13[0]>0.0)
							fi1=2;
					    else if (strsame(etype,db_hitype2str(DB_XLINE))) 
							fi1=3;
						}
					else if (p15[0]-p13[0]<0.0)
						{
					    if (p14[0]-p13[0]<0.0)
							fi1=2;
					    else if (strsame(etype,db_hitype2str(DB_XLINE))) 
							fi1=3;
						}
					else if (p15[1]-p13[1]>0.0)
						{
					    if (p14[1]-p13[1]>0.0)
							fi1=2;
					    else if (strsame(etype,db_hitype2str(DB_XLINE)))
							fi1=3;
						}
					else if (p15[1]-p13[1]<0.0)
						{
					    if (p14[1]-p13[1]<0.0)
							fi1=2;
					    else if (strsame(etype,db_hitype2str(DB_XLINE))) 
							fi1=3;
						}
					else 
						goto exit_point;
					}
			    if (fi1==0 && strsame(etype,db_hitype2str(DB_RAY)))
					{
					fi1=1;
					ic_ptcpy(p15,p13);
					}
			    if (fi1>0)
					{
/*				    if (SDS_osnap_chkdsq(pickptrp,p15,osptrp,dsq))
						{
					    ic_ptcpy(osptrp,p15);
					    fr1=sds_distance(p13,p15)/sds_distance(p13,p14);
					    if (fi1!=3)
							{
						    ospt[0]=p11[0]+fr1*(p12[0]-p11[0]);
						    ospt[1]=p11[1]+fr1*(p12[1]-p11[1]);
						    ospt[2]=p11[2]+fr1*(p12[2]-p11[2]);
							}
						else
							{
						    fr1+=1.0;
						    ospt[0]=p12[0]+fr1*(p11[0]-p12[0]);
						    ospt[1]=p12[1]+fr1*(p11[1]-p12[1]);
						    ospt[2]=p12[2]+fr1*(p11[2]-p12[2]);
							}
						}
*/
					fr1=sds_distance(p13,p15)/sds_distance(p13,p14);
					if (fi1!=3)
					{
						ucsPt[0]=p11[0]+fr1*(p12[0]-p11[0]);
						ucsPt[1]=p11[1]+fr1*(p12[1]-p11[1]);
						ucsPt[2]=p11[2]+fr1*(p12[2]-p11[2]);
					}
					else
					{
					    fr1+=1.0;
					    ucsPt[0]=p12[0]+fr1*(p11[0]-p12[0]);
					    ucsPt[1]=p12[1]+fr1*(p11[1]-p12[1]);
					    ucsPt[2]=p12[2]+fr1*(p11[2]-p12[2]);
					}
					osnap_helper_addPtRec( IC_OSMODE_NEAR, ucsPt, p15, pickptrp, ptAp1rp, ptAp2rp );
					}
				//now check end cap of line or ray for nearest
				if (!strsame(etype,db_hitype2str(DB_XLINE)))//xlines have no end cap
					{
					if (1==ic_ptlndist(pickptrp,p3,p13,&fr5,p5))
						{
/*						if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
							{
							ic_ptcpy(osptrp,p5);
							fr5=sds_distance(p3,p5)/sds_distance(p3,p13);
							ospt[0]=p1[0]+fr5*(p11[0]-p1[0]);
							ospt[1]=p1[1]+fr5*(p11[1]-p1[1]);
							ospt[2]=p1[2]+fr5*(p11[2]-p1[2]);
							}
*/
						fr5=sds_distance(p3,p5)/sds_distance(p3,p13);
						ucsPt[0]=p1[0]+fr5*(p11[0]-p1[0]);
						ucsPt[1]=p1[1]+fr5*(p11[1]-p1[1]);
						ucsPt[2]=p1[2]+fr5*(p11[2]-p1[2]);
						osnap_helper_addPtRec( IC_OSMODE_NEAR, ucsPt, p5, pickptrp, ptAp1rp, ptAp2rp );
						}
					if (strsame(etype,db_hitype2str(DB_LINE)))
						{
/*						if (1==ic_ptlndist(pickptrp,p4,p14,&fr5,p5))
							{
							ic_ptcpy(osptrp,p5);
							fr5=sds_distance(p4,p5)/sds_distance(p4,p14);
							ospt[0]=p2[0]+fr5*(p12[0]-p2[0]);
							ospt[1]=p2[1]+fr5*(p12[1]-p2[1]);
							ospt[2]=p2[2]+fr5*(p12[2]-p2[2]);
							}
*/
						fr5=sds_distance(p4,p5)/sds_distance(p4,p14);
						ucsPt[0]=p2[0]+fr5*(p12[0]-p2[0]);
						ucsPt[1]=p2[1]+fr5*(p12[1]-p2[1]);
						ucsPt[2]=p2[2]+fr5*(p12[2]-p2[2]);
						osnap_helper_addPtRec( IC_OSMODE_NEAR, ucsPt, p5, pickptrp, ptAp1rp, ptAp2rp );
						}
					}
				}
			}
		// IC_OSMODE_TAN should be removed because it is not only tangency for lines
		// does not make any sense but also the implementation below is for finding an
		// perpendicular point. SWH, Sept 24, 2003
		//	if (osmode & (IC_OSMODE_PERP | IC_OSMODE_TAN))//PER or TAN
		if (osmode & IC_OSMODE_PERP)//PER 
		{
			tmpOsmode = osmode & IC_OSMODE_PERP;
			if( !tmpOsmode )
				tmpOsmode = IC_OSMODE_TAN;

			//note: on perpendicular, we must do math in UCS
			fi1=ic_ptlndist(lastpt,p1,p2,&fr1,p5);
			gr_ucs2rp(lastpt,p7,view);
			double denominator;
			if (thickflg)
			{
                fi1=ic_ptlndist(lastpt,p11,p12,&fr1,p15);
			    if (fi1>-1)
				{
					fr2 = 1.0;
					denominator = sds_dist2d(p11,p12);
					if (fabs(denominator)>0)
						fr2=sds_dist2d(p11,p15)/denominator;

				    // On next line, include (fi1 ||) for speed
				    if (fi1 || icadDirectionEqual(p11,p12,p11,p15))
					    p15[2]=p11[2]+(p12[2]-p11[2])*fr2;
				    else
					    p15[2]=p12[2]+(p11[2]-p12[2])*(fr2+1.0);

				    gr_ucs2rp(p15,p16,view);
					osnap_helper_addPtRec( tmpOsmode, p15, p16, pickptrp, ptAp1rp, ptAp2rp );
				}
			}
			if (fi1>-1)
			{
				fr2 = 0.0;
				denominator = sds_dist2d(p13,p14);
				if (fabs(denominator)>0)
					fr2=sds_dist2d(p13,p7)/denominator;

				// On next line, include (fi1 ||) for speed
				if (fi1 || icadDirectionEqual(p1,p2,p1,p5))
					p5[2]=p1[2]+(p2[2]-p1[2])*fr2;
				else
					p5[2]=p2[2]+(p1[2]-p2[2])*(fr2+1.0);

				gr_ucs2rp(p5,p6,view);
				osnap_helper_addPtRec( tmpOsmode, p5, p6, pickptrp, ptAp1rp, ptAp2rp );
			}
		}
		if (osmode & IC_OSMODE_MID)//MID
			{
			if (strsame(etype,db_hitype2str(DB_LINE)))
				{
				p3[0]=(p1[0]+p2[0])/2.0;
				p3[1]=(p1[1]+p2[1])/2.0;
				p3[2]=(p1[2]+p2[2])/2.0;
				gr_ucs2rp(p3,p5,view);
/*				if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
					{
					ic_ptcpy(ospt,p3);
					ic_ptcpy(osptrp,p5);
					}
*/
				osnap_helper_addPtRec( IC_OSMODE_MID, p3, p5, pickptrp, ptAp1rp, ptAp2rp );
				if (thickflg)
					{
					p13[0]=(p11[0]+p12[0])/2.0;
					p13[1]=(p11[1]+p12[1])/2.0;
					p13[2]=(p11[2]+p12[2])/2.0;
					gr_ucs2rp(p13,p15,view);
/*					if (SDS_osnap_chkdsq(pickptrp,p15,osptrp,dsq))
						{
						ic_ptcpy(ospt,p13);
						ic_ptcpy(osptrp,p15);
						}
*/
				p13[0]=(p11[0]+p12[0])/2.0;
				p13[1]=(p11[1]+p12[1])/2.0;
				p13[2]=(p11[2]+p12[2])/2.0;
				osnap_helper_addPtRec( IC_OSMODE_MID, p13, p15, pickptrp, ptAp1rp, ptAp2rp );
//	AG: I commented out the following two cases thinking it's not appropriate
/*					p13[0]=(p11[0]+p1[0])/2.0;
					p13[1]=(p11[1]+p1[1])/2.0;
					p13[2]=(p11[2]+p1[2])/2.0;
					gr_ucs2rp(p13,p15,view);
//*					if (SDS_osnap_chkdsq(pickptrp,p15,osptrp,dsq))
						{
						ic_ptcpy(ospt,p13);
						ic_ptcpy(osptrp,p15);
						}
////
					osnap_helper_addPtRec( IC_OSMODE_MID, p13, p15, pickptrp/*, recArray );
					p13[0]=(p2[0]+p12[0])/2.0;
					p13[1]=(p2[1]+p12[1])/2.0;
					p13[2]=(p2[2]+p12[2])/2.0;
					gr_ucs2rp(p13,p15,view);
//*					if (SDS_osnap_chkdsq(pickptrp,p15,osptrp,dsq))
						{
						ic_ptcpy(ospt,p13);
						ic_ptcpy(osptrp,p15);
						}
///
					osnap_helper_addPtRec( IC_OSMODE_MID, p13, p15, pickptrp/*, recArray );
*/
					}
				}
			else if (strsame(etype,db_hitype2str(DB_XLINE)))//yes, it's possible
				{
				gr_ucs2rp(p1,p5,view);
/*				if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
					{
					ic_ptcpy(ospt,p1);
					ic_ptcpy(osptrp,p5);
					}
*/
				osnap_helper_addPtRec( IC_OSMODE_MID, p1, p5, pickptrp, ptAp1rp, ptAp2rp );
				if (thickflg)
					{
					gr_ucs2rp(p11,p15,view);
/*					if (SDS_osnap_chkdsq(pickptrp,p15,osptrp,dsq))
						{
						ic_ptcpy(ospt,p11);
						ic_ptcpy(osptrp,p15);

						}
*/
					osnap_helper_addPtRec( IC_OSMODE_MID, p11, p15, pickptrp, ptAp1rp, ptAp2rp );
					}
				}
			}
		}
	else if (strsame(etype,db_hitype2str(DB_ARC)) || strsame(etype,db_hitype2str(DB_CIRCLE)))
		{
		fr2=fr3=rbent.resval.rpoint[0]=rbent.resval.rpoint[1]=0.0;
		rbent.resval.rpoint[2]=1.0;
		rbent.restype=RT3DPOINT;
		for (rbtemp=elist;rbtemp!=NULL;rbtemp=rbtemp->rbnext)
			{
			if (rbtemp->restype==10)
				ic_ptcpy(p1,rbtemp->resval.rpoint);
			else if (rbtemp->restype==40)
				fr1=rbtemp->resval.rreal;
			else if (rbtemp->restype==50)
				fr2=rbtemp->resval.rreal;
			else if (rbtemp->restype==51)
				fr3=rbtemp->resval.rreal;
			else if (rbtemp->restype==210)
				ic_ptcpy(rbent.resval.rpoint,rbtemp->resval.rpoint);
			}
		if (thickflg)
			{
            ic_ptcpy(p11,p1);
            p11[0]+=thickness*p210[0];
            p11[1]+=thickness*p210[1];
            p11[2]+=thickness*p210[2];
			}
        
        ic_normang(&fr2,&fr3);
		if ((osmode & IC_OSMODE_END)&&(strsame(etype,db_hitype2str(DB_ARC))))//END
			{
			sds_polar(p1,fr2,fr1,p2);
			if (m_ecs2ucs)
				ic_usemat(p2,p4,m_ecs2ucs,0,0,0);
			else
				sds_trans(p2,&rbent,&rbucs,0,p4);
			gr_ucs2rp(p4,p2,view);
/*			if (SDS_osnap_chkdsq(pickptrp,p2,osptrp,dsq))
				{
				ic_ptcpy(ospt,p4);
				ic_ptcpy(osptrp,p2);
				}
*/
			osnap_helper_addPtRec( IC_OSMODE_END, p4, p2, pickptrp, ptAp1rp, ptAp2rp );
			sds_polar(p1,fr3,fr1,p2);
			if (m_ecs2ucs)
				ic_usemat(p2,p4,m_ecs2ucs,0,0,0);
			else
				sds_trans(p2,&rbent,&rbucs,0,p4);
			gr_ucs2rp(p4,p2,view);
/*			if (SDS_osnap_chkdsq(pickptrp,p2,osptrp,dsq))
				{
				ic_ptcpy(ospt,p4);
				ic_ptcpy(osptrp,p2);
				}
*/
			osnap_helper_addPtRec( IC_OSMODE_END, p4, p2, pickptrp, ptAp1rp, ptAp2rp );
			if (thickflg)
				{
                sds_polar(p11,fr2,fr1,p12);
				if (m_ecs2ucs)
					ic_usemat(p12,p14,m_ecs2ucs,0,0,0);
				else
					sds_trans(p12,&rbent,&rbucs,0,p14);
			    gr_ucs2rp(p14,p12,view);
/*			    if (SDS_osnap_chkdsq(pickptrp,p12,osptrp,dsq))
					{
					ic_ptcpy(ospt,p14);
					ic_ptcpy(osptrp,p12);
					}
*/
				osnap_helper_addPtRec( IC_OSMODE_END, p14, p12, pickptrp, ptAp1rp, ptAp2rp );
			    sds_polar(p11,fr3,fr1,p12);
				if (m_ecs2ucs)
					ic_usemat(p12,p14,m_ecs2ucs,0,0,0);
				else
					sds_trans(p12,&rbent,&rbucs,0,p14);
			    gr_ucs2rp(p14,p12,view);
/*			    if (SDS_osnap_chkdsq(pickptrp,p12,osptrp,dsq))
					{
					ic_ptcpy(ospt,p14);
					ic_ptcpy(osptrp,p12);
					}
*/
				osnap_helper_addPtRec( IC_OSMODE_END, p14, p12, pickptrp, ptAp1rp, ptAp2rp );
			    }
			}
		//don't do nea & per together - per uses lastpt
		if (osmode & IC_OSMODE_NEAR)//NEA
			{
			if (m_ecs2ucs)
				ic_usemat(pickptucs,p3,m_ecs2ucs,0,0,1);
			else
				sds_trans(pickptucs,&rbucs,&rbent,0,p3);
			SDS_getvar(NULL,DB_QVIEWDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			if (m_ecs2ucs)
				ic_usemat(rb.resval.rpoint,p5,m_ecs2ucs,1,0,1);
			else
				sds_trans(rb.resval.rpoint,&rbucs,&rbent,1,p5);
			if (fabs(p5[2])<IC_ZRO)
				goto exit_point;
			p4[2]=p1[2];
			p4[0]=p3[0]+((p1[2]-p3[2])/p5[2])*p5[0];
			p4[1]=p3[1]+((p1[2]-p3[2])/p5[2])*p5[1];
			//p4 now represents the picked point in ECS system!
			p2[0]=fr1;
			p2[1]=fr2;
			p2[2]=fr3;
			//now take p4 & snap it to the arc
			if (strsame(etype,db_hitype2str(DB_ARC)))
				ic_segdist(p4,p1,p2,1,&fr4,p3);
			else
				ic_segdist(p4,p1,p2,2,&fr4,p3);
			if (m_ecs2ucs)
				ic_usemat(p3,p2,m_ecs2ucs,0,0,0);
			else
				sds_trans(p3,&rbent,&rbucs,0,p2);
			gr_ucs2rp(p2,p3,view);
/*			if (SDS_osnap_chkdsq(pickptrp,p3,osptrp,dsq))
			{
				ic_ptcpy(ospt,p2);
				ic_ptcpy(osptrp,p3);
			}
*/
			osnap_helper_addPtRec( IC_OSMODE_NEAR, p2, p3, pickptrp, ptAp1rp, ptAp2rp );
			if (thickflg)
				{
				if (m_ecs2ucs)
					ic_usemat(pickptucs,p13,m_ecs2ucs,0,0,1);
				else
					sds_trans(pickptucs,&rbucs,&rbent,0,p13);
			    SDS_getvar(NULL,DB_QVIEWDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			    sds_trans(rb.resval.rpoint,&rbucs,&rbent,1,p15);
			    if (fabs(p15[2])<IC_ZRO)
					goto exit_point;
			    p14[2]=p11[2];
			    p14[0]=p13[0]+((p11[2]-p13[2])/p15[2])*p15[0];
			    p14[1]=p13[1]+((p11[2]-p13[2])/p15[2])*p15[1];
			    //p14 now represents the picked point in ECS system!
			    p12[0]=fr1;
			    p12[1]=fr2;
			    p12[2]=fr3;
			    //now take p14 & snap it to the arc
			    if (strsame(etype,db_hitype2str(DB_ARC)))
				    ic_segdist(p14,p11,p12,1,&fr4,p13);
			    else
				    ic_segdist(p14,p11,p12,2,&fr4,p13);
				if (m_ecs2ucs)
					ic_usemat(p13,p12,m_ecs2ucs,0,0,0);
				else
					sds_trans(p13,&rbent,&rbucs,0,p12);
			    gr_ucs2rp(p12,p13,view);
/*			    if (SDS_osnap_chkdsq(pickptrp,p13,osptrp,dsq))
					{
					ic_ptcpy(ospt,p12);
					ic_ptcpy(osptrp,p13);
					}
*/
				osnap_helper_addPtRec( IC_OSMODE_NEAR, p12, p13, pickptrp, ptAp1rp, ptAp2rp );

				// AG: I commented out the following two cases thinking it's not appropriate
/*			    //now take p14 & snap it to the arc
				if (strsame(etype,db_hitype2str(DB_ARC)))
					{
					sds_polar(p11,fr1,fr2,p13);
					sds_polar(p1,fr1,fr2,p3);
					//test along edge of start
					gr_ucs2rp(p13,p17,view);
					gr_ucs2rp(p3,p6,view);
					ic_segdist(pickptrp,p17,p6,0,NULL,p7);
//*					if (SDS_osnap_chkdsq(pickptrp,p7,osptrp,dsq))
						{
						ic_ptcpy(osptrp,p7);
						fr5=sds_distance(p17,p7)/sds_distance(p17,p6);
						ospt[0]=p13[0]+(fr5*(p3[0]-p13[0]));
						ospt[1]=p13[1]+(fr5*(p3[1]-p13[1]));
						ospt[2]=p13[2]+(fr5*(p3[2]-p13[2]));
						}
///
					osnap_helper_addPtRec( IC_OSMODE_NEAR, p7, pickptrp/*, recArray );
					sds_polar(p11,fr1,fr3,p14);
					sds_polar(p1,fr1,fr3,p4);
					//test along edge of end
					gr_ucs2rp(p14,p17,view);
					gr_ucs2rp(p4,p6,view);
					ic_segdist(pickptrp,p17,p6,0,NULL,p7);
//*					if (SDS_osnap_chkdsq(pickptrp,p7,osptrp,dsq))
						{
						ic_ptcpy(osptrp,p7);
						fr5=sds_distance(p17,p7)/sds_distance(p17,p6);
						ospt[0]=p14[0]+(fr5*(p4[0]-p14[0]));
						ospt[1]=p14[1]+(fr5*(p4[1]-p14[1]));
						ospt[2]=p14[2]+(fr5*(p4[2]-p14[2]));
						}
///
					osnap_helper_addPtRec( IC_OSMODE_NEAR, p7, pickptrp/*, recArray );

					}
*/
			    }
		    }

		if ((osmode & (IC_OSMODE_PERP | IC_OSMODE_NEAR))==IC_OSMODE_PERP)//PER must be calc'd in ECS
			{
            if (m_ecs2ucs) {
				ic_usemat(lastpt,p5,m_ecs2ucs,0,0,1);
                ic_ptcpy(p6, pickptucs);
            }
            else {
				sds_trans(lastpt,&rbucs,&rbent,0,p5);
                sds_trans(pickptucs,&rbucs,&rbent,0,p6);
            }
            double rPick;
            bool bSingleCandidate = true;
            if (icadPointEqual(p1, p5)) {
			    fr4 = sds_angle(p1,p6);
			    ic_normang(&fr2,&fr4);
            }
            else {
			    fr4=sds_angle(p1,p5);
			    ic_normang(&fr2,&fr4);

			    rPick = sds_angle(p1,p6);
			    ic_normang(&fr2, &rPick);

                bSingleCandidate = false;
            }
;
			if (strsame(etype,db_hitype2str(DB_CIRCLE)) || (fr4>=fr2 && fr4<=fr3))
				{
				sds_polar(p1,fr4,fr1,p3);

                if (!bSingleCandidate) {      
                    double rCandidate1 = fr4;
                    double rCandidate2 = rCandidate1 + PI;
                    ic_normang(&fr2, &rCandidate2);
                    if (strsame(etype,db_hitype2str(DB_CIRCLE)) || rCandidate2 >= fr2 && rCandidate2 <= fr3) {
                        double dis11 = fabs(rCandidate1 - rPick);
                        double dis12 = TWOPI - dis11;
                        double dis1 = MIN(dis11, dis12);

                        dis11 = fabs(rCandidate2 - rPick);
                        dis12 = TWOPI - dis11;
                        double dis2 = MIN(dis11, dis12);
                        // The second point is more closed to the pick point
                        if (dis2 < dis1) 
                            sds_polar(p1, rCandidate2,fr1,p3);
                    }
                }

				if (m_ecs2ucs)
					ic_usemat(p3,p4,m_ecs2ucs,0,0,0);
				else
					sds_trans(p3,&rbent,&rbucs,0,p4);
				gr_ucs2rp(p4,p3,view);
/*				if (SDS_osnap_chkdsq(pickptrp,p3,osptrp,dsq))
					{
					ic_ptcpy(ospt,p4);
					ic_ptcpy(osptrp,p3);
					}
*/
				osnap_helper_addPtRec( IC_OSMODE_PERP, p4, p3, pickptrp, ptAp1rp, ptAp2rp );
			    if (thickflg)
					{
					if (m_ecs2ucs)
						ic_usemat(lastpt,p15,m_ecs2ucs,0,0,1);
					else
						sds_trans(lastpt,&rbucs,&rbent,0,p15);
			        fr4=sds_angle(p11,p15);
			        ic_normang(&fr2,&fr4);
			        if (strsame(etype,db_hitype2str(DB_CIRCLE)) || (fr4>=fr2 && fr4<=fr3))
						{
				        sds_polar(p11,fr4,fr1,p13);
						if (m_ecs2ucs)
							ic_usemat(p13,p14,m_ecs2ucs,0,0,0);
						else
							sds_trans(p13,&rbent,&rbucs,0,p14);
				        gr_ucs2rp(p14,p13,view);
/*				        if (SDS_osnap_chkdsq(pickptrp,p13,osptrp,dsq))
							{
							ic_ptcpy(ospt,p14);
							ic_ptcpy(osptrp,p13);
							}
*/
						osnap_helper_addPtRec( IC_OSMODE_PERP, p14, p13, pickptrp, ptAp1rp, ptAp2rp );
						}
					}
				}
			}
		if (osmode & IC_OSMODE_QUADRANT)//QUA
			{
			//test if entity is parallel to current ucs...
			p7[0]=p7[1]=1.0;
			p7[2]=0.0;
			sds_trans(p7,&rbent,&rbucs,1,p7);
			if (icadRealEqual(p7[2],0.0))
				{
				p2[0]=1.0;
				p2[1]=p2[2]=0.0;
				sds_trans(p2,&rbent,&rbucs,1,p2);
				fr5=atan2(p2[1],p2[0]);
				for (fi1=0;fi1<4;fi1++)
					{
					fr4=(fi1*IC_PI/2.0)-fr5;
					ic_normang(&fr2,&fr4);
					if (strsame(etype,db_hitype2str(DB_CIRCLE)) || fr4<=fr3)
						{
						ic_ptcpy(p6,p1);
						p7[0]=sin(fr4);
						p7[1]=cos(fr4);
						p6[0]+=fr1*p7[1];
						p6[1]+=fr1*p7[0];
						if (m_ecs2ucs)
							ic_usemat(p6,p7,m_ecs2ucs,0,0,0);
						else
							sds_trans(p6,&rbent,&rbucs,0,p7);
						gr_ucs2rp(p7,p5,view);
/*						if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
							{
							ic_ptcpy(ospt,p7);
							ic_ptcpy(osptrp,p5);
							}
*/
						osnap_helper_addPtRec( IC_OSMODE_QUADRANT, p7, p5, pickptrp, ptAp1rp, ptAp2rp );
						if (thickflg)
							{
							p6[0]+=thickness*p210[0];
							p6[1]+=thickness*p210[1];
							p6[2]+=thickness*p210[2];
							if (m_ecs2ucs)
								ic_usemat(p6,p7,m_ecs2ucs,0,0,0);
							else
								sds_trans(p6,&rbent,&rbucs,0,p7);
// AG: The next 2 lines to enable another 4 points for this mode
							gr_ucs2rp( p7, p5, view );
							osnap_helper_addPtRec( IC_OSMODE_QUADRANT, p7, p5, pickptrp, ptAp1rp, ptAp2rp );
							}
						}//if
					}//for
				}
			}
		if (osmode & IC_OSMODE_CENTER)//CEN
			{
			if (m_ecs2ucs)
				ic_usemat(p1,p4,m_ecs2ucs,0,0,0);
			else
				sds_trans(p1,&rbent,&rbucs,0,p4);
			gr_ucs2rp(p4,p5,view);
/*			if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
				{
				ic_ptcpy(ospt,p4);
				ic_ptcpy(osptrp,p5);
				}
*/
			osnap_helper_addPtRec( IC_OSMODE_CENTER, p4, p5, pickptrp, ptAp1rp, ptAp2rp );
			if (thickflg)
				{
				if (m_ecs2ucs)
					ic_usemat(p11,p14,m_ecs2ucs,0,0,0);
				else
					sds_trans(p11,&rbent,&rbucs,0,p14);
			    gr_ucs2rp(p14,p15,view);
/*			    if (SDS_osnap_chkdsq(pickptrp,p15,osptrp,dsq))
					{
					ic_ptcpy(ospt,p14);
					ic_ptcpy(osptrp,p15);
					}
*/
				osnap_helper_addPtRec( IC_OSMODE_CENTER, p14, p15, pickptrp, ptAp1rp, ptAp2rp );
				}
			}
		if ((osmode & IC_OSMODE_MID) && strsame(etype,db_hitype2str(DB_ARC))) //MID
			{
			fr4=(fr2+fr3)/2.0;
			sds_polar(p1,fr4,fr1,p4);
			if (m_ecs2ucs)
				ic_usemat(p4,p5,m_ecs2ucs,0,0,0);
			else
				sds_trans(p4,&rbent,&rbucs,0,p5);
			gr_ucs2rp(p5,p4,view);
/*			if (SDS_osnap_chkdsq(pickptrp,p4,osptrp,dsq))
				{
				ic_ptcpy(ospt,p5);
				ic_ptcpy(osptrp,p4);
				}
*/
			osnap_helper_addPtRec( IC_OSMODE_MID, p5, p4, pickptrp, ptAp1rp, ptAp2rp );
			if (thickflg)
				{
                sds_polar(p11,fr4,fr1,p14);
				if (m_ecs2ucs)
					ic_usemat(p14,p15,m_ecs2ucs,0,0,0);
				else
					sds_trans(p14,&rbent,&rbucs,0,p15);
			    gr_ucs2rp(p15,p14,view);
/*			    if (SDS_osnap_chkdsq(pickptrp,p14,osptrp,dsq))
					{
					ic_ptcpy(ospt,p15);
					ic_ptcpy(osptrp,p14);
					}
*/
				osnap_helper_addPtRec( IC_OSMODE_MID, p15, p14, pickptrp, ptAp1rp, ptAp2rp );
				}
			}
		if (osmode & IC_OSMODE_TAN)//TAN
			{
			if (m_ecs2ucs)
				ic_usemat(lastpt,p5,m_ecs2ucs,0,0,1);
			else
				sds_trans(lastpt,&rbucs,&rbent,0,p5);

			fr4=sds_dist2d(p1,p5);
			if (fr4<=fr1)
				{
				fr5=sds_angle(p1,p5);
				ic_normang(&fr2,&fr5);
				if (strsame(etype,db_hitype2str(DB_CIRCLE)) || (fr5>=fr2 && fr5<=fr3))
					{
					sds_polar(p1,fr5,fr1,p2);
					if (m_ecs2ucs)
						ic_usemat(p2,p3,m_ecs2ucs,0,0,0);
					else
						sds_trans(p2,&rbent,&rbucs,0,p3);
					gr_ucs2rp(p3,p4,view);
/*					if (SDS_osnap_chkdsq(pickptrp,p4,osptrp,dsq))
						{
						ic_ptcpy(ospt,p3);
						ic_ptcpy(osptrp,p4);
						}
*/
					osnap_helper_addPtRec( IC_OSMODE_TAN, p3, p4, pickptrp, ptAp1rp, ptAp2rp );
					}
				}
			else
				{
				//get dist from lastpt to tangent pt
				//fr5=sqrt((fr4*fr4)-(fr1*fr1)); //this is nonsense math.  fr4= distance between circle centers, fr1= radius of circle1

				if (SDS_CMainWindow->m_pDragVarsCur && (SDS_CMainWindow->m_pDragVarsCur->mode == 1))
					{
					//fr5, radiusof new circle has 2 solutions : fr4-fr1 or fr4+fr1, depending on position of osptrp AM 11/17/98
					//calculate the dot product (c-a).(b-a) where a,b are circle centers and c is osnap point  
					double dotp = (osptrp[0]-p1[0])*(p5[0]-p1[0]) + (osptrp[1]-p1[1])*(p5[1]-p1[1]) + (osptrp[2]-p1[2])*(p5[2]-p1[2]);
					if (dotp < 0.0)	
						fr5 = fr4 + fr1;
					else
						fr5 = fr4 - fr1;
					}
				else
					fr5=sqrt((fr4*fr4)-(fr1*fr1));

				fi1=ic_arcxarc(p1,fr1,fr2,fr3,p5,fr5,0.0,IC_PI,p2,p3);
				p2[2]=p3[2]=p1[2];
				if (fi1>=0 && strsame(etype,db_hitype2str(DB_ARC)))
					{
					if (fi1&1)
						{
						if (m_ecs2ucs)
							ic_usemat(p2,p4,m_ecs2ucs,0,0,0);
						else
							sds_trans(p2,&rbent,&rbucs,0,p4);
						gr_ucs2rp(p4,p5,view);
/*						if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
							{
							ic_ptcpy(ospt,p4);
							ic_ptcpy(osptrp,p5);
							}
*/
						osnap_helper_addPtRec( IC_OSMODE_TAN, p4, p5, pickptrp, ptAp1rp, ptAp2rp );
						}
					if (fi1&4)
						{
						if (m_ecs2ucs)
							ic_usemat(p3,p4,m_ecs2ucs,0,0,0);
						else
							sds_trans(p3,&rbent,&rbucs,0,p4);
						gr_ucs2rp(p4,p5,view);
/*						if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
							{
							ic_ptcpy(ospt,p4);
							ic_ptcpy(osptrp,p5);
							}
*/
						osnap_helper_addPtRec( IC_OSMODE_TAN, p4, p5, pickptrp, ptAp1rp, ptAp2rp );
						}
					}
				else if (fi1>=0 && strsame(etype,db_hitype2str(DB_CIRCLE)))
					{
					if (m_ecs2ucs)
						ic_usemat(p2,p4,m_ecs2ucs,0,0,0);
					else
						sds_trans(p2,&rbent,&rbucs,0,p4);
					gr_ucs2rp(p4,p5,view);
/*					if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
						{
						ic_ptcpy(ospt,p4);
						ic_ptcpy(osptrp,p5);
						}
*/
					osnap_helper_addPtRec( IC_OSMODE_TAN, p4, p5, pickptrp, ptAp1rp, ptAp2rp );
					if (m_ecs2ucs)
						ic_usemat(p3,p4,m_ecs2ucs,0,0,0);
					else
						sds_trans(p3,&rbent,&rbucs,0,p4);
					gr_ucs2rp(p4,p5,view);
/*					if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
						{
						ic_ptcpy(ospt,p4);
						ic_ptcpy(osptrp,p5);
						}
*/
					osnap_helper_addPtRec( IC_OSMODE_TAN, p4, p5, pickptrp, ptAp1rp, ptAp2rp );
					}
				}
			if (thickflg)
				{
				if (m_ecs2ucs)
					ic_usemat(lastpt,p15,m_ecs2ucs,0,0,1);
				else
					sds_trans(lastpt,&rbucs,&rbent,0,p15);
			    fr4=sds_dist2d(p11,p15);
			    if (fr4<=fr1)
					{
				    fr5=sds_angle(p11,p15);
				    ic_normang(&fr2,&fr5);
				    if ((strsame(etype,db_hitype2str(DB_CIRCLE)))||((fr5>=fr2)&&(fr5<=fr3))){
					    sds_polar(p11,fr5,fr1,p12);
						if (m_ecs2ucs)
							ic_usemat(p12,p13,m_ecs2ucs,0,0,0);
						else
							sds_trans(p12,&rbent,&rbucs,0,p13);
					    gr_ucs2rp(p13,p14,view);
/*					    if (SDS_osnap_chkdsq(pickptrp,p14,osptrp,dsq))
							{
							ic_ptcpy(ospt,p13);
							ic_ptcpy(osptrp,p14);
							}
*/
						osnap_helper_addPtRec( IC_OSMODE_TAN, p13, p14, pickptrp, ptAp1rp, ptAp2rp );
						}
					}
				else
					{
				    //get dist from lastpt to tangent pt
				    fr5=sqrt((fr4*fr4)-(fr1*fr1));
				    fi1=ic_arcxarc(p11,fr1,fr2,fr3,p15,fr5,0.0,IC_PI,p12,p13);
					p12[2]=p13[2]=p11[2];
				    if (fi1>=0 && strsame(etype,db_hitype2str(DB_ARC)))
						{
					    if (fi1&1)
							{
							if (m_ecs2ucs)
								ic_usemat(p12,p14,m_ecs2ucs,0,0,0);
							else
								sds_trans(p12,&rbent,&rbucs,0,p14);
						    gr_ucs2rp(p14,p15,view);
/*						    if (SDS_osnap_chkdsq(pickptrp,p15,osptrp,dsq))
								{
								ic_ptcpy(ospt,p14);
								ic_ptcpy(osptrp,p15);
								}
*/
					osnap_helper_addPtRec( IC_OSMODE_TAN, p14, p15, pickptrp, ptAp1rp, ptAp2rp );
							}
					    if (fi1&4)
							{
							if (m_ecs2ucs)
								ic_usemat(p13,p14,m_ecs2ucs,0,0,0);
							else
								sds_trans(p13,&rbent,&rbucs,0,p14);
						    gr_ucs2rp(p14,p15,view);
/*						    if (SDS_osnap_chkdsq(pickptrp,p15,osptrp,dsq))
								{
								ic_ptcpy(ospt,p14);
								ic_ptcpy(osptrp,p15);
								}
*/
							osnap_helper_addPtRec( IC_OSMODE_TAN, p14, p15, pickptrp, ptAp1rp, ptAp2rp );
							}
						}
					else if (fi1>=0 && strsame(etype,db_hitype2str(DB_CIRCLE)))
						{
						if (m_ecs2ucs)
							ic_usemat(p12,p14,m_ecs2ucs,0,0,0);
						else
							sds_trans(p12,&rbent,&rbucs,0,p14);
					    gr_ucs2rp(p14,p15,view);
/*					    if (SDS_osnap_chkdsq(pickptrp,p15,osptrp,dsq))
							{
							ic_ptcpy(ospt,p14);
							ic_ptcpy(osptrp,p15);
							}
*/
						osnap_helper_addPtRec( IC_OSMODE_TAN, p14, p15, pickptrp, ptAp1rp, ptAp2rp );
						if (m_ecs2ucs)
							ic_usemat(p13,p14,m_ecs2ucs,0,0,0);
						else
							sds_trans(p13,&rbent,&rbucs,0,p14);
					    gr_ucs2rp(p14,p15,view);
/*					    if (SDS_osnap_chkdsq(pickptrp,p15,osptrp,dsq))
							{
							ic_ptcpy(ospt,p14);
							ic_ptcpy(osptrp,p15);
							}
*/
						osnap_helper_addPtRec( IC_OSMODE_TAN, p14, p15, pickptrp, ptAp1rp, ptAp2rp );
						}
					}
				}
			}
		}
	else if (strsame(etype,db_hitype2str(DB_ELLIPSE)))
		{
		p5[0]=p5[1]=0.0;p5[2]=1.0;
		for (rbtemp=elist;rbtemp!=NULL;rbtemp=rbtemp->rbnext)
			{
			if (rbtemp->restype==10)
				ic_ptcpy(p1,rbtemp->resval.rpoint);		// center
			else if (rbtemp->restype==11)
				ic_ptcpy(p2,rbtemp->resval.rpoint);		// major axis
			else if (rbtemp->restype==40)
				fr1=rbtemp->resval.rreal;				// the ratio of major/minor axes
			else if (rbtemp->restype==41)
				fr2=rbtemp->resval.rreal;				// start parameter
			else if (rbtemp->restype==42)
				fr3=rbtemp->resval.rreal;				// end parameter
			else if (rbtemp->restype==210)
				ic_ptcpy(p5,rbtemp->resval.rpoint);		// extrusion direction
			}
		// minor axis 
		p6[0]=fr1*(p5[1]*p2[2]-p2[1]*p5[2]);
		p6[1]=fr1*(p5[2]*p2[0]-p2[2]*p5[0]);
		p6[2]=fr1*(p5[0]*p2[1]-p2[0]*p5[1]);

		if (!icadRealEqual(fr2,0.0) || !icadRealEqual(fr3,IC_TWOPI))
			ic_normang(&fr2,&fr3);

        if (osmode & IC_OSMODE_END)//END
			{
			if (!icadRealEqual(fr3-IC_TWOPI,fr2))
				{
				//check starting point
				fr4=cos(fr2);
				fr5=sin(fr2);

				p3[0]=p1[0]+p2[0]*fr4+p6[0]*fr5;
				p3[1]=p1[1]+p2[1]*fr4+p6[1]*fr5;
				p3[2]=p1[2]+p2[2]*fr4+p6[2]*fr5;

				if (m_ecs2ucs)
					ic_usemat(p3,p4,m_ecs2ucs,0,0,0);
				else
					sds_trans(p3,&rbwcs,&rbucs,0,p4);
				gr_ucs2rp(p4,p3,view);
/*				if (SDS_osnap_chkdsq(pickptrp,p3,osptrp,dsq))
					{
					ic_ptcpy(ospt,p4);
					ic_ptcpy(osptrp,p3);
					}
*/
				osnap_helper_addPtRec( IC_OSMODE_END, p4, p3, pickptrp, ptAp1rp, ptAp2rp );

                //check ending point
				fr4=cos(fr3);
				fr5=sin(fr3);

				p3[0]=p1[0]+p2[0]*fr4+p6[0]*fr5;
				p3[1]=p1[1]+p2[1]*fr4+p6[1]*fr5;
				p3[2]=p1[2]+p2[2]*fr4+p6[2]*fr5;
				if (m_ecs2ucs)
					ic_usemat(p3,p4,m_ecs2ucs,0,0,0);
				else
					sds_trans(p3,&rbwcs,&rbucs,0,p4);
				gr_ucs2rp(p4,p3,view);
/*				if (SDS_osnap_chkdsq(pickptrp,p3,osptrp,dsq))
					{
					ic_ptcpy(ospt,p4);
					ic_ptcpy(osptrp,p3);
					}
*/
				osnap_helper_addPtRec( IC_OSMODE_END, p4, p3, pickptrp, ptAp1rp, ptAp2rp );
				}
			}
		if (osmode & IC_OSMODE_NEAR)//NEA
			{
			CEllipArc ellipArc(C3Point(p1[0], p1[1], p1[2]),	// center
				               C3Point(p2[0], p2[1], p2[2]),	// major axis
							   C3Point(p6[0], p6[1], p6[2]),	// minor axis
							   fr2,								// start parameter
							   fr3);							// end parameter

			if (m_ecs2ucs)
				ic_usemat(pickptucs,p3,m_ecs2ucs,0,0,1);
			else
				sds_trans(pickptucs, &rbucs,&rbwcs, 0,p3);

			C3Point pickPoint(p3[0], p3[1], p3[2]);

			C3Point nearestPoint;
			double rAt, rDis;
			ellipArc.NearestPoint(pickPoint, nearestPoint, rAt, rDis);

			p3[0] = nearestPoint.X(); p3[1] = nearestPoint.Y(); p3[2] = nearestPoint.Z();

			if (m_ecs2ucs)
				ic_usemat(p3,p4,m_ecs2ucs,0,0,0);
			else
				sds_trans(p3,&rbwcs,&rbucs,0,p4);
			gr_ucs2rp(p4,p3,view);

			osnap_helper_addPtRec(IC_OSMODE_NEAR, p4, p3, pickptrp, ptAp1rp, ptAp2rp );
		    }
		if (osmode & IC_OSMODE_MID) //MID
			{
			if (!icadRealEqual(fr3-IC_TWOPI,fr2))
				{
				//check starting point
				double mid = (fr2 + fr3) / 2.0;
				fr4=cos(mid);
				fr5=sin(mid);
				p3[0]=p1[0]+p2[0]*fr4+p6[0]*fr5;
				p3[1]=p1[1]+p2[1]*fr4+p6[1]*fr5;
				p3[2]=p1[2]+p2[2]*fr4+p6[2]*fr5;
				if (m_ecs2ucs)
					ic_usemat(p3,p4,m_ecs2ucs,0,0,0);
				else
					sds_trans(p3,&rbwcs,&rbucs,0,p4);
				gr_ucs2rp(p4,p3,view);
				osnap_helper_addPtRec( IC_OSMODE_MID, p4, p3, pickptrp, ptAp1rp, ptAp2rp );
				}
		    }
		if ((osmode & (IC_OSMODE_PERP | IC_OSMODE_NEAR))==IC_OSMODE_PERP)//PER must be calc'd in ECS
			{
			CEllipArc ellipArc(C3Point(p1[0], p1[1], p1[2]),	// center
				               C3Point(p2[0], p2[1], p2[2]),	// major axis
							   C3Point(p6[0], p6[1], p6[2]),	// minor axis
							   fr2,								// start parameter
							   fr3);							// end parameter

            if (m_ecs2ucs) {
				ic_usemat(lastpt,p5,m_ecs2ucs,0,0,1);
                ic_usemat(pickptucs,p6,m_ecs2ucs,0,0,1);
            }
            else {			
				sds_trans(lastpt,&rbucs,&rbwcs,0,p5);
                sds_trans(pickptucs,&rbucs,&rbwcs,0,p6);
            }
            C3Point lastPoint(p5[0], p5[1], p5[2]);
            C3Point ptPicked(p6[0], p6[1], p6[2]);

			double param;
			C3Point ptPerpendicular;
			if (ellipArc.PerpendicularThru(lastPoint, ptPicked, ptPerpendicular, param) == SUCCESS)
				{
				p3[0] = ptPerpendicular.X();
				p3[1] = ptPerpendicular.Y();
				p3[2] = ptPerpendicular.Z();

				if (m_ecs2ucs)
					ic_usemat(p3,p4,m_ecs2ucs,0,0,0);
				else
					sds_trans(p3,&rbwcs,&rbucs,0,p4);
				gr_ucs2rp(p4,p3,view);

				osnap_helper_addPtRec( IC_OSMODE_PERP, p4, p3, pickptrp, ptAp1rp, ptAp2rp );
				}
			}
		if (osmode & IC_OSMODE_TAN)	// TAN
			{
			CEllipArc ellipArc(C3Point(p1[0], p1[1], p1[2]),	// center
				               C3Point(p2[0], p2[1], p2[2]),	// major axis
							   C3Point(p6[0], p6[1], p6[2]),	// minor axis
							   fr2,								// start parameter
							   fr3);							// end parameter

			if (m_ecs2ucs)
				ic_usemat(lastpt,p5,m_ecs2ucs,0,0,1);
			else			
				sds_trans(lastpt,&rbucs,&rbwcs,0,p5);

			C3Point lastPoint(p5[0], p5[1], p5[2]);
			if (ellipArc.IsOnPlane(lastPoint))
				{

				if (m_ecs2ucs)
					ic_usemat(pickptucs,p3,m_ecs2ucs,0,0,1);
				else
					sds_trans(pickptucs, &rbucs,&rbwcs, 0,p3);

				C3Point pickPoint(p3[0], p3[1], p3[2]);

				double rAt, rDis;
				C3Point ptNearest;
				if (ellipArc.NearestPoint(pickPoint, ptNearest, rAt, rDis) != SUCCESS) 
					rAt = (ellipArc.Start() + ellipArc.End()) / 2.0;

				C3Point ptTangent, tangent;
				if (ellipArc.TangentThru(lastPoint, rAt, rAt) == SUCCESS &&
					ellipArc.Evaluate(rAt, ptTangent, tangent) == SUCCESS) 
					{
					p3[0] = ptTangent.X();
					p3[1] = ptTangent.Y();
					p3[2] = ptTangent.Z();

					if (m_ecs2ucs)
						ic_usemat(p3,p4,m_ecs2ucs,0,0,0);
					else
						sds_trans(p3,&rbwcs,&rbucs,0,p4);
					gr_ucs2rp(p4,p3,view);

					osnap_helper_addPtRec(IC_OSMODE_TAN, p4, p3, pickptrp, ptAp1rp, ptAp2rp );
					}
				}
			}
		if (osmode & IC_OSMODE_QUADRANT)//QUA
			{
			for (fi1=0;fi1<4;fi1++)
				{
				fr4=fi1*IC_PI/2.0;
				ic_normang(&fr2,&fr4);
				if (fr4<=fr3)
					{
					switch(fi1)
						{
						case 0: //3 o'clock
							p3[0]=p1[0]+p2[0];
							p3[1]=p1[1]+p2[1];
							p3[2]=p1[2]+p2[2];
							break;
						case 1:	//12 o'clock
							p3[0]=p1[0]+p6[0];
							p3[1]=p1[1]+p6[1];
							p3[2]=p1[2]+p6[2];
							break;
						case 2:	//9 o'clock
							p3[0]=p1[0]-p2[0];
							p3[1]=p1[1]-p2[1];
							p3[2]=p1[2]-p2[2];
							break;
						case 3:	//6o'clock
							p3[0]=p1[0]-p6[0];
							p3[1]=p1[1]-p6[1];
							p3[2]=p1[2]-p6[2];
							break;
						}
					if (m_ecs2ucs)
						ic_usemat(p3,p4,m_ecs2ucs,0,0,0);
					else
						sds_trans(p3,&rbwcs,&rbucs,0,p4);
					gr_ucs2rp(p4,p5,view);
/*					if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
						{
						ic_ptcpy(ospt,p4);
						ic_ptcpy(osptrp,p5);
						}
*/
					osnap_helper_addPtRec( IC_OSMODE_QUADRANT, p4, p5, pickptrp, ptAp1rp, ptAp2rp );
					}//if
				}//for
			}
		if (osmode & IC_OSMODE_CENTER)//CEN
			{
			ic_ptcpy(p6,p1);
			if (m_ecs2ucs)
				ic_usemat(p6,p4,m_ecs2ucs,0,0,0);
			else
				sds_trans(p6,&rbwcs,&rbucs,0,p4);
			gr_ucs2rp(p4,p5,view);
/*			if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
				{
				ic_ptcpy(ospt,p4);
				ic_ptcpy(osptrp,p5);
				}
*/
				osnap_helper_addPtRec( IC_OSMODE_CENTER, p4, p5, pickptrp, ptAp1rp, ptAp2rp );
			}
		}
	else if (strsame(etype,db_hitype2str(DB_SOLID)) || strsame(etype,db_hitype2str(DB_3DFACE)) || strsame(etype,db_hitype2str(DB_TRACE)))
		{
		//NOTE: ordering for 3Dfaces is different & we need the invisibility flags
		fi2=0;
		for (rbtemp=elist;rbtemp!=NULL;rbtemp=rbtemp->rbnext)
			{
			if (rbtemp->restype==10)
				ic_ptcpy(p1,rbtemp->resval.rpoint);
			else if (rbtemp->restype==11)
				ic_ptcpy(p2,rbtemp->resval.rpoint);
			else if (rbtemp->restype==12)
				ic_ptcpy(p3,rbtemp->resval.rpoint);
			else if (rbtemp->restype==13)
				ic_ptcpy(p4,rbtemp->resval.rpoint);
			else if (rbtemp->restype==70)
				fi2=rbtemp->resval.rint;
			else if (rbtemp->restype==-1)
				ic_encpy(rbent.resval.rlname,rbtemp->resval.rlname);
			}
		rbtemp=NULL;
        if (thickflg)
			{
            ic_ptcpy(p11,p1);
            ic_ptcpy(p12,p2);
            ic_ptcpy(p13,p3);
            ic_ptcpy(p14,p4);
            p11[0]+=thickness*p210[0];
            p11[1]+=thickness*p210[1];
            p11[2]+=thickness*p210[2];
            p12[0]+=thickness*p210[0];
            p12[1]+=thickness*p210[1];
            p12[2]+=thickness*p210[2];
            p13[0]+=thickness*p210[0];
            p13[1]+=thickness*p210[1];
            p13[2]+=thickness*p210[2];
            p14[0]+=thickness*p210[0];
            p14[1]+=thickness*p210[1];
            p14[2]+=thickness*p210[2];
			}
		if (m_ecs2ucs)
			{
			ic_usemat(p1,p1,m_ecs2ucs,0,0,0);
			ic_usemat(p2,p2,m_ecs2ucs,0,0,0);
			ic_usemat(p3,p3,m_ecs2ucs,0,0,0);
			ic_usemat(p4,p4,m_ecs2ucs,0,0,0);
			}
		else
			{
			sds_trans(p1,&rbent,&rbucs,0,p1);
			sds_trans(p2,&rbent,&rbucs,0,p2);
			sds_trans(p3,&rbent,&rbucs,0,p3);
			sds_trans(p4,&rbent,&rbucs,0,p4);
			}
		if (thickflg)
			{
			if (m_ecs2ucs)
				{
				ic_usemat(p11,p11,m_ecs2ucs,0,0,0);
				ic_usemat(p12,p12,m_ecs2ucs,0,0,0);
				ic_usemat(p13,p13,m_ecs2ucs,0,0,0);
				ic_usemat(p14,p14,m_ecs2ucs,0,0,0);
				}
			else
				{
				sds_trans(p11,&rbent,&rbucs,0,p11);
				sds_trans(p12,&rbent,&rbucs,0,p12);
				sds_trans(p13,&rbent,&rbucs,0,p13);
				sds_trans(p14,&rbent,&rbucs,0,p14);
				}
			}

        if ((osmode & IC_OSMODE_CENTER) && !strsame(etype,db_hitype2str(DB_3DFACE))) //CEN of solid or trace
			{
			if (strsame(etype,db_hitype2str(DB_SOLID)) && icadPointEqual(p3,p4))
				{
				p5[0]=(p1[0]+p2[0]+p3[0])/3.0;
				p5[1]=(p1[1]+p2[1]+p3[1])/3.0;
				p5[2]=(p1[2]+p2[2]+p3[2])/3.0;
				}
			else
				{
				p5[0]=(p1[0]+p2[0]+p3[0]+p4[0])/4.0;
				p5[1]=(p1[1]+p2[1]+p3[1]+p4[1])/4.0;
				p5[2]=(p1[2]+p2[2]+p3[2]+p4[2])/4.0;
				}
			gr_ucs2rp(p5,p6,view);
/*			if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
				{
				ic_ptcpy(ospt,p5);
				ic_ptcpy(osptrp,p6);
				}
*/
			osnap_helper_addPtRec( IC_OSMODE_CENTER, p5, p6, pickptrp, ptAp1rp, ptAp2rp );
			if (thickflg)
				{
                if (strsame(etype,db_hitype2str(DB_SOLID)) && icadPointEqual(p13,p14))
					{
				    p15[0]=(p11[0]+p12[0]+p13[0])/3.0;
				    p15[1]=(p11[1]+p12[1]+p13[1])/3.0;
				    p15[2]=(p11[2]+p12[2]+p13[2])/3.0;
					}
				else
					{
				    p15[0]=(p11[0]+p12[0]+p13[0]+p14[0])/4.0;
				    p15[1]=(p11[1]+p12[1]+p13[1]+p14[1])/4.0;
				    p15[2]=(p11[2]+p12[2]+p13[2]+p14[2])/4.0;
					}
			    gr_ucs2rp(p15,p16,view);
/*			    if (SDS_osnap_chkdsq(pickptrp,p16,osptrp,dsq))
					{
					ic_ptcpy(ospt,p15);
					ic_ptcpy(osptrp,p16);
					}
*/
				osnap_helper_addPtRec( IC_OSMODE_CENTER, p15, p16, pickptrp, ptAp1rp, ptAp2rp );
				}
			}
		if (osmode & (IC_OSMODE_END | IC_OSMODE_INT | IC_OSMODE_APP | IC_OSMODE_PLAN))
			{
			tmpOsmode = osmode & IC_OSMODE_END;
			if( !tmpOsmode )
				tmpOsmode = osmode & IC_OSMODE_INT;
			if( !tmpOsmode )
				tmpOsmode = osmode & IC_OSMODE_PLAN;
			if( !tmpOsmode )
				tmpOsmode = IC_OSMODE_APP;

			//INT modes or END 
			if (strsame(etype,db_hitype2str(DB_3DFACE)))  //int only
				{
				if (!(fi2&(8+1)==9))
					{
					gr_ucs2rp(p1,p5,view);
/*					if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
						{
						ic_ptcpy(ospt,p1);
						ic_ptcpy(osptrp,p5);
						}
*/
					osnap_helper_addPtRec( tmpOsmode, p1, p5, pickptrp, ptAp1rp, ptAp2rp );
					}
				if (!(fi2&(1+2)==3))
					{
					gr_ucs2rp(p2,p5,view);
/*					if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
						{
						ic_ptcpy(ospt,p2);
						ic_ptcpy(osptrp,p5);
						}
*/
					osnap_helper_addPtRec( tmpOsmode, p2, p5, pickptrp, ptAp1rp, ptAp2rp );
					}
				if (!(fi2&(2+4)==6))
					{
					gr_ucs2rp(p3,p5,view);
/*					if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
						{
						ic_ptcpy(ospt,p3);
						ic_ptcpy(osptrp,p5);
						}
*/
					osnap_helper_addPtRec( tmpOsmode, p3, p5, pickptrp, ptAp1rp, ptAp2rp );
					}
				if (!(fi2&(4+8)==12))
					{
					gr_ucs2rp(p4,p5,view);
/*					if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
						{
						ic_ptcpy(ospt,p4);
						ic_ptcpy(osptrp,p5);
						}
*/
					osnap_helper_addPtRec( tmpOsmode, p4, p5, pickptrp, ptAp1rp, ptAp2rp );
					}
				}
			else
				{
				gr_ucs2rp(p1,p5,view);
/*				if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
					{
					ic_ptcpy(ospt,p1);
					ic_ptcpy(osptrp,p5);
					}
*/
					osnap_helper_addPtRec( tmpOsmode, p1, p5, pickptrp, ptAp1rp, ptAp2rp );
				gr_ucs2rp(p2,p5,view);
/*				if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
					{
					ic_ptcpy(ospt,p2);
					ic_ptcpy(osptrp,p5);
					}
*/
				osnap_helper_addPtRec( tmpOsmode, p2, p5, pickptrp, ptAp1rp, ptAp2rp );
				gr_ucs2rp(p3,p5,view);
/*				if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
					{
					ic_ptcpy(ospt,p3);
					ic_ptcpy(osptrp,p5);
					}
*/
				osnap_helper_addPtRec( tmpOsmode, p3, p5, pickptrp, ptAp1rp, ptAp2rp );
				gr_ucs2rp(p4,p5,view);
/*				if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
					{
					ic_ptcpy(ospt,p4);
					ic_ptcpy(osptrp,p5);
					}
*/
				osnap_helper_addPtRec( tmpOsmode, p4, p5, pickptrp, ptAp1rp, ptAp2rp );
				}
			if (thickflg)
				{
                if (strsame(etype,db_hitype2str(DB_3DFACE)))  //int only
					{
					if (!(fi2&(8+1)==9))
						{
					    gr_ucs2rp(p11,p15,view);
/*					    if (SDS_osnap_chkdsq(pickptrp,p15,osptrp,dsq))
							{
							ic_ptcpy(ospt,p11);
							ic_ptcpy(osptrp,p15);
							}
*/
						osnap_helper_addPtRec( tmpOsmode, p11, p15, pickptrp, ptAp1rp, ptAp2rp );
						}
				    if (!(fi2&(1+2)==3))
						{
					    gr_ucs2rp(p12,p15,view);
/*					    if (SDS_osnap_chkdsq(pickptrp,p15,osptrp,dsq))
							{
							ic_ptcpy(ospt,p12);
							ic_ptcpy(osptrp,p15);
							}
*/
						osnap_helper_addPtRec( tmpOsmode, p12, p15, pickptrp, ptAp1rp, ptAp2rp );
						}
				    if (!(fi2&(2+4)==6))
						{
					    gr_ucs2rp(p13,p15,view);
/*					    if (SDS_osnap_chkdsq(pickptrp,p15,osptrp,dsq))
							{
							ic_ptcpy(ospt,p13);
							ic_ptcpy(osptrp,p15);
							}
*/
						osnap_helper_addPtRec( tmpOsmode, p13, p15, pickptrp, ptAp1rp, ptAp2rp );
						}
				    if (!(fi2&(4+8)==12))
						{
					    gr_ucs2rp(p14,p15,view);
/*					    if (SDS_osnap_chkdsq(pickptrp,p15,osptrp,dsq))
							{
							ic_ptcpy(ospt,p14);
							ic_ptcpy(osptrp,p15);
							}
*/
						osnap_helper_addPtRec( tmpOsmode, p14, p15, pickptrp, ptAp1rp, ptAp2rp );
						}

					}
				else
					{
				    gr_ucs2rp(p11,p15,view);
/*				    if (SDS_osnap_chkdsq(pickptrp,p15,osptrp,dsq))
						{
						ic_ptcpy(ospt,p11);
						ic_ptcpy(osptrp,p15);
						}
*/
					osnap_helper_addPtRec( tmpOsmode, p11, p15, pickptrp, ptAp1rp, ptAp2rp );
				    gr_ucs2rp(p12,p15,view);
/*				    if (SDS_osnap_chkdsq(pickptrp,p15,osptrp,dsq))
						{
						ic_ptcpy(ospt,p12);
						ic_ptcpy(osptrp,p15);
						}
*/
					osnap_helper_addPtRec( tmpOsmode, p12, p15, pickptrp, ptAp1rp, ptAp2rp );
				    gr_ucs2rp(p13,p15,view);
/*				    if (SDS_osnap_chkdsq(pickptrp,p15,osptrp,dsq))
						{
						ic_ptcpy(ospt,p13);
						ic_ptcpy(osptrp,p15);
						}
*/
					osnap_helper_addPtRec( tmpOsmode, p13, p15, pickptrp, ptAp1rp, ptAp2rp );
				    gr_ucs2rp(p14,p15,view);
/*				    if (SDS_osnap_chkdsq(pickptrp,p15,osptrp,dsq))
						{
						ic_ptcpy(ospt,p14);
						ic_ptcpy(osptrp,p15);
						}
*/
					osnap_helper_addPtRec( tmpOsmode, p14, p15, pickptrp, ptAp1rp, ptAp2rp );
					}
				}
			}
		if (osmode & IC_OSMODE_NEAR)//NEA
			{
			//NOTE: all nea calcs depend upon view, so they must be done in RP!
			if (!strsame(etype,db_hitype2str(DB_3DFACE)))  //solid & trace
				{
				gr_ucs2rp(p1,p5,view);
				gr_ucs2rp(p2,p6,view);
				ic_segdist(pickptrp,p5,p6,0,&fr1,p7);
/*				if (SDS_osnap_chkdsq(pickptrp,p7,osptrp,dsq))
					{
					ic_ptcpy(osptrp,p7);
					fr1=sds_distance(p5,p7)/sds_distance(p5,p6);
					ospt[0]=p1[0]+(fr1*(p2[0]-p1[0]));
					ospt[1]=p1[1]+(fr1*(p2[1]-p1[1]));
					ospt[2]=p1[2]+(fr1*(p2[2]-p1[2]));
					}
*/
			fr1=sds_distance(p5,p7)/sds_distance(p5,p6);
			ucsPt[0]=p1[0]+(fr1*(p2[0]-p1[0]));
			ucsPt[1]=p1[1]+(fr1*(p2[1]-p1[1]));
			ucsPt[2]=p1[2]+(fr1*(p2[2]-p1[2]));
			osnap_helper_addPtRec( IC_OSMODE_NEAR, ucsPt, p7, pickptrp, ptAp1rp, ptAp2rp );
				gr_ucs2rp(p3,p6,view);
				ic_segdist(pickptrp,p5,p6,0,&fr1,p7);
/*				if (SDS_osnap_chkdsq(pickptrp,p7,osptrp,dsq))
					{
					ic_ptcpy(osptrp,p7);
					fr1=sds_distance(p5,p7)/sds_distance(p5,p6);
					ospt[0]=p1[0]+(fr1*(p3[0]-p1[0]));
					ospt[1]=p1[1]+(fr1*(p3[1]-p1[1]));
					ospt[2]=p1[2]+(fr1*(p3[2]-p1[2]));
					}
*/
				fr1=sds_distance(p5,p7)/sds_distance(p5,p6);
				ucsPt[0]=p1[0]+(fr1*(p3[0]-p1[0]));
				ucsPt[1]=p1[1]+(fr1*(p3[1]-p1[1]));
				ucsPt[2]=p1[2]+(fr1*(p3[2]-p1[2]));
				osnap_helper_addPtRec( IC_OSMODE_NEAR, ucsPt, p7, pickptrp, ptAp1rp, ptAp2rp );
				gr_ucs2rp(p4,p5,view);
				gr_ucs2rp(p2,p6,view);
				ic_segdist(pickptrp,p5,p6,0,&fr1,p7);
/*				if (SDS_osnap_chkdsq(pickptrp,p7,osptrp,dsq))
					{
					ic_ptcpy(osptrp,p7);
					fr1=sds_distance(p5,p7)/sds_distance(p5,p6);
					ospt[0]=p4[0]+(fr1*(p2[0]-p4[0]));
					ospt[1]=p4[1]+(fr1*(p2[1]-p4[1]));
					ospt[2]=p4[2]+(fr1*(p2[2]-p4[2]));
					}
*/
					fr1=sds_distance(p5,p7)/sds_distance(p5,p6);
				ucsPt[0]=p4[0]+(fr1*(p2[0]-p4[0]));
				ucsPt[1]=p4[1]+(fr1*(p2[1]-p4[1]));
				ucsPt[2]=p4[2]+(fr1*(p2[2]-p4[2]));
				osnap_helper_addPtRec( IC_OSMODE_NEAR, ucsPt, p7, pickptrp, ptAp1rp, ptAp2rp );
				gr_ucs2rp(p3,p6,view);
				ic_segdist(pickptrp,p5,p6,0,&fr1,p7);
/*				if (SDS_osnap_chkdsq(pickptrp,p7,osptrp,dsq))
					{
					ic_ptcpy(osptrp,p7);
					fr1=sds_distance(p5,p7)/sds_distance(p5,p6);
					ospt[0]=p4[0]+(fr1*(p3[0]-p4[0]));
					ospt[1]=p4[1]+(fr1*(p3[1]-p4[1]));
					ospt[2]=p4[2]+(fr1*(p3[2]-p4[2]));
					}
*/
				fr1=sds_distance(p5,p7)/sds_distance(p5,p6);
				ucsPt[0]=p4[0]+(fr1*(p3[0]-p4[0]));
				ucsPt[1]=p4[1]+(fr1*(p3[1]-p4[1]));
				ucsPt[2]=p4[2]+(fr1*(p3[2]-p4[2]));
				osnap_helper_addPtRec( IC_OSMODE_NEAR, ucsPt, p7, pickptrp, ptAp1rp, ptAp2rp );
				if (thickflg)
					{
				    gr_ucs2rp(p11,p15,view);
				    gr_ucs2rp(p12,p16,view);
				    ic_segdist(pickptrp,p15,p16,0,&fr1,p17);
/*				    if (SDS_osnap_chkdsq(pickptrp,p17,osptrp,dsq))
						{
					    ic_ptcpy(osptrp,p17);
					    fr1=sds_distance(p15,p17)/sds_distance(p15,p16);
					    ospt[0]=p11[0]+(fr1*(p12[0]-p11[0]));
					    ospt[1]=p11[1]+(fr1*(p12[1]-p11[1]));
					    ospt[2]=p11[2]+(fr1*(p12[2]-p11[2]));
						}
*/
					fr1=sds_distance(p15,p17)/sds_distance(p15,p16);
					ucsPt[0]=p11[0]+(fr1*(p12[0]-p11[0]));
					ucsPt[1]=p11[1]+(fr1*(p12[1]-p11[1]));
					ucsPt[2]=p11[2]+(fr1*(p12[2]-p11[2]));
					osnap_helper_addPtRec( IC_OSMODE_NEAR, ucsPt, p17, pickptrp, ptAp1rp, ptAp2rp );
				    gr_ucs2rp(p13,p16,view);
				    ic_segdist(pickptrp,p15,p16,0,&fr1,p17);
/*				    if (SDS_osnap_chkdsq(pickptrp,p17,osptrp,dsq))
						{
					    ic_ptcpy(osptrp,p17);
					    fr1=sds_distance(p15,p17)/sds_distance(p15,p16);
					    ospt[0]=p11[0]+(fr1*(p13[0]-p11[0]));
					    ospt[1]=p11[1]+(fr1*(p13[1]-p11[1]));
					    ospt[2]=p11[2]+(fr1*(p13[2]-p11[2]));
					    }
*/
					fr1=sds_distance(p15,p17)/sds_distance(p15,p16);
					ucsPt[0]=p11[0]+(fr1*(p13[0]-p11[0]));
					ucsPt[1]=p11[1]+(fr1*(p13[1]-p11[1]));
					ucsPt[2]=p11[2]+(fr1*(p13[2]-p11[2]));
					osnap_helper_addPtRec( IC_OSMODE_NEAR, ucsPt, p17, pickptrp, ptAp1rp, ptAp2rp );
				    gr_ucs2rp(p14,p15,view);
				    gr_ucs2rp(p12,p16,view);
				    ic_segdist(pickptrp,p15,p16,0,&fr1,p17);
/*				    if (SDS_osnap_chkdsq(pickptrp,p17,osptrp,dsq))
						{
					    ic_ptcpy(osptrp,p17);
					    fr1=sds_distance(p15,p17)/sds_distance(p15,p16);
					    ospt[0]=p14[0]+(fr1*(p12[0]-p14[0]));
					    ospt[1]=p14[1]+(fr1*(p12[1]-p14[1]));
					    ospt[2]=p14[2]+(fr1*(p12[2]-p14[2]));
						}
*/
					fr1=sds_distance(p15,p17)/sds_distance(p15,p16);
					ucsPt[0]=p14[0]+(fr1*(p12[0]-p14[0]));
					ucsPt[1]=p14[1]+(fr1*(p12[1]-p14[1]));
					ucsPt[2]=p14[2]+(fr1*(p12[2]-p14[2]));
					osnap_helper_addPtRec( IC_OSMODE_NEAR, ucsPt, p17, pickptrp, ptAp1rp, ptAp2rp );
				    gr_ucs2rp(p13,p16,view);
				    ic_segdist(pickptrp,p15,p16,0,&fr1,p17);
/*				    if (SDS_osnap_chkdsq(pickptrp,p17,osptrp,dsq))
						{
					    ic_ptcpy(osptrp,p17);
					    fr1=sds_distance(p15,p17)/sds_distance(p15,p16);
					    ospt[0]=p14[0]+(fr1*(p13[0]-p14[0]));
					    ospt[1]=p14[1]+(fr1*(p13[1]-p14[1]));
					    ospt[2]=p14[2]+(fr1*(p13[2]-p14[2]));
						}
*/
				    fr1=sds_distance(p15,p17)/sds_distance(p15,p16);
				    ucsPt[0]=p14[0]+(fr1*(p13[0]-p14[0]));
				    ucsPt[1]=p14[1]+(fr1*(p13[1]-p14[1]));
				    ucsPt[2]=p14[2]+(fr1*(p13[2]-p14[2]));
					osnap_helper_addPtRec( IC_OSMODE_NEAR, ucsPt, p17, pickptrp, ptAp1rp, ptAp2rp );
					//check along the edges extending up along extrusion
				    gr_ucs2rp(p11,p15,view);
				    gr_ucs2rp(p1,p16,view);
				    ic_segdist(pickptrp,p15,p16,0,&fr1,p17);
/*				    if (SDS_osnap_chkdsq(pickptrp,p17,osptrp,dsq))
						{
					    ic_ptcpy(osptrp,p17);
					    fr1=sds_distance(p15,p17)/sds_distance(p15,p16);
					    ospt[0]=p11[0]+(fr1*(p1[0]-p11[0]));
					    ospt[1]=p11[1]+(fr1*(p1[1]-p11[1]));
					    ospt[2]=p11[2]+(fr1*(p1[2]-p11[2]));
						}
*/
				    fr1=sds_distance(p15,p17)/sds_distance(p15,p16);
				    ucsPt[0]=p11[0]+(fr1*(p1[0]-p11[0]));
				    ucsPt[1]=p11[1]+(fr1*(p1[1]-p11[1]));
				    ucsPt[2]=p11[2]+(fr1*(p1[2]-p11[2]));
					osnap_helper_addPtRec( IC_OSMODE_NEAR, ucsPt, p17, pickptrp, ptAp1rp, ptAp2rp );
				    gr_ucs2rp(p12,p15,view);
				    gr_ucs2rp(p2,p16,view);
				    ic_segdist(pickptrp,p15,p16,0,&fr1,p17);
/*				    if (SDS_osnap_chkdsq(pickptrp,p17,osptrp,dsq))
						{
					    ic_ptcpy(osptrp,p17);
					    fr1=sds_distance(p15,p17)/sds_distance(p15,p16);
					    ospt[0]=p12[0]+(fr1*(p2[0]-p12[0]));
					    ospt[1]=p12[1]+(fr1*(p2[1]-p12[1]));
					    ospt[2]=p12[2]+(fr1*(p2[2]-p12[2]));
						}
*/
				    fr1=sds_distance(p15,p17)/sds_distance(p15,p16);
				    ucsPt[0]=p12[0]+(fr1*(p2[0]-p12[0]));
				    ucsPt[1]=p12[1]+(fr1*(p2[1]-p12[1]));
				    ucsPt[2]=p12[2]+(fr1*(p2[2]-p12[2]));
					osnap_helper_addPtRec( IC_OSMODE_NEAR, ucsPt, p17, pickptrp, ptAp1rp, ptAp2rp );
				    gr_ucs2rp(p13,p15,view);
				    gr_ucs2rp(p3,p16,view);
				    ic_segdist(pickptrp,p15,p16,0,&fr1,p17);
/*				    if (SDS_osnap_chkdsq(pickptrp,p17,osptrp,dsq))
						{
					    ic_ptcpy(osptrp,p17);
					    fr1=sds_distance(p15,p17)/sds_distance(p15,p16);
					    ospt[0]=p13[0]+(fr1*(p3[0]-p13[0]));
					    ospt[1]=p13[1]+(fr1*(p3[1]-p13[1]));
					    ospt[2]=p13[2]+(fr1*(p3[2]-p13[2]));
						}
*/
				    fr1=sds_distance(p15,p17)/sds_distance(p15,p16);
				    ucsPt[0]=p13[0]+(fr1*(p3[0]-p13[0]));
				    ucsPt[1]=p13[1]+(fr1*(p3[1]-p13[1]));
				    ucsPt[2]=p13[2]+(fr1*(p3[2]-p13[2]));
					osnap_helper_addPtRec( IC_OSMODE_NEAR, ucsPt, p17, pickptrp, ptAp1rp, ptAp2rp );
				    gr_ucs2rp(p14,p15,view);
				    gr_ucs2rp(p4,p16,view);
				    ic_segdist(pickptrp,p15,p16,0,&fr1,p17);
/*				    if (SDS_osnap_chkdsq(pickptrp,p17,osptrp,dsq))
						{
					    ic_ptcpy(osptrp,p17);
					    fr1=sds_distance(p15,p17)/sds_distance(p15,p16);
					    ospt[0]=p14[0]+(fr1*(p4[0]-p14[0]));
					    ospt[1]=p14[1]+(fr1*(p4[1]-p14[1]));
					    ospt[2]=p14[2]+(fr1*(p4[2]-p14[2]));
						}
*/
					fr1=sds_distance(p15,p17)/sds_distance(p15,p16);
					ucsPt[0]=p14[0]+(fr1*(p4[0]-p14[0]));
					ucsPt[1]=p14[1]+(fr1*(p4[1]-p14[1]));
					ucsPt[2]=p14[2]+(fr1*(p4[2]-p14[2]));
					osnap_helper_addPtRec( IC_OSMODE_NEAR, ucsPt, p17, pickptrp, ptAp1rp, ptAp2rp );
					}
				}
			else	//3DFACE
				{
				//3dfaces can't have thickness
				if (!(fi2 & IC_FACE3D_FACE1INVIS))
					{
					gr_ucs2rp(p1,p5,view);
					gr_ucs2rp(p2,p6,view);
					ic_segdist(pickptrp,p5,p6,0,&fr1,p7);
/*					if (SDS_osnap_chkdsq(pickptrp,p7,osptrp,dsq))
						{
						ic_ptcpy(osptrp,p7);
						fr1=sds_distance(p5,p7)/sds_distance(p5,p6);
						ospt[0]=p1[0]+fr1*(p2[0]-p1[0]);
						ospt[1]=p1[1]+fr1*(p2[1]-p1[1]);
						ospt[2]=p1[2]+fr1*(p2[2]-p1[2]);
						}
*/
					fr1=sds_distance(p5,p7)/sds_distance(p5,p6);
					ucsPt[0]=p1[0]+fr1*(p2[0]-p1[0]);
					ucsPt[1]=p1[1]+fr1*(p2[1]-p1[1]);
					ucsPt[2]=p1[2]+fr1*(p2[2]-p1[2]);
					osnap_helper_addPtRec( IC_OSMODE_NEAR, ucsPt, p7, pickptrp, ptAp1rp, ptAp2rp );
					}
				if (!(fi2 & IC_FACE3D_FACE2INVIS))
					{
					gr_ucs2rp(p2,p5,view);
					gr_ucs2rp(p3,p6,view);
					ic_segdist(pickptrp,p5,p6,0,&fr1,p7);
/*					if (SDS_osnap_chkdsq(pickptrp,p7,osptrp,dsq))
						{
						ic_ptcpy(osptrp,p7);
						fr1=sds_distance(p5,p7)/sds_distance(p5,p6);
						ospt[0]=p2[0]+fr1*(p3[0]-p2[0]);
						ospt[1]=p2[1]+fr1*(p3[1]-p2[1]);
						ospt[2]=p2[2]+fr1*(p3[2]-p2[2]);
						}
*/
					fr1=sds_distance(p5,p7)/sds_distance(p5,p6);
					ucsPt[0]=p2[0]+fr1*(p3[0]-p2[0]);
					ucsPt[1]=p2[1]+fr1*(p3[1]-p2[1]);
					ucsPt[2]=p2[2]+fr1*(p3[2]-p2[2]);
					osnap_helper_addPtRec( IC_OSMODE_NEAR, ucsPt, p7, pickptrp, ptAp1rp, ptAp2rp );
					}
				if (!(fi2 & IC_FACE3D_FACE3INVIS))
					{
					gr_ucs2rp(p3,p5,view);
					gr_ucs2rp(p4,p6,view);
					ic_segdist(pickptrp,p5,p6,0,&fr1,p7);
/*					if (SDS_osnap_chkdsq(pickptrp,p7,osptrp,dsq))
						{
						ic_ptcpy(osptrp,p7);
						fr1=sds_distance(p5,p7)/sds_distance(p5,p6);
						ospt[0]=p3[0]+fr1*(p4[0]-p3[0]);
						ospt[1]=p3[1]+fr1*(p4[1]-p3[1]);
						ospt[2]=p3[2]+fr1*(p4[2]-p3[2]);
						}
*/
					fr1=sds_distance(p5,p7)/sds_distance(p5,p6);
					ucsPt[0]=p3[0]+fr1*(p4[0]-p3[0]);
					ucsPt[1]=p3[1]+fr1*(p4[1]-p3[1]);
					ucsPt[2]=p3[2]+fr1*(p4[2]-p3[2]);
					osnap_helper_addPtRec( IC_OSMODE_NEAR, ucsPt, p7, pickptrp, ptAp1rp, ptAp2rp );
					}
				if (!(fi2 & IC_FACE3D_FACE4INVIS))
					{
					gr_ucs2rp(p4,p5,view);
					gr_ucs2rp(p1,p6,view);
					ic_segdist(pickptrp,p5,p6,0,&fr1,p7);
/*					if (SDS_osnap_chkdsq(pickptrp,p7,osptrp,dsq))
						{
						ic_ptcpy(osptrp,p7);
						fr1=sds_distance(p5,p7)/sds_distance(p5,p6);
						ospt[0]=p4[0]+fr1*(p1[0]-p4[0]);
						ospt[1]=p4[1]+fr1*(p1[1]-p4[1]);
						ospt[2]=p4[2]+fr1*(p1[2]-p4[2]);
						}
*/
					fr1=sds_distance(p5,p7)/sds_distance(p5,p6);
					ucsPt[0]=p4[0]+fr1*(p1[0]-p4[0]);
					ucsPt[1]=p4[1]+fr1*(p1[1]-p4[1]);
					ucsPt[2]=p4[2]+fr1*(p1[2]-p4[2]);
					osnap_helper_addPtRec( IC_OSMODE_NEAR, ucsPt, p7, pickptrp, ptAp1rp, ptAp2rp );
					}
				}
			}
		if (osmode & IC_OSMODE_PERP)//PER
			{
			if (!strsame(etype,db_hitype2str(DB_3DFACE)))  //for solid & trace, use ecs
				{
				//NOTE: can't do calc in RP, because per is independent of view
				ic_ptcpy(p5,lastpt);
				if (-1!=ic_ptlndist(p5,p1,p2,&fr1,p7))
					{
					gr_ucs2rp(p7,p6,view);
/*					if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
						{
						ic_ptcpy(ospt,p7);
						ic_ptcpy(osptrp,p6);
						}
*/
					osnap_helper_addPtRec( IC_OSMODE_PERP, p7, p6, pickptrp, ptAp1rp, ptAp2rp );
					}	
				if (-1!=ic_ptlndist(p5,p1,p3,&fr1,p7))
					{
					gr_ucs2rp(p7,p6,view);
/*					if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
						{
						ic_ptcpy(ospt,p7);
						ic_ptcpy(osptrp,p6);
						}
*/
					osnap_helper_addPtRec( IC_OSMODE_PERP, p7, p6, pickptrp, ptAp1rp, ptAp2rp );
					}	
				if (-1!=ic_ptlndist(p5,p4,p2,&fr1,p7))
					{
					gr_ucs2rp(p7,p6,view);
/*					if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
						{
						ic_ptcpy(ospt,p7);
						ic_ptcpy(osptrp,p6);
						}
*/
					osnap_helper_addPtRec( IC_OSMODE_PERP, p7, p6, pickptrp, ptAp1rp, ptAp2rp );
					}	
				if (-1!=ic_ptlndist(p5,p4,p3,&fr1,p7))
					{
					gr_ucs2rp(p7,p6,view);
/*					if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
						{
						ic_ptcpy(ospt,p7);
						ic_ptcpy(osptrp,p6);
						}
*/
					osnap_helper_addPtRec( IC_OSMODE_PERP, p7, p6, pickptrp, ptAp1rp, ptAp2rp );
					}
				}
			else	//PER for 3DFACE
				{
				//NOTE: WCS is native to the 3DFACE, but we can still do the calcs in UCS
				/*DG - 14.10.2002*/// 'lastpt' should be used instead of 'pickptucs' in 'ic_ptlndist', of course.
				// Also, all strange statements with sds_dist2d below are commented out.
				if (!(fi2 & IC_FACE3D_FACE1INVIS) && -1!=ic_ptlndist(lastpt/*pickptucs*/,p1,p2,&fr1,p5))
					{
					//p5[2]=p1[2]+(p2[2]-p1[2])*sds_dist2d(p1,p5)/sds_dist2d(p1,p2);
					gr_ucs2rp(p5,p6,view);
/*					if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
						{
						ic_ptcpy(ospt,p5);
						ic_ptcpy(osptrp,p6);
						}
*/
					osnap_helper_addPtRec( IC_OSMODE_PERP, p5, p6, pickptrp, ptAp1rp, ptAp2rp );
					}

				if (!(fi2 & IC_FACE3D_FACE2INVIS) && -1!=ic_ptlndist(lastpt/*pickptucs*/,p2,p3,&fr1,p5))
					{
					//p5[2]=p1[2]+(p3[2]-p2[2])*sds_dist2d(p2,p5)/sds_dist2d(p2,p3);
					gr_ucs2rp(p5,p6,view);
/*					if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
						{
						ic_ptcpy(ospt,p5);
						ic_ptcpy(osptrp,p6);
						}
*/
					osnap_helper_addPtRec( IC_OSMODE_PERP, p5, p6, pickptrp, ptAp1rp, ptAp2rp );
					}
				if (!(fi2 & IC_FACE3D_FACE3INVIS) && -1!=ic_ptlndist(lastpt/*pickptucs*/,p3,p4,&fr1,p5))
					{
					//p5[2]=p1[3]+(p3[4]-p2[3])*sds_dist2d(p3,p5)/sds_dist2d(p3,p4);
					gr_ucs2rp(p5,p6,view);
/*					if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
						{
						ic_ptcpy(ospt,p5);
						ic_ptcpy(osptrp,p6);
						}
*/
					osnap_helper_addPtRec( IC_OSMODE_PERP, p5, p6, pickptrp, ptAp1rp, ptAp2rp );
					}
				if (!(fi2 & IC_FACE3D_FACE4INVIS) && -1!=ic_ptlndist(lastpt/*pickptucs*/,p4,p1,&fr1,p5))
					{
					//p5[2]=p4[2]+(p1[2]-p4[2])*sds_dist2d(p4,p5)/sds_dist2d(p4,p1);
					gr_ucs2rp(p5,p6,view);
/*					if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
						{
						ic_ptcpy(ospt,p5);
						ic_ptcpy(osptrp,p6);
						}
*/
					osnap_helper_addPtRec( IC_OSMODE_PERP, p5, p6, pickptrp, ptAp1rp, ptAp2rp );
					}
				}
			if (thickflg)
				{
                if (!strsame(etype,db_hitype2str(DB_3DFACE)))  //for solid & trace, use ecs
					{
					//NOTE: can't do calc in RP, because per is independent of view
					ic_ptcpy(p15,lastpt);
				    if (-1!=ic_ptlndist(p15,p11,p12,&fr1,p16))
						{
					    gr_ucs2rp(p17,p16,view);
/*					    if (SDS_osnap_chkdsq(pickptrp,p16,osptrp,dsq))
							{
							ic_ptcpy(ospt,p17);
							ic_ptcpy(osptrp,p16);
							}
*/
						osnap_helper_addPtRec( IC_OSMODE_PERP, p17, p16, pickptrp, ptAp1rp, ptAp2rp );
						}	
				    if (-1!=ic_ptlndist(p15,p11,p13,&fr1,p16))
						{
					    gr_ucs2rp(p17,p16,view);
/*					    if (SDS_osnap_chkdsq(pickptrp,p16,osptrp,dsq))
							{
							ic_ptcpy(ospt,p17);
							ic_ptcpy(osptrp,p16);
							}
*/
						osnap_helper_addPtRec( IC_OSMODE_PERP, p17, p16, pickptrp, ptAp1rp, ptAp2rp );
						}
				    if (-1!=ic_ptlndist(p15,p14,p12,&fr1,p16))
						{
					    gr_ucs2rp(p17,p16,view);
/*					    if (SDS_osnap_chkdsq(pickptrp,p16,osptrp,dsq))
							{
							ic_ptcpy(ospt,p17);
							ic_ptcpy(osptrp,p16);
							}
*/
						osnap_helper_addPtRec( IC_OSMODE_PERP, p17, p16, pickptrp, ptAp1rp, ptAp2rp );
						}
				    if (-1!=ic_ptlndist(p15,p14,p13,&fr1,p16))
						{
					    gr_ucs2rp(p17,p16,view);
/*					    if (SDS_osnap_chkdsq(pickptrp,p16,osptrp,dsq))
							{
							ic_ptcpy(ospt,p17);
							ic_ptcpy(osptrp,p16);
							}
*/
						osnap_helper_addPtRec( IC_OSMODE_PERP, p17, p16, pickptrp, ptAp1rp, ptAp2rp );
						}
					}
				}
			}
		if (osmode & IC_OSMODE_MID)//MID
			{
			if (!(fi2 & IC_FACE3D_FACE1INVIS))				  //1-2			
				{
				p5[0]=(p1[0]+p2[0])/2.0;
				p5[1]=(p1[1]+p2[1])/2.0;
				p5[2]=(p1[2]+p2[2])/2.0;
				gr_ucs2rp(p5,p6,view);
/*				if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
					{
					ic_ptcpy(ospt,p5);
					ic_ptcpy(osptrp,p6);
					}
*/
				osnap_helper_addPtRec( IC_OSMODE_MID, p5, p6, pickptrp, ptAp1rp, ptAp2rp );
				}
			if (!strsame(etype,db_hitype2str(DB_3DFACE)))//1-3
				{
				p5[0]=(p1[0]+p3[0])/2.0;
				p5[1]=(p1[1]+p3[1])/2.0;
				p5[2]=(p1[2]+p3[2])/2.0;
				gr_ucs2rp(p5,p6,view);
/*				if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
					{
					ic_ptcpy(ospt,p5);
					ic_ptcpy(osptrp,p6);
					}
*/
				osnap_helper_addPtRec( IC_OSMODE_MID, p5, p6, pickptrp, ptAp1rp, ptAp2rp );
				}
			else if (!(fi2 & IC_FACE3D_FACE2INVIS))			  //2-3
				{
				p5[0]=(p2[0]+p3[0])/2.0;
				p5[1]=(p2[1]+p3[1])/2.0;
				p5[2]=(p2[2]+p3[2])/2.0;
				gr_ucs2rp(p5,p6,view);
/*				if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
					{
					ic_ptcpy(ospt,p5);
					ic_ptcpy(osptrp,p6);
					}
*/
				osnap_helper_addPtRec( IC_OSMODE_MID, p5, p6, pickptrp, ptAp1rp, ptAp2rp );
				}
			if (!(fi2 & IC_FACE3D_FACE3INVIS))				  //3-4
				{
				p5[0]=(p4[0]+p3[0])/2.0;
				p5[1]=(p4[1]+p3[1])/2.0;
				p5[2]=(p4[2]+p3[2])/2.0;
				gr_ucs2rp(p5,p6,view);
/*				if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
					{
					ic_ptcpy(ospt,p5);
					ic_ptcpy(osptrp,p6);
					}
*/
				osnap_helper_addPtRec( IC_OSMODE_MID, p5, p6, pickptrp, ptAp1rp, ptAp2rp );
				}
			if (!strsame(etype,db_hitype2str(DB_3DFACE)))//2-4
				{
				p5[0]=(p4[0]+p2[0])/2.0;
				p5[1]=(p4[1]+p2[1])/2.0;
				p5[2]=(p4[2]+p2[2])/2.0;
				gr_ucs2rp(p5,p6,view);
/*				if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
					{
					ic_ptcpy(ospt,p5);
					ic_ptcpy(osptrp,p6);
					}
*/
				osnap_helper_addPtRec( IC_OSMODE_MID, p5, p6, pickptrp, ptAp1rp, ptAp2rp );
				}
			else if (!(fi2 & IC_FACE3D_FACE4INVIS))			  //1-4
				{
				p5[0]=(p4[0]+p1[0])/2.0;
				p5[1]=(p4[1]+p1[1])/2.0;
				p5[2]=(p4[2]+p1[2])/2.0;
				gr_ucs2rp(p5,p6,view);
/*				if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
					{
					ic_ptcpy(ospt,p5);
					ic_ptcpy(osptrp,p6);
					}
*/
				osnap_helper_addPtRec( IC_OSMODE_MID, p5, p6, pickptrp, ptAp1rp, ptAp2rp );
				}
			if (thickflg)
				{
                if (!(fi2 & IC_FACE3D_FACE1INVIS))				  //1-2			
					{
					p15[0]=(p11[0]+p12[0])/2.0;
				    p15[1]=(p11[1]+p12[1])/2.0;
				    p15[2]=(p11[2]+p12[2])/2.0;
				    gr_ucs2rp(p15,p16,view);
/*				    if (SDS_osnap_chkdsq(pickptrp,p16,osptrp,dsq))
						{
						ic_ptcpy(ospt,p15);
						ic_ptcpy(osptrp,p16);
						}
*/
					osnap_helper_addPtRec( IC_OSMODE_MID, p15, p16, pickptrp, ptAp1rp, ptAp2rp );
					}
			    if (!strsame(etype,db_hitype2str(DB_3DFACE))) //1-3
					{
					p15[0]=(p11[0]+p13[0])/2.0;
				    p15[1]=(p11[1]+p13[1])/2.0;
				    p15[2]=(p11[2]+p13[2])/2.0;
				    gr_ucs2rp(p15,p16,view);
/*				    if (SDS_osnap_chkdsq(pickptrp,p16,osptrp,dsq))
						{
						ic_ptcpy(ospt,p15);
						ic_ptcpy(osptrp,p16);
						}
*/
					osnap_helper_addPtRec( IC_OSMODE_MID, p15, p16, pickptrp, ptAp1rp, ptAp2rp );
					}
				else if (!(fi2 & IC_FACE3D_FACE2INVIS))			  //2-3
					{
					p15[0]=(p12[0]+p13[0])/2.0;
				    p15[1]=(p12[1]+p13[1])/2.0;
				    p15[2]=(p12[2]+p13[2])/2.0;
				    gr_ucs2rp(p15,p16,view);
/*				    if (SDS_osnap_chkdsq(pickptrp,p16,osptrp,dsq))
						{
						ic_ptcpy(ospt,p15);
						ic_ptcpy(osptrp,p16);
						}
*/
					osnap_helper_addPtRec( IC_OSMODE_MID, p15, p16, pickptrp, ptAp1rp, ptAp2rp );
					}
			    if (!(fi2 & IC_FACE3D_FACE3INVIS))				  //3-4
					{
					p15[0]=(p14[0]+p13[0])/2.0;
				    p15[1]=(p14[1]+p13[1])/2.0;
				    p15[2]=(p14[2]+p13[2])/2.0;
				    gr_ucs2rp(p15,p16,view);
/*				    if (SDS_osnap_chkdsq(pickptrp,p16,osptrp,dsq))
						{
						ic_ptcpy(ospt,p15);
						ic_ptcpy(osptrp,p16);
						}
*/
					osnap_helper_addPtRec( IC_OSMODE_MID, p15, p16, pickptrp, ptAp1rp, ptAp2rp );
					}
			    if (!strsame(etype,db_hitype2str(DB_3DFACE)))//2-4
					{
					p15[0]=(p14[0]+p12[0])/2.0;
				    p15[1]=(p14[1]+p12[1])/2.0;
				    p15[2]=(p14[2]+p12[2])/2.0;
				    gr_ucs2rp(p15,p16,view);
/*				    if (SDS_osnap_chkdsq(pickptrp,p16,osptrp,dsq))
						{
						ic_ptcpy(ospt,p15);
						ic_ptcpy(osptrp,p16);
						}
*/
					osnap_helper_addPtRec( IC_OSMODE_MID, p15, p16, pickptrp, ptAp1rp, ptAp2rp );
					}
				else if (!(fi2 & IC_FACE3D_FACE4INVIS))			  //1-4
					{
					p15[0]=(p14[0]+p11[0])/2.0;
				    p15[1]=(p14[1]+p11[1])/2.0;
				    p15[2]=(p14[2]+p11[2])/2.0;
				    gr_ucs2rp(p15,p16,view);
/*				    if (SDS_osnap_chkdsq(pickptrp,p16,osptrp,dsq))
						{
						ic_ptcpy(ospt,p15);
						ic_ptcpy(osptrp,p16);
						}
*/
					osnap_helper_addPtRec( IC_OSMODE_MID, p15, p16, pickptrp, ptAp1rp, ptAp2rp );
					}
				}
			}
		}
	else if (strsame(etype,db_hitype2str(DB_IMAGE)))
		{
		fi2=0;
		for (rbtemp=elist;rbtemp!=NULL;rbtemp=rbtemp->rbnext)
			{
			if (rbtemp->restype==10)
				ic_ptcpy(p1,rbtemp->resval.rpoint);
			else if (rbtemp->restype==11)
				ic_ptcpy(p2,rbtemp->resval.rpoint);
			else if (rbtemp->restype==12)
				ic_ptcpy(p3,rbtemp->resval.rpoint);
			else if (rbtemp->restype==13)
				ic_ptcpy(p4,rbtemp->resval.rpoint);
			else if (rbtemp->restype==70)
				fi2=rbtemp->resval.rint;
			else if (rbtemp->restype==-1)
				ic_encpy(rbent.resval.rlname,rbtemp->resval.rlname);
			}
		rbtemp=NULL;
		//convert it into the 4 corners of the image
		double width = p4[0];
		double height = p4[1];
		p2[0] = p1[0] + p2[0]*width;
		p2[1] = p1[1] + p2[1]*width;
		p2[2] = p1[2] + p2[2]*width;

		p4[0] = p1[0] + p3[0]*height;
		p4[1] = p1[1] + p3[1]*height;
		p4[2] = p1[2] + p3[2]*height;

		p3[0] = p4[0] + (p2[0] - p1[0]);
		p3[1] = p4[1] + (p2[1] - p1[1]);
		p3[2] = p4[2] + (p2[2] - p1[2]);

		if (m_ecs2ucs)
			{
			ic_usemat(p1,p1,m_ecs2ucs,0,0,0);
			ic_usemat(p2,p2,m_ecs2ucs,0,0,0);
			ic_usemat(p3,p3,m_ecs2ucs,0,0,0);
			ic_usemat(p4,p4,m_ecs2ucs,0,0,0);
			}
		else
			{
			sds_trans(p1,&rbent,&rbucs,0,p1);
			sds_trans(p2,&rbent,&rbucs,0,p2);
			sds_trans(p3,&rbent,&rbucs,0,p3);
			sds_trans(p4,&rbent,&rbucs,0,p4);
			}

		if (osmode & IC_OSMODE_INS) 
			{//INS of solid or trace
			p5[0]=p1[0];
			p5[1]=p1[1];
			p5[2]=p1[2];
			gr_ucs2rp(p5,p6,view);
/*			if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
				{
				ic_ptcpy(ospt,p5);
				ic_ptcpy(osptrp,p6);
				}
*/
			osnap_helper_addPtRec( IC_OSMODE_INS, p5, p6, pickptrp, ptAp1rp, ptAp2rp );
			}

		if (osmode & IC_OSMODE_CENTER) 
			{//CEN of solid or trace
			p5[0]=(p1[0]+p2[0]+p3[0]+p4[0])/4.0;
			p5[1]=(p1[1]+p2[1]+p3[1]+p4[1])/4.0;
			p5[2]=(p1[2]+p2[2]+p3[2]+p4[2])/4.0;
			gr_ucs2rp(p5,p6,view);
/*			if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
				{
				ic_ptcpy(ospt,p5);
				ic_ptcpy(osptrp,p6);
				}
*/
				osnap_helper_addPtRec( IC_OSMODE_CENTER, p5, p6, pickptrp, ptAp1rp, ptAp2rp );
			}

		if (osmode & (IC_OSMODE_END | IC_OSMODE_INT | IC_OSMODE_APP | IC_OSMODE_PLAN))
			{	//INT modes or END 
			tmpOsmode = osmode & IC_OSMODE_END;
			if( !tmpOsmode )
				tmpOsmode = osmode & IC_OSMODE_INT;
			if( !tmpOsmode )
				tmpOsmode = osmode & IC_OSMODE_PLAN;
			if( !tmpOsmode )
				tmpOsmode = IC_OSMODE_APP;

			gr_ucs2rp(p1,p5,view);
/*			if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
				{
				ic_ptcpy(ospt,p1);
				ic_ptcpy(osptrp,p5);
				}
*/
			osnap_helper_addPtRec( tmpOsmode, p1, p5, pickptrp, ptAp1rp, ptAp2rp );
			gr_ucs2rp(p2,p5,view);
/*			if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
				{
				ic_ptcpy(ospt,p2);
				ic_ptcpy(osptrp,p5);
				}
*/
			osnap_helper_addPtRec( tmpOsmode, p2, p5, pickptrp, ptAp1rp, ptAp2rp );
			gr_ucs2rp(p3,p5,view);
/*			if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
				{
				ic_ptcpy(ospt,p3);
				ic_ptcpy(osptrp,p5);
				}
*/
			osnap_helper_addPtRec( tmpOsmode, p3, p5, pickptrp, ptAp1rp, ptAp2rp );
			gr_ucs2rp(p4,p5,view);
/*			if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
				{
				ic_ptcpy(ospt,p4);
				ic_ptcpy(osptrp,p5);
				}
*/
			osnap_helper_addPtRec( tmpOsmode, p4, p5, pickptrp, ptAp1rp, ptAp2rp );
			}

		if (osmode & IC_OSMODE_NEAR)
			{//NEA	//NOTE: all nea calcs depend upon view, so they must be done in RP!
			gr_ucs2rp(p1,p5,view);
			gr_ucs2rp(p2,p6,view);
			ic_segdist(pickptrp,p5,p6,0,&fr1,p7);
/*			if (SDS_osnap_chkdsq(pickptrp,p7,osptrp,dsq))
				{
				ic_ptcpy(osptrp,p7);
				fr1=sds_distance(p5,p7)/sds_distance(p5,p6);
				ospt[0]=p1[0]+fr1*(p2[0]-p1[0]);
				ospt[1]=p1[1]+fr1*(p2[1]-p1[1]);
				ospt[2]=p1[2]+fr1*(p2[2]-p1[2]);
				}
*/
			fr1=sds_distance(p5,p7)/sds_distance(p5,p6);
			ucsPt[0]=p1[0]+fr1*(p2[0]-p1[0]);
			ucsPt[1]=p1[1]+fr1*(p2[1]-p1[1]);
			ucsPt[2]=p1[2]+fr1*(p2[2]-p1[2]);
			osnap_helper_addPtRec( IC_OSMODE_NEAR, ucsPt, p7, pickptrp, ptAp1rp, ptAp2rp );

			gr_ucs2rp(p2,p5,view);
			gr_ucs2rp(p3,p6,view);
			ic_segdist(pickptrp,p5,p6,0,&fr1,p7);
/*			if (SDS_osnap_chkdsq(pickptrp,p7,osptrp,dsq))
				{
				ic_ptcpy(osptrp,p7);
				fr1=sds_distance(p5,p7)/sds_distance(p5,p6);
				ospt[0]=p2[0]+fr1*(p3[0]-p2[0]);
				ospt[1]=p2[1]+fr1*(p3[1]-p2[1]);
				ospt[2]=p2[2]+fr1*(p3[2]-p2[2]);
				}
*/
			fr1=sds_distance(p5,p7)/sds_distance(p5,p6);
			ucsPt[0]=p2[0]+fr1*(p3[0]-p2[0]);
			ucsPt[1]=p2[1]+fr1*(p3[1]-p2[1]);
			ucsPt[2]=p2[2]+fr1*(p3[2]-p2[2]);
			osnap_helper_addPtRec( IC_OSMODE_NEAR, ucsPt, p7, pickptrp, ptAp1rp, ptAp2rp );

			gr_ucs2rp(p3,p5,view);
			gr_ucs2rp(p4,p6,view);
			ic_segdist(pickptrp,p5,p6,0,&fr1,p7);
/*			if (SDS_osnap_chkdsq(pickptrp,p7,osptrp,dsq))
				{
				ic_ptcpy(osptrp,p7);
				fr1=sds_distance(p5,p7)/sds_distance(p5,p6);
				ospt[0]=p3[0]+fr1*(p4[0]-p3[0]);
				ospt[1]=p3[1]+fr1*(p4[1]-p3[1]);
				ospt[2]=p3[2]+fr1*(p4[2]-p3[2]);
				}
*/
			fr1=sds_distance(p5,p7)/sds_distance(p5,p6);
			ucsPt[0]=p3[0]+fr1*(p4[0]-p3[0]);
			ucsPt[1]=p3[1]+fr1*(p4[1]-p3[1]);
			ucsPt[2]=p3[2]+fr1*(p4[2]-p3[2]);
			osnap_helper_addPtRec( IC_OSMODE_NEAR, ucsPt, p7, pickptrp, ptAp1rp, ptAp2rp );

			gr_ucs2rp(p4,p5,view);
			gr_ucs2rp(p1,p6,view);
			ic_segdist(pickptrp,p5,p6,0,&fr1,p7);
/*			if (SDS_osnap_chkdsq(pickptrp,p7,osptrp,dsq))
				{
				ic_ptcpy(osptrp,p7);
				fr1=sds_distance(p5,p7)/sds_distance(p5,p6);
				ospt[0]=p4[0]+fr1*(p1[0]-p4[0]);
				ospt[1]=p4[1]+fr1*(p1[1]-p4[1]);
				ospt[2]=p4[2]+fr1*(p1[2]-p4[2]);
				}
*/
			fr1=sds_distance(p5,p7)/sds_distance(p5,p6);
			ucsPt[0]=p4[0]+fr1*(p1[0]-p4[0]);
			ucsPt[1]=p4[1]+fr1*(p1[1]-p4[1]);
			ucsPt[2]=p4[2]+fr1*(p1[2]-p4[2]);
			osnap_helper_addPtRec( IC_OSMODE_NEAR, ucsPt, p7, pickptrp, ptAp1rp, ptAp2rp );
			}

		if (osmode & IC_OSMODE_PERP)
			{//PER
			if ( -1!=ic_ptlndist(lastpt,p1,p2,&fr1,p5))
				{
				gr_ucs2rp(p5,p6,view);
/*				if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
					{
					ic_ptcpy(ospt,p5);
					ic_ptcpy(osptrp,p6);
					}
*/
				osnap_helper_addPtRec( IC_OSMODE_PERP, p5, p6, pickptrp, ptAp1rp, ptAp2rp );
				}
			if (-1!=ic_ptlndist(lastpt,p2,p3,&fr1,p5))
				{
				gr_ucs2rp(p5,p6,view);
/*				if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
					{
					ic_ptcpy(ospt,p5);
					ic_ptcpy(osptrp,p6);
					}
*/
				osnap_helper_addPtRec( IC_OSMODE_PERP, p5, p6, pickptrp, ptAp1rp, ptAp2rp );
				}
			if (-1!=ic_ptlndist(lastpt,p3,p4,&fr1,p5))
				{
				gr_ucs2rp(p5,p6,view);
/*				if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
					{
					ic_ptcpy(ospt,p5);
					ic_ptcpy(osptrp,p6);
					}
*/
				osnap_helper_addPtRec( IC_OSMODE_PERP, p5, p6, pickptrp, ptAp1rp, ptAp2rp );
				}
			if (-1!=ic_ptlndist(lastpt,p4,p1,&fr1,p5))
				{
				gr_ucs2rp(p5,p6,view);
/*				if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
					{
					ic_ptcpy(ospt,p5);
					ic_ptcpy(osptrp,p6);
					}
*/
				osnap_helper_addPtRec( IC_OSMODE_PERP, p5, p6, pickptrp, ptAp1rp, ptAp2rp );
				}
			}

		if (osmode & IC_OSMODE_MID)
			{//MID
			p5[0]=(p1[0]+p2[0])/2.0;
			p5[1]=(p1[1]+p2[1])/2.0;
			p5[2]=(p1[2]+p2[2])/2.0;
			gr_ucs2rp(p5,p6,view);
/*			if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
				{
				ic_ptcpy(ospt,p5);
				ic_ptcpy(osptrp,p6);
				}
*/
			osnap_helper_addPtRec( IC_OSMODE_MID, p5, p6, pickptrp, ptAp1rp, ptAp2rp );

			p5[0]=(p2[0]+p3[0])/2.0;
			p5[1]=(p2[1]+p3[1])/2.0;
			p5[2]=(p2[2]+p3[2])/2.0;
			gr_ucs2rp(p5,p6,view);
/*			if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
				{
				ic_ptcpy(ospt,p5);
				ic_ptcpy(osptrp,p6);
				}
*/
			osnap_helper_addPtRec( IC_OSMODE_MID, p5, p6, pickptrp, ptAp1rp, ptAp2rp );

			p5[0]=(p4[0]+p3[0])/2.0;
			p5[1]=(p4[1]+p3[1])/2.0;
			p5[2]=(p4[2]+p3[2])/2.0;
			gr_ucs2rp(p5,p6,view);
/*			if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
				{
				ic_ptcpy(ospt,p5);
				ic_ptcpy(osptrp,p6);
				}
*/
			osnap_helper_addPtRec( IC_OSMODE_MID, p5, p6, pickptrp, ptAp1rp, ptAp2rp );

			p5[0]=(p4[0]+p1[0])/2.0;
			p5[1]=(p4[1]+p1[1])/2.0;
			p5[2]=(p4[2]+p1[2])/2.0;
			gr_ucs2rp(p5,p6,view);
/*			if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
				{
				ic_ptcpy(ospt,p5);
				ic_ptcpy(osptrp,p6);
				}
*/
			osnap_helper_addPtRec( IC_OSMODE_MID, p5, p6, pickptrp, ptAp1rp, ptAp2rp );
			}
		}

	else if ((strsame(etype,db_hitype2str(DB_TEXT)))||
			 (strsame(etype,db_hitype2str(DB_SHAPE)))||
			 (strsame(etype,db_hitype2str(DB_ATTDEF)))||
			 (strsame(etype,db_hitype2str(DB_ATTRIB))))
		{
		if (osmode & IC_OSMODE_INS)//INS
			{
			ic_assoc(elist,-1);
			ic_encpy(rbent.resval.rlname,ic_rbassoc->resval.rlname);
			fi1=0;
			if (strsame(etype,db_hitype2str(DB_TEXT)) || strsame(etype,db_hitype2str(DB_ATTDEF)) || strsame(etype,db_hitype2str(DB_ATTRIB)))
				{
				if (strsame(etype,db_hitype2str(DB_TEXT)))
					fi2=73;
				else 
					fi2=74;
				for (rbtemp=elist;rbtemp!=NULL;rbtemp=rbtemp->rbnext)
					{
					if (rbtemp->restype==fi2 && rbtemp->resval.rint>0)
						{
						fi1=1;
						break;
						}
					else if (rbtemp->restype==72 && (rbtemp->resval.rint==1 || rbtemp->resval.rint==2 || rbtemp->resval.rint==4))
						{
						fi1=1;
						break;
						}
					}
				rbtemp=NULL;
				}
			if (fi1)
				ic_assoc(elist,11);
			else 
				ic_assoc(elist,10);
			if (thickflg)
				ic_ptcpy(p15,ic_rbassoc->resval.rpoint);
			if (m_ecs2ucs)
				ic_usemat(ic_rbassoc->resval.rpoint,p5,m_ecs2ucs,0,0,0);
			else
				sds_trans(ic_rbassoc->resval.rpoint,&rbent,&rbucs,0,p5);
			gr_ucs2rp(p5,p6,view);
/*			if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
				{
				ic_ptcpy(ospt,p5);
				ic_ptcpy(osptrp,p6);
				}
*/
			osnap_helper_addPtRec( IC_OSMODE_INS, p5, p6, pickptrp, ptAp1rp, ptAp2rp );
            if (thickflg)
				{
				p15[0]+=thickness*p210[0];
				p15[1]+=thickness*p210[1];
				p15[2]+=thickness*p210[2];
				if (m_ecs2ucs)
					ic_usemat(p15,p15,m_ecs2ucs,0,0,0);
				else
					sds_trans(p15,&rbent,&rbucs,0,p15);
			    gr_ucs2rp(p15,p16,view);
/*			    if (SDS_osnap_chkdsq(pickptrp,p16,osptrp,dsq))
					{
					ic_ptcpy(ospt,p15);
					ic_ptcpy(osptrp,p16);
					}
*/
				osnap_helper_addPtRec( IC_OSMODE_INS, p15, p16, pickptrp, ptAp1rp, ptAp2rp );
	            }
			}
		}
    //<alm>
	else if (strsame(etype, db_hitype2str(DB_3DSOLID)) ||
             strsame(etype, db_hitype2str(DB_BODY)) ||
             strsame(etype, db_hitype2str(DB_REGION)))
    {
        using icl::gvector;
        using icl::point;
        using icl::transf;
        do
        {
            // Acic modeler should work
            if (!CModeler::get()->canView())
                break;

            // get 1-st group (sat data)
            if (ic_assoc(elist, 1))
                break;

            CDbAcisEntityData obj(ic_rbassoc->resval.rstring, etype, CDbAcisEntityData::eDbType);

            gvector dir;
            {
                resbuf rb;
                sds_getvar("VIEWDIR", &rb);
                dir = rb.resval.rpoint;
            }

            // get points for snaping
            point pick(pickptucs);
			point last(lastpt);
			transf u2w = ucs2wcs();
			pick *= u2w;
			last *= u2w;
			dir *= u2w;

            xstd<snaping>::vector pnts;
            if (!CModeler::get()->getSnapingPoints(obj, osmode, pick, last, dir, pnts))
                break;
            if (pnts.empty())
                break;

            transf w2u = wcs2ucs();
            xstd<snaping>::vector::iterator pnt = pnts.begin();
            for (; pnt != pnts.end(); ++pnt)
            {
                sds_point rp;
                point upnt = (pnt->first)*w2u;
                gr_ucs2rp(upnt, rp, view);
                osnap_helper_addPtRec(pnt->second, upnt, rp, pickptrp, ptAp1rp, ptAp2rp);
            }
        }
        while (false);
    }//</alm>
exit_point:
	return(ret);
}

////////////////////////////////////////////////////////////////////////////////////////////////

void osnap_helper_addPtRec( int osmode, sds_point snapPt, sds_point snapPtRP, sds_point inPtRP, sds_point ptAp1rp, sds_point ptAp2rp )
{
	// Added point here is in UCS
	COsnapMode *pTmpMode = NULL;
	COsnapManagerImp *pOsnapManager = static_cast<COsnapManagerImp*>(((CEngineApplication*)SDSApplication::GetEngineApplication())->getOsnapManager());
	
	if( pTmpMode = pOsnapManager->findMode( osmode ) )
		pOsnapManager->addOsnapRecord( pTmpMode, snapPt, snapPtRP, inPtRP, ptAp1rp, ptAp2rp ); 
}

void freePFaceEdgeList( struct pface_edge *list )
{
	struct pface_edge *tmpList = NULL;
	while( list )
	{
		tmpList = list->next;
		delete list ;
		list = tmpList;
	}
}
