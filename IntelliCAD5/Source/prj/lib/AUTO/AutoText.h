/* INTELLICAD\PRJ\LIB\AUTO\AUTOTEXT.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Declaration of CAutoText - the Automation Text object
 * 
 */ 

#ifndef __AUTOTEXT_H_
#define __AUTOTEXT_H_

#include "AutoEntity.h"
#include "AutoBaseDefs.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoText
class ATL_NO_VTABLE CAutoText : 
	public CAutoEntity,
	public CComCoClass<CAutoText, &CLSID_Text>,
	public IDispatchImpl<IIcadText, &IID_IIcadText, &LIBID_IntelliCAD>
{
public:
	CAutoText()
	{
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoText)
		COM_INTERFACE_ENTRY(IIcadText)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadText)
		COM_INTERFACE_ENTRY_CHAIN(CAutoEntity)
	END_COM_MAP()

	virtual HRESULT Init (CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem);

public:
	DEFINE_IICADENTITY_METHODS
	DEFINE_IICADDBITEM_METHODS 

	// IIcadText
	STDMETHOD(get_TextAlignmentPoint)(IIcadPoint * * ppPoint);
	STDMETHOD(put_TextAlignmentPoint)(IIcadPoint * ppPoint);
	STDMETHOD(get_Normal)(IIcadVector * * ppExtrusionDirection);
	STDMETHOD(put_Normal)(IIcadVector * ExtrusionDirection);
	STDMETHOD(get_TextGenerationFlag)(TextFlag * pFlags);
	STDMETHOD(put_TextGenerationFlag)(TextFlag pFlags);
	STDMETHOD(get_Height)(double * pHeight);
	STDMETHOD(put_Height)(double pHeight);
	STDMETHOD(get_HorizontalAlignment)(HorizontalAlignment * pHorizAlign);
	STDMETHOD(put_HorizontalAlignment)(HorizontalAlignment pHorizAlign);
	STDMETHOD(get_InsertionPoint)(IIcadPoint * * ppInsPoint);
	STDMETHOD(put_InsertionPoint)(IIcadPoint * ppInsPoint);
	STDMETHOD(get_ObliqueAngle)(double * pAngle);
	STDMETHOD(put_ObliqueAngle)(double pAngle);
	STDMETHOD(get_Rotation)(double * pRotation);
	STDMETHOD(put_Rotation)(double pRotation);
	STDMETHOD(get_ScaleFactor)(double * pScale);
	STDMETHOD(put_ScaleFactor)(double pScale);
	STDMETHOD(get_StyleName)(BSTR * pStyleName);
	STDMETHOD(put_StyleName)(BSTR pStyleName);
	STDMETHOD(get_TextString)(BSTR * pText);
	STDMETHOD(put_TextString)(BSTR pText);
	STDMETHOD(get_Thickness)(double * pThick);
	STDMETHOD(put_Thickness)(double pThick);
	STDMETHOD(get_VerticalAlignment)(VerticalAlignment * pVertAlign);
	STDMETHOD(put_VerticalAlignment)(VerticalAlignment pVertAlign);
};

#endif //__AUTOTEXT_H_


