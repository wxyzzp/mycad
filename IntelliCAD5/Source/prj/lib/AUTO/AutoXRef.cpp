/* INTELLICAD\PRJ\LIB\AUTO\AUTOXREF.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * $Revision: 1.3 $ $Date: 2001/03/16 21:48:12 $ 
 * 
 * Abstract
 * 
 * Implementation of CAutoExternalReference - the Automation External Reference object
 * 
 */ 

#include "StdAfx.h"
#include "AutoPoint.h"
#include "AutoVector.h"
#include "AutoAttributes.h"
#include "AutoXRef.h"
#include "AutoHelpers.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoExternalReference

HRESULT CAutoExternalReference::Init(CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem)
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
		}
	
	if (NULL == m_pDbHandItem) 
		return FALSE;
	
	return NOERROR;
	}

STDMETHODIMP CAutoExternalReference::get_InsertionPoint (IIcadPoint * * ppInsertPoint)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppInsertPoint == NULL)
		return E_POINTER;
	
	CComObject<CAutoPoint> *pPoint;
	CComObject<CAutoPoint>::CreateInstance(&pPoint);
	if (!pPoint)
		return E_FAIL;
	
	sds_point pt;
	m_pDbHandItem->get_10(pt);	
	
	pPoint->Init(NULL, NULL, pt[0], pt[1], pt[2]);
	
	return pPoint->QueryInterface (ppInsertPoint);
	}

STDMETHODIMP CAutoExternalReference::put_InsertionPoint (IIcadPoint * ppInsertPoint)
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

STDMETHODIMP CAutoExternalReference::get_Name (BSTR * Name)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (Name == NULL)
		return E_POINTER;
	
	char* pStr;
	if ( !m_pDbHandItem->get_2 (&pStr)) 
		return E_FAIL;
	
	CString str(pStr);
	*Name = str.AllocSysString();
	return NOERROR;
	}

STDMETHODIMP CAutoExternalReference::put_Name (BSTR Name)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	CString str(Name);
	if (str.IsEmpty ())
		return E_INVALIDARG;

	if (!ic_isvalidname(str.GetBuffer(0)))
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_2 (str.GetBuffer(0), m_pDoc->m_dbDrawing))
		return E_FAIL;
	
	return NOERROR;
	}

STDMETHODIMP CAutoExternalReference::get_Normal (IIcadVector * * ppExtrusionDirection)
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

STDMETHODIMP CAutoExternalReference::put_Normal (IIcadVector * ExtrusionDirection)
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

STDMETHODIMP CAutoExternalReference::get_HasAttributes (VARIANT_BOOL * pbHasAttributes)
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

STDMETHODIMP CAutoExternalReference::get_Path (BSTR * Path)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (Path == NULL)
		return E_POINTER;
	
    db_blocktabrec *bthip = ((db_insert *)m_pDbHandItem)->ret_bthip();
	if (bthip == NULL)
		return E_FAIL;

	char* pStr;
	if ( !bthip->get_1 (&pStr)) 
		return E_FAIL;
	
	CString str(pStr);
	*Path = str.AllocSysString();
	
	return NOERROR;
	}

STDMETHODIMP CAutoExternalReference::put_Path (BSTR Path)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	CString str(Path);
	if (str.IsEmpty ())
		return E_INVALIDARG;

	//check for file existence
	if (!FileExists (str.GetBuffer(0)))
		return E_INVALIDARG;

	//make sure it has a dwg extension
	char ext[10];
	ic_getext( str.GetBuffer(0), ext);
	CString StrExt(ext);
	if (StrExt.CompareNoCase (".DWG") != 0)
		return E_INVALIDARG;


    db_blocktabrec *bthip = ((db_insert *)m_pDbHandItem)->ret_bthip();
	if (bthip == NULL)
		return E_FAIL;

	if (!bthip->set_1 (str.GetBuffer(0)))
		return E_FAIL;
	
	return NOERROR;
	}


STDMETHODIMP CAutoExternalReference::get_Rotation (double * pRotation)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pRotation == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_50(pRotation))
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoExternalReference::put_Rotation (double pRotation)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (!m_pDbHandItem->set_50(pRotation))
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoExternalReference::get_XScaleFactor (double * pScaleFactor)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pScaleFactor == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_41(pScaleFactor))
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoExternalReference::put_XScaleFactor (double pScaleFactor)
	{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (pScaleFactor <= 0.0)
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_41(pScaleFactor))
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoExternalReference::get_YScaleFactor (double * pScaleFactor)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pScaleFactor == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_42(pScaleFactor))
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoExternalReference::put_YScaleFactor (double pScaleFactor)
	{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (pScaleFactor <= 0.0)
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_42(pScaleFactor))
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoExternalReference::get_ZScaleFactor (double * pScaleFactor)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pScaleFactor == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_43(pScaleFactor))
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoExternalReference::put_ZScaleFactor (double pScaleFactor)
	{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (pScaleFactor <= 0.0)
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_43(pScaleFactor))
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoExternalReference::GetAttributes (IIcadAttributes * * ppAttributes)
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



