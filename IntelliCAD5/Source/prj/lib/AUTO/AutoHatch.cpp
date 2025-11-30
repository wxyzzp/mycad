/* INTELLICAD\PRJ\LIB\AUTO\AUTOHATCH.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * $Revision: 1.4 $ $Date: 2001/10/26 15:13:32 $ 
 * 
 * Abstract
 * 
 * Implementation of CAutoHatch - the Automation Hatch object
 * 
 */ 

#include "StdAfx.h"
#include "AutoPoint.h"
#include "AutoVector.h"
#include "AutoHatch.h"
#include "AutoHelpers.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoHatch

HRESULT CAutoHatch::Init(CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem)
	{
	CAutoDbItem::Init(pDoc, pParent);
	
	if (pDbHandItem)
		m_pDbHandItem = pDbHandItem;
	else
		{
		if (pDoc)
			m_pDbHandItem = new db_hatch(pDoc->m_dbDrawing);
		else
			m_pDbHandItem = new db_hatch();
		}
	
	if (NULL == m_pDbHandItem) 
		return FALSE;
	
	return NOERROR;
	}

STDMETHODIMP CAutoHatch::get_AssociativeHatch (VARIANT_BOOL * Associative)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (Associative == NULL)
		return E_POINTER;
	
	int flags;
	m_pDbHandItem->get_71 (&flags);

	*Associative = (flags = 1) ? VARIANT_TRUE : VARIANT_FALSE;
	return NOERROR;
	}

STDMETHODIMP CAutoHatch::get_Normal (IIcadVector * * ppExtrusionDirection)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppExtrusionDirection == NULL)
		return E_POINTER;
	
	sds_point v;
	m_pDbHandItem->get_210(v);
	
	CComObject<CAutoVector> *pVector;
	CComObject<CAutoVector>::CreateInstance(&pVector);
	if (!pVector)
		return E_FAIL;
	
	pVector->Init(NULL, v[0], v[1], v[2]);
	
	return pVector->QueryInterface (ppExtrusionDirection);
	}

STDMETHODIMP CAutoHatch::put_Normal (IIcadVector * ExtrusionDirection)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	sds_point v;
	v[0] = v[1] = v[2] = 1.0;
	if (ExtrusionDirection)
		{
		ExtrusionDirection->get_x(&v[0]);
		ExtrusionDirection->get_y(&v[1]);
		ExtrusionDirection->get_z(&v[2]);

		if (db_unitizevector(v) != 0)
			return E_INVALIDARG;
		}
	m_pDbHandItem->init_210 (v);
	
	return NOERROR;
	}

STDMETHODIMP CAutoHatch::get_HatchStyle (HatchStyle * pHatch)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pHatch == NULL)
		return E_POINTER;
	
	m_pDbHandItem->get_75 ((int *)pHatch);
	return NOERROR;
	}

STDMETHODIMP CAutoHatch::put_HatchStyle (HatchStyle pHatch)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pHatch < vicHatchStyleNormal || pHatch > vicHatchStyleIgnore)
		return E_INVALIDARG;

	m_pDbHandItem->set_75 (pHatch);
	return NOERROR;
	}

STDMETHODIMP CAutoHatch::get_PatternAngle (double * pAngle)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pAngle == NULL)
		return E_POINTER;
	
	m_pDbHandItem->get_52 (pAngle);
	return NOERROR;
	}

STDMETHODIMP CAutoHatch::put_PatternAngle (double pAngle)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	m_pDbHandItem->set_52 (pAngle);
	return NOERROR;
	}

STDMETHODIMP CAutoHatch::get_PatternDouble (VARIANT_BOOL * pPat)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pPat == NULL)
		return E_POINTER;
	
	int flags;
	m_pDbHandItem->get_77 (&flags);

	*pPat = (flags = 1) ? VARIANT_TRUE : VARIANT_FALSE;
	return NOERROR;
	}

STDMETHODIMP CAutoHatch::put_PatternDouble (VARIANT_BOOL pPat)
	{
	RETURN_IF_INVALID (m_pDbHandItem);

	int flags  = 0;	
	if (pPat == VARIANT_TRUE) 
		flags = 1;
	
	m_pDbHandItem->set_77 (flags);
	return NOERROR;
	}

STDMETHODIMP CAutoHatch::get_PatternName (BSTR * pPatternName)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pPatternName == NULL)
		return E_POINTER;
			
	char* pStr;
	if ( !m_pDbHandItem->get_2 (&pStr)) 
		return E_FAIL;
	
	CString str(pStr);
	*pPatternName = str.AllocSysString();
	return NOERROR;
	}

STDMETHODIMP CAutoHatch::put_PatternName (BSTR pPatternName)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	CString str(pPatternName);

	/*DG - 26.10.2001*/// Set the appropriate flag if it's 'SOLID' and don't check if the pattern exists.
	if(!str.CompareNoCase("SOLID"/*DNT*/))
		m_pDbHandItem->set_70(1);
	else
		if(FAILED(m_pDoc->FindHatchPattern(str)))
			return E_INVALIDARG;

	if(!m_pDbHandItem->set_2(str.GetBuffer(0)))
		return E_FAIL;
	
	return NOERROR;
}

STDMETHODIMP CAutoHatch::get_PatternScale (double * pScale)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pScale == NULL)
		return E_POINTER;
	
	m_pDbHandItem->get_41 (pScale);
	return NOERROR;
	}

STDMETHODIMP CAutoHatch::put_PatternScale (double pScale)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	m_pDbHandItem->set_41 (pScale);
	return NOERROR;
	}

STDMETHODIMP CAutoHatch::get_PatternSpace (double * pSpace)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pSpace == NULL)
		return E_POINTER;
	
	m_pDbHandItem->get_41 (pSpace);
	return NOERROR;
	}

STDMETHODIMP CAutoHatch::put_PatternSpace (double pSpace)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	m_pDbHandItem->set_41 (pSpace);
	return NOERROR;
	}

STDMETHODIMP CAutoHatch::get_PatternType (HatchType * pType)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pType == NULL)
		return E_POINTER;
	
	m_pDbHandItem->get_76 ((int *)pType);
	return NOERROR;
	}

STDMETHODIMP CAutoHatch::put_PatternType (HatchType pType)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if ((pType < vicHatchPatternTypeUserDefined) || (pType > vicHatchPatternTypeCustomDefined))
		return E_INVALIDARG;

	m_pDbHandItem->set_76 (pType);
	return NOERROR;
	}

/*DG - 26.10.2001*/// Added the method.
STDMETHODIMP CAutoHatch::get_SolidFillFlag(int* pSolidFillFlag)
{
	RETURN_IF_INVALID (m_pDbHandItem);

	if(!pSolidFillFlag)
		return E_POINTER;

	m_pDbHandItem->get_70(pSolidFillFlag);

	return NOERROR;
}

/*DG - 26.10.2001*/// Added the method.
STDMETHODIMP CAutoHatch::put_SolidFillFlag(int solidFillFlag)
{
	RETURN_IF_INVALID (m_pDbHandItem);

	if(solidFillFlag && solidFillFlag != 1)
		return E_INVALIDARG;

	m_pDbHandItem->set_70(solidFillFlag);

	return NOERROR;
}

STDMETHODIMP CAutoHatch::SetPattern (HatchType Type, BSTR PatternName)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	m_pDbHandItem->set_76 (Type);

	CString str(PatternName);
	if (!ic_isvalidname(str.GetBuffer(0)))
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_2 (str.GetBuffer(0)))
		return E_FAIL;
	
	return NOERROR;
	}

STDMETHODIMP CAutoHatch::ApplyHatch (IIcadSelectionSet * ObjectsToHatch, IIcadPoints *SeedPoints)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	sds_name sset;
	HRESULT hr;

	if (!ObjectsToHatch)
		return E_INVALIDARG;

	long npts = 0, i;
	if (SeedPoints)
		{
		SeedPoints->get_Count (&npts);

		sds_point pt;
		for (i = 0; i < npts; i++)
			{
			CComPtr<IIcadPoint> pPoint;
			SeedPoints->Item(i+1, &pPoint); //AutoPoints is 1 based indexing
			pPoint->get_x(&pt[0]);
			pPoint->get_y(&pt[1]);
			pPoint->get_z(&pt[2]);
			pPoint = NULL;

			((db_hatch *)m_pDbHandItem)->set_seed(pt, i);
			}
		}
	
	hr = ConvertAutoSelectionSetToSDSsset (ObjectsToHatch, sset, m_pDoc);
	if (FAILED(hr))
		return E_FAIL;
	
	hr = m_pDoc->ApplyHatch (sset, (db_hatch *)m_pDbHandItem);
	m_pDoc->AUTO_ssfree (sset);
	if (SUCCEEDED(hr))
		{
		hr = m_pOwner->AddItemToDrawing (m_pDbHandItem);
		if (FAILED(hr))
			return E_FAIL;
		}
	else
		{
		m_bDeleteHanditem = TRUE;
		}

	return hr;
	}
