/* INTELLICAD\PRJ\LIB\AUTO\AUTOHATCH.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * $Revision: 1.4 $ $Date: 2001/10/26 15:13:32 $ 
 * 
 * Abstract
 * 
 * Declaration of CAutoHatch - the Automation Hatch object
 * 
 */ 

#ifndef __AUTOHATCH_H_
#define __AUTOHATCH_H_

#include "AutoEntity.h"
#include "AutoBaseDefs.h"
#include "AutoEntities.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoHatch
class ATL_NO_VTABLE CAutoHatch : 
	public CAutoEntity,
	public CComCoClass<CAutoHatch, &CLSID_Hatch>,
	public IDispatchImpl<IIcadHatch, &IID_IIcadHatch, &LIBID_IntelliCAD>
{
public:
	CAutoHatch()
	{
	m_pOwner = NULL;
	m_bDeleteHanditem = FALSE;
	}
	~CAutoHatch()
	{
	if (m_bDeleteHanditem)
		{
		delete m_pDbHandItem;
		m_pDbHandItem = NULL;
		}
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoHatch)
		COM_INTERFACE_ENTRY(IIcadHatch)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadHatch)
		COM_INTERFACE_ENTRY_CHAIN(CAutoEntity)
	END_COM_MAP()

	virtual HRESULT Init (CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem);

	CComPtr<CAutoEntities> m_pOwner;
	BOOL m_bDeleteHanditem;	//this is special for Hatch.  A Hatch is created in 2 steps:
	// 1.ModelSpace.AddHatch - which creates the handitem
	// 2.Hatch.ApplyHatch - which adds the handitem to the drawing if the hatch can be applied successfully.
	// If ApplyHatch fails, the handitem is not added to the drawing and will be cleaned up in this object's
	// destructor.  The bDeleteHanditem flag is used to indicate whether this needs to be deleted.

public:
	DEFINE_IICADENTITY_METHODS
	DEFINE_IICADDBITEM_METHODS 

	// IIcadHatch
	STDMETHOD(get_AssociativeHatch)(VARIANT_BOOL * Associative);
	STDMETHOD(get_Normal)(IIcadVector * * ppExtrusionDirection);
	STDMETHOD(put_Normal)(IIcadVector * ppExtrusionDirection);
	STDMETHOD(get_HatchStyle)(HatchStyle * pHatch);
	STDMETHOD(put_HatchStyle)(HatchStyle pHatch);
	STDMETHOD(get_PatternAngle)(double * pAngle);
	STDMETHOD(put_PatternAngle)(double pAngle);
	STDMETHOD(get_PatternDouble)(VARIANT_BOOL * pPat);
	STDMETHOD(put_PatternDouble)(VARIANT_BOOL pPat);
	STDMETHOD(get_PatternName)(BSTR * pPatternName);
	STDMETHOD(put_PatternName)(BSTR pPatternName);
	STDMETHOD(get_PatternScale)(double * pScale);
	STDMETHOD(put_PatternScale)(double pScale);
	STDMETHOD(get_PatternSpace)(double * pSpace);
	STDMETHOD(put_PatternSpace)(double pSpace);
	STDMETHOD(get_PatternType)(HatchType * pType);
	STDMETHOD(put_PatternType)(HatchType pType);
	STDMETHOD(get_SolidFillFlag)(int* pSolidFillFlag);
	STDMETHOD(put_SolidFillFlag)(int solidFillFlag);
	STDMETHOD(SetPattern)(HatchType Type, BSTR PatternName);
	STDMETHOD(ApplyHatch)(IIcadSelectionSet * ObjectsToHatch, IIcadPoints *SeedPoints);
	
};

#endif //__AUTOHATCH_H_


