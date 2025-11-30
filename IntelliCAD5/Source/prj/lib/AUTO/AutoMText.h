/* INTELLICAD\PRJ\LIB\AUTO\AUTOMTEXT.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * Declaration of CAutoMText - the Automation MText object
 *
 */

#ifndef __AUTOMTEXT_H_
#define __AUTOMTEXT_H_

#include "AutoEntity.h"
#include "AutoBaseDefs.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoMText
class ATL_NO_VTABLE CAutoMText :
	public CAutoEntity,
	public CComCoClass<CAutoMText, &CLSID_MText>,
	public IDispatchImpl<IIcadMText, &IID_IIcadMText, &LIBID_IntelliCAD>
{
public:
	CAutoMText()
	{
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoMText)
		COM_INTERFACE_ENTRY(IIcadMText)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadMText)
		COM_INTERFACE_ENTRY_CHAIN(CAutoEntity)
	END_COM_MAP()

	virtual HRESULT Init (CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem);

public:
	DEFINE_IICADENTITY_METHODS
	DEFINE_IICADDBITEM_METHODS

	// IIcadMText
	STDMETHOD(get_AttachmentPoint)(AttachmentPoint * pPoint);
	STDMETHOD(put_AttachmentPoint)(AttachmentPoint pPoint);
	STDMETHOD(get_DrawingDirection)(DrawingDirection * pDrawingDirection);
	STDMETHOD(put_DrawingDirection)(DrawingDirection pDrawingDirection);
	STDMETHOD(get_Normal)(IIcadVector * * ppExtrusionDirection);
	STDMETHOD(put_Normal)(IIcadVector * ExtrusionDirection);
	STDMETHOD(get_Height)(double * pHeight);
	STDMETHOD(put_Height)(double pHeight);
	STDMETHOD(get_InsertionPoint)(IIcadPoint * * ppInsertPoint);
	STDMETHOD(put_InsertionPoint)(IIcadPoint * ppInsertPoint);
	STDMETHOD(get_Rotation)(double * pRotation);
	STDMETHOD(put_Rotation)(double pRotation);
	STDMETHOD(get_StyleName)(BSTR * pStyleName);
	STDMETHOD(put_StyleName)(BSTR pStyleName);
	STDMETHOD(get_TextString)(BSTR * pText);
	STDMETHOD(put_TextString)(BSTR pText);
	STDMETHOD(get_Width)(double * pWidth);
	STDMETHOD(put_Width)(double pWidth);
	// EBATECH(CNBR) 04/02/2002 Add Explode
	STDMETHOD(Explode)(IIcadSelectionSet * * pObjects);
};

#endif //__AUTOMTEXT_H_


