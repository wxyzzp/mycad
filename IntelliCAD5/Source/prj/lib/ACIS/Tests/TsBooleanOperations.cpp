#include "stdafx.h"
#include "Modeler.h"
#include "DbAcisEntityData.h"

#include "TsTools.h"
#include "TsCrasher.h"

#include "gvector.h"
#include "transf.h"
#include "xstd.h"
using namespace icl;

#include <vector>
#include <utility>
using std::pair;

#include "testunit/collection.h"
#include "testunit/assert.h"
#include "testunit/autoregistrar.h"
using namespace testunit;


class CTsBooleanOperations: public collection
{
	//class with test common data
	class CTsBooleanData
	{
	protected:
		std::vector<CDbAcisEntityData> m_objs;
	public:
		void init();
	};

	class CTsSubtractionData
	{
	protected:
		std::vector<CDbAcisEntityData> m_tools;
		std::vector<CDbAcisEntityData> m_blanks;
	public:
		void init();
	};

	class CTsInterfereData
	{
	protected:
		std::vector<CDbAcisEntityData> m_set1;
		std::vector<CDbAcisEntityData> m_set2;
	public:
		void init();
	};


	//tests:
	class CTsBoolean: public collection, public CTsBooleanData
	{
		//tests:
		void boolean();
		void booleanCrash();
		void booleanIncorrectParameters();
		void unite();
		void intersect();

		void pre_action();
	public:
		CTsBoolean();
	};

	class CTsSubtraction: public collection, public CTsSubtractionData
	{
		//tests
		void subtract1Tool1Blank();
		void subtract1ToolNBlanks();
		void subtractNTools1Blank();
		void subtractNToolsNBlanks();
		void subtractNToolsNBlanksCrash();

		void pre_action();
	public:
		CTsSubtraction();
	};

	class CTsInterfere: public collection, public CTsInterfereData
	{
		//tests
		void interfere();
		void interfereCrash();

		void pre_action();
	public:

		CTsInterfere();
	};

public:
	CTsBooleanOperations();
};
static 
tauto_registrar<CTsBooleanOperations> MAKE_NAME(base_collection());

CTsBooleanOperations::CTsBooleanOperations()
:collection("Test boolean operations")
{
	add(new CTsBooleanOperations::CTsBoolean());
	add(new CTsBooleanOperations::CTsSubtraction());
	add(new CTsBooleanOperations::CTsInterfere());
}

CTsBooleanOperations::CTsBoolean::CTsBoolean()
:
collection("Test CAsModeler::boolean(),unite(),intersect()"), 
CTsBooleanOperations::CTsBooleanData()
{
	typedef TTsMethod<CTsBooleanOperations::CTsBoolean> test_t;
	add(new test_t("boolean", this, boolean));
	add(new test_t("boolean: crashing", this, booleanCrash));
	add(new test_t("boolean: incorrect parameters", this, booleanIncorrectParameters));
	add(new test_t("unite", this, unite));
	add(new test_t("intersect", this, intersect));
}

CTsBooleanOperations::CTsSubtraction::CTsSubtraction()
:
collection("Test CAsModeler::subtraction()"), 
CTsBooleanOperations::CTsSubtractionData()
{
	typedef TTsMethod<CTsBooleanOperations::CTsSubtraction> test_t;
	add(new test_t("subtract: one from one", this, subtract1Tool1Blank));
	add(new test_t("subtract: one from many", this, subtract1ToolNBlanks));
	add(new test_t("subtract: many from one", this, subtractNTools1Blank));
	add(new test_t("subtract: many from many", this, subtractNToolsNBlanks));
	add(new test_t("subtract: crashing", this, subtractNToolsNBlanksCrash));
}

CTsBooleanOperations::CTsInterfere::CTsInterfere()
:
collection("Test CAsModeler::interfere()"),
CTsBooleanOperations::CTsInterfereData()
{
	typedef TTsMethod<CTsBooleanOperations::CTsInterfere> test_t;
	add(new test_t("interfere", this, interfere));
	add(new test_t("interfere: crashing", this, interfereCrash));
}

void CTsBooleanOperations::CTsBoolean::boolean()
{
	INSTALL_FINALLY_CLEANER
		
	CDbAcisEntityData obj;
	{
		// uniting
		if (0 == modeler()->boolean(m_objs, obj, CModeler::eUnite))
			throwex(errorMessage().c_str());

		// intersection
		if (0 == modeler()->boolean(m_objs, obj, CModeler::eIntersect))
			throwex(errorMessage().c_str());
	}
}

void CTsBooleanOperations::CTsBoolean::booleanCrash()
{
	//CTsCrasher crasher;
	crasher.startMonitoring();

	CDbAcisEntityData obj;
	for (int i = 0; !i || crasher.startNextSession(); i++)
	{
		INSTALL_FINALLY_CLEANER

		// uniting
		int rc = modeler()->unite(m_objs, obj);
		if ((i == 0 && rc == 0) ||
			(i != 0 && rc != 0))
			throwex(errorMessage().c_str());

		crasher.endMonitoring();
	}
}

void CTsBooleanOperations::CTsBoolean::booleanIncorrectParameters()
{
	INSTALL_FINALLY_CLEANER

	CDbAcisEntityData obj;
	{
		// subtracting; to subtract use method CModeler::subtract
		if (0 != modeler()->boolean(m_objs, obj, CModeler::eSubtract))
			throwex("unsupported mode");

		// insufficient object's number
		std::vector<CDbAcisEntityData> objs(1);
		objs[1] = m_objs[0];
		if (0 != modeler()->boolean(objs, obj, CModeler::eSubtract))
			throwex("insufficient object's number");
	}
}

void CTsBooleanOperations::CTsSubtraction::subtract1Tool1Blank()
{
	INSTALL_FINALLY_CLEANER

	std::vector<CDbAcisEntityData> tool(1);
	tool[0] = m_tools[0];
	std::vector<CDbAcisEntityData> blank(1);
	blank[0] = m_blanks[0];
	xstd<CDbAcisEntityData>::vector obj;

	if (0 == modeler()->subtract(blank, tool, obj))
		throwex(errorMessage().c_str());
}

void CTsBooleanOperations::CTsSubtraction::subtract1ToolNBlanks()
{
	INSTALL_FINALLY_CLEANER

	std::vector<CDbAcisEntityData> tool(1);
	tool[0] = m_tools[0];
	xstd<CDbAcisEntityData>::vector obj;

	if (0 == modeler()->subtract(m_blanks, tool, obj))
		throwex(errorMessage().c_str());
}

void CTsBooleanOperations::CTsSubtraction::subtractNTools1Blank()
{
	INSTALL_FINALLY_CLEANER

	std::vector<CDbAcisEntityData> blank(1);
	blank[0] = m_blanks[0];
	xstd<CDbAcisEntityData>::vector obj;
	if (0 == modeler()->subtract(blank, m_tools, obj))
		throwex(errorMessage().c_str());
}

void CTsBooleanOperations::CTsSubtraction::subtractNToolsNBlanks()
{
	INSTALL_FINALLY_CLEANER

	xstd<CDbAcisEntityData>::vector obj;
	if (0 == modeler()->subtract(m_blanks, m_tools, obj))
		throwex(errorMessage().c_str());
}

void CTsBooleanOperations::CTsSubtraction::subtractNToolsNBlanksCrash()
{
	//CTsCrasher crasher;
	crasher.startMonitoring();
	xstd<CDbAcisEntityData>::vector obj;
	for (int i = 0; !i || crasher.startNextSession(); i++)
	{
		INSTALL_FINALLY_CLEANER

		obj.clear();

		int rc = modeler()->subtract(m_blanks, m_tools, obj);
		if ((i == 0 && rc == 0) ||
			(i != 0 && rc != 0))
			throwex(errorMessage().c_str());

		crasher.endMonitoring();
	}
}

void CTsBooleanOperations::CTsBoolean::unite()
{
	INSTALL_FINALLY_CLEANER

	CDbAcisEntityData obj;
	if (0 == modeler()->unite(m_objs, obj))
		throwex(errorMessage().c_str());
}

void CTsBooleanOperations::CTsBoolean::intersect()
{

	INSTALL_FINALLY_CLEANER

	CDbAcisEntityData obj;
	if (0 == modeler()->intersect(m_objs, obj))
		throwex(errorMessage().c_str());
}

void CTsBooleanOperations::CTsInterfere::interfere()
{
	INSTALL_FINALLY_CLEANER

	xstd<pair<long, long> >::vector pairs;
	xstd<CDbAcisEntityData>::vector objs;
	if (0 == modeler()->interfere(m_set1, m_set2, pairs, objs))
		throwex(errorMessage().c_str());
}

void CTsBooleanOperations::CTsInterfere::interfereCrash()
{
	//CTsCrasher crasher;
	crasher.startMonitoring();

	xstd<pair<long, long> >::vector pairs;
	xstd<CDbAcisEntityData>::vector objs;
	for (int i = 0; !i || crasher.startNextSession(); i++)
	{
		INSTALL_FINALLY_CLEANER

		pairs.clear();
		objs.clear();
		int rc = modeler()->interfere(m_set1, m_set2, pairs, objs);

		if ((i == 0 && rc == 0) ||
			(i != 0 && rc != 0))
			throwex(errorMessage().c_str());

		crasher.endMonitoring();
	}
}

void CTsBooleanOperations::CTsBooleanData::init()
{
	m_objs.resize(2);
	icl::transf t;
	if (0 == modeler()->createSphere(point(0,0,0), 10, t, m_objs[0]))
		throwex(errorMessage().c_str());

	if (0 == modeler()->createBlock(point(0,0,0), point(10,10,10), t, m_objs[1]))
		throwex(errorMessage().c_str());
}

void CTsBooleanOperations::CTsSubtractionData::init()
{
	m_tools.resize(2);
	m_blanks.resize(2);
	icl::transf t;
	if (0 == modeler()->createSphere(point(-5,-5,-5), 4.5, t, m_tools[0]))
		throwex(errorMessage().c_str());
	if (0 == modeler()->createSphere(point(5,5,5), 4.5, t, m_tools[1]))
		throwex(errorMessage().c_str());
	
	if (0 == modeler()->createBlock(point(-5,-5,-5), point(5,5,5), t, m_blanks[0]))
		throwex(errorMessage().c_str());
	if (0 == modeler()->createBlock(point(-10,-10,-10), point(10,10,10), t, m_blanks[1]))
		throwex(errorMessage().c_str());
}

void CTsBooleanOperations::CTsInterfereData::init()
{
	m_set1.resize(2);
	m_set2.resize(2);
	icl::transf t;
	if (0 == modeler()->createSphere(point(-5,-5,-5), 4.5, t, m_set1[0]))
		throwex(errorMessage().c_str());
	if (0 == modeler()->createSphere(point(5,5,5), 4.5, t, m_set1[1]))
		throwex(errorMessage().c_str());
	
	if (0 == modeler()->createBlock(point(-5,-5,-5), point(5,5,5), t, m_set2[0]))
		throwex(errorMessage().c_str());
	if (0 == modeler()->createBlock(point(-10,-10,-10), point(10,10,10), t, m_set2[1]))
		throwex(errorMessage().c_str());
}

void CTsBooleanOperations::CTsBoolean::pre_action()
{
	createModeler();
	init();
}

void CTsBooleanOperations::CTsSubtraction::pre_action()
{
	createModeler();
	init();
}

void CTsBooleanOperations::CTsInterfere::pre_action()
{
	createModeler();
	init();
}
