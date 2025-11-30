/* INTELLICAD\PRJ\LIB\AUTO\AUTOELLIPSE.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Declaration of CAutoEllipse - the Automation Ellipse object
 * 
 */ 

#ifndef __AUTOELLIPSE_H_
#define __AUTOELLIPSE_H_

#include "AutoEntity.h"
#include "AutoBaseDefs.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoEllipse
class ATL_NO_VTABLE CAutoEllipse : 
	public CAutoEntity,
	public CComCoClass<CAutoEllipse, &CLSID_Ellipse>,
	public IDispatchImpl<IIcadEllipse, &IID_IIcadEllipse, &LIBID_IntelliCAD>
{
public:
	CAutoEllipse()
	{
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoEllipse)
		COM_INTERFACE_ENTRY(IIcadEllipse)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadEllipse)
		COM_INTERFACE_ENTRY_CHAIN(CAutoEntity)
	END_COM_MAP()

	virtual HRESULT Init (CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem);

public:
	DEFINE_IICADENTITY_METHODS
	DEFINE_IICADDBITEM_METHODS 

	// IIcadEllipse
	STDMETHOD(get_Area)(double * pArea);
	STDMETHOD(get_Length)(double * pLength);
	STDMETHOD(get_Center)(IIcadPoint * * ppCenter);
	STDMETHOD(put_Center)(IIcadPoint * ppCenter);
	STDMETHOD(get_EndAngle)(double * pAngle);
	STDMETHOD(put_EndAngle)(double pAngle);
	STDMETHOD(get_EndParameter)(double * pParam);
	STDMETHOD(put_EndParameter)(double pParam);
	STDMETHOD(get_EndPoint)(IIcadPoint * * ppEndPoint);
	STDMETHOD(get_Normal)(IIcadVector * * ppExtrusionDirection);
	STDMETHOD(put_Normal)(IIcadVector * ExtrusionDirection);
	STDMETHOD(get_MajorAxis)(IIcadVector * * ppMajorAxis);
	STDMETHOD(put_MajorAxis)(IIcadVector * ppMajorAxis);
	STDMETHOD(get_MinorAxis)(IIcadVector * * ppMinorAxis);
	STDMETHOD(get_RadiusRatio)(double * pRadius);
	STDMETHOD(put_RadiusRatio)(double pRadius);
	STDMETHOD(get_StartAngle)(double * pAngle);
	STDMETHOD(put_StartAngle)(double pAngle);
	STDMETHOD(get_StartParameter)(double * pParam);
	STDMETHOD(put_StartParameter)(double pParam);
	STDMETHOD(get_StartPoint)(IIcadPoint * * ppStartPoint);
	STDMETHOD(Offset)(double Offset, IIcadSelectionSet * * ppArrayOfEntities);
};

#endif //__AUTOELLIPSE_H_


