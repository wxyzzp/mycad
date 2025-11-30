/* INTELLICAD\PRJ\LIB\AUTO\AUTOMATRIX.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Declaration of the CAutoMatrix- the Automation Matrix object
 * 
 */ 

#ifndef __AUTOMATRIX_H_
#define __AUTOMATRIX_H_


/////////////////////////////////////////////////////////////////////////////
// CAutoMatrix
class AUTO_DECLSPEC ATL_NO_VTABLE CAutoMatrix : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CAutoMatrix, &CLSID_Matrix>,
	public IDispatchImpl<IIcadMatrix, &IID_IIcadMatrix, &LIBID_IntelliCAD>
{
private:
	CComPtr<IIcadLibrary> m_pLib;
	double m_value[4][4];

public:
	CAutoMatrix()
		{
		m_pLib = NULL;
		}
	~CAutoMatrix()
		{
		}
	
	DECLARE_NO_REGISTRY()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoMatrix)
		COM_INTERFACE_ENTRY(IIcadMatrix)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadMatrix)
	END_COM_MAP()

	HRESULT Init (IIcadLibrary *pLib);

public:
	// IIcadMatrix
	STDMETHOD(get_Application)(IIcadApplication * * ppApp);
	STDMETHOD(get_Parent)(IIcadLibrary * * ppParent);
	STDMETHOD(GetValue)(int Row, int Column, double * pValue);
	STDMETHOD(SetValue)(int Row, int Column, double Value);
};

#endif //__AUTOMATRIX_H_


