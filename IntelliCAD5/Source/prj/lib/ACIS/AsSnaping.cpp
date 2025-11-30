// File  :
// Author: Alexey Malov
#include "stdafx.h"
#include "AsSnaping.h"
#include "Modeler.h" //snaping
#include "IcadDef.h" //IC_OSMODE
#include "AsTools.h"
#include "gvector.h"
using icl::point;
#include "geometry.hxx"
#include <algorithm>
using std::find;
#ifdef _TEST
#include "testunit/assert.h"
#else
#include <cassert>
#endif

int getEndSnapPoints
(
BODY* pBody,
xstd<snaping>::vector& pnts
)
{
	assert(pBody);

    ENTITY_LIST vertices;
    checkOutcome(api_get_vertices(pBody, vertices));
    
    for (ENTITY* e = 0; e = vertices.next();)
    {
        const SPAposition& p = static_cast<VERTEX*>(e)->geometry()->coords();
        pnts.push_back(snaping(point(p.x(), p.y(), p.z()), IC_OSMODE_END));
    }

	return 1;
}

int getMidSnapPoints
(
EDGE* pEdge,
xstd<snaping>::vector& pnts
)
{
	assert(pEdge);
	assert(pEdge->geometry());

    SPAposition p = edge_mid_pos(pEdge);
    pnts.push_back(snaping(point(p.x(), p.y(), p.z()), IC_OSMODE_MID));
	return 1;
}

int getCenterSnapPoints
(
EDGE* pEdge,
xstd<snaping>::vector& pnts
)
{
	assert(pEdge);
	assert(pEdge->geometry());
	
	if(!is_ELLIPSE(pEdge->geometry()))
		return 1;

    const SPAposition& p = static_cast<const ELLIPSE*>(pEdge->geometry())->centre();
    pnts.push_back(snaping(point(p.x(), p.y(), p.z()), IC_OSMODE_CENTER));
	
	return 1;
}

int getQuadrantSnapPoints
(
EDGE* pEdge,
xstd<snaping>::vector& pnts
)
{
	assert(pEdge);
	assert(pEdge->geometry());

	if(!is_ELLIPSE(pEdge->geometry()))
		return 1;

	const curve& crv = pEdge->geometry()->equation();
	SPAinterval range = pEdge->param_range();
    double q = 0.;
    for (int i = 0; i < 4; i++, q += pi()*0.5)
    {
        if (contain(range, q, pi2()))
        {
            SPAposition p = crv.eval_position(q);
            pnts.push_back(snaping(point(p.x(), p.y(), p.z()), IC_OSMODE_QUADRANT));
        }
    }
	return 1;
}

int getNearSnapPoints
(
bounded_curve* crv,
const SPAposition& pick,
const SPAunit_vector& dir,
xstd<snaping>::vector& pnts
)
{
	assert(crv);

	pick_ray pr(pick, dir);

	double t = crv->eval_t(pr);
	if (contain(crv->get_start_param(), crv->get_end_param(), t))
	{
		SPAposition p = crv->eval_position(t);
		pnts.push_back(snaping(point(p.x(), p.y(), p.z()), IC_OSMODE_NEAR));
	}

	return 1;
}

int getPerpSnapPoints
(
bounded_curve* crv,
const SPAposition& base,
xstd<snaping>::vector& pnts
)
{
	assert(crv);

	SPAposition p;
	double t;
	crv->point_perp(base, &p, 0, 0, &t);
	if (crv->is_line() || contain(crv->get_start_param(), crv->get_end_param(), t))
		pnts.push_back(snaping(point(p.x(), p.y(), p.z()), IC_OSMODE_PERP));

	return 1;
}

int getTangSnapPoints
(
EDGE* pEdge,
const SPAposition& base,
xstd<snaping>::vector& pnts
)
{
	assert(pEdge);
	assert(pEdge->geometry());

	CURVE* pCurve = pEdge->geometry();
	if (is_ELLIPSE(pCurve))
		getTangentPointsOnEllipse(pEdge, base, pnts);
	else if (is_INTCURVE(pCurve))
		getTangentPointsOnSpline(pEdge, base, pnts);

	return 1;
}

int getTangentPointsOnEllipse
(
EDGE* pEdge,
const SPAposition& base,
xstd<snaping>::vector& pnts
)
{
	assert(is_ELLIPSE(pEdge->geometry()));
	const ELLIPSE* pEllipse = static_cast<const ELLIPSE*>(pEdge->geometry());
	
	// ellipse's axes
	const SPAunit_vector& normal = pEllipse->normal();
	const SPAposition& center = pEllipse->centre();

	// project base point on ellipse plane along dir direction
	SPAposition pbase = proj_pt_to_plane(base, center, normal);
	SPAvector pebase = pbase - center;

	double param[4];
	{
		const SPAvector& major = pEllipse->major_axis();
		SPAvector minor = normal*major*pEllipse->radius_ratio();

		double pa2 = (pebase*major).len_sq();
		double pb = (pebase*minor)%normal;
		double pb2 = pb*pb;
		if (pa2+pb2 < resabs2())
			return 0;

		double ab = (major*minor)%normal;

		double A = 1./sqrt(pb2 + pa2);

		double ac1 = ab*A;
		assert(ac1 > -1. && ac1 < 1.);
		double ac2 = pb*A;
		assert(ac2 > -1. && ac2 < 1.);
		double a1 = acos(ac1);
		double a2 = acos(ac2);

		param[0] =  a1+a2;
		param[1] =  a1-a2;
		param[2] = -a1+a2;
		param[3] = -a1-a2;
	}

	SPAinterval range = pEdge->param_range();
	const curve& crv = pEllipse->equation();
	for (int i = 0, ip = 0; i < 4 && ip < 2; i++)
	{
		// such param was added already
		if (find(param, param+i, param[i]) != param+i)
			continue;
		
		if (!contain(range, param[i]))
			continue;

		SPAposition p;
		SPAvector t1;
		crv.eval(param[i], p, t1);

		SPAvector t2 = pbase - p;

		if (biparallel(t1, t2))
			pnts.push_back(snaping(point(p.x(), p.y(), p.z()), IC_OSMODE_TAN)), ++ip;
	}

	return 1;
}

int getTangentPointsOnSpline
(
EDGE* pEdge,
const SPAposition& base,
xstd<snaping>::vector& pnts
)
{
	assert(is_INTCURVE(pEdge->geometry()));
	const INTCURVE* pSpline = static_cast<const INTCURVE*>(pEdge->geometry()); 
	bs3_curve crv = static_cast<const intcurve&>(pSpline->equation()).cur();

	SPAunit_vector normal;
	if (!bs3_curve_planar(crv, normal))
		return 1;

	const int nmax = 10;
	SPAposition tanpts[nmax];
	int npts;
	if (!bs3_curve_line_tan_pt_crv(base, crv, normal, nmax, npts, tanpts))
		return 0;

	for (int i = 0; i < npts; i++)
		pnts.push_back(snaping(point(tanpts[i].x(), tanpts[i].y(), tanpts[i].z()), IC_OSMODE_TAN));

	return 1;
}
