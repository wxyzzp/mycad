/* INTELLICAD\PRJ\LIB\AUTO\AUTOMATRIX.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Implementation of the CAutoMatrix- the Automation Matrix object
 * 
 */ 

#include "StdAfx.h"
#include "AutoMatrix.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoMatrix

HRESULT CAutoMatrix::Init(IIcadLibrary *pLib)
	{
	m_pLib = pLib;
	return NOERROR;
	}

STDMETHODIMP CAutoMatrix::get_Application (IIcadApplication * * ppApp)
	{
	if (ppApp == NULL)
		return E_POINTER;
	
	return GetApp()->m_pAutoApp->QueryInterface (ppApp);
	}

STDMETHODIMP CAutoMatrix::get_Parent (IIcadLibrary * * ppParent)
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

STDMETHODIMP CAutoMatrix::GetValue (int Row, int Column, double * pValue)
	{
	if (pValue == NULL)
		return E_POINTER;
	
	if ((Row < 0) || (Row > 3))
		return E_INVALIDARG;

	if ((Column < 0) || (Column > 3))
		return E_INVALIDARG;

	*pValue = m_value[Row][Column];
	return NOERROR;
	}

STDMETHODIMP CAutoMatrix::SetValue (int Row, int Column, double Value)
	{
	if ((Row < 0) || (Row > 3))
		return E_INVALIDARG;

	if ((Column < 0) || (Column > 3))
		return E_INVALIDARG;

	m_value[Row][Column] = Value;
	return NOERROR;
	}



