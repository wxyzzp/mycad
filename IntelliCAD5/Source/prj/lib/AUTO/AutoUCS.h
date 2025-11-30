/* INTELLICAD\PRJ\LIB\AUTO\AUTOUCS.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Declaration of CAutoUCS - the Automation UCS object
 * 
 */ 

#ifndef __AUTOUCS_H_
#define __AUTOUCS_H_

#include "AutoTableItem.h"
#include "AutoBaseDefs.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoUCS
class ATL_NO_VTABLE CAutoUCS : 
	public CAutoTableItem,
	public CComCoClass<CAutoUCS, &CLSID_UserCoordSystem>,
	public IDispatchImpl<IIcadUserCoordSystem, &IID_IIcadUserCoordSystem, &LIBID_IntelliCAD>
{
public:
	CAutoUCS()
	{
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoUCS)
		COM_INTERFACE_ENTRY(IIcadUserCoordSystem)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadUserCoordSystem)
		COM_INTERFACE_ENTRY_CHAIN(CAutoTableItem)
	END_COM_MAP()

	virtual HRESULT Init (CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem);

public:
	DEFINE_IICADDBITEM_METHODS 
	DEFINE_IICADTABLEITEM_METHODS

	// IIcadUserCoordSystem
	STDMETHOD(get_Origin)(IIcadPoint * * ppOrigin);
	STDMETHOD(put_Origin)(IIcadPoint * ppOrigin);
	STDMETHOD(get_Xvector)(IIcadVector * * ppVector);
	STDMETHOD(put_Xvector)(IIcadVector * ppVector);
	STDMETHOD(get_Yvector)(IIcadVector * * ppVector);
	STDMETHOD(put_Yvector)(IIcadVector * ppVector);
	STDMETHOD(get_Zvector)(IIcadVector * * ppVector);
	STDMETHOD(GetUCSMatrix)(IIcadMatrix * * ppMatrix);
};

#endif //__AUTOUCS_H_


