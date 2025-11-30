/* INTELLICAD\PRJ\LIB\AUTO\AUTORAY.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Declaration of CAutoRay - the Automation Ray object
 * 
 */ 

#ifndef __AUTORAY_H_
#define __AUTORAY_H_

#include "AutoEntity.h"
#include "AutoBaseDefs.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoRay
class ATL_NO_VTABLE CAutoRay : 
	public CAutoEntity,
	public CComCoClass<CAutoRay, &CLSID_Ray>,
	public IDispatchImpl<IIcadRay, &IID_IIcadRay, &LIBID_IntelliCAD>
{
public:
	CAutoRay()
	{
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoRay)
		COM_INTERFACE_ENTRY(IIcadRay)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadRay)
		COM_INTERFACE_ENTRY_CHAIN(CAutoEntity)
	END_COM_MAP()

	virtual HRESULT Init (CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem);

public:
	DEFINE_IICADENTITY_METHODS
	DEFINE_IICADDBITEM_METHODS 

	// IIcadRay
	STDMETHOD(get_BasePoint)(IIcadPoint * * ppSnapBasePoint);
	STDMETHOD(put_BasePoint)(IIcadPoint * ppSnapBasePoint);
	STDMETHOD(get_DirectionVector)(IIcadVector * * ppDirection);
	STDMETHOD(put_DirectionVector)(IIcadVector * ppDirection);
	STDMETHOD(Offset)(double Offset, IIcadSelectionSet * * ppArrayOfEntities);
};

#endif //__AUTORAY_H_


