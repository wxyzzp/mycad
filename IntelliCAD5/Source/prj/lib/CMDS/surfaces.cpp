/* F:\DEV\PRJ\LIB\CMDS\SURFACES.CPP
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
#include "IcadView.h"
#include "Modeler.h"
#include "GeoData.h"
#include "CmdQueryTools.h"

#include <algorithm>
using namespace std;

#if defined(SDS_NEWDB)
	#define gra_displayobject gr_displayobject
	#define gra_view      gr_view
	#define dwg_entlink	  db_handitem
	#define dwg_filelink  db_drawing
	#define gra_rp2ucs    gr_rp2ucs
	#define gra_ucs2rp    gr_ucs2rp
	#define gra_rp2pix    gr_rp2pix
	#define gra_pix2rp    gr_pix2rp
	#define gra_twist4ucs gr_twist4ucs
	#define gra_initview  gr_initview
#endif

#define  ARC        1
#define  LINE       2
#define  OPLINE2D   3
#define  OPLINE3D   4
#define  POINT      5
#define  CIRCLE     6
#define  ELLIPSE    7
#define  CPLINE2D   8
#define  CPLINE3D   9
#define  PLINE2D    10
#define  PLINE3D    11

static
bool isKindOf(db_handitem* pe, ...);

struct cmd_rev_pts 
	{
    sds_point pt;
    struct cmd_rev_pts *nextpt;
    struct cmd_rev_pts *prevpt;
	};


short cmd_coons(sds_name ename1,sds_name ename2,sds_name ename3,sds_name ename4,short ecsflag1,short ecsflag2,short ecsflag3,short ecsflag4)
	{

    long i,j,q;
    RT	rc,ret;
	short	mtab,ntab,flag2,rflag2,rflag3,rflag4,flag4,tmpf2,tmpf3,tmpf4;//,grcolor;
    struct resbuf setgetrb,*pline=NULL,*ptll1=NULL,*ptll2=NULL,*ptll3=NULL,*ptll4=NULL,*ent=NULL;
    struct resbuf *tmprb=NULL,wcs,ecs;
    sds_point *grid1=NULL,*grid2=NULL,endpts1[2],endpts2[2],endpts3[2],endpts4[2],tecs2,tecs3,tecs4;
    sds_point *segpts1=NULL,*segpts2=NULL,*segpts3=NULL,*segpts4=NULL,ecspt1,ecspt2,ecspt3,ecspt4;
    sds_point *pp1=NULL,ptn,pt0={0.0,0.0,0.0},tmppts2[2],tmppts3[2],tmppts4[2];//,grpt;
    sds_real dx,dy,dz;
    sds_name tmpname2,tmpname3,tmpname4;
	tmpname2[0] = 0;
	tmpname2[1] = 0;
	tmpname3[0] = 0;
	tmpname3[1] = 0;
	tmpname4[0] = 0;
	tmpname4[1] = 0;


    wcs.restype=RTSHORT;
    wcs.resval.rint=0;
    ecs.restype=RT3DPOINT;
    ecs.resval.rpoint[0]=0.0;
    ecs.resval.rpoint[1]=0.0;
    ecs.resval.rpoint[2]=1.0;
    
    setgetrb.rbnext=NULL;

	//Get color for grdraw
//    if ((ret=SDS_getvar(NULL,DB_QCECOLOR,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM) return(ret);
//    grcolor=setgetrb.resval.rint;

    //get the var that holds the number of surfaces to draw
    if ((ret=SDS_getvar(NULL,DB_QSURFTAB1,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM) {rc=ret;goto bail;} 
    mtab=setgetrb.resval.rint;

    //get the var that holds the number of surfaces to draw
    if ((ret=SDS_getvar(NULL,DB_QSURFTAB2,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM) {rc=ret;goto bail;} 
    ntab=setgetrb.resval.rint;

	//make an array #1 that will contain all of the points in the N direction
	if ((grid1= new sds_point [(ntab+1)*(mtab+1)] )==NULL)
		{
        cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);            
        rc=RTERROR; 
		goto bail;                                    
		}
	memset(grid1,0,sizeof(sds_point)*(ntab+1)*(mtab+1));
	//make an array #2 that will contain all of the points M direction
	if ((grid2= new sds_point [(ntab+1)*(mtab+1)] )==NULL)
		{
        cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);            
        rc=RTERROR;
		goto bail;                                    
		}
	memset(grid2,0,sizeof(sds_point)*(ntab+1)*(mtab+1));
    //get start and end point of selected entity
	if (db_getendpts(ename1,endpts1)) {rc=RTERROR;goto bail;}
	if (db_getendpts(ename2,endpts2)) {rc=RTERROR;goto bail;}
	if (db_getendpts(ename3,endpts3)) {rc=RTERROR;goto bail;}
	if (db_getendpts(ename4,endpts4)) {rc=RTERROR;goto bail;}

    //convert endpts to wcs
    if (ecsflag1) 
		{
        if ((ent=sds_entget(ename1))==NULL) {rc=RTERROR;goto bail;}
        ic_assoc(ent,210);
        ic_ptcpy(ecspt1,ic_rbassoc->resval.rpoint);
        ic_ptcpy(ecs.resval.rpoint,ic_rbassoc->resval.rpoint);
        sds_trans(endpts1[0],&ecs,&wcs,0,endpts1[0]);
        sds_trans(endpts1[1],&ecs,&wcs,0,endpts1[1]);
		}
    if (ecsflag2) 
		{
        if ((ent=sds_entget(ename2))==NULL) {rc=RTERROR;goto bail;}
        ic_assoc(ent,210);
        ic_ptcpy(ecspt2,ic_rbassoc->resval.rpoint);
        ic_ptcpy(ecs.resval.rpoint,ic_rbassoc->resval.rpoint);
        sds_trans(endpts2[0],&ecs,&wcs,0,endpts2[0]);
        sds_trans(endpts2[1],&ecs,&wcs,0,endpts2[1]);
		}
    if (ecsflag3) 
		{
        if ((ent=sds_entget(ename3))==NULL) {rc=RTERROR;goto bail;}
        ic_assoc(ent,210);
        ic_ptcpy(ecspt3,ic_rbassoc->resval.rpoint);
        ic_ptcpy(ecs.resval.rpoint,ic_rbassoc->resval.rpoint);
        sds_trans(endpts3[0],&ecs,&wcs,0,endpts3[0]);
        sds_trans(endpts3[1],&ecs,&wcs,0,endpts3[1]);
		}
    if (ecsflag4) 
		{
        if ((ent=sds_entget(ename4))==NULL) {rc=RTERROR;goto bail;}
        ic_assoc(ent,210);
        ic_ptcpy(ecspt4,ic_rbassoc->resval.rpoint);
        ic_ptcpy(ecs.resval.rpoint,ic_rbassoc->resval.rpoint);
        sds_trans(endpts4[0],&ecs,&wcs,0,endpts4[0]);
        sds_trans(endpts4[1],&ecs,&wcs,0,endpts4[1]);
		}
    
    //find out which end of which entity is connected to the end point of the first entity 
    if (icadPointEqual(endpts1[1],endpts2[0])) 
		{
        ic_ptcpy(tmppts2[0],endpts2[0]);   
        ic_ptcpy(tmppts2[1],endpts2[1]);   
        flag2=2;
        rflag2=0;
        ic_ptcpy(tecs2,ecspt2);
        tmpf2=ecsflag2;
        ic_encpy(tmpname2,ename2);
		}
	else if (icadPointEqual(endpts1[1],endpts2[1])) 
		{
        ic_ptcpy(tmppts2[0],endpts2[1]);    
        ic_ptcpy(tmppts2[1],endpts2[0]); 
        flag2=2;
        rflag2=1;
        ic_ptcpy(tecs2,ecspt2);
        tmpf2=ecsflag2;
        ic_encpy(tmpname2,ename2);
		}
	else if (icadPointEqual(endpts1[1],endpts3[0])) 
		{
        ic_ptcpy(tmppts2[0],endpts3[0]);     
        ic_ptcpy(tmppts2[1],endpts3[1]);     
        flag2=3;
        rflag2=0;
        ic_ptcpy(tecs2,ecspt3);
        tmpf2=ecsflag3;
        ic_encpy(tmpname2,ename3);
		}
	else if (icadPointEqual(endpts1[1],endpts3[1])) 
		{
        ic_ptcpy(tmppts2[0],endpts3[1]);      
        ic_ptcpy(tmppts2[1],endpts3[0]);      
        flag2=3;
        rflag2=1;
        ic_ptcpy(tecs2,ecspt3);
        tmpf2=ecsflag3;
        ic_encpy(tmpname2,ename3);
		}
	else if (icadPointEqual(endpts1[1],endpts4[0])) 
		{
        ic_ptcpy(tmppts2[0],endpts4[0]); 
        ic_ptcpy(tmppts2[1],endpts4[1]); 
        flag2=4;
        rflag2=0;
        ic_ptcpy(tecs2,ecspt4);
        tmpf2=ecsflag4;
        ic_encpy(tmpname2,ename4);
		}
	else if (icadPointEqual(endpts1[1],endpts4[1])) 
		{ 
        ic_ptcpy(tmppts2[0],endpts4[1]);                                                        
        ic_ptcpy(tmppts2[1],endpts4[0]);                                                        
        flag2=4;
        rflag2=1;
        ic_ptcpy(tecs2,ecspt4);
        tmpf2=ecsflag4;
        ic_encpy(tmpname2,ename4);
	    }
	else
		{
        cmd_ErrorPrompt(CMD_ERR_NONCOINCENDPTS,0);
        goto bail;
		}

    //find out which end of which entity is connected to the start point of the first entity 
    if (icadPointEqual(endpts1[0],endpts2[0])) 
		{
        ic_ptcpy(tmppts4[0],endpts2[0]);   
        ic_ptcpy(tmppts4[1],endpts2[1]);   
        flag4=2;
        rflag4=0;
        ic_ptcpy(tecs4,ecspt2);
        tmpf4=ecsflag2;
        ic_encpy(tmpname4,ename2);
		}
	else if (icadPointEqual(endpts1[0],endpts2[1])) 
		{
        ic_ptcpy(tmppts4[0],endpts2[1]);    
        ic_ptcpy(tmppts4[1],endpts2[0]); 
        flag4=2;
        rflag4=1;
        ic_ptcpy(tecs4,ecspt2);
        tmpf4=ecsflag2;
        ic_encpy(tmpname4,ename2);
		}
	else if (icadPointEqual(endpts1[0],endpts3[0])) 
		{
        ic_ptcpy(tmppts4[0],endpts3[0]);     
        ic_ptcpy(tmppts4[1],endpts3[1]);     
        flag4=3;
        rflag4=0;
        ic_ptcpy(tecs4,ecspt3);
        tmpf4=ecsflag3;
        ic_encpy(tmpname4,ename3);
		}
	else if (icadPointEqual(endpts1[0],endpts3[1])) 
		{
        ic_ptcpy(tmppts4[0],endpts3[1]);      
        ic_ptcpy(tmppts4[1],endpts3[0]);      
        flag4=3;
        rflag4=1;
        ic_ptcpy(tecs4,ecspt3);
        tmpf4=ecsflag3;
        ic_encpy(tmpname4,ename3);
		}
	else if (icadPointEqual(endpts1[0],endpts4[0])) 
		{
        ic_ptcpy(tmppts4[0],endpts4[0]); 
        ic_ptcpy(tmppts4[1],endpts4[1]); 
        flag4=4;
        rflag4=0;
        ic_ptcpy(tecs4,ecspt4);
        tmpf4=ecsflag4;
        ic_encpy(tmpname4,ename4);
		}
	else if (icadPointEqual(endpts1[0],endpts4[1])) 
		{  
        ic_ptcpy(tmppts4[0],endpts4[1]);                                                        
        ic_ptcpy(tmppts4[1],endpts4[0]);                                                        
        flag4=4;
        rflag4=1;
        ic_ptcpy(tecs4,ecspt4);
        tmpf4=ecsflag4;
        ic_encpy(tmpname4,ename4);
		}
	else
		{
        cmd_ErrorPrompt(CMD_ERR_NONCOINCENDPTS,0);
        goto bail;
		}

    //now we have the first,second and fourth segment
    //the only thing left to find is the third segment
    //this is done by elimination
    if ((flag2==2 && flag4==3) || (flag2==3 && flag4==2))
		{
        if (icadPointEqual(endpts4[0],tmppts4[1]))
			{
            ic_ptcpy(tmppts3[0],endpts4[0]);    
            ic_ptcpy(tmppts3[1],endpts4[1]);    
            rflag3=0;
			}
		else if (icadPointEqual(endpts4[0],tmppts2[1]))
			{
            ic_ptcpy(tmppts3[0],endpts4[1]); 
            ic_ptcpy(tmppts3[1],endpts4[0]); 
            rflag3=1;
			}
		else
			{
            cmd_ErrorPrompt(CMD_ERR_NONCOINCENDPTS,0);
            goto bail;
			} 
        ic_ptcpy(tecs3,ecspt4);
        tmpf3=ecsflag4;
        ic_encpy(tmpname3,ename4);
		}
	else if ((flag2==3 && flag4==4) || (flag2==4 && flag4==3))
		{
        if (icadPointEqual(endpts2[0],tmppts4[1]))
			{
            ic_ptcpy(tmppts3[0],endpts2[0]);                    
            ic_ptcpy(tmppts3[1],endpts2[1]);                    
            rflag3=0;
			}
		else if (icadPointEqual(endpts2[0],tmppts2[1]))
			{
            ic_ptcpy(tmppts3[0],endpts2[1]);                    
            ic_ptcpy(tmppts3[1],endpts2[0]);                    
            rflag3=1;
			}
		else
			{
            cmd_ErrorPrompt(CMD_ERR_NONCOINCENDPTS,0);
            goto bail;
			} 
        ic_ptcpy(tecs3,ecspt2);
        tmpf3=ecsflag2;
        ic_encpy(tmpname3,ename2);
		}
	else if ((flag2==2 && flag4==4) || (flag2==4 && flag4==2))
		{
        if (icadPointEqual(endpts3[0],tmppts4[1]))
			{
            ic_ptcpy(tmppts3[0],endpts3[0]);                    
            ic_ptcpy(tmppts3[1],endpts3[1]);                    
            rflag3=0;
			}
		else if (icadPointEqual(endpts3[0],tmppts2[1]))
			{
            ic_ptcpy(tmppts3[0],endpts3[1]);                    
            ic_ptcpy(tmppts3[1],endpts3[0]);                    
            rflag3=1;
			}
		else
			{
            cmd_ErrorPrompt(CMD_ERR_NONCOINCENDPTS,0);
            goto bail;
			}
        ic_ptcpy(tecs3,ecspt3);
        tmpf3=ecsflag3;
        ic_encpy(tmpname3,ename3);
		}

	//set the names and endpts back in the proper order
	ic_ptcpy(endpts2[0],tmppts2[0]);
	ic_ptcpy(endpts2[1],tmppts2[1]);
	ic_encpy(ename2,tmpname2);
	ic_ptcpy(endpts3[0],tmppts3[0]);    
	ic_ptcpy(endpts3[1],tmppts3[1]);    
	ic_encpy(ename3,tmpname3);         
	ic_ptcpy(endpts4[0],tmppts4[0]);    
	ic_ptcpy(endpts4[1],tmppts4[1]);    
	ic_encpy(ename4,tmpname4);         
    ecsflag2=tmpf2;
    ecsflag3=tmpf3;
    ecsflag4=tmpf4;	
    ic_ptcpy(ecspt2,tecs2);
    ic_ptcpy(ecspt3,tecs3);
    ic_ptcpy(ecspt4,tecs4);

	//get the points along the edges
    if ((ret=ic_divmeas(ename1,mtab,NULL,1.0,0.0,0,0,&ptll1))!=0) {rc=ret;goto bail;} 
    if ((ret=ic_divmeas(ename2,ntab,NULL,1.0,0.0,0,0,&ptll2))!=0) {rc=ret;goto bail;} 
    if ((ret=ic_divmeas(ename3,mtab,NULL,1.0,0.0,0,0,&ptll3))!=0) {rc=ret;goto bail;} 
    if ((ret=ic_divmeas(ename4,ntab,NULL,1.0,0.0,0,0,&ptll4))!=0) {rc=ret;goto bail;} 
    
    //the start and end points have been changed to protect the inocent
    //now we will have to reverse the divide and measured points 
    if (rflag2==1)
		{
        if ((ret=cmd_reverse_link_list(&ptll2))!=RTNORM) {rc=ret;goto bail;}
		}
    if (rflag3==1)
		{
        if ((ret=cmd_reverse_link_list(&ptll3))!=RTNORM) {rc=ret;goto bail;}
		}
    if (rflag4==1)
		{
        if ((ret=cmd_reverse_link_list(&ptll4))!=RTNORM) {rc=ret;goto bail;}
		}
    
    //this will move the start point,linked list and end point into an arrary
	//this is for segment1
    //count points for segs 1,3
    for (q=2,tmprb=ptll1;tmprb!=NULL;tmprb=tmprb->rbnext,q++);
    //alloc space for points
    //and copy points for seg1
    if (ecsflag1)
		ic_ptcpy(ecs.resval.rpoint,ecspt1);
    if ((segpts1= new sds_point [q])==NULL) 
		{
		cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
		rc=RTERROR; 
		goto bail;
		}
    ic_ptcpy(segpts1[0],endpts1[0]);                         
    for (i=1,tmprb=ptll1;tmprb!=NULL;i++)
		{
        ic_ptcpy(segpts1[i],tmprb->resval.rpoint);
        if (ecsflag1)
			sds_trans(segpts1[i],&ecs,&wcs,0,segpts1[i]);
        tmprb=tmprb->rbnext;
		}
    ic_ptcpy(segpts1[i],endpts1[1]); 
    //alloc space for points
    //and copy points for seg3
    if (ecsflag3) 
		ic_ptcpy(ecs.resval.rpoint,ecspt3);
    if ((segpts3= new sds_point [q])==NULL) {cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);rc=RTERROR; goto bail;}
    ic_ptcpy(segpts3[0],endpts3[0]);                         
    for (i=1,tmprb=ptll3;tmprb!=NULL;i++)
		{
        ic_ptcpy(segpts3[i],tmprb->resval.rpoint);
        if (ecsflag3) 
			sds_trans(segpts3[i],&ecs,&wcs,0,segpts3[i]);
        tmprb=tmprb->rbnext;
		}
    ic_ptcpy(segpts3[i],endpts3[1]); 

    //count points for segs 2,4
    for (q=2,tmprb=ptll2;tmprb!=NULL;tmprb=tmprb->rbnext,q++);
    //alloc space for points
    //and copy points for seg2
    if (ecsflag2)
		ic_ptcpy(ecs.resval.rpoint,ecspt2);
    if ((segpts2= new sds_point [q])==NULL) {cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);rc=RTERROR; goto bail;}
    ic_ptcpy(segpts2[0],endpts2[0]);                         
    for (i=1,tmprb=ptll2;tmprb!=NULL;i++)
		{
        ic_ptcpy(segpts2[i],tmprb->resval.rpoint);
        if (ecsflag2) 
			sds_trans(segpts2[i],&ecs,&wcs,0,segpts2[i]);
        tmprb=tmprb->rbnext;
		}
    ic_ptcpy(segpts2[i],endpts2[1]); 
    //alloc space for points
    //and copy points for seg4
    if (ecsflag4)
		ic_ptcpy(ecs.resval.rpoint,ecspt4);
    if ((segpts4= new sds_point [q])==NULL) {cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);rc=RTERROR; goto bail;}
    ic_ptcpy(segpts4[0],endpts4[0]);                         
    for (i=1,tmprb=ptll4;tmprb!=NULL;i++)
		{
        ic_ptcpy(segpts4[i],tmprb->resval.rpoint);
        if (ecsflag4)
			sds_trans(segpts4[i],&ecs,&wcs,0,segpts4[i]);
        tmprb=tmprb->rbnext;
		}
    ic_ptcpy(segpts4[i],endpts4[1]); 

    //now that we are done with the ptll resbufs free them
    //IC_RELRB(ptll1);IC_RELRB(ptll2);IC_RELRB(ptll3);IC_RELRB(ptll4);
        
    //fill the first grid
    for (i=0,q=0;i<(mtab+1);i++)
		{
        for (j=0;j<(ntab+1);j++,q++)
			{
            dx=(segpts2[j][0]-segpts4[j][0])/mtab;
            dy=(segpts2[j][1]-segpts4[j][1])/mtab;
            dz=(segpts2[j][2]-segpts4[j][2])/mtab;
            ptn[0]=segpts4[j][0]+i*dx;
            ptn[1]=segpts4[j][1]+i*dy; 
            ptn[2]=segpts4[j][2]+i*dz; 
            ic_ptcpy(grid1[q],ptn);
			}
		}

    //fill the second grid
    for (i=0,q=0;i<(mtab+1);i++)
		{
        for (j=0;j<(ntab+1);j++,q++)
			{
            dx=(segpts3[i][0]-segpts1[i][0])/ntab;
            dy=(segpts3[i][1]-segpts1[i][1])/ntab;
            dz=(segpts3[i][2]-segpts1[i][2])/ntab;
            ptn[0]=segpts1[i][0]+j*dx;
            ptn[1]=segpts1[i][1]+j*dy; 
            ptn[2]=segpts1[i][2]+j*dz; 
            ic_ptcpy(grid2[q],ptn);
			}
		}

    //build the pline header
    if ((pline=sds_buildlist(RTDXF0,"POLYLINE"/*DNT*/,
                                66,1,                       
                                10,pt0,  //dummy point
                                70,16,
                                71,mtab+1,
                                72,ntab+1,
                                 0))==NULL) 
		{
        cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);            
        rc=RTERROR; goto bail;                                    
		}                                                           
    if ((ret=sds_entmake(pline))!=RTNORM) {rc=ret;goto bail;} 
    IC_RELRB(pline);

    //build the skelleton vertex
    if ((pline=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,
                                10,pt0,  //dummy point
                                70,64,
                                 0))==NULL)
		{
        cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);            
        rc=RTERROR; goto bail;                                    
		}                                 
    ic_assoc(pline,10);          
	pp1=&ic_rbassoc->resval.rpoint;
    
	// For the Coons formula, see any extbook on geometric modeling,
	// e.g. the chapter "Coons Patches" in G. Farin's book "Curves and 
	// Surfaces for Computer Aided Design".  The points from the 2 
	// blendings of pairs of oposite curves are already stored in 
	//grid1 and grid2, here we add them up, and subtract the blend
	// of the 4 corners, stored in endpts1 and endpts2
	dx = 1.0 / mtab;
	dy = 1.0 / ntab;
    for (i=0, q=0;  i<=mtab;  i++)
		{
		double u = i * dx; 
        for (j=0;  j<=ntab;  j++,q++)
			{
			double v = j * dy;
			for (int k = 0;   k < 3;   k++)
				// Coons patch formula
				(*pp1)[k] = grid1[q][k] + grid2[q][k] - 
							(1 - u) * (1 - v) * endpts1[0][k] - 
							   u    * (1 - v) * endpts1[1][k] - 
							(1 - u) *    v    * endpts3[0][k] - 
							   u    *    v    *	endpts3[1][k];

			// Got the point, add it to the entity
			if ((ret=sds_entmake(pline))!=RTNORM) {rc=ret;goto bail;} 
            }
        }    
    
	  IC_RELRB(pline);

    //build the seqend
    if ((pline=sds_buildlist(RTDXF0,"SEQEND"/*DNT*/,
                                 0))==NULL)
		{                        
        cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
        rc=RTERROR; 
		goto bail;                                           
		}                                                                
    if ((ret=sds_entmake(pline))!=RTNORM) {rc=ret;goto bail;}         

    rc=RTNORM;
bail:
    IC_FREE(segpts1);
	IC_FREE(segpts2);
	IC_FREE(segpts3);
	IC_FREE(segpts4);
    IC_FREE(grid1);
	IC_FREE(grid2);
    IC_RELRB(ptll1);
	IC_RELRB(ptll2);
	IC_RELRB(ptll3);
	IC_RELRB(ptll4);
    IC_RELRB(pline);
    IC_RELRB(ent);
    return(rc);
	}

static
short cmd_edgesurf_acis()
{
    using namespace geodata;

    if (!CModeler::get()->canModify())
        return RTERROR;

    RT rc = RTERROR;
    sds_name enames[4] = { {0,0}, {0,0}, {0,0}, {0,0}};
    db_handitem* items[4];
    vector<curve*> crvs(4);
	ptr_vector_holder<curve> hcrvs(crvs);

	RT ret;
    sds_point pt;
    for (register i = 0; i < 4; i++)
	{
        for (;;)
		{
			switch(i) {
			case 0:
				ret=sds_entsel(ResourceString(IDC_SURFACES__NSELECT_THE_FI_0, "\nSelect the first of four connected linear entities for edge surface: " ),enames[0], pt);
				break;
			case 1:
				ret=sds_entsel(ResourceString(IDC_SURFACES__NSELECT_SECOND_3, "\nSelect second edge: " ), enames[1], pt);
				break;
			case 2:
				ret=sds_entsel(ResourceString(IDC_SURFACES__NSELECT_THIRD__4, "\nSelect third edge: " ), enames[2], pt);
				break;
			case 3:
				ret=sds_entsel(ResourceString(IDC_SURFACES__NSELECT_LAST_E_5, "\nSelect last edge: " ), enames[3], pt);
				break;
			default:
				break;
			}
            if (ret==RTERROR)
				continue;

            if (ret==RTNORM) 
                sds_redraw(enames[i],IC_REDRAW_HILITE);
			else
			{
                rc=ret;
				goto bail;
			}

			items[i] = reinterpret_cast<db_handitem*>(enames[i][0]);
			if (isKindOf(items[i], DB_LINE, DB_ARC, DB_ELLIPSE, DB_CIRCLE, DB_POLYLINE, DB_LWPOLYLINE, DB_SPLINE, -1)) {
				if (!extract(*items[i], crvs[i])) {
					rc = RTERROR;
					goto bail;
				}
				break;
			}
   			sds_redraw(enames[i],IC_REDRAW_UNHILITE);
		}
    }
	{
		CDbAcisEntityData body;
		if (CModeler::get()->createSurfaceCurves(crvs, getCurrColorIndex(), body))
		{
			if (create(body))
				rc = RTNORM;
		}
	}
bail:
    for (register j = 0; j < i; j++)
        if (enames[j][0])
		    sds_redraw(enames[j],IC_REDRAW_UNHILITE);
    return rc;
}

//===============================================================================================

short cmd_edgesurf(void)  
{
    if (cmd_iswindowopen() == RTERROR)
		return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    //<ALM>
    if (CModeler::get()->canModify())
        return cmd_edgesurf_acis();
    //</ALM>
	else
		return cmd_edgesurf_mesh();
}

short cmd_edgesurf_mesh(void)  
{
    if (cmd_iswindowopen() == RTERROR)
		return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    struct resbuf *ent=NULL;
    sds_name ename1,ename2,ename3,ename4;
    sds_point selpt1,selpt2,selpt3,selpt4;
    RT		rc,ret;
	short	flag1,flag2,flag3,flag4,ecsflag1,ecsflag2,ecsflag3,ecsflag4;

	ename1[0]=ename1[1]=0L;
	ename2[0]=ename2[1]=0L;
	ename3[0]=ename3[1]=0L;
	ename4[0]=ename4[1]=0L;
    for (;;)
		{
        if ((ret=sds_entsel(ResourceString(IDC_SURFACES__NSELECT_THE_FI_0, "\nSelect the first of four connected entities for edge surface: " ) ,ename1,selpt1))!=RTNORM) return(ret);
        if ((ent=sds_entget(ename1))==NULL) {rc=RTERROR;goto bail;}
        ic_assoc(ent,0);
        if (strisame(db_hitype2str(DB_LINE),ic_rbassoc->resval.rstring))
			{
            flag1=LINE;           
            ecsflag1=0;
            break;              
			}                       
        else if (strisame("ARC"/*DNT*/ ,ic_rbassoc->resval.rstring))
			{
            flag1=ARC; 
            ecsflag1=1;
            break;
			}
        else if (strisame("POLYLINE"/*DNT*/  ,ic_rbassoc->resval.rstring))
			{
            ic_assoc(ent,70);
            if (ic_rbassoc->resval.rint & IC_PLINE_CLOSED)
				{
	            if (ic_rbassoc->resval.rint & IC_PLINE_3DPLINE)
					{
                    flag1=CPLINE3D; //3D
                    ecsflag1=0;
					}
				else
					{
                    flag1=CPLINE2D; //2D
                    ecsflag1=1;
					}   
				}
			else
				{
	            if (ic_rbassoc->resval.rint & IC_PLINE_3DPLINE)
					{
                    flag1=OPLINE3D; //3D            
                    ecsflag1=0;
					}
				else
					{
                    flag1=OPLINE2D; //2D            
                    ecsflag1=1;
					}                                  
				}
            break;
			}                
        else if (strisame("LWPOLYLINE"/*DNT*/  ,ic_rbassoc->resval.rstring))
			{
            ic_assoc(ent,70);
            if (ic_rbassoc->resval.rint & IC_PLINE_CLOSED)
				{
                flag1=CPLINE2D; //2D
                ecsflag1=1;
				}
			else
				{
                flag1=OPLINE2D; //2D            
                ecsflag1=1;
				}
            break;
			}
        else if (strisame("ELLIPSE"/*DNT*/  ,ic_rbassoc->resval.rstring) || 
			strisame("SPLINE"/*DNT*/  ,ic_rbassoc->resval.rstring))
			{          
            ecsflag1=0;
            break;
			}
		cmd_ErrorPrompt(CMD_ERR_ENTTYPE,0);
        IC_RELRB(ent);
		}          
    sds_redraw(ename1,IC_REDRAW_HILITE);
    IC_RELRB(ent);
    
    for (;;) 
		{
        if ((ret=sds_entsel(ResourceString(IDC_SURFACES__NSELECT_SECOND_3, "\nSelect second edge: " ) ,ename2,selpt2))!=RTNORM)
			return(ret);
        if (sds_name_equal(ename2,ename1)) 
			{
            cmd_ErrorPrompt(CMD_ERR_DUPENTITY,0);
            continue;
	        }
        if ((ent=sds_entget(ename2))==NULL) {rc=RTERROR;goto bail;}
        ic_assoc(ent,0);
        if (strisame(db_hitype2str(DB_LINE),ic_rbassoc->resval.rstring))
			{
            flag2=LINE;           
            ecsflag2=0;
            break;              
			}                       
        else if (strisame("ARC"/*DNT*/  ,ic_rbassoc->resval.rstring))
			{
            flag2=ARC; 
            ecsflag2=1;
            break;
			}
        else if (strisame("POLYLINE"/*DNT*/  ,ic_rbassoc->resval.rstring))
			{
            ic_assoc(ent,70);
            if (ic_rbassoc->resval.rint & IC_PLINE_CLOSED)
				{
	            if (ic_rbassoc->resval.rint & IC_PLINE_3DPLINE)
					{
                    flag2=CPLINE3D; //3D
                    ecsflag2=0;
					}
				else
					{
                    flag2=CPLINE2D; //2D
                    ecsflag2=1;
					}   
				}
			else
				{
	            if (ic_rbassoc->resval.rint & IC_PLINE_3DPLINE)
					{
                    flag2=OPLINE3D; //3D            
                    ecsflag2=0;
					}
				else
					{
                    flag2=OPLINE2D; //2D            
                    ecsflag2=1;
					}
				}
            break;
			}                                              
        else if (strisame("LWPOLYLINE"/*DNT*/  ,ic_rbassoc->resval.rstring))
			{
            ic_assoc(ent,70);
            if (ic_rbassoc->resval.rint & IC_PLINE_CLOSED)
				{
                flag2=CPLINE2D; //2D
                ecsflag2=1;
				}
			else
				{
                flag2=OPLINE2D; //2D            
                ecsflag2=1;
				}
            break;
			} 
        else if (strisame("ELLIPSE"/*DNT*/  ,ic_rbassoc->resval.rstring) || 
			strisame("SPLINE"/*DNT*/  ,ic_rbassoc->resval.rstring))
			{          
            ecsflag1=0;
            break;
			}
        cmd_ErrorPrompt(CMD_ERR_ENTTYPE,0);
        IC_RELRB(ent);
		}          
    sds_redraw(ename2,IC_REDRAW_HILITE);
    IC_RELRB(ent);
    

    for (;;)
		{
        if ((ret=sds_entsel(ResourceString(IDC_SURFACES__NSELECT_THIRD__4, "\nSelect third edge: " ) ,ename3,selpt3))!=RTNORM)
			return(ret);
        if ((sds_name_equal(ename3,ename1))||
           (sds_name_equal(ename3,ename2)))
			{
            cmd_ErrorPrompt(CMD_ERR_DUPENTITY,0);
            continue;
			}
        if ((ent=sds_entget(ename3))==NULL) {rc=RTERROR;goto bail;}
        ic_assoc(ent,0);
        if (strisame(db_hitype2str(DB_LINE),ic_rbassoc->resval.rstring))
			{ 
            flag3=LINE;           
            ecsflag3=0;
            break;              
			}                       
        else if (strisame("ARC"/*DNT*/  ,ic_rbassoc->resval.rstring))
			{
            flag3=ARC; 
            ecsflag3=1;
            break;
			}
        else if (strisame("POLYLINE"/*DNT*/  ,ic_rbassoc->resval.rstring))
			{
            ic_assoc(ent,70);
            if (ic_rbassoc->resval.rint & IC_PLINE_CLOSED)
				{
	            if (ic_rbassoc->resval.rint & IC_PLINE_3DPLINE)
					{
                    flag3=CPLINE3D; //3D
                    ecsflag3=0;
					}
				else
					{
                    flag3=CPLINE2D; //2D
                    ecsflag3=1;
					}   
				}
			else
				{
	            if (ic_rbassoc->resval.rint & IC_PLINE_3DPLINE)
					{
                    flag3=OPLINE3D; //3D            
                    ecsflag3=0;
					}
				else
					{
                    flag3=OPLINE2D; //2D            
                    ecsflag3=1;
					}                                  
				}
            break;
			}                                              
        else if (strisame("LWPOLYLINE"/*DNT*/  ,ic_rbassoc->resval.rstring))
			{
            ic_assoc(ent,70);
            if (ic_rbassoc->resval.rint & IC_PLINE_CLOSED)
				{
                flag3=CPLINE2D; //2D
                ecsflag3=1;
				}
			else
				{
                flag3=OPLINE2D; //2D            
                ecsflag3=1;
				}
            break;
			}    
        else if (strisame("ELLIPSE"/*DNT*/  ,ic_rbassoc->resval.rstring) || 
			strisame("SPLINE"/*DNT*/  ,ic_rbassoc->resval.rstring))
			{          
            ecsflag1=0;
            break;
			}
        cmd_ErrorPrompt(CMD_ERR_ENTTYPE,0);
        IC_RELRB(ent);
		}          
    sds_redraw(ename3,IC_REDRAW_HILITE);
    IC_RELRB(ent);
    

    for (;;)
		{
        if ((ret=sds_entsel(ResourceString(IDC_SURFACES__NSELECT_LAST_E_5, "\nSelect last edge: " ) ,ename4,selpt4))!=RTNORM)
			return(ret);
        if ((sds_name_equal(ename4,ename1))||
           (sds_name_equal(ename4,ename2))||
           (sds_name_equal(ename4,ename3))) 
			{
            cmd_ErrorPrompt(CMD_ERR_DUPENTITY,0);
            continue;
			}
        if ((ent=sds_entget(ename4))==NULL) {rc=RTERROR;goto bail;}
        ic_assoc(ent,0);
        if (strisame(db_hitype2str(DB_LINE),ic_rbassoc->resval.rstring))
			{ 
            flag4=LINE;           
            ecsflag4=0;
            break;              
			}                       
        else if (strisame("ARC"/*DNT*/  ,ic_rbassoc->resval.rstring))
			{
            flag4=ARC; 
            ecsflag4=1;
            break;
			}
        else if (strisame("POLYLINE"/*DNT*/  ,ic_rbassoc->resval.rstring))
			{
            ic_assoc(ent,70);
            if (ic_rbassoc->resval.rint & IC_PLINE_CLOSED)
				{
	            if (ic_rbassoc->resval.rint & IC_PLINE_3DPLINE)
					{					
                    flag4=CPLINE3D; //3D
                    ecsflag4=0;
					}
				else
					{
                    flag4=CPLINE2D; //2D
                    ecsflag4=1;
					}   
				}
			else
				{
	            if (ic_rbassoc->resval.rint & IC_PLINE_3DPLINE)
					{
                    flag4=OPLINE3D; //3D            
                    ecsflag4=0;
					}
				else
					{
                    flag4=OPLINE2D; //2D            
                    ecsflag4=1;
					}                                  
				}
            break;
			}                                              
        else if (strisame("LWPOLYLINE"/*DNT*/  ,ic_rbassoc->resval.rstring))
			{
            ic_assoc(ent,70);
            if (ic_rbassoc->resval.rint & IC_PLINE_CLOSED)
				{
                flag4=CPLINE2D; //2D
                ecsflag4=1;
				}
			else
				{
                flag4=OPLINE2D; //2D            
                ecsflag4=1;
				}
            break;
			}   
        else if (strisame("ELLIPSE"/*DNT*/  ,ic_rbassoc->resval.rstring) || 
			strisame("SPLINE"/*DNT*/  ,ic_rbassoc->resval.rstring))
			{          
            ecsflag1=0;
            break;
			}
        cmd_ErrorPrompt(CMD_ERR_ENTTYPE,0);
        IC_RELRB(ent);
		}          
    sds_redraw(ename4,IC_REDRAW_HILITE);
    IC_RELRB(ent);

    if ((ret=cmd_coons(ename1,ename2,ename3,ename4,ecsflag1,ecsflag2,ecsflag3,ecsflag4))!=RTNORM)
		{
		rc=ret;
		goto bail;
		}

    rc=RTNORM;
bail:
    IC_RELRB(ent);
    sds_redraw(ename1,IC_REDRAW_UNHILITE);
    sds_redraw(ename2,IC_REDRAW_UNHILITE);
    sds_redraw(ename3,IC_REDRAW_UNHILITE);
    sds_redraw(ename4,IC_REDRAW_UNHILITE);
    return(rc); 
}

//================================================================================================

short cmd_rev_path_pts(sds_name ename,struct cmd_rev_pts **rev_ptll,int *ntab) {
    struct resbuf *pEnt=NULL,*pTmp=NULL;
    int i=0;
	RT	ret;
    struct cmd_rev_pts *rev_tmp=NULL;

    if ((pTmp=pEnt=sds_entget(ename))==NULL) {ret=RTERROR;goto bail;}    
    if (pTmp->restype==-2) pTmp=pTmp->rbnext;
    if (pTmp->restype==-1) pTmp=pTmp->rbnext;
    if (pTmp->restype==0) {
        if (strisame(db_hitype2str(DB_LINE),pTmp->resval.rstring)) {
            for (;pTmp!=NULL;pTmp=pTmp->rbnext) {
                if (pTmp->restype==10) {
                    if (((*rev_ptll)= new cmd_rev_pts )==NULL) {;  
                        cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);            
                        ret=RTERROR; goto bail;                                    
                    }
                    (*rev_ptll)->prevpt=NULL;
                    (*rev_ptll)->nextpt=NULL;
                    (*rev_ptll)->pt[0]=pTmp->resval.rpoint[0];
                    (*rev_ptll)->pt[1]=pTmp->resval.rpoint[1];
                    (*rev_ptll)->pt[2]=pTmp->resval.rpoint[2];
                    (*ntab)++;
                }
            }
            for (pTmp=pEnt;pTmp!=NULL;pTmp=pTmp->rbnext) {
                if (pTmp->restype==11) {
                    if ((rev_tmp= new cmd_rev_pts )==NULL) {;  
                        cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);            
                        ret=RTERROR; goto bail;                                    
                    }
                    (*rev_ptll)->nextpt=rev_tmp;
                    rev_tmp->prevpt=*rev_ptll;
                    rev_tmp->nextpt=NULL;
                    rev_tmp->pt[0]=pTmp->resval.rpoint[0];
                    rev_tmp->pt[1]=pTmp->resval.rpoint[1];
                    rev_tmp->pt[2]=pTmp->resval.rpoint[2];
                    (*ntab)++;
                }
            }
        }      
    }
    ret=RTNORM;
bail:
    return(ret);

}

//================================================================================================

short cmd_rev_it_up(sds_name ename1,sds_point vpt,sds_point mpt,double sa,double inang,bool closeflag,bool ecsflag,bool splineflag){
    struct resbuf wcs,ecs,tcs,setgetrb,*ptll=NULL,*mesh=NULL,*pRbb=NULL,*pRb;
    sds_point pt0,*ptarry=NULL,tmpt,endpts[2];
    RT	rc,ret;
	short	pclose;
    double c,s;
    long vcount=0L;
    int mtab,ntab,i,ribs;
    
    wcs.restype=RTSHORT;
    wcs.resval.rint=0;
    tcs.restype=RT3DPOINT;
    ic_ptcpy(tcs.resval.rpoint,vpt);

    sds_entmake(NULL);
    pt0[0]=pt0[1]=0.0;
    
    if ((pRbb=sds_entget(ename1))==NULL) {rc=RTERROR;goto bail;}
    //get elevation
    for (pRb=pRbb;(pRb!=NULL)&&(pRb->restype!=10);pRb=pRb->rbnext);
    pt0[2]=pRb->resval.rpoint[2];

    if (ecsflag)
		{
        ecs.restype=RT3DPOINT;
        //we'll get the 210 group
        for (pRb=pRbb;(pRb!=NULL)&&(pRb->restype!=210);pRb=pRb->rbnext);
        ic_ptcpy(ecs.resval.rpoint,pRb->resval.rpoint);
		}
	else
		{
        ecs.restype=RTSHORT;
        ecs.resval.rint=0;
		}
    IC_RELRB(pRbb);

    //get the var that holds the number of surfaces to draw
    if ((ret=SDS_getvar(NULL,DB_QSURFTAB1,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM) {rc=ret;goto bail;} 
    ribs=mtab=setgetrb.resval.rint;

    //get the var that holds the number of surfaces to draw
    if ((ret=SDS_getvar(NULL,DB_QSURFTAB2,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM) {rc=ret;goto bail;} 
    ntab=setgetrb.resval.rint;

    //set the 70 flag 
    if (icadRealEqual(fabs(inang),IC_TWOPI))
		{
        if (closeflag)
            pclose=IC_PLINE_CLOSEDN | IC_PLINE_3DMESH | IC_PLINE_CLOSED; //closed in m and n direction
		else
			{
            pclose=IC_PLINE_3DMESH | IC_PLINE_CLOSED; //closed in m direction
            ntab+=1;
			}    
        inang=IC_TWOPI;
		}
	else
		{
        mtab+=1;
        if (closeflag)
            pclose=IC_PLINE_CLOSEDN | IC_PLINE_3DMESH; //closed in n direction                                  
		else
			{ 
            pclose=IC_PLINE_3DMESH; //not closed
            ntab+=1;
			}                                                
		}
    
    //divide the segment into segs number of points
    if (splineflag)
		{
        if (closeflag)
			{
            if ((ret=ic_divmeas(ename1,ntab,NULL,1.0,0.0,0,0,&ptll))!=0) {rc=ret;goto bail;} 
			}
		else
			{
            if ((ret=db_getendpts(ename1,endpts))!=0) {rc=RTERROR;goto bail;}
            if ((ret=ic_divmeas(ename1,ntab-1,NULL,1.0,0.0,0,0,&ptll))!=0) {rc=ret;goto bail;} 
            vcount=2;
			}
        for (pRb=ptll;pRb!=NULL;pRb=pRb->rbnext)
			vcount++;
		}
	else
		{
        //get the points for a non-fit pline
        if ((ret=cmd_divmeas_pline_pts(ename1,ntab-1,&vcount,&ptll))!=RTNORM) {rc=ret; goto bail;}
        ntab=vcount;
		}

    //malloc the point array
    if ((ptarry= new sds_point [vcount])==NULL) {rc=RTERROR;goto bail;}

    //move the points into wcs if not already there
    i=0;
    if (closeflag)
		{
        if (ecsflag) 
			{
            for (pRb=ptll;pRb!=NULL;pRb=pRb->rbnext,i++) 
				{
	            if (sds_usrbrk()) {rc=RTCAN; goto bail;}
                sds_trans(pRb->resval.rpoint,&ecs,&wcs,0,ptarry[i]);
				}
			}
		else
			{
            for (pRb=ptll;pRb!=NULL;pRb=pRb->rbnext,i++) 
				{
	            if (sds_usrbrk()) {rc=RTCAN; goto bail;}
                ic_ptcpy(ptarry[i],pRb->resval.rpoint);
				}
			}
		}
	else
		{
        if (ecsflag) 
			{
            if (splineflag) 
				{
                sds_trans(endpts[0],&ecs,&wcs,0,ptarry[0]);
                i++;
				}
            for (pRb=ptll;pRb!=NULL;pRb=pRb->rbnext,i++) 
				{
	            if (sds_usrbrk()) {rc=RTCAN; goto bail;}
                sds_trans(pRb->resval.rpoint,&ecs,&wcs,0,ptarry[i]);
				}
            if (splineflag)
				sds_trans(endpts[1],&ecs,&wcs,0,ptarry[i]);
			}
		else
			{
            if (splineflag) 
				{
                ic_ptcpy(ptarry[0],endpts[0]);
		        i++;
			    }
            for (pRb=ptll/*,i=1*/;pRb!=NULL;pRb=pRb->rbnext,i++) 
				{
	            if (sds_usrbrk()) {rc=RTCAN; goto bail;}
                ic_ptcpy(ptarry[i],pRb->resval.rpoint);
				}
            if (splineflag)
				ic_ptcpy(ptarry[i],endpts[1]);
			}
		}
    
    //if the starting angle is not 0.0 then move the first set of points to the starting angle
    if (!icadRealEqual(sa,0.0))
		{
        if (sa>=IC_TWOPI) 
			ic_normang(&sa,NULL);
        //move to tcs
        for (i=0;i<vcount;i++) 
			{
	        if (sds_usrbrk()) {rc=RTCAN; goto bail;}
            sds_trans(ptarry[i],&wcs,&tcs,0,ptarry[i]);
			}
        sds_trans(mpt,&wcs,&tcs,0,mpt);
        //move points 
        c=cos(sa);
        s=sin(sa);
        for (i=0;i<vcount;i++) 
			{
	        if (sds_usrbrk()) {rc=RTCAN; goto bail;}
            tmpt[0]=(ptarry[i][0]-mpt[0])*c-(ptarry[i][1]-mpt[1])*s+mpt[0];
            tmpt[1]=(ptarry[i][1]-mpt[1])*c+(ptarry[i][0]-mpt[0])*s+mpt[1];
            ptarry[i][0]=tmpt[0];
            ptarry[i][1]=tmpt[1];
			}
        //move back to the wcs
        for (i=0;i<vcount;i++) 
			{
	        if (sds_usrbrk()) {rc=RTCAN; goto bail;}
            sds_trans(ptarry[i],&tcs,&wcs,0,ptarry[i]);
			}
        sds_trans(mpt,&tcs,&wcs,0,mpt);
		}

    //build the pline header
    if ((mesh=sds_buildlist(RTDXF0,"POLYLINE"/*DNT*/,
                                66,1,                       
                                10,pt0, //use z value
                                70,pclose,
                                71,mtab,
                                72,ntab,
                                 0))==NULL) 
		{
        cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);            
        rc=RTERROR;
		goto bail;                                    
		}                                                           
    if ((ret=sds_entmake(mesh))!=RTNORM) {rc=ret;goto bail;} 
    IC_RELRB(mesh);

    //build the skelleton vertex
    if ((mesh=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,
                               10,pt0,  //dummy point
                               70,64,
                                 0))==NULL) 
		{
        cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);            
        rc=RTERROR;
		goto bail;                                    
		}                                 

    if ((ret=cmd_revolve_pl_ptarray(mpt,vpt,ptarry,vcount,inang,ribs,mesh))!=RTNORM) {rc=ret;goto bail;}
    IC_RELRB(mesh);
    //build the seqend
    if ((mesh=sds_buildlist(RTDXF0,"SEQEND"/*DNT*/,
                                 0))==NULL)
		{
        cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
        rc=RTERROR;
		goto bail;                                           
		}                                                                
    if ((ret=sds_entmake(mesh))!=RTNORM) {rc=ret;goto bail;}

    rc=RTNORM;
bail:
    if (ptarry)
		IC_FREE(ptarry);
    IC_RELRB(pRbb);
    IC_RELRB(ptll);  
    IC_RELRB(mesh); 
    return(rc);      
}

static
RT cmd_revsurf_acis()
{
    if (!CModeler::get()->canModify())
        return RTERROR;

    RT rc = RTERROR;
    sds_name enames[2] = { {0,0}, {0,0} };
    db_handitem* items[2];
    sds_real inAngle;
    sds_real startAngle = 0.0;

    // Get profile
    sds_point pt;
    for (;;)
	{
        for (;;)
		{
            RT ret=sds_entsel(ResourceString(IDC_SURFACES__NSELECT_A_LINE_6, "\nSelect an entity to revolve: "), enames[0], pt);
            if (ret==RTERROR || ret == RTCAN)
				goto bail;

            if (ret==RTNORM)
			{
                sds_redraw(enames[0],IC_REDRAW_HILITE);
                break;
			}
			else
			{
                rc=ret;
				goto bail;
			}
		}
        items[0] = reinterpret_cast<db_handitem*>(enames[0][0]);
        if (isKindOf(items[0], DB_LINE, DB_ARC, DB_ELLIPSE, DB_CIRCLE, DB_POLYLINE, DB_LWPOLYLINE, DB_SPLINE, -1))
            break;

	    sds_redraw(enames[0],IC_REDRAW_UNHILITE);
    }

    // Get axis
    for (;;)
	{
        for (;;)
		{
            RT ret=sds_entsel(ResourceString(IDC_SURFACES__NSELECT_THE_AX_8, "\nSelect the axis of revolution: " ), enames[1], pt);
            if (ret==RTERROR || ret==RTCAN)
				goto bail;
            if (ret==RTNORM)
			{
                sds_redraw(enames[1],IC_REDRAW_HILITE);
                break;
			}
			else
			{
                rc=ret;
				goto bail;
			}
		}
        items[1] = reinterpret_cast<db_handitem*>(enames[1][0]);
        if (isKindOf(items[1], DB_RAY, DB_XLINE, DB_LINE, DB_POLYLINE, DB_LWPOLYLINE, -1))
            break;

	    sds_redraw(enames[1],IC_REDRAW_UNHILITE);
    }

    // Get start angle
    {
        char szTmp[IC_ACADBUF];
        sds_angtos(startAngle,-1,-1,szTmp);
    
        char prompt[IC_ACADBUF];
        sprintf(prompt,ResourceString(IDC_SURFACES__NANGLE_TO_BEGI_9, "\nAngle to begin surface of revolution <%s>: " ) ,szTmp);

        RT ret;
	    if ((ret=sds_initget(0,NULL))!=RTNORM) 
        {
            rc=ret;
            goto bail;
        }

        ret=sds_getangle(NULL,prompt,&startAngle);
	    if (ret == RTERROR || ret == RTCAN) 
	    {
	        rc=ret;
		    goto bail;
	    }
    }

    // Get interior angle value
    {
        resbuf wcs;
        RT ret = SDS_getvar(NULL,DB_QSURFTAB2,&wcs,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
        if (ret != RTNORM) 
        {
            rc=ret;
            goto bail;
        }
        bool angleDir = 1 == wcs.resval.rint;

        char prompt[IC_ACADBUF];
	    if (angleDir)
           sprintf(prompt,ResourceString(IDC_SURFACES__NDEGREES_TO_R_10, "\nDegrees to revolve entity (+ for cw, - for ccw <360>: "));
	    else
           sprintf(prompt,ResourceString(IDC_SURFACES__NDEGREES_TO_R_11, "\nDegrees to revolve entity (+ for ccw, - for cw <360>: "));
        
        if ((ret = sds_initget(0,NULL)) != RTNORM)
        {
            rc = ret;
            goto bail;
        }

        ret = SDS_getanglebig(NULL, prompt, &inAngle);
		if (ret == RTCAN || ret == RTERROR)
		{
            rc=ret;
			goto bail;
	    }
		else if (ret == RTNONE)
            inAngle = IC_TWOPI;
		
        if (angleDir)
			inAngle = -inAngle;
    }

    do
    {
        using namespace geodata;

        curve* profile;
        if (!extract(*items[0], profile))
            break;
        auto_ptr<curve> hprof(profile);

        curve* axis;
        if (!extract(*items[1], axis))
            break;
        auto_ptr<curve> haxis(axis);

		CDbAcisEntityData body;
        if (CModeler::get()->createSurfaceRevolved(*profile, *axis, startAngle, inAngle, getCurrColorIndex(), body))
		{
			if (create(body))
				rc = RTNORM;
		}
    }
    while (false);

bail:
    if (enames[0][0])
		sds_redraw(enames[0],IC_REDRAW_UNHILITE);
    if (enames[1][0])
		sds_redraw(enames[1],IC_REDRAW_UNHILITE);
    return rc;
}

//==========================================================================================

short cmd_revsurf(void)  
{
    if (cmd_iswindowopen() == RTERROR) 
		return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    //<ALM>
    if (CModeler::get()->canModify())
        return cmd_revsurf_acis();
    //</ALM>
	else
		return cmd_revsurf_mesh();
}

short cmd_revsurf_mesh(void)  
{
    if (cmd_iswindowopen() == RTERROR) 
		return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    char prompt[IC_ACADBUF],szTmp[IC_ACADBUF];
    sds_name ename1,ename2;
    sds_point pt1,vpt,mpt,pttmp[2];
    sds_real sa,inang,dist1,dist2;
    bool closeflag,ecs1flag,splineflag,ecs2flag,angdir;
    RT rc,ret;
    struct resbuf *pRbb=NULL,*pRb,ecs,wcs,ucs;
	struct gra_view *view=SDS_CURGRVW;


    if ((ret=SDS_getvar(NULL,DB_QSURFTAB2,&wcs,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM) {rc=ret;goto bail;} 
	if (1==wcs.resval.rint)
		angdir=TRUE;
	else angdir=FALSE;
	
	ename1[0]=ename1[1]=0L;
	ename2[0]=ename2[1]=0L;
    ecs.restype=RT3DPOINT;
    wcs.restype=RTSHORT;
    wcs.resval.rint=0;
    ucs.restype=RTSHORT;
    ucs.resval.rint=1;
    
    //get the first entity and check to see if it is closed
    for (;;)
    	{
        if ((ret=sds_entsel(ResourceString(IDC_SURFACES__NSELECT_A_LINE_6, "\nSelect a entity to revolve: " ) ,ename1,pt1))!=RTNORM) {rc=ret;goto bail;}
        sds_redraw(ename1,IC_REDRAW_HILITE);

        if ((pRbb=sds_entget(ename1))==NULL) {rc=RTERROR;goto bail;}
        for (pRb=pRbb;(pRb!=NULL)&&(pRb->restype!=0);pRb=pRb->rbnext);
        if (strisame("ARC"/*DNT*/  ,pRb->resval.rstring))
			{
            closeflag=0;
            ecs1flag=1;
            splineflag=1;
            break;
			}
		else if (strisame(db_hitype2str(DB_LINE),pRb->resval.rstring))
			{
			closeflag=0;
            ecs1flag=0;
            splineflag=1;
            break;              
			}
		else if (strisame("POLYLINE"/*DNT*/  ,pRb->resval.rstring))
			{
            for (pRb=pRbb;(pRb!=NULL)&&(pRb->restype!=70);pRb=pRb->rbnext);
            //check for mesh
			if (pRb->resval.rint & (IC_PLINE_POLYFACEMESH | IC_PLINE_CLOSEDN | IC_PLINE_3DMESH))
				{
                cmd_ErrorPrompt(CMD_ERR_ENTTYPE,0);                          
                IC_RELRB(pRbb);
                continue;
				}
            //check for spline or fit
            if (pRb->resval.rint & (IC_PLINE_SPLINEFIT | IC_PLINE_CURVEFIT))
                splineflag=1;
			else
                splineflag=0;
            //check for closed
            if (pRb->resval.rint & IC_PLINE_CLOSED)
                closeflag=1;
			else
                closeflag=0;
            //check for 2D or 3D
            if (pRb->resval.rint & IC_PLINE_3DPLINE)
                ecs1flag=0;
            else
                ecs1flag=1;
            break;
			}
		else if (strisame("LWPOLYLINE"/*DNT*/  ,pRb->resval.rstring))
			{
			splineflag=0;
			ecs1flag=1;
			for (pRb=pRbb;(pRb!=NULL)&&(pRb->restype!=70);pRb=pRb->rbnext);
            //check for closed
            if (pRb->resval.rint & IC_PLINE_CLOSED)
                closeflag=1;
			else
                closeflag=0;
            break;
			}
		else if (strisame("CIRCLE"/*DNT*/  ,pRb->resval.rstring))
			{
            closeflag=1;
            ecs1flag=1;
            splineflag=1;
            break;       
			}
		else if (strisame("ELLIPSE",pRb->resval.rstring)) {
			db_ellipse* pEllipse = (db_ellipse*)ename1[0];

			double start, end;
			pEllipse->get_41(&start);
			pEllipse->get_42(&end);

            if (icadRealEqual(end - start,IC_TWOPI)) 
                closeflag=1;
			else {
                closeflag=0;
                db_getendpts(ename1,pttmp);
            }
            ecs1flag=0;
            splineflag=1;
            break;
		}
		else if (strisame("SPLINE"/*DNT*/, pRb->resval.rstring)) {
			db_spline* pSpline = (db_spline*)ename1[0];
            if (pSpline->isClosed() || pSpline->isPeriodic()) 
                closeflag=1;
			else {
                closeflag=0;
                db_getendpts(ename1,pttmp);
            }
            ecs1flag=0;
            splineflag=1;
			break;
		}
		else {
            cmd_ErrorPrompt(CMD_ERR_ENTTYPE,0);
            sds_redraw(ename1,IC_REDRAW_UNHILITE);
            IC_RELRB(pRbb);
			}
		}          
    IC_RELRB(pRbb);
    //get the second entity and check to see if it is not closed
    for (;;)
		{
        if ((ret=sds_entsel(ResourceString(IDC_SURFACES__NSELECT_THE_AX_8, "\nSelect the axis of revolution: " ) ,ename2,pt1))!=RTNORM) {rc=ret;goto bail;}
        sds_redraw(ename2,IC_REDRAW_HILITE);

        if ((pRbb=sds_entget(ename2))==NULL) {rc=RTERROR;goto bail;}
        for (pRb=pRbb;(pRb!=NULL)&&(pRb->restype!=0);pRb=pRb->rbnext);
        if (strisame(db_hitype2str(DB_LINE),pRb->resval.rstring))
			{ 
            ecs2flag=0;
            for (pRb=pRbb;(pRb!=NULL)&&(pRb->restype!=10);pRb=pRb->rbnext);
            ic_ptcpy(pttmp[0],pRb->resval.rpoint);
            for (pRb=pRbb;(pRb!=NULL)&&(pRb->restype!=11);pRb=pRb->rbnext);
            ic_ptcpy(pttmp[1],pRb->resval.rpoint);
            break;
			}
		else if (strisame("POLYLINE"/*DNT*/  ,pRb->resval.rstring))
			{
            for (pRb=pRbb;(pRb!=NULL)&&(pRb->restype!=70);pRb=pRb->rbnext);
            if (pRb->resval.rint & (IC_PLINE_POLYFACEMESH | IC_PLINE_CLOSEDN | IC_PLINE_3DMESH | IC_PLINE_CLOSED))
				{
                cmd_ErrorPrompt(CMD_ERR_ENTTYPE,0);                          
                IC_RELRB(pRbb);
                continue;
				}
            if (pRb->resval.rint & IC_PLINE_3DPLINE)
				{
                ecs2flag=0;
				}
			else
				{
                ecs2flag=1;
                for (pRb=pRbb;(pRb!=NULL)&&(pRb->restype!=210);pRb=pRb->rbnext);
                ic_ptcpy(ecs.resval.rpoint,pRb->resval.rpoint);
				}
            db_getendpts(ename2,pttmp);
            break;
			}
		else if (strisame("LWPOLYLINE"/*DNT*/  ,pRb->resval.rstring))
			{
            for (pRb=pRbb;(pRb!=NULL)&&(pRb->restype!=70);pRb=pRb->rbnext);
            ecs2flag=1;
            for (pRb=pRbb;(pRb!=NULL)&&(pRb->restype!=210);pRb=pRb->rbnext);
            ic_ptcpy(ecs.resval.rpoint,pRb->resval.rpoint);
            db_getendpts(ename2,pttmp);
            break;
			}
		else
			{
            cmd_ErrorPrompt(CMD_ERR_ENTTYPE,0);
            sds_redraw(ename2,IC_REDRAW_UNHILITE);
            IC_RELRB(pRbb);
			}
		}          
    IC_RELRB(pRbb);

    sa=0.0;
    sds_angtos(sa,-1,-1,szTmp);
    sprintf(prompt,ResourceString(IDC_SURFACES__NANGLE_TO_BEGI_9, "\nAngle to begin surface of revolution <%s>: " ) ,szTmp);
	if ((ret=sds_initget(0,NULL))!=RTNORM) {rc=ret;goto bail;}
	if ((ret=sds_getangle(NULL,prompt,&sa))==RTERROR) 
		{
	    rc=ret;
		goto bail;
		}
	else if (ret==RTCAN)
		{
        rc=ret;
		goto bail;
		}
    
	if (angdir)
       sprintf(prompt,ResourceString(IDC_SURFACES__NDEGREES_TO_R_10, "\nDegrees to revolve entity (+ for cw, - for ccw <360>: " ) );
	else
       sprintf(prompt,ResourceString(IDC_SURFACES__NDEGREES_TO_R_11, "\nDegrees to revolve entity (+ for ccw, - for cw <360>: " ) );
    for (;;)
		{        
        sds_initget(0,NULL);
    	if ((ret=SDS_getanglebig(NULL,prompt,&inang))==RTNORM)
			{
			}
		else if (ret==RTCAN||ret==RTERROR)
			{
            rc=ret;
			goto bail;
	        }
		else if (ret==RTNONE)
            inang=IC_TWOPI;
		if (angdir)
			inang=-inang;
        break;
		}

    //check and see if the vector is a line or pline
    vpt[0]=pttmp[1][0]-pttmp[0][0];        
    vpt[1]=pttmp[1][1]-pttmp[0][1];    
    vpt[2]=pttmp[1][2]-pttmp[0][2];    
    mpt[0]=pttmp[0][0]+(vpt[0]/2.0);
    mpt[1]=pttmp[0][1]+(vpt[1]/2.0);
    mpt[2]=pttmp[0][2]+(vpt[2]/2.0);

    if (ecs2flag)
		{
        sds_trans(pttmp[0],&ecs,&ucs,0,pttmp[0]);
        sds_trans(pttmp[1],&ecs,&ucs,0,pttmp[1]);
		}
	else
		{
        sds_trans(pttmp[0],&wcs,&ucs,0,pttmp[0]);
        sds_trans(pttmp[1],&wcs,&ucs,0,pttmp[1]);
		}

	gra_ucs2rp(pt1,pt1,view);
    gra_ucs2rp(pttmp[0],pttmp[0],view);
    gra_ucs2rp(pttmp[1],pttmp[1],view);
    dist1=cmd_dist2d(pttmp[0],pt1);
    dist2=cmd_dist2d(pttmp[1],pt1);
    if (dist1>dist2)
		{
		vpt[0]=-vpt[0];    
        vpt[1]=-vpt[1];    
        vpt[2]=-vpt[2];    
		}                                                
    
    if (ecs2flag) 
		{
        if ((ret=sds_trans(vpt,&ecs,&wcs,1,vpt))!=RTNORM) {rc=ret;goto bail;}
        if ((ret=sds_trans(mpt,&ecs,&wcs,0,mpt))!=RTNORM) {rc=ret;goto bail;}
		}

    if ((ret=cmd_rev_it_up(ename1,vpt,mpt,sa,inang,closeflag,ecs1flag,splineflag))!=RTNORM) {rc=ret;goto bail;}

    rc=RTNORM;
bail:
    if (ename1[0])
		sds_redraw(ename1,IC_REDRAW_UNHILITE);
    if (ename2[0])
		sds_redraw(ename2,IC_REDRAW_UNHILITE);
    IC_RELRB(pRbb);
    return(rc); 
}

short cmd_reverse_link_list(struct resbuf **linklist) {
    struct resbuf *rbb=NULL,*rbn=NULL,*rbt=NULL;
    RT rc;

    //store the pointer to the link list to rbb
    //*linklist will always point to the begining of the list
    //as a safty factor. In the event of a "bail" nothing will be lost
    rbb=*linklist;

    //reverse the linklist by changing the rbnext pointers.
    if (rbb->rbnext!=NULL){   //if there is only one element then don't do anything
        for (rbn=rbt=rbb;rbn!=NULL;rbb->rbnext=rbt){
            if (sds_usrbrk()) { rc=RTCAN; goto bail; }  //check for break  
            if (rbb==rbt){
                rbb=rbb->rbnext;
                rbn=rbb->rbnext;
                rbt->rbnext=NULL; //now the first link will have the NULL for rbnext
            }else{
                rbt=rbb;
                rbb=rbn;
                rbn=rbn->rbnext;
            }
        }     
    }
    //after a successful reversal reset the address to the beginning of the new list 
    *linklist=rbb;        

    rc=RTNORM;
bail:
    return(rc);
}

static 
RT cmd_get_boundary_curves(db_handitem* pDbCurve,					// Input: A curve
						   db_handitem* pPoint,						// Input: A point
						   std::vector<geodata::curve*> &crvs)		// Output: Boundary curves
{
	using namespace geodata;

	db_point* pDbPoint = static_cast<db_point*>(pPoint);
	
	sds_point point;
	pDbPoint->get_10(point);	
	icl::point pt(point[0], point[1], point[2]);


	curve* pCurve1;
	if (!extract(*pDbCurve, pCurve1))
		return RTERROR;

	icl::point start, end;
	pCurve1->getStartPoint(start);
	pCurve1->getEndPoint(end);

	line* pCurve2 = new geodata::line;
	pCurve2->m_start = end;
	pCurve2->m_end = pt;

	line* pCurve3 = new geodata::line;
	pCurve3->m_start = pt;
	pCurve3->m_end = start;

	crvs.push_back(pCurve1);
	crvs.push_back(pCurve2);
	crvs.push_back(pCurve3);

	return RTNORM;
}


static
RT cmd_rulesurf_acis()
{
    if (!CModeler::get()->canModify())
        return RTERROR;

    RT rc = RTERROR;
    sds_name ename1 = {0,0}, ename2 = {0,0};

    sds_point pt;
	bool isPoint = false;
    for (;;)
	{
        for (;;)
		{
            RT ret=sds_entsel(ResourceString(IDC_SURFACES__NSELECT_ENTIT_12, "\nSelect entity to define first end of ruled surface: " ) ,ename1, pt);
            if (ret==RTERROR)
				continue;
            if (ret==RTNORM)
			{
                sds_redraw(ename1,IC_REDRAW_HILITE);
                break;
			}
			else
			{
                rc=ret;
				goto bail;
			}
		}
        db_handitem* item = reinterpret_cast<db_handitem*>(ename1[0]);
        if (isKindOf(item, DB_LINE, DB_ARC, DB_ELLIPSE, DB_CIRCLE, DB_POLYLINE, DB_LWPOLYLINE, DB_SPLINE, -1))
            break;

		if (isKindOf(item, DB_POINT)) {
			isPoint = true;
			break;
		}
	    sds_redraw(ename1,IC_REDRAW_UNHILITE);
    }

    for (;;)
	{
        for (;;)
		{
            RT ret=sds_entsel(ResourceString(IDC_SURFACES__NSELECT_ENTIT_15, "\nSelect entity to define other end: " ) ,ename2,pt);
            if (ret==RTERROR)
				continue;
            if (ret==RTNORM)
			{
                sds_redraw(ename2,IC_REDRAW_HILITE);
                break;
			}
			else
			{
                rc=ret;
				goto bail;
			}
		}
        db_handitem* item = reinterpret_cast<db_handitem*>(ename2[0]);
        if (isKindOf(item, DB_LINE, DB_ARC, DB_ELLIPSE, DB_CIRCLE, DB_POLYLINE, DB_LWPOLYLINE, DB_SPLINE, -1))
            break;

		if (isKindOf(item, DB_POINT)) {
			if (isPoint) 
				cmd_ErrorPrompt(CMD_ERR_INVALID_RULESURF_INPUT,0); 
			else {
				isPoint = true;
				break;
			}
		}
			
	    sds_redraw(ename2,IC_REDRAW_UNHILITE);
    }

    do
    {
        using namespace geodata;

		if (!isPoint) {
			curve* curve1;
			if (!extract(*reinterpret_cast<db_handitem*>(ename1[0]), curve1))
				break;
			auto_ptr<curve> holder1(curve1);
        
			curve* curve2;
			if (!extract(*reinterpret_cast<db_handitem*>(ename2[0]), curve2))
				break;
			auto_ptr<curve> holder2(curve2);

			CDbAcisEntityData body;
			if (CModeler::get()->createSurfaceRuled(*curve1, *curve2, getCurrColorIndex(), body))
			{
				if (create(body))
					rc = RTNORM;
			}
		}
		else {
			CDbAcisEntityData body;
			std::vector<geodata::curve*> crvs;
			db_handitem* item = reinterpret_cast<db_handitem*>(ename1[0]);

			RT oc;
			if (isKindOf(item, DB_POINT)) 
				oc = cmd_get_boundary_curves(reinterpret_cast<db_handitem*>(ename2[0]), item, crvs);
			else 
				oc = cmd_get_boundary_curves(reinterpret_cast<db_handitem*>(ename1[0]), 
											reinterpret_cast<db_handitem*>(ename2[0]), crvs);

			if (oc == RTNORM) {
				if (CModeler::get()->createSurfaceCurves(crvs, getCurrColorIndex(), body)) {
					if (create(body))
						rc = RTNORM;
				}
				for (register i = 0; i < 3; i++) 
					delete crvs[i];
			}
		}
    }
    while (false);

bail:
    if (ename1[0])
		sds_redraw(ename1,IC_REDRAW_UNHILITE);
    if (ename2[0])
		sds_redraw(ename2,IC_REDRAW_UNHILITE);
    return rc;
}

//rewrote the whole function 5-22-97 SJP
short cmd_rulesurf(void)  
{
    if (cmd_iswindowopen() == RTERROR)
		return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    // <ALM>
    if (CModeler::get()->canModify())
        return cmd_rulesurf_acis(); // run implementation based on Acis
    // </ALM>
	else
		return cmd_rulesurf_mesh();
}

short cmd_rulesurf_mesh(void)  
{
    if (cmd_iswindowopen() == RTERROR)
		return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    int segs,pclose,i;
    double dist1,dist2;
    sds_name ename1, ename2;
    sds_point spt1,spt2,ept2,ept1,pt1,pt2,pttmp[2],tpt,*pp1;
    struct resbuf *ent=NULL,ecs,wcs,ucs,setgetrb,*trb=NULL,*pline=NULL,*ptll1=NULL,*ptll2=NULL;
    RT rc, ret,openflag1,openflag2;
	struct gra_view *view=SDS_CURGRVW;

	ename1[0]=ename1[1]=0L;
	ename2[0]=ename2[1]=0L;
    ucs.restype=RTSHORT;
    ucs.resval.rint=1;
    wcs.restype=RTSHORT;
    wcs.resval.rint=0;
    ecs.restype=RT3DPOINT;
    ecs.resval.rpoint[0]=0.0;
    ecs.resval.rpoint[1]=0.0;
    ecs.resval.rpoint[2]=1.0;


    //get the var that holds the number of surfaces to draw
    if ((ret=SDS_getvar(NULL,DB_QSURFTAB1,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM) {rc=ret;goto bail;} 
    segs=setgetrb.resval.rint;

    //get the first entity and check to see if it is closed
    for (;;)
		{
        for (;;)
			{
            ret=sds_entsel(ResourceString(IDC_SURFACES__NSELECT_ENTIT_12, "\nSelect entity to define first end of ruled surface: " ) ,ename1,pt1);
            if (ret==RTERROR)
				continue;
            if (ret==RTNORM)
				{
                sds_redraw(ename1,IC_REDRAW_HILITE);
                break;
				}
			else
				{
                rc=ret;
				goto bail;
				}
			}

        if ((ent=sds_entget(ename1))==NULL) {rc=RTERROR;goto bail;}
        ic_assoc(ent,0);
        if (strisame(db_hitype2str(DB_LINE),ic_rbassoc->resval.rstring))
			{
         	//get points
            if ((ret=ic_divmeas(ename1,segs,NULL,1.0,0.0,0,0,&ptll1))!=0) {rc=RTERROR;goto bail;} 
            db_getendpts(ename1,pttmp);
            //trans endpts and pick to ucs
            sds_trans(pttmp[0],&wcs,&ucs,0,spt1);
            sds_trans(pttmp[1],&wcs,&ucs,0,ept1);
            gra_ucs2rp(spt1,spt1,view);
            gra_ucs2rp(ept1,ept1,view);
            gra_ucs2rp(pt1,pt1,view);
            //find distance
            dist1=cmd_dist2d(spt1,pt1);
            dist2=cmd_dist2d(ept1,pt1);
            //reverse points
            if (dist1>dist2)
				{
				ic_ptcpy(tpt,pttmp[1]);
                ic_ptcpy(pttmp[1],pttmp[0]);
                ic_ptcpy(pttmp[0],tpt);
                if ((ret=cmd_reverse_link_list(&ptll1))!=RTNORM) {rc=RTERROR;goto bail;} 
				}
            //set spt and ept
            ic_ptcpy(spt1,pttmp[0]);ic_ptcpy(ept1,pttmp[1]);
            openflag1=1;
			break;              
			}
		else if (strisame("ARC"/*DNT*/  ,ic_rbassoc->resval.rstring))
			{
            //set ecs direction
            ic_assoc(ent,210);
            ic_ptcpy(ecs.resval.rpoint,ic_rbassoc->resval.rpoint); 
         	//get points
            if ((ret=ic_divmeas(ename1,segs,NULL,1.0,0.0,0,0,&ptll1))!=0) {rc=RTERROR;goto bail;} 
            db_getendpts(ename1,pttmp);
            //trans endpts and pick to ucs
            sds_trans(pttmp[0],&ecs,&ucs,0,spt1);
            sds_trans(pttmp[1],&ecs,&ucs,0,ept1);
            gra_ucs2rp(spt1,spt1,view);
            gra_ucs2rp(ept1,ept1,view);
            gra_ucs2rp(pt1,pt1,view);
            //find distance
            dist1=cmd_dist2d(spt1,pt1);
            dist2=cmd_dist2d(ept1,pt1);
            //reverse points
            if (dist1>dist2)
				{ 
                ic_ptcpy(tpt,pttmp[1]);
                ic_ptcpy(pttmp[1],pttmp[0]);
                ic_ptcpy(pttmp[0],tpt);
                if ((ret=cmd_reverse_link_list(&ptll1))!=RTNORM) {rc=RTERROR;goto bail;} 
				}
            //make points wcs
            sds_trans(pttmp[0],&ecs,&wcs,0,spt1);
            sds_trans(pttmp[1],&ecs,&wcs,0,ept1);
            for (trb=ptll1;trb!=NULL;trb=trb->rbnext) 
				{
                sds_trans(trb->resval.rpoint,&ecs,&wcs,0,trb->resval.rpoint); 
				}
            openflag1=1;
            break;
			}
		else if (strisame("POINT"/*DNT*/  ,ic_rbassoc->resval.rstring))
			{
            ic_assoc(ent,10);
            ic_ptcpy(spt1,ic_rbassoc->resval.rpoint);
            openflag1=2;
            break;
	        }
		else if (strisame("POLYLINE"/*DNT*/  ,ic_rbassoc->resval.rstring))
			{
            ic_assoc(ent,70);
            if (ic_rbassoc->resval.rint & (IC_PLINE_POLYFACEMESH | IC_PLINE_3DMESH)) 
				{
                cmd_ErrorPrompt(CMD_ERR_ENTTYPE,0);
                continue;
				}
            if (ic_rbassoc->resval.rint & IC_PLINE_3DPLINE)
				{
                if (ic_rbassoc->resval.rint & (IC_PLINE_CLOSED | IC_PLINE_CLOSEDN)) //if CLOSED
					{
					//get points
                    if ((ret=ic_divmeas(ename1,segs,NULL,1.0,0.0,0,0,&ptll1))!=0) {rc=ret;goto bail;} 
                    if ((ret=cmd_reverse_link_list(&ptll1))!=RTNORM) {rc=RTERROR;goto bail;} 
                    openflag1=0;
					}
				else
					{
                    //get points
                    if ((ret=ic_divmeas(ename1,segs,NULL,1.0,0.0,0,0,&ptll1))!=0) {rc=ret;goto bail;} 
                    db_getendpts(ename1,pttmp);
                    //trans endpts and jpick to ucs
                    sds_trans(pttmp[0],&wcs,&ucs,0,spt1);
                    sds_trans(pttmp[1],&wcs,&ucs,0,ept1);
                    gra_ucs2rp(spt1,spt1,view);
                    gra_ucs2rp(ept1,ept1,view);
                    gra_ucs2rp(pt1,pt1,view);
                    //find distance
                    dist1=cmd_dist2d(spt1,pt1);
                    dist2=cmd_dist2d(ept1,pt1);
                    //reverse points
                    if (dist1>dist2)
						{ 
                        ic_ptcpy(tpt,pttmp[1]);
                        ic_ptcpy(pttmp[1],pttmp[0]);
                        ic_ptcpy(pttmp[0],tpt);
                        if ((ret=cmd_reverse_link_list(&ptll1))!=RTNORM) {rc=RTERROR;goto bail;} 
						}
                    //set spt and ept
                    ic_ptcpy(spt1,pttmp[0]);
					ic_ptcpy(ept1,pttmp[1]);
                    openflag1=1;
					}   
				}
			else  //ECS
                {
				//get 210
                ic_assoc(ent,210);
                ic_ptcpy(ecs.resval.rpoint,ic_rbassoc->resval.rpoint); 
                ic_assoc(ent,70);
                if (ic_rbassoc->resval.rint & (IC_PLINE_CLOSED | IC_PLINE_CLOSEDN)) //if CLOSED
					{
					//get points
                    if ((ret=ic_divmeas(ename1,segs,NULL,1.0,0.0,0,0,&ptll1))!=0) {rc=ret;goto bail;} 
                    if ((ret=cmd_reverse_link_list(&ptll1))!=RTNORM) {rc=RTERROR;goto bail;} 
                    //make points wcs
                    for (trb=ptll1;trb!=NULL;trb=trb->rbnext)
                        sds_trans(trb->resval.rpoint,&ecs,&wcs,0,trb->resval.rpoint); 
                    openflag1=0;
	                }
				else
					{
                    //get points
                    if ((ret=ic_divmeas(ename1,segs,NULL,1.0,0.0,0,0,&ptll1))!=0) {rc=ret;goto bail;} 
                    db_getendpts(ename1,pttmp);
                    //trans endpts and pick to ucs
                    sds_trans(pttmp[0],&ecs,&ucs,0,spt1);
                    sds_trans(pttmp[1],&ecs,&ucs,0,ept1);
                    gra_ucs2rp(spt1,spt1,view);
                    gra_ucs2rp(ept1,ept1,view);
                    gra_ucs2rp(pt1,pt1,view);
                    //find distance
                    dist1=cmd_dist2d(spt1,pt1);
                    dist2=cmd_dist2d(ept1,pt1);
                    //reverse points
                    if (dist1>dist2)
						{ 
                        ic_ptcpy(tpt,pttmp[1]);
                        ic_ptcpy(pttmp[1],pttmp[0]);
                        ic_ptcpy(pttmp[0],tpt);
                        if ((ret=cmd_reverse_link_list(&ptll1))!=RTNORM) {rc=RTERROR;goto bail;} 
						}
                    //make points wcs
                    sds_trans(pttmp[0],&ecs,&wcs,0,spt1);
                    sds_trans(pttmp[1],&ecs,&wcs,0,ept1);
                    for (trb=ptll1;trb!=NULL;trb=trb->rbnext) 
                        sds_trans(trb->resval.rpoint,&ecs,&wcs,0,trb->resval.rpoint); 
                    openflag1=1;
	                }   
		        }
            break;
			}
		else if (strisame("LWPOLYLINE"/*DNT*/  ,ic_rbassoc->resval.rstring))
			{
            ic_assoc(ent,70);
			//get 210
            ic_assoc(ent,210);
            ic_ptcpy(ecs.resval.rpoint,ic_rbassoc->resval.rpoint); 
            ic_assoc(ent,70);
            if (ic_rbassoc->resval.rint & IC_PLINE_CLOSED) //if CLOSED
				{
				//get points
                if ((ret=ic_divmeas(ename1,segs,NULL,1.0,0.0,0,0,&ptll1))!=0) {rc=ret;goto bail;} 
                if ((ret=cmd_reverse_link_list(&ptll1))!=RTNORM) {rc=RTERROR;goto bail;} 
                //make points wcs
                for (trb=ptll1;trb!=NULL;trb=trb->rbnext)
                    sds_trans(trb->resval.rpoint,&ecs,&wcs,0,trb->resval.rpoint); 
                openflag1=0;
	            }
			else
				{
                //get points
                if ((ret=ic_divmeas(ename1,segs,NULL,1.0,0.0,0,0,&ptll1))!=0) {rc=ret;goto bail;} 
                db_getendpts(ename1,pttmp);
                //trans endpts and pick to ucs
                sds_trans(pttmp[0],&ecs,&ucs,0,spt1);
                sds_trans(pttmp[1],&ecs,&ucs,0,ept1);
                gra_ucs2rp(spt1,spt1,view);
                gra_ucs2rp(ept1,ept1,view);
                gra_ucs2rp(pt1,pt1,view);
                //find distance
                dist1=cmd_dist2d(spt1,pt1);
                dist2=cmd_dist2d(ept1,pt1);
                //reverse points
                if (dist1>dist2)
					{ 
                    ic_ptcpy(tpt,pttmp[1]);
                    ic_ptcpy(pttmp[1],pttmp[0]);
                    ic_ptcpy(pttmp[0],tpt);
                    if ((ret=cmd_reverse_link_list(&ptll1))!=RTNORM) {rc=RTERROR;goto bail;} 
					}
                //make points wcs
                sds_trans(pttmp[0],&ecs,&wcs,0,spt1);
                sds_trans(pttmp[1],&ecs,&wcs,0,ept1);
                for (trb=ptll1;trb!=NULL;trb=trb->rbnext) 
                    sds_trans(trb->resval.rpoint,&ecs,&wcs,0,trb->resval.rpoint); 
                openflag1=1;
	            }   
            break;
			}
		else if (strisame("CIRCLE"/*DNT*/  ,ic_rbassoc->resval.rstring))
			{
            //set ecs direction
            ic_assoc(ent,210);
            ic_ptcpy(ecs.resval.rpoint,ic_rbassoc->resval.rpoint); 
         	//get points
            if ((ret=ic_divmeas(ename1,segs,NULL,1.0,0.0,0,0,&ptll1))!=0) {rc=ret;goto bail;} 
            //make points wcs
            for (trb=ptll1;trb!=NULL;trb=trb->rbnext)
                sds_trans(trb->resval.rpoint,&ecs,&wcs,0,trb->resval.rpoint); 
            openflag1=0;    
            break;       
	        }
		else if (strisame("ELLIPSE"/*DNT*/  ,ic_rbassoc->resval.rstring)) {
            if ((ret=ic_divmeas(ename1,segs,NULL,1.0,0.0,0,0,&ptll1))!=0) {
				rc=ret;
				goto bail;
			} 

            ic_assoc(ent,41);
			//not using icadAngleEqual, because it must be 0.0, not 2pi
            double start = ic_rbassoc->resval.rreal; 
            ic_assoc(ent,42);
            double end = ic_rbassoc->resval.rreal;
			if (icadRealEqual(end - start, IC_TWOPI)) 
                openflag1=0;
			else {
                db_getendpts(ename1,pttmp);
                //trans endpts and pick to ucs
                sds_trans(pttmp[0],&wcs,&ucs,0,spt1);
                sds_trans(pttmp[1],&wcs,&ucs,0,ept1);
                gra_ucs2rp(spt1,spt1,view);
                gra_ucs2rp(ept1,ept1,view);
                gra_ucs2rp(pt1,pt1,view);
                //find distance
                dist1=cmd_dist2d(spt1,pt1);
                dist2=cmd_dist2d(ept1,pt1);
                //reverse points
                if (dist1 > dist2) {
					ic_ptcpy(tpt,pttmp[1]);
                    ic_ptcpy(pttmp[1],pttmp[0]);
                    ic_ptcpy(pttmp[0],tpt);
                    if ((ret=cmd_reverse_link_list(&ptll1))!=RTNORM) {
						rc=RTERROR;
						goto bail;
					} 
				}
                //set spt and ept
                ic_ptcpy(spt1,pttmp[0]);
				ic_ptcpy(ept1,pttmp[1]);
                openflag1=1;
			}
            break;
		}
		else if (strisame("SPLINE"/*DNT*/, ic_rbassoc->resval.rstring)) {
            if ((ret=ic_divmeas(ename1,segs,NULL,1.0,0.0,0,0,&ptll1))!=0) {
				rc=ret;
				goto bail;
			} 

			db_spline* pSpline = (db_spline*)ename1[0];
            if (pSpline->isClosed() || pSpline->isPeriodic()) 
                openflag1=0;
			else {
                db_getendpts(ename1,pttmp);
                //trans endpts and pick to ucs
                sds_trans(pttmp[0],&wcs,&ucs,0,spt1);
                sds_trans(pttmp[1],&wcs,&ucs,0,ept1);
                gra_ucs2rp(spt1,spt1,view);
                gra_ucs2rp(ept1,ept1,view);
                gra_ucs2rp(pt1,pt1,view);
                //find distance
                dist1=cmd_dist2d(spt1,pt1);
                dist2=cmd_dist2d(ept1,pt1);
                //reverse points
                if (dist1 > dist2) {
					ic_ptcpy(tpt,pttmp[1]);
                    ic_ptcpy(pttmp[1],pttmp[0]);
                    ic_ptcpy(pttmp[0],tpt);
                    if ((ret=cmd_reverse_link_list(&ptll1))!=RTNORM) {
						rc=RTERROR;
						goto bail;
					} 
				}
                //set spt and ept
                ic_ptcpy(spt1,pttmp[0]);
				ic_ptcpy(ept1,pttmp[1]);
                openflag1=1;
            }

			break;
		}
		else {
			cmd_ErrorPrompt(CMD_ERR_ENTTYPE,0);
			sds_redraw(ename1,IC_REDRAW_UNHILITE);
			IC_RELRB(ent);
		}
	}          
    IC_RELRB(ent);
    //get the second entity and check to see if it is closed
    for (;;)
		{
        for (;;) 
			{
            ret=sds_entsel(ResourceString(IDC_SURFACES__NSELECT_ENTIT_15, "\nSelect entity to define other end: " ) ,ename2,pt2);
            if (ret==RTERROR)
				continue;
            if (ret==RTNORM)
				{
                if (sds_name_equal(ename1,ename2)) 
					{
                    cmd_ErrorPrompt(CMD_ERR_DUPENTITY,0);
                    continue;
					}
                sds_redraw(ename2,IC_REDRAW_HILITE);
                break;
				}
			else
				{
                rc=ret;
				goto bail;
				}
			}

        if ((ent=sds_entget(ename2))==NULL) {rc=RTERROR;goto bail;}
        ic_assoc(ent,0);
        if (strisame(db_hitype2str(DB_LINE),ic_rbassoc->resval.rstring))
			{
         	//get points
            if ((ret=ic_divmeas(ename2,segs,NULL,1.0,0.0,0,0,&ptll2))!=0) {rc=ret;goto bail;} 
            db_getendpts(ename2,pttmp);
            //trans endpts and pick to ucs
            sds_trans(pttmp[0],&wcs,&ucs,0,spt2);
            sds_trans(pttmp[1],&wcs,&ucs,0,ept2);
            gra_ucs2rp(spt2,spt2,view);
            gra_ucs2rp(ept2,ept2,view);
            gra_ucs2rp(pt2,pt2,view);
            //find distance
            dist1=cmd_dist2d(spt2,pt2);
            dist2=cmd_dist2d(ept2,pt2);
            //reverse points
            if (dist1>dist2)
				{ 
                ic_ptcpy(tpt,pttmp[1]);
                ic_ptcpy(pttmp[1],pttmp[0]);
                ic_ptcpy(pttmp[0],tpt);
                if ((ret=cmd_reverse_link_list(&ptll2))!=RTNORM) {rc=RTERROR;goto bail;} 
				}
            //set spt and ept
            ic_ptcpy(spt2,pttmp[0]);
			ic_ptcpy(ept2,pttmp[1]);
            openflag2=1;
            break;      
			}
		else if (strisame("ARC"/*DNT*/  ,ic_rbassoc->resval.rstring))
			{
            //set ecs direction
            ic_assoc(ent,210);
            ic_ptcpy(ecs.resval.rpoint,ic_rbassoc->resval.rpoint); 
         	//get points
            if ((ret=ic_divmeas(ename2,segs,NULL,1.0,0.0,0,0,&ptll2))!=0) {rc=ret;goto bail;} 
            db_getendpts(ename2,pttmp);
            //trans endpts and pick to ucs
            sds_trans(pttmp[0],&ecs,&ucs,0,spt2);
            sds_trans(pttmp[1],&ecs,&ucs,0,ept2);
            gra_ucs2rp(spt2,spt2,view);
            gra_ucs2rp(ept2,ept2,view);
            gra_ucs2rp(pt2,pt2,view);
            //find distance
            dist1=cmd_dist2d(spt2,pt2);
            dist2=cmd_dist2d(ept2,pt2);
            //reverse points
            if (dist1>dist2)
				{
                ic_ptcpy(tpt,pttmp[1]);
                ic_ptcpy(pttmp[1],pttmp[0]);
                ic_ptcpy(pttmp[0],tpt);
                if ((ret=cmd_reverse_link_list(&ptll2))!=RTNORM) {rc=RTERROR;goto bail;} 
	            }
            //make points wcs
            sds_trans(pttmp[0],&ecs,&wcs,0,spt2);
            sds_trans(pttmp[1],&ecs,&wcs,0,ept2);
            for (trb=ptll2;trb!=NULL;trb=trb->rbnext)
                sds_trans(trb->resval.rpoint,&ecs,&wcs,0,trb->resval.rpoint); 
            openflag2=1;
            break;
			}
		else if (strisame("POINT"/*DNT*/  ,ic_rbassoc->resval.rstring))
			{
            ic_assoc(ent,10);
            ic_ptcpy(spt2,ic_rbassoc->resval.rpoint);
            openflag2=2;
            break;
			}
		else if (strisame("POLYLINE"/*DNT*/  ,ic_rbassoc->resval.rstring))
			{
            ic_assoc(ent,70);
            if (ic_rbassoc->resval.rint & (IC_PLINE_POLYFACEMESH | IC_PLINE_3DMESH)) 
				{
                cmd_ErrorPrompt(CMD_ERR_ENTTYPE,0);
                continue;
				}
            if (ic_rbassoc->resval.rint & IC_PLINE_3DPLINE)  //if WCS            
                {
				if (ic_rbassoc->resval.rint & (IC_PLINE_CLOSED | IC_PLINE_CLOSEDN)) //if CLOSED
					{
					//get points
                    if ((ret=ic_divmeas(ename2,segs,NULL,1.0,0.0,0,0,&ptll2))!=0) {rc=ret;goto bail;} 
                    if ((ret=cmd_reverse_link_list(&ptll2))!=RTNORM) {rc=RTERROR;goto bail;} 
                    openflag1=0;
					}
				else
					{
                    //get points
                    if ((ret=ic_divmeas(ename2,segs,NULL,1.0,0.0,0,0,&ptll2))!=0) {rc=ret;goto bail;} 
                    db_getendpts(ename2,pttmp);
                    //trans endpts and jpick to ucs
                    sds_trans(pttmp[0],&wcs,&ucs,0,spt2);
                    sds_trans(pttmp[1],&wcs,&ucs,0,ept2);
                    gra_ucs2rp(spt2,spt2,view);
                    gra_ucs2rp(ept2,ept2,view);
                    gra_ucs2rp(pt2,pt2,view);
                    //find distance
                    dist1=cmd_dist2d(spt2,pt2);
                    dist2=cmd_dist2d(ept2,pt2);
                    //reverse points
                    if (dist1>dist2)
						{ 
                        ic_ptcpy(tpt,pttmp[1]);
                        ic_ptcpy(pttmp[1],pttmp[0]);
                        ic_ptcpy(pttmp[0],tpt);
                        if ((ret=cmd_reverse_link_list(&ptll2))!=RTNORM) {rc=RTERROR;goto bail;} 
						}
                    //set spt and ept
                    ic_ptcpy(spt2,pttmp[0]);ic_ptcpy(ept2,pttmp[1]);
                    openflag2=1;
					}   
				}
			else  //ECS
                {
				//get 210
                ic_assoc(ent,210);
                ic_ptcpy(ecs.resval.rpoint,ic_rbassoc->resval.rpoint); 
                ic_assoc(ent,70);
                if (ic_rbassoc->resval.rint & (IC_PLINE_CLOSED | IC_PLINE_CLOSEDN)) //if CLOSED
					{
					//get points
                    if ((ret=ic_divmeas(ename2,segs,NULL,1.0,0.0,0,0,&ptll2))!=0) {rc=ret;goto bail;} 
                    if ((ret=cmd_reverse_link_list(&ptll2))!=RTNORM) {rc=RTERROR;goto bail;} 
                    //make points wcs
                    for (trb=ptll2;trb!=NULL;trb=trb->rbnext) 
						{
                        sds_trans(trb->resval.rpoint,&ecs,&wcs,0,trb->resval.rpoint); 
						}
                    openflag2=0;
					}
				else
					{
                    //get points
                    if ((ret=ic_divmeas(ename2,segs,NULL,1.0,0.0,0,0,&ptll2))!=0) {rc=ret;goto bail;} 
                    db_getendpts(ename2,pttmp);
                    //trans endpts and pick to ucs
                    sds_trans(pttmp[0],&ecs,&ucs,0,spt2);
                    sds_trans(pttmp[1],&ecs,&ucs,0,ept2);
                    gra_ucs2rp(spt2,spt2,view);
                    gra_ucs2rp(ept2,ept2,view);
                    gra_ucs2rp(pt2,pt2,view);
                    //find distance
                    dist1=cmd_dist2d(spt2,pt2);
                    dist2=cmd_dist2d(ept2,pt2);
                    //reverse points
                    if (dist1>dist2)
						{ 
                        ic_ptcpy(tpt,pttmp[1]);
                        ic_ptcpy(pttmp[1],pttmp[0]);
                        ic_ptcpy(pttmp[0],tpt);
                        if ((ret=cmd_reverse_link_list(&ptll2))!=RTNORM) {rc=RTERROR;goto bail;} 
						}
                    //make points wcs
                    sds_trans(pttmp[0],&ecs,&wcs,0,spt2);
                    sds_trans(pttmp[1],&ecs,&wcs,0,ept2);
                    for (trb=ptll2;trb!=NULL;trb=trb->rbnext) 
						{
                        sds_trans(trb->resval.rpoint,&ecs,&wcs,0,trb->resval.rpoint); 
						}
                    openflag2=1;
					}   
				}
            break;
			}
		else if (strisame("LWPOLYLINE"/*DNT*/  ,ic_rbassoc->resval.rstring))
			{
            ic_assoc(ent,70);
			//get 210
            ic_assoc(ent,210);
            ic_ptcpy(ecs.resval.rpoint,ic_rbassoc->resval.rpoint); 
            ic_assoc(ent,70);
            if (ic_rbassoc->resval.rint & IC_PLINE_CLOSED) //if CLOSED
				{
				//get points
                if ((ret=ic_divmeas(ename2,segs,NULL,1.0,0.0,0,0,&ptll2))!=0) {rc=ret;goto bail;} 
                if ((ret=cmd_reverse_link_list(&ptll2))!=RTNORM) {rc=RTERROR;goto bail;} 
                //make points wcs
                for (trb=ptll2;trb!=NULL;trb=trb->rbnext) 
					{
                    sds_trans(trb->resval.rpoint,&ecs,&wcs,0,trb->resval.rpoint); 
					}
                openflag2=0;
				}
			else
				{
                //get points
                if ((ret=ic_divmeas(ename2,segs,NULL,1.0,0.0,0,0,&ptll2))!=0) {rc=ret;goto bail;} 
                db_getendpts(ename2,pttmp);
                //trans endpts and pick to ucs
                sds_trans(pttmp[0],&ecs,&ucs,0,spt2);
                sds_trans(pttmp[1],&ecs,&ucs,0,ept2);
                gra_ucs2rp(spt2,spt2,view);
                gra_ucs2rp(ept2,ept2,view);
                gra_ucs2rp(pt2,pt2,view);
                //find distance
                dist1=cmd_dist2d(spt2,pt2);
                dist2=cmd_dist2d(ept2,pt2);
                //reverse points
                if (dist1>dist2)
					{ 
                    ic_ptcpy(tpt,pttmp[1]);
                    ic_ptcpy(pttmp[1],pttmp[0]);
                    ic_ptcpy(pttmp[0],tpt);
                    if ((ret=cmd_reverse_link_list(&ptll2))!=RTNORM) {rc=RTERROR;goto bail;} 
					}
                //make points wcs
                sds_trans(pttmp[0],&ecs,&wcs,0,spt2);
                sds_trans(pttmp[1],&ecs,&wcs,0,ept2);
                for (trb=ptll2;trb!=NULL;trb=trb->rbnext) 
					{
                    sds_trans(trb->resval.rpoint,&ecs,&wcs,0,trb->resval.rpoint); 
					}
                openflag2=1;
				}   
            break;
			}
		else if (strisame("CIRCLE"/*DNT*/  ,ic_rbassoc->resval.rstring))
			{
            //set ecs direction
            ic_assoc(ent,210);
            ic_ptcpy(ecs.resval.rpoint,ic_rbassoc->resval.rpoint); 
         	//get points
            if ((ret=ic_divmeas(ename2,segs,NULL,1.0,0.0,0,0,&ptll2))!=0) {rc=ret;goto bail;} 
            //make points wcs
            for (trb=ptll2;trb!=NULL;trb=trb->rbnext)
                sds_trans(trb->resval.rpoint,&ecs,&wcs,0,trb->resval.rpoint); 
            openflag2=0;    
            break;         
	        }
		else if (strisame("ELLIPSE"/*DNT*/  ,ic_rbassoc->resval.rstring)) {
            if ((ret=ic_divmeas(ename2,segs,NULL,1.0,0.0,0,0,&ptll2))!=0) {
				rc=ret;
				goto bail;
			} 

            ic_assoc(ent,41);
			//not using icadAngleEqual, because it must be 0.0, not 2pi
            double start = ic_rbassoc->resval.rreal; 
            ic_assoc(ent,42);
            double end = ic_rbassoc->resval.rreal;
			if (icadRealEqual(end - start, IC_TWOPI)) 
                openflag2=0;
			else {
                db_getendpts(ename2,pttmp);
                //trans endpts and pick to ucs
                sds_trans(pttmp[0],&wcs,&ucs,0,spt2);
                sds_trans(pttmp[1],&wcs,&ucs,0,ept2);
                gra_ucs2rp(spt2,spt2,view);
                gra_ucs2rp(ept2,ept2,view);
                gra_ucs2rp(pt2,pt2,view);
                //find distance
                dist1=cmd_dist2d(spt2,pt2);
                dist2=cmd_dist2d(ept2,pt2);
                //reverse points
                if (dist1 > dist2) {
					ic_ptcpy(tpt,pttmp[1]);
                    ic_ptcpy(pttmp[1],pttmp[0]);
                    ic_ptcpy(pttmp[0],tpt);
                    if ((ret=cmd_reverse_link_list(&ptll2))!=RTNORM) {
						rc=RTERROR;
						goto bail;
					} 
				}
                //set spt and ept
                ic_ptcpy(spt2,pttmp[0]);
				ic_ptcpy(ept2,pttmp[1]);
                openflag1=2;
			}
            break;
		}
		else if (strisame("SPLINE"/*DNT*/, ic_rbassoc->resval.rstring)) {
            if ((ret=ic_divmeas(ename2,segs,NULL,1.0,0.0,0,0,&ptll2))!=0) {
				rc=ret;
				goto bail;
			} 

			db_spline* pSpline = (db_spline*)ename2[0];
            if (pSpline->isClosed() || pSpline->isPeriodic()) 
                openflag2=0;
			else {
                db_getendpts(ename2,pttmp);
                //trans endpts and pick to ucs
                sds_trans(pttmp[0],&wcs,&ucs,0,spt2);
                sds_trans(pttmp[1],&wcs,&ucs,0,ept2);
                gra_ucs2rp(spt2,spt2,view);
                gra_ucs2rp(ept2,ept2,view);
                gra_ucs2rp(pt2,pt2,view);
                //find distance
                dist1=cmd_dist2d(spt2,pt2);
                dist2=cmd_dist2d(ept2,pt2);
                //reverse points
                if (dist1 > dist2) {
					ic_ptcpy(tpt,pttmp[1]);
                    ic_ptcpy(pttmp[1],pttmp[0]);
                    ic_ptcpy(pttmp[0],tpt);
                    if ((ret=cmd_reverse_link_list(&ptll2))!=RTNORM) {
						rc=RTERROR;
						goto bail;
					} 
				}
                //set spt and ept
                ic_ptcpy(spt2,pttmp[0]);
				ic_ptcpy(ept2,pttmp[1]);
                openflag2=1;
            }

			break;
		}
		else {
			cmd_ErrorPrompt(CMD_ERR_ENTTYPE,0);
			sds_redraw(ename2,IC_REDRAW_UNHILITE);
			IC_RELRB(ent);
		}
	}
    IC_RELRB(ent);

    //check for incompatable types
    if ((openflag1==0 && openflag2==1) || (openflag1==1 && openflag2==0))
		{
        cmd_ErrorPrompt(CMD_ERR_MIXCLOSEOPEN,0);
        rc=RTNORM;
        goto bail;
		}
    if (openflag1==2 && openflag2==2)
		{
    	cmd_ErrorPrompt(CMD_ERR_2PTSURFACE,0);
        rc=RTNORM;
        goto bail;
		}

    sds_entmake(NULL);
    //build pline header  
    //get the var that holds the number of surfaces to draw
    if ((ret=SDS_getvar(NULL,DB_QELEVATION,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM) {rc=ret;goto bail;} 
    pt1[2]=setgetrb.resval.rint;

    pclose=16;
    if (openflag1==0)
		pclose=48;
    if (openflag2==0)
		pclose=48;
    
    if ((pline=sds_buildlist(RTDXF0,"POLYLINE"/*DNT*/,
                                66,1,                       
                                10,pt1, //use z value
                                70,pclose,
                                71,2,
                                72,((pclose==16)?segs+1:segs),
                                 0))==NULL) 
		{
        cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);            
        rc=RTERROR; goto bail;                                    
		}                                                           
    if ((ret=sds_entmake(pline))!=RTNORM) {rc=ret;goto bail;} 
    IC_RELRB(pline);

    if ((pline=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,
                                10,pt1,  //dummy point
                                70,64,
                                 0))==NULL) 
		{
        cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);            
        rc=RTERROR; goto bail;                                    
		}                                 
    ic_assoc(pline,10);          
	pp1=&ic_rbassoc->resval.rpoint;

    //make first entity
    if (openflag1==2) 
		{
        //make set of points on the point                          
        ic_ptcpy(*pp1,spt1);
        for (i=0; i<((pclose==16)?segs+1:segs); i++) 
			{
            if ((ret=sds_entmake(pline))!=RTNORM) {rc=ret;goto bail;}         	
			}                                                                    
		}
	else
		{
        if (openflag1) 
			{
            //start point                                                
            ic_ptcpy(*pp1,spt1);                                     
            if ((ret=sds_entmake(pline))!=RTNORM) {rc=ret;goto bail;}     
			}
        //make set of points on the path                     
        for (trb=ptll1; trb!=NULL; trb=trb->rbnext) 
			{
            ic_ptcpy(*pp1,trb->resval.rpoint);                          
            if ((ret=sds_entmake(pline))!=RTNORM) {rc=ret;goto bail;}    
			}
        if (openflag1)
			{
            //end point                                                
            ic_ptcpy(*pp1,ept1);                                     
            if ((ret=sds_entmake(pline))!=RTNORM) {rc=ret;goto bail;}     
			}
		}
    
    //make second entity
    if (openflag2==2) 
		{
        //make set of points on the point                          
        ic_ptcpy(*pp1,spt2);
        for (i=0; i<((pclose==16)?segs+1:segs); i++) 
			{
            if ((ret=sds_entmake(pline))!=RTNORM) {rc=ret;goto bail;}         	
			}                                                                    
		}
	else
		{
        if (openflag2) 
			{
            //start point                                                
            ic_ptcpy(*pp1,spt2);                                     
            if ((ret=sds_entmake(pline))!=RTNORM) {rc=ret;goto bail;}     
			}
        //make set of points on the path                     
        for (trb=ptll2; trb!=NULL; trb=trb->rbnext) 
			{
            ic_ptcpy(*pp1,trb->resval.rpoint);                          
            if ((ret=sds_entmake(pline))!=RTNORM) {rc=ret;goto bail;}    
			}
        if (openflag2)
			{
            //end point                                                
            ic_ptcpy(*pp1,ept2);                                     
            if ((ret=sds_entmake(pline))!=RTNORM) {rc=ret;goto bail;}     
			}
		}
    IC_RELRB(pline);
        
    //build the seqend
    if ((pline=sds_buildlist(RTDXF0,"SEQEND"/*DNT*/,
                                 0))==NULL) 
		{
        cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);                     
        rc=RTERROR;
		goto bail;                                           
		}                                                                
    if ((ret=sds_entmake(pline))!=RTNORM) {rc=ret;goto bail;}         

    rc=RTNORM;
bail:
    sds_entmake(NULL);
    IC_RELRB(ent);
    IC_RELRB(pline);
    IC_RELRB(ptll1);
    IC_RELRB(ptll2);
    if (ename1[0])
		sds_redraw(ename1,IC_REDRAW_UNHILITE);
    if (ename2[0])
		sds_redraw(ename2,IC_REDRAW_UNHILITE);
    return(rc);
}

//===============================================================================================

//
short cmd_tabsurf_waves(sds_name ent,sds_point vpt,bool closeflag,bool ecs1flag,bool reverseflag){
    RT ret,rc,flag70;
    sds_point *pp1,pttmp[2],pt1={0.0,0.0,0.0};
    struct resbuf *mesh=NULL,setgetrb,*ptll=NULL,*trb=NULL,*pRbb=NULL,*pRb,wcs,ecs;
    int segs;

    wcs.restype=RTSHORT;
    wcs.resval.rint=0;
    ecs.restype=RT3DPOINT;

    if ((ret=SDS_getvar(NULL,DB_QSURFTAB1,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM) {rc=ret;goto bail;} 
    segs=setgetrb.resval.rint;

    //if ecs get 210
    if (ecs1flag){
        //get the entity
        pRbb=sds_entget(ent);
        for (pRb=pRbb;(pRb!=NULL)&&(pRb->restype!=210);pRb=pRb->rbnext);
        if (pRb!=NULL) {
            ic_ptcpy(ecs.resval.rpoint,pRb->resval.rpoint);
        }
        IC_RELRB(pRbb);
    }

    sds_entmake(NULL);
    //build pline header  
    if (closeflag){
        flag70=48;
    }else{
        flag70=16;
    }
    if ((mesh=sds_buildlist(RTDXF0,"POLYLINE"/*DNT*/,
                                66,1,                       
                                10,pt1, //use z value
                                70,flag70,
                                71,2,
                                72,segs,
                                 0))==NULL) {
        cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);            
        rc=RTERROR; goto bail;                                    
    }                                                           
    if ((ret=sds_entmake(mesh))!=RTNORM) {rc=ret;goto bail;} 
    IC_RELRB(mesh);

    if ((mesh=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,
                                10,pt1,  //dummy point
                                70,64,
                                 0))==NULL) {
        cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);             
        rc=RTERROR; goto bail;                                    
    }                                 
    pp1=&mesh->rbnext->resval.rpoint;
    
    //if this entity is not closed some things need to be adjusted
    if (closeflag==0){
    	if ((ret=ic_divmeas(ent,segs-1,NULL,1.0,0.0,0,0,&ptll))!=0) {rc=ret;goto bail;} 
	
        //find start and end points
        if ((ret=db_getendpts(ent,pttmp))!=0) {rc=ret;goto bail;} 
    
        //check for reverse flag
        if (reverseflag){
            cmd_reverse_link_list(&ptll);            
            ic_ptcpy(pt1,pttmp[0]);
            ic_ptcpy(pttmp[0],pttmp[1]);
            ic_ptcpy(pttmp[1],pt1);
        }

        //start point
    	if (ecs1flag) sds_trans(pttmp[0],&ecs,&wcs,0,pttmp[0]);
        ic_ptcpy(*pp1,pttmp[0]);
        if ((ret=sds_entmake(mesh))!=RTNORM) {rc=ret;goto bail;}   
	
    	//make first set of points on the curv path
        for (trb=ptll; trb!=NULL; trb=trb->rbnext) {
        	if (ecs1flag) sds_trans(trb->resval.rpoint,&ecs,&wcs,0,trb->resval.rpoint);
            ic_ptcpy(*pp1,trb->resval.rpoint);
            if ((ret=sds_entmake(mesh))!=RTNORM) {rc=ret;goto bail;}   
    
        }
    	//end point
    	if (ecs1flag) sds_trans(pttmp[1],&ecs,&wcs,0,pttmp[1]);
    	ic_ptcpy(*pp1,pttmp[1]);                                            
        if ((ret=sds_entmake(mesh))!=RTNORM) {rc=ret;goto bail;}       
	
        //start point + vector
    	pttmp[0][0]+=vpt[0];          
    	pttmp[0][1]+=vpt[1];          
    	pttmp[0][2]+=vpt[2];          
    	ic_ptcpy(*pp1,pttmp[0]);
        if ((ret=sds_entmake(mesh))!=RTNORM) {rc=ret;goto bail;}   
	
    	//now build the mesh to the second entity using the point vector
        for (trb=ptll; trb!=NULL; trb=trb->rbnext) {
            pt1[0]=trb->resval.rpoint[0]+vpt[0];
            pt1[1]=trb->resval.rpoint[1]+vpt[1];
            pt1[2]=trb->resval.rpoint[2]+vpt[2];
            ic_ptcpy(*pp1,pt1);
            if ((ret=sds_entmake(mesh))!=RTNORM) {rc=ret;goto bail;}   
        }
    
    	//end point
    	pttmp[1][0]+=vpt[0];      
    	pttmp[1][1]+=vpt[1];      
    	pttmp[1][2]+=vpt[2];      
    	ic_ptcpy(*pp1,pttmp[1]);                                            
        if ((ret=sds_entmake(mesh))!=RTNORM) {rc=ret;goto bail;}       
    
    }else{     //if this entity is not closed some things need to be adjusted
     	
     	if ((ret=ic_divmeas(ent,segs,NULL,1.0,0.0,0,0,&ptll))!=0) {rc=ret;goto bail;} 
	
    	if (reverseflag) cmd_reverse_link_list(&ptll);
        
        //make first set of points on the curv path
        for (trb=ptll; trb!=NULL; trb=trb->rbnext) {
        	if (ecs1flag) sds_trans(trb->resval.rpoint,&ecs,&wcs,0,trb->resval.rpoint);
            ic_ptcpy(*pp1,trb->resval.rpoint);
            if ((ret=sds_entmake(mesh))!=RTNORM) {rc=ret;goto bail;}   
        }
	
    	//now build the mesh to the second entity using the point vector
        for (trb=ptll; trb!=NULL; trb=trb->rbnext) {
            pt1[0]=trb->resval.rpoint[0]+vpt[0];
            pt1[1]=trb->resval.rpoint[1]+vpt[1];
            pt1[2]=trb->resval.rpoint[2]+vpt[2];
            ic_ptcpy(*pp1,pt1);
            if ((ret=sds_entmake(mesh))!=RTNORM) {rc=ret;goto bail;}   
        }
    }
    IC_RELRB(mesh);
    IC_RELRB(ptll);
        
    //build the seqend
    if ((mesh=sds_buildlist(RTDXF0,"SEQEND"/*DNT*/,
                                 0))==NULL) {                        
        cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);                      
        rc=RTERROR; goto bail;                                           
    }                                                                
    if ((ret=sds_entmake(mesh))!=RTNORM) {rc=ret;goto bail;}         
    IC_RELRB(mesh);                                                  

    rc=RTNORM;
bail:
 sds_entmake(NULL);
 IC_RELRB(mesh);
 IC_RELRB(ptll);      
 return(rc);
}
 
//this is for plines that are not spline or curve fit
short cmd_tabsurf_nonspline(sds_name ent,sds_point vpt,bool ecs1flag,bool reverseflag)
	{
    struct resbuf setgetrb,*mesh=NULL,*pRbb=NULL,*pRb,ecs,wcs;
    struct resbuf *ptll=NULL,*tmpll=NULL;
    long vcount=0;
    short segs,flag70;
    RT	ret,rc;
	sds_point pt1={0.0,0.0,0.0},*pp1;
    double bulge=0.0;
	bool islwpl=false;

    ecs.restype=RT3DPOINT;
    wcs.restype=RTSHORT;
    wcs.restype=0;

    sds_entmake(NULL);
    
    if ((ret=SDS_getvar(NULL,DB_QSURFTAB1,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM) {rc=ret;goto bail;} 
    segs=setgetrb.resval.rint;
        
    //get first entity
    pRbb=sds_entget(ent);
    //make sure it is a polyline
    pRb=ic_ret_assoc(pRbb,0);
    if (!strisame("POLYLINE"/*DNT*/ ,pRb->resval.rstring) && !strisame("LWPOLYLINE"/*DNT*/, pRb->resval.rstring))
		{
        rc=RTERROR; 
		goto bail;
		}   
    if (strisame("LWPOLYLINE"/*DNT*/ , pRb->resval.rstring))
		islwpl=true;
	
	//check the closed flag
    pRb=ic_ret_assoc(pRbb,70);
	if (pRb->resval.rint & IC_PLINE_CLOSED)
        flag70=48;
    else
        flag70=16;
    //if ecs get 210
    if (ecs1flag)
		{
        for (pRb=pRbb; pRb!=NULL && pRb->restype!=210; pRb=pRb->rbnext);
        ic_ptcpy(ecs.resval.rpoint,pRb->resval.rpoint);
		}

	if (islwpl)
		{
		pRb=ic_ret_assoc(pRbb,38);
		pt1[2]=pRb->resval.rreal;
		}
	else
		{
		//get the elevation of the polyline
		for (pRb=pRbb; pRb!=NULL && pRb->restype!=10; pRb=pRb->rbnext);
		pt1[2]=pRb->resval.rpoint[2];    
		}

    IC_RELRB(pRbb);

    //get the points for a non-fit pline
    if ((ret=cmd_divmeas_pline_pts(ent,segs,&vcount,&ptll))!=RTNORM) {rc=ret; goto bail;}

    //now build the mesh 
    if ((mesh=sds_buildlist(RTDXF0,"POLYLINE"/*DNT*/,
                                66,1,                       
                                10,pt1, //use z value
                                70,flag70,
                                71,2,
                                72,vcount,
                                 0))==NULL) 
		{
        cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);            
        rc=RTERROR;
		goto bail;                                    
		}                                                           
    if ((ret=sds_entmake(mesh))!=RTNORM) {rc=ret;goto bail;} 
    IC_RELRB(mesh);

    if ((mesh=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,
                                10,pt1,  //dummy point
                                70,64,
                                 0))==NULL) 
		{
        cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);             
        rc=RTERROR;
		goto bail;                                    
		}                                 
    pp1=&mesh->rbnext->resval.rpoint;
    //check for reverse flag
    if (reverseflag)
		cmd_reverse_link_list(&ptll);
    //copy first set of points
    for (tmpll=ptll;tmpll!=NULL;tmpll=tmpll->rbnext)
		{
        if (ecs1flag)
            sds_trans(tmpll->resval.rpoint,&ecs,&wcs,0,*pp1);
		else
            ic_ptcpy(*pp1,tmpll->resval.rpoint);
        if ((ret=sds_entmake(mesh))!=RTNORM) {rc=ret;goto bail;}
	    }

    //again with the vector offset for the second set
    for (tmpll=ptll;tmpll!=NULL;tmpll=tmpll->rbnext)
		{
        if (ecs1flag)
            sds_trans(tmpll->resval.rpoint,&ecs,&wcs,0,*pp1);
        else
            ic_ptcpy(*pp1,tmpll->resval.rpoint);
        cmd_pt_add(*pp1,vpt,*pp1);
        if ((ret=sds_entmake(mesh))!=RTNORM) {rc=ret;goto bail;}
	    }
    IC_RELRB(mesh);                                                  

    //build the seqend
    if ((mesh=sds_buildlist(RTDXF0,"SEQEND"/*DNT*/,
                                 0))==NULL) 
		{
        cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);                      
        rc=RTERROR;
		goto bail;                                           
		}                                                                
    if ((ret=sds_entmake(mesh))!=RTNORM) {rc=ret;goto bail;}         

    rc=RTNORM;
bail:
    IC_RELRB(mesh);
    IC_RELRB(ptll);
    return(rc);   
}

static
RT cmd_tabsurf_acis()
{
    if (!CModeler::get()->canModify())
        return RTERROR;

    RT rc = RTERROR;
    sds_name ename1 = {0,0}, ename2 = {0,0};

    sds_point pt;
    for (;;)
	{
        for (;;)
		{
            RT ret=sds_entsel(ResourceString(IDC_SURFACES__NSELECT_ENTIT_16, "\nSelect entity to extrude: " ), ename1, pt);
            if (ret==RTERROR)
				continue;
            if (ret==RTNORM)
			{
                sds_redraw(ename1,IC_REDRAW_HILITE);
                break;
			}
			else
			{
                rc=ret;
				goto bail;
			}
		}
        db_handitem* item = reinterpret_cast<db_handitem*>(ename1[0]);
        if (isKindOf(item, DB_LINE, DB_ARC, DB_ELLIPSE, DB_CIRCLE, DB_POLYLINE, DB_LWPOLYLINE, DB_SPLINE, -1))
            break;

	    sds_redraw(ename1,IC_REDRAW_UNHILITE);
    }

    for (;;)
	{
        for (;;)
		{
            RT ret=sds_entsel(ResourceString(IDC_SURFACES__NSELECT_LINE__17, "\nSelect line or open polyline for extrusion path: " ), ename2, pt);
            if (ret==RTERROR)
				continue;
            if (ret==RTNORM)
			{
                sds_redraw(ename2,IC_REDRAW_HILITE);
                break;
			}
			else
			{
                rc=ret;
				goto bail;
			}
		}
        db_handitem* item = reinterpret_cast<db_handitem*>(ename2[0]);
        if (isKindOf(item, DB_LINE, DB_ARC, DB_ELLIPSE, DB_CIRCLE, DB_POLYLINE, DB_LWPOLYLINE, DB_SPLINE, -1))
            break;

	    sds_redraw(ename2,IC_REDRAW_UNHILITE);
    }

    do
    {
        using namespace geodata;
        
        curve* profile;
        if (!extract(*reinterpret_cast<db_handitem*>(ename1[0]), profile))
            break;
        auto_ptr<curve> hprofile(profile);

        curve* path;
        if (!extract(*reinterpret_cast<db_handitem*>(ename2[0]), path))
            break;
        auto_ptr<curve> hpath(path);

		CDbAcisEntityData body;
        if (CModeler::get()->createSurfaceExtruded(*profile, *path, getCurrColorIndex(), body))
		{
			if (create(body))
				rc = RTNORM;
		}
    }
    while (false);

bail:
    if (ename1[0])
		sds_redraw(ename1,IC_REDRAW_UNHILITE);
    if (ename2[0])
		sds_redraw(ename2,IC_REDRAW_UNHILITE);
    return rc;
}

//===================================================================================================

//ucs friendly as of 7/9/97
short cmd_tabsurf(void) 
{
    if (cmd_iswindowopen() == RTERROR)
		return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    //<ALM>
    if (CModeler::get()->canModify())
        return cmd_tabsurf_acis();
    //</ALM>
	else
		return cmd_tabsurf_mesh();
}

short cmd_tabsurf_mesh(void) 
{
    if (cmd_iswindowopen() == RTERROR)
		return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    bool ecs1flag,ecs2flag,closeflag,splineflag=1,reverseflag=0;
    sds_name ename1,ename2;
    sds_point pt1,vpt,pttmp[2];
    struct resbuf ecs,wcs,ucs,*pRbb=NULL,*pRb;
    RT rc,ret;
    sds_real dist1,dist2;
	struct gra_view *view=SDS_CURGRVW;

	ename1[0]=ename1[1]=0L;
	ename2[0]=ename2[1]=0L;
    ecs.restype=RT3DPOINT;
    ucs.restype=RTSHORT;
    ucs.resval.rint=1;
    wcs.restype=RTSHORT;
    wcs.resval.rint=0;

    //get the first entity
    for (;;)
		{
        if ((ret=sds_entsel(ResourceString(IDC_SURFACES__NSELECT_ENTIT_16, "\nSelect entity to extrude: " ) ,ename1,pt1))!=RTNORM) {rc=ret;goto bail;}
        sds_redraw(ename1,IC_REDRAW_HILITE);

        if ((pRbb=sds_entget(ename1))==NULL) {rc=RTERROR;goto bail;}
        for (pRb=pRbb;(pRb!=NULL)&&(pRb->restype!=0);pRb=pRb->rbnext);
        if (strisame("ARC"/*DNT*/  ,pRb->resval.rstring))
			{
            closeflag=0; 
            ecs1flag=1;
            splineflag=1;
            db_getendpts(ename1,pttmp);
            break;
			}
		else if (strisame(db_hitype2str(DB_LINE),pRb->resval.rstring))
			{
            closeflag=0; 
            ecs1flag=0;
            splineflag=1;
            db_getendpts(ename1,pttmp);
            break;
			}
		else if (strisame("POLYLINE"/*DNT*/  ,pRb->resval.rstring))
			{
            for (pRb=pRbb;(pRb!=NULL)&&(pRb->restype!=70);pRb=pRb->rbnext);
            //check for mesh
			if (pRb->resval.rint & (IC_PLINE_POLYFACEMESH | IC_PLINE_CLOSEDN | IC_PLINE_3DMESH))
				{
                cmd_ErrorPrompt(CMD_ERR_ENTTYPE,0);                          
                sds_redraw(ename1,IC_REDRAW_UNHILITE);
                IC_RELRB(pRbb);
                continue;
            }
            //check for spline or fit
            if (pRb->resval.rint & (IC_PLINE_SPLINEFIT | IC_PLINE_CURVEFIT))
                splineflag=1;
            else
                splineflag=0;
            //check for closed
            if (pRb->resval.rint & IC_PLINE_CLOSED)
                closeflag=1;
			else
				{
                closeflag=0;
                db_getendpts(ename1,pttmp);
            	}   
            //check for 2D or 3D
            if (pRb->resval.rint & IC_PLINE_3DPLINE)
                ecs1flag=0;
            else
                ecs1flag=1;
            break;
			}
		else if (strisame("LWPOLYLINE"/*DNT*/  ,pRb->resval.rstring))
			{
            for (pRb=pRbb;(pRb!=NULL)&&(pRb->restype!=70);pRb=pRb->rbnext);
            splineflag=0;
            //check for closed
            if (pRb->resval.rint & IC_PLINE_CLOSED)
                closeflag=1;
			else
				{
                closeflag=0;
                db_getendpts(ename1,pttmp);
            	}   
            ecs1flag=1;
            break;
			}
		else if (strisame("CIRCLE"/*DNT*/  ,pRb->resval.rstring))
			{
            closeflag=1;
            ecs1flag=1;
            splineflag=1;
            break;

			}
		else if (strisame("ELLIPSE"/*DNT*/, pRb->resval.rstring))
			{
			db_ellipse* pEllipse = (db_ellipse*)ename1[0];

			double start, end;
			pEllipse->get_41(&start);
			pEllipse->get_42(&end);

            if (icadRealEqual(end - start,IC_TWOPI)) 
                closeflag=1;
			else {
                closeflag=0;
                db_getendpts(ename1,pttmp);
            }
            ecs1flag=0;
            splineflag=1;
            break;
			}
		else if (strisame("SPLINE"/*DNT*/, pRb->resval.rstring))
			{
			db_spline* pSpline = (db_spline*)ename1[0];
            if (pSpline->isClosed() || pSpline->isPeriodic()) 
                closeflag=1;
			else {
                closeflag=0;
                db_getendpts(ename1,pttmp);
            }
            ecs1flag=0;
            splineflag=1;
			break;
			}
		else
			{
            cmd_ErrorPrompt(CMD_ERR_ENTTYPE,0); 
            sds_redraw(ename1,IC_REDRAW_UNHILITE);
            IC_RELRB(pRbb);
	        }
		}          
    IC_RELRB(pRbb);

    //check the pick point
    if (!closeflag)
		{
        if (ecs1flag)
			{
            sds_trans(pttmp[0],&ecs,&ucs,0,pttmp[0]);
            sds_trans(pttmp[1],&ecs,&ucs,0,pttmp[1]);
			}
		else
			{
            sds_trans(pttmp[0],&wcs,&ucs,0,pttmp[0]);
            sds_trans(pttmp[1],&wcs,&ucs,0,pttmp[1]);
			}
    	gra_ucs2rp(pt1,pt1,view);
        gra_ucs2rp(pttmp[0],pttmp[0],view);
        gra_ucs2rp(pttmp[1],pttmp[1],view);
        dist1=cmd_dist2d(pttmp[0],pt1);
        dist2=cmd_dist2d(pttmp[1],pt1);
        if (dist1>dist2)
            reverseflag=1;
        else
            reverseflag=0;
	    }    
    //get the second entity and make sure it is only a line or open polyline
    for (;;)
		{
        if ((ret=sds_entsel(ResourceString(IDC_SURFACES__NSELECT_LINE__17, "\nSelect line or open polyline for extrusion path: " ) ,ename2,pt1))!=RTNORM) {rc=ret;goto bail;}
        sds_redraw(ename2,IC_REDRAW_HILITE);

        if ((pRbb=sds_entget(ename2))==NULL) {rc=RTERROR;goto bail;}               
        for (pRb=pRbb;(pRb!=NULL)&&(pRb->restype!=0);pRb=pRb->rbnext);
        if (strisame(db_hitype2str(DB_LINE),pRb->resval.rstring))
			{
            ecs2flag=0;
            for (pRb=pRbb;(pRb!=NULL)&&(pRb->restype!=10);pRb=pRb->rbnext);
            ic_ptcpy(pttmp[0],pRb->resval.rpoint);
            for (pRb=pRbb;(pRb!=NULL)&&(pRb->restype!=11);pRb=pRb->rbnext);
            ic_ptcpy(pttmp[1],pRb->resval.rpoint);
            break;
			}
		else if (strisame("POLYLINE"/*DNT*/  ,pRb->resval.rstring))
			{
            for (pRb=pRbb;(pRb!=NULL)&&(pRb->restype!=70);pRb=pRb->rbnext);
            if (pRb->resval.rint & (IC_PLINE_POLYFACEMESH | IC_PLINE_CLOSEDN | IC_PLINE_3DMESH | IC_PLINE_CLOSED))
				{
                sds_redraw(ename2,IC_REDRAW_UNHILITE);
                cmd_ErrorPrompt(CMD_ERR_ENTTYPE,0);                          
                IC_RELRB(pRbb);
                continue;
				}
            if (pRb->resval.rint & IC_PLINE_3DPLINE)
                ecs2flag=0;
            else
				{
                ecs2flag=1;
                for (pRb=pRbb;(pRb!=NULL)&&(pRb->restype!=210);pRb=pRb->rbnext);
                ic_ptcpy(ecs.resval.rpoint,pRb->resval.rpoint);
				}
            db_getendpts(ename2,pttmp);
            break;
			}
		else if (strisame("LWPOLYLINE"/*DNT*/  ,pRb->resval.rstring))
			{
            for (pRb=pRbb; pRb!=NULL && pRb->restype!=70; pRb=pRb->rbnext);
            if (pRb->resval.rint & IC_PLINE_CLOSED)
				{
                sds_redraw(ename2,IC_REDRAW_UNHILITE);
                cmd_ErrorPrompt(CMD_ERR_ENTTYPE,0);                          
                IC_RELRB(pRbb);
                continue;
				}
            ecs2flag=1;
            for (pRb=pRbb; pRb!=NULL && pRb->restype!=210; pRb=pRb->rbnext);
            ic_ptcpy(ecs.resval.rpoint,pRb->resval.rpoint);
            db_getendpts(ename2,pttmp);
            break;
			}
		else
			{
            cmd_ErrorPrompt(CMD_ERR_ENTTYPE,0);  
            sds_redraw(ename2,IC_REDRAW_UNHILITE);
            IC_RELRB(pRbb);
			}
		}
    IC_RELRB(pRbb);

    //check and see if the vector is a line or pline
    vpt[0]=pttmp[1][0]-pttmp[0][0];        
    vpt[1]=pttmp[1][1]-pttmp[0][1];    
    vpt[2]=pttmp[1][2]-pttmp[0][2];    
    
    if (ecs2flag)
		{
        sds_trans(pttmp[0],&ecs,&ucs,0,pttmp[0]);
        sds_trans(pttmp[1],&ecs,&ucs,0,pttmp[1]);
		}
	else
		{
        sds_trans(pttmp[0],&wcs,&ucs,0,pttmp[0]);
        sds_trans(pttmp[1],&wcs,&ucs,0,pttmp[1]);
		}
    gra_ucs2rp(pttmp[0],pttmp[0],view);
    gra_ucs2rp(pttmp[1],pttmp[1],view);
    gra_ucs2rp(pt1,pt1,view);
    
//    gra_ucs2rp(pttmp[0],pttmp[0],view);
//    gra_ucs2rp(pttmp[1],pttmp[1],view);
    dist1=cmd_dist2d(pttmp[0],pt1);
    dist2=cmd_dist2d(pttmp[1],pt1);
    if (dist1>dist2)
		{
        vpt[0]=-vpt[0];    
        vpt[1]=-vpt[1];    
        vpt[2]=-vpt[2];    
	    }                                                
    if (ecs2flag) 
		sds_trans(vpt,&ecs,&wcs,1,vpt);

    if (splineflag)
        cmd_tabsurf_waves(ename1,vpt,closeflag,ecs1flag,reverseflag);
    else
        cmd_tabsurf_nonspline(ename1,vpt,ecs1flag,reverseflag);

    rc=RTNORM;
bail:    
    IC_RELRB(pRbb);
    if (ename1[0]) 
		sds_redraw(ename1,IC_REDRAW_UNHILITE);
    if (ename2[0]) 
		sds_redraw(ename2,IC_REDRAW_UNHILITE);
    return(rc); 
}


short cmd_divmeas_pline_pts(sds_name ent,short curvesegs,long *vcount,struct resbuf **result)
	{
    struct resbuf *pRbb=NULL,*pRb,*ptll=NULL,*tmpll;
    double sa,ea,rr,elev,inc,bulge=0.0;
    sds_point cc;
    RT	rc;
	int i;
    bool breakflg=false,closeflg=false,islwpl=false;

    (*vcount)=0;
    //get first entity
    pRbb=sds_entget(ent);
    //make sure it is a polyline
    for (pRb=pRbb;(pRb!=NULL)&&(pRb->restype!=0);pRb=pRb->rbnext);
    if (!strsame("POLYLINE"/*DNT*/,pRb->resval.rstring) && !strsame("LWPOLYLINE"/*DNT*/,pRb->resval.rstring))
		{
        rc=RTERROR;
		goto bail;
		}   
	if (strsame("LWPOLYLINE"/*DNT*/,pRb->resval.rstring))
		islwpl=true;
	
	//check for close
    for (pRb=pRbb;(pRb!=NULL)&&(pRb->restype!=70);pRb=pRb->rbnext);
    closeflg=(pRb->resval.rint & IC_PLINE_CLOSED);

	if (islwpl)
		{
		//get the elevation of the lwpolyline
		pRb=ic_ret_assoc(pRbb,38);
		if (pRb)
			elev=pRb->resval.rpoint[2];
		//loop through the vertices
		pRb=pRbb;
		for (;;)
			{
			pRb=ic_ret_assoc(pRb,10);
			//if a vertex
			if (pRb==NULL)
				{
				if (!icadRealEqual(bulge,0.0) && closeflg) //if closed and last ent has bulge then set flg and make a few more verts
					breakflg=true; 
				else
					break;
				}   
			//get the 10 point
			if (breakflg)
				{  //if breakflag then get first point to calc the bulge2arc for the last segment.
				for (pRb=pRbb; pRb!=NULL && pRb->restype!=10; pRb=pRb->rbnext);
				}
			//if the bulge flag then 
			if (!icadRealEqual(bulge,0.0))
				{
				ic_bulge2arc(tmpll->resval.rpoint,((breakflg)?ptll->resval.rpoint:pRb->resval.rpoint),bulge,cc,&rr,&sa,&ea);
				ic_normang(&sa,&ea);
				inc=(ea-sa)/curvesegs;
				for (i=1; i<curvesegs; i++)
					{
					//make a new link
					(*vcount)++;
					if (ptll==NULL)
					{
						ptll=tmpll= new struct resbuf;
						memset(ptll,0,sizeof(resbuf));
					}
					else
					{
						tmpll=tmpll->rbnext= new struct resbuf ;
						memset(tmpll,0,sizeof(resbuf));
					}
					tmpll->restype=RT3DPOINT;
					//copy the point
					if (bulge>0.0)
						{
						tmpll->resval.rpoint[0]=cc[0]+(rr*cos(sa+(i*inc)));
						tmpll->resval.rpoint[1]=cc[1]+(rr*sin(sa+(i*inc)));
						}
					else
						{
						tmpll->resval.rpoint[0]=cc[0]+(rr*cos(ea-(i*inc)));
						tmpll->resval.rpoint[1]=cc[1]+(rr*sin(ea-(i*inc)));
						}
					tmpll->resval.rpoint[2]=elev;
					}
				//make a new link
				if (!breakflg)
					{
					(*vcount)++;
					if (ptll==NULL)
					{
						ptll=tmpll= new struct resbuf ;
						memset(ptll,0,sizeof(resbuf));
					}
					else
					{
						tmpll=tmpll->rbnext= new struct resbuf;
						memset(tmpll,0,sizeof(resbuf));
					}
					tmpll->restype=RT3DPOINT;
					//copy the point
					tmpll->resval.rpoint[0]=pRb->resval.rpoint[0];
					tmpll->resval.rpoint[1]=pRb->resval.rpoint[1];
					tmpll->resval.rpoint[2]=pRb->resval.rpoint[2];
					}
				}
			else
				{
				//make a new link
				(*vcount)++;
				if (ptll==NULL)
				{
					ptll=tmpll= new struct resbuf ;
					memset(ptll,0,sizeof(resbuf));
				}
				else
				{
					tmpll=tmpll->rbnext= new struct resbuf ;
					memset(tmpll,0,sizeof(resbuf));
				}
				tmpll->restype=RT3DPOINT;
				//copy the point
				tmpll->resval.rpoint[0]=pRb->resval.rpoint[0];
				tmpll->resval.rpoint[1]=pRb->resval.rpoint[1];
				tmpll->resval.rpoint[2]=pRb->resval.rpoint[2];
				}
			if (breakflg)
				break;
			//get the bulge
			pRb=ic_ret_assoc(pRb,42);
			bulge=pRb->resval.rreal;
			}
		IC_RELRB(pRbb);
		*result=ptll;

		rc=RTNORM;
		}
	else
		{
		//get the elevation of the polyline
		for (pRb=pRbb;(pRb!=NULL)&&(pRb->restype!=10);pRb=pRb->rbnext);
		elev=pRb->resval.rpoint[2];
		IC_RELRB(pRbb);
		//loop through the vertices
		for (;;)
			{
			sds_entnext_noxref(ent,ent);
			pRbb=sds_entget(ent);
			for (pRb=pRbb;(pRb!=NULL)&&(pRb->restype!=0);pRb=pRb->rbnext);
			//if a vertex
			if (strsame("SEQEND"/*DNT*/  ,pRb->resval.rstring))
				{
				if (!icadRealEqual(bulge,0.0) && closeflg) //if closed and last ent has bulge then set flg and make a few more verts
					breakflg=true; 
				else
					break;
				}   
			//get the 10 point
			if (breakflg)
				{  //if breakflag then get first point to calc the bulge2arc for the last segment.
				for (pRb=pRbb;(pRb!=NULL)&&(pRb->restype!=10);pRb=pRb->rbnext);
				}
			//if the bulge flag then 
			if (!icadRealEqual(bulge,0.0))
				{
				ic_bulge2arc(tmpll->resval.rpoint,((breakflg)?ptll->resval.rpoint:pRb->resval.rpoint),bulge,cc,&rr,&sa,&ea);
				ic_normang(&sa,&ea);
				inc=(ea-sa)/curvesegs;
				for (i=1;i<curvesegs;i++)
					{
					//make a new link
					(*vcount)++;
					if (ptll==NULL)
					{
						ptll=tmpll= new struct resbuf ;
						memset(ptll,0,sizeof(resbuf));
					}
					else
					{
						tmpll=tmpll->rbnext= new struct resbuf;
						memset(tmpll,0,sizeof(resbuf));
					}
					tmpll->restype=RT3DPOINT;
					//copy the point
					if (bulge>0.0)
						{
						tmpll->resval.rpoint[0]=cc[0]+(rr*cos(sa+(i*inc)));
						tmpll->resval.rpoint[1]=cc[1]+(rr*sin(sa+(i*inc)));
						}
					else
						{
						tmpll->resval.rpoint[0]=cc[0]+(rr*cos(ea-(i*inc)));
						tmpll->resval.rpoint[1]=cc[1]+(rr*sin(ea-(i*inc)));
						}
					tmpll->resval.rpoint[2]=elev;
					}
				//make a new link
				if (!breakflg)
					{
					(*vcount)++;
					if (ptll==NULL)
					{
						ptll=tmpll= new struct resbuf ;
						memset(ptll,0,sizeof(resbuf));
					}
					else
					{
						tmpll=tmpll->rbnext= new struct resbuf ;
						memset(tmpll,0,sizeof(resbuf));
					}
					tmpll->restype=RT3DPOINT;
					//copy the point
					tmpll->resval.rpoint[0]=pRb->resval.rpoint[0];
					tmpll->resval.rpoint[1]=pRb->resval.rpoint[1];
					tmpll->resval.rpoint[2]=pRb->resval.rpoint[2];
					}
				}
			else
				{
				//make a new link
				(*vcount)++;
				if (ptll==NULL)
				{
					ptll=tmpll= new struct resbuf ;
					memset(ptll,0,sizeof(resbuf));
				}
				else
				{
					tmpll=tmpll->rbnext= new struct resbuf ;
					memset(tmpll,0,sizeof(resbuf));

				}
				tmpll->restype=RT3DPOINT;
				//copy the point
				tmpll->resval.rpoint[0]=pRb->resval.rpoint[0];
				tmpll->resval.rpoint[1]=pRb->resval.rpoint[1];
				tmpll->resval.rpoint[2]=pRb->resval.rpoint[2];
				}
			if (breakflg)
				break;
			//get the bulge
			for (pRb=pRbb; pRb!=NULL && pRb->restype!=42; pRb=pRb->rbnext);
			bulge=pRb->resval.rreal;
			IC_RELRB(pRbb);
			}
		*result=ptll;

		rc=RTNORM;
		}
bail:
    IC_RELRB(pRbb);
    return(rc);
}

short cmd_amess(sds_point pt1,sds_point pt2,sds_point pt3,sds_point pt4,int msize,int nsize){
	//points are passed in UCS coords, and ent made in WCS

    sds_point *pp1,ptn,ptm1,ptm2;
    struct resbuf *pline=NULL,rbucs,rbwcs;
    RT	rc,ret;
	int	i,j;
    sds_real dx,dy,dz;

	rbucs.restype=rbwcs.restype=RTSHORT;
	rbucs.rbnext=rbwcs.rbnext=NULL;
	rbucs.resval.rint=1;
	rbwcs.resval.rint=0;

    sds_entmake(NULL);
    //build pline header  
    if ((pline=sds_buildlist(RTDXF0,"POLYLINE"/*DNT*/,
                                66,1,                       
                                10,pt1,
                                70,16,
                                71,msize,
                                72,nsize,
                                 0))==NULL) {
        cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);            
        rc=RTERROR; goto bail;                                    
    }                                                           
    if ((ret=sds_entmake(pline))!=RTNORM) {rc=ret;goto bail;} 
    IC_RELRB(pline);
   
    //build the first point
    if ((pline=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,
                                10,pt1,  //dummy point
                                70,64,
                                 0))==NULL) {
        cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);            
        rc=RTERROR; goto bail;                                    
    }                                 
    
    //set pointer to the build list point
    ic_assoc(pline,10);          
	pp1=&ic_rbassoc->resval.rpoint;

	//loop thru the mesh
//	ic_ptcpy(ptn,pt1);
	ic_ptcpy(ptm1,pt1);
	ic_ptcpy(ptm2,pt2);
	for (i=0;i<msize;i++){
        dx=(ptm2[0]-ptm1[0])/(nsize-1);
        dy=(ptm2[1]-ptm1[1])/(nsize-1);
        dz=(ptm2[2]-ptm1[2])/(nsize-1);
        for (j=0;j<nsize;j++){
            ptn[0]=ptm1[0]+j*dx;
            ptn[1]=ptm1[1]+j*dy;
            ptn[2]=ptm1[2]+j*dz;
			sds_trans(ptn,&rbucs,&rbwcs,0,*pp1);
            if ((ret=sds_entmake(pline))!=RTNORM) {rc=ret;goto bail;}      
	    }
        
        ptm1[0]+=(pt4[0]-pt1[0])/(msize-1); 
        ptm1[1]+=(pt4[1]-pt1[1])/(msize-1); 
        ptm1[2]+=(pt4[2]-pt1[2])/(msize-1); 
        ptm2[0]+=(pt3[0]-pt2[0])/(msize-1);      
        ptm2[1]+=(pt3[1]-pt2[1])/(msize-1);      
        ptm2[2]+=(pt3[2]-pt2[2])/(msize-1);      
    }
    IC_RELRB(pline);

    //build the seqend
    if ((pline=sds_buildlist(RTDXF0,"SEQEND"/*DNT*/,
                                 0))==NULL) {                        
        cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);                     
        rc=RTERROR; goto bail;                                           
    }                                                                
    if ((ret=sds_entmake(pline))!=RTNORM) {rc=ret;goto bail;}         
    IC_RELRB(pline);                                                  

    rc=RTNORM;
bail:
 IC_RELRB(pline);
 return(rc);
}

short cmd_mesh(void) {

    if (cmd_iswindowopen() == RTERROR) return RTERROR;			

    RT rc,ret=0;
    sds_point pt1,pt2,pt3,pt4;
    int msize,nsize;

    if (sds_initget(1,NULL)!=RTNORM) {rc=ret;goto bail;}                 
    if ((ret=sds_getpoint(NULL,ResourceString(IDC_SURFACES__NFIRST_OF_FOU_19, "\nFirst of four corners for polygon mesh: " ) ,pt1))!=RTNORM) {rc=ret; goto bail;}
    if (sds_initget(1,NULL)!=RTNORM) {rc=ret;goto bail;}                                 
    if ((ret=sds_getpoint(pt1,ResourceString(IDC_SURFACES__NSECOND_CORNE_20, "\nSecond corner: " ) ,pt2))!=RTNORM) {rc=ret; goto bail;}
    if (sds_initget(1,NULL)!=RTNORM) {rc=ret;goto bail;}                                 
    if ((ret=sds_getpoint(pt2,ResourceString(IDC_SURFACES__NTHIRD_CORNER_21, "\nThird corner: " ) ,pt3))!=RTNORM) {rc=ret; goto bail;}
    if (sds_initget(1,NULL)!=RTNORM) {rc=ret;goto bail;}                                 
    if ((ret=sds_getpoint(pt3,ResourceString(IDC_SURFACES__NLAST_CORNER__22, "\nLast corner for mesh: " ) ,pt4))!=RTNORM) {rc=ret; goto bail;}
    while(1){
        if (sds_initget(1,NULL)!=RTNORM) {rc=ret;goto bail;}    
        if ((ret=sds_getint(ResourceString(IDC_SURFACES__NNUMBER_OF_DI_23, "\nNumber of divisions along primary axis (between 2 and 256): " ) ,&msize))!=RTNORM) {rc=ret; goto bail;}
        if (msize>=2&&msize<=256) break;
        cmd_ErrorPrompt(CMD_ERR_MESHSIZE,0);                     
    }
    while(1){
        if (sds_initget(1,NULL)!=RTNORM) {rc=ret;goto bail;}    
        if ((ret=sds_getint(ResourceString(IDC_SURFACES__NNUMBER_OF_DI_24, "\nNumber of divisions along secondary axis (between 2 and 256): " ) ,&nsize))!=RTNORM) {rc=ret; goto bail;}
        if (nsize>=2&&nsize<=256) break;
        cmd_ErrorPrompt(CMD_ERR_MESHSIZE,0);                     
    }
    if ((ret=cmd_amess(pt1,pt2,pt3,pt4,msize,nsize))!=RTNORM) {rc=ret; goto bail;} 
    
    rc=RTNORM;
bail:
    return(rc);
}

//<ALM>
// check entity type
bool isKindOf(db_handitem* pe, ...)
{
    va_list types;
    va_start(types, pe);

    int type = pe->ret_type();
    int itype;
    while ((itype = va_arg(types, int)) != -1)
    {
        if (type == itype)
            break;
    }

    va_end (types);

    return itype != -1;
}
//</ALM>
