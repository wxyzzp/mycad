#include "stdafx.h"
#include "../AsModeler.h"
#include "TsCrasher.h"
#include "TsTools.h"

#include "gvector.h"
#include "transf.h"
using icl::point;
using icl::gvector;

#include "GeoData.h"
namespace gd = geodata;

#include <vector>

#include "testunit/collection.h"
#include "testunit/assert.h"
#include "testunit/autoregistrar.h"
using namespace testunit;

class CTsPrimitiveConstruction: public collection
{
	//tests
	void regionCrashTest();
	void region2();
	void region();
	void pyramidTop();
	void pyramidFacePlanarity();
	void pyramidPointCoincidence();
	void pyramidCrashTest();
	void pyramid2();
	void pyramid();
	void domeCrashTest();
	void domeRadius();
	void dome();
	void dishCrashTest();
	void dishRadius();
	void dish();
	void ellipticalCylinderCrashTest();
	void ellipticalCylinderHeight();
	void ellipticalCylinderAxis();
	void ellipticalCylinderRadius();
	void ellipticalCylinder();
	void cylinderCrashTest();
	void cylinderHeight();
	void cylinderRadius();
	void cylinder();
	void ellipticalConeCrashTest();
	void ellipticalConeHeight();
	void ellipticalConeAxis();
	void ellipticalConeRadius();
	void ellipticalCone();
	void coneCrashTest();
	void coneHeight();
	void coneRadius();
	void cone();
	void wedgeCrashTest();
	void wedgeBase();
	void wedgeHeight();
	void wedge();
	void torusCrashTest();
	void torusRadius();
	void torus();
	void sphereCrashTest();
	void sphereRadius();
	void sphere();
	void blockCrashTest();
	void blockDegeneratedBase();
	void blockZeroHeight();
	void block();

public:
	CTsPrimitiveConstruction();
};
static 
tauto_registrar<CTsPrimitiveConstruction> MAKE_NAME(base_collection());

CTsPrimitiveConstruction::CTsPrimitiveConstruction()
:collection("Test construction of solid primitives")
{
	typedef TTsMethod<CTsPrimitiveConstruction> test_t;
	add(new test_t("region: crashing", this, regionCrashTest));
	add(new test_t("region: another version", this, region2));
	add(new test_t("region", this, region));
	add(new test_t("pyramid: invalid top", this, pyramidTop));
	add(new test_t("pyramid: invalid face planarity", this, pyramidFacePlanarity));
	add(new test_t("pyramid: invalid point coincidence", this, pyramidPointCoincidence));
	add(new test_t("pyramid: crashing", this, pyramidCrashTest));
	add(new test_t("pyramid: another test", this, pyramid2));
	add(new test_t("pyramid", this, pyramid));
	add(new test_t("dome: crashing", this, domeCrashTest));
	add(new test_t("dome: invalid radius ", this, domeRadius));
	add(new test_t("dome", this, dome));
	add(new test_t("dish: crashing", this, dishCrashTest));
	add(new test_t("dish: invalid radius", this, dishRadius));
	add(new test_t("dish", this, dish));
	add(new test_t("elliptical cylinder: crashing", this, ellipticalCylinderCrashTest));
	add(new test_t("elliptical cylinder: invalid height", this, ellipticalCylinderHeight));
	add(new test_t("elliptical cylinder: invalid axis", this, ellipticalCylinderAxis));
	add(new test_t("elliptical cylinder: invalid radius", this, ellipticalCylinderRadius));
	add(new test_t("elliptical cylinder", this, ellipticalCylinder));
	add(new test_t("cylinder: crashing", this, cylinderCrashTest));
	add(new test_t("cylinder: invalid height", this, cylinderHeight));
	add(new test_t("cylinder: invalid radius", this, cylinderRadius));
	add(new test_t("cylinder", this, cylinder));
	add(new test_t("elliptical cone: crashing", this, ellipticalConeCrashTest));
	add(new test_t("elliptical cone: invalid height", this, ellipticalConeHeight));
	add(new test_t("elliptical cone: invalid axis", this, ellipticalConeAxis));
	add(new test_t("elliptical cone: invalid radius", this, ellipticalConeRadius));
	add(new test_t("elliptical cone", this, ellipticalCone));
	add(new test_t("cone: crashing", this, coneCrashTest));
	add(new test_t("cone: invalid height", this, coneHeight));
	add(new test_t("cone: invalid radius", this, coneRadius));
	add(new test_t("cone", this, cone));
	add(new test_t("wedge: crashing", this, wedgeCrashTest));
	add(new test_t("wedge: invalid base", this, wedgeBase));
	add(new test_t("wedge: invalid height", this, wedgeHeight));
	add(new test_t("wedge", this, wedge));
	add(new test_t("torus: crashing", this, torusCrashTest));
	add(new test_t("torus: invalid radius", this, torusRadius));
	add(new test_t("torus", this, torus));
	add(new test_t("sphere: crashing", this, sphereCrashTest));
	add(new test_t("sphere: invalid radius", this, sphereRadius));
	add(new test_t("sphere", this, sphere));
	add(new test_t("block: crashing", this, blockCrashTest));
	add(new test_t("block: invalid base", this, blockDegeneratedBase));
	add(new test_t("block: invalid height", this, blockZeroHeight));
	add(new test_t("block", this, block));
}

//Block creation testing
void CTsPrimitiveConstruction::block()
{
	INSTALL_FINALLY_CLEANER

	point p1(0,0,0);
	point p2(10,10,10);

	icl::transf tr = icl::translation(gvector(1,-1,1));

	CDbAcisEntityData obj;
	assertex(modeler()->createBlock(p1, p2, tr, obj),
		errorMessage().c_str());

	assertex(modeler()->createBlock(p2, p1, tr, obj),
		errorMessage().c_str());
}

void CTsPrimitiveConstruction::blockZeroHeight()
{
	INSTALL_FINALLY_CLEANER

	point p1(0,0,0);
	point p2(10,10,0);

	icl::transf tr = icl::translation(gvector(1,-1,1));
	CDbAcisEntityData obj;
	assertex(!modeler()->createBlock(p1, p2, tr, obj),
		"Attemption to create block with zero height.");
}

void CTsPrimitiveConstruction::blockDegeneratedBase()
{
	INSTALL_FINALLY_CLEANER

	point p1(0,0,0);

	icl::transf tr = icl::translation(gvector(1,-1,1));
	CDbAcisEntityData obj;
	assertex(!modeler()->createBlock(p1, p1, tr, obj),
		"Attemption to create block with degenerated base.");
}

void CTsPrimitiveConstruction::blockCrashTest()
{
	//CTsCrasher crasher;
	crasher.startMonitoring();

	for (int i = 0; !i || crasher.startNextSession(); i++)
	{
		INSTALL_FINALLY_CLEANER

		CDbAcisEntityData obj;
		int rc = modeler()->createBlock(point(0,0,0), point(10,10,10), icl::transf(), obj);
		if ((i == 0 && rc == 0) ||
			(i != 0 && rc != 0))
			throwex(errorMessage().c_str());
		crasher.endMonitoring();
	}
}

//Sphere creation testing
void CTsPrimitiveConstruction::sphere()
{
	INSTALL_FINALLY_CLEANER

	point c(1,1,1);
	double r = 1;

	icl::transf tr = icl::scaling(gvector(2,2,2));
	CDbAcisEntityData obj;
	assertex(modeler()->createSphere(c, r, tr, obj),
		errorMessage().c_str());
}

void CTsPrimitiveConstruction::sphereRadius()
{
	INSTALL_FINALLY_CLEANER

	point c(1,1,1);
	double r = 0;

	icl::transf tr = icl::scaling(gvector(2,2,2));
	CDbAcisEntityData obj;
	assertex(!modeler()->createSphere(c, r, tr, obj),
		"Attemption to create sphere with zero radius.");

	r = -10;
	assertex(!modeler()->createSphere(c, r, tr, obj),
		"Attemption to create sphere with negative radius.");
}

void CTsPrimitiveConstruction::sphereCrashTest()
{
	//CTsCrasher crasher;
	crasher.startMonitoring();

	for (int i = 0; !i || crasher.startNextSession(); i++)
	{
		INSTALL_FINALLY_CLEANER

		CDbAcisEntityData obj;
		int rc = modeler()->createSphere(point(0,0,0), 10, icl::transf(), obj);
		if ((i == 0 && rc == 0) ||
			(i != 0 && rc != 0))
			throwex(errorMessage().c_str());
		crasher.endMonitoring();
	}
}

//Torus creation testing
void CTsPrimitiveConstruction::torus()
{
	INSTALL_FINALLY_CLEANER

	point c(1,1,1);
	double major = 10;
	double minor = 5;
	CDbAcisEntityData obj;
	icl::transf tr = icl::rotation(2., gvector(1,1,1));
	{
		assertex(modeler()->createTorus(c, major, minor, tr, obj),
			errorMessage().c_str());
	}

	minor = major;
	{
		assertex(modeler()->createTorus(c, major, minor, tr, obj),
			errorMessage().c_str());
	}

	minor = 10*major;
	{
		assertex(modeler()->createTorus(c, major, minor, tr, obj),
			errorMessage().c_str());
	}
}

void CTsPrimitiveConstruction::torusRadius()
{
	INSTALL_FINALLY_CLEANER

	point c(1,1,1);
	icl::transf tr = icl::rotation(2., gvector(1,1,1));
	CDbAcisEntityData obj;
	
	double major = 0;
	double minor = 5;
	{
		assertex(!modeler()->createTorus(c, major, minor, tr, obj),
			"Attemption to create torus with zero external radius.");
	}

	major = 10;
	minor = 0;
	{
		assertex(!modeler()->createTorus(c, major, minor, tr, obj),
			"Attemption to create torus with zero internal radius.");
	}

	major = -10;
	minor = 5;
	{
		assertex(!modeler()->createTorus(c, major, minor, tr, obj),
			"Attemption to create torus with neagative external radius.");
	}

	major = 10;
	minor = -5;
	{
		assertex(!modeler()->createTorus(c, major, minor, tr, obj),
			"Attemption to create torus with neagative internal radius.");
	}
}

void CTsPrimitiveConstruction::torusCrashTest()
{
	//CTsCrasher crasher;
	crasher.startMonitoring();

	for (int i = 0; !i || crasher.startNextSession(); i++)
	{
		INSTALL_FINALLY_CLEANER

		CDbAcisEntityData obj;
		int rc = modeler()->createTorus(point(0,0,0), 20, 5, icl::transf(), obj);
		if ((i == 0 && rc == 0) ||
			(i != 0 && rc != 0))
			throwex(errorMessage().c_str());
		crasher.endMonitoring();
	}
}

//Wedge creation testing
void CTsPrimitiveConstruction::wedge()
{
	INSTALL_FINALLY_CLEANER

	point p1(0,0,0);
	point p2(10,10,0);
	icl::transf tr = icl::translation(gvector(1,0,0));
	CDbAcisEntityData obj;
	
	double height = 10;
	{
		assertex(modeler()->createWedge(p1, p2, height, tr, obj),
			errorMessage().c_str());
	}

	height = -10;
	{
		assertex(modeler()->createWedge(p1, p2, height, tr, obj),
			errorMessage().c_str());
	}
}

void CTsPrimitiveConstruction::wedgeHeight()
{
	INSTALL_FINALLY_CLEANER

	point p1(0,0,0);
	point p2(10,10,0);
	icl::transf tr = icl::translation(gvector(1,0,0));
	CDbAcisEntityData obj;

	double height = 0;
	{
		assertex(!modeler()->createWedge(p1, p2, height, tr, obj),
			"Attemption to create wedge with zero height.");
	}
}

void CTsPrimitiveConstruction::wedgeBase()
{
	INSTALL_FINALLY_CLEANER

	point p1(0,0,0);
	icl::transf tr = icl::translation(gvector(1,0,0));
	double height = 0;
	CDbAcisEntityData obj;
	{
		assertex(!modeler()->createWedge(p1, p1, height, tr, obj),
			"Attemption to create wedge with degenerated base.");
	}
}

void CTsPrimitiveConstruction::wedgeCrashTest()
{
	//CTsCrasher crasher;
	crasher.startMonitoring();

	for (int i = 0; !i || crasher.startNextSession(); i++)
	{
		INSTALL_FINALLY_CLEANER

		CDbAcisEntityData obj;
		int rc = modeler()->createWedge(point(0,0,0), point(10,10,0), 5, icl::transf(), obj);
		if ((i == 0 && rc == 0) ||
			(i != 0 && rc != 0))
			throwex(errorMessage().c_str());
		crasher.endMonitoring();
	}
}

//Test cone creation
void CTsPrimitiveConstruction::cone()
{
	INSTALL_FINALLY_CLEANER

	point bottom(0,0,0);
	point top(0,0,10);
	double bottomR = 10;
	double topR = 1;
	CDbAcisEntityData obj;
	icl::transf mtx = icl::translation(gvector(1,0,0));
	{
		assertex(modeler()->createCone(bottom, bottomR, top, topR, mtx, obj),
			errorMessage().c_str());
	}

	topR = 0;
	{
		assertex(modeler()->createCone(bottom, bottomR, top, topR, mtx, obj),
			errorMessage().c_str());
	}
}

void CTsPrimitiveConstruction::coneRadius()
{
	INSTALL_FINALLY_CLEANER

	point bottom(0,0,0);
	point top(0,0,10);
	icl::transf mtx = icl::translation(gvector(1,0,0));
	CDbAcisEntityData obj;

	double bottomR = 0;
	double topR = 10;
	{
		assertex(!modeler()->createCone(bottom, bottomR, top, topR, mtx, obj),
			"Attemption to create cone with zero radius of base.");
	}

	bottomR = 0;
	topR = 0;
	{
		assertex(!modeler()->createCone(bottom, bottomR, top, topR, mtx, obj),
			"Attemtion to create cone with zero radiuses.");
	}

	topR = 10;
	bottomR = -1;
	{
		assertex(!modeler()->createCone(bottom, bottomR, top, topR, mtx, obj),
			"Attemtion to create cone with negative bottom radius.");
	}

	bottomR = 10;
	topR = -1;
	{
		assertex(!modeler()->createCone(bottom, bottomR, top, topR, mtx, obj),
			"Attemtion to create cone with negative top radius.");
	}
}

void CTsPrimitiveConstruction::coneHeight()
{
	INSTALL_FINALLY_CLEANER

	point bottom(0,0,0);
	icl::transf mtx = icl::translation(gvector(1,0,0));
	CDbAcisEntityData obj;
	double bottomR = 10;
	double topR = 0;
	{
		assertex(!modeler()->createCone(bottom, bottomR, bottom, topR, mtx, obj),
			"Attemtion to create cone with zero height.");
	}
}

void CTsPrimitiveConstruction::coneCrashTest()
{
	//CTsCrasher crasher;
	crasher.startMonitoring();

	for (int i = 0; !i || crasher.startNextSession(); i++)
	{
		INSTALL_FINALLY_CLEANER

		CDbAcisEntityData obj;
		int rc = modeler()->createCone(point(0,0,0), 5, point(0,0,10), 0, icl::transf(), obj);
		if ((i == 0 && rc == 0) ||
			(i != 0 && rc != 0))
			throwex(errorMessage().c_str());
		crasher.endMonitoring();
	}
}

void CTsPrimitiveConstruction::ellipticalCone()
{
	INSTALL_FINALLY_CLEANER

	point bottom(0,0,0);
	point top(0,0,10);
	gvector axis(1,0,0);
	double ratio = 0.5;
	CDbAcisEntityData obj;
	icl::transf& mtx = icl::rotation(3, gvector(1,1,0));
	{
		assertex(modeler()->createCone(bottom, axis, ratio, top, mtx, obj),
			errorMessage().c_str());
	}

	// if axis is not perpendicular to the bottom plane, 
	// then it just projected on the plane
	axis.set(1,1,1);
	{
		assertex(modeler()->createCone(bottom, axis, ratio, top, mtx, obj),
			errorMessage().c_str());
	}
}

void CTsPrimitiveConstruction::ellipticalConeRadius()
{
	INSTALL_FINALLY_CLEANER

	point bottom(0,0,0);
	point top(0,0,10);
	gvector axis(1,0,0);
	icl::transf mtx = icl::translation(gvector(1,0,0));
	CDbAcisEntityData obj;

	double ratio = 0;
	{
		assertex(!modeler()->createCone(bottom, axis, ratio, top, mtx, obj),
			"Attemtion to create elliptical cone with zero radius.");
	}

	ratio = -2;
	{
		assertex(!modeler()->createCone(bottom, axis, ratio, top, mtx, obj),
			"Attemtion to create elliptical cone with negative radius.");
	}
}

void CTsPrimitiveConstruction::ellipticalConeAxis()
{
	INSTALL_FINALLY_CLEANER

	point bottom(0,0,0);
	point top(0,0,10);
	icl::transf mtx = icl::translation(gvector(1,0,0));
	double ratio = 2;
	CDbAcisEntityData obj;

	gvector axis(0,0,0);
	{
		assertex(!modeler()->createCone(bottom, axis, ratio, top, mtx, obj),
			"Attemtion to create elliptical cone with zero radius.");
	}
}

void CTsPrimitiveConstruction::ellipticalConeHeight()
{
	INSTALL_FINALLY_CLEANER

	point bottom(0,0,0);
	icl::transf mtx = icl::translation(gvector(1,0,0));
	CDbAcisEntityData obj;
	gvector axis(1,0,0);
	double ratio = 2;
	{
		assertex(!modeler()->createCone(bottom, axis, ratio, bottom, mtx, obj),
			"Attemtion to create elliptical cone with zero height.");
	}
}

void CTsPrimitiveConstruction::ellipticalConeCrashTest()
{
	//CTsCrasher crasher;
	crasher.startMonitoring();

	for (int i = 0; !i || crasher.startNextSession(); i++)
	{
		INSTALL_FINALLY_CLEANER

		CDbAcisEntityData obj;
		int rc = modeler()->createCone(point(0,0,0), gvector(1,0,0), 0.1, point(0,0,10), icl::transf(), obj);
		if ((i == 0 && rc == 0) ||
			(i != 0 && rc != 0))
			throwex(errorMessage().c_str());
		crasher.endMonitoring();
	}
}

// Cylinder
void CTsPrimitiveConstruction::cylinder()
{
	INSTALL_FINALLY_CLEANER

	point c(0,0,0);
	double radius = 10;
	point top(0,0,10);
	CDbAcisEntityData obj;
	icl::transf mtx = icl::translation(gvector(10,0,0));
	{
		assertex(modeler()->createCylinder(c, radius, top, mtx, obj),
			errorMessage().c_str());
	}
}

void CTsPrimitiveConstruction::cylinderRadius()
{
	INSTALL_FINALLY_CLEANER

	point c(0,0,0);
	point top(0,0,10);
	icl::transf mtx = icl::translation(gvector(10,0,0));
	CDbAcisEntityData obj;

	double radius = 0;
	{
		assertex(!modeler()->createCylinder(c, radius, top, mtx, obj),
			"Attemption to create cylinder with zero radius.");
	}

	radius = -10;
	{
		assertex(!modeler()->createCylinder(c, radius, top, mtx, obj),
			"Attemption to create cylinder with negative radius.");
	}
}

void CTsPrimitiveConstruction::cylinderHeight()
{
	INSTALL_FINALLY_CLEANER

	point c(0,0,0);
	icl::transf mtx = icl::translation(gvector(10,0,0));
	CDbAcisEntityData obj;
	double radius = 10;
	{
		assertex(!modeler()->createCylinder(c, radius, c, mtx, obj),
			"Attemption to create cylinder with zero height.");
	}
}

void CTsPrimitiveConstruction::cylinderCrashTest()
{
	//CTsCrasher crasher;
	crasher.startMonitoring();

	for (int i = 0; !i || crasher.startNextSession(); i++)
	{
		INSTALL_FINALLY_CLEANER

		CDbAcisEntityData obj;
		int rc = modeler()->createCylinder(point(0,0,0), 5, point(0,0,10), icl::transf(), obj);
		if ((i == 0 && rc == 0) ||
			(i != 0 && rc != 0))
			throwex(errorMessage().c_str());
		crasher.endMonitoring();
	}
}

void CTsPrimitiveConstruction::ellipticalCylinder()
{
	INSTALL_FINALLY_CLEANER

	point c(0,0,0);
	gvector a(10,0,0);
	double ratio = 3; 
	point top(0,0,20);
	CDbAcisEntityData obj;
	icl::transf mtx = icl::translation(gvector(1,0,0));
	{
		assertex(modeler()->createCylinder(c, a, ratio, top, mtx, obj),
			errorMessage().c_str());
	}
	
	// if axis is not perpendicular to the bottom plane, 
	// then it just projected on the plane
	a.set(1,1,1);
	{
		assertex(modeler()->createCylinder(c, a, ratio, top, mtx, obj),
			"not perpendicular axis for creation of elliptical cylinder should be projected on the bottom plane.");
	}

}

void CTsPrimitiveConstruction::ellipticalCylinderRadius()
{
	INSTALL_FINALLY_CLEANER

	point bottom(0,0,0);
	point top(0,0,10);
	gvector axis(1,0,0);
	icl::transf mtx = icl::translation(gvector(1,0,0));
	CDbAcisEntityData obj;

	double ratio = 0;
	{
		assertex(!modeler()->createCylinder(bottom, axis, ratio, top, mtx, obj),
			"Attemtion to create elliptical cylinder with zero radius.");
	}

	ratio = -2;
	{
		assertex(!modeler()->createCylinder(bottom, axis, ratio, top, mtx, obj),
			"Attemtion to create elliptical cylinder with negative radius.");
	}
}

void CTsPrimitiveConstruction::ellipticalCylinderAxis()
{
	INSTALL_FINALLY_CLEANER

	point bottom(0,0,0);
	point top(0,0,10);
	icl::transf mtx = icl::translation(gvector(1,0,0));
	CDbAcisEntityData obj;
	double ratio = 2;

	gvector axis(0,0,0);
	{
		assertex(!modeler()->createCylinder(bottom, axis, ratio, top, mtx, obj),
			"Attemtion to create elliptical cylinder with zero radius.");
	}
}

void CTsPrimitiveConstruction::ellipticalCylinderHeight()
{
	INSTALL_FINALLY_CLEANER

	point bottom(0,0,0);
	icl::transf mtx = icl::translation(gvector(1,0,0));
	CDbAcisEntityData obj;
	gvector axis(1,0,0);
	double ratio = 2;
	{
		assertex(!modeler()->createCylinder(bottom, axis, ratio, bottom, mtx, obj),
			"Attemtion to create elliptical cylinder with zero height.");
	}
}

void CTsPrimitiveConstruction::ellipticalCylinderCrashTest()
{
	//CTsCrasher crasher;
	crasher.startMonitoring();

	for (int i = 0; !i || crasher.startNextSession(); i++)
	{
		INSTALL_FINALLY_CLEANER

		CDbAcisEntityData obj;
		int rc = modeler()->createCylinder(point(0,0,0), gvector(10,0,0), 0.5, point(0,0,20), icl::transf(), obj);
		if ((i == 0 && rc == 0) ||
			(i != 0 && rc != 0))
			throwex(errorMessage().c_str());
		crasher.endMonitoring();
	}


}

// Dish
void CTsPrimitiveConstruction::dish()
{
	INSTALL_FINALLY_CLEANER

	point c(0,0,0);
	double radius = 10;
	CDbAcisEntityData obj;
	icl::transf mtx = icl::translation(gvector(1,0,0));
	{
		assertex(modeler()->createDish(c, radius, mtx, obj),
			errorMessage().c_str());
	}
}

void CTsPrimitiveConstruction::dishRadius()
{
	INSTALL_FINALLY_CLEANER

	point c(0,0,0);
	icl::transf mtx = icl::translation(gvector(1,0,0));
	CDbAcisEntityData obj;

	double radius = 0;
	{
		assertex(!modeler()->createDish(c, radius, mtx, obj),
			"Atemption to create dish with zero radius.");
	}
	radius = -10;
	{
		assertex(!modeler()->createDish(c, radius, mtx, obj),
			"Atemption to create dish with negative radius.");
	}
}

void CTsPrimitiveConstruction::dishCrashTest()
{
	//CTsCrasher crasher;
	crasher.startMonitoring();

	for (int i = 0; !i || crasher.startNextSession(); i++)
	{
		INSTALL_FINALLY_CLEANER

		CDbAcisEntityData obj;
		int rc = modeler()->createDish(point(0,0,0), 10, icl::transf(), obj);
		if ((i == 0 && rc == 0) ||
			(i != 0 && rc != 0))
			throwex(errorMessage().c_str());
		crasher.endMonitoring();
	}
}

// Dome
void CTsPrimitiveConstruction::dome()
{
	INSTALL_FINALLY_CLEANER

	point c(0,0,0);
	double radius = 10;
	icl::transf mtx = icl::translation(gvector(1,0,0));
	CDbAcisEntityData obj;
	{
		assertex(modeler()->createDome(c, radius, mtx, obj),
			errorMessage().c_str());
	}
}

void CTsPrimitiveConstruction::domeRadius()
{
	INSTALL_FINALLY_CLEANER

	point c(0,0,0);
	icl::transf mtx = icl::translation(gvector(1,0,0));
	CDbAcisEntityData obj;

	double radius = 0;
	{
		assertex(!modeler()->createDome(c, radius, mtx, obj),
			"Atemption to create dome with zero radius.");
	}
	radius = -10;
	{
		assertex(!modeler()->createDome(c, radius, mtx, obj),
			"Atemption to create dome with negative radius.");
	}
}

void CTsPrimitiveConstruction::domeCrashTest()
{
	//CTsCrasher crasher;
	crasher.startMonitoring();

	for (int i = 0; !i || crasher.startNextSession(); i++)
	{
		INSTALL_FINALLY_CLEANER

		CDbAcisEntityData obj;
		int rc = modeler()->createDome(point(0,0,0), 10, icl::transf(), obj);
		if ((i == 0 && rc == 0) ||
			(i != 0 && rc != 0))
			throwex(errorMessage().c_str());
		crasher.endMonitoring();
	}
}

// Pyramid
void CTsPrimitiveConstruction::pyramid()
{
	INSTALL_FINALLY_CLEANER

	point bp1(0,0,0);
	point bp2(10,0,0);
	point bp3(10,10,0);
	point bp4(0,10,0);
	point tp1(1,1,10);
	point tp2(9,1,10);
	point tp3(9,9,10);
	point tp4(1,9,10);
	icl::transf mtx = icl::translation(gvector(1,0,0));
	CDbAcisEntityData obj;

	//0 - all points are used
	//1 - used all bottom points and 2 top points
	//2 - tetrahedron
	//3 - prism
	for (int mode = 0; mode < 4; mode++)
	{
		assertex(modeler()->createPyramid(bp1,bp2,bp3,bp4,tp1,tp2,tp3,tp4,mode,mtx, obj),
			errorMessage().c_str());
	}
	//special 1 mode
	tp2 = tp1;
	assertex(modeler()->createPyramid(bp1,bp2,bp3,bp4,tp1,tp2,tp3,tp4,1,mtx, obj),
		errorMessage().c_str());
}

void CTsPrimitiveConstruction::pyramid2()
{
	INSTALL_FINALLY_CLEANER

	point bp1(0,0,0);
	point bp2(0,10,0);
	point bp3(10,10,0);
	point bp4(10,0,0);
	point tp1(1,1,10);
	point tp2(1,9,10);
	point tp3(9,9,10);
	point tp4(9,1,10);
	icl::transf mtx = icl::translation(gvector(1,0,0));
	CDbAcisEntityData obj;

	//0 - all points are used
	//1 - used all bottom points and 2 top points
	//2 - tetrahedron
	//3 - prism
	for (int mode = 0; mode < 4; mode++)
	{
		assertex(modeler()->createPyramid(bp1,bp2,bp3,bp4,tp1,tp2,tp3,tp4,mode,mtx, obj),
			errorMessage().c_str());
	}
	//special 1 mode
	tp2 = tp1;
	assertex(modeler()->createPyramid(bp1,bp2,bp3,bp4,tp1,tp2,tp3,tp4,1,mtx, obj),
		errorMessage().c_str());
}

void CTsPrimitiveConstruction::pyramidCrashTest()
{
	point bp1(0,0,0);
	point bp2(10,0,0);
	point bp3(10,10,0);
	point bp4(0,10,0);
	point tp1(1,1,10);
	point tp2(9,1,10);
	point tp3(9,9,10);
	point tp4(1,9,10);

	for (int mode = 0; mode < 4; mode++)
	{
		//CTsCrasher crasher;
		crasher.startMonitoring();

		for (int i = 0; !i || crasher.startNextSession(); i++)
		{
			INSTALL_FINALLY_CLEANER

			CDbAcisEntityData obj;
			int rc = modeler()->createPyramid(bp1,bp2,bp3,bp4,tp1,tp2,tp3,tp4,mode,icl::transf(),obj);
			if ((i == 0 && rc == 0) ||
				(i != 0 && rc != 0))
				throwex(errorMessage().c_str());
			crasher.endMonitoring();
		}
	}
}

void CTsPrimitiveConstruction::pyramidPointCoincidence()
{
	
	INSTALL_FINALLY_CLEANER

	//test point coincidence
	point bp1(0,0,0);
	point bp2(0,0,0);
	point bp3(10,10,0);
	point bp4(0,10,0);
	point tp1(1,1,10);
	point tp2(1,1,10);
	point tp3(9,9,10);
	point tp4(1,9,10);
	icl::transf mtx = icl::translation(gvector(1,0,0));
	CDbAcisEntityData obj;

	//0 - all points are used
	//1 - used all bottom points and 2 top points
	//2 - tetrahedron
	//3 - prism
	for (int mode = 0; mode < 4; mode++)
	{
		assertex(!modeler()->createPyramid(bp1,bp2,bp3,bp4,tp1,tp2,tp3,tp4,mode,mtx, obj),
			"Attemption to create pyramid with degenerated base and top");
	}
}

void CTsPrimitiveConstruction::pyramidFacePlanarity()
{
	
	INSTALL_FINALLY_CLEANER

	//test point coincidence
	point bp1(0,0,0);
	point bp2(10,0,0);
	point bp3(10,10,0);
	point bp4(0,10,0);
	point tp1(1,1,10);
	point tp2(9,2,10);
	point tp3(9,9,10);
	point tp4(1,8,10);
	icl::transf mtx = icl::translation(gvector(1,0,0));
	CDbAcisEntityData obj;

	//0 - all points are used
	//1 - used all bottom points and 2 top points
	//2 - tetrahedron
	//3 - prism
	for (int mode = 0; mode < 4; mode++)
	{
		if (mode == 2)
			continue;

		assertex(!modeler()->createPyramid(bp1,bp2,bp3,bp4,tp1,tp2,tp3,tp4,mode,mtx, obj),
			"Attemption to create pyramid with non-planar faces");
	}
}

void CTsPrimitiveConstruction::pyramidTop()
{
	
	INSTALL_FINALLY_CLEANER

	//test point coincidence
	point bp1(0,0,0);
	point bp2(10,0,0);
	point bp3(10,10,0);
	point bp4(0,10,0);
	point tp1(1,1,10);
	point tp2(9,1,-10);
	point tp3(9,9,10);
	point tp4(1,9,-10);
	icl::transf mtx = icl::translation(gvector(1,0,0));
	CDbAcisEntityData obj;

	//0 - all points are used
	//1 - used all bottom points and 2 top points
	//2 - tetrahedron
	//3 - prism
	for (int mode = 0; mode < 4; mode++)
	{
		if (mode == 2)
			continue;

		assertex(!modeler()->createPyramid(bp1,bp2,bp3,bp4,tp1,tp2,tp3,tp4,mode,mtx, obj),
			"Attemption to create pyramid with inproper top");
	}
}

void CTsPrimitiveConstruction::region()
{
	
	INSTALL_FINALLY_CLEANER

	gd::pline curve1;
	{
		curve1.m_pts.push_back(point(0,0,0));
		curve1.m_pts.push_back(point(10,0,0));
		curve1.m_pts.push_back(point(10,10,0));
		curve1.m_pts.push_back(point(0,10,0));
		curve1.m_normal.set(0,0,0);
		curve1.m_flags = gd::pline::e3dPolyline;
	}
	gd::line curve2;
	{
		curve2.m_start.set(0,0,0);
		curve2.m_end.set(0,10,0);
	}
	std::vector<gd::curve*> curves(2);
	curves[0] = &curve1;
	curves[1] = &curve2;

	xstd<CDbAcisEntityData>::vector regions;
	{
		assertex(modeler()->region(curves, regions),
			errorMessage().c_str());
	}
}

void CTsPrimitiveConstruction::region2()
{
	gd::aspline spl;
	{
		const int ncpts = 9;
		const int nknots = 12;
		const int degree = nknots - ncpts - 1;
		
		point cpts[ncpts] = {
			point( 20,   0, 0),
			point( 20,  20, 0),
			point(  0,  20, 0),
			point(-20,  20, 0),
			point(-20,   0, 0),
			point(-20, -20, 0),
			point(  0, -20, 0),
			point( 20, -20, 0),
			point( 20,   0, 0)};
		spl.m_cpts.insert(spl.m_cpts.end(), cpts, cpts+ncpts);

		double knots[12] = {0.,0.,0.,0.25,0.25,0.5,0.5,0.75,0.75,1.,1.,1.};
		spl.m_knots.insert(spl.m_knots.end(), knots, knots+nknots);

		const double sq2 = 0.70710678118654752440084436210485;
		double weights[9] = {1.,sq2,1.,sq2,1.,sq2,1.,sq2,1.};
		spl.m_weights.insert(spl.m_weights.begin(), weights, weights+ncpts);

		spl.m_degree = degree;
		spl.m_normal.set(0,0,1);
		spl.m_flags = gd::spline::eClosed|gd::spline::eRational|gd::spline::ePlanar;
		spl.m_knottol = icl::resabs();
		spl.m_cpttol = icl::resabs();
	}
	std::vector<gd::curve*> crvs(1);
	crvs[0] = &spl;

	xstd<CDbAcisEntityData>::vector objs;
	assertex(modeler()->region(crvs, objs),
		errorMessage().c_str());
}

void CTsPrimitiveConstruction::regionCrashTest()
{
	gd::pline curve1;
	{
		curve1.m_pts.push_back(point(0,0,0));
		curve1.m_pts.push_back(point(10,0,0));
		curve1.m_pts.push_back(point(10,10,0));
		curve1.m_pts.push_back(point(0,10,0));
		curve1.m_normal.set(0,0,0);
		curve1.m_flags = gd::pline::e3dPolyline;
	}
	gd::line curve2;
	{
		curve2.m_start.set(0,0,0);
		curve2.m_end.set(0,10,0);
	}
	std::vector<gd::curve*> curves(2);
	curves[0] = &curve1;
	curves[1] = &curve2;

	//CTsCrasher crasher;
	crasher.startMonitoring();

	for (int i = 0; !i || crasher.startNextSession(); i++)
	{
		INSTALL_FINALLY_CLEANER

		xstd<CDbAcisEntityData>::vector regions;
		int rc = modeler()->region(curves, regions);

		if ((i == 0 && rc == 0) || (i != 0 && rc != 0))
			throwex(errorMessage().c_str());
		
		crasher.endMonitoring();
	}
}
