// File  :
// Author: Alexey Malov
#include "stdafx.h"
#include "AsExtractor.h"
#include "GeoData.h"
#include "AsTools.h"
#include "gvector.h"
using icl::point;
using icl::gvector;
#include <exception>
using std::exception;

geodata::curve* extr::curve
(
EDGE* edge
)
throw (exception)
{
    CURVE* pCurve = edge->geometry();
    if (!pCurve)
		return 0;

    const ::curve& acrv = edge->geometry()->equation();
    double start = edge->start_param();
    double end = edge->end_param();
    switch (acrv.type())
    {
    case straight_type:
        {
            const ::straight& s = static_cast<const ::straight&>(acrv);
            return extr::line(s, start, end);
        }
    case ellipse_type:
        {
            const ::ellipse& e = static_cast<const ::ellipse&>(acrv);
            return extr::ellipse(e, start, end);
        }
    case intcurve_type:
        {
            const ::intcurve& ic = static_cast<const ::intcurve&>(acrv);
            return extr::spline(ic, start, end);
        }
    default:
        throw exception();
    }
}

geodata::curve* extr::line
(
const ::straight& s,
double start, double end
)
throw (exception)
{
    geodata::line* l = new geodata::line;
    s.evaluate(start, asPosition(l->m_start));
    s.evaluate(end, asPosition(l->m_end));

    return l;
}

geodata::curve* extr::ellipse
(
const ::ellipse& ae,
double start, 
double end
)
throw (exception)
{
    geodata::ellipse* ge = new geodata::ellipse;
    extr::point(ge->m_center, ae.centre);
    extr::gvector(ge->m_normal, ae.normal);
    extr::gvector(ge->m_major_axis, ae.major_axis);
    ge->m_ratio = ae.radius_ratio;
    ge->m_start = start + ae.param_off;
    ge->m_end = end + ae.param_off;

    return ge;
}

geodata::curve* extr::spline
(
const ::intcurve& ic,
double start, double end
)
throw (exception)
{
    bs3_curve bsc_full = ic.cur();
    if (!bsc_full)
        return 0;

	double fittol;
	//take subset of full curve
	bs3_curve bsc = bs3_curve_subset (bsc_full, 
									  SPAinterval(start, end),
									  SPAresabs, 
									  fittol);
	if (!bsc)
		return 0;

    geodata::aspline* as = new geodata::aspline;
    {
        int nDim;
        logical bRational;
        int nCtrlPnts;
        SPAposition* aCtrlPnts;
        double* aWeights;
        int nKnots;
        double* aKnots;
        bs3_curve_to_array(bsc, nDim, as->m_degree, bRational, nCtrlPnts, aCtrlPnts, aWeights, nKnots, aKnots);

		//define property of curve
	    as->m_flags = 0;
	    if (bs3_curve_closed(bsc))
	    {
            as->m_flags |= geodata::spline::eClosed;
	    }
	    if (bs3_curve_periodic(bsc))
	    {
		    as->m_flags |= geodata::spline::ePeriodic;
	    }
	    if (bRational)
	    {
		    as->m_flags |= geodata::spline::eRational;
		    as->m_weights.assign(aWeights, aWeights+nCtrlPnts);
	    }
	    if (nDim == 2)
	    {
		    as->m_flags |= geodata::spline::ePlanar;
	    }

        as->m_knottol = bs3_curve_knottol();
        as->m_cpttol = fittol;

        as->m_cpts.assign(reinterpret_cast<icl::point*>(aCtrlPnts),
            reinterpret_cast<icl::point*>(aCtrlPnts)+nCtrlPnts);
	    as->m_knots.assign(aKnots, aKnots+nKnots);

	    acis_free(aCtrlPnts);
	    acis_free(aKnots);
	    acis_free(aWeights);
    }

	bs3_curve_delete(bsc);

    return as;
}

void extr::point
(
icl::point& pnt,
const SPAposition& pos
)
{
    pnt.set(pos.x(), pos.y(), pos.z());
}

void extr::gvector
(
icl::gvector& gvec,
const SPAvector& vec
)
{
    gvec.set(vec.x(), vec.y(), vec.z());
}

void extr::gvector
(
icl::gvector& gvec,
const SPAunit_vector& uvec
)
{
    gvec.set(uvec.x(), uvec.y(), uvec.z());
}

