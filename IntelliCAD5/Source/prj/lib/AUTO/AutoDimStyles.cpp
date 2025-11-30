/* INTELLICAD\PRJ\LIB\AUTO\AUTODIMSTYLES.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Implementation of CAutoDimensionStyles - The Automation DimensionStyles collection
 * 
 */ 

#include "StdAfx.h"
#include "AutoDimStyles.h"
#include "AutoDimStyle.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoDimensionStyles

HRESULT CAutoDimensionStyles::Init(CIcadDoc *pDoc)
	{
	if (!m_pDoc)
		{
		m_pDoc = pDoc;
		}
	return S_OK;
	}

STDMETHODIMP CAutoDimensionStyles::get_Application (IIcadApplication * * ppApp)
	{
	if (ppApp == NULL)
		return E_POINTER;
	
	return GetApp()->m_pAutoApp->QueryInterface (ppApp);
	}

STDMETHODIMP CAutoDimensionStyles::get_Count (long * pCount)
	{
	if (pCount == NULL)
		return E_POINTER;
	
	ASSERT (m_pDoc);
	db_handitem* pDbHandItem = NULL;
	
	long lCount = 0;
	db_dimstyletabrec *pTableRecord = (db_dimstyletabrec *)m_pDoc->m_dbDrawing->tblnext( DB_DIMSTYLETAB, 1 );
	
	while( pTableRecord != NULL )
		{
		ASSERT( pTableRecord->ret_type() == DB_DIMSTYLETAB );
		if (!pTableRecord->ret_deleted())
			lCount++;
		pTableRecord = (db_dimstyletabrec *)pTableRecord->next;
		}
	
	*pCount = lCount;
	return NOERROR;
	}

STDMETHODIMP CAutoDimensionStyles::get_Parent (IIcadDocument * * ppParent)
	{
	if (ppParent == NULL)
		return E_POINTER;
	
	ASSERT (m_pDoc);
	return m_pDoc->m_pAutoDoc->QueryInterface (ppParent);
	}

STDMETHODIMP CAutoDimensionStyles::get__NewEnum (IUnknown * * ppUnk)
	{
	if (ppUnk == NULL)
		return E_POINTER;
	
	CComObject<CAutoEnumDimensionStyles>* penum;
	CComObject<CAutoEnumDimensionStyles>::CreateInstance(&penum);
	if (!penum)
        return E_FAIL;    
	
	CComPtr<IIcadDimensionStyles> pEnts;
	QueryInterface(IID_IIcadDimensionStyles, (void **)&pEnts);
	penum->Init(pEnts);
	
	return penum->QueryInterface(IID_IUnknown, (void **)ppUnk);
	}

STDMETHODIMP CAutoDimensionStyles::Add (BSTR Name, IIcadDimensionStyle * * ppItem)
	{
	if (ppItem == NULL)
		return E_POINTER;
	
	CComObject<CAutoDimensionStyle> *pDimensionStyle;
	CComObject<CAutoDimensionStyle>::CreateInstance(&pDimensionStyle);
	if (!pDimensionStyle)
		return E_FAIL;
	
	HRESULT hr;
	hr = pDimensionStyle->Init(m_pDoc, this, NULL);
	if (FAILED(hr))
		return hr;
	
	hr = pDimensionStyle->put_Name (Name);
	if (FAILED(hr))
		return hr;
	
	if (m_pDoc->m_dbDrawing->addhanditem (pDimensionStyle->m_pDbHandItem) == 0)
		return pDimensionStyle->QueryInterface(ppItem);
	else
		return E_FAIL;
	}

STDMETHODIMP CAutoDimensionStyles::Item (VARIANT Index, IIcadDimensionStyle * * ppItem)
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
	
	db_dimstyletabrec *pTableRecord = NULL;
	if (byIndex)
		{
		if (lIndex < 1)
			return E_INVALIDARG;

		long lCount = 0;
		pTableRecord = (db_dimstyletabrec *)m_pDoc->m_dbDrawing->tblnext( DB_DIMSTYLETAB, 1 );
		if (!pTableRecord)
			return E_FAIL;

		if (!pTableRecord->ret_deleted())
			lCount++;
		
		while( (lCount < lIndex) && (pTableRecord != NULL) )
			{
			pTableRecord = (db_dimstyletabrec *)pTableRecord->next;
			ASSERT( pTableRecord->ret_type() == DB_DIMSTYLETABREC );
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
		pTableRecord = (db_dimstyletabrec *)m_pDoc->m_dbDrawing->findtabrec( DB_DIMSTYLETAB, str.GetBuffer(0), 1 );
		}
	
	if (pTableRecord)
		{
		CComObject<CAutoDimensionStyle> *pEnt;
		CComObject<CAutoDimensionStyle>::CreateInstance(&pEnt);
		if (!pEnt)
			return E_FAIL;
		
		if (FAILED (pEnt->Init (m_pDoc, this, pTableRecord)))
			return E_FAIL;
		
		return pEnt->QueryInterface(ppItem);
		}
	else
		return NOERROR;
	}


/////////////////////////////////////////////////////////////////////////////
// CAutoEnumDimensionStyles

HRESULT CAutoEnumDimensionStyles::Init (IIcadDimensionStyles *pDimensionStyles)
	{
	m_pDimensionStyles = pDimensionStyles;
	m_nCurrent = 0;
	return NOERROR;
	}

STDMETHODIMP CAutoEnumDimensionStyles::Next (ULONG cElements, VARIANT * pvar, ULONG * pcElementFetched)
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
	m_pDimensionStyles->get_Count(&nElements);
	
    for (l1 = m_nCurrent, l2 = 0; 
	l1 < nElements && l2 < cElements; 
	l1++, l2++)    
		{
		IIcadDimensionStyle * pEnt = NULL;
		CComVariant Index;
		V_VT(&Index) = VT_I4;
		V_I4(&Index) = l1 + 1;	
		if (FAILED (m_pDimensionStyles->Item(Index, &pEnt)))
			return S_FALSE;
		
		CComPtr<IDispatch> pDispEnt;
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


STDMETHODIMP CAutoEnumDimensionStyles::Skip (ULONG cElements)
	{
    m_nCurrent += cElements;
	
	long nElements = 0;
	m_pDimensionStyles->get_Count(&nElements);
	
	if (m_nCurrent > nElements)
		{        
		m_nCurrent = nElements;
        return ResultFromScode(S_FALSE);    
		}    
	else 
		return NOERROR;
	}


STDMETHODIMP CAutoEnumDimensionStyles::Reset ()
	{
	m_nCurrent = 0;
	return NOERROR;
	}

STDMETHODIMP CAutoEnumDimensionStyles::Clone (IEnumVARIANT * * ppenum)
	{
	if (ppenum == NULL)
		return E_POINTER;
	
	*ppenum = NULL;    
	CComObject<CAutoEnumDimensionStyles>* penum;
	CComObject<CAutoEnumDimensionStyles>::CreateInstance(&penum);
	if (!penum)
        return E_FAIL;    
	
	penum->Init(m_pDimensionStyles);
	penum->AddRef();    
	
	*ppenum = penum;    
	return NOERROR;
	}


