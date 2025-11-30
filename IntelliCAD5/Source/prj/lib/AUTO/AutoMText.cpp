/* INTELLICAD\PRJ\LIB\AUTO\AUTOMTEXT.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * Implementation of CAutoMText - the Automation MText object
 *
 */

#include "StdAfx.h"
#include "AutoMText.h"
#include "AutoPoint.h"
#include "AutoVector.h"
#include "AutoHelpers.h"
#include "DbMText.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoMText

HRESULT CAutoMText::Init(CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem)
	{
	CAutoDbItem::Init(pDoc, pParent);

	if (pDbHandItem)
		m_pDbHandItem = pDbHandItem;
	else
		{
		if (pDoc)
			m_pDbHandItem = new CDbMText(pDoc->m_dbDrawing);
		else
			m_pDbHandItem = new CDbMText();
		}

	if (NULL == m_pDbHandItem)
		return FALSE;

	return NOERROR;
	}

STDMETHODIMP CAutoMText::get_AttachmentPoint (AttachmentPoint * pPoint)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pPoint == NULL)
		return E_POINTER;

	if (!m_pDbHandItem->get_71((int *)pPoint))
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoMText::put_AttachmentPoint (AttachmentPoint pPoint)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
    if ((pPoint < vicAttachmentPointTopLeft) || (pPoint > vicAttachmentPointBottomRight))
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_71(pPoint))
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoMText::get_DrawingDirection (DrawingDirection * pDrawingDirection)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pDrawingDirection == NULL)
		return E_POINTER;

	if (!m_pDbHandItem->get_72((int *)pDrawingDirection))
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoMText::put_DrawingDirection (DrawingDirection pDrawingDirection)
	{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (pDrawingDirection < vicLeftToRight || pDrawingDirection > vicBottomToTop)
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_72(pDrawingDirection))
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoMText::get_Normal (IIcadVector * * ppExtrusionDirection)
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

STDMETHODIMP CAutoMText::put_Normal (IIcadVector * ExtrusionDirection)
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

STDMETHODIMP CAutoMText::get_Height (double * pHeight)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pHeight == NULL)
		return E_POINTER;

	if (!m_pDbHandItem->get_40(pHeight))
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoMText::put_Height (double pHeight)
	{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (pHeight < 0.0)
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_40(pHeight))
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoMText::get_InsertionPoint (IIcadPoint * * ppInsPoint)
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

STDMETHODIMP CAutoMText::put_InsertionPoint (IIcadPoint * ppInsPoint)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	sds_point pt;
	pt[0] = pt[1] = pt[2] = 0.0;
	if (ppInsPoint)
		{
		ppInsPoint->get_x(&pt[0]);
		ppInsPoint->get_y(&pt[1]);
		ppInsPoint->get_z(&pt[2]);
		}
	m_pDbHandItem->set_10(pt);

	return NOERROR;
	}

STDMETHODIMP CAutoMText::get_Rotation (double * pRotation)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pRotation == NULL)
		return E_POINTER;

	if (!((CDbMText *)m_pDbHandItem)->get_50(pRotation, m_pDoc->m_dbDrawing))
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoMText::put_Rotation (double pRotation)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (!((CDbMText *)m_pDbHandItem)->set_50(pRotation, m_pDoc->m_dbDrawing))
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoMText::get_StyleName (BSTR * pStyleName)
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

STDMETHODIMP CAutoMText::put_StyleName (BSTR pStyleName)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	CString str(pStyleName);
	if (str.IsEmpty ())
		return E_INVALIDARG;

	if ( !m_pDbHandItem->set_7 ((char*)str.GetBuffer(0), m_pDoc->m_dbDrawing))
		return E_FAIL;
	return NOERROR;
	}

STDMETHODIMP CAutoMText::get_TextString (BSTR * pText)
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

STDMETHODIMP CAutoMText::put_TextString (BSTR pText)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	CString str(pText);

	if ( !m_pDbHandItem->set_1 (str.GetBuffer(0)))
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoMText::get_Width (double * pWidth)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pWidth == NULL)
		return E_POINTER;

	if (!m_pDbHandItem->get_41(pWidth))
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoMText::put_Width (double pWidth)
	{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (pWidth < 0.0)
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_41(pWidth))
		return E_FAIL;

	return NOERROR;
	}

// EBATECH(CNBR) -[ 04/02/2002 MTEXT Explode
STDMETHODIMP CAutoMText::Explode (IIcadSelectionSet * * pObjects)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pObjects == NULL)
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

	hr = ConvertSDSssetToAutoSelectionSet (ssetout, pObjects, m_pDoc);
	if (FAILED(hr))
		return E_FAIL;

	m_pDoc->AUTO_ssfree (ssetout);

	return NOERROR;
	}
// EBATECH(CNBR) ]-

