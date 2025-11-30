/* INTELLICAD\PRJ\LIB\AUTO\AUTOSPLINE.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Implementation of CAutoSpline - the Automation Spline object
 * 
 */ 

#include "StdAfx.h"
#include "AutoSpline.h"
#include "AutoPoint.h"
#include "AutoVector.h"
#include "AutoHelpers.h"
#include "splines.h"	// for using of gr_get_spline_edata

/////////////////////////////////////////////////////////////////////////////
// CAutoSpline

HRESULT CAutoSpline::Init(CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem)
	{
	CAutoDbItem::Init(pDoc, pParent);
	
	if (pDbHandItem)
		m_pDbHandItem = pDbHandItem;
	else
		{
		if (pDoc)
			m_pDbHandItem = new db_spline(pDoc->m_dbDrawing);
		else
			m_pDbHandItem = new db_spline();
		}
	
	if (NULL == m_pDbHandItem) 
		return FALSE;
	
	return NOERROR;
	}

STDMETHODIMP CAutoSpline::get_Area(double* pArea)
{
	/*DG - 11.12.2001*/// Implemented the method.
	RETURN_IF_INVALID((m_pDbHandItem && pArea))
	
	*pArea = 0.0;
	resbuf*		elist;
	if(!(elist = m_pDbHandItem->entgetx(NULL, m_pDoc->m_dbDrawing)))
		return E_FAIL;
	int	ret = gr_get_spline_edata(elist, NULL, pArea);
	sds_relrb(elist);

	if(ret)
		return E_FAIL;
	else
		return S_OK;
}

STDMETHODIMP CAutoSpline::get_Length(double* pLength)
{
	/*DG - 11.12.2001*/// Implemented the method.
	RETURN_IF_INVALID((m_pDbHandItem && pLength))
	
	*pLength = 0.0;
	resbuf*		elist;
	if(!(elist = m_pDbHandItem->entgetx(NULL, m_pDoc->m_dbDrawing)))
		return E_FAIL;
	int	ret = gr_get_spline_edata(elist, pLength, NULL);
	sds_relrb(elist);

	if(ret)
		return E_FAIL;
	else
		return S_OK;
}

STDMETHODIMP CAutoSpline::get_Closed (VARIANT_BOOL * pClosed)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pClosed == NULL)
		return E_POINTER;
	
	int flags = 0;
	if (!m_pDbHandItem->get_70(&flags)) 
		return E_FAIL;
	
	if (flags & IC_SPLINE_CLOSED) 
		*pClosed = VARIANT_TRUE;
	else 
		*pClosed = VARIANT_FALSE;
	
	return NOERROR;
	}

STDMETHODIMP CAutoSpline::put_Closed(VARIANT_BOOL closed)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	int	flags = 0;
	
	if(!m_pDbHandItem->get_70(&flags)) 
		return E_FAIL;

	if(closed)
	{
		/*DG - 11.12.2001*/// Check if we really may set the 'closed' flag.
		int			fitPointsNum;
		m_pDbHandItem->get_74(&fitPointsNum);
		sds_real	*pFirstFitPoint = m_pDbHandItem->retp_11(0),
					*pLastFitPoint = m_pDbHandItem->retp_11(fitPointsNum-1);

		if( !pFirstFitPoint || !pLastFitPoint ||
			fabs(pFirstFitPoint[0] - pLastFitPoint[0]) > IC_ZRO ||
			fabs(pFirstFitPoint[1] - pLastFitPoint[1]) > IC_ZRO ||
			fabs(pFirstFitPoint[2] - pLastFitPoint[2]) > IC_ZRO )
			return S_FALSE;

		flags |= IC_SPLINE_CLOSED;
	}
	else
		flags &= ~IC_SPLINE_CLOSED;
	
	if(!m_pDbHandItem->set_70(flags)) 
		return E_FAIL;
	
	return S_OK;
}

STDMETHODIMP CAutoSpline::get_Degree (int * pDegree)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pDegree == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_71(pDegree)) 
		return E_FAIL;
	
	return NOERROR;
	}

STDMETHODIMP CAutoSpline::get_EndTangent (IIcadVector * * ppTangent)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppTangent == NULL)
		return E_POINTER;
	
	sds_point v;
	m_pDbHandItem->get_13(v);
	
	CComObject<CAutoVector> *pVector;
	CComObject<CAutoVector>::CreateInstance(&pVector);
	if (!pVector)
		return E_FAIL;
	
	pVector->Init(NULL, v[0], v[1], v[2]);
	
	return pVector->QueryInterface (ppTangent);
	}

STDMETHODIMP CAutoSpline::put_EndTangent (IIcadVector * ppTangent)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	sds_point v;
	v[0] = 1.0; v[1] = 0.0;	v[2] = 0.0;
	if (ppTangent != NULL)
		{
		ppTangent->get_x(&v[0]);
		ppTangent->get_y(&v[1]);
		ppTangent->get_z(&v[2]);

		sds_point vn;
		ic_ptcpy (vn, v);
		if (db_unitizevector(vn) != 0)
			return E_INVALIDARG;
		}

	if (!m_pDbHandItem->set_13(v))
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoSpline::get_FitTolerance (double * Tolerance)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (Tolerance == NULL)
		return E_POINTER;
	
	m_pDbHandItem->get_44 (Tolerance);
	return NOERROR;
	}

STDMETHODIMP CAutoSpline::put_FitTolerance (double Tolerance)
	{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (Tolerance < 0.0)
		return E_INVALIDARG;

	m_pDbHandItem->set_44 (Tolerance);
	return NOERROR;
	}

STDMETHODIMP CAutoSpline::get_IsRational (VARIANT_BOOL * pRational)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pRational == NULL)
		return E_POINTER;
	
	int flags = 0;
	if (!m_pDbHandItem->get_70(&flags)) 
		return E_FAIL;
	
	if (flags & IC_SPLINE_RATIONAL) 
		*pRational = VARIANT_TRUE;
	else 
		*pRational = VARIANT_FALSE;
	
	return NOERROR;
	}

STDMETHODIMP CAutoSpline::get_StartTangent (IIcadVector * * ppTangent)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppTangent == NULL)
		return E_POINTER;
	
	sds_point v;
	m_pDbHandItem->get_12(v);
	
	CComObject<CAutoVector> *pVector;
	CComObject<CAutoVector>::CreateInstance(&pVector);
	if (!pVector)
		return E_FAIL;
	
	pVector->Init(NULL, v[0], v[1], v[2]);
	
	return pVector->QueryInterface (ppTangent);
	}

STDMETHODIMP CAutoSpline::put_StartTangent (IIcadVector * ppTangent)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	sds_point v;
	v[0] = 1.0; v[1] = 0.0; v[2] = 0.0;
	if (ppTangent != NULL)
		{
		ppTangent->get_x(&v[0]);
		ppTangent->get_y(&v[1]);
		ppTangent->get_z(&v[2]);

		sds_point vn;
		ic_ptcpy (vn, v);
		if (db_unitizevector(vn) != 0)
			return E_INVALIDARG;
		}

	if (!m_pDbHandItem->set_12(v))
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoSpline::get_NumberOfControlPoints (int * pNumControlPoints)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pNumControlPoints == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_73(pNumControlPoints)) 
		return E_FAIL;
	
	return NOERROR;
	}

STDMETHODIMP CAutoSpline::get_NumberOfFitPoints (int * pNumFitPoints)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pNumFitPoints == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_74(pNumFitPoints)) 
		return E_FAIL;
	
	return NOERROR;
	}

STDMETHODIMP CAutoSpline::get_ControlPoints (IIcadPoints * * ppCoords)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppCoords == NULL)
		return E_POINTER;

	int numCtrlPts = 0, i;
	HRESULT hr;

	hr = get_NumberOfControlPoints (&numCtrlPts);
	if (FAILED(hr))
		return E_FAIL;

	sds_point *CtrlPoints = new sds_point[numCtrlPts];
	if (!CtrlPoints)
		return E_OUTOFMEMORY;

	if (!m_pDbHandItem->get_10(CtrlPoints, numCtrlPts))
		{
		delete [] CtrlPoints;
		return E_FAIL;
		}

	CComObject<CAutoPoints> *pVertices;
	CComObject<CAutoPoints>::CreateInstance(&pVertices);
	
	CComPtr<IIcadPoints> Vertices;
	pVertices->QueryInterface(&Vertices);
	if (!Vertices)
		return E_OUTOFMEMORY;

	CComPtr<IIcadPoint> Vertex = NULL;
	for (i = 0; i < numCtrlPts; i++)
		{
		Vertices->Add (CtrlPoints[i][0], CtrlPoints[i][1], CtrlPoints[i][2], 0, &Vertex);

		if (Vertex == NULL)
			return E_OUTOFMEMORY;
		
		Vertex = NULL;
		}

	delete [] CtrlPoints;

	return Vertices->QueryInterface (ppCoords);
	}

STDMETHODIMP CAutoSpline::put_ControlPoints (IIcadPoints * ppCoords)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppCoords == NULL)
		return E_POINTER;

	int numControlPts = 0;
	long i;
	HRESULT hr;

	hr = get_NumberOfControlPoints (&numControlPts);
	if (FAILED(hr))
		return E_FAIL;

	hr = ppCoords->get_Count (&i);
	if (FAILED(hr))
		return E_FAIL;

	if (numControlPts != 0)		//we have some existing control points
		{
		if (i != numControlPts)	//then make sure we are not adding new ones
			return E_INVALIDARG;
		}
	numControlPts = i;

	CComPtr<IIcadPoint> Vertex;
	sds_point *points = new sds_point [numControlPts];

	for (i = 0; i < numControlPts; i++)
		{
		ppCoords->Item(i+1, &Vertex); //AutoPoints is 1 based indexing

		if (Vertex == NULL)
			return E_FAIL;
		
		Vertex->get_x(&points[i][0]);
		Vertex->get_y(&points[i][1]);
		Vertex->get_z(&points[i][2]);
		Vertex = NULL;
		}

	if (!m_pDbHandItem->set_10 (points, numControlPts))
		return E_FAIL;

	delete [] points;
	return NOERROR;
	}

STDMETHODIMP CAutoSpline::get_FitPoints (IIcadPoints * * ppCoords)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppCoords == NULL)
		return E_POINTER;

	int numFitPts = 0, i;
	HRESULT hr;

	hr = get_NumberOfFitPoints (&numFitPts);
	if (FAILED(hr))
		return E_FAIL;

	sds_point *FitPoints = new sds_point[numFitPts];
	if (!FitPoints)
		return E_OUTOFMEMORY;

	if (!m_pDbHandItem->get_11(FitPoints, numFitPts))
		{
		delete [] FitPoints;
		return E_FAIL;
		}

	CComObject<CAutoPoints> *pVertices;
	CComObject<CAutoPoints>::CreateInstance(&pVertices);
	
	CComPtr<IIcadPoints> Vertices;
	pVertices->QueryInterface(&Vertices);
	if (!Vertices)
		return E_OUTOFMEMORY;

	CComPtr<IIcadPoint> Vertex = NULL;
	for (i = 0; i < numFitPts; i++)
		{
		Vertices->Add (FitPoints[i][0], FitPoints[i][1], FitPoints[i][2], 0, &Vertex);

		if (Vertex == NULL)
			return E_OUTOFMEMORY;
		
		Vertex = NULL;
		}

	delete [] FitPoints;

	return Vertices->QueryInterface (ppCoords);

	}

STDMETHODIMP CAutoSpline::put_FitPoints (IIcadPoints * ppCoords)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppCoords == NULL)
		return E_POINTER;

	int numFitPts = 0;
	long i;
	HRESULT hr;

	hr = get_NumberOfFitPoints (&numFitPts);
	if (FAILED(hr))
		return E_FAIL;

	hr = ppCoords->get_Count (&i);
	if (FAILED(hr))
		return E_FAIL;

	//if there are no fit points, we are creating a new spline.  Ensure at least 2 points
	if (numFitPts == 0)
		{
		 if (i < 2)
			return E_INVALIDARG;
		}
	else	//we are modifying an existing spline - it must have the same number of fit points
		{
		if (i != numFitPts)
			return E_INVALIDARG;
		}
	numFitPts = i;

	CComPtr<IIcadPoint> Vertex;
	sds_point *points = new sds_point [numFitPts];

	for (i = 0; i < numFitPts; i++)
		{
		ppCoords->Item(i+1, &Vertex); //AutoPoints is 1 based indexing

		if (Vertex == NULL)
			return E_FAIL;
		
		Vertex->get_x(&points[i][0]);
		Vertex->get_y(&points[i][1]);
		Vertex->get_z(&points[i][2]);
		Vertex = NULL;
		}

	if (!m_pDbHandItem->set_11 (points, numFitPts))
		return E_FAIL;

	/*DG - 11.12.2001*/// Set the 'planar' flag if the spline is planar.
	for(i = numFitPts - 1; i--; )
		if(fabs(points[i][2] - points[i+1][2]) > IC_ZRO)
			break;

	if(i < 0)
	{
		int	flags;
		m_pDbHandItem->get_70(&flags);
		m_pDbHandItem->set_70(flags | IC_SPLINE_PLANAR);
	}

	delete [] points;
	return NOERROR;
	}

STDMETHODIMP CAutoSpline::AddFitPoint (int Index, IIcadPoint * FitPoint)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	int numFitPts = 0, i, j;
	int lIndex = Index - 1;		//lIndex is our local 0 based index
	HRESULT hr;

	hr = get_NumberOfFitPoints (&numFitPts);
	if (lIndex > numFitPts)
		return E_INVALIDARG;

	sds_point *fitPoints = new sds_point[numFitPts];
	if (!fitPoints)
		return E_OUTOFMEMORY;

	if (!m_pDbHandItem->get_11(fitPoints, numFitPts))
		{
		delete [] fitPoints;
		return E_FAIL;
		}

	sds_point *newfitPoints = new sds_point[numFitPts + 1];
	if (!newfitPoints)
		return E_OUTOFMEMORY;

	for (i = 0, j = 0; i < numFitPts + 1; i++, j++)
		{
		if (i == lIndex )
			{
			sds_point p;
			if (FitPoint)
				{
				FitPoint->get_x(&p[0]);
				FitPoint->get_y(&p[1]);
				FitPoint->get_z(&p[2]);
				}

			(newfitPoints[i])[0] = p[0];
			(newfitPoints[i])[1] = p[1];
			(newfitPoints[i])[2] = p[2];
			j--;
			}
		else
			{
			(newfitPoints[i])[0] = (fitPoints[j])[0];
			(newfitPoints[i])[1] = (fitPoints[j])[1];
			(newfitPoints[i])[2] = (fitPoints[j])[2];
			}
		}

	if (!m_pDbHandItem->set_11(newfitPoints, numFitPts + 1))
		hr = E_FAIL;
	else
		hr = NOERROR;

	delete [] fitPoints;
	delete [] newfitPoints;

	return hr;
	}

STDMETHODIMP CAutoSpline::DeleteFitPoint (int Index)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	int numFitPts = 0, i;
	int lIndex = Index - 1;		//lIndex is our local 0 based index
	HRESULT hr;

	hr = get_NumberOfFitPoints (&numFitPts);
	if (lIndex >= numFitPts)
		return E_INVALIDARG;

	sds_point *fitPoints = new sds_point[numFitPts];
	if (!fitPoints)
		return E_OUTOFMEMORY;

	if (!m_pDbHandItem->get_11(fitPoints, numFitPts))
		{
		delete [] fitPoints;
		return E_FAIL;
		}

	for (i = lIndex; i < numFitPts - 1; i++)
		{
		(fitPoints[i])[0] = (fitPoints[i+1])[0];
		(fitPoints[i])[1] = (fitPoints[i+1])[1];
		(fitPoints[i])[2] = (fitPoints[i+1])[2];
		}

	if (!m_pDbHandItem->set_11(fitPoints, numFitPts - 1))
		hr = E_FAIL;
	else
		hr = NOERROR;

	delete [] fitPoints;
	return hr;
	}

STDMETHODIMP CAutoSpline::ElevateOrder (long Order)
	{
	return E_NOTIMPL;
	}

STDMETHODIMP CAutoSpline::GetControlPoint (int Index, IIcadPoint * * ppControlPoint)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppControlPoint == NULL)
		return E_POINTER;
	
	int numCtrlPts = 0;
	int lIndex = Index - 1;		//lIndex is our local 0 based index
	HRESULT hr;

	hr = get_NumberOfControlPoints (&numCtrlPts);
	if (lIndex >= numCtrlPts)
		return E_INVALIDARG;

	sds_point *CtrlPoints = new sds_point[numCtrlPts];
	if (!CtrlPoints)
		return E_OUTOFMEMORY;

	if (!m_pDbHandItem->get_10(CtrlPoints, numCtrlPts))
		{
		delete [] CtrlPoints;
		return E_FAIL;
		}

	hr = E_FAIL;
	CComObject<CAutoPoint> *pPoint;
	CComObject<CAutoPoint>::CreateInstance(&pPoint);
	if (pPoint)
		{
		pPoint->Init(NULL, NULL, (CtrlPoints[lIndex])[0], (CtrlPoints[lIndex])[1], (CtrlPoints[lIndex])[2]);
		
		hr = pPoint->QueryInterface (ppControlPoint);
		}
	
	delete [] CtrlPoints;
	return hr;	
	}

STDMETHODIMP CAutoSpline::GetFitPoint (int Index, IIcadPoint * * ppFitPoint)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppFitPoint == NULL)
		return E_POINTER;
	
	int numFitPts = 0;
	int lIndex = Index - 1;		//lIndex is our local 0 based index
	HRESULT hr;

	hr = get_NumberOfFitPoints (&numFitPts);
	if (lIndex >= numFitPts)
		return E_INVALIDARG;

	sds_point *fitPoints = new sds_point[numFitPts];
	if (!fitPoints)
		return E_OUTOFMEMORY;

	if (!m_pDbHandItem->get_11(fitPoints, numFitPts))
		{
		delete [] fitPoints;
		return E_FAIL;
		}

	hr = E_FAIL;
	CComObject<CAutoPoint> *pPoint;
	CComObject<CAutoPoint>::CreateInstance(&pPoint);
	if (pPoint)
		{
		pPoint->Init(NULL, NULL, (fitPoints[lIndex])[0], (fitPoints[lIndex])[1], (fitPoints[lIndex])[2]);
		
		hr = pPoint->QueryInterface (ppFitPoint);
		}
	
	delete [] fitPoints;
	return hr;	
	}

STDMETHODIMP CAutoSpline::GetWeight (int Index, double * pWeight)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pWeight == NULL)
		return E_POINTER;
	
	int numCtrlPts = 0;
	int lIndex = Index - 1;		//lIndex is our local 0 based index
	HRESULT hr;

	hr = get_NumberOfControlPoints (&numCtrlPts);
	if (numCtrlPts == 0)
		{
		*pWeight = 1.0;
		return NOERROR;
		}

	if (lIndex >= numCtrlPts)
		return E_INVALIDARG;

	sds_real *Weights = new sds_real[numCtrlPts];
	if (!Weights)
		return E_OUTOFMEMORY;

	if (!m_pDbHandItem->get_41array (Weights, numCtrlPts))
		{
		delete [] Weights;
		return E_FAIL;
		}

	*pWeight = Weights[lIndex];

	delete [] Weights;

	return NOERROR;	
	}

STDMETHODIMP CAutoSpline::Offset (double Offset, IIcadSelectionSet * * ppArrayOfEntities)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppArrayOfEntities == NULL)
		return E_POINTER;

	if (icadRealEqual(Offset,0.0))
		return E_INVALIDARG;
	
    sds_name ename, ssetout;
	ename[0] = (long)m_pDbHandItem;
	ename[1] = (long)(m_pDoc->m_dbDrawing);

	sds_point dir, xvec, yvec, zvec;
	zvec[0] = zvec[1] = 0.0; zvec[2] = 1.0;
	m_pDbHandItem->get_11 (xvec);
	m_pDbHandItem->get_210(zvec);
	ic_crossproduct (xvec, zvec, yvec);

	if (Offset < 0.0)
		ic_add (yvec, xvec, dir);
	else
		ic_sub (yvec, xvec, dir);
	
	HRESULT hr = m_pDoc->AUTO_cmd_offset_func(ename, fabs(Offset), 0, 0, dir, ssetout);
	if (FAILED(hr))
		return E_FAIL;

	hr = ConvertSDSssetToAutoSelectionSet (ssetout, ppArrayOfEntities, m_pDoc);
	if (FAILED(hr))
		return E_FAIL;

	m_pDoc->AUTO_ssfree (ssetout);

	return NOERROR;
	}

STDMETHODIMP CAutoSpline::PurgeFitData ()
	{
	return E_NOTIMPL;
	}

STDMETHODIMP CAutoSpline::Reverse ()
	{
	return E_NOTIMPL;
	}

STDMETHODIMP CAutoSpline::SetControlPoint (int Index, IIcadPoint * ControlPoint)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	int numCtrlPts = 0;
	int lIndex = Index - 1;		//lIndex is our local 0 based index
	HRESULT hr;

	hr = get_NumberOfControlPoints (&numCtrlPts);
	if (lIndex >= numCtrlPts)
		return E_INVALIDARG;

	sds_point *CtrlPoints = new sds_point[numCtrlPts];
	if (!CtrlPoints)
		return E_OUTOFMEMORY;

	if (!m_pDbHandItem->get_11(CtrlPoints, numCtrlPts))
		{
		delete [] CtrlPoints;
		return E_FAIL;
		}

	sds_point p;
	if (ControlPoint)
		{
		ControlPoint->get_x(&p[0]);
		ControlPoint->get_y(&p[1]);
		ControlPoint->get_z(&p[2]);
		}

	(CtrlPoints[lIndex])[0] = p[0];
	(CtrlPoints[lIndex])[1] = p[1];
	(CtrlPoints[lIndex])[2] = p[2];

	if (!m_pDbHandItem->set_11(CtrlPoints, numCtrlPts))
		hr = E_FAIL;
	else
		hr = NOERROR;

	delete [] CtrlPoints;

	return hr;
	}

STDMETHODIMP CAutoSpline::SetFitPoint (int Index, IIcadPoint * FitPoint)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	int numFitPts = 0;
	int lIndex = Index - 1;		//lIndex is our local 0 based index
	HRESULT hr;

	hr = get_NumberOfFitPoints (&numFitPts);
	if (lIndex >= numFitPts)
		return E_INVALIDARG;

	sds_point *fitPoints = new sds_point[numFitPts];
	if (!fitPoints)
		return E_OUTOFMEMORY;

	if (!m_pDbHandItem->get_11(fitPoints, numFitPts))
		{
		delete [] fitPoints;
		return E_FAIL;
		}

	sds_point p;
	if (FitPoint)
		{
		FitPoint->get_x(&p[0]);
		FitPoint->get_y(&p[1]);
		FitPoint->get_z(&p[2]);
		}

	(fitPoints[lIndex])[0] = p[0];
	(fitPoints[lIndex])[1] = p[1];
	(fitPoints[lIndex])[2] = p[2];

	if (!m_pDbHandItem->set_11(fitPoints, numFitPts))
		hr = E_FAIL;
	else
		hr = NOERROR;

	delete [] fitPoints;

	return hr;
	}

STDMETHODIMP CAutoSpline::SetWeight (int Index, double Weight)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	int numCtrlPts = 0;
	int lIndex = Index - 1;		//lIndex is our local 0 based index
	HRESULT hr;

	hr = get_NumberOfControlPoints (&numCtrlPts);
	if (lIndex >= numCtrlPts)
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_41 (Weight, lIndex))
		return E_FAIL;

	return NOERROR;	
	}


