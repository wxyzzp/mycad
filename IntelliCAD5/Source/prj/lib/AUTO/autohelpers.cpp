/* INTELLICAD\PRJ\LIB\AUTO\AUTOHELPERS.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * 
 * Abstract
 * 
 * Helper functions used by automation
 * 
 */ 

#include "stdafx.h"
#include "AutoHelpers.h"
#include "AutoLine.h"
#include "AutoArc.h"
#include "AutoCircle.h"
#include "AutoPolyline.h"
#include "AutoSpline.h"
#include "AutoPointEnt.h"
#include "AutoSolid.h"
#include "AutoTrace.h"
#include "AutoInfLine.h"
#include "AutoVector.h"
#include "AutoRay.h"
#include "Auto3dFace.h"
#include "AutoAttribute.h"
#include "AutoAttributeDef.h"
#include "AutoBlockInsert.h"
#include "AutoPViewport.h"
#include "AutoEllipse.h"
#include "AutoText.h"
#include "AutoMText.h"
#include "AutoTolerance.h"
#include "Auto3DPoly.h"
#include "AutoPolygonMesh.h"
#include "AutoPolyfaceMesh.h"
#include "AutoLeader.h"
#include "AutoImage.h"
#include "AutoXRef.h"
#include "AutoHatch.h"
#include "AutoLWPolyline.h"
#include "AutoSelSet.h"
#include "AutoDimension.h"
#include "AutoDimAngular.h"
#include "AutoDim3PointAngular.h"
#include "AutoDimRotated.h"
#include "AutoDimRadial.h"
#include "AutoDimOrdinate.h"
#include "AutoDimAligned.h"
#include "AutoDimDiametric.h"
#include "AutoShape.h"	// EBATECH(CNBR) 2002/4/24 SHAPE Support


HRESULT ConvertAutoSelectionSetToSDSsset (IIcadSelectionSet *SelectionSet, sds_name sset, CIcadDoc *pDoc)
	{
	long i, nCount;
	HRESULT hr = SelectionSet->get_Count (&nCount);
	if (FAILED(hr) || (nCount == 0))
		return E_INVALIDARG;

	long nhip;
    sds_name ename;
	CComPtr<IIcadEntity> pEnt;
	CComPtr<IDispatch> pDisp;

	pDoc->AUTO_ssadd (NULL, NULL, sset);

	for (i = 1; i <= nCount; i++)
		{
		hr = SelectionSet->Item (i, &pDisp);
		if (FAILED(hr))
			continue;

		hr = pDisp->QueryInterface (&pEnt);
		if (FAILED(hr))
			continue;

		hr = pEnt->_handitem (&nhip);
		if (FAILED(hr) || (nhip == 0))
			continue;

		pEnt = NULL;
		pDisp = NULL;

		ename[0] = nhip;
		ename[1] = (long)(pDoc->m_dbDrawing);

		pDoc->AUTO_ssadd (ename, sset, sset);
		}
	
	return NOERROR;
	}

HRESULT ConvertSDSssetToAutoSelectionSet (sds_name sset, IIcadSelectionSet **SelectionSet, CIcadDoc *pDoc)
	{
	HRESULT hr;
	long len;
	hr = pDoc->AUTO_sslength (sset, &len);
	if (FAILED(hr) || (len == 0))
		return E_INVALIDARG;

	CComObject<CAutoSelectionSet>* pSelectionSet;
	CComObject<CAutoSelectionSet>::CreateInstance(&pSelectionSet);
	if (!pSelectionSet)
		return E_FAIL;
	
	CString Name("_INTERNAL");
	pSelectionSet->Init(pDoc, Name);

	pSelectionSet->InitializeFromSDSsset (sset);

	return pSelectionSet->QueryInterface( SelectionSet );;
	}


void ConvertSdsMatrixToIIcadMatrix (sds_matrix sdsmatrix, IIcadMatrix *Matrix)
	{
	short row, column;
	for (row = 0; row < 4; row++)
		for (column = 0; column < 4; column++)
			Matrix->SetValue (row, column, sdsmatrix[row][column]);

	}

void ConvertIIcadMatrixToSdsMatrix (sds_matrix sdsmatrix, IIcadMatrix *Matrix)
	{
	short row, column;
	double x;

	for (row = 0; row < 4; row++)
		for (column = 0; column < 4; column++)
			{
			Matrix->GetValue (row, column, &x);
			sdsmatrix[row][column] = x;
			}
	}


db_handitem *CopyComplexHip (db_handitem *hip)
	{
	//copy the main hip
	db_handitem *newheadhip = hip->newcopy();
	newheadhip->SetHandle(NULL);	// copy constructor also copies handle; we need a new one.
	
	//copy all the vertices
	db_handitem *newhip = newheadhip;
	db_handitem *oldhip = hip->next;
	while ( (oldhip->ret_type() != DB_SEQEND) &&
			(oldhip->ret_type() != DB_ENDBLK) )
		{
		newhip->next = oldhip->newcopy();
		newhip->next->SetHandle(NULL);	// copy constructor also copies handle; we need a new one.
		newhip = newhip->next;
		oldhip = oldhip->next;
		if (oldhip == NULL)
			break;
		}

	//copy the sequend
	if (oldhip)
		{
		newhip->next = oldhip->newcopy();
		newhip->next->SetHandle(NULL);	// copy constructor also copies handle; we need a new one.
		}

	return newheadhip;
	}

db_handitem *CopyHip (db_handitem *hip)
	{
	db_handitem * newhip = NULL;
	int type = hip->ret_type();
	int fi1 = 0;

	switch (type)
		{
		case DB_POLYLINE:
			newhip = CopyComplexHip (hip);
			break;
	    // IntelliKorea 2001/05/18    
        // Block explode -> creates a new copy via the appropriate copy constructor.
		case DB_INSERT:
								// If block has attibutes then we need to
								// do a complex copy otherwise its just
								// a plain copy
			fi1 = 0;
			hip->get_66(&fi1);
			if (fi1)
			{
				newhip = CopyComplexHip (hip);
			}
			else
			{
				newhip = hip->newcopy();
				newhip->SetHandle(NULL);	// copy constructor also copies handle; we need a new one.
			}
			break;
        // End of modifiaction.  IntelliKorea 2001/05/18
		default:
			newhip = hip->newcopy();
			newhip->SetHandle(NULL);	// copy constructor also copies handle; we need a new one.
			break;
		}
										
	return newhip;
	}

bool AddToDrawing (CIcadDoc *pDoc, db_handitem *pDbHandItem)
	{
	db_handitem *hip, *savehip;
	hip = pDbHandItem->next;
	if (pDoc->m_dbDrawing->addhanditem (pDbHandItem) == 0)
		{
		if ((pDbHandItem->ret_type() == DB_INSERT) || (pDbHandItem->ret_type() == DB_POLYLINE))
			{
			while ( hip != NULL)
				{
				savehip = hip->next;
				if (pDoc->m_dbDrawing->addhanditem (hip) != 0)
					return false;	//fail if any of the sub ents were not added
				hip = savehip;
				}
			}
		pDoc->SetModifiedFlag(TRUE);
		return true;
		}
	else 
		return false;
	}


//Helper functions
HRESULT CreateEntityWrapper (CIcadDoc *pDoc, IDispatch *pOwner, db_handitem* pDbHandItem, IIcadEntity **ppEntity)
	{
	if (NULL == pDbHandItem) 
		return E_INVALIDARG;
	
	HRESULT hr;
	switch(pDbHandItem->ret_type())
		{
		case DB_3DFACE:		
			{
			CComObject<CAuto3DFace> *pEnt;
			CComObject<CAuto3DFace>::CreateInstance(&pEnt);
			if (!pEnt)
				return E_FAIL;
			
			hr = pEnt->Init (pDoc, pOwner, pDbHandItem);
			if (FAILED(hr))
				return hr;
			
			return pEnt->QueryInterface(ppEntity);
			
			break;
			}
		case DB_ARC:
			{
			CComObject<CAutoArc> *pEnt;
			CComObject<CAutoArc>::CreateInstance(&pEnt);
			if (!pEnt)
				return E_FAIL;
			
			hr = pEnt->Init (pDoc, pOwner, pDbHandItem);
			if (FAILED(hr))
				return hr;
			
			return pEnt->QueryInterface(ppEntity);
			
			break;
			}
		case DB_ATTDEF:		
			{
			CComObject<CAutoAttributeDef> *pEnt;
			CComObject<CAutoAttributeDef>::CreateInstance(&pEnt);
			if (!pEnt)
				return E_FAIL;
			
			hr = pEnt->Init (pDoc, pOwner, pDbHandItem);
			if (FAILED(hr))
				return hr;
			
			return pEnt->QueryInterface(ppEntity);
			
			break;
			}
		case DB_ATTRIB:
			{
			CComObject<CAutoAttribute> *pEnt;
			CComObject<CAutoAttribute>::CreateInstance(&pEnt);
			if (!pEnt)
				return E_FAIL;
			
			hr = pEnt->Init (pDoc, pOwner, pDbHandItem);
			if (FAILED(hr))
				return hr;
			
			return pEnt->QueryInterface(ppEntity);
			
			break;
			}
		case DB_INSERT:		
			{
			int flags;
			bool bIsXRef;

			db_blocktabrec *bthip = ((db_insert *)pDbHandItem)->ret_bthip();
			if (bthip == NULL)
				return E_FAIL;

			bthip->get_70 (&flags);
			
			bIsXRef = (flags & (IC_BLOCK_XREF | IC_BLOCK_XREF_OVERLAID)) ? true : false;

			if (bIsXRef)
				{
				CComObject<CAutoExternalReference> *pEnt;
				CComObject<CAutoExternalReference>::CreateInstance(&pEnt);
				if (!pEnt)
					return E_FAIL;
				
				hr = pEnt->Init (pDoc, pOwner, pDbHandItem);
				if (FAILED(hr))
					return hr;
				
				return pEnt->QueryInterface(ppEntity);
				}
			else
				{
				CComObject<CAutoBlockInsert> *pEnt;
				CComObject<CAutoBlockInsert>::CreateInstance(&pEnt);
				if (!pEnt)
					return E_FAIL;
				
				hr = pEnt->Init (pDoc, pOwner, pDbHandItem);
				if (FAILED(hr))
					return hr;
				
				return pEnt->QueryInterface(ppEntity);
				}
			
			break;
			}
		case DB_CIRCLE:
			{
			CComObject<CAutoCircle> *pEnt;
			CComObject<CAutoCircle>::CreateInstance(&pEnt);
			if (!pEnt)
				return E_FAIL;
			
			hr = pEnt->Init (pDoc, pOwner, pDbHandItem);
			if (FAILED(hr))
				return hr;
			
			return pEnt->QueryInterface(ppEntity);
			
			break;
			}
		case DB_DIMENSION:	
			{
			/*DG - 26.12.2001*/// Dimension inheritors implemented.
			int		dimType;
			pDbHandItem->get_70(&dimType);
			dimType &= 7;
			CComObject<CAutoDimension>*	pEnt;

			switch(dimType)
			{
			case 0  : CComObject<CAutoDimRotated>::CreateInstance((CComObject<CAutoDimRotated>**)&pEnt);
					  break;
			case 1  : CComObject<CAutoDimAligned>::CreateInstance((CComObject<CAutoDimAligned>**)&pEnt);
					  break;
			case 2  : CComObject<CAutoDimAngular>::CreateInstance((CComObject<CAutoDimAngular>**)&pEnt);
					  break;
			case 3  : CComObject<CAutoDimDiametric>::CreateInstance((CComObject<CAutoDimDiametric>**)&pEnt);
					  break;
			case 4  : CComObject<CAutoDimRadial>::CreateInstance((CComObject<CAutoDimRadial>**)&pEnt);
					  break;
			case 5  : CComObject<CAutoDim3PointAngular>::CreateInstance((CComObject<CAutoDim3PointAngular>**)&pEnt);
					  break;
			case 6  : CComObject<CAutoDimOrdinate>::CreateInstance((CComObject<CAutoDimOrdinate>**)&pEnt);
					  break;
			default : _ASSERTE(!"WARNING: Wrong type of the dimension!");
					  return E_FAIL;
			}

			if(!pEnt)
				return E_FAIL;
			
			hr = pEnt->Init(pDoc, pOwner, pDbHandItem);
			if(FAILED(hr))
				return hr;
			
			return pEnt->QueryInterface(ppEntity);
			
			break;
			}
		case DB_ELLIPSE:	
			{
			CComObject<CAutoEllipse> *pEnt;
			CComObject<CAutoEllipse>::CreateInstance(&pEnt);
			if (!pEnt)
				return E_FAIL;
			
			hr = pEnt->Init (pDoc, pOwner, pDbHandItem);
			if (FAILED(hr))
				return hr;
			
			return pEnt->QueryInterface(ppEntity);
			
			break;
			}
		case DB_HATCH:		
			{
			CComObject<CAutoHatch> *pEnt;
			CComObject<CAutoHatch>::CreateInstance(&pEnt);
			if (!pEnt)
				return E_FAIL;
			
			hr = pEnt->Init (pDoc, pOwner, pDbHandItem);
			if (FAILED(hr))
				return hr;
			
			return pEnt->QueryInterface(ppEntity);
			
			break;
			}
		case DB_IMAGE:		
			{
			CComObject<CAutoImage> *pEnt;
			CComObject<CAutoImage>::CreateInstance(&pEnt);
			if (!pEnt)
				return E_FAIL;
			
			hr = pEnt->Init (pDoc, pOwner, pDbHandItem);
			if (FAILED(hr))
				return hr;
			
			return pEnt->QueryInterface(ppEntity);
			
			break;
			}
		case DB_XLINE:		
			{
			CComObject<CAutoInfLine> *pEnt;
			CComObject<CAutoInfLine>::CreateInstance(&pEnt);
			if (!pEnt)
				return E_FAIL;
			
			hr = pEnt->Init (pDoc, pOwner, pDbHandItem);
			if (FAILED(hr))
				return hr;
			
			return pEnt->QueryInterface(ppEntity);
			
			break;
			}
		case DB_LEADER:		
			{
			CComObject<CAutoLeader> *pEnt;
			CComObject<CAutoLeader>::CreateInstance(&pEnt);
			if (!pEnt)
				return E_FAIL;
			
			hr = pEnt->Init (pDoc, pOwner, pDbHandItem);
			if (FAILED(hr))
				return hr;
			
			return pEnt->QueryInterface(ppEntity);
			
			break;
			}
		case DB_LINE:		
			{
			CComObject<CAutoLine> *pEnt;
			CComObject<CAutoLine>::CreateInstance(&pEnt);
			if (!pEnt)
				return E_FAIL;
			
			hr = pEnt->Init (pDoc, pOwner, pDbHandItem);
			if (FAILED(hr))
				return hr;
			
			return pEnt->QueryInterface(ppEntity);
			
			break;
			}
		case DB_LWPOLYLINE:
			{
			CComObject<CAutoLWPolyline> *pEnt;
			CComObject<CAutoLWPolyline>::CreateInstance(&pEnt);
			if (!pEnt)
				return E_FAIL;
			
			hr = pEnt->Init (pDoc, pOwner, pDbHandItem);
			if (FAILED(hr))
				return hr;
			
			return pEnt->QueryInterface(ppEntity);
			
			break;
			}
		case DB_MTEXT:		
			{
			CComObject<CAutoMText> *pEnt;
			CComObject<CAutoMText>::CreateInstance(&pEnt);
			if (!pEnt)
				return E_FAIL;
			
			hr = pEnt->Init (pDoc, pOwner, pDbHandItem);
			if (FAILED(hr))
				return hr;
			
			return pEnt->QueryInterface(ppEntity);
			
			break;
			}
		case DB_POINT:		
			{
			CComObject<CAutoPointEnt> *pEnt;
			CComObject<CAutoPointEnt>::CreateInstance(&pEnt);
			if (!pEnt)
				return E_FAIL;
			
			hr = pEnt->Init (pDoc, pOwner, pDbHandItem);
			if (FAILED(hr))
				return hr;
			
			return pEnt->QueryInterface(ppEntity);
			
			break;
			}
		case DB_POLYLINE:	
			{
			int flags;
			pDbHandItem->get_70 (&flags);
			
			if (flags & IC_PLINE_3DPLINE)
				{
				CComObject<CAuto3DPolyline> *pEnt;
				CComObject<CAuto3DPolyline>::CreateInstance(&pEnt);
				if (!pEnt)
					return E_FAIL;
				
				hr = pEnt->Init (pDoc, pOwner, pDbHandItem);
				if (FAILED(hr))
					return hr;
				
				return pEnt->QueryInterface(ppEntity);
				}
			else if (flags & IC_PLINE_3DMESH)
				{
				CComObject<CAutoPolygonMesh> *pEnt;
				CComObject<CAutoPolygonMesh>::CreateInstance(&pEnt);
				if (!pEnt)
					return E_FAIL;
				
				hr = pEnt->Init (pDoc, pOwner, pDbHandItem);
				if (FAILED(hr))
					return hr;
				
				return pEnt->QueryInterface(ppEntity);
				}
			else if (flags & IC_PLINE_POLYFACEMESH)
				{
				CComObject<CAutoPolyfaceMesh> *pEnt;
				CComObject<CAutoPolyfaceMesh>::CreateInstance(&pEnt);
				if (!pEnt)
					return E_FAIL;
				
				hr = pEnt->Init (pDoc, pOwner, pDbHandItem);
				if (FAILED(hr))
					return hr;
				
				return pEnt->QueryInterface(ppEntity);
				}
			else 
				{
				CComObject<CAutoPolyline> *pEnt;
				CComObject<CAutoPolyline>::CreateInstance(&pEnt);
				if (!pEnt)
					return E_FAIL;
				
				hr = pEnt->Init (pDoc, pOwner, pDbHandItem);
				if (FAILED(hr))
					return hr;
				
				return pEnt->QueryInterface(ppEntity);
				}
			break;
			}
		case DB_VIEWPORT:	
			{
			CComObject<CAutoPViewport> *pEnt;
			CComObject<CAutoPViewport>::CreateInstance(&pEnt);
			if (!pEnt)
				return E_FAIL;
			
			hr = pEnt->Init (pDoc, pOwner, pDbHandItem);
			if (FAILED(hr))
				return hr;
			
			return pEnt->QueryInterface(ppEntity);
			
			break;
			}
		case DB_RAY:		
			{
			CComObject<CAutoRay> *pEnt;
			CComObject<CAutoRay>::CreateInstance(&pEnt);
			if (!pEnt)
				return E_FAIL;
			
			hr = pEnt->Init (pDoc, pOwner, pDbHandItem);
			if (FAILED(hr))
				return hr;
			
			return pEnt->QueryInterface(ppEntity);
			
			break;
			}
		case DB_SOLID:		
			{
			CComObject<CAutoSolid> *pEnt;
			CComObject<CAutoSolid>::CreateInstance(&pEnt);
			if (!pEnt)
				return E_FAIL;
			
			hr = pEnt->Init (pDoc, pOwner, pDbHandItem);
			if (FAILED(hr))
				return hr;
			
			return pEnt->QueryInterface(ppEntity);
			
			break;
			}
		case DB_SPLINE:		
			{
			CComObject<CAutoSpline> *pEnt;
			CComObject<CAutoSpline>::CreateInstance(&pEnt);
			if (!pEnt)
				return E_FAIL;
			
			hr = pEnt->Init (pDoc, pOwner, pDbHandItem);
			if (FAILED(hr))
				return hr;
			
			return pEnt->QueryInterface(ppEntity);
			
			break;
			}
		case DB_TEXT:		
			{
			CComObject<CAutoText> *pEnt;
			CComObject<CAutoText>::CreateInstance(&pEnt);
			if (!pEnt)
				return E_FAIL;
			
			hr = pEnt->Init (pDoc, pOwner, pDbHandItem);
			if (FAILED(hr))
				return hr;
			
			return pEnt->QueryInterface(ppEntity);
			
			break;
			}
		case DB_TOLERANCE:	
			{
			CComObject<CAutoTolerance> *pEnt;
			CComObject<CAutoTolerance>::CreateInstance(&pEnt);
			if (!pEnt)
				return E_FAIL;
			
			hr = pEnt->Init (pDoc, pOwner, pDbHandItem);
			if (FAILED(hr))
				return hr;
			
			return pEnt->QueryInterface(ppEntity);
			
			break;
			}
		case DB_TRACE:		
			{
			CComObject<CAutoTrace> *pEnt;
			CComObject<CAutoTrace>::CreateInstance(&pEnt);
			if (!pEnt)
				return E_FAIL;
			
			hr = pEnt->Init (pDoc, pOwner, pDbHandItem);
			if (FAILED(hr))
				return hr;
			
			return pEnt->QueryInterface(ppEntity);
			
			break;
			}
		case DB_SHAPE:		// EBATECH(CNBR) 2002/4/24 -[ SHAPE Support
			{
			CComObject<CAutoShape> *pEnt;
			CComObject<CAutoShape>::CreateInstance(&pEnt);
			if (!pEnt)
				return E_FAIL;
			
			hr = pEnt->Init (pDoc, pOwner, pDbHandItem);
			if (FAILED(hr))
				return hr;
			
			return pEnt->QueryInterface(ppEntity);
			
			break;
			}				// EBATECH(CNBR) 2002/4/24 ]-
		default:
			{
			break;
			}
	}
	return E_FAIL;
}

BOOL UnsupportedEntity (db_handitem *pDbHandItem)
	{
	switch (pDbHandItem->ret_type()) //these are the types we don't support, so don't count them as entities.
		{
		case DB_3DSOLID		  :
		case DB_BODY          :
		case DB_OLE2FRAME     :
		case DB_REGION        :
		//case DB_SHAPE         :	// EBATECH(CNBR) 2002/4/24 SHAPE Support
		case DB_DICTIONARY    :
		case DB_DICTIONARYVAR :
		case DB_GROUP         :
		case DB_IMAGEDEF      :
		case DB_IMAGEDEF_REACTOR   :
		case DB_MLINE         :
		case DB_MLINESTYLE    :
		case DB_ACAD_PROXY_OBJECT  :
		case DB_ACAD_PROXY_ENTITY  :
			return TRUE;
		}
	return FALSE;
	}

BOOL NotACountableEntity (db_handitem *pDbHandItem)
	{
	switch (pDbHandItem->ret_type()) //these are the types we don't support, so don't count them as entities.
		{
		case DB_ENDBLK        :
		case DB_SEQEND		  :
		case DB_IDBUFFER      :
		case DB_RASTERVARIABLES    :
		case DB_SPATIAL_FILTER     :
		case DB_XRECORD       :
		case DB_PLOTSETTINGS  :
		case DB_LAYOUT        :
		case DB_PLACEHOLDER       :
		case DB_DICTIONARYWDFLT   :
		case DB_WIPEOUTVARIABLES   :
		case DB_VBAPROJECT :
		case DB_LAYER_INDEX   :
		case DB_OBJECT_PTR    :
		case DB_SORTENTSTABLE :
		case DB_SPATIAL_INDEX :
		case DB_VERTEX :
			return TRUE;
		}
	return FALSE;
	}

BOOL DeletedEntity (db_handitem *pDbHandItem)
	{
	return (pDbHandItem->ret_deleted());
	}

BOOL PaperSpaceEntity (db_handitem *pDbHandItem)
	{
	return (pDbHandItem->ret_pspace());
	}

BOOL ModelSpaceEntity (db_handitem *pDbHandItem)
	{
	return (!pDbHandItem->ret_pspace());
	}

BOOL UnimplementedEntity (db_handitem *pDbHandItem)
	{
/*
	switch (pDbHandItem->ret_type()) //these are the types we don't support, so don't count them as entities.
		{
		//case DB_3DFACE   :
		//case DB_ARC      :
		//case DB_ATTDEF   :
		//case DB_ATTRIB   :
		//case DB_BLOCK    :
		//case DB_CIRCLE   :
		//case DB_DIMENSION:
		//case DB_ELLIPSE  :
		//case DB_INSERT   :
		//case DB_LEADER   :
		//case DB_LINE     :
		//case DB_MTEXT    :
		//case DB_POINT    :
		//case DB_POLYLINE :
		//case DB_RAY      :
		//case DB_SOLID    :
		//case DB_SPLINE   :
		//case DB_TEXT     :
		//case DB_TOLERANCE:
		//case DB_TRACE    :
		//case DB_VIEWPORT :
		//case DB_XLINE    :
		//case DB_HATCH    :
		//case DB_IMAGE    :
		//case DB_LWPOLYLINE:
		//case DB_SHAPE    :	// EBATECH(CNBR) 2002/4/24 SHAPE Support
			return TRUE;
		}
*/
	return FALSE;
	}

BOOL FileExists (char *path)
	{
	if (path == NULL) 
		return FALSE;

	if ((-1) == access(path, 00))
		return FALSE;

	char drive[_MAX_DRIVE];   
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];   
	char ext[_MAX_EXT];
	fname[0] = '\0';
	ext[0] = '\0';

	_splitpath ( path, drive, dir, fname, ext );

	if ((fname[0] == '\0') || (ext[0] == '\0'))
		return FALSE;

	return TRUE;
	}


