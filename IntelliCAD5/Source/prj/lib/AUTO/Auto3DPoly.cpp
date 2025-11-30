/* INTELLICAD\PRJ\LIB\AUTO\AUTO3DPOLY.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * $Revision: 1.4 $ $Date: 2001/07/05 12:25:21 $ 
 * 
 * Abstract
 * 
 * Implementation of CAuto3DPolyline - the Automation 3D Polyline object
 * 
 */ 

#include "StdAfx.h"
#include "Auto3DPoly.h"
#include "AutoPoint.h"
#include "AutoVector.h"
#include "AutoHelpers.h"

/////////////////////////////////////////////////////////////////////////////
// CAuto3DPolyline

HRESULT CAuto3DPolyline::Init(CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem)
	{
	CAutoDbItem::Init(pDoc, pParent);
	
	if (pDbHandItem)
		m_pDbHandItem = pDbHandItem;
	else
		{
		if (pDoc)
			m_pDbHandItem = new db_polyline(pDoc->m_dbDrawing);
		else
			m_pDbHandItem = new db_polyline();

		int flags = IC_PLINE_3DPLINE;
		m_pDbHandItem->set_70 (flags);
		}
	
	if (NULL == m_pDbHandItem) 
		return FALSE;
	
	return NOERROR;
	}

//This could be an expensive operation, so call with care.
HRESULT CAuto3DPolyline::UpdatePolylineData ()
	{
	int nVerts = 0;
	BOOL bAdd = FALSE;
	db_handitem *hipwalk;
	
	//Count the number of vertices
	for (hipwalk = m_pDbHandItem; hipwalk != NULL; hipwalk = hipwalk->next) 
		{
		if (hipwalk->ret_type()==DB_SEQEND)
			break;
		else if (hipwalk->ret_type()==DB_VERTEX)
			nVerts++;
		}
	
	//if the number of vertices have changed, realloc the arrays
	if (nVerts != m_nVertices)	
		{
		m_nVertices = nVerts;
		if (m_Vertices)
			m_Vertices = NULL;
		
		CComObject<CAutoPoints> *pVertices;
		CComObject<CAutoPoints>::CreateInstance(&pVertices);
		pVertices->QueryInterface(&m_Vertices);
		if (!m_Vertices)
			return E_OUTOFMEMORY;
		
		bAdd = TRUE; //setting this will cause the following code to add CAutoPoint objects to the m_Vertices collection
		}
	
	//Fill in the arrays
	int i = 0;
	sds_point pt;
	db_vertex *thevert;
	CComPtr<IIcadPoint> pIIcadPoint = NULL;
	for (hipwalk = m_pDbHandItem; hipwalk != NULL; hipwalk = hipwalk->next) 
		{
		if (hipwalk->ret_type()==DB_SEQEND)
			break;
		else if (hipwalk->ret_type()==DB_VERTEX)
			{
			thevert = (db_vertex *)hipwalk;
			thevert->get_10(pt);
			
			if (bAdd)
				m_Vertices->Add (pt[0], pt[1], pt[2], 0, &pIIcadPoint);
			else
				m_Vertices->Item(i+1, &pIIcadPoint); //AutoPoints is 1 based indexing
			
			if (pIIcadPoint == NULL)
				return E_OUTOFMEMORY;
			
			pIIcadPoint->put_x (pt[0]);
			pIIcadPoint->put_y (pt[1]);
			pIIcadPoint->put_z (pt[2]);
			pIIcadPoint = NULL;
			
			i++;	//increment the counter only for vertices.
			}
		}
	return NOERROR;
	}

STDMETHODIMP CAuto3DPolyline::get_Closed (VARIANT_BOOL * pClosed)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pClosed == NULL)
		return E_POINTER;
	
	BOOL bclose = 0;
	if (!m_pDbHandItem->get_70(&bclose)) 
		return E_FAIL;
	
	if (bclose & IC_PLINE_CLOSED) 
		*pClosed = VARIANT_TRUE;
	else 
		*pClosed = VARIANT_FALSE;
	
	return NOERROR;
	}

STDMETHODIMP CAuto3DPolyline::put_Closed (VARIANT_BOOL pClosed)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	int flags = 0;
	
	if (!m_pDbHandItem->get_70(&flags)) 
		return E_FAIL;

	if (pClosed == VARIANT_TRUE) 
		flags |= IC_PLINE_CLOSED;
	else
		flags &= ~IC_PLINE_CLOSED;
	
	if (!m_pDbHandItem->set_70(flags)) 
		return E_FAIL;
	
	return NOERROR;
	}

STDMETHODIMP CAuto3DPolyline::get_Coordinates (IIcadPoints * * ppCoords)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppCoords == NULL)
		return E_POINTER;
	
	if (!m_Vertices)
		UpdatePolylineData();
	
	ASSERT (m_Vertices);
	return m_Vertices->QueryInterface (IID_IIcadPoints, (void **)ppCoords);
	}

STDMETHODIMP CAuto3DPolyline::put_Coordinates (IIcadPoints * ppCoords)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	db_handitem *hipwalk = m_pDbHandItem;
	sds_point pt;
	db_vertex *thevert;
	db_handitem *prevhip = NULL;
	
	//Delete the existing vertices if any
	for (hipwalk = m_pDbHandItem; hipwalk != NULL; prevhip = hipwalk, hipwalk = hipwalk->next) 
		{
		if (hipwalk->ret_type()==DB_SEQEND)
			break;
		else if (hipwalk->ret_type()==DB_VERTEX)
			m_pDoc->m_dbDrawing->delhanditem (prevhip, hipwalk, 0);
		}
	
	//Create the list of handitems for the vertices
	int i;
	long nVerts = 1;
	
	if (ppCoords)
		ppCoords->get_Count (&nVerts);
	else
		nVerts = 1;
	
	for (hipwalk = m_pDbHandItem, i = 0; i < nVerts; hipwalk = hipwalk->next, i++)
		{
		if (ppCoords)
			{
			CComPtr<IIcadPoint> pPoint;
			ppCoords->Item(i+1, &pPoint); //AutoPoints is 1 based indexing
			pPoint->get_x(&pt[0]);
			pPoint->get_y(&pt[1]);
			pPoint->get_z(&pt[2]);
			}
		else
			{
			//if no coords passed in (optional arg) create a dummy one
			pt[0] = pt[1] = pt[2] = 0.0;
			}
		
		thevert = new db_vertex();
		thevert->set_10(pt);
		thevert->set_70(IC_VERTEX_3DPLINE);
		hipwalk->next = thevert;
		}
	
	//create the handitem for the SEQEND
	db_seqend *thesequend;
	thesequend = new db_seqend ();
	thesequend->next = NULL;
	hipwalk->next = thesequend;
	
	UpdatePolylineData();
	
	return NOERROR;
	}

STDMETHODIMP CAuto3DPolyline::get_Length (double * pLength)
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

STDMETHODIMP CAuto3DPolyline::AppendVertex (IIcadPoint * Vertex)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	db_handitem *hip, *prevhip = NULL;
	
	prevhip = m_pDbHandItem;
	hip = m_pDbHandItem->next;
	
	while ( hip != NULL)
		{
		if (hip->ret_type() == DB_SEQEND)
			break;
		prevhip = hip;
		hip = hip->next;
		}
	
	sds_point pt;
	Vertex->get_x(&pt[0]);
	Vertex->get_y(&pt[1]);
	Vertex->get_z(&pt[2]);
	
	db_vertex *thevert;
	thevert = new db_vertex();
	thevert->set_10(pt);
	thevert->set_70(IC_VERTEX_3DPLINE);
	prevhip->next = thevert;
	thevert->next = hip;
	
	UpdatePolylineData();
	return NOERROR;
	}

STDMETHODIMP CAuto3DPolyline::Explode (IIcadSelectionSet * * ppArrayOfEntities)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppArrayOfEntities == NULL)
		return E_POINTER;
	
	//construct the input selection set
	sds_name ssetout, ssetin;
    sds_name ename;
	m_pDoc->AUTO_ssadd (NULL, NULL, ssetin);
	ename[0] = (long)m_pDbHandItem;
	ename[1] = (long)(m_pDoc->m_dbDrawing);
	m_pDoc->AUTO_ssadd (ename, ssetin, ssetin);
	
	HRESULT hr = m_pDoc->AUTO_cmd_explode_objects (ssetin, ssetout);
	if (FAILED(hr))
		return E_FAIL;

	hr = ConvertSDSssetToAutoSelectionSet (ssetout, ppArrayOfEntities, m_pDoc);
	if (FAILED(hr))
		return E_FAIL;

	m_pDoc->AUTO_ssfree (ssetout);

	return NOERROR;
	}


