//
// Contents extracted from example files at
// http://support.microsoft.com/support/kb/articles/Q230/6/89.asp
//
/***************************************************************************
 * COMADDIN.H
 *
 * Common Declarations for Office 2000 COM Addins. 
 *
 *  Copyright (c)1999 Microsoft Corporation, All Rights Reserved
 *  Microsoft Product Support Services, Developer Support Office Integration
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 *  EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 ***************************************************************************/

#ifndef _IDTEXTENSIBILITY2_H
#define _IDTEXTENSIBILITY2_H

#ifndef STRICT
#define STRICT
#endif

#include <windows.h>
#include <ocidl.h>
#include <tchar.h>

//
// Enums used by IDTExtensibility2
//
enum ext_ConnectMode {
    ext_cm_AfterStartup = 0,
    ext_cm_Startup = 1,
    ext_cm_External = 2,
    ext_cm_CommandLine = 3
};

enum ext_DisconnectMode {
    ext_dm_HostShutdown = 0,
    ext_dm_UserClosed = 1
};

//
// Definition of IDTExtensibility2
//
static const GUID IID_IDTExtensibility2 = 
    {0xB65AD801L,0xABAF,0x11D0,{0xBB,0x8B,0x00,0xA0,0xC9,0x0F,0x27,0x44}};

DECLARE_INTERFACE_(IDTExtensibility2, IDispatch)
{
    STDMETHOD(OnConnection)(THIS_ IDispatch FAR* Application, ext_ConnectMode ConnectMode,
                IDispatch FAR* AddInInst, LPSAFEARRAY FAR* custom) PURE;
    STDMETHOD(OnDisconnection)(THIS_ ext_DisconnectMode RemoveMode, LPSAFEARRAY FAR* custom) PURE;
    STDMETHOD(OnAddInsUpdate)(THIS_ LPSAFEARRAY FAR* custom) PURE;
    STDMETHOD(OnStartupComplete)(THIS_ LPSAFEARRAY FAR* custom) PURE;
    STDMETHOD(OnBeginShutdown)(THIS_ LPSAFEARRAY FAR* custom) PURE;
};

struct __declspec(uuid("{B65AD801-ABAF-11D0-BB8B-00A0C90F2744}"))
	IDTExtensibility2;

#endif //_IDTEXTENSIBILITY2_H
