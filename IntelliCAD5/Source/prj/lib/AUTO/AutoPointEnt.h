/* D:\INTELLICAD\PRJ\LIB\AUTO\AUTOPOINTENT.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Declaration of CAutoPointEnt - the Automation Point Entity object 
 * This is an actual database entity, not to be confused with Point which is
 * a helper object in the Library.
 * 
 */ 

#ifndef __AUTOPOINTENT_H_
#define __AUTOPOINTENT_H_

#include "AutoEntity.h"
#include "AutoBaseDefs.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoPointEnt
class ATL_NO_VTABLE CAutoPointEnt : 
	public CAutoEntity,
	public CComCoClass<CAutoPointEnt, &CLSID_PointEntity>,
	public IDispatchImpl<IIcadPointEntity, &IID_IIcadPointEntity, &LIBID_IntelliCAD>
{
public:
	CAutoPointEnt()
	{
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoPointEnt)
		COM_INTERFACE_ENTRY(IIcadPointEntity)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadPointEntity)
		COM_INTERFACE_ENTRY_CHAIN(CAutoEntity)
	END_COM_MAP()

	virtual HRESULT Init (CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem);

public:
	DEFINE_IICADENTITY_METHODS
	DEFINE_IICADDBITEM_METHODS 

	// IIcadPointEntity
	STDMETHOD(get_Coordinates)(IIcadPoints * * ppCoord);
	STDMETHOD(put_Coordinates)(IIcadPoints * ppCoord);
	STDMETHOD(get_Normal)(IIcadVector * * ppExtrusionDirection);
	STDMETHOD(put_Normal)(IIcadVector * ExtrusionDirection);
	STDMETHOD(get_Thickness)(double * pThick);
	STDMETHOD(put_Thickness)(double pThick);
};

#endif //__AUTOPOINTENT_H_


