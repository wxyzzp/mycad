/* D:\INTELLICAD\PRJ\LIB\AUTO\AUTOVIEW.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Declaration of CAutoView - the Automation View object
 * 
 */ 

#ifndef __AUTOVIEW_H_
#define __AUTOVIEW_H_

#include "AutoTableItem.h"
#include "AutoBaseDefs.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoView
class ATL_NO_VTABLE CAutoView : 
	public CAutoTableItem,
	public CComCoClass<CAutoView, &CLSID_View>,
	public IDispatchImpl<IIcadView, &IID_IIcadView, &LIBID_IntelliCAD>
{
public:
	CAutoView()
	{
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoView)
		COM_INTERFACE_ENTRY(IIcadView)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadView)
		COM_INTERFACE_ENTRY_CHAIN(CAutoTableItem)
	END_COM_MAP()

	virtual HRESULT Init (CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem);

public:
	DEFINE_IICADDBITEM_METHODS 
	DEFINE_IICADTABLEITEM_METHODS

	// IIcadView
	STDMETHOD(get_Center)(IIcadPoint * * ppCenter);
	STDMETHOD(put_Center)(IIcadPoint * ppCenter);
	STDMETHOD(get_Direction)(IIcadVector * * ppNormal);
	STDMETHOD(put_Direction)(IIcadVector * ppNormal);
	STDMETHOD(get_Height)(double * pHeight);
	STDMETHOD(put_Height)(double pHeight);
	STDMETHOD(get_LensLength)(double * pLensLength);
	STDMETHOD(get_Target)(IIcadPoint * * ppTarget);
	STDMETHOD(put_Target)(IIcadPoint * ppTarget);
	STDMETHOD(get_Width)(double * pWidth);
	STDMETHOD(put_Width)(double pWidth);
};

#endif //__AUTOVIEW_H_


