/* INTELLICAD\PRJ\LIB\AUTO\AUTOSOLID.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Declaration of CAutoSolid - the Automation Solid object
 * 
 */ 

#ifndef __AUTOSOLID_H_
#define __AUTOSOLID_H_

#include "AutoEntity.h"
#include "AutoBaseDefs.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoSolid
class ATL_NO_VTABLE CAutoSolid : 
	public CAutoEntity,
	public CComCoClass<CAutoSolid, &CLSID_Solid>,
	public IDispatchImpl<IIcadSolid, &IID_IIcadSolid, &LIBID_IntelliCAD>
{
public:
	CAutoSolid()
	{
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoSolid)
		COM_INTERFACE_ENTRY(IIcadSolid)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadSolid)
		COM_INTERFACE_ENTRY_CHAIN(CAutoEntity)
	END_COM_MAP()

	virtual HRESULT Init (CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem);

public:
	DEFINE_IICADENTITY_METHODS
	DEFINE_IICADDBITEM_METHODS 

	// IIcadSolid
	STDMETHOD(get_Coordinates)(IIcadPoints * * ppCoords);
	STDMETHOD(put_Coordinates)(IIcadPoints * ppCoords);
	STDMETHOD(get_Normal)(IIcadVector * * ppExtrusionDirection);
	STDMETHOD(put_Normal)(IIcadVector * ExtrusionDirection);
	STDMETHOD(get_Thickness)(double * pThick);
	STDMETHOD(put_Thickness)(double pThick);
};

#endif //__AUTOSOLID_H_


