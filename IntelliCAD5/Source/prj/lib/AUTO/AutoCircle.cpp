/* INTELLICAD\PRJ\LIB\AUTO\AUTOCIRCLE.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Implementation of CAutoCircle - the Automation Circle object
 * 
 */ 

#include "StdAfx.h"
#include "AutoCircle.h"
#include "AutoPoint.h"
#include "AutoVector.h"
#include "AutoHelpers.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoCircle

HRESULT CAutoCircle::Init(CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem)
	{
	CAutoDbItem::Init(pDoc, pParent);
	
	if (pDbHandItem)
		m_pDbHandItem = pDbHandItem;
	else
		{
		if (pDoc)
			m_pDbHandItem = new db_circle(pDoc->m_dbDrawing);
		else
			m_pDbHandItem = new db_circle();
		}
	
	if (NULL == m_pDbHandItem) 
		return FALSE;
	
	return NOERROR;
	}


STDMETHODIMP CAutoCircle::get_Area (double * pArea)
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

STDMETHODIMP CAutoCircle::get_Circumference (double * pCircumference)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pCircumference == NULL)
		return E_POINTER;
		/*		
		double Radius = 0.0;
		m_pDbHandItem->get_40 (&Radius);
		*pCircumference = 2.0 * PI * Radius ;
	*/
	sds_name ename;
	ename[0] = (long)m_pDbHandItem;
	ename[1] = (long)m_pDoc->m_dbDrawing;
	
	if (db_edata(ename, NULL, NULL, pCircumference, NULL) == 0)
		return NOERROR;
	return E_FAIL;
	}

STDMETHODIMP CAutoCircle::get_Center (IIcadPoint * * ppCenter)
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

STDMETHODIMP CAutoCircle::put_Center (IIcadPoint * ppCenter)
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

STDMETHODIMP CAutoCircle::get_Normal (IIcadVector * * ppExtrusionDirection)
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

STDMETHODIMP CAutoCircle::put_Normal (IIcadVector * ExtrusionDirection)
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

STDMETHODIMP CAutoCircle::get_Radius (double * pRadius)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pRadius == NULL)
		return E_POINTER;
	
	m_pDbHandItem->get_40 (pRadius);
	return NOERROR;
	}

STDMETHODIMP CAutoCircle::put_Radius (double pRadius)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pRadius < 0.0)
		return E_INVALIDARG;

	m_pDbHandItem->set_40 (pRadius);
	return NOERROR;
	}

STDMETHODIMP CAutoCircle::get_Thickness (double * pThick)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pThick == NULL)
		return E_POINTER;
	
	m_pDbHandItem->get_39 (pThick);
	return NOERROR;
	}

STDMETHODIMP CAutoCircle::put_Thickness (double pThick)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pThick < 0.0)
		return E_INVALIDARG;

	m_pDbHandItem->set_39 (pThick);
	return NOERROR;
	}

STDMETHODIMP CAutoCircle::Offset (double Offset, IIcadSelectionSet * * ppArrayOfEntities)
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




