#include "cmds.h"
#include "cmdsDimBlock.h"
#include "IcadApi.h"
#include "IcadView.h"
#include "DbMText.h"

// 2003/3/31 Ebatech(CNBR) : cmd_dimaddXXXX() Add sLweight to support lineweight.
int	cmd_getCelweight(void)
{
	resbuf rb;
	SDS_getvar(NULL,DB_QCELWEIGHT, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES );
	return rb.resval.rint;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DIM_INITIALRBCS
//
// This function initializes the transformation structures for point transformation's, This function only initializes
// the params passed, any of the resbufs can be NULL.
//
// (MR) mode FALSE simply copies the extrusion from the plink pt210 group to the
// rbecs rpoint.  mode TRUE computes the extrusion as the cross product of the current
// X and Y UCS axes.  I'm not at the moment sure for what purpose this difference is used.
//
// return : void
//
void cmd_initialrbcs(struct resbuf *rbecs, struct resbuf *rbwcs, struct resbuf *rbucs, db_drawing *flp, sds_real* pExtrusion /* = NULL */)
{
    resbuf		rbb;
    sds_point   p1 = {0.0, 0.0, 0.0},
				pt210 = {0.0, 0.0, 1.0};

    if (rbwcs) 
	{
        rbwcs->restype    =RTSHORT;
        rbwcs->resval.rint=0;
    }
    if (rbucs) 
	{
        rbucs->restype    =RTSHORT;
        rbucs->resval.rint=1;
    }
    // Set up the (Z) axis of the current (UCS).
    if (rbecs) 
	{
        rbecs->restype=RT3DPOINT;
        if (pExtrusion == NULL) 
		{
            SDS_getvar(NULL,DB_QUCSXDIR,&rbb,flp,NULL,NULL);
            ic_ptcpy(p1,rbb.resval.rpoint);
            SDS_getvar(NULL,DB_QUCSYDIR,&rbb,flp,NULL,NULL);
            ic_crossproduct(p1,rbb.resval.rpoint,pt210);
            ic_ptcpy(rbecs->resval.rpoint,pt210);
        } 
		else 
		{
            ic_ptcpy(rbecs->resval.rpoint, pExtrusion);
        }
    }
}

void setColor(short *sColor)
{
    struct resbuf curColor;
    sds_getvar("CECOLOR"/*DNT*/,&curColor);
    if (*sColor == 0)
        *sColor = strisame(curColor.resval.rstring, "BYLAYER") ||
                  strisame(curColor.resval.rstring, "BYBLOCK")  ? 256 : atoi(curColor.resval.rstring);

    IC_FREE(curColor.resval.rstring);
    return;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DWG_ADDBLOCK
//
// This function puts the begining link called an "BLOCK" to the begining of the link list that makes up the dimension block.
//
// return : RTERROR if fail.
//        : other   if ok.
//
//
int dwg_addblock(char *szLayer,short sColor, char *szBlockName,short flag70,sds_point pt10,char *szXrefPath,db_drawing *pFlp) 
{
    struct resbuf *pRb=NULL;
    short          ret = 0;

//    setColor(&sColor);
    if ((pRb=sds_buildlist(RTDXF0,"BLOCK"/*DNT*/,8,szLayer,62,sColor,67,0,2,szBlockName,70,flag70,10,pt10,1,(szXrefPath!=NULL?szXrefPath:""/*DNT*/),0))==NULL) return(RTERROR);
    //put the table into the drawing
    if ((ret=SDS_entmake(pRb,pFlp))!=RTNORM) { IC_RELRB(pRb); return(RTERROR); }
    IC_RELRB(pRb);
    return(RTNORM);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DWG_ADDENDBLK
//
// This function puts the end link called an "ENDBLK" to the end of the link list that makes up the dimension block.
//
// return : RTERROR if fail.
//        : other   if ok.
//
int dwg_addendblk(db_drawing *pFlp) 
{
    struct resbuf *pRb=NULL;
    int            ret = 0;

    if ((pRb=sds_buildlist(RTDXF0,"ENDBLK"/*DNT*/,0))==NULL) return(RTERROR);
    //put the table into the drawing
    if ((ret=SDS_entmake(pRb,pFlp))==RTERROR) { IC_RELRB(pRb); return(RTERROR); }
    IC_RELRB(pRb);
    return(ret);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DIMADDCIRCLE
//
// If (dragmode)==1 then the dimension is being dragged and the line is not part of a block.
// If (dragmode)==0 then the dimension is being created and the line is part of a block definition.
//
// return : RTERROR if fail.
//        : RTNORM  ok.
//
int cmd_dimaddCircle(char *szLayer,short sColor,short sLweight, sds_point pt10,sds_real r40,db_drawing *pFlp,short dragmode) 
{
    struct resbuf *pEed=NULL;
    struct resbuf *pRb=NULL,
                  *pRbtmp1=NULL;
    int            ret = 0;
//    setColor(&sColor);

    if (dragmode==-1) return RTNORM;
	if (dragmode) {
		db_handitem *elp;
        sds_name     ename;
	    elp=new db_circle(pFlp);
        elp->set_6(DIM_BYBLOCK);
        elp->set_8(szLayer);
        elp->set_62(sColor);
        elp->set_370(sLweight);
        elp->set_10(pt10);
        elp->set_40(r40);
		elp->set_39(0.0);
	    ename[0]=(long)elp;
	    ename[1]=(long)pFlp;
	    SDS_UpdateEntDisp(ename,0);
	    sds_redraw(ename,IC_REDRAW_XOR);
        delete elp;
	} else {
		if ((pRb=sds_buildlist(RTDXF0,"CIRCLE"/*DNT*/,8,szLayer,62,sColor,370,sLweight,10,pt10,40,r40,6,DIM_BYBLOCK,39,0.0,0))==NULL) return(RTERROR);
        // if there is eed then append it to the end of the list
        if (pEed){
            for (pRbtmp1=pRb;pRbtmp1->rbnext!=NULL;pRbtmp1=pRbtmp1->rbnext);
            pRbtmp1->rbnext=pEed;
        }
        // entmake the object into the drawing
        if ((ret=SDS_entmake(pRb,pFlp))!=RTNORM) { IC_RELRB(pRb); return(RTERROR); }
        IC_RELRB(pRb);
	}
    return RTNORM;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DIMADDPOLYLINE
//
//
//
// return : RTERROR if fail.
//        : RTNORM  ok.
//
int cmd_dimaddPolyline(char *szLayer,short sColor,short sLweight,short flag66,sds_point pt10,short flag70,sds_real r40,sds_real r41,db_drawing *pFlp,short dragmode,db_handitem **elp) 
{
    struct resbuf *pEed=NULL;
    struct resbuf *pRb=NULL,
                  *pRbtmp1=NULL;
    int            ret = 0;

    setColor(&sColor);

    if (dragmode==-1) return RTNORM;
	if (dragmode) {
		db_handitem *elp = NULL;
        elp = new db_polyline(pFlp);

		if (NULL != elp)
		{
			elp->set_6(DIM_BYBLOCK);
			elp->set_8(szLayer);
			elp->set_62(sColor);
			elp->set_370(sLweight);
			elp->set_66(flag66);
			elp->set_10(pt10);
			elp->set_70(flag70);
			elp->set_40(r40);
			elp->set_41(r41);
			elp->set_39(0.0);
			sds_name ename;
			ename[0]=(long)elp;
			ename[1]=(long)pFlp;
			SDS_UpdateEntDisp(ename,0);
			sds_redraw(ename,IC_REDRAW_XOR);
			delete elp;
		}
	} else {
		if ((pRb=sds_buildlist(RTDXF0,"POLYLINE"/*DNT*/,8,szLayer,62,sColor,370,sLweight,66,flag66,10,pt10,70,flag70,40,r40,41,r41,6,DIM_BYBLOCK,39,0.0,0))==NULL) return(RTERROR);
        // if there is eed then append it to the end of the list
        if (pEed){
            for (pRbtmp1=pRb;pRbtmp1->rbnext!=NULL;pRbtmp1=pRbtmp1->rbnext);
            pRbtmp1->rbnext=pEed;
        }
        // entmake the object into the drawing
        if ((ret=SDS_entmake(pRb,pFlp))!=RTNORM) { IC_RELRB(pRb); return(RTERROR); }
        IC_RELRB(pRb);
	}
    return RTNORM;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DIMADDVERTEX
//
//
//
// return : RTERROR if fail.
//        : RTNORM  ok.
//
int cmd_dimaddVertex(char *szLayer,short sColor,short sLweight, sds_point pt10,sds_real r40,sds_real r41,sds_real r42,db_drawing *pFlp,short dragmode) 
{
    struct resbuf *pEed=NULL;
    struct resbuf *pRb=NULL,
                  *pRbtmp1=NULL;
    int            ret = 0;

    setColor(&sColor);

    if (dragmode==-1) return RTNORM;
	if (dragmode) {
		db_handitem *elp = NULL;
        elp=new db_vertex(pFlp);

		if (NULL != elp)
		{
			elp->set_6(DIM_BYBLOCK);
			elp->set_8(szLayer);
			elp->set_62(sColor);
			elp->set_370(sLweight);
			elp->set_10(pt10);
			elp->set_40(r40);
			elp->set_41(r41);
			elp->set_42(r42);
			elp->set_39(0.0);
			sds_name ename;
			ename[0]=(long)elp;
			ename[1]=(long)pFlp;
			SDS_UpdateEntDisp(ename,0);
			sds_redraw(ename,IC_REDRAW_XOR);
			delete elp;
		}
	} else {
		if ((pRb=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,8,szLayer,62,sColor,370,sLweight,10,pt10,40,r40,41,r41,42,r42,6,DIM_BYBLOCK,39,0.0,0))==NULL) return(RTERROR);
        // if there is eed then append it to the end of the list
        if (pEed){
            for (pRbtmp1=pRb;pRbtmp1->rbnext!=NULL;pRbtmp1=pRbtmp1->rbnext);
            pRbtmp1->rbnext=pEed;
        }
        // entmake the object into the drawing
        if ((ret=SDS_entmake(pRb,pFlp))!=RTNORM) { IC_RELRB(pRb); return(RTERROR); }
        IC_RELRB(pRb);
	}
    return RTNORM;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DIMADDSEQEND
//
// This function is used to put the end link in the link list of a polyline "SEQEND".
//
// return : RTERROR if fail.
//        : RTNORM  ok.
//
int cmd_dimaddSeqend(char *szLayer,short sColor,short sLweight, db_drawing *pFlp,short dragmode) 
{
    struct resbuf *pRb=NULL;
    RT ret = 0;

    setColor(&sColor);
    if (dragmode==-1) return RTNORM;
	if (dragmode) {
		db_handitem *elp = NULL;
        elp=new db_vertex(pFlp);

		if (NULL != elp)
		{
			elp->set_8(szLayer);
			elp->set_62(sColor);
			elp->set_370(sLweight);
			elp->set_39(0.0);
			sds_name ename;
			ename[0]=(long)elp;
			ename[1]=(long)pFlp;
			SDS_UpdateEntDisp(ename,0);
			sds_redraw(ename,IC_REDRAW_XOR);
			delete elp;
		}
	} else {
        //build the entity list
        if ((pRb=sds_buildlist(RTDXF0,"SEQEND"/*DNT*/,8,szLayer,62,sColor,370,sLweight,39,0.0,0))==NULL) return(RTERROR);
        //entmake the object into the drawing
        if ((ret=SDS_entmake(pRb,pFlp))!=RTNORM) { IC_RELRB(pRb); return(RTERROR); }
        IC_RELRB(pRb);
	}
    return RTNORM;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DIMADDARC
//
// If (dragmode)==1 then the dimension is being dragged and the arc is not part of a block.
// If (dragmode)==0 then the dimension is being created and the arc is part of a block definition.
//
// return : RTERROR if fail.
//        : RTNORM  ok.
//
//
int cmd_dimaddarc(char *szLayer,short sColor,short sLweight, sds_point pt10,sds_real r40,sds_real r50,sds_real r51,sds_point extrusion,db_drawing *pFlp,short dragmode)
{
    struct resbuf  *pEed=NULL;
    struct resbuf  *pRb=NULL,
                   *pRbtmp1=NULL,
                    rbucs,
                    rbecs;
    int             ret = 0;
    sds_point       pt1 = {0.0, 0.0, 0.0},
                    pt2 = {0.0, 0.0, 0.0},
                    pt3 = {0.0, 0.0, 0.0},
                    pt210 = {0.0, 0.0, 1.0};
    struct gr_view *view=SDS_CURGRVW;
//    setColor(&sColor);

    if (dragmode==-1) return RTNORM;
    cmd_initialrbcs(&rbecs,NULL,&rbucs,pFlp,extrusion);
    pt210[0]=pt210[1]=0.0;pt210[2]=1.0;
    // Calculate the angle of the text,since the UCS can be different then the WCS.
    pt2[0]=cos(r51);
    pt2[1]=sin(r51);
    pt2[2]=0.0;
    pt3[0]=cos(r50);
    pt3[1]=sin(r50);
    pt3[2]=0.0;
	if (dragmode) {
		db_handitem *elp;
        sds_name     ename;
        // If dragmode is 1 then transform the point from (UCS) to (ECS This is the Native format of the entity). This is becouse
        // the arc is not going to be part of a block definition when it is being dragged.
        sds_trans(pt10,&rbucs,&rbecs,0,pt1);
        sds_trans(pt3,&rbucs,&rbecs,1,pt3);
        if(fabs(pt3[0])>CMD_FUZZ || fabs(pt3[1])>CMD_FUZZ) r50=atan2(pt3[1],pt3[0]);
        sds_trans(pt2,&rbucs,&rbecs,1,pt2);
        if(fabs(pt2[0])>CMD_FUZZ || fabs(pt2[1])>CMD_FUZZ) r51=atan2(pt2[1],pt2[0]);
        elp=new db_arc(pFlp);
        elp->set_6(DIM_BYBLOCK);
        elp->set_8(szLayer);
        elp->set_62(sColor);
        elp->set_370(sLweight);
        elp->set_10(pt1);
        elp->set_40(r40);
        elp->set_50(r50);
        elp->set_51(r51);
        elp->set_210(view->ucsaxis[2]);
		elp->set_39(0.0);
	    ename[0]=(long)elp;
	    ename[1]=(long)pFlp;
	    SDS_UpdateEntDisp(ename,0);
	    sds_redraw(ename,IC_REDRAW_XOR);
        delete elp;
	} else {
        // Calcualte the (r50).
        sds_trans(pt3,&rbucs,&rbecs,1,pt3);
        if(fabs(pt3[0])>CMD_FUZZ || fabs(pt3[1])>CMD_FUZZ) r50=atan2(pt3[1],pt3[0]);
        // Calcualte the (r51).
        sds_trans(pt2,&rbucs,&rbecs,1,pt2);
        if(fabs(pt2[0])>CMD_FUZZ || fabs(pt2[1])>CMD_FUZZ) r51=atan2(pt2[1],pt2[0]);
        sds_trans(pt10,&rbucs,&rbecs,0,pt1); // pt1[2]=0.0;
		if ((pRb=sds_buildlist(RTDXF0,"ARC"/*DNT*/,8,szLayer,62,sColor,370,sLweight,10,pt1,40,r40,50,r50,51,r51,210,pt210,6,DIM_BYBLOCK,39,0.0,0))==NULL) return(RTERROR);
        // if there is eed then append it to the end of the list
        if (pEed) {
            for (pRbtmp1=pRb;pRbtmp1->rbnext!=NULL;pRbtmp1=pRbtmp1->rbnext);
            pRbtmp1->rbnext=pEed;
        }
        // entmake the object into the drawing
        if ((ret=SDS_entmake(pRb,pFlp))!=RTNORM) { IC_RELRB(pRb); return(RTERROR); }
        IC_RELRB(pRb);
	}
    return RTNORM;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DIMADDMTEXT
//
//
//
// return : RTERROR if fail.
//        : RTNORM  ok.
//
//
int cmd_dimaddmtext(char *szLayer,short sColor,short sLweight, sds_point pt10,sds_real r40,char *szText,sds_real r50,sds_real r41,char *szTextStyle,short flag71,sds_point extrusion,db_drawing *pFlp,short dragmode) 
{
//    struct resbuf *pEed=NULL;
    struct resbuf *pRb=NULL,
                  *pRbtmp1=NULL,
                    rbucs,
                    rbwcs,
                    rbecs;
    int            ret = 0;

    if (dragmode==-1) return RTNORM;

//    setColor(&sColor);

	// If the text style of this dimension overrides the text height, 
	// use it. 
	sds_real textHeight;
	db_handitem *textStyle = pFlp->tblsearch(DB_STYLETAB, szTextStyle, 0);
	ASSERT(textStyle != NULL);
	if (NULL == textStyle)
		return RTERROR;

	textStyle->get_40(&textHeight);
	if(!icadRealEqual(textHeight,0.0))	/*D.Gavrilov*/// We should override the text height only if 
		r40 = textHeight;	// the text style of this dimension is fixed (i.e. != 0.0).

    cmd_initialrbcs(&rbecs,&rbwcs,&rbucs,pFlp,extrusion);

	sds_point pt;						/*D.Gavrilov*/
	sds_trans(pt10,&rbucs,&rbecs,0,pt);	// pt10 should be translated into another pt

    if (dragmode) {
        sds_name     ename;
        CDbMText *elp = new CDbMText(pFlp);

        elp->set_6(DIM_BYBLOCK, (db_drawing *)pFlp);
        elp->set_8(szLayer, (db_drawing *)pFlp);
        elp->set_62(sColor);
        elp->set_370(sLweight);
        elp->set_10(pt);
        elp->set_40(r40);
        elp->set_41(r41); 
        elp->set_50(r50, (db_drawing *)pFlp);
        elp->set_1(szText);
        elp->set_7(szTextStyle, (db_drawing *)pFlp);
        elp->set_71(flag71);
		ename[0]=(long)elp;
		ename[1]=(long)pFlp;
		SDS_UpdateEntDisp(ename,0);
		sds_redraw(ename,IC_REDRAW_XOR);
		delete elp;
    } else {

        if ((pRb=sds_buildlist(RTDXF0,"MTEXT"/*DNT*/,8,szLayer,62,sColor,370,sLweight,10,pt,40,r40,1,szText,50,r50,41,r41,7,szTextStyle,71,flag71,6,DIM_BYBLOCK,0))==NULL) return(RTERROR);
//        // if there is eed then append it to the end of the list
//        if (pEed){
//            for (pRbtmp1=pRb;pRbtmp1->rbnext!=NULL;pRbtmp1=pRbtmp1->rbnext);
//            pRbtmp1->rbnext=pEed;
//        }
        // entmake the object into the drawing
        if ((ret=SDS_entmake(pRb,pFlp))!=RTNORM) { IC_RELRB(pRb); return(RTERROR); }
        IC_RELRB(pRb);
	}
    return RTNORM;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DIMADDSOLID
//
// If (dragmode)==1 then the dimension is being dragged and the solid is not part of a block.
// If (dragmode)==0 then the dimension is being created and the solid is part of a block definition.
//
// return : RTERROR if fail.
//        : RTNORM  ok.
//
//
int cmd_dimaddsolid(char *szLayer,short sColor,short sLweight, sds_point pt10,sds_point pt11,sds_point pt12,sds_point pt13,db_drawing *pFlp,short dragmode,short flags,sds_point init210) {
    struct resbuf *pEed=NULL;
    struct resbuf *pRb=NULL,
                  *pRbtmp1=NULL,
                   rbucs,
                   rbwcs,
                   rbecs;
    int            ret = 0;
    sds_point      pt1 = {0.0, 0.0, 0.0},
                   pt2 = {0.0, 0.0, 0.0},
                   pt3 = {0.0, 0.0, 0.0},
                   pt4 = {0.0, 0.0, 0.0},
                   pt210 = {0.0, 0.0, 1.0};
    struct gr_view *view=SDS_CURGRVW;
//    setColor(&sColor);

    if (dragmode==-1) return RTNORM;
    pt210[0]=pt210[1]=0.0;pt210[2]=1.0;
    cmd_initialrbcs(&rbecs,&rbwcs,&rbucs,pFlp,init210);
	if (dragmode) {
		db_handitem *elp;
        sds_name     ename;

        // If dragmode is 1 then transform the point from (UCS) to (ECS This is the Native format of the entity). This is becouse
        // the solid is not going to be part of a block definition when it is being dragged.
        sds_trans(pt10,&rbucs,&rbecs,0,pt1);
        sds_trans(pt11,&rbucs,&rbecs,0,pt2);
        sds_trans(pt12,&rbucs,&rbecs,0,pt3);
        sds_trans(pt13,&rbucs,&rbecs,0,pt4);

        elp=new db_solid(pFlp);
        elp->set_6(DIM_BYBLOCK);
        elp->set_8(szLayer);
        elp->set_62(sColor);
        elp->set_370(sLweight);
        elp->set_10(pt1);
        elp->set_11(pt2);
        elp->set_12(pt3);
        elp->set_13(pt4);
        elp->set_210(view->ucsaxis[2]);
		elp->set_39(0.0);
	    ename[0]=(long)elp;
	    ename[1]=(long)pFlp;
	    SDS_UpdateEntDisp(ename,0);
	    sds_redraw(ename,IC_REDRAW_XOR);
        delete elp;
	} else {
		if(flags & DIM_ADD_TO_BLOCK)
		{
			ASSERT((flags & DIM_ADD_ASSOC) == 0);
			if ((pRb=sds_buildlist(RTDXF0,"SOLID"/*DNT*/,8,szLayer,62,sColor,370,sLweight,10,pt10,11,pt11,12,pt12,13,pt13,210,pt210,6,DIM_BYBLOCK,39,0.0,0))==NULL) 
				return(RTERROR);
		}
		else
	        if (flags & DIM_ADD_ASSOC) {
		        sds_trans(pt10,&rbucs,&rbecs,0,pt1); // pt1[2]=0.0;
			    sds_trans(pt11,&rbucs,&rbecs,0,pt2); // pt2[2]=0.0;
				sds_trans(pt12,&rbucs,&rbecs,0,pt3); // pt3[2]=0.0;
				sds_trans(pt13,&rbucs,&rbecs,0,pt4); // pt4[2]=0.0;
				if ((pRb=sds_buildlist(RTDXF0,"SOLID"/*DNT*/,8,szLayer,62,sColor,370,sLweight,10,pt1,11,pt2,12,pt3,13,pt4,210,pt210,6,DIM_BYBLOCK,39,0.0,0))==NULL) return(RTERROR);
			} else {
				sds_trans(pt10,&rbucs,&rbecs,0,pt1);
				sds_trans(pt11,&rbucs,&rbecs,0,pt2);
				sds_trans(pt12,&rbucs,&rbecs,0,pt3);
				sds_trans(pt13,&rbucs,&rbecs,0,pt4);
				if ((pRb=sds_buildlist(RTDXF0,"SOLID"/*DNT*/,8,szLayer,62,sColor,370,sLweight,10,pt1,11,pt2,12,pt3,13,pt4,210,view->ucsaxis[2],6,DIM_BYBLOCK,39,0.0,0))==NULL) return(RTERROR);
			}
		// if there is eed then append it to the end of the list
		if (pEed){
			for (pRbtmp1=pRb;pRbtmp1->rbnext!=NULL;pRbtmp1=pRbtmp1->rbnext);
			pRbtmp1->rbnext=pEed;
		}
		// entmake the object into the drawing
		if ((ret=SDS_entmake(pRb,pFlp))!=RTNORM) { IC_RELRB(pRb); return(RTERROR); }
		IC_RELRB(pRb);
	}
    return RTNORM;
}

#if 0
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DIMADDTEXT
//
// If (dragmode)==1 then the dimension is being dragged and the text is not part of a block.
// If (dragmode)==0 then the dimension is being created and the text is part of a block definition.
//
// return : RTERROR if fail.
//        : RTNORM  ok.
//
int cmd_dimaddtext(char *szLayer,short sColor,short sLweight, sds_point pt10,sds_real r40,char *szText,sds_real r50,sds_real r41,
                    sds_real r51,char *szTextStyle,short flag72,short flag73,sds_point pt11,db_drawing *pFlp,short dragmode) 
{
    struct resbuf  *pEed=NULL;
    struct resbuf  *pRb=NULL,
                   *pRbtmp1=NULL,
                    rbucs,
                    rbwcs,
                    rbecs;
    int             ret = 0;
    sds_point       pt1 = {0.0, 0.0, 0.0},
                    pt2 = {0.0, 0.0, 0.0},
                    pt3 = {0.0, 0.0, 0.0},
                    pt210 = {0.0, 0.0, 1.0};
    sds_real        fr1 = 0.0;
    struct gr_view *view=SDS_CURGRVW;

//    setColor(&sColor);

    if (dragmode==-1) 
        return RTNORM;
    cmd_initialrbcs(&rbecs,&rbwcs,&rbucs,pFlp,plink->pt210);
    pt210[0]=pt210[1]=0.0;pt210[2]=1.0;
    // Calculate the angle of the text,since the UCS can be different then the WCS.
    pt3[0]=cos(r50); pt3[1]=sin(r50); pt3[2]=0.0;
    if (dragmode) {
        db_handitem *elp;
        sds_name     ename;
        // If dragmode is 1 then transform the point from (UCS) to (ECS This is the Native format of the entity). This is becouse
        // the text is not going to be part of a block definition when it is being dragged.
        sds_trans(pt10,&rbucs,&rbecs,0,pt1);
        sds_trans(pt11,&rbucs,&rbecs,0,pt2);
        sds_trans(pt3,&rbucs,&rbecs,1,pt3);
		// Calculate the 50 group.
        if (fabs(pt3[0])>CMD_FUZZ || fabs(pt3[1])>CMD_FUZZ) r50=atan2(pt3[1],pt3[0]);

        elp=new db_text(pFlp);
        elp->set_6(DIM_BYBLOCK);
        elp->set_8(szLayer);
        elp->set_62(sColor);
        elp->set_370(sLweight);
        elp->set_10(pt1);
        elp->set_11(pt2);
        elp->set_210(view->ucsaxis[2]);
        elp->set_40(r40);
        elp->set_41(r41);
        elp->set_50(r50);
        elp->set_51(r51);
        elp->set_1(szText);
        elp->set_7(szTextStyle,pFlp);
        elp->set_72(flag72);
        elp->set_73(flag73);

		CDC *dc = NULL;
		CIcadView *icadView = SDS_CMainWindow->GetIcadView();
		if (icadView)
			{
			ASSERT_KINDOF(CIcadView, icadView);
			dc = icadView->GetTargetDeviceCDC();
			}
		else
			{
			delete elp;
			return(RTERROR);
			}

        gr_fixtextjust(elp, pFlp, NULL, dc, 0);
		ename[0]=(long)elp;
		ename[1]=(long)pFlp;
		SDS_UpdateEntDisp(ename,0);
		sds_redraw(ename,IC_REDRAW_XOR);
		delete elp;
	} else {
	    db_ucshelper(dimucs[0],dimucs+1,dimelev,pt1,pt2,&fr1);
//        setColor(&sColor);

        if (plinkvar[DIMASO].dval.ival) {
            // Calculate the text angle.
            sds_trans(pt3,&rbucs,&rbecs,1,pt3);
            if(fabs(pt3[0])>CMD_FUZZ || fabs(pt3[1])>CMD_FUZZ) r50=atan2(pt3[1],pt3[0]);
            sds_trans(pt10,&rbucs,&rbecs,0,pt1); pt1[2]=0.0;
            sds_trans(pt10,&rbucs,&rbecs,0,pt2); pt2[2]=0.0;
            if ((pRb=sds_buildlist(RTDXF0,"TEXT"/*DNT*/,8,szLayer,62,sColor,370,sLweight,10,pt1,40,r40,1,szText,50,r50,41,r41,50,r50,51,r51,7,szTextStyle,72,flag72,73,flag73,11,pt2,210,pt210,6,DIM_BYBLOCK,0))==NULL) return(RTERROR);
        } else {
            sds_trans(pt10,&rbucs,&rbecs,0,pt1);
            sds_trans(pt11,&rbucs,&rbecs,0,pt2);
            sds_trans(pt3,&rbucs,&rbecs,1,pt3);
            if(fabs(pt3[0])>CMD_FUZZ || fabs(pt3[1])>CMD_FUZZ) r50=atan2(pt3[1],pt3[0]);
            if ((pRb=sds_buildlist(RTDXF0,"TEXT"/*DNT*/,8,szLayer,62,sColor,370,sLweight,10,pt1,40,r40,1,szText,50,r50,41,r41,50,r50,51,r51,7,szTextStyle,72,flag72,73,flag73,11,pt2,210,view->ucsaxis[2],6,DIM_BYBLOCK,0))==NULL) return(RTERROR);
        }
        // if there is eed then append it to the end of the list
        if (pEed){
            for (pRbtmp1=pRb;pRbtmp1->rbnext!=NULL;pRbtmp1=pRbtmp1->rbnext);
            pRbtmp1->rbnext=pEed;
        }
        // entmake the object into the drawing
        if ((ret=SDS_entmake(pRb,pFlp))!=RTNORM) { IC_RELRB(pRb); return(RTERROR); }
        IC_RELRB(pRb);
	}
    return RTNORM;
}
#endif


