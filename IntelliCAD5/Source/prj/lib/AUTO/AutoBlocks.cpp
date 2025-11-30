/* INTELLICAD\PRJ\LIB\AUTO\AUTOBLOCKS.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Implementation of CAutoBlocks - The Automation Blocks collection
 * 
 */ 

#include "StdAfx.h"
#include "AutoBlocks.h"
#include "AutoBlock.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoBlocks

HRESULT CAutoBlocks::Init(CIcadDoc *pDoc)
	{
	if (!m_pDoc)
		m_pDoc = pDoc;
	return NOERROR;
	}

STDMETHODIMP CAutoBlocks::get_Application (IIcadApplication * * ppApp)
	{
	if (ppApp == NULL)
		return E_POINTER;
	
	return GetApp()->m_pAutoApp->QueryInterface (ppApp);
	}

STDMETHODIMP CAutoBlocks::get_Count (long * pCount)
	{
	if (pCount == NULL)
		return E_POINTER;
	
	ASSERT (m_pDoc);
	db_handitem* pDbHandItem = NULL;
	
	long lCount = 0;
	db_blocktabrec *pTableRecord = (db_blocktabrec *)m_pDoc->m_dbDrawing->tblnext( DB_BLOCKTAB, 1 );
	
	while( pTableRecord != NULL )
		{
		ASSERT( pTableRecord->ret_type() == DB_BLOCKTABREC );
		if (pTableRecord && !pTableRecord->ret_deleted())
			lCount++;
		pTableRecord = (db_blocktabrec *)pTableRecord->next;
		}
	
	*pCount = lCount;
	return NOERROR;
	}

STDMETHODIMP CAutoBlocks::get_Parent (IIcadDocument * * ppParent)
	{
	if (ppParent == NULL)
		return E_POINTER;
	
	ASSERT (m_pDoc);
	return m_pDoc->m_pAutoDoc->QueryInterface (ppParent);
	}

STDMETHODIMP CAutoBlocks::get__NewEnum (IUnknown * * ppUnk)
	{
	if (ppUnk == NULL)
		return E_POINTER;
	
	CComObject<CAutoEnumBlocks>* penum;
	CComObject<CAutoEnumBlocks>::CreateInstance(&penum);
	if (!penum)
        return E_FAIL;    
	
	CComPtr<IIcadBlocks> pEnts;
	QueryInterface(IID_IIcadBlocks, (void **)&pEnts);
	penum->Init(pEnts);
	
	return penum->QueryInterface(IID_IUnknown, (void **)ppUnk);
	}

STDMETHODIMP CAutoBlocks::Add (IIcadPoint * InsertionPoint, BSTR Name, IIcadBlock * * ppItem)
	{
	if (ppItem == NULL)
		return E_POINTER;
	
	CComObject<CAutoBlock> *pBlock;
	CComObject<CAutoBlock>::CreateInstance(&pBlock);
	if (!pBlock)
		return E_FAIL;
	
	HRESULT hr;
	hr = pBlock->Init(m_pDoc, this, NULL);
	if (FAILED(hr))
		return hr;
	
	hr = pBlock->put_Name (Name);
	if (FAILED(hr))
		return hr;
	
	if (m_pDoc->m_dbDrawing->addhanditem (pBlock->m_pDbHandItem) != 0)
		return E_FAIL;

    db_handitem *endblk_hip = NULL;
	db_block *blk_hip = NULL;
	db_handitem *pCLayer = NULL;
	db_handitem *pCLtype = NULL;
	pCLayer = m_pDoc->m_dbDrawing->ret_currenttabrec (DB_QCLAYER);
	pCLtype = m_pDoc->m_dbDrawing->ret_currenttabrec (DB_QCELTYPE);

	blk_hip = new db_block;
	if (blk_hip == NULL)
		return E_OUTOFMEMORY;
	blk_hip->assignhand(m_pDoc->m_dbDrawing);
	blk_hip->set_layerhip(pCLayer);
	blk_hip->set_ltypehip(pCLtype);

	endblk_hip = new db_endblk;
	if (endblk_hip == NULL)
		return E_OUTOFMEMORY;
	endblk_hip->assignhand(m_pDoc->m_dbDrawing);
	endblk_hip->set_layerhip(pCLayer);
	endblk_hip->set_ltypehip(pCLtype);

	pBlock->m_pDbHandItem->addent(blk_hip, m_pDoc->m_dbDrawing);
	pBlock->m_pDbHandItem->addent(endblk_hip, m_pDoc->m_dbDrawing);

	/*DG - 1.11.2001*/// Set origin too. Note, AFTER adding of db_block.
	hr = pBlock->put_Origin(InsertionPoint);
	if(FAILED(hr))
		return hr;
	
	return pBlock->QueryInterface(ppItem);
	}

STDMETHODIMP CAutoBlocks::Item (VARIANT Index, IIcadBlock * * ppItem)
	{
	if (ppItem == NULL)
		return E_POINTER;
	
	ASSERT (m_pDoc);
	
	BOOL byIndex = FALSE;
	BOOL byName = FALSE;
	int lIndex;
	BSTR Name;
	CComVariant vIndex(Index);
	CComVariant nIndex;
	if (Index.vt == VT_BSTR)
		{
		byName = TRUE;
		Name = V_BSTR(&vIndex);
		}
	else if (SUCCEEDED (nIndex.ChangeType(VT_I4, &vIndex)))
		{
		byIndex = TRUE;
		lIndex = V_I4 (&nIndex);
		}
	else if (SUCCEEDED (nIndex.ChangeType(VT_BSTR, &vIndex)))
		{
		byName = TRUE;
		Name = V_BSTR (&nIndex);
		}
	
	db_blocktabrec *pTableRecord = NULL;
	if (byIndex)
		{
		if (lIndex < 1)
			return E_INVALIDARG;

		long lCount = 0;
		pTableRecord = (db_blocktabrec *)m_pDoc->m_dbDrawing->tblnext( DB_BLOCKTAB, 1 );
		if (!pTableRecord)
			return E_FAIL;

		if (!pTableRecord->ret_deleted())
			lCount++;
		
		while( (lCount < lIndex) && (pTableRecord != NULL) )
			{
			pTableRecord = (db_blocktabrec *)pTableRecord->next;
			ASSERT( pTableRecord->ret_type() == DB_BLOCKTABREC );
			if (!pTableRecord->ret_deleted())
				lCount++;
			}
		
		//ASSERT (lCount == lIndex );
		if (lIndex > lCount)
			return E_INVALIDARG;
		}
	else if (byName)
		{
		CString str(Name);
		pTableRecord = (db_blocktabrec *)m_pDoc->m_dbDrawing->findtabrec( DB_BLOCKTAB, str.GetBuffer(0), 1 );
		}
	
	if (pTableRecord)
		{
		CComObject<CAutoBlock> *pEnt;
		CComObject<CAutoBlock>::CreateInstance(&pEnt);
		if (!pEnt)
			return E_FAIL;
		
		CComPtr<IDispatch> pOwner;
		QueryInterface (IID_IDispatch, (void **)&pOwner);
		if (FAILED (pEnt->Init (m_pDoc, pOwner, pTableRecord)))
			return E_FAIL;
		
		return pEnt->QueryInterface(ppItem);
		}
	else
		return NOERROR;
	}




/////////////////////////////////////////////////////////////////////////////
// CAutoEnumBlocks

HRESULT CAutoEnumBlocks::Init (IIcadBlocks *pBlocks)
	{
	m_pBlocks = pBlocks;
	m_nCurrent = 0;
	return NOERROR;
	}

STDMETHODIMP CAutoEnumBlocks::Next (ULONG cElements, VARIANT * pvar, ULONG * pcElementFetched)
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
	long nElements = 0;
	m_pBlocks->get_Count(&nElements);
	
    for (l1 = m_nCurrent, l2 = 0; 
	l1 < nElements && l2 < cElements; 
	l1++, l2++)    
		{
		CComPtr<IIcadBlock> pEnt;
		CComVariant Index;
		V_VT(&Index) = VT_I4;
		V_I4(&Index) = l1 + 1;	
		if (FAILED (m_pBlocks->Item(Index, &pEnt)))
			return S_FALSE;
		
		IDispatch *pDispEnt;
		hr = pEnt->QueryInterface(IID_IDispatch, (void **)&pDispEnt);
		if (FAILED(hr))
            goto error;     
		
		pvar[l2].vt = VT_DISPATCH;
		pvar[l2].pdispVal = pDispEnt;
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


STDMETHODIMP CAutoEnumBlocks::Skip (ULONG cElements)
	{
    m_nCurrent += cElements;
	
	long nElements = 0;
	m_pBlocks->get_Count(&nElements);
	
	if (m_nCurrent > nElements)
		{        
		m_nCurrent = nElements;
        return ResultFromScode(S_FALSE);    
		}    
	else 
		return NOERROR;
	}


STDMETHODIMP CAutoEnumBlocks::Reset ()
	{
	m_nCurrent = 0;
	return NOERROR;
	}

STDMETHODIMP CAutoEnumBlocks::Clone (IEnumVARIANT * * ppenum)
	{
	if (ppenum == NULL)
		return E_POINTER;
	
	*ppenum = NULL;    
	CComObject<CAutoEnumBlocks>* penum;
	CComObject<CAutoEnumBlocks>::CreateInstance(&penum);
	if (!penum)
        return E_FAIL;    
	
	penum->Init(m_pBlocks);
	penum->AddRef();    
	
	*ppenum = penum;    
	return NOERROR;
	}


