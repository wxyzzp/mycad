/* INTELLICAD\PRJ\LIB\AUTO\AUTOSELSETS.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Implementation of the CAutoSelectionSets - the Automation Selection Sets Collection
 * 
 */ 

#include "StdAfx.h"
#include "AutoSelSet.h"
#include "AutoSelSets.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoSelectionSets

HRESULT CAutoSelectionSets::Init (CIcadDoc *pDoc)
	{
	m_pDoc = pDoc;
	return NOERROR;
	}

STDMETHODIMP CAutoSelectionSets::get_Application (IIcadApplication * * ppApp)
	{
	if (ppApp == NULL)
		return E_POINTER;
	
	return GetApp()->m_pAutoApp->QueryInterface (ppApp);
	}

STDMETHODIMP CAutoSelectionSets::get_Count (long * pCount)
	{
	if (pCount == NULL)
		return E_POINTER;
	
	*pCount = m_Contents.GetSize();
	
	return NOERROR;
	}

STDMETHODIMP CAutoSelectionSets::get_Parent (IIcadDocument * * ppParent)
	{
	if (ppParent == NULL)
		return E_POINTER;
	
	return m_pDoc->m_pAutoDoc->QueryInterface (ppParent);
	}

STDMETHODIMP CAutoSelectionSets::get__NewEnum (IUnknown * * ppUnk)
	{
	if (ppUnk == NULL)
		return E_POINTER;
	
	CComObject<CAutoEnumSelectionSets>* penum;
	CComObject<CAutoEnumSelectionSets>::CreateInstance(&penum);
	if (!penum)
        return E_FAIL;    
	
	penum->Init(this);
	
	return penum->QueryInterface(IID_IUnknown, (void **)ppUnk);
	}

STDMETHODIMP CAutoSelectionSets::Add (BSTR Name, IIcadSelectionSet * * ppItem)
	{
	if (ppItem == NULL)
		return E_POINTER;
	
	CComObject<CAutoSelectionSet>* pSelectionSet;
	CComObject<CAutoSelectionSet>::CreateInstance(&pSelectionSet);
	if (!pSelectionSet)
		return E_FAIL;
	
	pSelectionSet->Init(m_pDoc, Name);
	
	IIcadSelectionSet *pIIcadSelectionSet;
	pSelectionSet->QueryInterface( &pIIcadSelectionSet );
	m_Contents.Add( pIIcadSelectionSet );
	
	return pSelectionSet->QueryInterface(ppItem);
	}

STDMETHODIMP CAutoSelectionSets::Item (VARIANT Index, IIcadSelectionSet * * ppItem)
	{
	if (ppItem == NULL)
		return E_POINTER;
	
	CComPtr<IIcadSelectionSet> pSelectionSet;
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
		if (lIndex < 1 || lIndex > m_Contents.GetSize() )
			return E_INVALIDARG; 	

		pSelectionSet = m_Contents[lIndex - 1];
		}
	else if (byName)
		{
		int i;
		CString str1(Name);
		lIndex = m_Contents.GetSize();
		for (i = 0; i < lIndex; i++)
			{
			BSTR lName;
			pSelectionSet = m_Contents[i];
			pSelectionSet->get_Name (&lName);
			CString str2(lName);
			if (str1 == str2)
				break;
			}
		}
	
	if (pSelectionSet)
		return pSelectionSet->QueryInterface(ppItem);
	else
		return E_FAIL;
	}



/////////////////////////////////////////////////////////////////////////////
// CAutoEnumSelectionSets

HRESULT CAutoEnumSelectionSets::Init (IIcadSelectionSets *pSelectionSets)
	{
	m_pSelSets = pSelectionSets;
	m_nCurrent = 0;
	return NOERROR;
	}

STDMETHODIMP CAutoEnumSelectionSets::Next (ULONG cElements, VARIANT * pvar, ULONG * pcElementFetched)
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
	m_pSelSets->get_Count(&nElements);
	
    for (l1 = m_nCurrent, l2 = 0; 
	l1 < nElements && l2 < cElements; 
	l1++, l2++)    
		{
		CComPtr<IIcadSelectionSet> pSelectionSet;
		CComVariant Index;
		V_VT(&Index) = VT_I4;
		V_I4(&Index) = l1 + 1;	
		m_pSelSets->Item(Index, &pSelectionSet);
		
		IDispatch *pDispSelectionSet;
		hr = pSelectionSet->QueryInterface(&pDispSelectionSet);
		if (FAILED(hr))
            goto error;     
		
		pvar[l2].vt = VT_DISPATCH;
		pvar[l2].pdispVal = pDispSelectionSet;
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


STDMETHODIMP CAutoEnumSelectionSets::Skip (ULONG cElements)
	{
    m_nCurrent += cElements;
	
	long nElements = 0;
	m_pSelSets->get_Count(&nElements);
	
	if (m_nCurrent > nElements)
		{        
		m_nCurrent = nElements;
        return ResultFromScode(S_FALSE);    
		}    
	else 
		return NOERROR;
	}


STDMETHODIMP CAutoEnumSelectionSets::Reset ()
	{
	m_nCurrent = 0;
	return NOERROR;
	}

STDMETHODIMP CAutoEnumSelectionSets::Clone (IEnumVARIANT * * ppenum)
	{
	if (ppenum == NULL)
		return E_POINTER;
	
	*ppenum = NULL;    
	CComObject<CAutoEnumSelectionSets>* penum;
	CComObject<CAutoEnumSelectionSets>::CreateInstance(&penum);
	if (!penum)
        return E_FAIL;    
	
	penum->Init(m_pSelSets);
	penum->AddRef();    
	
	*ppenum = penum;    
	return NOERROR;
	}



