/* INTELLICAD\PRJ\LIB\AUTO\AUTOTEXT.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Implementation of CAutoText - the Automation Text object
 * 
 */ 

#include "StdAfx.h"
#include "AutoText.h"
#include "AutoPoint.h"
#include "AutoVector.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoText

HRESULT CAutoText::Init(CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem)
	{
	CAutoDbItem::Init(pDoc, pParent);
	
	if (pDbHandItem)
		m_pDbHandItem = pDbHandItem;
	else
		{
		if (pDoc)
			m_pDbHandItem = new db_text(pDoc->m_dbDrawing);
		else
			m_pDbHandItem = new db_text();
		}
	
	if (NULL == m_pDbHandItem) 
		return FALSE;
	
	return NOERROR;
	}

STDMETHODIMP CAutoText::get_TextAlignmentPoint (IIcadPoint * * ppPoint)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppPoint == NULL)
		return E_POINTER;
	
	sds_point pt;
	m_pDbHandItem->get_11(pt);
	
	CComObject<CAutoPoint> *pPoint;
	CComObject<CAutoPoint>::CreateInstance(&pPoint);
	if (!pPoint)
		return E_FAIL;
	
	pPoint->Init(NULL, NULL, pt[0], pt[1], pt[2]);
	
	return pPoint->QueryInterface (ppPoint);
	}

STDMETHODIMP CAutoText::put_TextAlignmentPoint (IIcadPoint * pPoint)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	sds_point pt;
	pt[0] = pt[1] = pt[2] = 0.0;
	if (pPoint)	//this is an optional arg
		{
		pPoint->get_x(&pt[0]);
		pPoint->get_y(&pt[1]);
		pPoint->get_z(&pt[2]);
		}
	m_pDbHandItem->set_11(pt);
	
	return NOERROR;
	}

STDMETHODIMP CAutoText::get_Normal (IIcadVector * * ppExtrusionDirection)
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

STDMETHODIMP CAutoText::put_Normal (IIcadVector * ExtrusionDirection)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	sds_point v;
	v[0] = v[1] = 0.0; v[2] = 1.0;
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

STDMETHODIMP CAutoText::get_TextGenerationFlag (TextFlag * pFlags)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pFlags == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_71((int *)pFlags)) 
		return E_FAIL;
	
	return NOERROR;
	}

STDMETHODIMP CAutoText::put_TextGenerationFlag (TextFlag pFlags)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
    switch (pFlags)
		{
		case vicTextFlagNormal:		//0x0
		case vicTextFlagBackward:	//0x2
		case vicTextFlagUpsideDown:	//0x4
		case (vicTextFlagBackward + vicTextFlagUpsideDown):
			break;
		default:
			return E_INVALIDARG;
		}

	if (!m_pDbHandItem->set_71(pFlags)) 
		return E_FAIL;
	
	return NOERROR;
	}

STDMETHODIMP CAutoText::get_Height (double * pHeight)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pHeight == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_40(pHeight)) 
		return E_FAIL;
	
	return NOERROR;
	}

STDMETHODIMP CAutoText::put_Height (double pHeight)
	{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (pHeight < 0.0)
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_40(pHeight)) 
		return E_FAIL;
	
	return NOERROR;
	}

STDMETHODIMP CAutoText::get_HorizontalAlignment (HorizontalAlignment * pHorizAlign)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pHorizAlign == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_72((int *)pHorizAlign)) 
		return E_FAIL;
	
	return NOERROR;
	}

STDMETHODIMP CAutoText::put_HorizontalAlignment (HorizontalAlignment pHorizAlign)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if ((pHorizAlign < vicHorizontalAlignmentLeft) || (pHorizAlign > vicHorizontalAlignmentFit))
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_72(pHorizAlign)) 
		return E_FAIL;
	
	return NOERROR;
	}

STDMETHODIMP CAutoText::get_InsertionPoint (IIcadPoint * * ppInsPoint)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppInsPoint == NULL)
		return E_POINTER;
	
	sds_point pt;
	m_pDbHandItem->get_10(pt);
	
	CComObject<CAutoPoint> *pPoint;
	CComObject<CAutoPoint>::CreateInstance(&pPoint);
	if (!pPoint)
		return E_FAIL;
	
	pPoint->Init(NULL, NULL, pt[0], pt[1], pt[2]);
	
	return pPoint->QueryInterface (ppInsPoint);
	}

STDMETHODIMP CAutoText::put_InsertionPoint (IIcadPoint * ppInsPoint)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	sds_point pt;
	pt[0] = pt[1] = pt[2] = 0.0;
	if (ppInsPoint)	//this is an optional arg
		{
		ppInsPoint->get_x(&pt[0]);
		ppInsPoint->get_y(&pt[1]);
		ppInsPoint->get_z(&pt[2]);
		}
	m_pDbHandItem->set_10(pt);
	
	return NOERROR;
	}

STDMETHODIMP CAutoText::get_ObliqueAngle (double * pAngle)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pAngle == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_51(pAngle)) 
		return E_FAIL;
	
	return NOERROR;
	}

STDMETHODIMP CAutoText::put_ObliqueAngle (double pAngle)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (!m_pDbHandItem->set_51(pAngle)) 
		return E_FAIL;
	
	return NOERROR;
	}

STDMETHODIMP CAutoText::get_Rotation (double * pRotation)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pRotation == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_50(pRotation)) 
		return E_FAIL;
	
	return NOERROR;
	}

STDMETHODIMP CAutoText::put_Rotation (double pRotation)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (!m_pDbHandItem->set_50(pRotation)) 
		return E_FAIL;
	
	return NOERROR;
	}

STDMETHODIMP CAutoText::get_ScaleFactor (double * pScale)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pScale == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_41(pScale)) 
		return E_FAIL;
	
	return NOERROR;
	}

STDMETHODIMP CAutoText::put_ScaleFactor (double pScale)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pScale <= 0.0)
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_41(pScale)) 
		return E_FAIL;
	
	return NOERROR;
	}

STDMETHODIMP CAutoText::get_StyleName (BSTR * pStyleName)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pStyleName == NULL)
		return E_POINTER;
	
	char* pStr;
	m_pDbHandItem->get_7 (&pStr);
	
	CString str (pStr);
	*pStyleName = str.AllocSysString();
	return NOERROR;
	}

STDMETHODIMP CAutoText::put_StyleName (BSTR pStyleName)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	
	CString str(pStyleName);
	if (str.IsEmpty ())
		return E_INVALIDARG;

	if ( !m_pDbHandItem->set_7 ((char*)str.GetBuffer(0), m_pDoc->m_dbDrawing)) 
		return E_FAIL;
	return NOERROR;
	}

STDMETHODIMP CAutoText::get_TextString (BSTR * pText)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pText == NULL)
		return E_POINTER;
	
	char* pStr;
	m_pDbHandItem->get_1 (&pStr);
	
	CString str (pStr);
	*pText = str.AllocSysString();
	return NOERROR;
	}

STDMETHODIMP CAutoText::put_TextString (BSTR pText)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	CString str(pText);
	if ( !m_pDbHandItem->set_1 (str.GetBuffer(0))) 
		return ResultFromScode(E_FAIL);
	return NOERROR;
	}

STDMETHODIMP CAutoText::get_Thickness (double * pThick)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pThick == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_39(pThick)) 
		return E_FAIL;
	
	return NOERROR;
	}

STDMETHODIMP CAutoText::put_Thickness (double pThick)
	{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (pThick < 0.0)
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_39(pThick)) 
		return E_FAIL;
	
	return NOERROR;
	}

STDMETHODIMP CAutoText::get_VerticalAlignment (VerticalAlignment * pVertAlign)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pVertAlign == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_73((int *)pVertAlign)) 
		return E_FAIL;
	
	return NOERROR;
	}

STDMETHODIMP CAutoText::put_VerticalAlignment (VerticalAlignment pVertAlign)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
    if ((pVertAlign < vicVerticalAlignmentBaseline) || (pVertAlign > vicVerticalAlignmentTop))
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_73(pVertAlign)) 
		return E_FAIL;
	
	return NOERROR;
	}


