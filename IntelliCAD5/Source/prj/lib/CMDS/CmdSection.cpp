// File  :
// Author: Alexey Malov
#include "CmdCommand.h"
#include <afx.h>	/*DG - 1.3.2002*/// hack: include MFC before inclusion of windows.h in subsequential includes from CmdQueryTools.h
#include "CmdQueryTools.h"
#include <string>
using std::string;
#include "gvector.h"
using icl::point;
using icl::gvector;
using icl::perpendicular;
#include "GeoData.h"
#include <memory>
using std::auto_ptr;
#include "transf.h"
using icl::transf;
using icl::coordinate;
#include "values.h"
using icl::resabs;
using icl::eq;

class CCmdSelectPlanarCurve: public CCmdQueryEntity
{
public:
	virtual bool isAppropriate() const;
};

bool CCmdSelectPlanarCurve::isAppropriate() const
{
	db_handitem* dbitem = getEntity();
	if (isKindOf(dbitem, DB_ARC, DB_CIRCLE, DB_ELLIPSE, DB_LWPOLYLINE))
		return true;

	if (dbitem->ret_type() == DB_SPLINE)
	{
		int flags;
		dbitem->get_70(&flags);
		if (flags & 1<<3)
			return true;
	}

	if (dbitem->ret_type() == DB_POLYLINE)
	{
		int flags;
		dbitem->get_70(&flags);
		if (flags & 1<<3)
		{
			using namespace geodata;
			curve* crv;
			if (!extract(*dbitem, crv))
				return false;
			auto_ptr<curve> hcrv(crv);

			pline* pl = static_cast<pline*>(crv);
			int n = pl->m_pts.size();
			if (n == 0)
				return false;

			point base = pl->m_pts[0];
			for (int i = 1; i < n; ++i)
			{
				gvector chord = pl->m_pts[i] - base;
				if (!perpendicular(chord, pl->m_normal))
					return false;
			}

			return true;
		}
	}
	return false;
}

class CCmdQuerySectionPlane: public CCmdCommand, public TCmdQueryLink<1>
{
	enum
	{
		eObject = 0,
		eZaxis,
		eView,
		eXY,
		eYZ,
		eZX,
		e3points,
		eCount
	};
    TCmdQueryPointOrKeyWord<eCount> m_selectMethod;

    //method Object
    CCmdSelectPlanarCurve m_getPlanarCurve;

    //method Zaxis
    CCmdQueryPoint m_getPointOnSectionPlane;
    CCmdQueryPoint m_getPointOnZaxis;

    //method View
    CCmdQueryPoint m_getPointOnViewPlane;

    //method XY
    CCmdQueryPoint m_getPointOnXYPlane;

    //method YZ
    CCmdQueryPoint m_getPointOnYZPlane;

    //method ZX
    CCmdQueryPoint m_getPointOnZXPlane;

    //method 3point
    CCmdQueryPoint m_getSecondPoint;
    CCmdQueryPoint m_getThirdPoint;

	//transformation of user coordinate system to world cpprdinates
	//variable is initialized in ctor
	transf m_u2w;

public:
	CCmdQuerySectionPlane();
    void initScenario();
	void initPrompts();

	int getPlane(point&, gvector&) const;

	virtual CCmdQuery* query() throw (exception);

private:
	int getPlaneFrom3points(point&, gvector&) const;
	int getPlaneFromZX(point&, gvector&) const;
	int getPlaneFromYZ(point&, gvector&) const;
	int getPlaneFromXY(point&, gvector&) const;
	int getPlaneFromView(point&, gvector&) const;
	int getPlaneFromZaxis(point&, gvector&) const;
	int getPlaneFromObject(point&, gvector&) const;
};

CCmdQuery* CCmdQuerySectionPlane::query() throw (exception)
{
	// the value of m_rc will be RTNONE if users select the default value of the 
	// point on XY, YZ, or ZX plane
	if (!run() || (m_rc != RTNORM && m_rc != RTNONE))
		throw exception();

	return next();
}

int CCmdQuerySectionPlane::getPlane
(
point& root, 
gvector& normal
) const
{
	switch (m_selectMethod.getSelection())
	{
	case eObject:
		return getPlaneFromObject(root, normal);
	case eZaxis:
		return getPlaneFromZaxis(root, normal);
	case eView:
		return getPlaneFromView(root, normal);
	case eXY:
		return getPlaneFromXY(root, normal);
	case eYZ:
		return getPlaneFromYZ(root, normal);
	case eZX:
		return getPlaneFromZX(root, normal);
	case e3points:
		return getPlaneFrom3points(root, normal);
	default:
		return 0;
	}
}

int CCmdQuerySectionPlane::getPlaneFromObject
(
point& root, 
gvector& normal
) const
{
	db_handitem* dbitem = m_getPlanarCurve.getEntity();
	if (!dbitem)
		return 0;

	dbitem->get_210(normal);

	switch (dbitem->ret_type())
	{
	case DB_ARC:
	case DB_CIRCLE:
		{
			dbitem->get_10(root);
			//from ecs to wcs
			root *= coordinate(point(0.,0.,0.), normal).inverse();

			break;
		}
	case DB_ELLIPSE:
		{
			dbitem->get_10(root);

			break;
		}
	case DB_LWPOLYLINE:
		{
			db_lwpolyline* lwp = static_cast<db_lwpolyline*>(dbitem);
			if (lwp->ret_90() == 0)
				return 0;

			root = lwp->ret_10(0);

			double elevation = lwp->ret_38();
			root.z() = elevation;

			//from ecs to wcs
			root *= coordinate(point(0.,0.,0.), normal).inverse();

			break;
		}
	case DB_SPLINE:
		{
			db_spline* sp = static_cast<db_spline*>(dbitem);

			int nfit;
			dbitem->get_74(&nfit);
			if (nfit > 0)
			{
				//get fitpoint
				root = sp->retp_11(0);
				break;
			}

			int nctl;
			dbitem->get_73(&nctl);
			if (nctl > 0)
			{
				//get control point
				root = sp->retp_10(0);
				break;
			}
		}
	case DB_POLYLINE:
		{
			using namespace geodata;
			curve* crv;
			if (!extract(*dbitem, crv))
				return 0;
			auto_ptr<curve> hcrv(crv);

			root = static_cast<pline*>(crv)->m_pts[0];

			break;
		}
	default:
		return 0;
	}

	return 1;
}

int CCmdQuerySectionPlane::getPlaneFromZaxis
(
point& root, 
gvector& normal
) const
{
    root = m_getPointOnSectionPlane.getValue() * m_u2w;
	normal = m_getPointOnZaxis.getValue()*m_u2w - root;

	return normal.normalize() > resabs();
}

int CCmdQuerySectionPlane::getPlaneFromView
(
point& root, 
gvector& normal
) const
{
    root = m_getPointOnViewPlane.getValue() * m_u2w;

	resbuf rb;
	if (RTNORM != sds_getvar("VIEWDIR"/*DNT*/, &rb))
		return 0;
	normal = rb.resval.rpoint;
	normal *= m_u2w;

	return normal.normalize() > resabs();
}

int CCmdQuerySectionPlane::getPlaneFromXY
(
point& root, 
gvector& normal
) const
{
    root = m_getPointOnXYPlane.getValue() * m_u2w;
	normal.set(0., 0., 1.);
	normal *= m_u2w;

	return 1;
}

int CCmdQuerySectionPlane::getPlaneFromYZ
(
point& root, 
gvector& normal
) const
{
	root = m_getPointOnYZPlane.getValue() * m_u2w;
	normal.set(1., 0., 0.);
	normal *= m_u2w;

	return 1;
}

int CCmdQuerySectionPlane::getPlaneFromZX
(
point& root, 
gvector& normal
) const
{
	root = m_getPointOnZXPlane.getValue() * m_u2w;
	normal.set(0., 1., 0.);
	normal *= m_u2w;

	return 1;
}

int CCmdQuerySectionPlane::getPlaneFrom3points
(
point& root, 
gvector& normal
) const
{
    root = m_selectMethod.getPoint() * m_u2w;
	normal = (root - m_getSecondPoint.getValue()*m_u2w) * 
			 (root - m_getThirdPoint.getValue()*m_u2w);

	return normal.normalize() > resabs();
}

CCmdQuerySectionPlane::CCmdQuerySectionPlane()
{
	m_u2w = ucs2wcs();

	initScenario();
	initPrompts();
}

void CCmdQuerySectionPlane::initScenario()
{
    m_pStart = &m_selectMethod;

    m_selectMethod.set(eObject, &m_getPlanarCurve);
    m_selectMethod.set(eZaxis, &m_getPointOnSectionPlane);
        m_getPointOnSectionPlane.set(0, &m_getPointOnZaxis);
    m_selectMethod.set(eView, &m_getPointOnViewPlane);
    m_selectMethod.set(eXY, &m_getPointOnXYPlane);
    m_selectMethod.set(eYZ, &m_getPointOnYZPlane);
    m_selectMethod.set(eZX, &m_getPointOnZXPlane);
    m_selectMethod.set(e3points, &m_getSecondPoint);
	    m_getSecondPoint.set(0, &m_getThirdPoint);
}

void CCmdQuerySectionPlane::initPrompts()
{
    int accept = 0;
    
    m_selectMethod.init(ResourceString(IDC_SECTION_SPECIFY_FIRST_POINT_ON_SECTION_PLANE_OR, "\nSpecify first point on section plane or"),
		ResourceString(IDC_SECTION_OBJECT_ZAXIS_VIEW_XY_YZ_ZX, "Object Zaxis View XY YZ ZX"),
		accept);

    m_getPlanarCurve.           init(ResourceString(IDC_SECTION_SELECT_CIRCLE_ELLIPSE_ARC_2DSPLINE_2DPOLYLINE, "\nSelect a circle, ellipse, arc, 2D-spline, or 2D-polyline: "));
	m_getPlanarCurve.			setAccept(RSG_NONULL);
    m_getPointOnSectionPlane.   init(ResourceString(IDC_SECTION_SPECIFY_POINT_ON_THE_SECTION_PLANE, "\nSpecify a point on the section plane: "));
	m_getPointOnSectionPlane.	setAccept(RSG_NONULL);
    m_getPointOnZaxis.          init(ResourceString(IDC_SECTION_SPECIFY_POINT_ON_THE_ZAXIS, "\nSpecify a point on the Z-axis (normal) of the plane: "));
	m_getPointOnZaxis.			setAccept(RSG_NONULL);
    m_getPointOnViewPlane.      init(ResourceString(IDC_SECTION_SPECIFY_POINT_ON_THE_CURRENT, "\nSpecify a point on the current view plane <0,0,0>: "));
    m_getPointOnXYPlane.        init(ResourceString(IDC_SECTION_SPECIFY_POINT_ON_THE_XYPLANE, "\nSpecify a point on the XY-plane <0,0,0>: "));
    m_getPointOnYZPlane.        init(ResourceString(IDC_SECTION_SPECIFY_POINT_ON_THE_YZPLANE, "\nSpecify a point on the YZ-plane <0,0,0>: "));
    m_getPointOnZXPlane.        init(ResourceString(IDC_SECTION_SPECIFY_POINT_ON_THE_ZXPLANE, "\nSpecify a point on the ZX-plane <0,0,0>: "));
    m_getSecondPoint.           init(ResourceString(IDC_SECTION_SPECIFY_SECOND_POINT_ON_PLANE, "\nSpecify second point on plane: "));
	m_getSecondPoint.			setAccept(RSG_NONULL);
    m_getThirdPoint.			init(ResourceString(IDC_SECTION_SPECIFY_THIRD_POINT_ON_PLANE, "\nSpecify third point on plane: "));
	m_getThirdPoint.			setAccept(RSG_NONULL);
}

class CCmdQuerySolids: public CCmdQuerySelectionSet
{
public:
	CCmdQuerySolids();
};

CCmdQuerySolids::CCmdQuerySolids()
{
    m_sTypes = "3DSOLID"/*DNT*/;
    m_filter.resval.rstring = const_cast<char*>(m_sTypes.c_str());
}

class CCmdSection: public CCmdCommand
{
	CCmdQuerySolids m_getSolids;
	CCmdQuerySectionPlane m_getPlane;

public:
	CCmdSection()
	{
		m_pStart = &m_getSolids;
		m_getSolids.set(0, &m_getPlane);
	}
	virtual int run();
};

int CCmdSection::run()
{
	if (CCmdCommand::run())
	{
		point root;
		gvector normal;
		m_getPlane.getPlane(root, normal);

		sds_name ename;
		sds_name solids;
		m_getSolids.getEntities(solids);
		for (long i = 0l; sds_ssname(solids, i, ename) == RTNORM; ++i)
		{
			CDbAcisEntity* item = reinterpret_cast<CDbAcisEntity*>(ename[0]);
			CDbAcisEntityData region;
			if (!CModeler::get()->section(item->getData(), region, root, normal))
			{
				m_rc = RTERROR;
				return m_rc;
			}

			sds_name eregion;
			create(region, eregion);
		}
	}
	return m_rc;
}

class CCmdSlice: public CCmdCommand
{
	CCmdQuerySolids m_getSolids;
	CCmdQuerySectionPlane m_getPlane;
	
	enum
	{
		eBothSide,
		eInputPoint,
		eCount
	};
    TCmdQueryPointOrKeyWord<eCount> m_getPart;

public:
	CCmdSlice();

	virtual int run();
};

CCmdSlice::CCmdSlice()
{
	m_pStart = &m_getSolids;
	m_getSolids.set(0, &m_getPlane);
	m_getPlane.set(0, &m_getPart);

	m_getPart.init(ResourceString(IDC_SECTION_SPECIFY_POINT_ON_DESIRED_SIDE_OF_THE_PLANE, "\nSpecify a point on desired side of the plane or"),
				   ResourceString(IDC_SECTION_BOTH, "Both"), RSG_NONULL);
}


int CCmdSlice::run()
{
	if (CCmdCommand::run())
	{
		point root;
		gvector normal;
		m_getPlane.getPlane(root, normal);

		point pt;
		point* ppt = &pt;
		if (m_getPart.getSelection() == eInputPoint)
			pt = m_getPart.getPoint() * ucs2wcs();
		else
			ppt = 0;

		sds_name ename;
		sds_name solids;
		m_getSolids.getEntities(solids);
		for (long i = 0l; sds_ssname(solids, i, ename) == RTNORM; ++i)
		{
			CDbAcisEntity* item = reinterpret_cast<CDbAcisEntity*>(ename[0]);
			CDbAcisEntityData parts[2];
			if (!CModeler::get()->slice(item->getData(), parts, root, normal, ppt))
				return m_rc = RTERROR;

			char layer[100], linetype[100];
			item->ret_layerhip()->get_2(layer, 100);
			item->ret_ltypehip()->get_2(linetype, 100);
			int color = item->ret_color();
			sds_entdel(ename);

			sds_name ent;
			if (!parts[0].getSat().empty())
			{
				create(ent, parts[0], color, layer, linetype);
			}
			if (!parts[1].getSat().empty())
			{
				create(ent, parts[1], color, layer, linetype);
			}
		}
	}
	return m_rc;
}

short cmd_section()
{
	if(!CModeler::checkFullModeler()) 
	{
		sds_printf(ResourceString(IDC_NOT_FUNCTIONAL, "\nNot functional"));
		return RTERROR;
	}

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	CCmdSection cmd;
	return cmd.run();
}

short cmd_slice()
{
	if(!CModeler::checkFullModeler()) 
	{
		sds_printf(ResourceString(IDC_NOT_FUNCTIONAL, "\nNot functional"));
		return RTERROR;
	}

	//BugZilla No. 78050; 25-03-2002
	if (cmd_istransparent() == RTERROR) return RTERROR;

    CCmdSlice cmd;
    return cmd.run();
}

