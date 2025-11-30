/* INTELLICAD\PRJ\LIB\AUTO\AUTOLINE.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Implementation of CAutoLine - the Automation Line object
 * 
 */ 

#include "StdAfx.h"
#include "AutoLine.h"
#include "AutoPoint.h"
#include "AutoVector.h"
#include "AutoHelpers.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoLine

HRESULT CAutoLine::Init(CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem)
	{
	CAutoDbItem::Init(pDoc, pParent);
	
	if (pDbHandItem)
		m_pDbHandItem = pDbHandItem;
	else
		{
		if (pDoc)
			m_pDbHandItem = new db_line(pDoc->m_dbDrawing);
		else
			m_pDbHandItem = new db_line();
		}
	
	if (NULL == m_pDbHandItem) 
		return FALSE;
	
	return NOERROR;
	}

STDMETHODIMP CAutoLine::get_EndPoint (IIcadPoint * * ppPoint)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppPoint == NULL)
		return E_POINTER;
	
	sds_point pt;
	m_pDbHandItem->get_11(pt);
	
	CComObject<CAutoPoint> *pPoint;
	CComObject<CAutoPoint>::CreateInstance(&pPoint);
	if (!pPoint)
		return E_FAIL;
	
	pPoint->Init(NULL, NULL, pt[0], pt[1], pt[2]);
	
	return pPoint->QueryInterface (ppPoint);
	}

STDMETHODIMP CAutoLine::put_EndPoint (IIcadPoint * pPoint)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	sds_point pt;
	pt[0] = pt[1] = pt[2] = 0.0;
	if (pPoint)	//this is an optional arg
		{
		pPoint->get_x(&pt[0]);
		pPoint->get_y(&pt[1]);
		pPoint->get_z(&pt[2]);
		}
	m_pDbHandItem->set_11(pt);
	
	return NOERROR;
	}

STDMETHODIMP CAutoLine::get_Normal (IIcadVector * * ppExtrusionDirection)
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

STDMETHODIMP CAutoLine::put_Normal (IIcadVector * ExtrusionDirection)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	sds_point v;
	v[0] = v[1] = v[2] = 1.0;
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

STDMETHODIMP CAutoLine::get_Length (double * pLength)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pLength == NULL)
		return E_POINTER;
	
	sds_point pt1;
	m_pDbHandItem->get_10(pt1);
	
	sds_point pt2;
	m_pDbHandItem->get_11(pt2);
	
	double dx = pt2[0] - pt1[0];
	double dy = pt2[1] - pt1[1];
	double dz = pt2[2] - pt1[2];
	
	*pLength = sqrt (dx*dx + dy*dy + dz*dz);
	
	return NOERROR;
	}

STDMETHODIMP CAutoLine::get_StartPoint (IIcadPoint * * ppPoint)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppPoint == NULL)
		return E_POINTER;
	
	sds_point pt;
	m_pDbHandItem->get_10(pt);
	
	CComObject<CAutoPoint> *pPoint;
	CComObject<CAutoPoint>::CreateInstance(&pPoint);
	if (!pPoint)
		return E_FAIL;
	
	pPoint->Init(NULL, NULL, pt[0], pt[1], pt[2]);
	
	return pPoint->QueryInterface (ppPoint);
	}

STDMETHODIMP CAutoLine::put_StartPoint (IIcadPoint * pPoint)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	sds_point pt;
	pt[0] = pt[1] = pt[2] = 0.0;
	if (pPoint)	//this is an optional arg
		{
		pPoint->get_x(&pt[0]);
		pPoint->get_y(&pt[1]);
		pPoint->get_z(&pt[2]);
		}
	m_pDbHandItem->set_10(pt);
	
	return NOERROR;
	}

STDMETHODIMP CAutoLine::get_Thickness (double * pThick)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pThick == NULL)
		return E_POINTER;
	
	m_pDbHandItem->get_39(pThick);
	return NOERROR;
	}

STDMETHODIMP CAutoLine::put_Thickness (double pThick)
	{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (pThick < 0.0)
		return E_INVALIDARG;

	m_pDbHandItem->set_39(pThick);
	return NOERROR;
	}

STDMETHODIMP CAutoLine::Offset (double Offset, IIcadSelectionSet * * ppArrayOfEntities)
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



