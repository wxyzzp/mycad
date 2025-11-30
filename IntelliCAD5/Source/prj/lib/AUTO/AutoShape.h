/* INTELLICAD\PRJ\LIB\AUTO\AUTOSHAPE.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Declaration of CAutoShape - the Automation Shape object
 * 
 */ 

#ifndef __AUTOSHAPE_H_
#define __AUTOSHAPE_H_

#include "AutoEntity.h"
#include "AutoBaseDefs.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoShape
class ATL_NO_VTABLE CAutoShape : 
	public CAutoEntity,
	public CComCoClass<CAutoShape, &CLSID_Shape>,
	public IDispatchImpl<IIcadShape, &IID_IIcadShape, &LIBID_IntelliCAD>
{
public:
	CAutoShape()
	{
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoShape)
		COM_INTERFACE_ENTRY(IIcadShape)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadShape)
		COM_INTERFACE_ENTRY_CHAIN(CAutoEntity)
	END_COM_MAP()

	virtual HRESULT Init (CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem);

public:
	DEFINE_IICADENTITY_METHODS
	DEFINE_IICADDBITEM_METHODS 

	// IIcadShape
	STDMETHOD(get_Normal)(IIcadVector * * ppExtrusionDirection);
	STDMETHOD(put_Normal)(IIcadVector * ExtrusionDirection);
	STDMETHOD(get_Height)(double * pHeight);
	STDMETHOD(put_Height)(double pHeight);
	STDMETHOD(get_InsertionPoint)(IIcadPoint * * ppInsPoint);
	STDMETHOD(put_InsertionPoint)(IIcadPoint * ppInsPoint);
	STDMETHOD(get_ObliqueAngle)(double * pAngle);
	STDMETHOD(put_ObliqueAngle)(double pAngle);
	STDMETHOD(get_Rotation)(double * pRotation);
	STDMETHOD(put_Rotation)(double pRotation);
	STDMETHOD(get_ScaleFactor)(double * pScale);
	STDMETHOD(put_ScaleFactor)(double pScale);
	STDMETHOD(get_Name)(BSTR * pName);
	STDMETHOD(put_Name)(BSTR pName);
	STDMETHOD(get_Thickness)(double * pThick);
	STDMETHOD(put_Thickness)(double pThick);
};

#endif //__AUTOSHAPE_H_


