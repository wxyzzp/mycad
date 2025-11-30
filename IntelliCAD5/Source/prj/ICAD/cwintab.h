/* ICAD\CWINTAB.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * Definition of WinTab Helper classes
 * Originally Authored by Eric Vandewater of CalComp
 *
 */

#pragma once

#define  PACKETDATA     0xFFFF

#include "wintab.h"/*DNT*/
#include "pktdef.h"/*DNT*/



// WinTAB packet definition

//#include <wintab\pktdef.h>

// WinTab I/O error codes

#define WTERR_NONE                  0
#define WTERR_WINTAB32NOTFOUND      1
#define WTERR_WINTABINVALID         2


/////////////////////////////////////////////////////////////////////////////
//
//  CWinTab class definition
//
/////////////////////////////////////////////////////////////////////////////


class CWinTab
{
public:
    // Constructor/destructor

    CWinTab();
    ~CWinTab();

    // General info functions

    WTPKT GetCsrDataItems(UINT wCursor);
    BOOL  GetCsrName(UINT wCursor, char* pName, int nSize);
	BOOL  GetDeviceName(char* pName, int nSize);
    WORD  GetDriverVersion();
    UINT  GetLastError(char* pError=0, int nSize=0);
    LONG  GetMaxPressure();
    UINT  GetNbrCursorTypes();
    WORD  GetSpecVersion();
    BOOL  GetTabletSize(SIZE* pSize);

protected:
    UINT mwErrorCode;
};


/////////////////////////////////////////////////////////////////////////////
//
//  CWinTabCsr class definition
//
/////////////////////////////////////////////////////////////////////////////


class CWinTabCsr : public CWinTab
{
public:
    CWinTabCsr(UINT wCursor);
    ~CWinTabCsr();

    BOOL  GetButtonMap(BYTE* pLogBtns, BYTE* pSysBtns);
	BOOL  GetButtonMarks(UINT* pwRelease, UINT* pwPress);
    WTPKT GetDataItems();
    BOOL  GetPressureCurve(UINT* pCurve);
    BOOL  SetButtonMap(BYTE* pLogBtns, BYTE* pSysBtns);
    BOOL  SetButtonMarks(UINT wRelease, UINT wPress);
    BOOL  SetPressureCurve(UINT* pCurve);

protected:
    UINT mwCursor;
    HMGR mhMgr;
};


/////////////////////////////////////////////////////////////////////////////
//
//  CWinTabCtx class definition
//
/////////////////////////////////////////////////////////////////////////////

enum xform_type
	{
	XFORM_ORTHOGONAL = 0,
	XFORM_AFFINE,
	XFORM_PROJECTIVE
	};

class CWinTabCtx : public CWinTab
{
public:
    CWinTabCtx(BOOL bSystem);
    CWinTabCtx(HCTX hCtx);
    ~CWinTabCtx();

    BOOL  Close();
    BOOL  Enable(BOOL bEnable);
    DWORD GetBtnDnMask();
    DWORD GetBtnUpMask();
    DWORD GetButtons();
    UINT  GetCtxName(char* pName, int nSize);
	UINT  GetCursor();
	HCTX  GetHandle();
    WTPKT GetMoveMask();
    UINT  GetOptions();
    UINT  GetPressure();
    void  GetTabletPosition(POINT* pptPos);
    void  GetTabletPosition(sds_point ptPos);
    BOOL  GetTrackingMode();
	BOOL  IsEnabled();
    BOOL  IsSystem();
	void  MouseEvent();
    HCTX  Open(HWND hWnd, BOOL bEnable=TRUE);
    BOOL  Overlap(BOOL bToTop);
    BOOL  ReadPacket(UINT wSerial=0);
    void  SetBtnDnMask(DWORD dwMask);
    void  SetBtnUpMask(DWORD dwMask);
    UINT  SetCtxName(LPCTSTR pName);
    void  SetMoveMask(WTPKT pktMask);
    void  SetOptions(UINT wOptions);
    void  SetPktData(WTPKT pktData);
	void  SetPktMode(WTPKT pktMode);
    void  SetInputArea(RECT* pArea);
    void  SetOutputArea(RECT* pArea);
    void  SetScreenArea(RECT* pArea);
    void  SetTrackingMode(BOOL bRelative);
	void  SetTransformation(sds_matrix xform, xform_type type);

protected:
    void       UpdateAttributes();

    HCTX       mhCtx;
	HWND	   mhWnd;
    LOGCONTEXT mLogCtx;
	BOOL       mbOwner;
    PACKET     mPacket;
	sds_matrix m_pXform;
	xform_type m_xformtype;
	BOOL	   m_bTransformed;
};

// CWinTabCtx inline member functions

inline DWORD CWinTabCtx::GetBtnDnMask()
    { return mLogCtx.lcBtnDnMask; }

inline DWORD CWinTabCtx::GetBtnUpMask()
    { return mLogCtx.lcBtnUpMask; }

inline DWORD CWinTabCtx::GetButtons()
    { return mPacket.pkButtons; }

inline UINT CWinTabCtx::GetCursor()
    { return mPacket.pkCursor; }

inline HCTX CWinTabCtx::GetHandle()
	{ return mhCtx; }

inline UINT CWinTabCtx::GetOptions()
    { return mLogCtx.lcOptions; }

inline UINT CWinTabCtx::GetPressure()
    { return mPacket.pkNormalPressure; }

inline WTPKT CWinTabCtx::GetMoveMask()
    { return mLogCtx.lcMoveMask; }

inline void CWinTabCtx::GetTabletPosition(POINT* pptPos)
    { pptPos->x = mPacket.pkX; pptPos->y = mPacket.pkY; }

inline BOOL CWinTabCtx::IsEnabled()
	{ return (mLogCtx.lcStatus & CXS_DISABLED) == 0; }

inline BOOL CWinTabCtx::IsSystem()
    { return (mLogCtx.lcOptions & CXO_SYSTEM) != 0; }

inline void CWinTabCtx::SetBtnDnMask(DWORD dwMask)
    { mLogCtx.lcBtnDnMask = dwMask; UpdateAttributes(); }

inline void CWinTabCtx::SetBtnUpMask(DWORD dwMask)
    { mLogCtx.lcBtnUpMask = dwMask; UpdateAttributes(); }

inline void CWinTabCtx::SetMoveMask(WTPKT pktMask)
    { mLogCtx.lcMoveMask = pktMask; UpdateAttributes(); }

inline void CWinTabCtx::SetOptions(UINT wOptions)
    { mLogCtx.lcOptions = wOptions; UpdateAttributes(); }

inline void CWinTabCtx::SetPktData(WTPKT pktData)
    { mLogCtx.lcPktData = pktData; UpdateAttributes(); }

inline void CWinTabCtx::SetPktMode(WTPKT pktMode)
    { mLogCtx.lcPktMode = pktMode; UpdateAttributes(); }


