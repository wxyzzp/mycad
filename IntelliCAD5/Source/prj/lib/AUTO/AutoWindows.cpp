/* D:\INTELLICAD\PRJ\LIB\AUTO\AUTOWINDOWS.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Implementation of CAutoWindows - The Automation Windows collection
 * 
 */ 

#include "StdAfx.h"
#include "AutoWindows.h"
#include "AutoWindow.h"
#include "IcadMain.h"
#include "IcadChildWnd.h"
#include "IcadApi.h" 

/////////////////////////////////////////////////////////////////////////////
// CAutoWindows

HRESULT CAutoWindows::Init(CIcadDoc *pDoc)
	{
	if (!m_pDoc)
		m_pDoc = pDoc;

	CWnd *hCurItem;
	CIcadApp *pApp = GetApp();
	CWnd *hWnd = ((CMDIFrameWnd*)pApp->m_pMainWnd)->MDIGetActive(NULL);

    for ( hCurItem = hWnd; hCurItem; hCurItem = hCurItem->GetNextWindow(GW_HWNDNEXT)) 
		{
		if (pApp->IsIcadChildWindow(hCurItem))
			m_Contents.Add (hCurItem);
		}
	
	return S_OK;
	}


STDMETHODIMP CAutoWindows::get_Application (IIcadApplication * * ppApp)
	{
	if (ppApp == NULL)
		return E_POINTER;
	
	return GetApp()->m_pAutoApp->QueryInterface (ppApp);
	}

STDMETHODIMP CAutoWindows::get_Count (long * pCount)
	{
	if (pCount == NULL)
		return E_POINTER;
	
	*pCount = m_Contents.GetSize();
	return NOERROR;
	}

STDMETHODIMP CAutoWindows::get_Parent (IIcadApplication * * ppParent)
	{
	if (ppParent == NULL)
		return E_POINTER;
	
	return GetApp()->m_pAutoApp->QueryInterface (ppParent);
	}

STDMETHODIMP CAutoWindows::get__NewEnum (IUnknown * * ppUnk)
	{
	if (ppUnk == NULL)
		return E_POINTER;
	
	CComObject<CAutoEnumWindows>* penum;
	CComObject<CAutoEnumWindows>::CreateInstance(&penum);
	if (!penum)
        return E_FAIL;    
	
	penum->Init(this);
	
	return penum->QueryInterface(IID_IUnknown, (void **)ppUnk);
	}

STDMETHODIMP CAutoWindows::Add (IIcadWindow * * ppWindow)
	{
	if (ppWindow == NULL)
		return E_POINTER;
	
	CIcadApp *pApp = GetApp();

//	pApp->m_pIcadMainWnd->OnWindowNew();
	SendMessage(pApp->m_pIcadMainWnd->GetSafeHwnd(), WM_COMMAND, ICAD_WNDACTION_WOPEN, 0 );

	CWnd *pWnd = ((CMDIFrameWnd*)pApp->m_pMainWnd)->MDIGetActive(NULL);
	CIcadDoc* pDoc = (CIcadDoc*)(((CFrameWnd *)pWnd)->GetActiveDocument());

	m_Contents.Add (pWnd);
	
	CComObject<CAutoWindow> *pWindow;
	CComObject<CAutoWindow>::CreateInstance(&pWindow);
	if (!pWindow)
		return E_FAIL;

	pWindow->Init(this, pWnd, pDoc);

	return pWindow->QueryInterface(ppWindow);
	}

STDMETHODIMP CAutoWindows::Arrange (ArrangeStyle Style)
	{
	CMainWindow *pMainWnd = (CMainWindow *)GetApp()->m_pIcadMainWnd;
	
	if (Style == vicArrangeTileVertical)
		pMainWnd->MDITile (MDITILE_VERTICAL);
	else if (Style == vicArrangeTileHorizontal)
		pMainWnd->MDITile (MDITILE_HORIZONTAL);
	else if (Style == vicArrangeCascade)
		pMainWnd->MDICascade ();

	return NOERROR;
	}

STDMETHODIMP CAutoWindows::CloseAll ()
	{
/*	int i;
	for (i = 0; i < m_Contents.GetSize(); i++)
		{
		((CMDIChildWnd *)m_Contents[i])->MDIDestroy();
		}
*/
	m_Contents.RemoveAll();

	return GetApp()->CloseAllWindows();
	}

STDMETHODIMP CAutoWindows::Item (VARIANT Index, IIcadWindow * * ppItem)
	{
	if (ppItem == NULL)
		return E_POINTER;
	
	CComObject<CAutoWindow> *pWindow;
	CComObject<CAutoWindow>::CreateInstance(&pWindow);
	if (!pWindow)
		return E_FAIL;
	
	BOOL byIndex = FALSE;
	BOOL byName = FALSE;
	int lIndex;
	BSTR Name;
	CComVariant vIndex(Index);
	CComVariant nIndex;
	if (Index.vt == VT_BSTR)
		{
		byName = TRUE;
		Name = V_BSTR(&vIndex);
		}
	else if (SUCCEEDED (nIndex.ChangeType(VT_I4, &vIndex)))
		{
		byIndex = TRUE;
		lIndex = V_I4 (&nIndex);
		}
	else if (SUCCEEDED (nIndex.ChangeType(VT_BSTR, &vIndex)))
		{
		byName = TRUE;
		Name = V_BSTR (&nIndex);
		}

	if (byIndex)
		{		
		if (lIndex < 1 || lIndex > m_Contents.GetSize())
			return E_INVALIDARG; 	
		
		CWnd *pWnd = (CWnd *)(m_Contents[lIndex - 1]);
		CIcadDoc* pDoc = (CIcadDoc*)(((CFrameWnd *)pWnd)->GetActiveDocument());

		pWindow->Init(this, pWnd, pDoc);
		}
	else if (byName)
		{
		//TODO
		ASSERT (0);
		return E_FAIL;
		}

	return pWindow->QueryInterface(ppItem);
	}



/////////////////////////////////////////////////////////////////////////////
// CAutoEnumWindows

HRESULT CAutoEnumWindows::Init (IIcadWindows *pWindows)
	{
	m_pWindows = pWindows;
	m_nCurrent = 0;
	return NOERROR;
	}

STDMETHODIMP CAutoEnumWindows::Next (ULONG cElements, VARIANT * pvar, ULONG * pcElementFetched)
	{
    HRESULT hr;    
	ULONG l;    
	long l1;    
	ULONG l2;
	
    if (pcElementFetched != NULL)
		*pcElementFetched = 0;
    
	for (l = 0; l < cElements; l++)
		VariantInit (&pvar[l]);
	
    // Retrieve the next cElements elements.
	long nElements = 0;
	m_pWindows->get_Count(&nElements);
	
    for (l1 = m_nCurrent, l2 = 0; 
	l1 < nElements && l2 < cElements; 
	l1++, l2++)    
		{
		CComPtr<IIcadWindow> pEnt;
		CComVariant Index;
		V_VT(&Index) = VT_I4;
		V_I4(&Index) = l1 + 1;	
		if (FAILED (m_pWindows->Item(Index, &pEnt)))
			return S_FALSE;
		
		IDispatch *pDispEnt;
		hr = pEnt->QueryInterface(IID_IDispatch, (void **)&pDispEnt);
		if (FAILED(hr))
            goto error;     
		
		pvar[l2].vt = VT_DISPATCH;
		pvar[l2].pdispVal = pDispEnt;
		}    
	
	// Set count of elements retrieved.
    if (pcElementFetched != NULL)        
		*pcElementFetched = l2;
	
    m_nCurrent = l1;
	
    return  (l2 < cElements) ? ResultFromScode(S_FALSE) : NOERROR;
	
error:
    for (l = 0; l < cElements; l++)        
		VariantClear (&pvar[l]);
	
	return hr;
	}


STDMETHODIMP CAutoEnumWindows::Skip (ULONG cElements)
	{
    m_nCurrent += cElements;
	
	long nElements = 0;
	m_pWindows->get_Count(&nElements);
	
	if (m_nCurrent > nElements)
		{        
		m_nCurrent = nElements;
        return ResultFromScode(S_FALSE);    
		}    
	else 
		return NOERROR;
	}


STDMETHODIMP CAutoEnumWindows::Reset ()
	{
	m_nCurrent = 0;
	return NOERROR;
	}

STDMETHODIMP CAutoEnumWindows::Clone (IEnumVARIANT * * ppenum)
	{
	if (ppenum == NULL)
		return E_POINTER;
	
	*ppenum = NULL;    
	CComObject<CAutoEnumWindows>* penum;
	CComObject<CAutoEnumWindows>::CreateInstance(&penum);
	if (!penum)
        return E_FAIL;    
	
	penum->Init(m_pWindows);
	penum->AddRef();    
	
	*ppenum = penum;    
	return NOERROR;
	}



