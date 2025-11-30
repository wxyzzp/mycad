/* INTELLICAD\PRJ\LIB\AUTO\AUTOWINDOWS.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Declaration of CAutoWindows - the Windows collection object
 * 
 */ 

#ifndef __AUTOWINDOWS_H_
#define __AUTOWINDOWS_H_

/////////////////////////////////////////////////////////////////////////////
// CAutoWindows
class ATL_NO_VTABLE CAutoWindows : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CAutoWindows, &CLSID_Windows>,
	public IDispatchImpl<IIcadWindows, &IID_IIcadWindows, &LIBID_IntelliCAD>
{
private:
	CIcadDoc * m_pDoc;
	CSimpleArray<CWnd *> m_Contents;

public:
	CAutoWindows()
		{
		m_pDoc = NULL;
		}
	~CAutoWindows()
		{
		if (m_Contents.GetSize())
			m_Contents.RemoveAll();
		}
	
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoWindows)
		COM_INTERFACE_ENTRY(IIcadWindows)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadWindows)
	END_COM_MAP()

	HRESULT Init (CIcadDoc *pDoc);

public:
	// IIcadWindows
	STDMETHOD(get_Application)(IIcadApplication * * ppApp);
	STDMETHOD(get_Count)(long * pCount);
	STDMETHOD(get_Parent)(IIcadApplication * * ppParent);
	STDMETHOD(get__NewEnum)(IUnknown * * ppUnk);
	STDMETHOD(Add)(IIcadWindow * * ppWindow);
	STDMETHOD(Arrange)(ArrangeStyle Style);
	STDMETHOD(CloseAll)();
	STDMETHOD(Item)(VARIANT Index, IIcadWindow * * ppItem);
};


/////////////////////////////////////////////////////////////////////////////
// CAutoEnumWindows

class ATL_NO_VTABLE CAutoEnumWindows : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IEnumVARIANT, &IID_IEnumVARIANT, &LIBID_IntelliCAD>
{
protected:
	long m_nCurrent;
	CComPtr<IIcadWindows> m_pWindows;

public:
	CAutoEnumWindows()
		{
		m_pWindows = NULL;
		}
	~CAutoEnumWindows()
		{
		if (m_pWindows)
			m_pWindows = NULL;
		}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoEnumWindows)
		COM_INTERFACE_ENTRY(IEnumVARIANT)
		COM_INTERFACE_ENTRY2(IDispatch, IEnumVARIANT)
	END_COM_MAP()

	HRESULT Init (IIcadWindows *pWindows);

public:
	// IEnumVARIANT
	STDMETHOD(Next)(ULONG celt, VARIANT * rgvar, ULONG * pceltFetched);
	STDMETHOD(Skip)(ULONG celt);
	STDMETHOD(Reset)();
	STDMETHOD(Clone)(IEnumVARIANT * * ppenum);
};

#endif //__AUTOWINDOWS_H_


