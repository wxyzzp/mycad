/* INTELLICAD\PRJ\LIB\AUTO\AUTOSHAPE.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * Implementation of CAutoShape - the Automation Shape object
 *
 */

#include "StdAfx.h"
#include "AutoShape.h"
#include "AutoPoint.h"
#include "AutoVector.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoShape

HRESULT CAutoShape::Init(CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem)
	{
	CAutoDbItem::Init(pDoc, pParent);

	if (pDbHandItem)
		m_pDbHandItem = pDbHandItem;
	else
		{
		if (pDoc)
			m_pDbHandItem = new db_shape(pDoc->m_dbDrawing);
		else
			m_pDbHandItem = new db_shape();
		}

	if (NULL == m_pDbHandItem)
		return FALSE;

	return NOERROR;
	}

STDMETHODIMP CAutoShape::get_Normal (IIcadVector * * ppExtrusionDirection)
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

STDMETHODIMP CAutoShape::put_Normal (IIcadVector * ExtrusionDirection)
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

STDMETHODIMP CAutoShape::get_Height (double * pHeight)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pHeight == NULL)
		return E_POINTER;

	if (!m_pDbHandItem->get_40(pHeight))
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoShape::put_Height (double pHeight)
	{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (pHeight < 0.0)
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_40(pHeight))
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoShape::get_InsertionPoint (IIcadPoint * * ppInsPoint)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppInsPoint == NULL)
		return E_POINTER;

	sds_point pt;
	m_pDbHandItem->get_10(pt);

	CComObject<CAutoPoint> *pPoint;
	CComObject<CAutoPoint>::CreateInstance(&pPoint);
	if (!pPoint)
		return E_FAIL;

	pPoint->Init(NULL, NULL, pt[0], pt[1], pt[2]);

	return pPoint->QueryInterface (ppInsPoint);
	}

STDMETHODIMP CAutoShape::put_InsertionPoint (IIcadPoint * ppInsPoint)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	sds_point pt;
	pt[0] = pt[1] = pt[2] = 0.0;
	if (ppInsPoint)	//this is an optional arg
		{
		ppInsPoint->get_x(&pt[0]);
		ppInsPoint->get_y(&pt[1]);
		ppInsPoint->get_z(&pt[2]);
		}
	m_pDbHandItem->set_10(pt);

	return NOERROR;
	}

STDMETHODIMP CAutoShape::get_ObliqueAngle (double * pAngle)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pAngle == NULL)
		return E_POINTER;

	if (!m_pDbHandItem->get_51(pAngle))
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoShape::put_ObliqueAngle (double pAngle)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (!m_pDbHandItem->set_51(pAngle))
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoShape::get_Rotation (double * pRotation)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pRotation == NULL)
		return E_POINTER;

	if (!m_pDbHandItem->get_50(pRotation))
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoShape::put_Rotation (double pRotation)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (!m_pDbHandItem->set_50(pRotation))
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoShape::get_ScaleFactor (double * pScale)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pScale == NULL)
		return E_POINTER;

	if (!m_pDbHandItem->get_41(pScale))
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoShape::put_ScaleFactor (double pScale)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pScale <= 0.0)
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_41(pScale))
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoShape::get_Name (BSTR * pText)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pText == NULL)
		return E_POINTER;

	char* pStr;
	m_pDbHandItem->get_2 (&pStr);

	CString str (pStr);
	*pText = str.AllocSysString();
	return NOERROR;
	}

STDMETHODIMP CAutoShape::put_Name (BSTR pText)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	CString str(pText);
	if ( !m_pDbHandItem->set_2 ((char*)str.GetBuffer(0), m_pDoc->m_dbDrawing))
		return ResultFromScode(E_FAIL);
	return NOERROR;
	}

STDMETHODIMP CAutoShape::get_Thickness (double * pThick)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pThick == NULL)
		return E_POINTER;

	if (!m_pDbHandItem->get_39(pThick))
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoShape::put_Thickness (double pThick)
	{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (pThick < 0.0)
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_39(pThick))
		return E_FAIL;

	return NOERROR;
	}

