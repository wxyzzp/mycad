/* INTELLICAD\PRJ\LIB\AUTO\AUTOTABLEITEM.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Declaration of CAutoTable
 * This is the base class for all Table objects like CAutoLayer, CAutoBlock etc.
 * This is derived from CAutoDbItem.
 * 
 */ 

#ifndef __AUTOTABLEITEM_H_
#define __AUTOTABLEITEM_H_

#include "AutoDbItem.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoTableItem
class ATL_NO_VTABLE CAutoTableItem : 
	public CAutoDbItem,
	public IDispatchImpl<IIcadTableItem, &IID_IIcadTableItem, &LIBID_IntelliCAD>
{
public:
	CAutoTableItem()
		{
		}
	~CAutoTableItem()
		{
		}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoTableItem)
		COM_INTERFACE_ENTRY(IIcadTableItem)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadTableItem)
		COM_INTERFACE_ENTRY_CHAIN(CAutoDbItem)
	END_COM_MAP()

public:
	// IIcadTableItem
	STDMETHOD(get_Name)(BSTR * pstrName)
		{
		if (!m_pDbHandItem)
			return E_POINTER;

		if (pstrName == NULL)
			return E_POINTER;
		
		char* pStr;
		if ( !m_pDbHandItem->get_2 (&pStr)) 
			return E_FAIL;
		
		CString str(pStr);
		*pstrName = str.AllocSysString();
		return NOERROR;
		}
	
	STDMETHOD(put_Name)(BSTR Name)
		{
		if (!m_pDbHandItem)
			return E_POINTER;

		CString str(Name);

		if (!ic_isvalidname(str.GetBuffer(0)))
			return E_INVALIDARG;

		if (!m_pDbHandItem->set_2 (str.GetBuffer(0)))
			return E_FAIL;
		
		return NOERROR;
		}
};

#endif //__AUTOTABLEITEM_H_


