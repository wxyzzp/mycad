/* INTELLICAD\PRJ\LIB\AUTO\AUTODIMSTYLES.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Declaration of CAutoDimensionStyles
 * 
 */ 

#ifndef __AUTODIMSTYLES_H_
#define __AUTODIMSTYLES_H_


/////////////////////////////////////////////////////////////////////////////
// CAutoDimensionStyles
class ATL_NO_VTABLE CAutoDimensionStyles : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CAutoDimensionStyles, &CLSID_DimensionStyles>,
	public IDispatchImpl<IIcadDimensionStyles, &IID_IIcadDimensionStyles, &LIBID_IntelliCAD>
{
protected:
	CIcadDoc *m_pDoc;

public:
	CAutoDimensionStyles()
		{
		m_pDoc = NULL;
		}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoDimensionStyles)
		COM_INTERFACE_ENTRY(IIcadDimensionStyles)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadDimensionStyles)
	END_COM_MAP()

	HRESULT Init (CIcadDoc *pDoc);

public:
	// IIcadDimensionStyles
	STDMETHOD(get_Application)(IIcadApplication * * ppApp);
	STDMETHOD(get_Count)(long * pCount);
	STDMETHOD(get_Parent)(IIcadDocument * * ppParent);
	STDMETHOD(get__NewEnum)(IUnknown * * ppUnk);
	STDMETHOD(Add)(BSTR Name, IIcadDimensionStyle * * ppItem);
	STDMETHOD(Item)(VARIANT Index, IIcadDimensionStyle * * ppItem);
};


/////////////////////////////////////////////////////////////////////////////
// CAutoEnumDimensionStyles
class ATL_NO_VTABLE CAutoEnumDimensionStyles : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IEnumVARIANT, &IID_IEnumVARIANT, &LIBID_IntelliCAD>
{
protected:
	long m_nCurrent;
	CComPtr<IIcadDimensionStyles> m_pDimensionStyles;

public:
	CAutoEnumDimensionStyles()
		{
		m_pDimensionStyles = NULL;
		}
	~CAutoEnumDimensionStyles()
		{
		}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoEnumDimensionStyles)
		COM_INTERFACE_ENTRY(IEnumVARIANT)
		COM_INTERFACE_ENTRY2(IDispatch, IEnumVARIANT)
	END_COM_MAP()

	HRESULT Init (IIcadDimensionStyles *pDimensionStyles);

public:
	// IEnumVARIANT
	STDMETHOD(Next)(ULONG celt, VARIANT * rgvar, ULONG * pceltFetched);
	STDMETHOD(Skip)(ULONG celt);
	STDMETHOD(Reset)();
	STDMETHOD(Clone)(IEnumVARIANT * * ppenum);
};

#endif //__AUTODIMSTYLES_H_


