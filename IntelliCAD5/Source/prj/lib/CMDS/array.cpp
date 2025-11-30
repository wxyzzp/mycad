/* F:\DEV\PRJ\LIB\CMDS\ARRAY.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * ARRAY command
 * 3DARRAY command
 * 
 */ 

// THIS FILE HAS BEEN GLOBALIZED!
#include "cmds.h"/*DNT*/
#include "Icad.h"/*DNT*/
#include "db.h"/*DNT*/
#include "IcadApi.h"/*DNT*/
#include <float.h>
#include "Modeler.h"
#include "gvector.h"
using icl::gvector;
using icl::point;
#include "transf.h"
using icl::transf;
#include "CmdQueryTools.h"//ucs2wcs2ucs()

// SystemMetrix(Hiro)-[FIX: the copied annotation is associated with invalid leader.
#include "fixup1005.h"
// ]-SystemMetrix(Hiro) FIX: the copied annotation is associated with invalid leader.

#define CMD_ENT_NATIVEWCS	147.4747

// Proto
short cmd_ask4ColsAndRows
(
int* colcount, 
int* rowcount, 
sds_real* coldistance, 
sds_real* rowdistance
);

static
short cmd_ask4ColsAndRows3D
(
int* colcount,
int* rowcount,
int* levelcount,
sds_real* coldistance,
sds_real* rowdistance,
sds_real* leveldistance
);

void cmd_xformpt
(
sds_point from, 
sds_point to, 
sds_matrix mat
); /*D.Gavrilov*/// It was added for using by cmd_array_func.

// Globals
char cmd_array_defchar[]="Rectangular";   //*** Default array option - (R)ectangular or (P)olar
short cmd_array_rotflg;                   //*** Last rotation for a polar array: 0 - not rotated, 1 - rotated
//Modified Cybage SBD 17/12/2001 DD/MM/YYYY 
//Reason : Fix for Bug No. 77866 and 77867 from Bugzilla( array case ) 
extern BOOL bdimUpdate;



// helpers -- get cols and rows
static
short cmd_ask4ColsAndRows3D
(
int* colcount,
int* rowcount,
int* levelcount,
sds_real* coldistance,
sds_real* rowdistance,
sds_real* leveldistance
)
{
	//*** This function prompts the user for column count, row count, levelcount, column spacing, row 
	//*** spacing and level spacing and sets colct, rowct, levelct, coldist, rowdist and leveldist.
	//***
	//*** RETURNS:  RTNORM  - Success.
	//***           RTCAN   - User cancel.
	//***           RTERROR -or a value less than -1 indicating the (source code) line number of an error.
	//***
    
    sds_real rowdist,coldist,leveldist;
    RT ret;
	int rowct,colct,levelct;
    short ask4col=1;
	
    rowct=colct=levelct=1; rowdist=coldist=leveldist=0.0;
    //*** Get the number of rows.
    do
	{
        if(sds_initget(SDS_RSG_NODOCCHG | RSG_NOZERO | RSG_NONEG,NULL)!=RTNORM) 
		{ 
			ret=RTERROR; 
			goto bail; 
		}
		
        if((ret=sds_getint(ResourceString(IDC_INSERT__NNUMBER_OF_ROWS_47, "\nNumber of rows in the array <1>: " ),&rowct))==RTERROR) 
		{
            goto bail;
		}
		else if(ret==RTCAN)
		{
            goto bail;
		}
		
		//*** Get the number of columns.
		if(sds_initget(SDS_RSG_NODOCCHG | RSG_NOZERO | RSG_NONEG,NULL)!=RTNORM) 
		{ 
			ret=RTERROR; 
			goto bail; 
		}
		if((ret=sds_getint(ResourceString(IDC_INSERT__NNUMBER_OF_COLU_48, "\nNumber of columns <1>: " ),&colct))==RTERROR) 
		{
			goto bail;
		}
		else if(ret==RTCAN)
		{
			goto bail;
		}
		
		//*** Get the number of levels.
		if(sds_initget(SDS_RSG_NODOCCHG | RSG_NOZERO | RSG_NONEG,NULL)!=RTNORM) 
		{ 
			ret=RTERROR; 
			goto bail; 
		}
		if((ret=sds_getint(ResourceString(IDC_INSERT__NNUMBER_OF_LEVE_49, "\nNumber of levels <1>: " ),&levelct))==RTERROR) 
		{
			goto bail;
		}
		else if(ret==RTCAN)
		{
			goto bail;
		}
		if((rowct+colct+levelct)>3) 
			break;
		cmd_ErrorPrompt(CMD_ERR_NOTHINGTODO,0);
		
	}while(1);
		
	//*** Get the distance between rows.
	if(rowct!=1)
	{
		do
		{
			if(sds_initget(SDS_RSG_NODOCCHG | RSG_NOZERO,NULL)!=RTNORM) 
			{ 
				ret=RTERROR; 
				goto bail; 
			}
			if((ret=sds_getdist(NULL,ResourceString(IDC_INSERT__NVERTICAL_DISTA_50, "\nVertical distance between rows: " ),&rowdist))==RTERROR) 
			{
				goto bail;
			}
			else if(ret==RTCAN)
			{
				goto bail;
			}
			else if(ret==RTNORM)
			{
				if(icadRealEqual(rowdist,0.0)) 
				{
					cmd_ErrorPrompt(CMD_ERR_NONZERO,0);
				}
				else
				{
					break;
				}
				
			}
			else if(ret==RTNONE)
			{
				cmd_ErrorPrompt(CMD_ERR_DIST2PTS,0);
			}
		}while(1);
	}
	
	//*** Get the distance between columns.
    if(colct!=1)
	{    
        do
		{
            if(sds_initget(SDS_RSG_NODOCCHG | RSG_NOZERO,NULL)!=RTNORM) 
			{ 
				ret=RTERROR; 
				goto bail; 
			}
            if((ret=sds_getdist(NULL,ResourceString(IDC_INSERT__NHORIZONTAL_DIS_51, "\nHorizontal distance between columns: " ),&coldist))==RTERROR) 
			{
                goto bail;
			}
			else if(ret==RTCAN)
			{
                goto bail;
			}
			else if(ret==RTNORM)
			{
				if(icadRealEqual(coldist,0.0)) 
				{
					cmd_ErrorPrompt(CMD_ERR_NONZERO,0);
				}
				else
				{
					break;
				}
			}
			else if(ret==RTNONE)
			{
				cmd_ErrorPrompt(CMD_ERR_DIST2PTS,0);
			}
		}while(1);
	}

    //*** Get the distance between levels.
    if(levelct!=1)
	{    
        do
		{
            if(sds_initget(SDS_RSG_NODOCCHG | RSG_NOZERO,NULL)!=RTNORM) 
			{ 
				ret=RTERROR; 
				goto bail; 
			}
            if((ret=sds_getdist(NULL,ResourceString(IDC_INSERT__NDEPTH_BETWEEN__52, "\nDepth between levels: " ),&leveldist))==RTERROR) 
			{
                goto bail;
			}
			else if(ret==RTCAN)
			{
                goto bail;
			}
			else if(ret==RTNORM)
			{
				if(icadRealEqual(leveldist,0.0))
				{
					cmd_ErrorPrompt(CMD_ERR_NONZERO,0);
				}
				else
				{
					break;
				}
			}
			else if(ret==RTNONE)
			{
				cmd_ErrorPrompt(CMD_ERR_DIST2PTS,0);
			}
		}while(1);
	}

    *colcount=colct; *rowcount=rowct; *levelcount=levelct;
    *coldistance=coldist; *rowdistance=rowdist;	*leveldistance=leveldist;
	
bail:
	
    return(ret);
}

short cmd_ask4ColsAndRows
(
int* colcount, 
int* rowcount, 
sds_real* coldistance, 
sds_real* rowdistance
)
{
	//*** This function prompts the user for column count, row count, column spacing, and row 
	//*** spacing and sets colct, rowct, coldist, and rowdist.
	//***
	//*** RETURNS:  0 - Success.
	//***          -1 - User cancel.
	//***          or a value less than -1 indicating the (source code) line number of an error.
	//***
    char fs1[IC_ACADBUF];
    sds_point pt1,pt2;
    sds_real rowdist,coldist;
    int rowct,colct;
    RT ret,ask4col=1;
	struct resbuf setgetrb;
	
	
    rowct=colct=1; rowdist=coldist=0.0;
    //*** Get the number of rows.
    if((ret=sds_initget(SDS_RSG_NODOCCHG | RSG_NOZERO | RSG_NONEG,NULL))!=RTNORM) 
		goto bail;
    if((ret=sds_getint(ResourceString(IDC_INSERT__NNUMBER_OF_ROWS_47, "\nNumber of rows in the array <1>: " ),&rowct))==RTERROR) 
	{
        goto bail;
	} 
	else if(ret==RTCAN) 
	{
        goto bail;
	}
    
	//*** Get the number of columns.
    if((ret=sds_initget(SDS_RSG_NODOCCHG | RSG_NOZERO | RSG_NONEG,NULL))!=RTNORM) 
		goto bail; 
    if((ret=sds_getint(ResourceString(IDC_INSERT__NNUMBER_OF_COLU_48, "\nNumber of columns <1>: " ),&colct))==RTERROR) 
	{
        goto bail;
	} 
	else if(ret==RTCAN) 
	{
        goto bail;
	}
	
    if(rowct!=1 || colct!=1) 
	{
        if(rowct>1) 
		{
            //*** Get the distance between rows.
            for(;;) 
			{
                if((ret=sds_initget(SDS_RSG_NODOCCHG | RSG_OTHER,NULL))!=RTNORM) 
					goto bail; 
                if((ret=sds_getpoint(NULL,ResourceString(IDC_INSERT__NVERTICAL_DISTA_53, "\nVertical distance between rows, or spacing rectangle: " ),pt1))==RTERROR) 
				{
                    goto bail;
				} 
				else if(ret==RTCAN) 
				{
                    goto bail;
				} 
				else if(ret==RTNORM) 
				{
					SDS_getvar(NULL,DB_QDRAGMODE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
					if((ret=SDS_dragobject(21,setgetrb.resval.rint,pt1,NULL,0.0,ResourceString(IDC_INSERT__NOPPOSITE_CORNE_54, "\nOpposite corner: " ),NULL,pt2,NULL))==RTERROR) 
					{ 
                        goto bail;
					} 
					else if(ret==RTCAN) 
					{
                        goto bail;
					}
                    coldist=pt2[0]-pt1[0];
                    rowdist=pt2[1]-pt1[1];
                    ask4col=0;
                    break;
				} 
				else if(ret==RTKWORD) 
				{
                    if((ret=sds_getinput(fs1))!=RTNORM) 
						goto bail; 
                    if(sds_distof(fs1,-1,&rowdist)!=RTNORM)
					{
						cmd_ErrorPrompt(CMD_ERR_DIST2PTS,0);
                        continue;
					}
                    break;
				} 
				else 
				{
                    cmd_ErrorPrompt(CMD_ERR_DIST2PTS,0);
                    continue;
				}
			}
		}
		
        if(ask4col && colct>1) 
		{
            //*** Get the distance between columns.
            if((ret=sds_initget(SDS_RSG_NODOCCHG | RSG_NONULL,NULL))!=RTNORM) 
				goto bail; 
            if((ret=sds_getdist(NULL,ResourceString(IDC_INSERT__NHORIZONTAL_DIS_51, "\nHorizontal distance between columns: " ),&coldist))==RTERROR) 
			{
                goto bail;
			} 
			else if(ret==RTCAN) 
			{
                goto bail;
			}
		}
	}
	
    *colcount=colct; *rowcount=rowct;
    *coldistance=coldist; *rowdistance=rowdist;
	
    ret=RTNORM;
bail:

    return(ret);
}

short cmd_3darray(void) 
{
    if (cmd_iswindowopen() == RTERROR) 
		return RTERROR;	
	
	if (cmd_isinpspace() == RTERROR) 
		return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    return(cmd_array_func(1,NULL,NULL,NULL,NULL));
}

short cmd_array(void) 
{
    if (cmd_iswindowopen() == RTERROR) 
		return RTERROR;
	
	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    return(cmd_array_func(IC_ARRAYMODE_2D,NULL,NULL,NULL,NULL));
}

// Modified Cybage PP 16/03/2001 [
// Reason: Fix for bug # 67790 
BOOL bAskPointForArray = FALSE ;
// Modified Cybage PP 16/03/2001 ]

short cmd_array_func
(
int mode3d,
sds_name rotateset,
sds_point ucsbase,
sds_point ucsvect,
sds_real rotang,
bool bInteractive, 
struct rect_array_info* rinfo, 
struct polar_array_info* pinfo,
sds_name setofnewents
)
{
	//*** This function creates multiple copies of entities in a pattern.
	//***	mode3d==IC_ARRAYMODE_2D 2D array (no other parameters req'd)
	//***	mode3d==IC_ARRAYMODE_3D 3D array (no other parameters req'd)
	//***	mode3d==IC_ARRAYMODE_3DROT 3D rotate using all parameters - Rotate3D does prompting
	//***		if mode3d==IC_ARRAYMODE_3DROTATE, caller must free selection set
	/*D.Gavrilov*/// IC_ARRAYMODE_3DROT not used now really.
				  // It was used by cmd_rotate3d before.

	//*** RETURNS: RTNORM  - Success.
	//***          RTCAN   - User cancel.
	//***          RTERROR - Error returned from an ads function.
	//***
	
    if (cmd_iswindowopen() == RTERROR) 
		return RTERROR;			
    
	sds_point cmd_zeropt={0.0,0.0,0.0};
	
    char fs1[IC_ACADBUF],fs2[IC_ACADBUF];
    struct resbuf *tmprb=NULL,setgetrb,*rbname=NULL,rbent,rbent2,rbucs,rbwcs,rb3d;// *rb50=NULL,*rb51=NULL;
    struct cmd_elistll *ellbeg,*ellcur,*elltmp;//,*elltmp2;
    sds_point coldist,rowdist,basept,pt2,tmppt,tmppt2,pl_stpt,*ptarray=NULL;
    sds_name ss1, ename, newSS;
    sds_real leveldist,fillang,angbtwn,angsin,angcos,rtmp;
    int fi1,fi2,fi3,rowct,colct,levelct,numitems=0,curprogress,nextprogress;
    RT ret,cwflag;
    long fl1,fl2;
    bool baseflg=false;
	double	TmpInteger;
	bool bCloneBeginNotification = false;
	
	ss1[0] = ss1[1] = newSS[0] = newSS[1] = 0;	// for safety

	// Having pt2 uninitialized was bad
	pt2[0] = 0.0;
	pt2[1] = 0.0;
	pt2[2] = 0.0;
	
	//Bugzilla No. 77993 ; date : 18-03-2002 [
	int SavedSDS_AtNodeDrag;
	SavedSDS_AtNodeDrag = SDS_AtNodeDrag;
	//Bugzilla No. 77993 ; date : 18-03-2002 ]

	if (setofnewents)
		sds_ssadd (NULL, NULL, setofnewents);

	//rbucs is for xforming into/out of current ucs
	rbucs.restype=rbwcs.restype=RTSHORT;
	rbucs.resval.rint=1;
	rbwcs.resval.rint=0;
	
	SDS_getvar(NULL,DB_QANGDIR,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
    cwflag=setgetrb.resval.rint;
	
    ic_ptcpy(basept,cmd_zeropt);
    elltmp=ellbeg=ellcur=NULL; 
	tmprb=NULL;
	
	if(IC_ARRAYMODE_3DROT==mode3d)
	{
		if(NULL==rotateset || NULL==ucsbase || NULL==ucsvect)
			return(RTERROR);
		
		ic_encpy(ss1,rotateset);
		
//		sds_sslength(ss1,&fl1);
		if((ret=sds_sslength(ss1,&fl1))!=RTNORM) 
			goto bail;
		
		if(fl1==0L) 
		{
			ret=RTERROR;
			goto bail;
		}
		//don't set cmd_array_defchar, because we'll overwrite default for ROTATE command
	}
	else
	{
		//*** Get selection set of entities.
		if (bInteractive)
		{
			if((ret=sds_pmtssget(ResourceString(IDC_ARRAY__NSELECT_ENTITIES__1, "\nSelect entities to array: " ),NULL,NULL,NULL,NULL,ss1,0))!=RTNORM) 
				return(ret);
		}
		else
		{
			ic_encpy(ss1,rotateset);
		}
		
		if((ret=sds_sslength(ss1,&fl1))!=RTNORM) 
			goto bail;
		
		if(fl1==0L) 
		{
			ret=RTERROR;
			goto bail;
		}
		
		if (RemoveHatchFromSS(ss1) == RTNIL) 
		{
			ret = RTNIL;
			goto bail;
		}
		
		//*** Rectangular or Polar array
		if (bInteractive)
		{
			if(strisame(cmd_array_defchar, "RECTANGULAR"/*DNT*/))
				sprintf(fs2,ResourceString(IDC_ARRAY__NTYPE_OF_ARRAY____2, "\nType of array:  Polar/<Rectangular>: " ));
			else
				sprintf(fs2,ResourceString(IDC_ARRAY__NTYPE_OF_ARRAY____3, "\nType of array:  Rectangular/<Polar>: " ));
			
			if((ret=sds_initget(SDS_RSG_NODOCCHG,ResourceString(IDC_ARRAY_INITGET_POLAR_RECT_4, "Polar Rectangular Center ~_Polar ~_Rectangular ~_Center" )))!=RTNORM) 
				goto bail;
			
			if((ret=sds_getkword(fs2,fs1))==RTERROR) 
			{
				goto bail;
			} 
			else if(ret==RTCAN) 
			{
				goto bail;	
			} 
			else if(ret==RTNORM) 
			{
				strupr(fs1);
				_tcscpy(cmd_array_defchar, fs1);		
			} 
			else if(ret==RTNONE)
			{
				if(strisame(cmd_array_defchar, "CENTER"/*DNT*/))
					_tcscpy(cmd_array_defchar, "POLAR"/*DNT*/);
				
			}
		}
		else
		{
			if (rinfo)
				_tcscpy(cmd_array_defchar, "RECTANGULAR"/*DNT*/);
			else if (pinfo)
				_tcscpy(cmd_array_defchar, "POLAR"/*DNT*/);
			else
				goto bail;
		}
	}
	
	// Send notification to external sds-apps
	SDS_CallUserCallbackFunc(SDS_CBBEGINCLONE, (void *)ss1, NULL, NULL);
	bCloneBeginNotification = true;

	if (mode3d!=IC_ARRAYMODE_3DROT && (strisame(cmd_array_defchar, "RECTANGULAR"/*DNT*/))) 
	{
		//*** Allocate the linked list of entity lists (this list now includes complex entity info also).
		if(cmd_ss2elistll(&ellbeg,&ellcur,ss1,210)!=0) 
		{
			ret=RTERROR;
			goto bail;
		}
		
		if (bInteractive)
		{
			//*** Rectangular array
			if(mode3d==IC_ARRAYMODE_3D)
			{
				if((ret=cmd_ask4ColsAndRows3D(&colct,&rowct,&levelct,&coldist[2],&rowdist[2],&leveldist))!=RTNORM) 
					goto bail;
			}
			else
			{
				if((ret=cmd_ask4ColsAndRows(&colct,&rowct,&coldist[2],&rowdist[2]))!=RTNORM) 
					goto bail;
				levelct=1;
				leveldist=0.0;	
			}
		}
		else
		{
			if (!rinfo)
				goto bail;

			colct = rinfo->nColumns;
			rowct = rinfo->nRows;
			levelct = rinfo->nLevels;
			coldist[2] = rinfo->ColDist;
			rowdist[2] = rinfo->RowDist;
			leveldist  = rinfo->LevDist;
		}
		
		SDS_getvar(NULL,DB_QSNAPANG,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		coldist[0]=cos(setgetrb.resval.rreal)*coldist[2];
		coldist[1]=sin(setgetrb.resval.rreal)*coldist[2];
		rowdist[0]=-sin(setgetrb.resval.rreal)*rowdist[2];
		rowdist[1]=cos(setgetrb.resval.rreal)*rowdist[2];
		
		if(colct*rowct*levelct*fl1>500)
		{
			curprogress=0;
			SDS_ProgressStart();
		}
		else
		{
			curprogress=-1;
		}
		
		for(fi1=0; fi1<rowct; fi1++) 
		{
			for(fi2=0; fi2<colct; fi2++) 
			{
				for(fi3=0; fi3<levelct; fi3++) 
				{
					if(sds_usrbrk()) 
					{
						ret=RTCAN; 
						goto bail;
					}
					
					if(curprogress>=0)
					{
						nextprogress=((fi1*fi2*fi3)+((fi1+1)*colct+fi2))*100/(colct*rowct*levelct);
						if(nextprogress-curprogress>=5)
						{
							curprogress=nextprogress;
							SDS_ProgressPercent(curprogress);
						}
					}
					
					//*** Skip making the selected entities again
					if(fi1==0 && fi2==0 && fi3==0) 
						continue;
					
                    // SystemMetrix(Hiro)-[FIX: the copied annotation is associated with invalid leader.
                    ReactorFixups reactorFixup;
                    // ]-SystemMetrix(Hiro) FIX: the copied annotation is associated with invalid leader.

					for(elltmp=ellbeg;elltmp!=NULL;elltmp=elltmp->next) 
					{
                        //<ALM>: store sat data before transformation
                        char* pSatData = 0;
                        //</ALM>

						// prepare resbufs list for new entity
						for(tmprb=elltmp->entlst;tmprb!=NULL;tmprb=tmprb->rbnext) 
						{
							if(tmprb->restype==0)
							{
								rbname=tmprb;
								if(!strsame(rbname->resval.rstring,"VERTEX"/*DNT*/) && !strsame(rbname->resval.rstring,"SEQEND"/*DNT*/))
								{
									if(elltmp->grpptr!=NULL && RTNORM!=sds_wcmatch(rbname->resval.rstring,"LINE,POINT,RAY,XLINE,MTEXT,ELLIPSE,SPLINE,TOLERANCE,IMAGE"/*DNT*/))
									{
										rbent.restype=RT3DPOINT;
										ic_ptcpy(rbent.resval.rpoint,elltmp->grpptr->resval.rpoint);
									}
									else
									{
										rbent.restype=RTSHORT;
										rbent.resval.rint=0;//it's wcs
									}
								}
							}
							else if((tmprb->restype>=10 && tmprb->restype<=18) || tmprb->restype == 1011)	/*D.G.*/// 1011 added.
							{
								//NOTE: any changes to this area need to be duplicated below
								if(tmprb->restype==12 && strsame(rbname->resval.rstring,"DIMENSION"/*DNT*/))
									continue;
								else if(tmprb->restype==11 &&		//if this is a vector parameter...
									((strsame(rbname->resval.rstring,"RAY"/*DNT*/)) ||
									(strsame(rbname->resval.rstring,"XLINE"/*DNT*/)) ||
									(strsame(rbname->resval.rstring,"MTEXT"/*DNT*/)) ||
									(strsame(rbname->resval.rstring,"IMAGE"/*DNT*/)) ||
									(strsame(rbname->resval.rstring,"TOLERANCE"/*DNT*/)) ||
									(strsame(rbname->resval.rstring,"ELLIPSE"/*DNT*/))))
								{
									continue;
								}
								else if(tmprb->restype==12 && strsame(rbname->resval.rstring,"IMAGE"/*DNT*/))
									continue;
								else if((tmprb->restype==12 || tmprb->restype==13) && (strsame(rbname->resval.rstring,"SPLINE"/*DNT*/) || strsame(rbname->resval.rstring,"MLINE"/*DNT*/)))
									continue;									
								
								tmppt[0]=(coldist[0]*fi2)+(rowdist[0]*fi1);
								tmppt[1]=(rowdist[1]*fi1)+(coldist[1]*fi2);
								tmppt[2]=(leveldist*fi3);

								if((strsame(rbname->resval.rstring,"DIMENSION"/*DNT*/) && tmprb->restype!=11 && tmprb->restype!=16) ||
									tmprb->restype == 1011)
									//dims have some wcs point
									sds_trans(tmppt,&rbucs,&rbwcs,1,tmppt2);
								else
									sds_trans(tmppt,&rbucs,&rbent,1,tmppt2);

								tmprb->resval.rpoint[0]+=tmppt2[0];
								tmprb->resval.rpoint[1]+=tmppt2[1];
								tmprb->resval.rpoint[2]+=tmppt2[2];
								
							}
							else if (tmprb->restype==38 && strsame(rbname->resval.rstring,"LWPOLYLINE"/*DNT*/))
							{
								tmprb->resval.rreal+=leveldist*fi3;
							}
                            //<ALM>
                            else if ((tmprb->restype == 1) && 
                                     (RTNORM==sds_wcmatch(rbname->resval.rstring,"3DSOLID,BODY,REGION"/*DNT*/)))
                            {
                                // make transformation matrix;
#if 0
								tmppt[0]=(coldist[0]*fi2)+(rowdist[0]*fi1);
								tmppt[1]=(rowdist[1]*fi1)+(coldist[1]*fi2);
								tmppt[2]=(leveldist*fi3);
								sds_trans(tmppt,&rbucs,&rbent,1,tmppt2);

			                    sds_matrix mx = {{ 1.0,       0.0,       0.0,      0.0},
									             { 0.0,       1.0,       0.0,      0.0},
									             { 0.0,       0.0,       1.0,      0.0},
									             { tmppt2[0], tmppt2[1], tmppt2[2], 1.0}};
#else
                                gvector shift(coldist[0]*fi2 + rowdist[0]*fi1,
                                              coldist[1]*fi2 + rowdist[1]*fi1,
                                              leveldist*fi3);
                                shift *= ucs2wcs();
                                transf move;
                                move.translate(shift);
#endif
								CDbAcisEntityData newObj;
								CDbAcisEntityData oldObj;
								oldObj.setSat(tmprb->resval.rstring);
								oldObj.setDbtype(rbname->resval.rstring);

                                if (CModeler::get()->transform(oldObj, newObj, move))
                                {
									pSatData = tmprb->resval.rstring;
                                    tmprb->resval.rstring = new char [newObj.getSat().size()+1];
                                    strcpy(tmprb->resval.rstring, newObj.getSat().c_str());
                                }
                            }
                            //</ALM>
						}	// for(  thru resbufs list
						
						struct sds_resbuf *tmpBuf = NULL;
						for(tmpBuf = elltmp->entlst; tmpBuf != NULL, tmpBuf->restype != 0; tmpBuf = tmpBuf->rbnext) ;
						
						ASSERT(tmpBuf != NULL);
						
						if (strsame(tmpBuf->resval.rstring, "IMAGE"/*DNT*/))
						{
							if(SDS_entmakeImageCopy(elltmp->entlst) != RTNORM)  
							{
								goto bail;
							}
						}
						else
						{
							//*** Make the new entity
							//Modified Cybage SBD 17/12/2001 DD/MM/YYYY 
							//Reason : Fix for Bug No. 77866 and 77867 from Bugzilla( array case ) 
							bdimUpdate=TRUE;							
							
							//Bugzilla No. 77993 ; date : 18-03-2002 [
							SDS_AtNodeDrag = 1;				
							if((ret=sds_entmake(elltmp->entlst))!=RTNORM) 
							{
								goto bail;
							}
							SDS_AtNodeDrag = SavedSDS_AtNodeDrag;
							//Bugzilla No. 77993 ; date : 18-03-2002 ]
							
							//Modified Cybage SBD 17/12/2001 DD/MM/YYYY 
							//Reason : Fix for Bug No. 77866 and 77867 from Bugzilla( array case ) 
							bdimUpdate=FALSE;

							if (setofnewents)
							{
								sds_name tmpename;
								sds_entlast(tmpename);
								if (tmpename[0] != ellbeg->ename[0])
									sds_ssadd (tmpename, setofnewents, setofnewents);
							}
						}
							
						// add new entity to the newSS for SDS_CBENDCLONE notification
						sds_name tmpEname;
						sds_entlast( tmpEname );
						sds_ssadd( tmpEname, newSS, newSS );

                        // SystemMetrix(Hiro)-[FIX: the copied annotation is associated with invalid leader.
                        db_handitem *oldEntity = (db_handitem*)elltmp->ename[0];
                        db_handitem *newEntity = (db_handitem*)tmpEname[0];
                        reactorFixup.AddHanditem(newEntity);
                        reactorFixup.AddHandleMap(oldEntity->RetHandle(), newEntity->RetHandle());
                        // ]-SystemMetrix(Hiro) FIX: the copied annotation is associated with invalid leader.
						
						//*** Set the point coordinates back to original
						for(tmprb=elltmp->entlst; tmprb!=NULL; tmprb=tmprb->rbnext) 
						{
							//NOTE: any changes to this area need to be duplicated above
							if(tmprb->restype==12 && strsame(rbname->resval.rstring,"DIMENSION"/*DNT*/))
								continue;
							else if((tmprb->restype>=10 && tmprb->restype<=18) || tmprb->restype == 1011)
							{
								if(tmprb->restype==11 &&		//if this is a vector parameter...
									((strsame(rbname->resval.rstring,"RAY"/*DNT*/)) ||
									(strsame(rbname->resval.rstring,"XLINE"/*DNT*/)) ||
									(strsame(rbname->resval.rstring,"MTEXT"/*DNT*/)) ||
									(strsame(rbname->resval.rstring,"IMAGE"/*DNT*/)) ||
									(strsame(rbname->resval.rstring,"TOLERANCE"/*DNT*/)) ||
									(strsame(rbname->resval.rstring,"ELLIPSE"/*DNT*/))))
								{
									continue;
								}
								else if(tmprb->restype==12 && strsame(rbname->resval.rstring,"IMAGE"/*DNT*/))
									continue;
								else if((tmprb->restype==12 || tmprb->restype==13) && (strsame(rbname->resval.rstring,"SPLINE"/*DNT*/) || strsame(rbname->resval.rstring,"MLINE"/*DNT*/)))
									continue;
								
								tmprb->resval.rpoint[0]-=tmppt2[0];
								tmprb->resval.rpoint[1]-=tmppt2[1];
								tmprb->resval.rpoint[2]-=tmppt2[2];
							}
							else if (tmprb->restype==38 && strsame(rbname->resval.rstring,"LWPOLYLINE"/*DNT*/))
							{
								tmprb->resval.rreal-=leveldist*fi3;
							}
                            //<ALM>
                            else if ((tmprb->restype == 1) && 
                                     (RTNORM==sds_wcmatch(rbname->resval.rstring,"3DSOLID,BODY,REGION"/*DNT*/)))
                            {
                                IC_FREE(tmprb->resval.rstring);
                                tmprb->resval.rstring = pSatData;
								pSatData = NULL;
                            }
                            //</ALM>

						}	// for(  thru resbufs list

					}	// for(  thru entities

                    // SystemMetrix(Hiro)-[FIX: the copied annotation is associated with invalid leader.
                    // fix the reactors on the copied entities
                    reactorFixup.ApplyFixups(SDS_CURDWG);
                    // ]-SystemMetrix(Hiro) FIX: the copied annotation is associated with invalid leader.

				}	// for(  thru levels
			}	// for(  thru columns
		}	// for (  thru rows

	}	// Rectangular case
	else 
	{	//if cmd_array_defchar=='P' OR if mode3d==IC_ARRAYMODE_3DROT
		//*** Polar array

		if (bInteractive)
		{
			if(IC_ARRAYMODE_2D==mode3d)
			{
				if((ret=sds_initget(SDS_RSG_NODOCCHG | RSG_NONULL,ResourceString(IDC_ARRAY_INITGET_BASE_6, "Base ~_Base" )))!=RTNORM) 
					goto bail;
				
				if((ret=sds_getpoint(NULL,ResourceString(IDC_ARRAY__NBASE_CENTER_OF_P_7, "\nBase/Center of polar array: " ),pt2))==RTERROR) 
				{
					goto bail;
				}
				else if(ret==RTCAN)
				{
					goto bail;
				}
				else if(ret==RTNORM)
				{
					//*** Set LASTPOINT system variable.
					setgetrb.restype=RT3DPOINT;
					ic_ptcpy(setgetrb.resval.rpoint,pt2);
					SDS_setvar(NULL,DB_QLASTPOINT,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
				}
				else if(ret==RTKWORD)
				{
					baseflg=true;
					if((ret=sds_initget(SDS_RSG_NODOCCHG | RSG_NONULL,NULL))!=RTNORM) 
						goto bail;
					
					if((ret=sds_getpoint(NULL,ResourceString(IDC_ARRAY__NBASE_OF_POLAR_AR_8, "\nBase of polar array entities: " ),basept))!=RTNORM) 
						goto bail;
					
					//get the center point of the array
					if((ret=sds_initget(SDS_RSG_NODOCCHG | RSG_NONULL,NULL))!=RTNORM) 
						goto bail;
					
					if((ret=sds_getpoint(NULL,ResourceString(IDC_ARRAY__NCENTER_POINT_OF__9, "\nCenter point of array: " ),pt2))!=RTNORM) 
						goto bail;
				}
			}
		}
		else
		{
			if (!pinfo)
				goto bail;

			ic_ptcpy (basept, pinfo->BasePoint);
			if (pinfo->PointOnAxis)
			{
				ic_ptcpy (pt2, pinfo->PointOnAxis);
				ic_ptcpy (tmppt2, pinfo->PointOnAxis);
			}
		}
		
		if(IC_ARRAYMODE_3DROT==mode3d)
		{
			//rotate3d
			ic_ptcpy(pt2,ucsbase);
			fillang=angbtwn=rotang;
			numitems=2;//always count original as one copy
			rb3d.restype=RT3DPOINT;
			ic_ptcpy(rb3d.resval.rpoint,ucsvect);
			tmppt[0]=sqrt(ucsvect[0]*ucsvect[0]+ucsvect[1]*ucsvect[1]+ucsvect[2]*ucsvect[2]);
			
			if(tmppt[0]<CMD_FUZZ)
			{
				ret=RTERROR;
				goto bail;
			}

			rb3d.resval.rpoint[0]/=tmppt[0];
			rb3d.resval.rpoint[1]/=tmppt[0];
			rb3d.resval.rpoint[2]/=tmppt[0];
			//don't set cmd_array_rotflag - it's a global & we don't wanna mess w/it.
		}
		else
		{
			if(strisame(cmd_array_defchar, "CENTER"/*DNT*/)) 
			{
				if(IC_ARRAYMODE_2D==mode3d)
				{
					if (bInteractive)
					{
						//get angle between
						if((ret=sds_initget(SDS_RSG_NODOCCHG | RSG_NONULL | RSG_NOZERO,NULL))!=RTNORM) 
							goto bail;
						
						/*D.G.*/// Use SDS_getorientneg for enabling inputting negative values instead of sds_getangle.
						strncpy(fs1,
								(cwflag) ? ResourceString(IDC_ARRAY__NANGLE_BETWEEN_I_10, "\nAngle between items (- for ccw, + for cw): " ) :
										   ResourceString(IDC_ARRAY__NANGLE_BETWEEN_I_11, "\nAngle between items (+ for ccw, - for cw): " ),
								sizeof(fs1) - 1);
						
						if((ret = SDS_getorientneg(pt2, fs1, &angbtwn)) == RTERROR) 
						
							goto bail;
						else
							if(ret == RTCAN) 
								goto bail;
						
//						if((ret=sds_getangle(pt2,(cwflag)?ResourceString(IDC_ARRAY__NANGLE_BETWEEN_I_10, "\nAngle between items (- for ccw, + for cw): " ):ResourceString(IDC_ARRAY__NANGLE_BETWEEN_I_11, "\nAngle between items (+ for ccw, - for cw): " ),&angbtwn))!=RTNORM) 
//							goto bail;
						
						//get the number of items
						if((ret=sds_initget(SDS_RSG_NODOCCHG,NULL))!=RTNORM) 
							goto bail;
						
						if((ret=sds_getint(ResourceString(IDC_ARRAY__NNUMBER_OF_ITEMS_12, "\nNumber of items or -(deg to fill): " ),&numitems))!=RTNORM) 
							goto bail;
					}
					else
					{
						if (!pinfo)
							goto bail;

						numitems = pinfo->nItems;
						angbtwn = pinfo->AngleBetweenItems;
					}
					
					//calc the number of items and round the result
					if(numitems<0) 
					{
						fillang=-numitems;
						if(fillang>360)
						{
							fillang=IC_TWOPI;
						}
						else
						{
							fillang*=IC_PI/180.0;
						}
						
						/*D.G.*/// Calculate number of items by this way (old code commented out below).
						// If you enter -30 for angle between items and 300 for filling angle then
						// old way sets numitems = 9 (before comparing with IC_TWOPI)
						// with compiling optimization switched on, because fillang / angbtwn == -9.99999999999...
						rtmp = fabs(fillang / angbtwn);
						if(modf(rtmp, &TmpInteger) < IC_ZRO || TmpInteger + 1.0 - rtmp >= IC_ZRO)
							numitems = (int)TmpInteger;
						else
							numitems = (int)(TmpInteger + 1.0);

//						numitems = abs((int)(fillang / angbtwn));

						if(IC_TWOPI - fillang > IC_ZRO)
							numitems++;
					}
					
					//get the rotate flag
					if (bInteractive)
					{
						for (;;) 
						{
							sds_initget(SDS_RSG_NODOCCHG | RSG_OTHER,ResourceString(IDC_ARRAY_INITGET_YES_ROTAT_13, "Yes-Rotate_entities|Yes No-Do_not_rotate|No ~_Yes ~_No" ));
							if((ret=sds_getkword(ResourceString(IDC_ARRAY__NROTATE_ENTITIES_14, "\nRotate entities around the array?  No/<Yes>: " ),fs1))==RTERROR) 
							{
								goto bail;
							} 
							else if(ret==RTCAN) 
							{
								goto bail;
							} 
							else if(ret==RTNONE) 
							{
								cmd_array_rotflg=1; break;
								break;			// Default, for AutoCAD compatibility is "Yes", no matter what happened last time.
							} 
							else if(ret==RTNORM) 
							{
								if (strisame(fs1,"YES"/*DNT*/)) 
								{
									cmd_array_rotflg=1; 
									break;
								} 
								else if (strisame(fs1,"NO"/*DNT*/)) 
								{
									cmd_array_rotflg=0; 
									break;
								} 
								else 
								{
									cmd_ErrorPrompt(CMD_ERR_YESNO,0);  // "Yes or No, please"
									continue;
								}
							} 
						}
					}
					else
					{
						if (!pinfo)
							goto bail;
						cmd_array_rotflg = pinfo->RotateItems;
					}
				}
				else
				{
					//get the number of items
					if (bInteractive)
					{
						if((ret=sds_initget(SDS_RSG_NODOCCHG | RSG_NONULL | RSG_NOZERO | RSG_NONEG,NULL))!=RTNORM) 
							goto bail;
						
						if((ret=sds_getint(ResourceString(IDC_ARRAY__NNUMBER_OF_ITEMS_17, "\nNumber of items: " ),&numitems))!=RTNORM) 
							goto bail;
					}
					else
					{
						if (!pinfo)
							goto bail;
						numitems = pinfo->nItems;
					}
					
					//get the fill angle
					if (bInteractive)
					{
						while(1)
						{
							SDS_getvar(NULL,DB_QAUNITS,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
							//NOTE:  Can't use angtos because 2PI is converted to 0.
							if(setgetrb.resval.rint==2)
								strncpy(fs1,(cwflag)?ResourceString(IDC_ARRAY__NANGLE_TO_FILL___18, "\nAngle to fill (- for ccw, + for cw) <400>: " ):ResourceString(IDC_ARRAY__NANGLE_TO_FILL___19, "\nAngle to fill (+ for ccw, - for cw) <400>: " ),sizeof(fs1)-1);
							else if(setgetrb.resval.rint==3)
								strncpy(fs1,(cwflag)?ResourceString(IDC_ARRAY__NANGLE_TO_FILL___20, "\nAngle to fill (- for ccw, + for cw) <6.2832>: " ):ResourceString(IDC_ARRAY__NANGLE_TO_FILL___21, "\nAngle to fill (+ for ccw, - for cw) <6.2832>: " ),sizeof(fs1)-1);
							else
								strncpy(fs1,(cwflag)?ResourceString(IDC_ARRAY__NANGLE_TO_FILL___22, "\nAngle to fill (- for ccw, + for cw) <360>: " ):ResourceString(IDC_ARRAY__NANGLE_TO_FILL___23, "\nAngle to fill (+ for ccw, - for cw) <360>: " ),sizeof(fs1)-1);
							
							if((ret=sds_initget(SDS_RSG_NODOCCHG | RSG_NOZERO,NULL))!=RTNORM) 
								goto bail;
							//progesoft
							//if((ret=SDS_getorientbig(NULL,fs1,&fillang))==RTERROR)
							//if((ret=sds_getangle(NULL,fs1,&fillang))==RTERROR)
							//BugZilla No. 78518 12-05-2003								
							if((ret=SDS_getorientbig(NULL,fs1,&fillang))==RTERROR)
							//fine progesoft
							{
								goto bail;					
							} 
							else if(ret==RTCAN) 
							{
								goto bail;
							} 
							else if(ret==RTNONE) 
							{
								fillang=IC_TWOPI;
							}
							break;
						}
					}
					else
					{
						if (!pinfo)
							goto bail;
						fillang = pinfo->FillAngle;
					}
					
					if(fillang<0.0)
					{
						if(fillang<-IC_TWOPI) 
							fillang=-IC_TWOPI;
					}
					else
					{
						if(fillang>IC_TWOPI) 
							fillang=IC_TWOPI;
					}
					
					if(icadAngleEqual(fabs(fillang),IC_TWOPI))
					{
						angbtwn=fillang/numitems;
					}
					else
					{
						angbtwn=fillang/(numitems-1);
					}
					
					if(cwflag)
					{
						angbtwn=-angbtwn;
						//fillang=-fillang;   NOTE: Fillang not used from here on, just angbtwn
					}
					
					//get the rotate flag
					if (bInteractive)
					{
						while(1)
						{
							sds_initget(SDS_RSG_NODOCCHG | RSG_OTHER,ResourceString(IDC_ARRAY_INITGET_YES_ROTAT_13, "Yes-Rotate_entities|Yes No-Do_not_rotate|No ~_Yes ~_No" ));
							if((ret=sds_getkword(ResourceString(IDC_ARRAY__NROTATE_ENTITIES_14, "\nRotate entities around the array?  No/<Yes>: " ),fs1))==RTERROR) 
							{
								goto bail;
							} 
							else if(ret==RTCAN) 
							{
								goto bail;
							} 
							else if(ret==RTNONE) 
							{
								cmd_array_rotflg=1; break;
								break;			// Default, for AutoCAD compatibility is "Yes", no matter what happened last time.
								
							} 
							else if(ret==RTNORM) 
							{
								if (strisame(fs1,"YES"/*DNT*/)) 
								{
									cmd_array_rotflg=1; 
									break;
								} 
								else if (strisame(fs1,"NO"/*DNT*/)) 
								{
									cmd_array_rotflg=0; break;
								} 
								else 
								{
									cmd_ErrorPrompt(CMD_ERR_YESNO,0);  // "Yes or No, please"
									continue;
								}
							} 
						}
					}
					else
					{
						if (!pinfo)
							goto bail;
						cmd_array_rotflg = pinfo->RotateItems;
					}
					
					//get the center point of the array
					if (bInteractive)
					{
						if((ret=sds_initget(SDS_RSG_NODOCCHG | RSG_NONULL,NULL))!=RTNORM) 
							goto bail;
						
						if((ret=sds_getpoint(NULL,ResourceString(IDC_ARRAY__NCENTER_POINT_OF__9, "\nCenter point of array: " ),pt2))!=RTNORM) 
							goto bail;
						
						//*** Set LASTPOINT system variable.
						setgetrb.restype=RT3DPOINT;
						ic_ptcpy(setgetrb.resval.rpoint,pt2);
						SDS_setvar(NULL,DB_QLASTPOINT,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
						
						//Get second point
						if((ret=sds_initget(SDS_RSG_NODOCCHG | RSG_NONULL,NULL))!=RTNORM) 
							goto bail;
						
						if((ret=sds_getpoint(pt2,ResourceString(IDC_ARRAY__NSPECIFY_SECOND__24, "\nSpecify second point along central axis of array: " ),tmppt2))!=RTNORM) 
							goto bail;
						
						//*** Set LASTPOINT system variable.
						setgetrb.restype=RT3DPOINT;
						ic_ptcpy(setgetrb.resval.rpoint,tmppt2);
						SDS_setvar(NULL,DB_QLASTPOINT,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
					}
					else
					{
						if (!pinfo)
							goto bail;

						ic_ptcpy (pt2, pinfo->BasePoint);
						if (pinfo->PointOnAxis)
							ic_ptcpy (tmppt2, pinfo->PointOnAxis);
					}
				}
				
				if(mode3d==IC_ARRAYMODE_3D)
				{
					rb3d.restype=RT3DPOINT;
					rb3d.resval.rpoint[0]=tmppt2[0]-pt2[0];
					rb3d.resval.rpoint[1]=tmppt2[1]-pt2[1];
					rb3d.resval.rpoint[2]=tmppt2[2]-pt2[2];
					tmppt[0]=sqrt(rb3d.resval.rpoint[0]*rb3d.resval.rpoint[0]+
						rb3d.resval.rpoint[1]*rb3d.resval.rpoint[1]+
						rb3d.resval.rpoint[2]*rb3d.resval.rpoint[2]);
					if(tmppt[0] < IC_ZRO)	/*D.Gavrilov*/// ACAD breaks execution
					{					// of the command in this case, but we'll
											// continue with the default value
						rb3d.resval.rpoint[0] = rb3d.resval.rpoint[1] = 0.0;
						rb3d.resval.rpoint[2] = 1.0;
					}
					else
					{
						rb3d.resval.rpoint[0]/=tmppt[0];
						rb3d.resval.rpoint[1]/=tmppt[0];
						rb3d.resval.rpoint[2]/=tmppt[0];
					}
				}
				else
				{
					//just rotating in current ucs xy plane, so rb3d=current ucs (its z)
					rb3d.restype=RTSHORT;
					rb3d.resval.rint=1;
				}				
			}
			else
			{
				//*** Number of items in array
				if (bInteractive)
				{
					if((ret=sds_initget(SDS_RSG_NODOCCHG | RSG_NONEG,NULL))!=RTNORM) 
						goto bail;
					
					if((ret=sds_getint(ResourceString(IDC_ARRAY__NENTER_TO_SPECIF_25, "\nENTER to specify angle between items/<Number of items to array>: " ),&numitems))==RTERROR) 
					{
						goto bail;
					} 
					else if(ret==RTCAN) 
					{
						goto bail;
					}
				}
				else
				{
					if (!pinfo)
						goto bail;

					numitems = pinfo->nItems;
				}

				//*** Angle to fill
				if (bInteractive)
				{
					for(;;) 
					{
						SDS_getvar(NULL,DB_QAUNITS,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
						//NOTE:  Can't use angtos because 2PI is converted to 0.
						
						if(setgetrb.resval.rint==2)
							strncpy(fs1,(cwflag)?ResourceString(IDC_ARRAY__NANGLE_TO_ARRAY__26, "\nAngle to array (- for ccw, + for cw) <400>: " ):ResourceString(IDC_ARRAY__NANGLE_TO_ARRAY__27, "\nAngle to array (+ for ccw, - for cw) <400>: " ),sizeof(fs1)-1);
						else if(setgetrb.resval.rint==3)
							strncpy(fs1,(cwflag)?ResourceString(IDC_ARRAY__NANGLE_TO_ARRAY__28, "\nAngle to array (- for ccw, + for cw) <6.2832>: " ):ResourceString(IDC_ARRAY__NANGLE_TO_ARRAY__29, "\nAngle to array (+ for ccw, - for cw) <6.2832>: " ),sizeof(fs1)-1);
						else
							strncpy(fs1,(cwflag)?ResourceString(IDC_ARRAY__NANGLE_TO_ARRAY__30, "\nAngle to array (- for ccw, + for cw) <360>: " ):ResourceString(IDC_ARRAY__NANGLE_TO_ARRAY__31, "\nAngle to array (+ for ccw, - for cw) <360>: " ),sizeof(fs1)-1);
						
						if((ret=sds_initget(SDS_RSG_NODOCCHG,NULL))!=RTNORM) 
							goto bail;

//						if((ret=SDS_getanglebig(pt2,fs1,&fillang))==RTERROR) 
						//if((ret=sdsengine_getorient(pt2,fs1,&fillang))==RTERROR) 						
						//BugZilla No. 78518 12-05-2003	
						if((ret=SDS_getorientbig(pt2,fs1,&fillang))==RTERROR) 
//end progesoft
						{
							goto bail;					
						} 
						else if(ret==RTCAN) 
						{
							goto bail;
						} 
						else if(ret==RTNONE) 
						{
							fillang=IC_TWOPI;
						}
						if(numitems==0 && icadRealEqual(fillang,0.0))
							continue;
						break;
					}
				}
				else
				{
					if (!pinfo)
						goto bail;

					fillang = pinfo->FillAngle;
				}

				/*D.G.*/// Make fill angle to be in [-2pi, 2pi].
				if(!(rtmp = fmod(fillang, IC_TWOPI)) && fillang)
					rtmp = _copysign(IC_TWOPI, fillang);
				fillang = rtmp;
								
				//*** Angle between items (only if 0 was entered for number of items or angle to fill)
				if(numitems==0 || icadRealEqual(fillang,0.0)) 
				{
					if (bInteractive)
					{
						for(;;) 
						{
							if(numitems==0) 
								strncpy(fs1,ResourceString(IDC_ARRAY__NANGLE_BETWEEN_I_32, "\nAngle between items: " ),sizeof(fs1)-1);
							else 
								strncpy(fs1,(cwflag)?ResourceString(IDC_ARRAY__NANGLE_BETWEEN_I_10, "\nAngle between items (- for ccw, + for cw): " ):ResourceString(IDC_ARRAY__NANGLE_BETWEEN_I_11, "\nAngle between items (+ for ccw, - for cw): " ),sizeof(fs1)-1);
							if((ret=sds_initget(SDS_RSG_NODOCCHG | RSG_NONULL,NULL))!=RTNORM) 
								goto bail; 
							
								// Modified Cybage PP 16/03/2001[
							// Reason: Fix for bug # 67790 
							bAskPointForArray = TRUE ;
							if((ret=SDS_getorientneg(pt2,fs1,&angbtwn))==RTERROR) 
								{
								bAskPointForArray = FALSE ;
								goto bail;
								} 
							else if(ret==RTCAN) 
								{
								bAskPointForArray = FALSE ;
								goto bail;
								}
							bAskPointForArray = FALSE ;
							// Modified Cybage PP 16/03/2001]
							
							if(numitems==0) 
							{
								if (icadRealEqual(angbtwn,0.0))
									goto bail;

								/*D.G.*/// Calculate number of items by this way (old code commented out below).
								// If you enter 30 for angle between items and -300 for filling angle then
								// old way sets numitems = 9 (before comparing with IC_TWOPI)
								// with compiling optimization switched on, because fillang / angbtwn == -9.99999999999...
								rtmp = fabs(fillang / angbtwn);
								if(modf(rtmp, &TmpInteger) < IC_ZRO || TmpInteger + 1.0 - rtmp >= IC_ZRO)
									numitems = (int)TmpInteger;
								else
									numitems = (int)(TmpInteger + 1.0);

//								numitems=abs((int)(fillang / angbtwn))/* + 1*/;	/*D.G.*/// Add 1
								if(fabs(fabs(fillang) - IC_TWOPI) > IC_ZRO)		// only in this case (ACAD behaviour%).
									numitems++;
							}

							//match signs on the angles 
							if((fillang>0.0 && angbtwn<0.0) || (fillang<0.0 && angbtwn>0.0))
								angbtwn=-angbtwn;
							break;
						}
					}
					else
					{
						if (!pinfo)
							goto bail;

						angbtwn = pinfo->AngleBetweenItems;
					}
				} 
				else 
				{
					if(icadAngleEqual(fabs(fillang), IC_TWOPI))	/*D.G.*/// fabs added.
						angbtwn=fillang/numitems;
					else
						angbtwn=fillang/(numitems-1);
				}

				SDS_getvar(NULL,DB_QANGDIR,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				if(setgetrb.resval.rint)
				{
					angbtwn=-angbtwn;
					//fillang=-fillang;   NOTE: Fillang not used from here on, just angbtwn
				}
				
				//*** Rotate entities?
				if (bInteractive)
				{
					for (;;) 
					{
						sds_initget(SDS_RSG_NODOCCHG | RSG_OTHER,ResourceString(IDC_ARRAY_INITGET_YES_ROTAT_13, "Yes-Rotate_entities|Yes No-Do_not_rotate|No ~_Yes ~_No" ) );
						if((ret=sds_getkword(ResourceString(IDC_ARRAY__NROTATE_ENTITIES_14, "\nRotate entities around the array?  No/<Yes>: " ),fs1))==RTERROR) 
						{
							goto bail;
						} 
						else if(ret==RTCAN) 
						{
							goto bail;	
						} 
						else if(ret==RTNONE) 
						{
							cmd_array_rotflg=1; break;
							break;			// Default, for AutoCAD compatibility is "Yes", no matter what happened last time.			
						} 
						else if(ret==RTNORM) 
						{
							if (strisame(fs1,"YES"/*DNT*/)) 
							{
								cmd_array_rotflg=1; 
								break;
							} 
							else if (strisame(fs1,"NO"/*DNT*/)) 
							{
								cmd_array_rotflg=0; 
								break;
							} 
							else 
							{
								cmd_ErrorPrompt(CMD_ERR_YESNO,0);  // "Yes or No, please"
								continue;
							}
						} 
					}
				}
				else
				{
					if (!pinfo)
						goto bail;

					cmd_array_rotflg = pinfo->RotateItems;
				}
								
				if(mode3d==IC_ARRAYMODE_3D)
				{
					//rb3d for xforming in/out of user-defined axis system
					if (bInteractive)
					{
						if((ret=sds_initget(SDS_RSG_NODOCCHG | RSG_NONULL,NULL))!=RTNORM) 
							goto bail;
						if((ret=sds_getpoint(NULL,ResourceString(IDC_ARRAY__NCENTER_OF_POLAR_33, "\nCenter of polar array: " ),pt2))==RTERROR) 
						{
							goto bail;
						} 
						else if(ret==RTCAN) 
						{
							goto bail;
						} 
						else if(ret==RTNORM) 
						{
							//*** Set LASTPOINT system variable.
							setgetrb.restype=RT3DPOINT;
							ic_ptcpy(setgetrb.resval.rpoint,pt2);
							SDS_setvar(NULL,DB_QLASTPOINT,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
						}
						
						if((ret=sds_initget(SDS_RSG_NODOCCHG | RSG_NONULL,NULL))!=RTNORM) 
							goto bail;
						
						if((ret=sds_getpoint(pt2,ResourceString(IDC_ARRAY__NSPECIFY_SECOND__24, "\nSpecify second point along central axis of array: " ),tmppt2))==RTERROR) 
						{
							goto bail;
						} 
						else if(ret==RTCAN) 
						{
							goto bail;
						} 
						else if(ret==RTNORM) 
						{
							//*** Set LASTPOINT system variable.
							setgetrb.restype=RT3DPOINT;
							ic_ptcpy(setgetrb.resval.rpoint,tmppt2);
							SDS_setvar(NULL,DB_QLASTPOINT,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
						}
					}
					else
					{
						if (!pinfo)
							goto bail;

						ic_ptcpy (pt2, pinfo->BasePoint);
						if (pinfo->PointOnAxis)
							ic_ptcpy (tmppt2, pinfo->PointOnAxis);
					}
					
					rb3d.restype=RT3DPOINT;
					rb3d.resval.rpoint[0]=tmppt2[0]-pt2[0];
					rb3d.resval.rpoint[1]=tmppt2[1]-pt2[1];
					rb3d.resval.rpoint[2]=tmppt2[2]-pt2[2];
					tmppt[0]=sqrt(rb3d.resval.rpoint[0]*rb3d.resval.rpoint[0]+
						rb3d.resval.rpoint[1]*rb3d.resval.rpoint[1]+
						rb3d.resval.rpoint[2]*rb3d.resval.rpoint[2]);
					if(tmppt[0] < IC_ZRO)	/*D.Gavrilov*/// ACAD breaks execution
					{					// of the command in this case, but we'll
											// continue with the default value
						rb3d.resval.rpoint[0] = rb3d.resval.rpoint[1] = 0.0;
						rb3d.resval.rpoint[2] = 1.0;
					}
					else
					{
						rb3d.resval.rpoint[0]/=tmppt[0];
						rb3d.resval.rpoint[1]/=tmppt[0];
						rb3d.resval.rpoint[2]/=tmppt[0];
					}
				}
				else
				{
					//just rotating in current ucs xy plane, so rb3d=current ucs (its z)
					rb3d.restype=RTSHORT;
					rb3d.resval.rint=1;
				}
			}//Done asking questions
		}
			
		//*** Allocate the linked list of entity lists (this list now includes complex entity info also).
		//set group ptr in elistll to 0 group so we can quickly get entity types from list
		if(cmd_ss2elistll(&ellbeg,&ellcur,ss1,0)!=0) 
		{
			ret=RTERROR;
			goto bail;
		}
			/*NOTES:  If rotating the ents about the axis, this f'n arrays "incrementally" - it 
			rotates the entity around the axis by the amount angbtwn EACH TIME,
			and updates & re-makes the elist.  It then does it for each successive
			iteration.  
			If not rotating the objects, it keep modifying the original llist
			by a different amount each iteration, & then ResourceString(IDC_ARRAY_UNDOES_34, "undoes" ) the last offset
			before adding on the next one (using coldist[1] as current & coldist[0] as previous, for example).
			*/

		if(mode3d!=IC_ARRAYMODE_3DROT && !cmd_array_rotflg)
		{
			//we'll do this as an offset each time, and keep the last
			//offset so we can "counteract" it.
			coldist[1]=rowdist[1]=0.0;
		}
		else
		{
			//just keep rotating simple ents each time by the same amount
			angsin=sin(angbtwn);
			angcos=cos(angbtwn);
		}
			
		if(mode3d!=IC_ARRAYMODE_3DROT && !cmd_array_rotflg)
		{
			//If not rotating around the array, we still need a basept.  This is different
			//	from pt2, which is the center of the array.  basept is the "reference" point
			//	which will remain at a constant distance from pt2 throughout the rotation.
			//	basept is used for ALL objects in the set, and is taken from the last object in
			//	the selection set.
			//	It's usually the 10-pt for an entity, or if a pline it's the 10pt of the 
			//	1st vertex.  
			//*** If 2D, Get the base point from the LAST entity picked in selection set.
			//*** If 3D, Get the base point from the FIRST entity picked in selection set.
			//rather than walk each list and find if ent is complex, get info from ent
			//adjust the base pt so it's in UCS coordinates
			if (mode3d==IC_ARRAYMODE_2D) 
				sds_ssname(ss1,fl1-1,ename);
			else 
				sds_ssname(ss1,0,ename);

			if (NULL==(tmprb=sds_entget(ename))) 
			{
				ret=RTERROR;
				goto bail;
			}
				
			ic_assoc(tmprb,0);
			strncpy(fs1,ic_rbassoc->resval.rstring,sizeof(fs1)-1);
			if(strsame(fs1,"POLYLINE"/*DNT*/))
			{
				ic_assoc(tmprb,70);
				if(ic_rbassoc->resval.rint&(IC_PLINE_3DPLINE | IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH))
				{
					rbent.restype=RTSHORT;
					rbent.resval.rint=0;
				}
				else
				{
					strcpy(fs1,"CIRCLE"/*DNT*/);//get 210 group off ent
					ic_assoc(tmprb,10);
					pl_stpt[2]=ic_rbassoc->resval.rpoint[2];	
				}
			}

			if(RTNORM==sds_wcmatch(fs1,"LWPOLYLINE,CIRCLE,ARC,INSERT,TEXT,DIMENSION,SOLID,TRACE,ATTDEF,ATTRIB,SHAPE"/*DNT*/))
			{
				rbent.restype=rbent2.restype=RT3DPOINT;
				if(0==ic_assoc(tmprb,210))
				{
					ic_ptcpy(rbent.resval.rpoint,ic_rbassoc->resval.rpoint);
					ic_ptcpy(rbent2.resval.rpoint,ic_rbassoc->resval.rpoint);
				}
				else
				{
					rbent.resval.rpoint[0]=rbent.resval.rpoint[1]=rbent2.resval.rpoint[0]=rbent2.resval.rpoint[1]=0.0;
					rbent.resval.rpoint[2]=rbent2.resval.rpoint[2]=1.0;	
				}
			}
			else
			{
				rbent.restype=RTSHORT;
				rbent.resval.rint=0;	
			}

			if(0==(ic_assoc(tmprb,66)) && ic_rbassoc->resval.rint==1)
			{
				sds_relrb(tmprb);
				sds_entnext_noxref(ename,ename);
				tmprb=sds_entget(ename);
			}

			if(baseflg==false)
			{ //if basepoint was not given
				//note: for ellipses, default base is center
				if(strsame(fs1,"SPLINE"/*DNT*/) || strsame(fs1,"MLINE"/*DNT*/))
				{
					ic_assoc(tmprb,11);
					ic_ptcpy(basept,ic_rbassoc->resval.rpoint);
				}
                //<ALM>
                else if (strsame(fs1,"3DSOLID") || strsame(fs1,"BODY") || strsame(fs1, "REGION"))
                {
					CDbAcisEntityData obj;
					obj.setDbtype(fs1);
					obj.setSat(ic_rbassoc->resval.rstring); 
                    point base;
                    if (!CModeler::get()->getBasePoint(obj, base))
                    {
                        IC_RELRB(tmprb);
                        ret = RTERROR;
                        goto bail;
                    }
                    ic_ptcpy(basept, base);
                }
                //</ALM>
				else
				{
					if(0!=ic_assoc(tmprb,10))
					{
						IC_RELRB(tmprb);
						ret=RTERROR;
						goto bail;
					}

					ic_ptcpy(basept,ic_rbassoc->resval.rpoint);
					if(strsame(fs1,"POLYLINE"/*DNT*/) && rbent.restype==RT3DPOINT)
						basept[2]=pl_stpt[2];

				}
			}
			sds_trans(basept,&rbent,&rbucs,0,basept);
			sds_relrb(tmprb);tmprb=NULL;
		}
		else
		{
			//If we're rotating each ent about an axis and rotating them individually, then
			//	each ent is going to get a new extrusion direction.  Keep track of this by
			//	putting the current extru in an array.  If the x element of the extru element
			//	is CMD_ENT_NATIVEWCS, then the ent is a wcs entity.
			//So, let's alloc an array of points, and put ent extrusions in them
			//Note: fl1 was length of set chosen by user.  We need to increment that by
			//	the number of attribs, because they too have extrusions
			for(elltmp=ellbeg; elltmp!=NULL; elltmp=elltmp->next) 
			{
				//remember, grpptr points to etype in array
				if(strsame(elltmp->grpptr->resval.rstring,"ATTRIB"/*DNT*/))
					fl1++;

				//SPECIAL NOTE: for mtext ent, we need to omit the 50 group from
				//	the elist.  when we entmod below, we'll use ONLY the 11 to
				//	determine the angle of the mtext
				else if(strsame(elltmp->grpptr->resval.rstring,"MTEXT"/*DNT*/))
				{
					struct resbuf *rb50;
					for(tmprb=elltmp->entlst; tmprb!=NULL && tmprb->rbnext!=NULL; tmprb=tmprb->rbnext) 
					{
						if(tmprb->rbnext->restype==50)
						{
							rb50=tmprb->rbnext;
							tmprb->rbnext=rb50->rbnext;
							rb50->rbnext=NULL;
							sds_relrb(rb50);
							rb50=tmprb=NULL;
							break;
						}
					}
				}
			}
			
			if(NULL==(ptarray= new sds_point [fl1] ))
			{
				ret=RTERROR;goto bail;
			}
			memset(ptarray,0,sizeof(sds_point)*fl1);

			fl2=0L;
			for (elltmp=ellbeg; elltmp!=NULL && fl2<fl1; elltmp=elltmp->next)
			{
				//for sub-ents, use the parent's extrusion
				if (RTNORM==sds_wcmatch(elltmp->grpptr->resval.rstring,"VERTEX,SEQEND"/*DNT*/))
					continue;
				ptarray[fl2][0]=CMD_ENT_NATIVEWCS; //default flag for a wcs entity
				if (strsame(elltmp->grpptr->resval.rstring,"POLYLINE"/*DNT*/))
				{
					if (0== ic_assoc(elltmp->entlst,70) &&
						0==(ic_rbassoc->resval.rint&(IC_PLINE_3DPLINE | IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH)))
					{
						if(0==ic_assoc(elltmp->entlst,210))
							ic_ptcpy(ptarray[fl2],ic_rbassoc->resval.rpoint);
						else
						{
							ptarray[fl2][0]=ptarray[fl2][1]=0.0;
							ptarray[fl2][2]=1.0;
						}
					}
				}
				else if (strsame(elltmp->grpptr->resval.rstring,"LWPOLYLINE"/*DNT*/))
				{
					if(0==ic_assoc(elltmp->entlst,210))
						ic_ptcpy(ptarray[fl2],ic_rbassoc->resval.rpoint);
					else
					{
						ptarray[fl2][0]=ptarray[fl2][1]=0.0;
						ptarray[fl2][2]=1.0;
					}
				}
				else if(RTNORM==sds_wcmatch(elltmp->grpptr->resval.rstring,"CIRCLE,ARC,INSERT,DIMENSION,TEXT,SOLID,TRACE,ATTDEF,ATTRIB,SHAPE"/*DNT*/))
				{
					//note: we'll put dims as ecs, because we'll have the wcs resbuf when we need it
					if(0==ic_assoc(elltmp->entlst,210))
						ic_ptcpy(ptarray[fl2],ic_rbassoc->resval.rpoint);
					else
					{
						ptarray[fl2][0]=ptarray[fl2][1]=0.0;
						ptarray[fl2][2]=1.0;
					}
				}
				fl2++;
			}		
		}

		//use a progress bar if we're going to make a boatload of stuff
		if(numitems*fl1>500)
		{
			curprogress=0;
			SDS_ProgressStart();
		}
		else
		{
			curprogress=-1;	
		}

		/*D.Gavrilov*/// We'll do things in 3D cases like as in 3DROTATE
		// (look at cmd_rotate3d), because we may have rotated UCS.
		sds_matrix mat;
		sds_point rb3d_vec;
		if(mode3d!=IC_ARRAYMODE_2D) 
		{
			sds_point temp_pt2;
			sds_trans(pt2, &rbucs, &rbwcs, 0, temp_pt2);
			sds_trans(pt2,&rbucs,&rb3d,0,pt2);
			sds_trans(rb3d.resval.rpoint, &rbucs, &rbwcs, 1, rb3d_vec);
			ic_rotxform(temp_pt2, rb3d_vec, angbtwn, mat);
			if(mode3d!=IC_ARRAYMODE_3DROT && !cmd_array_rotflg)
				sds_trans(basept,&rbucs,&rb3d,0,basept);
		}

		for(fi1=1;fi1<numitems;fi1++)
		{
			if(curprogress>=0)
			{
				nextprogress=(fi1*100/numitems);
				if(nextprogress-curprogress>=5)
				{
					curprogress=nextprogress;
					SDS_ProgressPercent(curprogress);
				}
			}

			if(mode3d!=IC_ARRAYMODE_3DROT && !cmd_array_rotflg)
			{
				//each successive iteration of a non-rotated ent has different offsets
				//	from the previous iteration.  We need to keep the last one & counteract it
				coldist[0]=coldist[1];
				rowdist[0]=rowdist[1];
				angcos=cos(fi1*angbtwn);//we need to keep getting new rotation values
				angsin=sin(fi1*angbtwn);
				coldist[1]=(basept[0]-pt2[0])*angcos-(basept[1]-pt2[1])*angsin+pt2[0]-basept[0];//x offset
				rowdist[1]=(basept[1]-pt2[1])*angcos+(basept[0]-pt2[0])*angsin+pt2[1]-basept[1];//y offset 
			}

            // SystemMetrix(Hiro)-[FIX: the copied annotation is associated with invalid leader.
            ReactorFixups reactorFixup;
            // ]-SystemMetrix(Hiro) FIX: the copied annotation is associated with invalid leader.

			fl2=-1L;//counter for which pt in array is current extru -we'll pre-increment it
			for(elltmp=ellbeg; elltmp!=NULL; elltmp=elltmp->next) 
			{
				if(sds_usrbrk()) 
				{
					ret=RTCAN;
					goto bail;
				}

				for(tmprb=elltmp->entlst; tmprb!=NULL; tmprb=tmprb->rbnext) 
				{
					if(tmprb->restype==0)
					{
						//set up the UCS native to the entity (don't do it for subents using parent's coords
						rbname=tmprb;
						if((IC_ARRAYMODE_3DROT==mode3d || cmd_array_rotflg) && 
                            RTNORM!=sds_wcmatch(tmprb->resval.rstring,"VERTEX,SEQEND"/*DNT*/))
						{
							fl2++;
							//lines & pts may have extru dir, but are still in WCS
							if(ptarray[fl2][0]==CMD_ENT_NATIVEWCS)
							{
								//translate basept for array from ucs to wcs
								rbent.restype=rbent2.restype=RTSHORT;
								rbent.resval.rint=rbent2.resval.rint=0;
							}
							else
							{
								//translate basept into ecs
								rbent.restype=rbent2.restype=RT3DPOINT;
								ic_ptcpy(rbent.resval.rpoint,ptarray[fl2]);

								if(IC_ARRAYMODE_3DROT==mode3d || cmd_array_rotflg)
								{
									/*D.Gavrilov*/// We must do it like as in SDS_xforment in 3D cases
									if(mode3d == IC_ARRAYMODE_2D)
									{
										sds_trans(ptarray[fl2],&rbwcs,&rb3d,1,tmppt2);
										tmppt[0]=tmppt2[0]*angcos-tmppt2[1]*angsin;
										tmppt[1]=tmppt2[1]*angcos+tmppt2[0]*angsin;
										tmppt[2]=tmppt2[2];
										sds_trans(tmppt,&rb3d,&rbwcs,1,rbent2.resval.rpoint);
									}
									else
									{
										sds_point ap1, ap2, newextru;
										ap1[0] = ap1[1] = ap1[2] = 0.0;
										ic_ptcpy(ap2, ptarray[fl2]);
										cmd_xformpt(ap1, ap1, mat);
										cmd_xformpt(ap2, ap2, mat);
										newextru[0] = ap2[0] - ap1[0];
										newextru[1] = ap2[1] - ap1[1];
										newextru[2] = ap2[2] - ap1[2];
										sds_real l = sqrt(newextru[0] * newextru[0] +
														  newextru[1] * newextru[1] +
														  newextru[2] * newextru[2]);
										newextru[0] /= l; newextru[1] /= l; newextru[2] /= l;
										ic_ptcpy(rbent2.resval.rpoint, newextru);
									}

								}
								else
								{
									ic_ptcpy(rbent2.resval.rpoint,rbent.resval.rpoint);	
								}	
							}
						}

						if(mode3d!=IC_ARRAYMODE_3DROT && !cmd_array_rotflg && rbent.restype==RT3DPOINT && 
                           strsame(rbname->resval.rstring,"POLYLINE"/*DNT*/))
						{
							//if this is a 2D poly and we're not rotating the ents, get the
							//	10 pt on the 1st vertex, and copy it to the 10pt on main ent
							if(0==ic_assoc(elltmp->next->entlst,10))
								ic_ptcpy(pl_stpt,ic_rbassoc->resval.rpoint);
						}
					}
					else if((mode3d==IC_ARRAYMODE_3DROT || cmd_array_rotflg) && 
                            (tmprb->restype==41 || tmprb->restype==42) && 
                            (strsame(elltmp->grpptr->resval.rstring,"ELLIPSE"/*DNT*/)))
					{
						//for ellipses, parameter is NOT measured from angbase, it's measured from
						//the axis direction.  If we walk that correctly, there's no need to adjust the parameters
						continue;	
					}
					else if((mode3d==IC_ARRAYMODE_3DROT || cmd_array_rotflg) && 
                            (tmprb->restype==51) && 
                            (sds_wcmatch(elltmp->grpptr->resval.rstring,"TEXT,ATTRIB,ATTDEF"/*DNT*/)==RTNORM))
					{
						//do not change the oblique angle when rotatint text
						continue;		
					}
					else if(((mode3d==IC_ARRAYMODE_3DROT || cmd_array_rotflg) && 
                             (tmprb->restype==50 || tmprb->restype==51) && 
                             (sds_wcmatch(elltmp->grpptr->resval.rstring,"ARC,INSERT,TEXT,DIMENSION,ATTRIB,ATTDEF"/*DNT*/)==RTNORM)))
					{
						//increment angle.  In 3d, convert angle to vector (z=0) in ecs plane.
						//	Then transform vector into new ecs and convert back to angle (ignoring z)
						if(strsame(elltmp->grpptr->resval.rstring,"DIMENSION"/*DNT*/) && tmprb->restype==51)continue;
						tmppt[0]=cos(tmprb->resval.rreal);
						tmppt[1]=sin(tmprb->resval.rreal);
						tmppt[2]=0.0;
						if(mode3d!=IC_ARRAYMODE_2D)
							sds_trans(tmppt,&rbent,&rb3d,1,tmppt2);
						else
							sds_trans(tmppt,&rbent,&rbucs,1,tmppt2);
						tmppt[0]=tmppt2[0]*angcos-tmppt2[1]*angsin;
						tmppt[1]=tmppt2[1]*angcos+tmppt2[0]*angsin;
						tmppt[2]=tmppt2[2];
						if(mode3d!=IC_ARRAYMODE_2D)
							sds_trans(tmppt,&rb3d,&rbent2,1,tmppt2);
						else
							sds_trans(tmppt,&rbucs,&rbent2,1,tmppt2);
						tmprb->resval.rreal=atan2(tmppt2[1],tmppt2[0]);
						ic_normang(&tmprb->resval.rreal,NULL);

					}
					else if(tmprb->restype>=10 && tmprb->restype<=18) 
					{
						if(tmprb->restype==12 && strsame(rbname->resval.rstring,"DIMENSION"/*DNT*/))
							continue;
						else if(						//if this is a vector parameter...
								(tmprb->restype==11 &&
								((strsame(rbname->resval.rstring,"RAY"/*DNT*/)) ||
								(strsame(rbname->resval.rstring,"XLINE"/*DNT*/)) ||
								(strsame(rbname->resval.rstring,"MTEXT"/*DNT*/)) ||
								(strsame(rbname->resval.rstring,"IMAGE"/*DNT*/)) ||
								(strsame(rbname->resval.rstring,"TOLERANCE"/*DNT*/)) ||
								(strsame(rbname->resval.rstring,"ELLIPSE"/*DNT*/))))
								||
								(tmprb->restype==12 && strsame(rbname->resval.rstring,"IMAGE"/*DNT*/))
								||
								((tmprb->restype==12 || tmprb->restype==13) && (strsame(rbname->resval.rstring,"SPLINE"/*DNT*/) || strsame(rbname->resval.rstring,"MLINE"/*DNT*/)))
							   )
						{
							//for vector items, translate about the origin rather than pt2
							if(mode3d!=IC_ARRAYMODE_3DROT && !cmd_array_rotflg)
								continue;

							if(mode3d!=IC_ARRAYMODE_2D)
								sds_trans(tmprb->resval.rpoint,&rbent,&rb3d,1,tmppt2);
							else
								sds_trans(tmprb->resval.rpoint,&rbent,&rbucs,1,tmppt2);	

							tmppt[0]=tmppt2[0]*angcos-tmppt2[1]*angsin;
							tmppt[1]=tmppt2[1]*angcos+tmppt2[0]*angsin;
							tmprb->resval.rpoint[0]=tmppt[0];
							tmprb->resval.rpoint[1]=tmppt[1];
							tmprb->resval.rpoint[2]=tmppt2[2];

							if(mode3d!=IC_ARRAYMODE_2D)
								sds_trans(tmprb->resval.rpoint,&rb3d,&rbent2,1,tmprb->resval.rpoint);
							else
								sds_trans(tmprb->resval.rpoint,&rbucs,&rbent2,1,tmprb->resval.rpoint);

							continue;
						}

						if(mode3d!=IC_ARRAYMODE_3DROT && !cmd_array_rotflg)
						{
							if(rbent.restype==RT3DPOINT && strsame(rbname->resval.rstring,"POLYLINE"/*DNT*/))
								ic_ptcpy(tmprb->resval.rpoint,pl_stpt);//make sure we transform to correct elev

							if(mode3d==IC_ARRAYMODE_3D)
							{
								if(strsame(rbname->resval.rstring,"DIMENSION"/*DNT*/) && tmprb->restype!=11 && tmprb->restype!=16)
									sds_trans(tmprb->resval.rpoint,&rbwcs,&rb3d,0,tmppt);
								else
									sds_trans(tmprb->resval.rpoint,&rbent,&rb3d,0,tmppt);
							}
							else
							{
								if(strsame(rbname->resval.rstring,"DIMENSION"/*DNT*/) && tmprb->restype!=11 && tmprb->restype!=16)
									sds_trans(tmprb->resval.rpoint,&rbwcs,&rbucs,0,tmppt);
								else
									sds_trans(tmprb->resval.rpoint,&rbent,&rbucs,0,tmppt);	
							}
							tmppt[0]+=(coldist[1]-coldist[0]);
							tmppt[1]+=(rowdist[1]-rowdist[0]);

							if(mode3d==IC_ARRAYMODE_3D)
							{
								if(strsame(rbname->resval.rstring,"DIMENSION"/*DNT*/) && tmprb->restype!=11 && tmprb->restype!=16)
									sds_trans(tmppt,&rb3d,&rbwcs,0,tmprb->resval.rpoint);
								else
									sds_trans(tmppt,&rb3d,&rbent2,0,tmprb->resval.rpoint);
							}
							else
							{
								if(strsame(rbname->resval.rstring,"DIMENSION"/*DNT*/) && tmprb->restype!=11 && tmprb->restype!=16)
									sds_trans(tmppt,&rbucs,&rbwcs,0,tmprb->resval.rpoint);
								else
									sds_trans(tmppt,&rbucs,&rbent2,0,tmprb->resval.rpoint);	
							}
							if(rbent.restype==RT3DPOINT && strsame(rbname->resval.rstring,"POLYLINE"/*DNT*/))
							{
								//save this elev for use on subsequent vertices
								pl_stpt[2]=tmprb->resval.rpoint[2];
								tmprb->resval.rpoint[0]=tmprb->resval.rpoint[1]=0.0;
							}
							else if(rbent.restype==RT3DPOINT && strsame(rbname->resval.rstring,"VERTEX"/*DNT*/))
							{
								//reset elev on 2d vertex to that of parent
								tmprb->resval.rpoint[2]=pl_stpt[2];	
							}
						}
						else
						{
							if(mode3d!=IC_ARRAYMODE_2D)
							{
								if(strsame(rbname->resval.rstring,"DIMENSION"/*DNT*/) && tmprb->restype!=11 && tmprb->restype!=16)
									sds_trans(tmprb->resval.rpoint,&rbwcs,&rb3d,0,tmprb->resval.rpoint);
								else
									sds_trans(tmprb->resval.rpoint,&rbent,&rbwcs,0,tmprb->resval.rpoint);
							}
							else
							{
								if(strsame(rbname->resval.rstring,"DIMENSION"/*DNT*/) && tmprb->restype!=11 && tmprb->restype!=16)
									sds_trans(tmprb->resval.rpoint,&rbwcs,&rbucs,0,tmprb->resval.rpoint);
								else
									sds_trans(tmprb->resval.rpoint,&rbent,&rbucs,0,tmprb->resval.rpoint);	
							}

							/*D.Gavrilov*/// let's transform in cases when we'll not use mat
							if( (mode3d == IC_ARRAYMODE_2D) ||
								(strsame(rbname->resval.rstring, "DIMENSION") &&
								 tmprb->restype != 11 &&
								 tmprb->restype != 16) )
							{
								tmppt[0]=pt2[0]+(tmprb->resval.rpoint[0]-pt2[0])*angcos-(tmprb->resval.rpoint[1]-pt2[1])*angsin;
								tmppt[1]=pt2[1]+(tmprb->resval.rpoint[1]-pt2[1])*angcos+(tmprb->resval.rpoint[0]-pt2[0])*angsin;
								tmprb->resval.rpoint[0]=tmppt[0];
								tmprb->resval.rpoint[1]=tmppt[1];
							}

							if(mode3d!=IC_ARRAYMODE_2D)
							{
								if(strsame(rbname->resval.rstring,"DIMENSION"/*DNT*/) && tmprb->restype!=11 && tmprb->restype!=16)
									sds_trans(tmprb->resval.rpoint,&rb3d,&rbwcs,0,tmprb->resval.rpoint);
								else
								{	/*D.Gavrilov*/// now let's transform by mat
									cmd_xformpt(tmprb->resval.rpoint, tmprb->resval.rpoint, mat);
									sds_trans(tmprb->resval.rpoint,&rbwcs,&rbent2,0,tmprb->resval.rpoint);
								}
							}
							else
							{
								if(strsame(rbname->resval.rstring,"DIMENSION"/*DNT*/) && tmprb->restype!=11 && tmprb->restype!=16)
									sds_trans(tmprb->resval.rpoint,&rbucs,&rbwcs,0,tmprb->resval.rpoint);
								else
									sds_trans(tmprb->resval.rpoint,&rbucs,&rbent2,0,tmprb->resval.rpoint);	
							}	
						}								
					}
                    // SystemMetrix(Hiro)-[FIX: the X direction of leader is not rotated in rotated mode.
                    else if (tmprb->restype == 211)
                    {
                        if (cmd_array_rotflg)
                        {
                            // x direction of leader
                            if (mode3d == IC_ARRAYMODE_2D)
                            {
                                sds_trans(tmprb->resval.rpoint, &rbwcs, &rbucs, 1, tmppt2);
                                tmppt[0] = tmppt2[0] * angcos - tmppt2[1] * angsin;
                                tmppt[1] = tmppt2[1] * angcos + tmppt2[0] * angsin;
                                tmppt[2] = tmppt2[2];
                                sds_trans(tmppt, &rbucs, &rbwcs, 1, tmprb->resval.rpoint);
                            }
                            else
                            {
                                sds_point ap1, ap2, NewVec;
                                ap1[0] = ap1[1] = ap1[2] = 0.0;
                                ic_ptcpy(ap2, tmprb->resval.rpoint);
                                cmd_xformpt(ap1, ap1, mat);
                                cmd_xformpt(ap2, ap2, mat);
                                NewVec[0] = ap2[0] - ap1[0];
                                NewVec[1] = ap2[1] - ap1[1];
                                NewVec[2] = ap2[2] - ap1[2];
                                ic_unitize(NewVec);
                                ic_ptcpy(tmprb->resval.rpoint, NewVec);
                            }
                        }
                    }
                    // ]-SystemMetrix(Hiro) FIX: the X direction of leader is not rotated in rotated mode.
					else if((mode3d==IC_ARRAYMODE_3DROT || cmd_array_rotflg) && tmprb->restype==210)
					{
						//no wcs entities should be in here, because they don't have 210's
						if(ptarray[fl2][0]!=CMD_ENT_NATIVEWCS)
						{
							ic_ptcpy(tmprb->resval.rpoint,rbent2.resval.rpoint);
							ic_ptcpy(ptarray[fl2],tmprb->resval.rpoint);
						}
						else
							/*D.Gavrilov*/// Do it like as in SDS_xforment in 3D cases
							// (old code commented out below).
						{
							if(mode3d!=IC_ARRAYMODE_2D)
							{
								sds_point ap1, ap2, newextru;
								ap1[0] = ap1[1] = ap1[2] = 0.0;
								ic_ptcpy(ap2, tmprb->resval.rpoint);
								cmd_xformpt(ap1, ap1, mat);
								cmd_xformpt(ap2, ap2, mat);
								newextru[0] = ap2[0] - ap1[0];
								newextru[1] = ap2[1] - ap1[1];
								newextru[2] = ap2[2] - ap1[2];
								ic_unitize(newextru);
								ic_ptcpy(tmprb->resval.rpoint, newextru);
							}
							else
							{
								sds_trans(tmprb->resval.rpoint,&rbwcs,&rbucs,1,tmppt2);
								tmppt[0]=tmppt2[0]*angcos-tmppt2[1]*angsin;
								tmppt[1]=tmppt2[1]*angcos+tmppt2[0]*angsin;
								tmppt[2]=tmppt2[2];
								sds_trans(tmppt,&rbucs,&rbwcs,1,tmprb->resval.rpoint);	
							}
						}

							/*{
							//rotate entity's extrusion direction manually (ray,line,xline,point,ellipse)
							if(mode3d!=IC_ARRAYMODE_2D)
								sds_trans(tmprb->resval.rpoint,&rbwcs,&rb3d,1,tmppt2);
							else
								sds_trans(tmprb->resval.rpoint,&rbwcs,&rbucs,1,tmppt2);
							tmppt[0]=tmppt2[0]*angcos-tmppt2[1]*angsin;
							tmppt[1]=tmppt2[1]*angcos+tmppt2[0]*angsin;
							tmppt[2]=tmppt2[2];
							if(mode3d!=IC_ARRAYMODE_2D)
								sds_trans(tmppt,&rb3d,&rbwcs,1,tmprb->resval.rpoint);
							else
								sds_trans(tmppt,&rbucs,&rbwcs,1,tmprb->resval.rpoint);	
						}*/
					}
						/*D.G.*/// Transforming of 1011-1013 EED added.
					else if(tmprb->restype == 1011)
					{
						if(mode3d != IC_ARRAYMODE_3DROT && !cmd_array_rotflg)
						{
							if(mode3d == IC_ARRAYMODE_3D)
								sds_trans(tmprb->resval.rpoint, &rbwcs, &rb3d, 0, tmppt);
							else
								sds_trans(tmprb->resval.rpoint, &rbwcs, &rbucs, 0, tmppt);

							tmppt[0] += coldist[1] - coldist[0];
							tmppt[1] += rowdist[1] - rowdist[0];

							if(mode3d == IC_ARRAYMODE_3D)
								sds_trans(tmppt, &rb3d, &rbwcs, 0, tmprb->resval.rpoint);
							else
								sds_trans(tmppt, &rbucs, &rbwcs, 0, tmprb->resval.rpoint);
						}
						else
						{
							if(mode3d == IC_ARRAYMODE_2D)
							{
								sds_trans(tmprb->resval.rpoint, &rbwcs, &rbucs, 0, tmprb->resval.rpoint);
								tmppt[0] = pt2[0] + (tmprb->resval.rpoint[0] - pt2[0]) * angcos - (tmprb->resval.rpoint[1] - pt2[1]) * angsin;
								tmppt[1] = pt2[1] + (tmprb->resval.rpoint[1] - pt2[1]) * angcos + (tmprb->resval.rpoint[0] - pt2[0]) * angsin;
								tmprb->resval.rpoint[0] = tmppt[0];
								tmprb->resval.rpoint[1] = tmppt[1];
							}
							else
								cmd_xformpt(tmprb->resval.rpoint, tmprb->resval.rpoint, mat);
						}								
					}
					else if((mode3d == IC_ARRAYMODE_3DROT || cmd_array_rotflg) &&
							(tmprb->restype == 1012 || tmprb->restype == 1013))
					{
							if(mode3d != IC_ARRAYMODE_2D)
							{
								sds_point ap1, ap2, NewVec;
								ap1[0] = ap1[1] = ap1[2] = 0.0;
								ic_ptcpy(ap2, tmprb->resval.rpoint);
								cmd_xformpt(ap1, ap1, mat);
								cmd_xformpt(ap2, ap2, mat);
								NewVec[0] = ap2[0] - ap1[0];
								NewVec[1] = ap2[1] - ap1[1];
								NewVec[2] = ap2[2] - ap1[2];
								ic_unitize(NewVec);
								ic_ptcpy(tmprb->resval.rpoint, NewVec);
							}
							else
							{
								sds_trans(tmprb->resval.rpoint, &rbwcs, &rbucs, 1, tmppt2);
								tmppt[0] = tmppt2[0] * angcos - tmppt2[1] * angsin;
								tmppt[1] = tmppt2[1] * angcos + tmppt2[0] * angsin;
								tmppt[2] = tmppt2[2];
								sds_trans(tmppt, &rbucs, &rbwcs, 1, tmprb->resval.rpoint);	
							}
					}
                    //<ALM>
                    else if (tmprb->restype == 1 &&
                            (sds_wcmatch(rbname->resval.rstring, "3DSOLID,BODY,REGION"/*DNT*/)==RTNORM))
                    {
#if 0
                        // Calculate transformation matrix
                        sds_matrix mx;
                        if (mode3d == IC_ARRAYMODE_2D)
                        {
    		                if (cmd_array_rotflg)
                            {
                                // with rotation
                                ic_rotxform(pt2, angbtwn, mx);
                            }
                            else
                            {
                                // without rotation

                                // rotate base point around the array center 
                                // on the current angle value
                                double angle = fi1*angbtwn;
                                ic_rotxform(pt2, angle, mx);
                                sds_point nextBase;
                                ic_ptcpy(nextBase, basept);
                                ic_xformpt(basept, nextBase, mx);

                                // rotate base point on the previous angle value
                                sds_point curBase;
                                if (fi1 == 1)
                                {
                                    ic_ptcpy(curBase, basept);
                                }
                                else
                                {
                                    angle = (fi1-1)*angbtwn;
                                    ic_rotxform(pt2, angle, mx);
                                    ic_xformpt(basept, curBase, mx);
                                }

                                // need transformation is just translation
                                sds_point shift;
                                shift[0] = nextBase[0] - curBase[0];
                                shift[1] = nextBase[1] - curBase[1];
                                shift[2] = nextBase[2] - curBase[2];

                                ic_transmat(shift[0], shift[1], shift[2], mx);
                            }
                        }
                        else
                        {
                            // for this case transformation was calculated early
                            ic_copy(mx, mat);
                        }

                        // Reduce to Acis transformation form
                        ic_transpose(mx);
#else
                        // Calculate transformation matrix
                        transf tr;
                        if (mode3d == IC_ARRAYMODE_2D)
                        {
                            point root(pt2);
                            gvector axis(0.,0.,1.);

    		                if (cmd_array_rotflg)
                            {
                                // with rotation
                                tr.rotate(angbtwn, root, axis);
                            }
                            else
                            {
                                // without rotation

                                // rotate base point around the array center 
                                // on the current angle value
                                double angle = fi1*angbtwn;

                                point nextBase(basept);
                                nextBase *= icl::rotation(angle, root, axis);

                                {
                                    point proot(root.x(), root.y(), 0);
                                    point pnbase(nextBase.x(), nextBase.y(), 0);
                                    point pbase(basept[0], basept[1], 0);

                                    double d1 = (pbase-proot).norm();
                                    double d2 = (pnbase-proot).norm();
                                    assert(fabs(d1-d2) < 1.e-6);
                                }

                                // rotate base point on the previous angle value
                                point curBase(basept);
                                if (fi1 != 1)
                                {
                                    angle = (fi1-1)*angbtwn;
                                    curBase *= icl::rotation(angle, root, axis);
                                }

                                // need transformation is just translation
                                tr.translate(nextBase - curBase);
                            }
                        }
                        else
                        {
                            // for this case transformation was calculated early
                            tr = mat;
                        }
                        wcs2ucs2wcs(tr);
#endif

                        // Transform object
                        CDbAcisEntityData newObj;
                        CDbAcisEntityData oldObj;
						oldObj.setSat(tmprb->resval.rstring);
						oldObj.setDbtype(rbname->resval.rstring);
                        if (CModeler::get()->transform(oldObj, newObj, tr))
                        {
                            // Change resbuffer data
							char*& sat = tmprb->resval.rstring;
                            IC_FREE(sat);
                            sat = new char [newObj.getSat().size()+1];
                            strcpy(sat, newObj.getSat().c_str());
                        }
                    }
                    //</ALM>

				} // for(tmprb=elltmp->entlst; ... thru resbufs list

				/*D.Gavrilov*/// We should set the correct elevation.
				if(strsame(rbname->resval.rstring,"LWPOLYLINE"/*DNT*/))
				{
					struct resbuf* prb10 = ic_ret_assoc(elltmp->entlst, 10);
					struct resbuf* prb38 = ic_ret_assoc(elltmp->entlst, 38);
					prb38->resval.rreal = prb10->resval.rpoint[2];
				}
                // Make entity
				//Modified Cybage SBD 17/12/2001 DD/MM/YYYY 
				//Reason : Fix for Bug No. 77866 and 77867 from Bugzilla( array case ) 
				bdimUpdate=TRUE;
				if(mode3d==IC_ARRAYMODE_3DROT)
				{
					if((ret=sds_entmod(elltmp->entlst))!=RTNORM) 
						goto bail; 
				}
				else
				{
					//Bugzilla No. 77993 ; date : 18-03-2002 [					
					SDS_AtNodeDrag = 1;
					if((ret=sds_entmake(elltmp->entlst))!=RTNORM) 
						goto bail; 	
					SDS_AtNodeDrag = SavedSDS_AtNodeDrag;					
					//Bugzilla No. 77993 ; date : 18-03-2002 ]

					if (setofnewents)
					{
						sds_name tmpename;
						sds_entlast(tmpename);
						if (tmpename[0] != ellbeg->ename[0])
							sds_ssadd (tmpename, setofnewents, setofnewents);
					}
					
						// add new entity to the newSS for SDS_CBENDCLONE notification	
						sds_name tmpEname;
						sds_entlast( tmpEname );
						sds_ssadd( tmpEname, newSS, newSS );

                    // SystemMetrix(Hiro)-[FIX: the copied annotation is associated with invalid leader.
                    db_handitem *oldEntity = (db_handitem*)elltmp->ename[0];
                    db_handitem *newEntity = (db_handitem*)tmpEname[0];
                    reactorFixup.AddHanditem(newEntity);
                    reactorFixup.AddHandleMap(oldEntity->RetHandle(), newEntity->RetHandle());
                    // ]-SystemMetrix(Hiro) FIX: the copied annotation is associated with invalid leader.
				}

			} // for(elltmp=ellbeg; ...	thru entities

            // SystemMetrix(Hiro)-[FIX: the copied annotation is associated with invalid leader.
            // fix the reactors on the copied entities
            reactorFixup.ApplyFixups(SDS_CURDWG);
            // ]-SystemMetrix(Hiro) FIX: the copied annotation is associated with invalid leader.

		} // for(fi1=1; ... thru created items

	} // if cmd_array_defchar == 'P' OR if mode3d == IC_ARRAYMODE_3DROT

	ret=RTNORM;

bail:
	if( bCloneBeginNotification )
{
		sds_name initSS;
		initSS[0] = initSS[1] = 0;
		SDS_sscpy( initSS, ss1 );	// to avoid modifying ss1 in notification handler
		SDS_CallUserCallbackFunc(SDS_CBENDCLONE, (void*)initSS, (void*)newSS, (void*)0);
		sds_ssfree( initSS );
}

	sds_ssfree( newSS );
	
	SDS_getvar(NULL,DB_QHIGHLIGHT,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
    if(setgetrb.resval.rint) 
	{
        //*** Unhighlight selection set of entities.
        for(long ssct=0L; sds_ssname(ss1,ssct,ename)==RTNORM; ssct++) 
			sds_redraw(ename,IC_REDRAW_UNHILITE);
	}

	if(curprogress>=0)
		SDS_ProgressStop();
    
	cmd_free_elistll(&ellbeg,&ellcur);
    if(mode3d!=IC_ARRAYMODE_3DROT)
		sds_ssfree(ss1); 
	IC_RELRB(tmprb);

	if(ptarray!=NULL)
		IC_FREE(ptarray);
	//Modified Cybage SBD 17/12/2001 DD/MM/YYYY 
	//Reason : Fix for Bug No. 77866 and 77867 from Bugzilla( array case ) 
    bdimUpdate=FALSE;
	
	//Bugzilla No. 77993 ; date : 18-03-2002 
	SDS_AtNodeDrag = SavedSDS_AtNodeDrag;
	return(ret);
}

void cmd_xformpt(sds_point from, sds_point to, sds_matrix mat)
{
/*D.Gavrilov*/// It's like as sds_xformpt in sds_xform.cpp.
//  Applies an ADS-type matrix to a point.
//  'from and 'to' can be the same point in memory.

    sds_point ap1;

    ap1[0] = mat[0][0] * from[0] + mat[0][1] * from[1] + mat[0][2] * from[2] + mat[0][3];
    ap1[1] = mat[1][0] * from[0] + mat[1][1] * from[1] + mat[1][2] * from[2] + mat[1][3];
    ap1[2] = mat[2][0] * from[0] + mat[2][1] * from[1] + mat[2][2] * from[2] + mat[2][3];

    to[0] = ap1[0]; to[1] = ap1[1]; to[2] = ap1[2];
}

