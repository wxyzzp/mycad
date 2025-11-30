/* INTELLICAD\PRJ\LIB\AUTO\AUTOARC.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Declaration of CAutoArc - the Automation Arc object
 * 
 */ 

#ifndef __AUTOARC_H_
#define __AUTOARC_H_

#include "AutoEntity.h"
#include "AutoBaseDefs.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoArc
class ATL_NO_VTABLE CAutoArc : 
	public CAutoEntity,
	public CComCoClass<CAutoArc, &CLSID_Arc>,
	public IDispatchImpl<IIcadArc, &IID_IIcadArc, &LIBID_IntelliCAD>
{
public:
	CAutoArc()
	{
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoArc)
		COM_INTERFACE_ENTRY(IIcadArc)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadArc)
		COM_INTERFACE_ENTRY_CHAIN(CAutoEntity)
	END_COM_MAP()

	virtual HRESULT Init (CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem);

public:
	DEFINE_IICADENTITY_METHODS
	DEFINE_IICADDBITEM_METHODS 

	// IIcadArc
	STDMETHOD(get_Area)(double * pArea);
	STDMETHOD(get_Center)(IIcadPoint * * ppCenter);
	STDMETHOD(put_Center)(IIcadPoint * ppCenter);
	STDMETHOD(get_EndAngle)(double * pAngle);
	STDMETHOD(put_EndAngle)(double pAngle);
	STDMETHOD(get_EndPoint)(IIcadPoint * * ppEndPoint);
	STDMETHOD(get_Normal)(IIcadVector * * ppExtrusionDirection);
	STDMETHOD(put_Normal)(IIcadVector * ExtrusionDirection);
	STDMETHOD(get_Length)(double * pLength);
	STDMETHOD(get_Radius)(double * pRadius);
	STDMETHOD(put_Radius)(double pRadius);
	STDMETHOD(get_StartAngle)(double * pAngle);
	STDMETHOD(put_StartAngle)(double pAngle);
	STDMETHOD(get_StartPoint)(IIcadPoint * * ppStartPoint);
	STDMETHOD(get_Thickness)(double * pThick);
	STDMETHOD(put_Thickness)(double pThick);
	STDMETHOD(Offset)(double Offset, IIcadSelectionSet * * ppArrayOfEntities);
};

#endif //__AUTOARC_H_


