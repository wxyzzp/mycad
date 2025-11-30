/* INTELLICAD\PRJ\LIB\AUTO\AUTOREGAPPS.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Declaration of CAutoRegisteredApplications
 * 
 */ 

#ifndef __AUTOREGAPPS_H_
#define __AUTOREGAPPS_H_


/////////////////////////////////////////////////////////////////////////////
// CAutoRegisteredApplications
class ATL_NO_VTABLE CAutoRegisteredApplications : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CAutoRegisteredApplications, &CLSID_RegisteredApplications>,
	public IDispatchImpl<IIcadRegisteredApplications, &IID_IIcadRegisteredApplications, &LIBID_IntelliCAD>
{
protected:
	CIcadDoc *m_pDoc;

public:
	CAutoRegisteredApplications()
		{
		m_pDoc = NULL;
		}
	
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoRegisteredApplications)
		COM_INTERFACE_ENTRY(IIcadRegisteredApplications)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadRegisteredApplications)
	END_COM_MAP()

	HRESULT Init (CIcadDoc *pDoc);

public:
	// IIcadRegisteredApplications
	STDMETHOD(get_Application)(IIcadApplication * * ppApp);
	STDMETHOD(get_Count)(long * pCount);
	STDMETHOD(get_Parent)(IIcadDocument * * ppParent);
	STDMETHOD(get__NewEnum)(IUnknown * * ppUnk);
	STDMETHOD(Add)(BSTR Name, IIcadRegisteredApplication * * ppItem);
	STDMETHOD(Item)(VARIANT Index, IIcadRegisteredApplication * * ppItem);
};


/////////////////////////////////////////////////////////////////////////////
// CAutoEnumRegisteredApplications
class ATL_NO_VTABLE CAutoEnumRegisteredApplications : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IEnumVARIANT, &IID_IEnumVARIANT, &LIBID_IntelliCAD>
{
protected:
	long m_nCurrent;
	CComPtr<IIcadRegisteredApplications> m_pRegisteredApplications;

public:
	CAutoEnumRegisteredApplications()
		{
		m_pRegisteredApplications = NULL;
		}
	~CAutoEnumRegisteredApplications()
		{
		//if (m_pRegisteredApplications)
		//	m_pRegisteredApplications->Release();
		}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoEnumRegisteredApplications)
		COM_INTERFACE_ENTRY(IEnumVARIANT)
		COM_INTERFACE_ENTRY2(IDispatch, IEnumVARIANT)
	END_COM_MAP()

	HRESULT Init (IIcadRegisteredApplications *pRegisteredApplications);

public:
	// IEnumVARIANT
	STDMETHOD(Next)(ULONG celt, VARIANT * rgvar, ULONG * pceltFetched);
	STDMETHOD(Skip)(ULONG celt);
	STDMETHOD(Reset)();
	STDMETHOD(Clone)(IEnumVARIANT * * ppenum);
};

#endif //__AUTOREGAPPS_H_


