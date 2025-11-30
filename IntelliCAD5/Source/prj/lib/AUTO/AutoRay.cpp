/* INTELLICAD\PRJ\LIB\AUTO\AUTORAY.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Implementation of CAutoRay - the Automation Ray object
 * 
 */ 

#include "StdAfx.h"
#include "AutoRay.h"
#include "AutoPoint.h"
#include "AutoVector.h"
#include "AutoHelpers.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoRay

HRESULT CAutoRay::Init(CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem)
	{
	CAutoDbItem::Init(pDoc, pParent);
	
	if (pDbHandItem)
		m_pDbHandItem = pDbHandItem;
	else
		{
		if (pDoc)
			m_pDbHandItem = new db_ray(pDoc->m_dbDrawing);
		else
			m_pDbHandItem = new db_ray();
		}
	
	if (NULL == m_pDbHandItem) 
		return FALSE;
	
	return NOERROR;
	}

STDMETHODIMP CAutoRay::get_BasePoint (IIcadPoint * * ppBasePoint)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppBasePoint == NULL)
		return E_POINTER;
	
	sds_point pt;
	m_pDbHandItem->get_10(pt);
	
	CComObject<CAutoPoint> *pPoint;
	CComObject<CAutoPoint>::CreateInstance(&pPoint);
	if (!pPoint)
		return E_FAIL;
	
	pPoint->Init(NULL, NULL, pt[0], pt[1], pt[2]);
	
	return pPoint->QueryInterface (ppBasePoint);
	}

STDMETHODIMP CAutoRay::put_BasePoint (IIcadPoint * ppBasePoint)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	sds_point pt;
	pt[0] = pt[1] = pt[2] = 0.0;
	if (ppBasePoint)	//this is an optional arg
		{
		ppBasePoint->get_x(&pt[0]);
		ppBasePoint->get_y(&pt[1]);
		ppBasePoint->get_z(&pt[2]);
		}
	m_pDbHandItem->set_10(pt);
	
	return NOERROR;
	}

STDMETHODIMP CAutoRay::get_DirectionVector (IIcadVector * * ppDirection)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppDirection == NULL)
		return E_POINTER;
	
	sds_point pt;
	m_pDbHandItem->get_11(pt);
	
	CComObject<CAutoVector> *pVector;
	CComObject<CAutoVector>::CreateInstance(&pVector);
	if (!pVector)
		return E_FAIL;
	
	pVector->Init(NULL, pt[0], pt[1], pt[2]);
	
	return pVector->QueryInterface (ppDirection);
	}

STDMETHODIMP CAutoRay::put_DirectionVector (IIcadVector * ppDirection)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	sds_point pt;
	pt[0] = 1.0; pt[1] = pt[2] = 0.0;
	if (ppDirection)	//this is an optional arg
		{
		ppDirection->get_x(&pt[0]);
		ppDirection->get_y(&pt[1]);
		ppDirection->get_z(&pt[2]);

		sds_point v;
		ic_ptcpy (v, pt);
		if (db_unitizevector(v) != 0)
			return E_INVALIDARG;
		}

	m_pDbHandItem->set_11(pt);
	
	return NOERROR;
	}

STDMETHODIMP CAutoRay::Offset (double Offset, IIcadSelectionSet * * ppArrayOfEntities)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppArrayOfEntities == NULL)
		return E_POINTER;

	if (icadRealEqual(Offset,0.0))
		return E_INVALIDARG;
	
    sds_name ename, ssetout;
	ename[0] = (long)m_pDbHandItem;
	ename[1] = (long)(m_pDoc->m_dbDrawing);

	sds_point dir, xvec, yvec, zvec, p1, p2;
	zvec[0] = zvec[1] = 0.0; zvec[2] = 1.0;
	m_pDbHandItem->get_10 (p1);
	m_pDbHandItem->get_11 (p2);
	ic_sub (p1, p2, xvec);
	m_pDbHandItem->get_210(zvec);
	ic_crossproduct (xvec, zvec, yvec);

	if (Offset < 0.0)
		ic_add (yvec, p1, dir);
	else
		ic_sub (yvec, p1, dir);
	
	HRESULT hr = m_pDoc->AUTO_cmd_offset_func(ename, fabs(Offset), 0, 0, dir, ssetout);
	if (FAILED(hr))
		return E_FAIL;

	hr = ConvertSDSssetToAutoSelectionSet (ssetout, ppArrayOfEntities, m_pDoc);
	if (FAILED(hr))
		return E_FAIL;

	m_pDoc->AUTO_ssfree (ssetout);

	return NOERROR;
	}



