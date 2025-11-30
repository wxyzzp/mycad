// File  :
// Author: Alexey Malov
#include "stdafx.h"

#ifndef BUILD_WITH_LIMITED_ACIS_SUPPORT

#include "AsConstructor.h"
#include "AsTools.h"
#include "AsAutoPtr.h"
#include "GeoData.h"
#include "AsMesh.h"
#include "api.err"
#ifdef _TEST
#include "testunit/assert.h"
#else
#include <cassert>
#endif

#include "acis.hxx"  
#include "coverapi.hxx"  
#include "api.hxx"  
#include "face.hxx"  
#include "wire.hxx"  
#include "surdef.hxx"  
#include "acis_options.hxx"  

#include <fstream.h>



// statics
namespace cstr
{
static
bool compareLists
(
ENTITY_LIST& list1,
ENTITY_LIST& list2,
ENTITY_LIST& common
);
static
void createVertices
(
const std::vector<point>&,
std::vector<VERTEX*>&
);
/*-------------------------------------------------------------------------*//**
Create wire from polyline. It's assumed, polyline is curve, not mesh!
@return 1 - for success; 0 - for error
*//*--------------------------------------------------------------------------*/
static
BODY* wire
(
const geodata::pline&
)throw (exception);
/*-------------------------------------------------------------------------*//**
Create straight wire from 2 points.
@return 1 - for success; 0 - for error
*//*--------------------------------------------------------------------------*/
static
BODY* wire
(
const SPAposition&,
const SPAposition&
)throw (exception);
}

int cstr::line
(
const geodata::curve& c,
straight& s
)
throw (exception)
{
    switch (c.type())
    {
    case geodata::eRay:
        {
            const geodata::ray& r = static_cast<const geodata::ray&>(c);
            s.root_point = asPosition(r.m_root);
            s.direction = normalise(asVector(r.m_dir));
        }
        break;
    case geodata::eLine:
        {
            const geodata::line& l = static_cast<const geodata::line&>(c);
            s.root_point = asPosition(l.m_start);
            s.direction = normalise(asPosition(l.m_end) - s.root_point);
        }
        break;
    case geodata::ePline:
        {
            const geodata::pline& p = static_cast<const geodata::pline&>(c);
            if (p.m_pts.size() < 2)
                return 0;

            s.root_point = asPosition(p.m_pts.front());
            s.direction  = normalise(asPosition(p.m_pts.back()) - s.root_point);
        }
        break;
    default:
        return 0;
    }
    return s.direction.len_sq() > resabs2();
}

PLANE* cstr::plane
(
const LOOP* pLoop
)
throw (exception)
{
	PLANE* pPlane = 0;
    API_BEGIN

    SPAposition pos0 = pLoop->start()->start_pos();
    SPAposition pos1 = pLoop->start()->next()->start_pos();
    SPAposition pos2 = pLoop->start()->next()->end_pos();

    SPAunit_vector xdir = normalise(pos2 - pos1);
    SPAunit_vector ydir = normalise(pos0 - pos1);
    SPAunit_vector normal = normalise(xdir*ydir);

    pPlane = new PLANE(pos1, normal);

    _API_END
    _checkOutcome(result);

	return pPlane;
}

BODY* cstr::wire
(
const geodata::curve& c
)
throw (exception)
{
    if (c.type() == geodata::ePline)
    {
        const geodata::pline& p = static_cast<const geodata::pline&>(c);
        return cstr::wire(p);
    }
    else
    {
		EDGE* pEdge = cstr::edge(c);
        CEntityHolder hEdge(pEdge);

		CBodyPtr pWire;
        checkOutcome(api_make_ewire(1, &pEdge, pWire.init()));
		hEdge.release();

		checkEntity(pWire);
		return pWire.release();
    }
}

static
BODY* cstr::wire
(
const geodata::pline& p
)
throw (exception)
{
    if (p.m_flags & geodata::pline::ePolyfaceMesh ||
        p.m_flags & geodata::pline::ePolygonMesh)
        throw exception();

    TAutoPtr<SPAposition, TDeleteArray<SPAposition> > pts;
    TAutoPtr<double, TDeleteArray<double> > bulges;

    int npts = p.m_pts.size();
	if (npts < 2)
        throw exception();

    if (p.m_flags & geodata::pline::eClosed)
    {
        pts = TAutoPtr<SPAposition, TDeleteArray<SPAposition> >(new SPAposition[npts+1]);
        memcpy(pts, p.m_pts.begin(), npts*sizeof(SPAposition));

        pts[npts++] = asPosition(p.m_pts.front());
    }
    else
    {
        pts = TAutoPtr<SPAposition, TDeleteArray<SPAposition> >(new SPAposition[npts]);
        memcpy(pts, p.m_pts.begin(), npts*sizeof(SPAposition));
    }

	CBodyPtr pWire;
    if (p.m_flags & geodata::pline::e3dPolyline)
    {
        checkOutcome(api_make_wire(0, npts, pts, pWire.init()));
    }
    else
    {
        bulges = TAutoPtr<double, TDeleteArray<double> >(new double[npts-1]);
        memcpy(bulges, p.m_bulges.begin(), (npts-1)*sizeof(double));

        SPAunit_vector normal = normalise(asVector(p.m_normal));

        checkOutcome(api_make_kwire(0, normal, npts, pts, bulges, pWire.init()));
    }
	checkEntity(pWire);

	return pWire.release();
}

void cstr::edges
(
BODY* pWire,
std::vector<EDGE*>& ae
)
throw (exception)
{
	ENTITY_LIST le;
	checkOutcome(api_get_edges(pWire, le));
	struct restore_vector: public vector_handler<EDGE*>
	{
		int _s;
		restore_vector(std::vector<EDGE*>& vec):
		vector_handler<EDGE*>(vec),_s(_vec.size()){}

		~restore_vector()
		{if (_ex) _vec.resize(_s);}
	} cv(ae);

	API_BEGIN
	try
	{
		le.init();
		for (EDGE* e = 0; e = static_cast<EDGE*>(le.next());)
		{
			COEDGE* c = e->coedge();
			e->set_coedge(0);
			{
				EDGE* prev = e->start()->edge();
				e->start()->set_edge(e);
				{
					EDGE* next = e->end()->edge();
					e->end()->set_edge(e);
					{
						ENTITY* n;
						checkOutcome(api_copy_entity(e, n));

						ae.push_back(static_cast<EDGE*>(n));
					}
					e->end()->set_edge(next);
				}
				e->start()->set_edge(prev);
			}
			e->set_coedge(c);
		}
	}
	catch (const exception&)
	{
		sys_error(API_FAILED);
	}
	_API_END
	if (result.ok())
		cv.release();
	_checkOutcome(result);
}

void cstr::edges
(
const geodata::curve& c,
std::vector<EDGE*>& ae
)
throw (exception)
{
    if (c.type() == geodata::ePline)
    {
        const geodata::pline& p = static_cast<const geodata::pline&>(c);
		CBodyPtr pWire(cstr::wire(p));

		cstr::edges(pWire.get(), ae);
    }
    else
    {
		ae.push_back(cstr::edge(c));
    }
}

EDGE* cstr::edge
(
const geodata::curve& c
)
throw (exception)
{
    switch (c.type())
    {
    case geodata::eLine:
        {
            const geodata::line& l = static_cast<const geodata::line&>(c);
            return cstr::edge(l);
        }
    case geodata::eEllipse:
        {
            const geodata::ellipse& e = static_cast<const geodata::ellipse&>(c);
            return cstr::edge(e);
        }
    case geodata::eISpline:
        {
            const geodata::ispline& is = static_cast<const geodata::ispline&>(c);
            return cstr::edge(is);
        }
    case geodata::eASpline:
        {
            const geodata::aspline& as = static_cast<const geodata::aspline&>(c);
            return cstr::edge(as);
        }
    default:
        throw exception();
    }
}

EDGE* cstr::edge
(
const geodata::line& l
)
throw (exception)
{
	EDGE* pEdge;
    checkOutcome(api_mk_ed_line(asPosition(l.m_start),
                                       asPosition(l.m_end), pEdge));
	return pEdge;
}

EDGE* cstr::edge
(
VERTEX* pStart,
VERTEX* pEnd
)
throw (exception)
{
    assert(pStart && pEnd);

	EDGE* pEdge;
    API_BEGIN
	{
		SPAposition root = pStart->geometry()->coords();
		SPAunit_vector dir = normalise(pEnd->geometry()->coords() - root);

		STRAIGHT* pCurve = new STRAIGHT(root, dir);
		pEdge = new EDGE(pStart, pEnd, pCurve, FORWARD);
	}
    _API_END
    _checkOutcome(result);

	return pEdge;
}

EDGE* cstr::edge
(
const geodata::ellipse& e
)
throw (exception)
{
	EDGE* pEdge;
#if 0
    API_BEGIN
	{
		SPAposition center = asPosition(e.m_center);
		SPAunit_vector normal = normalise(asVector(e.m_normal));
		vector major = asVector(e.m_major_axis);

		ELLIPSE* pEllipse = new ELLIPSE(center, normal, major, e.m_ratio);

		double start = e.m_start;
		double end = e.m_end;
		while (start > end)
			start -= pi2();

		SPAposition  p0, p1;
		pEllipse->equation().eval(start, p0);
		pEllipse->equation().eval(end, p1);

		VERTEX* v0 = new VERTEX(new APOINT(p0));
		VERTEX* v1 = new VERTEX(new APOINT(p1));

		pEdge = new EDGE(v0, v1, pEllipse, FORWARD);
	}
    _API_END
	_checkOutcome(result);
#else

	SPAposition center = asPosition(e.m_center);
	SPAvector major = asVector(e.m_major_axis);
	SPAunit_vector normal = normalise(asVector(e.m_normal));
	double start = e.m_start;
	double end = e.m_end;
	while (start > end)
		start -= pi2();

	checkOutcome(api_mk_ed_ellipse(center, normal, major, e.m_ratio, start, end, pEdge));

#endif

    return pEdge;
}

EDGE* cstr::edge
(
const geodata::ispline& is
)
throw (exception)
{
    int nfpts = is.m_fpts.size();
    if (nfpts < 2)
        throw exception();

    const SPAposition* fpts = reinterpret_cast<const SPAposition*>(is.m_fpts.begin());
    SPAunit_vector start = normalise(asVector(is.m_start_tang));
    SPAunit_vector end = normalise(asVector(is.m_end_tang));

	EDGE* pEdge;
    checkOutcome(api_curve_spline(nfpts, fpts, &start, &end, pEdge));
	return pEdge;
}

EDGE* cstr::edge
(
const geodata::aspline& as
)
throw (exception)
{
    // Nurbs data

    // control points:
    int ncpts = as.m_cpts.size();
    const SPAposition* cpts = reinterpret_cast<const SPAposition*>(as.m_cpts.begin());

    // rational:
    logical rational = as.m_flags & geodata::spline::eRational;

    // closed:
    logical closed = as.m_flags & geodata::spline::eClosed;

    // periodic:
    logical periodic = as.m_flags & geodata::spline::ePeriodic;

	EDGE* pEdge;
    checkOutcome(api_mk_ed_int_ctrlpts(as.m_degree, rational, closed, periodic,
                                              ncpts, cpts,
                                              as.m_weights.begin(), as.m_cpttol,
                                              as.m_knots.size(), as.m_knots.begin(), as.m_knottol,
                                              pEdge));
	return pEdge;
}

static
bool cstr::compareLists
(
ENTITY_LIST& list1,
ENTITY_LIST& list2,
ENTITY_LIST& common
)
{
    list1.init();
    for (ENTITY* pEnt1 = list1.next(); pEnt1; pEnt1 = list1.next())
    {
        list2.init();
        for (ENTITY* pEnt2 = list2.next(); pEnt2; pEnt2 = list2.next())
        {
            if (pEnt1 == pEnt2)
                common.add(pEnt1);
        }
    }

    return common.count() != 0;
}

FACE* cstr::face
(
const std::vector<VERTEX*>& aVertices,
const int4& face
)
throw (exception)
{
	if (aVertices.empty() || face.size() == 0)
		return 0;

	FACE* pFace;

	API_BEGIN
	try
	{
		// Create loop's coedges
		COEDGE* pFirstCoedge = 0;
		COEDGE* pPrevCoedge = 0;
		COEDGE* pPartnerCoedge = 0;
		for (int i = 0, n = face.size(); i < n; i++)
		{
			EDGE* pEdge = 0;
			REVBIT sense = REVERSED;

			int icurr = face[i]-1;
			ENTITY_LIST edges1;
			sg_q_edges_around_vertex(aVertices[icurr], &edges1);
			int inext = face[(i+1)%n]-1;
			ENTITY_LIST edges2;
			sg_q_edges_around_vertex(aVertices[inext], &edges2);

			ENTITY_LIST common;
			if (cstr::compareLists(edges1, edges2, common))
			{
				if (common.count() != 1)
					throw exception();

				pEdge = static_cast<EDGE*>(common[0]);
				sense = REVERSED;
				pPartnerCoedge = static_cast<COEDGE*>(pEdge->owner());
			}
			else
			{
				pEdge = cstr::edge(aVertices[icurr], aVertices[inext]);
				sense = FORWARD;
				pPartnerCoedge = 0;
			}

			COEDGE* pCoedge = new COEDGE(pEdge, sense, 0, 0);

			if (pPrevCoedge)
			{
				pPrevCoedge->set_next(pCoedge);
				pCoedge->set_previous(pPrevCoedge);
			}
			else
			{
				pFirstCoedge = pCoedge;
			}

			if (pPartnerCoedge)
			{
				pCoedge->set_partner(pPartnerCoedge);
				pPartnerCoedge->set_partner(pCoedge);
			}

			pPrevCoedge = pCoedge;
		}
		pPrevCoedge->set_next(pFirstCoedge);
		pFirstCoedge->set_previous(pPrevCoedge);

		// Create loop
		LOOP* pLoop = new LOOP(pFirstCoedge, 0);

		// Create geometry
		PLANE* pPlane = cstr::plane(pLoop);

		// Create face
		pFace = new FACE(pLoop, 0, pPlane, FORWARD);
	}
	catch (const exception&)
	{
		sys_error(API_FAILED);
	}
    _API_END
	_checkOutcome(result);

	return pFace;
}

BODY* cstr::body
(
ENTITY* pEntity
)
throw (exception)
{
	BODY* pBody = 0;

	API_BEGIN
	do
	{
		if (is_BODY(pEntity))
		{
			pBody = static_cast<BODY*>(pEntity);
			break;
		}

		pBody = new BODY;

		if (is_LUMP(pEntity))
		{
			LUMP* pLump = static_cast<LUMP*>(pEntity);
			pBody->set_lump(pLump);
			pLump->set_body(pBody);
			break;
		}

		LUMP* pLump = new LUMP;
		pBody->set_lump(pLump);
		pLump->set_body(pBody);

		if (is_SHELL(pEntity))
		{
			SHELL* pShell = static_cast<SHELL*>(pEntity);
			pLump->set_shell(pShell);
			pShell->set_lump(pLump);
			break;
		}

		SHELL* pShell = new SHELL;
		pLump->set_shell(pShell);
		pShell->set_lump(pLump);

		if (is_SUBSHELL(pEntity))
		{
			SUBSHELL* pSubshell = static_cast<SUBSHELL*>(pEntity);
			pShell->set_subshell(pSubshell);
			break;
		}

		if (is_FACE(pEntity))
		{
			FACE* pFace = static_cast<FACE*>(pEntity);
			pShell->set_face(pFace);
			pFace->set_shell(pShell);
			break;
		}

		sys_error(API_FAILED);
	}
	while (false);
	_API_END
	_checkOutcome(result);

	return pBody;
}

static
void cstr::createVertices
(
const std::vector<point>& points,
std::vector<VERTEX*>& aVertices
)
{
    // Create vertices
    for (int i = 0, n = points.size(); i < n; i++)
    {
        aVertices.push_back(new VERTEX(new APOINT(asPosition(points[i]))));
    }
}

BODY* cstr::body
(
const std::vector<point>& points,
const std::vector<int4>& faces
)
throw (exception)
{
	BODY* pBody = 0;

    API_BEGIN
	try
	{
		std::vector<VERTEX*> aVertices;
		cstr::createVertices(points, aVertices);

		int n = faces.size();
		std::vector<FACE*> aFaces(n);
		for (int i = 0; i < n; i++)
		{
			aFaces[i] = cstr::face(aVertices, faces[i]);

			if (i > 0)
				aFaces[i-1]->set_next(aFaces[i]);
		}

		SHELL* pShell = new SHELL(aFaces[0], 0, 0);
		LUMP* pLump = new LUMP(pShell, 0);
		pBody = new BODY(pLump);
	}
	catch (const exception&)
	{
		sys_error(API_FAILED);
	}
    _API_END
    _checkOutcome(result);

	return pBody;
}

// Construct a region from a planar closed wire. The function check if the
// wire is self-intersected, planal, and closed internally. The first BODY
// parameter contains the wire for creating a region. If the operation is 
// successful, The wire in the body will be replaced with a region and the 
// body will represent a region. The second parameter is option for specifing
// the desired normal direction and single or double side of the region. If
// the parameter value is provided, it will create a single side region. 
// Otherwise, it will create a double side region. NOTE: boolean operations
// do not work with single regions.
// 
// The function returns 1 it succeeds and one of the following error codes when
// it fails:
//
//		 0 - general failure
//		-1 - non-planar wire
//		-2 - open wire
//		-3 - self-intersected wire
// 
// Author:	Stephen W. Hou
// Date:	5/8/2003
//
int cstr::region(BODY* pBody, const SPAvector &refNormal) throw (exception)
{
	BODY* pRegion = NULL;
	WIRE* pWire = NULL;
    if (!pBody)
		return 0;
	
	ENTITY_LIST wires;
	checkOutcome(api_get_wires(pBody, wires));
	if (wires.count() > 0 && (pWire = static_cast<WIRE*>(wires[0])) != NULL) {
		// make sure the wire is planar
		//
		SPAposition centroid;
		SPAunit_vector normal; 
		if (!is_planar_wire(pWire, centroid, normal))
			return -1;

		// make sure the wire is closed
		//
		outcome oc = api_closed_wire(pWire);
		if (!oc.ok())
			return -2;

		// make sure the wire does not have self-intersections
		//
		oc = api_check_wire_self_inters(pWire);
		if (!oc.ok())
			return -3;
			
		FACE* pFace = NULL;
		oc = api_cover_wire(pWire, *(surface*)NULL, pFace);
		if (oc.ok()) {
			assert(pFace);

			if (&refNormal != NULL) {
				// make sure the face normal direction is the same as desired
				//
				if (static_cast<PLANE*>(pFace->geometry())->normal() % refNormal > 0.0) {
					if (pFace->sense() == REVERSED)
						checkOutcome(api_reverse_face(pFace));
				}
				else {
					if (pFace->sense() == FORWARD)
						checkOutcome(api_reverse_face(pFace));
				}
			}
			else { // create a double side sheet
				pFace->set_sides(DOUBLE_SIDED);
				pFace->set_cont(BOTH_OUTSIDE );
			}
			return 1;
		}
	}
	return 0;
}

#if _UNUSED
EDGE* cstr::edge
(
const SPAposition& p1,
const SPAposition& p2
)
throw (exception)
{
	EDGE* pEdge;
    checkOutcome(api_mk_ed_line(p1, p2, pEdge));
	return pEdge;
}

BODY* wire
(
const SPAposition& start,
const SPAposition& end
)
throw (exception)
{
    EDGE* pEdge = cstr::edge(start, end);
    CEntityHolder hEdge(pEdge);

	CBodyPtr pWire;
    checkOutcome(api_make_ewire(1, &pEdge, pWire.init()));
	hEdge.release();

	checkEntity(pWire);
	return pWire.release();
}


int cstr::face
(
std::vector<BODY*>& wires,
SURFACE* pSurface,
FACE*& pFace
)
{
    assert(wires.size() > 0);
    assert(pSurface);

    int i;
    int n = wires.size();
    std::vector<bool> o(wires.size());
    for (i = 0; i < n; i++)
        o[i] = true;

    if (!orient(wires.begin(), o.begin(), o.size(), true))
        return 0;

    API_BEGIN

    COEDGE* pFirstCoedge = 0;
    COEDGE* pLastCoedge = 0;
    for (i = 0; i < n; i++)
    {
        WIRE* pWire = wires[i]->lump()->shell()->wire();
        CEntityHolder hwire(pWire);

        COEDGE* pCoedge = pWire->coedge();
        pWire->set_coedge(0);

        COEDGE* pFirstWireCoedge = pCoedge;

        if (!i)
        {
            pFirstCoedge = pCoedge;
        }
        else
        {
            // link coedges
            pCoedge->set_previous(pLastCoedge);
            pLastCoedge->set_next(pCoedge);

            // make vertices shareble
            VERTEX* pVertex1 = getStart(pCoedge);
            VERTEX* pVertex2 = getEnd(pLastCoedge);

            if (pVertex1->geometry()->coords() != pVertex2->geometry()->coords())
                return 0;

            setStart(pCoedge, pVertex2);
            api_delent(pVertex1);
        }

        for (; pCoedge; pCoedge = pCoedge->next())
        {
            pCoedge->set_owner(0);

            if (pCoedge->next() == pCoedge ||
                pCoedge->next() == pFirstWireCoedge)
                break;
        }

        pLastCoedge = pCoedge;
    }
    {
        pFirstCoedge->set_previous(pLastCoedge);
        pLastCoedge->set_next(pFirstCoedge);

        // make vertices shareble
        VERTEX* pVertex1 = getStart(pFirstCoedge);
        VERTEX* pVertex2 = getEnd(pLastCoedge);

        if (pVertex1->geometry()->coords() != pVertex2->geometry()->coords())
            return 0;

        setStart(pFirstCoedge, pVertex2);
        api_delent(pVertex1);
    }

    LOOP* pLoop = new LOOP(pFirstCoedge, 0);
    pFace = new FACE(pLoop, 0, pSurface, REVERSED);
    pFace->set_sides(DOUBLE_SIDED);
    pFace->set_cont(BOTH_OUTSIDE);
#if 0
    sg_add_pcurves_to_entity(pFace);
#else
    {
        ENTITY_LIST coedges;
        api_get_coedges(pFace, coedges);

        for (int i = 0, n = coedges.count(); i < n; i++)
        {
            COEDGE* pc = (COEDGE*)coedges[i];
            sg_add_pcurve_to_coedge(pc, TRUE);
        }
    }
#endif // 0

    logical bExternal;
    checkOutcome(api_loop_external(pLoop, &bExternal));

    if (!bExternal)
    {
        pFace->set_sense(!pFace->sense(FORWARD));
#if 0
        sg_add_pcurves_to_entity(pFace);
#else
        {
            ENTITY_LIST coedges;
            api_get_coedges(pFace, coedges);

            for (int i = 0, n = coedges.count(); i < n; i++)
            {
                COEDGE* pc = (COEDGE*)coedges[i];
                sg_add_pcurve_to_coedge(pc);
            }
        }
#endif // 0
        api_loop_external(pLoop, &bExternal);
        assert(bExternal);
    }

    _API_END
    _checkOutcome(result);
	checkEntity(pFace);

    return 1;
}

#endif //_UNUSED
#endif // BUILD_WITH_LIMITED_ACIS_SUPPORT
