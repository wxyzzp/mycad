/* INTELLICAD\PRJ\LIB\AUTO\AUTOBLOCKINSERT.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Implementation of CAutoBlockInsert - the Automation Block Insert object
 * 
 */ 

#include "StdAfx.h"
#include "AutoBlockInsert.h"
#include "AutoPoint.h"
#include "AutoVector.h"
#include "AutoAttributes.h"
#include "AutoHelpers.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoBlockInsert

HRESULT CAutoBlockInsert::Init(CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem)
	{
	CAutoDbItem::Init(pDoc, pParent);
	
	if (pDbHandItem)
		m_pDbHandItem = pDbHandItem;
	else
		{
		if (pDoc)
			m_pDbHandItem = new db_insert(pDoc->m_dbDrawing);
		else
			m_pDbHandItem = new db_insert();

		//For now we create a new entity.  But when this is inserted by a call to CIcadDoc::InsertBlock
		//another db_insert is new'ed.  Hence we set this flag to remind us to cleanup.
		m_bNeedToDelete = TRUE;
		}
	
	if (NULL == m_pDbHandItem) 
		return FALSE;
	
	return NOERROR;
	}

STDMETHODIMP CAutoBlockInsert::get_InsertionPoint (IIcadPoint * * ppInsertPoint)
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

STDMETHODIMP CAutoBlockInsert::put_InsertionPoint (IIcadPoint * ppInsertPoint)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	sds_point pt;
	pt[0] = pt[1] = pt[2] = 0.0;
	if (ppInsertPoint)	//this is an optional arg
		{
		ppInsertPoint->get_x(&pt[0]);
		ppInsertPoint->get_y(&pt[1]);
		ppInsertPoint->get_z(&pt[2]);
		}
	m_pDbHandItem->set_10(pt);
	
	return NOERROR;
	}

STDMETHODIMP CAutoBlockInsert::get_Name (BSTR * pBlockName)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pBlockName == NULL)
		return E_POINTER;
	
	char* pStr;
	if ( !m_pDbHandItem->get_2 (&pStr)) 
		return E_FAIL;
	
	CString str(pStr);
	*pBlockName = str.AllocSysString();
	return NOERROR;
	}

STDMETHODIMP CAutoBlockInsert::put_Name (BSTR pBlockName)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	CString str(pBlockName);
	if (str.IsEmpty ())
		return E_INVALIDARG;

	if (!ic_isvalidname(str.GetBuffer(0)))
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_2 (str.GetBuffer(0), m_pDoc->m_dbDrawing))
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoBlockInsert::get_Normal (IIcadVector * * ppExtrusionDirection)
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

STDMETHODIMP CAutoBlockInsert::put_Normal (IIcadVector * ExtrusionDirection)
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

STDMETHODIMP CAutoBlockInsert::get_HasAttributes (VARIANT_BOOL * pbHasAttributes)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pbHasAttributes == NULL)
		return E_POINTER;
	
	BOOL bHasAttribs;
	if (!m_pDbHandItem->get_66(&bHasAttribs)) 
		return E_FAIL;
	
	if(bHasAttribs) 
		*pbHasAttributes = VARIANT_TRUE;
	else 
		*pbHasAttributes = VARIANT_FALSE;
	
	return NOERROR;
	}

STDMETHODIMP CAutoBlockInsert::get_Rotation (double * pRotation)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pRotation == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_50(pRotation))
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoBlockInsert::put_Rotation (double pRotation)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (!m_pDbHandItem->set_50(pRotation))
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoBlockInsert::get_XScaleFactor (double * pScaleFactor)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pScaleFactor == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_41(pScaleFactor))
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoBlockInsert::put_XScaleFactor (double pScaleFactor)
	{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (pScaleFactor <= 0.0)
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_41(pScaleFactor))
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoBlockInsert::get_YScaleFactor (double * pScaleFactor)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pScaleFactor == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_42(pScaleFactor))
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoBlockInsert::put_YScaleFactor (double pScaleFactor)
	{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (pScaleFactor <= 0.0)
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_42(pScaleFactor))
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoBlockInsert::get_ZScaleFactor (double * pScaleFactor)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pScaleFactor == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_43(pScaleFactor))
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoBlockInsert::put_ZScaleFactor (double pScaleFactor)
	{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (pScaleFactor <= 0.0)
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_43(pScaleFactor))
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoBlockInsert::Explode (IIcadSelectionSet * * ppArrayOfEntities)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppArrayOfEntities == NULL)
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

	hr = ConvertSDSssetToAutoSelectionSet (ssetout, ppArrayOfEntities, m_pDoc);
	if (FAILED(hr))
		return E_FAIL;

	m_pDoc->AUTO_ssfree (ssetout);

	return NOERROR;
	}

STDMETHODIMP CAutoBlockInsert::GetAttributes (IIcadAttributes * * ppAttributes)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppAttributes == NULL)
		return E_POINTER;
	
	CComObject<CAutoAttributes> *pAttributes;
	CComObject<CAutoAttributes>::CreateInstance(&pAttributes);
	if (!pAttributes)
		return E_OUTOFMEMORY;

	db_handitem *pFirstAttrib = m_pDoc->m_dbDrawing->entnext_noxref(m_pDbHandItem);

	if (pFirstAttrib && (DB_ATTRIB == pFirstAttrib->ret_type()))
		{
		CComPtr<IDispatch> pDisp;
		HRESULT hr = QueryInterface (IID_IDispatch, (void **)&pDisp);
		if (FAILED(hr))
			return hr;

		hr = pAttributes->Init (m_pDoc, pDisp, pFirstAttrib);
		if (FAILED(hr))
			return hr;

		return pAttributes->QueryInterface (ppAttributes);
		}
	else
		return E_FAIL;
	}



