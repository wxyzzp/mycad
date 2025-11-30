/* INTELLICAD\PRJ\LIB\AUTO\AUTOMSPACE.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Implementation of CAutoMSpace - the Automation Model Space collection object
 * Implementation of the Model space enumerator
 * 
 */ 

#include "StdAfx.h"
#include "AutoMSpace.h"
#include "AutoHelpers.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoMSpace

HRESULT CAutoMSpace::Init(CIcadDoc *pDoc)
	{
	if (!m_pDoc)
		{
		m_pDoc = pDoc;
		}
	return S_OK;
	}

HRESULT CAutoMSpace::AddItemToDrawing (db_handitem *pDbHandItem, BOOL complex)
	{
	ASSERT (m_pDoc);
	
	//ensure the entity is in model space - TODO

	pDbHandItem->set_67 (FALSE);
	
	/*DG 30.10.2001*/// Call setUndo below with correct 2nd parameter (pLastItem).
	db_handitem*	pLastItem;
	/*DG - 24.1.2002*/// Should use different llist for objects (i.e. set pLastItem to the last object).
	if(db_is_objecttype(pDbHandItem->ret_type()))
		m_pDoc->m_dbDrawing->get_objllends(NULL, &pLastItem);
	else
		m_pDoc->m_dbDrawing->get_entllends(NULL, &pLastItem);

	//now add it to the drawing
	db_handitem *hip, *savehip;
	hip = pDbHandItem->next;
	if (m_pDoc->m_dbDrawing->addhanditem (pDbHandItem) == 0)
		{
		if (complex)
			{
			while ( hip != NULL)
				{
				savehip = hip->next;
				
				hip->set_67 (FALSE);

				if (m_pDoc->m_dbDrawing->addhanditem (hip) != 0)
					return E_FAIL;	//fail if any of the sub ents were not added
				hip = savehip;
				}
			}
		m_pDoc->SetModifiedFlag(TRUE);
		// AG: Set Undo
		m_pDoc->setUndo( IC_UNDO_NEW_ENT_TAB_CLS, (void*)pLastItem/*NULL*/, (void*)pDbHandItem, (void*)-1 );
		return NOERROR;
		}
	else 
		return E_FAIL;
	}

CIcadDoc * CAutoMSpace::GetDocument ()	//return the document in which this collection exists
	{
	return m_pDoc;
	}

HRESULT CAutoMSpace::GetOwner (IDispatch **ppOwner) //return a pointer to the owner - MSpace
	{
	return QueryInterface(IID_IDispatch, (void **)ppOwner);
	}

STDMETHODIMP CAutoMSpace::get_Application (IIcadApplication * * ppApp)
	{
	if (ppApp == NULL)
		return E_POINTER;
	
	return GetApp()->m_pAutoApp->QueryInterface (ppApp);
	}

STDMETHODIMP CAutoMSpace::get_Count (long * pCount)
	{
	if (pCount == NULL)
		return E_POINTER;
	
	ASSERT (m_pDoc);
	db_handitem* pDbHandItem = NULL;
	
	long lCount=0L;
	do
		{
		pDbHandItem=m_pDoc->m_dbDrawing->entnext_noxref(pDbHandItem);
		
		if (pDbHandItem)
			{
			lCount++;
			
			//Eliminate the entities that we dont support
			if ((UnsupportedEntity (pDbHandItem)) || (NotACountableEntity (pDbHandItem)) ||
				(PaperSpaceEntity (pDbHandItem))  || (UnimplementedEntity (pDbHandItem)))
				lCount--;
			}
		} while (pDbHandItem);
		
		*pCount = lCount;
		return NOERROR;
	}

STDMETHODIMP CAutoMSpace::get_Parent (IIcadDocument * * ppParent)
	{
	if (ppParent == NULL)
		return E_POINTER;
	
	ASSERT (m_pDoc);
	return m_pDoc->m_pAutoDoc->QueryInterface (ppParent);
	}

STDMETHODIMP CAutoMSpace::get__NewEnum (IUnknown * * ppUnk)
	{
	if (ppUnk == NULL)
		return E_POINTER;
	
	CComObject<CAutoEnumMSpaceEnts>* penum;
	CComObject<CAutoEnumMSpaceEnts>::CreateInstance(&penum);
	if (!penum)
        return E_FAIL;    
	
	penum->Init(this, m_pDoc);
	
	return penum->QueryInterface(IID_IUnknown, (void **)ppUnk);
	}

STDMETHODIMP CAutoMSpace::Item (long Index, IDispatch * * ppItem)
	{
	if (ppItem == NULL)
		return E_POINTER;
	
	if (Index < 1)
		return E_INVALIDARG;

	ASSERT (m_pDoc);
	db_handitem* pDbHandItem = NULL; 
	
	long lCount = 0L;
	do 
		{
		pDbHandItem = m_pDoc->m_dbDrawing->entnext_noxref(pDbHandItem);
		if (pDbHandItem)
			{
			lCount++;
			
			//Eliminate the entities that we dont support
			if ((UnsupportedEntity (pDbHandItem)) || (NotACountableEntity (pDbHandItem)) ||
				(PaperSpaceEntity (pDbHandItem))  || (UnimplementedEntity (pDbHandItem)))
				lCount--;
			}
		} while (lCount < Index && NULL != pDbHandItem);
		
		
	ASSERT (lCount == Index );
	if (Index > lCount)
		return E_INVALIDARG;

	CComPtr<IIcadEntity> pEnt;
	if (pDbHandItem)
		if (FAILED (CreateEntityWrapper (pDbHandItem, &pEnt)))
			return E_FAIL;

	return pEnt->QueryInterface (ppItem);
	}


/////////////////////////////////////////////////////////////////////////////
// CAutoEnumMSpaceEnts
HRESULT CAutoEnumMSpaceEnts::Init (CAutoMSpace *pMSpace, CIcadDoc *pDoc)
	{
	m_pMSpace = pMSpace;
	m_pDoc = pDoc;
	m_nCurrent = 0;
	m_hipCurrent = GetNextValidHip (NULL);
	pMSpace->get_Count (&m_nElements);
	return NOERROR;
	}

db_handitem * CAutoEnumMSpaceEnts::GetNextValidHip (db_handitem *pDbHandItem)
	{
	do
		{
		pDbHandItem = m_pDoc->m_dbDrawing->entnext_noxref(pDbHandItem);
		if (!pDbHandItem)
			return pDbHandItem;

		if ((UnsupportedEntity (pDbHandItem)) || (NotACountableEntity (pDbHandItem)) ||
			(PaperSpaceEntity (pDbHandItem))  || (UnimplementedEntity (pDbHandItem)))
			continue;
		else
			break;
		} while (pDbHandItem);
	
	return pDbHandItem;
	}

STDMETHODIMP CAutoEnumMSpaceEnts::Next (ULONG cElements, VARIANT * pvar, ULONG * pcElementFetched)
	{
    HRESULT hr;    
	ULONG l;    
	long l1;    
	ULONG l2;
	
    if (pcElementFetched != NULL)
		*pcElementFetched = 0;
    
	for (l = 0; l < cElements; l++)
		VariantInit (&pvar[l]);
	
    // Retrieve the next cElements elements.
    for (l1 = m_nCurrent, l2 = 0; 
	l1 < m_nElements && l2 < cElements; 
	l1++, l2++)    
		{
		CComPtr<IIcadEntity> pEnt;

		ASSERT (m_hipCurrent);
		hr = m_pMSpace->CreateEntityWrapper (m_hipCurrent, &pEnt);
		if (FAILED(hr))
			goto error;   

		IDispatch *pDispEnt;
		hr = pEnt->QueryInterface(IID_IDispatch, (void **)&pDispEnt);
		if (FAILED(hr))
			goto error;   
		
		pvar[l2].vt = VT_DISPATCH;
		pvar[l2].pdispVal = pDispEnt;

		//Advance the current hip pointer
		m_hipCurrent = GetNextValidHip (m_hipCurrent);
		}    
	
	// Set count of elements retrieved.
    if (pcElementFetched != NULL)        
		*pcElementFetched = l2;
	
    m_nCurrent = l1;
	
    return  (l2 < cElements) ? ResultFromScode(S_FALSE) : NOERROR;
	
error:
    for (l = 0; l < cElements; l++)        
		VariantClear (&pvar[l]);
	
	return hr;
	}


STDMETHODIMP CAutoEnumMSpaceEnts::Skip (ULONG cElements)
	{
    m_nCurrent += cElements;

	ULONG i;
	for (i = 0; i < cElements; i++)
		m_hipCurrent = GetNextValidHip (m_hipCurrent);
	
	if (m_nCurrent > m_nElements)
		{        
		m_nCurrent = m_nElements;
        return ResultFromScode(S_FALSE);    
		}    
	else 
		return NOERROR;
	}


STDMETHODIMP CAutoEnumMSpaceEnts::Reset ()
	{
	m_nCurrent = 0;
	m_hipCurrent = GetNextValidHip (NULL);
	return NOERROR;
	}

STDMETHODIMP CAutoEnumMSpaceEnts::Clone (IEnumVARIANT * * ppenum)
	{
	if (ppenum == NULL)
		return E_POINTER;
	
	*ppenum = NULL;    
	CComObject<CAutoEnumMSpaceEnts>* penum;
	CComObject<CAutoEnumMSpaceEnts>::CreateInstance(&penum);
	if (!penum)
        return E_FAIL;    
	
	penum->Init(m_pMSpace, m_pDoc);
	penum->AddRef();    
	
	*ppenum = penum;    
	return NOERROR;
	}


