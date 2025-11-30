/* C:\ICADDEV\PRJ\LIB\CMDS\GEOCONVERT.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

//Precompilation
#include "cmds.h"/*DNT*/
#include "Icad.h"/*DNT*/

#include "db.h"
#include "DbMText.h"
#include "gr.h"
#include "DoBase.h"
#include "Geometry.h"
#include "Array.h"
#include "Point.h"
#include "Knots.h"
#include "CtrlPts.h"
#include "Curve.h"
#include "NURBS.h"
#include "EllipArc.h"
#include "Line.h"
#include "Path.h"
#include "listcoll.h" // for HandItemCollection
#include "trnsform.h"
#include "GeoConvert.h"
#include "SplineUtils.h"
#include "IcadView.h"

DB_SOURCE_NAME;

/*
  Entities used in BPOLY

  Name		Coodinate		DXF 210
  --------	--------------	-------
  3dface	WCS				No
  Arc		ECS				Yes
  Circle	ECS				Yes
  Ellipse	WCS				Yes
  Image		WCS				No
  Line		WCS				Yes
  Mline		WCS				Yes
  Ray/XLine	WCS				No
  Polyline	ECS				Yes

*/

//////////////////////////////////////////////////////////////////
//							Utilities							//
//																//

void SdsGetUcsZ(sds_point result)
	{
	ASSERT(result);
	//Get the z axis of current UCS.
	struct resbuf rbx,rby;
	SDS_getvar(NULL,DB_QUCSXDIR,&rbx,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	SDS_getvar(NULL,DB_QUCSYDIR,&rby,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	//Set rbFrom.rpoint to the z axis of current UCS (treat this as an entity's extrusion vector)
	ic_crossproduct(rbx.resval.rpoint,rby.resval.rpoint,result);
	}

void RptToSds(
	C3Point & P,	// I: Geo-style point
	sds_point q)	// O: sds-style point
	{
	q[0] = P.X();
	q[1] = P.Y();
	q[2] = P.Z();
	}

//////////////////////////////////////////////////////////////////
//					Implementation of CDWGToGeo					//
//
// Construct a Geo line segment from DWG line entity
RC CDWGToGeo::ExtractLine(
	db_line* pLine,		// I: Line entity's DB handle
    bool bInWCS,
	PPath & pPath)		// I: Corresponding GEO PPath containing the line
	{
	sds_point p;
	C3Point A, B;
	PCurve pCurve = NULL;

	sds_point pNormal;
	pLine->get_210(pNormal);
    // Get the data.  The start and end points are in WCS.
	pLine->get_10(p); 
    if (!bInWCS)
        SdsWcsToEcs(p, p, pNormal);

	SdsToRpt(p, A); 
    if (!bInWCS && m_bProject) 
        A.SetZ(0.0);

	pLine->get_11(p); 
    if (!bInWCS)
        SdsWcsToEcs(p, p, pNormal);

	SdsToRpt(p, B); 
    if (!bInWCS && m_bProject) 
        B.SetZ(0.0);

	// Construct the line
	RC rc = ALLOC_FAIL(pCurve = new CLine(A, B));
	if (pCurve)
    {  // begin creating a new boundary and adding the line (pCurve) to it
		pPath = new CPath();
		pPath->Add(pCurve);
        pPath->WrapUp();
	}  //   end creating a new boundary and adding the line (pCurve) to it

	m_pCurrent = pLine->next;

	RETURN_RC(CDWGToGeo::ExtractLine, rc);
	}
/*****************************************************************/
// Construct a Geo arc from a DWG arc
RC CDWGToGeo::ExtractArc(
	db_arc* pArc,		// I: Arc entity's DB handle
    bool bInWCS,
	PPath & pPath)		// O: Corresponding GEO PPath containing the arc..
	{
	RC rc = SUCCESS;
	sds_point p;
	C3Point ptCenter;
	double radius, start, end;
	PCurve pCurve = NULL;

	// Get the data
	sds_point normal;
	pArc->get_210(normal);

    pArc->get_10(p);
    if (bInWCS)
        SdsEcsToWcs(p, p, normal);

	SdsToRpt(p, ptCenter); 
    if (!bInWCS && m_bProject) 
        ptCenter.SetZ(0.0);
    pArc->get_40(&radius);
    pArc->get_50(&start);
    pArc->get_51(&end);

	// Make sure end > start
	while (end <= start)
		end += TWOPI;

	// Construct the arc
	// Note:  the arc that is constructed will be in a plane of
	//        constant z; the z-value taken from ptCenter.
	double sweep = end - start;
    pCurve = (bInWCS) ? 
             new CEllipArc(radius, ptCenter, C3Vector(normal[0], normal[1], normal[2]), start, sweep, rc) :
             new CEllipArc(radius, ptCenter, start, sweep, rc);
	if (!pCurve)
		rc = DM_MEM;

	if (rc)
		{
		delete pCurve;
		pCurve = NULL;
		}
	else
	{   // begin creating a new boundary and adding the Geo arc (pCurve) to it
		pPath = new CPath();
		pPath->Add(pCurve);
		pPath->WrapUp();
	}   //   end creating a new path and adding the Geo arc (pCurve) to it

	m_pCurrent = pArc->next;

	RETURN_RC(ArcFromEdge, rc);
	}

RC CDWGToGeo::ExtractCircle(
		db_circle* pCircle,
        bool bInWCS,
		PPath & pPath)
  {
	RC rc = SUCCESS;
	sds_point p;
	C3Point ptCenter;
	double radius;
	PCurve pCurve = NULL;

	// Get the data
	sds_point normal;
	pCircle->get_210(normal);

    pCircle->get_10(p);
    if (bInWCS)
        SdsEcsToWcs(p, p, normal);

	SdsToRpt(p, ptCenter);
    pCircle->get_40(&radius);

	if (!bInWCS && m_bProject)	
        ptCenter.SetZ(0.0);

	// Construct the circle
    rc = ALLOC_FAIL(pCurve = (bInWCS) ? 
                             new CEllipse(ptCenter, radius, C3Vector(normal[0], normal[1], normal[2]), rc) :
                             new CEllipse(ptCenter, radius, rc));
	if (pCurve)
		{   // begin creating a new boundary and adding the Geo circle (pCurve) to it
		rc = ALLOC_FAIL(pPath = new CPath());
		if (pPath)
			{
			pPath->Add(pCurve);
			pPath->WrapUp();
			}
		else
			delete pCurve;
		}   // end creating a new path and adding the Geo circle (pCurve) to it

	m_pCurrent = pCircle->next;

	RETURN_RC(CDWGToGeo::ExtractCircle, rc);
	}

RC CDWGToGeo::ExtractEllipse(
	db_ellipse* pEllipse,		// I: Ellipse entity's DB handle
    bool bInWCS,
	PPath & pPath)			    // O: Corresponding GEO PPath containing the ellipse.
	{

	RC rc = SUCCESS;
	sds_point p;
	C3Point ptCenter, ptMajorAxis, ptMinorAxis;
	double start, end;
	double ratioMinorToMajor; //Ratio of minor axis to major axis
	PCurve pCurve = NULL;

	sds_point pNormal;
	pEllipse->get_210(pNormal);
	// Get the data:  WCS coordinates.
	// DXF code, 11 is a vector.
    pEllipse->get_10(p); 
    if (!bInWCS)
        SdsWcsToEcs(p, p, pNormal); 
    
    SdsToRpt(p, ptCenter); 
    if (!bInWCS && m_bProject) 
        ptCenter.SetZ(0.0);

	pEllipse->get_11(p); 
    if (!bInWCS)
        SdsWcsToEcs(p, p, pNormal, true); 
    
    SdsToRpt(p, ptMajorAxis); 
    if (!bInWCS && m_bProject) 
        ptMajorAxis.SetZ(0.0);

    pEllipse->get_40(&ratioMinorToMajor);
    pEllipse->get_41(&start);
    pEllipse->get_42(&end);

	// Make sure end > start
	while (end <= start)
		end += TWOPI;

    if (!bInWCS) {
	    ptMinorAxis.SetX(-ptMajorAxis.Y()*sin(TWOPI/4));	// 90 degreee counterclockwise rotation to get minor axis vector
	    ptMinorAxis.SetY(ptMajorAxis.X()*sin(TWOPI/4));
	    ptMinorAxis.SetZ(ptMajorAxis.Z());
	    ptMinorAxis = ptMinorAxis * ratioMinorToMajor;		// Scale it properly
    }
    else {
		C3Point normal(pNormal[0], pNormal[1], pNormal[2]);
		normal.Unitize();
		ptMinorAxis = normal ^ ptMajorAxis;
		ptMinorAxis.Unitize();

	    double minorLength =  ptMajorAxis.Norm() * ratioMinorToMajor;
	    ptMinorAxis = ptMinorAxis * minorLength;	
    }

	rc = ALLOC_FAIL(pCurve = new CEllipArc(ptCenter, ptMajorAxis, ptMinorAxis, start, end));
	if (pCurve)
		{   // begin creating a new boundary and adding the Geo ellipse (pCurve) to it
		rc = ALLOC_FAIL(pPath = new CPath());
		if (pPath)
			{
			pPath->Add(pCurve);
			pPath->WrapUp();
			}
		else
			delete pCurve;
		}   //   end creating a new path and adding the Geo ellipse (pCurve) to it

	m_pCurrent = pEllipse->next;

	RETURN_RC(CDWGToGeo::ExtractEllipse, rc);
	}

RC CDWGToGeo::ExtractRayOrXline(
	db_entity* pRayOrXline,		    // I: Ray entity's DB handle
    bool bInWCS,
	PPath & pPath)				    // I: Corresponding GEO PPath containing the ray
	{
	RC rc = SUCCESS;
	sds_point start, dirVector;
	sds_point projStart, projEnd;	//Projected start and end point
	C3Point ptStart, ptEnd;
	PCurve pCurve = NULL;

	sds_point pNormal;
	SdsGetUcsZ(pNormal);

	pPath = NULL;
	if (pRayOrXline->ret_type() != DB_RAY &&
		pRayOrXline->ret_type() != DB_XLINE)
		rc = FAILURE;
	else
		{
		// Get the data
		pRayOrXline->get_10(start);
		pRayOrXline->get_11(dirVector);

		//Clip the xline or ray using the current view's extents
		gr_rayxlinegenpts(start,
						dirVector,
						pRayOrXline->ret_type(),
						projStart,
						projEnd,
						SDS_CURVIEW->m_pCurDwgView,
						SDS_CURDWG);

        if (!bInWCS)
		    SdsWcsToEcs(projStart, projStart, pNormal); 
        SdsToRpt(projStart, ptStart); 
        if (!bInWCS && m_bProject) 
            ptStart.SetZ(0.0);

        if (!bInWCS)
		    SdsWcsToEcs(projEnd, projEnd, pNormal); 
        SdsToRpt(projEnd, ptEnd); 
        if (!bInWCS && m_bProject) 
            ptEnd.SetZ(0.0);

		// Construct the bounded ray or xline
		// NOTE: CRay does not return valid EndPoint, and consequently deleted from CPath.
		rc = ALLOC_FAIL(pCurve = new CLine(ptStart, ptEnd));
		if (pCurve)
			{  // begin creating a new boundary and adding the ray or xline (pCurve) to it
			rc = ALLOC_FAIL(pPath = new CPath());
			if (pPath)
				{
				pPath->Add(pCurve);
				pPath->WrapUp();
				}
			else
				delete pCurve;
			}  //   end creating a new boundary and adding the ray or xline (pCurve) to it
		}

	m_pCurrent = pRayOrXline->next;

	RETURN_RC(CDWGToGeo::ExtractRayOrXline, rc);
	}


RC CDWGToGeo::Extract3dface(
		db_3dface* p3dface,
        bool bInWCS,
		PPath & pPath)
	{
	RC rc=SUCCESS;
	int nVerts = 4;
	sds_point p;
	int fInvisibleEdges;
	C3Point ptCorners[4];	//Up to 4 corner points(edges)
	PCurve pCurve = NULL;
	
	pPath = NULL;

	sds_point pNormal;
	SdsGetUcsZ(pNormal);
    // Get the corner points of 3dface. These coordinates are in WCS.
	p3dface->get_10(p); 
    if (!bInWCS)
        SdsWcsToEcs(p, p, pNormal); 
    SdsToRpt(p, ptCorners[0]);

	p3dface->get_11(p); 
    if (!bInWCS)
        SdsWcsToEcs(p, p, pNormal); 
    SdsToRpt(p, ptCorners[1]);

	p3dface->get_12(p);
    if (!bInWCS)
        SdsWcsToEcs(p, p, pNormal); 
    SdsToRpt(p, ptCorners[2]);

	p3dface->get_13(p); 
    if (!bInWCS)
        SdsWcsToEcs(p, p, pNormal); 
    SdsToRpt(p, ptCorners[3]);

	if (ptCorners[2] == ptCorners[3])	//Check if 3dface has only 3 edges.
		nVerts = 3;
	/*
	Invisible edges flags:
	1 = First edge is invisible.
	2 = Second edge is invisible.
	4 = Third edge is invisible.
	8 = Fourth edge is invisible.
	*/
	p3dface->get_70(&fInvisibleEdges);

	if (fInvisibleEdges != 15)	//Check if all four edges are invisible.
		{
		rc = ALLOC_FAIL(pPath = new CPath());
		if (pPath)
			{
			for (int nVertex=0, nEdge=1; nVertex<nVerts; nVertex++, nEdge <<= 1)
				{
				if (!(fInvisibleEdges & nEdge))
					{
					rc = ALLOC_FAIL(pCurve = new CLine(ptCorners[nVertex], ptCorners[(nVertex+1) % nVerts]));	
					if (pCurve)
						pPath->Add(pCurve);
					if (rc)
						break;
					}
				
				}
			pPath->WrapUp();
			}        
		}		
	else
		rc = FAILURE;

	m_pCurrent = p3dface->next;

	RETURN_RC(CDWGToGeo::Extract3dface, rc);
	}

RC CDWGToGeo::ExtractImage(
		db_image* pImage,
        bool bInWCS,
		PPath & pPath)
	{
	RC rc = SUCCESS;
	sds_point p;
	C3Point ptInsertion, ptUWidth, ptVHeight, ptImageSize;
	PCurve pCurve = NULL;
	C3Point ptImages[4];

	sds_point pNormal;
	SdsGetUcsZ(pNormal);

	// Get the data.
	// NOTE: The clipping points are in image source pixel coordinate, but
	// Insertion point, U and V vectors are in WCS. (DXF Document says ECS, but that's not true)
    pImage->get_10(p); 
    if (!bInWCS)
        SdsWcsToEcs(p, p, pNormal); 	
    SdsToRpt(p, ptInsertion);

    pImage->get_11(p); 
    if (!bInWCS)
        SdsWcsToEcs(p, p, pNormal, true);
	SdsToRpt(p, ptUWidth);		//U-vector of a single pixel (Width)
    
	pImage->get_12(p); 
    if (!bInWCS)
        SdsWcsToEcs(p, p, pNormal, true);
	SdsToRpt(p, ptVHeight);		//V-vector of a single pixel (Height)

	pImage->get_13(p);
	SdsToRpt(p, ptImageSize);	//Image size in pixels

	ptImages[0] = ptInsertion;
	ptImages[1] = ptInsertion + ptVHeight*ptImageSize.Y();
	ptImages[2] = ptInsertion + ptVHeight*ptImageSize.Y() + ptUWidth*ptImageSize.X();
	ptImages[3] = ptInsertion + ptUWidth*ptImageSize.X();

	int bClipOn, ImageDisplayProp, bClipBoundaryType;
	pImage->get_280(&bClipOn);			//Clipping state: clipping points available, 0=OFF, 1=ON
	pImage->get_70(&ImageDisplayProp);	//Image display property:
										//	1 = Show image
										//	2 = Show image when not aligned with screen
										//	4 = Use clipping boundary
										//	8 = Transparency is on
	pImage->get_71(&bClipBoundaryType);	//Clipping boundary type:
										//	1 = rectangular, 2 = polygonal 


	//To enable the image clipping, comment out the below
	bClipOn=0;

	int nClipVerts=0;
	sds_point* pClipVerts=NULL;
	C3Point* ptClipVerts=NULL;
	if (bClipOn)
		{
		pImage->get_91(&nClipVerts);

		if (1 == bClipBoundaryType) // Rectangular clipping boundary
			{
			nClipVerts = 4;
			pClipVerts = new sds_point[nClipVerts];
			ptClipVerts = new C3Point[nClipVerts];
			pImage->get_14(pClipVerts, 2);	//Rectangular clipping boundary stores only the two opposite corners
			pClipVerts[2][0] = pClipVerts[1][0]; 
			pClipVerts[2][1] = pClipVerts[1][1];
			pClipVerts[2][2] = pClipVerts[1][2];

			pClipVerts[1][0] = pClipVerts[0][0]; 
			pClipVerts[1][1] = pClipVerts[2][1];
			pClipVerts[1][2] = pClipVerts[0][2];

			pClipVerts[3][0] = pClipVerts[2][0]; 
			pClipVerts[3][1] = pClipVerts[0][1];
			pClipVerts[3][2] = pClipVerts[0][2];
			}
		else
			{
			--nClipVerts;	//Polygonal clipping boundary's last point is same as the first point
			pClipVerts = new sds_point[nClipVerts];
			ptClipVerts = new C3Point[nClipVerts];
			pImage->get_14(pClipVerts, nClipVerts);
			}

		/*
		Affine transformation matrix from image source pixels to model coordinates:
		[1	0	0	0.5									]
		[0	-1	0	# of pixel in y-axis (V-vector)-0.5	]
		[0	0	0	1									]
		*/
		for (int i=0; i<nClipVerts; i++)
			{
			pClipVerts[i][0] += 0.5; pClipVerts[i][1] = ptImageSize.Y()-0.5- pClipVerts[i][1];  //See the comment above
			SdsToRpt(pClipVerts[i], ptClipVerts[i]);

			//Transform clipping vertices from pixel to model coordinate
			//Vertex in model coord. = Insertion Point + (V-vector * distance from vertex to X-axis) + (U-vector * distance from vertex to Y-axis)
			ptClipVerts[i] = ptInsertion + ptVHeight*ptClipVerts[i].Y() + ptUWidth*ptClipVerts[i].X();  //Convert from pixel to WCS
			}
		}

	int nVerts=4;
	C3Point* ptVerts=&ptImages[0];
	if ((ImageDisplayProp & 4) && bClipOn)
		{
		nVerts = nClipVerts;
		ptVerts = ptClipVerts;
		}

	// Construct the image
	rc = ALLOC_FAIL(pPath = new CPath());
	if (pPath)
		{
		for (int nVertex=0; nVertex < nVerts; nVertex++)
			{
			if (!bInWCS && m_bProject)
				{
				//Not an efficient way to set Z.
				ptVerts[nVertex].SetZ(0.0);  
				ptVerts[(nVertex+1) % nVerts].SetZ(0.0); 
				}
			rc = ALLOC_FAIL(pCurve = new CLine(ptVerts[nVertex], ptVerts[(nVertex+1) % nVerts]));
			if (pCurve)
				{
				pPath->Add(pCurve);
				}
			}
		pPath->WrapUp();
		}

	delete [] pClipVerts;
	delete [] ptClipVerts;

	m_pCurrent = pImage->next;

	RETURN_RC(CDWGToGeo::ExtractImage, rc);
	}

//To see how to draw mline, take a look at GR\MLINEGEN.CPP
RC CDWGToGeo::ExtractMline(
	db_mline* pMline,			// I: Mline entity's DB handle
    bool bInWCS,
	stdPPathList &pPathList)	// O: Pointer to the corresponding GEO boundaries
	{
	RC rc = SUCCESS;
	PCurve pCurve = NULL;
	PPath pPath;

	db_handitem* pMlineStyleEntity=NULL;
	db_mlvertlink* pVertLink=NULL;
	int nVerts, nElems, nMlineFlags;
	int nStyleFlag=0;
    double scale;
	int nFlag;

    pVertLink=((db_mline *)(pMline))->ret_vert();

    pMline->get_40(&scale);
    pMline->get_71(&nMlineFlags);
    pMline->get_72(&nVerts);
    pMline->get_73(&nElems);
	pMline->get_340(&pMlineStyleEntity);	

	if (pMlineStyleEntity)
		pMlineStyleEntity->get_70(&nStyleFlag);

	sds_point pNormal, pVertex;
	pMline->get_210(pNormal);

	int elem, vert, vertNext;
	C3Point ptStart, ptEnd;
	//Process elements
	for (elem=0; elem<nElems; elem++)
		{
		rc = ALLOC_FAIL(pPath = new CPath());
		if (pPath)
			{
			for (vert=0; vert<((nMlineFlags & IC_MLINE_CLOSED) ? nVerts : (nVerts-1)); vert++)
				{
				pVertex[0] = pVertLink[vert].pt[0]+pVertLink[vert].elem[elem].lpar[0]*pVertLink[vert].miterdir[0];
				pVertex[1] = pVertLink[vert].pt[1]+pVertLink[vert].elem[elem].lpar[0]*pVertLink[vert].miterdir[1];
				pVertex[2] = pVertLink[vert].pt[2]+pVertLink[vert].elem[elem].lpar[0]*pVertLink[vert].miterdir[2];

                if (!bInWCS)
				    SdsWcsToEcs(pVertex, pVertex, pNormal); 
                SdsToRpt(pVertex, ptStart); 
                if (!bInWCS && m_bProject) 
                    ptStart.SetZ(0.0);

				vertNext = (vert+1) % nVerts;
				pVertex[0] = pVertLink[vertNext].pt[0]+pVertLink[vertNext].elem[elem].lpar[0]*pVertLink[vertNext].miterdir[0];
				pVertex[1] = pVertLink[vertNext].pt[1]+pVertLink[vertNext].elem[elem].lpar[0]*pVertLink[vertNext].miterdir[1];
				pVertex[2] = pVertLink[vertNext].pt[2]+pVertLink[vertNext].elem[elem].lpar[0]*pVertLink[vertNext].miterdir[2];

                if (!bInWCS)
				    SdsWcsToEcs(pVertex, pVertex, pNormal); 
                SdsToRpt(pVertex, ptEnd); 
                if (!bInWCS && m_bProject) 
                    ptEnd.SetZ(0.0);

				rc = ALLOC_FAIL(pCurve = new CLine(ptStart, ptEnd));
				if (pCurve)
					pPath->Add(pCurve);
				}
			pPath->WrapUp();
			pPathList.push_back(pPath);
			}
		}

	//Process miter lines and line caps
	for (vert=0; vert<nVerts; vert++)
		{

		nFlag=((nMlineFlags & IC_MLINE_CLOSED) && !(nStyleFlag & IC_MLSTYLE_SHOW_MITERS));	/* Closed and has no miter lines. */

		/* No beginning line if it's open and has no start line cap, */
		/* or it's closed and has no miter lines: */
		if (vert<1 && (!(nMlineFlags & IC_MLINE_CLOSED) && !(nStyleFlag & IC_MLSTYLE_START_SQUARE)) || nFlag) continue;

		/* No ending one if it's open and has no ending line cap, */
		/* or it's closed and has no miter lines: */
		if (vert>=nVerts-1 && (!(nMlineFlags & IC_MLINE_CLOSED) && !(nStyleFlag & IC_MLSTYLE_END_SQUARE)) || nFlag) continue;

		/* No internal ones if we don't have miter lines: */
		if (vert>0 && vert<nVerts-1 && !(nStyleFlag & IC_MLSTYLE_SHOW_MITERS)) continue;


		pVertex[0] = pVertLink[vert].pt[0]+pVertLink[vert].elem[0].lpar[0]*pVertLink[vert].miterdir[0];
		pVertex[1] = pVertLink[vert].pt[1]+pVertLink[vert].elem[0].lpar[0]*pVertLink[vert].miterdir[1];
		pVertex[2] = pVertLink[vert].pt[2]+pVertLink[vert].elem[0].lpar[0]*pVertLink[vert].miterdir[2];
				
        if (!bInWCS)
		    SdsWcsToEcs(pVertex, pVertex, pNormal); 
        SdsToRpt(pVertex, ptStart); 
        if (!bInWCS && m_bProject) 
            ptStart.SetZ(0.0);

        pVertex[0] = pVertLink[vert].pt[0]+pVertLink[vert].elem[nElems-1].lpar[0]*pVertLink[vert].miterdir[0];
        pVertex[1] = pVertLink[vert].pt[1]+pVertLink[vert].elem[nElems-1].lpar[0]*pVertLink[vert].miterdir[1];
        pVertex[2] = pVertLink[vert].pt[2]+pVertLink[vert].elem[nElems-1].lpar[0]*pVertLink[vert].miterdir[2];

        if (!bInWCS)
		    SdsWcsToEcs(pVertex, pVertex, pNormal); 
        SdsToRpt(pVertex, ptEnd); 
        if (!bInWCS && m_bProject) 
            ptEnd.SetZ(0.0);

		rc = ALLOC_FAIL(pPath = new CPath());
		if (pPath)
			{
			rc = ALLOC_FAIL(pCurve = new CLine(ptStart, ptEnd));
			if (pCurve)
				pPath->Add(pCurve);
			pPath->WrapUp();
			pPathList.push_back(pPath);
			}
		}

	//Process arc caps
	double bulge;
	int nArcCaps,nEnding,nEndingStart,nEndingEnd;
	int e0, e1;		// Element start, end
	int v0;			// Vertex index
	int nCapIndex;
	if (nMlineFlags!=IC_MLINE_CLOSED) // Only if it's open.
		{	
		nEndingStart = 0;
		nEndingEnd = 2;
		if (nMlineFlags & IC_MLINE_SUPPRESS_START_CAPS) nEndingStart=1;
		if (nMlineFlags & IC_MLINE_SUPPRESS_END_CAPS) nEndingEnd=1;

		nArcCaps=nElems/2;  // The number of possible arc caps.
		
		for (nEnding=nEndingStart; nEnding<nEndingEnd; nEnding++) 
			{	/* Beginning arcs, then ending. */

			for (nCapIndex=0; nCapIndex<nArcCaps; nCapIndex++) 
				{  /* Each cap */

				/* Avoid outers if we aren't supposed to do them: */
				if (!nCapIndex && !(nStyleFlag & ((nEnding) ? IC_MLSTYLE_END_ROUND : IC_MLSTYLE_START_ROUND))) continue;

				/* Avoid inners if we aren't supposed to do them: */
				if (nCapIndex && !(nStyleFlag & ((nEnding) ? IC_MLSTYLE_END_INNER_ARC : IC_MLSTYLE_START_INNER_ARC))) break;

				/* We're pairing elements from the outside in.  Set e0 and e1 */
				/* such that the CCW arc will go from element e0 to e1. */
				/* (Note the reversal effect of a negative scale!) */
				if ((nEnding && scale>=0.0) || (!nEnding && scale<=0.0))
					{
					e1=nCapIndex; 
					e0=nElems-1-e1; 
					}
				else
					{
					e0=nCapIndex; 
					e1=nElems-1-e0;
					}

				v0 = (nEnding) ? nVerts-1 : 0;  /* Which vertex */

				pVertex[0] = pVertLink[v0].pt[0]+pVertLink[v0].elem[e0].lpar[0]*pVertLink[v0].miterdir[0];
				pVertex[1] = pVertLink[v0].pt[1]+pVertLink[v0].elem[e0].lpar[0]*pVertLink[v0].miterdir[1];
				pVertex[2] = pVertLink[v0].pt[2]+pVertLink[v0].elem[e0].lpar[0]*pVertLink[v0].miterdir[2];

                if (!bInWCS)
				    SdsWcsToEcs(pVertex, pVertex, pNormal); 
                SdsToRpt(pVertex, ptStart); 
                if (!bInWCS && m_bProject) 
                    ptStart.SetZ(0.0);

				pVertex[0] = pVertLink[v0].pt[0]+pVertLink[v0].elem[e1].lpar[0]*pVertLink[v0].miterdir[0];
				pVertex[1] = pVertLink[v0].pt[1]+pVertLink[v0].elem[e1].lpar[0]*pVertLink[v0].miterdir[1];
				pVertex[2] = pVertLink[v0].pt[2]+pVertLink[v0].elem[e1].lpar[0]*pVertLink[v0].miterdir[2];

                if (!bInWCS)
				    SdsWcsToEcs(pVertex, pVertex, pNormal); 
                SdsToRpt(pVertex, ptEnd); 
                if (!bInWCS && m_bProject) 
                    ptEnd.SetZ(0.0);

				bulge = 1;  //Semicircle, Counterclockwise

				rc = ALLOC_FAIL(pPath = new CPath());
				if (pPath)
					{
					rc = ALLOC_FAIL(pCurve = new CEllipArc(ptStart, ptEnd, bulge, rc));
					if (pCurve)
						pPath->Add(pCurve);

					pPath->WrapUp();
					pPathList.push_back(pPath);
					}
	
	
				}  /* End for each cap on this end */

			}  /* End for ending */

		}  /* End of arc caps */


	RETURN_RC(CDWGToGeo::ExtractMline, rc);
	}
/*****************************************************************/

// determine whether a db_handitem is a Spline Frame Vertex
static bool isSplineFrameVertex(db_handitem * pHanditem)
{
	bool SplineFrameVertex;
	int flags;

	if (pHanditem->ret_type() == DB_VERTEX)
	{
		pHanditem->get_70(&flags);

		if (flags & IC_VERTEX_SPLINEFRAME)
			SplineFrameVertex = true;
		else
			SplineFrameVertex = false;

	}
	else
	{
		SplineFrameVertex = false;
	}

	return SplineFrameVertex;
}

/*****************************************************************/
// Get the next polyline vertex
RC CDWGToGeo::GetNextVertex(
	C3Point& P,			// O: The vertex point
	double& bulge,		// O: The bulge
    sds_point* pNormal) 
	{
	sds_point point;

	// get the next handle
	m_pCurrent = m_pCurrent->next;

	// skip over all db_handitems that are of type DB_VERTEX
	// but that are flagged as Spline Frame Control Points
	// This type of vertex should not participate in the
	// DWGToGeo conversion.

	while (isSplineFrameVertex(m_pCurrent))
	{
		m_pCurrent = m_pCurrent->next;
	}

	// check to see if it is a vertex.
	if (m_pCurrent->ret_type() != DB_VERTEX)
		return E_BadInput;

	// Get the data
	m_pCurrent->get_10(point);
    if (pNormal)
        SdsEcsToWcs(point, point, *pNormal, true);

	SdsToRpt(point, P);	
    if (!pNormal && m_bProject)	
        P.SetZ(0.0);
	m_pCurrent->get_42(&bulge);

	return SUCCESS;
	}
/*****************************************************************/
// Add a polyline edge to the path
RC CDWGToGeo::ExtractPolyEdge(
	C3Point& P,			// I: edge's start point
	double bulge,		// I: The bulge
	C3Point& Q,			// I: edge's end point
    C3Point* pNormal,   // I: Optional input for the normal of pline
	PPath & pPath)      // IO: Path to which to add the next edge
	{
	RC rc = SUCCESS;
	PCurve pCurve = NULL;

	if (PointDistance(P, Q) < FUZZ_DIST)
		// It's a degenerate edge, skip it
		goto exit;

	if (bulge != 0)
		{
		// Try to construct an arc
        pCurve = new CEllipArc(P, Q, bulge, rc, pNormal);
		if (!pCurve)
			{
			rc = DM_MEM;
			QUIT
			}

		if (rc)
			{
			// Arc construction failed, we'll try a line segment
			delete pCurve;
			pCurve = NULL;
			}
		}

	// if arc failed or if a line instead of an arc - create a line
	if (rc || bulge == 0)
		if (rc = ALLOC_FAIL(pCurve = new CLine(P, Q)))
			QUIT


    pPath->Add(pCurve);


exit:
	RETURN_RC(CDWGToGeo::ExtractPolyEdge, rc);
	}
/*****************************************************************/
// Convert a DWG polyline to a Geo Path made up of GEO curves.
RC CDWGToGeo::ExtractPolyline(
	db_polyline* pPoly,	 // I: Polyline's DB handle
    bool bInWCS,
	PPath & pPath)       // O: Corresponding GEO PPath containing the polyline
	{
	RC rc = SUCCESS;
	int flags;
	double b, bNext;
	C3Point P, PNext, PFirst;

	m_pCurrent = pPoly;

    sds_point normal;
    pPoly->get_210(normal);
    C3Point ptNormal(normal[0], normal[1], normal[2]);

	// Get the flags and reject if this is a mesh
	pPoly->get_70(&flags);
	if ( (flags & 16) || (flags & 64) )
		{
		rc = E_BadInput;
		QUIT
		}


		pPath = new CPath();

	// If we get here we should have data for a non-trivial polyline.

	// Get the first vertex
    GetNextVertex(P, b, (bInWCS) ? &normal : NULL);
	PFirst = P;


	while (GetNextVertex(PNext, bNext, (bInWCS) ? &normal : NULL) == SUCCESS)
		{

        if (rc = (!bInWCS) ? 
                  ExtractPolyEdge(P, b, PNext, NULL, pPath) : 
                  ExtractPolyEdge(P, b, PNext, &ptNormal, pPath))
			QUIT
		P = PNext;
		b = bNext;
		}
			
	// Create the closing edge, if the polyline is closed
	if (flags & 1)
		{
		if (rc = (!bInWCS) ? 
                  ExtractPolyEdge(P, b, PFirst, NULL, pPath) : 
                  ExtractPolyEdge(P, b, PNext, &ptNormal, pPath))
			QUIT
		} // End if closed

	pPath->WrapUp();

	// Get the pointer to the next entity
	m_pCurrent = m_pCurrent->next;
exit:
	RETURN_RC(ExtractPolyline, rc);
	}
/*****************************************************************/
// Convert a DWG spline to a Geo Path made up of GEO curves.
RC CDWGToGeo::ExtractSpline(
							db_spline * pDWGSpline,
							PPath & pGEOPath)
{

	RC rc = SUCCESS;

	sds_resbuf * resbufSpline;

	sds_name ename;

	ename[0] = (long) pDWGSpline;
	ename[1] = (long) SDS_CURDWG;

	resbufSpline = sds_entget(ename);

	PSpline pGEOSpline;

	C3Point ptNormal;

	rc = GetSplineFrom_resbuf(resbufSpline, pGEOSpline, ptNormal);

	// Todo --- delete the resbufSpline

	if (rc == SUCCESS)
	{   // begin creating a new boundary and adding the pGEOSpline to it
		pGEOPath = new CPath();
		pGEOPath->Add(pGEOSpline);
		pGEOPath->WrapUp();
	}   //   end creating a new boundary and adding the pGEOSpline to it



	RETURN_RC(ExtractSpline, rc);
}
/********************************************************************************************/
// propoese new helper function for ExtractText

static RC GetOrientedBBox(C3Point AlignmentPt, // I: the first alignment point (DXF 10)
						  double height,       // I: the height (DXF 40)
						  double angle,        // I: the oblique angle (DXF 51)
						  C3Point LL,          // I: lower left corner of unoriented bbox
						  C3Point UR,          // I: upper right corner of unoriented bbox
						  double padding,      // I: the amount by which to pad
                          sds_point* pNormal,  // I: The normal of text
						  PPath & pPath)       // O: the oriented and padded bounding box
	{
	RC rc;
	
	
	
	C3Point A, B, C, D;
	
	B = AlignmentPt;
	
	// construct a vector whose direction is 'angle' + 90 and magnitude is equal to 'height'
	C3Point H(-height*sin(angle), height*cos(angle));
	
	A = B + H;
	
	// the point C will be at the intersection of the infinite line through B making
	// angle 'angle' with the X axis and the line x = xcoord of UR or x = xcoord of LL.
	
	if ( cos(angle) > FUZZ_GEN)
		C.SetX(UR.X());
	else if ( cos(angle) < -1.0*FUZZ_GEN)
		C.SetX(LL.X());
	else 
	    C.SetX(B.X());

	if (fabs(cos(angle) > FUZZ_GEN))
		C.SetY(B.Y() + tan(angle)*(C.X()-B.X()));
	else
		{
		if (sin(angle) > 0.0)
			{
			C.SetY(UR.Y());
			}
		else
			{
			C.SetY(LL.Y());
			}
		}

	
	D = C + H;

	// compute the tweak factors
	double tweakDown = 0.0;
	double tweakUp = 0.0;
	double tweakLeft = 0.0;
	double tweakRight = 0.0;

	// if angle is zero, do not tweak the box except for padding
	if (fabs(angle) > FUZZ_GEN)
		{  // begin computing tweak factors
		tweakDown = 0.5*height;
		tweakRight = 0.25*height;
		}  //   end computing tweak factors
	
	// now adjust the points to take the padding and tweak factors into consideration
	
	C3Point U = (C - B);
	U.Unitize();
	C3Point V = (A - B);
	V.Unitize();
	
	A = A - padding*U + padding*V;
	B = B - padding*U - padding*V - tweakDown*V;
	C = C + padding*U - padding*V - tweakDown*V + tweakRight*U;
	D = D + padding*U + padding*V + tweakRight*U;
	
	// lets check that we have a rectangle here
	// ASSERT( fabs((A - B)*(C - B)) < FUZZ_GEN );
	// ASSERT( fabs((D - C)*(B - C)) < FUZZ_GEN );
	// ASSERT( fabs((B - A)*(D - A)) < FUZZ_GEN );
	// ASSERT( fabs((A - D)*(C - D)) < FUZZ_GEN );
	
	// finally, build the path
    if (pNormal) {
        CAffine tr(C3Vector((*pNormal)[0], (*pNormal)[1], (*pNormal)[2]));
        tr.ApplyToPoint(A, A);
        tr.ApplyToPoint(B, B);
        tr.ApplyToPoint(C, C);
        tr.ApplyToPoint(D, D);
    }
	
	PCurve AB, BC, CD, DA;
	
	AB = new CLine(A,B);
	BC = new CLine(B,C);
	CD = new CLine(C,D);
	DA = new CLine(D,A);
	
	if (AB && BC && CD && DA)
		{
		
		rc = ALLOC_FAIL(pPath = new CPath());
		if (pPath)
			{
			pPath->Add(AB);
			pPath->Add(BC);
			pPath->Add(CD);
			pPath->Add(DA);
			pPath->WrapUp();
			}
		}
	else
		{
		rc = FAILURE;
		}
	
	
	
	RETURN_RC(GetOrientedBbox, rc);
	}


/*****************************************************************/
// Helper function for ExtractText
// The path from points A to B to C to D to A goes around the oriented text box
// in a counter clockwise manner

static RC GetOrientedBBoxFromExtents(C3Point Base,    // I:  the base point of the text
									   C3Point LL,   // I: the ll corner of the extents
									   C3Point UR,   // I: the ur corner of the extents
									   double theta, // I: the angle of the text
									   double padding, // I: the amount by which to pad
									   PPath & pPath) // O: the oriented bounding box 	
									   
	{
	RC rc;
	C3Point A, B, C, D;


	// first make the bounding box not taking theta into consideration
	B = LL;
	D = UR;
	A.SetX(B.X());
	A.SetY(UR.Y());
	C.SetX(UR.X());
	C.SetY(LL.Y());
	
	// now build a transformation that rotates theta about Base.
	CAffine T;
	T.RotateAbout(theta, Base);
	
	// now apply this transformation to the four points.
	
	T.ApplyToPoint(A, A);
	T.ApplyToPoint(B, B);
	T.ApplyToPoint(C, C);
	T.ApplyToPoint(D, D);

	// now adjust the points to take the padding factor into consideration

    C3Point U = (C - B);
    U.Unitize();
	C3Point V = (A - B);
	V.Unitize();
	U = padding*U;
	V = padding*V;

	A = A - U + V;
	B = B - U - V;
	C = C + U - V;
	D = D + U + V;

	

	// finally, build the path
	
	PCurve AB, BC, CD, DA;
	
	AB = new CLine(A,B);
	BC = new CLine(B,C);
	CD = new CLine(C,D);
	DA = new CLine(D,A);
	
	if (AB && BC && CD && DA)
		{
		
		rc = ALLOC_FAIL(pPath = new CPath());
		if (pPath)
			{
			pPath->Add(AB);
			pPath->Add(BC);
			pPath->Add(CD);
			pPath->Add(DA);
			pPath->WrapUp();
			}
		}
	else
		{
		rc = FAILURE;
		}

	return rc;
	
	
	}



/*****************************************************************/
// another helper function, RotateItem
static void RotateItem(
					   db_handitem * pItem,
					   double theta) // angle in radians
	{
	// we are going to rotate about the base point of the item
	sds_point basept;
	pItem->get_10(basept);

	// now we are going to build the sds_transform that rotates by
	// the given angle about the basepoint.  If angle > 0 the rotation
	// is counterclockwise, as per usual

	// basept[0] = basept[1] = basept[2] = 0.0; // try this instead

 
	sds_matrix mtx;

	ic_rotxform(basept, theta, mtx);

	sds_name nmEntity;

	nmEntity[0] = (long) pItem;
	nmEntity[1] = (long) SDS_CURDWG;

	// now we are going to apply the transformation to the item.

	SDS_xforment(nmEntity, NULL, mtx);

	}
/*****************************************************************/

/*****************************************************************/
static RC ExtractBbox(db_handitem * pItem,
					  double theta,
                      sds_point* pNormal,
					  PPath & pPath)
	{
	RC rc = FAILURE;

	C3Point AlignmentPt;
	double height;
	C3Point LL;
	C3Point UR;

	pItem->get_40(&height);
	double padding = height * .25;

	sds_point sdsll;
	sds_point sdsur;
	sds_point sdsbase;

#ifdef _USE_DISP_OBJ_PTS_3D_
//#pragma message("_USE_DISP_OBJ_PTS_3D_ defined in GeoConvert.cpp")
	CD3 minPoint, maxPoint;
	CDoBase::getPlanarBoundingBox(SDS_CURGRVW, static_cast<db_entity*>(pItem), minPoint, maxPoint);
	memcpy(sdsll, (sds_real*)minPoint, 3 * sizeof(sds_real));
	memcpy(sdsur, (sds_real*)maxPoint, 3 * sizeof(sds_real));
#else
	pItem->get_extent(sdsll, sdsur);
#endif
	pItem->get_10(sdsbase);


	SdsToRpt(sdsll, LL);
	SdsToRpt(sdsur, UR);
	SdsToRpt(sdsbase, AlignmentPt);

	// this may enable us to properly handle descenders in the case of theta == 0.0
	if (fabs(theta) < FUZZ_GEN)
		{
		AlignmentPt = LL;
		height = UR.Y() - LL.Y();
		}


	rc = GetOrientedBBox(AlignmentPt, height, theta, LL, UR, padding, pNormal, pPath);


	RETURN_RC(ExtractBbox, rc);
	}
/*****************************************************************/
RC CDWGToGeo::ExtractText(db_text * pText,
                          bool bInWCS,
						  PPath & pPath,
						  bool advance_pCurrent)
	{
	RC rc;
	double theta;
	theta = *(pText->retp_50());

    sds_point normal;
    pText->get_210(normal);

	// we are not using the following right now.
	// update the entdisp so that we don't get a crash!
	// sds_name ename;
	// ename[0] = (long) pText;
	// ename[1] = (long) SDS_CURDWG;
	// SDS_UpdateEntDisp(ename, 0);

    rc = ExtractBbox(pText, theta, (bInWCS) ? &normal : NULL, pPath);

	// we are allowing for the possibility that ExtractText may be
	// called when processing a dimension entity, in which case, we
	// would not want to advance m_pCurrent
	if (advance_pCurrent)
		m_pCurrent = pText->next;

	RETURN_RC(CDWGToGeo::ExtractText, rc);


	}
/*****************************************************************/
RC CDWGToGeo::ExtractMtext(CDbMText * pMtext,
                          bool bInWCS,
						  PPath & pPath,
						  bool advance_pCurrent)
	{
	RC rc;
	double theta;

	// for text we used retp_50, but that won't work for mtext.
	pMtext->get_50(&theta, SDS_CURDWG);
    sds_point normal;
    pMtext->get_210(normal);

	rc = ExtractBbox(pMtext, theta, (bInWCS) ? &normal : NULL, pPath);

	// we are allowing for the possibility that ExtractMtext may be
	// called when processing a dimension entity, in which case, we
	// would not want to advance m_pCurrent
	if (advance_pCurrent)
		m_pCurrent = pMtext->next;

	RETURN_RC(CDWGToGeo::ExtractMtext, rc);


	}
/*****************************************************************/
RC CDWGToGeo::ExtractDimension(db_dimension * pDimension,
                               bool bInWCS,
							   PPath & pPath)
	{
	RC rc = FAILURE;

	// find the associated text or mtext with the dimension and
	// call the appropriate extract operation

	db_text * pText;
	CDbMText * pMtext;
	bool advance_pCurrent = false;

	if (pDimension->get_texthandle(pText, pMtext))
		{
		if (pMtext != NULL)
			rc = ExtractMtext(pMtext, bInWCS, pPath, advance_pCurrent);
		else if (pText != NULL)
			rc = ExtractText(pText, bInWCS, pPath, advance_pCurrent);
		else
			rc = FAILURE;
		}

	// Get the pointer to the next entity
    m_pCurrent = pDimension->next;

	RETURN_RC(CDWGToGeo::ExtractDimension, rc);


	}
/*****************************************************************/
RC CDWGToGeo::ExtractTolerance(db_tolerance * pTolerance,
                               bool bInWCS,
							   PPath & pPath)
	{
	RC rc;

    double theta; 

	sds_point xdir;
	pTolerance->get_11(xdir);
	theta = atan2(xdir[1], xdir[0]);

    sds_point normal;
    pTolerance->get_210(normal);

	rc = ExtractBbox(pTolerance, theta, (bInWCS) ? &normal : NULL, pPath);


	// Get the pointer to the next entity
    m_pCurrent = pTolerance->next;

	RETURN_RC(CDWGToGeo::ExtractDimension, rc);


	}
								   
/*****************************************************************/
RC CDWGToGeo::ExtractLWPolyline(db_lwpolyline * pLWPolyline,
                                bool bInWCS,
								PPath & pPath)
	{
	RC rc;
	C3Point P, Q;
	
	int flags;
	bool closed;
	int i;
	
	int num_verts = pLWPolyline->ret_90();
	
	if (num_verts < 2)
		{
		pPath = NULL;
		rc = FAILURE;
		}
	else 
		{  // begin normal case
		
		// allocate arrays of sds_points and C3Points
		sds_point * sdspt = new sds_point[num_verts];
		C3Point * geopt = new C3Point[num_verts];
		PCurve pCurve = NULL;
		
		pLWPolyline->get_70(&flags);
		if (flags & 1)
			closed = true;
		else
			closed = false;

        sds_point normal;
        pLWPolyline->get_210(normal);
        C3Point ptNormal(normal[0], normal[1], normal[2]);

		pPath = new CPath();
		
		pLWPolyline->get_10(sdspt, num_verts);
		
        sds_point point;
		for (i = 0; i < num_verts; i++)
			{
            if (bInWCS)
                SdsEcsToWcs(sdspt[i], point,normal);
            else
                ic_ptcpy(point, sdspt[i]);

			SdsToRpt(point, geopt[i]);
            if (!bInWCS)
			    geopt[i].SetZ(0.0);
			}
		
		
		if (pLWPolyline->hasbulges())
			{  
			double * bulge = new double[num_verts];
			
			pLWPolyline->get_42array(bulge, num_verts);

			for (i = 0; i < num_verts-1; i++) {
                if (bInWCS) {
				    if (rc = ExtractPolyEdge(geopt[i], bulge[i], geopt[i+1], &ptNormal, pPath))
					    QUIT;
                }
                else {
                    if (rc = ExtractPolyEdge(geopt[i], bulge[i], geopt[i+1], NULL, pPath))
					    QUIT;
                }
			}

			if (closed) {
                if (bInWCS) {
				    if (rc = ExtractPolyEdge(geopt[num_verts-1], bulge[num_verts-1], geopt[0], &ptNormal, pPath))
					    QUIT;
                }
                else {
				if (rc = ExtractPolyEdge(geopt[num_verts-1], bulge[num_verts-1], geopt[0], NULL, pPath))
					QUIT;
                }
			}

			pPath->WrapUp();


			delete [] bulge;

			}
		else
			{  // begin case with no bulges

			
			for (i = 0; i < num_verts-1; i++)
				{
				if (rc = ALLOC_FAIL(pCurve = new CLine(geopt[i], geopt[i+1])))
					QUIT;
				pPath->Add(pCurve);
				}
			
			if (closed)
				{
				if (rc = ALLOC_FAIL(pCurve = new CLine(geopt[num_verts-1],geopt[0])))
					QUIT;
				pPath->Add(pCurve);
				
				}
			
			pPath->WrapUp();
			
			
			}   //  end case with no bulges
		
		delete [] sdspt;
		delete [] geopt;
		}   //  end normal case
	
	// Get the pointer to the next entity
	m_pCurrent = pLWPolyline->next;
	
exit:
	RETURN_RC(CDWGToGeo::ExtractLWPolyline, rc);
	}
/*****************************************************************/
RC CDWGToGeo::ExtractAttrib(db_attrib * pAttrib,
                            bool bInWCS,
							PPath & pPath)
	{

	// get the pointer to the next entity

	RC rc;
	int flags;
	pAttrib->get_70(&flags);

    sds_point normal;
    pAttrib->get_210(normal);

	if (flags & DB_INVISIBLE)
		{  // begin case of invisible attribute
            rc = FAILURE; // this for the benefit of the calling routine
			pPath = NULL;    
		}  //   end case of invisible attribute
	else
		{  // begin case of visible attribute
		double theta;
		pAttrib->get_50(&theta);

        rc = ExtractBbox(pAttrib, theta, (bInWCS) ? &normal : NULL, pPath);

		}  //   end case of visible attribute


	m_pCurrent = pAttrib->next;
	RETURN_RC(CDWGToGeo::ExtractAttrib, rc);
	}
/*****************************************************************/
// Create geo curves from a DWG entity
RC CDWGToGeo::ExtractEntity(
	db_handitem* pEntity,		// I: Entity's DB handle
    stdPPathList &pathList,	    // O: Pointer to the corresponding GEO paths
	bool bExtend,		        // I: Try to extend entity to biggest possible size 
    bool bInWCS)
	{
	db_handitem* p = pEntity;
	RC rc = FAILURE; // better to initialize to FAILURE to prevent downstream crashes.
	PPath pPath;

	bool advance_pCurrent = true;

	switch (pEntity->ret_type())
		{
		case DB_POLYLINE:
			{
				if (rc = ExtractPolyline((db_polyline*)pEntity, bInWCS, pPath))
					QUIT
				
				if(bExtend)
				{
					sds_point sdsStart, sdsEnd;
					sds_point sdsDir;
					sds_point sdsNewStart, sdsNewEnd;
					sds_point n;
					pEntity->get_210(n);

					C3Point pathStart, pathEnd;

					C3Point start, end;
					pPath->Curve(0)->GetStartPoint(start);
					pPath->Curve(0)->GetEndPoint(end);
					RptToSds(start, sdsStart);
					RptToSds(end, sdsEnd);
                    if (!bInWCS) {
					    SdsEcsToWcs(sdsStart, sdsStart, n);
					    SdsEcsToWcs(sdsEnd, sdsEnd, n);
                    }

					sdsDir[0] = - sdsEnd[0] + sdsStart[0];
					sdsDir[1] = - sdsEnd[1] + sdsStart[1];
					sdsDir[2] = - sdsEnd[2] + sdsStart[2];
					gr_rayxlinegenpts(sdsEnd,
						sdsDir,
						DB_RAY,
						sdsNewStart,
						sdsNewEnd,
						SDS_CURVIEW->m_pCurDwgView,
						SDS_CURDWG);

                    if (!bInWCS) 
					    SdsWcsToEcs(sdsNewEnd, sdsNewEnd, n);
					SdsToRpt(sdsNewEnd, pathStart);

					pPath->Curve(pPath->Size() - 1)->GetStartPoint(start);
					pPath->Curve(pPath->Size() - 1)->GetEndPoint(end);
					RptToSds(start, sdsStart);
					RptToSds(end, sdsEnd);
                    if (!bInWCS) {
					    SdsEcsToWcs(sdsStart, sdsStart, n);
					    SdsEcsToWcs(sdsEnd, sdsEnd, n);
                    }

					sdsDir[0] = sdsEnd[0] - sdsStart[0];
					sdsDir[1] = sdsEnd[1] - sdsStart[1];
					sdsDir[2] = sdsEnd[2] - sdsStart[2];
					gr_rayxlinegenpts(sdsEnd,
						sdsDir,
						DB_RAY,
						sdsNewStart,
						sdsNewEnd,
						SDS_CURVIEW->m_pCurDwgView,
						SDS_CURDWG);

                    if (!bInWCS) 
					    SdsWcsToEcs(sdsNewEnd, sdsNewEnd, n);
					SdsToRpt(sdsNewEnd, pathEnd);

					pPath->SnapEnds(pathStart, pathEnd);
				}
				break;
			}

		case DB_LINE:
			{
				if(bExtend)
				{
					sds_point start, end, direction;
					pEntity->set_type(DB_XLINE);
					pEntity->get_10(start);
					pEntity->get_11(end);
					direction[0] = end[0] - start[0];
					direction[1] = end[1] - start[1];
					direction[2] = end[2] - start[2];
					pEntity->set_11(direction);

					if (rc = ExtractRayOrXline((db_entity*)pEntity, bInWCS, pPath))
						QUIT

					pEntity->set_type(DB_LINE);
					pEntity->set_11(end);
				}
				else
				{
					if (rc = ExtractLine((db_line*)pEntity, bInWCS, pPath))
						QUIT
				}
				break;
			}

		case DB_ARC:
			{
				if (rc = ExtractArc((db_arc*)pEntity, bInWCS, pPath))
					QUIT

				if(bExtend)
				{
					ASSERT(pPath->Size() == 1);
					(CEllipArc*)pPath->Curve(0)->ResetEnd(((CEllipArc*)pPath->Curve(0))->Start() + TWOPI);
				}
				break;
			}
		
		case DB_CIRCLE:
			if (rc = ExtractCircle((db_circle*)pEntity, bInWCS, pPath))
				QUIT
			break;

		case DB_ELLIPSE:
			{
				if (rc = ExtractEllipse((db_ellipse*)pEntity, bInWCS, pPath))
					QUIT

				if(bExtend)
				{
					ASSERT(pPath->Size() == 1);
					(CEllipArc*)pPath->Curve(0)->ResetEnd(((CEllipArc*)pPath->Curve(0))->Start() + TWOPI);
				}
				break;
			}

		case DB_RAY:	//fall thru
		case DB_XLINE:
			{
				int realType = pEntity->ret_type();
				if(bExtend && realType == DB_RAY)
					pEntity->set_type(DB_XLINE);

				if (rc = ExtractRayOrXline((db_entity*)pEntity, bInWCS, pPath))
					QUIT

				if(bExtend && realType == DB_RAY)
					pEntity->set_type(DB_RAY);

				break;
			}

		case DB_3DFACE:
			if (rc = Extract3dface((db_3dface*)pEntity, bInWCS, pPath))
				QUIT
			break;

		case DB_IMAGE:
			if (rc = ExtractImage((db_image*)pEntity, bInWCS, pPath))
				QUIT
			break;

		case DB_SPLINE:
			if (rc = ExtractSpline((db_spline*)pEntity, pPath))
				QUIT
			break;

		case DB_MLINE:
			if (rc = ExtractMline((db_mline*)pEntity, bInWCS, pathList))
				QUIT
				RETURN_RC(CDWGToGeo::ExtractEntity, rc);
			break;

		case DB_TEXT:
			if (rc = ExtractText((db_text*)pEntity, bInWCS, pPath, advance_pCurrent))
				QUIT
			break;

		case DB_MTEXT:
			if (rc = ExtractMtext((CDbMText*)pEntity, bInWCS, pPath, advance_pCurrent))
				QUIT
			break;

		case DB_DIMENSION:
			if (rc = ExtractDimension((db_dimension*)pEntity, bInWCS, pPath))
				QUIT
			break;

		case DB_LEADER:
			// the mtext associated with a leader should already be handled
			// by the DB_MTEXT case above.  Also, as of now (4/15/99) leaders
			// are not fully implemented in IntelliCAD.  When they are, we may
			// need to revisit this case and implement something here.
			break;

		case DB_TOLERANCE:
			if (rc = ExtractTolerance((db_tolerance*)pEntity, bInWCS, pPath))
				QUIT			
			break;

		case DB_LWPOLYLINE:
			{
				if (rc = ExtractLWPolyline((db_lwpolyline*)pEntity, bInWCS, pPath))
					QUIT

				if(bExtend)
				{
					sds_point sdsStart, sdsEnd;
					sds_point sdsDir;
					sds_point sdsNewStart, sdsNewEnd;
					sds_point n;
					pEntity->get_210(n);

					C3Point pathStart, pathEnd;

					C3Point start, end;
					pPath->Curve(0)->GetStartPoint(start);
					pPath->Curve(0)->GetEndPoint(end);
					RptToSds(start, sdsStart);
					RptToSds(end, sdsEnd);
                    if (!bInWCS) {
					    SdsEcsToWcs(sdsStart, sdsStart, n);
					    SdsEcsToWcs(sdsEnd, sdsEnd, n);
                    }

					sdsDir[0] = - sdsEnd[0] + sdsStart[0];
					sdsDir[1] = - sdsEnd[1] + sdsStart[1];
					sdsDir[2] = - sdsEnd[2] + sdsStart[2];
					gr_rayxlinegenpts(sdsEnd,
						sdsDir,
						DB_RAY,
						sdsNewStart,
						sdsNewEnd,
						SDS_CURVIEW->m_pCurDwgView,
						SDS_CURDWG);

                    if (!bInWCS)
					    SdsWcsToEcs(sdsNewEnd, sdsNewEnd, n);
					SdsToRpt(sdsNewEnd, pathStart);

					pPath->Curve(pPath->Size() - 1)->GetStartPoint(start);
					pPath->Curve(pPath->Size() - 1)->GetEndPoint(end);
					RptToSds(start, sdsStart);
					RptToSds(end, sdsEnd);
                    if (!bInWCS) {
					    SdsEcsToWcs(sdsStart, sdsStart, n);
					    SdsEcsToWcs(sdsEnd, sdsEnd, n);
                    }

					sdsDir[0] = sdsEnd[0] - sdsStart[0];
					sdsDir[1] = sdsEnd[1] - sdsStart[1];
					sdsDir[2] = sdsEnd[2] - sdsStart[2];
					gr_rayxlinegenpts(sdsStart,
						sdsDir,
						DB_RAY,
						sdsNewStart,
						sdsNewEnd,
						SDS_CURVIEW->m_pCurDwgView,
						SDS_CURDWG);
                    if (!bInWCS) 
					    SdsWcsToEcs(sdsNewEnd, sdsNewEnd, n);
					SdsToRpt(sdsNewEnd, pathEnd);

					pPath->SnapEnds(pathStart, pathEnd);
				}
				break;
			}

		case DB_ATTRIB:
			if (rc = ExtractAttrib((db_attrib*)pEntity, bInWCS, pPath))
				QUIT
			break;

		}

	if (SUCCESS == rc)
		pathList.push_back(pPath);

exit:

	RETURN_RC(CDWGToGeo::ExtractEntity, rc);
	}
//////////////////////////////////////////////////////////////////
//					Implementation of CGeoToDWG					//
//

// Utility functions for CGeoToDWG
// 
static bool IsCurvePolylineComponent(PCurve pCurve)
{
	bool curveIsPolylineComponent;

	// a curve is a component of a polyline if it is a straight line
	// or a circular arc.

	double radius;

    switch (pCurve->Type())
	{
	case TYPE_Line:
	    curveIsPolylineComponent = true;	
		break;
	case TYPE_UnboundedLine:
	case TYPE_Ray:
		if (pCurve->Bounded())
			curveIsPolylineComponent = true;
		else
			curveIsPolylineComponent = false;
		break;
	case TYPE_Elliptical:
		if (((CEllipArc *)pCurve)->IsCircular(radius))
			curveIsPolylineComponent = true;
		else
			curveIsPolylineComponent = false;
		break;
	default:
		curveIsPolylineComponent = false;
		break;
	}

	return curveIsPolylineComponent;
}


static bool IsPathPolyline( // true if curves in path are all straight lines or circular arcs
							PPath pPath) // In:  The path to check.  For generality allow any
							             //      PPath, not just PPath, as input. 
{
	bool pathIsPolyline = true; // innocent until proven guilty
	bool curveIsPolylineComponent;

	for (int i = 0; i < pPath->Size(); i++)
	{  // begin checking i-th curve in pPath
		curveIsPolylineComponent = IsCurvePolylineComponent(pPath->Curve(i));
		if (curveIsPolylineComponent == false)
		{
			pathIsPolyline = false;
			break;
		}

	}  //   end checking i-th curve in pPath

	return pathIsPolyline;
							
}



// Populate a polyline 
RC CGeoToDWG::PolylineFromPath(
	PPath pPath,		  // In: The path to persist
    HanditemCollection *& pPolyCollection) // Out: Polyline is added to HanditemCollection  must not be NULL
	{
	RC rc = SUCCESS;
	db_handitem * prev; // for use in creating the linked list

	// If we are not going to approximate (this capability may be added later) and if the
	// input path is not a polyline, return immediately

    bool AllowApproximation = false; // for now, don't allow approximation
	                                 // later this may be an input parameter.

	if (AllowApproximation== false)
	{
		if (IsPathPolyline(pPath) == false)
		{
			return rc = E_NotAPolyline;
		}
	}

	bool destroyHI = true; // the intent is to destroy the items when destroying
	                       // the collection.


	// create the polyline itself, passing the drawing to the constructor
	db_polyline * pPolyline = new db_polyline(m_pDrawing);

	// set other attributes of this polyline as needed

	if (pPath->Closed())
	{
		pPolyline->set_70(IC_PLINE_CLOSED); 
	}

	//Set extrusion vector
	sds_point pNormal;
	pNormal[0]=m_ptNormal.X(); pNormal[1]=m_ptNormal.Y(); pNormal[2]=m_ptNormal.Z();
	pPolyline->set_210(pNormal);

	//Set elevation
	sds_resbuf rbElev;
	double elevation=0.0;
	if(RTNORM == SDS_getvar(NULL,DB_QELEVATION,&rbElev,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))
		{
		sds_point pElev;
		pElev[0] = pElev[1] = 0; pElev[2] = rbElev.resval.rreal;
		SdsUcsToEcs(pElev, pElev, pNormal);
		pElev[0] = pElev[1] = 0; //X and Y are just dummy values and should be set to 0.
		pPolyline->set_10(pElev);
		elevation = pElev[2];	 //Save the elevation for vertices.
		}

	// set the thickness to the default value of 0
	int default_thickness=0;
	pPolyline->set_39(default_thickness);

	// make the polyline be the first node of the HanditemCollection
	pPolyCollection->AddItem(m_pDrawing, pPolyline, destroyHI);
	prev = pPolyline;

	// add nodes for the vertices

	int nCurves = pPath->Size();

	// The following local variables will be used by GetPolylineEdges
	C3Point geo_location_point[2];
	double bulge[2];
	int nEdges;
	int i,j; // loop indices
	sds_point sds_location_point[2];

	for (i = 0; i < nCurves; i++)
	{   // begin getting vertices for i-th curve

		rc = pPath->Curve(i)->GetPolylineEdges(geo_location_point, bulge, nEdges);
	
        if (rc != SUCCESS)
		{   // begin error processing
			break; // get out of this loop, something is wrong.
		}   //   end error processing
		else
		{   // begin normal processing
			for (j = 0; j < nEdges; j++)
			{  // begin processing j-th vertex of i-th curve
				db_vertex * pVertex = new db_vertex(m_pDrawing);
				RptToSds(geo_location_point[j], sds_location_point[j]);
				sds_location_point[j][2] = elevation;
				pVertex->set_10(sds_location_point[j]);	
				pVertex->set_42(bulge[j]);
				pPolyCollection->AddItem(m_pDrawing, pVertex, destroyHI);
				prev->next = pVertex;
				prev = prev->next;
			}  //   end processing j-th vertex of i-th curve
		}   //   end normal processing

	}   //   end getting vertices for i-th curve

	// add a node for the final vertex, the one not picked up using
	// GetPolylineEdges.  This needs to be done if and only if the 
	// path is NOT closed

	if (!(pPath->Closed()) && (rc == SUCCESS))
	{  // begin adding final vertex
		C3Point geoEndPoint;
		sds_point sdsEndPoint;
		rc = pPath->Curve(nCurves - 1)->GetEndPoint(geoEndPoint);

		if (rc == SUCCESS)
		{
			RptToSds(geoEndPoint, sdsEndPoint);
			db_vertex * pEndVertex = new db_vertex(m_pDrawing);
			sdsEndPoint[2] = elevation;
			pEndVertex->set_10(sdsEndPoint);

			// the last bulge factor should just be a formality here, since
			// there is no next segment, so let's set it to 0.0, which is the default anyway.
			pEndVertex->set_42(0.0);
			
			pPolyCollection->AddItem(m_pDrawing, pEndVertex, destroyHI);
			prev->next = pEndVertex;
			prev = prev->next;
		}
		
	}  //   end adding final vertex

	// create the seqend
	db_seqend * pSeqend = new db_seqend(m_pDrawing);

	// make the seqend be the last node of the HanditemCollection
	pPolyCollection->AddItem(m_pDrawing, pSeqend, destroyHI);
	prev->next = pSeqend;
    pSeqend->next = NULL;


	RETURN_RC(CGeoToDWG::PolylineFromPath, rc);
	}
/*****************************************************************/
// Populate a lightweight polyline 

RC CGeoToDWG::LWPolylineFromPath(
	PPath pPath,		  // In: The boundary to persist
	HanditemCollection * & pLWPolyCollection) // In/Out: Lightweight polyline is added to HanditemCollection Must not be NULL
	{
	RC rc = SUCCESS;

	// If we are not going to approximate (this capability may be added later) and if the
	// input path is not a polyline, return immediately

    bool AllowApproximation = false; // for now, don't allow approximation
	                                 // later this may be an input parameter.

	if (AllowApproximation== false)
	{
		if (IsPathPolyline(pPath) == false)
		{
			return rc = E_NotAPolyline;
		}
	}

	int i, j, n, nCurves, nEdges;
    sds_point* v = NULL;  
	double* b = NULL;
	C3Point P[2];

	int LastIndex;
	C3Point LastPoint;
	double LastBulge;

    bool destroyHI = true; // the intent is to destroy the items when destroying
	                       // the collection

	bool hasBulges = false;
	int nVerts = 0;
     
	db_lwpolyline * pLWPoly = new db_lwpolyline(m_pDrawing);


	//TO-DO: Set the elevation(in UCS) correctly, DXF code, 38
	//Also, set z coord. of DXF 10 to 0.

	// Allocate buffers for vertices and bulges
	if ((nCurves = pPath->Size()) < 1)
		goto exit;
	n = nCurves * 2;	// There can be at most 2 edges per curve
	if (ALLOC_FAIL(v = new sds_point[n]))
		QUIT
	if (ALLOC_FAIL(b = new double[n]))
		QUIT

	// Load the vertices and bulges
	nEdges = 0;
	for (i = 0;   !rc  &&  i < nCurves;   i++)
		{
		if (rc = pPath->Curve(i)->GetPolylineEdges(P, &b[nEdges], n))
			QUIT
			// n may be 1 or 2
		for (j = 0;   j < n;   j++)
			RptToSds(P[j], v[nEdges + j]);
		nEdges += n;
		}

	nVerts = nEdges;

	// Last vertex (only needed if path is not closed!)
	if (!(pPath->Closed()))
		{
		LastIndex = nEdges;
		// there is going to be an additional vertex
		nVerts = nEdges + 1;
		LastBulge = 0.0;
		if (rc = pPath->Curve(nCurves-1)->GetEndPoint(LastPoint))
			QUIT
			RptToSds(LastPoint, v[LastIndex]);
		b[LastIndex] = LastBulge;
		}



	// populate the entity
	pLWPoly->set_10(v, nVerts);

	// Only set the bulges in the lightweight polyline if at least
	// one of them is non-zero.  This will save some storage in the
	// case where all pieces of the lightweight polyline are linear.
	
	for (i = 0; i < nVerts; i++)
		{
		if (!icadRealEqual(b[i],0.0))
			{
			hasBulges = true;
			break;
			}
		}

	if (hasBulges)
		{
		pLWPoly->set_42(b, nVerts);
		}


	//Set closed flag (Note: we are doing this after the set_10 because the set_10
	//zeroes all but the linegen component of flags.
	if (pPath->Closed())
		{
		pLWPoly->set_70(IC_PLINE_CLOSED);
		}

	//Set extrusion vector
	sds_point pNormal;
	pNormal[0]=m_ptNormal.X(); pNormal[1]=m_ptNormal.Y(); pNormal[2]=m_ptNormal.Z();
	pLWPoly->set_210(pNormal);


	pLWPolyCollection->AddItem(m_pDrawing, pLWPoly, destroyHI);



exit:
	delete [] b;
	delete [] v;
	RETURN_RC(CGeoToDWG::LWPolylineFromPath, rc);
	}


/*****************************************************************/
// Note:  The arrays v and b must be allocated and deleted by the calling routine.
RC CGeoToDWG::PolylineDataFromPath(PPath pPath, // In:  The path from which to get polyline data
								   sds_point * v, // Out:  array of vertices
								   double * b, // Out:  array of bulges
								   int & closed, // Out: 1 iff closed, 0 otherwise
								   int & hasBulges, // Out:  1 iff circ arcs present, 0 otherwise
								   int & nVerts) // Out:  number of vertices
	{
	RC rc = SUCCESS;
	int i;
	int nEdges = 0;
	
	if (!IsPathPolyline(pPath))
		{
		rc = E_NotAPolyline;
		}
	else
		{  // begin polyline case
		
		if (pPath->Closed())
			closed = 1;
		else
			closed = 0;

		int nCurves = pPath->Size();
		int nEdges = 0;
		int n;
		C3Point GeoPt[2];
		for (i = 0; i < nCurves  && !rc; i++)
			{  // begin i-loop
			rc = pPath->Curve(i)->GetPolylineEdges(GeoPt, &b[nEdges], n);
			if (!rc)
				{  // begin updating information
				for (int j = 0; j < n; j++)
					{  // begin j-loop
					RptToSds(GeoPt[j], v[nEdges + j]);
					}  //   end j-loop
				nEdges += n;
				}  //   end updating information

			}  //   end i-loop

		nVerts = nEdges;

		// Last vertex (only needed if path is not closed)
		if (!pPath->Closed())
			{  // begin non-closed case
			int LastIndex = nEdges;
			// there is going to be an additional vertex, so...
			nVerts = nEdges + 1;
			C3Point LastPoint;
			double LastBulge = 0.0;
			rc = pPath->Curve(nCurves-1)->GetEndPoint(LastPoint);
			if (!rc)
				{
				RptToSds(LastPoint, v[LastIndex]);
				b[LastIndex] = LastBulge;
				}
			}  //   end non-closed case

		hasBulges = 0;
		for (i = 0; i < nVerts; i++)
			{
			if (!icadRealEqual(b[i],0.0))
				{
				hasBulges = 1;
				break;
				}
			}

		}  //  end polyline case
	
	RETURN_RC(CGeoToDWG::PolylineDataFromPath, rc);
	}

#ifdef SplitArcs
/*****************************************************************/
// The quantity (b-a) is added to or subtracted from x until x is in the interval [a,b].
// It is assumed, without checking that a < b.
static void map_into_range(double & x, // In & Out: the value to be mapped into the range
						   double a,   // In: the lower end of the range
						   double b)   // In: the upper end of the range
	{
	
	double c = b - a;
	
	while (x < a)
		x += c;
	
	while (x > b)
		x -= c;

	}
/*****************************************************************/
static void snap_to(double & x, // In & Out: the value to be adjusted
					double snapval, // In: the value to snap to
					double tol) // In: not a double tol latte, the tolerance
	{
	if (fabs(x - snapval) < tol)
		x = snapval;
	}
/*****************************************************************/
// Note:  This is not presently being used, but it is available in case
// we need it.

// The conventions used are as follows.  They are subject to change
// If the input is an elliptic arc that is circular, we will break
// up the input arc if it crosses 0 or PI (modulo TWOPI).
// If the input arc is not circular, we will not break it up.
RC CGeoToDWG::BhatchArcParmsFromEllipArc(
										 PEllipArc pEllipArc, // In: The arc to analyze
										 int & nEdges, // Out:  edges in corresp DWG entity
										 double start[3], // Out: the array of (up to 3) start parms
										 double end[3])   // Out: the array of (up to 3) end parms
				{
				RC rc = SUCCESS;
				double radius; // returned by IsCircular function call but not used.

				// we will use these because they are a bit more accurate than the global
				// constants PI and TWOPI now in Geo, which we eventually may change.
				double pi = 4*atan(1.0);
				double twopi = 2*pi;
				
				if (pEllipArc->IsCircular(radius))
					{   // begin circular case
					
					if (pEllipArc->Closed())
						{  // begin closed case
						nEdges = 2;
						start[0] = 0.0;
						end[0] = pi;
						start[1] = pi;
						end[1] = twopi;
						}  //   end closed case
					else
						{  // begin non-closed case
						
						double s = pEllipArc->Start();
						double t = pEllipArc->End();
						// reject this if Start >= End
						if (s >= t)
							rc = FAILURE;
						else
							{  // begin good circular arc case 
							// first adjust s and t so that they are both between 0 and TWOPI
							
							
							map_into_range(s, 0.0, twopi);
							map_into_range(t, 0.0, twopi);

							snap_to(s, 0.0, FUZZ_DIST);
							snap_to(s, pi, FUZZ_DIST);
							snap_to(s, twopi, FUZZ_DIST);

							snap_to(t, 0.0, FUZZ_DIST);
							snap_to(t, pi, FUZZ_DIST);
							snap_to(t, twopi, FUZZ_DIST);
							
							// now of course it may happen that s > t
							if (s < t)
								{  // begin case where s < t
								if ((s < pi) && (pi < t))
									{
									nEdges = 2;
									start[0] = s;
									end[0] = pi;
									start[1] = pi;
									end[1] = t;
									}
								else
									{
									nEdges = 1;
									start[0] = s;
									end[0] = t;
									}
								}  //   end case where s < t
							else
								{  // begin case where s > t
								if (s < pi)
									{   // begin case where s < pi
									nEdges = 3;
									start[0] = s;
									end[0] = pi;
									start[1] = pi;
									end[1] = twopi;
									start[2] = 0.0;
									end[2] = t;
									}   //   end case where s < pi
								else
									{   // begin case where s >= pi
									nEdges = 2;
									start[0] = s;
									end[0] = twopi;
									start[1] = 0.0;
									end[1] = t;
									}   //   end case where s >= pi
								}  //   end case where s > t
							
							}  //   end good circular arc case
						}   // end non-closed case 
					
					}   // end circular case
				else
					{
					nEdges = 1;
					}
				RETURN_RC(CGeoToDWG::BhatchArcParmsFromEllipArc, rc);
				}
/*****************************************************************/
#endif // SplitArcs

// Populate a BHATCH

	RC CGeoToDWG::BhatchFromPath(
		PPath pPath,			// In:  The path to persist
		int i,              // In:  The index of this path in the Bhatch
		int pathFlag,           // In and Modified internally: the boundary path type flag
		stdObjHandleList * pAssociates, // In: The list of handles for associated objects
		db_hatch *& pHatch)	// In/Out: The Bhatch in which the path participates
		{
		RC rc = SUCCESS;
		int nedges;

		ASSERT(pHatch != NULL);
        ASSERT(0 <= i);

#ifdef _DEBUG
		int num_paths;
		pHatch->get_91(&num_paths);
		ASSERT(i < num_paths);
#endif // _DEBUG

		if (IsPathPolyline(pPath))
			{  // begin polyline case
			// if it is a polyline there is only one segment

			pathFlag += 2; 
			nedges = 1; // that is the convention for pline
			pHatch->set_path(i, pathFlag, nedges);

			// Determine the values of the following three parameters, which are going
			// to be passed in to set_pathedgepline
			int closed;
			int hasBulges;
			int nVerts;

			int nCurves = pPath->Size();
			int n = nCurves*2; // there can be at most 2 edges per curve
			sds_point * v = new sds_point[n];
			double * b = new double[n];

			rc = PolylineDataFromPath(pPath, v, b, closed, hasBulges, nVerts);

			if (!rc)
				{  // begin setting data for polyline
				
				pHatch->set_pathedgepline(i, closed, hasBulges, nVerts);

				for (int j = 0; j < nVerts; j++)
					pHatch->set_pathedgeplinevert(i,j,v[j][0],v[j][1],b[j]);
				
				}  //   end setting data for polyline
			
			delete [] v;
			delete [] b;

			}  //   end polyline case
		else
			{  // begin non-polyline case
			nedges = pPath->Size();
			pHatch->set_path(i, pathFlag, nedges);
			C3Point P;
			C3Point Q;
			double radius;
			C3Point Center;
			double startangle;
			double endangle;
			short isccw;

			C3Point MajorAxis;
			C3Point MinorAxis;
			C3Point Normal;

			PCurve pCurve;
			PEllipArc pEllipArc;
			PNURBS pNURBS;
			for (int j = 0; (j < nedges) && !rc; j++)
				{   // begin j-loop
				pCurve = pPath->Curve(j);
				switch (pCurve->Type())
					{  // begin switch pCurve->Type()
					case TYPE_Line:
					case TYPE_UnboundedLine:
					case TYPE_Ray:
						pCurve->GetStartPoint(P);
						pCurve->GetEndPoint(Q);
						pHatch->set_pathedgeline(i,j,P.X(),P.Y(),Q.X(),Q.Y());
						break;
					case TYPE_Arc: // this case has been retired and should not occur
						rc = FAILURE;
						break;
					case TYPE_Elliptical:
						// this covers the circular and non-circular cases

						// Note:  In the present implementation we are assuming that
						// we do not have to break up the circular arc into several
						// smaller pieces, as AutoCAD does on hatch creation cases
						// that have arcs as part of the boundary.  However, we have
						// some code available above (BhatchArcParmsFromEllipArc) that
						// will help us do that in case we need it.  It appears that
						// AutoCAD can read in a hand-edited DXF file in which the
						// multi-segment arcs were made into a one-segment arc.

						pEllipArc = (CEllipArc *)pCurve;
						Center = pEllipArc->Center();
						startangle = pEllipArc->Start();
						endangle = pEllipArc->End();

						MajorAxis = pEllipArc->MajorAxis();
						MinorAxis = pEllipArc->MinorAxis();
						Normal = MajorAxis ^ MinorAxis;

						if ( Normal.Z() > 0.0)
							isccw = 1;
						else
							isccw = 0;

						if (pEllipArc->IsCircular(radius))
							{  // begin circular case
							
							pHatch->set_pathedgearc(i,j, 
								Center.X(), Center.Y(),
								radius, 
								startangle, endangle, 
								isccw);
							}  //   end circular case
						else
							{  // begin elliptical case
							C3Point MajorAxisVec = pEllipArc->MajorAxis();
							C3Point MinorAxisVec = pEllipArc->MinorAxis();
							double MajorLen = MajorAxisVec.Norm();
							double MinorLen = MinorAxisVec.Norm();
							if ((MajorLen > FUZZ_DIST) && (MinorLen > FUZZ_DIST))
								{  // begin good elliptical case
								double minmajratio = MinorLen/MajorLen;
								pHatch->set_pathedgeellarc(i,j,
									Center.X(), Center.Y(),
									MajorAxisVec.X(), MajorAxisVec.Y(),
									minmajratio,
									startangle, endangle,
									isccw);
								}  //   end good elliptical case
							else
								{  // begin bad elliptical case
								rc = FAILURE;
								}  //   end bad elliptical case

							}  //   end elliptical case
						break;
					case TYPE_NURBS:

						// assign the basic information for the spline
						long degree, numknots, numctrlpts;
						short rational, periodic;
						pNURBS = (CNURBS *)pCurve;
						degree = pNURBS->Degree();
						numctrlpts = pNURBS->PointCount();
						numknots = numctrlpts + degree + 1;
						if (pNURBS->Rational())
							rational = 1;
						else
							rational = 0;
						periodic = 0;
						pHatch->set_pathedgespline(i,j,
							rational,periodic,
							degree,numknots,numctrlpts);

						long k;

						// assign the knots
						double knot;

						for (k = 0; k < numknots; k++)
							{
							knot = pNURBS->Knot(k);
							pHatch->set_pathedgesplineknot(i, j, k, knot);
							}

						// assign the control points
						for (k = 0; k < numctrlpts; k++)
							{
							// note we are calling ControlPointGeom since it works in
							// both the rational and polynomial cases.
							// In the rational case, a call to pNURBS->ControlPoint
							// would return the homogeneous form, which is not what
							// we want here.
							P = pNURBS->ControlPointGeom(k);
							pHatch->set_pathedgesplinecontrol(i, j, k, P.X(), P.Y());
							}

						// assign the weights, if necessary
						if (rational)
							{
							double weight;

							for (k = 0; k < numctrlpts; k++)
								{
								weight = pNURBS->Weight(k);
								pHatch->set_pathedgesplineweight(i, j, k, weight);
								}
							}
						break;
					case TYPE_PolyLine: // we are not presently handling this type
						rc = FAILURE;
						break;
					case TYPE_Gap: // we are not presently handling this type
						rc = FAILURE;
						break;
					case TYPE_Unknown: 
						rc = FAILURE;
						break;
					default:
						rc = FAILURE;
						break;
					}  //   end switch pCurve->Type()
				}   //   end j-loop
			
			}  //   end non-polyline case

			int assoc;
			pHatch->get_71(&assoc);

			if (pAssociates && assoc)
				{  // begin assigning the source boundary objects
				long numboundaryobjects = pAssociates->size();

				// If there are old objects then we must be doing a modify.  In this case
				// the boundary objects will not change so we do not want to update them.
				// Updating the boundarys will change the order in the boundary list which
				// causes problems when an entity such as a line has more than one reactor.
				db_hatchboundobjslink *oldobjs;
				pHatch->get_pathboundobj(i, &oldobjs);

				if (numboundaryobjects > 0 && !oldobjs)
					{
					db_hatchboundobjslink * objectsLink;
					objectsLink = new db_hatchboundobjslink(SDS_CURDWG);
					
					stdObjHandleList::const_iterator pHandle;
					
					for (pHandle = pAssociates->begin();
					pHandle != pAssociates->end();
					pHandle++)
						{
						db_objhandle Handle = *pHandle;
						db_handitem * hip = SDS_CURDWG->handent(Handle);
						objectsLink->addBoundaryObj(hip);
						}

					pHatch->set_pathBoundaryAndAssociate(i, numboundaryobjects, objectsLink, FALSE);
					
					
					}
				}  //   end assigning the source boundary objects


		RETURN_RC(CGeoToDWG::BhatchFromPath, rc);
		}



