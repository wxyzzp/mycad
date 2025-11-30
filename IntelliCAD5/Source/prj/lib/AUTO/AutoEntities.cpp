/* INTELLICAD\PRJ\LIB\AUTO\AUTOENTITIES.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Implementation of CAutoEntities
 * This is a helper class that is the base class for all objects that implement IIcadEntities
 * CAutoModelSpace, CAutoPaperSpace, CAutoBlock and CAutoReferenceFile all derive from this.
 * 
 */ 

#include "StdAfx.h"
#include "AutoEntities.h"
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
#include "AutoHelpers.h"
#include "AutoDimAngular.h"
#include "AutoDim3PointAngular.h"
#include "AutoDimRotated.h"
#include "AutoDimRadial.h"
#include "AutoDimOrdinate.h"
#include "AutoDimAligned.h"
#include "AutoDimDiametric.h"
#include "AutoShape.h"	// EBATECH(CNBR) 2002/4/24 Support SHAPE


/////////////////////////////////////////////////////////////////////////////
// CAutoEntities

STDMETHODIMP CAutoEntities::get_Application (IIcadApplication * * ppApp)
	{
	if (ppApp == NULL)
		return E_POINTER;
	
	return GetApp()->m_pAutoApp->QueryInterface (ppApp);
	}

STDMETHODIMP CAutoEntities::get_Count (long * pCount)
	{
	if (pCount == NULL)
		return E_POINTER;
	
	return E_FAIL;	/*DG - 26.12.2001*/// E_FAIL instead of E_NOTIMPL.
	}

STDMETHODIMP CAutoEntities::get_Parent (IIcadDocument * * ppParent)
	{
	if (ppParent == NULL)
		return E_POINTER;
	
	return E_FAIL;	/*DG - 26.12.2001*/// E_FAIL instead of E_NOTIMPL.
	}

STDMETHODIMP CAutoEntities::get__NewEnum (IUnknown * * ppUnk)
	{
	if (ppUnk == NULL)
		return E_POINTER;
	
	return E_FAIL;	/*DG - 26.12.2001*/// E_FAIL instead of E_NOTIMPL.
	}

STDMETHODIMP CAutoEntities::Item (long Index, IDispatch * * ppItem)
	{
	if (ppItem == NULL)
		return E_POINTER;
	
	return E_FAIL;	/*DG - 26.12.2001*/// E_FAIL instead of E_NOTIMPL.
	}

STDMETHODIMP CAutoEntities::Add3DFace (IIcadPoint * Point1, IIcadPoint * Point2, IIcadPoint * Point3, IIcadPoint * Point4, IIcad3DFace * * pp3DFace)
	{
	if (pp3DFace == NULL)
		return E_POINTER;
	
	CComObject<CAuto3DFace> *p3DFace;
	CComObject<CAuto3DFace>::CreateInstance(&p3DFace);
	if (!p3DFace)
		return E_FAIL;
	
	HRESULT hr;
	CComPtr<IDispatch> pOwner;
	hr = GetOwner (&pOwner.p);
	hr = p3DFace->Init(GetDocument(), pOwner, NULL);
	
	if (FAILED(hr))
		return hr;
		
	CComObject<CAutoPoints> *pVertices;
	CComObject<CAutoPoints>::CreateInstance(&pVertices);
	if (!pVertices)
		return E_OUTOFMEMORY;

	CComPtr<IIcadPoint> pIIcadPoint = NULL;

	double x, y, z;
	x = y = z = 0.0;
	if (Point1)
		{
		Point1->get_x (&x);
		Point1->get_y (&y);
		Point1->get_z (&z);
		}
	pVertices->Add (x, y, z, 0, &pIIcadPoint);
	pIIcadPoint = NULL;

	x = y = z = 0.0;
	if (Point2)
		{
		Point2->get_x (&x);
		Point2->get_y (&y);
		Point2->get_z (&z);
		}
	pVertices->Add (x, y, z, 0, &pIIcadPoint);
	pIIcadPoint = NULL;

	x = y = z = 0.0;
	if (Point3)
		{
		Point3->get_x (&x);
		Point3->get_y (&y);
		Point3->get_z (&z);
		}
	pVertices->Add (x, y, z, 0, &pIIcadPoint);
	pIIcadPoint = NULL;

	x = y = z = 0.0;
	if (Point4)
		{
		Point4->get_x (&x);
		Point4->get_y (&y);
		Point4->get_z (&z);
		}
	pVertices->Add (x, y, z, 0, &pIIcadPoint);
	pIIcadPoint = NULL;

	hr = p3DFace->put_Coordinates (pVertices);
	if (FAILED(hr))
		return hr;
	
	hr = AddItemToDrawing (p3DFace->m_pDbHandItem);
	if (SUCCEEDED(hr))
		return p3DFace->QueryInterface(pp3DFace);
	else
		return hr;
	}

STDMETHODIMP CAutoEntities::Add3DMesh (long M, long N, IIcadPoints * Vertices, IIcadPolygonMesh * * ppMesh)
	{
	if (ppMesh == NULL)
		return E_POINTER;
	
	CComObject<CAutoPolygonMesh> *pPolyline;
	CComObject<CAutoPolygonMesh>::CreateInstance(&pPolyline);
	if (!pPolyline)
		return E_FAIL;
	
	HRESULT hr;
	CComPtr<IDispatch> pOwner;
	hr = GetOwner (&pOwner.p);
	hr = pPolyline->Init(GetDocument(), pOwner, NULL);
	if (FAILED(hr))
		return hr;
	
	hr = pPolyline->put_Coordinates (Vertices);
	if (FAILED(hr))
		return hr;
	
	pPolyline->m_pDbHandItem->set_71 (M);
	pPolyline->m_pDbHandItem->set_72 (N);

	//All polylines have the 66 flag set to true
	pPolyline->m_pDbHandItem->set_66 (1);

	hr = AddItemToDrawing (pPolyline->m_pDbHandItem, TRUE);
	if (SUCCEEDED(hr))
		return pPolyline->QueryInterface(ppMesh);
	else
		return hr;
	}

STDMETHODIMP CAutoEntities::Add3DPoly (IIcadPoints * Vertices, IIcad3DPoly * * pp3DPoly)
	{
	if (pp3DPoly == NULL)
		return E_POINTER;
	
	CComObject<CAuto3DPolyline> *pPolyline;
	CComObject<CAuto3DPolyline>::CreateInstance(&pPolyline);
	if (!pPolyline)
		return E_FAIL;
	
	HRESULT hr;
	CComPtr<IDispatch> pOwner;
	hr = GetOwner (&pOwner.p);
	hr = pPolyline->Init(GetDocument(), pOwner, NULL);
	if (FAILED(hr))
		return hr;
	
	hr = pPolyline->put_Coordinates (Vertices);
	if (FAILED(hr))
		return hr;
	
	//All polylines have the 66 flag set to true
	pPolyline->m_pDbHandItem->set_66 (1);

	hr = AddItemToDrawing (pPolyline->m_pDbHandItem, TRUE);
	if (SUCCEEDED(hr))
		return pPolyline->QueryInterface(pp3DPoly);
	else
		return hr;
	}

STDMETHODIMP CAutoEntities::AddArc (IIcadPoint * Center, double Radius, double StartAngle, double EndAngle, IIcadArc * * ppArc)
	{
	if (ppArc == NULL)
		return E_POINTER;
	
	CComObject<CAutoArc> *pArc;
	CComObject<CAutoArc>::CreateInstance(&pArc);
	if (!pArc)
		return E_FAIL;
	
	HRESULT hr;
	CComPtr<IDispatch> pOwner;
	hr = GetOwner (&pOwner.p);
	hr = pArc->Init(GetDocument(), pOwner, NULL);
	if (FAILED(hr))
		return hr;
	
	hr = pArc->put_Center (Center);
	if (FAILED(hr))
		return hr;
	
	hr = pArc->put_Radius (Radius);
	if (FAILED(hr))
		return hr;
	
	hr = pArc->put_StartAngle (StartAngle);
	if (FAILED(hr))
		return hr;
	
	hr = pArc->put_EndAngle (EndAngle);
	if (FAILED(hr))
		return hr;
	
	hr = AddItemToDrawing (pArc->m_pDbHandItem);
	if (SUCCEEDED(hr))
		return pArc->QueryInterface(ppArc);
	else
		return hr;
	}

STDMETHODIMP CAutoEntities::AddAttributeDef (double Height, AttributeMode Mode, BSTR Prompt, IIcadPoint * InsertionPoint, BSTR Tag, BSTR Value, IIcadAttributeDef * * ppAttribute)
	{
	if (ppAttribute == NULL)
		return E_POINTER;
	
	CComObject<CAutoAttributeDef> *pAttdef;
	CComObject<CAutoAttributeDef>::CreateInstance(&pAttdef);
	if (!pAttdef)
		return E_FAIL;
	
	HRESULT hr;
	CComPtr<IDispatch> pOwner;
	hr = GetOwner (&pOwner.p);
	hr = pAttdef->Init(GetDocument(), pOwner, NULL);
	if (FAILED(hr))
		return hr;

	hr = pAttdef->put_Height (Height);
	if (FAILED(hr))
		return hr;
	
	hr = pAttdef->put_Mode (Mode);
	if (FAILED(hr))
		return hr;
	
	hr = pAttdef->put_PromptString (Prompt);
	if (FAILED(hr))
		return hr;
	
	hr = pAttdef->put_InsertionPoint (InsertionPoint);
	if (FAILED(hr))
		return hr;
	
	hr = pAttdef->put_TagString (Tag);
	if (FAILED(hr))
		return hr;
	
	hr = pAttdef->put_TextString (Value);
	if (FAILED(hr))
		return hr;

	hr = AddItemToDrawing (pAttdef->m_pDbHandItem);
	if (SUCCEEDED(hr))
		return pAttdef->QueryInterface(ppAttribute);
	else
		return hr;	
	}

STDMETHODIMP CAutoEntities::AddBox (IIcadPoint * Origin, double Length, double Width, double Height, IIcadPolygonMesh * * ppBox)
	{
	if (ppBox == NULL)
		return E_POINTER;
	
	sds_point pt1, pt2;
	pt1[0] = pt1[1] = pt1[2] = 0.0;
	pt2[0] = pt2[1] = pt2[2] = 1.0;

	if (Origin)
		{
		Origin->get_x (&pt1[0]);
		Origin->get_y (&pt1[1]);
		Origin->get_z (&pt1[2]);
		}
	pt2[0] = pt1[0] + Length;
	pt2[1] = pt1[1] + Width;
	pt2[2] = pt1[2];
	
	db_handitem *hip, *pNewHip;
	HRESULT hr;

	hr = GetDocument()->AUTO_cmd_box_create (pt1, pt2, Height, &hip);
	if (FAILED(hr))
		return E_FAIL;
	
	if (OwnerIsPaperSpace())
		{
		setComplexEntitySpaceFlag(hip, TRUE);
		}
	else if (OwnerIsModelSpace())
		{
		setComplexEntitySpaceFlag(hip, FALSE);
		}
	else if (OwnerIsBlock())
		{
		pNewHip = CopyComplexHip (hip);

		hr = AddItemToDrawing (pNewHip, TRUE);
		if (FAILED(hr))
			return E_FAIL;

		GetDocument()->DeleteEntity(hip);
		hip = pNewHip;
		}

	CComPtr<IIcadEntity> pEntity;
	hr = CreateEntityWrapper (hip, &pEntity);
	if (FAILED(hr))
		return E_FAIL;

	GetDocument()->SetModifiedFlag(TRUE);

	return pEntity->QueryInterface(ppBox);
	}

STDMETHODIMP CAutoEntities::AddCircle (IIcadPoint * Center, double Radius, IIcadCircle * * ppCircle)
	{
	if (ppCircle == NULL)
		return E_POINTER;
	
	CComObject<CAutoCircle> *pCircle;
	CComObject<CAutoCircle>::CreateInstance(&pCircle);
	if (!pCircle)
		return E_FAIL;
	
	HRESULT hr;
	CComPtr<IDispatch> pOwner;
	hr = GetOwner (&pOwner.p);
	hr = pCircle->Init (GetDocument(), pOwner, NULL);
	if (FAILED(hr))
		return hr;
	
	hr = pCircle->put_Center (Center);
	if (FAILED(hr))
		return hr;
	
	hr = pCircle->put_Radius (Radius);
	if (FAILED(hr))
		return hr;
	
	hr = AddItemToDrawing (pCircle->m_pDbHandItem);
	if (SUCCEEDED(hr))
		return pCircle->QueryInterface(ppCircle);
	else
		return hr;
	}

STDMETHODIMP CAutoEntities::AddCone (IIcadPoint * Center, double BaseRadius, double Height, IIcadPolygonMesh * * ppCone)
	{
	if (ppCone == NULL)
		return E_POINTER;
	
	sds_point pt1, pt2;
	pt1[0] = pt1[1] = pt1[2] = 0.0;

	if (Center)
		{
		Center->get_x (&pt1[0]);
		Center->get_y (&pt1[1]);
		Center->get_z (&pt1[2]);
		}
	pt2[0] = pt1[0];
	pt2[1] = pt1[1];
	pt2[2] = pt1[2] + Height;
	
	db_handitem *hip, *pNewHip;
	HRESULT hr;

	hr = GetDocument()->AUTO_cmd_cone_create (pt1, BaseRadius*2.0, 0.0, pt2, 16, &hip);
	if (FAILED(hr))
		return E_FAIL;

	if (OwnerIsPaperSpace())
		{
		setComplexEntitySpaceFlag(hip, TRUE);
		}
	else if (OwnerIsModelSpace())
		{
		setComplexEntitySpaceFlag(hip, FALSE);
		}
	else if (OwnerIsBlock())
		{
		pNewHip = hip->newcopy();

		hr = AddItemToDrawing (pNewHip, TRUE);
		if (FAILED(hr))
			return E_FAIL;

		GetDocument()->DeleteEntity(hip);
		hip = pNewHip;
		}

	CComPtr<IIcadEntity> pEntity;
	hr = CreateEntityWrapper (hip, &pEntity);
	if (FAILED(hr))
		return E_FAIL;

	GetDocument()->SetModifiedFlag(TRUE);

	return pEntity->QueryInterface(ppCone);
	}

STDMETHODIMP CAutoEntities::AddCylinder (IIcadPoint * Center, double Radius, double Height, IIcadPolygonMesh * * ppCylinder)
	{
	if (ppCylinder == NULL)
		return E_POINTER;
	
	sds_point pt1, pt2;
	pt1[0] = pt1[1] = pt1[2] = 0.0;

	if (Center)
		{
		Center->get_x (&pt1[0]);
		Center->get_y (&pt1[1]);
		Center->get_z (&pt1[2]);
		}
	pt2[0] = pt1[0];
	pt2[1] = pt1[1];
	pt2[2] = pt1[2] + Height;
	
	db_handitem *hip, *pNewHip;
	HRESULT hr;

	hr = GetDocument()->AUTO_cmd_cyl_create (pt1, Radius*2.0, pt2, 16, &hip);
	if (FAILED(hr))
		return E_FAIL;

	if (OwnerIsPaperSpace())
		{
		setComplexEntitySpaceFlag(hip, TRUE);
		}
	else if (OwnerIsModelSpace())
		{
		setComplexEntitySpaceFlag(hip, FALSE);
		}
	else if (OwnerIsBlock())
		{
		pNewHip = hip->newcopy();

		hr = AddItemToDrawing (pNewHip, TRUE);
		if (FAILED(hr))
			return E_FAIL;

		GetDocument()->DeleteEntity(hip);
		hip = pNewHip;
		}

	CComPtr<IIcadEntity> pEntity;
	hr = CreateEntityWrapper (hip, &pEntity);
	if (FAILED(hr))
		return E_FAIL;

	GetDocument()->SetModifiedFlag(TRUE);

	return pEntity->QueryInterface(ppCylinder);
	}

STDMETHODIMP CAutoEntities::AddDim3PointAngular(IIcadPoint* AngleVertex, IIcadPoint* FirstEndPoint, IIcadPoint* SecondEndPoint, IIcadPoint* TextLocation, IIcadDim3PointAngular** ppDimension)
{
	/*DG - 26.12.2001*/// Implemented the method.
	if(!ppDimension)
		return E_POINTER;
	
	CComObject<CAutoDim3PointAngular>*	pDim;
	CComObject<CAutoDim3PointAngular>::CreateInstance(&pDim);
	if(!pDim)
		return E_FAIL;
	
	HRESULT				hr;
	CComPtr<IDispatch>	pOwner;
	hr = GetOwner(&pOwner.p);
	hr = pDim->Init(GetDocument(), pOwner, NULL);
	if(FAILED(hr))
		return hr;

	if(!SUCCEEDED(hr = pDim->put_AngleVertex(AngleVertex)))			// dxf15
		return hr;
	if(!SUCCEEDED(hr = pDim->put_ExtLine1EndPoint(FirstEndPoint)))	// dxf13
		return hr;
	if(!SUCCEEDED(hr = pDim->put_ExtLine2EndPoint(SecondEndPoint)))	// dxf14
		return hr;
	if(!SUCCEEDED(hr = pDim->put_TextPosition(TextLocation)))		// dxf11
		return hr;

	sds_point	pt;
	TextLocation->get_x(&pt[0]);
	TextLocation->get_y(&pt[1]);
	TextLocation->get_z(&pt[2]);
	pDim->m_pDbHandItem->set_10(pt);

	hr = AddItemToDrawing(pDim->m_pDbHandItem);
	if(SUCCEEDED(hr))
		return pDim->QueryInterface(ppDimension);
	else
		return hr;
}

STDMETHODIMP CAutoEntities::AddDimAligned(IIcadPoint* ExtPoint1, IIcadPoint* ExtPoint2, IIcadPoint* TextLocation, IIcadDimAligned** ppDimension)
{
	/*DG - 26.12.2001*/// Implemented the method.
	if(!ppDimension)
		return E_POINTER;
	
	CComObject<CAutoDimAligned>*	pDim;
	CComObject<CAutoDimAligned>::CreateInstance(&pDim);
	if(!pDim)
		return E_FAIL;
	
	HRESULT				hr;
	CComPtr<IDispatch>	pOwner;
	hr = GetOwner(&pOwner.p);
	hr = pDim->Init(GetDocument(), pOwner, NULL);
	if(FAILED(hr))
		return hr;

	if(!SUCCEEDED(hr = pDim->put_ExtLine1Point(ExtPoint1)))		// dxf13
		return hr;
	if(!SUCCEEDED(hr = pDim->put_ExtLine2Point(ExtPoint2)))		// dxf14
		return hr;
	if(!SUCCEEDED(hr = pDim->put_TextPosition(TextLocation)))	// dxf11
		return hr;

	sds_point	pt;
	TextLocation->get_x(&pt[0]);
	TextLocation->get_y(&pt[1]);
	TextLocation->get_z(&pt[2]);
	pDim->m_pDbHandItem->set_10(pt);

	hr = AddItemToDrawing(pDim->m_pDbHandItem);
	if(SUCCEEDED(hr))
		return pDim->QueryInterface(ppDimension);
	else
		return hr;
}

STDMETHODIMP CAutoEntities::AddDimAngular(IIcadPoint* AngleVertex, IIcadPoint* FirstEndPoint, IIcadPoint* SecondEndPoint, IIcadPoint *TextLocation, IIcadDimAngular** ppDimension)
{
	/*DG - 26.12.2001*/// Implemented the method.
	if(!ppDimension)
		return E_POINTER;
	
	CComObject<CAutoDimAngular>*	pDim;
	CComObject<CAutoDimAngular>::CreateInstance(&pDim);
	if(!pDim)
		return E_FAIL;
	
	HRESULT				hr;
	CComPtr<IDispatch>	pOwner;
	hr = GetOwner(&pOwner.p);
	hr = pDim->Init(GetDocument(), pOwner, NULL);
	if(FAILED(hr))
		return hr;

	if(!SUCCEEDED(hr = pDim->put_ExtLine1StartPoint(FirstEndPoint)))	// dxf14
		return hr;
	if(!SUCCEEDED(hr = pDim->put_ExtLine1EndPoint(AngleVertex)))		// dxf13
		return hr;
	if(!SUCCEEDED(hr = pDim->put_ExtLine2StartPoint(SecondEndPoint)))	// dxf10
		return hr;
	if(!SUCCEEDED(hr = pDim->put_ExtLine2EndPoint(AngleVertex)))		// dxf15
		return hr;
	if(!SUCCEEDED(hr = pDim->put_TextPosition(TextLocation)))			// dxf11
		return hr;

	sds_point	pt;
	TextLocation->get_x(&pt[0]);
	TextLocation->get_y(&pt[1]);
	TextLocation->get_z(&pt[2]);
	pDim->m_pDbHandItem->set_16(pt);

	hr = AddItemToDrawing(pDim->m_pDbHandItem);
	if(SUCCEEDED(hr))
		return pDim->QueryInterface(ppDimension);
	else
		return hr;
}

STDMETHODIMP CAutoEntities::AddDimDiametric(IIcadPoint* ChordPoint, IIcadPoint* FarChordPoint, double LeaderLength, IIcadDimDiametric** ppDimension)
{
	/*DG - 26.12.2001*/// Implemented the method.
	if(!ppDimension)
		return E_POINTER;
	
	CComObject<CAutoDimDiametric>*	pDim;
	CComObject<CAutoDimDiametric>::CreateInstance(&pDim);
	if(!pDim)
		return E_FAIL;
	
	HRESULT				hr;
	CComPtr<IDispatch>	pOwner;
	hr = GetOwner(&pOwner.p);
	hr = pDim->Init(GetDocument(), pOwner, NULL);
	if(FAILED(hr))
		return hr;

	if(!SUCCEEDED(hr = pDim->put_LeaderLength(LeaderLength)))	// dxf40
		return hr;

	sds_point	pt;
	ChordPoint->get_x(&pt[0]);
	ChordPoint->get_y(&pt[1]);
	ChordPoint->get_z(&pt[2]);
	pDim->m_pDbHandItem->set_15(pt);

	FarChordPoint->get_x(&pt[0]);
	FarChordPoint->get_y(&pt[1]);
	FarChordPoint->get_z(&pt[2]);
	pDim->m_pDbHandItem->set_10(pt);

	hr = AddItemToDrawing(pDim->m_pDbHandItem);
	if(SUCCEEDED(hr))
		return pDim->QueryInterface(ppDimension);
	else
		return hr;
}

STDMETHODIMP CAutoEntities::AddDimOrdinate(IIcadPoint* DefintionPoint, IIcadPoint* LeaderEndPoint, VARIANT_BOOL UseXAxis, IIcadDimOrdinate** ppDimension)
{
	/*DG - 26.12.2001*/// Implemented the method.
	if(!ppDimension)
		return E_POINTER;
	
	CComObject<CAutoDimOrdinate>*	pDim;
	CComObject<CAutoDimOrdinate>::CreateInstance(&pDim);
	if(!pDim)
		return E_FAIL;
	
	HRESULT				hr;
	CComPtr<IDispatch>	pOwner;
	hr = GetOwner(&pOwner.p);
	hr = pDim->Init(GetDocument(), pOwner, NULL);
	if(FAILED(hr))
		return hr;

	sds_point	pt;
	DefintionPoint->get_x(&pt[0]);
	DefintionPoint->get_y(&pt[1]);
	DefintionPoint->get_z(&pt[2]);
	pDim->m_pDbHandItem->set_13(pt);

	LeaderEndPoint->get_x(&pt[0]);
	LeaderEndPoint->get_y(&pt[1]);
	LeaderEndPoint->get_z(&pt[2]);
	pDim->m_pDbHandItem->set_14(pt);

	if(UseXAxis == VARIANT_TRUE)
		pDim->m_pDbHandItem->set_70(70);

	hr = AddItemToDrawing(pDim->m_pDbHandItem);
	if(SUCCEEDED(hr))
		return pDim->QueryInterface(ppDimension);
	else
		return hr;
}

STDMETHODIMP CAutoEntities::AddDimRadial(IIcadPoint* Center, IIcadPoint* ChordPoint, double LeaderLength, IIcadDimRadial** ppDimension)
{
	/*DG - 26.12.2001*/// Implemented the method.
	if(!ppDimension)
		return E_POINTER;
	
	CComObject<CAutoDimRadial>*	pDim;
	CComObject<CAutoDimRadial>::CreateInstance(&pDim);
	if(!pDim)
		return E_FAIL;
	
	HRESULT				hr;
	CComPtr<IDispatch>	pOwner;
	hr = GetOwner(&pOwner.p);
	hr = pDim->Init(GetDocument(), pOwner, NULL);
	if(FAILED(hr))
		return hr;

	if(!SUCCEEDED(hr = pDim->put_LeaderLength(LeaderLength)))	// dxf40
		return hr;

	sds_point	pt;
	ChordPoint->get_x(&pt[0]);
	ChordPoint->get_y(&pt[1]);
	ChordPoint->get_z(&pt[2]);
	pDim->m_pDbHandItem->set_15(pt);

	Center->get_x(&pt[0]);
	Center->get_y(&pt[1]);
	Center->get_z(&pt[2]);
	pDim->m_pDbHandItem->set_10(pt);

	hr = AddItemToDrawing(pDim->m_pDbHandItem);
	if(SUCCEEDED(hr))
		return pDim->QueryInterface(ppDimension);
	else
		return hr;
}

STDMETHODIMP CAutoEntities::AddDimRotated(IIcadPoint* ExtPoint1, IIcadPoint* ExtPoint2, IIcadPoint* DimLineLocation, double Rotation, IIcadDimRotated** ppDimension)
{
	/*DG - 26.12.2001*/// Implemented the method.
	if(!ppDimension)
		return E_POINTER;
	
	CComObject<CAutoDimRotated>*	pDim;
	CComObject<CAutoDimRotated>::CreateInstance(&pDim);
	if(!pDim)
		return E_FAIL;
	
	HRESULT				hr;
	CComPtr<IDispatch>	pOwner;
	hr = GetOwner(&pOwner.p);
	hr = pDim->Init(GetDocument(), pOwner, NULL);
	if(FAILED(hr))
		return hr;

	sds_point	pt;
	ExtPoint1->get_x(&pt[0]);
	ExtPoint1->get_y(&pt[1]);
	ExtPoint1->get_z(&pt[2]);
	pDim->m_pDbHandItem->set_13(pt);

	ExtPoint2->get_x(&pt[0]);
	ExtPoint2->get_y(&pt[1]);
	ExtPoint2->get_z(&pt[2]);
	pDim->m_pDbHandItem->set_14(pt);

	DimLineLocation->get_x(&pt[0]);
	DimLineLocation->get_y(&pt[1]);
	DimLineLocation->get_z(&pt[2]);
	pDim->m_pDbHandItem->set_10(pt);

	pDim->m_pDbHandItem->set_40(Rotation);

	hr = AddItemToDrawing(pDim->m_pDbHandItem);
	if(SUCCEEDED(hr))
		return pDim->QueryInterface(ppDimension);
	else
		return hr;
}

STDMETHODIMP CAutoEntities::AddEllipse (IIcadPoint * Center, IIcadVector * MajorAxis, double RadiusRatio, IIcadEllipse * * ppEllipse)
	{
	if (ppEllipse == NULL)
		return E_POINTER;
	
	CComObject<CAutoEllipse> *pEllipse;
	CComObject<CAutoEllipse>::CreateInstance(&pEllipse);
	if (!pEllipse)
		return E_FAIL;
	
	HRESULT hr;
	CComPtr<IDispatch> pOwner;
	hr = GetOwner (&pOwner.p);
	hr = pEllipse->Init (GetDocument(), pOwner, NULL);
	if (FAILED(hr))
		return hr;
	
	hr = pEllipse->put_Center (Center);
	if (FAILED(hr))
		return hr;
	
	hr = pEllipse->put_MajorAxis (MajorAxis);
	if (FAILED(hr))
		return hr;
	
	hr = pEllipse->put_RadiusRatio (RadiusRatio);
	if (FAILED(hr))
		return hr;
	
	hr = AddItemToDrawing (pEllipse->m_pDbHandItem);
	if (SUCCEEDED(hr))
		return pEllipse->QueryInterface(ppEllipse);
	else
		return hr;
	}

STDMETHODIMP CAutoEntities::AddHatch (HatchType PatternType, BSTR PatternName, VARIANT_BOOL Associativity, IIcadHatch * * ppHatch)
	{
	if (ppHatch == NULL)
		return E_POINTER;
	
	CComObject<CAutoHatch> *pHatch;
	CComObject<CAutoHatch>::CreateInstance(&pHatch);
	if (!pHatch)
		return E_FAIL;
	
	HRESULT hr;
	CComPtr<IDispatch> pOwner;
	hr = GetOwner (&pOwner.p);
	hr = pHatch->Init (GetDocument(), pOwner, NULL);
	if (FAILED(hr))
		return hr;
	
	hr = pHatch->put_PatternType (PatternType);
	if (FAILED(hr))
		return hr;
	
	hr = pHatch->put_PatternName (PatternName);
	if (FAILED(hr))
		return hr;
	
	int flags = (Associativity == VARIANT_TRUE) ? 1 : 0;
	pHatch->m_pDbHandItem->set_71 (flags);

	pHatch->m_pOwner = this;
	/*
	hr = AddItemToDrawing (pHatch->m_pDbHandItem);
	if (SUCCEEDED(hr))
		return pHatch->QueryInterface(ppHatch);
	else
		return hr;
	*/
	return pHatch->QueryInterface(ppHatch);
	}

STDMETHODIMP CAutoEntities::AddImage (BSTR ImageFile, IIcadPoint * InsertionPoint, double ScaleFactor, double RotationAngle, IIcadImage * * ppImage)
	{
	if (ppImage == NULL)
		return E_POINTER;

	//ScaleFactor is not currently supported, so return an error if it is not 1.0
	if (!icadRealEqual(ScaleFactor,1.0))
		return E_INVALIDARG;

	//RotationAngle is not currently supported, so return an error if it is not 0.0
	if (!icadAngleEqual(RotationAngle,0.0))
		return E_INVALIDARG;
	
	sds_point pt1;
	pt1[0] = pt1[1] = pt1[2] = 0.0;

	if (InsertionPoint)
		{
		InsertionPoint->get_x (&pt1[0]);
		InsertionPoint->get_y (&pt1[1]);
		InsertionPoint->get_z (&pt1[2]);
		}
	
	db_handitem *hip, *pNewHip;
	HRESULT hr;
	CString ImageFileName (ImageFile);

	CString DictName(ImageFile);
	int idxChar = DictName.ReverseFind('\\');
	if(idxChar!=(-1))
		{
		int n = DictName.GetLength() - idxChar - 1;
		DictName = DictName.Right(n);
		}
	
	idxChar = DictName.ReverseFind('.');
	if(idxChar!=(-1))
		{
		DictName = DictName.Left(idxChar);
		}

	if (DictName.IsEmpty())
		DictName = "Image";

	hr = GetDocument()->CreateImage (ImageFileName, DictName, pt1, &hip);
	if (FAILED(hr))
		return E_FAIL;

	if (OwnerIsPaperSpace())
		{
		setComplexEntitySpaceFlag(hip, TRUE);
		}
	else if (OwnerIsModelSpace())
		{
		setComplexEntitySpaceFlag(hip, FALSE);
		}
	else if (OwnerIsBlock())
		{
		pNewHip = hip->newcopy();

		hr = AddItemToDrawing (pNewHip, TRUE);
		if (FAILED(hr))
			return E_FAIL;

		GetDocument()->DeleteEntity(hip);
		hip = pNewHip;
		}

	CComPtr<IIcadEntity> pEntity;
	hr = CreateEntityWrapper (hip, &pEntity);
	if (FAILED(hr))
		return E_FAIL;

	GetDocument()->SetModifiedFlag(TRUE);

	return pEntity->QueryInterface(ppImage);
	}

STDMETHODIMP CAutoEntities::AddInfiniteLine (IIcadPoint * Point1, IIcadPoint * Point2, IIcadInfiniteLine * * ppInfiniteLine)
	{
	if (ppInfiniteLine == NULL)
		return E_POINTER;
	
	CComObject<CAutoInfLine> *pLine;
	CComObject<CAutoInfLine>::CreateInstance(&pLine);
	if (!pLine)
		return E_FAIL;
	
	HRESULT hr;
	CComPtr<IDispatch> pOwner;
	hr = GetOwner (&pOwner.p);
	hr = pLine->Init(GetDocument(), pOwner, NULL);
	
	if (FAILED(hr))
		return hr;
	
	hr = pLine->put_BasePoint(Point1);
	if (FAILED(hr))
		return hr;

	CComPtr<IIcadVector> ppDirection;
	if (Point1 && Point2)
		{
		CComObject<CAutoVector> *pVector;
		CComObject<CAutoVector>::CreateInstance(&pVector);
		if (!pVector)
			return E_FAIL;
		
		double x1, y1, z1;
		Point1->get_x (&x1);
		Point1->get_y (&y1);
		Point1->get_z (&z1);

		double x2, y2, z2;
		Point2->get_x (&x2);
		Point2->get_y (&y2);
		Point2->get_z (&z2);
		
		double len = sqrt ((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1) + (z2 - z1)*(z2 - z1)) ;
		
		pVector->Init(NULL, (x2 - x1)/len, (y2 - y1)/len, (z2 - z1)/len);
		
		pVector->QueryInterface (&ppDirection);
		}
	
	hr = pLine->put_DirectionVector(ppDirection);
	if (FAILED(hr))
		return hr;
	
	hr = AddItemToDrawing (pLine->m_pDbHandItem);
	if (SUCCEEDED(hr))
		return pLine->QueryInterface(ppInfiniteLine);
	else
		return hr;
	
	}

STDMETHODIMP CAutoEntities::AddLeader(IIcadPoints* Points, BSTR Annotation, LeaderType Type, IIcadLeader** ppLeader)
{
	if(ppLeader == NULL)
		return E_POINTER;

	CComObject<CAutoLeader>*	pLeader;
	CComObject<CAutoLeader>::CreateInstance(&pLeader);
	if(!pLeader)
		return E_FAIL;

	HRESULT				hr;
	CComPtr<IDispatch>	pOwner;
	hr = GetOwner(&pOwner.p);
	hr = pLeader->Init(GetDocument(), pOwner, NULL);

	if(FAILED(hr))
		return hr;

	hr = pLeader->put_Coordinates(Points);
	if(FAILED(hr))
		return hr;

	/*DG - 1.11.2001*/// Add annotation mtext.
	CComPtr<IIcadPoint>	pInsPoint = NULL;
	long				vertsNum = 0;
	IIcadMText*			pImtext = NULL;
	long				mtext = NULL;

	Points->get_Count(&vertsNum);
	if(FAILED(hr = Points->Item(vertsNum, &pInsPoint)))
		return hr;
	
	//Bugzilla No 78476 28-04-2003 [
	sds_point verts[2];
	pInsPoint->get_z(&verts[1][2]); // The z-coordinate of Insertion Point
	CComPtr<IIcadPoint>	pFirstPoint = NULL;
	if(FAILED(hr = Points->Item(1, &pFirstPoint)))
		return hr;
	pFirstPoint->get_z(&verts[0][2]); // The z-coordinate of First Point
	pInsPoint->put_z(verts[0][2]); // Set z-coordinate of Insertion Point 
								   // equal to z-coordinate of First Point
	//Bugzilla No 78476 28-04-2003 ]
	if(FAILED(hr = AddMText(pInsPoint, 0, Annotation, &pImtext)))
		return hr;
	//Bugzilla No 78476 28-04-2003
	pInsPoint->put_z(verts[1][2]); // Reset z-coordinate of Insertion Point to actual value 
	pFirstPoint = NULL;
	
	pImtext->_handitem(&mtext);
	if(!pLeader->m_pDbHandItem->set_340((db_handitem*)mtext))
	{
		pImtext->Delete();
		pImtext->Release();
		return E_FAIL;
	}
	pImtext->Release();

	hr = pLeader->put_Annotation(Annotation);
	if(FAILED(hr))
	{
		pImtext->Delete();
		return hr;
	}
	
	hr = pLeader->put_Type(Type);
	if(FAILED(hr))
	{
		pImtext->Delete();
		return hr;
	}
	
	hr = AddItemToDrawing(pLeader->m_pDbHandItem);
	if(SUCCEEDED(hr))
		return pLeader->QueryInterface(ppLeader);
	else
	{
		pImtext->Delete();
		return hr;
	}
}

STDMETHODIMP CAutoEntities::AddLightWeightPolyline (IIcadPoints * Vertices, IIcadLightWeightPolyline * * ppLWPolyline)
	{
	if (ppLWPolyline == NULL)
		return E_POINTER;
	
	CComObject<CAutoLWPolyline> *pLWPoly;
	CComObject<CAutoLWPolyline>::CreateInstance(&pLWPoly);
	if (!pLWPoly)
		return E_FAIL;
	
	HRESULT hr;
	CComPtr<IDispatch> pOwner;
	hr = GetOwner (&pOwner.p);
	hr = pLWPoly->Init(GetDocument(), pOwner, NULL);
	
	if (FAILED(hr))
		return hr;

	hr = pLWPoly->put_Coordinates (Vertices);
	if (FAILED(hr))
		return hr;
		
	hr = AddItemToDrawing (pLWPoly->m_pDbHandItem);
	if (SUCCEEDED(hr))
		return pLWPoly->QueryInterface(ppLWPolyline);
	else
		return hr;
	}

STDMETHODIMP CAutoEntities::AddLine (IIcadPoint * StartPoint, IIcadPoint * EndPoint, IIcadLine * * ppLine)
	{
	if (ppLine == NULL)
		return E_POINTER;
	
	CComObject<CAutoLine> *pLine;
	CComObject<CAutoLine>::CreateInstance(&pLine);
	if (!pLine)
		return E_FAIL;
	
	HRESULT hr;
	CComPtr<IDispatch> pOwner;
	hr = GetOwner (&pOwner.p);
	hr = pLine->Init(GetDocument(), pOwner, NULL);
	
	if (FAILED(hr))
		return hr;
	
	hr = pLine->put_StartPoint(StartPoint);
	if (FAILED(hr))
		return hr;
	
	hr = pLine->put_EndPoint(EndPoint);
	if (FAILED(hr))
		return hr;
	
	hr = AddItemToDrawing (pLine->m_pDbHandItem);

	if (SUCCEEDED(hr))
		return pLine->QueryInterface(ppLine);
	else
		return hr;
	
	}

STDMETHODIMP CAutoEntities::AddMText (IIcadPoint * InsertionPoint, double Width, BSTR Text, IIcadMText * * ppMText)
	{
	if (ppMText == NULL)
		return E_POINTER;
	
	CComObject<CAutoMText> *pText;
	CComObject<CAutoMText>::CreateInstance(&pText);
	if (!pText)
		return E_FAIL;
	
	HRESULT hr;
	CComPtr<IDispatch> pOwner;
	hr = GetOwner (&pOwner.p);
	hr = pText->Init(GetDocument(), pOwner, NULL);
	
	if (FAILED(hr))
		return hr;

	hr = pText->put_InsertionPoint (InsertionPoint);
	if (FAILED(hr))
		return hr;
	
	hr = pText->put_Width (Width);
	if (FAILED(hr))
		return hr;
	
	hr = pText->put_TextString (Text);
	if (FAILED(hr))
		return hr;
	
	hr = AddItemToDrawing (pText->m_pDbHandItem);
	if (SUCCEEDED(hr))
		return pText->QueryInterface(ppMText);
	else
		return hr;
	}

STDMETHODIMP CAutoEntities::AddPointEntity (IIcadPoint * Point, IIcadPointEntity * * ppPointEntity)
	{
	if (ppPointEntity == NULL)
		return E_POINTER;
	
	CComObject<CAutoPointEnt> *pPoint;
	CComObject<CAutoPointEnt>::CreateInstance(&pPoint);
	if (!pPoint)
		return E_FAIL;
	
	HRESULT hr;
	CComPtr<IDispatch> pOwner;
	hr = GetOwner (&pOwner.p);
	hr = pPoint->Init(GetDocument(), pOwner, NULL);
	if (FAILED(hr))
		return hr;

	sds_point pt;
	pt[0] = pt[1] = pt[2] = 0.0;
	if (Point)
		{
		Point->get_x(&pt[0]);
		Point->get_y(&pt[1]);
		Point->get_z(&pt[2]);
		}

	CComPtr<IIcadPoints> Vertices;
	CComPtr<IIcadPoint> Vertex = NULL;

	CComObject<CAutoPoints> *pVertices;
	CComObject<CAutoPoints>::CreateInstance(&pVertices);
	
	pVertices->QueryInterface(&Vertices);
	if (!Vertices)
		return E_OUTOFMEMORY;

	Vertices->Add (pt[0], pt[1], pt[2], 0, &Vertex);

	if (Vertex == NULL)
		return E_OUTOFMEMORY;

	hr = pPoint->put_Coordinates (Vertices);
	if (FAILED(hr))
		return hr;
	
	hr = AddItemToDrawing (pPoint->m_pDbHandItem, TRUE);
	if (SUCCEEDED(hr))
		return pPoint->QueryInterface(ppPointEntity);
	else
		return hr;	
	}

STDMETHODIMP CAutoEntities::AddPolyline (IIcadPoints * Vertices, IIcadPolyline * * ppPolyline)
	{
	if (ppPolyline == NULL)
		return E_POINTER;
	
	CComObject<CAutoPolyline> *pPolyline;
	CComObject<CAutoPolyline>::CreateInstance(&pPolyline);
	if (!pPolyline)
		return E_FAIL;
	
	HRESULT hr;
	CComPtr<IDispatch> pOwner;
	hr = GetOwner (&pOwner.p);
	hr = pPolyline->Init(GetDocument(), pOwner, NULL);
	if (FAILED(hr))
		return hr;
	
	hr = pPolyline->put_Coordinates (Vertices);
	if (FAILED(hr))
		return hr;
	
	//All polylines have the 66 flag set to true
	pPolyline->m_pDbHandItem->set_66 (1);

	hr = AddItemToDrawing (pPolyline->m_pDbHandItem, TRUE);
	if (SUCCEEDED(hr))
		return pPolyline->QueryInterface(ppPolyline);
	else
		return hr;
	}

STDMETHODIMP CAutoEntities::AddPolyfaceMesh (IIcadPoints * Vertices, VARIANT Faces, IIcadPolyfaceMesh * * ppMesh)
	{
	if (ppMesh == NULL)
		return E_POINTER;
	
	CComObject<CAutoPolyfaceMesh> *pPfaceMesh;
	CComObject<CAutoPolyfaceMesh>::CreateInstance(&pPfaceMesh);
	if (!pPfaceMesh)
		return E_FAIL;
	
	HRESULT hr;
	CComPtr<IDispatch> pOwner;
	hr = GetOwner (&pOwner.p);
	hr = pPfaceMesh->Init(GetDocument(), pOwner, NULL);
	if (FAILED(hr))
		return hr;
	
	hr = pPfaceMesh->put_Coordinates (Vertices);
	if (FAILED(hr))
		return hr;
	
	hr = pPfaceMesh->PutFaces (Faces);
	if (FAILED(hr))
		return hr;

	//All polylines have the 66 flag set to true
	pPfaceMesh->m_pDbHandItem->set_66 (1);

	hr = AddItemToDrawing (pPfaceMesh->m_pDbHandItem, TRUE);
	if (SUCCEEDED(hr))
		return pPfaceMesh->QueryInterface(ppMesh);
	else
		return hr;
	}

STDMETHODIMP CAutoEntities::AddRay (IIcadPoint * Point1, IIcadPoint * Point2, IIcadRay * * ppRay)
	{
	if (ppRay == NULL)
		return E_POINTER;
	
	CComObject<CAutoRay> *pRay;
	CComObject<CAutoRay>::CreateInstance(&pRay);
	if (!pRay)
		return E_FAIL;
	
	HRESULT hr;
	CComPtr<IDispatch> pOwner;
	hr = GetOwner (&pOwner.p);
	hr = pRay->Init(GetDocument(), pOwner, NULL);
	
	if (FAILED(hr))
		return hr;
	
	hr = pRay->put_BasePoint(Point1);
	if (FAILED(hr))
		return hr;

	CComPtr<IIcadVector> ppDirection;
	if (Point1 && Point2)
		{
		CComObject<CAutoVector> *pVector;
		CComObject<CAutoVector>::CreateInstance(&pVector);
		if (!pVector)
			return E_FAIL;
		
		double x1, y1, z1;
		Point1->get_x (&x1);
		Point1->get_y (&y1);
		Point1->get_z (&z1);

		double x2, y2, z2;
		Point2->get_x (&x2);
		Point2->get_y (&y2);
		Point2->get_z (&z2);
		
		double len = sqrt ((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1) + (z2 - z1)*(z2 - z1)) ;
		
		pVector->Init(NULL, (x2 - x1)/len, (y2 - y1)/len, (z2 - z1)/len);
		
		pVector->QueryInterface (&ppDirection);
		}
	
	hr = pRay->put_DirectionVector(ppDirection);
	if (FAILED(hr))
		return hr;
	
	hr = AddItemToDrawing (pRay->m_pDbHandItem);
	if (SUCCEEDED(hr))
		return pRay->QueryInterface(ppRay);
	else
		return hr;
	
	}

STDMETHODIMP CAutoEntities::AddSolid (IIcadPoint * Point1, IIcadPoint * Point2, IIcadPoint * Point3, IIcadPoint * Point4, IIcadSolid * * ppSolid)
	{
	if (ppSolid == NULL)
		return E_POINTER;
	
	CComObject<CAutoSolid> *pSolid;
	CComObject<CAutoSolid>::CreateInstance(&pSolid);
	if (!pSolid)
		return E_FAIL;
	
	HRESULT hr;
	CComPtr<IDispatch> pOwner;
	hr = GetOwner (&pOwner.p);
	hr = pSolid->Init(GetDocument(), pOwner, NULL);
	
	if (FAILED(hr))
		return hr;
		
	CComObject<CAutoPoints> *pVertices;
	CComObject<CAutoPoints>::CreateInstance(&pVertices);
	if (!pVertices)
		return E_OUTOFMEMORY;

	CComPtr<IIcadPoint> pIIcadPoint = NULL;

	double x, y, z;
	x = y = z = 0.0;
	if (Point1)
		{
		Point1->get_x (&x);
		Point1->get_y (&y);
		Point1->get_z (&z);
		}
	pVertices->Add (x, y, z, 0, &pIIcadPoint);
	pIIcadPoint = NULL;

	x = y = z = 0.0;
	if (Point2)
		{
		Point2->get_x (&x);
		Point2->get_y (&y);
		Point2->get_z (&z);
		}
	pVertices->Add (x, y, z, 0, &pIIcadPoint);
	pIIcadPoint = NULL;

	x = y = z = 0.0;
	if (Point3)
		{
		Point3->get_x (&x);
		Point3->get_y (&y);
		Point3->get_z (&z);
		}
	pVertices->Add (x, y, z, 0, &pIIcadPoint);
	pIIcadPoint = NULL;

	x = y = z = 0.0;
	if (Point4)
		{
		Point4->get_x (&x);
		Point4->get_y (&y);
		Point4->get_z (&z);
		}
	pVertices->Add (x, y, z, 0, &pIIcadPoint);
	pIIcadPoint = NULL;

	hr = pSolid->put_Coordinates (pVertices);
	if (FAILED(hr))
		return hr;
	
	hr = AddItemToDrawing (pSolid->m_pDbHandItem);
	if (SUCCEEDED(hr))
		return pSolid->QueryInterface(ppSolid);
	else
		return hr;

	}

STDMETHODIMP CAutoEntities::AddSphere (IIcadPoint * Center, double Radius, IIcadPolygonMesh * * ppSphere)
	{
	if (ppSphere == NULL)
		return E_POINTER;

	sds_point pt1, pt2;
	pt1[0] = pt1[1] = pt1[2] = 0.0;
	pt2[0] = pt2[1] = pt2[2] = 1.0;

	if (Center)
		{
		Center->get_x (&pt1[0]);
		Center->get_y (&pt1[1]);
		Center->get_z (&pt1[2]);
		}

	db_handitem *hip, *pNewHip;
	HRESULT hr;

	hr = GetDocument()->AUTO_cmd_sphere_create (pt1, Radius, 16, 8, 0, &hip);
	if (FAILED(hr))
		return E_FAIL;

	if (OwnerIsPaperSpace())
		{
		setComplexEntitySpaceFlag(hip, TRUE);
		}
	else if (OwnerIsModelSpace())
		{
		setComplexEntitySpaceFlag(hip, FALSE);
		}
	else if (OwnerIsBlock())
		{
		pNewHip = hip->newcopy();

		hr = AddItemToDrawing (pNewHip, TRUE);
		if (FAILED(hr))
			return E_FAIL;

		GetDocument()->DeleteEntity(hip);
		hip = pNewHip;
		}

	CComPtr<IIcadEntity> pEntity;
	hr = CreateEntityWrapper (hip, &pEntity);
	if (FAILED(hr))
		return E_FAIL;

	GetDocument()->SetModifiedFlag(TRUE);

	return pEntity->QueryInterface(ppSphere);
	}

STDMETHODIMP CAutoEntities::AddSpline (IIcadPoints * * Points, IIcadVector * StartTangent, IIcadVector * EndTangent, IIcadSpline * * ppSpline)
	{
	if (ppSpline == NULL)
		return E_POINTER;
	
	CComObject<CAutoSpline> *pSpline;
	CComObject<CAutoSpline>::CreateInstance(&pSpline);
	if (!pSpline)
		return E_FAIL;
	
	HRESULT hr;
	CComPtr<IDispatch> pOwner;
	hr = GetOwner (&pOwner.p);
	hr = pSpline->Init(GetDocument(), pOwner, NULL);
	
	if (FAILED(hr))
		return hr;
	
	hr = pSpline->put_FitPoints (*Points);
	if (FAILED(hr))
		return hr;
	
	hr = pSpline->put_StartTangent (StartTangent);
	if (FAILED(hr))
		return hr;
	
	hr = pSpline->put_EndTangent (EndTangent);
	if (FAILED(hr))
		return hr;

	hr = AddItemToDrawing (pSpline->m_pDbHandItem);
	if (SUCCEEDED(hr))
		return pSpline->QueryInterface(ppSpline);
	else
		return hr;
	
	}

STDMETHODIMP CAutoEntities::AddText (BSTR TextString, IIcadPoint * InsertionPoint, double Height, IIcadText * * ppText)
	{
	if (ppText == NULL)
		return E_POINTER;
	
	CComObject<CAutoText> *pText;
	CComObject<CAutoText>::CreateInstance(&pText);
	if (!pText)
		return E_FAIL;
	
	HRESULT hr;
	CComPtr<IDispatch> pOwner;
	hr = GetOwner (&pOwner.p);
	hr = pText->Init(GetDocument(), pOwner, NULL);
	
	if (FAILED(hr))
		return hr;

	hr = pText->put_InsertionPoint (InsertionPoint);
	if (FAILED(hr))
		return hr;
	
	hr = pText->put_Height (Height);
	if (FAILED(hr))
		return hr;
	
	hr = pText->put_TextString (TextString);
	if (FAILED(hr))
		return hr;
	
	hr = AddItemToDrawing (pText->m_pDbHandItem);
	if (SUCCEEDED(hr))
		return pText->QueryInterface(ppText);
	else
		return hr;
	}

STDMETHODIMP CAutoEntities::AddTolerance (BSTR Text, IIcadPoint * InsertionPoint, IIcadVector * Direction, IIcadTolerance * * ppTolerance)
	{
	if (ppTolerance == NULL)
		return E_POINTER;
	
	CComObject<CAutoTolerance> *pTolerance;
	CComObject<CAutoTolerance>::CreateInstance(&pTolerance);
	if (!pTolerance)
		return E_FAIL;
	
	HRESULT hr;
	CComPtr<IDispatch> pOwner;
	hr = GetOwner (&pOwner.p);
	hr = pTolerance->Init(GetDocument(), pOwner, NULL);
	
	if (FAILED(hr))
		return hr;

	hr = pTolerance->put_InsertionPoint (InsertionPoint);
	if (FAILED(hr))
		return hr;
	
	hr = pTolerance->put_TextString (Text);
	if (FAILED(hr))
		return hr;
	
	hr = pTolerance->put_DirectionVector (Direction);
	if (FAILED(hr))
		return hr;
	
	hr = AddItemToDrawing (pTolerance->m_pDbHandItem);
	if (SUCCEEDED(hr))
		return pTolerance->QueryInterface(ppTolerance);
	else
		return hr;
	}

STDMETHODIMP CAutoEntities::AddTorus (IIcadPoint * Center, double TorusRadius, double TubeRadius, IIcadPolygonMesh * * pTorus)
	{
	if (pTorus == NULL)
		return E_POINTER;
	
	sds_point pt1;
	pt1[0] = pt1[1] = pt1[2] = 0.0;

	if (Center)
		{
		Center->get_x (&pt1[0]);
		Center->get_y (&pt1[1]);
		Center->get_z (&pt1[2]);
		}
	
	db_handitem *hip, *pNewHip;
	HRESULT hr;

	hr = GetDocument()->AUTO_cmd_torus_create (pt1, TorusRadius * 2.0, TubeRadius * 2.0, 16, 16, &hip);

	if (FAILED(hr))
		return E_FAIL;

	if (OwnerIsPaperSpace())
		{
		setComplexEntitySpaceFlag(hip, TRUE);
		}
	else if (OwnerIsModelSpace())
		{
		setComplexEntitySpaceFlag(hip, FALSE);
		}
	else if (OwnerIsBlock())
		{
		pNewHip = hip->newcopy();

		hr = AddItemToDrawing (pNewHip, TRUE);
		if (FAILED(hr))
			return E_FAIL;

		GetDocument()->DeleteEntity(hip);
		hip = pNewHip;
		}

	CComPtr<IIcadEntity> pEntity;
	hr = CreateEntityWrapper (hip, &pEntity);
	if (FAILED(hr))
		return E_FAIL;

	GetDocument()->SetModifiedFlag(TRUE);

	return pEntity->QueryInterface(pTorus);
	}

STDMETHODIMP CAutoEntities::AddTrace (IIcadPoints * Points, IIcadTrace * * ppTrace)
	{
	if (ppTrace == NULL)
		return E_POINTER;
	
	CComObject<CAutoTrace> *pTrace;
	CComObject<CAutoTrace>::CreateInstance(&pTrace);
	if (!pTrace)
		return E_FAIL;
	
	HRESULT hr;
	CComPtr<IDispatch> pOwner;
	hr = GetOwner (&pOwner.p);
	hr = pTrace->Init(GetDocument(), pOwner, NULL);
	
	if (FAILED(hr))
		return hr;

	hr = pTrace->put_Coordinates (Points);
	if (FAILED(hr))
		return hr;
	
	hr = AddItemToDrawing (pTrace->m_pDbHandItem);
	if (SUCCEEDED(hr))
		return pTrace->QueryInterface(ppTrace);
	else
		return hr;
	}

STDMETHODIMP CAutoEntities::AddWedge (IIcadPoint * Origin, double Length, double Width, double Height, IIcadPolygonMesh * * ppWedge)
	{
	if (ppWedge == NULL)
		return E_POINTER;
	
	sds_point pt1, pt2;
	pt1[0] = pt1[1] = pt1[2] = 0.0;
	pt2[0] = pt2[1] = pt2[2] = 1.0;

	if (Origin)
		{
		Origin->get_x (&pt1[0]);
		Origin->get_y (&pt1[1]);
		Origin->get_z (&pt1[2]);
		}
	pt2[0] = pt1[0] + Length;
	pt2[1] = pt1[1] + Width;
	pt2[2] = pt1[2];
	
	db_handitem *hip, *pNewHip;
	HRESULT hr;

	hr = GetDocument()->AUTO_cmd_wedge_create (pt1, pt2, Height, 0.0, &hip);
	if (FAILED(hr))
		return E_FAIL;

	if (OwnerIsPaperSpace())
		{
		setComplexEntitySpaceFlag(hip, TRUE);
		}
	else if (OwnerIsModelSpace())
		{
		setComplexEntitySpaceFlag(hip, FALSE);
		}
	else if (OwnerIsBlock())
		{
		pNewHip = hip->newcopy();

		hr = AddItemToDrawing (pNewHip, TRUE);
		if (FAILED(hr))
			return E_FAIL;

		GetDocument()->DeleteEntity(hip);
		hip = pNewHip;
		}

	CComPtr<IIcadEntity> pEntity;
	hr = CreateEntityWrapper (hip, &pEntity);
	if (FAILED(hr))
		return E_FAIL;

	GetDocument()->SetModifiedFlag(TRUE);

	return pEntity->QueryInterface(ppWedge);
	}

STDMETHODIMP CAutoEntities::AttachExternalReference (BSTR PathName, BSTR Name, IIcadPoint * InsertionPoint, double XScale, double YScale, double ZScale, double Rotation, VARIANT_BOOL bOverlay, IIcadExternalReference * * ppXRef)
	{
	if (ppXRef == NULL)
		return E_POINTER;
	
	db_handitem *hip, *pNewHip;
	HRESULT hr;

	if( InsertionPoint == NULL )
		return E_INVALIDARG;

	CString pathname(PathName);
	if (pathname.IsEmpty())
		return E_INVALIDARG;

	CString name (Name);

	sds_point pt;
	pt[0] = pt[1] = pt[2] = 0.0;

	InsertionPoint->get_x (&pt[0]);
	InsertionPoint->get_y (&pt[1]);
	InsertionPoint->get_z (&pt[2]);

	if (icadRealEqual(XScale,0.0)) XScale = 1.0;	
	if (icadRealEqual(YScale,0.0)) YScale = 1.0;
	if (icadRealEqual(ZScale,0.0)) ZScale = 1.0;

	bool mode = (bOverlay == VARIANT_TRUE) ? true : false;

	hr = GetDocument()->AttachXRef (pathname, name, pt, XScale, YScale, ZScale, Rotation, mode, &hip);
	if (FAILED(hr))
		return E_FAIL;

	if (OwnerIsPaperSpace())
		{
		setComplexEntitySpaceFlag(hip, TRUE);
		}
	else if (OwnerIsModelSpace())
		{
		setComplexEntitySpaceFlag(hip, FALSE);
		}
	else if (OwnerIsBlock())
		{
		//I dont think this works...
		pNewHip = hip->newcopy();

		hr = AddItemToDrawing (pNewHip, TRUE);
		if (FAILED(hr))
			return E_FAIL;

		GetDocument()->DeleteEntity(hip);
		hip = pNewHip;
		}

	CComPtr<IIcadEntity> pEntity;
	hr = CreateEntityWrapper (hip, &pEntity);
	if (FAILED(hr))
		return E_FAIL;

	GetDocument()->SetModifiedFlag(TRUE);

	return pEntity->QueryInterface(ppXRef);
	
	}

STDMETHODIMP CAutoEntities::InsertBlock (IIcadPoint * InsertionPoint, BSTR BlockName, double XScale, double YScale, double ZScale, double Rotation, IIcadBlockInsert * * ppBlockInsert)
	{
	if (ppBlockInsert == NULL)
		return E_POINTER;

	CString str (BlockName);
	if (str.IsEmpty())
		return E_INVALIDARG;

	CComObject<CAutoBlockInsert> *pInsert;
	CComObject<CAutoBlockInsert>::CreateInstance(&pInsert);
	if (!pInsert)
		return E_FAIL;
	
	HRESULT hr;
	CComPtr<IDispatch> pOwner;
	hr = GetOwner (&pOwner.p);
	hr = pInsert->Init(GetDocument(), pOwner, NULL);
	if (FAILED(hr))
		return hr;

	hr = pInsert->put_InsertionPoint (InsertionPoint);
	if (FAILED(hr))
		return hr;
	
	hr = pInsert->put_XScaleFactor (XScale);
	if (FAILED(hr))
		return hr;
	
	hr = pInsert->put_YScaleFactor (YScale);
	if (FAILED(hr))
		return hr;
	
	hr = pInsert->put_ZScaleFactor (ZScale);
	if (FAILED(hr))
		return hr;
	
	hr = pInsert->put_Rotation (Rotation);
	if (FAILED(hr))
		return hr;

	sds_point pt;
	pt[0] = pt[1] = pt[2] = 0.0;
	if (InsertionPoint)
		{
		InsertionPoint->get_x (&pt[0]);
		InsertionPoint->get_y (&pt[1]);
		InsertionPoint->get_z (&pt[2]);
		}

	CString blkname(BlockName);
	db_insert *newins;

	hr = GetDocument()->InsertBlock(blkname.GetBuffer(0),
									(db_insert *)(pInsert->m_pDbHandItem),
									pt, &newins);
	if (FAILED(hr))
		return E_FAIL;

	if (pInsert->m_bNeedToDelete && pInsert->m_pDbHandItem)
		delete pInsert->m_pDbHandItem;
	pInsert->m_pDbHandItem = newins;

	return pInsert->QueryInterface(ppBlockInsert);
	}

// EBATECH(CNBR) -[ 2002/4/24 Support SHAPE
STDMETHODIMP CAutoEntities::AddShape (BSTR Name, IIcadPoint * InsertionPoint, double Height, double Rotation, IIcadShape * * ppShape)
	{
	if (ppShape == NULL)
		return E_POINTER;
	
	CComObject<CAutoShape> *pShape;
	CComObject<CAutoShape>::CreateInstance(&pShape);
	if (!pShape)
		return E_FAIL;
	
	HRESULT hr;
	CComPtr<IDispatch> pOwner;
	hr = GetOwner (&pOwner.p);
	hr = pShape->Init(GetDocument(), pOwner, NULL);
	
	if (FAILED(hr))
		return hr;

	hr = pShape->put_InsertionPoint (InsertionPoint);
	if (FAILED(hr))
		return hr;
	
	hr = pShape->put_Height (Height);
	if (FAILED(hr))
		return hr;
	
	hr = pShape->put_Rotation (Rotation);
	if (FAILED(hr))
		return hr;
	
	hr = pShape->put_Name (Name);
	if (FAILED(hr))
		return hr;
	
	hr = AddItemToDrawing (pShape->m_pDbHandItem);
	if (SUCCEEDED(hr))
		return pShape->QueryInterface(ppShape);
	else
		return hr;
	}
// EBATECH(CNBR) ]- 

HRESULT CAutoEntities::CreateEntityWrapper (db_handitem* pDbHandItem, IIcadEntity **ppEntity)
	{
	if (NULL == pDbHandItem) 
		return E_INVALIDARG;
	
	CComPtr<IDispatch> pOwner;
	GetOwner (&pOwner.p);

	CIcadDoc *pDoc = GetDocument();

	return ::CreateEntityWrapper (pDoc, pOwner, pDbHandItem, ppEntity);
	}


/********************************************************************************
 * Author:	Sachin Dange.
 *
 * Purpose:	To set flag 67 to the value passed in "bFlag" for all the nodes 
 *			present in the linked list pointed to by the parameter "pDbHandItem"
 *
 * Returns:	void
 ********************************************************************************/
void CAutoEntities::setComplexEntitySpaceFlag(db_handitem *pDbHandItem, BOOL bFlag)
{
	while (pDbHandItem != NULL)
	{
		pDbHandItem->set_67(bFlag);
		pDbHandItem = pDbHandItem->next;
	}
}
