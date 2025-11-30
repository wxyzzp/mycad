/* INTELLICAD\PRJ\LIB\AUTO\AUTOVIEWPORTS.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Declaration of CAutoViewports - The Automation Viewports collection
 * 
 */ 

#ifndef __AUTOVIEWPORTS_H_
#define __AUTOVIEWPORTS_H_


/////////////////////////////////////////////////////////////////////////////
// CAutoViewports
class ATL_NO_VTABLE CAutoViewports : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CAutoViewports, &CLSID_Viewports>,
	public IDispatchImpl<IIcadViewports, &IID_IIcadViewports, &LIBID_IntelliCAD>
{
protected:
	CIcadDoc *m_pDoc;

public:
	CAutoViewports()
		{
		m_pDoc = NULL;
		}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoViewports)
		COM_INTERFACE_ENTRY(IIcadViewports)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadViewports)
	END_COM_MAP()

	HRESULT Init (CIcadDoc *pDoc);

public:
	// IIcadViewports
	STDMETHOD(get_Application)(IIcadApplication * * ppApp);
	STDMETHOD(get_Count)(long * pCount);
	STDMETHOD(get_Parent)(IIcadDocument * * ppParent);
	STDMETHOD(get__NewEnum)(IUnknown * * ppUnk);
	STDMETHOD(Add)(BSTR Name, IIcadViewport * * ppItem);
	STDMETHOD(Item)(VARIANT Index, IIcadViewport * * ppItem);
};


/////////////////////////////////////////////////////////////////////////////
// CAutoEnumViewports
class ATL_NO_VTABLE CAutoEnumViewports : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IEnumVARIANT, &IID_IEnumVARIANT, &LIBID_IntelliCAD>
{
protected:
	long m_nCurrent;
	CComPtr<IIcadViewports> m_pViewports;

public:
	CAutoEnumViewports()
		{
		m_pViewports = NULL;
		}
	~CAutoEnumViewports()
		{
		if (m_pViewports)
			m_pViewports = NULL;
		}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoEnumViewports)
		COM_INTERFACE_ENTRY(IEnumVARIANT)
		COM_INTERFACE_ENTRY2(IDispatch, IEnumVARIANT)
	END_COM_MAP()

	HRESULT Init (IIcadViewports *pViewports);

public:
	// IEnumVARIANT
	STDMETHOD(Next)(ULONG celt, VARIANT * rgvar, ULONG * pceltFetched);
	STDMETHOD(Skip)(ULONG celt);
	STDMETHOD(Reset)();
	STDMETHOD(Clone)(IEnumVARIANT * * ppenum);
};


#endif //__AUTOVIEWPORTS_H_


