/* INTELLICAD\PRJ\LIB\AUTO\AUTODOCPREFS.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * $Revision: 1.3 $ $Date: 2001/03/16 21:48:11 $ 
 * 
 * Abstract
 * 
 * Declaration of the CAutoDocPreferences - The Document level preferences
 * 
 */ 

#ifndef __AUTODOCPREFERENCES_H_
#define __AUTODOCPREFERENCES_H_


/////////////////////////////////////////////////////////////////////////////
// CAutoDocPreferences
class ATL_NO_VTABLE CAutoDocPreferences : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CAutoDocPreferences, &CLSID_DocumentPreferences>,
	public IDispatchImpl<IIcadDocumentPreferences, &IID_IIcadDocumentPreferences, &LIBID_IntelliCAD>
{
protected:
	CIcadDoc *m_pDoc;
	CIcadApp *m_pApp;

public:
	CAutoDocPreferences()
		{
		m_pApp = (CIcadApp *)GetApp();
		}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoDocPreferences)
		COM_INTERFACE_ENTRY(IIcadDocumentPreferences)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadDocumentPreferences)
	END_COM_MAP()

	virtual HRESULT Init (CIcadDoc *pDoc);

public:
	// IIcadDocumentPreferences
	STDMETHOD(get_Application)(IIcadApplication * * Application);
	STDMETHOD(get_Parent)(IIcadDocument * * Document);
	STDMETHOD(get_ContourLinesPerSurface)(int * NumberOfLines);
	STDMETHOD(put_ContourLinesPerSurface)(int NumberOfLines);
	STDMETHOD(get_DisplaySilhouette)(VARIANT_BOOL * Visible);
	STDMETHOD(put_DisplaySilhouette)(VARIANT_BOOL Visible);
	STDMETHOD(get_MaxActiveViewports)(int * Number);
	STDMETHOD(put_MaxActiveViewports)(int Number);
	STDMETHOD(get_ObjectSnapMode)(ObjectSnapMode * SnapMode);
	STDMETHOD(put_ObjectSnapMode)(ObjectSnapMode SnapMode);
	STDMETHOD(get_RenderSmoothness)(double * RenderSmoothness);
	STDMETHOD(put_RenderSmoothness)(double RenderSmoothness);
	STDMETHOD(get_SegmentPerPolyline)(int * Number);
	STDMETHOD(put_SegmentPerPolyline)(int Number);
	STDMETHOD(get_SolidFill)(VARIANT_BOOL * SolidFill);
	STDMETHOD(put_SolidFill)(VARIANT_BOOL SolidFill);
	STDMETHOD(get_TextFrameDisplay)(VARIANT_BOOL * TextFrameDisplay);
	STDMETHOD(put_TextFrameDisplay)(VARIANT_BOOL TextFrameDisplay);
	STDMETHOD(get_XRefLayerVisibility)(VARIANT_BOOL * Visible);
	STDMETHOD(put_XRefLayerVisibility)(VARIANT_BOOL Visible);
	
};

#endif //__AUTODOCPREFERENCES_H_


