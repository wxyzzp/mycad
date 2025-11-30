/* INTELLICAD\PRJ\LIB\AUTO\AUTODOCPREFS.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * $Revision: 1.3 $ $Date: 2001/03/16 21:48:11 $ 
 * 
 * Abstract
 * 
 * Implementation of the CAutoDocPreferences - The Document level preferences
 * 
 */ 

#include "StdAfx.h"
#include "AutoDocPrefs.h"

#define CURDBDWG (m_pDoc->m_dbDrawing)

/////////////////////////////////////////////////////////////////////////////
// CAutoDocPreferences

HRESULT CAutoDocPreferences::Init (CIcadDoc *pDoc)
	{
	m_pDoc = pDoc;
	return NOERROR;
	}

STDMETHODIMP CAutoDocPreferences::get_Application (IIcadApplication * * Application)
	{
	if (Application == NULL)
		return E_POINTER;
	
	return m_pApp->m_pAutoApp->QueryInterface (Application);
	}

STDMETHODIMP CAutoDocPreferences::get_Parent (IIcadDocument * * Document)
	{
	if (Document == NULL)
		return E_POINTER;
	
	return m_pDoc->m_pAutoDoc->QueryInterface (Document);
	}

STDMETHODIMP CAutoDocPreferences::get_ContourLinesPerSurface (int * NumberOfLines)
	{
	if (NumberOfLines == NULL)
		return E_POINTER;
	
	struct resbuf rb;
	db_getvar (NULL, DB_QISOLINES, &rb, CURDBDWG, NULL, NULL);
	
	*NumberOfLines = (int)rb.resval.rint;
	
	return NOERROR;
	}

STDMETHODIMP CAutoDocPreferences::put_ContourLinesPerSurface (int NumberOfLines)
	{
	if ((NumberOfLines < 0) || (NumberOfLines > 2047))
		return E_INVALIDARG;

	struct resbuf rb;
	rb.restype = RTSHORT;
	rb.resval.rint = NumberOfLines;
	
	db_setvar (NULL, DB_QISOLINES, &rb, CURDBDWG, NULL, NULL, 0);

	return NOERROR;
	}

STDMETHODIMP CAutoDocPreferences::get_DisplaySilhouette (VARIANT_BOOL * Visible)
	{
	if (Visible == NULL)
		return E_POINTER;
	
	struct resbuf rb;
	db_getvar (NULL, DB_QDISPSILH, &rb, CURDBDWG, NULL, NULL);

	*Visible =  (rb.resval.rint == 1) ? VARIANT_TRUE : VARIANT_FALSE;

	return NOERROR;
	}

STDMETHODIMP CAutoDocPreferences::put_DisplaySilhouette (VARIANT_BOOL Visible)
	{
	struct resbuf rb;
	rb.restype = RTSHORT;
	rb.resval.rint = (Visible == VARIANT_TRUE) ? 1 : 0;

	db_setvar (NULL, DB_QDISPSILH, &rb, CURDBDWG, NULL, NULL, 0);

	return NOERROR;
	}

STDMETHODIMP CAutoDocPreferences::get_MaxActiveViewports (int * Number)
	{
	if (Number == NULL)
		return E_POINTER;
	
	struct resbuf rb;
	db_getvar (NULL, DB_QMAXACTVP, &rb, CURDBDWG, NULL, NULL);
	
	*Number = (int)rb.resval.rint;
	
	return NOERROR;
	}

STDMETHODIMP CAutoDocPreferences::put_MaxActiveViewports (int Number)
	{
	if ((Number < 2) || (Number > 48))
		return E_INVALIDARG;

	struct resbuf rb;
	rb.restype = RTSHORT;
	rb.resval.rint = Number;
	
	db_setvar (NULL, DB_QMAXACTVP, &rb, CURDBDWG, NULL, NULL, 0);

	return NOERROR;
	}

STDMETHODIMP CAutoDocPreferences::get_ObjectSnapMode (ObjectSnapMode *SnapMode)
	{
	if (SnapMode == NULL)
		return E_POINTER;
	
	struct resbuf rb;
	db_getvar (NULL, DB_QFACETRES, &rb, CURDBDWG, NULL, NULL);
	
	*SnapMode = (ObjectSnapMode)rb.resval.rint;
	
	return NOERROR;
	}

STDMETHODIMP CAutoDocPreferences::put_ObjectSnapMode (ObjectSnapMode SnapMode)
	{
	if (SnapMode < 0)
		return E_INVALIDARG;

	struct resbuf rb;
	rb.restype = RTSHORT;
	rb.resval.rreal = SnapMode;
	
	db_setvar (NULL, DB_QOSMODE, &rb, CURDBDWG, NULL, NULL, 0);

	return NOERROR;
	}

STDMETHODIMP CAutoDocPreferences::get_RenderSmoothness (double * RenderSmoothness)
	{
	if (RenderSmoothness == NULL)
		return E_POINTER;
	
	struct resbuf rb;
	db_getvar (NULL, DB_QFACETRES, &rb, CURDBDWG, NULL, NULL);
	
	*RenderSmoothness = (int)rb.resval.rreal;
	
	return NOERROR;
	}

STDMETHODIMP CAutoDocPreferences::put_RenderSmoothness (double RenderSmoothness)
	{
	if ((RenderSmoothness < 0.01) || (RenderSmoothness > 10.0))
		return E_INVALIDARG;

	struct resbuf rb;
	rb.restype = RTREAL;
	rb.resval.rreal = RenderSmoothness;
	
	db_setvar (NULL, DB_QFACETRES, &rb, CURDBDWG, NULL, NULL, 0);

	return NOERROR;
	}

STDMETHODIMP CAutoDocPreferences::get_SegmentPerPolyline (int * Number)
	{
	if (Number == NULL)
		return E_POINTER;
	
	struct resbuf rb;
	db_getvar (NULL, DB_QSPLINESEGS, &rb, CURDBDWG, NULL, NULL);
	
	*Number = (int)rb.resval.rint;
	
	return NOERROR;
	}

STDMETHODIMP CAutoDocPreferences::put_SegmentPerPolyline (int Number)
	{
	if ((Number < -32768) || (Number > 32767))
		return E_INVALIDARG;

	struct resbuf rb;
	rb.restype = RTSHORT;
	rb.resval.rint = Number;
	
	db_setvar (NULL, DB_QSPLINESEGS, &rb, CURDBDWG, NULL, NULL, 0);

	return NOERROR;
	}

STDMETHODIMP CAutoDocPreferences::get_SolidFill (VARIANT_BOOL * SolidFill)
	{
	if (SolidFill == NULL)
		return E_POINTER;
	
	struct resbuf rb;
	db_getvar (NULL, DB_QFILLMODE, &rb, CURDBDWG, NULL, NULL);

	*SolidFill =  (rb.resval.rint == 1) ? VARIANT_TRUE : VARIANT_FALSE;

	return NOERROR;
	}

STDMETHODIMP CAutoDocPreferences::put_SolidFill (VARIANT_BOOL SolidFill)
	{
	struct resbuf rb;
	rb.restype = RTSHORT;
	rb.resval.rint = (SolidFill == VARIANT_TRUE) ? 1 : 0;

	db_setvar (NULL, DB_QFILLMODE, &rb, CURDBDWG, NULL, NULL, 0);

	return NOERROR;
	}

STDMETHODIMP CAutoDocPreferences::get_TextFrameDisplay (VARIANT_BOOL * TextFrameDisplay)
	{
	if (TextFrameDisplay == NULL)
		return E_POINTER;
	
	struct resbuf rb;
	db_getvar (NULL, DB_QQTEXTMODE, &rb, CURDBDWG, NULL, NULL);

	*TextFrameDisplay =  (rb.resval.rint == 1) ? VARIANT_TRUE : VARIANT_FALSE;

	return NOERROR;
	}

STDMETHODIMP CAutoDocPreferences::put_TextFrameDisplay (VARIANT_BOOL TextFrameDisplay)
	{
	struct resbuf rb;
	rb.restype = RTSHORT;
	rb.resval.rint = (TextFrameDisplay == VARIANT_TRUE) ? 1 : 0;

	db_setvar (NULL, DB_QQTEXTMODE, &rb, CURDBDWG, NULL, NULL, 0);

	return NOERROR;
	}

STDMETHODIMP CAutoDocPreferences::get_XRefLayerVisibility (VARIANT_BOOL * Visible)
	{
	if (Visible == NULL)
		return E_POINTER;
	
	struct resbuf rb;
	db_getvar (NULL, DB_QVISRETAIN, &rb, CURDBDWG, NULL, NULL);

	*Visible =  (rb.resval.rint == 1) ? VARIANT_TRUE : VARIANT_FALSE;

	return NOERROR;
	}

STDMETHODIMP CAutoDocPreferences::put_XRefLayerVisibility (VARIANT_BOOL Visible)
	{
	struct resbuf rb;
	rb.restype = RTSHORT;
	rb.resval.rint = (Visible == VARIANT_TRUE) ? 1 : 0;

	db_setvar (NULL, DB_QVISRETAIN, &rb, CURDBDWG, NULL, NULL, 0);

	return NOERROR;
	}



