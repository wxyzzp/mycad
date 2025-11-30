/* INTELLICAD\PRJ\LIB\AUTO\AUTOLINE.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Declaration of CAutoLine - the Automation Line object
 * 
 */ 

#ifndef __AUTOLINE_H_
#define __AUTOLINE_H_

#include "AutoEntity.h"
#include "AutoBaseDefs.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoLine
class ATL_NO_VTABLE CAutoLine : 
	public CAutoEntity,
	public CComCoClass<CAutoLine, &CLSID_Line>,
	public IDispatchImpl<IIcadLine, &IID_IIcadLine, &LIBID_IntelliCAD>
{
public:
	CAutoLine()
		{
		}
	~CAutoLine()
		{
		}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoLine)
		COM_INTERFACE_ENTRY(IIcadLine)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadLine)
		COM_INTERFACE_ENTRY_CHAIN(CAutoEntity)
	END_COM_MAP()

	virtual HRESULT Init (CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem);

public:
	DEFINE_IICADENTITY_METHODS
	DEFINE_IICADDBITEM_METHODS 

	// IIcadLine
	STDMETHOD(get_EndPoint)(IIcadPoint * * ppPoint);
	STDMETHOD(put_EndPoint)(IIcadPoint * ppPoint);
	STDMETHOD(get_Normal)(IIcadVector * * ppExtrusionDirection);
	STDMETHOD(put_Normal)(IIcadVector * ExtrusionDirection);
	STDMETHOD(get_Length)(double * pLength);
	STDMETHOD(get_StartPoint)(IIcadPoint * * ppPoint);
	STDMETHOD(put_StartPoint)(IIcadPoint * ppPoint);
	STDMETHOD(get_Thickness)(double * pThick);
	STDMETHOD(put_Thickness)(double pThick);
	STDMETHOD(Offset)(double Offset, IIcadSelectionSet * * ppArrayOfEntities);
};

#endif //__AUTOLINE_H_


