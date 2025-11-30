/* INTELLICAD\PRJ\LIB\AUTO\AUTOENTITY.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * Implementation of CAutoEntity
 * This is the base class for all Entity objects like CAutoLine, CAutoBlock etc.
 * This is derived from CAutoDbItem.
 *
 */

#include "StdAfx.h"
#include "autorc.h"
#include "AutoEntity.h"
#include "AutoBaseDefs.h"
#include "AutoHelpers.h"
#include "AutoSelSet.h"
#include "AutoPoint.h"
#include "AutoLWPolyline.h"
#include "ltypetabrec.h"
#include "layertabrec.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoEntity

STDMETHODIMP CAutoEntity::get_Color (Colors * pColor)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pColor == NULL)
		return E_POINTER;


	int color;
	if (!m_pDbHandItem->get_62 (&color))
		return E_FAIL;

	*pColor = (Colors) color;

	return NOERROR;
	}

STDMETHODIMP CAutoEntity::put_Color (Colors pColor)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	int Color;

	switch ((Colors)pColor)
		{
		case vicByBlock:	Color =	0	; break;
		case vicRed:		Color =	1	; break;
		case vicYellow:		Color =	2	; break;
		case vicGreen:		Color =	3	; break;
		case vicCyan:		Color =	4	; break;
		case vicBlue:		Color =	5	; break;
		case vicMagenta:	Color =	6	; break;
		case vicWhite:		Color =	7	; break;
		case vicByLayer:	Color =	256	; break;
		default :			Color = pColor;
		}

	if (Color < 0 || Color > 256)
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_62 (Color))
		return E_FAIL;

	if (m_pDbHandItem->ret_type() == DB_POLYLINE)
		{
		db_handitem *hip = m_pDbHandItem->next;
		while ( (hip != NULL) && (hip->ret_type() != DB_SEQEND))
			{
			hip->set_62 (Color);
			hip = hip->next;
			}
		}

	m_pDoc->SetModifiedFlag(TRUE);
	return NOERROR;
	}

STDMETHODIMP CAutoEntity::get_EntityName (BSTR * pEntName)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (pEntName == NULL)
		return E_POINTER;

	EntityType Type;
	get_EntityType (&Type);
	CString ename;

	switch (Type)
		{
		case vicUnknownEntity	:	ename.LoadString(IDS_UnknownType);  break;
		case vic3dFace	:			ename.LoadString(IDS_3dFace);		break;
		case vic3dPoly	:			ename.LoadString(IDS_3dPoly);		break;
		case vicArc	:				ename.LoadString(IDS_Arc);			break;
		case vicAttribute	:		ename.LoadString(IDS_Attribute);	break;
		case vicAttributeDef	:	ename.LoadString(IDS_AttributeDef); break;
		case vicBlockInsert	:		ename.LoadString(IDS_BlockInsert);	break;
		case vicCircle	:			ename.LoadString(IDS_Circle);		break;
		case vicDim3PointAngular:	ename.LoadString(IDS_Dim3PtAngular);break;
		case vicDimAligned	:		ename.LoadString(IDS_DimAligned);	break;
		case vicDimAngular	:		ename.LoadString(IDS_DimAngular);	break;
		case vicDimDiametric	:	ename.LoadString(IDS_DimDiametric); break;
		case vicDimOrdinate	:		ename.LoadString(IDS_DimOrdinate);	break;
		case vicDimRadial	:		ename.LoadString(IDS_DimRadial);	break;
		case vicDimRotated	:		ename.LoadString(IDS_DimRotated);	break;
		case vicEllipse	:			ename.LoadString(IDS_Ellipse);		break;
		case vicExternalReference:	ename.LoadString(IDS_ExtReference);	break;
		case vicHatch	:			ename.LoadString(IDS_Hatch);		break;
		case vicImage	:			ename.LoadString(IDS_Image);		break;
		case vicInfiniteLine	:	ename.LoadString(IDS_InfiniteLine); break;
		case vicLeader	:			ename.LoadString(IDS_Leader);		break;
		case vicLine	:			ename.LoadString(IDS_Line);			break;
		case vicLWPolyline	:		ename.LoadString(IDS_LWPolyline);	break;
		case vicMtext	:			ename.LoadString(IDS_Mtext);		break;
		case vicPointEntity	:		ename.LoadString(IDS_PointEntity);	break;
		case vicPolyline	:		ename.LoadString(IDS_Polyline);		break;
		case vicPolyfaceMesh	:	ename.LoadString(IDS_PolyfaceMesh);	break;
		case vicPolygonMesh	:		ename.LoadString(IDS_PolygonMesh);	break;
		case vicPViewport	:		ename.LoadString(IDS_PViewport);	break;
		case vicRay	:				ename.LoadString(IDS_Ray);			break;
		case vicShape	:			ename.LoadString(IDS_Shape);		break; // EBATECH(CNBR) 2002/4/24
		case vicSolid	:			ename.LoadString(IDS_Solid);		break;
		case vicSpline	:			ename.LoadString(IDS_Spline);		break;
		case vicText	:			ename.LoadString(IDS_Text);			break;
		case vicTolerance	:		ename.LoadString(IDS_Tolerance);	break;
		case vicTrace	:			ename.LoadString(IDS_Trace);		break;
		}

	*pEntName = ename.AllocSysString();

	return NOERROR;

	}

STDMETHODIMP CAutoEntity::get_EntityType (EntityType * pType)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pType == NULL)
		return E_POINTER;

	switch(m_pDbHandItem->ret_type())
		{
		case DB_3DFACE:		*pType = vic3dFace; break;
		case DB_3DSOLID:	*pType = vic3dPoly;			break;
		case DB_ARC:		*pType = vicArc;			break;
		case DB_ATTDEF:		*pType = vicAttributeDef;	break;
		case DB_ATTRIB:		*pType = vicAttribute;		break;
		case DB_INSERT:
			{
			int flags;
			db_blocktabrec *bthip = ((db_insert *)m_pDbHandItem)->ret_bthip();
			if (bthip == NULL)
				return E_FAIL;

			bthip->get_70 (&flags);

			*pType = (flags & (IC_BLOCK_XREF | IC_BLOCK_XREF_OVERLAID)) ? vicExternalReference : vicBlockInsert;
			}
			break;
		case DB_CIRCLE:		*pType = vicCircle;			break;
		case DB_DIMENSION:
			{
			int flags;
			m_pDbHandItem->get_70 (&flags);
			flags = flags % 32;
			switch (flags)
				{
				case 0:	*pType = vicDimRotated; break;
				case 1: *pType = vicDimAligned; break;
				case 2: *pType = vicDimAngular; break;
				case 3: *pType = vicDimDiametric; break;
				case 4: *pType = vicDimRadial; break;
				case 5: *pType = vicDim3PointAngular; break;
				case 6: *pType = vicDimOrdinate; break;
				default:*pType = vicUnknownEntity;
				}
			}
			break;

		case DB_ELLIPSE:	*pType = vicEllipse;		break;
		case DB_HATCH:		*pType = vicHatch;			break;
		case DB_IMAGE:		*pType = vicImage;			break;
		case DB_XLINE:		*pType = vicInfiniteLine;	break;
		case DB_LEADER:		*pType = vicLeader;			break;
		case DB_LINE:		*pType = vicLine;			break;
		case DB_LWPOLYLINE:	*pType = vicLWPolyline;		break;
		case DB_MTEXT:		*pType = vicMtext;			break;
		case DB_POINT:		*pType = vicPointEntity;	break;
		case DB_POLYLINE:
			{
			int flags;
			m_pDbHandItem->get_70 (&flags);
			if (flags & IC_PLINE_3DPLINE)
				*pType = vic3dPoly;
			else if (flags & IC_PLINE_3DMESH)
				*pType = vicPolygonMesh;
			else if (flags & IC_PLINE_POLYFACEMESH)	//currently not supported
				*pType = vicPolyfaceMesh;
			else
				*pType = vicPolyline;
			}
			break;
		case DB_VIEWPORT:	*pType = vicPViewport;		break;
		case DB_RAY:		*pType = vicRay;			break;
		case DB_SHAPE:		*pType = vicShape;			break;	// EBATECH(CNBR) 2002/4/24
		case DB_SOLID:		*pType = vicSolid;			break;
		case DB_SPLINE:		*pType = vicSpline;			break;
		case DB_TEXT:		*pType = vicText;			break;
		case DB_TOLERANCE:	*pType = vicTolerance;		break;
		case DB_TRACE:		*pType = vicTrace;			break;
		default:			*pType = vicUnknownEntity;
		}

	return NOERROR;
	}

STDMETHODIMP CAutoEntity::get_Layer (BSTR * pLayer)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pLayer == NULL)
		return E_POINTER;

	char* pStr;
	m_pDbHandItem->get_8 (&pStr);

	CString str (pStr);
	*pLayer = str.AllocSysString();

	return NOERROR;
	}

STDMETHODIMP CAutoEntity::put_Layer (BSTR pLayer)
	{
	RETURN_IF_INVALID (m_pDbHandItem);

	CString str(pLayer);
	if (str.IsEmpty ())
		return E_INVALIDARG;

	db_layertabrec *pTableRecord = NULL;
	pTableRecord = (db_layertabrec *)m_pDoc->m_dbDrawing->findtabrec( DB_LAYERTAB, str.GetBuffer(0), 1 );
	if (!pTableRecord)
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_8 ((char*)str.GetBuffer(0), m_pDoc->m_dbDrawing))
		return E_FAIL;

	if (m_pDbHandItem->ret_type() == DB_POLYLINE)
		{
		db_handitem *hip = m_pDbHandItem->next;
		while ( (hip != NULL) && (hip->ret_type() != DB_SEQEND))
			{
			hip->set_8 ((char*)str.GetBuffer(0), m_pDoc->m_dbDrawing);
			hip = hip->next;
			}
		}

	m_pDoc->SetModifiedFlag(TRUE);
	return NOERROR;
	}

STDMETHODIMP CAutoEntity::get_Linetype (BSTR * pLinetype)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pLinetype == NULL)
		return E_POINTER;

	char* pStr;
	m_pDbHandItem->get_6 (&pStr);

	CString str (pStr);
	*pLinetype = str.AllocSysString();

	return NOERROR;
	}

STDMETHODIMP CAutoEntity::put_Linetype (BSTR pLinetype)
	{
	RETURN_IF_INVALID (m_pDbHandItem);

	CString str(pLinetype);
	if (str.IsEmpty ())
		return E_INVALIDARG;

	db_ltypetabrec *pTableRecord = NULL;
	pTableRecord = (db_ltypetabrec *)m_pDoc->m_dbDrawing->findtabrec( DB_LTYPETAB, str.GetBuffer(0), 1 );
	if (!pTableRecord)
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_6 ((char*)str.GetBuffer(0), m_pDoc->m_dbDrawing))
		return E_FAIL;

	if (m_pDbHandItem->ret_type() == DB_POLYLINE)
		{
		db_handitem *hip = m_pDbHandItem->next;
		while ( (hip != NULL) && (hip->ret_type() != DB_SEQEND))
			{
			hip->set_6 ((char*)str.GetBuffer(0), m_pDoc->m_dbDrawing);
			hip = hip->next;
			}
		}

	m_pDoc->SetModifiedFlag(TRUE);
	return NOERROR;
	}

STDMETHODIMP CAutoEntity::get_LinetypeScale (double * LinetypeScale)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (LinetypeScale == NULL)
		return E_POINTER;

	m_pDbHandItem->get_48 (LinetypeScale);

	return NOERROR;
	}

STDMETHODIMP CAutoEntity::put_LinetypeScale (double LinetypeScale)
	{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (LinetypeScale <= 0.0)
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_48 (LinetypeScale))
		return E_FAIL;

	if (m_pDbHandItem->ret_type() == DB_POLYLINE)
		{
		db_handitem *hip = m_pDbHandItem->next;
		while ( (hip != NULL) && (hip->ret_type() != DB_SEQEND))
			{
			hip->set_48 (LinetypeScale);
			hip = hip->next;
			}
		}

	m_pDoc->SetModifiedFlag(TRUE);
	return NOERROR;
	}

STDMETHODIMP CAutoEntity::get_Visible (VARIANT_BOOL * pVisible)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pVisible == NULL)
		return E_POINTER;

	int vis;
	m_pDbHandItem->get_60 (&vis);
	if (vis)
		*pVisible = VARIANT_FALSE;
	else
		*pVisible = VARIANT_TRUE;

	return NOERROR;
	}

STDMETHODIMP CAutoEntity::put_Visible (VARIANT_BOOL pVisible)
	{
	RETURN_IF_INVALID (m_pDbHandItem);

	int bVis = 1;
	if (pVisible == VARIANT_TRUE)
		bVis = 0;

	if (!m_pDbHandItem->set_60 (bVis))
		return E_FAIL;

	if (m_pDbHandItem->ret_type() == DB_POLYLINE)
		{
		db_handitem *hip = m_pDbHandItem->next;
		while ( (hip != NULL) && (hip->ret_type() != DB_SEQEND))
			{
			hip->set_60 (bVis);
			hip = hip->next;
			}
		}

	m_pDoc->SetModifiedFlag(TRUE);
	return NOERROR;
	}

STDMETHODIMP CAutoEntity::ArrayPolar (int NumberOfObjects, double IncludedAngle, IIcadPoint * CenterPoint, IIcadPoint* PointOnAxis, VARIANT_BOOL RotateObjects, IIcadSelectionSet * * ppObjects)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppObjects == NULL)
		return E_POINTER;

	if (NumberOfObjects <= 1)
		return E_INVALIDARG;

	if (icadRealEqual(IncludedAngle,0.0))
		return E_INVALIDARG;

	if (!CenterPoint)
		return E_INVALIDARG;

	sds_point pt1;
	pt1[0] = pt1[1] = pt1[2] = 0.0;
	CenterPoint->get_x(&pt1[0]);
	CenterPoint->get_y(&pt1[1]);
	CenterPoint->get_z(&pt1[2]);

	sds_point pt2;
	double *dpt2;
	if (PointOnAxis)
		{
		PointOnAxis->get_x(&pt2[0]);
		PointOnAxis->get_y(&pt2[1]);
		PointOnAxis->get_z(&pt2[2]);
		dpt2 = pt2;
		}
	else
		dpt2 = NULL;

	bool bRotate = (RotateObjects == VARIANT_TRUE) ? true : false;

	//construct the input selection set
	sds_name ssetout, ssetin;
    sds_name ename;
	m_pDoc->AUTO_ssadd (NULL, NULL, ssetin);
	ename[0] = (long)m_pDbHandItem;
	ename[1] = (long)(m_pDoc->m_dbDrawing);
	m_pDoc->AUTO_ssadd (ename, ssetin, ssetin);

	HRESULT hr = m_pDoc->AUTO_cmd_array_polar (ssetin, NumberOfObjects, IncludedAngle, 0.0, pt1, dpt2, bRotate, ssetout);
	if (FAILED(hr))
		return E_FAIL;

	CComObject<CAutoSelectionSet>* pSelectionSet;
	CComObject<CAutoSelectionSet>::CreateInstance(&pSelectionSet);
	if (!pSelectionSet)
		return E_FAIL;

	CString Name("_Internal");
	pSelectionSet->Init(m_pDoc, Name);
	hr = pSelectionSet->InitializeFromSDSsset (ssetout);
	if (FAILED(hr))
		return E_FAIL;

	m_pDoc->AUTO_ssfree (ssetout);

	return pSelectionSet->QueryInterface(ppObjects);
	}

STDMETHODIMP CAutoEntity::ArrayRectangular (int NumberOfRows, int NumberOfColumns, int NumberOfLevels, double GapBetweenRows, double GapBetweenColumns, double GapBetweenLevels, IIcadSelectionSet * * ppObjects)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppObjects == NULL)
		return E_POINTER;

	if ((NumberOfRows <= 0) || (NumberOfColumns <= 0))
		return E_INVALIDARG;

	if ((NumberOfRows == 1) && (NumberOfColumns == 1))
		if (NumberOfLevels < 2)
			return E_INVALIDARG;

	if (NumberOfLevels == 0)
		NumberOfLevels = 1;

	//construct the input selection set
    sds_name ename;
	sds_name ssetout, ssetin;
	m_pDoc->AUTO_ssadd (NULL, NULL, ssetin);
	ename[0] = (long)m_pDbHandItem;
	ename[1] = (long)(m_pDoc->m_dbDrawing);
	m_pDoc->AUTO_ssadd (ename, ssetin, ssetin);

	HRESULT hr = m_pDoc->AUTO_cmd_array_rectangular (ssetin, NumberOfRows, NumberOfColumns, NumberOfLevels, GapBetweenRows, GapBetweenColumns, GapBetweenLevels, ssetout);
	if (FAILED(hr))
		return E_FAIL;

//	hr = ConvertSDSssetToAutoSelectionSet (ssetout, ppObjects, m_pDoc);
//	if (FAILED(hr))
//		return E_FAIL;

	CComObject<CAutoSelectionSet>* pSelectionSet;
	CComObject<CAutoSelectionSet>::CreateInstance(&pSelectionSet);
	if (!pSelectionSet)
		return E_FAIL;

	CString Name("_Internal");
	pSelectionSet->Init(m_pDoc, Name);
	hr = pSelectionSet->InitializeFromSDSsset (ssetout);
	if (FAILED(hr))
		return E_FAIL;

	m_pDoc->AUTO_ssfree (ssetout);

	return pSelectionSet->QueryInterface(ppObjects);
	}

STDMETHODIMP CAutoEntity::Copy (IIcadEntity * * ppCopy)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppCopy == NULL)
		return E_POINTER;

	db_handitem * copy = CopyHip (m_pDbHandItem);

	if (copy == NULL)
		return E_FAIL;

	if (!AddToDrawing (m_pDoc, copy))
		return E_FAIL;

	return CreateEntityWrapper (m_pDoc, NULL, copy, ppCopy);
	}

STDMETHODIMP CAutoEntity::Erase ()
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	return m_pDoc->RedrawEntity (m_pDbHandItem, IC_REDRAW_UNDRAW);
	}

STDMETHODIMP CAutoEntity::GetBoundingBox (IIcadPoint * * MinPoint, IIcadPoint * * MaxPoint)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	sds_point ll, ur;
	HRESULT hr;
	m_pDbHandItem->get_extent (ll, ur);

	CComObject<CAutoPoint> *pPoint1;
	CComObject<CAutoPoint>::CreateInstance(&pPoint1);
	if (!pPoint1)
		return E_FAIL;

	pPoint1->Init(NULL, NULL, ll[0], ll[1], ll[2]);

	hr = pPoint1->QueryInterface (MinPoint);
	if (FAILED(hr))
		return E_FAIL;

	CComObject<CAutoPoint> *pPoint2;
	CComObject<CAutoPoint>::CreateInstance(&pPoint2);
	if (!pPoint2)
		return E_FAIL;

	pPoint2->Init(NULL, NULL, ur[0], ur[1], ur[2]);

	hr = pPoint2->QueryInterface (MaxPoint);
	if (FAILED(hr))
		return E_FAIL;

	return NOERROR;
	}

STDMETHODIMP CAutoEntity::Highlight (VARIANT_BOOL HighlightFlag)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (HighlightFlag == VARIANT_TRUE)
		return m_pDoc->RedrawEntity (m_pDbHandItem, IC_REDRAW_HILITE);
	else
		return m_pDoc->RedrawEntity (m_pDbHandItem, IC_REDRAW_UNHILITE);
	}

STDMETHODIMP CAutoEntity::IntersectWith (IDispatch * IntersectingObject, IntersectOptions Option, IIcadPoints * * ppArrayPoints)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (ppArrayPoints == NULL)
		return E_POINTER;

	if (!IntersectingObject)
		return E_INVALIDARG;

	CComPtr<IIcadEntity> pEnt;
	HRESULT hr;

	hr = IntersectingObject->QueryInterface (&pEnt);
	if (FAILED(hr))
		return E_INVALIDARG;

	db_handitem * pDbHandItem = NULL;
	long nhip;
	hr = pEnt->_handitem (&nhip);
	pDbHandItem = (db_handitem *)nhip;
	if (FAILED(hr) || (pDbHandItem == NULL))
		return E_FAIL;

	//construct the first selection set using the current entity
    sds_name ename;
	sds_name sset1;
	m_pDoc->AUTO_ssadd (NULL, NULL, sset1);
	ename[0] = (long)m_pDbHandItem;
	ename[1] = (long)(m_pDoc->m_dbDrawing);
	m_pDoc->AUTO_ssadd (ename, sset1, sset1);

	//construct the second selection set using the input intersecting object
	sds_name sset2;
	m_pDoc->AUTO_ssadd (NULL, NULL, sset2);
	ename[0] = (long)pDbHandItem;
	ename[1] = (long)(m_pDoc->m_dbDrawing);
	m_pDoc->AUTO_ssadd (ename, sset2, sset2);

	struct resbuf *ptlist, *rb;

	//Calculate the intersections
	int ret = m_pDoc->AUTO_ssxss (sset1, sset2, IC_ZRO, &ptlist, 1, NULL, NULL, Option);
	m_pDoc->AUTO_ssfree (sset1);
	m_pDoc->AUTO_ssfree (sset2);
	if (ret != 0)
		return E_FAIL;

	if (ptlist == NULL)
		return E_FAIL;

	//create a points collection
	CComObject<CAutoPoints> *pVertices;
	CComObject<CAutoPoints>::CreateInstance(&pVertices);
	if (!pVertices)
		return E_OUTOFMEMORY;

	CComPtr<IIcadPoint> pIIcadPoint = NULL;
	sds_point pt;

	//fill in the points collection
	for (rb = ptlist; rb != NULL; rb = rb->rbnext)
		{
		ic_ptcpy (pt, rb->resval.rpoint);

		pVertices->Add (pt[0], pt[1], pt[2], 0, &pIIcadPoint);
		if (pIIcadPoint == NULL)
			return E_OUTOFMEMORY;

		pIIcadPoint = NULL;
		}

	sds_relrb (ptlist);

	return pVertices->QueryInterface (ppArrayPoints);
	}

STDMETHODIMP CAutoEntity::Mirror (IIcadPoint * Point1, IIcadPoint * Point2, VARIANT_BOOL MakeCopy, IIcadEntity * * MirroredEntity)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (!Point1)
		return E_INVALIDARG;

	if (!Point2)
		return E_INVALIDARG;

	sds_point pt1;
	pt1[0] = pt1[1] = pt1[2] = 0.0;
	Point1->get_x(&pt1[0]);
	Point1->get_y(&pt1[1]);
	//Point1->get_z(&pt1[2]); ignore the z's

	sds_point pt2;
	pt2[0] = pt2[1] = pt2[2] = 0.0;
	Point2->get_x(&pt2[0]);
	Point2->get_y(&pt2[1]);
	//Point2->get_z(&pt2[2]); ignore the z's

	if (ic_pt_equal(pt1, pt2))
		return E_INVALIDARG;

	sds_matrix mtx;
	bool ok;
	ic_mirrorLine(pt1, pt2, ok, mtx);
	if (!ok)
		return E_FAIL;

	db_handitem *pDbHandItem = NULL;

	if (MakeCopy == VARIANT_TRUE)
		{
		pDbHandItem = CopyHip (m_pDbHandItem);
		if (pDbHandItem == NULL)
			return E_FAIL;

		if (!AddToDrawing (m_pDoc, pDbHandItem))
			return E_FAIL;

		if (FAILED(CreateEntityWrapper (m_pDoc, NULL, pDbHandItem, MirroredEntity)))
			return E_FAIL;
		}
	else
		pDbHandItem = m_pDbHandItem;

	m_pDoc->SetModifiedFlag(TRUE);

	HRESULT res;
	res = m_pDoc->XformEntity(pDbHandItem, mtx);

	if (pDbHandItem->ret_type() == DB_LWPOLYLINE)
	{
		CAutoLWPolyline::FixMirroring(pDbHandItem);
	}

	return res;
	}

STDMETHODIMP CAutoEntity::Mirror3D (IIcadPoint * Point1, IIcadPoint * Point2, IIcadPoint * Point3, VARIANT_BOOL MakeCopy, IIcadEntity * * MirroredEntity)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (!Point1)
		return E_INVALIDARG;

	if (!Point2)
		return E_INVALIDARG;

	if (!Point3)
		return E_INVALIDARG;

	sds_point pt1;
	pt1[0] = pt1[1] = pt1[2] = 0.0;
	Point1->get_x(&pt1[0]);
	Point1->get_y(&pt1[1]);
	Point1->get_z(&pt1[2]);

	sds_point pt2;
	pt2[0] = pt2[1] = pt2[2] = 0.0;
	Point2->get_x(&pt2[0]);
	Point2->get_y(&pt2[1]);
	Point2->get_z(&pt2[2]);

	if (ic_pt_equal(pt1, pt2))
		return E_INVALIDARG;

	sds_point pt3;
	pt3[0] = pt3[1] = pt3[2] = 0.0;
	Point3->get_x(&pt3[0]);
	Point3->get_y(&pt3[1]);
	Point3->get_z(&pt3[2]);

	if (ic_pt_equal(pt2, pt3))
		return E_INVALIDARG;

	if (ic_pt_equal(pt1, pt3))
		return E_INVALIDARG;

	sds_point xvec, yvec, zvec;
	ic_sub (pt1, pt2, xvec);
	ic_sub (pt3, pt2, yvec);
	ic_crossproduct(xvec, yvec, zvec);
	if (db_unitizevector(zvec) != 0)
		return E_INVALIDARG;

	sds_matrix mtx;
	bool ok;
	ic_mirrorPlane(pt2, zvec, ok, mtx);
	if (!ok)
		return E_FAIL;

	db_handitem *pDbHandItem = NULL;

	if (MakeCopy == VARIANT_TRUE)
		{
		pDbHandItem = CopyHip (m_pDbHandItem);
		if (pDbHandItem == NULL)
			return E_FAIL;

		if (!AddToDrawing (m_pDoc, pDbHandItem))
			return E_FAIL;

		if (FAILED(CreateEntityWrapper (m_pDoc, NULL, pDbHandItem, MirroredEntity)))
			return E_FAIL;
		}
	else
		pDbHandItem = m_pDbHandItem;


	m_pDoc->SetModifiedFlag(TRUE);
	return (m_pDoc->XformEntity (pDbHandItem, mtx));
	}

STDMETHODIMP CAutoEntity::Move (IIcadPoint * Point1, IIcadPoint * Point2)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	sds_point pt1;
	pt1[0] = pt1[1] = pt1[2] = 0.0;
	if (Point1)
		{
		Point1->get_x(&pt1[0]);
		Point1->get_y(&pt1[1]);
		Point1->get_z(&pt1[2]);
		}

	sds_point pt2;
	pt2[0] = pt2[1] = pt2[2] = 0.0;
	if (Point2)
		{
		Point2->get_x(&pt2[0]);
		Point2->get_y(&pt2[1]);
		Point2->get_z(&pt2[2]);
		}

	sds_matrix mtx;
	ic_transmat ((pt2[0] - pt1[0]), (pt2[1] - pt1[1]), (pt2[2] - pt1[2]), mtx);

	m_pDoc->SetModifiedFlag(TRUE);
	return (m_pDoc->XformEntity (m_pDbHandItem, mtx));
	}

STDMETHODIMP CAutoEntity::Rotate (IIcadPoint * BasePoint, double theta)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	sds_matrix mtx;
	sds_point pt;
	pt[0] = pt[1] = pt[2] = 0.0;
	if (BasePoint)
		{
		BasePoint->get_x(&pt[0]);
		BasePoint->get_y(&pt[1]);
		BasePoint->get_z(&pt[2]);
		}

	//this matrix composition was copied from cmds\scale_and_rotate.cpp line 523
	mtx[0][0]=cos(theta); mtx[0][1]=(-sin(theta)); mtx[0][2]=0.0; mtx[0][3]=((pt[1]*sin(theta))+(pt[0]*(1-cos(theta))));
	mtx[1][0]=sin(theta); mtx[1][1]=cos(theta);    mtx[1][2]=0.0; mtx[1][3]=(-(pt[0]*sin(theta))+(pt[1]*(1-cos(theta))));
	mtx[2][0]=0.0;        mtx[2][1]=0.0;           mtx[2][2]=1.0; mtx[2][3]=0.0;
	mtx[3][0]=0.0;        mtx[3][1]=0.0;           mtx[3][2]=0.0; mtx[3][3]=1.0;

	m_pDoc->SetModifiedFlag(TRUE);
	return (m_pDoc->XformEntity (m_pDbHandItem, mtx));
	}

STDMETHODIMP CAutoEntity::Rotate3D (IIcadPoint * Point1, IIcadPoint * Point2, double RotationAngle)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (!Point1)
		return E_INVALIDARG;

	if (!Point2)
		return E_INVALIDARG;

	sds_point pt1, pt2;
	Point1->get_x (&pt1[0]);
	Point1->get_y (&pt1[1]);
	Point1->get_z (&pt1[2]);
	Point2->get_x (&pt2[0]);
	Point2->get_y (&pt2[1]);
	Point2->get_z (&pt2[2]);

	sds_point extrusionvec;
	ic_sub (pt1, pt2, extrusionvec);

	//ensure that the 2 points are not the same
	sds_point v;
	ic_ptcpy (v, extrusionvec);
	if (db_unitizevector(v) != 0)
		return E_INVALIDARG;

	sds_matrix mtx;
	ic_rotxform (pt1, extrusionvec, RotationAngle, mtx);

	m_pDoc->SetModifiedFlag(TRUE);
	return (m_pDoc->XformEntity (m_pDbHandItem, mtx));
	}

STDMETHODIMP CAutoEntity::ScaleEntity (IIcadPoint * BasePoint, double scl)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (scl <= 0.0)
		return E_INVALIDARG;

	sds_matrix mtx;
	sds_point pt;
	pt[0] = pt[1] = pt[2] = 0.0;
	if (BasePoint)
		{
		BasePoint->get_x(&pt[0]);
		BasePoint->get_y(&pt[1]);
		BasePoint->get_z(&pt[2]);
		}

	//this matrix composition was copied from cmds\scale_and_rotate.cpp line 136
	mtx[0][0]=scl; mtx[0][1]=0.0; mtx[0][2]=0.0; mtx[0][3]=(-(scl-1)*pt[0]);
	mtx[1][0]=0.0; mtx[1][1]=scl; mtx[1][2]=0.0; mtx[1][3]=(-(scl-1)*pt[1]);
	mtx[2][0]=0.0; mtx[2][1]=0.0; mtx[2][2]=scl; mtx[2][3]=(-(scl-1)*pt[2]);
	mtx[3][0]=0.0; mtx[3][1]=0.0; mtx[3][2]=0.0; mtx[3][3]=1.0;

	m_pDoc->SetModifiedFlag(TRUE);
	return (m_pDoc->XformEntity (m_pDbHandItem, mtx));
	}

STDMETHODIMP CAutoEntity::TransformBy (IIcadMatrix * Matrix)
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	sds_matrix mtx;
	ic_idmat (mtx);

	if (Matrix)
		{
		int i, j;
		for (i = 0; i < 4; i++)
			for (j = 0; j < 4; j++)
				Matrix->GetValue (i, j, &(mtx[i][j]));
		}

	m_pDoc->SetModifiedFlag(TRUE);
	return (m_pDoc->XformEntity (m_pDbHandItem, mtx));
	}

STDMETHODIMP CAutoEntity::Update ()
	{
	RETURN_IF_INVALID (m_pDbHandItem);
	/*
	sds_name ename;
	ename[0]=(long)m_pDbHandItem;
	ename[1]=(long)m_pDoc->m_dbDrawing;

	HINSTANCE hInst;
	int (*sdsFunc)(sds_name ename);
	if((hInst = GetModuleHandle("icad.exe"/*DNT*))!=NULL &&
		(sdsFunc = (int (*)(sds_name ename))GetProcAddress(hInst,"sds_entupd"/*DNT*))!=NULL)
		{
		if ((*sdsFunc)(ename) == RTNORM)
			return NOERROR;
		else
			return E_FAIL;
		}
	else
		return E_NOTIMPL;
		*/
	return m_pDoc->UpdateEntity (m_pDbHandItem);
	}



