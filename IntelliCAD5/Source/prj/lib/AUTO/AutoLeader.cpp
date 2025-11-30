/*INTELLICAD\PRJ\LIB\AUTO\AUTOLEADER.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * $Revision: 1.3.10.1 $ $Date: 2003/05/03 05:19:12 $ 
 * 
 * Abstract
 * 
 * Implementation of CAutoLeader - the Automation Leader object
 * 
 */ 

#include "StdAfx.h"
#include "AutoPoint.h"
#include "AutoVector.h"
#include "AutoLeader.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoLeader

HRESULT CAutoLeader::Init(CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem)
	{
	CAutoDbItem::Init(pDoc, pParent);
	
	if (pDbHandItem)
		m_pDbHandItem = pDbHandItem;
	else
		{
		if (pDoc)
			m_pDbHandItem = new db_leader(pDoc->m_dbDrawing);
		else
			m_pDbHandItem = new db_leader();
		}
	
	if (NULL == m_pDbHandItem) 
		return FALSE;
	
	return NOERROR;
	}

STDMETHODIMP CAutoLeader::get_Annotation (BSTR * pAnnot)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pAnnot == NULL)
		return E_POINTER;
	

	db_handitem *annothip = NULL;
	m_pDbHandItem->get_340 (&annothip);
	if (!annothip)
		{
		CString str = "";
		*pAnnot = str.AllocSysString();
		return NOERROR;
		}

	if (annothip->ret_type() == DB_INSERT)
		return E_FAIL; //cant get annotation from an insert

	char* pStr;
	if ((annothip->ret_type() == DB_MTEXT) || (annothip->ret_type() == DB_TOLERANCE))
		annothip->get_1 (&pStr);
		
	CString str (pStr);
	*pAnnot = str.AllocSysString();

	return NOERROR;
	}

STDMETHODIMP CAutoLeader::put_Annotation (BSTR pAnnot)
	{
	RETURN_IF_INVALID (m_pDbHandItem);

	CString str(pAnnot);

	db_handitem *annothip = NULL;
	m_pDbHandItem->get_340 (&annothip);
	if (!annothip)
		return E_FAIL;

	if (annothip->ret_type() == DB_INSERT)
		return E_FAIL; //cant get annotation from an insert

	if ((annothip->ret_type() == DB_MTEXT) || (annothip->ret_type() == DB_TOLERANCE))
		{
		if ( !annothip->set_1 (str.GetBuffer(0))) 
			return E_FAIL;
		}
	
	return NOERROR;
	}

STDMETHODIMP CAutoLeader::get_Coordinates (IIcadPoints * * ppCoords)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppCoords == NULL)
		return E_POINTER;
	
	CComObject<CAutoPoints> *pVertices;
	CComObject<CAutoPoints>::CreateInstance(&pVertices);
	if (!pVertices)
		return E_OUTOFMEMORY;


	CComPtr<IIcadPoint> pPoint = NULL;
    sds_point *verts;
	int nverts, i;

	verts = ((db_leader *)m_pDbHandItem)->retp_10array(); 
	
	m_pDbHandItem->get_76(&nverts);
	
	for (i = 0; i < nverts; i++)
		{
		pVertices->Add (verts[i][0], verts[i][1], verts[i][2], 0, &pPoint);
		if (pPoint == NULL)
			return E_OUTOFMEMORY;

		pPoint = NULL;
		}

	return pVertices->QueryInterface (IID_IIcadPoints, (void **)ppCoords);
	}

STDMETHODIMP CAutoLeader::put_Coordinates (IIcadPoints * ppCoords)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppCoords == NULL)
		return E_INVALIDARG;
	
	long nverts = 0, i;
	ppCoords->get_Count (&nverts);
	if (nverts < 2)
		return E_INVALIDARG;

	sds_point *verts = new sds_point [nverts];
	for (i = 0; i < nverts; i++)
		{
		CComPtr<IIcadPoint> pPoint;
		ppCoords->Item(i+1, &pPoint); //AutoPoints is 1 based indexing
		pPoint->get_x(&verts[i][0]);
		pPoint->get_y(&verts[i][1]);
		//Bugzilla No 78476 28-04-2003 [
		//pPoint->get_z(&verts[i][2]);
		if(i == 0)
		pPoint->get_z(&verts[i][2]);
		else
			verts[i][2] = verts[0][2];
		//Bugzilla No 78476 28-04-2003 ]
		pPoint = NULL;
		}

	if (!m_pDbHandItem->set_10 (verts, nverts))
		return E_FAIL;

	delete [] verts;
	return NOERROR;
	}

STDMETHODIMP CAutoLeader::get_Normal (IIcadVector * * ppExtrusionDirection)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppExtrusionDirection == NULL)
		return E_POINTER;
	
	sds_point v;
	m_pDbHandItem->get_210(v);
	
	CComObject<CAutoVector> *pVector;
	CComObject<CAutoVector>::CreateInstance(&pVector);
	if (!pVector)
		return E_FAIL;
	
	pVector->Init(NULL, v[0], v[1], v[2]);
	
	return pVector->QueryInterface (ppExtrusionDirection);
	}

STDMETHODIMP CAutoLeader::put_Normal (IIcadVector * ExtrusionDirection)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	sds_point v;
	v[0] = v[1] = 0.0; v[2] = 1.0;
	if (ExtrusionDirection)
		{
		ExtrusionDirection->get_x(&v[0]);
		ExtrusionDirection->get_y(&v[1]);
		ExtrusionDirection->get_z(&v[2]);

		if (db_unitizevector(v) != 0)
			return E_INVALIDARG;
		}
	m_pDbHandItem->init_210 (v);
	
	return NOERROR;
	}

STDMETHODIMP CAutoLeader::get_StyleName (BSTR * pStyleName)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pStyleName == NULL)
		return E_POINTER;
	
	char* pStr;
	m_pDbHandItem->get_3 (&pStr);
	
	CString str (pStr);
	*pStyleName = str.AllocSysString();
	return NOERROR;
	}

STDMETHODIMP CAutoLeader::put_StyleName (BSTR pStyleName)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	
	CString str(pStyleName);
	if (str.IsEmpty ())
		return E_INVALIDARG;

	if ( !m_pDbHandItem->set_3 ((char*)str.GetBuffer(0), m_pDoc->m_dbDrawing)) 
		return E_FAIL;
	return NOERROR;
	}

STDMETHODIMP CAutoLeader::get_Type (LeaderType * pType)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pType == NULL)
		return E_POINTER;
	
	int arrowtype, linetype;

	m_pDbHandItem->get_71 (&arrowtype);
	m_pDbHandItem->get_72 (&linetype);

	if (arrowtype == 0)	//no arrowhead
		{
		*pType = (linetype == 0) ? vicLineNoArrow : vicSplineNoArrow;
		}
	else if (arrowtype == 1)
		{
		*pType = (linetype == 0) ? vicLineWithArrow : vicSplineWithArrow;
		}
	else
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoLeader::put_Type (LeaderType pType)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	int arrowtype, linetype;

	switch (pType)
		{
		case vicLineNoArrow :
			arrowtype = 0;
			linetype = 0;
			break;
		case vicSplineNoArrow:
			arrowtype = 0;
			linetype = 1;
			break;
		case vicLineWithArrow :
			arrowtype = 1;
			linetype = 0;
			break;
		case vicSplineWithArrow:
			arrowtype = 1;
			linetype = 1;
			break;
		default:
			return E_FAIL;
		}

	m_pDbHandItem->set_71 (arrowtype);
	m_pDbHandItem->set_72 (linetype);

	return NOERROR;
	}



