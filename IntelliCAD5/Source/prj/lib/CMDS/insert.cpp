/* F:\DEV\PRJ\LIB\CMDS\INSERT.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * Insert command
 * Minsert command
 *
 */
// THIS FILE HAS BEEN GLOBALIZED!
#include "cmds.h"/*DNT*/
#include "Icad.h"/*DNT*/
#include "db.h"/*DNT*/
#include "commandqueue.h" /*DNT*/
#include "dimstyletabrec.h"
#include "layertabrec.h"
#include "ltypetabrec.h"
#include "styletabrec.h"
#include "IcadApi.h"
#include "insert.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define dwg_entlink		db_handitem

extern char *cmd_InsertSavePath;
extern double SDS_CorrectElevation;
extern bool	  SDS_ElevationChanged;
extern int SDS_GetZScale;
extern int	  SDS_IgnoreMenuWQuote;

//** Prototypes
void cmd_OnOpenOK(HWND hDlg);
short cmd_ask4ColsAndRows(int *colcount, int *rowcount, sds_real *coldistance, sds_real *rowdistance);


short GetBlockName(char *defaultblkname,char *theblkname)
	{
// Pass "" as defaultblkname if no default block name

	ASSERT(defaultblkname);

	char blkprompt[IC_ACADBUF],blkname[IC_ACADBUF];
	RT ret;
	struct resbuf rb;

	theblkname[0]=blkname[0]=0;
	if (defaultblkname[0])
		sprintf(blkprompt,ResourceString(IDC_INSERT__N__TO_LIST_BLOCK_0, "\n? to list blocks in drawing/BROWSE/EXPLORE/<Block to insert> <%s>: " ),defaultblkname);
	else
		strcpy(blkprompt,ResourceString(IDC_INSERT__N__TO_LIST_BLOCK_1, "\n? to list blocks in drawing/BROWSE/EXPLORE/<Block to insert>: " ));

    //Get the block name.
	ret=sds_initget(RSG_OTHER,ResourceString(IDC_INSERT_INITGET_LIST_BLOC_2, "List_blocks|? ~ Browse...|BROWSE Explore...|EXPLORE ~_? ~_ ~_BROWSE ~_EXPLORE" ));
	if (ret!=RTNORM)
		goto bail;
	SDS_IgnoreMenuWQuote=1;
	if ((ret=sds_getstring(1,blkprompt,blkname))==RTERROR || ret==RTCAN)
		goto bail;
		
	if ((blkname[0]=='~'/*DNT*/) || (strisame(blkname,"BROWSE"/*DNT*/)) || (strisame(blkname,"_BROWSE"/*DNT*/)))    // If there is a tilde or "BROWSE" call getfiled().
		{
		if (RTNORM!=SDS_GetFiled(ResourceString(IDC_INSERT_INSERT_BLOCK_4, "Insert Block" ),
			cmd_InsertSavePath,ResourceString(IDC_INSERT_STANDARD_DRAWING__5, "Standard Drawing File|dwg,Drawing Exchange Format|dxf" ),
			512,&rb,MAKEINTRESOURCE(IDD_OPENSAVE_PREVIEW),cmd_OnOpenOK))
			{
			ret=RTERROR;
			goto bail;
			}
		if (rb.restype==RTSHORT && rb.resval.rint==1)
			{
			ret=RTERROR;
			goto bail;
			}

		strncpy(blkname,rb.resval.rstring,sizeof(blkname)-1);
		IC_FREE(rb.resval.rstring);
        }

	if (*blkname==0)
		{
		if (*defaultblkname)
			{
			strncpy(blkname,defaultblkname,sizeof(blkname)-1);
			ret=RTNORM;
			}
		else 
			{
			cmd_ErrorPrompt(CMD_ERR_INVALIDNAME,0);
			ret=RTCAN;
			}
		}

	strcpy(theblkname,blkname);
bail:
	SDS_IgnoreMenuWQuote=0;
	return ret;
	}




short ProcessInsertBlockName(char *inblkname,char *outblkname,char *outblkfile)
// split inblkname into outblkname and outblkfile, as appropriate
	{
	char blkname[IC_ACADBUF],blkfile[IC_ACADBUF];
	char *eqptr;
	RT ret=RTNORM;
	struct resbuf *tmprb,setgetrb;
	char spldrive[_MAX_DRIVE];
	char splpath[_MAX_DIR];
	char splfname[_MAX_FNAME];
	char splext[_MAX_EXT];
	//char *cptr; // No use..Modified by Mohan Nayak on 03 Sep 2004 Bugzilla 78869
	short filedia;

// there are three possibilities for the input name:
// 1) It is the name of a block in the drawing
// 2) It is the name of an external file, with or without path info
// 3) It is of the form block=file
// we check for the third one first.  If it is not of that form, we check to see if the
// block name as given is a defined block in the current drawing.  If so, it is of the first type.  If not, the second.
// Ultimately we want a block name, and a path, if any

	ASSERT(inblkname);

	blkname[0]=blkfile[0]=0;

	if (*inblkname)
		{
		strncpy(blkname,inblkname,sizeof(blkname)-1);
        }
	else
		{
		ret=RTERROR;
		goto bail;
		}



	blkfile[0]=0;
	// If there is an "=" get the new block name...
	if ((eqptr=strchr(blkname,'='/*DNT*/))!=NULL)
		{
		*eqptr=0;
		strncpy(blkfile,&eqptr[1],sizeof(blkfile)-1);	// and the file name
		}
	else
		{
		tmprb=sds_tblsearch("BLOCK"/*DNT*/,blkname,0);
		if (tmprb!=NULL)	// this block is present in the drawing, so not from a disk file
			{
			IC_RELRB(tmprb);
			}
		else				// the block is not present, so get its path and set the appropriate vbls
			{
			strcpy(blkfile,blkname);
			blkname[0]=0;	// indicate need to get block name from file name
			}
		}


	if (blkfile[0])
		{
		if (strrchr(blkfile,'\\'/*DNT*/))
			{
			db_astrncpy(&cmd_InsertSavePath,blkfile,strlen(blkfile)+1);		// save the default insert path
			*(strrchr(cmd_InsertSavePath,'\\'/*DNT*/)+1)=0;
			}

		ChangeSlashToBackslashAndRemovePipe(blkfile);

		_splitpath(blkfile,spldrive,splpath,splfname,splext);

		// Modified by Mohan Nayak on 03 Sep 2004 Bugzilla 78869 [
		/*
		if (splext[0])
		{
			// Check for a ".dwg" extension and get rid of it
			if ((cptr=strrchr(blkfile,'.'/*DNT* /))!=NULL) 	*cptr=0;
		}
		*/
		//..............Bugzilla 78869]

		if (!blkname[0])					// create the block name from the filename
			{
			strcpy(blkname,splfname);
			}
		}

// at this point we have a block name and a block file, if needed
// we can use the the "nullstringness" of blkfile to determine whether we are inserting from disk or not

// get value of filedia
    if (RTNORM!=SDS_getvar(NULL,DB_QFILEDIA,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))
		return (RTERROR);
	filedia=setgetrb.resval.rint;

// Check validity of block name.
	if (!ic_isvalidname(blkname))
		{
		if (filedia)
			{
			SDS_CMainWindow->m_strWndAction.Empty();
// this needs modification to give a suggested name for too-long (or containing spaces) xrefs (& block inserts?)
			ExecuteUIAction( ICAD_WNDACTION_INSNAME );
			if (SDS_CMainWindow->m_strWndAction.IsEmpty())
				{
				ret=RTERROR;
				goto bail;
				}
			strncpy(blkname,SDS_CMainWindow->m_strWndAction,SDS_CMainWindow->m_strWndAction.GetLength()+1);
			SDS_CMainWindow->m_strWndAction.Empty();
			}
		else
			{
			printf(ResourceString(IDC_INSERT_BLOCK_NAME_IS_INV_6, "Block name is invalid. Please use <block>=<filename>." ));
			ret=RTERROR;
			goto bail;
			}
		}

	strcpy(outblkname,blkname);
	strcpy(outblkfile,blkfile);
bail:
	return ret;
}


short FindTheBlockFile(char *inblkfile, char *outblkfile)
// search for the file indicated by inblkfile, give error message and return RTERROR if not found,
// otherwise return the path on which the file was found in outblkfile
	{
	char spldrive[IC_PATHSIZE];
	char splpath[IC_PATHSIZE],splfname[IC_PATHSIZE],splext[IC_PATHSIZE];
	char blkfile[IC_PATHSIZE];
	char blkfoundpath[IC_PATHSIZE];

	ASSERT(inblkfile);
	ASSERT(outblkfile);

	strcpy(blkfile,inblkfile);
	_splitpath(blkfile,spldrive,splpath,splfname,splext);
	if (splext[0]==0) ic_setext(blkfile,".dwg"/*DNT*/);
	
	if (RTERROR==sds_findfile(blkfile,blkfoundpath))
		{
		sds_printf(ResourceString(IDC_INSERT__N___COULD_NOT_FI_9, "\n-- Could not find %s %s. --" ),ResourceString(IDC_INSERT_FILE_10, "file" ), inblkfile);
		return RTERROR;
		}

	strcpy(outblkfile,blkfoundpath);
	return RTNORM;
	}


short LoadTheBlock(char *blkname, char *blkfile)
	{
	// Load the block, tracking the eed 1005 handles
	// so that they can be updated to the new handle
	// information later
	SDS_CURDWG->Make1005FixupTables();
	if (cmd_ReadAsBlock(blkfile,blkname,IC_INSMODE_INSERT,NULL)==RTERROR)
		{
		if (SDS_CURDWG->FixingUp1005s())
			SDS_CURDWG->Delete1005FixupTables();
			return(RTERROR);
		}
	else
		{
		if (SDS_CURDWG->FixingUp1005s())
			{
			SDS_CURDWG->Apply1005Fixups();
			SDS_CURDWG->Delete1005FixupTables();
			}
		}
	return RTNORM;
	}

bool PrintMode(void)
// NOTE : This function is also called by cmd_shape()
	{
	struct resbuf rb;
	short printit;

	// return value to indicate whether or not error msgs should be printed
	SDS_getvar(NULL,DB_QEXPERT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if (rb.resval.rint<3)
		{
		SDS_getvar(NULL,DB_QCMDECHO,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		printit=(CMD_CMDECHO_PRINTF(rb.resval.rint));
		}
	else printit=0;
	if (printit) return true;
	return false;
	}

short GetExtrusionAndRotation(sds_real *extru, sds_real *rotang, sds_real *rotbase)
// get extru.  Also rotang, and rotbase in ECS
// NOTE : This function is also called by cmd_shape()
	{
	struct resbuf setgetrb;
	sds_point pt1,tpt;
	struct resbuf rbucs,rbwcs,rbent;

	if (SDS_getvar(NULL,DB_QUCSXDIR,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
		return RTERROR;
	ic_ptcpy(pt1,setgetrb.resval.rpoint);
	if (SDS_getvar(NULL,DB_QUCSYDIR,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
		return RTERROR;
	ic_crossproduct(pt1,setgetrb.resval.rpoint,extru);
	ic_ptcpy(rbent.resval.rpoint,extru);
	rbent.restype=RT3DPOINT;
	rbucs.restype=rbwcs.restype=RTSHORT;
	rbucs.resval.rint=1;
	rbwcs.resval.rint=0;

	tpt[1]=tpt[2]=0.0;
	tpt[0]=1.0;
	sds_trans(tpt,&rbucs,&rbent,1,tpt);
	//tpt now represents x axis direction in insert's ecs - our default zero angle
	*rotbase=atan2(tpt[1],tpt[0]);
	ic_normang(rotbase,NULL);
	*rotang += *rotbase;
	return RTNORM;
	}

short MakeDragParlink(char *blkname,sds_real *extru, sds_real rotang, db_insert **ins)
// make a parlink for dragging.
	{
	db_insert *elp;
	elp=new db_insert(SDS_CURDWG);
	if (!elp)
		return RTERROR;
	elp->set_210(extru);
	elp->set_2(blkname,SDS_CURDWG);
	elp->set_50(rotang);
	*ins=elp;
	return RTNORM;
	}

short DoDrag(sds_name ename, double *pt,char *promptstr,double *pt1, short dragmode)
// NOTE : This function is also called by cmd_shape()
	{
	sds_point temppoint;
	temppoint[SDS_X]=*(sds_real *)ename;
	temppoint[SDS_Y]=temppoint[SDS_Z]=0.0;
	return SDS_dragobject(-1,dragmode,temppoint,pt,0.0,promptstr,NULL,pt1,NULL);
	}

// EBATECH(CNBR) 2002/4/24 This function is also called by cmd_shape()
//short GetOneScale(sds_real *scl, bool givetblsloadedmsg)
short GetOneScale(db_handitem *elp, sds_real *scl, bool givetblsloadedmsg)
	{
	RT ret;
	if (sds_initget(0,NULL)!=RTNORM)
		return RTERROR;

	// EBATECH(CNBR) -[ 2002/4/24 This function is also called by cmd_shape()
	char promptstr[IC_ACADBUF];
	if( elp->ret_type() == DB_SHAPE ){
		strcpy(promptstr,ResourceString(IDC_SHAPE_NSCALE_FACTOR, "\nScale factor for shape <1.0>: " ));
	}else{
		strcpy(promptstr,ResourceString(IDC_INSERT__NSCALE_FACTOR_F_32, "\nScale factor for block <1.0>: " ));
	}
	//if ((ret=sds_getdist(NULL,ResourceString(IDC_INSERT__NSCALE_FACTOR_F_32, "\nScale factor for block <1.0>: " ),scl))==RTERROR)
	if ((ret=sds_getdist(NULL,promptstr,scl))==RTERROR)
	// EBATECH(CNBR) ]-
		return ret;
	else if (ret==RTCAN)
		{
		if (givetblsloadedmsg)
			cmd_ErrorPrompt(CMD_ERR_INSERTDEF,-1);
		return ret;
		}
	else if (ret==RTNONE)
		*scl=1.0;
	return RTNORM;
	}

// EBATECH(CNBR) 2002/4/24 This function is also called by cmd_shape()
//short GetRotation(sds_name ename,db_insert *elp, sds_point pt1, sds_real rotbase, bool givetblsloadedmsg, sds_real rotang)
short GetRotation(sds_name ename, db_handitem *elp, sds_point pt1, sds_real rotbase, bool givetblsloadedmsg, sds_real rotang)
	{
	char rotstr[IC_ACADBUF],rotstr2[IC_ACADBUF];
	struct resbuf rb;
	RT ret;
	sds_point tpt;

	//*** Rotation angle.
	if (sds_initget(0,NULL)!=RTNORM)
	return RTERROR;

	if (sds_angtos(0.0,-1,-1,rotstr)!=RTNORM)
		return RTERROR;
	//don't prompt w/rotang, because user only wants angle in ucs
	// EBATECH(CNBR) -[ 2002/4/24 This function is also called by cmd_shape()
	if( elp->ret_type() == DB_SHAPE ){
		sprintf(rotstr2,ResourceString(IDC_SHAPE_NROTATION_ANGLE, "\nRotation angle for shape <%s>: " ),rotstr);
	}else{
		sprintf(rotstr2,ResourceString(IDC_INSERT__NROTATION_ANGLE_43, "\nRotation angle for block <%s>: " ),rotstr);
	}
	//sprintf(rotstr2,ResourceString(IDC_INSERT__NROTATION_ANGLE_43, "\nRotation angle for block <%s>: " ),rotstr);
	// EBATECH(CNBR) ]-
	SDS_getvar(NULL,DB_QDRAGMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	sds_point temppoint;
	temppoint[SDS_X]=*(sds_real *)ename;
	temppoint[SDS_Y]=temppoint[SDS_Z]=0.0;
	tpt[0]=tpt[1]=0.0;
	ret=SDS_dragobject(-3,rb.resval.rint,temppoint,(double *)elp->retp_50(),0.0,rotstr2,(struct resbuf **)pt1,tpt,NULL);

	if (ret==RTNORM)
		{
		rotang=tpt[0]+rotbase;
		ic_normang(&rotang,NULL);
		elp->set_50(rotang);
		}
	else if (ret==RTERROR)
		return RTERROR;
	else if (ret==RTCAN)
		{
  		if (givetblsloadedmsg) cmd_ErrorPrompt(CMD_ERR_INSERTDEF,-1);
           return RTCAN;
		}
	else if (ret==RTNONE)
		// Modified Cybage SBD 11/02/2002 DD/MM/YYYY [
		// Reason: Fix for bug no. 77974
		//elp->set_50(0.0);
		{
		sds_real ang=0.0;
		sds_angtof(rotstr,0,&ang);
		rotang=ang+rotbase;
		ic_normang(&rotang,NULL);
		elp->set_50(rotang);
		}
		// Modified Cybage SBD 11/02/2002 DD/MM/YYYY ]
	return RTNORM;
	}


short GetScales(db_insert *elp, sds_name ename, sds_point pt1,bool givetblsloadedmsg,FLAGS_BYTE flags=REQ_SCALE_X|REQ_SCALE_Y)
	{
	// Modified Cybage SBD 23/04/2001 dd/mm/yyyy
	// Reason: Mail from "Ronald Prepchuk" <ronp@intellicad.org> Dated 14/04/2001
	//char scalestr[IC_ACADBUF],/*scalestr2[IC_ACADBUF], */keyword[IC_ACADBUF];
	char scalestr[IC_ACADBUF],scalestr2[IC_ACADBUF],keyword[IC_ACADBUF];
	struct resbuf rb;
	RT ret;

	sds_real xscl,yscl,zscl;
	bool gotcorner=false;
	short dragmode;
	bool stayinloop;

	// Modified Cybage PP 02/02/2001 [
	// Reason: Fix for bug no. 72162.
	sds_point tpt = { 0 } ;
	BOOL bGotY = FALSE ;
	BOOL bGotZ = FALSE ;
	// Modified Cybage PP 02/02/2001 ]

	elp->get_41(&xscl);
	elp->get_42(&yscl);
	elp->get_43(&zscl);

	SDS_getvar(NULL,DB_QDRAGMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	dragmode=rb.resval.rint;

	// top loop -- get keywords or x scale factor.
	do 	{
		stayinloop=false;	// we exit the loop on anything but the "XYZ" ("X") choice
		if (flags & REQ_SCALE_Z) {
			if (sds_initget(SDS_RSG_NOCANCEL,ResourceString(IDC_INSERT_INITGET_CORNER_D_34, "Corner ~_Corner" ))!=RTNORM)
				return RTERROR;
			sprintf(scalestr,ResourceString(IDC_INSERT__NDONE_CORNER_X__35, "\nCorner/X scale factor <%f>: " ),xscl);
		}
		else{
			if (sds_initget(SDS_RSG_NOCANCEL,ResourceString(IDC_INSERT_INITGET_CORNER_S_36, "Corner Scale_X_Y_Z|XYZ ~ ~X ~_Corner ~_XYZ  ~_ ~_X" ))!=RTNORM)
				return RTERROR;
			sprintf(scalestr,ResourceString(IDC_INSERT__NDONE_CORNER_XY_37, "\nCorner/XYZ/X scale factor <%f>: " ),xscl);
		}

		sds_point temppoint;
		temppoint[SDS_X]=*(sds_real *)ename;
		temppoint[SDS_Y]=temppoint[SDS_Z]=0.0;
		ret=SDS_dragobject(-4,dragmode,temppoint,(double *)ename[0],0.0,scalestr,(struct resbuf **)pt1,tpt,NULL);

		if (ret==RTKWORD){
			if (sds_getinput(keyword)!=RTNORM)
				return RTERROR;
			if (strisame(keyword,"XYZ"/*DNT*/) || strisame(keyword,"X"/*DNT*/)){
				flags|=REQ_SCALE_Z;
				stayinloop=true;  // if we had an XYZ, stay in the loop, otherwise leave
			}
			else
			if (strisame(keyword,"CORNER"/*DNT*/)){
				if (sds_initget(0,NULL)!=RTNORM)
					return RTERROR;
				sprintf(scalestr,ResourceString(IDC_INSERT__NSELECT_OTHER_C_33, "\nSelect other corner for scale or enter X scale factor <%f>: " ),xscl);
				sds_point temppoint;
				temppoint[SDS_X]=*(sds_real *)ename;
				temppoint[SDS_Y]=temppoint[SDS_Z]=0.0;
				ret=SDS_dragobject(-4,dragmode,temppoint,(double *)ename[0],0.0,scalestr,(struct resbuf **)pt1,tpt,NULL);
				if (ret==RTCAN){
  					if (givetblsloadedmsg)
						cmd_ErrorPrompt(CMD_ERR_INSERTDEF,-1);
				return RTERROR;
				}
				else
				if (ret==RTERROR) return RTERROR;
				else
				if (ret==RTNORM){
					if (icadRealEqual(tpt[0],0.0) || icadRealEqual(tpt[1],0.0)){
						sds_printf(ResourceString(IDC_INSERT__NCORNER_SPECIFI_38, "\nCorner specification requires that delta X and delta Y be nonzero." ));
						stayinloop=true;	// stay on a failed corner get also
					}
					xscl=/*zscl=*/tpt[0];	/*D.Gavrilov*///don't change zscl
					yscl=tpt[1];
					elp->set_scales(xscl,yscl,zscl);
					gotcorner=true;
				}
			}
			else stayinloop=true;	// garbage string input
		}
	} while (stayinloop);

	if (!gotcorner){
		if (ret==RTERROR) return RTERROR;
		else
		if (ret==RTCAN){
  			if (givetblsloadedmsg)
				cmd_ErrorPrompt(CMD_ERR_INSERTDEF,-1);
		return RTCAN;
		}
		else
		if (ret==RTSTR || ret==RTNORM || ret==RTNONE){
			if (ret==RTSTR){
				xscl=tpt[0];
				yscl=tpt[1];
//				zscl=tpt[2];	/*D.Gavrilov*///don't change zscl
								//(we can change yscl because it will
								// be asked for later and confirmed
								// or changed, but zscl may will not
								// be asked for)
				elp->set_scales(xscl,yscl,zscl);
			}
			else
			if (ret==RTNORM){
				// Modified Cybage PP 02/02/2001
				// Reason: Fix for bug no. 72162.
				if( tpt[1] ) bGotY = TRUE ;

				xscl=/*zscl=*/tpt[0];	/*D.Gavrilov*///don't change zscl
				yscl=tpt[1];
			}
			// for the none case, xscl just stays what it was
			// now get y scale
			elp->set_scales(xscl,yscl,zscl);

			//*** Get the Y scale factor.
			// Modified Cybage PP 02/02/2001
			// Reason: Fix for bug no. 72162.
			if( !bGotY )
			{
			yscl=/*zscl=*/xscl;		/*D.Gavrilov*///don't change zscl
			sprintf(scalestr,ResourceString(IDC_INSERT__NY_SCALE_FACTOR_41, "\nY scale factor:  < Equal to X scale (%f)>: " ),yscl);
			sds_initget(0,NULL);
			sds_point temppoint;
			temppoint[SDS_X]=*(sds_real *)ename;
			temppoint[SDS_Y]=temppoint[SDS_Z]=0.0;
			ret=SDS_dragobject(-2,rb.resval.rint,temppoint,
					 (double *)elp->retp_42(),
					 0.0,scalestr,(struct resbuf **)pt1,tpt,NULL);
			if (ret==RTNORM) {
				elp->set_42(tpt[0]);
				yscl=tpt[0];
			}
			else
			if (ret==RTERROR) return RTERROR;
			else
			if (ret==RTCAN){
  				if (givetblsloadedmsg)
					cmd_ErrorPrompt(CMD_ERR_INSERTDEF,-1);
			return RTCAN;
			}
			else if (ret==RTNONE)
				elp->set_42(yscl);
			}
			}
		}

	if ( (flags & REQ_SCALE_Z) && !bGotZ ){
		//*** Get the Z scale factor.
		SDS_GetZScale=1;
		zscl=xscl;
		sprintf(scalestr2,ResourceString(IDC_INSERT__NX_SCALE_IS__S__42, "\nX scale is %f, Y scale is %f.  Z scale factor:  <Equal to X scale (%f)>: " ),xscl,yscl,zscl);
		SDS_getvar(NULL,DB_QDRAGMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
        sds_initget(0,NULL);
		sds_point temppoint;
		temppoint[SDS_X]=*(sds_real *)ename;
		temppoint[SDS_Y]=temppoint[SDS_Z]=0.0;
		ret=SDS_dragobject(-2,rb.resval.rint,temppoint,
				 (double *)elp->retp_43(),
				 0.0,scalestr2,(struct resbuf **)pt1,tpt,NULL);
		SDS_GetZScale=0;

		if (ret==RTNORM)
			zscl=tpt[0];
		else if (ret==RTERROR)
			return RTERROR;
		else if (ret==RTCAN){
  			if (givetblsloadedmsg)
				cmd_ErrorPrompt(CMD_ERR_INSERTDEF,-1);
			return RTCAN;
		}
		else if (ret==RTNONE)
			elp->set_43(zscl);
	}
	elp->set_scales(xscl,yscl,zscl);
return RTNORM;
}


void SetInitgetAndPromptStrs(bool givemultiprompt, char *initgetstr, char *promptstr)
	{
	if (!givemultiprompt)
		{
		strcpy(initgetstr,ResourceString(INSERT_INITGET_1,"Scale X_Scale|Xscale Y_Scale|Yscale Z_Scale|Zscale Rotate  ~ ~PScale ~PXscale ~PYscale ~PZscale ~PRotate ~ ~_Scale ~_Xscale ~_Yscale ~_Zscale ~_Rotate ~_ ~_PScale ~_PXscale ~_PYscale ~_PZscale ~_PRotate ~_"));
		strcpy(promptstr,ResourceString(IDC_INSERT__NINSERTION_POIN_12, "\nInsertion point for block: " ));
		}
	else
		{
		strcpy(initgetstr,ResourceString(INSERT_INITGET_2,"Scale X_Scale|Xscale Y_Scale|Yscale Z_Scale|Zscale Rotate ~PScale ~PXscale ~PYscale ~PZscale ~PRotate ~ Multiple_blocks|Multiple ~_Scale ~_Xscale ~_Yscale ~_Zscale ~_Rotate ~_PScale ~_PXscale ~_PYscale ~_PZscale ~_PRotate ~_ ~_Multiple"));
		strcpy(promptstr,ResourceString(IDC_INSERT__NMULTIPLE_BLOCK_13, "\nMultiple blocks/<Insertion point for block>: " ));
		}
	}

short GetInsertionPoint(bool *multiIns,sds_point pt1,sds_name ename,db_insert *elp,bool draginspt,bool *needscales, bool *needrotation,
						sds_real *xscl, sds_real *yscl, sds_real *zscl, sds_real *rotang)
	{
	char initgetstr[IC_ACADBUF],promptstr[IC_ACADBUF];
	struct resbuf rb;
	short holdorthomode;
	RT ret;
	bool givemultiprompt;
	char kword[IC_ACADBUF];

	do
		{
		if (multiIns==NULL) // if this is passed in as NULL, multiple insert is not allowed
			givemultiprompt=false;
		else givemultiprompt=!(*multiIns);	// otherwise we do ask for multiple if we are not
											// currently DOING multiple, we don't if we're not

		SetInitgetAndPromptStrs(givemultiprompt,initgetstr,promptstr);

		if (sds_initget(RSG_NONULL+SDS_RSG_NOCANCEL,initgetstr)!=RTNORM)
			return RTERROR;
		if (!draginspt)
			{
			ret=sds_getpoint(NULL,promptstr,pt1); // need to look at this
			return ret;
			}
		else
			{
			SDS_getvar(NULL,DB_QORTHOMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);   // turn ortho off, save what it was
			holdorthomode=rb.resval.rint;
			if (holdorthomode!=0)
				{
				rb.resval.rint=0;
				sds_setvar("ORTHOMODE"/*DNT*/,&rb);
				}
			SDS_getvar(NULL,DB_QDRAGMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			extern bool SDS_ApplyNoOrtho;
			SDS_ApplyNoOrtho=TRUE;

			ret=DoDrag(ename,(double *)elp->retp_10(),promptstr,pt1,rb.resval.rint);					// do the drag

			SDS_ApplyNoOrtho=FALSE;
			if (holdorthomode)																// if ortho was on, turn it back on
				{
				rb.resval.rint=1;
				sds_setvar("ORTHOMODE"/*DNT*/,&rb);
				}
			}
		if (ret==RTKWORD)  // process the hidden keywords
			{
			if (sds_getinput(kword)!=RTNORM)
				return RTERROR;
			strupr(kword);
			if (sds_initget(RSG_NONULL,NULL)!=RTNORM)
				return RTERROR;
			if (strsame(kword,"SCALE"/*DNT*/) || strsame(kword,"PSCALE"/*DNT*/))
				{
				if (sds_getdist(NULL,ResourceString(IDC_INSERT__NSCALE_FACTOR_F_17, "\nScale factor for block: " ),xscl)==RTERROR)
					return RTERROR;
				*zscl = *yscl = *xscl;
				if (strsame(kword,"SCALE"/*DNT*/))
					*needscales=false;
				}
			else if (strsame(kword,"XSCALE"/*DNT*/) || strsame(kword,"PXSCALE"/*DNT*/))
				{
				if (sds_getdist(NULL,ResourceString(IDC_INSERT__NX_SCALE_FACTOR_20, "\nX scale factor for block: " ),xscl)==RTERROR)
					return RTERROR;
				if (strsame(kword,"XSCALE"/*DNT*/))
					*needscales=false;
				}
			else if (strsame(kword,"YSCALE"/*DNT*/) || strsame(kword,"PYSCALE"/*DNT*/))
				{
				if (sds_getdist(NULL,ResourceString(IDC_INSERT__NY_SCALE_FACTOR_23, "\nY scale factor for block: " ),yscl)==RTERROR)
					return RTERROR;
				if (strsame(kword,"YSCALE"/*DNT*/))
					*needscales=false;
				}
			else if (strsame(kword,"ZSCALE"/*DNT*/) || strsame(kword,"PZSCALE"/*DNT*/))
				{
				if (sds_getdist(NULL,ResourceString(IDC_INSERT__NZ_SCALE_FACTOR_26, "\nZ scale factor for block: " ),zscl)==RTERROR)
					return RTERROR;
				if (strsame(kword,"ZSCALE"/*DNT*/))
					*needscales=false;
				}
			else if (strsame(kword,"ROTATE"/*DNT*/) || strsame(kword,"PROTATE"/*DNT*/))
				{
				if (sds_getangle(NULL,ResourceString(IDC_INSERT__NROTATION_ANGLE_29, "\nRotation angle for block: " ),rotang)==RTERROR)
					return RTERROR;
				if (strsame(kword,"ROTATE"/*DNT*/))
					*needrotation=false;
				}
			else if (strsame(kword,"MULTIPLE"/*DNT*/))
				*multiIns=true;
			elp->set_41(*xscl);
			elp->set_42(*yscl);
			elp->set_43(*zscl);
			elp->set_50(*rotang);
			}
		}
	while (ret==RTKWORD);

	return ret;
	}



short GetInsertParameters(db_insert *elp,
						  bool minsert,short ask4onescl,
						  bool draginspt,bool givetblsloadedmsg,
						  sds_real rotbase,sds_real *curelev,
						  sds_real rotang,sds_point retinspt,
						  bool *multiIns,FLAGS_BYTE flags/*=0xFF*/){
	sds_name ename;
	sds_point pt1;
	RT ret = RTNORM;
	struct resbuf rbent;
	struct resbuf rb;
	sds_real xscl,yscl,zscl;
	struct resbuf rbucs,rbwcs;
	sds_point tpt;
	struct resbuf setgetrb;
//	bool needscales		=	(0!= (flags & (REQ_SCALES | REQ_EQUAL_SCALE)))
//		,needrotation	=	(0!= (flags & REQ_ANGLE));
	bool needscales		=	INT2BOOL(flags & (REQ_SCALES | REQ_EQUAL_SCALE))
		,needrotation	=	INT2BOOL(flags & REQ_ANGLE);

	rbucs.restype=rbwcs.restype=RTSHORT;
	rbucs.resval.rint=1;
	rbwcs.resval.rint=0;
	rbent.restype=RT3DPOINT;
	elp->get_210(rbent.resval.rpoint);

	pt1[0]=pt1[1]=pt1[2]=0.0;
	ename[0]=(long)elp;
	ename[1]=(long)SDS_CURDWG;
	xscl=*(elp->retp_41());
	yscl=*(elp->retp_42());
	zscl=*(elp->retp_43());
	elp->get_50(&rotang);

	// Get parameters
	if (flags & REQ_POINT)
		for (ret=RTERROR;ret!=RTNORM;){
			ret=GetInsertionPoint(multiIns,pt1,ename,elp,draginspt,&needscales,&needrotation,&xscl,&yscl,&zscl,&rotang);
			if (ret==RTERROR)	return RTERROR;
			else
			if (ret==RTCAN) {
				SDS_getvar(NULL,DB_QCMDECHO,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				if (givetblsloadedmsg) cmd_ErrorPrompt(CMD_ERR_INSERTDEF,-1);
			return RTCAN;
			}
		} // end of for (;;)
	else elp->get_10(pt1);

	ic_ptcpy(retinspt,pt1);

	extern sds_point SDS_PointDisplay;
	ic_ptcpy(SDS_PointDisplay,pt1);

	// Update the parlink so the dragging is displayed correctly.
	sds_trans(pt1,&rbucs,&rbent,0,tpt);
	// pt1 will be converted on dragging loop below.
	elp->set_10(tpt);

	//get current elevation
	SDS_getvar(NULL,DB_QELEVATION,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	*curelev=setgetrb.resval.rreal;

	if (!icadRealEqual(pt1[2],*curelev)){
		//temporarily set elevation so angle/scale dragging is correct
		SDS_CorrectElevation=*curelev;
		SDS_ElevationChanged=TRUE;
		setgetrb.resval.rreal=pt1[2];
		setgetrb.restype=RTREAL;
		sds_setvar("ELEVATION"/*DNT*/,&setgetrb);
	}
	// Set LASTPOINT system variable, even if inserting from dialog
	setgetrb.restype=RT3DPOINT;
	ic_ptcpy(setgetrb.resval.rpoint,pt1);
	//SDS_setvar(NULL,DB_QLASTPOINT,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);

	if (needscales)	{
		if (ask4onescl){
			//if (GetOneScale(&xscl,givetblsloadedmsg)!=RTNORM) // EBATECH(CNBR) 2002/4/24 Use both SHAPE and INSERT
			if (GetOneScale( elp, &xscl, givetblsloadedmsg)!=RTNORM)
				return RTERROR;
	        /*BUGBEGIN 	#BRX_1084
	        yscl=zscl=xscl;
			BUGEND	#BRX_1084
			*/
			//#BRX_1084 FIXED
			//FIXED:
			elp->set_scales(xscl,xscl,xscl);
			//END of FIXED #BRX_1084
		}
		else{
			// Modified Cybage SBD 23/04/2001 dd/mm/yyyy
			// Reason: Mail from "Ronald Prepchuk" <ronp@intellicad.org> Dated 14/04/2001
			//if (GetScales(elp,ename,pt1,givetblsloadedmsg,flags)!=RTNORM)
			if (GetScales(elp,ename,pt1,givetblsloadedmsg)!=RTNORM)
				return RTERROR;
		}
	}
	if (needrotation)
		ret=GetRotation(ename,elp,pt1,rotbase,givetblsloadedmsg,rotang);
	if (ret!=RTNORM) return ret;
  	SDS_setvar(NULL,DB_QLASTPOINT,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
	    //*** Column count, row count, column spacing, and row spacing.
    if (minsert){
		int colct,rowct;
        sds_real coldist,rowdist;
		if ((ret=cmd_ask4ColsAndRows(&colct,&rowct,&coldist,&rowdist))!=RTNORM)
			return ret;
		elp->set_70(colct);
		elp->set_71(rowct);
		elp->set_44(coldist);
		elp->set_45(rowdist);
	}

return RTNORM;
}


short InsertExplodedBlock(char *blkname,struct resbuf *elist,db_insert *ins,bool fromdisk,sds_point pt1in)
	{
	struct resbuf *rbptr,rb;
	sds_name ename;
	short psflag;
	sds_point blkinspt;
	RT rc;
	sds_real xscl,yscl,zscl,rotang;
	sds_point extru,pt1;

	ins->get_41(&xscl);
	ins->get_42(&yscl);
	ins->get_43(&zscl);
	ins->get_50(&rotang);
	ins->get_210(extru);

	struct resbuf rbent,rbucs;
	rbent.restype=RT3DPOINT;
	ic_ptcpy(rbent.resval.rpoint,extru);
	rbucs.restype=RTSHORT;
	rbucs.resval.rint=1;
	sds_trans(pt1in,&rbucs,&rbent,0,pt1);	// transform ins pt -- fixes this for non parallel UCS

	//*** Make separate entities instead of an insert.
    if ((rbptr=ic_ret_assoc(elist,-2))==NULL)
		{
		return RTERROR;
		}
    ic_encpy(ename,rbptr->resval.rlname);
    if ((rbptr=ic_ret_assoc(elist,10))==NULL)
		{
		return RTERROR;
		}
    ic_ptcpy(blkinspt,rbptr->resval.rpoint);

	psflag=0;
	SDS_getvar(NULL,DB_QTILEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if (rb.resval.rint==0)
		{
		SDS_getvar(NULL,DB_QCVPORT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		if (rb.resval.rint==1)
			psflag=1;
		}

		// track the eed 1005 handles so that they can be updated
		// to the new handle information later
	SDS_CURDWG->Make1005FixupTables();

    do
		{
		//note: rbinherit is null because we don't want to inherit any of the parent ent's properties
		resbuf *subEntities = NULL;
// NOTE -- change return from cmd_explode_makesubents to short
        rc = (short)cmd_explode_makesubents(ename,pt1,blkinspt,xscl,yscl,zscl,rotang,extru,psflag,NULL,&subEntities);
		if (rc<0)
			{
			if (SDS_CURDWG->FixingUp1005s())
				SDS_CURDWG->Delete1005FixupTables();

			return RTERROR;
			}
		rc=RTNORM;
		if (subEntities)
			{
				rc = sds_entmake(subEntities); // Make it
			}

		IC_RELRB(subEntities);

		if (rc != RTNORM)
			{
			if (SDS_CURDWG->FixingUp1005s())
				SDS_CURDWG->Delete1005FixupTables();

			return RTERROR;
			}

		}
	while(sds_entnext_noxref(ename,ename)==RTNORM);

	if (SDS_CURDWG->FixingUp1005s())
		{
		SDS_CURDWG->Apply1005Fixups();
		SDS_CURDWG->Delete1005FixupTables();
		}

	// Because this insert was made with the '*' We need to remove it from the blocks table.-- only if it was from a file
	// do not remove if an xref either
	if (fromdisk)
		{
		db_handitem *hip;
		hip=SDS_CURDWG->tblsearch(DB_BLOCKTAB,blkname,1);
		if (!(((db_blocktabrec *)hip)->ret_flags() & IC_BLOCK_XREF))
			{
			ename[0]=(long)hip;
			ename[1]=(long)SDS_CURDWG;
			sds_entdel(ename);
			}
		}
		return rc;
	}

short ProcessAttributes(struct resbuf *elist,bool dodialogs,db_insert *ins,sds_real rotbase)
	{
	RT rc;
	struct resbuf *rbptr,rb,rbucs;
	struct resbuf *newent,*tmprb,*curattlst,*tmpattlst,*begattlst=NULL;
	sds_point attdefextru,tpt,tpt2,tpt3,pt2,attribextru,blkinspt;
	sds_name ename,firstent;
	short initflg, allconstantattribs;
	char promptstr[IC_ACADBUF],fs1[IC_ACADBUF],fs2[IC_ACADBUF];
	RT ret;
	sds_real xscl,yscl,zscl,rotang,oldrotang;
	sds_point extru;
	struct resbuf rbent;
	sds_point pt1;
	int entmakeret;

	rbucs.restype=RTSHORT;
	rbucs.resval.rint=1;

	ins->get_10(pt1);
	ins->get_41(&xscl);
	ins->get_42(&yscl);
	ins->get_43(&zscl);
	ins->get_50(&rotang);
	ins->get_210(extru);
	rbent.restype=RT3DPOINT;
	ic_ptcpy(rbent.resval.rpoint,extru);

	SDS_getvar(NULL,DB_QATTDIA,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	int attdia=rb.resval.rint;
   	SDS_getvar(NULL,DB_QATTREQ,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	int attreq=rb.resval.rint;

	//if inserting from ddinsert, don't prompt for atts, and call up dialog
	if (dodialogs)
		{
		attreq=0;
		attdia=1;
		}

	tmprb=NULL;
	//*** Get the ATTDEFs from the BLOCK and make the ATTRIBs in the INSERT
	if ((rbptr=ic_ret_assoc(elist,-2))==NULL)
		{
		rc=(-__LINE__);
		goto bail;
		}
	ic_encpy(ename,rbptr->resval.rlname);
	ic_encpy(firstent,ename);
	if ((rbptr=ic_ret_assoc(elist,10))==NULL)
		{
		rc=(-__LINE__);
		goto bail;
		}
	ic_ptcpy(blkinspt,rbptr->resval.rpoint);
	initflg=0;
	IC_FREEIT
	//*** Loop through sub-entities to get attribute values from user
	do
		{
		//*** This loop creates a linked list of attribute values only.  The next do loop
		//*** checks the verify flag in the ATTDEF and then creates the ATTRIB
		if ((elist=sds_entget(ename))==NULL)
			{
			cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
			rc=(-__LINE__);
			goto bail;
			}
		if ((rbptr=ic_ret_assoc(elist,0))==NULL)
			{
			rc=(-__LINE__);
			goto bail;
			}
		if (strsame("ENDBLK"/*DNT*/,rbptr->resval.rstring)) break;
		else if (strsame("ATTDEF"/*DNT*/,rbptr->resval.rstring))
			{
			//*** Get attribute value
			if ((rbptr=ic_ret_assoc(elist,70))==NULL)
				{
				rc=(-__LINE__);
				goto bail;
				}
			int flags=rbptr->resval.rint;
			if (!(flags & IC_ATTDEF_CONST))
				{
// indicate non-const attribute found.  Further on, we don't put up a dialog if no non-consts were found
				allconstantattribs=0;
				if ((rbptr=ic_ret_assoc(elist,3))==NULL)
					{
					rc=(-__LINE__);
					goto bail;
					}
				if (strsame(rbptr->resval.rstring,""/*DNT*/))
					{
					if ((rbptr=ic_ret_assoc(elist,2))==NULL)
						{
						rc=(-__LINE__);
						goto bail;
						}
					}
				strncpy(promptstr,rbptr->resval.rstring,sizeof(promptstr)-1);
				if ((rbptr=ic_ret_assoc(elist,1))==NULL)
					{
					rc=(-__LINE__);
					goto bail;
					}
				tmprb=rbptr;
				strncpy(fs1,tmprb->resval.rstring,sizeof(fs1)-1);
				if ((flags & IC_ATTDEF_PRESET) || attdia==1 || attreq==0)
					{
					strncpy(fs1,tmprb->resval.rstring,sizeof(fs1)-1);
					}
				else
					{
					if (*fs1) sprintf(fs2,"\n%s <%s>: "/*DNT*/,promptstr,fs1);
					else sprintf(fs2,"\n%s: "/*DNT*/,promptstr);
					if ((ret=sds_getstring(1,fs2,fs1))==RTERROR)
						{
						rc=(-__LINE__);
						goto bail;
						}
					else if (ret==RTCAN)
						{
						rc=(-1L);
						goto bail;
						}
					if (!*fs1) strncpy(fs1,tmprb->resval.rstring,sizeof(fs1)-1);
					}
				if ((tmpattlst=sds_buildlist(RTSTR,fs1,0))==NULL)
					{
					cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
					rc=(-__LINE__); goto bail;
					}
				if (begattlst==NULL)
					{
					begattlst=curattlst=tmpattlst;
					}
				else
					{
					curattlst=curattlst->rbnext=tmpattlst;
					}
				tmprb=NULL;
				}
			}
			IC_FREEIT
		}
	while(sds_entnext_noxref(ename,ename)==RTNORM);
	//*** Loop through sub-entities again to verify attribute values and create
	curattlst=begattlst;
	ic_encpy(ename,firstent);
	do
		{
		if ((elist=sds_entget(ename))==NULL)
			{
			cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
			rc=(-__LINE__); goto bail;
			}
		if ((rbptr=ic_ret_assoc(elist,0))==NULL)
			{
			rc=(-__LINE__);
			goto bail;
			}
		if (strsame("ENDBLK"/*DNT*/,rbptr->resval.rstring)) break;
		else if (strsame("ATTDEF"/*DNT*/,rbptr->resval.rstring))
			{
			//*** Instead of building a new entity list for the ATTRIB I modify
			//*** the elist of the ATTDEF and then entmake() that modified elist.
			strncpy(rbptr->resval.rstring,"ATTRIB"/*DNT*/,strlen(rbptr->resval.rstring));
			if ((rbptr=ic_ret_assoc(elist,70))==NULL)
				{
				rc=(-__LINE__);
				goto bail; }
			int flags=rbptr->resval.rint;
			if (!(flags & IC_ATTDEF_CONST))
				//*** Delete the 3 group from the elist - not needed for an ATTRIB
				{
				struct resbuf *cur,*prev;

				for (prev=cur=elist; cur!=NULL && cur->restype!=3; prev=cur,cur=cur->rbnext);
				if (cur!=NULL) {
					strncpy(promptstr,cur->resval.rstring,sizeof(promptstr)-1);
					prev->rbnext=cur->rbnext;
					cur->rbnext=NULL;
					IC_RELRB(cur);
				}
				//deal w/ extru direction
				if ((rbptr=ic_ret_assoc(elist,210))==NULL)
					{
					for (tmprb=elist;tmprb->rbnext!=NULL;tmprb=tmprb->rbnext);
					if (NULL==(tmprb->rbnext=sds_buildlist(210,extru,0))){rc=(-__LINE__); goto bail;}
					attdefextru[0]=0.0;//!=extru[0]?
					attdefextru[1]=0.0;
					attdefextru[2]=1.0;
					}
				else
					{
					//**calculate new extrusion vector for attrib***
					ic_ptcpy(attdefextru,rbptr->resval.rpoint);
					tpt[0]=attdefextru[0]/xscl;
					tpt[1]=attdefextru[1]/yscl;
					tpt[2]=attdefextru[2]/zscl;
					//normalize the result
					tpt2[0]=sqrt(tpt[0]*tpt[0]+tpt[1]*tpt[1]+tpt[2]*tpt[2]);
					tpt[0]/=tpt2[0];
					tpt[1]/=tpt2[0];
					tpt[2]/=tpt2[0];
					//rotate only by that portion of rotang requested by user.
					//	subtract off rotation needed to align ucs systems
					tpt2[0]=tpt[0]*cos(rotang-rotbase)-tpt[1]*sin(rotang-rotbase);
					tpt2[1]=tpt[1]*cos(rotang-rotbase)+tpt[0]*sin(rotang-rotbase);
					tpt[0]=tpt2[0];tpt[1]=tpt2[1];
					//convert result back into wcs coordinates
					rb.restype=RTSHORT;rb.resval.rint=0;
					sds_trans(tpt,&rbucs,&rb,1,rbptr->resval.rpoint);
					ic_ptcpy(attribextru,rbptr->resval.rpoint);
					}
				//*** Text start point
				for (tmprb=elist;tmprb!=NULL;tmprb=tmprb->rbnext)
					{
					if (10==tmprb->restype || 11==tmprb->restype)
						{
//						sds_trans(pt1,&rbucs,&rbent,0,tpt2);	/*D.G.*/
 						ic_ptcpy(tpt2, pt1);					// We need pt1 being in WCS.
						ic_ptcpy(pt2,tmprb->resval.rpoint);
						rb.restype=RT3DPOINT;
						ic_ptcpy(rb.resval.rpoint,attdefextru);
						rbucs.resval.rint=0;//change rbucs to wcs resbuf temporarily
						sds_trans(pt2,&rb,&rbucs,0,pt2);//transform pt2 into block space (WCS)
						pt2[0]-=blkinspt[0];
						pt2[1]-=blkinspt[1];
						pt2[2]-=blkinspt[2];
						pt2[0]*=xscl; pt2[1]*=yscl; pt2[2]*=zscl;
						tpt[0]=pt2[0];tpt[1]=pt2[1];
						pt2[0]=tpt[0]*cos(rotang)-tpt[1]*sin(rotang);
						pt2[1]=tpt[1]*cos(rotang)+tpt[0]*sin(rotang);
						pt2[0]+=tpt2[0];
						pt2[1]+=tpt2[1];
						pt2[2]+=tpt2[2];
						ic_ptcpy(rb.resval.rpoint,attribextru);
						sds_trans(pt2,&rbent,&rb,0,tmprb->resval.rpoint);
						rbucs.resval.rint=1;//change rbucs back to ucs resbuf
						}
					else if (50==tmprb->restype)
						{
						//sds_trans(pt1,&rbucs,&rbent,0,tpt2);
						oldrotang=tmprb->resval.rreal;
						tpt[2]=0.0;
						tpt[0]=cos(tmprb->resval.rreal);
						tpt[1]=sin(tmprb->resval.rreal);
						tpt[2]=0.0;
						rb.restype=RT3DPOINT;
						ic_ptcpy(rb.resval.rpoint,attdefextru);
						rbucs.resval.rint=0;	//change rbucs to wcs resbuf temporarily
						sds_trans(tpt,&rb,&rbucs,1,tpt2);//transform pt2 into block space (WCS)
						//we'll save the attdef's angle relative to the block system
						//oldrotang=atan2(tpt2[1],tpt2[0]);
						rbucs.resval.rint=1;
						tpt2[0]*=xscl;
						tpt2[1]*=yscl;
						tpt2[2]*=zscl;
						//rotate it by rotang...
						tpt[0]=tpt2[0]*cos(rotang)-tpt2[1]*sin(rotang);
						tpt[1]=tpt2[1]*cos(rotang)+tpt2[0]*sin(rotang);
						tpt[2]=tpt2[2];
						ic_ptcpy(rb.resval.rpoint,attribextru);
						sds_trans(tpt,&rbent,&rb,1,tpt2);
						tmprb->resval.rreal=atan2(tpt2[1],tpt2[0]);
						}
					else if (40==tmprb->restype)
						{
						//*** Text height
						tpt3[0]=tmprb->resval.rreal;
						}
					else if (41==tmprb->restype)
						{
						//*** Relative X scl fact
						tpt3[1]=tmprb->resval.rreal;
						}
					else if (51==tmprb->restype)
						{
						//*** Oblique angle
						tpt3[2]=tmprb->resval.rreal;
						}
					else if (71==tmprb->restype)
						{
						if (yscl<0.0)
							{
							if (tmprb->resval.rint&4)tmprb->resval.rint&=~4;
							else tmprb->resval.rint|=4;
							}
			            }
					} // end of for

				//tpt3 now contains the info we'll need for height, width, & oblique angle
				//let tpt represent x axis for text entity
				tpt[0]=tpt3[1]*tpt3[0];
				tpt[1]=tpt[2]=0.0;
				//let tpt2 represent left edge of text entity - y axis
				tpt2[0]=tpt3[0]*tan(tpt3[2]);
				tpt2[1]=tpt3[0];
				tpt2[2]=0.0;
				//now apply original rotation
				tpt3[0]=tpt[0];tpt3[1]=tpt[1];
				tpt[0]=tpt3[0]*cos(oldrotang)-tpt3[1]*sin(oldrotang);
				tpt[1]=tpt3[1]*cos(oldrotang)+tpt3[0]*sin(oldrotang);
				tpt3[0]=tpt2[0];tpt3[1]=tpt2[1];
				tpt2[0]=tpt3[0]*cos(oldrotang)-tpt3[1]*sin(oldrotang);
				tpt2[1]=tpt3[1]*cos(oldrotang)+tpt3[0]*sin(oldrotang);
				//now transform tpt and tpt2
				rb.restype=RT3DPOINT;
				ic_ptcpy(rb.resval.rpoint,attdefextru);
				rbucs.resval.rint=0;	//change rbucs to wcs resbuf temporarily
				sds_trans(tpt,&rb,&rbucs,1,tpt);//transform tpt into block space (WCS)
				sds_trans(tpt2,&rb,&rbucs,1,tpt2);
				rbucs.resval.rint=1;

				tpt[0]*=xscl;
				tpt[1]*=yscl;
				tpt[2]*=zscl;
				tpt2[0]*=xscl;
				tpt2[1]*=yscl;
				tpt2[2]*=zscl;

				ic_ptcpy(rb.resval.rpoint,attribextru);
				sds_trans(tpt,&rbent,&rb,1,tpt);
				sds_trans(tpt2,&rbent,&rb,1,tpt2);
				//assume vectors lie flat in new plane, so their z's are 0.0
				tpt3[0]=sqrt(tpt[0]*tpt[0]+tpt[1]*tpt[1]);//+tpt[2]*tpt[2]	//new length of x unit vector
				tpt3[1]=sqrt(tpt2[0]*tpt2[0]+tpt2[1]*tpt2[1]);//+tpt2[2]*tpt2[2]	//new length of y unit vector
				tpt3[2]=(tpt[0]*tpt2[0]+tpt[1]*tpt2[1])/(tpt3[0]*tpt3[1]);	//dot product to yield cosine
				//tpt3[2]is cos of angle between the 2 vectors
				if (fabs(tpt3[2])<1.0 && tpt3[0]>CMD_FUZZ && tpt3[1]>CMD_FUZZ)
					{
					//reset the ht, xscl & obl ang
					if (tpt3[2]>1.0)
						tpt3[2]=1.0;
					else if (tpt3[2]<-1.0)
						tpt3[2]=-1.0;
					tpt3[2]=acos(tpt3[2]);	//angle between vectors (90-oblique ang)
					for (tmprb=elist;tmprb!=NULL;tmprb=tmprb->rbnext)
						{
						if (tmprb->restype==40)
							{
							//height
							tmprb->resval.rreal=tpt3[1]*sin(tpt3[2]);
							}
						else if (tmprb->restype==41)
							{
							//relative x scale factor
							tmprb->resval.rreal=tpt3[0]/(tpt3[1]*sin(tpt3[2]));
							}
						else if (tmprb->restype==51)
							{
							//oblique angle
							tmprb->resval.rreal=(0.5*IC_PI)-tpt3[2];
							}
						}
					}

				//*** Get attribute value
				if ((rbptr=ic_ret_assoc(elist,1))==NULL)
					{
					rc=(-__LINE__);
					goto bail;
					}
				tmprb=rbptr;
				strncpy(fs1,curattlst->resval.rstring,sizeof(fs1)-1);
				if ((rbptr=ic_ret_assoc(elist,70))!=NULL &&
					(rbptr->resval.rint & IC_ATTRIB_VERIFY) &&
					!(rbptr->resval.rint & IC_ATTRIB_CONST) &&
					!(rbptr->resval.rint & IC_ATTRIB_PRESET) &&
					 attreq==1)
					{
					//*** If the verify attributes flag is set prompt again.
					if (curattlst==begattlst)
						{
						sds_printf(ResourceString(IDC_INSERT__NVERIFY_ATTRIBU_45, "\nVerify attribute values" ));
						}
					if (*fs1) sprintf(fs2,"\n%s <%s>: "/*DNT*/,promptstr,fs1);
					else sprintf(fs2,"\n%s: "/*DNT*/,promptstr);
					if ((ret=sds_getstring(1,fs2,fs1))==RTERROR)
						{
						rc=(-__LINE__);
						goto bail;
						}
					else if (ret==RTCAN)
						{
						cmd_ErrorPrompt(CMD_ERR_UNTERMINATED,0);
						rc=(-1L); goto bail;

						}
					if (!*fs1) strncpy(fs1,curattlst->resval.rstring,sizeof(fs1)-1);
					}
				//*** Set the group 1 string
				if ((tmprb->resval.rstring=ic_realloc_char(tmprb->resval.rstring,strlen(fs1)+1))==NULL)
					{
					cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
					rc=(-__LINE__); goto bail;
					}
				strcpy(tmprb->resval.rstring,fs1);
				tmprb=NULL;
				//*** Make attribute
				// Link around the paperspace flag.
				// Let entmake set the PS/MS bit.
				struct resbuf *rbt;
				for (rbt=elist;rbt;rbt=rbt->rbnext)
					{
					if (rbt->rbnext && rbt->rbnext->restype==67)
						{
						tmprb=rbt->rbnext;
						rbt->rbnext=rbt->rbnext->rbnext;
						tmprb->rbnext=NULL;
						sds_relrb(tmprb);
						break;
						}
					}
				if (sds_entmake(elist)!=RTNORM)
					{
					rc=(-__LINE__);
					goto bail;
					}
				curattlst=curattlst->rbnext;
				}
			}
			IC_FREEIT
		}
	while(curattlst!=NULL && (sds_entnext_noxref(ename,ename)==RTNORM));
	//*** Make the SEQEND entity
	if ((newent=sds_buildlist(RTDXF0,"SEQEND"/*DNT*/,0))==NULL)
		{
		cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
		rc=(-__LINE__);
		goto bail;
		}
	entmakeret=sds_entmake(newent);
	sds_relrb(newent);
	if (entmakeret!=RTNORM)
		{
		rc=(-__LINE__);
		goto bail;
		}
	sds_relrb(begattlst);

	// Edit the attributes in the DDATTE dialog.
	if (attdia && attreq && !allconstantattribs)
		{
        extern sds_name SDS_AttEditName;
		sds_entlast(SDS_AttEditName);
		ExecuteUIAction( ICAD_WNDACTION_ATTE );
		}
bail:
	return rc;
	}

short DoTheInsert(char *blkname, bool explodeflag, db_insert *ins, bool fromdisk, sds_point retinspt,sds_real curelev,bool multiIns, bool redefineregen)
	{
	struct resbuf *elist,setgetrb;
	bool blockhasattribs;
	RT ret;
	sds_real rotang;

	ins->get_50(&rotang);

	if ((elist=sds_tblsearch("BLOCK"/*DNT*/,blkname,0))==NULL)
	return RTERROR;

	blockhasattribs=false;

	int colct,rowct;
	ins->get_70(&colct);
	ins->get_71(&rowct);

	if (explodeflag && (colct==1) && (rowct==1))
	{

		if (InsertExplodedBlock(blkname,elist,ins,fromdisk,retinspt)!=RTNORM)
			return RTERROR;
		IC_RELRB(elist);
		return RTNORM;

	}
	//Modified Cybage AJK 25-02-2002 DD/MM/YYYY ]

	struct resbuf *rbptr;
	if ((rbptr=ic_ret_assoc(elist,70))==NULL)
		{
		IC_RELRB(elist);
		return RTERROR;
		}
	if (rbptr->resval.rint & IC_BLOCK_HASATTRIBUTES)
		blockhasattribs=true;

	ret=AddInsertEntity(blkname,ins,curelev,blockhasattribs,multiIns, redefineregen, rotang);
	if (ret!=RTNORM)
		return RTERROR;
	if(explodeflag && (colct!=1 || rowct!=1))
	{

		sds_name ss;
		sds_name ename;
		sds_entlast(ename);
		if(sds_ssadd(ename,NULL,ss)!=RTNORM) return RTERROR;

		if(cmd_explode_objects(0,false,ss,NULL)!=RTNORM)  return RTERROR;

		return RTNORM;
	}

	IC_RELRB(elist);

// Set INSNAME system variable.
	setgetrb.restype=RTSTR;
	setgetrb.resval.rstring=blkname;
	if (sds_setvar("INSNAME"/*DNT*/,&setgetrb)!=RTNORM)
		{
		return RTERROR;
		}

	return RTNORM;
	}

short DeleteBlockIfNecc(bool fromdisk, char *blkname)
// we delete a block which we are attempting to insert iff it is from the disk and is not an xref
	{
	sds_name ename;
	db_handitem *hip;

	if (fromdisk)
		{
		hip=SDS_CURDWG->tblsearch(DB_BLOCKTAB,blkname,1);
		if (!(((db_blocktabrec *)hip)->ret_flags() & IC_BLOCK_XREF))
			{
			ename[0]=(long)hip;
			ename[1]=(long)SDS_CURDWG;
			return(sds_entdel(ename));
			}
		}
		return RTNORM;
	}

short cmd_insert2(bool minsert)
	{
	char defaultblkname[IC_ACADBUF],blkname[IC_ACADBUF],blkfile[IC_ACADBUF];
	struct resbuf rb;
	RT ret;
	bool ask4onescl=false;
	bool explodeflag;
	int ask4rows,ask4cols;
	bool ask4rot;
	sds_real xscl,yscl,zscl,rowdist,coldist,rotang;
	bool printit;
	db_insert *ins;
	bool fromdisk=false;
	sds_point extru,retinspt;
	sds_real rotbase;
	sds_real curelev;
	bool givetblsloadedmsg;
	bool multiIns;
	bool dragmode;

	ask4rot=true;
	ask4rows=ask4cols=0;
	xscl=yscl=zscl=1.0;
	rowdist=coldist=rotang=0.0;
	explodeflag=false;

    if (cmd_iswindowopen() == RTERROR) return RTERROR;

	ret=SDS_getvar(NULL,DB_QINSNAME,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);			// get default block name
	if (ret!=RTNORM)
		{
		return ret;
		}
	strncpy(defaultblkname,rb.resval.rstring,sizeof(defaultblkname)-1);
	IC_FREE(rb.resval.rstring);

	do
		{
		ret=GetBlockName(defaultblkname,blkname);										// get the block name from the user
		}
	while (ret!=RTNORM && ret!=RTCAN);

	if (ret!=RTNORM) return ret;

	ic_trim(blkname,"be");

	if (strisame(blkname,"?"/*DNT*/))								// list the blocks and exit
		{
		cmd_block_list(0);
		return RTNORM;
		}
	else if (strisame(blkname,"EXPLORE"/*DNT*/))					// call explorer and exit
		{
		ExecuteUIAction( ICAD_WNDACTION_TBLBLK );
		return RTNORM;
		}

	if (blkname[0]=='*'/*DNT*/)										// explode on insert, only one scale & indicate explode
		{
		ask4onescl=true;
		explodeflag=true;
		memmove(blkname,blkname+1,strlen(blkname)+1);
		}

	if (ProcessInsertBlockName(blkname,blkname,blkfile)!=RTNORM)	// takes blkname and returns block name, and path if any
		return RTERROR;

	if (blkfile[0])
		{
		int expert = 0;
		struct resbuf rb;
		if (SDS_getvar(NULL,DB_QEXPERT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES) == RTNORM)
			expert = rb.resval.rint;

		struct resbuf *tmprb = NULL;
		if ((tmprb=sds_tblsearch("BLOCK"/*DNT*/,blkname,0))!=NULL && (expert < 2))  // it already exists
			{
			sds_relrb(tmprb);
			char warning[IC_ACADBUF];
			char caption[IC_ACADBUF];
			_snprintf(warning,sizeof(warning), ResourceString(IDC_BLOCK__NTHE_BLOCK__S_ARE_1, "\nThe block %s already exists. Do you want to redefine it? <N> " ),blkname);
			_snprintf(caption,sizeof(caption), ResourceString(IDC_INSERT_BLOCK_ALREADY_EXI_8,"Block already exists") );			
			sds_getvar("CMDDIA", &rb);

			if (rb.resval.rint && MessageBox(IcadSharedGlobals::GetMainHWND(), warning, caption, MB_YESNO) == IDNO)
				{
				blkfile[0]=0;  // use the one we already have defined, as ACAD does
				fromdisk=false;
				}
			else
				{
				if (FindTheBlockFile(blkfile,blkfile)!=RTNORM)	// find the file's true path using sds_findfile
					return RTERROR;

				if (LoadTheBlock(blkname,blkfile)!=RTNORM)
					return RTERROR;
				fromdisk=true;
				}
			}
		else
			{
			sds_relrb(tmprb);

			if (FindTheBlockFile(blkfile,blkfile)!=RTNORM)				// find the file's true path using sds_findfile
				return RTERROR;

			if (LoadTheBlock(blkname,blkfile)!=RTNORM)
				return RTERROR;
			fromdisk=true;
			}
		}

	printit=PrintMode();
	givetblsloadedmsg = (printit && fromdisk);

	if (GetExtrusionAndRotation(extru,&rotang,&rotbase)!=RTNORM)
		{
		DeleteBlockIfNecc(fromdisk,blkname);
		return RTERROR;
		}

	if (MakeDragParlink(blkname,extru,rotang,&ins)!=RTNORM)  // creates ins
		{
		DeleteBlockIfNecc(fromdisk,blkname);
		return RTERROR;
		}

	ins->set_210(extru);

	multiIns=false;

	SDS_getvar(NULL,DB_QDRAGMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if (rb.resval.rint)
		dragmode=true;
	else
		dragmode=false;

	if (GetInsertParameters(ins,minsert,ask4onescl,dragmode,givetblsloadedmsg,rotbase,&curelev,rotang,retinspt,&multiIns)!=RTNORM)
		{
		delete ins;
		DeleteBlockIfNecc(fromdisk,blkname);
		return RTERROR;
		}

// DO THE INSERT NOW
	ret=DoTheInsert(blkname,explodeflag,ins,fromdisk,retinspt,curelev,multiIns,false);
	delete ins;
	if (ret!=RTNORM)
		DeleteBlockIfNecc(fromdisk,blkname);
	return ret;

	}

short cmd_insert(void)
	{
//*** This function places a named block or drawing into the current drawing.
//***
//*** RETURNS: RTNORM  - Success.
//***          RTCAN   - User cancel.
//***          RTERROR - Error returned from an ads function.
//***
	if (cmd_istransparent() == RTERROR) return RTERROR;

	return(cmd_insert2(false));
	}

short cmd_minsert(void)
	{
//*** This function inserts multiple instances of a block in a rectangular
//*** array.
//***
//*** RETURNS: RTNORM  - Success.
//***          RTCAN   - User cancel.
//***          RTERROR - Error returned from an ads function.
//***

	if (cmd_istransparent() == RTERROR) return RTERROR;

	return(cmd_insert2(true));
	}


short AddXrefPseudoBlock(db_drawing *xrefdp, char *fullfilepath, char *blockname,short attachoroverlay)
	{
	short attflag,xrefidx;
	db_handitem *hip;
	struct resbuf rb;
	struct resbuf *elist;
	RT ret;

	if (attachoroverlay==IC_INSMODE_XREF_ATTACH)
		attflag=IC_XREF_RESOLVED | IC_BLOCK_XREF;
	else if(attachoroverlay==IC_INSMODE_XREF_OVERLAY)
		attflag=IC_XREF_RESOLVED | IC_BLOCK_XREF | IC_BLOCK_XREF_OVERLAID;

	//get the index of the NEXT block to be created, which will be our XREF block.
	//all sub-blocks of this xref will get that xrefidx!!! NOTE: don't use sds_tblnext
	//	because we need to count blocks which are marked for deletion as well.
	xrefidx=0;
	for (hip=SDS_CURDWG->tblnext(DB_BLOCKTABREC,1); hip!=NULL; hip=hip->next)
		xrefidx++;

	db_qgetvar(DB_QINSBASE  ,xrefdp->ret_headerbuf(),rb.resval.rpoint,DB_3DPOINT,0);
	elist=sds_buildlist(RTDXF0,"BLOCK"/*DNT*/,1,fullfilepath,2,blockname,70,attflag,10,rb.resval.rpoint,0);
	if (elist==NULL)
		{
		cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
		return RTERROR;
		}

	if (sds_entmake(elist)!=RTNORM)
		{
		cmd_ErrorPrompt(CMD_ERR_DEFBLOCK,0);
		sds_relrb(elist);
		return RTERROR;
		}

	sds_relrb(elist);
	//we opened xrefdp, so set the xref dependency pointer in the blocktabrec
	(hip=SDS_LastHandItem)->set_xrefdp(xrefdp);
	hip->set_looked4xref(1);

	if ((elist=sds_buildlist(RTDXF0,"ENDBLK"/*DNT*/,0))==NULL)
		{
		cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
		SDS_CURDWG->addhanditem(NULL);   // cancels pending block
		return(RTERROR);
		}

	if ((ret=sds_entmake(elist))==RTERROR || ret==RTREJ)
		{
		sds_relrb(elist);
		cmd_ErrorPrompt(CMD_ERR_DEFBLOCK,0);
		SDS_CURDWG->addhanditem(NULL);   // cancels pending block
		return(RTERROR);
		}

	sds_relrb(elist);
	hip=SDS_CURDWG->findtabrec(DB_BLOCKTAB,blockname,1);
	if(NULL!=hip)
		((db_tablerecord *)hip)->set_xrefidx(xrefidx);
  	return RTNORM;
	}

void ReportUnsupportedEntities(db_drawing *flp, bool *gaveACISMsg, bool *gaveProxyMsg, bool *gaveA2KEntMsg)
	{
	sds_name ename;

	ename[1]=(long)flp;
	for(ename[0]=(long)flp->entnext_noxref(NULL); ename[0]!=0L; ename[0]=(long)flp->entnext_noxref((db_handitem *)ename[0]))
		{
		if (*gaveProxyMsg && *gaveACISMsg && *gaveA2KEntMsg) break;

		if (IC_TYPE_FROM_ENAME(ename)==DB_ACAD_PROXY_ENTITY && *gaveProxyMsg==0)
			{
			cmd_ErrorPrompt(CMD_ERR_INSERTPROXY,1);
			*gaveProxyMsg=true;
			continue;
			}
#ifdef BUILD_WITH_ACIS_SUPPORT
		if ((IC_TYPE_FROM_ENAME(ename)==DB_3DSOLID ||
             IC_TYPE_FROM_ENAME(ename)==DB_REGION ||
             IC_TYPE_FROM_ENAME(ename)==DB_BODY) &&
             *gaveACISMsg==0)
			{
			cmd_ErrorPrompt(CMD_ERR_INSERTACIS,1);
			*gaveACISMsg=true;
			continue;
			}
#endif
		if ((IC_TYPE_FROM_ENAME(ename)==DB_RTEXT || IC_TYPE_FROM_ENAME(ename)==DB_ARCALIGNEDTEXT || IC_TYPE_FROM_ENAME(ename)==DB_WIPEOUT) && *gaveA2KEntMsg==0)
			{
			cmd_ErrorPrompt(CMD_ERR_UNSUPPORTEDA2KENT,1);
			*gaveA2KEntMsg=true;
			continue;
			}
		}
	}


short AddXrefObjects(sds_name insname,char *xrefattachblkname)
// adds the required objects for Xrefs
	{
	sds_name dictname,idbufname;
	int ret;
	db_handitem *blkhip,*lastblkhip,*newxrefblkhip;

// Add dictionary and IDBUFFER objects
	resbuf *newobj;
	newobj=sds_buildlist(RTDXF0,"DICTIONARY"/*DNT*/,0);
	ret=sds_entmakex(newobj,dictname);
	sds_relrb(newobj);
	ASSERT(ret==RTNORM);

	newobj=sds_buildlist(RTDXF0,"IDBUFFER"/*DNT*/,0);
	ret=sds_entmakex(newobj,idbufname);
	sds_relrb(newobj);
	ASSERT(ret==RTNORM);

// add xdictionary entry to block header referencing the blkrefs dictionary
// note that this assumes we continue to allow only one attach.  This section will
// have to be reworked when that limitation is removed.
	struct resbuf *lastrb;
	for (blkhip=lastblkhip=(SDS_CURDWG->tblnext(DB_BLOCKTABREC,1));
	  blkhip!=NULL && !strisame(xrefattachblkname,((db_blocktabrec *)lastblkhip)->retp_name()); lastblkhip=blkhip,blkhip=blkhip->next) ;

// lastblkhip now points to the block def for this xref
	newxrefblkhip=lastblkhip;
	newxrefblkhip->set_xdict(DB_HARD_OWNER,((db_handitem *)dictname[0])->RetHandle(),(db_handitem *)dictname[0]);

// add pointer to idbuffer to the xdic object
	lastrb=newobj=sds_entget(dictname);
	while (lastrb->rbnext!=NULL) lastrb=lastrb->rbnext;
	lastrb->rbnext=sds_buildlist(3,"ACAD_BLKREFS"/*DNT*/,360,idbufname,0);
	ret=sds_entmod(newobj);
	ASSERT(ret==RTNORM);
	sds_relrb(newobj);

// add pointer to the insert and xdict to the idbuffer
	lastrb=newobj=sds_entget(idbufname);
	while (lastrb->rbnext!=NULL) lastrb=lastrb->rbnext;
	lastrb->rbnext=sds_buildlist(102,"{ACAD_REACTORS"/*DNT*/,330,dictname,102,"}"/*DNT*/,330,insname,0);
	ret=sds_entmod(newobj);
	ASSERT(ret==RTNORM);
	sds_relrb(newobj);
	return RTNORM;
	}

short RemoveTopLevelTables(db_drawing *topdp, char *xrefblkname)
	{
	db_handitem *hip;
	char xrefstring[IC_ACADBUF];
	sds_name ename;

	ASSERT(topdp!=NULL);

	sprintf(xrefstring,"%s|",xrefblkname);
	// order important
	ename[1]=(long)topdp;
	for (hip=topdp->tblnext(DB_STYLETABREC,1); hip!=NULL; hip=topdp->tblnext(DB_STYLETABREC,0))
		{
		if (strstr(((db_styletabrec *)hip)->retp_name(),xrefstring)!=NULL)
			{
			ename[0]=(long)hip;
			sds_entdel(ename);
			}
		}
	for (hip=topdp->tblnext(DB_DIMSTYLETABREC,1); hip!=NULL; hip=topdp->tblnext(DB_DIMSTYLETABREC,0))
		{
		if (strstr(((db_dimstyletabrec *)hip)->retp_name(),xrefstring)!=NULL)
			{
			ename[0]=(long)hip;
			sds_entdel(ename);
			}
		}
	for (hip=topdp->tblnext(DB_LTYPETABREC,1); hip!=NULL; hip=topdp->tblnext(DB_LTYPETABREC,0))
		{
		if (strstr(((db_ltypetabrec *)hip)->retp_name(),xrefstring)!=NULL)
			{
			ename[0]=(long)hip;
			sds_entdel(ename);
			}
		}
	for (hip=topdp->tblnext(DB_LAYERTABREC,1); hip!=NULL; hip=topdp->tblnext(DB_LAYERTABREC,0))
		{
		if (strstr(((db_layertabrec *)hip)->retp_name(),xrefstring)!=NULL)
			{
			ename[0]=(long)hip;
			sds_entdel(ename);
			}
		}
	return RTNORM;
	}

void RemoveXrefPseudoBlock(char *blkname)
	{
	db_handitem *hip;
	db_drawing *dp=SDS_CURDWG;
	char *blk;
	sds_name ename;

	ename[1]=(long)dp;
	for (hip=dp->tblnext(DB_BLOCKTABREC,1); hip!=NULL; hip=dp->tblnext(DB_BLOCKTABREC,0))
		{
		if (hip->ret_deleted())
			continue;
		blk=((db_blocktabrec *)hip)->retp_name();
		if (strsame(blkname,blk))
			{
			/*DG - 20.2.2002*/// Undo other additions which has been made in AddXrefPseudoBlock as well.
			((db_blocktabrec*)hip)->ret_block()->null_xrefdp();
		    dp->delhanditem(NULL, ((db_blocktabrec*)hip)->ret_block(), 2);
		    dp->delhanditem(NULL, ((db_blocktabrec*)hip)->ret_block()->next, 2);

			ename[0]=(long)hip;
			sds_entdel(ename);
			}
		}
	}




short AddDoubleToResbufList(struct resbuf **tmprb,double dbl,short dxfnum)
	{
	if (((*tmprb)=(*tmprb)->rbnext=sds_buildlist(dxfnum,dbl,0))==NULL)
		{
		cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
		return RTERROR;
		}
		return RTNORM;
	}

short AddShortToResbufList(struct resbuf **tmprb,short shrt,short dxfnum)
	{
	if (((*tmprb)=(*tmprb)->rbnext=sds_buildlist(dxfnum,shrt,0))==NULL)
		{
		cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
		return RTERROR;
		}
		return RTNORM;
	}


short AddInsertValuesToResbufList(struct resbuf *tmprb,db_insert *ins,bool indicateattributes)
	{
	sds_real xscl,yscl,zscl,rotang;
	int colct,rowct;
	sds_real coldist,rowdist;

	ASSERT(tmprb);
	ASSERT(ins);
	while (tmprb->rbnext!=NULL)
		tmprb=tmprb->rbnext;

	ins->get_41(&xscl);
	ins->get_42(&yscl);
	ins->get_43(&zscl);
	ins->get_50(&rotang);
	if (xscl!=1.0 && AddDoubleToResbufList(&tmprb,xscl,41)!=RTNORM)
		return RTERROR;
	if (yscl!=1.0 && AddDoubleToResbufList(&tmprb,yscl,42)!=RTNORM)
		return RTERROR;
	if (zscl!=1.0 && AddDoubleToResbufList(&tmprb,zscl,43)!=RTNORM)
		return RTERROR;
	if (AddDoubleToResbufList(&tmprb,rotang,50)!=RTNORM)
		return RTERROR;
	ins->get_70(&colct);
	ins->get_71(&rowct);
	if (colct!=1 || rowct!=1)
		{
		ins->get_44(&coldist);
		ins->get_45(&rowdist);

		if (AddShortToResbufList(&tmprb,colct,70)!=RTNORM)
			return RTERROR;
		if (AddShortToResbufList(&tmprb,rowct,71)!=RTNORM)
			return RTERROR;
		if (AddDoubleToResbufList(&tmprb,coldist,44)!=RTNORM)
			return RTERROR;
		if (AddDoubleToResbufList(&tmprb,rowdist,45)!=RTNORM)
			return RTERROR;
		}
	if (indicateattributes)
		if (AddShortToResbufList(&tmprb,1,66)!=RTNORM)
			return RTERROR;
	return RTNORM;
	}




short AddInsertEntity(char *blkname,db_insert *ins,sds_real curelev,bool indicateattributes,bool multiIns, bool redefineregen, sds_real RotAng /* = 0.0*/)
	{
//*** RETURNS: RTNORM  - Success.
//***          RTCAN   - User cancel.
//***          RTERROR - Error returned from an ads function.

    struct resbuf setgetrb,*newent,rb,rbent,rbucs;
    sds_point tpt,extru;
    int ret,draginspt;
    long rc=0L;
	sds_point pt1;

	if (SDS_getvar(NULL,DB_QUCSXDIR,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
		return RTERROR;
	ic_ptcpy(pt1,setgetrb.resval.rpoint);
	if (SDS_getvar(NULL,DB_QUCSYDIR,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
		return RTERROR;
	ic_crossproduct(pt1,setgetrb.resval.rpoint,extru);
	ic_ptcpy(rbent.resval.rpoint,extru);
	rbent.restype=RT3DPOINT;

	rbucs.restype=RTSHORT;
	rbucs.resval.rint=1;

	db_handitem *newxrefblkhip=NULL;
	sds_name ename;

	newent=NULL;

	draginspt=1;  // need to condition this on where we came from

	for (;;)		// for multiIns
		{
		//NOTE: Unlike ARRAY, MINSERT doesn't take into account snap angle!!!
		//		Also, minsert doesn't update lastpoint from the lower left pt in array,
		//			so previous setting above is okay
		ic_ptcpy(tpt,ins->retp_10());
		if ((newent=sds_buildlist(RTDXF0,"INSERT"/*DNT*/,2,blkname,10,tpt,210,extru,0))==NULL)
			{
			cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
			rc=(-__LINE__);
			goto bail;
			}

		if (AddInsertValuesToResbufList(newent,ins,indicateattributes)!=RTNORM)
			{
			rc=(-__LINE__);
			goto bail;
			}

		// TODO Need to adjust the text for attribs if the
		// block has x,y, and z scaling.

		// now make the insert entity
		if (sds_entmake(newent)!=RTNORM)
			{
			rc=(-__LINE__);
			goto bail;
			}

		if(indicateattributes)
		{
			resbuf* elist;
			if((elist = sds_tblsearch("BLOCK"/*DNT*/, blkname, 0)) == NULL)
				return RTERROR;
			ProcessAttributes(elist, false, ins, RotAng);
		}
		sds_relrb(newent);

		if (multiIns)
			{
			if (sds_initget(0,NULL)!=RTNORM)
				{
				rc=(-__LINE__);
				goto bail;
				}
			if (!draginspt)
				{
				ret=sds_getpoint(NULL,ResourceString(IDC_INSERT__NINSERTION_POIN_46, "\nInsertion point for next block: " ),ins->retp_10());
				}
			else
				{
				ename[0]=(long)ins;
				ename[1]=(long)SDS_CURDWG;
				SDS_getvar(NULL,DB_QDRAGMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				sds_point temppoint;
				temppoint[SDS_X]=*(sds_real *)ename;
				temppoint[SDS_Y]=temppoint[SDS_Z]=0.0;
				ret=SDS_dragobject(-1,rb.resval.rint,temppoint,
						 (double *)ins->retp_10(),
						 0.0,ResourceString(IDC_INSERT__NINSERTION_POIN_46, "\nInsertion point for next block: " ),NULL,ins->retp_10(),NULL);
				}
			if (ret!=RTNORM) break;
			}
		else break;
		}


    bail:

	if (redefineregen)
        cmd_regen();

	if (SDS_ElevationChanged)
		{
		setgetrb.restype=RTREAL;
		setgetrb.resval.rreal=curelev;
		sds_setvar("ELEVATION"/*DNT*/,&setgetrb);
		SDS_ElevationChanged=FALSE;
		}

	if (rc==0L)
		return(RTNORM);

    if (rc==(-1L)) return(RTCAN);

    if (rc<(-1L)) CMD_INTERNAL_MSG(rc);
    return(RTERROR);
	}


/*********************** New functions *********************/
bool  correct_xrefName(char *name){
//	Does correct a blockname by necessity
//	(does make inquiry to the user on change of a block name)


/*	===================================================
		Here we check validity of block name
	===================================================*/
	if (!ic_isvalidname(name)){
		// get value of filedia
		struct resbuf rb;
	    if (RTNORM!=SDS_getvar(NULL,DB_QFILEDIA,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))
			return false;
		if (rb.resval.rint){ // It is possible to make inquiry to the user on change of a block name
			SDS_CMainWindow->m_strWndAction.Empty();
			//	The original comment:
			//		this needs modification to give a suggested name
			//		for too-long (or containing spaces) xrefs (& block inserts?)
			ExecuteUIAction( ICAD_WNDACTION_INSNAME );//inquiry to the user for newname
			if (SDS_CMainWindow->m_strWndAction.IsEmpty())
				return false;
			strncpy(name,SDS_CMainWindow->m_strWndAction,SDS_CMainWindow->m_strWndAction.GetLength()+1);
			SDS_CMainWindow->m_strWndAction.Empty();
		}
		else {   // The inquiry is impossible
			printf(ResourceString(INVALID_BLOCK_NAME, "Block name is invalid."));
			return false;
		}
	}


	/*	================================================================================
							Now we check duplication of a name
		NOTE:
			We check up the xref name and all is OK if was found the xref (not block)
	====================================================================================*/
	sds_resbuf *tmprb;
	if ((tmprb=sds_tblsearch("BLOCK"/*DNT*/,name,0))) {  // Already is present
		bool b_checkFault=true;
		for (sds_resbuf *p_rb=tmprb;p_rb!=NULL;p_rb=p_rb->rbnext)
			if (p_rb->restype==70/*block type record is found*/
				&& p_rb->resval.rint & IC_BLOCK_XREF)
				b_checkFault=false;

		sds_relrb(tmprb);
		if (b_checkFault){
			sds_printf(ResourceString(EXISTING_BLOCK_NAME, "A block already exists with that name."));
		return false;
		}
	}

return true;
}

void take_insert_params(db_insert *hip,sds_resbuf	*param, FLAGS_BYTE &flags){
/*	===================================================
		Here we take parameters from the resbuf list
			 and update flags of request
	===================================================*/

	for (sds_resbuf *prb=param; prb; prb=prb->rbnext)
		switch (prb->restype){
		case 10	:	// insertion point
			hip->set_10(prb->resval.rpoint);
			flags&=~REQ_POINT;
			break;
		case 41	:	// scale on an axis "X"
			hip->set_41(prb->resval.rreal);
			flags&=~REQ_SCALE_X;
			break;
		case 42	:	// scale on an axis "Y"
			hip->set_42(prb->resval.rreal);
			flags&=~REQ_SCALE_Y;
			break;
		case 43	:	// scale on an axis "Z"
			hip->set_43(prb->resval.rreal);
			flags&=~REQ_SCALE_Z;
			break;
		case 50	:	// rotation angle
		case RTANG		:	// angle in SDS linked list code
			hip->set_50(prb->resval.rreal);
			flags&=~REQ_ANGLE;
			break;
	}
}


short xref_add_new(
	bool		 isOverlay,			// attach / overlay
	sds_resbuf	*param	/*=NULL*/,	/* If it is NULL - in such case take all parameters
										from the user */
	bool isCropPath		/*=false*/	/* if it's 'true' will be kept a file name only */
	/*	In resbuf list record "name" (code 2) can be absent
		 - in such case take xref name from path (as filename)
		(but not from the user)*/
	){
/*===================================================================
		This code is constructed on the basis of a code
		of old function "cmd_doxref()"

(!)		"recurlevel" parameter in all calls is "0" and this function
		will not be called recurrently. I have removed it from this
		function - look a label "RECURSION LEVEL" in the comments
====================================================================*/

/* The original comments:
		 Steps
	 0) Check for duplicate block name
	 0a) Check validity of block name
	 1) Save pending items
	 2) Load file
	 3) Add pseudo-definition (start block and end block)
	 4) Check identifier length for validity
	 5) Report unsupported entities
	 6) Import tables to top level
	 7) Recurse on nested xrefs
	 8) Get insertion point
	 9) Finalize -- restore pending items
*/


	FLAGS_BYTE	 flags=REQ_POINT|REQ_SCALES|REQ_ANGLE; /*DNT*/ // default request flags

	//	find path and block name in resbuf list
	char	name[IC_ACADBUF],path[IC_ACADBUF];
			*name=*path='\0';
	for (sds_resbuf *prb=param; prb && (*name=='\0' || *path=='\0'); prb=prb->rbnext){
		if (prb->restype==1 && *path=='\0') {
			strncpy(path,prb->resval.rstring,strlen(prb->resval.rstring)+1);
		}
		else
		if (prb->restype==2 && *name=='\0')
			strncpy(name,prb->resval.rstring,strlen(prb->resval.rstring)+1);
	}

	RT nRet=RTERROR;
	struct resbuf rb;

	if (*path=='\0'){
		if (RTNORM!=(nRet=sds_xref(isOverlay?SDS_XREF_FIND|SDS_XREF_OVERLAY:SDS_XREF_FIND,&rb)))
			return nRet;
		strcpy(path,rb.resval.rstring);
		IC_FREE(rb.resval.rstring);
	}

	if (*name=='\0') {
		// get blockname from path
		char *pName,*pchar,*suffix;
			pName=pchar=suffix=path;
		// may be path as 'Z:\directory.syffixdir\filename.dwg'
		// and we does find an end point:
		for (char *pc=suffix; *pc!='\0';pc++) if (*pc=='.'/*DNT*/) suffix=pc;
		//now we take a filename
		for (;*pchar!='\0';pchar++){
			// EBATECH(Futagami)-[MBCS String
			if((_MBC_LEAD ==_mbbtype((unsigned char)*pchar,0)) &&
				  (_MBC_TRAIL==_mbbtype((unsigned char)*(pchar+1),1))){
				pchar++;
				continue;
			}// ]-EBATECH
			if (*pchar=='\\' || *pchar=='/') pName=pchar+1;
			if (pchar==suffix) {
				*pchar='\0';
				strncpy(name,pName,pchar-pName+1);
				*pchar='.';/*DNT*/
			break;
			}
		}
	}

	if (!correct_xrefName(name)) return RTERROR;

	// Keep pending items
	char          pendflagsp;
    db_handitem  *keeper[4];
/*======================================================
    Storing pending items. Restore it on lable "depart".
	From here instead of "return" to use "goto depart"
  ======================================================*/
	if(SDS_CURDWG){// Store pending items
		SDS_CURDWG->get_pendstuff(&pendflagsp,keeper,keeper+1,keeper+2,keeper+3);
		//Reset pending items
		SDS_CURDWG->set_pendstuff(0,NULL,NULL,NULL,NULL);
	}

	SDS_getvar(NULL,DB_QPLINETYPE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

	// load the file
    drw_readwrite *pFileIO=new drw_readwrite;
	db_drawing *pdb;	// pointer to drawing data base

	TCHAR	fullPath[MAX_PATH] = {0};
	if( -1==access(path,00) )
	{
		TCHAR szDrive[ MAX_PATH]	= {0};
		TCHAR szFileName[ MAX_PATH]	= {0};
		TCHAR szFileExt[MAX_PATH]	= {0};

		_tsplitpath( path, szDrive, NULL, szFileName, szFileExt);
		if ( _tcscmp(szDrive,"") == 0 )
		{
			// only when file path is not given
			/*DG - 31.1.2002*/// Allow also relative paths.
			if(sds_findfile(path, fullPath) != RTNORM)
			{
				_tcscat(szFileName, szFileExt);
				if ( sds_findfile(szFileName, fullPath)==RTNORM )
					_tcscpy(path, fullPath);
			}
		}
	}
	else
		_tcscpy(fullPath, path);

	if ((pdb=pFileIO->drw_openfile(NULL,fullPath/*path*/,NULL,rb.resval.rint==2?true:false))){
		//all is OK (pdb!=NULL)
	    delete pFileIO;
		char *pFileName=pdb->ret_pn();
		if (isCropPath) {
			for (char *pc=pFileName=path;*pc!='\0';pc++)
				if (*pc=='\\' || *pc=='/') pFileName=pc;
			strcpy(pdb->ret_pn(),++pFileName); // strcpy: cropped path always shorter of full path
		}

		/*DG - 20.2.2002*/// We must allow user cancelling entering inspoint, scales etc. w/o creating
		// any objects (blocks etc.) in db. Also, the warning message "Any drawing definitions in
		// the requested block have been imported." is not needed now.
		/*DG - 31.5.2002*/// This message is necessary only in the success import.

		// build the pseudo-block
		if (RTERROR!=AddXrefPseudoBlock(pdb,path,name,
					isOverlay?IC_INSMODE_XREF_OVERLAY:IC_INSMODE_XREF_ATTACH)){
			if (TableNameLengthsLegalForXref(pdb,name) == RTNORM){

				bool gaveACISMsg=FALSE;
				bool gaveProxyMsg=FALSE;
				bool gaveA2KEntMsg=FALSE;

				ReportUnsupportedEntities(pdb,&gaveACISMsg,&gaveProxyMsg,&gaveA2KEntMsg);
				db_blocktabrec *bthip=(db_blocktabrec *)SDS_CURDWG->findtabrec(DB_BLOCKTAB,name,1);
				if (bthip)
				// update top level layer table with layers from this just-loaded file
				// only for overlays at the first level, or all attaches
				if ( UpdateTopLevelTables(SDS_CURDWG,pdb,bthip))//||  RECURSION LEVEL (recurlevel>1 && attachoroverlay==IC_INSMODE_XREF_OVERLAY))
				{
					sds_point extru;
					sds_real rotbase=0.0,rotang=0.0;

					if (GetExtrusionAndRotation(extru,&rotang,&rotbase)==RTNORM){
						db_insert *ins;
						if (MakeDragParlink(name,extru,rotang,&ins)==RTNORM){
							// creates ins
							ins->set_210(extru); // set extrusion

							take_insert_params(ins,param,flags);

							sds_name ename={(long)ins,(long)SDS_CURDWG};

							sds_real curelev=0.0; // TEMP
							sds_point retinspt;
							bool multiIns=false;

							/*DG - 20.2.2002*/// remember retcode from GetInsertParameters
							// and don't show the message (see my comments above)
							nRet = GetInsertParameters(ins,
								false, // Don't ask for multiple
								false, // Don't ask for all scales
								true,  // Dragging mode switch on
								false, //true,  // Produce messages mode switch on
								rotbase,&curelev,rotang,retinspt,&multiIns,
								flags);

							/*DG - 20.2.2002*/// check nRet (see my comments above)
							if (nRet == RTNORM && AddInsertEntity(name,ins,curelev,false,false,false)==RTNORM)	{
							// get the entity name for the insert so that the objects can refer to it
								sds_name insname;
								sds_entlast(insname);
								if (AddXrefObjects(insname,name)==RTNORM){	// this is the success point
									delete ins;
									nRet=RTNORM;
									/*DG - 31.5.2002*/// Now we can print the message.
									cmd_ErrorPrompt(CMD_ERR_XREFADDED, -1); 
									goto depart;
								}
								sds_entdel(insname);
							}
						delete ins;
						}//if MakeDragParlink()
					}//if GetExtrusionAndRotation()
				}// if UpdateTopLevelTables(SDS_CURDWG,pdb,bthip)
			// RECURSION LEVEL	if (!(recurlevel>1 && attachoroverlay==IC_INSMODE_XREF_OVERLAY)){
			RemoveTopLevelTables(SDS_CURDWG,name);
			// RECURSION LEVEL		}
			} // if TableNameLengthsLegalForXref(pdb,name)
		RemoveXrefPseudoBlock(name);
		} // if AddXrefPseudoBlock(pdb,...)

	delete pdb;
	} //NULL!=(pdb=pFileIO->drw_openfile() )
depart:
	// restore pending items
	if (SDS_CURDWG)	SDS_CURDWG->set_pendstuff(pendflagsp,keeper[0],keeper[1],keeper[2],keeper[3]);
	return nRet;
}



int		xref_reload(resbuf *param){
/*=====================================================================
Note:

	This function is almost an original piece of a code
	from old function "cmd_xref()"
======================================================================*/

	int		nCt=0;	// Xref counter
	sds_name ssdrag,ename;
	db_handitem *hip;

	for (resbuf *p=param; p!=NULL; p=p->rbnext)
	if (2==p->restype || RTSTR==p->restype) {// DXF code 2 or SDS code RTSTR
		// now p->resval.rstring is next xref name

		//search block table and find all xrefs matching pattern to update.
		//pass insertfunc blkname and name of file that's referenced
		strupr(p->resval.rstring);

		for(struct resbuf *pBlock=sds_tblnext("BLOCK"/*DNT*/,1);
				pBlock!=NULL;
				sds_relrb(pBlock),pBlock=sds_tblnext("BLOCK"/*DNT*/,0)
			){
			ic_assoc(pBlock,70); //type
			if (ic_rbassoc->resval.rint & IC_BLOCK_XREF){
				ic_assoc(pBlock,2);//name
				char strBuff[IC_ACADBUF];
				strncpy(strBuff,ic_rbassoc->resval.rstring,sizeof(strBuff)-1);
				strupr(strBuff);//name -> fs1
				if (RTNORM==sds_wcmatch(strBuff,p->resval.rstring)){ // name==fs1
					//if we're updating an xref, don't bother with asking for the scale & stuff.
					//just update the appearance of the xref in the drawing
					strncpy(strBuff,ic_rbassoc->resval.rstring,sizeof(strBuff)-1);
					hip=SDS_CURDWG->findtabrec(DB_BLOCKTABREC,strBuff,0);
					if (!hip)continue; // hip!=NULL
					((db_block *)hip)->set_xrefdp(NULL);

					struct resbuf *ptmp_rb=sds_buildlist(RTDXF0,"INSERT"/*DNT*/,2,strBuff,0);
					RT nRet=sds_pmtssget(NULL,"X"/*DNT*/,NULL,NULL,ptmp_rb,ssdrag,1);
					sds_relrb(ptmp_rb);
					if (RTNORM!=nRet) return -nRet;

					for(long xrefcnt=0L;
						RTNORM==sds_ssname(ssdrag,xrefcnt,ename);
						xrefcnt++) sds_entupd(ename);	//this reloads xref dependency
					sds_ssfree(ssdrag);
					//at this point, we're done updating existing xrefs from file, so go ahead
					//	and reload the table entries & block defs.
					db_drawing	*p_subblock=(db_drawing *)hip->ret_xrefdp();
					if (!p_subblock) continue; // not required to read the sulock if "p_subblock" is NULL
					cmd_ReadAsBlock(NULL,strBuff,IC_INSMODE_XREF_UPDATE,p_subblock);
					nCt++;
				}
			}
		} //for
	}// for RBL (resbuf list)
	sds_printf(ResourceString(IDC_CMDS3__N_I__S_UPDATED__499, "\n%i %s updated." ),nCt,(nCt==1)?ResourceString(IDC_CMDS3_REFERENCE_500, "reference" ) : ResourceString(IDC_CMDS3_REFERENCES_501, "references" ));
return nCt;
}


int xref_unload(resbuf *param){
	int		nCt=0;	// Xref counter

	for (resbuf *p=param; p!=NULL; p=p->rbnext)
	if (2==p->restype || RTSTR==p->restype) {// DXF code 2 or SDS code RTSTR
	// now p->resval.rstring is next xref name
	strupr(p->resval.rstring);
	/*===========================
		To do unload code here
	============================*/
	// if OK nCt++;
	}
	sds_printf("\nIt's in a stage of development yet.\n");
return nCt;
}
