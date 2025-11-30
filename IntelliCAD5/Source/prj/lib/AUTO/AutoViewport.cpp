/* INTELLICAD\PRJ\LIB\AUTO\AUTOVIEWPORT.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Implementation of CAutoViewport - the Automation Viewport object
 * 
 */ 

#include "StdAfx.h"
#include "AutoViewport.h"
#include "AutoPoint.h"
#include "AutoVector.h"
#include "IcadView.h"
#include "vporttabrec.h"
#include "viewport.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoViewport

HRESULT CAutoViewport::Init(CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem)
	{
	CAutoDbItem::Init(pDoc, pParent);
	
	if (pDbHandItem)
		m_pDbHandItem = pDbHandItem;
	else
		m_pDbHandItem = new db_vporttabrec();
	
	if (NULL == m_pDbHandItem) 
		return FALSE;
	
	return NOERROR;
	}

HRESULT CAutoViewport::FindAssociatedView (CIcadView **ppFoundView)
	{
	//Find the CIcadView that corresponds to the Viewport
	CIcadView* pView = NULL;
	BSTR VportHandle;
	HRESULT hr = get_Handle (&VportHandle);
	if (FAILED(hr))
		return E_FAIL;

	CString str(VportHandle);
	db_objhandle hand1(str);
	db_objhandle hand2;

	POSITION pos = m_pDoc->GetFirstViewPosition();
	while (pos != NULL) 
		{
		pView = (CIcadView*) m_pDoc->GetNextView(pos);

		if (!pView->m_pVportTabHip)
			{
			CFrameWnd* pFrame;
			pFrame = pView->GetParentFrame();
			if (pFrame)
				pFrame->SetFocus();
			}

		if (!pView->m_pVportTabHip)
			continue;

		hand2 = pView->m_pVportTabHip->RetHandle();
		if (hand1 == hand2)
			break;

		pView = NULL;
		}

	if (pView == NULL)
		return E_FAIL;

	*ppFoundView = pView;
	return NOERROR;
	}

void GetActiveViewportName (CIcadDoc *pDoc, CString *ActiveVpName)
	{
	CIcadView* pView = NULL;
	POSITION pos = pDoc->GetFirstViewPosition();
	pView = (CIcadView*) pDoc->GetNextView(pos);

	db_handitem *orighip = NULL;
	if (pView && pView->m_pVportTabHip)
		((db_vporttabrec *)(pView->m_pVportTabHip))->get_original_viewport (&orighip);

	char* pStr;
	if (orighip)
		orighip->get_2 (&pStr);
	else
		pView->m_pVportTabHip->get_2 (&pStr);

	ActiveVpName->Format(pStr);
	return;
	}


// IIcadTableItem properties are overridden here because the Viewport entity needs to be treated differently

STDMETHODIMP CAutoViewport::get_Name(BSTR * pstrName)
	{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (pstrName == NULL)
		return E_POINTER;
	
	char* pStr;
	if ( !m_pDbHandItem->get_2 (&pStr)) 
		return E_FAIL;
/*	
	if (strcmp (pStr,"*ACTIVE") == 0)
		{
		db_handitem *vp = NULL;
		((db_vporttabrec *)m_pDbHandItem)->get_original_viewport (&vp);
		if (vp)
			vp->get_2 (&pStr);
		}
*/
	CString str(pStr);

	*pstrName = str.AllocSysString();
	return NOERROR;
	}

STDMETHODIMP CAutoViewport::put_Name(BSTR Name)
	{
	RETURN_IF_INVALID (m_pDbHandItem);

	CString str(Name);

	if (!ic_isvalidname(str.GetBuffer(0)))
		return E_INVALIDARG;

	//do not allow changing a name to *ACTIVE, since this is reserved
	if (str == "*ACTIVE")
		return E_INVALIDARG;

	//find the current name of this viewport
	BSTR CurrentName;
	this->get_Name (&CurrentName);
	CString strCurName(CurrentName);
	if (strCurName == "*ACTIVE")
		{
		char* pStr;
		db_handitem *vp = NULL;
		((db_vporttabrec *)m_pDbHandItem)->get_original_viewport (&vp);
		if (vp)
			vp->get_2 (&pStr);
		else
			return E_FAIL; //cannot set the name of a *ACTIVE viewport that has no corresponding original viewport
		strCurName = pStr;
		}

	//search through the table for all viewports with this name
	db_handitem *hip = NULL;
	for (hip=m_pDoc->m_dbDrawing->tblnext(DB_VPORTTABREC,1); hip!=NULL; hip=hip->next) 
		{
		if(hip->ret_deleted()) 
			continue;

		char *fcp1;
		db_handitem *orighip = NULL;

		hip->get_2(&fcp1);
		if(strCurName.Compare(fcp1) == 0) 
			{
			//change the name 
			if (!hip->set_2 (str.GetBuffer(0)))
				return E_FAIL;
			}
		}
	
	return NOERROR;
	}


// IIcadViewport properties and methods

STDMETHODIMP CAutoViewport::get_Center (IIcadPoint * * ppCenter)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppCenter == NULL)
		return E_POINTER;
	
	sds_point pt;
	m_pDbHandItem->get_12(pt);
	
	CComObject<CAutoPoint> *pPoint;
	CComObject<CAutoPoint>::CreateInstance(&pPoint);
	if (!pPoint)
		return E_FAIL;
	
	pPoint->Init(NULL, NULL, pt[0], pt[1], pt[2]);
	
	return pPoint->QueryInterface (ppCenter);
	}

STDMETHODIMP CAutoViewport::put_Center (IIcadPoint * ppCenter)
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
	m_pDbHandItem->set_12(pt);

	m_pDoc->RedrawCurrentView();

	return NOERROR;
	}

STDMETHODIMP CAutoViewport::get_Direction (IIcadVector * * ppNormal)
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

STDMETHODIMP CAutoViewport::put_Direction (IIcadVector * ppNormal)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	sds_point pt;
	pt[0] = pt[1] = 0.0; pt[2] = 1.0;

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
	
	m_pDoc->RedrawCurrentView();
	
	return NOERROR;
	}

STDMETHODIMP CAutoViewport::get_GridOn (VARIANT_BOOL * pGridOn)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pGridOn == NULL)
		return E_POINTER;
	
	int bFlag;
	if (!m_pDbHandItem->get_76(&bFlag)) 
		return E_FAIL;

	if (bFlag)
		*pGridOn = VARIANT_TRUE;
	else
		*pGridOn = VARIANT_FALSE;

	return NOERROR;
	}

STDMETHODIMP CAutoViewport::put_GridOn (VARIANT_BOOL pGridOn)
	{
	RETURN_IF_INVALID (m_pDbHandItem);

	int bFlag = 0;
	if (pGridOn == VARIANT_TRUE)
		bFlag = 1;

	if (!m_pDbHandItem->set_76(bFlag)) 
		return E_FAIL;

	m_pDoc->RedrawCurrentView();
	
	return NOERROR;
	}

STDMETHODIMP CAutoViewport::get_Height (double * pHeight)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pHeight == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_40(pHeight)) 
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoViewport::put_Height (double pHeight)
	{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (pHeight <= 0.0)
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_40(pHeight)) 
		return E_FAIL;

	m_pDoc->RedrawCurrentView();
	
	return NOERROR;
	}

STDMETHODIMP CAutoViewport::get_LowerLeftCorner (IIcadPoint * * ppLowerLeftCorner)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppLowerLeftCorner == NULL)
		return E_POINTER;

	sds_point pt;
	m_pDbHandItem->get_10(pt);
	
	CComObject<CAutoPoint> *pPoint;
	CComObject<CAutoPoint>::CreateInstance(&pPoint);
	if (!pPoint)
		return E_FAIL;
	
	pPoint->Init(NULL, NULL, pt[0], pt[1], pt[2]);
	
	return pPoint->QueryInterface (ppLowerLeftCorner);
	}

STDMETHODIMP CAutoViewport::put_LowerLeftCorner (IIcadPoint * ppLowerLeftCorner)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	sds_point pt;
	pt[0] = pt[1] = pt[2] = 0.0;
	if (ppLowerLeftCorner)	//this is an optional arg
		{
		ppLowerLeftCorner->get_x(&pt[0]);
		ppLowerLeftCorner->get_y(&pt[1]);
		//ppLowerLeftCorner->get_z(&pt[2]); z value does not matter
		}

	//the values should be between 0 and 1 since this is a parametrized value
	if (pt[0]<0.0 || pt[0]>1.0)
		return E_INVALIDARG;
	if (pt[1]<0.0 || pt[1]>1.0)
		return E_INVALIDARG;
	
	m_pDbHandItem->set_10(pt);
	
	m_pDoc->RedrawCurrentView();
	
	return NOERROR;
	}

STDMETHODIMP CAutoViewport::get_OrthoOn (VARIANT_BOOL * pOrthoOn)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pOrthoOn == NULL)
		return E_POINTER;
	
	*pOrthoOn = m_bOrthoOn;

	return NOERROR;
	}

STDMETHODIMP CAutoViewport::put_OrthoOn (VARIANT_BOOL pOrthoOn)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	m_bOrthoOn = pOrthoOn;

	m_pDoc->RedrawCurrentView();
	
	return NOERROR;
	}

STDMETHODIMP CAutoViewport::get_SnapBasePoint (IIcadPoint * * ppSnapBasePoint)
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

STDMETHODIMP CAutoViewport::put_SnapBasePoint (IIcadPoint * ppSnapBasePoint)
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
	
	m_pDoc->RedrawCurrentView();
	
	return NOERROR;
	}

STDMETHODIMP CAutoViewport::get_SnapOn (VARIANT_BOOL * pSnapOn)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pSnapOn == NULL)
		return E_POINTER;
	
	int bFlag;
	if (!m_pDbHandItem->get_75(&bFlag)) 
		return E_FAIL;

	if (bFlag)
		*pSnapOn = VARIANT_TRUE;
	else
		*pSnapOn = VARIANT_FALSE;

	return NOERROR;
	}

STDMETHODIMP CAutoViewport::put_SnapOn (VARIANT_BOOL pSnapOn)
	{
	RETURN_IF_INVALID (m_pDbHandItem);

	int bFlag = 0;
	if (pSnapOn == VARIANT_TRUE)
		bFlag = 1;

	if (!m_pDbHandItem->set_75(bFlag)) 
		return E_FAIL;

	m_pDoc->RedrawCurrentView();
	
	return NOERROR;
	}

STDMETHODIMP CAutoViewport::get_SnapRotationAngle (double * pAngle)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pAngle == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_50(pAngle)) 
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoViewport::put_SnapRotationAngle (double pAngle)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (!m_pDbHandItem->set_50(pAngle)) 
		return E_FAIL;

	m_pDoc->RedrawCurrentView();
	
	return NOERROR;
	}

STDMETHODIMP CAutoViewport::get_Target (IIcadPoint * * ppTarget)
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

STDMETHODIMP CAutoViewport::put_Target (IIcadPoint * ppTarget)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	sds_point pt;
	pt[0] = pt[1] = 0.0; pt[2] = 1.0;
	if (ppTarget)
		{
		ppTarget->get_x(&pt[0]);
		ppTarget->get_y(&pt[1]);
		ppTarget->get_z(&pt[2]);
		}
	m_pDbHandItem->set_17(pt);
	
	m_pDoc->RedrawCurrentView();
	
	return NOERROR;
	}

STDMETHODIMP CAutoViewport::get_UCSIconAtOrigin (VARIANT_BOOL * pIcon)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pIcon == NULL)
		return E_POINTER;
	
	int bFlag;
	if (!m_pDbHandItem->get_74(&bFlag)) 
		return E_FAIL;

	if (bFlag & 3)
		*pIcon = VARIANT_TRUE;
	else
		*pIcon = VARIANT_FALSE;

	return NOERROR;
	}

STDMETHODIMP CAutoViewport::put_UCSIconAtOrigin (VARIANT_BOOL pIcon)
	{
	RETURN_IF_INVALID (m_pDbHandItem);

	int bFlag = 0;
	if (!m_pDbHandItem->get_74(&bFlag)) 
		return E_FAIL;

	if (pIcon == VARIANT_TRUE)
		bFlag |= 3;
	else 
		bFlag &= ~3;

	if (!m_pDbHandItem->set_74(bFlag)) 
		return E_FAIL;

	m_pDoc->RedrawCurrentView();
	
	return NOERROR;
	}

STDMETHODIMP CAutoViewport::get_UCSIconOn (VARIANT_BOOL * pIcon)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pIcon == NULL)
		return E_POINTER;
	
	int bFlag;
	if (!m_pDbHandItem->get_74(&bFlag)) 
		return E_FAIL;

	if (bFlag & 1)
		*pIcon = VARIANT_TRUE;
	else
		*pIcon = VARIANT_FALSE;

	return NOERROR;
	}

STDMETHODIMP CAutoViewport::put_UCSIconOn (VARIANT_BOOL pIcon)
	{
	RETURN_IF_INVALID (m_pDbHandItem);

	int bFlag = 0;
	if (!m_pDbHandItem->get_74(&bFlag)) 
		return E_FAIL;

	if (pIcon == VARIANT_TRUE)
		bFlag = bFlag | 1;
	else 
		bFlag = bFlag & (~1);

	if (!m_pDbHandItem->set_74(bFlag)) 
		return E_FAIL;

	m_pDoc->RedrawCurrentView();
	
	return NOERROR;
	}

STDMETHODIMP CAutoViewport::get_UpperRightCorner (IIcadPoint * * ppUpperRightCorner)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppUpperRightCorner == NULL)
		return E_POINTER;
	
	sds_point pt;
	m_pDbHandItem->get_11(pt);
	
	CComObject<CAutoPoint> *pPoint;
	CComObject<CAutoPoint>::CreateInstance(&pPoint);
	if (!pPoint)
		return E_FAIL;
	
	pPoint->Init(NULL, NULL, pt[0], pt[1], pt[2]);
	
	return pPoint->QueryInterface (ppUpperRightCorner);
	}

STDMETHODIMP CAutoViewport::put_UpperRightCorner (IIcadPoint * ppUpperRightCorner)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	sds_point pt;
	pt[0] = pt[1] = pt[2] = 1.0;
	if (ppUpperRightCorner)
		{
		ppUpperRightCorner->get_x(&pt[0]);
		ppUpperRightCorner->get_y(&pt[1]);
		//ppUpperRightCorner->get_z(&pt[2]);
		}
	
	//the values should be between 0 and 1 since this is a parametrized value
	if (pt[0]<0.0 || pt[0]>1.0)
		return E_INVALIDARG;
	if (pt[1]<0.0 || pt[1]>1.0)
		return E_INVALIDARG;

	m_pDbHandItem->set_11(pt);
	
	m_pDoc->RedrawCurrentView();
	
	return NOERROR;
	}

STDMETHODIMP CAutoViewport::get_Width (double * pWidth)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pWidth == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_41(pWidth)) 
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoViewport::put_Width (double pWidth)
	{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (pWidth <= 0.0)
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_41(pWidth)) 
		return E_FAIL;

	m_pDoc->RedrawCurrentView();
	
	return NOERROR;
	}

STDMETHODIMP CAutoViewport::GetGridSpacing (double * Xspacing, double * Yspacing)
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

STDMETHODIMP CAutoViewport::GetSnapSpacing (double * Xspacing, double * Yspacing)
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

STDMETHODIMP CAutoViewport::SetGridSpacing (double Xspacing, double Yspacing)
	{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (Xspacing <= 0.0)
		return E_INVALIDARG;
	if (Yspacing <= 0.0)
		return E_INVALIDARG;

	sds_point pt;
	pt[0] = Xspacing;
	pt[1] = Yspacing;
	pt[2] = 0.0;
	
	m_pDbHandItem->set_15(pt);
	
	m_pDoc->RedrawCurrentView();
	
	return NOERROR;
	}

STDMETHODIMP CAutoViewport::SetSnapSpacing (double Xspacing, double Yspacing)
	{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (Xspacing <= 0.0)
		return E_INVALIDARG;
	if (Yspacing <= 0.0)
		return E_INVALIDARG;

	sds_point pt;
	pt[0] = Xspacing;
	pt[1] = Yspacing;
	pt[2] = 0.0;
	
	m_pDbHandItem->set_14(pt);
	
	m_pDoc->RedrawCurrentView();
	
	return NOERROR;
	}

STDMETHODIMP CAutoViewport::SetView (IIcadView * View)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (View == NULL)
		return E_INVALIDARG;

	HRESULT hr;
	long nhip;

	hr = View->_handitem (&nhip);
	if (FAILED(hr) || (nhip == 0))
		return E_FAIL;

	db_handitem *pViewHip;
	pViewHip = (db_handitem *)nhip;

	return m_pDoc->SetView((db_vporttabrec *)m_pDbHandItem, (db_viewtabrec *)pViewHip);
	}

STDMETHODIMP CAutoViewport::Split (ViewportSplitType NumberOfWindows)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	HRESULT hr;
	BSTR VportName;
	hr = get_Name (&VportName);
	if (FAILED(hr))
		return E_FAIL;
	CString VpName(VportName);

	//Find the CIcadView that corresponds to the Viewport
	CIcadView* pView = NULL;
	FindAssociatedView (&pView);
	
	//if we didnt find the view, restore the appropriate configuration and find the correct view
	if (pView == NULL)
		{
		m_pDoc->RestoreViewportConfiguration (VpName.GetBuffer(0));
		FindAssociatedView (&pView);
		if (pView == NULL)
			return E_FAIL;
		}

	//determine the number of windows and orientation.  The values are arbitrarily defined in cmd_viewports.cpp
	int nHowMany, nOrientation;

	switch (NumberOfWindows)
		{
		case vicViewport2Horizontal:
			nHowMany = 2;
			nOrientation = 1;
			break;
		case vicViewport2Vertical:
			nHowMany = 2;
			nOrientation = 2;
			break;
		case vicViewport3Left:
			nHowMany = 3;
			nOrientation = 5;
			break;
		case vicViewport3Right:
			nHowMany = 3;
			nOrientation = 6;
			break;
		case vicViewport3Horizontal:
			nHowMany = 3;
			nOrientation = 1;
			break;
		case vicViewport3Vertical:
			nHowMany = 3;
			nOrientation = 2;
			break;
		case vicViewport3Above:
			nHowMany = 3;
			nOrientation = 3;
			break;
		case vicViewport3Below:
			nHowMany = 3;
			nOrientation = 4;
			break;
		case vicViewport4:
			nHowMany = 4;
			nOrientation = 0;
			break;
		default:
			return E_INVALIDARG;
		}
	
	hr = m_pDoc->SplitView (pView, nHowMany, nOrientation);
	if (FAILED (hr))
		return E_FAIL;

	if (pView)
		{
		//Now cycle through all the views and activate them so that the m_pVportTabHip gets intialized correctly
		POSITION pos = m_pDoc->GetFirstViewPosition();
		while (pos != NULL) 
			{
			pView = (CIcadView*) m_pDoc->GetNextView(pos);

			if (!pView->m_pVportTabHip)
				{
				CFrameWnd* pFrame;
				pFrame = pView->GetParentFrame();
				if (pFrame)
					pFrame->SetFocus();
				}

			if (!pView->m_pVportTabHip)
				continue;
			}
		}


	if (VpName != "*ACTIVE")	//then we need to save the viewport configuration
		{
		//CString ActiveVpName;
		//GetActiveViewportName (m_pDoc , &ActiveVpName);
		//if (VpName != ActiveVpName)
			m_pDoc->SaveViewportConfiguration (VpName.GetBuffer(0));
		}

	return NOERROR;
	}

int ic_pt_equal(sds_point p1, sds_point p2) ;

STDMETHODIMP CAutoViewport::ZoomAll ()
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	CIcadView* pView = NULL;
	if (FAILED(FindAssociatedView (&pView)))
		return E_FAIL;

	if (pView == NULL)
		return E_FAIL;

	struct resbuf rb;
	db_getvar (NULL, DB_QVIEWDIR, &rb, m_pDoc->m_dbDrawing, NULL, NULL);

	sds_point pt1;
	pt1[0] = pt1[1] = 0.0;
	pt1[2] = 1.0;

	if (!ic_pt_equal (rb.resval.rpoint, pt1))
		return m_pDoc->AUTO_cmd_zoom_extents(pView, 0);
	else 
		return m_pDoc->AUTO_cmd_zoom_extents(pView, 1);
	}

STDMETHODIMP CAutoViewport::ZoomCenter (IIcadPoint * Center, double ZoomFactor)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (Center == NULL)
		return E_INVALIDARG;

	if (icadRealEqual(ZoomFactor,0.0))
		return E_INVALIDARG;

	CIcadView* pView = NULL;
	if (FAILED(FindAssociatedView (&pView)))
		return E_FAIL;

	if (pView == NULL)
		return E_FAIL;

	sds_point zoomcenter;		
	Center->get_x (&zoomcenter[0]);	
	Center->get_y (&zoomcenter[1]);	
	Center->get_z (&zoomcenter[2]);

	return m_pDoc->AUTO_cmd_zoom(pView, zoomcenter, ZoomFactor);
	}

STDMETHODIMP CAutoViewport::ZoomExtents ()
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	CIcadView* pView = NULL;
	if (FAILED(FindAssociatedView (&pView)))
		return E_FAIL;

	if (pView == NULL)
		return E_FAIL;

	return m_pDoc->AUTO_cmd_zoom_extents(pView, 0);
	}

STDMETHODIMP CAutoViewport::ZoomPickWindow ()
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	return m_pDoc->AUTO_cmd_zoom(NULL, NULL, 1.0);
	}

STDMETHODIMP CAutoViewport::ZoomScaled (double Scale, ZoomScale ScaleType)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (Scale <= 0.0)
		return E_INVALIDARG;

	CIcadView* pView = NULL;
	if (FAILED(FindAssociatedView (&pView)))
		return E_FAIL;

	if (pView == NULL)
		return E_FAIL;

	return m_pDoc->AUTO_cmd_zoom(pView, NULL, Scale, ScaleType);
	}

STDMETHODIMP CAutoViewport::ZoomWindow (IIcadPoint * Lowerleft, IIcadPoint * UpperRight)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (Lowerleft == NULL)
		return E_INVALIDARG;

	if (UpperRight == NULL)
		return E_INVALIDARG;

	CIcadView* pView = NULL;
	if (FAILED(FindAssociatedView (&pView)))
		return E_FAIL;

	if (pView == NULL)
		return E_FAIL;

	sds_point pt1;		
	Lowerleft->get_x (&pt1[0]);	
	Lowerleft->get_y (&pt1[1]);	
	Lowerleft->get_z (&pt1[2]);

	sds_point pt2;		
	UpperRight->get_x (&pt2[0]);	
	UpperRight->get_y (&pt2[1]);	
	UpperRight->get_z (&pt2[2]);

	if (ic_pt_equal (pt1, pt2))
		return E_INVALIDARG;

	sds_point zoomcenter;
	double zoomfactor = 1.0;

	zoomcenter[0] = (pt1[0] + pt2[0])/2.0;
	zoomcenter[1] = (pt1[1] + pt2[1])/2.0;
	zoomcenter[2] = (pt1[2] + pt2[2])/2.0;

	struct resbuf rb;
	//Bugzilla No : 78242 ; 29-07-02
	//db_getvar (NULL, DB_QSCREENSIZE, &rb, m_pDoc->m_dbDrawing, NULL, NULL);
	GetApp()->SDSGetVar (m_pDoc, "SCREENSIZE", &rb);

	if((fabs(pt2[0]-pt1[0])/fabs(pt2[1]-pt1[1]))>(rb.resval.rpoint[0]/rb.resval.rpoint[1]))
		zoomfactor = fabs(pt2[0]-pt1[0])*rb.resval.rpoint[1]/rb.resval.rpoint[0];
	else
		zoomfactor = fabs(pt2[1]-pt1[1]);

	return m_pDoc->AUTO_cmd_zoom(pView, zoomcenter, zoomfactor);
	}


