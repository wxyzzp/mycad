/* INTELLICAD\PRJ\LIB\AUTO\AUTOXREF.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * $Revision: 1.3 $ $Date: 2001/03/16 21:48:12 $ 
 * 
 * Abstract
 * 
 * Declaration of CAutoExternalReference - the Automation External Reference object
 * 
 */ 

#ifndef __AUTOEXTERNALREFERENCE_H_
#define __AUTOEXTERNALREFERENCE_H_

#include "AutoEntity.h"
#include "AutoBaseDefs.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoExternalReference
class ATL_NO_VTABLE CAutoExternalReference : 
	public CAutoEntity,
	public CComCoClass<CAutoExternalReference, &CLSID_ExternalReference>,
	public IDispatchImpl<IIcadExternalReference, &IID_IIcadExternalReference, &LIBID_IntelliCAD>
{
public:
	CAutoExternalReference()
	{
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoExternalReference)
		COM_INTERFACE_ENTRY(IIcadExternalReference)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadExternalReference)
		COM_INTERFACE_ENTRY_CHAIN(CAutoEntity)
	END_COM_MAP()

	virtual HRESULT Init (CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem);

public:
	DEFINE_IICADENTITY_METHODS
	DEFINE_IICADDBITEM_METHODS 

	// IIcadExternalReference
	STDMETHOD(get_InsertionPoint)(IIcadPoint * * ppInsertPoint);
	STDMETHOD(put_InsertionPoint)(IIcadPoint * ppInsertPoint);
	STDMETHOD(get_Name)(BSTR * Name);
	STDMETHOD(put_Name)(BSTR Name);
	STDMETHOD(get_Normal)(IIcadVector * * ppExtrusionDirection);
	STDMETHOD(put_Normal)(IIcadVector * ppExtrusionDirection);
	STDMETHOD(get_HasAttributes)(VARIANT_BOOL * pbHasAttributes);
	STDMETHOD(get_Path)(BSTR * Path);
	STDMETHOD(put_Path)(BSTR Path);
	STDMETHOD(get_Rotation)(double * pRotation);
	STDMETHOD(put_Rotation)(double pRotation);
	STDMETHOD(get_XScaleFactor)(double * pScaleFactor);
	STDMETHOD(put_XScaleFactor)(double pScaleFactor);
	STDMETHOD(get_YScaleFactor)(double * pScaleFactor);
	STDMETHOD(put_YScaleFactor)(double pScaleFactor);
	STDMETHOD(get_ZScaleFactor)(double * pScaleFactor);
	STDMETHOD(put_ZScaleFactor)(double pScaleFactor);
	STDMETHOD(GetAttributes)(IIcadAttributes * * ppAttributes);
	
};

#endif //__AUTOEXTERNALREFERENCE_H_


