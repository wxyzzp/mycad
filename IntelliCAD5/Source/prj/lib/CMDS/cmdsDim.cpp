#include "cmds.h"
#include "cmdsDim.h"
#include "dimensions.h"
#include "IcadApi.h"

extern struct cmd_dimeparlink* plink;
extern short cmd_dimmode;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DIMFUNC
//
// RETURNS: RTNORM  - Success.
//          RTCAN   - User cancel.
//          RTERROR - Misc. error occured.
//
short cmd_DimFunc(short dimmode,db_drawing *flp) { 	// Prompts are frozen  5-9-96
    char          fs1[IC_ACADBUF] = ""/*DNT*/,
                  lastcmd[IC_ACADBUF] = ""/*DNT*/;
    RT			  ret=RTNORM;
    long          rc=0L;
	struct resbuf rb;

    if (!plink) plink=(struct cmd_dimeparlink *)new (struct cmd_dimeparlink);
    memset(plink,(int)0,(sizeof(struct cmd_dimeparlink))-4);
    plink->BeginDrag=TRUE;
    cmd_setparlink(flp);

	lastcmd[0]=0;
	// Bugzilla No. 78117; 22/04/2002 [
	bool bAddToLastcmd;
    for (;;) {
        *fs1=0; 
		bAddToLastcmd = TRUE;
        cmd_dimmode=1;
		LOAD_COMMAND_OPTIONS_5(IDC_DIMENSIONS_INITGET_HORI_12)
        if(sds_initget(SDS_RSG_OTHER, LOADEDSTRING)!=RTNORM) { rc=RTERROR; goto exit; }
			// I'm using "DOne" above, instead of "`" because the code here sets dimmode back to 0 before it exits.
			// and because "Diameter" already has "D" used.
        ret=sds_getkword(ResourceString(IDC_DIMENSIONS__NDIMENSIONI_13, "\nDimensioning command: " ),fs1);
        //Bugzilla No. 46352 ; 28-05-2002
		//Check if drawing is switched
		if ( flp != SDS_CURDWG)
		{
			flp = SDS_CURDWG;
			cmd_setparlink(flp);
		}
        strupr(fs1);

		if(ret==RTNONE) {
			//if(*lastcmd==0) goto exit;
			if(*lastcmd==0)
			{
				cmd_ErrorPrompt(CMD_ERR_EXITDONE,0);
				continue;
			}
			else {
				ret=RTNORM; strcpy(fs1,lastcmd);
				SDS_getvar(NULL,DB_QCMDECHO,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
                if(CMD_CMDECHO_PRINTF(rb.resval.rint)) sds_printf("\n%s"/*DNT*/,fs1);
			}
		}

        if(ret!=RTNORM && ret!=RTSTR) goto exit;
        SDS_CallUserCallbackFunc(SDS_CBCMDBEGIN,(void *)fs1,NULL,NULL);

        if       (strsame("EXIT"/*DNT*/,      fs1)) { ret=RTNORM; goto exit;
		} else if(strsame("DONE"/*DNT*/,      fs1)) { ret=RTNORM; goto exit;
        } else if(strsame("HORIZONTAL"/*DNT*/,fs1)) { ret=cmd_DimLinearFunc(1,flp);
        } else if(strsame("VERTICAL"/*DNT*/,  fs1)) { ret=cmd_DimLinearFunc(2,flp);
        } else if(strsame("ALIGNED"/*DNT*/,   fs1)) { ret=cmd_DimLinearFunc(5,flp);
        //Bugzilla No. 46352 ; 28-05-2002
		} else if(strsame("ANGULAR"/*DNT*/,   fs1)) { ret=cmd_dimangular(FALSE,flp);

		//Modified Cybage SBD 09/03/2001 DD/MM/YYYY
		//Reason : Fix for Bug No. 45931 from Bugzilla 
		//} else if(strsame("LEADER"/*DNT*/,	fs1)) { ret=cmd_leader();
        } else if(strsame("LEADER"/*DNT*/,    fs1)) { ret=cmd_dimleader();
		} else if(strsame("OBLIQUE"/*DNT*/,   fs1)) { ret=cmd_DimEditFunc(3);
        } else if(strsame("ROTATED"/*DNT*/,   fs1)) { ret=cmd_DimLinearFunc(3,flp);
        //BugZilla No. 78284; 23-09-2002
		//} else if(strsame("CENTER"/*DNT*/  ,  fs1)) { ret=cmd_dimcenter();
		} else if(strsame("CENTER"/*DNT*/  ,  fs1)) { ret=cmd_DimArcOrCircleFunc(0,flp);
        } else if(strsame("DIAMETER"/*DNT*/,  fs1)) { ret=cmd_DimArcOrCircleFunc(3,flp);
        } else if(strsame("RADIUS"/*DNT*/,    fs1)) { ret=cmd_DimArcOrCircleFunc(4,flp);
        } else if(strsame("BASELINE"/*DNT*/,  fs1)) { ret=cmd_DimContinueFunc(3,flp);
        } else if(strsame("CONTINUE"/*DNT*/,  fs1)) { ret=cmd_DimContinueFunc(2,flp);
        //Bugzilla No. 46352 ; 28-05-2002
		} else if(strsame("ORDINATE"/*DNT*/,  fs1)) { ret=cmd_dimordinate(flp);
        } else if((strsame("POSITION"/*DNT*/,  fs1)) || (strsame("TEDIT"/*DNT*/,  fs1))) { ret=cmd_dimtedit(); 
        } else if(strsame("EDIT"/*DNT*/,	  fs1)) { ret=cmd_DimEditFunc(0);	// We don't need dimEdit also here 'cause it'll just work anyway.
        } else if(strsame("OVERRIDE"/*DNT*/,  fs1)) { ret=cmd_doverride(flp);
        } else if(strsame("SETTINGS"/*DNT*/,  fs1)) { ret=cmd_ddim(); 
        } else if((strsame("APPLY"/*DNT*/,     fs1))||(strsame("UPDATE"/*DNT*/,     fs1))) { ret=cmd_DimStyleFunc(5,flp); 
        } else if(strsame("HOMETEXT"/*DNT*/,  fs1)) { ret=cmd_DimEditFunc(1); 
        } else if(strsame("NEWTEXT"/*DNT*/,   fs1)) { ret=cmd_DimEditFunc(2); 
        } else if(strsame("RESTORE"/*DNT*/,   fs1)) { ret=cmd_DimStyleFunc(2,flp);
        } else if(strsame("SAVE"/*DNT*/,      fs1)) { ret=cmd_DimStyleFunc(1,flp);
        } else if(strsame("STATUS"/*DNT*/,    fs1)) { ret=cmd_DimStyleFunc(3,flp);
        } else if(strsame("TROTATE"/*DNT*/,   fs1)) { ret=cmd_DimEditFunc(4); 
        } else if(strsame("VARIABLES"/*DNT*/, fs1)) { ret=cmd_DimStyleFunc(4,flp); 
        } else if(strisame("DIM"/*DNT*/,      fs1)) { cmd_ErrorPrompt(CMD_ERR_UNRECOGCMD,0); bAddToLastcmd = FALSE; ret=RTNONE;
		} 
		//BugZilla No. 78014; 15-07-2002	
		//else{
		else 
		{	
			char fsTransCommand[IC_ACADBUF] ="\'"/*DNT*/;
			strcpy(fsTransCommand+1, fs1);
			// TODO These command name ptrs should be static strings to save
			//      The allocation of memory.
			SDS_SetUndo(IC_UNDO_COMMAND_BEGIN,(void *)fsTransCommand,NULL,NULL,flp);
			SDS_DoOneCommand(fsTransCommand,0);
			SDS_SetUndo(IC_UNDO_COMMAND_END,(void *)fsTransCommand,NULL,NULL,flp);
			bAddToLastcmd = FALSE;
		}
		
		if(bAddToLastcmd)
			strcpy(lastcmd,fs1);
        if(ret==RTNORM) {
            //strcpy(lastcmd,fs1);
            SDS_CallUserCallbackFunc(SDS_CBCMDEND,(void *)fs1,NULL,NULL);
        }
		if(dimmode) break;
    }
	// Bugzilla No. 78117; 22/04/2002 ]

    exit:
        cmd_dimmode=0;
		cmd_CleanUpDimVarStrings();		// Clean up first, then return ret. 
        return(ret);
}

short cmd_dimleader(void) 
{
    if (cmd_iswindowopen() == RTERROR) return RTERROR;
	return(cmd_DrawLeader(1));
}

short cmd_leader(void) {
    if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    // This is a temporary fix we need to set it to 0 later.
	//Modified Cybage SBD 09/03/2001 DD/MM/YYYY
	//Reason : Fix for Bug No. 45931 from Bugzilla 
	//return(cmd_DrawLeader(1));
	return(cmd_DrawLeader(0));
}

short cmd_dimang(void) 
{
	//BugZilla No. 78149; 13-05-2002
	RT ret=RTERROR;

    if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;
	//BugZilla No. 78149; 13-05-2002
	cmd_initialize();
    cmd_setparlink(SDS_CURDWG);

	//BugZilla No. 78149; 13-05-2002
    //return(cmd_dimangular(TRUE,SDS_CURDWG));
	ret=cmd_dimangular(TRUE,SDS_CURDWG);
	cmd_CleanUpDimVarStrings();		// Clean up first, then return ret.
	return ret;

}

short cmd_dimstyle(void) 
{

	RT ret=RTERROR;

    if (cmd_iswindowopen() == RTERROR) return RTERROR;
    cmd_initialize();
    cmd_setparlink(SDS_CURDWG);
    ret=(cmd_DimStyleFunc(0,SDS_CURDWG));

	cmd_CleanUpDimVarStrings();		// Clean up first, then return ret.
	return ret;

}

short cmd_dim(void) 
{
    if (cmd_iswindowopen() == RTERROR) return RTERROR;
	
	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    return(cmd_DimFunc(0,SDS_CURDWG));
}

short cmd_dim1(void) 
{
    if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    return(cmd_DimFunc(1,SDS_CURDWG));
}

short cmd_dimaligned(void) 
{
	RT ret=RTNORM;

    if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    cmd_initialize(true);
    cmd_setparlink(SDS_CURDWG);
    ret=(cmd_DimLinearFunc(4,SDS_CURDWG));

	cmd_CleanUpDimVarStrings();		// Clean up first, then return ret. 
	return ret;
}

short cmd_dimlinear(void) 
{
	RT ret=RTNORM;

    if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    cmd_initialize(true);
    cmd_setparlink(SDS_CURDWG);
    ret=(cmd_DimLinearFunc(0,SDS_CURDWG));

	cmd_CleanUpDimVarStrings();		// Clean up first, then return ret.
	return ret;
}

short cmd_dimbaseline(void) 
{

	RT ret=RTNORM;

    if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    cmd_initialize(true);
    cmd_setparlink(SDS_CURDWG);
    ret=(cmd_DimContinueFunc(1,SDS_CURDWG));

	cmd_CleanUpDimVarStrings();		// Clean up first, then return ret.
	return ret;

}

short cmd_dimcontinue(void) 
{

	RT ret=RTNORM;

    if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    cmd_initialize(true);
    cmd_setparlink(SDS_CURDWG);
    ret=(cmd_DimContinueFunc(0,SDS_CURDWG));

	cmd_CleanUpDimVarStrings();		// Clean up first, then return ret.
	return ret;

}

short cmd_dimcenter(void) 
{

	RT ret=RTERROR;

    if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    cmd_initialize(true);
    cmd_setparlink(SDS_CURDWG);
    ret=(cmd_DimArcOrCircleFunc(0,SDS_CURDWG));

	cmd_CleanUpDimVarStrings();		// Clean up first, then return ret.
	return ret;

}

short cmd_dimdiameter(void) 
{
	RT ret=RTERROR;

    if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    cmd_initialize(true);
    cmd_setparlink(SDS_CURDWG);
    ret=(cmd_DimArcOrCircleFunc(1,SDS_CURDWG));	// HURLS 

	cmd_CleanUpDimVarStrings();		// Clean up first, then return ret.
	return ret;

}

short cmd_dimradius(void) 
{
	RT ret=RTERROR;

    if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    cmd_initialize(true);
    cmd_setparlink(SDS_CURDWG);
    ret=(cmd_DimArcOrCircleFunc(2,SDS_CURDWG));

	cmd_CleanUpDimVarStrings();		// Clean up first, then return ret. 
	return ret;

}

short cmd_dimedit(void) 
{
	RT ret=RTERROR;

    if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    cmd_initialize();
    cmd_setparlink(SDS_CURDWG);
    ret=(cmd_DimEditFunc(0));

	cmd_CleanUpDimVarStrings();		// Clean up first, then return ret. 
	return ret;

}

short cmd_dimord(void) 
{

	RT ret=RTERROR;

    if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    cmd_initialize();
    cmd_setparlink(SDS_CURDWG);
    ret=(cmd_dimordinate(SDS_CURDWG));

	cmd_CleanUpDimVarStrings();		// Clean up first, then return ret. 
	return ret;

}

short cmd_dimoverride(void) 
{

	RT ret=RTNORM;

    if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    cmd_initialize(true);
    cmd_setparlink(SDS_CURDWG);
    ret=(cmd_doverride(SDS_CURDWG));

	cmd_CleanUpDimVarStrings();		// Clean up first, then return ret.
	return ret;

}

