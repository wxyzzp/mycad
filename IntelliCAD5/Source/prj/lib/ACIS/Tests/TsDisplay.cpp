#include "stdafx.h"
#include "../AsModeler.h"
#include "TsCrasher.h"
#include "TsTools.h"

#include "gvector.h"
#include "transf.h"
#include "xstd.h"
using namespace icl;

#include "GeoData.h"
namespace gd = geodata;

#include <vector>

#include "testunit/collection.h"
#include "testunit/assert.h"
#include "testunit/autoregistrar.h"
using namespace testunit;

class CTsDisplay: public collection
{
	void display();
	void displayIsolineCrash();
	void displayMeshCrash();

public:
	CTsDisplay();
};
static 
tauto_registrar<CTsDisplay> MAKE_NAME(base_collection());

CTsDisplay::CTsDisplay()
:collection("Test display functions")
{
	typedef TTsMethod<CTsDisplay> test_t;
	add(new test_t("display: ordinal test", this, display));
	add(new test_t("display isoline mode: crashing", this, displayIsolineCrash));
	add(new test_t("display mesh mode: crashing", this, displayMeshCrash));
}

void CTsDisplay::display()
{
	INSTALL_FINALLY_CLEANER
	//create model
	CDbAcisEntityData obj;
	{
		if (0 == modeler()->createBlock(point(0,0,0), point(10,10,10), icl::transf(), obj))
			throwex(errorMessage().c_str());
	}
	{
		//generate display objects, in isogd::line mode
		sds_dobjll* dispobj;
		if (0 == modeler()->display(obj, 4, 0.01, &dispobj))
			throwex(errorMessage().c_str());
		freeDispObjects(dispobj);
		//check cache usage
		if (0 == modeler()->display(obj, 4, 0.01, &dispobj))
			throwex(errorMessage().c_str());
		freeDispObjects(dispobj);
	}
	{
		//generate display objects in mesh mode
		sds_dobjll* dispobj;
		if (0 == modeler()->display(obj, 0, 0.01, &dispobj))
			throwex(errorMessage().c_str());
		freeDispObjects(dispobj);
	}

	{
		//try cone instead of block
		CDbAcisEntityData cone;
		if (0 == modeler()->createCone(point(0,0,0), 10, point(0,0,10), 0, icl::transf(), cone))
			throwex(errorMessage().c_str());

		sds_dobjll* dob;
		if (0 == modeler()->display(cone, 4, 0.01, &dob))
			throwex(errorMessage().c_str());
		freeDispObjects(dob);
	}

	{
		//try gd::ellipse based "wall"
		CDbAcisEntityData wall;
		{
			gd::ellipse e1;
			e1.m_center.set(0,0,0);
			e1.m_major_axis.set(1000,0,0);
			e1.m_normal.set(0,0,1);
			e1.m_ratio = 1;
			e1.m_start = 0;
			e1.m_end = PI;

			gd::ellipse e2;
			e2.m_center.set(0,0,0);
			e2.m_major_axis.set(999,0,0);
			e2.m_normal.set(0,0,1);
			e2.m_ratio = 1;
			e2.m_start = 0;
			e2.m_end = PI;

			gd::line l1;
			l1.m_start.set(999,0,0);
			l1.m_end.set(1000,0,0);

			gd::line l2;
			l2.m_start.set(-1000,0,0);
			l2.m_end.set(-999,0,0);

			std::vector<gd::curve*> crvs(4);
			crvs[0] = &e1; crvs[1] = &l2; crvs[2] = &e2; crvs[3] = &l1;

			xstd<CDbAcisEntityData>::vector objs;
			if (0 == modeler()->region(crvs, objs))
				throwex(errorMessage().c_str());

			gd::line path;
			path.m_start.set(0,0,0);
			path.m_end.set(0,0,10);

			if (0 == modeler()->extrude(objs[0], wall, path))
				throwex(errorMessage().c_str());

			sds_dobjll* dob;
			if (0 == modeler()->display(wall, 4, 0.0001, &dob))
				throwex(errorMessage().c_str());
			freeDispObjects(dob);
		}
	}
}

void CTsDisplay::displayIsolineCrash()
{
	//create model

	for (int i = 0;; i++)
	{
		INSTALL_FINALLY_CLEANER

		CDbAcisEntityData obj;
		if (0 == modeler()->createCone(point(0,0,0), 5, point(0,0,10), 0, icl::transf(), obj))
			throwex(errorMessage().c_str());

		if (i == 0)
			g_crasher.startMonitoring();
		else if (!g_crasher.startNextSession())
			break;

		//generate display objects, in isogd::line mode
		sds_dobjll* dispobj;
		int rc = modeler()->display(obj, 4, 0.01, &dispobj);
		if (i == 0 && rc == 0)
			throwex(errorMessage().c_str());
		else if (i != 0 && rc != 0)
			throwex(errorMessage().c_str());

		freeDispObjects(dispobj);

		crasher.endMonitoring();
	}
}

void CTsDisplay::displayMeshCrash()
{
	for (int i = 0;; i++)
	{
		INSTALL_FINALLY_CLEANER

		CDbAcisEntityData obj;
		if (0 == modeler()->createCone(point(0,0,0), 5, point(0,0,10), 0, icl::transf(), obj))
			throwex(errorMessage().c_str());

		if (i == 0)
			g_crasher.startMonitoring();
		else if (!g_crasher.startNextSession())
			break;

		//generate display objects in mesh mode
		sds_dobjll* dispobj;
		int rc = modeler()->display(obj, 0, 0.01, &dispobj);
		if (i == 0 && rc == 0)
			throwex(errorMessage().c_str());
		else if (i != 0 && rc != 0)
			throwex(errorMessage().c_str());

		freeDispObjects(dispobj);

		g_crasher.endMonitoring();
	}
}
