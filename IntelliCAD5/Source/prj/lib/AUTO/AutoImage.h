/* INTELLICAD\PRJ\LIB\AUTO\AUTOIMAGE.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * $Revision: 1.3 $ $Date: 2001/03/16 21:48:11 $ 
 * 
 * Abstract
 * 
 * Declaration of CAutoImage - the Automation Image object
 * 
 */ 

#ifndef __AUTOIMAGE_H_
#define __AUTOIMAGE_H_

#include "AutoEntity.h"
#include "AutoBaseDefs.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoImage
class ATL_NO_VTABLE CAutoImage : 
	public CAutoEntity,
	public CComCoClass<CAutoImage, &CLSID_Image>,
	public IDispatchImpl<IIcadImage, &IID_IIcadImage, &LIBID_IntelliCAD>
{
public:
	CAutoImage()
	{
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoImage)
		COM_INTERFACE_ENTRY(IIcadImage)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadImage)
		COM_INTERFACE_ENTRY_CHAIN(CAutoEntity)
	END_COM_MAP()

	virtual HRESULT Init (CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem);

public:
	DEFINE_IICADENTITY_METHODS
	DEFINE_IICADDBITEM_METHODS 

	// IIcadImage
	STDMETHOD(get_ImageFile)(BSTR * pImageFileName);
	STDMETHOD(put_ImageFile)(BSTR pImageFileName);
	STDMETHOD(get_ImageVisibility)(VARIANT_BOOL * pVisible);
	STDMETHOD(put_ImageVisibility)(VARIANT_BOOL pVisible);
	STDMETHOD(get_Origin)(IIcadPoint * * ppOrigin);
	STDMETHOD(put_Origin)(IIcadPoint * ppOrigin);
	STDMETHOD(get_ImageHeight)(double * pHeight);
	STDMETHOD(put_ImageHeight)(double pHeight);
	STDMETHOD(get_ImageWidth)(double * pWidth);
	STDMETHOD(put_ImageWidth)(double pWidth);
	STDMETHOD(get_Height)(double * pHeight);
	STDMETHOD(get_Width)(double * pWidth);
	
};

#endif //__AUTOIMAGE_H_


