/* G:\ICADDEV\PRJ\LIB\CMDS\ENTITYDATA.CPP
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
#include "Icad.h"/*DNT*/
#include "IcadApi.h"

static char        SDS_string[IC_ACADBUF];

short cmd_deledata(void){
	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	short flag;
	char  appname[50],
         *string;
    sds_name ss;
    struct resbuf *rb;            /* argument buffer from autolisp */
    RT	ret, rc;
    struct resbuf *elist,
                   appnamo;

    if((rb=sds_getargs())==NULL) {
        do {
            if (RTNORM!=(ret=sds_initget(RSG_OTHER,ResourceString(IDC_ENTITYDATA_INITGET_LIST__0, "List_app_names|? ~_?" )))) return(ret);
            if (RTNORM!=(ret=sds_getkword(ResourceString(IDC_ENTITYDATA__NDELETE_ENTI_1, "\nDelete Entity Data:  ? to list application names/<Name of application>: " ),appname))) return(ret);
            if (strisame(appname, "?"/*DNT*/)) {
                elist=sds_tblnext("APPID"/*DNT*/,1);
                if (!elist) return(RTNORM);
                do {
                    for(rb=elist;rb;rb=rb->rbnext) {
                        if (rb->restype==2) sds_printf("\n%s"/*DNT*/,rb->resval.rstring);
                    }
                    elist=sds_tblnext("APPID"/*DNT*/,0);
                } while(elist);
            }
        } while(strisame(appname, "?"/*DNT*/));
        string=_strupr(appname); strcpy(appname,string);
        appnamo.restype = RTSTR;
        appnamo.resval.rstring = appname;
        appnamo.rbnext = NULL;

        flag=0;
        sds_ssfree(ss);
        if (sds_pmtssget(ResourceString(IDC_ENTITYDATA__NSELECT_ENTI_2, "\nSelect entities to delete Entity Data: " ),NULL,NULL,NULL,NULL,ss,0)!=RTNORM) {
            sds_retvoid(); return(RTERROR);
        }
    }
    rc=cmd_deleed(ss,appname);
	sds_ssfree(ss);
    if(flag) sds_rett(); else sds_retvoid();
	if (rc==0)return(RTNORM);
	else return(RTERROR);
}

short cmd_moveedata(void){
	if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	short flag;
	char  appname[50],
         *string;
    sds_point pt;
    sds_name ename,ss;
    struct resbuf *rb;            /* argument buffer from autolisp */
    RT rc, ret;
	short fi1;
    struct resbuf *elist,
                   appnamo;

    if ((rb=sds_getargs())==NULL) {
        do {
            if (RTNORM!=(ret=sds_initget(RSG_OTHER,ResourceString(IDC_ENTITYDATA_INITGET_LIST__0, "List_app_names|? ~_?" )))) return(ret);
            if (RTNORM!=(ret=sds_getkword(ResourceString(IDC_ENTITYDATA__NMOVE_ENTITY_3, "\nMove Entity Data:  ? to list application names/<Name of application>: " ),appname))) return(ret);
            if (strisame(appname, "?"/*DNT*/)) {
                elist=sds_tblnext("APPID"/*DNT*/,1);
                if (!elist) return(RTNORM);
                do {
                    for(rb=elist;rb;rb=rb->rbnext) {
                        if (rb->restype==2) sds_printf("\n%s"/*DNT*/,rb->resval.rstring);
                    }
                    elist=sds_tblnext("APPID"/*DNT*/,0);
                } while(elist);
            }
        } while(strisame(appname, "?"/*DNT*/));
        string=_strupr(appname); strcpy(appname,string);
        appnamo.restype = RTSTR;
        appnamo.resval.rstring = appname;
        appnamo.rbnext = NULL;
        flag=0;
        if (sds_entsel(ResourceString(IDC_ENTITYDATA__NSELECT_ENTI_4, "\nSelect entity from which to move data: " ), ename, pt)!=RTNORM) {
            sds_retvoid(); return(RTERROR);
        }

        // Check if there is an application name associated with the selected entity.
        if ((elist=sds_entgetx(ename,&appnamo))!=NULL) {
            for(rb=elist; rb!=NULL; rb=rb->rbnext) { if (rb->restype==-3) break; }
            if (!rb) {
                sprintf(SDS_string,ResourceString(IDC_ENTITYDATA_NO_DATA_WITH__5, "No data with application name \"%s\" associated with entity." ),appname);
				MessageBox(IcadSharedGlobals::GetMainHWND(),SDS_string,ResourceString(IDC_ENTITYDATA_MESSAGE_7, "Message" ),MB_OK);
                return(RTERROR);
            }
        }

        sds_ssfree(ss);
        if (sds_pmtssget(ResourceString(IDC_ENTITYDATA__NSELECT_ENTI_8, "\nSelect entities to which Entity Data will be moved: " ),NULL,NULL,NULL,NULL,ss,0)!=RTNORM) {
            sds_retvoid(); return(RTERROR);
        }
//        fi1=MessageBox(SDS_hwnd,"Overwrite any existing Entity Data?","Message",MB_YESNO);
    }

	fi1 = 1; //this variable was uninitialized in v1.0
	rc=cmd_moveeed(ename,ss,appname);
	sds_ssfree(ss);
    if(flag) sds_rett(); else sds_retvoid();
	if (rc==0)return(RTNORM);
	else return(RTERROR);

}
short cmd_copyedata(void){

	if (cmd_iswindowopen() == RTERROR) return RTERROR;
	
	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	short          flag;
	char           appname[50],
                  *string;
    sds_point      pt;
    sds_name       ename,
                   ss;
    struct resbuf *rb;            /* argument buffer from autolisp */
    short          rc,
                   fi1;
    RT	           ret;
    struct resbuf *elist,
                   appnamo;
    BOOL           LoopFlag=TRUE;


    if ((rb=sds_getargs())==NULL) {
        while (LoopFlag) {
            LoopFlag=FALSE;
            do {
                if (RTNORM!=(ret=sds_initget(RSG_OTHER,ResourceString(IDC_ENTITYDATA_INITGET_LIST__0, "List_app_names|? ~_?" )))) return(ret);
                if (RTNORM!=(ret=sds_getkword(ResourceString(IDC_ENTITYDATA__NCOPY_ENTITY_9, "\nCopy Entity Data:  ? to list application names/<Name of application>: " ),appname))) return(ret);
                if (strisame(appname, "?"/*DNT*/)) {
                    elist=sds_tblnext("APPID"/*DNT*/,1);
                    if (!elist) return(RTNORM);
                    do {
                        for(rb=elist;rb;rb=rb->rbnext) {
                            if (rb->restype==2) sds_printf("\n%s"/*DNT*/,rb->resval.rstring);
                        }
                        elist=sds_tblnext("APPID"/*DNT*/,0);
                    } while(elist);
                }
            } while(strisame(appname, "?"/*DNT*/));
            string=_strupr(appname); strcpy(appname,string);
            appnamo.restype = RTSTR;
            appnamo.resval.rstring = appname;
            appnamo.rbnext = NULL;
            flag=0;
            if (sds_entsel(ResourceString(IDC_ENTITYDATA__NSELECT_ENT_10, "\nSelect entity from which to copy Entity Data: " ), ename, pt)!=RTNORM) {
                sds_retvoid(); return(RTERROR);
            }
            // Check if there is an application name associated with the selected entity.
            if ((elist=sds_entgetx(ename,&appnamo))!=NULL) {
                for(rb=elist; rb!=NULL; rb=rb->rbnext) { if (rb->restype==-3) break; }
                if (!rb) {
                    sprintf(SDS_string,ResourceString(IDC_ENTITYDATA_NO_DATA_WITH__5, "No data with application name \"%s\" associated with entity." ),appname);
					MessageBox(IcadSharedGlobals::GetMainHWND(),SDS_string,ResourceString(IDC_ENTITYDATA_MESSAGE_7, "Message" ),MB_OK);
                    LoopFlag=TRUE;
                }
				IC_RELRB(elist);
            }
            sds_ssfree(ss);
            if (sds_pmtssget(ResourceString(IDC_ENTITYDATA__NSELECT_ENT_11, "\nSelect entities to which Entity Data will be copied: " ),NULL,NULL,NULL,NULL,ss,0)!=RTNORM) {
                sds_retvoid(); return(RTERROR);
            }
        }
    }

	fi1 = 1; //this variable was uninitialized in v1.0
    rc=cmd_copyeed(ename,ss,appname);
	sds_ssfree(ss);
    if(flag) sds_rett(); else sds_retvoid();
	if (rc==0)return(RTNORM);
	else return(RTERROR);
}

/*||||||||||||||||||||||||||||||||||||
||		Rename EED					||
||||||||||||||||||||||||||||||||||||*/

short cmd_renameed(sds_name ss, char *oldapp, char *newapp, short mode) {
	//mode 1 = overwrite - new data w/like varname from oldapp will overwrite newapp
	//mode 0 = no overwrite - data under newapp takes precedence
    char          fs1[IC_ACADBUF],
                  fs2[IC_ACADBUF],
				  *fsp1,
				  *fsp2;
    sds_name      curr;
    struct resbuf *appso,
                  *appsn,
                  appnamo,
                  appnamn,
                  *holdn,
				  *holdo,
				  *holdtemp,
                  *tempo,
                  *tempn;

	short		  flag;

    long          fl1;

    oldapp=_strupr(oldapp);
    newapp=_strupr(newapp);

    appnamo.restype = RTSTR;
	appnamo.resval.rstring = oldapp;
    appnamo.rbnext = NULL;

    appnamn.restype = RTSTR;
	appnamn.resval.rstring = newapp;
    appnamn.rbnext = NULL;
	appso=appsn=NULL;

    for(fl1=0L; sds_ssname(ss,fl1,curr)==RTNORM; ++fl1) {
        if ((appso=sds_entgetx(curr,&appnamo))!=NULL) {
            for(tempo=appso; tempo->rbnext!=NULL; tempo=tempo->rbnext) {
                if (tempo->restype==1001) {
					holdo=tempo;
                    break;
                }
            }
            if (tempo->rbnext !=NULL) {
                if ((appsn=sds_entgetx(curr,&appnamn))!=NULL) {
                    for(tempn=appsn; tempn->rbnext!=NULL; tempn=tempn->rbnext) {
                        if (tempn->restype==1001) {
                            break;
                        }
                    }
                    if (tempn->rbnext!=NULL) {	//the app was already present on entity
						while ((tempn->rbnext != NULL) && (tempn->restype != 1000)) (tempn=tempn->rbnext);
						while ((tempo->rbnext != NULL) && (tempo->restype != 1000)) (tempo=tempo->rbnext);
						holdn=tempn;
						if(tempo && tempo->restype==1000){
							while (tempo != NULL) {
								strcpy(fs1,tempo->resval.rstring);
								fsp1=strchr(fs1,'='/*DNT*/);  //fs1 contains old variable name
								if (fsp1!= NULL){
									*fsp1=0;
									tempn=holdn;
									flag=0;
									while (tempn != NULL) {
										strcpy(fs2,tempn->resval.rstring);
										fsp2=strchr(fs2,'='/*DNT*/);   //fs2 contains new variable name
										if (fsp2!= NULL){
											*fsp2=0;
											if(strisame(fs1,fs2)){ //if we have same var in old and new apps
												flag=1;
												if(mode == 1){
													IC_FREE(tempn->resval.rstring);	//release new var if in overwrite mode
													tempn->resval.rstring= new char [strlen(tempo->resval.rstring)+1];													
													strcpy(tempn->resval.rstring,tempo->resval.rstring);
												}
												break;
											}
										}
									if (tempn->rbnext->restype == 1002) break;
									tempn=tempn->rbnext;
									}
									if (!flag) { //if we didn't find var in new app list, add it
										holdtemp=tempn->rbnext;
                                        tempn->rbnext=sds_newrb(RTSTR);
										tempn=tempn->rbnext;
										tempn->rbnext=holdtemp;
										//IC_FREE(tempn->resval.rstring);
										tempn->resval.rstring= new char [strlen(tempo->resval.rstring)+1];
										tempn->restype=1000;
										strcpy(tempn->resval.rstring,tempo->resval.rstring);
									}
								}
								if (tempo->rbnext == NULL || tempo->rbnext->restype == 1002) break;
								tempo=tempo->rbnext;
							}
						}
                        sds_entmod(appsn);
					  	tempo=holdo->rbnext;
						holdo->rbnext=NULL;
                        sds_relrb(tempo);
                        sds_entmod(appso);
                    } else {
						IC_FREE(tempo->resval.rstring);
						tempo->resval.rstring= new char [strlen(newapp)+1];
						strcpy(tempo->resval.rstring,newapp);
                        sds_regapp(newapp);
                        if(sds_entmod(appso)== RTNORM){  //if we successfully put the data under the new app
							IC_FREE(tempo->resval.rstring); //remove old data
							tempo->resval.rstring= new char [strlen(oldapp)+1];
							strcpy(tempo->resval.rstring,oldapp);
                            sds_relrb(tempo->rbnext);
							tempo->rbnext=NULL;
                            if(sds_entmod(appso)!=RTNORM) cmd_ErrorPrompt(CMD_ERR_REMOVEEED,0);

						}else
                        	cmd_ErrorPrompt(CMD_ERR_UPDATEENT,0);
                    }
				IC_RELRB(appsn);
                }
            }
			IC_RELRB(appso);
		}
    }
	return(0);
 }


/*||||||||||||||||||||||||||||||||||||
||		Delete EED					||
||||||||||||||||||||||||||||||||||||*/

short cmd_deleed(sds_name ss, char *appname) {
    char           fs1[IC_ACADBUF];
    sds_name       curr;
    struct resbuf *apps,
                   appnam,
                  *temp,
                  *rbbeg,
                  *rbprv;
    long           fl1;

    strcpy(fs1,appname);
    appnam.restype = RTSTR;
    appnam.resval.rstring = fs1;
    appnam.rbnext = NULL;
	apps=NULL;

    for(fl1=0; sds_ssname(ss,fl1,curr)==RTNORM; ++fl1) {
        if ((apps=sds_entgetx(curr,&appnam))!=NULL) {
            // Search for the first application name.
            for(temp=apps;temp!=NULL && temp->restype!=1001;temp=temp->rbnext);
            // If application exists.
            if (temp==NULL) continue;
            for(rbbeg=temp,temp=temp->rbnext;temp!=NULL;temp=temp->rbnext) {
                if (temp->restype==1001) {
                    rbprv->rbnext=NULL;
                    sds_relrb(rbbeg->rbnext);
                    rbbeg->rbnext=temp;
                    rbbeg=rbbeg->rbnext;
                } else rbprv=temp;
            }
            if (rbbeg && rbbeg->rbnext) { sds_relrb(rbbeg->rbnext); rbbeg->rbnext=NULL;


/*
                if (temp->rbnext!=NULL) {
                    sds_relrb(temp->rbnext);
                    temp->rbnext=NULL;
                }
*/
                sds_entmod(apps);
            }
            IC_RELRB(apps);

        }
    }
    return(0);
}


void AttachOneEED(struct resbuf  *pTargetList , struct resbuf  **pTempSourceList , 
				  struct resbuf **pEndOfOrigionalTarget , bool bEndOfOrigionalTarget)
{
	struct resbuf  *pTempTargetList=NULL;

	for(pTempTargetList=pTargetList ; pTempTargetList->rbnext!=NULL ; pTempTargetList=pTempTargetList->rbnext);

	if(bEndOfOrigionalTarget==true)
	{
		*pEndOfOrigionalTarget=pTempTargetList;
	}

	pTempTargetList->rbnext=*pTempSourceList;
	pTempTargetList=pTempTargetList->rbnext;

	for(; pTempTargetList->rbnext!=NULL && pTempTargetList->rbnext->restype!=1001; pTempTargetList=pTempTargetList->rbnext);

	if(pTempTargetList->rbnext==NULL)
	{
		for(*pTempSourceList=(*pTempSourceList)->rbnext; *pTempSourceList!=NULL && (*pTempSourceList)->restype!=1001; *pTempSourceList=(*pTempSourceList)->rbnext);
		return;
            }
		
	for(*pTempSourceList=(*pTempSourceList)->rbnext; *pTempSourceList!=NULL && (*pTempSourceList)->restype!=1001; *pTempSourceList=(*pTempSourceList)->rbnext);
	pTempTargetList->rbnext=NULL ;

	return;

}

struct resbuf *AddInToExistingEED(struct resbuf *pTargetList , struct resbuf  *pSourceList , 
								  struct resbuf *pTargetApp ,  bool *bEndOfOrigionalTarget) 
{
	char msgStr[IC_ACADBUF] = "Overwrite ";
	char userChoice[IC_ACADBUF];

	struct resbuf  *pTempTargetList = NULL,
				   *pTempSourceList = NULL,
				   *rbbeg = NULL,
				   *rbprv = NULL,
				   *temp=NULL,
				   *pEndOfOrigionalTarget = NULL,
				   expert;
	
	bool  bAppFound =0,
		  bIsNotApp =0,
		  bAddNewEED=0,
		  bFlag=1;
	int	  nResult=0;

	for(pTempSourceList=pSourceList;pTempSourceList!=NULL;) 
	{	// If same data exists in pSourceApp and APP2
	
		bAppFound=0; // New Application not found
	
		for(pTempTargetList=pTargetList;pTempTargetList!=NULL;) 
		{
			if((pTempTargetList->restype == 1001) && 
				(pTempSourceList->restype == 1001) && 
				(pTempTargetList->resval.rstring!=NULL) && 
				(pTempSourceList->resval.rstring!=NULL) && 
				(strcmp(pTempSourceList->resval.rstring,pTempTargetList->resval.rstring)==0)) 
			{
				sprintf(msgStr,"%s %s %s",ResourceString(IDC_ENTITYDATA_OVERWRITE_AN_12,"Overwrite existing Entity Data "),pTempSourceList->resval.rstring," ? (Yes/No): ");
				SDS_getvar(NULL, DB_QEXPERT, &expert, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
			
				if(expert.resval.rint == 5 )
				{
					sds_initget(RSG_OTHER , ResourceString(IDC_ENTITYDATA_OVERWRITE_AN_13, "Yes No ~_Yes ~_No"));
					sds_getkword(msgStr,userChoice);
        }
				
				else			
					nResult = MessageBox(IcadSharedGlobals::GetMainHWND(),msgStr,"Message",MB_YESNO);
			
				if ((expert.resval.rint == 5 && strisame(userChoice, "NO") ) || nResult ==IDNO ) 				
				{
					for(pTempSourceList=pTempSourceList->rbnext ; pTempSourceList!=NULL && pTempSourceList->restype!=1001 ; pTempSourceList=pTempSourceList->rbnext);
					
					if(pTempSourceList==NULL) 
                            break;
					pTempTargetList = pTargetList;
					bAppFound = 0; // New Application not found
					continue;
                        }
			
				pTempTargetList=pTargetList;

				while(1) 
				{
					if(pTempTargetList->rbnext->restype == 1001)	
					{
						if(strcmp(pTempSourceList->resval.rstring,pTempTargetList->rbnext->resval.rstring)==0)
							break;
                    }
			
					pTempTargetList=pTempTargetList->rbnext;
						}
			
				for(rbbeg=pTempTargetList,pTempTargetList=pTempTargetList->rbnext->rbnext;pTempTargetList!=NULL;pTempTargetList=pTempTargetList->rbnext) 
				{
					if (pTempTargetList->restype==1001) 
					{
						rbprv->rbnext=NULL;
						sds_relrb(rbbeg->rbnext);
						rbbeg->rbnext=NULL;
						rbbeg->rbnext=pTempTargetList;
						rbbeg=rbbeg->rbnext;
						break;
                	}

					else
						rbprv=pTempTargetList;
            	}
      						
				if((pTempTargetList==NULL) && rbbeg && rbbeg->rbnext) 
				{ 
					sds_relrb(rbbeg->rbnext); 
					rbbeg->rbnext=NULL;
                    sds_entmod(pTargetApp);
            }

				bAddNewEED=0; // Overwrite the application
				
				AttachOneEED(pTargetList , &pTempSourceList , &pEndOfOrigionalTarget , bFlag);
				if(*bEndOfOrigionalTarget==true)
				{
					bFlag=false;
					*bEndOfOrigionalTarget=false;
        }
				bAppFound=1; 
				bIsNotApp=0; 
			
				if(pTempSourceList==NULL) break;
    }
		
			if((pTempSourceList!=NULL) && (pTempSourceList->restype != 1001)) bIsNotApp=1;
			if(pTempTargetList!=NULL) pTempTargetList=pTempTargetList->rbnext;
}

		if(pTempSourceList==NULL) break;
		if(bIsNotApp==1)
			for(pTempSourceList=pTempSourceList->rbnext; pTempSourceList!=NULL && pTempSourceList->restype!=1001; pTempSourceList=pTempSourceList->rbnext);

		// A new source application is found and not present in dest. 
		if(bAppFound==0 && bIsNotApp==0) 
	{
			bAddNewEED=1;  // Add new application

			AttachOneEED(pTargetList , &pTempSourceList , &pEndOfOrigionalTarget , bFlag);
			if(*bEndOfOrigionalTarget==true)
				{
					bFlag=false;
					*bEndOfOrigionalTarget=false;
				}
		}

		else
			bAppFound=0;
	}

	return pEndOfOrigionalTarget; 
}											


/*||||||||||||||||||||||||||||||||||||
||		Copy EED					||
||||||||||||||||||||||||||||||||||||*/
short cmd_copyeed(sds_name ename, sds_name ss, char *appname) 
{
    char          strApp[IC_ACADBUF];

   struct resbuf  *pSourceApp  = NULL,
                  *pTargetApp  = NULL,
                  applicationName,
                  *pTempList   = NULL,
				  *pSourceList = NULL,
				  *pEndOfOrigionalTarget = NULL,
				  *pTargetList = NULL,
                  *pTempSourceList = NULL;
    sds_name      entity;
    long          targetCount;
	bool bEndOfOrigionalTarget=true;

    strcpy(strApp,appname);
    applicationName.restype = RTSTR;
    applicationName.resval.rstring = strApp;
    applicationName.rbnext = NULL;
	pSourceApp=pTargetApp=NULL;

    if ((pSourceApp=sds_entgetx(ename,&applicationName))!=NULL)
		{
        for(pTempSourceList=pSourceApp; pTempSourceList!=NULL; pTempSourceList=pTempSourceList->rbnext) 	
			{
            if (pTempSourceList->restype==-3) 	
				{
                break;
	            }
	        }
        
		if (pTempSourceList!=NULL) 
				{
          pSourceList=pTempSourceList->rbnext;

		  strcpy(applicationName.resval.rstring,"*");

          for(targetCount=0; sds_ssname(ss,targetCount,entity)==RTNORM; ++targetCount) 
					{
				bEndOfOrigionalTarget=true;

                if((pTargetApp=sds_entgetx(entity,&applicationName))!=NULL) 
						{
                    for(pTargetList=pTargetApp; pTargetList->rbnext!=NULL; pTargetList=pTargetList->rbnext) 
							{
                        if (pTargetList->rbnext->restype==-3) 
								{
						   pTargetList=pTargetList->rbnext;
                            break;
	                        }
		                }
	                if (pTargetList->rbnext!=NULL) 
						{
							pEndOfOrigionalTarget=AddInToExistingEED(pTargetList,pSourceList,pTargetApp,&bEndOfOrigionalTarget);
							bEndOfOrigionalTarget=false;
		        	    } 

					else 
						{  //app data doesn't exist on target entity, make new -3 and 1001 markers
						pTempSourceList=pSourceList;
                	    for(pTargetList=pTargetApp; pTargetList->rbnext!=NULL; pTargetList=pTargetList->rbnext);
			
                        pTargetList->rbnext=sds_newrb(-3);
                       	pTargetList=pTargetList->rbnext;

                        pTargetList->rbnext=sds_newrb(1001);
    	                pTargetList=pTargetList->rbnext;

        	            pTargetList->resval.rstring=(char *)calloc(1,strlen(pTempSourceList->resval.rstring)+1); // It seems like sizeof is returning 1 less then the string length.
            	        strncpy(pTargetList->resval.rstring,pTempSourceList->resval.rstring,strlen(pTempSourceList->resval.rstring)+1);
                    	
						pTargetList->rbnext=pTempSourceList->rbnext;
						pEndOfOrigionalTarget=pTargetList;
						for(pTempSourceList=pTempSourceList->rbnext; pTempSourceList!=NULL && pTempSourceList->restype!=1001; pTempSourceList=pTempSourceList->rbnext);
						}
			
                    sds_entmod(pTargetApp);
					sds_redraw(entity,IC_REDRAW_DRAW);
					if(pEndOfOrigionalTarget!=NULL)
						{
						pEndOfOrigionalTarget->rbnext=NULL;
		        }
				
					pTargetList=NULL;
					sds_relrb(pTargetList);
        	        IC_RELRB(pTargetApp);
				}
				}
		    }
	
        IC_RELRB(pSourceApp);
	    }
    return(0);
	}

/*||||||||||||||||||||||||||||||||||||
||		Move EED					||
||||||||||||||||||||||||||||||||||||*/
// Bugzilla::57684; 1-09-03 
short cmd_moveeed(sds_name ename, sds_name ss, char *appname) 
	{

	sds_name ss1;
	cmd_copyeed( ename, ss, appname);
	sds_ssadd(ename,NULL,ss1);
	cmd_deleed(ss1,appname);
	sds_ssfree(ss1);

		return(0);
	}
