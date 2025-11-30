/* INTELLICAD\PRJ\LIB\AUTO\AUTOINFLINE.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Declaration of CAutoInfLine - the Automation Infinite Line object
 * 
 */ 

#ifndef __AUTOINFLINE_H_
#define __AUTOINFLINE_H_

#include "AutoEntity.h"
#include "AutoBaseDefs.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoInfLine
class ATL_NO_VTABLE CAutoInfLine : 
	public CAutoEntity,
	public CComCoClass<CAutoInfLine, &CLSID_InfiniteLine>,
	public IDispatchImpl<IIcadInfiniteLine, &IID_IIcadInfiniteLine, &LIBID_IntelliCAD>
{
public:
	CAutoInfLine()
	{
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoInfLine)
		COM_INTERFACE_ENTRY(IIcadInfiniteLine)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadInfiniteLine)
		COM_INTERFACE_ENTRY_CHAIN(CAutoEntity)
	END_COM_MAP()

	virtual HRESULT Init (CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem);

public:
	DEFINE_IICADENTITY_METHODS
	DEFINE_IICADDBITEM_METHODS 

	// IIcadInfiniteLine
	STDMETHOD(get_BasePoint)(IIcadPoint * * ppSnapBasePoint);
	STDMETHOD(put_BasePoint)(IIcadPoint * ppSnapBasePoint);
	STDMETHOD(get_DirectionVector)(IIcadVector * * ppDirection);
	STDMETHOD(put_DirectionVector)(IIcadVector * ppDirection);
	STDMETHOD(Offset)(double Offset, IIcadSelectionSet * * ppArrayOfEntities);
};

#endif //__AUTOINFLINE_H_


