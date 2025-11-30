//File: \source\prj\lib\auto\AutoDimension.h
/* Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 *
 * Abstract
 * Declaration of CAutoDimension - the Automation Dimension object
 */

#pragma once


#include "AutoEntity.h"
#include "AutoBaseDefs.h"


class ATL_NO_VTABLE CAutoDimension :
	public CAutoEntity,
	public CComCoClass<CAutoDimension, &CLSID_Dimension>,
	public IDispatchImpl<IIcadDimension, &IID_IIcadDimension, &LIBID_IntelliCAD>
{
public:

	CAutoDimension() {}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoDimension)
		COM_INTERFACE_ENTRY(IIcadDimension)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadDimension)
		COM_INTERFACE_ENTRY_CHAIN(CAutoEntity)
	END_COM_MAP()

	virtual HRESULT Init(CIcadDoc* pDoc, IDispatch* pParent, db_handitem* pDbHandItem);

	DEFINE_IICADENTITY_METHODS
	DEFINE_IICADDBITEM_METHODS

	// IIcadDimension methods
	STDMETHOD(get_ArrowheadSize)(double* pArrowheadSize);
	STDMETHOD(put_ArrowheadSize)(double arrowheadSize);

	STDMETHOD(get_DecimalSeparator)(BSTR* pDecimalSeparator);
	STDMETHOD(put_DecimalSeparator)(BSTR decimalSeparator);

	STDMETHOD(get_ExtensionLineOffset)(double* pOffset);
	STDMETHOD(put_ExtensionLineOffset)(double offset);

	STDMETHOD(get_Measurement)(double* pValue);
	STDMETHOD(put_Measurement)(double value);

	STDMETHOD(get_Normal)(IIcadVector** ppExtrusionDirection);
	STDMETHOD(put_Normal)(IIcadVector* pExtrusionDirection);

	STDMETHOD(get_Rotation)(double* pRotation);
	STDMETHOD(put_Rotation)(double rotation);

	STDMETHOD(get_ScaleFactor)(double* pScaleFactor);
	STDMETHOD(put_ScaleFactor)(double scaleFactor);

	STDMETHOD(get_StyleName)(BSTR* pStyleName);
	STDMETHOD(put_StyleName)(BSTR styleName);

	STDMETHOD(get_SuppressLeadingZeroes)(VARIANT_BOOL* pSuppress);
	STDMETHOD(put_SuppressLeadingZeroes)(VARIANT_BOOL suppress);

	STDMETHOD(get_SuppressTrailingZeroes)(VARIANT_BOOL* pSuppress);
	STDMETHOD(put_SuppressTrailingZeroes)(VARIANT_BOOL suppress);

	STDMETHOD(get_TextColor)(Colors* pColor);
	STDMETHOD(put_TextColor)(Colors color);

	STDMETHOD(get_TextGap)(double* pGap);
	STDMETHOD(put_TextGap)(double gap);

	STDMETHOD(get_TextHeight)(double* pHeight);
	STDMETHOD(put_TextHeight)(double height);

	STDMETHOD(get_TextMovement)(DimTextMovement* pMovement);
	STDMETHOD(put_TextMovement)(DimTextMovement movement);

	STDMETHOD(get_TextPosition)(IIcadPoint** ppPosition);
	STDMETHOD(put_TextPosition)(IIcadPoint* pPosition);

	STDMETHOD(get_TextOverride)(BSTR* pString);
	STDMETHOD(put_TextOverride)(BSTR string);

	STDMETHOD(get_TextPrefix)(BSTR* pPrefixString);
	STDMETHOD(put_TextPrefix)(BSTR prefixString);

	STDMETHOD(get_TextString)(BSTR* pString);
	STDMETHOD(put_TextString)(BSTR string);

	STDMETHOD(get_TextRotation)(double* pRotation);
	STDMETHOD(put_TextRotation)(double rotation);

	STDMETHOD(get_TextStyle)(BSTR* pStyleName);
	STDMETHOD(put_TextStyle)(BSTR styleName);

	STDMETHOD(get_TextSuffix)(BSTR* pSuffixString);
	STDMETHOD(put_TextSuffix)(BSTR suffixString);

	STDMETHOD(get_ToleranceDisplay)(DimToleranceMethod* pTolDisplay);
	STDMETHOD(put_ToleranceDisplay)(DimToleranceMethod tolDisplay);

	STDMETHOD(get_ToleranceHeightScale)(double* pHeightScale);
	STDMETHOD(put_ToleranceHeightScale)(double heightScale);

	STDMETHOD(get_ToleranceJustification)(DimToleranceJustify* pTolJustify);
	STDMETHOD(put_ToleranceJustification)(DimToleranceJustify tolJustify);

	STDMETHOD(get_ToleranceLowerLimit)(double* pLimit);
	STDMETHOD(put_ToleranceLowerLimit)(double limit);

	STDMETHOD(get_ToleranceSuppressLeadingZeroes)(VARIANT_BOOL* pSuppress);
	STDMETHOD(put_ToleranceSuppressLeadingZeroes)(VARIANT_BOOL suppress);

	STDMETHOD(get_ToleranceSuppressTrailingZeroes)(VARIANT_BOOL* pSuppress);
	STDMETHOD(put_ToleranceSuppressTrailingZeroes)(VARIANT_BOOL suppress);

	STDMETHOD(get_ToleranceSuppressZeroInches)(VARIANT_BOOL* pSuppress);
	STDMETHOD(put_ToleranceSuppressZeroInches)(VARIANT_BOOL suppress);

	STDMETHOD(get_ToleranceSuppressZeroFeet)(VARIANT_BOOL* pSuppress);
	STDMETHOD(put_ToleranceSuppressZeroFeet)(VARIANT_BOOL suppress);

	STDMETHOD(get_ToleranceUpperLimit)(double* pLimit);
	STDMETHOD(put_ToleranceUpperLimit)(double limit);

	STDMETHOD(get_TolerancePrecision)(DimPrecision* pPrecision);
	STDMETHOD(put_TolerancePrecision)(DimPrecision precision);

	STDMETHOD(get_UnitsFormat)(DimLUnits* pUnits);
	STDMETHOD(put_UnitsFormat)(DimLUnits units);

	STDMETHOD(get_VerticalTextPosition)(DimVerticalJustification* pPosition);
	STDMETHOD(put_VerticalTextPosition)(DimVerticalJustification position);

protected:
	bool	getOverrideValue(resbuf& value);
	HRESULT	getStyleProperty(short dxfCode, double* pValue);
	HRESULT	getStyleProperty(short dxfCode, int* pValue);
	HRESULT	getStyleProperty(short dxfCode, char** pValue, bool bHandleString = false);

	HRESULT	setStyleProperty(resbuf& value);

	/*DG - 26.12.2001*/// The following functions are called from
	// two or more classes (methods) derived from CAutoDimension.
	// For example, get_AngleFormat is called
	// from CAutoDimAngular::get_AngleFormat and
	// from CAutoDim3PointAngular::get_AngleFormat.
	// However, these methods are not part of IIcadDimension.

	HRESULT get_AngleFormat(AngleUnits* Movement);
	HRESULT put_AngleFormat(AngleUnits Movement);

	HRESULT get_Arrowhead1Block(BSTR* BlockName);
	HRESULT put_Arrowhead1Block(BSTR BlockName);

	HRESULT get_Arrowhead1Type(DimArrowheadType* ArrowheadType);
	HRESULT put_Arrowhead1Type(DimArrowheadType ArrowheadType);

	HRESULT get_Arrowhead2Block(BSTR* BlockName);
	HRESULT put_Arrowhead2Block(BSTR BlockName);

	HRESULT get_Arrowhead2Type(DimArrowheadType* ArrowheadType);
	HRESULT put_Arrowhead2Type(DimArrowheadType ArrowheadType);

	HRESULT get_DimensionLineColor(Colors* Color);
	HRESULT put_DimensionLineColor(Colors Color);

	HRESULT get_DimLine1Suppress(VARIANT_BOOL* Supress);
	HRESULT put_DimLine1Suppress(VARIANT_BOOL Suppress);

	HRESULT get_DimLine2Suppress(VARIANT_BOOL* Supress);
	HRESULT put_DimLine2Suppress(VARIANT_BOOL Suppress);

	HRESULT get_DimLineInside(VARIANT_BOOL* Inside);
	HRESULT put_DimLineInside(VARIANT_BOOL Inside);

	HRESULT get_ExtensionLineColor(Colors* Color);
	HRESULT put_ExtensionLineColor(Colors Color);

	HRESULT get_ExtensionLineExtend(double* Extension);
	HRESULT put_ExtensionLineExtend(double Extension);

	HRESULT get_ExtLine1EndPoint(IIcadPoint** Point);
	HRESULT put_ExtLine1EndPoint(IIcadPoint* Point);

	HRESULT get_ExtLine1Suppress(VARIANT_BOOL* Supress);
	HRESULT put_ExtLine1Suppress(VARIANT_BOOL Suppress);

	HRESULT get_ExtLine2EndPoint(IIcadPoint** Point);
	HRESULT put_ExtLine2EndPoint(IIcadPoint* Point);

	HRESULT get_ExtLine2Suppress(VARIANT_BOOL* Supress);
	HRESULT put_ExtLine2Suppress(VARIANT_BOOL Suppress);

	HRESULT get_Fit(DimFit* Fit);
	HRESULT put_Fit(DimFit Fit);

	HRESULT get_ForceLineInside(VARIANT_BOOL* Inside);
	HRESULT put_ForceLineInside(VARIANT_BOOL Inside);

	HRESULT get_HorizontalTextPosition(DimHorizontalJustification* Position);
	HRESULT put_HorizontalTextPosition(DimHorizontalJustification Position);

	HRESULT get_TextInside(VARIANT_BOOL* Inside);
	HRESULT put_TextInside(VARIANT_BOOL Inside);

	HRESULT get_TextInsideAlign(VARIANT_BOOL* Alignment);
	HRESULT put_TextInsideAlign(VARIANT_BOOL Alignment);

	HRESULT get_TextOutsideAlign(VARIANT_BOOL* Alignment);
	HRESULT put_TextOutsideAlign(VARIANT_BOOL Alignment);

	HRESULT get_TextPrecision(DimPrecision* Precision);
	HRESULT put_TextPrecision(DimPrecision Precision);

	HRESULT get_AltRoundDistance(double* Round);
	HRESULT put_AltRoundDistance(double Round);

	HRESULT get_AltSuppressLeadingZeroes(VARIANT_BOOL* Supress);
	HRESULT put_AltSuppressLeadingZeroes(VARIANT_BOOL Suppress);

	HRESULT get_AltSuppressTrailingZeroes(VARIANT_BOOL* Supress);
	HRESULT put_AltSuppressTrailingZeroes(VARIANT_BOOL Supress);

	HRESULT get_AltSuppressZeroInches(VARIANT_BOOL* Supress);
	HRESULT put_AltSuppressZeroInches(VARIANT_BOOL Supress);

	HRESULT get_AltSuppressZeroFeet(VARIANT_BOOL* Supress);
	HRESULT put_AltSuppressZeroFeet(VARIANT_BOOL Supress);

	HRESULT get_AltTextPrefix(BSTR* PrefixString);
	HRESULT put_AltTextPrefix(BSTR PrefixString);

	HRESULT get_AltTextSuffix(BSTR* SuffixString);
	HRESULT put_AltTextSuffix(BSTR SuffixString);

	HRESULT get_AltTolerancePrecision(DimPrecision* Precision);
	HRESULT put_AltTolerancePrecision(DimPrecision Precision);

	HRESULT get_AltToleranceSuppressLeadingZeroes(VARIANT_BOOL* Supress);
	HRESULT put_AltToleranceSuppressLeadingZeroes(VARIANT_BOOL Supress);

	HRESULT get_AltToleranceSuppressTrailingZeroes(VARIANT_BOOL* Supress);
	HRESULT put_AltToleranceSuppressTrailingZeroes(VARIANT_BOOL Supress);

	HRESULT get_AltToleranceSuppressZeroInches(VARIANT_BOOL* Supress);
	HRESULT put_AltToleranceSuppressZeroInches(VARIANT_BOOL Supress);

	HRESULT get_AltToleranceSuppressZeroFeet(VARIANT_BOOL* Supress);
	HRESULT put_AltToleranceSuppressZeroFeet(VARIANT_BOOL Supress);

	HRESULT get_AltUnits(VARIANT_BOOL* Enable);
	HRESULT put_AltUnits(VARIANT_BOOL Enable);

	HRESULT get_AltUnitsFormat(DimLUnits* Units);
	HRESULT put_AltUnitsFormat(DimLUnits Units);

	HRESULT get_AltUnitsPrecision(DimPrecision* Precision);
	HRESULT put_AltUnitsPrecision(DimPrecision Precision);

	HRESULT get_AltUnitsScale(double* Scale);
	HRESULT put_AltUnitsScale(double Scale);

	HRESULT get_DimensionLineExtend(double* pScaleFactor);
	HRESULT put_DimensionLineExtend(double ScaleFactor);

	HRESULT get_FractionFormat(DimFractionType* Format);
	HRESULT put_FractionFormat(DimFractionType Format);

	HRESULT get_LinearScaleFactor(double* ScaleFactor);
	HRESULT put_LinearScaleFactor(double ScaleFactor);

	HRESULT get_PrimaryUnitsPrecision(DimPrecision* Precision);
	HRESULT put_PrimaryUnitsPrecision(DimPrecision Precision);

	HRESULT get_RoundDistance(double* Round);
	HRESULT put_RoundDistance(double Round);

	HRESULT get_SuppressZeroInches(VARIANT_BOOL* Supress);
	HRESULT put_SuppressZeroInches(VARIANT_BOOL Supress);

	HRESULT get_SuppressZeroFeet(VARIANT_BOOL* Supress);
	HRESULT put_SuppressZeroFeet(VARIANT_BOOL Supress);

	HRESULT get_CenterMarkSize(double* Size);
	HRESULT put_CenterMarkSize(double Size);

	HRESULT get_CenterType(DimCenterType* Type);
	HRESULT put_CenterType(DimCenterType Type);

	HRESULT get_LeaderLength(double* LeaderLength);
	HRESULT put_LeaderLength(double LeaderLength);
};
