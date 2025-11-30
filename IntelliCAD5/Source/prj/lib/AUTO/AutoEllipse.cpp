/* INTELLICAD\PRJ\LIB\AUTO\AUTOELLIPSE.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Implementation of CAutoEllipse - the Automation Ellipse object
 * 
 */ 

#include "StdAfx.h"
#include "AutoEllipse.h"
#include "AutoPoint.h"
#include "AutoVector.h"
#include "AutoHelpers.h"
#include "splines.h"	// for using of gr_get_ellipse_edata in get_Length

/////////////////////////////////////////////////////////////////////////////
// CAutoEllipse

HRESULT CAutoEllipse::Init(CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem)
	{
	CAutoDbItem::Init(pDoc, pParent);
	
	if (pDbHandItem)
		m_pDbHandItem = pDbHandItem;
	else
		{
		if (pDoc)
			m_pDbHandItem = new db_ellipse(pDoc->m_dbDrawing);
		else
			m_pDbHandItem = new db_ellipse();
		}
	
	if (NULL == m_pDbHandItem) 
		return FALSE;
	
	return NOERROR;
	}

STDMETHODIMP CAutoEllipse::get_Area(double* pArea)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if(pArea == NULL)
		return E_POINTER;
	
	/*DG - 21.11.2001*/// Implementing of the method.
	sds_point	majAxis;
	double		ratio, startAng, endAng, majAxisSemiLen;
	m_pDbHandItem->get_11(majAxis);
	m_pDbHandItem->get_40(&ratio);
	m_pDbHandItem->get_41(&startAng);
	m_pDbHandItem->get_42(&endAng);
	majAxisSemiLen = sqrt(majAxis[0] * majAxis[0] + majAxis[1] * majAxis[1] + majAxis[2] * majAxis[2]);
	ic_angpar(startAng, &startAng, majAxisSemiLen, majAxisSemiLen * ratio, 1);
	ic_angpar(endAng, &endAng, majAxisSemiLen, majAxisSemiLen * ratio, 1);

	if(fabs(startAng) < IC_ZRO && (fabs(endAng - IC_TWOPI) < IC_ZRO || fabs(endAng) < IC_ZRO))
	{
		*pArea = IC_PI * ratio * majAxisSemiLen * majAxisSemiLen;
		return NOERROR;
	}
	else
	{
		*pArea = 0.0;
		return S_FALSE;
	}
}


STDMETHODIMP CAutoEllipse::get_Length(double* pLength)
{
	RETURN_IF_INVALID(m_pDbHandItem);
	if(pLength == NULL)
		return E_POINTER;
	
	/*DG - 21.11.2001*/// Implementing of the method.
	*pLength = 0.0;
	resbuf*		elist;
	if(!(elist = m_pDbHandItem->entgetx(NULL, m_pDoc->m_dbDrawing)))
		return E_FAIL;
	int	ret = gr_get_ellipse_edata(elist, pLength, NULL);
	sds_relrb(elist);

	if(ret)
		return E_FAIL;
	else
		return NOERROR;
}

STDMETHODIMP CAutoEllipse::get_Center (IIcadPoint * * ppCenter)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppCenter == NULL)
		return E_POINTER;
	
	sds_point pt;
	m_pDbHandItem->get_10 (pt);
	//TODO convert the point to UCS
	
	CComObject<CAutoPoint> *pPoint;
	CComObject<CAutoPoint>::CreateInstance(&pPoint);
	if (!pPoint)
		return E_FAIL;
	
	pPoint->Init(NULL, NULL, pt[0], pt[1], pt[2]);
	
	return pPoint->QueryInterface (ppCenter);
	}

STDMETHODIMP CAutoEllipse::put_Center (IIcadPoint * ppCenter)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	sds_point pt;
	pt[0] = pt[1] = pt[2] = 0.0;
	if (ppCenter)	//this is an optional arg
		{
		ppCenter->get_x(&pt[0]);
		ppCenter->get_y(&pt[1]);
		ppCenter->get_z(&pt[2]);
		//TODO convert the point from UCS to ECS
		}
	m_pDbHandItem->set_10 (pt);
	
	return NOERROR;
	}

STDMETHODIMP CAutoEllipse::get_EndAngle (double * pAngle)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pAngle == NULL)
		return E_POINTER;
	
	m_pDbHandItem->get_42 (pAngle);
	return NOERROR;
	}

STDMETHODIMP CAutoEllipse::put_EndAngle (double pAngle)
	{
	RETURN_IF_INVALID (m_pDbHandItem);

	double startAngle, endAngle;
	m_pDbHandItem->get_41 (&startAngle);
	endAngle = pAngle;

	if (startAngle == endAngle)
		endAngle += IC_TWOPI;

	ic_normang (&startAngle, &endAngle);
	
	m_pDbHandItem->set_41 (startAngle);
	m_pDbHandItem->set_42 (endAngle);
		
	return NOERROR;
	}

STDMETHODIMP CAutoEllipse::get_EndParameter (double * pParam)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pParam == NULL)
		return E_POINTER;
	
	m_pDbHandItem->get_42 (pParam);
	return NOERROR;
	}

STDMETHODIMP CAutoEllipse::put_EndParameter (double pParam)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	m_pDbHandItem->set_42 (pParam);
	return NOERROR;
	}

STDMETHODIMP CAutoEllipse::get_EndPoint(IIcadPoint** ppEndPoint)
{
	RETURN_IF_INVALID(m_pDbHandItem);
	if(ppEndPoint == NULL)
		return E_POINTER;

	/*DG - 21.11.2001*/// Implementing of the method.
	CComObject<CAutoPoint>*	pPoint;
	CComObject<CAutoPoint>::CreateInstance(&pPoint);
	if(!pPoint)
		return E_FAIL;

	sds_point	center, majAxis, minAxis;
	double		ratio, endAng, majAxisSemiLen, cosine, sine;
	m_pDbHandItem->get_10(center);
	m_pDbHandItem->get_11(majAxis);
	m_pDbHandItem->get_40(&ratio);
	m_pDbHandItem->get_42(&endAng);
	minAxis[0] = -ratio * majAxis[1];
	minAxis[1] =  ratio * majAxis[0];
	minAxis[2] = -ratio * majAxis[2];
	majAxisSemiLen = sqrt(majAxis[0] * majAxis[0] + majAxis[1] * majAxis[1] + majAxis[2] * majAxis[2]);
	ic_angpar(endAng, &endAng, majAxisSemiLen, majAxisSemiLen * ratio, 1);
	sine = sin(endAng);
	cosine = cos(endAng);

	pPoint->Init(NULL, NULL,
				 center[0] + majAxis[0] * cosine + minAxis[0] * sine,
				 center[1] + majAxis[1] * cosine + minAxis[1] * sine,
				 center[2] + majAxis[2] * cosine + minAxis[2] * sine);
	
	return pPoint->QueryInterface(ppEndPoint);
}

STDMETHODIMP CAutoEllipse::get_Normal (IIcadVector * * ppExtrusionDirection)
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

STDMETHODIMP CAutoEllipse::put_Normal (IIcadVector * ExtrusionDirection)
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

STDMETHODIMP CAutoEllipse::get_MajorAxis (IIcadVector * * ppMajorAxis)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppMajorAxis == NULL)
		return E_POINTER;
	
	sds_point v2;
	m_pDbHandItem->get_11(v2);
	
	CComObject<CAutoVector> *pVector;
	CComObject<CAutoVector>::CreateInstance(&pVector);
	if (!pVector)
		return E_FAIL;
	
	pVector->Init(NULL, v2[0], v2[1], v2[2]);
	
	return pVector->QueryInterface (ppMajorAxis);
	}

STDMETHODIMP CAutoEllipse::put_MajorAxis (IIcadVector * ppMajorAxis)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	sds_point v2;
	v2[0] = 1.0; v2[1] = v2[2] = 0.0;
	if (ppMajorAxis)
		{
		ppMajorAxis->get_x (&v2[0]);
		ppMajorAxis->get_y (&v2[1]);
		ppMajorAxis->get_z (&v2[2]);
		}

	if (ic_veclength (v2) < IC_ZRO)
		return E_INVALIDARG;

	m_pDbHandItem->set_11(v2);
	
	return NOERROR;
	}

STDMETHODIMP CAutoEllipse::get_MinorAxis (IIcadVector * * ppMinorAxis)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppMinorAxis == NULL)
		return E_POINTER;
	
	sds_point v1;
	v1[0] = v1[1] = v1[2] = 0.0;
	//m_pDbHandItem->get_10(v1);
	sds_point v2;
	m_pDbHandItem->get_11(v2);
	sds_real ratio;
	m_pDbHandItem->get_40(&ratio);

	CComObject<CAutoVector> *pVector;
	CComObject<CAutoVector>::CreateInstance(&pVector);
	if (!pVector)
		return E_FAIL;
	
	pVector->Init(NULL, (v2[0] - v1[0])*ratio, (v2[1] - v1[1])*ratio, (v2[2] - v1[2])*ratio);
	
	return pVector->QueryInterface (ppMinorAxis);
	}

STDMETHODIMP CAutoEllipse::get_RadiusRatio (double * pRadius)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pRadius == NULL)
		return E_POINTER;
	
	m_pDbHandItem->get_40(pRadius);
	return NOERROR;
	}

STDMETHODIMP CAutoEllipse::put_RadiusRatio (double pRadius)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pRadius <= 0.0)
		return E_INVALIDARG;

	m_pDbHandItem->set_40(pRadius);
	return NOERROR;
	}

STDMETHODIMP CAutoEllipse::get_StartAngle (double * pAngle)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pAngle == NULL)
		return E_POINTER;
	
	m_pDbHandItem->get_41 (pAngle);
	return NOERROR;
	}

STDMETHODIMP CAutoEllipse::put_StartAngle (double pAngle)
	{
	RETURN_IF_INVALID (m_pDbHandItem);

	double startAngle, endAngle;
	m_pDbHandItem->get_42 (&endAngle);
	startAngle = pAngle;
	
	if (startAngle == endAngle)
		endAngle += IC_TWOPI;

	ic_normang (&startAngle, &endAngle);
	
	m_pDbHandItem->set_41 (startAngle);
	m_pDbHandItem->set_42 (endAngle);

	return NOERROR;
	}

STDMETHODIMP CAutoEllipse::get_StartParameter (double * pParam)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pParam == NULL)
		return E_POINTER;
	
	m_pDbHandItem->get_41 (pParam);
	return NOERROR;
	}

STDMETHODIMP CAutoEllipse::put_StartParameter (double pParam)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	m_pDbHandItem->set_41 (pParam);
	return NOERROR;
	}

STDMETHODIMP CAutoEllipse::get_StartPoint(IIcadPoint** ppStartPoint)
{
	RETURN_IF_INVALID(m_pDbHandItem);
	if(ppStartPoint == NULL)
		return E_POINTER;

	/*DG - 21.11.2001*/// Implementing of the method.
	CComObject<CAutoPoint>*	pPoint;
	CComObject<CAutoPoint>::CreateInstance(&pPoint);
	if(!pPoint)
		return E_FAIL;

	sds_point	center, majAxis, minAxis;
	double		ratio, startAng, majAxisSemiLen, cosine, sine;
	m_pDbHandItem->get_10(center);
	m_pDbHandItem->get_11(majAxis);
	m_pDbHandItem->get_40(&ratio);
	m_pDbHandItem->get_41(&startAng);
	minAxis[0] = -ratio * majAxis[1];
	minAxis[1] =  ratio * majAxis[0];
	minAxis[2] = -ratio * majAxis[2];
	majAxisSemiLen = sqrt(majAxis[0] * majAxis[0] + majAxis[1] * majAxis[1] + majAxis[2] * majAxis[2]);
	ic_angpar(startAng, &startAng, majAxisSemiLen, majAxisSemiLen * ratio, 1);
	sine = sin(startAng);
	cosine = cos(startAng);

	pPoint->Init(NULL, NULL,
				 center[0] + majAxis[0] * cosine + minAxis[0] * sine,
				 center[1] + majAxis[1] * cosine + minAxis[1] * sine,
				 center[2] + majAxis[2] * cosine + minAxis[2] * sine);
	
	return pPoint->QueryInterface(ppStartPoint);
}

STDMETHODIMP CAutoEllipse::Offset (double Offset, IIcadSelectionSet * * ppArrayOfEntities)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppArrayOfEntities == NULL)
		return E_POINTER;

	if (icadRealEqual(Offset,0.0))
		return E_INVALIDARG;
	
    sds_name ename, ssetout;
	ename[0] = (long)m_pDbHandItem;
	ename[1] = (long)(m_pDoc->m_dbDrawing);

	sds_point dir;
	if (Offset > 0.0)
		{
		m_pDbHandItem->get_11 (dir);
		dir[0] *= 1.1;
		dir[1] *= 1.1;
		}
	else
		{
		m_pDbHandItem->get_10 (dir);
		}
	
	HRESULT hr = m_pDoc->AUTO_cmd_offset_func(ename, fabs(Offset), 0, 0, dir, ssetout);
	if (FAILED(hr))
		return E_FAIL;

	hr = ConvertSDSssetToAutoSelectionSet (ssetout, ppArrayOfEntities, m_pDoc);
	if (FAILED(hr))
		return E_FAIL;

	m_pDoc->AUTO_ssfree (ssetout);

	return NOERROR;
	}



