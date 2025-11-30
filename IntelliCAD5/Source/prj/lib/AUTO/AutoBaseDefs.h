// File: <DevDir>\sources\prj\lib\auto\AutoBaseDefs.h
/* Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * Helper macros 
 */ 


#pragma once


#define RETURN_IF_INVALID(p)  if(!p) return E_POINTER;


/*DG - 26.12.2001*/// Let me explain these huge macros.
// We don't create instances of our COM classes directly; instead, we use CComObject.
// Therefore we can use ATL_NO_VTABLE (i.e. __declspec(novtable)) for optimization.
// Since we use ATL_NO_VTABLE then we should define ALL derived methods of base classes
// and we do that by simple recalling.
// EBATECH(CNBR) 2002/4/24 Support SHAPE

//////////////////////////////
// IIcadDbItem methods
#define DBITEM_METHOD_RECALL(method, type, arg) \
	STDMETHOD(method)(type arg)\
	{  return CAutoDbItem::method(arg);  }

#define DEFINE_IICADDBITEM_METHODS \
	\
	DBITEM_METHOD_RECALL(get_Application, IIcadApplication**, ppApp)\
	DBITEM_METHOD_RECALL(get_Document, IIcadDocument**, ppDoc)\
	DBITEM_METHOD_RECALL(get_Parent, IDispatch**, ppParent)\
	DBITEM_METHOD_RECALL(get_Handle, BSTR*, pstrHandle)\
	DBITEM_METHOD_RECALL(_handitem, long*, pHanditem)\
	\
	STDMETHOD(GetXData)(BSTR AppName, VARIANT* TypeArray, VARIANT* DataArray)\
	{  return CAutoDbItem::GetXData(AppName, TypeArray, DataArray);  }\
	\
	STDMETHOD(SetXData)(VARIANT TypeArray, VARIANT DataArray)\
	{  return CAutoDbItem::SetXData(TypeArray, DataArray);  }\
	\
	STDMETHOD(Delete)()\
	{  return CAutoDbItem::Delete();  }



//////////////////////////////
// IIcadEntity methods
#define ENTITY_METHOD_RECALL(method, type, arg) \
	STDMETHOD(method)(type arg)\
	{  return CAutoEntity::method(arg);  }

#define DEFINE_IICADENTITY_METHODS \
	\
	ENTITY_METHOD_RECALL(get_Color, Colors*, pColor)\
	ENTITY_METHOD_RECALL(put_Color, Colors, pColor)\
	ENTITY_METHOD_RECALL(get_EntityName, BSTR*, pEntName)\
	ENTITY_METHOD_RECALL(get_EntityType, EntityType*, pType)\
	ENTITY_METHOD_RECALL(get_Layer, BSTR*, pLayer)\
	ENTITY_METHOD_RECALL(put_Layer, BSTR, pLayer)\
	ENTITY_METHOD_RECALL(get_Linetype, BSTR*, pLinetype)\
	ENTITY_METHOD_RECALL(put_Linetype, BSTR, pLinetype)\
	ENTITY_METHOD_RECALL(get_LinetypeScale, double*, LinetypeScale)\
	ENTITY_METHOD_RECALL(put_LinetypeScale, double, LinetypeScale)\
	ENTITY_METHOD_RECALL(get_Visible, VARIANT_BOOL*, pVisible)\
	ENTITY_METHOD_RECALL(put_Visible, VARIANT_BOOL, pVisible)\
	ENTITY_METHOD_RECALL(Copy, IIcadEntity**, ppCopy)\
	ENTITY_METHOD_RECALL(Highlight, VARIANT_BOOL, HighlightFlag)\
	ENTITY_METHOD_RECALL(TransformBy, IIcadMatrix*, Matrix)\
	\
	STDMETHOD(ArrayPolar)(int NumberOfObjects, double IncludedAngle, IIcadPoint* CenterPoint, IIcadPoint* PointOnAxis, VARIANT_BOOL RotateObjects, IIcadSelectionSet** ppObjects)\
	{  return CAutoEntity::ArrayPolar(NumberOfObjects, IncludedAngle, CenterPoint, PointOnAxis, RotateObjects, ppObjects);  }\
	\
	STDMETHOD(ArrayRectangular)(int NumberOfRows, int NumberOfColumns, int NumberOfLevels, double GapBetweenRows, double GapBetweenColumns, double GapBetweenLevels, IIcadSelectionSet** ppObjects)\
	{  return CAutoEntity::ArrayRectangular(NumberOfRows, NumberOfColumns, NumberOfLevels, GapBetweenRows, GapBetweenColumns, GapBetweenLevels, ppObjects);  }\
	\
	STDMETHOD(Erase)()\
	{  return CAutoEntity::Erase();  }\
	\
	STDMETHOD(GetBoundingBox)(IIcadPoint** MinPoint, IIcadPoint** MaxPoint)\
	{  return CAutoEntity::GetBoundingBox(MinPoint, MaxPoint);  }\
	\
	STDMETHOD(IntersectWith)(IDispatch* IntersectingObject, IntersectOptions Option, IIcadPoints** ppArrayPoints)\
	{  return CAutoEntity::IntersectWith(IntersectingObject, Option, ppArrayPoints);  }\
	\
	STDMETHOD(Mirror)(IIcadPoint* Point1, IIcadPoint* Point2, VARIANT_BOOL MakeCopy, IIcadEntity** MirroredEntity)\
	{  return CAutoEntity::Mirror(Point1, Point2, MakeCopy, MirroredEntity);  }\
	\
	STDMETHOD(Mirror3D)(IIcadPoint* Point1, IIcadPoint* Point2, IIcadPoint* Point3, VARIANT_BOOL MakeCopy, IIcadEntity** MirroredEntity)\
	{  return CAutoEntity::Mirror3D(Point1, Point2, Point3, MakeCopy, MirroredEntity);  }\
	\
	STDMETHOD(Move)(IIcadPoint* Point1, IIcadPoint* Point2)\
	{  return CAutoEntity::Move(Point1, Point2);  }\
	\
	STDMETHOD(Rotate)(IIcadPoint* BasePoint, double RotationAngle)\
	{  return CAutoEntity::Rotate(BasePoint, RotationAngle);  }\
	\
	STDMETHOD(Rotate3D)(IIcadPoint* Point1, IIcadPoint* Point2, double RotationAngle)\
	{  return CAutoEntity::Rotate3D(Point1, Point2, RotationAngle);  }\
	\
	STDMETHOD(ScaleEntity)(IIcadPoint* BasePoint, double ScaleFactor)\
	{  return CAutoEntity::ScaleEntity(BasePoint, ScaleFactor);  }\
	\
	STDMETHOD(Update)()\
	{  return CAutoEntity::Update();  }


//////////////////////////////
// IIcadEntities methods
#define DEFINE_IICADENTITIES_METHODS \
	\
	STDMETHOD(get_Application)(IIcadApplication** ppApp);\
	STDMETHOD(get_Count)(long* pCount);\
	STDMETHOD(get_Parent)(IIcadDocument** ppParent);\
	STDMETHOD(get__NewEnum)(IUnknown** ppUnk);\
	STDMETHOD(Item)(long Index, IDispatch** ppItem);\
	\
	STDMETHOD(Add3DFace)(IIcadPoint* Point1, IIcadPoint* Point2, IIcadPoint* Point3, IIcadPoint* Point4, IIcad3DFace** pp3DFace)\
	{  return CAutoEntities::Add3DFace(Point1, Point2, Point3, Point4, pp3DFace);  }\
	\
	STDMETHOD(Add3DMesh)(long M, long N, IIcadPoints* Vertices, IIcadPolygonMesh** ppMesh)\
	{  return CAutoEntities::Add3DMesh(M, N, Vertices, ppMesh);  }\
	\
	STDMETHOD(Add3DPoly)(IIcadPoints* Vertices, IIcad3DPoly** pp3DPoly)\
	{  return CAutoEntities::Add3DPoly(Vertices, pp3DPoly);  }\
	\
	STDMETHOD(AddArc)(IIcadPoint* Center, double Radius, double StartAngle, double EndAngle, IIcadArc** ppArc)\
	{  return CAutoEntities::AddArc(Center, Radius, StartAngle, EndAngle, ppArc);  }\
	\
	STDMETHOD(AddAttributeDef)(double Height, AttributeMode Mode, BSTR Prompt, IIcadPoint* InsertionPoint, BSTR Tag, BSTR Value, IIcadAttributeDef** ppAttribute)\
	{  return CAutoEntities::AddAttributeDef(Height, Mode, Prompt, InsertionPoint, Tag, Value, ppAttribute);  }\
	\
	STDMETHOD(AddBox)(IIcadPoint* Origin, double Length, double Width, double Height, IIcadPolygonMesh** ppBox)\
	{  return CAutoEntities::AddBox(Origin, Length, Width, Height, ppBox);  }\
	\
	STDMETHOD(AddCircle)(IIcadPoint* Center, double Radius, IIcadCircle** ppCircle)\
	{  return CAutoEntities::AddCircle(Center, Radius, ppCircle);  }\
	\
	STDMETHOD(AddCone)(IIcadPoint* Center, double BaseRadius, double Height, IIcadPolygonMesh** ppCone)\
	{  return CAutoEntities::AddCone(Center, BaseRadius, Height, ppCone);  }\
	\
	STDMETHOD(AddCylinder)(IIcadPoint* Center, double Height, double Radius, IIcadPolygonMesh** ppCylinder)\
	{  return CAutoEntities::AddCylinder(Center, Height, Radius, ppCylinder);  }\
	\
	STDMETHOD(AddDim3PointAngular)(IIcadPoint* AngleVertex, IIcadPoint* FirstEndPoint, IIcadPoint* SecondEndPoint, IIcadPoint* TextLocation, IIcadDim3PointAngular** ppDimension)\
	{  return CAutoEntities::AddDim3PointAngular(AngleVertex, FirstEndPoint, SecondEndPoint, TextLocation, ppDimension);  }\
	\
	STDMETHOD(AddDimAligned)(IIcadPoint* ExtPoint1, IIcadPoint* ExtPoint2, IIcadPoint* TextLocation, IIcadDimAligned** ppDimension)\
	{  return CAutoEntities::AddDimAligned(ExtPoint1, ExtPoint2, TextLocation, ppDimension);  }\
	\
	STDMETHOD(AddDimAngular)(IIcadPoint* AngleVertex, IIcadPoint* FirstEndPoint, IIcadPoint* SecondEndPoint, IIcadPoint* TextLocation, IIcadDimAngular** ppDimension)\
	{  return CAutoEntities::AddDimAngular(AngleVertex, FirstEndPoint, SecondEndPoint, TextLocation, ppDimension);  }\
	\
	STDMETHOD(AddDimDiametric)(IIcadPoint* ChordPoint, IIcadPoint* FarChordPoint, double LeaderLength, IIcadDimDiametric** ppDimension)\
	{  return CAutoEntities::AddDimDiametric(ChordPoint, FarChordPoint, LeaderLength, ppDimension);  }\
	\
	STDMETHOD(AddDimOrdinate)(IIcadPoint* DefintionPoint, IIcadPoint* LeaderEndPoint, VARIANT_BOOL UseXAxis, IIcadDimOrdinate** ppDimension)\
	{  return CAutoEntities::AddDimOrdinate(DefintionPoint, LeaderEndPoint, UseXAxis, ppDimension);  }\
	\
	STDMETHOD(AddDimRadial)(IIcadPoint* Center, IIcadPoint* ChordPoint, double LeaderLength, IIcadDimRadial** ppDimension)\
	{  return CAutoEntities::AddDimRadial(Center, ChordPoint, LeaderLength, ppDimension);  }\
	\
	STDMETHOD(AddDimRotated)(IIcadPoint* ExtPoint1, IIcadPoint* ExtPoint2, IIcadPoint *DimLineLocation, double Rotation, IIcadDimRotated** ppDimension)\
	{  return CAutoEntities::AddDimRotated(ExtPoint1, ExtPoint2, DimLineLocation, Rotation, ppDimension);  }\
	\
	STDMETHOD(AddEllipse)(IIcadPoint* Center, IIcadVector* MajorAxis, double RadiusRatio, IIcadEllipse** ppEllipse)\
	{  return CAutoEntities::AddEllipse(Center, MajorAxis, RadiusRatio, ppEllipse);  }\
	\
	STDMETHOD(AddHatch)(HatchType PatternType, BSTR PatternName, VARIANT_BOOL Associativity, IIcadHatch** ppHatch)\
	{  return CAutoEntities::AddHatch(PatternType, PatternName, Associativity, ppHatch);  }\
	\
	STDMETHOD(AddImage)(BSTR ImageFile, IIcadPoint* InsertionPoint, double ScaleFactor, double RotationAngle, IIcadImage** ppImage)\
	{  return CAutoEntities::AddImage(ImageFile, InsertionPoint, ScaleFactor, RotationAngle, ppImage);  }\
	\
	STDMETHOD(AddInfiniteLine)(IIcadPoint* Point1, IIcadPoint* Point2, IIcadInfiniteLine** ppInfiniteLine)\
	{  return CAutoEntities::AddInfiniteLine(Point1, Point2, ppInfiniteLine);  }\
	\
	STDMETHOD(AddLeader)(IIcadPoints* Points, BSTR Annotation, LeaderType Type, IIcadLeader** ppLeader)\
	{  return CAutoEntities::AddLeader(Points, Annotation, Type, ppLeader);  }\
	\
	STDMETHOD(AddLightWeightPolyline)(IIcadPoints* Vertices, IIcadLightWeightPolyline** ppLWPolyline)\
	{  return CAutoEntities::AddLightWeightPolyline(Vertices, ppLWPolyline);  }\
	\
	STDMETHOD(AddLine)(IIcadPoint* StartPoint, IIcadPoint* EndPoint, IIcadLine** ppLine)\
	{  return CAutoEntities::AddLine(StartPoint, EndPoint, ppLine);  }\
	\
	STDMETHOD(AddMText)(IIcadPoint* InsertionPoint, double Width, BSTR Text, IIcadMText** ppMText)\
	{  return CAutoEntities::AddMText(InsertionPoint, Width, Text, ppMText);  }\
	\
	STDMETHOD(AddPointEntity)(IIcadPoint* Point, IIcadPointEntity** ppPointEntity)\
	{  return CAutoEntities::AddPointEntity(Point, ppPointEntity);  }\
	\
	STDMETHOD(AddPolyline)(IIcadPoints* Vertices, IIcadPolyline** ppPolyline)\
	{  return CAutoEntities::AddPolyline(Vertices, ppPolyline);  }\
	\
	STDMETHOD(AddPolyfaceMesh)(IIcadPoints* Vertices, VARIANT Faces, IIcadPolyfaceMesh** ppMesh)\
	{  return CAutoEntities::AddPolyfaceMesh(Vertices, Faces, ppMesh);  }\
	\
	STDMETHOD(AddRay)(IIcadPoint* Point1, IIcadPoint* Point2, IIcadRay** ppRay)\
	{  return CAutoEntities::AddRay(Point1, Point2, ppRay);  }\
	\
	STDMETHOD(AddSolid)(IIcadPoint* Point1, IIcadPoint* Point2, IIcadPoint* Point3, IIcadPoint* Point4, IIcadSolid** ppSolid)\
	{  return CAutoEntities::AddSolid(Point1, Point2, Point3, Point4, ppSolid);  }\
	\
	STDMETHOD(AddSphere)(IIcadPoint* Center, double Radius, IIcadPolygonMesh** ppSphere)\
	{  return CAutoEntities::AddSphere(Center, Radius, ppSphere);  }\
	\
	STDMETHOD(AddSpline)(IIcadPoints** Points, IIcadVector* StartTangent, IIcadVector* EndTangent, IIcadSpline** ppSpline)\
	{  return CAutoEntities::AddSpline(Points, StartTangent, EndTangent, ppSpline);  }\
	\
	STDMETHOD(AddText)(BSTR TextString, IIcadPoint* InsertionPoint, double Height, IIcadText** ppText)\
	{  return CAutoEntities::AddText(TextString, InsertionPoint, Height, ppText);  }\
	\
	STDMETHOD(AddTolerance)(BSTR Text, IIcadPoint* InsertionPoint, IIcadVector* Direction, IIcadTolerance** ppTolerance)\
	{  return CAutoEntities::AddTolerance(Text, InsertionPoint, Direction, ppTolerance);  }\
	\
	STDMETHOD(AddTorus)(IIcadPoint* Center, double TorusRadius, double TubeRadius, IIcadPolygonMesh** pTorus)\
	{  return CAutoEntities::AddTorus(Center, TorusRadius, TubeRadius, pTorus);  }\
	\
	STDMETHOD(AddTrace)(IIcadPoints* Points, IIcadTrace** ppTrace)\
	{  return CAutoEntities::AddTrace(Points, ppTrace);  }\
	\
	STDMETHOD(AddWedge)(IIcadPoint* Origin, double Length, double Width, double Height, IIcadPolygonMesh** ppWedge)\
	{  return CAutoEntities::AddWedge(Origin, Length, Width, Height, ppWedge);  }\
	\
	STDMETHOD(AttachExternalReference)(BSTR PathName, BSTR Name, IIcadPoint* InsertionPoint, double XScale, double YScale, double ZScale, double Rotation, VARIANT_BOOL bOverlay, IIcadExternalReference** ppXRef)\
	{  return CAutoEntities::AttachExternalReference(PathName, Name, InsertionPoint, XScale, YScale, ZScale, Rotation, bOverlay, ppXRef);  }\
	\
	STDMETHOD(InsertBlock)(IIcadPoint* InsertionPoint, BSTR BlockName, double XScale, double YScale, double ZScale, double Rotation, IIcadBlockInsert** ppBlockInsert)\
	{  return CAutoEntities::InsertBlock(InsertionPoint, BlockName, XScale, YScale, ZScale, Rotation, ppBlockInsert);  }\
	\
	STDMETHOD(AddShape)(BSTR Name, IIcadPoint* InsertionPoint, double Height, double Rotation, IIcadShape** ppShape)\
	{  return CAutoEntities::AddShape(Name, InsertionPoint, Height, Rotation, ppShape);  }

//////////////////////////////
// IIcadTableItem methods
#define DEFINE_IICADTABLEITEM_METHODS \
	\
	STDMETHOD(get_Name)(BSTR* pName)\
	{  return CAutoTableItem::get_Name(pName);  }\
	\
	STDMETHOD(put_Name)(BSTR Name)\
	{  return CAutoTableItem::put_Name(Name);  }


//////////////////////////////
// IIcadDimension methods
#define DIMENSION_METHOD_RECALL(method, type, arg) \
	STDMETHOD(method)(type arg)\
	{  return CAutoDimension::method(arg);  }

#define DEFINE_IICADDIMENSION_METHODS \
	\
	DIMENSION_METHOD_RECALL(get_ArrowheadSize, double*, pArrowheadSize)\
	DIMENSION_METHOD_RECALL(put_ArrowheadSize, double, arrowheadSize)\
	\
	DIMENSION_METHOD_RECALL(get_DecimalSeparator, BSTR*, pDecimalSeparator)\
	DIMENSION_METHOD_RECALL(put_DecimalSeparator, BSTR, decimalSeparator)\
	\
	DIMENSION_METHOD_RECALL(get_ExtensionLineOffset, double*, pOffset)\
	DIMENSION_METHOD_RECALL(put_ExtensionLineOffset, double, offset)\
	\
	DIMENSION_METHOD_RECALL(get_Measurement, double*, pValue)\
	DIMENSION_METHOD_RECALL(put_Measurement, double, value)\
	\
	DIMENSION_METHOD_RECALL(get_Normal, IIcadVector**, ppExtrusionDirection)\
	DIMENSION_METHOD_RECALL(put_Normal, IIcadVector*, pExtrusionDirection)\
	\
	DIMENSION_METHOD_RECALL(get_Rotation, double*, pRotation)\
	DIMENSION_METHOD_RECALL(put_Rotation, double, rotation)\
	\
	DIMENSION_METHOD_RECALL(get_ScaleFactor, double*, pScaleFactor)\
	DIMENSION_METHOD_RECALL(put_ScaleFactor, double, scaleFactor)\
	\
	DIMENSION_METHOD_RECALL(get_StyleName, BSTR*, pStyleName)\
	DIMENSION_METHOD_RECALL(put_StyleName, BSTR, styleName)\
	\
	DIMENSION_METHOD_RECALL(get_SuppressLeadingZeroes, VARIANT_BOOL*, pSuppress)\
	DIMENSION_METHOD_RECALL(put_SuppressLeadingZeroes, VARIANT_BOOL, suppress)\
	\
	DIMENSION_METHOD_RECALL(get_SuppressTrailingZeroes, VARIANT_BOOL*, pSuppress)\
	DIMENSION_METHOD_RECALL(put_SuppressTrailingZeroes, VARIANT_BOOL, suppress)\
	\
	DIMENSION_METHOD_RECALL(get_TextColor, Colors*, pColor)\
	DIMENSION_METHOD_RECALL(put_TextColor, Colors, color)\
	\
	DIMENSION_METHOD_RECALL(get_TextGap, double*, pGap)\
	DIMENSION_METHOD_RECALL(put_TextGap, double, gap)\
	\
	DIMENSION_METHOD_RECALL(get_TextHeight, double*, pHeight)\
	DIMENSION_METHOD_RECALL(put_TextHeight, double, height)\
	\
	DIMENSION_METHOD_RECALL(get_TextMovement, DimTextMovement*, pMovement)\
	DIMENSION_METHOD_RECALL(put_TextMovement, DimTextMovement, movement)\
	\
	DIMENSION_METHOD_RECALL(get_TextPosition, IIcadPoint**, ppPosition)\
	DIMENSION_METHOD_RECALL(put_TextPosition, IIcadPoint*, pPosition)\
	\
	DIMENSION_METHOD_RECALL(get_TextOverride, BSTR*, pString)\
	DIMENSION_METHOD_RECALL(put_TextOverride, BSTR, string)\
	\
	DIMENSION_METHOD_RECALL(get_TextPrefix, BSTR*, pPrefixString)\
	DIMENSION_METHOD_RECALL(put_TextPrefix, BSTR, prefixString)\
	\
	DIMENSION_METHOD_RECALL(get_TextString, BSTR*, pString)\
	DIMENSION_METHOD_RECALL(put_TextString, BSTR, string)\
	\
	DIMENSION_METHOD_RECALL(get_TextRotation, double*, pRotation)\
	DIMENSION_METHOD_RECALL(put_TextRotation, double, rotation)\
	\
	DIMENSION_METHOD_RECALL(get_TextStyle, BSTR*, pStyleName)\
	DIMENSION_METHOD_RECALL(put_TextStyle, BSTR, styleName)\
	\
	DIMENSION_METHOD_RECALL(get_TextSuffix, BSTR*, pSuffixString)\
	DIMENSION_METHOD_RECALL(put_TextSuffix, BSTR, suffixString)\
	\
	DIMENSION_METHOD_RECALL(get_ToleranceDisplay, DimToleranceMethod*, pTolDisplay)\
	DIMENSION_METHOD_RECALL(put_ToleranceDisplay, DimToleranceMethod, tolDisplay)\
	\
	DIMENSION_METHOD_RECALL(get_ToleranceHeightScale, double*, pHeightScale)\
	DIMENSION_METHOD_RECALL(put_ToleranceHeightScale, double, heightScale)\
	\
	DIMENSION_METHOD_RECALL(get_ToleranceJustification, DimToleranceJustify*, pTolJustify)\
	DIMENSION_METHOD_RECALL(put_ToleranceJustification, DimToleranceJustify, tolJustify)\
	\
	DIMENSION_METHOD_RECALL(get_ToleranceLowerLimit, double*, pLimit)\
	DIMENSION_METHOD_RECALL(put_ToleranceLowerLimit, double, limit)\
	\
	DIMENSION_METHOD_RECALL(get_ToleranceSuppressLeadingZeroes, VARIANT_BOOL*, pSuppress)\
	DIMENSION_METHOD_RECALL(put_ToleranceSuppressLeadingZeroes, VARIANT_BOOL, suppress)\
	\
	DIMENSION_METHOD_RECALL(get_ToleranceSuppressTrailingZeroes, VARIANT_BOOL*, pSuppress)\
	DIMENSION_METHOD_RECALL(put_ToleranceSuppressTrailingZeroes, VARIANT_BOOL, suppress)\
	\
	DIMENSION_METHOD_RECALL(get_ToleranceSuppressZeroInches, VARIANT_BOOL*, pSuppress)\
	DIMENSION_METHOD_RECALL(put_ToleranceSuppressZeroInches, VARIANT_BOOL, suppress)\
	\
	DIMENSION_METHOD_RECALL(get_ToleranceSuppressZeroFeet, VARIANT_BOOL*, pSuppress)\
	DIMENSION_METHOD_RECALL(put_ToleranceSuppressZeroFeet, VARIANT_BOOL, suppress)\
	\
	DIMENSION_METHOD_RECALL(get_ToleranceUpperLimit, double*, pLimit)\
	DIMENSION_METHOD_RECALL(put_ToleranceUpperLimit, double, limit)\
	\
	DIMENSION_METHOD_RECALL(get_TolerancePrecision, DimPrecision*, pPrecision)\
	DIMENSION_METHOD_RECALL(put_TolerancePrecision, DimPrecision, precision)\
	\
	DIMENSION_METHOD_RECALL(get_UnitsFormat, DimLUnits*, pUnits)\
	DIMENSION_METHOD_RECALL(put_UnitsFormat, DimLUnits, units)\
	\
	DIMENSION_METHOD_RECALL(get_VerticalTextPosition, DimVerticalJustification*, pPosition)\
	DIMENSION_METHOD_RECALL(put_VerticalTextPosition, DimVerticalJustification, position)
