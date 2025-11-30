/* INTELLICAD\PRJ\LIB\AUTO\AUTOLEADER.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * $Revision: 1.3 $ $Date: 2001/03/16 21:48:12 $ 
 * 
 * Abstract
 * 
 * Declaration of CAutoLeader - the Automation Leader object
 * 
 */ 

#ifndef __AUTOLEADER_H_
#define __AUTOLEADER_H_

#include "AutoEntity.h"
#include "AutoBaseDefs.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoLeader
class ATL_NO_VTABLE CAutoLeader : 
	public CAutoEntity,
	public CComCoClass<CAutoLeader, &CLSID_Leader>,
	public IDispatchImpl<IIcadLeader, &IID_IIcadLeader, &LIBID_IntelliCAD>
{
public:
	CAutoLeader()
	{
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoLeader)
		COM_INTERFACE_ENTRY(IIcadLeader)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadLeader)
		COM_INTERFACE_ENTRY_CHAIN(CAutoEntity)
	END_COM_MAP()

	virtual HRESULT Init (CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem);

public:
	DEFINE_IICADENTITY_METHODS
	DEFINE_IICADDBITEM_METHODS 

	// IIcadLeader
	STDMETHOD(get_Annotation)(BSTR * pAnnot);
	STDMETHOD(put_Annotation)(BSTR pAnnot);
	STDMETHOD(get_Coordinates)(IIcadPoints * * ppCoords);
	STDMETHOD(put_Coordinates)(IIcadPoints * ppCoords);
	STDMETHOD(get_Normal)(IIcadVector * * ppExtrusionDirection);
	STDMETHOD(put_Normal)(IIcadVector * ExtrusionDirection);;
	STDMETHOD(get_StyleName)(BSTR * pStyleName);
	STDMETHOD(put_StyleName)(BSTR pStyleName);
	STDMETHOD(get_Type)(LeaderType * pType);
	STDMETHOD(put_Type)(LeaderType pType);
	
};

#endif //__AUTOLEADER_H_


