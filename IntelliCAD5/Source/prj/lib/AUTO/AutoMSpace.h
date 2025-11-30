/* INTELLICAD\PRJ\LIB\AUTO\AUTOMSPACE.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Declaration of the CAutoMSpace - The collection of all Model space entities.
 * 
 */ 

#ifndef __AUTOMSPACE_H_
#define __AUTOMSPACE_H_

#include "IcadDoc.h"
#include "AutoBaseDefs.h"
#include "AutoEntities.h"
class db_handitem;

/////////////////////////////////////////////////////////////////////////////
// CAutoMSpace
class ATL_NO_VTABLE CAutoMSpace : 
	public CAutoEntities,
	public CComCoClass<CAutoMSpace, &CLSID_ModelSpace>,
	public IDispatchImpl<IIcadModelSpace, &IID_IIcadModelSpace, &LIBID_IntelliCAD>
{
protected:
	CIcadDoc *m_pDoc;

public:
	CAutoMSpace()
		{
		m_pDoc = NULL;
		}
	~CAutoMSpace()
		{
		if (m_pDoc)
			m_pDoc = NULL;
		}
	
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoMSpace)
		COM_INTERFACE_ENTRY(IIcadModelSpace)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadModelSpace)
		COM_INTERFACE_ENTRY_CHAIN(CAutoEntities)
	END_COM_MAP()

	HRESULT Init (CIcadDoc *pDoc);
	HRESULT AddItemToDrawing (db_handitem *pDbHandItem, BOOL complex = FALSE);
	CIcadDoc * GetDocument ();	//return the document in which this collection exists
	HRESULT GetOwner (IDispatch **ppOwner); //return a pointer to the owner - PSpace
	virtual BOOL OwnerIsModelSpace() {return TRUE;};
	virtual BOOL OwnerIsPaperSpace() {return FALSE;};
	virtual BOOL OwnerIsBlock() {return FALSE;};

public:
	//  IIcadEntities
	DEFINE_IICADENTITIES_METHODS
	
	// IIcadModelSpace - no methods

};


/////////////////////////////////////////////////////////////////////////////
// CAutoEnumMSpaceEnts
class ATL_NO_VTABLE CAutoEnumMSpaceEnts : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IEnumVARIANT, &IID_IEnumVARIANT, &LIBID_IntelliCAD>
{
protected:
	long		m_nElements;
	long		m_nCurrent;
	db_handitem *m_hipCurrent;
	CIcadDoc	*m_pDoc;
	CAutoMSpace *m_pMSpace;

public:
	CAutoEnumMSpaceEnts()
		{
		}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoEnumMSpaceEnts)
		COM_INTERFACE_ENTRY(IEnumVARIANT)
		COM_INTERFACE_ENTRY2(IDispatch, IEnumVARIANT)
	END_COM_MAP()

	HRESULT Init (CAutoMSpace *pMSpace, CIcadDoc *pDoc);
	db_handitem * GetNextValidHip (db_handitem *pDbHandItem);

public:
	// IEnumVARIANT
	STDMETHOD(Next)(ULONG celt, VARIANT * rgvar, ULONG * pceltFetched);
	STDMETHOD(Skip)(ULONG celt);
	STDMETHOD(Reset)();
	STDMETHOD(Clone)(IEnumVARIANT * * ppenum);
};

#endif //__AUTOMSPACE_H_


