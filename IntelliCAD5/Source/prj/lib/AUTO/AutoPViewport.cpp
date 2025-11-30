/* INTELLICAD\PRJ\LIB\AUTO\AUTOPVIEWPORT.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Implementation of CAutoPViewport - the Automation Paper space Viewport object
 * 
 */ 

#include "StdAfx.h"
#include "AutoPViewport.h"
#include "AutoPoint.h"
#include "AutoVector.h"
#include "viewport.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoPViewport

HRESULT CAutoPViewport::Init(CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem)
	{
	CAutoDbItem::Init(pDoc, pParent);
	
	if (pDbHandItem)
		m_pDbHandItem = pDbHandItem;
	else
		{
		if (pDoc)
			{
			m_pDbHandItem = new db_viewport(pDoc->m_dbDrawing);
			m_pDbHandItem->set_69(pDoc->m_dbDrawing->ret_ntabrecs(DB_VXTAB,0)+1);
			}
		else
			{
			m_pDbHandItem = new db_viewport();
			m_pDbHandItem->set_69((m_pDoc && m_pDoc->m_dbDrawing) ?
								  m_pDoc->m_dbDrawing->ret_ntabrecs(DB_VXTAB,0)+1 : 1);
			}
		}

	
	if (NULL == m_pDbHandItem) 
		return FALSE;
	
	return NOERROR;
	}

STDMETHODIMP CAutoPViewport::get_Center (IIcadPoint * * ppCenter)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppCenter == NULL)
		return E_POINTER;
	
	sds_point pt;
	m_pDbHandItem->get_10(pt);	/*D.G.*/// was get_12
	
	CComObject<CAutoPoint> *pPoint;
	CComObject<CAutoPoint>::CreateInstance(&pPoint);
	if (!pPoint)
		return E_FAIL;
	
	pPoint->Init(NULL, NULL, pt[0], pt[1], pt[2]);
	
	return pPoint->QueryInterface (ppCenter);
	}

STDMETHODIMP CAutoPViewport::put_Center (IIcadPoint * ppCenter)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	sds_point pt;
	pt[0] = pt[1] = pt[2] = 0.0;
	if (ppCenter)	//this is an optional arg
		{
		ppCenter->get_x(&pt[0]);
		ppCenter->get_y(&pt[1]);
		ppCenter->get_z(&pt[2]);
		}
	m_pDbHandItem->set_10(pt);	/*D.G.*/// was set_12
	
	return NOERROR;
	}

STDMETHODIMP CAutoPViewport::get_Direction (IIcadVector * * ppNormal)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppNormal == NULL)
		return E_POINTER;
	
	sds_point v;
	m_pDbHandItem->get_16(v);
	
	CComObject<CAutoVector> *pVector;
	CComObject<CAutoVector>::CreateInstance(&pVector);
	if (!pVector)
		return E_FAIL;
	
	pVector->Init(NULL, v[0], v[1], v[2]);
	
	return pVector->QueryInterface (ppNormal);
	}

STDMETHODIMP CAutoPViewport::put_Direction (IIcadVector * ppNormal)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	sds_point pt;
	pt[0] = 1.0; pt[1] = pt[2] = 0.0;

	if (ppNormal)
		{
		ppNormal->get_x(&pt[0]);
		ppNormal->get_y(&pt[1]);
		ppNormal->get_z(&pt[2]);

		sds_point v;
		ic_ptcpy (v, pt);
		if (db_unitizevector(v) != 0)
			return E_INVALIDARG;
		}
	
	m_pDbHandItem->set_16(pt);
	
	return NOERROR;
	}

STDMETHODIMP CAutoPViewport::get_GridOn (VARIANT_BOOL * pGridOn)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pGridOn == NULL)
		return E_POINTER;
	
	int bFlag;
	if (!m_pDbHandItem->get_70(&bFlag)) 
		return E_FAIL;

	if (bFlag & IC_VPENT_STATUS_GRIDON)
		*pGridOn = VARIANT_TRUE;
	else
		*pGridOn = VARIANT_FALSE;

	return NOERROR;
	}

STDMETHODIMP CAutoPViewport::put_GridOn (VARIANT_BOOL pGridOn)
	{
	RETURN_IF_INVALID (m_pDbHandItem);

	int bFlag = 0;
	if (!m_pDbHandItem->get_70(&bFlag)) 
		return E_FAIL;

	if (pGridOn == VARIANT_TRUE)
		bFlag |= IC_VPENT_STATUS_GRIDON;
	else
		bFlag &= ~IC_VPENT_STATUS_GRIDON;

	if (!m_pDbHandItem->set_70(bFlag)) 
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoPViewport::get_Height (double * pHeight)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pHeight == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_41(pHeight)) 
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoPViewport::put_Height (double pHeight)
	{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (pHeight < 0.0)
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_41(pHeight)) 
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoPViewport::get_LensLength (double * pLensLength)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pLensLength == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_42(pLensLength)) 
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoPViewport::put_LensLength (double pLensLength)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (!m_pDbHandItem->set_42(pLensLength)) 
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoPViewport::get_RemoveHiddenLines (VARIANT_BOOL * pRemoveHiddenLines)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pRemoveHiddenLines == NULL)
		return E_POINTER;
	
	int bFlag;
	if (!m_pDbHandItem->get_70(&bFlag)) 
		return E_FAIL;

	if (bFlag & IC_VPENT_STATUS_HIDDENINPLOT)
		*pRemoveHiddenLines = VARIANT_TRUE;
	else
		*pRemoveHiddenLines = VARIANT_FALSE;

	return NOERROR;
	}

STDMETHODIMP CAutoPViewport::put_RemoveHiddenLines (VARIANT_BOOL pRemoveHiddenLines)
	{
	RETURN_IF_INVALID (m_pDbHandItem);

	int bFlag = 0;
	if (!m_pDbHandItem->get_70(&bFlag)) 
		return E_FAIL;

	if (pRemoveHiddenLines == VARIANT_TRUE)
		bFlag |= IC_VPENT_STATUS_HIDDENINPLOT;
	else 
		bFlag &= ~IC_VPENT_STATUS_HIDDENINPLOT;

	if (!m_pDbHandItem->set_70(bFlag)) 
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoPViewport::get_SnapBasePoint (IIcadPoint * * ppSnapBasePoint)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppSnapBasePoint == NULL)
		return E_POINTER;
	
	sds_point pt;
	m_pDbHandItem->get_13(pt);
	
	CComObject<CAutoPoint> *pPoint;
	CComObject<CAutoPoint>::CreateInstance(&pPoint);
	if (!pPoint)
		return E_FAIL;
	
	pPoint->Init(NULL, NULL, pt[0], pt[1], pt[2]);
	
	return pPoint->QueryInterface (ppSnapBasePoint);
	}

STDMETHODIMP CAutoPViewport::put_SnapBasePoint (IIcadPoint * ppSnapBasePoint)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	sds_point pt;
	pt[0] = pt[1] = pt[2] = 0.0;
	if (ppSnapBasePoint)
		{
		ppSnapBasePoint->get_x(&pt[0]);
		ppSnapBasePoint->get_y(&pt[1]);
		ppSnapBasePoint->get_z(&pt[2]);
		}
	m_pDbHandItem->set_13(pt);
	
	return NOERROR;
	}

STDMETHODIMP CAutoPViewport::get_SnapOn (VARIANT_BOOL * pSnapOn)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pSnapOn == NULL)
		return E_POINTER;
	
	int bFlag;
	if (!m_pDbHandItem->get_70(&bFlag)) 
		return E_FAIL;

	if (bFlag & IC_VPENT_STATUS_SNAPON)
		*pSnapOn = VARIANT_TRUE;
	else
		*pSnapOn = VARIANT_FALSE;

	return NOERROR;
	}

STDMETHODIMP CAutoPViewport::put_SnapOn (VARIANT_BOOL pSnapOn)
	{
	RETURN_IF_INVALID (m_pDbHandItem);

	int bFlag = 0;
	if (!m_pDbHandItem->get_70(&bFlag)) 
		return E_FAIL;

	if (pSnapOn == VARIANT_TRUE)
		bFlag |= IC_VPENT_STATUS_SNAPON;
	else
		bFlag &= ~IC_VPENT_STATUS_SNAPON;

	if (!m_pDbHandItem->set_70(bFlag)) 
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoPViewport::get_SnapRotationAngle (double * pAngle)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pAngle == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_50(pAngle)) 
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoPViewport::put_SnapRotationAngle (double pAngle)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (!m_pDbHandItem->set_50(pAngle)) 
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoPViewport::get_Target (IIcadPoint * * ppTarget)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppTarget == NULL)
		return E_POINTER;
	
	sds_point pt;
	m_pDbHandItem->get_17(pt);
	
	CComObject<CAutoPoint> *pPoint;
	CComObject<CAutoPoint>::CreateInstance(&pPoint);
	if (!pPoint)
		return E_FAIL;
	
	pPoint->Init(NULL, NULL, pt[0], pt[1], pt[2]);
	
	return pPoint->QueryInterface (ppTarget);
	}


STDMETHODIMP CAutoPViewport::put_Target (IIcadPoint * ppTarget)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	sds_point pt;
	pt[0] = pt[1] = pt[2] = 0.0;
	if (ppTarget)
		{
		ppTarget->get_x(&pt[0]);
		ppTarget->get_y(&pt[1]);
		ppTarget->get_z(&pt[2]);
		}
	m_pDbHandItem->set_17(pt);
	
	return NOERROR;
	}

STDMETHODIMP CAutoPViewport::get_TwistAngle (double * pAngle)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pAngle == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_51(pAngle)) 
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoPViewport::put_TwistAngle (double pAngle)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (!m_pDbHandItem->set_51(pAngle)) 
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoPViewport::get_UCSIconAtOrigin (VARIANT_BOOL * pIcon)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pIcon == NULL)
		return E_POINTER;
	
	int bFlag;
	if (!m_pDbHandItem->get_70(&bFlag)) 
		return E_FAIL;

	if (bFlag & IC_VPENT_STATUS_UCSICONATORIGIN)
		*pIcon = VARIANT_TRUE;
	else
		*pIcon = VARIANT_FALSE;

	return NOERROR;
	}

STDMETHODIMP CAutoPViewport::put_UCSIconAtOrigin (VARIANT_BOOL pIcon)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	int bFlag = 0;
	if (!m_pDbHandItem->get_70(&bFlag)) 
		return E_FAIL;

	if (pIcon == VARIANT_TRUE)
		bFlag |= IC_VPENT_STATUS_UCSICONATORIGIN;
	else
		bFlag &= IC_VPENT_STATUS_UCSICONATORIGIN;

	if (!m_pDbHandItem->set_70(bFlag)) 
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoPViewport::get_UCSIconOn (VARIANT_BOOL * pIcon)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pIcon == NULL)
		return E_POINTER;
	
	int bFlag;
	if (!m_pDbHandItem->get_70(&bFlag)) 
		return E_FAIL;

	if (bFlag & IC_VPENT_STATUS_UCSICON)
		*pIcon = VARIANT_TRUE;
	else
		*pIcon = VARIANT_FALSE;

	return NOERROR;
	}

STDMETHODIMP CAutoPViewport::put_UCSIconOn (VARIANT_BOOL pIcon)
	{
	RETURN_IF_INVALID (m_pDbHandItem);

	int bFlag = 0;
	if (!m_pDbHandItem->get_70(&bFlag)) 
		return E_FAIL;

	if (pIcon == VARIANT_TRUE)
		bFlag |= IC_VPENT_STATUS_UCSICON;
	else
		bFlag &= ~IC_VPENT_STATUS_UCSICON;

	if (!m_pDbHandItem->set_70(bFlag)) 
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoPViewport::get__ViewportID (int * pID)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pID == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_69 (pID)) 
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoPViewport::get_Width (double * pWidth)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pWidth == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_40(pWidth)) 
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoPViewport::put_Width (double pWidth)
	{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (pWidth < 0.0)
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_40(pWidth)) 
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoPViewport::Display (VARIANT_BOOL Status)
	{
	RETURN_IF_INVALID (m_pDbHandItem);

	int bFlag = 0;

	if(!m_pDbHandItem->get_90(&bFlag)) 
		return E_FAIL;

	if(Status == VARIANT_TRUE)
		bFlag &= ~0x20000;
	else
		bFlag |= 0x20000;

	if(!m_pDbHandItem->set_90(bFlag)) 
		return E_FAIL;

	if(m_pDoc)
		m_pDoc->UpdateEntity(m_pDbHandItem);

	return NOERROR;
	}

STDMETHODIMP CAutoPViewport::GetGridSpacing (double * Xspacing, double * Yspacing)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (Xspacing == NULL)
		return E_POINTER;
	
	if (Yspacing == NULL)
		return E_POINTER;
	
	sds_point pt;
	m_pDbHandItem->get_15(pt);

	*Xspacing = pt[0];
	*Yspacing = pt[1];

	return NOERROR;
	}

STDMETHODIMP CAutoPViewport::GetSnapSpacing (double * Xspacing, double * Yspacing)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (Xspacing == NULL)
		return E_POINTER;
	
	if (Yspacing == NULL)
		return E_POINTER;
	
	sds_point pt;
	m_pDbHandItem->get_14(pt);

	*Xspacing = pt[0];
	*Yspacing = pt[1];

	return NOERROR;
	}

STDMETHODIMP CAutoPViewport::SetGridSpacing (double Xspacing, double Yspacing)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	sds_point pt;
	pt[0] = Xspacing;
	pt[1] = Yspacing;
	pt[2] = 0.0;
	
	m_pDbHandItem->set_15(pt);
	
	return NOERROR;
	}

STDMETHODIMP CAutoPViewport::SetSnapSpacing (double Xspacing, double Yspacing)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	sds_point pt;
	pt[0] = Xspacing;
	pt[1] = Yspacing;
	pt[2] = 0.0;
	
	m_pDbHandItem->set_14(pt);
	
	return NOERROR;
	}

STDMETHODIMP CAutoPViewport::ZoomAll ()
	{
	return E_NOTIMPL;
	}

STDMETHODIMP CAutoPViewport::ZoomCenter (IIcadPoint * Center, double ZoomFactor)
	{
	return E_NOTIMPL;
	}

STDMETHODIMP CAutoPViewport::ZoomExtents ()
	{
	return E_NOTIMPL;
	}

STDMETHODIMP CAutoPViewport::ZoomPickWindow ()
	{
	return E_NOTIMPL;
	}

STDMETHODIMP CAutoPViewport::ZoomScaled (double Scale, ZoomScale ScaleType)
	{
	return E_NOTIMPL;
	}

STDMETHODIMP CAutoPViewport::ZoomWindow (IIcadPoint * Lowerleft, IIcadPoint * UpperRight)
	{
	return E_NOTIMPL;
	}


