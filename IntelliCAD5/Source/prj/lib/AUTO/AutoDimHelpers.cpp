// File: <DevDir>\sources\prj\lib\auto\AutoDimHelpers.cpp
//


#include "stdafx.h"
#include "AutoDimension.h"
#include "dimstyletabrec.h"
#include "AutoPoint.h"


const int dimblk_count = 20;

/*DG - 26.12.2001*/// Here are standard dimblk names of icad used in the following function.
// !!! Keep their numbers in synch with DimArrowheadType enum. !!!
// TO DO: 1. put DimArrowheadType into the synch with acad's AcDimArrowheadType;
//        2. implement in dimensions commented 5th and 10th dimblks.
const char* dimBlks[dimblk_count] =
{ "",				//  0
  "DOT",			//  1
  "DOTSMALL",		//  2
  "DOTBLANK",		//  3
  "ORIGIN",			//  4
  "", //"ORIGIN2",	//  5
  "OPEN",			//  6
  "OPEN90",			//  7
  "OPEN30",			//  8
  "CLOSED",			//  9
  "", //"SMALL",	// 10
  "NONE",			// 11
  "OBLIQUE",		// 12
  "BOXFILLED",		// 13
  "BOXBLANK",		// 14
  "CLOSEDBLANK",	// 15
  "DATUMFILLED",	// 16
  "DATUMBLANK",		// 17
  "INTEGRAL",		// 18
  "ARCHTICK" };		// 19


// Author: Dmitry Gavrilov
// Return values: 0..19 if dimblkName is a standard dimblk name; -1 otherwise
int dimblkNum(CString&	dimblkName)
{
	int	num = dimblk_count;

	for( ; num--; )
		if(!dimblkName.CompareNoCase(dimBlks[num]))
			break;

	return num;
}


HRESULT
CAutoDimension::get_AngleFormat(AngleUnits* Movement)
{
	return getStyleProperty(275, (int*)Movement);
}


HRESULT
CAutoDimension::put_AngleFormat(AngleUnits Movement)
{
	resbuf	styleProperty;
	styleProperty.restype = 275;
	styleProperty.resval.rint = Movement;
	return setStyleProperty(styleProperty);
}


HRESULT
CAutoDimension::get_Arrowhead1Block(BSTR* BlockName)
{
	RETURN_IF_INVALID(BlockName)

	char*	pStyleProperty = NULL;
	HRESULT	ret = getStyleProperty(6, &pStyleProperty);

	if(SUCCEEDED(ret))
	{
		CString	str(pStyleProperty);
		*BlockName = str.AllocSysString();
	}

	IC_FREE(pStyleProperty);

	return ret;
}


HRESULT
CAutoDimension::put_Arrowhead1Block(BSTR BlockName)
{
	CString		str(BlockName);

	if(dimblkNum(str) < 0 && !m_pDoc->m_dbDrawing->findtabrec(DB_BLOCKTAB, (LPCTSTR)str, 1))
		return E_INVALIDARG;
	else
	{
		resbuf		styleProperty;
		styleProperty.restype = 6;
		styleProperty.resval.rstring = (char*)(LPCTSTR)str;
		return setStyleProperty(styleProperty);
	}
}


HRESULT
CAutoDimension::get_Arrowhead1Type(DimArrowheadType* ArrowheadType)
{
	char*	pStyleProperty = NULL;
	HRESULT	ret = getStyleProperty(6, &pStyleProperty);

	if(SUCCEEDED(ret))
	{
		if(!*pStyleProperty)					// may remove these lines after implementing
			*ArrowheadType = vicArrowDefault;	// "Small" and "Origin2" dimblks.
		else
		{
			if((*ArrowheadType = (DimArrowheadType)dimblkNum(CString(pStyleProperty))) < 0)
				*ArrowheadType = vicArrowUserDefined;
		}
	}

	return ret;
}


HRESULT
CAutoDimension::put_Arrowhead1Type(DimArrowheadType ArrowheadType)
{
	if(ArrowheadType == vicArrowUserDefined)
		return S_OK;

	CString	str(dimBlks[ArrowheadType]);
	BSTR	bstr = str.AllocSysString();
	HRESULT	ret = put_Arrowhead1Block(bstr);
	SysFreeString(bstr);
	return ret;
}


HRESULT
CAutoDimension::get_Arrowhead2Block(BSTR* BlockName)
{
	RETURN_IF_INVALID(BlockName)

	char*	pStyleProperty = NULL;
	HRESULT	ret = getStyleProperty(7, &pStyleProperty);

	if(SUCCEEDED(ret))
	{
		CString	str(pStyleProperty);
		*BlockName = str.AllocSysString();
	}

	IC_FREE(pStyleProperty);

	return ret;
}


HRESULT
CAutoDimension::put_Arrowhead2Block(BSTR BlockName)
{
	CString		str(BlockName);

	if(dimblkNum(str) < 0 && !m_pDoc->m_dbDrawing->findtabrec(DB_BLOCKTAB, (LPCTSTR)str, 1))
		return E_INVALIDARG;
	else
	{
		resbuf		styleProperty;
		styleProperty.restype = 7;
		styleProperty.resval.rstring = (char*)(LPCTSTR)str;
		return setStyleProperty(styleProperty);
	}
}


HRESULT
CAutoDimension::get_Arrowhead2Type(DimArrowheadType* ArrowheadType)
{
	char*	pStyleProperty = NULL;
	HRESULT	ret = getStyleProperty(7, &pStyleProperty);

	if(SUCCEEDED(ret))
	{
		if(!*pStyleProperty)									// may remove these lines after implementing
			*ArrowheadType = vicArrowDefault;	// "Small" and "Origin2" dimblks.
		else
		{
			if((*ArrowheadType = (DimArrowheadType)dimblkNum(CString(pStyleProperty))) < 0)
				*ArrowheadType = vicArrowUserDefined;
		}
	}

	return ret;
}


HRESULT
CAutoDimension::put_Arrowhead2Type(DimArrowheadType ArrowheadType)
{
	if(ArrowheadType == vicArrowUserDefined)
		return S_OK;

	CString	str(dimBlks[ArrowheadType]);
	BSTR	bstr = str.AllocSysString();
	HRESULT	ret = put_Arrowhead2Block(bstr);
	SysFreeString(bstr);
	return ret;
}


HRESULT
CAutoDimension::get_DimensionLineColor(Colors* Color)
{
	return getStyleProperty(176, (int*)Color);
}


HRESULT
CAutoDimension::put_DimensionLineColor(Colors Color)
{
	resbuf	styleProperty;
	styleProperty.restype = 176;
	styleProperty.resval.rint = Color;
	return setStyleProperty(styleProperty);
}


HRESULT
CAutoDimension::get_DimLine1Suppress(VARIANT_BOOL* Supress)
{
	RETURN_IF_INVALID(Supress)

	int		styleProperty;
	HRESULT	ret = getStyleProperty(281, &styleProperty);

	if(SUCCEEDED(ret))
		*Supress = styleProperty ? VARIANT_TRUE : VARIANT_FALSE;

	return ret;
}


HRESULT
CAutoDimension::put_DimLine1Suppress(VARIANT_BOOL Suppress)
{
	resbuf	styleProperty;
	styleProperty.restype = 281;
	styleProperty.resval.rint = Suppress ? VARIANT_TRUE : VARIANT_FALSE;
	return setStyleProperty(styleProperty);
}


HRESULT
CAutoDimension::get_DimLine2Suppress(VARIANT_BOOL* Supress)
{
	RETURN_IF_INVALID(Supress)

	int		styleProperty;
	HRESULT	ret = getStyleProperty(282, &styleProperty);

	if(SUCCEEDED(ret))
		*Supress = styleProperty ? VARIANT_TRUE : VARIANT_FALSE;

	return ret;
}


HRESULT
CAutoDimension::put_DimLine2Suppress(VARIANT_BOOL Suppress)
{
	resbuf	styleProperty;
	styleProperty.restype = 282;
	styleProperty.resval.rint = Suppress ? VARIANT_TRUE : VARIANT_FALSE;
	return setStyleProperty(styleProperty);
}


HRESULT
CAutoDimension::get_DimLineInside(VARIANT_BOOL* Inside)
{
	RETURN_IF_INVALID(Inside)

	int		styleProperty;
	HRESULT	ret = getStyleProperty(175, &styleProperty);

	if(SUCCEEDED(ret))
		*Inside = styleProperty ? VARIANT_TRUE : VARIANT_FALSE;

	return ret;
}


HRESULT
CAutoDimension::put_DimLineInside(VARIANT_BOOL Inside)
{
	resbuf	styleProperty;
	styleProperty.restype = 175;
	styleProperty.resval.rint = Inside ? VARIANT_TRUE : VARIANT_FALSE;
	return setStyleProperty(styleProperty);
}


HRESULT
CAutoDimension::get_ExtensionLineColor(Colors* Color)
{
	return getStyleProperty(177, (int*)Color);
}


HRESULT
CAutoDimension::put_ExtensionLineColor(Colors Color)
{
	resbuf	styleProperty;
	styleProperty.restype = 177;
	styleProperty.resval.rint = Color;
	return setStyleProperty(styleProperty);
}


HRESULT
CAutoDimension::get_ExtensionLineExtend(double* Extension)
{
	return getStyleProperty(44, Extension);
}


HRESULT
CAutoDimension::put_ExtensionLineExtend(double Extension)
{
	resbuf	styleProperty;
	styleProperty.restype = 44;
	styleProperty.resval.rreal = Extension;
	return setStyleProperty(styleProperty);
}


HRESULT
CAutoDimension::get_ExtLine1EndPoint(IIcadPoint** Point)
{
	RETURN_IF_INVALID((m_pDbHandItem && Point))
	
	sds_point	pt;
	m_pDbHandItem->get_13(pt);
	
	CComObject<CAutoPoint>*	pPoint;
	CComObject<CAutoPoint>::CreateInstance(&pPoint);
	if(!pPoint)
		return E_FAIL;
	
	pPoint->Init(NULL, NULL, pt[0], pt[1], pt[2]);
	
	return pPoint->QueryInterface(Point);
}


HRESULT
CAutoDimension::put_ExtLine1EndPoint(IIcadPoint* Point)
{
	RETURN_IF_INVALID(m_pDbHandItem)

	sds_point	pt = {0.0, 0.0, 0.0};
	if(Point)
	{
		Point->get_x(&pt[0]);
		Point->get_y(&pt[1]);
		Point->get_z(&pt[2]);
	}
	m_pDbHandItem->set_13(pt);
	
	return S_OK;
}


HRESULT
CAutoDimension::get_ExtLine1Suppress(VARIANT_BOOL* Supress)
{
	RETURN_IF_INVALID(Supress)

	int		styleProperty;
	HRESULT	ret = getStyleProperty(75, &styleProperty);

	if(SUCCEEDED(ret))
		*Supress = styleProperty ? VARIANT_TRUE : VARIANT_FALSE;

	return ret;
}


HRESULT
CAutoDimension::put_ExtLine1Suppress(VARIANT_BOOL Suppress)
{
	resbuf	styleProperty;
	styleProperty.restype = 75;
	styleProperty.resval.rint = Suppress ? VARIANT_TRUE : VARIANT_FALSE;
	return setStyleProperty(styleProperty);
}


HRESULT
CAutoDimension::get_ExtLine2EndPoint(IIcadPoint** Point)
{
	RETURN_IF_INVALID((m_pDbHandItem && Point))
	
	sds_point	pt;
	int			dimType;
	m_pDbHandItem->get_70(&dimType);
	if((dimType & 7) == 2)
		m_pDbHandItem->get_15(pt);	// DimAngular
	else
	{
		_ASSERTE((dimType & 7) == 5);
		m_pDbHandItem->get_14(pt);	// Dim3PointAngular
	}

	CComObject<CAutoPoint>*	pPoint;
	CComObject<CAutoPoint>::CreateInstance(&pPoint);
	if(!pPoint)
		return E_FAIL;
	
	pPoint->Init(NULL, NULL, pt[0], pt[1], pt[2]);
	
	return pPoint->QueryInterface(Point);
}


HRESULT
CAutoDimension::put_ExtLine2EndPoint(IIcadPoint* Point)
{
	RETURN_IF_INVALID(m_pDbHandItem)

	sds_point	pt = {0.0, 0.0, 0.0};
	if(Point)
	{
		Point->get_x(&pt[0]);
		Point->get_y(&pt[1]);
		Point->get_z(&pt[2]);
	}
	int		dimType;
	m_pDbHandItem->get_70(&dimType);
	if((dimType & 7) == 2)
		m_pDbHandItem->set_15(pt);	// DimAngular
	else
	{
		_ASSERTE((dimType & 7) == 5);
		m_pDbHandItem->set_14(pt);	// Dim3PointAngular
	}
	
	return S_OK;
}


HRESULT
CAutoDimension::get_ExtLine2Suppress(VARIANT_BOOL* Supress)
{
	RETURN_IF_INVALID(Supress)

	int		styleProperty;
	HRESULT	ret = getStyleProperty(76, &styleProperty);

	if(SUCCEEDED(ret))
		*Supress = styleProperty ? VARIANT_TRUE : VARIANT_FALSE;

	return ret;
}


HRESULT
CAutoDimension::put_ExtLine2Suppress(VARIANT_BOOL Suppress)
{
	resbuf	styleProperty;
	styleProperty.restype = 76;
	styleProperty.resval.rint = Suppress ? VARIANT_TRUE : VARIANT_FALSE;
	return setStyleProperty(styleProperty);
}


HRESULT
CAutoDimension::get_Fit(DimFit* Fit)
{
	return getStyleProperty(289, (int*)Fit);
}


HRESULT
CAutoDimension::put_Fit(DimFit Fit)
{
	resbuf	styleProperty;
	styleProperty.restype = 289;
	styleProperty.resval.rint = Fit;
	return setStyleProperty(styleProperty);
}


HRESULT
CAutoDimension::get_ForceLineInside(VARIANT_BOOL* Inside)
{
	RETURN_IF_INVALID(Inside)

	int		styleProperty;
	HRESULT	ret = getStyleProperty(172, &styleProperty);

	if(SUCCEEDED(ret))
		*Inside = styleProperty ? VARIANT_TRUE : VARIANT_FALSE;

	return ret;
}


HRESULT
CAutoDimension::put_ForceLineInside(VARIANT_BOOL Inside)
{
	resbuf	styleProperty;
	styleProperty.restype = 172;
	styleProperty.resval.rint = Inside ? VARIANT_TRUE : VARIANT_FALSE;
	return setStyleProperty(styleProperty);
}


HRESULT
CAutoDimension::get_HorizontalTextPosition(DimHorizontalJustification* Position)
{
	return getStyleProperty(280, (int*)Position);
}


HRESULT
CAutoDimension::put_HorizontalTextPosition(DimHorizontalJustification Position)
{
	resbuf	styleProperty;
	styleProperty.restype = 280;
	styleProperty.resval.rint = Position;
	return setStyleProperty(styleProperty);
}


HRESULT
CAutoDimension::get_TextInside(VARIANT_BOOL* Inside)
{
	RETURN_IF_INVALID(Inside)

	int		styleProperty;
	HRESULT	ret = getStyleProperty(174, &styleProperty);

	if(SUCCEEDED(ret))
		*Inside = styleProperty ? VARIANT_TRUE : VARIANT_FALSE;

	return ret;
}


HRESULT
CAutoDimension::put_TextInside(VARIANT_BOOL Inside)
{
	resbuf	styleProperty;
	styleProperty.restype = 174;
	styleProperty.resval.rint = Inside ? VARIANT_TRUE : VARIANT_FALSE;
	return setStyleProperty(styleProperty);
}


HRESULT
CAutoDimension::get_TextInsideAlign(VARIANT_BOOL* Alignment)
{
	RETURN_IF_INVALID(Alignment)

	int		styleProperty;
	HRESULT	ret = getStyleProperty(73, &styleProperty);

	if(SUCCEEDED(ret))
		*Alignment = styleProperty ? VARIANT_TRUE : VARIANT_FALSE;

	return ret;
}


HRESULT
CAutoDimension::put_TextInsideAlign(VARIANT_BOOL Alignment)
{
	resbuf	styleProperty;
	styleProperty.restype = 73;
	styleProperty.resval.rint = Alignment ? VARIANT_TRUE : VARIANT_FALSE;
	return setStyleProperty(styleProperty);
}


HRESULT
CAutoDimension::get_TextOutsideAlign(VARIANT_BOOL* Alignment)
{
	RETURN_IF_INVALID(Alignment)

	int		styleProperty;
	HRESULT	ret = getStyleProperty(74, &styleProperty);

	if(SUCCEEDED(ret))
		*Alignment = styleProperty ? VARIANT_TRUE : VARIANT_FALSE;

	return ret;
}


HRESULT
CAutoDimension::put_TextOutsideAlign(VARIANT_BOOL Alignment)
{
	resbuf	styleProperty;
	styleProperty.restype = 74;
	styleProperty.resval.rint = Alignment ? VARIANT_TRUE : VARIANT_FALSE;
	return setStyleProperty(styleProperty);
}


HRESULT
CAutoDimension::get_TextPrecision(DimPrecision* Precision)
{
	/*DG - 26.12.2001*/// Uncomment when dxf179 (DIMADEC) is supported by dimstyle tabrec.
	//return getStyleProperty(179, (int*)Precision);

	return E_NOTIMPL;
}


HRESULT
CAutoDimension::put_TextPrecision(DimPrecision Precision)
{
	/*DG - 26.12.2001*/// Uncomment when dxf179 (DIMADEC) is supported by dimstyle tabrec.
	/*
	resbuf	styleProperty;
	styleProperty.restype = 179;
	styleProperty.resval.rint = Precision;
	return setStyleProperty(styleProperty);*/

	return E_NOTIMPL;
}


HRESULT
CAutoDimension::get_AltRoundDistance(double* Round)
{
	/*DG - 26.12.2001*/// Uncomment when dxf148 (DIMALTRND) is supported by dimstyle tabrec.
	//return getStyleProperty(148, Round);

	return E_NOTIMPL;
}


HRESULT
CAutoDimension::put_AltRoundDistance(double Round)
{
	/*DG - 26.12.2001*/// Uncomment when dxf148 (DIMALTRND) is supported by dimstyle tabrec.
	/*
	resbuf	styleProperty;
	styleProperty.restype = 148;
	styleProperty.resval.rreal = Round;
	return setStyleProperty(styleProperty);*/

	return E_NOTIMPL;
}


HRESULT
CAutoDimension::get_AltSuppressLeadingZeroes(VARIANT_BOOL* Supress)
{
	RETURN_IF_INVALID(Supress)

	int		styleProperty;
	HRESULT	ret = getStyleProperty(285, &styleProperty);

	if(SUCCEEDED(ret))
		*Supress = (styleProperty & 4) ? VARIANT_TRUE : VARIANT_FALSE;

	return ret;
}


HRESULT
CAutoDimension::put_AltSuppressLeadingZeroes(VARIANT_BOOL Suppress)
{
	int		oldStyleProperty;
	HRESULT	ret = getStyleProperty(285, &oldStyleProperty);
	if(SUCCEEDED(ret))
	{
		resbuf	styleProperty;
		styleProperty.restype = 285;
		styleProperty.resval.rint = (Suppress) ? oldStyleProperty | 4 : oldStyleProperty & ~4;
		ret = setStyleProperty(styleProperty);
	}

	return ret;
}


HRESULT
CAutoDimension::get_AltSuppressTrailingZeroes(VARIANT_BOOL* Supress)
{
	RETURN_IF_INVALID(Supress)

	int		styleProperty;
	HRESULT	ret = getStyleProperty(285, &styleProperty);

	if(SUCCEEDED(ret))
		*Supress = (styleProperty & 8) ? VARIANT_TRUE : VARIANT_FALSE;

	return ret;
}


HRESULT
CAutoDimension::put_AltSuppressTrailingZeroes(VARIANT_BOOL Supress)
{
	int		oldStyleProperty;
	HRESULT	ret = getStyleProperty(285, &oldStyleProperty);
	if(SUCCEEDED(ret))
	{
		resbuf	styleProperty;
		styleProperty.restype = 285;
		styleProperty.resval.rint = (Supress) ? oldStyleProperty | 8 : oldStyleProperty & ~8;
		ret = setStyleProperty(styleProperty);
	}

	return ret;
}


HRESULT
CAutoDimension::get_AltSuppressZeroInches(VARIANT_BOOL* Supress)
{
	RETURN_IF_INVALID(Supress)

	int		styleProperty;
	HRESULT	ret = getStyleProperty(285, &styleProperty);

	if(SUCCEEDED(ret))
		*Supress = (styleProperty & 1) ? VARIANT_FALSE : VARIANT_TRUE;

	return ret;
}


HRESULT
CAutoDimension::put_AltSuppressZeroInches(VARIANT_BOOL Supress)
{
	int		oldStyleProperty;
	HRESULT	ret = getStyleProperty(285, &oldStyleProperty);
	if(SUCCEEDED(ret))
	{
		resbuf	styleProperty;
		styleProperty.restype = 285;
		styleProperty.resval.rint = (Supress) ? oldStyleProperty & ~1 : oldStyleProperty | 1;
		ret = setStyleProperty(styleProperty);
	}

	return ret;
}


HRESULT
CAutoDimension::get_AltSuppressZeroFeet(VARIANT_BOOL* Supress)
{
	RETURN_IF_INVALID(Supress)

	int		styleProperty;
	HRESULT	ret = getStyleProperty(285, &styleProperty);

	if(SUCCEEDED(ret))
		*Supress = (!(styleProperty & 3) || (styleProperty & 3) == 3) ? VARIANT_TRUE : VARIANT_FALSE;

	return ret;
}


HRESULT
CAutoDimension::put_AltSuppressZeroFeet(VARIANT_BOOL Supress)
{
	int		oldStyleProperty;
	HRESULT	ret = getStyleProperty(285, &oldStyleProperty);
	if(SUCCEEDED(ret))
	{
		resbuf	styleProperty;
		styleProperty.restype = 285;
		if(Supress)
			styleProperty.resval.rint = (oldStyleProperty & 1) ? oldStyleProperty | 3 : oldStyleProperty & ~3;
		else
			styleProperty.resval.rint = (oldStyleProperty & 1) ? oldStyleProperty | 2 : oldStyleProperty | 1;
		ret = setStyleProperty(styleProperty);
	}

	return ret;
}


HRESULT
CAutoDimension::get_AltTextPrefix(BSTR* PrefixString)
{
	RETURN_IF_INVALID(PrefixString)

	char*	pStyleProperty = NULL;
	HRESULT	ret = getStyleProperty(4, &pStyleProperty);

	if(SUCCEEDED(ret))
	{
		CString	str(pStyleProperty);
		// EBATECH(CNBR) -[ 2002/5/15
		int nPos = str.Find("[]");
		if( nPos > 0 )
			str = str.Left(nPos);
		//str = str.Left(str.Find('<'*//*DNT*/));
		// EBATECH(CNBR) ]-
		*PrefixString = str.AllocSysString();
	}

	IC_FREE(pStyleProperty);

	return ret;
}


HRESULT
CAutoDimension::put_AltTextPrefix(BSTR PrefixString)
{
	char*	pOldStyleProperty = NULL;
	HRESULT	ret = getStyleProperty(4, &pOldStyleProperty);

	if(SUCCEEDED(ret))
	{
		CString	str(pOldStyleProperty);
		// EBATECH(CNBR) -[ 2002/5/15 
		CString SuffixString("");
		int nPos = str.Find("[]");
		if( nPos > 0 )
			SuffixString = str.Mid(nPos+2);
		str = CString(PrefixString) + CString("[]") + SuffixString;
		if( str.Compare("[]") == 0 )
			str = CString("");
		//str = CString(PrefixString) + str.Right(str.GetLength() - str.Find('<'/*DNT*/));
		// EBATECH(CNBR) ]-
		resbuf	styleProperty;
		styleProperty.restype = 4;
		styleProperty.resval.rstring = (char*)(LPCTSTR)str;
		ret = setStyleProperty(styleProperty);
	}

	IC_FREE(pOldStyleProperty);

	return ret;
}


HRESULT
CAutoDimension::get_AltTextSuffix(BSTR* SuffixString)
{
	RETURN_IF_INVALID(SuffixString)

	char*	pStyleProperty = NULL;
	HRESULT	ret = getStyleProperty(4, &pStyleProperty);

	if(SUCCEEDED(ret))
	{
		CString	str(pStyleProperty);
		// EBATECH(CNBR) -[ 2002/5/15
		int nPos = str.Find("[]");
		if( nPos >= 0 )
			str = str.Mid( nPos + 2 );
		else
			str = CString("");
		//str = str.Right(str.GetLength() - str.Find('>'/*DNT*/) - 1);
		// EBATECH(CNBR) ]-
		*SuffixString = str.AllocSysString();
	}

	IC_FREE(pStyleProperty);

	return ret;
}


HRESULT
CAutoDimension::put_AltTextSuffix(BSTR SuffixString)
{
	char*	pOldStyleProperty = NULL;
	HRESULT	ret = getStyleProperty(4, &pOldStyleProperty);

	if(SUCCEEDED(ret))
	{
		CString	str(pOldStyleProperty);
		// EBATECH(CNBR) -[ 2002/5/15
		CString prefixString;
		int nPos = str.Find("[]");
		if( nPos < 0 )
			prefixString = str;
		else
			prefixString = str.Left(nPos);
		str = prefixString + CString("[]") + CString(SuffixString);
		if( str.Compare("[]") == 0 )
			str = CString("");
		//str = str.Left(str.Find('>'/*DNT*/) + 1) + CString(SuffixString);
		// EBATECH(CNBR) ]-

		resbuf	styleProperty;
		styleProperty.restype = 4;
		styleProperty.resval.rstring = (char*)(LPCTSTR)str;
		ret = setStyleProperty(styleProperty);
	}

	IC_FREE(pOldStyleProperty);

	return ret;
}


HRESULT
CAutoDimension::get_AltTolerancePrecision(DimPrecision* Precision)
{
	return getStyleProperty(274, (int*)Precision);
}


HRESULT
CAutoDimension::put_AltTolerancePrecision(DimPrecision Precision)
{
	resbuf	styleProperty;
	styleProperty.restype = 274;
	styleProperty.resval.rint = Precision;
	return setStyleProperty(styleProperty);
}


HRESULT
CAutoDimension::get_AltToleranceSuppressLeadingZeroes(VARIANT_BOOL* Supress)
{
	RETURN_IF_INVALID(Supress)

	int		styleProperty;
	HRESULT	ret = getStyleProperty(286, &styleProperty);

	if(SUCCEEDED(ret))
		*Supress = (styleProperty & 4) ? VARIANT_TRUE : VARIANT_FALSE;

	return ret;
}


HRESULT
CAutoDimension::put_AltToleranceSuppressLeadingZeroes(VARIANT_BOOL Supress)
{
	int		oldStyleProperty;
	HRESULT	ret = getStyleProperty(286, &oldStyleProperty);
	if(SUCCEEDED(ret))
	{
		resbuf	styleProperty;
		styleProperty.restype = 286;
		styleProperty.resval.rint = (Supress) ? oldStyleProperty | 4 : oldStyleProperty & ~4;
		ret = setStyleProperty(styleProperty);
	}

	return ret;
}


HRESULT
CAutoDimension::get_AltToleranceSuppressTrailingZeroes(VARIANT_BOOL* Supress)
{
	RETURN_IF_INVALID(Supress)

	int		styleProperty;
	HRESULT	ret = getStyleProperty(286, &styleProperty);

	if(SUCCEEDED(ret))
		*Supress = (styleProperty & 8) ? VARIANT_TRUE : VARIANT_FALSE;

	return ret;
}


HRESULT
CAutoDimension::put_AltToleranceSuppressTrailingZeroes(VARIANT_BOOL Supress)
{
	int		oldStyleProperty;
	HRESULT	ret = getStyleProperty(286, &oldStyleProperty);
	if(SUCCEEDED(ret))
	{
		resbuf	styleProperty;
		styleProperty.restype = 286;
		styleProperty.resval.rint = (Supress) ? oldStyleProperty | 8 : oldStyleProperty & ~8;
		ret = setStyleProperty(styleProperty);
	}

	return ret;
}


HRESULT
CAutoDimension::get_AltToleranceSuppressZeroInches(VARIANT_BOOL* Supress)
{
	RETURN_IF_INVALID(Supress)

	int		styleProperty;
	HRESULT	ret = getStyleProperty(286, &styleProperty);

	if(SUCCEEDED(ret))
		*Supress = (styleProperty & 1) ? VARIANT_FALSE : VARIANT_TRUE;

	return ret;
}


HRESULT
CAutoDimension::put_AltToleranceSuppressZeroInches(VARIANT_BOOL Supress)
{
	int		oldStyleProperty;
	HRESULT	ret = getStyleProperty(286, &oldStyleProperty);
	if(SUCCEEDED(ret))
	{
		resbuf	styleProperty;
		styleProperty.restype = 286;
		styleProperty.resval.rint = (Supress) ? oldStyleProperty & ~1 : oldStyleProperty | 1;
		ret = setStyleProperty(styleProperty);
	}

	return ret;
}


HRESULT
CAutoDimension::get_AltToleranceSuppressZeroFeet(VARIANT_BOOL* Supress)
{
	RETURN_IF_INVALID(Supress)

	int		styleProperty;
	HRESULT	ret = getStyleProperty(286, &styleProperty);

	if(SUCCEEDED(ret))
		*Supress = (!(styleProperty & 3) || (styleProperty & 3) == 3) ? VARIANT_TRUE : VARIANT_FALSE;

	return ret;
}


HRESULT
CAutoDimension::put_AltToleranceSuppressZeroFeet(VARIANT_BOOL Supress)
{
	int		oldStyleProperty;
	HRESULT	ret = getStyleProperty(286, &oldStyleProperty);
	if(SUCCEEDED(ret))
	{
		resbuf	styleProperty;
		styleProperty.restype = 286;
		if(Supress)
			styleProperty.resval.rint = (oldStyleProperty & 1) ? oldStyleProperty | 3 : oldStyleProperty & ~3;
		else
			styleProperty.resval.rint = (oldStyleProperty & 1) ? oldStyleProperty | 2 : oldStyleProperty | 1;
		ret = setStyleProperty(styleProperty);
	}

	return ret;
}


HRESULT
CAutoDimension::get_AltUnits(VARIANT_BOOL* Enable)
{
	RETURN_IF_INVALID(Enable)

	int		styleProperty;
	HRESULT	ret = getStyleProperty(170, &styleProperty);

	if(SUCCEEDED(ret))
		*Enable = styleProperty ? VARIANT_TRUE : VARIANT_FALSE;

	return ret;
}


HRESULT
CAutoDimension::put_AltUnits(VARIANT_BOOL Enable)
{
	resbuf	styleProperty;
	styleProperty.restype = 170;
	styleProperty.resval.rint = Enable ? VARIANT_TRUE : VARIANT_FALSE;
	return setStyleProperty(styleProperty);
}


HRESULT
CAutoDimension::get_AltUnitsFormat(DimLUnits* Units)
{
	HRESULT	ret = getStyleProperty(273, (int*)Units);

	if(SUCCEEDED(ret))
		*Units = (DimLUnits)(*Units - 1);

	return ret;
}


HRESULT
CAutoDimension::put_AltUnitsFormat(DimLUnits Units)
{
	resbuf	styleProperty;
	styleProperty.restype = 273;
	styleProperty.resval.rint = Units + 1;
	return setStyleProperty(styleProperty);
}


HRESULT
CAutoDimension::get_AltUnitsPrecision(DimPrecision* Precision)
{
	return getStyleProperty(171, (int*)Precision);
}


HRESULT
CAutoDimension::put_AltUnitsPrecision(DimPrecision Precision)
{
	resbuf	styleProperty;
	styleProperty.restype = 171;
	styleProperty.resval.rint = Precision;
	return setStyleProperty(styleProperty);
}


HRESULT
CAutoDimension::get_AltUnitsScale(double* Scale)
{
	return getStyleProperty(143, Scale);
}


HRESULT
CAutoDimension::put_AltUnitsScale(double Scale)
{
	resbuf	styleProperty;
	styleProperty.restype = 143;
	styleProperty.resval.rreal = Scale;
	return setStyleProperty(styleProperty);
}


HRESULT
CAutoDimension::get_DimensionLineExtend(double* pScaleFactor)
{
	return getStyleProperty(46, pScaleFactor);
}


HRESULT
CAutoDimension::put_DimensionLineExtend(double ScaleFactor)
{
	resbuf	styleProperty;
	styleProperty.restype = 46;
	styleProperty.resval.rreal = ScaleFactor;
	return setStyleProperty(styleProperty);
}


HRESULT
CAutoDimension::get_FractionFormat(DimFractionType* Format)
{
	return getStyleProperty(276, (int*)Format);
}


HRESULT
CAutoDimension::put_FractionFormat(DimFractionType Format)
{
	resbuf	styleProperty;
	styleProperty.restype = 276;
	styleProperty.resval.rint = Format;
	return setStyleProperty(styleProperty);
}


HRESULT
CAutoDimension::get_LinearScaleFactor(double* ScaleFactor)
{
	return getStyleProperty(144, ScaleFactor);
}


HRESULT
CAutoDimension::put_LinearScaleFactor(double ScaleFactor)
{
	resbuf	styleProperty;
	styleProperty.restype = 144;
	styleProperty.resval.rreal = ScaleFactor;
	return setStyleProperty(styleProperty);
}


HRESULT
CAutoDimension::get_PrimaryUnitsPrecision(DimPrecision* Precision)
{
	return getStyleProperty(271, (int*)Precision);
}


HRESULT
CAutoDimension::put_PrimaryUnitsPrecision(DimPrecision Precision)
{
	resbuf	styleProperty;
	styleProperty.restype = 271;
	styleProperty.resval.rint = Precision;
	return setStyleProperty(styleProperty);
}


HRESULT
CAutoDimension::get_RoundDistance(double* Round)
{
	return getStyleProperty(45, Round);
}


HRESULT
CAutoDimension::put_RoundDistance(double Round)
{
	resbuf	styleProperty;
	styleProperty.restype = 45;
	styleProperty.resval.rreal = Round;
	return setStyleProperty(styleProperty);
}


HRESULT
CAutoDimension::get_SuppressZeroInches(VARIANT_BOOL* Supress)
{
	RETURN_IF_INVALID(Supress)

	int		styleProperty;
	HRESULT	ret = getStyleProperty(78, &styleProperty);

	if(SUCCEEDED(ret))
		*Supress = (styleProperty & 1) ? VARIANT_FALSE : VARIANT_TRUE;

	return ret;
}


HRESULT
CAutoDimension::put_SuppressZeroInches(VARIANT_BOOL Supress)
{
	int		oldStyleProperty;
	HRESULT	ret = getStyleProperty(78, &oldStyleProperty);
	if(SUCCEEDED(ret))
	{
		resbuf	styleProperty;
		styleProperty.restype = 78;
		styleProperty.resval.rint = (Supress) ? oldStyleProperty & ~1 : oldStyleProperty | 1;
		ret = setStyleProperty(styleProperty);
	}

	return ret;
}


HRESULT
CAutoDimension::get_SuppressZeroFeet(VARIANT_BOOL* Supress)
{
	RETURN_IF_INVALID(Supress)

	int		styleProperty;
	HRESULT	ret = getStyleProperty(78, &styleProperty);

	if(SUCCEEDED(ret))
		*Supress = (!(styleProperty & 3) || (styleProperty & 3) == 3) ? VARIANT_TRUE : VARIANT_FALSE;

	return ret;
}


HRESULT
CAutoDimension::put_SuppressZeroFeet(VARIANT_BOOL Supress)
{
	int		oldStyleProperty;
	HRESULT	ret = getStyleProperty(78, &oldStyleProperty);
	if(SUCCEEDED(ret))
	{
		resbuf	styleProperty;
		styleProperty.restype = 78;
		if(Supress)
			styleProperty.resval.rint = (oldStyleProperty & 1) ? oldStyleProperty | 3 : oldStyleProperty & ~3;
		else
			styleProperty.resval.rint = (oldStyleProperty & 1) ? oldStyleProperty | 2 : oldStyleProperty | 1;
		ret = setStyleProperty(styleProperty);
	}

	return ret;
}


HRESULT
CAutoDimension::get_CenterMarkSize(double* Size)
{
	return getStyleProperty(141, Size);
}


HRESULT
CAutoDimension::put_CenterMarkSize(double Size)
{
	resbuf	styleProperty;
	styleProperty.restype = 141;
	styleProperty.resval.rreal = Size;
	return setStyleProperty(styleProperty);
}


HRESULT
CAutoDimension::get_CenterType(DimCenterType* Type)
{
	RETURN_IF_INVALID(Type)

	int		styleProperty;
	HRESULT	ret = getStyleProperty(141, &styleProperty);

	if(SUCCEEDED(ret))
		*Type = styleProperty < 0.0 ? vicCenterLine :
				styleProperty > 0.0 ? vicCenterMark :
				vicCenterNone;

	return ret;
}


HRESULT
CAutoDimension::put_CenterType(DimCenterType Type)
{
	double		oldStyleProperty;
	HRESULT	ret = getStyleProperty(141, &oldStyleProperty);
	if(SUCCEEDED(ret))
	{
		resbuf	styleProperty;
		styleProperty.restype = 141;
		styleProperty.resval.rreal = (Type == vicCenterLine) ? -fabs(oldStyleProperty) :
									 (Type == vicCenterMark) ? fabs(oldStyleProperty) :
									 0.0;
		ret = setStyleProperty(styleProperty);
	}

	return ret;
}


HRESULT
CAutoDimension::get_LeaderLength(double* LeaderLength)
{
	RETURN_IF_INVALID(m_pDbHandItem && LeaderLength)
	
	m_pDbHandItem->get_40(LeaderLength);
	return S_OK;
}


HRESULT
CAutoDimension::put_LeaderLength(double LeaderLength)
{
	RETURN_IF_INVALID(m_pDbHandItem);

	m_pDbHandItem->set_40(LeaderLength);
	return S_OK;
}


const char*	ACAD_str = "ACAD"/*DNT*/;
const char*	DSTYLE_str = "DSTYLE"/*DNT*/;

// Note, these defines are the same as ODT analogs.
#define AD_XD_STRING		1000
#define AD_XD_APPOBJHANDLE	1001
#define AD_XD_CONTROL		1002
#define AD_XD_OBJHANDLE		1005
#define AD_XD_REAL			1040
#define AD_XD_INT			1070

/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Get a value for the given dxfCode (eg, 41 for DIMASZ)
 *			from overriding EED (if it exists).
 * Returns:	'true' in success and 'false' in failure or there are no EED.
 ********************************************************************************/
bool
CAutoDimension::getOverrideValue
(
 resbuf&	value		// <=> NOTE, rstring value has to be freed in callers
)
{
	resbuf	*pEED = NULL,
			rbApp = {NULL, RTSTR, 0},
			*pCurRb;
    rbApp.resval.rstring = (char*)ACAD_str;

	if(!(pEED = m_pDbHandItem->get_eed(&rbApp, NULL)))
		return false;

	for(pCurRb = pEED; pCurRb && (pCurRb->restype != AD_XD_INT || pCurRb->resval.rint != value.restype); pCurRb = pCurRb->rbnext)
		if(pCurRb->restype == AD_XD_INT)
			pCurRb = pCurRb->rbnext;
	
	if(pCurRb)
	{
		if(ic_resval(pCurRb->rbnext->restype) == RTSTR)
		{
			value.resval.rstring = new char [strlen(pCurRb->rbnext->resval.rstring) + 1];
			strcpy(value.resval.rstring, pCurRb->rbnext->resval.rstring);
		}
		else
			value.resval = pCurRb->rbnext->resval;
	}

	sds_relrb(pEED);

	return !!pCurRb;
}


/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Get a 'double' value for the given dxfCode (eg, 41 for DIMASZ)
 *			from dimension style.
 *			Supported DXFs: 40, 41, 42, 44, 45, 46, 47, 48, 140, 141, 143, 144,
 *							146, 147.
 *			Calling with another dxfCode requires adding an appropriate case
 *			in the method.
 * Returns:	E_POINTER if m_pDbHandItem or pValue is NULL,
 *			E_FAIL in failure of getting dimstyle,
 *			E_INVALIDARG if dxfCode is invalid,
 *			S_OK otherwise.
 ********************************************************************************/
HRESULT
CAutoDimension::getStyleProperty
(
 short		dxfCode,	// =>
 double*	pValue		// <=
)
{
	RETURN_IF_INVALID((m_pDbHandItem && pValue))

	resbuf	rb = {NULL, dxfCode, 0};
	if(getOverrideValue(rb))
		*pValue = rb.resval.rreal;
	else
	{
		db_dimstyletabrec*	pDimStyle = NULL;
		m_pDbHandItem->get_3((db_handitem**)&pDimStyle);
		if(!pDimStyle)
			return E_FAIL;

		switch(dxfCode)
		{
		case  40 :	pDimStyle->get_40(pValue);
					break;
		case  41 :	pDimStyle->get_41(pValue);
					break;
		case  42 :	pDimStyle->get_42(pValue);
					break;
		case  44 :	pDimStyle->get_44(pValue);
					break;
		case  45 :	pDimStyle->get_45(pValue);
					break;
		case  46 :	pDimStyle->get_46(pValue);
					break;
		case  47 :	pDimStyle->get_47(pValue);
					break;
		case  48 :	pDimStyle->get_48(pValue);
					break;
		case 140 :	pDimStyle->get_140(pValue);
					break;
		case 141 :	pDimStyle->get_141(pValue);
					break;
		case 143 :	pDimStyle->get_143(pValue);
					break;
		case 144 :	pDimStyle->get_144(pValue);
					break;
		case 146 :	pDimStyle->get_146(pValue);
					break;
		case 147 :	pDimStyle->get_147(pValue);
					break;
		/*          ...isn't supported yet...
		case 148 :	pDimStyle->get_148(pValue);
					break;
		*/
		default  :	_ASSERTE(!"WARNING: Wrong DXF code!");
					return E_INVALIDARG;
		}
	}

	return S_OK;
}


/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Get an 'int' value for the given dxfCode (eg, 78 for DIMZIN)
 *			from dimension style.
 *			Supported DXFs: 73, 74, 75, 76, 77, 78, 170, 171, 172, 174, 175, 176,
 *							177, 178, 271, 272, 273, 274, 275, 276, 277, 279, 280,
 *							281, 282, 283, 284, 285, 286, 289.
 *			Calling with another dxfCode requires adding an appropriate case
 *			in the method.
 * Returns:	E_POINTER if m_pDbHandItem or pValue is NULL,
 *			E_FAIL in failure of getting dimstyle,
 *			E_INVALIDARG if dxfCode is invalid,
 *			S_OK otherwise.
 ********************************************************************************/
HRESULT
CAutoDimension::getStyleProperty
(
 short	dxfCode,	// =>
 int*	pValue		// <=
)
{
	RETURN_IF_INVALID((m_pDbHandItem && pValue))

	resbuf	rb = {NULL, dxfCode, 0};
	if(getOverrideValue(rb))
		*pValue = rb.resval.rint;
	else
	{
		db_dimstyletabrec*	pDimStyle = NULL;
		m_pDbHandItem->get_3((db_handitem**)&pDimStyle);
		if(!pDimStyle)
			return E_FAIL;

		switch(dxfCode)
		{
		case  73 :	pDimStyle->get_73(pValue);
					break;
		case  74 :	pDimStyle->get_74(pValue);
					break;
		case  75 :	pDimStyle->get_75(pValue);
					break;
		case  76 :	pDimStyle->get_76(pValue);
					break;
		case  77 :	pDimStyle->get_77(pValue);
					break;
		case  78 :	pDimStyle->get_78(pValue);
					break;
		case 170 :	pDimStyle->get_170(pValue);
					break;
		case 171 :	pDimStyle->get_171(pValue);
					break;
		case 172 :	pDimStyle->get_172(pValue);
					break;
		case 174 :	pDimStyle->get_174(pValue);
					break;
		case 175 :	pDimStyle->get_175(pValue);
					break;
		case 176 :	pDimStyle->get_176(pValue);
					break;
		case 177 :	pDimStyle->get_177(pValue);
					break;
		case 178 :	pDimStyle->get_178(pValue);
					break;
		/*          ...isn't supported yet...
		case 179 :	pDimStyle->get_179(pValue);
					break;
		*/
		case 271 :	pDimStyle->get_271(pValue);
					break;
		case 272 :	pDimStyle->get_272(pValue);
					break;
		case 273 :	pDimStyle->get_273(pValue);
					break;
		case 274 :	pDimStyle->get_274(pValue);
					break;
		case 275 :	pDimStyle->get_275(pValue);
					break;
		case 276 :	pDimStyle->get_276(pValue);
					break;
		case 277 :	pDimStyle->get_277(pValue);
					break;
		case 279 :	pDimStyle->get_279(pValue);
					break;
		case 280 :	pDimStyle->get_280(pValue);
					break;
		case 281 :	pDimStyle->get_281(pValue);
					break;
		case 282 :	pDimStyle->get_282(pValue);
					break;
		case 283 :	pDimStyle->get_283(pValue);
					break;
		case 284 :	pDimStyle->get_284(pValue);
					break;
		case 285 :	pDimStyle->get_285(pValue);
					break;
		case 286 :	pDimStyle->get_286(pValue);
					break;
		case 289 :	pDimStyle->get_289(pValue);
					break;
		default  :	_ASSERTE(!"WARNING: Wrong DXF code!");
					return E_INVALIDARG;
		}
	}

	return S_OK;
}


/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Get a 'char*' value for the given dxfCode (eg, 278 for DIMDSEP)
 *			from dimension style.
 *			Supported DXFs: 3, 4, 6, 7, 340, 341.
 *			Calling with another dxfCode requires adding an appropriate case
 *			in the method.
 * Returns:	E_POINTER if m_pDbHandItem or pValue is NULL,
 *			E_FAIL in failure of getting dimstyle,
 *			E_INVALIDARG if dxfCode is invalid,
 *			S_OK otherwise.
 ********************************************************************************/
HRESULT
CAutoDimension::getStyleProperty
(
 short	dxfCode,					// =>
 char**	ppValue,					// <= // NOTE, *ppValue has to be freed in callers
 bool	bHandleString /*= false*/	// =>
)
{
	RETURN_IF_INVALID((m_pDbHandItem && ppValue))

	resbuf	rb = {NULL, dxfCode, 0};
	char*	pTmpStr = NULL;

	if(getOverrideValue(rb))
	{
		if(bHandleString)
		{
			db_objhandle	handle(rb.resval.rstring);
			IC_FREE(rb.resval.rstring);
			db_handitem*	pTabRec = m_pDoc->m_dbDrawing->handent(handle);
			if(!pTabRec)
				return E_FAIL;
			pTabRec->get_2(&pTmpStr);
		}
		else
			*ppValue = rb.resval.rstring;
	}
	else
	{
		db_dimstyletabrec*	pDimStyle = NULL;
		m_pDbHandItem->get_3((db_handitem**)&pDimStyle);
		if(!pDimStyle)
			return E_FAIL;

		switch(dxfCode)
		{
		case   3 :	pDimStyle->get_3(&pTmpStr);
					break;
		case   4 :	pDimStyle->get_4(&pTmpStr);
					break;
		case   6 :	pDimStyle->get_6(&pTmpStr);
					break;
		case   7 :	pDimStyle->get_7(&pTmpStr);
					break;
		/*          ...isn't supported yet...
		case 278 :	pDimStyle->get_278(&pTmpStr);
					break;
		*/
		case 340 :	pDimStyle->get_340name(&pTmpStr);
					break;
		case 341 :	pDimStyle->get_341name(&pTmpStr);
					break;
		default  :	_ASSERTE(!"WARNING: Wrong DXF code!");
					return E_INVALIDARG;
		}
	}

	if(pTmpStr)
	{
		*ppValue = new char [strlen(pTmpStr) + 1];
		strcpy(*ppValue, pTmpStr);
	}

	return S_OK;
}


/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Set the given overriding property.
 * Returns:	E_POINTER if m_pDbHandItem is NULL,
 *			E_FAIL in failure of getting EED,
 *			E_INVALIDARG if dxfCode is invalid,
 *			S_OK otherwise.
 ********************************************************************************/
HRESULT
CAutoDimension::setStyleProperty
(
 resbuf&	value	// =>
)
{
// There are 3 cases of things:
// 1. the dim already has overriding of value.restype;
// 2. the dim has EED but hasn't value.restype overridden yet;
// 3. the dim hasn't EED at all yet;
// I commented these cases in code. Enjoy :)

	RETURN_IF_INVALID(m_pDbHandItem)

	resbuf	*pOldEED = NULL, *pNewEED = NULL,
			rbApp = {NULL, RTSTR, 0},
			*pCurRb;
    rbApp.resval.rstring = (char*)ACAD_str;
    
	short	rc;
	pOldEED = m_pDbHandItem->get_eed(&rbApp, &rc);
	if(rc == -2)
		return E_FAIL;

	for(pCurRb = pOldEED; pCurRb && (pCurRb->restype != AD_XD_INT || pCurRb->resval.rint != value.restype); pCurRb = pCurRb->rbnext)
		if(pCurRb->restype == AD_XD_INT)
			pCurRb = pCurRb->rbnext;
	
	if(pCurRb)
	{	// 1.
		pCurRb = pCurRb->rbnext;
		if(ic_resval(pCurRb->restype) == RTSTR)
		{
			IC_FREE(pCurRb->resval.rstring);
			pCurRb->resval.rstring = new char [strlen(value.resval.rstring) + 1];
			strcpy(pCurRb->resval.rstring, value.resval.rstring);
		}
		else
			pCurRb->resval = value.resval;

		m_pDbHandItem->set_eed(pOldEED, m_pDoc->m_dbDrawing);
		sds_relrb(pOldEED);
	}
	else
	{	// 2. and 3.
		resbuf	*pRbAfterControlBracket = NULL;

		if(pOldEED)
		{	// 2.
			pNewEED = pOldEED;

			for(pCurRb = pOldEED; pCurRb && pCurRb->restype != AD_XD_CONTROL; pCurRb = pCurRb->rbnext)
				;
			_ASSERT(pCurRb && pCurRb->rbnext);
			pRbAfterControlBracket = pCurRb->rbnext;
		}
		else
		{	// 3.
			pNewEED = pCurRb = sds_newrb(-3);

			pCurRb->rbnext = sds_newrb(AD_XD_APPOBJHANDLE);
			pCurRb = pCurRb->rbnext;
			pCurRb->resval.rstring = new char [strlen(ACAD_str) + 1];
			strcpy(pCurRb->resval.rstring, ACAD_str);

			pCurRb->rbnext = sds_newrb(AD_XD_STRING);
			pCurRb = pCurRb->rbnext;
			pCurRb->resval.rstring = new char [strlen(DSTYLE_str) + 1];
			strcpy(pCurRb->resval.rstring, DSTYLE_str);

			pCurRb->rbnext = sds_newrb(AD_XD_CONTROL);
			pCurRb = pCurRb->rbnext;
			pCurRb->resval.rstring = new char [3];
			strcpy(pCurRb->resval.rstring, "{"/*DNT*/);
		}

		pCurRb->rbnext = sds_newrb(AD_XD_INT);
		pCurRb = pCurRb->rbnext;
		pCurRb->resval.rint = value.restype;


		switch(ic_resval(value.restype))
		{
		case RTSHORT :	pCurRb->rbnext = sds_newrb(AD_XD_INT);
						pCurRb = pCurRb->rbnext;
						pCurRb->resval.rint = value.resval.rint;
						break;

		case RTREAL  :	pCurRb->rbnext = sds_newrb(AD_XD_REAL);
						pCurRb = pCurRb->rbnext;
						pCurRb->resval.rreal = value.resval.rreal;
						break;

		case RTENAME :
		case RTSTR   :	if(value.restype == 341)
							pCurRb->rbnext = sds_newrb(AD_XD_OBJHANDLE);
						else
							pCurRb->rbnext = sds_newrb(AD_XD_STRING);
						pCurRb = pCurRb->rbnext;
						pCurRb->resval.rstring = new char [strlen(value.resval.rstring) + 1];
						strcpy(pCurRb->resval.rstring, value.resval.rstring);
						break;

		default      :	_ASSERTE(!"WARNING: Wrong DXF code!");
						sds_relrb(pNewEED);
						sds_relrb(pRbAfterControlBracket);
						return E_INVALIDARG;
		}

		if(pOldEED)
			pCurRb->rbnext = pRbAfterControlBracket;
		else
		{
			pCurRb->rbnext = sds_newrb(AD_XD_CONTROL);
			pCurRb = pCurRb->rbnext;
			pCurRb->resval.rstring = new char [3];
			strcpy(pCurRb->resval.rstring, "}"/*DNT*/);
		}

		m_pDbHandItem->set_eed(pNewEED, m_pDoc->m_dbDrawing);
		sds_relrb(pNewEED);
	}

	return S_OK;
}
