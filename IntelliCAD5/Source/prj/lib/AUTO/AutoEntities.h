/* INTELLICAD\PRJ\LIB\AUTO\AUTOENTITIES.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Declaration of CAutoEntities
 * This is a helper class that is the base class for all objects that implement IIcadEntities
 * CAutoModelSpace, CAutoPaperSpace, CAutoBlock and CAutoReferenceFile all derive from this.
 * 
 */ 

#ifndef __AUTOENTITIES_H_
#define __AUTOENTITIES_H_

#include "AutoDoc.h"
class db_handitem;
#include "AutoEntity.h"


/////////////////////////////////////////////////////////////////////////////
// CAutoEntities
class ATL_NO_VTABLE CAutoEntities : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IIcadEntities, &IID_IIcadEntities, &LIBID_IntelliCAD>
{

public:
	CAutoEntities()
		{
		}
	~CAutoEntities()
		{
		}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoEntities)
		COM_INTERFACE_ENTRY(IIcadEntities)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadEntities)
	END_COM_MAP()

	virtual HRESULT AddItemToDrawing (db_handitem *pDbHandItem, BOOL complex = FALSE) = 0;
	virtual CIcadDoc * GetDocument () = 0;	//return the document in which this collection exists
	virtual HRESULT GetOwner (IDispatch **ppOwner) = 0; //return a pointer to the owner - MSpace, PSpace, Block or XRef
	HRESULT CreateEntityWrapper (db_handitem *pDbHandItem, IIcadEntity **ppEntity);
	virtual BOOL OwnerIsModelSpace() = 0;
	virtual BOOL OwnerIsPaperSpace() = 0;
	virtual BOOL OwnerIsBlock() = 0;

	void setComplexEntitySpaceFlag (db_handitem *pDbHandItem, BOOL bFlag);

public:
	//  IIcadEntities
	STDMETHOD(get_Application)(IIcadApplication * * ppApp);
	STDMETHOD(get_Count)(long * pCount);
	STDMETHOD(get_Parent)(IIcadDocument * * ppParent);
	STDMETHOD(get__NewEnum)(IUnknown * * ppUnk);
	STDMETHOD(Item)(long Index, IDispatch * * ppItem);
	STDMETHOD(Add3DFace)(IIcadPoint * Point1, IIcadPoint * Point2, IIcadPoint * Point3, IIcadPoint * Point4, IIcad3DFace * * pp3DFace);
	STDMETHOD(Add3DMesh)(long M, long N, IIcadPoints * Vertices, IIcadPolygonMesh * * ppMesh);
	STDMETHOD(Add3DPoly)(IIcadPoints * Vertices, IIcad3DPoly * * pp3DPoly);
	STDMETHOD(AddArc)(IIcadPoint * Center, double Radius, double StartAngle, double EndAngle, IIcadArc * * ppArc);
	STDMETHOD(AddAttributeDef)(double Height, AttributeMode Mode, BSTR Prompt, IIcadPoint * InsertionPoint, BSTR Tag, BSTR Value, IIcadAttributeDef * * ppAttribute);
	STDMETHOD(AddBox)(IIcadPoint * Origin, double Length, double Width, double Height, IIcadPolygonMesh * * ppBox);
	STDMETHOD(AddCircle)(IIcadPoint * Center, double Radius, IIcadCircle * * ppCircle);
	STDMETHOD(AddCone)(IIcadPoint * Center, double BaseRadius, double Height, IIcadPolygonMesh * * ppCone);
	STDMETHOD(AddCylinder)(IIcadPoint * Center, double Radius, double Height, IIcadPolygonMesh * * ppCylinder);
	STDMETHOD(AddDim3PointAngular)(IIcadPoint *AngleVertex, IIcadPoint *FirstEndPoint, IIcadPoint *SecondEndPoint, IIcadPoint *TextLocation, IIcadDim3PointAngular **ppDimension);
	STDMETHOD(AddDimAligned)(IIcadPoint *ExtPoint1, IIcadPoint *ExtPoint2, IIcadPoint *TextLocation, IIcadDimAligned **ppDimension);
	STDMETHOD(AddDimAngular)(IIcadPoint *AngleVertex, IIcadPoint *FirstEndPoint, IIcadPoint *SecondEndPoint, IIcadPoint *TextLocation, IIcadDimAngular **ppDimension);
	STDMETHOD(AddDimDiametric)(IIcadPoint *ChordPoint, IIcadPoint *FarChordPoint, double LeaderLength, IIcadDimDiametric **ppDimension);
	STDMETHOD(AddDimOrdinate) (IIcadPoint *DefintionPoint, IIcadPoint *LeaderEndPoint, VARIANT_BOOL UseXAxis, IIcadDimOrdinate **ppDimension);
	STDMETHOD(AddDimRadial)(IIcadPoint *Center, IIcadPoint *ChordPoint, double LeaderLength, IIcadDimRadial **ppDimension);
	STDMETHOD(AddDimRotated)(IIcadPoint *ExtPoint1, IIcadPoint *ExtPoint2, IIcadPoint *DimLineLocation, double Rotation, IIcadDimRotated **ppDimension);
	STDMETHOD(AddEllipse)(IIcadPoint * Center, IIcadVector * MajorAxis, double RadiusRatio, IIcadEllipse * * ppEllipse);
	STDMETHOD(AddHatch)(HatchType PatternType, BSTR PatternName, VARIANT_BOOL Associativity, IIcadHatch * * ppHatch);
	STDMETHOD(AddImage)(BSTR ImageFile, IIcadPoint * InsertionPoint, double ScaleFactor, double RotationAngle, IIcadImage * * ppImage);
	STDMETHOD(AddInfiniteLine)(IIcadPoint * Point1, IIcadPoint * Point2, IIcadInfiniteLine * * ppInfiniteLine);
	STDMETHOD(AddLeader)(IIcadPoints * Points, BSTR Annotation, LeaderType Type, IIcadLeader * * ppLeader);
	STDMETHOD(AddLightWeightPolyline)(IIcadPoints * Vertices, IIcadLightWeightPolyline * * ppLWPolyline);
	STDMETHOD(AddLine)(IIcadPoint * StartPoint, IIcadPoint * EndPoint, IIcadLine * * ppLine);
	STDMETHOD(AddMText)(IIcadPoint * InsertionPoint, double Width, BSTR Text, IIcadMText * * ppMText);
	STDMETHOD(AddPointEntity)(IIcadPoint * Point, IIcadPointEntity * * ppPointEntity);
	STDMETHOD(AddPolyline)(IIcadPoints * Vertices, IIcadPolyline * * ppPolyline);
	STDMETHOD(AddPolyfaceMesh)(IIcadPoints * Vertices, VARIANT Faces, IIcadPolyfaceMesh * * ppMesh);
	STDMETHOD(AddRay)(IIcadPoint * Point1, IIcadPoint * Point2, IIcadRay * * ppRay);
	STDMETHOD(AddSolid)(IIcadPoint * Point1, IIcadPoint * Point2, IIcadPoint * Point3, IIcadPoint * Point4, IIcadSolid * * ppSolid);
	STDMETHOD(AddSphere)(IIcadPoint * Center, double Radius, IIcadPolygonMesh * * ppSphere);
	STDMETHOD(AddSpline)(IIcadPoints * * Points, IIcadVector * StartTangent, IIcadVector * EndTangent, IIcadSpline * * ppSpline);
	STDMETHOD(AddText)(BSTR TextString, IIcadPoint * InsertionPoint, double Height, IIcadText * * ppText);
	STDMETHOD(AddTolerance)(BSTR Text, IIcadPoint * InsertionPoint, IIcadVector * Direction, IIcadTolerance * * ppTolerance);
	STDMETHOD(AddTorus)(IIcadPoint * Center, double TorusRadius, double TubeRadius, IIcadPolygonMesh * * pTorus);
	STDMETHOD(AddTrace)(IIcadPoints * Points, IIcadTrace * * ppTrace);
	STDMETHOD(AddWedge)(IIcadPoint * Origin, double Length, double Width, double Height, IIcadPolygonMesh * * ppWedge);
	STDMETHOD(AttachExternalReference)(BSTR PathName, BSTR Name, IIcadPoint * InsertionPoint, double XScale, double YScale, double ZScale,double Rotation, VARIANT_BOOL bOverlay, IIcadExternalReference * * ppXRef);
	STDMETHOD(InsertBlock)(IIcadPoint * InsertionPoint, BSTR BlockName, double XScale, double YScale, double ZScale,double Rotation, IIcadBlockInsert * * ppBlockInsert);
	// EBATECH(CNBR) 2002/4/24 Support SHAPE
	STDMETHOD(AddShape)(BSTR Name, IIcadPoint * InsertionPoint, double Height, double Rotation, IIcadShape * * ppShape);

};



#endif //__AUTOENTITIES_H_


