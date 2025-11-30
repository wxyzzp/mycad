/* INTELLICAD\PRJ\LIB\AUTO\AUTOBLOCK.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Declaration of the CAutoBlock - The Automation Block object.
 * 
 */ 

#include "StdAfx.h"
#include "AutoBlock.h"
#include "AutoPoint.h"
#include "AutoHelpers.h"
#include "appid.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoBlock


HRESULT CAutoBlock::Init(CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem)
	{
	m_pDoc = pDoc;
	m_pParent = pParent;
	
	if (pDbHandItem)
		m_pDbHandItem = pDbHandItem;
	else
		m_pDbHandItem = new db_blocktabrec();
	
	if (NULL == m_pDbHandItem) 
		return FALSE;
	
	return NOERROR;
	}

//This function name is a misnomer.  It is inherited from AutoEntities - a generic class.
//This function adds a handitem to an owner.  The owner could be a Drawing (for Paper space and model 
//space collections) or a Block as in this case.
HRESULT CAutoBlock::AddItemToDrawing (db_handitem *pDbHandItem, BOOL complex)
	{
	ASSERT (m_pDoc);

	//make sure it has a handle
	pDbHandItem->assignhand (m_pDoc->m_dbDrawing);

    //make sure it has a layer
    if (pDbHandItem->ret_layerhip()==NULL) 
		{
		db_handitem *pTableRecord = NULL;
		pTableRecord = m_pDoc->m_dbDrawing->ret_currenttabrec (DB_QCLAYER);

		pDbHandItem->set_layerhip(pTableRecord);
		}

    //make sure it has a ltype
    if (pDbHandItem->ret_ltypehip()==NULL) 
		{
		db_handitem *pTableRecord = NULL;
		pTableRecord = m_pDoc->m_dbDrawing->ret_currenttabrec (DB_QCELTYPE);

		pDbHandItem->set_ltypehip(pTableRecord);
		}

	//if we are adding a non constant attribute, set the 70 flags on the block
	if (pDbHandItem->ret_type() == DB_ATTDEF)
		{
		int mode = 0;
		pDbHandItem->get_70(&mode);
		
		if (!(mode & IC_ATTDEF_CONST))
			{
			int flags;
			m_pDbHandItem->get_70 (&flags);

			flags |= IC_BLOCK_HASATTRIBUTES;

			m_pDbHandItem->set_70 (flags);
			}
		}

	db_handitem *hip, *prevhip = NULL;
	
	prevhip = m_pDbHandItem;
	hip = ((db_blocktabrec *)m_pDbHandItem)->ret_block();
	
	while ( hip != NULL)
		{
		if (hip->ret_type() == DB_ENDBLK)
			break;
		prevhip = hip;
		hip = hip->next;
		}
	
	prevhip->next = pDbHandItem;
	if (complex && pDbHandItem->next)	/*D.G.*/// pDbHandItem->next added because this method may be called 
		{								// with complex=true not for really complex entities (see CAutoBlock::AddItems)
		while ( (pDbHandItem->ret_type() != DB_SEQEND) &&
				(pDbHandItem->ret_type() != DB_ENDBLK) )
			{
			pDbHandItem = pDbHandItem->next;
			if (pDbHandItem == NULL)
				break;
			}
		}

	if(pDbHandItem)
		pDbHandItem->next = hip;

	m_pDoc->SetModifiedFlag(TRUE);
	return NOERROR;
	}

CIcadDoc * CAutoBlock::GetDocument ()	//return the document in which this collection exists
	{
	return m_pDoc;
	}

HRESULT CAutoBlock::GetOwner (IDispatch **ppOwner) //return a pointer to the owner - Block
	{
	return QueryInterface(IID_IDispatch, (void **)ppOwner);
	}

STDMETHODIMP CAutoBlock::get_Application (IIcadApplication * * ppApp)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppApp == NULL)
		return E_POINTER;
	
	return GetApp()->m_pAutoApp->QueryInterface (ppApp);
	}

STDMETHODIMP CAutoBlock::get_Count (long * pCount)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pCount == NULL)
		return E_POINTER;
	
	ASSERT (m_pDoc);
	db_handitem* pDbHandItem = ((db_blocktabrec *)m_pDbHandItem)->ret_block();
	
	long lCount=0L;
	do
		{
		pDbHandItem = pDbHandItem->next;
		
		if (pDbHandItem && (pDbHandItem->ret_type() != DB_ENDBLK))
			{
			lCount++;
			
			//Eliminate the entities that we dont support
			if ((UnsupportedEntity (pDbHandItem)) || (NotACountableEntity (pDbHandItem)) ||
				(UnimplementedEntity (pDbHandItem)))
				lCount--;
			}
		} while (pDbHandItem);
		
	*pCount = lCount;
	return NOERROR;
	}

STDMETHODIMP CAutoBlock::get_Parent (IIcadDocument * * ppParent)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppParent == NULL)
		return E_POINTER;
	
	ASSERT (m_pDoc);
	return m_pDoc->m_pAutoDoc->QueryInterface (ppParent);
	}

STDMETHODIMP CAutoBlock::get__NewEnum (IUnknown * * ppUnk)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppUnk == NULL)
		return E_POINTER;
	
	CComObject<CAutoEnumBlockEnts>* penum;
	CComObject<CAutoEnumBlockEnts>::CreateInstance(&penum);
	if (!penum)
        return E_FAIL;    
	
	penum->Init(this, ((db_blocktabrec *)m_pDbHandItem)->ret_block());
	
	return penum->QueryInterface(IID_IUnknown, (void **)ppUnk);
	}

STDMETHODIMP CAutoBlock::Item (long Index, IDispatch * * ppItem)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppItem == NULL)
		return E_POINTER;

	if (Index < 1)
		return E_INVALIDARG;
	
	ASSERT (m_pDoc);
	db_handitem* pDbHandItem = ((db_blocktabrec *)m_pDbHandItem)->ret_block();
	
	long lCount=0L;
	do
		{
		pDbHandItem = pDbHandItem->next;
		
		if (pDbHandItem && (pDbHandItem->ret_type() != DB_ENDBLK))
			{
			lCount++;
			
			//Eliminate the entities that we dont support
			if ((UnsupportedEntity (pDbHandItem)) || (NotACountableEntity (pDbHandItem)) ||
				(UnimplementedEntity (pDbHandItem)))
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

STDMETHODIMP CAutoBlock::get_Origin (IIcadPoint * * ppOrigin)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppOrigin == NULL)
		return E_POINTER;
	
	sds_point pt;
	m_pDbHandItem->get_10(pt);
	
	CComObject<CAutoPoint> *pPoint;
	CComObject<CAutoPoint>::CreateInstance(&pPoint);
	if (!pPoint)
		return E_FAIL;
	
	pPoint->Init(NULL, NULL, pt[0], pt[1], pt[2]);
	
	return pPoint->QueryInterface (ppOrigin);
	}

STDMETHODIMP CAutoBlock::put_Origin (IIcadPoint * ppOrigin)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	sds_point pt;
	pt[0] = pt[1] = pt[2] = 0.0;

	if (ppOrigin)	//this is an optional arg
		{
		ppOrigin->get_x(&pt[0]);
		ppOrigin->get_y(&pt[1]);
		ppOrigin->get_z(&pt[2]);
		}
	m_pDbHandItem->set_10(pt);
	
	return NOERROR;
	}

STDMETHODIMP CAutoBlock::get_Name (BSTR * pstrName)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pstrName == NULL)
		return E_POINTER;
	
	char* pStr;
	if ( !m_pDbHandItem->get_2 (&pStr)) 
		return E_FAIL;
	
	CString str(pStr);
	*pstrName = str.AllocSysString();
	return NOERROR;	
	}

STDMETHODIMP CAutoBlock::put_Name (BSTR pstrName)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	CString str(pstrName);
	if (str.IsEmpty ())
		return E_INVALIDARG;

	if (!ic_isvalidname(str.GetBuffer(0)))
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_2 (str.GetBuffer(0)))
		return E_FAIL;
	
	return NOERROR;
	}

STDMETHODIMP CAutoBlock::get_IsXRef (VARIANT_BOOL * IsXRef)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (IsXRef == NULL)
		return E_POINTER;
	
	int flags;
	m_pDbHandItem->get_70 (&flags);
	
	*IsXRef = (flags & (IC_BLOCK_XREF | IC_BLOCK_XREF_OVERLAID)) ? VARIANT_TRUE : VARIANT_FALSE;

	return NOERROR;
	}

STDMETHODIMP CAutoBlock::AddItems (IIcadSelectionSet * Entities, VARIANT_BOOL DeleteOriginals)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (Entities == NULL)
		return E_INVALIDARG;

	long i, nCount;
	HRESULT hr = Entities->get_Count (&nCount);
	if (FAILED(hr) || (nCount == 0))
		return E_INVALIDARG;

	CComPtr<IIcadEntity> pEnt;
	CComPtr<IDispatch> pDisp;
	long nhip;
	db_handitem *pHip, *pNewHip;
	BOOL bAdded = FALSE;

	for (i = 1; i <= nCount; i++)
		{
		hr = Entities->Item (i, &pDisp);
		if (FAILED(hr))
			continue;

		hr = pDisp->QueryInterface (&pEnt);
		if (FAILED(hr))
			continue;

		hr = pEnt->_handitem (&nhip);
		if (FAILED(hr) || (nhip == 0))
			continue;

		pHip = (db_handitem *)nhip;
		pNewHip = CopyHip (pHip);

		hr = AddItemToDrawing (pNewHip, TRUE);
		if (FAILED(hr))
			continue;

		if (DeleteOriginals == VARIANT_TRUE)
			pEnt->Delete();

		bAdded++;
		pEnt = NULL;
		pDisp = NULL;
		}

	if (bAdded)
		return NOERROR;
	else	//nothing was added to the block
		return E_FAIL;
	}

STDMETHODIMP CAutoBlock::GetXData (BSTR AppName, VARIANT * TypeArray, VARIANT * DataArray)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (TypeArray == NULL)
		return E_POINTER;
	
	if (DataArray == NULL)
		return E_POINTER;
	
	CString strApp (AppName);
	CString strEmpty ("*");

	struct resbuf rb;
	rb.restype = RTSTR;
	if (strApp.IsEmpty())
		rb.resval.rstring = strEmpty.GetBuffer(0);
	else
		rb.resval.rstring = strApp.GetBuffer(0);
	rb.rbnext = NULL;
 
	struct resbuf *prbExtendedData = m_pDbHandItem->get_eed (&rb, NULL);

	if (prbExtendedData == NULL)
		{
		(*TypeArray).vt = VT_EMPTY;
		(*TypeArray).parray = NULL;
		(*DataArray).vt = VT_EMPTY;
		(*DataArray).parray = NULL;
		return NOERROR;
		}

	long nCount = 0;
	struct resbuf *ptr = prbExtendedData;
	while( ptr )
		{
		ptr = ptr->rbnext;
		nCount++;
		}

	VARIANT var;
	SAFEARRAY FAR* Types;
	SAFEARRAY FAR* Values;
	SAFEARRAYBOUND Bound[1];
	Bound[0].lLbound = 0;
	Bound[0].cElements = nCount - 1;
	Types = SafeArrayCreate(VT_I2, 1, Bound);
	Values = SafeArrayCreate(VT_VARIANT, 1, Bound);

	ptr = prbExtendedData->rbnext;
	nCount = 0;
	while( ptr )
		{
		SafeArrayPutElement(Types, &nCount, &(ptr->restype));
		switch (ptr->restype)
			{
			case 1000:	//String
			case 1001:	//Application name
			case 1002:	//Control string
			case 1003:	//Layer name
			case 1005:	//*Database handle
				{
				CString str(ptr->resval.rstring);
				var.vt = VT_BSTR;
				var.bstrVal = str.AllocSysString();
				SafeArrayPutElement(Values, &nCount, &var);
				}
				break;
			case 1010:
			case 1020:
			case 1030:
			case 1011:
			case 1021:
			case 1031:
			case 1012:
			case 1022:
			case 1032:
			case 1013:
			case 1023:
			case 1033:
				{
				CComObject<CAutoPoint> *pPoint;
				CComObject<CAutoPoint>::CreateInstance(&pPoint);
				if (!pPoint)
					return E_FAIL;
				
				pPoint->Init(NULL, NULL, ptr->resval.rpoint[0], ptr->resval.rpoint[1], ptr->resval.rpoint[2]);
				
				IDispatch *pDisp;
				if (FAILED (pPoint->QueryInterface (&pDisp)))
					return E_FAIL;

				var.vt = VT_DISPATCH;
				var.pdispVal = pDisp;
				SafeArrayPutElement(Values, &nCount, &var);
				}
				break;
			case 1040:
			case 1041:
			case 1042:
				var.vt = VT_R4;
				var.dblVal = ptr->resval.rreal;
				SafeArrayPutElement(Values, &nCount, &var);
				break;
			case 1070:
				var.vt = VT_I4;
				var.lVal = ptr->resval.rint;
				SafeArrayPutElement(Values, &nCount, &var);
				break;
			case 1071:
				var.vt = VT_I4;
				var.lVal = ptr->resval.rlong;
				SafeArrayPutElement(Values, &nCount, &var);
				break;
			}
		ptr = ptr->rbnext;
		nCount++;
		}	

	(*TypeArray).vt = VT_ARRAY | VT_I2;
	(*TypeArray).parray = Types;
	(*DataArray).vt = VT_ARRAY | VT_VARIANT;
	(*DataArray).parray = Values;

	if ( prbExtendedData != NULL )
		{
		sds_relrb( prbExtendedData );
		prbExtendedData = NULL;
		}

	return NOERROR;
	}

STDMETHODIMP CAutoBlock::SetXData (VARIANT TypeArray, VARIANT DataArray)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	SAFEARRAY FAR* arrayType;
	SAFEARRAY FAR* arrayData;

	//get the array of types
	if (TypeArray.vt == (VT_VARIANT|VT_BYREF))
		{
		if (!(TypeArray.pvarVal->vt & VT_ARRAY))
			return E_INVALIDARG;

		arrayType = TypeArray.pvarVal->parray;
		}
	else if (TypeArray.vt & VT_ARRAY)
		arrayType = TypeArray.parray;
	else
		return E_INVALIDARG;
	
	//get the array of data
	if (DataArray.vt == (VT_VARIANT|VT_BYREF))
		{
		if (!(DataArray.pvarVal->vt & VT_ARRAY))
			return E_INVALIDARG;

		arrayData = DataArray.pvarVal->parray;
		}
	else if (DataArray.vt & VT_ARRAY)
		arrayData = DataArray.parray;
	else
		return E_INVALIDARG;
	

	HRESULT hr;
	long LBound, UBound;
	hr = SafeArrayGetLBound(arrayType, 1, &LBound);
	if (FAILED(hr))
		return E_INVALIDARG;
	
	hr = SafeArrayGetUBound(arrayType, 1, &UBound);
	if (FAILED(hr))
		return E_INVALIDARG;
	
	struct resbuf *prb, *headrb;
	
	prb = headrb = sds_buildlist (-3,0);
	
	long i;
	short type;
	VARIANT var;
	for (i = LBound; i <= UBound; i++)
		{
		
		SafeArrayGetElement(arrayType, &i, &type);
		switch (type)
			{
			case 1000:	//String
			case 1001:	//Application name
			case 1002:	//Control string
			case 1003:	//Layer name
			case 1005:	//*Database handle
				{
				SafeArrayGetElement(arrayData, &i, &var);
				ASSERT (var.vt == VT_BSTR);
				CString str(var.bstrVal);
				prb = prb->rbnext = sds_newrb(type);
				prb->resval.rstring = new char [str.GetLength()+1]; 
				memset(prb->resval.rstring,0,str.GetLength()+1);
				strncpy(prb->resval.rstring, str, str.GetLength());

				if (type == 1001)
					{
					//ensure the app name is registered
					db_appidtabrec *pTableRecord = NULL;
					pTableRecord = (db_appidtabrec *)m_pDoc->m_dbDrawing->findtabrec( DB_APPIDTAB, str.GetBuffer(0), 1 );
					if (pTableRecord == NULL)
						{
						if (!ic_isvalidname(str.GetBuffer(0)))
							return E_INVALIDARG;

						db_handitem * hip = new db_appidtabrec();

						if (!hip->set_2 (str.GetBuffer(0)))
							return E_FAIL;

						if (m_pDoc->m_dbDrawing->addhanditem (hip) != 0)
							return E_FAIL;
						}
					}
				}
				break;
			case 1010:
			case 1020:
			case 1030:
			case 1011:
			case 1021:
			case 1031:
			case 1012:
			case 1022:
			case 1032:
			case 1013:
			case 1023:
			case 1033:
				{
				SafeArrayGetElement(arrayData, &i, &var);
				ASSERT (var.vt == VT_DISPATCH);

				CComPtr<IIcadPoint> pPoint;
				var.pdispVal->QueryInterface (&pPoint);
				if (!pPoint)
					return E_FAIL;

				prb = prb->rbnext = sds_newrb(type);
				pPoint->get_x (&prb->resval.rpoint[0]); 
				pPoint->get_y (&prb->resval.rpoint[1]); 
				pPoint->get_z (&prb->resval.rpoint[2]); 
				}
				break;
			case 1040:
			case 1041:
			case 1042:
				{
				SafeArrayGetElement(arrayData, &i, &var);
				ASSERT (var.vt == VT_R4);
				prb = prb->rbnext = sds_newrb(type);
				prb->resval.rreal = var.dblVal; 
				}
				break;
			case 1070:
				SafeArrayGetElement(arrayData, &i, &var);
				ASSERT (var.vt == VT_I4);
				prb = prb->rbnext = sds_newrb(type);
				prb->resval.rint = (short)var.lVal; 
				break;
			case 1071:
				SafeArrayGetElement(arrayData, &i, &var);
				ASSERT (var.vt == VT_I4);
				prb = prb->rbnext = sds_newrb(type);
				prb->resval.rlong = var.lVal; 
				break;
			}
		}
	
	m_pDbHandItem->set_eed (headrb, m_pDoc->m_dbDrawing);
	sds_relrb(headrb);
	return NOERROR;
	}

STDMETHODIMP CAutoBlock::Delete ()
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	//if (m_pDoc->m_dbDrawing->delhanditem (NULL, m_pDbHandItem, 0) == 0) //this doensn't handle complex entities
	if (DeletedEntity (m_pDbHandItem))
		{
		ASSERT (0);	//should never get here - the user cannot access a deleted entity
		return S_OK;	//but it's OK, no big deal - just dont undelete it.
		}

	return m_pDoc->DeleteEntity(m_pDbHandItem);
	}

STDMETHODIMP CAutoBlock::Bind ()
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	CString str;
	return m_pDoc->XRefOperations (3, str, m_pDbHandItem);
	}

STDMETHODIMP CAutoBlock::Detach ()
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	CString str;
	return m_pDoc->XRefOperations (4, str, m_pDbHandItem);
	}

STDMETHODIMP CAutoBlock::Unload ()
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	CString str;
	return m_pDoc->XRefOperations (2, str, m_pDbHandItem);
	}

STDMETHODIMP CAutoBlock::Reload ()
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	CString str;
	return m_pDoc->XRefOperations (1, str, m_pDbHandItem);
	}

STDMETHODIMP CAutoBlock::WBlock (BSTR FileName)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	CString name(FileName);
	if (name.IsEmpty())
		return E_INVALIDARG;

	sds_point origin;
	origin[0] = origin[1] = origin[2] = 0.0;
	m_pDbHandItem->get_10(origin);

	ASSERT (m_pDoc);
	db_handitem* pDbHandItem = ((db_blocktabrec *)m_pDbHandItem)->ret_block();
	
    sds_name ename;
	sds_name sset;
	m_pDoc->AUTO_ssadd (NULL, NULL, sset);

	do
		{
		pDbHandItem = pDbHandItem->next;
		
		if (pDbHandItem && (pDbHandItem->ret_type() != DB_ENDBLK))
			{
			ename[0] = (long)pDbHandItem;
			ename[1] = (long)(m_pDoc->m_dbDrawing);

			m_pDoc->AUTO_ssadd (ename, sset, sset);
			}
		} while (pDbHandItem);
		
	return m_pDoc->AUTO_cmd_wblock_entmake(sset, origin, name.GetBuffer(0), NULL, NULL);
	}


/////////////////////////////////////////////////////////////////////////////
// CAutoEnumBlockEnts

HRESULT CAutoEnumBlockEnts::Init (CAutoBlock *pBlock, db_block *pBlockHip)
	{
	m_pBlock = pBlock;
	m_pBlockHip = pBlockHip;
	m_nCurrent = 0;
	m_hipCurrent = m_pBlockHip->next;
	pBlock->get_Count (&m_nElements);
	return NOERROR;
	}

db_handitem * CAutoEnumBlockEnts::GetNextValidHip (db_handitem *pDbHandItem)
	{
	do
		{
		pDbHandItem = pDbHandItem->next;
		if (!pDbHandItem)
			return pDbHandItem;

		if ((UnsupportedEntity (pDbHandItem)) || (NotACountableEntity (pDbHandItem)) ||
			(UnimplementedEntity (pDbHandItem)))
			continue;
		else
			break;
		} while (pDbHandItem);
	
	return pDbHandItem;
	}

STDMETHODIMP CAutoEnumBlockEnts::Next (ULONG cElements, VARIANT * pvar, ULONG * pcElementFetched)
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
		hr = m_pBlock->CreateEntityWrapper (m_hipCurrent, &pEnt);
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


STDMETHODIMP CAutoEnumBlockEnts::Skip (ULONG cElements)
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


STDMETHODIMP CAutoEnumBlockEnts::Reset ()
	{
	m_nCurrent = 0;
	m_hipCurrent = m_pBlockHip->next;
	return NOERROR;
	}

STDMETHODIMP CAutoEnumBlockEnts::Clone (IEnumVARIANT * * ppenum)
	{
	if (ppenum == NULL)
		return E_POINTER;
	
	*ppenum = NULL;    
	CComObject<CAutoEnumBlockEnts>* penum;
	CComObject<CAutoEnumBlockEnts>::CreateInstance(&penum);
	if (!penum)
        return E_FAIL;    
	
	penum->Init(m_pBlock, m_pBlockHip);
	penum->AddRef();    
	
	*ppenum = penum;    
	return NOERROR;
	}



