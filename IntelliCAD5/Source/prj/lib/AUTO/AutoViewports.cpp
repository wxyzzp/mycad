/* INTELLICAD\PRJ\LIB\AUTO\AUTOVIEWPORTS.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Implementation of CAutoViewports - The Automation Viewports collection
 * 
 */ 

#include "StdAfx.h"
#include "AutoViewports.h"
#include "AutoViewport.h"
#include "vporttabrec.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoViewports

HRESULT CAutoViewports::Init(CIcadDoc *pDoc)
	{
	if (!m_pDoc)
		{
		m_pDoc = pDoc;
		}
	return S_OK;
	}

STDMETHODIMP CAutoViewports::get_Application (IIcadApplication * * ppApp)
	{
	if (ppApp == NULL)
		return E_POINTER;
	
	return GetApp()->m_pAutoApp->QueryInterface (ppApp);
	}

STDMETHODIMP CAutoViewports::get_Count (long * pCount)
	{
	if (pCount == NULL)
		return E_POINTER;
	
	ASSERT (m_pDoc);
	db_handitem* pDbHandItem = NULL;
	
	long lCount = 0;
	db_vporttabrec *pTableRecord = (db_vporttabrec *)m_pDoc->m_dbDrawing->tblnext( DB_VPORTTAB, 1 );
	
	while( pTableRecord != NULL )
		{
		ASSERT( pTableRecord->ret_type() == DB_VPORTTABREC );
		if (!pTableRecord->ret_deleted())
			lCount++;
		pTableRecord = (db_vporttabrec *)pTableRecord->next;
		}
	
	*pCount = lCount;
	return NOERROR;
	}

STDMETHODIMP CAutoViewports::get_Parent (IIcadDocument * * ppParent)
	{
	if (ppParent == NULL)
		return E_POINTER;
	
	ASSERT (m_pDoc);
	return m_pDoc->m_pAutoDoc->QueryInterface (ppParent);
	}

STDMETHODIMP CAutoViewports::get__NewEnum (IUnknown * * ppUnk)
	{
	if (ppUnk == NULL)
		return E_POINTER;
	
	CComObject<CAutoEnumViewports>* penum;
	CComObject<CAutoEnumViewports>::CreateInstance(&penum);
	if (!penum)
        return E_FAIL;    
	
	CComPtr<IIcadViewports> pEnts;
	QueryInterface(IID_IIcadViewports, (void **)&pEnts);
	penum->Init(pEnts);
	
	return penum->QueryInterface(IID_IUnknown, (void **)ppUnk);
	}

STDMETHODIMP CAutoViewports::Add (BSTR Name, IIcadViewport * * ppItem)
	{
	if (ppItem == NULL)
		return E_POINTER;

	db_vporttabrec *pTableRecord = NULL;
	CString str(Name);
	pTableRecord = (db_vporttabrec *)m_pDoc->m_dbDrawing->findtabrec( DB_VPORTTAB, str.GetBuffer(0), 1 );
	if (pTableRecord)
		return E_INVALIDARG;	//a viewport with that name already exists
	
	CComObject<CAutoViewport> *pViewport;
	CComObject<CAutoViewport>::CreateInstance(&pViewport);
	if (!pViewport)
		return E_FAIL;
	
	HRESULT hr;
	hr = pViewport->Init(m_pDoc, this, NULL);
	if (FAILED(hr))
		return hr;
	
	if (!pViewport->m_pDbHandItem->set_2 (str.GetBuffer(0)))
		return E_FAIL;
	
	if (m_pDoc->m_dbDrawing->addhanditem (pViewport->m_pDbHandItem) == 0)
		return pViewport->QueryInterface(ppItem);
	else
		return E_FAIL;
	}

STDMETHODIMP CAutoViewports::Item (VARIANT Index, IIcadViewport * * ppItem)
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
	
	db_vporttabrec *pTableRecord = NULL;
	if (byIndex)
		{
		if (lIndex < 1)
			return E_INVALIDARG;

		long lCount = 0;
		pTableRecord = (db_vporttabrec *)m_pDoc->m_dbDrawing->tblnext( DB_VPORTTAB, 1 );
		if (!pTableRecord)
			return E_FAIL;

		if (!pTableRecord->ret_deleted())
			lCount++;
		
		while( (lCount < lIndex) && (pTableRecord != NULL) )
			{
			pTableRecord = (db_vporttabrec *)pTableRecord->next;
			ASSERT( pTableRecord->ret_type() == DB_VPORTTABREC );
			if (!pTableRecord->ret_deleted())
				lCount++;
			}
		
		//ASSERT (lCount == lIndex );
		if (lIndex - 1 > lCount)
			return E_INVALIDARG;
		}
	else if (byName)
		{
		CString str(Name);
		pTableRecord = (db_vporttabrec *)m_pDoc->m_dbDrawing->findtabrec( DB_VPORTTAB, str.GetBuffer(0), 1 );
		}
	
	if (pTableRecord)
		{
		CComObject<CAutoViewport> *pEnt;
		CComObject<CAutoViewport>::CreateInstance(&pEnt);
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
// CAutoEnumViewports

HRESULT CAutoEnumViewports::Init (IIcadViewports *pViewports)
	{
	m_pViewports = pViewports;
	m_nCurrent = 0;
	return NOERROR;
	}

STDMETHODIMP CAutoEnumViewports::Next (ULONG cElements, VARIANT * pvar, ULONG * pcElementFetched)
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
	m_pViewports->get_Count(&nElements);
	
    for (l1 = m_nCurrent, l2 = 0; 
	l1 < nElements && l2 < cElements; 
	l1++, l2++)    
		{
		CComPtr<IIcadViewport> pEnt;
		CComVariant Index;
		V_VT(&Index) = VT_I4;
		V_I4(&Index) = l1 + 1;	
		if (FAILED (m_pViewports->Item(Index, &pEnt)))
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


STDMETHODIMP CAutoEnumViewports::Skip (ULONG cElements)
	{
    m_nCurrent += cElements;
	
	long nElements = 0;
	m_pViewports->get_Count(&nElements);
	
	if (m_nCurrent > nElements)
		{        
		m_nCurrent = nElements;
        return ResultFromScode(S_FALSE);    
		}    
	else 
		return NOERROR;
	}


STDMETHODIMP CAutoEnumViewports::Reset ()
	{
	m_nCurrent = 0;
	return NOERROR;
	}

STDMETHODIMP CAutoEnumViewports::Clone (IEnumVARIANT * * ppenum)
	{
	if (ppenum == NULL)
		return E_POINTER;
	
	*ppenum = NULL;    
	CComObject<CAutoEnumViewports>* penum;
	CComObject<CAutoEnumViewports>::CreateInstance(&penum);
	if (!penum)
        return E_FAIL;    
	
	penum->Init(m_pViewports);
	penum->AddRef();    
	
	*ppenum = penum;    
	return NOERROR;
	}



