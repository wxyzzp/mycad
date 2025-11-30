/* INTELLICAD\PRJ\LIB\AUTO\AUTOTOLERANCE.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Implementation of CAutoTolerance - the Automation Tolerance object
 * 
 */ 

#include "StdAfx.h"
#include "AutoTolerance.h"
#include "AutoPoint.h"
#include "AutoVector.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoTolerance

HRESULT CAutoTolerance::Init(CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem)
	{
	CAutoDbItem::Init(pDoc, pParent);
	
	if (pDbHandItem)
		m_pDbHandItem = pDbHandItem;
	else
		{
		if (pDoc)
			m_pDbHandItem = new db_tolerance(pDoc->m_dbDrawing);
		else
			m_pDbHandItem = new db_tolerance();
		}
	
	if (NULL == m_pDbHandItem) 
		return FALSE;
	
	return NOERROR;
	}

STDMETHODIMP CAutoTolerance::get_DirectionVector (IIcadVector * * ppDirection)
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

STDMETHODIMP CAutoTolerance::put_DirectionVector (IIcadVector * ppDirection)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	sds_point pt;
	pt[0] = 1.0; pt[1] = pt[2] = 0.0;
	if (ppDirection)
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

STDMETHODIMP CAutoTolerance::get_Normal (IIcadVector * * ppExtrusionDirection)
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

STDMETHODIMP CAutoTolerance::put_Normal (IIcadVector * ExtrusionDirection)
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

STDMETHODIMP CAutoTolerance::get_TextHeight (double * pHeight)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pHeight == NULL)
		return E_POINTER;
	
	((db_tolerance *)m_pDbHandItem)->get_textht(pHeight);
	
	return NOERROR;
	}

STDMETHODIMP CAutoTolerance::put_TextHeight (double pHeight)
	{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (pHeight < 0.0)
		return E_INVALIDARG;

	((db_tolerance *)m_pDbHandItem)->set_textht(pHeight);
	
	return NOERROR;
	}

STDMETHODIMP CAutoTolerance::get_InsertionPoint (IIcadPoint * * ppInsPoint)
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

STDMETHODIMP CAutoTolerance::put_InsertionPoint (IIcadPoint * ppInsPoint)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	sds_point pt;
	pt[0] = pt[1] = pt[2] = 0.0;
	if (ppInsPoint)	
		{
		ppInsPoint->get_x(&pt[0]);
		ppInsPoint->get_y(&pt[1]);
		ppInsPoint->get_z(&pt[2]);
		}
	m_pDbHandItem->set_10(pt);
	
	return NOERROR;
	}

STDMETHODIMP CAutoTolerance::get_StyleName (BSTR * pStyleName)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pStyleName == NULL)
		return E_POINTER;
	
	char* pStr;
	m_pDbHandItem->get_3 (&pStr);
	
	CString str (pStr);
	*pStyleName = str.AllocSysString();
	return NOERROR;
	}

STDMETHODIMP CAutoTolerance::put_StyleName (BSTR pStyleName)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	CString str(pStyleName);
	if (str.IsEmpty ())
		return E_INVALIDARG;

	if ( !m_pDbHandItem->set_3 (str.GetBuffer(0), m_pDoc->m_dbDrawing)) 
		return ResultFromScode(E_FAIL);
	return NOERROR;
	}

STDMETHODIMP CAutoTolerance::get_TextString (BSTR * pText)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pText == NULL)
		return E_POINTER;
	
	char* pStr;
	m_pDbHandItem->get_1 (&pStr);
	
	CString str (pStr);
	*pText = str.AllocSysString();
	return NOERROR;
	}

STDMETHODIMP CAutoTolerance::put_TextString (BSTR pText)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	CString str(pText);
	if ( !m_pDbHandItem->set_1 (str.GetBuffer(0))) 
		return ResultFromScode(E_FAIL);
	return NOERROR;
	}



