/* INTELLICAD\PRJ\LIB\AUTO\AUTOUCS.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Implementation of CAutoUCS - the Automation UCS object
 * 
 */ 

#include "StdAfx.h"
#include "AutoUCS.h"
#include "AutoPoint.h"
#include "AutoVector.h"
#include "AutoMatrix.h"
#include "ucstabrec.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoUCS

HRESULT CAutoUCS::Init(CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem)
	{
	CAutoDbItem::Init(pDoc, pParent);
	
	if (pDbHandItem)
		m_pDbHandItem = pDbHandItem;
	else
		m_pDbHandItem = new db_ucstabrec();
	
	if (NULL == m_pDbHandItem) 
		return FALSE;
	
	return NOERROR;
	}

STDMETHODIMP CAutoUCS::get_Origin (IIcadPoint * * ppOrigin)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppOrigin == NULL)
		return E_POINTER;
	
	sds_point pt;
	m_pDbHandItem->get_10(pt);
	
	CComObject<CAutoPoint> *pPoint;
	CComObject<CAutoPoint>::CreateInstance(&pPoint);
	if (!pPoint)
		return E_FAIL;
	
	pPoint->Init(NULL, NULL, pt[0], pt[1], pt[2]);
	
	return pPoint->QueryInterface (ppOrigin);
	}

STDMETHODIMP CAutoUCS::put_Origin (IIcadPoint * ppOrigin)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	sds_point pt;
	pt[0] = pt[1] = pt[2] = 0.0;
	if (ppOrigin)	//this is an optional arg
		{
		ppOrigin->get_x(&pt[0]);
		ppOrigin->get_y(&pt[1]);
		ppOrigin->get_z(&pt[2]);
		}
	m_pDbHandItem->set_10(pt);
	
	return NOERROR;
	}

STDMETHODIMP CAutoUCS::get_Xvector (IIcadVector * * ppVector)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppVector == NULL)
		return E_POINTER;
	
	sds_point v;
	m_pDbHandItem->get_11(v);
	
	CComObject<CAutoVector> *pVector;
	CComObject<CAutoVector>::CreateInstance(&pVector);
	if (!pVector)
		return E_FAIL;
	
	pVector->Init(NULL, v[0], v[1], v[2]);
	
	return pVector->QueryInterface (ppVector);
	}

STDMETHODIMP CAutoUCS::put_Xvector (IIcadVector * ppVector)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	sds_point pt;
	pt[0] = 1.0; pt[1] = pt[2] = 0.0;

	if (ppVector)
		{
		ppVector->get_x(&pt[0]);
		ppVector->get_y(&pt[1]);
		ppVector->get_z(&pt[2]);

		sds_point v;
		ic_ptcpy (v, pt);
		if (db_unitizevector(v) != 0)
			return E_INVALIDARG;
		}
	
	m_pDbHandItem->set_11(pt);
	
	return NOERROR;
	}

STDMETHODIMP CAutoUCS::get_Yvector (IIcadVector * * ppVector)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppVector == NULL)
		return E_POINTER;
	
	sds_point v;
	m_pDbHandItem->get_12(v);
	
	CComObject<CAutoVector> *pVector;
	CComObject<CAutoVector>::CreateInstance(&pVector);
	if (!pVector)
		return E_FAIL;
	
	pVector->Init(NULL, v[0], v[1], v[2]);
	
	return pVector->QueryInterface (ppVector);
	}

STDMETHODIMP CAutoUCS::put_Yvector (IIcadVector * ppVector)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	sds_point pt;
	pt[1] = 0.0; pt[0] = 1.0; pt[2] = 0.0;

	if (ppVector)
		{
		ppVector->get_x(&pt[0]);
		ppVector->get_y(&pt[1]);
		ppVector->get_z(&pt[2]);
		
		sds_point v;
		ic_ptcpy (v, pt);
		if (db_unitizevector(v) != 0)
			return E_INVALIDARG;
		}
	
	m_pDbHandItem->set_12(pt);
	
	return NOERROR;
	}

STDMETHODIMP CAutoUCS::get_Zvector (IIcadVector * * ppVector)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppVector == NULL)
		return E_POINTER;
	
	HRESULT hr;
	CComPtr<IIcadVector> xVec;
	CComPtr<IIcadVector> yVec;

	hr = get_Xvector (&xVec);
	if (FAILED(hr))
		return hr;

	hr = get_Yvector (&yVec);
	if (FAILED(hr))
		return hr;

	sds_point x, y, z;
	xVec->get_x(&x[0]);
	xVec->get_y(&x[1]);
	xVec->get_z(&x[2]);
	yVec->get_x(&y[0]);
	yVec->get_y(&y[1]);
	yVec->get_z(&y[2]);

	//calculate the cross product
	ic_crossproduct(x, y, z);
	
	CComObject<CAutoVector> *pVector;
	CComObject<CAutoVector>::CreateInstance(&pVector);
	if (!pVector)
		return E_FAIL;
	
	pVector->Init(NULL, z[0], z[1], z[2]);
	
	return pVector->QueryInterface (ppVector);
	}

STDMETHODIMP CAutoUCS::GetUCSMatrix (IIcadMatrix * * ppMatrix)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppMatrix == NULL)
		return E_POINTER;
	
	HRESULT hr;
	CComPtr<IIcadVector> xVec;
	CComPtr<IIcadVector> yVec;
	CComPtr<IIcadVector> zVec;
	CComPtr<IIcadPoint> Origin;

	hr = get_Xvector (&xVec);
	if (FAILED(hr))
		return hr;

	hr = get_Yvector (&yVec);
	if (FAILED(hr))
		return hr;

	hr = get_Zvector (&zVec);
	if (FAILED(hr))
		return hr;

	hr = get_Origin (&Origin);
	if (FAILED(hr))
		return hr;

	sds_point x, y, z, p;
	xVec->get_x(&x[0]);
	xVec->get_y(&x[1]);
	xVec->get_z(&x[2]);
	yVec->get_x(&y[0]);
	yVec->get_y(&y[1]);
	yVec->get_z(&y[2]);
	zVec->get_x(&z[0]);
	zVec->get_y(&z[1]);
	zVec->get_z(&z[2]);
	Origin->get_x(&p[0]);
	Origin->get_y(&p[1]);
	Origin->get_z(&p[2]);

	CComObject<CAutoMatrix> *pMatrix;
	CComObject<CAutoMatrix>::CreateInstance(&pMatrix);
	if (!pMatrix)
		return E_FAIL;
	
	pMatrix->Init(NULL);
	int i;
	for (i = 0; i < 3; i++)
		{
		pMatrix->SetValue (i, 0, x[i]);
		pMatrix->SetValue (i, 1, y[i]);
		pMatrix->SetValue (i, 2, z[i]);
		pMatrix->SetValue (i, 3, p[i]);
		}
	pMatrix->SetValue (3, 0, 0.0);
	pMatrix->SetValue (3, 1, 0.0);
	pMatrix->SetValue (3, 2, 0.0);
	pMatrix->SetValue (3, 3, 1.0);
	
	return pMatrix->QueryInterface (ppMatrix);
	}



