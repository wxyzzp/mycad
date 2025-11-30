/* INTELLICAD\PRJ\LIB\AUTO\AUTOVIEWPORT.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Declaration of CAutoViewport - the Automation Viewport object
 * 
 */ 

#ifndef __AUTOVIEWPORT_H_
#define __AUTOVIEWPORT_H_

#include "AutoTableItem.h"
#include "AutoBaseDefs.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoViewport
class ATL_NO_VTABLE CAutoViewport : 
	public CAutoTableItem,
	public CComCoClass<CAutoViewport, &CLSID_Viewport>,
	public IDispatchImpl<IIcadViewport, &IID_IIcadViewport, &LIBID_IntelliCAD>
{
private:
	VARIANT_BOOL m_bOrthoOn;	//currently this does not affect the DB

public:
	CAutoViewport()
		{
		}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoViewport)
		COM_INTERFACE_ENTRY(IIcadViewport)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadViewport)
		COM_INTERFACE_ENTRY_CHAIN(CAutoTableItem)
	END_COM_MAP()

	virtual HRESULT Init (CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem);
	virtual HRESULT FindAssociatedView (CIcadView **ppFoundView);

public:
	DEFINE_IICADDBITEM_METHODS 
	//DEFINE_IICADTABLEITEM_METHODS - overridden methods
	// IIcadTableItem
	STDMETHOD(get_Name)(BSTR * pstrName);
	STDMETHOD(put_Name)(BSTR Name);

	// IIcadViewport
	STDMETHOD(get_Center)(IIcadPoint * * ppCenter);
	STDMETHOD(put_Center)(IIcadPoint * ppCenter);
	STDMETHOD(get_Direction)(IIcadVector * * ppNormal);
	STDMETHOD(put_Direction)(IIcadVector * ppNormal);
	STDMETHOD(get_GridOn)(VARIANT_BOOL * pGridOn);
	STDMETHOD(put_GridOn)(VARIANT_BOOL pGridOn);
	STDMETHOD(get_Height)(double * pHeight);
	STDMETHOD(put_Height)(double pHeight);
	STDMETHOD(get_LowerLeftCorner)(IIcadPoint * * ppLowerLeftCorner);
	STDMETHOD(put_LowerLeftCorner)(IIcadPoint * ppLowerLeftCorner);
	STDMETHOD(get_OrthoOn)(VARIANT_BOOL * pOrthoOn);
	STDMETHOD(put_OrthoOn)(VARIANT_BOOL pOrthoOn);
	STDMETHOD(get_SnapBasePoint)(IIcadPoint * * ppSnapBasePoint);
	STDMETHOD(put_SnapBasePoint)(IIcadPoint * ppSnapBasePoint);
	STDMETHOD(get_SnapOn)(VARIANT_BOOL * pSnapOn);
	STDMETHOD(put_SnapOn)(VARIANT_BOOL pSnapOn);
	STDMETHOD(get_SnapRotationAngle)(double * pAngle);
	STDMETHOD(put_SnapRotationAngle)(double pAngle);
	STDMETHOD(get_Target)(IIcadPoint * * ppTarget);
	STDMETHOD(put_Target)(IIcadPoint * ppTarget);
	STDMETHOD(get_UCSIconAtOrigin)(VARIANT_BOOL * pIcon);
	STDMETHOD(put_UCSIconAtOrigin)(VARIANT_BOOL pIcon);
	STDMETHOD(get_UCSIconOn)(VARIANT_BOOL * pIcon);
	STDMETHOD(put_UCSIconOn)(VARIANT_BOOL pIcon);
	STDMETHOD(get_UpperRightCorner)(IIcadPoint * * ppUpperRightCorner);
	STDMETHOD(put_UpperRightCorner)(IIcadPoint * ppUpperRightCorner);
	STDMETHOD(get_Width)(double * pWidth);
	STDMETHOD(put_Width)(double pWidth);
	STDMETHOD(GetGridSpacing)(double * Xspacing, double * Yspacing);
	STDMETHOD(GetSnapSpacing)(double * Xspacing, double * Yspacing);
	STDMETHOD(SetGridSpacing)(double Xspacing, double Yspacing);
	STDMETHOD(SetSnapSpacing)(double Xspacing, double Yspacing);
	STDMETHOD(SetView)(IIcadView * View);
	STDMETHOD(Split)(ViewportSplitType NumberOfWindows);
	STDMETHOD(ZoomAll)();
	STDMETHOD(ZoomCenter)(IIcadPoint * Center, double ZoomFactor);
	STDMETHOD(ZoomExtents)();
	STDMETHOD(ZoomPickWindow)();
	STDMETHOD(ZoomScaled)(double Scale, ZoomScale ScaleType);
	STDMETHOD(ZoomWindow)(IIcadPoint * Lowerleft, IIcadPoint * UpperRight);
};

#endif //__AUTOVIEWPORT_H_


