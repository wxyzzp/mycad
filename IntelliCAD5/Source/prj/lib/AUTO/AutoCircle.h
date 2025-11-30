/* INTELLICAD\PRJ\LIB\AUTO\AUTOCIRCLE.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Declaration of CAutoCircle - the Automation Circle object
 * 
 */ 

#ifndef __AUTOCIRCLE_H_
#define __AUTOCIRCLE_H_

#include "AutoEntity.h"
#include "AutoBaseDefs.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoCircle
class ATL_NO_VTABLE CAutoCircle : 
	public CAutoEntity,
	public CComCoClass<CAutoCircle, &CLSID_Circle>,
	public IDispatchImpl<IIcadCircle, &IID_IIcadCircle, &LIBID_IntelliCAD>
{
public:
	CAutoCircle()
	{
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoCircle)
		COM_INTERFACE_ENTRY(IIcadCircle)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadCircle)
		COM_INTERFACE_ENTRY_CHAIN(CAutoEntity)
	END_COM_MAP()

	virtual HRESULT Init (CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem);

public:
	DEFINE_IICADENTITY_METHODS
	DEFINE_IICADDBITEM_METHODS 

	// IIcadCircle
	STDMETHOD(get_Area)(double * pArea);
	STDMETHOD(get_Circumference)(double * pCircumference);
	STDMETHOD(get_Center)(IIcadPoint * * ppCenter);
	STDMETHOD(put_Center)(IIcadPoint * ppCenter);
	STDMETHOD(get_Normal)(IIcadVector * * ppExtrusionDirection);
	STDMETHOD(put_Normal)(IIcadVector * ExtrusionDirection);
	STDMETHOD(get_Radius)(double * pRadius);
	STDMETHOD(put_Radius)(double pRadius);
	STDMETHOD(get_Thickness)(double * pThick);
	STDMETHOD(put_Thickness)(double pThick);
	STDMETHOD(Offset)(double Offset, IIcadSelectionSet * * ppArrayOfEntities);
};

#endif //__AUTOCIRCLE_H_


