/* F:\DEV\PRJ\LIB\CMDS\PEDIT.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */
// THIS FILE HAS BEEN GLOBALIZED!
#include "cmds.h"/*DNT*/
#include "Icad.h"/*DNT*/
#include "splines.h"/*DNT*/
#include "IcadApi.h"
#include "IcadView.h"//IntelliKorea add. 2001/3/4
#include "gvector.h"
#include "transf.h"
#include "CmdQueryTools.h"
using namespace icl;

// Globals

//** Structures
struct cmd_elistll_ptr {			//this struct will point to main entity's elistll ptr
	struct cmd_elistll *elistll;
	struct cmd_elistll_ptr *next;
};

// Protos
int cmd_reset_plwidth(sds_name pent,sds_real width);
int cmd_reverse_pline(sds_name pline,sds_name vtxent);
int cmd_taper_pline(sds_name pent, sds_real stwidth, sds_real endwidth);
int cmd_join_pline(sds_name polyent, sds_name lwpolyent, sds_name ss);
int cmd_pline_editvtx(sds_name polyent);
int cmd_join_seg2start(sds_name ent, sds_point newstart, sds_real bulge);
int cmd_join_seg2end(sds_name ent, sds_point newend, sds_real bulge);
int cmd_join_plend2start(sds_name ent1, sds_name ent2, long *addct, int header2use);
int cmd_join_plines2close(sds_name ent1,sds_name ent2,long *addct);
int cmd_pedit_drawst(sds_point drawpt,int mode, sds_point extru);

bool PolylineCanBeLwpolyline(sds_name pent)
	{
	db_handitem *plptr=(db_handitem *)pent[0];
	int flags;

	flags=((db_polyline *)plptr)->ret_70();
	if ((flags & IC_PLINE_CURVEFIT) ||
	    (flags & IC_PLINE_SPLINEFIT) ||
	    (flags & IC_PLINE_3DPLINE) ||
	    (flags & IC_PLINE_3DMESH) ||
	    (flags & IC_PLINE_POLYFACEMESH))
		return false;				// can't convert any of these

	plptr=plptr->next;
	while (plptr->next->next!=NULL) // while we're on the verts
		{
		if (plptr->ret_eed()!=NULL) // can't convert if eed on verts
			return false;
									// shd also search for added tangent direction
									// here, but ICAD does not currently allow it
		plptr=plptr->next;
		}

	return true;
	}

short AddPlineDataToLwpolylineElist(resbuf *lwplist, db_handitem *origplptr)
	{
	resbuf *trb;
	int vertcount,i;
	db_handitem *plptr=origplptr;
	sds_point temppt;
	short holdflags;

	plptr->get_10(temppt);
	vertcount=-1;  // deduct pline entity from count
	while (plptr->ret_type()!=DB_SEQEND)
		{
		vertcount++;
		plptr=plptr->next;
		}

	trb=lwplist;
	while (trb->rbnext!=NULL)
		{
		if (trb->restype==70)
			holdflags=trb->resval.rint;
		// walk to the end
		trb=trb->rbnext;
		}
	trb->rbnext=sds_buildlist(38, temppt[2], 0);
	trb=trb->rbnext;  // step down one more onto the 38 group

	trb->rbnext=sds_buildlist(90, vertcount, 0);
	trb=trb->rbnext;  // step down one more onto the 90 group

	trb->rbnext=sds_buildlist(70, holdflags, 0);
	trb=trb->rbnext;  // step down one more onto the 70 group

	plptr=origplptr->next;	// point to first vertex

	for (i=0; i<vertcount; i++)
		{
		sds_point temppt;
		sds_real bulge, stwid,endwid;
		((db_vertex *)plptr)->get_10(temppt);
		((db_vertex *)plptr)->get_40(&stwid);
		((db_vertex *)plptr)->get_41(&endwid);
		((db_vertex *)plptr)->get_42(&bulge);
		trb->rbnext=sds_buildlist(10, temppt,40,stwid,41,endwid,42,bulge,0);
		while (trb->rbnext!=NULL)	// walk to end again
			trb=trb->rbnext;
		plptr=plptr->next;
		}
	return RTNORM;
	}

short PutPolylineDataOnExistingLwpolyline(sds_name lwpent,sds_name pent)
	{
	resbuf *lwplist,*nameptr;

	lwplist=sds_entget(lwpent);
	nameptr=ic_ret_assoc(lwplist,0);
	sds_relrb(nameptr->rbnext);		// chop off the end
	nameptr->rbnext=NULL;

	AddPlineDataToLwpolylineElist(lwplist, (db_handitem *)pent[0]);
	resbuf *rb=ic_ret_assoc(lwplist,70);
	rb->resval.rint=((db_polyline *)pent[0])->ret_70();
	int ret=sds_entmod(lwplist);
	sds_redraw(lwpent,IC_REDRAW_DRAW);
	sds_relrb(lwplist);
	return RTNORM;
	}

short ConvertLwpolylineToPolyline(sds_name lwpent, sds_name pent)
	{
	resbuf *pllist,*lwplist,applist,*tmprb,*delrb,*templatelist;
	sds_point elevpt;
	db_lwpolyline *lwphip=(db_lwpolyline *)lwpent[0];
	short i;
	sds_real stwid,endwid,bulge;

// get the lwpoly's resbuf list
	applist.restype=RTSTR;
	applist.resval.rstring="*";
	lwplist=sds_entgetx(lwpent,&applist);
// now filter out all of the lwpoly-specific resbufs
	tmprb=lwplist;
// we assume the first one is one we can keep
	while (tmprb->rbnext)
		{
		if (tmprb->rbnext->restype==90 ||
			tmprb->rbnext->restype==38 ||
			tmprb->rbnext->restype==40 ||
			tmprb->rbnext->restype==41 ||
			tmprb->rbnext->restype==42)
			{
			delrb=tmprb->rbnext;
			tmprb->rbnext=tmprb->rbnext->rbnext;
			delrb->rbnext=NULL;
			sds_relrb(delrb);
			}
		else
			tmprb=tmprb->rbnext;
		}

	templatelist=SDS_ResBufCopy(lwplist);  // keep a copy for other entmakes
	sds_entmake(NULL);
	elevpt[0]=elevpt[1]=0.0;
	elevpt[2]=lwphip->ret_38();
	tmprb->rbnext=sds_buildlist(66,1,10,elevpt,0);  // add to the end
	tmprb=ic_ret_assoc(lwplist,0);
	strcpy(tmprb->resval.rstring,"POLYLINE"/*DNT*/); // "POLYLINE" is shorter, so it will fit over "LWPOLYLINE"

	if (sds_entmake(lwplist)!=RTNORM)				// make the polyline entity
		return RTERROR;
	sds_relrb(lwplist);

								// Now we need to filter out any EED in the
								// template since the vertices should 
								// not include the EED
								// Note: As in the loop above we assume that 
								// the first item on the list should be kept so
								// we don't have to worry about keeping a
								// trailing pointer to tidy up the links
	tmprb = templatelist;
	while (tmprb->rbnext)
	{
		if (tmprb->rbnext->restype == -3 ||
			(tmprb->rbnext->restype >= 1000 &&
			 tmprb->rbnext->restype <= 1071))
		{
			delrb=tmprb->rbnext;
			tmprb->rbnext=tmprb->rbnext->rbnext;
			delrb->rbnext=NULL;
			sds_relrb(delrb);
		}
		else
			tmprb=tmprb->rbnext;
	}
	
	for (i=0; i<lwphip->ret_90(); i++)
		{
		sds_point vertpt;
		lwphip->get_10(vertpt,i);
		lwphip->get_40(&stwid,i);
		lwphip->get_41(&endwid,i);
		lwphip->get_42(&bulge,i);
		tmprb=pllist=SDS_ResBufCopy(templatelist);
		while (tmprb->rbnext!=NULL)	// walk to the end
			tmprb=tmprb->rbnext;
		tmprb->rbnext=sds_buildlist(10,vertpt,40,stwid,
			41,endwid,42,bulge,70,0,0);
		tmprb=ic_ret_assoc(pllist,0);
		strcpy(tmprb->resval.rstring,"VERTEX"/*DNT*/); // "VERTEX" is shorter, so it will fit over "POLYLINE"
		if (sds_entmake(pllist)!=RTNORM)
			return RTERROR;
		sds_relrb(pllist);
		}

	pllist=SDS_ResBufCopy(templatelist);
	tmprb=ic_ret_assoc(pllist,0);
	strcpy(tmprb->resval.rstring,"SEQEND"/*DNT*/); // same length
	if (sds_entmake(pllist)!=RTNORM)
		return RTERROR;
	sds_relrb(pllist);

	sds_entlast(pent);
	sds_redraw(pent,IC_REDRAW_DRAW);
	sds_relrb(templatelist);

	return RTNORM;
	}

short ConvertPolylineToLwpolyline(sds_name pent, sds_name lwpent)
	{
	resbuf *pllist,applist,*tmprb,*delrb;
	sds_real lwpelev;
	int ret;

// get the pline's resbuf list
	applist.restype=RTSTR;
	applist.resval.rstring="*";
	pllist=sds_entgetx(pent,&applist);

	tmprb=ic_ret_assoc(pllist,10);
	lwpelev=tmprb->resval.rpoint[2];
// now filter out all of the pline-specific resbufs
	tmprb=pllist;
// we assume the first one is one we can keep
	while (tmprb->rbnext)
		{
		if (tmprb->rbnext->restype==71 ||
			tmprb->rbnext->restype==72 ||
			tmprb->rbnext->restype==73 ||
			tmprb->rbnext->restype==74 ||
			tmprb->rbnext->restype==75 ||
			tmprb->rbnext->restype==40 ||
			tmprb->rbnext->restype==41 ||
			tmprb->rbnext->restype==10)
			{
			delrb=tmprb->rbnext;
			tmprb->rbnext=tmprb->rbnext->rbnext;
			delrb->rbnext=NULL;
			sds_relrb(delrb);
			}
		else
			tmprb=tmprb->rbnext;
		}
	tmprb=ic_ret_assoc(pllist,0);
	IC_FREE(tmprb->resval.rstring);
	tmprb->resval.rstring= new char [11];
	strcpy(tmprb->resval.rstring,"LWPOLYLINE"/*DNT*/);
	AddPlineDataToLwpolylineElist(pllist, (db_handitem *)pent[0]);
	ret=sds_entmake(pllist);
	sds_entdel(pent);
	sds_entlast(lwpent);
	sds_redraw(lwpent,IC_REDRAW_DRAW);
	sds_relrb(pllist);
	return RTNORM;
	}


//** Functions
short cmd_pedit(void) 
	{

	/*NOTES:
	We DO allow join of 3dpoly to other entities - all the curves are taken
	out	of bulged sections or arcs added.  We also allow the user to convert
	a CIRCLE into a closed pline w/vertices at 3o'clock and 9o'clock.
	This command reports number of VERTICES added - acad reports number of
	SEGMENTS added.  We also have slightly different prompting - this
	command tells user to select entities in order of attempted joining(ACAD
	doesn't but works that way).  We also report back if we have closed
	or opened the pline (change isn't always visible).

	We can REVERSE the pline vertex ordering and TAPER all vertices for
	a uniform appearance.

	UNDO buffer	won't undo a join or a break (UNDO holds data for single entity)
	*/

    if (cmd_iswindowopen() == RTERROR)
		return RTERROR;

	if (cmd_istransparent() == RTERROR) return RTERROR;

    sds_point p0,p1,p2,p3,pt210;
    sds_name pent,etemp1,etemp2,ssjoin,lwpent;
	struct cmd_elistll *elist_llbeg, *elist_llcur;
	struct cmd_elistll_ptr *undo_beg, *undo_cur, *undo_temp;
	struct resbuf *elist, rb, rb2, *rbtemp;
	short ret2;
	RT ret=0;
 	int fi1,fi2,poly3d = 0,splinesegs,splinetype,fittype,redrawflag;
	long fl1;
	char fs1[IC_ACADBUF],fs2[IC_ACADBUF],etype[35];
    sds_real fr1,fr2,fr3,fr4,fr5,fr6,fr7,fr8;
	bool originalwaslwpolyline=false;
	bool wasPolyline=false;

	pent[0] = pent[1] = lwpent[0] = lwpent[1] = 0L;
	elist_llbeg=elist_llcur=NULL;
	undo_beg=undo_cur=undo_temp=NULL;
	elist=rbtemp=NULL;

	SDS_getvar(NULL,DB_QPLINETYPE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	int plinetype=rb.resval.rint;

	SDS_getvar(NULL,DB_QUCSXDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	ic_ptcpy(p0,rb.resval.rpoint);
	SDS_getvar(NULL,DB_QUCSYDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	pt210[0]=p0[1]*rb.resval.rpoint[2]-p0[2]*rb.resval.rpoint[1];
	pt210[1]=p0[2]*rb.resval.rpoint[0]-p0[0]*rb.resval.rpoint[2];
	pt210[2]=p0[0]*rb.resval.rpoint[1]-p0[1]*rb.resval.rpoint[0];

	SDS_getvar(NULL,DB_QSPLINESEGS,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	splinesegs=abs(rb.resval.rint);
	SDS_getvar(NULL,DB_QSPLINETYPE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	splinetype=rb.resval.rint;
	if (SDS_getvar(NULL,DB_QFITTYPE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
		fittype=2;
	else
		fittype=rb.resval.rint;

	for(;;)
		{
		FreeResbufIfNotNull(&elist);
		fi1=sds_pmtssget(ResourceString(IDC_PEDIT__NSELECT_POLYLINE__0, "\nSelect polyline to edit: " ),""/*DNT*/,&elist,NULL,NULL,ssjoin,1);
		FreeResbufIfNotNull(&rbtemp);
		if (fi1==RTNORM)
			{
			sds_sslength(ssjoin,&fl1);
			if (fl1==0L)
				{
				cmd_ErrorPrompt(CMD_ERR_SELECTION,0);
				sds_ssfree(ssjoin);
				continue;
				//can't continue.  ic_rbassoc hasn't been set for condition test
				}
			else if (fl1>1L)
				{
				for (fi1=1;sds_ssname(ssjoin,fi1,pent)==RTNORM;fi1++)
					{
					sds_redraw(pent,IC_REDRAW_UNHILITE);
					}
				sds_printf(ResourceString(IDC_PEDIT__NONLY_ONE_ENTITY__1, "\nOnly one entity may be edited." ));
				}
  			if (sds_ssname(ssjoin,0L,pent)!=RTNORM)
				{
				ret=-1;
				goto exit;
				}
			sds_ssfree(ssjoin);
			}
		else if (fi1==RTERROR)
			continue;	//if nothing picked, try again
		else goto exit;

		rb.restype=RTSTR;
		rb.rbnext=NULL;
		rb.resval.rstring=const_cast<char*>("*"/*DNT*/);
        FreeResbufIfNotNull(&elist);
        if ((elist=sds_entgetx(pent,&rb))==NULL) 
			{
			ret=(-__LINE__);
			goto exit;
			}
		ic_assoc(elist,0);
		strncpy(etype,ic_rbassoc->resval.rstring,sizeof(etype)-1);
		if (RTNORM!=sds_wcmatch(etype,"LINE,ARC,POLYLINE,CIRCLE,LWPOLYLINE"/*DNT*/)) 
			{
			sds_redraw(pent,IC_REDRAW_UNHILITE);
			cmd_ErrorPrompt(CMD_ERR_POLYLINE,0);
			pent[0] = pent[1] = 0L;
			continue;
			}

		wasPolyline = (RTNORM==sds_wcmatch(etype, "POLYLINE"/*DNT*/)) ? true : false; 

		//*****************************************************************************
		//***********TURN LINE, ARC, OR CIRCLE INTO A PLINE****************************
		//*****************************************************************************
		if (strsame(etype,db_hitype2str(DB_LINE))) 
			{
			//proceed as if you're gonna convert it to pline, then ask 'em just before
			//	entmake.  We need to check if ent in current XY plane.
            sds_entmake(NULL);
			ic_assoc(elist,0);
			if ((ic_rbassoc->resval.rstring=ic_realloc_char(ic_rbassoc->resval.rstring,strlen("POLYLINE"/*DNT*/)+1))==NULL)
				{
				cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
				ret=(-__LINE__);
				goto exit;
				}
			strcpy(ic_rbassoc->resval.rstring,"POLYLINE"/*DNT*/);
			ic_assoc(elist,10);
			ic_ptcpy(p1,ic_rbassoc->resval.rpoint);
			ic_assoc(elist,11);
			ic_ptcpy(p2,ic_rbassoc->resval.rpoint);
			ic_rbassoc->restype=66;
			ic_rbassoc->resval.rint=1;
			rb.restype=RTSHORT;
			rb.resval.rint=0;
			rb2.restype=RT3DPOINT;
			ic_ptcpy(rb2.resval.rpoint,pt210);
			sds_trans(p1,&rb,&rb2,0,p1);
			sds_trans(p2,&rb,&rb2,0,p2);
			if (!icadRealEqual(p1[2],p2[2])) 
				{
				cmd_ErrorPrompt(CMD_ERR_NOTINUCS,0);
				goto exit;
				}
			ic_assoc(elist,210);
			ic_ptcpy(ic_rbassoc->resval.rpoint,pt210);
			

			// Only display the "Turn it into one?" message if PEDITACCEPT is set to 1
			resbuf	peditAcceptRb;
			if(SDS_getvar(NULL, DB_QPEDITACCEPT, &peditAcceptRb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES) != RTNORM) 
				peditAcceptRb.resval.rint = 1; // on error, cause message to be displayed
			if (peditAcceptRb.resval.rint == 1)
				{
				sds_initget(0,ResourceString(IDC_PEDIT_INITGET_YES_TURN_I_2, "Yes-Turn_into_polyline|Yes No-Leave_as_is|No ~_Yes ~_No" ));
				if ((fi1=sds_getkword(ResourceString(IDC_PEDIT__NTHE_ENTITY_SELEC_3, "\nThe entity selected is not a polyline.  Turn it into one? <Y> " ),fs1))==RTCAN) 
					{
					sds_redraw(pent,IC_REDRAW_UNHILITE);
					goto exit;
					}
				if (strisame(fs1,"NO"/*DNT*/ )) 
					{
					sds_redraw(pent,IC_REDRAW_UNHILITE);
					continue;
					}
				}
            if (sds_entmake(elist)!=RTNORM) 
				{
				ret=(-__LINE__);
				goto exit;
				}
            sds_relrb(elist);
			elist=NULL;
            elist=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,10,p1,42,0.0,0);
            if (sds_entmake(elist)!=RTNORM) 
				{
				ret=(-__LINE__);
				goto exit;
				}
            sds_relrb(elist);
			elist=NULL;
            elist=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,10,p2,0);
            if (sds_entmake(elist)!=RTNORM) 
				{
				ret=(-__LINE__);
				goto exit;
				}
            sds_relrb(elist);
			elist=NULL;
            elist=sds_buildlist(RTDXF0,"SEQEND"/*DNT*/,0);
            if (sds_entmake(elist)!=RTNORM) 
				{
				ret=(-__LINE__);
				goto exit;
				}
            sds_relrb(elist);
			elist=NULL;
            sds_entdel(pent);
            sds_entlast(pent);
			sds_redraw(pent,IC_REDRAW_DRAW);
			strncpy(etype, "POLYLINE"/*DNT*/,sizeof(etype)-1);
            FreeResbufIfNotNull(&elist);
			}
		else if (RTNORM==sds_wcmatch(etype, "ARC,CIRCLE"/*DNT*/)) 
			{
			//examine entity first...
			for (rbtemp=elist;; rbtemp=rbtemp->rbnext) //breakout is inside loop
				{
				if (rbtemp->restype==0)
					ic_rbassoc=rbtemp;
				else if (rbtemp->restype==10)
					ic_ptcpy(p0,rbtemp->resval.rpoint);
				else if (rbtemp->restype==40) 
					{
					fr1=rbtemp->resval.rreal;
					//change 40 to 66
					rbtemp->restype=66;
					rbtemp->resval.rint=1;
					}
				else if (rbtemp->restype==50)
					fr2=rbtemp->resval.rreal;
				else if (rbtemp->restype==51)
					fr3=rbtemp->resval.rreal;
				else if (rbtemp->restype==210) 
					{
					if (!icadPointEqual(rbtemp->resval.rpoint,pt210)) 
						{
						cmd_ErrorPrompt(CMD_ERR_NOTINUCS,0);
						rbtemp=NULL;
						sds_redraw(pent,IC_REDRAW_HILITE);
						goto exit;
						}
					}
				//add 70 group onto end of elist
				if (rbtemp->rbnext==NULL) 
					{
					if (strsame(etype,"CIRCLE"/*DNT*/))
						fi2=1;
					else 
						fi2=0;
                    if ((rbtemp->rbnext=sds_buildlist(70,fi2,0))==NULL) 
						{
						ret=(-__LINE__);
						goto exit;
						}
					rbtemp=NULL;
					break;//avoid infinite loop
					}

				}
			if ((ic_rbassoc->resval.rstring=ic_realloc_char(ic_rbassoc->resval.rstring,strlen("POLYLINE"/*DNT*/)+1))==NULL) 
				{
				cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
				ret=(-__LINE__);
				goto exit;
				}
			strcpy(ic_rbassoc->resval.rstring,"POLYLINE"/*DNT*/);

			// Only display the "Turn it into one?" message if PEDITACCEPT is set to 1
			resbuf	peditAcceptRb;
			if(SDS_getvar(NULL, DB_QPEDITACCEPT, &peditAcceptRb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES) != RTNORM) 
				peditAcceptRb.resval.rint = 1; // on error, cause message to be displayed
			if (peditAcceptRb.resval.rint == 1)
				{
				//NOTE: do all the questioning after examining elist to see if ent in UCS.
				//			User won't see time lag
				sds_initget(0,ResourceString(IDC_PEDIT_INITGET_YES_TURN_I_2, "Yes-Turn_into_polyline|Yes No-Leave_as_is|No ~_Yes ~_No" ));
				if ((fi1=sds_getkword(ResourceString(IDC_PEDIT__NTHE_ENTITY_SELEC_3, "\nThe entity selected is not a polyline.  Turn it into one? <Y> " ),fs1))==RTCAN) 
					{
					sds_redraw(pent,IC_REDRAW_UNHILITE);
					goto exit;
					}
				if (strisame(fs1,"NO"/*DNT*/ )) 
					{
					sds_redraw(pent,IC_REDRAW_UNHILITE);
					continue;
					}
				}

			if (strsame(etype,"ARC"/*DNT*/)) 
				{
				//omit 50 & 51 groups from elist
				for (fi2=50;fi2<=51;fi2++) 
					{
					for (rbtemp=elist; rbtemp->rbnext!=NULL && rbtemp->rbnext->restype!=fi2; rbtemp=rbtemp->rbnext);//eliminate 50 from elist
					if (rbtemp->rbnext==NULL) 
						{
						ret=(-__LINE__);
						goto exit;
						}
					ic_rbassoc=rbtemp->rbnext;
					rbtemp->rbnext=ic_rbassoc->rbnext;
					ic_rbassoc->rbnext=NULL;
                    sds_relrb(ic_rbassoc);
					}
				}
			else 				//chg circle to CLOSED pline vertices at 3 o'clock & 9 o'clock
				{
				fr2=0.0;
				fr3=IC_PI;
				}
            sds_polar(p0,fr2,fr1,p1);
            sds_polar(p0,fr3,fr1,p2);
			ic_normang(&fr2,&fr3);
			fr1=tan((fr3-fr2)/4.0);
			fr4=tan((IC_TWOPI-fr3+fr2)/4.0);
            sds_entmake(NULL);
            if (sds_entmake(elist)!=RTNORM) 
				{
				ret=(-__LINE__);
				goto exit;
				}
            if ((rbtemp=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,10,p1,42,fr1,210,pt210,0))==NULL) 
				{
				ret=(-__LINE__);
				goto exit;
				}
            if (sds_entmake(rbtemp)!=RTNORM) 
				{
				ret=(-__LINE__);
				goto exit;
				}
            sds_relrb(rbtemp);
			rbtemp=NULL;
            if ((rbtemp=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,10,p2,42,fr4,210,pt210,0))==NULL) 
				{
				ret=(-__LINE__);
				goto exit;
				}
            if (sds_entmake(rbtemp)!=RTNORM) 
				{
				ret=(-__LINE__);
				goto exit;
				}
            sds_relrb(rbtemp);
			rbtemp=NULL;
            if ((rbtemp=sds_buildlist(RTDXF0,"SEQEND"/*DNT*/,0))==NULL) 
				{
				ret=(-__LINE__);
				goto exit;
				}
            if (sds_entmake(rbtemp)!=RTNORM) 
				{
				ret=(-__LINE__);
				goto exit;
				}
			else
				{
                sds_relrb(rbtemp);
				rbtemp=NULL;
                sds_entdel(pent);
                sds_entlast(pent);
				sds_redraw(pent,IC_REDRAW_DRAW);
				}
            FreeResbufIfNotNull(&elist);
			strncpy(etype,"POLYLINE"/*DNT*/,sizeof(etype)-1);
			}
		else if (strsame(etype,"LWPOLYLINE")) 
			{
			ic_encpy(lwpent,pent);
			if (ConvertLwpolylineToPolyline(pent,pent)!=RTNORM)
				goto exit;
			originalwaslwpolyline=true;
			strncpy(etype,"POLYLINE"/*DNT*/,sizeof(etype)-1);
			}

		//**************LINE/ARC/CIRCLE IS NOW A PLINE********
		if (!strsame(etype,"POLYLINE"/*DNT*/))
			goto exit;

		//note:elist will be generated prior to each pline modification & saved
		if (elist!=NULL) 
			{
			ic_assoc(elist,70);
			if (ic_rbassoc->resval.rint & IC_PLINE_POLYFACEMESH) 
				{
				cmd_ErrorPrompt(CMD_ERR_PEDITPFACE,0);
				goto exit;
				}
			poly3d=ic_rbassoc->resval.rint;
			if (0==(poly3d & (IC_PLINE_3DPLINE | IC_PLINE_3DMESH)) && 0==ic_assoc(elist,210) && !icadPointEqual(ic_rbassoc->resval.rpoint,pt210)) 
				{
				cmd_ErrorPrompt(CMD_ERR_NOTINUCS,0);
				goto exit;
				}
            sds_relrb(elist);
			elist=NULL;
			}
		else//we converted the entity into a pline & blew away orig elist
			{
			poly3d=0;
			}
		//*************************************************
		//********* MODIFY PLINE STARTS HERE **************
		//*************************************************
		sds_redraw(pent,IC_REDRAW_HILITE);
		redrawflag=0;
		for (;;)
			{
			if (poly3d & IC_PLINE_3DMESH) 
				{
				if ((poly3d & IC_PLINE_CLOSED) && (poly3d & IC_PLINE_CLOSEDN)) 
                    sds_initget(SDS_RSG_NOCANCEL,ResourceString(IDC_PEDIT_INITGET_EDIT_VERTI_5, "Edit_vertices|Edit ~ Desmooth M_open|Mopen N_open|Nopen ~Smooth ~ Undo ~eXit ` ~_Edit ~_ ~_Desmooth ~_Mopen ~_Nopen ~_Smooth ~_ ~_Undo ~_eXit" ));
				else if (!(poly3d & IC_PLINE_CLOSED) && !(poly3d & IC_PLINE_CLOSEDN))
                    sds_initget(SDS_RSG_NOCANCEL,ResourceString(IDC_PEDIT_INITGET_EDIT_VERTI_6, "Edit_vertices|Edit ~ Desmooth M_close|Mclose N_close|Nclose ~Smooth ~ Undo ~eXit ` ~_Edit ~_ ~_Desmooth ~_Mclose ~_Nclose ~_Smooth ~_ ~_Undo ~_eXit" ));
				else if (poly3d & IC_PLINE_CLOSEDN)
                    sds_initget(SDS_RSG_NOCANCEL,ResourceString(IDC_PEDIT_INITGET_EDIT_VERTI_7, "Edit_vertices|Edit ~ Desmooth M_close|Mclose N_open|Nopen ~Smooth ~ Undo ~eXit ` ~_Edit ~_ ~_Desmooth ~_Mclose ~_Nopen ~_Smooth ~_ ~_Undo ~_eXit" ));
				else // closedm for this case
                    sds_initget(SDS_RSG_NOCANCEL,ResourceString(IDC_PEDIT_INITGET_EDIT_VERTI_8, "Edit_vertices|Edit ~ Desmooth M_open|Mopen N_close|Nclose ~Smooth ~ Undo ~eXit ` ~_Edit ~_ ~_Desmooth ~_Mopen ~_Nclose ~_Smooth ~_ ~_Undo ~_eXit" ));
				sprintf(fs2,ResourceString(IDC_PEDIT__NEDIT_POLYLINE____9, "\nEdit polyline:  Edit vertices/Desmooth/%s/%s/Undo/<eXit>: " ),(poly3d & 1)?ResourceString(IDC_PEDIT_MOPEN_10, "Mopen" ):ResourceString(IDC_PEDIT_MCLOSE_11, "Mclose" ),(poly3d & 32)?ResourceString(IDC_PEDIT_NOPEN_12, "Nopen" ):ResourceString(IDC_PEDIT_NCLOSE_13, "Nclose" ));
				//sprintf(fs2,"\nEdit vertex/Smooth Surface/Desmooth/%s/%s/Undo/eXit <X>: ",(poly3d & 1)?"Mopen":"Mclose",(poly3d & 32)?"Nopen":"Nclose");
                fi1=sds_getkword(fs2,fs1);
				}
			else if (poly3d & IC_PLINE_3DPLINE) 
				{
				if (poly3d & IC_PLINE_CLOSED) 
					{
					//NOTE:Acad won't allow 3d join - this routine does!
                    sds_initget(SDS_RSG_NOCANCEL,ResourceString(IDC_PEDIT_INITGET_EDIT_VERT_14, "Edit_vertices|Edit ~ Open ~ Decurve ~Fit Join Reverse Spline ~ Undo ~eXit ` ~_Edit ~_ ~_Open ~_ ~_Decurve ~_Fit ~_Join ~_Reverse ~_Spline ~_ ~_Undo ~_eXit" ));
                    //fi1=sds_getkword("\nClose/Edit vertex/Spline/Decurve/Undo/eXit <X>: ",fs1);
                    fi1=sds_getkword(ResourceString(IDC_PEDIT__NEDIT_POLYLINE___15, "\nEdit polyline:  Edit vertices/Open/Decurve/Join/Reverse/Undo/<eXit>: " ),fs1);
					}
				else
					{
					//NOTE:Acad won't allow 3d join - this routine does!
                    sds_initget(SDS_RSG_NOCANCEL,ResourceString(IDC_PEDIT_INITGET_EDIT_VERT_16, "Edit_vertices|Edit ~ Close ~ Decurve ~Fit Join Reverse Spline ~ Undo ~eXit ` ~_Edit ~_ ~_Close ~_ ~_Decurve ~_Fit ~_Join ~_Reverse ~_Spline ~_ ~_Undo ~_eXit" ));
                    //fi1=sds_getkword("\nClose/Edit vertex/Spline/Decurve/Undo/eXit <X>: ",fs1);
                    fi1=sds_getkword(ResourceString(IDC_PEDIT__NEDIT_POLYLINE___17, "\nEdit polyline:  Edit vertices/Close/Decurve/Join/Reverse/Undo/<eXit>: " ),fs1);
					}
				}
			else if (!(poly3d & IC_PLINE_POLYFACEMESH) && !(poly3d & IC_PLINE_3DMESH) && !(poly3d & IC_PLINE_3DPLINE)) 
				{
				if (poly3d & IC_PLINE_CLOSED) 
					{
					LOAD_COMMAND_OPTIONS_2(IDC_PEDIT_INITGET_EDIT_VERT_18)
                    sds_initget(SDS_RSG_NOCANCEL, LOADEDSTRING);
                    //fi1=sds_getkword("\nClose/Join/Width/Edit vertex/Fit/Spline/Decurve/Ltype gen/Undo/eXit <X>: ",fs1);
                    fi1=sds_getkword(ResourceString(IDC_PEDIT__NEDIT_POLYLINE___19, "\nEdit polyline:  Edit vertices/Open/Decurve/Fit/Join/Linetype-mode/Reverse/Spline/Taper/Width/Undo/<eXit>: " ),fs1);
					}
				else
					{
					LOAD_COMMAND_OPTIONS_2(IDC_PEDIT_INITGET_EDIT_VERT_20)
                    sds_initget(SDS_RSG_NOCANCEL, LOADEDSTRING);
                    //fi1=sds_getkword("\nClose/Join/Width/Edit vertex/Fit/Spline/Decurve/Ltype gen/Undo/eXit <X>: ",fs1);
                    fi1=sds_getkword(ResourceString(IDC_PEDIT__NEDIT_POLYLINE___21, "\nEdit polyline:  Edit vertices/Close/Decurve/Fit/Join/Linetype-mode/Reverse/Spline/Taper/Width/Undo/<eXit>: " ),fs1);
					}
				}
			else
				{
				// shouldn't get here
				//
				ASSERT( false );
				goto exit;
				}
			if (redrawflag==0) 
				{
				redrawflag=1;
				sds_redraw(pent,IC_REDRAW_UNHILITE);
				}
			if ((fi1!=RTNORM) || (strisame(fs1,"EXIT"/*DNT*/ )))
				goto exit;//covers RTNONE which is always exit

			if (!strisame(fs1,"UNDO"/*DNT*/ )) 
				{
				//setup undo list for current status of pline
				elist_llbeg=elist_llcur=NULL;
				cmd_alloc_elistll(&elist_llbeg,&elist_llcur,pent,0);
				if (undo_beg==NULL) 
					{
					//*** Allocate a new link in property list.
					if ((undo_temp= new struct cmd_elistll_ptr )==NULL) 
						{
					    cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
					    ret=(-__LINE__); goto exit;
						}
					memset(undo_temp,0,sizeof(struct cmd_elistll_ptr));
					undo_beg=undo_cur=undo_temp;
					undo_cur->elistll=elist_llbeg;
					}
				else
					{
					if ((undo_temp= new struct cmd_elistll_ptr )==NULL) 
						{
					    cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
					    ret=(-__LINE__); goto exit;
						}
					memset(undo_temp,0,sizeof(struct cmd_elistll_ptr));
					undo_cur->next=undo_temp;
					undo_cur=undo_cur->next;
					undo_cur->elistll=elist_llbeg;
					}
	            FreeResbufIfNotNull(&elist);
				//NOTE: DON'T point elist to undo_cur->elistll->entlst or
				//		undo buffer will get screwed up!
                if ((elist=sds_entgetx(pent,&rb))==NULL) 
					{
					ret=(-__LINE__);
					goto exit;
					}
				//ic_encpy(pent,undo_cur->elistll->ename);
				}
			else
				{
				//undo last pline operation
				if (undo_beg==NULL)
					cmd_ErrorPrompt(CMD_ERR_NOMOREUNDO,0);
				else
					{
					undo_temp=undo_cur;
                    sds_entdel(pent);
					if (0==ic_assoc(undo_temp->elistll->entlst,70))
						poly3d=ic_rbassoc->resval.rint;
					do
						{
                        if (sds_entmake(undo_temp->elistll->entlst)!=RTNORM) 
							{
							sds_entdel(pent);
							ret=(-__LINE__);
							goto exit;
							}
						}
					while ((undo_temp->elistll=undo_temp->elistll->next)!=NULL);
                    sds_entlast(pent);

					//sds_redraw(pent,IC_REDRAW_DRAW);

					//step to element just before cur
					for (undo_temp=undo_beg;(undo_temp->next!=undo_cur)&&(undo_temp->next!=NULL);undo_temp=undo_temp->next);
					cmd_free_elistll(&undo_cur->elistll,NULL);
					if (undo_cur!=undo_beg) 
						{
						delete undo_cur;
						undo_cur=undo_temp;
						undo_cur->next=NULL;
						}
					else
						{
						delete undo_beg;
						undo_cur=undo_beg=NULL;
						}
					}
				}
			if (strisame(fs1,"CLOSE"/*DNT*/ )) 
				{
				ic_assoc(elist,70);
				int flags=ic_rbassoc->resval.rint;
				if (flags & IC_PLINE_CLOSED) 
					{
					cmd_ErrorPrompt(CMD_ERR_CLOSEDPLINE,0);
					}
				else if (IC_PLINE_CURVEFIT==(flags&(IC_PLINE_CURVEFIT | IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH))) 
					{
// in other words, it is curve fit, and is not one ofthose surfaces
					if (cmd_decurve_pline(pent,1)!=0) 
						{
						ret=(-__LINE__);
						goto exit;
						}
		            FreeResbufIfNotNull(&elist);
                    if ((elist=sds_entgetx(pent,&rb))==NULL) 
						{
						ret=(-__LINE__);
						goto exit;
						}
					ic_assoc(elist,70);
					ic_rbassoc->resval.rint |= IC_PLINE_CLOSED;
					poly3d=ic_rbassoc->resval.rint;
                    if (sds_entmod(elist)!=RTNORM) 
						goto exit;
					if ((ret2=cmd_pedit_fitcurve(elist,1,fittype,splinesegs,pent))!=RTNORM) 
						{
						if (ret2==RTERROR)
							cmd_ErrorPrompt(CMD_ERR_FITCURVE,0);
						ret=(-__LINE__);
						goto exit;
						}
					poly3d|=IC_PLINE_CURVEFIT;
					}
				else if (IC_PLINE_SPLINEFIT==(flags&(IC_PLINE_SPLINEFIT | IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH))) 
					{
// in other words, it is spline fit, and not one of those surfaces
					if (cmd_decurve_pline(pent,1)!=0) 
						{
						ret=(-__LINE__);
						goto exit;
						}
		            FreeResbufIfNotNull(&elist);
                    if ((elist=sds_entgetx(pent,&rb))==NULL) 
						{
						ret=(-__LINE__);
						goto exit;
						}
					ic_assoc(elist,70);
					ic_rbassoc->resval.rint |= IC_PLINE_CLOSED;
					poly3d=ic_rbassoc->resval.rint;
                    if (sds_entmod(elist)!=RTNORM)
						goto exit;
					if ((ret2=cmd_pedit_fitcurve(elist,0,splinetype,splinesegs,pent))!=RTNORM) 
						{
						if (ret2==RTERROR)
							cmd_ErrorPrompt(CMD_ERR_FITCURVE,0);
						ret=(-__LINE__);
						goto exit;
						}
					poly3d|=IC_PLINE_SPLINEFIT;
					}
				else
					{
					ic_rbassoc->resval.rint |= IC_PLINE_CLOSED;
                    if (sds_entmod(elist)!=RTNORM) 
						{
						ret=(-__LINE__);
						goto exit;
						}
					poly3d=ic_rbassoc->resval.rint;
					//get bulge of closing segment as fr1
					//we need bulge of last segment before we close
					if (cmd_pline_next2lastvtx(pent,etemp1)) 
						{
						ret=(-__LINE__);
						goto exit;
						}
		            FreeResbufIfNotNull(&rbtemp);
					if ((rbtemp=sds_entget(etemp1))==NULL) 
						{
						ret=(-__LINE__);
						goto exit;
						}
					ic_assoc(rbtemp,42);
					fr2=ic_rbassoc->resval.rreal;
					//calculate new closing bulge-
					//if last seg was straight, close w/straight seg
					if (icadRealEqual(fr2,0.0))
						fr1=0.0;
					else
						{ //close w/bulge tangent to last vertex
						//let p1 be next2last vtx pt, p2 last vtx pt, p3 start pt
						ic_assoc(rbtemp,10);
						ic_ptcpy(p1,ic_rbassoc->resval.rpoint);
                        sds_entnext_noxref(pent,etemp2);
			            FreeResbufIfNotNull(&rbtemp);
                        if ((rbtemp=sds_entget(etemp2))==NULL) 
							{
							ret=(-__LINE__);
							goto exit;
							}
						ic_assoc(rbtemp,10);
						ic_ptcpy(p3,ic_rbassoc->resval.rpoint);
                        sds_entnext_noxref(etemp1,etemp2);
			            FreeResbufIfNotNull(&rbtemp);
                        if ((rbtemp=sds_entget(etemp2))==NULL) 
							{
							ret=(-__LINE__);
							goto exit;
							}
						ic_assoc(rbtemp,10);
						ic_ptcpy(p2,ic_rbassoc->resval.rpoint);
						ic_bulge2arc(p1,p2,fr2,p0,&fr3,&fr4,&fr5);
						//get tangent angle for pline continuation - fr3
						if (fr2>0.0)  //ccw arc
							{
							fr3=fr5+(IC_PI/2.0);
							ic_2pta2arc(p2,p3,fr3,p0,&fr6,&fr7,&fr8);
							ic_normang(&fr7,&fr8);
							if (icadAngleEqual(fr5,fr7))
								fr1=tan((fr8-fr7)/4.0);
							else
								fr1=tan((fr7-fr8)/4.0);
							}
						else //cw arc
							{
							fr3=fr4-(IC_PI/2.0);
							ic_2pta2arc(p2,p3,fr3,p0,&fr6,&fr7,&fr8);
							ic_normang(&fr7,&fr8);
							if (icadAngleEqual(fr4,fr8))
								fr1=tan((fr7-fr8)/4.0);
							else
								fr1=tan((fr8-fr7)/4.0);
							}
						}
                    sds_entnext_noxref(etemp1,etemp1);
		            FreeResbufIfNotNull(&rbtemp);
                    if ((rbtemp=sds_entget(etemp1))==NULL) 
						{
						ret=(-__LINE__);
						goto exit;
						}
					ic_assoc(rbtemp,42);
					ic_rbassoc->resval.rreal=fr1;
                    if (sds_entmod(rbtemp)!=RTNORM) 
						{
						ret=(-__LINE__);
						goto exit;
						}
                    sds_relrb(rbtemp);
					rbtemp=NULL;
                    if (sds_entupd(pent)!=RTNORM) 
						{
						ret=(-__LINE__);
						goto exit;
						}
					}
				}
			else if (strisame(fs1,"MCLOSE"/*DNT*/ )) 
				{
				if (poly3d & IC_PLINE_CLOSED)
					cmd_ErrorPrompt(CMD_ERR_CLOSEDMPLINE,0);
				else
					{
					ic_assoc(elist,70);
					ic_rbassoc->resval.rint++;
                    if (sds_entmod(elist)!=RTNORM)
						goto exit;
					poly3d=ic_rbassoc->resval.rint;
					}
				}
			else if (strisame(fs1,"NCLOSE"/*DNT*/ )) 
				{
				if (poly3d & IC_PLINE_CLOSEDN)
					cmd_ErrorPrompt(CMD_ERR_CLOSEDNPLINE,0);
				else
					{
					ic_assoc(elist,70);
					ic_rbassoc->resval.rint |= IC_PLINE_CLOSEDN;
                    if (sds_entmod(elist)!=RTNORM)
						goto exit;
					poly3d=ic_rbassoc->resval.rint;
					}
				}
			else if ((strisame(fs1,"OPEN"/*DNT*/ ))||(strisame(fs1,"MOPEN"/*DNT*/ ))) 
				{
				ic_assoc(elist,70);
				int flags=ic_rbassoc->resval.rint;
				if ((flags & IC_PLINE_CLOSED) && (flags & IC_PLINE_CURVEFIT) &&
					!(flags & IC_PLINE_3DMESH) && !(flags & IC_PLINE_POLYFACEMESH)) 
					{
					if (cmd_decurve_pline(pent,1)!=0) 
						{
						ret=(-__LINE__);
						goto exit;
						}
		            FreeResbufIfNotNull(&elist);
                    if ((elist=sds_entgetx(pent,&rb))==NULL) 
						{
						ret=(-__LINE__);
						goto exit;
						}
					ic_assoc(elist,70);
					ic_rbassoc->resval.rint &= ~IC_PLINE_CLOSED;
					poly3d=ic_rbassoc->resval.rint;
                    if (sds_entmod(elist)!=RTNORM)
						goto exit;
					if ((ret2=cmd_pedit_fitcurve(elist,1,fittype,splinesegs,pent))!=RTNORM) 
						{
						if (ret2==RTERROR)
							cmd_ErrorPrompt(CMD_ERR_FITCURVE,0);
						ret=(-__LINE__);
						goto exit;
						}
					poly3d|=IC_PLINE_CURVEFIT; // shd be already

					}
				else if ((flags & IC_PLINE_CLOSED) && (flags & IC_PLINE_SPLINEFIT) &&
					!(flags & IC_PLINE_3DMESH) && !(flags & IC_PLINE_POLYFACEMESH)) 
					{
					if (cmd_decurve_pline(pent,1)!=0) 
						{
						ret=(-__LINE__);
						goto exit;
						}
		            FreeResbufIfNotNull(&elist);
                    if ((elist=sds_entgetx(pent,&rb))==NULL) 
						{
						ret=(-__LINE__);
						goto exit;
						}
					ic_assoc(elist,70);
					ic_rbassoc->resval.rint &= ~IC_PLINE_CLOSED;
					poly3d=ic_rbassoc->resval.rint;
                    if (sds_entmod(elist)!=RTNORM) 
						goto exit;
					if ((ret2=cmd_pedit_fitcurve(elist,0,splinetype,splinesegs,pent))!=RTNORM) 
						{
						if (ret2==RTERROR)
							cmd_ErrorPrompt(CMD_ERR_FITCURVE,0);
						ret=(-__LINE__);
						goto exit;
						}
					poly3d|=4;
					}
				else if (ic_rbassoc->resval.rint & IC_PLINE_CLOSED) 
					{
					ic_rbassoc->resval.rint &= ~IC_PLINE_CLOSED;
                    if (sds_entmod(elist)!=RTNORM)
						goto exit;
					poly3d=ic_rbassoc->resval.rint;
					}
				else
					cmd_ErrorPrompt(CMD_ERR_NOTCLOSEDPLINE,0);
				}
			else if (strisame(fs1,"NOPEN"/*DNT*/ )) 
				{
				if (poly3d & IC_PLINE_CLOSEDN) 
					{
					ic_assoc(elist,70);
					ic_rbassoc->resval.rint &= ~IC_PLINE_CLOSEDN;
                    if (sds_entmod(elist)!=RTNORM)
						goto exit;
					poly3d=ic_rbassoc->resval.rint;
					}
				else
					cmd_ErrorPrompt(CMD_ERR_NOTCLOSEDNPLINE,0);
				}
			else if (strisame(fs1,"WIDTH"/*DNT*/ )) 
				{
                sds_initget(5,NULL);
                if (sds_getdist(NULL,ResourceString(IDC_PEDIT__NENTER_NEW_WIDTH_27, "\nEnter new width for all segments: " ),&fr1)!=RTNORM)goto exit;
				if (0!=cmd_reset_plwidth(pent,fr1)) 
					{
					ret=-1;
					goto exit;
					}
				}
			else if (strisame(fs1,"TAPER"/*DNT*/ )) 
				{
                sds_entnext_noxref(pent,etemp1);
	            FreeResbufIfNotNull(&rbtemp);
                if ((rbtemp=sds_entget(etemp1))==NULL) 
					{
					ret=(-__LINE__);
					goto exit;
					}
				if (ic_assoc(rbtemp,40))
					fr1=0.0;
				else
					fr1=ic_rbassoc->resval.rreal;
                sds_rtos(fr1,-1,-1,fs1);
				sprintf(fs2,ResourceString(IDC_PEDIT__NENTER_NEW_START_29, "\nEnter new starting polyline width <%s> : " ),fs1);
                sds_initget(4,NULL);
                if ((fi1=sds_getdist(NULL,fs2,&fr2))==RTCAN)
					goto exit;
				if (fi1==RTNORM)
					fr1=fr2;
				cmd_pline_next2lastvtx(pent,etemp1);
				ic_assoc(elist,70);
                if (ic_rbassoc->resval.rint & IC_PLINE_CLOSED)
					sds_entnext_noxref(etemp1,etemp1);
	            FreeResbufIfNotNull(&rbtemp);
                if ((rbtemp=sds_entget(etemp1))==NULL) 
					{
					ret=(-__LINE__);
					goto exit;
					}
				if (ic_assoc(rbtemp,41))
					fr2=0.0;
				else
					fr2=ic_rbassoc->resval.rreal;
                sds_rtos(fr2,-1,-1,fs1);
				sprintf(fs2,ResourceString(IDC_PEDIT__NENTER_NEW_ENDIN_30, "\nEnter new ending polyline width <%s> : " ),fs1);
                sds_initget(4,NULL);
                if ((fi1=sds_getdist(NULL,fs2,&fr3))==RTCAN)
					goto exit;
				if (fi1==RTNORM)
					fr2=fr3;
	            FreeResbufIfNotNull(&rbtemp);
				if (cmd_taper_pline(pent,fr1,fr2)!=0) 
					{
					ret=-1;
					goto exit;
					}
				}
			else if (strisame(fs1,"LTYPE"/*DNT*/ )) 
				{
				ic_assoc(elist,70);
				fi1=(ic_rbassoc->resval.rint & IC_PLINE_CONTINUELT);
				sprintf(fs1,ResourceString(IDC_PEDIT__NLINETYPE_CONTIN_32, "\nLinetype continuous along polyline:  ON/OFF <%s>: " ),(fi1)?ResourceString(IDC_PEDIT_ON_33, "ON" ):ResourceString(IDC_PEDIT_OFF_34, "OFF" ));
                sds_initget(0,ResourceString(IDC_PEDIT_INITGET_CONTINUOU_35, "Continuous_on|ON Continuous_off|OFf ~_ON ~_OFf" ));
                if (sds_getkword(fs1,fs2)==RTNORM) 
					{
					if (strisame(fs2,"ON"/*DNT*/ )&& !fi1)
						ic_rbassoc->resval.rint|=IC_PLINE_CONTINUELT;
					else if (strisame(fs2,"OFF"/*DNT*/ )&& fi1)
						ic_rbassoc->resval.rint &= (~IC_PLINE_CONTINUELT);
					else
						continue;
                    if (sds_entmod(elist)!=RTNORM) 
						{
						ret=-1;
						goto exit;
						}
					}
				}
			else if ((strisame(fs1,"DECURVE"/*DNT*/ ))|| (strisame(fs1,"DESMOOTH"/*DNT*/ ))) 
				{
				if (!(poly3d&(IC_PLINE_CURVEFIT | IC_PLINE_SPLINEFIT))) 
					{
					if (cmd_decurve_pline(pent,1)!=0) 
						{
						ret=(-__LINE__);
						goto exit;
						}
					poly3d&=(~(IC_PLINE_CURVEFIT | IC_PLINE_SPLINEFIT));
					}
				else
					{
					if (cmd_decurve_pline(pent,0)!=0) 
						{
						ret=(-__LINE__);
						goto exit;
						}
					}
				}
			else if (strisame(fs1,"JOIN"/*DNT*/)) 
				{
				//JOIN does an implied decurve - we can always re-generate the
				//	spline or curve once the new objects have been joined to the
				//	pline
				ic_assoc(elist,70);
				fi1=ic_rbassoc->resval.rint;
				if (fi1 & IC_PLINE_CLOSED) 
					{
					cmd_ErrorPrompt(CMD_ERR_JOINCLOSEDPLINE,0);
					continue;
					}
				else if ((fi1 & IC_PLINE_POLYFACEMESH) || (fi1 & IC_PLINE_3DMESH)) 
					{
					cmd_ErrorPrompt(CMD_ERR_POLYLINE,0);
					//this should never happen, but I put it here anyway
					continue;
					}
				//join selected entities to pline.....
				//don't need elist - dump it
	            FreeResbufIfNotNull(&elist);
				rb.restype=RTSTR;
				rb.rbnext=NULL;
				rb.resval.rstring=NULL; // otherwise assert at db_mstrncpy call

				// DP: LWPOLYLINE in string is allowing to select lwpolylines to join
				//db_mstrncpy(&rb.resval.rstring, "LINE,ARC,POLYLINE"/*DNT*/,-1);
				db_mstrncpy(&rb.resval.rstring, "LINE,ARC,POLYLINE,LWPOLYLINE"/*DNT*/,-1);

				//NOTE:  You could include circles & convert them to
				//open plines w/vertices at intersection pt, pt opposite,
				//& intersection pt. (Don't use 2-vertex w/infinite bulge)
				resbuf rb2;
				SDS_getvar(NULL,DB_QCMDECHO,&rb2,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				if (CMD_CMDECHO_PRINTF(rb2.resval.rint)) 
					{
                    sds_printf(ResourceString(IDC_PEDIT__NSELECT_ENTITIES_40, "\nSelect entities to join." ));
                    if (poly3d & IC_PLINE_3DPLINE) sds_printf(ResourceString(IDC_PEDIT__ALL_CURVED_ENTIT_41, " All curved entities will be straightened." ));
					}
                int tmpret=sds_pmtssget(NULL,NULL,NULL,NULL,&rb,ssjoin,0);
				IC_FREE(rb.resval.rstring);
				if (tmpret!=RTNORM)
					goto exit;

				if ((fi1 & IC_PLINE_CURVEFIT) || (fi1 & IC_PLINE_SPLINEFIT)) 
					{
					if (cmd_decurve_pline(pent,0)!=0) 
						{
						ret=(-__LINE__);
						goto exit;
						}
					}
				cmd_join_pline(pent, originalwaslwpolyline ? lwpent : pent, ssjoin);
				sds_ssfree(ssjoin);
				}
			else if (strisame(fs1,"EDIT"/*DNT*/ )) 
				{
				//NOTE: cmd_pline_editvtx will modify ent & chg value in pent
				if (RTCAN==(ret2=cmd_pline_editvtx(pent)))
					continue;
				if (ret2!=RTNORM)
					{
					ret=(-__LINE__);
					goto exit;
					}
				}
			else if (strisame(fs1,"REVERSE"/*DNT*/ )) 
				{
				//you can't reverse a splined/fit pline
				ic_assoc(elist,70);
				if (ic_rbassoc->resval.rint&(IC_PLINE_CURVEFIT | IC_PLINE_SPLINEFIT)) 
					{
                    if ((ret2=sds_initget(0,ResourceString(IDC_PEDIT_INITGET_YES_DECUR_44, "Yes-Decurve|Yes No-Do_not_Decurve|No ~_Yes ~_No" )))!=RTNORM)
						{
						ret=(-__LINE__);
						goto exit;
						}
                    if ((ret2=sds_getkword(ResourceString(IDC_PEDIT__NREVERSING_POLYL_45, "\nReversing polyline will remove curve and fitting information. Continue? <Yes> " ),fs1))==RTERROR || ret2==RTCAN)
						{
						ret=(-__LINE__);
						goto exit;
						}
					else if (ret2==RTNORM) 
						{
						strupr(fs1);
						if (*fs1=='N') 
							goto exit;
						} //if RTNONE do nothing
					if (cmd_decurve_pline(pent,0)!=0) 
						{
						ret=(-__LINE__);
						goto exit;
						}
					}
				if ((ret2=cmd_reverse_pline(pent,NULL))<0) 
					{
					ret=(-__LINE__);
					goto exit;
					}
				}
			else if (strisame(fs1,"FIT"/*DNT*/)) 
				{
				if ((ret2=cmd_pedit_fitcurve(elist,1,fittype,splinesegs,pent))!=RTNORM) 
					{
					if (ret2==RTERROR)
						cmd_ErrorPrompt(CMD_ERR_FITCURVE,0);
					ret=(-__LINE__);
					goto exit;
					}
				poly3d|=IC_PLINE_CURVEFIT;
				}
			else if (strisame(fs1,"SPLINE"/*DNT*/)) //performs a de-fit if necessary
				{
				if ((ret2=cmd_pedit_fitcurve(elist,0,splinetype,splinesegs,pent))!=RTNORM) 
					{
					if (ret2==RTERROR)
						cmd_ErrorPrompt(CMD_ERR_FITCURVE,0);
					ret=(-__LINE__);
					goto exit;
					}
				poly3d|=IC_PLINE_SPLINEFIT;
				}

			}
		}

	exit:
	//only free elist AFTER freeing undo linked lists - it may point to one

	undo_temp=undo_cur=undo_beg;
	while(undo_cur!=NULL) 
		{
		undo_temp=undo_cur->next;
		cmd_free_elistll(&undo_cur->elistll,NULL);
		delete undo_cur;
		undo_cur=NULL;
		undo_cur=undo_temp;
		}

	undo_beg=NULL;
	FreeResbufIfNotNull(&elist);
	FreeResbufIfNotNull(&rbtemp);
	if (originalwaslwpolyline)
	{
		if (PolylineCanBeLwpolyline(pent))
		{
			PutPolylineDataOnExistingLwpolyline(lwpent,pent);
			sds_entdel(pent);
			sds_redraw(lwpent,IC_REDRAW_DRAW);
		}
		else
		{
			sds_entdel(lwpent);  // remove the original lwpolyline
			sds_redraw(pent,IC_REDRAW_DRAW);
		}
	}
	else {
		if (pent[0] && !wasPolyline && plinetype != 0 && PolylineCanBeLwpolyline(pent)) 
			ConvertPolylineToLwpolyline(pent, pent);
	}
		
	if (ret==0)
	{
		return(RTNORM);
	}
	if (ret<0)
		sds_entmake(NULL);
	if (ret==-1) 
		return(RTCAN);
	if (ret<-1)
		CMD_INTERNAL_MSG(ret);
	return(ret);
	}

short cmd_join(void) {

	if (cmd_istransparent() == RTERROR) return RTERROR;

	struct resbuf *elist,*rbp1,*rbptmp,rb;
	sds_real fr1;
	sds_name ent1,ent2,ss;
	sds_point p1,p2,p3,p4,p5,p6,p7;
	char fs1[IC_ACADBUF];
	RT	ret=RTNORM;
	int	fi1,same_ent=0;
	long ssct,parallel=0,colinear=0,coplanar=0,concentric=0,samerad=0,badtype=0;

	rbp1=elist=NULL;

	//angdir is used only for arcs.  for lines, fi1 is a counter
	SDS_getvar(NULL,DB_QANGDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	fi1=rb.resval.rint;


	pick1:
	if (RTNORM!=(ret=sds_entsel(ResourceString(IDC_PEDIT__NSELECT_ARC_OR_L_48, "\nSelect arc or line for joining: " ),ent1,p1)))goto exit;
	FreeResbufIfNotNull(&elist);
	if (NULL==(elist=sds_entget(ent1))) {ret=RTERROR;goto exit;}
	sds_redraw(ent1,IC_REDRAW_HILITE);
	ic_assoc(elist,0);
	if (strsame(ic_rbassoc->resval.rstring,"ARC"/*DNT*/)) {
		if (fi1)
			strncpy(fs1,ResourceString(IDC_PEDIT__NSELECT_ARCS_FOR_49, "\nSelect arcs for clockwise join: " ),sizeof(fs1)-1);
		else
			strncpy(fs1,ResourceString(IDC_PEDIT__NSELECT_ARCS_FOR_50, "\nSelect arcs for counterclockwise join: " ),sizeof(fs1)-1);
		rb.resval.rstring="ARC"/*DNT*/;
	}else if (strsame(ic_rbassoc->resval.rstring,db_hitype2str(DB_LINE))) {
		strncpy(fs1,ResourceString(IDC_PEDIT__NSELECT_LINES_FO_51, "\nSelect lines for attempted join: " ),sizeof(fs1)-1);
		rb.resval.rstring=db_hitype2str(DB_LINE);
	}else{
		cmd_ErrorPrompt(CMD_ERR_ENTTYPE,0);
		sds_redraw(ent1,IC_REDRAW_UNHILITE);
		goto pick1;
	}
	//don't dump ents into prev selection set - they're going to be erased by join
	if (RTNORM!=(ret=sds_pmtssget(fs1,NULL,NULL,NULL,NULL,ss,1)))goto exit;
	//you get single selection mode if you use:
	//		if (RTNORM!=(ret=sds_pmtssget(fs1,"",NULL,NULL,NULL,ss,1)))goto exit;
	//get data for parent entity
	if (strsame(rb.resval.rstring,db_hitype2str(DB_LINE))) {
		ic_assoc(elist,10);
		ic_ptcpy(p1,ic_rbassoc->resval.rpoint);
		ic_assoc(elist,11);
		ic_ptcpy(p2,ic_rbassoc->resval.rpoint);
		if (icadPointEqual(p1,p2)) {
			cmd_ErrorPrompt(CMD_ERR_DIFFPTS,0);
			ret=RTERROR;
			goto exit;
		}
		//make sure line goes from small pt to large pt
		fi1=0;
		if (icadRealEqual(p1[0],p2[0])) {
			if (icadRealEqual(p1[1],p2[1])) {
				if (icadRealEqual(p1[2],p2[2])) {
					ret=RTERROR;goto exit;
				}else if (p1[2]>p1[2]) fi1=1;
			}else if (p1[1]>p2[1])fi1=1;
		}else if (p1[0]>p2[0])fi1=1;
		if (fi1) {
			ic_ptcpy(p3,p1);
			ic_ptcpy(p1,p2);
			ic_ptcpy(p2,p3);
		}
	}else{
		//an arc
		for (rbptmp=elist; rbptmp!=NULL; rbptmp=rbptmp->rbnext) {
			if (rbptmp->restype==10)ic_ptcpy(p1,rbptmp->resval.rpoint);
			else if (rbptmp->restype==40)p2[0]=rbptmp->resval.rreal;
			else if (rbptmp->restype==50)p2[1]=rbptmp->resval.rreal;
			else if (rbptmp->restype==51)p2[2]=rbptmp->resval.rreal;
			else if (rbptmp->restype==210)ic_ptcpy(p3,rbptmp->resval.rpoint);
		}
		rbptmp=NULL;
		ic_normang(&p2[1],&p2[2]);
	}

	for (ssct=0L; RTNORM==sds_ssname(ss,ssct,ent2);ssct++) {
		sds_redraw(ent2,IC_REDRAW_UNHILITE);
		if (ent2[0]==ent1[0] && ent2[1]==ent1[1]) {
			same_ent=1;
			continue;
		}
		FreeResbufIfNotNull(&rbp1);
		if (NULL==(rbp1=sds_entget(ent2))) {ret=RTERROR;goto exit;}
		ic_assoc(rbp1,0);
		if (!strsame(rb.resval.rstring,ic_rbassoc->resval.rstring)) {
			badtype++;
			continue;
		}
		if (strsame(rb.resval.rstring,db_hitype2str(DB_LINE))) {
			ic_assoc(rbp1,10);
			ic_ptcpy(p3,ic_rbassoc->resval.rpoint);
			ic_assoc(rbp1,11);
			ic_ptcpy(p4,ic_rbassoc->resval.rpoint);

			//cross the two vectors.  if reault is non-zero, they're not parallel
			p5[0]=(p2[1]-p1[1])*(p4[2]-p3[2])-(p4[1]-p3[1])*(p2[2]-p1[2]);
			p5[1]=(p4[0]-p3[0])*(p2[2]-p1[2])-(p2[0]-p1[0])*(p4[2]-p3[2]);
			p5[2]=(p2[0]-p1[0])*(p4[1]-p3[1])-(p4[0]-p3[0])*(p2[1]-p1[1]);
			
			if (!icadRealEqual(p5[0],0.0,CMD_FUZZ2) ||
				!icadRealEqual(p5[1],0.0,CMD_FUZZ2) ||
				!icadRealEqual(p5[2],0.0,CMD_FUZZ2)) {
				parallel++;
				continue;
			}
			//they're parallel, but are they collinear?
			fr1=max(fabs(p2[0]-p1[0]),fabs(p2[1]-p1[1]));
			fr1=max(fr1,fabs(p2[2]-p1[2]));
			if (!icadPointEqual(p1,p3)) {
				p5[0]=(p2[1]-p1[1])*(p3[2]-p1[2])-(p3[1]-p1[1])*(p2[2]-p1[2]);
				p5[1]=(p3[0]-p1[0])*(p2[2]-p1[2])-(p2[0]-p1[0])*(p3[2]-p1[2]);
				p5[2]=(p2[0]-p1[0])*(p3[1]-p1[1])-(p3[0]-p1[0])*(p2[1]-p1[1]);
			}else{
				p5[0]=(p1[1]-p2[1])*(p3[2]-p2[2])-(p3[1]-p2[1])*(p1[2]-p2[2]);
				p5[1]=(p3[0]-p2[0])*(p1[2]-p2[2])-(p1[0]-p2[0])*(p3[2]-p2[2]);
				p5[2]=(p1[0]-p2[0])*(p3[1]-p2[1])-(p3[0]-p2[0])*(p1[1]-p2[1]);
			}
			if ((fabs(p5[0])>fr1*CMD_FUZZ)||
				(fabs(p5[1])>fr1*CMD_FUZZ)||
				(fabs(p5[0])>fr1*CMD_FUZZ)) {
				colinear++;
				continue;
			}
			//At this pt, they're both collinear and parallel, so let's DO IT!!!
			fi1=0;
			if (!icadRealEqual(p1[0],p2[0])) {
				if (p3[0]<p4[0]) {
					if (p3[0]<p1[0]) fi1|=1;
					if (p4[0]>p2[0]) fi1|=4;
				}else{
					if (p4[0]<p1[0]) fi1|=2;
					if (p3[0]>p2[0]) fi1|=8;
				}
			}else if (!icadRealEqual(p1[1],p2[1])) {
				if (p3[1]<p4[1]) {
					if (p3[1]<p1[1]) fi1|=1;
					if (p4[1]>p2[1]) fi1|=4;
				}else{
					if (p4[1]<p1[1]) fi1|=2;
					if (p3[1]>p2[1]) fi1|=8;
				}
			}else{
				if (p3[2]<p4[2]) {
					if (p3[2]<p1[2]) fi1|=1;
					if (p4[2]>p2[2]) fi1|=4;
				}else{
					if (p4[2]<p1[2]) fi1|=2;
					if (p3[2]>p2[2]) fi1|=8;
				}
			}
			if (fi1&1)
				ic_ptcpy(p1,p3);
			else if (fi1&2)
				ic_ptcpy(p1,p4);
			if (fi1&4)
				ic_ptcpy(p2,p4);
			else if (fi1&8)
				ic_ptcpy(p2,p3);
			sds_entdel(ent2);
		}else{
			for (rbptmp=rbp1; rbptmp!=NULL; rbptmp=rbptmp->rbnext) {
				if (rbptmp->restype==10)ic_ptcpy(p4,rbptmp->resval.rpoint);
				else if (rbptmp->restype==40)p5[0]=rbptmp->resval.rreal;
				else if (rbptmp->restype==50)p5[1]=rbptmp->resval.rreal;
				else if (rbptmp->restype==51)p5[2]=rbptmp->resval.rreal;
				else if (rbptmp->restype==210)ic_ptcpy(p6,rbptmp->resval.rpoint);
			}
			rbptmp=NULL;
			//check to make sure that extrusions are parallel - use cross product
			ic_crossproduct(p3,p6,p7);
			if ((fabs(p7[0])>CMD_FUZZ)||
				(fabs(p7[1])>CMD_FUZZ)||
				(fabs(p7[0])>CMD_FUZZ)) {
				coplanar++;
				continue;
			}
			if ((p6[0]*p3[0]<-CMD_FUZZ)||
				(p6[1]*p3[1]<-CMD_FUZZ) ||
				(p6[2]*p3[2]<-CMD_FUZZ)) {
				//if extrusions are "anti-parallel"
				if (!icadRealEqual(p1[2],-p4[2])) {
					coplanar++;
					continue;
				}
				if (!icadRealEqual(p1[0],-p4[0]) || !icadRealEqual(p1[1],p4[1])) {
					concentric++;
					continue;
				}
				//alter start and end angles if arcs are anti-parallel
				p7[0]=-p5[1]+IC_PI;
				p5[1]=-p5[2]+IC_PI;
				p5[2]=p7[0];
			}else{
				//parallel extrusions
				if (!icadRealEqual(p1[2],p4[2])) {
					coplanar++;
					continue;
				}
				if (!icadRealEqual(p1[0],p4[0]) || !icadRealEqual(p1[1],p4[1])) {
					concentric++;
					continue;
				}
			}
			//throw away any ents w/o correct radius...
			 if (!icadRealEqual(p2[0],p5[0])) {
				samerad++;
				continue;
			}
			if (0==fi1) {
				 //ccw arc
				 ic_normang(&p2[1],&p5[2]);
				 if (p5[2]>p2[2])p2[2]=p5[2];
			}else{
				 //clockwise arc
				 fr1=p2[2]-p2[1];
				 ic_normang(&p5[1],&p2[2]);
				 if (p2[2]-p5[1]>fr1)p2[1]=p5[1];
				 ic_normang(&p2[1],&p2[2]);

			}
			sds_entdel(ent2);
		}
	}
	if (strsame(rb.resval.rstring,db_hitype2str(DB_LINE))) {
		ic_assoc(elist,10);
		ic_ptcpy(ic_rbassoc->resval.rpoint,p1);
		ic_assoc(elist,11);
		ic_ptcpy(ic_rbassoc->resval.rpoint,p2);
	}else{
		if (fi1) {
			ic_assoc(elist,50);
			ic_rbassoc->resval.rreal=p2[1];
		}else{
			ic_assoc(elist,51);
			ic_rbassoc->resval.rreal=p2[2];
		}
	}
	ret=sds_entmod(elist);



	exit:
	if (ret==RTNORM) {
		if (parallel+colinear+coplanar+concentric+samerad+badtype>0L) {
			sds_printf("\n"/*DNT*/);
#ifdef LOCALISE_ISSUE
// The following string manipulation causes lots of translation grief.
// Suggest biting the bullet & adding another 6 strings..
#endif
			if (badtype)sds_printf(ResourceString(IDC_PEDIT__LI__S_NOT__SS____52, "%li %s not %ss.  " ),badtype,(badtype>1L)?ResourceString(IDC_PEDIT_WERE_53, "were" ):ResourceString(IDC_PEDIT__WAS_54, " was" ),rb.resval.rstring);
			if (parallel)sds_printf(ResourceString(IDC_PEDIT___LI_LINE_S_NOT_P_55, " %li line%s not parallel.  " ),parallel,(parallel>1L)?ResourceString(IDC_PEDIT_S_WERE_56, "s were" ):ResourceString(IDC_PEDIT__WAS_54, " was" ));
			if (colinear)sds_printf(ResourceString(IDC_PEDIT___LI_LINE_S_NOT_C_57, " %li line%s not colinear." ),colinear,(colinear>1L)?ResourceString(IDC_PEDIT_S_WERE_56, "s were" ):ResourceString(IDC_PEDIT__WAS_54, " was" ));
			if (coplanar)sds_printf(ResourceString(IDC_PEDIT___LI_ARC_S_NOT_CO_58, " %li arc%s not coplanar." ),coplanar,(coplanar>1L)?ResourceString(IDC_PEDIT_S_WERE_56, "s were" ):ResourceString(IDC_PEDIT__WAS_54, " was" ));
			if (concentric)sds_printf(ResourceString(IDC_PEDIT___LI_ARC_S_NOT_CO_59, " %li arc%s not concentric." ),concentric,(concentric>1L)?ResourceString(IDC_PEDIT_S_WERE_56, "s were" ):ResourceString(IDC_PEDIT__WAS_54, " was" ));
			if (samerad)sds_printf(ResourceString(IDC_PEDIT___LI_ARC_S_NOT_OF_60, " %li arc%s not of equal radius." ),samerad,(samerad>1L)? ResourceString(IDC_PEDIT_S_WERE_56, "s were" ):ResourceString(IDC_PEDIT__WAS_54, " was" ));
		}
		ssct-=parallel+colinear+coplanar+concentric+samerad+badtype+same_ent;
		sds_printf(ResourceString(IDC_PEDIT__N_LI_OBJECT_S_ME_61, "\n%li object%s merged." ),ssct,(ssct==1L)?""/*DNT*/:"s");
	}
	FreeResbufIfNotNull(&elist);
	FreeResbufIfNotNull(&rbp1);
	sds_ssfree(ss);
	return(ret);
}

int cmd_pline_vtxprev(sds_name pline,sds_name vtx, sds_name vtxprev){
	RT ret;
    sds_name etemp,etemp2;

    if ((ret=sds_entnext_noxref(pline,etemp))==RTNORM) {
		if ((etemp[0]==vtx[0])&&(etemp[1]==vtx[1]))return(-1);
        while(((ret=sds_entnext_noxref(etemp,etemp2))==RTNORM)&&((etemp2[0]!=vtx[0])||(etemp2[1]!=vtx[1]))) {
			ic_encpy(etemp,etemp2);
		}
		if (ret==RTNORM) {
			ic_encpy(vtxprev,etemp);
			ret=0;
		}
	}
	return(ret);
}


int cmd_pedit_fitcurve(struct resbuf *elist,int mode,int fit_type, int splinesegs,sds_name newent) {
	//given pline entity list ELIST, alters pline by fitting or splining.
	//MODE=0:	spline pline - valid fit_types are 1-?? (SPLINETYPE var)
	//MODE=1:	fit pline	 - valid fit_types are 2-3 (FITTYPE var)
	//splinesegs is number of pts per curved section (SPLINESEGS var)
	//NEWENT will be name of the new pline created by routine
	//NOTE: Perfoming a Spline or Fit removes any existing spline or fit vertices!
	//RETURNS: RTNORM if successful, RTREJ if trying to fit too many vertices on one pline,
	//			RTERROR if some other error, or  -__LINE__ number if internal error
	struct resbuf *rbtemp,*rb10,*rb70,*rb40,*rb41;;
	sds_name pent,etemp1;
	sds_point p0;
	long vtxcnt,fl1,fl2,newvtxcnt;
	sds_point *ptarray_in,*ptarray_out;
	sds_real *widarray_in,lastwidth,totlen;
	RT ret=RTNORM,poly3d;

	widarray_in=NULL;
	ptarray_in=ptarray_out=NULL;
	rbtemp=rb10=rb70=rb40=rb41=NULL;

	//step thru pline and find out how many vertices we have which
	// have not been added by splining or fitting

	ic_assoc(elist,70);
	poly3d=ic_rbassoc->resval.rint;
	ic_assoc(elist,-1);
	ic_encpy(pent,ic_rbassoc->resval.rlname);
    sds_entnext_noxref(pent,etemp1);
	vtxcnt=0;
	do{
		FreeResbufIfNotNull(&rbtemp);
        if ((rbtemp=sds_entget(etemp1))==NULL) {ret=(-__LINE__);goto exit;}
		ic_assoc(rbtemp,0);
		if (strsame(ic_rbassoc->resval.rstring,"SEQEND"/*DNT*/)) {
            sds_relrb(rbtemp); rbtemp=NULL;
			break;
		}
		ic_assoc(rbtemp,70);
		if ((ic_rbassoc->resval.rint & (IC_VERTEX_FROMFIT | IC_VERTEX_SPLINEVERTEX))==0) {
			vtxcnt++;
		}
    }while(sds_entnext_noxref(etemp1,etemp1)==RTNORM);
	if (vtxcnt<3) {
		ret=RTERROR;
		goto exit;
	}
	if (poly3d & IC_PLINE_CLOSED)vtxcnt++;

	//allocate an array of points to pass to routine for interpolation
	ptarray_in= new sds_point [vtxcnt+1] ;
	memset(ptarray_in,0,sizeof(sds_point)*vtxcnt+1);
	widarray_in= (sds_real *)new sds_point [2*(vtxcnt+1)] ;
	memset(widarray_in,0,sizeof(sds_point )*2*(vtxcnt+1));
	//walk the pline and populate the points in the structure
    sds_entnext_noxref(pent,etemp1);
	fl1=0;
	do{
		FreeResbufIfNotNull(&rbtemp);
        if ((rbtemp=sds_entget(etemp1))==NULL) {ret=(-__LINE__);goto exit;}
		if (0!=ic_assoc(rbtemp,70))break;
		if ((ic_rbassoc->resval.rint & (IC_VERTEX_FROMFIT | IC_VERTEX_SPLINEVERTEX))==0) {
			for (rb10=rbtemp; rb10!=NULL; rb10=rb10->rbnext) {
				//note: first catch here for "VERTEX" should never happen...
				if (rb10->restype==0 && !strsame(rb10->resval.rstring,"VERTEX"/*DNT*/))break;
				else if (rb10->restype==10)ic_ptcpy(ptarray_in[fl1],rb10->resval.rpoint);
				else if (rb10->restype==40)widarray_in[2*fl1]=rb10->resval.rreal;
				else if (rb10->restype==41)widarray_in[2*fl1+1]=rb10->resval.rreal;
			}
			fl1++;
			//NOTE: rb10 should be set back to NULL
		}
		if (1==(vtxcnt-fl1) && (poly3d & IC_PLINE_CLOSED))break;
    }while(sds_entnext_noxref(etemp1,etemp1)==RTNORM && fl1<=vtxcnt);
	if (poly3d & IC_PLINE_CLOSED) {
		ic_ptcpy(ptarray_in[fl1],ptarray_in[0]);
		widarray_in[2*fl1]  =widarray_in[0];
		widarray_in[2*fl1+1]=widarray_in[1];
		fl1++;
	}

	if (fl1!=vtxcnt) {ret=RTERROR;goto exit;}

// Change Alert: The exact value of newvtxcnt is now computed inside
// Geo spline functions

	rb70=rb10=NULL;
	if (mode!=0) {	//a fit...
		gr_interp_spline(vtxcnt,ptarray_in,splinesegs,&newvtxcnt,&ptarray_out);

		//we don't need ptarray_in, and it's early in the game, so free it
		if (ptarray_in!=NULL) {IC_FREE(ptarray_in);ptarray_in=NULL;}
		//Make new main ent...
		ic_assoc(elist,70);
		ic_rbassoc->resval.rint&=(~IC_PLINE_SPLINEFIT);//set spline bit off
		ic_rbassoc->resval.rint|=IC_PLINE_CURVEFIT;
		if (0==ic_assoc(elist,75))ic_rbassoc->resval.rint=0;
        if (sds_entmake(elist)!=RTNORM) {ret=RTERROR;goto exit;}
		FreeResbufIfNotNull(&rbtemp);
		if ((rbtemp=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,10,p0,70,1,42,0.0,40,0.0,41,0.0,0))==NULL) {ret=(-__LINE__);goto exit;}
		ic_assoc(rbtemp,10);
		rb10=ic_rbassoc;
		ic_assoc(rbtemp,70);
		rb70=ic_rbassoc;
		ic_assoc(rbtemp,40);
		rb40=ic_rbassoc;
		ic_assoc(rbtemp,41);
		rb41=ic_rbassoc;
		lastwidth=widarray_in[0];
		for (fl1=fl2=0;fl1<newvtxcnt;fl1++) {
			ic_ptcpy(rb10->resval.rpoint,ptarray_out[fl1]);
			if (0==(fl1%splinesegs) || (!(poly3d & IC_PLINE_CLOSED) && fl1==(newvtxcnt-1)))
				rb70->resval.rint=0;
			else
				rb70->resval.rint=IC_VERTEX_FROMFIT;
			//calc starting/ending width
			if (fl1>0 && 0==(fl1%splinesegs)) fl2++;
			if (0==(fl1%splinesegs)) rb40->resval.rreal=widarray_in[2*fl2];//beginning of segment
			else rb40->resval.rreal=lastwidth;
			if ((splinesegs-1)==(fl1%splinesegs))lastwidth=widarray_in[2*fl2+1];//end of segment
			else lastwidth=widarray_in[2*fl2]+(widarray_in[2*fl2+1]-widarray_in[2*fl2])*((fl1%splinesegs)+1)/splinesegs;
			rb41->resval.rreal=lastwidth;
			if (sds_entmake(rbtemp)!=RTNORM) {ret=RTERROR;goto exit;}
		}
	}else{	//make the splined vertices as follows:
				//make main ent and starting vtx w/bit 4 (16) set
				//make ALL splined vertices w/bit 3 (8) set
				//make all remaining control vertices
		if (gr_raw_spline(vtxcnt,ptarray_in,splinesegs,&newvtxcnt,&ptarray_out)) {
			cmd_ErrorPrompt(CMD_ERR_PLINESEGS,0);
			ret=RTREJ;
			goto exit;
			}

		//calculate the length of the newly splined pline...
		totlen=0.0;
		for (fl1=0;fl1<newvtxcnt;fl1++) {
			if ((newvtxcnt-fl1)==1 && (poly3d & IC_PLINE_CLOSED))break;
			totlen+=sds_distance(ptarray_out[fl1],ptarray_out[fl1+1]);
		}
		if (icadRealEqual(totlen,0.0)) {
			ret=RTERROR;goto exit;
		}
		//Make new main ent...
		ic_assoc(elist,70);
		ic_rbassoc->resval.rint&=(~IC_PLINE_CURVEFIT);
		ic_rbassoc->resval.rint|=IC_PLINE_SPLINEFIT;
		if (0==ic_assoc(elist,75))ic_rbassoc->resval.rint=0;
        if (sds_entmake(elist)!=RTNORM) {ret=(-__LINE__);goto exit;}
		FreeResbufIfNotNull(&rbtemp);
		if ((rbtemp=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,10,ptarray_in[0],70,16,42,0.0,40,widarray_in[0],41,widarray_in[1],0))==NULL) {ret=RTERROR;goto exit;}
		//make 1st vertex as copy of original 1st vertex...
		if (sds_entmake(rbtemp)!=RTNORM) {ret=(-__LINE__);goto exit;}
		ic_assoc(rbtemp,10);
		rb10=ic_rbassoc;
		ic_assoc(rbtemp,70);
		rb70=ic_rbassoc;
		rb70->resval.rint=IC_VERTEX_SPLINEVERTEX;//70 flag for next set of vertices to be 8
		ic_assoc(rbtemp,40);
		rb40=ic_rbassoc;
		//rb40->resval.rreal=0.0;//start width for next set of vertices to be 0.0
		ic_assoc(rbtemp,41);
		rb41=ic_rbassoc;
		//rb41->resval.rreal=0.0;//end width for next set of vertices to be 0.0
		//for any seg, width=start wid+distsofar*width diff/total length
		totlen=(widarray_in[2*vtxcnt-1]-widarray_in[0])/totlen;
		lastwidth=widarray_in[0];
		for (fl1=0;fl1<newvtxcnt;fl1++) {	 //start w/1st point in new array
			ic_ptcpy(rb10->resval.rpoint,ptarray_out[fl1]);
			rb40->resval.rreal=lastwidth;
			if (newvtxcnt-fl1>1) {
				lastwidth+=totlen*sds_distance(ptarray_out[fl1],ptarray_out[fl1+1]);
			}
			rb41->resval.rreal=lastwidth;
			if (sds_entmake(rbtemp)!=RTNORM) {ret=RTERROR;goto exit;}
		}
		if (poly3d & IC_PLINE_CLOSED)vtxcnt--;//remove duplicate vtx at start/end pt
		rb70->resval.rint=IC_VERTEX_SPLINEFRAME;
		for (fl1=1;fl1<vtxcnt;fl1++) {
			ic_ptcpy(rb10->resval.rpoint,ptarray_in[fl1]);
			rb40->resval.rreal=widarray_in[2*fl1];
			rb41->resval.rreal=widarray_in[2*fl1+1];
			if (sds_entmake(rbtemp)!=RTNORM) {ret=RTERROR;goto exit;}
		}
	}
	FreeResbufIfNotNull(&rbtemp);
	rb10=rb70=NULL;
	if ((rbtemp=sds_buildlist(RTDXF0,"SEQEND"/*DNT*/,0))==NULL) {ret=RTERROR;goto exit;}
	if (sds_entmake(rbtemp)!=RTNORM) {ret=(-__LINE__);goto exit;}
	FreeResbufIfNotNull(&rbtemp);
    sds_entdel(pent);
    sds_entlast(newent);
	sds_redraw(newent,IC_REDRAW_DRAW);
	exit:
	//NOTE: Do NOT free elist - it's allocated by calling routine
	if (ptarray_in!=NULL)  IC_FREE(ptarray_in);
	if (ptarray_out!=NULL) IC_FREE(ptarray_out);
	if (widarray_in!=NULL) IC_FREE(widarray_in);
	FreeResbufIfNotNull(&rbtemp);
	return(ret);

}

int cmd_decurve_pline(sds_name pent, int mode) {
	//NOTE: name of entity will change!
	//If mode==0, remove spline/fit vertices and set their pline flags off
	//If mode==1, set all bulges to 0.0
	struct resbuf *rbtemp=NULL,*rb2make=NULL, rb;
	RT ret=0,plflag;
	sds_name etemp1,etemp2,etemp3;
	sds_real lastwid;
	char seqend=0;

	if (1==mode) {
        sds_entnext_noxref(pent,etemp1);
		do{
			FreeResbufIfNotNull(&rbtemp);
            if ((rbtemp=sds_entget(etemp1))==NULL) {ret=(-__LINE__);goto exit;}
			ic_assoc(rbtemp,0);
			if (strsame(ic_rbassoc->resval.rstring,"SEQEND"/*DNT*/))break;
			if (!ic_assoc(rbtemp,42))ic_rbassoc->resval.rreal=0.0;
            if (sds_entmod(rbtemp)!=RTNORM) {
				ret=(-__LINE__);
				goto exit;
			}
        }while(sds_entnext_noxref(etemp1,etemp1)==RTNORM);
		sds_entupd(pent);
	}else if (0==mode) {

		rb.restype=RTSTR;
		rb.resval.rstring=const_cast<char*>("*"/*DNT*/);
		sds_entmake(NULL);
		rb2make=sds_entgetx(pent,&rb);
		ic_assoc(rb2make,70);
		plflag=ic_rbassoc->resval.rint;
		ic_rbassoc->resval.rint&=~(IC_PLINE_CURVEFIT | IC_PLINE_SPLINEFIT);//set spline & fit flags off
		if (RTNORM!=(ret=sds_entmake(rb2make)))goto exit;
		sds_entnext_noxref(pent,etemp1);
		do{
			if (plflag & IC_PLINE_SPLINEFIT) {	//de-spline
				FreeResbufIfNotNull(&rb2make);
				rb2make=sds_entgetx(etemp1,&rb);
				ic_assoc(rb2make,0);
				if (strsame(ic_rbassoc->resval.rstring,"SEQEND"/*DNT*/)) {
					ret=sds_entmake(rb2make);
					goto exit;
				}
				ic_assoc(rb2make,70);
				if (ic_rbassoc->resval.rint & IC_VERTEX_SPLINEFRAME) {
					ic_rbassoc->resval.rint &= ~IC_VERTEX_SPLINEFRAME;
					if (RTNORM!=(ret=sds_entmake(rb2make)))goto exit;
				}
			}else if (plflag & IC_PLINE_CURVEFIT) {	//de-fit
				FreeResbufIfNotNull(&rb2make);
				rb2make=sds_entgetx(etemp1,&rb);
				ic_assoc(rb2make,70);
				if (ic_rbassoc->resval.rint & IC_VERTEX_FROMFIT)continue;//should never happen
				ic_encpy(etemp2,etemp1);
				for (;;) {
					sds_entnext_noxref(etemp2,etemp2);
					FreeResbufIfNotNull(&rbtemp);
					rbtemp=sds_entget(etemp2);
					ic_assoc(rbtemp,0);
					if (strsame(ic_rbassoc->resval.rstring,"SEQEND"/*DNT*/)) {
						seqend=1;
						if (!(plflag & IC_PLINE_CLOSED)) {
							if (RTNORM!=sds_entmake(rb2make))goto exit;
							sds_entmake(rbtemp);
							goto exit;
						}else break;
					}
					ic_assoc(rbtemp,70);
					if (!(ic_rbassoc->resval.rint & IC_VERTEX_FROMFIT))break;
					ic_assoc(rbtemp,41);
					lastwid=ic_rbassoc->resval.rreal;
					ic_encpy(etemp3,etemp2);
				}
				ic_assoc(rb2make,41);
				ic_rbassoc->resval.rreal=lastwid;
				if (RTNORM!=(ret=sds_entmake(rb2make)))goto exit;
				if (1==seqend) {//closed pline only
					ret=sds_entmake(rbtemp);//make seqend
					goto exit;
				}
				ic_encpy(etemp1,etemp3);
			}

		}while(RTNORM==sds_entnext_noxref(etemp1,etemp1));

	}
exit:
	FreeResbufIfNotNull(&rbtemp);
	FreeResbufIfNotNull(&rb2make);
	if (ret==RTNORM)ret=0;
	if (0==mode && ret>=0) {
		sds_entdel(pent);
		sds_entlast(pent);
		sds_redraw(pent,IC_REDRAW_DRAW);
	}
	return(ret);
}

int cmd_taper_pline(sds_name pent, sds_real stwidth, sds_real endwidth){
	RT	ret=0;
	int	fi1,got1;
	short closure,nverts;
	struct resbuf *rbp1,*rbp2,*rbptemp;
    sds_real totlen,lensofar,lastwidth,fr1,rr,sa,ea;
    sds_name etemp1,etemp2;
    sds_point p1,p2,cc;

	rbp1=rbp2=rbptemp=NULL;
	//NOTE: If pline has been splined, NVERTS will include splined AND control
	//	vertices, even though TOTLEN is the length of the spline only.
	db_edata(pent,&closure,&nverts,&totlen,NULL);
	ic_encpy(etemp1,pent);
	FreeResbufIfNotNull(&rbp1);
	if ((rbp1=sds_entget(etemp1))==NULL) {ret=-1;goto exit;}
	ic_assoc(rbp1,70);
	if (ic_rbassoc->resval.rint & IC_PLINE_SPLINEFIT) {
		//splined pline....
		FreeResbufIfNotNull(&rbp1);
		if (sds_entnext_noxref(etemp1,etemp1)!=RTNORM) {ret=-1;goto exit;} //1st vtx
		ic_encpy(etemp2,etemp1);
		nverts=0;
		for (;;) {
			if (sds_entnext_noxref(etemp2,etemp2)!=RTNORM) {ret=-1;goto exit;}
			FreeResbufIfNotNull(&rbp1);
			if ((rbp1=sds_entget(etemp2))==NULL) {ret=-1;goto exit;}
			if (ic_assoc(rbp1,70)!=0)break;
			if (ic_rbassoc->resval.rint&8)nverts++;
			else break;
		}
	}
	FreeResbufIfNotNull(&rbp1);
	lensofar=0.0;
	lastwidth=stwidth;
	if (closure)
		fi1=0;
	else
		fi1=1;
	for (;fi1<nverts;fi1++) {
        if (sds_entnext_noxref(etemp1,etemp1)!=RTNORM) {ret=-1;goto exit;}
		if (rbp1==NULL) {
            if ((rbp1=sds_entget(etemp1))==NULL) {ret=-1;goto exit;}
		}else{
            sds_relrb(rbp1); rbp1=NULL;
			rbp1=rbp2;
		}
		if (cmd_pline_entnext(etemp1,etemp2,closure))break;
		rbp2=NULL;
        if ((rbp2=sds_entget(etemp2))==NULL) {ret=-1;goto exit;}
		got1=0;
		for (rbptemp=rbp1; rbptemp!=NULL; rbptemp=rbptemp->rbnext) {
			if (rbptemp->restype==10)ic_ptcpy(p1,rbptemp->resval.rpoint);
			else if (rbptemp->restype==42)fr1=rbptemp->resval.rreal;
			else if (rbptemp->restype==40) {
				got1=1;
				rbptemp->resval.rreal=lastwidth;
			}
		}
		if (got1==0) {
			for (rbptemp=rbp1; rbptemp->rbnext!=NULL; rbptemp=rbptemp->rbnext);
            if ((rbptemp->rbnext=sds_buildlist(40,lastwidth,0))==NULL) {ret=-1;goto exit;}
			rbptemp=NULL;
		}
 		ic_assoc(rbp2,10);
		ic_ptcpy(p2,ic_rbassoc->resval.rpoint);
		if (icadRealEqual(fr1,0.0)) {
            lensofar+=sds_distance(p1,p2);
		}else{
			ic_bulge2arc(p1,p2,fr1,cc,&rr,&sa,&ea);
			ic_normang(&sa,&ea);
			lensofar+=(rr*(ea-sa));
		}
		lastwidth=stwidth+(endwidth-stwidth)*lensofar/totlen;
		if (ic_assoc(rbp1,41)) {
			for (rbptemp=rbp1; rbptemp->rbnext!=NULL; rbptemp=rbptemp->rbnext);
            if ((rbptemp->rbnext=sds_buildlist(41,0.0,0))==NULL) {ret=-1;goto exit;}
			ic_rbassoc=rbptemp->rbnext;
			rbptemp=NULL;
		}
		ic_rbassoc->resval.rreal=lastwidth;
        if (sds_entmod(rbp1)!=RTNORM) {ret=-1;goto exit;}
	}
	exit:
    if (sds_entupd(pent)!=RTNORM)ret=-1;
	FreeResbufIfNotNull(&rbp1);
	FreeResbufIfNotNull(&rbp2);
	return(ret);
}

int cmd_join_pline(sds_name polyent, sds_name lwpolyent, sds_name ss) {
	//RETURNS: 0 IF SUCCESSFUL
	//		  <0 ERROR
	//Function assumes polyent is an OPEN pline, and will only
	//	join it w/lines, arcs, & other plines.  Opens closed plines
	//  before joining them to polyent.
	//NOTE: Function may alter value in polyent if new ename req'd
	//	during entmake process

	//NOTE:This join DOES NOT remove spline & fit vertices - caller must
	//do this.  It adds all vertices to the pline that it possibly can.

    sds_name joinent,ent,etemp1,etemp2,ent2del;
	RT	ret=0;
	int	fi1,didjoin,badents;
	long i,addct,addflag=0;	//addflag counts # of vertices added to pline - NOT segments
    sds_real fr1,fr2,fr3;
	struct resbuf *rbp1, *rbptemp,rbpoly,rbwcs,rbtemp;
    sds_point startpt,endpt,p0,p1,p2;
	db_handitem *hip;
	bool pl_2dto3d;

	rbp1=NULL;

	rbwcs.restype=RTSHORT;
	rbwcs.resval.rint=0;

	hip=(db_handitem *)polyent[0];
	hip->get_70(&fi1);
	if ((fi1 & IC_PLINE_POLYFACEMESH) || (fi1 & IC_PLINE_3DMESH)) {
		//can't join to mesh or pface
		return(-1);
	}
	if (fi1 & IC_PLINE_3DPLINE) {
		rbpoly.restype=RTSHORT;
		rbpoly.resval.rint=0;
	}else{
		rbpoly.restype=RT3DPOINT;
		hip->get_210(rbpoly.resval.rpoint);
	}
	rbtemp.restype=RT3DPOINT;//use for arcs & 2d plines to join


	ic_encpy(joinent,polyent);
	cmd_get_startend(joinent,startpt,endpt);
	addct=0;
	badents=0;
	do{
		didjoin=0;
        for (i=0L;sds_ssname(ss,i,ent)==RTNORM;i++) {
			ic_encpy(ent2del,ent);//we do this because pline's name changes
			if ((ent[1]==joinent[1] && ent[0]==joinent[0]) || (ent[1]==lwpolyent[1] && ent[0]==lwpolyent[0])) {
				sds_ssdel(ent2del,ss);
				i--;
				continue;
			}
			FreeResbufIfNotNull(&rbp1);

			// Some of the entities may have been turned into Polylines already by now (like the start entity),
			// in which case sds_entget() fails.  Just continue on
			//
			if ((rbp1=sds_entget(ent))==NULL)
				{
				sds_ssdel(ent2del,ss);
				i--;
				continue;
				}

			ic_assoc(rbp1,0);

			// DP: This code is allowing to join to polylines lwpolylines.
			// DP: I don't think that it's best solution of problem but ...
			// Begin
			if (strsame(ic_rbassoc->resval.rstring,db_hitype2str(DB_LWPOLYLINE)))
			{
				sds_name lwpline;
				ic_encpy(lwpline, ent);
				if (ConvertLwpolylineToPolyline(ent,ent)!=RTNORM)
				{
					badents++;
					continue;
				}
				sds_entdel(lwpline);
				FreeResbufIfNotNull(&rbp1);

				if ((rbp1=sds_entget(ent))==NULL)
				{
					sds_ssdel(ent2del,ss);
					i--;
					continue;
				}

				ic_assoc(rbp1,0);
			}
			// End

			if (strsame(ic_rbassoc->resval.rstring,db_hitype2str(DB_LINE))) {
				ic_assoc(rbp1,10);
				ic_ptcpy(p1,ic_rbassoc->resval.rpoint);
				ic_assoc(rbp1,11);
				ic_ptcpy(p2,ic_rbassoc->resval.rpoint);
				if (rbpoly.restype=RT3DPOINT) {
					sds_trans(p1,&rbwcs,&rbpoly,0,p1);
					sds_trans(p2,&rbwcs,&rbpoly,0,p2);
					//line must be XY planar in ECS system
					if (!icadRealEqual(p1[2],p2[2])) {
						badents++;
						continue;
					}
				}
				if ((icadPointEqual(p1,startpt)&&icadPointEqual(p2,endpt))||
				   (icadPointEqual(p2,startpt)&&icadPointEqual(p1,endpt))) {
				   //just close pline & exit joining process.
				   //can't join anything once pline is closed....
					FreeResbufIfNotNull(&rbp1);
                    if ((rbp1=sds_entget(joinent))==NULL) {ret=(-1);goto exit;}
					ic_assoc(rbp1,70);
					ic_rbassoc->resval.rint |= IC_PLINE_CLOSED;
					if (sds_entmod(rbp1)!=RTNORM) {ret=(-1);goto exit;}

					/*D.Gavrilov*/// now we should set to zero the bulge of the last vertex
					cmd_pline_next2lastvtx(joinent, etemp1);
	                sds_entnext_noxref(etemp1, etemp1);
					FreeResbufIfNotNull(&rbp1);
			        if((rbp1 = sds_entget(etemp1)) == NULL)
						{ ret=(-1); goto exit; }
					ic_assoc(rbp1, 42);
					ic_rbassoc->resval.rreal = 0.0;
                    if(sds_entmod(rbp1) != RTNORM)
						{ ret=(-1); goto exit; }

                    sds_printf(ResourceString(IDC_PEDIT__NPOLYLINE_IS_NOW_62, "\nPolyline is now closed." ));
                    sds_entdel(ent);
                    sds_entupd(joinent);
					goto done_joining;
				}
				if (icadPointEqual(p1,startpt)) {
					cmd_join_seg2start(joinent,p2,0.0);
					ic_ptcpy(startpt,p2);
					addflag=1;
				}else if (icadPointEqual(p2,startpt)) {
					cmd_join_seg2start(joinent,p1,0.0);
					ic_ptcpy(startpt,p1);
					addflag=1;
				}else if (icadPointEqual(p1,endpt)) {
					cmd_join_seg2end(joinent,p2,0.0);
					ic_ptcpy(endpt,p2);
					addflag=1;
				}else if (icadPointEqual(p2,endpt)) {
					cmd_join_seg2end(joinent,p1,0.0);
					ic_ptcpy(endpt,p1);
					addflag=1;
				}
			}else if (strsame(ic_rbassoc->resval.rstring,"ARC"/*DNT*/)) {
				//can't join arc to 3d poly
				if (rbpoly.restype==RTSHORT) {
					badents++;
					continue;
				}

				for (rbptemp=rbp1; rbptemp!=NULL; rbptemp=rbptemp->rbnext) {
					if (rbptemp->restype==10)ic_ptcpy(p0,rbptemp->resval.rpoint);
					else if (rbptemp->restype==40)fr1=rbptemp->resval.rreal;
					else if (rbptemp->restype==50)fr2=rbptemp->resval.rreal;
					else if (rbptemp->restype==51)fr3=rbptemp->resval.rreal;
					else if (rbptemp->restype==210)ic_ptcpy(rbtemp.resval.rpoint,rbptemp->resval.rpoint);
				}
				//if joining to 2d poly and extrusions not equal, continue;
				if (!icadPointEqual(rbtemp.resval.rpoint,rbpoly.resval.rpoint)) {
					p1[0]=-rbtemp.resval.rpoint[0];
					p1[1]=-rbtemp.resval.rpoint[1];
					p1[2]=-rbtemp.resval.rpoint[2];
					if (!icadPointEqual(p1,rbpoly.resval.rpoint)) {
						badents++;
						continue;
					}else{
						//antiparallel
						p0[0]=-p0[0];
						p0[2]=-p0[2];
						//reverse start & and and subtract 'em from 3pi
						rbtemp.resval.rpoint[0]=(3.0*IC_PI)-fr3;
						fr3=(3.0*IC_PI)-fr2;
						fr2=rbtemp.resval.rpoint[0];
						ic_ptcpy(rbtemp.resval.rpoint,p1);
					}
				}

                sds_polar(p0,fr2,fr1,p1);
                sds_polar(p0,fr3,fr1,p2);
				ic_normang(&fr2,&fr3);
				fr1=tan((fr3-fr2)/4.0);
				if ((icadPointEqual(p1,startpt)&&icadPointEqual(p2,endpt))||
				   (icadPointEqual(p2,startpt)&&icadPointEqual(p1,endpt))) {
				   //just close pline & exit joining process.
				   //can't join anything once pline is closed....
					FreeResbufIfNotNull(&rbp1);
                    if ((rbp1=sds_entget(joinent))==NULL) {ret=(-1);goto exit;}
					ic_assoc(rbp1,70);
					ic_rbassoc->resval.rint |= IC_PLINE_CLOSED;
                    if (sds_entmod(rbp1)!=RTNORM) {ret=(-1);goto exit;}
					cmd_pline_next2lastvtx(joinent,etemp1);
                    sds_entnext_noxref(etemp1,etemp1);
					FreeResbufIfNotNull(&rbp1);
                    if ((rbp1=sds_entget(etemp1))==NULL) {ret=(-1);goto exit;}
					ic_assoc(rbp1,42);
					if (icadPointEqual(p1,endpt))
						ic_rbassoc->resval.rreal=fr1;
					else
						ic_rbassoc->resval.rreal=-fr1;
                    if (sds_entmod(rbp1)!=RTNORM) {ret=(-1);goto exit;}
                    sds_printf(ResourceString(IDC_PEDIT__NPOLYLINE_IS_NOW_62, "\nPolyline is now closed." ));
                    sds_entdel(ent);
                    sds_entupd(joinent);
					goto done_joining;
				}
				if (icadPointEqual(p1,startpt)) {
					cmd_join_seg2start(joinent,p2,-fr1);
					ic_ptcpy(startpt,p2);
					addflag=1;
				}else if (icadPointEqual(p2,startpt)) {
					cmd_join_seg2start(joinent,p1,fr1);
					ic_ptcpy(startpt,p1);
					addflag=1;
				}else if (icadPointEqual(p1,endpt)) {
					cmd_join_seg2end(joinent,p2,fr1);
					ic_ptcpy(endpt,p2);
					addflag=1;
				}else if (icadPointEqual(p2,endpt)) {
					cmd_join_seg2end(joinent,p1,-fr1);
					ic_ptcpy(endpt,p1);
					addflag=1;
				}
			}else if (strsame(ic_rbassoc->resval.rstring,"POLYLINE"/*DNT*/)) {
				pl_2dto3d=FALSE;//flag for joining 2d poly to 3d
				//join pline to pline
				ic_assoc(rbp1,70);
				fi1=ic_rbassoc->resval.rint;
				if (fi1&(IC_PLINE_SPLINEFIT | IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH)) {
					//can't join 3d pline to 2d
					if (rbpoly.restype==RT3DPOINT) {
						badents++;
						continue;
					}
				}else{
					//can't join 2d polylines w/unequal extrusions.
					ic_assoc(rbp1,210);
					if (rbpoly.restype==RT3DPOINT && !icadPointEqual(ic_rbassoc->resval.rpoint,rbpoly.resval.rpoint)) {
						badents++;
						continue;
					}
					ic_ptcpy(rbtemp.resval.rpoint,ic_rbassoc->resval.rpoint);
					if (rbpoly.restype==RTSHORT)pl_2dto3d=TRUE;//set flag for joining 2d poly to 3d
				}

				//can't use cmd_get_startend(ent,p1,p2), which will set both
				//pts to start if pline is closed.  Have to get pts by hand...

				if (fi1&(IC_PLINE_CURVEFIT | IC_PLINE_SPLINEFIT)) {
					//NOTE: decurve routine alters name of ent!
					cmd_decurve_pline(ent,0);
					sds_entlast(ent);
				}
				ic_encpy(etemp2,ent);
				fi1=0;
				do{
					ic_encpy(etemp1,etemp2);
                    sds_entnext_noxref(etemp2,etemp2);
					FreeResbufIfNotNull(&rbp1);
                    if ((rbp1=sds_entget(etemp2))==NULL) {ret=(-1);goto exit;}
					if (fi1==0) {
						ic_assoc(rbp1,10);
						ic_ptcpy(p1,ic_rbassoc->resval.rpoint);
						if (pl_2dto3d)sds_trans(p1,&rbtemp,&rbpoly,0,p2);
						fi1=1;
					}
					ic_assoc(rbp1,0);
				}while(strsame(ic_rbassoc->resval.rstring,"VERTEX"/*DNT*/));
				FreeResbufIfNotNull(&rbp1);
                if ((rbp1=sds_entget(etemp1))==NULL) {ret=(-1);goto exit;}
				ic_assoc(rbp1,10);
				ic_ptcpy(p2,ic_rbassoc->resval.rpoint);
				if (pl_2dto3d)sds_trans(p2,&rbtemp,&rbpoly,0,p2);
				//we now have start point and "endpoint" we'll use...
				if ((icadPointEqual(p1,startpt)&&icadPointEqual(p2,endpt))||
				   (icadPointEqual(p2,startpt)&&icadPointEqual(p1,endpt))) {
					//just close pline & exit joining process.
					//can't join anything once pline is closed....
					if (icadPointEqual(p1,startpt)&&icadPointEqual(p2,endpt)) {
						cmd_reverse_pline(ent,NULL);
						cmd_join_plines2close(joinent,ent,&addflag);
					}else
						cmd_join_plines2close(joinent,ent,&addflag);
                    sds_printf(ResourceString(IDC_PEDIT__NPOLYLINE_IS_NOW_62, "\nPolyline is now closed." ));
					addct+=addflag;
                    sds_entdel(joinent);
                    sds_entlast(joinent);
                    sds_entdel(ent);
					goto done_joining;
				}
				if (icadPointEqual(p1,startpt)) {
					cmd_reverse_pline(ent,NULL);
					if (ret=cmd_join_plend2start(ent,joinent,&addflag,1)) goto exit;
					ic_ptcpy(startpt,p2);
				}else if (icadPointEqual(p2,startpt)) {
					if (ret=cmd_join_plend2start(ent,joinent,&addflag,1)) goto exit;
					ic_ptcpy(startpt,p1);
				}else if (icadPointEqual(p1,endpt)) {
					if (ret=cmd_join_plend2start(joinent,ent,&addflag,0)) goto exit;
					ic_ptcpy(endpt,p2);
				}else if (icadPointEqual(p2,endpt)) {
					cmd_reverse_pline(ent,NULL);
					if (ret=cmd_join_plend2start(joinent,ent,&addflag,0)) goto exit;
					ic_ptcpy(endpt,p1);
				}
			}
			sds_redraw(ent,IC_REDRAW_UNHILITE);
			if (addflag) {
				addct+=addflag;
                sds_entdel(joinent);
                sds_entlast(joinent);
				sds_ssdel(ent2del,ss);
                sds_entdel(ent);
				i--;
				addflag=0;
				didjoin=1;
			}
		}
		if (sds_sslength(ss,&i)!=RTNORM)i=0L;
	}while(didjoin==1 && i>0L);
	done_joining:
    sds_printf(ResourceString(IDC_PEDIT__N_LI__S_ADDED_TO_63, "\n%li %s added to polyline." ),addct,(addct==1)?ResourceString(IDC_PEDIT_VERTEX_64, "vertex" ):ResourceString(IDC_PEDIT_VERTICES_65, "vertices" )); 
	exit:
	//don't free ss - let caller do it
	ic_encpy(polyent,joinent);
	sds_redraw(polyent,IC_REDRAW_DRAW);
	FreeResbufIfNotNull(&rbp1);
	return(ret);
}

int cmd_reverse_pline(sds_name pline,sds_name vtxent) 
	{

	//this command reverses the order of the pline entity pline.  If vtxent!=NULL,
	//the command sets vtxent to the new name of the vertex spanning the same section
	//after the reversal.  NOTE: this vertex won't have the same point - it will actually
	//be the vertex AFTER it in the original order.

	//Returns -1 or -__LINE_ if error
	//		   0 if OK
	//		   1 if OK AND the new entity is the closing segment on a closed pline
    sds_name e1;
	struct resbuf rb,*rbp1,*rbp2,*rbptemp;
    sds_real bulge1,bulge2,stwid1,stwid2,endwid1,endwid2;
	RT	ret=0;
	int	closure,color1,color2,foundflag;
	struct cmd_elistll *elistll_cur,*elistll_beg,*elistll_tmp;
	long fl1,vtxno,vtxcnt;

	rbp1=rbp2=NULL;
	elistll_cur=elistll_beg=elistll_tmp=NULL;

	rb.rbnext=NULL;
	rb.restype=RTSTR;
	rb.resval.rstring=const_cast<char*>("*"/*DNT*/);

	ic_encpy(e1,pline);
	vtxcnt=vtxno=0;
	bulge1=bulge2=stwid1=stwid2=endwid1=0.0;
	color1=color2=256;
	foundflag=0;
	for (;;)
		{
		//build linked list in reverse order correcting bulges as we go....
        if (sds_entnext_noxref(e1,e1)!=RTNORM) {ret=(-1);goto exit;}
        if ((rbp1=sds_entget(e1))==NULL) {ret=(-1);goto exit;}
		if (!(foundflag & 1))
			stwid2=0.0;
		if (!(foundflag & 2))
			endwid2=0.0;		//default widths
		if (!(foundflag & 4))
			color2=256;		//default color
		foundflag=0;
		for (rbptemp=rbp1; rbptemp!=NULL; rbptemp=rbptemp->rbnext) 
			{
			if (rbptemp->restype==0 && strsame(rbptemp->resval.rstring,"SEQEND"/*DNT*/))
				break;
			else if (rbptemp->restype==42) 
				{
				bulge2=-rbptemp->resval.rreal;
				rbptemp->resval.rreal=bulge1;
				}
			else if (rbptemp->restype==40) 
				{
				stwid2=rbptemp->resval.rreal;
				rbptemp->resval.rreal=stwid1;
				foundflag|=1;
				}
			else if (rbptemp->restype==41) 
				{
				endwid2=rbptemp->resval.rreal;
				rbptemp->resval.rreal=endwid1;
				foundflag|=2;
				}
			else if (rbptemp->restype==62) 
				{
				color2=rbptemp->resval.rint;
				rbptemp->resval.rint=color1;
				foundflag|=4;
				}
			}
		if (rbptemp!=NULL && rbptemp->restype==0 && strsame(rbptemp->resval.rstring,"SEQEND"/*DNT*/))
			break;
		//note: counter for vertices must be after exit for loop!
		vtxcnt++;

		//if we didn't find optional resbufs in list & they need to be set, do so
		if (fabs(stwid1)>CMD_FUZZ && !(foundflag & 1))
			{
			foundflag|=1;
			for (rbptemp=rbp1; rbptemp->rbnext!=NULL; rbptemp=rbptemp->rbnext);
			if ((rbptemp->rbnext=sds_buildlist(40,stwid1,0))==NULL) 
				{
				cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
				ret=(-__LINE__);
				goto exit;
				}
			}
		if (fabs(endwid1)>CMD_FUZZ && !(foundflag & 2)) 
			{
			foundflag|=2;
			for (rbptemp=rbp1; rbptemp->rbnext!=NULL; rbptemp=rbptemp->rbnext);
			if ((rbptemp->rbnext=sds_buildlist(41,endwid1,0))==NULL) 
				{
				cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
				ret=(-__LINE__);
				goto exit;
				}
			}
		if (color1!=256 && !(foundflag & 4))
			{
			foundflag|=4;
			for (rbptemp=rbp1; rbptemp->rbnext!=NULL; rbptemp=rbptemp->rbnext);
			if ((rbptemp->rbnext=sds_buildlist(62,color1,0))==NULL) 
				{
				cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
				ret=(-__LINE__); 
				goto exit;
				}
			}


		bulge1=bulge2;
		stwid1=endwid2;
		endwid1=stwid2;
		color1=color2;
	    if ((elistll_cur= new struct cmd_elistll )==NULL) 
			{
	        cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
	        ret=(-__LINE__);
			goto exit;
			}
		memset(elistll_cur,0,sizeof(struct cmd_elistll));
		if (vtxent!=NULL && e1[0]==vtxent[0] && e1[1]==vtxent[1])
			vtxno=vtxcnt;
		elistll_cur->entlst=rbp1;
		if (elistll_beg==NULL)
			elistll_beg=elistll_tmp=elistll_cur;
			//keep tmp as a pointer to new last vtx(old 1st vtx) for closing bulge
		else
			{
			elistll_cur->next=elistll_beg;
			elistll_beg=elistll_cur;
			}
		}

	for (rbptemp=elistll_tmp->entlst; rbptemp!=NULL; rbptemp=rbptemp->rbnext) 
		{
		if (rbptemp->restype==40)
			rbptemp->resval.rreal=stwid1;
		else if (rbptemp->restype==41)
			rbptemp->resval.rreal=endwid1;
		else if (rbptemp->restype==42)
			rbptemp->resval.rreal=bulge1;
		else if (rbptemp->restype==62)
			rbptemp->resval.rint=color1;
	}
	if (fabs(stwid1)>CMD_FUZZ && !(foundflag & 1))
		{
		for (rbptemp=elistll_tmp->entlst; rbptemp->rbnext!=NULL; rbptemp=rbptemp->rbnext);
		if ((rbptemp->rbnext=sds_buildlist(40,stwid1,0))==NULL) 
			{
			cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
			ret=(-__LINE__);
			goto exit;
			}
		}
	if (fabs(endwid1)>CMD_FUZZ && !(foundflag & 2))
		{
		for (rbptemp=elistll_tmp->entlst; rbptemp->rbnext!=NULL; rbptemp=rbptemp->rbnext);
		if ((rbptemp->rbnext=sds_buildlist(41,endwid1,0))==NULL) 
			{
			cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
			ret=(-__LINE__);
			goto exit;
			}
		}
	if (color1!=256 && !(foundflag & 4))
		{
		for (rbptemp=elistll_tmp->entlst; rbptemp->rbnext!=NULL; rbptemp=rbptemp->rbnext);
		if ((rbptemp->rbnext=sds_buildlist(62,color1,0))==NULL) 
			{
			cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
			ret=(-__LINE__);
			goto exit;
			}
		}

    sds_entmake(NULL);
    if ((rbp2=sds_entgetx(pline,&rb))==NULL) 
		{
		ret=(-1);
		goto exit;
		}

	ic_assoc(rbp2,70);
	closure=ic_rbassoc->resval.rint&1;

    if (sds_entmake(rbp2)!=RTNORM) 
		{
		ret=(-1);
		goto exit;
		}
	for (elistll_cur=elistll_beg;elistll_cur!=NULL;elistll_cur=elistll_cur->next) 
		{
        if (sds_entmake(elistll_cur->entlst)!=RTNORM) 
			{
			ret=-1;
			goto exit;
			}
		}

	FreeResbufIfNotNull(&rbp2);
    if ((rbp2=sds_entget(e1))==NULL) {ret=(-1);goto exit;}
    if (sds_entmake(rbp2)!=RTNORM) {ret=(-1);goto exit;}
    sds_entdel(pline);
    sds_entlast(pline);
	sds_redraw(pline,IC_REDRAW_DRAW);
	if (vtxno!=0) 
		{
		if (closure && vtxno==vtxcnt)
			//vtxno=vtxcnt
			ret=1;
		else
			vtxno=vtxcnt-vtxno;
		ic_encpy(e1,pline);
		for (fl1=0;fl1<vtxno;fl1++) 
			{
            if (sds_entnext_noxref(e1,e1)!=RTNORM) 
				{
				ret=-1;
				goto exit;
				}
			}
		ic_encpy(vtxent,e1);
		}
exit:
	cmd_free_elistll(&elistll_beg,&elistll_cur);
	elistll_tmp=NULL;
	FreeResbufIfNotNull(&rbp1);
	FreeResbufIfNotNull(&rbp2);
	return(ret);
}


int cmd_reverse_lwpline(sds_name lwpline,int *vtxno) 
	{
	// I decided not to derive this from cmd_reverse_pline because it is a much simpler process for LWPLINE
	//
	//this command reverses the order of the lwpline entity lwpline.  If vtxno!=NULL,
	//the command sets vtxno to the new number of the vertex spanning the same section
	//after the reversal.  NOTE: this vertex won't have the same point - it will actually
	//be the vertex AFTER it in the original order.

	//Returns -1 or -__LINE_ if error
	//		   0 if OK
	//		   1 if OK AND the new entity is the closing segment on a closed pline
	//

	db_lwpolyline *lwpl=(db_lwpolyline *)lwpline[0];
	resbuf *lwplrb=sds_entget(lwpline);
	resbuf *curvertrb;
	int ret=0;
	int i;
	
	if (lwplrb==NULL)
		return -1;

	int nverts=lwpl->ret_90();

	curvertrb=lwplrb;
	while (curvertrb->restype!=10)		// step up to first vertex
		curvertrb=curvertrb->rbnext;
	
	for (i=0; i<nverts; i++)
		{
		lwpl->set_10(curvertrb->resval.rpoint,nverts-1-i);			// points are exactly reversed
		curvertrb=curvertrb->rbnext;
		if (i<nverts-1)
			{
			lwpl->set_41(curvertrb->resval.rreal,nverts-2-i);		// set endwid to stwid
			curvertrb=curvertrb->rbnext;
			lwpl->set_40(curvertrb->resval.rreal,nverts-2-i);		// set stwid to endwid
			curvertrb=curvertrb->rbnext;
			lwpl->set_42(-curvertrb->resval.rreal,nverts-2-i);		// set bulge to -bulge
			curvertrb=curvertrb->rbnext;							// step to next point
			}
		else // the final vert -- these values shouldn't really matter
			{
			lwpl->set_40(0.0,nverts-1);		// set stwid to 0.0
			lwpl->set_41(0.0,nverts-1);		// set endwid to 0.0
			lwpl->set_42(0.0,nverts-1);		// set bulge to 0.0
			}
		}


    if (sds_entmod(lwplrb)!=RTNORM)
		{
		ret=-1;
		goto exit;
		}
	sds_redraw(lwpline,IC_REDRAW_DRAW);

	if (vtxno!=NULL)
		{
		if (*vtxno!=nverts-1)  // stays the same if it is ==
			*vtxno=nverts-2-(*vtxno);
		if (*vtxno==nverts-1 && (lwpl->ret_70() & IC_PLINE_CLOSED))
			ret=1;
		}

exit:
	sds_relrb(lwplrb);
	return ret;
	}



int cmd_join_plines2close(sds_name ent1,sds_name ent2,long *addct) {
	//closes pline ent1 by adding on vertices from ent2
	//builds header ent 70 flag by combining bits 1,2,&3.  Bit 0 is 1
	//pline vertices must be in order of building for BOTH entities
    sds_name etemp1,etemp2,etemp3;
	struct resbuf rb,*rbp1,*rbp2,*rbp3,rbfrom,rbto;
	RT ret=0,fi1,fi2;
	long fl1;
    sds_point dummy;
	bool xform=FALSE;

	dummy[0]=dummy[1]=dummy[2]=0;

	rbp1=rbp2=rbp3=NULL;
	rb.rbnext=NULL;
	rb.restype=RTSTR;
	rb.resval.rstring=const_cast<char*>("*"/*DNT*/);
	//make header entity - union 70 bits 1,2,3.  rbp1=parent, rbp2=child
    sds_entmake(NULL);
    if ((rbp1=sds_entgetx(ent1,&rb))==NULL) 
		{
		ret=-1;
		goto exit;
		}
    if ((rbp2=sds_entget(ent2))==NULL) 
		{
		ret=-1;
		goto exit;
		}
	ic_assoc(rbp2,70);
	fi2=(ic_rbassoc->resval.rint);
	ic_assoc(rbp1,70);
	fi1=ic_rbassoc->resval.rint;
	if ((fi1 & (IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH)) ||
		(fi2 & (IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH))) 
		{
		ret=-1;
		goto exit;
		}
	if (0==(fi1 & IC_PLINE_3DPLINE) && (fi2 & IC_PLINE_3DPLINE))
		{	//can't add 3d poly to 2d
		ret=-1;
		goto exit;
		}
	ic_rbassoc->resval.rint |= IC_PLINE_CLOSED;
	ic_rbassoc->resval.rint &= (~(IC_PLINE_CURVEFIT | IC_PLINE_SPLINEFIT | IC_PLINE_3DMESH | IC_PLINE_CLOSEDN | IC_PLINE_POLYFACEMESH));
    if (sds_entmake(rbp1)!=RTNORM) 
		{
		ret=-1;
		goto exit;
		}
	if (0==(fi2 & IC_PLINE_3DPLINE) && (fi1 & IC_PLINE_3DPLINE)) 
		{
		xform=TRUE;
		ic_assoc(rbp2,210);
		ic_ptcpy(rbfrom.resval.rpoint,ic_rbassoc->resval.rpoint);
		rbfrom.restype=RT3DPOINT;
		rbto.restype=RTSHORT;
		rbto.resval.rint=0;
		}
	FreeResbufIfNotNull(&rbp1);
	FreeResbufIfNotNull(&rbp2);
	//make vertices for entity from first(main) ent - to up to next2last vertex
	//don't need to keep vertex count
	ic_encpy(etemp2,ent1);
	ic_encpy(etemp3,ent1);
	fl1=0;
	do{
		ic_encpy(etemp1,etemp2);
		FreeResbufIfNotNull(&rbp1);
		rbp1=rbp2;
		ic_encpy(etemp2,etemp3);
		rbp2=rbp3;
        sds_entnext_noxref(etemp3,etemp3);
		fl1++;
        //if (rbp3!=NULL) {sds_relrb(rbp3); rbp3=NULL;}
        if ((rbp3=sds_entget(etemp3))==NULL) {
			ret=(-__LINE__);
			cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
			goto exit;
		}
		ic_assoc(rbp1,0);
		if ((fl1>=3)&&(strsame(ic_rbassoc->resval.rstring,"VERTEX"/*DNT*/))) {
            if (sds_entmake(rbp1)!=RTNORM) {ret=-1;goto exit;}
		}
		ic_assoc(rbp3,0);
	}while(strsame(ic_rbassoc->resval.rstring,"VERTEX"/*DNT*/));
	FreeResbufIfNotNull(&rbp1);
	FreeResbufIfNotNull(&rbp2);
	FreeResbufIfNotNull(&rbp3);
	//make vertices for entity from ent to join - to up to next2last vertex
	//also - keep track of # of vertices added
	ic_encpy(etemp2,ent2);
	ic_encpy(etemp3,ent2);
	fl1=0;
	do{
		ic_encpy(etemp1,etemp2);
		FreeResbufIfNotNull(&rbp1);
		rbp1=rbp2;
		ic_encpy(etemp2,etemp3);
		rbp2=rbp3;
        sds_entnext_noxref(etemp3,etemp3);
		fl1++;
        //if (rbp3!=NULL) {sds_relrb(rbp3); rbp3=NULL;}
        if ((rbp3=sds_entget(etemp3))==NULL) {
			ret=(-__LINE__);
			cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
			goto exit;
		}
		ic_assoc(rbp1,0);
		if ((fl1>=3)&&(strsame(ic_rbassoc->resval.rstring,"VERTEX"/*DNT*/))) {
			if (xform) {
				ic_assoc(rbp1,10);
				sds_trans(ic_rbassoc->resval.rpoint,&rbfrom,&rbto,0,ic_rbassoc->resval.rpoint);
				ic_assoc(rbp1,42);
				ic_rbassoc->resval.rreal=0.0;
			}
            if (sds_entmake(rbp1)!=RTNORM) {ret=-1;goto exit;}
		}
		ic_assoc(rbp3,0);
	}while(strsame(ic_rbassoc->resval.rstring,"VERTEX"/*DNT*/));
	FreeResbufIfNotNull(&rbp1);
    if ((rbp1=sds_buildlist(RTDXF0,"SEQEND" /*DNT*/,0))==NULL) {ret=(-__LINE__);goto exit;}
    if (sds_entmake(rbp1)!=RTNORM) {ret=(-1);goto exit;}
	fl1-=3;	//don't count 1st & last vtx(= vtx of main ent) or seqend
	if (fl1>0)(*addct)+=fl1;

	exit:
	FreeResbufIfNotNull(&rbp1);
	FreeResbufIfNotNull(&rbp2);
	FreeResbufIfNotNull(&rbp3);
	return(ret);

}

int cmd_join_plend2start(sds_name ent1, sds_name ent2, long *addct, int header2use) {
	//returns 0 if successful, -1 otherwise
	//NOTE: won't join a 3d poly to a 2d poly

	//HEADER2USE=0: joins the 2 plines together so that end of ent1 matches
	//		start of ent2, using layer,lt,xdata,etc from ent1
	//HEADER2USE=1: joins the 2 plines together so that end of ent1 matches
	//		start of ent2, using layer,lt,xdata,etc from ent2
	//70 bit 0 must be off for main ent to use, bit 0 ignored on ent 2 join

    sds_name etemp1,etemp2,etemp3;
	struct resbuf rb,*rbp1,*rbp2,*rbp3,rbfrom,rbto;
	RT ret=0,fi1,fi2;
	long fl1;
    sds_point dummy;
	char xform=0;

	dummy[0]=dummy[1]=dummy[2]=0;

	rbp1=rbp2=rbp3=NULL;
	rb.rbnext=NULL;
	rb.restype=RTSTR;
	rb.resval.rstring=const_cast<char*>("*"/*DNT*/);
	//make header entity - union 70 bits 1,2,3.  rbp1=parent, rbp2=child
    sds_entmake(NULL);
	if (header2use) {
        if ((rbp1=sds_entgetx(ent2,&rb))==NULL) {ret=-1;goto exit;}
        if ((rbp2=sds_entget(ent1))==NULL) {ret=-1;goto exit;}
	}else{
        if ((rbp1=sds_entgetx(ent1,&rb))==NULL) {ret=-1;goto exit;}
        if ((rbp2=sds_entget(ent2))==NULL) {ret=-1;goto exit;}
	}
	ic_assoc(rbp2,70);
	fi2=(ic_rbassoc->resval.rint);
	ic_assoc(rbp1,70);
	fi1=ic_rbassoc->resval.rint;
	if ((fi1&80) || (fi2&80)) {ret=-1;goto exit;}
	if (0!=(fi2&8) && 0==(fi1&8)) {
		//can't join 3d pline to 2d
		ret=-1;
		goto exit;
	}
	if ((fi1&8) || (fi2&8))ic_rbassoc->resval.rint|=8;
    if (sds_entmake(rbp1)!=RTNORM) {ret=-1;goto exit;}
	if (0!=(fi1&8) && 0==(fi2&8)) {
		//we're joining 2d poly to 3d, so xform all 2d ent pts to wcs
		if (header2use)xform=1;
		else xform=2;
		rbfrom.restype=RT3DPOINT;
		ic_assoc(rbp2,210);
		ic_ptcpy(rbfrom.resval.rpoint,ic_rbassoc->resval.rpoint);
		rbto.restype=RTSHORT;
		rbto.resval.rint=0;
	}

	FreeResbufIfNotNull(&rbp1);
	FreeResbufIfNotNull(&rbp2);
	//make vertices for entity from ent1
	ic_encpy(etemp2,ent1);
	ic_encpy(etemp3,ent1);
	fl1=0;
	do{
		ic_encpy(etemp1,etemp2);
		FreeResbufIfNotNull(&rbp1);
		rbp1=rbp2;
		ic_encpy(etemp2,etemp3);
		rbp2=rbp3;
        sds_entnext_noxref(etemp3,etemp3);
		fl1++;
        //if (rbp3!=NULL) {sds_relrb(rbp3); rbp3=NULL;}
        if ((rbp3=sds_entget(etemp3))==NULL) {
			ret=(-__LINE__);
			cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
			goto exit;
		}
		ic_assoc(rbp1,0);
		if ((fl1>=3)&&(strsame(ic_rbassoc->resval.rstring,"VERTEX"/*DNT*/))) {
			if (header2use)(*addct)++;   //add vertices being added at front
										//of existing pline
			if (1==xform) {//need to transform ent1's points
				ic_assoc(rbp1,10);
				sds_trans(ic_rbassoc->resval.rpoint,&rbfrom,&rbto,0,ic_rbassoc->resval.rpoint);
				ic_assoc(rbp1,42);
				ic_rbassoc->resval.rreal=0.0;
			}
            if (sds_entmake(rbp1)!=RTNORM) {ret=-1;goto exit;}
		}
		ic_assoc(rbp3,0);
	}while(strsame(ic_rbassoc->resval.rstring,"VERTEX"/*DNT*/));
	//now we're done adding to the beginning of the pline, so add on
	//the vertices and seqend from ent2
	ic_encpy(etemp1,ent2);
	fl1=-2;	//we won't count SEQEND and 1st vertex
	do{
		fl1++;
        sds_entnext_noxref(etemp1,etemp1);
		FreeResbufIfNotNull(&rbp1);
        if ((rbp1=sds_entget(etemp1))==NULL) {ret=(-__LINE__);goto exit;}
		if (2==xform) {//need to transform ent2's points
			ic_assoc(rbp1,10);
			sds_trans(ic_rbassoc->resval.rpoint,&rbfrom,&rbto,0,ic_rbassoc->resval.rpoint);
			ic_assoc(rbp1,42);
			ic_rbassoc->resval.rreal=0.0;
		}
		if (RTNORM!=sds_entmake(rbp1)) {ret=-1;goto exit;}
		ic_assoc(rbp1,0);
	}while(strsame(ic_rbassoc->resval.rstring,"VERTEX"/*DNT*/));
	if (!header2use && fl1>0)(*addct)+=fl1;
	exit:
	FreeResbufIfNotNull(&rbp1);
	FreeResbufIfNotNull(&rbp2);
	FreeResbufIfNotNull(&rbp3);
	return(ret);
}

int cmd_join_seg2start(sds_name ent, sds_point newstart, sds_real bulge) {

	struct resbuf *rbp1=NULL;
    sds_name etemp;
    sds_point dummy;
	dummy[0]=dummy[1]=dummy[2]=0.0;

    sds_entnext_noxref(ent,etemp);
	if (cmd_make_pline_start(ent,etemp)!=0) return(-1);
    if ((rbp1=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,10,newstart,42,bulge,0))==NULL) return(-1);
    if (sds_entmake(rbp1)!=RTNORM) {
		FreeResbufIfNotNull(&rbp1);
		return(-1);
	}
	if (cmd_make_pline_end(ent,0,dummy)!=0) return(-1);
	FreeResbufIfNotNull(&rbp1);
	return(0);
}

int cmd_join_seg2end(sds_name ent, sds_point newend, sds_real bulge) {
	//note:pline not closed
	struct resbuf *rbp1=NULL;
    sds_name etemp1,etemp2;

	ic_encpy(etemp2,ent);
	do{
		ic_encpy(etemp1,etemp2);
        sds_entnext_noxref(etemp2,etemp2);
		FreeResbufIfNotNull(&rbp1);
        if ((rbp1=sds_entget(etemp2))==NULL)return(-1);
		ic_assoc(rbp1,0);
	}while(strsame(ic_rbassoc->resval.rstring,"VERTEX"/*DNT*/));
	FreeResbufIfNotNull(&rbp1);
	if (cmd_make_pline_start(ent,etemp1)!=0) return(-1);
    if ((rbp1=sds_entget(etemp1))==NULL)return(-1);
	ic_assoc(rbp1,42);
	ic_rbassoc->resval.rreal=bulge;
    if (sds_entmake(rbp1)!=RTNORM) {
		FreeResbufIfNotNull(&rbp1);
		return(-1);
	}
	FreeResbufIfNotNull(&rbp1);
    if ((rbp1=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,10,newend,42,0.0,0))==NULL) return(-1);
    if (sds_entmake(rbp1)!=RTNORM) {
		FreeResbufIfNotNull(&rbp1);
		return(-1);
	}
	FreeResbufIfNotNull(&rbp1);
    if ((rbp1=sds_buildlist(RTDXF0,"SEQEND"/*DNT*/,0))==NULL) return(-1);
    if (sds_entmake(rbp1)!=RTNORM) {
		FreeResbufIfNotNull(&rbp1);
		return(-1);
	}
	FreeResbufIfNotNull(&rbp1);
	return(0);
}

void drawcursorandstart(sds_point curpt, sds_point startpt, int drawxflag, sds_real *extruptr)
{
	cmd_pedit_drawx(curpt, drawxflag,extruptr);
	cmd_pedit_drawst(startpt,drawxflag,extruptr);
}

void erasepline(sds_name polyent,int drawmode, sds_point curpt, sds_point startpt, int drawxflag, sds_real *extruptr)
// redraw polyline, erasing cursor and start mark first, then restoring them.
{
	drawcursorandstart(curpt,startpt,drawxflag,extruptr);	// undraw them
	sds_redraw(polyent,drawmode);
}

void redrawpline(sds_name polyent,int drawmode, sds_point curpt, sds_point startpt, int drawxflag, sds_real *extruptr)
// redraw polyline, erasing cursor and start mark first, then restoring them.
{
	drawcursorandstart(curpt,startpt,drawxflag,extruptr);	// undraw them
	sds_redraw(polyent,drawmode);
	drawcursorandstart(curpt,startpt,drawxflag,extruptr);	// and redraw them
}


void regenpline(sds_name polyent,int drawmode, sds_point curpt, sds_point startpt, int drawxflag, sds_real *extruptr)
// regen polyline, erasing cursor and start mark first, then restoring them.
{
	drawcursorandstart(curpt,startpt,drawxflag,extruptr);	// undraw them
	sds_entupd(polyent);
	drawcursorandstart(curpt,startpt,drawxflag,extruptr);	// and redraw them
}


int vertexnum(struct cmd_elistll *plinelist, struct cmd_elistll *vertptr)
// return the index of the vertex in this polyline.  0 is the first -- NOT 1 BIASED.  -1 return means not found
{
	int vertno;
	
	ASSERT(plinelist!=NULL);
	ASSERT(vertptr!=NULL);
	
	plinelist=plinelist->next;
	vertno=0;
	while (plinelist!=NULL) {
		if (plinelist==vertptr) return(vertno);
		vertno++;
		plinelist=plinelist->next;
	}
	return(-1);
}


void eraseanddrawnewpline(sds_name polyent, struct cmd_elistll *curvert,
					 sds_point lastcurpt,sds_point laststpt,int drawxflag, sds_real *extruptr)
{
	erasepline(polyent,IC_REDRAW_UNDRAW,lastcurpt,laststpt,drawxflag,extruptr);	// erase it
	sds_entmod(curvert->entlst);												// update the vertex
	sds_entupd(polyent);														// update the entity
	drawcursorandstart(lastcurpt,laststpt,drawxflag,extruptr);					// and redraw these
	SDS_DBModified( IC_DBMOD_ENTITIES_MODIFIED);
}

struct cmd_elistll *prevvert(struct cmd_elistll *plinelist, struct cmd_elistll *vertptr)
// return a pointer to the vertex preceding the one in vertptr
{
	ASSERT(plinelist!=NULL);
	ASSERT(vertptr!=NULL);

// if it's the first one, return the last
	if (vertptr==plinelist->next) {
		while (plinelist->next->next!=NULL) plinelist=plinelist->next;
		return(plinelist);
	}

// not the first, find the one before it	
	plinelist=plinelist->next;   // step to the first vertex
	for (;;) {
		if (plinelist==NULL || plinelist->next==vertptr)
			return(plinelist);
		plinelist=plinelist->next;
	}
	return(NULL);
}

struct cmd_elistll *nextvert(cmd_elistll *pl_elist,cmd_elistll *vert)
// step to next vertex; wrap around at end
{
	ASSERT(pl_elist!=NULL);
	ASSERT(vert!=NULL);
	
	vert=vert->next;
	if (vert->next==NULL)		// if we're on the seqend
		vert=pl_elist->next;	// loop back to the first vert
	return vert;
}

				 
int cmd_peditvtx_select(sds_name polyent,struct cmd_elistll *pl_elist, struct cmd_elistll **vertptr,
					 sds_point lastcurpt,sds_point laststpt,int drawxflag, sds_real *extruptr)
{
	sds_point p0;
	int vertexno,ret,exactvert;
	
	redrawpline(polyent,IC_REDRAW_HILITE,lastcurpt,laststpt,drawxflag,extruptr);
	ret=internalGetpoint(NULL,ResourceString(IDC_PEDIT__NSELECT_POINT_AL_78, "\nSelect point along segment to edit: " ),p0);
	redrawpline(polyent,IC_REDRAW_UNHILITE,lastcurpt,laststpt,drawxflag,extruptr);

	if (ret!=RTNORM) return ret;

	vertexno=cmd_pl_vtx_no(polyent,p0,1,&exactvert);
	(*vertptr)=pl_elist;  // starts at the polyline entity, but vertexno is biased by 1 anyway
	if (exactvert<0)
		*vertptr=(*vertptr)->next; // move to first vert
	else 
		{
		// This has caused a crash before, do a little extra error checking
		while ( (exactvert>0) && (*vertptr) && ((*vertptr)->next) )
			{
			(*vertptr)=(*vertptr)->next;
			exactvert--;
			}
		}
	return RTNORM;
}

struct cmd_elistll *cmd_peditvtx_right(struct cmd_elistll *pl_elist, struct cmd_elistll *vertptr,int nmax)
{
	if ((vertexnum(pl_elist,vertptr) % nmax)<(nmax-1)) vertptr=vertptr->next;
	return(vertptr);
}

struct cmd_elistll *cmd_peditvtx_left(struct cmd_elistll *pl_elist, struct cmd_elistll *vertptr,int nmax)
{
	if ((vertexnum(pl_elist,vertptr) % nmax)>0) vertptr=prevvert(pl_elist,vertptr);
	return(vertptr);
}

struct cmd_elistll *cmd_peditvtx_up(struct cmd_elistll *pl_elist, struct cmd_elistll *vertptr,int mmax,int nmax)
{
	int i;

	if (vertexnum(pl_elist,vertptr)/nmax < mmax-1) {  // if true, step ahead nmax verts
		for (i=0; i<nmax; i++)
			vertptr=vertptr->next;
	}
	return(vertptr);
}

struct cmd_elistll *cmd_peditvtx_down(struct cmd_elistll *pl_elist, struct cmd_elistll *vertptr,int mmax, int nmax)
{
	int vertno=vertexnum(pl_elist,vertptr);

	if (vertno/nmax > 0) {						// step back nmax verts by computing a new vertno
		vertno-=nmax;							// and advancing from the start
		vertptr=pl_elist->next;
		while (vertno--) vertptr=vertptr->next;
	}
	return(vertptr);
}

void cmd_peditvtx_move(sds_name polyent, struct cmd_elistll *pl_elist,struct cmd_elistll *curvert,
					sds_point lastcurpt, sds_point laststpt, int drawxflag, sds_real *extruptr,
					struct resbuf *rbucs, struct resbuf *rbent)
{
	sds_point p0,p1;
	struct resbuf *temprb;

	temprb=ic_ret_assoc(curvert->entlst,10);
	sds_trans(temprb->resval.rpoint,rbent,rbucs,0,p1);
	if (sds_getpoint(p1,ResourceString(IDC_PEDIT__NNEW_LOCATION_FO_97, "\nNew location for vertex: " ),p0)==RTNORM) {
		sds_trans(p0,rbucs,rbent,0,temprb->resval.rpoint);
		eraseanddrawnewpline(polyent,curvert,lastcurpt,laststpt,drawxflag,extruptr);
	}
}

int getawidth(struct cmd_elistll *vert, int dxfgroup, const char *promptstr, double *retwidth)
{
	struct resbuf *widthrb;
	char defaultwidthstr[IC_ACADBUF];
	char widthprompt[IC_ACADBUF];
	RT ret;
	sds_real defaultwidth=0.0;

	if ((widthrb=ic_ret_assoc(vert->entlst,dxfgroup))!=NULL) {
		defaultwidth=widthrb->resval.rreal;
	}
	sds_rtos(defaultwidth,-1,-1,defaultwidthstr);
	sprintf(widthprompt,promptstr,defaultwidthstr);
	sds_initget(4+SDS_RSG_NODOCCHG,NULL);
	ret=sds_getdist(vert->grpptr->resval.rpoint,widthprompt,retwidth);
	if (ret==RTNONE) {
		*retwidth=defaultwidth;
		ret=RTNORM;
	}
	return(ret);
}

void cmd_peditvtx_width(sds_name polyent, struct cmd_elistll *pl_elist,struct cmd_elistll *curvert,
					sds_point lastcurpt, sds_point laststpt, int drawxflag, sds_real *extruptr,
					struct resbuf *rbucs, struct resbuf *rbent)
{
	struct resbuf *widthrb;
	sds_real startwidth,endwidth;

	if (getawidth(curvert,40,ResourceString(IDC_PEDIT__NENTER_STARTING__90,"\nEnter starting width <%s>: "),&startwidth)!=RTNORM) return;
	if (getawidth(curvert,41,ResourceString(IDC_PEDIT__NENTER_ENDING_WI_91, "\nEnter ending width <%s>: "),&endwidth)!=RTNORM) return;
	if ((widthrb=ic_ret_assoc(curvert->entlst,40))==NULL) return;
	widthrb->resval.rreal=startwidth;
	if ((widthrb=ic_ret_assoc(curvert->entlst,41))==NULL) return;
	widthrb->resval.rreal=endwidth;
	eraseanddrawnewpline(polyent,curvert,lastcurpt,laststpt,drawxflag,extruptr);
}


void cmd_peditvtx_angle(sds_name polyent, struct cmd_elistll *pl_elist,struct cmd_elistll *curvert,
					sds_point lastcurpt, sds_point laststpt, int drawxflag, sds_real *extruptr,
										struct resbuf *rbucs, struct resbuf *rbent)
{
	sds_real inclang,newinclang;
	struct resbuf *angrb,rb;
	int angdir;
	char inclangstr[IC_ACADBUF];
	char angprompt[IC_ACADBUF];
	RT ret;
	sds_point dragpt;

	angrb=ic_ret_assoc(curvert->entlst,42);
	if (angrb->resval.rreal<0.0)
		inclang=-4.0*atan(angrb->resval.rreal);
	else
		inclang=4.0*atan(angrb->resval.rreal);
	//angtos uses angbase & angdir, so correct for it
	SDS_getvar(NULL,DB_QANGDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	angdir=rb.resval.rint;
	SDS_getvar(NULL,DB_QANGBASE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if (angdir==0)
		inclang+=rb.resval.rreal;
	else
		inclang-=rb.resval.rreal;
	if (angdir==1) inclang-=IC_TWOPI;
	sds_angtos(inclang,-1,-1,inclangstr);
	if (angdir==1)
		sprintf(angprompt,ResourceString(IDC_PEDIT__NINCLUDED_ANGLE__86, "\nIncluded angle for segment (>0 is cw, 0 is straight, <0 is ccw) <%s>: " ),inclangstr);
	else
		sprintf(angprompt,ResourceString(IDC_PEDIT__NINCLUDED_ANGLE__87, "\nIncluded angle for segment (>0 is ccw, 0 is straight, <0 is cw) <%s>: " ),inclangstr);

	sds_trans(curvert->grpptr->resval.rpoint,rbent,rbucs,0,dragpt);

	sds_initget(SDS_RSG_NODOCCHG,NULL);
	ret=SDS_getangleneg(dragpt,angprompt,&newinclang);
	if (ret!=RTNORM && ret!=RTNONE) return;
	
	if (ret==RTNONE) newinclang=inclang;
	else if (angdir==1) newinclang=-newinclang; // RTNORM

	angrb->resval.rreal=tan(newinclang/4.0);	// new bulge
	eraseanddrawnewpline(polyent,curvert,lastcurpt,laststpt,drawxflag,extruptr);
}

int getsecondvertex(sds_name polyent, struct cmd_elistll **pl_elist,struct cmd_elistll *vert1,struct cmd_elistll **vert2,const char *initgetstr,const char *promptstr,
					int drawxflag, sds_real *extruptr)
{
	const int NEXT=0;
	const int PREV=1;
	int nextprev=NEXT;
	const char *nextprevstr;
	RT ret;
	char prompt[IC_ACADBUF],retkeyword[IC_ACADBUF];
	struct cmd_elistll *lastvert2=vert1;
	sds_point p1;
	int vertno;
	int exactvert;

	for (;;) {
		if (nextprev==NEXT) nextprevstr=ResourceString(IDC_PEDIT_NEXT_66,"Next");
		else nextprevstr=ResourceString(IDC_PEDIT_PREVIOUS_75,"Previous");

		if (lastvert2!=vert1) {
			cmd_pedit_drawx(lastvert2->grpptr->resval.rpoint,drawxflag,extruptr);
		}
		if ((*vert2)!=vert1) {
			cmd_pedit_drawx((*vert2)->grpptr->resval.rpoint,drawxflag,extruptr);
		}
		lastvert2=(*vert2);

		sds_initget(SDS_RSG_NOCANCEL+SDS_RSG_NODOCCHG,initgetstr);
		sprintf(prompt,promptstr,nextprevstr);
		if ((ret=sds_getkword(prompt,retkeyword))==RTCAN) return RTCAN;
		if (ret==RTNONE) {
			if (nextprev==NEXT) strcpy(retkeyword,"NEXT"/*DNT*/);
			else strcpy(retkeyword,"PREVIOUS"/*DNT*/);
		}

		if (strisame(retkeyword,"EXIT"/*DNT*/ )) {
			return RTNONE;
		}

		if (strisame(retkeyword,"NEXT"/*DNT*/)) {
			nextprev=NEXT;
			(*vert2)=nextvert(*pl_elist,(*vert2));
		}
		else if (strisame(retkeyword,"PREVIOUS"/*DNT*/)) {
			nextprev=PREV;
			(*vert2)=prevvert(*pl_elist,(*vert2));
		}
		else if (strisame(retkeyword,"SELECT"/*DNT*/ )) {
			if ((ret=internalGetpoint(NULL,ResourceString(IDC_PEDIT__NSELECT_POINT_AL_95, "\nSelect point along segment move to: " ),p1))==RTCAN) {
				return RTNONE;
			}
			if (ret!=RTNORM) continue;
			vertno=cmd_pl_vtx_no(polyent,p1,1,&exactvert);
			(*vert2)=*pl_elist;  // starts at the polyline entity, but vertexno is biased by 1 anyway
			while ( (exactvert) && (*vert2) && ((*vert2)->next) ){
				(*vert2)=(*vert2)->next;
				exactvert--;
			}
		}
		else if (strisame(retkeyword,"Go"/*DNT*/)) { 			//go go go
			return(RTNORM);
		}
	}
	return(RTNORM);
}


void cmd_peditvtx_straighten(sds_name polyent, struct cmd_elistll **pl_elist,struct cmd_elistll **curvert,
					sds_point lastcurpt, sds_point laststpt, int drawxflag, sds_real *extruptr)
{
	struct cmd_elistll *vert1=*curvert;
	struct cmd_elistll *junkcur;
	struct cmd_elistll *vert2=vert1;
	struct cmd_elistll *lastvert2=vert1;
	RT ret;
	int vert1no,vert2no;
	sds_point vert2pt;

	if (getsecondvertex(polyent,pl_elist,vert1,&vert2,ResourceString(IDC_PEDIT_INITGET_NEXT_PREV_93, "Next Previous Select ~ Go Exit|eXit ~_Next ~_Previous ~_Select ~_ ~_Go ~_eXit"),
		ResourceString(IDC_PEDIT__NSTRAIGHTEN___NE_94, "\nStraighten:  Next/Previous/Select/Go/eXit/<%s>: "),drawxflag,extruptr)!=RTNORM) {
		goto exit;
	}
	
// only get to here on "go"
	ic_ptcpy(vert2pt,vert2->grpptr->resval.rpoint);
	if (vert2==vert1) {	// just straighten the vertex we're on
		struct resbuf *temprb=ic_ret_assoc(vert1->entlst,42);
		temprb->resval.rreal=0.0;
		eraseanddrawnewpline(polyent,vert1,lastcurpt,laststpt,drawxflag,extruptr);
	}
	else {
		struct cmd_elistll *startvert=vert1;
		struct cmd_elistll *endvert=vert2;

		vert1no=vertexnum(*pl_elist,vert1);
		vert2no=vertexnum(*pl_elist,vert2);
		if (vert1no>vert2no) {
			startvert=vert2;
			endvert=vert1;
		}

		startvert=startvert->next;
		while (startvert!=endvert) {
			startvert->modflag=-1;
			startvert=startvert->next;
		}
				
		if (vert2!=vert1)
			cmd_pedit_drawx(lastvert2->grpptr->resval.rpoint,drawxflag,extruptr);	// erase
		drawcursorandstart(lastcurpt,laststpt,drawxflag,extruptr);					// erase
		sds_entdel(polyent);
		for (startvert=*pl_elist; startvert!=NULL; startvert=startvert->next) {
			if (startvert->modflag>=0) {
				if (sds_entmake(startvert->entlst)!=RTNORM) {
					sds_entdel(polyent);
					//cmd_pedit_drawx(p0,drawxflag,*pp1);
					ret=(-__LINE__);
					goto exit;	
				}
			}
		}
		sds_entlast(polyent);
		if (vert2!=vert1)
			cmd_pedit_drawx(lastvert2->grpptr->resval.rpoint,drawxflag,extruptr);	// draw
		drawcursorandstart(lastcurpt,laststpt,drawxflag,extruptr);					// draw
		cmd_free_elistll(pl_elist,&junkcur);
		cmd_alloc_elistll(pl_elist,&junkcur,polyent,10);
		*curvert=(*pl_elist)->next;
		while (vert1no) {
			(*curvert)=(*curvert)->next;
			vert1no--;
		}
	} // vert2!=vert1

exit:
	if (vert2!=vert1)
		cmd_pedit_drawx(vert2pt,drawxflag,extruptr);
}


void cmd_peditvtx_insert(sds_name polyent, struct cmd_elistll **pl_elist,struct cmd_elistll **curvert,
					sds_point lastcurpt, sds_point laststpt, int drawxflag, sds_real *extruptr,
					struct resbuf *rbucs, struct resbuf *rbent)
{
	sds_point p0,p1;
	sds_real newwidth;
	struct cmd_elistll *vertptr,*junkcur;
	struct resbuf *temprb;
	int vertno;
	
	//Insert new vertex into linked list of elists....
	sds_trans((*curvert)->grpptr->resval.rpoint,rbent,rbucs,0,p1);
	if (sds_getpoint(p1,ResourceString(IDC_PEDIT__NLOCATION_FOR_NE_99, "\nLocation for new vertex: " ),p0)==RTNORM) {
		sds_trans(p0,rbucs,rbent,0,p0);
		temprb=ic_ret_assoc((*curvert)->entlst,42);
		temprb->resval.rreal=0.0;
		if ((temprb=ic_ret_assoc((*curvert)->entlst,41))==NULL)
			newwidth=0.0;
		else
			newwidth=temprb->resval.rreal;
		drawcursorandstart(lastcurpt,laststpt,drawxflag,extruptr);					// erase
		vertno=vertexnum(*pl_elist,*curvert);
		sds_entdel(polyent);
		vertptr=*pl_elist;
		for (vertptr=*pl_elist; vertptr!=NULL; vertptr=vertptr->next) {
			if (sds_entmake(vertptr->entlst)!=RTNORM) {
//				sds_entdel(polyent);										// TODO -- error recovery
				return;
			}
			if (vertptr==(*curvert)) {
				if ((temprb=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,10,p0,40,newwidth,41,newwidth,42,0.0,0))==NULL) {
					return;
				}
				if (sds_entmake(temprb)!=RTNORM) {
					return;
				}
			}
		}
		sds_entlast(polyent);
		drawcursorandstart(lastcurpt,laststpt,drawxflag,extruptr);					// draw
		cmd_free_elistll(pl_elist,&junkcur);
		cmd_alloc_elistll(pl_elist,&junkcur,polyent,10);
		*curvert=(*pl_elist)->next;
		while (vertno--) (*curvert)=(*curvert)->next;
	}
}


void cmd_peditvtx_break(sds_name polyent, struct cmd_elistll **pl_elist,struct cmd_elistll **curvert,
					sds_point lastcurpt, sds_point laststpt, int drawxflag, sds_real *extruptr,
					struct resbuf *rbucs, struct resbuf *rbent)
{
	struct cmd_elistll *vert1=*curvert;
	struct cmd_elistll *junkcur;
	struct cmd_elistll *vert2=vert1;
	struct cmd_elistll *lastvert2=vert1;
	struct cmd_elistll *finalvertex,*breakvert1,*breakvert2;
	int closedflag;
	int vert1no,vert2no;
	struct resbuf *temprb;
	sds_point p0;
	sds_name etemp,etemp2;
	int breakvert1wasfirst;

	if (getsecondvertex(polyent,pl_elist,vert1,&vert2,ResourceString(IDC_PEDIT_INITGET_NEXT_PRE_103, "Next Previous Select Go Exit|eXit ~_Next ~_Previous ~_Select ~_Go ~_eXit"),
		ResourceString(IDC_PEDIT__NNEXT_PREVIOUS__104, "\nNext/Previous/Select/Go/eXit/<%s>: "),drawxflag,extruptr)!=RTNORM) 
		{
		//paint out x at go vertex
		cmd_pedit_drawx(vert2->grpptr->resval.rpoint,drawxflag,extruptr);
		return;
		}
	
	// only get to here on "go"
	//get pointer to final vertex
	finalvertex=*pl_elist;
	while (finalvertex->next->next!=NULL)
		finalvertex=finalvertex->next;
	
	//make sure both breaks aren't at start or end of pline
	if ((vert1==vert2) && (vert1==(*pl_elist)->next || vert1==finalvertex))
		{
		cmd_ErrorPrompt(CMD_ERR_SELECTION,0);
		return;
		}

	// and that they don't encompass the entire polyline
	if ((vert1==(*pl_elist)->next && vert2==finalvertex) ||
		(vert2==(*pl_elist)->next && vert1==finalvertex)) 
		{
		cmd_ErrorPrompt(CMD_ERR_BREAKDELETE,0);
		//paint out x at go vertex
		cmd_pedit_drawx(vert2->grpptr->resval.rpoint,drawxflag,extruptr);
		return;
		}
	vert1no=vertexnum(*pl_elist,vert1);
	vert2no=vertexnum(*pl_elist,vert2);
	
	breakvert1=vert1;
	breakvert2=vert2;
	if (vert1no>vert2no) 
		{
		breakvert1=vert2;
		breakvert2=vert1;
		}
	if (breakvert1==(*pl_elist)->next)
		breakvert1wasfirst=1;
	else 
		breakvert1wasfirst=0;
	
	//get closure flag from pline
	temprb=ic_ret_assoc((*pl_elist)->entlst,70);
	closedflag=temprb->resval.rint;
	//get closure point(1st vtx pt) & store in p0
	temprb=ic_ret_assoc((*pl_elist)->next->entlst,10);
	ic_ptcpy(p0,temprb->resval.rpoint);
	//paint out x at the second vertex, if not the same as the first
	if (vert1!=vert2)
		cmd_pedit_drawx(vert2->grpptr->resval.rpoint,drawxflag,extruptr); // erase
	// paint out the first vertex's x, and the start indicator
	drawcursorandstart(lastcurpt,laststpt,drawxflag,extruptr);
	etemp[0]=etemp[1]=0;

	//make end of pline
	if (breakvert2!=finalvertex) 
		{
		sds_entmake(NULL);
		if ((cmd_make_pline_start(polyent,(*pl_elist)->next->ename)!=0) ||	//header ent
		   (sds_entmake(breakvert2->entlst)!=RTNORM) ||						//current vtx ent
		   (cmd_make_pline_end(breakvert2->ename,closedflag,p0)!=0))			//pline end
			return;
		sds_entlast(etemp);
		}
	//make begin of pline
	if (breakvert1!=(*pl_elist)->next)  // if not the first vertex
		{
		if ((temprb=sds_buildlist(RTDXF0,"SEQEND"/*DNT*/,0))==NULL)
			return;
		sds_entmake(NULL);
		if ((cmd_make_pline_start(polyent,breakvert1->ename)!=0) ||
		   (sds_entmake(breakvert1->entlst)!=RTNORM) ||
		   (sds_entmake(temprb)!=RTNORM))
			return;
		sds_entlast(etemp2);
		}
	else
		{
		//we aren't making a beginning pline, just an end
		ic_encpy(etemp2,etemp);
		etemp[0]=etemp[1]=0;
		}

	sds_entdel(polyent);
	ic_encpy(polyent,etemp2);
	sds_redraw(polyent,1);
	if ((etemp[0]!=0) || (etemp[1]!=0))
		sds_redraw(etemp,1);

// 'pears to me that etemp is always the "not the part we continue editing" part,
// so we check to see if this should be a lightweight polyline.
	
	if(etemp[0]!=0)
	{
		resbuf pltrb;
		SDS_getvar(NULL,DB_QPLINETYPE,&pltrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		if (pltrb.resval.rint!=0 && PolylineCanBeLwpolyline(etemp))
			{
			sds_name lwpname;
			ConvertPolylineToLwpolyline(etemp,lwpname);
			}
	}

	cmd_free_elistll(pl_elist,&junkcur);
	cmd_alloc_elistll(pl_elist,&junkcur,polyent,10);
	*curvert=*pl_elist;
	if (breakvert1wasfirst)
		(*curvert)=(*curvert)->next; // go to first vertex
	else
		while ((*curvert)->next->next!=NULL)
			(*curvert)=(*curvert)->next; // go to last
	//paint curvert x
	ic_ptcpy(lastcurpt,(*curvert)->grpptr->resval.rpoint);
	ic_ptcpy(laststpt,(*pl_elist)->next->grpptr->resval.rpoint);
	drawcursorandstart(lastcurpt,laststpt,drawxflag,extruptr); // put these back
}

	
int cmd_pline_editvtx(sds_name polyent) {
// pedit edit vertex, rewritten 8/28/98--9/2/98.

// if you do not init llbeg, llcur to NULL, cmd_alloc_elistll does not work properly
	struct cmd_elistll *pl_elist=NULL,*elist_llcur=NULL,*curvert=NULL,*junkptr=NULL;//,*elist_lltemp;
	struct resbuf rbucs,rbent;
//	struct resbuf *rbptr;
	int polyflag;
	db_polyline *polyhip;
	int mmax,nmax;
	char promptstr[IC_ACADBUF],retkeyword[IC_ACADBUF];
	const int NEXT=0;
	const int PREV=1;
	int nextprev=NEXT;
	const char *nextprevstr;
	int promptret,drawxflag;
	double plextru[3],*plextruptr=NULL;									// default case, no extrusion used
	CString mesh3dprompt;
	CString mesh3dinitgetstr;
	CString pline3dprompt;
	CString pline3dinitgetstr;
	CString pline2dprompt; 
	CString pline2dinitgetstr;
	CString theinitgetstr;
	sds_point lastcursorpoint,laststartpoint;

    mesh3dprompt=ResourceString(IDC_PEDIT__NEDIT_VERTEX__I__70, "\nEdit vertex(%i,%i):  Select/Next/Previous/Left/RIght/Up/Down/Move/REgen/eXit/<%s>: ");
	mesh3dinitgetstr=ResourceString(IDC_PEDIT_INITGET_SELECT____69, "Select ~ Next_vertex|Next Previous_vertex|Previous ~ Left Right|RIght ~ Up Down ~ Move Regen|REgen ~ Exit|eXit ~_Select ~_ ~_Next ~_Previous ~_ ~_Left ~_RIght ~_ ~_Up ~_Down ~_ ~_Move ~_REgen ~_ ~_eXit");
	pline3dprompt=ResourceString(IDC_PEDIT__NEDIT_VERTICES___72, "\nEdit vertices:  Next/Previous/Break/Insert/Move/Regen/SElect/eXit/<%s>: ");
	pline3dinitgetstr=ResourceString(IDC_PEDIT_INITGET_NEXT_VERT_71, "Next_vertex|Next Previous_vertex|Previous ~ Break Insert_vertex|Insert Move Regen Select|SElect ~ Exit|eXit ~_Next ~_Previous ~_ ~_Break ~_Insert ~_Move ~_Regen ~_SElect ~_ ~_eXit");
	pline2dprompt=ResourceString(IDC_PEDIT__NEDIT_VERTICES___74, "\nEdit vertices:  Next/Previous/Angle/Break/Insert/Move/Regen/SElect/Straighten/Width/eXit/<%s>: ");
	LOAD_COMMAND_OPTIONS_2(IDC_PEDIT_INITGET_NEXT_VERT_73)
	pline2dinitgetstr = LOADEDSTRING;

// init rb's for transforms
	rbucs.restype=RTSHORT;
	rbucs.resval.rint=1;
	rbent.restype=RT3DPOINT;

	polyhip=(db_polyline *)polyent[0];									// get a pointer to the pline ent so we can access directly
	cmd_alloc_elistll(&pl_elist,&elist_llcur,polyent,10);			// get the polyline
	curvert=pl_elist->next;											// set the current vertext to the first vertex
	polyflag=polyhip->ret_70();											// get its flags

	mmax=nmax=0;
	drawxflag=0;														// points are in wcs (true only for 3d plines)
	if (polyflag & IC_PLINE_3DMESH)  									// if 3D, set n and m max values
		{
		mmax=polyhip->ret_71();
		nmax=polyhip->ret_72();
		}
	else if (!(polyflag & IC_PLINE_3DPLINE))   // 2d case
		{
		drawxflag=2;													// points are in ucs, and we provide an extrusion direction
		polyhip->get_210(plextru);										// get the extrusion
		plextruptr=plextru;												// and point to it.  Extrusion now used in drawx/drawst
		ic_ptcpy(rbent.resval.rpoint,plextru);
		}

	//Add by SMR for pedit bug fix. 2001/3/4
	//Changed by ALM
	struct gr_view *pBoundaryView = SDS_CURGRVW;
	gr_real_rectangle bb = pBoundaryView->GetClipRectangle();

	bool IsInBoundary = false;
	transf w2u = wcs2ucs();
	while (!IsInBoundary)
	{
		point ptRp, ptUcs;
		if (polyflag & IC_PLINE_3DMESH)
		{
			point ptWcs = curvert->grpptr->resval.rpoint;
			ptUcs = ptWcs * w2u;
		}
		else
		{
			point ptOcs = curvert->grpptr->resval.rpoint;
			sds_trans(ptOcs, &rbent, &rbucs, 0, ptUcs);
		}
		gr_ucs2rp(ptUcs, ptRp, pBoundaryView);

		if (((bb.xmin <= ptRp.x()) && (bb.xmax >= ptRp.x())) &&
			((bb.ymin <= ptRp.y()) && (bb.ymax >= ptRp.y())))
		{
			IsInBoundary = true;
		}
		else
		{
			curvert = nextvert(pl_elist,curvert);
			if (curvert == pl_elist->next)
				break;
		}
	}
	
// initial draw of X cursor and start marker to start the ball rolling
	ic_ptcpy(lastcursorpoint,curvert->grpptr->resval.rpoint);
	ic_ptcpy(laststartpoint,pl_elist->next->grpptr->resval.rpoint);
	drawcursorandstart(lastcursorpoint,laststartpoint,drawxflag,plextruptr);

	for (;;) 
		{
		drawcursorandstart(lastcursorpoint,laststartpoint,drawxflag,plextruptr);
		ic_ptcpy(lastcursorpoint,curvert->grpptr->resval.rpoint);
		ic_ptcpy(laststartpoint,pl_elist->next->grpptr->resval.rpoint);
		drawcursorandstart(lastcursorpoint,laststartpoint,drawxflag,plextruptr);

		if (nextprev==NEXT) 
            nextprevstr=ResourceString(IDC_PEDIT_NEXT_66,"Next");
		else 
            nextprevstr=ResourceString(IDC_PEDIT_PREVIOUS_75,"Previous");

// set up the prompt strings
		if (polyflag & IC_PLINE_3DMESH) 									// if 3D, set n and m max values
			{
			theinitgetstr=mesh3dinitgetstr;
			int vertexno=vertexnum(pl_elist,curvert);
			sprintf(promptstr,mesh3dprompt,(vertexno/nmax),(vertexno % nmax),nextprevstr);
			}
		else if (polyflag & IC_PLINE_3DPLINE) 
			{
			//3d poly - allow no options related to width or bulge
			theinitgetstr=pline3dinitgetstr;
			sprintf(promptstr,pline3dprompt,nextprevstr);
			}
		else
			{
			//2d poly
			theinitgetstr=pline2dinitgetstr;
			sprintf(promptstr,pline2dprompt,nextprevstr);
			}

		sds_initget(SDS_RSG_NOCANCEL+SDS_RSG_NODOCCHG,theinitgetstr);
		promptret=sds_getkword(promptstr,retkeyword);

		if (promptret==RTCAN)
			goto exit;

		if (promptret==RTNONE)   // apply the default
			strcpy(retkeyword,nextprevstr);
// now process the keyword
		if (strisame(retkeyword,"EXIT"/*DNT*/)) 
			goto exit;

		if (strisame(retkeyword,"NEXT"/*DNT*/ )) 
			{
			//Modified by SMR 2001/3/4 for pedit bug fix.
			IsInBoundary = false;
			transf w2u = wcs2ucs();
			do
			{
				curvert = nextvert(pl_elist,curvert);
				point ptRp, ptUcs;
				if (polyflag & IC_PLINE_3DMESH)
				{
					point ptWcs = curvert->grpptr->resval.rpoint;
					ptUcs = ptWcs * w2u;
				}
				else
				{
					point ptOcs = curvert->grpptr->resval.rpoint;
					sds_trans(ptOcs, &rbent, &rbucs, 0, ptUcs);
				}
				gr_ucs2rp(ptUcs, ptRp, pBoundaryView);

				if (((bb.xmin <= ptRp.x()) && (bb.xmax >= ptRp.x())) &&
					((bb.ymin <= ptRp.y()) && (bb.ymax >= ptRp.y())))
				{
					IsInBoundary = true;
				}
				if (curvert == pl_elist->next)
					break;
			}while(!IsInBoundary);
			//End of modification. SMR 2001/3/4
			nextprev=NEXT;
			}
		else if (strisame(retkeyword,"PREVIOUS" )) 
			{
			//Modified by SMR 2001/3/4 for pedit bug fix.
			IsInBoundary = false;
			transf w2u = wcs2ucs();
			do
			{
				curvert = prevvert(pl_elist,curvert);
				point ptRp, ptUcs;
				if (polyflag & IC_PLINE_3DMESH)
				{
					point ptWcs = curvert->grpptr->resval.rpoint;
					ptUcs = ptWcs * w2u;
				}
				else
				{
					point ptOcs = curvert->grpptr->resval.rpoint;
					sds_trans(ptOcs, &rbent, &rbucs, 0, ptUcs);
				}
				gr_ucs2rp(ptUcs, ptRp, pBoundaryView);

				if (((bb.xmin <= ptRp.x()) && (bb.xmax >= ptRp.x())) &&
					((bb.ymin <= ptRp.y()) && (bb.ymax >= ptRp.y())))
				{
					IsInBoundary = true;
				}
				if (curvert == pl_elist->next)
					break;
			}while(!IsInBoundary);
			//End of modification SMR. 2001/3/4

			nextprev=PREV;
			}
		else if (strisame(retkeyword,"SELECT"/*DNT*/ )) 
			cmd_peditvtx_select(polyent,pl_elist,&curvert,lastcursorpoint,laststartpoint,drawxflag,plextruptr);
		else if (strisame(retkeyword,"RIGHT"/*DNT*/ )) 
			curvert=cmd_peditvtx_right(pl_elist,curvert,nmax);
		else if (strisame(retkeyword,"LEFT"/*DNT*/ )) 
			curvert=cmd_peditvtx_left(pl_elist,curvert,nmax);
		else if (strisame(retkeyword,"UP"/*DNT*/ )) 
			curvert=cmd_peditvtx_up(pl_elist,curvert,mmax,nmax);
		else if (strisame(retkeyword,"DOWN"/*DNT*/ ))
			curvert=cmd_peditvtx_down(pl_elist,curvert,mmax,nmax);
		else if (strisame(retkeyword,"REGEN"/*DNT*/ ))
			sds_entupd(polyent);
		else if (strisame(retkeyword,"ANGLE"/*DNT*/ ))
			cmd_peditvtx_angle(polyent,pl_elist,curvert,lastcursorpoint,laststartpoint,drawxflag,plextruptr,&rbucs,&rbent);
		else if (strisame(retkeyword,"WIDTH"/*DNT*/ ))
			cmd_peditvtx_width(polyent,pl_elist,curvert,lastcursorpoint,laststartpoint,drawxflag,plextruptr,&rbucs,&rbent);
		else if (strisame(retkeyword,"STRAIGHTEN"/*DNT*/ )) 
			cmd_peditvtx_straighten(polyent,&pl_elist,&curvert,lastcursorpoint,laststartpoint,drawxflag,plextruptr);
		else if (strisame(retkeyword,"MOVE"/*DNT*/ ))
			cmd_peditvtx_move(polyent,pl_elist,curvert,lastcursorpoint,laststartpoint,drawxflag,plextruptr,&rbucs,&rbent);
		else if (strisame(retkeyword,"INSERT"/*DNT*/ ))
			cmd_peditvtx_insert(polyent,&pl_elist,&curvert,lastcursorpoint,laststartpoint,drawxflag,plextruptr,&rbucs,&rbent);
		else if (strisame(retkeyword,"TANGENT"/*DNT*/ ))
            sds_printf("\nA tangent direction is not used in IntelliCAD for curve fitting." );
		else if (strisame(retkeyword,"BREAK"/*DNT*/ ))
			cmd_peditvtx_break(polyent,&pl_elist,&curvert,lastcursorpoint,laststartpoint,drawxflag,plextruptr,&rbucs,&rbent);
		}

exit:
	if (pl_elist!=NULL) 
		{
		cmd_pedit_drawx(lastcursorpoint,drawxflag,plextruptr);	// undraw last cursor
		cmd_pedit_drawst(laststartpoint,drawxflag,plextruptr);	// undraw last start indicator
		cmd_free_elistll(&pl_elist,&junkptr);					// free the polyline -- junkptr is unused (see comment in cmd_free_elistll)
		}

	return RTNORM;
	}



int cmd_reset_plwidth(sds_name pent,sds_real width) {
    sds_name etemp1;
	struct resbuf *rbp1,*rbptemp;

	rbp1=NULL;
	ic_encpy(etemp1,pent);
	do{
		FreeResbufIfNotNull(&rbp1);
        if ((rbp1=sds_entget(etemp1))==NULL)return(-1);
		for (rbptemp=rbp1; rbptemp!=NULL; rbptemp=rbptemp->rbnext) {
			if (rbptemp->restype==0 && strsame(rbptemp->resval.rstring,"SEQEND"/*DNT*/))goto done;
			else if (rbptemp->restype==40 || rbptemp->restype==41)rbptemp->resval.rreal=width;
		}
        if (sds_entmod(rbp1)!=RTNORM) {
            sds_relrb(rbp1);
			rbp1=rbptemp=NULL;
			return(-1);
		}
        if (sds_entnext_noxref(etemp1,etemp1)!=RTNORM)return(-1);
	}while(1);
	done:
	FreeResbufIfNotNull(&rbp1);
    if (sds_entupd(pent)!=RTNORM)return(-1);
	return(0);
}

int cmd_pline_next2lastvtx(sds_name mainent,sds_name next2lastvtx) {
    sds_name etemp1,etemp2,etemp3;
	struct resbuf *rbp1;
	int vtxcnt=0;

	rbp1=NULL;
    if (sds_entnext_noxref(mainent,etemp3)!=RTNORM) return(-1);
	ic_encpy(etemp2,etemp3);
	do{
		FreeResbufIfNotNull(&rbp1);
		vtxcnt++;
		ic_encpy(etemp1,etemp2);
		ic_encpy(etemp2,etemp3);
        if (sds_entnext_noxref(etemp3,etemp3)!=RTNORM)return(-1);
        if ((rbp1=sds_entget(etemp3))==NULL)return(-1);
		ic_assoc(rbp1,0);
	}while(!strsame(ic_rbassoc->resval.rstring,"SEQEND"/*DNT*/));
	FreeResbufIfNotNull(&rbp1);
	if (vtxcnt==1)return(-1);
	ic_encpy(next2lastvtx,etemp1);
	return(0);
}

int cmd_pedit_drawx(sds_point drawpt,int mode, sds_point extru) {
	//NOTE: if mode==0, drawpt is in wcs
	//		if mode==1, drawpt is in ucs
	//		if mode==2, drawpt is in ecs defined by extru
	struct resbuf rb,rb2;
    sds_real fr1,fr2,drawdist;
	int fi1;
    sds_point p0,p1,p2;

	if (SDS_getvar(NULL,DB_QVIEWSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) return(-1);
	fr1=rb.resval.rreal;
	rb.restype=RTPOINT;
	if (SDS_getvar(NULL,DB_QSCREENSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) return(-1);
	fr2=rb.resval.rpoint[1];
	rb.restype=RTSHORT;
	if (SDS_getvar(NULL,DB_QAPERTURE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) return(-1);
	fi1=rb.resval.rint;
	drawdist=2.0*fr1*fi1/fr2;

	if (mode<2) {
		rb.restype=RTSHORT;
		rb.resval.rint=mode;
	}else{
		rb.restype=RT3DPOINT;
		ic_ptcpy(rb.resval.rpoint,extru);
	}
	rb2.restype=RTSHORT;
	rb2.resval.rint=2;//DCS
	sds_trans(drawpt,&rb,&rb2,0,p0);
	rb.restype=RTSHORT;
	rb.resval.rint=1;

    sds_polar(p0,0.78534,drawdist,p1);
    sds_polar(p0,3.92670,drawdist,p2);
	sds_trans(p1,&rb2,&rb,0,p1);
	sds_trans(p2,&rb2,&rb,0,p2);
    fi1=sds_grdraw(p1,p2,-1,0);

    sds_polar(p0,2.35619,drawdist,p1);
    sds_polar(p0,5.49779,drawdist,p2);
	sds_trans(p1,&rb2,&rb,0,p1);
	sds_trans(p2,&rb2,&rb,0,p2);
    sds_grdraw(p1,p2,-1,0);

	return(0);
}

int cmd_pedit_drawst(sds_point drawpt,int mode,sds_point extru) {
	struct resbuf rb,rb2;
    sds_real fr1,fr2,drawdist;
	int fi1;
    sds_point p0,p1,p2,p3;

	if (SDS_getvar(NULL,DB_QVIEWSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) return(-1);
	fr1=rb.resval.rreal;
	rb.restype=RTPOINT;
	if (SDS_getvar(NULL,DB_QSCREENSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) return(-1);
	fr2=rb.resval.rpoint[1];
	rb.restype=RTSHORT;
	if (SDS_getvar(NULL,DB_QAPERTURE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) return(-1);
	fi1=rb.resval.rint;
	drawdist=fr1*fi1/fr2;


	if (mode<2) {
		rb.restype=RTSHORT;
		rb.resval.rint=mode;
	}else{
		rb.restype=RT3DPOINT;
		ic_ptcpy(rb.resval.rpoint,extru);
	}
	rb2.restype=RTSHORT;
	rb2.resval.rint=2;//DCS
	sds_trans(drawpt,&rb,&rb2,0,p0);
	rb.restype=RTSHORT;
	rb.resval.rint=1;

    sds_polar(p0,1.57080,drawdist,p1);
    sds_polar(p0,3.66519,drawdist,p2);
    sds_polar(p0,5.75959,drawdist,p3);
	sds_trans(p1,&rb2,&rb,0,p1);
	sds_trans(p2,&rb2,&rb,0,p2);
	sds_trans(p3,&rb2,&rb,0,p3);

    sds_grdraw(p1,p2,-1,0);
    sds_grdraw(p2,p3,-1,0);
    sds_grdraw(p3,p1,-1,0);

	return(0);
}

int cmd_lengthen_pline_end(sds_name pline,sds_real dist) {
	//returns: -1=ERROR
	//			0=unable to shorten/lengthen further
	//			1=SUCCESS
	//dist may be > or < 0.0
	//NOTE:  PLINE CANNOT BE CLOSED!!!

	struct resbuf *rbp1,*rbp2,rb;
 	RT ret=1;
    sds_name e1,e2;
    sds_point p1,p2,p3;
    sds_real fr1,fr2,fr3,fr4,fr5=0.0,fr6,fr7,fr8,newlen,curlen;
	rb.restype=RTSTR;
	rb.resval.rstring=const_cast<char*>("*"/*DNT*/);
	rb.rbnext=NULL;
	rbp1=rbp2=NULL;

    sds_entmake(NULL);
    if ((rbp1=sds_entgetx(pline,&rb))==NULL) {ret=-1;goto exit;}
    if (sds_entmake(rbp1)!=RTNORM) {ret=-1;goto exit;}
    sds_relrb(rbp1);
	rbp1=NULL;

	if (dist>0.0) {
		cmd_pline_next2lastvtx(pline,e1);
		FreeResbufIfNotNull(&rbp1);
        if ((rbp1=sds_entget(e1))==NULL) {ret=-1;goto exit;}
		ic_assoc(rbp1,0);
        if (sds_entnext_noxref(e1,e2)!=RTNORM) {ret=-1;goto exit;}
		FreeResbufIfNotNull(&rbp2);
        if ((rbp2=sds_entget(e2))==NULL) {ret=-1;goto exit;}
		ic_assoc(rbp1,42);
		fr1=ic_rbassoc->resval.rreal;
		ic_assoc(rbp1,10);
		ic_ptcpy(p1,ic_rbassoc->resval.rpoint);
		ic_assoc(rbp2,10);
		ic_ptcpy(p2,ic_rbassoc->resval.rpoint);
		if (icadRealEqual(fr1,0.0)) {//adding onto a straight segment
            sds_polar(p2,sds_angle(p1,p2),dist,p3);
			ic_assoc(rbp2,10);
			ic_ptcpy(ic_rbassoc->resval.rpoint,p3);
            if (sds_entmod(rbp2)!=RTNORM) {ret=-1;goto exit;}
            sds_entupd(pline);
		}else{
			if (ic_bulge2arc(p1,p2,fr1,p3,&fr2,&fr3,&fr4)==-1) {
				cmd_ErrorPrompt(CMD_ERR_COINCVERPTS,0);
//              sds_printf("\nCannot extend from coincident curved vertices.");
				ret=-1;
				goto exit;
			}
			//adjust ending width....
			ic_normang(&fr3,&fr4);
			ic_assoc(rbp1,40);
			fr6=ic_rbassoc->resval.rreal;
			ic_assoc(rbp1,41);
			fr8=ic_rbassoc->resval.rreal;
			fr8=fr8+((fr8-fr6)*(1+(fr5/(fr4-fr3))));
			ic_rbassoc->resval.rreal=fr8;
			//adjust bulge
			fr5=dist/fr2;
			if (fr1>0.0) {//ccw extension
				fr6=fr4+fr5;
				ic_normang(&fr3,&fr6);
				for (;(fr6-fr3)>IC_TWOPI;fr6=fr6-IC_TWOPI);
				fr7=tan((fr6-fr3)/4.0);
			}else{
				fr6=fr3-fr5;
				ic_normang(&fr6,&fr4);
				for (;(fr4-fr6)>IC_TWOPI;fr4=fr4-IC_TWOPI);
				fr7=-tan((fr4-fr6)/4.0);
			}
			ic_assoc(rbp1,42);
			ic_rbassoc->resval.rreal=fr7;
            if (sds_entmod(rbp1)!=RTNORM) {ret=-1;goto exit;}
            sds_polar(p3,fr6,fr2,p2);
			ic_assoc(rbp2,10);
			ic_ptcpy(ic_rbassoc->resval.rpoint,p2);
            if (sds_entmod(rbp2)!=RTNORM) {ret=-1;goto exit;}
            sds_entupd(pline);
		}
	}else{
		db_edata(pline,NULL,NULL,&fr1,NULL);
		newlen=fr1+dist;  //dist <0
		curlen=0.0;
		if (fr1<fabs(dist))goto exit;
		ic_encpy(e1,pline);
		do{
			FreeResbufIfNotNull(&rbp1);
            if (sds_entnext_noxref(e1,e1)!=RTNORM) {ret=-1;goto exit;}
            if ((rbp1=sds_entget(e1))==NULL) {ret=-1;goto exit;}
			ic_assoc(rbp1,0);
			if (strsame(ic_rbassoc->resval.rstring,"SEQEND"/*DNT*/)) {ret=-1;goto exit;}
            if (sds_entnext_noxref(e1,e2)!=RTNORM) {ret=-1;goto exit;}
			FreeResbufIfNotNull(&rbp2);
            if ((rbp2=sds_entget(e2))==NULL) {ret=-1;goto exit;}
			ic_assoc(rbp2,0);
			if (strsame(ic_rbassoc->resval.rstring,"SEQEND"/*DNT*/)) {ret=-1;goto exit;}
			ic_assoc(rbp1,42);
			fr1=ic_rbassoc->resval.rreal;
			ic_assoc(rbp1,10);
			ic_ptcpy(p1,ic_rbassoc->resval.rpoint);
			ic_assoc(rbp2,10);
			ic_ptcpy(p2,ic_rbassoc->resval.rpoint);
			if (icadRealEqual(fr1,0.0)) {	//straight segment
                fr2=sds_distance(p1,p2);
				if ((curlen+fr2)<newlen) {
                    if (sds_entmake(rbp1)!=RTNORM) {ret=-1;goto exit;}
					curlen+=fr2;
					continue;
				}else{
                    sds_polar(p1,sds_angle(p1,p2),(newlen-curlen),p3);
					ic_assoc(rbp2,10);
					ic_ptcpy(ic_rbassoc->resval.rpoint,p3);
					ic_assoc(rbp1,40);
					fr3=ic_rbassoc->resval.rreal;
					ic_assoc(rbp1,41);
					fr4=ic_rbassoc->resval.rreal;
					ic_rbassoc->resval.rreal=fr3+((fr4-fr3)*(newlen-curlen)/fr2);
                    if (sds_entmake(rbp1)!=RTNORM) {ret=-1;goto exit;}
                    if (sds_entmake(rbp2)!=RTNORM) {ret=-1;goto exit;}
					break;
				}
			}else{	//curved segment
				if (ic_bulge2arc(p1,p2,fr1,p3,&fr3,&fr4,&fr5)==-1) {
                    if (sds_entmake(rbp1)!=RTNORM) {ret=-1;goto exit;}
					continue;
				}
				ic_normang(&fr4,&fr5);
				fr6=fr3*(fr5-fr4);
				if ((curlen+fr6)<newlen) {
                    if (sds_entmake(rbp1)!=RTNORM) {ret=-1;goto exit;}
					curlen+=fr6;
					continue;
				}else{
					//we want to end pline within curved segment...
					ic_assoc(rbp1,40);
					fr2=ic_rbassoc->resval.rreal;
					ic_assoc(rbp1,41);
					fr8=ic_rbassoc->resval.rreal;
					fr6=(newlen-curlen)/fr3;  //new included angle for segment
					fr8=fr2+(fr8-fr2)*fr6/(fr5-fr4);
					ic_rbassoc->resval.rreal=fr8;
					if (fr1>0.0) {
						fr7=fr4+fr6;
						fr2=tan(fr6/4.0);
					}else{
						fr7=fr5-fr6;
						fr2=-tan(fr6/4.0);
					}
                    sds_polar(p3,fr7,fr3,p2);
					ic_assoc(rbp2,10);
					ic_ptcpy(ic_rbassoc->resval.rpoint,p2);
					ic_assoc(rbp1,42);
					ic_rbassoc->resval.rreal=fr2;
                    if (sds_entmake(rbp1)!=RTNORM) {ret=-1;goto exit;}
                    if (sds_entmake(rbp2)!=RTNORM) {ret=-1;goto exit;}
					break;
				}
			}
		}while(1);
		FreeResbufIfNotNull(&rbp1);
        if ((rbp1=sds_buildlist(RTDXF0,"SEQEND"/*DNT*/,0))==NULL) {ret=-1;goto exit;}
        if (sds_entmake(rbp1)!=RTNORM) {ret=-1;goto exit;}
        sds_entdel(pline);
        sds_entlast(pline);
		sds_redraw(pline,IC_REDRAW_DRAW);
	}

	exit:
    sds_entmake(NULL);
	FreeResbufIfNotNull(&rbp1);
	FreeResbufIfNotNull(&rbp2);
	return(ret);
}

