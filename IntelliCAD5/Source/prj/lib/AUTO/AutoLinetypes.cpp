/* INTELLICAD\PRJ\LIB\AUTO\AUTOLINETYPES.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Implementation of CAutoLinetypes - The Automation Linetypes collection
 * 
 */ 

#include "StdAfx.h"
#include "AutoLinetypes.h"
#include "AutoLinetype.h"
#include "ltypetabrec.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoLinetypes

HRESULT CAutoLinetypes::Init(CIcadDoc *pDoc)
	{
	if (!m_pDoc)
		{
		m_pDoc = pDoc;
		}
	return S_OK;
	}

STDMETHODIMP CAutoLinetypes::get_Application (IIcadApplication * * ppApp)
	{
	if (ppApp == NULL)
		return E_POINTER;
	
	return GetApp()->m_pAutoApp->QueryInterface (ppApp);
	}

STDMETHODIMP CAutoLinetypes::get_Count (long * pCount)
	{
	if (pCount == NULL)
		return E_POINTER;
	
	ASSERT (m_pDoc);
	db_handitem* pDbHandItem = NULL;
	
	long lCount = 0;
	db_ltypetabrec *pTableRecord = (db_ltypetabrec *)m_pDoc->m_dbDrawing->tblnext( DB_LTYPETAB, 1 );
	
	while( pTableRecord != NULL )
		{
		ASSERT( pTableRecord->ret_type() == DB_LTYPETABREC );
		if (!pTableRecord->ret_deleted())
			lCount++;
		pTableRecord = (db_ltypetabrec *)pTableRecord->next;
		}
	
	*pCount = lCount;
	return NOERROR;
	}

STDMETHODIMP CAutoLinetypes::get_Parent (IIcadDocument * * ppParent)
	{
	if (ppParent == NULL)
		return E_POINTER;
	
	ASSERT (m_pDoc);
	return m_pDoc->m_pAutoDoc->QueryInterface (ppParent);
	}

STDMETHODIMP CAutoLinetypes::get__NewEnum (IUnknown * * ppUnk)
	{
	if (ppUnk == NULL)
		return E_POINTER;
	
	CComObject<CAutoEnumLinetypes>* penum;
	CComObject<CAutoEnumLinetypes>::CreateInstance(&penum);
	if (!penum)
        return E_FAIL;    
	
	CComPtr<IIcadLinetypes> pEnts;
	QueryInterface(IID_IIcadLinetypes, (void **)&pEnts);
	penum->Init(pEnts);
	
	return penum->QueryInterface(IID_IUnknown, (void **)ppUnk);
	}

STDMETHODIMP CAutoLinetypes::Add (BSTR Name, IIcadLinetype * * ppItem)
	{
	if (ppItem == NULL)
		return E_POINTER;
	
	CComObject<CAutoLinetype> *pLinetype;
	CComObject<CAutoLinetype>::CreateInstance(&pLinetype);
	if (!pLinetype)
		return E_FAIL;
	
	HRESULT hr;
	hr = pLinetype->Init(m_pDoc, this, NULL);
	if (FAILED(hr))
		return hr;
	
	hr = pLinetype->put_Name (Name);
	if (FAILED(hr))
		return hr;
	
	if (m_pDoc->m_dbDrawing->addhanditem (pLinetype->m_pDbHandItem) == 0)
		return pLinetype->QueryInterface(ppItem);
	else
		return E_FAIL;
	}

STDMETHODIMP CAutoLinetypes::Item (VARIANT Index, IIcadLinetype * * ppItem)
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
	
	db_ltypetabrec *pTableRecord = NULL;
	if (byIndex)
		{
		if (lIndex < 1)
			return E_INVALIDARG;

		long lCount = 0;
		pTableRecord = (db_ltypetabrec *)m_pDoc->m_dbDrawing->tblnext( DB_LTYPETAB, 1 );
		if (!pTableRecord)
			return E_FAIL;

		if (!pTableRecord->ret_deleted())
			lCount++;
		
		while( (lCount < lIndex) && (pTableRecord != NULL) )
			{
			pTableRecord = (db_ltypetabrec *)pTableRecord->next;
			ASSERT( pTableRecord->ret_type() == DB_LTYPETABREC );
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
		pTableRecord = (db_ltypetabrec *)m_pDoc->m_dbDrawing->findtabrec( DB_LTYPETAB, str.GetBuffer(0), 1 );
		}
	
	if (pTableRecord)
		{
		CComObject<CAutoLinetype> *pEnt;
		CComObject<CAutoLinetype>::CreateInstance(&pEnt);
		if (!pEnt)
			return E_FAIL;
		
		if (FAILED (pEnt->Init (m_pDoc, this, pTableRecord)))
			return E_FAIL;
		
		return pEnt->QueryInterface(ppItem);
		}
	else
		return NOERROR;
	}

STDMETHODIMP CAutoLinetypes::Load(BSTR LinetypeName, BSTR FileName)
	{
	CString LType (LinetypeName);
	CString Name (FileName);

	if (LType.IsEmpty())
		return E_INVALIDARG;

	if (Name.IsEmpty())
		return E_INVALIDARG;

	return m_pDoc->LoadorSaveLinetype (LType.GetBuffer(0), Name.GetBuffer(0), 0);
	}


/////////////////////////////////////////////////////////////////////////////
// CAutoEnumLinetypes

HRESULT CAutoEnumLinetypes::Init (IIcadLinetypes *pLinetypes)
	{
	m_pLinetypes = pLinetypes;
	m_nCurrent = 0;
	return NOERROR;
	}

STDMETHODIMP CAutoEnumLinetypes::Next (ULONG cElements, VARIANT * pvar, ULONG * pcElementFetched)
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
	m_pLinetypes->get_Count(&nElements);
	
    for (l1 = m_nCurrent, l2 = 0; 
	l1 < nElements && l2 < cElements; 
	l1++, l2++)    
		{
		IIcadLinetype * pEnt = NULL;
		CComVariant Index;
		V_VT(&Index) = VT_I4;
		V_I4(&Index) = l1 + 1;	
		if (FAILED (m_pLinetypes->Item(Index, &pEnt)))
			return S_FALSE;
		
		CComPtr<IDispatch> pDispEnt;
		hr = pEnt->QueryInterface(IID_IDispatch, (void **)&pDispEnt);
		if (FAILED(hr))
            goto error;     
		
		pvar[l2].vt = VT_DISPATCH;
		pvar[l2].pdispVal = pDispEnt;
		//pEnt->AddRef();
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


STDMETHODIMP CAutoEnumLinetypes::Skip (ULONG cElements)
	{
    m_nCurrent += cElements;
	
	long nElements = 0;
	m_pLinetypes->get_Count(&nElements);
	
	if (m_nCurrent > nElements)
		{        
		m_nCurrent = nElements;
        return ResultFromScode(S_FALSE);    
		}    
	else 
		return NOERROR;
	}


STDMETHODIMP CAutoEnumLinetypes::Reset ()
	{
	m_nCurrent = 0;
	return NOERROR;
	}

STDMETHODIMP CAutoEnumLinetypes::Clone (IEnumVARIANT * * ppenum)
	{
	if (ppenum == NULL)
		return E_POINTER;
	
	*ppenum = NULL;    
	CComObject<CAutoEnumLinetypes>* penum;
	CComObject<CAutoEnumLinetypes>::CreateInstance(&penum);
	if (!penum)
        return E_FAIL;    
	
	penum->Init(m_pLinetypes);
	penum->AddRef();    
	
	*ppenum = penum;    
	return NOERROR;
	}

