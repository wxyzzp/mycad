/* INTELLICAD\PRJ\LIB\AUTO\AUTOPOINT.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Implementation of CAutoPoint - - the Automation Point object
 * 
 */ 

 
#include "StdAfx.h"
#include "AutoLibrary.h"
#include "AutoPoints.h"
#include "AutoPoint.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoPoint

HRESULT CAutoPoint::Init(IIcadLibrary *pLib, IIcadPoints *pPoints, double x, double y, double z)
	{
	m_pLib = pLib;
	m_pPoints = pPoints;
	//m_pPoints->AddRef();
	m_dX = x;
	m_dY = y;
	m_dZ = z;
	return NOERROR;
	}

STDMETHODIMP CAutoPoint::get_Application (IIcadApplication * * ppApp)
	{
	if (ppApp == NULL)
		return E_POINTER;
	
	return GetApp()->m_pAutoApp->QueryInterface (ppApp);
	}

STDMETHODIMP CAutoPoint::get_Parent (IIcadLibrary * * ppParent)
	{
	if (ppParent == NULL)
		return E_POINTER;
	
	if (m_pLib)
		return m_pLib->QueryInterface(IID_IIcadLibrary, (void **) ppParent);
	else
		{
		HRESULT hr;
		IIcadApplication *pApp = NULL;
		hr = this->get_Application(&pApp);
		if (pApp)
			hr = pApp->get_Library(ppParent);
		
		return hr;
		}
	}

STDMETHODIMP CAutoPoint::get_PointsCollection (IIcadPoints * * ppPoints)
	{
	if (ppPoints == NULL)
		return E_POINTER;
	
	if (m_pPoints)
		return m_pPoints->QueryInterface(IID_IIcadPoints, (void **) ppPoints);
	else
		return E_NOINTERFACE;
	}

STDMETHODIMP CAutoPoint::get_x (double * pX)
	{
	if (pX == NULL)
		return E_POINTER;
	
	*pX = m_dX;
	return NOERROR;
	}

STDMETHODIMP CAutoPoint::put_x (double pX)
	{
	m_dX = pX;
	return NOERROR;
	}

STDMETHODIMP CAutoPoint::get_y (double * pY)
	{
	if (pY == NULL)
		return E_POINTER;
	
	*pY = m_dY;
	return NOERROR;
	}

STDMETHODIMP CAutoPoint::put_y (double pY)
	{
	m_dY = pY;
	return NOERROR;
	}

STDMETHODIMP CAutoPoint::get_z (double * pZ)
	{
	if (pZ == NULL)
		return E_POINTER;
	
	*pZ = m_dZ;
	return NOERROR;
	}

STDMETHODIMP CAutoPoint::put_z (double pZ)
	{
	m_dZ = pZ;
	return NOERROR;
	}



