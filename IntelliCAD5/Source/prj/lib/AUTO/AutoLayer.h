/* INTELLICAD\PRJ\LIB\AUTO\AUTOLAYER.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Declaration of CAutoLayer - the Automation Layer object
 * 
 */ 

#ifndef __AUTOLAYER_H_
#define __AUTOLAYER_H_

#include "AutoTableItem.h"
#include "AutoBaseDefs.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoLayer
class ATL_NO_VTABLE CAutoLayer : 
	public CAutoTableItem,
	public CComCoClass<CAutoLayer, &CLSID_Layer>,
	public IDispatchImpl<IIcadLayer, &IID_IIcadLayer, &LIBID_IntelliCAD>
{
public:
	CAutoLayer()
		{
		}
	~CAutoLayer()
		{
		}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoLayer)
		COM_INTERFACE_ENTRY(IIcadLayer)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadLayer)
		COM_INTERFACE_ENTRY_CHAIN(CAutoTableItem)
	END_COM_MAP()

	virtual HRESULT Init (CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem);

public:
	DEFINE_IICADDBITEM_METHODS 
	DEFINE_IICADTABLEITEM_METHODS

	// IIcadLayer
	STDMETHOD(get_Color)(Colors * pColor);
	STDMETHOD(put_Color)(Colors pColor);
	STDMETHOD(get_Freeze)(VARIANT_BOOL * pFreeze);
	STDMETHOD(put_Freeze)(VARIANT_BOOL pFreeze);
	STDMETHOD(get_LayerOn)(VARIANT_BOOL * pOn);
	STDMETHOD(put_LayerOn)(VARIANT_BOOL pOn);
	STDMETHOD(get_Linetype)(BSTR * pLinetype);
	STDMETHOD(put_Linetype)(BSTR pLinetype);
	STDMETHOD(get_Lock)(SHORT * pLock);
	STDMETHOD(put_Lock)(SHORT pLock);
};

#endif //__AUTOLAYER_H_


