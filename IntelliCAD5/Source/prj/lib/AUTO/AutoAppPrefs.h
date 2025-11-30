/* INTELLICAD\PRJ\LIB\AUTO\AUTOAPPPREFS.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * $Revision: 1.2 $ $Date: 2001/03/16 21:48:11 $ 
 * 
 * Abstract
 * 
 * Declaration of the CAutoAppPreferences - The Application level preferences
 * 
 */ 

#ifndef __AUTOAPPPREFERENCES_H_
#define __AUTOAPPPREFERENCES_H_


/////////////////////////////////////////////////////////////////////////////
// CAutoAppPreferences
class ATL_NO_VTABLE CAutoAppPreferences : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CAutoAppPreferences, &CLSID_ApplicationPreferences>,
	public IDispatchImpl<IIcadApplicationPreferences, &IID_IIcadApplicationPreferences, &LIBID_IntelliCAD>
{
private:
	CIcadApp *m_pApp;

public:
	CAutoAppPreferences()
		{
		m_pApp = (CIcadApp *)GetApp();
		}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoAppPreferences)
		COM_INTERFACE_ENTRY(IIcadApplicationPreferences)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadApplicationPreferences)
	END_COM_MAP()

public:
	// IIcadApplicationPreferences
	STDMETHOD(get_Application)(IIcadApplication * * Application);
	STDMETHOD(get_Parent)(IIcadApplication * * Application);
	STDMETHOD(get_CursorSize)(int * Size);
	STDMETHOD(put_CursorSize)(int Size);
	STDMETHOD(get_CursorXColor)(Colors * pColor);
	STDMETHOD(put_CursorXColor)(Colors pColor);
	STDMETHOD(get_CursorYColor)(Colors * pColor);
	STDMETHOD(put_CursorYColor)(Colors pColor);
	STDMETHOD(get_CursorZColor)(Colors * pColor);
	STDMETHOD(put_CursorZColor)(Colors pColor);
	STDMETHOD(get_DisplayScrollBars)(VARIANT_BOOL * Visible);
	STDMETHOD(put_DisplayScrollBars)(VARIANT_BOOL Visible);
	STDMETHOD(get_DockedVisibleLines)(int * NumberOfLines);
	STDMETHOD(put_DockedVisibleLines)(int NumberOfLines);
	STDMETHOD(get_GraphicsWinBackgrndColor)(Colors * pColor);
	STDMETHOD(put_GraphicsWinBackgrndColor)(Colors pColor);
	STDMETHOD(get_HistoryLines)(int * NumberOfLines);
	STDMETHOD(put_HistoryLines)(int NumberOfLines);
	STDMETHOD(get_MaxIntelliCADWindow)(VARIANT_BOOL * Maximize);
	STDMETHOD(put_MaxIntelliCADWindow)(VARIANT_BOOL Maximize);
	STDMETHOD(get_AutoSaveInterval)(int * Interval);
	STDMETHOD(put_AutoSaveInterval)(int Interval);
	STDMETHOD(get_LogFileName)(BSTR * LogFileName);
	STDMETHOD(put_LogFileName)(BSTR LogFileName);
	STDMETHOD(get_LogFileOn)(VARIANT_BOOL * On);
	STDMETHOD(put_LogFileOn)(VARIANT_BOOL On);
	STDMETHOD(get_AltFontFile)(BSTR * AltFontFileName);
	STDMETHOD(put_AltFontFile)(BSTR AltFontFileName);
};

#endif //__AUTOAPPPREFERENCES_H_


