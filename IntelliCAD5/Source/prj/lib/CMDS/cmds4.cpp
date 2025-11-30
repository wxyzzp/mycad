/* C:\DEV\LIB\CMDS\CMDS4.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

//*********************************************************************
//*********************************************************************
//**Commands contained in this file:  *********************************
//
//			BREAK		FILLET 		ORTHO  		TRIM
//			CHAMFER		GRAPHSCR	OSNAP		EXTEND
//			DIST		ID			REDRAW		FILLET
//			DRAGMODE	LIMITS		SETVAR
//			ELEV		LTSCALE		TEXTSCR
//*********************************************************************
//*********************************************************************
#include "cmds.h"/*DNT*/
#include "Icad.h"/*DNT*/
#include "IcadView.h"
#include "IcadDoc.h"
#include "IcadTextScrn.h"
#include "entities.h"
#include "curve.h"
#include "trnsform.h"
#include "compcrvs.h"

// used in extending and trimming of elliptical arcs
#include "GeoConvert.h"
#include "EntConversion.h"
#include "path.h"
#include "crvpair.h"
#include "elliparc.h"
#include "compcrvs.h"

//4M Item:77->
#include "icaddef.h"
//<-4M Item:77
#include "nurbs.h"
#include "splineutils.h"

// warning about possible divide by 0
// disabled so icadRealEqual will compile, even if inlined all over the place
//
#pragma warning(disable:4723)


extern int SDS_MultibleMode;
extern int lsp_cmdhasmore;
// Modified PK 31/05/2000
extern int calledfrom_cmd_setvar; // Reason : Fix for bug no. 74219

static const double HIT_TOLERANCE = 0.1;
static const double EPSILON = 0.001;
static const double INT_EPSILON = 0.001;
static const double INT_EPSILON1 = 1.0e-6;

struct cmd_elistll_ptr {			//this struct will point to main entity's elistll ptr
	struct cmd_elistll *elistll;
	struct cmd_elistll_ptr *next;
};

sds_real sds_dist2d (sds_point p1,sds_point p2);//proto for f'n found in sds1

//*********************************************************************
//****PROTOS FOR SUPPORT FUNCTIONS FOUND AT END OF FILE****************
//*********************************************************************

int cmd_trim_object(sds_name ename, sds_point epoint, cmd_elistll *bdry_setll,sds_name xtraent, long trimsetlen);

int cmd_extend_object(sds_name ename, sds_point epoint, sds_name ss);

int cmd_adj_pline_trimpts(sds_name plname,sds_point chkpt,sds_point pickpt, sds_point trimpt1,sds_point trimpt2,
						  int plineflag, sds_point start, sds_point end,
						  int (*distfunc)(sds_name, sds_point, sds_point, sds_real *));
int cmd_pt_begsegdist (sds_point p0, sds_point p1, sds_real bulge, sds_point ptest, sds_real *dist);
void  cmd_adj_arc_trimang(sds_point cc, sds_real pickang, sds_point chkpt,
							sds_real *trimang1, sds_real *trimang2);
int cmd_get_textpts(struct resbuf *rbp,sds_point p1,sds_point p2,sds_point p3,sds_point p4);
void cmd_adj_line_trimpt(sds_point epoint, sds_point testpt, sds_point linestart, sds_point lineend,
						 sds_point starttrim, sds_point endtrim);
void cmd_adj_xline_trimpt(sds_point epoint, sds_point testpt, sds_point linestart, sds_point linevect,
						  int *ptchk, sds_point starttrim, sds_point endtrim);
void cmd_adj_ray_trimpt(sds_point epoint, sds_point testpt, sds_point raystart, sds_point rayvect,
						sds_point starttrim, sds_point endtrim);
void  cmd_pt_2_plseg (sds_name vtx1, sds_point pt);
void cmd_pt_2_lwplseg (db_lwpolyline *lwpline, int vertno, sds_point pt);
int cmd_trim_or_extend(int trimit);
int cmd_pline_breakup(sds_name ename, sds_point plstart, sds_point plend, int closure, sds_name e1, sds_point trimpt1,
					  bool specialcaseflag, sds_name e2, sds_point trimpt2, int trimret, sds_name xtraent);
int cmd_lwpline_breakup(sds_name ename, sds_point plstart, sds_point plend,
						int closure, int pt1vertno, sds_point trimpt1, bool specialcaseflag,
						int pt2vertno, sds_point trimpt2, int trimret, sds_name xtraent);
int cmd_lwpline_dist(sds_name lwplename, sds_point pFrom, sds_point pTo, sds_real *dist);

// EBATECH(watanabe)-[add arrow block
void cmd_addarrowblock(char* str);
// ]-EBATECH(watanabe)

//*********************************************************************
//*********************************************************************
//*********************************************************************

// EBATECH(watanabe)-[
bool isArrowBlock(char* str)
{
	if (strisame(ResourceString(IDC_CMDS4__NONE_188,		"NONE"/*DNT*/), 		str)||
		strisame(ResourceString(IDC_CMDS4__CLOSED_190,		"CLOSED"/*DNT*/),		str)||
		strisame(ResourceString(IDC_CMDS4__DOT_192, 		"DOT"/*DNT*/),			str)||
		strisame(ResourceString(IDC_CMDS4__CLOSEDBLANK_194, "CLOSEDBLANK"/*DNT*/),	str)||
		strisame(ResourceString(IDC_CMDS4__OBLIQUE_196, 	"OBLIQUE"/*DNT*/),		str)||
		strisame(ResourceString(IDC_CMDS4__ARCHTICK_198,	"ARCHTICK"/*DNT*/), 	str)||
		strisame(ResourceString(IDC_CMDS4__OPEN_200,		"OPEN"/*DNT*/), 		str)||
		strisame(ResourceString(IDC_CMDS4__ORIGIN_202,		"ORIGIN"/*DNT*/),		str)||
		strisame(ResourceString(IDC_CMDS4__OPEN90_204,		"OPEN90"/*DNT*/),		str)||
		strisame(ResourceString(IDC_CMDS4__OPEN30_206,		"OPEN30"/*DNT*/),		str)||
		strisame(ResourceString(IDC_CMDS4__DOTBLANK_208,	"DOTBLANK"/*DNT*/), 	str)||
		strisame(ResourceString(IDC_CMDS4__DOTSMALL_210,	"DOTSMALL"/*DNT*/), 	str)||
		strisame(ResourceString(IDC_CMDS4__BOXFILLED_212,	"BOXFILLED"/*DNT*/),	str)||
		strisame(ResourceString(IDC_CMDS4__BOXBLANK_214,	"BOXBLANK"/*DNT*/), 	str)||
		strisame(ResourceString(IDC_CMDS4__DATUMFILLED_216, "DATUMFILLED"/*DNT*/),	str)||
		strisame(ResourceString(IDC_CMDS4__DATUMBLANK_218,	"DATUMBLANK"/*DNT*/),	str)||
		strisame(ResourceString(IDC_CMDS4__INTEGRAL_220,	"INTEGRAL"/*DNT*/), 	str))
	{
		return true;
	}
	return false;
}
// ]-EBATECH(watanabe)

short cmd_trim(void){
	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	if (cmd_istransparent() == RTERROR) return RTERROR;

	return(RT(cmd_trim_or_extend(1)));
}
short cmd_extend(void){
	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	if (cmd_istransparent() == RTERROR) return RTERROR;

	return(RT(cmd_trim_or_extend(0)));
}


bool cmd_ucs_match_wcs(const C3Point &ucsOrigin, const C3Vector &ucsXDir, const C3Vector &ucsYDir)
{
    return (icadIsZero(ucsOrigin.X()) && 
            icadIsZero(ucsOrigin.Y()) && 
            icadIsZero(ucsOrigin.Z()) &&
            icadIsZero(ucsXDir.Y()) && 
            icadIsZero(ucsXDir.Z()) && 
            icadIsZero(ucsYDir.X()) &&
            icadIsZero(ucsYDir.Z()) &&
            icadRealEqual(ucsXDir.X(), 1.0) &&
            icadRealEqual(ucsYDir.Y(), 1.0));
}


int cmd_trim_or_extend(int trimit)
	{

	//NOTE:  front end for trim same as extend.  just change prompt & ssget filter
	//	if trimit non-zero, we'll trim.  Otherwise, we'll extend
	char fs1[IC_ACADBUF],fs2[IC_ACADBUF],fs3[IC_ACADBUF];
	RT ret;
	long fl1,trimsetlen;
	int edgemode,projmode;
	sds_name ss1,ss2,ename,xtraent;
	sds_point epoint,pt1,pt2;
	struct resbuf rb,*rbfence=NULL,*rbtemp,*rbtemp2;
	struct cmd_elistll *elist_llbeg, *elist_llcur;
	struct cmd_elistll_ptr *undo_beg, *undo_cur, *undo_temp;
	struct cmd_elistll *bdry_setll;


	elist_llbeg=elist_llcur=NULL;
	undo_beg=undo_cur=undo_temp=NULL;
	bdry_setll=NULL;

	//*** Get selection set of entities to trim/extend.

	SDS_getvar(NULL,DB_QEDGEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	edgemode=rb.resval.rint;
	SDS_getvar(NULL,DB_QPROJMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	projmode=rb.resval.rint;
	rb.restype=0;
	rb.resval.rstring = "LINE,ARC,CIRCLE,ELLIPSE,SPLINE,LWPOLYLINE,POLYLINE,TEXT,RAY,XLINE"/*DNT*/;
	rb.rbnext=NULL;
	xtraent[0]=xtraent[1]=0;

	if (trimit)
		ret=sds_pmtssget(ResourceString(IDC_CMDS4__NSELECT_CUTTING_E_1, "\nSelect cutting entities for trim <ENTER to select all>: " ),NULL,NULL,NULL,&rb,ss1,0);
	else
		ret=sds_pmtssget(ResourceString(IDC_CMDS4__NSELECT_BOUNDARY__2, "\nSelect boundary entities for extend <ENTER to select all>: " ),NULL,NULL,NULL,&rb,ss1,0);

	//NOTE:  Trim and extend don't go by ENAME, in case one of the boundary ents is altered.
	//	We have to take a "snap shot" of the ents NOW in case one of these is altered in the
	//	trimming/extending process.
	if (ret==RTERROR)//
		{
		//user tried and didnt get any ents OR user hit enter, so select all items & drawing was empty
	   if (RTNORM!=sds_pmtssget(NULL,"~ALL ~_ALL"/*DNT*/ ,NULL,NULL,NULL,ss1,0))
		   {
			cmd_ErrorPrompt(CMD_ERR_NOSELECT,0);
			goto exit;
			}
		SDS_getvar(NULL,DB_QCMDECHO,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		if (CMD_CMDECHO_PRINTF(rb.resval.rint))
			{
			if (trimit)
				sds_printf(ResourceString(IDC_CMDS4__NUSING_ALL_ENTITI_4, "\nUsing all entities as trim boundary." ));
			else
				sds_printf(ResourceString(IDC_CMDS4__NUSING_ALL_ENTITI_5, "\nUsing all entities as extend boundary." ));
			}

		}
	else if (ret!=RTNORM)
		return(ret); //this return is ok, nothing's been allocated yet.

	if (RemoveHatchFromSS(ss1) == RTNIL)
		{
		sds_ssfree(ss1);
		return -1;
		}

	if (trimit)
		{
		sds_sslength(ss1,&trimsetlen);
		if ((ret=cmd_ss2elistll(&bdry_setll,NULL,ss1,0))!=0)   //grpptr set to group 0
			{
			sds_ssfree(ss1);
			goto exit;
			}
   		sds_ssfree(ss1);
		}

	sds_initget(0,ResourceString(IDC_TRIM_EDGEMODE_INIT1, "Edge_mode|Edge Fence Projection_mode|Projection ~_Edge ~_Fence ~_Projection"));
	sprintf(fs3,ResourceString(IDC_CMDS4__NEDGE_MODE___FENC_7, "\nEdge mode/Fence/Projection/<Select entity to %s>: " ),(trimit)?ResourceString(IDC_CMDS4_TRIM_8, "trim" ):ResourceString(IDC_CMDS4_EXTEND_9, "extend" ));
	while (((ret=(sds_entsel(fs3,ename,epoint)))==RTNORM)||(ret==RTKWORD)||(ret==RTERROR))
		{
		if (ret==RTERROR)
			{
			SDS_getvar(NULL,DB_QERRNO,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			if (rb.resval.rint==OL_ENTSELPICK)
				{
				cmd_ErrorPrompt(CMD_ERR_NOENT,0);
				if (undo_beg!=NULL)
					{
					sds_initget(0,ResourceString(IDC_TRIM_EDGEMODE_INIT2,"Edge_mode|Edge Fence Projection ~ Undo ` ~_Edge ~_Fence ~_Projection ~_ ~_Undo"));
					sprintf(fs3,ResourceString(IDC_CMDS4__NEDGE_MODE___FEN_11, "\nEdge mode/Fence/Projection/Undo/<Select entity to %s>: " ),(trimit)?ResourceString(IDC_CMDS4_TRIM_8, "trim" ):ResourceString(IDC_CMDS4_EXTEND_9, "extend" ));
					}
				else
					{
					sds_initget(0,ResourceString(IDC_TRIM_EDGEMODE_INIT3, "Edge_mode|Edge Fence Projection ` ~_Edge ~_Fence ~_Projection"));
					sprintf(fs3,ResourceString(IDC_CMDS4__NEDGE_MODE___FENC_7, "\nEdge mode/Fence/Projection/<Select entity to %s>: " ),(trimit)?ResourceString(IDC_CMDS4_TRIM_8, "trim" ):ResourceString(IDC_CMDS4_EXTEND_9, "extend" ));
					}
				continue;
				}
			else
				{
				ret=RTNORM;
				goto exit;
				}
			}
		if (xtraent[0]!=0)
			{
			//insert dummy record to beginning of undo_cur->elistll where
			//	->entlst is left to null but ->ename points to ent to delete
			//hey, I know this is alot for one ename, but it's easy to write			
			if ((elist_llcur= new struct cmd_elistll )==NULL)
				{
				cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
				ret=(-__LINE__);
				goto exit;
				}
			memset(elist_llcur,0,sizeof(struct cmd_elistll));
			elist_llcur->next=undo_cur->elistll;
			elist_llbeg=elist_llcur;
			undo_cur->elistll=elist_llbeg;
			ic_encpy(elist_llbeg->ename,xtraent);
			xtraent[0]=xtraent[1]=0;
			}
		if (ret==RTNORM)
			{
			//*****SETUP COMMAND's UNDO LIST FOR CURRENT STATUS OF ENTITY*****
			//NOTE:  We'll keep current (old version of) ename & it's properties
			//	in undo_cur->elistll->entlst.  We'll keep modified ent's ename
			//	in undo_cur->elistll->ename.

			elist_llbeg=elist_llcur=NULL;
			cmd_alloc_elistll(&elist_llbeg,&elist_llcur,ename,0);
			if (undo_beg==NULL)
				{
				//*** Allocate a new link in list.
				if ((undo_temp= new struct cmd_elistll_ptr )==NULL)
					{
					cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
					ret=(-__LINE__);
					goto exit;
					}
				memset(undo_temp,0,sizeof(struct cmd_elistll_ptr));
				undo_beg=undo_cur=undo_temp;
				undo_cur->elistll=elist_llbeg; //needed to start entmake in
											   //proper place and set undo entity
				}
			else
				{
				if ((undo_temp= new struct cmd_elistll_ptr )==NULL)
					{
					cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
					ret=(-__LINE__);
					goto exit;
					}
				memset(undo_temp,0,sizeof(struct cmd_elistll_ptr));
				undo_cur->next=undo_temp;
				undo_cur=undo_cur->next;
				undo_cur->elistll=elist_llbeg;
				}
			}
		if (ret==RTKWORD)
			{
			sds_getinput(fs1);
			if (strisame(fs1,"UNDO" ))
				{
				//undo last operation
				if (undo_beg!=NULL)
					{
	 				undo_temp=undo_cur;
					//if ((ret=sds_entdel(undo_cur->elistll->ename))!=RTNORM)goto exit;
					ic_encpy(ename,undo_cur->elistll->ename);
					if (undo_cur->elistll->entlst==NULL)
						{
						//an extra ent made by trimming out from center..
						if ((ret=sds_entdel(ename))!=RTNORM)goto exit;
						elist_llcur=undo_cur->elistll;
						undo_cur->elistll=undo_cur->elistll->next;
						elist_llcur->next=NULL;
						cmd_free_elistll(&elist_llcur,NULL);
						ic_encpy(ename,undo_cur->elistll->ename);
						}
					ic_assoc(undo_cur->elistll->entlst,-1);
					if (ic_rbassoc->resval.rlname[0]==ename[0] && ic_rbassoc->resval.rlname[1]==ename[1])
						{
						do
							{
							if (sds_entmod(undo_temp->elistll->entlst)!=RTNORM)
								{
								sds_entdel(ename);
								ret=(-__LINE__);
								goto exit;
								}
							}
						while ((undo_temp->elistll=undo_temp->elistll->next)!=NULL);
						}
					else
						{
						if ((ret=sds_entdel(ename))!=RTNORM)
							goto exit;
						do
							{
							if (sds_entmake(undo_temp->elistll->entlst)!=RTNORM)
								{
								sds_entdel(ename);
								ret=(-__LINE__);
								goto exit;
								}
							}
						while ((undo_temp->elistll=undo_temp->elistll->next)!=NULL);
						}

					//step to element just before cur where elistll!=NULL
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
						sds_printf(ResourceString(IDC_CMDS4__NEVERYTHING_HAS__14, "\nEverything has been undone." ));
						}
					}
				else
					{
					cmd_ErrorPrompt(CMD_ERR_NOMOREUNDO,0);//should never get here
					}
			}
			else if (strisame(fs1,"EDGE" ))
				{
				sprintf(fs1,ResourceString(IDC_CMDS4__NEXTEND___NO_EXT_16, "\nExtend/No extend <%s>: " ),(edgemode)?ResourceString(IDC_CMDS4_EXTEND_17, "Extend" ):ResourceString(IDC_CMDS4_NO_EXTEND_18, "No extend" ));
				sds_initget(0,ResourceString (IDC_EXTEND_EDGE_INIT1, "Extend No_extend|No ~_EXTEND ~_NO"));
				if ((ret=sds_getkword(fs1,fs2))==RTNORM)
					{
					rb.restype=RTSHORT;
					rb.rbnext=NULL;
					if (strisame(fs2,"NO" ))
						rb.resval.rint=edgemode=0;
					else
						rb.resval.rint=edgemode=1;
					if (sds_setvar("EDGEMODE"/*DNT*/,&rb)!=RTNORM) {ret=(-__LINE__); goto exit;}
					}
				else if (RTERROR==ret || RTCAN==ret)
					goto exit;
				}
			else if (strisame(fs1,"PROJECTION" ))
				{
				if (projmode==1)
				   strncpy(fs2,"UCS"/*DNT*/,sizeof(fs2)-1);
				else if (projmode==2)
				   strncpy(fs2,"VIEW"/*DNT*/,sizeof(fs2)-1);
				else
				   strncpy(fs2,"NONE"/*DNT*/,sizeof(fs2)-1);
				sprintf(fs1,ResourceString(IDC_CMDS4__NPROJECTION___NO_26, "\nProjection:  None/Ucs/View <%s>: " ),fs2);
				sds_initget(0, ResourceString (IDC_TRIM_PROJECTION_INIT1, "No_projection|None xy_plane_of_UCS|Ucs Current_view|View ~_NONE ~_UCS ~_VIEW"));
				if (sds_getkword(fs1,fs2)==RTNORM)
					{
					rb.restype=RTSHORT;
					rb.rbnext=NULL;
					if (strisame(fs2,"NONE" ))
						rb.resval.rint=projmode=0;
					else if (strisame(fs2,"UCS" ))
						rb.resval.rint=projmode=1;
					else
						rb.resval.rint=projmode=2;
					if (sds_setvar("PROJMODE"/*DNT*/,&rb)!=RTNORM) {ret=(-__LINE__); goto exit;}
					}
				}
			else if (strisame(fs1,"FENCE" ))
				{
				//let user pick a fence to define entities and their selected points...
				if (RTNORM!=(ret=internalGetpoint(NULL,ResourceString(IDC_CMDS4__NFIRST_POINT_OF__30, "\nFirst point of fence: " ),pt1)))
					goto exit;
				for (;;)
					{
					if (RTNORM!=(ret=internalGetpoint(pt1,ResourceString(IDC_CMDS4__NNEXT_FENCE_POIN_31, "\nNext fence point: " ),pt2)))
						break;
					if (rbfence!=NULL)
						sds_relrb(rbfence);
					rbfence=sds_buildlist(RT3DPOINT,pt1,RT3DPOINT,pt2,0);
					if (RTNORM!=(ret=sds_pmtssget(NULL,"F"/*DNT*/,rbfence,NULL,NULL,ss2,0)))
						{
						//if nothing crosses the fence, keep getting points
						ic_ptcpy(pt1,pt2);
						continue;
						}
					sds_relrb(rbfence);
					rbfence=NULL;
					if (ic_entxss(pt1,pt2,ss2,CMD_FUZZ,&rbfence,&fl1,2)!=0)
						{
						sds_ssfree(ss2);
						ret=RTERROR;
						goto exit;
						}
					sds_ssfree(ss2);
					for (rbtemp=rbfence; rbtemp!=NULL && rbtemp->rbnext!=NULL; rbtemp=rbtemp->rbnext->rbnext)
						{
						ic_encpy(ename,rbtemp->resval.rlname);
						//check to see if we already trimmed this entity once
						for (rbtemp2=rbfence; rbtemp2!=NULL && rbtemp2->rbnext!=NULL; rbtemp2=rbtemp2->rbnext->rbnext)
							{
							if (rbtemp2==rbtemp)
								break;
							if (rbtemp2->resval.rlname[0]==ename[0] && rbtemp2->resval.rlname[1]==ename[1])
								{
								//if we already did this entity...
								rbtemp2=NULL;
								break;
								}
							}
						if (rbtemp2==NULL)
							continue;
						ic_ptcpy(epoint,rbtemp->rbnext->resval.rpoint);
						//*****SETUP UNDO LIST FOR CURRENT STATUS OF ENTITY*****
						//NOTE:  We'll keep current (old version of) ename & it's properties
						//	in undo_cur->elistll->entlst.  We'll keep modified ent's ename
						//	in undo_cur->elistll->ename.
						elist_llbeg=elist_llcur=NULL;
						cmd_alloc_elistll(&elist_llbeg,&elist_llcur,ename,0);
						if (undo_beg==NULL)
							{
							//*** Allocate a new link in list.
							if ((undo_temp= new struct cmd_elistll_ptr )==NULL)
								{
								cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
								ret=(-__LINE__);
								goto exit;
								}
							memset(undo_temp,0,sizeof(struct cmd_elistll_ptr));
							undo_beg=undo_cur=undo_temp;
							undo_cur->elistll=elist_llbeg; //needed to start entmake in
														   //proper place and set undo entity
							}
						else
							{
							if ((undo_temp= new struct cmd_elistll_ptr )==NULL)
								{
								cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
								ret=(-__LINE__);
								goto exit;
								}
							memset(undo_temp,0,sizeof(struct cmd_elistll_ptr));
							undo_cur->next=undo_temp;
							undo_cur=undo_cur->next;
							undo_cur->elistll=elist_llbeg;
							}
						if (trimit)
							ret=cmd_trim_object(ename,epoint,bdry_setll,xtraent,trimsetlen);
						else
							ret=cmd_extend_object(ename,epoint,ss1);
						if (xtraent[0]!=0)
							{
							//insert dummy record to beginning of undo_cur->elistll where
							//	->entlst is left to null but ->ename points to ent to delete
							//hey, I know this is alot for one ename, but it's easy to write
							if ((elist_llcur= new struct cmd_elistll )==NULL)
								{
								cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
								ret=(-__LINE__);
								goto exit;
								}
							memset(elist_llcur,0,sizeof(struct cmd_elistll));
							elist_llcur->next=undo_cur->elistll;
							elist_llbeg=elist_llcur;
							undo_cur->elistll=elist_llbeg;
							ic_encpy(elist_llbeg->ename,xtraent);
							xtraent[0]=xtraent[1]=0;
							}
						}
					FreeResbufIfNotNull(&rbfence);
					ic_ptcpy(pt1,pt2);
					}
				}
			}
		else  //entsel returned RTNORM, so we're going to trim the entity
			{
			// Set the Undo so we corectly redraw the entity Undo.
			SDS_SetUndo(IC_UNDO_ENTITY_REDRAW,(void *)ename[0],NULL,NULL,SDS_CURDWG);
			if (trimit)
				ret=cmd_trim_object(ename,epoint,bdry_setll,xtraent,trimsetlen);
			else
				ret=cmd_extend_object(ename,epoint,ss1);

			if (ret!=RTCAN && ret!=RTERROR)
				cmd_updateReactors((db_handitem*) ename[0]);

			//NOTE: line below necessary ONLY IF we do an entmake and change ename,
			//		which trim sometimes does....
			ic_encpy(undo_cur->elistll->ename,ename);//ename MAY have changed
			if (ret>=0)
				{
				//sds_name elast;
				//sds_entlast(elast);
				// Set the Undo so we corectly redraw the entity Redo.
				SDS_SetUndo(IC_UNDO_ENTITY_REDRAW,(void *)ename[0],NULL,NULL,SDS_CURDWG);
				if (xtraent[0]!=0L)
					SDS_SetUndo(IC_UNDO_ENTITY_REDRAW,(void *)xtraent[0],NULL,NULL,SDS_CURDWG);
				}
			}

		//reset initget before testing entsel condition again...
		if (undo_beg!=NULL)
			{
			sds_initget(0,ResourceString(IDC_TRIM_EDGEMODE_INIT4, "Edge_mode|Edge Fence Projection_mode|Projection ~ Undo ` ~_EDGE ~_FENCE ~_PROJECTION ~_ ~_UNDO"));
			sprintf(fs3,ResourceString(IDC_CMDS4__NEDGE_MODE___FEN_11, "\nEdge mode/Fence/Projection/Undo/<Select entity to %s>: " ),(trimit)?ResourceString(IDC_CMDS4_TRIM_8, "trim" ):ResourceString(IDC_CMDS4_EXTEND_9, "extend" ));
			}
		else
			{
			sds_initget(0, ResourceString(IDC_TRIM_EDGEMODE_INIT5, "Edge_mode|Edge Fence Projection_mode|Projection ` ~_EDGE ~_FENCE ~_PROJECTION"));
			sprintf(fs3,ResourceString(IDC_CMDS4__NEDGE_MODE___FENC_7, "\nEdge mode/Fence/Projection/<Select entity to %s>: " ),(trimit)?ResourceString(IDC_CMDS4_TRIM_8, "trim" ):ResourceString(IDC_CMDS4_EXTEND_9, "extend" ));
			}
		}

	if (ret==RTCAN || ret==RTERROR)
		ret=-1;

	exit:

	undo_temp=undo_cur=undo_beg;
	while (undo_cur!=NULL)
		{
		undo_temp=undo_cur->next;
		cmd_free_elistll(&undo_cur->elistll,NULL);
		delete undo_cur;
		undo_cur=NULL;
		undo_cur=undo_temp;
		}
	undo_beg=NULL;
	if (trimit && bdry_setll!=NULL)
		{
		//NOTE: this redraw operation must be AFTER undo llist is freed, because
		//	we're temporarily using one of its pointers!
		for (elist_llcur=bdry_setll; elist_llcur!=NULL; elist_llcur=elist_llcur->next)
			sds_redraw(elist_llcur->ename,IC_REDRAW_DRAW);
		cmd_free_elistll(&bdry_setll,NULL);
		}
	if (!trimit)
		{
		for (long fl1=0L; RTNORM==sds_ssname(ss1,fl1,ename); fl1++,sds_redraw(ename,IC_REDRAW_DRAW));
		sds_ssfree(ss1);
		}
	FreeResbufIfNotNull(&rbfence);
	if (ret==0)
		return(RTNORM);
	if (ret==-1)
		return(RTCAN);
	//if (ret<-1)  CMD_INTERNAL_MSG(__LINE__);
	return(ret);
	}


int cmd_lwpline_entnext(int thisvert,db_lwpolyline *lwpl)
	{
	int closed,nextvert;

	if (lwpl->ret_70() & IC_PLINE_CLOSED)
		closed=1;
	else
		closed=0;

	nextvert=thisvert+1;
	if (nextvert==lwpl->ret_90())
		{
		if (closed)
			nextvert=0;
		else
			nextvert=-1;  // error
		}
	return nextvert;
	}

void LwplrbSetNumVerts(resbuf *rb)
	{
	int vertcount=0;
	resbuf *rb90=NULL;
	resbuf *tmprb=rb;

	while (tmprb!=NULL)
		{
		if (tmprb->restype==90)
			rb90=tmprb;
		if (tmprb->restype==10)
			vertcount++;
		tmprb=tmprb->rbnext;
		}
		if (rb90!=NULL)
			rb90->resval.rint=vertcount;
	}

void LwplrbDeleteVertexData(resbuf *lwpl)
	{
		resbuf *delrb;
		// delete all of the vertex data
	delrb=lwpl;
	while (delrb->rbnext->restype!=90)
		delrb=delrb->rbnext;
	delrb->rbnext->resval.rint=0;			// no verts right now -- we will update later
	while (delrb->rbnext->restype!=10)		// find the verts
		delrb=delrb->rbnext;
	sds_relrb(delrb->rbnext);				// delete vertex data and terminate
	delrb->rbnext=NULL;
	}

void LwplrbAddVertFromData(resbuf **lwpladdrb, sds_point pt, sds_real startwidth, sds_real endwidth, sds_real bulge)
	{
	(*lwpladdrb)->rbnext=sds_buildlist(10,pt,40,startwidth,41,endwidth,42,bulge,0);
	while ((*lwpladdrb)->rbnext!=NULL)
		(*lwpladdrb)=(*lwpladdrb)->rbnext;
	}

void LwplrbAddVert(db_lwpolyline *lwpl, resbuf **lwpladdrb, int vertno)
	{
	sds_point pt;
	sds_real startwidth, endwidth, bulge;

	lwpl->get_10(pt,vertno);
	lwpl->get_40(&startwidth, vertno);
	lwpl->get_41(&endwidth, vertno);
	lwpl->get_42(&bulge, vertno);
	LwplrbAddVertFromData(lwpladdrb, pt, startwidth, endwidth, bulge);
	}

void LwplrbAddVerts(db_lwpolyline *lwpl, resbuf **lwpladdrb,int startvert,int endvert, bool circular)
// adds verts from lwpline to resbuf list, up to BUT NOT INCLUDING endvert
	{
	while (startvert!=endvert)
		{
		LwplrbAddVert(lwpl,lwpladdrb,startvert);
		if (circular)
			startvert=cmd_lwpline_entnext(startvert,lwpl);
		else
			startvert++;
		}
	}

short cmd_break(void)
	{

	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	if (cmd_istransparent() == RTERROR) return RTERROR;

	char etype[20];
	RT rc=0;
	int entdel_flag=0,plineflag=0,gotpoint=0,trim_ends;
	long fl1,fl2,fl3,pt1vertno,pt2vertno;
	sds_name ename, evtx1, evtx2, etemp, redraw_ent;
	struct resbuf *rbp1, *elist, *elist1, *rbp2, *rbptemp,rbucs,rbwcs,rbdcs,rbecs;
	sds_point pt1, pt2, ep1, ep2, ep10, ep20, ptemp1, ptemp2;
	sds_real fr1, fr2, fr3, fr4, fr5, fr6, plineelev;
	struct gr_view *view=SDS_CURGRVW;
	int exactvert;

	rbucs.restype=rbwcs.restype=rbdcs.restype=RTSHORT;
	rbwcs.resval.rint=0;
	rbucs.resval.rint=1;
	rbdcs.resval.rint=2;
	rbecs.restype=RT3DPOINT;

	elist=rbp1=rbp2=NULL;
	redraw_ent[0]=redraw_ent[1]=0; //if breaking pline in center, you must redraw both new ents after
								   //deleting old ent.	entlast only gives you one entity name to redraw
	rbp1=sds_buildlist(RTSTR,"*",0);
	for (;;)
		{
		gotpoint=0;
		if (elist!=NULL)
			{
			sds_relrb(elist);
			elist=NULL;
			}
		if (rbp2!=NULL)
			{
			sds_relrb(rbp2);
			rbp2=NULL;
			}
		rc=sds_pmtssget(ResourceString(IDC_CMDS4__NSELECT_ENTITY__113, "\nSelect entity to break: " ),"~ ~_"/*DNT*/,&rbp2,NULL,NULL,evtx1,1);
		if (rc==RTCAN)
			goto exit;
		else if (rc!=RTNORM)
			{
			//if we got RTERROR, the only way we can tell the difference
			//	between a right click and an empty sset is that if the user
			//	right clicked, rbp2==NULL;
			if (rbp2==NULL)
				goto exit;
			else
				continue;
			}
		//rc==RTNORM, so figure out what we got back...
		sds_sslength(evtx1,&fl1);
		if (fl1==0L)
			{
			sds_ssfree(evtx1);
			continue;
			}
		else if (fl1>1L)
			{
			for (fl1=1;sds_ssname(evtx1,fl1,ename)==RTNORM;fl1++)
				sds_redraw(ename,IC_REDRAW_UNHILITE);
			}
  		if (sds_ssname(evtx1,0L,ename)!=RTNORM)
			return(-1);
		sds_ssfree(evtx1);
		if (rbp2!=NULL)
			{
			if (rbp2->restype==RTSTR && strsame(rbp2->resval.rstring,"S"/*DNT*/))
				{
				ic_ptcpy(pt1,rbp2->rbnext->resval.rpoint);
				gotpoint=1;
				}
			sds_relrb(rbp2);
			rbp2=NULL;
			}
		if ((elist=sds_entgetx(ename,rbp1))==NULL)
			cmd_ErrorPrompt(CMD_ERR_CANTGETDATA,0);	// no error out here?

		ic_assoc(elist,0);
		elist1 = ic_rbassoc;
		if (sds_wcmatch(ic_rbassoc->resval.rstring,"INSERT,DIMENSION"/*DNT*/)==RTNORM)
			{
			cmd_ErrorPrompt(CMD_ERR_BREAKBLOCK,0);
			continue;
			}
		if (sds_wcmatch(ic_rbassoc->resval.rstring,"LINE,ARC,CIRCLE,ELLIPSE,POLYLINE,RAY,XLINE,SPLINE,LWPOLYLINE"/*DNT*/)!=RTNORM)
			{
			cmd_ErrorPrompt(CMD_ERR_BREAKENT,0);
			continue;
			}
		if (strsame(ic_rbassoc->resval.rstring,"POLYLINE"/*DNT*/))
			{
			for (rbptemp=elist; rbptemp!=NULL; rbptemp=rbptemp->rbnext)
				{
				if (rbptemp->restype==70)
					{
					if (rbptemp->resval.rint & (IC_PLINE_POLYFACEMESH | IC_PLINE_3DMESH))
						{
						rbptemp=NULL;
						cmd_ErrorPrompt(CMD_ERR_BREAKENT,0);
						rc=RTERROR;
						goto exit;
						}
					else
						{
						// Set the Undo so we corectly redraw the entity Undo.
						SDS_SetUndo(IC_UNDO_ENTITY_REDRAW,(void *)ename[0],NULL,NULL,SDS_CURDWG);
						plineflag=rbptemp->resval.rint;
						if (plineflag & IC_PLINE_3DPLINE)	//3d
							{
							rbecs.restype=RTSHORT;
							rbecs.resval.rint=0;
							}
						}
					}
				else if (rbptemp->restype==210)
					ic_ptcpy(rbecs.resval.rpoint,rbptemp->resval.rpoint);
				else if (rbptemp->restype==10)
					plineelev=rbptemp->resval.rpoint[2];//keep elevation of pline
				}
			}
		else if (strsame(ic_rbassoc->resval.rstring,"LWPOLYLINE"/*DNT*/))
			{
			((db_lwpolyline *)ename[0])->get_210(rbecs.resval.rpoint);
			}
		break;
		}
	sds_relrb(rbp1);
	rbp1=NULL;
	strncpy(etype,ic_rbassoc->resval.rstring,sizeof(etype)-1);
	sds_redraw(ename,IC_REDRAW_HILITE);

	if (gotpoint==1)  //if user didn't enter "L" for last entity....
		{
		sds_initget(9,ResourceString(IDC_CMDS4_FIRST_SAME_AS_FI_116, "First Same_as_first_point|@ ~_First ~_@" ));
		if ((rc=(sds_getpoint(NULL,ResourceString(IDC_CMDS4__NFIRST_BREAK_PO_117, "\nFirst break point/<Second break point>: " ),pt2)))==RTCAN)
			{
			sds_redraw(ename,IC_REDRAW_UNHILITE);
			rc=-1;
			goto exit;
			}
		else if (rc==RTKWORD)
			gotpoint=0;
		}
	if (gotpoint==0)	//if user wanted last entity or reset RC to RTKWORD above by entering First
		{
		sds_redraw(ename,IC_REDRAW_HILITE);
		if ((rc=(sds_getpoint(NULL,ResourceString(IDC_CMDS4__NFIRST_BREAK_PO_118, "\nFirst break point: " ),pt1))) != RTNORM)
			{
			rc=-1;
			sds_redraw(ename,IC_REDRAW_UNHILITE);
			goto exit;
			}
		if ((rc=(sds_getpoint(NULL,ResourceString(IDC_CMDS4__NSECOND_BREAK_P_119, "\nSecond break point: " ),pt2))) != RTNORM)
			{
			rc=-1;
			sds_redraw(ename,IC_REDRAW_UNHILITE);
			goto exit;
			}
		sds_redraw(ename,IC_REDRAW_UNHILITE);
		}
	//*********************************************************************************************
	if (strsame(etype,db_hitype2str(DB_LINE)))
		{
		ic_assoc(elist,10);
		ic_ptcpy(ep10,ic_rbassoc->resval.rpoint);
		rbp1=ic_rbassoc;
		ic_assoc(elist,11);
		ic_ptcpy(ep20,ic_rbassoc->resval.rpoint);
		rbp2=ic_rbassoc;
		sds_trans(ep10,&rbwcs,&rbucs,0,ptemp1);
		sds_trans(ep20,&rbwcs,&rbucs,0,ptemp2);
		gr_ucs2rp(ptemp2,ptemp2,view);
		gr_ucs2rp(ptemp1,ptemp1,view);
		if (icadPointEqual(ptemp1,ptemp2))
			{
			//work in wcs if we must
			ic_ptcpy(ptemp1,ep10);
			ic_ptcpy(ptemp2,ep20);
			sds_trans(pt1,&rbucs,&rbwcs,0,pt1);
			sds_trans(pt2,&rbucs,&rbwcs,0,pt2);
			}
		else
			{
			//work in rp if we can
			gr_ucs2rp(pt1,pt1,view);
			gr_ucs2rp(pt2,pt2,view);
			}
		//make sure that the points are on the line
		if (0>ic_ptlndist(pt1,ptemp2,ptemp1,&fr1,pt1) ||
			0>ic_ptlndist(pt2,ptemp2,ptemp1,&fr1,pt2))
			{
			rbp1=rbp2=NULL;
			goto exit;
			}
		//make sure pt1-pt2 direction is same as that for entity
		if (!icadPointEqual(pt1,pt2) && !icadDirectionEqual(ptemp1,ptemp2,pt1,pt2))
			{
			sds_point dummy;
			ic_ptcpy(dummy,pt1);
			ic_ptcpy(pt1,pt2);
			ic_ptcpy(pt2,dummy);
			}
		fr1=sds_distance(ptemp1,ptemp2);
		fr2=sds_distance(ptemp1,pt1);
		fr3=sds_distance(ptemp1,pt2);
		char didmod=0;
		if (!icadPointEqual(ptemp1,pt1) && icadDirectionEqual(ptemp1,ptemp2,ptemp1,pt1))
			{
			rbp2->resval.rpoint[0]=ep10[0]+(ep20[0]-ep10[0])*fr2/fr1;
			rbp2->resval.rpoint[1]=ep10[1]+(ep20[1]-ep10[1])*fr2/fr1;
			rbp2->resval.rpoint[2]=ep10[2]+(ep20[2]-ep10[2])*fr2/fr1;
			if (sds_entmod(elist)!=RTNORM){rbp1=rbp2=NULL;goto exit;}
			didmod=1;
			}
		if (!icadPointEqual(ptemp2,pt2) && icadDirectionEqual(ptemp2,ptemp1,ptemp2,pt2))
			{
			if (1==didmod)ic_ptcpy(rbp2->resval.rpoint,ep20);
			rbp1->resval.rpoint[0]=ep10[0]+(ep20[0]-ep10[0])*fr3/fr1;
			rbp1->resval.rpoint[1]=ep10[1]+(ep20[1]-ep10[1])*fr3/fr1;
			rbp1->resval.rpoint[2]=ep10[2]+(ep20[2]-ep10[2])*fr3/fr1;
			if (1==didmod)
				{
				if (sds_entmake(elist)!=RTNORM)
					{
					rbp1=rbp2=NULL;
					goto exit;
					}
				}
			else
				{
				if (sds_entmod(elist)!=RTNORM)
					{
					rbp1=rbp2=NULL;
					goto exit;
					}
				}
			}
		else if (0==didmod)
			sds_entdel(ename);	//both points were outside the ends of the line...
		rbp1=rbp2=NULL;

		goto exit;
		}//end of line
	//*********************************************************************************************
	else if (strsame(etype,"CIRCLE"/*DNT*/ ))
		{
		//reset entity type to ARC
		if (icadPointEqual(pt1,pt2))
			{
			cmd_ErrorPrompt(CMD_ERR_BREAKDISTINCT,0);
			goto exit;
			}
		ic_assoc(elist,0);
		if ((ic_rbassoc->resval.rstring=ic_realloc_char(ic_rbassoc->resval.rstring,strlen("ARC"/*DNT*/ )+1))==NULL)
			{
			cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
			rc=(-__LINE__);
			goto exit;
			}
		strcpy(ic_rbassoc->resval.rstring,"ARC"/*DNT*/ );
		//reset pt1 and pt2 to be sure that they're on the circle
		ic_assoc(elist,210);
		ic_ptcpy(rbecs.resval.rpoint,ic_rbassoc->resval.rpoint);
		sds_trans(pt1,&rbucs,&rbecs,0,pt1);
		sds_trans(pt2,&rbucs,&rbecs,0,pt2);
		ic_assoc(elist,10);
		cmd_elev_correct(ic_rbassoc->resval.rpoint[2],pt1,3,rbecs.resval.rpoint);
		cmd_elev_correct(ic_rbassoc->resval.rpoint[2],pt2,3,rbecs.resval.rpoint);
		fr1=sds_angle(ic_rbassoc->resval.rpoint,pt1);
		fr2=sds_angle(ic_rbassoc->resval.rpoint,pt2);
		//add 50 & 51 groups so that they're on the entity
		for (rbptemp=ic_rbassoc; rbptemp->rbnext!=NULL; rbptemp=rbptemp->rbnext);
		if ((rbptemp->rbnext=sds_buildlist(50,fr2,51,fr1,0))==NULL)
			{
			cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
			rc=(-__LINE__);
			goto exit;
			}
		if (sds_entmake(elist)==RTNORM)
			entdel_flag=1;
		else
			{
			cmd_ErrorPrompt(CMD_ERR_MODENT,0);
			rc=(-__LINE__);
			}
		}//end of circle

	//*********************************************************************************************
	else if (RTNORM==sds_wcmatch(etype,"RAY,XLINE"/*DNT*/))
		{
		ic_assoc(elist,10);
		ic_ptcpy(ep10,ic_rbassoc->resval.rpoint);
		ic_assoc(elist,11);
		ic_ptcpy(ep20,ic_rbassoc->resval.rpoint);
		//make temp pt
		ptemp1[0]=ep10[0]+ep20[0];
		ptemp1[1]=ep10[1]+ep20[1];
		ptemp1[2]=ep10[2]+ep20[2];
		//make sure that the points are on the line
		gr_ucs2rp(ptemp1,ptemp2,view);
		gr_ucs2rp(ep10,ptemp1,view);
		if (icadPointEqual(ptemp1,ptemp2))
			{
			//work in wcs
			ic_ptcpy(ptemp1,ep10);
			ptemp2[0]=ep10[0]+ep20[0];
			ptemp2[1]=ep10[1]+ep20[1];
			ptemp2[2]=ep10[2]+ep20[2];
			sds_trans(pt1,&rbucs,&rbwcs,0,pt1);
			sds_trans(pt2,&rbucs,&rbwcs,0,pt2);
			}
		else
			{
			//work in rp if we can
			gr_ucs2rp(pt1,pt1,view);
			gr_ucs2rp(pt2,pt2,view);
			}
		if (0>ic_ptlndist(pt1,ptemp2,ptemp1,&fr1,pt1) ||
			0>ic_ptlndist(pt2,ptemp2,ptemp1,&fr1,pt2))
			goto exit;
		//make sure pt1-pt2 direction is same as that for entity
		if (!icadPointEqual(pt1,pt2) && !icadDirectionEqual(ptemp1,ptemp2,pt1,pt2))
			{
			sds_point dummy;
			ic_ptcpy(dummy,pt1);
			ic_ptcpy(pt1,pt2);
			ic_ptcpy(pt2,dummy);
			}
		fr1=sds_distance(ptemp1,ptemp2);
		fr2=sds_distance(ptemp1,pt1);
		fr3=sds_distance(ptemp1,pt2);
		if (!icadDirectionEqual(ptemp1,ptemp2,ptemp1,pt1))
			{
			if (strsame(etype,"RAY"/*DNT*/ ))
			   {
				fr2=0.0;
				ic_ptcpy(pt1,ptemp1);
				}
		   else fr2=-fr2;
			}
		if (!icadDirectionEqual(ptemp1,ptemp2,ptemp1,pt2))
			{
			if (strsame(etype,"RAY"/*DNT*/ ))
			   {
				fr3=0.0;
				ic_ptcpy(pt2,ptemp1);
				}
			else fr3=-fr3;
			}
		//mod the ent
		ic_assoc(elist,10);
		ic_rbassoc->resval.rpoint[0]+=ep20[0]*fr3/fr1;
		ic_rbassoc->resval.rpoint[1]+=ep20[1]*fr3/fr1;
		ic_rbassoc->resval.rpoint[2]+=ep20[2]*fr3/fr1;
		if (strsame(etype,"XLINE"/*DNT*/ ))
			{
			ic_assoc(elist,0);
			if ((ic_rbassoc->resval.rstring=ic_realloc_char(ic_rbassoc->resval.rstring,strlen("RAY"/*DNT*/ )+1))==NULL)
				{
				rc=(-__LINE__);
				goto exit;
				}
			strcpy(ic_rbassoc->resval.rstring,"RAY"/*DNT*/ );
			if ((rc=sds_entmake(elist))!=RTNORM)
				goto exit;
			sds_entdel(ename);
			sds_entlast(ename);
			sds_redraw(ename,IC_REDRAW_DRAW);
			}
		else
			{
			if ((rc=sds_entmod(elist))!=RTNORM)
				goto exit;
			}
		//make the line or ray
	   if (strsame(etype,"RAY"/*DNT*/ ))
		   {
			if (!icadDirectionEqual(ptemp1,ptemp2,ptemp1,pt1))
				goto exit;
			ic_assoc(elist,11);
			ic_ptcpy(ic_rbassoc->resval.rpoint,ep10);
			ic_rbassoc->resval.rpoint[0]+=ep20[0]*fr2/fr1;
			ic_rbassoc->resval.rpoint[1]+=ep20[1]*fr2/fr1;
			ic_rbassoc->resval.rpoint[2]+=ep20[2]*fr2/fr1;
			ic_assoc(elist,10);
			ic_ptcpy(ic_rbassoc->resval.rpoint,ep10);
			ic_assoc(elist,0);
			if ((ic_rbassoc->resval.rstring=ic_realloc_char(ic_rbassoc->resval.rstring,strlen(db_hitype2str(DB_LINE))+1))==NULL)
				{
				rc=(-__LINE__);
				goto exit;
				}
			strcpy(ic_rbassoc->resval.rstring,db_hitype2str(DB_LINE));

			}
	   else
		   {
			ic_assoc(elist,10);
			ic_ptcpy(ic_rbassoc->resval.rpoint,ep10);
			ic_rbassoc->resval.rpoint[0]+=ep20[0]*fr2/fr1;
			ic_rbassoc->resval.rpoint[1]+=ep20[1]*fr2/fr1;
			ic_rbassoc->resval.rpoint[2]+=ep20[2]*fr2/fr1;
			ic_assoc(elist,11);
			ic_rbassoc->resval.rpoint[0]=-ep20[0];
			ic_rbassoc->resval.rpoint[1]=-ep20[1];
			ic_rbassoc->resval.rpoint[2]=-ep20[2];
			//already realloc'd to a ray
			}
		if ((rc=sds_entmake(elist))!=RTNORM)
			goto exit;
		}//end of ray/xline

	//*********************************************************************************************

   else if (strsame(etype,"ELLIPSE"/*DNT*/ ))
	   {
		sds_real minmajratio;
		//	VAR SETUP...  ep1=center of ellipse;ep2 is vector for semimajor axis
		//quicker to work in parameter, since it's native to the ellipse
		//fr1=arc start PARAMETER, fr2= arc end PARAMETER
		//fr3=parameter from arc ctr to pt1, fr4=parameter from arc ctr to pt2
		for (rbptemp=elist; rbptemp!=NULL; rbptemp=rbptemp->rbnext)
			{
			if (rbptemp->restype==10)
				ic_ptcpy(ep1,rbptemp->resval.rpoint);
			else if (rbptemp->restype==11)
				ic_ptcpy(ep2,rbptemp->resval.rpoint);
			else if (rbptemp->restype==40)
				minmajratio=rbptemp->resval.rreal;
			else if (rbptemp->restype==41)
				fr1=rbptemp->resval.rreal;
			else if (rbptemp->restype==42)
				fr2=rbptemp->resval.rreal;
			else if (rbptemp->restype==210)
				ic_ptcpy(rbecs.resval.rpoint,rbptemp->resval.rpoint);
			}
		sds_trans(pt1,&rbucs,&rbecs,0,pt1);
		sds_trans(pt2,&rbucs,&rbecs,0,pt2);
		//move ellipse pts into plane as well
		sds_trans(ep1,&rbwcs,&rbecs,0,ptemp1);
		sds_trans(ep2,&rbwcs,&rbecs,1,ptemp2);
		//move pt1 & pt2 into plane of ellipse along viewdir
		cmd_elev_correct(ptemp1[2],pt1,3,rbecs.resval.rpoint);
		cmd_elev_correct(ptemp1[2],pt2,3,rbecs.resval.rpoint);

		fr3=atan2(ptemp2[1],ptemp2[0]);
		fr5=sds_angle(ptemp1,pt1);
		fr6=sds_angle(ptemp1,pt2);

		ptemp1[1]=sqrt(ep2[0]*ep2[0]+ep2[1]*ep2[1]+ep2[2]*ep2[2]);
		ptemp1[2]=minmajratio*ptemp1[1];
		//get start and end as angles, not parameters.

		fr5-=fr3;
		fr6-=fr3;
		if (icadRealEqual(fr5,0.0) && icadRealEqual(fr6,0.0)) goto exit;
		ic_angpar(fr5,&fr3,ptemp1[1],ptemp1[2],0);
		ic_angpar(fr6,&fr4,ptemp1[1],ptemp1[2],0);

		ic_normang(&fr1,&fr3);
		ic_normang(&fr1,&fr4);
		if (!icadAngleEqual(fr2,IC_TWOPI) || !icadRealEqual(fr1,0.0))
			{
			//open ellipse
			if (!icadAngleEqual(fr2,IC_TWOPI))
				ic_normang(&fr1,&fr2);
			if (fr3>fr2)
				{
				if ((fr3-fr2)<((IC_TWOPI-(fr2-fr1))/2.0))
					fr3=fr2;
				else
					fr3=fr1;
				}
			if (fr4>fr2)
				{
				if ((fr4-fr2)<((IC_TWOPI-(fr2-fr1))/2.0))
					fr4=fr2;
				else
					fr4=fr1;
				}
			fr5=min(fr3,fr4);
			fr6=max(fr3,fr4);
			fr3=fr5;
			fr4=fr6;
			//if both angles match endpoints of ent, delete the whole thing
			if (fr1==fr3 && fr2==fr4)
				{
				entdel_flag=1;
				goto exit;
				}
			}


		//*******Special section for closed ellipse*******
		if (icadRealEqual(fr1,0.0) && icadAngleEqual(fr2,IC_TWOPI))
			{
			ic_assoc(elist,41);
			ic_rbassoc->resval.rreal=fr4;
			ic_assoc(elist,42);
			ic_rbassoc->resval.rreal=fr3;
			if (sds_entmod(elist)!=RTNORM)
				{
				cmd_ErrorPrompt(CMD_ERR_CREATEENT,0);
				rc=(-__LINE__);
				}
			goto exit;
			}
		//*******Open ellipses*******
		if (fr3>fr1)
			{
			ic_assoc(elist,42);
			ic_rbassoc->resval.rreal=fr3;
			if (sds_entmod(elist)!=RTNORM)
				{
				cmd_ErrorPrompt(CMD_ERR_CREATEENT,0);
				rc=(-__LINE__);
				goto exit;
				}
			}
		if (fr4<fr2)
			{
			ic_assoc(elist,41);
			ic_rbassoc->resval.rreal=fr4;
			if (fr3>fr1)
				{
				ic_assoc(elist,42);
				ic_rbassoc->resval.rreal=fr2;
				if (sds_entmake(elist)!=RTNORM)	//we've modified old ellipse,
					{
					cmd_ErrorPrompt(CMD_ERR_CREATEENT,0);	//so create new one
					rc=(-__LINE__);
					}
				}
			else
				{
				if (sds_entmod(elist)!=RTNORM)
					{
					cmd_ErrorPrompt(CMD_ERR_CREATEENT,0);
					rc=(-__LINE__);
					}
				}
			}
		goto exit;
		}//end of ellipse

	//*********************************************************************************************
   else if (strsame(etype,"ARC"/*DNT*/ ))
	   {

		//	VAR SETUP...  ep1=center of arc
		//fr1=arc start angle, fr2= arc end angle
		//fr3=angle from arc ctr to pt1, fr4=angle from arc ctr to pt2
		for (rbptemp=elist; rbptemp!=NULL; rbptemp=rbptemp->rbnext)
			{
			if (rbptemp->restype==10)
				ic_ptcpy(ep1,rbptemp->resval.rpoint);
			else if (rbptemp->restype==50)
				fr1=rbptemp->resval.rreal;
			else if (rbptemp->restype==51)
				fr2=rbptemp->resval.rreal;
			else if (rbptemp->restype==210)
				ic_ptcpy(rbecs.resval.rpoint,rbptemp->resval.rpoint);
			}
		sds_trans(pt1,&rbucs,&rbecs,0,pt1);
		sds_trans(pt2,&rbucs,&rbecs,0,pt2);
		cmd_elev_correct(ep1[2],pt1,3,rbecs.resval.rpoint);
		cmd_elev_correct(ep1[2],pt2,3,rbecs.resval.rpoint);
		fr5=sds_angle(ep1,pt1);
		fr6=sds_angle(ep1,pt2);
		ic_normang(&fr1,&fr2);
		ic_normang(&fr1,&fr5);
		ic_normang(&fr1,&fr6);
		if (fr5>fr2)
			{
			if ((fr5-fr2)<((IC_TWOPI-(fr2-fr1))/2.0))
				fr5=fr2;
			else
				fr5=fr1;
			}
		if (fr6>fr2)
			{
			if ((fr6-fr2)<((IC_TWOPI-(fr2-fr1))/2.0))
				fr6=fr2;
			else
				fr6=fr1;
			}
		fr3=min(fr5,fr6);
		fr4=max(fr5,fr6);

		//if both angles match endpoints of arc, delete the whole thing
		if ((fr1==fr3)&&(fr2==fr4))
			{
			entdel_flag=1;
			goto exit;
			}
		if (fr3>fr1)
			{
			ic_assoc(elist,51);
			ic_rbassoc->resval.rreal=fr3;
			if (sds_entmod(elist)!=RTNORM)
				{
				cmd_ErrorPrompt(CMD_ERR_CREATEENT,0);
				rc=(-__LINE__);
				goto exit;
				}
			}
		if (fr4<fr2)
			{
			ic_assoc(elist,50);
			ic_rbassoc->resval.rreal=fr4;
			if (fr3>fr1)
				{
				ic_assoc(elist,51);
				ic_rbassoc->resval.rreal=fr2;
				if (sds_entmake(elist)!=RTNORM)	//we've modified old arc,
					{
					cmd_ErrorPrompt(CMD_ERR_CREATEENT,0);	//so create new one
					rc=(-__LINE__);
					}
				}
			else
				{
				if (sds_entmod(elist)!=RTNORM)
					{
					cmd_ErrorPrompt(CMD_ERR_CREATEENT,0);
					rc=(-__LINE__);
					}
				}
			}
		goto exit;
		}//end of arc
	//*********************************************************************************************
	else if (strsame(etype,"SPLINE"/*DNT*/ ))
		{
			gr_ucs2rp(pt1,pt1,view);
			gr_ucs2rp(pt2,pt2,view);

			C3Point ptNormal;
			CNURBS* pNurbs = NULL;
			if (GetSplineFrom_resbuf(elist, pNurbs, ptNormal) != SUCCESS) {
				sds_redraw(ename, IC_REDRAW_UNHILITE);
				cmd_ErrorPrompt(CMD_ERR_CREATEENT,0);
				rc=(-__LINE__);
				goto exit;
			}
			double sParam = pNurbs->Start();
			double eParam = pNurbs->End();

			ASSERT(!icadRealEqual(sParam, eParam));
			double param1, param2, dist1, dist2;
			C3Point point1(pt1[0], pt1[1], pt1[2]), point2(pt2[0], pt2[1], pt2[2]), nPt1, nPt2;
			if (pNurbs->PullBack(point1, nPt1, param1, dist1) != SUCCESS || 
				pNurbs->PullBack(point2, nPt2, param2, dist2) != SUCCESS) {
				delete pNurbs;
			}

			SIDE side = BOTH;
			if (icadRealEqual(param2, sParam, 1.0e-3))
				side = RIGHT;
			else if (icadRealEqual(param2, eParam, 1.0e-3))
				side = LEFT;

			PCurve pCurve1 = NULL, pCurve2 = NULL;
			RC rc = pNurbs->Split(param1, side, pCurve1, pCurve2);
			delete pNurbs;
			if (rc != SUCCESS) {
				sds_redraw(ename, IC_REDRAW_UNHILITE);
				cmd_ErrorPrompt(CMD_ERR_CREATEENT,0);
				rc=(-__LINE__);
				goto exit;
			}

			CNURBS* pNurbs1 = static_cast<CNURBS*>(pCurve1);
			CNURBS* pNurbs2 = static_cast<CNURBS*>(pCurve2);
			if (side == BOTH) {
				SIDE side1 = RIGHT;
				if (param1 > param2) {
					CNURBS* pTmp = pNurbs1;
					pNurbs1 = pNurbs2;
					pNurbs2 = pTmp;
					side1 = LEFT;
				}

				rc = pNurbs2->Split(param2, side1, pCurve1, pCurve2);
				delete pNurbs2;

				if (rc != SUCCESS) {
					delete pNurbs1;
					sds_redraw(ename, IC_REDRAW_UNHILITE);
					cmd_ErrorPrompt(CMD_ERR_CREATEENT,0);
					rc=(-__LINE__);
					goto exit;
				}				CNURBS *pLeft = static_cast<CNURBS*>(pCurve1);
				CNURBS *pRight = static_cast<CNURBS*>(pCurve2);

				pNurbs2 = (side1 == LEFT) ? pLeft : pRight;
			}
			else if (side == RIGHT) {
				pNurbs1 = pNurbs2;
				pNurbs2 = NULL;
			}

			struct resbuf *rbSpline = NULL;
			if (pNurbs1) {				
				if (BuildSplineList(pNurbs1, &rbSpline) != SUCCESS ||
					CopyAttributes(elist, rbSpline) != SUCCESS) {
					delete pNurbs1;
					cmd_ErrorPrompt(CMD_ERR_CREATEENT,0);	//so create new one
					rc=(-__LINE__);
					goto exit;
				}
				delete pNurbs1;

				SWAP(elist1->rbnext, rbSpline->rbnext, struct resbuf*);
				if (sds_entmod(elist)!=RTNORM) {
					cmd_ErrorPrompt(CMD_ERR_CREATEENT,0);	//so create new one
					rc=(-__LINE__);
					goto exit;
				}
				if (rbSpline) {
					sds_relrb(rbSpline);
					rbSpline = NULL;
				}
			}

			// create the second curve
			//
			if (pNurbs2 != NULL) {
				if (BuildSplineList(pNurbs2, &rbSpline) != SUCCESS ||
					CopyAttributes(elist, rbSpline) != SUCCESS) {
					delete pNurbs2;
					cmd_ErrorPrompt(CMD_ERR_CREATEENT,0);	//so create new one
					rc=(-__LINE__);
					goto exit;
				}
				delete pNurbs2;

				if (sds_entmake(rbSpline)!=RTNORM) {
					cmd_ErrorPrompt(CMD_ERR_CREATEENT,0);	//so create new one
					rc=(-__LINE__);
					goto exit;
				}
				if (rbSpline) 
					sds_relrb(rbSpline);
			}
			sds_relrb(elist);
			elist = NULL;
		}
	//*********************************************************************************************
	else if (strsame(etype,"POLYLINE"/*DNT*/ ))
		{
		//fi1 = 1-based vertex # for pt1, fi2 = 1-based vertex # for pt2
		//find out between which vertices the points lie
		//NOTE:  if a splined pline, cmd_pl_vtx_no will skip control vertices
		fl1=cmd_pl_vtx_no(ename,pt1,1,&exactvert);
		fl2=cmd_pl_vtx_no(ename,pt2,1,&exactvert);
		trim_ends=0;
		//if fi2 < fi1, switch the points so that pt1 is closer to start of open pline
		if (fl2<fl1)
			{
			ic_ptcpy(ptemp1,pt1);
			ic_ptcpy(pt1,pt2);
			ic_ptcpy(pt2,ptemp1);
			fl3=fl1;
			fl1=fl2;
			fl2=fl3;
			if (plineflag & IC_PLINE_CLOSED)
				trim_ends=1;
			}
		//step to the correct entities
		ic_encpy(evtx1,ename);
		ic_encpy(evtx2,ename);
		for (fl3=0L;fl3<fl1;sds_entnext_noxref(evtx1,evtx1),fl3++);
		for (fl3=0L;fl3<fl2;sds_entnext_noxref(evtx2,evtx2),fl3++);
		//move points to closest spot on pline to get final breaking points...
		sds_trans(pt1,&rbucs,&rbecs,0,pt1);
		sds_trans(pt2,&rbucs,&rbecs,0,pt2);
		if (!(plineflag & IC_PLINE_3DPLINE))
			{
			cmd_elev_correct(plineelev,pt1,3,rbecs.resval.rpoint);
			cmd_elev_correct(plineelev,pt2,3,rbecs.resval.rpoint);
			}
		cmd_pt_2_plseg(evtx1,pt1);
		cmd_pt_2_plseg(evtx2,pt2);
		//if fl1==fl2, both pts are on the same segment, and we ALWAYS break just
		//that segment w/o regard to direction.  If we want to change this, do it here

		//Now, both pt1 and pt2 are on the pline.
		//If both are within the same segment, make sure pt1 is closer to start than pt2
		//(we don't have to switch evtx entities or indexes)
		if (fl1==fl2)
			{
			rbp1=sds_entget(evtx1);
			ic_assoc(rbp1,10);
			ic_ptcpy(ptemp1,ic_rbassoc->resval.rpoint);
			ic_assoc(rbp1,42);
			fr1=ic_rbassoc->resval.rreal;
			if (0.0==fr1 && sds_distance(pt2,ptemp1)<sds_distance(pt1,ptemp1))	//straight segment
				{
				ic_ptcpy(ptemp1,pt1);
				ic_ptcpy(pt1,pt2);
				ic_ptcpy(pt2,ptemp1);
				}
			if (0.0!=fr1)															//curved segment
				{
				sds_entnext_noxref(evtx1,etemp);
				rbp2=sds_entget(etemp);
				ic_assoc(rbp2,0);
				//if entnext vtx1 isn't another vertex, get the ent start point
			   if (strsame(ic_rbassoc->resval.rstring,"SEQEND"/*DNT*/ ))
				   {
					ic_assoc(rbp2,-2);
					ic_encpy(etemp,ic_rbassoc->resval.rlname);
					sds_relrb(rbp2);
					rbp2=NULL;
					sds_entnext_noxref(etemp,etemp);
					rbp2=sds_entget(etemp);
					}
				ic_assoc(rbp2,10);
				ic_ptcpy(ptemp2,ic_rbassoc->resval.rpoint);
				if (fr1>0)		//counter-clockwise bulged segment
					{
					ic_bulge2arc(ptemp1,ptemp2,fr1,ep10,&fr2,&fr3,&fr4);
					if (cmd_ang_betw(sds_angle(ep10,pt2),fr3,sds_angle(ep10,pt1)))
						{
						ic_ptcpy(ptemp1,pt1);
						ic_ptcpy(pt1,pt2);
						ic_ptcpy(pt2,ptemp1);
						}
					}
				else			//clockwise bulged segment - we'll get counter-clockwise arc info
					{
					ic_bulge2arc(ptemp1,ptemp2,fr1,ep10,&fr2,&fr3,&fr4);
					if (cmd_ang_betw(sds_angle(ep10,pt1),fr3,sds_angle(ep10,pt2)))
						{
 						ic_ptcpy(ptemp1,pt1);
						ic_ptcpy(pt1,pt2);
						ic_ptcpy(pt2,ptemp1);
						}
					}
				sds_relrb(rbp2);
				rbp2=NULL;
				}
			sds_relrb(rbp1);
			rbp1=NULL;
			}
		//we now know that p1 is closer to start of pline than p2, even if they're in the same segment!
		cmd_get_startend(ename,ep10,ep20);
		if (plineflag & IC_PLINE_CLOSED)
			{
			ic_ptcpy(ep20,ep10);
			}
		else
			{
			//if both points are on start of pline or end of pline, just exit...
			if ((icadPointEqual(pt1,ep20))||(icadPointEqual(pt2,ep10)))
				{
				rc=0;
				goto exit;
				}
			//if pt1 is on start and pt2 is on end, delete entity...
			if ((icadPointEqual(pt1,ep10))&&(icadPointEqual(pt2,ep20)))
				{
				entdel_flag=1;
				goto exit;
				}
			}
		rc=cmd_pline_breakup(ename,ep10,ep20,plineflag,evtx1,pt1,false,evtx2,pt2,trim_ends,redraw_ent);
		/*
		// There are 2 cases left to do (not mutually exclusive events):
		// CASE 1:	break pt 1 is not at begin of pline, so we need to make a new pline begin piece
		// CASE 2:	break pt 2 is not at end of polyline, so we need to make a new pline end piece

		//***********CASE 1***************
		if (!icadPointEqual(pt1,ep10)){
			//in this case we just have to entmod the first break vertex & then make a new seqend
			if (cmd_make_pline_start(ename,evtx1)==-1) goto exit;
			if (rbp1!=NULL){sds_relrb(rbp1); rbp1=NULL;}
			rbp1=sds_entget(evtx1);
			ic_assoc(rbp1,42);
			fr1=ic_rbassoc->resval.rreal;
			ic_assoc(rbp1,10);
			ic_ptcpy(ptemp1,ic_rbassoc->resval.rpoint);
			////set pt1 to the vertex point
			//ic_ptcpy(ic_rbassoc->resval.rpoint,pt1);
			//get point of next vertex
			sds_entnext_noxref(evtx1,etemp);
			tryagain1:	//we'll jump back here if next vtx a spline ctrl pt
			if (rbp2!=NULL){sds_relrb(rbp2); rbp2=NULL;}
			rbp2=sds_entget(etemp);
			ic_assoc(rbp2,0);
			//if entnext evtx2 isn't another vertex, get the ent start point
		   if (strsame(ic_rbassoc->resval.rstring,"SEQEND" )) {
				ic_assoc(rbp2,-2);
				ic_encpy(etemp,ic_rbassoc->resval.rlname);
				sds_relrb(rbp2);
				rbp2=NULL;
				sds_entnext_noxref(etemp,etemp);
				rbp2=sds_entget(etemp);
			}
			if (plineflag & 4){  //make sure next vtx isn't a control pt
				ic_assoc(rbp2,70);
				if (ic_rbassoc->resval.rint & 16){
					sds_entnext_noxref(etemp,etemp);
					goto tryagain1;
				}
			}
			ic_assoc(rbp2,10);
			ic_ptcpy(ptemp2,ic_rbassoc->resval.rpoint);
			//setup data for calculating widths
			if (ic_assoc(rbp1,41)!=0){//if there's no ending width present, make one
				rbptemp=rbp1;
				if (rbptemp->rbnext!=NULL)for (; rbptemp->rbnext!=NULL; rbptemp=rbptemp->rbnext);
				rbptemp->rbnext=sds_buildlist(41,0.0,0);
				fr6=0.0;
			}else
				fr6=ic_rbassoc->resval.rreal;
			if (ic_assoc(rbp1,40)!=0)
				fr5=0.0;
			else
				fr5=ic_rbassoc->resval.rreal;
			if (fr1!=0){		//if segment is curved, calculate new bulge & ending width
				if ((fr1==IC_TWOPI)||(fr1==-IC_TWOPI))fr1=fr1*0.99999999;
				//get a counter-clockwise arc to define the center point
				ic_bulge2arc(ptemp1,ptemp2,fr1,ep1,&fr2,&fr3,&fr4);
				if (fr3>sds_angle(ep1,pt1))fr3=fr3-IC_TWOPI;
				if (fr4<sds_angle(ep1,pt1))fr4=fr4+IC_TWOPI;
				if (fr1>0){  //counter-clockwise pline
					//update bulge
					fr2=tan((sds_angle(ep1,pt1)-fr3)/4.0);
					ic_assoc(rbp1,42);
					ic_rbassoc->resval.rreal=fr2;
					//update width.  fr5=start width, fr6=end width
					ic_assoc(rbp1,41);
					ic_rbassoc->resval.rreal=fr5+((fr6-fr5)*(sds_angle(ep1,pt1)-fr3)/(fr4-fr3));
				}else{	//clockwise pline
					//update bulge
					fr2=tan((sds_angle(ep1,pt1)-fr4)/4.0);
					ic_assoc(rbp1,42);
					ic_rbassoc->resval.rreal=fr2;
					//update width
					ic_assoc(rbp1,41);
					ic_rbassoc->resval.rreal=fr5+((fr6-fr5)*(sds_angle(ep1,pt1)-fr4)/(fr3-fr4));
				}
			}else{	//straight segment - just update starting width
				ic_assoc(rbp1,41);
				ic_rbassoc->resval.rreal=fr5+((fr6-fr5)*sds_distance(pt1,ptemp1)/sds_distance(ptemp1,ptemp2));
			}
			if (sds_entmake(rbp1)!=RTNORM){
				rc=(-__LINE__);
				goto exit;
			}

			//RBPTEMP not freed in exit routine
		   rbptemp=sds_buildlist(RTDXF0,"VERTEX"DNT,10,pt1,0);
			if (sds_entmake(rbptemp)!=RTNORM){
				rc=(-__LINE__);
				goto exit;
			}
			sds_relrb(rbptemp);
		   rbptemp=sds_buildlist(RTDXF0,"SEQEND"DNT,0);
			if (sds_entmake(rbptemp)==RTNORM){
				sds_entlast(redraw_ent);
				entdel_flag=1;
			}else
				rc=(-__LINE__);
			sds_relrb(rbptemp);
		}
		//***********CASE 2***************
		if (!icadPointEqual(pt2,ep20)){
			//Make header with 70 flag=0;
			sds_entnext_noxref(ename,etemp);
			if (cmd_make_pline_start(ename,etemp)!=0){
				rc=(-__LINE__);
				goto exit;
			}
			//modify vertex info for pt2 and make breakpoint vertex
			if (rbp1!=NULL){sds_relrb(rbp1); rbp1=NULL;}
			rbp1=sds_entget(evtx2);
			ic_assoc(rbp1,42);
			fr1=ic_rbassoc->resval.rreal;
			ic_assoc(rbp1,10);
			ic_ptcpy(ptemp1,ic_rbassoc->resval.rpoint);
			//copy pt2 to beginning of segment
			ic_ptcpy(ic_rbassoc->resval.rpoint,pt2);
			sds_entnext_noxref(evtx2,etemp);
			tryagain2:	 //we'll jump back here if next vtx a spline ctrl pt
			if (rbp2!=NULL){sds_relrb(rbp2); rbp2=NULL;}
			rbp2=sds_entget(etemp);
			ic_assoc(rbp2,0);
			//if entnext evtx2 isn't another vertex, get the ent start point
		   if (strsame(ic_rbassoc->resval.rstring,"SEQEND" )){
				ic_assoc(rbp2,-2);
				ic_encpy(etemp,ic_rbassoc->resval.rlname);
				sds_relrb(rbp2);
				rbp2=NULL;
				sds_entnext_noxref(etemp,etemp);
				rbp2=sds_entget(etemp);
			}
			if (plineflag & 4){  //make sure next vtx isn't a control pt
				ic_assoc(rbp2,70);
				if (ic_rbassoc->resval.rint & 16){
					sds_entnext_noxref(etemp,etemp);
					goto tryagain2;
				}
			}
			ic_assoc(rbp2,10);
			ic_ptcpy(ptemp2,ic_rbassoc->resval.rpoint);
			//setup vars to calculate width.  fr5=start width, fr6=end width
			if (ic_assoc(rbp1,40)!=0){//if there's no starting width present, make one
				rbptemp=rbp1;
				if (rbptemp->rbnext!=NULL)for (; rbptemp->rbnext!=NULL; rbptemp=rbptemp->rbnext);
				rbptemp->rbnext=sds_buildlist(40,0.0,0);
				fr5=0.0;
			}else
				fr5=ic_rbassoc->resval.rreal;
			if (ic_assoc(rbp1,41)!=0)
				fr6=0.0;
			else
				fr6=ic_rbassoc->resval.rreal;
			if (fr1!=0){		//if segment is curved, calculate new bulge & starting width
				if ((fr1==IC_TWOPI)||(fr1==-IC_TWOPI))fr1=fr1*0.99999999;
				//get a counter-clockwise arc to define the center point
				ic_bulge2arc(ptemp1,ptemp2,fr1,ep1,&fr2,&fr3,&fr4);
				if (fr3>sds_angle(ep1,pt2))fr3=fr3-IC_TWOPI;
				if (fr4<sds_angle(ep1,pt2))fr4=fr4+IC_TWOPI;
				if (fr1>0){  //counter-clockwise pline
					//update bulge
					fr2=tan((fr4-sds_angle(ep1,pt2))/4.0);
					ic_assoc(rbp1,42);
					ic_rbassoc->resval.rreal=fr2;
					//update width.  fr5=start width, fr6=end width
					ic_assoc(rbp1,40);
					ic_rbassoc->resval.rreal=fr5+((fr6-fr5)*(fr4-sds_angle(ep1,pt2))/(fr4-fr3));
				}else{	//clockwise pline
					//update bulge
					fr2=tan((fr3-sds_angle(ep1,pt2))/4.0);
					ic_assoc(rbp1,42);
					ic_rbassoc->resval.rreal=fr2;
					//update width
					ic_assoc(rbp1,40);
					ic_rbassoc->resval.rreal=fr5+((fr6-fr5)*(sds_angle(ep1,pt2)-fr3)/(fr4-fr3));
				}
			}else{	//straight segment - just update starting width
				ic_assoc(rbp1,40);
				ic_rbassoc->resval.rreal=fr5+((fr6-fr5)*sds_distance(pt2,ptemp2)/sds_distance(ptemp1,ptemp2));
			}
			if (sds_entmake(rbp1)!=RTNORM){
				cmd_ErrorPrompt(CMD_ERR_CREATEENT,0);
			 	rc=(-__LINE__);
			}
			//make the rest of the entity
			//if pline closed, make sure we make a new vertex at old starting point
			if ((!ic_assoc(elist,70))&&(ic_rbassoc->resval.rint&1)){
				sds_entnext_noxref(ename,etemp);
				rbptemp=sds_entget(etemp);
				ic_assoc(rbptemp,10);
				fl3=cmd_make_pline_end(evtx2,1,ic_rbassoc->resval.rpoint);
				sds_relrb(rbptemp); rbptemp=NULL;
			}else{
				fl3=cmd_make_pline_end(evtx2,0,pt1); //pt1 is dummy - closure flag=0
			}
			if (fl3==0L){
				entdel_flag=1;
			}else
			 	rc=(-__LINE__);
			sds_relrb(rbp1);
			sds_relrb(rbp2);
			rbp1=rbp2=NULL;
		}*/
		}
		else if (strsame(etype,"LWPOLYLINE"/*DNT*/ ))
		{
		db_lwpolyline *lwpline=(db_lwpolyline *)ename[0];
		int nverts;
		lwpline->get_90(&nverts);
		plineflag=lwpline->ret_70();
		//fi1 = 1-based vertex # for pt1, fi2 = 1-based vertex # for pt2
		//find out between which vertices the points lie
		//NOTE:  if a splined pline, cmd_pl_vtx_no will skip control vertices
		pt1vertno=cmd_lwpl_vtx_no(ename,pt1,1,&exactvert);
		pt2vertno=cmd_lwpl_vtx_no(ename,pt2,1,&exactvert);
		trim_ends=0;
		//if fi2 < fi1, switch the points so that pt1 is closer to start of open pline
		if (pt2vertno<pt1vertno)
			{
			ic_ptcpy(ptemp1,pt1);
			ic_ptcpy(pt1,pt2);
			ic_ptcpy(pt2,ptemp1);
			fl3=pt1vertno;
			pt1vertno=pt2vertno;
			pt2vertno=fl3;
			if (plineflag & IC_PLINE_CLOSED)
				trim_ends=1;
			}

		//move points to closest spot on pline to get final breaking points...
		sds_trans(pt1,&rbucs,&rbecs,0,pt1);
		sds_trans(pt2,&rbucs,&rbecs,0,pt2);

// lwpline is always not-3D
		cmd_elev_correct(plineelev,pt1,3,rbecs.resval.rpoint);
		cmd_elev_correct(plineelev,pt2,3,rbecs.resval.rpoint);

		cmd_pt_2_lwplseg(lwpline,pt1vertno,pt1);
		cmd_pt_2_lwplseg(lwpline,pt2vertno,pt2);
		//if pt1vertno==pt2vertno, both pts are on the same segment, and we ALWAYS break just
		//that segment w/o regard to direction.  If we want to change this, do it here

		//Now, both pt1 and pt2 are on the pline.
		//If both are within the same segment, make sure pt1 is closer to start than pt2
		//(we don't have to switch evtx entities or indexes)
		if (pt1vertno==pt2vertno)
			{
			// Modified CyberAge PP 09/06/2000 [
			// Fix for the bug: polylines not breaking properly.
			lwpline->get_10(ptemp1,pt1vertno);
			lwpline->get_42(&fr1,pt1vertno);
			// Modified CyberAge PP 09/06/2000 ]
			if (0.0==fr1 && sds_distance(pt2,ptemp1)<sds_distance(pt1,ptemp1))	//straight segment
				{
				ic_ptcpy(ptemp1,pt1);
				ic_ptcpy(pt1,pt2);
				ic_ptcpy(pt2,ptemp1);
				}
			if (0.0!=fr1)															//curved segment
				{
				if (pt1vertno+1==nverts)
					lwpline->get_10(ptemp2,0);
				else
					lwpline->get_10(ptemp2,pt1vertno+1);
				if (fr1>0)		//counter-clockwise bulged segment
					{
					ic_bulge2arc(ptemp1,ptemp2,fr1,ep10,&fr2,&fr3,&fr4);
					if (cmd_ang_betw(sds_angle(ep10,pt2),fr3,sds_angle(ep10,pt1)))
						{
						ic_ptcpy(ptemp1,pt1);
						ic_ptcpy(pt1,pt2);
						ic_ptcpy(pt2,ptemp1);
						}
					}
				else			//clockwise bulged segment - we'll get counter-clockwise arc info
					{
					ic_bulge2arc(ptemp1,ptemp2,fr1,ep10,&fr2,&fr3,&fr4);
					if (cmd_ang_betw(sds_angle(ep10,pt1),fr3,sds_angle(ep10,pt2)))
						{
 						ic_ptcpy(ptemp1,pt1);
						ic_ptcpy(pt1,pt2);
						ic_ptcpy(pt2,ptemp1);
						}
					}
				}
			}
		//we now know that p1 is closer to start of pline than p2, even if they're in the same segment!
		lwpline->get_10(ep10,0);
		lwpline->get_10(ep20,nverts-1);
		if (plineflag & IC_PLINE_CLOSED)
			{
			ic_ptcpy(ep20,ep10);
			}
		else
			{
			//if both points are on start of pline or end of pline, just exit...
			if (icadPointEqual(pt1,ep20) || icadPointEqual(pt2,ep10))
				{
				rc=0;
				goto exit;
				}
			//if pt1 is on start and pt2 is on end, delete entity...
			if (icadPointEqual(pt1,ep10) && icadPointEqual(pt2,ep20))
				{
				entdel_flag=1;
				goto exit;
				}
			}
		rc=cmd_lwpline_breakup(ename,ep10,ep20,plineflag,pt1vertno,pt1,false,pt2vertno,pt2,trim_ends,redraw_ent);

		}

	exit:
	if (elist!=NULL)
		sds_relrb(elist);
	if (rbp1!=NULL)
		sds_relrb(rbp1);
	if (rbp2!=NULL)
		sds_relrb(rbp2);
	elist=rbp1=rbp2=rbptemp=NULL;
	if (entdel_flag==1)
		{
		sds_entdel(ename);
		sds_entlast(ename);
		sds_redraw(ename,IC_REDRAW_DRAW);
		if (redraw_ent[1] || redraw_ent[0])
			sds_redraw(redraw_ent,IC_REDRAW_DRAW);
		}
	return(rc);
	}

short cmd_elev(void) {

	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	sds_real resp1, resp2;
	RT rt1, rt2;
	struct resbuf rb1, rb2;
	char oldvar[IC_ACADBUF], pmt[IC_ACADBUF];

	//	retrieve exist elev & thickness
	SDS_getvar(NULL,DB_QELEVATION,&rb1,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	SDS_getvar(NULL,DB_QTHICKNESS,&rb2,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

	//	format var, then prompt for new elev and thickness
	sds_rtos((rb1.resval.rreal),-1,-1,oldvar);
	sprintf(pmt,ResourceString(IDC_CMDS4__NNEW_CURRENT_EL_122, "\nNew current elevation <%s>: " ),oldvar);
	if ((rt1=(sds_getdist(NULL,pmt,&resp1)))==RTCAN) return (-2);
	sds_rtos((rb2.resval.rreal),-1,-1,oldvar);
  	sprintf(pmt,ResourceString(IDC_CMDS4__NNEW_CURRENT_TH_123, "\nNew current thickness <%s>: " ),oldvar);
	if ((rt2=(sds_getdist(NULL,pmt,&resp2)))==RTCAN) return (-2);

	//	reset changed vars
	if (rt1 == RTNORM) {
		rb1.resval.rreal=resp1;
		sds_setvar("ELEVATION"/*DNT*/,&rb1);
	}
	if (rt2 == RTNORM) {
		rb2.resval.rreal=resp2;
		sds_setvar("THICKNESS"/*DNT*/,&rb2);
	}
	return(RTNORM);
}

short cmd_limits(void) {

	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	RT rt1, rt2, limitson;			// limitson is 1 if limits are on (LIMCHECK==0), 0 if not.
	struct resbuf rb1, rb2, rb3;
	sds_point newptmin, newptmax;
	char oldvar[2][IC_ACADBUF], strvar[IC_ACADBUF],do_redraw;

	do_redraw=0;

	//	retrieve exist limits

	int tilemode,cvport;
	if (SDS_getvar(NULL,DB_QTILEMODE,&rb1,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)return(RTERROR);
	tilemode=rb1.resval.rint;
	if (SDS_getvar(NULL,DB_QCVPORT,&rb1,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)return(RTERROR);
	cvport=rb1.resval.rint;

	SDS_getvar(NULL,((tilemode==0 && cvport==1) ? DB_QP_LIMMIN : DB_QLIMMIN),&rb1,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	SDS_getvar(NULL,((tilemode==0 && cvport==1) ? DB_QP_LIMMAX : DB_QLIMMAX),&rb2,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

	do {
		//	get vars for lower left corner
		sds_rtos((rb1.resval.rpoint[0]),-1,-1,oldvar[0]);
		sds_rtos((rb1.resval.rpoint[1]),-1,-1,oldvar[1]);

		if (SDS_getvar(NULL,DB_QLIMCHECK,&rb3,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)return(RTERROR);
		limitson=rb3.resval.rint;
		if (limitson) {
		   sds_initget(8, ResourceString(IDC_LIMITS_INIT1, "~ON Turn_limits_off|OFf ~Toggle ~_ON ~_OFF ~_TOGGLE")); 							 // Allow pick outside current limits
			sprintf(strvar,ResourceString(IDC_CMDS4__NLIMITS_ARE_ON__127, "\nLimits are on:  OFF/<Lower left corner> <%s,%s>: " ),oldvar[0],oldvar[1]);
		} else {
		   sds_initget(8, ResourceString(IDC_LIMITS_INIT2, "Turn_limits_on|ON ~OFf ~Toggle ~_ON ~_OFF ~_TOGGLE"));								 // Allow pick outside current limits
			sprintf(strvar,ResourceString(IDC_CMDS4__NLIMITS_ARE_OFF_129, "\nLimits are off:  ON/<Lower left corner> <%s,%s>: " ),oldvar[0],oldvar[1]);
		}
		if ((rt1=(sds_getpoint(NULL,strvar,newptmin))) == RTCAN) return(-2);

		//	if the user entered ON or OFF...
		if (rt1 == RTKWORD){
			sds_getinput(strvar);
			SDS_getvar(NULL,DB_QLIMCHECK,&rb1,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			if (0 == stricmp(strvar,ResourceString(IDC_CMDS4_TOGGLE_130, "Toggle" )) || 0 == stricmp(strvar,"Toggle" )) {		 // The toggle option is for the menu/toolbar mostly.  Just On/Off.
				if (rb1.resval.rint==0)
					rb1.resval.rint=1;
				else
					rb1.resval.rint=0;
		   } else if (0 == stricmp(strvar,ResourceString(IDC_CMDS4_ON_131, "ON" )) || 0 == stricmp(strvar,"ON" )) {
				rb1.resval.rint=1;
			} else{
				rb1.resval.rint=0;
			}
			sds_setvar("LIMCHECK"/*DNT*/,&rb1);
			return(0);
		}

		//	if the user hit enter
		if (rt1 == RTNONE) {
			newptmin[0] = rb1.resval.rpoint[0];
			newptmin[1] = rb1.resval.rpoint[1];
		} else if (RTNORM==rt1) {
			do_redraw=1;
		} else {
			do_redraw=0;
		}

		//	get vars for upper right corner
		sds_rtos((rb2.resval.rpoint[0]),-1,-1,oldvar[0]);
		sds_rtos((rb2.resval.rpoint[1]),-1,-1,oldvar[1]);
		sds_initget(8,NULL);
		sprintf(strvar,ResourceString(IDC_CMDS4__NUPPER_RIGHT_CO_133, "\nUpper right corner <%s,%s>: " ),oldvar[0],oldvar[1]);
		SDS_getvar(NULL,DB_QDRAGMODE,&rb3,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		if ((rt2=SDS_dragobject(21,rb3.resval.rint,newptmin,NULL,0.0,strvar,NULL,newptmax,NULL)) == RTCAN) return(-2);
		//if ((rt2=(sds_getcorner(newptmin,strvar,newptmax))) == RTCAN) return(-2);

		//	if user hit enter
		if (rt2 == RTNONE) {
			newptmax[0] = rb2.resval.rpoint[0];
			newptmax[1] = rb2.resval.rpoint[1];
		}else if (RTNORM==rt2) do_redraw=1;
		else do_redraw=0;

		//	check for valid points selection
		if ((newptmin[0] >= newptmax[0]) | (newptmin[1] >= newptmax[1])) {
			sds_printf(ResourceString(IDC_CMDS4__NINVALID_LIMITS_134, "\nInvalid limits." ));
			do_redraw=0;
		}

	} while ((newptmin[0] >= newptmax[0]) | (newptmin[1] >= newptmax[1]));


	if (do_redraw){
		//	if valid pts selected, reset limits vars
		rb1.resval.rpoint[0]=newptmin[0];
		rb1.resval.rpoint[1]=newptmin[1];
		rb2.resval.rpoint[0]=newptmax[0];
		rb2.resval.rpoint[1]=newptmax[1];
		if ((rt1=(sds_setvar((tilemode==0 && cvport==1) ?"P_LIMMIN"/*DNT*/	:"LIMMIN"/*DNT*/ ,&rb1)))!=RTNORM) return(-2);
		if ((rt1=(sds_setvar((tilemode==0 && cvport==1) ?"P_LIMMAX"/*DNT*/	:"LIMMAX"/*DNT*/ ,&rb2)))!=RTNORM) return(-2);
		sds_redraw(NULL,IC_REDRAW_DONTCARE);
	}

	return(RTNORM);
}


short cmd_graphscr(void) {
	SDS_MultibleMode=0;
	return(RT(sds_graphscr()));
}
short cmd_textscr(void) {
	SDS_MultibleMode=0;
	return(RT(sds_textscr()));
}

short cmd_dist(void) {

	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	sds_point p1, p2;
	RT ret;
	struct resbuf varbuf,rbucs,rbwcs;		// For "WNDLCMD" -- 0 if command line is hidden, 1 if it is showing.
	char strvar[IC_ACADBUF];
	sds_real dist,fr1;

	rbucs.restype=rbwcs.restype=RTSHORT;
	rbucs.resval.rint=1;
	rbwcs.resval.rint=0;

	// get pts from user
	do{
		sds_initget(128,NULL);
		if ((ret=sds_getpoint(NULL,ResourceString(IDC_CMDS4__NSTARTING_POINT_139, "\nStarting point for distance: " ),p1))==RTERROR) {
			return(ret);
		}else if (ret==RTCAN){
			return(ret);
		}else if (ret==RTKWORD){
			sds_getinput(strvar);
			if (sds_distof(strvar,-1,&dist)!=RTNORM){
				cmd_ErrorPrompt(CMD_ERR_DIST2PTS,0);
				continue;
			}
			if (dist<1.0E-16) dist=0.0;
			sds_rtos(dist,-1,-1,strvar);
			sds_printf(ResourceString(IDC_CMDS4__NDISTANCE____S_140, "\nDistance = %s" ),strvar);

			varbuf.restype=RTREAL;
			varbuf.resval.rreal=dist;
			sds_setvar("DISTANCE"/*DNT*/,&varbuf);

			return(RTNORM);
		}else if (ret==RTNORM){
			break;
		}
	}while (1);
	sds_initget(1,NULL);
	if ((ret=sds_getpoint(p1,ResourceString(IDC_CMDS4__NEND_POINT___142, "\nEnd point: " ),p2))!=RTNORM) return(ret);
	// print 1st line w/ dist, XY angle, and Z angle

	dist=sds_distance(p1,p2);

	if (dist<1.0E-16)dist=0.0;

	varbuf.restype=RTREAL;
	varbuf.resval.rreal=dist;
	sds_setvar("DISTANCE"/*DNT*/,&varbuf);

	sds_rtos(dist,-1,-1,strvar);
	sds_printf(ResourceString(IDC_CMDS4__NDISTANCE____S_140, "\nDistance = %s" ),strvar);

	if (0.0==dist){
		sds_printf(ResourceString(IDC_CMDS4__TIDENTICAL_POIN_143, "\tIdentical points selected." ));
		return(RTNORM);
	}

	fr1=sds_angle(p1,p2);
	sds_angtos(fr1,-1,-1,strvar);
	sds_printf(ResourceString(IDC_CMDS4____ANGLE_IN_XY_P_144, ",  Angle in XY Plane = %s" ),strvar);
	//	Z angle relative to p1-p2 dist projected into XY plane (Z's=0)
	dist=sds_dist2d(p1,p2);
	if (dist>CMD_FUZZ)
		fr1=atan2((p2[2]-p1[2]),dist);
	else
		fr1=0.0;
	sds_angtos(fr1,-1,-1,strvar);
	sds_printf(ResourceString(IDC_CMDS4____ANGLE_FROM_XY_145, ",  Angle from XY Plane = %s" ),strvar);

	if ((ret=SDS_getvar(NULL,DB_QWNDLCMD,&varbuf,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM)return(ret);
	if (varbuf.resval.rint==0)										// If command line's off
		sds_textscr();												//	  bring up the text screen.

	//	print new line w/ deltaX, deltaY, & deltaZ
	sds_rtos((p2[0]-p1[0]),-1,-1,strvar);
	sds_printf(ResourceString(IDC_CMDS4__NDELTA_X____S_146, "\nDelta X = %s" ),strvar);
	sds_rtos((p2[1]-p1[1]),-1,-1,strvar);
	sds_printf(ResourceString(IDC_CMDS4____DELTA_Y____S_147, ",  Delta Y = %s" ),strvar);
	sds_rtos((p2[2]-p1[2]),-1,-1,strvar);
	sds_printf(ResourceString(IDC_CMDS4____DELTA_Z____S_148, ",  Delta Z = %s" ),strvar);

	return(RTNORM);
}

short cmd_id(void) {

	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	RT rt1;
	struct resbuf rb1;
	struct resbuf varbuf;			// For checking whether the command bar is showing or not.
	char pt_string[3][IC_ACADBUF];

	// get point from user & convert to units
	if ((rt1=(sds_getpoint(NULL,ResourceString(IDC_CMDS4__NSELECT_POINT_T_149, "\nSelect point to identify coordinates: " ),rb1.resval.rpoint))) != RTNORM) return(rt1);
	if (icadRealEqual(rb1.resval.rpoint[0],0.0))rb1.resval.rpoint[0]=0.0;
	if (icadRealEqual(rb1.resval.rpoint[1],0.0))rb1.resval.rpoint[1]=0.0;
	if (icadRealEqual(rb1.resval.rpoint[2],0.0))rb1.resval.rpoint[2]=0.0;
	sds_rtos(rb1.resval.rpoint[0],-1,-1,pt_string[0]);
	sds_rtos(rb1.resval.rpoint[1],-1,-1,pt_string[1]);
	sds_rtos(rb1.resval.rpoint[2],-1,-1,pt_string[2]);

	if ((rt1=SDS_getvar(NULL,DB_QWNDLCMD,&varbuf,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM)return(rt1);
	if (varbuf.resval.rint==0)										// If command line's off
		sds_textscr();												//	  bring up the text screen.

	// print pt for user
	sds_printf(ResourceString(IDC_CMDS4__N_X__S__Y__S__Z_150, "\n X=%s  Y=%s  Z=%s" ),pt_string[0],pt_string[1],pt_string[2]);

	SDS_setvar(NULL,DB_QLASTPOINT,&rb1,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);

	return(RTNORM);
}

extern char *SDS_SetVarByPass;

//4M Spiros Item:31->
extern CString MdMacro;
//4M Spiros Item:31<-

short cmd_setvar(void) {
	struct resbuf rb,varbuf,tempbuf,rbucs,rbwcs,*tmp=NULL;
	static char lastvar[IC_ACADBUF] = ""/*DNT*/;
	char varname[IC_ACADBUF], str2[IC_ACADBUF], pmt[IC_ACADBUF],strtmp[50],*fcp1;
	char strpt0[IC_ACADBUF],strpt1[IC_ACADBUF],strpt2[IC_ACADBUF];
	RT rt1;
	bool bAgain=true;
	int fi1,scrollmax,scrollcur,savval;
	struct db_sysvarlink *sysvar=db_Getsysvar(),*svp1=NULL;

	rbucs.restype=rbwcs.restype=RTSHORT;
	rbucs.resval.rint=1;
	rbwcs.resval.rint=0;

	// This global var is so the command line can bypass the prompt for the variable itself.
	if (SDS_SetVarByPass) {
		strncpy(varname,SDS_SetVarByPass,sizeof(varname)-1);
		SDS_SetVarByPass=NULL;
	   if (strisame(varname,"TILEMODE"/*DNT*/ ) && cmd_istransparent() == RTERROR) return RTERROR;
	} else {
		if (0 != stricmp(lastvar,""/*DNT*/)) {
			sprintf(varname,"<%s>"/*DNT*/,lastvar);
		} else {
			strcpy(varname,lastvar);  // set varname to ""
		}
		sprintf(str2,ResourceString(IDC_CMDS4__NVARIABLE_NAME__153, "\nVariable name or ? %s: " ),varname);
		if (sds_initget(RSG_OTHER,NULL)!=RTNORM) return(RTERROR);
		if ((rt1=(sds_getkword(str2,varname)))==RTERROR){
			return(rt1);
		}else if (rt1==RTCAN){
			return(rt1);
		}else if (rt1==RTNONE){
			if (0!=(stricmp(""/*DNT*/,lastvar))) {		 // and there was a lastvar
				(strcpy(varname,lastvar));		   // use lastvar
			}else{								 // else exit
				return(RTCAN);
			}
		}
		strupr(varname);
		if (strisame(varname,"?"/*DNT*/) || strisame(varname,"_?"/*DNT*/)) {		  // if user wants a listing get vars list
			if (sds_initget(RSG_OTHER,NULL) != RTNORM) return(-2);
			if ((rt1=(sds_getkword(ResourceString(IDC_CMDS4__NVARIABLE_S__TO_155, "\nVariable(s) to list <*>: " ),pmt))) !=RTNORM && rt1!=RTNONE) return(rt1);
			if (pmt[0]==0 || rt1==RTNONE)strncpy(pmt,"*"/*DNT*/,2);
			strupr(pmt);

			SDS_getvar(NULL,DB_QCMDECHO,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			if (CMD_CMDECHO_PRINTF(rb.resval.rint)){
				sds_textscr();
				if (SDS_getvar(NULL,DB_QSCRLHIST,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) return(RTERROR);
				scrollmax=min(SDS_CMainWindow->m_pTextScrn->m_pPromptWnd->m_pPromptWndList->m_nLinesPerPage,rb.resval.rint);
				scrollmax-=3;
				scrollcur=0;

								// display wildcard setvars
				for (fi1=0; fi1<db_Getnvars(); fi1++) {
					if ((sysvar[fi1].spl & IC_SYSVARFLAG_UNDOCUMENTED)!=0) continue;
					if (sds_wcmatch(sysvar[fi1].name,pmt)!=RTNORM)continue;
					if (sds_getvar(sysvar[fi1].name,&tempbuf)!=RTNORM) {
						//removed so when no dwg open we don't get a msg
						//sds_printf("\nERROR: Unable to GET %s system variable",sysvar[fi1].name);
						continue;
					}

					int		index = fi1;
					bool	paperspace = false;
					if ( paperspace )
						switch ( index )
							{
							case DB_QELEVATION:
								index = DB_QP_ELEVATION;
								break;
							case DB_QEXTMAX:
								index = DB_QP_EXTMAX;
								break;
							case DB_QEXTMIN:
								index = DB_QP_EXTMIN;
								break;
							case DB_QINSBASE:
								index = DB_QP_INSBASE;
								break;
							case DB_QLIMMAX:
								index = DB_QP_LIMMAX;
								break;
							case DB_QLIMMIN:
								index = DB_QP_LIMMIN;
								break;
							case DB_QUCSNAME:
								index = DB_QP_UCSNAME;
								break;
							case DB_QUCSORG:
								index = DB_QP_UCSORG;
								break;
							case DB_QUCSXDIR:
								index = DB_QP_UCSXDIR;
								break;
							case DB_QUCSYDIR:
								index = DB_QP_UCSYDIR;
								break;
							}

					strncpy(varname,sysvar[index].name,sizeof(varname)-1);
					ic_rpad(varname,15);
					switch(tempbuf.restype){
						case RTREAL:
							cmd_prtreal(tempbuf.resval.rreal,varname);
							if (sysvar[index].spl&IC_SYSVARFLAG_USERREADONLY) sds_printf(ResourceString(IDC_CMDS4_____READ_ONLY__156, "   (Read Only)" ));
							if (cmd_entlister_scrollchk(&scrollcur,scrollmax)!=RTNORM) return(RTERROR);
							break;
						case RT3DPOINT:
						case RTPOINT:
							cmd_prtpt(tempbuf.resval.rpoint,varname);
							if (sysvar[index].spl&IC_SYSVARFLAG_USERREADONLY) sds_printf(ResourceString(IDC_CMDS4_____READ_ONLY__156, "   (Read Only)" ));
							if (cmd_entlister_scrollchk(&scrollcur,scrollmax)!=RTNORM) return(RTERROR);
							break;
						case RTSHORT:
							cmd_prtint(tempbuf.resval.rint,varname);
							if (sysvar[index].spl&IC_SYSVARFLAG_USERREADONLY) sds_printf(ResourceString(IDC_CMDS4_____READ_ONLY__156, "   (Read Only)" ));
							if (cmd_entlister_scrollchk(&scrollcur,scrollmax)!=RTNORM) return(RTERROR);
							break;
						case RTSTR:
							{
								char* cp1;
								CString cs="\""/*DNT*/;
								for (cp1=tempbuf.resval.rstring; *cp1!=0; cp1++) {
									if (*cp1=='\n'/*DNT*/) cs+="\\n"/*DNT*/;
									else if (*cp1=='\r'/*DNT*/) cs+="\\r"/*DNT*/;
									else cs+=(*cp1);
								}
								cs+="\""/*DNT*/;
								cmd_prtstr((char *)((LPCSTR)cs),varname);
							}
							if (sysvar[index].spl&IC_SYSVARFLAG_USERREADONLY) sds_printf(ResourceString(IDC_CMDS4_____READ_ONLY__156, "   (Read Only)" ));
							IC_FREE(tempbuf.resval.rstring);
							if (cmd_entlister_scrollchk(&scrollcur,scrollmax)!=RTNORM) return(RTERROR);
							break;
						case RTLONG:
							cmd_prtlong(tempbuf.resval.rlong,varname);
							if (sysvar[index].spl&IC_SYSVARFLAG_USERREADONLY) sds_printf(ResourceString(IDC_CMDS4_____READ_ONLY__156, "   (Read Only)" ));
							if (cmd_entlister_scrollchk(&scrollcur,scrollmax)!=RTNORM) return(RTERROR);
							break;
					}
				}
			}
			return(RTNORM);
		}
	}

	// check for valid var name & put in varbuf
	if ( (svp1=db_findsysvar(varname,-1,SDS_CURDWG))==NULL
			|| (					// skip undocumented flags unless paperspace equivalent
									// of a documented flag
				(svp1->spl & IC_SYSVARFLAG_UNDOCUMENTED)
				&& !(svp1->spl & IC_SYSVARFLAG_PAPERSPACE)
				)
			){
		cmd_ErrorPrompt(CMD_ERR_UNRECOGVAR,0);
		return(RTNORM);
	} else {
		strcpy(lastvar,(strupr(varname)));
	}
	if ((rt1=sds_getvar(varname,&varbuf))!=RTNORM)return(rt1);

	//This will report unsupported variables and then set them as normal
	// EBATECH(FUTA) skip under code , shoud be don't stop LISP code (command "DIMTFAC" 2)
	//if (strnsame("DIM"/*DNT*/ ,varname,3)){  //this will help speed by only checking dim vars
	//	  if (strsame("DIMTFAC"/*DNT*/,varname)||
	//		 strsame("DIMTOLJ"/*DNT*/,varname))
	//	  {
	//		cmd_ErrorPrompt(CMD_ERR_UNUSEDVARS,0);
	//	  }
	//}

	bAgain=true;
	while (bAgain){
		switch(svp1->type){
			case RTSHORT : {
				savval=varbuf.resval.rint;
			   if (strsame("PDMODE"/*DNT*/ ,varname)){
					do{
						sprintf(pmt,ResourceString(IDC_CMDS4__NNEW_CURRENT_VA_162, "\nNew current value for %s <%d>: " ),varname,varbuf.resval.rint);
						sds_initget(0,NULL);
						// Modified PK 31/05/2000
						calledfrom_cmd_setvar = 1; // Reason : Fix for bug no. 74219
						if ((rt1=sds_getint(pmt,&fi1))==RTERROR){
							continue;
						}else if (rt1==RTNONE) {
							return(RTNORM);
						}else if (rt1==RTNORM){
							switch(fi1){
								case   0:
								case   1:
								case   2:
								case   3:
								case   4:
								case  32:
								case  33:
								case  34:
								case  35:
								case  36:
								case  64:
								case  65:
								case  66:
								case  67:
								case  68:
								case  96:
								case  97:
								case  98:
								case  99:
								case 100:
									break;
								default:
									cmd_ErrorPrompt(CMD_ERR_NUMRANGE,0);
									rt1=RTERROR;
							}
						}
					}while (rt1==RTERROR);
					if (rt1!=RTNORM) return(rt1);
				}else{
					if (svp1->spl & IC_SYSVARFLAG_SPECIALSOURCE ||
						svp1->spl & IC_SYSVARFLAG_USERREADONLY){
						sds_printf(ResourceString(IDC_CMDS4__N_S____D__READ__163, "\n%s = %d (read only)" ),varname,varbuf.resval.rint,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
						return(RTNORM);
					}
					strncpy(str2,svp1->range,sizeof(str2)-1);
					if ((fcp1=strchr(str2,'_'/*DNT*/))!=NULL){
						*fcp1=0;
						fcp1++;
					}
					if (fcp1!=NULL){
						if (*fcp1)
							if (strsame(svp1->range,"0_1"/*DNT*/)) {
								if (varbuf.resval.rint){
									sprintf(pmt,ResourceString(IDC_CMDS4__NNEW_CURRENT_VA_165, "\nNew current value for %s (Off or On) <On>: " ),varname);
								}else{
									sprintf(pmt,ResourceString(IDC_CMDS4__NNEW_CURRENT_VA_166, "\nNew current value for %s (Off or On) <Off>: " ),varname);
								}
							}else{
								sprintf(pmt,ResourceString(IDC_CMDS4__NNEW_CURRENT_VA_167, "\nNew current value for %s (%s to %s) <%d>: " ),varname,str2,fcp1,varbuf.resval.rint);
							}
						else
							sprintf(pmt,ResourceString(IDC_CMDS4__NNEW_CURRENT_VA_168, "\nNew current value for %s (%s or greater) <%d>: " ),varname,str2,varbuf.resval.rint);
					}else{
							sprintf(pmt,ResourceString(IDC_CMDS4__NNEW_CURRENT_VA_162, "\nNew current value for %s <%d>: " ),varname,varbuf.resval.rint);
					}
					if (strsame(svp1->range,"0_1"/*DNT*/)) {
					   sds_initget(0, ResourceString(IDC_SETVAR_0_1_INIT1, "On|ON Off|OFf ~_ON ~_OFF"));
					} else {
						sds_initget(0,NULL);
					}
					// Modified PK 31/05/2000
					calledfrom_cmd_setvar = 1; // Reason : Fix for bug no. 74219
					rt1=(sds_getint(pmt,&fi1));
					if (rt1 == RTKWORD) {
						sds_getinput(pmt);
						if (strisame(pmt,"ON" )) fi1=1;
						if (strisame(pmt,"OFf" )) fi1=0;
						rt1=RTNORM;
					}
					if (rt1 == RTNONE) return(RTNORM);
					if (rt1 != RTNORM) return(rt1);
					if (fcp1!=NULL){
						if ((fi1<atof(str2))||((*fcp1)&&(fi1>atof(fcp1)))){
							cmd_ErrorPrompt(CMD_ERR_NUMRANGE,0);
							break;
						}
					}
				}
				tempbuf.resval.rint=(short)fi1;
				varbuf.resval.rint=tempbuf.resval.rint;
				/*D.G.*/// Regenerate a model after changing XCLIPFRAME or ISOLINES
				// and save this regeneration to the undo/redo stack.
				bool	bRegen = (strisame("XCLIPFRAME"/*DNT*/, varname) || strisame("ISOLINES"/*DNT*/, varname))
								 && varbuf.resval.rint != savval;
				if(bRegen)
					SDS_SetUndo(IC_UNDO_REGEN_ON_UNDO, NULL, NULL, NULL, SDS_CURDWG);
				if((rt1 = sds_setvar(varname,&varbuf)) == RTNORM)
				{
				   if(strisame("GRIDMODE"/*DNT*/, varname) && !varbuf.resval.rint && savval == 1)
					   cmd_redraw();
				   if(bRegen)
					   cmd_regen();
					return(rt1);
				}
				return(RTERROR);
				break;
			}
			case RTREAL : {
				if (svp1->spl & IC_SYSVARFLAG_SPECIALSOURCE || svp1->spl & IC_SYSVARFLAG_USERREADONLY) {  // Read only
					if (svp1->spl & IC_SYSVARFLAG_ANGLE){	//angle
						sds_angtos(varbuf.resval.rreal,-1,-1,str2);
					}else if (svp1->spl & IC_SYSVARFLAG_TIMEDATE){	 //date has 8 places
						if (strsame("DATE"/*DNT*/ ,varname)){
							cmd_prn_dwg_times(1);
							return(RTNORM);
						}
						sds_rtos(varbuf.resval.rreal,2,8,str2);
					}else{						 //true real
						sds_rtos(varbuf.resval.rreal,-1,-1,str2);
					}
					sds_printf(ResourceString(IDC_CMDS4__N_S____S__READ__173, "\n%s = %s (read only)" ),varname,str2);
					return(RTNORM);
				}
				if (svp1->spl & IC_SYSVARFLAG_ANGLE){	//angle
					//if we're getting angbase, remember that angtos accounts
					//	for angbase already.  We need to add angbase to itself
					//	(double it), because ANGTOF will subtract angbase in doing
					//	the conversion to string
					fi1=0;
					double fd1;
					SDS_getvar(NULL,DB_QANGBASE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
					fd1=rb.resval.rreal;
				   if ((0==stricmp(varname,"ANGBASE"/*DNT*/ ))||
					  (0==stricmp(varname,"CHAMFERD"/*DNT*/ )) ||
					  (0==stricmp(varname,"VIEWTWIST"/*DNT*/ ))){
						SDS_getvar(NULL,DB_QANGDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
						if (rb.resval.rint==1)fi1=2;
						else fi1=1;

						sds_angtos_absolute(varbuf.resval.rreal,-1,-1,str2);
					}else{
						sds_angtos(varbuf.resval.rreal,-1,-1,str2);
					}
					sprintf(pmt,ResourceString(IDC_CMDS4__NNEW_CURRENT_AN_176, "\nNew current angle for %s <%s>: " ),varname,str2);
					sds_initget(0,NULL);
					if (fi1==0)
						rt1=sds_getangle(NULL,pmt,&tempbuf.resval.rreal);
					else{
						rt1=sds_getorient(NULL,pmt,&tempbuf.resval.rreal);
						if (RTNORM==rt1){
							if(fi1==1)
								tempbuf.resval.rreal -= fd1; // CCW
							else if(fi1==2)
							{	// CW support
								tempbuf.resval.rreal = fd1 - tempbuf.resval.rreal; // inputted angle or normalized angle
								tempbuf.resval.rreal = -tempbuf.resval.rreal;	   // because CW
							}
							ic_normang(&tempbuf.resval.rreal, NULL);
						}
					}
					fcp1=NULL;
				}else{
					if (svp1->spl & IC_SYSVARFLAG_TIMEDATE){	//dates always to 8 places
						sds_rtos(varbuf.resval.rreal,2,8,str2);
					}else{
					   if (strisame(varname,"AREA" )){
							cmd_rtos_area(varbuf.resval.rreal,-1,-1,str2);
						}else{
							if (strisame(varname,"DIMALTF"/*DNT*/)||
							   strisame(varname,"DIMLFAC"/*DNT*/)||
							   strisame(varname,"DIMSCALE"/*DNT*/)||
							   strisame(varname,"DIMTFAC"/*DNT*/)||
							   strisame(varname,"CELTSCALE"/*DNT*/ )||
							   strisame(varname,"CMLSCALE"/*DNT*/ )||
							   strisame(varname,"FACETRES"/*DNT*/ )||
							   strisame(varname,"LTSCALE"/*DNT*/ )){
								sds_rtos(varbuf.resval.rreal,2,-1,str2);
							}else{
								sds_rtos(varbuf.resval.rreal,-1,-1,str2);
							}
						}
					}
					strncpy(strtmp,svp1->range,sizeof(strtmp)-1);
					if ((fcp1=strchr(strtmp,'_'/*DNT*/))!=NULL){
						*fcp1=0;
						fcp1++;
					}
					if (fcp1!=NULL){
						if (*fcp1)
							sprintf(pmt,ResourceString(IDC_CMDS4__NNEW_CURRENT_VA_182, "\nNew current value for %s (%s-%s) <%s>: " ),varname,strtmp,fcp1,str2);
						else if (atof(strtmp)>0.0 && atof(strtmp)<CMD_FUZZ)
							sprintf(pmt,ResourceString(IDC_CMDS4__NNEW_CURRENT_VA_183, "\nNew current value for %s (greater than zero) <%s>: " ),varname,str2);
						else
							sprintf(pmt,ResourceString(IDC_CMDS4__NNEW_CURRENT_VA_184, "\nNew current value for %s (%s or greater) <%s>: " ),varname,strtmp,str2);
					}else{
							sprintf(pmt,ResourceString(IDC_CMDS4__NNEW_CURRENT_VA_185, "\nNew current value for %s <%s>: " ),varname,str2);
					}
					sds_initget(0,NULL);
				   if (strisame(varname,"THICKNESS" ) || strisame(varname,"ELEVATION" )){
						rt1=(sds_getdist(NULL,pmt,&tempbuf.resval.rreal));
					}else{
						rt1=(sds_getreal(pmt,&tempbuf.resval.rreal));
					}
				}
				if (rt1 == RTNONE) return(RTNORM);
				if (rt1 != RTNORM) {
					return(rt1);
				} else {
					if (fcp1!=NULL){
						if ((tempbuf.resval.rreal<atof(strtmp))||((*fcp1)&&(tempbuf.resval.rreal>atof(fcp1)))){
							cmd_ErrorPrompt(CMD_ERR_NUMRANGE,0);
							break;
						}
					}
					varbuf.resval.rreal=tempbuf.resval.rreal;
					if ((rt1=(sds_setvar(varname,&varbuf)))==RTNORM) return(rt1);
				}
				return(RTERROR);
				break;
			}
			case RTSTR : {

				// EBATECH(watanabe)-[remove under score from block name
				if (strisame(varname, "DIMBLK"/*DNT*/ ) ||
					strisame(varname, "DIMBLK1"/*DNT*/) ||
					strisame(varname, "DIMBLK2"/*DNT*/) ||
					strisame(varname, "DIMLDRBLK"/*DNT*/))
				{
					if (varbuf.resval.rstring[0] == '_') {
						if (isArrowBlock(varbuf.resval.rstring + 1)) {
							char* p = new char[strlen(varbuf.resval.rstring)];							
							strcpy(p, varbuf.resval.rstring + 1);
							IC_FREE(varbuf.resval.rstring);
							varbuf.resval.rstring = p;
						}
					}
				}
				// ]-EBATECH(watanabe)

				char* cp1;
				CString cs="\""/*DNT*/;
				for (cp1=varbuf.resval.rstring; *cp1!=0; cp1++) {
					if (*cp1=='\n') cs+="\\n"/*DNT*/;
					else if (*cp1=='\r') cs+="\\r"/*DNT*/;
					else cs+=(*cp1);
				}
				cs+="\""/*DNT*/;

				if (svp1->spl & IC_SYSVARFLAG_SPECIALSOURCE || svp1->spl & IC_SYSVARFLAG_USERREADONLY) {  // Read only
					sds_printf(ResourceString(IDC_CMDS4__N_S____S__READ__173, "\n%s = %s (read only)" ),varname,(LPCSTR)cs);
					IC_FREE(varbuf.resval.rstring);
					return(RTNORM);
				}

				// PLOTSTYLE variables are read only when PSTYLEMODE == 1. EBATECH(CNBR) 2002/9/16
				if(	strisame(varname, "CPLOTSTYLE"/*DNT*/) ||
					strisame(varname, "DEFLPLSTYLE"/*DNT*/) ||
					strisame(varname, "DEFPLSTYLE"/*DNT*/))
				{
					SDS_getvar(NULL,DB_QPSTYLEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
					if( rb.resval.rint == DB_BYCOL_PSTYLEMODE )
					{
						sds_printf(ResourceString(IDC_CMDS4__N_S____S__READ__173, "\n%s = %s (read only)" ),varname,(LPCSTR)cs);
						IC_FREE(varbuf.resval.rstring);
						return(RTNORM);
					}
				}
				// CTAB and CPLOTSTYLE does not consider dot as none. 2002/9/30 ebatech(cnbr) -[
				if (strisame(varname, "CTAB"/*DNT*/ ) ||
					strisame(varname, "CPLOTSTYLE"/*DNT*/) ||
					strisame(varname, "DEFLPLSTYLE"/*DNT*/) ||
					strisame(varname, "DEFPLSTYLE"/*DNT*/))
				{
					sprintf(pmt,ResourceString(IDC_CMDS4__NNEW_CURRENT_VA_185, "\nNew current value for %s <%s>: "), varname,(LPCSTR)cs);
					rt1=(sds_getstring((svp1->spl & IC_SYSVARFLAG_FILEPATH) ? 1 : 0,pmt,str2));
					IC_FREE(varbuf.resval.rstring);
					if (0 == str2[0]) return(RTNORM);
				}
				// Otherwise, many string system variable can accept dot as none.
				else
				{
					// Make prompt same with Acad. 2002/8/29 ebatech(cnbr)
					sprintf(pmt,ResourceString(IDC_CMDS4__NNEW_VALUE_FOR__186, "\nNew value for %s, or . for none/<%s>: "), varname,(LPCSTR)cs);
					rt1=(sds_getstring((svp1->spl & IC_SYSVARFLAG_FILEPATH) ? 1 : 0,pmt,str2));
					IC_FREE(varbuf.resval.rstring);
					if (0 == str2[0]) return(RTNORM);
					if (str2[0]=='.' && str2[1]==0)str2[0]=0;
				}
				// EBATECH(CNBR) 2002/9/30 ]-
				if (rt1 != RTNORM) {
					return(rt1);
				} else {
					if (strisame(varname,"MODEMACRO"/*DNT*/ )){
//4M Spiros Item:31->
						MdMacro=str2;
						if (MdMacro.GetLength())
//4M Spiros Item:31<-
						sds_grtext(-1,str2,0);
//4M Spiros Item:31->
						else
							sds_grtext(-1,ResourceString(IDC_ICADWNDACTION_READY_1, "Ready" ),0);
//4M Spiros Item:31<-
					// EBATECH(watanabe)-[dimldrblk
					//} else if ((strisame(varname,"DIMBLK"/*DNT*/) || strisame(varname,"DIMBLK1"/*DNT*/) || strisame(varname,"DIMBLK2"/*DNT*/)) && str2[0]!=0){
					} else if ((strisame(varname,"DIMBLK"/*DNT*/ ) ||
								strisame(varname,"DIMBLK1"/*DNT*/) ||
								strisame(varname,"DIMBLK2"/*DNT*/) ||
								strisame(varname,"DIMLDRBLK"/*DNT*/))
								&& str2[0] != 0) {
					// ]-EBATECH(watanabe)
						// EBATECH(watanabe)-[add arrow block
						//if (strisame(ResourceString(IDC_CMDS4__NONE_188, "NONE"/*DNT*/ ),str2)||
						//	 strisame(ResourceString(IDC_CMDS4__CLOSED_190, "CLOSED"/*DNT*/ ),str2)||
						//	 strisame(ResourceString(IDC_CMDS4__DOT_192, "DOT"/*DNT*/ ),str2)||
						//	 strisame(ResourceString(IDC_CMDS4__CLOSEDBLANK_194, "CLOSEDBLANK"/*DNT*/ ),str2)||
						//	 strisame(ResourceString(IDC_CMDS4__OBLIQUE_196, "OBLIQUE"/*DNT*/ ),str2)||
						//	 strisame(ResourceString(IDC_CMDS4__ARCHTICK_198, "ARCHTICK"/*DNT*/ ),str2)||
						//	 strisame(ResourceString(IDC_CMDS4__OPEN_200, "OPEN"/*DNT*/ ),str2)||
						//	 strisame(ResourceString(IDC_CMDS4__ORIGIN_202, "ORIGIN"/*DNT*/ ),str2)||
						//	 strisame(ResourceString(IDC_CMDS4__OPEN90_204, "OPEN90"/*DNT*/ ),str2)||
						//	 strisame(ResourceString(IDC_CMDS4__OPEN30_206, "OPEN30"/*DNT*/ ),str2)||
						//	 strisame(ResourceString(IDC_CMDS4__DOTBLANK_208, "DOTBLANK"/*DNT*/ ),str2)||
						//	 strisame(ResourceString(IDC_CMDS4__DOTSMALL_210, "DOTSMALL"/*DNT*/ ),str2)||
						//	 strisame(ResourceString(IDC_CMDS4__BOXFILLED_212, "BOXFILLED"/*DNT*/ ),str2)||
						//	 strisame(ResourceString(IDC_CMDS4__BOXBLANK_214, "BOXBLANK" ),str2)||
						//	 strisame(ResourceString(IDC_CMDS4__DATUMFILLED_216, "DATUMFILLED"/*DNT*/ ),str2)||
						//	 strisame(ResourceString(IDC_CMDS4__DATUMBLANK_218, "DATUMBLANK"/*DNT*/ ),str2)||
						//	 strisame(ResourceString(IDC_CMDS4__INTEGRAL_220, "INTEGRAL"/*DNT*/ ),str2)) {
						//}else{
						if (isArrowBlock(str2)) {
							if (str2[0] != '_') {
								// add under score to block name
								char temp[IC_ACADBUF];
								strncpy(temp, str2, sizeof(temp));
								sprintf(str2, "_%s", temp);
							}
							cmd_addarrowblock(str2);
						}

						if (str2[0]) {
						// ]-EBATECH(watanabe)
							if ((tmp=sds_tblsearch("BLOCK"/*DNT*/,str2,0))==NULL){
								cmd_ErrorPrompt(CMD_ERR_FINDBLOCK,0);
								return(RTERROR);
							}
							IC_RELRB(tmp);
						}
					}
					varbuf.resval.rstring=str2;
					if ((rt1=(sds_setvar(varname,&varbuf)))!=RTNORM){
						cmd_ErrorPrompt(CMD_ERR_SYSVARVAL,0);
						return(rt1);
					}
				}
				return(RTNORM);
				break;
			}
			case RTLONG : {
				if (svp1->spl & IC_SYSVARFLAG_SPECIALSOURCE || svp1->spl & IC_SYSVARFLAG_USERREADONLY) {  // Read only
					sds_printf(ResourceString(IDC_CMDS4__N_S____LD__READ_223, "\n%s = %ld (read only)" ),varname,varbuf.resval.rlong);
					IC_FREE(varbuf.resval.rstring);
					return(RTNORM);
				}
				sprintf(pmt,ResourceString(IDC_CMDS4__NNEW_VALUE_FOR__224, "\nNew value for %s <%ld>: " ),varname,varbuf.resval.rlong);
				// Modified PK 31/05/2000
				calledfrom_cmd_setvar = 1; // Reason : Fix for bug no. 74219
				rt1=(sds_getint(pmt,&fi1));
				if (rt1 != RTNORM) {
					return(rt1);
				} else {
					varbuf.resval.rlong=fi1;
					if ((rt1=(sds_setvar(varname,&varbuf)))!=RTNORM){
						cmd_ErrorPrompt(CMD_ERR_SYSVARVAL,0);
						return(rt1);
					}
				}
				return(RTNORM);
				break;
			}
			case RT3DPOINT : {
				sds_rtos(varbuf.resval.rpoint[0],-1,-1,strpt0);
				sds_rtos(varbuf.resval.rpoint[1],-1,-1,strpt1);
				sds_rtos(varbuf.resval.rpoint[2],-1,-1,strpt2);
				if (svp1->spl & IC_SYSVARFLAG_SPECIALSOURCE || svp1->spl & IC_SYSVARFLAG_USERREADONLY) {  // Read only
					sds_printf(ResourceString(IDC_CMDS4__N_S____S__S__S__225, "\n%s = %s,%s,%s (read only)" ),varname,
						strpt0,
						strpt1,
						strpt2);
					return(RTNORM);
				}
				sprintf(pmt,ResourceString(IDC_CMDS4__NNEW_VALUE_FOR__226, "\nNew value for %s <%s,%s,%s>: " ),varname,
						strpt0,
						strpt1,
						strpt2);
				sds_initget(0,NULL);
				rt1=(sds_getpoint(NULL,pmt,tempbuf.resval.rpoint));
				if (rt1 == RTNONE) return(0);
				if (rt1 != RTNORM) {
					return(rt1);
				} else {
					varbuf.resval.rpoint[0]=tempbuf.resval.rpoint[0];
					varbuf.resval.rpoint[1]=tempbuf.resval.rpoint[1];
					varbuf.resval.rpoint[2]=tempbuf.resval.rpoint[2];
					if ((rt1=(sds_setvar(varname,&varbuf)))!=RTNORM){
						cmd_ErrorPrompt(CMD_ERR_SYSVARVAL,0);
						return(rt1);
					}
					return(RTNORM);
				}
				return(RTERROR);
				break;
			}
			case RTPOINT : {
				sds_rtos(varbuf.resval.rpoint[0],-1,-1,strpt0);
				sds_rtos(varbuf.resval.rpoint[1],-1,-1,strpt1);
				if (svp1->spl & IC_SYSVARFLAG_SPECIALSOURCE || svp1->spl & IC_SYSVARFLAG_USERREADONLY) {  // Read only
					sds_printf(ResourceString(IDC_CMDS4__N_S____S__S__RE_227, "\n%s = %s,%s (read only)" ),varname,
						strpt0,
						strpt1);
					return(RTNORM);
				}
				sprintf(pmt,ResourceString(IDC_CMDS4__NNEW_VALUE_FOR__228, "\nNew value for %s <%s,%s>: " ),varname,
						strpt0,
						strpt1);
				sds_initget(0,NULL);
				rt1=(sds_getpoint(NULL,pmt,tempbuf.resval.rpoint));
				if (rt1 == RTNONE) return(RTNORM);
				if (rt1 != RTNORM) {
					return(rt1);
				} else {
					varbuf.resval.rpoint[0]=tempbuf.resval.rpoint[0];
					varbuf.resval.rpoint[1]=tempbuf.resval.rpoint[1];
					if ((rt1=(sds_setvar(varname,&varbuf)))!=RTNORM){
						cmd_ErrorPrompt(CMD_ERR_SYSVARVAL,0);
						return(rt1);
					}
					return(RTNORM);
				}
				return(RTERROR);
				break;
			}
		}
	}
	return(RTNORM);
}

short cmd_dragmode(void) {

	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	RT rt1;
	char pmt[IC_ACADBUF], dmval[IC_ACADBUF];
	struct resbuf rb1;

	//	retrieve exist dragmode variable
	SDS_getvar(NULL,DB_QDRAGMODE,&rb1,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

	//	convert to string equivalent for prompt
	switch (rb1.resval.rint)	{
		case 0 : { strcpy(dmval,ResourceString(IDC_CMDS4_OFF_229, "Off" ));  break; }
		case 1 : { strcpy(dmval,ResourceString(IDC_CMDS4_ON_230, "On" ));	break; }
		case 2 : { strcpy(dmval,ResourceString(IDC_CMDS4_AUTO_231, "Auto" )); break; }
	}

	//	get key word from user for new value
	sprintf(pmt,ResourceString(IDC_CMDS4__NON___OFF___AUT_232, "\nON/OFF/Auto <%s>: " ),dmval);
	sds_initget(0,ResourceString(IDC_CMDS4_ON_OFF___AUTO_233, "ON OFf ~ Auto ~_ON ~_OFF ~_ ~_AUTO" ));
	if ((rt1=(sds_getkword(pmt,dmval))) != RTNORM) return(rt1);

	//	set new value
	if (0 == (stricmp(dmval,ResourceString(IDC_CMDS4_OFF_170, "OFf" )))) rb1.resval.rint=0;
	if (0 == (stricmp(dmval,ResourceString(IDC_CMDS4_ON_131, "ON" )))) rb1.resval.rint=1;
	if (0 == (stricmp(dmval,ResourceString(IDC_CMDS4_AUTO_231, "Auto" )))) rb1.resval.rint=2;
	if ((rt1=(sds_setvar("DRAGMODE"/*DNT*/,&rb1))) != RTNORM) return(rt1);

	return(rt1);
}

short cmd_osnap(void) {
// If the user enter a new osnap mode ("mid,qua" for instance), this function kills off any
// other osnap modes they had set before that, which is correct - it should do that.

	if (cmd_iswindowopen() == RTERROR) return RTERROR;
//4M Item:77->
	bool RunEsnap=false;
//<-4M Item:77
	int newmode;
	char quickchk, flagint;
	RT rt1;
	char osnapstr[IC_ACADBUF], *osnaptok;
	struct resbuf rb1;

	//	retrieve exist variable
	SDS_getvar(NULL,DB_QOSMODE,&rb1,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

	do {
		LOAD_COMMAND_OPTIONS_3(IDC_CMDS4_NEAREST_NEAREST__235)
		sds_initget(RSG_OTHER, LOADEDSTRING);
//4M Item:77->
/*
		rt1=(sds_getkword(ResourceString(IDC_CMDS4__NSNAPS__NEAREST_238, "\nSnaps: NEArest/ENDpoint/MIDpoint/CENter/PERpendicular/TANgent/QUAdrant/INSertion/POInt/INTersection/PLAnview/<Off>: " ),osnapstr));
*/
		rt1=(sds_getkword(ResourceString(IDC_CMDS4__NSNAPS__NEAREST_238, "\nSnaps: NEArest/ENDpoint/MIDpoint/CENter/PERpendicular/TANgent/QUAdrant/INSertion/POInt/INTersection/PLAnview/ON/<OFF>: " ),osnapstr));
//<-4M Item:77
//		Replace above two lines with these when we want to add Visible intersection back in.
//								"~ Intersection|INTersection Plan_view_intersection|PLAnviewint Visible_intersection|VISible ~APParent ~ Clear_entity_snaps|Off");
//		rt1=(sds_getkword("\nSnaps: NEArest/ENDpoint/MIDpoint/CENter/PERpendicular/TANgent/QUAdrant/INSertion/POInt/INTersection/PLAnview/VISible/<Off>: ",osnapstr));
		if (rt1==RTNONE) {		// Sets osmode to 0 (all entity snaps OFF), if user ENTERs.
//4M Item:77->
/*
		   newmode=0;
*/
		 newmode=rb1.resval.rint | IC_OSMODE_OFF;
//4M Item:77<-
			rb1.resval.rint=newmode;
			if ((rt1=(sds_setvar("OSMODE"/*DNT*/,&rb1))) != RTNORM) return(RTERROR);
			return(RTNORM);
		}
		if (rt1!=RTNORM && rt1!=RTSTR) return(rt1);
		if (!*osnapstr) {
			return(RTNORM);
		}
		strupr(osnapstr);
		newmode=quickchk=flagint=0;
		osnaptok=strtok(osnapstr,","/*DNT*/);
		if (osnaptok != NULL && *osnaptok == '_') osnaptok++;
		while ((osnaptok != NULL) && (flagint == 0)) {
			//	get new value for osmode
				 if (strnisame(osnaptok,ResourceString(IDC_CMDS4_END_241, "END" ),3) || strnisame(osnaptok,"END"/*DNT*/ ,3)) newmode = newmode | 1;
			else if (strnisame(osnaptok,ResourceString(IDC_CMDS4_MID_242, "MID" ),3) || strnisame(osnaptok,"MID"/*DNT*/ ,3)) newmode = newmode | 2;
			else if (strnisame(osnaptok,ResourceString(IDC_CMDS4_CEN_243, "CEN" ),3) || strnisame(osnaptok,"CEN"/*DNT*/ ,3)) newmode = newmode | 4;
			else if (strnisame(osnaptok,ResourceString(IDC_CMDS4_NOD_244, "NOD" ),3) || strnisame(osnaptok,"NOD"/*DNT*/ ,3)) newmode = newmode | 8;
			else if (strnisame(osnaptok,ResourceString(IDC_CMDS4_POI_245, "POI" ),3) || strnisame(osnaptok,"POI"/*DNT*/ ,3)) newmode = newmode | 8;
			else if (strnisame(osnaptok,ResourceString(IDC_CMDS4_QUA_246, "QUA" ),3) || strnisame(osnaptok,"QUA"/*DNT*/ ,3)) newmode = newmode | 16;
			else if (strnisame(osnaptok,ResourceString(IDC_CMDS4_INT_247, "INT" ),3) || strnisame(osnaptok,"INT"/*DNT*/ ,3)) newmode = newmode | 32;
			else if (strnisame(osnaptok,ResourceString(IDC_CMDS4_INS_248, "INS" ),3) || strnisame(osnaptok,"INS"/*DNT*/ ,3)) newmode = newmode | 64;
			else if (strnisame(osnaptok,ResourceString(IDC_CMDS4_PER_249, "PER" ),3) || strnisame(osnaptok,"PER"/*DNT*/ ,3)) newmode = newmode | 128;
			else if (strnisame(osnaptok,ResourceString(IDC_CMDS4_TAN_250, "TAN" ),3) || strnisame(osnaptok,"TAN"/*DNT*/ ,3)) newmode = newmode | 256;
			else if (strnisame(osnaptok,ResourceString(IDC_CMDS4_NEA_251, "NEA" ),3) || strnisame(osnaptok,"NEA"/*DNT*/ ,3)) newmode = newmode | 512;
			//quick is 1024 - see below

//			  else if (strnisame(osnaptok,"APP",3)) newmode = newmode | 2048;	// use these when we enable Visible/Apparent snap
//			  else if (strnisame(osnaptok,"VIS",3)) newmode = newmode | 2048;
			else if (strnisame(osnaptok,ResourceString(IDC_CMDS4_APP_252, "APP" ),3) || strnisame(osnaptok,"APP"/*DNT*/ ,3)) {			  // temporary - replace w/ above
				flagint=1;
				cmd_ErrorPrompt(CMD_ERR_OPTIONNOTSUP,0);
			}
		   else if (strnisame(osnaptok,ResourceString(IDC_CMDS4_VIS_253, "VIS" ),3) || strnisame(osnaptok,"VIS"/*DNT*/ ,3)) {
				flagint=1;
				cmd_ErrorPrompt(CMD_ERR_OPTIONNOTSUP,0);
			}

			else if (strnisame(osnaptok,ResourceString(IDC_CMDS4_PLA_254, "PLA" ),3) || strnisame(osnaptok,"PLA"/*DNT*/ ,3)) newmode = newmode | 4096;
			//	quick mode must be used in conjunction with at least 1 other mode
			else if (strnisame(osnaptok,ResourceString(IDC_CMDS4_QUI_255, "QUI" ),3) || strnisame(osnaptok,"QUI"/*DNT*/ ,3)) quickchk = 1;
//4M Item:77->
#if 0
			else if (strnisame(osnaptok,ResourceString(IDC_CMDS4_NON_256, "NON" ),3) || strnisame(osnaptok,"NON"/*DNT*/ ,3)) newmode = newmode;//do nothing
			else if (strnisame(osnaptok,ResourceString(IDC_CMDS4_OFF_257, "OFF" ),3) || strnisame(osnaptok,"OFF"/*DNT*/ ,3)) newmode = newmode;//do nothing
#endif
			else if (strnisame(osnaptok,ResourceString(IDC_CMDS4_NON_256, "NON" ),3) || strnisame(osnaptok,"NON"/*DNT*/ ,3)) newmode = 0; // Resets to 0
			else if (strnisame(osnaptok,ResourceString(IDC_CMDS4_ON_259,  "ON"	),2) || strnisame(osnaptok,"ON" /*DNT*/ ,2))
			{
			   newmode = rb1.resval.rint & (~IC_OSMODE_OFF);
			   if (newmode == 0){
				  RunEsnap=true;
			   }
			}
			else if (strnisame(osnaptok,ResourceString(IDC_CMDS4_OFF_257, "OFF" ),3) || strnisame(osnaptok,"OFF"/*DNT*/ ,3)) newmode = rb1.resval.rint | IC_OSMODE_OFF;
			else if (strnisame(osnaptok,ResourceString(IDC_CMDS4_TOGGLE_260,"T" ),1) || strnisame(osnaptok,"T"	/*DNT*/ ,1)){	  // Toggles Osnap
			   if (rb1.resval.rint & IC_OSMODE_OFF){ // If Osnap== OFF then set to On
				  newmode = rb1.resval.rint & (~IC_OSMODE_OFF);
				  if (newmode == 0){
					 RunEsnap=true;
				  }
			   }
			   else{								 // else set to Off
				  newmode = rb1.resval.rint | IC_OSMODE_OFF;
			   }
			}
//4M Item:77<-
			else {
				 flagint = 1;
				 cmd_ErrorPrompt(CMD_ERR_UNRECOGSNAP,0);
			}
			osnaptok=strtok(NULL,","/*DNT*/ );
			if (osnaptok != NULL && *osnaptok == '_') osnaptok++;
		}
		if (0==flagint && quickchk==1){
			if (newmode > 0){
				newmode=newmode | 1024;
			}else{
				flagint=1;
				cmd_ErrorPrompt(CMD_ERR_NOQUICKSNAP,0);
			}
		}
	} while (flagint);
	//check multiple insert modes precedence: 3d then 2d then apparent
	if (newmode & 32){	//3d
		if (newmode & 4096) newmode-=4096;
		if (newmode & 2048) newmode-=2048;
	}
	if (newmode & 4096){	//2d
		if (newmode & 2048) newmode-=2048;
	}
	rb1.resval.rint=newmode;
	if ((rt1=(sds_setvar("OSMODE"/*DNT*/,&rb1))) != RTNORM) return(RTERROR);
//4M Item:77->
	if (RunEsnap){
	   cmd_ddosnap();
	}
//<-4M Item:77

	return (RTNORM);
}


/**********************************************************************************
 *
 * The class defines the functionality for the operations to modify one curve 
 * by using another curve in 3D space or projected direction.
 *
 * Author:  Stephen W. Hou
 * Date:    June 17, 2004
 *
 **********************************************************************************/
class CCurveCurveModifier
{
public:

	enum OperType { Extended = 0, Trimmed };

protected:

	CCurveCurveModifier();
	CCurveCurveModifier(const CCurve* pModifiedCurve,   // The modified curve
                        const CPlane & projPlane);      // Optional project plane

public:

	virtual ~CCurveCurveModifier();

	virtual void		SetModifiedCurve(const CCurve* pModifiedCurve);
	const CCurve*	    GetModifiedCurve() const { return m_pModifiedCurve; }

	virtual void	    SetProjPlane(const CPlane &projPlane);
	const CPlane*	    GetProjPlane() const { return m_pProjPlane; }

	virtual void	    SetRefPoint(double rAt) { m_RefPoint = rAt; }
	double			    GetRefPoint() const { return m_RefPoint; }

    virtual OperType    GetOperType() const = 0;

    void                GetCandidatePoints(CRealArray &candidates) const;

	virtual bool	    ModifiedWith(const CCurve* pCurve);

protected:

    // These functions are called at the begin and end in ModifiedWith() funciton
    // respectively.
    //
    virtual bool        BeforeComputeModification() { return true; }
    virtual bool        AfterComputeModification() { return true; }

private:

	virtual bool	    AddIntersection(double rOnCrv3d);

protected:
	
	CCurve*			    m_pModifiedCurve;
	CCurve*			    m_pModifiedCurve2d;

	CPlane*			    m_pProjPlane;
	double			    m_RefPoint;

    CRealArray          m_CandidatePoints;
};


CCurveCurveModifier::CCurveCurveModifier()
                    : m_pModifiedCurve(NULL), m_pModifiedCurve2d(NULL), 
                      m_pProjPlane(NULL), m_RefPoint(0.0)
{
}

	
CCurveCurveModifier::CCurveCurveModifier(const CCurve* pModifiedCurve, const CPlane &projPlane)
                    : m_pModifiedCurve(NULL), m_pModifiedCurve2d(NULL), m_RefPoint(0.0)
{
    m_pProjPlane =(&projPlane) ? new CPlane(projPlane) : NULL;
	m_pModifiedCurve = pModifiedCurve->Clone();
}


CCurveCurveModifier::~CCurveCurveModifier()
{
	if (m_pModifiedCurve)
		delete m_pModifiedCurve;

	if (m_pModifiedCurve2d)
		delete m_pModifiedCurve2d;

    if (m_pProjPlane)
        delete m_pProjPlane;
}


void CCurveCurveModifier::SetModifiedCurve(const CCurve* pModifiedCurve)
{
	if (m_pModifiedCurve) 
		delete m_pModifiedCurve; 

	m_pModifiedCurve = (pModifiedCurve) ? pModifiedCurve->Clone() : NULL;
}


void CCurveCurveModifier::SetProjPlane(const CPlane &projPlane)
{
    if (m_pModifiedCurve2d) {
        delete m_pModifiedCurve2d;
        m_pModifiedCurve2d = NULL; 
    }
    if (m_pProjPlane)
        delete m_pProjPlane;

    m_pProjPlane = new CPlane(projPlane);
}


bool CCurveCurveModifier::ModifiedWith(const CCurve* pCurve)
{
	bool bModified = false;
    if (!BeforeComputeModification())
        return bModified;

	if (!pCurve || !m_pModifiedCurve)
		return bModified;

    if (m_pProjPlane) { // compute the intersections of curves in project direction.
        bool use2DCurves = true;
        C3Vector projDir = m_pProjPlane->GetNormal().Negate();
        C3Point dir = projDir.AsPoint();
    
        if (!m_pModifiedCurve2d) 
		    m_pModifiedCurve2d = m_pModifiedCurve->ProjectTo(*m_pProjPlane, projDir, use2DCurves);
    
        if (m_pModifiedCurve2d) {
		    CCurve* pCurve2d = pCurve->ProjectTo(*m_pProjPlane, projDir, use2DCurves);

		    CRealArray cOnModifiedCurve2d, cOnCrv2d, cOnModifiedCurve, cOnViewingLine;
		    RC rc = Intersect(m_pModifiedCurve2d, pCurve2d, cOnModifiedCurve2d, cOnCrv2d, INT_EPSILON1); 

		    if (rc == SUCCESS && cOnModifiedCurve2d.Size() > 0) {
			    double rCurvature;
			    C3Point ptPoint, ptTangent;
                CAffine planeToWCSXForm;
                planeToWCSXForm.SetInverse(m_pProjPlane->WcsToEcsTransform());
			    for (register i = 0; i < cOnModifiedCurve2d.Size(); i++) {
                    // The parameterization of 2D and 3D curves may be different so we can not directly
                    // use the parameter to intersection points on 3D curve.
                    //
				    m_pModifiedCurve2d->Evaluate(cOnModifiedCurve2d[i], ptPoint, ptTangent, rCurvature);
                    if (use2DCurves) {
                        // the point we got is in the the coordinate system of the projection plane
                        // so we need to transform it back to the model space.
                        //
                        planeToWCSXForm.ApplyToPoint(ptPoint, ptPoint);
                    }
				    CUnboundedLine viewingLine(ptPoint, m_pProjPlane->GetNormal().AsPoint());
				    Intersect(m_pModifiedCurve, &viewingLine, cOnModifiedCurve, cOnViewingLine, INT_EPSILON1);
					    
                    for (register j = 0; j < cOnModifiedCurve.Size(); j++) {
					    if (AddIntersection(cOnModifiedCurve[j]))
						    bModified = true;
				    }
			    }
		    }
	    }
    }
    else { 
        // compute real intersections of curves
        //
	    CRealArray cOnModifiedCurve, cOnCurve;
		Intersect(m_pModifiedCurve, const_cast<CCurve*>(pCurve), cOnModifiedCurve, cOnCurve, INT_EPSILON); 

		for (register i = 0; i < cOnModifiedCurve.Size(); i++) {
            if (AddIntersection(cOnModifiedCurve[i]))
			    bModified = true;
		}
    }
    if (!AfterComputeModification())
        return false;
    else
	    return bModified;
}


void CCurveCurveModifier::GetCandidatePoints(CRealArray &candidates) const 
{
    candidates = m_CandidatePoints;
}


bool CCurveCurveModifier::AddIntersection(double rOnCrv3d)
{
	bool bModified = true;

    int rAt;
    m_CandidatePoints.AddSort(rOnCrv3d, EPSILON, rAt);

	return bModified;
}


/***********************************************************************************
 * 
 * This class implements the functionality for trimming operation.
 *
 * Author:  Stephen W. Hou
 * Date:    July 7, 2004
 *
 ***********************************************************************************/
class CCurveTrimModifier : public CCurveCurveModifier
{
public:

    CCurveTrimModifier();
	CCurveTrimModifier(const CCurve* pModifiedCurve, 
                      const CPlane & projPlane = *(CPlane*)NULL);
    virtual ~CCurveTrimModifier() {}

    virtual OperType    GetOperType() const { return CCurveCurveModifier::Trimmed; }

	virtual bool	    GetCandidates(double &rAt1, double &rAt2) const;
};


CCurveTrimModifier::CCurveTrimModifier()
{
}

CCurveTrimModifier::CCurveTrimModifier(const CCurve* pModified, const CPlane & projPlane)
: CCurveCurveModifier(pModified, projPlane)
{
}

bool CCurveTrimModifier::GetCandidates(double &rAt1, double & rAt2) const
{
    bool bFound = false;
    for (register i = 0; i < m_CandidatePoints.Size() - 1; i++) {
        if (m_RefPoint >= m_CandidatePoints[i] && m_RefPoint <= m_CandidatePoints[i + 1]) {
            rAt1 = m_CandidatePoints[i];
            rAt2 = m_CandidatePoints[i + 1];
            bFound = true;
        }
    }
    if (!bFound) {
        if (!m_pModifiedCurve->Closed()) {
            if (m_RefPoint >= m_pModifiedCurve->Start() && 
                m_RefPoint <= m_CandidatePoints[0]) {
                rAt1 = m_pModifiedCurve->Start();
                rAt2 = m_CandidatePoints[0];
                bFound = true;
            }
            else if (m_RefPoint >= m_CandidatePoints[m_CandidatePoints.Size() - 1] && 
                m_RefPoint <= m_pModifiedCurve->End()) {
                rAt1 = m_CandidatePoints[m_CandidatePoints.Size() - 1];
                rAt2 = m_pModifiedCurve->End();
                bFound = true;
            }
        }
        else {
            if (m_RefPoint >= m_CandidatePoints[m_CandidatePoints.Size() - 1]) {
                double param = m_pModifiedCurve->Period() + m_CandidatePoints[0];
                if (m_RefPoint <= param) {
                    rAt1 = m_CandidatePoints[m_CandidatePoints.Size() - 1];
                    rAt2 = param;
                    bFound = true;
                }
            }
            else if (m_RefPoint <= m_CandidatePoints[0]) {
                double param = m_CandidatePoints[m_CandidatePoints.Size() - 1] - m_pModifiedCurve->Period();
                if (m_RefPoint >= param) {
                    rAt1 = param;
                    rAt2 = m_CandidatePoints[0];
                    bFound = true;
                }
            }
        }
    }

    return bFound;
}

/***********************************************************************************
 * 
 * This class implements the functionality for trimming operation on a composite curve.
 *
 * Author:  Stephen W. Hou
 * Date:    July 13, 2004
 *
 ***********************************************************************************/
class CCompositeCurveTrimModifier : public CCurveTrimModifier
{
public:

    CCompositeCurveTrimModifier() {}
	CCompositeCurveTrimModifier(const CCurve* pModifiedCurve, 
                                const CPlane & projPlane = *(CPlane*)NULL);
    virtual ~CCompositeCurveTrimModifier() {}

	bool			    ModifiedWith(const CCurve* pCurve);
};

CCompositeCurveTrimModifier::CCompositeCurveTrimModifier(const CCurve* pModified, 
                                                       const CPlane & projPlane)
                           : CCurveTrimModifier(pModified, projPlane)
{
}

bool CCompositeCurveTrimModifier::ModifiedWith(const CCurve* pCurve)
{
    CCompositeCurve* pCompositeCurve = static_cast<CCompositeCurve*>(m_pModifiedCurve);
    if (!pCompositeCurve)
        return false;

    bool bChanged = false;
    int num = pCompositeCurve->NumOfSegments();
    for (register i = 0; i < num; i++) {
        const CCurve* pCurveSegment = pCompositeCurve->Segment(i);
        CCurveTrimModifier modifier(pCurveSegment, *m_pProjPlane);
        if (modifier.ModifiedWith(pCurve)) {
            CRealArray candidates;
            modifier.GetCandidatePoints(candidates);

            double lowBound, upBound;
            pCompositeCurve->GetSegmentDomain(i, lowBound, upBound);

            double start = pCurveSegment->Start();
            for (register j = 0; j < candidates.Size(); j++) {
                m_CandidatePoints.AddSort(lowBound + candidates[j] - start);
                bChanged = true;
            }
        }
    }
    return bChanged;
}

/***********************************************************************************
 * 
 * This class implements the functionality for extending operation.
 *
 * Author:  Stephen W. Hou
 * Date:    July 21, 2004
 *
 ***********************************************************************************/
class CCurveExtendModifier : public CCurveCurveModifier
{
public:

    enum PointType
    {
        StartPoint = 0,
        EndPoint,
        Unknown
    };

    CCurveExtendModifier();
	CCurveExtendModifier(const CCurve* pModifiedCurve, 
                         const CPlane & projPlane = *(CPlane*)NULL);
    virtual ~CCurveExtendModifier();

	virtual bool	    ModifiedWith(const CCurve* pCurve);

	virtual void		SetModifiedCurve(const CCurve* pModifiedCurve);
	virtual void	    SetRefPoint(double rAt);

    virtual OperType    GetOperType() const { return CCurveCurveModifier::Extended; }

    virtual bool        GetMovedPoint(double &rAt, C3Point &ptMoved) const;
	virtual bool	    GetTargetPoint(double &rTo, C3Point &ptTarget) const;

    PointType           MovedPointType() const { return m_MovedPointType; }

    virtual bool        UpdateDatabase(sds_name ename) const = 0;

protected:

    virtual void        FindExtendedEndPoint();

    virtual bool        BeforeComputeModification();
    virtual bool        AfterComputeModification();

protected:

    PCurve              m_pCurve;
    bool                m_Swapped;

    PointType           m_MovedPointType;
            
    // The end points of the modified curve
    //
    double              m_CurveBound[2];
};

CCurveExtendModifier::CCurveExtendModifier() 
                         : m_MovedPointType(CCurveExtendModifier::Unknown), 
                           m_pCurve(NULL), m_Swapped(false)
{
}

CCurveExtendModifier::CCurveExtendModifier(const CCurve* pModified, const CPlane & projPlane)
                         : CCurveCurveModifier(pModified, projPlane), 
                           m_MovedPointType(CCurveExtendModifier::Unknown),
                           m_pCurve(NULL), m_Swapped(false)
{
    m_CurveBound[0] = pModified->Start();
    m_CurveBound[1] = pModified->End();
}

CCurveExtendModifier::~CCurveExtendModifier()
{
    if (m_pCurve)
        delete m_pCurve;
}

bool CCurveExtendModifier::ModifiedWith(const CCurve* pCurve)
{
    if (m_pModifiedCurve->Closed())
        return false;

    return CCurveCurveModifier::ModifiedWith(pCurve);
}

void CCurveExtendModifier::SetModifiedCurve(const CCurve* pModifiedCurve)
{
    m_CurveBound[0] = pModifiedCurve->Start();
    m_CurveBound[1] = pModifiedCurve->End();

    CCurveCurveModifier::SetModifiedCurve(pModifiedCurve);
}

void CCurveExtendModifier::SetRefPoint(double rAt)
{
    CCurveCurveModifier::SetRefPoint(rAt);
}


bool CCurveExtendModifier::GetMovedPoint(double &rAt, C3Point &ptMoved) const
{
    bool bSuccess = true;
    if (m_MovedPointType == CCurveExtendModifier::StartPoint) 
        rAt = m_CurveBound[0];
    else if (m_MovedPointType == CCurveExtendModifier::EndPoint) 
        rAt = m_CurveBound[1];
    else
        bSuccess = false; // Unknown - the point has not been identified yet

    if (bSuccess)
        return (m_pModifiedCurve->Evaluate(rAt, ptMoved) == SUCCESS);
    else
        return false;
}

bool CCurveExtendModifier::GetTargetPoint(double &rTo, C3Point &ptTarget) const
{
    bool bFound = false;
    double rFrom, rCandidate;
    C3Point ptMoved;
    if (m_CandidatePoints.Size() && GetMovedPoint(rFrom, ptMoved)) {
        if (MovedPointType() == CCurveExtendModifier::StartPoint) {
            rTo = -INFINITY;
            for (register i = 0; i < m_CandidatePoints.Size(); i++) {
                rCandidate = m_CandidatePoints[i];
                if (rCandidate < m_CurveBound[0] && rCandidate > rTo) {
                    rTo = rCandidate;
                    bFound = true;
                }
            }
        }
        else {
            rTo = INFINITY;
            for (register i = 0; i < m_CandidatePoints.Size(); i++) {
                rCandidate = m_CandidatePoints[i];
                if (rCandidate > m_CurveBound[1] && rCandidate < rTo) {
                    rTo = rCandidate;
                    bFound = true;
                }
            }
        }
        if (bFound) {
            // Since the location to be extended to is outside the bound of the 
            // original curve, we need to use new unbounded curve to evaluate 
            // the point.
            //
            m_pCurve->Evaluate(rTo, ptTarget);
        }
    }

    return bFound;
}

void CCurveExtendModifier::FindExtendedEndPoint()
{
    if (m_pModifiedCurve && !m_pModifiedCurve->Closed()) {
        double disToStart = fabs(m_RefPoint - m_CurveBound[0]);
        double disToEnd = fabs(m_RefPoint - m_CurveBound[1]);
        m_MovedPointType = (disToStart < disToEnd) ?
                            CCurveExtendModifier::StartPoint :
                            CCurveExtendModifier::EndPoint;
    }
}


bool CCurveExtendModifier::BeforeComputeModification()
{
    if (!m_pCurve)
        return false;

    FindExtendedEndPoint();

    // Swap the modified line with an unbounded line.
    //
    if (!m_Swapped && m_pModifiedCurve) {
        CCurve* pTmp = m_pModifiedCurve;
        m_pModifiedCurve = m_pCurve;
        m_pCurve = pTmp;
        m_Swapped = true;
    }
    return true;
}

bool CCurveExtendModifier::AfterComputeModification()
{
    // Swap the original line back if it has been swapped.
    //
    if (m_Swapped) {
        CCurve* pTmp = m_pModifiedCurve;
        m_pModifiedCurve = m_pCurve;
        m_pCurve = pTmp;
        m_Swapped = false;
    }
    return true;
}


/***********************************************************************************
 * 
 * This class implements projected extending operation for line.
 *
 * Author:  Stephen W. Hou
 * Date:    July 22, 2004
 *
 ***********************************************************************************/
class CLineExtendModifier : public CCurveExtendModifier
{
public:

    CLineExtendModifier();
	CLineExtendModifier(const CLine* pLine, 
                        const CPlane & projPlane = *(CPlane*)NULL);
    virtual ~CLineExtendModifier();

	virtual bool	    ModifiedWith(const CCurve* pCurve);

	virtual void		SetModifiedCurve(const CCurve* pModifiedCurve);

    virtual bool        GetMovedPoint(double &rAt, C3Point &ptMoved) const;

    virtual bool        UpdateDatabase(sds_name ename) const;

protected:

    virtual void        FindExtendedEndPoint();
};

CLineExtendModifier::CLineExtendModifier() 
{
}

CLineExtendModifier::CLineExtendModifier(const CLine* pLine, const CPlane & projPlane) 
                        : CCurveExtendModifier(pLine, projPlane)
{
    SetModifiedCurve(pLine);
}

CLineExtendModifier::~CLineExtendModifier() 
{
}

bool CLineExtendModifier::ModifiedWith(const CCurve* pCurve)
{
    if (m_pModifiedCurve->Type() == TYPE_UnboundedLine)
        return false;

    return CCurveExtendModifier::ModifiedWith(pCurve);
}

void CLineExtendModifier::SetModifiedCurve(const CCurve* pModifiedCurve)
{
    CUnboundedLine* pUnboundLine = NULL;
    if (pModifiedCurve->Type() == TYPE_Line) {
        CLine* pLine = (CLine*)pModifiedCurve;
        ASSERT(pLine != NULL);
        pUnboundLine = new CUnboundedLine(pLine->Origin(), pLine->Direction());
    }
    else if (pModifiedCurve->Type() == TYPE_Ray) {
        CRay* pRay = (CRay*)pModifiedCurve;
        ASSERT(pRay != NULL);
        pUnboundLine = new CUnboundedLine(pRay->Origin(), pRay->Direction());
    }
    else {
        CUnboundedLine* pUnboundedLine = (CUnboundedLine*)pModifiedCurve;
        ASSERT(pUnboundedLine != NULL);
        pUnboundLine = new CUnboundedLine(pUnboundedLine->Origin(), pUnboundedLine->Direction());
    }

    pUnboundLine->SetDomain(-INFINITY, INFINITY);
    m_pCurve = pUnboundLine;
}

bool CLineExtendModifier::GetMovedPoint(double &rTo, C3Point &ptTarget) const
{
    if (m_pModifiedCurve->Type() == TYPE_UnboundedLine) 
        return false;
    else if (m_pModifiedCurve->Type() == TYPE_Ray) {
        rTo = 0.0;
        ptTarget = ((CUnboundedLine*)m_pModifiedCurve)->Origin();
        return true;
    }
    else
        return CCurveExtendModifier::GetMovedPoint(rTo, ptTarget);
}


void CLineExtendModifier::FindExtendedEndPoint()
{
    if (m_pModifiedCurve->Type() == TYPE_Ray)
        m_MovedPointType = StartPoint;
    else
        CCurveExtendModifier::FindExtendedEndPoint();
}

bool CLineExtendModifier::UpdateDatabase(sds_name ename) const
{
    bool bSuccess = false;
    struct resbuf* pEntList = sds_entget(ename);
    if (!pEntList)
        return bSuccess;
                
    double rTarget;
    C3Point ptTarget;
    if (GetTargetPoint(rTarget, ptTarget)) {
        if (MovedPointType() == CCurveExtendModifier::StartPoint) {
		    ic_assoc(pEntList,10);
	        ic_rbassoc->resval.rpoint[0] = ptTarget.X();
		    ic_rbassoc->resval.rpoint[1] = ptTarget.Y();
		    ic_rbassoc->resval.rpoint[2] = ptTarget.Z();
        }
        else {
			ic_assoc(pEntList,11);
		    ic_rbassoc->resval.rpoint[0] = ptTarget.X();
			ic_rbassoc->resval.rpoint[1] = ptTarget.Y();
			ic_rbassoc->resval.rpoint[2] = ptTarget.Z();
        }
	    if (sds_entmod(pEntList) == RTNORM) 
            bSuccess = true;
    }
    return bSuccess;
}


/***********************************************************************************
 * 
 * This class implements projected extending operation for elliptic arc..
 *
 * Author:  Stephen W. Hou
 * Date:    July 22, 2004
 *
 ***********************************************************************************/
class CEllipArcExtendModifier : public CCurveExtendModifier
{
public:

    CEllipArcExtendModifier();
	CEllipArcExtendModifier(const CEllipArc* pEllipArc, 
                           const CPlane & projPlane = *(CPlane*)NULL);
    virtual ~CEllipArcExtendModifier();

	virtual void		SetModifiedCurve(const CCurve* pModifiedCurve);

	virtual bool	    GetTargetPoint(double &rTo, C3Point &ptTarget) const;

    virtual bool        UpdateDatabase(sds_name ename) const;
};

CEllipArcExtendModifier::CEllipArcExtendModifier() 
{
}

CEllipArcExtendModifier::CEllipArcExtendModifier(const CEllipArc* pEllipArc, const CPlane & projPlane) 
                        : CCurveExtendModifier(pEllipArc, projPlane)
{
    SetModifiedCurve(pEllipArc);
}

CEllipArcExtendModifier::~CEllipArcExtendModifier() 
{
}

void CEllipArcExtendModifier::SetModifiedCurve(const CCurve* pModifiedCurve)
{
    CEllipArc* pEllipArc = new CEllipArc(*(CEllipArc*)const_cast<CCurve*>(pModifiedCurve));
    pEllipArc->SetDomain(0.0, TWOPI);
    m_pCurve = pEllipArc;
}


bool CEllipArcExtendModifier::GetTargetPoint(double &rTo, C3Point &ptTarget) const
{
    bool bFound = false;

    double rFrom, rCandidate;
    C3Point ptMoved;
    if (GetMovedPoint(rFrom, ptMoved)) {
        double disToTheCloestPoint = INFINITY;
        if (MovedPointType() == CCurveExtendModifier::StartPoint) {
            for (register i = 0; i < m_CandidatePoints.Size(); i++) {
                rCandidate = m_CandidatePoints[i];

                double dis;
                if (rCandidate < m_CurveBound[0])
                    dis = m_CurveBound[0] - rCandidate;
                else
                    dis = m_CurveBound[0] + TWOPI - rCandidate;

                if (dis <= disToTheCloestPoint) {
                    disToTheCloestPoint = dis;
                    rTo = rCandidate;
                    bFound = true;
                }
                else
                    break;
            }
        }
        else {
            for (register i = 0; i < m_CandidatePoints.Size(); i++) {
                rCandidate = m_CandidatePoints[i];

                double dis;
                if (rCandidate > m_CurveBound[1])
                    dis = rCandidate - m_CurveBound[1];
                else
                    dis = TWOPI - m_CurveBound[1] + rCandidate;

                if (dis <= disToTheCloestPoint) {
                    disToTheCloestPoint = dis;
                    rTo = rCandidate;
                    bFound = true;
                }
                else
                    break;
            }
        }
    }
    if (bFound) {
        // Since the location to be extended to is outside the bound of the 
        // original curve, we need to use new unbounded curve to evaluate 
        // the point.
        //
        m_pCurve->Evaluate(rTo, ptTarget);
    }

    return bFound;
}

bool CEllipArcExtendModifier::UpdateDatabase(sds_name ename) const
{
    struct resbuf* pEntList = sds_entget(ename);
    if (!pEntList)
        return false;

    bool bSuccess = false;
    double rTarget;
    C3Point ptTarget;
    if (GetTargetPoint(rTarget, ptTarget)) {
        if (MovedPointType() == CCurveExtendModifier::StartPoint) {
		    ic_assoc(pEntList, 41);
		    ic_rbassoc->resval.rreal = rTarget;
        }
        else {
	        ic_assoc(pEntList, 42);
		    ic_rbassoc->resval.rreal = rTarget;
        }
	    if (sds_entmod(pEntList) == RTNORM) 
            bSuccess = true;

    }
    return bSuccess;
}


/***********************************************************************************
 * 
 * This class implements projected extending operation for circular arc.
 *
 * Author:  Stephen W. Hou
 * Date:    July 27, 2004
 *
 ***********************************************************************************/
class CCircArcExtendModifier : public CEllipArcExtendModifier
{
public:

    CCircArcExtendModifier();
	CCircArcExtendModifier(const CEllipArc* pCircularArc, 
                           const CPlane & projPlane = *(CPlane*)NULL);
    virtual ~CCircArcExtendModifier();

    virtual bool        UpdateDatabase(sds_name ename) const;
};


CCircArcExtendModifier::CCircArcExtendModifier()
{
}

CCircArcExtendModifier::CCircArcExtendModifier(const CEllipArc* pCircularArc, 
                                               const CPlane & projPlane)
                       : CEllipArcExtendModifier(pCircularArc, projPlane)
{
}

CCircArcExtendModifier::~CCircArcExtendModifier()
{
}

bool CCircArcExtendModifier::UpdateDatabase(sds_name ename) const
{
    struct resbuf* pEntList = sds_entget(ename);
    if (!pEntList)
        return false;

    bool bSuccess = false;

    double rTarget;
    C3Point ptTarget;
    if (GetTargetPoint(rTarget, ptTarget)) {
        if (MovedPointType() == CCurveExtendModifier::StartPoint) {
			ic_assoc(pEntList,50);
			ic_rbassoc->resval.rreal = rTarget;
        }
        else {
			ic_assoc(pEntList,51);
			ic_rbassoc->resval.rreal = rTarget;
        }
		if (sds_entmod(pEntList) == RTNORM) 
            bSuccess = true;
    }
    return bSuccess;
}


/***********************************************************************************
 * 
 * This class implements projected extending operation for polylines.
 *
 * Author:  Stephen W. Hou
 * Date:    July 22, 2004
 *
 ***********************************************************************************/
class CPolylineExtendModifier : public CCurveExtendModifier
{
public:

    CPolylineExtendModifier();
	CPolylineExtendModifier(const CCompositeCurve* pPolyline, 
                            const CPlane & projPlane = *(CPlane*)NULL);
    virtual ~CPolylineExtendModifier();

	virtual bool	    GetTargetPoint(double &rTo, C3Point &ptTarget) const;

    virtual bool        UpdateDatabase(sds_name ename) const;

protected:

    virtual bool        BeforeComputeModification();
};


CPolylineExtendModifier::CPolylineExtendModifier()
{
}


CPolylineExtendModifier::CPolylineExtendModifier(const CCompositeCurve* pPolyline, 
                                                 const CPlane & projPlane)
                        : CCurveExtendModifier(pPolyline, projPlane)
{
}

CPolylineExtendModifier::~CPolylineExtendModifier()
{
}

bool CPolylineExtendModifier::GetTargetPoint(double &rTo, C3Point &ptTarget) const
{
    if (m_pCurve->Type() == TYPE_UnboundedLine)
        return CCurveExtendModifier::GetTargetPoint(rTo, ptTarget);
    else {
        bool bFound = false;

        double rFrom, rCandidate;
        C3Point ptMoved;
        if (GetMovedPoint(rFrom, ptMoved)) {
            double disToTheCloestPoint = INFINITY;
            if (MovedPointType() == CCurveExtendModifier::StartPoint) {
                for (register i = 0; i < m_CandidatePoints.Size(); i++) {
                    rCandidate = m_CandidatePoints[i];

                    double dis;
                    if (rCandidate < m_CurveBound[0])
                        dis = m_CurveBound[0] - rCandidate;
                    else
                        dis = m_CurveBound[0] + TWOPI - rCandidate;

                    if (dis <= disToTheCloestPoint) {
                        disToTheCloestPoint = dis;
                        rTo = rCandidate;
                        bFound = true;
                    }
                    else
                        break;
                }
            }
            else {
                for (register i = 0; i < m_CandidatePoints.Size(); i++) {
                    rCandidate = m_CandidatePoints[i];

                    double dis;
                    if (rCandidate > m_CurveBound[1])
                        dis = rCandidate - m_CurveBound[1];
                    else
                        dis = TWOPI - m_CurveBound[1] + rCandidate;

                    if (dis <= disToTheCloestPoint) {
                        disToTheCloestPoint = dis;
                        rTo = rCandidate;
                        bFound = true;
                    }
                    else
                        break;
                }
            }
        }
        if (bFound) {
            // Since the location to be extended to is outside the bound of the 
            // original curve, we need to use new unbounded curve to evaluate 
            // the point.
            //
            m_pCurve->Evaluate(rTo, ptTarget);
        }

        return bFound;
    }
}

bool CPolylineExtendModifier::BeforeComputeModification()
{
    if (!m_Swapped && m_pModifiedCurve) {
        CCompositeCurve* pCompositeCurve = static_cast<CCompositeCurve*>(m_pModifiedCurve);
        if (!pCompositeCurve)
            return false;

        int pickedSegmentIndex = pCompositeCurve->FindSegment(m_RefPoint);
        int numSegments = pCompositeCurve->NumOfSegments();

        int modifiedSegmentIndex;
        if (numSegments > 1) {
            if (pickedSegmentIndex <= numSegments / 2 - 1) {
                m_MovedPointType = CCurveExtendModifier::StartPoint;
                modifiedSegmentIndex = 0;
            }
            else {
                m_MovedPointType = CCurveExtendModifier::EndPoint;
                modifiedSegmentIndex = numSegments - 1;
            }
        }
        else {
            CCurveExtendModifier::FindExtendedEndPoint();
            modifiedSegmentIndex = 0;
        }

        const CCurve* pCurve = pCompositeCurve->Segment(modifiedSegmentIndex);
        m_CurveBound[0] = pCurve->Start();
        m_CurveBound[1] = pCurve->End();

        if (m_pCurve)
            delete m_pCurve;

        m_pCurve = m_pModifiedCurve;
        if (pCurve->Type() == TYPE_Line) {
            m_pModifiedCurve = new CUnboundedLine((PLine)pCurve);
            m_pModifiedCurve->SetDomain(-INFINITY, INFINITY);
        }
        else {
            m_pModifiedCurve = pCurve->Clone();
            m_pModifiedCurve->SetDomain(0.0, TWOPI);
        }

        m_Swapped = true;
        return true;
    }
    else
        return false;
}


bool CPolylineExtendModifier::UpdateDatabase(sds_name ename) const
{
    bool bSuccess = false;

    C3Point ptTarget;
    double rTarget;
    if (GetTargetPoint(rTarget, ptTarget)) {
        CCompositeCurve* pPolyline = static_cast<CCompositeCurve*>(m_pModifiedCurve);
        ASSERT(pPolyline);

        sds_name vname;
        sds_entnext_noxref(ename, vname);

        double sWidth, eWidth, bulge;
        if (MovedPointType() == CCurveExtendModifier::StartPoint) { // update the first point
            const CCurve* pSegment = pPolyline->Segment(0);
            double start = pSegment->Start();
            double end = pSegment->End();

            C3Point ptStart, ptEnd;
            pSegment->GetStartPoint(ptStart);
            pSegment->GetEndPoint(ptEnd);

            struct resbuf* pRbVertex = sds_entget(vname);
			if (pRbVertex == NULL) 
				return bSuccess;

	        for (struct resbuf* pRb = pRbVertex; pRb != NULL; pRb = pRb->rbnext) {
                if (pRb->restype == 42) {
				    bulge = pRb->resval.rreal;  // bulge
                    if (!icadIsZero(bulge)) {
                        if (bulge > 0.0) {      //ccw bulge
                            pRb->resval.rreal = tan((end - rTarget) / 4.0);
                        }
                        else {
                            pRb->resval.rreal = tan((rTarget - end) / 4.0);
                            // bulge = tan((start - rTarget) / 4.0);
                        }
                    }
                }
                else if (pRb->restype == 10) {
                    pRb->resval.rpoint[0] = ptTarget.X();
                    pRb->resval.rpoint[1] = ptTarget.Y();
                    pRb->resval.rpoint[2] = ptTarget.Z();
                }
		        else if (pRb->restype == 41)    
					eWidth = pRb->resval.rreal;    // end width          
				else if (pRb->restype == 40)
					sWidth = pRb->resval.rreal;    // start width
			}
 
            double newStartWidth, widthDiff1, widthDiff2;
			if (icadRealEqual(bulge, 0.0))
				newStartWidth = max(0.0, (eWidth + (sWidth - eWidth) * ptTarget.DistanceTo(ptEnd) / ptEnd.DistanceTo(ptStart)));
			else if (bulge > 0.0) {
//			    ic_normang(&rTarget, &fr6);
			    widthDiff1 = end - rTarget;
//			    ic_normang(&fr5, &fr6);
				widthDiff2 = end - start;
				newStartWidth = max(0.0,(eWidth + (sWidth - eWidth) * widthDiff1 / widthDiff2));
		    }
		    else {
//				ic_normang(&fr5, &rTarget);
				widthDiff1 = rTarget - start;
//				ic_normang(&fr5,&fr6);
				widthDiff2 = end - start;
				newStartWidth = max(0.0,(eWidth + (sWidth - eWidth) * widthDiff1 / widthDiff2));
	        }
			ic_assoc(pRbVertex,40);
			ic_rbassoc->resval.rreal = newStartWidth;
			if (sds_entmod(pRbVertex) == RTNORM) 
                bSuccess = true;

            FreeResbufIfNotNull(&pRbVertex);
        }
        else {  // update the end point
            const CCurve* pSegment = pPolyline->Segment(pPolyline->NumOfSegments() - 1);
            double start = pSegment->Start();
            double end = pSegment->End();

            C3Point ptStart, ptEnd;
            pSegment->GetStartPoint(ptStart);
            pSegment->GetEndPoint(ptEnd);

            sds_name vname1, vname2;
		    ic_encpy(vname1, vname);
            struct resbuf* pRb = NULL;
		    for (register i = 1; ; i++) {
                if (sds_entnext_noxref(vname1, vname2) != RTNORM) 
                    return false;
                
                if ((pRb = sds_entget(vname2)) == NULL)
                    return false;

			    ic_assoc(pRb, 0);
			    if (strsame(ic_rbassoc->resval.rstring,"SEQEND"))
				    break;

			    FreeResbufIfNotNull(&pRb);

			    ic_encpy(vname, vname1);
			    ic_encpy(vname1,vname2);
		    }
		    FreeResbufIfNotNull(&pRb);

		    if ((pRb = sds_entget(vname)) == NULL) 
                return false;

		    ic_assoc(pRb, 42);                          // bulge
			double bulge = ic_rbassoc->resval.rreal;
            if (!icadIsZero(bulge)) {
                if (bulge > 0.0) {  //ccw bulge
//					ic_normang(&start, &rTarget);
					pRb->resval.rreal = tan((rTarget - start) / 4.0);
				}
				else {
//					ic_normang(&rTarget, &end);
                    pRb->resval.rreal = tan((start - rTarget) / 4.0);
					//pRb->resval.rreal = tan((rTarget - end) / 4.0);
				}
            }

			ic_assoc(pRb,40);                           // start width
			double sWidth = ic_rbassoc->resval.rreal;
			ic_assoc(pRb,41);                           // end width
			double eWidth = ic_rbassoc->resval.rreal;

            double newEndWidth, widthDiff1, widthDiff2;
			if (icadRealEqual(bulge, 0.0))
				newEndWidth = max(0.0, (sWidth + (eWidth - sWidth) * ptStart.DistanceTo(ptTarget) / ptStart.DistanceTo(ptEnd)));
			else if (bulge > 0.0) {
				ic_normang(&start, &rTarget);
				widthDiff1 = rTarget - start;
//				ic_normang(&start,&end);
				widthDiff2 = end - start;
				newEndWidth = max(0.0,(sWidth + (eWidth - sWidth) * widthDiff1 / widthDiff2));
	        }
		    else {
//				ic_normang(&rTarget, &end);
				widthDiff1 = end - rTarget;
//				ic_normang(&start, &end);
				widthDiff2 = end - start;
				newEndWidth = max(0.0,(sWidth + (eWidth - sWidth) * widthDiff2 / widthDiff1));
			}
			ic_rbassoc->resval.rreal = newEndWidth;
            if (sds_entmod(pRb) != RTNORM) {
			    FreeResbufIfNotNull(&pRb);
                return false;
            }
			FreeResbufIfNotNull(&pRb);

			if ((pRb = sds_entget(vname1)) == NULL) 
                return false;

			ic_assoc(pRb,10);
            ic_rbassoc->resval.rpoint[0] = ptTarget.X();
            ic_rbassoc->resval.rpoint[1] = ptTarget.Y();
            ic_rbassoc->resval.rpoint[2] = ptTarget.Z();

			if (sds_entmod(pRb) == RTNORM) 
                bSuccess = true;

            FreeResbufIfNotNull(&pRb);
        }
    }
    return bSuccess;
}

/***********************************************************************************
 * 
 * This class implements projected extending operation for light weight polylines.
 *
 * Author:  Stephen W. Hou
 * Date:    July 27, 2004
 *
 ***********************************************************************************/
class CLWPolylineExtendModifier : public CPolylineExtendModifier
{
public:

    CLWPolylineExtendModifier();
	CLWPolylineExtendModifier(const CCompositeCurve* pPolyline, 
                            const CPlane & projPlane = *(CPlane*)NULL);
    virtual ~CLWPolylineExtendModifier();

    virtual bool        UpdateDatabase(sds_name ename) const;
};


CLWPolylineExtendModifier::CLWPolylineExtendModifier()
{
}


CLWPolylineExtendModifier::CLWPolylineExtendModifier(const CCompositeCurve* pLWPolyline, 
                                                     const CPlane & projPlane)
                        : CPolylineExtendModifier(pLWPolyline, projPlane)
{
}

CLWPolylineExtendModifier::~CLWPolylineExtendModifier()
{
}

bool CLWPolylineExtendModifier::UpdateDatabase(sds_name ename) const
{
    struct resbuf* pEntList = sds_entget(ename);    
    if (!pEntList)
        return false;

    bool bSuccess = false;
    struct resbuf *pVertex = NULL;

    C3Point ptTarget;
    double rTarget;
    if (GetTargetPoint(rTarget, ptTarget)) {
        CCompositeCurve* pPolyline = static_cast<CCompositeCurve*>(m_pModifiedCurve);
        ASSERT(pPolyline);
        if (MovedPointType() == CCurveExtendModifier::StartPoint) { // update the first point

            const CCurve* pSegment = pPolyline->Segment(0);
            double start = pSegment->Start();
            double end = pSegment->End();

            C3Point ptStart, ptEnd;
            pSegment->GetStartPoint(ptStart);
            pSegment->GetEndPoint(ptEnd);

			// go to the first vertex
			pVertex = pEntList;
			while (pVertex->restype != 10)
				pVertex = pVertex->rbnext;
                
            pVertex->resval.rpoint[0] = ptTarget.X();
            pVertex->resval.rpoint[1] = ptTarget.Y();
            pVertex->resval.rpoint[2] = ptTarget.Z();

			pVertex = pVertex->rbnext;			            // step to 40 group
			struct resbuf *pStartWidth = pVertex;		    // hold this pointer
			double sWidth = pVertex->resval.rreal;

			pVertex = pVertex->rbnext;			            // step to 41 group
			double eWidth = pVertex->resval.rreal;

			pVertex = pVertex->rbnext;
			ASSERT(pVertex->restype == 42);
			double bulge = pVertex->resval.rreal;		    // set new bulge;
            if (!icadIsZero(bulge)) {
                if (bulge > 0.0)       //ccw bulge
                    pVertex->resval.rreal = tan((end - rTarget) / 4.0);
                else 
                    pVertex->resval.rreal = tan((rTarget - end) / 4.0);
            }

            double newStartWidth;
			if (icadIsZero(bulge))
				newStartWidth = max(0.0, (eWidth + (sWidth - eWidth) * ptTarget.DistanceTo(ptEnd) / ptEnd.DistanceTo(ptStart)));
			else if (bulge > 0.0) {
				ic_normang(&rTarget, &end);
				double extSweptAngle = end - rTarget;
				ic_normang(&start, &end);
				double prevSweptAngle = end - start;
				newStartWidth = max(0.0, (eWidth + (sWidth - eWidth) * extSweptAngle / prevSweptAngle));
			}
			else {
				ic_normang(&start, &rTarget);
				double extSweptAngle = rTarget - start;
				ic_normang(&start, &end);
				double prevSweptAngle = end - start;
				newStartWidth = max(0.0,(eWidth + (sWidth - eWidth) * extSweptAngle / prevSweptAngle));
			}
			pStartWidth->resval.rreal = newStartWidth;
        }
        else {  // update the end point

            const CCurve* pFirstSegment = pPolyline->Segment(pPolyline->NumOfSegments() - 1);
            double start = pFirstSegment->Start();
            double end = pFirstSegment->End();

            C3Point ptStart, ptEnd;
            pFirstSegment->GetStartPoint(ptStart);
            pFirstSegment->GetEndPoint(ptEnd);

			pVertex = pEntList;
			while (pVertex->restype != 90)
				 pVertex = pVertex->rbnext;

	        int numVertices = pVertex->resval.rint;
	        for (register i = 0; i < numVertices - 1; i++) {
		        pVertex = pVertex->rbnext;
	            while (pVertex->restype != 10)
				    pVertex = pVertex->rbnext;
            }
            // we are now on the second to last vertex
            //
			pVertex = pVertex->rbnext;	                // start width
			ASSERT(pVertex->restype == 40);
			double sWidth = pVertex->resval.rreal;
			pVertex = pVertex->rbnext;	                // end width
			ASSERT(pVertex->restype == 41);
			double eWidth = pVertex->resval.rreal;
			ASSERT(pVertex->rbnext->restype == 42);
			double bulge = pVertex->rbnext->resval.rreal;  // stay on the 41 group, but set new bulge on 42
            if (!icadIsZero(bulge)) {
                if (bulge > 0.0)       //ccw bulge
                    pVertex->rbnext->resval.rreal = tan((rTarget - start) / 4.0);
                else 
                    pVertex->rbnext->resval.rreal = tan((start - rTarget) / 4.0);
            }

            double newEndWidth, anggleDiff1, angleDiff2;
			if (icadIsZero(bulge))
				newEndWidth = max(0.0,(sWidth + (eWidth - sWidth) * ptTarget.DistanceTo(ptStart) / ptEnd.DistanceTo(ptStart)));
			else if (bulge > 0.0) {
				ic_normang(&start, &rTarget);
				anggleDiff1 = rTarget - start;
				ic_normang(&start, &end);
				angleDiff2 = end - start;
				newEndWidth=max(0.0, (sWidth + (eWidth - sWidth) * anggleDiff1 / angleDiff2));
			}
			else {
				ic_normang(&rTarget, &end);
				anggleDiff1 = end - rTarget;
				ic_normang(&start, &end);
				angleDiff2 = end - start;
				newEndWidth = max(0.0,(sWidth + (eWidth - sWidth) * angleDiff2 / anggleDiff1));
			}
			pVertex->resval.rreal = newEndWidth;        // set new end width
			while (pVertex->restype != 10)			    // step to next (final) vertex
				pVertex = pVertex->rbnext;

            pVertex->resval.rpoint[0] = ptTarget.X();
            pVertex->resval.rpoint[1] = ptTarget.Y();
            pVertex->resval.rpoint[2] = ptTarget.Z();
         }
         if (sds_entmod(pEntList) == RTNORM) 
             bSuccess = true;
    }
    return bSuccess;
}


static double CmdGetHitTolerance(gr_view *view)
{
	resbuf rb;
	// Let's get a window for crossing
	if (SDS_getvar( NULL, DB_QVIEWSIZE, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES ) != RTNORM)
		return HIT_TOLERANCE;
	sds_real aperture = rb.resval.rreal;

	if ((SDS_getvar( NULL, DB_QSCREENSIZE, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES ) != RTNORM)  ||
		icadRealEqual(rb.resval.rpoint[1],0.00000))
		return HIT_TOLERANCE;
	aperture /= rb.resval.rpoint[1];

	if ((SDS_getvar(NULL,DB_QAPERTURE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES) != RTNORM ) || rb.resval.rint == 0 )
		return HIT_TOLERANCE;
	aperture *= rb.resval.rint;

	sds_point llc, urc;
	llc[0] = llc[1] = - aperture;
	urc[0] = urc[1] = aperture;
	llc[2] = urc[2] = 0.0;

	// translate to UCS
	gr_rp2ucs(llc, llc, view);
	gr_rp2ucs(urc, urc, view);
	return (sds_dist2d(llc, urc) / 2.0);
}


//*********************BEGIN SUPPORT FUNCTIONS **********************************

int cmd_extend_object(sds_name ename, sds_point epoint, sds_name ss)
{
	//NOTE: epoint is UCS (passed from entsel)
	struct resbuf rb, rbent,rbucs, rbwcs;
	sds_point p0,/*p1,p2,p3,p4,*rayvect,extpt,*/ viewdir;
	int ret = 0/*,fi1*/;
	struct gr_view *view=SDS_CURGRVW;
		
    SDS_getvar(NULL, DB_QEDGEMODE, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
	bool bEdgeMode = rb.resval.rint > 0 ? true : false;

	double HitTolerance = CmdGetHitTolerance(view);
	rbwcs.restype = RTSHORT;
	rbwcs.resval.rint = 0;

	rbucs.restype = RTSHORT;
	rbucs.resval.rint = 1;

    C3Point ucsOrigin;
	SDS_getvar(NULL,DB_QUCSORG,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
    ucsOrigin.SetX(rb.resval.rpoint[0]);
    ucsOrigin.SetY(rb.resval.rpoint[1]);
    ucsOrigin.SetZ(rb.resval.rpoint[2]);

	SDS_getvar(NULL,DB_QUCSXDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

    C3Vector ucsXDir, ucsYDir, ucsZDir;
	ic_ptcpy(p0,rb.resval.rpoint);
    ucsXDir.Set(rb.resval.rpoint[0], rb.resval.rpoint[1], rb.resval.rpoint[2]);

	SDS_getvar(NULL,DB_QUCSYDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
    ucsYDir.Set(rb.resval.rpoint[0], rb.resval.rpoint[1], rb.resval.rpoint[2]);
    bool bUcsMatchWcs = cmd_ucs_match_wcs(ucsOrigin, ucsXDir, ucsYDir);

	ic_crossproduct(p0,rb.resval.rpoint,rbent.resval.rpoint);
    ic_unitize(rbent.resval.rpoint);
	rbent.restype=RT3DPOINT;

    ucsZDir.Set(rbent.resval.rpoint[0], rbent.resval.rpoint[1], rbent.resval.rpoint[2]);

    CAffine w2uXForm, u2wXForm(&ucsOrigin, &ucsXDir.AsPoint(), &ucsYDir.AsPoint(), &ucsZDir.AsPoint());
    w2uXForm.SetInverse(u2wXForm);

    SDS_getvar(NULL,DB_QVIEWDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	ic_ptcpy(viewdir,rb.resval.rpoint);

	SDS_getvar(NULL,DB_QPROJMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	int projmode=rb.resval.rint;    // 0 - NONE, 1 - UCS, 2 - VIEW

    ic_unitize(viewdir);
    C3Vector ucsViewDir(viewdir[0], viewdir[1], viewdir[2]);

    sds_trans(viewdir, &rbucs, &rbwcs, 1, viewdir);
    C3Vector wcsViewDir(viewdir[0], viewdir[1], viewdir[2]);

	SDS_getvar(NULL,DB_QPROJMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	projmode=rb.resval.rint;    // 0 - NONE, 1 - UCS, 2 - VIEW

    CPlane plane;
    C3Point origin;
	if (projmode == 1) {
        CPlane projPlane(ucsOrigin, ucsZDir, ucsXDir);
        plane = projPlane;
	}
	else if (projmode == 2) {
		SDS_getvar(NULL,DB_QVIEWCTR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

        origin.SetX(rb.resval.rpoint[0]);
        origin.SetY(rb.resval.rpoint[1]);
        origin.SetZ(rb.resval.rpoint[2]);

        CPlane projPlane(origin, wcsViewDir);
        plane = projPlane;
	}

    CDWGToGeo toGeo(false);
	stdPPathList pathList;
	db_handitem* pEntity;
    sds_name entityName;
	long ssLength;
	sds_sslength(ss, &ssLength);
	// extract entities from selection set into Geo objects
	for (int i = 0; i < ssLength; ++i) {
	    sds_ssname(ss, i, entityName);
		pEntity = (db_handitem*)entityName[0];
	    toGeo.ExtractEntity(pEntity, pathList, bEdgeMode, true);
	}

    pEntity = (db_entity*)ename[0];
    if (!pEntity)
        return (-__LINE__);

    stdPPathList::const_iterator it;

	CCurve* pCurve = NULL;
    CCurveExtendModifier* pModifier = NULL;
	if (pEntity->ret_type() == DB_LINE || pEntity->ret_type() == DB_RAY) {
		CLine *pLine = (pEntity->ret_type() == DB_LINE) ? asGeoLine((db_line*)ename[0]) : asGeoRay((db_ray*)ename[0]);
        if (!pLine)
            return (-__LINE__);

        pModifier = new CLineExtendModifier(pLine, (projmode) ? plane : *(CPlane*)NULL);
        pCurve = pLine;
    }
    else if (pEntity->ret_type() == DB_ARC) {
		CEllipArc* pArc = asGeoEllipArc((db_arc*)ename[0]);
        if (!pArc)
            return (-__LINE__);

        pModifier = new CCircArcExtendModifier(pArc, (projmode) ? plane : *(CPlane*)NULL);
        pCurve = pArc;
    }
    else if (pEntity->ret_type() == DB_POLYLINE) {
		CCompositeCurve* pPolyline = asGeoCompositeCurve((db_polyline *)ename[0]);
        if (!pPolyline)
            return (-__LINE__);

        pModifier = new CPolylineExtendModifier(pPolyline, (projmode) ? plane : *(CPlane*)NULL);
        pCurve = pPolyline;
    }
    else if (pEntity->ret_type() == DB_LWPOLYLINE) {
		CCompositeCurve* pLWPolyline = asGeoCompositeCurve((db_lwpolyline *)ename[0]);
        if (!pLWPolyline)
            return (-__LINE__);

        pModifier = new CLWPolylineExtendModifier(pLWPolyline, (projmode) ? plane : *(CPlane*)NULL);
        pCurve = pLWPolyline;
    }
    else if (pEntity->ret_type() == DB_ELLIPSE) {
		CEllipArc* pEllipArc = asGeoEllipArc((db_ellipse*)ename[0]);
        if (!pEllipArc)
            return (-__LINE__);

        pModifier = new CEllipArcExtendModifier(pEllipArc, (projmode) ? plane : *(CPlane*)NULL);
        pCurve = pEllipArc;
    }
    if (pModifier) {
        pCurve->Transform(w2uXForm);

        double pointParam;
		C3Point pickPoint(epoint[0], epoint[1], epoint[2]);
        if (pCurve->HitTest(pickPoint, ucsViewDir, HitTolerance, pointParam)) {
            pCurve->Transform(u2wXForm);

            pModifier->SetRefPoint(pointParam);

		    // find intersections of selection set with elliptical arc
            bool bModified = false;
            for (it = pathList.begin(); it != pathList.end(); ++it) {
                for (register j = 0; j < (*it)->Size(); ++j) {
                    if (pModifier->ModifiedWith((*it)->Curve(j)) && !bModified)
                        bModified = true;
                }
            }

	        for (it = pathList.begin(); it != pathList.end(); ++it)
		        delete (*it);

            if (bModified) { 
                if (pModifier->UpdateDatabase(ename))
				    ret=(-__LINE__);
            }

		    sds_entupd(ename);
        }
        delete pModifier;
        delete pCurve;
	}
	else
		cmd_ErrorPrompt(CMD_ERR_EXTENDENT,0);

	if (ret==0)
		return(RTNORM);
	if (ret==-1)
		return(RTCAN);
	if (ret < -1)
		CMD_INTERNAL_MSG(ret);
	return(ret);
}


void cmd_adj_linextpt(sds_point startpt, sds_point endpt, sds_point end2use, sds_point chkpt, sds_point extpt, sds_real *extdist){

	//we have to always make sure chkpt is in the proper direction and that it's not right at the
	//end of the line from which we're extending.

	if (icadRealEqual(*extdist,0.0)){							 //set the dist var & point
		if ((icadPointEqual(startpt,end2use))&&
				(icadDirectionEqual(endpt,startpt,endpt,chkpt))&&
				(sds_distance(end2use,chkpt)>0.0)){
			ic_ptcpy(extpt,chkpt);
			*extdist=sds_distance(end2use,chkpt);
		}
		if ((icadPointEqual(endpt,end2use))&&
				(icadDirectionEqual(startpt,endpt,startpt,chkpt))&&
				(sds_distance(end2use,chkpt)>0.0)){
			ic_ptcpy(extpt,chkpt);
			*extdist=sds_distance(end2use,chkpt);
		}
	}else{										 //check the dist var & point
		if ((icadPointEqual(startpt,end2use))&&
			(icadDirectionEqual(endpt,startpt,endpt,chkpt))&&
			(sds_distance(end2use,chkpt)<*extdist)&&
			(sds_distance(end2use,chkpt)> 0.0)){
				ic_ptcpy(extpt,chkpt);
				*extdist=sds_distance(end2use,chkpt);
		}
		if ((icadPointEqual(endpt,end2use))&&
			(icadDirectionEqual(startpt,endpt,startpt,chkpt))&&
			(sds_distance(end2use,chkpt)<*extdist)&&
			(sds_distance(end2use,chkpt)> 0.0)){
				ic_ptcpy(extpt,chkpt);
				*extdist=sds_distance(end2use,chkpt);
		}
	}
}


int cmd_trim_object(sds_name ename, sds_point epoint, cmd_elistll *bdry_setll, sds_name xtraent,long trimsetlen)
	{

	//given entity ename selected at epoint and selection set for trimming converted
	//to a cmd_elistll *, f'n calculates
	//NOTE: all pts are in native coords, and epoint is in UCS coords (from entsel)
	//points to use for trim.  ss1 should be lines, plines, arcs, circles, & text
	//returns -1 if no trim pts found, 0 if trim pts found.
	//NOTE: If trim chops ent in half (center trim), then a 2nd entity will be
	//		created.  The name of this ent will be placed in xtraent if this
	//		happens, otherwise xtraent[0]&[1] will be set to 0;

	//NOTE:  Caller responsible for freeing bdry_setll

	struct resbuf *rbp1, *rbp2, *elist, rb,*rbptemp,rbent,rbwcs,rbucs;
	sds_name e1,e2;
	sds_real trimang1=0.0, trimang2=0.0, fr1, fr2, fr3, fr4, fr5, fr6, fr7, fr8;
	sds_point trimpt1,trimpt2,plstart,plend,p0,p1,p2,p3,p4,p5,p6,p7,p8, viewdir;
	int ret=0,trimret,trimends=0,notinucs=0;
	int fi1, fi2, fi3, edgemode, projmode, closure, closure2, entdel_flag=0,xline_ptflag;
	long fl1,fl2,fl3;
	char etype[20];
	cmd_elistll *elistll,*elistll2,*elistll3;
	int exactvert;
	int trimeeType;

	double HitTolerance = CmdGetHitTolerance(SDS_CURGRVW);

	elist=rbp1=rbp2=rbptemp=NULL;
	elistll=elistll2=elistll3=NULL;

    //set up ucs's for transforming points
	rbucs.restype=rbwcs.restype=RTSHORT;
	rbucs.resval.rint=1; rbwcs.resval.rint=0;
    rbucs.rbnext=rbwcs.rbnext=rbent.rbnext=NULL;

    C3Point ucsOrigin;
	SDS_getvar(NULL,DB_QUCSORG,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
    ucsOrigin.SetX(rb.resval.rpoint[0]);
    ucsOrigin.SetY(rb.resval.rpoint[1]);
    ucsOrigin.SetZ(rb.resval.rpoint[2]);


	SDS_getvar(NULL,DB_QUCSXDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

    C3Vector ucsXDir, ucsYDir, ucsZDir;
	ic_ptcpy(p0,rb.resval.rpoint);
    ucsXDir.Set(rb.resval.rpoint[0], rb.resval.rpoint[1], rb.resval.rpoint[2]);

	SDS_getvar(NULL,DB_QUCSYDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
    ucsYDir.Set(rb.resval.rpoint[0], rb.resval.rpoint[1], rb.resval.rpoint[2]);
    bool bUcsMatchWcs = cmd_ucs_match_wcs(ucsOrigin, ucsXDir, ucsYDir);

	ic_crossproduct(p0,rb.resval.rpoint,rbent.resval.rpoint);
    ic_unitize(rbent.resval.rpoint);
	rbent.restype=RT3DPOINT;

    ucsZDir.Set(rbent.resval.rpoint[0], rbent.resval.rpoint[1], rbent.resval.rpoint[2]);

    CAffine w2uXForm, u2wXForm(&ucsOrigin, &ucsXDir.AsPoint(), &ucsYDir.AsPoint(), &ucsZDir.AsPoint());
    w2uXForm.SetInverse(u2wXForm);

    SDS_getvar(NULL,DB_QVIEWDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	ic_ptcpy(viewdir,rb.resval.rpoint);

	SDS_getvar(NULL,DB_QEDGEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	edgemode=rb.resval.rint;
	SDS_getvar(NULL,DB_QPROJMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	projmode=rb.resval.rint;    // 0 - NONE, 1 - UCS, 2 - VIEW
	rb.rbnext=NULL;
	rb.restype=RTSTR;
	rb.resval.rstring="*"/*DNT*/;

    ic_unitize(viewdir);
    C3Vector ucsViewDir(viewdir[0], viewdir[1], viewdir[2]);

    sds_trans(viewdir, &rbucs, &rbwcs, 1, viewdir);
    C3Vector wcsViewDir(viewdir[0], viewdir[1], viewdir[2]);


    CPlane plane;
    C3Point origin;
	if (projmode == 1) {
        CPlane projPlane(ucsOrigin, ucsZDir, ucsXDir);
        plane = projPlane;
	}
	else if (projmode == 2) {
		SDS_getvar(NULL,DB_QVIEWCTR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

        origin.SetX(rb.resval.rpoint[0]);
        origin.SetY(rb.resval.rpoint[1]);
        origin.SetZ(rb.resval.rpoint[2]);

        CPlane projPlane(origin, wcsViewDir);
        plane = projPlane;
	}

	if ((elist=sds_entgetx(ename,&rb))==NULL)
		return(-1);
	ic_assoc(elist,0);	strncpy(etype,ic_rbassoc->resval.rstring,sizeof(etype)-1);

	trimeeType=IC_TYPE_FROM_ENAME(ename);
	//********************************************************************************
	//**** Trim a Line	**************************************************************
	//********************************************************************************
	if (trimeeType==DB_LINE || trimeeType==DB_RAY || trimeeType==DB_XLINE) {
		ic_assoc(elist,10);
		ic_ptcpy(p1,ic_rbassoc->resval.rpoint);
		ic_assoc(elist,11);
		ic_ptcpy(p2,ic_rbassoc->resval.rpoint);
		//project epoint onto line and call it p0
		if (0==projmode) {
			sds_trans(epoint,&rbucs,&rbwcs,0,epoint);//work in WCS
			cmd_elev_correct(p1[2],epoint,1,NULL);

		    if (trimeeType==DB_LINE) {
			    ic_ptcpy(trimpt1,p1);
			    ic_ptcpy(trimpt2,p2);
			}
		    else if (trimeeType==DB_RAY) {
			    ic_ptcpy(trimpt1,p1);
			    ic_ptcpy(trimpt2,p1);
            }
		    else
			    xline_ptflag=0;

		    if (trimeeType!=DB_LINE) {
			    ic_ptcpy(p8,p2);//p2 already in ucs if projmode!=0
			    p2[0]+=p1[0];
			    p2[1]+=p1[1];
			    p2[2]+=p1[2];
			}
		    ic_ptlndist(epoint,p1,p2,&fr1,p0);
			fr1=sds_dist2d(p1,p2);

			if (fr1>CMD_FUZZ)
                p0[2]+=(p2[2]-p1[2])*sds_dist2d(p1,p0)/fr1;

		    for (elistll=bdry_setll;elistll!=NULL;elistll=elistll->next) {
			    int trimEntType=IC_TYPE_FROM_ENAME(elistll->ename);
			    if (ename[0] == elistll->ename[0])
				    continue;
			    //for (ssct=0L;sds_ssname(ss,ssct,etemp)==RTNORM;ssct++)
			    if (trimEntType==DB_LINE || trimEntType==DB_XLINE || trimEntType==DB_RAY) {
				    ic_assoc(elistll->entlst,10);
				    ic_ptcpy(p3,ic_rbassoc->resval.rpoint);
				    ic_assoc(elistll->entlst,11);
				    ic_ptcpy(p4,ic_rbassoc->resval.rpoint);
				    if (trimEntType!=DB_LINE) {
					    //for ray or xline, convert vector to point
					    p4[0]+=p3[0];
					    p4[1]+=p3[1];
					    p4[2]+=p3[2];
					}

					fi1=sds_inters(p3,p4,p1,p2,0,p5);
					if (fi1!=RTNORM)
                        fi1=-1;
					else {
						fi1=0;
						if (trimeeType!=DB_LINE && 1==ic_ptlndist(p5,p1,p2,NULL,p5))  // changed from trimEntType by rdf 6/24/99
							fi1|=2;
					}
				    if (-1==fi1)
					    continue;

				    if (trimEntType==DB_XLINE)
					    fi1|=1;
				    else if (trimEntType==DB_RAY) {
					    //if int is in correct direction, go ahead and accept it
					    if (icadDirectionEqual(p3,p4,p3,p5))
						    fi1|=1;
					}
                    if (trimeeType!=DB_LINE) { // changed from trimEntType by rdf 6/24/99
					    //if we don't have a line, then assume it hit the selected entity.
					    //	even though it may not have hit the ray.  cmd_adj_ray_trimpt will
					    //	do the checking to assure intersection is on the ray
					    fi1|=2;
					}
				    if (edgemode==0 && fi1!=3)
					    continue;
				    if (edgemode==1 && fi1<=1)
					    continue;
				    if (trimeeType==DB_LINE)
					    cmd_adj_line_trimpt(p0,p5,p1,p2,trimpt1,trimpt2);
				    else if (trimeeType==DB_XLINE) // changed from trimEntType by rdf 6/24/99
					    cmd_adj_xline_trimpt(p0,p5,p1,p8,&xline_ptflag,trimpt1,trimpt2);
				    else	//ray
					    cmd_adj_ray_trimpt(p0,p5,p1,p8,trimpt1,trimpt2);
				}
			    else if (trimEntType==DB_CIRCLE || trimEntType==DB_ARC) {
				    //trim line at circle or arc
				    fr2=0.0;fr3=IC_PI;
				    for (rbptemp=elistll->entlst; rbptemp!=NULL; rbptemp=rbptemp->rbnext) {
					    if (rbptemp->restype==10)
						    ic_ptcpy(p3,rbptemp->resval.rpoint);
					    else if (rbptemp->restype==40)
						    fr1=rbptemp->resval.rreal;
					    else if (rbptemp->restype==50)
						    fr2=rbptemp->resval.rreal;//not for circles
					    else if (rbptemp->restype==51)
						    fr3=rbptemp->resval.rreal;//not for circles
					    else if (rbptemp->restype==210)
						    ic_ptcpy(p4,rbptemp->resval.rpoint);
					}
                    // The following requirement is wrong because it assumes the arc should be 
                    // in lying on the current UCS plane.   SWH - 7/1/2004
                    /*
				    if (!icadPointEqual(rbent.resval.rpoint,p4)) {
					    notinucs++;
					    continue;
					}
                    */
                    // convert the line to the ECS of arc or circle
                    //
                    ic_ptcpy(rbent.resval.rpoint, p4);
					sds_trans(p1,&rbwcs,&rbent,0,p6);
					sds_trans(p2,&rbwcs,&rbent,0,p7);

				    fi1=ic_linexarc(p6,p7,p3,fr1,fr2,fr3,p4,p5);

				    if (fi1==-1) {
					    //check 1st bdry condition - tangency
					    fi2=ic_ptlndist(p3,p6,p7,&fr5,p4);
					    if (!icadRealEqual(fr5,fr1)) // to be tangent, the distance must be the radius of the arc/circle
						    continue;
					    if (fi2==1 || icadPointEqual(p4,p6) || icadPointEqual(p4,p7))
						    fi1=3;
					    else
						    continue;
					}
				    else {
					    //check 2nd boundary condition - endpoint
					    if (5!=(fi1&5) && trimeeType!=DB_LINE) // changed from trimEntType by rdf 6/24/99
						    fi1|=5;
					    if (3!=(fi1&3) && (icadPointEqual(p4,p6) || icadPointEqual(p4,p7)))
						    fi1|=3;
					    if (12!=(fi1&12) && (icadPointEqual(p5,p6) || icadPointEqual(p5,p7)))
						    fi1|=12;
					}
				    //do elevation checking
				    fr5=p6[2]+(p7[2]-p6[2])*sds_dist2d(p6,p4)/sds_dist2d(p6,p7);
				    fr6=p6[2]+(p7[2]-p6[2])*sds_dist2d(p6,p5)/sds_dist2d(p6,p7);

					if (!icadRealEqual(fr5,p3[2]))
						fi1&=~3;
					if (!icadRealEqual(fr6,p3[2]))
						fi1&=~12;

                    // transfomr the intersection points to WCS
                    //
					sds_trans(p4,&rbent,&rbwcs,0,p4);
					sds_trans(p5,&rbent,&rbwcs,0,p5);

				    if (edgemode || (trimEntType==DB_CIRCLE)) {
                        if (fi1 & 1) { //if intersection is on the line
						    //check to see if p4 is closer than one of the trim points
						    if (trimeeType==DB_LINE) // changed from trimEntType by rdf 6/24/99
							    cmd_adj_line_trimpt(p0,p4,p1,p2,trimpt1,trimpt2);
						    else if (trimeeType==DB_XLINE) // changed from trimEntType by rdf 6/24/99
							    cmd_adj_xline_trimpt(p0,p4,p1,p8,&xline_ptflag,trimpt1,trimpt2);
						    else	 //ray
							    cmd_adj_ray_trimpt(p0,p4,p1,p8,trimpt1,trimpt2);
						}
                        if ((fi1 & 4)==4) { //if 2nd intersection is on the line
						    //check to see if p5 is closer than one of the trim points
						    if (trimeeType==DB_LINE) // changed from trimEntType by rdf 6/24/99
							    cmd_adj_line_trimpt(p0,p5,p1,p2,trimpt1,trimpt2);
						    else if (trimeeType==DB_XLINE)
							    cmd_adj_xline_trimpt(p0,p5,p1,p8,&xline_ptflag,trimpt1,trimpt2);
						    else	//ray
							    cmd_adj_ray_trimpt(p0,p5,p1,p8,trimpt1,trimpt2);
						}
					    continue;
					}
                    else {  //an arc and edgemode=0 so we're not extending it into a circle
                        if ((fi1 & 3)== 3) { //if first intersections is on the line
						    //check to see if p4 is closer than one of the trim points
						    if (trimeeType==DB_LINE) // changed from trimEntType by rdf 6/24/99
							    cmd_adj_line_trimpt(p0,p4,p1,p2,trimpt1,trimpt2);
						    else if (trimeeType==DB_XLINE) // changed from trimEntType by rdf 6/24/99
							    cmd_adj_xline_trimpt(p0,p4,p1,p8,&xline_ptflag,trimpt1,trimpt2);
						    else	//ray
							    cmd_adj_ray_trimpt(p0,p4,p1,p8,trimpt1,trimpt2);
						}
                        if ((fi1 & 12)== 12) { //if 2nd intersection is on the line
						    //check to see if p5 is closer than one of the trim points
						    if (trimeeType==DB_LINE) // changed from trimEntType by rdf 6/24/99
							    cmd_adj_line_trimpt(p0,p5,p1,p2,trimpt1,trimpt2);
						    else if (trimeeType==DB_XLINE) // changed from trimEntType by rdf 6/24/99
							    cmd_adj_xline_trimpt(p0,p5,p1,p8,&xline_ptflag,trimpt1,trimpt2);
						    else	//ray
							    cmd_adj_ray_trimpt(p0,p5,p1,p8,trimpt1,trimpt2);
						}
					    continue;
					}
				}
			    else if (trimEntType==DB_ELLIPSE) {
				    CEllipArc* pEllipse = asGeoEllipArc((db_ellipse*)elistll->ename[0]);

				    // To extend to ellipse when edgemode > 0
				    if (!pEllipse->Closed() && edgemode) 
					    pEllipse->SetDomain(0.0, TWOPI);

				    CLine* pLine = NULL;
				    C3Point pt(p1[0], p1[1], p1[2]);
				    if (trimeeType==DB_LINE) 
					    pLine = new CLine(pt, C3Point(p2[0], p2[1], p2[2]));
				    else if (trimeeType==DB_XLINE) {
					    C3Point dir = C3Point(p2[0], p2[1], p2[2]) - pt;
					    dir.Unitize();
					    pLine = new CUnboundedLine(pt, dir);
				    }
				    else { // ray
					    C3Point dir = C3Point(p2[0], p2[1], p2[2]) - pt;
					    dir.Unitize();
					    pLine = new CRay(pt, dir);
					    pLine->SetDomain(0., INFINITY);
				    }

				    // find intersections of selection set with elliptical arc
				    CRealArray onLine;
				    CRealArray onEllipse;
				    Intersect(pLine, pEllipse, onLine, onEllipse, INT_EPSILON);

				    if (onLine.Size() > 0) {
					    C3Point xPoint;
					    pLine->Evaluate(onLine[0], xPoint);
					    p4[0] = xPoint.X(); p4[1] = xPoint.Y(); p4[2] = xPoint.Z();

					    //check to see if p4 is closer than one of the trim points
					    if (trimeeType==DB_LINE) 
						    cmd_adj_line_trimpt(p0,p4,p1,p2,trimpt1,trimpt2);
					    else if (trimeeType==DB_XLINE) 
						    cmd_adj_xline_trimpt(p0,p4,p1,p8,&xline_ptflag,trimpt1,trimpt2);
					    else	 //ray
						    cmd_adj_ray_trimpt(p0,p4,p1,p8,trimpt1,trimpt2);

					    if (onLine.Size() > 1) { // the second intersection

						    pLine->Evaluate(onLine[1], xPoint);
						    p5[0] = xPoint.X(); p5[1] = xPoint.Y(); p5[2] = xPoint.Z();

						    //check to see if p5 is closer than one of the trim points
						    if (trimeeType==DB_LINE) 
								cmd_adj_line_trimpt(p0,p5,p1,p2,trimpt1,trimpt2);
						    else if (trimeeType==DB_XLINE)
							    cmd_adj_xline_trimpt(p0,p5,p1,p8,&xline_ptflag,trimpt1,trimpt2);
						    else	//ray
							    cmd_adj_ray_trimpt(p0,p5,p1,p8,trimpt1,trimpt2);
					    }
				    }
				    delete pLine;
				    delete pEllipse;
			    }
			    else if (trimEntType==DB_SPLINE) {
				    CNURBS* pNurbs = asGeoSpline((db_spline*)elistll->ename[0]);

				    CLine* pLine = NULL;
				    C3Point pt(p1[0], p1[1], p1[2]);
				    if (trimeeType==DB_LINE) 
					    pLine = new CLine(pt, C3Point(p2[0], p2[1], p2[2]));
				    else if (trimeeType==DB_XLINE) {
					    C3Point dir = C3Point(p2[0], p2[1], p2[2]) - pt;
					    dir.Unitize();
					    pLine = new CUnboundedLine(pt, dir);
				    }
				    else { // ray
					    C3Point dir = C3Point(p2[0], p2[1], p2[2]) - pt;
					    dir.Unitize();
					    pLine = new CRay(pt, dir);
					    pLine->SetDomain(0., INFINITY);
				    }

				    // find intersections of selection set with spline
				    CRealArray onLine;
				    CRealArray onNurbs;
				    Intersect(pLine, pNurbs, onLine, onNurbs, INT_EPSILON);

				    if (onLine.Size() > 0) {
					    C3Point xPoint;
					    for (register i = 0; i < onLine.Size(); i++) {
						     pLine->Evaluate(onLine[i], xPoint);
						     p4[0] = xPoint.X(); p4[1] = xPoint.Y(); p4[2] = xPoint.Z();

						     //check to see if p4 is closer than one of the trim points
						     if (trimeeType==DB_LINE) 
							     cmd_adj_line_trimpt(p0,p4,p1,p2,trimpt1,trimpt2);
						     else if (trimeeType==DB_XLINE) 
							     cmd_adj_xline_trimpt(p0,p4,p1,p8,&xline_ptflag,trimpt1,trimpt2);
						     else	 //ray
							     cmd_adj_ray_trimpt(p0,p4,p1,p8,trimpt1,trimpt2);
					    }
				    }
				    delete pLine;
				    delete pNurbs;
				}
			    else if (trimEntType==DB_TEXT) {
				    ic_assoc(elistll->entlst,210);
                    // The following checks is wrong - SWH 7/1/2004
                    /*
				    if (!icadPointEqual(ic_rbassoc->resval.rpoint,rbent.resval.rpoint)) {
					    notinucs++;
					    continue;
                    }
                    */
                    ic_ptcpy(rbent.resval.rpoint, ic_rbassoc->resval.rpoint);
				    if (cmd_get_textpts(elistll->entlst,p3,p4,p5,p6))
					   continue;

                    sds_trans(p3,&rbent,&rbwcs,0,p3);
					sds_trans(p4,&rbent,&rbwcs,0,p4);
					sds_trans(p5,&rbent,&rbwcs,0,p5);
					sds_trans(p6,&rbent,&rbwcs,0,p6);

				    if (trimeeType==DB_LINE) {
					    //breaking a line at text...

						if (RTNORM==sds_inters(p3,p4,p1,p2,1,p7))
							cmd_adj_line_trimpt(p0,p7,p1,p2,trimpt1,trimpt2);
						if (RTNORM==sds_inters(p4,p5,p1,p2,1,p7))
							cmd_adj_line_trimpt(p0,p7,p1,p2,trimpt1,trimpt2);
						if (RTNORM==sds_inters(p5,p6,p1,p2,1,p7))
							cmd_adj_line_trimpt(p0,p7,p1,p2,trimpt1,trimpt2);
						if (RTNORM==sds_inters(p6,p3,p1,p2,1,p7))
							cmd_adj_line_trimpt(p0,p7,p1,p2,trimpt1,trimpt2);
				    }
				    else if (trimeeType==DB_XLINE) {
					    if (RTNORM==sds_inters(p3,p4,p1,p2,0,p7) && 1==ic_ptlndist(p7,p3,p4,NULL,p7))
							cmd_adj_xline_trimpt(p0,p7,p1,p8,&xline_ptflag,trimpt1,trimpt2);
						if (RTNORM==sds_inters(p4,p5,p1,p2,0,p7) && 1==ic_ptlndist(p7,p4,p5,NULL,p7))
							cmd_adj_xline_trimpt(p0,p7,p1,p8,&xline_ptflag,trimpt1,trimpt2);
						if (RTNORM==sds_inters(p5,p6,p1,p2,0,p7) && 1==ic_ptlndist(p7,p5,p6,NULL,p7))
							cmd_adj_xline_trimpt(p0,p7,p1,p8,&xline_ptflag,trimpt1,trimpt2);
						if (RTNORM==sds_inters(p6,p3,p1,p2,0,p7) && 1==ic_ptlndist(p7,p6,p3,NULL,p7))
							cmd_adj_xline_trimpt(p0,p7,p1,p8,&xline_ptflag,trimpt1,trimpt2);
					}
                    else {	//trim a RAY at text
						if (RTNORM==sds_inters(p3,p4,p1,p2,0,p7) && icadDirectionEqual(p1,p2,p1,p7) && 1==ic_ptlndist(p7,p3,p4,NULL,p7))
							cmd_adj_ray_trimpt(p0,p7,p1,p8,trimpt1,trimpt2);
						if (RTNORM==sds_inters(p4,p5,p1,p2,0,p7) && icadDirectionEqual(p1,p2,p1,p7) && 1==ic_ptlndist(p7,p4,p5,NULL,p7))
							cmd_adj_ray_trimpt(p0,p7,p1,p8,trimpt1,trimpt2);
						if (RTNORM==sds_inters(p5,p6,p1,p2,0,p7) && icadDirectionEqual(p1,p2,p1,p7) && 1==ic_ptlndist(p7,p5,p6,NULL,p7))
							cmd_adj_ray_trimpt(p0,p7,p1,p8,trimpt1,trimpt2);
						if (RTNORM==sds_inters(p6,p3,p1,p2,0,p7) && icadDirectionEqual(p1,p2,p1,p7) && 1==ic_ptlndist(p7,p6,p3,NULL,p7))
							cmd_adj_ray_trimpt(p0,p7,p1,p8,trimpt1,trimpt2);
					}
				}
			    else if (trimEntType==DB_POLYLINE) {
				    //trim a line at a pline
				    //remember, line is in ucs if edgemode!=0
				    ic_assoc(elistll->entlst,70);
				    closure=ic_rbassoc->resval.rint;
				    if (closure & IC_PLINE_CLOSED)
					    elistll3=elistll->next;	//mark first vtx
				    if (closure & (IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH)) {
					    cmd_ErrorPrompt(CMD_ERR_TRIMENT,0);
					    ret=RTERROR;
					    goto exit;
					}
				    else if (!(closure & IC_PLINE_3DPLINE)) {
					    ic_assoc(elistll->entlst,210);
                        ic_ptcpy(rbent.resval.rpoint, ic_rbassoc->resval.rpoint);
                        // The following check is wrong SWH 7/1/2004
                        /*
					    if (!icadPointEqual(ic_rbassoc->resval.rpoint,rbent.resval.rpoint)) {
						    notinucs++;
						    continue;
						}
                        */
					}

				    for (fi3=0;;fi3++) {
					    //sds_entnext_noxref(etemp,etemp);
					    elistll=elistll->next;
					    if (IC_TYPE_FROM_ENAME(elistll->ename)==DB_SEQEND)
					        break;
					    elistll2=elistll->next;
					    int nextenttype=IC_TYPE_FROM_ENAME(elistll2->ename);
					    if (nextenttype==DB_SEQEND) {
						    if (!(closure & IC_PLINE_CLOSED))
							    break;
						    elistll2=elistll3; //go back to 1st vtx if it's closed
						}
					    else if (!(closure & IC_PLINE_CLOSED)) {
						    if (nextenttype==DB_SEQEND) // I don't see how this code can ever be reached
						        fi3=0;
						}
                        if (closure & IC_PLINE_SPLINEFIT) {	 //if splined pline, make sure we ignore ctrl pts
						    ic_assoc(elistll->entlst,70);
						    fi2=ic_rbassoc->resval.rint;
						    ic_assoc(elistll2->entlst,70);
						    if ((fi2 & IC_VERTEX_SPLINEFRAME) && (ic_rbassoc->resval.rint & IC_VERTEX_SPLINEFRAME))
							    break;
						    if ((fi2 & IC_VERTEX_SPLINEFRAME) || (ic_rbassoc->resval.rint & IC_VERTEX_SPLINEFRAME))
							    continue;
						}
					    ic_assoc(elistll->entlst,10);
					    ic_ptcpy(p3,ic_rbassoc->resval.rpoint);
					    ic_assoc(elistll->entlst,42);
					    fr1=ic_rbassoc->resval.rreal;
					    ic_assoc(elistll2->entlst,10);
					    ic_ptcpy(p4,ic_rbassoc->resval.rpoint);

					    if (icadRealEqual(fr1,0.0)) {
						    if (!(closure & IC_PLINE_3DPLINE)) {
								sds_trans(p3,&rbent,&rbwcs,0,p3);
								sds_trans(p4,&rbent,&rbwcs,0,p4);
							}

							fi1=sds_inters(p3,p4,p1,p2,0,p5);
							if (fi1!=RTNORM)
                                fi1=-1;
							else {
								fi1=0;
								if (trimeeType!=DB_LINE && 1==ic_ptlndist(p5,p1,p2,NULL,p5))
									fi1|=2;
								else if (trimeeType==DB_XLINE)
									fi1|=2;
								else if (trimeeType==DB_RAY && icadDirectionEqual(p1,p2,p1,p5))
									fi1|=2;
								if (1==ic_ptlndist(p5,p3,p4,NULL,p5))
									fi1|=1;
                            }
						    if (fi1!=3 && trimeeType!=DB_LINE)
							    fi1|=2;
						    if (fi1==3 || (fi3==0 && edgemode && (fi1==2))) {
							    if (trimeeType==DB_LINE)
								    cmd_adj_line_trimpt(p0,p5,p1,p2,trimpt1,trimpt2);
							    else if (trimeeType==DB_RAY)
								    cmd_adj_ray_trimpt(p0,p5,p1,p8,trimpt1,trimpt2);
							    else	//xline
								    cmd_adj_xline_trimpt(p0,p5,p1,p8,&xline_ptflag,trimpt1,trimpt2);
							}
                        }
					    else {
						    ic_bulge2arc(p3,p4,fr1,p5,&fr2,&fr3,&fr4);
						    //convert pts 1&2 into pts 6&7 temporarily

							sds_trans(p1,&rbwcs,&rbent,0,p6);
							sds_trans(p2,&rbwcs,&rbent,0,p7);

						    fi1=ic_linexarc(p6,p7,p5,fr2,fr3,fr4,p3,p4);

						    if (fi1==-1) {
							    //check 1st bdry condition - tangency
							    fi2=ic_ptlndist(p3,p6,p7,&fr5,p3);
							    if (!icadRealEqual(fr5,fr2))
								    continue;
							    if (fi2==1 || icadPointEqual(p3,p1) || icadPointEqual(p3,p2))
								    fi1=3;
							    else
								    continue;
							}
						    else {
							    //check 2nd boundary condition - endpoint
							    if (5!=(fi1&5) && trimeeType!=DB_LINE)
								    fi1|=5;
							    if (3!=(fi1&3) && (icadPointEqual(p3,p6) || icadPointEqual(p3,p7)))
								    fi1|=3;
							    if (12!=(fi1&12) && (icadPointEqual(p4,p6) || icadPointEqual(p4,p7)))
								    fi1|=12;
						    }
						    fr5=p6[2]+(p7[2]-p6[2])*sds_dist2d(p6,p3)/sds_dist2d(p6,p7);
						    fr6=p6[2]+(p7[2]-p6[2])*sds_dist2d(p6,p4)/sds_dist2d(p6,p7);

							if (!icadRealEqual(fr5,p5[2]))
								fi1&=~3;
							if (!icadRealEqual(fr6,p5[2]))
								fi1&=~12;

						    if (((fi1 & 3)== 3) || (fi3==0 && edgemode &&((fi1 & 3)==1))) {
							    //if first intersections is on the line
							    //check to see if p3 is closer than one of the trim points
							    cmd_adj_line_trimpt(p0,p3,p1,p2,trimpt1,trimpt2);
							}
						    if (((fi1 & 12)== 12) || (fi3==0 && edgemode && ((fi1 & 12)==4))) {
							    //if 2nd intersection is on the line
							    //check to see if p4 is closer than one of the trim points
							    cmd_adj_line_trimpt(p0,p4,p1,p2,trimpt1,trimpt2);
							}
						}
					}
			    }
			    else if (trimEntType==DB_LWPOLYLINE) {
				    db_lwpolyline *trimlwpl=(db_lwpolyline *)(elistll->ename[0]);
				    int vert1no,vert2no;
				    //trim a line at a pline
				    //remember, line is in ucs if edgemode!=0
				    closure=trimlwpl->ret_70();
				    ic_assoc(elistll->entlst,210);
                    // The following check is wrong. SWH - 7/1/2004
                    /*
				    if (!icadPointEqual(ic_rbassoc->resval.rpoint,rbent.resval.rpoint)) {
					    notinucs++;
					    continue;
					}
                    */
                    ic_ptcpy(rbent.resval.rpoint, ic_rbassoc->resval.rpoint);
				    for (fi3=0; fi3<trimlwpl->ret_90(); fi3++) {
					    vert1no=fi3;
					    vert2no=fi3+1;
					    //sds_entnext_noxref(etemp,etemp);
					    if (vert2no>=trimlwpl->ret_90()) {
						    if (!(closure & IC_PLINE_CLOSED))
							    break;
						    vert2no=0; //go back to 1st vtx if it's closed
						}
					    else if (!(closure & IC_PLINE_CLOSED)) {
						    if (vert2no>=trimlwpl->ret_90()) // I don't see how this code can ever be reached
						        fi3=0;
						}
					    trimlwpl->get_10(p3,vert1no);
					    trimlwpl->get_42(&fr1,vert1no);
					    trimlwpl->get_10(p4,vert2no);

					    if (icadRealEqual(fr1,0.0)) {
							sds_trans(p3,&rbent,&rbwcs,0,p3);
							sds_trans(p4,&rbent,&rbwcs,0,p4);


							fi1=sds_inters(p3,p4,p1,p2,0,p5);
							if (fi1!=RTNORM)
                                fi1=-1;
							else {
								fi1=0;
								if (trimeeType!=DB_LINE && 1==ic_ptlndist(p5,p1,p2,NULL,p5))
									fi1|=2;
								else if (trimeeType!=DB_XLINE)
									fi1|=2;
								else if (trimeeType!=DB_RAY && icadDirectionEqual(p1,p2,p1,p5))
									fi1|=2;
								if (1==ic_ptlndist(p5,p3,p4,NULL,p5))
									fi1|=1;
                            }
						    if (fi1!=3 && trimeeType!=DB_LINE)
							    fi1|=2;
						    if (fi1==3 || (fi3==0 && edgemode && (fi1==2))) {
							    if (trimeeType==DB_LINE)
								    cmd_adj_line_trimpt(p0,p5,p1,p2,trimpt1,trimpt2);
							    else if (trimeeType==DB_RAY)
								    cmd_adj_ray_trimpt(p0,p5,p1,p8,trimpt1,trimpt2);
							    else	//xline
								    cmd_adj_xline_trimpt(p0,p5,p1,p8,&xline_ptflag,trimpt1,trimpt2);
							    }
					    }
					    else {
						    ic_bulge2arc(p3,p4,fr1,p5,&fr2,&fr3,&fr4);
						    //convert pts 1&2 into pts 6&7 temporarily

							sds_trans(p1,&rbwcs,&rbent,0,p6);
							sds_trans(p2,&rbwcs,&rbent,0,p7);

						    fi1=ic_linexarc(p6,p7,p5,fr2,fr3,fr4,p3,p4);

						    if (fi1==-1) {
							    //check 1st bdry condition - tangency
							    fi2=ic_ptlndist(p3,p6,p7,&fr5,p3);
							    if (!icadRealEqual(fr5,fr2))
								    continue;
							    if (fi2==1 || icadPointEqual(p3,p1) || icadPointEqual(p3,p2))
								    fi1=3;
							    else
								    continue;
							}
						    else {
							    //check 2nd boundary condition - endpoint
							    if (5!=(fi1&5) && trimeeType!=DB_LINE)
								    fi1|=5;
							    if (3!=(fi1&3) && (icadPointEqual(p3,p6) || icadPointEqual(p3,p7)))
								    fi1|=3;
							    if (12!=(fi1&12) && (icadPointEqual(p4,p6) || icadPointEqual(p4,p7)))
								    fi1|=12;
							}
						    fr5=p6[2]+(p7[2]-p6[2])*sds_dist2d(p6,p3)/sds_dist2d(p6,p7);
						    fr6=p6[2]+(p7[2]-p6[2])*sds_dist2d(p6,p4)/sds_dist2d(p6,p7);

							if (!icadRealEqual(fr5,p5[2]))
								fi1&=~3;
							if (!icadRealEqual(fr6,p5[2]))
								fi1&=~12;

						    if (((fi1 & 3)== 3) || (fi3==0 && edgemode &&((fi1 & 3)==1))) {
							    //if first intersections is on the line
							    //check to see if p3 is closer than one of the trim points
							    cmd_adj_line_trimpt(p0,p3,p1,p2,trimpt1,trimpt2);
							}
						    if (((fi1 & 12)== 12) || (fi3==0 && edgemode && ((fi1 & 12)==4))) {
							    //if 2nd intersection is on the line
							    //check to see if p4 is closer than one of the trim points
							    cmd_adj_line_trimpt(p0,p4,p1,p2,trimpt1,trimpt2);
							}
						}
					}
				} // end of lwpolyline trim
	        } // end of loop over trim objects

		    if (trimeeType==DB_LINE) {
			    //We now have the correct trim points, trimpt1 & trimpt2.  Go ahead and modify line.
			    if (icadPointEqual(p1,trimpt1) && icadPointEqual(p2,trimpt2))
				    goto exit;
			    if (icadPointEqual(p1,trimpt1)) {
				    ic_assoc(elist,10);
				    ic_ptcpy(ic_rbassoc->resval.rpoint,trimpt2);
				    if (sds_entmod(elist)!=RTNORM)
					    ret=-1;
				}
			    else if (icadPointEqual(p2,trimpt2)) {
				    ic_assoc(elist,11);
				    ic_ptcpy(ic_rbassoc->resval.rpoint,trimpt1);
				    if (sds_entmod(elist)!=RTNORM)
					    ret=-1;
			    }
			    else {
				    ic_assoc(elist,11);
				    ic_ptcpy(ic_rbassoc->resval.rpoint,trimpt1);
				    if (sds_entmod(elist)!=RTNORM) {
                        ret=-1;
                        goto exit;
                    }
				    ic_assoc(elist,10);
				    ic_ptcpy(ic_rbassoc->resval.rpoint,trimpt2);
				    ic_assoc(elist,11);
				    ic_ptcpy(ic_rbassoc->resval.rpoint,p2);
				    if (sds_entmake(elist)!=RTNORM) {
                        ret=-1;
                        goto exit;
                    }
				    sds_entlast(xtraent);
				}
		    }
		    else if (trimeeType==DB_RAY) {
			    if (!icadPointEqual(trimpt2,p1)) {
				    ic_assoc(elist,10);
				    ic_ptcpy(p8,ic_rbassoc->resval.rpoint);
				    ic_ptcpy(ic_rbassoc->resval.rpoint,trimpt2);
				    if (sds_entmod(elist)!=RTNORM) {
                        ret=-1;goto 
                            exit;
                    }
				    ic_ptcpy(ic_rbassoc->resval.rpoint,p8);
				}
			    if (!icadPointEqual(trimpt1,p1)) {
				    ic_assoc(elist,11);
				    ic_ptcpy(ic_rbassoc->resval.rpoint,trimpt1);
				    ic_assoc(elist,0);
				    if ((ic_rbassoc->resval.rstring=ic_realloc_char(ic_rbassoc->resval.rstring,strlen(db_hitype2str(DB_LINE))+1))==NULL) {
					    ret=(-__LINE__);
					    goto exit;
					}
				    strcpy(ic_rbassoc->resval.rstring,db_hitype2str(DB_LINE));
				    if (sds_entmake(elist)!=RTNORM) {
                        ret=-1;
                        goto exit;
                    }
				    if (!icadPointEqual(trimpt2,p1)) {
					    //if the ray will still be there
					    sds_entlast(xtraent);
					}
				    else {
					    sds_entdel(ename);
					    sds_entlast(ename);
					    sds_redraw(ename,IC_REDRAW_DRAW);
					}
				}
		    }
		    else if (xline_ptflag>0) {
			    //modify the xline into one or more rays
			    ic_assoc(elist,0);
			    if ((ic_rbassoc->resval.rstring=ic_realloc_char(ic_rbassoc->resval.rstring,strlen("RAY"/*DNT*/ )+1))==NULL) {
				    ret=(-__LINE__);
				    goto exit;
				}
			    strcpy(ic_rbassoc->resval.rstring,"RAY"/*DNT*/ );
			    //do ray for endpt 1st so we don't have to alter vector
			    ic_assoc(elist,10);
			    if (xline_ptflag&2) {
				    ic_ptcpy(ic_rbassoc->resval.rpoint,trimpt2);
				    if (sds_entmake(elist)!=RTNORM) {
                        ret=-1;
                        goto exit;
                    }
				    sds_entdel(ename);
				    sds_entlast(ename);
				    sds_redraw(ename,IC_REDRAW_DRAW);
				}
			    if (xline_ptflag&1) {
				    ic_ptcpy(ic_rbassoc->resval.rpoint,trimpt1);
				    //reverse the vector
				    ic_assoc(elist,11);
				    ic_rbassoc->resval.rpoint[0]=-ic_rbassoc->resval.rpoint[0];
				    ic_rbassoc->resval.rpoint[1]=-ic_rbassoc->resval.rpoint[1];
				    ic_rbassoc->resval.rpoint[2]=-ic_rbassoc->resval.rpoint[2];
				    //elist is now correct - just make or mod
				    if (xline_ptflag&2) {
					    //if I already modified the xline to be a ray
					    if (sds_entmake(elist)!=RTNORM) {
                            ret=-1;
                            goto exit;
                        }
					    sds_entlast(xtraent);
					}
				    else {
					    if (sds_entmake(elist)!=RTNORM) {
                            ret=-1;
                            goto exit;
                        }
					    sds_entdel(ename);
					    sds_entlast(ename);
					    sds_redraw(ename,IC_REDRAW_DRAW);
					}
				}
			}
        } 
        else { // projmode != 0, Stephen W. Hou - 6/21/2004
			CCurve* pTrimedLine = NULL;
			if (trimeeType == DB_LINE) 
				pTrimedLine = new CLine(C3Point(p1[0], p1[1], p1[2]), C3Point(p2[0], p2[1], p2[2]));
			else if (trimeeType == DB_RAY)
				pTrimedLine = new CRay(C3Point(p1[0], p1[1], p1[2]), C3Point(p2[0], p2[1], p2[2]));
			else {
				pTrimedLine = new CUnboundedLine(C3Point(p1[0], p1[1], p1[2]), C3Point(p2[0], p2[1], p2[2]));
				xline_ptflag = 0;
			}
            CAffine ucsXForm, iUcsXForm;
            if (!bUcsMatchWcs) {
                ucsXForm.Set(ucsOrigin, ucsXDir.AsPoint(), ucsYDir.AsPoint(), ucsZDir.AsPoint());
                iUcsXForm.SetInverse(ucsXForm);
                pTrimedLine->Transform(iUcsXForm);
            }
		    C3Point pickPoint(epoint[0], epoint[1], epoint[2]);

            double rPickParam;
            bool bModified = false;
            if (pTrimedLine->HitTest(pickPoint, ucsViewDir, HitTolerance, rPickParam)) {
                if (!bUcsMatchWcs) 
                    pTrimedLine->Transform(ucsXForm);

                CDWGToGeo toGeo;
		        bool bExtend = edgemode == 0 ? false : true;
		        stdPPathList pathList;
		        db_handitem* pEntity;
		        // extract entities from selection set into Geo objects
		        for (elistll = bdry_setll; elistll != NULL; elistll = elistll->next) {
			        // if the spline to be trimmed is also selected as a cutting curve,
			        // we do not perform trim on the curve because AUTOCAD also does not 
                    // do it.
			        if (ename[0] == elistll->ename[0])
				        continue;

			        pEntity = (db_handitem*)elistll->ename[0];
			        toGeo.ExtractEntity(pEntity, pathList, bExtend, true);
		        }

                stdPPathList::const_iterator it;

			    CCurveTrimModifier modifier(pTrimedLine, plane);
			    modifier.SetRefPoint(rPickParam);

		        CRealArray rootsEllipse, roots, roots1;
		        for (it = pathList.begin(); it != pathList.end(); ++it) {
			        for (int j = 0; j < (*it)->Size(); ++j) 
				        if (modifier.ModifiedWith((*it)->Curve(j)) && !bModified)
                            bModified = true;
		        }
		        for (it = pathList.begin(); it != pathList.end(); ++it)
			        delete (*it);

			    delete pTrimedLine;
			    pTrimedLine = NULL;

			    double rTrimAt1, rTrimAt2;
			    if (bModified) {
				    modifier.GetCandidates(rTrimAt1, rTrimAt2);
				    double start = modifier.GetModifiedCurve()->Start();
				    double end = modifier.GetModifiedCurve()->End();

				    double curvature;
				    C3Point ptPoint, ptTangent;
				    if (trimeeType==DB_LINE) {
					    if (icadRealEqual(start, rTrimAt1)) {
						    modifier.GetModifiedCurve()->Evaluate(rTrimAt2, ptPoint, ptTangent, curvature);
						    trimpt2[0] = ptPoint.X(); trimpt2[1] = ptPoint.Y(); trimpt2[2] = ptPoint.Z();

						    ic_assoc(elist,10);
						    ic_ptcpy(ic_rbassoc->resval.rpoint,trimpt2);
						    if (sds_entmod(elist)!=RTNORM)
							    ret=-1;
					    }
					    else if (icadRealEqual(end, rTrimAt2)) {
						    modifier.GetModifiedCurve()->Evaluate(rTrimAt1, ptPoint, ptTangent, curvature);
						    trimpt1[0] = ptPoint.X(); trimpt1[1] = ptPoint.Y(); trimpt1[2] = ptPoint.Z();

						    ic_assoc(elist,11);
						    ic_ptcpy(ic_rbassoc->resval.rpoint,trimpt1);
						    if (sds_entmod(elist)!=RTNORM)
							    ret=-1;
					    }
					    else {
						    modifier.GetModifiedCurve()->Evaluate(rTrimAt1, ptPoint, ptTangent, curvature);
						    trimpt1[0] = ptPoint.X(); trimpt1[1] = ptPoint.Y(); trimpt1[2] = ptPoint.Z();

						    ic_assoc(elist,11);
						    ic_ptcpy(ic_rbassoc->resval.rpoint,trimpt1);
						    if (sds_entmod(elist)!=RTNORM) {
							    ret=-1;
							    goto exit;
						    }

						    modifier.GetModifiedCurve()->Evaluate(rTrimAt2, ptPoint, ptTangent, curvature);
						    trimpt2[0] = ptPoint.X(); trimpt2[1] = ptPoint.Y(); trimpt2[2] = ptPoint.Z();

						    ic_assoc(elist,10);
						    ic_ptcpy(ic_rbassoc->resval.rpoint,trimpt2);
						    ic_assoc(elist,11);
						    ic_ptcpy(ic_rbassoc->resval.rpoint,p2);
						    if (sds_entmake(elist)!=RTNORM) {
							    ret=-1;
							    goto exit;
						    }
						    sds_entlast(xtraent);
					    }
				    }
				    else if (trimeeType==DB_RAY) {
					    if (!icadRealEqual(rTrimAt2, start)) {
						    modifier.GetModifiedCurve()->Evaluate(rTrimAt2, ptPoint, ptTangent, curvature);
						    trimpt2[0] = ptPoint.X(); trimpt2[1] = ptPoint.Y(); trimpt2[2] = ptPoint.Z();

						    ic_assoc(elist,10);
						    ic_ptcpy(p8,ic_rbassoc->resval.rpoint);
						    ic_ptcpy(ic_rbassoc->resval.rpoint,trimpt2);
						    if (sds_entmod(elist)!=RTNORM) {
							    ret=-1;
							    goto exit;
						    }
						    ic_ptcpy(ic_rbassoc->resval.rpoint,p8);
					    }
					    if (!icadRealEqual(rTrimAt1, start)) {
						    modifier.GetModifiedCurve()->Evaluate(rTrimAt1, ptPoint, ptTangent, curvature);
						    trimpt1[0] = ptPoint.X(); trimpt1[1] = ptPoint.Y(); trimpt1[2] = ptPoint.Z();

						    ic_assoc(elist,11);
						    ic_ptcpy(ic_rbassoc->resval.rpoint,trimpt1);
						    ic_assoc(elist,0);
						    if ((ic_rbassoc->resval.rstring=ic_realloc_char(ic_rbassoc->resval.rstring,strlen(db_hitype2str(DB_LINE))+1))==NULL) {
							    ret=(-__LINE__);
							    goto exit;
						    }
						    strcpy(ic_rbassoc->resval.rstring,db_hitype2str(DB_LINE));
						    if (sds_entmake(elist)!=RTNORM) {
							    ret=-1;
							    goto exit;
						    }
						    if (!icadRealEqual(rTrimAt2, start)) {
							    //if the ray will still be there
							    sds_entlast(xtraent);
						    }
						    else {
							    sds_entdel(ename);
							    sds_entlast(ename);
							    sds_redraw(ename,IC_REDRAW_DRAW);
						    }
					    }
				    }
				    else { // modify xline
					    //modify the xline into one or more rays
					    ic_assoc(elist,0);
					    if ((ic_rbassoc->resval.rstring=ic_realloc_char(ic_rbassoc->resval.rstring,strlen("RAY"/*DNT*/ )+1))==NULL) {
						    ret=(-__LINE__);
						    goto exit;
					    }
					    strcpy(ic_rbassoc->resval.rstring,"RAY"/*DNT*/ );
					    //do ray for endpt 1st so we don't have to alter vector
					    ic_assoc(elist,10);
					    if (!icadRealEqual(end, rTrimAt2)) {
						    modifier.GetModifiedCurve()->Evaluate(rTrimAt2, ptPoint, ptTangent, curvature);
						    trimpt2[0] = ptPoint.X(); trimpt2[1] = ptPoint.Y(); trimpt2[2] = ptPoint.Z();

						    ic_ptcpy(ic_rbassoc->resval.rpoint,trimpt2);
						    if (sds_entmake(elist)!=RTNORM) {
							    ret=-1;
							    goto exit;
						    }
						    sds_entdel(ename);
						    sds_entlast(ename);
						    sds_redraw(ename,IC_REDRAW_DRAW);
					    }
					    if (!icadRealEqual(start, rTrimAt1)) {
						    modifier.GetModifiedCurve()->Evaluate(rTrimAt1, ptPoint, ptTangent, curvature);
						    trimpt1[0] = ptPoint.X(); trimpt1[1] = ptPoint.Y(); trimpt1[2] = ptPoint.Z();

						    ic_ptcpy(ic_rbassoc->resval.rpoint,trimpt1);
						    //reverse the vector
						    ic_assoc(elist,11);
						    ic_rbassoc->resval.rpoint[0]=-ic_rbassoc->resval.rpoint[0];
						    ic_rbassoc->resval.rpoint[1]=-ic_rbassoc->resval.rpoint[1];
						    ic_rbassoc->resval.rpoint[2]=-ic_rbassoc->resval.rpoint[2];
						    //elist is now correct - just make or mod
						    if (!icadRealEqual(end, rTrimAt2)) {
							    //if I already modified the xline to be a ray
							    if (sds_entmake(elist)!=RTNORM) {
								    ret=-1;
								    goto exit;
							    }
							    sds_entlast(xtraent);
						    }
						    else {
							    if (sds_entmake(elist)!=RTNORM) {
								    ret=-1;
								    goto exit;
							    }
							    sds_entdel(ename);
							    sds_entlast(ename);
							    sds_redraw(ename,IC_REDRAW_DRAW);
						    }
					    }
                    }
                }
			}
        } // projmode != 0
		goto exit;
    }
	//********************************************************************************
	//**** Trim an Arc or Circle******************************************************
	//********************************************************************************
	else if (sds_wcmatch(etype,"ARC,CIRCLE"/*DNT*/)==RTNORM) {

        CEllipArc* pEllipse = (trimeeType==DB_ARC) ?
                               asGeoEllipArc((db_arc*)ename[0]) :
                               asGeoEllipse((db_circle*)ename[0]);

        double start = pEllipse->Start();
        double end = pEllipse->End();
		bool isClosed;
        if (trimeeType == DB_CIRCLE)
            isClosed = true;
        else
            isClosed = icadIsZero(pEllipse->Start() + TWOPI - pEllipse->End(), EPSILON);

        pEllipse->Transform(w2uXForm);


		CDWGToGeo toGeo;
		bool bExtend = edgemode == 0 ? false : true;
		stdPPathList pathList;
		db_handitem* pEntity;

		// extract entities from selection set into Geo objects
		for (elistll = bdry_setll; elistll != NULL; elistll = elistll->next) {
			if (ename[0] == elistll->ename[0])
				continue;
			pEntity = (db_handitem*)elistll->ename[0];
			toGeo.ExtractEntity(pEntity, pathList, bExtend, true);
		}

		double pointParam;
        if (pEllipse->HitTest(C3Point(epoint[0], epoint[1], epoint[2]), ucsViewDir, HitTolerance, pointParam)) {
            pEllipse->Transform(u2wXForm);

            stdPPathList::const_iterator it;
            bool bModified = false;

            CCurveTrimModifier modifier(pEllipse, (projmode) ? plane : *(CPlane*)NULL);
			modifier.SetRefPoint(pointParam);

		    CRealArray rootsEllipse, roots, roots1;
		    for (it = pathList.begin(); it != pathList.end(); ++it) {
			    for (int j = 0; j < (*it)->Size(); ++j) 
				    if (modifier.ModifiedWith((*it)->Curve(j)) && !bModified)
                        bModified = true;
		    }
            delete pEllipse;

		    for (it = pathList.begin(); it != pathList.end(); ++it) 
			    delete (*it);

            if (bModified && modifier.GetCandidates(trimang1, trimang2)) {
                if (trimang1 < 0.0) {
                    trimang1 += TWOPI;
                    trimang2 += TWOPI;
                }
            }
            else
                goto exit;

			//We now have the correct trim angles, trimang1 & trimang2.  Go ahead and modify arc.
			if ((icadAngleEqual(start, trimang1)) && (icadAngleEqual(end, trimang2)))
				goto exit;

			if (trimeeType == DB_ARC) {
				if (icadAngleEqual(start, trimang1)) {
					ic_assoc(elist,50);
					ic_rbassoc->resval.rreal = trimang2;
					if (sds_entmod(elist) != RTNORM)
						ret=-1;
				}
				else if (icadAngleEqual(end,trimang2)) {
					ic_assoc(elist, 51);
					ic_rbassoc->resval.rreal = trimang1;
					if (sds_entmod(elist)!=RTNORM)
						ret=-1;
				}
				else {
					ic_assoc(elist,51);
					ic_rbassoc->resval.rreal = trimang1;
					if (sds_entmod(elist)!=RTNORM) {
						ret=-1;
						goto exit;
					}
					ic_assoc(elist,50);
					ic_rbassoc->resval.rreal = trimang2;
					ic_assoc(elist,51);
					ic_rbassoc->resval.rreal = end;
					if (sds_entmake(elist)!=RTNORM)
						ret=-1;
					sds_entlast(xtraent);
				}
			}
			else {	//we're trimming a circle
				if (trimang1 == start || trimang2 == end)
					goto exit;
				//don't make arc from new list - elist for circle contains xdata!
				ic_assoc(elist,0);
				if ((ic_rbassoc->resval.rstring=ic_realloc_char(ic_rbassoc->resval.rstring,strlen("ARC"/*DNT*/)+1))==NULL) {
					ret=(-__LINE__);
					goto exit;
				}
				strcpy(ic_rbassoc->resval.rstring,"ARC"/*DNT*/ );
				for (rbptemp=elist; rbptemp->rbnext!=NULL; rbptemp=rbptemp->rbnext);
				if ((rbptemp->rbnext=sds_buildlist(50,trimang2,51,trimang1,0)) == NULL) {
					ret=(-__LINE__);
					goto exit;
				}
				rbptemp=NULL;
				if (sds_entmake(elist)!=RTNORM)
					ret=(-__LINE__);
				else {
					sds_entdel(ename);
					sds_entlast(ename);
					sds_redraw(ename,IC_REDRAW_DRAW);
				}
			}
        }
        else
            delete pEllipse;
	}
	//********************************************************************************
	//**** Trim a Polyline ***********************************************************
	//********************************************************************************
    else if (trimeeType==DB_POLYLINE) {
		//DEBUG NOTES FOR PLINE TRIMMING: We'll keep track of 2 trimpts, trimpt1 & trimpt2.
		//We'll start w/them at the endpoints to indicate nothing to trim in that direction
		//At 1st intersection in each direction from point picked, we'll reassign that
		//trimpt, and then on subsequent intersections we'll try and "walk" the int as far
		//away from the picked point as possible.
		//Trimpt1 will ALWAYS be BEFORE Trimpt2 along the pline.  In nearly all cases the
		//pickpoint is between the two.  The only time this WON'T be the case is if the pline
		//is closed AND they're trimming off the two pline ends and keeping the middle.
		//This will be identifiable by TRIMENDS, which will be 1 if the pickpt isn't between
		//trimpt1 and trimpt2 on a closed pline
		//NOTE: Special case:  If only ent in bdry_setll is the same polyline as the entity
		//	selected, then we trim out the segment selected by the user.

		ic_assoc(elist,70);
		closure=ic_rbassoc->resval.rint;
		if (closure & (IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH)) {
			cmd_ErrorPrompt(CMD_ERR_TRIMENT,0);
			ret=RTERROR;
			goto exit;
		}
		else if (!(closure & IC_PLINE_3DPLINE)) {
			ic_assoc(elist,210);
			if (!icadPointEqual(ic_rbassoc->resval.rpoint,rbent.resval.rpoint)) {
				cmd_ErrorPrompt(CMD_ERR_NOTINUCS,0);
				ret=RTERROR;
				goto exit;
			}
            if (!projmode)
			    sds_trans(epoint,&rbucs,&rbent,0,epoint);

			ic_assoc(elist,10);
			ic_ptcpy(p8,ic_rbassoc->resval.rpoint);//we'll need to keep elevation
		}
        else {
            if (!projmode)
			    sds_trans(epoint,&rbucs,&rbwcs,0,epoint);
        }
		ic_encpy(e1,ename);
		cmd_get_startend(ename,plstart,plend);
		//f'n doesn't get end if pline closed
		ic_ptcpy(trimpt1,plstart);
		if (closure & IC_PLINE_CLOSED)
			ic_ptcpy(plend,plstart);
		ic_ptcpy(trimpt2,plend);

        if (!projmode) {
		    //check for special case of trimming pline w/itself...
		    if (trimsetlen==1L) {
			    //if onlyone trimming entity...
			    ic_assoc(bdry_setll->entlst,-1);
			    if (ic_rbassoc->resval.rlname[0]==ename[0] && ic_rbassoc->resval.rlname[1]==ename[1]) {
				    fl1=cmd_pl_vtx_no(ename,epoint,1,&exactvert);
				    for (fl3=0; fl3<fl1; sds_entnext_noxref(e1,e1),fl3++);
				    //what to do if they pick an extraent???  search undo list??
				    //pass -1 as 2nd vertex number -- MR -- don't see that happening
				    ret=cmd_pline_breakup(ename,plstart,plend,closure,e1,epoint,true,e2,trimpt2,trimends,xtraent);
				    goto exit;
			    }
		    }


		    fl2=0;
		    do {
			    fl2++;	//keep count of # of vertices
			    sds_entnext_noxref(e1,e1);
			    FreeResbufIfNotNull(&rbp1);
			    if ((rbp1=sds_entget(e1))==NULL)
				    goto exit;	//passed a seqend
			    ic_assoc(rbp1,0);
			    if (IC_TYPE_FROM_ENAME(e1)==DB_SEQEND)
				    break;
			    sds_entnext_noxref(e1,e2);
			    FreeResbufIfNotNull(&rbp2);
			    if ((rbp2=sds_entget(e2))==NULL)
				    goto exit;
			    ic_assoc(rbp2,0);
			    if (IC_TYPE_FROM_ENAME(e2)==DB_SEQEND) {
				    if (!(closure & IC_PLINE_CLOSED))
					    break;
				    cmd_pline_entnext(e1,e2,1);
				    FreeResbufIfNotNull(&rbp2);
				    if ((rbp2=sds_entget(e2))==NULL)
					    goto exit;
			    }
                if (closure & IC_PLINE_SPLINEFIT) {	//if splined pline, make sure we ignore ctrl pts
				    ic_assoc(rbp1,70);
				    fi2=ic_rbassoc->resval.rint;
				    ic_assoc(rbp2,70);
				    if ((fi2 & IC_VERTEX_SPLINEFRAME) && (ic_rbassoc->resval.rint & IC_VERTEX_SPLINEFRAME))
					    break;
				    if ((fi2 & IC_VERTEX_SPLINEFRAME) || (ic_rbassoc->resval.rint & IC_VERTEX_SPLINEFRAME))
					    continue;
			    }
			    ic_assoc(rbp1,10);
			    ic_ptcpy(p1,ic_rbassoc->resval.rpoint);
			    ic_assoc(rbp1,42);
			    fr1=ic_rbassoc->resval.rreal;
			    ic_assoc(rbp2,10);
			    ic_ptcpy(p2,ic_rbassoc->resval.rpoint);
			    FreeResbufIfNotNull(&rbp1);
			    FreeResbufIfNotNull(&rbp2);
			    if (icadRealEqual(fr1,0.0)) {
				    //****************************************
				    //check polyline at a straight segment against all entities....
				    //****************************************
				    //for (ssct=0L;sds_ssname(ss,ssct,etemp)==RTNORM;ssct++)
				    for (elistll=bdry_setll; elistll!=NULL; elistll=elistll->next) {
					    int trimEntType=IC_TYPE_FROM_ENAME(elistll->ename);
			            if (ename[0] == elistll->ename[0])
				            continue;
					    if (trimEntType==DB_LINE || trimEntType==DB_XLINE || trimEntType==DB_RAY) {
						    ic_assoc(elistll->entlst,10);
						    ic_ptcpy(p3,ic_rbassoc->resval.rpoint);
						    ic_assoc(elistll->entlst,11);
						    ic_ptcpy(p4,ic_rbassoc->resval.rpoint);
						    if (trimEntType!=DB_LINE) {
							    //for ray or xline, convert vector to point
							    ic_ptcpy(p8,p4);
							    p4[0]+=p3[0];
							    p4[1]+=p3[1];
							    p4[2]+=p3[2];
							    if (!(closure & IC_PLINE_3DPLINE))
								    sds_trans(p8,&rbwcs,&rbent,1,p8);

						    }
						    if (!(closure & IC_PLINE_3DPLINE)) {
							    //trimming a 2d pline, so change line's pts into ecs
							    sds_trans(p3,&rbwcs,&rbent,0,p3);
							    sds_trans(p4,&rbwcs,&rbent,0,p4);
						    }

						    fi1=sds_inters(p3,p4,p1,p2,0,p5);
						    if (fi1!=RTNORM)
							    fi1=IC_LINEINT_UNDEF_OR_PARALLEL;
						    else {
							    fi1=IC_LINEINT_ON_NEITHER_SEG;
							    if (1==ic_ptlndist(p5,p1,p2,NULL,p6))
								    fi1|=IC_LINEINT_ON_FIRST_SEG;
							    if (trimEntType==DB_XLINE)
								    fi1|=IC_LINEINT_ON_SECOND_SEG;
							    else if (trimEntType==DB_RAY && icadDirectionEqual(p3,p4,p3,p5))
									    fi1|=IC_LINEINT_ON_SECOND_SEG;
						    }

                            if (fi1==IC_LINEINT_ON_SECOND_SEG && !edgemode)	{ //added !edgemode for speed
							    if (trimEntType==DB_XLINE)
								    fi1=(IC_LINEINT_ON_FIRST_SEG | IC_LINEINT_ON_SECOND_SEG);
							    if (trimEntType==DB_RAY) {
								    //if int is in correct direction, go ahead and accept it
								    if (fabs(p8[0])>fabs(p8[1])) {
									    if (fabs(p8[0])>fabs(p8[1]))
										    fi2=IC_LINEINT_ON_NEITHER_SEG;
									    else
										    fi2=IC_LINEINT_ON_FIRST_SEG;
								    }
								    else if (fabs(p8[1])>fabs(p8[2]))
									    fi2=IC_LINEINT_ON_FIRST_SEG;
								    else
									    fi2=IC_LINEINT_ON_SECOND_SEG;
								    if ((p8[fi2]*(p5[fi2]-p3[fi2]))>CMD_FUZZ)
									    fi1=(IC_LINEINT_ON_FIRST_SEG | IC_LINEINT_ON_SECOND_SEG);
							    }
						    }

						    if (edgemode && (fi1 >= IC_LINEINT_ON_SECOND_SEG)) {
							    trimret=cmd_adj_pline_trimpts(ename,p5,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_pline_dist);
							    if (trimret>-1)
								    trimends=trimret;
						    }
						    else if (!edgemode && IC_LINEINT_ON_BOTH_SEGS(fi1)) {
							    trimret=cmd_adj_pline_trimpts(ename,p5,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_pline_dist);
							    if (trimret>-1)
								    trimends=trimret;
						    }
					    }
					    else if (sds_wcmatch(elistll->grpptr->resval.rstring,"ARC,CIRCLE"/*DNT*/)==RTNORM) {
						    //trim str pline seg at circle or arc
						    fr3=0.0;
						    fr4=IC_PI;
						    for (rbptemp=elistll->entlst; rbptemp!=NULL; rbptemp=rbptemp->rbnext) {
							    if (rbptemp->restype==10)
								    ic_ptcpy(p3,rbptemp->resval.rpoint);
							    else if (rbptemp->restype==40)
								    fr2=rbptemp->resval.rreal;
							    else if (rbptemp->restype==50)
								    fr3=rbptemp->resval.rreal;//not for circles
							    else if (rbptemp->restype==51)
								    fr4=rbptemp->resval.rreal;//not for circles
							    else if (rbptemp->restype==210)
								    ic_ptcpy(p4,rbptemp->resval.rpoint);
						    }
						    if (!icadPointEqual(rbent.resval.rpoint,p4)) {
							    notinucs++;
							    continue;
						    }
						    if (!(closure & IC_PLINE_3DPLINE) && !icadRealEqual(p3[2],p0[2]))
							    continue;
						    if (closure & IC_PLINE_3DPLINE) {
							    //if trimming a 3d pline, make copies of line pts in ecs
							    sds_trans(p1,&rbwcs,&rbent,0,p6);
							    sds_trans(p2,&rbwcs,&rbent,0,p7);
						    }
						    else {
							    ic_ptcpy(p6,p1);
							    ic_ptcpy(p7,p2);
						    }
						    fi1=ic_linexarc(p6,p7,p3,fr2,fr3,fr4,p4,p5);
						    if (fi1==IC_ARCINT_NOINTS)
							    continue;

						    fr5=p6[2]+(p7[2]-p6[2])*sds_dist2d(p6,p4)/sds_dist2d(p6,p7);
						    fr6=p6[2]+(p7[2]-p6[2])*sds_dist2d(p6,p5)/sds_dist2d(p6,p7);

						    if (!icadRealEqual(fr5,p3[2]))
							    fi1 &= ~(IC_ARCINT_FIRSTINT_ON_LINE | IC_ARCINT_FIRSTINT_ON_ARC);
						    if (!icadRealEqual(fr6,p3[2]))
							    fi1 &= ~(IC_ARCINT_SECONDINT_ON_LINE | IC_ARCINT_SECONDINT_ON_ARC);
								    
						    if (closure & IC_PLINE_3DPLINE) {
							    //if trimming a 3d pline, move crossings back to wcs
							    sds_trans(p4,&rbent,&rbwcs,0,p4);
							    sds_trans(p5,&rbent,&rbwcs,0,p5);
						    }
						    if (edgemode || trimEntType==DB_CIRCLE) {
                                if (fi1 & IC_ARCINT_FIRSTINT_ON_LINE) { //if intersection is on the line
								    //check to see if p4 is closer than one of the trim points
								    trimret=cmd_adj_pline_trimpts(ename,p4,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_pline_dist);
								    if (trimret>-1)
									    trimends=trimret;
							    }
                                if (fi1 & IC_ARCINT_SECONDINT_ON_LINE) {	//if 2nd intersection is on the line
								    //check to see if p5 is closer than one of the trim points
								    trimret=cmd_adj_pline_trimpts(ename,p5,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_pline_dist);
								    if (trimret>-1)
									    trimends=trimret;
							    }
							    continue;
						    }
                            else {	//an arc and edgemode=0 so we're not extending it into a circle
                                if (IC_ARCINT_FIRSTINT_ON_BOTH(fi1)) { //if first intersections is on the line and arc both
								    //check to see if p4 is closer than one of the trim points
								    trimret=cmd_adj_pline_trimpts(ename,p4,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_pline_dist);
								    if (trimret>-1)
									    trimends=trimret;
							    }
                                if (IC_ARCINT_SECONDINT_ON_BOTH(fi1)) { //if 2nd intersection is on the line
								    //check to see if p5 is closer than one of the trim points
								    trimret=cmd_adj_pline_trimpts(ename,p5,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_pline_dist);
								    if (trimret>-1)
									    trimends=trimret;
							    }
							    continue;
						    }
					    }
					    else if (trimEntType==DB_ELLIPSE) {
						    //trim str pline seg at ellipse 
						    CEllipArc* pEllipse = asGeoEllipArc((db_ellipse*)elistll->ename[0]);
						    ASSERT(pEllipse != NULL);

						    if (0==projmode && !icadRealEqual(pEllipse->Center().Z(), p0[2])) {
							    delete pEllipse;
							    continue;
						    }

						    // To extend to ellipse when edgemode > 0
						    if (!pEllipse->Closed() && edgemode) 
							    pEllipse->SetDomain(0.0, TWOPI);

						    CLine* pLine = new CLine(C3Point(p1[0], p1[1], p1[2]), C3Point(p2[0], p2[1], p2[2]));
						    ASSERT(pLine != NULL);

						    // find intersections of selection set with elliptical arc
						    CRealArray onLine;
						    CRealArray onEllipse;
						    Intersect(pLine, pEllipse, onLine, onEllipse, INT_EPSILON);

						    delete pEllipse;

						    if (!onLine.Size()) { // no intersections
							    delete pLine;
							    continue;
						    }

						    C3Point xPoint;
						    pLine->Evaluate(onLine[0], xPoint);
						    p4[0] = xPoint.X(); p4[1] = xPoint.Y(); p4[2] = xPoint.Z();
						    if (onLine.Size() > 1) {
							    pLine->Evaluate(onLine[1], xPoint);
							    p5[0] = xPoint.X(); p5[1] = xPoint.Y(); p5[2] = xPoint.Z();
						    }
						    delete pLine;

						    //check to see if p4 is closer than one of the trim points
						    trimret=cmd_adj_pline_trimpts(ename,p4,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_pline_dist);
						    if (trimret>-1)
							    trimends=trimret;

						    if (onLine.Size() > 1) {	//if 2nd intersection is on the line
							    //check to see if p5 is closer than one of the trim points
							    trimret=cmd_adj_pline_trimpts(ename,p5,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_pline_dist);
							    if (trimret>-1)
								    trimends=trimret;
						    }
						    continue;
					    }
					    else if (trimEntType==DB_SPLINE) {
						    CNURBS* pNurbs = asGeoSpline((db_spline*)elistll->ename[0]);
						    ASSERT(pNurbs != NULL);

						    CLine* pLine = new CLine(C3Point(p1[0], p1[1], p1[2]), C3Point(p2[0], p2[1], p2[2]));
						    ASSERT(pLine != NULL);

						    // find intersections of selection set with elliptical arc
						    CRealArray onLine;
						    CRealArray onNurbs;
						    Intersect(pLine, pNurbs, onLine, onNurbs, INT_EPSILON);
						    delete pNurbs;

						    if (!onLine.Size()) { // no intersections
							    delete pLine;
							    continue;
						    }

						    // we need to identify the pick point is between which two intersection
						    // points.
						    //
						    C3Point xPoint;
						    for (register i = 0; i < onLine.Size(); i++) {
							    pLine->Evaluate(onLine[i], xPoint);
							    p4[0] = xPoint.X(); p4[1] = xPoint.Y(); p4[2] = xPoint.Z();
								    
							    //check to see if p4 is closer than one of the trim points
							    trimret=cmd_adj_pline_trimpts(ename,p4,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_pline_dist);
							    if (trimret>-1)
								    trimends=trimret;
						    }
						    delete pLine;
						    continue;
					    }
					    else if (trimEntType==DB_TEXT) {
						    ic_assoc(elistll->entlst,210);
						    if (!icadPointEqual(ic_rbassoc->resval.rpoint,rbent.resval.rpoint)) {
							    notinucs++;
							    continue;
						    }
						    if (!(closure & IC_PLINE_3DPLINE) && !icadRealEqual(p3[2],p0[2]))
							    continue;
						    //trimming a straight segment at text...
						    if (cmd_get_textpts(elistll->entlst,p3,p4,p5,p6))
							    continue;
						    if (closure & IC_PLINE_3DPLINE) {
							    sds_trans(p3,&rbent,&rbwcs,0,p3);
							    sds_trans(p4,&rbent,&rbwcs,0,p4);
							    sds_trans(p5,&rbent,&rbwcs,0,p5);
							    sds_trans(p6,&rbent,&rbwcs,0,p6);
						    }

						    if (RTNORM==sds_inters(p3,p4,p1,p2,1,p7)) {
							    trimret=cmd_adj_pline_trimpts(ename,p7,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_pline_dist);
							    if (trimret>-1)
								    trimends=trimret;
						    }
						    if (RTNORM==sds_inters(p4,p5,p1,p2,1,p7)) {
							    trimret=cmd_adj_pline_trimpts(ename,p7,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_pline_dist);
							    if (trimret>-1)
								    trimends=trimret;
						    }
						    if (RTNORM==sds_inters(p5,p6,p1,p2,1,p7)) {
							    trimret=cmd_adj_pline_trimpts(ename,p7,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_pline_dist);
							    if (trimret>-1)
								    trimends=trimret;
						    }
					        if (RTNORM==sds_inters(p6,p3,p1,p2,1,p7)) {
							    trimret=cmd_adj_pline_trimpts(ename,p7,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_pline_dist);
							    if (trimret>-1)
								    trimends=trimret;
						    }

					    }
					    else if (trimEntType==DB_POLYLINE) {
						    //**************************************
						    //trimming a straight pline seg at a pline
						    //**************************************
						    ic_assoc(elistll->entlst,70);
						    closure2=ic_rbassoc->resval.rint;
						    if (closure2 & (IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH))
							    continue;
						    //check elevations
						    if (!(closure & IC_PLINE_3DPLINE) && !(closure2 & IC_PLINE_3DPLINE)) {
							    ic_assoc(elistll->entlst,10);
							    if (!icadRealEqual(ic_rbassoc->resval.rpoint[2],p0[2]))
								    continue;
						    }

						    if (closure2 & IC_PLINE_CLOSED)
							    elistll3=elistll->next;	//mark first vtx
						    for (fi3=0;;fi3++) {
							    elistll=elistll->next;
							    if (IC_TYPE_FROM_ENAME(elistll->ename)==DB_SEQEND)
								    break;
							    elistll2=elistll->next;
							    if (IC_TYPE_FROM_ENAME(elistll2->ename)==DB_SEQEND) {
								    if (!(closure2 & IC_PLINE_CLOSED))
									    break;
								    elistll2=elistll3;
							    }
							    else if (!(closure2 & IC_PLINE_CLOSED)) {
								    if (IC_TYPE_FROM_ENAME(elistll2->ename)==DB_SEQEND)
									    fi3=0;
							    }
                                if (closure2 & IC_PLINE_SPLINEFIT) {	 //if splined pline, make sure we ignore ctrl pts
								    ic_assoc(elistll->entlst,70);
								    fi2=ic_rbassoc->resval.rint;
								    ic_assoc(elistll2->entlst,70);
								    if ((fi2 & IC_VERTEX_SPLINEFRAME) && (ic_rbassoc->resval.rint & IC_VERTEX_SPLINEFRAME))
									    break;
								    if ((fi2 & IC_VERTEX_SPLINEFRAME) || (ic_rbassoc->resval.rint & IC_VERTEX_SPLINEFRAME))
									    continue;
							    }
							    ic_assoc(elistll->entlst,10);
							    ic_ptcpy(p3,ic_rbassoc->resval.rpoint);
							    ic_assoc(elistll->entlst,42);
							    fr5=ic_rbassoc->resval.rreal;
							    ic_assoc(elistll2->entlst,10);
							    ic_ptcpy(p4,ic_rbassoc->resval.rpoint);
							    if ((closure2 & IC_PLINE_3DPLINE) && !(closure & IC_PLINE_3DPLINE)) {
								    //if this one's 3d and selected one was 2d, use ecs
								    if (fabs(fr5)>CMD_FUZZ)
                                        continue;
								    sds_trans(p3,&rbwcs,&rbent,0,p3);
								    sds_trans(p4,&rbwcs,&rbent,0,p4);
							    }
							    if (icadRealEqual(fr5,0.0)) {
								    fi2=ic_linexline(p3,p4,p1,p2,p5);
								    if (IC_LINEINT_ON_BOTH_SEGS(fi2) || (edgemode && fi3==0 && fi2==IC_LINEINT_ON_SECOND_SEG)) {
									    trimret=cmd_adj_pline_trimpts(ename,p5,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_pline_dist);
									    if (trimret>-1)
										    trimends=trimret;
								    }
							    }
							    else
								    {
								    if (!(closure2 & IC_PLINE_3DPLINE) && (closure & IC_PLINE_3DPLINE)) {
									    //if selected one's 3d and this one's 2d...
									    //make temp copies of p1 & p2 in ecs
									    sds_trans(p1,&rbwcs,&rbent,0,p6);
									    sds_trans(p2,&rbwcs,&rbent,0,p7);
								    }
								    else {
									    ic_ptcpy(p6,p1);
									    ic_ptcpy(p7,p2);
								    }
								    ic_bulge2arc(p3,p4,fr5,p5,&fr2,&fr3,&fr4);
								    fi1=ic_linexarc(p6,p7,p5,fr2,fr3,fr4,p3,p4);
								    if (fi1==IC_ARCINT_NOINTS)
									    continue;

								    fr5=p6[2]+(p7[2]-p6[2])*sds_dist2d(p6,p3)/sds_dist2d(p6,p7);
								    fr6=p6[2]+(p7[2]-p6[2])*sds_dist2d(p6,p4)/sds_dist2d(p6,p7);

								    if (!icadRealEqual(fr5,p5[2]))
									    fi1 &= ~(IC_ARCINT_FIRSTINT_ON_LINE | IC_ARCINT_FIRSTINT_ON_ARC);
								    if (!icadRealEqual(fr6,p5[2]))
									    fi1 &= ~(IC_ARCINT_SECONDINT_ON_LINE | IC_ARCINT_SECONDINT_ON_ARC);

								    if (!(closure2 & IC_PLINE_3DPLINE) && (closure & IC_PLINE_3DPLINE)) {
									    //if selected one's 3d and this one's 2d...
									    sds_trans(p3,&rbent,&rbwcs,0,p3);
									    sds_trans(p4,&rbent,&rbwcs,0,p4);
								    }
								    if (IC_ARCINT_FIRSTINT_ON_BOTH(fi1) ||
									    (edgemode && fi3==0 && ((fi1 & IC_ARCINT_FIRSTINT_ON_ARC) && !(fi1 & IC_ARCINT_FIRSTINT_ON_LINE)))) {
									    trimret=cmd_adj_pline_trimpts(ename,p3,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_pline_dist);
									    if (trimret>-1)
										    trimends=trimret;
								    }
								    if (IC_ARCINT_SECONDINT_ON_BOTH(fi1) ||
									    (edgemode && fi3==0 && ((fi1 & IC_ARCINT_SECONDINT_ON_ARC)	&& !(fi1 &IC_ARCINT_SECONDINT_ON_LINE)))) {
									    trimret=cmd_adj_pline_trimpts(ename,p4,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_pline_dist);
									    if (trimret>-1)
										    trimends=trimret;
								    }
							    }
						    }//for loop on pline
					    }//polyline
					    else if (trimEntType==DB_LWPOLYLINE) {
						    //**************************************
						    //trimming a straight pline seg at an lwpline
						    //**************************************
						    db_lwpolyline *trimlwpl=(db_lwpolyline *)(elistll->ename[0]);
						    int vert1no,vert2no;
						    closure2=trimlwpl->ret_70();

						    //check elevations
						    if (0==projmode && !(closure & IC_PLINE_3DPLINE)) {
							    if (!icadRealEqual(trimlwpl->ret_38(),p0[2]))
								    continue;
						    }

						    for (fi3=0; fi3<trimlwpl->ret_90(); fi3++) {
							    vert1no=fi3;
							    vert2no=fi3+1;
							    //sds_entnext_noxref(etemp,etemp);
							    if (vert2no>=trimlwpl->ret_90()) {
								    if (!(closure2 & IC_PLINE_CLOSED))
									    break;
								    vert2no=0; //go back to 1st vtx if it's closed
							    }
							    else if (!(closure2 & IC_PLINE_CLOSED)) {
								    if (vert2no>=trimlwpl->ret_90()) // I don't see how this code can ever be reached
								       fi3=0;
							    }

							    trimlwpl->get_10(p3,vert1no);
							    trimlwpl->get_42(&fr5,vert1no);
							    trimlwpl->get_10(p4,vert2no);

							    if (icadRealEqual(fr5,0.0)) {
								    fi2=ic_linexline(p3,p4,p1,p2,p5);
								    if (IC_LINEINT_ON_BOTH_SEGS(fi2) || (edgemode && fi3==0 && fi2==IC_LINEINT_ON_SECOND_SEG)) {
									    trimret=cmd_adj_pline_trimpts(ename,p5,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_pline_dist);
									    if (trimret>-1)
										    trimends=trimret;
								    }
							    }
							    else {
								    if (closure & IC_PLINE_3DPLINE) {
									    //if selected one's 3d and this one's 2d...
									    //make temp copies of p1 & p2 in ecs
									    sds_trans(p1,&rbwcs,&rbent,0,p6);
									    sds_trans(p2,&rbwcs,&rbent,0,p7);
								    }
								    else {
									    ic_ptcpy(p6,p1);
									    ic_ptcpy(p7,p2);
								    }
								    ic_bulge2arc(p3,p4,fr5,p5,&fr2,&fr3,&fr4);
								    fi1=ic_linexarc(p6,p7,p5,fr2,fr3,fr4,p3,p4);
								    if (fi1==IC_ARCINT_NOINTS)
									    continue;

								    fr5=p6[2]+(p7[2]-p6[2])*sds_dist2d(p6,p3)/sds_dist2d(p6,p7);
								    fr6=p6[2]+(p7[2]-p6[2])*sds_dist2d(p6,p4)/sds_dist2d(p6,p7);

								    if (!icadRealEqual(fr5,p5[2]))
									    fi1 &= ~(IC_ARCINT_FIRSTINT_ON_LINE | IC_ARCINT_FIRSTINT_ON_ARC);
								    if (!icadRealEqual(fr6,p5[2]))
									    fi1 &= ~(IC_ARCINT_SECONDINT_ON_LINE | IC_ARCINT_SECONDINT_ON_ARC);

								    if (closure & IC_PLINE_3DPLINE) {
									    //if selected one's 3d and this one's 2d...
									    sds_trans(p3,&rbent,&rbwcs,0,p3);
									    sds_trans(p4,&rbent,&rbwcs,0,p4);
								    }
								    if (IC_ARCINT_FIRSTINT_ON_BOTH(fi1) ||
									    (edgemode && fi3==0 && ((fi1 & IC_ARCINT_FIRSTINT_ON_ARC) && !(fi1 & IC_ARCINT_FIRSTINT_ON_LINE)))) {
									    trimret=cmd_adj_pline_trimpts(ename,p3,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_pline_dist);
									    if (trimret>-1)
										    trimends=trimret;
								    }
								    if (IC_ARCINT_SECONDINT_ON_BOTH(fi1) ||
									    (edgemode && fi3==0 && ((fi1 & IC_ARCINT_SECONDINT_ON_ARC)	&& !(fi1 &IC_ARCINT_SECONDINT_ON_LINE)))) {
									    trimret=cmd_adj_pline_trimpts(ename,p4,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_pline_dist);
									    if (trimret>-1)
										    trimends=trimret;
								    }
							    }
						    }//for loop on lwpline
					    }//lwpolyline
				    } //for loop on selection set
			    }
			    else {	//fr1!=0 - a bulged section in pline
				    //check polyline at curved pline segment
				    //we KNOW that the pline is defined in ECS
				    //let p0 be ctr of arc & fr1,2,&3 be radius, start ang, & end angle
				    ic_bulge2arc(p1,p2,fr1,p3,&fr2,&fr3,&fr4);
				    ic_ptcpy(p0,p3);
				    fr1=fr2;
				    fr2=fr3;
				    fr3=fr4;
				    //for (ssct=0L;sds_ssname(ss,ssct,etemp)==RTNORM;ssct++)
				    for (elistll=bdry_setll; elistll!=NULL; elistll=elistll->next) {
					    int trimEntType=IC_TYPE_FROM_ENAME(elistll->ename);
					    if (trimEntType==DB_LINE || trimEntType==DB_XLINE || trimEntType==DB_RAY) {
						    //trim arc at line
						    ic_assoc(elistll->entlst,10);
						    ic_ptcpy(p3,ic_rbassoc->resval.rpoint);
						    ic_assoc(elistll->entlst,11);
						    ic_ptcpy(p4,ic_rbassoc->resval.rpoint);
						    if (trimEntType!=DB_LINE) {
							    //ray or xline
							    if (fabs(p4[0])>fabs(p4[1])) {
								    if (fabs(p4[0])>fabs(p4[1]))
									    fi3=0;
								    else
									    fi3=1;
							    }
							    else if (fabs(p4[1])>fabs(p4[2]))
								    fi3=1;
							    else
								    fi3=2;
							    ic_ptcpy(p8,p4);
							    p4[0]+=p3[0];
							    p4[1]+=p3[1];
							    p4[2]+=p3[2];
						    }
						    sds_trans(p3,&rbwcs,&rbent,0,p3);
						    sds_trans(p4,&rbwcs,&rbent,0,p4);
						    fi1=ic_linexarc(p3,p4,p0,fr1,fr2,fr3,p5,p6);

						    if (fi1==IC_ARCINT_NOINTS) {
							    //check 1st bdry condition - tangency
							    fi2=ic_ptlndist(p0,p3,p4,&fr5,p5);
							    if (!icadRealEqual(fr5,fr1))
								    continue;
							    //line approaches tangentially, but is tangent pt on line?
							    if (trimEntType==DB_XLINE)
								    fi1=(IC_ARCINT_FIRSTINT_ON_LINE | IC_ARCINT_FIRSTINT_ON_ARC);
							    else if (fi2==1 || icadPointEqual(p3,p5) || icadPointEqual(p4,p5))
								    fi1=(IC_ARCINT_FIRSTINT_ON_LINE | IC_ARCINT_FIRSTINT_ON_ARC);
							    else if (fi2==0 && trimEntType==DB_RAY) {
								    if (fabs(p8[0])>fabs(p8[1])) {
									    if (fabs(p8[0])>fabs(p8[1]))
										    fi2=0;
									    else
										    fi2=1;
								    }
								    else if (fabs(p8[1])>fabs(p8[2]))
									    fi2=1;
								    else
									    fi2=2;
								    if ((p8[fi2]*(p5[fi2]-p3[fi2]))>CMD_FUZZ)
									    fi1=(IC_ARCINT_FIRSTINT_ON_LINE | IC_ARCINT_FIRSTINT_ON_ARC);
							    }
							    else
								    continue;
						    }
                            else if (trimEntType!=DB_LINE) {
							    if (trimEntType==DB_XLINE)
								    fi1|=(IC_ARCINT_FIRSTINT_ON_LINE | IC_ARCINT_SECONDINT_ON_LINE);
							    else {
								    if ((p8[fi3]*(p5[fi3]-p3[fi3]))>CMD_FUZZ)
									    fi1 |= IC_ARCINT_FIRSTINT_ON_LINE;
								    if ((p8[fi3]*(p6[fi3]-p3[fi3]))>CMD_FUZZ)
									    fi1 |= IC_ARCINT_SECONDINT_ON_LINE;
							    }
						    }
						    else {
							    //check 2nd bdry condition
							    if (!IC_ARCINT_FIRSTINT_ON_BOTH(fi1)   && (icadPointEqual(p5,p3) || icadPointEqual(p5,p4)))
								    fi1 |= (IC_ARCINT_FIRSTINT_ON_LINE | IC_ARCINT_FIRSTINT_ON_ARC);
							    if (IC_ARCINT_SECONDINT_ON_BOTH(fi1) && (icadPointEqual(p6,p3) || icadPointEqual(p6,p4)))
								    fi1 |= (IC_ARCINT_SECONDINT_ON_LINE | IC_ARCINT_SECONDINT_ON_ARC);
						    }
						    if (!edgemode) {
							    if (IC_ARCINT_FIRSTINT_ON_BOTH(fi1)) {
								    trimret=cmd_adj_pline_trimpts(ename,p5,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_pline_dist);
								    if (trimret>-1)
									    trimends=trimret;
							    }
							    if (IC_ARCINT_SECONDINT_ON_BOTH(fi1)) {
								    trimret=cmd_adj_pline_trimpts(ename,p6,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_pline_dist);
								    if (trimret>-1)
									    trimends=trimret;
							    }
						    }
						    else {
							    if (fi1 & IC_ARCINT_FIRSTINT_ON_ARC) {
								    trimret=cmd_adj_pline_trimpts(ename,p5,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_pline_dist);
								    if (trimret>-1)
									    trimends=trimret;
							    }
							    if (fi1 & IC_ARCINT_SECONDINT_ON_ARC) {
								    trimret=cmd_adj_pline_trimpts(ename,p6,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_pline_dist);
								    if (trimret>-1)
									    trimends=trimret;
							    }
						    }
					    }
					    else if (trimEntType==DB_CIRCLE || trimEntType==DB_ARC) {
						    //trim bulged section at circle or arc
						    fr6=0.0;
						    fr7=IC_PI;
						    for (rbptemp=elistll->entlst; rbptemp!=NULL; rbptemp=rbptemp->rbnext) {
							    if (rbptemp->restype==10)
								    ic_ptcpy(p1,rbptemp->resval.rpoint);
							    else if (rbptemp->restype==40)
								    fr5=rbptemp->resval.rreal;
							    else if (rbptemp->restype==50)
								    fr6=rbptemp->resval.rreal;//not for circles
							    else if (rbptemp->restype==51)
								    fr7=rbptemp->resval.rreal;//not for circles
							    else if (rbptemp->restype==210)
								    ic_ptcpy(p5,rbptemp->resval.rpoint);
						    }
						    if (!icadPointEqual(p5,rbent.resval.rpoint)) {
							    notinucs++;
							    continue;
						    }
						    if (!icadRealEqual(p1[2],p8[2]))
							    continue;
						    fi1=ic_arcxarc(p0,fr1,fr2,fr3,p1,fr5,fr6,fr7,p3,p4);
						    if (fi1==IC_ARCARCINT_NOINTS)
							    continue;
						    if (trimEntType==DB_CIRCLE || edgemode) {
							    if (fi1 & IC_ARCARCINT_FIRSTINT_ON_ARC0) {
								    trimret=cmd_adj_pline_trimpts(ename,p3,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_pline_dist);
								    if (trimret>-1)
									    trimends=trimret;
							    }
							    if (fi1 & IC_ARCARCINT_SECONDINT_ON_ARC0) {
								    trimret=cmd_adj_pline_trimpts(ename,p4,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_pline_dist);
								    if (trimret>-1)
									    trimends=trimret;
							    }
							    continue;
						    }
						    else {
							    if (IC_ARCARCINT_FIRSTINT_ON_BOTH(fi1)) {
								    trimret=cmd_adj_pline_trimpts(ename,p3,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_pline_dist);
								    if (trimret>-1)
									    trimends=trimret;
							    }
							    if (IC_ARCARCINT_SECONDINT_ON_BOTH(fi1)) {
								    trimret=cmd_adj_pline_trimpts(ename,p4,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_pline_dist);
								    if (trimret>-1)
									    trimends=trimret;
							    }
							    continue;
						    }
					    }
					    else if (sds_wcmatch(elistll->grpptr->resval.rstring,"ELLIPSE"/*DNT*/)==RTNORM) {
						    //trim bulged section at ellipse
						    CEllipArc* pEllipse = asGeoEllipArc((db_ellipse*)elistll->ename[0]);
						    ASSERT(pEllipse != NULL);

						    C3Vector normal = pEllipse->MajorAxis() ^ pEllipse->MinorAxis();
						    normal.Normalized();
						    p5[0] = normal.X();
						    p5[1] = normal.Y();
						    p5[2] = normal.Z();

						    if (!icadPointEqual(p5,rbent.resval.rpoint)) {
							    notinucs++;
							    continue;
						    }

						    RC rc;
						    CEllipArc* pEllipSeg = new CEllipArc(C3Point(p0[0], p0[1], p0[2]), fr1, rc);
						    if (fr3 < fr2)
							    fr3 += TWOPI;
						    pEllipSeg->SetDomain(fr2, fr3);

						    // find intersections of selection set with elliptical arc
						    CRealArray onEllipSeg;
						    CRealArray onEllipse;
						    Intersect(pEllipSeg, pEllipse, onEllipSeg, onEllipse, INT_EPSILON);

						    delete pEllipse;

						    if (!onEllipSeg.Size()) { // no intersections
							    delete pEllipSeg;
							    continue;
						    }

						    C3Point xPoint;
						    pEllipSeg->Evaluate(onEllipSeg[0], xPoint);
						    p3[0] = xPoint.X(); p3[1] = xPoint.Y(); p3[2] = xPoint.Z();
						    if (onEllipSeg.Size() > 1) {
							    pEllipSeg->Evaluate(onEllipSeg[1], xPoint);
							    p4[0] = xPoint.X(); p4[1] = xPoint.Y(); p4[2] = xPoint.Z();
						    }
						    delete pEllipSeg;

						    trimret=cmd_adj_pline_trimpts(ename,p3,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_pline_dist);
						    if (trimret>-1)
							    trimends=trimret;
		    
						    if (onEllipSeg.Size() > 1) {
							    trimret=cmd_adj_pline_trimpts(ename,p4,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_pline_dist);
							    if (trimret>-1)
								    trimends=trimret;
						    }
						    continue;
					    }
					    else if (trimEntType==DB_SPLINE) {
						    CNURBS* pNurbs = asGeoSpline((db_spline*)elistll->ename[0]);
						    ASSERT(pNurbs != NULL);

						    RC rc;
						    CEllipArc* pArcSeg = new CEllipArc(fr1, C3Point(p0[0], p0[1], p0[2]), fr2, fr3, rc);
						    ASSERT(pArcSeg != NULL);

						    // find intersections of selection set with elliptical arc
						    CRealArray onArcSeg;
						    CRealArray onNurbs;
						    Intersect(pArcSeg, pNurbs, onArcSeg, onNurbs, INT_EPSILON);

						    delete pNurbs;

						    if (!onArcSeg.Size()) { // no intersections
							    delete pArcSeg;
							    continue;
						    }

						    // we need to identify the pick point is between which two intersection
						    // points.
						    //
						    C3Point xPoint;
						    for (register i = 0; i < onArcSeg.Size(); i++) {
							    pArcSeg->Evaluate(onArcSeg[i], xPoint);
							    p4[0] = xPoint.X(); p4[1] = xPoint.Y(); p4[2] = xPoint.Z();
								    
							    //check to see if p4 is closer than one of the trim points
							    trimret=cmd_adj_pline_trimpts(ename,p4,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_pline_dist);
							    if (trimret>-1)
								    trimends=trimret;
						    }
						    delete pArcSeg;
						    continue;
					    }
					    else if (trimEntType==DB_TEXT) {
						    ic_assoc(elistll->entlst,210);
						    if (!icadPointEqual(ic_rbassoc->resval.rpoint,rbent.resval.rpoint)) {
							    notinucs++;
							    continue;
						    }
						    //breaking a curved section at text...
						    if (cmd_get_textpts(elistll->entlst,p3,p4,p5,p6))continue;
						    fi1=ic_linexarc(p3,p4,p0,fr1,trimang1,trimang2,p7,p8);
						    if (IC_ARCINT_FIRSTINT_ON_BOTH(fi1)) {
							    trimret=cmd_adj_pline_trimpts(ename,p7,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_pline_dist);
							    if (trimret>-1)
								    trimends=trimret;
						    }
						    if (IC_ARCINT_SECONDINT_ON_BOTH(fi1)) {
							    trimret=cmd_adj_pline_trimpts(ename,p8,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_pline_dist);
							    if (trimret>-1)
								    trimends=trimret;
						    }
						    fi1=ic_linexarc(p4,p5,p0,fr1,trimang1,trimang2,p7,p8);
						    if (IC_ARCINT_FIRSTINT_ON_BOTH(fi1)) {
							    trimret=cmd_adj_pline_trimpts(ename,p7,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_pline_dist);
							    if (trimret>-1)
								    trimends=trimret;
						    }
						    if (IC_ARCINT_SECONDINT_ON_BOTH(fi1)) {
							    trimret=cmd_adj_pline_trimpts(ename,p8,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_pline_dist);
							    if (trimret>-1)
								    trimends=trimret;
						    }
						    fi1=ic_linexarc(p5,p6,p0,fr1,trimang1,trimang2,p7,p8);
						    if (IC_ARCINT_FIRSTINT_ON_BOTH(fi1)) {
							    trimret=cmd_adj_pline_trimpts(ename,p7,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_pline_dist);
							    if (trimret>-1)
								    trimends=trimret;
						    }
						    if (IC_ARCINT_SECONDINT_ON_BOTH(fi1)) {
							    trimret=cmd_adj_pline_trimpts(ename,p8,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_pline_dist);
							    if (trimret>-1)
								    trimends=trimret;
						    }
						    fi1=ic_linexarc(p6,p3,p0,fr1,trimang1,trimang2,p7,p8);
						    if (IC_ARCINT_FIRSTINT_ON_BOTH(fi1)) {
							    trimret=cmd_adj_pline_trimpts(ename,p7,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_pline_dist);
							    if (trimret>-1)
								    trimends=trimret;
						    }
						    if (IC_ARCINT_SECONDINT_ON_BOTH(fi1)) {
							    trimret=cmd_adj_pline_trimpts(ename,p8,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_pline_dist);
							    if (trimret>-1)
								    trimends=trimret;
						    }
					    }
					    else if (trimEntType==DB_POLYLINE) {
						    //********************************************
						    //breaking a bulged pline segment at a pline
						    //********************************************
						    ic_assoc(elistll->entlst,70);
						    closure2=ic_rbassoc->resval.rint;
						    if (closure2 & (IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH))
							    continue;
						    else if (0==projmode && !(closure2 & IC_PLINE_3DPLINE)) {
							    ic_assoc(elistll->entlst,10);
							    if (!icadRealEqual(ic_rbassoc->resval.rpoint[2],p8[2]))
								    continue;
						    }
						    if (closure2 & IC_PLINE_CLOSED)
							    elistll3=elistll->next;	//mark first vtx
						    for (fi3=0;;fi3++) {
							    elistll=elistll->next;
							    if (IC_TYPE_FROM_ENAME(elistll->ename)==DB_SEQEND)
								    break;
							    elistll2=elistll->next;
							    if (IC_TYPE_FROM_ENAME(elistll2->ename)==DB_SEQEND) {
								    if (!(closure2 & IC_PLINE_CLOSED))
									    break;
								    elistll2=elistll3;
							    }
							    else if (!(closure2 & IC_PLINE_CLOSED)) {
								    if (IC_TYPE_FROM_ENAME(elistll2->ename)==DB_SEQEND)
									    fi3=0;
							    }
                                if (closure & IC_PLINE_SPLINEFIT) {	//if splined pline, make sure we ignore ctrl pts
								    ic_assoc(elistll->entlst,70);
								    fi2=ic_rbassoc->resval.rint;
								    ic_assoc(elistll2->entlst,70);
								    if ((fi2 & IC_VERTEX_SPLINEFRAME) && (ic_rbassoc->resval.rint & IC_VERTEX_SPLINEFRAME))
									    break;
								    if ((fi2 & IC_VERTEX_SPLINEFRAME) || (ic_rbassoc->resval.rint & IC_VERTEX_SPLINEFRAME))
									    continue;
							    }
							    ic_assoc(elistll->entlst,10);
							    ic_ptcpy(p3,ic_rbassoc->resval.rpoint);
							    ic_assoc(elistll->entlst,42);
							    fr5=ic_rbassoc->resval.rreal;
							    ic_assoc(elistll2->entlst,10);
							    ic_ptcpy(p4,ic_rbassoc->resval.rpoint);
							    //we know p1 & p2 are in ecs, because we're trimming
							    //	a bulged section at this pline...

							    if (icadRealEqual(fr5,0.0)) {
								    if (closure2 & IC_PLINE_3DPLINE) {
									    sds_trans(p3,&rbwcs,&rbent,0,p3);
									    sds_trans(p4,&rbwcs,&rbent,0,p4);
								    }
								    //trim bulged pline section at straight pline segment
								    fi1=ic_linexarc(p3,p4,p0,fr1,fr2,fr3,p7,p8);
								    if (fi1==IC_LINEINT_UNDEF_OR_PARALLEL)
									    continue;
								    if (IC_ARCINT_FIRSTINT_ON_BOTH(fi1) || (fi3==0 && edgemode && !(fi1 & IC_ARCINT_FIRSTINT_ON_LINE) && (fi1 & IC_ARCINT_FIRSTINT_ON_ARC))) {
									    trimret=cmd_adj_pline_trimpts(ename,p7,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_pline_dist);
									    if (trimret>-1)
										    trimends=trimret;
								    }
								    if (IC_ARCINT_SECONDINT_ON_BOTH(fi1) || (fi3==0 && edgemode && !(fi1 & IC_ARCINT_SECONDINT_ON_LINE) && (fi1 & IC_ARCINT_SECONDINT_ON_ARC))){
									    trimret=cmd_adj_pline_trimpts(ename,p8,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_pline_dist);
									    if (trimret>-1)
										    trimends=trimret;
								    }
							    }
							    else {
								    //both segments MUST be ecs, so no transform req'd
								    //trim curved pline seg at curved pline segment
								    ic_bulge2arc(p3,p4,fr5,p5,&fr6,&fr7,&fr8);
								    fi1=ic_arcxarc(p0,fr1,fr2,fr3,p5,fr6,fr7,fr8,p7,p8);
								    if (fi1==IC_ARCARCINT_NOINTS)
									    continue;
								    if (IC_ARCARCINT_FIRSTINT_ON_BOTH(fi1) || (fi3==0 && edgemode && !(fi1 & IC_ARCARCINT_FIRSTINT_ON_ARC0) && (fi1 & IC_ARCARCINT_FIRSTINT_ON_ARC1))) {
									    trimret=cmd_adj_pline_trimpts(ename,p7,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_pline_dist);
									    if (trimret>-1)
										    trimends=trimret;
								    }
								    if (IC_ARCARCINT_SECONDINT_ON_BOTH(fi1) || (fi3==0 && edgemode && !(fi1 & IC_ARCARCINT_SECONDINT_ON_ARC0) && (fi1 & IC_ARCARCINT_SECONDINT_ON_ARC1))) {
									    trimret=cmd_adj_pline_trimpts(ename,p8,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_pline_dist);
									    if (trimret>-1)
										    trimends=trimret;
								    }
							    }
						    }	//step thru vertices of trimming edge pline
					    }
					    else if (trimEntType==DB_LWPOLYLINE) {
						    //**************************************
						    //trimming a bulged pline seg at an lwpline
						    //**************************************
						    db_lwpolyline *trimlwpl=(db_lwpolyline *)(elistll->ename[0]);
						    int vert1no,vert2no;
						    closure2=trimlwpl->ret_70();

						    //check elevations
						    if (0==projmode && !(closure & IC_PLINE_3DPLINE)) {
							    if (!icadRealEqual(trimlwpl->ret_38(),p0[2]))
								    continue;
						    }

						    for (fi3=0; fi3<trimlwpl->ret_90(); fi3++) {
							    vert1no=fi3;
							    vert2no=fi3+1;
							    //sds_entnext_noxref(etemp,etemp);
							    if (vert2no>=trimlwpl->ret_90()) {
								    if (!(closure2 & IC_PLINE_CLOSED))
									    break;
								    vert2no=0; //go back to 1st vtx if it's closed
							    }
							    else if (!(closure2 & IC_PLINE_CLOSED)) {
								    if (vert2no>=trimlwpl->ret_90()) // I don't see how this code can ever be reached
								       fi3=0;
							    }

							    trimlwpl->get_10(p3,vert1no);
							    trimlwpl->get_42(&fr5,vert1no);
							    trimlwpl->get_10(p4,vert2no);

							    //we know p1 & p2 are in ecs, because we're trimming
							    //	a bulged section at this pline...

							    if (icadRealEqual(fr5,0.0)) {
								    //trim bulged pline section at straight pline segment
								    fi1=ic_linexarc(p3,p4,p0,fr1,fr2,fr3,p7,p8);
								    if (fi1==IC_LINEINT_UNDEF_OR_PARALLEL)
									    continue;
								    if (IC_ARCINT_FIRSTINT_ON_BOTH(fi1) || (fi3==0 && edgemode && !(fi1 & IC_ARCINT_FIRSTINT_ON_LINE) && (fi1 & IC_ARCINT_FIRSTINT_ON_ARC))) {
									    trimret=cmd_adj_pline_trimpts(ename,p7,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_pline_dist);
									    if (trimret>-1)
										    trimends=trimret;
								    }
								    if (IC_ARCINT_SECONDINT_ON_BOTH(fi1) || (fi3==0 && edgemode && !(fi1 & IC_ARCINT_SECONDINT_ON_LINE) && (fi1 & IC_ARCINT_SECONDINT_ON_ARC))) {
									    trimret=cmd_adj_pline_trimpts(ename,p8,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_pline_dist);
									    if (trimret>-1)
										    trimends=trimret;
								    }
							    }
							    else {
								    //both segments MUST be ecs, so no transform req'd
								    //trim curved pline seg at curved pline segment
								    ic_bulge2arc(p3,p4,fr5,p5,&fr6,&fr7,&fr8);
								    fi1=ic_arcxarc(p0,fr1,fr2,fr3,p5,fr6,fr7,fr8,p7,p8);
								    if (fi1==IC_ARCARCINT_NOINTS)
									    continue;
								    if (IC_ARCARCINT_FIRSTINT_ON_BOTH(fi1) || (fi3==0 && edgemode && !(fi1 & IC_ARCARCINT_FIRSTINT_ON_ARC0) && (fi1 & IC_ARCARCINT_FIRSTINT_ON_ARC1))) {
									    trimret=cmd_adj_pline_trimpts(ename,p7,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_pline_dist);
									    if (trimret>-1)
										    trimends=trimret;
								    }
								    if (IC_ARCARCINT_SECONDINT_ON_BOTH(fi1) || (fi3==0 && edgemode && !(fi1 & IC_ARCARCINT_SECONDINT_ON_ARC0) && (fi1 & IC_ARCARCINT_SECONDINT_ON_ARC1))) {
									    trimret=cmd_adj_pline_trimpts(ename,p8,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_pline_dist);
									    if (trimret>-1)
										    trimends=trimret;
								    }
							    }
						    }	//step thru vertices of trimming edge pline
					    }
				    }
			    }
            } while (1);		//step thru pline vertices of entity to trim
        }
        else { // Projection
            CCompositeCurve* pGeoPolyline = asGeoCompositeCurve((db_polyline *)ename[0]);

            CAffine ucsXForm, iUcsXForm;
            if (!bUcsMatchWcs) {
                ucsXForm.Set(ucsOrigin, ucsXDir.AsPoint(), ucsYDir.AsPoint(), ucsZDir.AsPoint());
                iUcsXForm.SetInverse(ucsXForm);
                pGeoPolyline->Transform(iUcsXForm);
            }
		    C3Point pickPoint(epoint[0], epoint[1], epoint[2]);

            double rPickParam;
            bool bModified = false;
            if (pGeoPolyline->HitTest(pickPoint, ucsViewDir, HitTolerance, rPickParam)) {
                if (!bUcsMatchWcs) 
                    pGeoPolyline->Transform(ucsXForm);

                CDWGToGeo toGeo;
		        bool bExtend = edgemode == 0 ? false : true;
		        stdPPathList pathList;
		        db_handitem* pEntity;
		        // extract entities from selection set into Geo objects
		        for (elistll = bdry_setll; elistll != NULL; elistll = elistll->next) {
			        // if the spline to be trimmed is also selected as a cutting curve,
			        // we do not perform trim on the curve because AUTOCAD also does not 
                    // do it.
			        if (ename[0] == elistll->ename[0])
				        continue;

			        pEntity = (db_handitem*)elistll->ename[0];
			        toGeo.ExtractEntity(pEntity, pathList, bExtend, true);
		        }

                stdPPathList::const_iterator it;

			    CCompositeCurveTrimModifier modifier(pGeoPolyline, plane);
			    modifier.SetRefPoint(rPickParam);

		        CRealArray rootsEllipse, roots, roots1;
		        for (it = pathList.begin(); it != pathList.end(); ++it) {
			        for (int j = 0; j < (*it)->Size(); ++j) 
				        if (modifier.ModifiedWith((*it)->Curve(j)) && !bModified)
                            bModified = true;
		        }
		        for (it = pathList.begin(); it != pathList.end(); ++it)
			        delete (*it);

                if (bModified) {
                    double rAt1, rAt2;
                    if (modifier.GetCandidates(rAt1, rAt2)) {
                        C3Point ptTrim1, ptTrim2;
                        pGeoPolyline->Evaluate(rAt1, ptTrim1);
                        pGeoPolyline->Evaluate(rAt2, ptTrim2);
                        trimpt1[0] = ptTrim1.X(); 
                        trimpt1[1] = ptTrim1.Y(); 
                        trimpt1[2] = ptTrim1.Z(); 
                        trimpt2[0] = ptTrim2.X(); 
                        trimpt2[1] = ptTrim2.Y(); 
                        trimpt2[2] = ptTrim2.Z(); 
                    }
                    else
                        bModified = false;
                }
            }
            else
                delete pGeoPolyline;

            if (!bModified)
                goto exit;
        }
		//********************************
		//********************************
		//begin actual trimmming of pline given trimpt1 and trimpt2
		//********************************
		//********************************
		//if there are no crossings, exit
		if (icadPointEqual(trimpt1,plstart) && icadPointEqual(trimpt2,plend)) {
			cmd_ErrorPrompt(CMD_ERR_NOINTERSECT,0);
			goto exit;
		}


		if (icadPointEqual(trimpt1,p1))
			fl1=1;
		else {
			if (projmode && (closure & IC_PLINE_3DPLINE))
				sds_trans(trimpt1,&rbwcs,&rbucs,0,p5);
			else
				sds_trans(trimpt1,&rbent,&rbucs,0,p5);

			fl1=cmd_pl_vtx_no(ename,p5,0,&exactvert);
		}
		if ((closure & IC_PLINE_CLOSED) && icadPointEqual(trimpt2,plend))
			//if trimpt2 is at the end/start pt, it's vtx # is last vtx #, not 1
			fl2--; //set fl2 to last vertex #
		else {
			if (projmode && (closure & IC_PLINE_3DPLINE))
				sds_trans(trimpt2,&rbwcs,&rbucs,0,p6);
			else 
                sds_trans(trimpt2,&rbent,&rbucs,0,p6);
			fl2=cmd_pl_vtx_no(ename,p6,0,&exactvert);
		}
		ic_encpy(e1,ename);
		ic_encpy(e2,ename);
		for (fl3=0; fl3<fl1; sds_entnext_noxref(e1,e1),fl3++);
		for (fl3=0; fl3<fl2; sds_entnext_noxref(e2,e2),fl3++);

		ret=cmd_pline_breakup(ename,plstart,plend,closure,e1,trimpt1,false,e2,trimpt2,trimends,xtraent);
		if (ret<0)
			goto exit;
	} //end of pline trimming
	//********************************************************************************
	//**** Trim a Lightweight Polyline ***********************************************
	//********************************************************************************
	else if (trimeeType==DB_LWPOLYLINE) {
		db_lwpolyline *lwpl=(db_lwpolyline *)ename[0];

		bool closed;
		closure=lwpl->ret_70();
		closed=(closure & IC_PLINE_CLOSED);

		lwpl->get_10(plstart,0);
		lwpl->get_10(plend,lwpl->ret_90()-1);

        if (!projmode) {

		    //DEBUG NOTES FOR PLINE TRIMMING: We'll keep track of 2 trimpts, trimpt1 & trimpt2.
		    //We'll start w/them at the endpoints to indicate nothing to trim in that direction
		    //At 1st intersection in each direction from point picked, we'll reassign that
		    //trimpt, and then on subsequent intersections we'll try and "walk" the int as far
		    //away from the picked point as possible.
		    //Trimpt1 will ALWAYS be BEFORE Trimpt2 along the pline.  In nearly all cases the
		    //pickpoint is between the two.  The only time this WON'T be the case is if the pline
		    //is closed AND they're trimming off the two pline ends and keeping the middle.
		    //This will be identifiable by TRIMENDS, which will be 1 if the pickpt isn't between
		    //trimpt1 and trimpt2 on a closed pline
		    //NOTE: Special case:  If only ent in bdry_setll is the same polyline as the entity
		    //	selected, then we trim out the segment selected by the user.

            sds_point lwplextru;
		    lwpl->get_210(lwplextru);
		    if (!icadPointEqual(lwplextru,rbent.resval.rpoint)) {
			    cmd_ErrorPrompt(CMD_ERR_NOTINUCS,0);
			    ret=RTERROR;
			    goto exit;
		    }
		    sds_trans(epoint,&rbucs,&rbent,0,epoint);

		    cmd_elistll *firstvertptr;

		    int vert1no,vert2no;

		    vert1no=0;
		    p8[2]=lwpl->ret_38();
            //	ic_assoc(elist,10);
            //	ic_ptcpy(p8,ic_rbassoc->resval.rpoint);//we'll need to keep elevation -- BUT WHERE DO YOU USE THIS?

            //f'n doesn't get end if pline closed
		    ic_ptcpy(trimpt1,plstart);
		    if (closed)
			    ic_ptcpy(plend,plstart);
		    ic_ptcpy(trimpt2,plend);

		    for (fl2=0; fl2<lwpl->ret_90(); fl2++) {
			    vert1no=fl2;
			    vert2no=vert1no+1;
			    if (vert2no>=lwpl->ret_90()) {
				    if (!closed)
					    break;
				    else
					    vert2no=0;	// otherwise
			    }
			    lwpl->get_10(p1,vert1no);
			    lwpl->get_42(&fr1,vert1no);
			    lwpl->get_10(p2,vert2no);
			    if (icadRealEqual(fr1,0.0)) {
				    //****************************************
				    //check polyline at a straight segment against all entities....
				    //****************************************
				    //for (ssct=0L;sds_ssname(ss,ssct,etemp)==RTNORM;ssct++)
				    for (elistll=bdry_setll; elistll!=NULL; elistll=elistll->next) {
			            if (ename[0] == elistll->ename[0])
				            continue;
					    int trimEntType=IC_TYPE_FROM_ENAME(elistll->ename);
					    if (trimEntType==DB_LINE || trimEntType==DB_XLINE || trimEntType==DB_RAY) {
						    ic_assoc(elistll->entlst,10);
						    ic_ptcpy(p3,ic_rbassoc->resval.rpoint);
						    ic_assoc(elistll->entlst,11);
						    ic_ptcpy(p4,ic_rbassoc->resval.rpoint);
						    if (trimEntType!=DB_LINE) {
							    //for ray or xline, convert vector to point
							    ic_ptcpy(p8,p4);
							    p4[0]+=p3[0];
							    p4[1]+=p3[1];
							    p4[2]+=p3[2];
							    sds_trans(p8,&rbwcs,&rbent,1,p8);

						    }
						    //trimming a 2d pline, so change line's pts into ecs
						    sds_trans(p3,&rbwcs,&rbent,0,p3);
						    sds_trans(p4,&rbwcs,&rbent,0,p4);
						    if (0==projmode) {
							    fi1=sds_inters(p3,p4,p1,p2,0,p5);
							    if (fi1!=RTNORM)
								    fi1=IC_LINEINT_UNDEF_OR_PARALLEL;
							    else {
								    fi1=IC_LINEINT_ON_NEITHER_SEG;
								    if (1==ic_ptlndist(p5,p1,p2,NULL,p6))
									    fi1|=IC_LINEINT_ON_FIRST_SEG;
								    if (trimEntType==DB_XLINE)
									    fi1|=IC_LINEINT_ON_SECOND_SEG;
								    else if (trimEntType==DB_RAY && icadDirectionEqual(p3,p4,p3,p5))
									    fi1|=IC_LINEINT_ON_SECOND_SEG;
							    }
						    }
						    else
							    fi1=ic_linexline(p3,p4,p1,p2,p5);
                            if (fi1==IC_LINEINT_ON_SECOND_SEG && !edgemode)	{ //added !edgemode for speed
							    if (trimEntType==DB_XLINE)
								    fi1=(IC_LINEINT_ON_FIRST_SEG | IC_LINEINT_ON_SECOND_SEG);
							    if (trimEntType==DB_RAY) {
								    //if int is in correct direction, go ahead and accept it
								    if (fabs(p8[0])>fabs(p8[1])) {
									    if (fabs(p8[0])>fabs(p8[1]))
										    fi2=IC_LINEINT_ON_NEITHER_SEG;
									    else
										    fi2=IC_LINEINT_ON_FIRST_SEG;
								    }
								    else if (fabs(p8[1])>fabs(p8[2]))
									    fi2=IC_LINEINT_ON_FIRST_SEG;
								    else
									    fi2=IC_LINEINT_ON_SECOND_SEG;
								    if ((p8[fi2]*(p5[fi2]-p3[fi2]))>CMD_FUZZ)
									    fi1=(IC_LINEINT_ON_FIRST_SEG | IC_LINEINT_ON_SECOND_SEG);
							    }
						    }

						    if (edgemode && (fi1 >= IC_LINEINT_ON_SECOND_SEG)) {
							    trimret=cmd_adj_pline_trimpts(ename,p5,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_lwpline_dist);
							    if (trimret>-1)
								    trimends=trimret;
						    }
						    else if (!edgemode && IC_LINEINT_ON_BOTH_SEGS(fi1)) {
							    trimret=cmd_adj_pline_trimpts(ename,p5,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_lwpline_dist);
							    if (trimret>-1)
								    trimends=trimret;
						    }
					    }
					    else if (trimEntType==DB_ARC || trimEntType==DB_CIRCLE) {
						    //trim str pline seg at circle or arc
						    fr3=0.0;
						    fr4=IC_PI;
						    for (rbptemp=elistll->entlst; rbptemp!=NULL; rbptemp=rbptemp->rbnext) {
							    if (rbptemp->restype==10)
								    ic_ptcpy(p3,rbptemp->resval.rpoint);
							    else if (rbptemp->restype==40)
								    fr2=rbptemp->resval.rreal;
							    else if (rbptemp->restype==50)
								    fr3=rbptemp->resval.rreal;//not for circles
							    else if (rbptemp->restype==51)
								    fr4=rbptemp->resval.rreal;//not for circles
							    else if (rbptemp->restype==210)
								    ic_ptcpy(p4,rbptemp->resval.rpoint);
						    }
						    if (!icadPointEqual(rbent.resval.rpoint,p4)) {
							    notinucs++;
							    continue;
						    }
						    if (!icadRealEqual(p3[2],p0[2]))
							    continue;
						    ic_ptcpy(p6,p1);
						    ic_ptcpy(p7,p2);
						    fi1=ic_linexarc(p6,p7,p3,fr2,fr3,fr4,p4,p5);
						    if (fi1==IC_ARCINT_NOINTS)
							    continue;

						    fr5=p6[2]+(p7[2]-p6[2])*sds_dist2d(p6,p4)/sds_dist2d(p6,p7);
						    fr6=p6[2]+(p7[2]-p6[2])*sds_dist2d(p6,p5)/sds_dist2d(p6,p7);

						    if (!icadRealEqual(fr5,p3[2]))
							    fi1 &= ~(IC_ARCINT_FIRSTINT_ON_LINE | IC_ARCINT_FIRSTINT_ON_ARC);
						    if (!icadRealEqual(fr6,p3[2]))
							    fi1 &= ~(IC_ARCINT_SECONDINT_ON_LINE | IC_ARCINT_SECONDINT_ON_ARC);

						    if (edgemode || trimEntType==DB_CIRCLE) {
                                if (fi1 & IC_ARCINT_FIRSTINT_ON_LINE) { //if intersection is on the line
								    //check to see if p4 is closer than one of the trim points
								    trimret=cmd_adj_pline_trimpts(ename,p4,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_lwpline_dist);
								    if (trimret>-1)
									    trimends=trimret;
							    }
                                if (fi1 & IC_ARCINT_SECONDINT_ON_LINE) {	//if 2nd intersection is on the line
								    //check to see if p5 is closer than one of the trim points
								    trimret=cmd_adj_pline_trimpts(ename,p5,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_lwpline_dist);
								    if (trimret>-1)
									    trimends=trimret;
							    }
							    continue;
						    }
                            else {	//an arc and edgemode=0 so we're not extending it into a circle
                                if (IC_ARCINT_FIRSTINT_ON_BOTH(fi1)) { //if first intersections is on the line and arc both
								    //check to see if p4 is closer than one of the trim points
								    trimret=cmd_adj_pline_trimpts(ename,p4,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_lwpline_dist);
								    if (trimret>-1)
									    trimends=trimret;
							    }
                                if (IC_ARCINT_SECONDINT_ON_BOTH(fi1)) { //if 2nd intersection is on the line
								    //check to see if p5 is closer than one of the trim points
								    trimret=cmd_adj_pline_trimpts(ename,p5,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_lwpline_dist);
								    if (trimret>-1)
									    trimends=trimret;
							    }
							    continue;
						    }
					    }
					    else if (trimEntType==DB_ELLIPSE) {
						    //trim str pline seg at ellipse 
						    CEllipArc* pEllipse = asGeoEllipArc((db_ellipse*)elistll->ename[0]);
						    ASSERT(pEllipse != NULL);

						    if (0==projmode && !icadRealEqual(pEllipse->Center().Z(), p0[2])) {
							    delete pEllipse;
							    continue;
						    }

						    // To extend to ellipse when edgemode > 0
						    if (!pEllipse->Closed() && edgemode) 
							    pEllipse->SetDomain(0.0, TWOPI);

						    CLine* pLine = new CLine(C3Point(p1[0], p1[1], p1[2]), C3Point(p2[0], p2[1], p2[2]));
						    ASSERT(pLine != NULL);

						    // find intersections of selection set with elliptical arc
						    CRealArray onLine;
						    CRealArray onEllipse;
						    Intersect(pLine, pEllipse, onLine, onEllipse, INT_EPSILON);

						    delete pEllipse;

						    if (!onLine.Size()) { // no intersections
							    delete pLine;
							    continue;
						    }

						    C3Point xPoint;
						    pLine->Evaluate(onLine[0], xPoint);
						    p4[0] = xPoint.X(); p4[1] = xPoint.Y(); p4[2] = xPoint.Z();
						    if (onLine.Size() > 1) {
							    pLine->Evaluate(onLine[1], xPoint);
							    p5[0] = xPoint.X(); p5[1] = xPoint.Y(); p5[2] = xPoint.Z();
						    }
						    delete pLine;

						    //check to see if p4 is closer than one of the trim points
						    trimret=cmd_adj_pline_trimpts(ename,p4,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_lwpline_dist);
						    if (trimret>-1)
							    trimends=trimret;

						    if (onLine.Size() > 1) {	//if 2nd intersection is on the line
							    //check to see if p5 is closer than one of the trim points
							    trimret=cmd_adj_pline_trimpts(ename,p5,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_lwpline_dist);
							    if (trimret>-1)
								    trimends=trimret;
						    }
						    continue;
					    }
					    else if (trimEntType==DB_SPLINE) {
						    CNURBS* pNurbs = asGeoSpline((db_spline*)elistll->ename[0]);
						    ASSERT(pNurbs != NULL);

						    CLine* pLine = new CLine(C3Point(p1[0], p1[1], p1[2]), C3Point(p2[0], p2[1], p2[2]));
						    ASSERT(pLine != NULL);

						    // find intersections of selection set with elliptical arc
						    CRealArray onLine;
						    CRealArray onNurbs;
						    Intersect(pLine, pNurbs, onLine, onNurbs, INT_EPSILON);
						    delete pNurbs;

						    if (!onLine.Size()) { // no intersections
							    delete pLine;
							    continue;
						    }

						    // we need to identify the pick point is between which two intersection
						    // points.
						    //
						    C3Point xPoint;
						    for (register i = 0; i < onLine.Size(); i++) {
							    pLine->Evaluate(onLine[i], xPoint);
							    p4[0] = xPoint.X(); p4[1] = xPoint.Y(); p4[2] = xPoint.Z();
								    
							    //check to see if p4 is closer than one of the trim points
							    trimret=cmd_adj_pline_trimpts(ename,p4,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_lwpline_dist);
							    if (trimret>-1)
								    trimends=trimret;
						    }
						    delete pLine;
						    continue;
					    }
					    else if (trimEntType==DB_TEXT) {
						    ic_assoc(elistll->entlst,210);
						    if (!icadPointEqual(ic_rbassoc->resval.rpoint,rbent.resval.rpoint)) {
							    notinucs++;
							    continue;
						    }
						    if (!icadRealEqual(p3[2],p0[2]))
							    continue;
						    //trimming a straight segment at text...
						    if (cmd_get_textpts(elistll->entlst,p3,p4,p5,p6))
							    continue;

						    if (RTNORM==sds_inters(p3,p4,p1,p2,1,p7)) {
							    trimret=cmd_adj_pline_trimpts(ename,p7,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_lwpline_dist);
							    if (trimret>-1)
								    trimends=trimret;
						    }
					        if (RTNORM==sds_inters(p4,p5,p1,p2,1,p7)) {
							    trimret=cmd_adj_pline_trimpts(ename,p7,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_lwpline_dist);
							    if (trimret>-1)
								    trimends=trimret;
						    }
						    if (RTNORM==sds_inters(p5,p6,p1,p2,1,p7)) {
							    trimret=cmd_adj_pline_trimpts(ename,p7,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_lwpline_dist);
							    if (trimret>-1)
								    trimends=trimret;
						    }
						    if (RTNORM==sds_inters(p6,p3,p1,p2,1,p7)) {
							    trimret=cmd_adj_pline_trimpts(ename,p7,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_lwpline_dist);
							    if (trimret>-1)
								    trimends=trimret;
						    }
                        }
					    else if (trimEntType==DB_POLYLINE) {
						    //**************************************
						    //trimming a straight pline seg at a pline
						    //**************************************
						    ic_assoc(elistll->entlst,70);
						    closure2=ic_rbassoc->resval.rint;
						    if (closure2 & (IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH))
							    continue;
						    //check elevations
						    if (0==projmode && !(closure2 & IC_PLINE_3DPLINE))
							    {
							    ic_assoc(elistll->entlst,10);
							    if (!icadRealEqual(ic_rbassoc->resval.rpoint[2],p0[2]))
								    continue;
							    }

						    if (closure2 & IC_PLINE_CLOSED)
							    firstvertptr=elistll->next;	//mark first vtx
						    for (fi3=0;;fi3++) {
							    elistll=elistll->next;
							    if (IC_TYPE_FROM_ENAME(elistll->ename)==DB_SEQEND)
								    break;
							    elistll2=elistll->next;
							    if (IC_TYPE_FROM_ENAME(elistll2->ename)==DB_SEQEND) {
								    if (!(closure2 & IC_PLINE_CLOSED))
									    break;
								    elistll2=firstvertptr;
							    }
							    else if (!(closure2 & IC_PLINE_CLOSED)) {
								    if (IC_TYPE_FROM_ENAME(elistll2->ename)==DB_SEQEND)
									    fi3=0;
							    }
                                if (closure2 & IC_PLINE_SPLINEFIT) {	 //if splined pline, make sure we ignore ctrl pts
								    ic_assoc(elistll->entlst,70);
								    fi2=ic_rbassoc->resval.rint;
								    ic_assoc(elistll2->entlst,70);
								    if ((fi2 & IC_VERTEX_SPLINEFRAME) && (ic_rbassoc->resval.rint & IC_VERTEX_SPLINEFRAME))
									    break;
								    if ((fi2 & IC_VERTEX_SPLINEFRAME) || (ic_rbassoc->resval.rint & IC_VERTEX_SPLINEFRAME))
									    continue;
							    }
							    ic_assoc(elistll->entlst,10);
							    ic_ptcpy(p3,ic_rbassoc->resval.rpoint);
							    ic_assoc(elistll->entlst,42);
							    fr5=ic_rbassoc->resval.rreal;
							    ic_assoc(elistll2->entlst,10);
							    ic_ptcpy(p4,ic_rbassoc->resval.rpoint);
							    if (closure2 & IC_PLINE_3DPLINE) {
								    //if this one's 3d and selected one was 2d, use ecs
								    if (fabs(fr5)>CMD_FUZZ)
									    continue;
								    sds_trans(p3,&rbwcs,&rbent,0,p3);
								    sds_trans(p4,&rbwcs,&rbent,0,p4);
							    }
							    if (icadRealEqual(fr5,0.0)) {
								    fi2=ic_linexline(p3,p4,p1,p2,p5);
								    if (IC_LINEINT_ON_BOTH_SEGS(fi2) || (edgemode && fi3==0 && fi2==IC_LINEINT_ON_SECOND_SEG)) {
									    trimret=cmd_adj_pline_trimpts(ename,p5,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_lwpline_dist);
									    if (trimret>-1)
										    trimends=trimret;
								    }
							    }
							    else {
								    ic_ptcpy(p6,p1);
								    ic_ptcpy(p7,p2);
								    ic_bulge2arc(p3,p4,fr5,p5,&fr2,&fr3,&fr4);
								    fi1=ic_linexarc(p6,p7,p5,fr2,fr3,fr4,p3,p4);
								    if (fi1==IC_ARCINT_NOINTS)
									    continue;

								    fr5=p6[2]+(p7[2]-p6[2])*sds_dist2d(p6,p3)/sds_dist2d(p6,p7);
								    fr6=p6[2]+(p7[2]-p6[2])*sds_dist2d(p6,p4)/sds_dist2d(p6,p7);

								    if (!icadRealEqual(fr5,p5[2]))
									    fi1 &= ~(IC_ARCINT_FIRSTINT_ON_LINE | IC_ARCINT_FIRSTINT_ON_ARC);
								    if (!icadRealEqual(fr6,p5[2]))
									    fi1 &= ~(IC_ARCINT_SECONDINT_ON_LINE | IC_ARCINT_SECONDINT_ON_ARC);

								    if (IC_ARCINT_FIRSTINT_ON_BOTH(fi1) ||
									    (edgemode && fi3==0 && ((fi1 & IC_ARCINT_FIRSTINT_ON_ARC) && !(fi1 & IC_ARCINT_FIRSTINT_ON_LINE)))) {
									    trimret=cmd_adj_pline_trimpts(ename,p3,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_lwpline_dist);
									    if (trimret>-1)
										    trimends=trimret;
								    }
								    if (IC_ARCINT_SECONDINT_ON_BOTH(fi1) ||
									    (edgemode && fi3==0 && ((fi1 & IC_ARCINT_SECONDINT_ON_ARC)	&& !(fi1 &IC_ARCINT_SECONDINT_ON_LINE)))) {
									    trimret=cmd_adj_pline_trimpts(ename,p4,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_lwpline_dist);
									    if (trimret>-1)
										    trimends=trimret;
								    }
							    }
						    }//for loop on pline
					    }//polyline
					    else if (trimEntType==DB_LWPOLYLINE) {
						    //**************************************
						    //trimming a straight pline seg at an lwpline
						    //**************************************
						    db_lwpolyline *trimlwpl=(db_lwpolyline *)(elistll->ename[0]);
						    int vert1no,vert2no;
						    closure2=trimlwpl->ret_70();
						    bool selfTrimmed = (trimlwpl == lwpl);

						    //check elevations
						    if (0==projmode && !(closure & IC_PLINE_3DPLINE)) {
							    if (!icadRealEqual(trimlwpl->ret_38(),p0[2]))
								    continue;
						    }

						    for (fi3=0; fi3<trimlwpl->ret_90(); fi3++) {
							    vert1no=fi3;
							    vert2no=fi3+1;
							    //sds_entnext_noxref(etemp,etemp);
							    if (vert2no>=trimlwpl->ret_90()) {
								    if (!(closure2 & IC_PLINE_CLOSED))
									    break;
								    vert2no=0; //go back to 1st vtx if it's closed
							    }
							    else if (!(closure2 & IC_PLINE_CLOSED)) {
								    if (vert2no>=trimlwpl->ret_90()) // I don't see how this code can ever be reached
								       fi3=0;
							    }

							    trimlwpl->get_10(p3,vert1no);
							    trimlwpl->get_42(&fr5,vert1no);
							    trimlwpl->get_10(p4,vert2no);

							    if (icadRealEqual(fr5,0.0)) {
								    fi2=ic_linexline(p3,p4,p1,p2,p5);
								    if (IC_LINEINT_ON_BOTH_SEGS(fi2) || (edgemode && fi3==0 && fi2==IC_LINEINT_ON_SECOND_SEG)) {
									    trimret=cmd_adj_pline_trimpts(ename,p5,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_lwpline_dist);
									    if (trimret>-1)
										    trimends=trimret;
								    }
							    }
							    else {
								    ic_ptcpy(p6,p1);
								    ic_ptcpy(p7,p2);
								    ic_bulge2arc(p3,p4,fr5,p5,&fr2,&fr3,&fr4);
								    fi1=ic_linexarc(p6,p7,p5,fr2,fr3,fr4,p3,p4);
								    if (fi1==IC_ARCINT_NOINTS)
									    continue;

								    fr5=p6[2]+(p7[2]-p6[2])*sds_dist2d(p6,p3)/sds_dist2d(p6,p7);
								    fr6=p6[2]+(p7[2]-p6[2])*sds_dist2d(p6,p4)/sds_dist2d(p6,p7);

								    if (!icadRealEqual(fr5,p5[2]))
									    fi1 &= ~(IC_ARCINT_FIRSTINT_ON_LINE | IC_ARCINT_FIRSTINT_ON_ARC);
								    if (!icadRealEqual(fr6,p5[2]))
									    fi1 &= ~(IC_ARCINT_SECONDINT_ON_LINE | IC_ARCINT_SECONDINT_ON_ARC);


								    if (IC_ARCINT_FIRSTINT_ON_BOTH(fi1) ||
									    (edgemode && fi3==0 && ((fi1 & IC_ARCINT_FIRSTINT_ON_ARC) && !(fi1 & IC_ARCINT_FIRSTINT_ON_LINE)))) {
									    trimret=cmd_adj_pline_trimpts(ename,p3,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_lwpline_dist);
									    if (trimret>-1)
										    trimends=trimret;
								    }
								    if (IC_ARCINT_SECONDINT_ON_BOTH(fi1) ||
									    (edgemode && fi3==0 && ((fi1 & IC_ARCINT_SECONDINT_ON_ARC)	&& !(fi1 &IC_ARCINT_SECONDINT_ON_LINE)))) {
									    trimret=cmd_adj_pline_trimpts(ename,p4,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_lwpline_dist);
									    if (trimret>-1)
										    trimends=trimret;
								    }
							    }
						    }//for loop on pline
					    }//lwpolyline
				    } //for loop on selection set
			    }
			    else {	//fr1!=0 - a bulged section in pline
				    //check polyline at curved pline segment
				    //we KNOW that the pline is defined in ECS
				    //let p0 be ctr of arc & fr1,2,&3 be radius, start ang, & end angle
				    ic_bulge2arc(p1,p2,fr1,p3,&fr2,&fr3,&fr4);
				    ic_ptcpy(p0,p3);
				    fr1=fr2;
				    fr2=fr3;
				    fr3=fr4;
				    //for (ssct=0L;sds_ssname(ss,ssct,etemp)==RTNORM;ssct++)
				    for (elistll=bdry_setll; elistll!=NULL; elistll=elistll->next) {
					    int trimEntType=IC_TYPE_FROM_ENAME(elistll->ename);
					    if (trimEntType==DB_LINE || trimEntType==DB_XLINE || trimEntType==DB_RAY) {
						    //trim arc at line
						    ic_assoc(elistll->entlst,10);
						    ic_ptcpy(p3,ic_rbassoc->resval.rpoint);
						    ic_assoc(elistll->entlst,11);
						    ic_ptcpy(p4,ic_rbassoc->resval.rpoint);
						    if (trimEntType!=DB_LINE) {
							    //ray or xline
							    if (fabs(p4[0])>fabs(p4[1])) {
								    if (fabs(p4[0])>fabs(p4[1]))
									    fi3=0;
								    else
									    fi3=1;
							    }
							    else if (fabs(p4[1])>fabs(p4[2]))
								    fi3=1;
							    else
								    fi3=2;
							    ic_ptcpy(p8,p4);
							    p4[0]+=p3[0];
							    p4[1]+=p3[1];
							    p4[2]+=p3[2];
						    }
						    sds_trans(p3,&rbwcs,&rbent,0,p3);
						    sds_trans(p4,&rbwcs,&rbent,0,p4);
						    fi1=ic_linexarc(p3,p4,p0,fr1,fr2,fr3,p5,p6);

						    if (fi1==IC_ARCINT_NOINTS) {
							    //check 1st bdry condition - tangency
							    fi2=ic_ptlndist(p0,p3,p4,&fr5,p5);
							    if (!icadRealEqual(fr5,fr1))
								    continue;
							    //line approaches tangentially, but is tangent pt on line?
							    if (trimEntType==DB_XLINE)
								    fi1=(IC_ARCINT_FIRSTINT_ON_LINE | IC_ARCINT_FIRSTINT_ON_ARC);
							    else if (fi2==1 || icadPointEqual(p3,p5) || icadPointEqual(p4,p5))
								    fi1=(IC_ARCINT_FIRSTINT_ON_LINE | IC_ARCINT_FIRSTINT_ON_ARC);
							    else if (fi2==0 && trimEntType==DB_RAY) {
								    if (fabs(p8[0])>fabs(p8[1])) {
									    if (fabs(p8[0])>fabs(p8[1]))
										    fi2=0;
									    else
										    fi2=1;
								    }
								    else if (fabs(p8[1])>fabs(p8[2]))
									    fi2=1;
								    else
									    fi2=2;
								    if ((p8[fi2]*(p5[fi2]-p3[fi2]))>CMD_FUZZ)
									    fi1=(IC_ARCINT_FIRSTINT_ON_LINE | IC_ARCINT_FIRSTINT_ON_ARC);
							    }
							    else
								    continue;
						    }
						    else if (trimEntType!=DB_LINE) {
							    if (trimEntType==DB_XLINE)
								    fi1|=(IC_ARCINT_FIRSTINT_ON_LINE | IC_ARCINT_SECONDINT_ON_LINE);
							    else {
								    if ((p8[fi3]*(p5[fi3]-p3[fi3]))>CMD_FUZZ)
									    fi1 |= IC_ARCINT_FIRSTINT_ON_LINE;
								    if ((p8[fi3]*(p6[fi3]-p3[fi3]))>CMD_FUZZ)
									    fi1 |= IC_ARCINT_SECONDINT_ON_LINE;
							    }
						    }
						    else {
							    //check 2nd bdry condition
							    if (!IC_ARCINT_FIRSTINT_ON_BOTH(fi1)   && (icadPointEqual(p5,p3) || icadPointEqual(p5,p4)))
								    fi1 |= (IC_ARCINT_FIRSTINT_ON_LINE | IC_ARCINT_FIRSTINT_ON_ARC);
							    if (IC_ARCINT_SECONDINT_ON_BOTH(fi1) && (icadPointEqual(p6,p3) || icadPointEqual(p6,p4)))
								    fi1 |= (IC_ARCINT_SECONDINT_ON_LINE | IC_ARCINT_SECONDINT_ON_ARC);
						    }
						    if (!edgemode) {
							    if (IC_ARCINT_FIRSTINT_ON_BOTH(fi1)) {
								    trimret=cmd_adj_pline_trimpts(ename,p5,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_lwpline_dist);
								    if (trimret>-1)
									    trimends=trimret;
							    }
							    if (IC_ARCINT_SECONDINT_ON_BOTH(fi1)) {
								    trimret=cmd_adj_pline_trimpts(ename,p6,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_lwpline_dist);
								    if (trimret>-1)
									    trimends=trimret;
							    }
						    }
						    else {
							    if (fi1 & IC_ARCINT_FIRSTINT_ON_ARC) {
								    trimret=cmd_adj_pline_trimpts(ename,p5,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_lwpline_dist);
								    if (trimret>-1)
									    trimends=trimret;
							    }
							    if (fi1 & IC_ARCINT_SECONDINT_ON_ARC) {
								    trimret=cmd_adj_pline_trimpts(ename,p6,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_lwpline_dist);
								    if (trimret>-1)
									    trimends=trimret;
							    }
						    }
					    }
					    else if (sds_wcmatch(elistll->grpptr->resval.rstring,"CIRCLE,ARC"/*DNT*/)==RTNORM) {
						    //trim bulged section at circle or arc
						    fr6=0.0;
						    fr7=IC_PI;
						    for (rbptemp=elistll->entlst; rbptemp!=NULL; rbptemp=rbptemp->rbnext) {
							    if (rbptemp->restype==10)
								    ic_ptcpy(p1,rbptemp->resval.rpoint);
							    else if (rbptemp->restype==40)
								    fr5=rbptemp->resval.rreal;
							    else if (rbptemp->restype==50)
								    fr6=rbptemp->resval.rreal;//not for circles
							    else if (rbptemp->restype==51)
								    fr7=rbptemp->resval.rreal;//not for circles
							    else if (rbptemp->restype==210)
								    ic_ptcpy(p5,rbptemp->resval.rpoint);
						    }
						    if (!icadPointEqual(p5,rbent.resval.rpoint)) {
							    notinucs++;
							    continue;
						    }
						    if (projmode==0 && !icadRealEqual(p1[2],p8[2]))
							    continue;
						    fi1=ic_arcxarc(p0,fr1,fr2,fr3,p1,fr5,fr6,fr7,p3,p4);
						    if (fi1==IC_ARCARCINT_NOINTS)
							    continue;
						    if (trimEntType==DB_CIRCLE || edgemode) {
							    if (fi1 & IC_ARCARCINT_FIRSTINT_ON_ARC0) {
								    trimret=cmd_adj_pline_trimpts(ename,p3,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_lwpline_dist);
								    if (trimret>-1)
									    trimends=trimret;
							    }
							    if (fi1 & IC_ARCARCINT_SECONDINT_ON_ARC0) {
								    trimret=cmd_adj_pline_trimpts(ename,p4,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_lwpline_dist);
								    if (trimret>-1)
									    trimends=trimret;
							    }
							    continue;
						    }
						    else {
							    if (IC_ARCARCINT_FIRSTINT_ON_BOTH(fi1)) {
								    trimret=cmd_adj_pline_trimpts(ename,p3,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_lwpline_dist);
								    if (trimret>-1)
									    trimends=trimret;
							    }
							    if (IC_ARCARCINT_SECONDINT_ON_BOTH(fi1)) {
								    trimret=cmd_adj_pline_trimpts(ename,p4,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_lwpline_dist);
								    if (trimret>-1)
									    trimends=trimret;
							    }
							    continue;
						    }
					    }
					    else if (sds_wcmatch(elistll->grpptr->resval.rstring,"ELLIPSE"/*DNT*/)==RTNORM) {
						    //trim bulged section at ellipse
						    CEllipArc* pEllipse = asGeoEllipArc((db_ellipse*)elistll->ename[0]);
						    ASSERT(pEllipse != NULL);

						    C3Vector normal = pEllipse->MajorAxis() ^ pEllipse->MinorAxis();
						    normal.Normalized();
						    p5[0] = normal.X();
						    p5[1] = normal.Y();
						    p5[2] = normal.Z();

						    if (!icadPointEqual(p5,rbent.resval.rpoint)) {
							    notinucs++;
							    continue;
						    }

						    RC rc;
						    CEllipArc* pEllipSeg = new CEllipArc(C3Point(p0[0], p0[1], p0[2]), fr1, rc);
						    if (fr3 < fr2)
							    fr3 += TWOPI;
						    pEllipSeg->SetDomain(fr2, fr3);

						    // find intersections of selection set with elliptical arc
						    CRealArray onEllipSeg;
						    CRealArray onEllipse;
						    Intersect(pEllipSeg, pEllipse, onEllipSeg, onEllipse, INT_EPSILON);

						    delete pEllipse;

						    if (!onEllipSeg.Size()) { // no intersections
							    delete pEllipSeg;
							    continue;
						    }

						    C3Point xPoint;
						    pEllipSeg->Evaluate(onEllipSeg[0], xPoint);
						    p3[0] = xPoint.X(); p3[1] = xPoint.Y(); p3[2] = xPoint.Z();
						    if (onEllipSeg.Size() > 1) {
							    pEllipSeg->Evaluate(onEllipSeg[1], xPoint);
							    p4[0] = xPoint.X(); p4[1] = xPoint.Y(); p4[2] = xPoint.Z();
						    }
						    delete pEllipSeg;

						    trimret=cmd_adj_pline_trimpts(ename,p3,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_lwpline_dist);
						    if (trimret>-1)
							    trimends=trimret;
		    
						    if (onEllipSeg.Size() > 1) {
							    trimret=cmd_adj_pline_trimpts(ename,p4,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_lwpline_dist);
							    if (trimret>-1)
								    trimends=trimret;
						    }
						    continue;
					    }
					    else if (trimEntType==DB_SPLINE) {
						    CNURBS* pNurbs = asGeoSpline((db_spline*)elistll->ename[0]);
						    ASSERT(pNurbs != NULL);

						    RC rc;
						    CEllipArc* pArcSeg = new CEllipArc(fr1, C3Point(p0[0], p0[1], p0[2]), fr2, fr3, rc);
						    ASSERT(pArcSeg != NULL);

						    // find intersections of selection set with elliptical arc
						    CRealArray onArcSeg;
						    CRealArray onNurbs;
						    Intersect(pArcSeg, pNurbs, onArcSeg, onNurbs, INT_EPSILON);

						    delete pNurbs;

						    if (!onArcSeg.Size()) { // no intersections
							    delete pArcSeg;
							    continue;
						    }

						    // we need to identify the pick point is between which two intersection
						    // points.
						    //
						    C3Point xPoint;
						    for (register i = 0; i < onArcSeg.Size(); i++) {
							    pArcSeg->Evaluate(onArcSeg[i], xPoint);
							    p4[0] = xPoint.X(); p4[1] = xPoint.Y(); p4[2] = xPoint.Z();
								    
							    //check to see if p4 is closer than one of the trim points
							    trimret=cmd_adj_pline_trimpts(ename,p4,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_lwpline_dist);
							    if (trimret>-1)
								    trimends=trimret;
						    }
						    delete pArcSeg;
						    continue;
					    }
					    else if (trimEntType==DB_TEXT) {
						    ic_assoc(elistll->entlst,210);
						    if (!icadPointEqual(ic_rbassoc->resval.rpoint,rbent.resval.rpoint)) {
							    notinucs++;
							    continue;
						    }
						    //breaking a curved section at text...
						    if (cmd_get_textpts(elistll->entlst,p3,p4,p5,p6))continue;
						    fi1=ic_linexarc(p3,p4,p0,fr1,trimang1,trimang2,p7,p8);
						    if (IC_ARCINT_FIRSTINT_ON_BOTH(fi1)) {
							    trimret=cmd_adj_pline_trimpts(ename,p7,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_lwpline_dist);
							    if (trimret>-1)
								    trimends=trimret;
						    }
						    if (IC_ARCINT_SECONDINT_ON_BOTH(fi1)) {
							    trimret=cmd_adj_pline_trimpts(ename,p8,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_lwpline_dist);
							    if (trimret>-1)
								    trimends=trimret;
						    }
						    fi1=ic_linexarc(p4,p5,p0,fr1,trimang1,trimang2,p7,p8);
						    if (IC_ARCINT_FIRSTINT_ON_BOTH(fi1)) {
							    trimret=cmd_adj_pline_trimpts(ename,p7,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_lwpline_dist);
							    if (trimret>-1)
								    trimends=trimret;
						    }
						    if (IC_ARCINT_SECONDINT_ON_BOTH(fi1)) {
							    trimret=cmd_adj_pline_trimpts(ename,p8,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_lwpline_dist);
							    if (trimret>-1)
								    trimends=trimret;
						    }
						    fi1=ic_linexarc(p5,p6,p0,fr1,trimang1,trimang2,p7,p8);
						    if (IC_ARCINT_FIRSTINT_ON_BOTH(fi1)) {
							    trimret=cmd_adj_pline_trimpts(ename,p7,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_lwpline_dist);
							    if (trimret>-1)
								    trimends=trimret;
						    }
						    if (IC_ARCINT_SECONDINT_ON_BOTH(fi1)) {
							    trimret=cmd_adj_pline_trimpts(ename,p8,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_lwpline_dist);
							    if (trimret>-1)
								    trimends=trimret;
						    }
						    fi1=ic_linexarc(p6,p3,p0,fr1,trimang1,trimang2,p7,p8);
						    if (IC_ARCINT_FIRSTINT_ON_BOTH(fi1)) {
							    trimret=cmd_adj_pline_trimpts(ename,p7,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_lwpline_dist);
							    if (trimret>-1)
								    trimends=trimret;
						    }
						    if (IC_ARCINT_SECONDINT_ON_BOTH(fi1)) {
							    trimret=cmd_adj_pline_trimpts(ename,p8,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_lwpline_dist);
							    if (trimret>-1)
								    trimends=trimret;
						    }
					    }
					    else if (trimEntType==DB_POLYLINE) {
						    //********************************************
						    //breaking a bulged pline segment at a pline
						    //********************************************
						    ic_assoc(elistll->entlst,70);
						    closure2=ic_rbassoc->resval.rint;
						    if (closure2 & (IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH))
							    continue;
						    else if (0==projmode && !(closure2 & IC_PLINE_3DPLINE)) {
							    ic_assoc(elistll->entlst,10);
							    if (!icadRealEqual(ic_rbassoc->resval.rpoint[2],p8[2]))
								    continue;
						    }
						    if (closure2 & IC_PLINE_CLOSED)
							    firstvertptr=elistll->next;	//mark first vtx
						    for (fi3=0;;fi3++) {
							    elistll=elistll->next;
							    if (IC_TYPE_FROM_ENAME(elistll->ename)==DB_SEQEND)
								    break;
							    elistll2=elistll->next;
							    if (IC_TYPE_FROM_ENAME(elistll2->ename)==DB_SEQEND){
								    if (!(closure2 & IC_PLINE_CLOSED))
									    break;
								    elistll2=firstvertptr;
							    }
							    else if (!(closure2 & IC_PLINE_CLOSED)){
								    if (IC_TYPE_FROM_ENAME(elistll2->ename)==DB_SEQEND)  // I don't think this can ever be true
									    fi3=0;
							    }
							    ic_assoc(elistll->entlst,10);
							    ic_ptcpy(p3,ic_rbassoc->resval.rpoint);
							    ic_assoc(elistll->entlst,42);
							    fr5=ic_rbassoc->resval.rreal;
							    ic_assoc(elistll2->entlst,10);
							    ic_ptcpy(p4,ic_rbassoc->resval.rpoint);
							    //we know p1 & p2 are in ecs, because we're trimming
							    //	a bulged section at this pline...

							    if (icadRealEqual(fr5,0.0)) {
								    if (closure2 & IC_PLINE_3DPLINE) {
									    sds_trans(p3,&rbwcs,&rbent,0,p3);
									    sds_trans(p4,&rbwcs,&rbent,0,p4);
								    }
								    //trim bulged pline section at straight pline segment
								    fi1=ic_linexarc(p3,p4,p0,fr1,fr2,fr3,p7,p8);
								    if (fi1==IC_LINEINT_UNDEF_OR_PARALLEL)
									    continue;
								    if (IC_ARCINT_FIRSTINT_ON_BOTH(fi1) || (fi3==0 && edgemode && !(fi1 & IC_ARCINT_FIRSTINT_ON_LINE) && (fi1 & IC_ARCINT_FIRSTINT_ON_ARC))) {
									    trimret=cmd_adj_pline_trimpts(ename,p7,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_lwpline_dist);
									    if (trimret>-1)
										    trimends=trimret;
								    }
								    if (IC_ARCINT_SECONDINT_ON_BOTH(fi1) || (fi3==0 && edgemode && !(fi1 & IC_ARCINT_SECONDINT_ON_LINE) && (fi1 & IC_ARCINT_SECONDINT_ON_ARC))) {
									    trimret=cmd_adj_pline_trimpts(ename,p8,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_lwpline_dist);
									    if (trimret>-1)
										    trimends=trimret;
								    }
							    }
							    else {
								    //both segments MUST be ecs, so no transform req'd
								    //trim curved pline seg at curved pline segment
								    ic_bulge2arc(p3,p4,fr5,p5,&fr6,&fr7,&fr8);
								    fi1=ic_arcxarc(p0,fr1,fr2,fr3,p5,fr6,fr7,fr8,p7,p8);
								    if (fi1==IC_ARCARCINT_NOINTS)
									    continue;
								    if (IC_ARCARCINT_FIRSTINT_ON_BOTH(fi1) || (fi3==0 && edgemode && !(fi1 & IC_ARCARCINT_FIRSTINT_ON_ARC0) && (fi1 & IC_ARCARCINT_FIRSTINT_ON_ARC1))) {
									    trimret=cmd_adj_pline_trimpts(ename,p7,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_lwpline_dist);
									    if (trimret>-1)
										    trimends=trimret;
								    }
								    if (IC_ARCARCINT_SECONDINT_ON_BOTH(fi1) || (fi3==0 && edgemode && !(fi1 & IC_ARCARCINT_SECONDINT_ON_ARC0) && (fi1 & IC_ARCARCINT_SECONDINT_ON_ARC1))) {
									    trimret=cmd_adj_pline_trimpts(ename,p8,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_lwpline_dist);
									    if (trimret>-1)
										    trimends=trimret;
								    }
							    }
						    }	//step thru vertices of trimming edge pline
					    }
					    else if (trimEntType==DB_LWPOLYLINE) {
						    //**************************************
						    //trimming a bulged pline seg at an lwpline
						    //**************************************
						    db_lwpolyline *trimlwpl=(db_lwpolyline *)(elistll->ename[0]);
						    int vert1no,vert2no;
						    closure2=trimlwpl->ret_70();

						    //check elevations
						    if (!(closure & IC_PLINE_3DPLINE)) {
							    if (!icadRealEqual(trimlwpl->ret_38(),p0[2]))
								    continue;
						    }

						    for (fi3=0; fi3<trimlwpl->ret_90(); fi3++) {
							    vert1no=fi3;
							    vert2no=fi3+1;
							    //sds_entnext_noxref(etemp,etemp);
							    if (vert2no>=trimlwpl->ret_90()) {
								    if (!(closure2 & IC_PLINE_CLOSED))
									    break;
								    vert2no=0; //go back to 1st vtx if it's closed
							    }
							    else if (!(closure2 & IC_PLINE_CLOSED)) {
								    if (vert2no>=trimlwpl->ret_90()) // I don't see how this code can ever be reached
								       fi3=0;
							    }

							    trimlwpl->get_10(p3,vert1no);
							    trimlwpl->get_42(&fr5,vert1no);
							    trimlwpl->get_10(p4,vert2no);

							    if (icadRealEqual(fr5,0.0)) {
								    //trim bulged pline section at straight pline segment
								    fi1=ic_linexarc(p3,p4,p0,fr1,fr2,fr3,p7,p8);
								    if (fi1==IC_LINEINT_UNDEF_OR_PARALLEL)
									    continue;
								    if (IC_ARCINT_FIRSTINT_ON_BOTH(fi1) || (fi3==0 && edgemode && !(fi1 & IC_ARCINT_FIRSTINT_ON_LINE) && (fi1 & IC_ARCINT_FIRSTINT_ON_ARC))) {
									    trimret=cmd_adj_pline_trimpts(ename,p7,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_lwpline_dist);
									    if (trimret>-1)
										    trimends=trimret;
								    }
								    if (IC_ARCINT_SECONDINT_ON_BOTH(fi1) || (fi3==0 && edgemode && !(fi1 & IC_ARCINT_SECONDINT_ON_LINE) && (fi1 & IC_ARCINT_SECONDINT_ON_ARC))) {
									    trimret=cmd_adj_pline_trimpts(ename,p8,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_lwpline_dist);
									    if (trimret>-1)
										    trimends=trimret;
								    }
							    }
							    else {
								    //both segments MUST be ecs, so no transform req'd
								    //trim curved pline seg at curved pline segment
								    ic_bulge2arc(p3,p4,fr5,p5,&fr6,&fr7,&fr8);
								    fi1=ic_arcxarc(p0,fr1,fr2,fr3,p5,fr6,fr7,fr8,p7,p8);
								    if (fi1==IC_ARCARCINT_NOINTS)
									    continue;
								    if (IC_ARCARCINT_FIRSTINT_ON_BOTH(fi1) || (fi3==0 && edgemode && !(fi1 & IC_ARCARCINT_FIRSTINT_ON_ARC0) && (fi1 & IC_ARCARCINT_FIRSTINT_ON_ARC1))) {
									    trimret=cmd_adj_pline_trimpts(ename,p7,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_lwpline_dist);
									    if (trimret>-1)
										    trimends=trimret;
								    }
								    if (IC_ARCARCINT_SECONDINT_ON_BOTH(fi1) || (fi3==0 && edgemode && !(fi1 & IC_ARCARCINT_SECONDINT_ON_ARC0) && (fi1 & IC_ARCARCINT_SECONDINT_ON_ARC1))) {
									    trimret=cmd_adj_pline_trimpts(ename,p8,epoint,trimpt1,trimpt2,closure,plstart,plend,cmd_lwpline_dist);
									    if (trimret>-1)
										    trimends=trimret;
								    }
							    }
						    }	//step thru vertices of trimming edge pline
					    }
				    }
			    }
		    }
        }
        else { // Projection mode
            CCompositeCurve* pGeoPolyline = asGeoCompositeCurve(lwpl);

            CAffine ucsXForm, iUcsXForm;
            if (!bUcsMatchWcs) {
                ucsXForm.Set(ucsOrigin, ucsXDir.AsPoint(), ucsYDir.AsPoint(), ucsZDir.AsPoint());
                iUcsXForm.SetInverse(ucsXForm);
                pGeoPolyline->Transform(iUcsXForm);
            }
		    C3Point pickPoint(epoint[0], epoint[1], epoint[2]);

            double rPickParam;
            bool bModified = false;
            if (pGeoPolyline->HitTest(pickPoint, ucsViewDir, HitTolerance, rPickParam)) {
                if (!bUcsMatchWcs) 
                    pGeoPolyline->Transform(ucsXForm);

                CDWGToGeo toGeo;
		        bool bExtend = edgemode == 0 ? false : true;
		        stdPPathList pathList;
		        db_handitem* pEntity;
		        // extract entities from selection set into Geo objects
		        for (elistll = bdry_setll; elistll != NULL; elistll = elistll->next) {
			        // if the spline to be trimmed is also selected as a cutting curve,
			        // we do not perform trim on the curve because AUTOCAD also does not 
                    // do it.
			        if (ename[0] == elistll->ename[0])
				        continue;

			        pEntity = (db_handitem*)elistll->ename[0];
			        toGeo.ExtractEntity(pEntity, pathList, bExtend, true);
		        }

                stdPPathList::const_iterator it;

			    CCompositeCurveTrimModifier modifier(pGeoPolyline, plane);
			    modifier.SetRefPoint(rPickParam);

		        CRealArray rootsEllipse, roots, roots1;
		        for (it = pathList.begin(); it != pathList.end(); ++it) {
			        for (int j = 0; j < (*it)->Size(); ++j) 
				        if (modifier.ModifiedWith((*it)->Curve(j)) && !bModified)
                            bModified = true;
		        }
		        for (it = pathList.begin(); it != pathList.end(); ++it)
			        delete (*it);

                if (bModified) {
                    double rAt1, rAt2;
                    if (modifier.GetCandidates(rAt1, rAt2)) {
                        C3Point ptTrim1, ptTrim2;
                        pGeoPolyline->Evaluate(rAt1, ptTrim1);
                        pGeoPolyline->Evaluate(rAt2, ptTrim2);
                        trimpt1[0] = ptTrim1.X(); 
                        trimpt1[1] = ptTrim1.Y(); 
                        trimpt1[2] = ptTrim1.Z(); 
                        trimpt2[0] = ptTrim2.X(); 
                        trimpt2[1] = ptTrim2.Y(); 
                        trimpt2[2] = ptTrim2.Z(); 
                    }
                    else
                        bModified = false;
                }
            }
            else
                delete pGeoPolyline;

            if (!bModified)
                goto exit;
        }
		//********************************
		//********************************
		//begin actual trimmming of pline given trimpt1 and trimpt2
		//********************************
		//********************************
		//if there are no crossings, exit
		if (icadPointEqual(trimpt1,plstart) && icadPointEqual(trimpt2,plend)) {
			cmd_ErrorPrompt(CMD_ERR_NOINTERSECT,0);
			goto exit;
		}


		if (icadPointEqual(trimpt1,p1)) {
			fl1 = (closed) ? lwpl->ret_90() - 1 : lwpl->ret_90() - 2;
		}
		else {
            if (!projmode)
			    sds_trans(trimpt1,&rbent,&rbucs,0,p5);
            else
                sds_trans(trimpt1,&rbwcs,&rbucs,0,p5);

			fl1 = cmd_lwpl_vtx_no(ename,p5,0,&exactvert);
		}
		if (icadPointEqual(trimpt2,plend)) {
			if (closed)
			//if trimpt2 is at the end/start pt, it's vtx # is last vtx #, not 1
				fl2 = 0;
			else
				fl2 = lwpl->ret_90() - 1;
		}
		else {
            if (!projmode)
			    sds_trans(trimpt2,&rbent,&rbucs,0,p6);
            else
                sds_trans(trimpt2,&rbwcs,&rbucs,0,p6);
			fl2 = cmd_lwpl_vtx_no(ename,p6,0,&exactvert);
		}

		if (fl1 > fl2) {
			if (closed) {
				long index = fl1;
				fl1 = fl2;
				fl2 = index;

				sds_point point;
				ic_ptcpy(point, trimpt1);
				ic_ptcpy(trimpt1, trimpt2);
				ic_ptcpy(trimpt2, point);

				trimends = 1;
			}
			else
				goto exit;
		}

		ret=cmd_lwpline_breakup(ename,plstart,plend,closure,fl1,trimpt1, false,fl2,trimpt2,trimends,xtraent);
		if (ret<0)
			goto exit;
    } //end of lwpline trimming
	else if(trimeeType == DB_ELLIPSE) {
		CDWGToGeo toGeo;
		bool bExtend = edgemode == 0 ? false : true;
		stdPPathList pathList;
		db_handitem* pEntity;

		// extract entities from selection set into Geo objects
		for (elistll = bdry_setll; elistll != NULL; elistll = elistll->next) {
			if (ename[0] == elistll->ename[0])
				continue;

			pEntity = (db_handitem*)elistll->ename[0];
			toGeo.ExtractEntity(pEntity, pathList, bExtend, true);
		}

		CEllipArc* pEllipse = asGeoEllipArc((db_ellipse*)ename[0]);
		bool isClosed = icadIsZero(pEllipse->Start() + TWOPI - pEllipse->End(), EPSILON);

        CEllipArc* pUcsEllipse = new CEllipArc(*pEllipse);
        if (pUcsEllipse->Transform(w2uXForm) != SUCCESS) {
            delete pUcsEllipse;
            delete pEllipse;
            QUIT
        }

		double pointParam;
        pUcsEllipse->HitTest(C3Point(epoint[0], epoint[1], epoint[2]), ucsViewDir, HitTolerance, pointParam);
        delete pUcsEllipse;

        
        stdPPathList::const_iterator it;
		sds_real trimInitialParam, trimFinalParam;
		CRealArray rootsEllipse, roots, roots1;
        bool bModified = false;
        if (!projmode) {
		    // find intersections of selection set with elliptical arc
		    int rAt;
		    double xsect;
		    for (it = pathList.begin(); it != pathList.end(); ++it) {
			    for (int j = 0; j < (*it)->Size(); ++j) {
				    Intersect(pEllipse, (*it)->Curve(j), roots1, roots, INT_EPSILON);

				    for (register j = 0; j < roots1.Size(); j++) {
					    xsect = roots1[j];
					    rootsEllipse.AddSort(xsect, EPSILON, rAt);
				    }
			    }
		    }
            if (!isClosed && rootsEllipse.Size() > 0 || isClosed && rootsEllipse.Size() > 1) {
			    // find the intersection points that are th closest to the pick point
			    //
			    trimInitialParam = pEllipse->Start();
			    trimFinalParam = pEllipse->End();
			    for (int i = 0; i < rootsEllipse.Size(); ++i) {
                    if (trimInitialParam < rootsEllipse[i] && rootsEllipse[i] < pointParam) {
					    trimInitialParam = rootsEllipse[i];
                        bModified = true;
                    }
                    if (trimFinalParam > rootsEllipse[i] && rootsEllipse[i] > pointParam) {
					    trimFinalParam = rootsEllipse[i];
                        bModified = true;
                    }
			    }
		        ASSERT(trimFinalParam >= trimInitialParam);
			    if (!icadRealEqual(trimInitialParam, pEllipse->Start(), EPSILON) ||
				    !icadRealEqual(trimFinalParam, pEllipse->End(), EPSILON)) {
				    if (isClosed) {
					    // trim full ellipse
					    if (icadRealEqual(trimInitialParam, pEllipse->Start(), EPSILON)) {
						    if (icadRealEqual(rootsEllipse[0], pEllipse->Start(), EPSILON)) // intersect at the start point
							    trimInitialParam = pEllipse->End();
						    else
							    trimInitialParam = rootsEllipse[rootsEllipse.Size() - 1];
					    }
					    else if (icadRealEqual(trimFinalParam, pEllipse->End(), EPSILON)) {
						    if (icadRealEqual(rootsEllipse[rootsEllipse.Size() - 1], pEllipse->End(), EPSILON)) // intersect at the end point
							    trimFinalParam = pEllipse->Start();
						    else
							    trimFinalParam = rootsEllipse[0];
					    }
					    else {
						    trimInitialParam = pEllipse->End() + trimInitialParam - pEllipse->Start();
					    }
                    }
                }
            }
        }
        else {
			CCurveTrimModifier modifier(pEllipse, plane);
			modifier.SetRefPoint(pointParam);

		    CRealArray rootsEllipse, roots, roots1;
		    for (it = pathList.begin(); it != pathList.end(); ++it) {
			    for (int j = 0; j < (*it)->Size(); ++j) 
				    if (modifier.ModifiedWith((*it)->Curve(j)) && !bModified)
                        bModified = true;
		    }
            if (bModified) {
                modifier.GetCandidates(trimInitialParam, trimFinalParam);
                if (trimInitialParam < 0.0) {
                    trimInitialParam += TWOPI;
                    trimFinalParam += TWOPI;
                }
            }

        }
		for (it = pathList.begin(); it != pathList.end(); ++it) 
			 delete (*it);

		if (bModified) {
			ic_assoc(elist,41);
			struct resbuf* pRb41 = ic_rbassoc;
			ic_assoc(elist,42);
			struct resbuf* pRb42 = ic_rbassoc;

			if (!icadRealEqual(trimInitialParam, pEllipse->Start(), EPSILON) ||
				!icadRealEqual(trimFinalParam, pEllipse->End(), EPSILON)) {
				if (isClosed) {
					pRb41->resval.rreal = trimFinalParam;
					pRb42->resval.rreal = trimInitialParam;
				}
				else {
					// trim elliptical arc
					if (icadRealEqual(trimInitialParam, pEllipse->Start(), EPSILON))
						pRb41->resval.rreal = trimFinalParam;
					else {
						if (icadRealEqual(trimFinalParam, pEllipse->End(), EPSILON))
							pRb42->resval.rreal = trimInitialParam;
						else {
							// create one more elliptical arc
							struct resbuf* elist1 = sds_entget(ename);
							    if (elist1 != NULL) {
							    ic_assoc(elist1, 41);
							    ic_rbassoc->resval.rreal = trimFinalParam;
							    if (sds_entmake(elist1) == RTNORM) 
                                    pRb42->resval.rreal = trimInitialParam;
                                else
								    ret = -1;

				                sds_entlast(xtraent);
							    FreeResbufIfNotNull(&elist1);
                            }
                            else 
                                ret = -1;
						}
					}
				}
                if (ret != -1)
				    sds_entmod(elist);
			}
		}
        if (pEllipse) 
            delete pEllipse;
	}
	else if (trimeeType == DB_SPLINE) {
		CNURBS* pSpline = asGeoSpline((db_spline*)ename[0]);
        if (pSpline->Transform(w2uXForm) != SUCCESS) {
            delete pSpline;
            QUIT
        }

		C3Point pickPoint(epoint[0], epoint[1], epoint[2]);

        sds_real pointParam; 
		CRealArray rootsSpline;
        // NOTE: for some reasons the compiler doesn't like if we just write pSpline->HitTest()
        // so we have to write it as pSpline->CCurve::HitTest().    SWH 7/22/2004
        if (pSpline->CCurve::HitTest(pickPoint, ucsViewDir, HitTolerance, pointParam)) { 
            pSpline->Transform(u2wXForm);

		    CDWGToGeo toGeo;
		    bool bExtend = edgemode == 0 ? false : true;
		    stdPPathList pathList;
		    db_handitem* pEntity;
		    // extract entities from selection set into Geo objects
		    for (elistll = bdry_setll; elistll != NULL; elistll = elistll->next) {
			    // if the spline to be trimmed is also selected as a cutting curve,
			    // we do not perform trim on the curve because AUTOCAD also does not 
                // do it.
			    if (ename[0] == elistll->ename[0])
				    continue;

			    pEntity = (db_handitem*)elistll->ename[0];
			    toGeo.ExtractEntity(pEntity, pathList, bExtend, true);
		    }

            stdPPathList::const_iterator it;
/*
            if (!projmode) { // trim the curve in 3D world space.
		        // find intersections of selection set with spline
		        int rAt;
                CRealArray roots, roots1;
		        for (it = pathList.begin(); it != pathList.end(); ++it) {
			        for (int j = 0; j < (*it)->Size(); ++j) {
				        Intersect(pSpline, (*it)->Curve(j), roots1, roots, INT_EPSILON);
				        for (register j = 0; j < roots1.Size(); j++)
					        rootsSpline.AddSort(roots1[j], EPSILON, rAt);
			        }
		        }
            }
            else {
*/
                CCurveTrimModifier modifier(pSpline, projmode ? plane : *(CPlane*)NULL);
			    modifier.SetRefPoint(pointParam);

                bool bModified = false;
		        for (it = pathList.begin(); it != pathList.end(); ++it) {
			        for (int j = 0; j < (*it)->Size(); ++j) 
				        if (modifier.ModifiedWith((*it)->Curve(j)) && !bModified)
                            bModified = true;
		        }
                if (bModified) 
                    modifier.GetCandidatePoints(rootsSpline);
//            }

		    for (it = pathList.begin(); it != pathList.end(); ++it)
			    delete (*it);
        }

		if (rootsSpline.Size() > 0) {
			rootsSpline.Sort();
			// ASSERT(fabs(pointSplineDistance) < EPSILON); 

			CNURBS* pNurbs1 = NULL, *pNurbs2 = NULL;
			CCurve* pCrv1 = NULL, *pCrv2 = NULL;
			double param1, param2;
			if (pSpline->Closed()) {
				if (rootsSpline.Size() > 1) {
					param1 = rootsSpline[0];
					param2 = rootsSpline[rootsSpline.Size() - 1];
					if (pointParam < param1 || pointParam > param2) 
						pSpline->Subset(param1, param2, *(PCurve*)&pNurbs1);
					else {
						bool found = false;
						for (register i = 0; i < rootsSpline.Size() - 1; i++) {
							param1 = rootsSpline[i];
							param2 = rootsSpline[i + 1];
							if (pointParam > param1 && pointParam < param2) {
								found = true;
								break;
							}
						}
						if (found) {
							if (pSpline->Split(param1, BOTH, *(PCurve*)&pNurbs1, *(PCurve*)&pNurbs2) != SUCCESS) {
								sds_redraw(ename, IC_REDRAW_UNHILITE);
								ret=(-__LINE__);
								goto exit;
							}
							RC rc = pNurbs2->Split(param2, RIGHT, pCrv1, pCrv2);
							delete pNurbs2;
							if (rc == SUCCESS) {
								pNurbs2 = static_cast<CNURBS*>(pCrv2);
								pCrv2 = NULL;
								if (pNurbs1->Reverse() != SUCCESS || 
									pNurbs2->Reverse() != SUCCESS || 
									pNurbs1->JoinWith(pNurbs2) != SUCCESS) {
									delete pNurbs1; pNurbs1 = NULL;
									delete pNurbs2; pNurbs2 = NULL;
									sds_redraw(ename, IC_REDRAW_UNHILITE);
									ret=(-__LINE__);
									goto exit;
								}
							}
							else {
								sds_redraw(ename, IC_REDRAW_UNHILITE);
								ret=(-__LINE__);
								goto exit;
							}
						}
					}
				}
			}
			else {
				if (pointParam < rootsSpline[0]) {
					if (pSpline->Split(rootsSpline[0], RIGHT, pCrv1, pCrv2) != SUCCESS) {
						sds_redraw(ename, IC_REDRAW_UNHILITE);
						ret=(-__LINE__);
						goto exit;
					}
					pNurbs1 = static_cast<CNURBS*>(pCrv2);
				}
				else if (pointParam > rootsSpline[rootsSpline.Size() - 1]) {
					if (pSpline->Split(rootsSpline[rootsSpline.Size() - 1], LEFT, pCrv1, pCrv2) != SUCCESS) {
						sds_redraw(ename, IC_REDRAW_UNHILITE);
						ret=(-__LINE__);
						goto exit;
					}
					pNurbs1 = static_cast<CNURBS*>(pCrv1);
				}
				else {
					bool found = false;
					for (register i = 0; i < rootsSpline.Size() - 1; i++) {
						param1 = rootsSpline[i]; 
						param2 = rootsSpline[i + 1];
						if (pointParam > param1 && pointParam < param2) {
							found = true;
							break;
						}
					}
					if (!found) {
						sds_redraw(ename, IC_REDRAW_UNHILITE);
						ret=(-__LINE__);
						goto exit;
					}

					if (pSpline->Split(param1, BOTH, pCrv1, pCrv2) != SUCCESS) {
						sds_redraw(ename, IC_REDRAW_UNHILITE);
						ret=(-__LINE__);
						goto exit;
					}
					pNurbs1 = static_cast<CNURBS*>(pCrv1);
					pNurbs2 = static_cast<CNURBS*>(pCrv2);

					if (pSpline->Split(param2, RIGHT, pCrv1, pCrv2) != SUCCESS) {
						sds_redraw(ename, IC_REDRAW_UNHILITE);
						ret=(-__LINE__);
						goto exit;
					}
					delete pNurbs2;
					pNurbs2 = static_cast<CNURBS*>(pCrv2);
				}
			}
            if (pSpline) {
                delete pSpline;
                pSpline = NULL;
            }

			struct resbuf *rbSpline = NULL;
			if (pNurbs1) {
				ic_assoc(elist,RTDXF0);
				if (BuildSplineList(pNurbs1, &rbSpline) != SUCCESS ||
					CopyAttributes(elist, rbSpline) != SUCCESS) {
					delete pNurbs1;
					cmd_ErrorPrompt(CMD_ERR_CREATEENT,0);	//so create new one
					ret=(-__LINE__);
					goto exit;
				}
				delete pNurbs1;

				SWAP(ic_rbassoc->rbnext, rbSpline->rbnext, struct resbuf*);
				if (sds_entmod(elist)!=RTNORM) {
					cmd_ErrorPrompt(CMD_ERR_CREATEENT,0);	//so create new one
					ret=(-__LINE__);
					goto exit;
				}
				if (rbSpline) {
					sds_relrb(rbSpline);
					rbSpline = NULL;
				}
			}
			if (pNurbs2) {
				if (BuildSplineList(pNurbs2, &rbSpline) != SUCCESS ||
					CopyAttributes(elist, rbSpline) != SUCCESS) {
					delete pNurbs2;
					cmd_ErrorPrompt(CMD_ERR_CREATEENT,0);	//so create new one
					ret=(-__LINE__);
					goto exit;
				}
				delete pNurbs2;

				if (sds_entmake(rbSpline)!=RTNORM) {
					cmd_ErrorPrompt(CMD_ERR_CREATEENT,0);	//so create new one
					ret=(-__LINE__);
					goto exit;
				}
				sds_entlast(xtraent);
				if (rbSpline) 
					sds_relrb(rbSpline);
			}
		}
	}

exit:
	if (ret>0 && notinucs>0) {
		sds_printf(ResourceString(IDC_CMDS4__N_I_BOUNDARY_EN_261, "\n%i boundary entities were not in the current ucs." ),notinucs);
	}
	FreeResbufIfNotNull(&rbp1);
	FreeResbufIfNotNull(&rbp2);
	FreeResbufIfNotNull(&elist);
	return(ret);
}


int cmd_pline_breakup(sds_name thepline, sds_point plstart, sds_point plend, int closure, sds_name vert1, sds_point trimpt1, bool specialcaseflag,
					  sds_name vert2, sds_point trimpt2, int trimret, sds_name xtraent)
{
	// There are 3 cases  to do:
	// CASE 1:	SPECIAL CASE!  Closed pline and we're trimming off both ends.
	//				This case prohibits the other two.
	// CASE 2:	trim pt 2 is not at end of polyline, so we need to make a new pline end piece
	// CASE 3:	trim pt 1 is not at begin of pline, so we need to make a new pline begin piece
	// NOTE: SPECIAL CASE if specialcaseflag==-1, just trim out vertex vert1 & leave the rest of the pline intact
	//			This will fall thru to one of the other 3 cases
	// NOTE: In cases 2 and 3, if the polyline was formerly closed, these two plines will be
	//			joined into ONE!
	//All pts must be in same coord system as pline. No transforms done here!!

	sds_real bulge,newbulge,junkreal,startangle,endangle,startwidth,endwidth;
	struct resbuf *rbp1,*rbp2,*rbptemp;
	sds_name etemp,redraw_ent;
	sds_point p1,p2,center;
	int ret=0,entdel_flag=0,fi3;

	rbp1=rbp2=rbptemp=NULL;

	redraw_ent[0]=redraw_ent[1]=0L;
	xtraent[0]=xtraent[1]=0;

	//Make header with 70 flag=0;
	sds_entnext_noxref(thepline,etemp);
	if (cmd_make_pline_start(thepline,etemp)!=0)
		{
		ret=(-__LINE__);
		goto exit;
		}

	//***********SPECIAL CASE**************
	//Do math for this case 1st and let it fall thru to which of the other 3 applies
	if (specialcaseflag)
		{
		FreeResbufIfNotNull(&rbp1);
		rbp1=sds_entget(vert1);
		ic_assoc(rbp1,10);
		ic_ptcpy(trimpt1,ic_rbassoc->resval.rpoint);
		//we don't know what the next ent is, so let's get it
		cmd_pline_entnext(vert1,vert2,closure);
		FreeResbufIfNotNull(&rbp2);
		rbp2=sds_entget(vert2);
		ic_assoc(rbp2,10);
		ic_ptcpy(trimpt2,ic_rbassoc->resval.rpoint);
		trimret=0;
		}


	//***********CASE 1***************
	if ((closure & 1) && trimret==1)
		{
		//modify vertex info for trimpt1 and make trimpoint vertex
		FreeResbufIfNotNull(&rbp1);
		rbp1=sds_entget(vert1);
		ic_assoc(rbp1,42);
		bulge=ic_rbassoc->resval.rreal;
		ic_assoc(rbp1,10);
		ic_ptcpy(p1,ic_rbassoc->resval.rpoint);
		//copy trimpt2 to beginning of segment
		ic_ptcpy(ic_rbassoc->resval.rpoint,trimpt1);
		cmd_pline_entnext(vert1,etemp,closure);
		FreeResbufIfNotNull(&rbp2);
		rbp2=sds_entget(etemp);
		ic_assoc(rbp2,10);
		ic_ptcpy(p2,ic_rbassoc->resval.rpoint);
		//setup vars to calculate width.  startwidth=start width, endwidth=end width
		if (ic_assoc(rbp1,40)!=0)//if there's no starting width present, make one
			{
			rbptemp=rbp1;
			if (rbptemp->rbnext!=NULL)
				for (; rbptemp->rbnext!=NULL; rbptemp=rbptemp->rbnext);
			if ((rbptemp->rbnext=sds_buildlist(40,0.0,0))==NULL)
				{
				ret=(-__LINE__);
				goto exit;
				}
			rbptemp=NULL;
			startwidth=0.0;
			}
		else
			startwidth=ic_rbassoc->resval.rreal;
		if (ic_assoc(rbp1,41)!=0)
			endwidth=0.0;
		else
			endwidth=ic_rbassoc->resval.rreal;
		if (!icadRealEqual(bulge,0.0))		//if segment is curved, calculate new bulge & starting width
			{
			//if ((bulge==IC_TWOPI)||(bulge==-IC_TWOPI))bulge=bulge*0.99999999;
			//get a counter-clockwise arc to define the center point
			ic_bulge2arc(p1,p2,bulge,center,&junkreal,&startangle,&endangle);
			if (startangle>sds_angle(center,trimpt1))
				startangle=startangle-IC_TWOPI;
			if (endangle<sds_angle(center,trimpt1))
				endangle=endangle+IC_TWOPI;
			if (bulge>0.0)	//counter-clockwise pline
				{
				//update bulge
				newbulge=tan((endangle-sds_angle(center,trimpt1))/4.0);
				ic_assoc(rbp1,42);
				ic_rbassoc->resval.rreal=newbulge;
				//update width.  startwidth=start width, endwidth=end width
				ic_assoc(rbp1,40);
				ic_rbassoc->resval.rreal=startwidth+((endwidth-startwidth)*(endangle-sds_angle(center,trimpt1))/(endangle-startangle));
				}
			else
				{	//clockwise pline
				//update bulge
				newbulge=tan((startangle-sds_angle(center,trimpt1))/4.0);
				ic_assoc(rbp1,42);
				ic_rbassoc->resval.rreal=newbulge;
				//update width
				ic_assoc(rbp1,40);
				ic_rbassoc->resval.rreal=startwidth+((endwidth-startwidth)*(sds_angle(center,trimpt1)-startangle)/(endangle-startangle));
				}
			}
		else
			{	//straight segment - just update starting width
			ic_assoc(rbp1,40);
			ic_rbassoc->resval.rreal=startwidth+((endwidth-startwidth)*sds_distance(p2,trimpt2)/sds_distance(p1,p2));
			}
		//Now, geometry for trimpt1 is updated in vert1.  If vert1!=vert2,
		// geometry complete and we can make vert1 & all ents up to BUT
		//NOT INCLUDING vert2
		if ((vert1[0]!=vert2[0]) || (vert1[1]!=vert2[1]))
			{
			if (sds_entmake(rbp1)!=RTNORM)
				{
				cmd_ErrorPrompt(CMD_ERR_CREATEENT,0);
		 		ret=(-__LINE__);
				}
			sds_entnext_noxref(vert1,etemp);
			while ((etemp[0]!=vert2[0])||(etemp[1]!=vert2[1]))
				{
				FreeResbufIfNotNull(&rbp1);
				if ((rbp1=sds_entget(etemp))==NULL)
					{
					ret=(-__LINE__);
					goto exit;
					}
				if ((ret=sds_entmake(rbp1))!=RTNORM)
					goto exit;
				sds_entnext_noxref(etemp,etemp);
				}
			FreeResbufIfNotNull(&rbp1);
			if ((rbp1=sds_entget(vert2))==NULL)
				{
				ret=(-__LINE__);
				goto exit;
				}
			}
		//Now rbp1 has correct beginning geometry for vert2....


		ic_assoc(rbp1,42);
		bulge=ic_rbassoc->resval.rreal;
		ic_assoc(rbp1,10);
		ic_ptcpy(p1,ic_rbassoc->resval.rpoint);
		//NOTE:  don't copy trimpt2 to rbp1 - we'll make that point as the new last vertex
		//get point of next vertex
		cmd_pline_entnext(vert2,etemp,closure);
		FreeResbufIfNotNull(&rbp2);
		rbp2=sds_entget(etemp);
		ic_assoc(rbp2,10);
		ic_ptcpy(p2,ic_rbassoc->resval.rpoint);
		//setup data for calculating widths just like in case 1 above
		if (ic_assoc(rbp1,41)!=0)//if there's no ending width present, make one
			{
			rbptemp=rbp1;
			if (rbptemp->rbnext!=NULL)
				for (; rbptemp->rbnext!=NULL; rbptemp=rbptemp->rbnext);
			if ((rbptemp->rbnext=sds_buildlist(41,0.0,0))==NULL)
				{
				ret=(-__LINE__);
				goto exit;
				}
			rbptemp=NULL;
			endwidth=0.0;
			}
		else
			endwidth=ic_rbassoc->resval.rreal;
		if (ic_assoc(rbp1,40)!=0)
			startwidth=0.0;
		else
			startwidth=ic_rbassoc->resval.rreal;
		if (bulge!=0)		//if segment is curved, calculate new bulge & ending width
			{
			//if ((bulge==IC_TWOPI)||(bulge==-IC_TWOPI))bulge=bulge*0.99999999;
			//get a counter-clockwise arc to define the center point
			ic_bulge2arc(p1,p2,bulge,center,&junkreal,&startangle,&endangle);
			if (startangle>sds_angle(center,trimpt2))
				startangle=startangle-IC_TWOPI;
			if (endangle<sds_angle(center,trimpt2))
				endangle=endangle+IC_TWOPI;
			if (bulge>0)  //counter-clockwise pline
				{ //update bulge
				newbulge=tan((sds_angle(center,trimpt2)-startangle)/4.0);
				ic_assoc(rbp1,42);
				ic_rbassoc->resval.rreal=newbulge;
				//update width.  startwidth=start width, endwidth=end width
				ic_assoc(rbp1,41);
				ic_rbassoc->resval.rreal=startwidth+((endwidth-startwidth)*(sds_angle(center,trimpt2)-startangle)/(endangle-startangle));
				}
			else	//clockwise pline
				{
				//update bulge
				newbulge=tan((sds_angle(center,trimpt2)-endangle)/4.0);
				ic_assoc(rbp1,42);
				ic_rbassoc->resval.rreal=newbulge;
				//update width
				ic_assoc(rbp1,41);
				ic_rbassoc->resval.rreal=startwidth+((endwidth-startwidth)*(sds_angle(center,trimpt2)-endangle)/(startangle-endangle));
				}
			}
		else	//straight segment - just update starting width
			{
			ic_assoc(rbp1,41);
			ic_rbassoc->resval.rreal=startwidth+((endwidth-startwidth)*sds_distance(p1,trimpt2)/sds_distance(p1,p2));
			}
		if (sds_entmake(rbp1)!=RTNORM)
			{
			ret=(-__LINE__);
			goto exit;
			}
		FreeResbufIfNotNull(&rbp1);
		if ((rbp1=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,10,trimpt2,0))==NULL)
			{
			ret=(-__LINE__);
			goto exit;
			}
		if (sds_entmake(rbp1)!=RTNORM)
			{
			ret=(-__LINE__);
			goto exit;
			}
		FreeResbufIfNotNull(&rbp1);
		if ((rbp1=sds_buildlist(RTDXF0,"SEQEND"/*DNT*/,0))==NULL)
			{
			ret=(-__LINE__);
			goto exit;
			}
		if (sds_entmake(rbp1)==RTNORM)
			{
			sds_entdel(thepline);
			sds_entlast(thepline);
			sds_redraw(thepline,IC_REDRAW_DRAW);
			}
		goto exit;	//an exclusive possibility
		}
	//***********CASE 2***************
	if (!icadPointEqual(plend,trimpt2))
		{
		//modify vertex info for trimpt2 and make trimpoint vertex
		FreeResbufIfNotNull(&rbp1);
		rbp1=sds_entget(vert2);
		ic_assoc(rbp1,42);
		bulge=ic_rbassoc->resval.rreal;
		ic_assoc(rbp1,10);
		ic_ptcpy(p1,ic_rbassoc->resval.rpoint);
		//copy trimpt2 to beginning of segment
		ic_ptcpy(ic_rbassoc->resval.rpoint,trimpt2);
		cmd_pline_entnext(vert2,etemp,closure);
		FreeResbufIfNotNull(&rbp2);
		rbp2=sds_entget(etemp);
		ic_assoc(rbp2,10);
		ic_ptcpy(p2,ic_rbassoc->resval.rpoint);
		//setup vars to calculate width.  startwidth=start width, endwidth=end width
		if (ic_assoc(rbp1,40)!=0) //if there's no starting width present, make one
			{
			rbptemp=rbp1;
			if (rbptemp->rbnext!=NULL)
				for (; rbptemp->rbnext!=NULL; rbptemp=rbptemp->rbnext);
			if ((rbptemp->rbnext=sds_buildlist(40,0.0,0))==NULL)
				{
				ret=(-__LINE__);
				goto exit;
				}
			rbptemp=NULL;
			startwidth=0.0;
			}
		else
			startwidth=ic_rbassoc->resval.rreal;
		if (ic_assoc(rbp1,41)!=0)
			endwidth=0.0;
		else
			endwidth=ic_rbassoc->resval.rreal;
		if (!icadRealEqual(bulge,0.0))		//if segment is curved, calculate new bulge & starting width
			{
			//if ((bulge==IC_TWOPI)||(bulge==-IC_TWOPI))bulge=bulge*0.99999999;
			//get a counter-clockwise arc to define the center point
			ic_bulge2arc(p1,p2,bulge,center,&junkreal,&startangle,&endangle);
			if (startangle>sds_angle(center,trimpt2))
				startangle=startangle-IC_TWOPI;
			if (endangle<sds_angle(center,trimpt2))
				endangle=endangle+IC_TWOPI;
			if (bulge>0.0)	//counter-clockwise pline
				{
				//update bulge
				newbulge=tan((endangle-sds_angle(center,trimpt2))/4.0);
				ic_assoc(rbp1,42);
				ic_rbassoc->resval.rreal=newbulge;
				//update width.  startwidth=start width, endwidth=end width
				ic_assoc(rbp1,40);
				ic_rbassoc->resval.rreal=startwidth+((endwidth-startwidth)*(endangle-sds_angle(center,trimpt2))/(endangle-startangle));
				}
			else	//clockwise pline
				{  //update bulge
				newbulge=tan((startangle-sds_angle(center,trimpt2))/4.0);
				ic_assoc(rbp1,42);
				ic_rbassoc->resval.rreal=newbulge;
				//update width
				ic_assoc(rbp1,40);
				ic_rbassoc->resval.rreal=startwidth+((endwidth-startwidth)*(sds_angle(center,trimpt2)-startangle)/(endangle-startangle));
				}
			}
		else	//straight segment - just update starting width
			{
			ic_assoc(rbp1,40);
			ic_rbassoc->resval.rreal=startwidth+((endwidth-startwidth)*sds_distance(p1,trimpt2)/sds_distance(p1,p2));
			}
		if (sds_entmake(rbp1)!=RTNORM)
			{
			cmd_ErrorPrompt(CMD_ERR_CREATEENT,0);
		 	ret=(-__LINE__);
			}
		//make the rest of the entity if it wasn't closed
		if (!(closure & IC_PLINE_CLOSED))
			{
			fi3=cmd_make_pline_end(vert2,0,p1); //pt1 is dummy - closure flag=0
			}
		else
			{
			fi3=1;
			FreeResbufIfNotNull(&rbp1);
			ic_encpy(etemp,vert2);
			do
				{
				if (rbp1!=NULL)
					{
					if ((ret=sds_entmake(rbp1))!=RTNORM)goto exit;
					sds_relrb(rbp1);
					rbp1=NULL;
					}
				sds_entnext_noxref(etemp,etemp);
				if ((rbp1=sds_entget(etemp))==NULL)
					{
					ret=(-__LINE__);
					goto exit;
					}
				ic_assoc(rbp1,0);
				}
			while (strsame(ic_rbassoc->resval.rstring,"VERTEX"/*DNT*/ ));
			}
		if (fi3==0)
			{
			entdel_flag=1;
			sds_entlast(redraw_ent);
			}
		else if (fi3<0)
		 	ret=(-__LINE__);
		FreeResbufIfNotNull(&rbp1);
		FreeResbufIfNotNull(&rbp2);
		if ((closure&1) && icadPointEqual(plstart,trimpt1))
			{
			//just make a vtx at the start/end point
			sds_entnext_noxref(thepline,etemp);
			if ((rbp1=sds_entget(etemp))==NULL)
				{
				ret=(-__LINE__);
				goto exit;
				}
			if ((ret=sds_entmake(rbp1))!=RTNORM)
				goto exit;
			FreeResbufIfNotNull(&rbptemp);
			if ((rbptemp=sds_buildlist(RTDXF0,"SEQEND"/*DNT*/,0))==NULL)
				{
				ret=(-__LINE__);
				goto exit;
				}
			if (sds_entmake(rbptemp)==RTNORM)
				{
				entdel_flag=1;
				}
			else
				ret=(-__LINE__);
			sds_relrb(rbptemp);
			rbptemp=NULL;
			}
		}
	//***********CASE 3***************
	if (!icadPointEqual(plstart,trimpt1))
		{
		//in this case we just have to entmod the first break vertex & then make a new seqend
		if (closure & 1)
			{
			//make everything up to BUT NOT INCLUDING vert1
			sds_entnext_noxref(thepline,etemp);
			while ((etemp[0]!=vert1[0])||(etemp[1]!=vert1[1]))
				{
				FreeResbufIfNotNull(&rbp1);
				if ((rbp1=sds_entget(etemp))==NULL)
					{
					ret=(-__LINE__);
					goto exit;
					}
				if ((ret=sds_entmake(rbp1))!=RTNORM)
					goto exit;
				cmd_pline_entnext(etemp,etemp,closure);
				}
			}
		else
			{
			//stop what we were making (next f'n does entmake(NULL) )and
			//make the header ent & everything up to BUT NOT INCLUDING vert1
			if (-1==cmd_make_pline_start(thepline,vert1))
				goto exit;
			}
		FreeResbufIfNotNull(&rbp1);
		rbp1=sds_entget(vert1);
		ic_assoc(rbp1,10);
		ic_ptcpy(p1,ic_rbassoc->resval.rpoint);
		if (icadPointEqual(trimpt1,p1))
			{
			if (sds_entmake(rbp1)!=RTNORM)
				{
				ret=(-__LINE__);
				goto exit;
				}
			}
		else
			{
			//calc new widths & bulge to make pline end at trimpt1

			//NOTE:  don't copy trimpt1 to rbp1 - we'll make that point as the new last vertex
			//get point of next vertex
			ic_assoc(rbp1,42);
			bulge=ic_rbassoc->resval.rreal;
			cmd_pline_entnext(vert1,etemp,closure);
			FreeResbufIfNotNull(&rbp2);
			rbp2=sds_entget(etemp);
			ic_assoc(rbp2,10);
			ic_ptcpy(p2,ic_rbassoc->resval.rpoint);
			//setup data for calculating widths just like in case 1 above
			if (ic_assoc(rbp1,41)!=0)//if there's no ending width present, make one
				{
				rbptemp=rbp1;
				if (rbptemp->rbnext!=NULL)
					for (; rbptemp->rbnext!=NULL; rbptemp=rbptemp->rbnext);
				if ((rbptemp->rbnext=sds_buildlist(41,0.0,0))==NULL)
					{
					ret=(-__LINE__);
					goto exit;
					}
				rbptemp=NULL;
				endwidth=0.0;
				}
			else
				endwidth=ic_rbassoc->resval.rreal;
			if (ic_assoc(rbp1,40)!=0)
				startwidth=0.0;
			else
				startwidth=ic_rbassoc->resval.rreal;
			if (bulge!=0)		//if segment is curved, calculate new bulge & ending width
				{
				//if ((bulge==IC_TWOPI)||(bulge==-IC_TWOPI))bulge=bulge*0.99999999;
				//get a counter-clockwise arc to define the center point
				ic_bulge2arc(p1,p2,bulge,center,&junkreal,&startangle,&endangle);
				if (startangle>sds_angle(center,trimpt1))
					startangle=startangle-IC_TWOPI;
				if (endangle<sds_angle(center,trimpt1))
					endangle=endangle+IC_TWOPI;
				if (bulge>0)  //counter-clockwise pline
					{ //update bulge
					newbulge=tan((sds_angle(center,trimpt1)-startangle)/4.0);
					ic_assoc(rbp1,42);
					ic_rbassoc->resval.rreal=newbulge;
					//update width.  startwidth=start width, endwidth=end width
					ic_assoc(rbp1,41);
					ic_rbassoc->resval.rreal=startwidth+((endwidth-startwidth)*(sds_angle(center,trimpt1)-startangle)/(endangle-startangle));
					}
				else	//clockwise pline
					{
					//update bulge
					newbulge=tan((sds_angle(center,trimpt1)-endangle)/4.0);
					ic_assoc(rbp1,42);
					ic_rbassoc->resval.rreal=newbulge;
					//update width
					ic_assoc(rbp1,41);
					ic_rbassoc->resval.rreal=startwidth+((endwidth-startwidth)*(sds_angle(center,trimpt1)-endangle)/(startangle-endangle));
					}
				}
			else
				{	//straight segment - just update starting width
				ic_assoc(rbp1,41);
				ic_rbassoc->resval.rreal=startwidth+((endwidth-startwidth)*sds_distance(p1,trimpt1)/sds_distance(p1,p2));
				}
			if (sds_entmake(rbp1)!=RTNORM)
				{
				ret=(-__LINE__);
				goto exit;
				}

			//RBPTEMP not freed in exit routine
			if ((rbptemp=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,10,trimpt1,0))==NULL)
				{
				ret=(-__LINE__);
				goto exit;
				}
			if (sds_entmake(rbptemp)!=RTNORM)
				{
				ret=(-__LINE__);
				goto exit;
				}
			sds_relrb(rbptemp);
			rbptemp=NULL;
			}
		if ((rbptemp=sds_buildlist(RTDXF0,"SEQEND"/*DNT*/,0))==NULL)
			{
			ret=(-__LINE__);
			goto exit;
			}
		if (sds_entmake(rbptemp)==RTNORM)
			{
			entdel_flag=1;
			}
		else
			ret=(-__LINE__);
		sds_relrb(rbptemp);
		rbptemp=NULL;
		}
	if (entdel_flag)
		{
		sds_entdel(thepline);
		sds_entlast(thepline);
		}
	sds_redraw(thepline,IC_REDRAW_DRAW);
	if (redraw_ent[1]||redraw_ent[0])
		{
		sds_redraw(redraw_ent,IC_REDRAW_DRAW);
		ic_encpy(xtraent,redraw_ent);
		}


	exit:
	FreeResbufIfNotNull(&rbp1);
	FreeResbufIfNotNull(&rbp2);
	return(ret);
}



int cmd_lwpline_breakup(sds_name thelwpline, sds_point plstart, sds_point plend,
						int closure, int pt1vertno, sds_point trimpt1, bool specialcaseflag,
						int pt2vertno, sds_point trimpt2, int trimret, sds_name xtraent)
	{

	// There are 3 cases to do:
	// CASE 1:	SPECIAL CASE!  Closed pline and we're trimming off both ends.
	//				This case prohibits the other two.
	// CASE 2:	trim pt 2 is not at end of polyline, so we need to make a new pline end piece
	// CASE 3:	trim pt 1 is not at begin of pline, so we need to make a new pline begin piece
	// NOTE: In cases 2 and 3, if the polyline was formerly closed, these two plines will be
	//			joined into ONE!
	//All pts must be in same coord system as pline. No transforms done here!!

	sds_real bulge,junkreal,startangle,endangle,startwidth,endwidth;
	sds_point p1,p2,center,tpt;
	int ret=0,nextvert,prevvert;
	resbuf *newlwpl;
	resbuf rb,*lwpladdrb;
	rb.restype=RTSTR;
	rb.resval.rstring="*"/*DNT*/;
	rb.rbnext=NULL;
	db_lwpolyline *lwpl;
	resbuf *rbEdd = NULL;
	xtraent[0]=xtraent[1]=0;

	newlwpl=sds_entgetx(thelwpline,&rb);	// create new lwpline resbuf to build on
	lwpladdrb=newlwpl;
	while (lwpladdrb->rbnext != NULL)
	{
		if ( lwpladdrb->rbnext->restype == -3 )
			break;
		lwpladdrb=lwpladdrb->rbnext;
	}
	if ( lwpladdrb->rbnext != NULL )
	{
		rbEdd = lwpladdrb->rbnext;
		lwpladdrb->rbnext = NULL;
	}

	lwpl=new db_lwpolyline(*((db_lwpolyline *)(thelwpline[0])));	// a copy for us to use to build the new one
	// delete all of the vertex data
	LwplrbDeleteVertexData(newlwpl);
	lwpladdrb=newlwpl;
	while (lwpladdrb->rbnext!=NULL)
		{
		if (lwpladdrb->restype==70)
			lwpladdrb->resval.rint &= ~IC_PLINE_CLOSED;		// remove the closure flag, if any
		lwpladdrb=lwpladdrb->rbnext;
		}

	//***********SPECIAL CASE**************
	//Do math for this case 1st and let it fall thru to which of the other 3 applies
	if (specialcaseflag)
		{
		lwpl->get_10(trimpt1,pt1vertno);
		pt2vertno=cmd_lwpline_entnext(pt1vertno,lwpl);
		//we don't know what the next ent is, so let's get it
		lwpl->get_10(trimpt2,pt2vertno);
		trimret=0;
		}

	//***********CASE 1***************
	if ((closure & 1) && trimret==1)
		{
		//modify vertex info for trimpt1 and make trimpoint vertex
		prevvert=pt1vertno;
		lwpl->get_42(&bulge,prevvert);
		lwpl->get_10(p1,prevvert);
		//setup vars to calculate width.  startwidth=start width, endwidth=end width
		lwpl->get_40(&startwidth,prevvert);
		lwpl->get_41(&endwidth,prevvert);

		//copy trimpt2 to beginning of segment
		lwpl->set_10(trimpt1,prevvert);

		nextvert=cmd_lwpline_entnext(prevvert,lwpl);

		lwpl->get_10(p2,nextvert);

		if (!icadRealEqual(bulge,0.0))		//if segment is curved, calculate new bulge & starting width
			{
			//if ((bulge==IC_TWOPI)||(bulge==-IC_TWOPI))bulge=bulge*0.99999999;
			//get a counter-clockwise arc to define the center point
			ic_bulge2arc(p1,p2,bulge,center,&junkreal,&startangle,&endangle);
			if (startangle>sds_angle(center,trimpt1))
				startangle=startangle-IC_TWOPI;
			if (endangle<sds_angle(center,trimpt1))
				endangle=endangle+IC_TWOPI;
			if (bulge>0.0)	//counter-clockwise pline
				{
				//update bulge
				bulge=tan((endangle-sds_angle(center,trimpt1))/4.0);
				//update width.  startwidth=start width, endwidth=end width
				startwidth=startwidth+((endwidth-startwidth)*(endangle-sds_angle(center,trimpt1))/(endangle-startangle));
				}
			else	//clockwise pline
				{
				//update bulge
				bulge=tan((startangle-sds_angle(center,trimpt1))/4.0);
				//update width
				startwidth=startwidth+((endwidth-startwidth)*(sds_angle(center,trimpt1)-startangle)/(endangle-startangle));
				}
			}
		else	//straight segment - just update starting width
			{
			startwidth=startwidth+((endwidth-startwidth)*sds_distance(p2,trimpt2)/sds_distance(p1,p2));
			}
		//Now, geometry for trimpt1 is updated in vert1.  If vert1!=vert2,
		// geometry complete and we can make vert1 & all ents up to BUT
		//NOT INCLUDING vert2
		if (pt1vertno!=pt2vertno)
			{
			if (prevvert<pt2vertno)
				{
				LwplrbAddVertFromData(&lwpladdrb, trimpt1, startwidth, endwidth, bulge);

				LwplrbAddVerts(lwpl, &lwpladdrb, prevvert+1, pt2vertno, false);
				}
			}

		prevvert=pt2vertno;

		lwpl->get_42(&bulge,prevvert);
		lwpl->get_10(p1,prevvert);
		//NOTE:  don't copy trimpt2 to this vert - we'll make that point as the new last vertex
		//get point of next vertex
		nextvert=cmd_lwpline_entnext(prevvert,lwpl);
		lwpl->get_10(p2,nextvert);
		//setup data for calculating widths just like in case 1 above
		lwpl->get_40(&startwidth,prevvert);
		lwpl->get_41(&endwidth,prevvert);

		if (bulge!=0)		//if segment is curved, calculate new bulge & ending width
			{
			//if ((bulge==IC_TWOPI)||(bulge==-IC_TWOPI))bulge=bulge*0.99999999;
			//get a counter-clockwise arc to define the center point
			ic_bulge2arc(p1,p2,bulge,center,&junkreal,&startangle,&endangle);
			if (startangle>sds_angle(center,trimpt2))
				startangle=startangle-IC_TWOPI;
			if (endangle<sds_angle(center,trimpt2))
				endangle=endangle+IC_TWOPI;
			if (bulge>0)  //counter-clockwise pline
				{
				//update bulge
				bulge=tan((sds_angle(center,trimpt2)-startangle)/4.0);
				//update width.  startwidth=start width, endwidth=end width
				endwidth=startwidth+((endwidth-startwidth)*(sds_angle(center,trimpt2)-startangle)/(endangle-startangle));
				}
			else	//clockwise pline
				{
				//update bulge
				bulge=tan((sds_angle(center,trimpt2)-endangle)/4.0);
				//update width
				endwidth=startwidth+((endwidth-startwidth)*(sds_angle(center,trimpt2)-endangle)/(startangle-endangle));
				}
			}
		else
			{
			//straight segment - just update starting width
			endwidth=startwidth+((endwidth-startwidth)*sds_distance(p1,trimpt2)/sds_distance(p1,p2));
			}

		LwplrbAddVertFromData(&lwpladdrb, p1, startwidth, endwidth, bulge);
		lwpladdrb->rbnext=sds_buildlist(10,trimpt2,40,0.0,41,0.0,42,0.0,0);

		LwplrbSetNumVerts(newlwpl);
		if (sds_entmake(newlwpl)==RTNORM)
			{
			sds_entdel(thelwpline);
			sds_entlast(thelwpline);
			sds_redraw(thelwpline,IC_REDRAW_DRAW);
			sds_relrb(newlwpl);
			newlwpl=NULL;
			}
		goto exit;	//an exclusive possibility
		}
	//***********CASE 2***************
	// DP: Here new lwpolyline should be created only if not CASE 3
	if (!icadPointEqual(plend,trimpt2))
		{
		//modify vertex info for trimpt2 and make trimpoint vertex
		prevvert=pt2vertno;
		lwpl->get_42(&bulge,prevvert);
		lwpl->get_10(p1,prevvert);
		nextvert=cmd_lwpline_entnext(prevvert,lwpl);
		lwpl->get_10(p2,nextvert);
		//setup vars to calculate width.  startwidth=start width, endwidth=end width
		lwpl->get_40(&startwidth,prevvert);
		lwpl->get_41(&endwidth,prevvert);

		if (!icadRealEqual(bulge,0.0))		//if segment is curved, calculate new bulge & starting width
			{
			//if ((bulge==IC_TWOPI)||(bulge==-IC_TWOPI))bulge=bulge*0.99999999;
			//get a counter-clockwise arc to define the center point
			ic_bulge2arc(p1,p2,bulge,center,&junkreal,&startangle,&endangle);
			if (startangle>sds_angle(center,trimpt2))
				startangle=startangle-IC_TWOPI;
			if (endangle<sds_angle(center,trimpt2))
				endangle=endangle+IC_TWOPI;
			if (bulge>0.0)	//counter-clockwise pline
				{
				//update bulge
				bulge=tan((endangle-sds_angle(center,trimpt2))/4.0);
				//update width.  startwidth=start width, endwidth=end width
				startwidth=startwidth+((endwidth-startwidth)*(endangle-sds_angle(center,trimpt2))/(endangle-startangle));
				}
			else	//clockwise pline
				{
				//update bulge
				bulge=tan((startangle-sds_angle(center,trimpt2))/4.0);
				//update width
				startwidth=startwidth+((endwidth-startwidth)*(sds_angle(center,trimpt2)-startangle)/(endangle-startangle));
				}
			}
		else	//straight segment - just update starting width
			startwidth=startwidth+((endwidth-startwidth)*sds_distance(p1,trimpt2)/sds_distance(p1,p2));

		LwplrbAddVertFromData(&lwpladdrb,trimpt2,startwidth,endwidth,bulge);

		nextvert=pt2vertno+1;
		LwplrbAddVerts(lwpl, &lwpladdrb, nextvert, lwpl->ret_90(), false);
		if (!(closure & IC_PLINE_CLOSED))
			{
			LwplrbSetNumVerts(newlwpl);
			if(icadPointEqual(plstart,trimpt1))
			{
				if(sds_entmod(newlwpl) != RTNORM)
					ret = -__LINE__;
			}
			else
			{
				lwpladdrb=newlwpl;
				while (lwpladdrb->rbnext != NULL)
					lwpladdrb=lwpladdrb->rbnext;
				lwpladdrb->rbnext = rbEdd;

				if(sds_entmakex(newlwpl, xtraent) != RTNORM)
					ret = -__LINE__;
			}

			sds_relrb(newlwpl);
			newlwpl=NULL;
			}

		if ((closure & IC_PLINE_CLOSED) && icadPointEqual(plstart,trimpt1))
			{
			//just make a vtx at the start/end point
			LwplrbAddVert(lwpl, &lwpladdrb, 0);
			LwplrbSetNumVerts(newlwpl);
			if (sds_entmod(newlwpl) != RTNORM)
				ret = -__LINE__;

			sds_relrb(newlwpl);
			newlwpl=NULL;
			}
		}
	//***********CASE 3***************
	if (!icadPointEqual(plstart,trimpt1))
		{
		//in this case we just have to entmod the first break vertex & then make a new seqend
		//make everything up to BUT NOT INCLUDING vert1
		if (closure & IC_PLINE_CLOSED)
			{
			nextvert=0;
			LwplrbAddVerts(lwpl, &lwpladdrb, nextvert, pt1vertno, true);
			}
		else	// start a new lwpline, make everything up to vert 1
			{
			FreeResbufIfNotNull(&newlwpl);
			newlwpl=sds_entgetx(thelwpline,&rb);	// create new lwpline resbuf to build on
			delete lwpl;
			lwpl=new db_lwpolyline(*((db_lwpolyline *)(thelwpline[0])));	// a copy for us to use to build the new one
			// delete all of the vertex data
			LwplrbDeleteVertexData(newlwpl);
			lwpladdrb=newlwpl;
			while (lwpladdrb->rbnext!=NULL)
				lwpladdrb=lwpladdrb->rbnext;
			nextvert=0;
			LwplrbAddVerts(lwpl, &lwpladdrb, nextvert, pt1vertno, true);
			}

		lwpl->get_10(p1,pt1vertno);
		if (icadPointEqual(trimpt1,p1))
			{
			LwplrbAddVert(lwpl, &lwpladdrb, pt1vertno);
			}
		else
			{
			//calc new widths & bulge to make pline end at trimpt1

			//NOTE:  don't copy trimpt1 to this vert - we'll make that point as the new last vertex
			//get point of next vertex
			lwpl->get_42(&bulge,pt1vertno);
			prevvert=pt1vertno;
			nextvert=cmd_lwpline_entnext(pt1vertno,lwpl);
			lwpl->get_10(p2,nextvert);
			//setup data for calculating widths just like in case 1 above
			lwpl->get_40(&startwidth,prevvert);
			lwpl->get_41(&endwidth,prevvert);

			if (bulge!=0)		//if segment is curved, calculate new bulge & ending width
				{
				//if ((bulge==IC_TWOPI)||(bulge==-IC_TWOPI))bulge=bulge*0.99999999;
				//get a counter-clockwise arc to define the center point
				ic_bulge2arc(p1,p2,bulge,center,&junkreal,&startangle,&endangle);
				if (startangle>sds_angle(center,trimpt1))
					startangle=startangle-IC_TWOPI;
				if (endangle<sds_angle(center,trimpt1))
					endangle=endangle+IC_TWOPI;
				if (bulge>0)  //counter-clockwise pline
					{
					//update bulge
					bulge=tan((sds_angle(center,trimpt1)-startangle)/4.0);
					//update width.  startwidth=start width, endwidth=end width
					endwidth=startwidth+((endwidth-startwidth)*(sds_angle(center,trimpt1)-startangle)/(endangle-startangle));
					}
				else	//clockwise pline
					{
					//update bulge
					bulge=tan((sds_angle(center,trimpt1)-endangle)/4.0);
					//update width
					endwidth=startwidth+((endwidth-startwidth)*(sds_angle(center,trimpt1)-endangle)/(startangle-endangle));
					}
				}
			else	//straight segment - just update starting width
				endwidth=startwidth+((endwidth-startwidth)*sds_distance(p1,trimpt1)/sds_distance(p1,p2));
			lwpl->get_10(tpt,prevvert);
			LwplrbAddVertFromData(&lwpladdrb,tpt,startwidth,endwidth,bulge);
			lwpladdrb->rbnext=sds_buildlist(10,trimpt1,40,0.0,41,0.0,42,0.0,0);
			}
		LwplrbSetNumVerts(newlwpl);
		if(sds_entmod(newlwpl) != RTNORM)
			ret = -__LINE__;

		sds_relrb(newlwpl);
		newlwpl=NULL;
		}

	sds_redraw(thelwpline, IC_REDRAW_DRAW);
	if(xtraent[0] || xtraent[1])
		sds_redraw(xtraent, IC_REDRAW_DRAW);

	exit:
	delete lwpl;
	return(ret);
}

int cmd_adj_pline_trimpts(sds_name plname,sds_point chkpt,sds_point pickpt, sds_point trimpt1,sds_point trimpt2,
						  int plineflag, sds_point start, sds_point end,
						  int (*distfunc)(sds_name, sds_point, sds_point, sds_real *))
	{

	//chkpt is point to check for possible chang in trimming pts trimpt1 and trimpt2
	//pickpt represents point user picked in selecting the pline, CONVERTED to NATIVE
	//	COORDS FOR THE PLINE AND ADJUSTED FOR ELEVATION (if 2D).
	//plineflag is used to check if plname is closed.
	// distfunc should be passed as either cmd_pline_dist or cmd_lwpline_dist depending on the entity type
	//NOTE: Trimpt1 always BEFORE trimpt2 on pline.
	//RETURNS: -1 = ERROR or trimpt not adjusted;
	//			0 = Pick pt between trimpt1 and trimpt2 - this is always the case
	//					for OPEN plines and CLOSED plines where we're trimming out
	//					a chunk in the center.
	//			1 = Pick pt NOT betw trimpt1 and trimpt2 - this only happens if pline
	//					is CLOSED AND pickpt NOT between trimpt1 and trimpt2. - if we're
	//					trimming off beginning AND trailing ends.

	sds_real fr1,fr2,fr3;
	int ret = -1;

	if (icadPointEqual(chkpt,trimpt1) || icadPointEqual(chkpt,trimpt2)) {

		return ret;
	}

	if ((*distfunc)(plname,chkpt,pickpt,&fr3))
		return ret;

	bool firstPointMoved = icadPointEqual(trimpt1, start) ? false : true;
	bool secondPointMoved = icadPointEqual(trimpt2, end) ? false : true;

	// If firstptequal and secondptequal both are true, this will be the first
	// point we receive for an open pline but may or may not be the first
	// point we receive for a closed pline. 
	if (!firstPointMoved && !secondPointMoved) { 
		if (fr3 >= 0.0) {	// the check point is located before the pick point
			ic_ptcpy(trimpt1,chkpt);
			if (plineflag & IC_PLINE_CLOSED)
				ic_ptcpy(trimpt2,chkpt);
		}
		else {			// the check point is located after the pick point
			ic_ptcpy(trimpt2,chkpt);
			if (plineflag & IC_PLINE_CLOSED)
				ic_ptcpy(trimpt1,chkpt);
		}
		return 0;
	}
	//else, we've already moved at least 1 trimpt...
	else if (!firstPointMoved && secondPointMoved) {	//if we've only moved end pt & trimpt1 is at start...
		if ((*distfunc)(plname,pickpt,trimpt2,&fr2))
			return ret;

		if (fr3 >= 0.0) {
			// the check point is before the pick point so it is between the start 
			// point and the pick point
			ic_ptcpy(trimpt1,chkpt);
			ret = 0;
		}
		else {	// The check point is after the pick point
			if (-fr3 < fr2)	{	// the check point is between the pick point and trimpt2
				ic_ptcpy(trimpt2,chkpt);
				ret = 0;
			}
		}
	}
	else if (firstPointMoved && !secondPointMoved) {
		if ((*distfunc)(plname,trimpt1,pickpt,&fr1))
			return ret;

		if (fr3 <= 0.0) {
			ic_ptcpy(trimpt2,chkpt);
			ret = 0;
		}
		else {
			if (fr3 < fr1) {
				ic_ptcpy(trimpt1,chkpt);
				ret = 0;
			}
		}
	}
	//lastly, if we've moved both trimpts off the endpts...
	else {
		if (fr3 <= 0.0) {
			if ((*distfunc)(plname,pickpt,trimpt2,&fr2))
				return ret;

			if (-fr3 < fr2) { // the check point is after the pick point
				ic_ptcpy(trimpt2, chkpt);
				ret = 0;
			}
			else {
				if (icadPointEqual(trimpt1, trimpt2)) {
					if (fr2 > 0.0)
						ic_ptcpy(trimpt1, chkpt);
					else 
						ic_ptcpy(trimpt2, chkpt);

					ret = 0;
				}
			}
		}
		else { // fr3 > 0.0
			if ((*distfunc)(plname,trimpt1,pickpt,&fr1))
				return ret;

			if (fr3 < fr1) { 	// the check point is before the pick point
				ic_ptcpy(trimpt1, chkpt);
				ret = 0;
			}
			else {		// the check point is far away to the pick point than trimpt1
				if (icadPointEqual(trimpt1, trimpt2)) {
					if (fr1 >= 0.0) 
						ic_ptcpy(trimpt2, chkpt);
					else 
						ic_ptcpy(trimpt1, chkpt);

					ret = 0;
				}
			}
		}
	}
	return ret;
}


int cmd_pline_dist(sds_name plent, sds_point pFrom, sds_point pTo, sds_real *dist)
	{
	//sets dist to distance along pline plent from pFrom to pTo. if pFrom BEFORE pTo,
	//distance is positive.  if pFrom AFTER pTo, distance will be negative.
	//returns 0 if measurement successful, negative value otherwise.
	//NOTE:if the pline is closed and pFrom is the pline start/end pt, we use it as startpoint of pline.
	//			if pline is closed and pTo is start/end pt we use it as endpoint of pline.
	//			in these cases, *dist is always positive
	//Points pFrom & pTo need not be on plent - measurements are done from closest pts on pline.
	//NOTE: NO TRANSFORMS DONE HERE - all pts must be in same coords

	sds_name vtxent1,vtxent2;
	sds_real fr1,fr2,fr3,distsofar;
	sds_point startpt,endpt,p1,p2,p3,p4,ptemp;
	int ret=0;
	int closure;
	long fl0,fl1,fl2;
	struct resbuf *rbp3,*rbp4,rbent,rbucs;
	int exactvert;

	rbent.restype=RTENAME;
	ic_encpy(rbent.resval.rlname,plent);
	rbucs.restype=RTSHORT;
	rbucs.resval.rint=1;

	rbp3=rbp4=NULL;

	ic_ptcpy(p1,pFrom);
	ic_ptcpy(p2,pTo);

	if ((rbp3=sds_entget(plent))==NULL) {ret=-1;goto exit;}
	if (ic_assoc(rbp3,70))
		closure=0;
	else if (ic_rbassoc->resval.rint & 1)
		closure=1;
	else
		closure=0;
	IC_RELRB(rbp3);
	if (sds_entnext_noxref(plent,vtxent1)!=RTNORM) {ret=-1;goto exit;}
	if ((rbp3=sds_entget(vtxent1))==NULL) {ret=-1;goto exit;}
	ic_assoc(rbp3,10);
	ic_ptcpy(p3,ic_rbassoc->resval.rpoint);

	if (icadPointEqual(p3,p1))
		fl0=1;
	else
		{
		if (icadPointEqual(p1,p2))
			{
			*dist=0.0;
			goto exit;
			}
		sds_trans(p1,&rbent,&rbucs,0,ptemp);
		//VTX_NO TAKES A UCS PT
		if ((fl0=cmd_pl_vtx_no(plent,ptemp,1,&exactvert))==-1) {ret=(-1);goto exit;}
		}
	if (closure && (icadPointEqual(p3,p2)))
		{
		//get vertex number of last vertex....
		ic_assoc(rbp3,0);
		for (fl1=1;(strsame(ic_rbassoc->resval.rstring,"VERTEX"/*DNT*/ ));fl1++)
			{
			if (sds_entnext_noxref(vtxent1,vtxent1)!=RTNORM) {ret=-1;goto exit;}
			IC_RELRB(rbp3);
			if ((rbp3=sds_entget(vtxent1))==NULL) {ret=-1;goto exit;}
			ic_assoc(rbp3,0);
			}
		}
	else
		{
		sds_trans(p2,&rbent,&rbucs,0,ptemp);
		if ((fl1=cmd_pl_vtx_no(plent,ptemp,1,&exactvert))==-1) {ret=-1;goto exit;}
		}
	if (fl0<fl1)
		{
		ic_ptcpy(startpt,p1);
		ic_ptcpy(endpt,p2);
		}
	else if (fl1<fl0)
		{
		ic_ptcpy(startpt,p2);
		ic_ptcpy(endpt,p1);
		}
	//if both pts are in same vtx, we'll handle it later...
	ic_encpy(vtxent1,plent);
	//walk to 1st vertex we need to examine
	for (fl2=0; fl2<(min(fl0,fl1)); sds_entnext_noxref(vtxent1,vtxent1),fl2++);
	IC_RELRB(rbp3);
	if ((rbp3=sds_entget(vtxent1))==NULL) {ret=-1;goto exit;}
	ic_assoc(rbp3,42);
	fr1=ic_rbassoc->resval.rreal;
	ic_assoc(rbp3,10);
	ic_ptcpy(p3,ic_rbassoc->resval.rpoint);
	cmd_pline_entnext(vtxent1,vtxent2,closure);
	if ((rbp4=sds_entget(vtxent2))==NULL) {ret=-1;goto exit;}
	ic_assoc(rbp4,10);
	ic_ptcpy(p4,ic_rbassoc->resval.rpoint);
	if (fl0==fl1)	//if both points are within the same segment....
		{
		cmd_pt_2_plseg(vtxent1,p1);
		cmd_pt_2_plseg(vtxent1,p2);
		ic_ptcpy(startpt,p2);
		ic_ptcpy(endpt,p1);
		cmd_pt_begsegdist(p3,p4,fr1,p1,&fr2);
		cmd_pt_begsegdist(p3,p4,fr1,p2,&fr3);
		*dist=fr3-fr2;
		goto exit;
		}
	else
		{
		for (fl2=0;fl2<=abs(fl1-fl0);fl2++)
			{
			if (fl2==0)
				{
				cmd_pt_2_plseg(vtxent1,startpt);

				if (fl0<fl1)
					ic_ptcpy(p1,startpt);
				else if (fl1<fl0)
					ic_ptcpy(p2,startpt);

				cmd_pt_begsegdist(p3,p4,fr1,startpt,&fr2);
				cmd_pt_begsegdist(p3,p4,fr1,p4,&fr3);
				distsofar=fr3-fr2;
				}
			else
				{
				//move on to next vertex
				sds_entnext_noxref(vtxent1,vtxent1);
				IC_RELRB(rbp3);
				if ((rbp3=sds_entget(vtxent1))==NULL)
					goto exit;	//passed a seqend
				ic_assoc(rbp3,0);
				if (strsame(ic_rbassoc->resval.rstring,"SEQEND"/*DNT*/ ))
					break;
				sds_entnext_noxref(vtxent1,vtxent2);
				IC_RELRB(rbp4);
				if ((rbp4=sds_entget(vtxent2))==NULL)
					goto exit;
				ic_assoc(rbp4,0);
				if (strsame(ic_rbassoc->resval.rstring,"SEQEND"/*DNT*/ ))
					{
					if (!closure)
						break;
					cmd_pline_entnext(vtxent1,vtxent2,closure);
					IC_RELRB(rbp4);
					if ((rbp4=sds_entget(vtxent2))==NULL)
						goto exit;
					}
				ic_assoc(rbp3,10);
				ic_ptcpy(p3,ic_rbassoc->resval.rpoint);
				ic_assoc(rbp3,42);
				fr1=ic_rbassoc->resval.rreal;
				ic_assoc(rbp4,10);
				ic_ptcpy(p4,ic_rbassoc->resval.rpoint);
				//now add on the additional distance
				if (fl2!=abs(fl1-fl0))
					{
					cmd_pt_begsegdist(p3,p4,fr1,p4,&fr2);
					distsofar+=fr2;
					}
				else
					{
					cmd_pt_2_plseg(vtxent1,endpt);
					cmd_pt_begsegdist(p3,p4,fr1,endpt,&fr2);
					distsofar+=fr2;
					}
				}
			}
		}
	exit:
	if (fl0!=fl1)
		{
		if (icadPointEqual(startpt,p2))
			*dist=distsofar*-1.0;
		else
			*dist=distsofar;
		}
	IC_RELRB(rbp3);
	IC_RELRB(rbp4);
	return(ret);
	}

int cmd_lwpline_dist(sds_name lwplename, sds_point pFrom, sds_point pTo, sds_real *dist)
	{
	//sets dist to distance along pline plent from pFrom to pTo. if pFrom BEFORE pTo,
	//distance is positive.  if pFrom AFTER pTo, distance will be negative.
	//returns 0 if measurement successful, negative value otherwise.
	//NOTE:if the pline is closed and pFrom is the pline start/end pt, we use it as startpoint of pline.
	//			if pline is closed and pTo is start/end pt we use it as endpoint of pline.
	//			in these cases, *dist is always positive
	//Points pFrom & pTo need not be on plent - measurements are done from closest pts on pline.
	//NOTE: NO TRANSFORMS DONE HERE - all pts must be in same coords

	int vert1no,vert2no;
	sds_real fr1,fr2,fr3,distsofar;
	sds_point startpt,endpt,frompt,topt,p3,p4,ptemp;
	int ret=0;
	int closure;
	long fl0,fl1,fl2;
	struct resbuf rbent,rbucs;
	int exactvert;
	db_lwpolyline *lwpl=(db_lwpolyline *)lwplename[0];
	sds_point vert0pt;

	rbent.restype=RTENAME;
	ic_encpy(rbent.resval.rlname,lwplename);
	rbucs.restype=RTSHORT;
	rbucs.resval.rint=1;

	ic_ptcpy(frompt,pFrom);
	ic_ptcpy(topt,pTo);

	if (lwpl->ret_70() & IC_PLINE_CLOSED)
		closure=1;
	else
		closure=0;

	lwpl->get_10(vert0pt,0);

	if (icadPointEqual(vert0pt,frompt))
		fl0=0;	// changed to 0 -- was 1
	else
		{
		if (icadPointEqual(frompt,topt))
			{
			*dist=0.0;
			goto exit;
			}
		sds_trans(frompt,&rbent,&rbucs,0,ptemp);
		//VTX_NO TAKES A UCS PT
		if ((fl0=cmd_lwpl_vtx_no(lwplename,ptemp,1,&exactvert))==-1) {ret=(-1);goto exit;}
		}
	if (closure && (icadPointEqual(vert0pt,topt)))
		{
		fl1=lwpl->ret_90()-1;
		}
	else
		{
		sds_trans(topt,&rbent,&rbucs,0,ptemp);
		if ((fl1=cmd_lwpl_vtx_no(lwplename,ptemp,1,&exactvert))==-1) {ret=-1;goto exit;}
		}
	if (fl0<fl1)
		{
		ic_ptcpy(startpt,frompt);
		ic_ptcpy(endpt,topt);
		}
	else if (fl1<fl0)
		{
		ic_ptcpy(startpt,topt);
		ic_ptcpy(endpt,frompt);
		}
	//if both pts are in same vtx, we'll handle it later...
	vert1no=min(fl0,fl1);
	lwpl->get_42(&fr1,vert1no);
	lwpl->get_10(p3,vert1no);
	vert2no=cmd_lwpline_entnext(vert1no,lwpl);
	lwpl->get_10(p4,vert2no);
	if (fl0==fl1)	//if both points are within the same segment....
		{
		cmd_pt_2_lwplseg(lwpl,vert1no,frompt);
		cmd_pt_2_lwplseg(lwpl,vert1no,topt);	// intuitively, this looks either wrong or wasteful.  It came from
											// cmd_pline_dist
		ic_ptcpy(startpt,topt);
		ic_ptcpy(endpt,frompt);
		cmd_pt_begsegdist(p3,p4,fr1,frompt,&fr2);
		cmd_pt_begsegdist(p3,p4,fr1,topt,&fr3);
		*dist=fr3-fr2;
		goto exit;
		}
	else
		{
		for (fl2=0; fl2<=abs(fl1-fl0); fl2++)
			{
			if (fl2==0)
				{
				cmd_pt_2_lwplseg(lwpl,vert1no,startpt);

				if (fl0<fl1)
					ic_ptcpy(frompt,startpt);
				else if (fl1<fl0)
					ic_ptcpy(topt,startpt);

				cmd_pt_begsegdist(p3,p4,fr1,startpt,&fr2);
				cmd_pt_begsegdist(p3,p4,fr1,p4,&fr3);
				distsofar=fr3-fr2;
				}
			else
				{
				//move on to next vertex
				vert1no++;
				if (!closure) {
					if (vert1no==lwpl->ret_90()-1)
						break;
				}
				else 
					vert2no = 0;
				
				vert2no=cmd_lwpline_entnext(vert1no,lwpl);
				lwpl->get_10(p3,vert1no);
				lwpl->get_42(&fr1,vert1no);
				lwpl->get_10(p4,vert2no);
				//now add on the additional distance
				if (fl2!=abs(fl1-fl0))
					{
					cmd_pt_begsegdist(p3,p4,fr1,p4,&fr2);
					distsofar+=fr2;
					}
				else
					{
					cmd_pt_2_lwplseg(lwpl,vert1no,endpt);
					cmd_pt_begsegdist(p3,p4,fr1,endpt,&fr2);
					distsofar+=fr2;
					break;
					}
				}
			}
		}
	exit:
	if (fl0!=fl1)
		{
		if (icadPointEqual(startpt,topt))
			*dist=distsofar*-1.0;
		else
			*dist=distsofar;
		}
	return(ret);
	}

int cmd_pt_begsegdist (sds_point p0, sds_point frompt, sds_real bulge, sds_point ptest, sds_real *dist){
	sds_real fr1,fr2,fr3,fr4;
	sds_point cc;

	if (icadPointEqual(p0,ptest)){
		*dist=0.0;
		return(0);
	}
	if (icadRealEqual(bulge,0.0)){
		*dist=sds_distance(p0,ptest);
		return(0);
	}
	ic_bulge2arc(p0,frompt,bulge,cc,&fr1,&fr2,&fr3);
	fr4=sds_angle(cc,ptest);
	if (bulge > 0.0){	//ccw
		ic_normang(&fr2,&fr4);
		*dist=fr1*(fr4-fr2);
		return(0);
	}else{	 					//cw
		ic_normang(&fr4,&fr3);
		*dist=fr1*(fr3-fr4);
		return(0);
	}
}



int cmd_get_textpts(struct resbuf *rbp,sds_point p1,sds_point p2,sds_point p3,sds_point p4){
	sds_point p0, lowleft, upright;
	sds_real theta, costheta, sintheta;

	if (sds_textbox(rbp,lowleft,upright)!=RTNORM) return(-1);
	ic_assoc(rbp,10);
	ic_ptcpy(p0,ic_rbassoc->resval.rpoint);
	if (ic_assoc(rbp,50))
		theta=0.0;
	else
		theta=ic_rbassoc->resval.rreal;
	costheta=cos(theta);
	sintheta=sin(theta);

	p1[SDS_X]=p0[SDS_X]+(lowleft[SDS_X]*costheta - lowleft[SDS_Y]*sintheta);
	p1[SDS_Y]=p0[SDS_Y]+(lowleft[SDS_X]*sintheta + lowleft[SDS_Y]*costheta);
	p2[SDS_X]=p0[SDS_X]+(upright[SDS_X]*costheta - lowleft[SDS_Y]*sintheta);
	p2[SDS_Y]=p0[SDS_Y]+(upright[SDS_X]*sintheta + lowleft[SDS_Y]*costheta);
	p3[SDS_X]=p0[SDS_X]+(upright[SDS_X]*costheta - upright[SDS_Y]*sintheta);
	p3[SDS_Y]=p0[SDS_Y]+(upright[SDS_X]*sintheta + upright[SDS_Y]*costheta);
	p4[SDS_X]=p0[SDS_X]+(lowleft[SDS_X]*costheta - upright[SDS_Y]*sintheta);
	p4[SDS_Y]=p0[SDS_Y]+(lowleft[SDS_X]*sintheta + upright[SDS_Y]*costheta);
	p4[SDS_Z]=p3[SDS_Z]=p2[SDS_Z]=p1[SDS_Z]=p0[SDS_Z];

	return(0);
}

void cmd_adj_arc_trimang(sds_point cc, sds_real pickang, sds_point chkpt,
							sds_real *trimang1, sds_real *trimang2){

	sds_real chkang;
	int fi1;

	chkang=sds_angle(cc,chkpt);
	if (icadAngleEqual(chkang,IC_TWOPI))chkang=0.0;
	//if we're on a circle and neither trimang has been set yet, set trimang2
	if ((*trimang1==-IC_TWOPI) && (*trimang2==IC_TWOPI)){
		*trimang2=chkang;
		return;
	}
	//if we're on a circle and trimang1 hasn't been set yet
	if (*trimang1==-IC_TWOPI){
		if (cmd_ang_betw(pickang,*trimang2,chkang)){
			*trimang1=*trimang2;
			*trimang2=chkang;
		}else{
			*trimang1=chkang;
		}
	}
	//if we're on an arc or a circle where we've already determined the trim points...
	if (fi1=cmd_ang_betw(chkang,pickang,*trimang2)) {
		*trimang2=chkang;
		return;
	}
	if (fi1=cmd_ang_betw(chkang,*trimang1,pickang))
		*trimang1=chkang;
	return;
}




void cmd_adj_line_trimpt(sds_point epoint, sds_point testpt, sds_point linestart, sds_point lineend, sds_point starttrim, sds_point endtrim){
	//epoint = selected pt to trim a line
	//starttrim= nearest trim pt toward line beginning,
	//endtrim= nearest trim pt toward line end
	//testpt= point to check if it's closer to starttrim or endtrim
	//linestart & lineend are start & end points of line

	int linedim;
	sds_point linevect;
	linevect[0]=lineend[0]-linestart[0];
	linevect[1]=lineend[1]-linestart[1];
	linevect[2]=lineend[2]-linestart[2];
	//check if testpt is not on ray
	if (fabs(linevect[0])>fabs(linevect[1])){
		if (fabs(linevect[0])>fabs(linevect[2]))linedim=0;
		else linedim=2;
	}else if (fabs(linevect[1])>fabs(linevect[2]))linedim=1;
	else linedim=2;

	//check if we're working on point toward start or end of line...
	if ((testpt[linedim]-epoint[linedim])*linevect[linedim]>0.0){
		//testpt is toward end of line
		if (fabs(testpt[linedim]-epoint[linedim])<fabs(endtrim[linedim]-epoint[linedim]))
			ic_ptcpy(endtrim,testpt);
	}else{
		//testpt is toward start of line
		if (fabs(testpt[linedim]-epoint[linedim])<fabs(starttrim[linedim]-epoint[linedim]))
			ic_ptcpy(starttrim,testpt);
	}

}
void cmd_adj_xline_trimpt(sds_point epoint, sds_point testpt, sds_point linestart, sds_point linevect,int *ptchk, sds_point starttrim, sds_point endtrim){
	//epoint = selected pt to trim a line
	//starttrim= nearest trim pt toward line beginning,
	//endtrim= nearest trim pt toward line end
	//testpt= point to check if it's closer to starttrim or endtrim
	//linestart=point defining xline
	//linevect=direction vector toward END of xline
	//ptchk:  if bit 0 set, we've got trim pt toward start of xline
	//		  if bit 1 set, we've got trim pt toward end of xline

	int linedim;
	//check if testpt is not on ray
	if (fabs(linevect[0])>fabs(linevect[1])){
		if (fabs(linevect[0])>fabs(linevect[2]))linedim=0;
		else linedim=2;
	}else if (fabs(linevect[1])>fabs(linevect[2]))linedim=1;
	else linedim=2;

	//check if we're working on point toward start or end of xline...
	if ((testpt[linedim]-epoint[linedim])*linevect[linedim]>0.0){
		//testpt is toward end of xline
		if (0==((*ptchk)&2) || (fabs(testpt[linedim]-epoint[linedim])<fabs(endtrim[linedim]-epoint[linedim]))){
			ic_ptcpy(endtrim,testpt);
			*ptchk|=2;
		}
	}else{
		//testpt is toward start of line
		if (0==((*ptchk)&1) || (fabs(testpt[linedim]-epoint[linedim])<fabs(starttrim[linedim]-epoint[linedim]))){
			ic_ptcpy(starttrim,testpt);
			*ptchk|=1;
		}
	}

}

void cmd_adj_ray_trimpt(sds_point epoint, sds_point testpt, sds_point raystart, sds_point rayvect, sds_point starttrim, sds_point endtrim){

	//epoint = selected pt on ray
	//starttrim= nearest trim pt toward ray beginning,
	//endtrim= trim pt furthest from start (beyond epoint)
	//testpt= point to check
	//raystart=10pt for ray
	//rayvect= 11vect for ray
	//NOTE: we start w/endtrim at starttrim and then move it away if we find a trim
	//	pt beyond selected point on ray
	//UNLIKE LINE POINT ADJUSTER, IT'S OKAY TO PASS A PT NOT ON RAY AS TESTPT!

	int raydim;
	//check if testpt is not on ray
	if (fabs(rayvect[0])>fabs(rayvect[1])){
		if (fabs(rayvect[0])>fabs(rayvect[2]))raydim=0;
		else raydim=2;
	}else if (fabs(rayvect[1])>fabs(rayvect[2]))raydim=1;
	else raydim=2;
	if ((testpt[raydim]-raystart[raydim])*rayvect[raydim]<CMD_FUZZ)return;

	//check if we're working on point toward start or end of ray...
	if ((testpt[raydim]-epoint[raydim])*rayvect[raydim]>0.0){
		//testpt is toward end of ray
		if (icadPointEqual(endtrim,raystart) ||
			(fabs(testpt[raydim]-epoint[raydim])<fabs(endtrim[raydim]-epoint[raydim])))
				ic_ptcpy(endtrim,testpt);
	}else{
		//testpt is toward start of ray
		if (fabs(testpt[raydim]-epoint[raydim])<fabs(starttrim[raydim]-epoint[raydim]))
				ic_ptcpy(starttrim,testpt);
	}

}
int cmd_pline_entnext(sds_name ent, sds_name result, int closed) {
	//This function sets result to the name of the next vertex following ent in the polyline, or
	//	NULL if closed==0 and the next entity after ent is the SEQEND marker.
	//NOTE: if closed bits 2 (4) or 4 (16) is set, pline will only return
	//	vertices which are NOT spline control vertices. If you want the very
	//	next vtx for a closed, splined pline, set closed bit 0 (1).  We use
	//	bits 2 and 4 so we can pass from either a vtx or a pline llist.
	//if passed the pline entity, the function returns the first vertex
	//IF CLOSED<0, pline will be searched and closed will be determined from
	//	pline's 70 flag
	struct resbuf *rbp,*rbp2;
	sds_name etemp;
	int ret=0;

	rbp2=rbp=NULL;

	if ((rbp=sds_entget(ent))==NULL){
		result[0]=result[1]=0;
		return(-1);
	}
	ic_assoc(rbp,0);
	if (closed<0){
	   if (!strsame(ic_rbassoc->resval.rstring,"VERTEX"/*DNT*/ ))goto nextent;
		ic_encpy(etemp,ent);
		for (;;){
			sds_entnext_noxref(etemp,etemp);
			if (rbp2!=NULL){sds_relrb(rbp2); rbp2=NULL;}
			rbp2=sds_entget(etemp);
			ic_assoc(rbp2,0);
		   if (strsame(ic_rbassoc->resval.rstring,"VERTEX"/*DNT*/ )){
				ic_assoc(rbp2,70);
				if (0!=(ic_rbassoc->resval.rint & 9))continue;
				else ic_encpy(result,etemp);
				goto exit;
		   }else if (strsame(ic_rbassoc->resval.rstring,"SEQEND"/*DNT*/ )){
				ic_assoc(rbp2,-2);
				ic_encpy(etemp,ic_rbassoc->resval.rlname);
				sds_relrb(rbp2); rbp2=NULL;
				if (NULL==(rbp2=sds_entget(etemp))){
					ret=-1;goto exit;
				}
				ic_assoc(rbp2,70);
				closed=ic_rbassoc->resval.rint;
				goto nextent;
		   }else if (strsame(ic_rbassoc->resval.rstring,"POLYLINE"/*DNT*/ )){
				continue;
			}else{
				ret=-1;goto exit;
			}
		}
	}
   if (strsame(ic_rbassoc->resval.rstring,"SEQEND"/*DNT*/ )){
		if (closed>0 && (0==(closed&1))){
			ret=-1;goto exit;
		}else goto nextent;
   }else if (strsame(ic_rbassoc->resval.rstring,"VERTEX"/*DNT*/ )||
		   strsame(ic_rbassoc->resval.rstring,"POLYLINE"/*DNT*/ )){
		ic_encpy(etemp,ent);
		nextent:
		if (rbp!=NULL){sds_relrb(rbp); rbp=NULL;}
		sds_entnext_noxref(etemp,etemp);
		if ((rbp=sds_entget(etemp))==NULL){
			ret=-1;goto exit;
		}
		ic_assoc(rbp,0);
	   if (strsame("VERTEX"/*DNT*/ ,ic_rbassoc->resval.rstring)){
			if (closed>0 && closed & 20){
				ic_assoc(rbp,70);
				if (ic_rbassoc->resval.rint&16)goto nextent;
			}
			ic_encpy(result,etemp);
			goto exit;
	   }else if (strsame("SEQEND"/*DNT*/ ,ic_rbassoc->resval.rstring)){
			if (closed>0 && 0==(closed&1)) {
				ret=-1;goto exit;
			}
			ic_assoc(rbp,-2);
			ic_encpy(etemp,ic_rbassoc->resval.rlname);
			goto nextent;
	   }else if (strsame("POLYLINE"/*DNT*/ ,ic_rbassoc->resval.rstring)){
			if (closed<0){
				ic_assoc(rbp,70);
				closed=ic_rbassoc->resval.rint;
			}
			goto nextent;
		}else{
			ret=-1;goto exit;
		}
	}else{
		ret=-1;goto exit;
	}
exit:
	if (rbp2!=NULL)
		sds_relrb(rbp2);
	if (rbp!=NULL)
		sds_relrb(rbp);
	if (ret==-1){result[0]=result[1]=0;}
	return(ret);
}

int cmd_champt_by_angle(sds_point startpt,sds_point intpt,sds_point segstart,
							sds_point segend,sds_real deflang,sds_point pickpt2, int onseg, sds_point result){
	//startpt is takeoff toward other segment, intpt is where they intersect
	//segstart and segend define the segment we're intersecting with
	//deflang is the deflection from the intersection and result is the 2nd
	//chamfer point by angle calculation

	//returns 0 if successful, -1 if unsuccessful

	sds_point ptest1,ptest2,ptemp;
	sds_real fr1;
	int fi1,fi2;

	fr1=sds_angle(startpt,intpt);
	sds_polar(startpt,fr1+deflang,1.0,ptest2);//get new pt from 1st chamfer pt cwise
	fi2=ic_linexline(startpt,ptest2,segstart,segend,ptemp);
	ic_ptcpy(ptest2,ptemp);
	sds_polar(startpt,fr1-deflang,1.0,ptest1);//get new pt from 1st chamfer pt ccwise
	fi1=ic_linexline(startpt,ptest1,segstart,segend,ptemp);
	ic_ptcpy(ptest1,ptemp);
	if (onseg && fi1<0 && fi2<0)return(-1);
	if (fi1>=0){
		if ((0==onseg || (fi1&2)) && icadDirectionEqual(intpt,ptest1,intpt,pickpt2)){
			ic_ptcpy(result,ptest1);
			return(0);
		}
	}
	if (fi2>=0){
		if ((0==onseg || (fi2&2)) && icadDirectionEqual(intpt,ptest2,intpt,pickpt2)){
			ic_ptcpy(result,ptest2);
			return(0);
		}
	}
	return(-1);
}




int cmd_modpts_4_chamfer(sds_point startpt,sds_point endpt, sds_point chamferpt, sds_point inters, sds_point pickpt){
	//this function modify's a line's start and end points for chamfer
	//Calling function must do ENTMOD & issue any warnings to user.
	//NOTE: CHAMFERPT and INTPT must NOT be equal

	//ptflag is just to tell us if pt was picked by hand or we arbitrarily
	//	used the start of the line

	sds_real fr1,fr2,fr3;

	//modify elist for the first line....
	fr1=sds_angle(inters,startpt);
	fr2=sds_angle(inters,endpt);
	if (icadPointEqual(inters,chamferpt))
		fr3=sds_angle(inters,pickpt);
	else
		fr3=sds_angle(inters,chamferpt);

	if (icadAngleEqual(fr1,fr2)){
		if (sds_distance(inters,startpt)>=sds_distance(inters,endpt))
			if (sds_distance(inters,startpt)>=sds_distance(inters,chamferpt))
				ic_ptcpy(endpt,chamferpt);
			else{
				return(-1);
			}
		else
			if (sds_distance(inters,endpt)>=sds_distance(inters,chamferpt))
				ic_ptcpy(startpt,chamferpt);
			else{
				return(-1);
			}
	}else{
		if (icadAngleEqual(fr1,fr3)){
			if (sds_distance(inters,startpt)>=sds_distance(inters,chamferpt))
				ic_ptcpy(endpt,chamferpt);
			else{
				return(-1);
			}
		}else{
			if (sds_distance(inters,endpt)>=sds_distance(inters,chamferpt))
				ic_ptcpy(startpt,chamferpt);
			else{
				return(-1);
			}
		}
	}
	return(0);
}


/*

int cmd_real_equal(sds_real r1, sds_real r2)
	{
	if (fabs(r2)<=CMD_FUZZ)
		{
		if (fabs(r1)>CMD_FUZZ)
			{
			return(0);
			}
		}
	else
		{
		if (fabs((r1/r2)-1.0) > CMD_FUZZ)
			{
			return(0);
			}
		}
	return(1);
}
 //Modified Cybage SBD 30/04/2001 DD/MM/YYYY [
 //Reason : Mail from Ben Thump(mcthum@siacad.com) Dated 17/04/2001 DD/MM/YYYY.Re: join not working
 int cmd_real_equal2(sds_real r1, sds_real r2)
 	{
 	if (fabs(r2)<=CMD_FUZZ2)
 		{
 		if (fabs(r1)>CMD_FUZZ2)
 			{
 			return(0);
 			}
 		}
 	else
 		{
 		if (fabs((r1/r2)-1.0) > CMD_FUZZ2)
 			{
 			return(0);
 			}
 		}
 	return(1);
 }
 //Modified Cybage SBD 30/04/2001 DD/MM/YYYY ]

int cmd_angle_equal(sds_real r1, sds_real r2) {
	//uses IC_ZRO, not CMD_FUZZ

	if (fabs(r2)<=IC_ZRO && fabs(r1)<IC_ZRO)return(1);
	if (fabs((r1/r2)-1.0) < IC_ZRO)return(1);
	r1+=IC_TWOPI;
	if (fabs(r2)<=IC_ZRO && fabs(r1)<IC_ZRO)return(1);
	if (fabs((r1/r2)-1.0) < IC_ZRO)return(1);
	r1-=IC_TWOPI;
	r1-=IC_TWOPI;
	if (fabs(r2)<=IC_ZRO && fabs(r1)<IC_ZRO)return(1);
	if (fabs((r1/r2)-1.0) < IC_ZRO)return(1);
	return(0);
}


int cmd_direction_equal(sds_point p1,sds_point p2, sds_point p3, sds_point p4){
	//assumes all 4 pts are colinear.
	//returns 1 if vector p1-p2 is same direction as vector p3-p4, 0 otherwise
	int i;

	if (fabs(p2[0]-p1[0])>fabs(p2[1]-p1[1])){
		if (fabs(p2[0]-p1[0])<fabs(p2[2]-p1[2]))i=2;
		else i=0;
	}else if (fabs(p2[1]-p1[1])<fabs(p2[2]-p1[2]))i=2;
	else i=1;

	if (((p2[i]-p1[i])*(p4[i]-p3[i]))>0.0)return(1);
	else return(0);
}


int cmd_pt_equal(sds_point p1, sds_point p2) {


	if (fabs(p2[0])<=CMD_FUZZ){
		if (fabs(p1[0])>CMD_FUZZ)return(0);
	}else if (fabs((p1[0]/p2[0])-1.0) >= CMD_FUZZ){
		return(0);
	}
	if (fabs(p2[1])<=CMD_FUZZ){
		if (fabs(p1[1])>CMD_FUZZ)return(0);
	}else if (fabs((p1[1]/p2[1])-1.0) >= CMD_FUZZ){
		return(0);
	}
	if (fabs(p2[2])<=CMD_FUZZ){
		if (fabs(p1[2])>CMD_FUZZ)return(0);
	}else if (fabs((p1[2]/p2[2])-1.0) >= CMD_FUZZ){
		return(0);
	}
	return(1);
}

*/

//function to check if an angle is between two other angles....
int cmd_ang_betw(sds_real angchk, sds_real startang, sds_real endang) {
	//returns 1 if angchk between startang and endang
	ic_normang(&startang,&endang);
	ic_normang(&startang,&angchk);
	if (angchk<=endang)return(1);
	else return(0);
}


long cmd_pl_vtx_no(sds_name pent, sds_point ppucs, int elevadj, int *exactvert) {

	//Takes POLYLINE ename and point & returns number of vertex segment closest
	//to point.  Vertex # is 1-based so you can step from main entity.
	//NOTE:  if vertices have been splined into polyline, it will only return
	//	vertex # of a splined vtx - not a control pt vtx.  This doesn't affect
	//	curve-fit plines, because they pass thru the control pts.
	//NOTE: if pp is exactly a vtx pt, f'n will return vtx # BEFORE pp.  Calling
	//	f'ns need this so that they don't make duplicate vtx pts at pp
	// PPUCS is a user-picked UCS POINT.  It gets transformed into the coord system
	//		native to the selected pline
	//if elevadj!=0 and the pline is 2d, the pickpt in ucs coords is adjusted so that
	//	it lies in the plane of the pline.	the adjustment is made in the direction of
	//	the VIEWDIR variable.

	// 9/3/98 -- added "exactvert" return if the vertex is picked exactly, so that we can
	// use the actual vertex if desired.  Exactvert returns the same as the function if no exact vert is found.

	struct resbuf *rbp1, *rbp2,rbucs,rbent,rb;
	long fi1, ret;
	int plflag=0,gotstart=0;
	sds_name evtx1, evtx2;
	sds_point pp,p0,p1,startpt;
	sds_real fr1, fr2;
	char fs1[20];

	*exactvert=-1;
	rbp1=sds_entget(pent);
	ic_assoc(rbp1,0);
   if (!strsame(ic_rbassoc->resval.rstring,"POLYLINE"/*DNT*/ )) return(-1);
	if (ic_assoc(rbp1,70)==0)
		plflag=ic_rbassoc->resval.rint;
	else
		plflag=0;
	rbucs.restype=RTSHORT;
	rbucs.resval.rint=1;
	if (!(plflag&(IC_PLINE_3DPLINE | IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH))){
		//2dpline
		rbent.restype=RT3DPOINT;
		ic_assoc(rbp1,210);
		ic_ptcpy(rbent.resval.rpoint,ic_rbassoc->resval.rpoint);
		sds_trans(ppucs,&rbucs,&rbent,0,pp);
		if (elevadj){
			SDS_getvar(NULL,DB_QVIEWDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			sds_trans(rb.resval.rpoint,&rbucs,&rbent,1,rb.resval.rpoint);
			ic_assoc(rbp1,10);
			if (ic_rbassoc->resval.rpoint[2]!=pp[2] && fabs(rb.resval.rpoint[2]>CMD_FUZZ)){
				pp[0]+=(ic_rbassoc->resval.rpoint[2]-pp[2])*rb.resval.rpoint[0]/rb.resval.rpoint[2];
				pp[1]+=(ic_rbassoc->resval.rpoint[2]-pp[2])*rb.resval.rpoint[1]/rb.resval.rpoint[2];
				pp[2]=ic_rbassoc->resval.rpoint[2];
			}
		}
	}else{
		rbent.restype=RTSHORT;
		rbent.resval.rint=0;
		sds_trans(ppucs,&rbucs,&rbent,0,pp);
	}

	sds_relrb(rbp1); rbp1=NULL;
	sds_entnext_noxref(pent,evtx1);
	rbp1=sds_entget(evtx1);
	fi1=0;fr2=-1.0;
	do{
		fi1++;
		if (ic_assoc(rbp1,10)) return(-1);
		ic_ptcpy(p0,ic_rbassoc->resval.rpoint);
		if (RTNORM!=sds_entnext_noxref(evtx1,evtx2)){ret=-1;goto exit;}
		if ((rbp2=sds_entget(evtx2))==NULL){ret=-1;goto exit;}
		ic_assoc(rbp2,0);
		strncpy(fs1,ic_rbassoc->resval.rstring,sizeof(fs1)-1);
		if (gotstart==0){
			if (plflag & IC_PLINE_SPLINEFIT){
				ic_assoc(rbp1,70);
				if (0==(ic_rbassoc->resval.rint & IC_VERTEX_SPLINEFRAME)){
					ic_ptcpy(startpt,p0);
					gotstart=1;
				}else{
					goto skiphere;
				}
			}else{
				ic_ptcpy(startpt,p0);
				gotstart=1;
			}
		}
		if (plflag & IC_PLINE_SPLINEFIT){
			if ((0==ic_assoc(rbp1,70)) && (ic_rbassoc->resval.rint & IC_VERTEX_SPLINEFRAME))goto skiphere;
			if ((0==ic_assoc(rbp2,70)) && (ic_rbassoc->resval.rint & IC_VERTEX_SPLINEFRAME))goto skiphere;
		}
	   if (strisame(fs1,"SEQEND"/*DNT*/ )){
			if (0==(IC_PLINE_CLOSED & plflag))
				break;
			else
				ic_ptcpy(p1,startpt);
		}else{
			ic_assoc(rbp2,10);
			ic_ptcpy(p1,ic_rbassoc->resval.rpoint);
		}
		ic_assoc(rbp1,42);
		fr1=ic_rbassoc->resval.rreal;
		ic_ptsegdist(pp,p0,p1,ic_rbassoc->resval.rreal,&fr1);
		if ((fr2<0.0)||(fr1<fr2)){
			ret=fi1;
			fr2=fr1;
			if (icadRealEqual(fr2,0.0)) {
				if (icadPointEqual(pp, p0)) *exactvert=fi1;
				else if (icadPointEqual(pp, p1)) *exactvert=fi1+1;
			}
			else {
				// determine if the selected point was closer to the start or end vertex point of the segment
				if ( (sds_distance(pp, p0)) < (sds_distance(pp, p1)) )
					*exactvert=fi1;
				else
					*exactvert=fi1+1;
			}
		}
		skiphere:
		sds_relrb(rbp1);
		rbp1=rbp2; rbp2=NULL;
		ic_encpy(evtx1,evtx2);
   }while (strsame(fs1,"VERTEX"/*DNT*/ ));
exit:
	if (rbp1!=NULL)sds_relrb(rbp1);
	if (rbp2!=NULL)sds_relrb(rbp2);
	rbp1=rbp2=NULL;
	return(ret);
}

long cmd_lwpl_vtx_no(sds_name pent, sds_point ppucs, int elevadj, int *exactvert)
	{

	//Takes LWPOLYLINE ename and point & returns number of vertex segment closest
	//to point.  Vertex # is 0-based, not 1-based like polyline
	//NOTE: if pp is exactly a vtx pt, f'n will return vtx # BEFORE pp.  Calling
	//	f'ns need this so that they don't make duplicate vtx pts at pp
	// PPUCS is a user-picked UCS POINT.  It gets transformed into the coord system
	//		native to the selected pline
	//if elevadj!=0 and the pline is 2d, the pickpt in ucs coords is adjusted so that
	//	it lies in the plane of the pline.	the adjustment is made in the direction of
	//	the VIEWDIR variable.

	resbuf rbucs,rbent,rb;
	long ret;
	int plflag=0,gotstart=0,nverts;
	sds_point pp,p0,p1,startpt;
	sds_real fr1, mindist=1.0E300;
	db_lwpolyline *lwpline=(db_lwpolyline *)pent[0];
	sds_real bulge;

	rbucs.restype=RTSHORT;
	rbucs.resval.rint=1;

	*exactvert=-1;
	if (((db_handitem *)(pent[0]))->ret_type()!=DB_LWPOLYLINE)
		return(-1);
	lwpline->get_70(&plflag);

	//2dpline
	rbent.restype=RT3DPOINT;
	lwpline->get_210(rbent.resval.rpoint);
	sds_trans(ppucs,&rbucs,&rbent,0,pp);
	if (elevadj)
		{
		SDS_getvar(NULL,DB_QVIEWDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		sds_trans(rb.resval.rpoint,&rbucs,&rbent,1,rb.resval.rpoint);
		sds_real lwplelev;
		lwpline->get_38(&lwplelev);
		if (lwplelev!=pp[2] && fabs(rb.resval.rpoint[2]>CMD_FUZZ))
			{
			pp[0]+=(lwplelev-pp[2])*rb.resval.rpoint[0]/rb.resval.rpoint[2];
			pp[1]+=(lwplelev-pp[2])*rb.resval.rpoint[1]/rb.resval.rpoint[2];
			pp[2]=lwplelev;
			}
		}

	lwpline->get_90(&nverts);
	int vertstoprocess=nverts;
	if (plflag & IC_PLINE_CLOSED)
		vertstoprocess++;
	for (int i=0; i<vertstoprocess; i++)
		{
		lwpline->get_10(p0,i);
		if (gotstart==0)
			{
			ic_ptcpy(startpt,p0);
			gotstart=1;
			}
		if (i==nverts-1) // off the end
			{
			if (0==(IC_PLINE_CLOSED & plflag))
				break;
			else
				ic_ptcpy(p1,startpt);
			}
		else
			lwpline->get_10(p1,i+1);
		lwpline->get_42(&bulge,i);
		ic_ptsegdist(pp,p0,p1,bulge,&fr1);
		if (fr1<mindist)
			{
			ret=i;
			mindist=fr1;
			if (icadRealEqual(mindist,0.0))
				{
				if (icadPointEqual(pp,p0))
					*exactvert=i;
				else if (icadPointEqual(pp,p1))
					*exactvert=i+1;
				}
			else
				*exactvert=i;
			}
		}
	return(ret);
}

void cmd_pt_2_plseg (sds_name vtx1, sds_point pt){
	//f'n to take a pline vertex and point and move point to closest place
	// on vertex seg.  if entnext is SEQEND, pline assumed closed and f'n
	//gets start point of pline as end point of vtx1 segment.

	struct resbuf *rbp1, *rbp2;
	sds_real fr1,fr2,fr3,fr4;
	sds_point p0,p1,p2;
	sds_name e0;

	rbp1=sds_entget(vtx1);
	ic_assoc(rbp1,42);
	fr1=ic_rbassoc->resval.rreal;
	ic_assoc(rbp1,10);
	ic_ptcpy(p0,ic_rbassoc->resval.rpoint);
	sds_entnext_noxref(vtx1,e0);
	rbp2=sds_entget(e0);
	ic_assoc(rbp2,0);
	//if entnext vtx1 is another vertex, get the point
   if (strsame(ic_rbassoc->resval.rstring,"SEQEND"/*DNT*/ )){
		ic_assoc(rbp2,-2);
		ic_encpy(e0,ic_rbassoc->resval.rlname);
		sds_relrb(rbp2);
		rbp2=NULL;
		sds_entnext_noxref(e0,e0);
		rbp2=sds_entget(e0);
	}
	ic_assoc(rbp2,10);
	ic_ptcpy(p1,ic_rbassoc->resval.rpoint);

	if (icadPointEqual(p0,p1)){
		ic_ptcpy(pt,p0);
	}else if (fr1!=0){
		ic_bulge2arc(p0,p1,fr1,p2,&fr2,&fr3,&fr4);
		p1[0]=fr2;
		p1[1]=fr3;
		p1[2]=fr4;
		ic_segdist(pt,p2,p1,1,&fr2,p0);
		ic_ptcpy(pt,p0);
	}else{
		ic_segdist(pt,p0,p1,0,&fr2,p2);
		ic_ptcpy(pt,p2);
	}
	sds_relrb(rbp1);
	sds_relrb(rbp2);
	rbp1=rbp2=NULL;
}

void cmd_pt_2_lwplseg (db_lwpolyline *lwpline, int vertno, sds_point pt)
	{
	//f'n to take a pline vertex and point and move point to closest place
	// on vertex seg.  if entnext is SEQEND, pline assumed closed and f'n
	//gets start point of pline as end point of vtx1 segment.

	sds_real fr1,fr2,fr3,fr4;
	sds_point p0,p1,p2;
	int nverts;

	lwpline->get_90(&nverts);
	lwpline->get_42(&fr1,vertno);
	lwpline->get_10(p0,vertno);

	// get the next vert, either from the start, or the one after this one
	if (vertno==nverts-1)
		lwpline->get_10(p1,0);
	else
		lwpline->get_10(p1,vertno+1);

	if (icadPointEqual(p0,p1))
		ic_ptcpy(pt,p0);
	else if (fr1!=0)
		{
		ic_bulge2arc(p0,p1,fr1,p2,&fr2,&fr3,&fr4);
		p1[0]=fr2;
		p1[1]=fr3;
		p1[2]=fr4;
		ic_segdist(pt,p2,p1,1,&fr2,p0);
		ic_ptcpy(pt,p0);
		}
	else
		{
		ic_segdist(pt,p0,p1,0,&fr2,p2);
		ic_ptcpy(pt,p2);
		}
	}

int cmd_get_startend(sds_name ent, sds_point startpt, sds_point endpt){
	//function to get start and end points of polyline
	sds_point ptarray[2];
	int ret;

	ret=db_getendpts(ent,ptarray);
	ic_ptcpy(startpt,ptarray[0]);
	ic_ptcpy(endpt,ptarray[1]);
	return(ret);
}


int cmd_make_pline_end(sds_name ename, int closeflag, sds_point closept){
	//this does an entmake on all vertexes & seqend AFTER ename without modification to
	//	finish pline build.
	//If closeflag&1, an extra vertex will be created at next appropriate
	//	starting vtx of pline
	//if closeflag&(4+16), only spline vertices will be created.
	//	otherwise spline vertices are skipped
	//Do NOT pass this function a SEQEND entity!
	struct resbuf *rbp1,*rbp2;
	sds_name ent;
	int ret;

	rbp1=NULL;
	//move to next ent
	sds_entnext_noxref(ename,ent);
	if ((rbp1=sds_entget(ent))==NULL) return(-1);
	ic_assoc(rbp1,0);
	//as long as there are vertexes, make 'em
   while (strsame(ic_rbassoc->resval.rstring,"VERTEX"/*DNT*/ )) {
		ic_assoc(rbp1,70);
		if (closeflag&20){
			if (ic_rbassoc->resval.rint & 16)
				ret=sds_entmake(rbp1);
		}else{
			if (0==(ic_rbassoc->resval.rint & 16))
				ret=sds_entmake(rbp1);
		}
		sds_relrb(rbp1);
		rbp1=NULL;
		if (ret!=RTNORM) return(-1);
		sds_entnext_noxref(ent,ent);
		if ((rbp1=sds_entget(ent))==NULL) return(-1);
		ic_assoc(rbp1,0);
	}
	//rbp1 now points to seqend list
	//if we need to make one last vertex, do it
	if (closeflag&1){
	   rbp2=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,10,closept,70,(closeflag&20)?16:0,0);
		ret=sds_entmake(rbp2);
		sds_relrb(rbp2);
		rbp2=NULL;
		if (ret!=RTNORM)return(-1);
	}
	//make seqend marker
	ret=sds_entmake(rbp1);
	if (rbp1!=NULL){
		sds_relrb(rbp1);
		rbp1=NULL;
	}
	if (ret!=RTNORM)
		return(-1);
	else
		return(0);
}

int cmd_make_pline_start(sds_name startent, sds_name endent){
	//this function takes a polyline entity, entmakes it, and entmakes everything up to BUT NOT IN-
	//CLUDING the vertex endent.  Endent is the vertex you'll be modifying data for.  You MUST pass
	//this routine a polyline startent.
	//NOTES: 70 flags for closure and spline-fit vertices (1 & 4) ALWAYS SET OFF by this f'n.
	//		 No spline control point vertices are made by this f'n
	struct resbuf *rbp1,rb;
	sds_name ent;

	rb.restype=RTSTR;
	rb.resval.rstring="*"/*DNT*/;
	rb.rbnext=NULL;
	rbp1=NULL,
	sds_entmake(NULL);
	ic_encpy(ent,startent);
	if ((rbp1=sds_entgetx(ent,&rb))==NULL)return(-1);
	//if main ent has closure flag on, set it off
	if (!ic_assoc(rbp1,70)){
		if (ic_rbassoc->resval.rint & 1) ic_rbassoc->resval.rint--;  //open pline
		if (ic_rbassoc->resval.rint & 4) ic_rbassoc->resval.rint-=4; //no splines
	}
	do{
		ic_assoc(rbp1,70);
		if (0==(ic_rbassoc->resval.rint & 16)){  //assure it's not a spline ctrl vtx
			if (sds_entmake(rbp1)!=RTNORM){sds_relrb(rbp1); return(-1);}
		}
		if (rbp1!=NULL){
			sds_relrb(rbp1);
			rbp1=NULL;
		}
		sds_entnext_noxref(ent,ent);
		if ((rbp1=sds_entget(ent))==NULL)return(-1);
		ic_assoc(rbp1,-1);
	}while ((endent[0]!=ic_rbassoc->resval.rlname[0])||(endent[1]!=ic_rbassoc->resval.rlname[1]));
	if (rbp1!=NULL){
		sds_relrb(rbp1);
		rbp1=NULL;
	}
	return(0);
}


//***************END SUPPORT FUNCTIONS **************************************


