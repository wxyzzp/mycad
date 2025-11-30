/* INTELLICAD\PRJ\LIB\AUTO\AUTOSPLINE.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Declaration of CAutoSpline - the Automation Spline object
 * 
 */ 

#ifndef __AUTOSPLINE_H_
#define __AUTOSPLINE_H_

#include "AutoEntity.h"
#include "AutoBaseDefs.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoSpline
class ATL_NO_VTABLE CAutoSpline : 
	public CAutoEntity,
	public CComCoClass<CAutoSpline, &CLSID_Spline>,
	public IDispatchImpl<IIcadSpline, &IID_IIcadSpline, &LIBID_IntelliCAD>
{
public:
	CAutoSpline()
	{
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoSpline)
		COM_INTERFACE_ENTRY(IIcadSpline)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadSpline)
		COM_INTERFACE_ENTRY_CHAIN(CAutoEntity)
	END_COM_MAP()

	virtual HRESULT Init (CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem);

public:
	DEFINE_IICADENTITY_METHODS
	DEFINE_IICADDBITEM_METHODS 

	// IIcadSpline
	STDMETHOD(get_Area)(double * pArea);
	STDMETHOD(get_Length)(double * pLength);
	STDMETHOD(get_Closed)(VARIANT_BOOL * bClosed);
	STDMETHOD(put_Closed)(VARIANT_BOOL bClosed);
	STDMETHOD(get_Degree)(int * pDegree);
	STDMETHOD(get_EndTangent)(IIcadVector * * ppTangent);
	STDMETHOD(put_EndTangent)(IIcadVector * ppTangent);
	STDMETHOD(get_FitTolerance)(double * Tolerance);
	STDMETHOD(put_FitTolerance)(double Tolerance);
	STDMETHOD(get_IsRational)(VARIANT_BOOL * pRational);
	STDMETHOD(get_StartTangent)(IIcadVector * * ppTangent);
	STDMETHOD(put_StartTangent)(IIcadVector * ppTangent);
	STDMETHOD(get_NumberOfControlPoints)(int * pNumControlPoints);
	STDMETHOD(get_NumberOfFitPoints)(int * pNumFitPoints);
	STDMETHOD(get_ControlPoints)(IIcadPoints * * ppCoords);
	STDMETHOD(put_ControlPoints)(IIcadPoints * ppCoords);
	STDMETHOD(get_FitPoints)(IIcadPoints * * ppCoords);
	STDMETHOD(put_FitPoints)(IIcadPoints * ppCoords);
	STDMETHOD(AddFitPoint)(int Index, IIcadPoint * FitPoint);
	STDMETHOD(DeleteFitPoint)(int Index);
	STDMETHOD(ElevateOrder)(long Order);
	STDMETHOD(GetControlPoint)(int Index, IIcadPoint * * ppControlPoint);
	STDMETHOD(GetFitPoint)(int Index, IIcadPoint * * ppFitPoint);
	STDMETHOD(GetWeight)(int Index, double * pWeight);
	STDMETHOD(Offset)(double Offset, IIcadSelectionSet * * ppArrayOfEntities);
	STDMETHOD(PurgeFitData)();
	STDMETHOD(Reverse)();
	STDMETHOD(SetControlPoint)(int Index, IIcadPoint * ControlPoint);
	STDMETHOD(SetFitPoint)(int Index, IIcadPoint * FitPoint);
	STDMETHOD(SetWeight)(int Index, double Weight);
};

#endif //__AUTOSPLINE_H_


