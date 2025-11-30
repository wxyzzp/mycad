// File  :
// Author: Alexey Malov
#include "GeoData.h"
#include "values.h"
#include <memory>
#include <math.h>
#include "point.h"
#include "knots.h"
#include "ctrlpts.h"
#include "nurbs.h"

using std::auto_ptr;
using namespace geodata;
using namespace icl;

#ifdef _DEBUG
#define inline
#include "GeoData.inl"
#undef inline
#endif

int ray::extract
(
db_ray& dbray
)
{
    dbray.get_10(m_root);
    dbray.get_11(m_dir);

    return 1;
}

int ray::extract
(
db_xline& dbray
)
{
    dbray.get_10(m_root);
    dbray.get_11(m_dir);
    return 1;
}

int line::extract
(
db_line& line
)
{
    // The start and end points are in WCS.
	line.get_10(m_start);
    line.get_11(m_end);

    return 1;
}

int pline::extract
(
db_polyline& dbpline
)
{
	dbpline.get_70(&m_flags);
	if ((m_flags & 16) || (m_flags & 64))
        return 0;

    // get extrusion direction
    dbpline.get_210(m_normal);

	bool vertexFound = false;
    db_handitem* pe = &dbpline;
    do
    {
		if (pe->ret_type() != DB_VERTEX) {
			if (!vertexFound)
				continue;
			else
				break;				// end case of == DB_VERTEX
		}
		else 
			vertexFound = true;		
        
        int f;
        pe->get_70(&f);

		if (f & IC_VERTEX_SPLINEFRAME)
			continue;

        point p;
	    pe->get_10(p);
        double bulge;
	    pe->get_42(&bulge);

        m_pts.push_back(p);
        m_bulges.push_back(bulge);
    }
    while (pe = pe->next);

    return 1;
}

int pline::extract
(
db_lwpolyline& dbpline
)
{
    // number of vertices
	long npts = dbpline.ret_90();
	if (npts < 2)
        return 0;

    m_pts.resize(npts);

    // get points
	dbpline.get_10(reinterpret_cast<sds_point*>(m_pts.begin()), npts);

    // get flags
	dbpline.get_70(&m_flags);
    
    // get extrusion direction
    dbpline.get_210(m_normal);

    cs ecs;
    ic_arbaxis(m_normal, ecs.m_x, ecs.m_y, ecs.m_z);

    // transform to wcs
    int i;
	for (i = 0; i < npts; i++)
        m_pts[i] = ecs.to_wcs(m_pts[i]);
    
    // get bulges
    m_bulges.resize(npts);
	dbpline.get_42array(m_bulges.begin(), npts);

    // check for identical successive vertices
    xstd<point>::vector::iterator ip1 = m_pts.begin();
    xstd<point>::vector::iterator ip2 = ip1; ip2++;
    xstd<double>::vector::iterator ib = m_bulges.begin();
    for (; ip2 != m_pts.end(); ip1 = ip2++, ib++)
    {
        if (*ip1 == *ip2)
        {
            m_pts.erase(ip2--);
            m_bulges.erase(ib--);
        }
    }

    return 1;
}

int ellipse::extract
(
db_arc& arc
)
{
    arc.get_50(&m_start);
    arc.get_51(&m_end);

    arc.get_210(m_normal);
    cs ecs(m_normal);

    arc.get_10(m_center);
    m_center = ecs.to_wcs(m_center);

    double radius;
    arc.get_40(&radius);
    m_major_axis[0] = radius*ecs.m_x[0];
    m_major_axis[1] = radius*ecs.m_x[1];
    m_major_axis[2] = radius*ecs.m_x[2];

    m_ratio = 1.;

	calculateEndPoints();

    return 1;
}

int ellipse::extract
(
db_circle& circle
)
{
    circle.get_210(m_normal);
    cs ecs(m_normal);

    circle.get_10(m_center);
    m_center = ecs.to_wcs(m_center);

    double radius;
    circle.get_40(&radius);
    m_major_axis[0] = radius*ecs.m_x[0];
    m_major_axis[1] = radius*ecs.m_x[1];
    m_major_axis[2] = radius*ecs.m_x[2];

    m_ratio = 1.;
    m_start = 0.;
    m_end = TWOPI;

	calculateEndPoints();

    return 1;
}

int ellipse::extract
(
db_ellipse& ell
)
{
    ell.get_210(m_normal);
    ell.get_10(m_center);
	ell.get_11(m_major_axis); 
    ell.get_40(&m_ratio);
    ell.get_41(&m_start);
    ell.get_42(&m_end);

	calculateEndPoints();

    return 1;
}


// Author:	Stephen W. Hou
// Date:	5/8/2003
//
void ellipse::calculateEndPoints()
{
	icl::gvector minor_axis = (m_normal * m_major_axis) * m_ratio;
	m_start_pt = m_center + cos(m_start) * m_major_axis + sin(m_start) * minor_axis;
	m_end_pt = m_center + cos(m_end) * m_major_axis + sin(m_end) * minor_axis;
}


int ispline::extract(db_spline& dbs)
{
    // number of fit points
    int nfpts;
    dbs.get_74(&nfpts);
    
    assert(nfpts);
    if (!nfpts)
        return 0;

    // flags
    dbs.get_70(&m_flags);

    // degree
    dbs.get_71(&m_degree);

    // extrusion direction
    dbs.get_210(m_normal);

    // fit points
    m_fpts.resize(nfpts);
    dbs.get_11(reinterpret_cast<sds_point*>(m_fpts.begin()), nfpts);

    // fit tolerance
    dbs.get_44(&m_fittol);

    // start/end tangles
    dbs.get_12(m_start_tang);
    dbs.get_13(m_end_tang);

    return 1;
}


int aspline::extract(db_spline& dbs)
{
    // number of control points (if any)
    int ncpts;
    dbs.get_73(&ncpts);
    
    assert(ncpts);
    if (!ncpts)
        return 0;

    // flags
    dbs.get_70(&m_flags);

    // degree
    dbs.get_71(&m_degree);

    // extrusion direction
    dbs.get_210(m_normal);

    // control points
    m_cpts.resize(ncpts);
    dbs.get_10(reinterpret_cast<sds_point*>(m_cpts.begin()), ncpts);

    // control points tolerance
    dbs.get_43(&m_cpttol);

    // weights
    if (m_flags & eRational)
    {
        m_weights.resize(ncpts);
        dbs.get_41array(m_weights.begin(), ncpts);
    }

    // knots
    int nknots;
    dbs.get_72(&nknots);
    m_knots.resize(nknots);
    dbs.get_40array(m_knots.begin(), nknots);

    // knot tolerance
    dbs.get_42(&m_knottol);

	return calculateEndPoints();
}


// Author:	Stephen W. Hou
// Date:	5/8/2003
//
int aspline::calculateEndPoints()
{
	RC rc;
	int nSize = m_knots.size() - m_degree - 1;
	CKnots* pKnots = new CKnots(m_degree, nSize, rc);
    if (rc == FAILURE) {
        if (pKnots)
            delete pKnots;
        return 0;
    }

	for (register i = 0; i <= nSize; i++) // NOTE: Don't change to i < nSize
		pKnots->Set(i, m_knots[i]);
    
	CControlPoints* pControlPoints = new CControlPoints(m_cpts.size(), rc);
    if (rc != SUCCESS) {
        if (pControlPoints)
            delete pControlPoints;

        if (pKnots)
            delete pKnots;
        return 0;
    }
    CRealArray* pWeights = NULL;
    if (m_weights.size()) {
	    pWeights = new CRealArray(m_cpts.size(), rc);
        if (rc != SUCCESS) {
            if (pWeights)
                delete pWeights;

            if (pControlPoints)
                delete pControlPoints;

            if (pKnots)
                delete pKnots;
            return 0;
        }
	    for (i = 0; i < m_cpts.size(); i++) 
		    (*pWeights)[i] = m_weights[i];

    }
	for (i = 0; i < m_cpts.size(); i++) 
		pControlPoints->Set(i, C3Point(m_cpts[i].x(), m_cpts[i].y(), m_cpts[i].z()));
	

	CNURBS nurbs(pKnots, pControlPoints, pWeights, rc);
    if (rc != SUCCESS)
        return 0;

	C3Point start, end;
	nurbs.Evaluate(nurbs.Start(), start);
	nurbs.Evaluate(nurbs.End(), end);

	m_start[0] = start.X(); m_start[1] = start.Y(); m_start[2] = start.Z();
	m_end[0] = end.X(); m_end[1] = end.Y(); m_end[2] = end.Z();
    return 1;
}


DB_API int geodata::extract
(
db_handitem& dbe,
curve*& pc
)
{
	switch (dbe.ret_type())
    {
	case DB_RAY:
		{
            db_ray& dbr = static_cast<db_ray&>(dbe);
            auto_ptr<ray> r(new ray);
            if (!r->extract(dbr))
                return 0;
            pc = r.release();
			pc->m_pDbEntity = &dbe;
            return 1;
		}
	case DB_XLINE:
		{
            db_xline& dbxl = static_cast<db_xline&>(dbe);
            auto_ptr<ray> r(new ray);
            if (!r->extract(dbxl))
                return 0;
            pc = r.release();
			pc->m_pDbEntity = &dbe;
            return 1;
		}
	case DB_LINE:
		{
            db_line& dbl = static_cast<db_line&>(dbe);
            auto_ptr<line> l(new line);
            if (!l->extract(dbl))
                return 0;
            pc = l.release();
			pc->m_pDbEntity = &dbe;
            return 1;
        }
	case DB_LWPOLYLINE:
		{
            db_lwpolyline& dblwp = static_cast<db_lwpolyline&>(dbe);
            auto_ptr<pline> pl(new pline);
            if (!pl->extract(dblwp))
                return 0;
            pc = pl.release();
			pc->m_pDbEntity = &dbe;
            return 1;
		}
    case DB_POLYLINE:
		{
            db_polyline& dbpline = static_cast<db_polyline&>(dbe);
            auto_ptr<pline> pl(new pline);
            if (!pl->extract(dbpline))
                return 0;
            pc = pl.release();
			pc->m_pDbEntity = &dbe;
            return 1;
		}
	case DB_ARC:
        {
            db_arc& dba = static_cast<db_arc&>(dbe);
            auto_ptr<ellipse> e(new ellipse);
            if (!e->extract(dba))
                return 0;
            pc = e.release();
			pc->m_pDbEntity = &dbe;
            return 1;
		}
	case DB_CIRCLE:
        {
            db_circle& dbc = static_cast<db_circle&>(dbe);
            auto_ptr<ellipse> c(new ellipse);
            if (!c->extract(dbc))
                return 0;
            pc = c.release();
			pc->m_pDbEntity = &dbe;
            return 1;
        }
	case DB_ELLIPSE:
		{
            db_ellipse& dbel = static_cast<db_ellipse&>(dbe);
            auto_ptr<ellipse> e(new ellipse);
            if (!e->extract(dbel))
                return 0;
            pc = e.release();
			pc->m_pDbEntity = &dbe;
            return 1;
		}
	case DB_SPLINE:
        {
		    db_spline& dbs = static_cast<db_spline&>(dbe);

            auto_ptr<aspline> as(new aspline);
            if (as->extract(dbs))
            {
                pc = as.release();
				pc->m_pDbEntity = &dbe;
                return 1;
            }
            return 0;
        }
    default:
        return 0;
	}

    return 0;
}

cs::cs(const gvector& n)
:
m_o(0., 0., 0.),
m_x(0., 0., 0.),
m_y(0., 0., 0.),
m_z(0., 0., 0.)
{
    double l = n.norm();
    if (eq(l, 0.))
        return;

    m_z = n/l;

    if (m_z == gvector(0.,0.,1.))
    {
        m_x.set(1., 0., 0.);
        m_y.set(0., 1., 0.);
        return;
    }

	double cap = 1./64.;  /* square polar cap. */

    if (eq(m_z[0], 0., cap) &&
        eq(m_z[1], 0., cap))
    {
        // ECSX = WCSY X ECSZ
        m_x.set(m_z[2], 0., -m_z[0]);
    }
    else
    {
        // ECSX = WCSZ X ECSZ
        m_x.set(-m_z[1], m_z[0], 0.);
    }

	l = m_x.norm();
    if (eq(l, 0.))
    {
        m_x.set(0.,0.,0.);
        m_y.set(0.,0.,0.);
        m_z.set(0.,0.,0.);
        return;
    }
	m_x /= l;
	m_y = m_z*m_x;
}

point cs::to_wcs(const point& p) const
{
    point pp = p[0]*m_x + p[1]*m_y + p[2]*m_z;
    pp += gvector(m_o[0], m_o[1], m_o[2]);

    return pp;
}

point cs::from_wcs(const point& p) const
{
    gvector v(p - m_o);
    return point((v%m_x), (v%m_y), (v%m_z));
}

