/* INTELLICAD\PRJ\LIB\AUTO\AUTOENTITY.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Declaration of CAutoEntity
 * This is the base class for all Entity objects like CAutoLine, CAutoBlock etc.
 * This is derived from CAutoDbItem.
 * 
 */ 

#ifndef __AUTOENTITY_H_
#define __AUTOENTITY_H_

#include "AutoDbItem.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoEntity
class ATL_NO_VTABLE CAutoEntity : 
	public CAutoDbItem,
	public IDispatchImpl<IIcadEntity, &IID_IIcadEntity, &LIBID_IntelliCAD>
{
public:
	CAutoEntity()
		{
		}
	~CAutoEntity()
		{
		}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoEntity)
		COM_INTERFACE_ENTRY(IIcadEntity)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadEntity)
		COM_INTERFACE_ENTRY_CHAIN(CAutoDbItem)
	END_COM_MAP()

public:
	// IIcadEntity
	STDMETHOD(get_Color)(Colors * pColor);
	STDMETHOD(put_Color)(Colors pColor);
	STDMETHOD(get_EntityName)(BSTR * pEntName);
	STDMETHOD(get_EntityType)(EntityType * pType);
	STDMETHOD(get_Layer)(BSTR * pLayer);
	STDMETHOD(put_Layer)(BSTR pLayer);
	STDMETHOD(get_Linetype)(BSTR * pLinetype);
	STDMETHOD(put_Linetype)(BSTR pLinetype);
	STDMETHOD(get_LinetypeScale)(double * LinetypeScale);
	STDMETHOD(put_LinetypeScale)(double LinetypeScale);
	STDMETHOD(get_Visible)(VARIANT_BOOL * pVisible);
	STDMETHOD(put_Visible)(VARIANT_BOOL pVisible);
	STDMETHOD(ArrayPolar)(int NumberOfObjects, double IncludedAngle, IIcadPoint * CenterPoint, IIcadPoint* PointOnAxis, VARIANT_BOOL RotateObjects, IIcadSelectionSet * * ppObjects);
	STDMETHOD(ArrayRectangular)(int NumberOfRows, int NumberOfColumns, int NumberOfLevels, double GapBetweenRows, double GapBetweenColumns, double GapBetweenLevels, IIcadSelectionSet * * ppObjects);
	STDMETHOD(Copy)(IIcadEntity * * ppCopy);
	STDMETHOD(Erase)();
	STDMETHOD(GetBoundingBox)(IIcadPoint * * MinPoint, IIcadPoint * * MaxPoint);
	STDMETHOD(Highlight)(VARIANT_BOOL HighlightFlag);
	STDMETHOD(IntersectWith)(IDispatch * IntersectingObject, IntersectOptions Option, IIcadPoints * * ppArrayPoints);
	STDMETHOD(Mirror)(IIcadPoint * Point1, IIcadPoint * Point2, VARIANT_BOOL MakeCopy, IIcadEntity * * MirroredEntity);
	STDMETHOD(Mirror3D)(IIcadPoint * Point1, IIcadPoint * Point2, IIcadPoint * Point3, VARIANT_BOOL MakeCopy, IIcadEntity * * MirroredEntity);
	STDMETHOD(Move)(IIcadPoint * Point1, IIcadPoint * Point2);
	STDMETHOD(Rotate)(IIcadPoint * BasePoint, double RotationAngle);
	STDMETHOD(Rotate3D)(IIcadPoint * Point1, IIcadPoint * Point2, double RotationAngle);
	STDMETHOD(ScaleEntity)(IIcadPoint * BasePoint, double ScaleFactor);
	STDMETHOD(TransformBy)(IIcadMatrix * Matrix);
	STDMETHOD(Update)();
};

#endif //__AUTOENTITY_H_


