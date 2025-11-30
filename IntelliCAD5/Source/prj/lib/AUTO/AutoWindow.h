/* INTELLICAD\PRJ\LIB\AUTO\AUTOWINDOW.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Declaration of CAutoWindow - the Automation Window object
 * 
 */ 

#ifndef __AUTOWINDOW_H_
#define __AUTOWINDOW_H_


/////////////////////////////////////////////////////////////////////////////
// CAutoWindow
class ATL_NO_VTABLE CAutoWindow : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CAutoWindow, &CLSID_Window>,
	public IDispatchImpl<IIcadWindow, &IID_IIcadWindow, &LIBID_IntelliCAD>
{
protected:
	CIcadDoc *m_pDoc;
	CWnd *m_pWnd;
	CComPtr<IIcadWindows> m_pParent;

public:
	CAutoWindow()
	{
		m_pDoc = NULL;
		m_pWnd = NULL;
		m_pParent = NULL;
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoWindow)
		COM_INTERFACE_ENTRY(IIcadWindow)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadWindow)
	END_COM_MAP()

	HRESULT Init(IIcadWindows *pParent, CWnd *pWnd, CIcadDoc *pDoc);

public:
// IIcadWindow
	STDMETHOD(get_Application)(IIcadApplication * * ppApp);
	STDMETHOD(get_Document)(IIcadDocument * * ppDoc);
	STDMETHOD(get_Parent)(IIcadWindows * * ppParent);
	STDMETHOD(get_Caption)(BSTR * pCaption);
	STDMETHOD(get_Visible)(VARIANT_BOOL * pVisible);
	STDMETHOD(put_Visible)(VARIANT_BOOL pVisible);
	STDMETHOD(get_WindowHandle32)(long * pHandle);
	STDMETHOD(get_WindowState)(WindowState * pState);
	STDMETHOD(put_WindowState)(WindowState pState);
	STDMETHOD(Activate)();
	STDMETHOD(Close)();
	STDMETHOD(GetWindowRect)(long * Left, long * Top, long * Width, long * Height);
	STDMETHOD(SetWindowRect)(long Left, long Top, long Width, long Height);
	STDMETHOD(Scroll)(ScrollFlagsX FlagsX, ScrollFlagsY FlagsY);
};

#endif //__AUTOWINDOW_H_


