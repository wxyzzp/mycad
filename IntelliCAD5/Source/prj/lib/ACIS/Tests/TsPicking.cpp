#include "stdafx.h"
#include "../AsModeler.h"
#include "TsCrasher.h"
#include "TsTools.h"
#include "TsFunctional.h"

#include "gvector.h"
#include "transf.h"
#include "xstd.h"
using icl::point;
using icl::gvector;

#include <vector>

#include "testunit/collection.h"
#include "testunit/assert.h"
#include "testunit/autoregistrar.h"
using namespace testunit;

class CTsPicking: public collection
{
	//tests
	void pickEdge();
	void pickEdgeCrash();
	void pickFace();
	void pickFaceCrash();
	void highlightUnhighlightEdge();
	void highlightUnhighlightFace();
	void getEdges();
	void getEdgesCrash();
	void getFaces();
	void getFacesCrash();

public:
	CTsPicking();
};
static 
tauto_registrar<CTsPicking> MAKE_NAME(base_collection());

CTsPicking::CTsPicking()
:collection("Test picking")
{
	typedef TTsMethod<CTsPicking> test_t;
	add(new test_t("pick edge", this, pickEdge));
	add(new test_t("pick edge: crashing", this, pickEdgeCrash));
	add(new test_t("pick face", this, pickFace));
	add(new test_t("pick face: crashing", this, pickFaceCrash));
	add(new test_t("highlight, unhighlight edge", this, highlightUnhighlightEdge));
	add(new test_t("highlight, unhighlight face", this, highlightUnhighlightFace));
	add(new test_t("get edges", this, getEdges));
	add(new test_t("get edges: crasing", this, getEdgesCrash));
	add(new test_t("get faces", this, getFaces));
	add(new test_t("get faces: crashing", this, getFacesCrash));
}

void CTsPicking::pickEdge()
{
	INSTALL_FINALLY_CLEANER

	CDbAcisEntityData obj;
	{
		assertex(modeler()->createBlock(point(0,0,0), point(10,10,10), icl::transf(), obj),
			errorMessage().c_str());
	}

	point root(5,5,-5);
	gvector dir(0,0,1);
	xstd<HEDGE>::vector hedges;

	typedef rmem0_t<CAsModeler,int> modeler_imem0_t;
	guard_t<modeler_imem0_t, modeler_imem0_t> locker(
		ptr_rmem0(modeler(), CAsModeler::lockDatabase),
		ptr_rmem0(modeler(), CAsModeler::unlockDatabase));

	assertex(modeler()->pickEdge(obj, root, dir, icl::resabs(), hedges),
		errorMessage().c_str());

	root.set(15,5,5);
	dir.set(-1,0,0);
	assertex(modeler()->pickEdge(obj, root, dir, icl::resabs(), hedges),
		errorMessage().c_str());
}

void CTsPicking::pickEdgeCrash()
{
	INSTALL_FINALLY_CLEANER

	CDbAcisEntityData obj;
	{
		assertex(modeler()->createBlock(point(0,0,0), point(10,10,10), icl::transf(), obj),
			errorMessage().c_str());
	}

	point root(5,5,-5);
	gvector dir(0,0,1);

	typedef rmem0_t<CAsModeler,int> modeler_imem0_t;
	guard_t<modeler_imem0_t, modeler_imem0_t> locker(
		ptr_rmem0(modeler(), CAsModeler::lockDatabase),
		ptr_rmem0(modeler(), CAsModeler::unlockDatabase));

	//CTsCrasher crasher;
	crasher.startMonitoring();

	for (int i = 0; !i || crasher.startNextSession(); i++)
	{
		xstd<HEDGE>::vector hedges;
		int rc = modeler()->pickEdge(obj, root, dir, icl::resabs(), hedges);

		if ((i == 0 && rc == 0) ||
			(i != 0 && rc != 0))
			throwex(errorMessage().c_str());

		crasher.endMonitoring();
	}
}

void CTsPicking::pickFace()
{
	INSTALL_FINALLY_CLEANER

	CDbAcisEntityData obj;
	{
		assertex(modeler()->createBlock(point(0,0,0), point(10,10,10), icl::transf(), obj),
			errorMessage().c_str());
	}

	typedef rmem0_t<CAsModeler,int> modeler_imem0_t;
	guard_t<modeler_imem0_t, modeler_imem0_t> locker(
		ptr_rmem0(modeler(), CAsModeler::lockDatabase),
		ptr_rmem0(modeler(), CAsModeler::unlockDatabase));

	point root(5,5,-5);
	gvector dir(0,0,1);
	xstd<HFACE>::vector hfaces;

	assertex(modeler()->pickFace(obj, root, dir, icl::resabs(), hfaces),
		errorMessage().c_str());

	root.set(15,5,5);
	dir.set(-1,0,0);
	assertex(modeler()->pickFace(obj, root, dir, icl::resabs(), hfaces),
		errorMessage().c_str());
}

void CTsPicking::pickFaceCrash()
{
	INSTALL_FINALLY_CLEANER

	CDbAcisEntityData obj;
	{
		assertex(modeler()->createBlock(point(0,0,0), point(10,10,10), icl::transf(), obj),
			errorMessage().c_str());
	}

	typedef rmem0_t<CAsModeler,int> modeler_imem0_t;
	guard_t<modeler_imem0_t, modeler_imem0_t> locker(
		ptr_rmem0(modeler(), CAsModeler::lockDatabase),
		ptr_rmem0(modeler(), CAsModeler::unlockDatabase));

	point root(5,5,-5);
	gvector dir(0,0,1);

	//CTsCrasher crasher;
	crasher.startMonitoring();

	for (int i = 0; !i || crasher.startNextSession(); i++)
	{
		xstd<HFACE>::vector hfaces;

		int rc = modeler()->pickFace(obj, root, dir, icl::resabs(), hfaces);
		if ((i == 0 && rc == 0) ||
			(i != 0 && rc != 0))
			throwex(errorMessage().c_str());

		crasher.endMonitoring();
	}
}

void CTsPicking::highlightUnhighlightEdge()
{
	INSTALL_FINALLY_CLEANER

	typedef rmem0_t<CAsModeler,int> modeler_imem0_t;
	guard_t<modeler_imem0_t, modeler_imem0_t> locker(
		ptr_rmem0(modeler(), CAsModeler::lockDatabase),
		ptr_rmem0(modeler(), CAsModeler::unlockDatabase));

	// pick edge
	xstd<HEDGE>::vector hedges;
	CDbAcisEntityData obj;
	{
		assertex(modeler()->createBlock(point(0,0,0), point(10,10,10), icl::transf(), obj),
			errorMessage().c_str());

		point root(15,5,5);
		gvector dir(-1,0,0);
		assertex(modeler()->pickEdge(obj, root, dir, icl::resabs(), hedges),
			errorMessage().c_str());
	}

	// highlight-unhighlight
	{
		sds_dobjll* dispobj;
		if (0 == modeler()->display(obj, 4, 0.01, &dispobj))
		{
			freeDispObjects(dispobj);
			throwex(errorMessage().c_str());
		}
		freeDispObjects(dispobj);

		for (int i = 0, n = hedges.size(); i < n; i++)
		{
			assertex(modeler()->highlight(obj, hedges[i]),
				errorMessage().c_str());
			assertex(modeler()->unhighlight(obj, hedges[i]),
				errorMessage().c_str());
		}
	}
}

void CTsPicking::highlightUnhighlightFace()
{
	INSTALL_FINALLY_CLEANER

	typedef rmem0_t<CAsModeler,int> modeler_imem0_t;
	guard_t<modeler_imem0_t, modeler_imem0_t> locker(
		ptr_rmem0(modeler(), CAsModeler::lockDatabase),
		ptr_rmem0(modeler(), CAsModeler::unlockDatabase));

	// pick face
	xstd<HFACE>::vector hfaces;
	CDbAcisEntityData obj;
	{
		assertex(modeler()->createBlock(point(0,0,0), point(10,10,10), icl::transf(), obj),
			errorMessage().c_str());

		point root(15,5,5);
		gvector dir(-1,0,0);
		assertex(modeler()->pickFace(obj, root, dir, icl::resabs(), hfaces),
			errorMessage().c_str());
	}

	// highlight-unhighlight
	{
		sds_dobjll* dispobj;
		if (0 == modeler()->display(obj, 4, 0.01, &dispobj))
		{
			freeDispObjects(dispobj);
			throwex(errorMessage().c_str());
		}
		freeDispObjects(dispobj);

		for (int i = 0, n = hfaces.size(); i < n; i++)
		{
			assertex(modeler()->highlight(obj, hfaces[i]),
				errorMessage().c_str());
			assertex(modeler()->unhighlight(obj, hfaces[i]),
				errorMessage().c_str());
		}
	}
}

void CTsPicking::getEdges()
{
	INSTALL_FINALLY_CLEANER

	xstd<HEDGE>::vector hedges;
	CDbAcisEntityData obj;
	{
		assertex(modeler()->createBlock(point(0,0,0), point(10,10,10), icl::transf(), obj),
			errorMessage().c_str());

		assertex(modeler()->getEdges(obj, hedges),
			errorMessage().c_str());
	}
}

void CTsPicking::getEdgesCrash()
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
		xstd<HEDGE>::vector hedges;
		int rc = modeler()->getEdges(obj, hedges);

		if ((i == 0 && rc == 0) ||
			(i != 0 && rc != 0))
			throwex(errorMessage().c_str());

		crasher.endMonitoring();
	}
}

void CTsPicking::getFaces()
{
	INSTALL_FINALLY_CLEANER

	xstd<HFACE>::vector hfaces;
	CDbAcisEntityData obj;
	{
		assertex(modeler()->createBlock(point(0,0,0), point(10,10,10), icl::transf(), obj),
			errorMessage().c_str());

		assertex(modeler()->getFaces(obj, hfaces),
			errorMessage().c_str());
	}
}

void CTsPicking::getFacesCrash()
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
		xstd<HFACE>::vector hfaces;

		int rc = modeler()->getFaces(obj, hfaces);

		if ((i == 0 && rc == 0) ||
			(i != 0 && rc != 0))
			throwex(errorMessage().c_str());

		crasher.endMonitoring();
	}
}
