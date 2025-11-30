/* INTELLICAD\PRJ\LIB\AUTO\AUTOATTRIBUTE.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Implementation of CAutoAttribute - the Automation Attribute object
 * 
 */ 

#include "StdAfx.h"
#include "AutoAttribute.h"
#include "AutoPoint.h"
#include "AutoVector.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoAttribute

HRESULT CAutoAttribute::Init(CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem)
	{
	CAutoDbItem::Init(pDoc, pParent);
	
	if (pDbHandItem)
		m_pDbHandItem = pDbHandItem;
	else
		{
		if (pDoc)
			m_pDbHandItem = new db_attrib(pDoc->m_dbDrawing);
		else
			m_pDbHandItem = new db_attrib();
		}
	
	if (NULL == m_pDbHandItem) 
		return FALSE;
	
	return NOERROR;
	}

STDMETHODIMP CAutoAttribute::get_Normal (IIcadVector * * ppExtrusionDirection)
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

STDMETHODIMP CAutoAttribute::put_Normal (IIcadVector * ExtrusionDirection)
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

STDMETHODIMP CAutoAttribute::get_FieldLength (int * pFieldLength)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pFieldLength == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_73(pFieldLength)) 
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoAttribute::put_FieldLength (int pFieldLength)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pFieldLength < 0 || pFieldLength > MAXCHAR)
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_73(pFieldLength))
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoAttribute::get_Height (double * pHeight)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pHeight == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_40(pHeight)) 
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoAttribute::put_Height (double pHeight)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pHeight < 0.0)
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_40(pHeight))
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoAttribute::get_HorizontalAlignment (HorizontalAlignment * pHorizAlign)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pHorizAlign == NULL)
		return E_POINTER;
	
	int nVal;
	if (!m_pDbHandItem->get_72(&nVal)) 
		return E_FAIL;

	*pHorizAlign = (HorizontalAlignment)nVal;
	return NOERROR;
	}

STDMETHODIMP CAutoAttribute::put_HorizontalAlignment (HorizontalAlignment pHorizAlign)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if ((pHorizAlign < vicHorizontalAlignmentLeft) || (pHorizAlign > vicHorizontalAlignmentFit))
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_72(pHorizAlign)) 
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoAttribute::get_InsertionPoint (IIcadPoint * * ppInsertPoint)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppInsertPoint == NULL)
		return E_POINTER;
	
	sds_point pt;
	m_pDbHandItem->get_10(pt);
	
	CComObject<CAutoPoint> *pPoint;
	CComObject<CAutoPoint>::CreateInstance(&pPoint);
	if (!pPoint)
		return E_FAIL;
	
	pPoint->Init(NULL, NULL, pt[0], pt[1], pt[2]);
	
	return pPoint->QueryInterface (ppInsertPoint);
	}

STDMETHODIMP CAutoAttribute::put_InsertionPoint (IIcadPoint * ppInsertPoint)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	sds_point pt;
	pt[0] = pt[1] = pt[2] = 0.0;
	if (ppInsertPoint)	
		{
		ppInsertPoint->get_x(&pt[0]);
		ppInsertPoint->get_y(&pt[1]);
		ppInsertPoint->get_z(&pt[2]);
		}
	m_pDbHandItem->set_10(pt);
	
	return NOERROR;
	}

STDMETHODIMP CAutoAttribute::get_Constant (VARIANT_BOOL * Mode)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (Mode == NULL)
		return E_POINTER;
	
	int nVal;
	if (!m_pDbHandItem->get_70(&nVal)) 
		return E_FAIL;

	*Mode = (nVal & IC_ATTRIB_CONST) ? VARIANT_TRUE : VARIANT_FALSE;

	return NOERROR;
	}

STDMETHODIMP CAutoAttribute::put_Constant (VARIANT_BOOL Mode)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	int flags  = 0;
	m_pDbHandItem->get_70 (&flags);
	
	if (Mode == VARIANT_TRUE) 
		flags |= IC_ATTRIB_CONST;
	else
		flags &= ~IC_ATTRIB_CONST;
	
	m_pDbHandItem->set_70 (flags);
	return NOERROR;
	}

STDMETHODIMP CAutoAttribute::get_Invisible (VARIANT_BOOL * Mode)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (Mode == NULL)
		return E_POINTER;
	
	int nVal;
	if (!m_pDbHandItem->get_70(&nVal)) 
		return E_FAIL;

	*Mode = (nVal & IC_ATTRIB_INVIS) ? VARIANT_TRUE : VARIANT_FALSE;

	return NOERROR;
	}

STDMETHODIMP CAutoAttribute::put_Invisible (VARIANT_BOOL Mode)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	int flags  = 0;
	m_pDbHandItem->get_70 (&flags);
	
	if (Mode == VARIANT_TRUE) 
		flags |= IC_ATTRIB_INVIS;
	else
		flags &= ~IC_ATTRIB_INVIS;
	
	m_pDbHandItem->set_70 (flags);
	return NOERROR;
	}

STDMETHODIMP CAutoAttribute::get_Preset (VARIANT_BOOL * Mode)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (Mode == NULL)
		return E_POINTER;
	
	int nVal;
	if (!m_pDbHandItem->get_70(&nVal)) 
		return E_FAIL;

	*Mode = (nVal & IC_ATTRIB_PRESET) ? VARIANT_TRUE : VARIANT_FALSE;

	return NOERROR;
	}

STDMETHODIMP CAutoAttribute::put_Preset (VARIANT_BOOL Mode)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	int flags  = 0;
	m_pDbHandItem->get_70 (&flags);
	
	if (Mode == VARIANT_TRUE) 
		flags |= IC_ATTRIB_PRESET;
	else
		flags &= ~IC_ATTRIB_PRESET;
	
	m_pDbHandItem->set_70 (flags);
	return NOERROR;
	}

STDMETHODIMP CAutoAttribute::get_Verify (VARIANT_BOOL * Mode)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (Mode == NULL)
		return E_POINTER;
	
	int nVal;
	if (!m_pDbHandItem->get_70(&nVal)) 
		return E_FAIL;

	*Mode = (nVal & IC_ATTRIB_VERIFY) ? VARIANT_TRUE : VARIANT_FALSE;

	return NOERROR;
	}

STDMETHODIMP CAutoAttribute::put_Verify (VARIANT_BOOL Mode)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	int flags  = 0;
	m_pDbHandItem->get_70 (&flags);
	
	if (Mode == VARIANT_TRUE) 
		flags |= IC_ATTRIB_VERIFY;
	else
		flags &= ~IC_ATTRIB_VERIFY;
	
	m_pDbHandItem->set_70 (flags);
	return NOERROR;
	}

STDMETHODIMP CAutoAttribute::get_ObliqueAngle (double * pAngle)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pAngle == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_51(pAngle)) 
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoAttribute::put_ObliqueAngle (double pAngle)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (!m_pDbHandItem->set_51(pAngle))
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoAttribute::get_Rotation (double * pRotation)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pRotation == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_50(pRotation)) 
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoAttribute::put_Rotation (double pRotation)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (!m_pDbHandItem->set_50(pRotation))
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoAttribute::get_ScaleFactor (double * pScaleFactor)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pScaleFactor == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_41(pScaleFactor)) 
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoAttribute::put_ScaleFactor (double pScaleFactor)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pScaleFactor <= 0.0)
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_41(pScaleFactor))
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoAttribute::get_StyleName (BSTR * pStyleName)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pStyleName == NULL)
		return E_POINTER;
	
	char* pStr;
	if (!m_pDbHandItem->get_7(&pStr)) 
		return E_FAIL;

	CString str(pStr);
	*pStyleName = str.AllocSysString();

	return NOERROR;
	}

STDMETHODIMP CAutoAttribute::put_StyleName (BSTR pStyleName)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	CString str(pStyleName);
	if (str.IsEmpty ())
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_7(str.GetBuffer(0), m_pDoc->m_dbDrawing)) 
		return E_FAIL;
	
	return NOERROR;
	}

STDMETHODIMP CAutoAttribute::get_TagString (BSTR * pTag)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pTag == NULL)
		return E_POINTER;
	
	char* pStr;
	if (!m_pDbHandItem->get_2(&pStr)) 
		return E_FAIL;

	CString str(pStr);
	*pTag = str.AllocSysString();

	return NOERROR;
	}

STDMETHODIMP CAutoAttribute::put_TagString (BSTR pTag)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	CString str(pTag);

	if (!m_pDbHandItem->set_2(str.GetBuffer(0))) 
		return E_FAIL;
	
	return NOERROR;
	}

STDMETHODIMP CAutoAttribute::get_Thickness (double * pThick)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pThick == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_39(pThick))
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoAttribute::put_Thickness (double pThick)
	{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (pThick < 0.0)
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_39(pThick))
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoAttribute::get_TextAlignmentPoint (IIcadPoint * * ppPoint)
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

STDMETHODIMP CAutoAttribute::put_TextAlignmentPoint (IIcadPoint * ppPoint)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	sds_point pt;
	pt[0] = pt[1] = pt[2] = 0.0;
	if (ppPoint)	//this is an optional arg
		{
		ppPoint->get_x(&pt[0]);
		ppPoint->get_y(&pt[1]);
		ppPoint->get_z(&pt[2]);
		}
	m_pDbHandItem->set_11(pt);
	
	return NOERROR;
	}

STDMETHODIMP CAutoAttribute::get_TextGenerationFlag (TextFlag * pFlags)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pFlags == NULL)
		return E_POINTER;
	
	int nVal;
	if (!m_pDbHandItem->get_71(&nVal)) 
		return E_FAIL;

	*pFlags = (TextFlag)nVal;

	return NOERROR;
	}

STDMETHODIMP CAutoAttribute::put_TextGenerationFlag (TextFlag pFlags)
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

STDMETHODIMP CAutoAttribute::get_TextString (BSTR * pText)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pText == NULL)
		return E_POINTER;
	
	char* pStr;
	if (!m_pDbHandItem->get_1(&pStr)) 
		return E_FAIL;

	CString str(pStr);
	*pText = str.AllocSysString();

	return NOERROR;
	}

STDMETHODIMP CAutoAttribute::put_TextString (BSTR pText)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	CString str(pText);

	if (!m_pDbHandItem->set_1(str.GetBuffer(0))) 
		return E_FAIL;
	
	return NOERROR;
	}

STDMETHODIMP CAutoAttribute::get_VerticalAlignment (VerticalAlignment * pVertAlign)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pVertAlign == NULL)
		return E_POINTER;
	
	int nVal;
	if (!m_pDbHandItem->get_73(&nVal)) 
		return E_FAIL;

	*pVertAlign = (VerticalAlignment)nVal;
	return NOERROR;
	}

STDMETHODIMP CAutoAttribute::put_VerticalAlignment (VerticalAlignment pVertAlign)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
    if ((pVertAlign < vicVerticalAlignmentBaseline) || (pVertAlign > vicVerticalAlignmentTop))
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_73(pVertAlign)) 
		return E_FAIL;

	return NOERROR;
	}



