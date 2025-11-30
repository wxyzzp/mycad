/* INTELLICAD\PRJ\LIB\AUTO\AUTOSELSET.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Implementation of CAutoSelectionSet - - the Automation SelectionSet object
 * 
 */ 

#include "StdAfx.h"
#include "AutoSelSet.h"
#include "AutoSelSets.h"
#include "AutoHelpers.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoSelectionSet

HRESULT CAutoSelectionSet::Init (CIcadDoc *pDoc, CString Name)
	{
	m_pDoc = pDoc;
	m_Name = Name;
	return NOERROR;
	}

HRESULT CAutoSelectionSet::InitializeFromSDSsset (sds_name sset)
	{
	HRESULT hr;
	long len;
	hr = m_pDoc->AUTO_sslength (sset, &len);
	if (FAILED(hr) || (len == 0))
		return E_INVALIDARG;

	CComPtr<IIcadEntity> pEntity;
	long nCount = 0;
	sds_name ename;

	if (m_Name.Compare("*ACTIVE") == 0) //it is the active sel set
		{
		for (int i = 0; i < m_Contents.GetSize(); i++)
			(m_Contents[i])->Release();
		m_Contents.RemoveAll();
		}
	else
		this->Clear();

	for ( long i = 0; m_pDoc->AUTO_ssname (sset, i, ename) == NOERROR; i++) 
		{
		pEntity = NULL;

		db_handitem* pDbHandItem;
		pDbHandItem = (db_handitem*)ename[0];

		hr = CreateEntityWrapper (m_pDoc, NULL, pDbHandItem, &pEntity);
		if (FAILED(hr))
            continue;     

		IDispatch *pDispEnt;
		hr = pEntity->QueryInterface(IID_IDispatch, (void **)&pDispEnt);
		if (FAILED(hr))
            continue;     

		Add(pDispEnt);
		}

	return NOERROR;
	}

HRESULT CAutoSelectionSet::Add (IDispatch *pDisp)
	{
	//this function will add an entity to the m_Contents array after checking to 
	//see whether it already exists in the array
	CComPtr<IIcadEntity> pEnt1;
	CComPtr<IIcadEntity> pEnt2;

	HRESULT hr;
	hr = pDisp->QueryInterface (&pEnt1);
	if (FAILED(hr))
		return E_FAIL;

	long hip1 = 0, hip2 = 0;
	hr = pEnt1->_handitem (&hip1);
	if (FAILED(hr))
		return E_FAIL;

	for (int i = 0; i < m_Contents.GetSize(); i++)
		{
		hr = (m_Contents[i])->QueryInterface (&pEnt2);
		if (FAILED(hr))
			return E_FAIL;

		hr = pEnt2->_handitem (&hip2);
		if (FAILED(hr))
			return E_FAIL;

		//if it is already in the array dont add it
		if (hip1 == hip2)
			return E_FAIL;

		pEnt2 = NULL;
		hip2 = 0;
		}

	m_Contents.Add(pDisp);
	return NOERROR;
	}

HRESULT CAutoSelectionSet::UpdateActiveSelectionSet ()
	{
	if (m_Name.Compare("*ACTIVE"))
		return NOERROR;

	HRESULT hr;
	sds_name ename;
	long hip = 0;

	//free the active selection set
	hr = m_pDoc->AUTO_ssfree (m_pDoc->m_pGripSelection);
	//may fail if there is no active sel set, ignore this failure

	CComPtr<IIcadEntity> pEnt;
	for (int i = 0; i < m_Contents.GetSize(); i++)
		{
		hr = (m_Contents[i])->QueryInterface (&pEnt);
		if (FAILED(hr))
			continue;

		hr = pEnt->_handitem (&hip);
		if (FAILED(hr))
			continue;
		
		ename[0] = hip;
		ename[1] = (long)m_pDoc->m_dbDrawing;

		if (i == 0)
			{
			//and initialize it
			hr = m_pDoc->AUTO_ssadd(NULL, NULL, m_pDoc->m_pGripSelection);
			if (FAILED(hr))
				return hr;
			}

		hr = m_pDoc->AUTO_ssadd (ename, m_pDoc->m_pGripSelection, m_pDoc->m_pGripSelection);
		if (FAILED(hr))
			continue;

		pEnt = NULL;
		hip = 0;
		}

	return NOERROR;
	}

STDMETHODIMP CAutoSelectionSet::get_Application (IIcadApplication * * ppApp)
	{
	if (ppApp == NULL)
		return E_POINTER;
	
	return GetApp()->m_pAutoApp->QueryInterface (ppApp);
	}

STDMETHODIMP CAutoSelectionSet::get_Count (long * pCount)
	{
	if (pCount == NULL)
		return E_POINTER;
	
	*pCount = m_Contents.GetSize();
	
	return NOERROR;
	}

STDMETHODIMP CAutoSelectionSet::get_Document (IIcadDocument * * ppDoc)
	{
	if (ppDoc == NULL)
		return E_POINTER;
	
	return m_pDoc->m_pAutoDoc->QueryInterface (ppDoc);
	}

STDMETHODIMP CAutoSelectionSet::get_Name (BSTR * pstrName)
	{
	if (pstrName == NULL)
		return E_POINTER;
	
	*pstrName = m_Name.AllocSysString();
	
	return NOERROR;
	}

STDMETHODIMP CAutoSelectionSet::get__NewEnum (IUnknown * * ppUnk)
	{
	if (ppUnk == NULL)
		return E_POINTER;
	
	CComObject<CAutoEnumSelSetEnts>* penum;
	CComObject<CAutoEnumSelSetEnts>::CreateInstance(&penum);
	if (!penum)
        return E_FAIL;    
	
	penum->Init(this);
	
	return penum->QueryInterface(IID_IUnknown, (void **)ppUnk);
	}

STDMETHODIMP CAutoSelectionSet::get_Parent (IIcadSelectionSets * * ppParent)
	{
	if (ppParent == NULL)
		return E_POINTER;
	
	CComPtr<IIcadDocument> pDoc;
	HRESULT hr = m_pDoc->m_pAutoDoc->QueryInterface (&pDoc);
	if (FAILED(hr))
		return hr;

	return pDoc->get_SelectionSets (ppParent);
	}

STDMETHODIMP CAutoSelectionSet::get_Color (Colors * pColor)
	{
	if (pColor == NULL)
		return E_POINTER;
	
	int i;
	Colors color, common_color;
	HRESULT hr;

	CComPtr<IIcadEntity> pEnt;
	for (i = 0; i < m_Contents.GetSize(); i++)
		{
		pEnt = NULL;
		hr = ((IDispatch *)m_Contents[i])->QueryInterface(&pEnt);

		if (SUCCEEDED(hr))
			{
			pEnt->get_Color (&color);
			
			if (i == 0)
				common_color = color;
			
			if (color != common_color)
				{
				common_color = vicVariousColors;
				break;
				}
			}
		}

	*pColor = common_color;
	return NOERROR;
	}

STDMETHODIMP CAutoSelectionSet::put_Color (Colors pColor)
	{
	int i;
	HRESULT hr;

	if (pColor < 0 || pColor > 256)
		return E_INVALIDARG;

	CComPtr<IIcadEntity> pEnt;
	for (i = 0; i < m_Contents.GetSize(); i++)
		{
		pEnt = NULL;
		hr = ((IDispatch *)m_Contents[i])->QueryInterface(&pEnt);

		if (SUCCEEDED(hr))
			pEnt->put_Color (pColor);
		}

	return NOERROR;
	}

STDMETHODIMP CAutoSelectionSet::get_Layer (BSTR * pLayer)
	{
	if (pLayer == NULL)
		return E_POINTER;
	
	int i;
	CString common_layer;
	BSTR layer;
	HRESULT hr;

	CComPtr<IIcadEntity> pEnt;
	for (i = 0; i < m_Contents.GetSize(); i++)
		{
		pEnt = NULL;
		hr = ((IDispatch *)m_Contents[i])->QueryInterface(&pEnt);

		if (SUCCEEDED(hr))
			{
			pEnt->get_Layer (&layer);
			
			if (i == 0)
				common_layer = layer;
			
			if (layer != common_layer)
				{
				common_layer = "";
				break;
				}
			}
		}

	*pLayer = common_layer.AllocSysString();
	return NOERROR;
	}

STDMETHODIMP CAutoSelectionSet::put_Layer (BSTR pLayer)
	{
	int i;
	HRESULT hr;

	CString str(pLayer);
	if (str.IsEmpty ())
		return E_INVALIDARG;

	CComPtr<IIcadEntity> pEnt;
	for (i = 0; i < m_Contents.GetSize(); i++)
		{
		pEnt = NULL;
		hr = ((IDispatch *)m_Contents[i])->QueryInterface(&pEnt);

		if (SUCCEEDED(hr))
			pEnt->put_Layer (pLayer);
		}

	return NOERROR;
	}

STDMETHODIMP CAutoSelectionSet::get_Linetype (BSTR * pLinetype)
	{
	if (pLinetype == NULL)
		return E_POINTER;
	
	int i;
	CString common_ltype;
	BSTR ltype;
	HRESULT hr;

	CComPtr<IIcadEntity> pEnt;
	for (i = 0; i < m_Contents.GetSize(); i++)
		{
		pEnt = NULL;
		hr = ((IDispatch *)m_Contents[i])->QueryInterface(&pEnt);

		if (SUCCEEDED(hr))
			{
			pEnt->get_Linetype (&ltype);
			
			if (i == 0)
				common_ltype = ltype;
			
			if (ltype != common_ltype)
				{
				common_ltype = "";
				break;
				}
			}
		}

	*pLinetype = common_ltype.AllocSysString();
	return NOERROR;
	}

STDMETHODIMP CAutoSelectionSet::put_Linetype (BSTR pLinetype)
	{
	int i;
	HRESULT hr;

	CString str(pLinetype);
	if (str.IsEmpty ())
		return E_INVALIDARG;

	CComPtr<IIcadEntity> pEnt;
	for (i = 0; i < m_Contents.GetSize(); i++)
		{
		pEnt = NULL;
		hr = ((IDispatch *)m_Contents[i])->QueryInterface(&pEnt);

		if (SUCCEEDED(hr))
			pEnt->put_Linetype (pLinetype);
		}

	return NOERROR;
	}

STDMETHODIMP CAutoSelectionSet::get_Visible (VARIANT_BOOL * pVisible)
	{
	if (pVisible == NULL)
		return E_POINTER;
	
	int i;
	VARIANT_BOOL visible, common_visible;
	HRESULT hr;

	CComPtr<IIcadEntity> pEnt;
	for (i = 0; i < m_Contents.GetSize(); i++)
		{
		pEnt = NULL;
		hr = ((IDispatch *)m_Contents[i])->QueryInterface(&pEnt);

		if (SUCCEEDED(hr))
			{
			pEnt->get_Visible (&visible);
			
			if (i == 0)
				common_visible = visible;
			
			if (visible != common_visible)
				{
				common_visible = -2;
				break;
				}
			}
		}

	*pVisible = common_visible;
	return NOERROR;
	}

STDMETHODIMP CAutoSelectionSet::put_Visible (VARIANT_BOOL pVisible)
	{
	int i;
	HRESULT hr;

	CComPtr<IIcadEntity> pEnt;
	for (i = 0; i < m_Contents.GetSize(); i++)
		{
		pEnt = NULL;
		hr = ((IDispatch *)m_Contents[i])->QueryInterface(&pEnt);

		if (SUCCEEDED(hr))
			pEnt->put_Visible (pVisible);
		}

	return NOERROR;
	}

STDMETHODIMP CAutoSelectionSet::AddItems (VARIANT Ents)
	{
	BOOL bVariants;

    if(V_VT(&Ents) == (VT_ARRAY | VT_VARIANT)) //ensure it is an array
		bVariants = TRUE;
	else if(V_VT(&Ents) == (VT_ARRAY | VT_DISPATCH))
		bVariants = FALSE;
	else
        return(E_INVALIDARG);
	
    if ((V_ARRAY(&Ents))->cDims != 1)	//ensure it is a one dimensional array
		return E_INVALIDARG;

    //if ((V_ARRAY(&Ents))->rgsabound[0].cElements == 0)	//ensure it has at least one element
	//	return E_INVALIDARG;

	SAFEARRAY *pSafeArray = V_ARRAY(&Ents);

	HRESULT hr;
	long LBound, UBound;
	hr = SafeArrayGetLBound(pSafeArray, 1, &LBound);
	if (FAILED(hr))
		return E_INVALIDARG;
	
	hr = SafeArrayGetUBound(pSafeArray, 1, &UBound);
	if (FAILED(hr))
		return E_INVALIDARG;

	long i;
	VARIANT var;
	IDispatch *pEntity, *pDisp;

	for (i = LBound; i <= UBound; i++)
		{
		if (bVariants)
			{
			pEntity = NULL;
			VariantInit(&var);

			hr = SafeArrayGetElement(V_ARRAY(&Ents), &i, &var);
			if (FAILED(hr))
				continue;

			ASSERT (V_VT(&var) == VT_DISPATCH);
			if (var.pdispVal == NULL)
				return E_INVALIDARG;

			if (V_VT(&var) == VT_DISPATCH)
				hr = V_DISPATCH(&var)->QueryInterface (&pEntity);
			
			VariantClear(&var);
			}
		else
			{
			hr = SafeArrayGetElement(V_ARRAY(&Ents), &i, (LPVOID *)&pDisp);
			if (FAILED(hr))
				continue;

			hr = pDisp->QueryInterface (&pEntity);
			}

		if (SUCCEEDED(hr) && pEntity)
			Add(pEntity);
		}

	UpdateActiveSelectionSet();

	return NOERROR;
	}

STDMETHODIMP CAutoSelectionSet::Clear ()
	{
	int i;
	for (i = 0; i < m_Contents.GetSize(); i++)
		{
		((IDispatch *)m_Contents[i])->Release();
		}
	m_Contents.RemoveAll();

	UpdateActiveSelectionSet();

	return NOERROR;
	}

STDMETHODIMP CAutoSelectionSet::Delete ()
	{
	int i;
	HRESULT hr;

	CComPtr<IIcadEntity> pEnt;
	for (i = 0; i < m_Contents.GetSize(); i++)
		{
		pEnt = NULL;
		hr = ((IDispatch *)m_Contents[i])->QueryInterface(&pEnt);

		if (SUCCEEDED(hr))
			pEnt->Delete ();
		}

	Clear();
	return NOERROR;
	}

STDMETHODIMP CAutoSelectionSet::Erase ()
	{
	int i;
	HRESULT hr;

	CComPtr<IIcadEntity> pEnt;
	for (i = 0; i < m_Contents.GetSize(); i++)
		{
		pEnt = NULL;
		hr = ((IDispatch *)m_Contents[i])->QueryInterface(&pEnt);

		if (SUCCEEDED(hr))
			pEnt->Delete ();
		}

	Clear();
	return NOERROR;
	}

STDMETHODIMP CAutoSelectionSet::Highlight (VARIANT_BOOL HighlightFlag)
	{
	int i;
	HRESULT hr;

	CComPtr<IIcadEntity> pEnt;
	for (i = 0; i < m_Contents.GetSize(); i++)
		{
		pEnt = NULL;
		hr = ((IDispatch *)m_Contents[i])->QueryInterface(&pEnt);

		if (SUCCEEDED(hr))
			pEnt->Highlight (HighlightFlag);
		}
	return NOERROR;
	}

STDMETHODIMP CAutoSelectionSet::Item (long Index, IDispatch * * ppItem)
	{
	if (ppItem == NULL)
		return E_POINTER;
	
	if (Index < 1 || Index > m_Contents.GetSize())
		return E_INVALIDARG;

	CComPtr<IDispatch> pEnt;
	pEnt = m_Contents[Index - 1];
	
	return pEnt->QueryInterface(ppItem);
	}

STDMETHODIMP CAutoSelectionSet::RemoveItems (VARIANT Ents)
	{
	if (!(Ents.vt & VT_ARRAY))
		return E_INVALIDARG;

	BOOL bDispatch = FALSE;
	if (Ents.vt == (VT_ARRAY | VT_DISPATCH))
		bDispatch = TRUE;
	if (Ents.vt == (VT_ARRAY | VT_VARIANT))
		bDispatch = FALSE;

	HRESULT hr;
	long LBound, UBound;
	hr = SafeArrayGetLBound(Ents.parray, 1, &LBound);
	if (FAILED(hr))
		return E_INVALIDARG;
	
	hr = SafeArrayGetUBound(Ents.parray, 1, &UBound);
	if (FAILED(hr))
		return E_INVALIDARG;
	
	long i;
	VARIANT var;
	IDispatch * pDisp;
	CComPtr<IDispatch> pDisp2;
	int nCount = m_Contents.GetSize();
	for (i = LBound; i <= UBound; i++)
		{
		if (bDispatch)
			{
			SafeArrayGetElement(Ents.parray, &i, &pDisp2);
			pDisp2->QueryInterface(&pDisp);	//this QI is reqd - the user may pass in any interface to the object, but we want the IDispatch
			}
		else
			{
			VariantInit (&var);
			SafeArrayGetElement(Ents.parray, &i, &var);

			ASSERT (var.vt == VT_DISPATCH);
			pDisp = var.pdispVal;
			}

		m_Contents.Remove (pDisp);
		pDisp->Release();

		if (bDispatch)
			{
			pDisp->Release();
			pDisp2 = NULL;
			}
		else
			VariantClear (&var);
		}

	UpdateActiveSelectionSet();

	return NOERROR;
	}

void GetSelectionTypeString (SelectionSetType Type, char *str)
	{
	switch (Type)
		{
		case vicSelectionSetCrossingPolygon	: strcpy (str, "CP"); break;
		case vicSelectionSetOutsidePolygon	: strcpy (str, "OP"); break;
		case vicSelectionSetInsidePolygon	: strcpy (str, "WP"); break;
		case vicSelectionSetCrossingCircle	: strcpy (str, "CC"); break;
		case vicSelectionSetOutsideCircle	: strcpy (str, "OC"); break;
		case vicSelectionSetInsideCircle	: strcpy (str, "WC"); break;
		case vicSelectionSetCrossingWindow	: strcpy (str, "C");  break;
		case vicSelectionSetOutsideWindow	: strcpy (str, "O");  break;
		case vicSelectionSetInsideWindow	: strcpy (str, "W");  break;
		case vicSelectionSetPrevious		: strcpy (str, "P");  break;
		case vicSelectionSetLast			: strcpy (str, "L");  break;
		case vicSelectionSetAll				: strcpy (str, "X");  break;
		case vicSelectionSetFence			: strcpy (str, "F");  break;
		default								: *str = NULL;   break;
		}
 	}

HRESULT GetFilterResbuf (VARIANT TypeArray, VARIANT DataArray, struct resbuf **rbFilter)
	{
	if (!(TypeArray.vt & VT_ARRAY))
		return E_INVALIDARG;
	
	if (!(DataArray.vt & VT_ARRAY))
		return E_INVALIDARG;
	
	HRESULT hr;
	long LBound, UBound;
	hr = SafeArrayGetLBound(TypeArray.parray, 1, &LBound);
	if (FAILED(hr))
		return E_INVALIDARG;
	
	hr = SafeArrayGetUBound(TypeArray.parray, 1, &UBound);
	if (FAILED(hr))
		return E_INVALIDARG;
	
	long LBound2, UBound2;
	hr = SafeArrayGetLBound(DataArray.parray, 1, &LBound2);
	if (FAILED(hr))
		return E_INVALIDARG;
	
	hr = SafeArrayGetUBound(DataArray.parray, 1, &UBound2);
	if (FAILED(hr))
		return E_INVALIDARG;

	if ((LBound != LBound2) || (UBound != UBound2))
		return E_INVALIDARG;

	struct resbuf *rb, *headrb;
	
	rb = headrb = sds_buildlist (-3,0);
	
	long i;
	short type, dtype;
	VARIANT var;
	for (i = LBound; i <= UBound; i++)
		{
		
		SafeArrayGetElement(TypeArray.parray, &i, &type);
		
		/*
		dtype = 1 - string
		dtype = 2 - 3dpoint
		dtype = 3 - double
		dtype = 4 - short
		dtype = 5 - long
		*/
		dtype = 0;
		if (type >= 0 && type <= 9)
			dtype = 1;
		if (type >= 1000 && type <= 1009)
			dtype = 1;
		if (type >= 10 && type <= 33)
			dtype = 2;
		if (type >= 1010 && type <= 1033)
			dtype = 2;
		if (type >= 34 && type <= 59)
			dtype = 3;
		if (type >= 1034 && type <= 1059)
			dtype = 3;
		if (type >= 60 && type <= 79)
			dtype = 4;
		if (type >= 1060 && type <= 1070)
			dtype = 4;
		if (type == 1071)
			dtype = 5;
		if (type < 0)
			dtype = 1;
		
		switch (dtype)
			{
			case 1:	//String
				{
				SafeArrayGetElement(DataArray.parray, &i, &var);
				ASSERT (var.vt == VT_BSTR);
				CString str(var.bstrVal);
				if (type == 0)
					type = SDS_RTDXF0; //to enable sds_buildlist to complete
				rb = rb->rbnext = sds_buildlist (type, str.GetBuffer(0), 0);
				}
				break;
			case 2: //3DPoint
				{
				SafeArrayGetElement(DataArray.parray, &i, &var);
				ASSERT (var.vt == VT_DISPATCH);
				CComPtr<IIcadPoint> Point;
				if (FAILED (var.pdispVal->QueryInterface (&Point)))
					return E_FAIL;

				sds_point pt;
				Point->get_x (&pt[0]);
				Point->get_y (&pt[1]);
				Point->get_z (&pt[2]);

				rb = rb->rbnext = sds_buildlist (type, pt, 0);
				}
				break;
			case 3: //Double
				SafeArrayGetElement(DataArray.parray, &i, &var);
				ASSERT (var.vt == VT_R8);
				rb = rb->rbnext = sds_buildlist (type, var.dblVal, 0);
				break;
			case 4: //Short
				SafeArrayGetElement(DataArray.parray, &i, &var);
				ASSERT (var.vt == VT_I2);
				rb = rb->rbnext = sds_buildlist (type, var.lVal, 0);
				break;
			case 5: //Long
				SafeArrayGetElement(DataArray.parray, &i, &var);
				ASSERT (var.vt == VT_I4);
				rb = rb->rbnext = sds_buildlist (type, var.lVal, 0);
				break;
			default:
				break;
			}
		}

	rb = headrb->rbnext;
	headrb->rbnext = NULL;
	sds_relrb (headrb);
	*rbFilter = rb;

	return NOERROR;
	}

STDMETHODIMP CAutoSelectionSet::Select (SelectionSetType Type, IIcadPoint * Point1, IIcadPoint * Point2, VARIANT FilterType, VARIANT FilterData)
	{
	if ((Type == vicSelectionSetCrossingPolygon) ||
		(Type == vicSelectionSetOutsidePolygon) ||
		(Type == vicSelectionSetInsidePolygon) ||
		(Type == vicSelectionSetFence) )
		return E_INVALIDARG;

	if ((Type == vicSelectionSetCrossingCircle) ||
		(Type == vicSelectionSetOutsideCircle) ||
		(Type == vicSelectionSetInsideCircle) ||
		(Type == vicSelectionSetCrossingWindow) ||
		(Type == vicSelectionSetOutsideWindow) ||
		(Type == vicSelectionSetInsideWindow) )
		{
		if ((Point1 == NULL) || (Point2 == NULL))
			return E_INVALIDARG;
		}

	char strType[3];
	GetSelectionTypeString (Type, strType);

	void *pFirstPoint = NULL, *pSecondPoint = NULL;
	sds_point pt1, pt2;

	if (Point1)
		{
		Point1->get_x (&pt1[0]);
		Point1->get_y (&pt1[1]);
		Point1->get_z (&pt1[2]);
		pFirstPoint = (void *)pt1;
		}

	if (Point2)
		{
		Point2->get_x (&pt2[0]);
		Point2->get_y (&pt2[1]);
		Point2->get_z (&pt2[2]);
		pSecondPoint = (void *)pt2;
		}

	sds_name sset, ename;
	sds_resbuf *rbFilter = NULL, *headrb = NULL;
	HRESULT hr;

	GetFilterResbuf (FilterType, FilterData, &rbFilter);

	if ((Type == vicSelectionSetCrossingCircle) ||
		(Type == vicSelectionSetOutsideCircle) ||
		(Type == vicSelectionSetInsideCircle) )
		{
		double rad = sqrt((pt2[0] - pt1[0])*(pt2[0] - pt1[0]) +
 						  (pt2[1] - pt1[1])*(pt2[1] - pt1[1]) +
						  (pt2[2] - pt1[2])*(pt2[2] - pt1[2]));

		headrb = sds_buildlist (RT3DPOINT, pt1, RTREAL, rad, 0);
		pFirstPoint = (void *)headrb;
		pSecondPoint = NULL;
		}
	
	m_pDoc->AUTO_pmtssget (NULL, strType, pFirstPoint, pSecondPoint, rbFilter, sset, 1);
	//dont check for failure, cause pmtssget returns failure if nothing was selected.  We want to handle "nothing selected" silently

	CComPtr<IIcadEntity> pEntity;
	IDispatch *pDispEnt;
	for ( long i = 0; m_pDoc->AUTO_ssname (sset, i, ename) == NOERROR; i++) 
		{
		db_handitem* pDbHandItem;
		pDbHandItem = (db_handitem*)ename[0];

		hr = CreateEntityWrapper (m_pDoc, NULL, pDbHandItem, &pEntity);
		if (FAILED(hr))
            continue;     

		hr = pEntity->QueryInterface(IID_IDispatch, (void **)&pDispEnt);
		if (FAILED(hr))
            continue;     

		if (pDispEnt)
			Add(pDispEnt);

		pEntity = NULL;
		}

	UpdateActiveSelectionSet();

	m_pDoc->AUTO_ssfree (sset);

	if (headrb)
		sds_relrb (headrb);
	return NOERROR;
	}

STDMETHODIMP CAutoSelectionSet::SelectAtPoint (IIcadPoint * Point, VARIANT FilterType, VARIANT FilterData)
	{
	if (Point == NULL)
		return E_INVALIDARG;

	void *pFirstPoint = NULL;
	sds_point pt;

	if (Point)
		{
		Point->get_x (&pt[0]);
		Point->get_y (&pt[1]);
		Point->get_z (&pt[2]);
		pFirstPoint = (void *)pt;
		}

	sds_name sset, ename;
	sds_resbuf *rbFilter = NULL;
	HRESULT hr;

	GetFilterResbuf (FilterType, FilterData, &rbFilter);
	
	m_pDoc->AUTO_pmtssget (NULL, NULL, pFirstPoint, NULL, rbFilter, sset, 1);
	//dont check for failure, cause pmtssget returns failure if nothing was selected.  We want to handle "nothing selected" silently

	CComPtr<IIcadEntity> pEntity;
	IDispatch *pDispEnt;
	for ( long i = 0; m_pDoc->AUTO_ssname (sset, i, ename) == NOERROR; i++) 
		{
		db_handitem* pDbHandItem;
		pDbHandItem = (db_handitem*)ename[0];

		hr = CreateEntityWrapper (m_pDoc, NULL, pDbHandItem, &pEntity);
		if (FAILED(hr))
            continue;     

		hr = pEntity->QueryInterface(IID_IDispatch, (void **)&pDispEnt);
		if (FAILED(hr))
            continue;     

		if (pDispEnt)
			Add(pDispEnt);

		pEntity = NULL;
		}
	
	m_pDoc->AUTO_ssfree (sset);

	UpdateActiveSelectionSet();

	return NOERROR;
	}

STDMETHODIMP CAutoSelectionSet::SelectByPolygon (SelectionSetType Type, IIcadPoints * PolygonPoints, VARIANT FilterType, VARIANT FilterData)
	{
	if (PolygonPoints == NULL)
		return E_INVALIDARG;

	long nVerts;
	PolygonPoints->get_Count (&nVerts);
	if (nVerts < 2)
		return E_INVALIDARG;

	if ((Type != vicSelectionSetCrossingPolygon) &&
		(Type != vicSelectionSetOutsidePolygon) &&
		(Type != vicSelectionSetInsidePolygon) &&
		(Type != vicSelectionSetFence) )
		return E_INVALIDARG;

	char strType[3];
	GetSelectionTypeString (Type, strType);

	CComPtr<IIcadPoint> pPoint;
	struct resbuf *rb, *rbhead;
	sds_point pt;

	for (long i = 0; i < nVerts; i++)
		{
		PolygonPoints->Item(i+1, &pPoint); //AutoPoints is 1 based indexing
		pPoint->get_x(&pt[0]);
		pPoint->get_y(&pt[1]);
		pPoint->get_z(&pt[2]);
		pPoint = NULL;
		
		if (i == 0)
			rb = rbhead = sds_buildlist (RT3DPOINT, pt, 0);
		else
			rb = rb->rbnext = sds_buildlist (RT3DPOINT, pt, 0);
		}

	sds_name sset, ename;
	sds_resbuf *rbFilter = NULL;
	HRESULT hr;

	GetFilterResbuf (FilterType, FilterData, &rbFilter);
	
	m_pDoc->AUTO_pmtssget (NULL, strType, rbhead, NULL, rbFilter, sset, 1);
	//dont check for failure, cause pmtssget returns failure if nothing was selected.  We want to handle "nothing selected" silently

	CComPtr<IIcadEntity> pEntity;
	IDispatch *pDispEnt;
	for ( i = 0; m_pDoc->AUTO_ssname (sset, i, ename) == NOERROR; i++) 
		{
		db_handitem* pDbHandItem;
		pDbHandItem = (db_handitem*)ename[0];

		hr = CreateEntityWrapper (m_pDoc, NULL, pDbHandItem, &pEntity);
		if (FAILED(hr))
            continue;     

		hr = pEntity->QueryInterface(IID_IDispatch, (void **)&pDispEnt);
		if (FAILED(hr))
            continue;     

		if (pDispEnt)
			Add(pDispEnt);

		pEntity = NULL;
		}
	
	m_pDoc->AUTO_ssfree (sset);
	sds_relrb (rbhead);

	UpdateActiveSelectionSet();

	return NOERROR;
	}

STDMETHODIMP CAutoSelectionSet::SelectOnScreen (VARIANT FilterType, VARIANT FilterData)
	{
	HRESULT hr;

	sds_name sset, ename;
	sds_resbuf *rbFilter = NULL;

	GetFilterResbuf (FilterType, FilterData, &rbFilter);

	hr = m_pDoc->AUTO_pmtssget ("Select Objects", NULL, NULL,  NULL, rbFilter, sset, 1);
	if (FAILED(hr))
		return E_FAIL;

	CComPtr<IIcadEntity> pEntity;
	IDispatch *pDispEnt;
	for ( long i = 0; m_pDoc->AUTO_ssname (sset, i, ename) == NOERROR; i++) 
		{
		db_handitem* pDbHandItem;
		pDbHandItem = (db_handitem*)ename[0];

		hr = CreateEntityWrapper (m_pDoc, NULL, pDbHandItem, &pEntity);
		if (FAILED(hr))
            continue;     

		hr = pEntity->QueryInterface(IID_IDispatch, (void **)&pDispEnt);
		if (FAILED(hr))
            continue;     

		if (pDispEnt)
			Add(pDispEnt);

		pEntity = NULL;
		}
	
	m_pDoc->AUTO_ssfree (sset);

	UpdateActiveSelectionSet();

	return NOERROR;
	}

STDMETHODIMP CAutoSelectionSet::Update ()
	{
	int i;
	HRESULT hr;

	CComPtr<IIcadEntity> pEnt;
	for (i = 0; i < m_Contents.GetSize(); i++)
		{
		pEnt = NULL;
		hr = ((IDispatch *)m_Contents[i])->QueryInterface(&pEnt);

		if (SUCCEEDED(hr))
			pEnt->Update ();
		}
	return NOERROR;
	}

STDMETHODIMP CAutoSelectionSet::Copy (IIcadSelectionSet * * ppCopy)
	{
	if (ppCopy == NULL)
		return E_POINTER;
	
	HRESULT hr;

	CComObject<CAutoSelectionSet>* pNewSelSet;
	CComObject<CAutoSelectionSet>::CreateInstance(&pNewSelSet);
	if (!pNewSelSet)
		return E_FAIL;
	
	pNewSelSet->Init(m_pDoc, "_INTERNAL");

	CComPtr<IIcadEntity> pEnt;
	CComPtr<IIcadEntity> pNewEnt;
	IDispatch * pDisp;
	for (long i = 0; i < m_Contents.GetSize(); i++)
		{
		pEnt = NULL;
		pNewEnt = NULL;
		hr = ((IDispatch *)m_Contents[i])->QueryInterface(&pEnt);

		if (SUCCEEDED(hr))
			hr = pEnt->Copy (&pNewEnt);

		if (SUCCEEDED(hr) && pNewEnt)
			{
			hr = pNewEnt->QueryInterface (&pDisp);
			if (SUCCEEDED(hr))
				pNewSelSet->Add (pDisp);
			}
		}

	return 	pNewSelSet->QueryInterface( ppCopy );
	}

STDMETHODIMP CAutoSelectionSet::Mirror (IIcadPoint * Point1, IIcadPoint * Point2, VARIANT_BOOL MakeCopy, IIcadSelectionSet * * ppCopy)
	{
	HRESULT hr;

	CComObject<CAutoSelectionSet>* pNewSelSet;
	CComObject<CAutoSelectionSet>::CreateInstance(&pNewSelSet);
	if (!pNewSelSet)
		return E_FAIL;
	
	pNewSelSet->Init(m_pDoc, "_INTERNAL");

	CComPtr<IIcadEntity> pEnt;
	CComPtr<IIcadEntity> pNewEnt;
	IDispatch * pDisp;
	for (long i = 0; i < m_Contents.GetSize(); i++)
		{
		pEnt = NULL;
		pNewEnt = NULL;
		hr = ((IDispatch *)m_Contents[i])->QueryInterface(&pEnt);

		if (SUCCEEDED(hr))
			hr = pEnt->Mirror (Point1, Point2, MakeCopy, &pNewEnt);

		if (SUCCEEDED(hr) && pNewEnt && (MakeCopy == VARIANT_TRUE))
			{
			hr = pNewEnt->QueryInterface (&pDisp);
			if (SUCCEEDED(hr))
				pNewSelSet->Add (pDisp);
			}
		}

	return 	pNewSelSet->QueryInterface( ppCopy );
	}

STDMETHODIMP CAutoSelectionSet::Mirror3D (IIcadPoint * Point1, IIcadPoint * Point2, IIcadPoint * Point3, VARIANT_BOOL MakeCopy, IIcadSelectionSet * * ppCopy)
	{
	HRESULT hr;

	CComObject<CAutoSelectionSet>* pNewSelSet;
	CComObject<CAutoSelectionSet>::CreateInstance(&pNewSelSet);
	if (!pNewSelSet)
		return E_FAIL;
	
	pNewSelSet->Init(m_pDoc, "_INTERNAL");

	CComPtr<IIcadEntity> pEnt;
	CComPtr<IIcadEntity> pNewEnt;
	IDispatch * pDisp;
	for (long i = 0; i < m_Contents.GetSize(); i++)
		{
		pEnt = NULL;
		pNewEnt = NULL;
		hr = ((IDispatch *)m_Contents[i])->QueryInterface(&pEnt);

		if (SUCCEEDED(hr))
			hr = pEnt->Mirror3D (Point1, Point2, Point3, MakeCopy, &pNewEnt);

		if (SUCCEEDED(hr) && pNewEnt && (MakeCopy == VARIANT_TRUE))
			{
			hr = pNewEnt->QueryInterface (&pDisp);
			if (SUCCEEDED(hr))
				pNewSelSet->Add (pDisp);
			}
		}

	return 	pNewSelSet->QueryInterface( ppCopy );
	}

STDMETHODIMP CAutoSelectionSet::Move (IIcadPoint * Point1, IIcadPoint * Point2)
	{
	int i;
	HRESULT hr;

	CComPtr<IIcadEntity> pEnt;
	for (i = 0; i < m_Contents.GetSize(); i++)
		{
		pEnt = NULL;
		hr = ((IDispatch *)m_Contents[i])->QueryInterface(&pEnt);

		if (SUCCEEDED(hr))
			pEnt->Move (Point1, Point2);
		}
	return NOERROR;
	}

STDMETHODIMP CAutoSelectionSet::Rotate (IIcadPoint * BasePoint, double RotationAngle)
	{
	int i;
	HRESULT hr;

	CComPtr<IIcadEntity> pEnt;
	for (i = 0; i < m_Contents.GetSize(); i++)
		{
		pEnt = NULL;
		hr = ((IDispatch *)m_Contents[i])->QueryInterface(&pEnt);

		if (SUCCEEDED(hr))
			pEnt->Rotate (BasePoint, RotationAngle);
		}
	return NOERROR;
	}

STDMETHODIMP CAutoSelectionSet::Rotate3D (IIcadPoint * Point1, IIcadPoint * Point2, double RotationAngle)
	{
	int i;
	HRESULT hr;

	CComPtr<IIcadEntity> pEnt;
	for (i = 0; i < m_Contents.GetSize(); i++)
		{
		pEnt = NULL;
		hr = ((IDispatch *)m_Contents[i])->QueryInterface(&pEnt);

		if (SUCCEEDED(hr))
			pEnt->Rotate3D (Point1, Point2, RotationAngle);
		}
	return NOERROR;
	}

STDMETHODIMP CAutoSelectionSet::ScaleEntity (IIcadPoint * BasePoint, double ScaleFactor)
	{
	int i;
	HRESULT hr;

	CComPtr<IIcadEntity> pEnt;
	for (i = 0; i < m_Contents.GetSize(); i++)
		{
		pEnt = NULL;
		hr = ((IDispatch *)m_Contents[i])->QueryInterface(&pEnt);

		if (SUCCEEDED(hr))
			pEnt->ScaleEntity (BasePoint, ScaleFactor);
		}
	return NOERROR;
	}

STDMETHODIMP CAutoSelectionSet::TransformBy (IIcadMatrix * Matrix)
	{
	int i;
	HRESULT hr;

	CComPtr<IIcadEntity> pEnt;
	for (i = 0; i < m_Contents.GetSize(); i++)
		{
		pEnt = NULL;
		hr = ((IDispatch *)m_Contents[i])->QueryInterface(&pEnt);

		if (SUCCEEDED(hr))
			pEnt->TransformBy (Matrix);
		}
	return NOERROR;
	}



/////////////////////////////////////////////////////////////////////////////
// CAutoEnumSelSetEnts

HRESULT CAutoEnumSelSetEnts::Init (IIcadSelectionSet *pSelSetEnts)
	{
	m_pSelSet = pSelSetEnts;
	m_nCurrent = 0;
	return NOERROR;
	}

STDMETHODIMP CAutoEnumSelSetEnts::Next (ULONG cElements, VARIANT * pvar, ULONG * pcElementFetched)
	{
	ULONG l;    
	long l1;    
	ULONG l2;
	
    if (pcElementFetched != NULL)
		*pcElementFetched = 0;
    
	for (l = 0; l < cElements; l++)
		VariantInit (&pvar[l]);
	
    // Retrieve the next cElements elements.
	long nElements = 0;
	m_pSelSet->get_Count(&nElements);
	
    for (l1 = m_nCurrent, l2 = 0; 
	l1 < nElements && l2 < cElements; 
	l1++, l2++)    
		{
		IDispatch * pDispEnt;
		if (FAILED (m_pSelSet->Item(l1 + 1, &pDispEnt)))
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
	
	return E_FAIL;
	}


STDMETHODIMP CAutoEnumSelSetEnts::Skip (ULONG cElements)
	{
    m_nCurrent += cElements;
	
	long nElements = 0;
	m_pSelSet->get_Count(&nElements);
	
	if (m_nCurrent > nElements)
		{        
		m_nCurrent = nElements;
        return ResultFromScode(S_FALSE);    
		}    
	else 
		return NOERROR;
	}


STDMETHODIMP CAutoEnumSelSetEnts::Reset ()
	{
	m_nCurrent = 0;
	return NOERROR;
	}

STDMETHODIMP CAutoEnumSelSetEnts::Clone (IEnumVARIANT * * ppenum)
	{
	if (ppenum == NULL)
		return E_POINTER;
	
	*ppenum = NULL;    
	CComObject<CAutoEnumSelSetEnts>* penum;
	CComObject<CAutoEnumSelSetEnts>::CreateInstance(&penum);
	if (!penum)
        return E_FAIL;    
	
	penum->Init(m_pSelSet);
	penum->AddRef();    
	
	*ppenum = penum;    
	return NOERROR;
	}

