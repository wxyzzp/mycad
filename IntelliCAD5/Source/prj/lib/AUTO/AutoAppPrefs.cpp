/* INTELLICAD\PRJ\LIB\AUTO\AUTOAPPPREFS.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 *
 *
 * Abstract
 *
 * Implementation of the CAutoAppPreferences - The Application level preferences
 *
 */

#include "StdAfx.h"
#include "AutoAppPrefs.h"
#include "IcadMain.h"

#define CURDBDWG (((CMainWindow *)(m_pApp->m_pIcadMainWnd))->m_pCurDoc->m_dbDrawing)

/////////////////////////////////////////////////////////////////////////////
// CAutoAppPreferences

STDMETHODIMP CAutoAppPreferences::get_Application (IIcadApplication * * Application)
	{
	if (Application == NULL)
		return E_POINTER;

	return m_pApp->m_pAutoApp->QueryInterface (Application);
	}

STDMETHODIMP CAutoAppPreferences::get_Parent (IIcadApplication * * Application)
	{
	if (Application == NULL)
		return E_POINTER;

	return get_Application (Application);
	}

STDMETHODIMP CAutoAppPreferences::get_CursorSize (int * Size)
	{
	if (Size == NULL)
		return E_POINTER;

	struct resbuf rb;
	char sysvar[IC_ACADBUF];
	strcpy (sysvar, "CURSORSIZE");

	HRESULT hr = m_pApp->SDSGetVar (NULL, sysvar, &rb);
	if (FAILED(hr))
		return E_FAIL;

	*Size = rb.resval.rint;
	return NOERROR;
	}

STDMETHODIMP CAutoAppPreferences::put_CursorSize (int Size)
	{
	struct resbuf rb;
	rb.restype = RTSHORT;
	rb.resval.rint = Size;

	char sysvar[IC_ACADBUF];
	strcpy (sysvar, "CURSORSIZE");

	return m_pApp->SDSSetVar (NULL, sysvar, &rb);
	}

STDMETHODIMP CAutoAppPreferences::get_CursorXColor (Colors * pColor)
	{
	if (pColor == NULL)
		return E_POINTER;

	struct resbuf rb;
	char sysvar[IC_ACADBUF];
	strcpy (sysvar, "COLORX");

	HRESULT hr = m_pApp->SDSGetVar (NULL, sysvar, &rb);
	if (FAILED(hr))
		return E_FAIL;

	*pColor = (Colors)rb.resval.rint;

	return NOERROR;
	}

STDMETHODIMP CAutoAppPreferences::put_CursorXColor (Colors pColor)
	{
	struct resbuf rb;
	char sysvar[IC_ACADBUF];
	strcpy (sysvar, "COLORX");

	rb.restype = RTSHORT;
	rb.resval.rint = pColor;

	return m_pApp->SDSSetVar (NULL, sysvar, &rb);
	}

STDMETHODIMP CAutoAppPreferences::get_CursorYColor (Colors * pColor)
	{
	if (pColor == NULL)
		return E_POINTER;

	struct resbuf rb;
	char sysvar[IC_ACADBUF];
	strcpy (sysvar, "COLORY");

	HRESULT hr = m_pApp->SDSGetVar (NULL, sysvar, &rb);
	if (FAILED(hr))
		return E_FAIL;

	*pColor = (Colors)rb.resval.rint;

	return NOERROR;
	}

STDMETHODIMP CAutoAppPreferences::put_CursorYColor (Colors pColor)
	{
	struct resbuf rb;
	char sysvar[IC_ACADBUF];
	strcpy (sysvar, "COLORY");

	rb.restype = RTSHORT;
	rb.resval.rint = pColor;

	return m_pApp->SDSSetVar (NULL, sysvar, &rb);
	}

STDMETHODIMP CAutoAppPreferences::get_CursorZColor (Colors * pColor)
	{
	if (pColor == NULL)
		return E_POINTER;

	struct resbuf rb;
	char sysvar[IC_ACADBUF];
	strcpy (sysvar, "COLORZ");

	HRESULT hr = m_pApp->SDSGetVar (NULL, sysvar, &rb);
	if (FAILED(hr))
		return E_FAIL;

	*pColor = (Colors)rb.resval.rint;

	return NOERROR;
	}

STDMETHODIMP CAutoAppPreferences::put_CursorZColor (Colors pColor)
	{
	struct resbuf rb;
	char sysvar[IC_ACADBUF];
	strcpy (sysvar, "COLORZ");

	rb.restype = RTSHORT;
	rb.resval.rint = pColor;

	return m_pApp->SDSSetVar (NULL, sysvar, &rb);
	}

STDMETHODIMP CAutoAppPreferences::get_DisplayScrollBars (VARIANT_BOOL * Visible)
	{
	if (Visible == NULL)
		return E_POINTER;

	struct resbuf rb;
	char sysvar[IC_ACADBUF];
	strcpy (sysvar, "WNDLSCRL");

	HRESULT hr = m_pApp->SDSGetVar (NULL, sysvar, &rb);
	if (FAILED(hr))
		return E_FAIL;

	*Visible =  (rb.resval.rint == 1) ? VARIANT_TRUE : VARIANT_FALSE;
	return NOERROR;
	}

STDMETHODIMP CAutoAppPreferences::put_DisplayScrollBars (VARIANT_BOOL Visible)
	{
	struct resbuf rb;
	rb.restype=RTSHORT;
	rb.resval.rint = (Visible == VARIANT_TRUE) ? 1 : 0;

	char sysvar[IC_ACADBUF];
	strcpy (sysvar, "WNDLSCRL");

	return m_pApp->SDSSetVar (NULL, sysvar, &rb);
	}

STDMETHODIMP CAutoAppPreferences::get_DockedVisibleLines (int * NumberOfLines)
	{
	if (NumberOfLines == NULL)
		return E_POINTER;

	struct resbuf rb;
	char sysvar[IC_ACADBUF];
	strcpy (sysvar, "WNDSCMD");

	HRESULT hr = m_pApp->SDSGetVar (NULL, sysvar, &rb);
	if (FAILED(hr))
		return E_FAIL;

	*NumberOfLines = (int)(rb.resval.rpoint[1]/22.);
	return NOERROR;
	}

STDMETHODIMP CAutoAppPreferences::put_DockedVisibleLines (int NumberOfLines)
	{
	struct resbuf rb;
	char sysvar[IC_ACADBUF];
	strcpy (sysvar, "WNDSCMD");

	HRESULT hr = m_pApp->SDSGetVar (NULL, sysvar, &rb);
	if (FAILED(hr))
		return E_FAIL;

	rb.resval.rpoint[1] = (double)(NumberOfLines*22);

	return m_pApp->SDSSetVar (NULL, sysvar, &rb);
	}

STDMETHODIMP CAutoAppPreferences::get_GraphicsWinBackgrndColor (Colors * pColor)
	{
	if (pColor == NULL)
		return E_POINTER;
	
	struct resbuf rb;
	char sysvar[IC_ACADBUF];
	strcpy (sysvar, "BKGCOLOR");

	HRESULT hr = m_pApp->SDSGetVar (NULL, sysvar, &rb);
	if (FAILED(hr))
		return E_FAIL;

	*pColor = (Colors)rb.resval.rint;
	return NOERROR;
	}

STDMETHODIMP CAutoAppPreferences::put_GraphicsWinBackgrndColor (Colors pColor)
	{
	struct resbuf rb;
	char sysvar[IC_ACADBUF];
	strcpy (sysvar, "BKGCOLOR");

	HRESULT hr = m_pApp->SDSGetVar (NULL, sysvar, &rb);
	if (FAILED(hr))
		return E_FAIL;

	rb.resval.rint = pColor;

	return m_pApp->SDSSetVar (NULL, sysvar, &rb);
	}

STDMETHODIMP CAutoAppPreferences::get_HistoryLines (int * NumberOfLines)
	{
	if (NumberOfLines == NULL)
		return E_POINTER;
	
	struct resbuf rb;
	char sysvar[IC_ACADBUF];
	strcpy (sysvar, "SCRLHIST");

	HRESULT hr = m_pApp->SDSGetVar (NULL, sysvar, &rb);
	if (FAILED(hr))
		return E_FAIL;

	*NumberOfLines = rb.resval.rint;
	return NOERROR;
	}

STDMETHODIMP CAutoAppPreferences::put_HistoryLines (int NumberOfLines)
	{
	struct resbuf rb;
	rb.restype=RTSHORT;
	rb.resval.rint = NumberOfLines;

	char sysvar[IC_ACADBUF];
	strcpy (sysvar, "SCRLHIST");

	return m_pApp->SDSSetVar (NULL, sysvar, &rb);
	}

STDMETHODIMP CAutoAppPreferences::get_MaxIntelliCADWindow (VARIANT_BOOL * Maximize)
	{
	if (Maximize == NULL)
		return E_POINTER;
	
	struct resbuf rb;
	char sysvar[IC_ACADBUF];
	strcpy (sysvar, "WNDLMAIN");

	HRESULT hr = m_pApp->SDSGetVar (NULL, sysvar, &rb);
	if (FAILED(hr))
		return E_FAIL;

	*Maximize =  (rb.resval.rint == 2) ? VARIANT_TRUE : VARIANT_FALSE;
	return NOERROR;
	}

STDMETHODIMP CAutoAppPreferences::put_MaxIntelliCADWindow (VARIANT_BOOL Maximize)
	{
	struct resbuf rb;
	rb.restype=RTSHORT;
	rb.resval.rint = (Maximize == VARIANT_TRUE) ? 2 : 0;

	char sysvar[IC_ACADBUF];
	strcpy (sysvar, "WNDLMAIN");

	return m_pApp->SDSSetVar (NULL, sysvar, &rb);
	}

STDMETHODIMP CAutoAppPreferences::get_AutoSaveInterval (int * Interval)
	{
	if (Interval == NULL)
		return E_POINTER;
	
	struct resbuf rb;
	db_getvar (NULL, DB_QSAVETIME, &rb, CURDBDWG, NULL, NULL);

	*Interval = rb.resval.rint;
	
	return NOERROR;
	}

STDMETHODIMP CAutoAppPreferences::put_AutoSaveInterval (int Interval)
	{
	struct resbuf rb;

	if (Interval <= 0)
		return E_INVALIDARG;

	rb.restype = RTSHORT;
	rb.resval.rint = Interval;

	char sysvar[IC_ACADBUF];
	strcpy (sysvar, "SAVETIME");

	return m_pApp->SDSGetVar (NULL, sysvar, &rb);

	}

STDMETHODIMP CAutoAppPreferences::get_LogFileName (BSTR * LogFileName)
	{
	if (LogFileName == NULL)
		return E_POINTER;
	
	struct resbuf rb;
	char sysvar[IC_ACADBUF];
	strcpy (sysvar, "LOGFILENAME");

	rb.resval.rstring = NULL;
	HRESULT hr = m_pApp->SDSGetVar (NULL, sysvar, &rb);
	if (FAILED(hr))
		return E_FAIL;

	CString str = rb.resval.rstring;
	*LogFileName = str.AllocSysString();
	
	IC_FREE(rb.resval.rstring);
	return NOERROR;
	}

STDMETHODIMP CAutoAppPreferences::put_LogFileName (BSTR LogFileName)
	{
	struct resbuf rb;

	CString str(LogFileName);
	if (str.IsEmpty ())
		return E_INVALIDARG;

	rb.restype = RTSTR;
	rb.resval.rstring=(char *)str.GetBuffer(0);

	char sysvar[IC_ACADBUF];
	strcpy (sysvar, "LOGFILENAME");
	return m_pApp->SDSSetVar (NULL, sysvar, &rb);
	}

STDMETHODIMP CAutoAppPreferences::get_LogFileOn (VARIANT_BOOL * On)
	{
	if (On == NULL)
		return E_POINTER;
	
	struct resbuf rb;
	char sysvar[IC_ACADBUF];
	strcpy (sysvar, "LOGFILEMODE");

	HRESULT hr = m_pApp->SDSGetVar (NULL, sysvar, &rb);
	if (FAILED(hr))
		return E_FAIL;

	*On =  (rb.resval.rint == 1) ? VARIANT_TRUE : VARIANT_FALSE;

	return NOERROR;
	}

STDMETHODIMP CAutoAppPreferences::put_LogFileOn (VARIANT_BOOL On)
	{
	struct resbuf rb;
	rb.restype=RTSHORT;
	rb.resval.rint = (On == VARIANT_TRUE) ? 1 : 0;

	char sysvar[IC_ACADBUF];
	strcpy (sysvar, "LOGFILEMODE");
	HRESULT hr = m_pApp->SDSSetVar (NULL, sysvar, &rb);
	
	//print an empty string to release the log file if we are closing it.
	CString str=" ";
	if (((CMainWindow *)(m_pApp->m_pIcadMainWnd))->m_pCurDoc)
		((CMainWindow *)(m_pApp->m_pIcadMainWnd))->m_pCurDoc->AUTO_prompt (str);

	return hr;
	}

STDMETHODIMP CAutoAppPreferences::get_AltFontFile (BSTR * AltFontFileName)
	{
	if (AltFontFileName == NULL)
		return E_POINTER;
	
	struct resbuf rb;
	rb.resval.rstring = NULL;
	db_getvar (NULL, DB_QFONTALT, &rb, CURDBDWG, NULL, NULL);
	CString str = rb.resval.rstring;

	*AltFontFileName = str.AllocSysString();
	
	IC_FREE(rb.resval.rstring);
	return NOERROR;
	}

STDMETHODIMP CAutoAppPreferences::put_AltFontFile (BSTR AltFontFileName)
	{
	struct resbuf rb;

	CString str(AltFontFileName);
	if (str.IsEmpty ())
		return E_INVALIDARG;

	rb.resval.rstring=(char *)str.GetBuffer(0);

	char sysvar[IC_ACADBUF];
	strcpy (sysvar, "FONTALT");

	rb.resval.rstring = NULL;
	return m_pApp->SDSGetVar (NULL, sysvar, &rb);

	}



