/* INTELLICAD\PRJ\LIB\AUTO\AUTOTRACE.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Implementation of CAutoTrace - the Automation Trace object
 * 
 */ 

#include "StdAfx.h"
#include "AutoTrace.h"
#include "AutoPoint.h"
#include "AutoVector.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoTrace

HRESULT CAutoTrace::Init(CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem)
	{
	CAutoDbItem::Init(pDoc, pParent);
	
	if (pDbHandItem)
		m_pDbHandItem = pDbHandItem;
	else
		{
		if (pDoc)
			m_pDbHandItem = new db_trace(pDoc->m_dbDrawing);
		else
			m_pDbHandItem = new db_trace();
		}
	
	if (NULL == m_pDbHandItem) 
		return FALSE;
	
	return NOERROR;
	}

STDMETHODIMP CAutoTrace::get_Coordinates (IIcadPoints * * ppCoords)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppCoords == NULL)
		return E_POINTER;
	
	CComObject<CAutoPoints> *pVertices;
	CComObject<CAutoPoints>::CreateInstance(&pVertices);
	if (!pVertices)
		return E_OUTOFMEMORY;

	CComPtr<IIcadPoint> pIIcadPoint = NULL;

	sds_point pt;
	if (!m_pDbHandItem->get_10(pt))
		return E_FAIL;
	pVertices->Add (pt[0], pt[1], pt[2], 0, &pIIcadPoint);
	if (pIIcadPoint == NULL)
		return E_OUTOFMEMORY;
	pIIcadPoint = NULL;

	if (!m_pDbHandItem->get_11(pt))
		return E_FAIL;
	pVertices->Add (pt[0], pt[1], pt[2], 0, &pIIcadPoint);
	if (pIIcadPoint == NULL)
		return E_OUTOFMEMORY;
	pIIcadPoint = NULL;

	if (!m_pDbHandItem->get_12(pt))
		return E_FAIL;
	pVertices->Add (pt[0], pt[1], pt[2], 0, &pIIcadPoint);
	if (pIIcadPoint == NULL)
		return E_OUTOFMEMORY;
	pIIcadPoint = NULL;

	if (!m_pDbHandItem->get_13(pt))
		return E_FAIL;
	pVertices->Add (pt[0], pt[1], pt[2], 0, &pIIcadPoint);
	if (pIIcadPoint == NULL)
		return E_OUTOFMEMORY;
	pIIcadPoint = NULL;

	return pVertices->QueryInterface (IID_IIcadPoints, (void **)ppCoords);
	}

STDMETHODIMP CAutoTrace::put_Coordinates (IIcadPoints * ppCoords)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	CComPtr<IIcadPoint> pIIcadPoint = NULL;
	HRESULT hr;

	sds_point pt;
	if (ppCoords)
		{
		pt[0] = pt[1] = pt[2] = 0.0;
		hr = ppCoords->Item(1, &pIIcadPoint);
		if (SUCCEEDED(hr) && pIIcadPoint)
			{
			pIIcadPoint->get_x (&pt[0]);
			pIIcadPoint->get_y (&pt[1]);
			pIIcadPoint->get_z (&pt[2]);
			pIIcadPoint = NULL;
			}
		if (!m_pDbHandItem->set_10(pt))
			return E_FAIL;

		pt[0] = pt[1] = pt[2] = 0.0;
		hr = ppCoords->Item(2, &pIIcadPoint);
		if (SUCCEEDED(hr) && pIIcadPoint)
			{
			pIIcadPoint->get_x (&pt[0]);
			pIIcadPoint->get_y (&pt[1]);
			pIIcadPoint->get_z (&pt[2]);
			pIIcadPoint = NULL;
			}
		if (!m_pDbHandItem->set_11(pt))
			return E_FAIL;

		pt[0] = pt[1] = pt[2] = 0.0;
		hr = ppCoords->Item(3, &pIIcadPoint);
		if (SUCCEEDED(hr) && pIIcadPoint)
			{
			pIIcadPoint->get_x (&pt[0]);
			pIIcadPoint->get_y (&pt[1]);
			pIIcadPoint->get_z (&pt[2]);
			pIIcadPoint = NULL;
			}
		if (!m_pDbHandItem->set_12(pt))
			return E_FAIL;

		pt[0] = pt[1] = pt[2] = 0.0;
		hr = ppCoords->Item(4, &pIIcadPoint);
		if (SUCCEEDED(hr) && pIIcadPoint)
			{
			pIIcadPoint->get_x (&pt[0]);
			pIIcadPoint->get_y (&pt[1]);
			pIIcadPoint->get_z (&pt[2]);
			pIIcadPoint = NULL;
			}
		if (!m_pDbHandItem->set_13(pt))
			return E_FAIL;
		}
	else
		{
		pt[0] = pt[1] = pt[2] = 0.0;
		if (!m_pDbHandItem->set_10(pt))
			return E_FAIL;
		if (!m_pDbHandItem->set_11(pt))
			return E_FAIL;
		if (!m_pDbHandItem->set_12(pt))
			return E_FAIL;
		if (!m_pDbHandItem->set_13(pt))
			return E_FAIL;
		}

	return NOERROR;
	}

STDMETHODIMP CAutoTrace::get_Normal (IIcadVector * * ppExtrusionDirection)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppExtrusionDirection == NULL)
		return E_POINTER;
	
	sds_point v;
	m_pDbHandItem->get_210(v);
	
	CComObject<CAutoVector> *pVector;
	CComObject<CAutoVector>::CreateInstance(&pVector);
	if (!pVector)
		return E_FAIL;
	
	pVector->Init(NULL, v[0], v[1], v[2]);
	
	return pVector->QueryInterface (ppExtrusionDirection);
	}

STDMETHODIMP CAutoTrace::put_Normal (IIcadVector * ExtrusionDirection)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	sds_point v;
	v[0] = v[1] = 0.0; v[2] = 1.0;
	if (ExtrusionDirection)
		{
		ExtrusionDirection->get_x(&v[0]);
		ExtrusionDirection->get_y(&v[1]);
		ExtrusionDirection->get_z(&v[2]);

		if (db_unitizevector(v) != 0)
			return E_INVALIDARG;
		}
	m_pDbHandItem->init_210 (v);
	
	return NOERROR;
	}

STDMETHODIMP CAutoTrace::get_Thickness (double * pThick)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pThick == NULL)
		return E_POINTER;
	
	m_pDbHandItem->get_39(pThick);
	return NOERROR;
	}

STDMETHODIMP CAutoTrace::put_Thickness (double pThick)
	{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (pThick < 0.0)
		return E_INVALIDARG;

	m_pDbHandItem->set_39(pThick);
	return NOERROR;
	}


