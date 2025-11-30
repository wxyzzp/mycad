#include "cmds.h"/*DNT*/
#include "Icad.h" /*DNT*/
#include "IcadView.h"
#include "GeoData.h"
#include "CmdOsnapModeDefTan.h"
#include "CmdQueryTools.h"
#include <algorithm>
#include "gvector.h"
#include "transf.h"
#include "values.h"
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

int CCmdOsnapMarkerDefTan::Draw(CDC* pDC) const
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

CCmdOsnapModeDefTan::CCmdOsnapModeDefTan()
:
m_localModeString(loadString(IDC_SDS_OSNAP_DEFTAN_16)),
m_globalModeString("_DEFTAN"/*DNT*/),
m_tooltipString(loadString(IDC_SDS_OSNAP_DEFTAN_TIP)),
m_pMarker(new CCmdOsnapMarkerDefTan()),
m_bEnabled(true),
m_lRefCount(1)
{}

const char* CCmdOsnapModeDefTan::localModeString() const
{
	return m_localModeString;
}

const char* CCmdOsnapModeDefTan::globalModeString() const 
{
	return m_globalModeString; 
}

const char* CCmdOsnapModeDefTan::tooltipString() const 
{
	return m_tooltipString; 
}

COsnapMarker* CCmdOsnapModeDefTan::marker() const
{
	return m_pMarker; 
}

long CCmdOsnapModeDefTan::AddRef()
{
	return InterlockedIncrement( &m_lRefCount );
}

void CCmdOsnapModeDefTan::Release()
{
	if( !InterlockedDecrement( &m_lRefCount ) )
		delete this;
}

CCmdOsnapModeDefTan::~CCmdOsnapModeDefTan()
{
	if( m_pMarker )
		m_pMarker->Release();
}

int CCmdOsnapModeDefTan::getSnapedArc
(
const icl::point& pnt,
icl::point& center,
icl::gvector& normal,
icl::gvector& axis, 
double& start, 
double& end
) const
{
	std::vector<SOsnapArc>::const_iterator i = m_arcs.begin();
	for (; i != m_arcs.end(); ++i)
	{
		center = i->m_center;
		gvector p = pnt - center;
		axis = i->m_axis;

		double n1 = p.norm();
		double n2 = axis.norm();
		if (!eq(n1, n2, resabs()))
			continue;

		normal = i->m_normal;
		gvector b = normal*axis;

		double pa = p%axis;
		double pb = p%b;
		if (eq(pa, 0., resabs2()) &&
			eq(pb, 0., resabs2()))
			continue;

		double param = atan2(pb, pa);
		
		while (lt(param, i->m_start, resnorm()))
			param += pi2();
		while (gt(param, i->m_end, resnorm()))
			param -= pi2();

		if (ge(param, i->m_start, resnorm())&&
			le(param, i->m_end))
		{
			start = i->m_start;
			end = i->m_end;
			
			return 1;
		}
	}
	return 0;
}

int CCmdOsnapModeDefTan::getSnapedArc
(
const icl::point& pnt,
SOsnapArc& arc
) const
{
	return getSnapedArc(pnt, arc.m_center, arc.m_normal, arc.m_axis, arc.m_start, arc.m_end);
}

void CCmdOsnapModeDefTan::addSnapedArc
(
const icl::point& center, 
const icl::gvector& normal,
const icl::gvector& axis, 
const double& start, 
const double& end
)
{
	icl::gvector n = normal; n.normalize();
	SOsnapArc arc; arc.init(center, n, axis, start, end);

	if (m_arcs.end() == std::find(m_arcs.begin(), m_arcs.end(), arc))
		m_arcs.push_back(arc);
}

int CCmdOsnapModeDefTan::getOsnapPoints
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

	if (!strsame(etype,db_hitype2str(DB_ARC)) && 
		!strsame(etype,db_hitype2str(DB_CIRCLE)) &&
		!strsame(etype,db_hitype2str(DB_LWPOLYLINE)))
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

	vector<SOsnapArc> arcs;

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
			vertices.push_back(point(vertices.front()));

		//create arcs
		std::vector<double>::iterator b;
		std::vector<point>::iterator v;
		for (b = bulges.begin(), v = vertices.begin();
				(b != (bulges.end()-1)) && (v != (vertices.end()-1));
				++b, ++v)
		{
			double bulge = *b;
			if (fabs(bulge) < icl::resnorm())
				continue;

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
			
			SOsnapArc arc; arc.init(centerWcs, normalWcs, axis1Wcs, start_param, end_param);
			arcs.push_back(arc);

			addSnapedArc(centerWcs, normalWcs, axis1Wcs, start_param, end_param);
		}
	}
	else
	{
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
			addSnapedArc(centerWcs, normalWcs, axisWcs, start, end);
			
			SOsnapArc arc; arc.init(centerWcs, normalWcs, axisWcs, start, end);
			arcs.push_back(arc);
		}
	}

	transf u2w = ucs2wcs();
	point pickWcs = point(inPtucs)*u2w;
	gvector viewDirWcs;
	{
		resbuf rb;
		SDS_getvar(NULL,DB_QVIEWDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

		viewDirWcs = point(rb.resval.rpoint)*u2w;
	}

	transf w2u(u2w); w2u.inverse();
	//add nearest point on circle as snaping point
	std::vector<SOsnapArc>::iterator iarc;
	resbuf* points = 0;
	*ppPointList = 0;
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

bool CCmdOsnapModeDefTan::customCondition() const
{
	if (!m_bEnabled)
		return false;

	// TANGENT snapping should be activated plus command LINE should be running
	resbuf rb;
	int osmode;
	if (RTNORM != SDS_getvar(NULL, DB_QOSMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES))
		return false;
	
	osmode = rb.resval.rint;

	extern bool CMD_COMMAND_LINE;
	if ((osmode & IC_OSMODE_TAN) &&
		CMD_COMMAND_LINE)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void CCmdOsnapModeDefTan::enable()
{
	m_bEnabled = true;
}

void CCmdOsnapModeDefTan::disable()
{
	m_bEnabled = false;
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
