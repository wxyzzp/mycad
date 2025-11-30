/* INTELLICAD\PRJ\LIB\AUTO\AUTOPOINTS.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 *  Implementation of CAutoPoints - the Automation Points Collection
 * 
 */ 


#include "StdAfx.h"
#include "AutoLibrary.h"
#include "AutoPoints.h"
#include "AutoPoint.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoPoints

HRESULT CAutoPoints::Init(IIcadLibrary *pLib)
	{
	m_pLib = pLib;
	return NOERROR;
	}

STDMETHODIMP CAutoPoints::get_Application (IIcadApplication * * ppApp)
	{
	if (ppApp == NULL)
		return E_POINTER;
	
	return GetApp()->m_pAutoApp->QueryInterface (ppApp);
	}

STDMETHODIMP CAutoPoints::get_Parent (IIcadLibrary * * ppParent)
	{
	if (ppParent == NULL)
		return E_POINTER;
	
	if (m_pLib)
		return m_pLib->QueryInterface(IID_IIcadLibrary, (void **) ppParent);
	else
		{
		HRESULT hr;
		IIcadApplication *pApp = NULL;
		hr = this->get_Application(&pApp);
		if (pApp)
			hr = pApp->get_Library(ppParent);
		
		return hr;
		}
	}

STDMETHODIMP CAutoPoints::get_Count (long * pCount)
	{
	if (pCount == NULL)
		return E_POINTER;
	
	*pCount = m_Contents.GetSize();
	
	return NOERROR;
	}

STDMETHODIMP CAutoPoints::get__NewEnum (IUnknown * * ppUnk)
	{
	if (ppUnk == NULL)
		return E_POINTER;
	
	CComObject<CAutoEnumPoints>* penum;
	CComObject<CAutoEnumPoints>::CreateInstance(&penum);
	if (!penum)
        return E_FAIL;    
	
	penum->Init(this);
	
	return penum->QueryInterface(IID_IUnknown, (void **)ppUnk);
	}

STDMETHODIMP CAutoPoints::Add (double x, double y, double z, long Index, IIcadPoint * * Point)
	{
	CComObject<CAutoPoint>* pPoint;
	CComObject<CAutoPoint>::CreateInstance(&pPoint);
	if (!pPoint)
		return E_FAIL;
	
	pPoint->Init(m_pLib, this, x, y, z);
	
	IIcadPoint *pIIcadPoint;
	pPoint->QueryInterface( &pIIcadPoint );
	if (Index > 0 )
		{		
		if (Index > m_Contents.GetSize() )
			return E_INVALIDARG; 
			
		if (!m_Contents.Add( pIIcadPoint ))
			return E_FAIL;
		
		double newx, newy, newz, X, Y, Z;
		newx = x;
		newy = y;
		newz = z;

		//Insert the point at the given index
		CComPtr<IIcadPoint> pPoint;
		for (int i = Index; i <= m_Contents.GetSize(); i++)
			{
			pPoint = m_Contents[i - 1];
			pPoint->get_x (&X);
			pPoint->get_y (&Y);
			pPoint->get_z (&Z);

			pPoint->put_x (newx);
			pPoint->put_y (newy);
			pPoint->put_z (newz);

			newx = X;
			newy = Y;
			newz = Z;
			pPoint = NULL;
			}
		
		}
	else
		{
		if (!m_Contents.Add( pIIcadPoint ))
			return E_FAIL;
		}
	
	return pPoint->QueryInterface(Point);
	}

STDMETHODIMP CAutoPoints::Item (long Index, IIcadPoint * * ppItem)
	{
	CComPtr<IIcadPoint> pPoint;
	
	if (Index < 1 || Index > m_Contents.GetSize() )
		return E_INVALIDARG; 	

	pPoint = m_Contents[Index - 1];
	
	return pPoint->QueryInterface(ppItem);
	}

STDMETHODIMP CAutoPoints::Remove (long Index, IIcadPoint * * Point)
	{
	if (Point == NULL)
		return E_POINTER;
		
	if (Index < 1 || Index > m_Contents.GetSize() )
		return E_INVALIDARG; 	

	if (m_Contents.RemoveAt(Index - 1))
		return NOERROR;
	else
		return E_FAIL;
	}

STDMETHODIMP CAutoPoints::RemoveAll ()
	{
	int i;
	for (i = 0; i < m_Contents.GetSize(); i++)
		{
		((IIcadPoint *)m_Contents[i])->Release();
		}
	m_Contents.RemoveAll();

	return NOERROR;
	}


/////////////////////////////////////////////////////////////////////////////
// CAutoEnumPoints

HRESULT CAutoEnumPoints::Init (IIcadPoints *pPoints)
	{
	m_pPoints = pPoints;
	m_nCurrent = 0;
	return NOERROR;
	}

STDMETHODIMP CAutoEnumPoints::Next (ULONG cElements, VARIANT * pvar, ULONG * pcElementFetched)
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
	m_pPoints->get_Count(&nElements);
	
    for (l1 = m_nCurrent, l2 = 0; 
	l1 < nElements && l2 < cElements; 
	l1++, l2++)    
		{
		CComPtr<IIcadPoint> pPoint;
		m_pPoints->Item(l1+1, &pPoint);
		
		IDispatch *pDispPoint;
		hr = pPoint->QueryInterface(&pDispPoint);
		if (FAILED(hr))
            goto error;     
		
		pvar[l2].vt = VT_DISPATCH;
		pvar[l2].pdispVal = pDispPoint;
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


STDMETHODIMP CAutoEnumPoints::Skip (ULONG cElements)
	{
    m_nCurrent += cElements;
	
	long nElements = 0;
	m_pPoints->get_Count(&nElements);
	
	if (m_nCurrent > nElements)
		{        
		m_nCurrent = nElements;
        return ResultFromScode(S_FALSE);    
		}    
	else 
		return NOERROR;
	}


STDMETHODIMP CAutoEnumPoints::Reset ()
	{
	m_nCurrent = 0;
	return NOERROR;
	}

STDMETHODIMP CAutoEnumPoints::Clone (IEnumVARIANT * * ppenum)
	{
	if (ppenum == NULL)
		return E_POINTER;
	
	*ppenum = NULL;    
	CComObject<CAutoEnumPoints>* penum;
	CComObject<CAutoEnumPoints>::CreateInstance(&penum);
	if (!penum)
        return E_FAIL;    
	
	penum->Init(m_pPoints);
	penum->AddRef();    
	
	*ppenum = penum;    
	return NOERROR;
	}



