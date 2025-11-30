/* INTELLICAD\PRJ\LIB\AUTO\AUTO3DFACE.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Declaration of CAuto3DFace - the Automation 3DFace object
 * 
 */ 

#ifndef __AUTO3DFACE_H_
#define __AUTO3DFACE_H_

#include "AutoEntity.h"
#include "AutoBaseDefs.h"

/////////////////////////////////////////////////////////////////////////////
// CAuto3DFace
class ATL_NO_VTABLE CAuto3DFace : 
	public CAutoEntity,
	public CComCoClass<CAuto3DFace, &CLSID_Face3D>,
	public IDispatchImpl<IIcad3DFace, &IID_IIcad3DFace, &LIBID_IntelliCAD>
{
public:
	CAuto3DFace()
	{
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAuto3DFace)
		COM_INTERFACE_ENTRY(IIcad3DFace)
		COM_INTERFACE_ENTRY2(IDispatch, IIcad3DFace)
		COM_INTERFACE_ENTRY_CHAIN(CAutoEntity)
	END_COM_MAP()

	virtual HRESULT Init (CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem);

public:
	DEFINE_IICADENTITY_METHODS
	DEFINE_IICADDBITEM_METHODS 

	// IIcad3DFace
	STDMETHOD(get_VisibilityEdge1)(VARIANT_BOOL * bVisible);
	STDMETHOD(put_VisibilityEdge1)(VARIANT_BOOL bVisible);
	STDMETHOD(get_VisibilityEdge2)(VARIANT_BOOL * bVisible);
	STDMETHOD(put_VisibilityEdge2)(VARIANT_BOOL bVisible);
	STDMETHOD(get_VisibilityEdge3)(VARIANT_BOOL * bVisible);
	STDMETHOD(put_VisibilityEdge3)(VARIANT_BOOL bVisible);
	STDMETHOD(get_VisibilityEdge4)(VARIANT_BOOL * bVisible);
	STDMETHOD(put_VisibilityEdge4)(VARIANT_BOOL bVisible);
	STDMETHOD(get_Coordinates)(IIcadPoints * * ppCoords);
	STDMETHOD(put_Coordinates)(IIcadPoints * ppCoords);
	STDMETHOD(GetInvisibleEdge)(int Index, VARIANT_BOOL * pState);
	STDMETHOD(SetInvisibleEdge)(int Index, VARIANT_BOOL State);
};

#endif //__AUTO3DFACE_H_


