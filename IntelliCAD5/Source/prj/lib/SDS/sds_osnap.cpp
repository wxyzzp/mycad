/*	LIB\SDS\SDS_OSNAP.CPP
 *	Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 *	Abstract
 *
 *  Object Snap Functions
 *
 */

#include "Icad.h"/*DNT*/
//#include "sdsapplication.h" /*DNT*/
#include "lisp.h"/*DNT*/
#include "cmds.h"/*DNT*/
#include "ListColl.h"
#include "IcadView.h"
#include "IcadEntityRenderer.h"
#include "IcadHelpers.h"/*DNT*/
#include "TransformUtils.h"
#include "elliparc.h"
#include "nurbs.h"
#include "splineutils.h"


#if defined(SDS_NEWDB)
// TODO RENAME THESE THINGS WITH THE DATABASE CHANGE!!!!
    #define gra_view        gr_view
    #define gra_freeviewll	gr_freeviewll
    #define gra_initview	gr_initview
    #define gra_rp2pix		gr_rp2pix
    #define gra_pix2rp		gr_pix2rp
    #define gra_rp2ucs		gr_rp2ucs
    #define gra_ucs2rp		gr_ucs2rp
    #define dwg_filelink	db_drawing
    #define dwg_endlink		db_handitem
#endif

//** Globals
signed char SDS_OSNAP_TANPER_IGNORE;
bool SDS_IsFromOsnapToXref = false;		/*D.G.*/// Used only in sds_osnap & sdsengine_entupd
    									// (look at comments in sds_osnap).

extern int		 SDS_AtNodeDrag;
extern sds_name  SDS_NodeDragEnt;

//	//Modified Cybage SBD 12/05/2001 DD/MM/YYYY
//	//Reason : Implementation of Flyover Snapping
//	//Snapping type filled in this variable
//	extern int g_nSnappedType;

/****************************************************************************/
/********************* Prototypes *******************************************/
/****************************************************************************/
int SDS_osnap_chkdsq(sds_point pickpt, sds_point chkpt, sds_point ospt, sds_real *dsq);
int SDS_osnap_elist(struct resbuf *elist, int osmode, sds_point pickptrp, sds_point pickptucs, sds_point ospt, sds_real *dsq,int pl_omitflag,sds_matrix *m_ecs2ucs);
sds_real sds_dist2d (sds_point p1,sds_point p2);

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


/****************************************************************************/
/********************* O-Snap functions *******************************************/
/****************************************************************************/


// *****************************************
// This is an SDS External API
//
int
sdsengine_osnap(const sds_point ptAperCtr, const char *szSnapModes, sds_point ptPoint)
{

    ASSERT( OnEngineThread() );

    //This f'n finds the entity point closest to the selected point ptAperCtr
    //	AS SEEN ON THE SCREEN!  (ptAperCtr is converted from UCS into RP coords)
    //Entity modes are the noted below.  If *szSnapModes is null, OSMODE var
    //	is used. If you want none, you must pass MODE as "NONE"
    //We'll actually track the square of the distance(dsq)since we're just
    //	after a minimum.  That way we don't have to use sds_distance.
    //NOTE:  Modes have following priority if distances are equal:
    //	Highest->Lowest: END,NEA,PER,QUA,INS,CEN,MID,
    //								NOD(POI),TAN,QUICK,INT
    //		(acad checks INTs after MIDs)
    /*
    		Bit			Priority		Snap
    		1			1				END
    		2			7				MID
    		32			12(8 in Acad)	INT
    		2048		11(not done)	APP (apparent int)
    		4			6				CEN
    		16			4				QUA
    		128			3				PER
    		256			10				TAN
    		8			9				NOD
    		64			5				INS
    		512			2				NEA
    		1024		11				QUI
    */
    //	We accept either NOD or POI for ACAD's NODE, which we call POInt.
    //	Modes will always be checked in this order(if applicable).
    //If quick mode enabled, we only get 1st valid point instead
    //	of the closest point
    //PER and TAN modes use the LASTPOINT var as the pt to be
    //	perpendicular or tangent to.
    //All modes EXCEPT INT are checked by subroutine.  This was just
    //	easier. To check Vertices, add a resbuf w/11 group to mark endpt
    //	of that particular vertex.
    //		BUG!!!
    //THIS FUNCTION IS SLIGHTLY DIFFERENT FROM ACAD'S FOR INT MODE!
    //	our intersection is a 2d inters (using ic_linexline, ic_linexarc).
    //	ic_ssxss calls ic_segxseg which calls ic_segdist. All need to be
    //	investigated.
    //All the intersection work is done by subroutine SDS_osnap_elist EXCEPT
    //	for closed pline with all straight segments and for INT corners of
    //  SOLIDs, TRACEs & 3DFACEs.  If CEN is one of our modes,
    //	we check it first here (subroutine is one entity only).

//		//Modified Cybage SBD 12/05/2001 DD/MM/YYYY
//		//Reason : Implementation of Flyover Snapping [
//		resbuf rbs = { 0 } ;
//		sds_getvar( "AUTOSNAP", &rbs ) ;
//		int iFlyOverSnap = rbs.resval.rint; //checks if flyover snapping is on/off
//		//Modified Cybage SBD 12/05/2001 DD/MM/YYYY ]
//
    sds_point ospt,pickptrp,p1,p2,p3,p4,p5,p6,p7,p9,ll,ur,p210;
    sds_real dsq,fr1,fr2,aperture;
    sds_name ss,ent,etemp;
    sds_name ss2={0,0};
    struct resbuf rb,rb2,*elist=NULL,*rbptemp=NULL, *polyelist=NULL;
    int  ret=RTNORM,osmode=0,closureflg,mct,nct,fi1,plcentroid,pl_omitflag;
    long vtxcnt,fl1,fl2;
    LPCTSTR fcp1;
    TCHAR	mode2[IC_ACADBUF];
    sds_matrix *m_ecs2ucs;
    gr_selectorlink  *sl=NULL;
    db_handitem *subenthip;
    sds_matrix blkmtx;
    db_handitem **savehip=NULL;
    bool bExplodeBlock=FALSE;
    db_handitem *BlockToExplode;
    HanditemCollection coll;
//	CHanditemCollectionWXRefs coll2;  //revert to the old implementation of snapping using dispobjs
    double maxDistFromPickBoxCen;

    struct gra_view *view=SDS_CURGRVW;
    if (view==NULL) return(RTERROR);

    struct pt_bulg *ptlst=NULL;

    struct pface_edge *pface_beg,*pface_cur,*pface_tmp;
    pface_beg=pface_cur=pface_tmp=NULL;

    strncpy(mode2,szSnapModes,sizeof(mode2)-1);
    ic_ptcpy(ospt,ptAperCtr);	//default to picked ptAperCtr
    gra_ucs2rp(ospt,pickptrp,view);  //won't take a const, so cast it

    //NOTE: DSQ will track square of distance between pt picked by user & osnap pt
    //			If dsq!=-1.0, then we found a point and can return RTNORM!!!
    dsq=-1.0;

    if ((*szSnapModes)==0)
    	{
    	if (SDS_getvar(NULL,DB_QOSMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) {ret=RTERROR;goto exit_point;}
    	osmode=rb.resval.rint;
    	}
    else
    	{
    	//get mode to use
    	if ((fcp1=stristr(mode2,ResourceString(IDC_SDS_OSNAP_END_0, "END" )))!=NULL)
    		osmode |= IC_OSMODE_END;
    	else if ((fcp1=stristr(mode2,"_END"/*DNT*/))!=NULL)
    		osmode |= IC_OSMODE_END;
    	if ((fcp1=stristr(mode2,ResourceString(IDC_SDS_OSNAP_MID_1, "MID" )))!=NULL)
    		osmode |= IC_OSMODE_MID;
    	else if ((fcp1=stristr(mode2,"_MID"/*DNT*/))!=NULL)
    		osmode |= IC_OSMODE_MID;
    	if ((fcp1=stristr(mode2,ResourceString(IDC_SDS_OSNAP_CEN_2, "CEN" )))!=NULL)
    		osmode |= IC_OSMODE_CENTER;
    	else if ((fcp1=stristr(mode2,"_CEN"/*DNT*/))!=NULL)
    		osmode |= IC_OSMODE_CENTER;
    	if ((fcp1=stristr(mode2,ResourceString(IDC_SDS_OSNAP_NOD_3, "NOD" )))!=NULL)
    		osmode |= IC_OSMODE_NODE;
    	else if ((fcp1=stristr(mode2,"_NOD"/*DNT*/))!=NULL)
    		osmode |= IC_OSMODE_NODE;
    	if ((fcp1=stristr(mode2,ResourceString(IDC_SDS_OSNAP_POI_4, "POI" )))!=NULL)
    		osmode |= IC_OSMODE_NODE;
    	else if ((fcp1=stristr(mode2,"_POI"/*DNT*/))!=NULL)
    		osmode |= IC_OSMODE_NODE;
    	if ((fcp1=stristr(mode2,ResourceString(IDC_SDS_OSNAP_QUA_5, "QUA" )))!=NULL)
    		osmode |= IC_OSMODE_QUADRANT;
    	else if ((fcp1=stristr(mode2,"_QUA"/*DNT*/))!=NULL)
    		osmode |= IC_OSMODE_QUADRANT;
    	if ((fcp1=stristr(mode2,ResourceString(IDC_SDS_OSNAP_INT_6, "INT" )))!=NULL)
    		osmode |= IC_OSMODE_INT;
    	else if ((fcp1=stristr(mode2,"_INT"/*DNT*/))!=NULL)
    		osmode |= IC_OSMODE_INT;
    	if ((fcp1=stristr(mode2,ResourceString(IDC_SDS_OSNAP_INS_7, "INS" )))!=NULL)
    		osmode |= IC_OSMODE_INS;
    	else if ((fcp1=stristr(mode2,"_INS"/*DNT*/))!=NULL)
    		osmode |= IC_OSMODE_INS;
    	if ((fcp1=stristr(mode2,ResourceString(IDC_SDS_OSNAP_PER_8, "PER" )))!=NULL)
    		osmode |= IC_OSMODE_PERP;
    	else if ((fcp1=stristr(mode2,"_PER"/*DNT*/))!=NULL)
    		osmode |= IC_OSMODE_PERP;
    	if ((fcp1=stristr(mode2,ResourceString(IDC_SDS_OSNAP_TAN_9, "TAN" )))!=NULL)
    		osmode |= IC_OSMODE_TAN;
    	else if ((fcp1=stristr(mode2,"_TAN"/*DNT*/))!=NULL)
    		osmode |= IC_OSMODE_TAN;
    	if ((fcp1=stristr(mode2,ResourceString(IDC_SDS_OSNAP_NEA_10, "NEA" )))!=NULL)
    		osmode |= IC_OSMODE_NEAR;
    	else if ((fcp1=stristr(mode2,"_NEA"/*DNT*/))!=NULL)
    		osmode |= IC_OSMODE_NEAR;
    	if ((fcp1=stristr(mode2,ResourceString(IDC_SDS_OSNAP_QUI_11, "QUI" )))!=NULL)
    		osmode |= IC_OSMODE_QUICK;
    	else if ((fcp1=stristr(mode2,"_QUI"/*DNT*/))!=NULL)
    		osmode |= IC_OSMODE_QUICK;
    	if ((fcp1=stristr(mode2,ResourceString(IDC_SDS_OSNAP_APP_12, "APP" )))!=NULL)
    		{
    		//osmode |= IC_OSMODE_APP;
    		sds_printf (ResourceString(CMD_ERR_UNRECOGSNAP, "Unable to recognize entity snap mode/s."));
    		}
    	if ((fcp1=stristr(mode2,ResourceString(IDC_SDS_OSNAP_VIS_13, "VIS" )))!=NULL)
    		{
    		//osmode |= IC_OSMODE_APP;
    		sds_printf (ResourceString(CMD_ERR_UNRECOGSNAP, "Unable to recognize entity snap mode/s."));
    		}
    	if ((fcp1=stristr(mode2,ResourceString(IDC_SDS_OSNAP_PLA_14, "PLA" )))!=NULL)
    		osmode |= IC_OSMODE_PLAN;
    	else if ((fcp1=stristr(mode2,"_PLA"/*DNT*/))!=NULL)
    		osmode |= IC_OSMODE_PLAN;
    	}
    if (osmode==0)
    	{
    	ic_ptcpy(ptPoint,ospt);
    	goto exit_point;
    	}

    fi1=(osmode & (IC_OSMODE_PLAN | IC_OSMODE_APP | IC_OSMODE_INT));
    //if multiple bits for INT set, only use bit for 3d int
    if (fi1>0 && fi1!=IC_OSMODE_INT && fi1!=IC_OSMODE_APP && fi1!=IC_OSMODE_PLAN)
    	osmode=IC_OSMODE_INT;

    if (SDS_getvar(NULL,DB_QVIEWSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) {ret=RTERROR;goto exit_point;}
    aperture=rb.resval.rreal;
    if (SDS_getvar(NULL,DB_QSCREENSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) {ret=RTERROR;goto exit_point;}
    aperture/=rb.resval.rpoint[1];
    if (SDS_getvar(NULL,DB_QAPERTURE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) {ret=RTERROR;goto exit_point;}
    aperture*=rb.resval.rint;
    //let p1 & p2 be the corners of the aperture window in rp
    //let p3 & p4 be the opposite corners in rp
    p1[0]=pickptrp[0]-aperture;//p1 must be lower left
    p1[1]=pickptrp[1]-aperture;
    p2[0]=pickptrp[0]+aperture;
    p2[1]=pickptrp[1]+aperture;
    p1[2]=p2[2]=0.0;
    ic_ptcpy(p3,p1);
    p3[1]=p2[1];
    ic_ptcpy(p4,p1);
    p4[0]=p2[0];
    maxDistFromPickBoxCen = (sds_distance(p1,p2)) / 2;

    //project all pts back and get crossing polygon for which objects to consider for
    //	snap selection.
    gra_rp2ucs(p1,p5,view);
    gra_rp2ucs(p2,p6,view);
    gra_rp2ucs(p3,p3,view);
    gra_rp2ucs(p4,p4,view);
    FreeResbufIfNotNull(&polyelist);
    polyelist=sds_buildlist(RT3DPOINT,p5,RT3DPOINT,p3,RT3DPOINT,p6,RT3DPOINT,p4,0);
    ss[1]=0L;  // this ssget seems to be having trouble sometimes.
    if ((ret=sds_pmtssget(NULL,"CPOLYGON"/*DNT*/,polyelist,NULL,NULL,ss,0,true,true))!=RTNORM)
    	goto exit_point;

    if (SDS_OSNAP_TANPER_IGNORE==-1)
    	{
#if 1//<alm>: move the flag manipulation into SDS_osnap_elist
    	//If we're snapping tangent or perpendicular in the LINE command, the
    	//	osnap MAY depend upon a pt not yet selected.  The -1 flags this...
    	SDS_OSNAP_TANPER_IGNORE=0;
    	if (osmode & IC_OSMODE_PERP)
    		{
    		osmode &= ~IC_OSMODE_PERP;
    		SDS_OSNAP_TANPER_IGNORE |= 1;//we'll set bit back to 0 if nothing ultimately found
    		}
    	if (osmode & IC_OSMODE_TAN)
    		{
    		osmode &= ~IC_OSMODE_TAN;
    		SDS_OSNAP_TANPER_IGNORE |= 2;
    		}
#endif
    	if (osmode==0)
    		{
    		ic_ptcpy(ptPoint,ospt);
    		dsq=0.0;
    		ret=RTNORM;
    		goto exit_point;
    		}
    	}
    //keep p1 & p2 for later use in weeding out pline vertices not near pickpt &
    //	for selecting ents within blocks
    //	- NOTE:they're in RP coords
    FreeResbufIfNotNull(&elist);
    sds_sslength(ss,&fl1);
    long	SSLen;
    sds_sslength(ss, &SSLen);
    if (fl1==0L)
    	{
    	ret=RTNONE;
    	goto exit_point;
    	}
	//Bugzilla ::78333; 03-03-03;
	//  if (fl1 > 25)  	//this is an arbitrary limit - tweak it if necessary
	if (fl1 > 75)  	
    	{
//			//Modified Cybage SBD 12/05/2001 DD/MM/YYYY
//			//Reason : Implementation of Flyover Snapping
//			if(!iFlyOverSnap) //print only when flyover snapping is off
//				{
    		sds_printf (ResourceString(IDC_SDS_OSNAP_ERROR1, "\nToo many entities selected.  No valid snap was found."));
//				}
    	ret=RTERROR;
    	goto exit_point;
    	}

    while(sds_ssname(ss,0L,ent)==RTNORM)
    	{
    	if (1==SDS_AtNodeDrag && ent[0]==SDS_NodeDragEnt[0] && ent[1]==SDS_NodeDragEnt[1])
    		{
    		//if we're node dragging an entity, exclude it from all osnap settings
    		sds_ssdel(ent,ss);
    		continue;
    		}

    	/*D.G.*/// TO DO: We should zero m_ecs2ucs only when all nested ents are off!
    	m_ecs2ucs=NULL;	//use pointer to matrix, and then we can point it to blkmtx
    						//	as needed!
    	nested_ent:	/*****NESTED ENTS JUMP IN HERE WITH ENAME & PBLKMTX SET*****/

    	//check to see if dist from picked pt is 0.0 or if
    	//we're in quick mode and dist != -1.0 - if so, we be done.
    	if ((osmode & IC_OSMODE_QUICK) && dsq!=-1.0)
    		break;
    	if (icadRealEqual(dsq,0.0))
    		break;

    	pl_omitflag=0;	//when checking plines, elist checking routine will check vertices
    					//as "intersections" unless this flag set. If point is at the beginning of
    					//2D open pline, for example, we set this flag.


    	FreeResbufIfNotNull(&elist);
    	if ((elist=sds_entget(ent))==NULL) {ret=RTERROR;goto exit_point;}
    	ic_assoc(elist,0);
    	if (strsame(ic_rbassoc->resval.rstring,db_hitype2str(DB_INSERT)) ||
    		strsame(ic_rbassoc->resval.rstring,db_hitype2str(DB_DIMENSION)))
    		{
    		//TODO osnap to entities within inserts!!!
    		if (osmode & IC_OSMODE_INS)
    			{
    			p2[0]=p2[1]=0.0;p2[2]=1.0;
    			fi1=0;
    			for (rbptemp=elist;rbptemp!=NULL;rbptemp=rbptemp->rbnext)
    				{
    				if (rbptemp->restype==10)
    					ic_ptcpy(p1,rbptemp->resval.rpoint);
    				else if (rbptemp->restype==66 && strsame(ic_rbassoc->resval.rstring,db_hitype2str(DB_INSERT)))
    					fi1=rbptemp->resval.rint;
    				else if (rbptemp->restype==210)
    					ic_ptcpy(p2,rbptemp->resval.rpoint);
    				}
    			rb.rbnext=rb2.rbnext=NULL;
    			rb.restype=RT3DPOINT;
    			rb2.restype=RTSHORT;
    			ic_ptcpy(rb.resval.rpoint,p2);
    			rb2.resval.rint=1;	//ucs
    			sds_trans(p1,&rb,&rb2,0,p2);
    			gra_ucs2rp(p2,p3,view);
    			if (SDS_osnap_chkdsq(pickptrp,p3,ospt,&dsq))
//					{
    				ic_ptcpy(ospt,p1);
//						g_nSnappedType= IC_OSMODE_INS;
//					}
    			if (fi1>0)
    				{
    				//if an insert w/attribs...
    				FreeResbufIfNotNull(&rbptemp);
    				for (ic_encpy(etemp,ent);RTNORM==sds_entnext_noxref(etemp,etemp); )
    					{
    					if (NULL==(rbptemp=sds_entget(etemp)))
    						break;
    					ic_assoc(rbptemp,0);
    					if (!strsame(ic_rbassoc->resval.rstring,db_hitype2str(DB_ATTRIB)))
    						break;
    					SDS_osnap_elist(rbptemp,osmode,pickptrp,(double *)ptPoint,ospt,&dsq,pl_omitflag,m_ecs2ucs);
    					FreeResbufIfNotNull(&rbptemp);
    					}
    				}
    			}
    		int is_db_insert = strsame(ic_rbassoc->resval.rstring,db_hitype2str(DB_INSERT));
    		int is_db_dimension = strsame(ic_rbassoc->resval.rstring,db_hitype2str(DB_DIMENSION));

    		if (osmode & (~IC_OSMODE_INS) && (is_db_insert || is_db_dimension))
    			{
    			//if any other bits are set...
    			rbptemp=sds_buildlist(RT3DPOINT,p1,RT3DPOINT,p2,0);
    			sl=gr_initselector('W','C',rbptemp);
    			FreeResbufIfNotNull(&rbptemp);

    			// get the main view's target device DC in case we have any TrueType text
    			CDC *dc = NULL;
    			CIcadView *icadView = SDS_CMainWindow->GetIcadView();
    			if (icadView)
    				{
    				ASSERT_KINDOF(CIcadView, icadView);
    				dc = icadView->GetTargetDeviceCDC();
    				}

    			int result = gr_nentselphelper( 0,
    										   (db_handitem *)ent[0],
    										   sl,
    										   view,
    										   SDS_CURDWG,
    										   dc,
    										   &subenthip,
    										   blkmtx,		//mtx is ecs to wcs
    										   NULL);

    			if (result)
    				{
    				gr_freeselector(sl);
    				sl=NULL;
    				continue;
    				}
    			//if we got this far, set the matrix pointer and ename, then go to the
    			//	beginning of the loop after the matrix check...
    			BlockToExplode = (db_handitem*)ent[0];
    			sds_ssdel(ent,ss);
    			// Transform matrix to UCS basis
    			sds_matrix	uw, wu, tmpMtx;
    			ic_idmat( uw );
    			ic_idmat( wu );
    			uc2wcMatrix( wu );
    			wc2ucMatrix( uw );
    			multMtx2Mtx( blkmtx, wu, tmpMtx );
    			multMtx2Mtx( uw, tmpMtx, blkmtx );
    			//
    			m_ecs2ucs=&blkmtx;
    			if (subenthip && ent[0]!=(long)subenthip)	//if nentselphelper fails, we might get back the same entity
    				{
    				//we've found an entity inside the block to which to snap to.  Copy it, transform the copy, and add it
    				//to the select set, and proceed as normal.  The copy is required, so that we dont transform the original.
    				int i=0;
    				db_handitem *starthip;
    				starthip = subenthip;
    				while (subenthip)
    					{
    					subenthip = subenthip->next;
    					i++;
    					};

    				savehip = new db_handitem *[i];
    				for(int j = 0; j < i; j++)	/*D.G.*/// for safety.
    					savehip[j] = NULL;

    				//HACK - SDS_simplexforment will trigger a call to entmod which gets logged for undo
    				//However, we are working with temp entities, which should never be logged for undo.
    				//The hack is to temporarily turn Undo off before xforment and turn it back on after.
    				struct resbuf undoctl;
					bool bUndoOn = false;
    				SDS_getvar(NULL,DB_QUNDOCTL,&undoctl,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
    				if (undoctl.resval.rint & IC_UNDOCTL_ON)
    					{
    					undoctl.resval.rint &= ~IC_UNDOCTL_ON;  // turn Undo off
    					sds_setvar("UNDOCTL",&undoctl);
						bUndoOn = true;
    					}

    				//Since we will be changing a global value, enclose it in a CriticalSection, so
    				//that none of the other threads will see this temporary change.
    				SingleThreadSection localCriticalSection;

    				localCriticalSection.Enter();

    				//Trick to prevent callbacks on these temporary entities
    				extern bool	  SDS_SetAUserCallback;
    				bool Save_SDS_SetAUserCallback = SDS_SetAUserCallback;
    				SDS_SetAUserCallback = FALSE;

    				subenthip=starthip;
    				i=0;

    				SDS_IsFromOsnapToXref = true;	/*D.G.*/// We don't redraw temporary entities
    					                 			// which are created from xref blocks in sub-
    												// sequential calls of sdsengine_entupd (from SDS_xforment).
 				// Here is the loop thru all subentites starting from the 1st one - subenthip - which falls into the aperture.
    				while (subenthip)
    					{
    					/*D.G.*/// Here is the new addition to snapping to inserts: we add a "subenthip" to "ss"
    					// only if it intersects "sl". The code is taken from SDS_PickObjects and changed a little.
    					// If some serious problems arises with this new addition just comment out the block
    					// in "{}" brackets below and set "ToAdd" to "true".
    					// TO DO. Note, we've already allocated the "savehip" array, but in most cases it's too big
    					// for us. So, make a linked list instead of it.
    					// Also, I'm not sure about frosen/locked layers.
 						int				EntType = subenthip->ret_type();
						bool ToAdd = false;
						{

    						gr_displayobject	*DispObjBeg, *DispObjEnd;
    						db_handitem			*TmpHip, *LayerHip;
    						sds_name			TmpEname;

    						TmpHip = subenthip->newcopy();	// Note, we don't copy DispObjs
    						TmpEname[0] = (long)TmpHip;
    						TmpEname[1] = (long)SDS_CURDWG;

						/*D.G.*/// VERY important: stop processing at the end of the block/insert.
						// Old code commented out below.
						if(EntType == DB_ENDBLK || EntType == DB_SEQEND)
							{
							delete TmpHip;
							break;
							}

						// Check for deleted objects and end stuff.
						if(TmpHip->ret_deleted() || TmpHip->ret_invisible() /*|| EntType == DB_ENDBLK*/)
							{
							delete TmpHip;
							subenthip = subenthip->next;
							continue;
							}

						/*D.G.*/// Enable snapping to ents on locked layers: temporally onlock layer.
						int				layerFlags = 0;
						db_handitem*	pLayer = TmpHip->ret_layerhip();

						if(pLayer)
						{
							pLayer->get_70(&layerFlags);
							pLayer->set_70(layerFlags & ~IC_LAYER_LOCKED);
						}

						SDS_xforment(TmpEname, NULL, blkmtx);

						if(pLayer)
							pLayer->set_70(layerFlags);

    						// special handling for text drawn with a TrueType font
    						// a similar approach should also work for images.
    						if( (DB_TEXT == EntType || DB_ATTDEF == EntType || DB_ATTRIB == EntType) &&
    							TrueTypeTextForGDI(TmpHip, view) )
    							{
    							sds_point corner1, corner2, corner3, corner4, corner5;
    							GetGDITrueTypeTextBox(subenthip, corner1, corner2, corner3, corner4, corner5);	// Note, using subenthip, not TmpHip because of DispObjs.

    							bool success;

    							CParallelogram EntityExtents(THREEPOINTS_t, corner2, corner1, corner4, success);

    							CRectangle PickWindow(p1, p2);

    							if(PickWindow.intersects(EntityExtents))
    								ToAdd = true;
    							}
    						else
    							{
    							db_handitem*	telp = TmpHip;

    							// check a layer
    							LayerHip = TmpHip->ret_layerhip();
    							if(LayerHip)
    								{
    								int flags;
									if(LayerHip->get_70(&flags) && (flags & IC_LAYER_FROZEN))
										goto out;
    								}

    							/*D.G.*/// TO DO: We should get DispObjs of the original polyline (not of the copy
    							// because its SubEnts are not copied) transformed and add its copy and copies of
    							// its SubEnts to the ss if it's "gr_selected"). We add all polylines now :(
    							if(EntType == DB_POLYLINE || EntType == DB_VERTEX || EntType == DB_SEQEND)
    							{
    								ToAdd = true;
    								goto out;
    							}

    							if(SDS_getdispobjs(SDS_CURDWG, &TmpHip, &DispObjBeg, &DispObjEnd, NULL, SDS_CURDWG, view, 1))
    								{
    								delete TmpHip;
    								subenthip = subenthip->next;
    								continue;
    								}
    							TmpHip = telp;
    							TmpHip->set_disp((void *)DispObjBeg);

    							if(gr_selected(TmpHip, sl))
    								{
    								// Must do a special case to handle attributes.
    								if(EntType == DB_ATTRIB)
    									{
    									db_handitem*	telp = TmpHip;
    									sds_name		ename;
    									for(; telp != NULL && telp->ret_type() == DB_ATTRIB; telp = SDS_CURDWG->entnext_noxref(telp))
    										;
    									if(telp == NULL)
    										goto out;
    									if(telp->ret_type() == DB_SEQEND)
    										((db_seqend *)telp)->get_mainent(&telp);
    									ename[0] = (long)telp;
    									ename[1] = (long)SDS_CURDWG;
										// DP: INSERT in the  BLOCK was loaded from file
										// TODO: change loading of BLOCKs so SEQEND pointer to INSERT object is valid
										if(telp == NULL)
											goto out;
    									if(RTNORM == sds_ssmemb(ename, ss))
    										goto out;
    									}
    								ToAdd = true;
    								}

    								out: ;
    							} // else
    						delete TmpHip;
    						}	// the end of the new addition

    					if(ToAdd)
    						{
    						savehip[i]=subenthip->newcopy();
    						ent[0]=(long)savehip[i];

    						/*D.G.*/// We shouldn't transform an entity twice: here and later in SDS_osnap_elist etc.
    						// Also look at the comments about m_ecs2ucs above.
							/*D.G.*/// Enable snapping to ents on locked layers: temporally onlock layer.
							int				layerFlags = 0;
							db_handitem*	pLayer = savehip[i]->ret_layerhip();

							if(pLayer)
								{
								pLayer->get_70(&layerFlags);
								pLayer->set_70(layerFlags & ~IC_LAYER_LOCKED);
								}

							SDS_xforment(ent,NULL,blkmtx);

							if(pLayer)
								pLayer->set_70(layerFlags);

							SDS_ssadd(ent, ss, ss);
    						if (i)
    							savehip[i-1]->next = savehip[i];
    						i++;
    						}

    					subenthip = subenthip->next;

 					/*D.G.*/// VERY important: it's a nested insert and it's not selected - with its attribs,
 					// so pass them (if any).
	 					if(!ToAdd && EntType == DB_INSERT)
	 						{
							/*DG - 11.3.2002*/// We shouldn't use this insert's hip in view->previnsdata
							// after this point (e.g. if we drag some attrib not belonging to this insert).
							if(view->previnsdata)
								view->previnsdata->hip = NULL;

	 						for( ; subenthip && subenthip->ret_type() != DB_SEQEND; subenthip = subenthip->next)
	 							;
	 						}

    					}	// while

    				SDS_IsFromOsnapToXref = false;

/*					// New implementation of block explosion
    				coll2.ClearList();
    				coll2.setDrawing( (db_drawing*)ent[1] );
    				coll2.AddItem( (db_drawing*)ent[1] , BlockToExplode, FALSE );
    				coll2.ExpandBlocks();
    				db_handitem *curItem = coll2.GetNext( NULL );
    				while( curItem )
    				{
    					ent[0] = (long)curItem;
    					SDS_ssadd( ent, ss, ss);
    					curItem = coll2.GetNext( curItem );
    				}
    				// EndOf New implementation of block explosion
*/
    				//restore the saved value to restart the callbacks if needed.
    				SDS_SetAUserCallback = Save_SDS_SetAUserCallback;

    				localCriticalSection.Leave();

    				if(bUndoOn)
					{
    					undoctl.resval.rint |= IC_UNDOCTL_ON;	// and now turn Undo back on
    					sds_setvar("UNDOCTL",&undoctl);
					}

    				//  to prevent from deadloop if there was no entity added while exploding block
    				/*D.G.*/// Don't look at the length of ss for it because
    				// we may have some ents other than subents of the insert in the ss!
//					sds_sslength(ss, &fl1);
    				if(i)	// we've added "i" subents of the insert into ss
    					{
    					bExplodeBlock=TRUE;
    					goto nested_ent;
    					}
    				}
    			}
    		}
    	else if (strsame(ic_rbassoc->resval.rstring,db_hitype2str(DB_POLYLINE)))
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
    		ic_encpy(etemp,ent);
    		while(sds_entnext_noxref(etemp,etemp)==RTNORM)
    			{
    			FreeResbufIfNotNull(&elist);
    			if ((elist=sds_entget(etemp))==NULL) {ret=RTERROR;goto exit_point;}
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
    					for (rbptemp=elist;rbptemp!=NULL;rbptemp=rbptemp->rbnext)
    						{
    						//walk the vertex and get the vertices to build this face
    						if (rbptemp->restype==71)
    							iEdge[0]=rbptemp->resval.rint;
    						else if (rbptemp->restype==72)
    							iEdge[1]=rbptemp->resval.rint;
    						else if (rbptemp->restype==73)
    							iEdge[2]=rbptemp->resval.rint;
    						else if (rbptemp->restype==74)
    							iEdge[3]=rbptemp->resval.rint;
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
    							ret=RTERROR;
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
    			ret=RTERROR;
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
            if ((ptlst= new struct pt_bulg [vtxcnt] )==NULL)
    			{
    			cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
    			ret=RTERROR;
    			goto exit_point;
    			}
			memset(ptlst,0,sizeof(struct pt_bulg));
    		ic_encpy(etemp,ent);
    		//populate array of points
    		for (fl1=0;fl1<vtxcnt;fl1++)
    			{
    			if ((ret=sds_entnext_noxref(etemp,etemp))!=RTNORM)
    				goto exit_point;
    			FreeResbufIfNotNull(&elist);
    			if ((elist=sds_entget(etemp))==NULL) {ret=RTERROR;goto exit_point;}
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
    		//before looping, allocate llist for call to pts checker
    		FreeResbufIfNotNull(&elist);
    		p7[0]=p7[1]=p7[2]=0.0;  // init this
    		if (closureflg & (IC_PLINE_3DPLINE | IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH))
    			elist=sds_buildlist(-1,ent,RTDXF0,"3ERTEX"/*DNT*/,10,p7,11,p7,42,fr1,0);
    		else
    			elist=sds_buildlist(-1,ent,RTDXF0,db_hitype2str(DB_VERTEX),10,p7,11,p7,42,fr1,39,fr2,210,p210,0);
    		if (elist==NULL)
    			{
    			cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
    			ret=RTERROR;
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
    					if (fl1==0)pface_cur=pface_beg;
    					else pface_cur=pface_cur->next;
    					if (pface_cur==NULL)break;
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
    							rb.restype=rb2.restype=RTSHORT;
    							rb.rbnext=rb2.rbnext=NULL;
    							rb.resval.rint=1;	//ucs
    							rb2.resval.rint=0;	//wcs
    							}
    						else if (fl1==0 && (closureflg & (IC_PLINE_3DPLINE | IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH)))
    							{
    							rb.restype=rb2.restype=RTSHORT;
    							rb.rbnext=rb2.rbnext=NULL;
    							rb.resval.rint=0;	//wcs
    							rb2.resval.rint=0;	//wcs
    							}
    						else if (fl1==0)
    							{
    							rb.rbnext=rb2.rbnext=NULL;
    							rb.restype=RTENAME;
    							ic_encpy(rb.resval.rlname,ent); //ecs
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
    						sds_trans(p5,&rb,&rb2,0,p5);
    						sds_trans(p6,&rb,&rb2,0,p6);//pts are now wcs
    						if (fi1==0 && fl1==0 && !icadRealEqual(fr2,0.0))
    							{
    							ic_usemat(p210,p210,m_ecs2ucs,1,0,0);
    							sds_trans(p210,&rb,&rb2,1,p210);
    							}
    						}
    					else
    						{
    						sds_trans(p5,&rb,&rb2,0,p5);
    						sds_trans(p6,&rb,&rb2,0,p6);//pts are now wcs
    						if (fi1==0 && fl1==0 && !icadRealEqual(fr2,0.0))
    							sds_trans(p210,&rb,&rb2,1,p210);
    						}
    					SDS_rpextents(p5,p6,ll,ur,NULL);
    					if (!icadRealEqual(fr2,0.0))
    						{
    						p9[0]=p210[0]*fr2;
    						p9[1]=p210[1]*fr2;
    						p9[2]=p210[2]*fr2;
    						//rb2 is always a wcs resbuf.  steal it & use as dcs
    						rbptemp=sds_buildlist(RTSHORT,2,0);
    						sds_trans(p9,&rb2,rbptemp,1,p9);
    						FreeResbufIfNotNull(&rbptemp);
    						p9[0]=fabs(p9[0]);p9[1]=fabs(p9[1]);
    						ll[0]-=p9[0];
    						ur[0]+=p9[0];
    						ll[1]-=p9[1];
    						ur[1]+=p9[1];
    						}
    					if ((ll[0]>p2[0])||	   //if segment out of bounds for
    					   (ll[1]>p2[1])||	   //orig ssget crossing box, continue
    					   (ur[0]<p1[0])||
    					   (ur[1]<p1[1])) continue;
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
    				SDS_osnap_elist(elist,osmode,pickptrp,(double *)ptPoint,ospt,&dsq,pl_omitflag,m_ecs2ucs);
                    // KLUGE alert!
                    // If the osnap is intersection and the number of entities in the selection set passed
                    // to this function is one. We have to do some special handling. What we will do is see
                    // if the pickpoint and its pickbox are over a vertex of the pline. If so we will return
                    // that point instead of what the call to SDS_osnap_elist() has returned. Bug 1-46576.
                    // This is kind of late in the shipping game, so this compromise was reached. We still do
                    // not handle self-intersecting plines. If we don't find a vertex that will work, then we
                    // return RTNONE and the user is re-prompted for a point.
                    if (osmode == IC_OSMODE_INT && SSLen == 1)	/*D.Gavrilov*/// We should compare with INT
                        {										// using == (not with &).
                        bool found = FALSE;
                        if (sds_distance(pickptrp,ospt) > maxDistFromPickBoxCen)
                            {
                            // Note 1 and vtxcnt-1, eliminates the start and end points
                            for (int i = 1; i < vtxcnt-1; i++)
                                {
                                if (sds_distance(pickptrp,ptlst[i].pt) < maxDistFromPickBoxCen)
                                    {
                                    ic_ptcpy(ospt, ptlst[fl1].pt);
                                    found = TRUE;
                                    ret=RTNORM;
                                    break;
                                    }
                                }
                            }
                        else
                            found = TRUE;

                        if (!found)
                        {	/*D.G.*/// Try to intersect polyline with itself.
    					sds_ssadd(ent, NULL, ss2);
    					rb.rbnext = rb2.rbnext = NULL;
    					rb.restype = rb2.restype = RTSHORT;
    					rb.resval.rint = 0;  rb2.resval.rint = 1;

						/*DG - 31.10.2001*/// Added interOption & bExtendAnyway parameters for ic_ssxss.
    					if(ic_ssxss(ss, ss2, IC_ZRO, &elist, 1, NULL, NULL, eExtendBoth, false) == 0)
    						{
    						for(rbptemp = elist; rbptemp != NULL; rbptemp = rbptemp->rbnext)
    							{
    							//transform point into ucs coords
    							sds_trans(rbptemp->resval.rpoint, &rb, &rb2, 0, rbptemp->resval.rpoint);
    							gra_ucs2rp(rbptemp->resval.rpoint, p5, view);
    							if(SDS_osnap_chkdsq(pickptrp, p5, ospt, &dsq))
//										{
    									ic_ptcpy(ospt, rbptemp->resval.rpoint);
//											g_nSnappedType= IC_OSMODE_INT;
//										}
//
    							}
    						ret = RTNORM;
    						break;
    						}
    					else
    						{
    						ret = RTNONE;
    						goto exit_point;
    						}

//                          ret=RTNONE;
//		                    goto exit_point;
                            }
                        }
    				}
    			}//for loop N direction

    		sds_ssname(ss,0L,ent);
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
    				rb.restype=RTSHORT;
    				rb.resval.rint=1;
    				rb2.restype=RTENAME;
    				ic_encpy(rb2.resval.rlname,ent);
    				sds_trans(p5,&rb2,&rb,0,p6);
    				}
    			gra_ucs2rp(p6,p7,view);
    			if (SDS_osnap_chkdsq(pickptrp,p7,ospt,&dsq))
//						{
    					ic_ptcpy(ospt,p6);
//							g_nSnappedType= IC_OSMODE_CENTER;
//						}
    			}
    		}
    	else if (strsame(ic_rbassoc->resval.rstring,db_hitype2str(DB_LWPOLYLINE)))
    		{
    		//Break the pline down into individual vertices & check
    		//the boundary of each.  If within crossing box, call
    		//simple osnap routine after adding 11 group to elist.
    		//NOTE: if it's not a 2D pline, change etype from VERTEX
    		//to 3ERTEX
            db_lwpolyline *lwpl=(db_lwpolyline *)ent[0];
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
    		ic_encpy(etemp,ent);
            // If only one pline and it has 2 vertices, it can't self intersect.
            if (fl1 == 1 && vtxcnt == 2)
                osmode &= ~IC_OSMODE_INT;

    		fr1=0.0;  // give it a value
// build a list to send to the existing points checker
    		FreeResbufIfNotNull(&elist);
    		elist=sds_buildlist(-1,ent,RTDXF0,db_hitype2str(DB_VERTEX),10,p7,11,p7,42,fr1,39,fr2,210,p210,0);
    		if (elist==NULL)
    			{
    			cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
    			ret=RTERROR;
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
    						rb.restype=rb2.restype=RTSHORT;
    						rb.rbnext=rb2.rbnext=NULL;
    						rb.resval.rint=1;	//ucs
    						rb2.resval.rint=0;	//wcs
    						}
    					else if (fl1==0)
    						{
    						rb.rbnext=rb2.rbnext=NULL;
    						rb.restype=RTENAME;
    						ic_encpy(rb.resval.rlname,ent); //ecs
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
    					sds_trans(p5,&rb,&rb2,0,p5);
    					sds_trans(p6,&rb,&rb2,0,p6);//pts are now wcs
    					if (fi1==0 && fl1==0 && !icadRealEqual(fr2,0.0))
    						{
    						ic_usemat(p210,p210,m_ecs2ucs,1,0,0);
    						sds_trans(p210,&rb,&rb2,1,p210);
    						}
    					}
    				else
    					{
    					sds_trans(p5,&rb,&rb2,0,p5);
    					sds_trans(p6,&rb,&rb2,0,p6);//pts are now wcs
    					if (fi1==0 && fl1==0 && !icadRealEqual(fr2,0.0))
    						sds_trans(p210,&rb,&rb2,1,p210);
    					}
    				SDS_rpextents(p5,p6,ll,ur,NULL);
    				if (!icadRealEqual(fr2,0.0))
    					{
    					p9[0]=p210[0]*fr2;
    					p9[1]=p210[1]*fr2;
    					p9[2]=p210[2]*fr2;
    					//rb2 is always a wcs resbuf.  steal it & use as dcs
    					rbptemp=sds_buildlist(RTSHORT,2,0);
    					sds_trans(p9,&rb2,rbptemp,1,p9);
    					FreeResbufIfNotNull(&rbptemp);
    					p9[0]=fabs(p9[0]);p9[1]=fabs(p9[1]);
    					ll[0]-=p9[0];
    					ur[0]+=p9[0];
    					ll[1]-=p9[1];
    					ur[1]+=p9[1];
    					}
    				if ((ll[0]>p2[0])||	   //if segment out of bounds for
    				   (ll[1]>p2[1])||	   //orig ssget crossing box, continue
    				   (ur[0]<p1[0])||
    				   (ur[1]<p1[1])) continue;
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
      			SDS_osnap_elist(elist,osmode,pickptrp,(double *)ptAperCtr,ospt,&dsq,pl_omitflag,m_ecs2ucs); // Reason: Fix for bug no.47837
                // KLUGE alert!
                // If the osnap is intersection and the number of entities in the selection set passed
                // to this function is one. We have to do some special handling. What we will do is see
                // if the pickpoint and its pickbox are over a vertex of the pline. If so we will return
                // that point instead of what the call to SDS_osnap_elist() has returned. Bug 1-46576.
                // This is kind of late in the shipping game, so this compromise was reached. We still do
                // not handle self-intersecting plines. If we don't find a vertex that will work, then we
                // return RTNONE and the user is re-prompted for a point.

//					//Modified Cybage SBD 12/05/2001 DD/MM/YYYY
//					//Reason : Implementation of Flyover Snapping
//					//This fix not enabled when fly over snapping is ON
//					if(!iFlyOverSnap)
//					{
    				if((osmode & (IC_OSMODE_INT | IC_OSMODE_PLAN)) &&
    					SSLen == 1)										/*D.G.*/// Use SSLen instead of fl1.
    					{
    					bool found = FALSE;
    					sds_point *points = new sds_point[vtxcnt];
    					lwpl->get_10(points, vtxcnt);

    					if (sds_distance(pickptrp,ospt) > maxDistFromPickBoxCen)
    						{
    						// Note 1 and vtxcnt-1, eliminates the start and end points
    						for (int i = 1; i < vtxcnt-1; i++)
    							{
    							if (sds_distance(pickptrp,points[i]) < maxDistFromPickBoxCen)
    								{
    								ic_ptcpy(ospt, points[i]);
    								found = TRUE;
    								ret=RTNORM;
    								break;
    								}
    							}
    						}
    					else
    						found = TRUE;

					IC_FREE(points);	/*D.G.*/

    					if (!found)
    						{	/*D.G.*/// Try to intersect polyline with itself.
    						sds_ssadd(ent, NULL, ss2);
    						rb.rbnext = rb2.rbnext = NULL;
    						rb.restype = rb2.restype = RTSHORT;
    						rb.resval.rint = 0;  rb2.resval.rint = 1;

							/*DG - 31.10.2001*/// Added interOption & bExtendAnyway parameters for ic_ssxss.
    						if(ic_ssxss(ss, ss2, IC_ZRO, &elist, 1, NULL, NULL, eExtendBoth, false) == 0)
    							{
    							for(rbptemp = elist; rbptemp != NULL; rbptemp = rbptemp->rbnext)
    								{
    								//transform point into ucs coords
    								sds_trans(rbptemp->resval.rpoint, &rb, &rb2, 0, rbptemp->resval.rpoint);
    								gra_ucs2rp(rbptemp->resval.rpoint, p5, view);
    								if(SDS_osnap_chkdsq(pickptrp, p5, ospt, &dsq))
//											{
    										ic_ptcpy(ospt, rbptemp->resval.rpoint);
//												g_nSnappedType= IC_OSMODE_INT;
//											}
    								}
    							ret = RTNORM;
    							break;
    							}
    						else
    							{
    							ret = RTNONE;
    							goto exit_point;
    							}

    //						ret=RTNONE;
    //						goto exit_point;
    						}
    					}
//						}//end of flyover check
//						//Modified Cybage SBD 12/05/2001 DD/MM/YYYY
//
    			}

    		sds_ssname(ss,0L,ent);
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
    				rb.restype=RTSHORT;
    				rb.resval.rint=1;
    				rb2.restype=RTENAME;
    				ic_encpy(rb2.resval.rlname,ent);
    				sds_trans(p5,&rb2,&rb,0,p6);
    				}
    			gra_ucs2rp(p6,p7,view);
    			if (SDS_osnap_chkdsq(pickptrp,p7,ospt,&dsq))
//						{
    					ic_ptcpy(ospt,p6);
//							g_nSnappedType= IC_OSMODE_CENTER;
//						}
    			}
    		}
    	else	//simple entity
    		SDS_osnap_elist(elist,osmode,pickptrp,(double *)ptAperCtr,ospt,&dsq,pl_omitflag,m_ecs2ucs);
    	if ((osmode & IC_OSMODE_QUICK) && (dsq!=-1.0))
    		break;
    	//delete ename from selection set
    	//check for INT snaps involving more than one entity
    	if (NULL==m_ecs2ucs)
    		sds_ssdel(ent,ss);
    	//else
    	//	continue;	//TODO use code below to do intersections within block ents
    	sds_sslength(ss,&fl1);
    	if (fl1 > 0L)
    		{
    		if (osmode & (IC_OSMODE_PLAN | IC_OSMODE_APP | IC_OSMODE_INT))//PlanViewInt,Apparent,3DINT
    			{
    			//NOTE: no special code here yet for apparent ints, see comment below
    			if (osmode & IC_OSMODE_INT)	//let fi1 represent if we're doing a 3D int
    				fi1=1;
    			else
    				fi1=0;
    			FreeResbufIfNotNull(&elist);
    			if (bExplodeBlock)
    				{
    				//initialize ss2 with all entities that are within the aperture - one of these is the block.
    				ss2[1]=0L;
    				if ((ret=sds_pmtssget(NULL,"CPOLYGON"/*DNT*/,polyelist,NULL,NULL,ss2,0,true,true))!=RTNORM)
    					goto exit_point;
    				//Now explode the block, and add all its sub entities to the ss2 selset.
    				coll.ClearList();
    				coll.AddItem((db_drawing *)ent[1], BlockToExplode, FALSE);
    				coll.ExpandBlocks((db_drawing *)ent[1]);
    				db_handitem *currentItem = coll.GetNext(NULL);
    				while (currentItem)
    					{
    					etemp[0] = (long)currentItem;
    					etemp[1] = ent[1];
						SDS_ssadd(etemp, ss2, ss2);
    					currentItem = coll.GetNext(currentItem);
    					}
    				}
    			else
    				sds_ssadd(ent,NULL,ss2);
    			rb.rbnext=rb2.rbnext=NULL;
    			rb.restype=rb2.restype=RTSHORT;
    			rb.resval.rint=0;rb2.resval.rint=1;
    			//make sure to use ssxss w/a fuzz>0, espcially for 3D stuff!
    			sds_sslength(ss,&fl1);
    			sds_sslength(ss2,&fl2);
    			if (fl1+fl2 > 40)	//this is an arbitrary limit - tweak it if necessary
    				{
//						//Modified Cybage SBD 12/05/2001 DD/MM/YYYY
//						//Reason : Implementation of Flyover Snapping
//						if(!iFlyOverSnap) //print only when flyover snapping is OFF
//							{
    					sds_printf (ResourceString(IDC_SDS_OSNAP_ERROR2, "\nToo many entities selected.  No valid intersection was found."));
//							}
    				sds_ssfree(ss2);
    				ret=RTERROR;
    				goto exit_point;
    				}

				/*DG - 31.10.2001*/// Added interOption & bExtendAnyway parameters for ic_ssxss.
    			if (ic_ssxss(ss,ss2,IC_ZRO,&elist,fi1,NULL,NULL, eExtendBoth, false)==0)
    				{
    				for (rbptemp=elist;rbptemp!=NULL;rbptemp=rbptemp->rbnext)
    					{
    					//transform point into ucs coords
    					sds_trans(rbptemp->resval.rpoint,&rb,&rb2,0,rbptemp->resval.rpoint);
    					gra_ucs2rp(rbptemp->resval.rpoint,p5,view);
    					if (SDS_osnap_chkdsq(pickptrp,p5,ospt,&dsq))
//								{
    							ic_ptcpy(ospt,rbptemp->resval.rpoint);
//									g_nSnappedType= IC_OSMODE_INT; //intializes here for plan intersection
//								}
    					}
    				}
    			}
    		}
    	sds_ssfree(ss2);
    	}

    exit_point:
    FreeResbufIfNotNull(&elist);
    FreeResbufIfNotNull(&polyelist);
    if (pface_beg!=NULL)
    	{
    	for (pface_tmp=pface_beg;pface_tmp!=NULL;)
    		{
    		pface_beg=pface_beg->next;
    		pface_tmp->next=NULL;
    		delete pface_tmp;
    		pface_tmp=pface_beg;
    		}
    	}
    sds_ssfree(ss);
    if (savehip)
    	{
    	int i=0;
    	db_handitem *tmphip = savehip[i];
    	while (tmphip)
    		{
    		tmphip = savehip[i]->next;
    		delete savehip[i];
    		i++;
    		}

    	delete [] savehip;
    	savehip=NULL;
    	}
    if (sl)
    	gr_freeselector(sl);

    if (ret==RTNORM && dsq!=-1.0)
    	ic_ptcpy(ptPoint,ospt);
    else if (ret==RTNORM && dsq==-1.0 && SDS_OSNAP_TANPER_IGNORE>0)
    	//no remaining snaps found and some were ignored
    	return(ret);
    else
    	{
        // If the DB_POLYLINE or DB_LWPOLYLINE case set this to RTNONE, let it go
        // on through as is.
        if(ret != RTNONE)
    		ret=RTERROR;
    	SDS_OSNAP_TANPER_IGNORE=0;
    	}
    if (ptlst!=NULL)
    	IC_FREE(ptlst);
    //if we don't find an osnap, don't update the result
    return(ret);
}

int SDS_osnap_elist(resbuf *elist, int osmode, sds_point pickptrp, sds_point pickptucs, sds_point ospt, sds_real *dsq,int pl_omitflag,sds_matrix *m_ecs2ucs)
{
    //NOTE: pickpt2d is a 2D point in RP coordinates!
    //NOTE: 2d and 3d pline vertices have an 11 group for endpoint, and 3d vertices passed
    //			with 0 group changed to 3ERTEX
    //NOTE: for some entities (SOLID, POLYLINE, TRACE, 3DFACE), INT is checked for
    //			intersections within entity itself. If pl_omitflag==1, don't check 10-pt
    //			on vtx for INT, if pl_omitflag==2, don't check 11pt
    //for ents w/thickness, MID and NEA are valid for "edges" which connect
    //	ends of entity base and extruded level

    sds_point osptrp,lastpt,p1,p2,p3,p4,p5,p6,p7,p11={0.0,0.0,0.0},p12={0.0,0.0,0.0},p13,p14,p15,p16={0.0,0.0,0.0},p17,p210;
    sds_real fr1,fr2,fr3,fr4,fr5,thickness;
    resbuf rb, *rbtemp, rbwcs, rbucs, rbent;
    int ret=RTNORM,fi1,fi2,thickflg;
    char etype[IC_ACADNMLEN];

	gra_view *view=SDS_CURGRVW;
    if (view==NULL) return(RTERROR);
    rbucs.restype=rbwcs.restype=RTSHORT;
    rbent.restype=RTENAME;
    rbucs.rbnext=rbwcs.rbnext=rbent.rbnext=NULL;
    rbucs.resval.rint=1;rbwcs.resval.rint=0;
    //NOTES ON VERTEX ENTS -
    	//for a VERTEX, the entity is "doctored" so it's name is 3ERTEX if
    	//the vtx is from a 3D poly or mesh.  Its 10 group is the center of
    	//the arc if bulge !=0.0.  if bulge==0.0, 10 group not altered
    	//It also has an 11 group to designate the endpoint of the
    	//straight segment or the rr, sa, &ea of the arc
    	//Essentially, it's in ic_segdist format!


    gra_ucs2rp(ospt,osptrp,view);

    if (SDS_getvar(NULL,DB_QLASTPOINT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) {ret=RTERROR;goto exit_point;}
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
    	gra_ucs2rp(p1,p3,view);
    	gra_ucs2rp(p2,p4,view);

    	if (osmode &(IC_OSMODE_END | IC_OSMODE_INT | IC_OSMODE_PLAN | IC_OSMODE_APP))
    		{
    		if (SDS_osnap_chkdsq(pickptrp,p3,osptrp,dsq))
    			{
    			ic_ptcpy(ospt,p1);
    			ic_ptcpy(osptrp,p3);
//					if (osmode & IC_OSMODE_END )
//						g_nSnappedType= IC_OSMODE_END;//according to snap priority
//					else
//						g_nSnappedType= IC_OSMODE_INT;
//
    			}
    		if (SDS_osnap_chkdsq(pickptrp,p4,osptrp,dsq))
    			{
    			ic_ptcpy(ospt,p2);
    			ic_ptcpy(osptrp,p4);
//					if (osmode & IC_OSMODE_END )
//						g_nSnappedType= IC_OSMODE_END;//according to snap priority
//					else
//						g_nSnappedType= IC_OSMODE_INT;
    			}
    		}
    	if (osmode & IC_OSMODE_NEAR)//NEA
    		{
    		//do calcs in rp to get nearest point on screen...
    		ic_segdist(pickptrp,p3,p4,0,&fr1,p5);
    		if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
    			{
    			ic_ptcpy(osptrp,p5);
    			fr1=sds_distance(p3,p5)/sds_distance(p3,p4);
    			ospt[0]=p1[0]+fr1*(p2[0]-p1[0]);
    			ospt[1]=p1[1]+fr1*(p2[1]-p1[1]);
    			ospt[2]=p1[2]+fr1*(p2[2]-p1[2]);
//					g_nSnappedType= IC_OSMODE_NEAR;
    			}
    		}
    	if (osmode & IC_OSMODE_PERP)//PER
    		{
    		sds_trans(lastpt,&rbucs,&rbwcs,0,p7);
    		if (-1!=ic_ptlndist(p7,p1,p2,&fr3,p6))
    			{
    			fr3=sds_dist2d(p1,p6)/sds_dist2d(p1,p2);
    			p6[2]=p1[2]+fr3*(p2[2]-p1[2]);
    			gra_ucs2rp(p6,p5,view);
    			if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
    				{
    				ic_ptcpy(ospt,p6);
    				ic_ptcpy(osptrp,p5);
//						g_nSnappedType= IC_OSMODE_PERP;
    				}
    			}
    		}
    	if (osmode & IC_OSMODE_MID)//MID
    		{
    		p5[0]=(p1[0]+p2[0])/2.0;
    		p5[1]=(p1[1]+p2[1])/2.0;
    		p5[2]=(p1[2]+p2[2])/2.0;
    		gra_ucs2rp(p5,p6,view);
    		if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
    			{
    			ic_ptcpy(ospt,p5);
    			ic_ptcpy(osptrp,p6);
//					g_nSnappedType= IC_OSMODE_MID;
    			}
    		}
    	}
    else if (strsame(etype,db_hitype2str(DB_VERTEX)))  //2D poly in ecs coordinates!
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
    			gra_ucs2rp(p5,p6,view);
    			if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
    				{
    				ic_ptcpy(ospt,p5);
    				ic_ptcpy(osptrp,p6);
//						if (osmode & IC_OSMODE_END )
//							g_nSnappedType= IC_OSMODE_END;//according to snap priority
//						else
//							g_nSnappedType= IC_OSMODE_INT;
//						if(osmode & IC_OSMODE_PLAN)//for plan /default INT view
//							{
//							if(osmode & IC_OSMODE_END )
//								g_nSnappedType= IC_OSMODE_END ;
//							else
//								g_nSnappedType= 0 ;
//							}
//
    				}
    			if (thickflg)
    				{
    				if (icadRealEqual(fr1,0.0))ic_ptcpy(p13,p11);
    				if (m_ecs2ucs)
    					ic_usemat(p13,p15,m_ecs2ucs,0,0,0);
    				else
    					sds_trans(p13,&rbent,&rbucs,0,p15);
    				gra_ucs2rp(p15,p16,view);
    				if (SDS_osnap_chkdsq(pickptrp,p16,osptrp,dsq))
    					{
    					ic_ptcpy(ospt,p15);
    					ic_ptcpy(osptrp,p16);
//							if (osmode & IC_OSMODE_END )
//								g_nSnappedType= IC_OSMODE_END;//according to snap priority
//							else
//								g_nSnappedType= IC_OSMODE_INT;
//							if(osmode & IC_OSMODE_PLAN)//for plan /default INT view
//								{
//								if(osmode & IC_OSMODE_END )
//									g_nSnappedType= IC_OSMODE_END ;
//								else
//									g_nSnappedType= 0 ;
//								}
    					}
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
    			gra_ucs2rp(p5,p6,view);
    			if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
    				{
    				ic_ptcpy(ospt,p5);
    				ic_ptcpy(osptrp,p6);
//						if (osmode & IC_OSMODE_END )
//							g_nSnappedType= IC_OSMODE_END;//according to snap priority
//						else
//							g_nSnappedType= IC_OSMODE_INT;
//						if(osmode & IC_OSMODE_PLAN)//for plan /default INT view
//							{
//							if(osmode & IC_OSMODE_END )
//								g_nSnappedType= IC_OSMODE_END ;
//							else
//								g_nSnappedType= 0 ;
//							}
    				}
    			if (thickflg)
    				{
    				if (icadRealEqual(fr1,0.0))
    					ic_ptcpy(p14,p12);
    				if (m_ecs2ucs)
    					ic_usemat(p14,p15,m_ecs2ucs,0,0,1);
    				else
    					sds_trans(p13,&rbent,&rbucs,0,p15);
    				gra_ucs2rp(p15,p16,view);
    				if (SDS_osnap_chkdsq(pickptrp,p16,osptrp,dsq))
    					{
    					ic_ptcpy(ospt,p15);
    					ic_ptcpy(osptrp,p16);
//							if (osmode & IC_OSMODE_END )
//								g_nSnappedType= IC_OSMODE_END;//according to snap priority
//							else
//								g_nSnappedType= IC_OSMODE_INT;
//							if(osmode & IC_OSMODE_PLAN)//for plan /default INT view
//								{
//								if(osmode & IC_OSMODE_END )
//									g_nSnappedType= IC_OSMODE_END ;
//								else
//									g_nSnappedType= 0 ;
//								}
    					}
    				}
    			}
    		}
    	if (osmode & IC_OSMODE_NEAR)//NEA
    		{
    		if (icadRealEqual(fr1,0.0))
    			{
    			sds_trans(p1,&rbent,&rbucs,0,p5);
    			sds_trans(p2,&rbent,&rbucs,0,p6);
    			gra_ucs2rp(p5,p5,view);
    			gra_ucs2rp(p6,p6,view);
    			ic_segdist(pickptrp,p5,p6,0,&fr4,p7);
    			if (SDS_osnap_chkdsq(pickptrp,p7,osptrp,dsq))
    				{
    				ic_ptcpy(osptrp,p7);
    				fr5=sds_dist2d(p5,p7)/sds_dist2d(p5,p6);
    				p7[0]=p1[0]+(fr5*(p2[0]-p1[0]));
    				p7[1]=p1[1]+(fr5*(p2[1]-p1[1]));
    				p7[2]=p1[2]+(fr5*(p2[2]-p1[2]));
    				sds_trans(p7,&rbent,&rbucs,0,ospt);
//						g_nSnappedType= IC_OSMODE_NEAR;
    				}
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
    			gra_ucs2rp(p6,p5,view);
    			if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
    				{
    				ic_ptcpy(ospt,p6);
    				ic_ptcpy(osptrp,p5);
//						g_nSnappedType= IC_OSMODE_NEAR;
    				}
    			}

    		if (thickflg)
    			{
    			if (icadRealEqual(fr1,0.0))
    				{
    				gra_ucs2rp(p11,p5,view);
    				gra_ucs2rp(p12,p6,view);
    				ic_segdist(pickptrp,p5,p6,0,&fr4,p7);
    				if (SDS_osnap_chkdsq(pickptrp,p7,osptrp,dsq))
    					{
    					ic_ptcpy(osptrp,p7);
    					fr5=sds_distance(p5,p7)/sds_distance(p5,p6);
    					ospt[0]=p11[0]+(fr5*(p12[0]-p11[0]));
    					ospt[1]=p11[1]+(fr5*(p12[1]-p11[1]));
    					ospt[2]=p11[2]+(fr5*(p12[2]-p11[2]));
//							g_nSnappedType= IC_OSMODE_NEAR;
    					}
    				//test along edges
    				gra_ucs2rp(p11,p5,view);
    				gra_ucs2rp(p1,p6,view);
    				ic_segdist(pickptrp,p5,p6,0,&fr4,p7);
    				if (SDS_osnap_chkdsq(pickptrp,p7,osptrp,dsq))
    					{
    					ic_ptcpy(osptrp,p7);
    					fr5=sds_distance(p5,p7)/sds_distance(p5,p6);
    					ospt[0]=p11[0]+(fr5*(p1[0]-p11[0]));
    					ospt[1]=p11[1]+(fr5*(p1[1]-p11[1]));
    					ospt[2]=p11[2]+(fr5*(p1[2]-p11[2]));
//							g_nSnappedType= IC_OSMODE_NEAR;
    					}
    				gra_ucs2rp(p12,p5,view);
    				gra_ucs2rp(p2,p6,view);
    				ic_segdist(pickptrp,p5,p6,0,&fr4,p7);
    				if (SDS_osnap_chkdsq(pickptrp,p7,osptrp,dsq))
    					{
    					ic_ptcpy(osptrp,p7);
    					fr5=sds_distance(p5,p7)/sds_distance(p5,p6);
    					ospt[0]=p12[0]+(fr5*(p2[0]-p12[0]));
    					ospt[1]=p12[1]+(fr5*(p2[1]-p12[1]));
    					ospt[2]=p12[2]+(fr5*(p2[2]-p12[2]));
//							g_nSnappedType= IC_OSMODE_NEAR;
    					}
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
    				gra_ucs2rp(p6,p5,view);
    				if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
    					{
    					ic_ptcpy(ospt,p6);
    					ic_ptcpy(osptrp,p5);
//							g_nSnappedType= IC_OSMODE_NEAR;
    					}
    				//test along edges
    				ic_segdist(p17,p3,p13,0,&fr4,p5);
    				if (m_ecs2ucs)
    					ic_usemat(p5,p6,m_ecs2ucs,0,0,0);
    				else
    					sds_trans(p5,&rbent,&rbucs,0,p6);
    				gra_ucs2rp(p6,p5,view);
    				if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
    					{
    					ic_ptcpy(ospt,p6);
    					ic_ptcpy(osptrp,p5);
//							g_nSnappedType= IC_OSMODE_NEAR;
    					}
    				ic_segdist(p17,p4,p14,0,&fr4,p5);
    				if (m_ecs2ucs)
    					ic_usemat(p5,p6,m_ecs2ucs,0,0,0);
    				else
    					sds_trans(p5,&rbent,&rbucs,0,p6);
    				gra_ucs2rp(p6,p5,view);
    				if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
    					{
    					ic_ptcpy(ospt,p6);
    					ic_ptcpy(osptrp,p5);
//							g_nSnappedType= IC_OSMODE_NEAR;
    					}
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
    				gra_ucs2rp(p6,p7,view);
    				if (SDS_osnap_chkdsq(pickptrp,p7,osptrp,dsq))
    					{
    					ic_ptcpy(ospt,p6);
    					ic_ptcpy(osptrp,p7);
//							g_nSnappedType= IC_OSMODE_PERP;
    					}
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

    				gra_ucs2rp(p6,p7,view);
    				if (SDS_osnap_chkdsq(pickptrp,p7,osptrp,dsq))
    					{
    					ic_ptcpy(ospt,p6);
    					ic_ptcpy(osptrp,p7);
//							g_nSnappedType= IC_OSMODE_PERP;
    					}
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
    					gra_ucs2rp(p6,p7,view);
    					if (SDS_osnap_chkdsq(pickptrp,p7,osptrp,dsq))
    						{
    						ic_ptcpy(ospt,p6);
    						ic_ptcpy(osptrp,p7);
//								g_nSnappedType= IC_OSMODE_PERP;
    						}
    				}
    			else
    				{
    				if (-1!=ic_ptlndist(p5,p11,p12,&fr3,p6))
    					{
    					if (m_ecs2ucs)
    						ic_usemat(p6,p6,m_ecs2ucs,0,0,0);
    					else
    						sds_trans(p6,&rbent,&rbucs,0,p6);
    					gra_ucs2rp(p6,p7,view);
    					if (SDS_osnap_chkdsq(pickptrp,p7,osptrp,dsq))
    						{
    						ic_ptcpy(ospt,p6);
    						ic_ptcpy(osptrp,p7);
//								g_nSnappedType= IC_OSMODE_PERP;
    						}
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
    					gra_ucs2rp(p7,p5,view);
    					if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
    						{
    						ic_ptcpy(ospt,p7);
    						ic_ptcpy(osptrp,p5);
//								g_nSnappedType= IC_OSMODE_QUADRANT;
    						}
    					if (thickflg)
    						{
    						p6[0]+=thickness*p210[0];
    						p6[1]+=thickness*p210[1];
    						p6[2]+=thickness*p210[2];
    						if (m_ecs2ucs)
    							ic_usemat(p6,p7,m_ecs2ucs,0,0,0);
    						else
    							sds_trans(p6,&rbent,&rbucs,0,p7);
    						gra_ucs2rp(p7,p5,view);
    						if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
    							{
    							ic_ptcpy(ospt,p7);
    							ic_ptcpy(osptrp,p5);
//									g_nSnappedType= IC_OSMODE_QUADRANT;
    							}
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
    			gra_ucs2rp(p5,p6,view);
    			if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
    				{
    				ic_ptcpy(ospt,p5);
    				ic_ptcpy(osptrp,p6);
//						g_nSnappedType= IC_OSMODE_CENTER;
    				}
    			if (thickflg)
    				{
                    sds_trans(p11,&rbent,&rbucs,0,p15);
    				gra_ucs2rp(p15,p16,view);
    				if (SDS_osnap_chkdsq(pickptrp,p16,osptrp,dsq))
    					{
    					ic_ptcpy(ospt,p15);
    					ic_ptcpy(osptrp,p16);
//							g_nSnappedType= IC_OSMODE_CENTER;
    					}
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
    		gra_ucs2rp(p6,p7,view);
    		if (SDS_osnap_chkdsq(pickptrp,p7,osptrp,dsq))
    			{
    			ic_ptcpy(ospt,p6);
    			ic_ptcpy(osptrp,p7);
//					g_nSnappedType= IC_OSMODE_MID;
    			}
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
    			gra_ucs2rp(p16,p17,view);
    			if (SDS_osnap_chkdsq(pickptrp,p17,osptrp,dsq))
    				{
    				ic_ptcpy(ospt,p16);
    				ic_ptcpy(osptrp,p17);
//						g_nSnappedType= IC_OSMODE_MID;
    				}
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
    			gra_ucs2rp(p16,p17,view);
    			if (SDS_osnap_chkdsq(pickptrp,p17,osptrp,dsq))
    				{
    				ic_ptcpy(ospt,p16);
    				ic_ptcpy(osptrp,p17);
//						g_nSnappedType= IC_OSMODE_MID;
    				}
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
    			gra_ucs2rp(p16,p17,view);
    			if (SDS_osnap_chkdsq(pickptrp,p17,osptrp,dsq))
    				{
    				ic_ptcpy(ospt,p16);
    				ic_ptcpy(osptrp,p17);
//						g_nSnappedType= IC_OSMODE_MID;
    				}
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
    				gra_ucs2rp(p6,p7,view);
    				if (SDS_osnap_chkdsq(pickptrp,p7,osptrp,dsq))
    					{
    					ic_ptcpy(ospt,p6);
    					ic_ptcpy(osptrp,p7);
//							g_nSnappedType = IC_OSMODE_TAN;
    					}
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
    				gra_ucs2rp(p5,p6,view);
    				if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
    					{
    					ic_ptcpy(ospt,p5);
    					ic_ptcpy(osptrp,p6);
//							g_nSnappedType= IC_OSMODE_TAN;
    					}
    				}
    			if (fi1&4)
    				{
    				if (m_ecs2ucs)
    					ic_usemat(p7,p5,m_ecs2ucs,0,0,0);
    				else
    					sds_trans(p7,&rbent,&rbucs,0,p5);
    				gra_ucs2rp(p5,p6,view);
    				if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
    					{
    					ic_ptcpy(ospt,p6);
    					ic_ptcpy(osptrp,p6);
//							g_nSnappedType= IC_OSMODE_TAN;
    					}
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
    					gra_ucs2rp(p16,p17,view);
    					if (SDS_osnap_chkdsq(pickptrp,p17,osptrp,dsq))
    						{
    						ic_ptcpy(ospt,p16);
    						ic_ptcpy(osptrp,p17);
//								g_nSnappedType= IC_OSMODE_TAN;
    						}
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
    					gra_ucs2rp(p15,p16,view);
    					if (SDS_osnap_chkdsq(pickptrp,p16,osptrp,dsq))
    						{
    						ic_ptcpy(ospt,p15);
    						ic_ptcpy(osptrp,p16);
//								g_nSnappedType= IC_OSMODE_TAN;
    						}
    					}
    				if (fi1&4)
    					{
    					if (m_ecs2ucs)
    						ic_usemat(p17,p15,m_ecs2ucs,0,0,0);
    					else
    						sds_trans(p17,&rbent,&rbucs,0,p15);
    					gra_ucs2rp(p15,p16,view);
    					if (SDS_osnap_chkdsq(pickptrp,p16,osptrp,dsq))
    						{
    						ic_ptcpy(ospt,p16);
    						ic_ptcpy(osptrp,p16);
//								g_nSnappedType= IC_OSMODE_TAN;
    						}
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
    		gra_ucs2rp(p7,p6,view);
    		if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
    			{
    			ic_ptcpy(ospt,p7);
    			ic_ptcpy(osptrp,p6);
//					g_nSnappedType= IC_OSMODE_CENTER;
    			}
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
    		gra_ucs2rp(p1,p6,view);
    		if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
    			{
    			ic_ptcpy(ospt,p1);
    			ic_ptcpy(osptrp,p6);
//					if (osmode & IC_OSMODE_END )
//						g_nSnappedType= IC_OSMODE_END;//according to snap priority
//					else
//						g_nSnappedType= IC_OSMODE_INT;
    			}
    		gra_ucs2rp(p2,p6,view);
    		if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
    			{
    			ic_ptcpy(ospt,p2);
    			ic_ptcpy(osptrp,p6);
//					if (osmode & IC_OSMODE_END )
//						g_nSnappedType= IC_OSMODE_END;//according to snap priority
//					else
//						g_nSnappedType= IC_OSMODE_INT;
    			}
    		gra_ucs2rp(p3,p6,view);
    		if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
    			{
    			ic_ptcpy(ospt,p3);
    			ic_ptcpy(osptrp,p6);
//					if (osmode & IC_OSMODE_END )
//						g_nSnappedType= IC_OSMODE_END;//according to snap priority
//					else
//						g_nSnappedType= IC_OSMODE_INT;
    			}
    		gra_ucs2rp(p4,p6,view);
    		if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
    			{
    			ic_ptcpy(ospt,p4);
    			ic_ptcpy(osptrp,p6);
//					if (osmode & IC_OSMODE_END )
//						g_nSnappedType= IC_OSMODE_END;//according to snap priority
//					else
//						g_nSnappedType= IC_OSMODE_INT;
    			}
    		}
    	if (osmode & IC_OSMODE_NEAR)//NEA
    		{
    		gra_ucs2rp(p1,p5,view);
    		gra_ucs2rp(p2,p6,view);
    		ic_segdist(pickptrp,p5,p6,0,&fr1,p7);
    		if (SDS_osnap_chkdsq(pickptrp,p7,osptrp,dsq))
    			{
    			ic_ptcpy(osptrp,p7);
    			fr1=sds_distance(p5,p7)/sds_distance(p5,p6);
    			ospt[0]=p1[0]+(fr1*(p2[0]-p1[0]));
    			ospt[1]=p1[1]+(fr1*(p2[1]-p1[1]));
    			ospt[2]=p1[2]+(fr1*(p2[2]-p1[2]));
//					g_nSnappedType= IC_OSMODE_NEAR;
    			}
    		gra_ucs2rp(p3,p5,view);
    		ic_segdist(pickptrp,p5,p6,0,&fr1,p7);
    		if (SDS_osnap_chkdsq(pickptrp,p7,osptrp,dsq))
    			{
    			ic_ptcpy(osptrp,p7);
    			fr1=sds_distance(p5,p7)/sds_distance(p5,p6);
    			ospt[0]=p3[0]+(fr1*(p2[0]-p3[0]));
    			ospt[1]=p3[1]+(fr1*(p2[1]-p3[1]));
    			ospt[2]=p3[2]+(fr1*(p2[2]-p3[2]));
//					g_nSnappedType= IC_OSMODE_NEAR;
    			}
    		gra_ucs2rp(p4,p6,view);
    		ic_segdist(pickptrp,p5,p6,0,&fr1,p7);
    		if (SDS_osnap_chkdsq(pickptrp,p7,osptrp,dsq))
    			{
    			ic_ptcpy(osptrp,p7);
    			fr1=sds_distance(p5,p7)/sds_distance(p5,p6);
    			ospt[0]=p3[0]+(fr1*(p4[0]-p3[0]));
    			ospt[1]=p3[1]+(fr1*(p4[1]-p3[1]));
    			ospt[2]=p3[2]+(fr1*(p4[2]-p3[2]));
//					g_nSnappedType= IC_OSMODE_NEAR;
    			}
    		gra_ucs2rp(p1,p5,view);
    		ic_segdist(pickptrp,p5,p6,0,&fr1,p7);
    		if (SDS_osnap_chkdsq(pickptrp,p7,osptrp,dsq))
    			{
    			ic_ptcpy(osptrp,p7);
    			fr1=sds_distance(p5,p7)/sds_distance(p5,p6);
    			ospt[0]=p1[0]+(fr1*(p4[0]-p1[0]));
    			ospt[1]=p1[1]+(fr1*(p4[1]-p1[1]));
    			ospt[2]=p1[2]+(fr1*(p4[2]-p1[2]));
//					g_nSnappedType= IC_OSMODE_NEAR;
    			}
    		}
    	if (osmode & IC_OSMODE_PERP)//PER
    		{
    		//NOTE: can't do calc in RP, because per is independent of view
    		//NOTE: all pts should be in UCS, including lastpt
    		//lastpt already in ucs
    		if (-1!=ic_ptlndist(lastpt,p1,p2,&fr1,p7))
    			{
    			gra_ucs2rp(p7,p6,view);
    			if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
    				{
    				ic_ptcpy(ospt,p7);
    				ic_ptcpy(osptrp,p6);
//						g_nSnappedType= IC_OSMODE_PERP;
    				}
    			}
    		if (-1!=ic_ptlndist(lastpt,p2,p3,&fr1,p7))
    			{
    			gra_ucs2rp(p7,p6,view);
    			if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
    				{
    				ic_ptcpy(ospt,p7);
    				ic_ptcpy(osptrp,p6);
//						g_nSnappedType= IC_OSMODE_PERP;
    				}
    			}
    		if (-1!=ic_ptlndist(lastpt,p3,p4,&fr1,p7))
    			{
    			gra_ucs2rp(p7,p6,view);
    			if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
    				{
    				ic_ptcpy(ospt,p7);
    				ic_ptcpy(osptrp,p6);
//						g_nSnappedType= IC_OSMODE_PERP;
    				}
    			}
    		if (-1!=ic_ptlndist(lastpt,p4,p1,&fr1,p7))
    			{
    			gra_ucs2rp(p7,p6,view);
    			if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
    				{
    				ic_ptcpy(ospt,p7);
    				ic_ptcpy(osptrp,p6);
//						g_nSnappedType= IC_OSMODE_PERP;
    				}
    			}
    		}
    	if (osmode & IC_OSMODE_MID)//MID
    		{
    		//1-2
    		p5[0]=(p1[0]+p2[0])/2.0;
    		p5[1]=(p1[1]+p2[1])/2.0;
    		gra_ucs2rp(p5,p6,view);
    		if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
    			{
    			ic_ptcpy(ospt,p5);
    			ic_ptcpy(osptrp,p6);
//					g_nSnappedType= IC_OSMODE_MID;
    			}
    		//2-3
    		p5[0]=(p2[0]+p3[0])/2.0;
    		p5[1]=(p2[1]+p3[1])/2.0;
    		gra_ucs2rp(p5,p6,view);
    		if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
    			{
    			ic_ptcpy(ospt,p5);
    			ic_ptcpy(osptrp,p6);
//					g_nSnappedType= IC_OSMODE_MID;
    			}
    		//3-4
    		p5[0]=(p4[0]+p3[0])/2.0;
    		p5[1]=(p4[1]+p3[1])/2.0;
    		p5[2]=(p4[2]+p3[2])/2.0;
    		gra_ucs2rp(p5,p6,view);
    		if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
    			{
    			ic_ptcpy(ospt,p5);
    			ic_ptcpy(osptrp,p6);
//					g_nSnappedType= IC_OSMODE_MID;
    			}
    		//1-4
    		p5[0]=(p4[0]+p1[0])/2.0;
    		p5[1]=(p4[1]+p1[1])/2.0;
    		p5[2]=(p4[2]+p1[2])/2.0;
    		gra_ucs2rp(p5,p6,view);
    		if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
    			{
    			ic_ptcpy(ospt,p5);
    			ic_ptcpy(osptrp,p6);
//					g_nSnappedType= IC_OSMODE_MID;
    			}
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

    	if (osmode & IC_OSMODE_END)	//END
    		{
    		fi1=fi2=0;
    		for (rbtemp=elist;rbtemp!=NULL;rbtemp=rbtemp->rbnext)
    			{
    			if (rbtemp->restype==74)
    				fi1=rbtemp->resval.rint;
    			if (fi1==0)
    				continue;//if we haven't hit a 74 yet, continue
    			if (rbtemp->restype==11)
    				{
    				fi2++;
    				if (fi2==1 || fi2==fi1)
    					{
    					sds_trans(rbtemp->resval.rpoint,&rbwcs,&rbucs,0,p5);
    					gra_ucs2rp(p5,p6,view);
    					if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
    						{
    						ic_ptcpy(ospt,p5);
    						ic_ptcpy(osptrp,p6);
//								g_nSnappedType= IC_OSMODE_END;
    						}
    					if (fi2==fi1)//speed up looping
    						{
    						rbtemp=NULL;
    						break;
    						}
    					}
    				}
    			}
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

				sds_trans(p3,&rbwcs,&rbucs,0,p4);
				gr_ucs2rp(p4,p3,view);

    			if (SDS_osnap_chkdsq(pickptrp,p3,osptrp,dsq)) 
					{
    				ic_ptcpy(ospt,p4);
    				ic_ptcpy(osptrp,p3);
	//					g_nSnappedType= IC_OSMODE_NEAR;
    				}
				}
		    }		 
		if (osmode & IC_OSMODE_MID) //MID
			{
			if (!nurbs.Closed())
				{
				//check starting point
				double mid = (pNurbs->Start() + pNurbs->End()) / 2.0;

				C3Point midPoint, tangent;
				if (nurbs.Evaluate(mid, midPoint, tangent) == SUCCESS) 
					{
					p3[0] = midPoint.X();
					p3[1] = midPoint.Y();
					p3[2] = midPoint.Z();

					sds_trans(p3,&rbwcs,&rbucs,0,p4);
					gr_ucs2rp(p4,p3,view);
    				if (SDS_osnap_chkdsq(pickptrp,p3,osptrp,dsq)) 
						{
    					ic_ptcpy(ospt,p4);
    					ic_ptcpy(osptrp,p3);
		//					g_nSnappedType= IC_OSMODE_MID;
    					}
					}	
				}
		    }
		if ((osmode & (IC_OSMODE_PERP | IC_OSMODE_NEAR))==IC_OSMODE_PERP)//PER must be calc'd in ECS
			{			
			sds_trans(lastpt,&rbucs,&rbwcs,0,p5);
            C3Point lastPoint(p5[0], p5[1], p5[2]);

			sds_trans(pickptucs,&rbucs,&rbwcs,0,p6);
            C3Point ptPickd(p6[0], p6[1], p6[2]);

			double param;
			C3Point ptPerpendicular;
			if (nurbs.PerpendicularThru(lastPoint, ptPickd, ptPerpendicular, param) == SUCCESS)
				{
				p3[0] = ptPerpendicular.X();
				p3[1] = ptPerpendicular.Y();
				p3[2] = ptPerpendicular.Z();

				sds_trans(p3,&rbwcs,&rbucs,0,p4);
				gr_ucs2rp(p4,p3,view);

    			if (SDS_osnap_chkdsq(pickptrp,p3,osptrp,dsq)) 
					{
    				ic_ptcpy(ospt,p4);
    				ic_ptcpy(osptrp,p3);
	//					g_nSnappedType= IC_OSMODE_PERP;
    				}
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

    			if (SDS_osnap_chkdsq(pickptrp,p3,osptrp,dsq)) 
					{
    				ic_ptcpy(ospt,p4);
    				ic_ptcpy(osptrp,p3);
	//					g_nSnappedType= TAN;
    				}
				}	
			}
    	}
    else if (strsame(etype,db_hitype2str(DB_POINT)))
    	{
    	if (osmode & (IC_OSMODE_NEAR | IC_OSMODE_NODE))	//NODE(a.k.a. POINT) or NEA
    		{
    		ic_assoc(elist,10);
    		ic_ptcpy(p1,ic_rbassoc->resval.rpoint);
    		if (m_ecs2ucs)
    			ic_usemat(p1,p1,m_ecs2ucs,0,0,0);
    		else
    			sds_trans(p1,&rbwcs,&rbucs,0,p1);
    		gra_ucs2rp(p1,p2,view);
    		if (SDS_osnap_chkdsq(pickptrp,p2,osptrp,dsq))
    			{
    			ic_ptcpy(ospt,p1);
    			ic_ptcpy(osptrp,p2);
//					if (osmode & IC_OSMODE_NEAR )
//						g_nSnappedType= IC_OSMODE_NEAR;//according to snap priority
//					else
//						g_nSnappedType= IC_OSMODE_NODE;
    			}
    		if (thickflg)
    			{
                ic_assoc(elist,10);
    			ic_ptcpy(p11,ic_rbassoc->resval.rpoint);
    			ic_assoc(elist,210);
                ic_rbassoc->resval.rpoint[0]*=thickness;ic_rbassoc->resval.rpoint[1]*=thickness;ic_rbassoc->resval.rpoint[1]*=thickness;
                p11[0]+=ic_rbassoc->resval.rpoint[0];p11[1]+=ic_rbassoc->resval.rpoint[1];p11[2]+=ic_rbassoc->resval.rpoint[2];
                sds_trans(p11,&rbwcs,&rbucs,0,p11);
    			gra_ucs2rp(p11,p12,view);
    			if (SDS_osnap_chkdsq(pickptrp,p12,osptrp,dsq))
    				{
    				ic_ptcpy(ospt,p11);
    				ic_ptcpy(osptrp,p12);
//						if (osmode & IC_OSMODE_NEAR )
//							g_nSnappedType= IC_OSMODE_NEAR;//according to snap priority
//						else
//							g_nSnappedType= IC_OSMODE_NODE;
    				}
    			}
    		}
    	}
    else if (strsame(etype,db_hitype2str(DB_MTEXT)))
    	{
    	if (osmode & (IC_OSMODE_INS | IC_OSMODE_NEAR))	//INS or NEA
    		{
    		ic_assoc(elist,10);
    		ic_ptcpy(p1,ic_rbassoc->resval.rpoint);
    		if (m_ecs2ucs)
    			ic_usemat(p1,p1,m_ecs2ucs,0,0,0);
    		else
    			sds_trans(p1,&rbwcs,&rbucs,0,p1);
    		gra_ucs2rp(p1,p2,view);
    		if (SDS_osnap_chkdsq(pickptrp,p2,osptrp,dsq))
    			{
    			ic_ptcpy(ospt,p1);
    			ic_ptcpy(osptrp,p2);
//					if (osmode & IC_OSMODE_NEAR )
//						g_nSnappedType= IC_OSMODE_NEAR;//according to snap priority
//					else
//						g_nSnappedType= IC_OSMODE_INS;
    			}
    		if (thickflg)
    			{
                ic_assoc(elist,10);
    			ic_ptcpy(p11,ic_rbassoc->resval.rpoint);
    			ic_assoc(elist,210);
                ic_rbassoc->resval.rpoint[0]*=thickness;ic_rbassoc->resval.rpoint[1]*=thickness;ic_rbassoc->resval.rpoint[1]*=thickness;
                p11[0]+=ic_rbassoc->resval.rpoint[0];p11[1]+=ic_rbassoc->resval.rpoint[1];p11[2]+=ic_rbassoc->resval.rpoint[2];
                sds_trans(p11,&rbwcs,&rbucs,0,p11);
    			gra_ucs2rp(p11,p12,view);
    			if (SDS_osnap_chkdsq(pickptrp,p12,osptrp,dsq))
    				{
    				ic_ptcpy(ospt,p11);
    				ic_ptcpy(osptrp,p12);
//						if (osmode & IC_OSMODE_NEAR )
//							g_nSnappedType= IC_OSMODE_NEAR;//according to snap priority
//						else
//							g_nSnappedType= IC_OSMODE_INS;
    				}
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
    	gra_ucs2rp(p1,p3,view);
    	gra_ucs2rp(p2,p4,view);
    	if ((osmode & IC_OSMODE_END) && !strsame(etype,db_hitype2str(DB_XLINE)))//END for line & ray
    		{
    		if (SDS_osnap_chkdsq(pickptrp,p3,osptrp,dsq))
    			{
    			ic_ptcpy(ospt,p1);
    			ic_ptcpy(osptrp,p3);
//					g_nSnappedType= IC_OSMODE_END;
    			}
    		if (strsame(etype,db_hitype2str(DB_LINE)) &&
    			SDS_osnap_chkdsq(pickptrp,p4,osptrp,dsq))
    			{
    			ic_ptcpy(ospt,p2);
    			ic_ptcpy(osptrp,p4);
//					g_nSnappedType= IC_OSMODE_END;
    			}
    		}
        if (thickflg)
    		{
    		gra_ucs2rp(p11,p13,view);
    		gra_ucs2rp(p12,p14,view);
    		if ((osmode & IC_OSMODE_END) && !strsame(etype,db_hitype2str(DB_XLINE)))//END for line & ray
    			{
    			if (SDS_osnap_chkdsq(pickptrp,p13,osptrp,dsq))
    				{
    				ic_ptcpy(ospt,p11);
    				ic_ptcpy(osptrp,p13);
//						g_nSnappedType= IC_OSMODE_END;
    				}
    			if (strsame(etype,db_hitype2str(DB_LINE)) &&
    				SDS_osnap_chkdsq(pickptrp,p14,osptrp,dsq))
    				{
    				ic_ptcpy(ospt,p12);
    				ic_ptcpy(osptrp,p14);
//						g_nSnappedType= IC_OSMODE_END;
    				}
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
    			if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
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
//						g_nSnappedType= IC_OSMODE_NEAR;
    				}

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
    				if (SDS_osnap_chkdsq(pickptrp,p15,osptrp,dsq))
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
//							g_nSnappedType= IC_OSMODE_NEAR;
    					}

    				}
    			//now check end cap of line or ray for nearest
    			if (!strsame(etype,db_hitype2str(DB_XLINE)))//xlines have no end cap
    				{
    				if (1==ic_ptlndist(pickptrp,p3,p13,&fr5,p5))
    					{
    					if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
    						{
    						ic_ptcpy(osptrp,p5);
    						fr5=sds_distance(p3,p5)/sds_distance(p3,p13);
    						ospt[0]=p1[0]+fr5*(p11[0]-p1[0]);
    						ospt[1]=p1[1]+fr5*(p11[1]-p1[1]);
    						ospt[2]=p1[2]+fr5*(p11[2]-p1[2]);
//								g_nSnappedType= IC_OSMODE_NEAR;
    						}
    					}
    				if (strsame(etype,db_hitype2str(DB_LINE)))
    					{
    					if (1==ic_ptlndist(pickptrp,p4,p14,&fr5,p5))
    						{
    						ic_ptcpy(osptrp,p5);
    						fr5=sds_distance(p4,p5)/sds_distance(p4,p14);
    						ospt[0]=p2[0]+fr5*(p12[0]-p2[0]);
    						ospt[1]=p2[1]+fr5*(p12[1]-p2[1]);
    						ospt[2]=p2[2]+fr5*(p12[2]-p2[2]);
    						}
    					}
    				}
    			}
    		}
    	if (osmode & (IC_OSMODE_PERP | IC_OSMODE_TAN))//PER or TAN
    		{
    		//note: on perpendicular, we must do math in UCS
    		fi1=ic_ptlndist(lastpt,p1,p2,&fr1,p5);
    		gr_ucs2rp(lastpt,p7,view);
    		if (fi1>-1)
    			{
    			fr2=sds_dist2d(p3,p7)/sds_dist2d(p3,p4);
    			//on next line, include (fi1 ||) for speed
    			if (fi1 || icadDirectionEqual(p1,p2,p1,p5))
    				p5[2]=p1[2]+(p2[2]-p1[2])*fr2;
    			else
    				p5[2]=p2[2]+(p1[2]-p2[2])*(fr2+1.0);
    			gra_ucs2rp(p5,p6,view);
    			if ( SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
    				{
    				ic_ptcpy(osptrp,p6);
    				ic_ptcpy(ospt,p5);
//						if (osmode & IC_OSMODE_PERP )
//							g_nSnappedType= IC_OSMODE_PERP;//according to snap priority
//						else
//							g_nSnappedType= IC_OSMODE_TAN;
    				}
    			}
    		if (thickflg)
    			{
                fi1=ic_ptlndist(lastpt,p11,p12,&fr1,p15);
    			if (fi1>-1)
    				{
    				fr2=sds_dist2d(p11,p15)/sds_dist2d(p11,p12);
    				//on next line, include (fi1 ||) for speed
    				if (fi1 || icadDirectionEqual(p11,p12,p11,p15))
    					p15[2]=p11[2]+(p12[2]-p11[2])*fr2;
    				else
    					p15[2]=p12[2]+(p11[2]-p12[2])*(fr2+1.0);
    				gra_ucs2rp(p15,p16,view);
    				if ( SDS_osnap_chkdsq(pickptrp,p16,osptrp,dsq))
    					{
    					ic_ptcpy(osptrp,p16);
    					ic_ptcpy(ospt,p15);
//							if (osmode & IC_OSMODE_PERP )
//								g_nSnappedType= IC_OSMODE_PERP;//according to snap priority
//							else
//								g_nSnappedType= IC_OSMODE_TAN;
    					}
    				}
    			}
    		}
    	if (osmode & IC_OSMODE_MID)//MID
    		{
    		if (strsame(etype,db_hitype2str(DB_LINE)))
    			{
    			p3[0]=(p1[0]+p2[0])/2.0;
    			p3[1]=(p1[1]+p2[1])/2.0;
    			p3[2]=(p1[2]+p2[2])/2.0;
    			gra_ucs2rp(p3,p5,view);
    			if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
    				{
    				ic_ptcpy(ospt,p3);
    				ic_ptcpy(osptrp,p5);
//						g_nSnappedType= IC_OSMODE_MID;
    				}
    			if (thickflg)
    				{
    				p13[0]=(p11[0]+p12[0])/2.0;
    				p13[1]=(p11[1]+p12[1])/2.0;
    				p13[2]=(p11[2]+p12[2])/2.0;
    				gra_ucs2rp(p13,p15,view);
    				if (SDS_osnap_chkdsq(pickptrp,p15,osptrp,dsq))
    					{
    					ic_ptcpy(ospt,p13);
    					ic_ptcpy(osptrp,p15);
//							g_nSnappedType= IC_OSMODE_MID;
    					}
    				p13[0]=(p11[0]+p1[0])/2.0;
    				p13[1]=(p11[1]+p1[1])/2.0;
    				p13[2]=(p11[2]+p1[2])/2.0;
    				gra_ucs2rp(p13,p15,view);
    				if (SDS_osnap_chkdsq(pickptrp,p15,osptrp,dsq))
    					{
    					ic_ptcpy(ospt,p13);
    					ic_ptcpy(osptrp,p15);
//							g_nSnappedType= IC_OSMODE_MID;
    					}
    				p13[0]=(p2[0]+p12[0])/2.0;
    				p13[1]=(p2[1]+p12[1])/2.0;
    				p13[2]=(p2[2]+p12[2])/2.0;
    				gra_ucs2rp(p13,p15,view);
    				if (SDS_osnap_chkdsq(pickptrp,p15,osptrp,dsq))
    					{
    					ic_ptcpy(ospt,p13);
    					ic_ptcpy(osptrp,p15);
//							g_nSnappedType= IC_OSMODE_MID;
    					}
    				}
    			}
    		else if (strsame(etype,db_hitype2str(DB_XLINE)))//yes, it's possible
    			{
    			gra_ucs2rp(p1,p5,view);
    			if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
    				{
    				ic_ptcpy(ospt,p1);
    				ic_ptcpy(osptrp,p5);
//						g_nSnappedType= IC_OSMODE_MID;
    				}
    			if (thickflg)
    				{
    				gra_ucs2rp(p11,p15,view);
    				if (SDS_osnap_chkdsq(pickptrp,p15,osptrp,dsq))
    					{
    					ic_ptcpy(ospt,p11);
    					ic_ptcpy(osptrp,p15);
//							g_nSnappedType= IC_OSMODE_MID;
    					}
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
    		gra_ucs2rp(p4,p2,view);
    		if (SDS_osnap_chkdsq(pickptrp,p2,osptrp,dsq))
    			{
    			ic_ptcpy(ospt,p4);
    			ic_ptcpy(osptrp,p2);
//					g_nSnappedType= IC_OSMODE_END;
    			}
    		sds_polar(p1,fr3,fr1,p2);
    		if (m_ecs2ucs)
    			ic_usemat(p2,p4,m_ecs2ucs,0,0,0);
    		else
    			sds_trans(p2,&rbent,&rbucs,0,p4);
    		gra_ucs2rp(p4,p2,view);
    		if (SDS_osnap_chkdsq(pickptrp,p2,osptrp,dsq))
    			{
    			ic_ptcpy(ospt,p4);
    			ic_ptcpy(osptrp,p2);
//					g_nSnappedType= IC_OSMODE_END;
    			}
    		if (thickflg)
    			{
                sds_polar(p11,fr2,fr1,p12);
    			if (m_ecs2ucs)
    				ic_usemat(p12,p14,m_ecs2ucs,0,0,0);
    			else
    				sds_trans(p12,&rbent,&rbucs,0,p14);
    			gra_ucs2rp(p14,p12,view);
    			if (SDS_osnap_chkdsq(pickptrp,p12,osptrp,dsq))
    				{
    				ic_ptcpy(ospt,p14);
    				ic_ptcpy(osptrp,p12);
//						g_nSnappedType= IC_OSMODE_END;
    				}
    			sds_polar(p11,fr3,fr1,p12);
    			if (m_ecs2ucs)
    				ic_usemat(p12,p14,m_ecs2ucs,0,0,0);
    			else
    				sds_trans(p12,&rbent,&rbucs,0,p14);
    			gra_ucs2rp(p14,p12,view);
    			if (SDS_osnap_chkdsq(pickptrp,p12,osptrp,dsq))
    				{
    				ic_ptcpy(ospt,p14);
    				ic_ptcpy(osptrp,p12);
//						g_nSnappedType= IC_OSMODE_END;
    				}
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
    		gra_ucs2rp(p2,p3,view);
			if (SDS_osnap_chkdsq(pickptrp,p3,osptrp,dsq)){ic_ptcpy(ospt,p2);ic_ptcpy(osptrp,p3);}
//				if (SDS_osnap_chkdsq(pickptrp,p3,osptrp,dsq))
//				{
//					ic_ptcpy(ospt,p2);
//					ic_ptcpy(osptrp,p3);
//					g_nSnappedType= IC_OSMODE_NEAR;
//				}
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
    			gra_ucs2rp(p12,p13,view);
    			if (SDS_osnap_chkdsq(pickptrp,p13,osptrp,dsq))
    				{
    				ic_ptcpy(ospt,p12);
    				ic_ptcpy(osptrp,p13);
//						g_nSnappedType= IC_OSMODE_NEAR;
    				}

    			//now take p14 & snap it to the arc
    			if (strsame(etype,db_hitype2str(DB_ARC)))
    				{
    				sds_polar(p11,fr1,fr2,p13);
    				sds_polar(p1,fr1,fr2,p3);
    				//test along edge of start
    				gra_ucs2rp(p13,p17,view);
    				gra_ucs2rp(p3,p6,view);
    				ic_segdist(pickptrp,p17,p6,0,NULL,p7);
    				if (SDS_osnap_chkdsq(pickptrp,p7,osptrp,dsq))
    					{
    					ic_ptcpy(osptrp,p7);
    					fr5=sds_distance(p17,p7)/sds_distance(p17,p6);
    					ospt[0]=p13[0]+(fr5*(p3[0]-p13[0]));
    					ospt[1]=p13[1]+(fr5*(p3[1]-p13[1]));
    					ospt[2]=p13[2]+(fr5*(p3[2]-p13[2]));
//							g_nSnappedType= IC_OSMODE_NEAR;
    					}
    				sds_polar(p11,fr1,fr3,p14);
    				sds_polar(p1,fr1,fr3,p4);
    				//test along edge of end
    				gra_ucs2rp(p14,p17,view);
    				gra_ucs2rp(p4,p6,view);
    				ic_segdist(pickptrp,p17,p6,0,NULL,p7);
    				if (SDS_osnap_chkdsq(pickptrp,p7,osptrp,dsq))
    					{
    					ic_ptcpy(osptrp,p7);
    					fr5=sds_distance(p17,p7)/sds_distance(p17,p6);
    					ospt[0]=p14[0]+(fr5*(p4[0]-p14[0]));
    					ospt[1]=p14[1]+(fr5*(p4[1]-p14[1]));
    					ospt[2]=p14[2]+(fr5*(p4[2]-p14[2]));
//							g_nSnappedType= IC_OSMODE_NEAR;
    					}
    				}
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
    			gra_ucs2rp(p4,p3,view);
    			if (SDS_osnap_chkdsq(pickptrp,p3,osptrp,dsq))
    				{
    				ic_ptcpy(ospt,p4);
    				ic_ptcpy(osptrp,p3);
//						g_nSnappedType= IC_OSMODE_PERP;
    				}
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
    				    gra_ucs2rp(p14,p13,view);
    				    if (SDS_osnap_chkdsq(pickptrp,p13,osptrp,dsq))
    						{
    						ic_ptcpy(ospt,p14);
    						ic_ptcpy(osptrp,p13);
//								g_nSnappedType= IC_OSMODE_PERP;
    						}
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
    					gra_ucs2rp(p7,p5,view);
    					if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
    						{
    						ic_ptcpy(ospt,p7);
    						ic_ptcpy(osptrp,p5);
//								g_nSnappedType= IC_OSMODE_QUADRANT;
    						}
    					if (thickflg)
    						{
    						p6[0]+=thickness*p210[0];
    						p6[1]+=thickness*p210[1];
    						p6[2]+=thickness*p210[2];
    						if (m_ecs2ucs)
    							ic_usemat(p6,p7,m_ecs2ucs,0,0,0);
    						else
    							sds_trans(p6,&rbent,&rbucs,0,p7);
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
    		gra_ucs2rp(p4,p5,view);
    		if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
    			{
    			ic_ptcpy(ospt,p4);
    			ic_ptcpy(osptrp,p5);
//					g_nSnappedType= IC_OSMODE_CENTER;
    			}
    		if (thickflg)
    			{
    			if (m_ecs2ucs)
    				ic_usemat(p11,p14,m_ecs2ucs,0,0,0);
    			else
    				sds_trans(p11,&rbent,&rbucs,0,p14);
    			gra_ucs2rp(p14,p15,view);
    			if (SDS_osnap_chkdsq(pickptrp,p15,osptrp,dsq))
    				{
    				ic_ptcpy(ospt,p14);
    				ic_ptcpy(osptrp,p15);
//						g_nSnappedType= IC_OSMODE_CENTER;
    				}
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
    		gra_ucs2rp(p5,p4,view);
    		if (SDS_osnap_chkdsq(pickptrp,p4,osptrp,dsq))
    			{
    			ic_ptcpy(ospt,p5);
    			ic_ptcpy(osptrp,p4);
//					g_nSnappedType= IC_OSMODE_MID;
    			}
    		if (thickflg)
    			{
                sds_polar(p11,fr4,fr1,p14);
    			if (m_ecs2ucs)
    				ic_usemat(p14,p15,m_ecs2ucs,0,0,0);
    			else
    				sds_trans(p14,&rbent,&rbucs,0,p15);
    			gra_ucs2rp(p15,p14,view);
    			if (SDS_osnap_chkdsq(pickptrp,p14,osptrp,dsq))
    				{
    				ic_ptcpy(ospt,p15);
    				ic_ptcpy(osptrp,p14);
//						g_nSnappedType= IC_OSMODE_MID;
    				}
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
    				gra_ucs2rp(p3,p4,view);
    				if (SDS_osnap_chkdsq(pickptrp,p4,osptrp,dsq))
    					{
    					ic_ptcpy(ospt,p3);
    					ic_ptcpy(osptrp,p4);
//							g_nSnappedType= IC_OSMODE_TAN;
    					}
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
    					gra_ucs2rp(p4,p5,view);
    					if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
    						{
    						ic_ptcpy(ospt,p4);
    						ic_ptcpy(osptrp,p5);
//								g_nSnappedType= IC_OSMODE_TAN;
    						}
    					}
    				if (fi1&4)
    					{
    					if (m_ecs2ucs)
    						ic_usemat(p3,p4,m_ecs2ucs,0,0,0);
    					else
    						sds_trans(p3,&rbent,&rbucs,0,p4);
    					gra_ucs2rp(p4,p5,view);
    					if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
    						{
    						ic_ptcpy(ospt,p4);
    						ic_ptcpy(osptrp,p5);
//								g_nSnappedType= IC_OSMODE_TAN;
    						}
    					}
    				}
    			else if (fi1>=0 && strsame(etype,db_hitype2str(DB_CIRCLE)))
    				{
    				if (m_ecs2ucs)
    					ic_usemat(p2,p4,m_ecs2ucs,0,0,0);
    				else
    					sds_trans(p2,&rbent,&rbucs,0,p4);
    				gra_ucs2rp(p4,p5,view);
    				if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
    					{
    					ic_ptcpy(ospt,p4);
    					ic_ptcpy(osptrp,p5);
//							g_nSnappedType= IC_OSMODE_TAN;
    					}
    				if (m_ecs2ucs)
    					ic_usemat(p3,p4,m_ecs2ucs,0,0,0);
    				else
    					sds_trans(p3,&rbent,&rbucs,0,p4);
    				gra_ucs2rp(p4,p5,view);
    				if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
    					{
    					ic_ptcpy(ospt,p4);
    					ic_ptcpy(osptrp,p5);
//							g_nSnappedType= IC_OSMODE_TAN;
    					}
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
    					gra_ucs2rp(p13,p14,view);
    					if (SDS_osnap_chkdsq(pickptrp,p14,osptrp,dsq))
    						{
    						ic_ptcpy(ospt,p13);
    						ic_ptcpy(osptrp,p14);
//								g_nSnappedType= IC_OSMODE_TAN;
    						}
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
    						gra_ucs2rp(p14,p15,view);
    						if (SDS_osnap_chkdsq(pickptrp,p15,osptrp,dsq))
    							{
    							ic_ptcpy(ospt,p14);
    							ic_ptcpy(osptrp,p15);
//									g_nSnappedType= IC_OSMODE_TAN;
    							}
    						}
    					if (fi1&4)
    						{
    						if (m_ecs2ucs)
    							ic_usemat(p13,p14,m_ecs2ucs,0,0,0);
    						else
    							sds_trans(p13,&rbent,&rbucs,0,p14);
    						gra_ucs2rp(p14,p15,view);
    						if (SDS_osnap_chkdsq(pickptrp,p15,osptrp,dsq))
    							{
    							ic_ptcpy(ospt,p14);
    							ic_ptcpy(osptrp,p15);
//									g_nSnappedType= IC_OSMODE_TAN;
    							}
    						}
    					}
    				else if (fi1>=0 && strsame(etype,db_hitype2str(DB_CIRCLE)))
    					{
    					if (m_ecs2ucs)
    						ic_usemat(p12,p14,m_ecs2ucs,0,0,0);
    					else
    						sds_trans(p12,&rbent,&rbucs,0,p14);
    					gra_ucs2rp(p14,p15,view);
    					if (SDS_osnap_chkdsq(pickptrp,p15,osptrp,dsq))
    						{
    						ic_ptcpy(ospt,p14);
    						ic_ptcpy(osptrp,p15);
//								g_nSnappedType= IC_OSMODE_TAN;
    						}
    					if (m_ecs2ucs)
    						ic_usemat(p13,p14,m_ecs2ucs,0,0,0);
    					else
    						sds_trans(p13,&rbent,&rbucs,0,p14);
    					gra_ucs2rp(p14,p15,view);
    					if (SDS_osnap_chkdsq(pickptrp,p15,osptrp,dsq))
    						{
    						ic_ptcpy(ospt,p14);
    						ic_ptcpy(osptrp,p15);
//								g_nSnappedType= IC_OSMODE_TAN;
    						}
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
    			ic_ptcpy(p1,rbtemp->resval.rpoint);	// center
    		else if (rbtemp->restype==11)
    			ic_ptcpy(p2,rbtemp->resval.rpoint);	// major axis
    		else if (rbtemp->restype==40)
    			fr1=rbtemp->resval.rreal;			// the ratio of major/minor axes
    		else if (rbtemp->restype==41)
    			fr2=rbtemp->resval.rreal;			// start parameter
    		else if (rbtemp->restype==42)
    			fr3=rbtemp->resval.rreal;			// end parameter
    		else if (rbtemp->restype==210)
    			ic_ptcpy(p5,rbtemp->resval.rpoint);	// extrusion direction
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
    			gra_ucs2rp(p4,p3,view);
    			if (SDS_osnap_chkdsq(pickptrp,p3,osptrp,dsq))
    				{
    				ic_ptcpy(ospt,p4);
    				ic_ptcpy(osptrp,p3);
//						g_nSnappedType= IC_OSMODE_END;
    				}

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
    			gra_ucs2rp(p4,p3,view);
    			if (SDS_osnap_chkdsq(pickptrp,p3,osptrp,dsq))
    				{
    				ic_ptcpy(ospt,p4);
    				ic_ptcpy(osptrp,p3);
//						g_nSnappedType= IC_OSMODE_END;
					}
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

    		if (SDS_osnap_chkdsq(pickptrp,p3,osptrp,dsq)) 
				{
    			ic_ptcpy(ospt,p4);
    			ic_ptcpy(osptrp,p3);
//					g_nSnappedType= IC_OSMODE_NEAR;
    			}
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
    			if (SDS_osnap_chkdsq(pickptrp,p3,osptrp,dsq)) 
					{
    				ic_ptcpy(ospt,p4);
    				ic_ptcpy(osptrp,p3);
//						g_nSnappedType= IC_OSMODE_MID;
					}
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

    			if (SDS_osnap_chkdsq(pickptrp,p3,osptrp,dsq)) 
					{
    				ic_ptcpy(ospt,p4);
    				ic_ptcpy(osptrp,p3);
//						g_nSnappedType= IC_OSMODE_PERP;
					}
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

    				if (SDS_osnap_chkdsq(pickptrp,p3,osptrp,dsq)) 
						{
    					ic_ptcpy(ospt,p4);
    					ic_ptcpy(osptrp,p3);
	//						g_nSnappedType= IC_OSMODE_TAN;
						}
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
    				gra_ucs2rp(p4,p5,view);
    				if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
    					{
    					ic_ptcpy(ospt,p4);
    					ic_ptcpy(osptrp,p5);
//							g_nSnappedType= IC_OSMODE_QUADRANT;
    					}
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
    		gra_ucs2rp(p4,p5,view);
    		if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
    			{
    			ic_ptcpy(ospt,p4);
    			ic_ptcpy(osptrp,p5);
//					g_nSnappedType= IC_OSMODE_CENTER;
    			}
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
    		gra_ucs2rp(p5,p6,view);
    		if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
    			{
    			ic_ptcpy(ospt,p5);
    			ic_ptcpy(osptrp,p6);
//					g_nSnappedType= IC_OSMODE_CENTER;
    			}
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
    			gra_ucs2rp(p15,p16,view);
    			if (SDS_osnap_chkdsq(pickptrp,p16,osptrp,dsq))
    				{
    				ic_ptcpy(ospt,p15);
    				ic_ptcpy(osptrp,p16);
//						g_nSnappedType= IC_OSMODE_CENTER;
    				}
    			}
    		}
    	if (osmode & (IC_OSMODE_END | IC_OSMODE_INT | IC_OSMODE_APP | IC_OSMODE_PLAN))
    		{
    		//INT modes or END
    		if (strsame(etype,db_hitype2str(DB_3DFACE)))  //int only
    			{
    			if (!(fi2&(8+1)==9))
    				{
    				gra_ucs2rp(p1,p5,view);
    				if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
    					{
    					ic_ptcpy(ospt,p1);
    					ic_ptcpy(osptrp,p5);
//							if (osmode & IC_OSMODE_END )
//								g_nSnappedType= IC_OSMODE_END;//according to snap priority
//							else
//								g_nSnappedType= IC_OSMODE_INT;
    					}
    				}
    			if (!(fi2&(1+2)==3))
    				{
    				gra_ucs2rp(p2,p5,view);
    				if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
    					{
    					ic_ptcpy(ospt,p2);
    					ic_ptcpy(osptrp,p5);
//							if (osmode & IC_OSMODE_END )
//								g_nSnappedType= IC_OSMODE_END;//according to snap priority
//							else
//								g_nSnappedType= IC_OSMODE_INT;
    					}
    				}
    			if (!(fi2&(2+4)==6))
    				{
    				gra_ucs2rp(p3,p5,view);
    				if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
    					{
    					ic_ptcpy(ospt,p3);
    					ic_ptcpy(osptrp,p5);
//							if (osmode & IC_OSMODE_END )
//								g_nSnappedType= IC_OSMODE_END;//according to snap priority
//							else
//								g_nSnappedType= IC_OSMODE_INT;
    					}
    				}
    			if (!(fi2&(4+8)==12))
    				{
    				gra_ucs2rp(p4,p5,view);
    				if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
    					{
    					ic_ptcpy(ospt,p4);
    					ic_ptcpy(osptrp,p5);
//							if (osmode & IC_OSMODE_END )
//								g_nSnappedType= IC_OSMODE_END;//according to snap priority
//							else
//								g_nSnappedType= IC_OSMODE_INT;
    					}
    				}
    			}
    		else
    			{
    			gra_ucs2rp(p1,p5,view);
    			if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
    				{
    				ic_ptcpy(ospt,p1);
    				ic_ptcpy(osptrp,p5);
//						if (osmode & IC_OSMODE_END )
//							g_nSnappedType= IC_OSMODE_END;//according to snap priority
//						else
//							g_nSnappedType= IC_OSMODE_INT;
    				}
    			gra_ucs2rp(p2,p5,view);
    			if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
    				{
    				ic_ptcpy(ospt,p2);
    				ic_ptcpy(osptrp,p5);
//						if (osmode & IC_OSMODE_END )
//							g_nSnappedType= IC_OSMODE_END;//according to snap priority
//						else
//							g_nSnappedType= IC_OSMODE_INT;
    				}
    			gra_ucs2rp(p3,p5,view);
    			if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
    				{
    				ic_ptcpy(ospt,p3);
    				ic_ptcpy(osptrp,p5);
//						if (osmode & IC_OSMODE_END )
//							g_nSnappedType= IC_OSMODE_END;//according to snap priority
//						else
//							g_nSnappedType= IC_OSMODE_INT;
    				}
    			gra_ucs2rp(p4,p5,view);
    			if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
    				{
    				ic_ptcpy(ospt,p4);
    				ic_ptcpy(osptrp,p5);
//						if (osmode & IC_OSMODE_END )
//							g_nSnappedType= IC_OSMODE_END;//according to snap priority
//						else
//							g_nSnappedType= IC_OSMODE_INT;
    				}
    			}
    		if (thickflg)
    			{
                if (strsame(etype,db_hitype2str(DB_3DFACE)))  //int only
    				{
    				if (!(fi2&(8+1)==9))
    					{
    					gra_ucs2rp(p11,p15,view);
    					if (SDS_osnap_chkdsq(pickptrp,p15,osptrp,dsq))
    						{
    						ic_ptcpy(ospt,p11);
    						ic_ptcpy(osptrp,p15);
//								if (osmode & IC_OSMODE_END )
//									g_nSnappedType= IC_OSMODE_END;//according to snap priority
//								else
//									g_nSnappedType= IC_OSMODE_INT;
    						}
    					}
    				if (!(fi2&(1+2)==3))
    					{
    					gra_ucs2rp(p12,p15,view);
    					if (SDS_osnap_chkdsq(pickptrp,p15,osptrp,dsq))
    						{
    						ic_ptcpy(ospt,p12);
    						ic_ptcpy(osptrp,p15);
//								if (osmode & IC_OSMODE_END )
//									g_nSnappedType= IC_OSMODE_END;//according to snap priority
//								else
//									g_nSnappedType= IC_OSMODE_INT;
    						}
    					}
    				if (!(fi2&(2+4)==6))
    					{
    					gra_ucs2rp(p13,p15,view);
    					if (SDS_osnap_chkdsq(pickptrp,p15,osptrp,dsq))
    						{
    						ic_ptcpy(ospt,p13);
    						ic_ptcpy(osptrp,p15);
//								if (osmode & IC_OSMODE_END )
//									g_nSnappedType= IC_OSMODE_END;//according to snap priority
//								else
//									g_nSnappedType= IC_OSMODE_INT;
    						}
    					}
    				if (!(fi2&(4+8)==12))
    					{
    					gra_ucs2rp(p14,p15,view);
    					if (SDS_osnap_chkdsq(pickptrp,p15,osptrp,dsq))
    						{
    						ic_ptcpy(ospt,p14);
    						ic_ptcpy(osptrp,p15);
//								if (osmode & IC_OSMODE_END )
//									g_nSnappedType= IC_OSMODE_END;//according to snap priority
//								else
//									g_nSnappedType= IC_OSMODE_INT;
    						}
    					}
    				}
    			else
    				{
    				gra_ucs2rp(p11,p15,view);
    				if (SDS_osnap_chkdsq(pickptrp,p15,osptrp,dsq))
    					{
    					ic_ptcpy(ospt,p11);
    					ic_ptcpy(osptrp,p15);
//							if (osmode & IC_OSMODE_END )
//								g_nSnappedType= IC_OSMODE_END;//according to snap priority
//							else
//								g_nSnappedType= IC_OSMODE_INT;
    					}
    				gra_ucs2rp(p12,p15,view);
    				if (SDS_osnap_chkdsq(pickptrp,p15,osptrp,dsq))
    					{
    					ic_ptcpy(ospt,p12);
    					ic_ptcpy(osptrp,p15);
//							if (osmode & IC_OSMODE_END )
//								g_nSnappedType= IC_OSMODE_END;//according to snap priority
//							else
//								g_nSnappedType= IC_OSMODE_INT;
    					}
    				gra_ucs2rp(p13,p15,view);
    				if (SDS_osnap_chkdsq(pickptrp,p15,osptrp,dsq))
    					{
    					ic_ptcpy(ospt,p13);
    					ic_ptcpy(osptrp,p15);
//							if (osmode & IC_OSMODE_END )
//								g_nSnappedType= IC_OSMODE_END;//according to snap priority
//							else
//								g_nSnappedType= IC_OSMODE_INT;
    					}
    				gra_ucs2rp(p14,p15,view);
    				if (SDS_osnap_chkdsq(pickptrp,p15,osptrp,dsq))
    					{
    					ic_ptcpy(ospt,p14);
    					ic_ptcpy(osptrp,p15);
//							if (osmode & IC_OSMODE_END )
//								g_nSnappedType= IC_OSMODE_END;//according to snap priority
//							else
//								g_nSnappedType= IC_OSMODE_INT;
    					}
    				}
    			}
    		}
    	if (osmode & IC_OSMODE_NEAR)//NEA
    		{
    		//NOTE: all nea calcs depend upon view, so they must be done in RP!
    		if (!strsame(etype,db_hitype2str(DB_3DFACE)))  //solid & trace
    			{
    			gra_ucs2rp(p1,p5,view);
    			gra_ucs2rp(p2,p6,view);
    			ic_segdist(pickptrp,p5,p6,0,&fr1,p7);
    			if (SDS_osnap_chkdsq(pickptrp,p7,osptrp,dsq))
    				{
    				ic_ptcpy(osptrp,p7);
    				fr1=sds_distance(p5,p7)/sds_distance(p5,p6);
    				ospt[0]=p1[0]+(fr1*(p2[0]-p1[0]));
    				ospt[1]=p1[1]+(fr1*(p2[1]-p1[1]));
    				ospt[2]=p1[2]+(fr1*(p2[2]-p1[2]));
//						g_nSnappedType= IC_OSMODE_NEAR;
    				}
    			gra_ucs2rp(p3,p6,view);
    			ic_segdist(pickptrp,p5,p6,0,&fr1,p7);
    			if (SDS_osnap_chkdsq(pickptrp,p7,osptrp,dsq))
    				{
    				ic_ptcpy(osptrp,p7);
    				fr1=sds_distance(p5,p7)/sds_distance(p5,p6);
    				ospt[0]=p1[0]+(fr1*(p3[0]-p1[0]));
    				ospt[1]=p1[1]+(fr1*(p3[1]-p1[1]));
    				ospt[2]=p1[2]+(fr1*(p3[2]-p1[2]));
//						g_nSnappedType= IC_OSMODE_NEAR;
    				}
    			gra_ucs2rp(p4,p5,view);
    			gra_ucs2rp(p2,p6,view);
    			ic_segdist(pickptrp,p5,p6,0,&fr1,p7);
    			if (SDS_osnap_chkdsq(pickptrp,p7,osptrp,dsq))
    				{
    				ic_ptcpy(osptrp,p7);
    				fr1=sds_distance(p5,p7)/sds_distance(p5,p6);
    				ospt[0]=p4[0]+(fr1*(p2[0]-p4[0]));
    				ospt[1]=p4[1]+(fr1*(p2[1]-p4[1]));
    				ospt[2]=p4[2]+(fr1*(p2[2]-p4[2]));
//						g_nSnappedType= IC_OSMODE_NEAR;
    				}
    			gra_ucs2rp(p3,p6,view);
    			ic_segdist(pickptrp,p5,p6,0,&fr1,p7);
    			if (SDS_osnap_chkdsq(pickptrp,p7,osptrp,dsq))
    				{
    				ic_ptcpy(osptrp,p7);
    				fr1=sds_distance(p5,p7)/sds_distance(p5,p6);
    				ospt[0]=p4[0]+(fr1*(p3[0]-p4[0]));
    				ospt[1]=p4[1]+(fr1*(p3[1]-p4[1]));
    				ospt[2]=p4[2]+(fr1*(p3[2]-p4[2]));
//						g_nSnappedType= IC_OSMODE_NEAR;
    				}
    			if (thickflg)
    				{
    				gra_ucs2rp(p11,p15,view);
    				gra_ucs2rp(p12,p16,view);
    				ic_segdist(pickptrp,p15,p16,0,&fr1,p17);
    				if (SDS_osnap_chkdsq(pickptrp,p17,osptrp,dsq))
    					{
    					ic_ptcpy(osptrp,p17);
    					fr1=sds_distance(p15,p17)/sds_distance(p15,p16);
    					ospt[0]=p11[0]+(fr1*(p12[0]-p11[0]));
    					ospt[1]=p11[1]+(fr1*(p12[1]-p11[1]));
    					ospt[2]=p11[2]+(fr1*(p12[2]-p11[2]));
//							g_nSnappedType= IC_OSMODE_NEAR;
    					}
    				gra_ucs2rp(p13,p16,view);
    				ic_segdist(pickptrp,p15,p16,0,&fr1,p17);
    				if (SDS_osnap_chkdsq(pickptrp,p17,osptrp,dsq))
    					{
    					ic_ptcpy(osptrp,p17);
    					fr1=sds_distance(p15,p17)/sds_distance(p15,p16);
    					ospt[0]=p11[0]+(fr1*(p13[0]-p11[0]));
    					ospt[1]=p11[1]+(fr1*(p13[1]-p11[1]));
    					ospt[2]=p11[2]+(fr1*(p13[2]-p11[2]));
//							g_nSnappedType= IC_OSMODE_NEAR;
    					}
    				gra_ucs2rp(p14,p15,view);
    				gra_ucs2rp(p12,p16,view);
    				ic_segdist(pickptrp,p15,p16,0,&fr1,p17);
    				if (SDS_osnap_chkdsq(pickptrp,p17,osptrp,dsq))
    					{
    					ic_ptcpy(osptrp,p17);
    					fr1=sds_distance(p15,p17)/sds_distance(p15,p16);
    					ospt[0]=p14[0]+(fr1*(p12[0]-p14[0]));
    					ospt[1]=p14[1]+(fr1*(p12[1]-p14[1]));
    					ospt[2]=p14[2]+(fr1*(p12[2]-p14[2]));
//							g_nSnappedType= IC_OSMODE_NEAR;
    					}
    				gra_ucs2rp(p13,p16,view);
    				ic_segdist(pickptrp,p15,p16,0,&fr1,p17);
    				if (SDS_osnap_chkdsq(pickptrp,p17,osptrp,dsq))
    					{
    					ic_ptcpy(osptrp,p17);
    					fr1=sds_distance(p15,p17)/sds_distance(p15,p16);
    					ospt[0]=p14[0]+(fr1*(p13[0]-p14[0]));
    					ospt[1]=p14[1]+(fr1*(p13[1]-p14[1]));
    					ospt[2]=p14[2]+(fr1*(p13[2]-p14[2]));
//							g_nSnappedType= IC_OSMODE_NEAR;
    					}
    				//check along the edges extending up along extrusion
    				gra_ucs2rp(p11,p15,view);
    				gra_ucs2rp(p1,p16,view);
    				ic_segdist(pickptrp,p15,p16,0,&fr1,p17);
    				if (SDS_osnap_chkdsq(pickptrp,p17,osptrp,dsq))
    					{
    					ic_ptcpy(osptrp,p17);
    					fr1=sds_distance(p15,p17)/sds_distance(p15,p16);
    					ospt[0]=p11[0]+(fr1*(p1[0]-p11[0]));
    					ospt[1]=p11[1]+(fr1*(p1[1]-p11[1]));
    					ospt[2]=p11[2]+(fr1*(p1[2]-p11[2]));
//							g_nSnappedType= IC_OSMODE_NEAR;
    					}
    				gra_ucs2rp(p12,p15,view);
    				gra_ucs2rp(p2,p16,view);
    				ic_segdist(pickptrp,p15,p16,0,&fr1,p17);
    				if (SDS_osnap_chkdsq(pickptrp,p17,osptrp,dsq))
    					{
    					ic_ptcpy(osptrp,p17);
    					fr1=sds_distance(p15,p17)/sds_distance(p15,p16);
    					ospt[0]=p12[0]+(fr1*(p2[0]-p12[0]));
    					ospt[1]=p12[1]+(fr1*(p2[1]-p12[1]));
    					ospt[2]=p12[2]+(fr1*(p2[2]-p12[2]));
//							g_nSnappedType= IC_OSMODE_NEAR;
    					}
    				gra_ucs2rp(p13,p15,view);
    				gra_ucs2rp(p3,p16,view);
    				ic_segdist(pickptrp,p15,p16,0,&fr1,p17);
    				if (SDS_osnap_chkdsq(pickptrp,p17,osptrp,dsq))
    					{
    					ic_ptcpy(osptrp,p17);
    					fr1=sds_distance(p15,p17)/sds_distance(p15,p16);
    					ospt[0]=p13[0]+(fr1*(p3[0]-p13[0]));
    					ospt[1]=p13[1]+(fr1*(p3[1]-p13[1]));
    					ospt[2]=p13[2]+(fr1*(p3[2]-p13[2]));
//							g_nSnappedType= IC_OSMODE_NEAR;
    					}
    				gra_ucs2rp(p14,p15,view);
    				gra_ucs2rp(p4,p16,view);
    				ic_segdist(pickptrp,p15,p16,0,&fr1,p17);
    				if (SDS_osnap_chkdsq(pickptrp,p17,osptrp,dsq))
    					{
    					ic_ptcpy(osptrp,p17);
    					fr1=sds_distance(p15,p17)/sds_distance(p15,p16);
    					ospt[0]=p14[0]+(fr1*(p4[0]-p14[0]));
    					ospt[1]=p14[1]+(fr1*(p4[1]-p14[1]));
    					ospt[2]=p14[2]+(fr1*(p4[2]-p14[2]));
//							g_nSnappedType= IC_OSMODE_NEAR;
    					}
    				}
    			}
    		else	//3DFACE
    			{
    			//3dfaces can't have thickness
    			if (!(fi2 & IC_FACE3D_FACE1INVIS))
    				{
    				gra_ucs2rp(p1,p5,view);
    				gra_ucs2rp(p2,p6,view);
    				ic_segdist(pickptrp,p5,p6,0,&fr1,p7);
    				if (SDS_osnap_chkdsq(pickptrp,p7,osptrp,dsq))
    					{
    					ic_ptcpy(osptrp,p7);
    					fr1=sds_distance(p5,p7)/sds_distance(p5,p6);
    					ospt[0]=p1[0]+fr1*(p2[0]-p1[0]);
    					ospt[1]=p1[1]+fr1*(p2[1]-p1[1]);
    					ospt[2]=p1[2]+fr1*(p2[2]-p1[2]);
//							g_nSnappedType= IC_OSMODE_NEAR;
    					}
    				}
    			if (!(fi2 & IC_FACE3D_FACE2INVIS))
    				{
    				gra_ucs2rp(p2,p5,view);
    				gra_ucs2rp(p3,p6,view);
    				ic_segdist(pickptrp,p5,p6,0,&fr1,p7);
    				if (SDS_osnap_chkdsq(pickptrp,p7,osptrp,dsq))
    					{
    					ic_ptcpy(osptrp,p7);
    					fr1=sds_distance(p5,p7)/sds_distance(p5,p6);
    					ospt[0]=p2[0]+fr1*(p3[0]-p2[0]);
    					ospt[1]=p2[1]+fr1*(p3[1]-p2[1]);
    					ospt[2]=p2[2]+fr1*(p3[2]-p2[2]);
//							g_nSnappedType= IC_OSMODE_NEAR;
    					}
    				}
    			if (!(fi2 & IC_FACE3D_FACE3INVIS))
    				{
    				gra_ucs2rp(p3,p5,view);
    				gra_ucs2rp(p4,p6,view);
    				ic_segdist(pickptrp,p5,p6,0,&fr1,p7);
    				if (SDS_osnap_chkdsq(pickptrp,p7,osptrp,dsq))
    					{
    					ic_ptcpy(osptrp,p7);
    					fr1=sds_distance(p5,p7)/sds_distance(p5,p6);
    					ospt[0]=p3[0]+fr1*(p4[0]-p3[0]);
    					ospt[1]=p3[1]+fr1*(p4[1]-p3[1]);
    					ospt[2]=p3[2]+fr1*(p4[2]-p3[2]);
//							g_nSnappedType= IC_OSMODE_NEAR;
    					}
    				}
    			if (!(fi2 & IC_FACE3D_FACE4INVIS))
    				{
    				gra_ucs2rp(p4,p5,view);
    				gra_ucs2rp(p1,p6,view);
    				ic_segdist(pickptrp,p5,p6,0,&fr1,p7);
    				if (SDS_osnap_chkdsq(pickptrp,p7,osptrp,dsq))
    					{
    					ic_ptcpy(osptrp,p7);
    					fr1=sds_distance(p5,p7)/sds_distance(p5,p6);
    					ospt[0]=p4[0]+fr1*(p1[0]-p4[0]);
    					ospt[1]=p4[1]+fr1*(p1[1]-p4[1]);
    					ospt[2]=p4[2]+fr1*(p1[2]-p4[2]);
//							g_nSnappedType= IC_OSMODE_NEAR;
    					}
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
    				gra_ucs2rp(p7,p6,view);
    				if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
    					{
    					ic_ptcpy(ospt,p7);
    					ic_ptcpy(osptrp,p6);
//							g_nSnappedType= IC_OSMODE_PERP;
    					}
    				}
    			if (-1!=ic_ptlndist(p5,p1,p3,&fr1,p7))
    				{
    				gra_ucs2rp(p7,p6,view);
    				if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
    					{
    					ic_ptcpy(ospt,p7);
    					ic_ptcpy(osptrp,p6);
//							g_nSnappedType= IC_OSMODE_PERP;
    					}
    				}
    			if (-1!=ic_ptlndist(p5,p4,p2,&fr1,p7))
    				{
    				gra_ucs2rp(p7,p6,view);
    				if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
    					{
    					ic_ptcpy(ospt,p7);
    					ic_ptcpy(osptrp,p6);
//							g_nSnappedType= IC_OSMODE_PERP;
    					}
    				}
    			if (-1!=ic_ptlndist(p5,p4,p3,&fr1,p7))
    				{
    				gra_ucs2rp(p7,p6,view);
    				if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
    					{
    					ic_ptcpy(ospt,p7);
    					ic_ptcpy(osptrp,p6);
//							g_nSnappedType= IC_OSMODE_PERP;
    					}
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
    					gra_ucs2rp(p5,p6,view);
    					if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
    					{
    						ic_ptcpy(ospt,p5);
    						ic_ptcpy(osptrp,p6);
//						g_nSnappedType= IC_OSMODE_PERP;
    					}
    				}
       				if (!(fi2 & IC_FACE3D_FACE2INVIS) && -1!=ic_ptlndist(lastpt/*pickptucs*/,p2,p3,&fr1,p5))
    				{
    					//p5[2]=p1[2]+(p3[2]-p2[2])*sds_dist2d(p2,p5)/sds_dist2d(p2,p3);
    					gra_ucs2rp(p5,p6,view);
    					if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
    					{
    						ic_ptcpy(ospt,p5);
    						ic_ptcpy(osptrp,p6);
//						g_nSnappedType= IC_OSMODE_PERP;
   					}
    				}
    				if (!(fi2 & IC_FACE3D_FACE3INVIS) && -1!=ic_ptlndist(lastpt/*pickptucs*/,p3,p4,&fr1,p5))
    				{
    					//p5[2]=p1[3]+(p3[4]-p2[3])*sds_dist2d(p3,p5)/sds_dist2d(p3,p4);
    					gra_ucs2rp(p5,p6,view);
    					if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
    					{
    						ic_ptcpy(ospt,p5);
    						ic_ptcpy(osptrp,p6);
//						g_nSnappedType= IC_OSMODE_PERP;
    					}
    				}
    				if (!(fi2 & IC_FACE3D_FACE4INVIS) && -1!=ic_ptlndist(lastpt/*pickptucs*/,p4,p1,&fr1,p5))
    				{
    					//p5[2]=p4[2]+(p1[2]-p4[2])*sds_dist2d(p4,p5)/sds_dist2d(p4,p1);
    					gra_ucs2rp(p5,p6,view);
    					if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
    					{
    						ic_ptcpy(ospt,p5);
    						ic_ptcpy(osptrp,p6);
//						g_nSnappedType= IC_OSMODE_PERP;
    					}
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
    					gra_ucs2rp(p17,p16,view);
    					if (SDS_osnap_chkdsq(pickptrp,p16,osptrp,dsq))
    						{
    						ic_ptcpy(ospt,p17);
    						ic_ptcpy(osptrp,p16);
//								g_nSnappedType= IC_OSMODE_PERP;
    						}
    					}
    				if (-1!=ic_ptlndist(p15,p11,p13,&fr1,p16))
    					{
    					gra_ucs2rp(p17,p16,view);
    					if (SDS_osnap_chkdsq(pickptrp,p16,osptrp,dsq))
    						{
    						ic_ptcpy(ospt,p17);
    						ic_ptcpy(osptrp,p16);
//								g_nSnappedType= IC_OSMODE_PERP;
    						}
    					}
    				if (-1!=ic_ptlndist(p15,p14,p12,&fr1,p16))
    					{
    					gra_ucs2rp(p17,p16,view);
    					if (SDS_osnap_chkdsq(pickptrp,p16,osptrp,dsq))
    						{
    						ic_ptcpy(ospt,p17);
    						ic_ptcpy(osptrp,p16);
//								g_nSnappedType= IC_OSMODE_PERP;
    						}
    					}
    				if (-1!=ic_ptlndist(p15,p14,p13,&fr1,p16))
    					{
    					gra_ucs2rp(p17,p16,view);
    					if (SDS_osnap_chkdsq(pickptrp,p16,osptrp,dsq))
    						{
    						ic_ptcpy(ospt,p17);
    						ic_ptcpy(osptrp,p16);
//								g_nSnappedType= IC_OSMODE_PERP;
    						}
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
    			gra_ucs2rp(p5,p6,view);
    			if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
    				{
    				ic_ptcpy(ospt,p5);
    				ic_ptcpy(osptrp,p6);
//						g_nSnappedType= IC_OSMODE_MID;
    				}
    			}
    		if (!strsame(etype,db_hitype2str(DB_3DFACE)))//1-3
    			{
    			p5[0]=(p1[0]+p3[0])/2.0;
    			p5[1]=(p1[1]+p3[1])/2.0;
    			p5[2]=(p1[2]+p3[2])/2.0;
    			gra_ucs2rp(p5,p6,view);
    			if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
    				{
    				ic_ptcpy(ospt,p5);
    				ic_ptcpy(osptrp,p6);
//						g_nSnappedType= IC_OSMODE_MID;
    				}
    			}
    		else if (!(fi2 & IC_FACE3D_FACE2INVIS))			  //2-3
    			{
    			p5[0]=(p2[0]+p3[0])/2.0;
    			p5[1]=(p2[1]+p3[1])/2.0;
    			p5[2]=(p2[2]+p3[2])/2.0;
    			gra_ucs2rp(p5,p6,view);
    			if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
    				{
    				ic_ptcpy(ospt,p5);
    				ic_ptcpy(osptrp,p6);
//						g_nSnappedType= IC_OSMODE_MID;
    				}
    			}
    		if (!(fi2 & IC_FACE3D_FACE3INVIS))				  //3-4
    			{
    			p5[0]=(p4[0]+p3[0])/2.0;
    			p5[1]=(p4[1]+p3[1])/2.0;
    			p5[2]=(p4[2]+p3[2])/2.0;
    			gra_ucs2rp(p5,p6,view);
    			if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
    				{
    				ic_ptcpy(ospt,p5);
    				ic_ptcpy(osptrp,p6);
//						g_nSnappedType= IC_OSMODE_MID;
    				}
    			}
    		if (!strsame(etype,db_hitype2str(DB_3DFACE)))//2-4
    			{
    			p5[0]=(p4[0]+p2[0])/2.0;
    			p5[1]=(p4[1]+p2[1])/2.0;
    			p5[2]=(p4[2]+p2[2])/2.0;
    			gra_ucs2rp(p5,p6,view);
    			if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
    				{
    				ic_ptcpy(ospt,p5);
    				ic_ptcpy(osptrp,p6);
//						g_nSnappedType= IC_OSMODE_MID;
    				}
    			}
    		else if (!(fi2 & IC_FACE3D_FACE4INVIS))			  //1-4
    			{
    			p5[0]=(p4[0]+p1[0])/2.0;
    			p5[1]=(p4[1]+p1[1])/2.0;
    			p5[2]=(p4[2]+p1[2])/2.0;
    			gra_ucs2rp(p5,p6,view);
    			if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
    				{
    				ic_ptcpy(ospt,p5);
    				ic_ptcpy(osptrp,p6);
//						g_nSnappedType= IC_OSMODE_MID;
    				}
    			}
    		if (thickflg)
    			{
                if (!(fi2 & IC_FACE3D_FACE1INVIS))				  //1-2
    				{
    				p15[0]=(p11[0]+p12[0])/2.0;
    				p15[1]=(p11[1]+p12[1])/2.0;
    				p15[2]=(p11[2]+p12[2])/2.0;
    				gra_ucs2rp(p15,p16,view);
    				if (SDS_osnap_chkdsq(pickptrp,p16,osptrp,dsq))
    					{
    					ic_ptcpy(ospt,p15);
    					ic_ptcpy(osptrp,p16);
//							g_nSnappedType= IC_OSMODE_MID;
    					}
    				}
    			if (!strsame(etype,db_hitype2str(DB_3DFACE))) //1-3
    				{
    				p15[0]=(p11[0]+p13[0])/2.0;
    				p15[1]=(p11[1]+p13[1])/2.0;
    				p15[2]=(p11[2]+p13[2])/2.0;
    				gra_ucs2rp(p15,p16,view);
    				if (SDS_osnap_chkdsq(pickptrp,p16,osptrp,dsq))
    					{
    					ic_ptcpy(ospt,p15);
    					ic_ptcpy(osptrp,p16);
//							g_nSnappedType= IC_OSMODE_MID;
    					}
    				}
    			else if (!(fi2 & IC_FACE3D_FACE2INVIS))			  //2-3
    				{
    				p15[0]=(p12[0]+p13[0])/2.0;
    				p15[1]=(p12[1]+p13[1])/2.0;
    				p15[2]=(p12[2]+p13[2])/2.0;
    				gra_ucs2rp(p15,p16,view);
    				if (SDS_osnap_chkdsq(pickptrp,p16,osptrp,dsq))
    					{
    					ic_ptcpy(ospt,p15);
    					ic_ptcpy(osptrp,p16);
//							g_nSnappedType= IC_OSMODE_MID;
    					}
    				}
    			if (!(fi2 & IC_FACE3D_FACE3INVIS))				  //3-4
    				{
    				p15[0]=(p14[0]+p13[0])/2.0;
    				p15[1]=(p14[1]+p13[1])/2.0;
    				p15[2]=(p14[2]+p13[2])/2.0;
    				gra_ucs2rp(p15,p16,view);
    				if (SDS_osnap_chkdsq(pickptrp,p16,osptrp,dsq))
    					{
    					ic_ptcpy(ospt,p15);
    					ic_ptcpy(osptrp,p16);
//							g_nSnappedType= IC_OSMODE_MID;
    					}
    				}
    			if (!strsame(etype,db_hitype2str(DB_3DFACE)))//2-4
    				{
    				p15[0]=(p14[0]+p12[0])/2.0;
    				p15[1]=(p14[1]+p12[1])/2.0;
    				p15[2]=(p14[2]+p12[2])/2.0;
    				gra_ucs2rp(p15,p16,view);
    				if (SDS_osnap_chkdsq(pickptrp,p16,osptrp,dsq))
    					{
    					ic_ptcpy(ospt,p15);
    					ic_ptcpy(osptrp,p16);
//							g_nSnappedType= IC_OSMODE_MID;
    					}
    				}
    			else if (!(fi2 & IC_FACE3D_FACE4INVIS))			  //1-4
    				{
    				p15[0]=(p14[0]+p11[0])/2.0;
    				p15[1]=(p14[1]+p11[1])/2.0;
    				p15[2]=(p14[2]+p11[2])/2.0;
    				gra_ucs2rp(p15,p16,view);
    				if (SDS_osnap_chkdsq(pickptrp,p16,osptrp,dsq))
    					{
    					ic_ptcpy(ospt,p15);
    					ic_ptcpy(osptrp,p16);
//							g_nSnappedType= IC_OSMODE_MID;
    					}
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
    		gra_ucs2rp(p5,p6,view);
			if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq)){ic_ptcpy(ospt,p5);ic_ptcpy(osptrp,p6);}
//				if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
//					{
//						ic_ptcpy(ospt,p5);
//						ic_ptcpy(osptrp,p6);
//						g_nSnappedType= IC_OSMODE_INS;
//					}
    		}

    	if (osmode & IC_OSMODE_CENTER)
    		{//CEN of solid or trace
    		p5[0]=(p1[0]+p2[0]+p3[0]+p4[0])/4.0;
    		p5[1]=(p1[1]+p2[1]+p3[1]+p4[1])/4.0;
    		p5[2]=(p1[2]+p2[2]+p3[2]+p4[2])/4.0;
    		gra_ucs2rp(p5,p6,view);
			if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq)){ic_ptcpy(ospt,p5);ic_ptcpy(osptrp,p6);}
//				if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
//					{
//						ic_ptcpy(ospt,p5);
//						ic_ptcpy(osptrp,p6);
//						g_nSnappedType= IC_OSMODE_CENTER;
//					}
    		}

    	if (osmode & (IC_OSMODE_END | IC_OSMODE_INT | IC_OSMODE_APP | IC_OSMODE_PLAN))
    		{	//INT modes or END
    		gra_ucs2rp(p1,p5,view);
			if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq)){ic_ptcpy(ospt,p1);ic_ptcpy(osptrp,p5);}
//				if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
//					{
//						ic_ptcpy(ospt,p1);
//						ic_ptcpy(osptrp,p5);
//						if (osmode & IC_OSMODE_END )
//							g_nSnappedType= IC_OSMODE_END;//according to snap priority
//						else
//							g_nSnappedType= IC_OSMODE_INT;
//					}
    		gra_ucs2rp(p2,p5,view);
			if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq)){ic_ptcpy(ospt,p2);ic_ptcpy(osptrp,p5);}
//				if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
//					{
//						ic_ptcpy(ospt,p2);
//						ic_ptcpy(osptrp,p5);
//						if (osmode & IC_OSMODE_END )
//							g_nSnappedType= IC_OSMODE_END;//according to snap priority
//						else
//							g_nSnappedType= IC_OSMODE_INT;
//					}
    		gra_ucs2rp(p3,p5,view);
			if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq)){ic_ptcpy(ospt,p3);ic_ptcpy(osptrp,p5);}
//				if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
//					{
//						ic_ptcpy(ospt,p3);
//						ic_ptcpy(osptrp,p5);
//						if (osmode & IC_OSMODE_END )
//							g_nSnappedType= IC_OSMODE_END;//according to snap priority
//						else
//							g_nSnappedType= IC_OSMODE_INT;
//					}
    		gra_ucs2rp(p4,p5,view);
			if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq)){ic_ptcpy(ospt,p4);ic_ptcpy(osptrp,p5);}
//				if (SDS_osnap_chkdsq(pickptrp,p5,osptrp,dsq))
//					{
//						ic_ptcpy(ospt,p4);
//						ic_ptcpy(osptrp,p5);
//						if (osmode & IC_OSMODE_END )
//							g_nSnappedType= IC_OSMODE_END;//according to snap priority
//						else
//							g_nSnappedType= IC_OSMODE_INT;
//					}
    		}

    	if (osmode & IC_OSMODE_NEAR)
    		{//NEA	//NOTE: all nea calcs depend upon view, so they must be done in RP!
    		gra_ucs2rp(p1,p5,view);
    		gra_ucs2rp(p2,p6,view);
    		ic_segdist(pickptrp,p5,p6,0,&fr1,p7);
    		if (SDS_osnap_chkdsq(pickptrp,p7,osptrp,dsq))
    			{
    			ic_ptcpy(osptrp,p7);
    			fr1=sds_distance(p5,p7)/sds_distance(p5,p6);
    			ospt[0]=p1[0]+fr1*(p2[0]-p1[0]);
    			ospt[1]=p1[1]+fr1*(p2[1]-p1[1]);
    			ospt[2]=p1[2]+fr1*(p2[2]-p1[2]);
//					g_nSnappedType= IC_OSMODE_NEAR;
    			}

    		gra_ucs2rp(p2,p5,view);
    		gra_ucs2rp(p3,p6,view);
    		ic_segdist(pickptrp,p5,p6,0,&fr1,p7);
    		if (SDS_osnap_chkdsq(pickptrp,p7,osptrp,dsq))
    			{
    			ic_ptcpy(osptrp,p7);
    			fr1=sds_distance(p5,p7)/sds_distance(p5,p6);
    			ospt[0]=p2[0]+fr1*(p3[0]-p2[0]);
    			ospt[1]=p2[1]+fr1*(p3[1]-p2[1]);
    			ospt[2]=p2[2]+fr1*(p3[2]-p2[2]);
//					g_nSnappedType= IC_OSMODE_NEAR;
    			}

    		gra_ucs2rp(p3,p5,view);
    		gra_ucs2rp(p4,p6,view);
    		ic_segdist(pickptrp,p5,p6,0,&fr1,p7);
    		if (SDS_osnap_chkdsq(pickptrp,p7,osptrp,dsq))
    			{
    			ic_ptcpy(osptrp,p7);
    			fr1=sds_distance(p5,p7)/sds_distance(p5,p6);
    			ospt[0]=p3[0]+fr1*(p4[0]-p3[0]);
    			ospt[1]=p3[1]+fr1*(p4[1]-p3[1]);
    			ospt[2]=p3[2]+fr1*(p4[2]-p3[2]);
//					g_nSnappedType= IC_OSMODE_NEAR;
    			}

    		gra_ucs2rp(p4,p5,view);
    		gra_ucs2rp(p1,p6,view);
    		ic_segdist(pickptrp,p5,p6,0,&fr1,p7);
    		if (SDS_osnap_chkdsq(pickptrp,p7,osptrp,dsq))
    			{
    			ic_ptcpy(osptrp,p7);
    			fr1=sds_distance(p5,p7)/sds_distance(p5,p6);
    			ospt[0]=p4[0]+fr1*(p1[0]-p4[0]);
    			ospt[1]=p4[1]+fr1*(p1[1]-p4[1]);
    			ospt[2]=p4[2]+fr1*(p1[2]-p4[2]);
//					g_nSnappedType= IC_OSMODE_NEAR;
    			}
    		}

    	if (osmode & IC_OSMODE_PERP)
    		{//PER
    		if ( -1!=ic_ptlndist(lastpt,p1,p2,&fr1,p5))
    			{
    			gra_ucs2rp(p5,p6,view);
    			if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
    				{
    				ic_ptcpy(ospt,p5);
    				ic_ptcpy(osptrp,p6);
//						g_nSnappedType= IC_OSMODE_PERP;
    				}
    			}
    		if (-1!=ic_ptlndist(lastpt,p2,p3,&fr1,p5))
    			{
    			gra_ucs2rp(p5,p6,view);
    			if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
    				{
    				ic_ptcpy(ospt,p5);
    				ic_ptcpy(osptrp,p6);
//						g_nSnappedType= IC_OSMODE_PERP;
    				}
    			}
    		if (-1!=ic_ptlndist(lastpt,p3,p4,&fr1,p5))
    			{
    			gra_ucs2rp(p5,p6,view);
    			if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
    				{
    				ic_ptcpy(ospt,p5);
    				ic_ptcpy(osptrp,p6);
//						g_nSnappedType= IC_OSMODE_PERP;
    				}
    			}
    		if (-1!=ic_ptlndist(lastpt,p4,p1,&fr1,p5))
    			{
    			gra_ucs2rp(p5,p6,view);
    			if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
    				{
    				ic_ptcpy(ospt,p5);
    				ic_ptcpy(osptrp,p6);
//						g_nSnappedType= IC_OSMODE_PERP;
    				}
    			}
    		}

    	if (osmode & IC_OSMODE_MID)
    		{//MID
    		p5[0]=(p1[0]+p2[0])/2.0;
    		p5[1]=(p1[1]+p2[1])/2.0;
    		p5[2]=(p1[2]+p2[2])/2.0;
    		gra_ucs2rp(p5,p6,view);
    		if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
    			{
    			ic_ptcpy(ospt,p5);
    			ic_ptcpy(osptrp,p6);
//					g_nSnappedType= IC_OSMODE_MID;
    			}

    		p5[0]=(p2[0]+p3[0])/2.0;
    		p5[1]=(p2[1]+p3[1])/2.0;
    		p5[2]=(p2[2]+p3[2])/2.0;
    		gra_ucs2rp(p5,p6,view);
    		if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
    			{
    			ic_ptcpy(ospt,p5);
    			ic_ptcpy(osptrp,p6);
//					g_nSnappedType= IC_OSMODE_MID;
    			}

    		p5[0]=(p4[0]+p3[0])/2.0;
    		p5[1]=(p4[1]+p3[1])/2.0;
    		p5[2]=(p4[2]+p3[2])/2.0;
    		gra_ucs2rp(p5,p6,view);
    		if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
    			{
    			ic_ptcpy(ospt,p5);
    			ic_ptcpy(osptrp,p6);
//					g_nSnappedType= IC_OSMODE_MID;
    			}

    		p5[0]=(p4[0]+p1[0])/2.0;
    		p5[1]=(p4[1]+p1[1])/2.0;
    		p5[2]=(p4[2]+p1[2])/2.0;
    		gra_ucs2rp(p5,p6,view);
    		if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
    			{
    			ic_ptcpy(ospt,p5);
    			ic_ptcpy(osptrp,p6);
//					g_nSnappedType= IC_OSMODE_MID;
    			}
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
    		gra_ucs2rp(p5,p6,view);
    		if (SDS_osnap_chkdsq(pickptrp,p6,osptrp,dsq))
    			{
    			ic_ptcpy(ospt,p5);
    			ic_ptcpy(osptrp,p6);
//					g_nSnappedType= IC_OSMODE_INS;
    			}
            if (thickflg)
    			{
    			p15[0]+=thickness*p210[0];
    			p15[1]+=thickness*p210[1];
    			p15[2]+=thickness*p210[2];
    			if (m_ecs2ucs)
    				ic_usemat(p15,p15,m_ecs2ucs,0,0,0);
    			else
    				sds_trans(p15,&rbent,&rbucs,0,p15);
    			gra_ucs2rp(p15,p16,view);
				if (SDS_osnap_chkdsq(pickptrp,p16,osptrp,dsq)){ic_ptcpy(ospt,p15);ic_ptcpy(osptrp,p16);}
//				    if (SDS_osnap_chkdsq(pickptrp,p16,osptrp,dsq))
//						{
//							ic_ptcpy(ospt,p15);
//							ic_ptcpy(osptrp,p16);
//							g_nSnappedType= IC_OSMODE_INS;
//						}
    	        }
    		}
    	}

    exit_point:
    return(ret);
}

int SDS_osnap_chkdsq(sds_point pickpt, sds_point chkpt, sds_point ospt, sds_real *dsq)
{
    //NOTE:  ALL pts MUST be in RP coordinates.
    //RETURNS: 1 if chkpt closer to pickpt than current ospt, 0 otherwise
    sds_real fr1;
    fr1=(chkpt[0]-pickpt[0])*(chkpt[0]-pickpt[0])+
    	(chkpt[1]-pickpt[1])*(chkpt[1]-pickpt[1]);//IGNORE THE Z!
    if ((*dsq<0.0)||(fr1<*dsq)){
    	*dsq=fr1;
    	return(1);
    }else
    	return(0);
}

void SDS_SetOsnapCursor(char *snapstr,int *osnapmode,bool readvar) 
{
    struct resbuf rb;

    // Check for osnap strings.
    *snapstr=0;
    if (readvar) {
    	SDS_getvar(NULL,DB_QOSMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
    	(*osnapmode)=rb.resval.rint;
    }

    if (!(*osnapmode)) return;
//4M Item:77
//Osnap cursor should not be displayed when osnap is OFF
  if ((*osnapmode) & IC_OSMODE_OFF)
     return;
//<-4M Item:77
    if ((*osnapmode) & IC_OSMODE_END) {
    	if (*snapstr) strcat(snapstr,","/*DNT*/);
    	strcat(snapstr,ResourceString(IDC_SDS_OSNAP_END_0, "END" ));
    	SDS_SetCursor(IDC_ENDPOINT);
    }
    if ((*osnapmode) & IC_OSMODE_MID) {
    	if (*snapstr) strcat(snapstr,","/*DNT*/);
    	strcat(snapstr,ResourceString(IDC_SDS_OSNAP_MID_1, "MID" ));
    	SDS_SetCursor(IDC_MIDPOINT);
    }
    if ((*osnapmode) & IC_OSMODE_CENTER) {
    	if (*snapstr) strcat(snapstr,","/*DNT*/);
    	strcat(snapstr,ResourceString(IDC_SDS_OSNAP_CEN_2, "CEN" ));
    	SDS_SetCursor(IDC_CENTER);
    }
    if ((*osnapmode) & IC_OSMODE_NODE) {
    	if (*snapstr) strcat(snapstr,","/*DNT*/);
    	strcat(snapstr,ResourceString(IDC_SDS_OSNAP_NOD_3, "NOD" ));
    	SDS_SetCursor(IDC_NODE);
    }
    if ((*osnapmode) & IC_OSMODE_QUADRANT) {
    	if (*snapstr) strcat(snapstr,","/*DNT*/);
    	strcat(snapstr,ResourceString(IDC_SDS_OSNAP_QUA_5, "QUA" ));
    	SDS_SetCursor(IDC_QUAD);
    }
    if ((*osnapmode) & IC_OSMODE_INT) {
    	if (*snapstr) strcat(snapstr,","/*DNT*/);
    	strcat(snapstr,ResourceString(IDC_SDS_OSNAP_INT_6, "INT" ));
    	SDS_SetCursor(IDC_ICAD_OS_INT3D);
    }
    if ((*osnapmode) & IC_OSMODE_INS) {
    	if (*snapstr) strcat(snapstr,","/*DNT*/);
    	strcat(snapstr,ResourceString(IDC_SDS_OSNAP_INS_7, "INS" ));
    	SDS_SetCursor(IDC_INSERT);
    }
    if ((*osnapmode) & IC_OSMODE_PERP) {
    	if (*snapstr) strcat(snapstr,","/*DNT*/);
    	strcat(snapstr,ResourceString(IDC_SDS_OSNAP_PER_8, "PER" ));
    	SDS_SetCursor(IDC_PERPEN);
    }
    if ((*osnapmode) & IC_OSMODE_TAN) {
    	if (*snapstr) strcat(snapstr,","/*DNT*/);
    	strcat(snapstr,ResourceString(IDC_SDS_OSNAP_TAN_9, "TAN" ));
    	SDS_SetCursor(IDC_TANGENT);
    }
    if ((*osnapmode) & IC_OSMODE_NEAR) {
    	if (*snapstr) strcat(snapstr,","/*DNT*/);
    	strcat(snapstr,"NEA"/*DNT*/);
    	SDS_SetCursor(IDC_NEAR);
    }
    if ((*osnapmode) & IC_OSMODE_QUICK) {
    	if (*snapstr) strcat(snapstr,","/*DNT*/);
    	strcat(snapstr,ResourceString(IDC_SDS_OSNAP_QUI_11, "QUI" ));
    	SDS_SetCursor(IDC_ICAD_SNAP);
    }
    if ((*osnapmode) & IC_OSMODE_APP) {
    	if (*snapstr) strcat(snapstr,","/*DNT*/);
    	strcat(snapstr,ResourceString(IDC_SDS_OSNAP_VIS_13, "VIS" ));
    	SDS_SetCursor(IDC_ICAD_OS_INTVS);
    }
    if ((*osnapmode) & IC_OSMODE_PLAN) {
    	if (*snapstr) strcat(snapstr,","/*DNT*/);
    	strcat(snapstr,ResourceString(IDC_SDS_OSNAP_PLA_14, "PLA" ));
    	SDS_SetCursor(IDC_ICAD_OS_INT2D);
    }

    if (strchr(snapstr,',')) SDS_SetCursor(IDC_ICAD_SNAP);
}


