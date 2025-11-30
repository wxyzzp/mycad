/*	PARENTUNDOUNIT.CPP - Implementation of CParentUndoUnit
 *	Copyright (C) 1998 Visio Corporation.  All rights reserved.
 */

#pragma warning (disable : 4786 4146)	/*D.G.*/

#include "stdafx.h"
#include "Undo.h"
#include "ParentUndoUnit.h"

/////////////////////////////////////////////////////////////////////////////
// 
//		CParentUndoUnit
//
/////////////////////////////////////////////////////////////////////////////

/***************************************************************************** 
 *+	CParentUndoUnit::Do() - Process undo units
 *
 *	Abstract:
 *		Instructs the undo unit to carry out its action. Note that if it contains 
 *		child undo units, it must call their Do methods as well. 
 */
STDMETHODIMP CParentUndoUnit::Do(/* [in] */ IOleUndoManager *pUndoManager)
	{
	// The undo unit is responsible for carrying out its action. 
	// Performing its own undo action results in another action that can 
	// potentially be reversed. However, if pUndoManager is NULL, the undo unit 
	// should perform its undo action but should not attempt to put anything on 
	// the redo or undo stack. 
	// If pUndoManager is not NULL, then the unit is required to put a 
	// corresponding unit on the redo or undo stack. As a result, this method 
	// either moves itself to the redo or undo stack, or it creates a new undo 
	// unit and adds it to the appropriate stack. After creating a new undo unit, 
	// this undo unit calls IOleUndoManager::Open or IOleUndoManager::Add. 
	// The undo manager will put the new undo unit on the undo or redo stack 
	// depending on its current state. 

	// A parent unit must pass to its children the same undo manager, possibly NULL, 
	// that was given to the parent. It is permissible, but not necessary, when 
	// pUndoManager is NULL to open a parent unit on the redo or undo stack as 
	// long as it is not committed. A blocked parent unit ensures that nothing is 
	// added to the stack by child units. 

	// If this undo unit is a parent unit, it should put itself on the redo or 
	// undo stack before calling the Do on its children. 

	// After calling this method, the undo manager must release the undo unit. 

	HRESULT hr = S_OK;

	if (pUndoManager)
		{
		hr = pUndoManager->Open( NULL );	// New simple parent gets created
		if (FAILED(hr))
			return hr;
		}

	// Walk the children in reverse order calling their Do's
	reverse_iterator iter = m_children.rbegin();
	reverse_iterator end = m_children.rend();

	for(; iter != end; iter++ )
		{
		hr = (*iter)->Do( pUndoManager);
		if (FAILED(hr))
			{
			break;
			}
		}
	
	// We commit our changes.. In a failure case, the UndoMgr will rollback using
	// any undo records created inside the bracket we just closed.
	if (pUndoManager)
		{
		pUndoManager->Close(NULL, TRUE);
		}

	return hr;
	}
        
/**************************************************************************
 *+ CParentUndoUnit::GetDescription() - Get undo unit description.
 *
 *	Abstract:
 *		Returns a string that describes the undo unit and can be used in 
 *		the undo or redo user interface.
 *
 *	Return Value:
 *		S_OK 
 *			The string was successfully returned. 
 */

STDMETHODIMP CParentUndoUnit::GetDescription( /* [out] */ BSTR *pBstr)
	{
	*pBstr = NULL;
	/*DG - 19.6.2003*/// now it gets description of the 1st child
	if(!m_children.empty())
		m_children.front()->GetDescription(pBstr);
	return S_OK;
	}
        
/**************************************************************************
 *+ CParentUndoUnit::GetUnitType() - Get type information.
 *
 *	Abstract:
 *		Returns the CLSID and a type identifier for the undo unit.
 *
 *	Return Value:
 *		S_OK 
 *			Both the CLSID and type identifier were successfully returned
 */

STDMETHODIMP CParentUndoUnit::GetUnitType( 
            /* [out] */ CLSID *pClsid,
            /* [out] */ long  *plID)
{
	*pClsid = __uuidof(ParentUndoUnit);
	/*DG - 19.6.2003*/// now it gets unit type of the 1st child, unless it's a parent one
	if(!m_children.empty())
	{
		CLSID	classId;
		long	type = -1;
		m_children.front()->GetUnitType(&classId, &type);
		if(classId != __uuidof(ParentUndoUnit))
			*plID = type;
	}
	return S_OK;
}
        
/**************************************************************************
 *+ CParentUndoUnit::OnNextAdd() - Notify that new unit has been added.
 *
 *	Abstract:
 *		Notifies the last undo unit in the collection that a new unit has been added.
 *
 *	Return Value:
 *		Return value description goes here.
 */

STDMETHODIMP CParentUndoUnit::OnNextAdd(  )
	{
	// Note that parent units merely delegate this method to their most 
	// recently added child unit. A parent unit should terminate communication 
	// through any private interfaces when it is closed. A parent unit knows it 
	// is being closed when it receives S_FALSE from calling IOleParentUndoUnit::Close. 
	if (!m_children.empty())
		{
		m_children.back()->OnNextAdd();
		}
	return S_OK;
	}

/**************************************************************************
 *+ CParentUndoUnit::Open() - Open a new parent undo unit
 *
 *	Abstract:
 *		Opens a new parent undo unit, which becomes part of the containing 
 *		unit's undo stack.
 *
 */

STDMETHODIMP CParentUndoUnit::Open( /* [in] */ IOleParentUndoUnit *pPUU)
	{
	HRESULT hr = S_OK;

	// Delegate to the currently open parent undo unit
	if (m_pOpenParent)
		{
		hr = m_pOpenParent->Open(pPUU);
		}
	else
		{
		// Otherwise a new undo bracket is open for use
		m_pOpenParent = pPUU;
		}

	return hr;
	}
        
/**************************************************************************
 *+ CParentUndoUnit::Close() - Close open parent unit.
 *
 *	Abstract:
 *		Closes the specified parent undo unit.
 *
 *	Return Value:
 *		S_OK 
 *			The parent unit had open child units and it was successfully closed. 
 *		S_FALSE 
 *			The parent undo unit did not have an open child and it was successfully closed. 
 */

STDMETHODIMP CParentUndoUnit::Close( 
            /* [in] */ IOleParentUndoUnit *pPUU,
            /* [in] */ BOOL fCommit)
	{
	// To process a Close, a parent undo unit first checks to see if it 
	// has an open child unit. If it does not, it returns S_FALSE. 
	if (!m_pOpenParent)
		{
		return S_FALSE;
		}

	// If it does have a child unit open, it calls the Close method 
	// on the child. If the child returns S_FALSE, then the parent undo 
	// unit verifies that pPUU points to the child unit, and closes that 
	// child undo unit. If the child returns S_OK then it handled the Close 
	// internally and its parent should do nothing. 
	HRESULT hr = m_pOpenParent->Close( pPUU, fCommit );
	if (hr == S_FALSE /* && pPUU == m_pOpenParent */)
		{
		CComPtr<IOleUndoUnit> pUnit = m_pOpenParent;
		m_pOpenParent = NULL;

		if (fCommit)
			hr=this->Add(pUnit);
		else
			hr=pUnit->Do(NULL);
		}
	else if (hr == S_OK)
		{
		}

	// If the parent unit is blocked, it should check the pPUU parameter 
	// to determine the appropriate return code. If pPUU is pointing to 
	// itself, then it should return S_FALSE. 

	// Otherwise, it should return S_OK; the fCommit parameter is ignored; 
	// and no action is taken. 

	// If pPUU does not match the currently open parent undo unit, then 
	// implementations of this method should return E_INVALIDARG without changing 
	// any internal state. The only exception to this is if the unit is blocked. 


	return S_OK;
	}
        
/**************************************************************************
 *+ CParentUndoUnit::Add() - Adds a simple undo unit to the collection.
 *
 */
STDMETHODIMP CParentUndoUnit::Add( /* [in] */ IOleUndoUnit *pUU)
	{
	HRESULT hr = S_OK;
	if (m_pOpenParent)
		{
		hr = m_pOpenParent->Add(pUU);
		}
	else
		{
		if (!m_children.empty())
			{
			m_children.back()->OnNextAdd();
			}
		m_children.push_back( pUU );
		}
	return hr;
	}
        
/**************************************************************************
 *+ CParentUndoUnit::FindUnit() - Is a specific unit in the tree?
 *
 *	Abstract:
 *		Indicates if the specified unit is a child of this undo unit or 
 *		one of its children, that is if the specified unit is part of the 
 *		hierarchy in this parent unit.
 *
 *	Return Value:
 *		S_OK 
 *			The specified undo unit is in the hierarchy subordinate to this parent. 
 *		S_FALSE 
 *			The specified undo unit is not part of the hierarchy under this parent. 
 *			An error indicates an RPC failure condition. 
 */

STDMETHODIMP CParentUndoUnit::FindUnit(/* [in] */ IOleUndoUnit *pUU)
	{
	// Look thru local list of children first.
	iterator iter = m_children.begin();
	iterator end = m_children.end();

	for(iter = m_children.begin(); iter != end; iter++)
		{
		IOleUndoUnit *pCompare = (IOleUndoUnit *)(*iter);
		if (pCompare == pUU)
			{
			return S_OK;
			}
		}


	// Then delegate to all child objects that are containers
	for( iter = m_children.begin(); iter != end; iter++)
		{
		CComQIPtr <IOleParentUndoUnit, &IID_IOleParentUndoUnit> iContainer( *iter);
		if (iContainer)
			{
			if (iContainer->FindUnit(pUU) == S_OK)
				{
				return S_OK;
				}
			}
		}

	return S_FALSE;
	}
        
/**************************************************************************
 *+ CParentUndoUnit::GetParentState() - Return state info
 *
 *	Abstract:
 *		Returns state information about the innermost open parent undo unit.
 *
 *	Return Value:
 *		Return value description goes here.
 */

STDMETHODIMP CParentUndoUnit::GetParentState(/* [out] */ DWORD *pdwState)
	{
	return E_NOTIMPL;
	}

/*-------------------------------------------------------------------------*//**
Finds the last (top) undo unit.

@author	Dmitry Gavrilov
@param	ppU	<= undo unit found or NULL
@return	S_OK if a unit was found succesfully,
		S_FALSE if there is no units to undo at all,
		E_FAIL for some other failure (stack corruption, etc.)
*//*--------------------------------------------------------------------------*/
STDMETHODIMP
CParentUndoUnit::getLastUnit(IOleUndoUnit** ppU)
{
	*ppU = NULL;

	if(m_pOpenParent)
		return static_cast<CParentUndoUnit*>(&*m_pOpenParent)->getLastUnit(ppU);
	else
	{
		if(m_children.empty())
			return S_FALSE;
		else
		{
			*ppU = m_children.back();
			return S_OK;
		}
	}
}
