#include "stdafx.h"
#include "TsTools.h"
#include "TsCrasher.h"

#include "GeoData.h"
namespace gd = geodata;

#include "gvector.h"
#include "transf.h"
using icl::point;
using icl::gvector;

#include <vector>
#include <memory>
using std::auto_ptr;

#include "testunit/collection.h"
#include "testunit/assert.h"
#include "testunit/autoregistrar.h"
using namespace testunit;

class CTsSurfaces: public collection
{
	//tests
	void surfaceRuled();
	void surfaceRuledCrashing();
	void surfaceRuledIncorrectCurves();
	void surfaceCurves();
	void surfaceCurvesCrasher();
	void surfaceCurvesMultipleLoops();
	void surfaceCurvesIncorrectParameters();
	void surfaceRevolved();
	void surfaceRevolvedCrashing();
	void surfaceRevolvedIncorrectParameters();
	void surfaceExtruded();
	void surfaceExtrudedCrashing();
	void surfaceExtrudedIncorrectParameters();
public:
	CTsSurfaces();
};

static 
tauto_registrar<CTsSurfaces> MAKE_NAME(base_collection());

CTsSurfaces::CTsSurfaces()
:collection("Test surface creation")
{
	typedef TTsMethod<CTsSurfaces> test_t;
	add(new test_t("create ruled surface", this, surfaceRuled));
	add(new test_t("ruled surface: crashing", this, surfaceRuledCrashing));
	add(new test_t("ruled surface: incorrect parameters", this, surfaceRuledIncorrectCurves));
	add(new test_t("create surface from curves", this, surfaceCurves));
	add(new test_t("create surface from curves: crashing", this, surfaceCurvesCrasher));
	add(new test_t("create surface from curves: multiple loops", this, surfaceCurvesMultipleLoops));
	add(new test_t("create surface from curves: incorrect parameters", this, surfaceCurvesIncorrectParameters));
	add(new test_t("create surface of revolution", this, surfaceRevolved));
	add(new test_t("surface of revolution: crashing", this, surfaceRevolvedCrashing));
	add(new test_t("surface of revolution: incorrect parameters", this, surfaceRevolvedIncorrectParameters));
	add(new test_t("create surface of extrusion", this, surfaceExtruded));
	add(new test_t("surface of extrusion: crashing", this, surfaceExtrudedCrashing));
	add(new test_t("surface of extrusion: incorrect parameters", this, surfaceExtrudedIncorrectParameters));
}

void CTsSurfaces::surfaceRuled()
{
	INSTALL_FINALLY_CLEANER

	gd::ellipse e;
	{
		e.m_center.set(0,0,0);
		e.m_normal.set(1,0,0);
		e.m_major_axis.set(0,10,0);
		e.m_ratio = 0.5;
		e.m_start = 0.;
		e.m_end = PI;
	}

	gd::line l;
	{
		l.m_start.set(-10,-5,0);
		l.m_end.set(-10,+5,0);
	}

	CDbAcisEntityData obj;
	assertex(modeler()->createSurfaceRuled(e, l, obj),
		errorMessage().c_str());
}

void CTsSurfaces::surfaceRuledCrashing()
{
	gd::ellipse e;
	{
		e.m_center.set(0,0,0);
		e.m_normal.set(1,0,0);
		e.m_major_axis.set(0,10,0);
		e.m_ratio = 0.5;
		e.m_start = 0.;
		e.m_end = PI;
	}

	gd::line l;
	{
		l.m_start.set(-10,-5,0);
		l.m_end.set(-10,+5,0);
	}

	g_crasher.startMonitoring();
	for (int i = 0; !i || g_crasher.startNextSession(); i++)
	{
		INSTALL_FINALLY_CLEANER
		CDbAcisEntityData obj;
		int rc = modeler()->createSurfaceRuled(e, l, obj);
		if ((rc == 0 && i == 0) ||
			(rc != 0 && i != 0))
			throwex(errorMessage().c_str());
		g_crasher.endMonitoring();
	}
}

void CTsSurfaces::surfaceRuledIncorrectCurves()
{
	INSTALL_FINALLY_CLEANER

	gd::ellipse e;
	{
		e.m_center.set(0,0,0);
		e.m_normal.set(1,0,0);
		e.m_major_axis.set(0,10,0);
		e.m_ratio = 0.5;
		e.m_start = 0.;
		e.m_end = PI;
	}

	gd::line l;
	{
		l.m_start.set(-10,0,0);
		l.m_end.set(-10,0,0);
	}

	CDbAcisEntityData obj;

	// incorrect second parameter
	assertex(!modeler()->createSurfaceRuled(e, l, obj),
		"Surface from zero-normal ellipse");

	// incorrect first parameter
	e.m_normal.set(0.,0.,0.);
	assertex(!modeler()->createSurfaceRuled(e, l, obj),
		"Surface from zero-length straight");
}

void CTsSurfaces::surfaceCurves()
{
	INSTALL_FINALLY_CLEANER

	std::vector<gd::curve*> crvs;
	std::vector<auto_ptr<gd::curve> > hcrvs;
	{
		icl::transf rot = icl::rotation(0.5*PI, point(0,0,0), gvector(0,0,1));
		
		gd::ellipse e;
		{
			e.m_center.set(10,0,0);
			e.m_normal.set(1,0,0);
			e.m_major_axis.set(0,10,0);
			e.m_ratio = 1;
			e.m_start = 0.;
			e.m_end = PI;
		}

		for (int i = 0; i < 4; i++)
		{
			crvs.push_back(new gd::ellipse(e));
			auto_ptr<gd::curve> ap(crvs.back());
			hcrvs.push_back(ap);

			e.m_center *= rot;
			e.m_normal *= rot;
			e.m_major_axis *= rot;
		}
	}

	CDbAcisEntityData obj;
	assertex(modeler()->createSurfaceCurves(crvs, obj),
		errorMessage().c_str());
}

void CTsSurfaces::surfaceCurvesCrasher()
{
	std::vector<gd::curve*> crvs;
	std::vector<auto_ptr<gd::curve> > hcrvs;
	{
		icl::transf rot = icl::rotation(0.5*PI, point(0,0,0), gvector(0,0,1));
		
		gd::ellipse e;
		{
			e.m_center.set(10,0,0);
			e.m_normal.set(1,0,0);
			e.m_major_axis.set(0,10,0);
			e.m_ratio = 1;
			e.m_start = 0.;
			e.m_end = PI;
		}

		for (int i = 0; i < 4; i++)
		{
			crvs.push_back(new gd::ellipse(e));
			auto_ptr<gd::curve> ap(crvs.back());
			hcrvs.push_back(ap);

			e.m_center *= rot;
			e.m_normal *= rot;
			e.m_major_axis *= rot;
		}
	}

	g_crasher.startMonitoring();
	for (int i = 0; !i || g_crasher.startNextSession(); i++)
	{
		INSTALL_FINALLY_CLEANER

		CDbAcisEntityData obj;
		int rc = modeler()->createSurfaceCurves(crvs, obj);
		if ((rc == 0 && i == 0) ||
			(rc != 0 && i != 0))
			throwex(errorMessage().c_str());
		g_crasher.endMonitoring();
	}
}

void CTsSurfaces::surfaceCurvesMultipleLoops()
{
	INSTALL_FINALLY_CLEANER

	std::vector<gd::curve*> crvs;
	gd::pline loop1;
	gd::pline loop2;
	{
		{
			loop1.m_pts.push_back(point(10,10,0));
			loop1.m_pts.push_back(point(-10,10,0));
			loop1.m_pts.push_back(point(-10,-10,0));
			loop1.m_pts.push_back(point(10,-10,0));
			loop1.m_normal.set(0,0,1);
			loop1.m_flags = gd::pline::eClosed | gd::pline::e3dPolyline;
		}
		{
			loop2.m_pts.push_back(point(20,20,10));
			loop2.m_pts.push_back(point(-20,20,10));
			loop2.m_pts.push_back(point(-20,-20,10));
			loop2.m_pts.push_back(point(20,-20,10));
			loop2.m_normal.set(0,0,1);
			loop2.m_flags = gd::pline::eClosed | gd::pline::e3dPolyline;
		}
		crvs.push_back(&loop1);
		crvs.push_back(&loop2);
	}

	CDbAcisEntityData obj;
	assertex(!modeler()->createSurfaceCurves(crvs, obj),
		"createSurfaceCurves can't process several loops at once ");
}

void CTsSurfaces::surfaceCurvesIncorrectParameters()
{
	INSTALL_FINALLY_CLEANER

	const int ncrv = 4;
	std::vector<gd::curve*> crvs;
	std::vector<auto_ptr<gd::curve> > hcrvs;
	{
		icl::transf rot = icl::rotation(0.5*PI, point(0,0,0), gvector(0,0,1));
		
		gd::ellipse e;
		{
			e.m_center.set(10,0,0);
			e.m_normal.set(1,0,0);
			e.m_major_axis.set(0,10,0);
			e.m_ratio = 1;
			e.m_start = 0.;
			e.m_end = PI;
		}

		for (int i = 0; i < ncrv; i++)
		{
			gd::ellipse* ec = new gd::ellipse(e);
			crvs.push_back(ec);
			auto_ptr<gd::curve> ap(crvs.back());
			hcrvs.push_back(ap);

			e.m_center *= rot;
			e.m_normal *= rot;
			e.m_major_axis *= rot;
		}
	}

	gd::ellipse* e = static_cast<gd::ellipse*>(crvs[1]);
	crvs[1] = 0;

	CDbAcisEntityData obj;
	assertex(!modeler()->createSurfaceCurves(crvs, obj),
		"surface from zero curve");

	e->m_center *= icl::translation(gvector(0,1,0));
	crvs[1] = e;
	assertex(!modeler()->createSurfaceCurves(crvs, obj),
		"surface from not-connected curves");
}

void CTsSurfaces::surfaceRevolved()
{
	INSTALL_FINALLY_CLEANER

	// polyline profile
	gd::pline profile;
	{
		icl::transf rot = icl::rotation(0.5*PI, point(0,0,0), gvector(0,0,1));
		point p(-10,-10,0);

		const int npts = 4;
		for (int i = 0; i < npts; i++, p*=rot)
		{
			profile.m_pts.push_back(p);
			profile.m_bulges.push_back(0.);
		}

		profile.m_normal.set(0,0,1);
		profile.m_flags = 0;
	}

	// line axis
	gd::line axis1;
	{
		axis1.m_start.set(20,0,0);
		axis1.m_end.set(20,10,0);
	}
	gd::pline axis2;
	{
		axis2.m_pts.push_back(point(20,0,0));
		axis2.m_pts.push_back(point(20,10,0));
	}
	gd::ray axis3;
	{
		axis3.m_root.set(20,0,0);
		axis3.m_dir.set(0,10,0);
	}

	// angles
	double start = PI;
	double value = 0.5*PI;

	CDbAcisEntityData obj;
	assertex(modeler()->createSurfaceRevolved(profile, axis1, start, value, obj),
		errorMessage().c_str());
	assertex(modeler()->createSurfaceRevolved(profile, axis2, start, value, obj),
		errorMessage().c_str());
	assertex(modeler()->createSurfaceRevolved(profile, axis3, start, value, obj),
		errorMessage().c_str());

	// zero angle involves 2PI sweeping
	value = 0;
	assertex(modeler()->createSurfaceRevolved(profile, axis1, start, value, obj),
		errorMessage().c_str());
}

void CTsSurfaces::surfaceRevolvedCrashing()
{
	// polyline profile
	gd::pline profile;
	{
		icl::transf rot = icl::rotation(0.5*PI, point(0,0,0), gvector(0,0,1));
		point p(-10,-10,0);

		const int npts = 4;
		for (int i = 0; i < npts; i++, p*=rot)
		{
			profile.m_pts.push_back(p);
			profile.m_bulges.push_back(0.);
		}

		profile.m_normal.set(0,0,1);
		profile.m_flags = 0;
	}

	// line axis
	gd::line axis1;
	{
		axis1.m_start.set(20,0,0);
		axis1.m_end.set(20,10,0);
	}
	gd::pline axis2;
	{
		axis2.m_pts.push_back(point(20,0,0));
		axis2.m_pts.push_back(point(20,10,0));
	}
	gd::ray axis3;
	{
		axis3.m_root.set(20,0,0);
		axis3.m_dir.set(0,10,0);
	}

	// angles
	double start = PI;
	double value = 0.5*PI;

	int i;
	g_crasher.startMonitoring();
	for (i = 0; !i || g_crasher.startNextSession(); i++)
	{
		INSTALL_FINALLY_CLEANER

		CDbAcisEntityData obj;
		int rc = modeler()->createSurfaceRevolved(profile, axis1, start, value, obj);

		if ((rc == 0 && i == 0) ||
			(rc != 0 && i != 0))
			throwex(errorMessage().c_str());
		g_crasher.endMonitoring();
	}

	g_crasher.startMonitoring();
	for (i = 0; !i || g_crasher.startNextSession(); i++)
	{
		INSTALL_FINALLY_CLEANER

		CDbAcisEntityData obj;
		int rc = modeler()->createSurfaceRevolved(profile, axis2, start, value, obj);

		if ((rc == 0 && i == 0) ||
			(rc != 0 && i != 0))
			throwex(errorMessage().c_str());
		g_crasher.endMonitoring();
	}

	g_crasher.startMonitoring();
	for (i = 0; !i || g_crasher.startNextSession(); i++)
	{
		INSTALL_FINALLY_CLEANER

		CDbAcisEntityData obj;
		int rc = modeler()->createSurfaceRevolved(profile, axis3, start, value, obj);

		if ((rc == 0 && i == 0) ||
			(rc != 0 && i != 0))
			throwex(errorMessage().c_str());
		g_crasher.endMonitoring();
	}
}

void CTsSurfaces::surfaceRevolvedIncorrectParameters()
{
	INSTALL_FINALLY_CLEANER

	// polyline profile
	gd::pline profile;
	{
		icl::transf rot = icl::rotation(0.5*PI, point(0,0,0), gvector(0,0,1));
		point p(-10,-10,0);

		const int npts = 4;
		for (int i = 0; i < npts; i++, p*=rot)
		{
			profile.m_pts.push_back(p);
			profile.m_bulges.push_back(0.);
		}

		profile.m_normal.set(0,0,1);
		profile.m_flags = 0;
	}

	// line axis
	gd::line axis;
	{
		axis.m_start.set(20,0,0);
		axis.m_end.set(20,10,0);
	}

	// angles
	double start = PI;
	double value = 0.5*PI;

	CDbAcisEntityData obj;
	// incorrect profile
	profile.m_flags = gd::pline::ePolygonMesh;
	assertex(!modeler()->createSurfaceRevolved(profile, axis, start, value, obj),
		"Incorrect profile");
	profile.m_flags = 0;
	
	// incorrect axis
	axis.m_start = axis.m_end;
	assertex(!modeler()->createSurfaceRevolved(profile, axis, start, value, obj),
		"Incorrect axis");
	axis.m_start = axis.m_end*icl::translation(gvector(0,1,0));
}

void CTsSurfaces::surfaceExtruded()
{
	INSTALL_FINALLY_CLEANER

	gd::ispline profile;
	{
		profile.m_fpts.push_back(point(-10, -10, 0));
		profile.m_fpts.push_back(point(-10, -5,  5));
		profile.m_fpts.push_back(point(-10,  0,  10));
		profile.m_fpts.push_back(point(-10,  5,  5));
		profile.m_fpts.push_back(point(-10,  10, 0));
		profile.m_start_tang.set(0,1,1);
		profile.m_end_tang.set(0,1,-1);
		profile.m_degree = 3;
		profile.m_fittol = 1.e-6;
		profile.m_normal.set(1,0,0);
		profile.m_flags = gd::spline::ePlanar;
	}

	gd::pline path;
	{
		path.m_pts.push_back(point(0,0,0));
		path.m_pts.push_back(point(10,0,0));
		path.m_pts.push_back(point(20,0,10));
		path.m_pts.push_back(point(30,0,10));
		path.m_normal.set(0,0,0);
		path.m_flags = gd::pline::e3dPolyline;
	}

	CDbAcisEntityData obj;
	assertex(modeler()->createSurfaceExtruded(profile, path, obj),
		errorMessage().c_str());
}

void CTsSurfaces::surfaceExtrudedCrashing()
{
	gd::ispline profile;
	{
		profile.m_fpts.push_back(point(-10, -10, 0));
		profile.m_fpts.push_back(point(-10, -5,  5));
		profile.m_fpts.push_back(point(-10,  0,  10));
		profile.m_fpts.push_back(point(-10,  5,  5));
		profile.m_fpts.push_back(point(-10,  10, 0));
		profile.m_start_tang.set(0,1,1);
		profile.m_end_tang.set(0,1,-1);
		profile.m_degree = 3;
		profile.m_fittol = 1.e-6;
		profile.m_normal.set(1,0,0);
		profile.m_flags = gd::spline::ePlanar;
	}

	gd::pline path;
	{
		path.m_pts.push_back(point(0,0,0));
		path.m_pts.push_back(point(10,0,0));
		path.m_pts.push_back(point(20,0,10));
		path.m_pts.push_back(point(30,0,10));
		path.m_normal.set(0,0,0);
		path.m_flags = gd::pline::e3dPolyline;
	}

	g_crasher.startMonitoring();
	for (int i = 0; !i || g_crasher.startNextSession(); i++)
	{
		INSTALL_FINALLY_CLEANER

		CDbAcisEntityData obj;
		int rc = modeler()->createSurfaceExtruded(profile, path, obj);
		if ((rc == 0 && i == 0) ||
			(rc != 0 && i != 0))
			throwex(errorMessage().c_str());
		g_crasher.endMonitoring();
	}
}

void CTsSurfaces::surfaceExtrudedIncorrectParameters()
{
	INSTALL_FINALLY_CLEANER

	gd::ispline profile;
	{
		profile.m_fpts.push_back(point(-10, -10, 0));
		profile.m_fpts.push_back(point(-10, -5,  5));
		profile.m_fpts.push_back(point(-10,  0,  10));
		profile.m_fpts.push_back(point(-10,  5,  5));
		profile.m_fpts.push_back(point(-10,  10, 0));
		profile.m_start_tang.set(0,0,1);
		profile.m_end_tang.set(0,0,-1);
		profile.m_degree = 3; //unnecessarily
		profile.m_fittol = 1.e-6; //unnecessarily
		profile.m_normal.set(1,0,0); //unnecessarily
		profile.m_flags = gd::spline::ePlanar; //unnecessarily
	}														

	gd::pline path;
	{
		path.m_pts.push_back(point(0,0,0));
		path.m_pts.push_back(point(10,0,0));
		path.m_pts.push_back(point(10,10,0));
		path.m_pts.push_back(point(10,10,10));
		path.m_normal.set(0,0,0);
		path.m_flags = gd::pline::e3dPolyline;
	}

	CDbAcisEntityData obj;

	// incorrect profile
	profile.m_start_tang.set(0,0,0);
	assertex(!modeler()->createSurfaceExtruded(profile, path, obj),
		"incorrect profile");
	profile.m_start_tang.set(0,0,1);

	// incorrect path
	path.m_pts.resize(1);
	assertex(!modeler()->createSurfaceExtruded(profile, path, obj),
		"incorrect path");
}

