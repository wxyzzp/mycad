/* INTELLICAD\PRJ\LIB\AUTO\AUTOTEXTSTYLE.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Declaration of CAutoTextStyle - the Automation TextStyle object
 * 
 */ 


#ifndef __AUTOTEXTSTYLE_H_
#define __AUTOTEXTSTYLE_H_

#include "AutoTableItem.h"
#include "AutoBaseDefs.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoTextStyle
class ATL_NO_VTABLE CAutoTextStyle : 
	public CAutoTableItem,
	public CComCoClass<CAutoTextStyle, &CLSID_TextStyle>,
	public IDispatchImpl<IIcadTextStyle, &IID_IIcadTextStyle, &LIBID_IntelliCAD>
{
public:
	CAutoTextStyle()
		{
		}
	
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoTextStyle)
		COM_INTERFACE_ENTRY(IIcadTextStyle)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadTextStyle)
		COM_INTERFACE_ENTRY_CHAIN(CAutoTableItem)
	END_COM_MAP()

	virtual HRESULT Init (CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem);

public:
	// EBATECH(CNBR) -[ 2002/5/11 Extruct macro expect SexXdata()
	//	DEFINE_IICADDBITEM_METHODS 
	DBITEM_METHOD_RECALL(get_Application, IIcadApplication**, ppApp)
	DBITEM_METHOD_RECALL(get_Document, IIcadDocument**, ppDoc)
	DBITEM_METHOD_RECALL(get_Parent, IDispatch**, ppParent)
	DBITEM_METHOD_RECALL(get_Handle, BSTR*, pstrHandle)
	DBITEM_METHOD_RECALL(_handitem, long*, pHanditem)
	STDMETHOD(GetXData)(BSTR AppName, VARIANT* TypeArray, VARIANT* DataArray)
	{  return CAutoDbItem::GetXData(AppName, TypeArray, DataArray);  }
	STDMETHOD(Delete)()
	{  return CAutoDbItem::Delete();  }
	// EBATECH(CNBR) ]-

	DEFINE_IICADTABLEITEM_METHODS

	// IIcadTextStyle
	STDMETHOD(get_BigFontFile)(BSTR * pFontFile);
	STDMETHOD(put_BigFontFile)(BSTR pFontFile);
	STDMETHOD(get_FontFile)(BSTR * pFontFile);
	STDMETHOD(put_FontFile)(BSTR pFontFile);
	STDMETHOD(get_Height)(double * pHeight);
	STDMETHOD(put_Height)(double pHeight);
	STDMETHOD(get_LastHeight)(double * pHeight);
	STDMETHOD(put_LastHeight)(double pHeight);
	STDMETHOD(get_ObliqueAngle)(double * pAngle);
	STDMETHOD(put_ObliqueAngle)(double pAngle);
	STDMETHOD(get_Width)(double * pWidth);
	STDMETHOD(put_Width)(double pWidth);
	STDMETHOD(get_TextGenerationFlag)(TextFlag * pTextFlags);
	STDMETHOD(put_TextGenerationFlag)(TextFlag pTextFlags);
	// EBATECH(CNBR) 2002/4/24 Support SHAPE
	STDMETHOD(get_Shape)(VARIANT_BOOL * pShape);
	STDMETHOD(put_Shape)(VARIANT_BOOL pShape);
	// EBATECH(CNBR) 2002/5/11
	STDMETHOD(SetXData)(VARIANT TypeArray, VARIANT DataArray);
};

#endif //__AUTOTEXTSTYLE_H_


