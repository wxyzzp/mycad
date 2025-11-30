#include "stdafx.h"
#include "../AsModeler.h"
#include "TsTools.h"
#include "TsCrasher.h"
#include "TsFunctional.h"
#include "GeoData.h"
namespace gd = geodata;

#include "xstd.h"
#include "gvector.h"
#include "transf.h"
using icl::point;
using icl::gvector;

#include <vector>
#include <algorithm>
using std::for_each;

#include "testunit/collection.h"
#include "testunit/assert.h"
#include "testunit/autoregistrar.h"
using namespace testunit;

typedef rmem0_t<CAsModeler,int> modeler_imem0_t;
typedef guard_t<modeler_imem0_t, modeler_imem0_t> locker_t;

#define INSTALL_DATABASE_LOCKER locker_t locker(\
	ptr_rmem0(modeler(), CAsModeler::lockDatabase),\
	ptr_rmem0(modeler(), CAsModeler::unlockDatabase));


class CTsSolidedit: public collection
{
	//tests
	void moveFaces();
	void moveFacesCrashing();
	void offsetFaces();
	void offsetFacesCrashing();
	void rotateFaces();
	void rotateFacesCrashing();
	void deleteFaces();
	void taperFaces();
	void taperFacesCrashing();
	void deleteFacesCrashing();
	void copyFaces();
	void copyFacesCrashing();
	void colorFaces();
	void colorFacesCrashing();
	void colorEdges();
	void colorEdgesCrashing();
	void copyEdges();
	void copyEdgesCrashing();
	void extrudeFacesAlongPath();
	void extrudeFacesAlongPathCrashing();
	void extrudeFaces();
	void extrudeFacesAnother();
	void extrudeFacesAnotherCrasher();
	void imprintEraseTool();
	void imprintEraseToolCrasher();
	void imprintDontEraseTool();
	void imprintDontEraseToolCrasher();
	void separate();
	void separateCrashing();
	void clean();
	void cleanCrashing();
	void shell();
	void shellCrashing();
	void check();
	void checkCrashing();
public:
	CTsSolidedit();
};

static 
tauto_registrar<CTsSolidedit> MAKE_NAME(base_collection());

CTsSolidedit::CTsSolidedit()
:collection("Test methods of CAsModeler for solid editing")
{
	typedef TTsMethod<CTsSolidedit> test_t;
	add(new test_t("move solid's faces", this, moveFaces));
	add(new test_t("move solid's faces: crashing", this, moveFacesCrashing));
	add(new test_t("offset solid's faces", this, offsetFaces));
	add(new test_t("offset solid's faces", this, offsetFacesCrashing));
	add(new test_t("rotate solid's faces", this, rotateFaces));
	add(new test_t("rotate solid's faces: crashing", this, rotateFacesCrashing));
	add(new test_t("taper solid's faces", this, taperFaces));
	add(new test_t("taper solid's faces: crashing", this, taperFacesCrashing));
	add(new test_t("delete solid's faces", this, deleteFaces));
	add(new test_t("delete solid's faces: crashing", this, deleteFacesCrashing));
	add(new test_t("copy solid's faces", this, copyFaces));
	add(new test_t("copy solid's faces: crashing", this, copyFacesCrashing));
	add(new test_t("color solid's faces", this, colorFaces));
	add(new test_t("color solid's faces: crashing", this, colorFacesCrashing));
	add(new test_t("color solid's edges", this, colorEdges));
	add(new test_t("color solid's edges: crashing", this, colorEdgesCrashing));
	add(new test_t("copy solid's edges", this, copyEdges));
	add(new test_t("copy solid's edges: crashing", this, copyEdgesCrashing));
	add(new test_t("extrude faces along path", this, extrudeFacesAlongPath));
	add(new test_t("extrude faces along path: crashing", this, extrudeFacesAlongPathCrashing));
	add(new test_t("extrude faces", this, extrudeFaces));
	add(new test_t("extrude faces another", this, extrudeFacesAnother));
	add(new test_t("extrude faces another: crashing", this, extrudeFacesAnotherCrasher));
	add(new test_t("imprint: erase tool", this, imprintEraseTool));
	add(new test_t("imprint: erase tool: crashing", this, imprintEraseToolCrasher));
	add(new test_t("imprint: dont erase tool", this, imprintDontEraseTool));
	add(new test_t("imprint: dont erase tool crashing", this, imprintDontEraseToolCrasher));
	add(new test_t("separate", this, separate));
	add(new test_t("separate: crashing", this, separateCrashing));
	add(new test_t("clean", this, clean));
	add(new test_t("clean: crashing", this, cleanCrashing));
	add(new test_t("shell", this, shell));
	add(new test_t("shell: crashing", this, shellCrashing));
	add(new test_t("check", this, check));
	add(new test_t("check: crashing", this, checkCrashing));
}

void CTsSolidedit::moveFaces()
{
	INSTALL_FINALLY_CLEANER
	INSTALL_DATABASE_LOCKER

	CDbAcisEntityData old;
	xstd<HFACE>::vector hfaces;
	{
		assertex(modeler()->createCylinder(point(0,0,0), 3, point(0,0,10), icl::transf(), old),
			errorMessage().c_str());

		point root(0,0,-5);
		gvector dir(0,0,1);

		assertex(modeler()->pickFace(old, root, dir, icl::resabs(), hfaces),
			errorMessage().c_str());
	}

	{
		// move cylinder's base down
		CDbAcisEntityData new1;
		std::vector<HFACE> movedFaces(1);
		movedFaces[0] = hfaces[0];
		assertex(modeler()->moveFaces(old, new1, movedFaces, icl::translation(gvector(0,0,-1))),
			errorMessage().c_str());

		// move cylinder's top down also
		CDbAcisEntityData new2;
		movedFaces[0] = hfaces[1];
		assertex(modeler()->moveFaces(old, new2, movedFaces, icl::translation(gvector(0,0,-15))),
			errorMessage().c_str());
	}
}

void CTsSolidedit::moveFacesCrashing()
{
	
	INSTALL_FINALLY_CLEANER
	INSTALL_DATABASE_LOCKER

	for (int i = 0;; i++)
	{
		CDbAcisEntityData old;
		xstd<HFACE>::vector hfaces;
		{
			assertex(modeler()->createCylinder(point(0,0,0), 3, point(0,0,10), icl::transf(), old),
				errorMessage().c_str());

			point root(0,0,-5);
			gvector dir(0,0,1);

			assertex(modeler()->pickFace(old, root, dir, icl::resabs(), hfaces),
				errorMessage().c_str());
		}

		if (i == 0)
			g_crasher.startMonitoring();
		else if (!g_crasher.startNextSession())
			break;

		// move cylinder's base down
		CDbAcisEntityData new1;
		std::vector<HFACE> movedFaces(1);
		movedFaces[0] = hfaces[0];
		int rc = modeler()->moveFaces(old, new1, movedFaces, icl::translation(gvector(0,0,-1)));
		if ((rc == 0 && i == 0) ||
			(rc != 0 && i != 0))
			throwex(errorMessage().c_str());
		g_crasher.endMonitoring();
	}
}

void CTsSolidedit::offsetFaces()
{
	INSTALL_FINALLY_CLEANER
	INSTALL_DATABASE_LOCKER

	CDbAcisEntityData old;
	xstd<HFACE>::vector hfaces;
	{
		assertex(modeler()->createCylinder(point(0,0,0), 3, point(0,0,10), icl::transf(), old),
			errorMessage().c_str());

		point root(10,0,5);
		gvector dir(-1,0,0);
		double prec = 0.;

		assertex(modeler()->pickFace(old, root, dir, prec, hfaces),
			errorMessage().c_str());
	}

	{
		// offset lateral face
		CDbAcisEntityData new1;
		std::vector<HFACE> offsetedFaces(1);
		offsetedFaces[0] = hfaces[0];
		assertex(modeler()->offsetFaces(old, new1, offsetedFaces, 1),
			errorMessage().c_str());
	}
}

void CTsSolidedit::offsetFacesCrashing()
{
	
	INSTALL_FINALLY_CLEANER
	INSTALL_DATABASE_LOCKER

	for (int i = 0;; i++)
	{
		CDbAcisEntityData old;
		xstd<HFACE>::vector hfaces;
		{
			assertex(modeler()->createCylinder(point(0,0,0), 3, point(0,0,10), icl::transf(), old),
				errorMessage().c_str());

			point root(10,0,5);
			gvector dir(-1,0,0);
			double prec = 0.;

			assertex(modeler()->pickFace(old, root, dir, prec, hfaces),
				errorMessage().c_str());
		}

		if (i == 0)
			g_crasher.startMonitoring();
		else if (!g_crasher.startNextSession())
			break;

		// offset lateral face
		CDbAcisEntityData new1;
		std::vector<HFACE> offsetedFaces(1);
		offsetedFaces[0] = hfaces[0];
		
		int rc = modeler()->offsetFaces(old, new1, offsetedFaces, 1);
		if ((rc == 0 && i == 0) ||
			(rc != 0 && i != 0))
			throwex(errorMessage().c_str());
		
		g_crasher.endMonitoring();
	}
}

void CTsSolidedit::taperFaces()
{
	
	INSTALL_FINALLY_CLEANER
	INSTALL_DATABASE_LOCKER

	CDbAcisEntityData obj;
	xstd<HFACE>::vector hfaces;
	{
		assertex(modeler()->createBlock(point(0,0,0), point(10,10,10), icl::transf(), obj),
			errorMessage().c_str());

		assertex(modeler()->pickFace(obj, point(15,5,5), gvector(-1,0,0), icl::resabs(), hfaces),
			errorMessage().c_str());
	}

	CDbAcisEntityData new1;
	{
		std::vector<HFACE> taperedFaces(1);
		taperedFaces[0] = hfaces[0];
		assertex(modeler()->taperFaces(obj, new1, taperedFaces, point(10,0,0), gvector(1,0,0), PI/60),
			errorMessage().c_str());
	}
}

void CTsSolidedit::taperFacesCrashing()
{
	
	INSTALL_FINALLY_CLEANER
	INSTALL_DATABASE_LOCKER

	for (int i = 0;; i++)
	{
		CDbAcisEntityData obj;
		xstd<HFACE>::vector hfaces;
		{
			assertex(modeler()->createBlock(point(0,0,0), point(10,10,10), icl::transf(), obj),
				errorMessage().c_str());

			assertex(modeler()->pickFace(obj, point(15,5,5), gvector(-1,0,0), icl::resabs(), hfaces),
				errorMessage().c_str());
		}

		CDbAcisEntityData new1;
		std::vector<HFACE> taperedFaces(1);
		taperedFaces[0] = hfaces[0];

		if (i == 0)
			g_crasher.startMonitoring();
		else if (!g_crasher.startNextSession())
			break;

		int rc = modeler()->taperFaces(obj, new1, taperedFaces, point(10,0,0), gvector(1,0,0), PI/60);
		if ((rc == 0 && i == 0) ||
			(rc != 0 && i != 0))
			throwex(errorMessage().c_str());
		
		g_crasher.endMonitoring();
	}
}

void CTsSolidedit::rotateFaces()
{
	
	INSTALL_FINALLY_CLEANER
	INSTALL_DATABASE_LOCKER

	CDbAcisEntityData obj;
	xstd<HFACE>::vector hfaces;
	{
		assertex(modeler()->createBlock(point(0,0,0), point(10,10,10), icl::transf(), obj),
			errorMessage().c_str());

		assertex(modeler()->pickFace(obj, point(15,5,5), gvector(-1,0,0), icl::resabs(), hfaces),
			errorMessage().c_str());
	}

	CDbAcisEntityData new1;
	std::vector<HFACE> rotatedFaces(1);
	rotatedFaces[0] = hfaces[0];

	g_crasher.startMonitoring();
	for (int i = 0; !i || g_crasher.startNextSession(); i++)
	{
		int rc = modeler()->rotateFaces(obj, new1, rotatedFaces, point(10,0,5), gvector(0,1,0), PI/60);

		if ((rc == 0 && i == 0) ||
			(rc != 0 && i != 0))
			throwex(errorMessage().c_str());
		g_crasher.endMonitoring();
	}
}

void CTsSolidedit::rotateFacesCrashing()
{
	
	INSTALL_FINALLY_CLEANER
	INSTALL_DATABASE_LOCKER

	for (int i = 0;; i++)
	{
		CDbAcisEntityData obj;
		xstd<HFACE>::vector hfaces;
		{
			assertex(modeler()->createBlock(point(0,0,0), point(10,10,10), icl::transf(), obj),
				errorMessage().c_str());

			assertex(modeler()->pickFace(obj, point(15,5,5), gvector(-1,0,0), icl::resabs(), hfaces),
				errorMessage().c_str());
		}

		CDbAcisEntityData new1;
		std::vector<HFACE> rotatedFaces(1);
		rotatedFaces[0] = hfaces[0];

		if (i == 0)
			g_crasher.startMonitoring();
		else if (!g_crasher.startNextSession())
			break;

		int rc = modeler()->rotateFaces(obj, new1, rotatedFaces, point(10,0,5), gvector(0,1,0), PI/60);
		if ((rc == 0 && i == 0) ||
			(rc != 0 && i != 0))
			throwex(errorMessage().c_str());
		
		g_crasher.endMonitoring();
	}
}

void CTsSolidedit::deleteFaces()
{
	
	INSTALL_FINALLY_CLEANER
	INSTALL_DATABASE_LOCKER
	
	CDbAcisEntityData obj;
	xstd<HFACE>::vector hfaces;
	{
		assertex(modeler()->createCone(point(0,0,0), 5, point(0,0,10), 2, icl::transf(), obj),
			errorMessage().c_str());

		assertex(modeler()->pickFace(obj, point(0,0,15), gvector(0,0,1), icl::resabs(), hfaces),
			errorMessage().c_str());
	}

	CDbAcisEntityData new1;
	{
		std::vector<HFACE> deletedFaces(1);
		deletedFaces[0] = hfaces[0];
		assertex(modeler()->deleteFaces(obj, new1, deletedFaces),
			errorMessage().c_str());
	}
}

void CTsSolidedit::deleteFacesCrashing()
{
	
	INSTALL_FINALLY_CLEANER
	INSTALL_DATABASE_LOCKER
	
	for (int i = 0;; i++)
	{
		CDbAcisEntityData obj;
		xstd<HFACE>::vector hfaces;
		{
			assertex(modeler()->createCone(point(0,0,0), 5, point(0,0,10), 2, icl::transf(), obj),
				errorMessage().c_str());

			assertex(modeler()->pickFace(obj, point(0,0,15), gvector(0,0,1), icl::resabs(), hfaces),
				errorMessage().c_str());
		}

		CDbAcisEntityData new1;
		std::vector<HFACE> deletedFaces(1);
		deletedFaces[0] = hfaces[0];

		if (i == 0)
			g_crasher.startMonitoring();
		else if (!g_crasher.startNextSession())
			break;

		int rc = modeler()->deleteFaces(obj, new1, deletedFaces);
		if ((rc == 0 && i == 0) ||
			(rc != 0 && i != 0))
			throwex(errorMessage().c_str());
		g_crasher.endMonitoring();
	}
}

void CTsSolidedit::copyFaces()
{
	
	INSTALL_FINALLY_CLEANER
	INSTALL_DATABASE_LOCKER

	CDbAcisEntityData obj;
	xstd<HFACE>::vector hfaces;
	{
		assertex(modeler()->createCone(point(0,0,0), 5, point(10,10,10), 2, icl::transf(), obj),
			errorMessage().c_str());

		assertex(modeler()->pickFace(obj, point(0,0,15), gvector(0,0,-1), icl::resabs(), hfaces),
			errorMessage().c_str());
	}

	xstd<CDbAcisEntityData>::vector copiedFaces;
	{
		std::vector<HFACE> hfacesCopy(1);
		hfacesCopy[0] = hfaces[0];
		assertex(modeler()->copyFaces(obj, hfacesCopy, copiedFaces, icl::translation(gvector(0,0,2))),
			errorMessage().c_str());
	}
}

void CTsSolidedit::copyFacesCrashing()
{
	
	INSTALL_FINALLY_CLEANER
	INSTALL_DATABASE_LOCKER

	for (int i = 0;; i++)
	{
		CDbAcisEntityData obj;
		xstd<HFACE>::vector hfaces;
		{
			assertex(modeler()->createCone(point(0,0,0), 5, point(10,10,10), 2, icl::transf(), obj),
				errorMessage().c_str());

			assertex(modeler()->pickFace(obj, point(0,0,15), gvector(0,0,-1), icl::resabs(), hfaces),
				errorMessage().c_str());
		}

		if (i == 0)
			g_crasher.startMonitoring();
		else if (!g_crasher.startNextSession())
			break;

		xstd<CDbAcisEntityData>::vector copiedFaces;
		std::vector<HFACE> hfacesCopy(1);
		hfacesCopy[0] = hfaces[0];

		int rc = modeler()->copyFaces(obj, hfacesCopy, copiedFaces, icl::translation(gvector(0,0,2)));
		if ((rc == 0 && i == 0) ||
			(rc != 0 && i != 0))
			throwex(errorMessage().c_str());
		
		g_crasher.endMonitoring();
	}
}

void CTsSolidedit::colorFaces()
{
	
	INSTALL_FINALLY_CLEANER
	INSTALL_DATABASE_LOCKER

	CDbAcisEntityData obj;
	xstd<HFACE>::vector pickedFaces;
	{
		assertex(modeler()->createCone(point(0,0,0), 5, point(10,10,10), 2, icl::transf(), obj),
			errorMessage().c_str());

		assertex(modeler()->pickFace(obj, point(0,0,15), gvector(0,0,-1), icl::resabs(), pickedFaces),
			errorMessage().c_str());
	}

	CDbAcisEntityData new1;
	{
		std::vector<HFACE> coloredFaces(1);
		coloredFaces[0] = pickedFaces[0];
		int color = 2;
		assertex(modeler()->colorFaces(obj, new1, coloredFaces, color),
			errorMessage().c_str());
	}
}

void CTsSolidedit::colorFacesCrashing()
{
	
	INSTALL_FINALLY_CLEANER
	INSTALL_DATABASE_LOCKER

	for (int i = 0;; i++)
	{
		CDbAcisEntityData obj;
		xstd<HFACE>::vector pickedFaces;
		{
			assertex(modeler()->createCone(point(0,0,0), 5, point(10,10,10), 2, icl::transf(), obj),
				errorMessage().c_str());

			assertex(modeler()->pickFace(obj, point(0,0,15), gvector(0,0,-1), icl::resabs(), pickedFaces),
				errorMessage().c_str());
		}

		CDbAcisEntityData new1;
		std::vector<HFACE> coloredFaces(1);
		coloredFaces[0] = pickedFaces[0];
		int color = 2;

		if (i == 0)
			g_crasher.startMonitoring();
		else if (!g_crasher.startNextSession())
			break;

		int rc = modeler()->colorFaces(obj, new1, coloredFaces, color);
		if ((rc == 0 && i == 0) ||
			(rc != 0 && i != 0))
			throwex(errorMessage().c_str());
		
		g_crasher.endMonitoring();
	}
}

void CTsSolidedit::colorEdges()
{
	
	INSTALL_FINALLY_CLEANER
	INSTALL_DATABASE_LOCKER

	CDbAcisEntityData obj;
	xstd<HEDGE>::vector pickedEdges;
	{
		assertex(modeler()->createBlock(point(0,0,0), point(10,10,10), icl::transf(), obj),
			errorMessage().c_str());

		assertex(modeler()->pickEdge(obj, point(15,5,0), gvector(-1,0,0), icl::resabs(), pickedEdges),
			errorMessage().c_str());
	}

	CDbAcisEntityData new1;
	{
		std::vector<HFACE> coloredEdges(1);
		coloredEdges[0] = pickedEdges[0];
		int color = 2;
		assertex(modeler()->colorEdges(obj, new1, coloredEdges, color),
			errorMessage().c_str());
	}
}

void CTsSolidedit::colorEdgesCrashing()
{
	
	INSTALL_FINALLY_CLEANER
	INSTALL_DATABASE_LOCKER

	for (int i = 0;; i++)
	{
		CDbAcisEntityData obj;
		xstd<HEDGE>::vector pickedEdges;
		{
			assertex(modeler()->createBlock(point(0,0,0), point(10,10,10), icl::transf(), obj),
				errorMessage().c_str());

			assertex(modeler()->pickEdge(obj, point(15,5,0), gvector(-1,0,0), icl::resabs(), pickedEdges),
				errorMessage().c_str());
		}

		CDbAcisEntityData new1;
		std::vector<HFACE> coloredEdges(1);
		coloredEdges[0] = pickedEdges[0];
		int color = 2;

		if (i == 0)
			g_crasher.startMonitoring();
		else if (!g_crasher.startNextSession())
			break;

		int rc = modeler()->colorEdges(obj, new1, coloredEdges, color);
		if ((rc == 0 && i == 0) ||
			(rc != 0 && i != 0))
			throwex(errorMessage().c_str());
		g_crasher.endMonitoring();
	}
}

void CTsSolidedit::copyEdges()
{
	
	INSTALL_FINALLY_CLEANER
	INSTALL_DATABASE_LOCKER

	CDbAcisEntityData obj;
	xstd<HEDGE>::vector pickedEdges;
	{
		assertex(modeler()->createBlock(point(0,0,0), point(10,10,10), icl::transf(), obj),
			errorMessage().c_str());

		assertex(modeler()->pickEdge(obj, point(15,5,0), gvector(-1,0,0), icl::resabs(), pickedEdges),
			errorMessage().c_str());
	}

	CDbAcisEntityData new1;
	{
		std::vector<HEDGE> edgesForCopying(1);
		edgesForCopying[0] = pickedEdges[0];

		xstd<gd::curve*>::vector copiedEdges;
		assertex(modeler()->copyEdges(obj, edgesForCopying, copiedEdges, icl::translation(gvector(0,0,1))),
			errorMessage().c_str());
		if (copiedEdges.size() != 1)
		{
			for_each(copiedEdges.begin(), copiedEdges.end(), ptr_vfun1(&::operator delete));
			throwex("incorrect number of copied edges");
		}
		for_each(copiedEdges.begin(), copiedEdges.end(), ptr_vfun1(&::operator delete));
	}
}

void CTsSolidedit::copyEdgesCrashing()
{
	
	INSTALL_FINALLY_CLEANER
	INSTALL_DATABASE_LOCKER

	for (int i = 0;; i++)
	{
		CDbAcisEntityData obj;
		xstd<HEDGE>::vector pickedEdges;
		{
			assertex(modeler()->createBlock(point(0,0,0), point(10,10,10), icl::transf(), obj),
				errorMessage().c_str());

			assertex(modeler()->pickEdge(obj, point(15,5,0), gvector(-1,0,0), icl::resabs(), pickedEdges),
				errorMessage().c_str());
		}

		CDbAcisEntityData new1;
		std::vector<HEDGE> edgesForCopying(1);
		edgesForCopying[0] = pickedEdges[0];

		if (i == 0)
			g_crasher.startMonitoring();
		else if (!g_crasher.startNextSession())
			break;

		xstd<gd::curve*>::vector copiedEdges;
		int rc = modeler()->copyEdges(obj, edgesForCopying, copiedEdges, icl::translation(gvector(0,0,1)));

		for_each(copiedEdges.begin(), copiedEdges.end(), ptr_vfun1(&::operator delete));

		if ((rc == 0 && i == 0) ||
			(rc != 0 && i != 0))
			throwex(errorMessage().c_str());
		
		g_crasher.endMonitoring();
	}
}

void CTsSolidedit::extrudeFacesAlongPath()
{
	
	INSTALL_FINALLY_CLEANER
	INSTALL_DATABASE_LOCKER

	CDbAcisEntityData obj;
	xstd<HFACE>::vector pickedFaces;
	{
		assertex(modeler()->createCone(point(0,0,0), 5, point(10,10,10), 2, icl::transf(), obj),
			errorMessage().c_str());

		assertex(modeler()->pickFace(obj, point(0,0,15), gvector(0,0,-1), icl::resabs(), pickedFaces),
			errorMessage().c_str());
	}

	CDbAcisEntityData new1;
	{
		std::vector<HFACE> facesToExtrude(1);
		facesToExtrude[0] = pickedFaces[0];
		gd::line path;
		{
			path.m_start.set(0,0,0);
			path.m_end.set(0,0,2);
		}
		assertex(modeler()->extrudeFacesAlongPath(obj, new1, facesToExtrude, path),
			errorMessage().c_str());
	}
}

void CTsSolidedit::extrudeFacesAlongPathCrashing()
{
	
	INSTALL_FINALLY_CLEANER
	INSTALL_DATABASE_LOCKER

	for (int i = 0;; i++)
	{
		CDbAcisEntityData obj;
		xstd<HFACE>::vector pickedFaces;
		{
			assertex(modeler()->createCone(point(0,0,0), 5, point(10,10,10), 2, icl::transf(), obj),
				errorMessage().c_str());

			assertex(modeler()->pickFace(obj, point(0,0,15), gvector(0,0,-1), icl::resabs(), pickedFaces),
				errorMessage().c_str());
		}

		CDbAcisEntityData new1;
		std::vector<HFACE> facesToExtrude(1);
		facesToExtrude[0] = pickedFaces[0];
		gd::line path;
		{
			path.m_start.set(0,0,0);
			path.m_end.set(0,0,2);
		}

		if (i == 0)
			g_crasher.startMonitoring();
		else if (!g_crasher.startNextSession())
			break;

		int rc = modeler()->extrudeFacesAlongPath(obj, new1, facesToExtrude, path);
		if ((rc == 0 && i == 0) ||
			(rc != 0 && i != 0))
			throwex(errorMessage().c_str());
		g_crasher.endMonitoring();
	}
}

void CTsSolidedit::extrudeFaces()
{
	
	INSTALL_FINALLY_CLEANER
	INSTALL_DATABASE_LOCKER

	CDbAcisEntityData obj;
	xstd<HFACE>::vector pickedFaces;
	{
		assertex(modeler()->createCone(point(0,0,0), 5, point(10,10,10), 2, icl::transf(), obj),
			errorMessage().c_str());

		assertex(modeler()->pickFace(obj, point(0,0,15), gvector(0,0,-1), icl::resabs(), pickedFaces),
			errorMessage().c_str());
	}

	CDbAcisEntityData new1;
	{
		std::vector<HFACE> facesToExtrude(1);
		facesToExtrude[0] = pickedFaces[0];
		double dist = 2.;
		double taperAngle = PI/30;
		assertex(modeler()->extrudeFaces(obj, new1, facesToExtrude, dist, taperAngle),
			errorMessage().c_str());
	}
}

void CTsSolidedit::extrudeFacesAnother()
{
	
	INSTALL_FINALLY_CLEANER
	INSTALL_DATABASE_LOCKER

	CDbAcisEntityData obj;
	xstd<HFACE>::vector pickedFaces;
	{
		assertex(modeler()->createBlock(point(0,0,0), point(10,10,10), icl::transf(), obj),
			errorMessage().c_str());

		assertex(modeler()->pickFace(obj, point(20,20,20), gvector(1,1,1), icl::resabs(), pickedFaces),
			errorMessage().c_str());
	}

	CDbAcisEntityData new1;
	{
		std::vector<HFACE> facesToExtrude(pickedFaces.begin(), pickedFaces.end());
		double dist = 2.;
		double taperAngle = PI/30;
		assertex(modeler()->extrudeFaces(obj, new1, facesToExtrude, dist, taperAngle),
			errorMessage().c_str());
	}
}

void CTsSolidedit::extrudeFacesAnotherCrasher()
{
	
	INSTALL_FINALLY_CLEANER
	INSTALL_DATABASE_LOCKER

	for (int i = 0;; i++)
	{
		CDbAcisEntityData obj;
		xstd<HFACE>::vector pickedFaces;
		{
			assertex(modeler()->createBlock(point(0,0,0), point(10,10,10), icl::transf(), obj),
				errorMessage().c_str());

			assertex(modeler()->pickFace(obj, point(20,20,20), gvector(1,1,1), icl::resabs(), pickedFaces),
				errorMessage().c_str());
		}

		CDbAcisEntityData new1;
		std::vector<HFACE> facesToExtrude(pickedFaces.begin(), pickedFaces.end());
		double dist = 2.;
		double taperAngle = PI/30;

		if (i == 0)
			g_crasher.startMonitoring();
		else if (!g_crasher.startNextSession())
			break;

		int rc = modeler()->extrudeFaces(obj, new1, facesToExtrude, dist, taperAngle);
		if ((rc == 0 && i == 0) ||
			(rc != 0 && i != 0))
			throwex(errorMessage().c_str());
		g_crasher.endMonitoring();
	}
}

void CTsSolidedit::imprintEraseTool()
{
	
	INSTALL_FINALLY_CLEANER

	CDbAcisEntityData blank;
	{
		assertex(modeler()->createCone(point(0,0,0), 5, point(10,10,10), 2, icl::transf(), blank),
			errorMessage().c_str());
	}
	CDbAcisEntityData tool;
	{
		assertex(modeler()->createBlock(point(0,0,0), point(10,10,10), icl::transf(), tool),
			errorMessage().c_str());
	}

	CDbAcisEntityData new1;
	{
		assertex(modeler()->imprint(blank, tool, new1, true),
			errorMessage().c_str());
	}
}

void CTsSolidedit::imprintEraseToolCrasher()
{
	
	INSTALL_FINALLY_CLEANER

	CDbAcisEntityData blank;
	{
		assertex(modeler()->createCone(point(0,0,0), 5, point(10,10,10), 2, icl::transf(), blank),
			errorMessage().c_str());
	}
	CDbAcisEntityData tool;
	{
		assertex(modeler()->createBlock(point(0,0,0), point(10,10,10), icl::transf(), tool),
			errorMessage().c_str());
	}

	CDbAcisEntityData new1;

	g_crasher.startMonitoring();
	for (int i = 0; !i || g_crasher.startNextSession(); i++)
	{
		int rc = modeler()->imprint(blank, tool, new1, true);
		if ((rc == 0 && i == 0) ||
			(rc != 0 && i != 0))
			throwex(errorMessage().c_str());
		g_crasher.endMonitoring();
	}
}

void CTsSolidedit::imprintDontEraseTool()
{
	
	INSTALL_FINALLY_CLEANER

	CDbAcisEntityData blank;
	{
		assertex(modeler()->createCone(point(0,0,0), 5, point(10,10,10), 2, icl::transf(), blank),
			errorMessage().c_str());
	}
	CDbAcisEntityData tool;
	{
		assertex(modeler()->createBlock(point(0,0,0), point(10,10,10), icl::transf(), tool),
			errorMessage().c_str());
	}

	CDbAcisEntityData new1;
	{
		assertex(modeler()->imprint(blank, tool, new1, false),
			errorMessage().c_str());
	}
}

void CTsSolidedit::imprintDontEraseToolCrasher()
{
	
	INSTALL_FINALLY_CLEANER

	CDbAcisEntityData blank;
	{
		assertex(modeler()->createCone(point(0,0,0), 5, point(10,10,10), 2, icl::transf(), blank),
			errorMessage().c_str());
	}
	CDbAcisEntityData tool;
	{
		assertex(modeler()->createBlock(point(0,0,0), point(10,10,10), icl::transf(), tool),
			errorMessage().c_str());
	}

	CDbAcisEntityData new1;

	g_crasher.startMonitoring();
	for (int i = 0; !i || g_crasher.startNextSession(); i++)
	{
		int rc = modeler()->imprint(blank, tool, new1, false);
		if ((rc == 0 && i == 0) ||
			(rc != 0 && i != 0))
			throwex(errorMessage().c_str());
		g_crasher.endMonitoring();
	}
}

void CTsSolidedit::separate()
{
	
	INSTALL_FINALLY_CLEANER

	std::vector<CDbAcisEntityData> blank(1);
	{
		assertex(modeler()->createBlock(point(0,0,0), point(10,10,10), icl::transf(), blank[0]),
			errorMessage().c_str());
	}
	std::vector<CDbAcisEntityData> tool(1);
	{
		assertex(modeler()->createBlock(point(2,0,0), point(8,10,10), icl::transf(), tool[0]),
			errorMessage().c_str());
	}
	xstd<CDbAcisEntityData>::vector objs;
	{
		assertex(modeler()->subtract(blank, tool, objs),
			errorMessage().c_str());
		assertex(objs.size() == 1,
			"incorrect number of result objects, after subtracting");
	}
	xstd<CDbAcisEntityData>::vector lumps;
	{
		assertex(modeler()->separate(objs[0], lumps),
			errorMessage().c_str());
		assertex(lumps.size() == 2,
			"incorrect number of lumps, after separating solid");
	}
}

void CTsSolidedit::separateCrashing()
{
	
	INSTALL_FINALLY_CLEANER

	std::vector<CDbAcisEntityData> blank(1);
	{
		assertex(modeler()->createBlock(point(0,0,0), point(10,10,10), icl::transf(), blank[0]),
			errorMessage().c_str());
	}
	std::vector<CDbAcisEntityData> tool(1);
	{
		assertex(modeler()->createBlock(point(2,0,0), point(8,10,10), icl::transf(), tool[0]),
			errorMessage().c_str());
	}
	xstd<CDbAcisEntityData>::vector objs;
	{
		assertex(modeler()->subtract(blank, tool, objs),
			errorMessage().c_str());
		assertex(objs.size() == 1,
			"incorrect number of result objects, after subtracting");
	}
	
	xstd<CDbAcisEntityData>::vector lumps;

	g_crasher.startMonitoring();
	for (int i = 0; !i || g_crasher.startNextSession(); i++)
	{
		int rc = modeler()->separate(objs[0], lumps);
		if ((rc == 0 && i == 0) ||
			(rc != 0 && i != 0))
			throwex(errorMessage().c_str());
		g_crasher.endMonitoring();
	}
}

void CTsSolidedit::clean()
{
	
	INSTALL_FINALLY_CLEANER

	CDbAcisEntityData obj;
	{
		CDbAcisEntityData blank;
		{
			assertex(modeler()->createCone(point(0,0,0), 5, point(10,10,10), 2, icl::transf(), blank),
				errorMessage().c_str());
		}
		CDbAcisEntityData tool;
		{
			assertex(modeler()->createBlock(point(0,0,0), point(10,10,10), icl::transf(), tool),
				errorMessage().c_str());
		}

		assertex(modeler()->imprint(blank, tool, obj, true),
			errorMessage().c_str());
	}

	CDbAcisEntityData cleaned;
	{
		assertex(modeler()->clean(obj, cleaned),
			errorMessage().c_str());
	}
}

void CTsSolidedit::cleanCrashing()
{
	
	INSTALL_FINALLY_CLEANER

	CDbAcisEntityData obj;
	{
		CDbAcisEntityData blank;
		{
			assertex(modeler()->createCone(point(0,0,0), 5, point(10,10,10), 2, icl::transf(), blank),
				errorMessage().c_str());
		}
		CDbAcisEntityData tool;
		{
			assertex(modeler()->createBlock(point(0,0,0), point(10,10,10), icl::transf(), tool),
				errorMessage().c_str());
		}

		assertex(modeler()->imprint(blank, tool, obj, true),
			errorMessage().c_str());
	}

	CDbAcisEntityData cleaned;

	g_crasher.startMonitoring();
	for (int i = 0; !i || g_crasher.startNextSession(); i++)
	{
		int rc = modeler()->clean(obj, cleaned);
		if ((rc == 0 && i == 0) ||
			(rc != 0 && i != 0))
			throwex(errorMessage().c_str());
		g_crasher.endMonitoring();
	}
}

void CTsSolidedit::shell()
{
	
	INSTALL_FINALLY_CLEANER

	CDbAcisEntityData obj;
	std::vector<HFACE> hfaces;
	{
		assertex(modeler()->createBlock(point(0,0,0), point(10,10,10), icl::transf(), obj),
			errorMessage().c_str());

		xstd<HFACE>::vector allfaces;
		assertex(modeler()->getFaces(obj, allfaces),
			errorMessage().c_str());
		allfaces.pop_back();

		hfaces.insert(hfaces.end(), allfaces.begin(), allfaces.end());
	}
	CDbAcisEntityData new1;
	{
		assertex(modeler()->shell(obj, new1, hfaces, 1),
			errorMessage().c_str());
	}
}

void CTsSolidedit::shellCrashing()
{
	
	INSTALL_FINALLY_CLEANER

	CDbAcisEntityData obj;
	std::vector<HFACE> hfaces;
	{
		assertex(modeler()->createBlock(point(0,0,0), point(10,10,10), icl::transf(), obj),
			errorMessage().c_str());

		xstd<HFACE>::vector allfaces;
		assertex(modeler()->getFaces(obj, allfaces),
			errorMessage().c_str());
		allfaces.pop_back();

		hfaces.insert(hfaces.end(), allfaces.begin(), allfaces.end());
	}

	CDbAcisEntityData new1;
	
	g_crasher.startMonitoring();
	for (int i = 0; !i || g_crasher.startNextSession(); i++)
	{
		int rc = modeler()->shell(obj, new1, hfaces, 1);
		if ((rc == 0 && i == 0) ||
			(rc != 0 && i != 0))
			throwex(errorMessage().c_str());
		g_crasher.endMonitoring();
	}
}

void CTsSolidedit::check()
{
	
	INSTALL_FINALLY_CLEANER

	CDbAcisEntityData obj;
	{
		assertex(modeler()->createBlock(point(0,0,0), point(10,10,10), icl::transf(), obj),
			errorMessage().c_str());
	}
	xstd<icl::string>::vector info;
	{
		assertex(modeler()->check(obj, info),
			errorMessage().c_str());
	}
}

void CTsSolidedit::checkCrashing()
{
	
	INSTALL_FINALLY_CLEANER

	CDbAcisEntityData obj;
	{
		assertex(modeler()->createBlock(point(0,0,0), point(10,10,10), icl::transf(), obj),
			errorMessage().c_str());
	}
	
	g_crasher.startMonitoring();
	for (int i = 0; !i || g_crasher.startNextSession(); i++)
	{
		xstd<icl::string>::vector info;
		int rc = modeler()->check(obj, info);
		if ((rc == 0 && i == 0) ||
			(rc != 0 && i != 0))
			throwex(errorMessage().c_str());
		g_crasher.endMonitoring();
	}
}
