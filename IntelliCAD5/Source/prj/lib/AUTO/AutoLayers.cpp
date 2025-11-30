/* INTELLICAD\PRJ\LIB\AUTO\AUTOLAYERS.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Implementation of CAutoLayers - The Automation Layers collection
 * 
 */ 

#include "StdAfx.h"
#include "AutoLayers.h"
#include "AutoLayer.h"
#include "layertabrec.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoLayers

HRESULT CAutoLayers::Init(CIcadDoc *pDoc)
	{
	if (!m_pDoc)
		{
		m_pDoc = pDoc;
		}
	return S_OK;
	}

STDMETHODIMP CAutoLayers::get_Application (IIcadApplication * * ppApp)
	{
	if (ppApp == NULL)
		return E_POINTER;
	
	return GetApp()->m_pAutoApp->QueryInterface (ppApp);
	}

STDMETHODIMP CAutoLayers::get_Count (long * pCount)
	{
	if (pCount == NULL)
		return E_POINTER;
	
	ASSERT (m_pDoc);
	db_handitem* pDbHandItem = NULL;
	
	long lCount = 0;
	db_layertabrec *pTableRecord = (db_layertabrec *)m_pDoc->m_dbDrawing->tblnext( DB_LAYERTAB, 1 );
	
	while( pTableRecord != NULL )
		{
		ASSERT( pTableRecord->ret_type() == DB_LAYERTABREC );
		if (!pTableRecord->ret_deleted())
			lCount++;
		pTableRecord = (db_layertabrec *)pTableRecord->next;
		}
	
	*pCount = lCount;
	return NOERROR;
	}

STDMETHODIMP CAutoLayers::get_Parent (IIcadDocument * * ppParent)
	{
	if (ppParent == NULL)
		return E_POINTER;
	
	ASSERT (m_pDoc);
	return m_pDoc->m_pAutoDoc->QueryInterface (ppParent);
	}

STDMETHODIMP CAutoLayers::get__NewEnum (IUnknown * * ppUnk)
	{
	if (ppUnk == NULL)
		return E_POINTER;
	
	CComObject<CAutoEnumLayers>* penum;
	CComObject<CAutoEnumLayers>::CreateInstance(&penum);
	if (!penum)
        return E_FAIL;    
	
	CComPtr<IIcadLayers> pEnts;
	QueryInterface(IID_IIcadLayers, (void **)&pEnts);
	penum->Init(pEnts);
	
	return penum->QueryInterface(IID_IUnknown, (void **)ppUnk);
	}

STDMETHODIMP CAutoLayers::Add (BSTR Name, IIcadLayer * * ppItem)
	{
	if (ppItem == NULL)
		return E_POINTER;
	
	CComObject<CAutoLayer> *pLayer;
	CComObject<CAutoLayer>::CreateInstance(&pLayer);
	if (!pLayer)
		return E_FAIL;
	
	HRESULT hr;
	hr = pLayer->Init(m_pDoc, this, NULL);
	if (FAILED(hr))
		return hr;
	
	hr = pLayer->put_Name (Name);
	if (FAILED(hr))
		return hr;
	
	if (m_pDoc->m_dbDrawing->addhanditem (pLayer->m_pDbHandItem) == 0)
		return pLayer->QueryInterface(ppItem);
	else
		return E_FAIL;
	}

STDMETHODIMP CAutoLayers::Item (VARIANT Index, IIcadLayer * * ppItem)
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
	
	db_layertabrec *pTableRecord = NULL;
	if (byIndex)
		{
		if (lIndex < 1)
			return E_INVALIDARG;

		long lCount = 0;
		pTableRecord = (db_layertabrec *)m_pDoc->m_dbDrawing->tblnext( DB_LAYERTAB, 1 );
		if (!pTableRecord)
			return E_FAIL;

		if (!pTableRecord->ret_deleted())
			lCount++;
		
		while( (lCount < lIndex) && (pTableRecord != NULL) )
			{
			pTableRecord = (db_layertabrec *)pTableRecord->next;
			ASSERT( pTableRecord->ret_type() == DB_LAYERTABREC );
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
		pTableRecord = (db_layertabrec *)m_pDoc->m_dbDrawing->findtabrec( DB_LAYERTAB, str.GetBuffer(0), 1 );
		}
	
	if (pTableRecord)
		{
		CComObject<CAutoLayer> *pEnt;
		CComObject<CAutoLayer>::CreateInstance(&pEnt);
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
// CAutoEnumLayers

HRESULT CAutoEnumLayers::Init (IIcadLayers *pLayers)
	{
	m_pLayers = pLayers;
	m_nCurrent = 0;
	return NOERROR;
	}

STDMETHODIMP CAutoEnumLayers::Next (ULONG cElements, VARIANT * pvar, ULONG * pcElementFetched)
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
	m_pLayers->get_Count(&nElements);
	
    for (l1 = m_nCurrent, l2 = 0; 
	l1 < nElements && l2 < cElements; 
	l1++, l2++)    
		{
		CComPtr<IIcadLayer> pEnt;
		CComVariant Index;
		V_VT(&Index) = VT_I4;
		V_I4(&Index) = l1 + 1;	
		if (FAILED (m_pLayers->Item(Index, &pEnt)))
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


STDMETHODIMP CAutoEnumLayers::Skip (ULONG cElements)
	{
    m_nCurrent += cElements;
	
	long nElements = 0;
	m_pLayers->get_Count(&nElements);
	
	if (m_nCurrent > nElements)
		{        
		m_nCurrent = nElements;
        return ResultFromScode(S_FALSE);    
		}    
	else 
		return NOERROR;
	}


STDMETHODIMP CAutoEnumLayers::Reset ()
	{
	m_nCurrent = 0;
	return NOERROR;
	}

STDMETHODIMP CAutoEnumLayers::Clone (IEnumVARIANT * * ppenum)
	{
	if (ppenum == NULL)
		return E_POINTER;
	
	*ppenum = NULL;    
	CComObject<CAutoEnumLayers>* penum;
	CComObject<CAutoEnumLayers>::CreateInstance(&penum);
	if (!penum)
        return E_FAIL;    
	
	penum->Init(m_pLayers);
	penum->AddRef();    
	
	*ppenum = penum;    
	return NOERROR;
	}

