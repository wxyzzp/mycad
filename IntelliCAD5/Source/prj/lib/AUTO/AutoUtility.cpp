/* INTELLICAD\PRJ\LIB\AUTO\AUTOUTILITY.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * $Revision: 1.3.10.1 $ $Date: 2003/03/09 00:06:32 $ 
 * 
 * Abstract
 * 
 * Implementation of the CAutoUtility
 * 
 */ 

#include "StdAfx.h"
#include "AutoUtility.h"
#include "AutoPoint.h"
#include "AutoMatrix.h"
#include "AutoHelpers.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoUtility

HRESULT CAutoUtility::Init (CIcadDoc *pDoc)
	{
	m_pDoc = pDoc;
	return NOERROR;
	}

STDMETHODIMP CAutoUtility::get_Application (IIcadApplication * * Application)
	{
	if (Application == NULL)
		return E_POINTER;
	
	return GetApp()->m_pAutoApp->QueryInterface (Application);
	}

STDMETHODIMP CAutoUtility::get_Parent (IIcadDocument * * ppDoc)
	{
	if (ppDoc == NULL)
		return E_POINTER;
	
	return m_pDoc->m_pAutoDoc->QueryInterface (ppDoc);
	}

STDMETHODIMP CAutoUtility::Alert (BSTR message)
	{
	CString msg(message);
	sds_alert (msg);
	return NOERROR;
	}

int ic_pt_equal(sds_point p1, sds_point p2) ;

STDMETHODIMP CAutoUtility::AngleFromXAxis (IIcadPoint * Point1, IIcadPoint * Point2, double * pAngle)
	{
	if (pAngle == NULL)
		return E_POINTER;
	
	if ((Point1 == NULL) || (Point2 == NULL))
		return E_INVALIDARG;
	
	sds_point p1, p2;
	Point1->get_x (&p1[0]);
	Point1->get_y (&p1[1]);
	Point1->get_z (&p1[2]);
	Point2->get_x (&p2[0]);
	Point2->get_y (&p2[1]);
	Point2->get_z (&p2[2]);

	if (ic_pt_equal(p1, p2))
		return E_INVALIDARG;

	/*D.G.*/// Note, for 3D vector we have an ambiguity.
	// We return an angle from 0 to pi, although from another point of view it's from pi to 2*pi.
	if(!icadRealEqual(p1[2],0.0) || !icadRealEqual(p2[2],0.0))
		{
		sds_point v;
		ic_unitvec (p1, p2, v);

		sds_point u;
		u[0] = 1.0; u[1] = 0.0; u[2] = 0.0;

		*pAngle = acos(ic_dotproduct (v, u));
		}
	else
		{
		sds_real dx = p2[0] - p1[0],
				 dy = p2[1] - p1[1];

		if(icadRealEqual(dx,0.0) && icadRealEqual(dy,0.0))
			return E_INVALIDARG;

		if((*pAngle = atan2(dy,dx)) < 0.0) 
			*pAngle += IC_TWOPI;
		}

	return NOERROR;
	}

STDMETHODIMP CAutoUtility::AngleToReal (BSTR Angle, AngleUnits Unit, double * pAngle)
	{
	if (pAngle == NULL)
		return E_POINTER;
	
	CString input(Angle);
	return m_pDoc->AUTO_angtof((LPCSTR)input, Unit, pAngle);
	}

STDMETHODIMP CAutoUtility::DistanceToReal (BSTR Distance, DistanceUnits Unit, double * pDistance)
	{
	if (pDistance == NULL)
		return E_POINTER;
	
	CString input(Distance);
	return m_pDoc->AUTO_distof((LPCSTR)input, Unit, pDistance);
	}

STDMETHODIMP CAutoUtility::RealToString (double Value, DistanceUnits Unit, long Precision, BSTR * Distance)
	{
	if (Distance == NULL)
		return E_POINTER;
	
	char str[IC_ACADBUF];
	HRESULT hr = m_pDoc->AUTO_rtos (Value, Unit, Precision, str, 0);
	if (FAILED (hr))
		return hr;

	CString Str(str);
	*Distance = Str.AllocSysString();

	return NOERROR;
	}

STDMETHODIMP CAutoUtility::AngleToString (double Angle, AngleUnits Unit, long Precision, BSTR * pAngle)
	{
	if (pAngle == NULL)
		return E_POINTER;
	
	char str[IC_ACADBUF];
	HRESULT hr = m_pDoc->AUTO_angtos (Angle, Unit, Precision, str);
	if (FAILED (hr))
		return hr;

	CString Str(str);
	*pAngle = Str.AllocSysString();

	return NOERROR;
	}

STDMETHODIMP CAutoUtility::Prompt (BSTR TextString)
	{
	CString str(TextString);
	if (str.IsEmpty())
		return E_INVALIDARG;

	return m_pDoc->AUTO_prompt (str);
	}

STDMETHODIMP CAutoUtility::ConvertUnit (double Value, BSTR From, BSTR To, double * ConvertedValue)
	{
	if (ConvertedValue == NULL)
		return E_POINTER;
	
	CString from(From);
	CString to(To);
	return m_pDoc->AUTO_cvunit (Value, from, to, ConvertedValue);
	}

STDMETHODIMP CAutoUtility::EntitySnap (IIcadPoint *InputPoint, ObjectSnapMode SnapMode, IIcadPoint * * ppPoint)
	{
	if (ppPoint == NULL)
		return E_POINTER;

	if (InputPoint == NULL)
		return E_INVALIDARG;

	sds_point pt1, pt2;
	InputPoint->get_x (&pt1[0]);
	InputPoint->get_y (&pt1[1]);
	InputPoint->get_z (&pt1[2]);

	CString str;
	str.Empty();

	if (SnapMode & vicOsnapEnd)
		str = str + "END ";
	if (SnapMode & vicOsnapMid)
		str = str + "MID ";
	if (SnapMode & vicOsnapCenter)
		str = str + "CEN ";
	if (SnapMode & vicOsnapNode)
		str = str + "NOD ";
	if (SnapMode & vicOsnapQuadrant)
		str = str + "QUA ";
	if (SnapMode & vicOsnapIntersection)
		str = str + "INT ";
	if (SnapMode & vicOsnapInsert)
		str = str + "INS ";
	if (SnapMode & vicOsnapPerpendicular)
		str = str + "PER ";
	if (SnapMode & vicOsnapTangent)
		str = str + "TAN ";
	if (SnapMode & vicOsnapNear)
		str = str + "NEA ";
	if (SnapMode & vicOsnapQuick)
		str = str + "QUI ";
	if (SnapMode & vicOsnapPoint)
		str = str + "POI ";
	if (SnapMode & vicOsnapPlanViewIntersection)
		str = str + "PLA ";

	if (SnapMode == vicOsnapNone)				//if no mode, try to snap using current OSMODE
		{
		str = "";
		m_pDoc->AUTO_osnap (pt1, str, pt2);	//do not check for failure, because it will fail if OSMODE = 0, which is OK
		}
	else
		{
		if (FAILED (m_pDoc->AUTO_osnap (pt1, str, pt2)))
			return E_FAIL;
		}

	CComObject<CAutoPoint> *pPoint;
	CComObject<CAutoPoint>::CreateInstance(&pPoint);
	if (!pPoint)
		return E_FAIL;
	
	pPoint->Init(NULL, NULL, pt2[0], pt2[1], pt2[2]);
	
	return pPoint->QueryInterface (ppPoint);
	}

STDMETHODIMP CAutoUtility::FindFile (BSTR Name, BSTR * FullPath)
	{
	if (FullPath == NULL)
		return E_POINTER;
	
	CString name(Name);
	char str[MAX_PATH];
	HRESULT hr = m_pDoc->AUTO_findfile (name, str);
	if (FAILED (hr))
		return hr;

	CString Str(str);
	*FullPath = Str.AllocSysString();

	return NOERROR;
	}

STDMETHODIMP CAutoUtility::GetAngle (IIcadPoint * Point, BSTR Prompt, double * pAngle)
	{
	if (pAngle == NULL)
		return E_POINTER;

	sds_point pt;
	HRESULT hr;

	CString prompt (Prompt);
	char *msg;
	if (prompt.IsEmpty())
		msg = NULL;
	else
		msg = prompt.GetBuffer(0);

	if (Point)
		{
		Point->get_x (&pt[0]);
		Point->get_y (&pt[1]);
		Point->get_z (&pt[2]);
		
		hr = m_pDoc->AUTO_GetAngDistOrient (pt, msg, pAngle, (enum getangdist_types_enum)1);
		}
	else
		{
		hr = m_pDoc->AUTO_GetAngDistOrient (NULL, msg, pAngle, (enum getangdist_types_enum)1);
		}

	if (hr == E_INVALIDARG)	//this is be due to a keyword entry
		hr = Error(_T("User input is a keyword"),IID_IIcadUtility, ICAD_E_USER_ENTERED_KEYWORD);

	return hr;
	}

STDMETHODIMP CAutoUtility::GetCorner (IIcadPoint * Point, BSTR Prompt, IIcadPoint * * ppCorner)
	{
	if (ppCorner == NULL)
		return E_POINTER;
	
	if (Point == NULL)
		return E_INVALIDARG;

	HRESULT hr;
	sds_point pt, corner;
	Point->get_x (&pt[0]);
	Point->get_y (&pt[1]);
	Point->get_z (&pt[2]);
	CString prompt (Prompt);
	char *msg;
	if (prompt.IsEmpty())
		msg = NULL;
	else
		msg = prompt.GetBuffer(0);

	hr = m_pDoc->AUTO_getcorner( pt, msg, corner, false );

	if (hr == E_INVALIDARG)	//this is be due to a keyword entry
		hr = Error(_T("User input is a keyword"),IID_IIcadUtility, ICAD_E_USER_ENTERED_KEYWORD);

	if (FAILED(hr))
		return E_FAIL;

	CComObject<CAutoPoint> *pPoint;
	CComObject<CAutoPoint>::CreateInstance(&pPoint);
	if (!pPoint)
		return E_FAIL;
	
	pPoint->Init(NULL, NULL, corner[0], corner[1], corner[2]);
	
	return pPoint->QueryInterface (ppCorner);
	}

STDMETHODIMP CAutoUtility::GetDistance (IIcadPoint * Point, BSTR Prompt, double * pDistance)
	{
	if (pDistance == NULL)
		return E_POINTER;
	
	sds_point pt;
	HRESULT hr;

	CString prompt (Prompt);
	char *msg;
	if (prompt.IsEmpty())
		msg = NULL;
	else
		msg = prompt.GetBuffer(0);

	if (Point)
		{
		Point->get_x (&pt[0]);
		Point->get_y (&pt[1]);
		Point->get_z (&pt[2]);
		
		hr = m_pDoc->AUTO_GetAngDistOrient (pt, msg, pDistance, (enum getangdist_types_enum)0);
		}
	else
		{
		hr = m_pDoc->AUTO_GetAngDistOrient (NULL, msg, pDistance, (enum getangdist_types_enum)0);
		}

	if (hr == E_INVALIDARG)	//this is be due to a keyword entry
		hr = Error(_T("User input is a keyword"),IID_IIcadUtility, ICAD_E_USER_ENTERED_KEYWORD);

	return hr;
	}

STDMETHODIMP CAutoUtility::GetEntity (IDispatch * * Object, IIcadPoint * * PickedPoint, BSTR Prompt)
	{
	if (Object == NULL)
		return E_POINTER;
	
	if (PickedPoint == NULL)
		return E_POINTER;

	CString prompt (Prompt);
	char *msg;
	if (prompt.IsEmpty())
		msg = NULL;
	else
		msg = prompt.GetBuffer(0);

	sds_point pt;
	sds_name ename;

	HRESULT hr = m_pDoc->AUTO_entsel (msg, ename, pt);

	if (hr == E_INVALIDARG)	//this is be due to a keyword entry
		hr = Error(_T("User input is a keyword"),IID_IIcadUtility, ICAD_E_USER_ENTERED_KEYWORD);

	if (FAILED(hr))
		return hr;

	CComObject<CAutoPoint> *pPoint;
	CComObject<CAutoPoint>::CreateInstance(&pPoint);
	if (!pPoint)
		return E_FAIL;
	
	pPoint->Init(NULL, NULL, pt[0], pt[1], pt[2]);
	
	pPoint->QueryInterface (PickedPoint);

	db_handitem* pDbHandItem;
	pDbHandItem = (db_handitem*)ename[0];

	//unhighlight the selected item
	m_pDoc->RedrawEntity (pDbHandItem, IC_REDRAW_UNHILITE);

	CComPtr<IIcadEntity> pEnt;
	hr = CreateEntityWrapper (m_pDoc, NULL, pDbHandItem, &pEnt);
	if (FAILED(hr))
		return hr;

	return pEnt->QueryInterface (Object);
	}

STDMETHODIMP CAutoUtility::GetSubEntity (IDispatch * * Object, IIcadPoint * * PickedPoint, IIcadMatrix * * TransMatrix, VARIANT * ContextData, BSTR Prompt)
	{
	if (Object == NULL)
		return E_POINTER;
	
	if (PickedPoint == NULL)
		return E_POINTER;
	
	CString prompt (Prompt);
	char *msg;
	if (prompt.IsEmpty())
		msg = NULL;
	else
		msg = prompt.GetBuffer(0);

	sds_point pt;
	sds_name ename;
	struct resbuf *rb;
	sds_point matrix[4];

	HRESULT hr = m_pDoc->AUTO_nentsel (msg, ename, pt, matrix, &rb);

	if (hr == E_INVALIDARG)	//this is be due to a keyword entry
		hr = Error(_T("User input is a keyword"),IID_IIcadUtility, ICAD_E_USER_ENTERED_KEYWORD);

	if (FAILED(hr))
		return E_FAIL;

	//TODO convert the rb to Variants
	//*ContextData = 

	CComObject<CAutoMatrix> *pMatrix;
	CComObject<CAutoMatrix>::CreateInstance(&pMatrix);
	if (!pMatrix)
		return E_FAIL;
	
	pMatrix->Init(NULL);
	int i;
	for (i = 0; i < 3; i++)
		{
		pMatrix->SetValue (i, 0, matrix[0][i]);
		pMatrix->SetValue (i, 1, matrix[1][i]);
		pMatrix->SetValue (i, 2, matrix[2][i]);
		pMatrix->SetValue (i, 3, matrix[3][i]);
		}
	pMatrix->SetValue (3, 0, 0.0);
	pMatrix->SetValue (3, 1, 0.0);
	pMatrix->SetValue (3, 2, 0.0);
	pMatrix->SetValue (3, 3, 1.0);
	
	pMatrix->QueryInterface (TransMatrix);

	CComObject<CAutoPoint> *pPoint;
	CComObject<CAutoPoint>::CreateInstance(&pPoint);
	if (!pPoint)
		return E_FAIL;
	
	pPoint->Init(NULL, NULL, pt[0], pt[1], pt[2]);
	
	pPoint->QueryInterface (PickedPoint);

	db_handitem* pDbHandItem;
	pDbHandItem = (db_handitem*)ename[0];

	//unhighlight the selected item
	m_pDoc->RedrawEntity (pDbHandItem, IC_REDRAW_UNHILITE);

	CComPtr<IIcadEntity> pEnt;
	hr = CreateEntityWrapper (m_pDoc, NULL, pDbHandItem, &pEnt);
	if (FAILED(hr))
		return hr;

	return pEnt->QueryInterface (Object);
	}

STDMETHODIMP CAutoUtility::GetInput (BSTR * InputString)
	{
	if (InputString == NULL)
		return E_POINTER;
	
	char str[IC_ACADBUF];
	HRESULT hr = m_pDoc->AUTO_getinput (str);
	if (FAILED (hr))
		return hr;

	CString Str(str);
	*InputString = Str.AllocSysString();

	return NOERROR;
	}

STDMETHODIMP CAutoUtility::GetInteger (BSTR Prompt, long * Value)
	{
	if (Value == NULL)
		return E_POINTER;
	
	CString prompt (Prompt);
	char *msg;
	if (prompt.IsEmpty())
		msg = NULL;
	else
		msg = prompt.GetBuffer(0);

	HRESULT hr = m_pDoc->AUTO_getint(msg, (int *)Value);

	if (hr == E_INVALIDARG)	//this is be due to a keyword entry
		hr = Error(_T("User input is a keyword"),IID_IIcadUtility, ICAD_E_USER_ENTERED_KEYWORD);
	return hr;
	}

STDMETHODIMP CAutoUtility::GetOrientation (IIcadPoint * Point, BSTR Prompt, double * pOrientation)
	{
	if (pOrientation == NULL)
		return E_POINTER;
	
	sds_point pt;
	HRESULT hr;

	CString prompt (Prompt);
	char *msg;
	if (prompt.IsEmpty())
		msg = NULL;
	else
		msg = prompt.GetBuffer(0);

	if (Point)
		{
		Point->get_x (&pt[0]);
		Point->get_y (&pt[1]);
		Point->get_z (&pt[2]);
		
		hr = m_pDoc->AUTO_GetAngDistOrient (pt, msg, pOrientation, (enum getangdist_types_enum)2);
		}
	else
		{
		hr = m_pDoc->AUTO_GetAngDistOrient (NULL, msg, pOrientation, (enum getangdist_types_enum)2);
		}

	if (hr == E_INVALIDARG)	//this is be due to a keyword entry
		hr = Error(_T("User input is a keyword"),IID_IIcadUtility, ICAD_E_USER_ENTERED_KEYWORD);
	return hr;
	}

STDMETHODIMP CAutoUtility::GetPoint (IIcadPoint * Point, BSTR Prompt, IIcadPoint * * ppPoint)
	{
	if (ppPoint == NULL)
		return E_POINTER;
	
	HRESULT hr, ret = S_OK;
	sds_point pt, pt2;
	pt2[0] = pt2[1] = pt2[2] = 0.0;

	CString prompt (Prompt);
	char *msg;
	if (prompt.IsEmpty())
		msg = NULL;
	else
		msg = prompt.GetBuffer(0);

	if (Point)
		{
		Point->get_x (&pt[0]);
		Point->get_y (&pt[1]);
		Point->get_z (&pt[2]);
		
		hr = m_pDoc->AUTO_getpoint (pt, msg, pt2);
		}
	else
		{
		hr = m_pDoc->AUTO_getpoint (NULL, msg, pt2);
		}

	if (hr == E_INVALIDARG)	//this is be due to a keyword entry
		hr = Error(_T("User input is a keyword"),IID_IIcadUtility, ICAD_E_USER_ENTERED_KEYWORD);

	if (FAILED (hr))
		return E_FAIL;

	CComObject<CAutoPoint> *pPoint;
	CComObject<CAutoPoint>::CreateInstance(&pPoint);
	if (!pPoint)
		return E_FAIL;
	
	pPoint->Init(NULL, NULL, pt2[0], pt2[1], pt2[2]);
	
	return pPoint->QueryInterface (ppPoint);
	}

STDMETHODIMP CAutoUtility::GetReal (BSTR Prompt, double * Value)
	{
	if (Value == NULL)
		return E_POINTER;
	
	CString prompt (Prompt);
	char *msg;
	if (prompt.IsEmpty())
		msg = NULL;
	else
		msg = prompt.GetBuffer(0);

	HRESULT hr = m_pDoc->AUTO_getreal (msg, Value);

	if (hr == E_INVALIDARG)	//this is be due to a keyword entry
		hr = Error(_T("User input is a keyword"),IID_IIcadUtility, ICAD_E_USER_ENTERED_KEYWORD);
	return hr;
	}

STDMETHODIMP CAutoUtility::GetString (VARIANT_BOOL HasSpaces, BSTR Prompt, BSTR * String)
	{
	if (String == NULL)
		return E_POINTER;
	
	CString prompt (Prompt);
	char *msg;
	if (prompt.IsEmpty())
		msg = NULL;
	else
		msg = prompt.GetBuffer(0);

	char result[IC_ACADBUF];
	int swSpaces = (HasSpaces == VARIANT_TRUE) ? 1 : 0;

	HRESULT hr = m_pDoc->AUTO_getstring( swSpaces, msg, result);

	if (hr == E_INVALIDARG)	//this is be due to a keyword entry
		hr = Error(_T("User input is a keyword"),IID_IIcadUtility, ICAD_E_USER_ENTERED_KEYWORD);
	
	if (FAILED(hr))
		return E_FAIL;

	CString res(result);
	*String = res.AllocSysString();

	return NOERROR;
	}

STDMETHODIMP CAutoUtility::TranslateCoordinates (IIcadPoint * OriginPoint, CoordinateSystemTypes From, CoordinateSystemTypes To, VARIANT_BOOL Disp, IIcadPoint **ppPoint)
	{
	if (OriginPoint == NULL)
		return E_INVALIDARG;

	struct resbuf from, to;
	from.restype = RTSHORT;
	from.resval.rint = (short) From;
	to.restype = RTSHORT;
	to.resval.rint = (short) To;

	int disp = (Disp == VARIANT_TRUE) ? 1 : 0;

	sds_point origin, pt;
	OriginPoint->get_x (&origin[0]);
	OriginPoint->get_y (&origin[1]);
	OriginPoint->get_z (&origin[2]);

	if (FAILED (m_pDoc->AUTO_trans (origin, &from, &to, disp, pt)))
		return E_FAIL;

	CComObject<CAutoPoint> *pPoint;
	CComObject<CAutoPoint>::CreateInstance(&pPoint);
	if (!pPoint)
		return E_FAIL;
	
	pPoint->Init(NULL, NULL, pt[0], pt[1], pt[2]);
	
	return pPoint->QueryInterface (ppPoint);
	}

STDMETHODIMP CAutoUtility::InitializeUserInput (UserInputType Bits, VARIANT Keywords)
	{
	HRESULT hr;
	CString str;

	if (Bits == 16)	//reserved keyword
		return E_INVALIDARG;
	if (Bits < 0 || Bits > 256)
		return E_INVALIDARG;

	if(V_VT(&Keywords) == (VT_BSTR))
		{
		str = (V_BSTR(&Keywords));
		}
    else if(V_VT(&Keywords) == (VT_ARRAY | VT_BSTR)) //ensure it is an array
		{
		long LBound, UBound;
		hr = SafeArrayGetLBound(V_ARRAY(&Keywords), 1, &LBound);
		if (FAILED(hr))
			return E_INVALIDARG;
		
		hr = SafeArrayGetUBound(V_ARRAY(&Keywords), 1, &UBound);
		if (FAILED(hr))
			return E_INVALIDARG;
		
		long i;
		BSTR bstr;

		str.Empty();
		for (i = LBound; i <= UBound; i++)
			{
			hr = SafeArrayGetElement(V_ARRAY(&Keywords), &i, &bstr);
			if (FAILED(hr))
				continue;

			CString temp(bstr);
			str = str + temp + " ";
			}
		}

	return m_pDoc->AUTO_initget( Bits, (LPCSTR) str);
	}

STDMETHODIMP CAutoUtility::GetKeyword (BSTR Prompt, BSTR *String)
	{
	if (String == NULL)
		return E_INVALIDARG;

	char str[IC_ACADBUF];
	CString prompt (Prompt);
	char *msg;
	if (prompt.IsEmpty())
		msg = NULL;
	else
		msg = prompt.GetBuffer(0);

	HRESULT hr = m_pDoc->AUTO_getkword (msg, str);

	if (hr == E_INVALIDARG)	//this is be due to a keyword entry
		hr = Error(_T("User input is a keyword"),IID_IIcadUtility, ICAD_E_USER_ENTERED_KEYWORD);
	
	if (FAILED(hr))
		return E_FAIL;

	CString Str(str);
	*String = Str.AllocSysString();

	return NOERROR;
	}

STDMETHODIMP CAutoUtility::PolarPoint (IIcadPoint * Point, double Angle, double Distance, IIcadPoint * * ppPoint)
	{
	if (ppPoint == NULL)
		return E_POINTER;
	
	if (Point == NULL)
		return E_INVALIDARG;

	sds_point pt1, pt2;
	Point->get_x (&pt1[0]);
	Point->get_y (&pt1[1]);
	Point->get_z (&pt1[2]);

	if (FAILED (m_pDoc->AUTO_polar (pt1, Angle, Distance, pt2)))
		return E_FAIL;

	CComObject<CAutoPoint> *pPoint;
	CComObject<CAutoPoint>::CreateInstance(&pPoint);
	if (!pPoint)
		return E_FAIL;
	
	pPoint->Init(NULL, NULL, pt2[0], pt2[1], pt2[2]);
	
	return pPoint->QueryInterface (ppPoint);
	}

STDMETHODIMP CAutoUtility::PaintLine (IIcadPoint * StartPoint, IIcadPoint * EndPoint, Colors Color, VARIANT_BOOL HighlightFlag)
	{
	if (StartPoint == NULL)
		return E_INVALIDARG;
	
	if (EndPoint == NULL)
		return E_INVALIDARG;

	sds_point pt1, pt2;
	StartPoint->get_x (&pt1[0]);
	StartPoint->get_y (&pt1[1]);
	StartPoint->get_z (&pt1[2]);
	EndPoint->get_x (&pt2[0]);
	EndPoint->get_y (&pt2[1]);
	EndPoint->get_z (&pt2[2]);
		
	int swHighlight = (HighlightFlag == VARIANT_TRUE) ? 1 : 0;

	return m_pDoc->AUTO_grdraw(pt1, pt2, Color, swHighlight); 
	}

STDMETHODIMP CAutoUtility::ShowGraphicScreen ()
	{
	return m_pDoc->AUTO_graphscr();
	}

STDMETHODIMP CAutoUtility::ShowTextScreen ()
	{
	return m_pDoc->AUTO_textscr();
	}

STDMETHODIMP CAutoUtility::WildCardMatch (BSTR Pattern, BSTR String, VARIANT_BOOL * Match)
	{
	if (Match == NULL)
		return E_POINTER;
	
	CString pattern(Pattern);
	CString str(String);
	if (FAILED (m_pDoc->AUTO_wcmatch (pattern, str)))
		*Match = VARIANT_FALSE;
	else
		*Match = VARIANT_TRUE;

	return NOERROR;
	}



