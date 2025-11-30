/* INTELLICAD\PRJ\LIB\AUTO\AUTODOCUMENTS.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Declaration of CAutoDocuments - the Automation Documents collection
 * 
 */ 

#ifndef __AUTODOCUMENTS_H_
#define __AUTODOCUMENTS_H_

class CIcadDoc;
class CIcadMultiDocTemplate;

/////////////////////////////////////////////////////////////////////////////
// CAutoDocuments
class ATL_NO_VTABLE CAutoDocuments : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CAutoDocuments, &CLSID_Documents>,
	public IDispatchImpl<IIcadDocuments, &IID_IIcadDocuments, &LIBID_IntelliCAD>
{
private:
	CIcadMultiDocTemplate* m_pDocTemplate;

public:
	CAutoDocuments()
		{
		m_pDocTemplate = NULL;
		}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoDocuments)
		COM_INTERFACE_ENTRY(IIcadDocuments)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadDocuments)
	END_COM_MAP()

	HRESULT Init (CIcadMultiDocTemplate* pDocTemplate);

public:
	// IIcadDocuments
	STDMETHOD(get_Application)(IIcadApplication * * ppApp);
	STDMETHOD(get_Count)(long * pCount);
	STDMETHOD(get_Parent)(IIcadApplication * * ppParent);
	STDMETHOD(get__NewEnum)(IUnknown * * ppUnk);
	STDMETHOD(Add)(IIcadDocument * * ppDocument);
	STDMETHOD(CloseAll)(VARIANT_BOOL AskSave);
	STDMETHOD(Item)(VARIANT Index, IIcadDocument * * ppItem);
	STDMETHOD(Open)(BSTR File, VARIANT_BOOL ReadOnly, IIcadDocument * * ppDocument);
	STDMETHOD(SaveAll)();
};



/////////////////////////////////////////////////////////////////////////////
// CAutoEnumDocuments
class ATL_NO_VTABLE CAutoEnumDocuments : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IEnumVARIANT, &IID_IEnumVARIANT, &LIBID_IntelliCAD>
{
protected:
	long m_nCurrent;
	CComPtr<IIcadDocuments> m_pDocuments;

public:
	CAutoEnumDocuments()
		{
		m_pDocuments = NULL;
		}
	~CAutoEnumDocuments()
		{
		if (m_pDocuments)
			m_pDocuments = NULL;
		}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoEnumDocuments)
		COM_INTERFACE_ENTRY(IEnumVARIANT)
		COM_INTERFACE_ENTRY2(IDispatch, IEnumVARIANT)
	END_COM_MAP()

	HRESULT Init (IIcadDocuments *pDocuments);

public:
	// IEnumVARIANT
	STDMETHOD(Next)(ULONG celt, VARIANT * rgvar, ULONG * pceltFetched);
	STDMETHOD(Skip)(ULONG celt);
	STDMETHOD(Reset)();
	STDMETHOD(Clone)(IEnumVARIANT * * ppenum);
};


#endif //__AUTODOCUMENTS_H_


