/* INTELLICAD\PRJ\LIB\AUTO\AUTOARC.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Implementation of CAutoArc - the Automation Arc object
 * 
 */ 

#include "StdAfx.h"
#include "AutoArc.h"
#include "AutoPoint.h"
#include "AutoVector.h"
#include "AutoHelpers.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoArc

HRESULT CAutoArc::Init(CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem)
	{
	CAutoDbItem::Init(pDoc, pParent);
	
	if (pDbHandItem)
		m_pDbHandItem = pDbHandItem;
	else
		{
		if (pDoc)
			m_pDbHandItem = new db_arc(pDoc->m_dbDrawing);
		else
			m_pDbHandItem = new db_arc();
		}
	
	if (NULL == m_pDbHandItem) 
		return FALSE;
	
	return NOERROR;
	}

STDMETHODIMP CAutoArc::get_Area (double * pArea)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pArea == NULL)
		return E_POINTER;
		/*		
		#define PI	3.1415926535898
		double Radius = 0.0;
		m_pDbHandItem->get_40 (&Radius);
		*pArea = PI * Radius * Radius;
	*/
	sds_name ename;
	ename[0] = (long)m_pDbHandItem;
	ename[1] = (long)m_pDoc->m_dbDrawing;
	
	if (db_edata(ename, NULL, NULL, NULL, pArea) == 0)
		return NOERROR;
	return E_FAIL;
	}

STDMETHODIMP CAutoArc::get_Center (IIcadPoint * * ppCenter)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppCenter == NULL)
		return E_POINTER;
	
	sds_point pt;
	m_pDbHandItem->get_10 (pt);
	//TODO convert the point to UCS
	
	CComObject<CAutoPoint> *pPoint;
	CComObject<CAutoPoint>::CreateInstance(&pPoint);
	if (!pPoint)
		return E_FAIL;
	
	pPoint->Init(NULL, NULL, pt[0], pt[1], pt[2]);
	
	return pPoint->QueryInterface (ppCenter);
	}

STDMETHODIMP CAutoArc::put_Center (IIcadPoint * ppCenter)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	sds_point pt;
	pt[0] = pt[1] = pt[2] = 0.0;
	if (ppCenter)	//this is an optional arg
		{
		ppCenter->get_x(&pt[0]);
		ppCenter->get_y(&pt[1]);
		ppCenter->get_z(&pt[2]);
		//TODO convert the point from UCS to ECS
		}
	m_pDbHandItem->set_10 (pt);
	
	return NOERROR;
	}

STDMETHODIMP CAutoArc::get_EndAngle (double * pAngle)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pAngle == NULL)
		return E_POINTER;
	
	m_pDbHandItem->get_51 (pAngle);
	return NOERROR;
	}

STDMETHODIMP CAutoArc::put_EndAngle (double pAngle)
	{
	RETURN_IF_INVALID (m_pDbHandItem);

	double startAngle, endAngle;
	m_pDbHandItem->get_50 (&startAngle);
	endAngle = pAngle;

	if (startAngle == endAngle)
		endAngle += IC_TWOPI;

	ic_normang (&startAngle, &endAngle);
	
	m_pDbHandItem->set_50 (startAngle);
	m_pDbHandItem->set_51 (endAngle);
	return NOERROR;
	}

STDMETHODIMP CAutoArc::get_EndPoint (IIcadPoint * * ppEndPoint)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppEndPoint == NULL)
		return E_POINTER;
	
	sds_point center;
	double endAngle, radius;
	m_pDbHandItem->get_10 (center);
	m_pDbHandItem->get_51 (&endAngle);
	m_pDbHandItem->get_40 (&radius);
	double x, y, z;
	x = center[0] + radius * cos(endAngle);
	y = center[1] + radius * sin(endAngle);
	z = 0.0;
	
	CComObject<CAutoPoint> *pPoint;
	CComObject<CAutoPoint>::CreateInstance(&pPoint);
	if (!pPoint)
		return E_FAIL;
	
	pPoint->Init(NULL, NULL, x, y, z);
	
	return pPoint->QueryInterface (ppEndPoint);
	}

STDMETHODIMP CAutoArc::get_Normal (IIcadVector * * ppExtrusionDirection)
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

STDMETHODIMP CAutoArc::put_Normal (IIcadVector * ExtrusionDirection)
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

STDMETHODIMP CAutoArc::get_Length (double * pLength)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pLength == NULL)
		return E_POINTER;
	
	sds_name ename;
	ename[0] = (long)m_pDbHandItem;
	ename[1] = (long)m_pDoc->m_dbDrawing;
	
	if (db_edata(ename, NULL, NULL, pLength, NULL) == 0)
		return NOERROR;
	return E_FAIL;
	}

STDMETHODIMP CAutoArc::get_Radius (double * pRadius)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pRadius == NULL)
		return E_POINTER;
	
	m_pDbHandItem->get_40 (pRadius);
	return NOERROR;
	}

STDMETHODIMP CAutoArc::put_Radius (double pRadius)
	{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (pRadius < 0.0)
		return E_INVALIDARG;

	m_pDbHandItem->set_40 (pRadius);
	return NOERROR;
	}

STDMETHODIMP CAutoArc::get_StartAngle (double * pAngle)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pAngle == NULL)
		return E_POINTER;
	
	m_pDbHandItem->get_50 (pAngle);
	return NOERROR;
	}

STDMETHODIMP CAutoArc::put_StartAngle (double pAngle)
	{
	RETURN_IF_INVALID (m_pDbHandItem);

	double startAngle, endAngle;
	m_pDbHandItem->get_51 (&endAngle);
	startAngle = pAngle;

	if (startAngle == endAngle)
		endAngle += IC_TWOPI;

	ic_normang (&startAngle, &endAngle);
	
	m_pDbHandItem->set_50 (startAngle);
	m_pDbHandItem->set_51 (endAngle);
	return NOERROR;
	}

STDMETHODIMP CAutoArc::get_StartPoint (IIcadPoint * * ppStartPoint)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppStartPoint == NULL)
		return E_POINTER;
	
	sds_point center;
	double startAngle, radius;
	m_pDbHandItem->get_10 (center);
	m_pDbHandItem->get_50 (&startAngle);
	m_pDbHandItem->get_40 (&radius);
	double x, y, z;
	x = center[0] + radius * cos(startAngle);
	y = center[1] + radius * sin(startAngle);
	z = 0.0;
	
	CComObject<CAutoPoint> *pPoint;
	CComObject<CAutoPoint>::CreateInstance(&pPoint);
	if (!pPoint)
		return E_FAIL;
	
	pPoint->Init(NULL, NULL, x, y, z);
	
	return pPoint->QueryInterface (ppStartPoint);
	}

STDMETHODIMP CAutoArc::get_Thickness (double * pThick)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pThick == NULL)
		return E_POINTER;
	
	m_pDbHandItem->get_39 (pThick);
	return NOERROR;
	}

STDMETHODIMP CAutoArc::put_Thickness (double pThick)
	{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (pThick < 0.0)
		return E_INVALIDARG;

	m_pDbHandItem->set_39 (pThick);
	return NOERROR;
	}

STDMETHODIMP CAutoArc::Offset (double Offset, IIcadSelectionSet * * ppArrayOfEntities)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppArrayOfEntities == NULL)
		return E_POINTER;

	if (icadRealEqual(Offset,0.0))
		return E_INVALIDARG;
	
    sds_name ename, ssetout;
	ename[0] = (long)m_pDbHandItem;
	ename[1] = (long)(m_pDoc->m_dbDrawing);

	sds_point dir;
	m_pDbHandItem->get_10 (dir);
	if (Offset > 0.0)
		{
		double Radius;
		m_pDbHandItem->get_40 (&Radius);
		dir[0] += Radius * 1.1;
		}
	
	HRESULT hr = m_pDoc->AUTO_cmd_offset_func(ename, fabs(Offset), 0, 0, dir, ssetout);
	if (FAILED(hr))
		return E_FAIL;

	hr = ConvertSDSssetToAutoSelectionSet (ssetout, ppArrayOfEntities, m_pDoc);
	if (FAILED(hr))
		return E_FAIL;

	m_pDoc->AUTO_ssfree (ssetout);

	return NOERROR;
	}



