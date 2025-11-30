/*	PARENTUNDOUNIT.H - Declaration of the CParentUndoUnit
 *	Copyright (C) 1998 Visio Corporation.  All rights reserved.
 */

#ifndef __PARENTUNDOUNIT_H_
#define __PARENTUNDOUNIT_H_

#include "Icadres.h"       // main symbols
#include <vector>

/////////////////////////////////////////////////////////////////////////////
// CParentUndoUnit
class ATL_NO_VTABLE CParentUndoUnit : 
	public CComObjectRoot,
	public CComCoClass<CParentUndoUnit, &CLSID_ParentUndoUnit>,
	public IOleParentUndoUnit
{
public:
	CParentUndoUnit()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_PARENTUNDOUNIT)

BEGIN_COM_MAP(CParentUndoUnit)
	COM_INTERFACE_ENTRY(IOleParentUndoUnit)
	COM_INTERFACE_ENTRY(IOleUndoUnit)	// IOleParentUndoUnit derives from this
END_COM_MAP()

// IOleUndoUnit
	STDMETHOD(Do)(/* [in] */ IOleUndoManager *pUndoManager);
	STDMETHOD(GetDescription)( /* [out] */ BSTR *pBstr);
	STDMETHOD(GetUnitType)( 
            /* [out] */ CLSID __RPC_FAR *pClsid,
            /* [out] */ LONG __RPC_FAR *plID);
	STDMETHOD(OnNextAdd)( void );

// IOleParentUndoUnit
	STDMETHOD(Open)( /* [in] */ IOleParentUndoUnit *pPUU);
	STDMETHOD(Close)( 
            /* [in] */ IOleParentUndoUnit *pPUU,
            /* [in] */ BOOL fCommit);
	STDMETHOD(Add)( /* [in] */ IOleUndoUnit *pUU);
	STDMETHOD(FindUnit)(/* [in] */ IOleUndoUnit *pUU);
    STDMETHOD(GetParentState)(/* [out] */ DWORD *pdwState);
	STDMETHOD(getLastUnit)(IOleUndoUnit** ppU);

public:
	typedef std::vector < CComPtr<IOleUndoUnit> >	container;	// container type

    const container& children() const { return m_children; }

protected:
	typedef container::iterator						iterator;	// iterator type
	typedef container::reverse_iterator				reverse_iterator;	// iterator type

	container					m_children;			// List of undo children
	CComPtr<IOleParentUndoUnit>	m_pOpenParent;		// Open undo parent (if any)
};

#endif //__PARENTUNDOUNIT_H_
