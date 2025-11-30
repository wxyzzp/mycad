// File: <DevDir>\sources\prj\lib\auto\AutoDim3PointAngular.h
//


#pragma once


#include "AutoDimension.h"


class ATL_NO_VTABLE CAutoDim3PointAngular :
	public CAutoDimension,
	public CComCoClass<CAutoDim3PointAngular, &CLSID_Dim3PointAngular>,
	public IDispatchImpl<IIcadDim3PointAngular, &IID_IIcadDim3PointAngular, &LIBID_IntelliCAD>
{
public:

	CAutoDim3PointAngular() {}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoDim3PointAngular)
		COM_INTERFACE_ENTRY(IIcadDim3PointAngular)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadDim3PointAngular)
		COM_INTERFACE_ENTRY_CHAIN(CAutoDimension)
	END_COM_MAP()

	virtual HRESULT Init(CIcadDoc* pDoc, IDispatch* pParent, db_handitem* pDbHandItem);

	// base classes methods
	DEFINE_IICADDBITEM_METHODS
	DEFINE_IICADENTITY_METHODS
	DEFINE_IICADDIMENSION_METHODS

	// IIcadDim3PointAngular methods
	DIMENSION_METHOD_RECALL(get_AngleFormat, AngleUnits*, Movement)
	DIMENSION_METHOD_RECALL(put_AngleFormat, AngleUnits, Movement)

	DIMENSION_METHOD_RECALL(get_Arrowhead1Block, BSTR*, BlockName)
	DIMENSION_METHOD_RECALL(put_Arrowhead1Block, BSTR, BlockName)

	DIMENSION_METHOD_RECALL(get_Arrowhead1Type, DimArrowheadType*, ArrowheadType)
	DIMENSION_METHOD_RECALL(put_Arrowhead1Type, DimArrowheadType, ArrowheadType)

	DIMENSION_METHOD_RECALL(get_Arrowhead2Block, BSTR*, BlockName)
	DIMENSION_METHOD_RECALL(put_Arrowhead2Block, BSTR, BlockName)

	DIMENSION_METHOD_RECALL(get_Arrowhead2Type, DimArrowheadType*, ArrowheadType)
	DIMENSION_METHOD_RECALL(put_Arrowhead2Type, DimArrowheadType, ArrowheadType)

	DIMENSION_METHOD_RECALL(get_DimensionLineColor, Colors*, Color)
	DIMENSION_METHOD_RECALL(put_DimensionLineColor, Colors, Color)

	DIMENSION_METHOD_RECALL(get_DimLine1Suppress, VARIANT_BOOL*, Supress)
	DIMENSION_METHOD_RECALL(put_DimLine1Suppress, VARIANT_BOOL, Suppress)

	DIMENSION_METHOD_RECALL(get_DimLine2Suppress, VARIANT_BOOL*, Supress)
	DIMENSION_METHOD_RECALL(put_DimLine2Suppress, VARIANT_BOOL, Suppress)

	DIMENSION_METHOD_RECALL(get_DimLineInside, VARIANT_BOOL*, Inside)
	DIMENSION_METHOD_RECALL(put_DimLineInside, VARIANT_BOOL, Inside)

	DIMENSION_METHOD_RECALL(get_ExtensionLineColor, Colors*, Color)
	DIMENSION_METHOD_RECALL(put_ExtensionLineColor, Colors, Color)

	DIMENSION_METHOD_RECALL(get_ExtensionLineExtend, double*, Extension)
	DIMENSION_METHOD_RECALL(put_ExtensionLineExtend, double, Extension)

	DIMENSION_METHOD_RECALL(get_ExtLine1EndPoint, IIcadPoint**, Point)
	DIMENSION_METHOD_RECALL(put_ExtLine1EndPoint, IIcadPoint*, Point)

	DIMENSION_METHOD_RECALL(get_ExtLine1Suppress, VARIANT_BOOL*, Supress)
	DIMENSION_METHOD_RECALL(put_ExtLine1Suppress, VARIANT_BOOL, Suppress)

	DIMENSION_METHOD_RECALL(get_ExtLine2EndPoint, IIcadPoint**, Point)
	DIMENSION_METHOD_RECALL(put_ExtLine2EndPoint, IIcadPoint*, Point)

	DIMENSION_METHOD_RECALL(get_ExtLine2Suppress, VARIANT_BOOL*, Supress)
	DIMENSION_METHOD_RECALL(put_ExtLine2Suppress, VARIANT_BOOL, Suppress)

	DIMENSION_METHOD_RECALL(get_Fit, DimFit*, Fit)
	DIMENSION_METHOD_RECALL(put_Fit, DimFit, Fit)

	DIMENSION_METHOD_RECALL(get_ForceLineInside, VARIANT_BOOL*, Inside)
	DIMENSION_METHOD_RECALL(put_ForceLineInside, VARIANT_BOOL, Inside)

	DIMENSION_METHOD_RECALL(get_HorizontalTextPosition, DimHorizontalJustification*, Position)
	DIMENSION_METHOD_RECALL(put_HorizontalTextPosition, DimHorizontalJustification, Position)

	DIMENSION_METHOD_RECALL(get_TextInside, VARIANT_BOOL*, Inside)
	DIMENSION_METHOD_RECALL(put_TextInside, VARIANT_BOOL, Inside)

	DIMENSION_METHOD_RECALL(get_TextInsideAlign, VARIANT_BOOL*, Alignment)
	DIMENSION_METHOD_RECALL(put_TextInsideAlign, VARIANT_BOOL, Alignment)

	DIMENSION_METHOD_RECALL(get_TextOutsideAlign, VARIANT_BOOL*, Alignment)
	DIMENSION_METHOD_RECALL(put_TextOutsideAlign, VARIANT_BOOL, Alignment)

	DIMENSION_METHOD_RECALL(get_TextPrecision, DimPrecision*, Precision)
	DIMENSION_METHOD_RECALL(put_TextPrecision, DimPrecision, Precision)

	STDMETHOD(get_AngleVertex)(IIcadPoint** Point);
	STDMETHOD(put_AngleVertex)(IIcadPoint* Point);

	STDMETHOD(get_ExtLine1StartPoint)(IIcadPoint** Point);
	STDMETHOD(put_ExtLine1StartPoint)(IIcadPoint* Point);

	STDMETHOD(get_ExtLine2StartPoint)(IIcadPoint** Point);
	STDMETHOD(put_ExtLine2StartPoint)(IIcadPoint* Point);
};
