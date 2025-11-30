/* INTELLICAD\PRJ\LIB\AUTO\AUTOSELSETS.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Declaration of the CAutoSelectionSets - the Automation Selection Sets Collection
 * 
 */ 

#ifndef __AUTOSELECTIONSETS_H_
#define __AUTOSELECTIONSETS_H_


/////////////////////////////////////////////////////////////////////////////
// CAutoSelectionSets
class ATL_NO_VTABLE CAutoSelectionSets : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CAutoSelectionSets, &CLSID_SelectionSets>,
	public IDispatchImpl<IIcadSelectionSets, &IID_IIcadSelectionSets, &LIBID_IntelliCAD>
{
private:
	CIcadDoc *m_pDoc;
	CSimpleArray<IIcadSelectionSet *> m_Contents;

public:
	CAutoSelectionSets()
		{
		}
	~CAutoSelectionSets()
		{
		int i;
		for (i = 0; i < m_Contents.GetSize(); i++)
			{
			((IIcadSelectionSet *)m_Contents[i])->Release();
			}
		}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoSelectionSets)
		COM_INTERFACE_ENTRY(IIcadSelectionSets)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadSelectionSets)
	END_COM_MAP()

	HRESULT Init (CIcadDoc *pDoc);

public:
	// IIcadSelectionSets
	STDMETHOD(get_Application)(IIcadApplication * * ppApp);
	STDMETHOD(get_Count)(long * pCount);
	STDMETHOD(get_Parent)(IIcadDocument * * ppParent);
	STDMETHOD(get__NewEnum)(IUnknown * * ppUnk);
	STDMETHOD(Add)(BSTR Name, IIcadSelectionSet * * ppItem);
	STDMETHOD(Item)(VARIANT Index, IIcadSelectionSet * * ppItem);
};


/////////////////////////////////////////////////////////////////////////////
// CAutoEnumSelectionSets
class ATL_NO_VTABLE CAutoEnumSelectionSets : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IEnumVARIANT, &IID_IEnumVARIANT, &LIBID_IntelliCAD>
{
protected:
	long m_nCurrent;
	CComPtr<IIcadSelectionSets> m_pSelSets;

public:
	CAutoEnumSelectionSets()
		{
		}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoEnumSelectionSets)
		COM_INTERFACE_ENTRY(IEnumVARIANT)
		COM_INTERFACE_ENTRY2(IDispatch, IEnumVARIANT)
	END_COM_MAP()

	HRESULT Init (IIcadSelectionSets *pSelectionSets);

public:
	// IEnumVARIANT
	STDMETHOD(Next)(ULONG celt, VARIANT * rgvar, ULONG * pceltFetched);
	STDMETHOD(Skip)(ULONG celt);
	STDMETHOD(Reset)();
	STDMETHOD(Clone)(IEnumVARIANT * * ppenum);
};


#endif //__AUTOSELECTIONSETS_H_


