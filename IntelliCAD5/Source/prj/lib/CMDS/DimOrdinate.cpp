// =========================================================================================================================================
// ================================================================ Includes ===============================================================
// =========================================================================================================================================

#include "cmds.h"/*DNT*/
//BugZilla No. 78155; 27-05-2002 
#include "IcadDoc.h"/*DNT*/

#include "dimensions.h"
#include "cmdsDimVars.h"
#include "IcadView.h"
#include "cmdsDimBlock.h"

// =========================================================================================================================================
// =========================================================== External Variables ==========================================================
// =========================================================================================================================================

extern                        SDS_AtNodeDrag;
extern short        cmd_dimmode;    // Flag specifying if the user is at the "Dim:" prompt (dimmode=1).
extern sds_point    dimzeropt; // Needed to put this variable here because it gets initialized.
extern char         dimlsttext[256], *layername;
extern struct cmd_dimvars plinkvar[];
//BugZilla No. 78155; 27-05-2002
//extern struct cmd_dimlastpt lastdimpts;
extern struct cmd_dimeparlink* plink;

// ============================================================================================================================================================
// ========================================================= ORDINATE DIMENSION FUNCTIONS =====================================================================
// ============================================================================================================================================================
//
//
//
//
// ============================================================================================================================================================

BOOL cmd_Ordinate(struct cmd_dimeparlink *plink,db_drawing *flp,short dragmode) {
    char           text[256] = ""/*DNT*/;
    int            fi1 = 0;
    struct resbuf  rb,
                  *rbb=NULL;
    sds_real       offset1 = 0.0,
                   offset2 = 0.0,
                   anglec = 0.0;
    sds_point      ptd[5]={{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0}}, // this is the dimension lines points.
                   ptt[4]={{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0}},   // this is a point array to be used as a temporary value.
                   ipt={0.0,0.0,0.0},
                   xpt[4]={{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0}},
                   txtbox[2]={{0.0,0.0,0.0},{0.0,0.0,0.0}};
    BOOL           dirflag = FALSE,
                   angflag = FALSE,
                   diagflg = FALSE;
    short          dimtxthalign=1,	                                                    // text alignment mode - horiz
				   dimtxtvalign=2,	                                                    // text alignment mode - vert
                   LoopFlag=FALSE;
    db_handitem   *handitem=NULL;
	int	celweight = cmd_getCelweight();

    do {
        //
        // This is the (Ordinate).                   ____ ptd[2]
        //                                          /
        //                                         /      _______ ptd[0]
        //  Ordinate               ptd[1]____     /      /
        // ----------                        \   ______  Text
        // pt13 = First point picked.         \ /
        // pt14 = Seconed point picked.   _____/  <------- Ordinate dimension. This dimension can start from a point and go any direction.
        //                                \
        //                                 \____ ptd[3]
        //
        ic_ptcpy(ptd[3],plink->pt13);                         // First point picked.
        ic_ptcpy(ptd[0],plink->pt14);                         // Seconed point picked.
        offset1=fabs(ptd[0][1]-ptd[3][1]);                      // Get the offset in the (Y) direction between point pt14 and pt13.
        offset2=fabs(ptd[0][0]-ptd[3][0]);                      // Get the offset in the (X) direction between point pt14 and pt13.
        // If the (Y) offset is bigger then set to 1, if the (X) offset is bigger then set to 0;
        if (SDS_AtNodeDrag || plink->DefValidflg) angflag=(plink->flags&64)?1:0; else angflag=(offset1>offset2)?1:0;
        // If elnang is set to 1.0 then the (X) ordinate or (Y) ordinate was set.
        if (plink->elnang) angflag=(((!dragmode)?(plink->flags^DIM_BIT_6):plink->flags)==6)?0:1;
        // If by (Y) then find which direction in the (Y) plane are we going, If by (X) then find which direction in the (X) plane are we going.
        dirflag=(plink->pt13[(angflag)?1:0]<plink->pt14[(angflag)?1:0])?1:0;
        // If by (Y) check if the (Y) are the same, If by (X) check if the (X) are the same.
		//BugZilla No. 78198 ; 01-07-2002
		// EBATECH(CNBR) -[ 13-03-2003 DiagLength = 2.0 * DIMASZ * DIMSCALE
        //if (ptd[0][(angflag)?0:1]!=ptd[3][(angflag)?0:1]) {
		if (!icadRealEqual(ptd[0][(angflag)?0:1],ptd[3][(angflag)?0:1])) {
            // This block of code does the diag. in the dimension, sometimes the dimension is only a line no diag.
            sds_real rdiag;
            rdiag = fabs( plinkvar[DIMASZ].dval.rval * 2.0 * ( icadRealEqual( plinkvar[DIMSCALE].dval.rval, 0.0 ) ? 1.0 : plinkvar[DIMSCALE].dval.rval ));
            diagflg = TRUE;
            ic_ptcpy(ptd[1],ptd[0]);
            ptd[1][(angflag)?1:0]+=(dirflag)?-(rdiag):rdiag;
            ic_ptcpy(ptd[2],ptd[1]);
            ptd[2][(angflag)?1:0]+=(dirflag)?-(rdiag):rdiag;
            ptd[2][(angflag)?0:1]=ptd[3][(angflag)?0:1];
            ptd[2][(angflag)?1:0]=(((angflag)?offset1:offset2)<(3*rdiag))?((dirflag)?(ptd[3][(angflag)?1:0]+rdiag):(ptd[3][(angflag)?1:0]-rdiag)):ptd[2][(angflag)?1:0];
        }else{ diagflg = FALSE; }
		// EBATECH(CNBR) ]-
        if (!dragmode && angflag) plink->flags|=64;

        plink->dimvalue=ptd[3][(angflag)?0:1];
        anglec=(angflag)?DIM_90_DEG:0.0;
        // Generate string for dimension.
        cmd_genstring(plink,text,(fabs(plink->dimvalue)),""/*DNT*/,TRUE,flp);
        // Buld the text insert.
        if((rbb=sds_buildlist(RTDXF0  ,"MTEXT"/*DNT*/,
                                   8  ,layername,
                                   6  ,"BYLAYER"/*DNT*/,
                                   39 ,0.0,
                                   62 ,plinkvar[DIMCLRT].dval.ival,
                                   370,plink->lweight,
                                   67 ,0,
                                   210,ptt[1],
                                   10 ,ptt[0],
                                   40 ,(plinkvar[DIMTXT].dval.rval*plinkvar[DIMSCALE].dval.rval),
                                   1  ,text,
                                   50 ,anglec,
                                   41 ,0.0,
                                   51 ,plink->hdir,
                                   7  ,plinkvar[DIMTXSTY].dval.cval,
                                   71 ,0,
                                   72 ,dimtxthalign,
                                   73 ,dimtxtvalign,
                                   11 ,plink->textmidpt,0))==NULL) return(RTERROR);	// Can this create rbb before returning?
        // Call function to get the width and height of the text.
        sds_textbox(rbb,txtbox[0],txtbox[1]);
        IC_RELRB(rbb);

        if (SDS_AtNodeDrag==2 && !LoopFlag) {
			// EBATECH(CNBR) 13-03-2003 Bugzilla#78455 DIMTMOVE=2
			if( !plink->HomeText && plinkvar[DIMTMOVE].dval.ival != 2 )
			{
				ic_ptcpy(ptt[0],plink->textmidpt);
				ptt[0][(angflag)?1:0]-=(txtbox[1][(angflag)?1:0]);
				ic_ptcpy(plink->pt14,ptt[0]);
				LoopFlag=TRUE;
			}
        } else if (LoopFlag) break;
    } while(LoopFlag);

	if( plink->HomeText || plinkvar[DIMTMOVE].dval.ival != 2 )
	{
		ic_ptcpy(ptt[0],plink->pt14);

		//BugZilla No. 78197 ; 01-07-2002 

		// calculate the text alignment pt & set the text alignment
		if (!angflag && plink->pt14[0]<plink->pt13[0]) {
			// This does the dimension going to the left.
			ptd[3][(angflag)?1:0]-=(plinkvar[DIMEXO].dval.rval*plinkvar[DIMSCALE].dval.rval);
			ptt[0][(angflag)?1:0]-=(fabs(plinkvar[DIMGAP].dval.rval*plinkvar[DIMSCALE].dval.rval)+(txtbox[1][0]/2));
		} else if(angflag && plink->pt14[1]<plink->pt13[1]) {
			// This does the dimension going to the bottom.
			ptd[3][(angflag)?1:0]-=(plinkvar[DIMEXO].dval.rval*plinkvar[DIMSCALE].dval.rval);
			ptt[0][(angflag)?1:0]-=(fabs(plinkvar[DIMGAP].dval.rval*plinkvar[DIMSCALE].dval.rval)+(txtbox[1][1]/2));
		} else if (angflag) {
			// This does the dimension going to the top.
			ptd[3][(angflag)?1:0]+=(plinkvar[DIMEXO].dval.rval*plinkvar[DIMSCALE].dval.rval);
			ptt[0][(angflag)?1:0]+=(fabs(plinkvar[DIMGAP].dval.rval*plinkvar[DIMSCALE].dval.rval)+(txtbox[1][1]/2));
		} else {
			// This does the dimension going to the right.
			ptd[3][(angflag)?1:0]+=(plinkvar[DIMEXO].dval.rval*plinkvar[DIMSCALE].dval.rval);
			ptt[0][(angflag)?1:0]+=(fabs(plinkvar[DIMGAP].dval.rval*plinkvar[DIMSCALE].dval.rval)+(txtbox[1][0]/2));
		}

		if(plinkvar[DIMTAD].dval.ival!=0)
		{
			if(angflag)
				ptt[0][(angflag)?0:1]-=GetUpperDimgap();
			else
				ptt[0][(angflag)?0:1]+=GetUpperDimgap();
		}
	
		// If dimtad is not zero then this will put the dimension text above the dimension line.
		if (plinkvar[DIMTAD].dval.ival!=0) 
		{
			ic_ptcpy(ptd[4],ptd[0]);
			if (!angflag && plink->pt14[0]<plink->pt13[0]) 
				ptd[4][(angflag)?1:0]-=txtbox[1][0]+fabs(plinkvar[DIMGAP].dval.rval*plinkvar[DIMSCALE].dval.rval);
			else if (angflag && plink->pt14[1]<plink->pt13[1])
				ptd[4][(angflag)?1:0]-=txtbox[1][1]+fabs(plinkvar[DIMGAP].dval.rval*plinkvar[DIMSCALE].dval.rval);
			else if (angflag) 
				ptd[4][(angflag)?1:0]+=txtbox[1][1]+fabs(plinkvar[DIMGAP].dval.rval*plinkvar[DIMSCALE].dval.rval);
			else 
				ptd[4][(angflag)?1:0]+=txtbox[1][0]+fabs(plinkvar[DIMGAP].dval.rval*plinkvar[DIMSCALE].dval.rval);
		}
	}

	// Create the dimenstion block.
	if (!dragmode && plinkvar[DIMASO].dval.ival) if(RTNORM!=dwg_addblock(layername,256,"*D"/*DNT*/,1,ipt,NULL,flp)) goto exit;
    if (plink->textrot) anglec=plink->textrot;
    
	if( plink->HomeText || plinkvar[DIMTMOVE].dval.ival != 2 )
		ic_ptcpy(plink->textmidpt,ptt[0]);
    else
    	ic_ptcpy(ptt[0],plink->textmidpt);
    
	if (RTNORM!=cmd_dimaddmtext(layername,plinkvar[DIMCLRT].dval.ival,plink->lweight,ptt[0],plinkvar[DIMTXT].dval.rval*plinkvar[DIMSCALE].dval.rval,text,anglec,0.0,plinkvar[DIMTXSTY].dval.cval,5,plink->pt210,flp,dragmode)) goto exit;
    if (RTNORM!=cmd_dimaddpoint(DIM_DEFPOINTS,plinkvar[DIMCLRE].dval.ival,celweight,plink->pt13,flp,dragmode)) goto exit;

	//BugZilla No. 78198 ; 01-07-2002 [
	if (diagflg /*ptd[1][0]*/) //leader line has 4 points.
	{
		//BugZilla No. 78198 ; 09-12-2002
		// Implemented code-review changes as suggested by Denis
		//for (fi1=3;fi1>1;--fi1) 
		//	if(RTNORM!=cmd_dimaddline(layername,plinkvar[DIMCLRD].dval.ival,ptd[fi1],ptd[fi1-1],flp,dragmode)) goto exit;
		if(RTNORM!=cmd_dimaddline(layername,plinkvar[DIMCLRE].dval.ival,plinkvar[DIMLWE].dval.ival,ptd[3],ptd[2],flp,dragmode)) goto exit;
		if(RTNORM!=cmd_dimaddline(layername,plinkvar[DIMCLRE].dval.ival,plinkvar[DIMLWE].dval.ival,ptd[2],ptd[1],flp,dragmode)) goto exit;

        if (plinkvar[DIMTAD].dval.ival!=0) 
			cmd_dimaddline(layername,plinkvar[DIMCLRE].dval.ival,plinkvar[DIMLWE].dval.ival,ptd[1],ptd[4],flp,dragmode);
		else
			cmd_dimaddline(layername,plinkvar[DIMCLRE].dval.ival,plinkvar[DIMLWE].dval.ival,ptd[1],ptd[0],flp,dragmode);
	}
	else // leader line is horizontal or vertical.
	{
		if (plinkvar[DIMTAD].dval.ival!=0) 
			cmd_dimaddline(layername,plinkvar[DIMCLRE].dval.ival,plinkvar[DIMLWE].dval.ival,ptd[3],ptd[4],flp,dragmode);
		else
			cmd_dimaddline(layername,plinkvar[DIMCLRE].dval.ival,plinkvar[DIMLWE].dval.ival,ptd[3],ptd[0],flp,dragmode);
	}
	//BugZilla No. 78198 ; 01-07-2002 ]

    if ((plinkvar[DIMGAP].dval.rval*plinkvar[DIMSCALE].dval.rval)<0.0) 
	{ 
		for (fi1=0;fi1<4;++fi1) 
			ic_ptcpy(xpt[fi1],ptt[0]);
		
		txtbox[1][0]+=(fabs(plinkvar[DIMGAP].dval.rval*plinkvar[DIMSCALE].dval.rval)*2);
		txtbox[1][1]+=(fabs(plinkvar[DIMGAP].dval.rval*plinkvar[DIMSCALE].dval.rval)*2);

        xpt[0][0]-=(txtbox[1][0]/2); xpt[1][0]+=(txtbox[1][0]/2);
        xpt[2][0]-=(txtbox[1][0]/2); xpt[3][0]+=(txtbox[1][0]/2);
        
		xpt[0][1]+=(txtbox[1][1]/2); xpt[1][1]+=(txtbox[1][1]/2);
        xpt[2][1]-=(txtbox[1][1]/2); xpt[3][1]-=(txtbox[1][1]/2);

		if (!cmd_drawbox(plinkvar[DIMCLRD].dval.ival,plinkvar[DIMLWD].dval.ival,xpt,flp,dragmode)) goto exit; 
	}
	strcpy(dimlsttext,text);
	// End create block.
	if (!dragmode && plinkvar[DIMASO].dval.ival) {
		if (RTERROR==dwg_addendblk(flp)) goto exit;
		handitem=flp->ret_lastblockdef();
		handitem->get_2(plink->bname,sizeof(plink->bname)-1);
	}
	return(TRUE);
exit:
	rb.restype    =RTSHORT;
	rb.resval.rint=OL_EMKANON;
	SDS_setvar("ERRNO"/*DNT*/,-1,&rb,flp,NULL,NULL,0);
	return(FALSE);
}
//BugZilla No. 78197 ; 01-07-2002 [
//old code - removed
#ifdef false
	// calculate the text alignment pt & set the text alignment
	if (!angflag && plink->pt14[0]<plink->pt13[0]) {
        // This does the dimension going to the left.
        ptd[3][(angflag)?1:0]-=(plinkvar[DIMEXO].dval.rval*plinkvar[DIMSCALE].dval.rval);
		ptt[0][(angflag)?1:0]-=(fabs(plinkvar[DIMGAP].dval.rval*plinkvar[DIMSCALE].dval.rval)+(txtbox[1][0]/2));
        if (plinkvar[DIMTAD].dval.ival==1) ptt[0][(angflag)?0:1]+=(fabs(plinkvar[DIMGAP].dval.rval*plinkvar[DIMSCALE].dval.rval)*3);
	} else if(angflag && plink->pt14[1]<plink->pt13[1]) {
        // This does the dimension going to the bottom.
        ptd[3][(angflag)?1:0]-=(plinkvar[DIMEXO].dval.rval*plinkvar[DIMSCALE].dval.rval);
		//BugZilla No. 78010 ; 17-06-2002
		//ptt[0][(angflag)?1:0]-=(fabs(plinkvar[DIMGAP].dval.rval*plinkvar[DIMSCALE].dval.rval)+(txtbox[1][0]/2));
		ptt[0][(angflag)?1:0]-=(fabs(plinkvar[DIMGAP].dval.rval*plinkvar[DIMSCALE].dval.rval)+(txtbox[1][1]/2));
        if (plinkvar[DIMTAD].dval.ival==1) ptt[0][(angflag)?0:1]-=(fabs(plinkvar[DIMGAP].dval.rval*plinkvar[DIMSCALE].dval.rval)*3);
	} else if (angflag) {
        // This does the dimension going to the top.
        ptd[3][(angflag)?1:0]+=(plinkvar[DIMEXO].dval.rval*plinkvar[DIMSCALE].dval.rval);
		//BugZilla No. 78010 ; 17-06-2002
		//ptt[0][(angflag)?1:0]+=(fabs(plinkvar[DIMGAP].dval.rval*plinkvar[DIMSCALE].dval.rval)+(txtbox[1][0]/2));
		ptt[0][(angflag)?1:0]+=(fabs(plinkvar[DIMGAP].dval.rval*plinkvar[DIMSCALE].dval.rval)+(txtbox[1][1]/2));
        if (plinkvar[DIMTAD].dval.ival==1) ptt[0][(angflag)?0:1]-=(fabs(plinkvar[DIMGAP].dval.rval*plinkvar[DIMSCALE].dval.rval)*3);
    } else {
        // This does the dimension going to the right.
        ptd[3][(angflag)?1:0]+=(plinkvar[DIMEXO].dval.rval*plinkvar[DIMSCALE].dval.rval);
		ptt[0][(angflag)?1:0]+=(fabs(plinkvar[DIMGAP].dval.rval*plinkvar[DIMSCALE].dval.rval)+(txtbox[1][0]/2));
        if (plinkvar[DIMTAD].dval.ival==1) ptt[0][(angflag)?0:1]+=(fabs(plinkvar[DIMGAP].dval.rval*plinkvar[DIMSCALE].dval.rval)*3);
	}
    // This calculates the text box size.
    // Caluculate the distance buffer around the text.
    txtbox[1][0]+=(fabs(plinkvar[DIMGAP].dval.rval*plinkvar[DIMSCALE].dval.rval)*2);
    ic_ptcpy(xpt[0],ptd[0]); ic_ptcpy(xpt[2],ptd[0]);
    xpt[0][(angflag)?0:1]+=-txtbox[1][1];
    xpt[2][(angflag)?0:1]+=txtbox[1][1];
    ic_ptcpy(xpt[1],xpt[0]); ic_ptcpy(xpt[3],xpt[2]);
    xpt[1][(angflag)?1:0]+=((dirflag)?txtbox[1][0]:-txtbox[1][0]);
    xpt[3][(angflag)?1:0]+=((dirflag)?txtbox[1][0]:-txtbox[1][0]);
    // If dimtad is one then this will put the dimension text above the dimension line.
    if (plinkvar[DIMTAD].dval.ival==1) {
        ic_ptcpy(ptd[4],ptd[0]);
	    // calculate the text alignment pt & set the text alignment
	    if (!angflag && plink->pt14[0]<plink->pt13[0]) {
            ptd[4][(angflag)?1:0]-=txtbox[1][0];
            for (fi1=0;fi1<4;++fi1) xpt[fi1][(angflag)?0:1]+=(txtbox[1][1]+fabs(plinkvar[DIMGAP].dval.rval*plinkvar[DIMSCALE].dval.rval));
        } else if (angflag && plink->pt14[1]<plink->pt13[1]) {
            ptd[4][(angflag)?1:0]-=txtbox[1][0];
            for (fi1=0;fi1<4;++fi1) xpt[fi1][(angflag)?0:1]-=(txtbox[1][1]+fabs(plinkvar[DIMGAP].dval.rval*plinkvar[DIMSCALE].dval.rval));
        } else if (angflag) {
            ptd[4][(angflag)?1:0]+=txtbox[1][0];
            for (fi1=0;fi1<4;++fi1) xpt[fi1][(angflag)?0:1]-=(txtbox[1][1]+fabs(plinkvar[DIMGAP].dval.rval*plinkvar[DIMSCALE].dval.rval));
        } else {
            ptd[4][(angflag)?1:0]+=txtbox[1][0];
            for (fi1=0;fi1<4;++fi1) xpt[fi1][(angflag)?0:1]+=(txtbox[1][1]+fabs(plinkvar[DIMGAP].dval.rval*plinkvar[DIMSCALE].dval.rval));
        }
    }
    // Create the dimenstion block.
	if (!dragmode && plinkvar[DIMASO].dval.ival) if(RTNORM!=dwg_addblock(layername,256,"*D"/*DNT*/,1,ipt,NULL,flp)) goto exit;
    if (plink->textrot) anglec=plink->textrot;
    // This is the text for all the dim types.
/*    if (plink->textmidpt[0] || plink->textmidpt[1]) ic_ptcpy(ptt[0],plink->textmidpt); else */ic_ptcpy(plink->textmidpt,ptt[0]);
    //
    // the paramaters to this function are as follows.
    // Param.               desc.
    // ----------------------------------------------------------------------------------------------
    //    1  If this is null it defaults layer to current dwg layer.
    //    2  if this is null it defaults linetype to "BYLAYER".
    //    3  This is the Thickness of the line.
    //    4  Color of dimension text.
    //    5  This is the scale.
    //    6  Invisable flag,should be 0 for default.
    //    7  Paperspace flag, should be set to 0.
    //    8  This is the ADE lock flag, should be set to 0.
    //    9  If null it defaults to WCS (0,0,1)
    //   10  Insertion point of text.
    //   11  This is the height of the dimension text.
    //   12  Text to insert.
    //   13  Rotation angle.
    //   14  Relative X-scale factor,default 0.
    //   15  Oblique angle.
    //   16  Textstyle, if this is null it defaults to "STANDARD"
    //   17  Text generation flags(2=backward,4=upsidedown).
    //   18  Horizontal alignment,(0=left,1=center,2=right,3=aligned,4=middle,5=fit).
    //   19  Vertical alignment,(0=baseline,1=bottom,2=middle,3=top).
    //   20  Alignment point, present only if Horizontal or Vertical alignment is present or nonzero.
    //                               1----------------------------2---------------------3------------------------4---------------------5----6------7--------8-----------------------------9------------------------10------------11--------12----13----14
    //                               |                            |                     |                        |                     |    |      |        |                             |                        |             |         |     |     |
    if (RTNORM!=cmd_dimaddmtext(layername,plinkvar[DIMCLRT].dval.ival,ptt[0],plinkvar[DIMTXT].dval.rval*plinkvar[DIMSCALE].dval.rval,text,anglec,0.0,plinkvar[DIMTXSTY].dval.cval,5,plink->pt210,flp,dragmode)) goto exit;
    if (RTNORM!=cmd_dimaddpoint(DIM_DEFPOINTS,plinkvar[DIMCLRE].dval.ival,plink->pt13,flp,dragmode)) goto exit;
    if (ptd[1][0]) {
        for (fi1=0;fi1<3;++fi1) if(RTNORM!=cmd_dimaddline(layername,plinkvar[DIMCLRD].dval.ival,ptd[fi1],ptd[fi1+1],flp,dragmode)) goto exit;
        if (plinkvar[DIMTAD].dval.ival==1) cmd_dimaddline(layername,plinkvar[DIMCLRD].dval.ival,ptd[0],ptd[4],flp,dragmode);
    } else {
        if (plinkvar[DIMTAD].dval.ival==1) {
	        if (!angflag && plink->pt14[0]<plink->pt13[0]) {
                ptd[0][(angflag)?1:0]-=txtbox[1][0];
            } else if (angflag && plink->pt14[1]<plink->pt13[1]) {
                ptd[0][(angflag)?1:0]-=txtbox[1][0];
            } else if (angflag) {
                ptd[0][(angflag)?1:0]+=txtbox[1][0];
            } else {
                ptd[0][(angflag)?1:0]+=txtbox[1][0];
            }
        }
        if(RTNORM!=cmd_dimaddline(layername,plinkvar[DIMCLRD].dval.ival,ptd[0],ptd[3],flp,dragmode)) goto exit;
    }
    ic_ptcpy(ptt[0],ptd[0]);
    if ((plinkvar[DIMGAP].dval.rval*plinkvar[DIMSCALE].dval.rval)<0.0) { if (!cmd_drawbox(plinkvar[DIMCLRD].dval.ival,xpt,flp,dragmode)) goto exit; }
    strcpy(dimlsttext,text);
	// End create block.
    if (!dragmode && plinkvar[DIMASO].dval.ival) {
		if (RTERROR==dwg_addendblk(flp)) goto exit;
		handitem=flp->ret_lastblockdef();
		handitem->get_2(plink->bname,sizeof(plink->bname)-1);
	}
    return(TRUE);
    exit:
        rb.restype    =RTSHORT;
        rb.resval.rint=OL_EMKANON;
        SDS_setvar("ERRNO"/*DNT*/,-1,&rb,flp,NULL,NULL,0);
	    return(FALSE);

}
#endif
//BugZilla No. 78197 ; 01-07-2002 ]

short cmd_dimordinate(db_drawing *flp) {
    if (cmd_iswindowopen() == RTERROR) return RTERROR;
	char            fs1[IC_ACADBUF] = ""/*DNT*/,
                    fs2[IC_ACADBUF] = ""/*DNT*/,
                    dimtxt[IC_ACADBUF] = ""/*DNT*/,
                   *sptr1 = NULL;
    struct resbuf  *newelist=NULL,
                    rb,
                    rbucs,
                    rbwcs,
                    rbecs;
    sds_name        ename = {0L, 0L};
    sds_real        txtang=0.0,
                    rSelflag=0.0,
                    fr1 = 0.0;
    sds_point       pt1 = {0.0, 0.0, 0.0},
                    pt2 = {0.0, 0.0, 0.0},
                    pt3 = {0.0, 0.0, 0.0},
                    pt4 = {0.0, 0.0, 0.0};
    short           xdatflg=0,
                    ydatflg=0,
                    dimtype=6;
    int             ret = 0;
    long            rc=0L;
    struct gr_view *view=SDS_CURGRVW;
	db_handitem    *elp=NULL;

    cmd_initialrbcs(&rbecs,&rbwcs,&rbucs,flp);
    ic_ptcpy(plink->pt210,rbecs.resval.rpoint);
    cmd_initialize();
	// Bugzilla No. 78106; 15/04/2002
    //cmd_setparlink(flp);
    *dimtxt=0;
    plink->DefValidflg=FALSE;
	plink->HomeText = TRUE;
    // Getting the point in (UCS).
    ret=sds_getpoint(NULL,ResourceString(IDC_DIMENSIONS__NSELECT_PO_140, "\nSelect point for ordinate dimension: " ),pt1);
    if (ret==RTNONE || ret==RTERROR || ret==RTCAN) goto exit;
    for (;;) {
        if (xdatflg || ydatflg) {
            // Getting the point in (UCS).
            ret=sds_getpoint(NULL,ResourceString(IDC_DIMENSIONS__NENDPOINT__141, "\nEndpoint of leader: " ),pt2);

			if (ret==RTERROR || ret==RTCAN)
				goto exit;

            rSelflag=1.0;
        } else {
            if (sds_initget(SDS_RSG_NOVIEWCHG,ResourceString(IDC_DIMENSIONS_INITGET_XDA_142, "Xdatum Ydatum Text ~Mtext ~_Xdatum ~_Ydatum ~_Text ~_Mtext" ))!=RTNORM)
				goto exit;
       		{
                elp=new db_dimension(flp);
                elp->set_1(dimtxt);
                elp->set_11(dimzeropt); // This doesn't matter at this point.
                elp->set_13(pt1);           // point 13 is in (UCS).
                elp->set_14(pt2);           // point 14 is in (UCS).
                elp->set_52(rSelflag);
                elp->set_53(txtang);
                elp->set_70(dimtype);

                db_ucshelper(view->ucsorg,view->ucsaxis,view->elevation,pt3,pt4,&fr1);
                elp->set_210(plink->pt210);
                elp->set_51(fr1);
                elp->set_12(pt4);

			    ename[0]=(long)elp;
			    ename[1]=(long)flp;
			    SDS_getvar(NULL,DB_QDRAGMODE,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
                ret=SDS_dragobject(-1,rb.resval.rint,(sds_real *)ename,(double *)elp->retp_14(),0.0,ResourceString(IDC_DIMENSIONS__NTEXT_X_DAT_68, "\nText/X datum/Y datum/<Leader endpoint>: " ),NULL,pt2,NULL);
				if (ret==RTERROR || ret==RTCAN)
				{
					delete elp;
					elp = NULL;
					goto exit;
				}

                cmd_makedimension(elp,flp,-1);
                if (plinkvar[DIMASO].dval.ival) { delete elp; elp=NULL; }
			}
        }
        if (ret==RTNONE) {
            goto exit;
        } else if (ret==RTKWORD) {
            if (sds_getinput(fs1)!=RTNORM) goto exit;
            strupr(fs1);
            if (strsame("TEXT"/*DNT*/ ,fs1)) {
                //*** ~| TODO AutoCAD calls the MTEXT command here.  Eventually we need to do the same.
				//EBATECH(CNBR) Bugzilla#78443 Use DIMZIN from current SYSVAR buf.
				cmd_rtos_dim(plink->dimvalue,plinkvar[DIMUNIT].dval.ival,plinkvar[DIMDEC].dval.ival,dimtxt, plinkvar[DIMZIN].dval.ival, plinkvar[DIMDSEP].dval.ival);
				//cmd_rtos_dim(plink->dimvalue,plinkvar[DIMUNIT].dval.ival,plinkvar[DIMDEC].dval.ival,dimtxt, DIMTYPEMAIN);
                sprintf(fs2,ResourceString(IDC_DIMENSIONS__NDIMENSION__52, "\nDimension text <%s>: " ),dimtxt);
                if ((ret=sds_getstring(1,fs2,fs1))==RTERROR) {
                    goto exit;
                } else if(ret==RTCAN) {
                    goto exit;
                }
                if (*fs1) strncpy(dimtxt,fs1,sizeof(dimtxt)-1);
            } else if (strsame("XDATUM"/*DNT*/ ,fs1)) {
                xdatflg=1;
                plink->flags=70;
                plink->DefValidflg=TRUE;
                continue;
            } else if (strsame("YDATUM"/*DNT*/ ,fs1)) {
                ydatflg=1;
                plink->flags=6;
                plink->DefValidflg=TRUE;
                continue;
            } else if (strsame("MTEXT"/*DNT*/ ,fs1)) {
                cmd_ErrorPrompt(CMD_ERR_OPTIONNOTSUP,0);
                    continue;
            } else {
                cmd_ErrorPrompt(CMD_ERR_PT,0);
                continue;
            }
            continue;
        }
        if (cmd_dimmode && *dimtxt==0) {
			//EBATECH(CNBR) Bugzilla#78443 Use DIMZIN from current SYSVAR buf.
			cmd_rtos_dim(fabs(plink->dimvalue),plinkvar[DIMUNIT].dval.ival,plinkvar[DIMDEC].dval.ival,dimtxt,plinkvar[DIMZIN].dval.ival, plinkvar[DIMDSEP].dval.ival);
			//cmd_rtos_dim(fabs(plink->dimvalue),plinkvar[DIMUNIT].dval.ival,plinkvar[DIMDEC].dval.ival,dimtxt, DIMTYPEMAIN);
            sprintf(fs2,ResourceString(IDC_DIMENSIONS__NDIMENSION__52, "\nDimension text <%s>: " ),dimtxt);
            if ((ret=sds_getstring(1,fs2,fs1))==RTERROR) {
                goto exit;
            } else if(ret==RTCAN) {
                goto exit;
            }
            //Bugzilla No. 78105; 01-07-2002 [
			//if (*fs1) {                
			    strncpy(dimtxt,fs1,sizeof(dimtxt)-1);
                if ((sptr1=strchr(fs1,'<'/*DNT*/))!=NULL) {
                    if (*(sptr1+1) && *(sptr1+1)=='>'/*DNT*/) {
                        *fs2=0;
                        *sptr1=0;
                        if (*fs1)       strcpy(fs2,fs1);
                        if (*dimtxt)    strcat(fs2,dimtxt);
                        if (*(sptr1+2)) strcat(fs2,(sptr1+2));
                        strcpy(fs1,fs2);
                   }
                }
                strncpy(dimtxt,fs1,sizeof(dimtxt)-1);
            //}
			//Bugzilla No. 78105; 01-07-2002 ]
        }
        break;
    }

    ic_ptcpy(plink->pt14,pt2);
    // Copy data to the last points picked structure.
	cmd_savelast(plink,view);
    // Do the transformaion from (UCS) to (NATIVE) format this is becouse when a dimension is created via entmake the points need to be in
    // native format for compatability when someone does an entmake from the command line.
    sds_trans(plink->textmidpt,&rbucs,&rbecs,0,plink->textmidpt);
    sds_trans(plink->pt13,&rbucs,&rbwcs,0,plink->pt13);
    sds_trans(plink->pt14,&rbucs,&rbwcs,0,plink->pt14);
    // Create dimension as a block.
    if (plinkvar[DIMASO].dval.ival) {
        db_ucshelper(view->ucsorg,view->ucsaxis,view->elevation,pt1,pt2,&fr1);
        if ((newelist=sds_buildlist(RTDXF0,"DIMENSION"/*DNT*/,
                                        1 ,dimtxt, //plink->text,
                                        11,plink->textmidpt,
                                        12,pt2,
                                        13,plink->pt13,
                                        14,plink->pt14,
                                        70,plink->flags,
                                        53,txtang,
                                        51,fr1,
                                        210,rbecs.resval.rpoint,
                                        0))==NULL) { cmd_ErrorPrompt(CMD_ERR_MOREMEM,0); goto exit; }
        cmd_buildEEdList(newelist);
        if (sds_entmake(newelist)!=RTNORM) goto exit;
        if (sds_entlast(lastdimpts.ename)!=RTNORM) goto exit;
    } else {
        elp->set_1(plink->text);
        elp->set_11(plink->textmidpt);
        elp->set_13(plink->pt13);
        elp->set_14(plink->pt14);
        cmd_makedimension(elp,flp,0);
    }

    exit:
		plink->HomeText = FALSE;
        IC_RELRB(newelist);
		// Bugzilla No. 78106; 15/04/2002
		//cmd_CleanUpDimVarStrings();		// Clean up first, then return ret.

        return(RTERROR);
}
