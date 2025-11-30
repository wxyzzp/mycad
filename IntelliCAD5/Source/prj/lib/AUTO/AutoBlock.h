/* INTELLICAD\PRJ\LIB\AUTO\AUTOBLOCK.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Declaration of CAutoBlock - the Automation Block object
 * 
 */ 

#ifndef __AUTOBLOCK_H_
#define __AUTOBLOCK_H_

#include "AutoTableItem.h"
#include "AutoBaseDefs.h"
#include "AutoEntities.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoBlock
class ATL_NO_VTABLE CAutoBlock : 
	public CAutoEntities,
	public CComCoClass<CAutoBlock, &CLSID_Block>,
	public IDispatchImpl<IIcadBlock, &IID_IIcadBlock, &LIBID_IntelliCAD>
{
public:
	CIcadDoc *m_pDoc;
	CComPtr<IDispatch> m_pParent;
	db_handitem *m_pDbHandItem;

public:
	CAutoBlock()
		{
		m_pDoc = NULL;
		m_pParent = NULL;
		m_pDbHandItem = NULL;
		}

	~CAutoBlock()
		{
		if (m_pParent)
			m_pParent = NULL;
		}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoBlock)
		COM_INTERFACE_ENTRY(IIcadBlock)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadBlock)
		COM_INTERFACE_ENTRY_CHAIN(CAutoEntities)
	END_COM_MAP()

	virtual HRESULT Init (CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem);
	HRESULT AddItemToDrawing (db_handitem *pDbHandItem, BOOL complex = FALSE);
	CIcadDoc * GetDocument ();	//return the document in which this collection exists
	HRESULT GetOwner (IDispatch **ppOwner); //return a pointer to the owner
	BOOL OwnerIsModelSpace() {return FALSE;};
	BOOL OwnerIsPaperSpace() {return FALSE;};
	BOOL OwnerIsBlock() {return TRUE;};

public:
	// IIcadEntities
	DEFINE_IICADENTITIES_METHODS

	// IIcadBlock
	STDMETHOD(get_Origin)(IIcadPoint * * ppOrigin);
	STDMETHOD(put_Origin)(IIcadPoint * ppOrigin);
	STDMETHOD(get_Name)(BSTR * pstrName);
	STDMETHOD(put_Name)(BSTR pstrName);
	STDMETHOD(get_IsXRef)(VARIANT_BOOL * IsXRef);
	STDMETHOD(AddItems)(IIcadSelectionSet * Entities, VARIANT_BOOL DeleteOriginals);
	STDMETHOD(GetXData)(BSTR AppName, VARIANT * TypeArray, VARIANT * DataArray);
	STDMETHOD(SetXData)(VARIANT TypeArray, VARIANT DataArray);
	STDMETHOD(Delete)();
	STDMETHOD(Bind)();
	STDMETHOD(Detach)();
	STDMETHOD(Unload)();
	STDMETHOD(Reload)();
	STDMETHOD(WBlock)(BSTR FileName);
};


/////////////////////////////////////////////////////////////////////////////
// CAutoEnumBlockEnts

class ATL_NO_VTABLE CAutoEnumBlockEnts : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IEnumVARIANT, &IID_IEnumVARIANT, &LIBID_IntelliCAD>
{
protected:
	long		m_nElements;
	long		m_nCurrent;
	db_handitem *m_hipCurrent;
	db_block	*m_pBlockHip;
	CAutoBlock	*m_pBlock;

public:
	CAutoEnumBlockEnts()
		{
		}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoEnumBlockEnts)
		COM_INTERFACE_ENTRY(IEnumVARIANT)
		COM_INTERFACE_ENTRY2(IDispatch, IEnumVARIANT)
	END_COM_MAP()

	HRESULT Init (CAutoBlock *pBlock, db_block *pBlockHip);
	db_handitem * GetNextValidHip (db_handitem *pDbHandItem);


public:
	// IEnumVARIANT
	STDMETHOD(Next)(ULONG celt, VARIANT * rgvar, ULONG * pceltFetched);
	STDMETHOD(Skip)(ULONG celt);
	STDMETHOD(Reset)();
	STDMETHOD(Clone)(IEnumVARIANT * * ppenum);
};


#endif //__AUTOBLOCK_H_


