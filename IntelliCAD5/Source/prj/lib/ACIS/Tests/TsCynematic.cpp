#include "stdafx.h"
#include "../AsModeler.h"
#include "TsCrasher.h"
#include "TsTools.h"

#include "xstd.h"
#include "gvector.h"
#include "transf.h"
using namespace icl;

#include "GeoData.h"
namespace gd = geodata;

#include <vector>

#include "testunit/collection.h"
#include "testunit/assert.h"
#include "testunit/autoregistrar.h"
using namespace testunit;

class CTsCynematic: public collection
{
	//tests
	void extrudeOneFace();
	void extrudeOneFaceCrash();
	void extrudeAlongPath();
	void extrudeAlongPathCrash();
	void revolve();
	void revolveCrash();
public:
	CTsCynematic();
};
static 
tauto_registrar<CTsCynematic> MAKE_NAME(base_collection());

CTsCynematic::CTsCynematic()
:collection("Test extruding and revolving")
{
	typedef TTsMethod<CTsCynematic> test_t;
	add(new test_t("extrude one face", this, extrudeOneFace));
	add(new test_t("extrude one face: crashing", this, extrudeOneFaceCrash));
	add(new test_t("extrude along path", this, extrudeAlongPath));
	add(new test_t("extrude along path: crashing", this, extrudeAlongPathCrash));
	add(new test_t("revolve", this, revolve));
	add(new test_t("revolve: crashing", this, revolveCrash));
}

void CTsCynematic::extrudeOneFace()
{
	INSTALL_FINALLY_CLEANER

	xstd<CDbAcisEntityData>::vector rgn;
	{
		gd::ellipse e;
		e.m_center.set(0,0,0);
		e.m_major_axis.set(10,0,0);
		e.m_normal.set(0,0,1);
		e.m_start = 0;
		e.m_end = 2*PI;
		e.m_ratio = 0.5;

		std::vector<gd::curve*> crvs(1);
		crvs[0] = &e;

		if (0 == modeler()->region(crvs, rgn))
			throwex(errorMessage().c_str());
	}
	CDbAcisEntityData obj;
	{
		double dist = 10;
		double taper = PI/30;
		if (0 == modeler()->extrude(rgn[0], obj, dist, taper))
			throwex(errorMessage().c_str());
	}
}

void CTsCynematic::extrudeOneFaceCrash()
{
	xstd<CDbAcisEntityData>::vector rgn;
	{
		gd::ellipse e;
		e.m_center.set(0,0,0);
		e.m_major_axis.set(10,0,0);
		e.m_normal.set(0,0,1);
		e.m_start = 0;
		e.m_end = 2*PI;
		e.m_ratio = 0.5;

		std::vector<gd::curve*> crvs(1);
		crvs[0] = &e;

		if (0 == modeler()->region(crvs, rgn))
			throwex(errorMessage().c_str());
	}

	//CTsCrasher crasher;
	crasher.startMonitoring();

	double dist = 10;
	double taper = PI/30;

	for (int i = 0; !i || crasher.startNextSession(); i++)
	{
		INSTALL_FINALLY_CLEANER
		
		CDbAcisEntityData reg(rgn[0]);
		CDbAcisEntityData obj;
		
		int rc = modeler()->extrude(reg, obj, dist, taper);

		if ((i == 0 && rc == 0) ||
			(i != 0 && rc != 0))
			throwex(errorMessage().c_str());
		
		crasher.endMonitoring();
	}
}

void CTsCynematic::extrudeAlongPath()
{
	INSTALL_FINALLY_CLEANER

	xstd<CDbAcisEntityData>::vector rgn;
	{
		gd::ellipse e;
		e.m_center.set(0,0,0);
		e.m_major_axis.set(10,0,0);
		e.m_normal.set(0,0,1);
		e.m_start = 0;
		e.m_end = 2*PI;
		e.m_ratio = 0.5;

		std::vector<gd::curve*> crvs(1);
		crvs[0] = &e;

		if (0 == modeler()->region(crvs, rgn))
			throwex(errorMessage().c_str());
	}
	gd::line path;
	{
		path.m_start.set(0,0,1);
		path.m_end.set(1,1,3);
	}

	CDbAcisEntityData obj;
	{
		if (0 == modeler()->extrude(rgn[0], obj, path))
			throwex(errorMessage().c_str());
	}
}

void CTsCynematic::extrudeAlongPathCrash()
{
	xstd<CDbAcisEntityData>::vector rgn;
	{
		gd::ellipse e;
		e.m_center.set(0,0,0);
		e.m_major_axis.set(10,0,0);
		e.m_normal.set(0,0,1);
		e.m_start = 0;
		e.m_end = 2*PI;
		e.m_ratio = 0.5;

		std::vector<gd::curve*> crvs(1);
		crvs[0] = &e;

		if (0 == modeler()->region(crvs, rgn))
			throwex(errorMessage().c_str());
	}
	gd::line path;
	{
		path.m_start.set(0,0,1);
		path.m_end.set(1,1,3);
	}

	//CTsCrasher crasher;
	crasher.startMonitoring();

	for (int i = 0; !i || crasher.startNextSession(); i++)
	{
		INSTALL_FINALLY_CLEANER

		CDbAcisEntityData reg(rgn[0]);
		CDbAcisEntityData obj;
		int rc = modeler()->extrude(reg, obj, path);

		if ((i == 0 && rc == 0) ||
			(i != 0 && rc != 0))
			throwex(errorMessage().c_str());

		crasher.endMonitoring();
	}
}

void CTsCynematic::revolve()
{
	INSTALL_FINALLY_CLEANER

	xstd<CDbAcisEntityData>::vector rgn;
	{
		gd::ellipse e;
		e.m_center.set(0,0,0);
		e.m_major_axis.set(10,0,0);
		e.m_normal.set(0,0,1);
		e.m_start = 0;
		e.m_end = 2*PI;
		e.m_ratio = 0.5;

		std::vector<gd::curve*> crvs(1);
		crvs[0] = &e;

		if (0 == modeler()->region(crvs, rgn))
			throwex(errorMessage().c_str());
	}
	CDbAcisEntityData obj;
	{
		point root(15,0,0);
		gvector axis(0,1,0);
		double angle = PI/2;
		if (0 == modeler()->revolve(rgn[0], obj, root, axis, angle))
			throwex(errorMessage().c_str());
	}
}

void CTsCynematic::revolveCrash()
{
	xstd<CDbAcisEntityData>::vector rgn;
	{
		gd::ellipse e;
		e.m_center.set(0,0,0);
		e.m_major_axis.set(10,0,0);
		e.m_normal.set(0,0,1);
		e.m_start = 0;
		e.m_end = 2*PI;
		e.m_ratio = 0.5;

		std::vector<gd::curve*> crvs(1);
		crvs[0] = &e;

		if (0 == modeler()->region(crvs, rgn))
			throwex(errorMessage().c_str());
	}

	//CTsCrasher crasher;
	crasher.startMonitoring();

	point root(15,0,0);
	gvector axis(0,1,0);
	double angle = PI/2;

	for (int i = 0; !i || crasher.startNextSession(); i++)
	{
		INSTALL_FINALLY_CLEANER

		CDbAcisEntityData reg(rgn[0]);
		CDbAcisEntityData obj;

		int rc = modeler()->revolve(reg, obj, root, axis, angle);

		if ((i == 0 && rc == 0) ||
			(i != 0 && rc != 0))
			throwex(errorMessage().c_str());

		crasher.endMonitoring();
	}
}
