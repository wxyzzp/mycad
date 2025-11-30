/* LIB\SDS\SDS_COMMAND.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Command Line Functions
 * 
 */ 

//** Includes
#include "Icad.h"/*DNT*/
#include "IcadApi.h"
#include "sdsthread.h" /*DNT*/
#include "sdsapplication.h"/*DNT*/
#include "threadcontroller.h"/*DNT*/
#include "lisp.h"/*DNT*/
#include "commandqueue.h" /*DNT*/
//#include "cmds.h"/*DNT*/
//#include <malloc.h>

// Modified Cybage PP 22/11/2000 [
// Reason: Fix for bug no. 47103
#include "commandatom.h"
// Modified Cybage PP 22/11/2000 ]

extern bool	  SDS_SetAUserCallback;

/****************************************************************************/
/*********************** Command Line Functions *****************************/
/****************************************************************************/

// *****************************************
// This is an SDS External API
//
int 
sdsengine_getcname(const char *szOtherLang, char **pszEnglish) 
	{
/*D.G.*/// Enabled for DBCS.

	// Modified Cybage PP 22/11/2000 [
	// Reason: Fix for bug no. 47103

	// db_astrncpy(pszEnglish,(char *)szOtherLang,IC_ACADBUF);
	if( szOtherLang == NULL ) return RTERROR ;

	class commandAtomObj *pCommandAtom = NULL;
	pCommandAtom = lsp_findatom(szOtherLang, 4) ;
	if( pCommandAtom == NULL ) return RTERROR ;

	if( pszEnglish != NULL )
	{
		delete [] *pszEnglish ;
		*pszEnglish = NULL ;
	}

	char lname[60] , gname[60] , sname[60] ;
	strcpy( lname , pCommandAtom->LName) ;
	strcpy( gname , pCommandAtom->GName) ;
	strcpy( sname , szOtherLang) ;

	if( strcmp( _tcsupr(sname), lname))
	{ // User's string is identical with GName.
		*pszEnglish = new char [ strlen(lname) + 1 ] ;
		strcpy(*pszEnglish, lname) ;
	}
	else
	{ // User's string is identical with LName.
		*pszEnglish = new char [ strlen(gname) + 1 ] ;
		strcpy(*pszEnglish, gname) ;
	}
	// Modified Cybage PP 22/11/2000 ]

	return(RTNORM);
	}


// *****************************************
// This is an SDS External API
//
// This is IntelliCAD specific (no AutoCAD version)
//
// TODOTHREAD -- this has to change dramatically
// 
int 
sdsengine_setcallbackfunc(int (*cbfnptr)(int flag,void *arg1,void *arg2,void *arg3)) 
	{
	if(cbfnptr==NULL) 
		{
		return(RTERROR);
		}
    DWORD dwCurThreadID = GetCurrentThreadId();
    for(SDSApplication *pcurApp=SDSApplication::GetEngineApplication(); pcurApp!=NULL; pcurApp=pcurApp->GetNextLink()) 
		{
        if(pcurApp->GetMainThread()->IsSame( dwCurThreadID ) ) 
			break;
	    }
    if(pcurApp==NULL) 
		{
		return(RTERROR);
		}
	pcurApp->SetCallBack( cbfnptr );
	SDS_SetAUserCallback=TRUE;	
	return(RTNORM);
	}


// *****************************************
// This is an SDS External API
//
int sdsengine_cmd(const struct sds_resbuf *prbCmdList) 
	{
	int res = RTNORM;


	// Basically, we can only call command on one of "our" threads.  The following line
	// looks up the current thread in our list of apps.  If it isn't there, it is either the
	// UI thread or one the user made up.  Return error
	//
	if ( SDSApplication::GetCurrentApplication() == NULL )
		{
		return RTERROR;
		}

	if ( IDS_IsIcadExiting() )
		{
		return RTERROR;
		}


	ASSERT( OnEngineThread() );

	if(IDS_GetThreadController()->GetInsideCmdFunction()) 
		{
		return(RTERROR);
		}


	struct resbuf *rbt=(struct resbuf *)prbCmdList,*ttrb;
	IDS_GetThreadController()->SetInsideCmdFunction( true );

    // Handle the sds_command(0); and Lisp ("command" nil nil ... )
    if( rbt==NULL || 
		rbt->restype==RTNIL ) 
		{ 
		SDS_CancelAllPending(0);
		rbt=sds_newrb(RTCAN);
		} 
	else 
		{
		// Link in the new commands make a copy too.
		rbt=sds_newrb(0);
		lsp_copysplrb(rbt,prbCmdList);
		}
	// Convert spaces to the spceial SDS_USERSPACE for the text command.
	for(ttrb=rbt; ttrb!=NULL; ttrb=ttrb->rbnext) 
		{
		if(ttrb->restype==RTSTR && ttrb->resval.rstring[0]==' ' && ttrb->resval.rstring[1]==0) 
			{
			ttrb->resval.rstring[0]=SDS_USERSPACE;
			}
		}

//	GetActiveCommandQueue()->AddItem( rbt );
	CommandQueue saveQueue;
	saveQueue.Copy( GetActiveCommandQueue() );
	GetActiveCommandQueue()->Flush();
	GetActiveCommandQueue()->AddItem( rbt );

	if(!IDS_GetThreadController()->GetDontThreadSwapCmd()) 
		{
		SDSApplication::GetActiveApplication()->SetCommandRunning( true );
		// ****************************************
		// THREADSWAP !!!!!!!!!!!!!!!!!
			{
			SDSApplication *pStartApp = SDSApplication::GetActiveApplication();
			SDSApplication *pStopApp = SDSApplication::GetActiveApplication();
			SDS_ThreadSwap( pStartApp->GetCommandThread(), pStopApp->GetMainThread());

			// I added this verification to make possible to return error status to the caller
			// if error took place while executing a command.
			if( pStartApp->getCmdThreadError() )
				{
				pStartApp->setCmdThreadError( false );
				res = RTCAN;
				}
			}
		}
	
	GetActiveCommandQueue()->Concat( &saveQueue );

	IDS_GetThreadController()->SetDontThreadSwapCmd( false );
	IDS_GetThreadController()->SetInsideCmdFunction( false );

    return res;
	}

// *****************************************
// This is an SDS External API
//
// This API is thread safe in itself and calls
// sds_cmd() rather than sdsengine_cmd() so that it can be called
// from outside the engine
//
// !!!! Because of varargs, there is no sdsengine_ version of this
// function
//
int sds_command(int nRType, ...) 
	{


    char fs1[IC_ACADBUF],*fcp1=NULL;
    struct sds_resbuf *beg,*cur,*tmp;
	sds_real *rp;
	long *lp;
    va_list argptr;
    sds_point pt1;
    int nexttype;

   	// Basically, we can only call command on one of "our" threads.  The following line
	// looks up the current thread in our list of apps.  If it isn't there, it is either the
	// UI thread or one the user made up.  Return error
	//
	if ( SDSApplication::GetCurrentApplication() == NULL )
		{
		return RTERROR;
		}


    beg=cur=tmp=NULL;
    va_start(argptr,nRType);
    nexttype=nRType;
	
	if( nexttype==0 || nexttype==NULL ) 
		{
		// DO NOT call sdsengine_cmd here, needs to make engine call
		//
		return(sds_cmd(NULL));
		}
		
    do 
		{
        if((tmp=sds_newrb(nexttype))==NULL) 
			{
            IC_RELRB(beg); return(RTERROR);
	        }
        if(beg==NULL) 
			{
            beg=cur=tmp;
			} 
		else 
			{
            cur=cur->rbnext=tmp;
	        }
        switch(nexttype) 
			{
            case RTDXF0:
            case RTVOID:
            case RTDOTE:
                break;
            case RTREAL:
            case RTANG:
            case RTORINT:
                cur->resval.rreal=va_arg(argptr,sds_real);
                break;
            case RTPOINT:                
				rp=va_arg(argptr,sds_pointp);
				ic_ptcpy(pt1,rp);
                cur->resval.rpoint[SDS_X]=pt1[SDS_X]; 
                cur->resval.rpoint[SDS_Y]=pt1[SDS_Y];
                cur->resval.rpoint[SDS_Z]=0.0;
                break;
            case RT3DPOINT:
				rp=va_arg(argptr,sds_pointp);
				ic_ptcpy(cur->resval.rpoint,rp);
                break;
            case RTSHORT:
                cur->resval.rint=va_arg(argptr,short);
                break;
            case RTSTR:
                if((fcp1=va_arg(argptr,char *))==NULL) 
					{
                    cur->resval.rstring=NULL; 
					break;
	                }
                strncpy(fs1,fcp1,sizeof(fs1)-1);
                cur->resval.rstring= new char [strlen(fs1)+1];
                strcpy(cur->resval.rstring,fs1);
                break;
            case RTLB:
                cur->resval.rstring=new char [sizeof(char)+1];
                strcpy(cur->resval.rstring,"("/*DNT*/);
                break;
            case RTLE:
                cur->resval.rstring=new char [sizeof(char)+1];
                strcpy(cur->resval.rstring,")"/*DNT*/);
                break;
            case RTENAME:
				lp=va_arg(argptr,long *);
                ic_encpy(cur->resval.rlname,lp);
                break;
            case RTPICKS:
				lp=va_arg(argptr,long *);
                ic_encpy(cur->resval.rlname,lp);
                break;
            case RTLONG:
                cur->resval.rlong=va_arg(argptr,long);
                break;
            case RTT:
                cur->resval.rstring= new char [ sizeof(char)+1];
                strcpy(cur->resval.rstring,"t"/*DNT*/);
                break;
            case RTNIL:
                cur->resval.rstring= new char [ sizeof(char)+3];
                strcpy(cur->resval.rstring,"nil"/*DNT*/);
                break;
            default: 
				return(RTERROR);
	        }        
	    } 
		while((nexttype=va_arg(argptr,int))!=0 && 
			   nexttype!=RTNONE);

    va_end(argptr);

	// DO NOT call sdsengine_cmd() here
	// must make engine call
	//
    int ret=sds_cmd(beg);

	sds_relrb(beg);

	return(ret);
	}

