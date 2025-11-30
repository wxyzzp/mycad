/* INTELLICAD\PRJ\LIB\AUTO\AUTOLAYER.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Implementation of CAutoTextStyle - the Automation TextStyle object
 * 
 */ 

#include "StdAfx.h"
#include "AutoTextStyle.h"
#include "AutoPoint.h"
#include "AutoVector.h"
#include "styletabrec.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoTextStyle

HRESULT CAutoTextStyle::Init(CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem)
	{
	CAutoDbItem::Init(pDoc, pParent);
	
	if (pDbHandItem)
		m_pDbHandItem = pDbHandItem;
	else
		m_pDbHandItem = new db_styletabrec();
	
	if (NULL == m_pDbHandItem) 
		return FALSE;
	
	return NOERROR;
	}

STDMETHODIMP CAutoTextStyle::get_BigFontFile(BSTR * pFontFile)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pFontFile == NULL)
		return E_POINTER;
	
	char* pStr;
	m_pDbHandItem->get_4 (&pStr);
	CString str (pStr);
	
	*pFontFile = str.AllocSysString();
	return NOERROR;
	}

STDMETHODIMP CAutoTextStyle::put_BigFontFile(BSTR pFontFile)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	CString str(pFontFile);

	if (!m_pDbHandItem->set_4((char*) str.GetBuffer(0), m_pDoc->m_dbDrawing)) 
		return E_FAIL;
	
	return NOERROR;
	}

STDMETHODIMP CAutoTextStyle::get_FontFile(BSTR * pFontFile)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pFontFile == NULL)
		return E_POINTER;
	
	char* pStr;
	m_pDbHandItem->get_3 (&pStr);
	CString str (pStr);
	
	*pFontFile = str.AllocSysString();
	return NOERROR;
	}

STDMETHODIMP CAutoTextStyle::put_FontFile(BSTR pFontFile)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	CString str(pFontFile);
	if (str.IsEmpty ())
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_3 ((char*) str.GetBuffer(0), m_pDoc->m_dbDrawing)) 
		return E_FAIL;
	
	return NOERROR;
	}

STDMETHODIMP CAutoTextStyle::get_Height(double * pHeight)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pHeight == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_40(pHeight)) 
		return E_FAIL;
	
	return NOERROR;
	}

STDMETHODIMP CAutoTextStyle::put_Height(double pHeight)
	{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (pHeight < 0.0)
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_40(pHeight)) 
		return E_FAIL;
	
	return NOERROR;
	}

STDMETHODIMP CAutoTextStyle::get_LastHeight(double * pHeight)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pHeight == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_42(pHeight)) 
		return E_FAIL;
	
	return NOERROR;
	}

STDMETHODIMP CAutoTextStyle::put_LastHeight(double pHeight)
	{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (pHeight < 0.0)
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_42(pHeight)) 
		return E_FAIL;
	
	return NOERROR;
	}

STDMETHODIMP CAutoTextStyle::get_ObliqueAngle(double * pAngle)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pAngle == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_50(pAngle)) 
		return E_FAIL;
	
	return NOERROR;
	}

STDMETHODIMP CAutoTextStyle::put_ObliqueAngle(double pAngle)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (!m_pDbHandItem->set_50(pAngle)) 
		return E_FAIL;
	
	return NOERROR;
	}

STDMETHODIMP CAutoTextStyle::get_Width(double * pWidth)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pWidth == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_41(pWidth)) 
		return E_FAIL;
	
	return NOERROR;
	}

STDMETHODIMP CAutoTextStyle::put_Width(double pWidth)
	{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (pWidth < 0.0)
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_41(pWidth)) 
		return E_FAIL;
	
	return NOERROR;
	}

STDMETHODIMP CAutoTextStyle::get_TextGenerationFlag(TextFlag * pTextFlags)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pTextFlags == NULL)
		return E_POINTER;
	
	int flags = 0;
	if (!m_pDbHandItem->get_71(&flags)) 
		return E_FAIL;
	
	*pTextFlags	= (TextFlag)flags;
	
	return NOERROR;
	}

STDMETHODIMP CAutoTextStyle::put_TextGenerationFlag(TextFlag pTextFlags)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
    switch (pTextFlags)
		{
		case vicTextFlagNormal:		//0x0
		case vicTextFlagBackward:	//0x2
		case vicTextFlagUpsideDown:	//0x4
		case (vicTextFlagBackward + vicTextFlagUpsideDown):
			break;
		default:
			return E_INVALIDARG;
		}

	if(!m_pDbHandItem->set_71(pTextFlags)) 
		return E_FAIL;
	
	return NOERROR;
	}

// EBATECH(CNBR) -[ 2002/4/24 Support SHAPE
STDMETHODIMP CAutoTextStyle::get_Shape (VARIANT_BOOL * pShape)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pShape == NULL)
		return E_POINTER;
	
	int flag = 0;
	if (!m_pDbHandItem->get_70(&flag)) 
		return E_FAIL;
	
	if (flag & IC_SHAPEFILE_SHAPES) 
		*pShape = VARIANT_TRUE;
	else 
		*pShape = VARIANT_FALSE;
	
	return NOERROR;
	}

STDMETHODIMP CAutoTextStyle::put_Shape (VARIANT_BOOL pShape)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	int flags = 0;
	
	if (!m_pDbHandItem->get_70(&flags)) 
		return E_FAIL;

	if (pShape == VARIANT_TRUE) 
		flags |= IC_SHAPEFILE_SHAPES;
	else
		flags &= ~IC_SHAPEFILE_SHAPES;
	
	if (!m_pDbHandItem->set_70(flags)) 
		return E_FAIL;
	
	return NOERROR;
	}
// EBATECH(CNBR) ]-

// EBATECH(CNBR) -[ 2002/5/11 
STDMETHODIMP CAutoTextStyle::SetXData (VARIANT TypeArray, VARIANT DataArray)
	{
	HRESULT hr;
	RETURN_IF_INVALID (m_pDbHandItem);
	
	hr = CAutoDbItem::SetXData( TypeArray, DataArray );
	if( hr == NOERROR )
		{
		((db_styletabrec*)m_pDbHandItem)->setFontFamilyEED();
		}
	return hr;
	}
