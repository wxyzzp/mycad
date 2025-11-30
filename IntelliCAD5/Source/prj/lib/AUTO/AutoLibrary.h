/* INTELLICAD\PRJ\LIB\AUTO\AUTOLIBRARY.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Declaration of the CAutoLibrary
 * 
 */ 

#ifndef __AUTOLIBRARY_H_
#define __AUTOLIBRARY_H_

/////////////////////////////////////////////////////////////////////////////
// CAutoLibrary
class ATL_NO_VTABLE CAutoLibrary : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CAutoLibrary, &CLSID_Library>,
	public IDispatchImpl<IIcadLibrary, &IID_IIcadLibrary, &LIBID_IntelliCAD>
{
public:
	CAutoLibrary()
		{
		}
	~CAutoLibrary()
		{
		}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoLibrary)
		COM_INTERFACE_ENTRY(IIcadLibrary)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadLibrary)
	END_COM_MAP()

public:
	// IIcadLibrary
	STDMETHOD(get_Application)(IIcadApplication * * Application);
	STDMETHOD(get_Parent)(IIcadApplication * * Application);
	STDMETHOD(CreatePoint)(double X1, double Y1, double Z1, IIcadPoint * * ppPoint);
	STDMETHOD(CreatePoints)(IIcadPoints * * ppPoints);
	STDMETHOD(CalculateDistance)(IIcadPoint * Point1, IIcadPoint * Point2, double * pDistance);
	STDMETHOD(CreateVector)(double X2, double Y2, double Z2, double X1, double Y1, double Z1, IIcadVector * * ppVector);
	STDMETHOD(AddVectors)(IIcadVector * Vector1, IIcadVector * Vector2, IIcadVector * * ppVector);
	STDMETHOD(SubtractVectors)(IIcadVector * Vector1, IIcadVector * Vector2, IIcadVector * * ppVector);
	STDMETHOD(VectorDotProduct)(IIcadVector * Vector1, IIcadVector * Vector2, double * pVal);
	STDMETHOD(VectorCrossProduct)(IIcadVector * Vector1, IIcadVector * Vector2, IIcadVector * * ppVector);
	STDMETHOD(VectorScalarMultiply)(IIcadVector * Vector, double Scale, IIcadVector * * ppVector);
	STDMETHOD(VectorUnitize)(IIcadVector * Vector, IIcadVector * * ppVector);
	STDMETHOD(VectorLength)(IIcadVector * Vector, double * pLength);
	STDMETHOD(DistanceBetweenVectors)(IIcadVector * Vector1, IIcadVector * Vector2, double * pVal);
	STDMETHOD(CreateIdentityMatrix)(IIcadMatrix * * ppMatrix);
	STDMETHOD(CreateRotationMatrix)(double Angle, IIcadPoint * AxisEnd, IIcadPoint * AxisStart, IIcadMatrix * * ppMatrix);
	STDMETHOD(CreateTranslationMatrix)(double x, double y, double z, IIcadMatrix * * ppMatrix);
	STDMETHOD(CreateScaleMatrix)(double x, double y, double z, IIcadMatrix * * ppMatrix);
	STDMETHOD(CreateMirrorMatrix)(VARIANT_BOOL x, VARIANT_BOOL y, VARIANT_BOOL z, IIcadMatrix * * ppMatrix);
	STDMETHOD(ComposeMatrix)(IIcadMatrix * Matrix1, IIcadMatrix * Matrix2, IIcadMatrix * * ppMatrix);
	STDMETHOD(ApplyMatrix)(IIcadMatrix * Matrix, IIcadPoint * Point, IIcadPoint * * ppPoint);
	STDMETHOD(InvertMatrix)(IIcadMatrix * Matrix, IIcadMatrix * * ppMatrix);
	STDMETHOD(ConvertArrayToPoint)(VARIANT ArrayOfDoubles, VARIANT_BOOL Is3DPoint, IIcadPoint * * Point);
	STDMETHOD(ConvertArrayToPoints)(VARIANT ArrayOfDoubles, VARIANT_BOOL Is3DPoint, IIcadPoints * * Points);
	STDMETHOD(ConvertArrayToVector)(VARIANT ArrayOfDoubles, VARIANT_BOOL Is3DVector, IIcadVector * * Vector);
	STDMETHOD(ConvertArrayToMatrix)(VARIANT ArrayOf16Doubles, IIcadMatrix * * Matrix);
	STDMETHOD(ConvertPointToArray)(IIcadPoint * Point, VARIANT_BOOL Is3DPoint, VARIANT * ArrayOfDoubles);
	STDMETHOD(ConvertPointsToArray)(IIcadPoints * Points, VARIANT_BOOL Is3DPoint, VARIANT * ArrayOfDoubles);
	STDMETHOD(ConvertVectorToArray)(IIcadVector * Vector, VARIANT_BOOL Is3DVector, VARIANT * ArrayOfDoubles);
	STDMETHOD(ConvertMatrixToArray)(IIcadMatrix * Matrix, VARIANT * ArrayOf16Doubles);
};

#endif //__AUTOLIBRARY_H_


