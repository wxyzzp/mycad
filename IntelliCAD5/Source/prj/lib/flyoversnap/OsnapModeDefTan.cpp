#include "Icad.h" /*DNT*/
#include "IcadView.h"
#include "OsnapModeDefTan.h"
#include "CmdQueryTools.h"
#include <vector>
#include <algorithm>
#include "gvector.h"
#include "transf.h"
#include "values.h"
using namespace std;
using namespace icl;

static
bool isZaxisUcs(const gvector& vectorEcs);
static
point pointOnCircle
(
const double& p,
const point& c,
const gvector& a1,
const gvector& a2
);
static
int centerFromBulge
(
point& center,
const double& bulge,
const point& start,
const point& end,
const gvector& normal
);

struct COsnapArc
{
	icl::point m_center;
	icl::gvector m_normal;
	icl::gvector m_axis;
	double m_start;
	double m_end;

	COsnapArc
	(
	const icl::point& center,
	const icl::gvector& normal,
	const icl::gvector& axis,
	double start,
	double end
	)
	:
	m_center(center),
	m_normal(normal),
	m_axis(axis),
	m_start(start),
	m_end(end)
	{}
};

struct COsnapLine
{
	point m_start;
    point m_end;
};

int COsnapMarkerDefTan::Draw(CDC* pDC) const
{
	POINT pt[2];
	pt[0].x = m_pixPt.x + m_iSize; pt[0].y = m_pixPt.y + m_iSize;
	pt[1].x = m_pixPt.x - m_iSize; pt[1].y = m_pixPt.y - m_iSize;
	pDC->Arc(pt[0].x, pt[0].y, pt[1].x, pt[1].y, pt[0].x, pt[0].y, pt[0].x, pt[0].y );
	pt[0].y = pt[1].y = pt[1].y - m_iThickness;
	pDC->Polyline( pt, 2 );

	pt[0].y = m_pixPt.y + m_iSize; pt[1].y = pt[0].y;
	pt[0].x = m_pixPt.x + m_iSize;
	pt[1].x = pt[0].x + m_iThickness;
	pDC->Polyline( pt, 2 );
	pt[0].x = pt[1].x + m_iThickness;
	pt[1].x = pt[0].x + m_iThickness;
	pDC->Polyline( pt, 2 );
	pt[0].x = pt[1].x + m_iThickness;
	pt[1].x = pt[0].x + m_iThickness;
	pDC->Polyline( pt, 2 );
	return TRUE;
}

static 
inline CString loadString(UINT id)
{
	CString s;
	s.LoadString(id);
	return s;
}

COsnapModeDefTan::COsnapModeDefTan()
:
m_localModeString(loadString(IDC_SDS_OSNAP_DEFTAN_16)),
m_globalModeString("_DEFTAN"/*DNT*/),
m_tooltipString(loadString(IDC_SDS_OSNAP_DEFTAN_TIP)),
m_pMarker(new COsnapMarkerDefTan()),
m_lRefCount(1)
{}

const char* COsnapModeDefTan::localModeString() const
{
	return m_localModeString;
}

const char* COsnapModeDefTan::globalModeString() const 
{
	return m_globalModeString; 
}

const char* COsnapModeDefTan::tooltipString() const 
{
	return m_tooltipString; 
}

COsnapMarker* COsnapModeDefTan::marker() const
{
	return m_pMarker; 
}

long COsnapModeDefTan::AddRef()
{
	return InterlockedIncrement( &m_lRefCount );
}

void COsnapModeDefTan::Release()
{
	if( !InterlockedDecrement( &m_lRefCount ) )
		delete this;
}

COsnapModeDefTan::~COsnapModeDefTan()
{
	if( m_pMarker )
		m_pMarker->Release();
}

int COsnapModeDefTan::getOsnapPoints
(
resbuf *elist, 
sds_point inPtucs, 
sds_point ptAp1ucs, 
sds_point ptAp2ucs, 
sds_resbuf **ppPointList
)
{
	char etype[IC_ACADNMLEN];
	ic_assoc(elist,0);
	strncpy(etype,ic_rbassoc->resval.rstring,sizeof(etype)-1);

	extern bool CMD_COMMAND_LINE;

	if (!strsame(etype,db_hitype2str(DB_ARC)) && 
		!strsame(etype,db_hitype2str(DB_CIRCLE)) &&
		!strsame(etype,db_hitype2str(DB_LWPOLYLINE)) &&
		(CMD_COMMAND_LINE || !strsame(etype,db_hitype2str(DB_LINE))) &&
		(CMD_COMMAND_LINE || !strsame(etype,db_hitype2str(DB_RAY))) &&
		(CMD_COMMAND_LINE || !strsame(etype,db_hitype2str(DB_XLINE))))
	{
		*ppPointList = 0;
		return 1;
	}

	gvector normalWcs;
	ic_assoc(elist, 210);
	normalWcs = ic_rbassoc->resval.rpoint;

	if (!isZaxisUcs(normalWcs))
		return 0;

	double elevation;
	ic_assoc(elist, 38);
	elevation = ic_rbassoc->resval.rreal;

	transf w2e = coordinate(point(0.,0.,0./*elevation*/), normalWcs);
	transf e2w(w2e); e2w.inverse();
	transf u2w = ucs2wcs();

	point pickWcs = point(inPtucs)*u2w;

	resbuf* points = 0;
	*ppPointList = 0;

	vector<COsnapArc> arcs;
	vector<COsnapLine> lines;

	if (strsame(etype, db_hitype2str(DB_LWPOLYLINE)))
	{
		ic_assoc(elist, 90);
		long npts = ic_rbassoc->resval.rlong;
		if (npts < 2)
			return 0;

		resbuf* rb;
		// get bulges, get vertices
		vector<double> bulges;
		vector<point> vertices;
		int flag = 0;
        for (rb = elist; rb != NULL; rb = rb->rbnext)
		{
            if (rb->restype == 42)
				bulges.push_back(rb->resval.rreal);
            if (rb->restype == 10)
			{
				vertices.push_back(rb->resval.rpoint);
				vertices.back() *= e2w;
			}

			if (rb->restype == 70)
				flag = rb->resval.rint;
		}
		if (bulges.empty())
			return 0;

		//if closed add point to the end
		if (flag & 1)
		{
			vertices.push_back(point(vertices.front()));
			bulges.push_back(double(bulges.front()));
		}

		//create arcs
		std::vector<double>::iterator b;
		std::vector<point>::iterator v;
		for (b = bulges.begin(), v = vertices.begin();
				(b != (bulges.end()-1)) && (v != (vertices.end()-1));
				++b, ++v)
		{
			double bulge = *b;
			if (fabs(bulge) < icl::resnorm())
			{
				if (CMD_COMMAND_LINE)
					continue;

				COsnapLine l;
				l.m_start = *v;
				l.m_end = *(v+1);

				lines.push_back(l);
			}
			else
			{
#if 0
				point center;
				double radius, sa, ea;
				ic_bulge2arc(*v, *(v+1), bulge, center, &radius, &sa, &ea);

				COsnapArc arc(center, normalWcs, axis1Wcs, start_param, end_param);
				arcs.push_back(arc);
#else
				const point& startWcs = *v;
				point& endWcs = *(v+1);
				point centerWcs;
				if (centerFromBulge(centerWcs, bulge, startWcs, endWcs, normalWcs))
					continue;

				gvector axis1Wcs = startWcs - centerWcs;
				double start_param = 0.;
				double end_param;

				gvector axis2Wcs = normalWcs*axis1Wcs;
				gvector toEnd = endWcs - centerWcs; 
				double x = toEnd % axis1Wcs;
				double y = toEnd % axis2Wcs;
				end_param = ic_atan2(y, x);
				
				if (gt(bulge, 0., resnorm()))
				{
					while (lt(end_param, 0., resnorm()))
						end_param += pi2();
				}
				else
				{
					while (gt(end_param, 0., resnorm()))
						end_param -= pi2();
					axis1Wcs = endWcs - centerWcs;
					end_param *= -1;
				}

				assert(lt(start_param, end_param));
				
				COsnapArc arc(centerWcs, normalWcs, axis1Wcs, start_param, end_param);
				arcs.push_back(arc);
#endif
			}
		}
	}
	else if (strsame(etype, db_hitype2str(DB_LINE)))
	{
		COsnapLine l;
        for (resbuf* rb = elist; rb != NULL; rb = rb->rbnext)
		{
            if (rb->restype == 10)
				l.m_start = rb->resval.rpoint;
            if (rb->restype == 11)
				l.m_end = rb->resval.rpoint;
		}

		lines.push_back(l);
	}
	else
	{
		// CIRCLE, ARC
		point centerWcs;//p1
		double radius = 0.; //fr1
		double start = 0.; //fr2
		double end = 0.; //fr3

		for (resbuf* rbtemp = elist; rbtemp != NULL; rbtemp = rbtemp->rbnext)
		{
			if (rbtemp->restype == 10)
			{
				//center
				centerWcs = rbtemp->resval.rpoint;
				centerWcs *= e2w;
			}
			else if (rbtemp->restype==40)
				//radius
				radius = rbtemp->resval.rreal;
			else if (rbtemp->restype==50)
				//start angle
				start = rbtemp->resval.rreal;
			else if (rbtemp->restype==51)
				//end angle
				end = rbtemp->resval.rreal;
		}

		ic_normang(&start, &end);
		if (eq(start, 0., resnorm()) && eq(end, 0., resnorm()))
			end = pi2();

		//just remember the circle/arc and nearest point on it
		{
			gvector axisWcs = radius*gvector(1.,0.,0.)*e2w;
			COsnapArc arc(centerWcs, normalWcs, axisWcs, start, end);
			arcs.push_back(arc);
		}
	}

	// add nearest point on lines as snaping point
	std::vector<COsnapLine>::iterator il;
	for (il = lines.begin(); il != lines.end(); ++il)
	{
		// calculate nearest point on line
		point closestPnt;
		ic_nearest(il->m_start, il->m_end, pickWcs, closestPnt);

		points = sds_buildlist(RT3DPOINT, closestPnt.operator double *(), 0);
		points->rbnext = *ppPointList;
		*ppPointList = points;
	}

	gvector viewDirWcs;
	{
		resbuf rb;
		SDS_getvar(NULL,DB_QVIEWDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

		viewDirWcs = point(rb.resval.rpoint)*u2w;
	}

	transf w2u(u2w); w2u.inverse();
	//add nearest point on circle as snaping point
	std::vector<COsnapArc>::iterator iarc;
	for (iarc = arcs.begin(); iarc != arcs.end(); ++iarc)
	{
		point center = iarc->m_center;
		gvector normal = iarc->m_normal;

		//Project pick point on the circle plane, in view direction
		point pickOnArcPlane = icl::projection(pickWcs, center, normal, &viewDirWcs);

		//radius-vector from arc center to pick-point
		gvector ray = pickOnArcPlane - center;
		//calculate length of the 'ray' projection on arc axes
		const gvector& axis1 = iarc->m_axis;
		double a = ray % axis1;
		const gvector axis2 = iarc->m_normal*axis1;
		double b = ray % axis2;
		//calculate angle(parameter) of the 'ray' in arc axes
		double angle = ic_atan2(b, a);

		//point on the circle, with parameter 'angle' will closest to the 'pick'
		point closestPnt = pointOnCircle(angle, center, axis1, axis2);
		assert(eq((center-closestPnt).norm(), axis1.norm(), 1.e-4));

		//now manage with arcs
		double openingAngle = iarc->m_end - iarc->m_start;
		if (fabs(openingAngle) > resnorm() &&
			fabs(pi2() - openingAngle) > resnorm())
			//that's arc
		{
			//to select closest arc end, compare 'angle' with start|end parameters

			//try to reduce 'angle' to arc opening
			assert(iarc->m_start < iarc->m_end);
			while (lt(angle, iarc->m_start, resnorm()))
				angle += pi2();
			while (gt(angle, iarc->m_end, resnorm()))
				angle -= pi2();

			if (lt(angle, iarc->m_start, resnorm()) ||
				gt(angle, iarc->m_end, resnorm()))
				//the 'ray' is outside of arc opening angle
			{
				if (lt(fabs(angle - iarc->m_start), fabs(angle - iarc->m_end), resnorm()))
					closestPnt = pointOnCircle(iarc->m_start, center, axis1, axis2);
				else
					closestPnt = pointOnCircle(iarc->m_end, center, axis1, axis2);
			}
		}

		point closestPntUcs = closestPnt*w2u;

		points = sds_buildlist(RT3DPOINT, closestPntUcs.operator double *(), 0);
		points->rbnext = *ppPointList;
		*ppPointList = points;
	}

	return 1;
}

bool COsnapModeDefTan::customCondition() const
{
	// TANGENT snapping should be activated plus command LINE should be running
	resbuf rb;
	int osmode;
	if (RTNORM != SDS_getvar(NULL, DB_QOSMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))
		return false;
	
	osmode = rb.resval.rint;

	extern signed char SDS_OSNAP_TANPER_IGNORE;
	if ((osmode & IC_OSMODE_TAN) &&
		SDS_OSNAP_TANPER_IGNORE == -1)
	{
		return true;
	}
	else
	{
		return false;
	}
}

static
bool isZaxisUcs(const gvector& zaxisEcs)
{
	resbuf rbucs;
	rbucs.restype = RTSHORT;
	rbucs.resval.rint = 1;
	rbucs.rbnext = NULL;

	resbuf rbwcs;
	rbwcs.rbnext = NULL;
	rbwcs.restype = RTSHORT;
	rbwcs.resval.rint = 0;

	gvector zaxisUcs;
	sds_trans(zaxisEcs, &rbwcs, &rbucs, 1, zaxisUcs);

	if (zaxisUcs == gvector(0.,0.,1.))
		return true;
	return false;
}

static
inline
point pointOnCircle
(
const double& p,
const point& c,
const gvector& a1,
const gvector& a2
)
{
	return c + a1*cos(p) + a2*sin(p);
}

static
int centerFromBulge
(
point& center,
const double& bulge,
const point& start,
const point& end,
const gvector& normal
)
{
	if (eq(bulge, 0., resnorm()) || start == end)
		return 1;

	double dist;
	{
		double length = (end - start).norm();
		double radius = 0.25*length*(bulge + 1/bulge);
		dist = sqrt(radius*radius - length*length*0.25);
	}

	gvector dirToCenter = normal*(end-start);
	dirToCenter.normalize();

	if ((le(bulge, 1., resnorm()) && gt(bulge, 0., resnorm())) ||
		lt(bulge, -1., resnorm()))
		//turn to left from mid-point
		center = start + (end-start)*.5 + dirToCenter*dist;
	else if ((lt(bulge, 0., resnorm()) && ge(bulge, -1., resnorm())) ||
			 gt(bulge, 1., resnorm()))
		//turn to right from mid-point
		center = start + (end-start)*.5 - dirToCenter*dist;
	else
		return 2;
	return 0;
}
