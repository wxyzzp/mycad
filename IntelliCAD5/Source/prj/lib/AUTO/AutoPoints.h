/* INTELLICAD\PRJ\LIB\AUTO\AUTOPOINTS.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Declaration of the CAutoPoints - the Automation Points Collection
 * 
 */ 


#ifndef __AUTOPOINTS_H_
#define __AUTOPOINTS_H_


/////////////////////////////////////////////////////////////////////////////
// CAutoPoints
class AUTO_DECLSPEC ATL_NO_VTABLE CAutoPoints : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CAutoPoints, &CLSID_Points>,
	public IDispatchImpl<IIcadPoints, &IID_IIcadPoints, &LIBID_IntelliCAD>
{
private:
	CComPtr<IIcadLibrary> m_pLib;
	CSimpleArray<IIcadPoint *> m_Contents;

public:
	CAutoPoints()
		{
		m_pLib = NULL;
		}
	~CAutoPoints()
		{
		//if (m_pLib)
		//	m_pLib->Release();
		//m_Contents.RemoveAll();
		int i;
		for (i = 0; i < m_Contents.GetSize(); i++)
			{
			((IIcadPoint *)m_Contents[i])->Release();
			}
		}

	DECLARE_NO_REGISTRY()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoPoints)
		COM_INTERFACE_ENTRY(IIcadPoints)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadPoints)
	END_COM_MAP()

	HRESULT Init (IIcadLibrary *pLib);

public:
	// IIcadPoints
	STDMETHOD(get_Application)(IIcadApplication * * ppApp);
	STDMETHOD(get_Parent)(IIcadLibrary * * ppParent);
	STDMETHOD(get_Count)(long * pCount);
	STDMETHOD(get__NewEnum)(IUnknown * * ppUnk);
	STDMETHOD(Add)(double x, double y, double z, long Index, IIcadPoint * * Point);
	STDMETHOD(Item)(long Index, IIcadPoint * * ppItem);
	STDMETHOD(Remove)(long Index, IIcadPoint * * Point);
	STDMETHOD(RemoveAll)();
};

// AutoPoints.h : Declaration of the CAutoEnumPoints


/////////////////////////////////////////////////////////////////////////////
// CAutoEnumPoints
class ATL_NO_VTABLE CAutoEnumPoints : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IEnumVARIANT, &IID_IEnumVARIANT, &LIBID_IntelliCAD>
{
protected:
	long m_nCurrent;
	CComPtr<IIcadPoints> m_pPoints;

public:
	CAutoEnumPoints()
		{
		}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoEnumPoints)
		COM_INTERFACE_ENTRY(IEnumVARIANT)
		COM_INTERFACE_ENTRY2(IDispatch, IEnumVARIANT)
	END_COM_MAP()

	HRESULT Init (IIcadPoints *pPoints);

public:
	// IEnumVARIANT
	STDMETHOD(Next)(ULONG celt, VARIANT * rgvar, ULONG * pceltFetched);
	STDMETHOD(Skip)(ULONG celt);
	STDMETHOD(Reset)();
	STDMETHOD(Clone)(IEnumVARIANT * * ppenum);
};

#endif //__AUTOPOINTS_H_


