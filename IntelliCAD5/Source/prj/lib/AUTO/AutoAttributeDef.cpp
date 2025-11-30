/* INTELLICAD\PRJ\LIB\AUTO\AUTOATTRIBUTEDEF.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Implementation of CAutoAttributeDef - the Automation Attribute Definition object
 * 
 */ 

#include "StdAfx.h"
#include "AutoAttributeDef.h"
#include "AutoPoint.h"
#include "AutoVector.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoAttributeDef

HRESULT CAutoAttributeDef::Init(CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem)
	{
	CAutoDbItem::Init(pDoc, pParent);
	
	if (pDbHandItem)
		m_pDbHandItem = pDbHandItem;
	else
		{
		if (pDoc)
			m_pDbHandItem = new db_attdef(pDoc->m_dbDrawing);
		else
			m_pDbHandItem = new db_attdef();
		}
	
	if (NULL == m_pDbHandItem) 
		return FALSE;
	
	return NOERROR;
	}

STDMETHODIMP CAutoAttributeDef::get_Normal (IIcadVector * * ppExtrusionDirection)
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

STDMETHODIMP CAutoAttributeDef::put_Normal (IIcadVector * ExtrusionDirection)
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

STDMETHODIMP CAutoAttributeDef::get_FieldLength (int * pFieldLength)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pFieldLength == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_73(pFieldLength)) 
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoAttributeDef::put_FieldLength (int pFieldLength)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pFieldLength < 0 || pFieldLength > MAXCHAR)
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_73(pFieldLength))
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoAttributeDef::get_Height (double * pHeight)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pHeight == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_40(pHeight)) 
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoAttributeDef::put_Height (double pHeight)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pHeight < 0.0)
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_40(pHeight))
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoAttributeDef::get_HorizontalAlignment (HorizontalAlignment * pHorizAlign)
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

STDMETHODIMP CAutoAttributeDef::put_HorizontalAlignment (HorizontalAlignment pHorizAlign)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if ((pHorizAlign < vicHorizontalAlignmentLeft) || (pHorizAlign > vicHorizontalAlignmentFit))
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_72(pHorizAlign)) 
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoAttributeDef::get_InsertionPoint (IIcadPoint * * ppInsertPoint)
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

STDMETHODIMP CAutoAttributeDef::put_InsertionPoint (IIcadPoint * ppInsertPoint)
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

STDMETHODIMP CAutoAttributeDef::get_Mode (AttributeMode * pMode)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pMode == NULL)
		return E_POINTER;
	
	int nVal;
	if (!m_pDbHandItem->get_70(&nVal)) 
		return E_FAIL;

	*pMode = (AttributeMode)nVal;
	return NOERROR;
	}

STDMETHODIMP CAutoAttributeDef::put_Mode (AttributeMode pMode)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if ((pMode < 0) || (pMode > 16))
		return E_INVALIDARG;

//	int flags  = 0;
//	m_pDbHandItem->get_70 (&flags);
//	flags = flags | pMode;
	
	m_pDbHandItem->set_70 (pMode);
	return NOERROR;
	}

STDMETHODIMP CAutoAttributeDef::get_Constant (VARIANT_BOOL * Mode)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (Mode == NULL)
		return E_POINTER;
	
	int nVal;
	if (!m_pDbHandItem->get_70(&nVal)) 
		return E_FAIL;

	*Mode = (nVal & IC_ATTDEF_CONST) ? VARIANT_TRUE : VARIANT_FALSE;

	return NOERROR;
	}

STDMETHODIMP CAutoAttributeDef::put_Constant (VARIANT_BOOL Mode)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	int flags  = 0;
	m_pDbHandItem->get_70 (&flags);
	
	if (Mode == VARIANT_TRUE) 
		flags |= IC_ATTDEF_CONST;
	else
		flags &= ~IC_ATTDEF_CONST;
	
	m_pDbHandItem->set_70 (flags);
	return NOERROR;
	}

STDMETHODIMP CAutoAttributeDef::get_Invisible (VARIANT_BOOL * Mode)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (Mode == NULL)
		return E_POINTER;
	
	int nVal;
	if (!m_pDbHandItem->get_70(&nVal)) 
		return E_FAIL;

	*Mode = (nVal & IC_ATTDEF_INVIS) ? VARIANT_TRUE : VARIANT_FALSE;

	return NOERROR;
	}

STDMETHODIMP CAutoAttributeDef::put_Invisible (VARIANT_BOOL Mode)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	int flags  = 0;
	m_pDbHandItem->get_70 (&flags);
	
	if (Mode == VARIANT_TRUE) 
		flags |= IC_ATTDEF_INVIS;
	else
		flags &= ~IC_ATTDEF_INVIS;
	
	m_pDbHandItem->set_70 (flags);
	return NOERROR;
	}

STDMETHODIMP CAutoAttributeDef::get_Preset (VARIANT_BOOL * Mode)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (Mode == NULL)
		return E_POINTER;
	
	int nVal;
	if (!m_pDbHandItem->get_70(&nVal)) 
		return E_FAIL;

	*Mode = (nVal & IC_ATTDEF_PRESET) ? VARIANT_TRUE : VARIANT_FALSE;

	return NOERROR;
	}

STDMETHODIMP CAutoAttributeDef::put_Preset (VARIANT_BOOL Mode)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	int flags  = 0;
	m_pDbHandItem->get_70 (&flags);
	
	if (Mode == VARIANT_TRUE) 
		flags |= IC_ATTDEF_PRESET;
	else
		flags &= ~IC_ATTDEF_PRESET;
	
	m_pDbHandItem->set_70 (flags);
	return NOERROR;
	}

STDMETHODIMP CAutoAttributeDef::get_Verify (VARIANT_BOOL * Mode)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (Mode == NULL)
		return E_POINTER;
	
	int nVal;
	if (!m_pDbHandItem->get_70(&nVal)) 
		return E_FAIL;

	*Mode = (nVal & IC_ATTDEF_VERIFY) ? VARIANT_TRUE : VARIANT_FALSE;

	return NOERROR;
	}

STDMETHODIMP CAutoAttributeDef::put_Verify (VARIANT_BOOL Mode)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	int flags  = 0;
	m_pDbHandItem->get_70 (&flags);
	
	if (Mode == VARIANT_TRUE) 
		flags |= IC_ATTDEF_VERIFY;
	else
		flags &= ~IC_ATTDEF_VERIFY;
	
	m_pDbHandItem->set_70 (flags);
	return NOERROR;
	}

STDMETHODIMP CAutoAttributeDef::get_ObliqueAngle (double * pAngle)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pAngle == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_51(pAngle)) 
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoAttributeDef::put_ObliqueAngle (double pAngle)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (!m_pDbHandItem->set_51(pAngle))
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoAttributeDef::get_PromptString (BSTR * pPrompt)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pPrompt == NULL)
		return E_POINTER;
	
	char* pStr;
	if (!m_pDbHandItem->get_3(&pStr)) 
		return E_FAIL;

	CString str(pStr);
	*pPrompt = str.AllocSysString();

	return NOERROR;
	}

STDMETHODIMP CAutoAttributeDef::put_PromptString (BSTR pPrompt)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	CString str(pPrompt);

	if (!m_pDbHandItem->set_3(str.GetBuffer(0))) 
		return E_FAIL;
	
	return NOERROR;
	}

STDMETHODIMP CAutoAttributeDef::get_Rotation (double * pRotation)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pRotation == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_50(pRotation)) 
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoAttributeDef::put_Rotation (double pRotation)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (!m_pDbHandItem->set_50(pRotation))
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoAttributeDef::get_ScaleFactor (double * pScaleFactor)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pScaleFactor == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_41(pScaleFactor)) 
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoAttributeDef::put_ScaleFactor (double pScaleFactor)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pScaleFactor <= 0.0)
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_41(pScaleFactor))
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoAttributeDef::get_StyleName (BSTR * pStyleName)
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

STDMETHODIMP CAutoAttributeDef::put_StyleName (BSTR pStyleName)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	CString str(pStyleName);
	if (str.IsEmpty ())
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_7(str.GetBuffer(0), m_pDoc->m_dbDrawing)) 
		return E_FAIL;
	
	return NOERROR;
	}

STDMETHODIMP CAutoAttributeDef::get_TagString (BSTR * pTag)
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

STDMETHODIMP CAutoAttributeDef::put_TagString (BSTR pTag)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	CString str(pTag);

	if (!m_pDbHandItem->set_2(str.GetBuffer(0))) 
		return E_FAIL;
	
	return NOERROR;
	}

STDMETHODIMP CAutoAttributeDef::get_Thickness (double * pThick)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pThick == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_39(pThick))
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoAttributeDef::put_Thickness (double pThick)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	
	if (pThick < 0.0)
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_39(pThick))
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoAttributeDef::get_TextAlignmentPoint (IIcadPoint * * ppPoint)
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

STDMETHODIMP CAutoAttributeDef::put_TextAlignmentPoint (IIcadPoint * ppPoint)
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

STDMETHODIMP CAutoAttributeDef::get_TextGenerationFlag (TextFlag * pFlags)
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

STDMETHODIMP CAutoAttributeDef::put_TextGenerationFlag (TextFlag pFlags)
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

STDMETHODIMP CAutoAttributeDef::get_TextString (BSTR * pText)
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

STDMETHODIMP CAutoAttributeDef::put_TextString (BSTR pText)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	CString str(pText);

	if (!m_pDbHandItem->set_1(str.GetBuffer(0))) 
		return E_FAIL;
	
	return NOERROR;
	}

STDMETHODIMP CAutoAttributeDef::get_VerticalAlignment (VerticalAlignment * pVertAlign)
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

STDMETHODIMP CAutoAttributeDef::put_VerticalAlignment (VerticalAlignment pVertAlign)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
    if ((pVertAlign < vicVerticalAlignmentBaseline) || (pVertAlign > vicVerticalAlignmentTop))
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_73(pVertAlign)) 
		return E_FAIL;

	return NOERROR;
	}



