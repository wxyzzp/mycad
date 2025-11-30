/* INTELLICAD\PRJ\LIB\AUTO\AUTOTOLERANCE.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Declaration of CAutoTolerance - the Automation Tolerance object
 * 
 */ 

#ifndef __AUTOTOLERANCE_H_
#define __AUTOTOLERANCE_H_

#include "AutoEntity.h"
#include "AutoBaseDefs.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoTolerance
class ATL_NO_VTABLE CAutoTolerance : 
	public CAutoEntity,
	public CComCoClass<CAutoTolerance, &CLSID_Tolerance>,
	public IDispatchImpl<IIcadTolerance, &IID_IIcadTolerance, &LIBID_IntelliCAD>
{
public:
	CAutoTolerance()
		{
		}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoTolerance)
		COM_INTERFACE_ENTRY(IIcadTolerance)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadTolerance)
		COM_INTERFACE_ENTRY_CHAIN(CAutoEntity)
	END_COM_MAP()

	virtual HRESULT Init (CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem);

public:
	DEFINE_IICADENTITY_METHODS
	DEFINE_IICADDBITEM_METHODS 

	// IIcadTolerance
	STDMETHOD(get_DirectionVector)(IIcadVector * * ppDirection);
	STDMETHOD(put_DirectionVector)(IIcadVector * ppDirection);
	STDMETHOD(get_Normal)(IIcadVector * * ppExtrusionDirection);
	STDMETHOD(put_Normal)(IIcadVector * ExtrusionDirection);
	STDMETHOD(get_TextHeight)(double * pHeight);
	STDMETHOD(put_TextHeight)(double pHeight);
	STDMETHOD(get_InsertionPoint)(IIcadPoint * * ppInsertPoint);
	STDMETHOD(put_InsertionPoint)(IIcadPoint * ppInsertPoint);
	STDMETHOD(get_StyleName)(BSTR * pStyleName);
	STDMETHOD(put_StyleName)(BSTR pStyleName);
	STDMETHOD(get_TextString)(BSTR * pText);
	STDMETHOD(put_TextString)(BSTR pText);
};

#endif //__AUTOTOLERANCE_H_


