/* DRAG.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * Supports drag commands 
 *
 */

// THIS FILE HAS BEEN GLOBALIZED!

//** Includes
#include "cmds.h"/*DNT*/
#include "Icad.h" /*DNT*/
#include "IcadApi.h"
// **********
// GLOBALS
//
BOOL cmd_InsideRotateCommand;
BOOL cmd_InsideScaleCommand;
BOOL cmd_InsideMoveCopyCommand;
BOOL cmd_InsideMirrorCommand;
struct cmd_drag_globalpacket cmd_drag_gpak;
sds_matrix init_matrix  = { 1.0, 0.0, 0.0, 0.0,
							0.0, 1.0, 0.0, 0.0,
							0.0, 0.0, 1.0, 0.0,
							0.0, 0.0, 0.0, 1.0 };

extern bool    SDS_DontBitBlt;
extern int SDS_SkipLastOrthoSet;


int MatrixCompose(sds_matrix mt,sds_matrix mtOut)
{
	int i,j,k;
	double sum;
	sds_matrix mtTemp;

	memcpy(mtTemp, mtOut, sizeof(sds_matrix));

	for(i=0;i<=3;i++)
	{
		for(j=0;j<=3;j++)
		{
			sum = 0.0;
			for(k=0;k<=3;k++)
				sum += mt[i][k]*mtTemp[k][j];
			mtOut[i][j] = sum;
		}
	}
	return 0;
}

void MatrixRotate(sds_real sintheta,sds_real costheta, int axis, sds_matrix mt)
{
    int axp1, axp2;
 
    axp1 = (axis + 1) % 3;
    axp2 = (axis + 2) % 3;
    memcpy(mt, init_matrix, sizeof(sds_matrix));
    mt[axp1][axp1] = mt[axp2][axp2] = costheta;
    mt[axp1][axp2] = -(mt[axp2][axp1] = sintheta);
}

int MirrorAxis(sds_point pt1,sds_point pt2, sds_matrix mt)
{
	double sin_val,cos_val;
	sds_matrix mtTemp;
	sds_real theta;
	int i;

	theta = sds_angle(pt1,pt2);
	sin_val = sin(-theta);
	cos_val = cos(-theta);

	//Initialize
    memcpy(mt, init_matrix, sizeof(sds_matrix));
	
	if (icadRealEqual(pt1[1],0.0) && icadRealEqual(pt2[1],0.0) && 
		icadRealEqual(pt1[2],0.0) && icadRealEqual(pt2[2],0.0)) {//(xz plane)
		mt[1][1]=-1.0;
		return false;
	}
	else if (icadRealEqual(pt1[0],0.0) && icadRealEqual(pt2[0],0.0) && 
		icadRealEqual(pt1[2],0.0) && icadRealEqual(pt2[2],0.0)) {//(yz plane)
		mt[0][0]=-1.0;
		return false;
	}
	else if (icadRealEqual(pt1[1],0.0) && icadRealEqual(pt2[1],0.0) && 
		icadRealEqual(pt1[0],0.0) && icadRealEqual(pt2[0],0.0)) {
		mt[2][2]=-1.0;
		return false;
	}


    for (i = 0; i <= 2; i++)
        mt[i][3] = -(pt1[i]);
	MatrixRotate(sin_val,cos_val, 2, mtTemp);
	MatrixCompose(mtTemp,mt);

	memcpy(mtTemp, init_matrix, sizeof(sds_matrix));
	mtTemp[1][1]=-1.0;
	MatrixCompose(mtTemp,mt);

	MatrixRotate(-sin_val,cos_val, 2, mtTemp);
	MatrixCompose(mtTemp,mt);

	memcpy(mtTemp, init_matrix, sizeof(sds_matrix));
    for (i = 0; i <= 2; i++)
        mtTemp[i][3] = pt1[i];
	MatrixCompose(mtTemp,mt);

	return 0;
}



// *******************
// HELPER FUNCTION
//

static int 
cmd_scale_drag(sds_point dragpt, sds_matrix mt) {
//*** Callback function for draggen() in the cmd_drag() function.
    struct cmd_drag_globalpacket *gpak;
    struct resbuf setgetrb;
    sds_point tmppt;
    sds_real len;
	int snapstyl;

    gpak=&cmd_drag_gpak;
	SDS_DontBitBlt=TRUE;
    if(gpak->rband) sds_grdraw(gpak->pt1,gpak->lastpt,-1,0);
	SDS_DontBitBlt=FALSE;
	if(mt==NULL)
		return(RTNORM);
    if(SDS_getvar(NULL,DB_QORTHOMODE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
		return(RTERROR);
    if((!setgetrb.resval.rint && SDS_CMainWindow->m_fIsShiftDown) || (setgetrb.resval.rint && !SDS_CMainWindow->m_fIsShiftDown)) {
        if(SDS_getvar(NULL,DB_QSNAPSTYL,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
			return(RTERROR);
		if(setgetrb.resval.rint==0)
			snapstyl=-1;
		else{
			if(SDS_getvar(NULL,DB_QSNAPISOPAIR,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
				return(RTERROR);
			snapstyl=setgetrb.resval.rint;
		}
        if(SDS_getvar(NULL,DB_QSNAPANG,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
			return(RTERROR);
        ic_ptcpy(tmppt,dragpt);
        if(!SDS_SkipLastOrthoSet && ic_ortho(gpak->pt1,setgetrb.resval.rreal,tmppt,dragpt,snapstyl)!=0)
			return(RTERROR);
    }
    len=sds_distance(gpak->pt1,dragpt);
    len/=gpak->refval;
    mt[0][0]=len; mt[0][1]=0.0; mt[0][2]=0.0; mt[0][3]=(-(len-1)*gpak->pt1[0]);
    mt[1][0]=0.0; mt[1][1]=len; mt[1][2]=0.0; mt[1][3]=(-(len-1)*gpak->pt1[1]);
    mt[2][0]=0.0; mt[2][1]=0.0; mt[2][2]=len; mt[2][3]=(-(len-1)*gpak->pt1[2]);
    mt[3][0]=0.0; mt[3][1]=0.0; mt[3][2]=0.0; mt[3][3]=1.0;


	SDS_DontBitBlt=TRUE;

    if(gpak->rband) sds_grdraw(gpak->pt1,dragpt,-1,0);
    ic_ptcpy(gpak->lastpt,dragpt);
	SDS_DontBitBlt=FALSE;

    return(RTNORM);
}

/*D.G.*/// "static" removed for usin in SDS_StartDraggingSS
int
cmd_translate_drag(sds_point dragpt, sds_matrix mt) {
//*** Callback function for draggen() in the cmd_drag() function.
    struct cmd_drag_globalpacket *gpak;
    struct resbuf setgetrb;
    sds_point tmppt;
    short fi1;
	int snapstyl;

    gpak=&cmd_drag_gpak;
	SDS_DontBitBlt=TRUE;
    if(gpak->rband) sds_grdraw(gpak->pt1,gpak->lastpt,-1,0);
	SDS_DontBitBlt=FALSE;

	if(mt==NULL)
		return(RTNORM);

    if(SDS_getvar(NULL,DB_QORTHOMODE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
		return(RTERROR);
    if((!setgetrb.resval.rint && SDS_CMainWindow->m_fIsShiftDown) || (setgetrb.resval.rint && !SDS_CMainWindow->m_fIsShiftDown)) {
        if(SDS_getvar(NULL,DB_QSNAPSTYL,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
			return(RTERROR);
		if(setgetrb.resval.rint==0)
			snapstyl=-1;
		else{
			if(SDS_getvar(NULL,DB_QSNAPISOPAIR,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
				return(RTERROR);
			snapstyl=setgetrb.resval.rint;
		}
        if(SDS_getvar(NULL,DB_QSNAPANG,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
			return(RTERROR);
        ic_ptcpy(tmppt,dragpt);
        if(!SDS_SkipLastOrthoSet && ic_ortho(gpak->pt1,setgetrb.resval.rreal,tmppt,dragpt,snapstyl)!=0)
			return(RTERROR);
		if(SDS_CMainWindow->m_pDragVarsCur) ic_ptcpy(SDS_CMainWindow->m_pDragVarsCur->LastDrag,dragpt);
    }
    for(fi1=0; fi1<=2; fi1++) {
        mt[fi1][3]=dragpt[fi1]-(gpak->pt1[fi1]);
    }

	SDS_DontBitBlt=TRUE;
    if(gpak->rband) sds_grdraw(gpak->pt1,dragpt,-1,0);
	SDS_DontBitBlt=FALSE;

    ic_ptcpy(gpak->lastpt,dragpt);

    return(RTNORM);
}

static int 
cmd_mirror_drag(sds_point dragpt, sds_matrix mt) {
//*** Callback function for draggen() in the cmd_drag() function.
    struct cmd_drag_globalpacket *gpak;
    struct resbuf setgetrb;
    sds_point tmppt;
	int snapstyl;

    gpak=&cmd_drag_gpak;

	SDS_DontBitBlt=TRUE;
    if(gpak->rband) sds_grdraw(gpak->pt1,gpak->lastpt,-1,0);
	SDS_DontBitBlt=FALSE;


	if(mt==NULL) return(RTNORM);
    if(SDS_getvar(NULL,DB_QORTHOMODE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) return(RTERROR);
    if((!setgetrb.resval.rint && SDS_CMainWindow->m_fIsShiftDown) || (setgetrb.resval.rint && !SDS_CMainWindow->m_fIsShiftDown)) {
        if(SDS_getvar(NULL,DB_QSNAPSTYL,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) return(RTERROR);
		if(setgetrb.resval.rint==0)
			snapstyl=-1;
		else{
			if(SDS_getvar(NULL,DB_QSNAPISOPAIR,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) return(RTERROR);
			snapstyl=setgetrb.resval.rint;
		}
        if(SDS_getvar(NULL,DB_QSNAPANG,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) return(RTERROR);
        ic_ptcpy(tmppt,dragpt);
        if(!SDS_SkipLastOrthoSet && ic_ortho(gpak->pt1,setgetrb.resval.rreal,tmppt,dragpt,snapstyl)!=0) return(RTERROR);
    }
 
	MirrorAxis(gpak->pt1,dragpt,mt);		

	SDS_DontBitBlt=TRUE;
    if(gpak->rband) sds_grdraw(gpak->pt1,dragpt,-1,0);
	SDS_DontBitBlt=FALSE;

    ic_ptcpy(gpak->lastpt,dragpt);

    return(RTNORM);
}

static int 
cmd_rotate_drag(sds_point dragpt, sds_matrix mt) {

//*** Callback function for draggen() in the cmd_drag() function.
    struct cmd_drag_globalpacket *gpak;
    struct resbuf setgetrb;
    sds_point tmppt;
    sds_real theta;
	int snapstyl;

    gpak=&cmd_drag_gpak;

	SDS_DontBitBlt=TRUE;
    if(gpak->rband) sds_grdraw(gpak->pt1,gpak->lastpt,-1,0);
	SDS_DontBitBlt=FALSE;


	if(mt==NULL)
		return(RTNORM);
    if(SDS_getvar(NULL,DB_QORTHOMODE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) return(RTERROR);
    if((!setgetrb.resval.rint && SDS_CMainWindow->m_fIsShiftDown) || (setgetrb.resval.rint && !SDS_CMainWindow->m_fIsShiftDown)) {
        if(SDS_getvar(NULL,DB_QSNAPSTYL,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) return(RTERROR);
		if(setgetrb.resval.rint==0)
			snapstyl=-1;
		else{
			if(SDS_getvar(NULL,DB_QSNAPISOPAIR,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) return(RTERROR);
			snapstyl=setgetrb.resval.rint;
		}
        if(SDS_getvar(NULL,DB_QSNAPANG,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) return(RTERROR);
        ic_ptcpy(tmppt,dragpt);
        if(!SDS_SkipLastOrthoSet && ic_ortho(gpak->pt1,setgetrb.resval.rreal,tmppt,dragpt,snapstyl)!=0) return(RTERROR);
    }
    theta=sds_angle(gpak->pt1,dragpt);
    if(gpak->reference) theta-=gpak->refval;
    mt[0][0]=cos(theta); mt[0][1]=(-sin(theta)); mt[0][2]=0.0; mt[0][3]=((gpak->pt1[1]*sin(theta))+(gpak->pt1[0]*(1-cos(theta))));
    mt[1][0]=sin(theta); mt[1][1]=cos(theta);    mt[1][2]=0.0; mt[1][3]=(-(gpak->pt1[0]*sin(theta))+(gpak->pt1[1]*(1-cos(theta))));
    mt[2][0]=0.0;        mt[2][1]=0.0;           mt[2][2]=1.0; mt[2][3]=0.0;
    mt[3][0]=0.0;        mt[3][1]=0.0;           mt[3][2]=0.0; mt[3][3]=1.0;


	SDS_DontBitBlt=TRUE;
    if(gpak->rband) sds_grdraw(gpak->pt1,dragpt,-1,0);
	SDS_DontBitBlt=FALSE;

    ic_ptcpy(gpak->lastpt,dragpt);

    return(RTNORM);
}


int cmd_drag(short mode, sds_name ss1, LPCTSTR prompt, char *initkwl,
    sds_point respt, char *inputstr) {
//*** This function is meant to be called from any function wanting to do a draggen()
//*** that adjusts the scale, translation, or rotation of a selection set.  For examples
//*** of how this function is used, see cmd_change().
//***
//***      mode - Use the #defines:  CMD_SCALE_DRAG, CMD_TRANSLATE_DRAG, CMD_ROTATE_DRAG.
//***       ss1 - Selection set for dragging.
//***    prompt - Prompt string to be displayed.
//***   initkwl - Keyword list that gets passed to initget().
//***     respt - Resulting point picked.
//***  inputstr - Pointer to a buffer used to store the string when this
//***             function returns RTSTR.
//***
//*** NOTE: The calling function needs to set up the cmd_drag_globalpacket
//*** structure before calling this function.
//***
//*** RETURNS:   RTNORM - Success.
//***           RTERROR - An error occured.
//***             RTCAN - User canceled the function.
//***            RTNONE - Null input.
//***             RTSTR - An unrecognized string was entered. "inputstr" gets set and
//***                     the calling function should process what to do with it.
//***
    char fs1[IC_ACADBUF],dragstr[IC_ACADBUF];
    struct cmd_drag_globalpacket *gpak;
    struct resbuf setgetrb;
    sds_point pt;
    int ret,snapstyl;
    short dragmode;
    char *defaultOption = "~Drag ~_Drag"/*DNT*/;

    gpak=&cmd_drag_gpak;
    //*** Check system variable DRAGMODE to see if dragging is enabled.
    if((ret=SDS_getvar(NULL,DB_QDRAGMODE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))!=RTNORM) return(ret);
    dragmode=setgetrb.resval.rint;
    if(initkwl!=NULL && *initkwl != 0) 
        catTwoResourceStrings(defaultOption, initkwl, fs1);
//        strcat(fs1,initkwl);
    else
        strcpy(fs1,defaultOption);

    if(prompt!=NULL) strcpy(dragstr,prompt);
    for(;;) {
        ic_ptcpy(gpak->lastpt,gpak->pt1);

		// We need to copy the base point into pt and pass it along to sds_draggen()
		// This is used to undraw the rubberband line when the user enters a transparent
		// command or switches windows.
        ic_ptcpy(pt,gpak->pt1);

		//Modified Cybage AW 03-12-01 [
		//Reason : The dragmode OFF or ON affect the commands like rotate/Scale
		//Reason : Global variables "bNotAPoint" and "strAngleScale" are not used anymore.
		//if(dragmode>=2) {
		if(dragmode<=3) {	//Modified Cybage AW 03-12-01 ]
            switch(mode) {
                case CMD_SCALE_DRAG:
					// Don't let the prompt menu come up if only DRAG is an option.
					if((ret=sds_initget(SDS_RSG_NODOCCHG|SDS_RSG_OTHER|SDS_RSG_NOLIM|(strsame(fs1,defaultOption) ? SDS_RSG_NOPMENU : 0),fs1))!=RTNORM) 
                        return(ret);
					cmd_InsideScaleCommand=TRUE;
					ret=sds_draggen(ss1,dragstr,0,cmd_scale_drag,pt);
					cmd_InsideScaleCommand=FALSE;
					break;
                case CMD_ROTATE_DRAG:
					// Don't let the prompt menu come up if only DRAG is an option.
					if((ret=sds_initget(SDS_RSG_NODOCCHG|SDS_RSG_OTHER|SDS_RSG_NOLIM|(strsame(fs1,defaultOption) ? SDS_RSG_NOPMENU : 0),fs1))!=RTNORM) 
                        return(ret);
					cmd_InsideRotateCommand=TRUE;
					ret=sds_draggen(ss1,dragstr,0,cmd_rotate_drag,pt);
					cmd_InsideRotateCommand=FALSE;
					break;
                case CMD_TRANSLATE_DRAG:
					// Don't let the prompt menu come up if only DRAG is an option.
					if((ret=sds_initget(SDS_RSG_OTHER|SDS_RSG_NOLIM|(strsame(fs1,defaultOption) ? SDS_RSG_NOPMENU : 0),fs1))!=RTNORM) 
                        return(ret);
					cmd_InsideMoveCopyCommand=TRUE;
					ret=sds_draggen(ss1,dragstr,0,cmd_translate_drag,pt);
					cmd_InsideMoveCopyCommand=FALSE;
					break;
				case CMD_MIRROR_DRAG:
					if((ret=sds_initget(SDS_RSG_NODOCCHG|SDS_RSG_OTHER|SDS_RSG_NOLIM|(strsame(fs1,defaultOption) ? SDS_RSG_NOPMENU : 0),fs1))!=RTNORM) 
                        return(ret);
					cmd_InsideMirrorCommand=TRUE;
					ret=sds_draggen(ss1,dragstr,0,cmd_mirror_drag,pt);
					cmd_InsideMirrorCommand=FALSE;
					break;
            }
        } else {
            ret=internalGetpoint(gpak->pt1,dragstr,pt);
        }
        if(ret==RTNORM) {
//          if(dragmode>=2 && gpak->rband) {
//				SDS_DontBitBlt=TRUE;
//				ExecuteUIAction( ICAD_WNDACTION_ENDDRAG );
//				sds_grdraw(gpak->pt1,gpak->lastpt,-1,0);
//				SDS_DontBitBlt=FALSE;
//				SDS_BitBlt2Scr(0);
//			}
			if(SDS_getvar(NULL,DB_QORTHOMODE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) return(RTERROR);
			if((setgetrb.resval.rint && !SDS_CMainWindow->m_fIsShiftDown) || (!setgetrb.resval.rint && SDS_CMainWindow->m_fIsShiftDown)) {
				if(SDS_getvar(NULL,DB_QSNAPSTYL,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) return(RTERROR);
				if(setgetrb.resval.rint==0)
					snapstyl=-1;
				else{
					if(SDS_getvar(NULL,DB_QSNAPISOPAIR,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) return(RTERROR);
					snapstyl=setgetrb.resval.rint;
				}
				if(SDS_getvar(NULL,DB_QSNAPANG,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) return(RTERROR);
				if(!SDS_SkipLastOrthoSet && ic_ortho(gpak->pt1,setgetrb.resval.rreal,pt,pt,snapstyl)!=0) return(RTERROR);
			}
            ic_ptcpy(respt,pt);
            return(RTNORM);
        } else if(ret==RTSTR || ret==RTKWORD) {
            if((ret=sds_getinput(fs1))!=RTNORM) return(ret);
//          if(dragmode>=2 && gpak->rband) {
//				SDS_DontBitBlt=TRUE;
//				sds_grdraw(gpak->pt1,gpak->lastpt,-1,0);
//				SDS_DontBitBlt=FALSE;
//				SDS_BitBlt2Scr(0);
//			}
            if(strisame("DRAG",fs1) || strisame(defaultOption,fs1)) {
                if(dragmode==1) dragmode=3;
                *dragstr=0;
                continue;
            }
            if(inputstr!=NULL) strcpy(inputstr,fs1);
            return(RTSTR);
        } else {
//          if(dragmode>=2 && gpak->rband) {
//				SDS_DontBitBlt=TRUE;
//				sds_grdraw(gpak->pt1,gpak->lastpt,-1,0);
//				SDS_DontBitBlt=FALSE;
//				SDS_BitBlt2Scr(0);
//			}
            return(ret);
        }
    }
    return(RTERROR);
}


