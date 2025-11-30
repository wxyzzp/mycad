/*	UNDOMGR.H - Declaration of the CUndoMgr class - Undo Manager.
 *	Copyright (C) 1998 Visio Corporation.  All rights reserved.
 */

#pragma warning (disable : 4786 4146)	/*D.G.*/

#ifndef _UNDOMGR_H_
#define _UNDOMGR_H_

#include "IcadRes.h"       // main symbols
#include "Undo.h"
#include <stack>


void writeWarningMessage(int n, ...);


class CUndoStack : public std::stack< CComPtr<IOleUndoUnit> >
	{
public:
	typedef std::deque< CComPtr<IOleUndoUnit> >	container;	// container type

	void clear() { c.clear(); }

	// Iterators begin at the back of the dequeue
	// and end at the beginning.   We traverse the stack in LIFO order.
	typedef container::reverse_iterator		iterator;	// iterator type

	iterator begin()
		{return c.end(); }
	iterator end()
		{return c.begin(); }

	iterator erase(iterator _F, iterator _L)
		{ return c.erase( _L.base(), _F.base() ); }

	};

	
/////////////////////////////////////////////////////////////////////////////
// CUndoMgr
class ATL_NO_VTABLE CUndoMgr : 
	public CComObjectRoot,
	public CComCoClass<CUndoMgr, &CLSID_UndoMgr>,
	public IOleUndoManager
	{
public:
	enum EStackCheckingType
	{
		eCmdEnd	// check if the last stacked unit is a command one and if command name in brackets is the same
	};

	CUndoMgr();

// IOleUndoManager
	STDMETHOD(Open)(/* [in] */ IOleParentUndoUnit *pPUU);
	STDMETHOD(Close)( 
		/* [in] */ IOleParentUndoUnit *pPUU,
		/* [in] */ BOOL fCommit);
	STDMETHOD(Add)( /* [in] */ IOleUndoUnit *pUU);
	STDMETHOD(GetOpenParentState)( /* [out] */ DWORD *pdwState);
	STDMETHOD(DiscardFrom)(/* [in] */ IOleUndoUnit *pUU);
	STDMETHOD(UndoTo)(/* [in] */ IOleUndoUnit *pUU);
	STDMETHOD(RedoTo)(/* [in] */ IOleUndoUnit *pUU);
	STDMETHOD(EnumUndoable)(/* [out] */ IEnumOleUndoUnits **ppEnum);
	STDMETHOD(EnumRedoable)(/* [out] */ IEnumOleUndoUnits **ppEnum);
	STDMETHOD(GetLastUndoDescription)(/* [out] */ BSTR *pBstr);
	STDMETHOD(GetLastRedoDescription)(/* [out] */ BSTR *pBstr);
	STDMETHOD(Enable)(/* [in] */ BOOL fEnable);
	STDMETHOD(GroupUnits)();
	STDMETHOD(getLastUndoable)(IOleUndoUnit** ppUU);
	STDMETHOD(checkStacks)(EStackCheckingType type, void* arg = NULL);

DECLARE_REGISTRY_RESOURCEID(IDR_UNDOMGR)

BEGIN_COM_MAP(CUndoMgr)
	COM_INTERFACE_ENTRY(IOleUndoManager)
END_COM_MAP()

	/*D.G.*/
	void	setGroupLevel(int newValue)	{ m_UserGroupLevel = newValue; }
	int		getGroupLevel()				{ return m_UserGroupLevel; }
	void	setAutoSave(bool autoSave)	{ m_bAutoSave = autoSave; }
	bool	getAutoSave()				{ return m_bAutoSave; }


protected:
	STDMETHOD(CreateDefaultParent)(/* [out] */ IOleParentUndoUnit **ppPUU);
	STDMETHOD(ProcessUndoUnits)( 
		CUndoStack *pDoStack,			//	Call Do on units in this stack
		CUndoStack *pRollbackStack,		//	Rollback units placed here (NULL means no rollback)
		IOleUndoUnit *pUU );			//	Process units up to and including this
										//		one in pDoStack (Can be NULL - Do all)

	BOOL IsDisabled() { return m_state == disabled; }
	
	enum { base, undo, redo, disabled } m_state;

	CUndoStack m_undoStack;
	CUndoStack m_redoStack;

	CComPtr<IOleParentUndoUnit>	m_pOpenParent;
	int							m_nOpenParentLevel;
	int							m_UserGroupLevel;	// Count of inserted IC_UNDO_GROUP_BEGIN units which have not yet closed by IC_UNDO_GROUP_END.
	bool						m_bAutoSave;		// 'true' if we should save the drawing before performing undo/redo on it
};


class ATL_NO_VTABLE CEnumOleUndoUnits : 
	public CComObjectRoot,
	public IEnumOleUndoUnits
	{
public:
BEGIN_COM_MAP(CEnumOleUndoUnits)
	COM_INTERFACE_ENTRY(IEnumOleUndoUnits)
END_COM_MAP()

protected:
	CEnumOleUndoUnits();
	HRESULT Init( CUndoStack *pUndoStack, CUndoStack::iterator begin, 
		CUndoStack::iterator end, CUndoStack::iterator iter );
	
public:
	static HRESULT Create(CUndoStack* pUndoStack, IEnumOleUndoUnits** ppEnum);

// IEnumOleUndoUnit
	STDMETHOD(Next)( 
		/* [in] */ ULONG cElt,
		/* [length_is][size_is][out] */ IOleUndoUnit  **rgElt,
		/* [out] */ ULONG *pcEltFetched);
        
	STDMETHOD(Skip)( /* [in] */ ULONG cElt);
        
	STDMETHOD(Reset)( void);
        
	STDMETHOD(Clone)( 
		/* [out] */ IEnumOleUndoUnits  **ppEnum);


private:
	CUndoStack *m_pUndoStack;
	CUndoStack::iterator m_iter;
	CUndoStack::iterator m_begin;
	CUndoStack::iterator m_end;
	};



#endif //__UNDOMGR_H_
