#include "stdafx.h"
#include "../AsModeler.h"
#include "TsFunctional.h"
#include "TsTools.h"
#include "TsCrasher.h"

#include "xstd.h"
#include "gvector.h"
#include "transf.h"
using icl::point;
using icl::gvector;

#include "GeoData.h"
namespace gd = geodata;

#include <vector>
#include <algorithm>
using std::for_each;

#include "testunit/collection.h"
#include "testunit/assert.h"
#include "testunit/autoregistrar.h"
using namespace testunit;

class CTsQuery: public collection
{
	//tests
	void getArea();
	void getAreaCrash();
	void getPerimeter();
	void getPerimeterCrashing();
	void getBasePoint();
	void getBasePointCrashing();
	void getMeshRepresentation();
	void getMeshRepresentationCrashing();
	void isCurvesConnected();
	void isCurvesConnectedCrashing();
	void getNumberOfFaces();
	void getNumberOfFacesCrashing();
	void getSatInVersion();
	void getSatInVersionCrashing();
	void getSnapingPoints();
	void getSnapingPointsCrashing();
	void getGrips();
	void getGripsCrashing();
	void getBoundingBox();
	void getBoundingBoxCrashing();
	void copy();
	void free();
	void errorCode();

public:
	CTsQuery();
};
static 
tauto_registrar<CTsQuery> MAKE_NAME(base_collection());

CTsQuery::CTsQuery()
:collection("Test query methods")
{
	typedef TTsMethod<CTsQuery> test_t;
	add(new test_t("area calculation", this, getArea));
	add(new test_t("area calculation: crashing", this, getAreaCrash));
	add(new test_t("perimeter calculation", this, getPerimeter));
	add(new test_t("perimeter calculation: crashing", this, getPerimeterCrashing));
	add(new test_t("base point determination", this, getBasePoint));
	add(new test_t("base point determination: crashing", this, getBasePointCrashing));
	add(new test_t("mesh calculating", this, getMeshRepresentation));
	add(new test_t("mesh calculating: crashing", this, getMeshRepresentationCrashing));
	add(new test_t("check curve connectivity", this, isCurvesConnected));
	add(new test_t("check curve connectivity: crashing", this, isCurvesConnectedCrashing));
	add(new test_t("query namber of faces", this, getNumberOfFaces));
	add(new test_t("query namber of faces: crashing", this, getNumberOfFacesCrashing));
	add(new test_t("transforming sat-data in given version", this, getSatInVersion));
	add(new test_t("transforming sat-data in given version: crashing", this, getSatInVersionCrashing));
	add(new test_t("snaping points calculation", this, getSnapingPoints));
	add(new test_t("snaping points calculation: crashing", this, getSnapingPointsCrashing));
	add(new test_t("grips calculation", this, getGrips));
	add(new test_t("grips calculation: crashing", this, getGripsCrashing));
	add(new test_t("bounding box calculation", this, getBoundingBox));
	add(new test_t("bounding box calculation: crashion", this, getBoundingBoxCrashing));
	add(new test_t("copy", this, copy));
	add(new test_t("free memory allocated by modeler", this, free));
	add(new test_t("return last error code", this, errorCode));
}

void CTsQuery::getArea()
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

		assertex(modeler()->region(crvs, rgn),
			errorMessage().c_str());
	}
	{
		double area;
		assertex(modeler()->getArea(rgn[0], area),
			errorMessage().c_str());
	}
}

void CTsQuery::getAreaCrash()
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

		assertex(modeler()->region(crvs, rgn),
			errorMessage().c_str());
	}

	//CTsCrasher crasher;
	crasher.startMonitoring();

	for (int i = 0; !i || crasher.startNextSession(); i++)
	{
		double area;
		int rc = modeler()->getArea(rgn[0], area);
		if ((i == 0 && rc == 0) ||
			(i != 0 && rc != 0))
			throwex(errorMessage().c_str());

		crasher.endMonitoring();
	}
}

void CTsQuery::getPerimeter()
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

		assertex(modeler()->region(crvs, rgn),
			errorMessage().c_str());
	}
	{
		double perim;
		assertex(modeler()->getPerimeter(rgn[0], perim),
			errorMessage().c_str());
	}
}

void CTsQuery::getPerimeterCrashing()
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

		assertex(modeler()->region(crvs, rgn),
			errorMessage().c_str());
	}

	//CTsCrasher crasher;
	crasher.startMonitoring();

	for (int i = 0; !i || crasher.startNextSession(); i++)
	{
		double perim;
		int rc = modeler()->getPerimeter(rgn[0], perim);

		if ((i == 0 && rc == 0) ||
			(i != 0 && rc != 0))
			throwex(errorMessage().c_str());

		crasher.endMonitoring();
	}
}

void CTsQuery::getBasePoint()
{
	INSTALL_FINALLY_CLEANER

	CDbAcisEntityData obj;
	{
		assertex(modeler()->createBlock(point(0,0,0), point(10,10,10), icl::transf(), obj),
			errorMessage().c_str());
		
		sds_dobjll* dispobj = 0;
		assertex(modeler()->display(obj, 4, 0.001, &dispobj),
			errorMessage().c_str());
		
		freeDispObjects(dispobj);
	}

	point p;
	assertex(modeler()->getBasePoint(obj, p),
		errorMessage().c_str());
}

void CTsQuery::getBasePointCrashing()
{
	INSTALL_FINALLY_CLEANER

	CDbAcisEntityData obj;
	{
		assertex(modeler()->createBlock(point(0,0,0), point(10,10,10), icl::transf(), obj),
			errorMessage().c_str());

		sds_dobjll* dispobj = 0;
		assertex(modeler()->display(obj, 4, 0.001, &dispobj),
			errorMessage().c_str());
		
		freeDispObjects(dispobj);
	}

	//CTsCrasher crasher;
	crasher.startMonitoring();

	for (int i = 0; !i || crasher.startNextSession(); i++)
	{
		point p;
		int rc = modeler()->getBasePoint(obj, p);

		if ((i == 0 && rc == 0) ||
			(i != 0 && rc != 0))
			throwex(errorMessage().c_str());

		crasher.endMonitoring();
	}
}

void CTsQuery::getMeshRepresentation()
{
	INSTALL_FINALLY_CLEANER

	CDbAcisEntityData obj;
	{
		assertex(modeler()->createSphere(point(0,0,0), 10, icl::transf(), obj),
			errorMessage().c_str());
	}
	xstd<point>::vector pnts;
	xstd<int>::vector faces;
	assertex(modeler()->getMeshRepresentation(obj, pnts, faces),
		errorMessage().c_str());
}

void CTsQuery::getMeshRepresentationCrashing()
{
	INSTALL_FINALLY_CLEANER

	CDbAcisEntityData obj;
	{
		if (0 == modeler()->createSphere(point(0,0,0), 10, icl::transf(), obj))
			throwex(errorMessage().c_str());
	}

	//CTsCrasher crasher;
	crasher.startMonitoring();

	for (int i = 0; !i || crasher.startNextSession(); i++)
	{
		xstd<point>::vector pnts;
		xstd<int>::vector faces;
		int rc = modeler()->getMeshRepresentation(obj, pnts, faces);

		if ((i == 0 && rc == 0) ||
			(i != 0 && rc != 0))
			throwex(errorMessage().c_str());

		crasher.endMonitoring();
	}
}

void CTsQuery::isCurvesConnected()
{
	INSTALL_FINALLY_CLEANER

	gd::line l1;
	gd::line l2;
	bool is;

	{
		l1.m_start.set(0,0,0);
		l1.m_end.set(10,0,0);
		l2.m_start.set(10,0,0);
		l2.m_end.set(10,10,0);

		assertex(modeler()->isCurvesConnected(is, l1, l2),
			errorMessage().c_str());
		if (!is)
			throwex("curve's connection is defined incorrectly");

		assertex(modeler()->isCurvesConnected(is, l2, l1),
			errorMessage().c_str());
		if (!is)
			throwex("curve's connection is defined incorrectly");
	}
	{
		l1.m_start.set(10,0,0);
		l1.m_end.set(0,0,0);
		l2.m_start.set(10,0,0);
		l2.m_end.set(10,10,0);

		assertex(modeler()->isCurvesConnected(is, l1, l2),
			errorMessage().c_str());
		if (!is)
			throwex("curve's connection is defined incorrectly");
	}
	{
		l1.m_start.set(10,0,0);
		l1.m_end.set(0,0,0);
		l2.m_start.set(0,10,0);
		l2.m_end.set(0,0,0);

		assertex(modeler()->isCurvesConnected(is, l1, l2),
			errorMessage().c_str());
		if (!is)
			throwex("curve's connection is defined incorrectly");
	}
	{
		l1.m_start.set(10,0,0);
		l1.m_end.set(0,0,0);
		l2.m_start.set(0,10,0);
		l2.m_end.set(0,1,0);

		assertex(modeler()->isCurvesConnected(is, l1, l2),
			errorMessage().c_str());
		if (is)
			throwex("curve's connection is defined incorrectly");
	}
}

void CTsQuery::isCurvesConnectedCrashing()
{
	INSTALL_FINALLY_CLEANER

	gd::line l1;
	gd::line l2;
	bool is;
 	{
		l1.m_start.set(0,0,0);
		l1.m_end.set(10,0,0);
		l2.m_start.set(10,0,0);
		l2.m_end.set(10,10,0);
	}

	//CTsCrasher crasher;
	crasher.startMonitoring();

	for (int i = 0; !i || crasher.startNextSession(); i++)
	{
		int rc = modeler()->isCurvesConnected(is, l1, l2);

		if ((i == 0 && rc == 0) ||
			(i != 0 && rc != 0))
			throwex(errorMessage().c_str());

		crasher.endMonitoring();
	}
}

void CTsQuery::getNumberOfFaces()
{
	INSTALL_FINALLY_CLEANER

	CDbAcisEntityData obj;
	{
		assertex(modeler()->createSphere(point(0,0,0), 10, icl::transf(), obj),
			errorMessage().c_str());
	}

	int nfaces;
	assertex(modeler()->getNumberOfFaces(obj, nfaces),
		errorMessage().c_str());
}

void CTsQuery::getNumberOfFacesCrashing()
{
	INSTALL_FINALLY_CLEANER

	CDbAcisEntityData obj;
	{
		assertex(modeler()->createSphere(point(0,0,0), 10, icl::transf(), obj),
			errorMessage().c_str());
	}

	//CTsCrasher crasher;
	crasher.startMonitoring();

	for (int i = 0; !i || crasher.startNextSession(); i++)
	{
		int nfaces;
		int rc = modeler()->getNumberOfFaces(obj, nfaces);

		if ((i == 0 && rc == 0) ||
			(i != 0 && rc != 0))
			throwex(errorMessage().c_str());

		crasher.endMonitoring();
	}
}

void CTsQuery::getSatInVersion()
{
	INSTALL_FINALLY_CLEANER

	CDbAcisEntityData obj;
	{
		assertex(modeler()->createSphere(point(0,0,0), 10, icl::transf(), obj),
			errorMessage().c_str());
	}

	icl::string sat;
	int version = 106;//ACIS 2.3
	assertex(modeler()->getSatInVersion(obj, sat, version),
		errorMessage().c_str());
}

void CTsQuery::getSatInVersionCrashing()
{
	INSTALL_FINALLY_CLEANER

	CDbAcisEntityData obj;
	{
		assertex(modeler()->createSphere(point(0,0,0), 10, icl::transf(), obj),
			errorMessage().c_str());
	}

	//CTsCrasher crasher;
	crasher.startMonitoring();

	for (int i = 0; !i || crasher.startNextSession(); i++)
	{
		icl::string sat;
		int version = 106;//ACIS 2.3
		int rc = modeler()->getSatInVersion(obj, sat, version);

		if ((i == 0 && rc == 0) ||
			(i != 0 && rc != 0))
			throwex(errorMessage().c_str());

		crasher.endMonitoring();
	}
}

void CTsQuery::getSnapingPoints()
{
	INSTALL_FINALLY_CLEANER

	CDbAcisEntityData obj;
	{
		assertex(modeler()->createCone(point(0,0,0), 10, point(0,0,100), 0, icl::transf(), obj),
			errorMessage().c_str());
	}

	int osmode;
	point pick(50,0,50);
	point last;
	gvector dir(-1,0,-1);
	xstd<snaping>::vector pnts;
	
	{
		osmode = IC_OSMODE_END|IC_OSMODE_MID|IC_OSMODE_CENTER|IC_OSMODE_QUADRANT|IC_OSMODE_NEAR;
		assertex(modeler()->getSnapingPoints(obj, osmode, pick, last, dir, pnts),
			errorMessage().c_str());
	}
	{
		last.set(50,5,50);
		osmode = IC_OSMODE_PERP|IC_OSMODE_TAN;
		assertex(modeler()->getSnapingPoints(obj, osmode, pick, last, dir, pnts),
			errorMessage().c_str());
	}
}

void CTsQuery::getSnapingPointsCrashing()
{
	INSTALL_FINALLY_CLEANER

	CDbAcisEntityData obj;
	{
		assertex(modeler()->createCone(point(0,0,0), 10, point(0,0,100), 0, icl::transf(), obj),
			errorMessage().c_str());
	}

	int osmode;
	int i;
	point pick(50,0,50);
	point last;
	gvector dir(-1,0,-1);
	xstd<snaping>::vector pnts;

	//CTsCrasher crasher;
	crasher.startMonitoring();

	osmode = IC_OSMODE_END|IC_OSMODE_MID|IC_OSMODE_CENTER|IC_OSMODE_QUADRANT|IC_OSMODE_NEAR;

	for (i = 0; !i || crasher.startNextSession(); i++)
	{
		int rc = modeler()->getSnapingPoints(obj, osmode, pick, last, dir, pnts);
		if ((i == 0 && rc == 0) ||
			(i != 0 && rc != 0))
			throwex(errorMessage().c_str());

		crasher.endMonitoring();
	}

	crasher.startMonitoring();

	last.set(50,5,50);
	osmode = IC_OSMODE_PERP|IC_OSMODE_TAN;

	for (i = 0; !i || crasher.startNextSession(); i++)
	{
		int rc = modeler()->getSnapingPoints(obj, osmode, pick, last, dir, pnts);
		if ((i == 0 && rc == 0) ||
			(i != 0 && rc != 0))
			throwex(errorMessage().c_str());

		crasher.endMonitoring();
	}
}

void CTsQuery::getGrips()
{
	INSTALL_FINALLY_CLEANER

	CDbAcisEntityData obj;
	{
		assertex(modeler()->createBlock(point(0,0,0), point(10,10,10), icl::transf(), obj),
			errorMessage().c_str());
	}
	xstd<point>::vector grips;
	assertex(modeler()->getGrips(obj, grips),
		errorMessage().c_str());

	grips.clear();
	{
		assertex(modeler()->createCone(point(0,0,0), gvector(2,0,0), 0.5, point(0,0,10), icl::transf(), obj),
			errorMessage().c_str());
	}
	assertex(modeler()->getGrips(obj, grips),
		errorMessage().c_str());

	grips.clear();
	{
		assertex(modeler()->createSphere(point(0,0,0), 10, icl::transf(), obj),
			errorMessage().c_str());
	}
	assertex(modeler()->getGrips(obj, grips),
		errorMessage().c_str());
}

void CTsQuery::getGripsCrashing()
{
	INSTALL_FINALLY_CLEANER

	CDbAcisEntityData obj;
	{
		assertex(modeler()->createBlock(point(0,0,0), point(10,10,10), icl::transf(), obj),
			errorMessage().c_str());
	}

	//CTsCrasher crasher;
	crasher.startMonitoring();

	int i;
	for (i = 0; !i || crasher.startNextSession(); i++)
	{
		xstd<point>::vector grips;
		int rc = modeler()->getGrips(obj, grips);

		if ((i == 0 && rc == 0) ||
			(i != 0 && rc != 0))
			throwex(errorMessage().c_str());

		crasher.endMonitoring();
	}

	{
		assertex(modeler()->createCone(point(0,0,0), gvector(2,0,0), 0.5, point(0,0,10), icl::transf(), obj),
			errorMessage().c_str());
	}

	crasher.startMonitoring();
	for (i = 0; !i || crasher.startNextSession(); i++)
	{
		xstd<point>::vector grips;
		int rc = modeler()->getGrips(obj, grips);

		if ((i == 0 && rc == 0) ||
			(i != 0 && rc != 0))
			throwex(errorMessage().c_str());

		crasher.endMonitoring();
	}

	{
		assertex(modeler()->createSphere(point(0,0,0), 10, icl::transf(), obj),
			errorMessage().c_str());
	}

	crasher.startMonitoring();
	for (i = 0; !i || crasher.startNextSession(); i++)
	{
		xstd<point>::vector grips;
		int rc = modeler()->getGrips(obj, grips);

		if ((i == 0 && rc == 0) ||
			(i != 0 && rc != 0))
			throwex(errorMessage().c_str());

		crasher.endMonitoring();
	}
}

void CTsQuery::getBoundingBox()
{
	INSTALL_FINALLY_CLEANER

	{
		CDbAcisEntityData obj;
		icl::transf rot = icl::rotation(PI/4, gvector(0,0,1));
		{
			assertex(modeler()->createBlock(point(0,0,0), point(10,10,10), rot, obj),
				errorMessage().c_str());
		}
		
		point low, high;
		icl::transf rot1 = rot;
		rot1.inverse();
		assertex(modeler()->getBoundingBox(obj, low, high, rot1),
			errorMessage().c_str());
		assertex((low == point(0,0,0) && high == point(10,10,10)),
			"bounding box is determined incorrectly");
	}
	{
		CDbAcisEntityData obj;
		{
			assertex(modeler()->createBlock(point(0,0,0), point(10,10,10), icl::transf(), obj),
				errorMessage().c_str());
		}
		point low, high;
		assertex(modeler()->getBoundingBox(obj, low, high, icl::transf()),
			errorMessage().c_str());
		assertex(low == point(0,0,0) && high == point(10,10,10),
			"bounding box is determined incorrectly");
	}
}

void CTsQuery::getBoundingBoxCrashing()
{
	INSTALL_FINALLY_CLEANER

	CDbAcisEntityData obj;
	{
		assertex(modeler()->createBlock(point(0,0,0), point(10,10,10), icl::transf(), obj),
			errorMessage().c_str());
	}

	//CTsCrasher crasher;
	crasher.startMonitoring();

	for (int i = 0; !i || crasher.startNextSession(); i++)
	{
		point low, high;
		int rc = modeler()->getBoundingBox(obj, low, high, icl::transf());

		if ((i == 0 && rc == 0) ||
			(i != 0 && rc != 0))
			throwex(errorMessage().c_str());

		crasher.endMonitoring();
	}
}

void CTsQuery::copy()
{
	INSTALL_FINALLY_CLEANER

	CDbAcisEntityData obj1;
	assertex(modeler()->createBlock(point(0,0,0), point(10,10,10), icl::transf(), obj1),
		errorMessage().c_str());

	CDbAcisEntityData obj2;
	assertex(modeler()->copy(obj1, obj2),
		errorMessage().c_str());
}

void CTsQuery::free()
{
	INSTALL_FINALLY_CLEANER

	CDbAcisEntityData obj;
	assertex(modeler()->createBlock(point(0,0,0), point(10,10,10), icl::transf(), obj),
		errorMessage().c_str());

	xstd<gd::curve*>::vector crvs;
	assertex(modeler()->explodeIntoCurves(obj, crvs),
		errorMessage().c_str());

	for_each(crvs.begin(), crvs.end(), ptr_vmem1((CAsViewer*)modeler(),CAsViewer::free));
}

void CTsQuery::errorCode()
{
	int ec = modeler()->getLastErrorCode();
	icl::string mess;
	assertex(modeler()->getErrorMessage(ec, mess),
		errorMessage().c_str());
}
