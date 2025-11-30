//
// A plane is defined by original and normal, and optional u-axis.
// Its domain is assumed to be (-INFINITY, INFINITY) in both U and
// V direction.
//

#include <math.h>
#include "plane.h"
#include "trnsform.h"
#include "line.h"

/********************************************************************************/
// 
// Default constructor.
//
// Author:  Stephen W. Hou
// Date:    6/23/2003
//
CPlane::CPlane() : m_pToEcsXform(NULL)
{
	m_Normal.Set(0.0, 0.0, 1.0);
	m_UAxis.Set(1.0, 0.0,0.0);
}

/********************************************************************************/
// 
// Constructor a plane from a normal vector.
//
// Author:  Stephen W. Hou
// Date:    7/24/2004
//
CPlane::CPlane(const C3Vector &normal)
    : m_Normal(normal)
{
    m_Normal.Normalized();

	C3Vector uAxis(1.0, 0.0, 0.0);

	if (m_Normal == uAxis)
		uAxis.Set(0.0, 1.0, 0.0);

	C3Vector vAxis = m_Normal ^ uAxis;
	if (!FZERO(uAxis * m_Normal, FUZZ_RAD)) {
		uAxis = vAxis ^ m_Normal;
		uAxis.Normalized();
	}

	m_UAxis = uAxis;

    CAffine tr(&m_Origin, &uAxis.AsPoint(), &vAxis.AsPoint(), &m_Normal.AsPoint());
    m_pToEcsXform = new CAffine;
    m_pToEcsXform->SetInverse(tr);
}

/********************************************************************************/
// 
// Constructor a plane from a point, normal vector, and an optional reference 
// direction as the U direction of plane.
//
// Author:  Stephen W. Hou
// Date:    6/23/2003
//
CPlane::CPlane(const C3Point & origin, 
			   const C3Vector & normal, 
			   const C3Vector & uRefAxis)
	: m_Origin(origin), m_Normal(normal), m_pToEcsXform(NULL)
{
	ASSERT(!FZERO(m_Normal.Magnitude(), FUZZ_DIST));

    m_Normal.Normalized();

	C3Vector uAxis;
    if (&uRefAxis) {
		uAxis = uRefAxis;
        uAxis.Normalized();
    }
	else 
		uAxis.Set(1.0, 0.0, 0.0);

	if (m_Normal == uAxis)
		uAxis.Set(0.0, 1.0, 0.0);

	C3Vector vAxis = m_Normal ^ uAxis;
	if (!FZERO(uAxis * m_Normal, FUZZ_RAD)) {
		uAxis = vAxis ^ m_Normal;
		uAxis.Normalized();
	}

	m_UAxis = uAxis;

    CAffine tr(&m_Origin, &uAxis.AsPoint(), &vAxis.AsPoint(), &m_Normal.AsPoint());
    m_pToEcsXform = new CAffine;
    m_pToEcsXform->SetInverse(tr);
}


/********************************************************************************/
// 
// Return the type of plane.
//
// Author:  Stephen W. Hou
// Date:    6/23/2003
//
CPlane::CPlane(PLANETYPE planeType) 
{
    if (planeType == TYPE_Xy) {
	    m_Normal.Set(0.0, 0.0, 1.0);
	    m_UAxis.Set(1.0, 0.0,0.0);
    }
    else if (planeType == TYPE_Yz) {
	    m_Normal.Set(1.0, 0.0, 0.0);
	    m_UAxis.Set(0.0, 1.0,0.0);
    }
    else {
	    m_Normal.Set(0.0, 1.0, 0.0);
	    m_UAxis.Set(0.0, 0.0,1.0);
    }
}

/********************************************************************************/
// 
// Make a copy of a plane.
//
// Author:  Stephen W. Hou
// Date:    6/23/2003
//
CSurface* CPlane::Clone() const
{
	return new CPlane(*this);
}


/********************************************************************************/
// 
// Set the origin of a plane.
//
// Author:  Stephen W. Hou
// Date:    6/23/2003
//
void CPlane::SetOrigin(const C3Point &origin)
{
    if (m_pToEcsXform) {
        delete m_pToEcsXform; 
        m_pToEcsXform = NULL;
    }
    m_Origin = origin;
}


/********************************************************************************/
// 
// Set the normal of a plane.
//
// Author:  Stephen W. Hou
// Date:    6/23/2003
//
void CPlane::SetNormal(const C3Vector &normal)
{
	ASSERT(!FZERO(normal.Length(), FUZZ_DIST));

    if (m_pToEcsXform) {
        delete m_pToEcsXform; 
        m_pToEcsXform = NULL;
    }
	m_Normal = normal;
	m_Normal.Normalized();
}

/********************************************************************************/
// 
// Set the U axis direction of a plane.
//
// Author:  Stephen W. Hou
// Date:    6/23/2003
//
void CPlane::SetUAxis(const C3Vector &uAxis)
{
	ASSERT(!FZERO(uAxis.Length(), FUZZ_DIST));

    if (m_pToEcsXform) {
        delete m_pToEcsXform; 
        m_pToEcsXform = NULL;
    }
	m_UAxis = uAxis;
	m_UAxis.Normalized();
}


/********************************************************************************/
// 
// Transform a plane.
//
// Author:  Stephen W. Hou
// Date:    6/23/2003
//
RC CPlane::Transform(CTransform & cTransform)
{
    if (m_pToEcsXform) {
        delete m_pToEcsXform; 
        m_pToEcsXform = NULL;
    }
	RC rc = cTransform.ApplyToPoint(m_Origin, m_Origin);
	if (rc)
		return rc;

	C3Point normal;
	rc = cTransform.ApplyToVector(m_Normal.AsPoint(), normal);
	if (rc)
		return rc;
	else
		m_Normal = normal;

	C3Point uAxus;
	rc = cTransform.ApplyToVector(m_UAxis.AsPoint(), uAxus);
	if (!rc) 
		m_UAxis = uAxus;

	return rc;
}


/********************************************************************************/
// 
// Transform the copy of a plane.
//
// Author:  Stephen W. Hou
// Date:    6/23/2003
//
RC CPlane::TransformCopy(CTransform & cTransform,    	// In: The transformation
						 PSurface & pOut) const       	// Out: the transformed surface
{
	CPlane* pCopy = new CPlane(*this);
	RC rc = pCopy->Transform(cTransform);
	if (!rc)
		pOut = pCopy;
	else
		pOut = NULL;

	return rc;
}

/********************************************************************************/
// 
// Evaluate a point on a plane.
//
// Author:  Stephen W. Hou
// Date:    6/23/2003
//
RC CPlane::Evaluate(double rU,                		// In: U parameter value to evaluate at
					double rV,                		// In: V parameter value to evaluate at
					C3Point & cPoint) const  		// Out: Point on the surface with that parameter
{
	C3Vector vAxis = m_Normal ^ m_UAxis;
	vAxis.Normalized();

	C3Point origin = m_Origin;
	CAffine tr(&origin, &m_UAxis.AsPoint(), &vAxis.AsPoint(), &m_Normal.AsPoint());

	C3Point pt(rU, rV, 0.0);
	return tr.ApplyToPoint(pt, cPoint);
}


/********************************************************************************/
// 
// Find the nearest point on a plane of a point.
//
// Author:  Stephen W. Hou
// Date:    6/23/2003
//
RC CPlane::NearestPoint(C3Point ptTo,			 		// In: The point we want to get closest to
						C3Point & ptNearest,     		// Out: The nearest point on the surface
						double & rU,            		// Out: Parameter value of Nearest in U direction
						double & rV,            		// Out: Parameter value of Nearest in V direction
						double & rDist) const			// Out: The distance from the point to the surface
{
	CAffine tr = WcsToEcsTransform();

	C3Point pt;
	tr.ApplyToPoint(ptTo, pt);

	rU = pt.X();
	rV = pt.Y();
	rDist = pt.Z();

	pt.SetZ(0.0);

    CAffine wcsXform;
    wcsXform.SetInverse(tr);
	return wcsXform.ApplyToPoint(pt, ptNearest);
}


/********************************************************************************/
// 
// Perform ray-hit test and return true with hit point on the plane if
// the plane is hit.
//
//
// Author:  Stephen W. Hou
// Date:    7/3/2004
//
BOOL CPlane::HitTest(const CRay & ray, double & rAt) const
{
    if (m_Normal.IsPerpendicularTo(C3Vector(ray.Direction())))
        return FALSE;

    C3Point ptHit, ptRayStart;
	ray.GetStartPoint(ptRayStart);
    ProjectPoint(ptRayStart, C3Vector(ray.Direction()), ptHit);
    rAt = ptRayStart.DistanceTo(ptHit);
    return TRUE;
}

/********************************************************************************/
// Project a point to a plane along a sepcified direction and return a point
// on the plane w.r.t. WCS or the plane coordinate system.
//
// Author:  Stephen W. Hou
// Date:    6/25/2004
//
RC CPlane::ProjectPoint(const C3Point &pt, 		// In: The point we want to test
						const C3Vector & along,	// In: Along the dirction
						C3Point & rPt,          // Out: Projected point on the surface
                        bool uvCooridnates) const // Optional In: // Optional In: TRUE to return the point in the plane coordinate system
{
    if (m_Normal.IsPerpendicularTo(along))
        return FAILURE;

	CAffine tr = WcsToEcsTransform();

	C3Point ePt;
	tr.ApplyToPoint(pt, ePt);

    C3Point ptNearest(ePt.X(), ePt.Y(), 0.0);

    double angle = along.GetAngleTo(m_Normal.Negate());
    if (!FZERO(angle, FUZZ_RAD)) {

        // Transform the project direction to the plane coordinate system
        //
        C3Point dir;
        tr.ApplyToVector(along.AsPoint(), dir);

        // Project the direction vertor to the X-Y-plane of plane coordinate system
        //
        C3Vector xyProjDir(dir.X(), dir.Y(), 0.0);
        xyProjDir.Normalized();

        // Find projection point on the plane.
        //
        rPt = ptNearest + ePt.Z() * tan(angle) * xyProjDir;
    }
    else 
        rPt = ptNearest;

    if (!uvCooridnates) {
        CAffine wcsXform;
        wcsXform.SetInverse(tr);
	    wcsXform.ApplyToPoint(rPt, rPt);
    }
	return SUCCESS;
}

/********************************************************************************/
// Project a vector to a plane along a sepcified direction and return a vector
// on the plane w.r.t. WCS or the plane coordinate system.
//
// Author:  Stephen W. Hou
// Date:    6/25/2004
//
RC CPlane::ProjectVector(const C3Vector &vec, 		// In: The vector we want to test
						const C3Vector & along,	    // In: Along the dirction
						C3Vector & rVec,            // Out: Projected vector on the surface
                        bool uvCooridnates) const   // Optional In: // Optional In: TRUE to return the point in the plane coordinate system
{
    C3Point pt1(0.0, 0.0, 0.0);
    C3Point pt2 = vec.AsPoint();

    C3Point ptOnPlane1, ptOnPlane2;
    if (ProjectPoint(pt1, along, ptOnPlane1, uvCooridnates) == SUCCESS && 
        ProjectPoint(pt2, along, ptOnPlane2, uvCooridnates) == SUCCESS) {
        rVec = ptOnPlane2 - ptOnPlane1;
        return SUCCESS;
    }
    return FAILURE;
}

/******************************************************************************/
// Compute the intersection of two planes and return an intersection line if 
// succese and NULL otherwise.
//
// Date:    7/25/2004
//
CLine* CPlane::IntersectWith(const CPlane &plane) const   
{
	if (m_Normal.IsParallelTo(plane.m_Normal))
		return NULL;

	C3Vector dir = m_Normal ^ plane.m_Normal;
	dir.Normalized();

	C3Point point;
	if (IntersectWith(plane, CPlane(dir), point)) 
        return new CUnboundedLine(point, dir.AsPoint());
    else
	    return NULL;
}

/********************************************************************************/
// Compute the intersection of three planes and return TRUE with the intersection
// point if there is an intersection, and FALSE otherwise.
//
// Date:    7/25/2004
//
BOOL CPlane::IntersectWith(const CPlane &plane1,         // In:  The second plane
                           const CPlane &plane2,         // In:  The third plane
                           C3Point &intersection) const  // Out: The intersection point
{
    double d = Determinant(m_Normal.AsPoint(), plane1.m_Normal.AsPoint(), 
                           plane2.m_Normal.AsPoint());

    if (FZERO(d, FUZZ_DIST))
        return FALSE;

    C3Vector n1 = plane1.m_Normal ^ plane2.m_Normal;
    C3Vector n2 = plane2.m_Normal ^ m_Normal;
    C3Vector n3 = m_Normal ^ plane1.m_Normal;

    C3Point origin;
    n1 = n1 * -DistanceTo(origin);
    n2 = n2 * -plane1.DistanceTo(origin);
    n3 = n3 * -plane2.DistanceTo(origin);

    intersection.SetX((n1[0] + n2[0] + n3[0]) / d);
    intersection.SetY((n1[1] + n2[1] + n3[1]) / d);
    intersection.SetZ((n1[2] + n2[2] + n3[2]) / d);

    return TRUE;
}


/**********************************************************************************/
// Test two planes for equality.
//
// Author:  Stephen W. Hou
// Date:    6/23/2003
//
BOOL CPlane::Equal(const CSurface & surface) const
{
	if (Type() != surface.Type())
		return FALSE;

    CPlane* pPlane = (CPlane*)const_cast<CSurface*>(&surface);

	return (m_Origin == pPlane->m_Origin &&
	        m_Normal == pPlane->m_Normal &&
	        m_UAxis == pPlane->m_UAxis);
}

/**********************************************************************************/
// Test if two planes are parallel.
//
// Author:  Stephen W. Hou
// Date:    6/23/2003
//
BOOL CPlane::IsParallelTo(const CPlane & plane) const
{
	return FEQUAL(plane.m_Normal * m_Normal, 1.0, FUZZ_DIST);
}


/**********************************************************************************/
// Test if two planes are perpendicular.
//
// Author:  Stephen W. Hou
// Date:    6/23/2003
//
BOOL CPlane::IsPerpendicularTo(const CPlane & plane) const
{
	return FZERO(plane.m_Normal * m_Normal, FUZZ_RAD);
}

/**********************************************************************************/
// Test if a point is on a plane.
//
// Author:  Stephen W. Hou
// Date:    6/23/2003
//
BOOL CPlane::IsOnPlane(const C3Point & point) const
{
	C3Vector vAxis = m_Normal ^ m_UAxis;
	vAxis.Normalized();

	C3Point origin = m_Origin;
	CAffine transf(&origin, &m_UAxis.AsPoint(), &vAxis.AsPoint(), &m_Normal.AsPoint());
	CAffine iTransf;
	iTransf.SetInverse(transf);

	C3Point pt;
	iTransf.ApplyToPoint(point, pt);
	return FZERO(pt.Z(), FUZZ_DIST);
}

/**********************************************************************************/
// Find the directed distance from a point to a plane if the point A is on the 
// same side of the plane as the side that the normal is pointing, we will return 
// a negative number.
//
// Author:  Stephen W. Hou
// Date:    6/23/2003
//

double CPlane::DistanceTo(const C3Point & point) const // I: Point from which distance is sought.
{
	return 	m_Normal.AsPoint() * (m_Origin - point);
}

/**********************************************************************************/
// Mirror a point about a plane.
//
// Author:  Stephen W. Hou
// Date:    6/23/2003
//
C3Point CPlane::Mirror(const C3Point & point) const
{
	double dist = DistanceTo(point);

	return (point + 2 * dist * m_Normal);
}

/**********************************************************************************/
// Return the type of a plane.
//
// Author:  Stephen W. Hou
// Date:    6/23/2003
//
CPlane::PLANETYPE CPlane::PlaneType() const
{
    if (m_Origin != C3Point(0.0, 0.0, 0.0))
        return TYPE_Arbitrary;

    C3Vector xAxis(1.0, 0.0, 0.0), yAxis(0.0, 1.0, 0.0), zAxis(0.0, 0.0, 1.0);
    if (FEQUAL(zAxis * m_Normal, 1.0, FUZZ_DIST) && 
        FEQUAL(xAxis * m_UAxis, 1.0, FUZZ_DIST))
        return TYPE_Xy;
    else if (FEQUAL(xAxis * m_Normal, 1.0, FUZZ_DIST) &&
        FEQUAL(yAxis * m_UAxis, 1.0, FUZZ_DIST))
        return TYPE_Yz;
    else if (FEQUAL(yAxis * m_Normal, 1.0, FUZZ_DIST) &&
        FEQUAL(zAxis * m_UAxis, 1.0, FUZZ_DIST))
        return TYPE_Zx;
    else
        return TYPE_Arbitrary;
}

/********************************************************************************/
// Return a transform from WCS to plane coordinate system.
//
// Author:  Stephen W. Hou
// Date:    6/25/2004
//
CAffine CPlane::WcsToEcsTransform() const
{
    if (!m_pToEcsXform) {
        C3Vector vDir = m_Normal ^ m_UAxis;
        C3Point origin = m_Origin;
        C3Point xDir = m_UAxis.AsPoint();
        C3Point yDir = vDir.AsPoint();
        C3Point zDir = m_Normal.AsPoint();
        CAffine tr(&origin, &xDir, &yDir, &zDir);

        CPlane* This = const_cast<CPlane*>(this);
        This->m_pToEcsXform = new CAffine;
        This->m_pToEcsXform->SetInverse(tr);
    }
    return CAffine(*m_pToEcsXform);
}


/**********************************************************************************/
// Assign operator.
//
// Author:  Stephen W. Hou
// Date:    6/23/2003
//
CPlane& CPlane::operator=(const CPlane &plane)
{
    if (plane.m_pToEcsXform) 
        m_pToEcsXform = new CAffine(*plane.m_pToEcsXform);
    else
        m_pToEcsXform = NULL;

    m_Origin = plane.m_Origin;
	m_Normal = plane.m_Normal;
	m_UAxis = plane.m_UAxis;

    return *this;
}






