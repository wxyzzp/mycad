/* INTELLICAD\PRJ\LIB\AUTO\AUTOLAYERS.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Declaration of CAutoLayers
 * 
 */ 

#ifndef __AUTOLAYERS_H_
#define __AUTOLAYERS_H_


/////////////////////////////////////////////////////////////////////////////
// CAutoLayers
class ATL_NO_VTABLE CAutoLayers : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CAutoLayers, &CLSID_Layers>,
	public IDispatchImpl<IIcadLayers, &IID_IIcadLayers, &LIBID_IntelliCAD>
{
protected:
	CIcadDoc *m_pDoc;

public:
	CAutoLayers()
		{
		m_pDoc = NULL;
		}
	~CAutoLayers()
		{
		if (m_pDoc)
			m_pDoc = NULL;
		}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoLayers)
		COM_INTERFACE_ENTRY(IIcadLayers)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadLayers)
	END_COM_MAP()

	HRESULT Init (CIcadDoc *pDoc);

public:
	// IIcadLayers
	STDMETHOD(get_Application)(IIcadApplication * * ppApp);
	STDMETHOD(get_Count)(long * pCount);
	STDMETHOD(get_Parent)(IIcadDocument * * ppParent);
	STDMETHOD(get__NewEnum)(IUnknown * * ppUnk);
	STDMETHOD(Add)(BSTR Name, IIcadLayer * * ppItem);
	STDMETHOD(Item)(VARIANT Index, IIcadLayer * * ppItem);
};


/////////////////////////////////////////////////////////////////////////////
// CAutoEnumLayers
class ATL_NO_VTABLE CAutoEnumLayers : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IEnumVARIANT, &IID_IEnumVARIANT, &LIBID_IntelliCAD>
{
protected:
	long m_nCurrent;
	CComPtr<IIcadLayers> m_pLayers;

public:
	CAutoEnumLayers()
		{
		m_pLayers = NULL;
		}
	~CAutoEnumLayers()
		{
		if (m_pLayers)
			m_pLayers = NULL;
		}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoEnumLayers)
		COM_INTERFACE_ENTRY(IEnumVARIANT)
		COM_INTERFACE_ENTRY2(IDispatch, IEnumVARIANT)
	END_COM_MAP()

	HRESULT Init (IIcadLayers *pLayers);

public:
	// IEnumVARIANT
	STDMETHOD(Next)(ULONG celt, VARIANT * rgvar, ULONG * pceltFetched);
	STDMETHOD(Skip)(ULONG celt);
	STDMETHOD(Reset)();
	STDMETHOD(Clone)(IEnumVARIANT * * ppenum);
};

#endif //__AUTOLAYERS_H_


