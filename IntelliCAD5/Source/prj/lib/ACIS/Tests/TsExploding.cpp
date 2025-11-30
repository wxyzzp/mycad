#include "stdafx.h"
#include "../AsModeler.h"
#include "TsCrasher.h"
#include "TsFunctional.h"
#include "TsTools.h"

#include "GeoData.h"
namespace gd = geodata;

#include "gvector.h"
#include "transf.h"
#include "xstd.h"
using icl::point;
using icl::gvector;

#include <vector>
#include <memory>
using std::auto_ptr;
#include <algorithm>
using std::for_each;

#include "testunit/collection.h"
#include "testunit/assert.h"
#include "testunit/autoregistrar.h"
using namespace testunit;

class CTsExploding: public collection
{
	//tests
	void explodeIntoBodies();
	void explodeIntoBodiesCrash();
	void explodeIntoCurves();
	void explodeIntoCurves2();
	void explodeIntoCurveCrash();

	createRegion(xstd<CDbAcisEntityData>::vector&);

public:
	CTsExploding();
};
static 
tauto_registrar<CTsExploding> MAKE_NAME(base_collection());

CTsExploding::CTsExploding()
:collection("Test exploding")
{
	typedef TTsMethod<CTsExploding> test_t;
	add(new test_t("exploding into bodies", this, explodeIntoBodies));
	add(new test_t("exploding into bodies: crashing", this, explodeIntoBodiesCrash));
	add(new test_t("exploding into curvs", this, explodeIntoCurves));
	add(new test_t("exploding into gd::curves: another variant", this, explodeIntoCurves2));
	add(new test_t("exploding into gd::curves: crashing", this, explodeIntoCurveCrash));
}

int CTsExploding::createRegion
(
xstd<CDbAcisEntityData>::vector& rgn
)
{
	gd::ellipse arc;
	{
		arc.m_center.set(0,0,0);
		arc.m_major_axis.set(10,0,0);
		arc.m_normal.set(0,0,1);
		arc.m_start = 0;
		arc.m_end = PI*0.5;
		arc.m_ratio = 0.5;
	}
	gd::line str;
	{
		str.m_start.set(0,5,0);
		str.m_end.set(-10,0,0);
	}
	gd::ispline spl;
	{
		spl.m_fpts.push_back(point(-10, 0,  0));
		spl.m_fpts.push_back(point(-8, -5,  0));
		spl.m_fpts.push_back(point( 0, -10, 0));
		spl.m_fpts.push_back(point( 8, -5,  0));
		spl.m_fpts.push_back(point( 10, 0,  0));
		spl.m_start_tang.set(0,-1,0);
		spl.m_end_tang.set(0,1,0);
		spl.m_degree = 3;
		spl.m_fittol = 1.e-6;
		spl.m_normal.set(0,0,1);
		spl.m_flags = gd::spline::ePlanar;
	}

	std::vector<gd::curve*> crvs(3);
	crvs[0] = &arc;
	crvs[1] = &str;
	crvs[2] = &spl;

	return modeler()->region(crvs, rgn);
}

void CTsExploding::explodeIntoBodies()
{
	INSTALL_FINALLY_CLEANER

	xstd<CDbAcisEntityData>::vector obj;
	{
		std::vector<CDbAcisEntityData> blank(1);
		if (0 == modeler()->createBlock(point(-5,-5,0), point(5,5,10), icl::transf(), blank[0]))
			throwex(errorMessage().c_str());

		std::vector<CDbAcisEntityData> tool(1);
		if (0 == modeler()->createCylinder(point(0,0,0), 2, point(0,0,10), icl::transf(), tool[0]))
			throwex(errorMessage().c_str());

		if (0 == modeler()->subtract(blank, tool, obj))
			throwex(errorMessage().c_str());
	}

	xstd<CDbAcisEntityData>::vector faces;
	if (0 == modeler()->explodeIntoBodies(obj[0], faces))
		throwex(errorMessage().c_str());
}

void CTsExploding::explodeIntoBodiesCrash()
{
	xstd<CDbAcisEntityData>::vector objs;
	{
		std::vector<CDbAcisEntityData> blank(1);
		if (0 == modeler()->createBlock(point(-5,-5,0), point(5,5,10), icl::transf(), blank[0]))
			throwex(errorMessage().c_str());

		std::vector<CDbAcisEntityData> tool(1);
		if (0 == modeler()->createCylinder(point(0,0,0), 2, point(0,0,10), icl::transf(), tool[0]))
			throwex(errorMessage().c_str());

		if (0 == modeler()->subtract(blank, tool, objs))
			throwex(errorMessage().c_str());
	}

	//CTsCrasher crasher;
	crasher.startMonitoring();

	for (int i = 0; !i || crasher.startNextSession(); i++)
	{
		INSTALL_FINALLY_CLEANER

		CDbAcisEntityData obj(objs[0]);
		xstd<CDbAcisEntityData>::vector faces;

		int rc = modeler()->explodeIntoBodies(obj, faces);

		if ((i == 0 && rc == 0) ||
			(i != 0 && rc != 0))
			throwex(errorMessage().c_str());

		crasher.endMonitoring();
	}

}

void CTsExploding::explodeIntoCurves()
{
	INSTALL_FINALLY_CLEANER

	xstd<CDbAcisEntityData>::vector rgn;
	point center(0,0,0);
	gvector major(10,0,0);
	gvector normal(0,0,1);
	double start = 0;
	double end = PI*2;
	double ratio = 0.5;
	{
		gd::ellipse e;
		e.m_center = center;
		e.m_major_axis = major;
		e.m_normal = normal;
		e.m_start = start;
		e.m_end = end;
		e.m_ratio = ratio;

		std::vector<gd::curve*> crvs(1);
		crvs[0] = &e;

		if (0 == modeler()->region(crvs, rgn))
			throwex(errorMessage().c_str());
	}

	xstd<gd::curve*>::vector crvs;
	if (0 == modeler()->explodeIntoCurves(rgn[0], crvs))
		throwex(errorMessage().c_str());
	
	std::vector<auto_ptr<gd::curve> > hcrvs;
	{
		for (xstd<gd::curve*>::vector::iterator i = crvs.begin(); i != crvs.end(); ++i)
		{
			auto_ptr<gd::curve> hcrv(*i);
			hcrvs.push_back(hcrv);
		}
	}

	if (crvs.size() != 1)
		throwex(errorMessage().c_str());
	if (crvs[0]->type() != gd::eEllipse)
		throwex(errorMessage().c_str());

	gd::ellipse* e = static_cast<gd::ellipse*>(crvs[0]);
	if (e->m_center != center)
		throwex(errorMessage().c_str());
	if (e->m_major_axis != major)
		throwex(errorMessage().c_str());
	if (e->m_normal != normal)
		throwex(errorMessage().c_str());
	if (fabs(e->m_start-start) > resabs)
		throwex(errorMessage().c_str());
	if (fabs(e->m_end-end) > resabs)
		throwex(errorMessage().c_str());
	if (fabs(e->m_ratio-ratio) > resabs)
		throwex(errorMessage().c_str());
}

void CTsExploding::explodeIntoCurves2()
{
	INSTALL_FINALLY_CLEANER

	CDbAcisEntityData obj;

	icl::transf rot = icl::rotation(PI/2, point(0,0,5), gvector(0,1,0));
	point center(0,0,0);
	gvector major(10,0,0);
	gvector normal(0,0,1);
	double start = 0;
	double end = PI*2;
	double ratio = 0.5;
	{
		gd::ellipse e;
		e.m_center = center;
		e.m_major_axis = major;
		e.m_normal = normal;
		e.m_start = start;
		e.m_end = end;
		e.m_ratio = ratio;

		std::vector<gd::curve*> crvs(1);
		crvs[0] = &e;

		xstd<CDbAcisEntityData>::vector rgn;
		if (0 == modeler()->region(crvs, rgn))
			throwex(errorMessage().c_str());
		if (0 == modeler()->transform(rgn[0], obj, rot))
			throwex(errorMessage().c_str());
	}

	xstd<gd::curve*>::vector crvs;
	if (0 == modeler()->explodeIntoCurves(obj, crvs))
		throwex(errorMessage().c_str());
	
	std::vector<auto_ptr<gd::curve> > hcrvs;
	{
		for (xstd<gd::curve*>::vector::iterator i = crvs.begin(); i != crvs.end(); ++i)
		{
			auto_ptr<gd::curve> hcrv(*i);
			hcrvs.push_back(hcrv);
		}
	}

	if (crvs.size() != 1)
		throwex(errorMessage().c_str());
	if (crvs[0]->type() != gd::eEllipse)
		throwex(errorMessage().c_str());

	gd::ellipse* e = static_cast<gd::ellipse*>(crvs[0]);
	if (e->m_center != center*rot)
		throwex(errorMessage().c_str());
	if (e->m_major_axis != major*rot)
		throwex(errorMessage().c_str());
	if (e->m_normal != normal*rot)
		throwex(errorMessage().c_str());
	if (fabs(e->m_start-start) > resabs)
		throwex(errorMessage().c_str());
	if (fabs(e->m_end-end) > resabs)
		throwex(errorMessage().c_str());
	if (fabs(e->m_ratio-ratio) > resabs)
		throwex(errorMessage().c_str());
}

void CTsExploding::explodeIntoCurveCrash()
{
	xstd<CDbAcisEntityData>::vector rgn;
	if (!createRegion(rgn))
		throwex(errorMessage().c_str());
	modeler()->clear();

	//CTsCrasher crasher;
	crasher.startMonitoring();

	for (int i = 0; !i || crasher.startNextSession(); i++)
	{
		INSTALL_FINALLY_CLEANER
		
		CDbAcisEntityData reg(rgn[0]);
		xstd<gd::curve*>::vector crvs;
		
		int rc = modeler()->explodeIntoCurves(reg, crvs);

		for_each(crvs.begin(), crvs.end(), ptr_vfun1(::operator delete));

		if ((i == 0 && rc == 0) ||
			(i != 0 && rc != 0))
			throwex(errorMessage().c_str());

		crasher.endMonitoring();
	}
}
