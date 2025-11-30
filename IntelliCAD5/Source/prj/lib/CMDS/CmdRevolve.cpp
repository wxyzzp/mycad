// File  :
// Author: Alexey Malov
#include "CmdCommand.h"
#include <afx.h>	/*DG - 1.3.2002*/// hack: include MFC before inclusion of windows.h in subsequential includes from CmdQueryTools.h
#include "CmdQueryTools.h"
#include "gvector.h"
using icl::point;
using icl::gvector;
#include "Geodata.h"
using geodata::curve;
#include <xmemory>
#include <vector>
using std::auto_ptr;
#include "transf.h"
using icl::transf;

class CCmdQueryLineOrLwPolyline: public CCmdQueryEntity
{
public:
	CCmdQueryLineOrLwPolyline()
		:CCmdQueryEntity()
	{
		init(ResourceString(IDC_REVOLVE_SELECT_LINE_OR_PLANAR_POLYLINE, "\nSelect line or planar polyline: "));
	}

	virtual bool isAppropriate();
};

bool CCmdQueryLineOrLwPolyline::isAppropriate()
{
	return isKindOf(getEntity(), DB_LINE, DB_LWPOLYLINE, -1);
}

class CCmdQueryAxisOfRevolution: public CCmdCommand, public TCmdQueryLink<1>
{
	// Queries:
	enum
	{
		eObject = 0,
		eXaxis,
		eYaxis,
		e2points,
		eCount
	};
    TCmdQueryPointOrKeyWord<eCount> m_selectMethod;

	// get second point on axis
    CCmdQueryPoint m_getSecondPoint;
	// get angle around the axis
	CCmdQueryAngle m_getAngleAroundAxis;

	// get axis by object
	CCmdQueryLineOrLwPolyline m_getCurve;
	// get angle around the object
	CCmdQueryAngle m_getAngleAroundTheObject;

	// get angle around OX
	CCmdQueryAngle m_getAngleAroundXaxis;

	// get angle around OY
	CCmdQueryAngle m_getAngleAroundYaxis;

	// Tools to calculate the axis of revolution:

	int getAxisOfRevolutionFromObject(point&, gvector&, double&);
	int getAxisOfRevolutionFromXaxis(point&, gvector&, double&);
	int getAxisOfRevolutionFromYaxis(point&, gvector&, double&);
	int getAxisOfRevolutionFrom2points(point&, gvector&, double&);

	// tranformation from user coordinates into world coordinates
	// variable is initialized in ctor
	transf m_u2w;

public:
	CCmdQueryAxisOfRevolution();

	virtual CCmdQuery* query() throw(exception);

	int getAxisOfRevolution(point&, gvector&, double&);
};

int CCmdQueryAxisOfRevolution::getAxisOfRevolution
(
point& root,
gvector& dir,
double& angle
)
{
	switch (m_selectMethod.getSelection())
	{
	case eObject:
		return getAxisOfRevolutionFromObject(root, dir, angle);
	case eXaxis:
		return getAxisOfRevolutionFromXaxis(root, dir, angle);
	case eYaxis:
		return getAxisOfRevolutionFromYaxis(root, dir, angle);
	case e2points:
		return getAxisOfRevolutionFrom2points(root, dir, angle);
	default:
		return 0;
	}

}

int CCmdQueryAxisOfRevolution::getAxisOfRevolutionFromObject
(
point& root,
gvector& dir,
double& angle
)
{
	using namespace geodata;

	db_handitem* dbitem = m_getCurve.getEntity();
	curve* c;
	if (!extract(*dbitem, c))
		return 0;
	auto_ptr<curve> hc(c);

	if (c->type() == eLine)
	{
		line* l = static_cast<line*>(c);

		root = l->m_start;
		dir = l->m_end - l->m_start;
	}
	else if (c->type() == ePline)
	{
		pline* p = static_cast<pline*>(c);

		if (p->m_pts.size() < 2)
			return 0;

		root = p->m_pts.front();
		dir = p->m_pts.back() - root;
	}
	else
		return 0;

	angle = m_getAngleAroundTheObject.getValue();

	return 1;
}

int CCmdQueryAxisOfRevolution::getAxisOfRevolutionFromXaxis
(
point& root,
gvector& dir,
double& angle
)
{
	root.set(0., 0., 0.);
	root *= m_u2w;
	dir.set(1., 0., 0.);
	dir *= m_u2w;
	angle = m_getAngleAroundXaxis.getValue();
	return 1;
}

int CCmdQueryAxisOfRevolution::getAxisOfRevolutionFromYaxis
(
point& root,
gvector& dir,
double& angle
)
{
	root.set(0., 0., 0.);
	root *= m_u2w;
	dir.set(0., 1., 0.);
	dir *= m_u2w;
	angle = m_getAngleAroundYaxis.getValue();
	return 1;
}

int CCmdQueryAxisOfRevolution::getAxisOfRevolutionFrom2points
(
point& root,
gvector& dir,
double& angle
)
{
	root = m_selectMethod.getPoint() * m_u2w;
	dir = m_getSecondPoint.getValue()*m_u2w - root;
	angle = m_getAngleAroundAxis.getValue();
	return 1;
}

CCmdQuery* CCmdQueryAxisOfRevolution::query() throw (exception)
{
	if (!run() || (m_rc != RTNORM && m_rc != RTNONE))
		throw exception();

	return next();
}

CCmdQueryAxisOfRevolution::CCmdQueryAxisOfRevolution()
{
	m_u2w = ucs2wcs();

	m_pStart = &m_selectMethod;
	m_selectMethod.set(eObject, &m_getCurve);
		m_getCurve.set(0, &m_getAngleAroundTheObject);
	m_selectMethod.set(eXaxis, &m_getAngleAroundXaxis);
	m_selectMethod.set(eYaxis, &m_getAngleAroundYaxis);
	m_selectMethod.set(e2points, &m_getSecondPoint);
		m_getSecondPoint.set(0, &m_getAngleAroundAxis);

	{
		m_selectMethod.init(ResourceString(IDC_REVOLVE_SPECIFY_START_POINT_FOR_AXIS_OF_REVOLUTION, "\nSpecify start point for axis of revolution or define axis by "),
							ResourceString(IDC_REVOLVE_OBJECT_XAXIS_YAXIS, "Object Xaxis Yaxis"));
	}
	{
		m_getSecondPoint.init(ResourceString(IDC_REVOLVE_SPECIFY_ENDPOINT_OF_AXIS,"\nSpecify endpoint of axis:"));
		m_getAngleAroundAxis.init(ResourceString(IDC_REVOLVE_SPECIFY_ANGLE_OF_REVOLUTION,"\nSpecify angle of revolution <360>:"));
		m_getAngleAroundAxis.setValue(icl::pi2());
	}
	{
		m_getCurve.init(ResourceString(IDC_REVOLVE_SELECT_LINE_OR_PLANAR_POLYLINE,"\nSelect line or planar polyline: "));
		m_getAngleAroundTheObject.init(ResourceString(IDC_REVOLVE_SPECIFY_ANGLE_OF_REVOLUTION,"\nSpecify angle of revolution <360>:"));
		m_getAngleAroundTheObject.setValue(icl::pi2());
	}
	{
		m_getAngleAroundXaxis.init(ResourceString(IDC_REVOLVE_SPECIFY_ANGLE_OF_REVOLUTION,"\nSpecify angle of revolution <360>:"));
		m_getAngleAroundXaxis.setValue(icl::pi2());
		m_getAngleAroundYaxis.init(ResourceString(IDC_REVOLVE_SPECIFY_ANGLE_OF_REVOLUTION,"\nSpecify angle of revolution <360>:"));
		m_getAngleAroundYaxis.setValue(icl::pi2());
	}
}

class CCmdRevolve: CCmdCommand
{
	CCmdQuerySelectionSet m_getPlanarEntities;
	CCmdQueryAxisOfRevolution m_getRevolution;

public:
	CCmdRevolve();

	virtual int run();
};

CCmdRevolve::CCmdRevolve()
{
	m_pStart = &m_getPlanarEntities;
	m_getPlanarEntities.set(0, &m_getRevolution);
	
	m_getPlanarEntities.setFilter("ELLIPSE,CIRCLE,POLYLINE,LWPOLYLINE,SPLINE,REGION"/*DNT*/);
}

int CCmdRevolve::run()
{
	if (CCmdCommand::run())
	{
		point root;
		gvector dir;
		double angle;
		m_getRevolution.getAxisOfRevolution(root, dir, angle);

		sds_name planarEntities;
		m_getPlanarEntities.getEntities(planarEntities);
		sds_name ename;
		int unqualifiedCurveCount = 0, operationFailureCount = 0;
		int color = getCurrColorIndex();
		for (long i = 0; RTNORM == sds_ssname(planarEntities, i, ename); ++i)
		{
			CDbAcisEntityData region;
			db_handitem* item = reinterpret_cast<db_handitem*>(ename[0]);
			if (item->ret_type() == DB_REGION) 
				region = reinterpret_cast<CDbAcisEntity*>(item)->getData();
			else {
				// convert to region
				//
				curve* crv = NULL;
				if (!geodata::extract(*item, crv)) {
					sds_redraw(ename, IC_REDRAW_UNHILITE);
					unqualifiedCurveCount++;
					if (crv)
						delete crv;
					continue;
				}
				if (crv->isClosed()) {
					int oc = CModeler::get()->region(crv, color, region);
					delete crv;
					if (oc <= 0) {
						sds_redraw(ename, IC_REDRAW_UNHILITE);
						unqualifiedCurveCount++;
						continue;
					}
				}
				else {
					sds_redraw(ename, IC_REDRAW_UNHILITE);
					delete crv;
					unqualifiedCurveCount++;
					continue;
				}
			}

			CDbAcisEntityData solid;
			if (angle < 0) {
				angle = -1.0 * angle;
				dir[0] = -1.0 * dir[0]; 
				dir[1] = -1.0 * dir[1];
				dir[2] = -1.0 * dir[2];
			}
			if (!CModeler::get()->revolve(region, solid, root, dir, angle, color)) {
				sds_redraw(ename, IC_REDRAW_UNHILITE);
				operationFailureCount++;
				continue;
			}
			sds_entdel(ename);

			sds_name esolid;
			create(solid, esolid);
		}
		if (unqualifiedCurveCount > 0) 
			sds_printf(ResourceString(CMD_ERR_CURVE_REJECTED, "\n%d selected curves are rejected"), unqualifiedCurveCount);

		if (operationFailureCount > 0) 
			sds_printf(ResourceString(CMD_ERR_UNABLE_TO_REVOLVE_CRV, "\nUnable to revolve %d selected curve(s)"), operationFailureCount);
	}
	return m_rc;
}

short cmd_revolve()
{
	if(!CModeler::checkFullModeler()) 
	{
		sds_printf(ResourceString(IDC_NOT_FUNCTIONAL, "\nNot functional"));
		return RTERROR;
	}

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	CCmdRevolve cmd;
	return cmd.run();
}
