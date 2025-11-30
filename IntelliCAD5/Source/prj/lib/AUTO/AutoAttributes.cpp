/* INTELLICAD\PRJ\LIB\AUTO\AUTOATTRIBUTES.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Implementation of CAutoAttributes - The Automation Attributes collection
 * 
 */ 

#include "StdAfx.h"
#include "AutoAttributes.h"
#include "AutoAttribute.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoAttributes

HRESULT CAutoAttributes::Init (CIcadDoc *pDoc, IDispatch *pParentBlockInsert, db_handitem *pDbHandItem)
	{
	m_pDoc = pDoc;
	m_pBlockInsert = pParentBlockInsert;
	m_pDbHandItem = pDbHandItem;
	return NOERROR;
	}

STDMETHODIMP CAutoAttributes::get_Application (IIcadApplication * * ppApp)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppApp == NULL)
		return E_POINTER;
	
	return GetApp()->m_pAutoApp->QueryInterface (ppApp);
	}

STDMETHODIMP CAutoAttributes::get_Parent (IDispatch * * ppParent)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppParent == NULL)
		return E_POINTER;
	
	return m_pBlockInsert->QueryInterface (ppParent);
	}

STDMETHODIMP CAutoAttributes::get_Count (LONG * pCount)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pCount == NULL)
		return E_POINTER;
	
	long lCount = 0;
	db_handitem* pDbHandItem = m_pDbHandItem;
	while (NULL != pDbHandItem && DB_ATTRIB == pDbHandItem->ret_type())
		{
		lCount++;
		pDbHandItem = m_pDoc->m_dbDrawing->entnext_noxref(pDbHandItem);
		}

	*pCount = lCount;
	return NOERROR;
	}

STDMETHODIMP CAutoAttributes::get__NewEnum (IUnknown * * ppUnk)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppUnk == NULL)
		return E_POINTER;
	
	CComObject<CAutoEnumAttributes>* penum;
	CComObject<CAutoEnumAttributes>::CreateInstance(&penum);
	if (!penum)
        return E_FAIL;    
	
	CComPtr<IIcadAttributes> pEnts;
	QueryInterface(IID_IIcadAttributes, (void **)&pEnts);
	penum->Init(pEnts);
	
	return penum->QueryInterface(IID_IUnknown, (void **)ppUnk);
	}

STDMETHODIMP CAutoAttributes::Add (IIcadAttribute * * Attribute)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (Attribute == NULL)
		return E_POINTER;
	
	CComObject<CAutoAttribute> *pAttribute;
	CComObject<CAutoAttribute>::CreateInstance(&pAttribute);
	if (!pAttribute)
		return E_FAIL;
	
	HRESULT hr;
	hr = pAttribute->Init(m_pDoc, this, NULL);
	if (FAILED(hr))
		return hr;
	
	if (m_pDoc->m_dbDrawing->addhanditem (pAttribute->m_pDbHandItem) != 0)
		return E_FAIL;

	db_handitem* prevhip = NULL;
	db_handitem* pDbHandItem = m_pDbHandItem;
	while (NULL != pDbHandItem && DB_ATTRIB == pDbHandItem->ret_type())
		{
		prevhip = pDbHandItem;
		pDbHandItem = m_pDoc->m_dbDrawing->entnext_noxref(pDbHandItem);
		}

	prevhip->next = pAttribute->m_pDbHandItem;
	pAttribute->m_pDbHandItem->next = pDbHandItem;

	return pAttribute->QueryInterface(Attribute);
	}

STDMETHODIMP CAutoAttributes::Item (VARIANT Index, IIcadAttribute * * ppItem)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppItem == NULL)
		return E_POINTER;
	
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
	
	db_handitem* pDbHandItem = NULL;
	if (byIndex)
		{
		lIndex--;
		long lCount = 0;
		pDbHandItem = m_pDbHandItem;
		while ((lCount < lIndex) && pDbHandItem && (DB_ATTRIB == pDbHandItem->ret_type()))
			{
			lCount++;
			pDbHandItem = m_pDoc->m_dbDrawing->entnext_noxref(pDbHandItem);
			}
		}
	else if (byName)
		{
		//TODO
		}

	if (pDbHandItem)
		{
		CComObject<CAutoAttribute> *pEnt;
		CComObject<CAutoAttribute>::CreateInstance(&pEnt);
		if (!pEnt)
			return E_FAIL;
		
		CComPtr<IDispatch> pOwner;
		QueryInterface (IID_IDispatch, (void **)&pOwner);
		if (FAILED (pEnt->Init (m_pDoc, pOwner, pDbHandItem)))
			return E_FAIL;
		
		return pEnt->QueryInterface(ppItem);
		}
	else
		return NOERROR;
	}


/////////////////////////////////////////////////////////////////////////////
// CAutoEnumAttributes

HRESULT CAutoEnumAttributes::Init (IIcadAttributes *pAttributes)
	{
	m_pAttributes = pAttributes;
	m_nCurrent = 0;
	return NOERROR;
	}

STDMETHODIMP CAutoEnumAttributes::Next (ULONG cElements, VARIANT * pvar, ULONG * pcElementFetched)
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
	m_pAttributes->get_Count(&nElements);
	
    for (l1 = m_nCurrent, l2 = 0; 
	l1 < nElements && l2 < cElements; 
	l1++, l2++)    
		{
		CComPtr<IIcadAttribute> pEnt;
		CComVariant Index;
		V_VT(&Index) = VT_I4;
		V_I4(&Index) = l1 + 1;	
		if (FAILED (m_pAttributes->Item(Index, &pEnt)))
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


STDMETHODIMP CAutoEnumAttributes::Skip (ULONG cElements)
	{
    m_nCurrent += cElements;
	
	long nElements = 0;
	m_pAttributes->get_Count(&nElements);
	
	if (m_nCurrent > nElements)
		{        
		m_nCurrent = nElements;
        return ResultFromScode(S_FALSE);    
		}    
	else 
		return NOERROR;
	}


STDMETHODIMP CAutoEnumAttributes::Reset ()
	{
	m_nCurrent = 0;
	return NOERROR;
	}

STDMETHODIMP CAutoEnumAttributes::Clone (IEnumVARIANT * * ppenum)
	{
	if (ppenum == NULL)
		return E_POINTER;
	
	*ppenum = NULL;    
	CComObject<CAutoEnumAttributes>* penum;
	CComObject<CAutoEnumAttributes>::CreateInstance(&penum);
	if (!penum)
        return E_FAIL;    
	
	penum->Init(m_pAttributes);
	penum->AddRef();    
	
	*ppenum = penum;    
	return NOERROR;
	}



