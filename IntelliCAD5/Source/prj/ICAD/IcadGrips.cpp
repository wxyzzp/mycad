/* F:\BLD\PRJ\ICAD\ICADGRIPS.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 *
 */

// ** Includes
#include "Icad.h"/*DNT*/
#include "IcadDoc.h"
#include "cmds.h"/*DNT*/
#include "IcadView.h"
#include "DrawDevice.h"
#include "Modeler.h"
#include "xstd.h" //xstd::vector
#include "gvector.h"
using icl::point;
using icl::gvector;
#include "transf.h"
using icl::transf;
#include "CmdQueryTools.h"//wcs2ucs
#include "DbAcisEntity.h"
#include "dimensions.h"
#include "undo\UndoSaver.h"/*DNT*/

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


int            SDS_AtNodeDrag;
sds_name	   SDS_NodeDragEnt;
struct gr_displayobject *SDS_NodeDragEntDispObj = NULL;

extern double  SDS_CorrectElevation;
extern bool    SDS_DontDrawVPGuts;
extern bool    SDS_ElevationChanged;


//4M Spiros Item:27->
extern bool	   SDS_AtDragMulti;
extern bool	   SDS_DragNOElevation;
//4M Spiros Item:27<-

extern struct cmd_dimeparlink* plink;

// ** Funtions

int SDS_AddGripNodes(CIcadDoc  *pDoc, sds_name ename, int addmode)
	{
    //addmode=2 to draw the grip node, but not add to llist of selected items. (sssetfirst ss1)
	//addmode=1 to add new nodes to llist & draw them
	//addmode=0 to paint out nodes
	//addmode=-1 to delete nodes from llist (no paint out)
	//NOTE: if the entity has thickness, then grips are added at the extruded
	//	edge ONLY if the extrusion direction is not parallel to viewdir.  This
	//	minimizes the number of nodes & omits the need for filtering the duplicates
	//	out in the dragging.
	db_handitem *elp=(db_handitem *)ename[0];
	db_drawing *flp=(db_drawing *)ename[1];
	sds_point pt,pt1,pt2,extru;
	sds_real thick;
	unsigned char plcharflag;
	sds_resbuf rbucs,rbwcs,rbecs;
	struct resbuf rb;

	if (addmode<0)
		{
//4M Item:25->
// Replacement of SDS_NodeList with SDS_NodeList_New
//<-4M Item:25
        pDoc->m_nodeList.remove( ename );
		return(RTNORM);
		}

	/*D.G.*//* Allow grips in perspective mode now.

	//no grips if we're in perspective mode
	SDS_getvar(NULL,DB_QVIEWMODE,&rbucs,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if (rbucs.resval.rint&1)
		return(RTNORM);*/

	//setup resbufs for transformation of ent pts.
	rbucs.restype=rbwcs.restype=RTSHORT;
	rbecs.restype=RT3DPOINT;
	rbucs.rbnext=rbwcs.rbnext=rbecs.rbnext=NULL;
	rbucs.resval.rint=1;
	rbwcs.resval.rint=0;
	rbecs.resval.rpoint[0]=rbecs.resval.rpoint[1]=0.0;
	rbecs.resval.rpoint[2]=1.0;

	if (elp==NULL || pDoc==NULL)
		return(RTERROR);

	if (addmode==1)
		{
		SDS_getvar(NULL,DB_QHIGHLIGHT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		if (rb.resval.rint)
			sds_redraw(ename,IC_REDRAW_HILITE);
		}
	switch(elp->ret_type())
		{
		case DB_SPLINE:
			{
			int fi1,fi2;
			sds_point *ptarray,*pp1;
			bool fitpts;

			elp->get_73(&fi1);
			if (0==fi1)
				{
				elp->get_74(&fi1);
				fitpts=TRUE;
				}
			else
				fitpts=FALSE;
			if (0==fi1)
				break;			
			if (NULL==(ptarray= new sds_point [ fi1 ]))
				break;
			memset(ptarray, 0, sizeof(sds_point)*fi1 );
			if (fitpts)
				elp->get_11(ptarray,fi1);
			else
				elp->get_10(ptarray,fi1);
			pp1=ptarray;
			for (fi2=0;fi2<fi1;fi2++)
				{
				sds_trans(*pp1,&rbwcs,&rbucs,0,*pp1);
				SDS_AddOneNode(pDoc,ename,*pp1,addmode);
				pp1++;
				}
			IC_FREE(ptarray);
			break;
			}
		case DB_LEADER:
			{
			int fi1,fi2;
			sds_point *ptarray,*pp1;

			elp->get_76(&fi1);
			if (fi1==0)
				break;			
			if (NULL==(ptarray= new sds_point [fi1]))
				break;
			memset(ptarray, 0, sizeof(sds_point)*fi1);
			elp->get_10(ptarray,fi1);
			pp1=ptarray;
			for (fi2=0;fi2<fi1;fi2++)
				{
				sds_trans(*pp1,&rbwcs,&rbucs,0,*pp1);
				SDS_AddOneNode(pDoc,ename,*pp1,addmode);
				pp1++;
				}
			IC_FREE(ptarray);
			break;
			}
		case DB_MTEXT:
			{
			int fi1;
			sds_real fr1;

			elp->get_71(&fi1);
			elp->get_10(pt1);
			elp->get_11(pt2);
			elp->get_41(&fr1);
			sds_trans(pt1,&rbwcs,&rbucs,0,pt);
			SDS_AddOneNode(pDoc,ename,pt,addmode);
			switch(fi1)
				{
				case 1:
				case 4:
				case 7:
					{
					pt[0]=pt1[0]+fr1*pt2[0];
					pt[1]=pt1[1]+fr1*pt2[1];
					pt[2]=pt1[2]+fr1*pt2[2];
					sds_trans(pt,&rbwcs,&rbucs,0,pt);
					SDS_AddOneNode(pDoc,ename,pt,addmode);
					}
					break;
				case 2:
				case 5:
				case 8:
					{
					pt[0]=pt1[0]+fr1*0.5*pt2[0];
					pt[1]=pt1[1]+fr1*0.5*pt2[1];
					pt[2]=pt1[2]+fr1*0.5*pt2[2];
					sds_trans(pt,&rbwcs,&rbucs,0,pt);
					SDS_AddOneNode(pDoc,ename,pt,addmode);
					}
					break;
				case 3:
				case 6:
				case 9:
					{
					pt[0]=pt1[0]-fr1*pt2[0];
					pt[1]=pt1[1]-fr1*pt2[1];
					pt[2]=pt1[2]-fr1*pt2[2];
					sds_trans(pt,&rbwcs,&rbucs,0,pt);
					SDS_AddOneNode(pDoc,ename,pt,addmode);
					}
					break;
				}//switch for justification
			}//case for mtext
			break;
		case DB_VIEWPORT:
			{
			elp->get_10(pt1);
			sds_trans(pt1,&rbwcs,&rbucs,0,pt);
			SDS_AddOneNode(pDoc,ename,pt,addmode);//center point
			elp->get_40(&pt2[0]);
			elp->get_41(&pt2[1]);
			pt2[0]*=0.5;
			pt2[1]*=0.5;
			pt1[1]+=pt2[1];
			sds_trans(pt1,&rbwcs,&rbucs,0,pt);
			SDS_AddOneNode(pDoc,ename,pt,addmode);//UM point
			pt1[0]+=pt2[0];
			sds_trans(pt1,&rbwcs,&rbucs,0,pt);
			SDS_AddOneNode(pDoc,ename,pt,addmode);//UR point
			pt1[1]-=pt2[1];
			sds_trans(pt1,&rbwcs,&rbucs,0,pt);
			SDS_AddOneNode(pDoc,ename,pt,addmode);//MR point
			pt1[1]-=pt2[1];
			sds_trans(pt1,&rbwcs,&rbucs,0,pt);
			SDS_AddOneNode(pDoc,ename,pt,addmode);//LR point
			pt1[0]-=pt2[0];
			sds_trans(pt1,&rbwcs,&rbucs,0,pt);
			SDS_AddOneNode(pDoc,ename,pt,addmode);//BM point
			pt1[0]-=pt2[0];
			sds_trans(pt1,&rbwcs,&rbucs,0,pt);
			SDS_AddOneNode(pDoc,ename,pt,addmode);//LL point
			pt1[1]+=pt2[1];
			sds_trans(pt1,&rbwcs,&rbucs,0,pt);
			SDS_AddOneNode(pDoc,ename,pt,addmode);//ML point
			pt1[1]+=pt2[1];
			sds_trans(pt1,&rbwcs,&rbucs,0,pt);
			SDS_AddOneNode(pDoc,ename,pt,addmode);//UL point
			}
			break;
		case DB_XLINE:
		case DB_RAY:
			{
			SDS_getvar(NULL,DB_QVIEWSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			rb.resval.rreal*=0.1;
			elp->get_10(pt1);
			sds_trans(pt1,&rbwcs,&rbucs,0,pt1);
			SDS_AddOneNode(pDoc,ename,pt1,addmode);
			elp->get_11(pt2);
			sds_trans(pt2,&rbwcs,&rbucs,1,pt2);
			pt1[0]+=pt2[0]*rb.resval.rreal;
			pt1[1]+=pt2[1]*rb.resval.rreal;
			pt1[2]+=pt2[2]*rb.resval.rreal;
			SDS_AddOneNode(pDoc,ename,pt1,addmode);
			if (elp->ret_type()==DB_XLINE)
				{
				pt1[0]-=2.0*pt2[0]*rb.resval.rreal;
				pt1[1]-=2.0*pt2[1]*rb.resval.rreal;
				pt1[2]-=2.0*pt2[2]*rb.resval.rreal;
				SDS_AddOneNode(pDoc,ename,pt1,addmode);
				}
			}
			break;
		case DB_DIMENSION:
			{
			int type;
			int real_addmode = addmode;	/*D.Gavrilov*/// We'll change it in one case following.
			struct resbuf rbx,rby;
			sds_point zdir;
			SDS_getvar(NULL,DB_QUCSXDIR,&rbx,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			SDS_getvar(NULL,DB_QUCSYDIR,&rby,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			ic_crossproduct(rbx.resval.rpoint,rby.resval.rpoint,zdir);

			elp->get_210(rbecs.resval.rpoint);
			if(!icadPointEqual(rbecs.resval.rpoint, zdir))	/*D.Gavrilov*/// In this case ACAD
				real_addmode = 2;					// shows the grips but tells something like
									// "1 was not parallel to the UCS" if you try to change them.
			elp->get_70(&type);
			type &= ~DIM_BIT_6; // Use macros EBATECH(CNBR) 13-03-2003
			//if (type & DIM_BIT_6)	type-= DIM_BIT_6;
			// If the dimension text has been moved at a user-defined location rather than at the default location.
			type &= ~DIM_TEXT_MOVED;
			//if (type & DIM_TEXT_MOVED) type -= DIM_TEXT_MOVED;

			if (type!=DIM_Y_ORDINATE && type!=DIM_x_ORDINATE && type!=DIM_DIAMETER && type!=DIM_RADIUS)
				{
				elp->get_10(pt1);
				sds_trans(pt1,&rbwcs,&rbucs,0,pt);
				SDS_AddOneNode(pDoc,ename,pt,real_addmode);
				}

			elp->get_11(pt1);
			sds_trans(pt1,&rbecs,&rbucs,0,pt);
			SDS_AddOneNode(pDoc,ename,pt,real_addmode);

//			if (type==DIM_DIAMETER) {
//				elp->get_12(pt1);
//				sds_trans(pt1,&rbwcs,&rbucs,0,pt);
//				SDS_AddOneNode(pDoc,ename,pt,real_addmode);
//			}

			if (type!=DIM_RADIUS && type!=DIM_DIAMETER)
				{
				elp->get_13(pt1);
				sds_trans(pt1,&rbwcs,&rbucs,0,pt);
				SDS_AddOneNode(pDoc,ename,pt,real_addmode);

				elp->get_14(pt1);
				sds_trans(pt1,&rbwcs,&rbucs,0,pt);
				SDS_AddOneNode(pDoc,ename,pt,real_addmode);
				}

			if (type==DIM_ANGULAR/* || type==DIM_RADIUS || type==DIM_DIAMETER*/)
				{
				elp->get_15(pt1);
				sds_trans(pt1,&rbwcs,&rbucs,0,pt);
				SDS_AddOneNode(pDoc,ename,pt,real_addmode);
				}
			if (type==DIM_ANGULAR)
				{
				elp->get_16(pt1);
				sds_trans(pt1,&rbwcs,&rbucs,0,pt);
				SDS_AddOneNode(pDoc,ename,pt,real_addmode);
				}
			}
			break;
		case DB_LINE:
			{
			elp->get_10(pt);
			sds_trans(pt,&rbwcs,&rbucs,0,pt1);
			SDS_AddOneNode(pDoc,ename,pt1,addmode);
			elp->get_11(pt);
			sds_trans(pt,&rbwcs,&rbucs,0,pt2);
			SDS_AddOneNode(pDoc,ename,pt2,addmode);
			pt[0]=0.5*(pt1[0]+pt2[0]);
			pt[1]=0.5*(pt1[1]+pt2[1]);
			pt[2]=0.5*(pt1[2]+pt2[2]);
			SDS_AddOneNode(pDoc,ename,pt,addmode);
			elp->get_39(&thick);
			if (fabs(thick)>CMD_FUZZ)
				{
				elp->get_210(extru);
				//transform extrusion into DCS and see if nodes will be overlaid...
				rbucs.resval.rint=2;
				sds_trans(extru,&rbwcs,&rbucs,1,extru);
				rbucs.resval.rint=1;
				if (fabs(extru[0])>CMD_FUZZ || fabs(extru[1])>CMD_FUZZ)
					{
					elp->get_210(extru);
					extru[0]*=thick;
					extru[1]*=thick;
					extru[2]*=thick;
					sds_trans(extru,&rbwcs,&rbucs,1,extru);
					pt1[0]+=extru[0];
					pt1[1]+=extru[1];
					pt1[2]+=extru[2];
					SDS_AddOneNode(pDoc,ename,pt1,addmode);
					pt2[0]+=extru[0];
					pt2[1]+=extru[1];
					pt2[2]+=extru[2];
					SDS_AddOneNode(pDoc,ename,pt2,addmode);
					//pt[0]+=extru[0];
					//pt[1]+=extru[1];
					//pt[2]+=extru[2];
					//SDS_AddOneNode(pDoc,ename,pt,addmode);
					}
				}
			}
			break;
		case DB_ARC:
            {
			sds_point cp;
			double rad,sa,ea;
			bool isthick=FALSE;
			elp->get_210(rbecs.resval.rpoint);
			elp->get_10(cp);
			elp->get_40(&rad);
			elp->get_50(&sa);
			elp->get_51(&ea);
			elp->get_39(&thick);
			if (fabs(thick)>CMD_FUZZ)
				{
				ic_ptcpy(extru,rbecs.resval.rpoint);
				rbucs.resval.rint=2;
				sds_trans(extru,&rbwcs,&rbucs,1,extru);//convert to dcs
				rbucs.resval.rint=1;
				if (fabs(extru[0])>CMD_FUZZ || fabs(extru[1])>CMD_FUZZ)
					{
					ic_ptcpy(extru,rbecs.resval.rpoint);
					isthick=TRUE;
					extru[0]*=thick;
					extru[1]*=thick;
					extru[2]*=thick;
					sds_trans(extru,&rbwcs,&rbucs,1,extru);
					}
				}
			sds_trans(cp,&rbecs,&rbucs,0,pt1);
			SDS_AddOneNode(pDoc,ename,pt1,addmode);
			/*if (isthick){
				pt1[0]+=extru[0];
				pt1[1]+=extru[1];
				pt1[2]+=extru[2];
				SDS_AddOneNode(pDoc,ename,pt1,addmode);
			}*/
			sds_polar(cp,sa,rad,pt);
			sds_trans(pt,&rbecs,&rbucs,0,pt1);
			SDS_AddOneNode(pDoc,ename,pt1,addmode);
			if (isthick)
				{
				pt1[0]+=extru[0];
				pt1[1]+=extru[1];
				pt1[2]+=extru[2];
				SDS_AddOneNode(pDoc,ename,pt1,addmode);
				}
			sds_polar(cp,ea,rad,pt);
			sds_trans(pt,&rbecs,&rbucs,0,pt1);
			SDS_AddOneNode(pDoc,ename,pt1,addmode);
			if (isthick)
				{
				pt1[0]+=extru[0];
				pt1[1]+=extru[1];
				pt1[2]+=extru[2];
				SDS_AddOneNode(pDoc,ename,pt1,addmode);
				}
			ic_normang(&sa,&ea);
			sds_polar(cp,(sa+ea)/2.0,rad,pt);
			sds_trans(pt,&rbecs,&rbucs,0,pt1);
			SDS_AddOneNode(pDoc,ename,pt1,addmode);
			if (isthick)
				{
				pt1[0]+=extru[0];
				pt1[1]+=extru[1];
				pt1[2]+=extru[2];
				SDS_AddOneNode(pDoc,ename,pt1,addmode);
				}
			}
			break;
		case DB_CIRCLE:
            {
			sds_point cp;
			double rad,angbase;
			int fi1;
			bool isthick=FALSE;
			elp->get_210(rbecs.resval.rpoint);
			elp->get_10(cp);
			elp->get_40(&rad);
			elp->get_39(&thick);
			if (fabs(thick)>CMD_FUZZ)
				{
				ic_ptcpy(extru,rbecs.resval.rpoint);
				rbucs.resval.rint=2;
				sds_trans(extru,&rbwcs,&rbucs,1,extru);//convert to dcs
				rbucs.resval.rint=1;
				if (fabs(extru[0])>CMD_FUZZ || fabs(extru[1])>CMD_FUZZ)
					{
					ic_ptcpy(extru,rbecs.resval.rpoint);
					isthick=TRUE;
					extru[0]*=thick;
					extru[1]*=thick;
					extru[2]*=thick;
					sds_trans(extru,&rbwcs,&rbucs,1,extru);
					}
				}
			pt1[1]=pt1[2]=0.0;
			pt1[0]=1.0;
			sds_trans(pt1,&rbecs,&rbucs,1,pt1);
			angbase=atan2(pt1[1],pt1[0]);
			for (fi1=0;fi1<4;fi1++)
				{
				pt2[2]=(fi1*IC_PI*0.5)-angbase;
				pt2[0]=sin(pt2[2]);
				pt2[1]=cos(pt2[2]);
				ic_ptcpy(pt1,cp);
				pt1[0]+=rad*pt2[1];
				pt1[1]+=rad*pt2[0];
				sds_trans(pt1,&rbecs,&rbucs,0,pt1);
				SDS_AddOneNode(pDoc,ename,pt1,addmode);
				if (isthick)
					{
					pt1[0]+=extru[0];
					pt1[1]+=extru[1];
					pt1[2]+=extru[2];
					SDS_AddOneNode(pDoc,ename,pt1,addmode);
					}
				}//for
			sds_trans(cp,&rbecs,&rbucs,0,pt1);
			SDS_AddOneNode(pDoc,ename,pt1,addmode);
			/*if (isthick){
				pt1[0]+=extru[0];
				pt1[1]+=extru[1];
				pt1[2]+=extru[2];
				SDS_AddOneNode(pDoc,ename,pt1,addmode);
			}*/
			}
			break;
		case DB_ELLIPSE:
			{
			// there are 4 kinds of grips - center to move entire entity
			//	major axis to rotate and shape eccentricity
			//	minor axis to scale entire entity
			//	endpoint to change start or end angle (open arc only)
			sds_point cp,smavect,norm,minvect;
			double min_maj,sa,ea;
			rbecs.restype=RTSHORT;
			rbecs.resval.rint=0;
			elp->get_210(norm);
			if ((cp[0]=norm[0]*norm[0]+norm[1]*norm[1]+norm[2]*norm[2])!=1.0)
				{
				//unitize the normal vector just in case
				cp[0]=sqrt(cp[0]);
				norm[0]/=cp[0];
				norm[1]/=cp[0];
				norm[2]/=cp[0];
				}
			elp->get_10(cp);
			elp->get_11(smavect);
			elp->get_40(&min_maj);
			elp->get_41(&sa);
			elp->get_42(&ea);
			sds_trans(cp,&rbecs,&rbucs,0,pt);
			SDS_AddOneNode(pDoc,ename,pt,addmode);
			if (!(sa>0.0 && sa<IC_PI && ea>IC_PI && ea<IC_TWOPI))
				{
				ic_ptcpy(pt,cp);
				pt[0]+=smavect[0];
				pt[1]+=smavect[1];
				pt[2]+=smavect[2];
				sds_trans(pt,&rbecs,&rbucs,0,pt);
				SDS_AddOneNode(pDoc,ename,pt,addmode);
				}
			if (!(sa>IC_PI && sa<IC_TWOPI && ea>IC_TWOPI && ea<3.0*IC_PI))
				{
				ic_ptcpy(pt,cp);
				pt[0]-=smavect[0];
				pt[1]-=smavect[1];
				pt[2]-=smavect[2];
				sds_trans(pt,&rbecs,&rbucs,0,pt);
				SDS_AddOneNode(pDoc,ename,pt,addmode);
				}
			ic_crossproduct(smavect,norm,minvect);
			minvect[0]*=min_maj;
			minvect[1]*=min_maj;
			minvect[2]*=min_maj;
			if (!(sa>0.5*IC_PI && sa<1.5*IC_PI && ea>1.5*IC_PI && ea<2.5*IC_PI))
				{
				ic_ptcpy(pt,cp);
				pt[0]-=minvect[0];
				pt[1]-=minvect[1];
				pt[2]-=minvect[2];
				sds_trans(pt,&rbecs,&rbucs,0,pt);
				SDS_AddOneNode(pDoc,ename,pt,addmode);
				}
			if (!((sa>1.5*IC_PI && ea>2.5*IC_PI && ea<3.5*IC_PI) ||
				 (sa<0.5*IC_PI && ea>0.5*IC_PI && ea<1.5*IC_PI)))
				{
				ic_ptcpy(pt,cp);
				pt[0]+=minvect[0];
				pt[1]+=minvect[1];
				pt[2]+=minvect[2];
				sds_trans(pt,&rbecs,&rbucs,0,pt);
				SDS_AddOneNode(pDoc,ename,pt,addmode);
				}
			if (!icadRealEqual(sa,0.0) || !icadRealEqual(ea,IC_TWOPI))
				{
				double angsin,angcos;
				angsin=sin(sa);angcos=cos(sa);
				pt[0]=cp[0]-minvect[0]*angsin+smavect[0]*angcos;
				pt[1]=cp[1]-minvect[1]*angsin+smavect[1]*angcos;
				pt[2]=cp[2]-minvect[2]*angsin+smavect[2]*angcos;
				sds_trans(pt,&rbecs,&rbucs,0,pt);
				SDS_AddOneNode(pDoc,ename,pt,addmode);
				angsin=sin(ea);angcos=cos(ea);
				pt[0]=cp[0]-minvect[0]*angsin+smavect[0]*angcos;
				pt[1]=cp[1]-minvect[1]*angsin+smavect[1]*angcos;
				pt[2]=cp[2]-minvect[2]*angsin+smavect[2]*angcos;
				sds_trans(pt,&rbecs,&rbucs,0,pt);
				SDS_AddOneNode(pDoc,ename,pt,addmode);
				}
			}
			break;
		case DB_POINT:
            {
			elp->get_10(pt1);
			sds_trans(pt1,&rbwcs,&rbucs,0,pt);
			SDS_AddOneNode(pDoc,ename,pt,addmode);
			elp->get_39(&thick);
			if (fabs(thick)>CMD_FUZZ)
				{
				elp->get_210(extru);
				rbucs.resval.rint=2;
				sds_trans(extru,&rbwcs,&rbucs,1,extru);//convert to dcs
				rbucs.resval.rint=1;
				if (fabs(extru[0])>CMD_FUZZ || fabs(extru[1])>CMD_FUZZ)
					{
					elp->get_210(extru);
					extru[0]*=thick;
					extru[1]*=thick;
					extru[2]*=thick;
					sds_trans(extru,&rbwcs,&rbucs,1,extru);
					pt[0]+=extru[0];
					pt[1]+=extru[1];
					pt[2]+=extru[2];
					SDS_AddOneNode(pDoc,ename,pt,addmode);
					}
				}
			}
			break;
		case DB_INSERT:
            {
			int fi1;
			sds_name attent;
			char *fcp1;

			elp->get_2(&fcp1);
			if (*fcp1=='*')
				{
				if (*(fcp1+1)=='X')
					{
					//hatch block.  Calc point for grip...
					if (RTNORM==cmd_hatch_blkmidpt(ename,pt1))
						SDS_AddOneNode(pDoc,ename,pt1,addmode);
					}
				else
//4M Spiros Item:66->
//Adding grips to user defined anonymous blocks...
					{
/*
                    break;  //no nodes for user-defined block
*/
                    struct sds_resbuf* elistsp=sds_entget(ename);
					if(ic_assoc(elistsp,10)==0)
						{
						sds_real ptsp[3];
						ic_ptcpy(ptsp,ic_rbassoc->resval.rpoint);
						SDS_AddOneNode(pDoc,ename,ptsp,addmode);
						}
					sds_relrb(elistsp);
					}
//4M Spiros Item:66<-
                }
			else
				{
				elp->get_210(rbecs.resval.rpoint);
				elp->get_10(pt1);
				sds_trans(pt1,&rbecs,&rbucs,0,pt);
				SDS_AddOneNode(pDoc,ename,pt,addmode);

				SDS_getvar(NULL,DB_QHIGHLIGHT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
//4M Item:67->
// Add grips to attribs that belong to inserts to anonymous blocks
/*
//4M Item:67<-
                elp->get_66(&fi1);
				if (fi1)
					{
					ic_encpy(attent,ename);//don't adjust ename - we need to hilight it
					for (elp=flp->entnext_noxref(elp); elp!=NULL && elp->ret_type()==DB_ATTRIB; elp=flp->entnext_noxref(elp))
						{
						elp->get_10(pt1);
						elp->get_210(rbecs.resval.rpoint);
						sds_trans(pt1,&rbecs,&rbucs,0,pt);
						attent[0]=(long)elp;
						SDS_AddOneNode(pDoc,attent,pt,addmode);
						//highlight attrib.  Main ent highlighted at end of switch stmt
						if (addmode && rb.resval.rint)
							sds_redraw(attent,IC_REDRAW_HILITE);
						}
					}
//4M Item:67->
// Add grips to attribs that belong to inserts to anonymous blocks
*/
//4M Item:67<-
                }
//4M Item:67->
// Add grips to attribs that belong to inserts to anonymous blocks
            elp->get_66(&fi1);
			if (fi1)
				{
				ic_encpy(attent,ename);//don't adjust ename - we need to hilight it
				for (elp=flp->entnext_noxref(elp); elp!=NULL && elp->ret_type()==DB_ATTRIB; elp=flp->entnext_noxref(elp))
					{
					elp->get_10(pt1);
					elp->get_210(rbecs.resval.rpoint);
					sds_trans(pt1,&rbecs,&rbucs,0,pt);
					attent[0]=(long)elp;
					SDS_AddOneNode(pDoc,attent,pt,addmode);
					//highlight attrib.  Main ent highlighted at end of switch stmt
					if (addmode && rb.resval.rint)
						sds_redraw(attent,IC_REDRAW_HILITE);
					}
				}
//<-4M Item:67
            }
			break;
/*		//Initial work on adding grips for hatch.  Removed due to lack of time.  Issues: grip is incorrectly
		//placed when view is changed.  Need to add to function so it will move when dragged.
		case DB_HATCH:
			{
				elp->get_210(rbecs.resval.rpoint);

				sds_point ll;
				sds_point ur;
				sds_point midpt;

				elp->get_10(midpt); //set elevation (z) - then calc x,y

				elp->get_extent(ll, ur);

				midpt[0] = 0.5 * (ll[0] + ur[0]);
				midpt[1] = 0.5 * (ll[1] + ur[1]);

				sds_trans(midpt,&rbecs,&rbucs,0,pt);
				SDS_AddOneNode(pDoc,ename,pt,addmode);
			}
			break;
*/
		case DB_SHAPE:
            {
			elp->get_210(rbecs.resval.rpoint);
			elp->get_10(pt1);
			sds_trans(pt1,&rbecs,&rbucs,0,pt);
			SDS_AddOneNode(pDoc,ename,pt,addmode);
			/* for now, let's not add a node at the extruded edge
			elp->get_39(&thick);
			if (fabs(thick)>CMD_FUZZ){
				ic_ptcpy(extru,rbecs.resval.rpoint);
				rbucs.resval.rint=2;
				sds_trans(extru,&rbwcs,&rbucs,1,extru);//convert to dcs
				rbucs.resval.rint=1;
				if (fabs(extru[0])>CMD_FUZZ || fabs(extru[1])>CMD_FUZZ){
					ic_ptcpy(extru,rbecs.resval.rpoint);
					extru[0]*=thick;
					extru[1]*=thick;
					extru[2]*=thick;
					sds_trans(extru,&rbwcs,&rbucs,1,extru);
					pt[0]+=extru[0];
					pt[1]+=extru[1];
					pt[2]+=extru[2];
					SDS_AddOneNode(pDoc,ename,pt,addmode);
				}
			}*/
			}
			break;
		case DB_TOLERANCE:
            {
			elp->get_210(rbecs.resval.rpoint);
			elp->get_10(pt1);
			sds_trans(pt1,&rbecs,&rbucs,0,pt);
			SDS_AddOneNode(pDoc,ename,pt,addmode);
			}
			break;
		case DB_SOLID:
		case DB_TRACE:
            {
			bool isthick=FALSE;
			elp->get_210(rbecs.resval.rpoint);
			elp->get_39(&thick);
			if (fabs(thick)>CMD_FUZZ)
				{
				ic_ptcpy(extru,rbecs.resval.rpoint);
				rbucs.resval.rint=2;
				sds_trans(extru,&rbwcs,&rbucs,1,extru);//convert to dcs
				rbucs.resval.rint=1;
				if (fabs(extru[0])>CMD_FUZZ || fabs(extru[1])>CMD_FUZZ)
					{
					isthick=TRUE;
					ic_ptcpy(extru,rbecs.resval.rpoint);
					extru[0]*=thick;
					extru[1]*=thick;
					extru[2]*=thick;
					sds_trans(extru,&rbwcs,&rbucs,1,extru);
					}
				}
			for (int fi1=0; fi1<4; fi1++)
				{
				if (elp->ret_type()==DB_SOLID && fi1==3 && elp->ret_is3pt())
					break;
				elp->get_ptbyidx(pt1,fi1);
				sds_trans(pt1,&rbecs,&rbucs,0,pt);
				SDS_AddOneNode(pDoc,ename,pt,addmode);
				if (isthick)
					{
					pt[0]+=extru[0];
					pt[1]+=extru[1];
					pt[2]+=extru[2];
					SDS_AddOneNode(pDoc,ename,pt,addmode);
					}
				}
			}
			break;
		case DB_3DFACE:
            {
			for (int fi1=0; fi1<4; fi1++)
				{
				if (fi1==3 && elp->ret_is3pt())
					break;
				elp->get_ptbyidx(pt1,fi1);
				sds_trans(pt1,&rbwcs,&rbucs,0,pt);
				SDS_AddOneNode(pDoc,ename,pt,addmode);
				}
			}
			break;
		case DB_TEXT:
		case DB_ATTDEF:
		case DB_ATTRIB:
            {
			elp->get_210(rbecs.resval.rpoint);
			int halign,valign;
			bool isthick=FALSE;
			elp->get_72(&halign);
			elp->get_73(&valign);
			elp->get_10(pt1);
			elp->get_39(&thick);
			if (fabs(thick)>CMD_FUZZ)
				{
				ic_ptcpy(extru,rbecs.resval.rpoint);
				rbucs.resval.rint=2;
				sds_trans(extru,&rbwcs,&rbucs,1,extru);//convert to dcs
				rbucs.resval.rint=1;
				if (fabs(extru[0])>CMD_FUZZ || fabs(extru[1])>CMD_FUZZ)
					{
					isthick=TRUE;
					ic_ptcpy(extru,rbecs.resval.rpoint);
					extru[0]*=thick;
					extru[1]*=thick;
					extru[2]*=thick;
					sds_trans(extru,&rbwcs,&rbucs,1,extru);
					}
				}
			//always put node on the 10pt
			//	put node on 11pt if text is justified
			sds_trans(pt1,&rbecs,&rbucs,0,pt);
			SDS_AddOneNode(pDoc,ename,pt,addmode);
			if (isthick)
				{
				pt[0]+=extru[0];
				pt[1]+=extru[1];
				pt[2]+=extru[2];
				SDS_AddOneNode(pDoc,ename,pt,addmode);
				}
			if (0!=halign || 0!=valign)
				{
				elp->get_11(pt1);
				sds_trans(pt1,&rbecs,&rbucs,0,pt);
				SDS_AddOneNode(pDoc,ename,pt,addmode);
				if (isthick)
					{
					pt[0]+=extru[0];
					pt[1]+=extru[1];
					pt[2]+=extru[2];
					SDS_AddOneNode(pDoc,ename,pt,addmode);
					}
				}
			}
			break;
		case DB_POLYLINE:
			//for splined plines, only add grips at control vertices
			//for fitted plines, add grips at each vertex and the editor will
			//	sort them out
			//for meshes, do not add grips at the face definition vertices (contain points at origin)
            {
			sds_point startpt,cc,arcdef;
			double bulge;
			int fi1;
			bool isthick=FALSE;
			elp->get_70(&fi1);
			plcharflag=fi1;
			elp->get_210(rbecs.resval.rpoint);
			if (!(fi1&(IC_PLINE_3DPLINE | IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH)))
				{
				elp->get_39(&thick);
				if (fabs(thick)>CMD_FUZZ)
					{
					ic_ptcpy(extru,rbecs.resval.rpoint);
					rbucs.resval.rint=2;
					sds_trans(extru,&rbwcs,&rbucs,1,extru);//convert to dcs
					rbucs.resval.rint=1;
					if (fabs(extru[0])>CMD_FUZZ || fabs(extru[1])>CMD_FUZZ)
						{
						isthick=TRUE;
						ic_ptcpy(extru,rbecs.resval.rpoint);
						extru[0]*=thick;
						extru[1]*=thick;
						extru[2]*=thick;
						sds_trans(extru,&rbwcs,&rbucs,1,extru);
						}
					}
				}
			elp=elp->next;
			if (elp->ret_type()!=DB_VERTEX)
				break;
			db_handitem *parnext=NULL;
			if (plcharflag & IC_PLINE_CLOSED)
				elp->get_10(startpt);

			for (;;)
				{
				if (elp->ret_type()!=DB_VERTEX)
					break;	//needed for closed pline
				elp->get_70(&fi1);
				if (plcharflag & IC_PLINE_SPLINEFIT)
					{
					//splined pline
					if (fi1 & IC_VERTEX_SPLINEVERTEX)
						{
						//if it's a vertex added by spline fitting, skip it
						elp=flp->entnext_noxref(elp);
						continue;
						}
					}

				// If the vertex describes a mesh face, skip it
				if ((plcharflag & (IC_PLINE_POLYFACEMESH | IC_PLINE_3DMESH)) &&
					(fi1 & IC_VERTEX_FACELIST) && !(fi1 & IC_VERTEX_3DMESHVERT))
					{
					elp=flp->entnext_noxref(elp);
					continue;
					}

				//add the grip for the current node...
				elp->get_10(pt);
				if (plcharflag & (IC_PLINE_3DPLINE | IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH))
					sds_trans(pt,&rbwcs,&rbucs,0,pt);
				else
					sds_trans(pt,&rbecs,&rbucs,0,pt);

				SDS_AddOneNode(pDoc,ename,pt,addmode);
				if (isthick)
					{
					pt[0]+=extru[0];
					pt[1]+=extru[1];
					pt[2]+=extru[2];
					SDS_AddOneNode(pDoc,ename,pt,addmode);
					}
				//setup the pointer to the next node or break if end of pline
				if (flp->entnext_noxref(elp)->ret_type()!=DB_VERTEX)
					{
					if (plcharflag & IC_PLINE_CLOSED)
						ic_ptcpy(pt,startpt);
					else
						break;
					}
				else
					{
					parnext=flp->entnext_noxref(elp);
					parnext->get_10(pt);
					}
				elp->get_10(pt1);
				elp->get_42(&bulge);
				if ((icadRealEqual(bulge,0.0)) ||
				  (0!=ic_bulge2arc(pt1,pt,bulge,cc,&arcdef[0],&arcdef[1],&arcdef[2])))
					{
					//if segment is straight or points are coincident...
					elp=flp->entnext_noxref(elp);
					continue;
					}
				else
					{
					//we have par->pt and pt to define start & end of segment
					//and bulge2arc gave us valid arc def. get midpt of segment:
					ic_normang(&arcdef[1],&arcdef[2]);
					sds_polar(cc,(arcdef[1]+arcdef[2])/2.0,arcdef[0],pt);
					if (plcharflag & (IC_PLINE_3DPLINE | IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH))
						sds_trans(pt,&rbwcs,&rbucs,0,pt);
					else
						sds_trans(pt,&rbecs,&rbucs,0,pt);
					SDS_AddOneNode(pDoc,ename,pt,addmode);
					if (isthick)
						{
						pt[0]+=extru[0];
						pt[1]+=extru[1];
						pt[2]+=extru[2];
						SDS_AddOneNode(pDoc,ename,pt,addmode);
						}
					elp=flp->entnext_noxref(elp);
					}
				}
			} // end of polyline
			break;
		case DB_LWPOLYLINE:
			//for fitted plines, add grips at each vertex and the editor will
			//	sort them out
            {
			sds_point startpt,cc,arcdef;
			double bulge;
			int fi1;
			bool isthick=FALSE;
			int vertno;
			db_lwpolyline *lwpl=(db_lwpolyline *)elp;

			lwpl->get_70(&fi1);
			plcharflag=fi1;
			lwpl->get_210(rbecs.resval.rpoint);
			lwpl->get_39(&thick);
			if (fabs(thick)>CMD_FUZZ)
				{
				ic_ptcpy(extru,rbecs.resval.rpoint);
				rbucs.resval.rint=2;
				sds_trans(extru,&rbwcs,&rbucs,1,extru);//convert to dcs
				rbucs.resval.rint=1;
				if (fabs(extru[0])>CMD_FUZZ || fabs(extru[1])>CMD_FUZZ)
					{
					isthick=TRUE;
					ic_ptcpy(extru,rbecs.resval.rpoint);
					extru[0]*=thick;
					extru[1]*=thick;
					extru[2]*=thick;
					sds_trans(extru,&rbwcs,&rbucs,1,extru);
					}
				}
			vertno=0;
			if (plcharflag & IC_PLINE_CLOSED)
				lwpl->get_10(startpt,vertno);

			for (;;)
				{
				if (vertno>=lwpl->ret_90())		// done with verts
					break;	//needed for closed pline
				//add the grip for the current node...
				lwpl->get_10(pt,vertno);
				sds_trans(pt,&rbecs,&rbucs,0,pt);
				SDS_AddOneNode(pDoc,ename,pt,addmode);
				if (isthick)
					{
					pt[0]+=extru[0];
					pt[1]+=extru[1];
					pt[2]+=extru[2];
					SDS_AddOneNode(pDoc,ename,pt,addmode);
					}
				//setup the pointer to the next node or break if end of pline
				if (vertno==lwpl->ret_90()-1)	// final vert
					{
					if (plcharflag & IC_PLINE_CLOSED)
						ic_ptcpy(pt,startpt);
					else
						break;
					}
				else
					{
					lwpl->get_10(pt,vertno+1);
					}
				lwpl->get_10(pt1,vertno);
				lwpl->get_42(&bulge,vertno);
				if ((icadRealEqual(bulge,0.0)) ||
				  (0!=ic_bulge2arc(pt1,pt,bulge,cc,&arcdef[0],&arcdef[1],&arcdef[2])))
					{
					//if segment is straight or points are coincident...
					vertno++;
					continue;
					}
				else
					{
					//we have par->pt and pt to define start & end of segment
					//and bulge2arc gave us valid arc def. get midpt of segment:
					ic_normang(&arcdef[1],&arcdef[2]);
					sds_polar(cc,(arcdef[1]+arcdef[2])/2.0,arcdef[0],pt);
					sds_trans(pt,&rbecs,&rbucs,0,pt);
					SDS_AddOneNode(pDoc,ename,pt,addmode);
					if (isthick)
						{
						pt[0]+=extru[0];
						pt[1]+=extru[1];
						pt[2]+=extru[2];
						SDS_AddOneNode(pDoc,ename,pt,addmode);
						}
					vertno++;
					}
				}
			} // end of lwpolyline
		break;
			//<alm>
		case DB_3DSOLID:
		case DB_REGION:
		case DB_BODY:
			{
				if (CModeler::get()->canView())
				{
					CDbAcisEntity* pObj = static_cast<CDbAcisEntity*>(elp);

					xstd<point>::vector grips;
					if (CModeler::get()->getGrips(pObj->getData(), grips))
					{
						transf w2u = wcs2ucs();
						for (int i = 0, n = grips.size(); i < n; i++)
						{
							grips[i] *= w2u;
							SDS_AddOneNode(pDoc, ename, grips[i], addmode);
						}
					}
				}
			}
			break;
			//</alm>
		}

	/*DG - 12.6.2003*/// We don't undraw grips actually here,
	// instead SDS_AddOneNode just adds them to CIcadDoc::m_gripsToUndraw;
	// SDS_DrawGripNodes can undraw them then;
	// so no need to redraw the entity here.
	/*if (addmode==0)
		sds_redraw(ename,IC_REDRAW_DRAW);*/


	return(RTNORM);
	}

int SDS_EntNodeDrag(CIcadDoc  *pDoc,sds_name ename, sds_point pt)
	{
	db_handitem *elp=(db_handitem *)ename[0];
	sds_real fr1,fr2,curelev,newelev,thick;
	struct resbuf rb,rbucs,rbent;
	sds_point p0,p1,p2,p3,p4,p5,p6,extru;
	db_handitem *savelp=NULL;
	struct db_BinChange *chglst = 0;
	int rc,dragmode, numFitPoints;
	db_drawing *flp;
	bool isthick=FALSE;

	bool bSkipReactorUpdation = FALSE;
	db_handitem *polyline = NULL; // if entity is a polyline, it stores the polyline entity so the
								  // any reactors on a polyline can be properly updated
	int res = RTERROR;

	// Save last point to be correspoinded to the selected node
	rb.restype = RT3DPOINT;
	ic_ptcpy( rb.resval.rpoint, pt );
	SDS_setvar(NULL,DB_QLASTPOINT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);

	/*D.G.*//* Allow grips editing in perspective mode now.

	//no grips if we're in perspective mode
	SDS_getvar(NULL,DB_QVIEWMODE,&rbucs,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if (rbucs.resval.rint&1)
		return(RTNORM);*/

	if (elp==NULL)
		goto out;

	SDS_AtNodeDrag=1;
	SDS_NodeDragEntDispObj = copyDisplayObject( (struct gr_displayobject*)elp->ret_disp() );
	//need to set this entity as the one being dragged to exclude it from
	//	various OSNAP settings
	ic_encpy(SDS_NodeDragEnt,ename);

	SDS_AddGripNodes(pDoc,ename,0);//paint out all the old nodes
//4M Item:28->
   SDS_DrawGripNodes(pDoc);
//<-4M Item:28
    sds_redraw(ename,IC_REDRAW_UNDRAW);
	SDS_getvar(NULL,DB_QDRAGMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	dragmode=rb.resval.rint;

	if (elp->ret_type()!=DB_POLYLINE && elp->ret_type()!=DB_LWPOLYLINE)
		SDS_CopyEntLink(&savelp,elp);

	rbucs.restype=RTSHORT;
	rbucs.resval.rint=1;

	flp=(db_drawing *)ename[1];

	//NOTE: all nodes, even those on 3D objects, are dragged at their
	//	current elevation.
	SDS_getvar(NULL,DB_QELEVATION,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	curelev=newelev=rb.resval.rreal;
//4M Spiros Item:27->
/*
    if (pt[2]!=curelev)
*/
	if (pt[2]!=curelev && !SDS_DragNOElevation)
//4M Spiros Item:27<-
        {
		SDS_CorrectElevation=curelev;
		SDS_ElevationChanged=TRUE;
		newelev=rb.resval.rreal=pt[2];
		rb.restype=RTREAL;
		//don't use quick mode (SDS_setvar) or coords will not update on screen correctly
		sds_setvar("ELEVATION",&rb);//this does an initview
		}

	switch(elp->ret_type())
		{
		case DB_VIEWPORT:
			{
			int eedupd=0,ret;
			struct sds_resbuf *prb,*rbp1,*viewptrx,*viewptry,*viewptrsiz,
				*elist10,*elist40,*elist41;
			sds_point pwcs;

			rbp1=prb=NULL;
			rb.restype=RTSTR;
			rb.resval.rstring="ACAD";
			rb.rbnext=NULL;
			if (NULL==(rbp1=sds_entgetx(ename,&rb)))
				goto vportbreak;
			//let p3 be new center
			//let p4[0] be new width,p4[1] be new height
			for (prb=rbp1;prb!=NULL;prb=prb->rbnext)
				{
				if (prb->restype==10)
					{
					elist10=prb;
					ic_ptcpy(p1,prb->resval.rpoint);
					}
				else if (prb->restype==40)
					{
					elist40=prb;
					fr1=prb->resval.rreal;
					}
				else if (prb->restype==41)
					{
					elist41=prb;
					fr2=prb->resval.rreal;
					}
				else if (prb->restype==-3)
					break;//eed always last
				}
			ic_ptcpy(p3,p1);
			p4[0]=fr1;
			p4[1]=fr2;
			fr1*=0.5;
			fr2*=0.5;
			//viewports are ALWAYS ortho in WCS, so we can do
			//	our math there (this is unique to viewports)

			//use rb as resbuf for wcs
			rb.restype=RTSHORT;
			rb.resval.rint=0;
			sds_trans(pt,&rbucs,&rb,0,pwcs);

			if (icadPointEqual(pwcs,p1))
				{
				SDS_DontDrawVPGuts=TRUE;
				if (SDS_dragobject(-1,dragmode,(sds_real *)ename,(double *)elp->retp_10(),
					0.0,ResourceString(IDC_GRIPS_1, "\nNew center for viewport: "),NULL,p3,NULL)!=RTNORM)
					{
					SDS_DontDrawVPGuts=FALSE;
					elp->set_10(p1);
					goto out;
					}
				SDS_DontDrawVPGuts=FALSE;
				sds_trans(p3,&rbucs,&rb,0,p3);
				elp->set_10(p3);
				}
			else if (icadRealEqual(pwcs[1],p1[1]) && pwcs[0]>p1[0])	//right side
				{
				p5[0]=elist10->resval.rpoint[0]-fr1;
				p5[1]=elist10->resval.rpoint[1]+fr2;
				p5[2]=0.0;
				p6[0]=elist10->resval.rpoint[0]-fr1;
				p6[1]=elist10->resval.rpoint[1]-fr2;
				p6[2]=0.0;
				if (SDS_dragobject(36,dragmode,p5,p6,
					 0.0,ResourceString(IDC_GRIPS_2, "\nNew location for edge of viewport: "),NULL,p2,NULL)!=RTNORM)
					 goto out;
				p4[0]=fabs(p2[0]-p1[0]+fr1);
				p3[0]=p1[0]-fr1+(p4[0]*0.5);
				eedupd=3;
				}
			else if (icadRealEqual(pwcs[1],p1[1]) && pwcs[0]<p1[0])	//left side
				{
				p5[0]=elist10->resval.rpoint[0]+fr1;
				p5[1]=elist10->resval.rpoint[1]+fr2;
				p5[2]=0.0;
				p6[0]=elist10->resval.rpoint[0]+fr1;
				p6[1]=elist10->resval.rpoint[1]-fr2;
				p6[2]=0.0;
				if (SDS_dragobject(36,dragmode,p5,p6,
					 0.0,ResourceString(IDC_GRIPS_2, "\nNew location for edge of viewport: "),NULL,p2,NULL)!=RTNORM)
					 goto out;
				p4[0]=fabs(p1[0]+fr1-p2[0]);
				p3[0]=p1[0]+fr1-(p4[0]*0.5);
				eedupd=3;
				}
			else if (icadRealEqual(pwcs[0],p1[0]) && pwcs[1]<p1[1])	//bottom edge
				{
				p5[0]=elist10->resval.rpoint[0]+fr1;
				p5[1]=elist10->resval.rpoint[1]+fr2;
				p5[2]=0.0;
				p6[0]=elist10->resval.rpoint[0]-fr1;
				p6[1]=elist10->resval.rpoint[1]+fr2;
				p6[2]=0.0;
				if (SDS_dragobject(37,dragmode,p5,p6,
					 0.0,ResourceString(IDC_GRIPS_2, "\nNew location for edge of viewport: "),NULL,p2,NULL)!=RTNORM)
					 goto out;
				p4[1]=fabs(p1[1]+fr2-p2[1]);
				p3[1]=p1[1]+fr2-(p4[1]*0.5);
				eedupd=3;
				}
			else if (icadRealEqual(pwcs[0],p1[0]) && pwcs[1]>p1[1])	//top edge
				{
				p5[0]=elist10->resval.rpoint[0]-fr1;
				p5[1]=elist10->resval.rpoint[1]-fr2;
				p5[2]=0.0;
				p6[0]=elist10->resval.rpoint[0]+fr1;
				p6[1]=elist10->resval.rpoint[1]-fr2;
				p6[2]=0.0;
				if (SDS_dragobject(37,dragmode,p5,p6,
					 0.0,ResourceString(IDC_GRIPS_2, "\nNew location for edge of viewport: "),NULL,p2,NULL)!=RTNORM)
					 goto out;
				p4[1]=fabs(p2[1]-p1[1]+fr2);
				p3[1]=p1[1]-fr2+(p4[1]*0.5);
				eedupd=3;
				}
			else if (pwcs[0]>p1[0] && pwcs[1]>p1[1])	//UR corner
				{
				p4[0]=pt[0]-(fr1*2.0);
				p4[1]=pt[1]-(fr2*2.0);
				p4[2]=0.0;
				if (RTNORM!=sds_getcorner(p4,ResourceString(IDC_GRIPS_3, "\nNew location for viewport corner: "),p2))
					goto out;
				p4[0]=fabs(p2[0]-p1[0]+fr1);
				p3[0]=p1[0]-fr1+(p4[0]*0.5);
				p4[1]=fabs(p2[1]-p1[1]+fr2);
				p3[1]=p1[1]-fr2+(p4[1]*0.5);
				eedupd=3;
				}
			else if (pwcs[0]>p1[0] && pwcs[1]<p1[1])	//LR corner
				{
				p4[0]=pt[0]-(fr1*2.0);
				p4[1]=pt[1]+(fr2*2.0);
				p4[2]=0.0;
				if (RTNORM!=sds_getcorner(p4,ResourceString(IDC_GRIPS_3, "\nNew location for viewport corner: "),p2))
					goto out;
				p4[0]=fabs(p2[0]-p1[0]+fr1);
				p3[0]=p1[0]-fr1+(p4[0]*0.5);
				p4[1]=fabs(p1[1]+fr2-p2[1]);
				p3[1]=p1[1]+fr2-(p4[1]*0.5);
				eedupd=3;
				}
			else if (pwcs[0]<p1[0] && pwcs[1]>p1[1])	//UL corner
				{
				p4[0]=pt[0]+(fr1*2.0);
				p4[1]=pt[1]-(fr2*2.0);
				p4[2]=0.0;
				if (RTNORM!=sds_getcorner(p4,ResourceString(IDC_GRIPS_3, "\nNew location for viewport corner: "),p2))
					goto out;
				p4[0]=fabs(p1[0]+fr1-p2[0]);
				p3[0]=p1[0]+fr1-(p4[0]*0.5);
				p4[1]=fabs(p2[1]-p1[1]+fr2);
				p3[1]=p1[1]-fr2+(p4[1]*0.5);
				eedupd=3;
				}
			else if (pwcs[0]<p1[0] && pwcs[1]<p1[1])	//LL corner
				{
				p4[0]=pt[0]+(fr1*2.0);
				p4[1]=pt[1]+(fr2*2.0);
				p4[2]=0.0;
				if (RTNORM!=sds_getcorner(p4,ResourceString(IDC_GRIPS_3, "\nNew location for viewport corner: "),p2))
					goto out;
				p4[0]=fabs(p1[0]+fr1-p2[0]);
				p3[0]=p1[0]+fr1-(p4[0]*0.5);
				p4[1]=fabs(p1[1]+fr2-p2[1]);
				p3[1]=p1[1]+fr2-(p4[1]*0.5);
				eedupd=3;
				}//else points won't get modified, so update won't do jack
			if (eedupd)
				{
				prb=ic_ret_assoc(rbp1,45);	// height
				if (NULL==prb)
					{
					sds_relrb(rbp1);
					goto vportbreak;
					}
				viewptrsiz=prb;
				fr1=0.5*prb->resval.rreal/fr2;//unit ratio (remember fr2 is half-height)
				prb=ic_ret_assoc(rbp1,12);	// center point in DCS
				if (NULL==prb)
					{
					sds_relrb(rbp1);
					goto vportbreak;
					}
				viewptrx=prb;
				prb=prb->rbnext;
				if (NULL==prb)
					{
					sds_relrb(rbp1);
					goto vportbreak;
					}
				viewptry=prb;
				if (eedupd&1)
					viewptrsiz->resval.rreal*=0.5*p4[1]/fr2;
				if (eedupd&2)
					{
					viewptrx->resval.rreal+=fr1*(p3[0]-p1[0]);
					viewptry->resval.rreal+=fr1*(p3[1]-p1[1]);
					}
				}
			ic_ptcpy(elist10->resval.rpoint,p3);
			elist40->resval.rreal=p4[0];
			elist41->resval.rreal=p4[1];
			ret=sds_entmod(rbp1);

			vportbreak:
			FreeResbufIfNotNull(&rbp1);
			prb=NULL;

			}
			break;
		case DB_MTEXT:
			{
			int fi1;
			rbent.restype=RTSHORT;
			rbent.resval.rint=0;

			elp->get_71(&fi1);
			elp->get_10(p1);
			elp->get_11(p2);
			elp->get_50(&fr1);
			elp->get_41(&fr2);
			sds_trans(pt,&rbucs,&rbent,0,p0);
			if (icadPointEqual(p0,p1))
				{
				//move the mtext
				if (SDS_dragobject(-1,dragmode,(sds_real *)ename,(double *)elp->retp_10(),
					0.0,ResourceString(IDC_GRIPS_4, "\nNew definition point: "),NULL,p0,NULL)!=RTNORM)
					{
					elp->set_10(p1);
					goto out;
					}
				sds_trans(p0,&rbucs,&rbent,0,p0);
				elp->set_10(p0);
				}
			else
				{
				if (SDS_dragobject(-7,dragmode,(sds_real *)ename,(double *)elp->retp_11(),
					0.0,ResourceString(IDC_GRIPS_5, "\nNew edge: "),NULL,p4,NULL)!=RTNORM)
					{
					elp->set_11(p2);
					elp->set_50(fr1);
					elp->set_41(fr2);
					goto out;
					}
				sds_trans(p4,&rbucs,&rbent,0,p4);

				// disable rotation of MTEXT on dragging, change only width
				p4[0] -= p1[0];
				p4[1] -= p1[1];
				p4[2] -= p1[2];
				fr2 = fabs(p4[0] * p2[0] + p4[1] * p2[1] + p4[2] * p2[2]) /
					(p2[0] * p2[0] + p2[1] * p2[1] + p2[2] * p2[2]);
				if(fi1 % 3 == 2)
					fr2 *= 2.0;//center justify
				elp->set_41(fr2);

				/* old code
				fr2=sds_distance(p4,p1);
				fr1=sds_angle(p1,p4);
				if (fr2<IC_ZRO)goto out;
				p3[0]=(p4[0]-p1[0])/fr2;
				p3[1]=(p4[1]-p1[1])/fr2;
				p3[2]=(p4[2]-p1[2])/fr2;


				if (2==(fi1%3))
					fr2*=2.0;//center justify
				else if (0==(fi1%3))
					{//right justify
					p3[0]=-p3[0];
					p3[1]=-p3[1];
					p3[2]=-p3[2];
					}
				elp->set_11(p3);
				elp->set_50(fr1);
				elp->set_41(fr2);
				*/
				}
			
			ReactorSubject *reactors = elp->getReactor();
			db_handitem *observer;
			reactors->ResetIterator();
			while (reactors->GetNextHanditem(SDS_CURDWG, &observer) && observer->ret_type() != DB_LEADER);
			if((observer!=NULL) && (observer->ret_type() == DB_LEADER))
				{
					int totalPoints;
					sds_point *ptarray = NULL;
					observer->get_76(&totalPoints);
					if(totalPoints != 0)
					{
						ptarray = new sds_point[totalPoints];
						if(NULL == ptarray)
							break;

						observer->get_10(ptarray, totalPoints);

						elp->get_10(p1);
						if(fabs(ptarray[totalPoints - 1][2] - p1[2]) > IC_ZRO)
							bSkipReactorUpdation = TRUE;
						
						delete []ptarray;
						ptarray = NULL;
					}
				}
			
			}
			break;
		case DB_SPLINE:
		case DB_LEADER:
			{
			int fi1,fi2,type,dragret,flag70;
			sds_point *ptarray,*pp1;
			bool fitpts;

			type=elp->ret_type();
			rbent.restype=RTSHORT;
			rbent.resval.rint=0;
			sds_trans(pt,&rbucs,&rbent,0,p1);
			if (type==DB_SPLINE)
				{
				elp->get_73(&fi1);
				if (fi1>0)
					fitpts=FALSE;
				else
					{
					fitpts=TRUE;
					elp->get_74(&fi1);
					}
				elp->get_70(&flag70);
				}
			else	//leader
				{
				elp->get_76(&fi1);
				flag70=0;
				}
			if (0==fi1)
				break;			
			if (NULL==(ptarray= new sds_point [fi1]))
				break;
			memset(ptarray, 0, sizeof(sds_point)*fi1);
			if (type==DB_SPLINE) {
				elp->get_74(&numFitPoints);
				if (fitpts)
					elp->get_11(ptarray,fi1);
				else {
					if (numFitPoints) 
						static_cast<db_spline*>(elp)->deleteFitPoints();
					elp->get_10(ptarray,fi1);
				}
			}
			else
				elp->get_10(ptarray,fi1);//leader
			pp1=ptarray;
			for (fi2=0;fi2<fi1;fi2++)
				{
				if (icadPointEqual(*pp1,p1))
					{
					ic_ptcpy(p2,*pp1);
					if (type==DB_SPLINE)
						{
						if (fitpts)
							dragret=SDS_dragobject(-1,dragmode,(sds_real *)ename,(double *)elp->retp_11(fi2),
								0.0,ResourceString(IDC_GRIPS_6, "\nNew location for fit point: "),NULL,p1,NULL);
						else
							dragret=SDS_dragobject(-1,dragmode,(sds_real *)ename,(double *)elp->retp_10(fi2),
								0.0,ResourceString(IDC_GRIPS_7, "\nNew location for control point: "),NULL,p1,NULL);
						}
					else
						dragret=SDS_dragobject(-1,dragmode,(sds_real *)ename,(double *)elp->retp_10(fi2),
						0.0,ResourceString(IDC_GRIPS_8, "\nNew location for leader point: "),NULL,p1,NULL);
					if (dragret!=RTNORM)
						{
						ic_ptcpy(*pp1,p2);
						if (0==fi2 && 1==(flag70&1))//if user moved 1st node & it's closed
							ic_ptcpy(ptarray[fi1-1],*pp1);

						if (type==DB_SPLINE)
							{
							if (fitpts)
								elp->set_11(ptarray,fi1);
							else
								elp->set_10(ptarray,fi1);
							}
						else
							elp->set_10(ptarray,fi1);	//leader
						goto out;
						}
					sds_trans(p1,&rbucs,&rbent,0,*pp1);
					if (0==fi2 && 1==(flag70&1))	//if user moved 1st node & it's closed
						ic_ptcpy(ptarray[fi1-1],*pp1);
					break;
					}
				pp1++;
				}
			if (type==DB_SPLINE)
			{
				/* There was a bug here: the control points where set as fit points, since set_11 () was allways used. (Kirk) */
				//elp->set_11(ptarray,fi1);
				if (fitpts)
					elp->set_11(ptarray,fi1);
				else
					elp->set_10(ptarray,fi1);
			}
			else
			{
				elp->set_10(ptarray,fi1);	//leader
				
				if(fi2 == fi1-1)
					((db_leader *)elp)->Calculate213();
				
			}

			IC_FREE(ptarray);
			break;
			}
		case DB_RAY:
		case DB_XLINE:
			{
			rbent.restype=RTSHORT;
			rbent.resval.rint=0;
			elp->get_10(p2);
			elp->get_11(p3);
			SDS_getvar(NULL,DB_QVIEWSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			rb.resval.rreal*=0.1;
			p4[0]=p2[0]+p3[0]*rb.resval.rreal;
			p4[1]=p2[1]+p3[1]*rb.resval.rreal;
			p4[2]=p2[2]+p3[2]*rb.resval.rreal;
			p5[0]=p2[0]-p3[0]*rb.resval.rreal;
			p5[1]=p2[1]-p3[1]*rb.resval.rreal;
			p5[2]=p2[2]-p3[2]*rb.resval.rreal;
			sds_trans(pt,&rbucs,&rbent,0,p1);
			if (icadPointEqual(p1,p2))
				{
				if (SDS_dragobject(-1,dragmode,(sds_real *)ename,(double *)elp->retp_10(),
					0.0,ResourceString(IDC_GRIPS_4, "\nNew definition point: "),NULL,p1,NULL)!=RTNORM)
					{
					elp->set_10(p2);
					goto out;
					}
				sds_trans(p1,&rbucs,&rbent,0,p1);
				elp->set_10(p1);
				}
			else if (icadPointEqual(p1,p4) || icadPointEqual(p1,p5))
				{
				if (SDS_dragobject(-7,dragmode,(sds_real *)ename,(double *)elp->retp_11(),
					0.0,ResourceString(IDC_GRIPS_9, "\nNew direction: "),NULL,p1,NULL)!=RTNORM)
					{
					elp->set_11(p3);
					goto out;
					}
				sds_trans(p1,&rbucs,&rbent,0,p1);
				p1[0]-=p2[0];
				p1[1]-=p2[1];
				p1[2]-=p2[2];
				p4[0]=sqrt(p1[0]*p1[0]+p1[1]*p1[1]+p1[2]*p1[2]);
				if (fabs(p4[0])>IC_ZRO)
					{
					p1[0]/=p4[0];
					p1[1]/=p4[0];
					p1[2]/=p4[0];
					elp->set_11(p1);
					}
				}
			}
			break;
		case DB_LINE:
			{
			rbent.restype=RTSHORT;
			rbent.resval.rint=0;
			elp->get_10(p2);
			elp->get_11(p3);
			
			p4[0]=(p2[0]+p3[0])/2.0;
			p4[1]=(p2[1]+p3[1])/2.0;
			p4[2]=(p2[2]+p3[2])/2.0;
			sds_trans(pt,&rbucs,&rbent,0,p1);
//4M Spiros Item:27->
			if(SDS_DragNOElevation)
				p1[2]=p3[2];
//4M Spiros Item:27<-
            if (icadPointEqual(p2,p1))
				{
				if (SDS_dragobject(-1,dragmode,(sds_real *)ename,(double *)elp->retp_10(),
					0.0,ResourceString(IDC_GRIPS_10, "\nNew start of line: "),NULL,p1,NULL)!=RTNORM)
					{
					elp->set_10(p2);
					goto out;
					}
				sds_trans(p1,&rbucs,&rbent,0,p1);
				elp->set_10(p1);
				}
			else if (icadPointEqual(p3,p1))
				{
				ic_ptcpy(p2,p3);
//4M Spiros Item:27->
/*
                if (SDS_dragobject(-1,dragmode,(sds_real *)ename,(double *)elp->retp_11(),
					0.0,ResourceString(IDC_GRIPS_11, "\nNew end of line: "),NULL,p1,NULL)!=RTNORM)
*/
                CString txt1(ResourceString(IDC_GRIPS_11, "\nNew end of line: "));
                if(SDS_AtDragMulti)
					   txt1=ResourceString(IDC_4M_GRIPS_DISPLACEMENT, "\nGrips' displacement vector: ");
                if (SDS_dragobject(-1,dragmode,(sds_real *)ename,(double *)elp->retp_11(),
					0.0,txt1,NULL,p1,NULL)!=RTNORM)
//4M Spiros Item:27<-
                    {
					elp->set_11(p2);
					goto out;
					}
				sds_trans(p1,&rbucs,&rbent,0,p1);
				elp->set_11(p1);
				}
			else if (icadPointEqual(p1,p4))
				{
				ic_ptcpy(p1,p2);
				ic_ptcpy(p2,p3);
				ic_ptcpy(p3,p4);
				sds_trans(p1,&rbent,&rbucs,0,p4);
				sds_trans(p2,&rbent,&rbucs,0,p5);
				sds_trans(p3,&rbent,&rbucs,0,p6);
				if (SDS_dragobject(30,dragmode,p4,p5,
					 0.0,ResourceString(IDC_GRIPS_12, "\nNew location for line: "),NULL,p6,NULL)!=RTNORM)goto out;
				sds_trans(p6,&rbucs,&rbent,0,p6);
				p1[0]+=p6[0]-p3[0];
				p2[0]+=p6[0]-p3[0];
				p1[1]+=p6[1]-p3[1];
				p2[1]+=p6[1]-p3[1];
				p1[2]+=p6[2]-p3[2];
				p2[2]+=p6[2]-p3[2];
				elp->set_10(p1);
				elp->set_11(p2);
				}
			else
				{
				elp->get_39(&thick);
				if (fabs(thick)<CMD_FUZZ)
					break;
				isthick=TRUE;
				elp->get_210(extru);
				p2[0]+=extru[0]*thick;
				p2[1]+=extru[1]*thick;
				p2[2]+=extru[2]*thick;
				if (icadPointEqual(p2,p1))
					{
					if (SDS_dragobject(-1,dragmode,(sds_real *)ename,(double *)elp->retp_10(),
						thick,ResourceString(IDC_GRIPS_10, "\nNew start of line: "),NULL,p1,NULL)!=RTNORM)
						{
						elp->set_10(p2);
						goto out;
						}
					sds_trans(p1,&rbucs,&rbent,0,p1);
					p1[0]-=extru[0]*thick;
					p1[1]-=extru[1]*thick;
					p1[2]-=extru[2]*thick;
					elp->set_10(p1);
					break;
					}
				p3[0]+=extru[0]*thick;
				p3[1]+=extru[1]*thick;
				p3[2]+=extru[2]*thick;
				if (icadPointEqual(p3,p1))
					{
					if (SDS_dragobject(-1,dragmode,(sds_real *)ename,(double *)elp->retp_11(),
						thick,ResourceString(IDC_GRIPS_11, "\nNew end of line: "),NULL,p1,NULL)!=RTNORM)
						{
						elp->set_10(p2);
						goto out;
						}
					sds_trans(p1,&rbucs,&rbent,0,p1);
					p1[0]-=extru[0]*thick;
					p1[1]-=extru[1]*thick;
					p1[2]-=extru[2]*thick;
					elp->set_11(p1);
					break;
					}
				//note: line currently has no middle node on extruded edge!
				}
			}
			break;
		case DB_ARC:
            {
			sds_point cent;
			double eang,sang,rad;
			rbent.restype=RT3DPOINT;
			elp->get_210(rbent.resval.rpoint);
			sds_trans(pt,&rbucs,&rbent,0,p0);
			SDS_getvar(NULL,DB_QVIEWDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			sds_trans(rb.resval.rpoint,&rbucs,&rbent,1,rb.resval.rpoint);
			elp->get_10(cent);
			elp->get_40(&rad);
			elp->get_50(&sang);
			elp->get_51(&eang);
			thick=0.0;

			redoarc:
			if (!icadRealEqual(p0[2],cent[2]) && !icadRealEqual(rb.resval.rpoint[2],0.0))
				{
				fr1=(cent[2]-p0[2])/rb.resval.rpoint[2];
				p0[1]+=fr1*rb.resval.rpoint[1];
				p0[0]+=fr1*rb.resval.rpoint[0];
				p0[2]=cent[2];
				}
			if (icadPointEqual(cent,p0))
				{
				elp->get_10(p2);
				if (SDS_dragobject(-1,dragmode,(sds_real *)ename,(double *)elp->retp_10(),
					0.0,ResourceString(IDC_GRIPS_13, "\nNew center of arc: "),NULL,p1,NULL)!=RTNORM)
					{
					elp->set_10(p2);
					goto out;
					}
				}
			else if (icadRealEqual(sds_distance(cent,p0),rad))
				{
				fr1=sds_angle(cent,p0);
				fr2=eang;
				ic_normang(&sang,&fr2);
				ic_normang(&eang,NULL);
				cent[2]-=thick;
				sds_polar(cent,sang,rad,p1);
				sds_polar(cent,eang,rad,p3);
				sds_polar(cent,(sang+fr2)/2.0,rad,p2);
				//this won't be an EXACT match with angle...
				//don't know what happens if user cancels...
				sds_trans(p1,&rbent,&rbucs,0,p1);
				sds_trans(p2,&rbent,&rbucs,0,p2);
				sds_trans(p3,&rbent,&rbucs,0,p3);
				if (fabs(fr1-sang) < IC_ZRO)
					{
					//move the start point
					if (SDS_dragobject(29,dragmode,p3,p2,thick,ResourceString(IDC_GRIPS_14, "\nNew start point: "),NULL,p1,NULL)!=RTNORM)
						goto out;
					}
				else if (fabs(fr1-eang) < IC_ZRO)
					{
					//move the end point
					if (SDS_dragobject(0,dragmode,p1,p2,thick,ResourceString(IDC_GRIPS_15, "\nNew end point: "),NULL,p3,NULL)!=RTNORM)
						goto out;
					}
				else
					{
					//move the midpoint
					if (SDS_dragobject(28,dragmode,p1,p3,thick,ResourceString(IDC_GRIPS_16, "\nNew midpoint: "),NULL,p2,NULL)!=RTNORM)
						goto out;
					}
				sds_trans(p1,&rbucs,&rbent,0,p1);
				sds_trans(p2,&rbucs,&rbent,0,p2);
				sds_trans(p3,&rbucs,&rbent,0,p3);
				if (!icadRealEqual(thick,0.0))
					{
					if (fabs(fr1-sang) < IC_ZRO)
						p1[2]-=thick;
					else if (fabs(fr1-eang) < IC_ZRO)
						p3[2]-=thick;
					else
						p2[2]-=thick;

					}
				if (0!=ic_3pt2arc(p1,p2,p3,cent,&rad,&sang,&eang))
					goto out;
				elp->set_10(cent);
				elp->set_40(rad);
				elp->set_50(sang);
				elp->set_51(eang);
				}
			else
				{
				if (!icadRealEqual(thick,0.0))
					break;
				elp->get_39(&thick);
				if (fabs(thick)<IC_ZRO)
					break;
				cent[2]+=thick;
				goto redoarc;
				}
			}
			break;
		case DB_CIRCLE:
            {
			rbent.restype=RT3DPOINT;
			elp->get_210(rbent.resval.rpoint);
			sds_point cent;
			double rad;
			elp->get_10(cent);
			elp->get_40(&rad);
			thick=0.0;
			sds_trans(pt,&rbucs,&rbent,0,p0);
			SDS_getvar(NULL,DB_QVIEWDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			sds_trans(rb.resval.rpoint,&rbucs,&rbent,1,rb.resval.rpoint);

			redocirc:
			if (!icadRealEqual(p0[2],cent[2]) && !icadRealEqual(rb.resval.rpoint[2], 0.0))
				{
				fr1=(cent[2]-p0[2])/rb.resval.rpoint[2];
				p0[1]+=fr1*rb.resval.rpoint[1];
				p0[0]+=fr1*rb.resval.rpoint[0];
				p0[2]=cent[2];
				}
			if (icadPointEqual(cent,p0))
				{
				cent[2]-=thick;
				ic_ptcpy(p2,cent);
				if (SDS_dragobject(-1,dragmode,(sds_real *)ename,(double *)elp->retp_10(),
					thick,ResourceString(IDC_GRIPS_17, "\nNew center of circle: "),NULL,p1,NULL)!=RTNORM)
					{
					elp->set_10(p2);
					goto out;
					}
				}
			else if (icadRealEqual(sds_distance(cent,p0),rad))
				{
				p2[0]=rad;
				cent[2]-=thick;
				sds_trans(cent,&rbent,&rbucs,0,p3);
				p1[0] = -1.0;
				if (SDS_dragobject(-2,dragmode,(sds_real *)ename,(double *)elp->retp_40(),
					thick,ResourceString(IDC_GRIPS_18, "\nNew circle radius: "),(struct resbuf **)p3,p1,NULL)!=RTNORM)
					{
					elp->set_40(p2[0]);
					goto out;
					}
				else
					{
					if (p1[0] > 0.0)
						{
						double newrad = p1[0];
						elp->set_40(newrad);

						// now reset lastpoint to be the point that the user
						// would have clicked had they not keyed in the radius
						struct resbuf rb;
						rb.restype = RT3DPOINT;
						sds_point lastPt;
						ic_ptcpy(lastPt, cent);

						double halfrad = rad/2;
						double xdiff = p0[0] - cent[0];
						double ydiff = p0[1] - cent[1];
						double diff;
						int icoord;

						if (fabs(xdiff) > halfrad)
							{
							icoord = 0;
							diff = xdiff;
							}
						else
							{
							icoord = 1;
							diff = ydiff;
							}

						if (diff > 0)
							lastPt[icoord] += newrad;
						else
							lastPt[icoord] -= newrad;


						ic_ptcpy(rb.resval.rpoint, lastPt);

						SDS_setvar(NULL,
							       DB_QLASTPOINT,
								   &rb,
								   SDS_CURDWG,
								   &SDS_CURCFG,
								   &SDS_CURSES,
								   0);

						}
					}
				}
			else
				{
				if (!icadRealEqual(thick,0.0))
					break;
				elp->get_39(&thick);
				if (fabs(thick)<IC_ZRO)
					break;
				cent[2]+=thick;
				goto redocirc;
				}
			}
			break;
		case DB_DIMENSION:
            {
			
			if(plink)
					plink->BeginDrag = TRUE;
			sds_point pt10, pt10ucs, pt11, pt11ucs, pt12, pt13, pt13ucs, pt14, pt14ucs, pt15, pt15ucs, pt16, pt16ucs, tp1, tp2;
            struct resbuf  rbwcs,
                           rbucs,
                           rbecs,
                           rbb;

			/*D.G.*//* Allow grips editing in perspective mode now.

	        // No grips if we're in perspective mode
	        SDS_getvar(NULL,DB_QVIEWMODE,&rbucs,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	        if (rbucs.resval.rint&1)
				return(RTNORM);*/

	        // Setup resbufs fo rtransformation of ent pts.
	        rbucs.rbnext=rbwcs.rbnext=rbecs.rbnext=NULL;
	        rbucs.restype=rbwcs.restype=RTSHORT;
	        rbucs.resval.rint=1;
	        rbwcs.resval.rint=0;

	        rbecs.restype=RT3DPOINT;

            SDS_getvar(NULL,DB_QUCSXDIR,&rbb,SDS_CURDWG,NULL,NULL);
            ic_ptcpy(tp1,rbb.resval.rpoint);
            SDS_getvar(NULL,DB_QUCSYDIR,&rbb,SDS_CURDWG,NULL,NULL);
            tp2[0]=tp1[1]*rbb.resval.rpoint[2]-rbb.resval.rpoint[1]*tp1[2];
            tp2[1]=tp1[2]*rbb.resval.rpoint[0]-rbb.resval.rpoint[2]*tp1[0];
            tp2[2]=tp1[0]*rbb.resval.rpoint[1]-rbb.resval.rpoint[0]*tp1[1];
            ic_ptcpy(rbecs.resval.rpoint,tp2);

            elp->get_10(pt10); sds_trans(pt10,&rbwcs,&rbucs,0,pt10ucs);
            elp->get_11(pt11); sds_trans(pt11,&rbecs,&rbucs,0,pt11ucs);
            elp->get_12(pt12);// sds_trans(pt12,&rbecs,&rbucs,0,pt12);
            elp->get_13(pt13); sds_trans(pt13,&rbwcs,&rbucs,0,pt13ucs);
            elp->get_14(pt14); sds_trans(pt14,&rbwcs,&rbucs,0,pt14ucs);
            elp->get_15(pt15); sds_trans(pt15,&rbwcs,&rbucs,0,pt15ucs);
            elp->get_16(pt16); sds_trans(pt16,&rbecs,&rbucs,0,pt16ucs);

			int type;
			elp->get_70(&type);
			type&=~DIM_BIT_6; // Use macros EBATECH(CNBR) 13-03-2003
			//if (type&DIM_BIT_6)	type-=DIM_BIT_6;
			// If the dimension text has been moved at a user-defined location rather than at the default location.
			type&=~DIM_TEXT_MOVED;
			//if (type&DIM_TEXT_MOVED) type-=DIM_TEXT_MOVED;

            if (type==DIM_DIAMETER || type==DIM_RADIUS)
				{
                SDS_AtNodeDrag=2;
	            cmd_makedimension(elp,flp,-1);
                cmd_makedimension(elp,flp,-1);
                SDS_AtNodeDrag=1;
				}

			if (type!=DIM_DIAMETER && type!=DIM_RADIUS && type!=DIM_Y_ORDINATE && type!=DIM_x_ORDINATE && icadRealEqual(pt10ucs[0],pt[0]) && icadRealEqual(pt10ucs[1],pt[1]))
				{
				SDS_AtNodeDrag=3;
				if (SDS_dragobject(-1,dragmode,(sds_real *)ename,(double *)elp->retp_10(),
					0.0,ResourceString(IDC_GRIPS_19, "\nNew dimension point: "),NULL,p1,NULL)!=RTNORM)
					{
			        elp->set_10(pt10);
			        elp->set_11(pt11);
			        elp->set_12(pt12);
			        elp->set_13(pt13);
			        elp->set_14(pt14);
			        elp->set_15(pt15);
			        elp->set_16(pt16);
					goto out;
					}
				}
			else if (icadRealEqual(pt11ucs[0],pt[0]) && icadRealEqual(pt11ucs[1],pt[1]))
				{
				SDS_AtNodeDrag=2;
				if (SDS_dragobject(-1,dragmode,(sds_real *)ename,(double *)elp->retp_11(),
					0.0,ResourceString(IDC_GRIPS_20, "\nNew text mid point: "),NULL,p1,NULL)!=RTNORM)
					{
			        elp->set_10(pt10);
			        elp->set_11(pt11);
			        elp->set_12(pt12);
			        elp->set_13(pt13);
			        elp->set_14(pt14);
			        elp->set_15(pt15);
			        elp->set_16(pt16);
					goto out;
					}
				}
			else if (type!=DIM_RADIUS && type!=DIM_DIAMETER && icadRealEqual(pt13ucs[0],pt[0]) && icadRealEqual(pt13ucs[1],pt[1]))
				{
				SDS_AtNodeDrag=4;
				if (SDS_dragobject(-1,dragmode,(sds_real *)ename,(double *)elp->retp_13(),
					0.0,ResourceString(IDC_GRIPS_19, "\nNew dimension point: "),NULL,p1,NULL)!=RTNORM)
					{
			        elp->set_10(pt10);
			        elp->set_11(pt11);
			        elp->set_12(pt12);
			        elp->set_13(pt13);
			        elp->set_14(pt14);
			        elp->set_15(pt15);
			        elp->set_16(pt16);
					goto out;
					}
				}
			else if (type!=DIM_RADIUS && type!=DIM_DIAMETER && icadRealEqual(pt14ucs[0],pt[0]) && icadRealEqual(pt14ucs[1],pt[1]))
				{
				SDS_AtNodeDrag=5;
    			if (SDS_dragobject(-1,dragmode,(sds_real *)ename,(double *)elp->retp_14(),
					0.0,ResourceString(IDC_GRIPS_19, "\nNew dimension point: "),NULL,p1,NULL)!=RTNORM)
					{
					elp->set_10(pt10);
					elp->set_11(pt11);
			        elp->set_12(pt12);
					elp->set_13(pt13);
					elp->set_14(pt14);
					elp->set_15(pt15);
					elp->set_16(pt16);
				goto out;
					}
				}
			else if ((type==DIM_ANGULAR/*  || type==DIM_RADIUS || type==DIM_DIAMETER*/) && (icadRealEqual(pt15ucs[0],pt[0]) && icadRealEqual(pt15ucs[1],pt[1])))
				{
				SDS_AtNodeDrag=6;
				if (SDS_dragobject(-1,dragmode,(sds_real *)ename,(double *)elp->retp_15(),
					0.0,ResourceString(IDC_GRIPS_19, "\nNew dimension point: "),NULL,p1,NULL)!=RTNORM)
					{
			        elp->set_10(pt10);
			        elp->set_11(pt11);
			        elp->set_12(pt12);
			        elp->set_13(pt13);
			        elp->set_14(pt14);
			        elp->set_15(pt15);
			        elp->set_16(pt16);
					goto out;
					}
				}
			else if ((type==DIM_ANGULAR || type==DIM_ANGULAR_3P) && (icadRealEqual(pt16ucs[0],pt[0]) && icadRealEqual(pt16ucs[1],pt[1])))
				{
				if (SDS_dragobject(-1,dragmode,(sds_real *)ename,(double *)elp->retp_16(),
					0.0,ResourceString(IDC_GRIPS_19, "\nNew dimension point: "),NULL,p1,NULL)!=RTNORM)
					{
			        elp->set_10(pt10);
			        elp->set_11(pt11);
			        elp->set_12(pt12);
			        elp->set_13(pt13);
			        elp->set_14(pt14);
			        elp->set_15(pt15);
			        elp->set_16(pt16);
					goto out;
					}
				}
			}
			break;
		case DB_ELLIPSE:
			{
			sds_point center,smavect,minvect;
			double axisratio,sa,ea;
			elp->get_10(center);
			elp->get_11(smavect);
			elp->get_40(&axisratio);
			elp->get_210(extru);
			rbent.restype=RTSHORT;
			rbent.resval.rint=0;
			sds_trans(pt,&rbucs,&rbent,0,p0);
			ic_crossproduct(smavect,extru,minvect);
			minvect[0]*=axisratio;
			minvect[1]*=axisratio;
			minvect[2]*=axisratio;
			if (icadPointEqual(center,p0))
				{
				ic_ptcpy(p2,center);
				if (SDS_dragobject(-1,dragmode,(sds_real *)ename,(double *)elp->retp_10(),
					0.0,ResourceString(IDC_GRIPS_21, "\nNew center of ellipse: "),NULL,p1,NULL)!=RTNORM)
					{
					elp->set_10(p2);
					goto out;
					}
				}
			else if (icadRealEqual(center[0]+smavect[0],p0[0]) && icadRealEqual(center[1]+smavect[1],p0[1]) && icadRealEqual(center[2]+smavect[2],p0[2]))
				{
				//move start point
				ic_ptcpy(p2,smavect);
				if (SDS_dragobject(-7,dragmode,(sds_real *)ename,(double *)elp->retp_11(),
					0.0,ResourceString(IDC_GRIPS_22, "\nNew ellipse major axis point: "),NULL,p1,NULL)!=RTNORM)
					{
					elp->set_11(p2);
					goto out;
					}
				smavect[0]=p1[0]-center[0];
				smavect[1]=p1[1]-center[1];
				smavect[2]=p1[2]-center[2];
				elp->set_11(smavect);
				}
			else if (icadRealEqual(center[0]-smavect[0],p0[0]) && icadRealEqual(center[1]-smavect[1],p0[1]) && icadRealEqual(center[2]-smavect[2],p0[2]))
				{
				//move end point
				ic_ptcpy(p2,smavect);
				//if the ang parameter is non-zero, we're dragging the side opposite the
				//	semimajor axis vector
				if ((rc=SDS_dragobject(-7,dragmode,(sds_real *)ename,(double *)elp->retp_11(),
					1.0,ResourceString(IDC_GRIPS_22, "\nNew ellipse major axis point: "),NULL,p1,NULL))!=RTNORM)
					{
					elp->set_11(p2);
					goto out;
					}
				smavect[0]=center[0]-p1[0];
				smavect[1]=center[1]-p1[1];
				smavect[2]=center[2]-p1[2];
				elp->set_11(smavect);
				if (rc!=RTNORM)
					goto out;
				}
			else if ((icadRealEqual(center[0]+minvect[0],p0[0]) && icadRealEqual(center[1]+minvect[1],p0[1]) && icadRealEqual(center[2]+minvect[2],p0[2])) ||
			         (icadRealEqual(center[0]-minvect[0],p0[0]) && icadRealEqual(center[1]-minvect[1],p0[1]) && icadRealEqual(center[2]-minvect[2],p0[2])))
				{
				//pt is on either semiminor axis...
				p2[0]=axisratio;
				if (SDS_dragobject(-2,dragmode,(sds_real *)ename,(double *)elp->retp_40(),
					0.0,ResourceString(IDC_GRIPS_23, "\nNew ellipse axis ratio: "),(struct resbuf **)center,p1,NULL)!=RTNORM)
					{
					elp->set_40(p2[0]);
					goto out;
					}
				else
					{
					// the following does not work for axis ratio; it seems to be
					// axis length instead.
					// elp->set_40(p1[0]);
					}
				}
			else
				{
				double angsin,angcos;
				elp->get_41(&sa);
				elp->get_42(&ea);
				//node is on start or end point of ellipse
				angsin=sin(sa);angcos=cos(sa);
				p1[0]=center[0]-minvect[0]*angsin+smavect[0]*angcos;
				p1[1]=center[1]-minvect[1]*angsin+smavect[1]*angcos;
				p1[2]=center[2]-minvect[2]*angsin+smavect[2]*angcos;
				if (icadPointEqual(p0,p1))		//adjust start parameter
					{
					if (SDS_dragobject(-10,dragmode,(sds_real *)ename,(double *)elp->retp_41(),
						0.0,ResourceString(IDC_GRIPS_24, "\nNew ellipse starting parameter: "),(struct resbuf **)center,p2,NULL)!=RTNORM)
						{
						elp->set_41(sa);
						goto out;
						}
					break;
					}
				angsin=sin(ea);angcos=cos(ea);
				p1[0]=center[0]-minvect[0]*angsin+smavect[0]*angcos;
				p1[1]=center[1]-minvect[1]*angsin+smavect[1]*angcos;
				p1[2]=center[2]-minvect[2]*angsin+smavect[2]*angcos;
				if (icadPointEqual(p0,p1))//adjust end parameter
					{
					if (SDS_dragobject(-10,dragmode,(sds_real *)ename,(double *)elp->retp_42(),
						0.0,ResourceString(IDC_GRIPS_25, "\nNew ellipse ending parameter: "),(struct resbuf **)center,p2,NULL)!=RTNORM)
						{
						elp->set_42(ea);
						goto out;
						}
					}
				}
			}
			break;
		case DB_POINT:
            {
			rbent.restype=RTSHORT;
			rbent.resval.rint=0;
			sds_trans(pt,&rbucs,&rbent,0,p0);
			elp->get_10(p2);
			if (icadPointEqual(p0,p2))
				{
				if (internalGetpoint(p2,ResourceString(IDC_GRIPS_26, "\nNew point location: "),elp->retp_10())!=RTNORM)
					{
					elp->set_10(p2);
					goto out;
					}
				}
			else
				{
				elp->get_39(&thick);
				if (fabs(thick)<IC_ZRO)
					break;
				db_handitem *elptemp;
				sds_name etemp;
				elp->get_210(extru);
				elptemp=new db_line(SDS_CURDWG);
				elptemp->set_39(thick);
				elptemp->set_210(extru);
				elptemp->set_10(p2);
				etemp[0]=(long)elptemp;
				etemp[1]=ename[1];
				if (SDS_dragobject(-1,dragmode,(sds_real *)etemp,(double *)elptemp->retp_11(),
					thick,ResourceString(IDC_GRIPS_27, "\nNew location for point: "),NULL,p1,NULL)==RTNORM)
					{
					sds_trans(p1,&rbucs,&rbent,0,p1);
					p1[0]-=extru[0]*thick;
					p1[1]-=extru[1]*thick;
					p1[2]-=extru[2]*thick;
					elp->set_10(p1);
					}
				delete elptemp;
				}
			}
			break;
		case DB_INSERT:
            {
			int fi1;
			char *fcp1;
			elp->get_2(&fcp1);
			if (strnsame(fcp1,"*X",2))
				{
				if (RTNORM!=cmd_hatch_blkmidpt(ename,p2) ||
					RTNORM!=internalGetpoint(p2,ResourceString(IDC_GRIPS_28, "\nNew center for hatch: "),p1))
					goto out;
				p1[0]-=p2[0];
				p1[1]-=p2[1];
				p1[2]-=p2[0];
				//p1 is now offset vector in UCS
				rbent.restype=RT3DPOINT;
				elp->get_210(rbent.resval.rpoint);
				sds_trans(p1,&rbucs,&rbent,1,p1);
				elp->get_10(p2);
				p2[0]+=p1[0];
				p2[1]+=p1[1];
				p2[2]+=p1[2];
				elp->set_10(p2);
				}
			else
				{
				elp->get_10(p2);
				SDS_getvar(NULL,DB_QDRAGMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				if (SDS_dragobject(-1,dragmode,(sds_real *)ename,(double *)elp->retp_10(),
					0.0,ResourceString(IDC_GRIPS_29, "\nNew insertion point: "),NULL,p1,NULL)!=RTNORM)
					{
					elp->set_10(p2);
					goto out;
					}
				}

			elp->get_66(&fi1);
			if (fi1&1)
				{
				struct resbuf *elist=NULL;
				sds_name attent;

				elp->get_10(p1);
				rbent.restype=RT3DPOINT;
				elp->get_210(rbent.resval.rpoint);
				p1[0]-=p2[0];
				p1[1]-=p2[1];
				p1[2]-=p2[2];
				sds_trans(p1,&rbent,&rbucs,1,p1);//translation vector in ucs
				//now work on attribs
				sds_entnext_noxref(ename,attent);
				for (;;)
					{
					if (elist!=NULL){sds_relrb(elist); elist=NULL;}
					if (NULL==(elist=sds_entget(attent)))
						goto out;
					//best to repatedly use ic_assoc, because we need to
					//check 0 & get 210 before pt translation
					ic_assoc(elist,0);
					if (!strsame(ic_rbassoc->resval.rstring,"ATTRIB"))
						{
						sds_relrb(elist);
						break;
						}
					if (ic_assoc(elist,210))
						{
						rbent.resval.rpoint[0]=rbent.resval.rpoint[1]=0.0;
						rbent.resval.rpoint[2]=1.0;
						}
					else
						ic_ptcpy(rbent.resval.rpoint,ic_rbassoc->resval.rpoint);
					sds_trans(p1,&rbucs,&rbent,1,p3);
					ic_assoc(elist,10);
					ic_rbassoc->resval.rpoint[0]+=p3[0];
					ic_rbassoc->resval.rpoint[1]+=p3[1];
					ic_rbassoc->resval.rpoint[2]+=p3[2];
					struct resbuf *temprb=ic_ret_assoc(elist,72);
					int halign=temprb->resval.rint;
					temprb=ic_ret_assoc(elist,74);
					int valign=temprb->resval.rint;
					// non lower-left justified ones need to have the second point moved also
					if (halign || valign)
						{
						ic_assoc(elist,11);
						ic_rbassoc->resval.rpoint[0]+=p3[0];
						ic_rbassoc->resval.rpoint[1]+=p3[1];
						ic_rbassoc->resval.rpoint[2]+=p3[2];
						}
					if (RTNORM!=sds_entmod(elist))
						{
						sds_relrb(elist);
						goto out;
						}
					FreeResbufIfNotNull(&elist);
					if (RTNORM!=sds_entnext_noxref(attent,attent))
						break;
					}
				}
			}
			break;
			//<alm>
		case DB_3DSOLID:
		case DB_REGION:
		case DB_BODY:
            {
				if (CModeler::get()->canModify())
				{
					CDbAcisEntity* pObj = static_cast<CDbAcisEntity*>(elp);
					transf u2w = ucs2wcs();
					pObj->draggingStart() = pt;
					pObj->draggingStart() *= u2w;

					transf forward, proj, backward;
					{
						resbuf rb;
						sds_getvar("VIEWDIR", &rb);
						gvector view = rb.resval.rpoint;
						view *= u2w;
						pObj->viewDir() = view;

						sds_getvar("UCSXDIR", &rb);
						gvector x = rb.resval.rpoint;
						sds_getvar("UCSYDIR", &rb);
						gvector y = rb.resval.rpoint;

						gvector normal = x*y;
						pObj->axisZ() = normal;
					}

					point npt = pt;
					if (SDS_dragobject(-1, dragmode, (sds_real *)ename, pObj->draggingCurr(),
						0.0, "\nNew solid's location: ", NULL, npt, NULL)!=RTNORM)
					{
						pObj->draggingCurr() = pObj->draggingStart() = point(0.,0.,0.);
						gr_freedisplayobjectll(reinterpret_cast<gr_displayobject*>(elp->ret_disp()));
						pObj->set_disp(0);
						goto out;
					}

					point pnt = pObj->draggingCurr();
					gvector axisZ = pObj->axisZ();
					gvector view = pObj->viewDir();
					point base = pObj->draggingStart();

					transf tr = icl::translation(pnt - base);
					pObj->draggingCurr() = pObj->draggingStart() = point(0.,0.,0.);

					CDbAcisEntityData newObj;
					if (CModeler::get()->transform(pObj->getData(), newObj, tr))
						pObj->setData(newObj);
				}
			}
			break;
			//</alm>
		case DB_SHAPE:
            {
				//note: no node added at extruded edge, so do NOT create dragging case for it
			elp->get_10(p2);
			if (internalGetpoint(p2,ResourceString(IDC_GRIPS_29, "\nNew insertion point: "),elp->retp_10())!=RTNORM)
				{
				elp->set_10(p2);
				goto out;
				}
			}
			break;
		case DB_TOLERANCE:
            {
			elp->get_10(p2);
			SDS_getvar(NULL,DB_QDRAGMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			if (SDS_dragobject(-1,dragmode,(sds_real *)ename,(double *)elp->retp_10(),
				0.0,ResourceString(IDC_GRIPS_29, "\nNew insertion point: "),NULL,p1,NULL)!=RTNORM)
				{
				elp->set_10(p2);
				goto out;
				}
			}
			break;
		case DB_SOLID:
            {
			sds_point parpt[4];
			elp->get_10(parpt[0]);
			elp->get_11(parpt[1]);
			elp->get_12(parpt[2]);
			elp->get_13(parpt[3]);
			thick=0.0;
			isthick=FALSE;

			rbent.restype=RT3DPOINT;
			elp->get_210(rbent.resval.rpoint);
			sds_trans(pt,&rbucs,&rbent,0,p0);
			SDS_getvar(NULL,DB_QVIEWDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			sds_trans(rb.resval.rpoint,&rbucs,&rbent,1,rb.resval.rpoint);

			redosolid:
			if (!icadRealEqual(p0[2],parpt[0][2]) && 0.0!=rb.resval.rpoint[2])
				{
				fr1=(parpt[0][2]-p0[2])/rb.resval.rpoint[2];
				p0[1]+=fr1*rb.resval.rpoint[1];
				p0[0]+=fr1*rb.resval.rpoint[0];
				p0[2]=parpt[0][2];
				}
			if (icadPointEqual(p0,parpt[0]))
				{
				elp->get_10(p2);
				if (SDS_dragobject(-1,dragmode,(sds_real *)ename,(double *)elp->retp_10(),
					thick,ResourceString(IDC_GRIPS_30, "\nNew corner for plane: "),NULL,p1,NULL)!=RTNORM)
					{
					elp->set_10(p2);
					goto out;
					}
				if (isthick)
					{
					p1[2]-=thick;
					elp->set_10(p1);
					}
				}
			else if (icadPointEqual(p0,parpt[1]))
				{
				elp->get_11(p2);
				if (SDS_dragobject(-1,dragmode,(sds_real *)ename,(double *)elp->retp_11(),
					thick,ResourceString(IDC_GRIPS_30, "\nNew corner for plane: "),NULL,p1,NULL)!=RTNORM)
					{
					elp->set_11(p2);
					goto out;
					}
				if (isthick)
					{
					p1[2]-=thick;
					elp->set_11(p1);
					}
				}
			else if (icadPointEqual(p0,parpt[2]))
				{
				elp->get_12(p2);
				if (SDS_dragobject(-1,dragmode,(sds_real *)ename,(double *)elp->retp_12(),
					thick,ResourceString(IDC_GRIPS_30, "\nNew corner for plane: "),NULL,p1,NULL)!=RTNORM)
					{
					elp->set_12(p2);
					goto out;
					}
				if (isthick)
					{
					p1[2]-=thick;
					elp->set_12(p1);
					}
				}
			else if (icadPointEqual(p0,parpt[3]))
				{
				elp->get_13(p2);
				if (SDS_dragobject(-1,dragmode,(sds_real *)ename,(double *)elp->retp_13(),
					thick,ResourceString(IDC_GRIPS_30, "\nNew corner for plane: "),NULL,p1,NULL)!=RTNORM)
					{
					elp->set_13(p2);
					goto out;
					}
				if (isthick)
					{
					p1[2]-=thick;
					elp->set_13(p1);
					}
				}
			else
				{
				if (!icadRealEqual(thick,0.0))
					break;
				elp->get_39(&thick);
				if (fabs(thick)<IC_ZRO)
					break;
				parpt[0][2]+=thick;
				parpt[1][2]+=thick;
				parpt[2][2]+=thick;
				parpt[3][2]+=thick;
				isthick=TRUE;
				goto redosolid;
				}
			}
			break;
		case DB_3DFACE:
            {
			sds_point parpt[4];
			elp->get_10(parpt[0]);
			elp->get_11(parpt[1]);
			elp->get_12(parpt[2]);
			elp->get_13(parpt[3]);
			rbent.restype=RTSHORT;
			rbent.resval.rint=0;
			sds_trans(pt,&rbucs,&rbent,0,p0);
			if (icadPointEqual(p0,parpt[0]))
				{
				elp->get_10(p2);
				if (SDS_dragobject(-1,dragmode,(sds_real *)ename,(double *)elp->retp_10(),
					0.0,ResourceString(IDC_GRIPS_31, "\nNew corner for face: "),NULL,p1,NULL)!=RTNORM)
					{
					elp->set_10(p2);
					goto out;
					}
				}
			else if (icadPointEqual(p0,parpt[1]))
				{
				elp->get_11(p2);
				if (SDS_dragobject(-1,dragmode,(sds_real *)ename,(double *)elp->retp_11(),
					0.0,ResourceString(IDC_GRIPS_31, "\nNew corner for face: "),NULL,p1,NULL)!=RTNORM)
					{
					elp->set_11(p2);
					goto out;
					}
				}
			else if (icadPointEqual(p0,parpt[2]))
				{
				elp->get_12(p2);
				if (SDS_dragobject(-1,dragmode,(sds_real *)ename,(double *)elp->retp_12(),
					0.0,ResourceString(IDC_GRIPS_31, "\nNew corner for face: "),NULL,p1,NULL)!=RTNORM)
					{
					elp->set_12(p2);
					goto out;
					}
				}
			else
				{
				elp->get_13(p2);
				if (SDS_dragobject(-1,dragmode,(sds_real *)ename,(double *)elp->retp_13(),
					0.0,ResourceString(IDC_GRIPS_31, "\nNew corner for face: "),NULL,p1,NULL)!=RTNORM)
					{
					elp->set_13(p2);
					goto out;
					}
				}
			}
			break;
		case DB_TRACE:
            {
			sds_point parpt[4];
			elp->get_10(parpt[0]);
			elp->get_11(parpt[1]);
			elp->get_12(parpt[2]);
			elp->get_13(parpt[3]);
			rbent.restype=RT3DPOINT;
			elp->get_210(rbent.resval.rpoint);
			sds_trans(pt,&rbucs,&rbent,0,p0);
			SDS_getvar(NULL,DB_QVIEWDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			sds_trans(rb.resval.rpoint,&rbucs,&rbent,1,rb.resval.rpoint);
			thick=0.0;

			redotrace:
			if (!icadRealEqual(p0[2],parpt[0][2]) && !icadRealEqual(rb.resval.rpoint[2],0.0))
				{
				fr1=(parpt[0][2]-p0[2])/rb.resval.rpoint[2];
				p0[1]+=fr1*rb.resval.rpoint[1];
				p0[0]+=fr1*rb.resval.rpoint[0];
				p0[2]=parpt[0][2];
				}
			if (icadPointEqual(p0,parpt[0]))
				{
				elp->get_10(p2);
				if (SDS_dragobject(-1,dragmode,(sds_real *)ename,(double *)elp->retp_10(),
					thick,ResourceString(IDC_GRIPS_32, "\nNew corner for trace: "),NULL,p1,NULL)!=RTNORM)
					{
					elp->set_10(p2);
					goto out;
					}
				if (isthick)
					{
					p1[2]-=thick;
					elp->set_10(p1);
					}
				}
			else if (icadPointEqual(p0,parpt[1]))
				{
				elp->get_11(p2);
				if (SDS_dragobject(-1,dragmode,(sds_real *)ename,(double *)elp->retp_11(),
					thick,ResourceString(IDC_GRIPS_32, "\nNew corner for trace: "),NULL,p1,NULL)!=RTNORM)
					{
					elp->set_11(p2);
					goto out;
					}
				if (isthick)
					{
					p1[2]-=thick;
					elp->set_11(p1);
					}
				}
			else if (icadPointEqual(p0,parpt[2]))
				{
				elp->get_12(p2);
				if (SDS_dragobject(-1,dragmode,(sds_real *)ename,(double *)elp->retp_12(),
					thick,ResourceString(IDC_GRIPS_32, "\nNew corner for trace: "),NULL,p1,NULL)!=RTNORM)
					{
					elp->set_12(p2);
					goto out;
					}
				if (isthick)
					{
					p1[2]-=thick;
					elp->set_12(p1);
					}
				}
			else if (icadPointEqual(p0,parpt[3]))
				{
				elp->get_13(p2);
				if (SDS_dragobject(-1,dragmode,(sds_real *)ename,(double *)elp->retp_13(),
					thick,ResourceString(IDC_GRIPS_32, "\nNew corner for trace: "),NULL,p1,NULL)!=RTNORM)
					{
					elp->set_13(p2);
					goto out;
					}
				if (isthick)
					{
					p1[2]-=thick;
					elp->set_13(p1);
					}
				}
			else
				{
				if (!icadRealEqual(thick,0.0))
					break;
				elp->get_39(&thick);
				if (fabs(thick)<IC_ZRO)
					break;
				parpt[0][2]+=thick;
				parpt[1][2]+=thick;
				parpt[2][2]+=thick;
				parpt[3][2]+=thick;
				isthick=TRUE;
				goto redotrace;
				}
			}
			break;
		case DB_TEXT:
		case DB_ATTDEF:
		case DB_ATTRIB:
            {
			int halign,valign;
			sds_real htwidth;
			rbent.restype=RT3DPOINT;
			elp->get_210(rbent.resval.rpoint);
			elp->get_10(p0);
			elp->get_11(p1);
			elp->get_50(&fr1);
			elp->get_72(&halign);
			elp->get_73(&valign);
			thick=0.0;
			if (3==halign)
				elp->get_40(&htwidth);//save height of aligned text
			else if (5==halign)
				elp->get_41(&htwidth);//width of fitted text
			sds_trans(pt,&rbucs,&rbent,0,p2);
			SDS_getvar(NULL,DB_QVIEWDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			sds_trans(rb.resval.rpoint,&rbucs,&rbent,1,rb.resval.rpoint);

			redotext:
			if (!icadRealEqual(p0[2],p2[2]) && !icadRealEqual(rb.resval.rpoint[2],0.0))
				{
				fr1=(p0[2]-p2[2])/rb.resval.rpoint[2];
				p2[1]+=fr1*rb.resval.rpoint[1];
				p2[0]+=fr1*rb.resval.rpoint[0];
				p2[2]=p0[2];
				}
			if (icadPointEqual(p2,p0))
				{
				if (SDS_dragobject(-1,dragmode,(sds_real *)ename,(double *)elp->retp_10(),
					thick,ResourceString(IDC_GRIPS_33, "\nNew base point for text: "),NULL,p3,NULL)!=RTNORM)
					{
					if (isthick)
						p0[2]-=thick;
					elp->set_10(p0);
					goto out;
					}
				sds_trans(p3,&rbucs,&rbent,0,p3);
				if (isthick)
					{
					p3[2]-=thick;
					p1[2]-=thick;
					p0[2]-=thick;
					}
				elp->set_10(p3);
				if (halign!=0 || valign!=0)
					{
					p1[0]+=p3[0]-p0[0];
					p1[1]+=p3[1]-p0[1];
					p1[2]+=p3[2]-p0[2];
					elp->set_11(p1);
					}
				}
			else if ((halign!=0 || valign!=0) && icadPointEqual(p2,p1))
				{
				if (SDS_dragobject(-1,dragmode,(sds_real *)ename,(double *)elp->retp_11(),
					thick,ResourceString(IDC_GRIPS_34, "\nNew alignment point for text: "),NULL,p3,NULL)!=RTNORM)
					{
					if (isthick)
						p1[2]-=thick;
					elp->set_11(p1);
					if (3==halign)
						elp->set_40(htwidth);//height of aligned
					else if (5==halign)
						elp->set_41(htwidth);//width of fitted
					elp->set_50(fr1);
					goto out;
					}
				sds_trans(p3,&rbucs,&rbent,0,p3);
				if (isthick)
					{
					p3[2]-=thick;
					p1[2]-=thick;
					p0[2]-=thick;
					}
				elp->set_11(p3);
				if (halign==3 || halign==5)
					{
					fr1=sds_angle(p0,p3);
					elp->set_50(fr1);
					}
				else if (halign!=0 || valign!=0)
					{
					p0[0]+=p3[0]-p1[0];
					p0[1]+=p3[1]-p1[1];
					p0[2]+=p3[2]-p1[2];
					elp->set_10(p0);
					}
				}
			else
				{
				if (!icadRealEqual(thick,0.0))
					break;
				elp->get_39(&thick);
				if (fabs(thick)<IC_ZRO)
					break;
				p0[2]+=thick;
				p1[2]+=thick;
				isthick=TRUE;
				goto redotext;
				}
			}
			break;
		case DB_POLYLINE:
            {
			int got1,flags,vflags;
			double bulge;

			polyline = elp;

			elp->get_70(&flags);
			thick=0.0;
			if (flags & (IC_PLINE_3DPLINE | IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH))
				{
				rbent.restype=RTSHORT;
				rbent.resval.rint=0;
				}
			else
				{
				rbent.restype=RT3DPOINT;
				elp->get_210(rbent.resval.rpoint);
				}
			rc=sds_trans(pt,&rbucs,&rbent,0,p0);

			redopoly:
			if (rbent.restype==RT3DPOINT)
				{
				SDS_getvar(NULL,DB_QVIEWDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				sds_trans(rb.resval.rpoint,&rbucs,&rbent,1,rb.resval.rpoint);
				elp->get_10(p1);
				p1[2]+=thick;
				if (!icadRealEqual(p0[2],p1[2]) && !icadRealEqual(rb.resval.rpoint[2], 0.0))
					{
					fr1=(p1[2]-p0[2])/rb.resval.rpoint[2];
					p0[1]+=fr1*rb.resval.rpoint[1];
					p0[0]+=fr1*rb.resval.rpoint[0];
					p0[2]=p1[2];
					}
				}
			elp=flp->entnext_noxref(elp);
			got1=0;
			for (;;)
				{
				if (elp->ret_type()!=DB_VERTEX)
					break;//don't worry about closure.  This loop for on-vtx grips only
				elp->get_10(p3);
				p3[2]+=thick;//always 0.0 if it's not 2D
				//on a splined pline, you have vertices which don't have grips, and which
				//may also be at the user-selected point.  We need to bypass them
				if (flags & (IC_PLINE_CURVEFIT | IC_PLINE_SPLINEFIT))
					{
					elp->get_70(&vflags);
					if ((flags & IC_PLINE_SPLINEFIT) && (vflags & IC_VERTEX_SPLINEVERTEX))
						{
						//skip spline-fit vertices
						elp=flp->entnext_noxref(elp);
						continue;
						}
					}
				if (icadPointEqual(p0,p3))
					{

					// Because we need to find the elp to move set the Undo savelp here.
					SDS_CopyEntLink(&savelp,elp);

					if ((flags & IC_PLINE_SPLINEFIT) || ((flags & IC_PLINE_CURVEFIT) && 0==(vflags & IC_VERTEX_FROMFIT)))
						{
						//if this is a splined pline, and grip on control point(we filtered out all the rest)
						//OR it's a fitted pline and the grip is on a control point then
						//re-fit the curve.  if a vertex added by fitting, it uses the other drag
						// and gets moved as if it were a regular vertex
						if (SDS_dragobject(-1,dragmode,(sds_real *)ename,(double *)elp->retp_10(),
							thick,ResourceString(IDC_GRIPS_7, "\nNew location of control point: "),NULL,p1,NULL)!=RTNORM)
							{
							elp->set_10(p0);
							goto out;
							}
						sds_trans(p1,&rbucs,&rbent,0,p1);
						p1[2]-=thick;
						elp->set_10(p1);
						got1=1;

						int fittype,splinesegs,ret;
						struct resbuf *elist=NULL;

						rb.restype=RTSTR;
						rb.rbnext=NULL;
						rb.resval.rstring="*";
						if (NULL==(elist=sds_entgetx(ename,&rb)))
							goto out;
						//if pline has been splined or fitted, re-fit the curve
						SDS_getvar(NULL,DB_QSPLINESEGS,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
						splinesegs=rb.resval.rint;
						if (flags & IC_PLINE_CURVEFIT)
							{
							SDS_getvar(NULL,DB_QFITTYPE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
							fittype=rb.resval.rint;
							ret=cmd_pedit_fitcurve(elist,1,fittype,splinesegs,ename);
							}
						else
							{
							SDS_getvar(NULL,DB_QSPLINETYPE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
							fittype=rb.resval.rint;
							ret=cmd_pedit_fitcurve(elist,0,fittype,splinesegs,ename);
							}
						FreeResbufIfNotNull(&elist);
						if (ret!=RTNORM)
							goto out;
						}
					else
						{
						CUndoSaver undoSaver(elp, flp);
						if (SDS_dragobject(-1,dragmode,(sds_real *)ename,(double *)elp->retp_10(),
							thick,ResourceString(IDC_GRIPS_35, "\nNew location of vertex: "),NULL,p1,NULL)!=RTNORM)
							{
							elp->set_10(p0);
							goto out;
							}
						sds_trans(p1,&rbucs,&rbent,0,p1);
						p1[2]-=thick;
						elp->set_10(p1);
						undoSaver.saveToUndo();
						got1=1;
						}
					break;
					}
				if (!got1)
					elp=flp->entnext_noxref(elp);
				//check the grip for the current node...
				}
			if (got1)
				break;
			//if we got this far, point wasn't on a vtx, but was midpt on an arc segment
			elp=(db_handitem *)ename[0];
			sds_point startpt,cc,arcdata;
			sds_real dist;


			elp=flp->entnext_noxref(elp);
			if (elp->ret_type()!=DB_VERTEX)
				break;

			if (flags & IC_PLINE_CLOSED)
				elp->get_10(startpt);

			for (; elp!=NULL; elp=flp->entnext_noxref(elp))
				{
				if (elp->ret_type()!=DB_VERTEX)	//needed for closed pline
					break;
				elp->get_42(&bulge);
				if (icadRealEqual(bulge,0.0))
					continue;
				//now we're on a curved segment, so check if it's the one
				//setup the pointer to the next node or break if end of open pline
				if (flp->entnext_noxref(elp)->ret_type()!=DB_VERTEX)
					{
					if (flags & IC_PLINE_CLOSED)
						ic_ptcpy(p1,startpt);
					else break;
					}
				else
					flp->entnext_noxref(elp)->get_10(p1);
				elp->get_10(p4);
				p4[2]+=thick;
				p1[2]+=thick;
				p3[0]=fabs(bulge)*cmd_dist2d(p4,p1)*IC_ZRO;
				if (0==ic_ptsegdist(p0,p4,p1,bulge,&dist) && dist<p3[0])
					{
					got1=1;
					sds_redraw(ename,IC_REDRAW_DRAW);

					// Because we need to find the elp to move set the Undo savelp here.
					SDS_CopyEntLink(&savelp,elp);

					sds_trans(p1,&rbent,&rbucs,0,p1);
					sds_trans(p2,&rbent,&rbucs,0,p2);
					sds_trans(p4,&rbent,&rbucs,0,p4);
					if (SDS_dragobject(28,dragmode,p4,p1,thick,ResourceString(IDC_GRIPS_36, "\nNew point along vertex: "),NULL,p2,NULL)!=RTNORM)
						goto out;
					sds_trans(p1,&rbucs,&rbent,0,p1);
					sds_trans(p2,&rbucs,&rbent,0,p2);
					sds_trans(p4,&rbucs,&rbent,0,p4);
					sds_redraw(ename,IC_REDRAW_UNDRAW);
					if (0==ic_3pt2arc(p4,p2,p1,cc,&arcdata[0],&arcdata[1],&arcdata[2]))
						{
						ic_normang(&arcdata[1],&arcdata[2]);
						if (fabs(sds_angle(cc,p4)-arcdata[1]) < IC_ZRO)
							//starting angle is at beginning of segment, seg ccw
							elp->set_42(tan((arcdata[2]-arcdata[1])/4.0));
						else
							elp->set_42(-tan((arcdata[2]-arcdata[1])/4.0));
						break;
						}
					}
				}
			if (rbent.restype==RT3DPOINT && !got1 && icadRealEqual(thick,0.0))
				{
				elp=(db_handitem *)ename[0];
				elp->get_39(&thick);
				if (fabs(thick)<IC_ZRO)
					break;
				goto redopoly;
				}
			}
			break;
		case DB_LWPOLYLINE:
            {
			int got1,flags;
			double bulge;
			int vertno;
			db_lwpolyline *lwpl=(db_lwpolyline *)elp;

			lwpl->get_70(&flags);
			thick=0.0;
			rbent.restype=RT3DPOINT;
			lwpl->get_210(rbent.resval.rpoint);
			rc=sds_trans(pt,&rbucs,&rbent,0,p0);

			redolwpoly:
			if (rbent.restype==RT3DPOINT)
				{
				SDS_getvar(NULL,DB_QVIEWDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				sds_trans(rb.resval.rpoint,&rbucs,&rbent,1,rb.resval.rpoint);
				lwpl->get_38(&p1[2]);
				p1[2]+=thick;
				if (!icadRealEqual(p0[2],p1[2]) && !icadRealEqual(rb.resval.rpoint[2],0.0))
					{
					fr1=(p1[2]-p0[2])/rb.resval.rpoint[2];
					p0[1]+=fr1*rb.resval.rpoint[1];
					p0[0]+=fr1*rb.resval.rpoint[0];
					p0[2]=p1[2];
					}
				}
			vertno=0;
			got1=0;
			for (;;)
				{
				if (vertno>=lwpl->ret_90())
					break;//don't worry about closure.  This loop for on-vtx grips only
				lwpl->get_10(p3,vertno);
				p3[2]+=thick;//always 0.0 if it's not 2D
				if (icadPointEqual(p0,p3))
					{

					// Because we need to find the lwpl to move set the Undo savelp here.
					SDS_CopyEntLink(&savelp,lwpl);

					if (SDS_dragobject(-1,dragmode,(sds_real *)ename,lwpl->ret_10(vertno),
						thick,ResourceString(IDC_GRIPS_35, "\nNew location of vertex: "),NULL,p1,NULL)!=RTNORM)
						{
						lwpl->set_10(p0,vertno);
						goto out;
						}
					sds_trans(p1,&rbucs,&rbent,0,p1);
					p1[2]-=thick;
					lwpl->set_10(p1,vertno);
					got1=1;
					break;
					}
				if (!got1)
					vertno++;
				//check the grip for the current node...
				}
			if (got1)
				break;
			//if we got this far, point wasn't on a vtx, but was midpt on an arc segment

			vertno=0;
			sds_point startpt,cc,arcdata;
			sds_real dist;

			if (flags & IC_PLINE_CLOSED)
				lwpl->get_10(startpt,vertno);

			for (; vertno<lwpl->ret_90(); vertno++)
				{
				lwpl->get_42(&bulge,vertno);
				if (icadRealEqual(bulge,0.0))
					continue;
				//now we're on a curved segment, so check if it's the one
				//setup the pointer to the next node or break if end of open pline
				if (vertno>=lwpl->ret_90()-1)
					{
					if (flags & IC_PLINE_CLOSED)
						ic_ptcpy(p1,startpt);
					else break;
					}
				else
					lwpl->get_10(p1,vertno+1);
				lwpl->get_10(p4,vertno);
				p4[2]+=thick;
				p1[2]+=thick;
				p3[0]=fabs(bulge)*cmd_dist2d(p4,p1)*IC_ZRO;
				if (0==ic_ptsegdist(p0,p4,p1,bulge,&dist) && dist<p3[0])
					{
					got1=1;
					sds_redraw(ename,IC_REDRAW_DRAW);

					// Because we need to find the lwpl to move set the Undo savelp here.
					SDS_CopyEntLink(&savelp,lwpl);

					sds_trans(p1,&rbent,&rbucs,0,p1);
					sds_trans(p2,&rbent,&rbucs,0,p2);
					sds_trans(p4,&rbent,&rbucs,0,p4);
					if (SDS_dragobject(28,dragmode,p4,p1,thick,ResourceString(IDC_GRIPS_36, "\nNew point along vertex: "),NULL,p2,NULL)!=RTNORM)
						goto out;
					sds_trans(p1,&rbucs,&rbent,0,p1);
					sds_trans(p2,&rbucs,&rbent,0,p2);
					sds_trans(p4,&rbucs,&rbent,0,p4);
					sds_redraw(ename,IC_REDRAW_UNDRAW);
					if (0==ic_3pt2arc(p4,p2,p1,cc,&arcdata[0],&arcdata[1],&arcdata[2]))
						{
						ic_normang(&arcdata[1],&arcdata[2]);
						if (fabs(sds_angle(cc,p4)-arcdata[1]) < IC_ZRO)
							//starting angle is at beginning of segment, seg ccw
							lwpl->set_42(tan((arcdata[2]-arcdata[1])/4.0),vertno);
						else
							lwpl->set_42(-tan((arcdata[2]-arcdata[1])/4.0),vertno);
						break;
						}
					}
				}
			if (rbent.restype==RT3DPOINT && !got1 && icadRealEqual(thick,0.0))
				{
				lwpl->get_39(&thick);
				if (fabs(thick)<IC_ZRO)
					break;
				goto redolwpoly;
				}
			}
			break;
		default:
			res = RTNORM;
			goto out;
			break;
		}
	if (elp->ret_type()==DB_DIMENSION)
		{
		((db_handitem *)ename[0])->set_2((db_handitem *)NULL);
		cmd_makedimension((db_handitem *)ename[0],(db_drawing *)ename[1],0);
		}
	sds_entupd(ename);

	// Set the drawing modified flag.
	SDS_DBModified( IC_DBMOD_ENTITIES_MODIFIED);

	// Set the Undo.
//4M Item:70->
// Undoing grip editing used to lose the grip point that has been edited
	if (elp->ret_type()==DB_POLYLINE || elp->ret_type()==DB_VERTEX)
	{
		db_handitem * newelp=(db_handitem *)ename[0];
		if (db_CompEntLinks(&chglst,polyline,newelp)==RTNORM && chglst)
		{
			SDS_SetUndo(IC_UNDO_CHG_ENT_TAB_CLS,(void *)newelp,(void *)chglst,NULL,SDS_CURDWG);
			// EBATECH(CNBR) #78643 Grip edit(move) don't call call back.
			SDS_CallUserCallbackFunc(SDS_CBENTMOD,(void *)newelp,NULL,NULL);
		}
	}
	else
	{
//<-4M Item:70
		if (db_CompEntLinks(&chglst,savelp,elp)==RTNORM && chglst)
		{
			SDS_SetUndo(IC_UNDO_CHG_ENT_TAB_CLS,(void *)elp,(void *)chglst,NULL,SDS_CURDWG);
			// EBATECH(CNBR) #78643 Grip edit(move) don't call call back.
			if(!SDS_AtDragMulti)
				SDS_CallUserCallbackFunc(SDS_CBENTMOD,(void *)elp,NULL,NULL);
		}
	}
	// Update the reactors appropriately
	// if type is a polyline, don't update until the vertices have been appropriately modified.

	if (elp->ret_type()==DB_POLYLINE || elp->ret_type()==DB_VERTEX)
	{
		if (polyline)
		{
			cmd_updateReactors(polyline);
		}
	}
	else if (bSkipReactorUpdation == FALSE)
		cmd_updateReactors(elp);

	res = RTNORM;
out:
	if (SDS_ElevationChanged)
	{
		rb.restype=RTREAL;
		rb.resval.rreal=curelev;
		//do NOT use quick mode here
		sds_setvar("ELEVATION",&rb);//this does an initview
		SDS_ElevationChanged=FALSE;
	}
	SDS_AtNodeDrag=0;
	gr_freedisplayobjectll( SDS_NodeDragEntDispObj );
	SDS_NodeDragEntDispObj = NULL;
	delete savelp;




	if( res == RTNORM )
	{
		SDS_AddGripNodes(pDoc,ename,-1);//remove old nodes from llist
		SDS_AddGripNodes(pDoc,ename,1);
//4M Item:28->
        SDS_DrawGripNodes(pDoc);
//<-4M Item:28
    }
	else
	{
		sds_entupd(ename);
//4M Item:71->
/*
      SDS_FreeNodeList(SDS_CURDOC);
		sds_ssfree(SDS_CURDOC->m_pGripSelection);
*/
   	sds_redraw(ename,IC_REDRAW_HILITE);
      SDS_AddGripNodes(SDS_CURDOC,ename,2);
//4M Item:28->
      SDS_DrawGripNodes(SDS_CURDOC);
//<-4M Item:28
//<-4M Item:71
    }

	return res;
}

int SDS_AddOneNode(CIcadDoc  *pDoc,sds_name ename, sds_point pt, int addmode)
	{
	//expects point pt to be in UCS
    //addmode=2 to paint new node, but NOT add to selected ents. (sssetfirst ss1)
	//addmode=1 to paint in new node
	//addmode=0 to paint out node
//4M Item:28->
/*
    struct resbuf rb;
//	sds_real fr1;
    sds_point p0;//,p1;
	CDC *cdc;
	HGDIOBJ SavObj=NULL;
	COLORREF BkCol;
	int pix[2],pbox;
*/
//<-4M Item:28


	if (pDoc==NULL)
		return(RTERROR);

	db_drawing *flp=(db_drawing *)ename[1];
	if (flp==NULL)
		return(RTERROR);

//4M Item:28->
/*
//<-4M Item:28
    struct gr_view *view;
	CIcadView *pView;

	SDS_getvar(NULL,DB_QGRIPSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	pbox=rb.resval.rint*2;

    POSITION pos=pDoc->GetFirstViewPosition();

	while (pos!=NULL)
		{
		pView=(CIcadView *)pDoc->GetNextView(pos);
		if (pView==NULL)
			return(RTERROR);
		ASSERT_KINDOF(CIcadView, pView);

		view=pView->m_pCurDwgView;
		if (view==NULL)
			return(RTERROR);

//      fr1=view->viewsize;             /*D.G.// Not
//		fr1/=pView->m_iWinY;			// needed
//		fr1*=(pbox/2.0);				// now.

		gr_ucs2rp(pt,p0,view);
//      p1[0]=p0[0]-fr1; p1[1]=p0[1]+fr1; p1[2]=p0[2];  /*D.G.// Not needed now.

		cdc=pView->GetDC();

		if (addmode>0)
			SDS_getvar(NULL,DB_QGRIPCOLOR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		else
			rb.resval.rint=256;  // Set backbround color.

		if (pView->m_pClipRectDC)
			cdc->IntersectClipRect(pView->m_pClipRectDC);

		SetROP2(cdc->m_hDC,R2_COPYPEN);

		pView->SetDcColorMem(rb.resval.rint,1,0,R2_COPYPEN);

		CPalette *oldpal=cdc->SelectPalette(SDS_CMainWindow->m_pPalette,TRUE);

//      gr_rp2pix(view,p1[0],p1[1],&pix[0],&pix[1]);        /*D.G.
		gr_rp2pix(view, p0[0], p0[1], &pix[0], &pix[1]);	// Use
		pix[0] -= pbox * 0.5;								// this
		pix[1] -= pbox * 0.5;								// way.

		// Add the grips to the screen as well as the memDC.
		cdc->FillSolidRect(pix[0],pix[1],pbox,pbox,SDS_BrushColorFromACADColor(rb.resval.rint));

		BkCol=pView->GetFrameBufferCDC()->GetBkColor();
		pView->GetFrameBufferCDC()->FillSolidRect(pix[0],pix[1],pbox,pbox,SDS_BrushColorFromACADColor(rb.resval.rint));
		pView->GetFrameBufferCDC()->SetBkColor(BkCol);
//		cdc->SelectPalette(oldpal,TRUE);
		pView->ReleaseDC(cdc);
	}

//4M Item:28->
*/
   if (addmode>0)
      pDoc->m_gripsToDraw.Add(pt);
   else if (addmode==0)
      pDoc->m_gripsToUndraw.Add(pt);
//<-4M Item:28
    if (addmode==1)
		{
//4M Item:25->
// Replacement of SDS_NodeList with SDS_NodeList_new
//<-4M Item:25
        pDoc->m_nodeList.add( ename, pt );
		}
	return(RTNORM);
	}

//4M Item:28->
void SDS_DrawGripNodes(CIcadDoc  *pDoc, CDC *cdc, CIcadView *pView, gr_view *view, sds_real const & fr1, int const &pbox, int addmode)
{
   struct resbuf rb;
   if (addmode>0)
		SDS_getvar(NULL,DB_QGRIPCOLOR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	else
		rb.resval.rint = 256;  // Set backbround color.

//	if (pView->m_pClipRectDC)
//		cdc->IntersectClipRect(pView->m_pClipRectDC);

//	SetROP2(cdc->m_hDC,R2_COPYPEN);

	CDrawDevice* pDD = pView->GetDrawDevice();
	pDD->SetDrawMode(RasterConstants::MODE_COPY);
	pDD->SetLineColorACAD(rb.resval.rint, 1);

//	CPalette *oldpal=cdc->SelectPalette(SDS_CMainWindow->m_pPalette,TRUE);

   sds_point p0,p1;
   int pix[2];
   if (addmode > 0)
   {
	   for (int i=0;i<pDoc->m_gripsToDraw.GetNoGrips();i++)
	   {
		   sds_point pt;
		   pDoc->m_gripsToDraw.Get(i+1,pt);
		   gr_ucs2rp(pt,p0,view);
		   p1[0]=p0[0]-fr1; p1[1]=p0[1]+fr1; p1[2]=p0[2];
		   //p2[0]=p0[0]+fr1; p2[1]=p0[1]-fr1; p2[2]=p0[2];
		   gr_rp2pix(view,p1[0],p1[1],&pix[0],&pix[1]);
		   RECT r1; r1.left=pix[0]; r1.bottom=pix[1]+pbox; r1.right=pix[0]+pbox; r1.top=pix[1];
		   pDD->fillRect(r1, rb.resval.rint);
		   //cdc->FrameRect(&r1,&br1);
		   //pView->GetFrameBufferCDC()->FrameRect(&r1,&br1);
	   }
   }
   else
   {
	   for (int i=0;i<pDoc->m_gripsToUndraw.GetNoGrips();i++){
           sds_point pt;
		   pDoc->m_gripsToUndraw.Get(i+1,pt);
		   gr_ucs2rp(pt,p0,view);
		   p1[0]=p0[0]-fr1; p1[1]=p0[1]+fr1; p1[2]=p0[2];
		   //p2[0]=p0[0]+fr1; p2[1]=p0[1]-fr1; p2[2]=p0[2];
		   gr_rp2pix(view,p1[0],p1[1],&pix[0],&pix[1]);
		   RECT r1; r1.left=pix[0]; r1.bottom=pix[1]+pbox; r1.right=pix[0]+pbox; r1.top=pix[1];
		   //cdc->FrameRect(&r1,&br1);
		   pDD->fillRect(r1, rb.resval.rint);
	   }
   }
   pView->ReleaseDrawDevice(pDD);
}

int SDS_DrawGripNodes(CIcadDoc  *pDoc)
{
	if(!pDoc)
		return(RTERROR);

	int	NoGripsToDraw = pDoc->m_gripsToDraw.GetNoGrips(),
		NoGripsToUndraw = pDoc->m_gripsToUndraw.GetNoGrips();

	if(!NoGripsToDraw && !NoGripsToUndraw)
		return RTERROR;

	gr_view*	view;
	CIcadView*	pView;
	resbuf		rb;

	SDS_getvar(NULL, DB_QGRIPSIZE, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
	int	pbox = rb.resval.rint+rb.resval.rint;

	POSITION	pos = pDoc->GetFirstViewPosition();
	while(pos != NULL)
	{
		pView = (CIcadView*)pDoc->GetNextView(pos);
		if(!pView)
			return RTERROR;
		ASSERT_KINDOF(CIcadView, pView);

		view = pView->m_pCurDwgView;
		if(!view)
			return RTERROR;
		sds_real	fr1 = (view->viewsize * pbox) / (pView->m_iWinY + pView->m_iWinY);

		CDC*	cdc = pView->GetDC();
		/*DG - 12.6.2003*/// First, undraw required grips, then draw other ones,
		// so congruent grips don't conflict.
		SDS_DrawGripNodes(pDoc,cdc,pView, view, fr1, pbox, 0);
		SDS_DrawGripNodes(pDoc,cdc,pView, view, fr1, pbox, 1);
		SDS_BitBlt2Scr(0);
		pView->ReleaseDC(cdc);
	}
	pDoc->m_gripsToDraw.Reset();
	pDoc->m_gripsToUndraw.Reset();

	return RTNORM;
}
//<-4M Item:28

void SDS_FreeNodeList(CIcadDoc  *pDoc)
	{
	sds_point pt;
	sds_name ename;

	if (pDoc==NULL) return;

	pDoc->m_nodeList.begin();
	while( pDoc->m_nodeList.getPair( ename, pt ) )
	{
		SDS_AddOneNode( pDoc, ename, pt, 0 );
	}
//4M Item:28->
   SDS_DrawGripNodes(pDoc);
//<-4M Item:28
    pDoc->m_nodeList.removeAll();

	long fl1=0L;
	while(RTNORM==sds_ssname(pDoc->m_pGripSelection,fl1,ename))
		{
		sds_redraw(ename,IC_REDRAW_DRAW);
		++fl1;
		}
    return;
	}



