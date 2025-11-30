/* INTELLICAD\PRJ\LIB\AUTO\AUTOLINETYPE.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Declaration of CAutoLinetype - the Automation Linetype object
 * 
 */ 

#ifndef __AUTOLINETYPE_H_
#define __AUTOLINETYPE_H_

#include "AutoTableItem.h"
#include "AutoBaseDefs.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoLinetype
class ATL_NO_VTABLE CAutoLinetype : 
	public CAutoTableItem,
	public CComCoClass<CAutoLinetype, &CLSID_Linetype>,
	public IDispatchImpl<IIcadLinetype, &IID_IIcadLinetype, &LIBID_IntelliCAD>
{
public:
	CAutoLinetype()
		{
		}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoLinetype)
		COM_INTERFACE_ENTRY(IIcadLinetype)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadLinetype)
		COM_INTERFACE_ENTRY_CHAIN(CAutoTableItem)
	END_COM_MAP()

	virtual HRESULT Init (CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem);

public:
	DEFINE_IICADDBITEM_METHODS 
	DEFINE_IICADTABLEITEM_METHODS

	// IIcadLinetype
	STDMETHOD(get_Description)(BSTR * pDescription);
	STDMETHOD(put_Description)(BSTR pDescription);
};

#endif //__AUTOLINETYPE_H_


