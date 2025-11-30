/* INTELLICAD\PRJ\LIB\AUTO\AUTODBITEM.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Implementation of CAutoDbItem - this is the root class for all database objects in the
 * Intellicad Automation model.  CAutoEntity and CAutoTable are derived from this.
 * 
 */ 

#include "StdAfx.h"
#include "AutoDbItem.h"
#include "AutoBaseDefs.h"
#include "AutoHelpers.h"
#include "AutoPoint.h"
#include "appid.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoDbItem

HRESULT CAutoDbItem::Init (CIcadDoc *pDoc, IDispatch *pParent)
	{
	m_pDoc = pDoc;
	m_pParent = pParent;
	return NOERROR;
	}

STDMETHODIMP CAutoDbItem::get_Application (IIcadApplication * * ppApp)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppApp == NULL)
		return E_POINTER;
	
	return GetApp()->m_pAutoApp->QueryInterface (ppApp);
	}

STDMETHODIMP CAutoDbItem::get_Document (IIcadDocument * * ppDoc)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppDoc == NULL)
		return E_POINTER;
	
	return m_pDoc->m_pAutoDoc->QueryInterface (ppDoc);
	}

STDMETHODIMP CAutoDbItem::get_Parent (IDispatch * * ppParent)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppParent == NULL)
		return E_POINTER;
	
	if (m_pParent)
		return m_pParent->QueryInterface(ppParent);
	else
		{
		//sometimes when Entity wrappers are created, the parent is not initialized.
		//this happens for example when doing an Entity.Copy - the new entity is created
		//and added to the appropriate space (model or paper) in the database, but its
		//COM counterpart, the AutoEntity is created without a parent.  So return the
		//correct parent here.  
		CComPtr<IIcadDocument> pDoc;
		HRESULT hr = m_pDoc->m_pAutoDoc->QueryInterface (&pDoc);
		if (FAILED (hr))
			return E_FAIL;

		if (m_pDbHandItem->ret_pspace())
			{
			CComPtr<IIcadPaperSpace> pPSpace;

			hr = pDoc->get_PaperSpace (&pPSpace);
			if (FAILED (hr))
				return E_FAIL;

			return pPSpace->QueryInterface (ppParent);
			}
		else
			{
			CComPtr<IIcadModelSpace> pMSpace;

			hr = pDoc->get_ModelSpace (&pMSpace);
			if (FAILED (hr))
				return E_FAIL;

			return pMSpace->QueryInterface (ppParent);
			}
		}
	}

STDMETHODIMP CAutoDbItem::get_Handle (BSTR * pstrHandle)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pstrHandle == NULL)
		return E_POINTER;
	
	
	char pStr[17];
	db_objhandle Handle = m_pDbHandItem->RetHandle();
	Handle.ToAscii(pStr);
	
	CString str(pStr);
	*pstrHandle = str.AllocSysString();
	
	return NOERROR;
	}

STDMETHODIMP CAutoDbItem::_handitem (long * pHanditem)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	
	*pHanditem = (long) m_pDbHandItem;
	
	return NOERROR;
	}

STDMETHODIMP CAutoDbItem::GetXData (BSTR AppName, VARIANT * TypeArray, VARIANT * DataArray)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (TypeArray == NULL)
		return E_POINTER;
	
	if (DataArray == NULL)
		return E_POINTER;
	
	CString strApp (AppName);
	CString strEmpty ("*");

	struct resbuf rb;
	rb.restype = RTSTR;
	if (strApp.IsEmpty())
		rb.resval.rstring = strEmpty.GetBuffer(0);
	else
		rb.resval.rstring = strApp.GetBuffer(0);
	rb.rbnext = NULL;
 
	struct resbuf *prbExtendedData = m_pDbHandItem->get_eed (&rb, NULL);

	if (prbExtendedData == NULL)
		{
		(*TypeArray).vt = VT_EMPTY;
		(*TypeArray).parray = NULL;
		(*DataArray).vt = VT_EMPTY;
		(*DataArray).parray = NULL;
		return NOERROR;
		}

	long nCount = 0;
	struct resbuf *ptr = prbExtendedData;
	while( ptr )
		{
		ptr = ptr->rbnext;
		nCount++;
		}

	VARIANT var;
	SAFEARRAY FAR* Types;
	SAFEARRAY FAR* Values;
	SAFEARRAYBOUND Bound[1];
	Bound[0].lLbound = 0;
	Bound[0].cElements = nCount - 1;
	Types = SafeArrayCreate(VT_I2, 1, Bound);
	Values = SafeArrayCreate(VT_VARIANT, 1, Bound);

	ptr = prbExtendedData->rbnext;
	nCount = 0;
	while( ptr )
		{
		SafeArrayPutElement(Types, &nCount, &(ptr->restype));
		switch (ptr->restype)
			{
			case 1000:	//String
			case 1001:	//Application name
			case 1002:	//Control string
			case 1003:	//Layer name
			case 1005:	//*Database handle
				{
				CString str(ptr->resval.rstring);
				var.vt = VT_BSTR;
				var.bstrVal = str.AllocSysString();
				SafeArrayPutElement(Values, &nCount, &var);
				}
				break;
			case 1010:
			case 1020:
			case 1030:
			case 1011:
			case 1021:
			case 1031:
			case 1012:
			case 1022:
			case 1032:
			case 1013:
			case 1023:
			case 1033:
				{
				CComObject<CAutoPoint> *pPoint;
				CComObject<CAutoPoint>::CreateInstance(&pPoint);
				if (!pPoint)
					return E_FAIL;
				
				pPoint->Init(NULL, NULL, ptr->resval.rpoint[0], ptr->resval.rpoint[1], ptr->resval.rpoint[2]);
				
				IDispatch *pDisp;
				if (FAILED (pPoint->QueryInterface (&pDisp)))
					return E_FAIL;

				var.vt = VT_DISPATCH;
				var.pdispVal = pDisp;
				SafeArrayPutElement(Values, &nCount, &var);
				}
				break;
			case 1040:
			case 1041:
			case 1042:
				var.vt = VT_R8;
				var.dblVal = ptr->resval.rreal;
				SafeArrayPutElement(Values, &nCount, &var);
				break;
			case 1070:
				var.vt = VT_I2;
				var.iVal = ptr->resval.rint;
				SafeArrayPutElement(Values, &nCount, &var);
				break;
			case 1071:
				var.vt = VT_I4;
				var.lVal = ptr->resval.rlong;
				SafeArrayPutElement(Values, &nCount, &var);
				break;
			}
		ptr = ptr->rbnext;
		nCount++;
		}	

	(*TypeArray).vt = VT_ARRAY | VT_I2;
	(*TypeArray).parray = Types;
	(*DataArray).vt = VT_ARRAY | VT_VARIANT;
	(*DataArray).parray = Values;

	if ( prbExtendedData != NULL )
		{
		sds_relrb( prbExtendedData );
		prbExtendedData = NULL;
		}

	return NOERROR;
	}

STDMETHODIMP CAutoDbItem::SetXData (VARIANT TypeArray, VARIANT DataArray)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	SAFEARRAY FAR* arrayType;
	SAFEARRAY FAR* arrayData;

	//get the array of types
	if (TypeArray.vt == (VT_VARIANT|VT_BYREF))
		{
		if (!(TypeArray.pvarVal->vt & VT_ARRAY))
			return E_INVALIDARG;

		arrayType = TypeArray.pvarVal->parray;
		}
	else if (TypeArray.vt & VT_ARRAY)
		arrayType = TypeArray.parray;
	else
		return E_INVALIDARG;
	
	//get the array of data
	if (DataArray.vt == (VT_VARIANT|VT_BYREF))
		{
		if (!(DataArray.pvarVal->vt & VT_ARRAY))
			return E_INVALIDARG;

		arrayData = DataArray.pvarVal->parray;
		}
	else if (DataArray.vt & VT_ARRAY)
		arrayData = DataArray.parray;
	else
		return E_INVALIDARG;
	

	HRESULT hr;
	long LBound, UBound;
	hr = SafeArrayGetLBound(arrayType, 1, &LBound);
	if (FAILED(hr))
		return E_INVALIDARG;
	
	hr = SafeArrayGetUBound(arrayType, 1, &UBound);
	if (FAILED(hr))
		return E_INVALIDARG;
	
	struct resbuf *prb, *headrb;
	
	prb = headrb = sds_buildlist (-3,0);
	
	long i;
	short type;
	VARIANT var;
	for (i = LBound; i <= UBound; i++)
		{
		
		SafeArrayGetElement(arrayType, &i, &type);
		switch (type)
			{
			case 1000:	//String
			case 1001:	//Application name
			case 1002:	//Control string
			case 1003:	//Layer name
			case 1005:	//*Database handle
				{
				SafeArrayGetElement(arrayData, &i, &var);
				ASSERT (var.vt == VT_BSTR);
				CString str(var.bstrVal);
				prb = prb->rbnext = sds_newrb(type);
				prb->resval.rstring = new char [str.GetLength()+1]; 
				memset(prb->resval.rstring,0,str.GetLength()+1);
				strncpy(prb->resval.rstring, str, str.GetLength());

				if (type == 1001)
					{
					//ensure the app name is registered
					db_appidtabrec *pTableRecord = NULL;
					pTableRecord = (db_appidtabrec *)m_pDoc->m_dbDrawing->findtabrec( DB_APPIDTAB, str.GetBuffer(0), 1 );
					if (pTableRecord == NULL)
						{
						if (!ic_isvalidname(str.GetBuffer(0)))
							return E_INVALIDARG;

						db_handitem * hip = new db_appidtabrec();

						if (!hip->set_2 (str.GetBuffer(0)))
							return E_FAIL;

						if (m_pDoc->m_dbDrawing->addhanditem (hip) != 0)
							return E_FAIL;
						}
					}
				}
				break;
			case 1010:
			case 1020:
			case 1030:
			case 1011:
			case 1021:
			case 1031:
			case 1012:
			case 1022:
			case 1032:
			case 1013:
			case 1023:
			case 1033:
				{
				SafeArrayGetElement(arrayData, &i, &var);
				ASSERT (var.vt == VT_DISPATCH);

				CComPtr<IIcadPoint> pPoint;
				var.pdispVal->QueryInterface (&pPoint);
				if (!pPoint)
					return E_FAIL;

				prb = prb->rbnext = sds_newrb(type);
				pPoint->get_x (&prb->resval.rpoint[0]); 
				pPoint->get_y (&prb->resval.rpoint[1]); 
				pPoint->get_z (&prb->resval.rpoint[2]); 
				}
				break;
			case 1040:
			case 1041:
			case 1042:
				{
				SafeArrayGetElement(arrayData, &i, &var);
				ASSERT (var.vt == VT_R8);
				prb = prb->rbnext = sds_newrb(type);
				prb->resval.rreal = var.dblVal; 
				}
				break;
			case 1070:
				SafeArrayGetElement(arrayData, &i, &var);
				ASSERT (var.vt == VT_I2);
				prb = prb->rbnext = sds_newrb(type);
				prb->resval.rint = (short)var.iVal; 
				break;
			case 1071:
				SafeArrayGetElement(arrayData, &i, &var);
				ASSERT (var.vt == VT_I4);
				prb = prb->rbnext = sds_newrb(type);
				prb->resval.rlong = var.lVal; 
				break;
			}
		}
	
	int ret = m_pDbHandItem->set_eed (headrb, m_pDoc->m_dbDrawing);
	sds_relrb(headrb);

	m_pDoc->SetModifiedFlag(TRUE);

	if (ret == 0)
		return NOERROR;
	else
		return E_FAIL;
	}

STDMETHODIMP CAutoDbItem::Delete ()
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	//if (m_pDoc->m_dbDrawing->delhanditem (NULL, m_pDbHandItem, 0) == 0) //this doensn't handle complex entities
	if (DeletedEntity (m_pDbHandItem))
		{
		ASSERT (0);	//should never get here - the user cannot access a deleted entity
		return S_OK;	//but it's OK, no big deal - just dont undelete it.
		}

	return m_pDoc->DeleteEntity(m_pDbHandItem);
	}



