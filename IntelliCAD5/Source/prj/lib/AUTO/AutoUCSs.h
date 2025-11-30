/* INTELLICAD\PRJ\LIB\AUTO\AUTOUCSS.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Declaration of CAutoUCSs - The Automation UCS collection
 * 
 */ 

#ifndef __AUTOUCSS_H_
#define __AUTOUCSS_H_

/////////////////////////////////////////////////////////////////////////////
// CAutoUCSs
class ATL_NO_VTABLE CAutoUCSs : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CAutoUCSs, &CLSID_UserCoordSystems>,
	public IDispatchImpl<IIcadUserCoordSystems, &IID_IIcadUserCoordSystems, &LIBID_IntelliCAD>
{
protected:
	CIcadDoc *m_pDoc;

public:
	CAutoUCSs()
		{
		m_pDoc = NULL;
		}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoUCSs)
		COM_INTERFACE_ENTRY(IIcadUserCoordSystems)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadUserCoordSystems)
	END_COM_MAP()

	HRESULT Init (CIcadDoc *pDoc);

public:
	// IIcadUserCoordSystems
	STDMETHOD(get_Application)(IIcadApplication * * ppApp);
	STDMETHOD(get_Count)(long * pCount);
	STDMETHOD(get_Parent)(IIcadDocument * * ppParent);
	STDMETHOD(get__NewEnum)(IUnknown * * ppUnk);
	STDMETHOD(Add)(IIcadPoint * Origin, IIcadVector * XAxis, IIcadVector * YAxis, BSTR Name, IIcadUserCoordSystem * * ppItem);
	STDMETHOD(Item)(VARIANT Index, IIcadUserCoordSystem * * ppItem);
};


/////////////////////////////////////////////////////////////////////////////
// CAutoEnumUCSs
class ATL_NO_VTABLE CAutoEnumUCSs : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IEnumVARIANT, &IID_IEnumVARIANT, &LIBID_IntelliCAD>
{
protected:
	long m_nCurrent;
	CComPtr<IIcadUserCoordSystems> m_pUCSs;

public:
	CAutoEnumUCSs()
		{
		m_pUCSs = NULL;
		}
	~CAutoEnumUCSs()
		{
		if (m_pUCSs)
			m_pUCSs = NULL;
		}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoEnumUCSs)
		COM_INTERFACE_ENTRY(IEnumVARIANT)
		COM_INTERFACE_ENTRY2(IDispatch, IEnumVARIANT)
	END_COM_MAP()

	HRESULT Init (IIcadUserCoordSystems *pUCSs);

public:
	// IEnumVARIANT
	STDMETHOD(Next)(ULONG celt, VARIANT * rgvar, ULONG * pceltFetched);
	STDMETHOD(Skip)(ULONG celt);
	STDMETHOD(Reset)();
	STDMETHOD(Clone)(IEnumVARIANT * * ppenum);
};

#endif //__AUTOUCSS_H_


