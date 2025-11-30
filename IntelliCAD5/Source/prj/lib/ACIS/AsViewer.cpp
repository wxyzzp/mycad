// File  :
// Author: Alexey Malov
#include "stdafx.h"
#include "AsSatStream.h"
#include "IcadDef.h"
#include "Acis.h"
#include "AsModeler.h"
#include "AsTools.h"
#include "AsSnaping.h"
#include "transf.h"
#include "gvector.h"
using icl::point;
using icl::gvector;
#include "AsAutoPtr.h"
#ifdef _TEST
#include "testunit/assert.h"
#else
#include <cassert>
#endif
#include <Delayimp.h>
#include <stdio.h>

							    
long CAsViewer::m_initedAcisModules = 0;

ACIS_API CModeler* createViewer()
{
    try
    {	
		CAsViewer::initializeViewerAcisDlls();
        static CAsViewer s_viewer;
        return &s_viewer;
    }
	catch (const exception&)
	{
		CAsViewer::terminateViewerAcisDlls();
		DUMP1("Can't create modeler: %s\n", find_err_mess(g_errorCode));
	}
    return 0;
}
						    
CAsViewer::CAsViewer()
throw (exception)
{
	ASSERT(m_initedAcisModules);

    // Disable logging
    checkOutcome(api_logging(0));

    // Set compress Bulletin Board option
    checkOutcome(api_set_int_option("compress_bb", 1));

	// Disable annotations
	checkOutcome(api_set_int_option("annotations", 0));

    // Set file version (use old format for AutoCad compability)
    checkOutcome(api_save_version(6, 0));

    // Set entity check level
    checkOutcome(api_set_int_option("check_level", 10));

    // Set product ID and units in file info, mandatory since Acis 6.3
    FileInfo fileInfo;
    fileInfo.set_product_id ("icad");
    fileInfo.set_units (1000.0); // Meters
    checkOutcome(api_set_file_info(FileId | FileUnits, fileInfo));

#if defined (_DEBUG)
	// Memory manager options for debugging
	char szMmgrFile[] = "mmgr.log";

    checkOutcome(api_set_str_option("mmgrfile", szMmgrFile));
	checkOutcome(api_set_int_option("mmgrlog", 0));
	checkOutcome(api_set_int_option("mmgrfill", 0));
#endif // defined (_DEBUG)
}

void unloadAcisDelayLoadedDlls()
{
/*
	__FUnloadDelayLoadedDLL(dll_SpaACIS); 
	__FUnloadDelayLoadedDLL(dll_SpaBase);
*/
}							

CAsViewer::~CAsViewer()
{
	try
	{
		// Clear cash, until Acis is runing
		m_cache.clear();

		terminateViewerAcisDlls();
	}
	catch (const exception&)
	{}

    // Clear static pointer
    s_pModeler = 0;

	unloadAcisDelayLoadedDlls();
}

void CAsViewer::initializeViewerAcisDlls() throw (exception)
{
	ASSERT(m_initedAcisModules == 0);

	// Start the modeler
    checkOutcome(api_start_modeller(0));

	// Initialize the base library (BASE)
	if (!initialize_base())
        throw exception("initialize_base");
	m_initedAcisModules |= EAcisModules::eBase;

	// Initialize the laws library (LAWS)
	if (!initialize_law())
		throw exception("initialize_law");
	m_initedAcisModules |= EAcisModules::eLaw;

	// Initialize Kernel (KERN)
    checkOutcome(api_initialize_kernel());
	m_initedAcisModules |= EAcisModules::eKernel;

    // Initialize Spline (AG)
    checkOutcome(api_initialize_spline());
	m_initedAcisModules |= EAcisModules::eSpline;

    // Initialize Faceter (FCT)
    // (faceting to get visualisation in mesh mode)
    checkOutcome(api_initialize_faceter());
	m_initedAcisModules |= EAcisModules::eFaceter;

	// Initialize Rendering Base (RBASE)
    // (ATTRIB_COL support)
    checkOutcome(api_initialize_rendering());
	m_initedAcisModules |= EAcisModules::eRendering;
}

void CAsViewer::terminateViewerAcisDlls()
{
	if(m_initedAcisModules == 0)
		return;	 // api_stop_modeller() should not be called twice

	try
	{
		// Terminate Faceter
		if(m_initedAcisModules & EAcisModules::eFaceter)
		{
			checkOutcome(api_terminate_faceter());
			m_initedAcisModules &= ~EAcisModules::eFaceter;
		}

		// Terminate Rendering Base
		if(m_initedAcisModules & EAcisModules::eRendering)
		{
			checkOutcome(api_terminate_rendering());
			m_initedAcisModules &= ~EAcisModules::eRendering;
		}

		// Terminate Spline
		if(m_initedAcisModules & EAcisModules::eSpline)
		{
			checkOutcome(api_terminate_spline());
			m_initedAcisModules &= ~EAcisModules::eSpline;
		}

		// Terminate Kernel
		if(m_initedAcisModules & EAcisModules::eKernel)
		{
			checkOutcome(api_terminate_kernel());
			m_initedAcisModules &= ~EAcisModules::eKernel;
		}

		// Terminate the laws library
		if(m_initedAcisModules & EAcisModules::eLaw)
		{
			terminate_law();
			m_initedAcisModules &= ~EAcisModules::eLaw;
		}

		// Terminate the base library
		if(m_initedAcisModules & EAcisModules::eBase)
		{
			terminate_base();
			m_initedAcisModules &= ~EAcisModules::eBase;
		}

		// Stop the modeler
		checkOutcome(api_stop_modeller());
	}
	catch (const exception&)
	{}
}

void CAsViewer::free(void* p)
{
	try
	{
		delete p;
	}
	catch (const exception&)
	{
		DUMP0("\nDeleting of bad pointer!\n");
	}
}

void CAsViewer::clear()
{
	try
	{
		m_cache.clear();
	}
	catch (...)
	{}
}

int CAsViewer::display
(
const CDbAcisEntityData& obj, 
int nIsoLines, 
double dPrecision, 
sds_dobjll** ppSdsDispObjs
)
{
	g_errorCode = 0;

	if (!ppSdsDispObjs)
		return 0;

	try
	{
		CAsObj* pObj = m_cache[obj];

		*ppSdsDispObjs = 0;
		pObj->display(*ppSdsDispObjs, nIsoLines, dPrecision);
		return 1;
	}
	catch (const exception&)
	{
		return 0;
	}
}

CAsObj* CAsViewer::create(const CDbAcisEntityData& obj)
{
	g_errorCode = 0;

    if (obj.getSat().empty())
        return 0;

	try
	{
//#define _DEBUG_BODY 1
#ifdef _DEBUG_BODY
        FILE *stream = fopen("acis_body.out", "w");
        fprintf(stream, "%s", obj.getSat().c_str());
        fclose(stream);
#endif // _DEBUG_BODY

		CAsSatStream isat(const_cast<char*>(obj.getSat().c_str()), obj.getSat().size(), strstream::in);
		ENTITY_LIST el;
		checkOutcome(api_restore_entity_list_file(&isat, el));
		isat.freeze();

		assert(el.count() == 1);
		assert(is_BODY(el[0]));

		CAsObj* pObj = new CAsObj;
		pObj->set(static_cast<BODY*>(el[0]));

		return pObj;
	}
	catch (const exception&)
	{
		return 0;
	}
}

int CAsViewer::getSnapingPoints
(
const CDbAcisEntityData& obj,
int osmode,
const point& _pick,
const point& _last,
const gvector& _dir,
xstd<snaping>::vector& pnts
)
{
	g_errorCode = 0;

	try
	{
		CAsObj* pObj = m_cache[obj];
		BODY* pBody = pObj->get();

		checkOutcome(api_change_body_trans(pBody, 0));

		pnts.resize(0);

		if (osmode & IC_OSMODE_END)
			getEndSnapPoints(pObj->get(), pnts);

		SPAposition pick(_pick.x(), _pick.y(), _pick.z());
		SPAunit_vector dir(_dir.x(), _dir.y(), _dir.z());
		SPAposition last(_last.x(), _last.y(), _last.z());

		ENTITY_LIST edges;
		checkOutcome(api_get_edges(pObj->get(), edges));
		edges.init();
		for (EDGE* pEdge = 0; pEdge = static_cast<EDGE*>(edges.next());)
		{
			CURVE* pCurve = pEdge->geometry();
			if (!pCurve)
				continue;

			if (osmode & IC_OSMODE_MID)
				getMidSnapPoints(pEdge, pnts);

			if (pCurve->identity() == ELLIPSE_TYPE)
			{
				if (osmode & IC_OSMODE_CENTER)
					getCenterSnapPoints(pEdge, pnts);
				if (osmode & IC_OSMODE_QUADRANT)
					getQuadrantSnapPoints(pEdge, pnts);
			}

			TAutoPtr<bounded_curve> bcrv(get_bounded_curve(pEdge));
			if (osmode & IC_OSMODE_NEAR)
				getNearSnapPoints(bcrv, pick, dir, pnts);

			if (osmode & IC_OSMODE_PERP)
				getPerpSnapPoints(bcrv, last, pnts);

			if (osmode & IC_OSMODE_TAN)
				getTangSnapPoints(pEdge, last, pnts);
		}

		return 1;
	}
	catch (const exception&)
	{
		pnts.clear();
		return 0;
	}
}

int CAsViewer::getGrips
(
const CDbAcisEntityData& obj,
xstd<icl::point>::vector& grips
)
{
	g_errorCode = 0;

	try
	{
		CAsObj* pObj = m_cache[obj];
		BODY* body = pObj->get();

		checkOutcome(api_change_body_trans(body, 0));

		ENTITY_LIST vertices;
		checkOutcome(api_get_vertices(body, vertices));
		vertices.init();
		for (VERTEX* v = 0; v = static_cast<VERTEX*>(vertices.next());)
		{
			const SPAposition& p = v->geometry()->coords();
			grips.push_back(point(p.x(), p.y(), p.z()));
		}

		ENTITY_LIST faces;
		checkOutcome(api_get_faces(body, faces));

		faces.init();
		for (FACE* face = 0; face = static_cast<FACE*>(faces.next());)
		{
			if (is_PLANE(face->geometry()))
			{
				do
				{
					if (face->loop() == 0 || face->loop()->next() != 0)
						break;

					LOOP* loop = face->loop();
					if (loop->start() == 0)
						break;

					COEDGE* coedge = loop->start();
					if (coedge != coedge->next())
						break;

					if (coedge->start() != coedge->end())
						break;

					if (coedge->edge() == 0)
						break;
					EDGE* edge = coedge->edge();

					if (edge->geometry() == 0)
						break;
					CURVE* cur = edge->geometry();

					if (!is_ELLIPSE(cur))
						break;
					ELLIPSE* el = static_cast<ELLIPSE*>(cur);

					const SPAposition& c = el->centre();
					grips.push_back(point(c.x(), c.y(), c.z()));
				}
				while (false);
			}
			else if (is_SPHERE(face->geometry()))
			{
				if (face->loop() == 0)
				{
					const SPAposition& c = static_cast<SPHERE*>(face->geometry())->centre();
					grips.push_back(point(c.x(), c.y(), c.z()));
				}
			}
		}

		return 1;
	}
	catch (const exception&)
	{
		grips.clear();
		return 0;
	}
}

int CAsViewer::getBoundingBox
(
const CDbAcisEntityData& obj,
point& low,
point& high,
const icl::transf& _w2u
)
{
	g_errorCode = 0;

	try
	{
		CAsObj* pObj = m_cache[obj];
		BODY* body = pObj->get();

		SPAtransf w2u = acisTransf(_w2u);

		SPAtransf t;
		TRANSFORM* pTransf = body->transform();
		if (pTransf != 0)
			t = pTransf->transform();
		t *= w2u;

		SPAbox bb = get_body_box(body, &t);
		low.set(bb.low().x(), bb.low().y(), bb.low().z());
		high.set(bb.high().x(), bb.high().y(), bb.high().z());

		return 1;
	}
	catch (const exception&)
	{
		return 0;
	}
}

int CAsViewer::getPerimeter
(
const CDbAcisEntityData& obj,
double& perimeter
)
{
	g_errorCode = 0;

	try
	{
		CAsObj* pObj = m_cache[obj];
		if (!isPlanar(pObj->get()))
			return 0;
		BODY* sheet = pObj->get();

		ENTITY_LIST edges;
		checkOutcome(api_get_edges(sheet, edges));

		perimeter = 0;
		edges.init();
		for (EDGE* edge = 0; edge = static_cast<EDGE*>(edges.next());)
		{
			perimeter += edge->length();
		}

		return 1;
	}
	catch (const exception&)
	{
		return 0;
	}
}

int CAsViewer::getLastErrorCode()
{
	return g_errorCode;
}

int CAsViewer::getErrorMessage
(
int ec,
icl::string& mes
)
{
	try
	{
		mes = find_err_mess(ec);
	}
	catch (const exception&)
	{
		return 0;
	}
	return 1;
}
