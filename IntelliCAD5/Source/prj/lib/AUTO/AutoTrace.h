/* D:\INTELLICAD\PRJ\LIB\AUTO\AUTOTRACE.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Declaration of CAutoTrace - the Automation Trace object
 * 
 */ 

#ifndef __AUTOTRACE_H_
#define __AUTOTRACE_H_

#include "AutoEntity.h"
#include "AutoBaseDefs.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoTrace
class ATL_NO_VTABLE CAutoTrace : 
	public CAutoEntity,
	public CComCoClass<CAutoTrace, &CLSID_Trace>,
	public IDispatchImpl<IIcadTrace, &IID_IIcadTrace, &LIBID_IntelliCAD>
{
public:
	CAutoTrace()
	{
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoTrace)
		COM_INTERFACE_ENTRY(IIcadTrace)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadTrace)
		COM_INTERFACE_ENTRY_CHAIN(CAutoEntity)
	END_COM_MAP()

	virtual HRESULT Init (CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem);

public:
	DEFINE_IICADENTITY_METHODS
	DEFINE_IICADDBITEM_METHODS 

	// IIcadTrace
	STDMETHOD(get_Coordinates)(IIcadPoints * * ppCoords);
	STDMETHOD(put_Coordinates)(IIcadPoints * ppCoords);
	STDMETHOD(get_Normal)(IIcadVector * * ppExtrusionDirection);
	STDMETHOD(put_Normal)(IIcadVector * ExtrusionDirection);
	STDMETHOD(get_Thickness)(double * pThick);
	STDMETHOD(put_Thickness)(double pThick);
};

#endif //__AUTOTRACE_H_


