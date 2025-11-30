/* INTELLICAD\PRJ\LIB\AUTO\AUTOIMAGE.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * $Revision: 1.3 $ $Date: 2001/03/16 21:48:11 $ 
 * 
 * Abstract
 * 
 * Implementation of CAutoImage - the Automation Image object
 * 
 */ 

#include "StdAfx.h"
#include "AutoPoint.h"
#include "AutoVector.h"
#include "AutoImage.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoImage

HRESULT CAutoImage::Init(CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem)
	{
	CAutoDbItem::Init(pDoc, pParent);
	
	if (pDbHandItem)
		m_pDbHandItem = pDbHandItem;
	else
		{
		ASSERT(0);	//should never get here.  images are not created this way - Entities.AddImage should be used
		m_pDbHandItem = new db_image();
		}
	
	if (NULL == m_pDbHandItem) 
		return FALSE;
	
	return NOERROR;
	}

STDMETHODIMP CAutoImage::get_ImageFile (BSTR * pImageFileName)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pImageFileName == NULL)
		return E_POINTER;
	
	db_handitem *imagedef_hip = NULL;
	m_pDbHandItem->get_340 (&imagedef_hip);
	if (!imagedef_hip)
		return E_FAIL;

	char* pStr;
	imagedef_hip->get_1 (&pStr);
		
	CString str (pStr);
	*pImageFileName = str.AllocSysString();

	return NOERROR;
	}

STDMETHODIMP CAutoImage::put_ImageFile (BSTR pImageFileName)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	CString str(pImageFileName);

	db_handitem *imagedef_hip = NULL;
	m_pDbHandItem->get_340 (&imagedef_hip);
	if (!imagedef_hip)
		return E_FAIL;

	if ( !imagedef_hip->set_1 (str.GetBuffer(0))) 
		return E_FAIL;
	
	return NOERROR;
	}

STDMETHODIMP CAutoImage::get_ImageVisibility (VARIANT_BOOL * pVisible)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pVisible == NULL)
		return E_POINTER;
	
	int flags = 0;
	if (!m_pDbHandItem->get_70(&flags)) 
		return E_FAIL;
	
	*pVisible = (flags == 1) ? VARIANT_TRUE : VARIANT_FALSE;
	
	return NOERROR;
	}

STDMETHODIMP CAutoImage::put_ImageVisibility (VARIANT_BOOL pVisible)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	int flags = 0;
	
	if (pVisible == VARIANT_TRUE) 
		flags = 1;
	
	if (!m_pDbHandItem->set_70(flags)) 
		return E_FAIL;
	
	return NOERROR;
	}

STDMETHODIMP CAutoImage::get_Origin (IIcadPoint * * ppOrigin)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppOrigin == NULL)
		return E_POINTER;
	
	sds_point pt;
	m_pDbHandItem->get_10(pt);
	
	CComObject<CAutoPoint> *pPoint;
	CComObject<CAutoPoint>::CreateInstance(&pPoint);
	if (!pPoint)
		return E_FAIL;
	
	pPoint->Init(NULL, NULL, pt[0], pt[1], pt[2]);
	
	return pPoint->QueryInterface (ppOrigin);
	}

STDMETHODIMP CAutoImage::put_Origin (IIcadPoint * ppOrigin)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	sds_point pt;
	pt[0] = pt[1] = pt[2] = 0.0;

	if (ppOrigin)	//this is an optional arg
		{
		ppOrigin->get_x(&pt[0]);
		ppOrigin->get_y(&pt[1]);
		ppOrigin->get_z(&pt[2]);
		}
	m_pDbHandItem->set_10(pt);
	
	return NOERROR;
	}

STDMETHODIMP CAutoImage::get_ImageHeight (double * pHeight)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pHeight == NULL)
		return E_POINTER;
	
	sds_point pt;
	m_pDbHandItem->get_13(pt);

	double height = pt[1];

	m_pDbHandItem->get_11(pt);

	*pHeight = height * sqrt((pt[0]*pt[0]) + (pt[1]*pt[1]) + (pt[2]*pt[2]));

	return NOERROR;
	}

STDMETHODIMP CAutoImage::put_ImageHeight (double pHeight)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	sds_point pt;
	double heightperpix ;

	m_pDbHandItem->get_12(pt);

	heightperpix = sqrt((pt[0]*pt[0]) + (pt[1]*pt[1]) + (pt[2]*pt[2]));

	m_pDbHandItem->get_13(pt);

	pt[1] = pHeight / heightperpix;

	m_pDbHandItem->set_13(pt);

	return NOERROR;
	}

STDMETHODIMP CAutoImage::get_ImageWidth (double * pWidth)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pWidth == NULL)
		return E_POINTER;
	
	sds_point pt;
	m_pDbHandItem->get_13(pt);

	double width = pt[1];

	m_pDbHandItem->get_11(pt);

	*pWidth = width * sqrt((pt[0]*pt[0]) + (pt[1]*pt[1]) + (pt[2]*pt[2]));

	return NOERROR;
	}

STDMETHODIMP CAutoImage::put_ImageWidth (double pWidth)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	sds_point pt;
	double widthperpix ;

	m_pDbHandItem->get_11(pt);

	widthperpix = sqrt((pt[0]*pt[0]) + (pt[1]*pt[1]) + (pt[2]*pt[2]));

	m_pDbHandItem->get_13(pt);

	pt[1] = pWidth / widthperpix;

	m_pDbHandItem->set_13(pt);

	return NOERROR;
	}

STDMETHODIMP CAutoImage::get_Height (double * pHeight)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pHeight == NULL)
		return E_POINTER;
	
	sds_point pt;
	m_pDbHandItem->get_13(pt);

	*pHeight = pt[1];

	return NOERROR;
	}

STDMETHODIMP CAutoImage::get_Width (double * pWidth)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pWidth == NULL)
		return E_POINTER;
	
	sds_point pt;
	m_pDbHandItem->get_13(pt);

	*pWidth = pt[0];

	return NOERROR;
	}



