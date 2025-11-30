/* INTELLICAD\PRJ\LIB\AUTO\AUTO3DFACE.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Implementation of Auto3DFace - the Automation 3DFace object
 * 
 */ 

#include "StdAfx.h"
#include "Auto3DFace.h"
#include "AutoPoint.h"
#include "AutoVector.h"

/////////////////////////////////////////////////////////////////////////////
// CAuto3DFace

HRESULT CAuto3DFace::Init(CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem)
	{
	CAutoDbItem::Init(pDoc, pParent);
	
	if (pDbHandItem)
		m_pDbHandItem = pDbHandItem;
	else
		{
		if (pDoc)
			m_pDbHandItem = new db_3dface(pDoc->m_dbDrawing);
		else
			m_pDbHandItem = new db_3dface();
		}
	
	if (NULL == m_pDbHandItem) 
		return FALSE;
	
	return NOERROR;
	}

STDMETHODIMP CAuto3DFace::get_VisibilityEdge1 (VARIANT_BOOL * bVisible)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (bVisible == NULL)
		return E_POINTER;
	
	int flag = 0;
	if (!m_pDbHandItem->get_70(&flag)) 
		return E_FAIL;
	
	if (flag & IC_FACE3D_FACE1INVIS) 
		*bVisible = VARIANT_FALSE;
	else 
		*bVisible = VARIANT_TRUE;
	
	return NOERROR;
	}

STDMETHODIMP CAuto3DFace::put_VisibilityEdge1 (VARIANT_BOOL bVisible)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	int flags = 0;
	
	if (!m_pDbHandItem->get_70(&flags)) 
		return E_FAIL;

	if (bVisible == VARIANT_FALSE) 
		flags |= IC_FACE3D_FACE1INVIS;
	else
		flags &= ~IC_FACE3D_FACE1INVIS;
	
	if (!m_pDbHandItem->set_70(flags)) 
		return E_FAIL;
	
	return NOERROR;
	}

STDMETHODIMP CAuto3DFace::get_VisibilityEdge2 (VARIANT_BOOL * bVisible)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (bVisible == NULL)
		return E_POINTER;
	
	int flag = 0;
	if (!m_pDbHandItem->get_70(&flag)) 
		return E_FAIL;
	
	if (flag & IC_FACE3D_FACE2INVIS) 
		*bVisible = VARIANT_FALSE;
	else 
		*bVisible = VARIANT_TRUE;
	
	return NOERROR;
	}

STDMETHODIMP CAuto3DFace::put_VisibilityEdge2 (VARIANT_BOOL bVisible)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	int flags = 0;
	
	if (!m_pDbHandItem->get_70(&flags)) 
		return E_FAIL;

	if (bVisible == VARIANT_FALSE) 
		flags |= IC_FACE3D_FACE2INVIS;
	else
		flags &= ~IC_FACE3D_FACE2INVIS;
	
	if (!m_pDbHandItem->set_70(flags)) 
		return E_FAIL;
	
	return NOERROR;
	}

STDMETHODIMP CAuto3DFace::get_VisibilityEdge3 (VARIANT_BOOL * bVisible)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (bVisible == NULL)
		return E_POINTER;
	
	int flag = 0;
	if (!m_pDbHandItem->get_70(&flag)) 
		return E_FAIL;
	
	if (flag & IC_FACE3D_FACE3INVIS) 
		*bVisible = VARIANT_FALSE;
	else 
		*bVisible = VARIANT_TRUE;
	
	return NOERROR;
	}

STDMETHODIMP CAuto3DFace::put_VisibilityEdge3 (VARIANT_BOOL bVisible)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	int flags = 0;
	
	if (!m_pDbHandItem->get_70(&flags)) 
		return E_FAIL;

	if (bVisible == VARIANT_FALSE) 
		flags |= IC_FACE3D_FACE3INVIS;
	else
		flags &= ~IC_FACE3D_FACE3INVIS;
	
	if (!m_pDbHandItem->set_70(flags)) 
		return E_FAIL;
	
	return NOERROR;
	}

STDMETHODIMP CAuto3DFace::get_VisibilityEdge4 (VARIANT_BOOL * bVisible)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (bVisible == NULL)
		return E_POINTER;
	
	int flag = 0;
	if (!m_pDbHandItem->get_70(&flag)) 
		return E_FAIL;
	
	if (flag & IC_FACE3D_FACE4INVIS) 
		*bVisible = VARIANT_FALSE;
	else 
		*bVisible = VARIANT_TRUE;
	
	return NOERROR;
	}

STDMETHODIMP CAuto3DFace::put_VisibilityEdge4 (VARIANT_BOOL bVisible)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	int flags = 0;
	
	if (!m_pDbHandItem->get_70(&flags)) 
		return E_FAIL;

	if (bVisible == VARIANT_FALSE) 
		flags |= IC_FACE3D_FACE4INVIS;
	else
		flags &= ~IC_FACE3D_FACE4INVIS;
	
	if (!m_pDbHandItem->set_70(flags)) 
		return E_FAIL;
	
	return NOERROR;
	}

STDMETHODIMP CAuto3DFace::get_Coordinates (IIcadPoints * * ppCoords)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppCoords == NULL)
		return E_POINTER;
	
	CComObject<CAutoPoints> *pVertices;
	CComObject<CAutoPoints>::CreateInstance(&pVertices);
	if (!pVertices)
		return E_OUTOFMEMORY;

	CComPtr<IIcadPoint> pIIcadPoint = NULL;

	sds_point pt;
	if (!m_pDbHandItem->get_10(pt))
		return E_FAIL;
	pVertices->Add (pt[0], pt[1], pt[2], 0, &pIIcadPoint);
	if (pIIcadPoint == NULL)
		return E_OUTOFMEMORY;
	pIIcadPoint = NULL;

	if (!m_pDbHandItem->get_11(pt))
		return E_FAIL;
	pVertices->Add (pt[0], pt[1], pt[2], 0, &pIIcadPoint);
	if (pIIcadPoint == NULL)
		return E_OUTOFMEMORY;
	pIIcadPoint = NULL;

	if (!m_pDbHandItem->get_12(pt))
		return E_FAIL;
	pVertices->Add (pt[0], pt[1], pt[2], 0, &pIIcadPoint);
	if (pIIcadPoint == NULL)
		return E_OUTOFMEMORY;
	pIIcadPoint = NULL;

	if (!m_pDbHandItem->get_13(pt))
		return E_FAIL;
	pVertices->Add (pt[0], pt[1], pt[2], 0, &pIIcadPoint);
	if (pIIcadPoint == NULL)
		return E_OUTOFMEMORY;
	pIIcadPoint = NULL;

	return pVertices->QueryInterface (IID_IIcadPoints, (void **)ppCoords);
	}

STDMETHODIMP CAuto3DFace::put_Coordinates (IIcadPoints * ppCoords)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	CComPtr<IIcadPoint> pIIcadPoint = NULL;
	HRESULT hr;

	sds_point pt;
	if (ppCoords)
		{
		pt[0] = pt[1] = pt[2] = 0.0;
		hr = ppCoords->Item(1, &pIIcadPoint);
		if (SUCCEEDED(hr) && pIIcadPoint)
			{
			pIIcadPoint->get_x (&pt[0]);
			pIIcadPoint->get_y (&pt[1]);
			pIIcadPoint->get_z (&pt[2]);
			pIIcadPoint = NULL;
			}
		if (!m_pDbHandItem->set_10(pt))
			return E_FAIL;

		pt[0] = pt[1] = pt[2] = 0.0;
		hr = ppCoords->Item(2, &pIIcadPoint);
		if (SUCCEEDED(hr) && pIIcadPoint)
			{
			pIIcadPoint->get_x (&pt[0]);
			pIIcadPoint->get_y (&pt[1]);
			pIIcadPoint->get_z (&pt[2]);
			pIIcadPoint = NULL;
			}
		if (!m_pDbHandItem->set_11(pt))
			return E_FAIL;

		pt[0] = pt[1] = pt[2] = 0.0;
		hr = ppCoords->Item(3, &pIIcadPoint);
		if (SUCCEEDED(hr) && pIIcadPoint)
			{
			pIIcadPoint->get_x (&pt[0]);
			pIIcadPoint->get_y (&pt[1]);
			pIIcadPoint->get_z (&pt[2]);
			pIIcadPoint = NULL;
			}
		if (!m_pDbHandItem->set_12(pt))
			return E_FAIL;

		pt[0] = pt[1] = pt[2] = 0.0;
		hr = ppCoords->Item(4, &pIIcadPoint);
		if (SUCCEEDED(hr) && pIIcadPoint)
			{
			pIIcadPoint->get_x (&pt[0]);
			pIIcadPoint->get_y (&pt[1]);
			pIIcadPoint->get_z (&pt[2]);
			pIIcadPoint = NULL;
			}
		if (!m_pDbHandItem->set_13(pt))
			return E_FAIL;
		}
	else
		{
		pt[0] = pt[1] = pt[2] = 0.0;
		if (!m_pDbHandItem->set_10(pt))
			return E_FAIL;
		if (!m_pDbHandItem->set_11(pt))
			return E_FAIL;
		if (!m_pDbHandItem->set_12(pt))
			return E_FAIL;
		if (!m_pDbHandItem->set_13(pt))
			return E_FAIL;
		}

	return NOERROR;
	}

STDMETHODIMP CAuto3DFace::GetInvisibleEdge (int Index, VARIANT_BOOL * pState)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pState == NULL)
		return E_POINTER;

	int flag = 0;
	if (!m_pDbHandItem->get_70(&flag)) 
		return E_FAIL;
	
	int val;
	if (Index == 1)
		val = IC_FACE3D_FACE1INVIS;
	else if (Index == 2)
		val = IC_FACE3D_FACE2INVIS;
	else if (Index == 3)
		val = IC_FACE3D_FACE3INVIS;
	else if (Index == 4)
		val = IC_FACE3D_FACE4INVIS;
	else
		return E_INVALIDARG;

	if (flag & val)
		*pState = VARIANT_TRUE;
	else 
		*pState = VARIANT_FALSE;
	
	return NOERROR;
	}

STDMETHODIMP CAuto3DFace::SetInvisibleEdge (int Index, VARIANT_BOOL State)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	int flags = 0;
	
	if (!m_pDbHandItem->get_70(&flags)) 
		return E_FAIL;

	int val;
	if (Index == 1)
		val = IC_FACE3D_FACE1INVIS;
	else if (Index == 2)
		val = IC_FACE3D_FACE2INVIS;
	else if (Index == 3)
		val = IC_FACE3D_FACE3INVIS;
	else if (Index == 4)
		val = IC_FACE3D_FACE4INVIS;
	else
		return E_INVALIDARG;

	if (State == VARIANT_TRUE) 
		flags |= val;
	else
		flags &= ~val;

	
	if (!m_pDbHandItem->set_70(flags)) 
		return E_FAIL;
	
	return NOERROR;
	}



