/* INTELLICAD\PRJ\LIB\AUTO\AUTOPOLYLINE.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Implementation of CAutoPolyline - the Automation Polyline object
 * 
 */ 

#include "StdAfx.h"
#include "AutoPolyline.h"
#include "AutoPoint.h"
#include "AutoVector.h"
#include "AutoHelpers.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoPolyline

HRESULT CAutoPolyline::Init(CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem)
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
		}
	
	if (NULL == m_pDbHandItem) 
		return FALSE;
	
	return NOERROR;
	}

//This could be an expensive operation, so call with care.
HRESULT CAutoPolyline::UpdatePolylineData ()
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
		if (m_StartWidth)
			delete [] m_StartWidth;
		if (m_EndWidth)
			delete [] m_EndWidth;
		if (m_Bulge)
			delete [] m_Bulge;
		
		//Allocate the arrays for width and bulge
		m_StartWidth = new double [m_nVertices];
		if (!m_StartWidth)
			return E_OUTOFMEMORY;
		
		m_EndWidth = new double [m_nVertices];
		if (!m_EndWidth)
			return E_OUTOFMEMORY;
		
		m_Bulge = new double [m_nVertices];
		if (!m_Bulge)
			return E_OUTOFMEMORY;
		
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
			
			thevert->get_40(&m_StartWidth[i]);
			thevert->get_41(&m_EndWidth[i]);
			thevert->get_42(&m_Bulge[i]);
			i++;	//increment the counter only for vertices.
			}
		}
	return NOERROR;
	}


STDMETHODIMP CAutoPolyline::get_Area (double * pArea)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pArea == NULL)
		return E_POINTER;
	
	sds_name ename;
	ename[0] = (long)m_pDbHandItem;
	ename[1] = (long)m_pDoc->m_dbDrawing;
	
	if (db_edata(ename, NULL, NULL, NULL, pArea) == 0)
		return NOERROR;
	return E_FAIL;
	}

STDMETHODIMP CAutoPolyline::get_Length (double * pLength)
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

STDMETHODIMP CAutoPolyline::get_Closed (VARIANT_BOOL * pClosed)
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

STDMETHODIMP CAutoPolyline::put_Closed (VARIANT_BOOL pClosed)
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

STDMETHODIMP CAutoPolyline::get_Coordinates (IIcadPoints * * ppCoords)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppCoords == NULL)
		return E_POINTER;
	
	if (!m_Vertices)
		UpdatePolylineData();
	
	ASSERT (m_Vertices);
	return m_Vertices->QueryInterface (IID_IIcadPoints, (void **)ppCoords);
	}

STDMETHODIMP CAutoPolyline::put_Coordinates (IIcadPoints * ppCoords)
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
		hipwalk->next = thevert;

		//set the elevation of the polyline to the first vertex z
		if (i == 0)
			m_pDbHandItem->set_10(pt);
		}
	
	//create the handitem for the SEQEND
	db_seqend *thesequend;
	thesequend = new db_seqend ();
	thesequend->next = NULL;
	hipwalk->next = thesequend;
	
	UpdatePolylineData();
	
	return NOERROR;
	}

STDMETHODIMP CAutoPolyline::get_Normal (IIcadVector * * ppExtrusionDirection)
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

STDMETHODIMP CAutoPolyline::put_Normal (IIcadVector * ExtrusionDirection)
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

STDMETHODIMP CAutoPolyline::get_Thickness (double * pThick)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pThick == NULL)
		return E_POINTER;
	
	m_pDbHandItem->get_39(pThick);
	return NOERROR;
	}

STDMETHODIMP CAutoPolyline::put_Thickness (double pThick)
	{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (pThick < 0.0)
		return E_INVALIDARG;

	m_pDbHandItem->set_39(pThick);
	return NOERROR;
	}

STDMETHODIMP CAutoPolyline::get_Type (PolylineType * pType)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pType == NULL)
		return E_POINTER;
	
	int type = 0;
	if (!m_pDbHandItem->get_70(&type)) 
		return E_FAIL;
	
	if (type & IC_PLINE_CURVEFIT)
		*pType = vicFitCurvePoly;
	else if (type & IC_PLINE_SPLINEFIT)
		*pType = vicCubicSplinePoly;
	else
		*pType = vicSimplePoly;
	
	return NOERROR;
	}

STDMETHODIMP CAutoPolyline::put_Type (PolylineType pType)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	int type = 0;
	//if (!m_pDbHandItem->get_70(&type)) 
	//	return E_FAIL;
	
	if (pType == vicSimplePoly) 
		type = 0;
	else if (pType == vicFitCurvePoly)
		type = IC_PLINE_CURVEFIT;
	else if (pType == vicCubicSplinePoly)
		type = IC_PLINE_SPLINEFIT;
	else if (pType == vicQuadSplinePoly) 
		return E_INVALIDARG;	//this is not supported in Intellicad
	else
		return E_INVALIDARG;
	
	if (!m_pDbHandItem->set_70(type)) 
		return E_FAIL;
	
	return NOERROR;
	}

STDMETHODIMP CAutoPolyline::AppendVertex (IIcadPoint * Vertex)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	db_handitem *hip, *prevhip = NULL;
	double elev = 0.0;
	BOOL bElevFound = FALSE;
	sds_point pt;
	
	prevhip = m_pDbHandItem;
	hip = m_pDbHandItem->next;
	
	while ( hip != NULL)
		{
		if (hip->ret_type() == DB_SEQEND)
			break;
		prevhip = hip;
		hip = hip->next;
		//determine the elevation (z coord) from the current vertices
		if (!bElevFound)
			{
			if (prevhip->ret_type()==DB_VERTEX)
				{
				prevhip->get_10(pt);
				elev = pt[2];
				bElevFound = TRUE;
				}
			}
		}
	
	//if we didnt find the elevation (because polyline had no vertices) find it from the sysvar
	if (!bElevFound)
		{
		struct resbuf rb;
		db_getvar (NULL, DB_QELEVATION, &rb, m_pDoc->m_dbDrawing, NULL, NULL);
		elev = rb.resval.rreal;
		bElevFound = TRUE;
		}

	Vertex->get_x(&pt[0]);
	Vertex->get_y(&pt[1]);
	//Vertex->get_z(&pt[2]);  do not get the z
	pt[2] = elev;
	
	db_vertex *thevert;
	thevert = new db_vertex();
	thevert->set_10(pt);
	prevhip->next = thevert;
	thevert->next = hip;
	
	UpdatePolylineData();
	return NOERROR;
	}

STDMETHODIMP CAutoPolyline::Explode (IIcadSelectionSet * * ppArrayOfEntities)
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

STDMETHODIMP CAutoPolyline::GetBulge (int Index, double * pBulge)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pBulge == NULL)
		return E_POINTER;
	
	if (!m_Vertices)
		UpdatePolylineData();
	
	if ((Index < 1) || (Index > m_nVertices))
		return E_INVALIDARG;
	
	ASSERT (m_Bulge);
	*pBulge = m_Bulge[Index - 1];
	
	return NOERROR;
	}

STDMETHODIMP CAutoPolyline::GetWidth (int Index, double * StartWidth, double * EndWidth)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (StartWidth == NULL)
		return E_POINTER;
	
	if (EndWidth == NULL)
		return E_POINTER;
	
	if (!m_Vertices)
		UpdatePolylineData();
	
	if ((Index < 1) || (Index > m_nVertices))
		return E_INVALIDARG;
	
	ASSERT (m_StartWidth);
	*StartWidth = m_StartWidth [Index - 1];
	
	ASSERT (m_EndWidth);
	*EndWidth = m_EndWidth [Index - 1];
	
	return NOERROR;
	}

STDMETHODIMP CAutoPolyline::Offset (double Offset, IIcadSelectionSet * * ppArrayOfEntities)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppArrayOfEntities == NULL)
		return E_POINTER;

	if (icadRealEqual(Offset,0.0))
		return E_INVALIDARG;
	
	if (!m_Vertices)
		UpdatePolylineData();

    sds_name ename, ssetout;
	ename[0] = (long)m_pDbHandItem;
	ename[1] = (long)(m_pDoc->m_dbDrawing);

	sds_point dir, xvec, yvec, zvec, p1, p2;
	zvec[0] = zvec[1] = 0.0; zvec[2] = 1.0;
	p1[0] = p1[1] = p1[2] = 0.0;
	p2[0] = 1.0; p2[1] = p2[2] = 0.0;
	CComPtr<IIcadPoint> pIIcadPoint = NULL;
	m_Vertices->Item(1, &pIIcadPoint); //AutoPoints is 1 based indexing
	if (pIIcadPoint)
		{
		pIIcadPoint->get_x (&p1[0]);
		pIIcadPoint->get_y (&p1[1]);
		pIIcadPoint->get_z (&p1[2]);
		}
	pIIcadPoint = NULL;
	m_Vertices->Item(2, &pIIcadPoint); //AutoPoints is 1 based indexing
	if (pIIcadPoint)
		{
		pIIcadPoint->get_x (&p2[0]);
		pIIcadPoint->get_y (&p2[1]);
		pIIcadPoint->get_z (&p2[2]);
		}
	ic_sub (p1, p2, xvec);
	m_pDbHandItem->get_210(zvec);
	ic_crossproduct (xvec, zvec, yvec);

	if (Offset < 0.0)
		ic_add (yvec, p1, dir);
	else
		ic_sub (yvec, p1, dir);
	
	HRESULT hr = m_pDoc->AUTO_cmd_offset_func(ename, Offset, 0, 1, dir, ssetout);
	if (FAILED(hr))
		return E_FAIL;

	hr = ConvertSDSssetToAutoSelectionSet (ssetout, ppArrayOfEntities, m_pDoc);
	if (FAILED(hr))
		return E_FAIL;

	m_pDoc->AUTO_ssfree (ssetout);

	return NOERROR;
	}

STDMETHODIMP CAutoPolyline::SetBulge (int Index, double Bulge)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (!m_Vertices)
		UpdatePolylineData();
	
	if ((Index < 1) || (Index > m_nVertices))
		return E_INVALIDARG;
	
	ASSERT (m_Bulge);
	m_Bulge[Index - 1] = Bulge;
	
	db_handitem *hip = m_pDbHandItem;
	int i = 0;
	while ( hip != NULL)
		{
		if (hip->ret_type() == DB_SEQEND)
			break;
		else if (hip->ret_type()==DB_VERTEX)
			{
			i++;
			if (i == Index)
				{
				hip->set_42(Bulge);
				break;
				}
			}
		hip = hip->next;
		}
	return NOERROR;
	}

STDMETHODIMP CAutoPolyline::SetWidth (int SegmentIndex, double StartWidth, double EndWidth)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (!m_Vertices)
		UpdatePolylineData();
	
	if ((SegmentIndex < 1) || (SegmentIndex > m_nVertices))
		return E_INVALIDARG;
	
	ASSERT (m_StartWidth);
	m_StartWidth [SegmentIndex - 1] = StartWidth;
	
	ASSERT (m_EndWidth);
	m_EndWidth [SegmentIndex - 1] = EndWidth;
	
	db_handitem *hip = m_pDbHandItem;
	int i = 0;
	while ( hip != NULL)
		{
		if (hip->ret_type() == DB_SEQEND)
			break;
		else if (hip->ret_type()==DB_VERTEX)
			{
			i++;
			if (i == SegmentIndex)
				{
				hip->set_40(StartWidth);
				hip->set_41(EndWidth);
				break;
				}
			}
		hip = hip->next;
		}
	return NOERROR;
	}



