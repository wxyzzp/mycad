#ifndef _ENTCONVERSION_H
#define _ENTCONVERSION_H

// This file defines the functions to convert database enities to corresponding
// geometry library entities.
//
// Author: Stephen W. Hou
// Date: October 10, 2003

class db_line;
class db_ray;
class db_xline;
class db_arc;
class db_circle;
class db_ellipse;
class db_spline;
class db_lwpolyline;
class db_polyline;


class CLine;
class CRay;
class CUnboundedLine;
class CEllipse;
class CEllipArc;
class CNURBS;
class CCompositeCurve;

// Converts a DB entity to a GEO entity. The funciton returns NULL if
// the conversion fails. The caller is responsible to free memory
// allocated the curves.
//
CLine* asGeoLine(db_line* pDbLine);
CRay* asGeoRay(db_ray* pDbRay);
CUnboundedLine* asGeoUnboundedLine(db_xline* pDbXline);
CEllipse* asGeoEllipse(db_circle* pDbCircle);
CEllipArc* asGeoEllipArc(db_arc* pDbArc);
CEllipArc* asGeoEllipArc(db_ellipse* pDbEllipse);
CNURBS* asGeoSpline(db_spline* pDbSpline);
CCompositeCurve* asGeoCompositeCurve(db_lwpolyline* pLWPolyline);
CCompositeCurve* asGeoCompositeCurve(db_polyline* pPolyline);


#endif // _ENTCONVERSION_H
