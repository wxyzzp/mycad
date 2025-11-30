// File: <DevDir>\sources\prj\lib\auto\AutoDimRadial.h
//


#pragma once


#include "AutoDimension.h"


class ATL_NO_VTABLE CAutoDimRadial :
	public CAutoDimension,
	public CComCoClass<CAutoDimRadial, &CLSID_DimRadial>,
	public IDispatchImpl<IIcadDimRadial, &IID_IIcadDimRadial, &LIBID_IntelliCAD>
{
public:

	CAutoDimRadial() {}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoDimRadial)
		COM_INTERFACE_ENTRY(IIcadDimRadial)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadDimRadial)
		COM_INTERFACE_ENTRY_CHAIN(CAutoDimension)
	END_COM_MAP()

	virtual HRESULT Init(CIcadDoc* pDoc, IDispatch* pParent, db_handitem* pDbHandItem);

	// base classes methods
	DEFINE_IICADDBITEM_METHODS
	DEFINE_IICADENTITY_METHODS
	DEFINE_IICADDIMENSION_METHODS

	// IIcadDimRadial methods
	DIMENSION_METHOD_RECALL(get_DimensionLineColor, Colors*, Color)
	DIMENSION_METHOD_RECALL(put_DimensionLineColor, Colors, Color)

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

	DIMENSION_METHOD_RECALL(get_CenterMarkSize, double*, Size);
	DIMENSION_METHOD_RECALL(put_CenterMarkSize, double, Size);

	DIMENSION_METHOD_RECALL(get_CenterType, DimCenterType*, Type);
	DIMENSION_METHOD_RECALL(put_CenterType, DimCenterType, Type);

	DIMENSION_METHOD_RECALL(get_LeaderLength, double*, LeaderLength);
	DIMENSION_METHOD_RECALL(put_LeaderLength, double, LeaderLength);

	STDMETHOD(get_ArrowheadBlock)(BSTR* BlockName);
	STDMETHOD(put_ArrowheadBlock)(BSTR BlockName);

	STDMETHOD(get_ArrowheadType)(DimArrowheadType* ArrowheadType);
	STDMETHOD(put_ArrowheadType)(DimArrowheadType ArrowheadType);

	STDMETHOD(get_DimLineSuppress)(VARIANT_BOOL* Supress);
	STDMETHOD(put_DimLineSuppress)(VARIANT_BOOL Suppress);
};
