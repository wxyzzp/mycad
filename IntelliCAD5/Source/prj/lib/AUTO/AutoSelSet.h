/* INTELLICAD\PRJ\LIB\AUTO\AUTOSELSET.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Declaration of CAutoSelectionSet - - the Automation SelectionSet object
 * 
 */ 

#ifndef __AUTOSELECTIONSET_H_
#define __AUTOSELECTIONSET_H_

#include "AutoSelSets.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoSelectionSet
class ATL_NO_VTABLE CAutoSelectionSet : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CAutoSelectionSet, &CLSID_SelectionSet>,
	public IDispatchImpl<IIcadSelectionSet, &IID_IIcadSelectionSet, &LIBID_IntelliCAD>
{
private:
	CIcadDoc *m_pDoc;
	CString m_Name;
	CSimpleArray<IDispatch *> m_Contents;

public:
	CAutoSelectionSet()
		{
		}
	~CAutoSelectionSet()
		{
		int i;
		for (i = 0; i < m_Contents.GetSize(); i++)
			{
			((IDispatch *)m_Contents[i])->Release();
			}
		}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoSelectionSet)
		COM_INTERFACE_ENTRY(IIcadSelectionSet)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadSelectionSet)
	END_COM_MAP()

	HRESULT Init (CIcadDoc *pDoc, CString Name);
	HRESULT InitializeFromSDSsset (sds_name sset);
	HRESULT Add (IDispatch *pDisp);
	HRESULT UpdateActiveSelectionSet ();

public:
	// IIcadSelectionSet
	STDMETHOD(get_Application)(IIcadApplication * * ppApp);
	STDMETHOD(get_Count)(long * pCount);
	STDMETHOD(get_Document)(IIcadDocument * * ppDoc);
	STDMETHOD(get_Name)(BSTR * pstrName);
	STDMETHOD(get__NewEnum)(IUnknown * * ppUnk);
	STDMETHOD(get_Parent)(IIcadSelectionSets * * ppParent);
	STDMETHOD(get_Color)(Colors * pColor);
	STDMETHOD(put_Color)(Colors pColor);
	STDMETHOD(get_Layer)(BSTR * pLayer);
	STDMETHOD(put_Layer)(BSTR pLayer);
	STDMETHOD(get_Linetype)(BSTR * pLinetype);
	STDMETHOD(put_Linetype)(BSTR pLinetype);
	STDMETHOD(get_Visible)(VARIANT_BOOL * pVisible);
	STDMETHOD(put_Visible)(VARIANT_BOOL pVisible);
	STDMETHOD(AddItems)(VARIANT Ents);
	STDMETHOD(Clear)();
	STDMETHOD(Delete)();
	STDMETHOD(Erase)();
	STDMETHOD(Highlight)(VARIANT_BOOL HighlightFlag);
	STDMETHOD(Item)(long Index, IDispatch * * ppItem);
	STDMETHOD(RemoveItems)(VARIANT Ents);
	STDMETHOD(Select)(SelectionSetType Type, IIcadPoint * Point1, IIcadPoint * Point2, VARIANT FilterType, VARIANT FilterData);
	STDMETHOD(SelectAtPoint)(IIcadPoint * Point, VARIANT FilterType, VARIANT FilterData);
	STDMETHOD(SelectByPolygon)(SelectionSetType Type, IIcadPoints * PolygonPoints, VARIANT FilterType, VARIANT FilterData);
	STDMETHOD(SelectOnScreen)(VARIANT FilterType, VARIANT FilterData);
	STDMETHOD(Update)();
	STDMETHOD(Copy)(IIcadSelectionSet * * ppCopy);
	STDMETHOD(Mirror)(IIcadPoint * Point1, IIcadPoint * Point2, VARIANT_BOOL MakeCopy, IIcadSelectionSet * * ppCopy);
	STDMETHOD(Mirror3D)(IIcadPoint * Point1, IIcadPoint * Point2, IIcadPoint * Point3, VARIANT_BOOL MakeCopy, IIcadSelectionSet * * ppCopy);
	STDMETHOD(Move)(IIcadPoint * Point1, IIcadPoint * Point2);
	STDMETHOD(Rotate)(IIcadPoint * BasePoint, double RotationAngle);
	STDMETHOD(Rotate3D)(IIcadPoint * Point1, IIcadPoint * Point2, double RotationAngle);
	STDMETHOD(ScaleEntity)(IIcadPoint * BasePoint, double ScaleFactor);
	STDMETHOD(TransformBy)(IIcadMatrix * Matrix);
};


/////////////////////////////////////////////////////////////////////////////
// CAutoEnumSelSetEnts
class ATL_NO_VTABLE CAutoEnumSelSetEnts : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IEnumVARIANT, &IID_IEnumVARIANT, &LIBID_IntelliCAD>
{
protected:
	long		m_nCurrent;
	CIcadDoc	*m_pDoc;
	CComPtr<IIcadSelectionSet> m_pSelSet;

public:
	CAutoEnumSelSetEnts()
		{
		}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoEnumSelSetEnts)
		COM_INTERFACE_ENTRY(IEnumVARIANT)
		COM_INTERFACE_ENTRY2(IDispatch, IEnumVARIANT)
	END_COM_MAP()

	HRESULT Init (IIcadSelectionSet *pSelSet);

public:
	// IEnumVARIANT
	STDMETHOD(Next)(ULONG celt, VARIANT * rgvar, ULONG * pceltFetched);
	STDMETHOD(Skip)(ULONG celt);
	STDMETHOD(Reset)();
	STDMETHOD(Clone)(IEnumVARIANT * * ppenum);
};

#endif //__AUTOSELECTIONSET_H_


