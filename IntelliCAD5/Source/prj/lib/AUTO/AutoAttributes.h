/* INTELLICAD\PRJ\LIB\AUTO\AUTOATTRIBUTES.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Implementation of CAutoAttributes - the Automation Attributes collection
 * 
 */ 

#ifndef __AUTOATTRIBUTES_H_
#define __AUTOATTRIBUTES_H_


/////////////////////////////////////////////////////////////////////////////
// CAutoAttributes
class ATL_NO_VTABLE CAutoAttributes : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CAutoAttributes, &CLSID_Attributes>,
	public IDispatchImpl<IIcadAttributes, &IID_IIcadAttributes, &LIBID_IntelliCAD>
{
private:
	CIcadDoc *m_pDoc;
	CComPtr<IDispatch> m_pBlockInsert;
	db_handitem *m_pDbHandItem;		//pointer to the first attribute in the linked list of attributes

public:
	CAutoAttributes()
		{
		m_pDoc = NULL;
		m_pBlockInsert = NULL;
		m_pDbHandItem = NULL;
		}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoAttributes)
		COM_INTERFACE_ENTRY(IIcadAttributes)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadAttributes)
	END_COM_MAP()

	virtual HRESULT Init (CIcadDoc *pDoc, IDispatch *pParentBlockInsert, db_handitem *pDbHandItem);

public:
	// IIcadAttributes
	STDMETHOD(get_Application)(IIcadApplication * * ppApp);
	STDMETHOD(get_Parent)(IDispatch * * ppParent);
	STDMETHOD(get_Count)(long * pCount);
	STDMETHOD(get__NewEnum)(IUnknown * * ppUnk);
	STDMETHOD(Add)(IIcadAttribute * * Attribute);
	STDMETHOD(Item)(VARIANT Index, IIcadAttribute * * ppItem);
};


/////////////////////////////////////////////////////////////////////////////
// CAutoEnumAttributes
class ATL_NO_VTABLE CAutoEnumAttributes : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IEnumVARIANT, &IID_IEnumVARIANT, &LIBID_IntelliCAD>
{
protected:
	long m_nCurrent;
	CComPtr<IIcadAttributes> m_pAttributes;

public:
	CAutoEnumAttributes()
		{
		m_pAttributes = NULL;
		}
	~CAutoEnumAttributes()
		{
		if (m_pAttributes)
			m_pAttributes = NULL;
		}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoEnumAttributes)
		COM_INTERFACE_ENTRY(IEnumVARIANT)
		COM_INTERFACE_ENTRY2(IDispatch, IEnumVARIANT)
	END_COM_MAP()

	HRESULT Init (IIcadAttributes *pAttributes);

public:
	// IEnumVARIANT
	STDMETHOD(Next)(ULONG celt, VARIANT * rgvar, ULONG * pceltFetched);
	STDMETHOD(Skip)(ULONG celt);
	STDMETHOD(Reset)();
	STDMETHOD(Clone)(IEnumVARIANT * * ppenum);
};


#endif //__AUTOATTRIBUTES_H_


