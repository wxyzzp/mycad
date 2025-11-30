/* INTELLICAD\PRJ\LIB\AUTO\AUTOLINETYPE.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Implementation of CAutoLinetype - the Automation Linetype object
 * 
 */ 

#include "StdAfx.h"
#include "AutoLinetype.h"
#include "AutoPoint.h"
#include "AutoVector.h"
#include "ltypetabrec.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoLinetype

HRESULT CAutoLinetype::Init(CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem)
	{
	CAutoDbItem::Init(pDoc, pParent);
	
	if (pDbHandItem)
		m_pDbHandItem = pDbHandItem;
	else
		m_pDbHandItem = new db_ltypetabrec();
	
	if (NULL == m_pDbHandItem) 
		return FALSE;
	
	return NOERROR;
	}

STDMETHODIMP CAutoLinetype::get_Description (BSTR * pDescription)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pDescription == NULL)
		return E_POINTER;
	
	char* pStr;
	m_pDbHandItem->get_3 (&pStr);
	CString str (pStr);
	*pDescription = str.AllocSysString();
	return NOERROR;
	}

STDMETHODIMP CAutoLinetype::put_Description (BSTR pDescription)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	CString str(pDescription);
	if (!m_pDbHandItem->set_3 ((char*)str.GetBuffer(0)))
		return E_FAIL;
	return NOERROR;
	}



