/* INTELLICAD\PRJ\LIB\AUTO\AUTOVIEWS.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Declaration of CAutoViews - The Automation Views collection
 * 
 */ 

#ifndef __AUTOVIEWS_H_
#define __AUTOVIEWS_H_

/////////////////////////////////////////////////////////////////////////////
// CAutoViews
class ATL_NO_VTABLE CAutoViews : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CAutoViews, &CLSID_Views>,
	public IDispatchImpl<IIcadViews, &IID_IIcadViews, &LIBID_IntelliCAD>
{
protected:
	CIcadDoc *m_pDoc;

public:
	CAutoViews()
		{
		m_pDoc = NULL;
		}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoViews)
		COM_INTERFACE_ENTRY(IIcadViews)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadViews)
	END_COM_MAP()

	HRESULT Init (CIcadDoc *pDoc);

public:
	// IIcadViews
	STDMETHOD(get_Application)(IIcadApplication * * ppApp);
	STDMETHOD(get_Count)(long * pCount);
	STDMETHOD(get_Parent)(IIcadDocument * * ppParent);
	STDMETHOD(get__NewEnum)(IUnknown * * ppUnk);
	STDMETHOD(Add)(BSTR Name, IIcadView * * ppItem);
	STDMETHOD(Item)(VARIANT Index, IIcadView * * ppItem);
};



/////////////////////////////////////////////////////////////////////////////
// CAutoEnumViews
class ATL_NO_VTABLE CAutoEnumViews : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IEnumVARIANT, &IID_IEnumVARIANT, &LIBID_IntelliCAD>
{
protected:
	long m_nCurrent;
	CComPtr<IIcadViews> m_pViews;

public:
	CAutoEnumViews()
		{
		m_pViews = NULL;
		}
	~CAutoEnumViews()
		{
		if (m_pViews)
			m_pViews = NULL;
		}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoEnumViews)
		COM_INTERFACE_ENTRY(IEnumVARIANT)
		COM_INTERFACE_ENTRY2(IDispatch, IEnumVARIANT)
	END_COM_MAP()

	HRESULT Init (IIcadViews *pViews);

public:
	// IEnumVARIANT
	STDMETHOD(Next)(ULONG celt, VARIANT * rgvar, ULONG * pceltFetched);
	STDMETHOD(Skip)(ULONG celt);
	STDMETHOD(Reset)();
	STDMETHOD(Clone)(IEnumVARIANT * * ppenum);
};


#endif //__AUTOVIEWS_H_


