// File: <DevDir>\sources\prj\lib\auto\AutoDimRadial.cpp
//

#include "stdafx.h"
#include "AutoDimRadial.h"
#include "dimstyletabrec.h"


int dimblkNum(CString&	dimblkName);
extern const char* dimBlks[];


HRESULT
CAutoDimRadial::Init(CIcadDoc* pDoc, IDispatch* pParent, db_handitem* pDbHandItem)
{
	HRESULT	hr = CAutoDimension::Init(pDoc, pParent, pDbHandItem);

	if(SUCCEEDED(hr))
		m_pDbHandItem->set_70(4);

	return hr;
}


STDMETHODIMP
CAutoDimRadial::get_ArrowheadBlock(BSTR* BlockName)
{
	RETURN_IF_INVALID((m_pDbHandItem && BlockName))

	char*	pStyleProperty = NULL;
	HRESULT	ret = getStyleProperty(341, &pStyleProperty, true);

	if(SUCCEEDED(ret))
	{
		CString	str(pStyleProperty);
		*BlockName = str.AllocSysString();
	}

	IC_FREE(pStyleProperty);

	return ret;
}


STDMETHODIMP
CAutoDimRadial::put_ArrowheadBlock(BSTR BlockName)
{
	CString			str(BlockName);
	db_handitem*	pTabRec = m_pDoc->m_dbDrawing->findtabrec(DB_BLOCKTAB, (LPCTSTR)str, 1);
	resbuf			styleProperty;

	if(!pTabRec)
		return E_INVALIDARG;

	styleProperty.restype = 341;
	styleProperty.resval.rstring = new char [17];
	pTabRec->RetHandlePtr()->ToAscii(styleProperty.resval.rstring);

	HRESULT ret = setStyleProperty(styleProperty);

	IC_FREE(styleProperty.resval.rstring);
	return ret;
}


STDMETHODIMP
CAutoDimRadial::get_ArrowheadType(DimArrowheadType* ArrowheadType)
{
	char*	pStyleProperty = NULL;
	HRESULT	ret = getStyleProperty(341, &pStyleProperty);

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


STDMETHODIMP
CAutoDimRadial::put_ArrowheadType(DimArrowheadType ArrowheadType)
{
	if(ArrowheadType == vicArrowUserDefined)
		return S_OK;

	CString	str(dimBlks[ArrowheadType]);
	BSTR	bstr = str.AllocSysString();
	HRESULT	ret = put_ArrowheadBlock(bstr);
	SysFreeString(bstr);
	return ret;
}


STDMETHODIMP
CAutoDimRadial::get_DimLineSuppress(VARIANT_BOOL* Supress)
{
	return CAutoDimension::get_DimLine2Suppress(Supress);
}


STDMETHODIMP
CAutoDimRadial::put_DimLineSuppress(VARIANT_BOOL Suppress)
{
	return CAutoDimension::put_DimLine2Suppress(Suppress);
}
