// This file implements the functions to convert database enities to corresponding
// geometry library entities.
//
// Author: Stephen W. Hou
// Date: October 10, 2003

#include "EntConversion.h"
#include "icadlib.h"
#include "db.h"
#include "handitem.h"
#include "entity.h"
#include "entities.h"
#include "constnts.h"
#include "line.h"
#include "elliparc.h"
#include "nurbs.h"
#include "trnsform.h"
#include "compcrvs.h"
#include "path.h"

CLine* asGeoLine(db_line* pDbLine)
{
	if (pDbLine != NULL) {
		sds_point start, end;
		pDbLine->get_10(start);
		pDbLine->get_11(end);

		// Construct the line
		return new CLine(C3Point(start[0], start[1], start[2]),
						 C3Point(end[0], end[1], end[2]));
	}
	else
		return NULL;
}


CRay* asGeoRay(db_ray* pDbRay)
{
	if (pDbRay != NULL) {
		sds_point pt, dir;
		pDbRay->get_10(pt);
		pDbRay->get_11(dir);

		return new CRay(C3Point(pt[0], pt[1], pt[2]), C3Point(dir[0], dir[1], dir[2]));
	}
	else
		return NULL;
}


CUnboundedLine* asGeoUnboundedLine(db_xline* pDbXline)
{
	if (pDbXline != NULL) {
		sds_point pt, dir;
		pDbXline->get_10(pt);
		pDbXline->get_11(dir);

		return new CUnboundedLine(C3Point(pt[0], pt[1], pt[2]), C3Point(dir[0], dir[1], dir[2]));
	}
	else
		return NULL;
}


CEllipse* asGeoEllipse(db_circle* pDbCircle)
{
	if (pDbCircle != NULL) {
        sds_point norm;
        pDbCircle->get_210(norm);

		sds_point cen;
		pDbCircle->get_10(cen);

		double radius;
		pDbCircle->get_40(&radius);

        C3Point center(cen[0], cen[1], cen[2]);
        C3Vector normal(norm[0], norm[1], norm[2]);

        // transform from ECS to WCS
        CAffine tr(normal);
        tr.ApplyToPoint(center, center);

        C3Point major(radius, 0.0, 0.0);
        tr.ApplyToVector(major, major);
        C3Vector majorAxis(major);

//        C3Point minor(0.0, radius, 0.0);
        C3Vector minorAxis(normal ^ majorAxis);
//        tr.ApplyToVector(minor, minor);

		CEllipse* pEllipse = new CEllipse(center, majorAxis, minorAxis);
//        RC rc;
//		CEllipse* pEllipse = new CEllipse(center, radius, normal.AsPoint(), rc);
	    return pEllipse;
	}
	else
		return NULL;
}


CEllipArc* asGeoEllipArc(db_arc* pDbArc)
{
	if (pDbArc != NULL) {
		C3Point ptCenter;
		double radius, start, end;

		// Get the data
		sds_point cen, norm;
        pDbArc->get_210(norm);

		pDbArc->get_10(cen);

		pDbArc->get_40(&radius);
		pDbArc->get_50(&start);
		pDbArc->get_51(&end);

		// Make sure end > start
		while (end <= start)
			end += TWOPI;

		// Construct the arc
		// Note:  the arc that is constructed will be in a plane of
		//        constant z; the z-value taken from ptCenter.
		double sweep = end - start;

        C3Point center(cen[0], cen[1], cen[2]);
        C3Vector normal(norm[0], norm[1], norm[2]);

        // transform from ECS to WCS
        CAffine tr(normal);
        tr.ApplyToPoint(center, center);

		RC rc = SUCCESS;
		CEllipArc* pEllipArc = new CEllipArc(radius, center, normal, start, sweep, rc);
		if (rc != SUCCESS) {
			delete pEllipArc;
			return NULL;
		}
		else
			return pEllipArc;
	}
	else
		return NULL;
}


CEllipArc* asGeoEllipArc(db_ellipse* pDbEllipse)
{
	if (pDbEllipse != NULL) {

		sds_point norm;
		pDbEllipse->get_210(norm);

		sds_point cen;
		pDbEllipse->get_10(cen);

		sds_point majAxis;
		pDbEllipse->get_11(majAxis);

		double start, end, minMajRatio;
		pDbEllipse->get_40(&minMajRatio);
		pDbEllipse->get_41(&start);
		pDbEllipse->get_42(&end);

		// Make sure end > start
		//
		while (end <= start)
			end += TWOPI;

		C3Point center(cen[0], cen[1], cen[2]);
		C3Point major(majAxis[0], majAxis[1], majAxis[2]);
		C3Point normal(norm[0], norm[1], norm[2]);
		normal.Unitize();
		C3Point minor = normal ^ major;
		minor.Unitize();
		double scale =  major.Norm() * minMajRatio;
		minor = minor * scale;		// Scale it properly

		return new CEllipArc(center, major, minor, start, end);
	}
	else
		return NULL;
}


CNURBS* asGeoSpline(db_spline* pDbSpline)
{
	if (pDbSpline != NULL) {

		int nKnots, nCtrlPoints, degree;

		pDbSpline->get_71(&degree);
		pDbSpline->get_72(&nKnots);
		pDbSpline->get_73(&nCtrlPoints);

		RC rc;

		CControlPoints* pControlPoints = new CControlPoints(nCtrlPoints, rc);

		sds_point ctrlPoint;
		for (register i = 0; i < nCtrlPoints; i++) {
			pDbSpline->getControlPoint(ctrlPoint, i);
			pControlPoints->Set(i, C3Point(ctrlPoint[0], ctrlPoint[1], ctrlPoint[2]));
		}

		CRealArray* pWeights = NULL;
		if (pDbSpline->isRational()) {
			sds_real weight;
			pWeights = new CRealArray(nCtrlPoints, rc);
			for (i = 0; i < nCtrlPoints; i++) {
				pDbSpline->getWeight(weight, i);
				(*pWeights)[i] = weight;
			}
		}

		double knot;
		CKnots* pKnotVector = new CKnots(degree, nCtrlPoints, rc);
		for (i = 0; i < nCtrlPoints; i++) {
			pDbSpline->getKnot(knot, i);
			(*pKnotVector)[i] = knot;
		}

		// Last knot
		pDbSpline->getKnot(knot, nCtrlPoints);
		pKnotVector->Set(nCtrlPoints, knot);
		CNURBS* pNurbs = (pWeights) ?
			             new CNURBS(pKnotVector, pControlPoints, pWeights, rc) :
						 new CNURBS(pKnotVector, pControlPoints, rc);

		if (rc != SUCCESS) {
			delete pNurbs;
			return NULL;
		}
		else
			return pNurbs;
	}
	else
		return NULL;
}


/*****************************************************************/

// determine whether a db_handitem is a Spline Frame Vertex
static bool isSplineFrameVertex(db_handitem * pHanditem)
{
	bool SplineFrameVertex;
	int flags;

	if (pHanditem->ret_type() == DB_VERTEX) {
		pHanditem->get_70(&flags);

		if (flags & IC_VERTEX_SPLINEFRAME)
			SplineFrameVertex = true;
		else
			SplineFrameVertex = false;

	}
	else {
		SplineFrameVertex = false;
	}

	return SplineFrameVertex;
}


db_handitem* GetNextVertex(db_handitem* pHanditem,
                           C3Point& ptVertex,		// O: The vertex point
	                       double& bulge)		    // O: The bulge
{
	sds_point point;

	// get the next handle
	pHanditem = pHanditem->next;

	// skip over all db_handitems that are of type DB_VERTEX
	// but that are flagged as Spline Frame Control Points
	// This type of vertex should not participate in the
	// DWGToGeo conversion.

	while (isSplineFrameVertex(pHanditem))
		pHanditem = pHanditem->next;

	// check to see if it is a vertex.
	if (pHanditem->ret_type() != DB_VERTEX)
		return NULL;

	// Get the data
	pHanditem->get_10(point);
    ptVertex.SetX(point[0]); ptVertex.SetY(point[1]); ptVertex.SetZ(point[2]);
	pHanditem->get_42(&bulge);

	return pHanditem;
}


PCurve ExtractPolyEdge(const C3Point &ptStart, double bulge, const C3Point &ptEnd)
{
	PCurve pCurve = NULL;

    RC rc;
	if (PointDistance(ptStart, ptEnd) < FUZZ_DIST)
		// It's a degenerate edge, skip it
		return pCurve;

	if (bulge != 0) {
		// Try to construct an arc
		pCurve = new CEllipArc(ptStart, ptEnd, bulge, rc);
        if (rc) {
			// Arc construction failed, we'll try a line segment
			delete pCurve;
			pCurve = NULL;
		}
	}

	// if arc failed or if a line instead of an arc - create a line
	if (rc || bulge == 0)
		pCurve = new CLine(ptStart, ptEnd);
			
    return pCurve;
}

CCompositeCurve* asGeoCompositeCurve(db_lwpolyline* pLWPolyline)
{
    CCompositeCurve* pCompositeCurve = NULL;
	
    CPath path;
    sds_point * pSdsPoints = NULL;
    C3Point * pVertexArray = NULL;
	int numVertices = pLWPolyline->ret_90();	
	if (numVertices < 2) 
		return pCompositeCurve;
	else {  
        int flags;
		pLWPolyline->get_70(&flags);

        bool closed;
		if (flags & IC_PLINE_CLOSED)
			closed = true;
		else
			closed = false;
		
		// allocate arrays of sds_points and C3Points
		pSdsPoints = new sds_point[numVertices];
		pVertexArray = new C3Point[numVertices];
		PCurve pCurve = NULL;

        double elev;
        pLWPolyline->get_38(&elev);

        sds_point normal;
        pLWPolyline->get_210(normal);
		
        CAffine tr(C3Vector(normal[0], normal[1], normal[2]));

		pLWPolyline->get_10(pSdsPoints, numVertices);
		
		for (register i = 0; i < numVertices; i++) {
            pVertexArray[i].SetX(pSdsPoints[i][0]);
            pVertexArray[i].SetY(pSdsPoints[i][1]);
			pVertexArray[i].SetZ(elev);
		}
		delete [] pSdsPoints;
        pSdsPoints = NULL;
		
        double rEndParam = 0.0;
		double * pBulgeArray = NULL;
		if (pLWPolyline->hasbulges()) {  
			pBulgeArray = new double[numVertices];
			
			pLWPolyline->get_42array(pBulgeArray, numVertices);

			for (i = 0; i < numVertices - 1; i++) {
				pCurve = ExtractPolyEdge(pVertexArray[i], pBulgeArray[i], pVertexArray[i+1]);
                if (pCurve) {
                    rEndParam += pCurve->End() - pCurve->Start();
                    path.Add(pCurve); 
                }
                else 
                    QUIT;
			}

			if (closed) {
				pCurve = ExtractPolyEdge(pVertexArray[numVertices - 1], pBulgeArray[numVertices - 1], pVertexArray[0]);
                if (pCurve) {
                    rEndParam += pCurve->End() - pCurve->Start();
				    path.Add(pCurve); 
                }
                else
                    QUIT;
            }
			path.WrapUp();
			delete [] pBulgeArray;
            pBulgeArray = NULL;
		}
		else {  // begin case with no bulges
			for (i = 0; i < numVertices - 1; i++) {
				pCurve = new CLine(pVertexArray[i], pVertexArray[i + 1]);
                if (pCurve) {
                    rEndParam += pCurve->End() - pCurve->Start();
                    path.Add(pCurve);
                }
                else
                    QUIT;
			}
			
			if (closed) {
				pCurve = new CLine(pVertexArray[numVertices - 1], pVertexArray[0]);
                if (pCurve) {
                    rEndParam += pCurve->End() - pCurve->Start();
				    path.Add(pCurve);
                }
                else
					QUIT;		
			}
			path.WrapUp();
	    }   //  end case with no bulges
		
		delete [] pVertexArray;
        pVertexArray = NULL;

        path.SetStartTrim(0.0);
        path.SetEndTrim(rEndParam);

        RC rc;
	    pCompositeCurve = new CCompositeCurve(&path, 0, path.Size() - 1, rc);
        if (rc != SUCCESS) {
            delete pCompositeCurve;
            return NULL;
        }
        else
            rc = pCompositeCurve->Transform(tr);
	}  
    
    return pCompositeCurve;

exit:
    if (pCompositeCurve)
        delete pCompositeCurve;
    else {
        for (register j = 0; j < path.Size(); j++)
            delete path.Curve(j);
    }

    if (pSdsPoints)
		delete [] pSdsPoints;

    if (pVertexArray)
		delete [] pVertexArray;

    return NULL;
}



CCompositeCurve* asGeoCompositeCurve(db_polyline* pPolyline)
{
	RC rc = SUCCESS;
	int flags;
	double b, bNext;
	C3Point ptVertex, ptNextVertex, ptFirstVertex;

    CCompositeCurve* pCompositeCurve = NULL;
	// Get the flags and reject if this is a mesh
	pPolyline->get_70(&flags);
	if ( (flags & IC_PLINE_3DMESH) || (flags & IC_PLINE_POLYFACEMESH) ) 
		return pCompositeCurve;

    bool bIs3D = false;
    if (flags & IC_PLINE_3DPLINE)
        bIs3D = true;

    double elev;
    pPolyline->get_38(&elev);

    sds_point normal;
    pPolyline->get_210(normal);
		
    CAffine tr(C3Vector(normal[0], normal[1], normal[2]));

    CPath path;

	// Get the first vertex
	db_handitem* pNextItem = GetNextVertex(pPolyline, ptVertex, b);
	ptFirstVertex = ptVertex;

    double rEndParam = 0.0;
    PCurve pCurve = NULL;
	while (GetNextVertex(pNextItem, ptNextVertex, bNext)) {

		pCurve = ExtractPolyEdge(ptVertex, b, ptNextVertex);
        if (!pCurve) 
			QUIT
        else {
            rEndParam += pCurve->End() - pCurve->Start();
            path.Add(pCurve);
        }

		ptVertex = ptNextVertex;
		b = bNext;
	}
			
	// Create the closing edge, if the polyline is closed
	if (flags & 1) {
		pCurve = ExtractPolyEdge(ptVertex, b, ptFirstVertex);
        if (!pCurve)
			QUIT
        else {
            rEndParam += pCurve->End() - pCurve->Start();
            path.Add(pCurve);
        }
    }
	path.WrapUp();

    path.SetStartTrim(0.0);
    path.SetEndTrim(rEndParam);
	pCompositeCurve = new CCompositeCurve(&path, 0, path.Size() - 1, rc);
    if (rc != SUCCESS) {
        delete pCompositeCurve;
        return NULL;
    }
    else {
        if (!bIs3D)
            rc = pCompositeCurve->Transform(tr);
    }
    
    return pCompositeCurve;
exit:

    return NULL;
}