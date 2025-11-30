/* INTELLICAD\PRJ\LIB\AUTO\AUTOPOINTENT.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Implementation of CAutoPointEnt - the Automation Point object
 * This is an actual database entity, not to be confused with Point which is
 * a helper object in the Library.
 * 
 */ 

#include "StdAfx.h"
#include "AutoPointEnt.h"
#include "AutoPoint.h"
#include "AutoVector.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoPointEnt

HRESULT CAutoPointEnt::Init(CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem)
	{
	CAutoDbItem::Init(pDoc, pParent);
	
	if (pDbHandItem)
		m_pDbHandItem = pDbHandItem;
	else
		{
		if (pDoc)
			m_pDbHandItem = new db_point(pDoc->m_dbDrawing);
		else
			m_pDbHandItem = new db_point();
		}
	
	if (NULL == m_pDbHandItem) 
		return FALSE;
	
	return NOERROR;
	}

STDMETHODIMP CAutoPointEnt::get_Coordinates (IIcadPoints * * ppCoord)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppCoord == NULL)
		return E_POINTER;
	
	sds_point pt;
	m_pDbHandItem->get_10(pt);
	
	CComPtr<IIcadPoints> Vertices;
	CComPtr<IIcadPoint> Vertex = NULL;

	CComObject<CAutoPoints> *pVertices;
	CComObject<CAutoPoints>::CreateInstance(&pVertices);
	
	pVertices->QueryInterface(&Vertices);
	if (!Vertices)
		return E_OUTOFMEMORY;

	Vertices->Add (pt[0], pt[1], pt[2], 0, &Vertex);

	if (Vertex == NULL)
		return E_OUTOFMEMORY;

	return Vertices->QueryInterface (ppCoord);	
	}

STDMETHODIMP CAutoPointEnt::put_Coordinates (IIcadPoints * ppCoords)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppCoords == NULL)
		return E_INVALIDARG;

	long nVerts;
	ppCoords->get_Count (&nVerts);
	if (nVerts != 1)
		return E_INVALIDARG;
	
	CComPtr<IIcadPoint> Vertex;
	sds_point pt;

	ppCoords->Item(1, &Vertex); //AutoPoints is 1 based indexing

	if (Vertex == NULL)
		return E_FAIL;
		
	Vertex->get_x(&pt[0]);
	Vertex->get_y(&pt[1]);
	Vertex->get_z(&pt[2]);

	if (!m_pDbHandItem->set_10 (pt))
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoPointEnt::get_Normal (IIcadVector * * ppExtrusionDirection)
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

STDMETHODIMP CAutoPointEnt::put_Normal (IIcadVector * ExtrusionDirection)
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

STDMETHODIMP CAutoPointEnt::get_Thickness (double * pThick)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pThick == NULL)
		return E_POINTER;
	
	m_pDbHandItem->get_39(pThick);
	return NOERROR;
	}

STDMETHODIMP CAutoPointEnt::put_Thickness (double pThick)
	{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (pThick < 0.0)
		return E_INVALIDARG;

	m_pDbHandItem->set_39(pThick);
	return NOERROR;
	}


