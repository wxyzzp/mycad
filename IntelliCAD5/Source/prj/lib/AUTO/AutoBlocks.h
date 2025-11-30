/* INTELLICAD\PRJ\LIB\AUTO\AUTOBLOCKS.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Declaration of CAutoBlocks
 * 
 */ 

#ifndef __AUTOBLOCKS_H_
#define __AUTOBLOCKS_H_


/////////////////////////////////////////////////////////////////////////////
// CAutoBlocks
class ATL_NO_VTABLE CAutoBlocks : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CAutoBlocks, &CLSID_Blocks>,
	public IDispatchImpl<IIcadBlocks, &IID_IIcadBlocks, &LIBID_IntelliCAD>
{
protected:
	CIcadDoc *m_pDoc;

public:
	CAutoBlocks()
		{
		m_pDoc = NULL;
		}
	
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoBlocks)
		COM_INTERFACE_ENTRY(IIcadBlocks)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadBlocks)
	END_COM_MAP()

	HRESULT Init (CIcadDoc *pDoc);

public:
	// IIcadBlocks
	STDMETHOD(get_Application)(IIcadApplication * * ppApp);
	STDMETHOD(get_Count)(long * pCount);
	STDMETHOD(get_Parent)(IIcadDocument * * ppParent);
	STDMETHOD(get__NewEnum)(IUnknown * * ppUnk);
	STDMETHOD(Add)(IIcadPoint * InsertionPoint, BSTR Name, IIcadBlock * * ppItem);
	STDMETHOD(Item)(VARIANT Index, IIcadBlock * * ppItem);
};


/////////////////////////////////////////////////////////////////////////////
// CAutoEnumBlocks
class ATL_NO_VTABLE CAutoEnumBlocks : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IEnumVARIANT, &IID_IEnumVARIANT, &LIBID_IntelliCAD>
{
protected:
	long m_nCurrent;
	CComPtr<IIcadBlocks> m_pBlocks;

public:
	CAutoEnumBlocks()
		{
		m_pBlocks = NULL;
		}
	~CAutoEnumBlocks()
		{
		if (m_pBlocks)
			m_pBlocks = NULL;
		}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoEnumBlocks)
		COM_INTERFACE_ENTRY(IEnumVARIANT)
		COM_INTERFACE_ENTRY2(IDispatch, IEnumVARIANT)
	END_COM_MAP()

	HRESULT Init (IIcadBlocks *pBlocks);

public:
	// IEnumVARIANT
	STDMETHOD(Next)(ULONG celt, VARIANT * rgvar, ULONG * pceltFetched);
	STDMETHOD(Skip)(ULONG celt);
	STDMETHOD(Reset)();
	STDMETHOD(Clone)(IEnumVARIANT * * ppenum);
};


#endif //__AUTOBLOCKS_H_


