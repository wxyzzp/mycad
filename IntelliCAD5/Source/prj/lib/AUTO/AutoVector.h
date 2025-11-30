/* INTELLICAD\PRJ\LIB\AUTO\AUTOVECTOR.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Declaration of the CAutoVector- the Automation Vector object
 * 
 */ 

#ifndef __AUTOVECTOR_H_
#define __AUTOVECTOR_H_


/////////////////////////////////////////////////////////////////////////////
// CAutoVector
class AUTO_DECLSPEC ATL_NO_VTABLE CAutoVector : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CAutoVector, &CLSID_Vector>,
	public IDispatchImpl<IIcadVector, &IID_IIcadVector, &LIBID_IntelliCAD>
{
private:
	CComPtr<IIcadLibrary> m_pLib;
	double m_dX;
	double m_dY;
	double m_dZ;

public:
	CAutoVector()
		{
		m_pLib = NULL;
		}
	~CAutoVector()
		{
		//if (m_pLib)
		//	m_pLib->Release();
		}

	DECLARE_NO_REGISTRY()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoVector)
		COM_INTERFACE_ENTRY(IIcadVector)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadVector)
	END_COM_MAP()

	HRESULT Init (IIcadLibrary *pLib, double x, double y, double z);

public:
	// IIcadVector
	STDMETHOD(get_Application)(IIcadApplication * * ppApp);
	STDMETHOD(get_Parent)(IIcadLibrary * * ppParent);
	STDMETHOD(get_x)(double * pX);
	STDMETHOD(put_x)(double pX);
	STDMETHOD(get_y)(double * pY);
	STDMETHOD(put_y)(double pY);
	STDMETHOD(get_z)(double * pZ);
	STDMETHOD(put_z)(double pZ);
};

#endif //__AUTOVECTOR_H_


