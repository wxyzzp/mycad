// File: <DevDir>\sources\prj\lib\auto\AutoDimRotated.h
//


#pragma once


#include "AutoDimension.h"


class ATL_NO_VTABLE CAutoDimRotated :
	public CAutoDimension,
	public CComCoClass<CAutoDimRotated, &CLSID_DimRotated>,
	public IDispatchImpl<IIcadDimRotated, &IID_IIcadDimRotated, &LIBID_IntelliCAD>
{
public:

	CAutoDimRotated() {}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoDimRotated)
		COM_INTERFACE_ENTRY(IIcadDimRotated)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadDimRotated)
		COM_INTERFACE_ENTRY_CHAIN(CAutoDimension)
	END_COM_MAP()

	virtual HRESULT Init(CIcadDoc* pDoc, IDispatch* pParent, db_handitem* pDbHandItem);

	// base classes methods
	DEFINE_IICADDBITEM_METHODS
	DEFINE_IICADENTITY_METHODS
	DEFINE_IICADDIMENSION_METHODS

	// IIcadDimRotated methods
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

	DIMENSION_METHOD_RECALL(get_ExtLine1Suppress, VARIANT_BOOL*, Supress)
	DIMENSION_METHOD_RECALL(put_ExtLine1Suppress, VARIANT_BOOL, Suppress)

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

	DIMENSION_METHOD_RECALL(get_AltRoundDistance, double*, Round);
	DIMENSION_METHOD_RECALL(put_AltRoundDistance, double, Round);

	DIMENSION_METHOD_RECALL(get_AltSuppressLeadingZeroes, VARIANT_BOOL*, Supress);
	DIMENSION_METHOD_RECALL(put_AltSuppressLeadingZeroes, VARIANT_BOOL, Suppress);

	DIMENSION_METHOD_RECALL(get_AltSuppressTrailingZeroes, VARIANT_BOOL*, Supress);
	DIMENSION_METHOD_RECALL(put_AltSuppressTrailingZeroes, VARIANT_BOOL, Supress);

	DIMENSION_METHOD_RECALL(get_AltSuppressZeroInches, VARIANT_BOOL*, Supress);
	DIMENSION_METHOD_RECALL(put_AltSuppressZeroInches, VARIANT_BOOL, Supress);

	DIMENSION_METHOD_RECALL(get_AltSuppressZeroFeet, VARIANT_BOOL*, Supress);
	DIMENSION_METHOD_RECALL(put_AltSuppressZeroFeet, VARIANT_BOOL, Supress);

	DIMENSION_METHOD_RECALL(get_AltTextPrefix, BSTR*, PrefixString);
	DIMENSION_METHOD_RECALL(put_AltTextPrefix, BSTR, PrefixString);

	DIMENSION_METHOD_RECALL(get_AltTextSuffix, BSTR*, SuffixString);
	DIMENSION_METHOD_RECALL(put_AltTextSuffix, BSTR, SuffixString);

	DIMENSION_METHOD_RECALL(get_AltTolerancePrecision, DimPrecision*, Precision);
	DIMENSION_METHOD_RECALL(put_AltTolerancePrecision, DimPrecision, Precision);

	DIMENSION_METHOD_RECALL(get_AltToleranceSuppressLeadingZeroes, VARIANT_BOOL*, Supress);
	DIMENSION_METHOD_RECALL(put_AltToleranceSuppressLeadingZeroes, VARIANT_BOOL, Supress);

	DIMENSION_METHOD_RECALL(get_AltToleranceSuppressTrailingZeroes, VARIANT_BOOL*, Supress);
	DIMENSION_METHOD_RECALL(put_AltToleranceSuppressTrailingZeroes, VARIANT_BOOL, Supress);

	DIMENSION_METHOD_RECALL(get_AltToleranceSuppressZeroInches, VARIANT_BOOL*, Supress);
	DIMENSION_METHOD_RECALL(put_AltToleranceSuppressZeroInches, VARIANT_BOOL, Supress);

	DIMENSION_METHOD_RECALL(get_AltToleranceSuppressZeroFeet, VARIANT_BOOL*, Supress);
	DIMENSION_METHOD_RECALL(put_AltToleranceSuppressZeroFeet, VARIANT_BOOL, Supress);

	DIMENSION_METHOD_RECALL(get_AltUnits, VARIANT_BOOL*, Enable);
	DIMENSION_METHOD_RECALL(put_AltUnits, VARIANT_BOOL, Enable);

	DIMENSION_METHOD_RECALL(get_AltUnitsFormat, DimLUnits*, Units);
	DIMENSION_METHOD_RECALL(put_AltUnitsFormat, DimLUnits, Units);

	DIMENSION_METHOD_RECALL(get_AltUnitsPrecision, DimPrecision*, Precision);
	DIMENSION_METHOD_RECALL(put_AltUnitsPrecision, DimPrecision, Precision);

	DIMENSION_METHOD_RECALL(get_AltUnitsScale, double*, Scale);
	DIMENSION_METHOD_RECALL(put_AltUnitsScale, double, Scale);

	DIMENSION_METHOD_RECALL(get_DimensionLineExtend, double*, pScaleFactor);
	DIMENSION_METHOD_RECALL(put_DimensionLineExtend, double, ScaleFactor);

	DIMENSION_METHOD_RECALL(get_FractionFormat, DimFractionType*, Format);
	DIMENSION_METHOD_RECALL(put_FractionFormat, DimFractionType, Format);

	DIMENSION_METHOD_RECALL(get_LinearScaleFactor, double*, ScaleFactor);
	DIMENSION_METHOD_RECALL(put_LinearScaleFactor, double, ScaleFactor);

	DIMENSION_METHOD_RECALL(get_PrimaryUnitsPrecision, DimPrecision*, Precision);
	DIMENSION_METHOD_RECALL(put_PrimaryUnitsPrecision, DimPrecision, Precision);

	DIMENSION_METHOD_RECALL(get_RoundDistance, double*, Round);
	DIMENSION_METHOD_RECALL(put_RoundDistance, double, Round);

	DIMENSION_METHOD_RECALL(get_SuppressZeroInches, VARIANT_BOOL*, Supress);
	DIMENSION_METHOD_RECALL(put_SuppressZeroInches, VARIANT_BOOL, Supress);

	DIMENSION_METHOD_RECALL(get_SuppressZeroFeet, VARIANT_BOOL*, Supress);
	DIMENSION_METHOD_RECALL(put_SuppressZeroFeet, VARIANT_BOOL, Supress);
};
