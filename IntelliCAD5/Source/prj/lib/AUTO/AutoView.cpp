/* INTELLICAD\PRJ\LIB\AUTO\AUTOVIEW.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Implementation of CAutoView - the Automation View object
 * 
 */ 

#include "StdAfx.h"
#include "AutoView.h"
#include "AutoPoint.h"
#include "AutoVector.h"
#include "viewtabrec.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoView

HRESULT CAutoView::Init(CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem)
	{
	CAutoDbItem::Init(pDoc, pParent);
	
	if (pDbHandItem)
		m_pDbHandItem = pDbHandItem;
	else
		m_pDbHandItem = new db_viewtabrec();
	
	if (NULL == m_pDbHandItem) 
		return FALSE;
	
	return NOERROR;
	}

STDMETHODIMP CAutoView::get_Center (IIcadPoint * * ppCenter)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppCenter == NULL)
		return E_POINTER;
	
	sds_point pt;
	m_pDbHandItem->get_10(pt);
	
	CComObject<CAutoPoint> *pPoint;
	CComObject<CAutoPoint>::CreateInstance(&pPoint);
	if (!pPoint)
		return E_FAIL;
	
	pPoint->Init(NULL, NULL, pt[0], pt[1], pt[2]);
	
	return pPoint->QueryInterface (ppCenter);
	}

STDMETHODIMP CAutoView::put_Center (IIcadPoint * ppCenter)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	sds_point pt;
	pt[0] = pt[1] = pt[2] = 0.0;
	if (ppCenter)	//this is an optional arg
		{
		ppCenter->get_x(&pt[0]);
		ppCenter->get_y(&pt[1]);
		ppCenter->get_z(&pt[2]);
		}
	m_pDbHandItem->set_10(pt);
	
	return NOERROR;
	}

STDMETHODIMP CAutoView::get_Direction (IIcadVector * * ppNormal)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppNormal == NULL)
		return E_POINTER;
	
	sds_point v;
	m_pDbHandItem->get_11(v);
	
	CComObject<CAutoVector> *pVector;
	CComObject<CAutoVector>::CreateInstance(&pVector);
	if (!pVector)
		return E_FAIL;
	
	pVector->Init(NULL, v[0], v[1], v[2]);
	
	return pVector->QueryInterface (ppNormal);
	}

STDMETHODIMP CAutoView::put_Direction (IIcadVector * ppNormal)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	sds_point pt;
	pt[0] = pt[1] = 0.0; pt[2] = 1.0;

	if (ppNormal)
		{
		ppNormal->get_x(&pt[0]);
		ppNormal->get_y(&pt[1]);
		ppNormal->get_z(&pt[2]);

		sds_point v;
		ic_ptcpy (v, pt);
		if (db_unitizevector(v) != 0)
			return E_INVALIDARG;
		}
	
	m_pDbHandItem->set_11(pt);
	
	return NOERROR;
	}

STDMETHODIMP CAutoView::get_Height (double * pHeight)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pHeight == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_40(pHeight)) 
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoView::put_Height (double pHeight)
	{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (pHeight <= 0.0)
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_40(pHeight)) 
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoView::get_LensLength (double * pLensLength)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pLensLength == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_42(pLensLength)) 
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoView::get_Target (IIcadPoint * * ppTarget)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppTarget == NULL)
		return E_POINTER;
	
	sds_point pt;
	m_pDbHandItem->get_12(pt);
	
	CComObject<CAutoPoint> *pPoint;
	CComObject<CAutoPoint>::CreateInstance(&pPoint);
	if (!pPoint)
		return E_FAIL;
	
	pPoint->Init(NULL, NULL, pt[0], pt[1], pt[2]);
	
	return pPoint->QueryInterface (ppTarget);
	}

STDMETHODIMP CAutoView::put_Target (IIcadPoint * ppTarget)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	sds_point pt;
	pt[0] = pt[1] = 0.0; pt[2] = 1.0;
	if (ppTarget)
		{
		ppTarget->get_x(&pt[0]);
		ppTarget->get_y(&pt[1]);
		ppTarget->get_z(&pt[2]);
		}
	m_pDbHandItem->set_12(pt);
	
	return NOERROR;
	}

STDMETHODIMP CAutoView::get_Width (double * pWidth)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pWidth == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_41(pWidth)) 
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoView::put_Width (double pWidth)
	{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (pWidth <= 0.0)
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_41(pWidth)) 
		return E_FAIL;

	return NOERROR;
	}



