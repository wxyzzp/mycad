//File: <DevDir>\source\prj\lib\auto\autodimension.cpp
/* Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 *
 * Abstract
 * Implementation of CAutoDimension - the Automation Dimension object
 */


#include "StdAfx.h"
#include "AutoDimension.h"
#include "AutoPoint.h"
#include "AutoVector.h"
#include "DbMText.h"
#include "dimstyletabrec.h"


HRESULT
CAutoDimension::Init
(
 CIcadDoc*		pDoc,
 IDispatch*		pParent,
 db_handitem*	pDbHandItem
)
{
	CAutoDbItem::Init(pDoc, pParent);

	if(pDbHandItem)
		m_pDbHandItem = pDbHandItem;
	else
	{
		/*DG - 26.12.2001*/// We do support now.
		//_ASSERTE(!"WARNING: We don't support creating concrete dimensions via COM yet!");
		if(pDoc)
			m_pDbHandItem = new db_dimension(pDoc->m_dbDrawing);
		else
			m_pDbHandItem = new db_dimension();
	}

	if(!m_pDbHandItem)
		return E_FAIL;

	return S_OK;
}


STDMETHODIMP
CAutoDimension::get_ArrowheadSize(double* pArrowheadSize)
{
	/*DG - 6.12.2001*/// Implemented the method.
	return getStyleProperty(41, pArrowheadSize);
}


STDMETHODIMP
CAutoDimension::put_ArrowheadSize(double arrowheadSize)
{
	/*DG - 6.12.2001*/// Implemented the method.
	resbuf	styleProperty;
	styleProperty.restype = 41;
	styleProperty.resval.rreal = arrowheadSize;
	return setStyleProperty(styleProperty);
}


STDMETHODIMP
CAutoDimension::get_DecimalSeparator(BSTR* pDecimalSeparator)
{
	/*DG - 6.12.2001*/// Implemented the method.
	// TO DO: Uncomment this and remove he code below when DIMDSEP variable is supported in IntelliCAD.
	/*
	RETURN_IF_INVALID(pDecimalSeparator)

	char*	pStyleProperty = NULL;
	HRESULT	ret = getStyleProperty(278, &pStyleProperty);

	if(SUCCEEDED(ret))
	{
		CString	str(pStyleProperty);
		*pDecimalSeparator = str.AllocSysString();
	}

	free(pStyleProperty);

	return ret;
	*/

	RETURN_IF_INVALID((m_pDbHandItem && pDecimalSeparator))
	return E_NOTIMPL;
}


STDMETHODIMP
CAutoDimension::put_DecimalSeparator(BSTR decimalSeparator)
{
	/*DG - 6.12.2001*/// Implemented the method.
	// TO DO: Uncomment this and remove he code below when DIMDSEP variable is supported in IntelliCAD.
	/*
	CString	str(decimalSeparator);
	resbuf	styleProperty;
	styleProperty.restype = 278;
	styleProperty.resval.rstring = (char*)(LPCTSTR)str;
	return setStyleProperty(styleProperty);
	*/

	RETURN_IF_INVALID(m_pDbHandItem)
	return E_NOTIMPL;
}


STDMETHODIMP
CAutoDimension::get_ExtensionLineOffset(double* pOffset)
{
	/*DG - 6.12.2001*/// Implemented the method.
	return getStyleProperty(42, pOffset);
}


STDMETHODIMP
CAutoDimension::put_ExtensionLineOffset(double offset)
{
	/*DG - 6.12.2001*/// Implemented the method.
	resbuf	styleProperty;
	styleProperty.restype = 42;
	styleProperty.resval.rreal = offset;
	return setStyleProperty(styleProperty);
}


STDMETHODIMP
CAutoDimension::get_Measurement(double* pValue)
{
	RETURN_IF_INVALID((m_pDbHandItem && pValue))

	/*DG - 6.12.2001*/// Uncomment the code when 42 dxf is supported by db_dimension.
	/*
	if(!m_pDbHandItem->get_42(pValue))
		return E_FAIL;

	return S_OK;
	*/

	return E_NOTIMPL;
}


STDMETHODIMP
CAutoDimension::put_Measurement(double value)
{
	RETURN_IF_INVALID(m_pDbHandItem)

	/*DG - 6.12.2001*/// This property is read-only. So, always E_FAIL.
	return E_FAIL;
}


STDMETHODIMP
CAutoDimension::get_Normal(IIcadVector** ppExtrusionDirection)
{
	RETURN_IF_INVALID((m_pDbHandItem && ppExtrusionDirection))

	sds_point	v;
	m_pDbHandItem->get_210(v);

	CComObject<CAutoVector>*	pVector;
	CComObject<CAutoVector>::CreateInstance(&pVector);
	if(!pVector)
		return E_FAIL;

	pVector->Init(NULL, v[0], v[1], v[2]);

	return pVector->QueryInterface(ppExtrusionDirection);
}


STDMETHODIMP
CAutoDimension::put_Normal(IIcadVector* pExtrusionDirection)
{
	RETURN_IF_INVALID(m_pDbHandItem);
	sds_point	v = {0.0, 0.0, 1.0};
	if(pExtrusionDirection)
	{
		pExtrusionDirection->get_x(&v[0]);
		pExtrusionDirection->get_y(&v[1]);
		pExtrusionDirection->get_z(&v[2]);

		if(db_unitizevector(v))
			return E_INVALIDARG;
	}
	m_pDbHandItem->init_210(v);

	return S_OK;
}


STDMETHODIMP
CAutoDimension::get_Rotation(double* pRotation)
{
	RETURN_IF_INVALID((m_pDbHandItem && pRotation))

	if(!m_pDbHandItem->get_53(pRotation))
		return E_FAIL;

	return S_OK;
}


STDMETHODIMP
CAutoDimension::put_Rotation(double rotation)
{
	RETURN_IF_INVALID(m_pDbHandItem)
	if(!m_pDbHandItem->set_53(rotation))
		return E_FAIL;

	return S_OK;
}


STDMETHODIMP
CAutoDimension::get_ScaleFactor(double* pScaleFactor)
{
	/*DG - 6.12.2001*/// Implemented the method.
	return getStyleProperty(40, pScaleFactor);
}


STDMETHODIMP
CAutoDimension::put_ScaleFactor(double scaleFactor)
{
	/*DG - 6.12.2001*/// Implemented the method.
	resbuf	styleProperty;
	styleProperty.restype = 40;
	styleProperty.resval.rreal = scaleFactor;
	return setStyleProperty(styleProperty);
}


STDMETHODIMP
CAutoDimension::get_StyleName(BSTR* pStyleName)
{
	RETURN_IF_INVALID((m_pDbHandItem && pStyleName))

	char*	pStr;
	m_pDbHandItem->get_3(&pStr);

	CString	str(pStr);
	*pStyleName = str.AllocSysString();
	return S_OK;
}


STDMETHODIMP
CAutoDimension::put_StyleName(BSTR styleName)
{
	RETURN_IF_INVALID(m_pDbHandItem)

	CString	str(styleName);
	if(str.IsEmpty())
		return E_INVALIDARG;

	if(!m_pDbHandItem->set_3((char*)str.GetBuffer(0), m_pDoc->m_dbDrawing))
		return E_FAIL;

	return S_OK;
}


STDMETHODIMP
CAutoDimension::get_SuppressLeadingZeroes(VARIANT_BOOL* pSuppress)
{
	/*DG - 6.12.2001*/// Implemented the method.
	RETURN_IF_INVALID(pSuppress)

	int		styleProperty;
	HRESULT	ret = getStyleProperty(78, &styleProperty);

	if(SUCCEEDED(ret))
		*pSuppress = (styleProperty & 4) ? VARIANT_TRUE : VARIANT_FALSE;

	return ret;
}


STDMETHODIMP
CAutoDimension::put_SuppressLeadingZeroes(VARIANT_BOOL suppress)
{
	/*DG - 6.12.2001*/// Implemented the method.
	int		oldStyleProperty;
	HRESULT	ret = getStyleProperty(78, &oldStyleProperty);
	if(SUCCEEDED(ret))
	{
		resbuf	styleProperty;
		styleProperty.restype = 78;
		styleProperty.resval.rint = (suppress) ? oldStyleProperty | 4 : oldStyleProperty & ~4;
		ret = setStyleProperty(styleProperty);
	}

	return ret;
}


STDMETHODIMP
CAutoDimension::get_SuppressTrailingZeroes(VARIANT_BOOL* pSuppress)
{
	/*DG - 6.12.2001*/// Implemented the method.
	RETURN_IF_INVALID(pSuppress)

	int		styleProperty;
	HRESULT	ret = getStyleProperty(78, &styleProperty);

	if(SUCCEEDED(ret))
		*pSuppress = (styleProperty & 8) ? VARIANT_TRUE : VARIANT_FALSE;

	return ret;
}


STDMETHODIMP
CAutoDimension::put_SuppressTrailingZeroes(VARIANT_BOOL suppress)
{
	/*DG - 6.12.2001*/// Implemented the method.
	int		oldStyleProperty;
	HRESULT	ret = getStyleProperty(78, &oldStyleProperty);
	if(SUCCEEDED(ret))
	{
		resbuf	styleProperty;
		styleProperty.restype = 78;
		styleProperty.resval.rint = (suppress) ? oldStyleProperty | 8 : oldStyleProperty & ~8;
		ret = setStyleProperty(styleProperty);
	}

	return ret;
}


STDMETHODIMP
CAutoDimension::get_TextColor(Colors* pColor)
{
	/*DG - 6.12.2001*/// Implemented the method.
	return getStyleProperty(178, (int*)pColor);
}


STDMETHODIMP
CAutoDimension::put_TextColor(Colors color)
{
	/*DG - 6.12.2001*/// Implemented the method.
	resbuf	styleProperty;
	styleProperty.restype = 178;
	styleProperty.resval.rint = color;
	return setStyleProperty(styleProperty);
}


STDMETHODIMP
CAutoDimension::get_TextGap(double* pGap)
{
	/*DG - 6.12.2001*/// Implemented the method.
	return getStyleProperty(147, pGap);
}


STDMETHODIMP
CAutoDimension::put_TextGap(double gap)
{
	/*DG - 6.12.2001*/// Implemented the method.
	resbuf	styleProperty;
	styleProperty.restype = 147;
	styleProperty.resval.rreal = gap;
	return setStyleProperty(styleProperty);
}


STDMETHODIMP
CAutoDimension::get_TextHeight(double* pHeight)
{
	/*DG - 6.12.2001*/// Implemented the method.
	return getStyleProperty(140, pHeight);
}


STDMETHODIMP
CAutoDimension::put_TextHeight(double height)
{
	/*DG - 6.12.2001*/// Implemented the method.
	resbuf	styleProperty;
	styleProperty.restype = 140;
	styleProperty.resval.rreal = height;
	return setStyleProperty(styleProperty);
}


STDMETHODIMP
CAutoDimension::get_TextMovement(DimTextMovement* pMovement)
{
	/*DG - 6.12.2001*/// Implemented the method.
	return getStyleProperty(279, (int*)pMovement);
}


STDMETHODIMP
CAutoDimension::put_TextMovement(DimTextMovement movement)
{
	/*DG - 6.12.2001*/// Implemented the method.
	resbuf	styleProperty;
	styleProperty.restype = 279;
	styleProperty.resval.rint = movement;
	return setStyleProperty(styleProperty);
}


STDMETHODIMP
CAutoDimension::get_TextPosition(IIcadPoint** ppPosition)
{
	/*DG - 6.12.2001*/// Implemented the method.
	RETURN_IF_INVALID((m_pDbHandItem && ppPosition))
	
	sds_point	pt;
	m_pDbHandItem->get_11(pt);
	
	CComObject<CAutoPoint>*	pPoint;
	CComObject<CAutoPoint>::CreateInstance(&pPoint);
	if(!pPoint)
		return E_FAIL;
	
	pPoint->Init(NULL, NULL, pt[0], pt[1], pt[2]);
	
	return pPoint->QueryInterface(ppPosition);
}


STDMETHODIMP
CAutoDimension::put_TextPosition(IIcadPoint* pPosition)
{
	/*DG - 6.12.2001*/// Implemented the method.
	RETURN_IF_INVALID(m_pDbHandItem)
	sds_point	pt = {0.0, 0.0, 0.0};
	if(pPosition)
	{
		pPosition->get_x(&pt[0]);
		pPosition->get_y(&pt[1]);
		pPosition->get_z(&pt[2]);
	}
	m_pDbHandItem->set_11(pt);
	
	return S_OK;
}


STDMETHODIMP
CAutoDimension::get_TextOverride(BSTR* pString)
{
	RETURN_IF_INVALID((m_pDbHandItem && pString))

	char*	pStr;
	m_pDbHandItem->get_1(&pStr);

	CString	str(pStr);
	*pString = str.AllocSysString();

	return S_OK;
}


STDMETHODIMP
CAutoDimension::put_TextOverride(BSTR string)
{
	RETURN_IF_INVALID(m_pDbHandItem)
	
	CString	str(string);
	if(str.IsEmpty())
		return E_INVALIDARG;

	if(!m_pDbHandItem->set_1((char*)str.GetBuffer(0)))
		return E_FAIL;

	return S_OK;
}


STDMETHODIMP
CAutoDimension::get_TextPrefix(BSTR* pPrefixString)
{
	/*DG - 6.12.2001*/// Implemented the method.
	RETURN_IF_INVALID(pPrefixString)

	char*	pStyleProperty = NULL;
	HRESULT	ret = getStyleProperty(3, &pStyleProperty);

	if(SUCCEEDED(ret))
	{
		CString	str(pStyleProperty);
		// EBATECH(CNBR) -[ 2002/5/15
		int nPos = str.Find("<>");
		if( nPos > 0 )
			str = str.Left(nPos);
		//str = str.Left(str.Find('<'*//*DNT*/));
		// EBATECH(CNBR) ]-
		*pPrefixString = str.AllocSysString();
	}

	IC_FREE(pStyleProperty);

	return ret;
}


STDMETHODIMP
CAutoDimension::put_TextPrefix(BSTR prefixString)
{
	/*DG - 6.12.2001*/// Implemented the method.
	char*	pOldStyleProperty = NULL;
	HRESULT	ret = getStyleProperty(3, &pOldStyleProperty);

	if(SUCCEEDED(ret))
	{
		CString	str(pOldStyleProperty);
		// EBATECH(CNBR) -[ 2002/5/15 
		CString SuffixString("");
		int nPos = str.Find("<>");
		if( nPos > 0 )
			SuffixString = str.Mid(nPos+2);
		str = CString(prefixString) + CString("<>") + SuffixString;
		if( str.Compare("<>") == 0 )
			str = CString("");
		//str = CString(PrefixString) + str.Right(str.GetLength() - str.Find('<'/*DNT*/));
		// EBATECH(CNBR) ]-

		resbuf	styleProperty;
		styleProperty.restype = 3;
		styleProperty.resval.rstring = (char*)(LPCTSTR)str;
		ret = setStyleProperty(styleProperty);
	}

	IC_FREE(pOldStyleProperty);

	return ret;
}


STDMETHODIMP
CAutoDimension::get_TextString(BSTR* pString)
{
	RETURN_IF_INVALID((m_pDbHandItem && pString))
	
	db_text*	pText = NULL;
	CDbMText*	pMText = NULL;

	((db_dimension*)m_pDbHandItem)->get_texthandle(pText, pMText);

	char*	pStr;
	if(pMText)
		pMText->get_1(&pStr);
	else
		if(pText)
			pText->get_1(&pStr);
		else
			return E_FAIL;

	CString	str(pStr);
	*pString = str.AllocSysString();

	return S_OK;
}


STDMETHODIMP
CAutoDimension::put_TextString(BSTR string)
{
	RETURN_IF_INVALID(m_pDbHandItem)

	CString	str(string);
	if(str.IsEmpty())
		return E_INVALIDARG;

	db_text*	pText = NULL;
	CDbMText*	pMText = NULL;

	((db_dimension*)m_pDbHandItem)->get_texthandle(pText, pMText);

	if(pMText)
		pMText->set_1((char*)str.GetBuffer(0));
	else
		if(pText)
			pText->set_1((char*)str.GetBuffer(0));
		else
			return E_FAIL;

	return S_OK;
}


STDMETHODIMP
CAutoDimension::get_TextRotation(double* pRotation)
{
	RETURN_IF_INVALID((m_pDbHandItem && pRotation))
	
	if(!m_pDbHandItem->get_51(pRotation))
		return E_FAIL;

	return S_OK;
}


STDMETHODIMP
CAutoDimension::put_TextRotation(double rotation)
{
	RETURN_IF_INVALID(m_pDbHandItem)
	if(!m_pDbHandItem->set_51(rotation))
		return E_FAIL;

	return S_OK;
}


STDMETHODIMP
CAutoDimension::get_TextStyle(BSTR* pStyleName)
{
	/*DG - 6.12.2001*/// Implemented the method.
	RETURN_IF_INVALID((m_pDbHandItem && pStyleName))

	char*	pStyleProperty = NULL;
	HRESULT	ret = getStyleProperty(340, &pStyleProperty, true);

	if(SUCCEEDED(ret))
	{
		CString	str(pStyleProperty);
		*pStyleName = str.AllocSysString();
	}

	IC_FREE(pStyleProperty);

	return ret;
}


STDMETHODIMP
CAutoDimension::put_TextStyle(BSTR styleName)
{
	/*DG - 6.12.2001*/// Implemented the method.
	CString			str(styleName);
	db_handitem*	pTabRec = m_pDoc->m_dbDrawing->findtabrec(DB_STYLETAB, (LPCTSTR)str, 1);
	resbuf			styleProperty;

	if(!pTabRec)
		return E_INVALIDARG;

	styleProperty.restype = 340;
	styleProperty.resval.rstring = new char [17];
	pTabRec->RetHandlePtr()->ToAscii(styleProperty.resval.rstring);

	HRESULT ret = setStyleProperty(styleProperty);

	IC_FREE(styleProperty.resval.rstring);
	return ret;
}


STDMETHODIMP
CAutoDimension::get_TextSuffix(BSTR* pSuffixString)
{
	/*DG - 6.12.2001*/// Implemented the method.
	RETURN_IF_INVALID(pSuffixString)

	char*	pStyleProperty = NULL;
	HRESULT	ret = getStyleProperty(3, &pStyleProperty);

	if(SUCCEEDED(ret))
	{
		CString	str(pStyleProperty);
		// EBATECH(CNBR) -[ 2002/5/15
		int nPos = str.Find("<>");
		if( nPos >= 0 )
			str = str.Mid( nPos + 2 );
		else
			str = CString("");
		//str = str.Right(str.GetLength() - str.Find('>'/*DNT*/) - 1);
		// EBATECH(CNBR) ]-
		*pSuffixString = str.AllocSysString();
	}

	IC_FREE(pStyleProperty);

	return ret;
}


STDMETHODIMP
CAutoDimension::put_TextSuffix(BSTR suffixString)
{
	/*DG - 6.12.2001*/// Implemented the method.
	char*	pOldStyleProperty = NULL;
	HRESULT	ret = getStyleProperty(3, &pOldStyleProperty);

	if(SUCCEEDED(ret))
	{
		CString	str(pOldStyleProperty);
		// EBATECH(CNBR) -[ 2002/5/15
		CString prefixString;
		int nPos = str.Find("<>");
		if( nPos < 0 )
			prefixString = str;
		else
			prefixString = str.Left(nPos);
		str = prefixString + CString("<>") + CString(suffixString);
		if( str.Compare("<>") == 0 )
			str = CString("");
		//str = str.Left(str.Find('>'/*DNT*/) + 1) + CString(suffixString);
		// EBATECH(CNBR) ]-

		resbuf	styleProperty;
		styleProperty.restype = 3;
		styleProperty.resval.rstring = (char*)(LPCTSTR)str;
		ret = setStyleProperty(styleProperty);
	}

	IC_FREE(pOldStyleProperty);

	return ret;
}


/*DG - 6.12.2001*/// Notes about ToleranceDisplay. Acad's default values:
//
//				DIMTOL	DIMTM			DIMTP		DIMLIM		DIMGAP
// None			0		0.0				0.0			0			0.625
// Symmetrical	1		0.0				0.0			0			0.625
// Deviation	1		0.000000001		0.0			0			0.625
// Limits		0		0.000000001		0.0			1			0.625
// Basic		0		0.000000001		0.0			0			-0.625
//

STDMETHODIMP
CAutoDimension::get_ToleranceDisplay(DimToleranceMethod* pTolDisplay)
{
	/*DG - 6.12.2001*/// Implemented the method.
	RETURN_IF_INVALID((m_pDbHandItem && pTolDisplay))
	
	db_dimstyletabrec*	pDimStyle = NULL;

	m_pDbHandItem->get_3((db_handitem**)&pDimStyle);
	if(!pDimStyle)
		return E_FAIL;

	resbuf	rbDimtol = {NULL, 71, 0}, rbDimlim = {NULL, 72, 0},
			rbDimtp = {NULL, 47, 0}, rbDimtm = {NULL, 48, 0}, rbDimgap = {NULL, 147, 0};

	if(!getOverrideValue(rbDimtol))
		pDimStyle->get_71((int*)&rbDimtol.resval.rint);

	if(!getOverrideValue(rbDimlim))
		pDimStyle->get_72((int*)&rbDimlim.resval.rint);

	if(!getOverrideValue(rbDimtp))
		pDimStyle->get_47(&rbDimtp.resval.rreal);

	if(!getOverrideValue(rbDimtm))
		pDimStyle->get_48(&rbDimtm.resval.rreal);

	if(!getOverrideValue(rbDimgap))
		pDimStyle->get_147(&rbDimgap.resval.rreal);

	if(rbDimtol.resval.rint)
	{
		if(rbDimtp.resval.rreal < IC_ZRO && rbDimtm.resval.rreal < IC_ZRO)
			*pTolDisplay = vicTolSymmetrical;
		else
			*pTolDisplay = vicTolDeviation;
	}
	else
	{
		if(rbDimlim.resval.rint)
			*pTolDisplay = vicTolLimits;
		else
			*pTolDisplay = (rbDimgap.resval.rreal < 0.0) ? vicTolBasic : vicTolNone;
	}

	return S_OK;
}


STDMETHODIMP
CAutoDimension::put_ToleranceDisplay(DimToleranceMethod tolDisplay)
{
	/*DG - 6.12.2001*/// Implemented the method.
	resbuf				rbDimtol = {NULL, 71, 0}, rbDimlim = {NULL, 72, 0},
						rbDimtp = {NULL, 47, 0}, rbDimtm = {NULL, 48, 0}, rbDimgap = {NULL, 147, 0};
	bool				bSetDimtp = false, bSetDimtm = false, bSetDimgap = false;
	db_dimstyletabrec*	pDimStyle = NULL;

	m_pDbHandItem->get_3((db_handitem**)&pDimStyle);
	if(!pDimStyle)
		return E_FAIL;

	rbDimtol.resval.rint = (tolDisplay == vicTolSymmetrical || tolDisplay == vicTolDeviation) ? 1: 0;
	rbDimlim.resval.rint = (tolDisplay == vicTolLimits) ? 1: 0;

	if(tolDisplay == vicTolNone || tolDisplay == vicTolSymmetrical)
	{
		rbDimtm.resval.rreal = rbDimtp.resval.rreal = 0.0;
		bSetDimtm = bSetDimtp = true;
	}
	else
	{
		if(!getOverrideValue(rbDimtp))
			pDimStyle->get_47(&rbDimtp.resval.rreal);

		if(!rbDimtp.resval.rreal)
		{
			rbDimtp.resval.rreal = 0.000000001;		// acad sets dimtp to this val
			bSetDimtp = true;
		}

		if(!getOverrideValue(rbDimtm))
			pDimStyle->get_48(&rbDimtm.resval.rreal);

		if(!rbDimtm.resval.rreal)
		{
			rbDimtm.resval.rreal = 0.000000001;		// acad sets dimtm to this val
			bSetDimtm = true;
		}
	}

	if(!getOverrideValue(rbDimgap))
		pDimStyle->get_147(&rbDimgap.resval.rreal);

	if((tolDisplay == vicTolBasic && rbDimgap.resval.rreal > 0.0) || (tolDisplay != vicTolBasic && rbDimgap.resval.rreal < 0.0))
	{
		rbDimgap.resval.rreal = - rbDimgap.resval.rreal;
		bSetDimgap = true;
	}


	HRESULT		ret;

	ret = setStyleProperty(rbDimtol);
	if(!SUCCEEDED(ret))
		return ret;

	ret = setStyleProperty(rbDimlim);
	if(!SUCCEEDED(ret))
		return ret;

	if(bSetDimtp)
	{
		ret = setStyleProperty(rbDimtp);
		if(!SUCCEEDED(ret))
			return ret;
	}

	if(bSetDimtm)
	{
		ret = setStyleProperty(rbDimtm);
		if(!SUCCEEDED(ret))
			return ret;
	}

	if(bSetDimgap)
	{
		ret = setStyleProperty(rbDimgap);
		if(!SUCCEEDED(ret))
			return ret;
	}

	return ret;
}


STDMETHODIMP
CAutoDimension::get_ToleranceHeightScale(double* pHeightScale)
{
	/*DG - 6.12.2001*/// Implemented the method.
	return getStyleProperty(146, pHeightScale);
}


STDMETHODIMP
CAutoDimension::put_ToleranceHeightScale(double heightScale)
{
	/*DG - 6.12.2001*/// Implemented the method.
	resbuf	styleProperty;
	styleProperty.restype = 146;
	styleProperty.resval.rreal = heightScale;
	return setStyleProperty(styleProperty);
}


STDMETHODIMP
CAutoDimension::get_ToleranceJustification(DimToleranceJustify* pTolJustify)
{
	/*DG - 6.12.2001*/// Implemented the method.
	return getStyleProperty(283, (int*)pTolJustify);
}


STDMETHODIMP
CAutoDimension::put_ToleranceJustification(DimToleranceJustify tolJustify)
{
	/*DG - 6.12.2001*/// Implemented the method.
	resbuf	styleProperty;
	styleProperty.restype = 283;
	styleProperty.resval.rint = tolJustify;
	return setStyleProperty(styleProperty);
}


STDMETHODIMP
CAutoDimension::get_ToleranceLowerLimit(double* pLimit)
{
	/*DG - 6.12.2001*/// Implemented the method.
	return getStyleProperty(48, pLimit);
}


STDMETHODIMP
CAutoDimension::put_ToleranceLowerLimit(double limit)
{
	/*DG - 6.12.2001*/// Implemented the method.
	resbuf	styleProperty;
	styleProperty.restype = 48;
	styleProperty.resval.rreal = limit;
	return setStyleProperty(styleProperty);
}


STDMETHODIMP
CAutoDimension::get_ToleranceSuppressLeadingZeroes(VARIANT_BOOL* pSuppress)
{
	/*DG - 6.12.2001*/// Implemented the method.
	RETURN_IF_INVALID(pSuppress)

	int		styleProperty;
	HRESULT	ret = getStyleProperty(284, &styleProperty);

	if(SUCCEEDED(ret))
		*pSuppress = (styleProperty & 4) ? VARIANT_TRUE : VARIANT_FALSE;

	return ret;
}


STDMETHODIMP
CAutoDimension::put_ToleranceSuppressLeadingZeroes(VARIANT_BOOL suppress)
{
	/*DG - 6.12.2001*/// Implemented the method.
	int		oldStyleProperty;
	HRESULT	ret = getStyleProperty(284, &oldStyleProperty);
	if(SUCCEEDED(ret))
	{
		resbuf	styleProperty;
		styleProperty.restype = 284;
		styleProperty.resval.rint = (suppress) ? oldStyleProperty | 4 : oldStyleProperty & ~4;
		ret = setStyleProperty(styleProperty);
	}

	return ret;
}


STDMETHODIMP
CAutoDimension::get_ToleranceSuppressTrailingZeroes(VARIANT_BOOL* pSuppress)
{
	/*DG - 6.12.2001*/// Implemented the method.
	RETURN_IF_INVALID(pSuppress)

	int		styleProperty;
	HRESULT	ret = getStyleProperty(284, &styleProperty);

	if(SUCCEEDED(ret))
		*pSuppress = (styleProperty & 8) ? VARIANT_TRUE : VARIANT_FALSE;

	return ret;
}


STDMETHODIMP
CAutoDimension::put_ToleranceSuppressTrailingZeroes(VARIANT_BOOL suppress)
{
	/*DG - 6.12.2001*/// Implemented the method.
	int		oldStyleProperty;
	HRESULT	ret = getStyleProperty(284, &oldStyleProperty);
	if(SUCCEEDED(ret))
	{
		resbuf	styleProperty;
		styleProperty.restype = 284;
		styleProperty.resval.rint = (suppress) ? oldStyleProperty | 8 : oldStyleProperty & ~8;
		ret = setStyleProperty(styleProperty);
	}

	return ret;
}


STDMETHODIMP
CAutoDimension::get_ToleranceSuppressZeroInches(VARIANT_BOOL* pSuppress)
{
	/*DG - 6.12.2001*/// Implemented the method.
	RETURN_IF_INVALID(pSuppress)

	int		styleProperty;
	HRESULT	ret = getStyleProperty(284, &styleProperty);

	if(SUCCEEDED(ret))
		*pSuppress = (styleProperty & 1) ? VARIANT_FALSE : VARIANT_TRUE;

	return ret;
}


STDMETHODIMP
CAutoDimension::put_ToleranceSuppressZeroInches(VARIANT_BOOL suppress)
{
	/*DG - 6.12.2001*/// Implemented the method.
	int		oldStyleProperty;
	HRESULT	ret = getStyleProperty(284, &oldStyleProperty);
	if(SUCCEEDED(ret))
	{
		resbuf	styleProperty;
		styleProperty.restype = 284;
		styleProperty.resval.rint = (suppress) ? oldStyleProperty & ~1 : oldStyleProperty | 1;
		ret = setStyleProperty(styleProperty);
	}

	return ret;
}


STDMETHODIMP
CAutoDimension::get_ToleranceSuppressZeroFeet(VARIANT_BOOL* pSuppress)
{
	/*DG - 6.12.2001*/// Implemented the method.
	RETURN_IF_INVALID(pSuppress)

	int		styleProperty;
	HRESULT	ret = getStyleProperty(284, &styleProperty);

	if(SUCCEEDED(ret))
		*pSuppress = (!(styleProperty & 3) || (styleProperty & 3) == 3) ? VARIANT_TRUE : VARIANT_FALSE;

	return ret;
}


STDMETHODIMP
CAutoDimension::put_ToleranceSuppressZeroFeet(VARIANT_BOOL suppress)
{
	/*DG - 6.12.2001*/// Implemented the method.
	int		oldStyleProperty;
	HRESULT	ret = getStyleProperty(284, &oldStyleProperty);
	if(SUCCEEDED(ret))
	{
		resbuf	styleProperty;
		styleProperty.restype = 284;
		if(suppress)
			styleProperty.resval.rint = (oldStyleProperty & 1) ? oldStyleProperty | 3 : oldStyleProperty & ~3;
		else
			styleProperty.resval.rint = (oldStyleProperty & 1) ? oldStyleProperty | 2 : oldStyleProperty | 1;
		ret = setStyleProperty(styleProperty);
	}

	return ret;
}


STDMETHODIMP
CAutoDimension::get_ToleranceUpperLimit(double* pLimit)
{
	/*DG - 6.12.2001*/// Implemented the method.
	return getStyleProperty(47, pLimit);
}


STDMETHODIMP
CAutoDimension::put_ToleranceUpperLimit(double limit)
{
	/*DG - 6.12.2001*/// Implemented the method.
	resbuf	styleProperty;
	styleProperty.restype = 47;
	styleProperty.resval.rreal = limit;
	return setStyleProperty(styleProperty);
}


STDMETHODIMP
CAutoDimension::get_TolerancePrecision(DimPrecision* pPrecision)
{
	/*DG - 6.12.2001*/// Implemented the method.
	return getStyleProperty(272, (int*)pPrecision);
}


STDMETHODIMP
CAutoDimension::put_TolerancePrecision(DimPrecision precision)
{
	/*DG - 6.12.2001*/// Implemented the method.
	resbuf	styleProperty;
	styleProperty.restype = 272;
	styleProperty.resval.rint = precision;
	return setStyleProperty(styleProperty);
}


STDMETHODIMP
CAutoDimension::get_UnitsFormat(DimLUnits* pUnits)
{
	/*DG - 6.12.2001*/// Implemented the method.
	HRESULT	ret = getStyleProperty(277, (int*)pUnits);

	if(SUCCEEDED(ret))
		*pUnits = (DimLUnits)(*pUnits - 1);

	return ret;
}


STDMETHODIMP
CAutoDimension::put_UnitsFormat(DimLUnits units)
{
	/*DG - 6.12.2001*/// Implemented the method.
	resbuf	styleProperty;
	styleProperty.restype = 277;
	styleProperty.resval.rint = units + 1;
	return setStyleProperty(styleProperty);
}


STDMETHODIMP
CAutoDimension::get_VerticalTextPosition(DimVerticalJustification* pPosition)
{
	/*DG - 6.12.2001*/// Implemented the method.
	return getStyleProperty(77, (int*)pPosition);
}


STDMETHODIMP
CAutoDimension::put_VerticalTextPosition(DimVerticalJustification position)
{
	/*DG - 6.12.2001*/// Implemented the method.
	resbuf	styleProperty;
	styleProperty.restype = 77;
	styleProperty.resval.rint = position;
	return setStyleProperty(styleProperty);
}
