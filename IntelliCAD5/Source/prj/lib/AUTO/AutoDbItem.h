/* INTELLICAD\PRJ\LIB\AUTO\AUTODBITEM.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Declaration of CAutoDbItem - this is the root class for all database objects in the
 * Intellicad Automation model.  CAutoEntity and CAutoTable are derived from this.
 * 
 */ 


#ifndef __AUTODBITEM_H_
#define __AUTODBITEM_H_

#include "AutoDoc.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoDbItem
class ATL_NO_VTABLE CAutoDbItem : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IIcadDbItem, &IID_IIcadDbItem, &LIBID_IntelliCAD>
{

public:
	CIcadDoc *m_pDoc;
	CComPtr<IDispatch> m_pParent;
	db_handitem *m_pDbHandItem;

	CAutoDbItem()
		{
		m_pDoc = NULL;
		m_pParent = NULL;
		m_pDbHandItem = NULL;
		}
	~CAutoDbItem()
		{
		if (m_pParent)
			m_pParent = NULL;
		}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoDbItem)
		COM_INTERFACE_ENTRY(IIcadDbItem)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadDbItem)
	END_COM_MAP()

	virtual HRESULT Init (CIcadDoc *pDoc, IDispatch *pParent);

public:
// IIcadDbItem
	STDMETHOD(get_Application)(IIcadApplication * * ppApp);
	STDMETHOD(get_Document)(IIcadDocument * * ppDoc);
	STDMETHOD(get_Parent)(IDispatch * * ppParent);
	STDMETHOD(get_Handle)(BSTR * pstrHandle);
	STDMETHOD(_handitem)(long * pHanditem);
	STDMETHOD(GetXData)(BSTR AppName, VARIANT * TypeArray, VARIANT * DataArray);
	STDMETHOD(SetXData)(VARIANT TypeArray, VARIANT DataArray);
	STDMETHOD(Delete)();
};

#endif //__AUTODBITEM_H_


