/* INTELLICAD\PRJ\LIB\AUTO\AUTOREGAPP.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Declaration of CAutoRegisteredApplication - the Automation RegisteredApplication object
 * 
 */ 

#ifndef __AUTOREGAPP_H_
#define __AUTOREGAPP_H_

#include "AutoTableItem.h"
#include "AutoBaseDefs.h"
#include "appid.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoRegisteredApplication
class ATL_NO_VTABLE CAutoRegisteredApplication : 
	public CAutoTableItem,
	public CComCoClass<CAutoRegisteredApplication, &CLSID_RegisteredApplication>,
	public IDispatchImpl<IIcadRegisteredApplication, &IID_IIcadRegisteredApplication, &LIBID_IntelliCAD>
{
public:
	CAutoRegisteredApplication()
		{
		}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoRegisteredApplication)
		COM_INTERFACE_ENTRY(IIcadRegisteredApplication)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadRegisteredApplication)
		COM_INTERFACE_ENTRY_CHAIN(CAutoTableItem)
	END_COM_MAP()

	virtual HRESULT Init (CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem)
		{
		CAutoDbItem::Init(pDoc, pParent);
		
		if (pDbHandItem)
			m_pDbHandItem = pDbHandItem;
		else
			m_pDbHandItem = new db_appidtabrec();
		
		if (NULL == m_pDbHandItem) 
			return FALSE;
		
		return NOERROR;
		}
	
public:
	DEFINE_IICADDBITEM_METHODS 
	DEFINE_IICADTABLEITEM_METHODS

	// IIcadRegisteredApplication
};

#endif //__AUTOREGAPP_H_


