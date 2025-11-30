// File  :
// Author: Alexey Malov
#include "CmdCurveConstructor.h"
#include "Geodata.h"
#include "gvector.h"
#include "values.h"
using icl::resabs;
using icl::resabs2;
using icl::pi2;
using icl::point;
using namespace geodata;

// statics
static
resbuf* addLayerAndColor
(
const char* szLayer, 
int color
);


// namespace dbcstr
int dbcstr::create
(
curve& crv, 
const char* szLayer, /*= 0*/
int color /*= -1*/
)
{
    switch (crv.type())
    {
    case eRay:
        {
            geodata::ray& r = static_cast<geodata::ray&>(crv);
            return dbcstr::ray(r, szLayer, color);
        }
    case eLine:
        {
            geodata::line& l = static_cast<geodata::line&>(crv);
            return dbcstr::line(l, szLayer, color);
        }
    case eEllipse:
        {
            geodata::ellipse& e =static_cast<geodata::ellipse&>(crv);
            return dbcstr::ellipse(e, szLayer, color);
        }
    case eISpline:
        {
            geodata::ispline& is = static_cast<geodata::ispline&>(crv);
            return dbcstr::spline(is, szLayer, color);
        }
    case eASpline:
        {
            geodata::aspline& as = static_cast<geodata::aspline&>(crv);
            return dbcstr::spline(as, szLayer, color);
        }
    default:
        return 0;
    }
}

int dbcstr::ray
(
geodata::ray& r,
const char* szLayer,
int color
)
{
    resbuf* elist = sds_buildlist(RTDXF0, db_hitype2str(DB_RAY), 
                                  10, (double*)r.m_root, 
                                  11, (double*)r.m_dir, 
                                  0);
    if (!elist)
        return 0;

    resbuf* tail = elist;
    while (tail->rbnext)
        tail = tail->rbnext;

    // Layer & color
    tail->rbnext = addLayerAndColor(szLayer, color);

    int rc = sds_entmake(elist) == RTNORM;
    sds_relrb(elist);

    return rc;
}

int dbcstr::line
(
geodata::line& l,
const char* szLayer,
int color
)
{
    sds_point extr = {0., 0., 1.};
    resbuf* elist = sds_buildlist(RTDXF0,db_hitype2str(DB_LINE),
                                  10, (double*)l.m_start,
                                  11, (double*)l.m_end,
                                  210, extr,
                                  0);
    if (!elist)
        return 0;

    resbuf* tail = elist;
    while (tail->rbnext)
        tail = tail->rbnext;

    // Layer & color
    tail->rbnext = addLayerAndColor(szLayer, color);

    // Make entity
    int rc = sds_entmake(elist) == RTNORM;
    sds_relrb(elist);

    return rc;
}

int dbcstr::lwpline
(
geodata::pline& p,
const char* szLayer,
int color
)
{
    // Lwpolyline
    assert(!(p.m_flags & (~ (pline::eClosed | pline::eLinetype)))); // only eClosed and eLinetype can be set on
    
    resbuf* header = sds_buildlist(RTDXF0, db_hitype2str(DB_LWPOLYLINE), 0);
    resbuf* tail = header;

    // Flag
    tail = tail->rbnext = sds_buildlist(70, p.m_flags, 0);

    // Number of vertices
    int n = p.m_pts.size();
    tail = tail->rbnext = sds_buildlist(90, n, 0);

    int i;
    // Vertices
    for (i = 0; i < n; i++)
    {
        tail = tail->rbnext = sds_buildlist(10, (double*)p.m_pts[i], 0);
    }

    // Bulges
    for (i = 0; i < n; i++)
    {
        tail = tail->rbnext = sds_buildlist(42, p.m_bulges[i], 0);
    }

    // Extrusion direction
    tail = tail->rbnext = sds_buildlist(210, (double*)p.m_normal, 0);

    // Layer & color
    tail->rbnext = addLayerAndColor(szLayer, color);

    // Make entity
    int rc = sds_entmake(header) == RTNORM;
    sds_relrb(header);

    return rc;
}

int dbcstr::pline3d
(
geodata::pline& p,
const char* szLayer,
int color
)
{
    // 3d Polyline
    assert(p.m_flags & pline::e3dPolyline);

    int rc;
    // Header
    {
        sds_point dummy = {0., 0., 0.};
	    resbuf* header = sds_buildlist(RTDXF0, db_hitype2str(DB_POLYLINE),
                                       66, 1, //"Entities follow" flag
                                       10, dummy,
                                       70, p.m_flags,
                                       210, (double*)p.m_normal,
                                       0);
        if (!header)
            return 0;

        resbuf* tail = header;
        while (tail->rbnext)
            tail = tail->rbnext;

        // Layer & color
        tail->rbnext = addLayerAndColor(szLayer, color);

	    rc = sds_entmake(header) == RTNORM;
        sds_relrb(header);
        if (!rc)
            return 0;
    }

    // Vertices
    {
        sds_point dummy = {0., 0., 0.};
	    resbuf* vertex = sds_buildlist(RTDXF0, db_hitype2str(DB_VERTEX),
                                       10, dummy, // geometry point
                                       70, IC_VERTEX_3DPLINE, //vertex flag
                                       42, 0.,//bulge
                                       0);
	    resbuf* pnt = vertex;
        while (pnt->restype != 10)
            pnt = pnt->rbnext;
        resbuf* bulge = vertex;
        while (pnt->restype != 42)
            pnt = pnt->rbnext;

	    for (int i = 0, n = p.m_pts.size(), bulges = p.m_bulges.size(); i < n; i++)
		{
            pnt->resval.rpoint[0] = p.m_pts[i][0];
            pnt->resval.rpoint[1] = p.m_pts[i][1];
            pnt->resval.rpoint[2] = p.m_pts[i][2];
            if (bulges)
                bulge->resval.rreal = p.m_bulges[i];

		    rc = sds_entmake(vertex) == RTNORM;
            if (!rc)
                break;
        }
	    sds_relrb(vertex);
        if (!rc)
            return 0;
    }

	// Sequence end
    {
	    resbuf* send = sds_buildlist(RTDXF0,db_hitype2str(DB_SEQEND), 0);
	    rc = sds_entmake(send) == RTNORM;
        sds_relrb(send);
    }

    return rc;
}

int dbcstr::pline
(
geodata::pline& p,
const char* szLayer,
int color
)
{
    if (p.m_flags & pline::e3dPolyline)
    {
        return dbcstr::pline3d(p, szLayer, color);
    }
    else
    {
        return dbcstr::lwpline(p, szLayer, color);
    }
}

int dbcstr::ellipse
(
geodata::ellipse& e,
const char* szLayer,
int color
)
{
    resbuf* elist = 0;
    if (fabs(e.m_ratio - 1.) < resabs())
    {
        if (e.m_normal.norm2() < resabs2())
            return 0;

        cs ecs(e.m_normal);
        point center = ecs.from_wcs(e.m_center);

        if (fabs(fabs(e.m_start - e.m_end)-pi2()) < resabs())
        {
            elist = sds_buildlist(RTDXF0, db_hitype2str(DB_CIRCLE),
                                10, (double*)center,
                                40, e.m_major_axis.norm(),
                                210, (double*)e.m_normal,
                                0);
        }
        else
        {
            elist = sds_buildlist(RTDXF0, db_hitype2str(DB_ARC),
                                10, (double*)center,
                                40, e.m_major_axis.norm(),
                                50, e.m_start,
                                51, e.m_end,
                                210, (double*)e.m_normal,
                                0);
        }
    }
    else
    {
        elist = sds_buildlist(RTDXF0,db_hitype2str(DB_ELLIPSE),
				            10, (double*)e.m_center,
				            11, (double*)e.m_major_axis,
				            40, e.m_ratio,
				            41, e.m_start,
				            42, e.m_end,
				            210, (double*)e.m_normal,
				            0);
    }
    if (!elist)
        return 0;

    resbuf* tail = elist;
    while (tail->rbnext)
        tail = tail->rbnext;

    // Layer & color
    tail->rbnext = addLayerAndColor(szLayer, color);

    // Make entity
    int rc = sds_entmake(elist) == RTNORM;
    sds_relrb(elist);

    return rc;
}

int dbcstr::spline
(
geodata::ispline& is,
const char* szLayer,
int color
)
{
    resbuf* header = sds_buildlist(RTDXF0, db_hitype2str(DB_SPLINE), 0);
    resbuf* tail = header;
    
    // Spline flag
    tail = tail->rbnext = sds_buildlist(70, is.m_flags, 0);
    if (is.m_flags & spline::ePlanar)
        // Extusion direction
        tail = tail->rbnext = sds_buildlist(210, (double*)is.m_normal, 0);

    // Degree
    tail = tail->rbnext = sds_buildlist(71, is.m_degree, 0);

    // Number of fit points
    tail = tail->rbnext = sds_buildlist(74, is.m_fpts.size(), 0);

    // Fit toerance
    tail = tail->rbnext = sds_buildlist(44, is.m_fittol, 0);

    // Start tangent
    tail = tail->rbnext = sds_buildlist(12, (double*)is.m_start_tang, 0);

    // End tangent
    tail = tail->rbnext = sds_buildlist(13, (double*)is.m_end_tang, 0);

    // Fit points
    for (int i = 0, n = is.m_fpts.size(); i < n; i++)
    {
        tail = tail->rbnext = sds_buildlist(11, (double*)is.m_fpts[i], 0);
    }

    // Layer & color
    tail->rbnext = addLayerAndColor(szLayer, color);

    // Make entity
    int rc = sds_entmake(header) == RTNORM;
    sds_relrb(header);

    return rc;
}

int dbcstr::spline
(
geodata::aspline& as,
const char* szLayer,
int color
)
{
    resbuf* header = sds_buildlist(RTDXF0, db_hitype2str(DB_SPLINE), 0);
    resbuf* tail = header;
    
    // Spline flag
    tail = tail->rbnext = sds_buildlist(70, as.m_flags, 0);
    if (as.m_flags & spline::ePlanar)
        // Extusion direction
        tail = tail->rbnext = sds_buildlist(210, (double*)as.m_normal, 0);

    // Degree
    tail = tail->rbnext = sds_buildlist(71, as.m_degree, 0);

    // Number of knots
    tail = tail->rbnext = sds_buildlist(72, as.m_knots.size(), 0);

    // Number of control points
    tail = tail->rbnext = sds_buildlist(73, as.m_cpts.size(), 0);

    // Knot tolerance
    tail = tail->rbnext = sds_buildlist(42, as.m_knottol, 0);

    // Control-point tolerance
    tail = tail->rbnext = sds_buildlist(43, as.m_cpttol, 0);

    int i;
    int n = as.m_knots.size();
    // Knots
    for (i = 0; i < n; i++)
    {
        tail = tail->rbnext = sds_buildlist(40, as.m_knots[i], 0);
    }

    if (as.m_flags & spline::eRational)
    {
        n = as.m_weights.size();
        for (i = 0; i < n; i++)
        {
            tail = tail->rbnext = sds_buildlist(41, as.m_weights[i], 0);
        }
    }

    // Control points
    n = as.m_cpts.size();
    for (i = 0; i < n; i++)
    {
        tail = tail->rbnext = sds_buildlist(10, (double*)as.m_cpts[i], 0);
    }

    // Layer & color
    tail->rbnext = addLayerAndColor(szLayer, color);

    // Make entity
    int rc = sds_entmake(header) == RTNORM;
    sds_relrb(header);

    return rc;
}

static
resbuf* addLayerAndColor(const char* szLayer, int color)
{
    resbuf* elist = 0;
    if (szLayer)
        elist = sds_buildlist(8, szLayer, 0);
    if (color > -1)
        (elist? elist->rbnext: elist) = sds_buildlist(62, color, 0);
    return elist;
}
