//*********************************************************************
//*  sds_printf.CPP                                                   *
//*  Copyright (C) 1997, 1998 by Visio Corporation                    *
//*                                                                   *
//*********************************************************************
//** Misc Functions that can all be stand alone 
//*********************************************************************

//** Includes

#include "Icad.h"/*DNT*/
#include "IcadApi.h"
#include "sds_engine.h" /*DNT*/
#include "sdsapplication.h"/*DNT*/

//** Functions
// *****************************************
// This is an SDS External API
//
// Because of varargs there is no sdsengine_ version
// of this.
//
// SPECIAL
//
// TODOTHREAD -- figure out if this is anywhere near thread safe
//
int 
sds_printf(const char *szPrintThis, ...) 
	{
	SDSAPI_Entry();

	char *astr = NULL,*thestr = NULL, *tmpstr = NULL;
	char fs1[IC_ACADBUF];
    va_list arg_ptr;
	int ret=RTNORM;
    
    va_start(arg_ptr, szPrintThis);

    if(-1==_vsnprintf(fs1,sizeof(fs1)-1,szPrintThis,arg_ptr)) 
		{
		int len=IC_ACADBUF*2;
		astr=new char[len];
		while(-1==_vsnprintf(astr,len-1,szPrintThis,arg_ptr)) 
			{
			len+=IC_ACADBUF;
			delete [] astr;
			if(len>(IC_ACADBUF*10)) 
				{
				return(RTERROR);
				}
			astr=new char[len];
			}
		thestr=astr;
		} 
	else 
		{
		thestr=fs1;
		//Bugzilla No. 78261; 09-09-2002 [		
		if( strchr(thestr, '|') != NULL )
			{
			char tempBuff[IC_ACADBUF];

			lstrcpy( tempBuff, thestr );
			ChangeSlashToBackslashAndRemovePipe( tempBuff );

			if ( (_access( tempBuff, 0 )) != -1  )
				ChangeSlashToBackslashAndRemovePipe( thestr );
			}
		//Bugzilla No. 78261; 09-09-2002 ]
		}
    va_end(arg_ptr);


	#if defined(SDS_SDSSTANDALONE)
	// **************************************
		HINSTANCE hInstLib;
		static int (*LibFunc)(const char *szPrintThis, ...);

		if(LibFunc==NULL) 
			{
			if((hInstLib=GetModuleHandle("icad.exe"/*DNT*/))==NULL) 
				{
				goto out;
				}
			if((LibFunc=(int (*)(const char *szPrintThis, ...))
				{
				GetProcAddress(hInstLib,"sds_printf"/*DNT*/))==NULL) goto out;
				}
			}

		// wonder if this really works with varargs????
		//
		ret=(*LibFunc)(thestr);

		if(ret==RTERROR) 
			{
			goto out;
			}

		if(astr) 
			{
			delete [] astr;
			}

		return(RTNORM);

		out: ;
		MessageBox(NULL,thestr,ResourceString(IDC_SDS_PRINTF_MESSAGE_0, "Message" ),MB_OK);
		if(astr) 
			{
			delete [] astr;
			}
		return(RTNORM);
	// **************************************
	#else
	// **************************************
		int fi1;
		char *fcp1,ch1;

		if(SDS_CMainWindow->m_hWnd==NULL) 
			{
			if(astr) 
				{
				delete astr;
				}
			return(RTERROR);
			}

		if(NULL!=(fcp1=strrchr(thestr,'\n'))) 
			{
			ch1=*(fcp1+1); 
			*(fcp1+1)=0;

			::SendMessage( IcadSharedGlobals::GetMainHWND(), ICAD_WNDACTION_PRINTF, NULL, (LPARAM)thestr );

			if(ch1) 
				{
				SDS_CMainWindow->m_bPrintHoldFill=TRUE;
				}
			
			// Too weird !!!  What does this do???
			//
			for(fi1=0; fi1<SDSApplication::GetActiveApplication()->GetCommandDepth(); fi1++)
				{
				tmpstr = "";
				::SendMessage( IcadSharedGlobals::GetMainHWND(), ICAD_WNDACTION_PRINTF, NULL, (LPARAM)tmpstr );
				}

			*(fcp1+1)=ch1;
			if(ch1) 
				{
				SDS_CMainWindow->m_bPrintHoldFill=FALSE;
				}

			tmpstr = fcp1 + 1;

			::SendMessage( IcadSharedGlobals::GetMainHWND(), ICAD_WNDACTION_PRINTF, NULL, (LPARAM)tmpstr );
			} 
		else 
			{
			::SendMessage( IcadSharedGlobals::GetMainHWND(), ICAD_WNDACTION_PRINTF, NULL, (LPARAM)thestr );
			}
		
		if(astr) 
			{
			delete astr;
			}

		return(RTNORM);
	#endif


	if(astr) 
		{
		delete astr;
		}

	return(ret);
	}

