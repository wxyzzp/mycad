/* C:\ICADDEV\PRJ\ICAD\ICADCOMMANDLINEINFO.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

//////////////////////////////////////////////////////////////////////
// Icad Command line class
#ifndef ICADCOMMANDLINEINFOINCLUDED
#define ICADCOMMANDLINEINFOINCLUDED


class CIcadCommandLineInfo : public CCommandLineInfo {

public:
    CIcadCommandLineInfo();
    ~CIcadCommandLineInfo();

public:
    CString  m_strOverridePath;
    CString  m_strOverrideProfileName;  // for registry name SystemMetrix(Maeda) 09.10.2001
    CString  m_strOverrideSplash;
    CString  m_strAltPath; //*** This string contains an alternate to .dwg file to open on startup

    enum { OpenNothing,OpenScript,RegServer,UnRegServer } m_nOpenCommand;

    CString VerifyExtension(LPCTSTR pszFileName, LPCTSTR pszExt);

protected:
    void ParseParam(LPCTSTR lpszParam, BOOL bFlag, BOOL bLast);
};

#endif


