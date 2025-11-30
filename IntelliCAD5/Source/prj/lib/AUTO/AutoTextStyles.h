/* INTELLICAD\PRJ\LIB\AUTO\AUTOTEXTSTYLES.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Declaration of CAutoTextStyles
 * 
 */ 

#ifndef __AUTOTEXTSTYLES_H_
#define __AUTOTEXTSTYLES_H_


/////////////////////////////////////////////////////////////////////////////
// CAutoTextStyles
class ATL_NO_VTABLE CAutoTextStyles : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CAutoTextStyles, &CLSID_TextStyles>,
	public IDispatchImpl<IIcadTextStyles, &IID_IIcadTextStyles, &LIBID_IntelliCAD>
{
protected:
	CIcadDoc *m_pDoc;

public:
	CAutoTextStyles()
		{
		m_pDoc = NULL;
		}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoTextStyles)
		COM_INTERFACE_ENTRY(IIcadTextStyles)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadTextStyles)
	END_COM_MAP()

	HRESULT Init (CIcadDoc *pDoc);

public:
	// IIcadTextStyles
	STDMETHOD(get_Application)(IIcadApplication * * ppApp);
	STDMETHOD(get_Count)(long * pCount);
	STDMETHOD(get_Parent)(IIcadDocument * * ppParent);
	STDMETHOD(get__NewEnum)(IUnknown * * ppUnk);
	STDMETHOD(Add)(BSTR Name, IIcadTextStyle * * ppItem);
	STDMETHOD(Item)(VARIANT Index, IIcadTextStyle * * ppItem);
};


/////////////////////////////////////////////////////////////////////////////
// CAutoEnumTextStyles
class ATL_NO_VTABLE CAutoEnumTextStyles : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IEnumVARIANT, &IID_IEnumVARIANT, &LIBID_IntelliCAD>
{
protected:
	long m_nCurrent;
	CComPtr<IIcadTextStyles> m_pTextStyles;

public:
	CAutoEnumTextStyles()
		{
		m_pTextStyles = NULL;
		}
	~CAutoEnumTextStyles()
		{
		//if (m_pTextStyles)
		//	m_pTextStyles->Release();
		}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoEnumTextStyles)
		COM_INTERFACE_ENTRY(IEnumVARIANT)
		COM_INTERFACE_ENTRY2(IDispatch, IEnumVARIANT)
	END_COM_MAP()

	HRESULT Init (IIcadTextStyles *pTextStyles);

public:
	// IEnumVARIANT
	STDMETHOD(Next)(ULONG celt, VARIANT * rgvar, ULONG * pceltFetched);
	STDMETHOD(Skip)(ULONG celt);
	STDMETHOD(Reset)();
	STDMETHOD(Clone)(IEnumVARIANT * * ppenum);
};

#endif //__AUTOTEXTSTYLES_H_


