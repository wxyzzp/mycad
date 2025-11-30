/* INTELLICAD\PRJ\LIB\AUTO\AUTOVECTOR.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Implementation of the CAutoVector- the Automation Vector object
 * 
 */ 

#include "StdAfx.h"
#include "AutoLibrary.h"
#include "AutoVector.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoVector

HRESULT CAutoVector::Init(IIcadLibrary *pLib, double x, double y, double z)
	{
	m_pLib = pLib;
	m_dX = x;
	m_dY = y;
	m_dZ = z;
	return NOERROR;
	}

STDMETHODIMP CAutoVector::get_Application (IIcadApplication * * ppApp)
	{
	if (ppApp == NULL)
		return E_POINTER;
	
	return GetApp()->m_pAutoApp->QueryInterface (ppApp);
	}

STDMETHODIMP CAutoVector::get_Parent (IIcadLibrary * * ppParent)
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

STDMETHODIMP CAutoVector::get_x (double * pX)
	{
	if (pX == NULL)
		return E_POINTER;
	
	*pX = m_dX;
	return NOERROR;
	}

STDMETHODIMP CAutoVector::put_x (double pX)
	{
	m_dX = pX;
	return NOERROR;
	}

STDMETHODIMP CAutoVector::get_y (double * pY)
	{
	if (pY == NULL)
		return E_POINTER;
	
	*pY = m_dY;
	return NOERROR;
	}

STDMETHODIMP CAutoVector::put_y (double pY)
	{
	m_dY = pY;
	return NOERROR;
	}

STDMETHODIMP CAutoVector::get_z (double * pZ)
	{
	if (pZ == NULL)
		return E_POINTER;
	
	*pZ = m_dZ;
	return NOERROR;
	}

STDMETHODIMP CAutoVector::put_z (double pZ)
	{
	m_dZ = pZ;
	return NOERROR;
	}



