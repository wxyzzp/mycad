/* INTELLICAD\PRJ\LIB\AUTO\AUTOPOLYFACEMESH.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * $Revision: 1.3 $ $Date: 2001/03/16 21:48:12 $ 
 * 
 * Abstract
 * 
 * Implementation of CAutoPolyfaceMesh - the Automation Polyline - Polyface mesh object
 * 
 */ 

#include "StdAfx.h"
#include "AutoPolyfaceMesh.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoPolyfaceMesh

HRESULT CAutoPolyfaceMesh::Init(CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem)
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

		int flags = IC_PLINE_POLYFACEMESH;
		m_pDbHandItem->set_70 (flags);
		}
	
	if (NULL == m_pDbHandItem) 
		return FALSE;
	
	return NOERROR;
	}

//This could be an expensive operation, so call with care.
HRESULT CAutoPolyfaceMesh::UpdatePolylineData ()
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
		else if ((hipwalk->ret_type()==DB_VERTEX) && (((db_polyline *)hipwalk)->ret_70()==(IC_VERTEX_FACELIST | IC_VERTEX_3DMESHVERT)))
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

HRESULT CAutoPolyfaceMesh::PutFaces (VARIANT Faces)
	{
	db_handitem *hipwalk = m_pDbHandItem;
	db_vertex *thevert;
	db_handitem *hip = NULL, *seqendhip = NULL;
	
	//walk to the end of the list
	for (hipwalk = m_pDbHandItem; hipwalk != NULL; hip = hipwalk, hipwalk = hipwalk->next) 
		{
		if (hipwalk->ret_type()==DB_SEQEND)
			break;
		}
	seqendhip = hipwalk;

	HRESULT hr;
	long LBound, UBound;
	if (!(Faces.vt & VT_ARRAY))
		return E_INVALIDARG;

	hr = SafeArrayGetLBound(Faces.parray, 1, &LBound);
	if (FAILED(hr))
		return E_INVALIDARG;
	
	hr = SafeArrayGetUBound(Faces.parray, 1, &UBound);
	if (FAILED(hr))
		return E_INVALIDARG;
	
	if ((UBound - LBound) < 3)	//we need at least 3 vertices in the array
		return E_INVALIDARG;

	long i, j;
	int vertnum;
	int nFaces = 0;
	sds_point pt;
	pt[0] = pt[1] = pt[2] = 0.0;
	for (i = LBound, j = 0; i <= UBound; i++)
		{
		SafeArrayGetElement(Faces.parray, &i, &vertnum);
		if (j == 0)
			{
			//for every 4 vertices, create a new face (actually a db_vertex)
			//and hook it up into the list
			nFaces ++;
			thevert = new db_vertex();
			thevert->set_10(pt);
			thevert->set_70(IC_VERTEX_FACELIST);
			hip->next = thevert;
			hip = hip->next;
			}
		if (j == 0)
			thevert->set_71 (vertnum);
		if (j == 1)
			thevert->set_72 (vertnum);
		if (j == 2)
			thevert->set_73 (vertnum);
		if (j == 3)
			{
			thevert->set_74 (vertnum);
			j = -1;
			}
		j++;
		}

	//hook up the seqend hip the last created face
	hip->next = seqendhip;

	m_pDbHandItem->set_72(nFaces);

	UpdatePolylineData();
	
	return NOERROR;
	}

STDMETHODIMP CAutoPolyfaceMesh::get_Coordinates (IIcadPoints * * ppCoords)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppCoords == NULL)
		return E_POINTER;
	
	if (!m_Vertices)
		UpdatePolylineData();
	
	ASSERT (m_Vertices);
	return m_Vertices->QueryInterface (IID_IIcadPoints, (void **)ppCoords);
	}

STDMETHODIMP CAutoPolyfaceMesh::put_Coordinates (IIcadPoints * ppCoords)
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
		thevert->set_70(IC_VERTEX_FACELIST | IC_VERTEX_3DMESHVERT);
		hipwalk->next = thevert;
		}
	
	//create the handitem for the SEQEND
	db_seqend *thesequend;
	thesequend = new db_seqend ();
	thesequend->next = NULL;
	hipwalk->next = thesequend;

	m_pDbHandItem->set_71(nVerts);

	UpdatePolylineData();
	
	return NOERROR;
	}

STDMETHODIMP CAutoPolyfaceMesh::get_NumberOfVertices (long * pCount)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pCount == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_71((int *)pCount)) 
		return E_FAIL;
	
	return NOERROR;
	}

STDMETHODIMP CAutoPolyfaceMesh::get_NumberOfFaces (long * pCount)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pCount == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_72((int *)pCount)) 
		return E_FAIL;
	
	return NOERROR;
	}



