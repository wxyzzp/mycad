/*	UNDOMGR.CPP - Implementation of CUndoMgr - Undo Manager
 *	Copyright (C) 1998 Visio Corporation.  All rights reserved.
 */

#pragma warning (disable : 4786 4146)	/*D.G.*/

#include "stdafx.h"
#include <algorithm>

#include "UndoMgr.h"
#include "ParentUndoUnit.h"
#include "icadmain.h"	// for CMainWindow

extern CMainWindow*	SDS_CMainWindow;	// for writeWarningMessage

#define ASSERT _ASSERT

/////////////////////////////////////////////////////////////////////////////
//		
//		CUndoMgr
//
/////////////////////////////////////////////////////////////////////////////

CUndoMgr::CUndoMgr() : m_state(base), m_nOpenParentLevel(0), m_UserGroupLevel(0), m_bAutoSave(false)
{
}

/**************************************************************************
 *+ CUndoMgr::Open() - Open an undo bracket/transaction
 *
 *	Abstract:
 *		Opens a new parent undo unit, which becomes part of its containing 
 *		unit's undo stack.
 *
 *		If a null parent undo unit is passed in, this method will auto-create
 *		a simple parent undo.   This is a deviation from the OLE undo spec.
 *		You can also pass in NULL to Close().
 *
 *	Return Value:
 *		S_OK 
 *			The parent undo unit was successfully opened or if a currently 
 *			open unit is blocked. If the undo manager is currently disabled, 
 *			it should return S_OK and do nothing else. 
 *
 */

STDMETHODIMP CUndoMgr::Open(/* [in] */ IOleParentUndoUnit *pPUU)
	{
	HRESULT hr = S_OK;

	// If the undomgr is disabled do nothing
	if (this->IsDisabled())
		{
		return S_OK;
		}

	CComPtr<IOleParentUndoUnit> pParent = pPUU;

	// If no undo parent was passed in, then we will create a default
	// one for the client.  This makes for simple, default implementation of
	// nested undo.
	if (!pParent)
		{
		hr = this->CreateDefaultParent(&pParent);
		if (FAILED(hr))
			{
			return hr;
			}
		}

	// Delegate to the currently open parent undo unit
	if (m_pOpenParent)
		{
		hr = m_pOpenParent->Open(pParent);
		}
	else
		{
		// Otherwise a new undo bracket is open for use
		m_pOpenParent = pParent;
		}

	// We keep track of how many levels deep we are open to in the undo hierarchy
	if (SUCCEEDED(hr))
		m_nOpenParentLevel++;

	return hr;
	}
        
/**************************************************************************
 *+ CUndoMgr::Close() - Close the currently open undo bracket/transaction.
 *
 *	Abstract:
 *		The currently open transaction is closed.  See Open().
 *
 *	Return Value:
 *		S_OK 
 *			The parent unit had open child units and it was successfully 
 *			closed. If the undo manager is disabled, it should immediately 
 *			return S_OK and do nothing else. 
 *		S_FALSE 
 *			The parent undo unit did not have an open child and it was successfully closed. 
 *
 */

STDMETHODIMP CUndoMgr::Close( 
		/* [in] */ IOleParentUndoUnit *pPUU,	// Can be NULL
		/* [in] */ BOOL fCommit)				// Commit == TRUE, Rollback ==FALSE
	{
	HRESULT hr = S_OK;

	// If the undomgr is disabled, we do nothing 
	if (this->IsDisabled())
		{
		return S_OK;
		}

	// NOTE:  The undo spec talks about returning S_FALSE from parent undo
	// units to signify that it itself is closing.  This may mean that we can
	// get rid of the hierarchy level count in the undomgr.  Time will tell..


	// We are now one more level up in the undo hierarchy
	m_nOpenParentLevel--;

	// If this was the top level parent undo unit
	if (!m_nOpenParentLevel)
		{
		ASSERT (pPUU == NULL || pPUU == m_pOpenParent);

		CComPtr<IOleParentUndoUnit> pParent = m_pOpenParent;
		m_pOpenParent = NULL;

		// If we are committing, then add to the appropriate stack
		if (fCommit)
			{
			hr = this->Add( pParent);
			// TODO:  if failure on add, then what?
			}
		// Otherwise, we rollback items in the undo unit
		else
			{
			pParent->Do(NULL);
			}
		}
	// Delegate to the currently open parent undo unit
	else  if (m_pOpenParent)
		{
		hr = m_pOpenParent->Close(pPUU, fCommit);
		// TODO: if failure on child close, then what?
		}

	return hr;
	}
        
/**************************************************************************
 *+ CUndoMgr::Add() - Adds an undo unit 
 *
 *	Abstract:
 *		Adds a simple undo unit to the collection.
 *
 *		If the undo manager is in the base state, it should put the new unit 
 *		on the undo stack and discard the entire redo stack. If the undo manager 
 *		is in the undo state, it should put new units on the redo stack. If the 
 *		undo manager is in the redo state, it should put units on the undo stack 
 *		without affecting the redo stack.
 *
 *	Return Value:
 *		S_OK 
 *			The specified unit was successfully added, the parent unit was 
 *			blocked, or the undo manager is disabled. 
 *
 */
STDMETHODIMP CUndoMgr::Add( /* [in] */ IOleUndoUnit *pUU)
	{
	HRESULT hr = S_OK;

	// If the undomgr is disabled, we do nadda
	if (this->IsDisabled())
		{
		return S_OK;
		}

	// Delegate to the open undo
	if (m_pOpenParent)
		{
		hr = m_pOpenParent->Add(pUU);
		}
	else
		{
		switch (m_state)
			{
			case base:
				m_undoStack.push(pUU);
				m_redoStack.clear();
				break;

			case undo:
				m_redoStack.push(pUU); 
				break;
			
			case redo: 
				m_undoStack.push(pUU); 
				break;

			default: 
				ASSERT(FALSE); 
				break;
			}
		}
	return hr;
	}
        
/**************************************************************************
 *+ CUndoMgr::GetOpenParentState() - Returns state information 
 *
 *	Abstract:
 *		Returns state information about the innermost open parent undo unit.
 *
 *	Return Value:
 *		S_OK 
 *			There was an open parent unit and its state was successfully 
 *			returned or the undo manager is disabled. 
 *		S_FALSE
 *			There is no open parent unit. 
 */

STDMETHODIMP CUndoMgr::GetOpenParentState( /* [out] */ DWORD *pdwState)
	{
	if (this->IsDisabled())
		{
		*pdwState = UAS_BLOCKED;
		return S_OK;
		}

	if (m_pOpenParent)
		{
		m_pOpenParent->GetParentState( pdwState );
		return S_OK;
		}
	return S_FALSE;
	}
        
/**************************************************************************
 *+ CUndoMgr::DiscardFrom() - Discard some units.
 *
 *	Abstract:
 *		Instructs the undo manager to discard the specified undo unit and 
 *		all undo units below it on the undo or redo stack.
 *
 *	Return Value:
 *		S_OK 
 *			The specified undo unit was successfully discarded. 
 *		E_INVALIDARG 
 *			The specified undo unit was not found in the stacks. 
 *		E_UNEXPECTED 
 *			The undo manager is disabled. 
 */

STDMETHODIMP CUndoMgr::DiscardFrom(/* [in] */ IOleUndoUnit *pUU)
	{
	if (this->IsDisabled())
		{
		return E_UNEXPECTED;
		}

	CUndoStack::iterator location ;
		
	// Find the unit in the undo stack
	for (location = m_undoStack.begin(); location != m_undoStack.end(); location++)
		{
		if (pUU == *location)
			break;
		}
    // matching element found
	if (location != m_undoStack.end())  
		{
		m_undoStack.erase( location, m_undoStack.end());
		return S_OK;
		}

	// Find the unit in the redo stack
	for (location = m_redoStack.begin(); location != m_redoStack.end(); location++)
		{
		if (pUU == *location)
			break;
		}

	// matching element found
	if (location != m_redoStack.end())  
		{
		m_redoStack.erase( location, m_redoStack.end());
		return S_OK;
		}

	return E_INVALIDARG;
	}
       
  
/**************************************************************************
 *+ CUndoMgr::UndoTo() - Undo some actions
 *
 *	Abstract:
 *		Instructs the undo manager to invoke undo actions back through the 
 *		undo stack, down to and including the specified undo unit.
 *
 *		If pUU is NULL, the most-recently added undo unit is processed.
 *
 *	Return Value:
 *		S_OK 
 *			The specified undo unit was successfully invoked to perform 
 *			its undo action. 
 *		E_INVALIDARG 
 *			The specified undo unit is not on the undo stack. 
 *		E_ABORT 
 *			Both the undo attempt and the rollback attempt failed. The undo 
 *			manager should never propagate the E_ABORT obtained from a 
 *			contained undo unit. Instead, it should map any E_ABORT values 
 *			returned from other undo units to E_FAIL. 
 *		E_UNEXPECTED 
 *			The undo manager is disabled. 
 *
 */
STDMETHODIMP CUndoMgr::UndoTo(
	/* [in] */ IOleUndoUnit *pUU	//Pointer to the top level unit to undo
	)
	{
	HRESULT hr = S_OK;
	m_state = undo;

	// If NULL is passed in, we process only the top unit
	if (!pUU && !m_undoStack.empty())
		{
		pUU = m_undoStack.top();
		}
	hr = this->ProcessUndoUnits( &m_undoStack, &m_redoStack, pUU );
	m_state = base;
	return S_OK;
	}
        
/**************************************************************************
 *+ CUndoMgr::RedoTo() - Redo some actions.
 *
 *	Abstract:
 *		Instructs the undo manager to invoke undo actions back through 
 *		the redo stack, down to and including the specified undo unit.
 *		
 *		If pUU is NULL, the most-recently added redo unit is processed.
 *
 *	Return Value:
 *		S_OK 
 *			The specified undo unit was successfully invoked to perform its 
 *			undo action. 
 *		E_INVALIDARG 
 *			The specified undo unit is not on the redo stack. 
 *		E_ABORT 
 *			Both the undo attempt and the rollback attempt failed. The undo 
 *			manager should never propagate the E_ABORT obtained from a contained 
 *			undo unit. Instead, it should map any E_ABORT values returned from 
 *			other undo units to E_FAIL. 
 *		E_UNEXPECTED 
 *			The undo manager is disabled. 
 */
STDMETHODIMP CUndoMgr::RedoTo(/* [in] */ IOleUndoUnit *pUU)
	{
	HRESULT hr = S_OK;
	m_state = redo;
	// If NULL is passed in, we process only the top unit
	if (!pUU && !m_redoStack.empty())
		{
		pUU = m_redoStack.top();
		}
	hr = this->ProcessUndoUnits( &m_redoStack, &m_undoStack, pUU );
	m_state = base;
	return S_OK;
	}
        
/**************************************************************************
 *+ CUndoMgr::EnumUndoable() - Create an enumerator over undo units
 *
 *	Abstract:
 *		Abstract text goes here.
 *
 *	Return Value:
 *		S_OK 
 *			The enumerator object was successfully created and the interface 
 *			pointer was returned. 
 *		E_UNEXPECTED 
 *			The undo manager is disabled. 
 */

STDMETHODIMP CUndoMgr::EnumUndoable(/* [out] */ IEnumOleUndoUnits **ppEnum)
	{
	HRESULT hr;
	
	// If the undomgr is disabled do nothing
	if (this->IsDisabled())
		{
		return E_UNEXPECTED;
		}


	hr = CEnumOleUndoUnits::Create(&m_undoStack, ppEnum);

	return hr;

	}
        
/**************************************************************************
 *+ CUndoMgr::EnumRedoable() - Create an enumerator over redo units
 *
 *	Abstract:
 *		Abstract text goes here.
 *
 *	Return Value:
 *		S_OK 
 *			The enumerator object was successfully created and the interface 
 *			pointer was returned. 
 *		E_UNEXPECTED 
 *			The undo manager is disabled. 
 */

STDMETHODIMP CUndoMgr::EnumRedoable(/* [out] */ IEnumOleUndoUnits **ppEnum)
	{
	HRESULT hr;

	// If the undomgr is disabled do nothing
	if (this->IsDisabled())
		{
		return E_UNEXPECTED;
		}


	hr = CEnumOleUndoUnits::Create(&m_redoStack, ppEnum);

	return hr;	
	}
        
/**************************************************************************
 *+ CUndoMgr::GetLastUndoDescription() - Get Undo unit description
 *
 *	Abstract:
 *		Returns the description for the top-level undo unit that is on 
 *		top of the undo stack.
 *
 *	Return Value:
 *		S_OK 
 *			The string was successfully returned and it contains a valid 
 *			description. 
 *		E_FAIL 
 *			The undo stack is empty. 
 *		E_UNEXPECTED 
 *			The undo manager is disabled. 
 */

STDMETHODIMP CUndoMgr::GetLastUndoDescription(/* [out] */ BSTR *pBstr)
	{
	// If the undomgr is disabled do nothing
	if (this->IsDisabled())
		{
		return E_UNEXPECTED;
		}

	if (m_undoStack.empty())
		{
		return E_FAIL;
		}

	HRESULT hr = m_undoStack.top()->GetDescription( pBstr );
	return hr;
	}
        
/**************************************************************************
 *+ CUndoMgr::GetLastRedoDescription() - Get Redo unit description
 *
 *	Abstract:
 *		Returns the description for the top-level undo unit that is on 
 *		top of the redo stack.
 *
 *	Return Value:
 *		S_OK 
 *			The string was successfully returned and it contains a valid 
 *			description. 
 *		E_FAIL 
 *			The redo stack is empty. 
 *		E_UNEXPECTED 
 *			The undo manager is disabled. 
 */
STDMETHODIMP CUndoMgr::GetLastRedoDescription(/* [out] */ BSTR *pBstr)
	{
	// If the undomgr is disabled do nothing
	if (this->IsDisabled())
		{
		return E_UNEXPECTED;
		}

	if (m_redoStack.empty())
		{
		return E_FAIL;
		}

	HRESULT hr = m_redoStack.top()->GetDescription( pBstr );
	return hr;
	}
        
/**************************************************************************
 *+ CUndoMgr::Enable() - Enables or disables the undo manager.
 *
 *	Notes:
 *		The undo manager should clear both stacks when making the transition 
 *		from enabled to disabled.
 *		If the undo manager is disabled, each method in IOleUndoManager must 
 *		behave as specified. See each method for details.
 *
 *	Return Value:
 *		S_OK 
 *			The undo manager was successfully enabled or disabled. 
 *		E_UNEXPECTED 
 *			There is an open undo unit on the stack or the undo manager is 
 *			currently performing an undo or a redo. 
 */

STDMETHODIMP CUndoMgr::Enable(/* [in] */ BOOL fEnable)
	{
	if (!fEnable)
		{
		// it is an error to disable the undomgr if it is in the
		// undo/redo states or if there is an open undo parent
		if (m_state == undo || m_state == redo || m_pOpenParent != NULL )
			{
			return E_UNEXPECTED;
			}
		// Transition to disabled state clears both undo and redo stack
		m_redoStack.clear();
		m_undoStack.clear();
		m_state = disabled;
		}
	else
		{
		m_state = base;
		}
	return S_OK;
	}

/**************************************************************************
 *+ CUndoMgr::CreateDefaultParent() - Create parent unit
 *
 *	Abstract:
 *		Create a default, simple parent undo unit.
 *
 */

STDMETHODIMP CUndoMgr::CreateDefaultParent(/* [out] */ IOleParentUndoUnit **ppPUU)
	{
	if (!ppPUU) return E_INVALIDARG;

	*ppPUU = NULL;
	CComObject<CParentUndoUnit> *pParent = NULL;

	HRESULT hr = CComObject<CParentUndoUnit>::CreateInstance(&pParent);
	if (SUCCEEDED(hr))
		{
		hr = pParent->QueryInterface(IID_IOleParentUndoUnit, (void **)ppPUU);
		}
	// TODO: if QI fails clean up
	return hr;
	}

/**************************************************************************
 *+ CUndoMgr::ProcessUndoUnits() - Central undo stack processor
 *
 *	Abstract:
 *		Calls Do on a undo stack with rollback on error.   UndoTo and
 *		RedoTo both use this method to deal with the nasties of processing
 *		the undo stacks and rolling back on error.   
 *
 */

STDMETHODIMP CUndoMgr::ProcessUndoUnits( 
	CUndoStack *pDoStack,			//	Call Do on units in this stack
	CUndoStack *pRollbackStack,		//	Rollback units placed here (NULL means no rollback)
	IOleUndoUnit *pUU )				//	Process units up to and including this
									//		one in pDoStack (Can be NULL - Do all)
	{
	ASSERT (pDoStack != NULL);

	HRESULT hr = NOERROR;

	// Record the top rollbackStack unit for rollback on errors.  We use this 
	// as a marker.  All units that get added to the rollback stack
	// are processed on error.
	CComPtr<IOleUndoUnit> rollbackMarker;
	if (pRollbackStack && !pRollbackStack->empty())
		{
		rollbackMarker = pRollbackStack->top();
		}	
	
	// Walk the Do stack and tell our kiddies to Do their stuff and stop after we reach
	// the unit specified by the caller
	CComPtr<IOleUndoUnit> unit;
	while (!pDoStack->empty() && (pUU == NULL || pUU != unit) )
		{
		// Pop off and Do the next unit
		unit = pDoStack->top();
		pDoStack->pop();
		hr = unit->Do(this);

		// If we fail then bail the loop - rollback occurs outside the loop
		if (FAILED(hr))
			break;
		}

	// If an error occurred, rollback to a consistent state by performing
	// a Do on any "new within the Do" rollback units.
	if (FAILED(hr) && pRollbackStack != NULL && !pRollbackStack->empty() 
			&& pRollbackStack->top() != rollbackMarker)
		{
			
		// Find the first unit that was added after the marker
		// Look thru local list of children first.
		CComPtr<IOleUndoUnit> rollbackUnit;
		CUndoStack::iterator iter = pRollbackStack->begin();
		CUndoStack::iterator end = pRollbackStack->end();
		for(; iter != end; ++iter)
			{
			if ( (IOleUndoUnit *)rollbackMarker == (*iter))
				{
				--iter;
				rollbackUnit = *iter;
				break;
				}
			}

		this->ProcessUndoUnits( pRollbackStack, NULL, rollbackUnit );
		}

	return hr;
	}

/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Create a parent undo unit, move last undo units from the undo stack
 *			which form undo group (from the last UndoBegin till the last UndoEnd),
 *			and push this parent unit to the stack.
 * Returns:	S_OK    - ok,
 *			S_FALSE - there are no undo groups on the stack,
 *			E_FAIL  - cannot create parent unit.
 ********************************************************************************/
STDMETHODIMP
CUndoMgr::GroupUnits()
{
	if(m_undoStack.empty())
		return S_OK;

	CComPtr<IOleParentUndoUnit> pParent = NULL;
	if(SUCCEEDED(CreateDefaultParent(&pParent)))
	{
		CLSID	classId;
		LONG	undoType = -1;
		CComPtr<IOleUndoUnit>	pUnit = m_undoStack.top();

		VERIFY(SUCCEEDED(pUnit->GetUnitType(&classId, &undoType)));
//		ASSERT(undoType == IC_UNDO_GROUP_END);	// TO DO: If we haven't grouping units on the stack then try to find them in opened parents.
		if(undoType != IC_UNDO_GROUP_END)
			return S_FALSE;

		CUndoStack::iterator	pBeginGroupItem = m_undoStack.begin(), pCurrentItem;
		for( ; pBeginGroupItem != m_undoStack.end(); ++pBeginGroupItem)
		{
			VERIFY(SUCCEEDED((*pBeginGroupItem)->GetUnitType(&classId, &undoType)));
			if(undoType == IC_UNDO_GROUP_BEGIN)
				break;
		}

		if(undoType == IC_UNDO_GROUP_BEGIN)
		{
			for(pCurrentItem = pBeginGroupItem; pCurrentItem != m_undoStack.begin(); --pCurrentItem)
				pParent->Add(*pCurrentItem);

			pParent->Add(*pCurrentItem);
			m_undoStack.erase(pCurrentItem, ++pBeginGroupItem);
		}
		else
			ASSERT(FALSE);	// we have IC_UNDO_GROUP_END but don't IC_UNDO_GROUP_BEGIN

		return Add(pParent);
	}

	return E_FAIL;
}


/*-------------------------------------------------------------------------*//**
Finds the last (top) undoable unit, which can be both on the undo stack and
as a child of an inner parent unit.

@author	Dmitry Gavrilov
@param	ppU	<= undo unit found or NULL
@return	S_OK if a unit was found succesfully,
		S_FALSE if there is no units to undo at all,
		E_FAIL for some other failure (stack corruption, etc.)
*//*--------------------------------------------------------------------------*/
STDMETHODIMP
CUndoMgr::getLastUndoable(IOleUndoUnit** ppU)
{
	*ppU = NULL;

	if(m_nOpenParentLevel)
		return static_cast<CParentUndoUnit*>(&*m_pOpenParent)->getLastUnit(ppU);
	else
	{
		if(m_undoStack.empty())
			return S_FALSE;
		else
		{
			*ppU = m_undoStack.top();
			return S_OK;
		}
	}
}


/*-------------------------------------------------------------------------*//**
Writes undo warning messages: in alpha/beta release and debug builds - to the
command line (using sds_printf) and in all builds to the log file (if it's opened).

@author	Dmitry Gavrilov
@param	n	=> message number
@param	...	=> optional additional params for messages
*//*--------------------------------------------------------------------------*/
void writeWarningMessage(int n, ...)
{
	static const char		*prefix = "UNDO mechanism warning (%d): ",
							*cr = "\n"/*DNT*/, *crlf = "\r\n"/*DNT*/, *sc = "; "/*DNT*/;
	static const char		*messages[] =
	{/*0*/ "redo stack is not empty after a command execution.",
	 /*1*/ "nothing to undo after a non-trivial command execution.",
	 /*2*/ "command_begin and command_end undo brackets have different names: \"%s\" and \"%s\".",
	 /*3*/ "the top undo unit has not the 'command' type after a command execution.",
	 /*4*/ "auto saving the drawing before the undo/redo action..."};

    const int	bufSize = 512;	// enough?
    char		msg[bufSize];

	sprintf(msg, prefix, n);
	va_list	args;
	va_start(args, n);
	if(_vsnprintf(msg + strlen(msg), bufSize - strlen(msg), messages[n], args) < 0)
	{
		ASSERT(0);	// enlarge bufSize?!
		return;
	}
	va_end(args);

#ifdef DEBUG	// maybe do this in alpha/beta release builds too (see VERSIONINFO_BUILDTYPE in blddate.h)
	sds_printf(cr);
	sds_printf(msg);
	sds_printf(cr);
#endif
	if(SDS_CMainWindow->m_fLogFile.m_hFile)
	{
		SDS_CMainWindow->m_fLogFile.Write(sc, 2);
		SDS_CMainWindow->m_fLogFile.Write(msg, strlen(msg));
		SDS_CMainWindow->m_fLogFile.Write(crlf, 2);
	}
}


/*-------------------------------------------------------------------------*//**
Performs various undo/redo stacks checks
(currently only one case - TODO).

@author	Dmitry Gavrilov
@param	type   => checking type
@param	arg    => optional additional argument:
					cast as char* - command name for eCmdEnd check
@return	S_OK for checking success,
		S_FALSE for checking failure,
		E_FAIL for some other failure (stack corruption, etc.)
*//*--------------------------------------------------------------------------*/
STDMETHODIMP
CUndoMgr::checkStacks(EStackCheckingType type, void* arg /*= NULL*/)
{
	HRESULT	hr = S_OK;

	switch(type)
	{
	case eCmdEnd :
		if(!m_redoStack.empty() && !m_nOpenParentLevel)
		{
			hr = S_FALSE;
			writeWarningMessage(0);
		}
		IOleUndoUnit*	pTopUnit;
		hr = getLastUndoable(&pTopUnit);
		if(FAILED(hr))
			return E_FAIL;
		if(hr == S_FALSE)
		{
			writeWarningMessage(1);
			return hr;
		}
		CLSID	classId;
		LONG	undoType = -1;
		if(FAILED(pTopUnit->GetUnitType(&classId, &undoType)))
			return E_FAIL;
		if(undoType == IC_UNDO_COMMAND_BEGIN)
		{
			BSTR	bstrDescription = NULL;
			if(FAILED(pTopUnit->GetDescription(&bstrDescription)))
				return E_FAIL;
			CString	cmdBegin(bstrDescription), cmdEnd((char*)arg);
			SysFreeString(bstrDescription);
			if(cmdBegin.CompareNoCase((LPCTSTR)cmdEnd))
			{
				hr = S_FALSE;
				writeWarningMessage(2, (LPCTSTR)cmdBegin, (LPCTSTR)cmdEnd);
			}
		}
		else
		{
			hr = S_FALSE;
			writeWarningMessage(3);
		}
	}	// switch(type)

	return hr;
}


//////////////////////////////////////////////////////////////////////////////
// 
//		CEnumOleUndoUnits
//
//////////////////////////////////////////////////////////////////////////////
/*static */ HRESULT CEnumOleUndoUnits::Create(
	CUndoStack* pUndoStack, 
	IEnumOleUndoUnits** ppEnum)
	{
	CComObject<CEnumOleUndoUnits> *pEnum = NULL;
	HRESULT hr = CComObject<CEnumOleUndoUnits>::CreateInstance(&pEnum);
	pEnum->Init(pUndoStack, pUndoStack->begin(), pUndoStack->end(), pUndoStack->begin());
	pEnum->QueryInterface(IID_IEnumOleUndoUnits, (void **)ppEnum);
	return S_OK;
	}

CEnumOleUndoUnits::CEnumOleUndoUnits()
	{
	m_pUndoStack = NULL;
	}

HRESULT CEnumOleUndoUnits::Init( 
	CUndoStack *pUndoStack,
	CUndoStack::iterator begin, 
	CUndoStack::iterator end,
	CUndoStack::iterator iter)
	{
	m_pUndoStack = pUndoStack;
	m_begin	= begin;
	m_end	= end;
	m_iter	= iter;
	return S_OK;
	}

//+CEnumOleUndoUnits::Next
//	Attempts to get the next celt items in the enumeration sequence,
//	and return them through the array pointed to by rgelt.
//
//@Parameters
//	celt			The number of elements to be returned. 
//	rgelt			An array of at least size celt in which the elements are to be returned. 
//	pceltFetched	Pointer to the number of elements returned in rgelt, or NULL. 
//
//@Return Value
//	The return value obtained from the returned HRESULT is one of the following: 
//
//	Return value	Meaning 
//	------------	-------
//	S_OK			The number of elements returned is celt. 
//	S_FALSE			The number of elements returned is less than celt. 
//
//@Comments
//	If fewer than the requested number of elements remain in the sequence,
//	Next returns only the remaining elements.
//	The actual number of elements returned is passed through *pceltFetched,
//	unless it is NULL. 
//
STDMETHODIMP CEnumOleUndoUnits::Next( 
		/* [in] */ ULONG cElt,
		/* [length_is][size_is][out] */ IOleUndoUnit  **rgElt,
		/* [out] */ ULONG *pcEltFetched)
	{
	ULONG i;

	if (rgElt == NULL || (cElt != 1 && pcEltFetched == NULL))
		return E_POINTER;
	
	//if (m_begin == NULL || m_end == NULL || m_iter == NULL)
	//	return E_FAIL;

	// get each element and place it into the return array
	for (i = 0; i < cElt && m_iter != m_end; ++i, ++m_iter)
		{
		IOleUndoUnit *pUnit = NULL;
		pUnit = (*m_iter);
		pUnit->AddRef();
		rgElt[i] = pUnit;
		}

	// return the number of elements fetched
	if (pcEltFetched)
		{
		*pcEltFetched = i;
		}

	// If we're returning less than they asked for return S_FALSE
	// (S_FALSE is a success code).
	if (i < cElt)
		{
		return S_FALSE;	// we're returning less than they asked for
		}
	else
		{
		return S_OK;	// we're returning what they asked for
		}	
	}
        
//+CEnumOleUndoUnits::Skip
//	Attempts to skip over the next celt elements in the enumeration sequence. 
//
//@Parameter
//	celt			The number of elements to skip. 
//
//@Return Value
//	The return value obtained from the returned HRESULT is one of the following: 
//
//	Return value	Meaning
//	------------	-------
//	S_OK			The specified number of elements was skipped. 
//	S_FALSE			The end of the sequence was reached before skipping the requested number of elements. 
//
STDMETHODIMP CEnumOleUndoUnits::Skip( /* [in] */ ULONG cElt)
	{
	ULONG i;

	for (i = 0; i < cElt && m_iter != m_end; ++i, ++m_iter)
		{
		/* do nothing */
		}

	if (i < cElt)
		{
		return S_OK;	// The specified number of elements was skipped.
		}
	else
		{
		return S_FALSE;	// The end of the sequence was reached before skipping the requested number of elements.
		}
	}
        
//+CEnumOleUndoUnits::Reset
//	Resets the enumeration sequence to the beginning. 
//
//@Parameters
//	None 
//
//@Return Value
//	The return value obtained from the returned HRESULT is one of the following: 
//
//	Return value	Meaning 
//	------------	-------
//	S_OK			Success. 
//	S_FALSE			Failure. 
//
//@Comments
//	There is no guarantee that exactly the same set of variants will be enumerated
//	the second time as was enumerated the first time. Although an exact duplicate
//	is desirable, the outcome depends on the collection being enumerated.
//	You may find that it is impractical for some collections to maintain this condition
//	(for example, an enumeration of the files in a directory).
STDMETHODIMP CEnumOleUndoUnits::Reset(void)
	{
	m_iter = m_begin;
	return S_OK;
	}
        
//+CVectorEnum::Clone
//	Creates a copy of the current state of enumeration. 
//
//@Parameter
//	ppEnum			On return, pointer to the location of the clone enumerator. 
//
//@Return Value
//	The return value obtained from the returned HRESULT is one of the following: 
//
//	Return value	Meaning 
//	------------	-------
//	S_OK			Success. 
//	E_OUTOFMEMORY	Insufficient memory to complete the operation. 
//
//@Comments
//	Using this function, a particular point in the enumeration sequence can be
//	recorded, and then returned to at a later time. The returned enumerator is
//	of the same actual interface as the one that is being cloned. 
//
//	There is no guarantee that exactly the same set of variants will be enumerated
//	the second time as was enumerated the first. Although an exact duplicate is
//	desirable, the outcome depends on the collection being enumerated. You may
//	find that it is impractical for some collections to maintain this condition
//	(for example, an enumeration of the files in a directory). 
//
STDMETHODIMP CEnumOleUndoUnits::Clone( 
		/* [out] */ IEnumOleUndoUnits **ppEnum)
	{
	CComObject<CEnumOleUndoUnits> *pEnum = NULL;
	HRESULT hr = CComObject<CEnumOleUndoUnits>::CreateInstance(&pEnum);
	pEnum->Init(m_pUndoStack, m_pUndoStack->begin(), m_pUndoStack->end(), m_pUndoStack->begin());
	pEnum->QueryInterface(IID_IEnumOleUndoUnits, (void **)ppEnum);
	return S_OK;
	}
