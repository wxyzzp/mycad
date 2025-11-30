/* INTELLICAD\PRJ\LIB\AUTO\AUTOLWPOLYLINE.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * $Revision: 1.5.8.1 $ $Date: 2003/03/09 00:06:32 $ 
 * 
 * Abstract
 * 
 * Implementation of CAutoLWPolyline - the Automation Lightweight polyline object
 * 
 */ 

#include "StdAfx.h"
#include "AutoPoint.h"
#include "AutoVector.h"
#include "AutoLWPolyline.h"
#include "AutoHelpers.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoLWPolyline


HRESULT CAutoLWPolyline::Init(CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem)
	{
	CAutoDbItem::Init(pDoc, pParent);
	
	if (pDbHandItem)
		m_pDbHandItem = pDbHandItem;
	else
		{
		if (pDoc)
			m_pDbHandItem = new db_lwpolyline(pDoc->m_dbDrawing);
		else
			m_pDbHandItem = new db_lwpolyline();
		}
	
	if (NULL == m_pDbHandItem) 
		return FALSE;
	
	return NOERROR;
	}

STDMETHODIMP CAutoLWPolyline::get_Area (double * pArea)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pArea == NULL)
		return E_POINTER;
	
	sds_name ename;
	ename[0] = (long)m_pDbHandItem;
	ename[1] = (long)m_pDoc->m_dbDrawing;

	int lwplflag = 0;
	if (!m_pDbHandItem->get_70(&lwplflag)) 
		return E_FAIL;

	sds_real lastlwplbulge;
	int lwplnumverts;

	if (!(lwplflag & IC_PLINE_CLOSED))
	{
		m_pDbHandItem->get_90(&lwplnumverts);
		
		m_pDbHandItem->set_70(lwplflag | IC_PLINE_CLOSED);
		m_pDbHandItem->get_42(&lastlwplbulge,lwplnumverts-1);
		m_pDbHandItem->set_42(0.0,lwplnumverts-1);		
	}

	int rval = db_edata(ename, NULL, NULL, NULL, pArea);
	
	if (!(lwplflag & IC_PLINE_CLOSED))
	{
		m_pDbHandItem->set_70(lwplflag);
		m_pDbHandItem->set_42(lastlwplbulge,lwplnumverts-1);
	}
	
	if (rval == 0)
		return NOERROR;
	else
		return E_FAIL;
	}

STDMETHODIMP CAutoLWPolyline::get_Length (double * pLength)
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

STDMETHODIMP CAutoLWPolyline::get_Closed (VARIANT_BOOL * pClosed)
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

STDMETHODIMP CAutoLWPolyline::put_Closed (VARIANT_BOOL pClosed)
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

STDMETHODIMP CAutoLWPolyline::get_Coordinates (IIcadPoints * * ppCoords)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppCoords == NULL)
		return E_POINTER;
	

	int nVerts, i;
	sds_point pt;

	m_pDbHandItem->get_90(&nVerts);

	CComPtr<IIcadPoints> Vertices;
	CComPtr<IIcadPoint> Vertex = NULL;

	CComObject<CAutoPoints> *pVertices;
	CComObject<CAutoPoints>::CreateInstance(&pVertices);
	
	pVertices->QueryInterface(&Vertices);
	if (!Vertices)
		return E_OUTOFMEMORY;

	for (i = 0; i < nVerts; i++)
		{
		m_pDbHandItem->get_10 (pt, i);

		Vertices->Add (pt[0], pt[1], pt[2], 0, &Vertex);

		if (Vertex == NULL)
			return E_OUTOFMEMORY;
		
		Vertex = NULL;
		}

	return Vertices->QueryInterface (ppCoords);
	}

STDMETHODIMP CAutoLWPolyline::put_Coordinates (IIcadPoints * ppCoords)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppCoords == NULL)
		return E_INVALIDARG;
	
	long i, nVerts;
	ppCoords->get_Count (&nVerts);
	if (nVerts < 2)
		return E_INVALIDARG;
	
	CComPtr<IIcadPoint> Vertex;
	sds_point *points = new sds_point [nVerts];

	for (i = 0; i < nVerts; i++)
		{
		ppCoords->Item(i+1, &Vertex); //AutoPoints is 1 based indexing

		if (Vertex == NULL)
			return E_FAIL;
		
		Vertex->get_x(&points[i][0]);
		Vertex->get_y(&points[i][1]);
		Vertex->get_z(&points[i][2]);
		Vertex = NULL;
		}

	if (!m_pDbHandItem->set_10 (points, nVerts))
		return E_FAIL;

	delete [] points;
	return NOERROR;
	}

STDMETHODIMP CAutoLWPolyline::get_Normal (IIcadVector * * ppExtrusionDirection)
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

STDMETHODIMP CAutoLWPolyline::put_Normal (IIcadVector * ExtrusionDirection)
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

STDMETHODIMP CAutoLWPolyline::get_Thickness (double * pThick)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pThick == NULL)
		return E_POINTER;
	
	m_pDbHandItem->get_39 (pThick);
	return NOERROR;
	}

STDMETHODIMP CAutoLWPolyline::put_Thickness (double pThick)
	{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (pThick < 0.0)
		return E_INVALIDARG;

	m_pDbHandItem->set_39 (pThick);
	return NOERROR;
	}

STDMETHODIMP CAutoLWPolyline::AddVertex (int Index, IIcadPoint * Point)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (!Point)
		return E_INVALIDARG;

	CComPtr<IIcadPoints> Vertices;
	CComPtr<IIcadPoint> Vertex;

	//get the existing vertices of the polyline
	if (FAILED(get_Coordinates (&Vertices)))
		return E_FAIL;

	long i, j, nVerts;
	Vertices->get_Count (&nVerts);
	if (nVerts < 2)
		return E_INVALIDARG;
	
	//allocate a new array to hold the new vertices
	sds_point *points = new sds_point [nVerts+1];
	sds_real *startwid = new sds_real [nVerts];
	sds_real *endwid = new sds_real [nVerts];
	sds_real *bulge = new sds_real [nVerts];
	sds_real *new_startwid = new sds_real [nVerts + 1];
	sds_real *new_endwid = new sds_real [nVerts + 1];
	sds_real *new_bulge = new sds_real [nVerts + 1];

	m_pDbHandItem->get_40array (startwid, nVerts);
	m_pDbHandItem->get_41array (endwid, nVerts);
	m_pDbHandItem->get_42array (bulge, nVerts);

	//fill in the new vertices
	for (i = 0, j = 0; i < nVerts + 1; i++)
		{
		if (i == Index - 1)	//when we reach the index to Add, get the vertex from the input arg Point.
			{
			Point->get_x(&points[i][0]);
			Point->get_y(&points[i][1]);
			Point->get_z(&points[i][2]);

			new_startwid[i] = 0.0;
			new_endwid[i] = 0.0;
			new_bulge[i] = 0.0;
			}
		else	//otherwise copy the vertex from the original vertices.
			{
			Vertices->Item(j+1, &Vertex); //AutoPoints is 1 based indexing
			
			if (Vertex == NULL)
				return E_FAIL;

			Vertex->get_x(&points[i][0]);
			Vertex->get_y(&points[i][1]);
			Vertex->get_z(&points[i][2]);

			new_startwid[i] = startwid[j];
			new_endwid[i] = endwid[j];
			new_bulge[i] = bulge[j];

			Vertex = NULL;
			j++;
			}
		}

	//now set the new arrays to the lwpolyline
	nVerts = nVerts+1;
	if (!m_pDbHandItem->set_10 (points, nVerts))
		return E_FAIL;
	m_pDbHandItem->set_40 (new_startwid, nVerts);
	m_pDbHandItem->set_41 (new_endwid, nVerts);
	m_pDbHandItem->set_42 (new_bulge, nVerts);

	//cleanup
	delete [] points;
	delete [] startwid;
	delete [] endwid;
	delete [] bulge;
	delete [] new_startwid;
	delete [] new_endwid;
	delete [] new_bulge;

	return NOERROR;
	}

STDMETHODIMP CAutoLWPolyline::Explode (IIcadSelectionSet * * ppArrayOfEntities)
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

void
CAutoLWPolyline::FixMirroring(db_handitem* pDbHandItem)
{
	if (!pDbHandItem)
		return;
	
	int nVerts;
	pDbHandItem->get_90(&nVerts);

	for (int i = 0; i < nVerts; i++)
	{
		double bulge;

		if (pDbHandItem->get_42(&bulge, i))
			if (!icadRealEqual(bulge,0.0))
				pDbHandItem->set_42(-bulge, i);
	}

	return;
}

STDMETHODIMP CAutoLWPolyline::GetBulge (int Index, double * pBulge)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pBulge == NULL)
		return E_POINTER;
	
	int nVerts;
	m_pDbHandItem->get_90(&nVerts);

	if (Index < 1 || Index > nVerts)
		return E_INVALIDARG;

	if (!m_pDbHandItem->get_42(pBulge, Index - 1)) 
		return E_FAIL;
	
	return NOERROR;
	}

STDMETHODIMP CAutoLWPolyline::GetWidth (int Index, double * StartWidth, double * EndWidth)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (StartWidth == NULL)
		return E_POINTER;
	
	if (EndWidth == NULL)
		return E_POINTER;
	
	int nVerts;
	m_pDbHandItem->get_90(&nVerts);

	if (Index < 1 || Index > nVerts)
		return E_INVALIDARG;

	if (!m_pDbHandItem->get_40(StartWidth, Index - 1)) 
		return E_FAIL;
	
	if (!m_pDbHandItem->get_41(EndWidth, Index - 1)) 
		return E_FAIL;
	
	return NOERROR;
	}

STDMETHODIMP CAutoLWPolyline::Offset (double Offset, IIcadSelectionSet * * ppArrayOfEntities)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppArrayOfEntities == NULL)
		return E_POINTER;

	if (icadRealEqual(Offset,0.0))
		return E_INVALIDARG;
	
    sds_name ename, ssetout;
	ename[0] = (long)m_pDbHandItem;
	ename[1] = (long)(m_pDoc->m_dbDrawing);

	sds_point dir, xvec, yvec, zvec, p1, p2;
	zvec[0] = zvec[1] = 0.0; zvec[2] = 1.0;
	m_pDbHandItem->get_10 (p1, 0);
	m_pDbHandItem->get_10 (p2, 1);
	ic_sub (p1, p2, xvec);
	m_pDbHandItem->get_210(zvec);
	ic_crossproduct (xvec, zvec, yvec);

	if (Offset < 0.0)
		ic_add (yvec, p1, dir);
	else
		ic_sub (yvec, p1, dir);
	
	HRESULT hr = m_pDoc->AUTO_cmd_offset_func(ename, fabs(Offset), 0, 1, dir, ssetout);
	if (FAILED(hr))
		return E_FAIL;

	hr = ConvertSDSssetToAutoSelectionSet (ssetout, ppArrayOfEntities, m_pDoc);
	if (FAILED(hr))
		return E_FAIL;

	m_pDoc->AUTO_ssfree (ssetout);

	return NOERROR;
	}

STDMETHODIMP CAutoLWPolyline::SetBulge (int Index, double Bulge)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	int nVerts;
	m_pDbHandItem->get_90(&nVerts);

	if (Index < 1 || Index > nVerts)
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_42(Bulge, Index - 1)) 
		return E_FAIL;
	
	return NOERROR;
	}

STDMETHODIMP CAutoLWPolyline::SetWidth (int SegmentIndex, double StartWidth, double EndWidth)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	int nVerts;
	m_pDbHandItem->get_90(&nVerts);

	if (SegmentIndex < 1 || SegmentIndex > nVerts)
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_40(StartWidth, SegmentIndex - 1)) 
		return E_FAIL;
	
	if (!m_pDbHandItem->set_41(EndWidth, SegmentIndex - 1)) 
		return E_FAIL;
	
	return NOERROR;
	}



