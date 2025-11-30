/* ICAD\CWINTAB.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Implementation of WinTab helper classes.
 * Originally Authored by Eric Vandewater of CalComp
 * 
 */ 
// THIS FILE HAS BEEN GLOBALIZED!

// Project includes

#include "icad.h"/*DNT*/
#include "IcadDoc.h"
#include "IcadView.h"
#include "CWinTab.h"/*DNT*/
#include "gr.h"/*DNT*/
#include "inputevent.h" /*DNT*/
#include "icadgridiconsnap.h"

extern sds_point SDS_PreGridPoint;
extern int SDS_AtCmdLine;
extern int SDS_AtEntGet;

// WinTab function pointer type definitions

extern "C"/*DNT*/ {
typedef UINT  (WINAPI *WCM_INFO)(UINT wCategory, UINT nIndex, LPVOID lpOutput);
typedef HCTX  (WINAPI *WCM_OPEN)(HWND hWnd, LPLOGCONTEXT lpLogCtx, BOOL fEnable);
typedef BOOL  (WINAPI *WCM_CLOSE)(HCTX hCtx);
typedef int   (WINAPI *WCM_PACKETSGET)(HCTX hCtx, int cMaxPkts, LPVOID lpPkts);
typedef BOOL  (WINAPI *WCM_PACKET)(HCTX hCtx, UINT wSerial, LPVOID lpPkt);
typedef BOOL  (WINAPI *WCM_ENABLE)(HCTX hCtx, BOOL fEnable);
typedef BOOL  (WINAPI *WCM_OVERLAP)(HCTX hCtx, BOOL fToTop);
typedef BOOL  (WINAPI *WCM_GET)(HCTX hCtx, LPLOGCONTEXT lpLogCtx);
typedef BOOL  (WINAPI *WCM_SET)(HCTX hCtx, LPLOGCONTEXT lpLogCtx);
typedef BOOL  (WINAPI *WCM_SAVE)(HCTX hCtx, LPVOID lpSaveInfo);
typedef HCTX  (WINAPI *WCM_RESTORE)(HWND hWnd, LPVOID lpSaveInfo, BOOL fEnable);
typedef int   (WINAPI *WCM_PACKETSPEEK)(HCTX hCtx, int cMaxPkts, LPVOID lpPkts);
typedef int   (WINAPI *WCM_DATAGET)(HCTX hCtx, UINT wBegin, UINT wEnd, int cMaxPkts, LPVOID lpPkts, LPINT lpNPkts);
typedef int   (WINAPI *WCM_DATAPEEK)(HCTX hCtx, UINT wBegin, UINT wEnd, int cMaxPkts, LPVOID lpPkts, LPINT lpNPkts);
typedef DWORD (WINAPI *WCM_QUEUEPACKETS)(HCTX hCtx);
typedef BOOL  (WINAPI *WCM_QUEUEPACKETSEX)(HCTX hCtx, UINT FAR* lpOld, UINT FAR* lpNew);
typedef int   (WINAPI *WCM_QUEUESIZEGET)(HCTX hCtx);
typedef BOOL  (WINAPI *WCM_QUEUESIZESET)(HCTX hCtx, int nPkts);
typedef HMGR  (WINAPI *WCM_MGROPEN)(HWND hWnd, UINT wMsgBase);
typedef BOOL  (WINAPI *WCM_MGRCLOSE)(HMGR hMgr);
typedef BOOL  (WINAPI *WCM_MGRCONTEXTENUM)(HMGR hMgr, WTENUMPROC lpEnumFunc, LPARAM lParam);
typedef HWND  (WINAPI *WCM_MGRCONTEXTOWNER)(HMGR hMgr, HCTX hCtx);
typedef HCTX  (WINAPI *WCM_MGRDEFCONTEXT)(HMGR hMgr, BOOL fSystem);
typedef BOOL  (WINAPI *WCM_MGREXT)(HMGR hMgr, UINT wExt, LPVOID lpData);
typedef BOOL  (WINAPI *WCM_MGRCSRENABLE)(HMGR hMgr, UINT wCursor, BOOL fEnable);
typedef BOOL  (WINAPI *WCM_MGRCSRBUTTONMAP)(HMGR hMgr, UINT wCursor, LPBYTE lpLogBtns, LPBYTE lpSysBtns);
typedef BOOL  (WINAPI *WCM_MGRCSRPRESSUREBTNMARKS)(HMGR hMgr, UINT wCsr, DWORD dwNMarks, DWORD dwTMarks);
typedef BOOL  (WINAPI *WCM_MGRCSRPRESSUREBTNMARKSEX)(HMGR hMgr, UINT wCsr, UINT FAR* lpNMarks, UINT FAR* lpTMarks);
typedef BOOL  (WINAPI *WCM_MGRCSRPRESSURERESPONSE)(HMGR hMgr, UINT wCsr, UINT FAR* lpNResp, UINT FAR* lpTResp);
}

// Local function prototypes

static UINT InitWinTab();
static BOOL GetFuncPtrs();

// WinTab function pointers to WinTab DLL

static WCM_INFO                         WcmInfoA;
static WCM_INFO                         WcmInfoW;
static WCM_OPEN                         WcmOpenA;
static WCM_OPEN                         WcmOpenW;
static WCM_CLOSE                        WcmClose;
static WCM_PACKETSGET                   WcmPacketsGet;
static WCM_PACKET                       WcmPacket;
static WCM_ENABLE                       WcmEnable;
static WCM_OVERLAP                      WcmOverlap;
static WCM_GET                          WcmGetA;
static WCM_GET                          WcmGetW;
static WCM_SET                          WcmSetA;
static WCM_SET                          WcmSetW;
static WCM_SAVE                         WcmSave;
static WCM_RESTORE                      WcmRestore;
static WCM_PACKETSPEEK                  WcmPacketsPeek;
static WCM_DATAGET                      WcmDataGet;
static WCM_DATAPEEK                     WcmDataPeek;
static WCM_QUEUEPACKETS                 WcmQueuePackets;
static WCM_QUEUEPACKETSEX               WcmQueuePacketsEx;
static WCM_QUEUESIZEGET                 WcmQueueSizeGet;
static WCM_QUEUESIZESET                 WcmQueueSizeSet;
static WCM_MGROPEN                      WcmMgrOpen;
static WCM_MGRCLOSE                     WcmMgrClose;
static WCM_MGRCONTEXTENUM               WcmMgrContextEnum;
static WCM_MGRCONTEXTOWNER              WcmMgrContextOwner;
static WCM_MGRDEFCONTEXT                WcmMgrDefContext;
static WCM_MGREXT                       WcmMgrExt;
static WCM_MGRCSRENABLE                 WcmMgrCsrEnable;
static WCM_MGRCSRBUTTONMAP              WcmMgrCsrButtonMap;
static WCM_MGRCSRPRESSUREBTNMARKS       WcmMgrCsrPressureBtnMarks;
static WCM_MGRCSRPRESSUREBTNMARKSEX     WcmMgrCsrPressureBtnMarksEx;
static WCM_MGRCSRPRESSURERESPONSE       WcmMgrCsrPressureResponse;

// Local parameters

static HINSTANCE mhWinTabLib = NULL;    // Handle of WinTab library
static int       mnRefCount  = 0;       // WinTab reference counter


//=============================================================================
//
//  Local function implementations
//
//=============================================================================


static BOOL GetFuncPtrs()
{
    // Load pointers to WinTab functions in library

    #define GETPTR(p,t,n)       p = (t)GetProcAddress(mhWinTabLib,(LPCSTR)(n)); \
                                if ( !p ) return FALSE;

    GETPTR(WcmInfoA, WCM_INFO,"WTInfoA"/*DNT*/);
    GETPTR(WcmInfoW, WCM_INFO,"WTInfoW"/*DNT*/);
    GETPTR(WcmOpenA, WCM_OPEN,"WTOpenA"/*DNT*/ );
    GETPTR(WcmOpenW, WCM_OPEN,"WTOpenW"/*DNT*/ );
    GETPTR(WcmClose, WCM_CLOSE,"WTClose"/*DNT*/ );
    GETPTR(WcmPacketsGet, WCM_PACKETSGET,"WTPacketsGet"/*DNT*/ );
    GETPTR(WcmPacket, WCM_PACKET,"WTPacket"/*DNT*/ );
    GETPTR(WcmEnable, WCM_ENABLE,"WTEnable"/*DNT*/ );
    GETPTR(WcmOverlap, WCM_OVERLAP,"WTOverlap"/*DNT*/ );
    GETPTR(WcmGetA, WCM_GET,"WTGetA"/*DNT*/ );
    GETPTR(WcmGetW, WCM_GET,"WTGetW"/*DNT*/ );
    GETPTR(WcmSetA, WCM_SET,"WTSetA"/*DNT*/ );
    GETPTR(WcmSetW, WCM_SET,"WTSetW"/*DNT*/ );
    GETPTR(WcmSave, WCM_SAVE,"WTSave"/*DNT*/ );
    GETPTR(WcmRestore, WCM_RESTORE,"WTRestore"/*DNT*/ );
    GETPTR(WcmPacketsPeek, WCM_PACKETSPEEK,"WTPacketsPeek"/*DNT*/ );
    GETPTR(WcmDataGet, WCM_DATAGET,"WTDataGet"/*DNT*/ );
    GETPTR(WcmDataPeek, WCM_DATAPEEK,"WTDataPeek"/*DNT*/ );
//    GETPTR(WcmQueuePackets, WCM_QUEUEPACKETS,"WTQueuePackets"/*DNT*/ ); this is commented out because it is never used.  It was preventing Kurta tablets from initializing
    GETPTR(WcmQueuePacketsEx, WCM_QUEUEPACKETSEX,"WTQueuePacketsEx"/*DNT*/ );
    GETPTR(WcmQueueSizeGet, WCM_QUEUESIZEGET,"WTQueueSizeGet"/*DNT*/ );
    GETPTR(WcmQueueSizeSet, WCM_QUEUESIZESET,"WTQueueSizeSet"/*DNT*/ );
    GETPTR(WcmMgrOpen, WCM_MGROPEN,"WTMgrOpen"/*DNT*/ );
    GETPTR(WcmMgrClose, WCM_MGRCLOSE,"WTMgrClose"/*DNT*/ );
    GETPTR(WcmMgrContextEnum, WCM_MGRCONTEXTENUM,"WTMgrContextEnum"/*DNT*/ );
    GETPTR(WcmMgrContextOwner, WCM_MGRCONTEXTOWNER,"WTMgrContextOwner"/*DNT*/ );
    GETPTR(WcmMgrDefContext, WCM_MGRDEFCONTEXT,"WTMgrDefContext"/*DNT*/ );
    GETPTR(WcmMgrExt, WCM_MGREXT,"WTMgrExt"/*DNT*/ );
    GETPTR(WcmMgrCsrEnable, WCM_MGRCSRENABLE,"WTMgrCsrEnable"/*DNT*/ );
    GETPTR(WcmMgrCsrButtonMap, WCM_MGRCSRBUTTONMAP,"WTMgrCsrButtonMap"/*DNT*/ );
    GETPTR(WcmMgrCsrPressureBtnMarks, WCM_MGRCSRPRESSUREBTNMARKS,"WTMgrCsrPressureBtnMarks"/*DNT*/ );
    GETPTR(WcmMgrCsrPressureBtnMarksEx, WCM_MGRCSRPRESSUREBTNMARKSEX,"WTMgrCsrPressureBtnMarksEx"/*DNT*/ );
    GETPTR(WcmMgrCsrPressureResponse, WCM_MGRCSRPRESSURERESPONSE, "WTMgrCsrPressureResponse"/*DNT*/ );

    return TRUE;
}

static UINT InitWinTab()
{
    // Load WinTab library

    mhWinTabLib = LoadLibrary("WinTab32.DLL"/*DNT*/);
    if ( !mhWinTabLib )
        return WTERR_WINTAB32NOTFOUND;

    // Load all function pointers

    if ( GetFuncPtrs() )
        return WTERR_NONE;

    // Free library on error

    FreeLibrary(mhWinTabLib);
    mhWinTabLib = NULL;

    return WTERR_WINTABINVALID;
}


//=============================================================================
//
//  CWinTab class implementation
//
//=============================================================================


CWinTab::CWinTab()
    : mwErrorCode(0)
{
    // Hook up to WinTab DLL on creation of first instance

    if ( mnRefCount++ == 0 )
        mwErrorCode = InitWinTab();
}

CWinTab::~CWinTab()
{
    // Unhook from WinTab DLL if no instances are left

    if ( --mnRefCount == 0 && mhWinTabLib )
    {
        FreeLibrary(mhWinTabLib);
        mhWinTabLib = NULL;
    }
}

WTPKT CWinTab::GetCsrDataItems(UINT wCursor)
{
    WTPKT PktData = 0;

    if ( mhWinTabLib )
        WcmInfoA(WTI_CURSORS+wCursor, CSR_PKTDATA, &PktData);

    return PktData;
}

BOOL CWinTab::GetCsrName(UINT wCursor, char* pName, int nSize)
{
    // Validate parameters

    if ( !mhWinTabLib || !pName || !nSize )
        return FALSE;

    // Read name into temp buffer

    WORD wLen = WcmInfoA(WTI_CURSORS+wCursor, CSR_NAME, NULL);
    char* pBuffer = new char[wLen];
    if ( !pBuffer )
        return FALSE;

    WcmInfoA(WTI_CURSORS+wCursor, CSR_NAME, (LPVOID)pBuffer);

    // Copy name to user buffer, truncate if neccessary

    strncpy(pName, pBuffer, nSize-1);
    pName[nSize-1] = '\0'/*DNT*/;

    delete [] pBuffer;

    return TRUE;
}

BOOL CWinTab::GetDeviceName(char* pName, int nSize)
{
    // Validate parameters

    if ( !mhWinTabLib || !pName || !nSize )
        return FALSE;

    // Read device name into temp buffer

    WORD wLen = WcmInfoA(WTI_DEVICES, DVC_NAME, NULL);
    char *pBuffer = new char[wLen];
    if ( !pBuffer )
        return FALSE;

    WcmInfoA(WTI_DEVICES, DVC_NAME, (LPVOID)pBuffer);

    // Name may start with WACOM's name to make the eraser work.
    // Skip over it if more data is present. Otherwise, replace
    // the fake terminator with a semicolon

    char* pInfo = pBuffer;
    if ( strncmp(pInfo,"WACOM Tablet"/*DNT*/ , 12) == 0 && wLen > 13 )
        pInfo += strlen(pInfo) + 1;
    else
        for ( WORD i = 0 ; i < wLen-1 ; i++ )
            if ( pInfo[i] == '\0'/*DNT*/)
            {
                pInfo[i] = ';'/*DNT*/;
                break;
            }
                
    // Copy device name to user buffer

    strncpy(pName, pInfo, nSize-1);
    pName[nSize-1] = '\0'/*DNT*/;

    delete [] pBuffer;

    return TRUE;
}

WORD CWinTab::GetDriverVersion()
{
    WORD wVersion = 0;

    if ( mhWinTabLib )
        WcmInfoA(WTI_INTERFACE, IFC_IMPLVERSION, &wVersion);

    return wVersion;
}

UINT CWinTab::GetLastError(char* pError, int nSize)
{
    if ( pError && nSize > 0 )
    {
        CString pMsg;

        switch ( mwErrorCode )
        {
            case WTERR_NONE:
                pMsg = ResourceString(IDC_CWINTAB_NO_ERRORS_35, "No Errors" );
                break;
            case WTERR_WINTAB32NOTFOUND:
                pMsg = ResourceString(IDC_CWINTAB_REQUIRED_FILE_W_36, "Required file WinTab32.DLL not found" );
                break;
            case WTERR_WINTABINVALID:
                pMsg = ResourceString(IDC_CWINTAB_INCOMPATIBLE_WI_37, "Incompatible WinTab32.DLL version" );
                break;
            default:
                pMsg =""/*DNT*/ ;
                break;
        }
        strncpy(pError, pMsg, nSize-1);
        pError[nSize-1] = '\0'/*DNT*/;
    }

    return mwErrorCode;
}

LONG CWinTab::GetMaxPressure()
{
    LONG lMaxPressure = 0;

    // Obtain pressure range info from WinTab

    if ( mhWinTabLib )
    {
        AXIS axPressure;
        WcmInfoA(WTI_DEVICES, DVC_NPRESSURE, &axPressure);
        lMaxPressure = axPressure.axMax;
    }

    return lMaxPressure;
}

UINT CWinTab::GetNbrCursorTypes()
{
    UINT wTypes = 0;

    if ( mhWinTabLib )
        WcmInfoA(WTI_DEVICES, DVC_NCSRTYPES, &wTypes);

    return wTypes;
}

WORD CWinTab::GetSpecVersion()
{
    WORD wVersion = 0;

    if ( mhWinTabLib )
        WcmInfoA(WTI_INTERFACE, IFC_SPECVERSION, &wVersion);

    return wVersion;
}

BOOL CWinTab::GetTabletSize(SIZE* pSize)
{
    AXIS axRangeX, axRangeY;

    BOOL bOk = mhWinTabLib                             &&
               WcmInfoA(WTI_DEVICES, DVC_X, &axRangeX) &&
               WcmInfoA(WTI_DEVICES, DVC_Y, &axRangeY);

    if ( bOk )
    {
        pSize->cx = axRangeX.axMax;
        pSize->cy = axRangeY.axMax;
    }

    return bOk;
}


//=============================================================================
//
//  CWinTabCsr class implementation
//
//=============================================================================


CWinTabCsr::CWinTabCsr(UINT wCursor)
{
    mhMgr    = WcmMgrOpen ? WcmMgrOpen(NULL, WT_DEFBASE) : 0;
    mwCursor = wCursor;
}

CWinTabCsr::~CWinTabCsr()
{
    if ( mhMgr && mhWinTabLib )
        WcmMgrClose(mhMgr);
}

BOOL CWinTabCsr::GetButtonMap(BYTE* pLogBtns, BYTE* pSysBtns)
{
    if ( mhWinTabLib )
    {
        if ( pLogBtns )
            WcmInfoA(WTI_CURSORS + mwCursor, CSR_BUTTONMAP, pLogBtns);
        if ( pSysBtns )
            WcmInfoA(WTI_CURSORS + mwCursor, CSR_SYSBTNMAP, pSysBtns);
    }

    return mhWinTabLib != NULL;
}

BOOL CWinTabCsr::GetButtonMarks(UINT* pwRelease, UINT* pwPress)
{
    if ( mhWinTabLib )
    {
        UINT wMarks[2];
        WcmInfoA(WTI_CURSORS + mwCursor, CSR_NPBTNMARKS, &wMarks);
        *pwRelease = wMarks[0];
        *pwPress   = wMarks[1];
    }

    return mhWinTabLib != NULL;
}

WTPKT CWinTabCsr::GetDataItems()
{
    WTPKT PktData = 0;

    if ( mhWinTabLib )
        WcmInfoA(WTI_CURSORS + mwCursor, CSR_PKTDATA, &PktData);

    return PktData;
}

BOOL CWinTabCsr::GetPressureCurve(UINT* pCurve)
{
    if ( mhWinTabLib )
        WcmInfoA(WTI_CURSORS + mwCursor, CSR_NPRESPONSE, pCurve);

    return WcmInfoA != NULL;
}

BOOL CWinTabCsr::SetButtonMap(BYTE* pLogBtns, BYTE* pSysBtns)
{
    // Select defaults for non-specified maps

    if ( !pLogBtns )
        pLogBtns = (BYTE*)WTP_LPDEFAULT;

    if ( !pSysBtns )
        pSysBtns = (BYTE*)WTP_LPDEFAULT;

    if ( mhMgr && mhWinTabLib )
        return WcmMgrCsrButtonMap(mhMgr, mwCursor, pLogBtns, pSysBtns);
    else
        return FALSE;
}

BOOL CWinTabCsr::SetButtonMarks(UINT wRelease, UINT wPress)
{
    UINT wMarks[] = { wRelease, wPress };

    // Select defaults if both release and pressure are zero

    UINT* pMarks = wRelease || wPress ? wMarks : (UINT*)WTP_LPDEFAULT;

    if ( mhMgr && mhWinTabLib )
        return WcmMgrCsrPressureBtnMarksEx(mhMgr, mwCursor, pMarks, wMarks);
    else
        return FALSE;
}

BOOL CWinTabCsr::SetPressureCurve(UINT* pCurve)
{
    if ( !pCurve )
        pCurve = (UINT*)WTP_LPDEFAULT;

    if ( mhMgr && mhWinTabLib )
        return WcmMgrCsrPressureResponse(mhMgr, mwCursor, pCurve, NULL);
    else
        return FALSE;
}


//=============================================================================
//
//  CWinTabCtx class implementation
//
//=============================================================================


CWinTabCtx::CWinTabCtx(BOOL bSystem)
    : mhCtx(NULL), mbOwner(FALSE), m_xformtype(XFORM_ORTHOGONAL), m_bTransformed(FALSE)
{
    // Get default context attributes

    if ( mhWinTabLib )
        WcmInfoA(bSystem ? WTI_DEFSYSCTX : WTI_DEFCONTEXT, 0, &mLogCtx);

	int i, j;
	for (i=0; i<4;i++)
		for (j=0;j<4;j++)
			{
			if (i==j)
				m_pXform[i][j] = 1.0;
			else
				m_pXform[i][j] = 0.0;
			}

}

CWinTabCtx::CWinTabCtx(HCTX hCtx)
    : mhCtx(hCtx), mbOwner(FALSE), m_xformtype(XFORM_ORTHOGONAL), m_bTransformed(FALSE)
{
    // Get attributes of specified context

    if ( mhCtx && mhWinTabLib )
        WcmGetA(hCtx, &mLogCtx);

	int i, j;
	for (i=0; i<4;i++)
		for (j=0;j<4;j++)
			{
			if (i==j)
				m_pXform[i][j] = 1.0;
			else
				m_pXform[i][j] = 0.0;
			}

}

CWinTabCtx::~CWinTabCtx()
{ 
	Close(); 
}


BOOL CWinTabCtx::Close()
{
    BOOL bOk = TRUE;

    if ( mhCtx && mbOwner && mhWinTabLib )
    {
        bOk   = WcmClose(mhCtx);
        mhCtx = 0;
    }

    return bOk;
}

BOOL CWinTabCtx::Enable(BOOL bEnable)
{
	if ( !mhCtx || !mhWinTabLib || !WcmEnable(mhCtx, bEnable) )
		return FALSE;

	if ( bEnable )
		mLogCtx.lcStatus &= ~CXS_DISABLED;
	else
		mLogCtx.lcStatus |= CXS_DISABLED;

    return TRUE;
}

UINT CWinTabCtx::GetCtxName(char* pName, int nSize)
{
    strncpy(pName, mLogCtx.lcName, nSize-1);
    pName[nSize-1] = '\0'/*DNT*/;

    return strlen(pName);
}

BOOL CWinTabCtx::GetTrackingMode()
{
    if ( IsSystem() )
        return mLogCtx.lcSysMode;
    else
        return (mLogCtx.lcPktMode & (PK_X | PK_Y)) != 0;
}

HCTX CWinTabCtx::Open(HWND hWnd, BOOL bEnable)
{
	if ( bEnable )
		mLogCtx.lcStatus &= ~CXS_DISABLED;
	else
		mLogCtx.lcStatus |= CXS_DISABLED;

    mhCtx   = mhWinTabLib ? WcmOpenA(hWnd, &mLogCtx, bEnable) : 0;
    mbOwner = mhCtx != NULL;
	mhWnd   = hWnd;

    return mhCtx;
}

BOOL CWinTabCtx::Overlap(BOOL bToTop)
{
    return mhCtx && mhWinTabLib && WcmOverlap(mhCtx, bToTop);
}

BOOL CWinTabCtx::ReadPacket(UINT wSerial)
{
    char Pkt[sizeof(PACKET)];

    // Select newest packet if no serial nbr specified

    if ( !wSerial )
    {
        UINT wOldest,wNewest;
        if ( !mhWinTabLib || !WcmQueuePacketsEx(mhCtx,&wOldest,&wNewest) )
            return FALSE;
        wSerial = wNewest;
    }

    // Read packet from WinTab

    if ( !mhWinTabLib || !mhCtx || !WcmPacket(mhCtx,wSerial,&Pkt) )
        return FALSE;

    // Extract enabled data items from packet

    WTPKT pktData = mLogCtx.lcPktData;
    char* pData   = Pkt;

    #define GETVAL(v)   { memcpy(&v, pData, sizeof(v)); pData += sizeof(v); }

    // Extract context handle
   
    if ( pktData & PK_CONTEXT )
        GETVAL(mPacket.pkContext);

    // Extract status information

    if ( pktData & PK_STATUS )
        GETVAL(mPacket.pkStatus);

    // Extract time stamp

    if ( pktData & PK_TIME )
        GETVAL(mPacket.pkTime);

    // Extract change information

    if ( pktData & PK_CHANGED )
        GETVAL(mPacket.pkChanged);

    // Extract serial number

    if ( pktData & PK_SERIAL_NUMBER )
        GETVAL(mPacket.pkSerialNumber);

    // Extract cursor type

    if ( pktData & PK_CURSOR )
        GETVAL(mPacket.pkCursor);

    // Extract button status

    if ( pktData & PK_BUTTONS )
        GETVAL(mPacket.pkButtons);

    // Extract cursor position

    if ( pktData & PK_X )
        GETVAL(mPacket.pkX);

    if ( pktData & PK_Y )
        GETVAL(mPacket.pkY);

    if ( pktData & PK_Z )
        GETVAL(mPacket.pkZ);

    // Extract pen pressure

    if ( pktData & PK_NORMAL_PRESSURE )
        GETVAL(mPacket.pkNormalPressure);

    // Extract pointer orientation

    if ( pktData & PK_ORIENTATION )
        GETVAL(mPacket.pkOrientation);

	m_bTransformed = FALSE;

    return TRUE;
}

void CWinTabCtx::GetTabletPosition(sds_point pptPos)
    {
    if (!m_bTransformed && ( mLogCtx.lcPktData & PK_X ) && ( mLogCtx.lcPktData & PK_Y ))
		{
		pptPos[0] =	mPacket.pkX*m_pXform[0][0]+
						mPacket.pkY*m_pXform[0][1]+
									m_pXform[0][3];
		pptPos[1] =	mPacket.pkX*m_pXform[1][0]+
						mPacket.pkY*m_pXform[1][1]+
									m_pXform[1][3];
		pptPos[2] =	mPacket.pkX*m_pXform[2][0]+
						mPacket.pkY*m_pXform[2][1]+
									m_pXform[2][3];

		if (m_xformtype == XFORM_PROJECTIVE)
			{
            // In the Affine and Ortho cases, the matrix m_pXform should have been constructed in such a way that
            // m_pXform[3][0] = m_pXform[3][1] = 0 and m_pXform[3][3] = 1, so in those cases we would
            // have denom = 1 in the following.

			double denom;
			denom =		mPacket.pkX*m_pXform[3][0]+
						mPacket.pkY*m_pXform[3][1]+
									m_pXform[3][3];

			// we need to somehow check that denom is not zero, but not through an assertion statement,
			// because denom CAN be zero for some legitimate values of (mPacket.pkX, mPacket.plY)

			if (fabs(denom) > 0.000001) // check that it is larger than some reasonable value
				{
				pptPos[0] /= denom;
				pptPos[1] /= denom;
				pptPos[2] /= denom;        
				}
			else
				{
			   // we will have to decide what to do here; perhaps return an error code, perhaps return (0,0,0),
			   // perhaps return pptPos not divided by denom.
				}

			}

		m_bTransformed = TRUE;
		}

	}

void CWinTabCtx::MouseEvent()
{
    static DWORD dwLastButtons;
    static POINT ptLastPos;

    // Get button information from tablet context
    // and determine button state changes

    DWORD dwBtnChange = dwLastButtons ^ mPacket.pkButtons;
    DWORD dwBtnDn     = dwBtnChange &  mPacket.pkButtons;
    DWORD dwBtnUp     = dwBtnChange & ~mPacket.pkButtons;
    dwLastButtons     = mPacket.pkButtons;

    // Convert tablet button events to system button events

    DWORD dwEvtFlags  = 0;
    DWORD dwBtnUpDbl  = 0;
    DWORD dwBtnDnDbl  = 0;

            DWORD dwDnFlag = 0;
            DWORD dwUpFlag = 0;

    if ( dwBtnChange )
    {
        // Get button map for current cursor type

        BYTE SysBtnMap[32];
        UINT wCursor = GetCursor();
        CWinTabCsr* pCursor = new CWinTabCsr(wCursor);
        pCursor->GetButtonMap(NULL, SysBtnMap);
        delete pCursor;

        // Set flags for all system button events

        for ( int nBtnNbr = 0 ; nBtnNbr < 32 ; nBtnNbr++ )
        {
            DWORD dwBtnBit = 0x00000001L << nBtnNbr;

            // Skip unchanged buttons

            if ( !(dwBtnChange & dwBtnBit) )
                continue;

            // Get system flags for current button function


            switch ( SysBtnMap[nBtnNbr] & 0x0F )
            {
                case SBN_LCLICK:
                case SBN_LDBLCLICK:
                case SBN_LDRAG:
                    dwDnFlag = MOUSEEVENTF_LEFTDOWN;
                    dwUpFlag = MOUSEEVENTF_LEFTUP;
                    break;
                case SBN_RCLICK:
                case SBN_RDBLCLICK:
                case SBN_RDRAG:
                    dwDnFlag = MOUSEEVENTF_RIGHTDOWN;
                    dwUpFlag = MOUSEEVENTF_RIGHTUP;
                    break;
                case SBN_MCLICK:
                case SBN_MDBLCLICK:
                case SBN_MDRAG:
                    dwDnFlag = MOUSEEVENTF_MIDDLEDOWN;
                    dwUpFlag = MOUSEEVENTF_MIDDLEUP;
                    break;
            }

            // Skip non-system buttons

            if ( !dwDnFlag || !dwUpFlag )
                continue;

            // Setup event flags depending on button function type

            static DWORD dwBtnDrag = 0;

            switch ( SysBtnMap[nBtnNbr] & 0x0F )
            {
                case SBN_LDBLCLICK:
                case SBN_RDBLCLICK:
                case SBN_MDBLCLICK:
                    if ( dwBtnDn & dwBtnBit )
                    {
                        dwBtnDnDbl |= dwDnFlag;
                        dwBtnUpDbl |= dwUpFlag;
                    }
                    // Fall through!!
                case SBN_LCLICK:
                case SBN_RCLICK:
                case SBN_MCLICK:
                    if ( dwBtnDn & dwBtnBit )
                        dwUpFlag = 0;
                    else
                        dwDnFlag = 0;
                    break;
                case SBN_LDRAG:
                case SBN_RDRAG:
                case SBN_MDRAG:
                    if ( dwBtnUp & dwBtnBit )
                    {
                        dwUpFlag = 0;
                        dwDnFlag = 0;
                    }
                    else
                    {
                        dwBtnDrag ^= dwBtnBit;
                        if ( dwBtnDrag & dwBtnBit )
                            dwUpFlag = 0;
                        else
                            dwDnFlag = 0;
                    }
                    break;
            }
            dwEvtFlags |= dwUpFlag | dwDnFlag;
        }
    }

    // Get tablet position information
	CRect crect;
    POINT ptPos;
	int nX,nY;
	sds_point point;

    GetTabletPosition(point);	//this gets the point in real world coordinaes
	gr_rp2pix(SDS_CURGRVW, point[0], point[1], &nX, &nY);	//convert it to client coords
	ptPos.x = nX;
	ptPos.y = nY;
	GetClientRect (SDS_CURVIEW->m_hWnd, &crect);

	//if the mouse is currently outside the Client Rect, Post events to the main engine thread, so that
	//Intellicad can continue to process the input.  If it is in the client rect, just send the regular
	//mouse events, so that cursor movement can be tracked in the standard way.
	if (!crect.PtInRect(ptPos))
		{
		sds_point eventPoint;
		struct resbuf rb;

		gr_rp2ucs( point, eventPoint, SDS_CURGRVW );
		ic_ptcpy( SDS_PreGridPoint, eventPoint );

		if( !SDS_AtEntGet && 
			!SDS_AtCmdLine) 
			{
			SDS_SnapPt( eventPoint, eventPoint );
			}

		SDS_getvar(NULL,DB_QCOORDS,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		SDS_CURVIEW->UpdateCoords( eventPoint, rb.resval.rint );
		
		SDS_CURVIEW->CursorDown( ptPos);

		int eventType;
        if (dwDnFlag)
			{
			eventType=SDS_EVM_MOUSEBUTTON;
			}
		else
			{
			eventType=SDS_EVM_MOUSEXYZ;
			}

		InputEvent theEvent( eventType, eventPoint );
		SDS_PostInputEvent( theEvent );
			
		return;
		}

    // Flag cursor movement and normalize coordinates

	ClientToScreen( SDS_CURVIEW->m_hWnd, &ptPos);

    if ( ptPos.x != ptLastPos.x || ptPos.y != ptLastPos.y )
    {
        dwEvtFlags |= MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
        ptLastPos   = ptPos;

        // Abs cursor data is normalized between 0 and 65535

        ptPos.x = ptPos.x * 65535L / GetSystemMetrics(SM_CXSCREEN);
        ptPos.y = ptPos.y * 65535L / GetSystemMetrics(SM_CYSCREEN);
    }

    // Emulate Windows mouse event if there's something to report

    if ( dwEvtFlags )
        mouse_event(dwEvtFlags, ptPos.x, ptPos.y, 0, 0);

    // Produce additional button up & down event for double clicks

    if ( dwBtnDnDbl || dwBtnUpDbl )
    {
        mouse_event(dwBtnUpDbl, ptPos.x, ptPos.y, 0, 0);
        mouse_event(dwBtnDnDbl, ptPos.x, ptPos.y, 0, 0);
    }
}

UINT CWinTabCtx::SetCtxName( LPCTSTR pName)
{
    // Validate paramaters

    if ( !pName )
        return 0;

    // Get name size within limits

    UINT nLen = strlen(pName);
    if ( nLen >= LCNAMELEN )
        nLen = LCNAMELEN - 1;

    // Copy name to context

    strncpy(mLogCtx.lcName, pName, nLen);
    mLogCtx.lcName[nLen] = '\0'/*DNT*/;

    UpdateAttributes();

    return strlen(mLogCtx.lcName);
}

void CWinTabCtx::SetInputArea(RECT* pArea)
{
    mLogCtx.lcInOrgX = pArea->left;
    mLogCtx.lcInOrgY = pArea->top;
    mLogCtx.lcInExtX = pArea->right  - pArea->left;
    mLogCtx.lcInExtY = pArea->bottom - pArea->top;

    UpdateAttributes();
}

void CWinTabCtx::SetOutputArea(RECT* pArea)
{
    mLogCtx.lcOutOrgX = pArea->left;
    mLogCtx.lcOutOrgY = pArea->top;
    mLogCtx.lcOutExtX = pArea->right  - pArea->left;
    mLogCtx.lcOutExtY = pArea->bottom - pArea->top;

    UpdateAttributes();
}

void CWinTabCtx::SetScreenArea(RECT* pArea)
{
    mLogCtx.lcSysOrgX = pArea->left;
    mLogCtx.lcSysOrgY = GetSystemMetrics(SM_CYSCREEN) - pArea->bottom;
    mLogCtx.lcSysExtX = pArea->right  - pArea->left;
    mLogCtx.lcSysExtY = pArea->bottom - pArea->top;

    UpdateAttributes();
}

void CWinTabCtx::SetTrackingMode(BOOL bRelative)
{
    mLogCtx.lcSysMode = bRelative;

    if ( bRelative )
        mLogCtx.lcPktMode |= PK_X | PK_Y;
    else
        mLogCtx.lcPktMode &= ~(PK_X | PK_Y);

    UpdateAttributes();
}

void CWinTabCtx::UpdateAttributes()
{
    if ( mhCtx && mhWinTabLib )
        WcmSetA(mhCtx, &mLogCtx);
}

void  CWinTabCtx::SetTransformation(sds_matrix xform, xform_type type)
	{
	int i, j;
	for (i=0; i<4;i++)
		for (j=0;j<4;j++)
			m_pXform[i][j] = xform[i][j];
	m_xformtype = type;
	}

