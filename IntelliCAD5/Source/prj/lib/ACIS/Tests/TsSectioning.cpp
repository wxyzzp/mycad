#include "stdafx.h"
#include "TsTools.h"
#include "TsCrasher.h"

#include "gvector.h"
#include "transf.h"
using icl::point;
using icl::gvector;

#include "testunit/collection.h"
#include "testunit/assert.h"
#include "testunit/autoregistrar.h"
using namespace testunit;

class CTsSectioning: public collection
{
	//tests
	void section();
	void sectionCrashing();
	void sliceBothSide();
	void sliceBothSideCrashing();
	void sliceOneSide();
	void sliceOneSideCrashing();

public:
	CTsSectioning();
};

static 
tauto_registrar<CTsSectioning> MAKE_NAME(base_collection());

CTsSectioning::CTsSectioning()
:collection("Test CAsModeler::section(),slice()")
{
	typedef TTsMethod<CTsSectioning> test_t;
	add(new test_t("section", this, section));
	add(new test_t("section: crashing", this, sectionCrashing));
	add(new test_t("slice both side", this, sliceBothSide));
	add(new test_t("slice both side: crashing", this, sliceBothSideCrashing));
	add(new test_t("slice one side", this, sliceOneSide));
	add(new test_t("slice one side: crashing", this, sliceOneSideCrashing));
}

void CTsSectioning::section()
{
	INSTALL_FINALLY_CLEANER

	CDbAcisEntityData obj;
	{
		assertex(modeler()->createTorus(point(0,0,0), 20, 5, icl::transf(), obj),
			errorMessage().c_str());
	}
	CDbAcisEntityData region;
	{
		assertex(modeler()->section(obj, region, point(0,0,0), gvector(1,0,0)),
			errorMessage().c_str());
	}
}

void CTsSectioning::sectionCrashing()
{
	INSTALL_FINALLY_CLEANER

	CDbAcisEntityData obj;
	{
		assertex(modeler()->createTorus(point(0,0,0), 20, 5, icl::transf(), obj),
			errorMessage().c_str());
	}
	CDbAcisEntityData region;

	g_crasher.startMonitoring();
	for (int i = 0; !i || g_crasher.startNextSession(); i++)
	{
		int rc = modeler()->section(obj, region, point(0,0,0), gvector(1,0,0));
		if ((rc == 0 && i == 0) ||
			(rc != 0 && i != 0))
			throwex(errorMessage().c_str());

		g_crasher.endMonitoring();
	}
}

void CTsSectioning::sliceBothSide()
{
	INSTALL_FINALLY_CLEANER

	CDbAcisEntityData obj;
	{
		assertex(modeler()->createTorus(point(0,0,0), 20, 5, icl::transf(), obj),
			errorMessage().c_str());
	}
	CDbAcisEntityData parts[2];
	{
		assertex(modeler()->slice(obj, parts, point(0,0,0), gvector(0,0,1), 0),
			errorMessage().c_str());
	}
	{
		assertex(modeler()->slice(obj, parts, point(0,0,10), gvector(0,0,1), 0),
			errorMessage().c_str());
	}
}

void CTsSectioning::sliceBothSideCrashing()
{
	INSTALL_FINALLY_CLEANER

	CDbAcisEntityData obj;
	{
		assertex(modeler()->createTorus(point(0,0,0), 20, 5, icl::transf(), obj),
			errorMessage().c_str());
	}

	CDbAcisEntityData parts[2];
	g_crasher.startMonitoring();
	for (int i = 0; !i || g_crasher.startNextSession(); i++)
	{
		int rc = modeler()->slice(obj, parts, point(0,0,0), gvector(0,0,1), 0);
		if ((rc == 0 && i == 0) ||
			(rc != 0 && i != 0))
			throwex(errorMessage().c_str());

		g_crasher.endMonitoring();
	}
}

void CTsSectioning::sliceOneSide()
{
	INSTALL_FINALLY_CLEANER
	
	CDbAcisEntityData obj;
	{
		assertex(modeler()->createTorus(point(0,0,0), 20, 5, icl::transf(), obj),
			errorMessage().c_str());
	}
	CDbAcisEntityData parts[2];
	{
		assertex(modeler()->slice(obj, parts, point(0,0,0), gvector(0,0,1), &point(0,0,10)),
			errorMessage().c_str());
	}
	{
		assertex(modeler()->slice(obj, parts, point(0,0,-10), gvector(0,0,-1), &point(0,0,10)),
			errorMessage().c_str());
	}
}

void CTsSectioning::sliceOneSideCrashing()
{
	INSTALL_FINALLY_CLEANER
	
	CDbAcisEntityData obj;
	{
		assertex(modeler()->createTorus(point(0,0,0), 20, 5, icl::transf(), obj),
			errorMessage().c_str());
	}
	CDbAcisEntityData parts[2];
	g_crasher.startMonitoring();
	for (int i = 0; !i || g_crasher.startNextSession(); i++)
	{
		int rc = modeler()->slice(obj, parts, point(0,0,0), gvector(0,0,1), &point(0,0,10));
		if ((rc == 0 && i == 0) ||
			(rc != 0 && i != 0))
			throwex(errorMessage().c_str());

		g_crasher.endMonitoring();
	}
}
