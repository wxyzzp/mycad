/* C:\DEV\LIB\CMDS\CMDS3.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */



//** Includes
#include "cmds.h"/*DNT*/
#include "objects.h"/*DNT*/
#include "Icad.h"/*DNT*/
#include "IcadDoc.h"/*DNT*/
#include "UCSIcon.h"
#include "splines.h"/*DNT*/
#include <errno.h>
#include <assert.h>
#include "commandqueue.h"/*DNT*/
#include "IcadView.h"/*DNT*/
#include "..\db\styletabrec.h"/*DNT*/
#include "IcadTextScrn.h"/*DNT*/
#include "Modeler.h"/*DNT*/
#include "DbAcisEntity.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if defined(SDS_NEWDB)
	#define gra_ucs2rp		gr_ucs2rp
	#define gra_rp2ucs		gr_rp2ucs
	#define gra_view		gr_view
	#define dwg_entlink		db_handitem
	#define dwg_filelink	db_drawing
	#define gra_initview	gr_initview
	#define gra_twist4ucs	gr_twist4ucs
#endif

//** Defines (for MultiLine Edit functions)
#define	MLEDIT_CLOSEDX		0
#define	MLEDIT_OPENX		1
#define	MLEDIT_MERGEDX		2
#define	MLEDIT_CLOSEDT		3
#define	MLEDIT_OPENT		4
#define	MLEDIT_MERGEDT		5
#define	MLEDIT_CORNER		6
#define	MLEDIT_ADDVERT		7
#define	MLEDIT_DELVERT		8
#define	MLEDIT_CUTSINGLE	9
#define	MLEDIT_CUTALL		10
#define	MLEDIT_HEAL			11
//** Defines used for creating Spline Ents which I can't find anywhere
//		in sysvars, and I can't find any way (as user) to alter.
#define SPLINE_KNOT_TOL		1.0e-007
#define SPLINE_CTRL_TOL		1.0e-007


//*** vars

//** Global Variables
extern struct cmd_drag_globalpacket cmd_drag_gpak;	//structure def moved to header file...
extern int	  lsp_cmdhasmore;
struct SDS_mTextData  SDS_mTextDataPtr;
extern double SDS_CorrectElevation;
extern bool	  SDS_ElevationChanged;

//** Prototypes
void cmd_OnOpenOK(HWND hDlg);

char *cmd_LinSavePath;

sds_real cmd_vpoint_xyplanang;
bool	cmd_vpoint_xyplanang_init;


struct cmd_mline_globalpacket {
	struct resbuf *arcdxf;
	sds_real stwid,endwid,bulge;
	short numents;
} cmd_mline_gpak;

struct cmd_mline_list { //*** This structure is for the linked list that keeps track
						//*** of all segments of the multiline.
	sds_name ename; 	//*** Segment's ename.
	sds_point vertexpt; //*** Segment's vertex point.
	sds_real stwidth;	//*** Segment's start width.
	sds_real endwidth;	//*** Segment's ending width.
	sds_real bulge; 	//*** Segment's bulge (huh-huh).
	sds_real lastang;	//*** Segment's angle. (used with line Length)
	struct cmd_mline_list *prev,*next;
} *cmd_mlinebeg,*cmd_mlinecur;

//** Protos
void  cmd_area_display(sds_real area, sds_real perimeter, sds_real *totarea, sds_real *totlen,short addmode, short circleflg);
short cmd_viewdelete(void);
short cmd_viewlister(void);
short cmd_viewrestore(void);
short cmd_viewsave(void);
short cmd_ltypelister(void);
short cmd_ltypefilelister(void);
short cmd_ltypeloader(void);
short cmd_ltypecreate(void);
short cmd_ltypeset(void);
short cmd_validlt(char *ss);
short cmd_do_xline_ray(int mode);
long  cmd_mline_addsegment(sds_point pt1,sds_point pt2,sds_real rCurElev);
long  cmd_mlstyle_list(void);
int   cmd_mledit_intersections(int nMLEditType, sds_name nmFirst, sds_point ptFirst, sds_name nmSecond, sds_point ptSecond);
int   cmd_mledit_vertices(int nMLEditType, sds_name nmFirst, sds_point ptFirst);
int   cmd_mledit_cut_and_heal(int nMLEditType, sds_name nmFirst, sds_point ptFirst, sds_point ptSecond);
int   cmd_Xrefblk2Blk(sds_name firstblkent,int mode);

sds_real sds_dist2d (sds_point p1,sds_point p2);//proto for f'n found in sds1



//================================================================================================

// Functions

//================================================================================================

short cmd_ray(void) {
	RT ret;

	if (cmd_iswindowopen() == RTERROR)
		return RTERROR;

	//BugZilla No. 78050; 25-03-2002
	if (cmd_istransparent() == RTERROR) return RTERROR;

	ret=cmd_do_xline_ray(DB_RAY);

	return(ret);
}

short cmd_xline(void) {
	RT ret;

	if (cmd_iswindowopen() == RTERROR)
		return RTERROR;

	//BugZilla No. 78050; 25-03-2002
	if (cmd_istransparent() == RTERROR) return RTERROR;

	ret=cmd_do_xline_ray(DB_XLINE);

	return(ret);
}


short cmd_do_xline_ray(int mode) {
	sds_point pt1={0.0,0.0,0.0},pt2,tmppt,startpt,endpt,unitvec={0.0,0.0,0.0},vertpt,pt210;//extru;
	RT ret=RTNORM,flag70;
	char fs1[IC_ACADBUF],szPrompt[IC_ACADBUF],szAng[IC_ACADBUF],szOffdist[IC_ACADBUF];
	long vertex_no,i;
	double static ang=0.0;
	sds_real objang,off;
	sds_name ename,objname,subobjname;
	struct resbuf *elist=NULL,rb_drag,rb_last,*objrb=NULL,*tmprb=NULL,rbucs,rbwcs,rbent,rbtmp,erRb;
	dwg_entlink *elp=NULL;
	int cecolor,orthomode,fi1,fi2;
	bool ortho_reset=FALSE;
	int exactvert;

	if (RTNORM!=(SDS_getvar(NULL,DB_QORTHOMODE,&rb_last,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)))
		return(RTERROR);
	orthomode=rb_last.resval.rint;
	SDS_getvar(NULL,DB_QUCSXDIR,&rb_last,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	ic_ptcpy(pt1,rb_last.resval.rpoint);
	SDS_getvar(NULL,DB_QUCSYDIR,&rb_last,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	ic_ptcpy(pt2,rb_last.resval.rpoint);
	pt210[0]=pt1[1]*pt2[2]-pt2[1]*pt1[2];
	pt210[2]=pt1[0]*pt2[1]-pt2[0]*pt1[1];
	pt210[1]=pt1[2]*pt2[0]-pt2[2]*pt1[0];

	//in certain cases we need to get cecolor for dragging purposes
	//get current color for ent and we'll see if it overlays ok w/crosshairs
	SDS_getvar(NULL,DB_QCECOLOR,&rb_last,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	fi1=cecolor=ic_colornum(rb_last.resval.rstring);
	IC_FREE(rb_last.resval.rstring);
	if (fi1==256 || fi1==0){
		SDS_getvar(NULL,DB_QCLAYER,&rb_last,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		if (tmprb!=NULL){sds_relrb(tmprb);tmprb=NULL;}
	   tmprb=sds_tblsearch("LAYER"/*DNT*/,rb_last.resval.rstring,0);
		ic_assoc(tmprb,62);
		cecolor=fi1=1+(ic_rbassoc->resval.rint%7);
		sds_relrb(tmprb);tmprb=NULL;
		IC_FREE(rb_last.resval.rstring);
	}
	SDS_getvar(NULL,DB_QCOLORX,&rb_last,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	fi2=rb_last.resval.rint;
	SDS_getvar(NULL,DB_QCOLORY,&rb_last,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	while(fi1==cecolor || fi2==cecolor || rb_last.resval.rint==cecolor){
		cecolor=1+(cecolor%7);
	}

	rbucs.restype=rbwcs.restype=RTSHORT;
	rbucs.resval.rint=1;rbwcs.resval.rint=0;
	if (mode==DB_XLINE) {
		elp=new db_xline(SDS_CURDWG);
		elp->set_10(pt1);
		elp->set_11(pt1);
		ename[0]=(long)elp;
		ename[1]=(long)SDS_CURDWG;
	}else{
		elp=new db_ray(SDS_CURDWG);
		elp->set_10(pt1);
		elp->set_11(pt1);
		ename[0]=(long)elp;
		ename[1]=(long)SDS_CURDWG;
	}
	//make sure successive drags don't paint out on overlay
	if (orthomode)elp->set_62(cecolor);

	SDS_getvar(NULL,DB_QDRAGMODE,&rb_drag,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);


//	  sds_getvar("LASTANGLE",&rb_last);
//	  ang=rb_last.resval.rreal;
	sds_angtos(ang,-1,-1,szAng);

	sds_getvar("OFFSETDIST"/*DNT*/,&rb_last);
		off=rb_last.resval.rreal;
	if (off<0.0) {
		strcpy(szOffdist,ResourceString(IDC_CMDS3_THROUGH_2, "Through" ));
	}else{
		sds_rtos(off,-1,-1,szOffdist);
	}

	sds_initget(0, ResourceString(IDC_XLINERAY_INIT1, "Horizontal Vertical ~ Angle Bisect ~ Parallel ~Offset ~_Horizontal ~_Vertical ~_ ~_Angle ~_Bisect ~_ ~_Parallel ~_Offset"));
	if (mode==DB_XLINE) {
		if ((ret=(sds_getpoint(NULL,ResourceString(IDC_CMDS3__NINFINITE_LINE____4, "\nInfinite line:  Bisect/Horizontal/Vertical/Angle/Parallel/<Point along Line>: " ),pt1))) == RTCAN)
			goto bail;
	}else{
		if ((ret=(sds_getpoint(NULL,ResourceString(IDC_CMDS3__NINFINITE_RAY___B_5, "\nInfinite ray:  Bisect/Horizontal/Vertical/Angle/Parallel/<Start of Ray>: " ),pt1))) == RTCAN)
			goto bail;
	}

	if (ret==RTNORM){
		ic_ptcpy(rb_last.resval.rpoint,pt1);
		rb_last.restype=RT3DPOINT;
		sds_trans(rb_last.resval.rpoint,&rbucs,&rbwcs,0,elp->retp_10());
		while((ret=SDS_dragobject(-7,rb_drag.resval.rint,(sds_real *)ename,(double *)elp->retp_11(),0.0,ResourceString(IDC_CMDS3__NDIRECTION___6, "\nDirection: " ),NULL,pt2,NULL))==RTNORM) {
			SDS_setvar(NULL,DB_QLASTPOINT,&rb_last,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
			pt2[0]-=pt1[0];
			pt2[1]-=pt1[1];
			pt2[2]-=pt1[2];
			if ((tmppt[0]=sqrt(pt2[0]*pt2[0]+pt2[1]*pt2[1]+pt2[2]*pt2[2]))!=1.0){
				if (tmppt[0]<CMD_FUZZ)continue;
				pt2[0]/=tmppt[0];
				pt2[1]/=tmppt[0];
				pt2[2]/=tmppt[0];
			}
			sds_trans(pt2,&rbucs,&rbwcs,1,elp->retp_11());
			if (mode==DB_XLINE) {
				elist=sds_buildlist(RTDXF0,"XLINE"/*DNT*/,10,elp->retp_10(),11,elp->retp_11(),210,pt210,0);
			}else{
				elist=sds_buildlist(RTDXF0,"RAY"/*DNT*/,10,elp->retp_10(),11,elp->retp_11(),210,pt210,0);
			}
			sds_entmake(elist);
			sds_relrb(elist);elist=NULL;
		}
		SDS_setvar(NULL,DB_QLASTPOINT,&rb_last,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
	}
	if (ret==RTKWORD){
		sds_getinput(fs1);
	if (!strisame(fs1,"BISECT"/*DNT*/)) {
			//unless we're bisecting, set ortho mode off
			if (RTNORM!=(SDS_getvar(NULL,DB_QORTHOMODE,&rb_last,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)))
				return(RTERROR);
			orthomode=rb_last.resval.rint;
			rb_last.resval.rint=0;
			SDS_setvar(NULL,DB_QORTHOMODE,&rb_last,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,1);
			ortho_reset=TRUE;
		}
		if (strisame(fs1,"HORIZONTAL"/*DNT*/)) {
			//set alternate dragging color
			elp->set_62(cecolor);
			pt2[0]=1.0;pt2[1]=0.0;pt2[2]=0.0;
			sds_trans(pt2,&rbucs,&rbwcs,1,elp->retp_11());
			while((ret=SDS_dragobject(-1,rb_drag.resval.rint,(sds_real *)ename,(double *)elp->retp_10(),0.0,ResourceString(IDC_CMDS3__NLOCATION___11, "\nLocation: " ),NULL,pt2,NULL))==RTNORM) {
				sds_trans(pt2,&rbucs,&rbwcs,0,pt2);
				if (mode==DB_XLINE) {
					elist=sds_buildlist(RTDXF0,"XLINE"/*DNT*/,10,pt2,11,elp->retp_11(),210,pt210,0);
				}else{
					elist=sds_buildlist(RTDXF0,"RAY"/*DNT*/,10,pt2,11,elp->retp_11(),210,pt210,0);
				}
				sds_entmake(elist);
				sds_relrb(elist);elist=NULL;
			}
		} else if (strisame(fs1,"VERTICAL"/*DNT*/)) {
			//set alternate dragging color
			elp->set_62(cecolor);
			pt2[0]=0.0;pt2[1]=1.0;pt2[2]=0.0;
			sds_trans(pt2,&rbucs,&rbwcs,1,elp->retp_11());
			while((ret=SDS_dragobject(-1,rb_drag.resval.rint,(sds_real *)ename,(double *)elp->retp_10(),0.0,ResourceString(IDC_CMDS3__NLOCATION___11, "\nLocation: " ),NULL,pt2,NULL))==RTNORM) {
				sds_trans(pt2,&rbucs,&rbwcs,0,pt2);
				if (mode==DB_XLINE) {
					elist=sds_buildlist(RTDXF0,"XLINE"/*DNT*/,10,pt2,11,elp->retp_11(),210,pt210,0);
				}else{
					elist=sds_buildlist(RTDXF0,"RAY"/*DNT*/,10,pt2,11,elp->retp_11(),210,pt210,0);
				}
				sds_entmake(elist);
				sds_relrb(elist);elist=NULL;
			}
		} else if (strisame(fs1,"ANGLE"/*DNT*/)) {
			sprintf(szPrompt,ResourceString(IDC_CMDS3__NREFERENCE____EN_14, "\nReference/<Enter angle (%s)>: " ),szAng);
			sds_initget(0, ResourceString(IDC_XLINERAY_ANGLE_INIT1, "Reference ~_Reference"));
			if ((ret=sds_getangle(NULL,szPrompt,&ang))==RTCAN)
				goto bail;

			if (ret==RTNORM || ret==RTNONE) {
				ic_normang(&ang,NULL);
				for(i=0;i<4;i++){
					if (icadAngleEqual(ang,i*0.5*IC_PI)){
						elp->set_62(cecolor);
						break;
					}
				}
				//Handle angbase & angdir
				double angbase;
				struct resbuf rb;
				SDS_getvar(NULL,DB_QANGBASE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				angbase = rb.resval.rreal;
				SDS_getvar(NULL,DB_QANGDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				if(rb.resval.rint)  // support CW direction was added
					angbase = IC_TWOPI - angbase;
				if (rb.resval.rint)
					ang -= angbase;
				else
					ang += angbase;

				pt2[0]=cos(ang);pt2[1]=sin(ang);pt2[2]=0.0;
				sds_trans(pt2,&rbucs,&rbwcs,1,elp->retp_11());
				while((ret=SDS_dragobject(-1,rb_drag.resval.rint,(sds_real *)ename,(double *)elp->retp_10(),0.0,ResourceString(IDC_CMDS3__NLOCATION___11, "\nLocation: " ),NULL,pt2,NULL))==RTNORM) {
					sds_trans(pt2,&rbucs,&rbwcs,0,pt2);
					if (mode==DB_XLINE) {
						elist=sds_buildlist(RTDXF0,"XLINE"/*DNT*/,10,pt2,11,elp->retp_11(),210,pt210,0);
					}else{
						elist=sds_buildlist(RTDXF0,"RAY"/*DNT*/,10,pt2,11,elp->retp_11(),210,pt210,0);
					}
					sds_entmake(elist);
					sds_relrb(elist);elist=NULL;
				}
			}else if (ret==RTKWORD) {
				sds_getinput(fs1);
				if (strisame(fs1,"REFERENCE"/*DNT*/)) {
					while(1) {
						while(1){
							if ((ret=sds_entsel(ResourceString(IDC_CMDS3__NSELECT_REFERENC_16, "\nSelect reference entity: " ),objname,tmppt))==RTNORM) {
								break;
							}else if (ret==RTCAN){
								goto bail;
							}else if (ret==RTERROR){
								SDS_getvar(NULL,DB_QERRNO,&erRb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
								if (erRb.resval.rint==OL_ENTSELPICK){
									cmd_ErrorPrompt(CMD_ERR_NOENT,0);
								}else if (erRb.resval.rint==OL_ENTSELNULL){
									ret=RTNORM;
									goto bail;
								}
							}
						}
						if ((objrb=sds_entget(objname))==NULL)
							goto bail;
						ic_assoc(objrb,0);
						if (strisame(db_hitype2str(DB_LINE),ic_rbassoc->resval.rstring)) {
							ic_assoc(objrb,10);
							sds_trans(ic_rbassoc->resval.rpoint,&rbwcs,&rbucs,0,startpt);
							ic_assoc(objrb,11);
							sds_trans(ic_rbassoc->resval.rpoint,&rbwcs,&rbucs,0,endpt);

							objang=sds_angle(startpt,endpt);

							sprintf(szPrompt,ResourceString(IDC_CMDS3__N_ENTER_ANGLE____17, "\n<Enter angle (%s)>: " ),szAng);
							if ((ret=sds_getangle(NULL,szPrompt,&ang))==RTCAN)
								goto bail;

							pt2[0]=cos(ang+objang);pt2[1]=sin(ang+objang);pt2[2]=0.0;
							sds_trans(pt2,&rbucs,&rbwcs,1,elp->retp_11());
							while((ret=SDS_dragobject(-1,rb_drag.resval.rint,(sds_real *)ename,(double *)elp->retp_10(),0.0,ResourceString(IDC_CMDS3__NLOCATION___11, "\nLocation: " ),NULL,pt2,NULL))==RTNORM) {
								sds_trans(pt2,&rbucs,&rbwcs,0,pt2);
								if (mode==DB_XLINE) {
									elist=sds_buildlist(RTDXF0,"XLINE"/*DNT*/,10,pt2,11,elp->retp_11(),210,pt210,0);
								}else{
									elist=sds_buildlist(RTDXF0,"RAY"/*DNT*/,10,pt2,11,elp->retp_11(),210,pt210,0);
								}
								sds_entmake(elist);
								IC_RELRB(elist);
							}
							IC_RELRB(objrb);
							break;
						}else if (strisame("RAY"/*DNT*/,ic_rbassoc->resval.rstring) || strisame("XLINE"/*DNT*/,ic_rbassoc->resval.rstring)) {
							ic_assoc(objrb,11);
							sds_trans(ic_rbassoc->resval.rpoint,&rbwcs,&rbucs,1,endpt);
							objang=atan2(endpt[1],endpt[0]);

							sprintf(szPrompt,ResourceString(IDC_CMDS3__NENTER_ANGLE___S_18, "\nEnter angle <%s>: " ),szAng);
							if ((ret=sds_getangle(NULL,szPrompt,&ang))==RTCAN)
								goto bail;

							pt2[0]=cos(ang+objang);pt2[1]=sin(ang+objang);pt2[2]=0.0;
							sds_trans(pt2,&rbucs,&rbwcs,1,elp->retp_11());
							while((ret=SDS_dragobject(-1,rb_drag.resval.rint,(sds_real *)ename,(double *)elp->retp_10(),0.0,ResourceString(IDC_CMDS3__NLOCATION___11, "\nLocation: " ),NULL,pt2,NULL))==RTNORM) {
								sds_trans(pt2,&rbucs,&rbwcs,0,pt2);
								if (mode==DB_XLINE) {
									elist=sds_buildlist(RTDXF0,"XLINE"/*DNT*/,10,pt2,11,elp->retp_11(),210,pt210,0);
								}else{
									elist=sds_buildlist(RTDXF0,"RAY"/*DNT*/,10,pt2,11,elp->retp_11(),210,pt210,0);
								}
								sds_entmake(elist);
								IC_RELRB(elist);
							}
							IC_RELRB(objrb);
							break;
						}else if (strisame("POLYLINE"/*DNT*/,ic_rbassoc->resval.rstring)) {
							ic_assoc(objrb,70);
							flag70=ic_rbassoc->resval.rint;
							if (ic_assoc(objrb,210)==0){
								rbent.restype=RT3DPOINT;
								ic_ptcpy(rbent.resval.rpoint,ic_rbassoc->resval.rpoint);
							}else{
								rbent.restype=RTSHORT;
								rbent.resval.rint=0;
							}
							IC_RELRB(objrb); //release pline header

							vertex_no=cmd_pl_vtx_no(objname,tmppt,1,&exactvert);
							if ((ret=sds_entnext_noxref(objname,subobjname))!=RTNORM)
								goto bail;
							for(i=1;i<vertex_no;i++) sds_entnext_noxref(subobjname,subobjname);
							if ((objrb=sds_entget(subobjname))==NULL)
								goto bail;
							ic_assoc(objrb,42);
							if (icadRealEqual(ic_rbassoc->resval.rreal,0.0)) {
								ic_assoc(objrb,10);
								sds_trans(ic_rbassoc->resval.rpoint,&rbent,&rbucs,0,startpt);
								IC_RELRB(objrb);
								if (cmd_pline_entnext(subobjname,subobjname,flag70)==0){
									if ((objrb=sds_entget(subobjname))==NULL)
										goto bail;
									ic_assoc(objrb,10);
									sds_trans(ic_rbassoc->resval.rpoint,&rbent,&rbucs,0,endpt);
									objang=sds_angle(startpt,endpt);
									IC_RELRB(objrb);

									sprintf(szPrompt,ResourceString(IDC_CMDS3__NENTER_ANGLE___S_18, "\nEnter angle <%s>: " ),szAng);
									if ((ret=sds_getangle(NULL,szPrompt,&ang))==RTCAN)
										goto bail;

									pt2[0]=cos(ang+objang);pt2[1]=sin(ang+objang);pt2[2]=0.0;
									sds_trans(pt2,&rbucs,&rbwcs,1,elp->retp_11());
									while((ret=SDS_dragobject(-1,rb_drag.resval.rint,(sds_real *)ename,(double *)elp->retp_10(),0.0,ResourceString(IDC_CMDS3__NLOCATION___11, "\nLocation: " ),NULL,pt2,NULL))==RTNORM) {
										sds_trans(pt2,&rbucs,&rbwcs,0,pt2);
										if (mode==DB_XLINE) {
											elist=sds_buildlist(RTDXF0,"XLINE"/*DNT*/,10,pt2,11,elp->retp_11(),210,pt210,0);
										}else{
											elist=sds_buildlist(RTDXF0,"RAY"/*DNT*/,10,pt2,11,elp->retp_11(),210,pt210,0);
										}
										sds_entmake(elist);
										IC_RELRB(elist);
									}
									break;
								}else{
									sds_printf(ResourceString(IDC_CMDS3__NUNABLE_TO_USE_T_20, "\nUnable to use the selected reference entity" ));
								}
							}else{
								sds_printf(ResourceString(IDC_CMDS3__NUNABLE_TO_USE_T_20, "\nUnable to use the selected reference entity" ));
							}
						}else{
							sds_printf(ResourceString(IDC_CMDS3__NUNABLE_TO_USE_T_20, "\nUnable to use the selected reference entity" ));
						}
					} //while
				}
			}
		} else if (strisame(fs1,"BISECT"/*DNT*/)) { //TODO make it split the difference
			sds_initget(0,ResourceString(IDC_XLINERAY_BISECT_INIT1, "Entity ~_Entity"));
			if ((ret=internalGetpoint(NULL,ResourceString(IDC_CMDS3__NENTITY____VERTE_22, "\nEntity/<Vertex point>: " ),vertpt))!=RTNORM && ret!=RTKWORD)
				goto bail;
			if (ret==RTNORM){
				if ((ret=internalGetpoint(NULL,ResourceString(IDC_CMDS3__NBISECTION_ANGLE_23, "\nBisection angle start point: " ),startpt))!=RTNORM)
					goto bail;
				while(1){
					sds_initget(0,NULL);
					if ((ret=internalGetpoint(vertpt,ResourceString(IDC_CMDS3__NBISECTION_ANGLE_24, "\nBisection angle end point: " ),endpt))==RTERROR) {
						goto bail;
					}else if (ret==RTCAN){
						goto bail;
					}else if (ret==RTNORM){
						//set last point
						ic_ptcpy(rb_last.resval.rpoint,vertpt);
						SDS_setvar(NULL,DB_QLASTPOINT,&rb_last,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
						//set to native ucs
						sds_trans(vertpt,&rbucs,&rbwcs,0,vertpt);
						sds_trans(startpt,&rbucs,&rbwcs,0,startpt);
						sds_trans(endpt,&rbucs,&rbwcs,0,endpt);
						//find 210 vector
						cmd_pt_sub(startpt,vertpt,pt1);
						cmd_pt_sub(endpt,vertpt,pt2);
						ic_crossproduct(pt1,pt2,pt210);
						// set rbtmp
						rbtmp.restype=RT3DPOINT;
						ic_ptcpy(rbtmp.resval.rpoint,pt210);
						// move points into new plane
						sds_trans(vertpt,&rbwcs,&rbtmp,0,vertpt);
						sds_trans(startpt,&rbwcs,&rbtmp,0,startpt);
						sds_trans(endpt,&rbwcs,&rbtmp,0,endpt);
						//calc only in x,y plane
						tmppt[0]=sds_angle(vertpt,startpt);
						tmppt[1]=sds_angle(vertpt,endpt);
						ic_normang(&tmppt[0],&tmppt[1]);
						tmppt[2]=tmppt[0]+tmppt[1];
						tmppt[2]*=0.5;
						tmppt[0]=cos(tmppt[2]);
						tmppt[1]=sin(tmppt[2]);
						tmppt[2]=0.0;
						//move tmppt and vertpt back in to wcs
						sds_trans(tmppt,&rbtmp,&rbwcs,1,tmppt);
						sds_trans(vertpt,&rbtmp,&rbwcs,0,vertpt);
						if (mode==DB_XLINE) {
							elist=sds_buildlist(RTDXF0,"XLINE"/*DNT*/,10,vertpt,11,tmppt,210,pt210,0);
						}else{
							elist=sds_buildlist(RTDXF0,"RAY"/*DNT*/,10,vertpt,11,tmppt,210,pt210,0);
						}
						if (RTNORM!=(ret=sds_entmake(elist)))
							goto bail;
						sds_relrb(elist);elist=NULL;
						sds_trans(vertpt,&rbwcs,&rbucs,0,vertpt);
						sds_trans(startpt,&rbtmp,&rbucs,0,startpt);
					}else{
						break;
					}
				}
			}else{
				for(;;){
					if ((ret=sds_entsel(ResourceString(IDC_CMDS3__NSELECT_LINE__AR_26, "\nSelect line, arc, or polyline segment to bisect: " ),ename,pt1))!=RTNORM)
						goto bail;
					if (elist!=NULL){sds_relrb(elist);elist=NULL;}
					if (NULL==(elist=sds_entget(ename)))
						goto bail;
					ic_assoc(elist,0);
					/*
					if (0==ic_assoc(elist,210))
						ic_ptcpy(extru,ic_rbassoc->resval.rpoint);
					else{
						extru[0]=extru[1]=0.0;
						extru[2]=1.0;
					}*/
		  if (strsame(ic_rbassoc->resval.rstring,"POLYLINE"/*DNT*/)){
						ic_assoc(elist,70);
						if (ic_rbassoc->resval.rint&(IC_PLINE_3DPLINE | IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH)){
							cmd_ErrorPrompt(CMD_ERR_2DPOLYLINE,0);
							continue;
						}
						flag70=ic_rbassoc->resval.rint;
						ic_assoc(elist,210);
						ic_ptcpy(pt210,ic_rbassoc->resval.rpoint);
						if ((vertex_no=cmd_pl_vtx_no(ename,pt1,1,&exactvert))<0){
							ret=RTERROR;
							goto bail;
						}
						for(i=0L;i<vertex_no && RTNORM==sds_entnext_noxref(ename,ename); i++);
						if (elist!=NULL){sds_relrb(elist);elist=NULL;}
						if (NULL==(elist=sds_entget(ename)))
							goto bail;
						cmd_pline_entnext(ename,objname,flag70);
						ic_assoc(elist,10);
						ic_ptcpy(pt1,ic_rbassoc->resval.rpoint);
						ic_assoc(elist,42);
						tmppt[0]=ic_rbassoc->resval.rreal;
						sds_relrb(elist);elist=NULL;
						if (NULL==(elist=sds_entget(objname)))
							goto bail;
						ic_assoc(elist,10);
						ic_ptcpy(pt2,ic_rbassoc->resval.rpoint);
						sds_relrb(elist);elist=NULL;
						if (!icadRealEqual(tmppt[0],0.0)){
							ic_bulge2arc(pt1,pt2,tmppt[0],startpt,&endpt[0],&endpt[1],&endpt[2]);
							goto doarc;
						}else{
							//note: for lines, points expected in wcs
							rbent.restype=RT3DPOINT;
							ic_ptcpy(rbent.resval.rpoint,pt210);
							sds_trans(pt1,&rbent,&rbwcs,0,startpt);
							sds_trans(pt2,&rbent,&rbwcs,0,endpt);
							goto doline;
						}
					}else if (strsame(ic_rbassoc->resval.rstring,db_hitype2str(DB_LINE))){
						for(tmprb=elist;tmprb!=NULL;tmprb=tmprb->rbnext){
							if (tmprb->restype==10)ic_ptcpy(startpt,tmprb->resval.rpoint);
							else if (tmprb->restype==11)ic_ptcpy(endpt,tmprb->resval.rpoint);
							else if (tmprb->restype==210)ic_ptcpy(pt210,tmprb->resval.rpoint);
						}
					doline:
						vertpt[0]=startpt[0]+(endpt[0]-startpt[0])/2.0;
						vertpt[1]=startpt[1]+(endpt[1]-startpt[1])/2.0;
						vertpt[2]=startpt[2]+(endpt[2]-startpt[2])/2.0;
						pt1[0]=endpt[0]-startpt[0];
						pt1[1]=endpt[1]-startpt[1];
						pt1[2]=endpt[2]-startpt[2];
						if (mode==DB_XLINE){
							//set unit vector perp to line IN CURRENT UCS XY PLANE
							sds_trans(pt1,&rbwcs,&rbucs,1,pt2);
							pt1[0]=-pt2[1];
							pt1[1]=pt2[0];
							pt1[2]=0.0;
							sds_trans(pt1,&rbucs,&rbwcs,1,pt1);
						}else{
							sds_trans(pt1,&rbwcs,&rbucs,1,pt1);
							pt1[2]=0.0;
							for(;;){
								sds_trans(vertpt,&rbwcs,&rbucs,0,tmppt);
								if (RTNORM!=internalGetpoint(tmppt,ResourceString(IDC_CMDS3__NSELECT_SIDE_FOR_27, "\nSelect side for ray bisection: " ),pt2))
									goto bail;
								sds_trans(startpt,&rbwcs,&rbucs,0,tmppt);
								pt2[0]-=tmppt[0];
								pt2[1]-=tmppt[1];
								pt2[2]=0.0;
								ic_crossproduct(pt1,pt2,tmppt);
								if (icadRealEqual(tmppt[2],0.0) && icadRealEqual(tmppt[1],0.0) & icadRealEqual(tmppt[2],0.0)){
									cmd_ErrorPrompt(CMD_ERR_BADGEOMETRY,0);
									continue;
								}
								ic_crossproduct(tmppt,pt1,pt2);
								//pt2[2]=0.0;//should already be 0.0
								sds_trans(pt2,&rbucs,&rbwcs,1,pt1);
								break;
							}
						}
						tmppt[0]=sqrt(pt1[0]*pt1[0]+pt1[1]*pt1[1]+pt1[2]*pt1[2]);
						pt1[0]/=tmppt[0];
						pt1[1]/=tmppt[0];
						pt1[2]/=tmppt[0];
						if (mode==DB_XLINE) {
							elist=sds_buildlist(RTDXF0,"XLINE"/*DNT*/,10,vertpt,11,pt1,210,pt210,0);
						}else{
							elist=sds_buildlist(RTDXF0,"RAY"/*DNT*/,10,vertpt,11,pt1,210,pt210,0);
						}
						sds_trans(vertpt,&rbwcs,&rbucs,0,rb_last.resval.rpoint);
						SDS_setvar(NULL,DB_QLASTPOINT,&rb_last,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
						if (RTNORM!=(ret=sds_entmake(elist)))
							goto bail;
						sds_relrb(elist);elist=NULL;
		  }else if (strsame(ic_rbassoc->resval.rstring,"ARC"/*DNT*/)){
						for(tmprb=elist;tmprb!=NULL;tmprb=tmprb->rbnext){
							if (tmprb->restype==10)ic_ptcpy(startpt,tmprb->resval.rpoint);
							//else if (tmprb->restype==40)endpt[0]=tmprb->resval.rreal;
							else if (tmprb->restype==50)endpt[1]=tmprb->resval.rreal;
							else if (tmprb->restype==51)endpt[2]=tmprb->resval.rreal;
							else if (tmprb->restype==210)ic_ptcpy(pt210,tmprb->resval.rpoint);
						}
					doarc:
						rbent.restype=RT3DPOINT;
						ic_ptcpy(rbent.resval.rpoint,pt210);
						ic_normang(&endpt[1],&endpt[2]);
						endpt[0]=(endpt[1]+endpt[2])/2.0;
						pt1[0]=cos(endpt[0]);
						pt1[1]=sin(endpt[0]);
						pt1[2]=0.0;
						ic_ptcpy(rb_last.resval.rpoint,startpt);
						rb_last.restype=RT3DPOINT;
						SDS_setvar(NULL,DB_QLASTPOINT,&rb_last,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
						sds_trans(startpt,&rbent,&rbwcs,0,startpt);
						sds_trans(pt1,&rbent,&rbwcs,1,pt1);
						if (mode==DB_XLINE) {
							elist=sds_buildlist(RTDXF0,"XLINE"/*DNT*/,10,startpt,11,pt1,210,pt210,0);
						}else{
							elist=sds_buildlist(RTDXF0,"RAY"/*DNT*/,10,startpt,11,pt1,210,pt210,0);
						}
						if (RTNORM!=(ret=sds_entmake(elist)))
							goto bail;
						sds_relrb(elist);elist=NULL;
					}else if (RTNORM!=sds_wcmatch(ic_rbassoc->resval.rstring,"LINE,ARC,POLYLINE"/*DNT*/)){
						cmd_ErrorPrompt(CMD_ERR_LINEAR,0);
						continue;
					}
				}	//for loop entity selection
			}//else if stmt for bisect

		} else if (strisame(fs1,"PARALLEL"/*DNT*/) || strisame(fs1,"OFFSET"/*DNT*/)) {

			sprintf(szPrompt,ResourceString(IDC_CMDS3__NTHROUGH_POINT___32, "\nThrough point/<Offset distance for parallel infinite line> (%s): " ),szOffdist);
			sds_initget(0,ResourceString(IDC_XLINERAY_PARALLEL_INIT1, "Through_point|Through ~_Through"));
			if ((ret=sds_getdist(NULL,szPrompt,&off))==RTERROR)
				goto bail;

			if (ret==RTCAN) {
				goto bail;
			}else if (ret==RTKWORD) {
				off=-1;
			}

			while(1) {
				while(1){
					if ((ret=sds_entsel(ResourceString(IDC_CMDS3__NSELECT_ENTITY_F_34, "\nSelect entity for parallel infinite line: " ),objname,tmppt))==RTNORM) {
						break;
					}else if (ret==RTCAN){
						goto bail;
					}else if (ret==RTERROR){
						SDS_getvar(NULL,DB_QERRNO,&erRb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
						if (erRb.resval.rint==OL_ENTSELPICK){
							cmd_ErrorPrompt(CMD_ERR_NOENT,0);
						}else if (erRb.resval.rint==OL_ENTSELNULL){
							ret=RTNORM;
							goto bail;
						}
					}
				}
				IC_RELRB(objrb);
				if ((objrb=sds_entget(objname))==NULL)
					goto bail;
				ic_assoc(objrb,0);
				if (strisame(db_hitype2str(DB_LINE),ic_rbassoc->resval.rstring)) {
					ic_assoc(objrb,10);
					sds_trans(ic_rbassoc->resval.rpoint,&rbwcs,&rbucs,0,startpt);
					ic_assoc(objrb,11);
					sds_trans(ic_rbassoc->resval.rpoint,&rbwcs,&rbucs,0,endpt);
					IC_RELRB(objrb);

					cmd_unit_vector(startpt,endpt,unitvec);

					if (off>=0.0) {
						if (internalGetpoint(NULL,ResourceString(IDC_CMDS3__NSIDE_FOR_PARALL_35, "\nSide for parallel infinite line: " ),pt1)!=RTNORM)
							goto bail;
						cmd_unit_vector(startpt,pt1,pt1);

						if (((unitvec[0]*pt1[1])-(unitvec[1]*pt1[0]))<0.0) {
							cmd_offset_linepts(startpt,endpt,off,pt1,pt2);
						}else{
							cmd_offset_linepts(startpt,endpt,-off,pt1,pt2);
						}
					}else{
						if (internalGetpoint(NULL,ResourceString(IDC_CMDS3__NTHROUGH_THE_POI_36, "\nThrough the point: " ),pt1)!=RTNORM)
							goto bail;
					}

					sds_trans(pt1,&rbucs,&rbwcs,0,pt1);
					sds_trans(unitvec,&rbucs,&rbwcs,1,unitvec);
					if (mode==DB_XLINE) {
						elist=sds_buildlist(RTDXF0,"XLINE"/*DNT*/,10,pt1,11,unitvec,210,pt210,0);
					}else{
						elist=sds_buildlist(RTDXF0,"RAY"/*DNT*/,10,pt1,11,unitvec,210,pt210,0);
					}
					sds_entmake(elist);
					sds_relrb(elist);elist=NULL;

				}else if (strisame("RAY"/*DNT*/,ic_rbassoc->resval.rstring) || strisame("XLINE"/*DNT*/,ic_rbassoc->resval.rstring)) {
					ic_assoc(objrb,10);
					sds_trans(ic_rbassoc->resval.rpoint,&rbwcs,&rbucs,0,startpt);
					ic_assoc(objrb,11);
					sds_trans(ic_rbassoc->resval.rpoint,&rbwcs,&rbucs,1,unitvec);

					if (off>=0.0) {
						if (internalGetpoint(NULL,ResourceString(IDC_CMDS3__NSIDE_FOR_PARALL_35, "\nSide for parallel infinite line: " ),pt1)!=RTNORM)
							goto bail;
						cmd_unit_vector(startpt,pt1,pt1);

						endpt[0]=startpt[0]+unitvec[0];
						endpt[1]=startpt[1]+unitvec[1];
						endpt[2]=startpt[2]+unitvec[2];

						if (((unitvec[0]*pt1[1])-(unitvec[1]*pt1[0]))<0.0) {  //cross to get the z
							cmd_offset_linepts(startpt,endpt,off,pt1,pt2);
						}else{
							cmd_offset_linepts(startpt,endpt,-off,pt1,pt2);
						}
					}else{
						if (internalGetpoint(NULL,ResourceString(IDC_CMDS3__NTHROUGH_THE_POI_36, "\nThrough the point: " ),pt1)!=RTNORM)
							goto bail;
					}

					sds_trans(pt1,&rbucs,&rbwcs,0,pt1);
					sds_trans(unitvec,&rbucs,&rbwcs,1,unitvec);
					if (mode==DB_XLINE) {
						elist=sds_buildlist(RTDXF0,"XLINE"/*DNT*/,10,pt1,11,unitvec,210,pt210,0);
					}else{
						elist=sds_buildlist(RTDXF0,"RAY"/*DNT*/,10,pt1,11,unitvec,210,pt210,0);
					}
					sds_entmake(elist);
					sds_relrb(elist);elist=NULL;

				}else if (strisame("POLYLINE"/*DNT*/,ic_rbassoc->resval.rstring)) {
					ic_assoc(objrb,70);
					flag70=ic_rbassoc->resval.rint;
					if (ic_assoc(objrb,210)==0){
						rbent.restype=RT3DPOINT;
						ic_ptcpy(rbent.resval.rpoint,ic_rbassoc->resval.rpoint);
					}else{
						rbent.restype=RTSHORT;
						rbent.resval.rint=0;
					}
					IC_RELRB(objrb); //release pline header

					vertex_no=cmd_pl_vtx_no(objname,tmppt,1,&exactvert);
					if ((ret=sds_entnext_noxref(objname,subobjname))!=RTNORM)
						goto bail;
					for(i=1;i<vertex_no;i++) sds_entnext_noxref(subobjname,subobjname);
					if ((objrb=sds_entget(subobjname))==NULL)
						goto bail;
					ic_assoc(objrb,42);
					if (icadRealEqual(ic_rbassoc->resval.rreal,0.0)) {
						ic_assoc(objrb,10);
						sds_trans(ic_rbassoc->resval.rpoint,&rbent,&rbucs,0,startpt);
						IC_RELRB(objrb);
						if (cmd_pline_entnext(subobjname,subobjname,flag70)==0){
							if ((objrb=sds_entget(subobjname))==NULL)
								goto bail;
							ic_assoc(objrb,10);
							sds_trans(ic_rbassoc->resval.rpoint,&rbent,&rbucs,0,endpt);
							cmd_unit_vector(startpt,endpt,unitvec);
							IC_RELRB(objrb);

							if (off>=0.0) {
								if (internalGetpoint(NULL,ResourceString(IDC_CMDS3__NSIDE_FOR_PARALL_35, "\nSide for parallel infinite line: " ),pt1)!=RTNORM)
									goto bail;
								cmd_unit_vector(startpt,pt1,pt1);

								if (((unitvec[0]*pt1[1])-(unitvec[1]*pt1[0]))<0.0) {
									cmd_offset_linepts(startpt,endpt,off,pt1,pt2);
								}else{
									cmd_offset_linepts(startpt,endpt,-off,pt1,pt2);
								}
							}else{
								if (internalGetpoint(NULL,ResourceString(IDC_CMDS3__NTHROUGH_THE_POI_36, "\nThrough the point: " ),pt1)!=RTNORM)
									goto bail;
							}

							sds_trans(pt1,&rbucs,&rbwcs,0,pt1);
							sds_trans(unitvec,&rbucs,&rbwcs,1,unitvec);
							if (mode==DB_XLINE) {
								elist=sds_buildlist(RTDXF0,"XLINE"/*DNT*/,10,pt1,11,unitvec,0);
							}else{
								elist=sds_buildlist(RTDXF0,"RAY"/*DNT*/,10,pt1,11,unitvec,0);
							}
							sds_entmake(elist);
							sds_relrb(elist);elist=NULL;

						}else{
							sds_printf(ResourceString(IDC_CMDS3__NUNABLE_TO_USE_S_37, "\nUnable to use selected parallel entity" ));
						}
					}else{
						sds_printf(ResourceString(IDC_CMDS3__NUNABLE_TO_USE_S_37, "\nUnable to use selected parallel entity" ));
					}
				}else{
					sds_printf(ResourceString(IDC_CMDS3__NUNABLE_TO_USE_S_37, "\nUnable to use selected parallel entity" ));
				}
			} //while
		}
	}
	rb_last.restype=RTREAL;
	rb_last.resval.rreal=off;
	sds_setvar("OFFSETDIST"/*DNT*/,&rb_last);
//	  rb_last.resval.rreal=ang;
//	SDS_setvar(NULL,DB_QLASTANGLE,&rb_last,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
bail:
	if (ortho_reset){
		rb_last.restype=RTSHORT;
		rb_last.resval.rint=orthomode;
		SDS_setvar(NULL,DB_QORTHOMODE,&rb_last,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,1);
	}
	IC_RELRB(objrb);
	if (elp) delete elp; elp=NULL;
	return(ret);
}


short cmd_validlt(char *ss) {
/*
**	Checks ss for a valid ACAD name (1-31 chars, alphanumeric and $-_#@.*?~[]-,`).
**	Returns: 1 = ok; 0 = bad
*/
	char *fcp1;
	short fi1;

	// EBATECH(FUTA)-[ 2001-01-07 MBCS
	//for (fcp1=ss,fi1=0; fi1<31 && *fcp1 &&
	//	  (sds_isalnum((unsigned char) *fcp1) || strchr("$-_#@.*?~[]-,`"/*DNT*/,*fcp1)); fcp1++,fi1++);
	short fi2;
	for (fcp1=ss,fi1=0,fi2=0;*fcp1 && fi2<IC_ACADNMLEN;fcp1++,fi1++,fi2++){
		if ((_MBC_LEAD ==_mbbtype((unsigned char)*fcp1,0)) &&
			(_MBC_TRAIL==_mbbtype((unsigned char)*(fcp1+1),1)) ){
			fcp1++;
			fi1++;
		} else {
			if (!(*fcp1 && (isalnum((unsigned char)*fcp1) || strchr(" !#$%&'()+-.@[]^_{|}~*?",*fcp1)))) break;
		}
	}
	// ]-EBATECH(FUTA) 2001-01-07
	return (fi1 && !*fcp1);
}

short cmd_ltypeloader(void) {
//*** This function will load a linetype out of a selected
//*** *.lin file. Linetype must be defined.

	char eltp[IC_ACADBUF],fs1[IC_ACADBUF],ltp[IC_ACADBUF];
	struct resbuf rb;
	RT ret;
	int i,j;

	//Get entered linetype to load
	for(;;){
		if ((ret=sds_getstring(1,ResourceString(IDC_CMDS3__NENTER_LINETYPE__39, "\nEnter linetype to load: " ),eltp))!=RTNORM) {
			return(ret);
		}
//		if (strchr(eltp,'*')) break;
		ic_trim(eltp,"be");
		if (!cmd_validlt(eltp))
			cmd_ErrorPrompt(CMD_ERR_LTYPENAME,0);
		else
			break;
	}

	//check to see if the user wanted a dialog box or a command line prompt
	*fs1=0;
	if ((ret=SDS_getvar(NULL,DB_QFILEDIA,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM) {
		return(ret);
	}
	if ( rb.resval.rint &&
		GetActiveCommandQueue()->IsEmpty() &&
		GetMenuQueue()->IsEmpty() &&
		!lsp_cmdhasmore) {
		dia: ;
		//EBATECH(CNBR)-[ 2002/9/20 Default linetype file is dependent on $MEASUREMENT.
		char* deflinfn;
		if ((ret=SDS_getvar(NULL,DB_QMEASUREMENT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM) {
			return(ret);
		}
		deflinfn = ( rb.resval.rint == DB_ENGLISH_MEASUREINIT ?	SDS_LTYPEFILE : SDS_ISOLTYPEFILE);
		if ((ret=sds_findfile(deflinfn,fs1))!=RTNORM)
		//if ((ret=sds_findfile("icad.lin"/*DNT*/,fs1))!=RTNORM)
		{
			*fs1=0;
			if (cmd_LinSavePath) strcpy(fs1,cmd_LinSavePath);
			strcat(fs1,deflinfn);
			//strcat(fs1,"icad.lin"/*DNT*/);
		}
		// EBATECH(CNBR)]-
		if ((ret=sds_getfiled(ResourceString(IDC_CMDS3_SELECT_LINETYPE_F_41, "Select Linetype File" ),fs1,ResourceString(IDC_CMDS3_LINETYPE_FILE_LIN_42, "Linetype File|lin" ),0,&rb))!=RTNORM) {
			rb.resval.rstring=NULL;
			return(ret);
		}
		if (rb.restype==RTSTR) {
			strcpy(fs1,rb.resval.rstring);
			IC_FREE(rb.resval.rstring);
		}
		else { goto typeit; } // EBATECH(CNBR) 2002/4/30 Enable TYPE button
	} else {
		typeit: ;
		if ((ret=sds_getstring(1,ResourceString(IDC_CMDS3__NLINETYPE_FILE_T_43, "\nLinetype file to load: " ),fs1))!=RTNORM) {
			return(ret);
		}
		if (*fs1=='~'/*DNT*/)
			goto dia;
	}
	if (strchr(fs1,'.'/*DNT*/)==NULL)ic_setext(fs1,".lin"/*DNT*/);
	ChangeSlashToBackslashAndRemovePipe(fs1);

	if (strrchr(fs1,'\\'/*DNT*/)) {
		db_astrncpy(&cmd_LinSavePath,fs1,strlen(fs1)+1);
		*(strrchr(cmd_LinSavePath,'\\'/*DNT*/)+1)=0;
	}

	//Load the linetype from the given file
	j=0;
	while(eltp[j]!='\0'/*DNT*/) {
		for(i=0;(eltp[j]!='\0'/*DNT*/)&&(eltp[j]!=','/*DNT*/);i++,j++) {
			ltp[i]=eltp[j];
		}
		ltp[i]='\0'/*DNT*/;
		if (eltp[j]==','/*DNT*/) j++;
		if ((ret=cmd_ltload(ltp,fs1,1))!=RTNORM) {
			return(ret);
		}
	}
	sds_printf("\n"/*DNT*/);

	ret=RTNORM;

	return(ret);
}

#define _COMPLEX_LINETYPE_
//============================================================================================
//This function will load a passed linetype from the passed ?.lin file.
short cmd_ltload(char* linetype, char* fname, short mode) {
	//mode==0: quiet mode
	//mode==1: verify for overwriting an existing linetype.

	char szLineName[IC_ACADBUF],rload[5],szPenUpDown[IC_ACADBUF],fs1[IC_ACADBUF];
	char fflin[IC_ACADBUF],szNameDesc[IC_ACADBUF],tmppen[IC_ACADBUF],szPrompt[IC_ACADBUF];
	char *cp1=NULL;
	struct resbuf *ltrb=NULL,*tmprb=NULL,*rbb=NULL,*rbc=NULL,*rbt=NULL;
	struct resbuf rb;
	int expert,gotit=0,got1=0,i,j,count;
	RT ret;
	double patlen=0.0;
	FILE *lfilestream=NULL;


	//check the ?.lin file
	//check if the file exists
	//then put it in fflin
	strupr(linetype);
	if (ret=sds_findfile(fname, fflin)!=RTNORM) {
		sds_printf(ResourceString(IDC_CMDS3__NFILE__S_NOT_FOU_45, "\nFile %s not found." ),fname);
		return(RTCAN);
	}

	//Check to make sure the file is readable
	if (_access(fflin,4)) {
		sds_printf(ResourceString(IDC_CMDS3__NFILE__S_DOES_NO_46, "\nFile %s does not have read permissions.\n" ),fflin);
		return(RTCAN);
	}

	//open the file in read mode
	if ( (lfilestream = fopen( fflin, "r"/*DNT*/)) == NULL ) {
		sds_printf( ResourceString(IDC_CMDS3__NUNABLE_TO_OPEN__48, "\nUnable to open file %s for reading." ),fflin);
		return(RTCAN);
	}

	for(;;){
		//read thru the lin file for the occurrence of the entered linetype
		while(ic_nfgets(fs1,IC_ACADBUF-1,lfilestream)) {
			if (sds_usrbrk()) {
				ret=RTCAN;
				goto bail;
			}  //check for break
			if (fs1[0]=='*'/*DNT*/) {
				//get the description name
				for(i=1,j=0;(fs1[i]!=','/*DNT*/)&&(fs1[i]!='\0'/*DNT*/);i++,j++){
					szLineName[j]=fs1[i];
				}
				szLineName[j]='\0'/*DNT*/;
				//get the line description
				for(i++,j=0;(fs1[i]!='\0'/*DNT*/);i++,j++){
					szNameDesc[j]=fs1[i];
				}
				szNameDesc[j]='\0'/*DNT*/;
				continue;
			}else if ((fs1[0]=='A'/*DNT*/)||(fs1[0]=='a'/*DNT*/)) {
				//get the pen up pen down description
				for(i=2,j=0;(fs1[i]!='\0'/*DNT*/);i++,j++){
					szPenUpDown[j]=fs1[i];
				}
				szPenUpDown[j]='\0'/*DNT*/;
			}else{
				continue;
			}
			//check if this is the linetype to load
			strupr(szLineName);
			if (sds_wcmatch(szLineName,linetype)==RTNORM) {
				got1=gotit=1;
				break;
			}
		}

		//If we got this far without gotit being set then no such linetype.
		if (gotit!=1) {
			if (got1==0) {
				sds_printf(ResourceString(IDC_CMDS3_NO_LINETYPE_MATCH_49, "No linetype matching \"%s\" found in %s." ),linetype,fflin);
				ret=RTERROR;
			}else if (got1==1) {
				ret=RTNORM;
			}else if (got1==2) {
				ret=RTERROR;
			}
			goto bail;
		}

		//check pattern to verify only 2 to 12 pen movements.
		strncpy(tmppen,szPenUpDown,sizeof(tmppen)-1);
		cp1=strtok((char *)tmppen,","/*DNT*/);
		if (atof(cp1)<0.0) {
			sds_printf(ResourceString(IDC_CMDS3__NFIRST_PEN_MOVEM_52, "\nFirst pen movement cannot be negative." ));
			ret=RTNORM;
			goto bail;
		}
#ifndef _COMPLEX_LINETYPE_
		for(i=0;cp1!=NULL;i++) {
			cp1=strtok(NULL,",");
		}
		if ((i<2)||(i>12)) {
			cmd_ErrorPrompt(CMD_ERR_DASHDOT,0);
			ret=RTNORM;
			goto bail;
		}
#endif
		//must have it by now so go ahead and load it
		//Load linetype by building the list and table making or table modifying

		//Search the dwg linetype table for a match to the entered linetype to load
		gotit=0;
		if (SDS_getvar(NULL,DB_QEXPERT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) {
			ret=RTERROR;
			goto bail;
		}
		expert=rb.resval.rint;

		if ((tmprb=sds_tblsearch("LTYPE"/*DNT*/,szLineName,0))!=NULL) {
			if (mode==1 && expert<3) {
				//if it is a match say it is already loaded.
				sprintf(szPrompt,ResourceString(IDC_CMDS3__NLINETYPE___54, "\nLinetype \"%s\" is already loaded. Reload it? <Y>: " ),szLineName);
				if ((ret=sds_initget(0, ResourceString(IDC_LTLOAD_LTYPE_INIT1, "Yes-Reload_linetype|Yes No-Do_not_reload|No ~_Yes ~_No")))!=RTNORM) {
					IC_RELRB(tmprb);
					goto bail;
				}
				if ((ret=sds_getkword(szPrompt,rload))==RTERROR) {
					IC_RELRB(tmprb);
					goto bail;
				} else if (ret==RTCAN) {
					IC_RELRB(tmprb);
					goto bail;
				} else if (strisame(rload,"NO"/*DNT*/)) {		//if the ltype has been found and
					IC_RELRB(tmprb); got1=2; continue;
				} else {										//the load. Else the user said YES
					sds_printf(ResourceString(IDC_CMDS3_LINETYPE__S_RELOA_58, "Linetype %s reloaded" ),szLineName);
				}
			}
			gotit=1;
			IC_RELRB(tmprb);
		}else{
			if (mode==1 && expert<3) {
				sds_printf(ResourceString(IDC_CMDS3__NLINETYPE__S_LOA_59, "\nLinetype %s loaded" ),szLineName);
			}
		}
		char* pStringPosition;
#ifdef _COMPLEX_LINETYPE_
		bool parsingText = false;
		bool parsingShape = false;
		bool secondEntryParsed = false;
		int stringLength;
		int transformationCode;
		sds_real transformationValue;
		long styleData[2];
		db_fontlink* pShapeFont = NULL;
		db_styletabrec* pShapeStyle = NULL;
		char shapeName[IC_ACADBUF];
		int shapeCode;
#endif
		// build list for linetype elements
		strncpy(tmppen, szPenUpDown, sizeof(tmppen)- 1);
		pStringPosition = strtok((char *)tmppen, ","/*DNT*/);
		count = 0;
		while(pStringPosition)
		{
			//check for break
			if(sds_usrbrk())
			{
				ret = RTCAN;
				IC_RELRB(rbb);
				goto bail;
			}
			// trim spaces from begin and end
			ic_trim(pStringPosition, "be"/*DNT*/);
#ifdef _COMPLEX_LINETYPE_
			if(parsingText || parsingShape)
			{
				// previous token was text or shape
				pStringPosition -= 2;
				if(*pStringPosition == ']')
				{
					// it's end of text or shape
					parsingText = false;
					parsingShape = false;
					secondEntryParsed = false;
					pStringPosition += 2;
					continue;
				}
				pStringPosition +=2;
				if(secondEntryParsed)
				{
					// text or shape have transform
					transformationValue = atof(pStringPosition + 2);
					switch(*pStringPosition)
					{
					case 'R':
					case 'r':
					case 'A':
					case 'a':
						{
							const double pi = 3.14159265358979323846;
							transformationValue *= (pi / 180);
							transformationCode = 50;
							break;
						}
					case 'S':
					case 's':
						{
							transformationCode = 46;
							break;
						}
					case 'X':
					case 'x':
						{
							transformationCode = 44;
							break;
						}
					case 'Y':
					case 'y':
						{
							transformationCode = 45;
							break;
						}
					default:
						IC_RELRB(rbb);
						ret = RTCAN;
						assert(false);
						goto bail;
					}
					rbt = sds_buildlist(transformationCode, transformationValue, 0);
				}
				else
				{
					// it's style of text or name of shapes file
					stringLength = strlen(pStringPosition);
					if(pStringPosition[stringLength] != ']')
						secondEntryParsed = true;
					else
						pStringPosition[stringLength - 1] = 0;
					styleData[1] = 0;
					if(parsingText)
					{
						styleData[0] = (long)SDS_CURDWG->findtabrec(DB_STYLETAB, pStringPosition,1);
						if(styleData[0] == 0)
						{
							cmd_ErrorPrompt(CMD_ERR_FINDSTYLE, 0);
							IC_RELRB(rbb);
							ret = RTCAN;
							assert(false);
							goto bail;
						}
						rbt = sds_buildlist(340, styleData, 0);
					}
					if(parsingShape)
					{
						// EBATECH(CNBR) -[ 2002/4/24 Avoid duplicate shape font link
						int  flag;
						bool foundStyletabrec = false;

						for( pShapeStyle = (db_styletabrec*)((long)SDS_CURDWG->tblnext(DB_STYLETABREC,1));
							pShapeStyle !=NULL ; pShapeStyle = (db_styletabrec*)((long)SDS_CURDWG->tblnext(DB_STYLETABREC,0)))
						{
							pShapeStyle->get_70(&flag);
							if(( flag & IC_SHAPEFILE_SHAPES ) && !( flag & IC_ENTRY_XREF_DEPENDENT ))
							{
								if( strisame( pStringPosition, pShapeStyle->retp_fontpn()))
								{
									foundStyletabrec = true;
									pShapeFont = pShapeStyle->ret_font();
									break;
								}
							}
						}
						if( foundStyletabrec == false )
						{
						// EBATECH(CNBR) ]-
							// try to load shape font
							if(db_setfont(pStringPosition, NULL, &pShapeFont, SDS_CURDWG) != 0)
							{
								cmd_ErrorPrompt(CMD_ERR_CANTFINDFILE, 0, pStringPosition);
								IC_RELRB(rbb);
								ret = RTCAN;
								assert(false);
								goto bail;
							}
							// check shape font
							if( strcmp( pShapeFont->desc,"Shape Definition"/*DNT*/))
							{
								cmd_ErrorPrompt(CMD_ERR_CANTFINDFILE, 0, pStringPosition);
								IC_RELRB(rbb);
								ret = RTCAN;
								assert(false);
								goto bail;
							}
							// try to create style table record for this shape
							pShapeStyle = new db_styletabrec();
							pShapeStyle->set_70(DB_SHAPESTYLE);
							if(SDS_CURDWG->addhanditem(pShapeStyle) != 0)
							{
								cmd_ErrorPrompt(CMD_ERR_MOREMEM, 0);
								IC_RELRB(rbb);
								ret = RTCAN;
								assert(false);
								goto bail;
							}
							pShapeStyle->set_3(pStringPosition, SDS_CURDWG);
							pShapeStyle->set_font(pShapeFont);
						}
						// EBATECH(CNBR) ]-
						// try to find shape code by name
						shapeCode = db_findCodeByName(pShapeFont, shapeName);
						if(shapeCode == 0)
						{
							cmd_ErrorPrompt(CMD_ERR_MOREMEM, 0);
							IC_RELRB(rbb);
							ret = RTCAN;
							assert(false);
							goto bail;
						}
						// form resbuf for shape
						rbt = sds_buildlist(75, shapeCode, 0);
						if(rbt == NULL)
						{
							cmd_ErrorPrompt(CMD_ERR_MOREMEM, 0);
							IC_RELRB(rbb);
							ret = RTCAN;
							goto bail;
						}
						if (rbb == NULL)
							rbb = rbc = rbt;
						else
							rbc = rbc->rbnext = rbt;
						rbt = sds_buildlist(74, 4, 0);
						if(rbt == NULL)
						{
							cmd_ErrorPrompt(CMD_ERR_MOREMEM, 0);
							IC_RELRB(rbb);
							ret = RTCAN;
							goto bail;
						}
						if (rbb == NULL)
							rbb = rbc = rbt;
						else
							rbc = rbc->rbnext = rbt;
						styleData[0] = (long)pShapeStyle;
						rbt = sds_buildlist(340, styleData, 0);
					}
					if(!secondEntryParsed)
					{
						parsingText = false;
						parsingShape = false;
					}
				}
			}
			else
			{
				// previous token was not text or shape
				if(*pStringPosition == '[')
				{
					// this token is text or shape
					++pStringPosition;
					if(*pStringPosition == '"')
					{
						// it's text, so build list element for string
						++pStringPosition;
						stringLength = strlen(pStringPosition);
						pStringPosition[stringLength - 1] = 0;
						parsingText = true;
						// text string
						rbt = sds_buildlist(9, pStringPosition, 0);
						if(rbt == NULL)
						{
							cmd_ErrorPrompt(CMD_ERR_MOREMEM, 0);
							IC_RELRB(rbb);
							ret = RTCAN;
							goto bail;
						}
						if (rbb == NULL)
							rbb = rbc = rbt;
						else
							rbc = rbc->rbnext = rbt;
						// text flags
						rbt = sds_buildlist(74, 2, 0);
						if(rbt == NULL)
						{
							cmd_ErrorPrompt(CMD_ERR_MOREMEM, 0);
							IC_RELRB(rbb);
							ret = RTCAN;
							goto bail;
						}
						if (rbb == NULL)
							rbb = rbc = rbt;
						else
							rbc = rbc->rbnext = rbt;
						rbt = sds_buildlist(75, 0, 0);
					}
					else
					{
						// it's shape
						parsingShape = true;
						strcpy(shapeName, pStringPosition);
						pStringPosition = strtok(NULL, ","/*DNT*/);
						continue;
					}
				}
				else
				{
					// this token is number
#endif
					rbt = sds_buildlist(49, atof(pStringPosition), 0);
#ifdef _COMPLEX_LINETYPE_
				}
			}
#endif
			if(rbt == NULL)
			{
				cmd_ErrorPrompt(CMD_ERR_MOREMEM, 0);
				ret = RTCAN;
				IC_RELRB(rbb);
				goto bail;
			}
			if(rbb == NULL)
				rbb = rbc = rbt;
			else
				rbc = rbc->rbnext = rbt;
			++count;
			if(rbc->restype == 49)
				patlen = patlen + fabs(rbc->resval.rreal);
			pStringPosition = strtok(NULL, ","/*DNT*/);
		}

		//Build the main body list
		if ((ltrb=sds_buildlist(RTDXF0,"LTYPE"/*DNT*/,
								2,szLineName,
								70,0,
								3,szNameDesc,
								72,'A'/*DNT*/,
								73,count,
								40,patlen,
								0))==NULL) {
			cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
			ret=RTCAN;
			IC_RELRB(rbb);
			goto bail;
		}

		//append the 49 list to the main list
		for(rbt=ltrb;rbt->rbnext!=NULL;rbt=rbt->rbnext) {
			if (sds_usrbrk()) {
				ret=RTCAN;
				IC_RELRB(rbb);
				IC_RELRB(ltrb);
				goto bail;
			}  //check for break
		}
		rbt->rbnext=rbb;
		if (gotit==1){
			if (SDS_tblmesser(ltrb,IC_TBLMESSER_MODIFY,SDS_CURDWG)!=RTNORM){
				ret=RTCAN;
				IC_RELRB(ltrb);
				goto bail;
			}  //modify
			gotit=0;
		}else{
			if (SDS_tblmesser(ltrb,IC_TBLMESSER_MAKE,SDS_CURDWG)!=RTNORM){
				ret=RTCAN;
				IC_RELRB(ltrb);
				goto bail;
			}	//make
		}
		IC_RELRB(ltrb); rbb=rbt=rbc=NULL;
		ret=RTNORM;
	}

bail:
	if (lfilestream!=NULL) {
		fclose( lfilestream );
		lfilestream=NULL;
	}
	return(ret);
}

//============================================================================================
//This function will set the current linetype.

short cmd_ltypeset(void) {
	char eltp[IC_ACADBUF],fs1[IC_ACADBUF];
	struct resbuf rb,rb2, *tmprb=NULL;
	struct sds_resbuf *fdtrb=NULL;
	RT ret=0;
	int ch=0,count=0;
	short nodsc=0,n=0,i=0;
	sds_real patlen=0.0;

	//get current linetype
	if (ret=SDS_getvar(NULL,DB_QCELTYPE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) {
		return(ret);
	}
	strupr(rb.resval.rstring);

	//get new linetype or ? to list loaded linetypes
	sprintf(fs1,ResourceString(IDC_CMDS3__N__TO_LIST_LINET_61, "\n? to list linetypes in drawing/Linetype to set as current <%s>: " ),rb.resval.rstring);
	if ((ret=sds_getstring(1,fs1,eltp))!=RTNORM){
		goto bail;
	}
	ic_trim(eltp,"be");
	strupr(eltp);

	//check for "?" if so then list loaded linetypes and return
	if (strisame(eltp,"?"/*DNT*/) || strisame(eltp,"_?"/*DNT*/)) {
		ret=cmd_ltypelister();
		goto bail;
	}

	//if return or current linetype entered just bail
	if (strsame(eltp,rb.resval.rstring) || (*eltp==0)) {
		ret=1;
		goto bail;
	}

	//if it is byblock or bylayer then just set the icadvar.
	if ((strsame(eltp,"BYLAYER"/*DNT*/))||(strsame(eltp,"BYBLOCK"/*DNT*/))) {
		rb.resval.rstring=ic_realloc_char(rb.resval.rstring,8);
		rb.restype=RTSTR;
		strcpy(rb.resval.rstring,eltp);
		if ((ret=sds_setvar("CELTYPE"/*DNT*/,&rb))!=RTNORM) {
			goto bail;
		}
	}

	//if it is not one of the above then it must be a linetype
	//first check if the linetype is loaded. Then check the "ICAD.LIN"
	//file.
	//here we check the linetype table in the current dwg.
	if ((tmprb=sds_tblsearch("LTYPE"/*DNT*/,eltp,0))!=NULL) {
		IC_RELRB(tmprb);   //already loaded
	}else{ //check the icad.lin file
		//EBATECH(CNBR)-[ 2002/9/20 Default linetype file is dependent on $MEASUREMENT.
		SDS_getvar(NULL,DB_QMEASUREMENT,&rb2,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		if( cmd_ltload( eltp, ( rb2.resval.rint == DB_ENGLISH_MEASUREINIT ?
			SDS_LTYPEFILE : SDS_ISOLTYPEFILE), 0) != RTNORM )
		//if ((ret = cmd_ltload(eltp, "ICAD.LIN"/*DNT*/, 0)) != RTNORM &&
		//	(ret = cmd_ltload(eltp, "ICADISO.LIN"/*DNT*/, 0)) != RTNORM)	/*D.G.*/// icadiso added.
		//EBATECH(CNBR)]-
		{
			ret=1;
			goto bail;
		}
	}

	//set the icadvar to the current linetype.
	rb.resval.rstring=ic_realloc_char(rb.resval.rstring,strlen(eltp)+1);
	strcpy(rb.resval.rstring,eltp);
	rb.restype=RTSTR;
	if ((ret=sds_setvar("CELTYPE"/*DNT*/,&rb))!=RTNORM) {
		goto bail;
	}

	bail:

	IC_FREE(rb.resval.rstring);
	return(ret);
}


//==================================================================================
short cmd_ltypecreate(void) {
//*** This function will create a linetype out in a selected
//*** *.lin file.

	char eltp[IC_ACADBUF],szLineName[IC_ACADBUF],szNameDesc[IC_ACADBUF],szPenUpDown[IC_ACADBUF],rload[5];
	char fr[IC_ACADBUF],lin1[IC_ACADBUF],lin2[IC_ACADBUF],tmplin[IC_ACADBUF];
	char *cp1=NULL,*name1=NULL,inlin[IC_ACADBUF],tmpinlin[IC_ACADBUF],tltp[IC_ACADBUF],fs1[IC_ACADBUF];
	struct resbuf rb;
	RT ret,exists,loop;
	int ch,i,j,n;
	int expert;				// Suppresses certain prompts depending on value of "EXPERT"
	FILE *lfilestream=NULL,*newfilestream=NULL;

	if (SDS_getvar(NULL,DB_QEXPERT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) {
		ret=RTERROR;
		goto bail;
	}
	expert=rb.resval.rint;

	loop=1;
	while(loop){
		//Get entered linetype to create
		while(1) {
			if ((ret=sds_getstring(1,ResourceString(IDC_CMDS3__NNAME_FOR_NEW_LI_67, "\nName for new linetype: " ),eltp))==RTERROR) {
				goto bail;
			} else if (ret==RTCAN) {
				goto bail;
			} else if (ic_trim(eltp,"be"), strsame(eltp,""/*DNT*/)) {
				goto bail;
			} else if (!ic_isvalidname(eltp)){
				cmd_ErrorPrompt(CMD_ERR_LTYPENAME,0);
			}else{
				break;
			}
		}

		//Get linetype to create from the *.lin file
		*fr=0;
		if ((ret=SDS_getvar(NULL,DB_QFILEDIA,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM) {
			return(ret);
		}
		if ( rb.resval.rint &&
			GetActiveCommandQueue()->IsEmpty() &&
			GetMenuQueue()->IsEmpty() &&
			!lsp_cmdhasmore) {
			dia: ;
			//EBATECH(CNBR)-[ 2002/9/20 Default linetype file is dependent on $MEASUREMENT.
			char* deflinfn;
			if ((ret=SDS_getvar(NULL,DB_QMEASUREMENT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM) {
				return(ret);
			}
			deflinfn = ( rb.resval.rint == DB_ENGLISH_MEASUREINIT ?	SDS_LTYPEFILE : SDS_ISOLTYPEFILE);
			if ((ret=sds_findfile(deflinfn,fr))!=RTNORM)
			//if ((ret=sds_findfile("icad.lin"/*DNT*/,fr))!=RTNORM)
			{
				*fr=0;
				if (cmd_LinSavePath) strcpy(fr,cmd_LinSavePath);
				strcat(fr,deflinfn);
				//strcat(fr,"icad.lin"/*DNT*/);
			}
			// EBATECH(CNBR)]-
			if ((ret=sds_getfiled(ResourceString(IDC_CMDS3_CREATE_OR_APPEND__69, "Create or Append Linetype File" ),fr,ResourceString(IDC_CMDS3_LINETYPE_FILE_LIN_42, "Linetype File|lin" ),1+32,&rb))!=RTNORM) {
				rb.resval.rstring=NULL;
				return(-1L);
			}
			if (rb.restype==RTSTR) {
				strcpy(fs1,rb.resval.rstring);
				IC_FREE(rb.resval.rstring);
			}
			else { goto typeit; } // EBATECH(CNBR) 2002/4/30 Enable TYPE button
		} else {
			typeit: ;
			if ((ret=sds_getstring(1,ResourceString(IDC_CMDS3__NLINETYPE_FILE_T_70, "\nLinetype file to Create or Append: " ),fr))!=RTNORM) {
				return(ret);
			}
			if (*fr=='~')
				goto dia;
			//check if the file exists
			if ((ret=sds_findfile(fr, fs1))!=RTNORM) {
				strcpy(fs1, fr); // EBATECH(CNBR) 2002/4/30 *BUG* fs1 is not initialized!
				sds_printf(ResourceString(IDC_CMDS3__NCREATING_NEW__S_71, "\nCreating new %s file" ),fs1);
			}
		}

		if (strchr(fs1,'.'/*DNT*/)==NULL)ic_setext(fs1,".lin"/*DNT*/);
		ChangeSlashToBackslashAndRemovePipe(fs1);

		if (strrchr(fs1,'\\'/*DNT*/)) {
			db_astrncpy(&cmd_LinSavePath,fs1,strlen(fs1)+1);
			*(strrchr(cmd_LinSavePath,'\\'/*DNT*/)+1)=0;
		}

		//Check to make sure the file has read and write permission
		if (!cmd_access(fs1)) {
			if (errno==EACCES) {
				sds_printf(ResourceString(IDC_CMDS3__NFILE___72, "\nFile \"%s\" does not have read/write permissions.\n" ),fs1);
				return(RTERROR);
			}
		}

		//open in read/append mode and create if not existing
		if ((lfilestream = fopen( fs1, "a+"/*DNT*/ )) == NULL ) {
			sds_printf( ResourceString(IDC_CMDS3__NUNABLE_TO_OPEN__75, "\nUnable to open \"%s\" to append linetype.\n" ),fs1);
			return(RTERROR);
		}

		//read thru the lin file for the occurrence of the entered linetype
		sds_printf(ResourceString(IDC_CMDS3__NONE_MOMENT____C_77, "\nOne moment... Checking existing linetypes for \"%s\"." ),eltp);

		exists=0;
		while(ic_nfgets(tmpinlin,IC_ACADBUF-1,lfilestream)) {
			if (sds_usrbrk()) {
				ret=RTCAN;
				goto bail;
			}  //check for break
			if (tmpinlin[0]=='*'/*DNT*/) {
				//get the description name
				for(i=1,j=0;(tmpinlin[i]!=','/*DNT*/)&&(tmpinlin[i]!='\0'/*DNT*/);i++,j++){
					szLineName[j]=tmpinlin[i];
				}
				szLineName[j]='\0'/*DNT*/;
				//get the line description
				for(i++,j=0;(tmpinlin[i]!='\0'/*DNT*/);i++,j++){
					szNameDesc[j]=tmpinlin[i];
				}
				szNameDesc[j]='\0'/*DNT*/;
				continue;
			}else if ((tmpinlin[0]=='A'/*DNT*/)||(tmpinlin[0]=='a'/*DNT*/)) {
				//get the pen up pen down description
				for(i=2,j=0;(tmpinlin[i]!='\0'/*DNT*/);i++,j++){
					szPenUpDown[j]=tmpinlin[i];
				}
				szPenUpDown[j]='\0'/*DNT*/;
			}else{
				continue;
			}
			//check if this is the linetype to load
			if (strisame(eltp,szLineName)) {
				exists=1;
				break;
			}
		}

		//if the linetype already exists display the linetype
		//and ask if they would like to overwrite it.
		if (exists==1 && expert<3) {
			sds_textscr();
			sds_printf(ResourceString(IDC_CMDS3__N_S_ALREADY_EXIS_79, "\n%s already exists. Current definition is: " ),szLineName);
			sds_printf("\n%.47s"/*DNT*/,szLineName);
			sds_printf("\n%.60s"/*DNT*/,szNameDesc);
			sds_printf("\n%.60s"/*DNT*/,szPenUpDown);

			if ((ret=sds_initget(0, ResourceString(IDC_LTYPECREATE_INIT1, "Yes-Overwrite_definition|Yes No-Do_not_overwrite|No ~_Yes ~_No")))!=RTNORM) {
				goto bail;
			}
			if ((ret=sds_getkword(ResourceString(IDC_CMDS3__NOVERWRITE___N___83, "\nOverwrite? <N>: " ),rload))==RTERROR) {
				goto bail;
			} else if (ret==RTCAN) {
				goto bail;
			} else if ((ret==RTNONE)||(strisame(rload,"NO"/*DNT*/))) { //if the ltype has been found and
				loop=1;
			} else if (strisame(rload,"YES"/*DNT*/)){
				loop=0;
			}
		}else{
			loop=0;
		}

		//flush the stream
		fflush( lfilestream );
		//rewind the open file to the begining before copying file
		rewind( lfilestream );
	}

	//if overwrite or not exists
	//Get descriptive text
	if ((ret=sds_getstring(1,ResourceString(IDC_CMDS3__NLINETYPE_DESCRI_85, "\nLinetype description: " ),szNameDesc))==RTERROR) {
		goto bail;
	} else if (ret==RTCAN) {
		goto bail;
	}

	//Get pattern
	loop=1;
	while(loop) {
		if ((ret=sds_getstring(1,ResourceString(IDC_CMDS3__NLINETYPE_DEFINI_86, "\nLinetype definition (positive numbers for lines, negative for spaces):\n  A," ),szPenUpDown))==RTERROR) {
			goto bail;
		} else if (ret==RTCAN) {
			goto bail;
		} else if (strsame(szPenUpDown,""/*DNT*/)) {
			cmd_ErrorPrompt(CMD_ERR_LTYPEPAT,0);
			continue;
		}

		//check pattern to verify only 2 to 12 pen movements.
		strncpy(tmplin,szPenUpDown,sizeof(tmplin)-1);
		cp1=strtok((char *)tmplin,","/*DNT*/);
		if (atof(cp1)<0.0) {
			sds_printf(ResourceString(IDC_CMDS3__NFIRST_PEN_MOVEM_52, "\nFirst pen movement cannot be negative." ));
			ret=RTERROR;
			goto bail;
		}
		for(i=0;cp1!=NULL;i++) {
			cp1=strtok(NULL,","/*DNT*/);
		}
		if ((i<2)||(i>12)) {
			cmd_ErrorPrompt(CMD_ERR_DASHDOT,0);
			continue;
		}

		//check for proper contence of the linepat
		n=i=0;
		while(n==0) {
			if (sds_usrbrk()) {
				ret=RTCAN;
				goto bail;
			}  //check for break
			ch=szPenUpDown[i];
			switch(ch){
				case ',':
				case '.':
				case '-':
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
					i++;
					break;
				case '\0':
					n=1;
					loop=0;
					break;
				default:
					cmd_ErrorPrompt(CMD_ERR_LTYPEPAT,0);
					n=1;
					loop=1;
					break;
			}
		}
		if (loop==1) {
			sds_printf(ResourceString(IDC_CMDS3__NTHE_LINETYPE_PA_87, "\nThe linetype pattern contains invalid character(s)." ));
			continue; //let the user fix it
		}
	}
	//assemble the strings of the new linetype to write to the file
	strcpy(lin1,"*"/*DNT*/);
	strcat(lin1,eltp);
	strcat(lin1,","/*DNT*/);
	strcat(lin1,szNameDesc);
	strcpy(lin2,ResourceString(IDC_CMDS3_A__89, "A,"/*DNT*/ ));
	strcat(lin2,szPenUpDown);
	//flush the stream
	fflush( lfilestream );
	//write out the new linetype
	//if the linetype does not exist append it to the current *.lin file
	if (exists==0) {
		if (fputs( "\n"/*DNT*/, lfilestream )==EOF) {
			ret=RTERROR;
			goto bail;
		}
		if (fputs( lin1, lfilestream )==EOF) {
			ret=RTERROR;
			goto bail;
		}
		if (fputs( "\n"/*DNT*/, lfilestream )==EOF) {
			ret=RTERROR;
			goto bail;
		}
		if (fputs( lin2, lfilestream )==EOF) {
			ret=RTERROR;
			goto bail;
		}
		sds_printf(ResourceString(IDC_CMDS3__WAS_DEFINED_IN___90, "\nLinetype \"%s\" was defined in %s." ),eltp,fs1);
	}else if (exists==1) {
		//if the linetype does exist create a temp name file
		//open in read/append mode
		if ((name1=tmpnam( NULL ))==NULL){
			ret=RTERROR;
			goto bail;
		}
		if ((newfilestream=fopen(name1, "a+"/*DNT*/))==NULL){
			sds_printf( ResourceString(IDC_CMDS3__NTHE_TEMPORARY_F_91, "\nThe temporary file could not be opened\n" ));
			ret=RTERROR;
			goto bail;
		}
		//the next 3 lines append a "*" to the front of the linetype name
		strcpy(tltp,"*"/*DNT*/);
		strcat(tltp,eltp);

		//this will write out linetype from lin file to the temp file
		while(ic_nfgets(inlin,sizeof(inlin)-1,lfilestream)) {
			if (sds_usrbrk()) {
				ret=RTCAN;
				goto bail;
			}  //check for break
			strcpy(tmpinlin,inlin);

			//after reading a line from the lin file
			//take it apart and look for the name of the linetype to replace
			if (((cp1=strtok((char *)tmpinlin,","/*DNT*/))!=NULL) && (strisame(cp1,tltp))){
				//redefine lin
				if (fputs( lin1, newfilestream )<0) {
					ret=RTERROR;
					goto bail;
				}
				if (fputs( "\n"/*DNT*/, newfilestream )<0) {
					ret=RTERROR;
					goto bail;
				}
				if (fputs( lin2, newfilestream )<0) {
					ret=RTERROR;
					goto bail;
				}
				if (fputs( "\n"/*DNT*/, newfilestream )<0) {
					ret=RTERROR;
					goto bail;
				}
				if (ic_nfgets(inlin,sizeof(inlin)-1,lfilestream)==NULL) {break;} //purge extra input line
			}else{	//if there is no linetype with the same name then copy from current line file to temp
				if (fputs(inlin,newfilestream )==EOF) {
					ret=RTERROR;
					goto bail;
				}
				if (fputs("\n"/*DNT*/,newfilestream )==EOF) {
					ret=RTERROR;
					goto bail;
				}
			}
		}
		//close temp line file
		if (newfilestream!=NULL) {
			fclose( newfilestream );
			newfilestream=NULL;
		}
		//close old line file
		if (lfilestream!=NULL) {
			fclose( lfilestream );
			lfilestream=NULL;
		}
		//delete old line file
		unlink(fs1);
		//rename temp line file to old line file
		if (ret=rename(name1, fs1)!=0) {
			ret=RTERROR;
			sds_printf(ResourceString(IDC_CMDS3_COULD_NOT_RENAME__92, "Could not rename '%s'\n" ), name1);
			goto bail;
		}
		sds_printf(ResourceString(IDC_CMDS3__NNEW_LINETYPE_DE_93, "\nNew linetype definition \"%s\" written to %s." ),eltp,fs1);
		return(RTNORM);
	}
bail:
	if (lfilestream!=NULL) {
		fclose( lfilestream );
		lfilestream=NULL;
	}
	if (newfilestream!=NULL) {
		fclose( newfilestream );
		newfilestream=NULL;
	}
	return(RTNORM);
}

//=======================================================================================

short cmd_ltypefilelister(void) {
//*** This function lists defined linetype information.
//*** from a *.lin file.
//*** Do not even think about listing a file type other than a *.lin

	char fs1[IC_ACADBUF],fr[IC_ACADBUF],szLineName[IC_ACADBUF],szPenUpDown[IC_ACADBUF],szNameDesc[IC_ACADBUF];
	char scrollines[IC_ACADBUF];
	struct resbuf rb;
	int i,j,scroll=0,numlines=0;
	RT ret;
	FILE *lfilestream=NULL;

	*fr=0;
	if (SDS_getvar(NULL,DB_QFILEDIA,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) {
		return(RTERROR);
	}
	if ( rb.resval.rint &&
		GetActiveCommandQueue()->IsEmpty() &&
		GetMenuQueue()->IsEmpty() &&
		!lsp_cmdhasmore) {
		dia: ;
		//EBATECH(CNBR)-[ 2002/9/20 Default linetype file is dependent on $MEASUREMENT.
		char* deflinfn;
		if ((ret=SDS_getvar(NULL,DB_QMEASUREMENT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM) {
			return(ret);
		}
		deflinfn = ( rb.resval.rint == DB_ENGLISH_MEASUREINIT ?	SDS_LTYPEFILE : SDS_ISOLTYPEFILE);
		if ((ret=sds_findfile(deflinfn,fr))!=RTNORM)
		//if ((ret=sds_findfile("icad.lin"/*DNT*/,fr))!=RTNORM)
		{
			*fr=0;
			if (cmd_LinSavePath) strcpy(fr,cmd_LinSavePath);
			strcat(fr,deflinfn);
			//strcat(fr,"icad.lin"/*DNT*/);
		}
		// EBATECH(CNBR)]-
		if ((ret=sds_getfiled(ResourceString(IDC_CMDS3_SELECT_LINETYPE_F_41, "Select Linetype File" ),fr,ResourceString(IDC_CMDS3_LINETYPE_FILE_LIN_42, "Linetype File|lin" ),0,&rb))!=RTNORM) {
			rb.resval.rstring=NULL;
			return(ret);
		}
		if (rb.restype==RTSTR) {
			strcpy(fs1,rb.resval.rstring);
			IC_FREE(rb.resval.rstring);
		}
		else { goto typeit; } // EBATECH(CNBR) 2002/4/30 Enable TYPE button
	} else {
		typeit: ;
		do{
			if ((ret=sds_getstring(1,ResourceString(IDC_CMDS3__NLINETYPE_FILE_T_95, "\nLinetype file to list: " ),fr))!=RTNORM) {
				return(ret);
			}
			if (*fr=='~'/*DNT*/)
				goto dia;
			//check if the file exists
			if ((ret=sds_findfile(fr, fs1))!=RTNORM) {
				sds_printf(ResourceString(IDC_CMDS3__N_S_COULD_NOT_BE_96, "\n%s could not be found\n" ),fr);
			}else{
				break;
			}
		}while(1);
	}
	if (strchr(fs1,'.'/*DNT*/)==NULL)ic_setext(fs1,".lin"/*DNT*/);

	ChangeSlashToBackslashAndRemovePipe(fs1);

	if (strrchr(fs1,'\\'/*DNT*/)) {
		db_astrncpy(&cmd_LinSavePath,fs1,strlen(fs1)+1);
		*(strrchr(cmd_LinSavePath,'\\'/*DNT*/)+1)=0;
	}

	//Check to make sure the file is readable
	if (_access(fs1,4)) {
		sds_printf(ResourceString(IDC_CMDS3__NFILE__S_DOES_NO_46, "\nFile %s does not have read permissions.\n" ),fs1);
		return(RTERROR);
	}

	if ((lfilestream=fopen(fs1,"r"/*DNT*/))==NULL){
		sds_printf( ResourceString(IDC_CMDS3__NTHE_FILE__S_WAS_97, "\nThe file %s was not opened.\n" ),fs1);
		return(RTERROR);
	}

	sds_textscr();
	//read past the header and comments to the linetypes
	while(ic_nfgets(fs1,IC_ACADBUF-1,lfilestream)) {
		if (sds_usrbrk()) {
			ret=RTCAN;
			goto bail;
		}  //check for break
		if (fs1[0]=='*'/*DNT*/) {
			//get the description name
			for(i=1,j=0;(fs1[i]!=',')&&(fs1[i]!='\0'/*DNT*/);i++,j++){
				szLineName[j]=fs1[i];
			}
			szLineName[j]='\0'/*DNT*/;
			//get the line description
			for(i++,j=0;(fs1[i]!='\0'/*DNT*/);i++,j++){
				szNameDesc[j]=fs1[i];
			}
			szNameDesc[j]='\0'/*DNT*/;
			continue;
		}else if ((fs1[0]=='A'/*DNT*/)||(fs1[0]=='a'/*DNT*/)) {
			//get the pen up pen down description
			for(i=2,j=0;(fs1[i]!='\0'/*DNT*/);i++,j++){
				szPenUpDown[j]=fs1[i];
			}
			szPenUpDown[j]='\0'/*DNT*/;
		}else{
			continue;
		}
		sds_printf("\n%.47s"/*DNT*/,szLineName);
		sds_printf("\n%.60s"/*DNT*/,szNameDesc);
		sds_printf("\n%.60s"/*DNT*/,szPenUpDown);
		sds_printf("\n"/*DNT*/);
		numlines+=4;
		if (numlines>=20 && !scroll) {
			sds_initget(0, ResourceString(IDC_LTYPEFILE_INIT1, "Scroll ~_Scroll"));
			if ((ret=sds_getkword(ResourceString(IDC_CMDS3__NSCROLL____ENTE_101, "\nScroll/<ENTER to view next screen>:" ),scrollines))==RTERROR) {
				goto bail;
			}else if (ret==RTCAN) {
				goto bail;
			}else if (ret==RTNONE) {
				numlines=0;
			}else if (ret==RTNORM) {
				scroll=1;
			}
		}
	}


	ret=RTNORM;
bail:
	if (lfilestream!=NULL) {
		fclose( lfilestream );
		lfilestream=NULL;
	}
	return(ret);
}

//=end ltypefilelister=========================================================================
//=============================================================================================

short cmd_ltypelister(void) {
//*** This function lists defined linetype information.
//***
//*** NOTE: This function does not list the linetypes in alphabetical order
//***		like AutoCAD does. ~|
//***
	char fs1[IC_ACADBUF],linestr[IC_ACADBUF],scrollines[IC_ACADBUF];
	struct resbuf *tmprb=NULL,*trb=NULL;
	RT ret;
	int dashitems=0,scroll=0,numlines=0;

	if ((ret=sds_getstring(0,ResourceString(IDC_CMDS3__NLINETYPE_S__TO_102, "\nLinetype(s) to list <* for all>: " ),linestr))!=RTNORM) {
		return(ret);
	}
	strupr(linestr);
	if (strsame(linestr,""/*DNT*/)) {
		strcpy(linestr,"*"/*DNT*/);
	}

	sds_textscr();
	if ((tmprb=sds_tblnext("LTYPE"/*DNT*/,1))==NULL) {
		return(RTERROR);
	}
	while(tmprb!=NULL) {
		if (sds_usrbrk()) {
			IC_RELRB(tmprb);
			return(RTCAN);
		}
		//*** LTYPE names.
		if (ic_assoc(tmprb,2)==0) {
			if (sds_wcmatch(ic_rbassoc->resval.rstring,linestr)==RTNORM) {
				strncpy(fs1,ic_rbassoc->resval.rstring,sizeof(fs1)-1);
				//*** Get properties of the line.
				if (ic_assoc(tmprb,3)!=0) {
					IC_RELRB(tmprb);
					return(RTCAN);
				}
				sds_printf(ResourceString(IDC_CMDS3__NLINETYPE_NAME__103, "\nLinetype name: %-*.*s"), IC_ACADNMLEN - 1, IC_ACADNMLEN - 1, fs1);	/*D.G.*/
				sds_printf(ResourceString(IDC_CMDS3__NDESCRIPTION____104, "\nDescription: %-50.50s\n" ),ic_rbassoc->resval.rstring);
			 	for(trb=tmprb; trb!=NULL; trb=trb->rbnext) {
					if (trb->restype==49)
						sds_printf("%f "/*DNT*/,trb->resval.rreal);
				}
				sds_printf("\n"/*DNT*/);
				numlines+=4;
				if (numlines>=20 && !scroll) {
					sds_initget(0, ResourceString(IDC_LTYPEFILE_INIT1, "Scroll ~_Scroll"));
					if ((ret=sds_getkword(ResourceString(IDC_CMDS3__NSCROLL____ENTE_101, "\nScroll/<ENTER to view next screen>:" ),scrollines))==RTERROR) {
						IC_RELRB(tmprb);
						return(ret);
					}else if (ret==RTCAN) {
						IC_RELRB(tmprb);
						return(ret);
					}else if (ret==RTNONE) {
						numlines=0;
					}else if (ret==RTNORM) {
						scroll=1;
					}
				}
			}
		}
		IC_RELRB(tmprb);
		tmprb=sds_tblnext("LTYPE"/*DNT*/,0);
	}

	return(RTNORM);
}

//=================================================================================================

short cmd_linetype(void) {

	if (cmd_iswindowopen() == RTERROR)
		return RTERROR;

	char fs1[IC_ACADBUF];
	RT ret;

 	while(1) {
		if (sds_usrbrk()) {
			return(RTCAN);
		} //check for break

		if ((ret=sds_initget(0, ResourceString(IDC_LINETYPE_INIT1, "List_linetypes|? ~ Create Load Set ~_? ~_ ~_Create ~_Load ~_Set")))!=RTNORM)
			return(ret);

		if ((ret=sds_getkword(ResourceString(IDC_CMDS3__NLINETYPE_____T_107, "\nLinetype:  ? to list/Create/Load/Set: " ),fs1))==RTERROR) {
			return(ret);
		} else if (ret==RTCAN) {
			return(ret);
		} else if (ret==RTNONE) {
			return(ret);
		}

	  if (strisame("?"/*DNT*/,fs1)) {
			ret=cmd_ltypefilelister();
		}else if (strisame("CREATE"/*DNT*/,fs1)) {
			ret=cmd_ltypecreate();
	  }else if (strisame("LOAD"/*DNT*/,fs1)) {
			ret=cmd_ltypeloader();
	  }else if (strisame("SET"/*DNT*/,fs1)) {
			ret=cmd_ltypeset();
		}
	}
}

//==================================================================================

short cmd_vpoint(void){

	if (cmd_iswindowopen() == RTERROR)
		return RTERROR;
	if (cmd_isinpspace() == RTERROR)
		return RTERROR;


	char fs1[IC_ACADBUF],fs2[25],olducsname[IC_ACADNMLEN],cst[3][30];
	RT ret=RTNORM;
	int	fi1,viewmode,oldviewmode,osmode;
	sds_real fr1,fr2,fr3,viewdirlen,pt[3],newvtwist,oldvtwist;
	struct resbuf rb,rbucs,rbwcs, rbWorldUCS;	  //,rb1,from,to;
	sds_point viewdir,lastpt,olducsxdir,olducsydir,olducsorg;	//zdir,p1,
	short zneg=0,ucschg,exit_from_dialog;
	sds_point pt1,pt2,pt3,pt4,pt5,savevdir;
	struct gra_view *view=SDS_CURGRVW;
	struct gr_view newview, *pnewview;

	rbucs.restype=rbwcs.restype=RTSHORT;
	rbucs.resval.rint=1;
	rbwcs.resval.rint=0;

	// keep WORLDUCS so it can be changed later if necessary
	if (ret=SDS_getvar(NULL,DB_QWORLDUCS,&rbWorldUCS,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
		goto exit;

	//need ucsxdir & ucsydir even if we're not changing to world view
	if (ret=SDS_getvar(NULL,DB_QUCSXDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
		goto exit;
	ic_ptcpy(olducsxdir,rb.resval.rpoint);
	if (ret=SDS_getvar(NULL,DB_QUCSYDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
		goto exit;
	ic_ptcpy(olducsydir,rb.resval.rpoint);
	SDS_getvar(NULL,DB_QVIEWMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	viewmode=oldviewmode=rb.resval.rint;
	SDS_getvar(NULL,DB_QVIEWTWIST,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	oldvtwist=newvtwist=rb.resval.rreal;

	//	NOTE: we need to switch to WCS if worldview==1
	ucschg=exit_from_dialog=0;
	if (SDS_getvar(NULL,DB_QWORLDVIEW,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
		return(RTERROR);
	if (0==(viewmode&1) && rb.resval.rint){
		SDS_getvar(NULL,DB_QCMDECHO,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		fi1=rb.resval.rint;
		if (ret=SDS_getvar(NULL,DB_QUCSORG,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
			goto exit;
		ic_ptcpy(olducsorg,rb.resval.rpoint);
		if (ret=SDS_getvar(NULL,DB_QUCSNAME,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
			goto exit;
		strncpy(olducsname,rb.resval.rstring,sizeof(olducsname)-1);
		IC_FREE(rb.resval.rstring);
		pt1[0]=pt1[1]=pt1[2]=0.0;
		if (!icadRealEqual(olducsxdir[0],1.0) || !icadRealEqual(olducsydir[1],1.0) || !icadPointEqual(olducsorg,pt1)){
			ucschg=1;
			if (CMD_CMDECHO_PRINTF(fi1))sds_printf(ResourceString(IDC_CMDS3__NSWITCHING_TO_W_111, "\nSwitching to WCS." ));
			SDS_DrawUcsIcon(NULL,NULL,NULL,1);//paint out current icon
			cmd_ucsworld();
			SDS_InitThisView(SDS_CURVIEW);
			SDS_DrawUcsIcon(NULL,NULL,NULL,0);//paint in wcs icon

		}
	}

	if (SDS_getvar(NULL,DB_QVIEWDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) {
		return(RTERROR);
	}
	fr1=rb.resval.rpoint[0]*rb.resval.rpoint[0]+rb.resval.rpoint[1]*rb.resval.rpoint[1];
	viewdirlen=fr1+rb.resval.rpoint[2]*rb.resval.rpoint[2];
	ic_ptcpy(viewdir,rb.resval.rpoint);

	//temporarily set OSMODE for getting new viewpoint.
	if (SDS_getvar(NULL,DB_QOSMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) {
		return(RTERROR);
	}
	osmode=rb.resval.rint;
	rb.resval.rint=0;
	if (SDS_getvar(NULL,DB_QLASTPOINT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) {
		return(RTERROR);
	}
	ic_ptcpy(lastpt,rb.resval.rpoint);

	sds_rtos(viewdir[0],-1,-1,cst[0]);
	sds_rtos(viewdir[1],-1,-1,cst[1]);
	sds_rtos(viewdir[2],-1,-1,cst[2]);
	if (SDS_CURDWG->ret_nmainents()<=SDS_CURDWG->ret_ntabrecs(DB_VXTAB,1)){
		//perspective not allowed if drawing empty
		sds_initget(SDS_RSG_NOLIM, ResourceString(IDC_VPOINT_INIT1,"Rotate Plan_view|Plan ~_Rotate ~_Plan"));
		sprintf(fs1,ResourceString(IDC_CMDS3__NPLAN___ROTATE__113, "\nPlan/Rotate/<View point> <%s,%s,%s>: " ),cst[0],cst[1],cst[2]);
	}else{
		sds_initget(SDS_RSG_NOLIM, ResourceString(IDC_VPOINT_INIT2, "Rotate Plan_view|PLan Perspective|PErspective ~_Rotate ~_PLan ~_PErspective"));
		sprintf(fs1,ResourceString(IDC_CMDS3__NPERSPECTIVE____115, "\nPErspective/PLan/Rotate/<View point> <%s,%s,%s>: " ),cst[0],cst[1],cst[2]);
	}
	ret=sds_getpoint(NULL,fs1,viewdir);

	//immediately restore OSMODE  & LASTPOINT before doing any checking on getpoint return
	rb.restype=RTSHORT;
	rb.resval.rint=osmode;
	if (sds_setvar("OSMODE"/*DNT*/,&rb)!=RTNORM) {
		ret=RTERROR;
		goto exit;
	}
	rb.restype=RT3DPOINT;
	ic_ptcpy(rb.resval.rpoint,lastpt);
	SDS_setvar(NULL,DB_QLASTPOINT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);

	if (ret==RTCAN)
		goto exit;


	if (ret==RTNONE) {
		ret=cmd_viewctl();
		//skip all the zoom extents stuff and go right to exit
		exit_from_dialog=1;
		SDS_getvar(NULL,DB_QVIEWDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		ic_ptcpy(viewdir,rb.resval.rpoint);
		goto exit;
	} else if (ret==RTNORM) {
		viewmode&=(~1);
	} else if (ret==RTERROR) {
		goto exit;
	} else if (ret==RTKWORD){
		sds_getinput(fs1);
		if (strisame(fs1,"ROTATE"/*DNT*/)) {
			if (fr1<CMD_FUZZ){
				if (viewdir[2]<0.0)
					fr2=IC_PI/-2.0;
				else
					fr2=IC_PI/2.0;
				//if you're in plan view, Acad remembers the angle you entered IN the
				//	XY plane, even though it's not used for any of the calcs.
				if (cmd_vpoint_xyplanang_init)fr3=cmd_vpoint_xyplanang;
				else{
					cmd_vpoint_xyplanang_init=TRUE;
					cmd_vpoint_xyplanang=fr3=1.5*IC_PI;
				}
			}else{
				fr3=atan2(viewdir[1],viewdir[0]);
				fr2=atan2(viewdir[2],sqrt(fr1));
			}
			SDS_getvar(NULL,DB_QANGDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			fi1=rb.resval.rint;
			SDS_getvar(NULL,DB_QANGBASE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			//angtos uses angdir and angbase.  Offset for angbase, not angdir
			if (fi1==0){
				fr3+=rb.resval.rreal;
				fr2+=rb.resval.rreal;
			}else{
				fr3-=rb.resval.rreal;
				fr2-=rb.resval.rreal;
			}
			sds_angtos(fr3,-1,-1,fs2);
			sds_initget(0,NULL);
			sprintf(fs1,ResourceString(IDC_CMDS3__NANGLE_IN_XY_PL_118, "\nAngle in XY plane from X axis <%s>: " ),fs2);
			if ((ret=sds_getorient(NULL,fs1,&fr3))==RTCAN)
				goto exit;
			if (ret==RTNORM) {
				if (fi1==1)fr3=-fr3;
			}//if RTNONE, fr3 is unchanged as angle IN XY plane
			sds_angtos(fr2,-1,-1,fs2);
			sds_initget(0,NULL);
			sprintf(fs1,ResourceString(IDC_CMDS3__NANGLE_FROM_XY__119, "\nAngle from XY plane <%s>: " ),fs2);
			if (RTCAN==(ret=sds_getorient(NULL,fs1,&fr2)))
				goto exit;
			if (ret==RTNORM) {
				if (fi1==1)fr2=-fr2;
			}
			ret=RTNORM;
			while(fr2>= IC_PI)fr2-=IC_TWOPI;
			while(fr2<=-IC_PI)fr2+=IC_TWOPI;
			if (fr2>IC_PI*0.5){
				fr2=IC_PI-fr2;
				fr3+=IC_PI;
			}else if (fr2<-0.5*IC_PI){
				fr2=-IC_PI-fr2;
				fr3+=IC_PI;
			}
			if (icadAngleEqual(fr2,0.5*IC_PI)){
				viewdir[0]=viewdir[1]=0.0;
 				viewdir[2]=sqrt(viewdirlen);	/*D.G.*/// sqrt added.
				cmd_vpoint_xyplanang_init=TRUE;
				cmd_vpoint_xyplanang=fr3;
			}else if (icadAngleEqual(fr2,-0.5*IC_PI)){
				viewdir[0]=viewdir[1]=0.0;
 				viewdir[2]=-sqrt(viewdirlen);	/*D.G.*/// sqrt added.
				cmd_vpoint_xyplanang_init=TRUE;
				cmd_vpoint_xyplanang=fr3;
			}else{
				pt[0]=cos(fr3);
				pt[1]=sin(fr3);
				pt[2]=tan(fr2);
				fr3=sqrt(viewdirlen/(pt[0]*pt[0]+pt[1]*pt[1]+pt[2]*pt[2]));
				viewdir[0]=pt[0]*fr3;
				viewdir[1]=pt[1]*fr3;
				viewdir[2]=pt[2]*fr3;
			}

		}else if (strisame(fs1,"PLAN"/*DNT*/)){
			//if world view was on, then restore the previous ucs before using
			//	the plan.  otherwise it makes little sense.
			if (ucschg){
				SDS_getvar(NULL,DB_QCMDECHO,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				if (CMD_CMDECHO_PRINTF(rb.resval.rint))
					sds_printf(ResourceString(IDC_CMDS3__NRESTORING_UCS__121, "\nRestoring UCS." ));
				SDS_DrawUcsIcon(NULL,NULL,NULL,1);//paint out old icon

				SDS_setvar(NULL,DB_QWORLDUCS,&rbWorldUCS,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,1);

				rb.restype=RT3DPOINT;
				ic_ptcpy(rb.resval.rpoint,olducsxdir);
				SDS_setvar(NULL,DB_QUCSXDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,1);
				ic_ptcpy(rb.resval.rpoint,olducsydir);
				SDS_setvar(NULL,DB_QUCSYDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,1);
				ic_ptcpy(rb.resval.rpoint,olducsorg);
				SDS_setvar(NULL,DB_QUCSORG,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,1);
				rb.restype=RTSTR;
				rb.resval.rstring=olducsname;
				sdsengine_setvar("UCSNAME"/*DNT*/,&rb);
				SDS_InitThisView(SDS_CURVIEW);
				SDS_DrawUcsIcon(NULL,NULL,NULL,0);//paint in new icon
				ucschg=0;
			}
			if (viewmode&1){
				SDS_getvar(NULL,DB_QEXTMIN,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				ic_ptcpy(pt4,rb.resval.rpoint);
				SDS_getvar(NULL,DB_QEXTMAX,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				ic_ptcpy(pt5,rb.resval.rpoint);
				fr3=sds_distance(pt4,pt5);
				SDS_getvar(NULL,DB_QLENSLENGTH,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				viewdirlen=rb.resval.rreal*fr3/35.0;
				pt4[0]=(pt4[0]+pt5[0])/2.0;
				pt4[1]=(pt4[1]+pt5[1])/2.0;
				pt4[2]=(pt4[2]+pt5[2])/2.0;
				viewdir[0]=viewdir[1]=0.0;
				viewdir[2]=viewdirlen;
				SDS_ZoomIt(NULL,1,pt4,&fr3,&viewmode,viewdir,NULL,NULL);
			}else{
				ret=(cmd_view_plan(0));
			}
			goto exit;
		}else if (strisame(fs1,"PERSPECTIVE"/*DNT*/)){
			for(;;){
				if ((viewmode & 1)==0){
					sds_initget(0, ResourceString(IDC_VPOINT_PERSPECTIVE_INIT1, "Perspective_on|ON Perspective_off|OFf ~_ON ~_OFf"));
					sprintf(fs1,ResourceString(IDC_CMDS3__NPERSPECTIVE_ON_125, "\nPerspective ON/<OFf>: " ));
				}else{
					sds_initget(0, ResourceString(IDC_VPOINT_PERSPECTIVE_INIT2, "Perspective_on|ON Perspective_off|OFf Front_clipping|Front Back_clipping|Back ~_ON ~_OFf ~_Front ~_Back"));
					sprintf(fs1,ResourceString(IDC_CMDS3__NPERSPECTIVE____127, "\nPerspective:  Front clipping/Back clipping/Off/<ON>: " ));
				}
				if ((ret=sds_getkword(fs1,fs2))==RTERROR || ret==RTCAN){
					goto exit;
				}else if (ret==RTNONE){
					ret=RTNORM;
					break;
				}else if (ret==RTNORM){
		  if (strisame(fs2,"ON"/*DNT*/)){
						if (viewmode & 1) break;
						else viewmode|=1;
		  }else if (strisame(fs2,"OFF"/*DNT*/)){
						if (viewmode & 1) viewmode-=1;
						else break;
		  }else if (strisame(fs2,"FRONT"/*DNT*/)){
						if ((viewmode & 2)==0){
							sds_initget(0,ResourceString(IDC_VPOINT_FRONT_INIT1, "Front_clipping_on|ON Front_clipping_off|OFf ~_ON ~_OFf"));
							if ((ret=sds_getkword(ResourceString(IDC_CMDS3__NFRONT_CLIPPING_132, "\nFront clipping ON/<OFf>: " ),fs2))==RTNORM)
				if (strisame(fs2,"ON"/*DNT*/))viewmode+=2;
							else
								break;
						}
						if (viewmode & 2){
							sds_initget(0, ResourceString(IDC_VPOINT_ON_INIT1, "Front_clipping_off|Off ~_Off"));
							SDS_getvar(NULL,DB_QFRONTZ,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
							sds_rtos(rb.resval.rreal,-1,-1,fs2);
							sprintf(fs1,ResourceString(IDC_CMDS3__NOFF___FRONT_CL_134, "\nOff/Front clipping distance <%s>: " ),fs2);
							if ((ret=sds_getdist(NULL,fs1,&fr1))==RTCAN || ret==RTERROR) break;
							//if (ret==RTNONE) continue;
							if (ret==RTNORM){
								rb.resval.rreal=fr1;
								sds_setvar("FRONTZ"/*DNT*/,&rb);
							}else if (ret==RTKWORD){
								ret=RTNORM;
								viewmode-=2;
							}
						}

		  }else if (strisame(fs2,"BACK"/*DNT*/)){
						if ((viewmode & 4)==0){
							sds_initget(0, ResourceString(IDC_VPOINT_BACK_INIT1, "Back_clipping_on|ON Back_clipping_off|OFf ~_ON ~_OFf"));
							if ((ret=sds_getkword(ResourceString(IDC_CMDS3__NBACK_CLIPPING__138, "\nBack clipping ON/<OFf>: " ),fs2))==RTNORM)
								if (strisame(fs2,"ON"/*DNT*/))
									viewmode+=4;
								else
									break;
						}
						if (viewmode & 4){
							sds_initget(0, ResourceString(IDC_VPOINT_BACK_INIT2, "Back_clipping_off|Off ~_OFf"));
							SDS_getvar(NULL,DB_QFRONTZ,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
							sds_rtos(rb.resval.rreal,-1,-1,fs2);
							sprintf(fs1,ResourceString(IDC_CMDS3__NOFF___BACK_CLI_140, "\nOff/Back clipping distance <%s>: " ),fs2);
							if ((ret=sds_getdist(NULL,fs1,&fr1))==RTCAN || ret==RTERROR) break;
							//if (ret==RTNONE) continue;
							if (ret==RTNORM){
								rb.resval.rreal=fr1;
								sds_setvar("BACKZ"/*DNT*/,&rb);
							}else if (ret==RTKWORD){
								ret=RTNORM;
								viewmode-=4;
							}
						}
					}
				}
			}
			if (viewmode&1){
				ucschg=0;
				SDS_getvar(NULL,DB_QEXTMIN,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				ic_ptcpy(pt3,rb.resval.rpoint);
				SDS_getvar(NULL,DB_QEXTMAX,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				ic_ptcpy(pt4,rb.resval.rpoint);
				SDS_getvar(NULL,DB_QVIEWCTR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				cmd_elev_correct(0.5*(pt3[2]+pt4[2]),rb.resval.rpoint,1,NULL);
				SDS_ZoomIt(SDS_CURDWG,1,rb.resval.rpoint,NULL,&viewmode,NULL,NULL,NULL);
			}else{
				SDS_getvar(NULL,DB_QVIEWCTR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				SDS_ZoomIt(SDS_CURDWG,2,rb.resval.rpoint,NULL,&viewmode,NULL,NULL,NULL);
			}
			goto exit;
		}
	}
	if (icadRealEqual(viewdir[0],0.0)) {
		if (icadRealEqual(viewdir[1],0.0)) {
			if (icadRealEqual(viewdir[2],0.0)) {
				viewdir[2]=1;
			}
		}
	}
	//ACAD automatically zooms extents during vpoint command
	//set up fi1 as dwg empty flag
	if (SDS_CURDWG->ret_nmainents()>SDS_CURDWG->ret_ntabrecs(DB_VXTAB,1))fi1=0;
	else fi1=1;

	int tilemode,cvport;
	if (SDS_getvar(NULL,DB_QTILEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
		return(RTERROR);
	tilemode=rb.resval.rint;
	if (SDS_getvar(NULL,DB_QCVPORT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
		return(RTERROR);
	cvport=rb.resval.rint;

	SDS_getvar(NULL,
		(fi1 ? ((tilemode==0 && cvport==1) ? DB_QP_LIMMIN : DB_QLIMMIN) : ((tilemode==0 && cvport==1) ? DB_QP_EXTMIN : DB_QEXTMIN)),
		  &rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	ic_ptcpy(pt3,rb.resval.rpoint);
	SDS_getvar(NULL,
		(fi1 ? ((tilemode==0 && cvport==1) ? DB_QP_LIMMAX : DB_QLIMMAX) : ((tilemode==0 && cvport==1) ? DB_QP_EXTMAX : DB_QEXTMAX)),
		  &rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	ic_ptcpy(pt4,rb.resval.rpoint);

	pt1[0]=min(pt3[0],pt4[0]);
	pt1[1]=min(pt3[1],pt4[1]);
	pt1[2]=min(pt3[2],pt4[2]);
	pt2[0]=max(pt3[0],pt4[0]);
	pt2[1]=max(pt3[1],pt4[1]);
	pt2[2]=max(pt3[2],pt4[2]);
	if (fi1){
		SDS_getvar(NULL,DB_QELEVATION,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		pt1[2]=pt2[2]=rb.resval.rreal;
	}


	//we want to zoom extents for the new view in ONE STEP.
	//Change viewdir, then get a view structure, and  populate
	//it with the correct stuff for our new view
	pnewview=&newview;

	SDS_getvar(NULL,DB_QVIEWDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	ic_ptcpy(savevdir,rb.resval.rpoint);

	if (ucschg){
		SDS_getvar(NULL,DB_QCMDECHO,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		if (CMD_CMDECHO_PRINTF(rb.resval.rint))
			sds_printf(ResourceString(IDC_CMDS3__NRESTORING_UCS__121, "\nRestoring UCS." ));
		//restore ucs we initially kept
		SDS_setvar(NULL,DB_QWORLDUCS,&rbWorldUCS,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,1);
		rb.restype=RT3DPOINT;
		ic_ptcpy(rb.resval.rpoint,olducsxdir);
		SDS_setvar(NULL,DB_QUCSXDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,1);
		ic_ptcpy(rb.resval.rpoint,olducsydir);
		SDS_setvar(NULL,DB_QUCSYDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,1);
		ic_ptcpy(rb.resval.rpoint,olducsorg);
		SDS_setvar(NULL,DB_QUCSORG,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,1);
		rb.restype=RTSTR;
		rb.resval.rstring=olducsname;
		sdsengine_setvar("UCSNAME"/*DNT*/,&rb);
		ucschg=0;

		sds_trans(viewdir,&rbwcs,&rbucs,1,viewdir);
	}else{
		//nothing to do.  viewdir is in ucs coords, and we never reset the current ucs
		//sds_trans(viewdir,&rbucs,&rbwcs,1,viewdir);
	}
	rb.restype=RT3DPOINT;
	ic_ptcpy(rb.resval.rpoint,viewdir);
	SDS_setvar(NULL,DB_QVIEWDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
	if (viewmode!=oldviewmode){
		rb.restype=RTSHORT;
		rb.resval.rint=viewmode;
		SDS_setvar(NULL,DB_QVIEWMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
	}


	//populate the new struct

	struct gr_viewvars viewvarsp;
	SDS_VarToTab(SDS_CURDWG,SDS_CURVIEW->m_pVportTabHip);
	SDS_ViewToVars2Use(SDS_CURVIEW,SDS_CURDWG,&viewvarsp);
	gr_initview(SDS_CURDWG,&SDS_CURCFG,&pnewview,&viewvarsp);
	if (SDS_getvar(NULL,DB_QWORLDVIEW,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
		return(RTERROR);
	if (0==rb.resval.rint){
		//calc the new viewtwist we need to set
		for(fi1=0;fi1<2;fi1++){
			pt3[0]=pt3[1]=pt3[2]=0.0;
			if (!fi1)
				pt3[2]=1.0;
			else
				pt3[0]=1.0;
			sds_trans(pt3,&rbwcs,&rbucs,1,pt4);
			//pt4 is WCS Z axis in UCS coords, pt3 is UCS Z in UCS coords
			//NOTE: can't use sds_trans, because we need to specify alternate set of vars to use in xform
			gr_ucs2rp(pt3,pt3,pnewview);
			gr_ucs2rp(pt4,pt4,pnewview);
			pt5[0]=pt5[1]=pt5[2]=0.0;
			gr_ucs2rp(pt5,pt5,pnewview);
			pt3[0]-=pt5[0];pt3[1]-=pt5[1];pt3[2]-=pt5[2];
			pt4[0]-=pt5[0];pt4[1]-=pt5[1];pt4[2]-=pt5[2];
			if (icadRealEqual(pt4[0],0.0)&&icadRealEqual(pt4[1],0.0))
				continue;
			if (icadRealEqual(pt3[0],0.0)&&icadRealEqual(pt3[1],0.0)){
				//if they're going back to plan view for where they were...
				fi1=-1;
			}
			break;
		}
		if (fi1!=-1){
			//ignore the z's and get the angle between the vectors
			pt3[2]=atan2(pt3[1],pt3[0]);
			pt4[2]=atan2(pt4[1],pt4[0]);
			ic_normang(&pt3[2],&pt4[2]);
			newvtwist=pt4[2]-pt3[2];
		}
	}else{
		newvtwist=0.0;
	}
	if (newvtwist!=oldvtwist){
		rb.restype=RTREAL;
		rb.resval.rreal=newvtwist;
		SDS_setvar(NULL,DB_QVIEWTWIST,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,1);
		SDS_VarToTab(SDS_CURDWG,SDS_CURVIEW->m_pVportTabHip);
		SDS_ViewToVars2Use(SDS_CURVIEW,SDS_CURDWG,&viewvarsp);
		gr_initview(SDS_CURDWG,&SDS_CURCFG,&pnewview,&viewvarsp);
	}

	//now get extents according to that view
	SDS_rpextents(pt1,pt2,pt3,pt4,&newview);
	//pt3 & pt4 now represent screen coordinates
	//of dwg's extents.  Convert center of view
	//back to UCS using our new view also
	pt1[0]=(pt3[0]+pt4[0])/2.0;
	pt1[1]=(pt3[1]+pt4[1])/2.0;
	pt1[2]=0.0;
	gra_rp2ucs(pt1,pt2,pnewview);
	SDS_getvar(NULL,DB_QSCREENSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if (icadRealEqual(pt4[1],pt3[1]) || (((pt4[0]-pt3[0])/(pt4[1]-pt3[1]))>(rb.resval.rpoint[0]/rb.resval.rpoint[1]))){
		fr1=((pt4[0]-pt3[0])*rb.resval.rpoint[1])/rb.resval.rpoint[0];
	}else{
		fr1=pt4[1]-pt3[1];
	}
	fr1*=1.02;
	//Now call zoomit to save all the stuff after restoring original settings

	rb.restype=RT3DPOINT;
	ic_ptcpy(rb.resval.rpoint,savevdir);
	SDS_setvar(NULL,DB_QVIEWDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,1);
	if (viewmode!=oldviewmode){
		rb.restype=RTSHORT;
		rb.resval.rint=oldviewmode;
		SDS_setvar(NULL,DB_QVIEWMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,1);
	}
	if (newvtwist!=oldvtwist){
		rb.restype=RTREAL;
		rb.resval.rreal=oldvtwist;
		SDS_setvar(NULL,DB_QVIEWTWIST,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,1);
	}
	SDS_ZoomIt(NULL,2,pt2,&fr1,&viewmode,viewdir,&newvtwist,NULL);
	exit:

	if (ucschg){
		//restore ucs we initially kept
		if (!exit_from_dialog)SDS_DrawUcsIcon(NULL,NULL,NULL,1);//paint out old icon
		SDS_setvar(NULL,DB_QWORLDUCS,&rbWorldUCS,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,1);
		rb.restype=RT3DPOINT;
		ic_ptcpy(rb.resval.rpoint,olducsxdir);
		SDS_setvar(NULL,DB_QUCSXDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,1);
		ic_ptcpy(rb.resval.rpoint,olducsydir);
		SDS_setvar(NULL,DB_QUCSYDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,1);
		ic_ptcpy(rb.resval.rpoint,olducsorg);
		SDS_setvar(NULL,DB_QUCSORG,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,1);
		rb.restype=RTSTR;
		rb.resval.rstring=olducsname;
		sdsengine_setvar("UCSNAME"/*DNT*/,&rb);
		if (!exit_from_dialog){
			SDS_InitThisView(SDS_CURVIEW);
			SDS_DrawUcsIcon(NULL,NULL,NULL,0);//paint in new icon
		}
		if (exit_from_dialog){
			//use these 2 lines and we'll save current stuff before setting new
			sds_trans(viewdir,&rbwcs,&rbucs,1,viewdir);
			SDS_ZoomIt(NULL,2,NULL,NULL,NULL,viewdir,NULL,NULL);
			//these lines below do the same thing w/o saving view
			//CIcadView *pView=SDS_CURVIEW;
			//gra_initview(SDS_CURDWG,&SDS_CURCFG,&pView->m_pCurDwgView,NULL);
			//SDS_BuildRegenList(SDS_CURDWG,pView->m_pCurDwgView,pView,SDS_CURDOC);
			//sds_redraw(NULL,IC_REDRAW_DONTCARE);
		}
	}
	return(ret);
}

//=================================================================================
//	The time command
//=================================================================================

void cmd_prn_dwg_times(int mode) {
// mode = 0 for call from time
//		  1 for call from date

	time_t ltime;
	struct resbuf rb;
	short iYear,iMonth,iDay,iDow,iHour,iMin;
	double dTmp,dTmp1,dDays,dSec;
	char usrtimestring[20];
	LPCTSTR szMonthResstrs[13] = {
		"Jan" , "Feb" ,  "Mar" ,  "Apr" ,  "May" ,	"Jun" ,  "Jul" ,  "Aug" ,  "Sep" ,	"Oct" ,  "Nov" ,  "Dec" };

	int monthresconsts[]= {
		IDC_CMDS3_JAN_142,	IDC_CMDS3_FEB_143,	IDC_CMDS3_MAR_144,	IDC_CMDS3_APR_145,	IDC_CMDS3_MAY_146,	IDC_CMDS3_JUN_147,
		IDC_CMDS3_JUL_148,	IDC_CMDS3_AUG_149,	IDC_CMDS3_SEP_150,	IDC_CMDS3_OCT_151,	IDC_CMDS3_NOV_152,	IDC_CMDS3_DEC_153
	};

	LPCTSTR szDayResstrs[7] = {
		"Sun" ,  "Mon" ,  "Tue" ,  "Wed" ,	"Thu" ,  "Fri" ,  "Sat"
	};

	int dayresconsts[]= {
		IDC_CMDS3_SUN_154, IDC_CMDS3_MON_155, IDC_CMDS3_TUE_156, IDC_CMDS3_WED_157, IDC_CMDS3_THU_158, IDC_CMDS3_FRI_159, IDC_CMDS3_SAT_160
	};


	if (mode==1){
		sds_getvar("DATE"/*DNT*/,&rb);
		db_jul2greg(rb.resval.rreal,&iYear,&iMonth,&iDay,&iDow,&iHour,&iMin,&dSec);
		sds_printf(ResourceString(IDC_CMDS3__N_3S__3S__2D__4_162, "\n%3s %3s %d, %4d  %2d:%02d:%02d" ),
						 ResourceString(dayresconsts[iDow],szDayResstrs[iDow]),
						 ResourceString(monthresconsts[iMonth-1],szMonthResstrs[iMonth-1]),
						 iDay,iYear,iHour,iMin,(int)dSec);
	}else if (mode==0){

		sds_getvar("USRTIMER"/*DNT*/,&rb);
		if (rb.resval.rint > 0) {
			strcpy(usrtimestring,ResourceString(IDC_CMDS3_ON_164, "on" ));
		}else{
			strcpy(usrtimestring,ResourceString(IDC_CMDS3_OFF_165, "off" ));
		}

		time(&ltime);
		sds_printf(ResourceString(IDC_CMDS3__NTHE_CURRENT_TI_166, "\nThe current time is %s" ),ctime(&ltime));

		sds_getvar("TDCREATE"/*DNT*/,&rb);
		db_jul2greg(rb.resval.rreal,&iYear,&iMonth,&iDay,&iDow,&iHour,&iMin,&dSec);
		sds_printf(ResourceString(IDC_CMDS3__NDRAWING_WAS_CR_168, "\nDrawing was created on:      %3s  %3s %d, %4d at %2d:%02d:%02d" ),
						 ResourceString(dayresconsts[iDow],szDayResstrs[iDow]),
						 ResourceString(monthresconsts[iMonth-1],szMonthResstrs[iMonth-1]),
						 iDay,iYear,iHour,iMin,(int)dSec);

		sds_getvar("TDUPDATE"/*DNT*/,&rb);
		db_jul2greg(rb.resval.rreal,&iYear,&iMonth,&iDay,&iDow,&iHour,&iMin,&dSec);
		sds_printf(ResourceString(IDC_CMDS3__NDRAWING_WAS_LA_170, "\nDrawing was last updated on: %3s  %3s %d, %4d at %2d:%02d:%02d" ),
						 ResourceString(dayresconsts[iDow],szDayResstrs[iDow]),
						 ResourceString(monthresconsts[iMonth-1],szMonthResstrs[iMonth-1]),
						 iDay,iYear,iHour,iMin,(int)dSec);

		sds_getvar("TDINDWG"/*DNT*/,&rb);
		//days
		dTmp=modf(rb.resval.rreal,&dDays);
		//hours
		dTmp*=24;
		dTmp=modf(dTmp,&dTmp1);
		iHour=(int)dTmp1;
		//minutes
		dTmp*=60;
		dTmp=modf(dTmp,&dTmp1);
		iMin=(int)dTmp1;
		//seconds
		dSec=dTmp*60;
		//print it
		sds_printf(ResourceString(IDC_CMDS3__NTOTAL_EDITING__172, "\nTotal editing time: %10.0f days %2d hours %2d minutes %2.4f seconds" ),dDays,iHour,iMin,dSec);

		sds_getvar("TDUSRTIMER"/*DNT*/,&rb);
		//days
		dTmp=modf(rb.resval.rreal,&dDays);
		//hours
		dTmp*=24;
		dTmp=modf(dTmp,&dTmp1);
		iHour=(int)dTmp1;
		//miniutes
		dTmp*=60;
		dTmp=modf(dTmp,&dTmp1);
		iMin=(int)dTmp1;
		//seconds
		dSec=dTmp*60;
		//print it
		sds_printf(ResourceString(IDC_CMDS3__NELAPSED_TIMER__174, "\nElapsed timer (%s): %10.0f days %2d hours %2d minutes %2.4f seconds" ),usrtimestring,dDays,iHour,iMin,dSec);
	}

}

short cmd_time(void) {

	if (cmd_iswindowopen() == RTERROR)
		return RTERROR;

	struct resbuf rb;
	char fs1[IC_ACADBUF];
	RT ret;

	while(1) {

		if ((sds_getvar("WNDLCMD"/*DNT*/,&rb))!=RTNORM) // Get state of command line
			return(RTERROR);
		if (rb.resval.rint==0) {							// If it's off
			sds_textscr();								//	  bring up the text screen.
		}

		cmd_prn_dwg_times(0);

		// TODO Need to display the current drawing timer stuff.
		if (sds_initget(0, ResourceString(IDC_CMDS3_TIMER_ON_ON_TIME_176,"Timer_on|ON Timer_off|OFf ~ Display_timer|Display Reset_timer|Reset ~_ON ~_OFf ~_ ~_Display ~_Reset"))!=RTNORM)
			return(-2);
		if ((ret=sds_getkword(ResourceString(IDC_CMDS3__NON___OFF___DIS_177, "\nON/OFF/Display/Reset: " ),fs1))==RTERROR) {
			return(ret);
		} else if (ret==RTCAN) {
			return(ret);
		} else if (ret==RTNONE) {
			return(ret);
		}

		 if (strisame("DISPLAY"/*DNT*/,fs1)) {
		} else if (strisame("ON"/*DNT*/,fs1)) {
			sds_printf(ResourceString(IDC_CMDS3__NTHE_DRAWING_TI_179, "\nThe drawing timer is ON." ));
			rb.restype=RTSHORT;
			rb.resval.rint=1;
			sds_setvar("USRTIMER"/*DNT*/,&rb);
		} else if (strisame("OFF"/*DNT*/,fs1)) {
			sds_printf(ResourceString(IDC_CMDS3__NTHE_DRAWING_TI_181, "\nThe drawing timer is OFF." ));
			rb.restype=RTSHORT;
			rb.resval.rint=0;
			sds_setvar("USRTIMER"/*DNT*/,&rb);
		} else if (strisame("RESET"/*DNT*/,fs1)) {
			rb.restype=RTREAL;
			rb.resval.rreal=0.0;
			sds_setvar("TDUSRTIMER"/*DNT*/,&rb);
		}
	}
	return(RTNORM);
}


//===================================================================================

short cmd_status(void)	 {

	if (cmd_iswindowopen() == RTERROR)
		return RTERROR;

	struct resbuf setgetrb,rbucs,rbwcs;
	LPCTSTR fcp1;

	rbucs.restype=rbwcs.restype=RTSHORT;
	rbucs.resval.rint=1;
	rbwcs.resval.rint=0;

	setgetrb.resval.rstring=NULL;
	setgetrb.rbnext=NULL;

	sds_textpage();
	sds_printf(ResourceString(IDC_CMDS3__NSTATUS__N_183, "\nStatus:\n" ));

	if (SDS_getvar(NULL,DB_QDWGNAME,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) {
		return(RTERROR);
	}
	cmd_prtstr(setgetrb.resval.rstring,ResourceString(IDC_CMDS3_CURRENT_DRAWING__184, "Current drawing name:" ));
	IC_FREE(setgetrb.resval.rstring);

	if (SDS_getvar(NULL,DB_QLIMMIN,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) {
		return(RTERROR);
	}
	cmd_prtpt(setgetrb.resval.rpoint,ResourceString(IDC_CMDS3_DRAWING_LIMITS_A_185, "Drawing limits are:" ));

	if (SDS_getvar(NULL,DB_QLIMMAX,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) {
		return(RTERROR);
	}
	cmd_prtpt(setgetrb.resval.rpoint,""/*DNT*/);

	if (SDS_getvar(NULL,DB_QP_LIMMIN,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) {
		return(RTERROR);
	}
	cmd_prtpt(setgetrb.resval.rpoint,ResourceString(IDC_CMDS3_PAPER_SPACE_LIMI_186, "Paper space limits are:" ));

	if (SDS_getvar(NULL,DB_QP_LIMMAX,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) {
		return(RTERROR);
	}
	cmd_prtpt(setgetrb.resval.rpoint,""/*DNT*/);

	if (SDS_getvar(NULL,DB_QSCREENSIZE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) {
		return(RTERROR);
	}
	cmd_prtint((int)setgetrb.resval.rpoint[0],ResourceString(IDC_CMDS3_SCREEN_WIDTH___P_187, "Screen width  (pixels):" ));
	cmd_prtint((int)setgetrb.resval.rpoint[1],ResourceString(IDC_CMDS3_SCREEN_HEIGHT__P_188, "Screen height (pixels):" ));

	if (SDS_getvar(NULL,DB_QINSBASE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) {
		return(RTERROR);
	}
	cmd_prtpt(setgetrb.resval.rpoint,ResourceString(IDC_CMDS3_INSERTION_BASE_I_189, "Insertion base is:" ));

	if (SDS_getvar(NULL,DB_QSNAPUNIT,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) {
		return(RTERROR);
	}
	cmd_prtpt(setgetrb.resval.rpoint,ResourceString(IDC_CMDS3_SNAP_RESOLUTION__190, "Snap resolution is:" ));

	if (SDS_getvar(NULL,DB_QGRIDUNIT,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) {
		return(RTERROR);
	}
	cmd_prtpt(setgetrb.resval.rpoint,ResourceString(IDC_CMDS3_GRID_SPACING_IS__191, "Grid spacing is:" ));

	if (SDS_getvar(NULL,DB_QCLAYER,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) {
		return(RTERROR);
	}
	cmd_prtstr(setgetrb.resval.rstring,ResourceString(IDC_CMDS3_CURRENT_LAYER__192, "Current layer:" ));
	IC_FREE(setgetrb.resval.rstring);

	if (SDS_getvar(NULL,DB_QCECOLOR,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) {
		return(RTERROR);
	}
	cmd_prtstr(setgetrb.resval.rstring,ResourceString(IDC_CMDS3_CURRENT_COLOR__193, "Current color:" ));
	IC_FREE(setgetrb.resval.rstring);

	if (SDS_getvar(NULL,DB_QCELTYPE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) {
		return(RTERROR);
	}
	cmd_prtstr(setgetrb.resval.rstring,ResourceString(IDC_CMDS3_CURRENT_LINETYPE_194, "Current linetype:" ));
	IC_FREE(setgetrb.resval.rstring);

	if (SDS_getvar(NULL,DB_QELEVATION,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) {
		return(RTERROR);
	}
	cmd_prtreal(setgetrb.resval.rreal,ResourceString(IDC_CMDS3_CURRENT_ELEVATIO_195, "Current elevation:" ));

	if (SDS_getvar(NULL,DB_QTHICKNESS,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) {
		return(RTERROR);
	}
	cmd_prtreal(setgetrb.resval.rreal,ResourceString(IDC_CMDS3_CURRENT_THICKNES_196, "Current thickness:" ));

	// EBATECH(CNBR) -[ 24-03-2003 Bugzilla#78471 Support Lineweight
	if (SDS_getvar(NULL,DB_QCELWEIGHT,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) {
		return(RTERROR);
	}
	fcp1=ResourceString(IDC_CMDS3_LINEWEIGHT_570, "Current Lineweight:" );
	switch(setgetrb.resval.rint) {
		case DB_DFWEIGHT:
			cmd_prtstr(ResourceString(IDC_CMDS3_DEFAULT_245, "Default" ),fcp1);
			break;
		case DB_BBWEIGHT:
			cmd_prtstr(ResourceString(IDC_CMDS3_BYBLOCK_325, "ByBlock" ),fcp1);
			break;
		case DB_BLWEIGHT:
			cmd_prtstr(ResourceString(IDC_CMDS3_BYLAYER_324, "ByLayer" ),fcp1);
			break;
		default:
			sds_real rVal = 0.01 * setgetrb.resval.rint;
			if (SDS_getvar(NULL,DB_QLWUNITS,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) {
				return(RTERROR);
			}
			if( setgetrb.resval.rint == 0 )
				rVal /= 25.4;
			cmd_prtreal(rVal,fcp1);
			break;
	}
	//EBATECH(CNBR)]-

	if (SDS_getvar(NULL,DB_QFILLMODE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) {
		return(RTERROR);
	}
	cmd_prtstr(setgetrb.resval.rint==0 ? ResourceString(IDC_CMDS3_OFF_165, "off" ) : ResourceString(IDC_CMDS3_ON_164, "on" ),ResourceString(IDC_CMDS3_FILL__197, "Fill:" ));

	if (SDS_getvar(NULL,DB_QGRIDMODE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) {
		return(RTERROR);
	}
	cmd_prtstr(setgetrb.resval.rint==0 ? ResourceString(IDC_CMDS3_OFF_165, "off" ) : ResourceString(IDC_CMDS3_ON_164, "on" ),ResourceString(IDC_CMDS3_GRID__198, "Grid:" ));

	if (SDS_getvar(NULL,DB_QORTHOMODE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) {
		return(RTERROR);
	}
	cmd_prtstr(setgetrb.resval.rint==0 ? ResourceString(IDC_CMDS3_OFF_165, "off" ) : ResourceString(IDC_CMDS3_ON_164, "on" ),ResourceString(IDC_CMDS3_ORTHO__199, "Ortho:" ));

	if (SDS_getvar(NULL,DB_QSNAPMODE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) {
		return(RTERROR);
	}
	cmd_prtstr(setgetrb.resval.rint==0 ? ResourceString(IDC_CMDS3_OFF_165, "off" ) : ResourceString(IDC_CMDS3_ON_164, "on" ),ResourceString(IDC_CMDS3_SNAP__200, "Snap:" ));

	if (SDS_getvar(NULL,DB_QBLIPMODE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) {
		return(RTERROR);
	}
	cmd_prtstr(setgetrb.resval.rint==0 ? ResourceString(IDC_CMDS3_OFF_165, "off" ) : ResourceString(IDC_CMDS3_ON_164, "on" ),ResourceString(IDC_CMDS3_BLIPS__201, "Blips:" ));

	if (SDS_getvar(NULL,DB_QDRAGMODE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) {
		return(RTERROR);
	}
	cmd_prtstr(setgetrb.resval.rint==0 ? ResourceString(IDC_CMDS3_OFF_165, "off" ) : ResourceString(IDC_CMDS3_ON_164, "on" ),ResourceString(IDC_CMDS3_DRAG__202, "Drag:" ));

	// EBATECH(CNBR) -[ 24-03-2003 Bugzilla#78471 Support Lineweight
	if (SDS_getvar(NULL,DB_QLWDISPLAY,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) {
		return(RTERROR);
	}
	cmd_prtstr(setgetrb.resval.rint==0 ? ResourceString(IDC_CMDS3_OFF_165, "off" ) : ResourceString(IDC_CMDS3_ON_164, "on" ),ResourceString(IDC_CMDS3_LINEWEIGHT_571, "LWeight:" ));
	// EBATECH(CNBR) ]-

	if (SDS_getvar(NULL,DB_QCMDECHO,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) {
		return(RTERROR);
	}
	cmd_prtstr(setgetrb.resval.rint==0 ? ResourceString(IDC_CMDS3_OFF_165, "off" ) : ResourceString(IDC_CMDS3_ON_164, "on" ),ResourceString(IDC_CMDS3_COMMAND_ECHO__203, "Command echo:" ));

	if (SDS_getvar(NULL,DB_QANGDIR,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) {
		return(RTERROR);
	}
	cmd_prtstr(setgetrb.resval.rint==0 ? ResourceString(IDC_CMDS3_COUNTERCLOCKWISE_204, "Counterclockwise" ) : ResourceString(IDC_CMDS3_CLOCKWISE_205, "Clockwise" ),ResourceString(IDC_CMDS3_POSITIVE_ANGLE_D_206, "Positive angle direction:" ));

	if (SDS_getvar(NULL,DB_QAUNITS,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) {
		return(RTERROR);
	}
	fcp1=ResourceString(IDC_CMDS3_ANGULAR_UNITS__207, "Angular units:" );
	switch(setgetrb.resval.rint) {
		case 0:
			cmd_prtstr(ResourceString(IDC_CMDS3_DECIMAL_DEGREES_208, "Decimal degrees" ),fcp1);
			break;
		case 1:
			cmd_prtstr(ResourceString(IDC_CMDS3_DEGREES_MINUTES__209, "Degrees/minutes/seconds" ),fcp1);
			break;
		case 2:
			cmd_prtstr(ResourceString(IDC_CMDS3_GRADS_210, "Grads" ),fcp1);
			break;
		case 3:
			cmd_prtstr(ResourceString(IDC_CMDS3_RADIANS_211, "Radians" ),fcp1);
			break;
		case 4:
			cmd_prtstr(ResourceString(IDC_CMDS3_SURVEYOR_S_UNITS_212, "Surveyor's units" ),fcp1);
			break;
	}

	if (SDS_getvar(NULL,DB_QDIMUNIT,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) {
		return(RTERROR);
	}
	fcp1=ResourceString(IDC_CMDS3_DIMENSION_UNITS__213, "Dimension units:" );		// TODO - Not an error
	switch(setgetrb.resval.rint) {
		case 1:
			cmd_prtstr(ResourceString(IDC_CMDS3_SCIENTIFIC_214, "Scientific" ),fcp1);
			break;
		case 2:
			cmd_prtstr(ResourceString(IDC_CMDS3_DECIMAL_215, "Decimal" ),fcp1);
			break;
		case 3:
			cmd_prtstr(ResourceString(IDC_CMDS3_ENGINEERING_216, "Engineering" ),fcp1);
			break;
		case 4:
			cmd_prtstr(ResourceString(IDC_CMDS3_ARCHITECTURAL_217, "Architectural" ),fcp1);
			break;
		case 5:
			cmd_prtstr(ResourceString(IDC_CMDS3_FRACTIONAL_218, "Fractional" ),fcp1);
			break;
	}

	if (SDS_getvar(NULL,DB_QPICKBOX,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) {
		return(RTERROR);
	}
	cmd_prtint(setgetrb.resval.rint,ResourceString(IDC_CMDS3_PICKBOX_HEIGHT__219, "Pickbox height:" ));
	cmd_prtint(SDS_CURDWG->ret_nmainents(),ResourceString(IDC_CMDS3_ENTITIES_IN_DRAW_220, "Entities in drawing:" ));

	if (setgetrb.restype==RTSTR) {
		IC_FREE(setgetrb.resval.rstring);
	}

	return(RTNORM);
}

//========================================================================================

short cmd_rectang(void)
	{

	if (cmd_iswindowopen() == RTERROR)
		return RTERROR;

	//BugZilla No. 78050; 25-03-2002
	if (cmd_istransparent() == RTERROR) return RTERROR;

	sds_name ename,tempent;
	int fi1,nverts;
	sds_point pt1, pt2, pt3, ptd;
	RT ret;
	struct resbuf *elist,rb,rbent,rbucs;
	sds_real fr1,width=0.0, thick,curelev,newelev,fill,chama,chamb,angsin,angcos;
	char fs1[IC_ACADNMLEN],fs2[IC_ACADBUF];
	int plinetype;
	resbuf *trb;
	int i;

	SDS_CURDOC->m_rRectang_mode;	//Bit-coded modes for cmd_rectang_mode
									//	IC_RECT_ROTATED				- Rotated
									//	IC_RECT_SQUARE				- Square
									//	IC_RECT_FILLETED			- Filleted mode on, but not necessarily overridden
									//	IC_RECT_CHAMFERED			- Chamfered mode on, but not necessarily overridden
									//	IC_RECT_THICKENESSOVERRIDE	- Thickness override on
									//	IC_RECT_WIDTHOVERRIDE		- Width override on
									//	IC_RECT_ELEVATIONOVERRIDE	- Elevation override on
									//	IC_RECT_USINGFILLETOVERRIDE - Using override ( NOT sysvar) for fillet
									//	IC_RECT_USINGCHAMFEROVERRIDE- Using override ( NOT sysvar) for chamfera & b

	SDS_CURDOC->m_rRectang_mode &= ~(IC_RECT_ROTATED | IC_RECT_SQUARE);  //All modes but rotated and square carry over for session

	ret=RTNORM;
	elist=NULL;

	if (SDS_getvar(NULL,DB_QUCSXDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
		return(-2);
	ic_ptcpy(pt1,rb.resval.rpoint);
	if (SDS_getvar(NULL,DB_QUCSYDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
		return(-2);
	ic_ptcpy(pt2,rb.resval.rpoint);
	rbent.restype=RT3DPOINT;
	rbent.resval.rpoint[0]=(pt1[1]*pt2[2])-(pt2[1]*pt1[2]);
	rbent.resval.rpoint[1]=(pt1[2]*pt2[0])-(pt2[2]*pt1[0]);
	rbent.resval.rpoint[2]=(pt1[0]*pt2[1])-(pt2[0]*pt1[1]);
	rbucs.restype=RTSHORT;
	rbucs.resval.rint=1;

	if (SDS_getvar(NULL,DB_QELEVATION,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
		return(-2);
	curelev=newelev=rb.resval.rreal;

	if (SDS_CURDOC->m_rRectang_mode & IC_RECT_FILLETED)
		{
		if (!(SDS_CURDOC->m_rRectang_mode & IC_RECT_USINGFILLETOVERRIDE))
			{
			if (SDS_getvar(NULL,DB_QFILLETRAD,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
				fill=0.0;
			else
				fill=rb.resval.rreal;
			}
		else
			fill=SDS_CURDOC->m_rRectang_fill;
		}
	else
		fill=0.0;

	chama=chamb=0.0;
	if (SDS_CURDOC->m_rRectang_mode & IC_RECT_CHAMFERED)
		{
		if (!(SDS_CURDOC->m_rRectang_mode & IC_RECT_USINGCHAMFEROVERRIDE))
			{
			if (SDS_getvar(NULL,DB_QCHAMMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
				return(-2);
			if (0==rb.resval.rint)
				{
				if (SDS_getvar(NULL,DB_QCHAMFERA,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)==RTNORM)
					chama=rb.resval.rreal;
				if (SDS_getvar(NULL,DB_QCHAMFERB,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)==RTNORM)
					chamb=rb.resval.rreal;
				}
			else
				{
				if (SDS_getvar(NULL,DB_QCHAMFERC,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)==RTNORM)
					chama=rb.resval.rreal;
				if (SDS_getvar(NULL,DB_QCHAMFERD,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)==RTNORM)
					chamb=rb.resval.rreal;
				}
			}
		else
			{
			chama=SDS_CURDOC->m_rRectang_chama;
			chamb=SDS_CURDOC->m_rRectang_chamb;
			}
		}

	if (SDS_CURDOC->m_rRectang_mode & IC_RECT_THICKNESSOVERRIDE)
		thick=SDS_CURDOC->m_rRectang_thick;
	else if (SDS_getvar(NULL,DB_QTHICKNESS,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
		thick=0.0;
	else
		thick=rb.resval.rreal;

	if (SDS_CURDOC->m_rRectang_mode & IC_RECT_WIDTHOVERRIDE)
		width=SDS_CURDOC->m_rRectang_width;
	else
		width=0.0;

	if (SDS_CURDOC->m_rRectang_mode & IC_RECT_ELEVATIONOVERRIDE)
		{
		SDS_CorrectElevation=curelev;
		SDS_ElevationChanged=TRUE;
		newelev=rb.resval.rreal=SDS_CURDOC->m_rRectang_elev;
		rb.restype=RTREAL;
		sds_setvar("ELEVATION"/*DNT*/,&rb);
		}
	firstcorner:
	if ((SDS_CURDOC->m_rRectang_mode&3)==0)
		{
		sds_initget(SDS_RSG_NONULL+SDS_RSG_OTHER, ResourceString(IDC_RECTANG_INIT1, "Chamfer Fillet ~ Rotated Square ~ Elevation Thickness Width_of_line|Width ~_Chamfer ~_Fillet ~_ ~_Rotated ~_Square ~_ ~_Elevation ~_Thickness ~_Width"));
		ret=sds_getpoint(NULL,ResourceString(IDC_CMDS3__NCHAMFER___ELEV_223, "\nChamfer/Elevation/Fillet/Rotated/Square/Thickness/Width/<Select first corner of rectangle>: " ),pt1);
		}
	else if ((SDS_CURDOC->m_rRectang_mode&3)==1)
		{
		sds_initget(SDS_RSG_NONULL+SDS_RSG_OTHER, ResourceString(IDC_RECTANG_INIT2, "Chamfer Fillet ~ Square ~ Elevation Thickness Width_of_line|Width ~_Chamfer ~_Fillet ~_ ~_Square ~_ ~_Elevation ~_Thickness ~_Width"));
		ret=sds_getpoint(NULL,ResourceString(IDC_CMDS3__NCHAMFER___ELEV_225, "\nChamfer/Elevation/Fillet/Square/Thickness/Width/<Select first corner of rectangle>: " ),pt1);
		}
	else
		{	//&3==2
		sds_initget(SDS_RSG_NONULL+SDS_RSG_OTHER, ResourceString(IDC_RECTANG_INIT3, "Chamfer Fillet ~ Rotated ~ Elevation Thickness Width_of_line|Width ~_Chamfer ~_Fillet ~_ ~_Rotated ~_ ~_Elevation ~_Thickness ~_Width"));
		ret=sds_getpoint(NULL,ResourceString(IDC_CMDS3__NCHAMFER___ELEV_227, "\nChamfer/Elevation/Fillet/Rotated/Thickness/Width/<Select first corner of rectangle>: " ),pt1);
		}
	if (ret==RTKWORD)
		{
		sds_getinput(fs1);
		if (strisame(fs1,"ROTATED"/*DNT*/))
			{
			SDS_CURDOC->m_rRectang_mode |= IC_RECT_ROTATED;
			SDS_CURDOC->m_rRectang_mode &= ~IC_RECT_SQUARE;
			}
		else if (strisame(fs1,"SQUARE"/*DNT*/))
			{
			SDS_CURDOC->m_rRectang_mode |= IC_RECT_SQUARE;
			SDS_CURDOC->m_rRectang_mode &= ~IC_RECT_ROTATED;
		}
	else if (strisame(fs1,"THICKNESS"/*DNT*/))
		{
		if (SDS_CURDOC->m_rRectang_mode & IC_RECT_THICKNESSOVERRIDE)
			{
			sds_rtos(thick,-1,-1,fs1);
			sds_initget(SDS_RSG_NONEG, ResourceString(IDC_RECTANG_DEFAULT_INIT1, "Use_default|Default ~_Default"));
			sprintf(fs2,ResourceString(IDC_CMDS3__NDEFAULT___RECT_232, "\nDefault/Rectangle thickness <%s> : " ),fs1);
			}
		else
			{
			sds_initget(SDS_RSG_NONEG,NULL);
			sprintf(fs2,ResourceString(IDC_CMDS3__NTHICKNESS_TO_U_233, "\nThickness to use for all rectangles: " ));
			}
			ret=sds_getdist(NULL,fs2,&rb.resval.rreal);
			if (ret==RTNORM)
				{
				SDS_CURDOC->m_rRectang_mode |= IC_RECT_THICKNESSOVERRIDE;
				SDS_CURDOC->m_rRectang_thick=thick=rb.resval.rreal;
				}
			else if (ret==RTKWORD)
				{
				SDS_CURDOC->m_rRectang_mode &= ~IC_RECT_THICKNESSOVERRIDE;
				SDS_getvar(NULL,DB_QTHICKNESS,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				thick=rb.resval.rreal;
				}
			else if (ret==RTCAN)
				return(ret);
		}
	else if (strisame(fs1,"WIDTH"/*DNT*/))
		{
		if (SDS_CURDOC->m_rRectang_mode & IC_RECT_WIDTHOVERRIDE)
			{
			sds_rtos(width,-1,-1,fs1);
			sds_initget(RSG_NONEG, ResourceString(IDC_RECTANG_DEFAULT_INIT1, "Use_default|Default ~_Default"));
			sprintf(fs2,ResourceString(IDC_CMDS3__NDEFAULT_PLINE__235, "\nDefault pline width/Rectangle width <%s> : " ),fs1);
		}
		else
			{
			sds_initget(0,NULL);
			sprintf(fs2,ResourceString(IDC_CMDS3__NWIDTH_TO_USE_F_236, "\nWidth to use for all rectangles: " ));
			}
		ret=sds_getdist(NULL,fs2,&rb.resval.rreal);
		if (ret==RTNORM)
			{
			SDS_CURDOC->m_rRectang_mode |= IC_RECT_WIDTHOVERRIDE;
			SDS_CURDOC->m_rRectang_width=width=rb.resval.rreal;
			}
		else if (ret==RTKWORD)
			{
				SDS_CURDOC->m_rRectang_mode &= ~IC_RECT_WIDTHOVERRIDE;
				SDS_getvar(NULL,DB_QPLINEWID,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				width=rb.resval.rreal;
			}
		else if (ret==RTCAN)
			return(ret);
		}
	else if (strisame(fs1,"ELEVATION"/*DNT*/))
		{
		if (SDS_CURDOC->m_rRectang_mode & IC_RECT_ELEVATIONOVERRIDE)
			{
			sds_rtos(newelev,-1,-1,fs1);
			sds_initget(0, ResourceString(IDC_RECTANG_DEFAULT_INIT1, "Use_default|Default ~_Default"));
			sprintf(fs2,ResourceString(IDC_CMDS3__NDEFAULT___RECT_238, "\nDefault/Rectangle elevation <%s> : " ),fs1);
			}
		else
			{
			sds_initget(0,NULL);
			sprintf(fs2,ResourceString(IDC_CMDS3__NELEVATION_TO_U_239, "\nElevation to use for all rectangles: " ));
			}
		ret=sds_getdist(NULL,fs2,&rb.resval.rreal);
		if (ret==RTKWORD ||(ret==RTNORM && icadRealEqual(rb.resval.rreal,0.0)))
			{
			SDS_CURDOC->m_rRectang_mode &= ~IC_RECT_ELEVATIONOVERRIDE;
			rb.resval.rreal=newelev=curelev;
			SDS_ElevationChanged=FALSE;
			rb.restype=RTREAL;
			sds_setvar("ELEVATION"/*DNT*/,&rb);
			}
		else if (ret==RTNORM)
			{
			SDS_CURDOC->m_rRectang_mode |= IC_RECT_ELEVATIONOVERRIDE;
			SDS_CURDOC->m_rRectang_elev=newelev=rb.resval.rreal;
			SDS_CorrectElevation=curelev;
			SDS_ElevationChanged=TRUE;
			rb.restype=RTREAL;
			sds_setvar("ELEVATION"/*DNT*/,&rb);
			}
		else if (ret==RTCAN)
			return(ret);
		}
	else if (strisame(fs1,"FILLET"/*DNT*/))
		{
		if (SDS_CURDOC->m_rRectang_mode & IC_RECT_FILLETED)
			{
			sds_rtos(fill,-1,-1,fs1);
			sds_initget(SDS_RSG_NONEG, ResourceString(IDC_RECTANG_FILLET_INIT1, "Turn_fillet_off|Off ~ Use_default_setting|Default ~_Off ~_ ~_Default"));
			sprintf(fs2,ResourceString(IDC_CMDS3__NOFF___DEFAULT__242, "\nOff/Default/Fillet distance to use for rectangles <%s> : " ),fs1);
			}
		else
			{
			sds_rtos(0.0,-1,-1,fs1);
			sds_initget(SDS_RSG_NONEG, ResourceString(IDC_RECTANG_DEFAULT_INIT2, "Use_default_setting|Default ~_Default"));
			sprintf(fs2,ResourceString(IDC_CMDS3__NFILLET_DISTANC_244, "\nFillet distance to use for all rectangles <%s> : " ),fs1);
			}
			ret=sds_getdist(NULL,fs2,&rb.resval.rreal);
			if (ret==RTNORM)
				{
				SDS_CURDOC->m_rRectang_mode |= (IC_RECT_FILLETED | IC_RECT_USINGFILLETOVERRIDE);
				SDS_CURDOC->m_rRectang_mode &= ~IC_RECT_CHAMFERED ;//turn off chamfer
				SDS_CURDOC->m_rRectang_fill=fill=rb.resval.rreal;
				}
			else if (ret==RTKWORD)
				{
				sds_getinput(fs2);
				if (strisame(fs2,"DEFAULT"/*DNT*/))
					{
					SDS_CURDOC->m_rRectang_mode |= IC_RECT_FILLETED;
					SDS_CURDOC->m_rRectang_mode &= ~IC_RECT_USINGFILLETOVERRIDE;
					SDS_CURDOC->m_rRectang_mode &= ~IC_RECT_CHAMFERED;
					SDS_getvar(NULL,DB_QFILLETRAD,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
					fill=rb.resval.rreal;
					}
				else	//off
					SDS_CURDOC->m_rRectang_mode &= ~IC_RECT_FILLETED;
				}
			else if (ret==RTCAN)
				return(ret);
			if (132==(SDS_CURDOC->m_rRectang_mode&(IC_RECT_FILLETED | IC_RECT_USINGFILLETOVERRIDE)) && icadRealEqual(fill,0.0))
				SDS_CURDOC->m_rRectang_mode&=~4;
		}
	else if (strisame(fs1,"CHAMFER"/*DNT*/))
		{
		if (SDS_CURDOC->m_rRectang_mode & IC_RECT_CHAMFERED)
			{
			sds_rtos(chama,-1,-1,fs1);
			sds_initget(SDS_RSG_NONEG, ResourceString(IDC_RECTANG_CHAMFER_INIT1, "Turn_chamfer_off|Off ~ Use_default_settings|Defaults ~_Off ~_ ~_Defaults"));
			sprintf(fs2,ResourceString(IDC_CMDS3__NOFF___DEFAULTS_248, "\nOff/Defaults/First chamfer distance to use for rectangles <%s> : " ),fs1);
			}
		else
			{
			sds_rtos(0.0,-1,-1,fs1);
			sds_initget(SDS_RSG_NONEG, ResourceString(IDC_RECTANG_DEFAULT_INIT3, "Use_default_settings|Defaults ~_Defaults"));
			sprintf(fs2,ResourceString(IDC_CMDS3__NDEFAULTS___FIR_250, "\nDefaults/First chamfer distance to use for all rectangles <%s> : " ),fs1);
			}
			ret=sds_getdist(NULL,fs2,&rb.resval.rreal);
			if (ret==RTNORM)
				{
				SDS_CURDOC->m_rRectang_mode |= (IC_RECT_CHAMFERED | IC_RECT_USINGCHAMFEROVERRIDE);
				SDS_CURDOC->m_rRectang_mode &= ~IC_RECT_FILLETED;//turn off fillet
				SDS_CURDOC->m_rRectang_chama=chama=rb.resval.rreal;
				}
			else if (ret==RTKWORD)
				{
				sds_getinput(fs2);
				if (strisame(fs2,"DEFAULTS"/*DNT*/))
					{
					SDS_CURDOC->m_rRectang_mode |= IC_RECT_CHAMFERED;
					SDS_CURDOC->m_rRectang_mode &= ~IC_RECT_USINGCHAMFEROVERRIDE;
					SDS_CURDOC->m_rRectang_mode &= ~IC_RECT_FILLETED;
					SDS_getvar(NULL,DB_QCHAMMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
					if (0==rb.resval.rint)
						{
						SDS_getvar(NULL,DB_QCHAMFERA,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
						chama=rb.resval.rreal;
						SDS_getvar(NULL,DB_QCHAMFERB,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
						chamb=rb.resval.rreal;
						}
					else
						{
						SDS_getvar(NULL,DB_QCHAMFERC,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
						chama=rb.resval.rreal;
						SDS_getvar(NULL,DB_QCHAMFERD,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
						chamb=rb.resval.rreal;
						}
					}
				else	//off
					SDS_CURDOC->m_rRectang_mode &= ~IC_RECT_CHAMFERED;
				}
			else if (ret==RTCAN)
				return(ret);
			if ((SDS_CURDOC->m_rRectang_mode & IC_RECT_CHAMFERED) &&
			   (SDS_CURDOC->m_rRectang_mode & IC_RECT_USINGCHAMFEROVERRIDE))
				{
				if (icadRealEqual(chamb,0.0))
					chamb=chama;
				sds_rtos(chamb,-1,-1,fs1);
				sds_initget(SDS_RSG_NONEG,NULL);
				sprintf(fs2,ResourceString(IDC_CMDS3__NSECOND_CHAMFER_252, "\nSecond chamfer distance to use for all rectangles <%s>: " ),fs1);
				ret=sds_getdist(NULL,fs2,&rb.resval.rreal);
				if (RTNORM==ret)
					SDS_CURDOC->m_rRectang_chamb=chamb=rb.resval.rreal;
				else if (ret==RTCAN)
					return(ret);
				}
			if ((SDS_CURDOC->m_rRectang_mode & IC_RECT_CHAMFERED) &&
			   (SDS_CURDOC->m_rRectang_mode & IC_RECT_USINGCHAMFEROVERRIDE) &&
			   (icadRealEqual(chama,0.0) || icadRealEqual(chamb,0.0)))
				SDS_CURDOC->m_rRectang_mode &= ~IC_RECT_CHAMFERED;
			}
		goto firstcorner;
		}
	else if (ret!=RTNORM)
		return(ret);


	//if user picked pt at other than current newelev, correct newelev temporarily so
	//	that dragging is correct
	if (SDS_CURDOC->m_rRectang_mode & IC_RECT_ELEVATIONOVERRIDE)
		{
		if (!icadRealEqual(pt1[2],newelev))
			{
			SDS_getvar(NULL,DB_QCMDECHO,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			if (CMD_CMDECHO_PRINTF(rb.resval.rint))
			   sds_printf(ResourceString(IDC_CMDS3__NUSING_ELEVATIO_253, "\nUsing elevation override." ));
			}
		pt1[2]=newelev;
		}
	else
		{
		if (!icadRealEqual(pt1[2],curelev))
			{
			SDS_CorrectElevation=curelev;
			SDS_ElevationChanged=TRUE;
			newelev=rb.resval.rreal=pt1[2];
			rb.restype=RTREAL;
			sds_setvar("ELEVATION"/*DNT*/,&rb);
			}
		}

	SDS_getvar(NULL,DB_QDRAGMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if (!(SDS_CURDOC->m_rRectang_mode & IC_RECT_SQUARE))
		ret=SDS_dragobject(21,rb.resval.rint,pt1,NULL,0.0,ResourceString(IDC_CMDS3__NOTHER_CORNER_O_254, "\nOther corner of rectangle: " ),NULL,pt3,NULL);
	else	//SDS_CURDOC->m_rRectang_mode==2
		ret=SDS_dragobject(19,rb.resval.rint,pt1,pt1,(sds_real)4.0,ResourceString(IDC_CMDS3__NSECOND_CORNER__255, "\nSecond corner of square: " ),NULL,pt2,NULL);

	if (ret!=RTNORM)
		return(ret);

	rb.restype=RTSHORT;
	rb.resval.rint=1;

	ptd[0]=0.0;
	ptd[1]=0.0;
	ptd[2]=pt1[2];
	sds_trans(ptd,&rb,&rbent,0,ptd);
	SDS_getvar(NULL,DB_QPLINETYPE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	plinetype=rb.resval.rint;
	if (plinetype==0)	// create old-style polyline
		{
		elist=sds_buildlist(RTDXF0,"POLYLINE"/*DNT*/,66,1,10,ptd,39,thick,40,width,41,width,210,rbent.resval.rpoint,70,1,0);
		if (elist==NULL || RTERROR==sds_entmake(elist))
			return(RTERROR);
		sds_relrb(elist);elist=NULL;

		rb.restype=RTSHORT;
		rb.resval.rint=1;
		sds_trans(pt1,&rb,&rbent,0,ptd);

		elist=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,10,ptd,39,thick,40,width,41,width,210,rbent.resval.rpoint,0);
		if (elist==NULL || RTERROR==sds_entmake(elist))
			return(RTERROR);
		sds_relrb(elist);elist=NULL;

		pt2[2]=pt1[2];
		if (!(SDS_CURDOC->m_rRectang_mode & IC_RECT_SQUARE))
			{
			pt2[1]=pt1[1];
			pt2[0]=pt3[0];
			}
		sds_trans(pt2,&rb,&rbent,0,ptd);
		elist=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,10,ptd,39,thick,40,width,41,width,210,rbent.resval.rpoint,0);
		if (elist==NULL || RTERROR==sds_entmake(elist))
			return(RTERROR);
		sds_relrb(elist);elist=NULL;

		pt3[2]=pt1[2];
		if (SDS_CURDOC->m_rRectang_mode&2)
			{
			fr1=sds_angle(pt1,pt2);
			angsin=sin(fr1);
			angcos=cos(fr1);
			fr1=sds_dist2d(pt1,pt2);
			pt3[0]=pt2[0]-angsin*fr1;
			pt3[1]=pt2[1]+angcos*fr1;
			}
		sds_trans(pt3,&rb,&rbent,0,ptd);
		elist=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,10,ptd,39,thick,40,width,41,width,210,rbent.resval.rpoint,0);
		if (elist==NULL || RTERROR==sds_entmake(elist))
			return(RTERROR);
		sds_relrb(elist);elist=NULL;

		if (SDS_CURDOC->m_rRectang_mode & IC_RECT_SQUARE)
			{
			pt3[0]=pt1[0]-angsin*fr1;
			pt3[1]=pt1[1]+angcos*fr1;
			}
		else
			pt3[0]=pt1[0];
		sds_trans(pt3,&rb,&rbent,0,ptd);
		elist=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,10,ptd,39,thick,40,width,41,width,210,rbent.resval.rpoint,0);
		if (elist==NULL || RTERROR==sds_entmake(elist))
			return(RTERROR);
		sds_relrb(elist);elist=NULL;

		elist=sds_buildlist(RTDXF0,"SEQEND"/*DNT*/,210,rbent.resval.rpoint,210,rbent.resval.rpoint,0);
		if (elist==NULL || RTERROR==sds_entmake(elist))
			return(RTERROR);
		sds_relrb(elist);elist=NULL;
		}
	else	// create lightweight polyline
		{
		resbuf *newlwpl,*lwpladdrb;
		newlwpl=lwpladdrb=sds_buildlist(RTDXF0,"LWPOLYLINE"/*DNT*/,43,width,210,rbent.resval.rpoint,90,2,70,1,38,ptd[2],39,thick,0);
		while (lwpladdrb->rbnext!=NULL)
			lwpladdrb=lwpladdrb->rbnext;

		rb.restype=RTSHORT;
		rb.resval.rint=1;
		sds_trans(pt1,&rb,&rbent,0,ptd);

		LwplrbAddVertFromData(&lwpladdrb,ptd,width,width,0.0);
		pt2[2]=pt1[2];
		if (!(SDS_CURDOC->m_rRectang_mode & IC_RECT_SQUARE))
			{
			pt2[1]=pt1[1];
			pt2[0]=pt3[0];
			}
		sds_trans(pt2,&rb,&rbent,0,ptd);
		LwplrbAddVertFromData(&lwpladdrb,ptd,width,width,0.0);
		pt3[2]=pt1[2];
		if (SDS_CURDOC->m_rRectang_mode & 2)
			{
			fr1=sds_angle(pt1,pt2);
			angsin=sin(fr1);
			angcos=cos(fr1);
			fr1=sds_dist2d(pt1,pt2);
			pt3[0]=pt2[0]-angsin*fr1;
			pt3[1]=pt2[1]+angcos*fr1;
			}
		sds_trans(pt3,&rb,&rbent,0,ptd);
		LwplrbAddVertFromData(&lwpladdrb,ptd,width,width,0.0);
		if (SDS_CURDOC->m_rRectang_mode & IC_RECT_SQUARE)
			{
			pt3[0]=pt1[0]-angsin*fr1;
			pt3[1]=pt1[1]+angcos*fr1;
			}
		else
			pt3[0]=pt1[0];
		sds_trans(pt3,&rb,&rbent,0,ptd);
		LwplrbAddVertFromData(&lwpladdrb,ptd,width,width,0.0);
		LwplrbSetNumVerts(newlwpl);
		int ret=sds_entmake(newlwpl);
		sds_relrb(newlwpl);
		if (ret==RTERROR)
			return(RTERROR);
		}


	nverts=4;
	sds_entlast(ename);
	if (!icadRealEqual(fill,0.0) && (SDS_CURDOC->m_rRectang_mode & IC_RECT_FILLETED))
		{
		if (fill<2.0*sds_distance(pt1,pt2) && fill<2.0*sds_distance(pt1,pt3))
			{
			nverts=8;
			//fillet the whole pline
			if (plinetype==0)	// regular pline
				{
				if (RTNORM!=(ret=cmd_fillet_or_chamfer_pline(ename,1,fill,0.0,NULL,NULL)))
				goto exit;
				}
			else				//lightweight pline
				{
				if (RTNORM!=(ret=cmd_fillet_or_chamfer_lwpline(ename,1,fill,0.0,NULL,NULL)))
				goto exit;
				}
			}
		}
	else if (!icadRealEqual(chama,0.0) && !icadRealEqual(chamb,0.0) && (SDS_CURDOC->m_rRectang_mode & IC_RECT_CHAMFERED))
		{
		//chamfer the whole pline
		fi1=3;
		if (!(SDS_CURDOC->m_rRectang_mode & IC_RECT_USINGCHAMFEROVERRIDE))
			{
			SDS_getvar(NULL,DB_QCHAMMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			//if we're using system settings & user wants angle mode...
			if (rb.resval.rint)
				fi1=5;
			}
		//after we know which mode to use, we need to be sure we can chamfer all
		//	four vertices
		if ((3==fi1 &&
		   (chama+chamb)<=sds_distance(pt1,pt3) &&
		   (chama+chamb)<=sds_distance(pt1,pt2))
		   ||
		   (5==fi1 &&
		   (chama*(1+tan(chamb)))<=sds_distance(pt1,pt3) &&
		   (chama*(1+tan(chamb)))<=sds_distance(pt1,pt2)))
			{
			if (plinetype==0)	// regular pline
				{
				if (RTNORM!=(ret=cmd_fillet_or_chamfer_pline(ename,fi1,chama,chamb,NULL,NULL)))
				goto exit;
				}
			else	// lightweight pline
				{
				if (RTNORM!=(ret=cmd_fillet_or_chamfer_lwpline(ename,fi1,chama,chamb,NULL,NULL)))
				goto exit;
				}
			nverts=8;
			}
		}

	if (SDS_CURDOC->m_rRectang_mode & IC_RECT_ROTATED)	//drag the rectang around
		{
		struct cmd_drag_globalpacket *gpak;
		gpak=&cmd_drag_gpak;

		//get the rotation angle for the rectang
		sds_trans(pt1,&rb,&rbent,0,pt1);
		ic_ptcpy(gpak->pt1,pt1);
		ic_ptcpy(gpak->lastpt,pt1);
		gpak->reference=0;
		gpak->refval=0.0;
		gpak->rband=0;
		sds_entlast(ename);
		IC_RELRB(elist);
		sds_rtos(SDS_CURDOC->m_rRectang_ang*180.0/IC_PI,-1,-1,fs1);
		sprintf(fs2,ResourceString(IDC_CMDS3__NROTATION_ANGLE_258, "\nRotation angle for rectangle <%s>: "),fs1);
//needangle:
		sds_initget(0,NULL);
		ret=sds_getangle(pt1,fs2,&fr1); //added  ret=cmd_drag(CMD_ROTATE_DRAG,tempent,fs2,NULL,pt2,fs1);
		if (ret==RTERROR)
			goto exit;
		else if (ret!=RTNORM  && ret!=RTNONE)
			{
			sds_entdel(ename);
			goto exit;
			}
		else
			{
			if (ret==RTNORM)
				SDS_CURDOC->m_rRectang_ang=fr1;
			//else if (ret==RTNONE) leave SDS_CURDOC->m_rRectang_ang unchanged
			angsin=sin(SDS_CURDOC->m_rRectang_ang);
			angcos=cos(SDS_CURDOC->m_rRectang_ang);
			if (plinetype==0)	// regular pline
				{
				sds_entnext_noxref(ename,tempent);	//step to vtx#1
				for(fi1=0; fi1<nverts; fi1++)
					{
					FreeResbufIfNotNull(&elist);
					if ((elist=sds_entget(tempent))==NULL)
						{
						ret=RTERROR;
						goto exit;
						}
					ic_assoc(elist,10);
					ic_ptcpy(pt3,ic_rbassoc->resval.rpoint);
					//pt2[2]=pt1[2];
					pt2[0]=(pt3[0]-pt1[0])*angcos-(pt3[1]-pt1[1])*angsin+pt1[0];
					pt2[1]=(pt3[1]-pt1[1])*angcos+(pt3[0]-pt1[0])*angsin+pt1[1];
					ic_ptcpy(ic_rbassoc->resval.rpoint,pt2);
					if ((ret=sds_entmod(elist))!=RTNORM)
						goto exit;
					sds_entnext_noxref(tempent,tempent);	//next vertex
					}
				sds_entupd(ename);
				}
			else
				{
				if ((elist=sds_entget(ename))==NULL)
					{
					ret=RTERROR;
					goto exit;
					}
				trb=elist;
				while (trb->restype!=90)
					trb=trb->rbnext;
				nverts=trb->resval.rint;
				while (trb->restype!=10)
					trb=trb->rbnext;
				for (i=0; i<nverts; i++)
					{
					ic_ptcpy(pt3,trb->resval.rpoint);
					pt2[0]=(pt3[0]-pt1[0])*angcos-(pt3[1]-pt1[1])*angsin+pt1[0];
					pt2[1]=(pt3[1]-pt1[1])*angcos+(pt3[0]-pt1[0])*angsin+pt1[1];
					ic_ptcpy(trb->resval.rpoint,pt2);
					trb=trb->rbnext->rbnext->rbnext->rbnext;
					}
				if ((ret=sds_entmod(elist))!=RTNORM)
					goto exit;
				sds_entupd(ename);
				}
			}
		}

	exit:

	if (SDS_ElevationChanged)
		{
		rb.restype=RTREAL;
		rb.resval.rreal=curelev;
		sds_setvar("ELEVATION"/*DNT*/,&rb);
		SDS_ElevationChanged=FALSE;
		}

	FreeResbufIfNotNull(&elist);
	return(ret);
}

//==========================================================================================

short cmd_plan(void)
	{
	if (cmd_iswindowopen() == RTERROR)
		{
		return RTERROR;
		}
	if (cmd_isinpspace() == RTERROR)
		{
		return RTERROR;
		}

	//BugZilla No. 78050; 25-03-2002
	if (cmd_istransparent() == RTERROR) return RTERROR;

	struct resbuf rb;
	// If we are in perspective mode, get out
	//
	SDS_getvar(NULL,DB_QVIEWMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if ( 1 & rb.resval.rint)
		{
		sds_point ptextmin,ptextmax;
		int viewmode = rb.resval.rint;

		SDS_getvar(NULL,DB_QEXTMIN,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		ic_ptcpy(ptextmin,rb.resval.rpoint);

		SDS_getvar(NULL,DB_QEXTMAX,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
 		ic_ptcpy(ptextmax,rb.resval.rpoint);
		sds_real rSize = sds_distance(ptextmin,ptextmax);

		SDS_getvar(NULL,DB_QLENSLENGTH,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		sds_real viewdirlen=rb.resval.rreal*rSize/35.0;

		sds_point ptcenter;
		ptcenter[0]=(ptextmin[0]+ptextmax[0])/2.0;
 		ptcenter[1]=(ptextmin[1]+ptextmax[1])/2.0;
		ptcenter[2]=(ptextmin[2]+ptextmax[2])/2.0;

		sds_point viewdir;
		viewdir[0]=viewdir[1]=0.0;
		viewdir[2]=viewdirlen;
		SDS_ZoomIt(NULL,1,ptcenter,&rSize,&viewmode,viewdir,NULL,NULL);
		}

	cmd_vpoint_xyplanang_init=FALSE;//default to 270 as angle in xy plane for viewdir
	return(cmd_view_plan(0));
}

short cmd_view_plan(int getmode){
	//NOTE: askmode 0: prompt for which UCS
	//				1: current UCS
	//				2: WCS
	char fs1[IC_ACADBUF];
	sds_point pt1={0.0,0.0,1.0},pt2={1.0,0.0,0.0},pt3,pt4,pt5,pt6,oldviewdir;
	sds_real fr1,oldviewtwist;
	RT ret=0;
	int	fi1,thisucs=0,vmode;
	struct resbuf rbucs, rbwcs, rb, *rbp1;
	sds_name elast;

	rbp1=NULL;
	rbucs.restype=rbwcs.restype=RTSHORT;
	rbucs.resval.rint=1;
	rbwcs.resval.rint=0;

	SDS_getvar(NULL,DB_QVIEWMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if (rb.resval.rint & 1)rb.resval.rint--;
	vmode=rb.resval.rint;

	ask_again:
	if (0==getmode){
		if (sds_initget(0,ResourceString(IDC_CMDS3_CURRENT_UCS_UCS__259,"Current UCS|Ucs World ~_Current ~_Ucs ~_World"))!=RTNORM)
			return(-2);
		if ((ret=sds_getkword(ResourceString(IDC_CMDS3__NPLAN_VIEW_OF___260, "\nPlan view of:  UCS/World/<Current UCS>: " ),fs1))==RTERROR) {
			return(ret);
		} else if (ret==RTCAN) {
			return(ret);
		}
	}
	if (1==getmode || strisame("CURRENT"/*DNT*/,fs1)|| ret==RTNONE) {
		SDS_getvar(NULL,DB_QUCSXDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		ic_ptcpy(pt2,rb.resval.rpoint);
		thisucs=1;
	}
	if (strisame("UCS"/*DNT*/,fs1)){
		while(1) {
			if (sds_usrbrk()) {
				return(RTCAN);
			}  //check for break
			if (RTNORM!=(ret=sds_getstring(0,ResourceString(IDC_CMDS3__N__TO_LIST____N_263, "\n? to list/<Name of UCS>: " ),fs1)))
				return(RTERROR);
			// This check of the ? character should be fine - German and Japanese both use the same character
			if (*fs1=='?' || strnisame (fs1, "_?", 2)) {
				if ((ret=cmd_ucslister())<0)
					return(RTERROR);
				else if (ret==RTNONE)
					goto ask_again;
			} else if (*fs1==0){
				break;
			}else if (strisame("WORLD"/*DNT*/,fs1)){
				break;
			} else {
				if (rbp1!=NULL){sds_relrb(rbp1);rbp1=NULL;}
			   if ((rbp1=sds_tblsearch("UCS"/*DNT*/,fs1,0))==NULL){
					cmd_ErrorPrompt(CMD_ERR_FINDUCS,0);
				}else{
					//get the base, xdir, & ydir for the requested ucs
					ic_assoc(rbp1,11);
					ic_ptcpy(pt2,ic_rbassoc->resval.rpoint);
					ic_assoc(rbp1,12);
					ic_ptcpy(pt3,ic_rbassoc->resval.rpoint);
					//cross x and y dirs to get z vector for requested ucs in wcs
					pt4[0]=pt2[1]*pt3[2]-pt2[2]*pt3[1];
					pt4[1]=pt2[2]*pt3[0]-pt2[0]*pt3[2];
					pt4[2]=pt2[0]*pt3[1]-pt2[1]*pt3[0];
					if (rbp1!=NULL){sds_relrb(rbp1);rbp1=NULL;}
					if ((rbp1=sds_buildlist(RT3DPOINT,pt4,0))==NULL)
						return(RTERROR);
					//this gives us requested ucs's plan in wcs coordinates
					//now convert it into current ucs coordinates
					sds_trans(pt4,&rbwcs,&rbucs,1,pt1);
					//we'll view to pt1's view point later
				}
				break;
			}
		}
	}

	if (2==getmode || strisame("WORLD"/*DNT*/,fs1)){ //use icmp in case user typed it in loop above
		sds_trans(pt1,&rbwcs,&rbucs,1,pt1);
		pt2[0]=1.0;
		pt2[1]=pt2[2]=0.0;
	}
	//at this point, pt1 now contains the new viewing direction we want (in UCS)
	//	and pt2 contains ucsxdir (in WCS) for ucs to which we'll be plan viewed
	//temporarily change viewdir to desired value
	struct gra_view *view=SDS_CURGRVW;
	SDS_getvar(NULL,DB_QVIEWDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	ic_ptcpy(oldviewdir,rb.resval.rpoint);
	ic_ptcpy(rb.resval.rpoint,pt1);
	SDS_setvar(NULL,DB_QVIEWDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
	gra_initview(SDS_CURDWG,&SDS_CURCFG,&view,NULL);
	//now get angle of x axis in dcs.  Make resbuf for xform
	rbucs.resval.rint=2;
	sds_trans(pt2,&rbwcs,&rbucs,1,pt5);
	rbucs.resval.rint=1;
	if (icadRealEqual(pt5[2],1.0))
		gra_twist4ucs(SDS_CURDWG,&fr1);
	else{
		fr1=atan2(pt5[1],pt5[0]);
	}
	SDS_getvar(NULL,DB_QVIEWTWIST,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	oldviewtwist=rb.resval.rreal;
	fr1=oldviewtwist-fr1;
	ic_normang(&fr1,NULL);
	rb.resval.rreal=fr1;
	SDS_setvar(NULL,DB_QVIEWTWIST,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
	gra_initview(SDS_CURDWG,&SDS_CURCFG,&view,NULL);

	//we need to zoom extents for this view

	if (sds_entlast(elast)==RTERROR)fi1=1;
	else fi1=0;

	int tilemode,cvport;
	if (SDS_getvar(NULL,DB_QTILEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
		return(RTERROR);
	tilemode=rb.resval.rint;
	if (SDS_getvar(NULL,DB_QCVPORT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
		return(RTERROR);
	cvport=rb.resval.rint;

	SDS_getvar(NULL,
		(fi1 ? ((tilemode==0 && cvport==1) ? DB_QP_LIMMIN : DB_QLIMMIN) : ((tilemode==0 && cvport==1) ? DB_QP_EXTMIN : DB_QEXTMIN)),
		  &rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	ic_ptcpy(pt5,rb.resval.rpoint);
	SDS_getvar(NULL,
		(fi1 ? ((tilemode==0 && cvport==1) ? DB_QP_LIMMAX : DB_QLIMMAX) : ((tilemode==0 && cvport==1) ? DB_QP_EXTMAX : DB_QEXTMAX)),
		  &rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	ic_ptcpy(pt6,rb.resval.rpoint);

	pt3[0]=min(pt5[0],pt6[0]);
	pt3[1]=min(pt5[1],pt6[1]);
	pt3[2]=min(pt5[2],pt6[2]);
	pt4[0]=max(pt5[0],pt6[0]);
	pt4[1]=max(pt5[1],pt6[1]);
	pt4[2]=max(pt5[2],pt6[2]);
	if (fi1){
		SDS_getvar(NULL,DB_QELEVATION,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		pt3[2]=pt4[2]=rb.resval.rreal;
	}

	SDS_rpextents(pt3,pt4,pt5,pt6,view);

	pt3[0]=(pt5[0]+pt6[0])/2.0;
	pt3[1]=(pt5[1]+pt6[1])/2.0;
	pt3[2]=0.0;
	gr_rp2ucs(pt3,pt4,view);
	SDS_getvar(NULL,DB_QSCREENSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if (icadRealEqual(pt6[1],pt5[1]) || ((pt6[0]-pt5[0])/(pt6[1]-pt5[1]))>(rb.resval.rpoint[0]/rb.resval.rpoint[1])){
		pt3[0]=((pt6[0]-pt5[0])*rb.resval.rpoint[1])/rb.resval.rpoint[0];
	}else{
		pt3[0]=pt6[1]-pt5[1];
	}
	pt3[0]*=1.02;


	//now set viewdir back to what it was
	rb.restype=RT3DPOINT;
	ic_ptcpy(rb.resval.rpoint,oldviewdir);
	SDS_setvar(NULL,DB_QVIEWDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
	rb.restype=RTREAL;
	rb.resval.rreal=oldviewtwist;
	SDS_setvar(NULL,DB_QVIEWTWIST,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
	gra_initview(SDS_CURDWG,&SDS_CURCFG,&view,NULL);


	//now do our zoom w/correct center,size,etc.
	SDS_ZoomIt(NULL,2,pt4,&pt3[0],&vmode,pt1,&fr1,NULL);
	//if (thisucs)gra_initview(SDS_CURDWG,&SDS_CURCFG,&view,NULL);

	return(RTNORM);
}

//============================================================================================

short cmd_viewlister(void){
//*** This function lists defined views information.
//***
//*** NOTE: This function does not list the views in alphabetical order
//***		like AutoCAD does. ~|
//***
	char fs1[IC_ACADBUF],viewstr[IC_ACADBUF];
	struct resbuf *tmprb=NULL,*prtrb,rbucs,rbwcs,rb;
	RT ret,viewflag=0;;
	int scrollcur, scrollmax;

	rbucs.restype=rbwcs.restype=RTSHORT;
	rbucs.resval.rint=1;
	rbwcs.resval.rint=0;

	for (;;) {		// Comes back for another try if user enters a bogus view name.
		if ((ret=sds_initget(SDS_RSG_OTHER, ResourceString(IDC_VIEWLIST_INIT1, "List_all_views|ALLVIEWS ~_ALLVIEWS")))!=RTNORM){
			return(ret);
		}
		if ((ret=sds_getkword(ResourceString(IDC_CMDS3__NVIEW_S__TO_LIS_267, "\nView(s) to list <List all views>: " ),viewstr))==RTERROR) {
			goto bail;								// This will still accept "*", like AutoCAD
		}else if (ret==RTCAN) {						// but we don't make them type it.
			goto bail;
		}else if (ret==RTNONE) {
			strcpy(viewstr, "*"/*DNT*/);					// Allows default to be "List all views".
		}else if (ret==RTNORM) {
			if (*viewstr!=0) {
				strupr(viewstr);
				if (strisame("ALLVIEWS"/*DNT*/,viewstr))	// Looks to code as if user typed "*" .
					strcpy(viewstr, "*"/*DNT*/);
			} else {
				strcpy(viewstr,"*"/*DNT*/);
			}
		}

		SDS_getvar(NULL,DB_QCMDECHO,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		if (!CMD_CMDECHO_PRINTF(rb.resval.rint))
			return(RTNORM);
		sds_textscr();
		if (SDS_getvar(NULL,DB_QSCRLHIST,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
			return(RTERROR);
		scrollmax=min(SDS_CMainWindow->m_pTextScrn->m_pPromptWnd->m_pPromptWndList->m_nLinesPerPage,rb.resval.rint);
		scrollmax-=3;
		scrollcur=0;

		if ((tmprb=sds_tblnext("VIEW"/*DNT*/,1))==NULL) {	   // If nothing in the table...
			cmd_ErrorPrompt(CMD_ERR_NOSAVEDVIEWS,0);	// tell the user...
			goto bail;									// and bail.
		}
		while(tmprb!=NULL) {
			if (sds_usrbrk()) {
				ret=RTCAN;
				goto bail;
			}
			//*** VIEW names.
			if (ic_assoc(tmprb,2)==0) {
				if (sds_wcmatch(ic_rbassoc->resval.rstring,viewstr)==RTNORM) {
					viewflag=1;
					//	 Get the name of the view
					strncpy(fs1,ic_rbassoc->resval.rstring,sizeof(fs1)-1);
					//*** Get properties of the view and print them
					sds_printf("\n"/*DNT*/);//extra return
					if (cmd_entlister_scrollchk(&scrollcur,scrollmax)!=RTNORM)
						return(RTERROR);
					cmd_prtstr(fs1,ResourceString(IDC_CMDS3_NAME_OF_VIEW__270, "Name of view:" ));
					if (cmd_entlister_scrollchk(&scrollcur,scrollmax)!=RTNORM)
						return(RTERROR);
					for(prtrb=tmprb;prtrb!=NULL;prtrb=prtrb->rbnext){
						if (prtrb->restype==70){
							cmd_prtstr((prtrb->resval.rint&1)?ResourceString(IDC_CMDS3_PAPER_271, "Paper" ):ResourceString(IDC_CMDS3_MODEL_272, "Model" ),ResourceString(IDC_CMDS3_SPACE__273, "Space:" ));
							if (cmd_entlister_scrollchk(&scrollcur,scrollmax)!=RTNORM)
								return(RTERROR);
						}else if (prtrb->restype==40){
							cmd_prtreal(prtrb->resval.rreal,ResourceString(IDC_CMDS3_HEIGHT_OF_VIEW__274, "Height of view:" ));
							if (cmd_entlister_scrollchk(&scrollcur,scrollmax)!=RTNORM)
								return(RTERROR);
						}else if (prtrb->restype==10){
							cmd_prtpt(prtrb->resval.rpoint,ResourceString(IDC_CMDS3_CENTER_OF_VIEW__275, "Center of view:" ));
							if (cmd_entlister_scrollchk(&scrollcur,scrollmax)!=RTNORM)
								return(RTERROR);
						}else if (prtrb->restype==50 && !icadRealEqual(prtrb->resval.rreal,0.0)){
							sds_angtos_absolute(prtrb->resval.rreal,-1,-1,fs1);
							cmd_prtstr(ResourceString(IDC_CMDS3_TWIST_ANGLE_FOR__276, "Twist angle for view:" ),fs1);
							if (cmd_entlister_scrollchk(&scrollcur,scrollmax)!=RTNORM)
								return(RTERROR);
						}else if (prtrb->restype==11){
							sds_trans(prtrb->resval.rpoint,&rbwcs,&rbucs,1,prtrb->resval.rpoint);
							if (!icadRealEqual(prtrb->resval.rpoint[0],0.0) || 
								!icadRealEqual(prtrb->resval.rpoint[1],0.0) ||
								!icadRealEqual(prtrb->resval.rpoint[2],1.0)){
								cmd_prtpt(prtrb->resval.rpoint,ResourceString(IDC_CMDS3_VIEW_DIRECTION_I_277, "View direction in UCS:" ));
								if (cmd_entlister_scrollchk(&scrollcur,scrollmax)!=RTNORM)
									return(RTERROR);
							}
						}
					}
				}
			}
			IC_RELRB(tmprb);
			//*** Do NOT check for NULL here.  NULL is checked in the while().
			tmprb=sds_tblnext("VIEW"/*DNT*/,0);
		}
		if (viewflag){
			break;
		} else {
			cmd_ErrorPrompt(CMD_ERR_FINDVIEW,0);
		}
	}			// end of  for (;;)

bail:
	IC_RELRB(tmprb);
	return(ret);
}

//=========================================================================================

short cmd_viewsave(short windowmode){
	//This f'n saves a view to the current drawing.  If windowmode != 0,
	//the view will be just within 2 selected pts. (the midpoint of the two will be
	//translated into real projection coordinates and saved as viewctr).
	struct resbuf *rbp1=NULL,rb,rbucs,rbwcs;
	char vname[IC_ACADBUF];
	sds_real height,width,lenslength,frontz,backz,viewtwist;
	sds_point viewdir,target,viewctr,p1,p2;
	int viewmode;
	short messermode,psflag=0,rindex;	//NOTE: we'll save all views as model space
	RT ret;
	struct gra_view *view=NULL;
	BOOL   NameExist;

	rbucs.restype=rbwcs.restype=RTSHORT;
	rbucs.resval.rint=1;
	rbwcs.resval.rint=0;

	while(1){
		sds_initget(SDS_RSG_OTHER,NULL);
		if (SDS_CMainWindow->m_bExpInsertState) {
			rindex=0;
			do {
			   NameExist=FALSE;
			   sprintf(vname,"%s%d"/*DNT*/,ResourceString(IDC_CMDS3_NEWVIEW_278, "NewView" ),(++rindex));
			   if ((rbp1=sds_tblsearch("VIEW"/*DNT*/,vname,0))!=NULL) NameExist=TRUE;
			   sds_relrb(rbp1);
			} while(NameExist);
			ret=RTNORM;
		} else {
			//if ((ret=sds_getkword(ResourceString(IDC_CMDS3__NSAVE_VIEW_AS___279, "\nSave view as: " ),vname))==RTERROR) {  
			if ((ret=sds_getstring(1,ResourceString(IDC_CMDS3__NSAVE_VIEW_AS___279, "\nSave view as: " ),vname))==RTERROR ) {			
				return(ret);
			}else if (ret==RTCAN) {
				return(ret);
			}else if (ret==RTNONE) {
				return(ret);
			}
		}
		ic_trim(vname,"be");
		strupr(vname);
		//make sure the user entered valid name
		if (!ic_isvalidname(vname)) {
			cmd_ErrorPrompt(CMD_ERR_ANYNAME,0);
			continue;
		}
		break;
	}

	if ((rbp1=sds_tblsearch("VIEW"/*DNT*/,vname,0))!=NULL){
		messermode=IC_TBLMESSER_MODIFY;
		sds_relrb(rbp1);
		rbp1=NULL;
	}else{
		messermode=IC_TBLMESSER_MAKE;
	}
	if (windowmode){
		//get window points
		if ((ret=sds_getpoint(NULL,ResourceString(IDC_CMDS3__NFIRST_CORNER_O_280, "\nFirst corner of view window: " ),p1))!=RTNORM) {
			goto bail;
		}
		if ((ret=sds_getcorner(p1,ResourceString(IDC_CMDS3__NOPPOSITE_CORNE_281, "\nOpposite corner: " ),p2))!=RTNORM) {
			goto bail;
		}
		//get height, width, and center point of the view.
		viewctr[0]=(p1[0]+p2[0])/2.0;
		viewctr[1]=(p1[1]+p2[1])/2.0;
		viewctr[2]=(p1[2]+p2[2])/2.0;
		height=fabs(p2[1]-p1[1]);
		width =fabs(p2[0]-p1[0]);
	}else{
		SDS_getvar(NULL,DB_QVIEWCTR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		ic_ptcpy(viewctr,rb.resval.rpoint);
		SDS_getvar(NULL,DB_QVIEWSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		height=rb.resval.rreal;
		SDS_getvar(NULL,DB_QSCREENSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		width=height*rb.resval.rpoint[0]/rb.resval.rpoint[1];
	}
	SDS_getvar(NULL,DB_QTILEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if (0==rb.resval.rint){
		SDS_getvar(NULL,DB_QCVPORT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		if (1==rb.resval.rint)psflag=1;
	}

	SDS_getvar(NULL,DB_QVIEWDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	sds_trans(rb.resval.rpoint,&rbucs,&rbwcs,1,viewdir);
	SDS_getvar(NULL,DB_QTARGET,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	sds_trans(rb.resval.rpoint,&rbucs,&rbwcs,1,target);
	SDS_getvar(NULL,DB_QLENSLENGTH,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	lenslength=rb.resval.rreal;
	SDS_getvar(NULL,DB_QFRONTZ,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	frontz=rb.resval.rreal;
	SDS_getvar(NULL,DB_QBACKZ,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	backz=rb.resval.rreal;
	SDS_getvar(NULL,DB_QVIEWTWIST,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	viewtwist=rb.resval.rreal;
	//get viewmode
	SDS_getvar(NULL,DB_QVIEWMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	viewmode=rb.resval.rint;

	//once vars are gathered, project viewctr onto projection plane to get 2D point
	//	we'll store in table...
	view=SDS_CURGRVW;
	if (view==NULL) {
		ret=RTERROR;
		goto bail;
	}
	gra_ucs2rp(viewctr,viewctr,view);

	//now wer're ready to build the linked list to stuff into the table
   if ((rbp1=sds_buildlist(RTDXF0,"VIEW"/*DNT*/,
								2,vname,
							   70,psflag,
							   40,height,
							   41,width,
							   10,viewctr,
							   11,viewdir,
							   12,target,
							   42,lenslength,
							   43,frontz,
							   44,backz,
							   50,viewtwist,
							   71,viewmode,
							   0))==NULL) {
		cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
		ret=RTERROR;
		goto bail;
	}
	ret=SDS_tblmesser(rbp1,messermode,SDS_CURDWG);

bail:
	IC_RELRB(rbp1);
	rbp1=NULL;
	return(ret);
}

//=============================================================================================

short cmd_viewrestore(void) {
	char vname[IC_ACADBUF];
	struct resbuf *tmprb=NULL, rb,rbucs,rbwcs;
	RT ret;
	int pspace=0;
	int vmode;
	sds_real vhigh,frontz,backz,viewtwist;
	sds_point ucscen,rpcen,target,viewdir,viewdirold;
	struct gra_view *view=NULL;
	rb.rbnext=NULL;

	rbucs.restype=rbwcs.restype=RTSHORT;
	rbucs.resval.rint=1;rbwcs.resval.rint=0;

	SDS_getvar(NULL,DB_QTILEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if (rb.resval.rint==0){
		SDS_getvar(NULL,DB_QCVPORT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		if (rb.resval.rint==1) pspace=1;
	}
	SDS_getvar(NULL,DB_QVIEWDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	ic_ptcpy(viewdirold,rb.resval.rpoint);

	while(1) {
		if (sds_usrbrk()) {
			ret=RTCAN;
			goto bail;
		}  //check for break by user
		//get the name
		//if ((ret=sds_getstring(0,ResourceString(IDC_CMDS3__NVIEW_TO_RESTOR_282, "\nView to restore: " ),vname))!=RTNORM) {
		if ((ret=sds_getstring(1,ResourceString(IDC_CMDS3__NVIEW_TO_RESTOR_282, "\nView to restore: " ),vname))!=RTNORM) {
 	   		goto bail;
 	   	}
 	   	ic_trim(vname,"be");
 	   	strupr(vname);

		//check if there is such a view
		if ((tmprb=sds_tblsearch("VIEW"/*DNT*/,vname,1))==NULL) {	  //if nothing in the table
			sds_printf(ResourceString(IDC_CMDS3__NCAN_NOT_FIND_V_283, "\nCannot find view %s\n" ),vname);
		}else{
			break;
		}
	}

	if (ic_assoc(tmprb,2)==0) {
		//*** Get properties of the view and print them
		if (ic_assoc(tmprb,70)!=0) {
			ret=RTERROR;
			goto bail;
		}
		if ((ic_rbassoc->resval.rint&1) && !pspace){
			cmd_ErrorPrompt(CMD_ERR_NOPAPERVIEW,0);
			goto bail;
		}
		if (!(ic_rbassoc->resval.rint&1) && pspace){
			cmd_ErrorPrompt(CMD_ERR_NOMODELVIEW,0);
			goto bail;
		}
		if (ic_assoc(tmprb,71)!=0) {
			ret=RTERROR;
			goto bail;
		}
		vmode=ic_rbassoc->resval.rint;
		if (ic_assoc(tmprb,43)!=0) {
			ret=RTERROR;
			goto bail;
		}
		frontz=ic_rbassoc->resval.rreal;
		if (ic_assoc(tmprb,44)!=0) {
			ret=RTERROR;
			goto bail;
		}
		backz=ic_rbassoc->resval.rreal;
		if (ic_assoc(tmprb,11)!=0) {
			ret=RTERROR;
			goto bail;
		}
		sds_trans(ic_rbassoc->resval.rpoint,&rbwcs,&rbucs,1,viewdir);
		//set real values not used by Zoomit
		rb.restype=RTREAL;
		if (ic_assoc(tmprb,42)!=0) {
			ret=RTERROR;
			goto bail;
		}
		rb.resval.rreal=ic_rbassoc->resval.rreal;
		sds_setvar("LENSLENGTH"/*DNT*/,&rb);
		if (ic_assoc(tmprb,50)!=0) {
			ret=RTERROR;
			goto bail;
		}
		rb.resval.rreal=ic_rbassoc->resval.rreal;
		sds_setvar("VIEWTWIST"/*DNT*/,&rb);
		viewtwist=rb.resval.rreal;
		//set point values not used by Zoomit
   		if (ic_assoc(tmprb,12)!=0) {
			ret=RTERROR;
			goto bail;
		}
		sds_trans(ic_rbassoc->resval.rpoint,&rbwcs,&rbucs,1,target);

		//Make sure this view is stored in the structure for zooming previous, so
		//we need to use the SDS_Zoomit f'n.  To use this, we need a UCS point, not a
		//pt in the projection plane like it's stored in the table.  We'll do a reverse
		//projection into UCS and pass that and the height to Zoomit.
		view=SDS_CURGRVW;
		if (view==NULL) {
			ret=RTERROR;
			goto bail;
		}
	  	if (ic_assoc(tmprb,10)!=0) {
			ret=RTERROR;
			goto bail;
		}
		ic_ptcpy(rpcen,ic_rbassoc->resval.rpoint);

		//set the new viewdir & init the view structure
		rb.restype=RT3DPOINT;
		ic_ptcpy(rb.resval.rpoint,viewdir);
		sdsengine_setvar("VIEWDIR"/*DNT*/,&rb);
		gra_initview(SDS_CURDWG,&SDS_CURCFG,&view,NULL);
		//now we can translate rpcen to ucscen according to new view
		gra_rp2ucs(rpcen,ucscen,view);
		//now set viewdir back to what it was so we can undo everything
		//	in one step
		ic_ptcpy(rb.resval.rpoint,viewdirold);
		sdsengine_setvar("VIEWDIR"/*DNT*/,&rb);
		gra_initview(SDS_CURDWG,&SDS_CURCFG,&view,NULL);

		if (ic_assoc(tmprb,40)!=0) {
			ret=RTERROR;
			goto bail;
		}
		vhigh=ic_rbassoc->resval.rreal;
		if (vmode&1){//perspective
			rb.restype=RT3DPOINT;
			ic_ptcpy(rb.resval.rpoint,rpcen);
			sdsengine_setvar("VIEWCTR"/*DNT*/,&rb);
			if (SDS_ZoomIt(NULL,1,target,&vhigh,&vmode,viewdir,&frontz,&backz)!=RTNORM)
				return(RTERROR);
		}else{
			rb.restype=RT3DPOINT;
			ic_ptcpy(rb.resval.rpoint,target);
			sds_setvar("TARGET"/*DNT*/,&rb);
			if (SDS_ZoomIt(NULL,2,ucscen,&vhigh,&vmode,viewdir,&viewtwist,&backz)!=RTNORM)
				return(RTERROR);
		}

	}
bail:
	IC_RELRB(tmprb);
	return(ret);
}

//=============================================================================================
short cmd_viewdelete(void) {
	char vname[IC_ACADBUF];
	struct resbuf *tmprb=NULL,rb;
	RT ret;
	int ndel;

	if ((tmprb=sds_tblnext("VIEW"/*DNT*/,1))==NULL) {	   // If there are no views in the drawing...
		cmd_ErrorPrompt(CMD_ERR_NOSAVEDVIEWS,0);	// tell the user right away...
		goto bail;									// and bail.
	}

		// Get the name of the view to be deleted...
	if ((ret=sds_initget(SDS_RSG_OTHER,""/*DNT*/))!=RTNORM){
		return(ret);
	}
	//ret=sds_getkword(ResourceString(IDC_CMDS3__NVIEW_NAMES_TO__289, "\nView names to be deleted: " ),vname);
	ret=sds_getstring(1,ResourceString(IDC_CMDS3__NVIEW_NAMES_TO__289, "\nView names to be deleted: " ),vname);
 	if (ret!=RTNORM || !*vname) {
 		goto bail;
 	}
   	ic_trim(vname,"be");
   	strupr(vname);
	ndel=0;
	do{
   		if (0==ic_assoc(tmprb,2) && RTNORM==sds_wcmatch(ic_rbassoc->resval.rstring,vname)) {
			SDS_tblmesser(tmprb,IC_TBLMESSER_DELETE,SDS_CURDWG);
			ndel++;
		}
		sds_relrb(tmprb);tmprb=NULL;
		tmprb=sds_tblnext("VIEW"/*DNT*/,0);
	}while(tmprb!=NULL);

	SDS_getvar(NULL,DB_QCMDECHO,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if (CMD_CMDECHO_PRINTF(rb.resval.rint)) {
		sds_printf(ResourceString(IDC_CMDS3__N_I__S_DELETED__290, "\n%i %s deleted." ),ndel,(ndel==1) ?
			ResourceString(IDC_CMDS3_VIEW_291, "view" ) :
			ResourceString(IDC_CMDS3_VIEWS_292, "views" ));
	}

bail:
	IC_RELRB(tmprb);
	return(ret);
}

//=============================================================================================

short cmd_view(void) {
	if (cmd_iswindowopen() == RTERROR)
		return RTERROR;

	//BugZilla No. 78050; 25-03-2002
	if (cmd_istransparent() == RTERROR) return RTERROR;

	char fs1[IC_ACADBUF];
	RT ret;

	while(1) {
		if (sds_usrbrk()) {
			return(RTCAN);
		} //check for break
		if (SDS_CMainWindow->m_bExpInsertState) {
			if ((ret=sds_initget(0, ResourceString(IDC_CMDS3_LIST_VIEWS_____S_293, "List_views|? ~ Save Window ~_? ~_ ~_Save ~_Window")))!=RTNORM){
				return(ret);
			}
			if ((ret=sds_getkword(ResourceString(IDC_CMDS3__NVIEW_____TO_LI_294, "\nView:  ? to list saved views/Save/Window: " ),fs1))==RTERROR) {
				return(ret);
			} else if (ret==RTCAN) {
				return(ret);
			} else if (ret==RTNONE) {
				return(ret);
			}
		} else {
			if ((ret=sds_initget(0, ResourceString(IDC_CMDS3_LIST_VIEWS_____D_295,"List_views|? ~ Delete Restore Save Window ~_? ~_ ~_Delete ~_Restore ~_Save ~_Window")))!=RTNORM){
				return(ret);
			}
			if ((ret=sds_getkword(ResourceString(IDC_CMDS3__NVIEW_____TO_LI_296, "\nView:  ? to list saved views/Delete/Restore/Save/Window: " ),fs1))==RTERROR) {
				return(ret);
			} else if (ret==RTCAN) {
				return(ret);
			} else if (ret==RTNONE) {
				return(ret);
			}
		}

		if (strisame("?"/*DNT*/,fs1)) {
			return(cmd_viewlister());
		}
		if (strisame("DELETE"/*DNT*/,fs1)) {
			return(cmd_viewdelete());
		}
		if (strisame("SAVE"/*DNT*/,fs1)) {
			return(cmd_viewsave(0));
		}
		if (strisame("WINDOW"/*DNT*/,fs1)) {
			return(cmd_viewsave(1));
		}
		if (strisame("RESTORE"/*DNT*/,fs1)) {
			return(cmd_viewrestore());
		}
	}
	return(RTNORM);
}

//===========================================================================================

short cmd_setonoff(char *varname) {

	char			prompt[IC_ACADBUF];
	char			response[IC_ACADBUF];
	struct resbuf	rb1;
	short			onoroff;

	sds_getvar(varname,&rb1);
	onoroff=rb1.resval.rint;


	sds_initget(SDS_RSG_NODOCCHG, ResourceString(IDC_SETONOFF_INIT1, "On|ON Off|OFf Toggle ~0 ~1 ~_ON ~_OFf ~_Toggle ~_0 ~_1"));
	if (onoroff == 1) {
		sprintf(prompt,ResourceString(IDC_CMDS3__N_S_IS_CURRENTL_302, "\n%s is currently on:  OFF/Toggle/<On>: " ),varname);
		sds_getkword(prompt,response);
	} else {
		sprintf(prompt,ResourceString(IDC_CMDS3__N_S_IS_CURRENTL_303, "\n%s is currently off:  ON/Toggle/<Off>: " ),varname);
		sds_getkword(prompt,response);
	}
  if (strisame(response,"TOGGLE"/*DNT*/)) {
		if (onoroff==0) rb1.resval.rint=1;
		else rb1.resval.rint=0;
		sds_setvar(varname,&rb1);
  } else if ( (strisame(response,"ON"/*DNT*/)) || (strisame(response,"1"/*DNT*/)) ) {
		rb1.resval.rint=1;
		sds_setvar(varname,&rb1);
  } else if ( (strisame(response,"OFF"/*DNT*/)) || (strisame(response,"0"/*DNT*/)) ) {
		rb1.resval.rint=0;
		sds_setvar(varname,&rb1);
	}
	return(0);	// A null response leave it as it was.
}

//=============================================================================================
short cmd_color(void){
	struct resbuf rb;
	RT ret;

	if (RTNORM==(ret=cmd_color_fromuser(&rb,0))){
		sds_setvar("CECOLOR"/*DNT*/,&rb);
		if (rb.resval.rstring) {IC_FREE(rb.resval.rstring);rb.resval.rstring=NULL;}
	}
	return(ret);
}


short cmd_color_fromuser(struct resbuf *rb, int intmode) {
	//if intmode, resbuf->restype set to 62 and resbuf->resval.rint to color number.  This is
	//	used for an entmake or sds_buildlist
	//if intmode==0, resbuf->restype set to RTSTR, and ->resval to string which caller must free - this is
	//	used for color command;
	if (cmd_iswindowopen() == RTERROR)
		return RTERROR;

 	struct resbuf rb1;
	RT answ;
	TCHAR jk[IC_ACADBUF];
	char disp[IC_ACADBUF+44];
	char dispval[IC_ACADBUF];
	char curcolor[IC_ACADBUF];
	int intval;
	RT response=-1;

	SDS_getvar(NULL,DB_QCECOLOR,&rb1,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	strcpy(curcolor,rb1.resval.rstring);
	IC_FREE(rb1.resval.rstring);
	intval=atoi(curcolor);
	switch (intval) {
		case 1 : strcpy(dispval,ResourceString(IDC_CMDS3_1__RED__308, "1 (Red)" )); 	break;
		case 2 : strcpy(dispval,ResourceString(IDC_CMDS3_2__YELLOW__309, "2 (Yellow)" ));  break;
		case 3 : strcpy(dispval,ResourceString(IDC_CMDS3_3__GREEN__310, "3 (Green)" ));   break;
		case 4 : strcpy(dispval,ResourceString(IDC_CMDS3_4__CYAN__311, "4 (Cyan)" ));	 break;
		case 5 : strcpy(dispval,ResourceString(IDC_CMDS3_5__BLUE__312, "5 (Blue)" ));	 break;
		case 6 : strcpy(dispval,ResourceString(IDC_CMDS3_6__MAGENTA__313, "6 (Magenta)" )); break;
		case 7 : strcpy(dispval,ResourceString(IDC_CMDS3_7__WHITE__314, "7 (White)" ));   break;
		default: strcpy(dispval,curcolor);
	}

	do{
		sprintf(disp,ResourceString(IDC_CMDS3__NCOLOR_FOR_DRAW_315, "\nColor for drawing new entities <%s>: " ),dispval);

		sds_initget(SDS_RSG_OTHER, ResourceString(IDC_COLOR_INIT1, "BYLAYER|BYLayer BYBLOCK|BYBlock ~ Red Yellow Green Cyan Blue Magenta White ~_BYLayer ~_BYBlock ~_ ~_Red ~_Yellow ~_Green ~_Cyan ~_Blue ~_Magenta ~_White"));
		answ=sds_getkword(disp,jk);
		if (answ==RTNONE){
			if (intmode==0) {
				return(RTNONE);
			}else{
				response=intval;
			}
		}else if (answ == RTCAN) {
			return(RTCAN);
		}else{
			if (!ic_isvalidcol(jk)) {
				cmd_ErrorPrompt(CMD_ERR_COLOR0256,0);
				continue;
			}
			if	  (strisame("RED"/*DNT*/,jk))
				response = 1;
			else if (strisame("YELLOW"/*DNT*/,jk))
				response = 2;
			else if (strisame("GREEN"/*DNT*/,jk) )
				response = 3;
			else if (strisame("CYAN"/*DNT*/,jk) )
				response = 4;
			else if (strisame("BLUE"/*DNT*/,jk) )
				response = 5;
			else if (strisame("MAGENTA"/*DNT*/,jk))
				response = 6;
			else if (strisame("WHITE"/*DNT*/,jk) )
				response = 7;
			else if (strisame("BYLAYER"/*DNT*/,jk))
				response = 256;
			else if (strisame("BYBLOCK"/*DNT*/,jk))
				response = 0;
			else response=atoi(jk);
		}
	}while(response<0 || response>256);
	strupr(jk);

	if (0==intmode){
		rb->restype=RTSTR;
		rb->resval.rstring= new char [(*jk == '_') ? strlen(jk) : strlen(jk) + 1];
		strcpy(rb->resval.rstring,(*jk == '_') ? jk + (1 * sizeof(TCHAR)) : jk);
	}else{
		rb->restype=62;
		rb->resval.rint=response;
	}
	return(RTNORM);
}

//========================================================================================

short cmd_base(void) {

	if (cmd_iswindowopen() == RTERROR)
		return RTERROR;

	//NOTE: keep insbase as a wcs point, but user always sees it as ucs pt

	sds_real xval;
	sds_real yval;
	sds_real zval;
	sds_point newpt;
	struct resbuf rb1,rbucs,rbwcs;
	RT answ;
	char disp[IC_ACADBUF+44];
	char dispval[IC_ACADBUF];

	rbucs.restype=rbwcs.restype=RTSHORT;
	rbucs.resval.rint=1;
	rbwcs.resval.rint=0;

	SDS_getvar(NULL,DB_QINSBASE,&rb1,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	xval=rb1.resval.rpoint[0];
	yval=rb1.resval.rpoint[1];
	zval=rb1.resval.rpoint[2];
	strcpy(disp,ResourceString(IDC_CMDS3__NBASE_POINT_FOR_326, "\nBase point for inserting this drawing <" ));
	sds_rtos(xval,-1,-1,dispval);
	strcat(disp,dispval);
   	strcat(disp,","/*DNT*/);
	sds_rtos(yval,-1,-1,dispval);
	strcat(disp,dispval);
	strcat(disp,","/*DNT*/);
	sds_rtos(zval,-1,-1,dispval);
	strcat(disp,dispval);
	strcat(disp,">: "/*DNT*/);
	answ=sds_getpoint(NULL,disp,newpt); /* Keep an eye on this one */
	if (answ == RTCAN)
		return(RTCAN);
	if (answ==RTNORM) {
		rb1.restype=RT3DPOINT;
		ic_ptcpy(rb1.resval.rpoint,newpt);
		sds_setvar("INSBASE"/*DNT*/,&rb1);
	}
	return(RTNORM);
}

//===========================================================================================

short cmd_aperture(void) {
	int response;
	struct resbuf rb1;
	RT ret;
	char disp[IC_ACADBUF];

	SDS_getvar(NULL,DB_QAPERTURE,&rb1,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	for(;;) {
		sprintf(disp,ResourceString(IDC_CMDS3__NSELECTION_AREA_329, "\nSelection area for entity snap (1-50 pixels) <%i>: " ),rb1.resval.rint);
		sds_initget(SDS_RSG_NOZERO|SDS_RSG_NONEG,NULL);
		ret=sds_getint(disp,&response);
		if (ret==RTNORM) {
			if (response<1 || response>50) {
				cmd_ErrorPrompt(CMD_ERR_SNAPTARGET,0);
				continue;
			} else {
				rb1.resval.rint=response;
				sds_setvar("APERTURE"/*DNT*/,&rb1);
				break;
			}
		} else break;
	}
	return(ret);
}

//==========================================================================================

short cmd_fill(void) {
	if (cmd_iswindowopen() == RTERROR)
		return RTERROR;
	cmd_setonoff("FILLMODE"/*DNT*/);
	return(RTNORM);
}

short cmd_qtext(void) {
	if (cmd_iswindowopen() == RTERROR)
		return RTERROR;

	//BugZilla No. 78050; 25-03-2002
	if (cmd_istransparent() == RTERROR) return RTERROR;

	cmd_setonoff("QTEXTMODE"/*DNT*/);
	return(RTNORM);
}

short cmd_blipmode(void) {
	if (cmd_iswindowopen() == RTERROR)
		return RTERROR;
   	cmd_setonoff("BLIPMODE"/*DNT*/);
	return(RTNORM);
}

short cmd_regenauto(void) {
	if (cmd_iswindowopen() == RTERROR)
		return RTERROR;
   	cmd_setonoff("REGENMODE"/*DNT*/);
	return(RTNORM);
}

short cmd_ortho(void) {
	if (cmd_iswindowopen() == RTERROR)
		return RTERROR;
   	cmd_setonoff("ORTHOMODE"/*DNT*/);
	return(RTNORM);
}

short cmd_scrlbar(void) {
   	cmd_setonoff("WNDLSCRL"/*DNT*/);
//	SDS_CMainWindowScrollBarsOn();	reset scrollbar state
	return(RTNORM);
}

short cmd_statbar(void) {
   	cmd_setonoff("WNDLSTAT"/*DNT*/);
	return(RTNORM);
}

short cmd_cmdbar(void) {

	short state = 0;						// The current state of the command bar
	static short beforehide;				// The state before it was hidden last
	struct resbuf rb1;
	char response[IC_ACADBUF];
	RT ret = 5100;

	SDS_getvar(NULL,DB_QWNDLCMD,&rb1,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);				// Values are 0-3:	0 is Hidden, 1 is Floating
	state=rb1.resval.rint;					// 2 is Docked Upper, and 3 is Docked Lower.
	if (state!=0)	   beforehide=state;	// Set beforehide to the current state, if it isn't hidden already.
	if (beforehide==0) beforehide=2;		// If it hasn't been displayed yet this session, it will be docked on top when user "Toggles".

	if (state==0) {
		(ret=sds_initget(0,ResourceString(IDC_CMDS3_SHOW_COMMAND_BAR_337,"Show_Command_Bar|Show ~Hide ~Toggle ~ Floating Dock_-_Lower|Lower Dock_-_Upper|Upper ~_Show ~_Hide ~_Toggle ~_ ~_Floating ~_Lower ~_Upper")));
	} else {
		(ret=sds_initget(0,ResourceString(IDC_CMDS3_HIDE_COMMAND_BAR_338,"Hide_Command_Bar|Hide ~Show ~Toggle ~ Floating Dock_-_Lower|Lower Dock_-_Upper|Upper ~_Hide ~_Show ~_Toggle ~_ ~_Floating ~_Lower ~_Upper")));
	}
	if (ret!=RTNORM)
		return(ret);
	if (state==0)
		(ret=sds_getkword(ResourceString(IDC_CMDS3__NFLOATING___LOW_339, "\nFloating/Lower/Upper/<Show Command Bar>: " ), response));
	else
		(ret=sds_getkword(ResourceString(IDC_CMDS3__NFLOATING___LOW_340, "\nFloating/Lower/Upper/<Hide Command Bar>: " ), response));
	if (ret!=RTNORM && ret!=RTNONE)
		return(ret);

	rb1.restype=RTSHORT;					// Whatever the response, it's going to be a short.
	if (ret==RTNONE || (strisame(response,"TOGGLE"/*DNT*/)))
	{
		if (state==0){
			rb1.resval.rint=beforehide;		// If it's hidden, change to what it was before hiding it.
		}else {
			rb1.resval.rint=0;				// If it's visible, hide it.
		}
	}
	else if ((strisame(response,"HIDE"/*DNT*/)))
		rb1.resval.rint=0;
	else if ((strisame(response,"FLOATING"/*DNT*/)))
		rb1.resval.rint=1;
	else if ((strisame(response,"UPPER"/*DNT*/)))
		rb1.resval.rint=2;
	else if ((strisame(response,"LOWER"/*DNT*/)))
		rb1.resval.rint=3;
	else if ((strisame(response,"SHOW"/*DNT*/)))
			rb1.resval.rint=beforehide;		// If it's hidden, change to what it was before hiding it.
	sds_setvar("WNDLCMD"/*DNT*/,&rb1);

	return(RTNORM);
}

//=============================================================================================

short cmd_snap(void) {					// TODO - Handle redrawing the grid better - it's handled just a little right at the bottom.

	if (cmd_iswindowopen() == RTERROR)
		return RTERROR;

	char response[IC_ACADBUF];
	char prompt[IC_ACADBUF];
	char initval[IC_ACADBUF];
	char oastr[IC_ACADBUF];

	char disp[IC_ACADBUF];				// This is where the prompt gets stored
	char xspacing[50];					// The horizontal spacing that gets displayed in the command line
	char yspacing[50];					// The vertical spacing that gets displayed in the command line
	char xbasestr[50];					// The x component of the snap grid base point
	char ybasestr[50];					// The y component of the snap grid base point

	struct resbuf rb1;

	short oldstyle;						// 0 is standard, 1 is isometric
	short newstyle;

	short mode = 0;						// 0 is off, 1 is on

	int   answ;
	short aspectflag;					// 0 is x=y, 1 is x!=y, 2 is isometric
	sds_real oldang;
	sds_real newang;
	sds_real xbase;
	sds_real ybase;
	sds_real horiz;
	sds_real vert;
	sds_real horiznew;
	sds_real vertnew;
	sds_point newpt;


	SDS_getvar(NULL,DB_QSNAPANG,&rb1,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	newang=oldang=rb1.resval.rreal;

	SDS_getvar(NULL,DB_QSNAPSTYL,&rb1,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);		// 0 = Standard, 1 = Isometric
	newstyle=oldstyle=rb1.resval.rint;

	SDS_getvar(NULL,DB_QSNAPMODE,&rb1,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);		// 0 = Off, 1 = On
	mode=rb1.resval.rint;

	SDS_getvar(NULL,DB_QSNAPUNIT,&rb1,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);		// A 2D point giving the x and y grid spacing, e.g. (2,3)
	horiznew=horiz=rb1.resval.rpoint[0];
	vertnew=vert=rb1.resval.rpoint[1];

	if (mode == 0) {											// Snap is off...

		if (oldstyle == 1) {									// Isometric snap
			aspectflag=2;										//	  so aspect doesn't apply
			sds_rtos(vert,-1,-1,yspacing);
			sprintf(disp,ResourceString(IDC_CMDS3__NSNAP_IS_OFF__I_346, "\nSnap is off (isometric, y = %s):  ON/Rotate/Style/<Spacing in y direction>: " ),yspacing);
			sds_initget(SDS_RSG_NONEG|SDS_RSG_NOZERO, ResourceString(IDC_SNAP_INIT1, "Snap_on|ON ~OFf ~Toggle ~ Rotate Snap_style|Style ~_ON ~_OFf ~_Toggle ~_ ~_Rotate ~_Style"));	// OK to forbid zero here, but not in Grid, where 0=synced with Snap
			answ = sds_getdist(NULL,disp,&vertnew);
			sds_getinput(response);

		} else {
			if (horiz != vert) {								// x and y are different
				aspectflag=1;
				sds_rtos(horiz,-1,-1,xspacing);
				sds_rtos(vert,-1,-1,yspacing);
				sprintf(disp,ResourceString(IDC_CMDS3__NSNAP_IS_OFF__X_348, "\nSnap is off (x = %s, y = %s):  ON/Rotate/Style/Equal/<Aspect>: " ),xspacing,yspacing);
				sds_initget(0, ResourceString(IDC_SNAP_INIT2, "Snap_on|ON ~OFf ~Toggle ~ Rotate Snap_style|Style ~ Equal_spacing|Equal Aspect ~_ON ~_OFf ~_Toggle ~_ ~_Rotate ~_Style ~_ ~_Equal ~_Aspect"));
				answ = sds_getkword(disp,response);
				if (answ==RTNONE)
					strcpy(response,"ASPECT"/*DNT*/);	// ENTER defaults to Aspect
				vertnew = vert;
				if ((strisame(response,"EQUAL"/*DNT*/))) {			// If setting back to Equal spacing, drops right into next if, below
					horiznew=horiz=vertnew=vert;
				}
			}

			if	(horiz==vert) {									// x and y are the same
				aspectflag=0;
				sds_rtos(vert,-1,-1,yspacing);
				sprintf(disp,ResourceString(IDC_CMDS3__NSNAP_IS_OFF__X_352, "\nSnap is off (x and y = %s):  ON/Rotate/Style/Aspect/<Snap spacing>: " ),yspacing);
				sds_initget(SDS_RSG_NONEG|SDS_RSG_NOZERO, ResourceString(IDC_SNAP_INIT3, "Snap_on|ON ~OFf ~Toggle ~ Rotate Snap_style|Style Aspect ~_ON ~_OFf ~_Toggle ~_ ~_Rotate ~_Style ~_Aspect"));
				answ = sds_getdist(NULL,disp,&vertnew);
				sds_getinput(response);
			}
		}

	} else {													// Snap is on...

		if (oldstyle == 1) {									// Isometric snap
			aspectflag=2;										//	  so aspect doesn't apply
			sds_rtos(vert,-1,-1,yspacing);
			sprintf(disp,ResourceString(IDC_CMDS3__NSNAP_IS_ON__IS_354, "\nSnap is on, isometric, y = %s:  OFF/Rotate/Style/<Spacing in y direction>: " ),yspacing);
			sds_initget(SDS_RSG_NONEG|SDS_RSG_NOZERO,ResourceString(IDC_CMDS3__ON_SNAP_OFF_OFF_355, "~ON Snap_off|OFf ~Toggle ~ Rotate Snap_style|Style ~_ON ~_OFf ~_Toggle ~_ ~_Rotate ~_Style" ));	// OK to forbid zero here, but not in Grid, where 0=synced with Snap
			answ = sds_getdist(NULL,disp,&vertnew);
			sds_getinput(response);

		} else {
			if (horiz != vert) {								// x and y are different
				aspectflag=1;
				sds_rtos(horiz,-1,-1,xspacing);
				sds_rtos(vert,-1,-1,yspacing);
				sprintf(disp,ResourceString(IDC_CMDS3__NSNAP_IS_ON__X__356, "\nSnap is on, x = %s, y = %s:  OFF/Rotate/Style/Equal/<Aspect>: " ),xspacing,yspacing);
				sds_initget(0, ResourceString(IDC_SNAP_INIT4, "~ON Snap_off|OFf ~Toggle ~ Rotate Snap_style|Style ~ Equal_spacing|Equal Aspect ~_ON ~_OFf ~_Toggle ~_ ~_Rotate ~_Style ~_ ~_Equal ~_Aspect"));
				answ = sds_getkword(disp,response);
				if (answ==RTNONE)
					strcpy(response,"ASPECT"/*DNT*/);	// ENTER defaults to Aspect
				vertnew = vert;
				if ((strisame(response,"EQUAL"/*DNT*/))) {			// If setting back to Equal spacing, drops right into next if, below
					horiznew=horiz=vertnew=vert;
				}
			}

			if (horiz==vert) {									// x and y are the same
			aspectflag=0;
			sds_rtos(vert,-1,-1,yspacing);
			sprintf(disp,ResourceString(IDC_CMDS3__NSNAP_IS_ON__X__358, "\nSnap is on, x and y = %s:  OFF/Rotate/Style/Aspect/<Snap spacing>: " ),yspacing);
			sds_initget(SDS_RSG_NONEG|SDS_RSG_NOZERO, ResourceString(IDC_SNAP_INIT5, "~ON Snap_off|OFf ~Toggle ~ Rotate Snap_style|Style Aspect ~_ON ~_OFf ~_Toggle ~_ ~_Rotate ~_Style ~_Aspect"));
			answ = sds_getdist(NULL,disp,&vertnew);
			sds_getinput(response);
			}
		}
	}


	if (answ == RTCAN)
		return (-1);
	else {

		if ((answ==RTNORM) && (aspectflag==0)) {			// ** User entered a number **	Only do this if x and y are the same
			horiznew=vertnew;											// Sets horizontal = "spacing" that user entered
			rb1.restype=RTPOINT;
			rb1.resval.rpoint[0]=horiznew;
			rb1.resval.rpoint[1]=vertnew;
			rb1.resval.rpoint[2]=0;
			sds_setvar("SNAPUNIT"/*DNT*/,&rb1);
			rb1.restype=RTSHORT;										// Turns Snap ON
			rb1.resval.rint=1;
			sds_setvar("SNAPMODE"/*DNT*/,&rb1);
		} else	if ((answ==RTNORM) && (aspectflag==2)) {	// ** User entered a number **	Only do this if grid is isometric
			horiznew = vertnew * (sqrt(3.0));
			rb1.restype=RTPOINT;
			rb1.resval.rpoint[0]=horiznew;
			rb1.resval.rpoint[1]=vertnew;
			rb1.resval.rpoint[2]=0;
			sds_setvar("SNAPUNIT"/*DNT*/,&rb1);
			rb1.restype=RTSHORT;										// Turns Snap ON
			rb1.resval.rint=1;
			sds_setvar("SNAPMODE"/*DNT*/,&rb1);
		}

		if ((strisame(response,"ON"/*DNT*/))) {	// Turns Snap ON
			rb1.restype=RTSHORT;
			rb1.resval.rint=1;
			sds_setvar("SNAPMODE"/*DNT*/,&rb1);

		} else if ((strisame(response,"OFF"/*DNT*/))) {		// ** OFF **  Turns Snap OFF
			rb1.restype=RTSHORT;
			rb1.resval.rint=0;
			sds_setvar("SNAPMODE"/*DNT*/,&rb1);

		} else if ((strisame(response,"TOGGLE"/*DNT*/))) {		// ** Toggle **  Turns Snap ON or OFF, depending on what it was before
			rb1.restype=RTSHORT;
			if (mode==0) rb1.resval.rint=1;
				else rb1.resval.rint=0;
			sds_setvar("SNAPMODE"/*DNT*/,&rb1);

		} else if ((strisame(response,"ASPECT"/*DNT*/))){		// ** Aspect **  Turns Snap ON, gets x and y spacing

			sds_rtos(horiz,-1,-1,xspacing); 							// Gets horizontal spacing
			sprintf(disp,ResourceString(IDC_CMDS3__NHORIZONTAL_SPA_362, "\nHorizontal spacing <%s>: " ),xspacing);
			sds_initget(6,""/*DNT*/);
			answ = sds_getdist(NULL,disp,&horiznew);
			if (answ == RTCAN) {
				return (-1);
			}

			if (answ == RTNONE) {
				horiznew=horiz;
			}

			sds_rtos(vert,-1,-1,yspacing);								// Gets vertical spacing
			sprintf(disp,ResourceString(IDC_CMDS3__NVERTICAL_SPACI_363, "\nVertical spacing <%s>: " ),yspacing);
			sds_initget(6,""/*DNT*/);
			answ = sds_getdist(NULL,disp,&vertnew);
			if (answ == RTCAN) {
				return (-1);
			}
			if (answ == RTNONE) {
				vertnew=vert;
			}

			rb1.restype=RTPOINT;										// Sets the snap spacing variable
			rb1.resval.rpoint[0]=horiznew;
			rb1.resval.rpoint[1]=vertnew;
			rb1.resval.rpoint[2]=0;
			sds_setvar("SNAPUNIT"/*DNT*/,&rb1);
			rb1.restype=RTSHORT;										// Turns Snap ON
			rb1.resval.rint=1;
			sds_setvar("SNAPMODE"/*DNT*/,&rb1);

		} else if ((strisame(response,"ROTATE"/*DNT*/))) {		// ** Rotate **  Turns Snap ON, gets base point, angle, and x & y spacing

			SDS_getvar(NULL,DB_QSNAPBASE,&rb1,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);								// Gets the base point for the rotated grid
			xbase=rb1.resval.rpoint[0];
			sds_rtos(xbase,-1,-1,xbasestr);
			ybase=rb1.resval.rpoint[1];
			sds_rtos(ybase,-1,-1,ybasestr);
	   		sprintf(disp,ResourceString(IDC_CMDS3__NBASE_POINT_FOR_365, "\nBase point for snap grid <%s,%s>: " ),xbasestr,ybasestr);
			answ=sds_getpoint(NULL,disp,newpt);
			if (answ == RTCAN) {
				return (-1);
			}
			if (answ == RTNONE) {
				newpt[0]=xbase;
				newpt[1]=ybase;
				newpt[2]=0;
			}

			SDS_getvar(NULL,DB_QSNAPANG,&rb1,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);									// Gets the angle for the rotated grid
			oldang=rb1.resval.rreal;
			sds_angtos(oldang,-1,-1,oastr);
			sprintf(disp,ResourceString(IDC_CMDS3__NROTATION_ANGLE_366, "\nRotation angle <%s>: " ),oastr);
			answ=sds_getangle(newpt,disp,&newang);
		   	if (answ == RTNONE) {
				newang=oldang;
			}

			rb1.restype=RTPOINT;										// Sets the base point for the rotated grid
			ic_ptcpy(rb1.resval.rpoint,newpt);
			sds_setvar("SNAPBASE"/*DNT*/,&rb1);
			rb1.restype=RTREAL;											// Sets the angle for the rotated grid
			rb1.resval.rreal=newang;
			sds_setvar("SNAPANG"/*DNT*/,&rb1);
			rb1.restype=RTSHORT;										// Turns Snap ON
			rb1.resval.rint=1;
			sds_setvar("SNAPMODE"/*DNT*/,&rb1);

		} else if ((strisame(response,"STYLE"/*DNT*/))) {		// ** Style **	Turns Snap ON, gets isometric or standard, and x and y spacing

			SDS_getvar(NULL,DB_QSNAPSTYL,&rb1,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			oldstyle=rb1.resval.rint;
			newstyle=oldstyle;

	   		if (oldstyle == 1) {									// If the style was Isometric...
				sprintf(disp,ResourceString(IDC_CMDS3__NSTANDARD____IS_370, "\nStandard/<Isometric>: " ));
				sds_initget(SDS_RSG_NOZERO, ResourceString(IDC_SNAP_STYLE_INIT1, "Standard Isometric ~_Standard ~_Isometric"));
				answ = sds_getkword(disp,prompt);
				if (answ==RTNONE) strcpy(prompt,ResourceString(IDC_CMDS3_ISOMETRIC_372, "Isometric" ));		// ENTER defaults to Isometric

			} else {												// If the style was Standard...
				sprintf(disp,ResourceString(IDC_CMDS3__NISOMETRIC____S_373, "\nIsometric/<Standard>: " ));
				sds_initget(SDS_RSG_NOZERO, ResourceString(IDC_SNAP_STYLE_INIT1,"Standard Isometric ~_Standard ~_Isometric"));
				answ = sds_getkword(disp,prompt);
				if (answ==RTNONE)
					strcpy(prompt,"Standard"/*DNT*/);		// ENTER defaults to Standard
			}

		if ((strisame(prompt,"STANDARD"/*DNT*/)) ) {
				newstyle=0;
		} else if ((strisame(prompt,"ISOMETRIC"/*DNT*/))) {
				newstyle=1;
			}

			if (newstyle == 1) {												// If it's now an Isometric snap grid...
				sprintf(disp,ResourceString(IDC_CMDS3__NVERTICAL_SPACI_363, "\nVertical spacing <%s>: " ),yspacing);
				sds_initget(SDS_RSG_NONEG|SDS_RSG_NOZERO,""/*DNT*/);
				answ = sds_getdist(NULL,disp,&vertnew);
				if (answ == RTCAN) {
					return (-1);
				}
				if (answ == RTNONE) {
					vertnew=vert;
				}

				horiznew = vertnew * (sqrt(3.0));
				rb1.restype=RTPOINT;
				rb1.resval.rpoint[0]=horiznew;
				rb1.resval.rpoint[1]=vertnew;
				rb1.resval.rpoint[2]=0.0;
				sds_setvar("SNAPUNIT"/*DNT*/,&rb1);
				rb1.restype=RTSHORT;
				rb1.resval.rint=1;
				sds_setvar("SNAPSTYL"/*DNT*/,&rb1);

			} else if (newstyle == 0) {											// If it's now a Standard snap grid...
				sds_rtos(vert,-1,-1,yspacing);
				sprintf(disp,ResourceString(IDC_CMDS3__NASPECT___VERTI_376, "\nAspect/Vertical spacing <%s>: " ),yspacing);
				sds_initget(SDS_RSG_NOZERO, ResourceString(IDC_SNAP_ISOMETRIC_INIT1, "Aspect ~_Aspect"));
				answ=sds_getreal(disp,&vertnew);
				if (answ == RTCAN) {
					return (-1);
				}
				sds_getinput(initval);
				if ((strisame(initval,"ASPECT"/*DNT*/))) {									// If the user enters Aspect...
					sds_rtos(horiz,-1,-1,xspacing);
					sprintf(disp,ResourceString(IDC_CMDS3__NHORIZONTAL_SPA_377, "\nHorizontal spacing <%s> :" ),xspacing);
					sds_initget(SDS_RSG_NONEG|SDS_RSG_NOZERO,""/*DNT*/);
					answ = sds_getdist(NULL,disp,&horiznew);
					if (answ == RTNONE) {
						horiznew=horiz;
					}
					sds_rtos(vert,-1,-1,yspacing);
					sprintf(disp,ResourceString(IDC_CMDS3__NVERTICAL_SPACI_363, "\nVertical spacing <%s>: " ),yspacing);
					sds_initget(SDS_RSG_NONEG|SDS_RSG_NOZERO,""/*DNT*/);
					answ = sds_getdist(NULL,disp,&vertnew);
					if (answ == RTNONE) {
						vertnew=vert;
					}
					rb1.restype=RTSHORT;
					rb1.resval.rint=0;
					sds_setvar("SNAPSTYL"/*DNT*/,&rb1);
					rb1.restype=RTPOINT;
					rb1.resval.rpoint[0]=horiznew;
					rb1.resval.rpoint[1]=vertnew;
					rb1.resval.rpoint[2]=0.0;
					sds_setvar("SNAPUNIT"/*DNT*/,&rb1);

				} else {										// NOTE:  This will make x=y, even if the user had *different* x and y
					if (answ == RTNONE) {						//		  to start with and accepted Standard and the current vertical
						vertnew=vert;							//		  spacing default.	Sorry, but it's a compatibility thing.
					}
					horiznew=vertnew;
					rb1.restype=RTSHORT;
					rb1.resval.rint=0;
					sds_setvar("SNAPSTYL"/*DNT*/,&rb1);
					rb1.restype=RTPOINT;
					rb1.resval.rpoint[0]=horiznew;
					rb1.resval.rpoint[1]=vertnew;
					rb1.resval.rpoint[2]=0;
					sds_setvar("SNAPUNIT"/*DNT*/,&rb1);
				}//** End initval checking else if
			}//** End Standard Style
		}//** End Style If

		if (SDS_CURDWG && SDS_CURVIEW) SDS_VarToTab(SDS_CURDWG,SDS_CURVIEW->m_pVportTabHip);

		// Redraw if x, y, rotation, or style changed && grid is on.
		SDS_getvar(NULL,DB_QGRIDMODE,&rb1,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);		// 0 = Off, 1 = On
		if (rb1.resval.rint && ((vertnew!=vert) || (horiznew!=horiz) || (oldstyle!=newstyle) || (oldang!=newang))) {
			sds_redraw(NULL,IC_REDRAW_DONTCARE);
		}

		return(0);
	} //** Big IF
}

//========================================================================================

short cmd_grid(void) {

	//*** TODO - This has to accept number with X's after them to indicate multiples of the Snap grid spacing.
	//***		 Even in Isometric mode.

	if (cmd_iswindowopen() == RTERROR)
		return RTERROR;

	char response[IC_ACADBUF];
	char disp[IC_ACADBUF];				// This is where the prompt gets stored
	char xspacing[50];					// The horizontal spacing that gets displayed in the command line
	char yspacing[50];					// The vertical spacing that gets displayed in the command line
	char *fcp1;
	struct resbuf rb1;
	RT answ = RTNORM;
	short mode;							// 0 is off, 1 is on
	short aspectflag;					// 0 is x=y, 1 is x!=y, 2 is isometric
	short snapstyl;						// 0 is standard, 1 is isometric
	sds_real horiz,vert,horiznew,vertnew;
	sds_point snapunit;

	// Get existing GRIDMODE value
	SDS_getvar(NULL,DB_QGRIDMODE,&rb1,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);			// 0 is off, 1 is on
	mode=rb1.resval.rint;

	// Check aspect ratio
	SDS_getvar(NULL,DB_QGRIDUNIT,&rb1,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);			// A 2D point giving the x and y grid spacing, e.g. (2,3)
	horiznew=horiz=rb1.resval.rpoint[0];
	vertnew=vert=rb1.resval.rpoint[1];
	SDS_getvar(NULL,DB_QSNAPSTYL,&rb1,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);			// 0 is standard, 1 is isometric
	snapstyl=rb1.resval.rint;
	SDS_getvar(NULL,DB_QSNAPUNIT,&rb1,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);			// A 2D point giving the x and y grid spacing, e.g. (2,3)
	snapunit[0]=rb1.resval.rpoint[0];
	snapunit[1]=rb1.resval.rpoint[1];

	if (mode==0) {												// The grid is off...
		if (snapstyl==1) {										// Isometric grid
			aspectflag=2;										//	  so aspect doesn't apply.
			sds_rtos((2.0*vert),-1,-1,xspacing);													// Does this need to be here?
			sds_rtos(vert,-1,-1,yspacing);
			sprintf(disp,ResourceString(IDC_CMDS3__NGRID_IS_OFF____378, "\nGrid is off:  ON/Snap/<Spacing in y direction (isometric, y = %s)>: " ),yspacing);
			sds_initget(SDS_RSG_NONEG+SDS_RSG_OTHER, ResourceString(IDC_GRID_INIT1, "Grid_on|ON ~OFf ~Toggle ~ Sync_to_Snap|Snap ~_ON ~_OFf ~_Toggle ~_ ~_Snap"));
			answ = sds_getdist(NULL,disp,&vertnew);
			sds_getinput(response);
			if (answ==RTNONE)
				strcpy(response,ResourceString(IDC_CMDS3_ON_380, "On" ));				// ENTER defaults to On, with current settings.
		} else {
			if ((horiz != vert)) {
				aspectflag=1;										// x and y are different.
				sds_rtos(horiz,-1,-1,xspacing);
				sds_rtos(vert,-1,-1,yspacing);
				sprintf(disp,ResourceString(IDC_CMDS3__NGRID_IS_OFF____381, "\nGrid is off:  ON/Snap/Equal/<Aspect (x = %s, y = %s)>: " ),xspacing,yspacing);
				sds_initget(0, ResourceString(IDC_GRID_INIT2, "Grid_on|ON ~OFf ~Toggle ~ Sync_to_Snap|Snap ~ Equal_spacing|Equal Aspect ~_ON  ~_OFf  ~_Toggle ~_  ~_Snap ~_  ~_Equal  ~_Aspect"));
				answ = sds_getkword(disp,response);
				if (answ==RTNONE)
					strcpy(response,"ASPECT"/*DNT*/);		// ENTER defaults to Aspect
				if ((strisame(response,"EQUAL"/*DNT*/))) {				// If setting back to Equal spacing, drops right into next if, below
					horiznew=horiz=vertnew=vert;
				}
				if (RTNORM==answ || RTNONE==answ)answ=RTKWORD;	//set answ as if using getdist, not getkword
			}
			if (horiz==vert) {
				aspectflag=0;										// x and y are the same
				sds_rtos(horiz,-1,-1,xspacing);
				if ((icadRealEqual(horiz,0.0)) && (icadRealEqual(vert,0.0))) {
					sprintf(disp,ResourceString(IDC_CMDS3__NGRID_IS_OFF____383, "\nGrid is off:  ON/Aspect/<Grid spacing (synced with snap)>: " ));
					sds_initget(SDS_RSG_NONEG+SDS_RSG_OTHER, ResourceString(IDC_GRID_INIT3, "Grid_on|ON ~OFf ~Toggle ~ ~Snap Aspect ~_ON ~_OFf ~_Toggle ~_ ~_Snap ~_Aspect"));
					answ = sds_getdist(NULL,disp,&vertnew);
					sds_getinput(response);
					horiznew=vertnew;
					if (answ==RTNONE) strcpy(response,ResourceString(IDC_CMDS3_SNAP_385, "Snap" ));		// ENTER defaults to On, with current settings.
				} else {
					sprintf(disp,ResourceString(IDC_CMDS3__NGRID_IS_OFF____386, "\nGrid is off:  ON/Snap/Aspect/<Grid spacing (x and y = %s)>: " ),xspacing);
					sds_initget(SDS_RSG_NONEG+SDS_RSG_OTHER, ResourceString(IDC_GRID_INIT4, "Grid_on|ON ~OFf ~Toggle ~ Sync_to_Snap|Snap Aspect ~_ON ~_OFf ~_Toggle ~_ ~_Snap ~_Aspect"));
					answ = sds_getdist(NULL,disp,&vertnew);
					sds_getinput(response);
					horiznew=vertnew;
					if (answ==RTNONE)
						strcpy(response,ResourceString(IDC_CMDS3_ON_380, "On" ));		// ENTER defaults to On, with current settings.
				}
			}
		}
	} else {													// The grid is on...
		if (snapstyl==1) {										// Isometric grid
			aspectflag=2;										//	  so aspect doesn't apply.
			sds_rtos((2.0*vert),-1,-1,xspacing);														// Does this need to be here?
			sds_rtos(vert,-1,-1,yspacing);
			sprintf(disp,ResourceString(IDC_CMDS3__NGRID_IS_ON__IS_388, "\nGrid is on, isometric:  OFF/Snap/<Spacing in y direction (y = %s)>: " ),yspacing);
			sds_initget(SDS_RSG_NONEG+SDS_RSG_OTHER, ResourceString(IDC_GRID_INIT5, "~ON Grid_off|OFf ~Toggle ~ Sync_to_Snap|Snap ~_ON ~_OFf ~_Toggle ~_ ~_Snap"));
			answ = sds_getdist(NULL,disp,&vertnew);
			sds_getinput(response);
			if (answ==RTNONE)
				strcpy(response,ResourceString(IDC_CMDS3_ON_380, "On" ));		// ENTER defaults to On, with current settings.
			// Leave this, even though already on, this keeps it from fussing about user not entering a point.
		} else {
			if ((horiz != vert)) {
				aspectflag=1;										// x and y are different.
				sds_rtos(horiz,-1,-1,xspacing);
				sds_rtos(vert,-1,-1,yspacing);
				sprintf(disp,ResourceString(IDC_CMDS3__NGRID_IS_ON___O_390, "\nGrid is on:  OFF/Snap/Equal/<Aspect (x = %s, y = %s)>: " ),xspacing,yspacing);
				sds_initget(0, ResourceString(IDC_GRID_INIT6, "~ON Grid_off|OFf ~Toggle ~ Sync_to_Snap|Snap ~ Equal_spacing|Equal Aspect ~_ON ~_OFf ~_Toggle ~_ ~_Snap ~_ ~_Equal ~_Aspect"));
				answ = sds_getkword(disp,response);
				if (answ==RTNONE)
					strcpy(response,"ASPECT"/*DNT*/);		// ENTER defaults to Aspect
				if ((strisame(response,"EQUAL"/*DNT*/))) {				// If setting back to Equal spacing, drops right into next if, below
					horiznew=horiz=vertnew=vert;
				}
				if (RTNORM==answ || RTNONE==answ)answ=RTKWORD;		// Falls through code below turning grid on w/Aspect settings.
			}
			if (horiz==vert) {
				aspectflag=0;										// x and y are the same
				sds_rtos(horiz,-1,-1,xspacing);
				if ((horiz==0) && (vert==0)) {
					sprintf(disp,ResourceString(IDC_CMDS3__NGRID_IS_ON___O_392, "\nGrid is on:  OFF/Aspect/<Grid spacing (synced with snap)>: " ));
					sds_initget(SDS_RSG_NONEG+SDS_RSG_OTHER,ResourceString(IDC_GRID_INIT7, "~ON Grid_off|OFf ~Toggle ~ ~Snap Aspect ~_ON ~_OFf ~_Toggle ~_ ~_Snap ~_Aspect"));
				} else {
					sprintf(disp,ResourceString(IDC_CMDS3__NGRID_IS_ON___O_394, "\nGrid is on:  OFF/Snap/Aspect/<Grid spacing (x and y = %s)>: " ),xspacing);
					sds_initget(SDS_RSG_NONEG+SDS_RSG_OTHER, ResourceString(IDC_GRID_INIT8, "~ON Grid_off|OFf ~Toggle ~ Sync_to_Snap|Snap Aspect ~_ON ~_OFf ~_Toggle ~_ ~_Snap ~_Aspect"));
				}
				answ = sds_getdist(NULL,disp,&vertnew);
				sds_getinput(response);
				horiznew=vertnew;
				if (answ==RTNONE)
					strcpy(response,"ON"/*DNT*/);		// ENTER defaults to On, with current settings.
				// Leave this, even though already on, this keeps it from fussing about user not entering a point.
			}
		}
	}

	if (answ==RTCAN)
		return (RTCAN);
	else {
		if (answ == RTNORM) {
			rb1.restype=RTPOINT;
			if (snapstyl!=0) {									// Isometric grid
				horiznew=vertnew*1.732050807568877;
				vertnew*=0.5;
			}
   			rb1.resval.rpoint[0]=horiznew;
			rb1.resval.rpoint[1]=vertnew;
			rb1.resval.rpoint[2]=0.0;
			sds_setvar("GRIDUNIT"/*DNT*/,&rb1);
			rb1.restype=RTSHORT;
			rb1.resval.rint=1;
			sds_setvar("GRIDMODE"/*DNT*/,&rb1);
		//KWORDS BEGIN HERE
		} else if ((strisame(response,"ON"/*DNT*/))) {
			rb1.restype=RTSHORT;
			rb1.resval.rint=1;
			sds_setvar("GRIDMODE"/*DNT*/,&rb1);
		} else if ((strisame(response,"OFF"/*DNT*/))) {
			rb1.restype=RTSHORT;
			rb1.resval.rint=0;
			sds_setvar("GRIDMODE"/*DNT*/,&rb1);
			sds_redraw(NULL,IC_REDRAW_DONTCARE);
		} else if ((strisame(response,"TOGGLE"/*DNT*/))) {
			rb1.restype=RTSHORT;
			if (mode==0) {
				rb1.resval.rint=1;
				sds_setvar("GRIDMODE"/*DNT*/,&rb1);
			} else {
				rb1.resval.rint=0;
				sds_setvar("GRIDMODE"/*DNT*/,&rb1);
				sds_redraw(NULL,IC_REDRAW_DONTCARE);
			}
		} else if ((strisame(response,"SNAP"/*DNT*/))) {
			rb1.restype=RTPOINT;
			rb1.resval.rpoint[0]=0.0;
			rb1.resval.rpoint[1]=0.0;
			rb1.resval.rpoint[2]=0.0;
			sds_setvar("GRIDUNIT"/*DNT*/,&rb1);
			rb1.restype=RTSHORT;
			rb1.resval.rint=1;
			sds_setvar("GRIDMODE"/*DNT*/,&rb1);
   		} else if (((strisame(response,"ASPECT"/*DNT*/))) || (aspectflag == 1)) {
			sds_rtos(horiz,-1,-1,xspacing);
			sprintf(disp,ResourceString(IDC_CMDS3__NHORIZONTAL_SPA_362, "\nHorizontal spacing <%s>: " ),xspacing);
			sds_initget(RSG_NONEG+RSG_OTHER,NULL);
			answ = sds_getdist(NULL,disp,&horiznew);
			if (answ == RTCAN)
				return (RTCAN);
			if (answ == RTNONE) horiznew=horiz;
			else if (answ==RTKWORD){
				sds_getinput(response);
				fcp1=response+strlen(response)-1;
				if (*fcp1!='X' && *fcp1!='x'){cmd_ErrorPrompt(CMD_ERR_DIST,0);
					return(RTERROR);}
				*fcp1=0;
				if ((snapunit[2]=atof(response))>0.0){	//z on snapunit not used
					horiznew=snapunit[2]*snapunit[0];
				}else{
					cmd_ErrorPrompt(CMD_ERR_DIST,0);
					return(RTERROR);
				}
			}
			sds_rtos(vert,-1,-1,yspacing);
			sprintf(disp,ResourceString(IDC_CMDS3__NVERTICAL_SPACI_363, "\nVertical spacing <%s>: " ),yspacing);
			sds_initget(RSG_NONEG+RSG_OTHER,NULL);
			answ = sds_getdist(NULL,disp,&vertnew);
			if (answ == RTCAN)
				return (-1);
			if (answ == RTNONE) vertnew=vert;
			else if (answ==RTKWORD){
				sds_getinput(response);
				fcp1=response+strlen(response)-1;
				if (*fcp1!='X' && *fcp1!='x'){
					cmd_ErrorPrompt(CMD_ERR_DIST,0);
					return(RTERROR);
				}
				*fcp1=0;
				if ((snapunit[2]=atof(response))>0.0){	//z on snapunit not used
					vertnew=snapunit[2]*snapunit[1];
				}else{
					cmd_ErrorPrompt(CMD_ERR_DIST,0);
					return(RTERROR);
				}
			}
			rb1.restype=RTPOINT;
			rb1.resval.rpoint[0]=horiznew;
			rb1.resval.rpoint[1]=vertnew;
			rb1.resval.rpoint[2]=0.0;
			sds_setvar("GRIDUNIT"/*DNT*/,&rb1);
			rb1.restype=RTSHORT;
			rb1.resval.rint=1;
			sds_setvar("GRIDMODE"/*DNT*/,&rb1);
		}else{
			//they must have entered something else.  We'll check if it's in
			//the form of "2.5x", which represents a grid relative to current
			//snap setting....
			fcp1=response+strlen(response)-1;
			if (*fcp1!='X'/*DNT*/ && *fcp1!='x'/*DNT*/){
				cmd_ErrorPrompt(CMD_ERR_DIST,0);
				return(RTERROR);
			}
			*fcp1=0;
			if ((snapunit[2]=atof(response))>0.0){	//z on snapunit not used
				vertnew=snapunit[2]*snapunit[1];
				if (snapstyl!=0) {					// Isometric grid
					horiznew=vertnew*1.732050807568877;
					vertnew*=0.5;
				}else{
					horiznew=vertnew;
				}
			}else{
				cmd_ErrorPrompt(CMD_ERR_DIST,0);
				return(RTERROR);
			}
			rb1.restype=RTPOINT;
			rb1.resval.rpoint[0]=horiznew;
			rb1.resval.rpoint[1]=vertnew;
			rb1.resval.rpoint[2]=0.0;
			sds_setvar("GRIDUNIT"/*DNT*/,&rb1);
			rb1.restype=RTSHORT;
			rb1.resval.rint=1;
			sds_setvar("GRIDMODE"/*DNT*/,&rb1);
		}
		// Redraw if either x or y has changed.
		if (SDS_CURDWG && SDS_CURVIEW) SDS_VarToTab(SDS_CURDWG,SDS_CURVIEW->m_pVportTabHip);
		if ((vertnew!=vert) || (horiznew!=horiz)) {
			sds_redraw(NULL,IC_REDRAW_DONTCARE);
		}
		return(RTNORM);
	} //** Big IF
	if (SDS_CURDWG && SDS_CURVIEW)
		SDS_VarToTab(SDS_CURDWG,SDS_CURVIEW->m_pVportTabHip);
	return(RTNORM);
}
//==========================================================================================

short cmd_attdisp(void) {

	if (cmd_iswindowopen() == RTERROR)
		return RTERROR;

	char response[8];
	struct resbuf rb1;
	short am;
	char prompt[8];

	SDS_getvar(NULL,DB_QATTMODE,&rb1,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	am=rb1.resval.rint;
	switch (am){
		case  0:
			strcpy(prompt, ResourceString(IDC_CMDS3_OFF_398, "Off"));
			break;
		case  1:
			strcpy(prompt, ResourceString(IDC_CMDS3_NORMAL_399, "Normal"));
			break;
		case  2:
			strcpy(prompt, ResourceString(IDC_CMDS3_ON_380, "On"));
			break;
	}/* End Switch */
	char tmpPrompt[100];
	sprintf(tmpPrompt, ResourceString(IDC_CMDS3__NATTRIBUTE_DISP_400, "\nAttribute display mode:  ON/OFF/Normal/<%s>: " ),prompt);
	sds_printf(tmpPrompt);
	sds_initget(SDS_RSG_NOZERO, ResourceString(IDC_ATTDISP_INIT1, "Display_Attribute|ON Hide_Attributes|OFf ~ Display_normally|Normal ~_ON ~_OFf ~_ ~_Normal"));
	sds_getkword(""/*DNT*/, response);
	if ((strisame(response,"ON"/*DNT*/))) {
		rb1.resval.rint=2;
		sds_setvar("ATTMODE"/*DNT*/,&rb1);
		cmd_regen();
	}
	if ((strisame(response,"OFF"/*DNT*/))) {
		rb1.resval.rint=0;
		sds_setvar("ATTMODE"/*DNT*/,&rb1);
		cmd_regen();
	}
 	if ((strisame(response,"NORMAL"/*DNT*/))) {
		rb1.resval.rint=1;
		sds_setvar("ATTMODE"/*DNT*/,&rb1);
		cmd_regen();
	}

	SDS_SetLastPrompt(tmpPrompt);
	return(RTNORM);
}

//==========================================================================================
short cmd_area(void) {
//*** This function calculates the area and perimeter of objects or of
//*** defined areas.
//*** RETURNS:	0 - Success.
//***		   -1 - User cancel.
//***		   -2 - RTERROR returned from an ads function.
//*** NOTE: This f'n works on ents which ACAD doesn't!

	if (cmd_iswindowopen() == RTERROR)
		return RTERROR;

	//BugZilla No. 78050; 25-03-2002
	if (cmd_istransparent() == RTERROR) return RTERROR;

	char fs1[IC_ACADBUF];
	struct resbuf setgetrb,*elist,*rbp1,rbucs,rbwcs;
	struct ic_pointsll *ptsllbeg,*ptsllcur,*tmp;
	sds_point ptOrig,ptPrev,ptNew;					//The very first point, most recent point, and newest point
	sds_real perimeter,area,totarea,totlen,fr1;
	sds_name ename,etemp,drawent;
	int ret,plflag,iPointCount;
	short circleflg,rc,addmode;
	int lwplflag,lwplnumverts;
	sds_real lastlwplbulge;
	db_lwpolyline *lwpline;

	drawent[0]=drawent[1]=0L;
	rc=circleflg=addmode=0;
	rbucs.restype=rbwcs.restype=RTSHORT;
	rbucs.resval.rint=1;
	rbwcs.resval.rint=0;

	//addmode=1 if adding ents, -1 if subtracting, 0 if neither
	perimeter=area=totarea=totlen=0.0;
	ptsllbeg=ptsllcur=tmp=NULL; elist=rbp1=NULL;
	for(;;) {
		restart:
		if (addmode==1) {
			strncpy(fs1,ResourceString(IDC_CMDS3__NADDING___ENTIT_403, "\nAdding:  Entity/Subtract/<First point>: " ),sizeof(fs1)-1);
			if (icadRealEqual(totarea,0.0) && icadRealEqual(totlen,0.0)) {
				if (sds_initget(SDS_RSG_OTHER, ResourceString(IDC_AREA_INIT1,"Add_entities_to_area|Entity ~Object ~ ~Add Subtract_areas|Subtract ~_Entity ~_Object ~_ ~_Add ~_Subtract"))!=RTNORM)
					return(-2);
			} else {
				if (sds_initget(SDS_RSG_OTHER+SDS_RSG_NOCANCEL, ResourceString(IDC_AREA_INIT2, "Add_entities_to_area|Entity ~Object ~ ~Add Subtract_areas|Subtract ~ ` ~_Entity ~_Object ~_ ~_Add ~_Subtract ~_")) != RTNORM)
					return(-2);
			}
		} else if (addmode==-1) {
			strncpy(fs1,ResourceString(IDC_CMDS3__NSUBTRACTING____406, "\nSubtracting:  Entity/Add/<First point>: " ),sizeof(fs1)-1);
			if (icadRealEqual(totarea,0.0) && icadRealEqual(totlen,0.0)) {
				if (sds_initget(SDS_RSG_OTHER, ResourceString(IDC_AREA_INIT3,"Subtract_entities_from_area|Entity ~Object ~ Add_to_area|Add ~Subtract ~_Entity ~_Object ~_ ~_Add ~_Subtract"))!=RTNORM)
					return(-2);
			} else {
				if (sds_initget(SDS_RSG_OTHER+SDS_RSG_NOCANCEL, ResourceString(IDC_AREA_INIT4, "Subtract_entities_from_area|Entity ~Object ~ Add_areas|Add ~Subtract ~ ` ~_Entity ~_Object ~_ ~_Add ~_Subtract ~_"))!=RTNORM)
					return(-2);
 			}
	   } else {
			strncpy(fs1,ResourceString(IDC_CMDS3__NENTITY___ADD___409, "\nEntity/Add/Subtract/<First point>: " ),sizeof(fs1)-1);
			if (sds_initget(SDS_RSG_OTHER, ResourceString(IDC_AREA_INIT5, "Find_area_of_one_entity|Entity ~Object ~ Add_areas_together|Add Subtract_areas|Subtract ~_Entity ~_Object ~_ ~_Add ~_Subtract"))!=RTNORM)
				return(-2);
		}
		if ((ret=sds_getpoint(NULL,fs1,ptOrig))==RTERROR) { 		 // Get the first point.  This should not get changed below.
			rc=(-2); goto bail;
		} else if (ret==RTCAN){
			rc=(-1); goto bail;
		} else if (ret==RTNONE){
			rc=(-1);goto bail;
		} else if (ret==RTNORM) {
			ic_ptcpy(ptNew,ptOrig);									// For calcs below, make "new" point same as first point.
			ic_ptcpy(ptPrev,ptOrig);								// Also make "previous" point same as first point.
			iPointCount=1;											// 1 means we've only gotten the first point so far.

			//*** First point options.
			for(;;) {
				// Set LASTPOINT system variable to the most recently picked point.
				setgetrb.restype=RT3DPOINT;
				ic_ptcpy(setgetrb.resval.rpoint,ptNew);
				SDS_setvar(NULL,DB_QLASTPOINT,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);

				// Allocate a linked list for points user will be picking.				
				if ((tmp= new struct ic_pointsll )==NULL) {
					rc=(-2);
					goto bail;
				}
				memset(tmp,0,sizeof(struct ic_pointsll));
				if (ptsllbeg==NULL) {
					ptsllbeg=ptsllcur=tmp;
				} else {
					ptsllcur=ptsllcur->next=tmp;
				}
				ic_ptcpy(ptsllcur->pt,ptNew);					// Stick the newest point into the linked list.

				// Set up the prompt string for the SDS_dragobject below.
				strcpy(fs1,"\n"/*DNT*/);
				if (addmode==1) {
					strcat(fs1,ResourceString(IDC_CMDS3_ADDING____411, "Adding:  " ));
				} else if (addmode==-1) {
					strcat(fs1,ResourceString(IDC_CMDS3_SUBTRACTING____412, "Subtracting:  " ));
				}
				strcat(fs1,ResourceString(IDC_CMDS3__NEXT_POINT____413, "<Next point>: " ));

				// Set up resbuf for SDS_dragobject, and counter and return variables.
				SDS_getvar(NULL,DB_QDRAGMODE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				ret=RTNORM;
				if (sds_initget(SDS_RSG_OTHER, ResourceString(IDC_AREA_INIT6, "Done_specifying_area|Done ~_Done"))!=RTNORM) {
					rc=(-2);
					goto bail;
				} // For adding and subtracting areas - not by entity

				// Either get the second point (if), or get subsequent points & rubberband from prev and orig.
				if (iPointCount==1)
					ret=internalGetpoint(ptOrig,fs1,ptNew);
				else
					ret=SDS_dragobject(35,setgetrb.resval.rint,ptOrig,ptPrev,0.0,fs1,NULL,ptNew,NULL);

				if (ret==RTERROR) {
					rc=(-2);
					goto bail;
				} else if (ret==RTCAN) {
					rc=(-1);
					goto bail;
				} else if ((ret==RTNONE) || (ret==RTKWORD)) {
					break;
				} else if (ret!=RTNORM) {
					cmd_ErrorPrompt(CMD_ERR_PT,0);
					continue;
				}

				++iPointCount;						// Increment the counter.
				sds_grdraw(ptPrev,ptNew,-1,1);		// Draw a line from the last point to the new one.
				ic_ptcpy(ptPrev,ptNew);				// Bump the "new" point to "prev" so we can get another new one.

			}
			//*** Calculate the area and perimeter from the list of points.
			if (ptsllbeg!=NULL) {
				//*** Calculate the area.
				if ((area=ic_llarea(ptsllbeg))<0) {
					area=0.0;
				}
				//*** Calculate the perimeter.
				ic_ptcpy(ptPrev,ptsllbeg->pt);
				ic_ptcpy(ptNew,ptsllbeg->pt);
				for(tmp=ptsllbeg->next; tmp!=NULL; tmp=tmp->next) {
					perimeter+=sds_distance(ptNew,tmp->pt);
					ic_ptcpy(ptNew,tmp->pt);
				}
				perimeter+=sds_distance(ptNew,ptPrev);
			}
			//*** Print to screen.
			cmd_area_display(area,perimeter,&totarea,&totlen,addmode,0);
			//*** Free the linked list of points.
			for(tmp=ptsllbeg; tmp!=NULL; tmp=ptsllcur) {
				ptsllcur=tmp->next;
				if (ptsllcur!=NULL)sds_grdraw(tmp->pt,ptsllcur->pt,-1,1);
				IC_FREE(tmp);
			}
			ptsllbeg=NULL;

			
			//*** Reinitialize variables before continuing.
			if (0==addmode)break;
			ptsllbeg=ptsllcur=tmp=NULL;
			perimeter=area=0.0;
			circleflg=0;
		} else if (ret==RTSTR || ret==RTKWORD) {
			sds_getinput(fs1);
			if ((strisame(fs1,"ENTITY"/*DNT*/)) || (strisame(fs1,"OBJECT"/*DNT*/))) {
				//*** Get object.
				for(;;) {
					//*** Make sure that area and perimeter are zero'd.
					perimeter=area=0.0;
					if (addmode==1) {
						strcpy(fs1,ResourceString(IDC_CMDS3__NADDING_AREA____418, "\nAdding area<Select entities>: " ));
					} else if (addmode==-1) {
						strcpy(fs1,ResourceString(IDC_CMDS3__NSUBTRACTING_AR_419, "\nSubtracting area<Select entities>: " ));
					} else {
						strcpy(fs1,ResourceString(IDC_CMDS3__NSELECT_ENTITY__420, "\nSelect entity for area calculation: " ));
					}
					sds_initget(0, ResourceString(IDC_AREA_INIT7, "Last ~_Last"));
					if ((ret=sds_entsel(fs1,ename,ptPrev))==RTCAN) {
						rc=(-1L);
						goto bail;
					}else if (ret==RTERROR) {
						if (SDS_getvar(NULL,DB_QERRNO,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) {
							rc=(-__LINE__);
							goto bail;
						}
						if (setgetrb.resval.rint==OL_ENTSELNULL) {
							if (0!=addmode)
								goto restart;
							else{
								rc=0L;
								goto bail;
							}
						} else if (setgetrb.resval.rint==OL_ENTSELPICK) {
							rc=0L;
							goto bail;
						}
					}else if (ret==RTKWORD){
						if ((ret=sds_entlast(ename))!=RTNORM) {
							rc=(-__LINE__);
							goto bail;
						}
					}
					if (drawent[0]!=0L && drawent[1]!=0L)
						sds_redraw(drawent,IC_REDRAW_UNHILITE);
					sds_redraw(ename,IC_REDRAW_HILITE);
					ic_encpy(drawent,ename);
					circleflg=0;	//default will be to print "Perimeter"
					if (elist!=NULL){sds_relrb(elist);elist=NULL;}
					if ((elist=sds_entget(ename))==NULL){
						rc=-1L;
						goto bail;
					}
					ic_assoc(elist,0);
					//*** Set the circleflg.
					if (strsame(ic_rbassoc->resval.rstring,"CIRCLE"/*DNT*/))
						circleflg=1;//print "Circumfrence"
					else if (strsame(ic_rbassoc->resval.rstring,db_hitype2str(DB_LINE)))
						circleflg=2;//print "Length"
					else if (strsame(ic_rbassoc->resval.rstring,"ARC"/*DNT*/))
						circleflg=2; //print "Length"
					else if (strsame(ic_rbassoc->resval.rstring,"SPLINE"/*DNT*/))
						circleflg = gr_get_spline_circleflag(elist);	//depends if closed
					else if (strsame(ic_rbassoc->resval.rstring,"ELLIPSE"/*DNT*/))
						circleflg = gr_get_ellipse_circleflag(elist);	//depends if closed
					else if (strsame(ic_rbassoc->resval.rstring,"POLYLINE"/*DNT*/)) {
						ic_assoc(elist,70);
						if (ic_rbassoc->resval.rint & (IC_PLINE_POLYFACEMESH | IC_PLINE_3DMESH)){	//Don't use if it is a 3dmesh
	   						cmd_ErrorPrompt(CMD_ERR_ENTTYPE,0);
							continue;
						}
						plflag=ic_rbassoc->resval.rint;
						if ((plflag & IC_PLINE_CLOSED)==0) {
							circleflg=2;	//print "Length"
							//temporarily close the polyline to get the area from db_edata
							//set the last vtx's bulge to zero - make sure we're closing w/straight line
							cmd_pline_next2lastvtx(ename,etemp);
							sds_entnext_noxref(etemp,etemp);
							if (rbp1!=NULL){sds_relrb(rbp1);rbp1=NULL;}
							if (NULL==(rbp1=sds_entget(etemp))){
								rc=-1L;
								goto bail;
							}
							ic_assoc(rbp1,42);
							fr1=ic_rbassoc->resval.rreal;
							ic_rbassoc->resval.rreal=0.0;
							if (sds_entmod(rbp1)!=RTNORM){
								rc=-1L;
								goto bail;
							}
							ic_assoc(rbp1,10);
							ic_ptcpy(ptNew,ic_rbassoc->resval.rpoint);
							//note: only close AFTER setting closing bulge to 0.0 so
							//the odd bulge doesn't flash on screen & give user wrong idea
							ic_assoc(elist,70);
							ic_rbassoc->resval.rint|=1;
							if (sds_entmod(elist)!=RTNORM){
								rc=-1L;
								goto bail;
							}
							sds_entnext_noxref(ename,etemp);
							if (rbp1!=NULL){sds_relrb(rbp1);rbp1=NULL;}
							if ((rbp1=sds_entget(etemp))==NULL){
								rc=-1L;
								goto bail;
							}
							ic_assoc(rbp1,10);
							ic_ptcpy(ptPrev,ic_rbassoc->resval.rpoint);
						}
					}
					else if (strsame(ic_rbassoc->resval.rstring,"LWPOLYLINE"/*DNT*/))
						{
						lwpline=(db_lwpolyline *)ename[0];
						lwpline->get_90(&lwplnumverts);
						lwpline->get_70(&lwplflag);
						if (!(lwplflag & IC_PLINE_CLOSED))
							{
							circleflg=2;
							int temp=lwplflag | IC_PLINE_CLOSED;
							lwpline->set_70(temp);
							lwpline->get_42(&lastlwplbulge,lwplnumverts-1);
							lwpline->set_42(0.0,lwplnumverts-1);
							}
						}

					 //*** Get the area and perimeter of the entity.
					ic_assoc(elist,0);
					if (strsame(ic_rbassoc->resval.rstring,"SPLINE"/*DNT*/))
						ret = gr_get_spline_edata(elist, &perimeter, &area);
					else if (strsame(ic_rbassoc->resval.rstring,"ELLIPSE"/*DNT*/))
						ret = gr_get_ellipse_edata(elist, &perimeter, &area);
					else if (strsame(ic_rbassoc->resval.rstring,"REGION"/*DNT*/))
					{
						perimeter = area = 0.;
						ret = -2; // entity is not handled.
						if (CModeler::get()->canView())
						{ 
							CDbAcisEntity* pObj = reinterpret_cast<CDbAcisEntity*>(ename[0]);
							if (CModeler::get()->getPerimeter(pObj->getData(), perimeter))
								ret = 0;
							else
								perimeter = 0.;
							if (CModeler::get()->canModify()) 
								if(CModeler::get()->getArea(pObj->getData(), area))
									ret = 0;
								else
									area = 0.;
    					}
					}
					else if (strsame(ic_rbassoc->resval.rstring,"3DSOLID"/*DNT*/))
					{
						perimeter = area = 0.;
						ret = -2; // entity is not handled.
						if (CModeler::get()->canModify())
						{ 
							CDbAcisEntity* pObj = reinterpret_cast<CDbAcisEntity*>(ename[0]);
							if(CModeler::get()->getArea(pObj->getData(), area))
								ret = 0;
							else
								area = 0.;
						}
					}
					else
						ret=db_edata(ename,NULL,NULL,&perimeter,&area);

					if (ret==0)
						{
						ic_assoc(elist,0);
						if (strsame(ic_rbassoc->resval.rstring,"POLYLINE"/*DNT*/) && (plflag & IC_PLINE_CLOSED)==0)
							{
							ic_assoc(rbp1,42);
							ic_rbassoc->resval.rreal=fr1;
							if (sds_entmod(rbp1)!=RTNORM)
								{
								rc=-1L;
								goto bail;
								}
							sds_relrb(rbp1);rbp1=NULL;
							ic_assoc(elist,70);
							ic_rbassoc->resval.rint=plflag;
							if (sds_entmod(elist)!=RTNORM)
								{
								rc=-1L;
								goto bail;
								}
							//subtract off perimeter we added in closing of pline
							perimeter-=sds_distance(ptPrev,ptNew);
							}
						else if (strsame(ic_rbassoc->resval.rstring,"LWPOLYLINE"/*DNT*/) && (lwplflag & IC_PLINE_CLOSED)==0)
							{ // a bit shorter when you use the hips, isn't it?
							lwpline->set_70(lwplflag);
							lwpline->set_42(lastlwplbulge,lwplnumverts-1);
							sds_point firstlwplpt, lastlwplpt;
							lwpline->get_10(firstlwplpt,0);
							lwpline->get_10(lastlwplpt,lwplnumverts-1);
							//subtract off perimeter we added in closing of pline
							perimeter-=sds_distance(lastlwplpt,firstlwplpt);
							}
						}
					IC_FREEIT
					ic_rbassoc=NULL;
					if (ret!=0) {
						cmd_ErrorPrompt(CMD_ERR_ENTTYPE,0);
//						sds_printf("\nSelected entity does not have an area.");
						continue;
					}
					//*** Print to screen.
					cmd_area_display(area,perimeter,&totarea,&totlen,addmode,circleflg);
					//if not in add or subt mode, we're done....
					if (addmode==0)
						goto bail;
				}
			} else if (strisame(fs1,"ADD"/*DNT*/)) {
				addmode=1;
				continue;
			} else if (strisame(fs1,"SUBTRACT"/*DNT*/)) {
				addmode=-1;
				continue;
			} else {
				cmd_ErrorPrompt(CMD_ERR_PTKWORD,0);
				continue;
			}
		}
	}

	bail:

	//*** Free the linked list of points & draw out our lines.
	for(tmp=ptsllbeg; tmp!=NULL; tmp=ptsllcur) {
		ptsllcur=tmp->next;
		if (ptsllcur!=NULL)sds_grdraw(tmp->pt,ptsllcur->pt,-1,1);
		IC_FREE(tmp);
	}
	if (elist!=NULL){sds_relrb(elist);elist=NULL;}
	if (rbp1!=NULL){sds_relrb(rbp1);rbp1=NULL;}
	if (drawent[0]!=0L && drawent[1]!=0L)
		sds_redraw(drawent,IC_REDRAW_UNHILITE);

	return(0);
}

//========================================================================================
void cmd_area_display(sds_real area, sds_real perimeter, sds_real *totarea, sds_real *totlen,
	short addmode, short circleflg) {
	//*** This function displays the area and perimeter on the screen and then
	//*** saves the values to the AREA and PERIMETER setvars.
	//*** Circleflg controls printout as follows:
	//***		0 = "Perimeter" is displayed
	//***		1 =	"Circumference" is displayed
	//***		2 = "Length" is displayed
	//***
	char fs1[IC_ACADBUF];
	struct resbuf setgetrb;
	struct resbuf varbuf,lubuf,declubuf;	// For getting whether the command bar is on or off, holding lunits
	
	sds_getvar("WNDLCMD"/*DNT*/,&varbuf);									// Get state of command line
	if (varbuf.resval.rint==0)										// If it's off
		sds_textscr();												//	  bring up the text screen.

	sds_getvar("LUNITS"/*DNT*/,&lubuf);
	sds_getvar("LUNITS"/*DNT*/,&declubuf);	// get a copy
	declubuf.resval.rint=2;					// force to decimal
	if (area>0.0){
		sds_setvar("LUNITS",&declubuf);
		cmd_rtos_area(area,-1,-1,fs1);

		if (lubuf.resval.rint==3 || lubuf.resval.rint==4)
		{
			double areaInFeet = area / 144.0;
			char areaInFeetString[IC_ACADBUF];
			cmd_rtos_area(areaInFeet, -1, -1, areaInFeetString);
			sds_printf(ResourceString(IDC_CMDS3__NAREA____S_SQ___428, "\nArea = %s\" sq. (%s\' sq.), " ), fs1, areaInFeetString);
		}
		else
		{
			sds_printf(ResourceString(IDC_CMDS3__NAREA____S_SQ___429, "\nArea = %s sq. units, " ),fs1);
		}

		sds_setvar("LUNITS",&lubuf);
	}else{
		sds_printf("\n"/*DNT*/);
	}
	sds_rtos(perimeter,-1,-1,fs1);
	if (circleflg==1) {
		sds_printf(ResourceString(IDC_CMDS3_CIRCUMFERENCE____430, "Circumference = %s" ),fs1);
	}else if (circleflg==0) {
		sds_printf(ResourceString(IDC_CMDS3_PERIMETER____S_431, "Perimeter = %s" ),fs1);
	}else if (circleflg==2) {
		sds_printf(ResourceString(IDC_CMDS3_LENGTH____S_432, "Length = %s" ),fs1);
	}
	//*** Set the PERIMETER system variable.
	setgetrb.restype=RTREAL;
	if (!addmode){

		setgetrb.resval.rreal=perimeter;
		sds_setvar("PERIMETER"/*DNT*/,&setgetrb);
	}else{
		if (addmode==1) *totlen+=perimeter;
		else if (addmode==-1) *totlen-=perimeter;
		
		setgetrb.resval.rreal=perimeter;
		
		sds_setvar("PERIMETER"/*DNT*/,&setgetrb);
		//*** Print total length to screen.
		sds_rtos(*totlen,-1,-1,fs1);
		sds_printf(ResourceString(IDC_CMDS3__NTOTAL_LENGTH___433, "\nTotal length = %s" ),fs1);
	}


	if (area > 0.0 || *totarea>0.0){
		if (!addmode) {
			//*** Set the AREA system variable.
			
			setgetrb.restype=RTREAL;
			setgetrb.resval.rreal=area;
			sds_setvar("AREA"/*DNT*/,&setgetrb);
			return;
		}

		if (addmode==1) *totarea+=area;
		else if (addmode==-1) *totarea-=area;
		//*** Print total area to screen.
		sds_setvar("LUNITS",&declubuf);
		cmd_rtos_area(*totarea,-1,-1,fs1);

		if (lubuf.resval.rint==3 || lubuf.resval.rint==4)
		{
			double totalAreaInFeet = *totarea / 144.0;
			char totalAreaInFeetString[IC_ACADBUF];
			cmd_rtos_area(totalAreaInFeet, -1, -1, totalAreaInFeetString);
			sds_printf(ResourceString(IDC_CMDS3__NTOTAL_AREA_____434, "\nTotal area = %s\" sq. (%s\' sq.)" ), fs1, totalAreaInFeetString);
		}
		else
		{
			sds_printf(ResourceString(IDC_CMDS3__NTOTAL_AREA_____435, "\nTotal area = %s sq. units" ),fs1);
		}

		sds_setvar("LUNITS",&lubuf);

		//*** Set the AREA system variable.
		
		setgetrb.restype=RTREAL;
		setgetrb.resval.rreal=*totarea;
		sds_setvar("AREA"/*DNT*/,&setgetrb);
	}
	return;
}

short cmd_mline(void)	 {
	cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0);
	SDS_FreeEventList(0);
	return(RTNORM);
}
// (prompting) 9 May, 1997
//*** This function creates 2D MultiLines (the only kind... for now).
//*** Multilines are a lot like Polylines.	They can have a bunch of straight segments, which,
//*** taken together, count as one entity.	They can't be 3D, or have arcs (not yet, anyway).
//***
//*** I stole most of this out of pline, so a lot of it will need to be tweeked.  The prompting
//*** should be right, and the rest is just a wild guess as to how this might work. 5/5/97
/*
	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	struct cmd_mline_globalpacket *gpak;
	struct cmd_mline_list *mlinetmp=NULL;
	struct resbuf	rbSetGet,
				   *prbNewEnt=NULL,
				   *prbResult=NULL,
				   *rbTemp,
					rbucs,
					rbwcs,
					rbLast;
	char			szMisc1[IC_ACADBUF],
					szMisc2[IC_ACADBUF],
					szKeyWord[IC_ACADBUF];
	sds_point		ptFirst,
					pt[3],
					ptTemp;
	sds_real		rCurElev,		// Current elevation.  The whole Mline is drawn at the current elevation.
					fr1,
					rScale,
					first_lastang;
	short			ret=RTNORM,		// Return value for the function.
					nAlignment=1,	// 0 = Aligns with most positive line, 1 = Aligns along zero position, 1 = , 2 = Aligns with most negative line.
					swClosed=0,		//
					swFollow=0;		//
	long			rc=0L;

	//*** Initialize global variables.
	gpak=&cmd_mline_gpak;
	cmd_mlinebeg=cmd_mlinecur=NULL;
	gpak->numents=0; gpak->bulge=0.0;
	rbucs.restype=rbwcs.restype=RTSHORT;
	rbucs.resval.rint=1;
	rbwcs.resval.rint=0;

	sds_printf(ResourceString(IDC_CMDS3__NTHE_MLINE_FUNC_436, "\nThe MLine function is under construction.  It prompts, but only draws polylines." ));
	if (SDS_getvar(NULL,DB_QISLASTCMDPT,&rbLast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { rc=(-__LINE__); goto bail; }
	for (;;) {
		if (rbLast.resval.rint){
			if (sds_initget(0,ResourceString(IDC_CMDS3_FOLLOW_ALIGN_SCA_437, "Follow Align Scale Style|STyle ~Justification" ))!=RTNORM) { rc=(-__LINE__); goto bail; }
			if ((ret=internalGetpoint(NULL,ResourceString(IDC_CMDS3__NMULTILINE___EN_438, "\nMultiLine:  ENTER to use last point/Follow/Align/Scale/STyle/<Start point>: " ),pt[0]))==RTCAN || ret==RTERROR) goto bail;
		}else{
			if (sds_initget(0,ResourceString(IDC_CMDS3_ALIGN_SCALE_STYL_439, "Align Scale Style|STyle ~Justification" ))!=RTNORM) { rc=(-__LINE__); goto bail; }
			if ((ret=internalGetpoint(NULL,ResourceString(IDC_CMDS3__NMULTILINE___AL_440, "\nMultiLine:  Align/Scale/STyle/<Start point>: " ),pt[0]))==RTCAN || ret==RTERROR) goto bail;
		}
		if (ret==RTNORM) break;
		if (ret==RTERROR)
			goto bail;
		if (ret==RTKWORD) {
			ret=sds_getinput(szKeyWord);
			if ((stricmp(szKeyWord,ResourceString(IDC_CMDS3_ALIGN_441, "Align" ))==0) || (stricmp(szKeyWord,ResourceString(IDC_CMDS3_JUSTIFICATION_442, "Justification" ))==0)) {
				if (SDS_getvar(NULL,DB_QCMLJUST,&rbSetGet,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { rc=(-__LINE__); goto bail; }	// Get current MLine justification
				nAlignment=rbSetGet.resval.rint;
				if (nAlignment==0) {				// These values correspond to CMLJUST, for consistency
					sprintf(szMisc1,ResourceString(IDC_CMDS3_POSITIVE_443, "Positive" ));
				} else if (nAlignment==1) {
					sprintf(szMisc1,ResourceString(IDC_CMDS3_ZERO_444, "Zero" ));
				} else if (nAlignment==2) {
					sprintf(szMisc1,ResourceString(IDC_CMDS3_NEGATIVE_445, "Negative" ));
				}
				if (sds_initget(0,ResourceString(IDC_CMDS3_POSITIVE_LINE_PO_446, "Positive_line|Positive Zero_position|Zero Negative_line|Negative ~Top ~Bottom" ))!=RTNORM) { rc=(-__LINE__); goto bail; }
				sprintf(szMisc2,ResourceString(IDC_CMDS3__NALIGN_MULTILIN_447, "\nAlign MultiLine along: Positive line/Zero position/Negative line/<%s>: " ),szMisc1);
				if ((ret=sds_getkword(szMisc2,szKeyWord))==RTCAN || ret==RTERROR) goto bail;
				if (ret==RTNONE) {
					strcpy(szKeyWord,szMisc1);
				}
		if ((strisame(szKeyWord,ResourceString(IDC_CMDS3_POSITIVE_443, "Positive" ))) || (strisame(szKeyWord,ResourceString(IDC_CMDS3_TOP_448, "Top" )))) {
					nAlignment=1;
		} else if (strisame(szKeyWord,ResourceString(IDC_CMDS3_ZERO_444, "Zero" ))) {
					nAlignment=0;
		} else if ((strisame(szKeyWord,ResourceString(IDC_CMDS3_NEGATIVE_445, "Negative" ))) || (strisame(szKeyWord,ResourceString(IDC_CMDS3_BOTTOM_449, "Bottom" )))) {
					nAlignment=2;
				}
				rbSetGet.restype=RTSHORT;
				rbSetGet.resval.rint=nAlignment;
				sds_setvar("CMLJUST"DNT,&rbSetGet);
			} else if (stricmp(szKeyWord,ResourceString(IDC_CMDS3_SCALE_451, "Scale" ))==0) {
				if (SDS_getvar(NULL,DB_QCMLSCALE,&rbSetGet,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { rc=(-__LINE__); goto bail; }	// Get current MLine scale
				sds_rtos(rbSetGet.resval.rreal,-1,-1,szMisc1);
				sprintf(szMisc2,ResourceString(IDC_CMDS3__NSCALE_FACTOR_F_452, "\nScale factor for MultiLine <%s>: " ),szMisc1);
				if ((ret=sds_getreal(szMisc2,&rScale))==RTCAN || ret==RTERROR) goto bail;
				if (ret!=RTNONE) {				// If user hits Enter, don't mess with this.
					rbSetGet.restype=RTREAL;
					rbSetGet.resval.rreal=rScale;
					sds_setvar("CMLSCALE"DNT,&rbSetGet);
				}
			} else if (stricmp(szKeyWord,ResourceString(IDC_CMDS3_STYLE_454, "STyle" ))==0) {
				if (SDS_getvar(NULL,DB_QCMLSTYLE,&rbSetGet,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { rc=(-__LINE__); goto bail; }	// Get current MLine scale
				strncpy(szMisc1,rbSetGet.resval.rstring,sizeof(szMisc1)-1);
				free(rbSetGet.resval.rstring);
				for(;;) {
					sprintf(szMisc2,ResourceString(IDC_CMDS3__NMULTILINE_STYL_455, "\nMultiLine style:  ? to list available styles/ <%s>: " ),szMisc1);
					if (sds_initget(0,ResourceString(IDC_CMDS3_LIST_MULTILINE_S_456, "List_MultiLine_styles|?" ))!=RTNORM) { rc=(-__LINE__); goto bail; }
					if ((ret=sds_getstring(0,szMisc2,szMisc1))==RTERROR) {
						rc=(-__LINE__); goto bail;
					} else if (ret==RTCAN) {
						rc=(-1L); break;
					}
					if (!*szMisc1) break;
					if (rbTemp!=NULL) { rc=(-__LINE__); goto bail; }
					if (strcmp(ResourceString(IDC_CMDS3___62, "?" ),szMisc1)==0) {
						if ((rc=cmd_mlstyle_list())!=0) goto bail;
						sds_printf("\n");
						continue;
				   } else if ((rbTemp=sds_tblsearch("MLSTYLE"DNT,szMisc1,0))==NULL) {	  // NOTE:	I'M GUESSING AT "MLSTYLE"!	MAKE SURE IT'S RIGHT.
						sds_printf(ResourceString(IDC_CMDS3__NMULTILINE_STYL_458, "\nMultiLine style %s not found. " ),szMisc1);
						continue;
					}
					IC_RELRB(rbTemp);
					// TODO - find out what styles are valid, and how to save them back.
				}
			} else if (rbLast.resval.rint && (stricmp(szKeyWord,ResourceString(IDC_CMDS3_FOLLOW_459, "Follow" ))==0)) {		//follow mode
				swFollow=1;
				SDS_getvar(NULL,DB_QLASTCMDPT,&rbLast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				ic_ptcpy(ptFirst,rbLast.resval.rpoint);
				ic_ptcpy(pt[0],rbLast.resval.rpoint);
			} else if (rbLast.resval.rint && ret==RTNONE) {								//just continue from last point
				SDS_getvar(NULL,DB_QLASTCMDPT,&rbLast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				ic_ptcpy(ptFirst,rbLast.resval.rpoint);
				ic_ptcpy(pt[0],rbLast.resval.rpoint);
			} else {
				swFollow=0;
				if (!rbLast.resval.rint && ret==RTNONE) goto bail;
				ic_ptcpy(ptFirst,pt[0]);
			}
		}		// end of RTKWORD
	}			// end of for(;;) loop

	rCurElev=ptFirst[2];

	first_lastang=rbLast.resval.rreal;

	//*** Get the rest of the points.
	for(;;) {
		SDS_getvar(NULL,DB_QISLASTCMDPT,&rbLast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		if (gpak->numents>1){
			//allow Follow, Undo and Close
			if (sds_initget(SDS_RSG_NOCANCEL,ResourceString(IDC_CMDS3_CLOSE_FOLLOW_LEN_460, "Close Follow Length ~ Undo" ))!=RTNORM) { rc=(-__LINE__); goto bail; }
			sprintf(szMisc1,ResourceString(IDC_CMDS3__NCLOSE___FOLLOW_461, "\nClose/Follow/Length/Undo/<Next point>: " ));
		}else if (rbLast.resval.rint && gpak->numents>0) {
			//allow only Follow and Undo
			if (sds_initget(SDS_RSG_NOCANCEL,ResourceString(IDC_CMDS3__CLOSE___FOLLOW__462, "~Close ~ Follow Length ~ Undo" ))!=RTNORM) { rc=(-__LINE__); goto bail; }
			sprintf(szMisc1,ResourceString(IDC_CMDS3__NFOLLOW___LENGT_463, "\nFollow/Length/Undo/<Next point>: " ));
		} else if (rbLast.resval.rint){
			//allow only Follow
			if (sds_initget(SDS_RSG_NOCANCEL,ResourceString(IDC_CMDS3_FOLLOW_LENGTH___464, "Follow Length" ))!=RTNORM) { rc=(-__LINE__); goto bail; }
			sprintf(szMisc1,ResourceString(IDC_CMDS3__NFOLLOW___LENGT_465, "\nFollow/Length/<Next point>: " ));
		}else{
			//allow none of 'em
			if (sds_initget(0,ResourceString(IDC_CMDS3_LENGTH_466, "Length" ))!=RTNORM) { rc=(-__LINE__); goto bail; }
			sprintf(szMisc1,ResourceString(IDC_CMDS3__NLENGTH____NEXT_467, "\nLength/<Next point>: " ));
		}
		if (swFollow!=1){
			ret=internalGetpoint(pt[0],szMisc1,pt[1]);
		}else{
			if (prbResult!=NULL){sds_relrb(prbResult);prbResult=NULL;}
			SDS_getvar(NULL,DB_QLASTCMDANG,&rbLast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			ret=SDS_dragobject(2,1,pt[0],pt[1],rbLast.resval.rreal,szMisc1,&prbResult,ptTemp,szMisc2);
			if (ret==RTNORM){
				ic_assoc(prbResult,11);
				ic_ptcpy(pt[1],ic_rbassoc->resval.rpoint);
				if (prbResult!=NULL){sds_relrb(prbResult);prbResult=NULL;}
			}
		}
		pt[1][2]=ptFirst[2];
		if (ret==RTERROR) {
			rc=(-__LINE__); goto bail;
		} else if (ret==RTCAN) {
			rc=(-1L); goto bail;
		} else if (ret==RTNONE) {
			rc=0L; goto bail;
		} else if (ret==RTNORM) {
			//*** Make sure they entered a valid 2D point.
			if (pt[1][2]!=ptFirst[2]){ //don't check if pt[1][2]!=rCurElev
				cmd_ErrorPrompt(CMD_ERR_2DPTKWORD,0);
				continue;
			}
			if ((rc=cmd_mline_addsegment(pt[0],pt[1],rCurElev))!=0L) goto bail;
			swFollow=0;
		} else if (ret==RTKWORD) {
			if (!swFollow) {
				if (sds_getinput(szMisc1)!=RTNORM) { rc=(-__LINE__); goto bail; }
			}else{
				strncpy(szMisc1,szMisc2,sizeof(szMisc2)-1);
				swFollow=0;
			}
			strupr(szMisc1);
			if (strcmp(szMisc1,ResourceString(IDC_CMDS3_CLOSE_468, "CLOSE" ))==0) {

				//*** ACAD seems to set the flag even if there are only 2 vertexes.
				if ((swClosed & 1)==0)swClosed+=1;
				rc=0L; goto bail;
			} else if (strcmp(szMisc1,ResourceString(IDC_CMDS3_LENGTH_469, "LENGTH" ))==0) {
				for(;;) {
					if ((ret=sds_getdist(pt[0],ResourceString(IDC_CMDS3__NLENGTH_OF_MULT_470, "\nLength of multiline: " ),&fr1))==RTERROR) {
						{ rc=(-__LINE__); goto bail; }
					} else if (ret==RTCAN) {
						rc=(-1L); goto bail;
					} else if (ret==RTNORM) {
						break;
					} else {
						cmd_ErrorPrompt(CMD_ERR_DIST2NDPT,0);
						continue;
					}
				}
				SDS_getvar(NULL,DB_QLASTCMDANG,&rbLast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				sds_polar(pt[0],rbLast.resval.rreal,fr1,pt[1]);
				if ((rc=cmd_mline_addsegment(pt[0],pt[1],rCurElev))!=0) goto bail;
			} else if (strcmp(szMisc1,ResourceString(IDC_CMDS3_UNDO_471, "UNDO" ))==0) {
				if (gpak->numents<1) {
					cmd_ErrorPrompt(CMD_ERR_NOMOREUNDO,0);
					continue;
				}
				mlinetmp=cmd_mlinecur;
				//*** Erase entity.
				if (sds_entdel(mlinetmp->ename)!=RTNORM) { rc=(-__LINE__); goto bail; }
				//*** Set pt[1] in case user presses ENTER right after an undo.
				ic_ptcpy(pt[0],mlinetmp->vertexpt);
				ic_ptcpy(pt[1],pt[0]);
				//*** Reset LASTCMDPT,ISLASTCMDPT variable.
				rbLast.restype=RT3DPOINT;
				ic_ptcpy(rbLast.resval.rpoint,pt[0]);
				//TODO - assure that pt[0] is a WCS point

				SDS_setvar(NULL,DB_QLASTCMDPT,&rbLast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
				rbLast.restype=RTSHORT;
				rbLast.resval.rint=1;
				SDS_setvar(NULL,DB_QISLASTCMDPT,&rbLast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
				//*** Reset start and ending mline widths.
				gpak->stwid=mlinetmp->stwidth;
				gpak->endwid=mlinetmp->endwidth;
				//***
				if (cmd_mlinecur->prev==NULL) {
					cmd_mlinebeg=cmd_mlinecur=NULL;
				} else {
					cmd_mlinecur=cmd_mlinecur->prev;
					cmd_mlinecur->next=NULL;
				}
				IC_FREE(mlinetmp);
				//*** Set lastangle global variable.
				rbLast.restype=RTREAL;
				if (cmd_mlinecur!=NULL) rbLast.resval.rreal=cmd_mlinecur->lastang;
				else rbLast.resval.rreal=first_lastang;
				SDS_setvar(NULL,DB_QLASTCMDANG,&rbLast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
				--gpak->numents;
				continue;
			} else {
				cmd_ErrorPrompt(CMD_ERR_PTKWORD,0);
				continue;
			}
		} else return(ret);
		//*** Set pt[0]=pt[1] for next internalGetpoint.
		ic_ptcpy(pt[0],pt[1]);
	}

	bail:

	if (rc>=(-1L) && cmd_mlinebeg!=NULL) {
		//*** Calculate the bulge for the last vertex
		//don't forget to set lastpt & LASTANGLE if we're closing....
		//TODO - assure we're using a UCS point for setvar
		SDS_getvar(NULL,DB_QLASTCMDPT,&rbLast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		if (swClosed & 1)ic_ptcpy(rbLast.resval.rpoint,cmd_mlinebeg->vertexpt);
		gpak->bulge=0.0;
		if (!icadPointEqual(pt[0],cmd_mlinebeg->vertexpt)) // && cmd_mlinecur!=cmd_mlinebeg)
			//if user input closing point isn't exactly on beginning pt
		if (swClosed & 1){
			fr1=sds_angle(pt[0],rbLast.resval.rpoint);
			rbLast.resval.rreal=fr1;	//use dummy var in case
		}
		rbLast.restype=RTREAL;
		SDS_setvar(NULL,DB_QLASTCMDANG,&rbLast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
	}

	 //*** Create multiline.

		//	You will need:
		//		nAlignment	(0=Zero, 1=Positive, -1=Negative)
		//		rScale		(The scaling factor)
		//		CMLSTYLE	(The variable for the current Multiline style)


********************This is stolen straight out of polyline, just as an example.  Delete if not useful. ************************************
		ptTemp[0]=0.0; ptTemp[1]=0.0; ptTemp[2]=rCurElev;
		if ((prbNewEnt=sds_buildlist(RTDXF0,"POLYLINE"DNT,66,1,70,swClosed,10,ptTemp,0))==NULL) {
			cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
			rc=(-1L); goto bail;
		}
		if (ThreeDmode) { //*** 3D Polyline
			ic_assoc(prbNewEnt,70);
			ic_rbassoc->resval.rint+=8;
		}
		if (sds_entmake(prbNewEnt)!=RTNORM) { rc=(-__LINE__); goto bail; }
		IC_RELRB(prbNewEnt);
		//*** Create multiline vertices.
		for(mlinetmp=cmd_mlinecur=cmd_mlinebeg; cmd_mlinecur!=NULL; mlinetmp=cmd_mlinecur) {
			if ((prbNewEnt=sds_buildlist(RTDXF0,"VERTEX"DNT,10,cmd_mlinecur->vertexpt,40,cmd_mlinecur->stwidth,
			   41,cmd_mlinecur->endwidth,42,cmd_mlinecur->bulge,0))==NULL) {
				cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
				rc=(-1); goto bail;
			}
			if (ThreeDmode) { //*** 3D multiline vertex
				ic_assoc(prbNewEnt,42);
				if ((ic_rbassoc->rbnext=sds_buildlist(70,32,0))==NULL) {
					cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
					rc=(-1); goto bail;
				}
			}
			if (sds_entmake(prbNewEnt)!=RTNORM) { rc=(-__LINE__); goto bail; }
			IC_RELRB(prbNewEnt);
			//*** Delete temporary mline segment and free link.
			if (sds_entdel(cmd_mlinecur->ename)!=RTNORM) { rc=(-__LINE__); goto bail; }
			cmd_mlinecur=cmd_mlinecur->next;
			IC_FREE(mlinetmp);
		}
		mlinetmp=cmd_mlinecur=cmd_mlinebeg=NULL;
		//*** Create last vertex.
		if ((prbNewEnt=sds_buildlist(RTDXF0,"VERTEX"DNT,10,pt[0],40,gpak->stwid,41,gpak->stwid,42,gpak->bulge,0))==NULL) {
			cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
			rc=(-1); goto bail;
		}
		if (sds_entmake(prbNewEnt)!=RTNORM) { rc=(-__LINE__); goto bail; }
		IC_RELRB(prbNewEnt);
		//*** Sequence end.
		prbNewEnt=sds_buildlist(RTDXF0,"SEQEND"DNT,0);
		if (sds_entmake(prbNewEnt)!=RTNORM) { rc=(-__LINE__); goto bail; }
		IC_RELRB(prbNewEnt);
		//*** Set PLINEWID system variable.
		rbSetGet.restype=RTREAL;
		rbSetGet.resval.rreal=gpak->endwid;
		if (sds_setvar("PLINEWID" ),&rbSetGet)!=RTNORM) { rc=(-__LINE__); goto bail; }
	} else {
		for(cmd_mlinecur=mlinetmp=cmd_mlinebeg; cmd_mlinecur!=NULL; mlinetmp=cmd_mlinecur) {
			//*** Delete tmp mline segment.
			if (sds_entdel(cmd_mlinecur->ename)!=RTNORM) { rc=(-__LINE__); goto bail; }
			cmd_mlinecur=cmd_mlinecur->next;
			IC_FREE(mlinetmp);
		}
	}
*************************************End of stolen part *******************************************************
	sds_entmake(NULL);
	IC_RELRB(prbNewEnt); IC_RELRB(gpak->arcdxf);
	if (rc==0L) return(RTNORM);
	if (rc==(-1L)) return(RTCAN);
	if (rc<(-1L)) CMD_INTERNAL_MSG(-rc);
	return(RTERROR);
}
*/

long cmd_mline_addsegment(sds_point pt1, sds_point pt2, sds_real rCurElev) {
//*** This function adds a multiline segment.
//*** Right now it is just doing plines, without the arc options.
//*** Replace this with whatever works.
//***
	struct cmd_mline_globalpacket *gpak;
	struct resbuf *prbNewEnt,rbLast;
	struct cmd_mline_list *mlinetmp;
	sds_point pt[2],ptTemp;
	sds_name entlast;
	short fi1;
	long rc=0L;

	gpak=&cmd_mline_gpak;
	prbNewEnt=NULL; rc=0;
	ic_ptcpy(pt[0],pt1); ic_ptcpy(pt[1],pt2);
	//*** Set LASTCMDPT,ISLASTCMDPT variable.
	rbLast.restype=RT3DPOINT;
	ic_ptcpy(rbLast.resval.rpoint,pt[1]);
	//TODO - assure we're adding a UCS point
	SDS_setvar(NULL,DB_QLASTCMDPT,&rbLast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
	rbLast.restype=RTSHORT;
	rbLast.resval.rint=1;
	SDS_setvar(NULL,DB_QLASTCMDPT,&rbLast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);

	gpak->bulge=0.0;
	//*** Set lastangle global variable.
	rbLast.resval.rreal=sds_angle(pt[0],pt[1]);

	rbLast.restype=RTREAL;
	SDS_setvar(NULL,DB_QLASTCMDANG,&rbLast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
	//*** Create temporary multiline segment.
	ptTemp[0]=0.0; ptTemp[1]=0.0; ptTemp[2]=rCurElev;
	prbNewEnt=sds_buildlist(RTDXF0,"POLYLINE"/*DNT*/,66,1,10,ptTemp,0);
	if (sds_entmake(prbNewEnt)!=RTNORM) {
		rc=(-__LINE__);
		goto bail;
	}
	IC_RELRB(prbNewEnt);
	//*** Create multiline vertices.
	for(fi1=0; fi1<2; fi1++) {
		prbNewEnt=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,10,pt[fi1],40,gpak->stwid,41,gpak->endwid,42,gpak->bulge,0);
		if (sds_entmake(prbNewEnt)!=RTNORM) {
			rc=(-__LINE__);
			goto bail;
		}
		IC_RELRB(prbNewEnt);
	}
	//*** Sequence end.
	prbNewEnt=sds_buildlist(RTDXF0,"SEQEND"/*DNT*/,0);
	if (sds_entmake(prbNewEnt)!=RTNORM) {
		rc=(-__LINE__);
		goto bail;
	}
	IC_RELRB(prbNewEnt);

	sds_entlast(entlast);

	//*** Allocate new segment for linked list.
	if ((mlinetmp= new struct cmd_mline_list)==NULL) {
		cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
		rc=(-__LINE__);
		goto bail;
	}
	memset(mlinetmp,0,sizeof(struct cmd_mline_list));
	if (cmd_mlinebeg==NULL) {
		cmd_mlinebeg=cmd_mlinecur=mlinetmp;
	} else {
		mlinetmp->prev=cmd_mlinecur;
		cmd_mlinecur=cmd_mlinecur->next=mlinetmp;
	}
	ic_encpy(cmd_mlinecur->ename,entlast);
	ic_ptcpy(cmd_mlinecur->vertexpt,pt[0]);
	cmd_mlinecur->stwidth=gpak->stwid;
	cmd_mlinecur->endwidth=gpak->endwid;
	cmd_mlinecur->bulge=gpak->bulge;
	SDS_getvar(NULL,DB_QLASTCMDANG,&rbLast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	cmd_mlinecur->lastang=rbLast.resval.rreal;
	//***
	gpak->numents++; gpak->stwid=gpak->endwid;

	bail:

	IC_RELRB(prbNewEnt);
	return(rc);
}

long cmd_mlstyle_list(void) {
// (prompting) 9 May, 1997
//*** This function lists defined MultiLineSstyle information.
//***
//*** NOTE: This function does not list in alphabetical order.
//***

//*************************  This is based on cmd_layer_list(), for reference. **************************
// Can all these listing functions (layer, linetype, xref, etc.) call the same code?  Or are they too different???
// Remember... this is stolen from cmd_layer_list.	It will need some fundamental changes to work right.
// This stuff is just here as a beginning, and the commented stuff is just for examples.

	char		//	fs1[IC_ACADBUF],
					szMLineStyle[IC_ACADBUF],
				   *fcp1,
				   *fcp2,
					fc1;
//	  char			szScrollLines[IC_ACADBUF];
	struct resbuf  *rbTemp=NULL,
				   *rbBegList=NULL,
				   *rbCurList=NULL,
				   *rbTempList;
//					rbSetGet;
	int				nRet,
					nScroll=0;
	short			nNumLines=0;
//					fi1;
	long			lErrorLine=0L;

	if ((nRet=sds_getstring(0,ResourceString(IDC_CMDS3__NMULTILINE_STYL_473, "\nMultiLine Styles to list <*>: " ),szMLineStyle))==RTERROR) {
		lErrorLine=(-__LINE__);
		goto bail;
	} else if (nRet==RTCAN) {
		lErrorLine=(-1L);
		goto bail;
	}
//	if (SDS_getvar(NULL,DB_QCLAYER,&rbSetGet,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM){lErrorLine=(-__LINE__); goto bail;}
//	strncpy(curlay,rbSetGet.resval.rstring,sizeof(curlay)-1);

//	  strupr(szMLineStyle); sds_textscr();
	sds_printf(ResourceString(IDC_CMDS3__N__LOADED_MULTI_474, "\n  Loaded MultiLine Styles: " ));
	sds_printf(ResourceString(IDC_CMDS3__N_N__MULTILINE__475, "\n\n  MultiLine Style          Description                                " ));
	sds_printf(ResourceString(IDC_CMDS3__N_______________476, "\n-------------------------  ---------------------------------------------" ));

	//*** ~| TODO - Some day I need to redo the way this is being done with a call to sds_wcmatch().
	if (!*szMLineStyle || (szMLineStyle[0]=='*'/*DNT*/ && szMLineStyle[1]==0)) {
		if ((rbBegList=rbCurList=sds_buildlist(RTSTR,"*"/*DNT*/,0))==NULL) {
			cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
			lErrorLine=(-__LINE__);
			goto bail;
		}
	} else {
		for(fcp1=fcp2=szMLineStyle; *fcp2!=0; fcp1=fcp2+1) {
			for(fcp2=fcp1; *fcp2!=',' && *fcp2!=0; fcp2++);
			fc1=*fcp2;
			*fcp2=0;
			if ((rbTempList=sds_buildlist(RTSTR,fcp1,0))==NULL) {
				cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
				lErrorLine=(-__LINE__);
				goto bail;
			}
			if (rbBegList==NULL) {
				rbBegList=rbCurList=rbTempList;
			} else {
				rbCurList=rbCurList->rbnext=rbTempList;
			}
			*fcp2=fc1;
		}
	}
//	MultiLine Styles are not accessable using tables.  I don't know how you get to them.

/*	if ((rbTemp=sds_tblnext("LAYER"DNT,1))==NULL) { lErrorLine=(-__LINE__); goto bail; }
	while(rbTemp!=NULL) {
		if (sds_usrbrk()) { lErrorLine=(-1L); goto bail; }
		//*** Layer names.
		if (ic_assoc(rbTemp,2)==0) {
			for(rbTempList=rbBegList; rbTempList!=NULL; rbTempList=rbTempList->rbnext) {
				if (sds_wcmatch(ic_rbassoc->resval.rstring,rbTempList->resval.rstring)==RTNORM) {
					strncpy(fs1,ic_rbassoc->resval.rstring,sizeof(fs1)-1);
					if (ic_assoc(rbTemp,6)!=0) { lErrorLine=(-__LINE__); goto bail; }
//					  sds_printf("\n%-31.31s %-12.12s %-12.12s %-7.7s",
//						  fs1,lstate,ic_rbassoc->resval.rstring,strupr(ic_colorstr(cecolor,NULL)));
					nNumLines++;
					if (nNumLines>=20 && !nScroll) {
						sds_initget(0,ResourceString(IDC_CMDS3_SCROLL_100, "Scroll" ));
						if ((nRet=sds_getkword(ResourceString(IDC_CMDS3__NSCROLL____ENTE_101, "\nScroll/<ENTER to view next screen>:" ),szScrollLines))==RTERROR) {
							lErrorLine=(-__LINE__); goto bail;
						}else if (nRet==RTCAN) {
							lErrorLine=(-1L); goto bail;
						}else if (nRet==RTNONE) {
							nNumLines=0;
						}else if (nRet==RTNORM) {
							nScroll=1;
						}
					}
				}
			}
		}
		IC_RELRB(rbTemp);
		//*** Do NOT check for NULL here.  NULL is checked in the while().
		rbTemp=sds_tblnext("LAYER"DNT,0);
	} */
	bail:

	sds_printf(ResourceString(IDC_CMDS3__NLISTING_MULTIL_477, "\nListing MultiLine Styles is under construction. \n\n" ));	// Take this out later.
//	  IC_RELRB(rbTemp);
//	  IC_RELRB(rbBegList);
	if (lErrorLine==0L)
		return(RTNORM);
	if (lErrorLine==(-1L))
		return(RTCAN);
	if (lErrorLine<(-1L))
		CMD_INTERNAL_MSG(lErrorLine);
	return(nRet);
}



short cmd_mledit(void)	 {
	cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0);
	SDS_FreeEventList(0);
	return(RTNORM);
}
// (prompting) 9 May, 1997
// The dialog that handles getting the edit mode is ID=ML_MULTILINEEDIT, or Icadmleditdia.cpp
// After the dialog gets the edit mode, it goes to cmd_mledit_prompts(), below.

/*	sds_printf("\nThe MLEdit function is under construction.  It displays a dialog box, but has no effect.");

	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	  ExecuteUIAction( ICAD_WNDACTION_MLEDIT );
	return(RTNORM);
}
*/



//*******************Notes for whoever writes the guts of MLEdit******************************************
// AutoCAD's MLEdit makes you look at the command line to see if you've actually selected an MLine.
// We could be a lot easier to use by highlighting the line when it is selected.  Also, their selection
// mechanism is really picky.  I don't think it uses the same aperture as the others, but I can't think
// why they would make it different.
// I have only included the options that AutoCAD has.  Are there others we should add?
// They use the word "weld", and I'm using "Heal", because I think it only applies to MLines that once
// were one, and got cut up by either a Cut command or by doing crosses, tees, etc.  If it is supposed to
// work with MLines that were not originally  the same line, then "Weld" or something like it might be more
// appropriate.
//
// There are three basic ways of editing MLines:  Intersecting two of them; adding or deleting vertices;
// and opening or closing gaps in them.  The main function gets information from the user, then passes it
// to one of three others, below.
//*******************************************************************************************************************
int cmd_mledit_prompts(int nMLEditType)  {			// Multiline editing function.

	int			nRet=RTNORM;		// This function's return value.
	sds_name	nmFirst,			// The entity name for the first MultiLine the user picks.
				nmSecond;			// The entity name for the second MultiLine the user picks.
	sds_point	ptFirst,			// Point by which the first MultiLine was selected.
				ptSecond;			// Point by which the second MultiLine was selected.
	short		swFirstPick=1;		// 0 = Not the first pick, 1 = First time user is going through prompts.

		swFirstPick=1;
		switch (nMLEditType){
					//					--	Crosses & tees will use similar code, and same prompting.			--
			case	MLEDIT_CLOSEDX:			//	Closed Cross	-- Second mline crosses "over" first, like a bridge on a map.	 --
			case	MLEDIT_OPENX:			//	Open Cross		-- Mlines cross with openings in both, like streets on a map.	 --
			case	MLEDIT_MERGEDX:			//	Merged Cross	-- Mlines cross with openings, but inside lines continue across. --

					//					--	For tees, the first mline picked is the upright part of the tee.	--
					//					--	The second mline is the cross part of the tee.						--
					//					--	Tee keeps first part picked, trims off opposite end of first mline.	--
			case	MLEDIT_CLOSEDT:			//	Closed Tee		-- First mline butts into side of second one, and ends there. --
			case	MLEDIT_OPENT:			//	Open Tee		-- First mline opens into side of second one, and ends. 	  --
			case	MLEDIT_MERGEDT:			//	Merged Tee		-- Like Open tee, but inner lines continue on into tee. 	  --

			case	MLEDIT_CORNER:			//	Corner Joint	-- Keeps the ends you picked.  Trims off other ends.			  --
					// This keeps asking for more pairs of mlines until the user is done (ENTER)
					for (;;) {
						// get first mline
						// initget that allows selection, cancel, or ENTER to end command
						if (swFirstPick==1) {
							if (sds_initget(RSG_OTHER,""/*DNT*/)!=RTNORM)
								return(RTERROR);
							nRet=sds_entsel(ResourceString(IDC_CMDS3__NSELECT_FIRST_M_478, "\nSelect first MultiLine: " ),nmFirst,ptFirst);
						} else {
							if (sds_initget(RSG_OTHER, ResourceString(IDC_MLEDIT_UNDO_INIT1, "Undo ~ ` ~_Undo ~_"))!=RTNORM)
								return(RTERROR);
							nRet=sds_entsel(ResourceString(IDC_CMDS3__NUNDO___SELECT__480, "\nUndo/Select first MultiLine: " ),nmFirst,ptFirst);
						}
						if (nRet==RTCAN || nRet==RTERROR)
							return(nRet);
						if (nRet==RTNORM) {
							// get second mline
							// initget that allows selection, cancel, or ENTER to end command
							nRet=sds_entsel(ResourceString(IDC_CMDS3__NSELECT_SECOND__481, "\nSelect second MultiLine: " ),nmSecond,ptSecond);
							if (nRet==RTCAN || nRet==RTERROR)
								return(nRet);
							if (nRet==RTNORM) {
								// send the selected mlines and pickpoints to the Cross/Tee/Corner funtion:
								nRet=cmd_mledit_intersections(nMLEditType,nmFirst,ptFirst,nmSecond,ptSecond);
								if (nRet!=RTNORM)
									return(nRet);
								swFirstPick=0;		// So we go through the right set of prompts
							}
						}
					}
					break;

			case	MLEDIT_ADDVERT:			//	Add Vertex		-- Adds a vertex, but doesn't give opportunity to move it around. --
			case	MLEDIT_DELVERT:			//	Delete Vertex	-- Deletes vertex.	Sometimes trims, sometimes straightens.  More research...  --
					// This keeps asking for points on mlines until the user is done (ENTER)
					for (;;) {
						// get first mline, and point at which to add or delete a vertex
						// initget that allows selection, cancel, or ENTER to end command
						if (swFirstPick==1) {
							if (sds_initget(RSG_OTHER,""/*DNT*/)!=RTNORM)
								return(RTERROR);
							nRet=sds_entsel(ResourceString(IDC_CMDS3__NSELECT_POINT_O_482, "\nSelect point on MultiLine: " ),nmFirst,ptFirst);
						} else {
							if (sds_initget(SDS_RSG_OTHER,ResourceString(IDC_MLEDIT_UNDO_INIT1, "Undo ~ ` ~_Undo ~_" ))!=RTNORM)
								return(RTERROR);
							nRet=sds_entsel(ResourceString(IDC_CMDS3__NUNDO___SELECT__483, "\nUndo/Select point on MultiLine: " ),nmFirst,ptFirst);
						}
						if (nRet==RTCAN || nRet==RTERROR)
							return(nRet);
						if (nRet==RTNORM) {
							nRet=cmd_mledit_vertices(nMLEditType,nmFirst,ptFirst);
							if (nRet!=RTNORM)
								return(nRet);
							swFirstPick=0;		// So we go through the right set of prompts
						}
					}
					break;


			case	MLEDIT_CUTSINGLE:		//	Cut Single		-- Cuts an opening between two points in one side of an mline. --
			case	MLEDIT_CUTALL:			//	Cut All			-- Cuts an opening between two points through all parts of an mline. --
			case	MLEDIT_HEAL:			//	Heal cuts		-- "Heals" cuts between two points - joins all parts back together. --
					// This keeps asking for sets of points on mlines until the user is done (ENTER)
					for (;;) {
						// get first point on an mline
						// initget that allows selection, cancel, or ENTER to end command
						if (swFirstPick==1) {
							if (sds_initget(RSG_OTHER,""/*DNT*/)!=RTNORM)
								return(RTERROR);
							nRet=sds_entsel(ResourceString(IDC_CMDS3__NSELECT_FIRST_P_484, "\nSelect first point on MultiLine: " ),nmFirst,ptFirst);
						} else {
							if (sds_initget(SDS_RSG_OTHER, ResourceString(IDC_MLEDIT_UNDO_INIT1, "Undo ~ ` ~_Undo ~_"))!=RTNORM)
								return(RTERROR);
							nRet=sds_entsel(ResourceString(IDC_CMDS3__NUNDO___SELECT__485, "\nUndo/Select first point on MultiLine: " ),nmFirst,ptFirst);
						}
						if (nRet==RTCAN || nRet==RTERROR)
							return(nRet);
						if (nRet==RTNORM) {
							// Get second point on the mline.  Cut or Heal between them.
							// initget that allows selection, cancel, or ENTER to end command
							nRet=sds_entsel(ResourceString(IDC_CMDS3__NSELECT_SECOND__486, "\nSelect second point on MultiLine: " ),nmSecond,ptSecond);
							if (nRet==RTCAN || nRet==RTERROR)
								return(nRet);
							if (nRet==RTNORM) {
								// send the selected mlines and pickpoints to the Cross/Tee/Corner funtion:
								nRet=cmd_mledit_cut_and_heal(nMLEditType,nmFirst,ptFirst,ptSecond);
								if (nRet!=RTNORM)
									return(nRet);
								swFirstPick=0;		// So we go through the right set of prompts
							}
						}
					}
					break;

		}
	return(nRet);
}


int   cmd_mledit_intersections(int nMLEditType, sds_name nmFirst, sds_point ptFirst, sds_name nmSecond, sds_point ptSecond){
// This function gets called by cmd_mledit
// It gets the 2 mlines the user picked, and the points by which they were selected.
	int			nRet=RTNORM;	// This function's return value.

	switch (nMLEditType){

		case	MLEDIT_CLOSEDX:			//	Closed Cross	-- Second mline crosses "over" first, like a bridge on a map.	 --
				break;
		case	MLEDIT_OPENX:			//	Open Cross		-- Mlines cross with openings in both, like streets on a map.	 --
				break;
		case	MLEDIT_MERGEDX:			//	Merged Cross	-- Mlines cross with openings, but inside lines continue across. --
				break;

				//					-- For tees, the first mline picked is the upright part of the tee.		--
				//					-- The second mline is the cross part of the tee.						--
				//					-- Tee keeps first part picked, trims off opposite end of first mline.	--
		case	MLEDIT_CLOSEDT:			//	Closed Tee		-- First mline butts into side of second one, and ends there. --
				break;
		case	MLEDIT_OPENT:			//	Open Tee		-- First mline opens into side of second one, and ends. 	  --
				break;
		case	MLEDIT_MERGEDT:			//	Merged Tee		-- Like Open tee, but inner lines continue on into tee. 	  --
				break;

		case	MLEDIT_CORNER:			//	Corner Joint	-- Keeps ends of first and second ones you pick.  Trims off other ends. --
				break;
	}
	return(nRet);
}

int   cmd_mledit_vertices(int nMLEditType, sds_name nmFirst, sds_point ptFirst) {
// This function gets called by cmd_mledit
// It gets the mline the user picked, and the point where the vertex is to be inserted/deleted.
	int			nRet=RTNORM;	// This function's return value.

	switch (nMLEditType){
		case	MLEDIT_ADDVERT:			//	Add Vertex		-- Adds a vertex, but doesn't give opportunity to move it around. --
				break;
		case	MLEDIT_DELVERT:			//	Delete Vertex	-- Deletes vertex.	Sometimes trims, sometimes straightens.  More research...  --
				break;
	}
	return(nRet);
}


int   cmd_mledit_cut_and_heal(int nMLEditType, sds_name nmFirst, sds_point ptFirst, sds_point ptSecond) {
// This function gets called by cmd_mledit																 ----*	 *----
// It gets the mline the user picked, and the points between which the mline is to be cut or repaired.	 ----*	 *----
	int			nRet=RTNORM;	// This function's return value.

	switch (nMLEditType){
		case	MLEDIT_CUTSINGLE:		//	Cut Single		-- Cuts an opening between two points in one side of an mline.		 --
				break;
		case	MLEDIT_CUTALL:			//	Cut All			-- Cuts an opening between two points through all parts of an mline. --
				break;
		case	MLEDIT_HEAL:			//	Heal cuts		-- "Heals" cuts between two points - joins all parts back together.  --
				break;
	}
	return(nRet);
}

/*====================================================================================================================
The original comment:

	XRef command.

	Notes for whoever writes the guts of XRef:
	Some notes:   (Read the manual, too.)
	On Bind XRef, incoming entity handles are lost, and replaced with new ones to avoid duplication.
	Attached XRefs can be nested.
	One XRef can be attached multiple times, with different insertion points, scales, and rotations.
	Once the XRef is attached, the user can control layer qualities (visibility, color, etc.) from within the drawing.
	XRefs get reloaded on Open or Print (and of course Reload), so they are most recently saved version.
	Circular references are not allowed.  (Good luck with that one!)
	We need an error message when the drawing can't find its XRef(s) on Open (or any other time?).
	XRef info is stored in a log file with the same name as the drawing  DRAWING.XLG

=====================================================================================================================*/

short	cmd_xref(void){
/*========================================================================
Note:
	  This function is created on the basis of the original version
	  in July 2000.
=========================================================================*/
  if (cmd_iswindowopen() == RTERROR) return RTERROR; // Does not exist active drawing

	//BugZilla No. 78050; 25-03-2002
	if (cmd_istransparent() == RTERROR) return RTERROR;

	char *keywords[]={
			 "ATTACH"	/*DNT*/ //key==0
			,"OVERLAY"	/*DNT*/ //key==1
			,"PATH"		/*DNT*/ //key==2
			,"RELOAD"	/*DNT*/ //key==3
			,"DETACH"	/*DNT*/ //key==4
			,"BIND"		/*DNT*/ //key==5
			,"INSERT"	/*DNT*/ //key==6
			,"?"		/*DNT*/ //key==7
			,"_?"		/*DNT*/ //key==8
			,"LIST"		/*DNT*/ //key==9
			,"UNLOAD"	/*DNT*/ //key==10
			, NULL		/*DNT*/
	};
	char	key;	//Index of a keyword in keywords array (to distinguish a key word)
	RT		nRet;	// This function's return value.

	db_setfontsubstfn(NULL);	//suppress font substitution messaging
	// EBATECH(FUTA) 2001-01-07 don't loop like Acad
	//for (;;) { // Loop of keywords reception from the user
		char str[IC_ACADBUF]; // Name of XRef (from dialog box or command line)


		if (RTNORM != sds_initget(0, ResourceString(IDC_XREF_INIT1, "List_External_References|? ~ Attach Overlay ~ Bind Insert ~ Detach Path Reload Unload ~List ~_? ~_ ~_Attach ~_Overlay ~_ ~_Bind ~_Insert ~_ ~_Detach ~_Path ~_Reload ~_Unload ~_List")))
			goto depart;
		nRet = sds_getkword(ResourceString(IDC_CMDS3__NEXTERNAL_REFER_488, "\nExternal References:  ? to list/Bind/Insert/Detach/Path/Reload/Overlay/<Attach>: " ),str);
		if (nRet==RTCAN || nRet==RTERROR)	goto depart;
		if (RTNONE == nRet) { // set "attach" as default
			strcpy(str, keywords[0]);
			nRet=RTNORM;
		}

		// After input of a key word the user will enter ordinary strings
		// (file path or xref name for example) and we does not using
		// entering a keywords:
		if (sds_initget(NULL,""/*DNT*/)!=RTNORM) goto depart;


		// now in "str" is keyword
		strupr(str);
		// To distinguish a key word
		for (key=0; keywords[key] && !strisame(str,keywords[key]); key++);
		// now in "key" is the index of a keyword in keywords array

		switch (key) {
		case 0:  // "Attach"
		case 1:  // "Overlay"
			if ((nRet = xref_add_new(key==1))!=RTNORM) goto depart;
			break;
		case 2:{ // "path"
				// get name of XRef:
				nRet=sds_getstring(1,ResourceString(IDC_CMDS3__NEXTERNALLY_REF_498, "\nExternally referenced drawing(s) to edit path <*>: " ),str);
				if (nRet==RTCAN || nRet==RTERROR) goto depart;
				ic_trim(str,"be");
				if (nRet==RTNORM && !*str)strncpy(str,"*"/*DNT*/,sizeof(str)-1); // all as default
				strupr(str);
				nRet=cmd_xref_editpath(str);
				if (nRet>=0)sds_printf(ResourceString(IDC_CMDS3__N_I__S_UPDATED__499, "\n%i %s updated." ),nRet,(nRet==1)?ResourceString(IDC_CMDS3_REFERENCE_500, "reference" ) : ResourceString(IDC_CMDS3_REFERENCES_501, "references" ));
			break;}
		case 3: { // "reload"
			// get name of XRef:
			nRet=sds_getstring(1,ResourceString(IDC_CMDS3__NEXTERNALLY_REF_503, "\nExternally referenced drawing(s) to reload <*>: " ),str);
			if (nRet==RTCAN	|| nRet==RTERROR) goto depart;
			ic_trim(str,"be");
			if (nRet==RTNORM && !*str)strncpy(str,"*"/*DNT*/,sizeof(str)-1);
			resbuf rb;
			rb.rbnext=NULL;
			rb.restype=RTSTR;// EBATECH(FUTA) 2001-01-07
			rb.resval.rstring=str;
			nRet=xref_reload(&rb);
			if (nRet<0){ nRet=-nRet; goto depart;}
			break;}
		case 4:{ // "detach"
				// EBATECH(FUTA) 2001-01-07 able to use "*"
				//do {
					nRet = sds_getstring(1, ResourceString(IDC_CMDS3__NEXTERNALLY_REF_508, "\nExternally referenced drawing(s) to detach: " ), str);// get name of XRef
					if (nRet==RTCAN || nRet==RTERROR) goto depart;
				//} while (strsame(str, "*"/*DNT*/));
				ic_trim(str,"be");
				strupr(str);
				int	nCt= cmd_xref_uncouple(str, IC_UNCOUPLE_DETACH, true);	// Xref counter

				if (nCt<0){
					nRet=RTERROR;
					goto depart;
				}
				// uncouple erases all inserts of deleted xrefs (can't find 'em after
				// we've deleted the block table record)

				sds_printf(ResourceString(IDC_CMDS3__N_I__S_DETACHED_509, "\n%i %s detached." ),nCt,(nCt==1)?ResourceString(IDC_CMDS3_REFERENCE_500, "reference" ) : ResourceString(IDC_CMDS3_REFERENCES_501, "references" ));
			break;}
		case 5	:	// "bind"
		case 6	:{	// "insert"
				// EBATECH(FUTA) 2001-01-07 able to use "*"
				//do {
				nRet=sds_getstring(
					1,key==5 // bind or insert
						?ResourceString(IDC_CMDS3__NEXTERNALLY_REF_511, "\nExternally referenced drawing(s) to bind: " )
						:ResourceString(IDC_CMDS3__NEXTERNALLY_REF_514, "\nExternally referenced drawing(s) to convert to blocks: " )
					,str);// get name of XRef
				if (nRet==RTCAN || nRet==RTERROR) goto depart;
				//} while (strsame(str,"*"/*DNT*/));
				ic_trim(str,"be");
				int	nCt=cmd_xref_uncouple(str,key==5?IC_UNCOUPLE_BIND:IC_UNCOUPLE_INSERT, true);
				if (nCt<0){
					nRet=RTERROR;
					goto depart;
				}
				if (key==5) sds_printf(ResourceString(IDC_CMDS3__N_I__S_BOUND__512, "\n%i %s bound." ),nCt,(nCt==1)?ResourceString(IDC_CMDS3_REFERENCE_500, "reference" ) : ResourceString(IDC_CMDS3_REFERENCES_501, "references" ));
				else		sds_printf(ResourceString(IDC_CMDS3__N_I__S_INSERTED_515, "\n%i %s inserted." ),nCt,(nCt==1)?ResourceString(IDC_CMDS3_REFERENCE_500, "reference" ) : ResourceString(IDC_CMDS3_REFERENCES_501, "references" ));
			break;}
		case 7	:	// "?"
		case 8	:	// "_?"
		case 9	:{	// "LIST"
			cmd_block_list(1);
			break;}
		case 10 : {
			xref_unload(NULL);
			break;}
		default:{ nRet=RTERROR;	goto depart;}
		} // switch
	// EBATECH(FUTA)
	//}// for (;;)

depart:
	db_setfontsubstfn(SDS_FontSubsMsg);	//restore font substitution messaging

	if (__LINE__==1)	return(RTCAN);
	if (__LINE__>1)		CMD_INTERNAL_MSG(-__LINE__);

return(nRet);
}







/*----------------------------------------------------------------------------*/
int		cmd_xref_editpath
(
         char	*xrefPattern,char *new_path/*=NULL*/,bool isCropPath/*=false*/, bool isExactName/*=false*/
        )
        {
		/* isExactName is false => xrefPattern is name template
			otherwise xrefPattern fits only one precise string
		*/
	RT ret=RTNORM;
	int found=0;
	struct sds_resbuf *rbp1=NULL,*rbpath,*rbpath2,*rbname,rb;
	char *lpTmp,oldpath[IC_ACADBUF],newpath[IC_ACADBUF],fs1[IC_ACADBUF],szTest[IC_FILESIZE+IC_PATHSIZE];
	char	enteredNewPath[IC_ACADBUF];
	bool convertpl;
	resbuf plrb;

	SDS_getvar(NULL,DB_QPLINETYPE,&plrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if (plrb.resval.rint==2)
		convertpl=true;
	else
		convertpl=false;

	for(rbp1=sds_tblnext("BLOCK"/*DNT*/,1);rbp1!=NULL;rbp1=sds_tblnext("BLOCK"/*DNT*/,0)){
		if (ic_assoc(rbp1,70))continue;
		if (0==(ic_rbassoc->resval.rint & IC_BLOCK_XREF))continue;

		ic_assoc(rbp1,3);
		strncpy(szTest,ic_rbassoc->resval.rstring,sizeof(szTest)-1);
		strupr(szTest);
		//*** Check for a path and loose it in szTest
		for(lpTmp=szTest;*lpTmp!=0;lpTmp++){
			if (*lpTmp=='/'/*DNT*/) *lpTmp='\\'/*DNT*/;
			if (*lpTmp=='|'/*DNT*/) *lpTmp=' '/*DNT*/;
			//2000-06-30 EBATECH(Ara)[start]
			if ((_MBC_LEAD ==_mbbtype((unsigned char)*lpTmp,0)) &&
				  (_MBC_TRAIL==_mbbtype((unsigned char)*(lpTmp+1),1))){
				lpTmp ++;
			}//2000-06-30 EBATECH(Ara)[end]
		}
		if ((lpTmp=strrchr(szTest,'\\'/*DNT*/))!=NULL) {
			*lpTmp=0;
			strcpy(szTest,lpTmp+1);
		}
		//*** Check for a ".dwg" extension and get rid of it
		if ((lpTmp=strrchr(szTest,'.'/*DNT*/))!=NULL) *lpTmp=0;
		lpTmp=NULL;

		if (stricmp(szTest,xrefPattern) != 0
			&& RTNORM != sds_wcmatch(szTest,xrefPattern))
				continue;

		if (ic_assoc(rbp1,1))
			rbpath=NULL;
		else{
			rbpath=ic_rbassoc;
			strncpy(oldpath,rbpath->resval.rstring,sizeof(oldpath)-1);
		}
		if (0==ic_assoc(rbp1,3)){
			rbpath2=ic_rbassoc;
			if (rbpath==NULL)strncpy(oldpath,rbpath2->resval.rstring,sizeof(oldpath)-1);
		}else rbpath2=NULL;
		ic_assoc(rbp1,2);
		rbname=ic_rbassoc;
		if (new_path){
			strcpy(newpath,new_path);
			strcpy(enteredNewPath, new_path);
			ret=RTNORM;
			goto update;
		}
		else {
			if (SDS_getvar(NULL,DB_QFILEDIA,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
				return(RTERROR);
			if (rb.resval.rint){
				dialog: ;
				sprintf(fs1,ResourceString(IDC_CMDS3_NEW_LOCATION_FOR_517, "New location for %s" ),rbname->resval.rstring);
				sds_findfile(oldpath, oldpath);		/*DG - 15.2.2002*/// Convert possibly relative path to absolute one.
				ret=sds_getfiled(fs1,oldpath,"dwg"/*DNT*/,0,&rb);
				if (ret==RTNORM && rb.restype==RTSHORT && rb.resval.rint==1)
					goto typeit;
				if (ret!=RTNORM){
					sds_relrb(rbp1);
					return(ret);
				}else if (RTNORM!=sds_findfile(rb.resval.rstring,newpath))
					goto dialog;
				if (*rb.resval.rstring)IC_FREE(rb.resval.rstring);
				if (NULL!=rbpath){
					rbpath->resval.rstring=ic_realloc_char(rbpath->resval.rstring,strlen(newpath)+1);
					strcpy(rbpath->resval.rstring,newpath);
				}
				if (rbpath2!=NULL){
					rbpath2->resval.rstring=ic_realloc_char(rbpath2->resval.rstring,strlen(newpath)+1);
					strcpy(rbpath2->resval.rstring,newpath);
				}
			}else{
				typeit: ;
				/*DG - 15.2.2002*/// Use the following str instead of newpath in some cases below
				// for allowing entering relative paths, i.e. set db_block::xrefpn exactly to this value.
				do{
					sprintf(fs1,ResourceString(IDC_CMDS3__NNEW_LOCATION_F_519, "\nNew location for %s <%s>: " ),rbname->resval.rstring,oldpath);
					ret=sds_getstring(1,fs1,enteredNewPath/*newpath*/);
				update:
					if (ret==RTNORM){
						  if (strsame(enteredNewPath/*newpath*/,"~"/*DNT*/)) // EBATECH(CNBR) 2002/4/30 use enteredNewPath var.
							  goto dialog;
						if (RTNORM!=sds_findfile(enteredNewPath/*newpath*/,newpath))ret=RTNONE;
						else{
							if (rbpath!=NULL){
								rbpath->resval.rstring=ic_realloc_char(rbpath->resval.rstring,strlen(enteredNewPath/*newpath*/)+1);
								strcpy(rbpath->resval.rstring,enteredNewPath/*newpath*/);
							}
							if (rbpath2!=NULL){
								rbpath2->resval.rstring=ic_realloc_char(rbpath2->resval.rstring,strlen(enteredNewPath/*newpath*/)+1);
								strcpy(rbpath2->resval.rstring,enteredNewPath/*newpath*/);
							}
						}
					}
				}while(!found && RTNORM!=ret && RTCAN != ret);	/*DG - 31.1.2002*/// Allow cancelling.
			}

		} // else new_path
		if (RTNORM==ret){
			db_drawing *flp;
			db_handitem *hip;
			long xrefcnt;
			sds_name ssdrag,ename;

			if ((flp=SDS_CMainWindow->m_pFileIO.drw_openfile(NULL,newpath,NULL,convertpl))==NULL) {
				sds_printf(ResourceString(IDC_CMDS3_ERROR_OPENING_DR_521, "Error opening drawing %s." ),newpath);
				return(RTERROR);
			}

			if (RTNORM!=(ret=SDS_tblmesser(rbp1,IC_TBLMESSER_MODIFY,SDS_CURDWG))){
				sds_relrb(rbp1);
				rbp1=NULL;
				break;
			}
			if (NULL==(hip=SDS_CURDWG->findtabrec(DB_BLOCKTABREC,rbname->resval.rstring,0)))continue;
			hip->set_xrefdp(NULL);
			hip->set_xrefdp(flp);
			hip->set_looked4xref(1);


			if (isCropPath/*!isRetain*/){	/*DG - 15.2.2002*/// The logic was lost with using isRetain instead of isCropPath.
				//	file name is kept without path
				char *name=newpath;
				for(char* pc=newpath; *pc!='\0'; pc++)
					if (*pc=='\\' || *pc=='/') name=pc;
			hip->set_1(++name);
			}


			strncpy(fs1,rbname->resval.rstring,sizeof(fs1)-1);
			if (rbp1!=NULL){sds_relrb(rbp1);rbp1=NULL;}
			/*====Bug:=================================================================
				In this place in old version there was a bug: need be "RTDXF0" instead of "0"
			old code:
				rbp1=sds_buildlist(0,"INSERT"/=DNT=/,2,fs1,0);
			==========================================================================*/
			rbp1=sds_buildlist(RTDXF0,"INSERT"/*DNT*/,2,fs1,0);
			if (RTNORM==sds_pmtssget(NULL,"X"/*DNT*/,NULL,NULL,rbp1,ssdrag,1)){
				for(xrefcnt=0L;RTNORM==sds_ssname(ssdrag,xrefcnt,ename);xrefcnt++){
					sds_entupd(ename);	//this reloads xref dependency
				}
				sds_ssfree(ssdrag);
				if (RTNORM!=(ret=cmd_ReadAsBlock(newpath,fs1,IC_INSMODE_XREF_UPDATE,flp)))break;
			}
		}
		found++;
		if (isExactName)
			break;
	}
	if (ret>=0)
		return(found);
	else
		return(ret);
}


/*----------------------------------------------------------------------------*/
int		cmd_xref_uncouple
(
const char	*cXrefPattern,
int		bindmode,
bool	displayMessages
)
	{
	char *fcp1,*fcp2,fs1[IC_ACADBUF],fs2[IC_ACADBUF],fsshort[6];
	char *xtblnames[]={"LTYPE"/*DNT*/,"LAYER"/*DNT*/,"STYLE"/*DNT*/,"APPID"/*DNT*/,"DIMSTYLE"/*DNT*/};//ltype before layer, because layers can have ltypes
	struct resbuf *rbp1=NULL,*rbp2=NULL,*tmprb=NULL,rb,*rbxref=NULL;
	long fl1;
	RT ret=0;
	int	nfound=0,ndone=0,fi1,blkidx;
	db_handitem *hip;

	//first, verify that some xrefs match pattern, and that if binding/inserting that the files exist
	char* xrefPattern=strdup(cXrefPattern);
	strupr(xrefPattern);

	//NOTE: the blkidx stuff isn't needed for release 1.0, but it doesn't slow things down appreciably
	blkidx=-1;

	for (hip = SDS_CURDWG->tblnext(DB_BLOCKTABREC, 1); hip != NULL; hip = hip->next)
		{
		blkidx++;

		if (hip->ret_deleted())
			continue;

		hip->get_70(&fi1);

		if (!(fi1 & IC_BLOCK_XREF))
			{
			//if block not an xref, go to next block!
			continue;
			}

		hip->get_2(fs1, sizeof(fs1) - 1);
		strupr(fs1);

		if (RTNORM != sds_wcmatch(fs1, xrefPattern))
			{
			//if xref'd block doesn't match the pattern, go on to next block!
			continue;
			}

		// If we got this far, then this block is one we want to bind/detach.
		// Keep its block index so that we know which block references we're working on
		tmprb = sds_buildlist(RTSHORT, blkidx, 0);

		if (rbxref==NULL)
			rbxref=rbp2=tmprb;
		else
			{
			rbp2->rbnext=tmprb;
			rbp2=rbp2->rbnext;
			}

		// If this block was xrefd by R14 and opened by MarComp, none of the
		// table entries are present.  Reload them if user wants to bind or
		// insert...
		if (IC_UNCOUPLE_DETACH != bindmode)
			{
			db_drawing *dp = NULL;

			if (NULL == (dp = (db_drawing *) hip->ret_xrefdp()))
				continue;

			//look for the imported style "STANDARD", which should always be present
			//if table entries are there...
			sprintf(fs2, ResourceString(IDC_CMDS3__S_CSTANDARD_524, "%s%cSTANDARD" ), fs1, CMD_XREF_TBLDIVIDER);
			FreeResbufIfNotNull(&rbp1);

			if (NULL== (rbp1 = sds_tblsearch("STYLE"/*DNT*/,fs2,0)))
				{
				if (displayMessages)
					sds_printf(ResourceString(IDC_CMDS3__NRELOADING_XREF_525, "\nReloading xref %s." ),fs1);

				hip=SDS_CURDWG->findtabrec(DB_BLOCKTABREC,fs1,0);

				if (!hip)
					continue;

				if (IC_UNCOUPLE_BIND == bindmode)
					{
					//at this point, we're done updating existing xrefs from file, so go ahead
					//	and reload the table entries & block defs.
					if (RTNORM != cmd_ReadAsBlock(NULL,fs1,IC_INSMODE_XREF_UPDATE,dp))
						continue;
					}
				else	//IC_UNCOUPLE_INSERT==bindmode, just insert the block into the table
					{
					/*sds_name etemp;
					etemp[0]=(long)hip;
					etemp[1]=(long)SDS_CURDWG;
					sds_entdel(etemp);*/

					hip->null_xrefdp();

					if (RTNORM==cmd_ReadAsBlock(dp->ret_pn(),fs1,IC_INSMODE_INSERT,dp))
						ndone++;
					else
						//sds_entdel(etemp);
						continue;
					}
				}
			}

		nfound++;
		}

	if (tmprb)
		sds_relrb(tmprb);  // this was a leak.

	rbp2 = tmprb = NULL;
	SDS_getvar(NULL,DB_QCMDECHO,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

	if (CMD_CMDECHO_PRINTF(rb.resval.rint))
		{
		if (displayMessages)
		{
			if (0 == nfound)
				sds_printf(ResourceString(IDC_CMDS3__NNO_EXTERNALLY__526, "\nNo externally referenced drawing(s) matching pattern found." ));
			else
				{
				if (IC_UNCOUPLE_DETACH == bindmode)
					sds_printf(ResourceString(IDC_CMDS3__N_I__S_FOUND_FO_527, "\n%i %s found for detach." ),nfound,(nfound==1)?ResourceString(IDC_CMDS3_XREF_528, "xref" ) : ResourceString(IDC_CMDS3_XREFS_529, "xrefs" ));
				else if (IC_UNCOUPLE_BIND == bindmode)
					sds_printf(ResourceString(IDC_CMDS3__N_I__S_FOUND_FO_530, "\n%i %s found for bind." ),nfound,(nfound==1)?ResourceString(IDC_CMDS3_XREF_528, "xref" ) : ResourceString(IDC_CMDS3_XREFS_529, "xrefs" ));
				else if (IC_UNCOUPLE_INSERT == bindmode)
					sds_printf(ResourceString(IDC_CMDS3__N_I__S_FOUND_FO_531, "\n%i %s found for conversion to insert." ),nfound,(nfound==1)?ResourceString(IDC_CMDS3_XREF_528, "xref" ) : ResourceString(IDC_CMDS3_XREFS_529, "xrefs" ));
				}
			}
		}

	if (0 == nfound)
	{
		free(xrefPattern);
		return 0;
	}

	// step through all the tables, and make the necessary changes or deletions...
	// if detaching, then just delete all table refs.
	// first search all the tables for any references to the xrefs
	// NOTE: this should be done w/entmakes, so that UNDO is preserved!
	if (IC_UNCOUPLE_DETACH != bindmode && nfound > 1)
		SDS_ProgressStart();

	for (fi1 = 0; fi1 < 5; fi1++)
		{
		if (bindmode!=IC_UNCOUPLE_DETACH && nfound>1)
			SDS_ProgressPercent((int)(100*fi1/(nfound+5)));

		FreeResbufIfNotNull(&rbp1);
		rbp1=sds_tblnext(xtblnames[fi1],1);
		if (rbp1==NULL)
			continue;//go on to next table
		do
			{
			ic_assoc(rbp1,70);
			tmprb=ic_rbassoc;
			int flags = tmprb->resval.rint; // EBATECH(CNBR) 2002/4/29 INSERT STYLE FOR SHAPE is no style name.
			if (!(tmprb->resval.rint & IC_ENTRY_XREF_DEPENDENT))//if not xref'd table entry, continue;
				{
				sds_relrb(rbp1);
				rbp1=NULL;
				continue;
				}
			//we can do this 2 ways.  we can either look at the hip->xrefid or
			//	we can look at the string.	Looking at the hip->xrefid is faster,
			//	but we will look at the name(string). This is because we'll need to
			//	alter the string or delete the entry using tblmesser, which uses name.
			//
			ic_assoc(rbp1,2);
			strncpy(fs1,ic_rbassoc->resval.rstring,sizeof(fs1)-1);
			strupr(fs1);
			if (NULL!=(fcp1=strchr(fs1,CMD_XREF_TBLDIVIDER)))
				{
				*fcp1=0;
				if (RTNORM!=(sds_wcmatch(fs1,xrefPattern)))
					continue;
				fcp1++;
				}
			else //should never happen
				{
				sds_relrb(rbp1);
				rbp1=NULL;
				continue;
				}
			if (IC_UNCOUPLE_DETACH==bindmode)
				//delete entry
				SDS_tblmesser(rbp1,IC_TBLMESSER_DELETE,SDS_CURDWG);
			else if (IC_UNCOUPLE_INSERT==bindmode)
				{
				//alter entry if not found in current dwg so that it's defined as "regular" entry
				//	if entry w/that name already exists, just delete xref entry
				FreeResbufIfNotNull(&rbp2);
				if (NULL!=(rbp2=sds_tblsearch(xtblnames[fi1],fcp1,0)))
					//if entry exists, then delete xref copy of it
					//we'll remake it when we insert the block defs and ents
					SDS_tblmesser(rbp1,IC_TBLMESSER_DELETE,SDS_CURDWG);
				else
					{
					//Entry doesn't exist.	Modify exist xref'd entry and then rename as "regular" entry
					tmprb->resval.rint&=(~(IC_XREF_RESOLVED | IC_ENTRY_XREF_DEPENDENT));
					SDS_tblmesser(rbp1,IC_TBLMESSER_MODIFY,SDS_CURDWG);
					// EBATECH(CNBR) 2002/4/29 -[ INSERT STYLE FOR SHAPE is no style name.
					if( strisame(xtblnames[fi1],"STYLE"/*DNT*/) && ( flags & IC_SHAPEFILE_SHAPES )){
						*fcp1 = 0;
					}
					// EBATECH(CNBR) ]-
					//Create resbuf w/new name & rename
					rbp2=sds_buildlist(1,fcp1,0);
					rbp2->rbnext=rbp1;
					rbp1=rbp2;
					rbp2=NULL;
					SDS_tblmesser(rbp1,IC_TBLMESSER_RENAME,SDS_CURDWG);
					}
				}
			else if (IC_UNCOUPLE_BIND==bindmode)
				{
				//alteration & binding
				for(fl1=0;fl1<=9999;fl1++)
					{
					FreeResbufIfNotNull(&rbp2);
					strncpy(fs2,fs1,sizeof(fs2)-1);
					for(fcp2=fs2;*fcp2!=0;fcp2++);
					*fcp2=CMD_XREF_TBLBINDER;
					fcp2++;
					*fcp2=0;
					itoa((int)fl1,fsshort,10);
					strcat(fs2,fsshort);
					//LEAVE FCP2 where it is - we may need to increment it if the block exists
					do
						fcp2++;
					while((*fcp2)!=0);
					*fcp2=CMD_XREF_TBLBINDER;
					*(fcp2+1)=0;
					strncat(fs2,fcp1,sizeof(fs2)-1);
					//now we have the name to make, but first, check to see if new name already exists
					if (NULL==(rbp2=sds_tblsearch(xtblnames[fi1],fs2,0)))
						break;
					}
				if (9999==fl1)//didn't get any entry
					{
					FreeResbufIfNotNull(&rbp1);
					FreeResbufIfNotNull(&rbp2);
					ret=RTERROR;
					goto exit;
					}
				//reset existing entry bits so it doesn't show as xref entry
				tmprb->resval.rint&=(~(IC_ENTRY_XREF_DEPENDENT | IC_XREF_RESOLVED));
				SDS_tblmesser(rbp1,IC_TBLMESSER_MODIFY,SDS_CURDWG);
				//now change entry name
				if (NULL==(rbp2=sds_buildlist(1,fs2,0)))
					{
					if (rbp1!=NULL)sds_relrb(rbp1);
					ret=RTERROR;
					goto exit;
					}
				rbp2->rbnext=rbp1;
				rbp1=rbp2;
				rbp2=NULL;
				if (RTNORM!=SDS_tblmesser(rbp1,IC_TBLMESSER_RENAME,SDS_CURDWG))
					{
					sds_relrb(rbp1);
					ret=RTERROR;
					goto exit;
					}
				}
			FreeResbufIfNotNull(&rbp1);
			}
		while(NULL!=(rbp1=sds_tblnext(xtblnames[fi1],0)));
		}

	if (IC_UNCOUPLE_DETACH!=bindmode)
		db_setfontsubstfn(NULL);//kill any font messages which may interfere w/prompting

	/* *****COMMENTED OUT FOR VER 1.0 *****
		This code updates all blocks dependent upon the xrefs we're binding/inserting.
		Since in ver 1.0 we won't read in the blocks or their subents, we can comment
		this code out.	We'll read the definitions in using cmd_ReadAsBlock at
		the end of the commented-out code!


	//we'll walk the block table again, this time stopping at all the blocks which
	//	are in our indes llist blocks to modify.
	for(hip=SDS_CURDWG->tblnext(DB_BLOCKTABREC,1); hip!=NULL; hip=hip->next) {
		if (hip->ret_deleted())continue;//check for deleted entries
		hip->get_70(&fi1);
		if (0==(fi1&(4+16)))continue;//check for non-xref'd entries
		//at this pt, block is valid xref or its dependency
		blkidx=((db_tablerecord *) hip)->ret_xrefidx();
		for(tmprb=rbxref;tmprb!=NULL;tmprb=tmprb->rbnext){
			if (blkidx==tmprb->resval.rint)break;
		}
		if (tmprb==NULL)continue;//didn't find this in list to delete

		//at this point, we're sure we're working on a block that's from an
		//	xref we're working on.	we need to entget it so that we can do
		//	the modifications necessary...
		if (rbp1!=NULL){sds_relrb(rbp1);rbp1=NULL;}
		if (NULL==(rbp1=hip->entgetx(NULL,SDS_CURDWG)))continue;
		hip->get_2(fs1,sizeof(fs1)-1);
		strupr(fs1);
		if (IC_UNCOUPLE_DETACH==bindmode){//if detaching...
			SDS_tblmesser(rbp1,IC_TBLMESSER_DELETE,SDS_CURDWG);//delete block from table
			//next, delete all inserts of the xref
			if (fi1&4){
				if (rbp2!=NULL){sds_relrb(rbp2);rbp2=NULL;}
			   rbp2=sds_buildlist(RTDXF0,"INSERT"DNT,2,fs1,0);
				if (RTNORM==sds_pmtssget(NULL,ResourceString(IDC_CMDS3_A_532, "A" ),NULL,NULL,rbp2,sset,0)){
					for(fl1=0L;RTNORM==sds_ssname(sset,fl1,ent);sds_entdel(ent),fl1++);
				}
			}
			sds_relrb(rbp2);
			sds_relrb(rbp1);
			rbp1=rbp2=NULL;
			continue;
		}
		//at this point, we're just dealing w/binding and inserting of xref
		//regardless of what else we're doing we have to set all the bit flags
		//	so the block won't be shown as xref'd.	This needs to be done
		//	separately from the rename operation
		ic_assoc(rbp1,70);
		ic_rbassoc->resval.rint&=~(4+16+32);
		if (fi1&4){
			//if block is an xref itself, get rid of the xref path in the block def
			if (0==ic_assoc(rbp1,1))rbp1->resval.rstring=0;
		}else{
			//modify all the subents in the block so that they have the correct
			//	layer,style,ltype,etc.
			hip->get_neg2(ent,SDS_CURDWG);
			cmd_Xrefblk2Blk(ent,bindmode);
		}
		SDS_tblmesser(rbp1,IC_TBLMESSER_MODIFY,SDS_CURDWG);
		if (NULL!=(fcp1=strchr(fs1,CMD_XREF_TBLDIVIDER))){
			*fcp1=0;
			fcp1++;
		}
		//DO NOT move fcp1 - it marks name of block within table

		//DO ANONYMOUS BLKS 1st.  They're handled the same for both bind and insert
		if (*fs1=='*'){
			//if it's an anonymous block, always just rename it w/o the |REF in it
			fcp1+=3;	//move fcp1 to number portion
			strncpy(fsshort,fcp1,sizeof(fsshort)-1);
			strncat(fs1,fsshort,sizeof(fsshort)-1);	//it's shorter anyway
			if (rbp2!=NULL)sds_relrb(rbp2);rbp2=NULL;
			rbp2=sds_buildlist(1,fs1,0);
			rbp2->rbnext=rbp1;
			rbp1=rbp2;
			SDS_tblmesser(rbp1,IC_TBLMESSER_RENAME,SDS_CURDWG);
			sds_relrb(rbp1);
			rbp1=NULL;
			continue;
		}else if (NULL==fcp1 || (RTNORM!=sds_wcmatch(fs1,xrefPattern))){	//should never happen
			//if xref'd block doesn't match the pattern, go on to next block!
			sds_relrb(rbp1);
			rbp1=NULL;
			continue;
		}
		//now deal w/all non-anonymous blocks.	These get handled differently for
		//	bind than for insert
		if (IC_UNCOUPLE_INSERT==bindmode){	//insert
			*fcp1=0;
			fcp1++;
			if (rbp2!=NULL){sds_relrb(rbp2);rbp2=NULL;}
		   if (NULL!=(rbp2=sds_tblsearch("BLOCK"DNT,fcp1,0))){
				sds_relrb(rbp2);rbp2=NULL;
				continue;
			}
			rbp2=sds_buildlist(1,fcp1,0);
			rbp2->rbnext=rbp1;
			rbp1=rbp2;rbp2=NULL;
			SDS_tblmesser(rbp1,IC_TBLMESSER_RENAME,SDS_CURDWG);
		}else if (IC_UNCOUPLE_BIND==bindmode){
			char fs3[IC_ACADBUF];
			//figure out the new name for the block
			*fcp1=0;
			fcp1++;
			for(fl1=0;fl1<9999;fl1++){
				if (rbp2!=NULL){sds_relrb(rbp2);rbp2=NULL;}
				strncpy(fs2,fs1,sizeof(fs2)-1);
				for(fcp2=fs2;*fcp2!=0;fcp2++);
				*fcp2=CMD_XREF_TBLBINDER;
				fcp2++;
				*fcp2=0;
				itoa((int)fl1,fsshort,10);
				strcat(fs2,fsshort);
				//LEAVE FCP2 where it is - we may need to increment it if the block exists
				do{
					fcp2++;
				}while((*fcp2)!=0);
				*fcp2=CMD_XREF_TBLBINDER;
				*(fcp2+1)=0;
				strncat(fs2,fcp1,sizeof(fs2)-1);
				if (0==fl1)strncpy(fs3,fs2,sizeof(fs3)-1);
				//now we have the block to make, but first, check to see if new block already exists
				if (NULL==(rbp2=sds_tblsearch("BLOCK"DNT,fs2,0)))break;
				 else strncpy(fs3,fs2,sizeof(fs3)-1);
			}
			if (NULL==(rbp2=sds_buildlist(1,fs3,0))){
				if (rbp1!=NULL)sds_relrb(rbp1);
				return(RTERROR);
			}
			rbp2->rbnext=rbp1;
			rbp1=rbp2;
			if (RTNORM!=SDS_tblmesser(rbp1,IC_TBLMESSER_RENAME,SDS_CURDWG)){
				sds_relrb(rbp1);
				return(RTERROR);
			}
			rbp2=NULL;
		}
	}

	if (IC_UNCOUPLE_DETACH==bindmode)goto exit;
	//now open the xref'd drawing and read in all the entities like a std block
	for(hip=SDS_CURDWG->tblnext(DB_BLOCKTABREC,1); hip!=NULL; hip=hip->next) {
		if (hip->ret_deleted())continue;
		hip->get_70(&fi1);
		if (0==(fi1 & 4))continue;//check for non-xref'd entries
		//at this pt, block is valid xref or its dependency
		blkidx=((db_tablerecord *) hip)->ret_xrefidx();
		for(tmprb=rbxref;tmprb!=NULL;tmprb=tmprb->rbnext){
			if (blkidx==tmprb->resval.rint)break;
		}
		if (tmprb==NULL)continue;//didn't find this in list to delete
		//at this point, we're sure we have an xref block that we're working
		//	on.  Check to affirm that we have the dwg in memory
		if (NULL==(dp=(db_drawing *)hip->ret_xrefdp()))continue;

		if (rbp1!=NULL){sds_relrb(rbp1);rbp1=NULL;}
		if (NULL==(rbp1=hip->entgetx(NULL,SDS_CURDWG)))continue;
		hip->get_2(fs1,sizeof(fs1)-1);
		strupr(fs1);
		if (IC_UNCOUPLE_INSERT==bindmode){//if inserting...


		}else if (IC_UNCOUPLE_BIND==bindmode){//if binding
			//code not yet completed
		}
	}
	******END OF CODE COMMENTED OUT FOR VER 1.0 */
	//code to replace commented out code...

	for (hip = SDS_CURDWG->tblnext(DB_BLOCKTABREC, 1); hip != NULL; hip = hip->next)
		{
		if (hip->ret_deleted())
			continue;

		hip->get_70(&fi1);

		if (!(fi1 & IC_BLOCK_XREF))
			//if block not an xref, go to next block!
			continue;

		hip->get_2(fs1,sizeof(fs1));
		strupr(fs1);

		if (RTNORM != sds_wcmatch(fs1, xrefPattern))
			//if xref'd block doesn't match the pattern, go on to next block!
			continue;

		if (IC_UNCOUPLE_BIND == bindmode)
			{
			db_drawing *dp = NULL;

			if (NULL == (dp = (db_drawing *) hip->ret_xrefdp()))
				continue;

			if (RTNORM==(ret=cmd_ReadAsBlock(NULL,fs1,IC_INSMODE_XREF_BIND,dp)))
				ndone++;
			else if (displayMessages)
				sds_printf(ResourceString(IDC_CMDS3__NERROR_BINDING__533, "\nError binding xref %s." ),fs1);

			if (nfound>1)
				SDS_ProgressPercent((int)100*(ndone+5)/(nfound+5));
			}
		else if (IC_UNCOUPLE_INSERT == bindmode)
			{
			db_drawing *dp = NULL;

			if (NULL == (dp = (db_drawing *) hip->ret_xrefdp()))
				continue;

			if (RTNORM==(ret=cmd_ReadAsBlock(NULL,fs1,IC_INSMODE_XREF_INSERT,dp)))
				ndone++;
			else if (displayMessages)
				sds_printf(ResourceString(IDC_CMDS3__NERROR_INSERTIN_534, "\nError inserting xref %s." ),fs1);

			if (nfound>1)
				SDS_ProgressPercent((int)100*(ndone+5)/(nfound+5));
			}
		else // IC_UNCOUPLE_DETACH == bindMode
			{
			sds_name sset,ename;
			long fl1;

			FreeResbufIfNotNull(&rbp1);

			if (NULL==(rbp1=hip->entgetx(NULL,SDS_CURDWG)))
				continue;

			//delete all inserts of xref
			FreeResbufIfNotNull(&rbp2);
			ic_assoc(rbp1,2);

			if (NULL==(rbp2=sds_buildlist(RTDXF0,"INSERT"/*DNT*/,2,ic_rbassoc->resval.rstring,0)))
				continue;

			if (RTNORM==sds_pmtssget(NULL,ResourceString(IDC_CMDS3_X_506, "X" ),NULL,NULL,rbp2,sset,0))
				{
				for (fl1=0L;RTNORM==sds_ssname(sset,fl1,ename);fl1++)
					sds_entdel(ename);
				}

			if (sset)
				sds_ssfree(sset);

			FreeResbufIfNotNull(&rbp2);

			//delete xref from blocks table
			//			SDS_tblmesser(rbp1,IC_TBLMESSER_DELETE,SDS_CURDWG);
			// Fix bug 6392.  SDS_tblmesser always undeletes items with the same name if
			// they are present in the table.  This is not what we want in this case.
			sds_name	xrb_ename = {(long)hip, (long)SDS_CURDWG},
						obj_ename = {0L, xrb_ename[1]};	// this will point to idbuffer and then to xdictionary
			db_hireflink*	xdicref = hip->ret_xdict();	// Get xdic reference for deletion

			// I put in this conditional to try to avoid the crash when you have main attaches a attaches b and
			// you issue xref detach b.  This is NOT the right way to handle this, but it crashes now so I'm trying
			// to at least prevent that.
			ASSERT(xdicref);

			if(xdicref)
				{
				db_handitem*	xdic = xdicref->ret_hip(SDS_CURDWG);
				ASSERT(xdic);
				db_dictionaryrec	*bptr = NULL;
				((db_dictionary*)xdic)->get_recllends(&bptr, NULL);
				ASSERT(bptr);
				// and the idbuffer
				if(bptr)	// this condition was added at 14.01.2001 to avoid crash on detach
					{
					db_handitem*	idbuf = bptr->hiref->ret_hip(SDS_CURDWG);  // removed direct ref to hip for the moment;
					ASSERT(idbuf);

					// delete the block -- DOES THIS DELETE THE WHOLE BLOCK DEF?  I guess so.
					sds_entdel(xrb_ename);

					// delete the idbuffer
					obj_ename[0] = (long)idbuf;
					RT	ret = sds_entdel(obj_ename);
					ASSERT(ret == RTNORM);
					}
				/*DG - 28.3.2003*/// The block should be deleted anyway.
				else
					sds_entdel(xrb_ename);

				// delete the xdictionary
				obj_ename[0] = (long)xdic;
				ret = sds_entdel(obj_ename);
				ASSERT(ret == RTNORM);
				++ndone;
				}
			/*DG - 28.3.2003*/// The block should be deleted anyway.
			else
				sds_entdel(xrb_ename);
			}	// IC_UNCOUPLE_DETACH == bindMode
		}

	cmd_regenall();

exit:
	FreeResbufIfNotNull(&rbp1);
	FreeResbufIfNotNull(&rbp2);

	if (IC_UNCOUPLE_DETACH!=bindmode)
		{
		db_setfontsubstfn(SDS_FontSubsMsg);//restore font substitution messaging
		if (nfound>1)
			SDS_ProgressStop();
		}
	free(xrefPattern);
	if (ret<0)
		return(ret);
	else
		return(ndone);
	}

int cmd_Xrefblk2Blk(sds_name firstblkent, int mode){
	//goes thru block's  subents to convert xrefs to std blocks.
	//pass ename as the first entity in the block definition
	//	may not be called recursively

	//if mode==1, bind the block definition
	//if mode==2, convert block def from xref to "standard" block
	//returns 0 if successful, -1 if internal error, error code otherwise

	struct resbuf *rbp1,*rbtemp,*rbtype,rbapp;
	sds_name ename;
	RT ret=0;
	char *fcp1,*fcp2,*tblnam,fs1[IC_ACADBUF],fsshort[6];

	rbp1=NULL;
	//now move on to modifying all ents within block
	rbapp.rbnext=NULL;
	rbapp.restype=RTSTR;
	rbapp.resval.rstring="*"/*DNT*/;
	do{
		if (rbp1!=NULL)sds_relrb(rbp1);
		if (NULL==(rbp1=sds_entgetx(ename,&rbapp)))
			return(-1);
		for(rbtemp=rbp1;rbtemp!=NULL;rbtemp=rbtemp->rbnext){
			//then a simple case stmt to convert components of blk entity's llist
			switch(rbtemp->restype){
				case 0:
					{
			if (strsame(rbtemp->resval.rstring,"ENDBLK"/*DNT*/)){
							//required if block is empty
							goto exit;
						}
						rbtype=rbtemp;
					}
					break;
				case 2:
					{
			if (strsame(rbtype->resval.rstring,"INSERT"/*DNT*/) ||
			  strsame(rbtype->resval.rstring,"DIMENSION"/*DNT*/)){
							int len=strlen(rbtemp->resval.rstring);
							if (NULL==(fcp1=strchr(rbtemp->resval.rstring,CMD_XREF_TBLDIVIDER)))continue;
								*fcp1=0;
							//convert the old name to the new name
							if (*rbtemp->resval.rstring=='*'){
								//dim or anonymous insert in this block
								fcp1+=4;
								strncpy(fsshort,fcp1,sizeof(fsshort)-1);
								strncat(rbtemp->resval.rstring,fsshort,sizeof(fsshort)-1);//string is shorter, so this is okay
							}else{
								//convert "regular" insert name to "standard" insert name
								if (2==mode){
									fcp1++;
									strncpy(fs1,fcp1,sizeof(fs1)-1);
									strncpy(rbtemp->resval.rstring,fs1,len);//it's shorter, so no realloc needed
								}else if (1==mode){	//add the binding symbology
									char fs3[IC_ACADBUF],fs2[IC_ACADBUF],*fcp2;
									struct resbuf *rbp2=NULL;
									int fl1;
									//figure out the new name for the block
									strncpy(fs1,rbtemp->resval.rstring,sizeof(fs1)-1);
									fcp1++;
									for(fl1=0;fl1<9999;fl1++){
										if (rbp2!=NULL){sds_relrb(rbp2);rbp2=NULL;}
										strncpy(fs2,fs1,sizeof(fs2)-1);
										for(fcp2=fs2;*fcp2!=0;fcp2++);
										*fcp2=CMD_XREF_TBLBINDER;
										fcp2++;
										*fcp2=0;
										itoa((int)fl1,fsshort,10);
										strcat(fs2,fsshort);
										//LEAVE FCP2 where it is - we may need to increment it if the block exists
										do{
											fcp2++;
										}while((*fcp2)!=0);
										*fcp2=CMD_XREF_TBLBINDER;
										*(fcp2+1)=0;
										strncat(fs2,fcp1,sizeof(fs2)-1);
										if (0==fl1)strncpy(fs3,fs2,sizeof(fs3)-1);
										//now we have the block to make, but first, check to see if new block already exists
										if (NULL==(rbp2=sds_tblsearch("BLOCK"/*DNT*/,fs2,0)))
											break;
										 else strncpy(fs3,fs2,sizeof(fs3)-1);
									}
									//now fs3 contains the new name of the insert in this block
									rbtemp->resval.rstring=ic_realloc_char(rbtemp->resval.rstring,sizeof(fs3)+1);
									strcpy(rbtemp->resval.rstring,fs3);
								}
							}
						}
					}
					break;
				case 3:
				case 6:
				case 7:
				case 8:
				case 1001:
					{
						//if not an xref entry, skip it
						if (NULL==(fcp1=strchr(rbtemp->resval.rstring,CMD_XREF_TBLDIVIDER)))
							continue;
						if (2==mode){
							fcp1++;
							strncpy(fs1,fcp1,sizeof(fs1)-1);
							strcpy(rbtemp->resval.rstring,fs1);//ok - it's shortened
						}else if (1==mode){
							//convert entry into proper format
							if (rbtemp->restype==3)tblnam="DIMSTYLE"/*DNT*/;
							else if (rbtemp->restype==6)tblnam="LTYPE"/*DNT*/;
							else if (rbtemp->restype==7)tblnam="STYLE"/*DNT*/;
							else if (rbtemp->restype==8)tblnam="LAYER"/*DNT*/;
							else tblnam="APPID"/*DNT*/;

							char fsshort[6];
							struct resbuf *rbp2=NULL;
							int fi1;
							*fcp1=0;//divide existing name into 2 parts, fs1=xref & fcp1=name
							fcp1++;
							for(fi1=0;fi1<9999;fi1++){
								if (rbp2!=NULL){sds_relrb(rbp2);rbp2=NULL;}
								strncpy(fs1,rbtemp->resval.rstring,sizeof(fs1)-1);
								for(fcp2=fs1;*fcp2!=0;fcp2++);
								*fcp2=CMD_XREF_TBLBINDER;
								fcp2++;
								*fcp2=0;
								itoa((int)fi1,fsshort,10);
								strcat(fs1,fsshort);
								//LEAVE FCP2 where it is - we may need to increment it if the block exists
								do{
									fcp2++;
								}while((*fcp2)!=0);
								*fcp2=CMD_XREF_TBLBINDER;
								*(fcp2+1)=0;
								strncat(fs1,fcp1,sizeof(fs1)-1);
								//now we have the block to make, but first, check to see if new block already exists
								if (NULL==(rbp2=sds_tblsearch(tblnam,fs1,0)))
									break;
							}
							if (rbp2!=NULL){//should never happen
								sds_relrb(rbp2);
								if (rbp1!=NULL)sds_relrb(rbp1);
									return(RTERROR);
							}
							//need to realloc rbtemp's string
							rbtemp->resval.rstring=ic_realloc_char(rbtemp->resval.rstring,strlen(fs1)+1);
							strcpy(rbtemp->resval.rstring,fs1);
							//need to do sds_entmod at end of for loop
							rbp2=NULL;
						}

					}
					break;
				default:
					continue;
					break;
			}
		}
		sds_entmod(rbp1);
	}while(RTNORM==sds_entnext_noxref(ename,ename));

	exit:
	if (rbp1!=NULL){
		sds_relrb(rbp1);
		rbp1=NULL;
	}
	return(0);
}


short cmd_xrefclip(void)  {

	cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0);
	SDS_FreeEventList(0);
	return(RTNORM);

//*******************Notes for whoever writes the guts of MLEdit******************************************
//

	sds_printf(ResourceString(IDC_CMDS3__NTHE_XREFCLIP_F_537, "\nThe XRefClip function is under construction.  It prompts, but has no effect." ));

	char			szXRefName[IC_ACADBUF],	// Name of XRef (must already be inserted in drawing)					// ???	IC_ACADBUF
					szLayer[IC_ACADBUF],	// Name of new (not existing) layer for clipped XRef
					szPrompt[IC_ACADBUF],	// For making up complicated prompt strings with %s
					szKeyWord[IC_ACADBUF];	// Keyword from command line
	sds_point		pt1stCorner,			// 1st corner of area for clipped XRef.
					pt2ndCorner,			// 2nd corner of area for clipped XRef.
					ptInsertion;			// Insertion point for clipped XRef.
	sds_real		rPaperUnits=1.0,		// Paper Space units for ratio								// ??? Should I be doing this with one point instead?
					rModelUnits=1.0;		// Model Space units for ratio								// ??? What about problems with reals and ==
	struct resbuf	//rb,						// For getting drawing names & info
					rbSetGet;				// For getting and setting system variables
	RT				nRet=RTNORM;			// This function's return value.
	long			lErrorLine=0L;			// Gives the line number if there is an error.
	short			swInvalidName=1,		// I just put this in to test the code.  Take it out when you are ready.
					swTryAgain=0;			// If user blows it on an XRef or Layer name, we ask again.

	// Rules are:  You're in Paper Space, or You're outta here...
	if (SDS_getvar(NULL,DB_QTILEMODE,&rbSetGet,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
		return(RTERROR);
	if (rbSetGet.resval.rint!=0)	{
		// If we are in in Model Space...
		sds_initget(0, ResourceString(IDC_XREFCLIP_INIT1, "Yes_-_Switch_to_Paper_Space|Yes No_-_Cancel|No ~Cancel ~Switch ~_Yes ~_No ~_Cancel ~_Switch"));
		if ((nRet=sds_getkword(ResourceString(IDC_CMDS3__NCURRENTLY_IN_M_539, "\nCurrently in Model Space.  Cancel/<Switch to Paper Space>: " ),szKeyWord))==RTERROR) {
			goto bail;
		} else if (nRet==RTCAN) {
			goto bail;
		} else if (nRet==RTNONE) {
			rbSetGet.resval.rint=0;
			rbSetGet.restype=RTSHORT;
			if ((sds_setvar("TILEMODE"/*DNT*/,&rbSetGet)!=RTNORM))
				goto bail;
		} else if (nRet==RTNORM) {
			if ((strisame(szKeyWord,"YES"/*DNT*/)) || strisame(szKeyWord,"SWITCH"/*DNT*/)) {
			rbSetGet.resval.rint=0;
			rbSetGet.restype=RTSHORT;
			if ((sds_setvar("TILEMODE"/*DNT*/,&rbSetGet)!=RTNORM))
				goto bail;
			} else if ((strisame(szKeyWord,"NO"/*DNT*/)) || strisame(szKeyWord,"CANCEL"/*DNT*/)) {
				goto bail;
			}
		}
	}

	// Get the name of an XRef that already exists in the drawing...
	for (;;) {		// AutoCAD *does* go back to the prompt if you give it a bad name, but doesn't give you the List option
		if (sds_initget(0,ResourceString(IDC_XREFCLIP_INIT2, "List_External_References|? ~_?"))!=RTNORM) {
			lErrorLine=(-__LINE__);
			goto bail;
		}
		if (!swTryAgain) {
			sprintf(szPrompt,ResourceString(IDC_CMDS3__N__TO_LIST_EXTE_544, "\n? to list external references/<Name of external reference to clip>: " ));
			swTryAgain=1;	// Set this (or take it out) as needed.  I have it here just for testing.
		} else {
			sprintf(szPrompt,ResourceString(IDC_CMDS3__NUNABLE_TO_FIND_545, "\nUnable to find %s in drawing.  ? to list/<Enter another XRef name>:  " ),szXRefName);
			swTryAgain=0;	// Set this (or take it out) as needed.  I have it here just for testing.
		}
		if ((nRet=sds_getstring(0,szPrompt,szXRefName))==RTERROR) { 										 // get szXRefName
			goto bail;
		} else if (nRet==RTCAN) {
			goto bail;
		} else if (*szXRefName==0){		// If the user hits ENTER (sds_getstring has no RTNONE).
			swTryAgain=0;
		} else if (nRet==RTNORM) {
			if (strisame(szXRefName, "?"/*DNT*/) || strisame(szXRefName, "_?"/*DNT*/)) {
				cmd_block_list(1);
				swTryAgain=0;
			} else if (strisame(szKeyWord,"WRONG"/*DNT*/)) {	// Just for now, for testing - this will go through the "Unable to find..." part.
			} else if (!strisame(szXRefName,""/*DNT*/)) {		// Just for now, for testing - this will accept anything (except a ?) to go on through prompts.
				break;
			}
		}

		// Check here for validity - must be one they've already inserted, not a new one from a file dialog.
		// If not valid,
		//		swTryAgain=1;		// Set to 1 to go back through the for (;;)
		// If valid,
		//		break;
	}

	swTryAgain=0;	// Reseting for use in next for (;;) loop

	// Get the name for a new layer (one that doesn't exist already)...
	for (;;) {
		if (sds_initget(0, ResourceString(IDC_XREFCLIP_INIT3, "List_existing_layers|? ~_?"))!=RTNORM) {
			lErrorLine=(-__LINE__);
			goto bail;
		}
		if (!swTryAgain) {
			(sprintf(szPrompt,ResourceString(IDC_CMDS3__N__TO_LIST_LAYE_548, "\n? to list layers/<Name for new layer to create for clipped XRef>: " )));
			swTryAgain=1;	// Set this (or take it out) as needed.  I have it here just for testing.
		} else {
			sprintf(szPrompt,ResourceString(IDC_CMDS3__NTHE_LAYER__S_A_549, "\nThe layer %s already exists.  ? to list/<Name for new layer>:  " ),szLayer);
			swTryAgain=0;	// Set this (or take it out) as needed.  I have it here just for testing.
		}
		if ((nRet=sds_getstring(0,szPrompt,szLayer))==RTERROR) {										 // get szXRefName
			goto bail;
		} else if (nRet==RTCAN) {
			goto bail;
		} else if (*szLayer==0){			// If the user hits ENTER (sds_getstring has no RTNONE).
												// Is there anything we have to release or free here before going away?
			cmd_vplayer();						// Sorry, but this is what AutoCAD does.  And they don't give you a way back.
			goto bail;
		} else if (nRet==RTNORM) {
			if (*szLayer=='?') {
				cmd_layer_list();
				swTryAgain=0;
			} else if (strisame(szLayer,"WRONG"/*DNT*/)) {	// Just for now, for testing - this will go through the "Unable to find..." part.
			} else if (!strisame(szLayer,""/*DNT*/)){		// Just for now, for testing - this will accept anything (except a ?) to go on through prompts.
				break;
			}
			// Check to see that it doesn't already exist
				// If it does, tell the user
		}

	}
	// Why does AutoCAD say "There are no active Model Space viewports" when it just switched you to Paper Space???

	// Get corners of clipping area		(accepts, and uses, negative values)
	sprintf(szPrompt,ResourceString(IDC_CMDS3__NFIRST_CORNER_O_550, "\nFirst corner of clipping area for %s: " ),szXRefName);
	if (sds_initget(0,""/*DNT*/)!=RTNORM) {
		lErrorLine=(-__LINE__);
		goto bail;
	}	// AutoCAD accepts a 3D point (and negative values) but discards Z.   ??? How ???
	if ((nRet=internalGetpoint(NULL,szPrompt,pt1stCorner))==RTCAN || nRet==RTERROR)
		goto bail;						// get pt1stCorner
	if (sds_initget(0, ""/*DNT*/)!=RTNORM) {
		lErrorLine=(-__LINE__);
		goto bail;
	}	// AutoCAD accepts a 3D point (and negative values) but discards Z
	if ((nRet=sds_getcorner(pt1stCorner,ResourceString(IDC_CMDS3__NOPPOSITE_CORNE_281, "\nOpposite corner: " ),pt2ndCorner))==RTCAN || nRet==RTERROR)
		goto bail;		// get pt2ndCorner

	// Get ratio of paper space units to model space units		(must be positive and non-zero)
	if (sds_initget(RSG_NONEG+RSG_NOZERO,""/*DNT*/)!=RTNORM) {
		lErrorLine=(-__LINE__);
		goto bail;
	}
	sprintf(szPrompt,ResourceString(IDC_CMDS3__NFOR_RATIO__PAP_551, "\nFor ratio: Paper Space units (%f)>: " ),rPaperUnits);
	if ((nRet=sds_getreal(szPrompt,&rPaperUnits))==RTCAN || nRet==RTERROR)
		goto bail;				// get Paper Space units for PS:MS ratio

	if (sds_initget(RSG_NONEG+RSG_NOZERO,"")!=RTNORM) {
		lErrorLine=(-__LINE__);
		goto bail;
	}
	sprintf(szPrompt,ResourceString(IDC_CMDS3__NFOR_RATIO__MOD_552, "\nFor ratio: Model Space units (%f)>: " ),rModelUnits);
	if ((nRet=sds_getreal(szPrompt,&rModelUnits))==RTCAN || nRet==RTERROR)
		goto bail;				// get Model Space units for PS:MS ratio

	// Get insertion point
	if (sds_initget(0,""/*DNT*/)!=RTNORM) {
		lErrorLine=(-__LINE__);
		goto bail;
	}	// AutoCAD accepts a 3D point (and negative values) but discards Z.
	if ((nRet=internalGetpoint(NULL,ResourceString(IDC_CMDS3__NINSERTION_POIN_553, "\nInsertion point:  " ),ptInsertion))==RTCAN || nRet==RTERROR)
		goto bail;		// get ptInsertion

	// Do all the hard stuff (sorry J')

bail:

	if (lErrorLine==0L)
		return(RTNORM);
	if (lErrorLine==(-1L))
		return(RTCAN);
	if (lErrorLine<(-1L))
		CMD_INTERNAL_MSG(lErrorLine);
	return(nRet);
}



//*********************************************************
short cmd_spline(void)	{

	//sds_name		nmEntity;				// The entity to be converted to a spline (not implemented yet)
	sds_point		ptStart,
					ptTang1,
					ptTang2,
					pt1,
					pt2,
					pt210;
	char			fs1[IC_ACADBUF],
					fs2[IC_ACADBUF];		// Keyword from command line
	static sds_real	srFitTol,				// Tolerance, in drawing units, for spline fit - see #defines above
					fr1;
	int				nFlag=12,				// 70 Group spline flags (default to planar, rational)
					nPts,					// 74 Group w/Number of fit pts
					nDegree;				// 71 Group w/Spline type
	RT				nRet=RTNORM;			// This function's return value.
	long			lErrorLine=0L;			// Gives the line number if there is an error.
	sds_resbuf		*rbSpline,				// Resbuf llist for building spline
					*rbPts=NULL,			// Ptr to resbuf w/Fit pts to use
					*rbPtsNext=NULL,
					setgetrb,
					rbucs,
					rbwcs;
	sds_name		ent2redraw;


	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002
	if (cmd_istransparent() == RTERROR) return RTERROR;

	nDegree=3;	//as far as I can tell, this never changes
	ent2redraw[0]=ent2redraw[1]=0L;

	rbSpline=rbPts=NULL;

	SDS_getvar(NULL,DB_QUCSXDIR,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	ic_ptcpy(pt1,setgetrb.resval.rpoint);
	SDS_getvar(NULL,DB_QUCSYDIR,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	ic_ptcpy(pt2,setgetrb.resval.rpoint);
	pt210[0]=pt1[1]*pt2[2]-pt2[1]*pt1[2];
	pt210[2]=pt1[0]*pt2[1]-pt2[0]*pt1[1];
	pt210[1]=pt1[2]*pt2[0]-pt2[2]*pt1[0];

	rbucs.restype=rbwcs.restype=RTSHORT;
	rbucs.resval.rint=1;
	rbwcs.resval.rint=0;

//	if (sds_initget(0,"Convert_spline-fit_plines|Convert ~Object")!=RTNORM) { lErrorLine=(-__LINE__); goto bail; }
//	if ((nRet=internalGetpoint(NULL,"\nConvert polyline/<First point for spline>: ",pt1))==RTCAN || nRet==RTERROR || nRet==RTNONE) goto bail;
	if (sds_initget(0, ResourceString(IDC_SPLINE_INIT1, "~Convert ~Object ~_Convert ~_Object"))!=RTNORM) {
		lErrorLine=(-__LINE__);
		goto bail;
	}
	if ((nRet=internalGetpoint(NULL,ResourceString(IDC_CMDS3__NFIRST_POINT_FO_555, "\nFirst point for spline: " ),pt1))==RTCAN || nRet==RTERROR || nRet==RTNONE)
		goto bail;
	if (nRet==RTNORM) {
		if (rbPts==NULL)
			rbPts=rbPtsNext=sds_buildlist(11,pt1,0);
		else{
			rbPtsNext->rbnext=sds_buildlist(11,pt1,0);
			rbPtsNext=rbPtsNext->rbnext;

			if (NULL != rbPtsNext)
				rbPtsNext->rbnext=NULL;
		}
		nPts=1;
		ic_ptcpy(ptStart,pt1);
		if (sds_initget(0,""/*DNT*/)!=RTNORM) {
			lErrorLine=(-__LINE__);
			goto bail;
		}
		if ((nRet=internalGetpoint(pt1,ResourceString(IDC_CMDS3__NSECOND_POINT___556, "\nSecond point: " ),pt2))==RTCAN || nRet==RTERROR || nRet==RTNONE)
			goto bail;
		rbPtsNext->rbnext=sds_buildlist(11,pt2,0);
		rbPtsNext=rbPtsNext->rbnext;
		sds_grdraw(pt1,pt2,-1,0);
		if (!icadRealEqual(pt1[2],pt2[2])){
			nFlag&=(~IC_SPLINE_PLANAR);//set planar bit OFF
		}
		ic_ptcpy(pt1,pt2);
		nPts++;

		for (;;) {		// Go through these prompts until user has entered all info they want
			if (nPts>=2){
				if (sds_initget(0, ResourceString(IDC_SPLINE_INIT2, "Close Fit_tolerance|Fit ~Tolerance ~_Close ~_Fit ~_Tolerance"))!=RTNORM) {
					lErrorLine=(-__LINE__);
					goto bail;
				}
				if (RTCAN==(nRet=internalGetpoint(pt1,ResourceString(IDC_CMDS3__NCLOSE___FIT_TO_558, "\nClose/Fit tolerance/<Next point>: " ),pt2)) || nRet==RTERROR)
					goto bail;
			}else{
				if (sds_initget(0, ResourceString(IDC_SPLINE_INIT3, "Fit_tolerance|Fit ~Tolerance ~_Fit ~_Tolerance"))!=RTNORM) {
					lErrorLine=(-__LINE__);
					goto bail;
				}
				if (RTCAN==(nRet=internalGetpoint(pt1,ResourceString(IDC_CMDS3__NFIT_TOLERANCE__560, "\nFit tolerance/<Next point>: " ),pt2)) || nRet==RTERROR)
					goto bail;
			}
			if (nRet==RTNORM) {
				rbPtsNext->rbnext=sds_buildlist(11,pt2,0);
				rbPtsNext=rbPtsNext->rbnext;
				rbPtsNext->rbnext=NULL;
				sds_grdraw(pt1,pt2,-1,0);
				if (!icadRealEqual(pt1[2],pt2[2])){
					nFlag&=(~IC_SPLINE_PLANAR);//set planar bit OFF
				}
				ic_ptcpy(pt1,pt2);
				nPts++;
			} else if (nRet==RTNONE) {
				break;
			} else if (nRet==RTKWORD) {
				nRet=sds_getinput(fs1);
		if (strisame(fs1,"CLOSE"/*DNT*/)) {
					nFlag|=1;
					rbPtsNext->rbnext=sds_buildlist(11,ptStart,0);
					rbPtsNext=rbPtsNext->rbnext;
					rbPtsNext->rbnext=NULL;
					sds_grdraw(pt1,ptStart,-1,0);
					// Get tangent angle for closed spline:
					if (sds_initget(0,""/*DNT*/)!=RTNORM) {
						lErrorLine=(-__LINE__);
						goto bail;
					}
					if ((nRet=internalGetpoint(ptStart,ResourceString(IDC_CMDS3__NENTER_TANGENT__562, "\nEnter tangent: " ),ptTang1))==RTCAN || nRet==RTERROR)
						goto bail;
					//if nRet==RTNORM just continue;
					nFlag |= IC_SPLINE_PERIODIC;
					if (nRet==RTNONE) {
						ptTang1[0]=ptTang1[1]=ptTang1[2]=0.0;
						//ic_ptcpy(ptTang1,rbPts->rbnext->resval.rpoint);
						// Accepts ENTER - goes straight into first direction
					}else{
						ptTang1[0]=ptStart[0]-ptTang1[0];
						ptTang1[1]=ptStart[1]-ptTang1[1];
						ptTang1[2]=ptStart[2]-ptTang1[2];

						fr1=sqrt( (ptTang1[0] * ptTang1[0]) + (ptTang1[1] * ptTang1[1]) + (ptTang1[2] * ptTang1[2]));
						if (1.0 != fr1 && 0.0 != fr1)
						{
							ptTang1[0]/=fr1;
							ptTang1[1]/=fr1;
							ptTang1[2]/=fr1;
						}
					}
					ic_ptcpy(ptTang2,ptTang1);
					nPts++;
					break;
		} else if (strisame(fs1,"FIT"/*DNT*/) || strisame(fs1,"TOLERANCE"/*DNT*/)) {
					if (sds_initget(RSG_NONEG,""/*DNT*/)!=RTNORM) {
						lErrorLine=(-__LINE__);
						goto bail;
					}
					sds_rtos(srFitTol,-1,-1,fs1);
					sprintf(fs2,ResourceString(IDC_CMDS3__NENTER_FIT_TOLE_565, "\nEnter fit tolerance (in drawing units) <%s>: " ),fs1);
					if ((nRet=sds_getdist(NULL,fs2,&fr1))==RTCAN || nRet==RTERROR)
						goto bail;
					//sds_printf("\nTESTING:  Fit tolerance is %f",rFitTol);
					if (RTNORM==nRet)srFitTol=fr1;

					// Fit tolerance is number of units slop around each selected point (according to ACAD manual)
					// The whole current spline is recalculated based on the new fit tolerance.
				}
			}
		}
		if (!(nFlag & IC_SPLINE_CLOSED)) {
			// Get starting tangent:
			if (sds_initget(0,""/*DNT*/)!=RTNORM) {
				lErrorLine=(-__LINE__);
				goto bail;
			}
			if ((nRet=internalGetpoint(ptStart,ResourceString(IDC_CMDS3__NSELECT_STARTIN_566, "\nSelect starting tangent point: " ),ptTang1))==RTCAN || nRet==RTERROR)
				goto bail;
			//if nRet==RTNORM just continue;
			if (nRet==RTNONE) {
				// Accepts ENTER - goes straight into first direction
				//ptTang1[0]=rbPts->rbnext->resval.rpoint[0]-ptStart[0];
				//ptTang1[1]=rbPts->rbnext->resval.rpoint[1]-ptStart[1];
				//ptTang1[2]=rbPts->rbnext->resval.rpoint[1]-ptStart[2];
				ptTang1[0]=ptTang1[1]=ptTang1[2]=0.0;
			}else{
				ptTang1[0]=ptStart[0]-ptTang1[0];
				ptTang1[1]=ptStart[1]-ptTang1[1];
				ptTang1[2]=ptStart[2]-ptTang1[2];

				fr1=sqrt((ptTang1[0] * ptTang1[0]) + (ptTang1[1] * ptTang1[1]) + (ptTang1[2]*ptTang1[2]));
				if (1.0 != fr1 && 0.0 != fr1)
				{
					ptTang1[0]/=fr1;
					ptTang1[1]/=fr1;
					ptTang1[2]/=fr1;
				}
			}
			//sds_printf("\nTESTING:  Starting tangent is %g",rStartTan);

			// Get ending tangent:
			if (sds_initget(0,""/*DNT*/)!=RTNORM) {
				lErrorLine=(-__LINE__);
				goto bail;
			}
			if ((nRet=internalGetpoint(pt1,ResourceString(IDC_CMDS3__NENTER_TANGENT__567, "\nEnter tangent for ending point: " ),ptTang2))==RTCAN || nRet==RTERROR)
				goto bail;
			//if (nRet==RTNORM) just continue
			if (nRet==RTNONE) {
				/*
				for(rbSpline=rbPts;rbSpline->rbnext!=NULL;rbSpline=rbSpline->rbnext){
					ic_ptcpy(ptTang2,rbSpline->resval.rpoint);
				}
				rbSpline=NULL;
				ptTang2[0]=pt2[0]-ptTang2[0];
				ptTang2[1]=pt2[1]-ptTang2[1];
				ptTang2[2]=pt2[2]-ptTang2[2];*/
				ptTang2[0]=ptTang2[1]=ptTang2[2]=0.0;
			}else{
				ptTang2[0]-=pt2[0];
				ptTang2[1]-=pt2[1];
				ptTang2[2]-=pt2[2];

				fr1=sqrt((ptTang2[0] * ptTang2[0]) + (ptTang2[1] * ptTang2[1]) + (ptTang2[2] * ptTang2[2]));
				if (1.0 != fr1 && 0.0 != fr1)
				{
					ptTang2[0]/=fr1;
					ptTang2[1]/=fr1;
					ptTang2[2]/=fr1;
				}
			//sds_printf("\nTESTING:  Ending tangent is %g",rEndTan);
			}
		}
		//we're done getting pts & tangents, so go ahead and make the spline
		//we'll need to convert the pts and tangents from ucs to wcs
		if (!(nFlag & IC_SPLINE_PERIODIC)){
			sds_trans(ptTang1,&rbucs,&rbwcs,1,ptTang1);
			sds_trans(ptTang2,&rbucs,&rbwcs,1,ptTang2);
		}
	    if (NULL==(rbSpline=sds_buildlist(RTDXF0,"SPLINE"/*DNT*/,70,nFlag,74,nPts,12,ptTang1,13,ptTang2,42,SPLINE_KNOT_TOL,43,SPLINE_CTRL_TOL,44,srFitTol,210,pt210,0))){
			nRet=RTERROR;
			goto bail;
		}
		for(rbPtsNext=rbPts;rbPtsNext!=NULL;rbPtsNext=rbPtsNext->rbnext){
			sds_trans(rbPtsNext->resval.rpoint,&rbucs,&rbwcs,0,rbPtsNext->resval.rpoint);
		}
		for(rbPtsNext=rbSpline;rbPtsNext->rbnext!=NULL;rbPtsNext=rbPtsNext->rbnext);
		rbPtsNext->rbnext=rbPts;
		nRet=sds_entmake(rbSpline);
		//NOTE: ALWAYS UNCOUPLE THE JOINED LLISTS FOR PROPER FREEING!!
		rbPtsNext->rbnext=NULL;
		//transform pts back to ucs, because we need to un-draw on screen
		for(rbPtsNext=rbPts;rbPtsNext!=NULL;rbPtsNext=rbPtsNext->rbnext){
			sds_trans(rbPtsNext->resval.rpoint,&rbwcs,&rbucs,0,rbPtsNext->resval.rpoint);
		}
		if (RTNORM==nRet){
			sds_entlast(ent2redraw);
			if (nFlag & IC_SPLINE_PERIODIC){	//for speed
				db_handitem *hipspline;
				int i74;
				double d44;

				hipspline=(db_handitem *)ent2redraw[0];
				hipspline->get_74(&i74);
				hipspline->get_44(&d44);
				gr_get_spline_tangents(i74,(sds_point *)hipspline->retp_11(0),d44,1,hipspline->retp_12(),hipspline->retp_13());
			}
		}

	} else if (nRet=RTKWORD) {		// Both keywords (Object & Convert) do same thing here, so no further comparing.

		cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0);
		goto bail;
//		if (sds_initget(0,"")!=RTNORM) { lErrorLine=(-__LINE__); goto bail; }

		//if ((nRet=sds_entsel("\nSelect spline-fitted polylines to convert to splines: ",nmEntity,pt1))==RTCAN || nRet==RTERROR) goto bail;
		// Remember sds_nentselp, if you need the nearest vertex.  Sds_entsel gets you the polyline header.
			// Verify selected entity is a spline-fitted polyline
			//		  if (/*selected-entity-is-not-a-spline-fitted-polyline*/) {
			//			  cmd_ErrorPrompt(CMD_ERR_UNABLETOSPLINE,0);
			//			  lErrorLine=(-__LINE__); goto bail;
			//		  }

	} else {
		goto bail;

	}


bail:
	if (rbPts!=NULL){
		ic_ptcpy(pt1,rbPts->resval.rpoint);
		for(rbPtsNext=rbPts->rbnext;rbPtsNext!=NULL;rbPtsNext=rbPtsNext->rbnext){
			sds_grdraw(pt1,rbPtsNext->resval.rpoint,-1,0);
			ic_ptcpy(pt1,rbPtsNext->resval.rpoint);
		}
		if (0L!=ent2redraw[0])
			sds_redraw(ent2redraw,IC_REDRAW_DRAW);
		sds_relrb(rbPts);
		rbPts=NULL;
	}
	if (rbSpline!=NULL){sds_relrb(rbSpline);rbSpline=NULL;}

	if (lErrorLine==0L)
		return(RTNORM);
	if (lErrorLine==(-1L))
		return(RTCAN);
	if (lErrorLine<(-1L))
		CMD_INTERNAL_MSG(lErrorLine);
	return(nRet);

	return(RTNORM);
}

short cmd_splinedit(void)  {
	cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0);
	SDS_FreeEventList(0);
	return(RTNORM);
}
/*


	sds_printf(ResourceString(IDC_CMDS3__NTHE_SPLINEDIT__568, "\nThe SplinEdit function is under construction.  It prompts, but has no effect" ));



	sds_name		nmEntity;				// The entity to be converted to a spline
	sds_point		ptPick,					// Point by which nmEntity was picked
					ptCurrent,				// current location of the selected control point
					ptNew;					// New location for the selected control point
	char			szKeyWord[IC_ACADBUF];	// Keyword from command line
					//szPrompt[IC_ACADBUF];	// For making up complicated prompt strings with %s
	int				nRet=RTNORM;			// This function's return value.
	long			lErrorLine=0L;			// Gives the line number if there is an error.
	short			swClosed=0,				// Is spline closed?  Close option only available if it's open.
					swHasFitData=1;			// Does spline have associated fit data?
					// The above should be set to 0 at first.  It is set at 1 to test some prompts.  Change it back.

	if (sds_initget(0,ResourceString(IDC_CMDS3__68, "" ))!=RTNORM) { lErrorLine=(-__LINE__); goto bail; }
	if ((nRet=sds_entsel(ResourceString(IDC_CMDS3__NSELECT_SPLINE__569, "\nSelect spline to edit: " ),nmEntity,ptPick))==RTCAN || nRet==RTERROR) goto bail;
	// Check to be sure the selected entity is actually a spline (either from SPLINE or PLINE/SPLINE)

	// bogus code	 Redo these the right way:
	// If (nmEntity!=spline) goto bail;
	// If (nmEntity has fit data) swHasFitData=1);
	// if (nmEntity is closed) swClosed=1);

	// TODO - all the Undo's need to toggle off and on, depending on whether there is anything to undo.  Maybe.

	for (;;) {
		if (swClosed && swHasFitData) {
			LOAD_COMMAND_OPTIONS_2(IDC_CMDS3_EDIT_CONTROL_DAT_570)
			if (sds_initget(0, LOADEDSTRING)!=RTNORM) { lErrorLine=(-__LINE__); goto bail; }
			if ((nRet=sds_getkword(ResourceString(IDC_CMDS3__NEDIT_CONTROL_D_571, "\nEdit control Data/Open spline/Move Vertex/Refine/rEverse/Undo/<eXit>: " ),szKeyWord))==RTCAN || nRet==RTERROR) goto bail;
		}else if (swClosed && !swHasFitData){
			if (sds_initget(0,ResourceString(IDC_CMDS3_OPEN_SPLINE_OPEN_572, "Open_spline|Open Move_vertex|Move ~Vertex Refine Reverse_direction|rEverse ~ Undo Exit|eXit" ))!=RTNORM) { lErrorLine=(-__LINE__); goto bail; }
			if ((nRet=sds_getkword(ResourceString(IDC_CMDS3__NOPEN_SPLINE____573, "\nOpen spline/Move Vertex/Refine/rEverse/Undo/<eXit>: " ),szKeyWord))==RTCAN || nRet==RTERROR) goto bail;
		}else if (!swClosed && swHasFitData){
			LOAD_COMMAND_OPTIONS_2(IDC_CMDS3_EDIT_CONTROL_DAT_574)
			if (sds_initget(0, LOADEDSTRING)!=RTNORM) { lErrorLine=(-__LINE__); goto bail; }
			if ((nRet=sds_getkword(ResourceString(IDC_CMDS3__NEDIT_CONTROL_D_575, "\nEdit control Data/Close spline/Move Vertex/Refine/rEverse/Undo/<eXit>: " ),szKeyWord))==RTCAN || nRet==RTERROR) goto bail;
		}else {		// Not closed, and has no Fit data
			if (sds_initget(0,ResourceString(IDC_CMDS3_CLOSE_SPLINE_CLO_576, "Close_spline|Close Move_vertex|Move ~Vertex Refine Reverse_direction|rEverse ~ Undo Exit|eXit" ))!=RTNORM) { lErrorLine=(-__LINE__); goto bail; }
			if ((nRet=sds_getkword(ResourceString(IDC_CMDS3__NCLOSE_SPLINE___577, "\nClose spline/Move Vertex/Refine/rEverse/Undo/<eXit>: " ),szKeyWord))==RTCAN || nRet==RTERROR) goto bail;
		}
		if (nRet==RTNORM) {
			if ((strcmp(szKeyWord,ResourceString(IDC_CMDS3_FIT_563, "Fit" ))==0) || (strcmp(szKeyWord,ResourceString(IDC_CMDS3_DATA_578, "Data" ))==0) || (strcmp(szKeyWord,ResourceString(IDC_CMDS3_EDIT_579, "Edit" ))==0)) { 	// In ACAD, F and D *both* work for "Fit Data"
				for(;;) {
					if (swClosed) {
						LOAD_COMMAND_OPTIONS_2(IDC_CMDS3_ADD_CONTROL_POIN_580)
						if (sds_initget(0, LOADEDSTRING)!=RTNORM) { lErrorLine=(-__LINE__); goto bail; }
						if ((nRet=sds_getkword(ResourceString(IDC_CMDS3__NADD___OPEN___D_581, "\nAdd/Open/Delete data/Move/Purge/Tangents/toLerance/<eXit>: " ),szKeyWord))==RTCAN || nRet==RTERROR) goto bail;
					} else {
						if (sds_initget(0,ResourceString(IDC_CMDS3_ADD_CONTROL_POIN_582, "Add_control_points|Add Close_spline|Close Delete_control_data|Delete Move_control_points|Move Purge Tangents Tolerance|toLerance ~ Exit|eXit" ))!=RTNORM) { lErrorLine=(-__LINE__); goto bail; }
						if ((nRet=sds_getkword(ResourceString(IDC_CMDS3__NADD___CLOSE____583, "\nAdd/Close/Delete data/Move/Purge/Tangents/toLerance/<eXit>: " ),szKeyWord))==RTCAN || nRet==RTERROR) goto bail;
					}
					if (nRet==RTNORM) {
						if ((stricmp(szKeyWord,ResourceString(IDC_CMDS3_ADD_425, "Add" ))==0)) {
						} else if ((strcmp(szKeyWord,ResourceString(IDC_CMDS3_CLOSE_561, "Close" ))==0)) {
						} else if ((strcmp(szKeyWord,ResourceString(IDC_CMDS3_OPEN_584, "Open" ))==0)) {
						} else if ((strcmp(szKeyWord,ResourceString(IDC_CMDS3_DELETE_297, "Delete" ))==0)) {
						} else if ((strcmp(szKeyWord,ResourceString(IDC_CMDS3_MOVE_585, "Move" ))==0)) {
						} else if ((strcmp(szKeyWord,ResourceString(IDC_CMDS3_PURGE_586, "Purge" ))==0)) {
							// Purge deletes the fit data.	I'm setting the variable explicitly here, but you should
							// do a check to see if there actually is any fit data, and set the variable based on that.
							swHasFitData=0;
						} else if ((strcmp(szKeyWord,ResourceString(IDC_CMDS3_TANGENTS_587, "Tangents" ))==0)) {
						} else if ((strcmp(szKeyWord,ResourceString(IDC_CMDS3_TOLERANCE_588, "toLerance" ))==0)) {
						} else if ((strcmp(szKeyWord,ResourceString(IDC_CMDS3_EXIT_589, "eXit" ))==0)) {
							break;
						}
					}
				}
			} else if (strcmp(szKeyWord,ResourceString(IDC_CMDS3_CLOSE_561, "Close" ))==0) {
				// Close the spline
				swClosed=1;
			} else if (strcmp(szKeyWord,ResourceString(IDC_CMDS3_OPEN_584, "Open" ))==0) {
				// Open the spline (just de-tangent's the closed part, I think)
				swClosed=0;
			} else if ((strcmp(szKeyWord,ResourceString(IDC_CMDS3_MOVE_585, "Move" ))==0) || (strcmp(szKeyWord,ResourceString(IDC_CMDS3_VERTEX_590, "Vertex" ))==0)) {	// In ACAD, M and V *both* work for "Move Vertex"
				for(;;) {
					// Get the starting point for the spline
					ptCurrent[0]=0;		// I'm just doing this for testing - take it out later.
					ptCurrent[1]=0;
					ptCurrent[2]=0;
					if (sds_initget(0,ResourceString(IDC_CMDS3_NEXT_PREVIOUS____591, "Next Previous ~ Select_control_point|Select ~ Exit|eXit" ))!=RTNORM) { lErrorLine=(-__LINE__); goto bail; }
					if ((nRet=internalGetpoint(ptCurrent,ResourceString(IDC_CMDS3__NNEXT___PREVIOU_592, "\nNext/Previous/Select point/eXit/<Enter new location>: " ),ptNew))==RTCAN || nRet==RTERROR) goto bail;
					if (nRet==RTNORM) {
						// Change the current point to ptNew
					} else if (nRet==RTKWORD) {
						nRet=sds_getinput(szKeyWord);
						if ((strcmp(szKeyWord,ResourceString(IDC_CMDS3_NEXT_593, "Next" ))==0)) {
						} else if ((strcmp(szKeyWord,ResourceString(IDC_CMDS3_PREVIOUS_594, "Previous" ))==0)) {
						} else if ((strcmp(szKeyWord,ResourceString(IDC_CMDS3_SELECT_595, "Select" ))==0)) {
						} else if ((strcmp(szKeyWord,ResourceString(IDC_CMDS3_EXIT_589, "eXit" ))==0)) {
							break;
						}
					}
				}


			} else if (strcmp(szKeyWord,ResourceString(IDC_CMDS3_REFINE_596, "Refine" ))==0) {
//				for (;;) {
//					if (sds_initget(0,"Add_control_point|Add Elevate_order|Elevate  Exit|eXit")!=RTNORM) { lErrorLine=(-__LINE__); goto bail; }
//					if ((nRet=sds_getkword("\nx/x/x/x/x/x/x/<eXit>: ",szKeyWord))==RTCAN || nRet==RTERROR) goto bail;
//					if (nRet==RTNORM) break;
//					if (nRet==RTERROR)
//						goto bail;
//					if (nRet==RTKWORD) {
//						nRet=sds_getinput(szKeyWord);
//						if ((stricmp(szKeyWord,"x")==0) || (stricmp(szKeyWord,"Justification")==0)) {
//						} else if ((strcmp(szKeyWord,"x")==0)) {
//						} else if ((strcmp(szKeyWord,"x")==0)) {
//						} else if ((strcmp(szKeyWord,"x")==0)) {
//						} else if ((strcmp(szKeyWord,"x")==0)) {
//						} else if ((strcmp(szKeyWord,"x")==0)) {
//						}
//

//				if (sds_initget(0," Exit|eXit")!=RTNORM) { lErrorLine=(-__LINE__); goto bail; }
//				if ((nRet=sds_getkword("\nx/x/x/x/x/x/x/<eXit>: ",szKeyWord))==RTCAN || nRet==RTERROR) goto bail;

				//	Add control point
				//	Elevate order
				//		Enter new order <current order>   then return to refine choices
				//	Weight
				//		"Spline is not rational.  Will make it so."
				//		Enter new weight <current weight>
				//		Next		(default switches from N N N to P when you hit the end of the spline)
				//		Previous
				//		Select Point
				//			Select Point
				//		eXit
				//	eXit

				swHasFitData=0;
				// Refine causes the spline to lose its fit data.  I'm setting the variable explicitly here, but you should
				// do a check to see if there actually is any fit data, and set the variable based on that.
			} else if (strcmp(szKeyWord,ResourceString(IDC_CMDS3_REVERSE_597, "rEverse" ))==0) {
				//
				// "Spline has been reversed."
			} else if (strcmp(szKeyWord,ResourceString(IDC_CMDS3_UNDO_598, "Undo" ))==0) {
				// Undoes in steps all the way to beginning of command.
			} else if ((strcmp(szKeyWord,ResourceString(IDC_CMDS3_EXIT_589, "eXit" ))==0)) {
				// Record all the editing done so far (?)
				break;
			} else {
				goto bail;
			}
		}
	}

bail:

	if (lErrorLine==0L) return(RTNORM);
	if (lErrorLine==(-1L)) return(RTCAN);
	if (lErrorLine<(-1L)) CMD_INTERNAL_MSG(-lErrorLine);
	return(nRet);

	return(RTNORM);
}
*/

//*********************************************************
short cmd_attredef(void)  {
	cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0);
	SDS_FreeEventList(0);
	return(RTNORM);
}
/*

	char			szPrompt[IC_ACADBUF],	// For making up complicated prompt strings with %s
					szKeyWord[IC_ACADBUF];	// Keyword from command line

	int				nRet=RTNORM;			// This function's return value.
	long			lErrorLine=0L;			// Gives the line number if there is an error.


	for (;;) {
		if (){
			if (sds_initget(0,ResourceString(IDC_CMDS3__68, "" ))!=RTNORM) { lErrorLine=(-__LINE__); goto bail; }
			if ((nRet=sds_get(ResourceString(IDC_CMDS3__N________599, "\n/<>: " )))==RTCAN || nRet==RTERROR) goto bail;
		}else{
			if (sds_initget(0,ResourceString(IDC_CMDS3__68, "" ))!=RTNORM) { lErrorLine=(-__LINE__); goto bail; }
			if ((nRet=sds_get(ResourceString(IDC_CMDS3__N________599, "\n/<>: " )))==RTCAN || nRet==RTERROR) goto bail;
		}
		if (nRet==RTNORM) break;
		if (nRet==RTERROR)
			goto bail;
		if (nRet==RTKWORD) {
			nRet=sds_getinput(szKeyWord);
			if ((stricmp(szKeyWord,ResourceString(IDC_CMDS3_X_600, "x" ))==0) || (stricmp(szKeyWord,ResourceString(IDC_CMDS3_JUSTIFICATION_442, "Justification" ))==0)) {
			} else if ((strcmp(szKeyWord,ResourceString(IDC_CMDS3_X_600, "x" ))==0)) {
			} else if ((strcmp(szKeyWord,ResourceString(IDC_CMDS3_X_600, "x" ))==0)) {
			} else if ((strcmp(szKeyWord,ResourceString(IDC_CMDS3_X_600, "x" ))==0)) {
			} else if ((strcmp(szKeyWord,ResourceString(IDC_CMDS3_X_600, "x" ))==0)) {
			} else if ((strcmp(szKeyWord,ResourceString(IDC_CMDS3_X_600, "x" ))==0)) {
			}

end:
	// put end stuff here if needed.

bail:

	if (lErrorLine==0L) return(RTNORM);
	if (lErrorLine==(-1L)) return(RTCAN);
	if (lErrorLine<(-1L)) CMD_INTERNAL_MSG(-lErrorLine);
	return(nRet);
}
*/

short cmd_oldmtext(CString text, CString style, double width, int rotation, int justify)
	{
	SDS_mTextDataPtr.nAlignment = justify;
	SDS_mTextDataPtr.rRotAngle = rotation;
	SDS_mTextDataPtr.rBoxWidth = width;
	strncpy(SDS_mTextDataPtr.szTextStyle, style.GetBuffer(0), 512);
	SDS_mTextDataPtr.text = text;
	// EBATECH(CNBR) Affect $MTEXTED
	LaunchMTEXTEditor( SDS_mTextDataPtr.text );
	//ExecuteUIAction(ICAD_WNDACTION_MTEXT);
	return 0;
	}

// Bugzilla No. 78034; 23-12-2002 [
short GetStyleHeight( char * pStyleName , sds_real &height )
{
	db_handitem   * pStyle = NULL;
	pStyle = SDS_CURDWG->findtabrec(DB_STYLETAB,pStyleName ,1);
	if ( pStyle != NULL )
	{
		pStyle->get_40(&height);
		return ( RTNORM );
	}
	return ( RTERROR );
}
// Bugzilla No. 78034; 23-12-2002 ]

short cmd_mtext(void)	{
	char szKeyWord[IC_ACADBUF],szPrompt[IC_ACADBUF],szTmp[IC_ACADBUF];
	szPrompt[0]=szKeyWord[0]=szTmp[0]=0;
	short swPointYet=0;
	RT ret;
	bool gotit=false;
	struct resbuf setgetrb;

	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002
	if (cmd_istransparent() == RTERROR) return RTERROR;

	SDS_getvar(NULL,DB_QTEXTSIZE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	SDS_mTextDataPtr.rTextHeight=setgetrb.resval.rreal;
	SDS_mTextDataPtr.text = _T(""); // EBATECH(CNBR)
	SDS_mTextDataPtr.rRotAngle	=0.0;
	SDS_mTextDataPtr.nAlignment =1;
	SDS_mTextDataPtr.nDirection =1;
	SDS_mTextDataPtr.rBoxWidth=0;
 	//DP: we decide that initial string is empty
 	SDS_mTextDataPtr.text.Empty();
	SDS_getvar(NULL,DB_QTEXTSTYLE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	strcpy(SDS_mTextDataPtr.szTextStyle,setgetrb.resval.rstring);
	if (setgetrb.resval.rstring){IC_FREE(setgetrb.resval.rstring);setgetrb.resval.rstring=NULL;}
	// Bugzilla No. 78034; 23-12-2002 [
	sds_real height = 0.0;
	if ( GetStyleHeight(SDS_mTextDataPtr.szTextStyle, height ) == RTNORM && !icadRealEqual(height,0.0))
		SDS_mTextDataPtr.rTextHeight= height;
	// Bugzilla No. 78034; 23-12-2002 ]

	sds_initget(1,NULL);
	if ((ret=internalGetpoint(NULL,ResourceString(IDC_CMDS3__NMULTILINE_TEXT_601, "\nMultiline Text:  First corner for block of text: " ),SDS_mTextDataPtr.ptInsertion))!=RTNORM)
		goto bail;
	// EBATECH-[ Point initilize
	SDS_mTextDataPtr.ptOppCorner[0] = SDS_mTextDataPtr.ptInsertion[0];
	SDS_mTextDataPtr.ptOppCorner[1] = SDS_mTextDataPtr.ptInsertion[1];
	SDS_mTextDataPtr.ptOppCorner[2] = SDS_mTextDataPtr.ptInsertion[2];
	// ]-EBATECH
	for (;;) {
		sds_initget(128,ResourceString(IDC_MTEXT_INIT1, "Justification|Justification Rotation_angle|Rotation Text_style|Style Text_height|Height Direction|Direction Width|Width ~_Justification ~_Rotation ~_Style ~_Height ~_Direction ~_Width"));
		if ((ret=sds_getcorner(SDS_mTextDataPtr.ptInsertion,ResourceString(IDC_CMDS3__NJUSTIFICATION__603, "\nJustification/Rotation/Style/Height/Direction/Width/<Opposite corner for block of text>: " ),SDS_mTextDataPtr.ptOppCorner))==RTERROR) {
			goto bail;
		}else if (ret==RTCAN){
			goto bail;
		}else if (ret==RTNONE){
			cmd_ErrorPrompt(CMD_ERR_2DPTKWORD,0);
			continue;
		}else if (ret==RTKWORD) {
			if ((ret=sds_getinput(szKeyWord))!=RTNORM)
				goto bail;
	  if (strisame(szKeyWord,"JUSTIFICATION"/*DNT*/)) {
				// Get horizontal & vertical alignment
				sds_initget(128, ResourceString(IDC_MTEXT_INIT2, "Top-Left|TL Top-Center|TC Top-Right|TR ~ Middle-Left|ML Middle-Center|MC Middle-Right|MR ~ Bottom-Left|BL Bottom-Center|BC Bottom-Right|BR ~_TL ~_TC ~_TR ~_ ~_ML ~_MC ~_MR ~_ ~_BL ~_BC ~_BR"));
				if ((ret=sds_getkword(ResourceString(IDC_CMDS3__NSPECIFY_TEXT_B_605, "\nSpecify text block justification:  TL/TC/TR/ML/MC/MR/BL/BC/BR/<Top Left> " ),szKeyWord))==RTERROR) {
					goto bail;
				}else if (ret==RTCAN){
					goto bail;
				}else if (ret==RTNORM){
		  if ((strisame(szKeyWord,"TL"/*DNT*/))) {
						SDS_mTextDataPtr.nAlignment=1;
		  } else if (strisame(szKeyWord,"TC"/*DNT*/)) {
						SDS_mTextDataPtr.nAlignment=2;
		  } else if (strisame(szKeyWord,"TR"/*DNT*/)) {
						SDS_mTextDataPtr.nAlignment=3;

		  } else if (strisame(szKeyWord,"ML"/*DNT*/)) {
						SDS_mTextDataPtr.nAlignment=4;
		  } else if (strisame(szKeyWord,"MC"/*DNT*/)) {
						SDS_mTextDataPtr.nAlignment=5;
		  } else if (strisame(szKeyWord,"MR"/*DNT*/)) {
						SDS_mTextDataPtr.nAlignment=6;

		  } else if (strisame(szKeyWord,"BL"/*DNT*/)) {
						SDS_mTextDataPtr.nAlignment=7;
		  } else if (strisame(szKeyWord,"BC"/*DNT*/)) {
						SDS_mTextDataPtr.nAlignment=8;
		  } else if (strisame(szKeyWord,"BR"/*DNT*/)) {
						SDS_mTextDataPtr.nAlignment=9;
					}
				}
	  }else if (strisame(szKeyWord,"ROTATION"/*DNT*/)) {
				// Get rotation angle
				do{
					sds_angtos(SDS_mTextDataPtr.rRotAngle,-1,-1,szTmp);
					sprintf(szPrompt,ResourceString(IDC_CMDS3__NROTATION_ANGLE_616, "\nRotation angle for block of text <%s>: " ),szTmp);
					if ((ret=sds_getangle(SDS_mTextDataPtr.ptInsertion,szPrompt,&SDS_mTextDataPtr.rRotAngle))==RTERROR) {
						goto bail;
					}else if (ret==RTCAN){
						goto bail;
					}else{
						break;
					}
				}while(1);
	  }else if (strisame(szKeyWord,"HEIGHT"/*DNT*/)) {
				do{
					sds_rtos(SDS_mTextDataPtr.rTextHeight,-1,-1,szTmp);
					sprintf(szPrompt,ResourceString(IDC_CMDS3__NHEIGHT_OF_TEXT_618, "\nHeight of text <%s>: " ),szTmp);
					if ((ret=sds_getreal(szPrompt,&SDS_mTextDataPtr.rTextHeight))==RTERROR) {
						goto bail;
					}else if (ret==RTCAN){
						goto bail;
					}else{
						break;
					}
				}while(1);
			}else if (strisame(szKeyWord,"STYLE"/*DNT*/)) {
				do{
					sprintf(szPrompt,ResourceString(IDC_CMDS3__NTEXT_STYLE_TO__619, "\nText style to use (or '?') <%s>: " ),SDS_mTextDataPtr.szTextStyle);
					sds_initget(128,NULL);
					if ((ret=sds_getstring(1,szPrompt,szKeyWord))==RTERROR) {
						goto bail;
					}else if (ret==RTCAN){
						goto bail;
					}else if (ret==RTNORM){
						ic_trim(szKeyWord,"be");
						strupr(szKeyWord);
						if (*szKeyWord=='?'){
							sds_textscr();
							SDS_StartTblRead("STYLE"/*DNT*/);
							while(SDS_TblReadHasMore()) {
								sds_printf(ResourceString(IDC_CMDS3__N_S_620, "\n%s" ),SDS_TblReadString());
							}
						}else{
							SDS_StartTblRead("STYLE"/*DNT*/);
							while(SDS_TblReadHasMore()) {
								strcpy(szPrompt,SDS_TblReadString());
								//if (strchr(szPrompt,'|'/*DNT*/)!=NULL) continue; //watch for xref styles EBATECH(CNBR) miss
								if (strchr(szPrompt,'|'/*DNT*/)!=NULL) continue; //watch for xref styles
								if (sds_wcmatch(szPrompt,szKeyWord)==RTNORM) {
									strcpy(SDS_mTextDataPtr.szTextStyle,szPrompt);
									gotit=true;
									break;
								}
							}
							if (gotit==false){
								cmd_ErrorPrompt(CMD_ERR_FINDSTYLE,0);
							}
						}
					}
					// EBATECH(CNBR) -[ default value: break loop if you hit [Enter] or [SPACE].
					else if (ret==RTNONE){
						gotit=true;
					}
					// EBATECH(CNBR) ]-
				}while(gotit==false);
			}else if (strisame(szKeyWord,"DIRECTION"/*DNT*/)) {
				// EBATECH(CNBR) -[ ADD 5=Depend as STYLE
				sds_initget(0,ResourceString(IDC_CMDS3_LEFT_TO_RIGHT_LR_622,
					"Left-to-Right|L Right-to-Left|R Top-to-Bottom|T Bottom-to-Top|B Style|S ~_L ~_R ~_T ~_B ~_S"));
				if ((ret=sds_getkword(ResourceString(IDC_CMDS3__NSPECIFY_DRAWIN_623,
					"\nSpecify drawing direction:  Left-to-right/Right-to-left/Top-to-bottom/Bottom-to-Top/Style<Left-to-right (L)> " ),szKeyWord))==RTERROR) {
					goto bail;
				}else if (ret==RTCAN){
					goto bail;
				}else if (ret==RTNONE){	// Same as L option (Left-to-right).
					SDS_mTextDataPtr.nDirection=1;
				}else if (ret==RTNORM){
					if (strisame(szKeyWord,"L"/*DNT*/)) {
						SDS_mTextDataPtr.nDirection=1;
					} else if (strisame(szKeyWord,"R"/*DNT*/)) {
						SDS_mTextDataPtr.nDirection=2;
					} else if (strisame(szKeyWord,"T"/*DNT*/)) {
						SDS_mTextDataPtr.nDirection=3;
					} else if (strisame(szKeyWord,"B"/*DNT*/)) {
						SDS_mTextDataPtr.nDirection=4;
					} else if (strisame(szKeyWord,"S"/*DNT*/)) {
						SDS_mTextDataPtr.nDirection=5;
					}
				}
				// ]- EBATECH(CNBR)
			}else if (strisame(szKeyWord,"WIDTH"/*DNT*/)) {
				do{
					if ((ret=sds_getdist(SDS_mTextDataPtr.ptInsertion,ResourceString(IDC_CMDS3_WIDTH_OF_TEXT_BO_628, "Width of text box: " ),&SDS_mTextDataPtr.rBoxWidth))==RTERROR) {
						goto bail;
					}else if (ret==RTCAN){
						goto bail;
					}else if (ret==RTNONE){
						break;
					}else if (ret==RTNORM){
						// EBATECH(CNBR) Affect $MTEXTED
						LaunchMTEXTEditor( SDS_mTextDataPtr.text, TRUE );
						//ExecuteUIAction(ICAD_WNDACTION_MTEXT);
						return(ret);
					}
				}while(1);
			}
		}else if (ret==RTNORM){
			SDS_mTextDataPtr.rBoxWidth=(SDS_mTextDataPtr.ptOppCorner[0]>SDS_mTextDataPtr.ptInsertion[0])
									  ?(SDS_mTextDataPtr.ptOppCorner[0]-SDS_mTextDataPtr.ptInsertion[0])
									  :(SDS_mTextDataPtr.ptInsertion[0]-SDS_mTextDataPtr.ptOppCorner[0]);
			break;
		}
	}
	// EBATECH(CNBR) Affect $MTEXTED
	LaunchMTEXTEditor( SDS_mTextDataPtr.text, TRUE );
	//ExecuteUIAction(ICAD_WNDACTION_MTEXT);

	ret=RTNORM;
bail:

	return(ret);
}


short cmd_cui_mtext(void)	{
	extern int CreateMTEXTObject( void );

	char szKeyWord[IC_ACADBUF],szPrompt[IC_ACADBUF],szTmp[IC_ACADBUF];
	short swPointYet=0;
	RT ret;
	bool gotit=false;
	struct resbuf setgetrb;

	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002
	if (cmd_istransparent() == RTERROR) return RTERROR;

	SDS_getvar(NULL,DB_QTEXTSIZE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	SDS_mTextDataPtr.rTextHeight=setgetrb.resval.rreal;
	SDS_mTextDataPtr.text = _T(""); // EBATECH(CNBR)
	SDS_mTextDataPtr.rRotAngle	=0.0;
	SDS_mTextDataPtr.nAlignment =1;
	SDS_mTextDataPtr.nDirection =1;
	SDS_mTextDataPtr.rBoxWidth=0;
	//DP: we decide that initial string is empty
	SDS_mTextDataPtr.text.Empty();
	SDS_getvar(NULL,DB_QTEXTSTYLE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	strcpy(SDS_mTextDataPtr.szTextStyle,setgetrb.resval.rstring);
	if (setgetrb.resval.rstring){IC_FREE(setgetrb.resval.rstring);setgetrb.resval.rstring=NULL;}
	// Bugzilla No. 78034; 23-12-2002 [
	sds_real height = 0.0;
	if ( GetStyleHeight(SDS_mTextDataPtr.szTextStyle, height ) == RTNORM && !icadRealEqual(height,0.0))
		SDS_mTextDataPtr.rTextHeight= height;
	// Bugzilla No. 78034; 23-12-2002 ]

	sds_initget(1,NULL);
	if ((ret=internalGetpoint(NULL,ResourceString(IDC_CMDS3__NMULTILINE_TEXT_601, "\nMultiline Text:  First corner for block of text: " ),SDS_mTextDataPtr.ptInsertion))!=RTNORM)
		goto bail;
	// EBATECH-[ Point initilize
	SDS_mTextDataPtr.ptOppCorner[0] = SDS_mTextDataPtr.ptInsertion[0];
	SDS_mTextDataPtr.ptOppCorner[1] = SDS_mTextDataPtr.ptInsertion[1];
	SDS_mTextDataPtr.ptOppCorner[2] = SDS_mTextDataPtr.ptInsertion[2];
	// ]-EBATECH
	for (;;) {
		sds_initget(128,ResourceString(IDC_MTEXT_INIT1, "Justification|Justification Rotation_angle|Rotation Text_style|Style Text_height|Height Direction|Direction Width|Width ~_Justification ~_Rotation ~_Style ~_Height ~_Direction ~_Width"));
		if ((ret=sds_getcorner(SDS_mTextDataPtr.ptInsertion,ResourceString(IDC_CMDS3__NJUSTIFICATION__603, "\nJustification/Rotation/Style/Height/Direction/Width/<Opposite corner for block of text>: " ),SDS_mTextDataPtr.ptOppCorner))==RTERROR) {
			goto bail;
		}else if (ret==RTCAN){
			goto bail;
		}else if (ret==RTNONE){
			cmd_ErrorPrompt(CMD_ERR_2DPTKWORD,0);
			continue;
		}else if (ret==RTKWORD) {
			if ((ret=sds_getinput(szKeyWord))!=RTNORM)
				goto bail;
	  if (strisame(szKeyWord,"JUSTIFICATION"/*DNT*/)) {
				// Get horizontal & vertical alignment
				sds_initget(128, ResourceString(IDC_MTEXT_INIT2, "Top-Left|TL Top-Center|TC Top-Right|TR ~ Middle-Left|ML Middle-Center|MC Middle-Right|MR ~ Bottom-Left|BL Bottom-Center|BC Bottom-Right|BR ~_TL ~_TC ~_TR ~_ ~_ML ~_MC ~_MR ~_ ~_BL ~_BC ~_BR"));
				if ((ret=sds_getkword(ResourceString(IDC_CMDS3__NSPECIFY_TEXT_B_605, "\nSpecify text block justification:  TL/TC/TR/ML/MC/MR/BL/BC/BR/<Top Left> " ),szKeyWord))==RTERROR) {
					goto bail;
				}else if (ret==RTCAN){
					goto bail;
				}else if (ret==RTNORM){
		  if ((strisame(szKeyWord,"TL"/*DNT*/))) {
						SDS_mTextDataPtr.nAlignment=1;
		  } else if (strisame(szKeyWord,"TC"/*DNT*/)) {
						SDS_mTextDataPtr.nAlignment=2;
		  } else if (strisame(szKeyWord,"TR"/*DNT*/)) {
						SDS_mTextDataPtr.nAlignment=3;

		  } else if (strisame(szKeyWord,"ML"/*DNT*/)) {
						SDS_mTextDataPtr.nAlignment=4;
		  } else if (strisame(szKeyWord,"MC"/*DNT*/)) {
						SDS_mTextDataPtr.nAlignment=5;
		  } else if (strisame(szKeyWord,"MR"/*DNT*/)) {
						SDS_mTextDataPtr.nAlignment=6;

		  } else if (strisame(szKeyWord,"BL"/*DNT*/)) {
						SDS_mTextDataPtr.nAlignment=7;
		  } else if (strisame(szKeyWord,"BC"/*DNT*/)) {
						SDS_mTextDataPtr.nAlignment=8;
		  } else if (strisame(szKeyWord,"BR"/*DNT*/)) {
						SDS_mTextDataPtr.nAlignment=9;
					}
				}
	  }else if (strisame(szKeyWord,"ROTATION"/*DNT*/)) {
				// Get rotation angle
				do{
					sds_angtos(SDS_mTextDataPtr.rRotAngle,-1,-1,szTmp);
					sprintf(szPrompt,ResourceString(IDC_CMDS3__NROTATION_ANGLE_616, "\nRotation angle for block of text <%s>: " ),szTmp);
					if ((ret=sds_getangle(SDS_mTextDataPtr.ptInsertion,szPrompt,&SDS_mTextDataPtr.rRotAngle))==RTERROR) {
						goto bail;
					}else if (ret==RTCAN){
						goto bail;
					}else{
						break;
					}
				}while(1);
	  }else if (strisame(szKeyWord,"HEIGHT"/*DNT*/)) {
				do{
					sds_rtos(SDS_mTextDataPtr.rTextHeight,-1,-1,szTmp);
					sprintf(szPrompt,ResourceString(IDC_CMDS3__NHEIGHT_OF_TEXT_618, "\nHeight of text <%s>: " ),szTmp);
					if ((ret=sds_getreal(szPrompt,&SDS_mTextDataPtr.rTextHeight))==RTERROR) {
						goto bail;
					}else if (ret==RTCAN){
						goto bail;
					}else{
						break;
					}
				}while(1);
			}else if (strisame(szKeyWord,"STYLE"/*DNT*/)) {
				do{
					sprintf(szPrompt,ResourceString(IDC_CMDS3__NTEXT_STYLE_TO__619, "\nText style to use (or '?') <%s>: " ),SDS_mTextDataPtr.szTextStyle);
					sds_initget(128,NULL);
					if ((ret=sds_getstring(1,szPrompt,szKeyWord))==RTERROR) {
						goto bail;
					}else if (ret==RTCAN){
						goto bail;
					}else if (ret==RTNORM){
						ic_trim(szKeyWord,"be");
						strupr(szKeyWord);
						if (*szKeyWord=='?'){
							sds_textscr();
							SDS_StartTblRead("STYLE"/*DNT*/);
							while(SDS_TblReadHasMore()) {
								sds_printf(ResourceString(IDC_CMDS3__N_S_620, "\n%s" ),SDS_TblReadString());
							}
						}else{
							SDS_StartTblRead("STYLE"/*DNT*/);
							while(SDS_TblReadHasMore()) {
								strcpy(szPrompt,SDS_TblReadString());
								//if (strchr(szPrompt,'|'/*DNT*/)!=NULL) continue; //watch for xref styles EBATECH(CNBR) miss
								if (strchr(szPrompt,'|'/*DNT*/)!=NULL) continue; //watch for xref styles
								if (sds_wcmatch(szPrompt,szKeyWord)==RTNORM) {
									strcpy(SDS_mTextDataPtr.szTextStyle,szPrompt);
									gotit=true;
									break;
								}
							}
							if (gotit==false){
								cmd_ErrorPrompt(CMD_ERR_FINDSTYLE,0);
							}
						}
					}
					// EBATECH(CNBR) -[ default value: break loop if you hit [Enter] or [SPACE].
					else if (ret==RTNONE){
						gotit=true;
					}
					// EBATECH(CNBR) ]-
				}while(gotit==false);
			}else if (strisame(szKeyWord,"DIRECTION"/*DNT*/)) {
				// EBATECH(CNBR) -[ ADD 5=Depend as STYLE
				sds_initget(0,ResourceString(IDC_CMDS3_LEFT_TO_RIGHT_LR_622,
					"Left-to-Right|L Right-to-Left|R Top-to-Bottom|T Bottom-to-Top|B Style|S ~_L ~_R ~_T ~_B ~_S"));
				if ((ret=sds_getkword(ResourceString(IDC_CMDS3__NSPECIFY_DRAWIN_623,
					"\nSpecify drawing direction:  Left-to-right/Right-to-left/Top-to-bottom/Bottom-to-Top/Style<Left-to-right (L)> " ),szKeyWord))==RTERROR) {
					goto bail;
				}else if (ret==RTCAN){
					goto bail;
				}else if (ret==RTNONE){ // Same as L option (Left-to-right).
					SDS_mTextDataPtr.nDirection=1;
				}else if (ret==RTNORM){
					if (strisame(szKeyWord,"L"/*DNT*/)) {
						SDS_mTextDataPtr.nDirection=1;
					} else if (strisame(szKeyWord,"R"/*DNT*/)) {
						SDS_mTextDataPtr.nDirection=2;
					} else if (strisame(szKeyWord,"T"/*DNT*/)) {
						SDS_mTextDataPtr.nDirection=3;
					} else if (strisame(szKeyWord,"B"/*DNT*/)) {
						SDS_mTextDataPtr.nDirection=4;
					} else if (strisame(szKeyWord,"S"/*DNT*/)) {
						SDS_mTextDataPtr.nDirection=5;
					}
				}
				// ]- EBATECH(CNBR)
			}else if (strisame(szKeyWord,"WIDTH"/*DNT*/)) {
				do{
					if ((ret=sds_getdist(SDS_mTextDataPtr.ptInsertion,ResourceString(IDC_CMDS3_WIDTH_OF_TEXT_BO_628, "Width of text box: " ),&SDS_mTextDataPtr.rBoxWidth))==RTERROR) {
						goto bail;
					}else if (ret==RTCAN){
						goto bail;
					}else if (ret==RTNONE){
						break;
					}else if (ret==RTNORM){
						goto done;
					}
				}while(1);
			}
		}else if (ret==RTNORM){
			SDS_mTextDataPtr.rBoxWidth=(SDS_mTextDataPtr.ptOppCorner[0]>SDS_mTextDataPtr.ptInsertion[0])
									  ?(SDS_mTextDataPtr.ptOppCorner[0]-SDS_mTextDataPtr.ptInsertion[0])
									  :(SDS_mTextDataPtr.ptInsertion[0]-SDS_mTextDataPtr.ptOppCorner[0]);
			break;
		}
	}
done:
	// So, CUI Text input
	for(;;)
		{
		ret=sds_getstring(1, ResourceString(IDC_DIMENSIONS__NMTEXT___139, "\nMText: " ),szTmp);
		if( ret != RTNORM ) // All Error Detected.
			{
			goto bail;
			}
		if( strlen(szTmp) > 0 )
			{
			if( SDS_mTextDataPtr.text.GetLength() > 0 )
				{
				SDS_mTextDataPtr.text += CString("\\P");
				}
			SDS_mTextDataPtr.text += CString(szTmp);
			}
		else	// Hit Enter Only
			{
			CreateMTEXTObject();
			break;
			}
		}
	ret=RTNORM;
bail:

	return(ret);
}

// 2002/4/24 EBATECH(CNBR) cmd_shape() move to Source/lib/CMDS/shape.cpp

short cmd_dash_group(void)	{
	return(RTNORM);
}

short cmd_mlstyle(void)   {
	cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0);
	SDS_FreeEventList(0);
	return(RTNORM);
}
/*
	char			szPrompt[IC_ACADBUF],	// For making up complicated prompt strings with %s
					szKeyWord[IC_ACADBUF];	// Keyword from command line
	int				nRet=RTNORM;			// This function's return value.
	long			lErrorLine=0L;			// Gives the line number if there is an error.


	for (;;) {
		if (){
			if (sds_initget(0,"")!=RTNORM) { lErrorLine=(-__LINE__); goto bail; }
			if ((nRet=sds_get(ResourceString(IDC_CMDS3__N________599, "\n/<>: " )))==RTCAN || nRet==RTERROR) goto bail;
		}else{
			if (sds_initget(0,"")!=RTNORM) { lErrorLine=(-__LINE__); goto bail; }
			if ((nRet=sds_get(ResourceString(IDC_CMDS3__N________599, "\n/<>: " )))==RTCAN || nRet==RTERROR) goto bail;
		}
		if (nRet==RTNORM) break;
		if (nRet==RTERROR)
			goto bail;
		if (nRet==RTKWORD) {
			nRet=sds_getinput(szKeyWord);
			if ((stricmp(szKeyWord,ResourceString(IDC_CMDS3_X_600, "x" ))==0) || (stricmp(szKeyWord,ResourceString(IDC_CMDS3_JUSTIFICATION_442, "Justification" ))==0)) {
			} else if ((stricmp(szKeyWord,ResourceString(IDC_CMDS3_X_600, "x" ))==0)) {
			} else if ((stricmp(szKeyWord,ResourceString(IDC_CMDS3_X_600, "x" ))==0)) {
			} else if ((stricmp(szKeyWord,ResourceString(IDC_CMDS3_X_600, "x" ))==0)) {
			} else if ((stricmp(szKeyWord,ResourceString(IDC_CMDS3_X_600, "x" ))==0)) {
			} else if ((stricmp(szKeyWord,ResourceString(IDC_CMDS3_X_600, "x" ))==0)) {
			}

end:
	// put end stuff here if needed.

bail:

	if (lErrorLine==0L) return(RTNORM);
	if (lErrorLine==(-1L)) return(RTCAN);
	if (lErrorLine<(-1L)) CMD_INTERNAL_MSG(-lErrorLine);
	return(nRet);
}
*/
