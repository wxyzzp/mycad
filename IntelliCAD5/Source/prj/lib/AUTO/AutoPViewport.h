/* INTELLICAD\PRJ\LIB\AUTO\AUTOPVIEWPORT.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Declaration of CAutoPViewport - the Automation Paper space Viewport object
 * 
 */ 

#ifndef __AUTOPVIEWPORT_H_
#define __AUTOPVIEWPORT_H_

#include "AutoEntity.h"
#include "AutoBaseDefs.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoPViewport
class ATL_NO_VTABLE CAutoPViewport : 
	public CAutoEntity,
	public CComCoClass<CAutoPViewport, &CLSID_PViewport>,
	public IDispatchImpl<IIcadPViewport, &IID_IIcadPViewport, &LIBID_IntelliCAD>
{
public:
	CAutoPViewport()
	{
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoPViewport)
		COM_INTERFACE_ENTRY(IIcadPViewport)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadPViewport)
		COM_INTERFACE_ENTRY_CHAIN(CAutoEntity)
	END_COM_MAP()

	virtual HRESULT Init (CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem);

public:
	DEFINE_IICADENTITY_METHODS
	DEFINE_IICADDBITEM_METHODS 

	// IIcadPViewport
	STDMETHOD(get_Center)(IIcadPoint * * ppCenter);
	STDMETHOD(put_Center)(IIcadPoint * ppCenter);
	STDMETHOD(get_Direction)(IIcadVector * * ppNormal);
	STDMETHOD(put_Direction)(IIcadVector * ppNormal);
	STDMETHOD(get_GridOn)(VARIANT_BOOL * pGridOn);
	STDMETHOD(put_GridOn)(VARIANT_BOOL pGridOn);
	STDMETHOD(get_Height)(double * pHeight);
	STDMETHOD(put_Height)(double pHeight);
	STDMETHOD(get_LensLength)(double * pLensLength);
	STDMETHOD(put_LensLength)(double pLensLength);
	STDMETHOD(get_RemoveHiddenLines)(VARIANT_BOOL * pRemoveHiddenLines);
	STDMETHOD(put_RemoveHiddenLines)(VARIANT_BOOL pRemoveHiddenLines);
	STDMETHOD(get_SnapBasePoint)(IIcadPoint * * ppSnapBasePoint);
	STDMETHOD(put_SnapBasePoint)(IIcadPoint * ppSnapBasePoint);
	STDMETHOD(get_SnapOn)(VARIANT_BOOL * pSnapOn);
	STDMETHOD(put_SnapOn)(VARIANT_BOOL pSnapOn);
	STDMETHOD(get_SnapRotationAngle)(double * pAngle);
	STDMETHOD(put_SnapRotationAngle)(double pAngle);
	STDMETHOD(get_Target)(IIcadPoint * * ppTarget);
	STDMETHOD(put_Target)(IIcadPoint * ppTarget);
	STDMETHOD(get_TwistAngle)(double * pAngle);
	STDMETHOD(put_TwistAngle)(double pAngle);
	STDMETHOD(get_UCSIconAtOrigin)(VARIANT_BOOL * pIcon);
	STDMETHOD(put_UCSIconAtOrigin)(VARIANT_BOOL pIcon);
	STDMETHOD(get_UCSIconOn)(VARIANT_BOOL * pIcon);
	STDMETHOD(put_UCSIconOn)(VARIANT_BOOL pIcon);
	STDMETHOD(get__ViewportID)(int * pID);
	STDMETHOD(get_Width)(double * pWidth);
	STDMETHOD(put_Width)(double pWidth);
	STDMETHOD(Display)(VARIANT_BOOL Status);
	STDMETHOD(GetGridSpacing)(double * Xspacing, double * Yspacing);
	STDMETHOD(GetSnapSpacing)(double * Xspacing, double * Yspacing);
	STDMETHOD(SetGridSpacing)(double Xspacing, double Yspacing);
	STDMETHOD(SetSnapSpacing)(double Xspacing, double Yspacing);
	STDMETHOD(ZoomAll)();
	STDMETHOD(ZoomCenter)(IIcadPoint * Center, double ZoomFactor);
	STDMETHOD(ZoomExtents)();
	STDMETHOD(ZoomPickWindow)();
	STDMETHOD(ZoomScaled)(double Scale, ZoomScale ScaleType);
	STDMETHOD(ZoomWindow)(IIcadPoint * Lowerleft, IIcadPoint * UpperRight);
};

#endif //__AUTOPVIEWPORT_H_


