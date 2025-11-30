/* D:\ICADDEV\PRJ\LIB\CMDS\RENAME.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 
// THIS FILE HAS BEEN GLOBALIZED!
//** Includes
#include "cmds.h"/*DNT*/
#include "Icad.h" /*DNT*/
#include "IcadApi.h"

//** Functions
short cmd_renameapp(void)
	{
	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	ExecuteUIAction( ICAD_WNDACTION_RENAMEAPP );
    return(RTNORM);
	}

short cmd_renamevarf(sds_name ss, char *fs1, char *fs2) {
    char          fs3[IC_ACADBUF],
                  fs4[IC_ACADBUF];
    LPCTSTR			fcp1;
    LPCTSTR			fcp2;
    sds_name      curr;

    struct resbuf *apps,
                  appnam,
                  *temp;

    long          fl1;

    *fs3=0;
    appnam.restype = RTSTR;
    appnam.resval.rstring = "*"/*DNT*/;
    appnam.rbnext = NULL;
	apps=NULL;

    for(fl1=0; sds_ssname(ss,fl1,curr)==RTNORM; ++fl1) {
        if ((apps=sds_entgetx(curr,&appnam))!=NULL) {
             for(temp=apps; temp!=NULL; temp=temp->rbnext) {
                if (temp->restype==1001) {
                    break;
                }
            }
            for(;temp!=NULL; temp=temp->rbnext) {
                if ((fcp1=stristr(temp->resval.rstring,fs1))!=NULL) {
                    if ((fcp2=strchr(temp->resval.rstring,'='/*DNT*/))!=NULL) {
                        if (*temp->resval.rstring=='&'/*DNT*/) {
                            sprintf(fs4,"&%s%s"/*DNT*/,fs2,fcp2);
                        } else {
                            sprintf(fs4,"%s%s"/*DNT*/,fs2,fcp2);
                        }
                        IC_FREE(temp->resval.rstring);
                        temp->resval.rstring= new char [strlen(fs4)+1];
                        strcpy(temp->resval.rstring, fs4);
                        sds_entmod(apps);
                    }
                }
            }
            IC_RELRB(apps);
        }
    }
    return(0);
}

//Bugzilla No. 78037 ; 29-04-2002 [
// This function updates the names of xref dependant tables when xref block is renamed.

short cmd_renameXrefDependantTables(char *origname,char *newname,db_drawing *dbDrawing)
{	
	char *TableName[]=		  {"STYLE"/*DNT*/,"LTYPE"/*DNT*/,"LAYER"/*DNT*/,"DIMSTYLE"/*DNT*/,"BLOCK"/*DNT*/};
	int nTableCount = sizeof(TableName)/sizeof(TableName[0]);	
	RT ret = RTNORM;	
	struct resbuf *rbptemp,*rbp,*rbb;	
	char szName[IC_ACADBUF],szBlockName[IC_ACADBUF];		
	char *tok = NULL;
	
	rbptemp = rbp = rbb = NULL;
	for(int i=0; i < nTableCount;i++)
	{
		rbb=SDS_tblgetter(TableName[i],NULL,TRUE,dbDrawing);
		while(rbb) 
		{													
			if (ic_assoc(rbb,2) == 0 && strchr(ic_rbassoc->resval.rstring,CMD_XREF_TBLDIVIDER)) 
			{
				strcpy(szBlockName,ic_rbassoc->resval.rstring);
				tok = strtok(szBlockName,CMD_XREF_TBLDIVIDERSTR);
				if ( tok != NULL && strisame(tok,origname) == 1 )
				{	
					tok = strtok(NULL,CMD_XREF_TBLDIVIDERSTR);
					if ( tok != NULL )
					{	
						sprintf(szName,"%s%s%s",newname,CMD_XREF_TBLDIVIDERSTR,tok);						
						for(rbptemp=rbb;rbptemp->rbnext!=NULL;rbptemp=rbptemp->rbnext);
						if((rbp=sds_buildlist(1,szName,0))==NULL){
							sds_relrb(rbb);rbb=NULL;
							cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
							return(RTERROR);
						}
						rbptemp->rbnext=rbp;
						rbptemp=rbp=NULL;
						ret=SDS_tblmesser(rbb,IC_TBLMESSER_RENAME,dbDrawing,true);  // ok for dupes
					}
				}				
			}
			IC_RELRB(rbb);
			rbb=SDS_tblgetter(TableName[i],NULL,FALSE,dbDrawing);
		}	
	}
	return (ret);
}
//Bugzilla No. 78037 ; 29-04-2002 ]

short cmd_rename(void)   { 

	if (cmd_iswindowopen() == RTERROR) return RTERROR;			

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    TCHAR fs3[IC_ACADBUF],origname[IC_ACADBUF],tabletoberenamed[IC_ACADBUF],
		tablefirstcharcap[IC_ACADBUF],newname[IC_ACADBUF],itemtoberenamed[IC_ACADBUF];
	RT ret;
    struct resbuf *rbp1,*rbp2,*rbptemp;
	short doingvports;

	rbp1=rbp2=NULL;
    if(sds_initget(0,ResourceString(IDC_RENAME_INITGET_BLOCK_DIM_0, "Block Dimension_style|Dimstyle Layer|LAyer Linetype|LType Style UCS|Ucs View|VIew Viewport|VPort ~_Block ~_Dimstyle ~_LAyer ~_LType ~_Style ~_Ucs ~_VIew ~_VPort" ))!=RTNORM) 
		return(-2);
    if((ret=sds_getkword(ResourceString(IDC_RENAME__NRENAME___BLOCK__1, "\nRename:  Block/Dimstyle/LAyer/LType/Style/Ucs/VIew/VPort: " ),tabletoberenamed))==RTERROR) {
        return(ret);
    } else if(ret==RTCAN) {
        return(ret);
    } else if(ret==RTNONE) {
        return(ret);
    }

	if (strisame(tabletoberenamed, "BLOCK"))
		sprintf(itemtoberenamed,ResourceString(IDC_RENAME_BLOCK, "Block" ));
	else if (strisame(tabletoberenamed, "DIMSTYLE"))
		sprintf(itemtoberenamed,ResourceString(IDC_RENAME_DIMSTYLE, "Dimension style" ));
	else if (strisame(tabletoberenamed, "LAYER"))
		sprintf(itemtoberenamed,ResourceString(IDC_RENAME_LAYER, "Layer" ));
	else if (strisame(tabletoberenamed, "LTYPE"))
		sprintf(itemtoberenamed,ResourceString(IDC_RENAME_LTYPE, "Linetype" ));
	else if (strisame(tabletoberenamed, "STYLE"))
		sprintf(itemtoberenamed,ResourceString(IDC_RENAME_STYLE, "Style" ));
	else if (strisame(tabletoberenamed, "UCS"))
		sprintf(itemtoberenamed,ResourceString(IDC_RENAME_UCS, "UCS" ));
	else if (strisame(tabletoberenamed, "VIEW"))
		sprintf(itemtoberenamed,ResourceString(IDC_RENAME_VIEW, "View" ));
	else if (strisame(tabletoberenamed, "VPORT"))
		sprintf(itemtoberenamed,ResourceString(IDC_RENAME_VPORT, "Viewport" ));

	_tcscpy(tablefirstcharcap,tabletoberenamed);
	_tcsupr(tablefirstcharcap);
	_tcslwr(&tablefirstcharcap[1]);
	sprintf(fs3,ResourceString(IDC_RENAME__N_S_TO_BE_RENAME_2, "\n%s to be renamed: " ),itemtoberenamed);

	while(1){														// Notes below refer to R14 compatibility - *it* is not consistent!
		if(((ret=sds_getstring(1,fs3,origname))!=RTNORM) || !*origname) return(ret);
		ic_trim(origname,"be");
		strupr(origname);

		if(!ic_isvalidname(origname)){									// 1-31 characters, isalnum, except $-_ are OK.
			cmd_ErrorPrompt(CMD_ERR_ANYNAME,0);						//		...Unable to recognize entry as a valid name.
			continue;												//		...and ask again.
		}
		if((strisame(tabletoberenamed,"LAYER"/*DNT*/ ))&&(strisame(origname,"0"/*DNT*/))){				// In AutoCAD, this says
			cmd_ErrorPrompt(CMD_ERR_CANTRENAME,0);					//		..."Can't rename"
			return(RTNORM);											//		...and bails (does not ask again).
		}
		if( (strisame(tabletoberenamed,"LTYPE"/*DNT*/ )) && ( (strisame(origname,"BYBLOCK"/*DNT*/)) || (strisame(origname,"BYLAYER"/*DNT*/)) ) ){		// In AutoCAD, this says... 
			cmd_ErrorPrompt(CMD_ERR_CANTRENAME,0);					//		...Invalid...
			continue;												//		...and asks again.
		}
		if( (strisame(tabletoberenamed,"LTYPE"/*DNT*/ )) && (strisame(origname,"CONTINUOUS"/*DNT*/)) ){		// In AutoCAD, this says
			cmd_ErrorPrompt(CMD_ERR_CANTRENAME,0);					//		...Can't rename
			return(RTNORM);											//		...and bails (does not ask again).
		}
//		if((strisame(tabletoberenamed,"Style"))&&((strisame(tabletoberenamed,"STANDARD"))){	// AutoCAD allows this.  You *can* rename standard.
//		if((strisame(tabletoberenamed,"Ucs"))&&((strisame(tabletoberenamed,"WORLD"))){			// AutoCAD allows this.  World or WCS are not in the table.
//		if(*origname=='*'){												// In AutoCAD, * for any of the things one can rename... 
//			cmd_ErrorPrompt(CMD_ERR_CANTRENAME,0);					//		...should give a message (can't do that)...
//			continue;												//		...and ask again.
//		}
		if(NULL==(rbp1=sds_tblsearch(tablefirstcharcap,origname,0))) {
			cmd_ErrorPrompt(CMD_ERR_NOTABLEITEM,0,itemtoberenamed,origname);
			continue;
		}				  
		break;
	}
	
	//Bugzilla No. 78037 ; 29-04-2002 [
	bool IsXrefBlock = false;
	if (strisame(tablefirstcharcap,"Block"/*DNT*/))
	{
		//Check if Block is Xref Block
		if (ic_assoc(rbp1,70)==0 && (ic_rbassoc->resval.rint & IC_BLOCK_XREF ) )
		{
			
			sds_printf(ResourceString(IDC_RENAME_XREF_CAUTION_MSG,"Caution! \"%s\" is an externally referenced block.\nRenaming it will also rename its dependent symbols."),origname);
			IsXrefBlock = true;
			SDS_SetUndo(IC_UNDO_COMMAND_END, (void *)"RENAME"/*DNT*/, NULL, NULL, SDS_CURDWG);
			SDS_SetUndo(IC_UNDO_COMMAND_BEGIN, (void *)"RENAME"/*DNT*/,	NULL, NULL, SDS_CURDWG);
		}
	}
	//Bugzilla No. 78037 ; 29-04-2002 ]

	sprintf(fs3,ResourceString(IDC_RENAME__NNEW_NAME_FOR_TH_5, "\nNew name for the %s %s: " ),itemtoberenamed,origname);
	while(1){
		if(RTNORM!=sds_getstring(1,fs3,newname) || !*newname){
			sds_relrb(rbp1);rbp1=NULL;
			return(ret);
		}
		/*D.G.*/
		ic_trim(newname,"be");
		// Bugzilla No. 78037 ; 29-04-2002
		//_tcsupr(newname);
		if(_tcsclen(newname) >= IC_ACADNMLEN){						// Would be handled below, but nicer to... 
			cmd_ErrorPrompt(CMD_ERR_NAMETOOLONG,0);					//		...let user know problem is length of name.
			continue;
		}
		if(!ic_isvalidname(newname)){								// 1 - (IC_ACADNMLEN-1) characters, isalnum, except $-_ are OK.
			cmd_ErrorPrompt(CMD_ERR_ANYNAME,0);						//		..."Unable to recognize entry as a valid name."
			continue;												//		...and ask again.
		}
		if((rbp2=sds_tblsearch(tablefirstcharcap,newname,0))!=NULL){
			cmd_ErrorPrompt(CMD_ERR_ALREADYEXISTS,0,tabletoberenamed);
			sds_relrb(rbp2);
			rbp2=NULL;
			continue;
		}
		break;
	}//now we have our new table entry name in newname

// now do the renames.  For vports, loop to rename ALL the entries with the origname
	if (strisame(tablefirstcharcap,"vport"/*DNT*/)) doingvports=1;
	else doingvports=0;
	

	do {
		for(rbptemp=rbp1;rbptemp->rbnext!=NULL;rbptemp=rbptemp->rbnext);
		if((rbp2=sds_buildlist(1,newname,0))==NULL){
			sds_relrb(rbp1);rbp1=NULL;
			cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
			return(RTERROR);
		}
		rbptemp->rbnext=rbp2;
		rbptemp=rbp2=NULL;
		ret=SDS_tblmesser(rbp1,IC_TBLMESSER_RENAME,SDS_CURDWG,true);  // ok for dupes
		//Bugzilla No. 78037 ; 29-04-2002 [
		if (IsXrefBlock)
		{	
			ret = cmd_renameXrefDependantTables(origname,newname,SDS_CURDWG);
			SDS_SetUndo(IC_UNDO_COMMAND_END, (void *)"RENAME"/*DNT*/, NULL, NULL, SDS_CURDWG);
		}
		//Bugzilla No. 78037 ; 29-04-2002 ]
		if(ret==RTNORM) sds_printf(ResourceString(IDC_RENAME__S__S_WAS_RENAMED_6, "%s %s was renamed to %s.\n" ),itemtoberenamed,origname,newname);
		sds_relrb(rbp1);
		if (doingvports)
			rbp1=sds_tblsearch(tablefirstcharcap,origname,0);
	} while (ret==RTNORM && doingvports && rbp1!=NULL);

//	cmd_ErrorPrompt(CMD_ERR_WASRENAMEDAS,0,tabletoberenamed,origname);		
			
	return(ret); 
}



