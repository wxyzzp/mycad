/* INTELLICAD\PRJ\LIB\AUTO\AUTOATTRIBUTEDEF.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Declaration of CAutoAttributeDef - the Automation Attribute Definition object
 * 
 */ 

#ifndef __AUTOATTRIBUTEDEF_H_
#define __AUTOATTRIBUTEDEF_H_

#include "AutoEntity.h"
#include "AutoBaseDefs.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoAttributeDef
class ATL_NO_VTABLE CAutoAttributeDef : 
	public CAutoEntity,
	public CComCoClass<CAutoAttributeDef, &CLSID_AttributeDef>,
	public IDispatchImpl<IIcadAttributeDef, &IID_IIcadAttributeDef, &LIBID_IntelliCAD>
{
public:
	CAutoAttributeDef()
	{
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoAttributeDef)
		COM_INTERFACE_ENTRY(IIcadAttributeDef)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadAttributeDef)
		COM_INTERFACE_ENTRY_CHAIN(CAutoEntity)
	END_COM_MAP()

	virtual HRESULT Init (CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem);

public:
	DEFINE_IICADENTITY_METHODS
	DEFINE_IICADDBITEM_METHODS 

	// IIcadAttributeDef
	STDMETHOD(get_Normal)(IIcadVector * * ppExtrusionDirection);
	STDMETHOD(put_Normal)(IIcadVector * ExtrusionDirection);
	STDMETHOD(get_FieldLength)(int * pFieldLength);
	STDMETHOD(put_FieldLength)(int pFieldLength);
	STDMETHOD(get_Height)(double * pHeight);
	STDMETHOD(put_Height)(double pHeight);
	STDMETHOD(get_HorizontalAlignment)(HorizontalAlignment * pHorizAlign);
	STDMETHOD(put_HorizontalAlignment)(HorizontalAlignment pHorizAlign);
	STDMETHOD(get_InsertionPoint)(IIcadPoint * * ppInsertPoint);
	STDMETHOD(put_InsertionPoint)(IIcadPoint * ppInsertPoint);
	STDMETHOD(get_Mode)(AttributeMode * pMode);
	STDMETHOD(put_Mode)(AttributeMode pMode);
	STDMETHOD(get_Constant)(VARIANT_BOOL * Mode);
	STDMETHOD(put_Constant)(VARIANT_BOOL Mode);
	STDMETHOD(get_Invisible)(VARIANT_BOOL * Mode);
	STDMETHOD(put_Invisible)(VARIANT_BOOL Mode);
	STDMETHOD(get_Preset)(VARIANT_BOOL * Mode);
	STDMETHOD(put_Preset)(VARIANT_BOOL Mode);
	STDMETHOD(get_Verify)(VARIANT_BOOL * Mode);
	STDMETHOD(put_Verify)(VARIANT_BOOL Mode);
	STDMETHOD(get_ObliqueAngle)(double * pAngle);
	STDMETHOD(put_ObliqueAngle)(double pAngle);
	STDMETHOD(get_PromptString)(BSTR * pPrompt);
	STDMETHOD(put_PromptString)(BSTR pPrompt);
	STDMETHOD(get_Rotation)(double * pRotation);
	STDMETHOD(put_Rotation)(double pRotation);
	STDMETHOD(get_ScaleFactor)(double * pScaleFactor);
	STDMETHOD(put_ScaleFactor)(double pScaleFactor);
	STDMETHOD(get_StyleName)(BSTR * pStyleName);
	STDMETHOD(put_StyleName)(BSTR pStyleName);
	STDMETHOD(get_TagString)(BSTR * pTag);
	STDMETHOD(put_TagString)(BSTR pTag);
	STDMETHOD(get_Thickness)(double * pThick);
	STDMETHOD(put_Thickness)(double pThick);
	STDMETHOD(get_TextAlignmentPoint)(IIcadPoint * * ppPoint);
	STDMETHOD(put_TextAlignmentPoint)(IIcadPoint * ppPoint);
	STDMETHOD(get_TextGenerationFlag)(TextFlag * pFlags);
	STDMETHOD(put_TextGenerationFlag)(TextFlag pFlags);
	STDMETHOD(get_TextString)(BSTR * pText);
	STDMETHOD(put_TextString)(BSTR pText);
	STDMETHOD(get_VerticalAlignment)(VerticalAlignment * pVertAlign);
	STDMETHOD(put_VerticalAlignment)(VerticalAlignment pVertAlign);
};

#endif //__AUTOATTRIBUTEDEF_H_


