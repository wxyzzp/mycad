/* INTELLICAD\PRJ\LIB\AUTO\AUTOPSPACE.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Declaration of the CAutoPSpace - The collection of all Paper space entities.
 *
 */ 

#ifndef __AUTOPSPACE_H_
#define __AUTOPSPACE_H_


#include "IcadDoc.h"
#include "AutoBaseDefs.h"
#include "AutoEntities.h"
class db_handitem;

/////////////////////////////////////////////////////////////////////////////
// CAutoPSpace
class ATL_NO_VTABLE CAutoPSpace : 
	public CAutoEntities,
	public CComCoClass<CAutoPSpace, &CLSID_PaperSpace>,
	public IDispatchImpl<IIcadPaperSpace, &IID_IIcadPaperSpace, &LIBID_IntelliCAD>
{
protected:
	CIcadDoc *m_pDoc;

public:
	CAutoPSpace()
		{
		m_pDoc = NULL;
		}
	~CAutoPSpace()
		{
		if (m_pDoc)
			m_pDoc = NULL;
		}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoPSpace)
		COM_INTERFACE_ENTRY(IIcadPaperSpace)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadPaperSpace)
		COM_INTERFACE_ENTRY_CHAIN(CAutoEntities)
	END_COM_MAP()

	HRESULT Init (CIcadDoc *pDoc);
	HRESULT AddItemToDrawing (db_handitem *pDbHandItem, BOOL complex = FALSE);
	CIcadDoc * GetDocument ();	//return the document in which this collection exists
	HRESULT GetOwner (IDispatch **ppOwner); //return a pointer to the owner - PSpace
	virtual BOOL OwnerIsModelSpace() {return FALSE;};
	virtual BOOL OwnerIsPaperSpace() {return TRUE;};
	virtual BOOL OwnerIsBlock() {return FALSE;};

public:
	//  IIcadEntities
	DEFINE_IICADENTITIES_METHODS

	// IIcadPaperSpace
	STDMETHOD(AddPViewport)(IIcadPoint * Center, double Width, double Height, IIcadPViewport * * ppViewport);
};

/////////////////////////////////////////////////////////////////////////////
// CAutoEnumPSpaceEnts
class ATL_NO_VTABLE CAutoEnumPSpaceEnts : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IEnumVARIANT, &IID_IEnumVARIANT, &LIBID_IntelliCAD>
{
protected:
	long		m_nElements;
	long		m_nCurrent;
	db_handitem *m_hipCurrent;
	CIcadDoc	*m_pDoc;
	CAutoPSpace *m_pPSpace;

public:
	CAutoEnumPSpaceEnts()
		{
		}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoEnumPSpaceEnts)
		COM_INTERFACE_ENTRY(IEnumVARIANT)
		COM_INTERFACE_ENTRY2(IDispatch, IEnumVARIANT)
	END_COM_MAP()

	HRESULT Init (CAutoPSpace *pPSpace, CIcadDoc *pDoc);
	db_handitem * GetNextValidHip (db_handitem *pDbHandItem);

public:
	// IEnumVARIANT
	STDMETHOD(Next)(ULONG celt, VARIANT * rgvar, ULONG * pceltFetched);
	STDMETHOD(Skip)(ULONG celt);
	STDMETHOD(Reset)();
	STDMETHOD(Clone)(IEnumVARIANT * * ppenum);
};


#endif //__AUTOPSPACE_H_


