/* D:\DEV\PRJ\ICAD\ICADCOMMANDLINEINFO.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

// IcadCommandLineInfo.cpp : implementation file
//

#include "Icad.h"/*DNT*/
#include "IcadApp.h"/*DNT*/
#include "IcadCommandLineInfo.h"/*DNT*/

#ifdef _DEBUG

    #if !defined(USE_SMARTHEAP)
    #define new DEBUG_NEW
    #endif

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIcadCommandLineInfo

CIcadCommandLineInfo::CIcadCommandLineInfo()
{
    m_strAltPath.Empty();
}

CIcadCommandLineInfo::~CIcadCommandLineInfo()
{
}

CString CIcadCommandLineInfo::VerifyExtension(LPCTSTR pszFileName, LPCTSTR pszExt)
{
//*** This function adds the extension specified in pszExt if an extension is not
//*** already present in pszFileName and returns the resulting string.  If an
//*** extension is present in pszFileName then the function returns pszFileName
//*** unchanged.
    CString strRet;
    char fname[IC_PATHSIZE+IC_FILESIZE];
    strncpy(fname,pszFileName,sizeof(fname)-1);
    char* pFileNameOnly;
    if(NULL==(pFileNameOnly=strrchr(fname,'\\')))
    {
        pFileNameOnly=fname;
    }
    if(NULL==strrchr(pFileNameOnly,'.'))
    {
        ic_setext(fname,(char*)pszExt);
    }
    strRet=fname;
    return strRet;
}

void CIcadCommandLineInfo::ParseParam(LPCTSTR lpszParam, BOOL bFlag, BOOL bLast)
{
	/*DG - 13.5.2003*/// Allow spaces after P and S switches.
	// Note, empty params after these switches are allowed here too. Caller will decide if it's allowed for icad at all.
	static bool	s_bSpaceAfterS = false, s_bSpaceAfterP = false;

    if(bFlag)
    {
        //*** Flags
		// if we are here and one of these booleans is true, it means the previous switch had empty param
		s_bSpaceAfterS = s_bSpaceAfterP = false;
        if(lpszParam && toupper(*lpszParam)=='S')
        {
            m_strOverridePath=lpszParam+1;
			if(m_strOverridePath.IsEmpty())
				s_bSpaceAfterS = true;
        }
        // SystemMetrix(Maeda)-[09.10.2001
        else if(lpszParam && toupper(*lpszParam)=='P')  // change profile name option
        {
            m_strOverrideProfileName=lpszParam+1;       // profile name for registry
			if(m_strOverrideProfileName.IsEmpty())
				s_bSpaceAfterP = true;
        }
        // ]-SystemMetrix(Maeda) 09.10.2001
        else if(lpszParam && toupper(*lpszParam)=='B')
        {
            m_nOpenCommand=CIcadCommandLineInfo::OpenScript;
        }
        else if(lpszParam && toupper(*lpszParam)=='L') //*** This was changed from 'B' so we could use 'B' for launching script files like r14.
        {
            m_bShowSplash=FALSE;
        }
        else
        {
            CCommandLineInfo::ParseParamFlag(lpszParam);
        }
    }
    else
    {
        //*** Not a flag
		if(s_bSpaceAfterS)
		{
			s_bSpaceAfterS = false;
			m_strOverridePath = lpszParam;
			return;
		}
		if(s_bSpaceAfterP)
		{
			s_bSpaceAfterP = false;
			m_strOverrideProfileName = lpszParam;
			return;
		}

		CString strParam(lpszParam);
		if(0==strParam.CompareNoCase("REGSERVER"/*DNT*/))
		{
			m_nOpenCommand=RegServer;
			return;
		}
		else if(0==strParam.CompareNoCase("UNREGSERVER"/*DNT*/))
		{
			m_nOpenCommand=UnRegServer;
			return;
		}

        if(m_nOpenCommand==OpenScript)
        {
            m_strAltPath=VerifyExtension(lpszParam,ResourceString(DNT_ICADCOMMANDLINEINFO_SCR_0, "scr" ));
        }
        else if(m_nShellCommand==FilePrint)
        {
            m_strFileName=VerifyExtension(lpszParam,ResourceString(DNT_ICADCOMMANDLINEINFO_DWG_1, "dwg" ));
        }
        else if(m_nShellCommand==FilePrintTo && m_strFileName.IsEmpty())
        {
            m_strFileName=lpszParam;
        }
        else if(m_nShellCommand==FilePrintTo && m_strPrinterName.IsEmpty())
        {
            m_strPrinterName=lpszParam;
        }
        else if(m_nShellCommand==FilePrintTo && m_strDriverName.IsEmpty())
        {
            m_strDriverName=lpszParam;
        }
        else if(m_nShellCommand==FilePrintTo && m_strPortName.IsEmpty())
        {
            m_strPortName=lpszParam;
        }
        else if(*lpszParam=='.')
        {
            m_nShellCommand=FileNew;
        }
        else if(lpszParam)
        {
            CString strFile;
            strFile=VerifyExtension(lpszParam,ResourceString(DNT_ICADCOMMANDLINEINFO_DWG_1, "dwg" ));
            char* pstr;
            if((pstr=strrchr(strFile,'.'))!=NULL && strisame(ResourceString(DNT_ICADCOMMANDLINEINFO_DWG_2, "DWG" ),pstr+1))
            {
                m_nShellCommand=FileOpen;
                m_strFileName=strFile;
            }
            else
            {
                m_strAltPath=lpszParam;
            }
        }
    }

//*** Don't call the base class function, it's easier to just do it all ourselves
//  int fb1=m_bShowSplash;
//  CCommandLineInfo::ParseParam(lpszParam,bFlag,bLast);
//  m_bShowSplash=fb1;
}
