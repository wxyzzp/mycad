/* INTELLICAD\PRJ\LIB\AUTO\AUTODOCUMENTS.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Implementation of CAutoDocuments - The Automation Documents collection
 * 
 */ 

#include "StdAfx.h"
#include "AutoDocuments.h"
#include "AutoDoc.h"
#include "IcadDocTemplate.h"
#include "IcadMain.h"


/////////////////////////////////////////////////////////////////////////////
// CAutoDocuments

HRESULT CAutoDocuments::Init (CIcadMultiDocTemplate* pDocTemplate)
	{
	if (!m_pDocTemplate)
		m_pDocTemplate = pDocTemplate;
	return NOERROR;
	}

STDMETHODIMP CAutoDocuments::get_Application (IIcadApplication * * ppApp)
	{
	if (ppApp == NULL)
		return E_POINTER;
	
	return GetApp()->m_pAutoApp->QueryInterface (ppApp);
	}

STDMETHODIMP CAutoDocuments::get_Count (long * pCount)
	{
	if (pCount == NULL)
		return E_POINTER;
	
	POSITION pos = m_pDocTemplate->GetFirstDocPosition();
	int Count = 0;
	CIcadDoc *pDoc;
	while(pos!=NULL)
		{
		pDoc = (CIcadDoc*)m_pDocTemplate->GetNextDoc(pos);
		Count++;
		}

	*pCount = Count;
	return NOERROR;
	}

STDMETHODIMP CAutoDocuments::get_Parent (IIcadApplication * * ppParent)
	{
	if (ppParent == NULL)
		return E_POINTER;
	
	return GetApp()->m_pAutoApp->QueryInterface (ppParent);
	}

STDMETHODIMP CAutoDocuments::get__NewEnum (IUnknown * * ppUnk)
	{
	if (ppUnk == NULL)
		return E_POINTER;
	
	CComObject<CAutoEnumDocuments>* penum;
	CComObject<CAutoEnumDocuments>::CreateInstance(&penum);
	if (!penum)
        return E_FAIL;    
	
	CComPtr<IIcadDocuments> pDocs;
	QueryInterface(IID_IIcadDocuments, (void **)&pDocs);
	penum->Init(pDocs);
	
	return penum->QueryInterface(IID_IUnknown, (void **)ppUnk);
	}

STDMETHODIMP CAutoDocuments::Add (IIcadDocument * * ppDocument)
	{
	if (ppDocument == NULL)
		return E_POINTER;
	
//	CIcadDoc* pDoc = (CIcadDoc*) m_pDocTemplate->OpenDocumentFile(NULL, TRUE);

	CIcadApp *pApp = (CIcadApp*)GetApp();
/*
	((CMainWindow*)pApp->m_pMainWnd)->m_bInOpenNew = 1;
	pApp->FileNew ();
	((CMainWindow*)pApp->m_pMainWnd)->m_bInOpenNew = 0;
*/
	if (FAILED(pApp->AutoOpenFile (NULL, FALSE)))
		return E_FAIL;

	CIcadDoc* pDoc = ((CMainWindow*)pApp->m_pMainWnd)->m_pCurDoc;
	if (pDoc)
		return pDoc->m_pAutoDoc->QueryInterface (ppDocument);
	else
		return E_FAIL;
	}

STDMETHODIMP CAutoDocuments::CloseAll (VARIANT_BOOL AskSave)
	{

	if (AskSave == VARIANT_TRUE)
		m_pDocTemplate->SaveAllModified();

	m_pDocTemplate->CloseAllDocuments(FALSE);

	return NOERROR;
	}

STDMETHODIMP CAutoDocuments::Item (VARIANT Index, IIcadDocument * * ppItem)
	{
	if (ppItem == NULL)
		return E_POINTER;
	
	CIcadDoc *pDoc;
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

	if (byIndex)
		{
		long Count = 0, i;
		get_Count (&Count);
		if ((lIndex < 1) || (lIndex > Count))
			return (E_INVALIDARG);
		
		POSITION pos = m_pDocTemplate->GetFirstDocPosition();
		for (i = 0; i < lIndex; i++)
			{
			pDoc = (CIcadDoc*)m_pDocTemplate->GetNextDoc(pos);
			}

		}
	else if (byName)
		{
		CString filename(Name);
		bool bFound = false;
	
		POSITION pos = m_pDocTemplate->GetFirstDocPosition();
		while(pos!=NULL)
			{
			pDoc = (CIcadDoc*)m_pDocTemplate->GetNextDoc(pos);
			CString strTitle = pDoc->GetTitle();
			if (strTitle.CompareNoCase(filename) == 0)
				{
				bFound = true;
				break;
				}
			}

		if (pos == NULL && !bFound)	//we didnt find the document
			return E_FAIL;
		}
	
	return pDoc->m_pAutoDoc->QueryInterface (ppItem);
	}

STDMETHODIMP CAutoDocuments::Open (BSTR File, VARIANT_BOOL ReadOnly, IIcadDocument * * ppDocument)
	{
	if (ppDocument == NULL)
		return E_POINTER;

	CString filename (File);
	if (filename.IsEmpty())
		return E_INVALIDARG;
	
	//check for file existence
	if ((-1) == access(filename,00))
		return E_INVALIDARG;
	
//	CIcadDoc* pDoc = (CIcadDoc*) m_pDocTemplate->OpenDocumentFile (filename, TRUE);

	CIcadApp *pApp = (CIcadApp*)GetApp();

	//First check to see if it is already open
	CIcadDoc* pOpenDoc = pApp->GetOpenFile(filename);
	if (pOpenDoc)
		return E_INVALIDARG;

	bool bReadOnly = (ReadOnly == VARIANT_TRUE) ? TRUE : FALSE;

	if (FAILED(pApp->AutoOpenFile (filename.GetBuffer(0), bReadOnly)))
		return E_FAIL;

	CIcadDoc* pDoc = ((CMainWindow*)pApp->m_pMainWnd)->m_pCurDoc;
	if (pDoc)
		return pDoc->m_pAutoDoc->QueryInterface (ppDocument);
	else
		return E_FAIL;
	}

STDMETHODIMP CAutoDocuments::SaveAll ()
	{
	POSITION pos = m_pDocTemplate->GetFirstDocPosition();
	while (pos != NULL)
		{
		CDocument* pDoc = m_pDocTemplate->GetNextDoc(pos);
		if (!pDoc->DoFileSave())
			return E_FAIL;
		}
	
	return NOERROR;
	}


/////////////////////////////////////////////////////////////////////////////
// CAutoEnumDocuments

HRESULT CAutoEnumDocuments::Init (IIcadDocuments *pDocuments)
	{
	m_pDocuments = pDocuments;
	m_nCurrent = 0;
	return NOERROR;
	}

STDMETHODIMP CAutoEnumDocuments::Next (ULONG cElements, VARIANT * pvar, ULONG * pcElementFetched)
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
	m_pDocuments->get_Count(&nElements);
	
    for (l1 = m_nCurrent, l2 = 0; 
	l1 < nElements && l2 < cElements; 
	l1++, l2++)    
		{
		CComPtr<IIcadDocument> pEnt;
		CComVariant Index;
		V_VT(&Index) = VT_I4;
		V_I4(&Index) = l1 + 1;	
		if (FAILED (m_pDocuments->Item(Index, &pEnt)))
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


STDMETHODIMP CAutoEnumDocuments::Skip (ULONG cElements)
	{
    m_nCurrent += cElements;
	
	long nElements = 0;
	m_pDocuments->get_Count(&nElements);
	
	if (m_nCurrent > nElements)
		{        
		m_nCurrent = nElements;
        return ResultFromScode(S_FALSE);    
		}    
	else 
		return NOERROR;
	}


STDMETHODIMP CAutoEnumDocuments::Reset ()
	{
	m_nCurrent = 0;
	return NOERROR;
	}

STDMETHODIMP CAutoEnumDocuments::Clone (IEnumVARIANT * * ppenum)
	{
	if (ppenum == NULL)
		return E_POINTER;
	
	*ppenum = NULL;    
	CComObject<CAutoEnumDocuments>* penum;
	CComObject<CAutoEnumDocuments>::CreateInstance(&penum);
	if (!penum)
        return E_FAIL;    
	
	penum->Init(m_pDocuments);
	penum->AddRef();    
	
	*ppenum = penum;    
	return NOERROR;
	}


