#include "stdafx.h"
#include "TsTools.h"

#include "gvector.h"
#include "transf.h"
#include "xstd.h"
using icl::point;
using icl::gvector;

#include <vector>
#include <utility>
using std::pair;

#include "testunit/collection.h"
#include "testunit/assert.h"
#include "testunit/autoregistrar.h"
using namespace testunit;

class CTsSaveLoad: public collection
{
	void saveLoad();
	void saveLoadIncorrectParameters();
public:
	CTsSaveLoad();
};

static 
tauto_registrar<CTsSaveLoad> MAKE_NAME(base_collection());

CTsSaveLoad::CTsSaveLoad()
:collection("Test CAsModeler::loadSatFile(),saveSatFile()")
{
	typedef TTsMethod<CTsSaveLoad> test_t;
	add(new test_t("test storing/restoring", this, saveLoad));
	add(new test_t("test storing/restoring: incorrect parameters", this, saveLoadIncorrectParameters));
}

void CTsSaveLoad::saveLoad()
{
	char file1[] = "../lib/acis/tests/sat/entities.sat";
	xstd<CDbAcisEntityData>::vector iobjs;
	std::vector<CDbAcisEntityData> oobjs(iobjs.begin(), iobjs.end());

	{
		assertex(modeler()->loadSatFile(file1, iobjs), errorMessage().c_str());
	}

	oobjs.assign(iobjs.begin(), iobjs.end());
	char file1copy[] = "../lib/acis/tests/sat/entities_copy.sat";
	{
		assertex(modeler()->saveSatFile(file1copy, oobjs), errorMessage().c_str());
	}

	char file2[] = "../lib/acis/tests/sat/3D.sat";
	iobjs.clear();
	{
		assertex(modeler()->loadSatFile(file2, iobjs), errorMessage().c_str());
	}

	oobjs.assign(iobjs.begin(), iobjs.end());
	char file2copy[] = "../lib/acis/tests/sat/3D_copy.sat";
	{
		assertex(modeler()->saveSatFile(file2copy, oobjs), errorMessage().c_str());
	}

	char file3[] = "../lib/acis/tests/sat/coloredbox.sat";
	iobjs.clear();
	{
		assertex(modeler()->loadSatFile(file3, iobjs), errorMessage().c_str());
	}

	oobjs.assign(iobjs.begin(), iobjs.end());
	char file3copy[] = "../lib/acis/tests/sat/coloredbox_copy.sat";
	{
		assertex(modeler()->saveSatFile(file3copy, oobjs), errorMessage().c_str());
	}
}

void CTsSaveLoad::saveLoadIncorrectParameters()
{
	char* filename = 0;
	std::vector<CDbAcisEntityData> oobjs;
	{
		assertex(!modeler()->saveSatFile(filename, oobjs), "attemption to work with null string");
	}
	xstd<CDbAcisEntityData>::vector iobjs;
	{
		assertex(!modeler()->loadSatFile(filename, iobjs), "attemption to work with null string");
	}
}
