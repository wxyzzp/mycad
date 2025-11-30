/* INTELLICAD\PRJ\LIB\AUTO\AUTOVIEWS.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Implementation of CAutoViews - The Automation Views collection
 * 
 */ 

#include "StdAfx.h"
#include "AutoViews.h"
#include "AutoView.h"
#include "viewtabrec.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoViews

HRESULT CAutoViews::Init(CIcadDoc *pDoc)
	{
	if (!m_pDoc)
		{
		m_pDoc = pDoc;
		}
	return S_OK;
	}

STDMETHODIMP CAutoViews::get_Application (IIcadApplication * * ppApp)
	{
	if (ppApp == NULL)
		return E_POINTER;
	
	return GetApp()->m_pAutoApp->QueryInterface (ppApp);
	}

STDMETHODIMP CAutoViews::get_Count (long * pCount)
	{
	if (pCount == NULL)
		return E_POINTER;
	
	ASSERT (m_pDoc);
	db_handitem* pDbHandItem = NULL;
	
	long lCount = 0;
	db_viewtabrec *pTableRecord = (db_viewtabrec *)m_pDoc->m_dbDrawing->tblnext( DB_VIEWTAB, 1 );
	
	while( pTableRecord != NULL )
		{
		ASSERT( pTableRecord->ret_type() == DB_VIEWTABREC );
		if (!pTableRecord->ret_deleted())
			lCount++;
		pTableRecord = (db_viewtabrec *)pTableRecord->next;
		}
	
	*pCount = lCount;
	return NOERROR;
	}

STDMETHODIMP CAutoViews::get_Parent (IIcadDocument * * ppParent)
	{
	if (ppParent == NULL)
		return E_POINTER;
	
	ASSERT (m_pDoc);
	return m_pDoc->m_pAutoDoc->QueryInterface (ppParent);
	}

STDMETHODIMP CAutoViews::get__NewEnum (IUnknown * * ppUnk)
	{
	if (ppUnk == NULL)
		return E_POINTER;
	
	CComObject<CAutoEnumViews>* penum;
	CComObject<CAutoEnumViews>::CreateInstance(&penum);
	if (!penum)
        return E_FAIL;    
	
	CComPtr<IIcadViews> pEnts;
	QueryInterface(IID_IIcadViews, (void **)&pEnts);
	penum->Init(pEnts);
	
	return penum->QueryInterface(IID_IUnknown, (void **)ppUnk);
	}

STDMETHODIMP CAutoViews::Add (BSTR Name, IIcadView * * ppItem)
	{
	if (ppItem == NULL)
		return E_POINTER;
	
	CComObject<CAutoView> *pView;
	CComObject<CAutoView>::CreateInstance(&pView);
	if (!pView)
		return E_FAIL;
	
	HRESULT hr;
	hr = pView->Init(m_pDoc, this, NULL);
	if (FAILED(hr))
		return hr;
	
	hr = pView->put_Name (Name);
	if (FAILED(hr))
		return hr;
	
	if (m_pDoc->m_dbDrawing->addhanditem (pView->m_pDbHandItem) == 0)
		return pView->QueryInterface(ppItem);
	else
		return E_FAIL;
	}

STDMETHODIMP CAutoViews::Item (VARIANT Index, IIcadView * * ppItem)
	{
	if (ppItem == NULL)
		return E_POINTER;
	
	ASSERT (m_pDoc);
	
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
	
	db_viewtabrec *pTableRecord = NULL;
	if (byIndex)
		{
		if (lIndex < 1)
			return E_INVALIDARG;

		long lCount = 0;
		pTableRecord = (db_viewtabrec *)m_pDoc->m_dbDrawing->tblnext( DB_VIEWTAB, 1 );
		if (!pTableRecord)
			return E_FAIL;

		if (!pTableRecord->ret_deleted())
			lCount++;
		
		while( (lCount < lIndex) && (pTableRecord != NULL) )
			{
			pTableRecord = (db_viewtabrec *)pTableRecord->next;
			ASSERT( pTableRecord->ret_type() == DB_VIEWTABREC );
			if (!pTableRecord->ret_deleted())
				lCount++;
			}
		
		//ASSERT (lCount == lIndex );
		if (lIndex > lCount)
			return E_INVALIDARG;
		}
	else if (byName)
		{
		CString str(Name);
		pTableRecord = (db_viewtabrec *)m_pDoc->m_dbDrawing->findtabrec( DB_VIEWTAB, str.GetBuffer(0), 1 );
		}
	
	if (pTableRecord)
		{
		CComObject<CAutoView> *pEnt;
		CComObject<CAutoView>::CreateInstance(&pEnt);
		if (!pEnt)
			return E_FAIL;
		
		CComPtr<IDispatch> pOwner;
		QueryInterface (IID_IDispatch, (void **)&pOwner);
		if (FAILED (pEnt->Init (m_pDoc, pOwner, pTableRecord)))
			return E_FAIL;
		
		return pEnt->QueryInterface(ppItem);
		}
	else
		return NOERROR;
	}



/////////////////////////////////////////////////////////////////////////////
// CAutoEnumViews

HRESULT CAutoEnumViews::Init (IIcadViews *pViews)
	{
	m_pViews = pViews;
	m_nCurrent = 0;
	return NOERROR;
	}

STDMETHODIMP CAutoEnumViews::Next (ULONG cElements, VARIANT * pvar, ULONG * pcElementFetched)
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
	m_pViews->get_Count(&nElements);
	
    for (l1 = m_nCurrent, l2 = 0; 
	l1 < nElements && l2 < cElements; 
	l1++, l2++)    
		{
		CComPtr<IIcadView> pEnt;
		CComVariant Index;
		V_VT(&Index) = VT_I4;
		V_I4(&Index) = l1 + 1;	
		if (FAILED (m_pViews->Item(Index, &pEnt)))
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


STDMETHODIMP CAutoEnumViews::Skip (ULONG cElements)
	{
    m_nCurrent += cElements;
	
	long nElements = 0;
	m_pViews->get_Count(&nElements);
	
	if (m_nCurrent > nElements)
		{        
		m_nCurrent = nElements;
        return ResultFromScode(S_FALSE);    
		}    
	else 
		return NOERROR;
	}


STDMETHODIMP CAutoEnumViews::Reset ()
	{
	m_nCurrent = 0;
	return NOERROR;
	}

STDMETHODIMP CAutoEnumViews::Clone (IEnumVARIANT * * ppenum)
	{
	if (ppenum == NULL)
		return E_POINTER;
	
	*ppenum = NULL;    
	CComObject<CAutoEnumViews>* penum;
	CComObject<CAutoEnumViews>::CreateInstance(&penum);
	if (!penum)
        return E_FAIL;    
	
	penum->Init(m_pViews);
	penum->AddRef();    
	
	*ppenum = penum;    
	return NOERROR;
	}


