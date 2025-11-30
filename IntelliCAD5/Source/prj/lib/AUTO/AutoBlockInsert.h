/* INTELLICAD\PRJ\LIB\AUTO\AUTOBLOCKINSERT.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Declaration of CAutoBlockInsert - the Automation Block Insert object
 * 
 */ 

#ifndef __AUTOBLOCKINSERT_H_
#define __AUTOBLOCKINSERT_H_

#include "AutoEntity.h"
#include "AutoBaseDefs.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoBlockInsert
class ATL_NO_VTABLE CAutoBlockInsert : 
	public CAutoEntity,
	public CComCoClass<CAutoBlockInsert, &CLSID_BlockInsert>,
	public IDispatchImpl<IIcadBlockInsert, &IID_IIcadBlockInsert, &LIBID_IntelliCAD>
{
public:
	//the BlockInsert handitem is recreated when the CIcadDoc::InsertBlock fucntion is
	//called.  Hence it needs to be deleted from here....
	BOOL m_bNeedToDelete;

public:
	CAutoBlockInsert()
		{
		m_bNeedToDelete = FALSE;
		}
	~CAutoBlockInsert()
		{
		}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoBlockInsert)
		COM_INTERFACE_ENTRY(IIcadBlockInsert)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadBlockInsert)
		COM_INTERFACE_ENTRY_CHAIN(CAutoEntity)
	END_COM_MAP()

	virtual HRESULT Init (CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem);

public:
	DEFINE_IICADENTITY_METHODS
	DEFINE_IICADDBITEM_METHODS 

	// IIcadBlockInsert
	STDMETHOD(get_InsertionPoint)(IIcadPoint * * ppInsertPoint);
	STDMETHOD(put_InsertionPoint)(IIcadPoint * ppInsertPoint);
	STDMETHOD(get_Name)(BSTR * pBlockName);
	STDMETHOD(put_Name)(BSTR pBlockName);
	STDMETHOD(get_Normal)(IIcadVector * * ppExtrusionDirection);
	STDMETHOD(put_Normal)(IIcadVector * ExtrusionDirection);
	STDMETHOD(get_HasAttributes)(VARIANT_BOOL * pbHasAttributes);
	STDMETHOD(get_Rotation)(double * pRotation);
	STDMETHOD(put_Rotation)(double pRotation);
	STDMETHOD(get_XScaleFactor)(double * pScaleFactor);
	STDMETHOD(put_XScaleFactor)(double pScaleFactor);
	STDMETHOD(get_YScaleFactor)(double * pScaleFactor);
	STDMETHOD(put_YScaleFactor)(double pScaleFactor);
	STDMETHOD(get_ZScaleFactor)(double * pScaleFactor);
	STDMETHOD(put_ZScaleFactor)(double pScaleFactor);
	STDMETHOD(Explode)(IIcadSelectionSet * * pObjects);
	STDMETHOD(GetAttributes)(IIcadAttributes * * ppAttributes);
};

#endif //__AUTOBLOCKINSERT_H_


