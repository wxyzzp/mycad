/* POLYLINE.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * Supports the polyline and polygon commands
 *
 */
// THIS FILE HAS BEEN GLOBALIZED!

//** Includes
#include "cmds.h"/*DNT*/
#include "Icad.h" /*DNT*/
#include "commandqueue.h" /*DNT*/
#include "IcadApi.h"

// **********
// GLOBALS
//
extern double SDS_CorrectElevation;
extern bool	  SDS_ElevationChanged;
extern int SDS_PointDisplayMode;


// *************
// MODULE DATA
//
static struct cmd_pline_globalpacket {
    struct resbuf *arcdxf;
    sds_real stwid,endwid,bulge;
    short numents;
} cmd_pline_gpak;


static struct cmd_pline_list { //*** This structure is for the linked list that keeps track
                        //*** of all segments of the polyline.
    sds_name ename;     //*** Segment's ename.
    sds_point vertexpt; //*** Segment's vertex point.
    sds_real stwidth;   //*** Segment's start width.
    sds_real endwidth;  //*** Segment's ending width.
    sds_real bulge;     //*** Segment's bulge (huh-huh).
    sds_real lastang;   //*** Segment's angle. (used with line Length)
    struct cmd_pline_list *prev,*next;
} *cmd_plinebeg,*cmd_plinecur;


// ************************************************
// ************************************************
// ************************************************
// ************************************************
// ****************************
// COMMAND FUNCTION
//
short cmd_polygon(void) 
	{
//*** This function creates an equilateral closed polyline.
//***
//***  E - Specifies polygon by showing one edge.
//***  C - Circumscribes around circle.(AKA Side)
//***  I - Inscribes within circle. (AKA Vertex)
//***
//*** RETURNS: -1 - User cancel.
//***          -2 - RTERROR returned from an ads function.
//***
//*** The dragging and entity creation for EDGE is not yet complete.
//***

    if (cmd_iswindowopen() == RTERROR) 
		return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    char fs1[IC_ACADBUF],fstemp[IC_ACADNMLEN];
    struct resbuf setgetrb,*rslt,*newent,*rbtmp,*rbtmp2,rb;
    sds_point pt1,pt2,pt3,pt210;
    sds_real curelev,newelev;
    static double width=0.0;
    short defsides;
	RT	ret=0;
    int numsides, fi1,repeatmode=0;
    static bool bvertex;
	int plinetype;

    if (SDS_getvar(NULL,DB_QUCSXDIR,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) {ret=-2;goto exit;}
	ic_ptcpy(pt1,setgetrb.resval.rpoint);
    if (SDS_getvar(NULL,DB_QUCSYDIR,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) {ret=-2;goto exit;}
	ic_ptcpy(pt2,setgetrb.resval.rpoint);
	pt210[0]=(pt1[1]*pt2[2])-(pt2[1]*pt1[2]);
	pt210[1]=(pt1[2]*pt2[0])-(pt2[2]*pt1[0]);
	pt210[2]=(pt1[0]*pt2[1])-(pt2[0]*pt1[1]);

	SDS_getvar(NULL,DB_QPLINETYPE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	plinetype=setgetrb.resval.rint;

    //*** Get current elevation to check for valid point values.
    if (SDS_getvar(NULL,DB_QELEVATION,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) {ret=-2;goto exit;}
    curelev=newelev=setgetrb.resval.rreal;

    rslt=newent=rbtmp=NULL;
    //*** Get current number of polygon sides.
    if (SDS_getvar(NULL,DB_QPOLYSIDES,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) {ret=-2;goto exit;}
    defsides=setgetrb.resval.rint;

    for (;;) 
		{
		if (0==repeatmode)
			{
			if (sds_initget(0,ResourceString(IDC_POLYLINE_INITGET_WIDTH_O_0, "Width_of_line|Width ~ Multiple_polygons|Multiple ~_Width ~_ ~_Multiple" ))!=RTNORM) 
				{
				ret=-2;
				goto exit;
				}
			sprintf(fs1,ResourceString(IDC_POLYLINE__NPOLYGON___MUL_1, "\nPolygon:  Multiple/Width of line/<Number of sides> <%d>: " ),defsides);
			}
		else
			{
			if (sds_initget(0,ResourceString(IDC_POLYLINE_INITGET_WIDTH_O_2, "Width_of_line|Width ~_Width" ))!=RTNORM) 
				{
                ret=-2;
                goto exit;
				}
			sprintf(fs1,ResourceString(IDC_POLYLINE__NPOLYGON___WID_3, "\nPolygon:  Width of line/<Number of sides> <%d>: " ),defsides);
			}
        if ((ret=sds_getint(fs1,&numsides))==RTERROR) 
			{
            ret=-2;
			goto exit;
			}
		else if (ret==RTCAN) 
			{
            ret=-1;
			goto exit;
			}
		else if (ret==RTNORM) 
			{
            if (numsides<3 || numsides>1024) 
				{
                cmd_ErrorPrompt(CMD_ERR_POLYSIDES,0);
                continue;
				}
            break;
			}
		else if (ret==RTNONE) 
			{
            numsides=defsides;
            break;
			}
		else if (ret==RTKWORD) 
			{
			sds_getinput(fs1);
			if (strisame(fs1,"MULTIPLE"/*DNT*/)) 
				{
				repeatmode=1;
				continue;
				}
			sds_rtos(width,-1,-1,fstemp);
			sprintf(fs1,ResourceString(IDC_POLYLINE__NPOLYLINE_WIDT_4, "\nPolyline width <%s> :" ),fstemp);
			sds_initget(SDS_RSG_NONEG,NULL);
			ret=sds_getdist(NULL,fs1,&width);
			if (ret==RTCAN)
				{
				ret=-1;
				goto exit;
				}
			}
		else 
			{
            cmd_ErrorPrompt(CMD_ERR_POLYSIDES,0);
            continue;
			}
		}
    //*** Set POLYSIDES system variable.
    setgetrb.restype=RTSHORT;
    setgetrb.resval.rint=numsides;
    if (sds_setvar("POLYSIDES"/*DNT*/,&setgetrb)!=RTNORM) {ret=-2;goto exit;}

    for (;;) 
		{
        if (sds_initget(SDS_RSG_OTHER,ResourceString(IDC_POLYLINE_INITGET_SPECIFY_5, "Specify_by_edge|Edge ~_Edge" ))!=RTNORM) 
			{
            ret=-2;
            goto exit;
			}
        if ((ret=internalGetpoint(NULL,ResourceString(IDC_POLYLINE__NSPECIFY_BY____6, "\nSpecify by:  Edge/<Center of polygon>: " ),pt1))==RTERROR) 
			{
            ret=-2;
			goto exit;
			}
		else if (ret==RTCAN) 
			{
            ret=-1;
			goto exit;
			}
		else if (ret==RTNORM) 
			{
            if (!icadRealEqual(pt1[2],curelev)) 
				{
				SDS_CorrectElevation=curelev;
				SDS_ElevationChanged=TRUE;
				newelev=setgetrb.resval.rreal=pt1[2];
				setgetrb.restype=RTREAL;
				sds_setvar("ELEVATION"/*DNT*/,&setgetrb);
				}
            break;
			}
		else if (ret==RTKWORD) 
			{
            if (sds_getinput(fs1)!=RTNORM) {ret=-2;goto exit;}
            if (strisame(fs1,"EDGE"/*DNT*/)) 
				{
                for (;;) 
					{
                    if ((ret=internalGetpoint(NULL,ResourceString(IDC_POLYLINE__NFIRST_ENDPOIN_7, "\nFirst endpoint of edge: " ),pt1))==RTERROR) 
						{
                        ret=-2;
						goto exit;
						}
					else if (ret==RTCAN) 
						{
                        ret=-1;
						goto exit;
						}
					else if (ret==RTNORM) 
						{
						if (!icadRealEqual(pt1[2],curelev)) 
							{
							SDS_CorrectElevation=curelev;
							SDS_ElevationChanged=TRUE;
							newelev=setgetrb.resval.rreal=pt1[2];
							setgetrb.restype=RTREAL;
							sds_setvar("ELEVATION"/*DNT*/,&setgetrb);
							}
                        break;
						}
					else 
						{
                        cmd_ErrorPrompt(CMD_ERR_2DPT,0);
                        continue;
						}
					}
                for (;;) 
					{
                    //*** Something is wrong with the dragging in this function ~|.
                    SDS_getvar(NULL,DB_QDRAGMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
      				IC_RELRB(rslt);
		            if ((ret=SDS_dragobject(19,rb.resval.rint,pt1,pt1,(sds_real)numsides,ResourceString(IDC_POLYLINE__NSECOND_ENDPOI_8, "\nSecond endpoint of edge: " ),&rslt,pt2,NULL))==RTERROR) 
						{
                        ret=-2;
						goto exit;
						}
					else if (ret==RTCAN) 
						{
                        ret=-1;
						goto exit;
						}
					else if (ret==RTNORM) 
						{
                        /* if (pt2[2]!=pt1[2] && pt2[2]!=curelev) {
						//use icadRealEqual!!!
                            cmd_ErrorPrompt(CMD_ERR_2DPT,0);
                            continue;
                        } */
						pt2[2]=pt1[2];
                        break;
						}
					else 
						{
                        cmd_ErrorPrompt(CMD_ERR_2DPT,0);
                        continue;
						}
					}
                //*** Create polyline.
				if (plinetype==0) // make regular polyline
					{
					pt1[0]=pt1[1]=0.0; pt1[2]=newelev;
					rb.restype=RTSHORT;
					rb.resval.rint=1;
					setgetrb.restype=RT3DPOINT;
					ic_ptcpy(setgetrb.resval.rpoint,pt210);
					sds_trans(pt1,&rb,&setgetrb,0,pt1);
					pt1[0]=pt1[1]=0.0;
					newent=sds_buildlist(RTDXF0,"POLYLINE"/*DNT*/,66,1,10,pt1,70,1,40,width,41,width,210,pt210,0);
					if (sds_entmake(newent)!=RTNORM) {ret=-2;goto exit;}
					IC_RELRB(newent);
					newent=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,10,pt210,40,width,41,width,210,pt210,0);
					ic_assoc(newent,10);
					rbtmp2=ic_rbassoc;
					//*** Create polyline vertices.
					for (rbtmp=rslt,fi1=1; rbtmp!=NULL && fi1<=numsides; rbtmp=rbtmp->rbnext) 
						{
						if (rbtmp->restype!=10)
							continue;//there is a 210 in the list
						fi1++;//after continue
						sds_trans(rbtmp->resval.rpoint,&rb,&setgetrb,0,rbtmp2->resval.rpoint);
						if (sds_entmake(newent)!=RTNORM) {ret=-2;goto exit;}
						}
					IC_RELRB(newent);
					//*** Sequence end.
					newent=sds_buildlist(RTDXF0,"SEQEND"/*DNT*/,0);
					if (sds_entmake(newent)!=RTNORM) {ret=-2;goto exit;}
					IC_RELRB(newent); IC_RELRB(rslt);
					ret=0;
					}
				else
					{
					pt1[0]=pt1[1]=0.0; pt1[2]=newelev;
					rb.restype=RTSHORT;
					rb.resval.rint=1;
					setgetrb.restype=RT3DPOINT;
					ic_ptcpy(setgetrb.resval.rpoint,pt210);
					sds_trans(pt1,&rb,&setgetrb,0,pt1);
					pt1[0]=pt1[1]=0.0;
					// EBATECH(Futagami.Y)-[17/05/2002 invalid 38(Elevation) value.
					//newent=sds_buildlist(RTDXF0,"LWPOLYLINE"/*DNT*/,38,newelev,90,0,70,1,43,width,210,pt210,0);
					newent=sds_buildlist(RTDXF0,"LWPOLYLINE"/*DNT*/,38,pt1[2],90,0,70,1,43,width,210,pt210,0);
					// 17/05/2002]-EBATECH(Futagami.Y)
					resbuf *lwpladdrb=newent;
					while (lwpladdrb->rbnext!=NULL)
						lwpladdrb=lwpladdrb->rbnext;
					//*** Create polyline vertices.
					for (rbtmp=rslt,fi1=1; rbtmp!=NULL && fi1<=numsides; rbtmp=rbtmp->rbnext) 
						{
						sds_point temppt;
						if (rbtmp->restype!=10)
							continue;//there is a 210 in the list
						fi1++;//after continue
						sds_trans(rbtmp->resval.rpoint,&rb,&setgetrb,0,temppt);
						LwplrbAddVertFromData(&lwpladdrb,temppt,width,width,0.0);
						}
					LwplrbSetNumVerts(newent);
					if (sds_entmake(newent)!=RTNORM) {ret=-2;goto exit;}
					IC_RELRB(newent);
					IC_RELRB(rslt);
					ret=0;
					}
				
				goto exit;
				}
			else 
				{
                cmd_ErrorPrompt(CMD_ERR_2DPTKWORD,0);
                continue;
				}
			}
		else 
			{
            cmd_ErrorPrompt(CMD_ERR_2DPTKWORD,0);
            continue;
			}
		}

    //*** Get the radius - inscribed or circumscribed and create entity.
	//		We allow the user to pick the vertex point right now at this prompt.
	//		AutoCAD forces the user to hit ENTER for vertex, then pick the point.
	//		We still have to support ENTER, I/C, and V/S here (which we do, just don't mess it up). 
    for (;;) 
		{
        SDS_getvar(NULL,DB_QDRAGMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		for (;;) 
			{
			if (bvertex) 
				{
                ret=RTKWORD;
                break;
				}
            sds_initget(0,ResourceString(IDC_POLYLINE_INITGET_SPECIFY_9, "Specify_by_side|Side ~Vertex ~Inscribed ~Circumscribed ~_Side ~_Vertex ~_Inscribed ~_Circumscribed" ));
			IC_RELRB(rslt);
			ret=SDS_dragobject(16,rb.resval.rint,pt1,pt1,(sds_real)numsides,ResourceString(IDC_POLYLINE__NSPECIFY_BY___10, "\nSpecify by:  Side/<Select vertex point>: " ),&rslt,pt2,fs1);
			if (ret==RTKWORD) 
				{
                if (strisame(fs1,"SIDE"/*DNT*/) || strisame(fs1,"CIRCUMSCRIBED"/*DNT*/)) 
                    bvertex=true;
				else
                    bvertex=false;
				break;
	            }
            if (ret!=RTNONE)
				break;
			}
		if (ret==RTKWORD)
			{
			if ((strisame(fs1,"SIDE"/*DNT*/)) || strisame(fs1,"CIRCUMSCRIBED"/*DNT*/) || bvertex) 
				{
				sds_initget(0,ResourceString(IDC_POLYLINE_INITGET__SIDE__11, "~Side Specify_by_vertex|Vertex ~Inscribed ~Circumscribed ~_Side ~_Vertex ~_Inscribed ~_Circumscribed" ));
				IC_RELRB(rslt);
				ret=SDS_dragobject(17,rb.resval.rint,pt1,pt1,(sds_real)numsides,ResourceString(IDC_POLYLINE__NSPECIFY_BY___12, "\nSpecify by:  Vertex/<Midpoint of side>: " ),&rslt,pt2,fs1);
				if (ret==RTNONE)   // For compatibility - AutoCAD requires ENTER, then coordinate input.
					{
					sds_initget(0,ResourceString(IDC_POLYLINE_INITGET__SIDE__11, "~Side Specify_by_vertex|Vertex ~Inscribed ~Circumscribed ~_Side ~_Vertex ~_Inscribed ~_Circumscribed" ));
					IC_RELRB(rslt);
					ret=SDS_dragobject(17,rb.resval.rint,pt1,pt1,(sds_real)numsides,ResourceString(IDC_POLYLINE__NMIDPOINT_OF__13, "\nMidpoint of side: " ),&rslt,pt2,fs1);
					}
				}
			else
				continue;
			}
		if (ret==RTCAN)
			{
			ret=-1;
			goto exit;
			}
		else if (ret==RTERROR)
			{
			ret=-2;
			goto exit;
			}
		else if (ret==RTKWORD)
			{
            if (strisame(fs1,"SIDE"/*DNT*/) || strisame(fs1,"CIRCUMSCRIBED"/*DNT*/)) 
                bvertex=true;
            else
                bvertex=false;
            continue;
	        }
		else if (ret==RTNORM) 
			{
			pt3[0]=pt3[1]=pt2[0]=pt2[1]=0.0;
			pt3[2]=pt2[2]=newelev;
			rb.restype=RTSHORT;
			rb.resval.rint=1;
			setgetrb.restype=RT3DPOINT;
			ic_ptcpy(setgetrb.resval.rpoint,pt210);
			sds_trans(pt3,&rb,&setgetrb,0,pt3);
			pt3[0]=pt3[1]=0.0;

			do
				{				//*** Create polyline.
				if (plinetype==0) // create regular polyline
					{
					newent=sds_buildlist(RTDXF0,"POLYLINE"/*DNT*/,66,1,10,pt3,70,1,40,width,41,width,210,pt210,0);
					if (sds_entmake(newent)!=RTNORM) {ret=-2;goto exit;}
					IC_RELRB(newent);
					newent=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,10,pt210,40,width,41,width,210,pt210,0);
					ic_assoc(newent,10);
					rbtmp2=ic_rbassoc;
					//*** Create polyline vertices.
					for (rbtmp=rslt,fi1=0; rbtmp!=NULL && fi1<=numsides; rbtmp=rbtmp->rbnext,fi1++) 
						{
						if (rbtmp->restype==210)
							continue;
						else if (rbtmp->restype!=10) 
							{
							ret=-2;
							goto exit;
							}
						if (repeatmode==1)
							{
							//get pts of polygon for centroid calc later
							pt2[0]+=rbtmp->resval.rpoint[0];
							pt2[1]+=rbtmp->resval.rpoint[1];
							}
						else if (repeatmode>1)
							{
							//offset points
							rbtmp->resval.rpoint[0]+=(pt2[0]-pt1[0]);
							rbtmp->resval.rpoint[1]+=(pt2[1]-pt1[1]);
							}
						sds_trans(rbtmp->resval.rpoint,&rb,&setgetrb,0,rbtmp2->resval.rpoint);
						if (sds_entmake(newent)!=RTNORM) {ret=-2;goto exit;}
						}
					IC_RELRB(newent);
					if (repeatmode==1)	//calc centroid
						{
						pt2[0]/=numsides;
						pt2[1]/=numsides;
						}
					//*** Sequence end.
					newent=sds_buildlist(RTDXF0,"SEQEND"/*DNT*/,0);
					if (sds_entmake(newent)!=RTNORM) {ret=-2;goto exit;}
					IC_RELRB(newent);
					}
				else
					{
					newent=sds_buildlist(RTDXF0,"LWPOLYLINE"/*DNT*/,38,pt3[2],210,pt210,90,0,43,width,70,1,0);
					resbuf *lwpladdrb=newent;
					while (lwpladdrb->rbnext!=NULL)
						lwpladdrb=lwpladdrb->rbnext;
					//*** Create polyline vertices.
					for (rbtmp=rslt,fi1=0; rbtmp!=NULL && fi1<=numsides; rbtmp=rbtmp->rbnext,fi1++) 
						{
						sds_point temppt;
						if (rbtmp->restype==210)
							continue;
						else if (rbtmp->restype!=10) 
							{
							ret=-2;
							goto exit;
							}
						if (repeatmode==1)
							{
							//get pts of polygon for centroid calc later
							pt2[0]+=rbtmp->resval.rpoint[0];
							pt2[1]+=rbtmp->resval.rpoint[1];
							}
						else if (repeatmode>1)
							{
							//offset points
							rbtmp->resval.rpoint[0]+=(pt2[0]-pt1[0]);
							rbtmp->resval.rpoint[1]+=(pt2[1]-pt1[1]);
							}
						sds_trans(rbtmp->resval.rpoint,&rb,&setgetrb,0,temppt);
						LwplrbAddVertFromData(&lwpladdrb,temppt,width,width,0.0);
						}
					if (repeatmode==1)	//calc centroid
						{
						pt2[0]/=numsides;
						pt2[1]/=numsides;
						}
					LwplrbSetNumVerts(newent);
					if (sds_entmake(newent)!=RTNORM) {ret=-2;goto exit;} // make the lwpline
					IC_RELRB(newent);
					}

				if (!repeatmode)
					{
					IC_RELRB(rslt);
					ret=0;
					goto exit;
					}
				ic_ptcpy(pt1,pt2);
				ret=internalGetpoint(pt1,ResourceString(IDC_POLYLINE__NCENTER_OF_PO_14, "\nCenter of polygon: " ),pt2);
				repeatmode++;
				}
			while(ret==RTNORM);
			IC_RELRB(rslt);
			ret=0;
			goto exit;
			}
		else 
			{
            cmd_ErrorPrompt(CMD_ERR_DIST2NDPT,0);
            continue;
			}
		}


    exit:
	if (SDS_ElevationChanged)
		{
		setgetrb.restype=RTREAL;
		setgetrb.resval.rreal=curelev;
		sds_setvar("ELEVATION"/*DNT*/,&setgetrb);
		SDS_ElevationChanged=FALSE;
		}

    IC_RELRB(newent); 
	IC_RELRB(rslt);
    sds_entmake(NULL);
    return(ret);
	}


// ************************
// HELPER FUNCTION
//
static long 
cmd_pline_addsegment(sds_point pt1, sds_point pt2, sds_real curelev, short arcmode, short ThreeDmode) {
//*** This function adds a polyline segment.
//*** arcmode: 0 - creates a straight line segment.
//***          1 - creates an arc segment in the counter-clockwise direction from pt1.
//***          2 - creates an arc segment in the clockwise direction from pt1.
//*** ThreeDmode is same as for cmd_pline.  0==2d, 1==3D, 2==hatch bdry polygon
//***	NOTE: all pts passed are UCS and will be converted appropriately for entmakes


    struct cmd_pline_globalpacket *gpak;
    struct resbuf *newent,rblast,rb4ent,rbucs,rbwcs;
    struct cmd_pline_list *plinetmp;
    sds_point pt[2],tmppt,pt210;
    sds_real fr1,fr2;
    sds_name entlast;
    short fi1;
    long rc=0L;

	//setup resubfs used in transforming UCS pts to entity pts
    if (1!=ThreeDmode){
	    SDS_getvar(NULL,DB_QUCSXDIR,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	    ic_ptcpy(pt[0],rblast.resval.rpoint);
	    SDS_getvar(NULL,DB_QUCSYDIR,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	    ic_ptcpy(pt[1],rblast.resval.rpoint);
	    pt210[0]=(pt[0][1]*pt[1][2])-(pt[1][1]*pt[0][2]);
	    pt210[1]=(pt[1][0]*pt[0][2])-(pt[0][0]*pt[1][2]);
	    pt210[2]=(pt[0][0]*pt[1][1])-(pt[1][0]*pt[0][1]);
    }
	rbucs.restype=rbwcs.restype=RTSHORT;
	rbucs.rbnext=rb4ent.rbnext=rbwcs.rbnext=NULL;
	rbucs.resval.rint=1;
	rbwcs.resval.rint=0;
	if (ThreeDmode==1){
		rb4ent.restype=RTSHORT;
		rb4ent.resval.rint=0;
	}else{
		rb4ent.restype=RT3DPOINT;
		ic_ptcpy(rb4ent.resval.rpoint,pt210);
	}

    gpak=&cmd_pline_gpak;
    newent=NULL; rc=0;
    //*** Set LASTCMDPT,ISLASTCMDPT variable.
    rblast.restype=RT3DPOINT;
    ic_ptcpy(rblast.resval.rpoint,pt2);
	SDS_setvar(NULL,DB_QLASTCMDPT,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
	SDS_setvar(NULL,DB_QLASTPOINT,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
    rblast.restype=RTSHORT;
    rblast.resval.rint=1;
	SDS_setvar(NULL,DB_QISLASTCMDPT,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);

    //ic_ptcpy(pt[0],pt1); ic_ptcpy(pt[1],pt2);
	sds_trans(pt1,&rbucs,&rb4ent,0,pt[0]);
	sds_trans(pt2,&rbucs,&rb4ent,0,pt[1]);

    if (arcmode && gpak->arcdxf!=NULL) {
        //*** Calculate the bulge from the arc's dxf codes.
        ic_assoc(gpak->arcdxf,50);
        fr1=ic_rbassoc->resval.rreal;
        ic_assoc(gpak->arcdxf,51);
        fr2=ic_rbassoc->resval.rreal;
        ic_normang(&fr1,&fr2);
        gpak->bulge=tan((fr2-fr1)/4.0);
		ic_assoc(gpak->arcdxf,40);
		if (ic_rbassoc->resval.rreal<CMD_FUZZ){
			//special case for arc w/radius of 0.0
			rblast.resval.rreal=0.5*IC_PI;
        //*** Check the direction of the arc.
        }else if (arcmode==2) {
            //*** Clockwise arc
            gpak->bulge=(-gpak->bulge);
            if ((rblast.resval.rreal=(fr1-(IC_PI/2.0)))<0.0) rblast.resval.rreal+=(2.0*IC_PI);
        } else {
            //*** Counter-clockwise arc
            if ((rblast.resval.rreal=(fr2+(IC_PI/2.0)))>=(2.0*IC_PI)) rblast.resval.rreal-=(2.0*IC_PI);
        }
		//transform the last angle from ecs to ucs
		tmppt[0]=cos(rblast.resval.rreal);
		tmppt[1]=sin(rblast.resval.rreal);
		tmppt[2]=0;
		sds_trans(tmppt,&rb4ent,&rbucs,1,tmppt);
		rblast.resval.rreal=atan2(tmppt[1],tmppt[0]);
    } else {
        gpak->bulge=0.0;
        //*** Set lastangle global variable.
        rblast.resval.rreal=sds_angle(pt1,pt2);//lastangle is based on UCS pts
    }
    rblast.restype=RTREAL;
	SDS_setvar(NULL,DB_QLASTCMDANG,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
	SDS_setvar(NULL,DB_QLASTANGLE,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
    //*** Create temporary polyline segment.
    tmppt[0]=0.0; tmppt[1]=0.0; tmppt[2]=pt[0][2];
	//don't mess w/pline start/end widths here - this is temp for use during build of pline
    if (1==ThreeDmode)
        newent=sds_buildlist(RTDXF0,"POLYLINE"/*DNT*/,66,1,10,tmppt,70,8,0);
    else
        newent=sds_buildlist(RTDXF0,"POLYLINE"/*DNT*/,66,1,10,tmppt,210,pt210,70,0,0);
    if (sds_entmake(newent)!=RTNORM) { rc=(-__LINE__); goto bail; }
    IC_RELRB(newent);
    //*** Create polyline vertices.
    for (fi1=0; fi1<2; fi1++) {
        newent=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,10,pt[fi1],40,gpak->stwid,41,gpak->endwid,42,gpak->bulge,0);
        if (sds_entmake(newent)!=RTNORM) { rc=(-__LINE__); goto bail; }
        IC_RELRB(newent);
    }
    //*** Sequence end.
    newent=sds_buildlist(RTDXF0,"SEQEND"/*DNT*/,0);
    if (sds_entmake(newent)!=RTNORM) { rc=(-__LINE__); goto bail; }
    IC_RELRB(newent);

    sds_entlast(entlast);

    //*** Allocate new segment for linked list.
    if ((plinetmp= new struct cmd_pline_list )==NULL) {
        cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
        rc=(-__LINE__); goto bail;
    }
	memset(plinetmp,0,sizeof(struct cmd_pline_list));
    if (cmd_plinebeg==NULL) {
        cmd_plinebeg=cmd_plinecur=plinetmp;
    } else {
        plinetmp->prev=cmd_plinecur;
        cmd_plinecur=cmd_plinecur->next=plinetmp;
    }
    ic_encpy(cmd_plinecur->ename,entlast);
    ic_ptcpy(cmd_plinecur->vertexpt,pt1);
    cmd_plinecur->stwidth=gpak->stwid;
    cmd_plinecur->endwidth=gpak->endwid;
    cmd_plinecur->bulge=gpak->bulge;
    SDS_getvar(NULL,DB_QLASTCMDANG,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
    cmd_plinecur->lastang=rblast.resval.rreal;
    //***
    gpak->numents++; gpak->stwid=gpak->endwid;

    bail:

    IC_RELRB(newent);
    return(rc);
}

// ***************
// COMMAND FUNCTION
//
short cmd_pline(void) {
//*** This function creates 2D polylines.
//***
//*** RETURNS: -1 - User cancel.
//***          -2 - RTERROR returned from an ads function.
//***
    if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    return(cmd_PolylineFunc(0));
}

// ***************
// COMMAND FUNCTION
//
short cmd_3dpoly(void) {
//*** This function creates 3D polylines.
//***
//*** RETURNS: -1 - User cancel.
//***          -2 - RTERROR returned from an ads function.
//***
    if (cmd_iswindowopen() == RTERROR) return RTERROR;
	
	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    return(cmd_PolylineFunc(1));
}

short cmd_pline_set_nonarcmode_prompts(short ThreeDmode, struct cmd_pline_globalpacket *gpak, char *prompt)
	{
	resbuf lastcmdptrb;
	int rc=0;

	if (!ThreeDmode) 
		{
		SDS_getvar(NULL,DB_QISLASTCMDPT,&lastcmdptrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		if (gpak->numents>1)
			{
			//allow both Follow, Undo and Close
	        if (sds_initget(SDS_RSG_NOCANCEL,ResourceString(IDC_POLYLINE_INITGET_DRAW_A_18, "Draw_arcs|Arc ~ Close Distance Follow Halfwidth Width ~Length ~ Undo ` ~_Arc ~_ ~_Close ~_Distance ~_Follow ~_Halfwidth ~_Width ~_Length ~_ ~_Undo" ))!=RTNORM) 
				{ 
                rc=(-__LINE__); 
                goto bail; 
				}
			sprintf(prompt,ResourceString(IDC_POLYLINE__NARC_CLOSE_DI_19, "\nArc/Close/Distance/Follow/Halfwidth/Width/Undo/<Next point>: " ));
			}
		else if (lastcmdptrb.resval.rint && gpak->numents>0) 
			{
			//allow only Follow and Undo
			if (sds_initget(SDS_RSG_NOCANCEL,ResourceString(IDC_POLYLINE_INITGET__CLOSE_20, "~Close Draw_arcs|Arc ~ Distance Follow Halfwidth Width ~Length ~ Undo ` ~_Close ~_Arc ~_ ~_Distance ~_Follow ~_Halfwidth ~_Width ~_Length ~_ ~_Undo" ))!=RTNORM) 
				{ 
                rc=(-__LINE__); 
                goto bail; 
				}
			sprintf(prompt,ResourceString(IDC_POLYLINE__NARC_DISTANCE_21, "\nArc/Distance/Follow/Halfwidth/Width/Undo/<Next point>: " ));
			}
		else if (lastcmdptrb.resval.rint)
			{
			//allow only Follow
			if (sds_initget(SDS_RSG_NOCANCEL,ResourceString(IDC_POLYLINE_INITGET_DRAW_A_22, "Draw_arcs|Arc ~ Distance Follow Halfwidth Width ~Length ` ~_Arc ~_ ~_Distance ~_Follow ~_Halfwidth ~_Width ~_Length" ))!=RTNORM) 
				{ 
                rc=(-__LINE__); 
                goto bail; 
				}
			sprintf(prompt,ResourceString(IDC_POLYLINE__NARC_DISTANCE_23, "\nArc/Distance/Follow/Halfwidth/Width/<Next point>: " ));
			}
		else
			{
			//allow none of 'em
	        if (sds_initget(0,ResourceString(IDC_POLYLINE_INITGET_DRAW_A_24, "Draw_arcs|Arc ~ Distance Halfwidth Width ~Length ~_Arc ~_ ~_Distance ~_Halfwidth ~_Width ~_Length" ))!=RTNORM) 
				{ 
                rc=(-__LINE__); 
                goto bail; 
				}
			sprintf(prompt,ResourceString(IDC_POLYLINE__NARC_DISTANCE_25, "\nArc/Distance/Halfwidth/Width/<Next point>: " ));
			}
	    }
	else // if it IS 3d mode
		{
		if (gpak->numents>1)
			{
			if (sds_initget(SDS_RSG_NOCANCEL,ResourceString(IDC_POLYLINE_INITGET_CLOSE__26, "Close ~ Undo ` ~_Close ~_ ~_Undo" ))!=RTNORM) 
				{ 
                rc=(-__LINE__); 
                goto bail; 
				}
			sprintf(prompt,ResourceString(IDC_POLYLINE__NCLOSE_UNDO___27, "\nClose/Undo/<Next point>: " ));
			}
		else if (gpak->numents>0)
			{
			if (sds_initget(SDS_RSG_NOCANCEL,ResourceString(IDC_POLYLINE_INITGET_CLOSE__26, "Close ~ Undo ` ~_Close ~_ ~_Undo" ))!=RTNORM) 
				{ 
                rc=(-__LINE__); 
                goto bail; 
				}
			sprintf(prompt,ResourceString(IDC_POLYLINE__NUNDO_NEXT_PO_28, "\nUndo/Next point: " ));
			}
		else 
			{
			if (sds_initget(0,NULL)!=RTNORM) 
				{ 
                rc=(-__LINE__); 
                goto bail; 
				}
			sprintf(prompt,ResourceString(IDC_POLYLINE__NNEXT_POINT___29, "\nNext point: " ));
			}
		}
bail:
	return rc;
	}


short cmd_pline_set_arcmode_prompts(struct cmd_pline_globalpacket *gpak, char *prompt)
	{
	int rc=0;

	if (gpak->numents>0)
		{
		//allow close & undo
		if (sds_initget(SDS_RSG_NOCANCEL,ResourceString(IDC_POLYLINE_INITGET_DRAW_L_30, "Draw_lines|Line ~ Angle Center|CEnter Close|CLose Direction Halfwidth Radius Second_point|Secondpt Width ~ Undo ` ~_Line ~_ ~_Angle ~_CEnter ~_CLose ~_Direction ~_Halfwidth ~_Radius ~_Secondpt ~_Width ~_ ~_Undo" ))!=RTNORM)
			{ 
            rc=(-__LINE__); 
            goto bail; 
			}
		sprintf(prompt,ResourceString(IDC_POLYLINE__NANGLE_CENTER_31, "\nAngle/CEnter/CLose/Direction/Halfwidth/Line/Radius/Secondpt/Width/Undo/<End of arc>: " ));
		}
	else 
		{
		//don't allow either of 'em
		if (sds_initget(0,ResourceString(IDC_POLYLINE_INITGET_DRAW_L_32, "Draw_lines|Line ~ Angle Center|CEnter Direction Halfwidth Radius Second_point|Secondpt Width ~_Line ~_ ~_Angle ~_CEnter ~_Direction ~_Halfwidth ~_Radius ~_Secondpt ~_Width" ))!=RTNORM) 
			{ 
            rc=(-__LINE__); 
            goto bail; 
			}
		sprintf(prompt,ResourceString(IDC_POLYLINE__NANGLE_CENTER_33, "\nAngle/CEnter/Direction/Halfwidth/Line/Radius/Secondpt/Width/<End of arc>: " ));
		}

bail:
	return rc;
	}


short cmd_pline_angle(short ThreeDmode, struct cmd_pline_globalpacket *gpak, short *arcmode, sds_point firstpt, resbuf *rb4ent, sds_point pt[3])
	{
	resbuf setgetrb,cmdangrb;
	sds_point tmppt;
    sds_real fr1,fr2,incang;
	char fs1[IC_ACADBUF],fs2[IC_ACADBUF];
	int ret,rc=0;
	resbuf rbucs;

	rbucs.restype=RTSHORT;
	rbucs.rbnext=NULL;
	rbucs.resval.rint=1;

	for (;;) 
		{
    	SDS_getvar(NULL,DB_QANGDIR,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		if (1==setgetrb.resval.rint)
			sprintf(fs2,ResourceString(IDC_POLYLINE__NINCLUDED_ANG_34, "\nIncluded angle for segment (>0 is cw, 0 is straight, <0 is ccw): " ),fs1);
		else
			sprintf(fs2,ResourceString(IDC_POLYLINE__NINCLUDED_ANG_35, "\nIncluded angle for segment (>0 is ccw, 0 is straight, <0 is cw): " ),fs1);
		sds_initget(0,NULL);
		if ((ret=SDS_getangleneg(pt[0],fs2,&fr1))==RTNORM)
			{
			if (setgetrb.resval.rint)
				fr1=-fr1;
			}
		else if (ret==RTNONE)
			{
			cmd_ErrorPrompt(CMD_ERR_ANGLE,0);
			goto bail;
			}
		else if (ret==RTCAN) 
			break;
		else if (ret!=RTNORM)
			{
			ret=RTERROR;
			break;
			}
		incang=fr1;
		break;
	    }
	if (ret==RTCAN || ret==RTERROR)
		{
		ret=RTNORM;
		goto bail;
		}
    if (sds_initget(RSG_NONULL,ResourceString(IDC_POLYLINE_INITGET_CENTER_36, "Center Radius ~_Center ~_Radius" ))!=RTNORM) 
		{ 
        rc=(-__LINE__); 
        goto bail; 
		}
    SDS_getvar(NULL,DB_QDRAGMODE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
    IC_RELRB(gpak->arcdxf);

    if ((ret=SDS_dragobject(25,setgetrb.resval.rint,pt[0],pt[1],incang,ResourceString(IDC_POLYLINE__NCENTER_RADIU_37, "\nCenter/Radius/<End of arc>: " ),&gpak->arcdxf,tmppt,fs1))==RTERROR) 
		{
        rc=(-__LINE__);
		goto bail;
		}
	else if (ret==RTCAN) 
		{
        rc=(-1L);
		goto bail;
		}
	else if (ret==RTNORM)
		{
        if (!icadRealEqual(tmppt[2],pt[0][2]) && !icadRealEqual(tmppt[2],firstpt[2])) 
			{
            cmd_ErrorPrompt(CMD_ERR_2DPTKWORD,0);
            goto bail;
			}
        ic_ptcpy(pt[1],tmppt);
		}
	else if (ret==RTKWORD || ret==RTSTR)
		{
        if (strisame(fs1,"CENTER"/*DNT*/))
			{
            if (sds_initget(SDS_RSG_NONULL,NULL)!=RTNORM)
				{
               rc=(-__LINE__);
               goto bail;
				}
            SDS_getvar(NULL,DB_QDRAGMODE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
            IC_RELRB(gpak->arcdxf);
            if ((ret=SDS_dragobject(26,setgetrb.resval.rint,pt[0],pt[1],incang,ResourceString(IDC_POLYLINE__NCENTER_OF_AR_38, "\nCenter of arc: " ),&gpak->arcdxf,tmppt,fs1))==RTERROR)
				{
                rc=(-__LINE__);
				goto bail;
				}
			else if (ret==RTCAN)
				{
                rc=(-1L);
				goto bail;
				}
			else if (ret==RTNORM) 
				{
				ic_assoc(gpak->arcdxf,10);
				sds_trans(ic_rbassoc->resval.rpoint,rb4ent,&rbucs,0,ic_rbassoc->resval.rpoint);
                if (!icadRealEqual(ic_rbassoc->resval.rpoint[2],pt[0][2]) && !icadRealEqual(ic_rbassoc->resval.rpoint[2],firstpt[2]))
					{
                    cmd_ErrorPrompt(CMD_ERR_2DPTKWORD,0);
                    goto bail;
					}
				}
			}
		else if (strisame(fs1,"RADIUS"/*DNT*/))
			{
            if (sds_initget(SDS_RSG_NONULL+SDS_RSG_NOZERO,NULL)!=RTNORM) 
				{
               rc=(-__LINE__);
               goto bail;
				}
            if ((ret=sds_getdist(NULL,ResourceString(IDC_POLYLINE__NRADIUS___39, "\nRadius: " ),&fr2))==RTERROR)
				{
                rc=(-__LINE__);
				goto bail;
				}
			else if (ret==RTCAN) 
				{
                rc=(-1L);
				goto bail;
				}
            //*** Get the direction of the chord
            SDS_getvar(NULL,DB_QLASTCMDANG,&cmdangrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
            if (sds_rtos((cmdangrb.resval.rreal*180.0/IC_PI),-1,-1,fs1)!=RTNORM) 
				{ 
				rc=(-__LINE__); 
				goto bail;
				}
            sprintf(fs2,ResourceString(IDC_POLYLINE__NDIRECTION_OF_40, "\nDirection of chord <%s>: " ),fs1);
            IC_RELRB(gpak->arcdxf);
            pt[1][0]=incang;
            if (sds_initget(0,NULL)!=RTNORM) 
				{
               rc=(-__LINE__);
               goto bail;
				}
            SDS_getvar(NULL,DB_QDRAGMODE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
            if ((ret=SDS_dragobject(24,setgetrb.resval.rint,pt[0],pt[1],fr2,fs2,&gpak->arcdxf,tmppt,fs1))==RTERROR) 
				{
				rc=(-__LINE__);
				goto bail;
				}
			else if (ret==RTCAN)
				{
                rc=(-1L);
				goto bail;
				}
			else
				{
				ic_assoc(gpak->arcdxf,10);
				sds_trans(ic_rbassoc->resval.rpoint,rb4ent,&rbucs,0,ic_rbassoc->resval.rpoint);
				}
	        }
        //*** Calculate the endpoint of the arc
        if (ic_assoc(gpak->arcdxf,10)!=0)
			{
			rc=(-__LINE__);
			goto bail;
			}
        if (!ThreeDmode)
			ic_rbassoc->resval.rpoint[2]=firstpt[2];
        ic_ptcpy(tmppt,ic_rbassoc->resval.rpoint);
        if (ic_assoc(gpak->arcdxf,40)!=0) 
			{
			rc=(-__LINE__);
			goto bail; 
			}
        fr1=ic_rbassoc->resval.rreal;
        if (incang<0.0) 
			{
            if (ic_assoc(gpak->arcdxf,50)!=0) 
				{
				rc=(-__LINE__);
				goto bail; 
				}
			}
		else 
			{
            if (ic_assoc(gpak->arcdxf,51)!=0) 
				{
				rc=(-__LINE__);
				goto bail; 
				}
			}
        fr2=ic_rbassoc->resval.rreal;
        sds_polar(tmppt,fr2,fr1,pt[1]);
    }
    //*** Set the direction of the arc
    if (incang<0.0)
		*arcmode=2; //*** (CW)
    else
		*arcmode=1; //*** (CCW)
    //*** It is important that "pt[1]" and "gpak->arcdxf" have been set at this point.

    if ((rc=cmd_pline_addsegment(pt[0],pt[1],firstpt[2],*arcmode,ThreeDmode))!=0L)
		{
		rc=(-__LINE__);
		goto bail;
		}
bail:
	return rc;
	}

short cmd_pline_center(short ThreeDmode, struct cmd_pline_globalpacket *gpak, short *arcmode, sds_point firstpt, resbuf *rb4ent,sds_point pt[3])
	{
	resbuf rbucs,setgetrb;
	sds_point tmppt;
	char fs1[IC_ACADBUF];
	int rc=0, ret;
	sds_real fr1,fr2;

	rbucs.restype=RTSHORT;
	rbucs.rbnext=NULL;
	rbucs.resval.rint=1;

	if (sds_initget(SDS_RSG_NONULL,NULL)!=RTNORM) 
		{
		rc=(-__LINE__);
		goto bail;
		}
    //*** Set the point(to be passed to addsegment()) equal to the specified center point.
    if ((ret=internalGetpoint(NULL,ResourceString(IDC_POLYLINE__NCENTER_POINT_41, "\nCenter point: " ),pt[1]))==RTERROR)
		{
        rc=(-__LINE__); 
		goto bail;
		}
	else if (ret==RTCAN) 
		{
        rc=(-1L);
		goto bail;
		}
    if (sds_initget(RSG_NONULL,ResourceString(IDC_POLYLINE_INITGET_ANGLE__42, "Angle Length ~_Angle ~_Length" ))!=RTNORM)
		{ 
        rc=(-__LINE__); 
        goto bail; 
		}
    SDS_getvar(NULL,DB_QDRAGMODE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
    IC_RELRB(gpak->arcdxf);
    if ((ret=SDS_dragobject(9,setgetrb.resval.rint,pt[0],pt[1],0,ResourceString(IDC_POLYLINE__NANGLE_LENGTH_43, "\nAngle/Length/<End of arc>: " ),&gpak->arcdxf,tmppt,fs1))==RTERROR)
		{
        rc=(-__LINE__);
		goto bail;
		}
	else if (ret==RTCAN)
		{
        rc=(-1L);
		goto bail;
	    }
	else if (ret==RTNORM) 
		{
		ic_assoc(gpak->arcdxf,10);
		sds_trans(ic_rbassoc->resval.rpoint,rb4ent,&rbucs,0,tmppt);
        if (!icadRealEqual(tmppt[2],pt[0][2]) && !icadRealEqual(tmppt[2],firstpt[2]))
			{
            cmd_ErrorPrompt(CMD_ERR_2DPTKWORD,0);
            goto bail;
	        }
		}
	else if (ret==RTKWORD || ret==RTSTR)
		{
        if (strisame(fs1,"ANGLE"/*DNT*/)) 
			{
            if (sds_initget(SDS_RSG_NONULL,NULL)!=RTNORM)
				{
				rc=(-__LINE__);
				goto bail;
				}
            SDS_getvar(NULL,DB_QDRAGMODE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
            IC_RELRB(gpak->arcdxf);
            if ((ret=SDS_dragobject(8,setgetrb.resval.rint,pt[0],pt[1],0,ResourceString(IDC_POLYLINE__NINCLUDED_ANG_44, "\nIncluded angle: " ),&gpak->arcdxf,tmppt,NULL))==RTERROR)
				{
                rc=(-__LINE__);
				goto bail;
				}
			else if (ret==RTCAN)
				{
                rc=(-1L);
				goto bail;
				}
	        }
		else if (strisame(fs1,"LENGTH"/*DNT*/))
			{
            if (sds_initget(SDS_RSG_NONULL,NULL)!=RTNORM) 
				{
				rc=(-__LINE__);
				goto bail;
	            }
            SDS_getvar(NULL,DB_QDRAGMODE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
            IC_RELRB(gpak->arcdxf);
            if ((ret=SDS_dragobject(10,setgetrb.resval.rint,pt[0],pt[1],0,ResourceString(IDC_POLYLINE__NLENGTH_OF_CH_45, "\nLength of chord: " ),&gpak->arcdxf,tmppt,NULL))==RTERROR)
				{
                rc=(-__LINE__);
				goto bail;
				}
			else if (ret==RTCAN)
				{
                rc=(-1L);
				goto bail;
				}
			}

		}
    //*** Calculate the endpoint and the direction of the arc
    if (ic_assoc(gpak->arcdxf,10)!=0) 
		{
		rc=(-__LINE__);
		goto bail;
		}
    ic_ptcpy(tmppt,ic_rbassoc->resval.rpoint);
    if (ic_assoc(gpak->arcdxf,40)!=0) 
		{
		rc=(-__LINE__);
		goto bail; 
		}
    fr1=ic_rbassoc->resval.rreal;
    if (ic_assoc(gpak->arcdxf,51)!=0) 
		{ 
		rc=(-__LINE__);
		goto bail;
		}
    fr2=ic_rbassoc->resval.rreal;
    sds_polar(tmppt,fr2,fr1,pt[1]);
	sds_trans(pt[1],rb4ent,&rbucs,0,pt[1]);
    if (icadRealEqual(pt[1][0],pt[0][0]) && icadRealEqual(pt[1][1],pt[0][1]))
		{
        //*** If the startpoint and endpoint are the same then calculate the endpoint
        //*** using the start angle instead of the end angle (Clockwise arc).
        if (ic_assoc(gpak->arcdxf,50)!=0) 
			{
			rc=(-__LINE__);
			goto bail;
			}
        fr2=ic_rbassoc->resval.rreal;
        sds_polar(tmppt,fr2,fr1,pt[1]);
		sds_trans(pt[1],rb4ent,&rbucs,0,pt[1]);
        *arcmode=2;
		}
	else
		*arcmode=1;
    //*** It is important that "pt[1]" and "gpak->arcdxf" have been set at this point.
    if ((rc=cmd_pline_addsegment(pt[0],pt[1],firstpt[2],*arcmode,ThreeDmode))!=0L) 
		goto bail;
bail:
	return rc;
	}

short cmd_pline_direction(short ThreeDmode, struct cmd_pline_globalpacket *gpak, short *arcmode, sds_point firstpt, resbuf *rb4ent,sds_point pt[3])
	{
	resbuf rbucs,setgetrb;
	sds_point tmppt;
	char fs1[IC_ACADBUF];
	int rc=0, ret;
	sds_real fr1;

	rbucs.restype=RTSHORT;
	rbucs.rbnext=NULL;
	rbucs.resval.rint=1;

    if (sds_initget(SDS_RSG_NONULL,NULL)!=RTNORM) 
		{
		rc=(-__LINE__);
		goto bail;
		}
    if ((ret=sds_getangle(pt[0],ResourceString(IDC_POLYLINE__NDIRECTION_FR_46, "\nDirection from start: " ),&fr1))==RTERROR) 
		{
        rc=(-__LINE__);
		goto bail;
		}
	else if (ret==RTCAN)
		{
        rc=(-1L);
		goto bail;
		}
    if (sds_initget(SDS_RSG_NONULL,NULL)!=RTNORM)
		{
		rc=(-__LINE__);
		goto bail;
	    }
    SDS_getvar(NULL,DB_QDRAGMODE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
    IC_RELRB(gpak->arcdxf);
    if ((ret=SDS_dragobject(15,setgetrb.resval.rint,pt[0],pt[1],fr1,ResourceString(IDC_POLYLINE__NEND_OF_ARC___47, "\nEnd of arc: " ),&gpak->arcdxf,tmppt,fs1))==RTERROR)
		{
        rc=(-__LINE__);
		goto bail;
		}
	else if (ret==RTCAN)
		{
        rc=(-1L);
		goto bail;
		}
	else if (ret==RTNORM)
		{
        if (!icadRealEqual(tmppt[2],pt[0][2]) && !icadRealEqual(tmppt[2],firstpt[2]))
			{
            cmd_ErrorPrompt(CMD_ERR_2DPTKWORD,0);
            goto bail;
			}
        ic_ptcpy(pt[1],tmppt);
		}
    //*** Calculate the direction of the arc.
    if ((((pt[1][0]-pt[0][0])*sin(fr1))-((pt[1][1]-pt[0][1])*cos(fr1)))<0.0)
		*arcmode=1; //*** Counter-clockwise
    else 
		*arcmode=2; //*** Clockwise
    //*** It is important that "pt[1]" and "gpak->arcdxf" have been set at this point.
    if ((rc=cmd_pline_addsegment(pt[0],pt[1],firstpt[2],*arcmode,ThreeDmode))!=0L)
		{
		goto bail;
		}
bail:
	return rc;
	}


short cmd_pline_distance(short ThreeDmode, struct cmd_pline_globalpacket *gpak, short *arcmode, sds_point firstpt,
						 resbuf *rb4ent,sds_point pt[3], char *fs1)
	{
	resbuf rbucs,rblast,*rslt = NULL;
	int rc=0, ret;
	sds_real fr1;

	rbucs.restype=RTSHORT;
	rbucs.rbnext=NULL;
	rbucs.resval.rint=1;

    for (;;) 
		{
        if ((ret=sds_getdist(pt[0],ResourceString(IDC_POLYLINE__NDISTANCE_FOR_48, "\nDistance for segment: " ),&fr1))==RTERROR)
            {
			rc=(-__LINE__);
			goto bail;
			}
        else if (ret==RTCAN)
			{
            rc=(-1L);
			goto bail;
			}
		else if (ret!=RTNORM)
			{
            cmd_ErrorPrompt(CMD_ERR_DIST2NDPT,0);
            continue;
			}
		break;
		}
	if (strisame(fs1,"LENGTH"/*DNT*/))
		{
		SDS_getvar(NULL,DB_QLASTCMDANG,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		sds_polar(pt[0],rblast.resval.rreal,fr1,pt[1]);
		}
	else
		{
		for (;;)
			{
			IC_RELRB(rslt);
			if ((ret=SDS_dragobject(27,1,pt[0],pt[1],fr1,ResourceString(IDC_POLYLINE__NANGLE_OF_SEG_49, "\nAngle of segment: " ),&rslt,pt[1],NULL))==RTERROR)
				{
				rc=(-__LINE__);
				goto bail;
				}
			else if (ret==RTCAN)
				{
				rc=(-1L);
				goto bail;
				}
			else if (ret!=RTNORM)
				{
				cmd_ErrorPrompt(CMD_ERR_DIST2NDPT,0);
				continue;
				}
			ic_assoc(rslt,11);
			sds_trans(ic_rbassoc->resval.rpoint,rb4ent,&rbucs,0,pt[1]);
			IC_RELRB(rslt);
			break;
			}
		}
    if ((rc=cmd_pline_addsegment(pt[0],pt[1],firstpt[2],0,ThreeDmode))!=0)
		goto bail;
bail:
	return rc;
	}


short cmd_pline_width(short ThreeDmode, struct cmd_pline_globalpacket *gpak, short *arcmode, sds_point firstpt, 
					  resbuf *rb4ent,sds_point pt[3], char *fs1)
	{
	resbuf rbucs;
	int rc=0, ret;
	char fs2[IC_ACADBUF];
	sds_real fr1;
	short halfwid=0;

	rbucs.restype=RTSHORT;
	rbucs.rbnext=NULL;
	rbucs.resval.rint=1;

    if (strisame(fs1,"HALFWIDTH"/*DNT*/))
		halfwid=1;
		
	for (;;)
		{
        if (sds_initget(SDS_RSG_OTHER | RSG_NONEG,NULL)!=RTNORM)
			{
			rc=(-__LINE__);
			goto bail;
			}
        if (!halfwid) 
			{
            if (sds_rtos(gpak->stwid,-1,-1,fs2)!=RTNORM) 
				{
				rc=(-__LINE__);
				goto bail;
				}
            sprintf(fs1,ResourceString(IDC_POLYLINE__NSTARTING_WID_50, "\nStarting width <%s>: " ),fs2);
			}
		else
			{
            if (sds_rtos((gpak->stwid/2.0),-1,-1,fs2)!=RTNORM) 
				{
				rc=(-__LINE__);
				goto bail; 
				}
            sprintf(fs1,ResourceString(IDC_POLYLINE__NSTARTING_HAL_51, "\nStarting half-width <%s>: " ),fs2);
			}
        if ((ret=sds_getdist(pt[0],fs1,&fr1))==RTERROR) 
			{
            rc=(-__LINE__); goto bail;
			}
		else if (ret==RTCAN) 
			{
            rc=(-1L);
			goto bail;
			}
		else if (ret==RTNORM) 
			{
            if (!halfwid) gpak->stwid=fr1;
            else gpak->stwid=(fr1*2);
            break;
			}
		else if (ret==RTNONE) 
			{
            break;
			}
		else 
			{
            cmd_ErrorPrompt(CMD_ERR_DIST2NDPT,0);
            continue;
			}
		}
    for (;;) 
		{
        if (sds_initget(SDS_RSG_OTHER|RSG_NONEG,NULL)!=RTNORM) 
			{
			rc=(-__LINE__);
			goto bail;
	        }
        if (!halfwid) 
			{
            if (sds_rtos(gpak->stwid,-1,-1,fs2)!=RTNORM) 
				{
				rc=(-__LINE__);
				goto bail;
				}
            sprintf(fs1,ResourceString(IDC_POLYLINE__NENDING_WIDTH_52, "\nEnding width <%s>: " ),fs2);
	        }
		else
			{
            if (sds_rtos((gpak->stwid/2.0),-1,-1,fs2)!=RTNORM) 
				{
				rc=(-__LINE__);
				goto bail; 
				}
            sprintf(fs1,ResourceString(IDC_POLYLINE__NENDING_HALF__53, "\nEnding half-width <%s>: " ),fs2);
			}
        if ((ret=sds_getdist(pt[0],fs1,&fr1))==RTERROR)
			{
            rc=(-__LINE__);
			goto bail;
	        }
		else if (ret==RTCAN)
			{
            rc=(-1L);
			goto bail;
			}
		else if (ret==RTNORM)
			{
            if (!halfwid)
				gpak->endwid=fr1;
            else
				gpak->endwid=(fr1*2);
            break;
			}
		else if (ret==RTNONE)
			{
            gpak->endwid=gpak->stwid;
            break;
			}
		else 
			{
            cmd_ErrorPrompt(CMD_ERR_DIST2NDPT,0);
            continue;
			}
		}
bail:
	return rc;
	}

short cmd_pline_radius(short ThreeDmode, struct cmd_pline_globalpacket *gpak, short *arcmode, sds_point firstpt, 
					  resbuf *rb4ent,sds_point pt[3])
	{
	resbuf rbucs,dragmoderb,setgetrb,lastcmdangrb;
	int rc=0, ret;
	char fs1[IC_ACADBUF],fs2[IC_ACADBUF];
	sds_real fr1,fr2,incang;
	short halfwid=0;
	sds_point tmppt;

	rbucs.restype=RTSHORT;
	rbucs.rbnext=NULL;
	rbucs.resval.rint=1;

	if (sds_initget(SDS_RSG_NONULL+SDS_RSG_NOZERO,NULL)!=RTNORM) 
		{
		rc=(-__LINE__);
		goto bail;
		}
    if ((ret=sds_getdist(NULL,ResourceString(IDC_POLYLINE__NRADIUS___39, "\nRadius: " ),&fr1))==RTERROR)
		{
        rc=(-__LINE__);
		goto bail;
	    }
	else if (ret==RTCAN)
		{
        rc=(-1L);
		goto bail;
		}
    if (sds_initget(SDS_RSG_NONULL,ResourceString(IDC_POLYLINE_INITGET_INCLUD_54, "Included_angle|Angle ~_Angle" ))!=RTNORM) 
		{ 
        rc=(-__LINE__); 
        goto bail; 
		}
    SDS_getvar(NULL,DB_QDRAGMODE,&dragmoderb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
    IC_RELRB(gpak->arcdxf);
	for(;;) /*D.Gavrilov*/// It was added just for entering a correct 2nd point.
		{
	    if ((ret=SDS_dragobject(23,dragmoderb.resval.rint,pt[0],pt[1],fr1,ResourceString(IDC_POLYLINE__NANGLE__END_O_55, "\nAngle/<End of arc>: " ),&gpak->arcdxf,tmppt,fs1))==RTERROR)
			{
		    rc=(-__LINE__);
			goto bail;
			}
		else if (ret==RTCAN) 
			{
			rc=(-1L);
			goto bail;
			}
		else if (ret==RTNORM)
			{
			if( (sqrt((tmppt[0] - pt[0][0]) * (tmppt[0] - pt[0][0]) +
				 	 (tmppt[1] - pt[0][1]) * (tmppt[1] - pt[0][1])) > 2.0 * fr1) ||
				(ThreeDmode != 1 && !icadRealEqual(tmppt[2], pt[0][2])) )
				{	/*D.Gavrilov*/// We shouldn't accept invalid points...
				cmd_ErrorPrompt(CMD_ERR_TRYAGAIN, 0);	// It's the only one case (now),
				continue;								// when we should execute the loop again
				}
	        ic_ptcpy(pt[1],tmppt);
			*arcmode=1;
		    }
		else if (ret==RTKWORD || ret==RTSTR)
			{
		    for (;;) 
				{
				//note: getorient doesn't accept negative values, so use getpoint
				sds_initget(0,NULL);
				SDS_getvar(NULL,DB_QANGDIR,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				if (1==setgetrb.resval.rint)
					sprintf(fs2,ResourceString(IDC_POLYLINE__NINCLUDED_ANG_56, "\nIncluded angle for segment (>0 is cw, <0 is ccw): " ),fs1);
				else
					sprintf(fs2,ResourceString(IDC_POLYLINE__NINCLUDED_ANG_57, "\nIncluded angle for segment (>0 is ccw, <0 is cw): " ),fs1);
				if ((ret=SDS_getangleneg(pt[0],fs2,&fr2))==RTNORM)
					{
					if (fabs(fr2)<CMD_FUZZ)
						{
						cmd_ErrorPrompt(CMD_ERR_NONZERO,0);
						continue;
						}
					if (setgetrb.resval.rint)
						fr2=-fr2;
					}
				else if (ret==RTNONE)
					{
					cmd_ErrorPrompt(CMD_ERR_ANGLE,0);
					continue;
					}
				else if (ret==RTCAN) 
					{
					break;
					}
				else if (ret!=RTNORM)
					{
					ret=RTERROR;
					break;
					}
				incang=fr2;
				break;
				}
			if (RTERROR==ret || RTCAN==ret)
				{
				ret=RTNORM;
				goto bail;
				}
	        //*** Get the direction of the chord
			SDS_getvar(NULL,DB_QLASTCMDANG,&lastcmdangrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		    if (sds_angtos(lastcmdangrb.resval.rreal,-1,-1,fs1)!=RTNORM) 
				{
				rc=(-__LINE__); 
				goto bail; 
				}
	        sprintf(fs2,ResourceString(IDC_POLYLINE__NDIRECTION_OF_40, "\nDirection of chord <%s>: " ),fs1);
			IC_RELRB(gpak->arcdxf);
		    pt[1][0]=incang;
	        if (sds_initget(0,NULL)!=RTNORM) 
				{
				rc=(-__LINE__);
				goto bail;
				}
			SDS_getvar(NULL,DB_QDRAGMODE,&dragmoderb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		    if ((ret=SDS_dragobject(24,dragmoderb.resval.rint,pt[0],pt[1],fr1,fs2,&gpak->arcdxf,tmppt,fs1))!=RTNORM && ret!=RTNONE)
				{
				//just go back to where we drag the general arc
			    goto bail;
				}
			if (ret==RTNORM)
				{
				//*** Calculate the endpoint and set the direction of the arc
				if (ic_assoc(gpak->arcdxf,10)!=0) 
					{
					rc=(-__LINE__); 
					goto bail; 
					}
				ic_ptcpy(tmppt,ic_rbassoc->resval.rpoint);
				if (ic_assoc(gpak->arcdxf,40)!=0) 
					{
					rc=(-__LINE__);
					goto bail; 
					}
				fr1=ic_rbassoc->resval.rreal;
				if (incang<0.0)  //*** Clockwise
					{
					if (ic_assoc(gpak->arcdxf,50)!=0) 
						{ 
						rc=(-__LINE__); 
						goto bail; 
						}
					*arcmode=2;
					}
				else //*** Counter-clockwise
					{
					if (ic_assoc(gpak->arcdxf,51)!=0) 
						{ 
						rc=(-__LINE__); 
						goto bail; 
						}
					*arcmode=1;
					}
				fr2=ic_rbassoc->resval.rreal;
				sds_polar(tmppt,fr2,fr1,pt[1]);
				sds_trans(pt[1],rb4ent,&rbucs,0,pt[1]);
				}
			else if (ret==RTNONE)
				{
				if (incang<0.0)
					*arcmode=2;
				else 
					*arcmode=1;
				sds_polar(pt[0],lastcmdangrb.resval.rreal,fabs(2.0*fr1*sin(0.5*incang)),pt[1]);
				}
			}
			break;	/*D.Gavrilov*/
	}				// This loop is executed once if a 2nd entered point is correct.
    //*** It is important that "pt[1]" and "gpak->arcdxf" have been set at this point.
    if ((rc=cmd_pline_addsegment(pt[0],pt[1],firstpt[2],*arcmode,ThreeDmode))!=0L)
		goto bail;
bail:
	return rc;
}


short cmd_pline_secondpt(short ThreeDmode, struct cmd_pline_globalpacket *gpak, short *arcmode, sds_point firstpt, 
					  resbuf *rb4ent,sds_point pt[3])
	{
	resbuf rbucs,setgetrb;
	int rc=0, ret;
	sds_real fr1;
	short halfwid=0;
	sds_point tmppt;
	char fs1[IC_ACADBUF];

	rbucs.restype=RTSHORT;
	rbucs.rbnext=NULL;
	rbucs.resval.rint=1;

	if (sds_initget(SDS_RSG_NONULL,NULL)!=RTNORM) 
		{
		rc=(-__LINE__);
		goto bail;
	    }
    //*** Set the point(to be passed to addsegment()) equal to the specified center point.
    if ((ret=internalGetpoint(NULL,ResourceString(IDC_POLYLINE__NSECOND_POINT_58, "\nSecond point: " ),pt[1]))==RTERROR)
		{
        rc=(-__LINE__);
		goto bail;
		}
	else if (ret==RTCAN) 
		{
        rc=(-1L);
		goto bail;
		}
    if (sds_initget(SDS_RSG_NONULL,NULL)!=RTNORM) 
		{
		rc=(-__LINE__);
		goto bail;
	    }
    SDS_getvar(NULL,DB_QDRAGMODE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
    IC_RELRB(gpak->arcdxf);
    if ((ret=SDS_dragobject(0,setgetrb.resval.rint,pt[0],pt[1],0,ResourceString(IDC_POLYLINE__NEND_OF_ARC___47, "\nEnd of arc: " ),&gpak->arcdxf,tmppt,fs1))==RTERROR)
		{
        rc=(-__LINE__);
		goto bail;
		}
	else if (ret==RTCAN)
		{
        rc=(-1L);
		goto bail;
		}
	else if (ret==RTNORM)
		{
        if (!icadRealEqual(tmppt[2],pt[0][2]) && !icadRealEqual(tmppt[2],firstpt[2])) 
			{
            cmd_ErrorPrompt(CMD_ERR_2DPTKWORD,0);
			goto bail;
			}
	    }
    //*** Calculate the direction of the arc.
    fr1=sds_angle(pt[0],pt[1]);
    if ((((tmppt[0]-pt[1][0])*sin(fr1))-((tmppt[1]-pt[1][1])*cos(fr1)))<0.0) 
		*arcmode=1; //*** Counter-clockwise
    else
		*arcmode=2; //*** Clockwise
    ic_ptcpy(pt[1],tmppt);
    //*** It is important that "pt[1]" and "gpak->arcdxf" have been set at this point.
    if ((rc=cmd_pline_addsegment(pt[0],pt[1],firstpt[2],*arcmode,ThreeDmode))!=0L)
		goto bail;

bail:
	return rc;
	}


short cmd_pline_undo(short ThreeDmode, struct cmd_pline_globalpacket *gpak, short *arcmode, sds_point firstpt, 
					  resbuf *rb4ent,sds_point pt[3], struct cmd_pline_list **plinetmp, sds_real first_lastang)
	{
	resbuf rbucs,rblast;
	int rc=0;
	short halfwid=0;

	rbucs.restype=RTSHORT;
	rbucs.rbnext=NULL;
	rbucs.resval.rint=1;


    if (gpak->numents<1)
		{
		cmd_ErrorPrompt(CMD_ERR_NOMOREUNDO,0);
        goto bail;
		}
    (*plinetmp)=cmd_plinecur;
    //*** Erase entity.
    if (sds_entdel((*plinetmp)->ename)!=RTNORM) 
		{
		rc=(-__LINE__); 
		goto bail; 
		}
    //*** Set pt[1] in case user presses ENTER right after an undo.
    ic_ptcpy(pt[0],(*plinetmp)->vertexpt);
    ic_ptcpy(pt[1],pt[0]);
    //*** Reset LASTCMDPT,ISLASTCMDPT variable.
    rblast.restype=RT3DPOINT;
    ic_ptcpy(rblast.resval.rpoint,pt[0]);
	SDS_setvar(NULL,DB_QLASTCMDPT,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
	SDS_setvar(NULL,DB_QLASTPOINT,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
    rblast.restype=RTSHORT;
    rblast.resval.rint=1;
	SDS_setvar(NULL,DB_QISLASTCMDPT,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
    //*** Reset start and ending pline widths.
    gpak->stwid=(*plinetmp)->stwidth;
    gpak->endwid=(*plinetmp)->endwidth;
    //***
    if (cmd_plinecur->prev==NULL)
		{
        cmd_plinebeg=cmd_plinecur=NULL;
		}
	else 
		{
        cmd_plinecur=cmd_plinecur->prev;
        cmd_plinecur->next=NULL;
		}
    IC_FREE((*plinetmp));
    //*** Set lastangle global variable.
    rblast.restype=RTREAL;
    if (cmd_plinecur!=NULL)
		rblast.resval.rreal=cmd_plinecur->lastang;
    else
		rblast.resval.rreal=first_lastang;
	SDS_setvar(NULL,DB_QLASTCMDANG,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
	SDS_setvar(NULL,DB_QLASTANGLE,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
    --gpak->numents;

bail:
	return rc;
	}


		
short CreateOldstylePline(short ThreeDmode, resbuf *rb4ent, sds_point firstpt, short isclosed, sds_point pt210, sds_point pt[3],
						  struct cmd_pline_globalpacket *gpak)
	{ 
	sds_point tmppt;
	resbuf *newent;
	resbuf rbucs,setgetrb;
	int rc=0;
    struct cmd_pline_list *plinetmp=NULL;

	rbucs.restype=RTSHORT;
	rbucs.rbnext=NULL;
	rbucs.resval.rint=1;

	tmppt[0]=tmppt[1]=0.0;
	tmppt[2]=(ThreeDmode)?0.0:firstpt[2];
	if (0==ThreeDmode)
		{
		sds_trans(tmppt,&rbucs,rb4ent,0,tmppt);//get ecs elevation
		tmppt[0]=tmppt[1]=0.0;
		}
    if ((newent=sds_buildlist(RTDXF0,"POLYLINE"/*DNT*/,
							 66,1,
							 70,(ThreeDmode)? isclosed+8 : isclosed,
							 10,tmppt,
							 40,cmd_plinebeg->stwidth,
							 41,cmd_plinebeg->endwidth,
							 210,pt210,
							 0))==NULL)
		{
        cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
        rc=(-1L);
		goto bail;
		}
    if (sds_entmake(newent)!=RTNORM) 
		{
		rc=(-__LINE__);
		goto bail; 
		}
    IC_RELRB(newent);
    //*** Create polyline vertices.
    for (plinetmp=cmd_plinecur=cmd_plinebeg; cmd_plinecur!=NULL; plinetmp=cmd_plinecur)
		{
		//before creating, translate pt into proper coord sys
		sds_trans(cmd_plinecur->vertexpt,&rbucs,rb4ent,0,tmppt);
        if ((newent=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,10,tmppt,40,cmd_plinecur->stwidth,
           41,cmd_plinecur->endwidth,42,cmd_plinecur->bulge,210,pt210,0))==NULL) 
			{
            cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
            rc=(-1);
			goto bail;
			}
        if (ThreeDmode) //*** 3D polyline vertex
			{
            ic_assoc(newent,210);
            if ((ic_rbassoc->rbnext=sds_buildlist(70,32,0))==NULL) 
				{
                cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
                rc=(-1);
				goto bail;
				}
			}
        if (sds_entmake(newent)!=RTNORM) 
			{
			rc=(-__LINE__);
			goto bail;
			}
        IC_RELRB(newent);
        //*** Delete temporary pline segment and free link.
        if (sds_entdel(cmd_plinecur->ename)!=RTNORM) 
			{
			rc=(-__LINE__); 
			goto bail; 
			}
        cmd_plinecur=cmd_plinecur->next;
        IC_FREE(plinetmp);
		}
    plinetmp=cmd_plinecur=cmd_plinebeg=NULL;
    //*** Create last vertex.
	sds_trans(pt[0],&rbucs,rb4ent,0,tmppt);
    if ((newent=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,10,tmppt,40,gpak->stwid,41,gpak->stwid,42,gpak->bulge,210,pt210,0))==NULL)
		{
        cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
        rc=(-1);
		goto bail;
		}
    if (ThreeDmode)  //*** 3D polyline vertex
		{
		ic_assoc(newent,210);
        if ((ic_rbassoc->rbnext=sds_buildlist(70,32,0))==NULL) 
			{
            cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
            rc=(-1);
			goto bail;
			}
	    }
    if (sds_entmake(newent)!=RTNORM) 
		{
		rc=(-__LINE__); 
		goto bail; 
		}
    IC_RELRB(newent);
    //*** Sequence end.
    newent=sds_buildlist(RTDXF0,"SEQEND"/*DNT*/,0);
    if (sds_entmake(newent)!=RTNORM) 
		{
		rc=(-__LINE__); 
		goto bail; 
		}
    IC_RELRB(newent);
    //*** Set PLINEWID system variable.
    setgetrb.restype=RTREAL;
    setgetrb.resval.rreal=gpak->endwid;
    if (sds_setvar("PLINEWID"/*DNT*/,&setgetrb)!=RTNORM) 
		{
		rc=(-__LINE__);
		goto bail; 
		}

    IC_RELRB(newent);
bail:
	return rc;
	}

	
short CreateLightweightPline(resbuf *rb4ent, sds_point firstpt, short isclosed, sds_point pt210, sds_point pt[3],
						  struct cmd_pline_globalpacket *gpak, sds_real curelev)
	{ 
	sds_point tmppt;
	resbuf *newent,rbthick,*prb,setgetrb;
	resbuf rbucs;
	int rc=0;
    struct cmd_pline_list *plinetmp=NULL,*plinetmp2=NULL;
    //Bugzilla: 78376 ; 09-12-02
	double FirstSegStartWidth=0.0, FirstSegEndWidth=0.0;
    BOOL bAllWidthsAreEqual=TRUE;

	
	rbucs.restype=RTSHORT;
	rbucs.rbnext=NULL;
	rbucs.resval.rint=1;

	SDS_getvar(NULL,DB_QTHICKNESS,&rbthick,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

	int nverts=0;
	for (plinetmp=cmd_plinebeg; plinetmp!=NULL; plinetmp=plinetmp->next) 
		{
		nverts++;
		}

	nverts++;  // to include the final one
	tmppt[0]=tmppt[1]=0.0;
	tmppt[2]=firstpt[2];
	sds_trans(tmppt,&rbucs,rb4ent,0,tmppt);//get ecs elevation
	tmppt[0]=tmppt[1]=0.0;
    if ((newent=sds_buildlist(RTDXF0,"LWPOLYLINE"/*DNT*/,
							90,nverts,
							39,rbthick.resval.rreal,
							38,curelev,
							70,isclosed,
							210,pt210,
							0))==NULL)
		{
        cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
        rc=(-1L);
		goto bail;
		}
	prb=newent;
    while (prb->rbnext!=NULL)
		prb=prb->rbnext;

    //Bugzilla: 78376 ; 09-12-02
	// save the first segments starting and ending widths
    FirstSegStartWidth = cmd_plinebeg->stwidth;
    FirstSegEndWidth = cmd_plinebeg->endwidth;
	
	for (plinetmp=cmd_plinebeg; plinetmp!=NULL; plinetmp=plinetmp->next) 
		{
        //Bugzilla: 78376 ; 09-12-02
		// set the constant widths flag to FALSE if we find more than 1 width
        if ( (bAllWidthsAreEqual == TRUE) &&
             ( (plinetmp->stwidth  != FirstSegStartWidth) ||
               (plinetmp->endwidth != FirstSegStartWidth) ) )
        {   
            bAllWidthsAreEqual = FALSE;
        }

		sds_trans(plinetmp->vertexpt,&rbucs,rb4ent,0,tmppt);
		prb->rbnext=sds_buildlist(10,tmppt,0);
		prb=prb->rbnext;
		prb->rbnext=sds_buildlist(40,plinetmp->stwidth,0);
		prb=prb->rbnext;
		prb->rbnext=sds_buildlist(41,plinetmp->endwidth,0);
		prb=prb->rbnext;
		prb->rbnext=sds_buildlist(42,plinetmp->bulge,0);
		prb=prb->rbnext;

		// Delete temporary pline segment
        if (sds_entdel(plinetmp->ename)!=RTNORM) 
			{
			rc=(-__LINE__); 
			goto bail; 
			}
		}

    //Bugzilla: 78376 ; 09-12-02
	// set the constant width flag if widths of all segments are the same
    if (bAllWidthsAreEqual == TRUE)
	{ // constant width
		prb->rbnext=sds_buildlist(43, FirstSegStartWidth, 0);
		prb=prb->rbnext;
	}

    //Create last vertex.
	sds_trans(pt[0],&rbucs,rb4ent,0,tmppt);
    if ((prb->rbnext=sds_buildlist(10,tmppt,40,gpak->stwid,41,gpak->stwid,42,gpak->bulge,0))==NULL)
		{
        cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
        rc=(-1);
		goto bail;
		}

	// free the temporary entities
	plinetmp=cmd_plinebeg;
	plinetmp2=cmd_plinebeg->next;
	while (plinetmp)
		{
		IC_FREE(plinetmp);
		plinetmp=plinetmp2;
		if (plinetmp2!=NULL)
			plinetmp2=plinetmp2->next;
		}
	
	if (sds_entmake(newent)!=RTNORM) 
		{
		rc=(-__LINE__);
		goto bail; 
		}
    IC_RELRB(newent);
    //*** Set PLINEWID system variable.
    setgetrb.restype=RTREAL;
    setgetrb.resval.rreal=gpak->endwid;
    if (sds_setvar("PLINEWID"/*DNT*/,&setgetrb)!=RTNORM) 
		{
		rc=(-__LINE__);
		goto bail; 
		}

bail:
	return rc;
	}

short cmd_PolylineFunc(short ThreeDmode) 
	{
//*** This function creates 2D and 3D polylines.
//***	NOTE: If ThreeDmode==2, f'n being called from HATCH and we want initial RTNONE
//***		to end function instead of attempting to use LASTCMDPT
//***
//*** RETURNS: -1 - User cancel.
//***          -2 - RTERROR returned from an ads function.
//***
    char fs1[IC_ACADBUF],fs2[IC_ACADBUF];
    struct cmd_pline_globalpacket *gpak;
    struct resbuf setgetrb,*rslt=NULL,rblast,rbucs,rbwcs,rb4ent;
    struct cmd_pline_list *plinetmp=NULL;
    sds_point firstpt,pt[3],tmppt,pt210;
    sds_real fr1,fr2,fr3,first_lastang,curelev,newelev;
    RT		ret;
	short	arcmode=0,isclosed=0,followmode=0;
    long rc=0L;

    //*** Initialize global variables.
    gpak=&cmd_pline_gpak;
    cmd_plinebeg=cmd_plinecur=NULL;
    gpak->numents=0; gpak->bulge=0.0;

	//*** Get and save current elevation
	if (RTNORM!=SDS_getvar(NULL,DB_QELEVATION,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))
		return(-2);
    curelev=newelev=rblast.resval.rreal;

	//setup resubfs used in transforming UCS pts to entity pts
	SDS_getvar(NULL,DB_QUCSXDIR,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	ic_ptcpy(pt[0],rblast.resval.rpoint);
	SDS_getvar(NULL,DB_QUCSYDIR,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	ic_ptcpy(pt[1],rblast.resval.rpoint);
	pt210[0]=(pt[0][1]*pt[1][2])-(pt[1][1]*pt[0][2]);
	pt210[1]=(pt[1][0]*pt[0][2])-(pt[0][0]*pt[1][2]);
	pt210[2]=(pt[0][0]*pt[1][1])-(pt[1][0]*pt[0][1]);

	rbucs.restype=rbwcs.restype=RTSHORT;
	rbucs.rbnext=rbwcs.rbnext=rb4ent.rbnext=NULL;
	rbucs.resval.rint=1;
	rbwcs.resval.rint=0;
	if (ThreeDmode==1)
		{
		rb4ent.restype=RTSHORT;
		rb4ent.resval.rint=0;
		}
	else
		{
		rb4ent.restype=RT3DPOINT;
		ic_ptcpy(rb4ent.resval.rpoint,pt210);
		}

	SDS_getvar(NULL,DB_QISLASTCMDPT,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if (ThreeDmode==2)
		{
		//prompting handled by calling f'n
		if (sds_initget(0,NULL)!=RTNORM) 
			{
			rc=(-__LINE__);
			goto bail;
			}
		if ((ret=internalGetpoint(NULL,""/*DNT*/,pt[0]))==RTCAN || ret==RTERROR)
			return(ret);
		}
	else if (rblast.resval.rint)
		{
		if (sds_initget(0,ResourceString(IDC_POLYLINE_INITGET_FOLLOW_15, "Follow ~_Follow" ))!=RTNORM) 
			{ 
            rc=(-__LINE__); 
            goto bail; 
			}
		if ((ret=internalGetpoint(NULL,ResourceString(IDC_POLYLINE__NENTER_TO_USE_16, "\nENTER to use last point/Follow/<Start of polyline>: " ),pt[0]))==RTCAN || ret==RTERROR)
			return(ret);
		}
	else
		{
		if (sds_initget(0,NULL)!=RTNORM)
			{
			rc=(-__LINE__);
			goto bail;
			}
		if ((ret=internalGetpoint(NULL,ResourceString(IDC_POLYLINE__NSTART_OF_POL_17, "\nStart of polyline: " ),pt[0]))==RTCAN || ret==RTERROR)
			return(ret);
		}

    //*** NOTE: If 3Dmode==2 I want this function to return RTNONE if getpoint() returns RTNONE.  This is
    //***       because I am calling this function from cmd_hatch()!
	//***		This exit from f'n must be BEFORE any reset of elev
	if (ret==RTNONE && ThreeDmode==2)
		return(ret);
	if (ThreeDmode==2)
		ThreeDmode=0;

	if (ThreeDmode!=1 && RTNORM==ret && pt[0][2]!=curelev)	//do we need this if in 3d???
		{
		SDS_CorrectElevation=curelev;
		SDS_ElevationChanged=TRUE;
		newelev=rblast.resval.rreal=pt[0][2];
		rblast.restype=RTREAL;
		sds_setvar("ELEVATION"/*DNT*/,&rblast);
		}

	SDS_getvar(NULL,DB_QISLASTCMDPT,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
    if (rblast.resval.rint &&  ret==RTKWORD)		//follow mode
		{
		followmode=1;
    	SDS_getvar(NULL,DB_QLASTCMDPT,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		ic_ptcpy(firstpt,rblast.resval.rpoint);
		ic_ptcpy(pt[0],firstpt);
		}
	else if (rblast.resval.rint && ret==RTNONE) 	  //just continue from last point
		{
		SDS_getvar(NULL,DB_QLASTCMDPT,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		ic_ptcpy(firstpt,rblast.resval.rpoint);
		ic_ptcpy(pt[0],firstpt);
		}
	else
		{
		followmode=0;
		if (!rblast.resval.rint && ret==RTNONE)
			return(ret);
		ic_ptcpy(firstpt,pt[0]);
		}

    if (SDS_getvar(NULL,DB_QPLINEGEN,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
		{
		rc=(-__LINE__); 
		goto bail; 
		}

	if (setgetrb.resval.rint)
		isclosed |= IC_PLINE_CONTINUELT;

    if (!ThreeDmode) 
		{
        if (SDS_getvar(NULL,DB_QPLINEWID,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
			{
			rc=(-__LINE__);
			goto bail;
			}
        gpak->stwid=gpak->endwid=setgetrb.resval.rreal;
        //***
        if (sds_rtos(gpak->stwid,-1,-1,fs1)!=RTNORM) 
			{
			rc=(-__LINE__);
			goto bail; 
			}
		}
	else
		gpak->stwid=gpak->endwid=0.0;

    SDS_getvar(NULL,DB_QLASTCMDANG,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
    first_lastang=rblast.resval.rreal;

    //*** Get the rest of the points.
    for (;;)
		{
		//NOTE: Acad has a LENGTH option, which continues along the lastangle a given length.
		//		We have FOLLOW, which continues but projects from a point (not a distance)
		//		We also have DISTANCE which takes a distance, but prompts for the angle
		int frombuf=0;
		if ( GetActiveCommandQueue()->IsNotEmpty() || 
			GetMenuQueue()->IsNotEmpty()) 
			{
			frombuf=1;
			}

        if (!arcmode) 
			{
			SDS_PointDisplayMode=5;

			if (cmd_pline_set_nonarcmode_prompts(ThreeDmode, gpak, fs1))
				{
				rc=(-__LINE__);
				goto bail; 
				}

			if (followmode!=1)
				{
				ret=internalGetpoint(pt[0],fs1,pt[1]);
				SDS_PointDisplayMode=0;
				if ( (ThreeDmode != 1) &&
					 (ret == RTNORM) )
					{
					pt[1][2]=pt[0][2];
					}
				}
			else
				{
				IC_RELRB(rslt);
				SDS_getvar(NULL,DB_QLASTCMDANG,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
                ret=SDS_dragobject(2,1,pt[0],pt[1],rblast.resval.rreal,fs1,&rslt,tmppt,fs2);
				if (ret==RTNORM)
					{
					ic_assoc(rslt,11);
					sds_trans(ic_rbassoc->resval.rpoint,&rbwcs,&rbucs,0,pt[1]);
					//if (ThreeDmode!=1)pt[1][2]=pt[0][2];//not needed because dragobj getting dist w/o elev
					IC_RELRB(rslt);
					}
				}
			}
		else  // arcmode is true
			{
			if (cmd_pline_set_arcmode_prompts(gpak, fs1))
				{
				rc=(-__LINE__);
				goto bail;
				}

            SDS_getvar(NULL,DB_QDRAGMODE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
            IC_RELRB(gpak->arcdxf);
			SDS_getvar(NULL,DB_QLASTCMDANG,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			//Modified Cybage SBD 22/03/2001 DD/MM/YYYY 
			//Reason : Fix for Bug No. 6157 from Bugzilla 
			back:
            if ((ret=SDS_dragobject(15,setgetrb.resval.rint,pt[0],pt[1],rblast.resval.rreal,fs1,&gpak->arcdxf,tmppt,fs1))==RTNORM)
				{
                //*** Set the point(to be passed to addsegment()) equal to the endpoint returned by this call to dragobject().
                ic_ptcpy(pt[1],tmppt);
				}

            if (ThreeDmode!=1 && ret==RTNORM)
				{
				if (!icadRealEqual(pt[1][2],pt[0][2]) && !icadRealEqual(pt[1][2],curelev))
					{
					cmd_ErrorPrompt(CMD_ERR_2DPTKWORD,0);
					continue;
					}
				pt[1][2]=pt[0][2];
				}
			} // end of arcmode is true

        if (ret==RTERROR)
			{
            rc=(-__LINE__);
			goto bail;
			}
		else if (ret==RTCAN)
			{
            rc=(-1L);
			goto bail;
			}
		else if (ret==RTNONE)
			{
            rc=0L;
			goto bail;
			}
		else if (ret==RTNORM)
			{
			if (frombuf && !ThreeDmode) pt[1][2]=firstpt[2];
            //if (!ThreeDmode) {//elev stuff done at each dragobj or getpoint call
                //*** Make sure they entered a valid 2D point.
                //if (pt[1][2]!=firstpt[2]){
                //    cmd_ErrorPrompt(CMD_ERR_2DPTKWORD,0);
                //    continue;
               // }
            //}
            if (arcmode)
				{
                //*** Calculate the direction of the arc.
    			SDS_getvar(NULL,DB_QLASTCMDANG,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
                if ((((pt[1][0]-pt[0][0])*sin(rblast.resval.rreal))-((pt[1][1]-pt[0][1])*cos(rblast.resval.rreal)))<0.0)
					arcmode=1; //*** Counter-clockwise
                else 
					arcmode=2; //*** Clockwise
				}
            if ((rc=cmd_pline_addsegment(pt[0],pt[1],firstpt[2],arcmode,ThreeDmode))!=0L)
				goto bail;
			followmode=0;
			}
		else if (ret==RTKWORD)
			{
			if (!followmode) 
				{
				if (sds_getinput(fs1)!=RTNORM) 
					{
					rc=(-__LINE__);
					goto bail;
					}
				}
			else
				{
				strncpy(fs1,fs2,sizeof(fs2)-1);
				followmode=0;
				}

			if (strisame(fs1,"ANGLE"/*DNT*/))
				{
				if (cmd_pline_angle(ThreeDmode,gpak,&arcmode,firstpt,&rb4ent,pt))
					{
					rc=(-__LINE__);
					goto bail;
					}
				}
			else if (strisame(fs1,"ARC"/*DNT*/)) 
				{
				arcmode=1;
				continue;
				}
			else if (strisame(fs1,"FOLLOW"/*DNT*/))	 //** Non Arc-mode only
				{
				followmode=1;
				continue;
				}
			else if (strisame(fs1,"CENTER"/*DNT*/))   //*** Arc mode only
				{
				if (cmd_pline_center(ThreeDmode,gpak,&arcmode,firstpt,&rb4ent,pt))
					{
					rc=(-__LINE__);
					goto bail;
					}
				}
			else if (strisame(fs1,"CLOSE"/*DNT*/))
				{
/*
                if (arcmode && cmd_plinecur!=cmd_plinebeg) {
                    ic_ptcpy(pt[1],cmd_plinebeg->vertexpt);
                    SDS_getvar(NULL,DB_QLASTCMDANG,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
                    if (ic_2pta2arc(pt[0],pt[1],rblast.resval.rreal,tmppt,&fr3,&fr1,&fr2)!=0) { rc=(-__LINE__); goto bail; }
                    ic_normang(&fr1,&fr2);
                    gpak->bulge=tan((fr2-fr1)/4.0);
                    //*** Calculate the direction of the arc.
                    if ((((pt[1][0]-pt[0][0])*sin(rblast.resval.rreal))-((pt[1][1]-pt[0][1])*cos(rblast.resval.rreal)))<0.0) {
                        //*** Counter-clockwise arc
                        if ((rblast.resval.rreal=(fr2+(IC_PI/2.0)))>=(2.0*IC_PI)) rblast.resval.rreal-=(2.0*IC_PI);
                    } else {
                        //*** Clockwise arc
                        gpak->bulge=(-gpak->bulge);
                        if ((rblast.resval.rreal=(fr1-(IC_PI/2.0)))<0.0) rblast.resval.rreal+=(2.0*IC_PI);
                    }
                    rblast.restype=RTREAL;
					SDS_setvar(NULL,DB_QLASTCMDANG,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
                } else if (!arcmode) gpak->bulge=0.0;
*/
                //*** ACAD seems to set the flag even if there are only 2 vertexes.
				if ((isclosed & 1)==0)
					isclosed+=1;
                rc=0L;
				goto bail;
				}
			else if (strisame(fs1,"DIRECTION"/*DNT*/))
				{
				if (cmd_pline_direction(ThreeDmode,gpak,&arcmode,firstpt,&rb4ent,pt))
					{
					rc=(-__LINE__);
					goto bail;
					}
				}
			else if (strisame(fs1,"DISTANCE"/*DNT*/) || strisame(fs1,"LENGTH"/*DNT*/))
				{
				//this is length or distance for STRAIGHT segment, DIFFERENT FROM
				//	Length option after specifying center of curved segment!!!
				if (cmd_pline_distance(ThreeDmode,gpak,&arcmode,firstpt,&rb4ent,pt,fs1))
					{
					rc=(-__LINE__);
					goto bail;
					}
				}
			else if (strisame(fs1,"LINE"/*DNT*/))
				{
                arcmode=0;
				continue;
				}
			else if (strisame(fs1,"HALFWIDTH"/*DNT*/) || strisame(fs1, "WIDTH"/*DNT*/))
				{
				if (cmd_pline_width(ThreeDmode,gpak,&arcmode,firstpt,&rb4ent,pt,fs1))
					{
					rc=(-__LINE__);
					goto bail;
					}
				continue;
				}
			else if (strisame(fs1,"RADIUS"/*DNT*/)) 
				{
				if (cmd_pline_radius(ThreeDmode,gpak,&arcmode,firstpt,&rb4ent,pt))
					{
					rc=(-__LINE__);
					goto bail;
					}
				}
			else if (strisame(fs1,"SECONDPT"/*DNT*/)) 
				{
				if (cmd_pline_secondpt(ThreeDmode,gpak,&arcmode,firstpt,&rb4ent,pt))
					{
					rc=(-__LINE__);
					goto bail;
					}
	            }
			else if (strisame(fs1,"UNDO"/*DNT*/))
				{
				if (cmd_pline_undo(ThreeDmode,gpak,&arcmode,firstpt,&rb4ent,pt,&plinetmp,first_lastang))
					{
					rc=(-__LINE__);
					goto bail;
					}
				continue;
				}
			//Modified Cybage SBD 22/03/2001 DD/MM/YYYY [
			//Reason : Fix for Bug No. 6157 from Bugzilla 
			else if ( strisame(fs1,"c"/*DNT*/) && (gpak->numents>0) )
				{
				sprintf(fs1,ResourceString(IDC_POLYLINE_CENTER_CLOSE_59, "Ambigious Please clarify...\nCEnter/CLose: " ));
				sds_initget(SDS_RSG_NOCANCEL,ResourceString(IDC_POLYLINE_INITGET_CECL_L_60, "Center|CEnter Close|CLose ~_CEnter ~_CLose" ));
				goto  back;
				}
			//Modified Cybage SBD 22/03/2001 DD/MM/YYYY ]
			else 
				{
				cmd_ErrorPrompt(CMD_ERR_PTKWORD,0);
                continue;
				}
			}
		else
			return(ret);
        //*** Set pt[0]=pt[1] for next internalGetpoint.
        ic_ptcpy(pt[0],pt[1]);
		}

    bail:

    if (rc>=(-1L) && cmd_plinebeg!=NULL) 
		{
        //*** Calculate the bulge for the last vertex
		//don't forget to set lastpt & LASTANGLE if we're closing....
		SDS_getvar(NULL,DB_QLASTCMDPT,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
        if (isclosed & 1)
			ic_ptcpy(rblast.resval.rpoint,cmd_plinebeg->vertexpt);
		gpak->bulge=0.0;//we'll reset it if in arcmode
		if (!icadPointEqual(pt[0],cmd_plinebeg->vertexpt))// && cmd_plinecur!=cmd_plinebeg)
			{
			//if user input closing point isn't exactly on beginning pt
			if (arcmode) 
				{
				ic_ptcpy(pt[1],cmd_plinebeg->vertexpt);
				SDS_getvar(NULL,DB_QLASTCMDANG,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
                if (ic_2pta2arc(pt[0],pt[1],rblast.resval.rreal,tmppt,&fr3,&fr1,&fr2)!=0)
					{
					rc=(-__LINE__);
					goto bail;
					}
				ic_normang(&fr1,&fr2);
				gpak->bulge=tan((fr2-fr1)/4.0);
				//*** Calculate the direction of the arc.
				if ((((pt[1][0]-pt[0][0])*sin(rblast.resval.rreal))-((pt[1][1]-pt[0][1])*cos(rblast.resval.rreal)))<0.0) 
					{
					//*** Counter-clockwise arc
					if (isclosed & 1)
						{
						if ((rblast.resval.rreal=(fr2+(IC_PI/2.0)))>=(2.0*IC_PI))
							rblast.resval.rreal-=(2.0*IC_PI);
						}
					} 
				else 
					{
					//*** Clockwise arc
					gpak->bulge=(-gpak->bulge);
					if (isclosed & 1)
						{
						if ((rblast.resval.rreal=(fr1-(IC_PI/2.0)))<0.0) 
							rblast.resval.rreal+=(2.0*IC_PI);
						}
					}
				}
			else if (isclosed & 1)
				{
                fr1=sds_angle(pt[0],rblast.resval.rpoint);
				rblast.resval.rreal=fr1;	//use dummy var in case
				}
			if (isclosed & 1)
				{
				rblast.restype=RTREAL;
				SDS_setvar(NULL,DB_QLASTCMDANG,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
				SDS_setvar(NULL,DB_QLASTANGLE,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
				}
			}
        //*** Create polyline.
		resbuf plinetyperb;
        SDS_getvar(NULL,DB_QPLINETYPE,&plinetyperb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		if (ThreeDmode || plinetyperb.resval.rint==0)
			CreateOldstylePline(ThreeDmode,&rb4ent,firstpt,isclosed,pt210,pt,gpak);
		else
			{
			// Prevents from falling to z=0 by assigning proper elevation
///*
			sds_point ptd;
			struct resbuf rb, rbent;
			
			rb.restype=RTSHORT;
			rb.resval.rint=1;
			
			rbent.restype=RT3DPOINT;
			rbent.resval.rpoint[0]=pt210[0];
			rbent.resval.rpoint[1]=pt210[1];
			rbent.resval.rpoint[2]=pt210[2];

			ptd[0]=0.0;
			ptd[1]=0.0;
			ptd[2]=pt[0][2];
			sds_trans(ptd,&rb,&rbent,0,ptd);
			newelev = ptd[2];
			// _end Prevent from falling
//*/
//			CreateLightweightPline(&rb4ent,firstpt,isclosed,pt210,pt,gpak,curelev);
			CreateLightweightPline(&rb4ent,firstpt,isclosed,pt210,pt,gpak,newelev);
			}
		}
	else 
		{
        for (cmd_plinecur=plinetmp=cmd_plinebeg; cmd_plinecur!=NULL; plinetmp=cmd_plinecur) 
			{
            //*** Delete tmp pline segment.
            if (sds_entdel(cmd_plinecur->ename)!=RTNORM) 
				{
				rc=(-__LINE__); 
				goto bail; 
				}
            cmd_plinecur=cmd_plinecur->next;
            IC_FREE(plinetmp);
			}
	    }

	if (SDS_ElevationChanged)
		{
		rblast.restype=RTREAL;
		rblast.resval.rreal=curelev;
		sds_setvar("ELEVATION"/*DNT*/,&rblast);
		SDS_ElevationChanged=FALSE;
		}

    sds_entmake(NULL);
    IC_RELRB(rslt);
    IC_RELRB(gpak->arcdxf);
    if (rc==0L) 
		return(RTNORM);
    if (rc==(-1L)) 
		return(RTCAN);
    if (rc<(-1L)) 
		CMD_INTERNAL_MSG(rc);
    return(RTERROR);
}


