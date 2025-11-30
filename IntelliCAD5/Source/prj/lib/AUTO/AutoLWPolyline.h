/* INTELLICAD\PRJ\LIB\AUTO\AUTOLWPOLYLINE.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * $Revision: 1.4 $ $Date: 2001/03/22 16:31:40 $ 
 * 
 * Abstract
 * 
 * Declaration of CAutoLWPolyline - the Automation Lightweight polyline object
 * 
 */ 

#ifndef __AUTOLWPOLYLINE_H_
#define __AUTOLWPOLYLINE_H_

#include "AutoEntity.h"
#include "AutoBaseDefs.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoLWPolyline
class ATL_NO_VTABLE CAutoLWPolyline : 
	public CAutoEntity,
	public CComCoClass<CAutoLWPolyline, &CLSID_LWPolyline>,
	public IDispatchImpl<IIcadLightWeightPolyline, &IID_IIcadLightWeightPolyline, &LIBID_IntelliCAD>
{
public:
	CAutoLWPolyline()
	{
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoLWPolyline)
		COM_INTERFACE_ENTRY(IIcadLightWeightPolyline)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadLightWeightPolyline)
		COM_INTERFACE_ENTRY_CHAIN(CAutoEntity)
	END_COM_MAP()

	virtual HRESULT Init (CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem);

public:
	DEFINE_IICADENTITY_METHODS
	DEFINE_IICADDBITEM_METHODS 

	// IIcadLightWeightPolyline
	STDMETHOD(get_Area)(double * pArea);
	STDMETHOD(get_Length)(double * pLength);
	STDMETHOD(get_Closed)(VARIANT_BOOL * pClosed);
	STDMETHOD(put_Closed)(VARIANT_BOOL pClosed);
	STDMETHOD(get_Coordinates)(IIcadPoints * * ppCoords);
	STDMETHOD(put_Coordinates)(IIcadPoints * ppCoords);
	STDMETHOD(get_Normal)(IIcadVector * * ppExtrusionDirection);
	STDMETHOD(put_Normal)(IIcadVector * ppExtrusionDirection);
	STDMETHOD(get_Thickness)(double * pThick);
	STDMETHOD(put_Thickness)(double pThick);
	STDMETHOD(AddVertex)(int Index, IIcadPoint * Point);
	STDMETHOD(Explode)(IIcadSelectionSet * * ppArrayOfEntities);
	STDMETHOD(GetBulge)(int Index, double * pBulge);
	STDMETHOD(GetWidth)(int Index, double * StartWidth, double * EndWidth);
	STDMETHOD(Offset)(double Offset, IIcadSelectionSet * * ppArrayOfEntities);
	STDMETHOD(SetBulge)(int Index, double Bulge);
	STDMETHOD(SetWidth)(int SegmentIndex, double StartWidth, double EndWidth);

	static void FixMirroring(db_handitem* pDbHandItem);
	
};

#endif //__AUTOLWPOLYLINE_H_


