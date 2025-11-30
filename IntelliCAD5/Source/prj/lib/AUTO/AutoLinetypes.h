/* INTELLICAD\PRJ\LIB\AUTO\AUTOLINETYPES.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Declaration of CAutoLinetypes
 * 
 */ 

#ifndef __AUTOLINETYPES_H_
#define __AUTOLINETYPES_H_


/////////////////////////////////////////////////////////////////////////////
// CAutoLinetypes
class ATL_NO_VTABLE CAutoLinetypes : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CAutoLinetypes, &CLSID_Linetypes>,
	public IDispatchImpl<IIcadLinetypes, &IID_IIcadLinetypes, &LIBID_IntelliCAD>
{
protected:
	CIcadDoc *m_pDoc;

public:
	CAutoLinetypes()
		{
		m_pDoc = NULL;
		}
	
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoLinetypes)
		COM_INTERFACE_ENTRY(IIcadLinetypes)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadLinetypes)
	END_COM_MAP()

	HRESULT Init (CIcadDoc *pDoc);

public:
// IIcadLinetypes
	STDMETHOD(get_Application)(IIcadApplication * * ppApp);
	STDMETHOD(get_Count)(long * pCount);
	STDMETHOD(get_Parent)(IIcadDocument * * ppParent);
	STDMETHOD(get__NewEnum)(IUnknown * * ppUnk);
	STDMETHOD(Add)(BSTR Name, IIcadLinetype * * ppItem);
	STDMETHOD(Item)(VARIANT Index, IIcadLinetype * * ppItem);
	STDMETHOD(Load)(BSTR LinetypeName, BSTR FileName);
};


/////////////////////////////////////////////////////////////////////////////
// CAutoEnumLinetypes
class ATL_NO_VTABLE CAutoEnumLinetypes : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IEnumVARIANT, &IID_IEnumVARIANT, &LIBID_IntelliCAD>
{
protected:
	long m_nCurrent;
	CComPtr<IIcadLinetypes> m_pLinetypes;

public:
	CAutoEnumLinetypes()
		{
		m_pLinetypes = NULL;
		}
	~CAutoEnumLinetypes()
		{
		//	if (m_pLinetypes)
		//		m_pLinetypes->Release();
		}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoEnumLinetypes)
		COM_INTERFACE_ENTRY(IEnumVARIANT)
		COM_INTERFACE_ENTRY2(IDispatch, IEnumVARIANT)
	END_COM_MAP()

	HRESULT Init (IIcadLinetypes *pLinetypes);

public:
	// IEnumVARIANT
	STDMETHOD(Next)(ULONG celt, VARIANT * rgvar, ULONG * pceltFetched);
	STDMETHOD(Skip)(ULONG celt);
	STDMETHOD(Reset)();
	STDMETHOD(Clone)(IEnumVARIANT * * ppenum);
};

#endif //__AUTOLINETYPES_H_


