/* D:\INTELLICAD\PRJ\LIB\AUTO\AUTOWINDOW.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Implementation of CAutoWindow - the Automation Window object
 * 
 */ 

#include "stdafx.h"
#include "AutoWindow.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoWindow

HRESULT CAutoWindow::Init(IIcadWindows *pParent, CWnd *pWnd, CIcadDoc *pDoc)
	{
	m_pDoc = pDoc;
	m_pWnd = pWnd;
	m_pParent = pParent;
	return S_OK;
	}

STDMETHODIMP CAutoWindow::get_Application (IIcadApplication * * ppApp)
	{
	if (ppApp == NULL)
		return E_POINTER;
	
	return GetApp()->m_pAutoApp->QueryInterface (ppApp);
	}

STDMETHODIMP CAutoWindow::get_Document (IIcadDocument * * ppDoc)
	{
	if (ppDoc == NULL)
		return E_POINTER;
	
	*ppDoc = NULL;

	if (m_pDoc)
		return m_pDoc->m_pAutoDoc->QueryInterface (ppDoc);
	else
		return NOERROR;
	}

STDMETHODIMP CAutoWindow::get_Parent (IIcadWindows * * ppParent)
	{
	if (ppParent == NULL)
		return E_POINTER;
	
	*ppParent = NULL;

	if (m_pParent)
		return (m_pParent->QueryInterface (ppParent));
	else
		return NOERROR;
	}

STDMETHODIMP CAutoWindow::get_Caption (BSTR * pCaption)
	{
	if (pCaption == NULL)
		return E_POINTER;
	
	if (!m_pWnd)
		return E_FAIL;

	CString name;
	m_pWnd->GetWindowText (name);

	*pCaption = name.AllocSysString();
	
	return NOERROR;
	}

STDMETHODIMP CAutoWindow::get_Visible (VARIANT_BOOL * pVisible)
	{
	if (pVisible == NULL)
		return E_POINTER;
	
	if (!m_pWnd)
		return E_FAIL;

	*pVisible = (m_pWnd->IsWindowVisible()) ? VARIANT_TRUE : VARIANT_FALSE;

	return NOERROR;
	}

STDMETHODIMP CAutoWindow::put_Visible (VARIANT_BOOL pVisible)
	{
	if (!m_pWnd)
		return E_FAIL;

	if (pVisible == VARIANT_TRUE)
		m_pWnd->ShowWindow(SW_SHOW);
	else
		m_pWnd->ShowWindow(SW_HIDE);

	return NOERROR;
	}

STDMETHODIMP CAutoWindow::get_WindowHandle32 (long * pHandle)
	{
	if (pHandle == NULL)
		return E_POINTER;
	
	if (!m_pWnd)
		return E_FAIL;

	*pHandle = (long)m_pWnd->m_hWnd;
	
	return NOERROR;
	}

STDMETHODIMP CAutoWindow::get_WindowState (WindowState * pState)
	{
	if (pState == NULL)
		return E_POINTER;
	
	if (!m_pWnd)
		return E_FAIL;

	return E_NOTIMPL;
	}

STDMETHODIMP CAutoWindow::put_WindowState (WindowState pState)
	{
	if (!m_pWnd)
		return E_FAIL;

	return E_NOTIMPL;
	}

STDMETHODIMP CAutoWindow::Activate ()
	{
	if (!m_pWnd)
		return E_FAIL;

	((CMDIChildWnd *)m_pWnd)->MDIActivate();
	return NOERROR;
	}

STDMETHODIMP CAutoWindow::Close ()
	{
	if (!m_pWnd)
		return E_FAIL;

	((CMDIChildWnd *)m_pWnd)->MDIDestroy();
	return NOERROR;
	}

STDMETHODIMP CAutoWindow::GetWindowRect (long * Left, long * Top, long * Width, long * Height)
	{
	if (!m_pWnd)
		return E_FAIL;

	if (Left == NULL)
		return E_POINTER;
	
	if (Top == NULL)
		return E_POINTER;
	
	if (Width == NULL)
		return E_POINTER;
	
	if (Height == NULL)
		return E_POINTER;
	
	CRect rect;
	m_pWnd->GetWindowRect (rect);
	*Left = rect.left;
	*Top = rect.top;
	*Width = rect.right - rect.left;
	*Height = rect.bottom - rect.top;

	return NOERROR;
	}

STDMETHODIMP CAutoWindow::SetWindowRect (long Left, long Top, long Width, long Height)
	{
	if (!m_pWnd)
		return E_FAIL;

	CPoint pt(Left, Top);
	m_pWnd->GetParent()->ScreenToClient(&pt);

	m_pWnd->SetWindowPos (NULL, pt.x, pt.y, Width, Height, SWP_DRAWFRAME);
	
	return NOERROR;
	}

STDMETHODIMP CAutoWindow::Scroll (ScrollFlagsX FlagsX, ScrollFlagsY FlagsY)
	{
	if (!m_pWnd)
		return E_FAIL;

	return E_NOTIMPL;
	}



