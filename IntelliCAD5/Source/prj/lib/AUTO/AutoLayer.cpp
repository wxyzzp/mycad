/* INTELLICAD\PRJ\LIB\AUTO\AUTOLAYER.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Implementation of CAutoLayer - the Automation Layer object
 * 
 */ 

#include "StdAfx.h"
#include "AutoLayer.h"
#include "AutoPoint.h"
#include "AutoVector.h"
#include "layertabrec.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoLayer

HRESULT CAutoLayer::Init(CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem)
	{
	CAutoDbItem::Init(pDoc, pParent);
	
	if (pDbHandItem)
		m_pDbHandItem = pDbHandItem;
	else
		m_pDbHandItem = new db_layertabrec();
	
	if (NULL == m_pDbHandItem) 
		return FALSE;
	
	return NOERROR;
	}

STDMETHODIMP CAutoLayer::get_Color (Colors * pColor)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pColor == NULL)
		return E_POINTER;
	
	int col;
	m_pDbHandItem->get_62 (&col);
	*pColor = (Colors)col;
	return NOERROR;
	}

STDMETHODIMP CAutoLayer::put_Color (Colors pColor)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pColor < 0 || pColor > 256)
		return E_INVALIDARG;

	m_pDbHandItem->set_62(pColor);
	return NOERROR;
	}

STDMETHODIMP CAutoLayer::get_Freeze (VARIANT_BOOL * pFreeze)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pFreeze == NULL)
		return E_POINTER;
	
	int flags;
	m_pDbHandItem->get_70 (&flags);
	*pFreeze = (flags & IC_LAYER_FROZEN) ? VARIANT_TRUE : VARIANT_FALSE;
	return NOERROR;
	}

STDMETHODIMP CAutoLayer::put_Freeze (VARIANT_BOOL pFreeze)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	int flags  = 0;
	m_pDbHandItem->get_70 (&flags);
	
	if (pFreeze == VARIANT_TRUE) 
		flags |= IC_LAYER_FROZEN;
	else
		flags &= ~IC_LAYER_FROZEN;
	
	m_pDbHandItem->set_70 (flags);
	return NOERROR;
	}

STDMETHODIMP CAutoLayer::get_LayerOn (VARIANT_BOOL * pOn)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pOn == NULL)
		return E_POINTER;
	
	int flags;
	m_pDbHandItem->get_62 (&flags);
	*pOn = (flags < 0) ? VARIANT_FALSE : VARIANT_TRUE;
	return NOERROR;
	}

STDMETHODIMP CAutoLayer::put_LayerOn (VARIANT_BOOL pOn)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	int flags  = 0;
	m_pDbHandItem->get_62 (&flags);
	
	if (pOn == VARIANT_TRUE)
		flags = (flags < 0) ? flags * -1 : flags;
	else
		flags = (flags < 0) ? flags : flags * -1 ;
	
	m_pDbHandItem->set_62 (flags);
	return NOERROR;
	}

STDMETHODIMP CAutoLayer::get_Linetype (BSTR * pLinetype)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pLinetype == NULL)
		return E_POINTER;
	
	char* pStr;
	m_pDbHandItem->get_6 (&pStr);
	
	CString str (pStr);
	*pLinetype = str.AllocSysString();
	return NOERROR;
	}

STDMETHODIMP CAutoLayer::put_Linetype (BSTR pLinetype)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	CString str(pLinetype);
	if (str.IsEmpty ())
		return E_INVALIDARG;

	if ( !m_pDbHandItem->set_6 ((char*)str.GetBuffer(0), m_pDoc->m_dbDrawing)) 
		return ResultFromScode(E_FAIL);
	return NOERROR;
	}

STDMETHODIMP CAutoLayer::get_Lock (VARIANT_BOOL * pLock)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pLock == NULL)
		return E_POINTER;
	
	int flags;
	m_pDbHandItem->get_70 (&flags);
	*pLock = (flags & IC_LAYER_LOCKED) ? VARIANT_TRUE : VARIANT_FALSE;
	return NOERROR;
	}

STDMETHODIMP CAutoLayer::put_Lock (VARIANT_BOOL pLock)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	int flags  = 0;
	m_pDbHandItem->get_70 (&flags);
	
	if (pLock == VARIANT_TRUE) 
		flags |= IC_LAYER_LOCKED;
	else
		flags &= ~IC_LAYER_LOCKED;
	
	m_pDbHandItem->set_70 (flags);
	return NOERROR;
	}



