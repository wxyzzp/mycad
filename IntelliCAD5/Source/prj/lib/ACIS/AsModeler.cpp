// File  :
// Author: Alexey Malov
#include "stdafx.h"

#ifndef BUILD_WITH_LIMITED_ACIS_SUPPORT
//Additional Acis headers
#include "geom_utl.hxx"
#include "geometry.hxx"

#include "acis.hxx"  
#include "intrapi.hxx"  
#include "api.hxx"  
#include "entity.hxx"  
#include "lists.hxx"  
#include "insanity_list.hxx"  
#include "acis_options.hxx"  

//As headers
#include "Acis.h"
#include "AsModeler.h"
#include "AsTools.h"
#include "AsAutoPtr.h"
#include "AsConstructor.h"
#include "AsMesh.h"
#include "AsConstructor.h"
#include "AsExtractor.h"
#include "AsFunctional.h"
//Icad headers
#include "GeoData.h"
#include "dbdefines.h" //DB_3DSOLID, DB_BODY, DB_REGION
#include "gvector.h"
#include "transf.h"
#include "vector.h"
using icl::point;
using icl::gvector;
//std
#include <algorithm>
using std::sort;
using std::for_each;
using std::sort;
using std::binary_search;
using std::find;
using std::find_if;
#include <memory>
using std::auto_ptr;
#include <functional>
using std::ptr_fun;
using std::pointer_to_unary_function;
using std::bind2nd;
using std::greater;
using std::less;
#include <algorithm>
using std::swap;
#include <iterator>
using std::back_inserter;
#ifdef _TEST
#include "testunit/assert.h"
#else
#include <cassert>
#endif

#include <fstream.h>
#include <stdio.h>
#include <Delayimp.h>

extern Map<rgb_color, int> ColorMapCache;


LONG DelayLoadDllExceptionFilter(PEXCEPTION_POINTERS pep) 
{
	// Assume we recognize this exception
	LONG lDisposition = EXCEPTION_EXECUTE_HANDLER;  

	// If this is a Delay-load problem, ExceptionInformation[0] points 
	// to a DelayLoadInfo structure that has detailed error info
	PDelayLoadInfo pdli = PDelayLoadInfo(pep->ExceptionRecord->ExceptionInformation[0]);

	switch(pep->ExceptionRecord->ExceptionCode) 
	{
	case VcppException(ERROR_SEVERITY_ERROR, ERROR_MOD_NOT_FOUND):
		// The DLL module was not found at runtime
		CAsModeler::terminateModelerAcisDlls();
		break;

	case VcppException(ERROR_SEVERITY_ERROR, ERROR_PROC_NOT_FOUND):
		// The DLL module was found, but it doesn't contain the function
		assert(pdli->dlp.fImportByName);
		CAsModeler::terminateModelerAcisDlls();
		break; 

	default:
		// We don't recognize this exception
		lDisposition = EXCEPTION_CONTINUE_SEARCH;  
		break;
	}
	lDisposition = EXCEPTION_CONTINUE_SEARCH;  // icad catchs this exeption and shows message box
	return lDisposition;
}

void safeInitializeModelerAcisDlls()
{
	__try
	{
		CAsModeler::initializeModelerAcisDlls();
	}
	__except(DelayLoadDllExceptionFilter(GetExceptionInformation())) 
	{}
}

ACIS_API CModeler* createModeler()
{
    try
    {
		safeInitializeModelerAcisDlls();
        static CAsModeler s_modeler;
		CAsModeler::s_pModeler = &s_modeler;
        return &s_modeler;
    }
	catch (const exception&)
	{
		CAsModeler::terminateViewerAcisDlls();
		DUMP1("Can't create modeler: %s\n", find_err_mess(g_errorCode));
	}

    return 0;
}

CAsModeler::CAsModeler() throw (exception)
{
	ASSERT(m_initedAcisModules);

	// Use Repair Body Intersections in Local Operations and Shelling
	checkOutcome(api_set_int_option ("lop_repair_self_int", 1));

	checkOutcome(api_set_int_option ("lop_check_invert", 0));

	checkOutcome(api_set_int_option ("lop_ff_int", 0));

	// Set entity check level
	checkOutcome(api_set_int_option("check_level", 10));
}

CAsModeler::~CAsModeler()
{
	try
	{
		// Clear cash, until Acis is runing
		m_cache.clear();
		
		terminateModelerAcisDlls();
	}
	catch (const exception&)
	{}
}

void CAsModeler::initializeModelerAcisDlls()
{
	CAsViewer::initializeViewerAcisDlls();

	// Initialize Generic Attributes (GA)
	// ::extrude
	checkOutcome(api_initialize_generic_attributes());
	m_initedAcisModules |= EAcisModules::eGenericAttributes;

	// Initialize Intersectors (INTR)
	// ray testing; api_check_entity;
	// api_face_nu_nv_isolines (visualization of faces)
	// api_facet_curve (visualization of curves)
	checkOutcome(api_initialize_intersectors());
	m_initedAcisModules |= EAcisModules::eIntersectors;

	// Initialize Constructors (CSTR)
	checkOutcome(api_initialize_constructors());
	m_initedAcisModules |= EAcisModules::eConstructors;

	// Initialize Sweeping (SWP)
	// CAsModeler::createSurfaceRevolved
	// CAsModeler::createSurfaceExtruded
	checkOutcome(api_initialize_sweeping());
	m_initedAcisModules |= EAcisModules::eSweeping;

	// Initialize Skinning (SKIN)
	// CAsModeler::createSurfaceRuled
	checkOutcome(api_initialize_skinning());
	m_initedAcisModules |= EAcisModules::eSkinning;

	// Initialize Covering (COVR)
	// CAsModeler::createSurfaceCurves
	checkOutcome(api_initialize_covering());
	m_initedAcisModules |= EAcisModules::eCovering;

	// Initialize Boolean (BOOL)
	checkOutcome(api_initialize_booleans());
	m_initedAcisModules |= EAcisModules::eBooleans;

	// Initialize Local Ops (LOP)
	// CAsModeler::moveFaces, rotateFaces, offsetFaces, taperFaces
	checkOutcome(api_initialize_local_ops());
	m_initedAcisModules |= EAcisModules::eLocalOps;

	// Initialize Remove Faces (REM)
	// CAsModeler::deleteFaces
	checkOutcome(api_initialize_face_removal());
	m_initedAcisModules |= EAcisModules::eFaceRemoval;

	// Initialize Euler Operations (EULR)
	// CAsModeler::imprint
	checkOutcome(api_initialize_euler_ops());
	m_initedAcisModules |= EAcisModules::eEulerOps;

	// Initialize Shelling (SHL)
	// CAsModeler::shell
	checkOutcome(api_initialize_shelling());
	m_initedAcisModules |= EAcisModules::eShelling;

	// Initialize Blending (BLND)
	// CAsModeler::fillet, chamfer
	checkOutcome(api_initialize_blending());
	m_initedAcisModules |= EAcisModules::eBlending;
}

void CAsModeler::terminateModelerAcisDlls()
{
	try
	{
		// Terminate Blending
		if(m_initedAcisModules & EAcisModules::eBlending)
		{
			checkOutcome(api_terminate_blending());
			m_initedAcisModules &= ~EAcisModules::eBlending;
		}

		// Terminate Generic Attributes
		if(m_initedAcisModules & EAcisModules::eGenericAttributes)
		{
			checkOutcome(api_terminate_generic_attributes());
			m_initedAcisModules &= ~EAcisModules::eGenericAttributes;
		}

		// Terminate Shelling
		if(m_initedAcisModules & EAcisModules::eShelling)
		{
			checkOutcome(api_terminate_shelling());
			m_initedAcisModules &= ~EAcisModules::eShelling;
		}

		// Terminate Euler Operations
		if(m_initedAcisModules & EAcisModules::eEulerOps)
		{
			checkOutcome(api_terminate_euler_ops());
			m_initedAcisModules &= ~EAcisModules::eEulerOps;
		}

		// Terminate Remove Faces
		if(m_initedAcisModules & EAcisModules::eFaceRemoval)
		{
			checkOutcome(api_terminate_face_removal());
			m_initedAcisModules &= ~EAcisModules::eFaceRemoval;
		}

		// Terminate Local Ops
		if(m_initedAcisModules & EAcisModules::eLocalOps)
		{
			checkOutcome(api_terminate_local_ops());
			m_initedAcisModules &= ~EAcisModules::eLocalOps;
		}

		// Terminate Boolean
		if(m_initedAcisModules & EAcisModules::eBooleans)
		{
			checkOutcome(api_terminate_booleans());
			m_initedAcisModules &= ~EAcisModules::eBooleans;
		}

		// Terminate Covering
		if(m_initedAcisModules & EAcisModules::eCovering)
		{
			checkOutcome(api_terminate_covering());
			m_initedAcisModules &= ~EAcisModules::eCovering;
		}

		// Terminate Skinning
		if(m_initedAcisModules & EAcisModules::eSkinning)
		{
			checkOutcome(api_terminate_skinning());
			m_initedAcisModules &= ~EAcisModules::eSkinning;
		}

		// Terminate Sweeping
		if(m_initedAcisModules & EAcisModules::eSweeping)
		{
			checkOutcome(api_terminate_sweeping());
			m_initedAcisModules &= ~EAcisModules::eSweeping;
		}

		// Terminate Constructors
		if(m_initedAcisModules & EAcisModules::eConstructors)
		{
			checkOutcome(api_terminate_constructors());
			m_initedAcisModules &= ~EAcisModules::eConstructors;
		}

		// Terminate Intersectors
		if(m_initedAcisModules & EAcisModules::eIntersectors)
		{
			checkOutcome(api_terminate_intersectors());
			m_initedAcisModules &= ~EAcisModules::eIntersectors;
		}

		CAsViewer::terminateViewerAcisDlls();
	}
	catch (const exception&)
	{}
}


int CAsModeler::copy
(
const CDbAcisEntityData& src,
CDbAcisEntityData& dest
)
{
	dest = src;
    return 1;
}

int CAsModeler::transform
(
const CDbAcisEntityData& src,
CDbAcisEntityData& dest,
const icl::transf& mx
)
{
	g_errorCode = 0;
	try
	{
		CAsCache::iterator_t iobj = m_cache.find(src);
		CAsObj* pObj = iobj->second.obj();

		pObj->transform(acisTransf(mx));
		CAsCacheItem item = m_cache.detach(iobj);

		CCharPtr sat(saveEntity(pObj->get()));
		dest.setSat(sat.get());
		dest.setType(src.getType());

		m_cache.update(dest, item);
	}
	catch (const exception&)
	{
		return 0;
	}
    return 1;
}

int CAsModeler::loadSatFile
(
const char* sFilename,
int currColor,
const RGBQUAD* pCurrPaletteColors,
xstd<CDbAcisEntityData>::vector& objs
)
{
	g_errorCode = 0;

	if (!sFilename)
		return 0;

    CFilePtr f(fopen(sFilename, "r"));
	if (!f)
		return 0;

	try
	{
        // Remove the cached values because we don't know if the palette is changed
        // since last call.
        //
        ColorMapCache.Clear();

	    ENTITY_LIST ents;
		checkOutcome(api_restore_entity_list(f, TRUE, ents));

		CDbAcisEntityData obj;
		for (int i = 0, n = ents.count(); i < n; i++)
		{
			CEntityPtr pEntity(ents[i]);

			CBodyPtr pBody;
			// Make Acis BODY (if can)
			try
			{
				pBody = cstr::body(pEntity);
                setBodyColor(pBody, currColor, pCurrPaletteColors);
			}
			catch (const exception&)
			{
				continue;
			}
			pEntity.release();

			// Save object's sat data
			CCharPtr sat(saveEntity(pBody.get()));

			// Form output
			obj.setSat(sat.get());
			obj.setType(isPlanar(pBody)? CDbAcisEntityData::eRegion: CDbAcisEntityData::eBody);
			objs.push_back(obj);

			// Add to cache
			m_cache.insert(obj, pBody.release());
		}
        // Clean up the cache because color map can be changed later
        ColorMapCache.Clear();
	}
	catch (const exception&)
	{
		objs.clear();

        // Clean up the cache because color map can be changed later
        ColorMapCache.Clear();
		return 0;
	}

	return 1;
}

int CAsModeler::saveSatFile
(
const char* sFilename,
const std::vector<CDbAcisEntityData>& objs,
const RGBQUAD* pCurrPaletteColors,
int version
)
{
	g_errorCode = 0;

	if (!sFilename)
		return 0;

	ENTITY_LIST ents;
	try
	{
        ColorMapCache.Clear();

		int stored_major, stored_minor;
		if (version >= 15) {
			checkOutcome(api_get_save_version(stored_major, stored_minor));

			int major = version / 10;
			int minor = version - major * 10;

			checkOutcome(api_save_version(major, minor));
		}

        // remove cached color map.
        //
		for (int i = 0, n = objs.size(); i < n; i++)
		{
			const CAsObj* pObj = m_cache[objs[i]];
			BODY* pBody = const_cast<BODY*>(pObj->get());
            mapBodyColorsToRGBValues(pBody, pCurrPaletteColors);
            ents.add(pBody);
		}

		CFilePtr f(fopen(sFilename, "w"));
		if (!f)
			return 0;

		checkOutcome(api_save_entity_list(f, TRUE, ents));

		if (version >= 15) 
			checkOutcome(api_save_version(stored_major, stored_minor));

        // remap color back
        if (!ColorMapCache.IsEmpty()) { 
		    for (i = 0, n = ents.count(); i < n; i++) {
                BODY* pBody = static_cast<BODY*>(ents[i]);
                setBodyColor(pBody, 0/*dummy*/, pCurrPaletteColors);
		    }
            ColorMapCache.Clear();
        }
	}
	catch (const exception&)
	{
        // remap color back
        if (!ColorMapCache.IsEmpty()) { 
		    for (int i = 0, n = ents.count(); i < n; i++) {
                BODY* pBody = static_cast<BODY*>(ents[i]);
                setBodyColor(pBody, 0/*dummy*/, pCurrPaletteColors);
		    }
            ColorMapCache.Clear();
        }		
        return 0;
	}

	return 1;
}

#if 0
int CAsModeler::create
(
CDbAcisEntityData data,
const char* type,
const char* szLayer/* = 0*/,
int color/* = -1*/
)
{
	g_errorCode = 0;

    if (!data)
        return 0;

    resbuf* elist = 0;
    if (szLayer && color >=0)
        elist = sds_buildlist(RTDXF0, type, 1, data, 8, szLayer, 62, color, 0);
    else if (szLayer)
        elist = sds_buildlist(RTDXF0, type, 1, data, 8, szLayer, 0);
    else if (color >= 0)
        elist = sds_buildlist(RTDXF0, type, 1, data, 62, color, 0);
    else
        elist = sds_buildlist(RTDXF0, type, 1, data, 0);

	if (!elist)
        return 0;

    sds_name elast;
    int rc = (sds_entmake(elist) == RTNORM) &&
             (sds_entlast(elast) == RTNORM) &&
             (sds_redraw(elast, IC_REDRAW_DRAW) == RTNORM);
    sds_relrb(elist);

    return rc;
}
#endif

int CAsModeler::getBasePoint
(
const CDbAcisEntityData& obj,
point& pnt
)
{
	g_errorCode = 0;

	try
	{
		CAsObj* pObj = m_cache[obj];
		return pObj->getBasePoint(pnt);
	}
	catch (const exception&)
	{
		return 0;
	}
}

int CAsModeler::getMeshRepresentation
(
const CDbAcisEntityData& obj,
xstd<point>::vector& points,
xstd<int>::vector& faces
)
{
	g_errorCode = 0;

	try
	{
		const CAsObj* pObj = m_cache[obj];
		return pObj->getMeshRepresentation(points, faces);
	}
	catch (const exception&)
	{
		return 0;
	}
}

int CAsModeler::createBlock
(
const point& p1,
const point& p2,
const icl::transf& mtx,
int color,
CDbAcisEntityData& obj
)
{
	g_errorCode = 0;

    if (is_zero((p1-p2).z()))
        return 0;
	if (p1 == p2)
		return 0;

	try
	{
		CBodyPtr pBody;
		checkOutcome(api_solid_block(asPosition(p1), asPosition(p2), pBody.init()));

		checkEntity(pBody);

		if (!mtx.identity())
			checkOutcome(api_transform_entity(pBody, acisTransf(mtx)));

		if (!setBodyColor(pBody, color))
			return 0;

		CCharPtr sat(saveEntity(pBody));

		obj.setSat(sat.get());
		obj.setType(CDbAcisEntityData::e3DSolid);

		m_cache.insert(obj, pBody.release());

		return 1;
	}
	catch (const exception&)
	{
		return 0;
	}
}

int CAsModeler::createSphere
(
const point& c,
double radius,
const icl::transf& mtx,
int color,
CDbAcisEntityData& obj
)
{
	g_errorCode = 0;

    if (is_zero(radius) || is_negative(radius))
        return 0;

	try
	{
		SPAposition center(asPosition(c));

		CBodyPtr pBody;
		checkOutcome(api_solid_sphere(center, radius, pBody.init()));

		checkEntity(pBody);

		if (!mtx.identity())
			checkOutcome(api_transform_entity(pBody, acisTransf(mtx)));

		if (!setBodyColor(pBody, color))
			return 0;

		CCharPtr sat(saveEntity(pBody));

		obj.setSat(sat.get());
		obj.setType(CDbAcisEntityData::e3DSolid);

		m_cache.insert(obj, pBody.release());

		return 1;
	}
	catch(const exception&)
	{
		return 0;
	}
}

int CAsModeler::createTorus
(
const point& c,
double majRad,
double minRad,
const icl::transf& mtx,
int color,
CDbAcisEntityData& obj
)
{
    if (is_zero(majRad) || is_zero(minRad) ||
		is_negative(majRad) || is_negative(minRad))
        return 0;

	try
	{
		SPAposition center(const_cast<point&>(c));
		CBodyPtr pBody;
		checkOutcome(api_solid_torus(center, majRad, minRad, pBody.init()));

		checkEntity(pBody);

		if (!mtx.identity())
			checkOutcome(api_transform_entity(pBody, acisTransf(mtx)));

		if (!setBodyColor(pBody, color))
			return 0;

		CCharPtr sat(saveEntity(pBody));
		obj.setSat(sat.get());
		obj.setType(CDbAcisEntityData::e3DSolid);

		m_cache.insert(obj, pBody.release());

		return 1;
	}
	catch (const exception&)
	{
		return 0;
	}
}

int CAsModeler::createWedge
(
const point& p1,
const point& p2,
double height,
const icl::transf& mtx,
int color,
CDbAcisEntityData& obj
)
{
	g_errorCode = 0;

	// Check input parameters
    if (is_zero(height))
        return 0;
    if (p1 == p2)
        return 0;

	try
	{
		// Form mesh, from input data
		std::vector<point> points;
		std::vector<int4> faces;
		if (!mesh::wedge(p1, p2, height, points, faces))
			return 0;

		// Create solid, from mesh
		CBodyPtr pBody(cstr::body(points, faces));
		checkEntity(pBody);

		if (!mtx.identity())
			checkOutcome(api_transform_entity(pBody, acisTransf(mtx)));

		if (!setBodyColor(pBody, color))
			return 0;

		CCharPtr sat(saveEntity(pBody));
		obj.setSat(sat.get());
		obj.setType(CDbAcisEntityData::e3DSolid);

		m_cache.insert(obj, pBody.release());

		return 1;
	}
	catch (const exception&)
	{
		return 0;
	}
}

int CAsModeler::createCone
(
const point& c,
double bottomRadius,
const point& p,
double topRadius,
const icl::transf& mtx,
int color,
CDbAcisEntityData& obj
)
{
	g_errorCode = 0;

    if (is_zero(bottomRadius) ||
	   (is_negative(bottomRadius) || is_negative(topRadius)))
        return 0;
	if (c == p)
		return 0;

	try
	{
		CBodyPtr pBody;
		checkOutcome(api_solid_cylinder_cone(asPosition(c),
										     asPosition(p),
											 bottomRadius,
											 bottomRadius,
											 topRadius,
											 0,
											 pBody.init()));
		checkEntity(pBody);

		if (!mtx.identity())
			checkOutcome(api_transform_entity(pBody, acisTransf(mtx)));

		if (!setBodyColor(pBody, color))
			return 0;

		CCharPtr sat(saveEntity(pBody));
		obj.setSat(sat.get());
		obj.setType(CDbAcisEntityData::e3DSolid);

		m_cache.insert(obj, pBody.release());

		return 1;
	}
	catch (const exception&)
	{
		return 0;
	}
}

int CAsModeler::createCone
(
const point& c,
const gvector& a1,
double ratio,
const point& p,
const icl::transf& mtx,
int color,
CDbAcisEntityData& obj
)
{
	g_errorCode = 0;

    if (is_zero(ratio) || is_negative(ratio))
        return 0;
	if (c == p)
		return 0;
	if (a1.norm2() < icl::resabs2())
		return 0;

	try
	{
		SPAposition bottomPos(asPosition(c));
		SPAposition topPos(asPosition(p));
		SPAvector bottomAxis1(asVector(a1));
		SPAposition majorAxisPos = bottomPos + bottomAxis1;

		double bottomRadius1 = bottomAxis1.len();
		double bottomRadius2 = bottomRadius1*ratio;
		double topRadius = 0.;

		CBodyPtr pBody;
		checkOutcome(api_solid_cylinder_cone(bottomPos,
											 topPos,
											 bottomRadius1,
											 bottomRadius2,
											 topRadius,
											 &majorAxisPos,
											 pBody.init()));
		checkEntity(pBody);

		if (!mtx.identity())
			checkOutcome(api_transform_entity(pBody, acisTransf(mtx)));

		if (!setBodyColor(pBody, color))
			return 0;

		CCharPtr sat(saveEntity(pBody));
		obj.setSat(sat.get());
		obj.setType(CDbAcisEntityData::e3DSolid);

		m_cache.insert(obj, pBody.release());

		return 1;
	}
	catch (const exception&)
	{
		return 0;
	}
}

int CAsModeler::createCylinder
(
const point& bottomCenter,
const gvector& majorAxis,
double ratio,
const point& topCenter,
const icl::transf& mtx,
int color,
CDbAcisEntityData& obj
)
{
	g_errorCode = 0;

    if (is_zero(ratio) || is_negative(ratio))
        return 0;
	if (bottomCenter == topCenter)
		return 0;
	if (majorAxis.norm2() < icl::resabs2())
		return 0;

	try
	{
		SPAposition majorAxisPos = asPosition(bottomCenter + majorAxis);
		double bottomRadius1 = majorAxis.norm();
		double bottomRadius2 = bottomRadius1*ratio;
		double topRadius = bottomRadius1;

		CBodyPtr pBody;
		checkOutcome(api_solid_cylinder_cone(asPosition(bottomCenter),
											 asPosition(topCenter),
											 bottomRadius1,
											 bottomRadius2,
											 topRadius,
											 &majorAxisPos,
											 pBody.init()));
		checkEntity(pBody);

		if (!mtx.identity())
			checkOutcome(api_transform_entity(pBody, acisTransf(mtx)));

		if (!setBodyColor(pBody, color))
			return 0;

		CCharPtr sat(saveEntity(pBody));
		obj.setSat(sat.get());
		obj.setType(CDbAcisEntityData::e3DSolid);

		m_cache.insert(obj, pBody.release());

		return 1;
	}
	catch (const exception&)
	{
		return 0;
	}
}

int CAsModeler::createCylinder
(
const point& bottomCenter,
double radius,
const point& topCenter,
const icl::transf& mtx,
int color,
CDbAcisEntityData& obj
)
{
	g_errorCode = 0;

    if (is_zero(radius) || is_negative(radius))
        return 0;
	if (bottomCenter == topCenter)
		return 0;

	try
	{
		CBodyPtr pBody;
		checkOutcome(api_solid_cylinder_cone(asPosition(bottomCenter),
											 asPosition(topCenter),
											 radius,
											 radius,
											 radius,
											 0,
											 pBody.init()));
		checkEntity(pBody);

		if (!mtx.identity())
			checkOutcome(api_transform_entity(pBody, acisTransf(mtx)));

		if (!setBodyColor(pBody, color))
			return 0;

		CCharPtr sat(saveEntity(pBody));
		obj.setSat(sat.get());
		obj.setType(CDbAcisEntityData::e3DSolid);

		m_cache.insert(obj, pBody.release());

		return 1;
	}
	catch (const exception&)
	{
		return 0;
	}
}

int CAsModeler::createDish
(
const point& c,
double radius,
const icl::transf& mtx,
int color,
CDbAcisEntityData& obj
)
{
	g_errorCode = 0;

    if (is_zero(radius) || is_negative(radius))
        return 0;

	try
	{
		SPAunit_vector	latitude(0., radius, 0.);
		SPAunit_vector	longitude(0., 0., -radius);

		static double pi = acos(0.)*2.;
		FACE* pFace;
		checkOutcome(api_make_spface(asPosition(c),
		 						     radius,
									 latitude,
									 longitude,
									 0., 0.5*pi,
									 0., 2*pi,
									 pFace));
		CEntityHolder hFace(pFace);

		pFace->set_sides(DOUBLE_SIDED);
		pFace->set_cont(BOTH_OUTSIDE);

		CBodyPtr pBody;
		checkOutcome(api_sheet_from_ff(1, &pFace, pBody.init()));
		checkEntity(pBody);

		if (!mtx.identity())
			checkOutcome(api_transform_entity(pBody, acisTransf(mtx)));

		if (!setBodyColor(pBody, color))
			return 0;

		CCharPtr sat(saveEntity(pBody));
		obj.setSat(sat.get());
		obj.setType(CDbAcisEntityData::eBody);

		m_cache.insert(obj, pBody.release());

		return 1;
	}
	catch (const exception&)
	{
		return 0;
	}
}

int CAsModeler::createDome
(
const point& c,
double radius,
const icl::transf& mtx,
int color,
CDbAcisEntityData& obj
)
{
	g_errorCode = 0;

    if (is_zero(radius) || is_negative(radius))
        return 0;

	try
	{
		SPAunit_vector	latitude(0., radius, 0.);
		SPAunit_vector	longitude(0., 0., radius);

		static double pi = acos(0.)*2.;
		FACE* pFace;
		checkOutcome(api_make_spface(asPosition(c),
									 radius,
									 latitude,
									 longitude,
									 0., 0.5*pi,
									 0., 2.*pi,
									 pFace));
		CEntityHolder hFace(pFace);

		pFace->set_sides(DOUBLE_SIDED);
		pFace->set_cont(BOTH_OUTSIDE);

		CBodyPtr pBody;
		checkOutcome(api_sheet_from_ff(1, &pFace, pBody.init()));
		checkEntity(pBody);

		if (!mtx.identity())
			checkOutcome(api_transform_entity(pBody, acisTransf(mtx)));

		if (!setBodyColor(pBody, color))
			return 0;

		CCharPtr sat(saveEntity(pBody));
		obj.setSat(sat.get());
		obj.setType(CDbAcisEntityData::eBody);

		m_cache.insert(obj, pBody.release());

		return 1;
	}
	catch (const exception&)
	{
		return 0;
	}
}

int CAsModeler::createPyramid
(
const point& bp1,
const point& bp2,
const point& bp3,
const point& bp4,
const point& tp1,
const point& tp2,
const point& tp3,
const point& tp4,
int mode,
const icl::transf& mtx,
int color,
CDbAcisEntityData& obj
)
{
	g_errorCode = 0;

	try
	{
		std::vector<point> points;
		std::vector<int4> faces;
		switch (mode)
		{
		case 0:
			{
				npoint<4> bottom;
				bottom[0] = bp1; bottom[1] = bp2; bottom[2] = bp3; bottom[3] = bp4;
				npoint<4> top;
				top[0] = tp1; top[1] = tp2; top[2] = tp3; top[3] = tp4;
				mesh::pyramid(bottom, top, points, faces);
			}
			break;
		case 1:
			{
				npoint<4> bottom;
				bottom[0] = bp1; bottom[1] = bp2; bottom[2] = bp3; bottom[3] = bp4;
				npoint<2> top;
				top[0] = tp1; top[1] = tp2;
				mesh::pyramid(bottom, top, points, faces);
			}
			break;
		case 2:
			{
				npoint<3> bottom;
				bottom[0] = bp1; bottom[1] = bp2; bottom[2] = bp3;
				npoint<1> top;
				top[0] = tp1;
				mesh::pyramid(bottom, top, points, faces);
			}
			break;
		case 3:
			{
				npoint<3> bottom;
				bottom[0] = bp1; bottom[1] = bp2; bottom[2] = bp3;
				npoint<3> top;
				top[0] = tp1; top[1] = tp2; top[2] = tp3;
				mesh::pyramid(bottom, top, points, faces);
			}
			break;
		default:
			return 0;
		};

		CBodyPtr pBody = cstr::body(points, faces);
		checkEntity(pBody);

		if (!mtx.identity())
			checkOutcome(api_transform_entity(pBody, acisTransf(mtx)));

		if (!setBodyColor(pBody, color))
			return 0;

		CCharPtr sat(saveEntity(pBody));
		obj.setSat(sat.get());
		obj.setType(CDbAcisEntityData::e3DSolid);

		m_cache.insert(obj, pBody.release());

		return 1;
	}
	catch (const exception&)
	{
		return 0;
	}
}

int CAsModeler::createSurfaceRuled
(
const geodata::curve& curve1,
const geodata::curve& curve2,
int color,
CDbAcisEntityData& obj
)
{
	g_errorCode = 0;

	try
	{
		// Transform curve data into wires
		BODY* wires[2];
		CEntityHolder hwires[2];
		{
			wires[0] = cstr::wire(curve1);
			hwires[0].set(wires[0]);

			wires[1] = cstr::wire(curve2);
			hwires[1].set(wires[1]);
		}

		// Create body
		CBodyPtr pBody;
		{
			checkOutcome(api_skin_wires(2, wires, pBody.init(),
				FALSE, //arc-length
				TRUE,  //twist allowed
				TRUE,  //align directions of wires
				FALSE  //simplify?
				));

			checkOutcome(api_body_to_2d(pBody));
			checkEntity(pBody);
		}

		if (!setBodyColor(pBody, color))
			return 0;

		CCharPtr sat(saveEntity(pBody));
		obj.setSat(sat.get());
		obj.setType(isPlanar(pBody)? CDbAcisEntityData::eRegion: CDbAcisEntityData::eBody);

		m_cache.insert(obj,	pBody.release());

		return 1;
	}
	catch( const exception&)
	{
		return 0;
	}
}

// Sort curves in terms of their end connection and return TRUE the curves form
// a closed loop and FALSE otherwise.
//
// Stephen Hou
//
static 
bool SortCurves(std::vector<geodata::curve*>& crvs)
{
	register n = crvs.size();
	if (n > 1) {
		geodata::curve* pThisCurve = crvs[0];

		icl::point thisPoint, endPoint;			// the current start and end point
		pThisCurve->getEndPoint(thisPoint);

		bool isStart = true;
		geodata::curve* pCurve = NULL;
		for (register j, i = 1; i < n; i++) {
			j = i;

			bool found = false;
			do {
				pCurve = crvs[j];
				pCurve->getStartPoint(endPoint); 
				if (endPoint == thisPoint) {
					isStart = true;
					found = true;				// candidate curve found
				}
				else {
					pCurve->getEndPoint(endPoint);
					if (endPoint == thisPoint) {
						isStart = false;
						found = true;
					}
				}
				if (found) {
					// update the current end point with the end point of the candidate curve
					if (isStart)
						pCurve->getEndPoint(thisPoint);
					else 
						pCurve->getStartPoint(thisPoint);
	
					// swap the SPAposition of the next and candidate curves
					if (j > i) 
						swap(crvs[i], crvs[j]);

					// set candidate curve as current one.
					pThisCurve = crvs[i];
					break;
				}
				j++;
			}
			while (j < n);

			if (!found)
				return false;
		}

		// verify if the loop is closed 
		//
		crvs[0]->getStartPoint(thisPoint);
		if (isStart)
			pCurve->getEndPoint(endPoint);
		else
			pCurve->getStartPoint(endPoint);

		if (thisPoint == endPoint)
			return true;
	}
	return false;
}

int CAsModeler::createSurfaceCurves
(
const std::vector<geodata::curve*>& crvs,
int color,
CDbAcisEntityData& obj
)
{
	g_errorCode = 0;

	if (crvs.end() != find(crvs.begin(), crvs.end(), (geodata::curve*)0))
		return 0;

	std::vector<geodata::curve*> curves(crvs);
	if (!SortCurves(curves))
		return 0;

	try
	{
		// Create wires
		std::vector<BODY*> wires;
		CBodyArrayHolder hwires(wires);
		std::transform(curves.begin(), curves.end(), back_inserter(wires), ptr_fun1r<const geodata::curve,BODY*>(cstr::wire));

		// Covering
		CBodyPtr pBody;
		{
			// Prepare one wire
			checkOutcome(api_body(pBody.init()));
			while (!wires.empty())
			{
				checkOutcome(api_unite(wires.back(), pBody));
				wires.pop_back();
			}

			WIRE* pWire = wire(pBody);
			if (0 == pWire)
				return 0;

			// Cover
			FACE* dumyFace;
			checkOutcome(api_cover_wire(pWire, *reinterpret_cast<surface*>(0), dumyFace));
			// Make sheet double-sided
			checkOutcome(api_body_to_2d(pBody));

			checkEntity(pBody);
		}

		if (!setBodyColor(pBody, color))
			return 0;

		CCharPtr sat(saveEntity(pBody));
		obj.setSat(sat.get());
		obj.setType(CDbAcisEntityData::eBody);

		m_cache.insert(obj, pBody.release());

		return 1;
	}
	catch (const exception&)
	{
		return 0;
	}
}

int CAsModeler::createSurfaceRevolved
(
const geodata::curve& _profile,
const geodata::curve& _axis,
double start,
double value,
int color,
CDbAcisEntityData& obj
)
{
	g_errorCode = 0;

	try
	{
		CBodyPtr profile;
		SPAvector direction;
		SPAposition root;
		{
			profile = cstr::wire(_profile);

			straight axis;
			if (!cstr::line(_axis, axis))
				return 0;

			root = axis.root_point;
			direction = axis.direction;

			if (!is_zero(start))
			{
				SPAtransf r = rotate_transf(start, direction);
				checkOutcome(api_transform_entity(profile, r));
			}
		}

		CBodyPtr pBody;
		{
			sweep_options opt;
			{
				// create double sided sheet
				opt.set_solid(0);
				opt.set_two_sided(TRUE);
				opt.set_sweep_angle(value);
			}

			checkOutcome(api_sweep_with_options(profile, root, direction, &opt, pBody.init()));
			assert(pBody==0);
			pBody = profile;

			checkOutcome(api_body_to_2d(pBody));
			checkEntity(pBody);
		}

		if (!setBodyColor(pBody, color))
			return 0;

		CCharPtr sat(saveEntity(pBody));
		obj.setSat(sat.get());
		obj.setType(isPlanar(pBody)? CDbAcisEntityData::eRegion: CDbAcisEntityData::eBody);

		m_cache.insert(obj, pBody.release());

		return 1;
	}
	catch (const exception&)
	{
		return 0;
	}
}

int CAsModeler::createSurfaceExtruded
(
const geodata::curve& _profile,
const geodata::curve& _path,
int color,
CDbAcisEntityData& obj
)
{
	g_errorCode = 0;

	try
	{
		// Create wires
		CBodyPtr profile;
		CBodyPtr path;
		{
			profile = cstr::wire(_profile);
			path = cstr::wire(_path);

			SPAposition p1 = getStartPoint(path);
			SPAposition p2 = getStartPoint(profile);

			checkOutcome(api_apply_transf(path, translate_transf(p2-p1)));
		}

		// Sweep
		CBodyPtr pBody;
		{
			sweep_options opt;
			{
				//create double sided sheet
				opt.set_solid(0);
				opt.set_two_sided(TRUE);
			}

			checkOutcome(api_sweep_with_options(profile, path, &opt, pBody.init()));
			assert(pBody==0);

			pBody = profile;

			checkOutcome(api_body_to_2d(pBody));
			checkEntity(pBody);
		}

		if (!setBodyColor(pBody, color))
			return 0;

		CCharPtr sat(saveEntity(pBody));
		obj.setSat(sat.get());
		obj.setType(isPlanar(pBody)? CDbAcisEntityData::eRegion: CDbAcisEntityData::eBody);

		m_cache.insert(obj, pBody.release());

		return 1;
	}
	catch (const exception&)
	{
		return 0;
	}
}

int CAsModeler::explodeIntoBodies
(
const CDbAcisEntityData& src,
xstd<CDbAcisEntityData>::vector& objs
)
{
	g_errorCode = 0;

	try
	{
		CAsCacheItem item = m_cache.detach(src);

		std::vector<BODY*> sheets;
		CBodyArrayHolder hsheets(sheets);
		if (!disassembleIntoSheets(item.body(), sheets) || !sheets.size())
			return 0;

		CDbAcisEntityData obj;
		while (!sheets.empty())
		{
			CCharPtr sat(saveEntity(sheets.back()));
			obj.setSat(sat.get());
			obj.setType(isPlanar(sheets.back())? CDbAcisEntityData::eRegion: CDbAcisEntityData::eBody);
			objs.push_back(obj);

			m_cache.insert(obj, sheets.back());
			sheets.pop_back();
		}

		return 1;
	}
	catch (const exception&)
	{
		objs.clear();
		return 0;
	}
}

int CAsModeler::explodeIntoCurves
(
const CDbAcisEntityData& obj,
xstd<geodata::curve*>::vector& crvs
)
{
	g_errorCode = 0;

	try
	{
		CAsCacheItem item = m_cache.detach(obj);

		checkOutcome(api_change_body_trans(item.body(), 0));

		ENTITY_LIST faces;
		checkOutcome(api_get_faces(item.body(), faces));

		faces.init();
		crvs.resize(0);
		for (FACE* pFace = 0; pFace = static_cast<FACE*>(faces.next());)
		{
			if (!disassembleIntoCurves(pFace, crvs))
				throw exception();
		}

		return 1;
	}
	catch (const exception&)
	{
		for_each(crvs.begin(), crvs.end(), ptr_vfun1(&::operator delete));
		crvs.clear();
		return 0;
	}
}

int CAsModeler::boolean
(
const std::vector<CDbAcisEntityData>& objs,
CDbAcisEntityData& result,
eBoolType _opcode
)
{
	g_errorCode = 0;

	assert(objs.size() >= 2);
    if (objs.size() < 2)
        return 0;
	assert(_opcode == eUnite || _opcode == eIntersect);
	if (_opcode != eUnite && _opcode != eIntersect)
		return 0;

	try
	{
		std::vector<CAsCacheItem> items(objs.size());
		for (int i = 0, n = objs.size(); i < n; i++)
		{
			items[i] = m_cache.detach(objs[i]);
		}

		BOOL_TYPE opcode = (BOOL_TYPE)_opcode;

		BODY* pBlank = items[0].body();
		result.setType(objs[0].getType());

		for (i = 1; i < n; i++)
		{
			checkOutcome(api_boolean(items[i].body(), pBlank, opcode));
			items[i].obj()->set(0);//release acis object
			result.setType(typeOfResult(result.getType(), objs[i].getType(), opcode));
		}

		CCharPtr sat(saveEntity(pBlank));
		result.setSat(sat.get());

		m_cache.insert(result, items[0]);

		return 1;
	}
	catch (const exception&)
	{
		return 0;
	}
}

int CAsModeler::unite
(
const std::vector<CDbAcisEntityData>& sats,
CDbAcisEntityData& result
)
{
    return boolean(sats, result, eUnite);
}

int CAsModeler::intersect
(
const std::vector<CDbAcisEntityData>& sats,
CDbAcisEntityData& result
)
{
    return boolean(sats, result, eIntersect);
}

int CAsModeler::subtract
(
const std::vector<CDbAcisEntityData>& blanks,
const std::vector<CDbAcisEntityData>& tools,
xstd<CDbAcisEntityData>::vector& result
)
{
	g_errorCode = 0;

    if (blanks.size() < 1 ||
        tools.size() < 1)
        return 0;

	try
	{
		BODY* pTool;
		checkOutcome(api_body(pTool));
		CBodyHolder hTool(pTool);
		CDbAcisEntityData::EType toolType = CDbAcisEntityData::eUndefined;

		for (int itool = 0, ntool = tools.size(); itool < ntool; itool++)
		{
			CAsCacheItem item = m_cache.detach(tools[itool]);

			checkOutcome(api_unite(item.body(), pTool));
			item.obj()->set(0);//release acis object
			toolType = toolType + tools[itool].getType();
		}

		CDbAcisEntityData obj;
		for (int iblank = 0, nblank = blanks.size(); iblank < nblank; iblank++)
		{
			CAsCacheItem blankItem = m_cache.detach(blanks[iblank]);
			BODY* pBlank = blankItem.body();

			BODY* pToolCopy;
			CBodyHolder hToolCopy;
			{
				if (nblank > 1)
				{
					checkOutcome(api_copy_body(pTool, pToolCopy));
					hToolCopy.set(pToolCopy);
				}
				else
				{
					pToolCopy = static_cast<BODY*>(hTool.release());
				}
			}

			checkOutcome(api_subtract(pToolCopy, pBlank));
			hToolCopy.release();

			if (pBlank->lump())
			{
				CCharPtr sat(saveEntity(pBlank));
				obj.setSat(sat.get());
				obj.setType(blanks[iblank].getType() - toolType);
				result.push_back(obj);

				m_cache.insert(result.back(), blankItem);
			}
			else
			{
				result.push_back(CDbAcisEntityData());
			}
		}

		return 1;
	}
	catch (const exception&)
	{
		result.clear();
		return 0;
	}
}

int CAsModeler::isCurvesConnected
(
bool& bConnected,
const geodata::curve& curve1,
const geodata::curve& curve2
)
{
	g_errorCode = 0;

	try
	{
		// Create wires
		CBodyPtr wires[2];
		{
			wires[0] = cstr::wire(curve1);
			wires[1] = cstr::wire(curve2);
		}

		// Check wire's ends for equality
		{
			bConnected = true;

			SPAposition start1 = getStartPoint(wires[0]);
			SPAposition start2 = getStartPoint(wires[1]);
			if (start1 == start2)
				return 1;

			SPAposition end1 = getEndPoint(wires[0]);
			if (end1 == start2)
				return 1;

			SPAposition end2 = getEndPoint(wires[1]);
			if (start1 == end2 ||
				end1 == end2)
				return 1;

			bConnected = false;
			return 1;
		}
	}
	catch (const exception&)
	{
		return 0;
	}
}

namespace icl
{
    static inline
    void destroy(std::pair<long,long>*){}
}

int CAsModeler::interfere
(
const std::vector<CDbAcisEntityData>& set1,
const std::vector<CDbAcisEntityData>& set2,
xstd<std::pair<long, long> >::vector& pairs,
xstd<CDbAcisEntityData>::vector& out
)
{
	g_errorCode = 0;

	try
	{
		for (int iobj1 = 0, nobj1 = set1.size(); iobj1 < nobj1; iobj1++)
		{
			CAsObj* pObj1 = m_cache[set1[iobj1]];
			BODY* pBody1 = pObj1->get();

			CDbAcisEntityData obj;
			for (int iobj2 = 0, nobj2 = set2.size(); iobj2 < nobj2; iobj2++)
			{
				CAsObj* pObj2 = m_cache[set2[iobj2]];
				BODY* pBody2 = pObj2->get();

				logical touch;
				checkOutcome(api_entity_entity_touch(pBody1, pBody2, touch));
				if (touch == TRUE) {
					CBodyPtr tool;
					checkOutcome(api_copy_body(pBody1, tool.init()));
					CBodyPtr blank;
					checkOutcome(api_copy_body(pBody2, blank.init()));

					checkOutcome(api_intersect(tool, blank));
					tool.release();

					if (blank) {
						if (blank->lump() || blank->wire()) {
							CCharPtr sat(saveEntity(blank));
							obj.setSat(sat.get());
							obj.setType(set1[iobj1].getType() * set2[iobj2].getType());
							out.push_back(obj);
							m_cache.insert(obj, blank.release());
						}
						pairs.push_back(std::pair<long,long>(iobj1, iobj2));
					}
				}
			}
		}
		return 1;
	}
	catch (const exception&)
	{
		out.clear();
		pairs.clear();
		return 0;
	}
}

int CAsModeler::lockDatabase()
{
    m_cache.lock(true);
    return 1;
}

int CAsModeler::unlockDatabase()
{
    m_cache.unlock();
    return 1;
}

int CAsModeler::pickEdge
(
const CDbAcisEntityData& obj,
const point& _root,
const gvector& _dir,
double precision,
int mode,
xstd<HEDGE>::vector& hedges
)
{
	g_errorCode = 0;

    assert(m_cache.lock());

	try
	{
		CAsObj* pObj = m_cache[obj];

		checkOutcome(api_change_body_trans(pObj->get(), 0));

		ENTITY_LIST ents;
		double* params = 0;
		SPAposition rayPoint = asPosition(_root);
		SPAunit_vector rayDirection = normalise(asVector(_dir));
		checkOutcome(api_ray_test_body(rayPoint,
									   rayDirection,
									   precision,
									   0,
									   pObj->get(),
									   ents,
									   params));
		ACIS_FREE(params);
		if (ents.count()) 
		{
			for (int i = 0; i < ents.count(); i++)
			{
				if (is_EDGE(ents[i]))
				{
					EDGE* pEdge = static_cast<EDGE*>(ents[i]);
					if (mode == CModeler::eChain)
					{
						// also select all smoothly connected edges
						ENTITY_LIST edges;
						checkOutcome(api_smooth_edge_seq(pEdge, edges));

						append(ents, edges);
					}
					HEDGE e = reinterpret_cast<HEDGE>(ents[i]);
					if (hedges.end() == find(hedges.begin(), hedges.end(), e))
						hedges.push_back(e);
				}
			}
		}
		else
		{
			// Since api_ray_test_body() only return hit points and hitted entities in forward direction,
			// and the ray point given by IntelliCAD is a through point, we need to perform the test 
			// in the reversed direction if ray doesn't hit the body in forward direction. SWH, 9/29/2004
			//
			checkOutcome(api_ray_test_body(rayPoint,
										   - rayDirection,
										   precision,
										   0,
										   pObj->get(),
										   ents,
										   params));
			ACIS_FREE(params);
			if (ents.count() > 0) 
			{
				for (int i = ents.count(); i >= 0; --i)
				{
					if (is_EDGE(ents[i]))
					{
						EDGE* pEdge = static_cast<EDGE*>(ents[i]);
						if (mode == CModeler::eChain)
						{
							// also select all smoothly connected edges
							ENTITY_LIST edges;
							checkOutcome(api_smooth_edge_seq(pEdge, edges));

							append(ents, edges);
						}
						HEDGE e = reinterpret_cast<HEDGE>(ents[i]);
						if (hedges.end() == find(hedges.begin(), hedges.end(), e))
							hedges.push_back(e);
					}
				}
			}
		}

		return 1;
	}
	catch (const exception&)
	{
		hedges.clear();
		return 0;
	}
}

int CAsModeler::pickLoop
(
const CDbAcisEntityData& obj,
const point& _root,
const gvector& _dir,
double precision,
HFACE hface,
xstd<HEDGE>::vector& hedges
)
{
	g_errorCode = 0;

    assert(m_cache.lock());

	try
	{
		CAsObj* pObj = m_cache[obj];

		checkOutcome(api_change_body_trans(pObj->get(), 0));

		ENTITY_LIST ents;
		double* params = 0;
		SPAposition rayPoint = asPosition(_root);
		SPAunit_vector rayDirection = normalise(asVector(_dir));
		checkOutcome(api_ray_test_body(rayPoint,
									   rayDirection,
									   precision,
									   0,
									   pObj->get(),
									   ents,
									   params));
		ACIS_FREE(params);
		if (ents.count() > 0) {
			for (int i = 0; i < ents.count(); i++)
			{
				if (is_EDGE(ents[i]))
				{
					EDGE* pEdge = static_cast<EDGE*>(ents[i]);
					// An edge can belong to either a wire or a loop of a face - SWH, 9/29/2004
					//
					if (pEdge) {
						ENTITY_LIST edges;
						checkOutcome(api_get_edges(pEdge->coedge()->owner(), edges));

						xstd<EDGE*>::vector& _hedges = reinterpret_cast<xstd<EDGE*>::vector&>(hedges);
						recast1(edges, back_inserter(_hedges));
						return 1;
					}
				}
			}
		}
		else 
		{
			// Since api_ray_test_body() only return hit points and hitted entities in forward direction,
			// and the ray point given by IntelliCAD is a through point, we also need to perform the test 
			// in the reversed direction if ray doesn't hit the body in forward direction. SWH, 9/29/2004
			//
			checkOutcome(api_ray_test_body(rayPoint,
										   - rayDirection,
										   precision,
										   0,
										   pObj->get(),
										   ents,
										   params));
			ACIS_FREE(params);
			for (int i = 0; i < ents.count(); i++)
			{
				if (is_EDGE(ents[i]))
				{
					EDGE* pEdge = static_cast<EDGE*>(ents[i]);
					if (pEdge) 
					{
						ENTITY_LIST edges;
						checkOutcome(api_get_edges(pEdge->coedge()->owner(), edges));

						xstd<EDGE*>::vector& _hedges = reinterpret_cast<xstd<EDGE*>::vector&>(hedges);
						recast1(edges, back_inserter(_hedges));
					}
				}
			}
		}
		return 1;
	}
	catch (const exception&)
	{
		hedges.clear();
		return 0;
	}
}

static
void delete_hit(hit* h)
{
	delete h;
}

int CAsModeler::pickFace
(
const CDbAcisEntityData& obj,
const point& _root,
const gvector& _dir,
double precision,
int mode,
xstd<HFACE>::vector& ahFaces
)
{
	g_errorCode = 0;

    assert(m_cache.lock());

	std::vector<hit*> hits;
	try
	{
		CAsObj* pObj = m_cache[obj];

		checkOutcome(api_change_body_trans(pObj->get(), 0));

		ray arrow = rayBox(pObj->getBoundingBox(),
						   asPosition(_root),
						   normalise(-asVector(_dir)), precision);

		if (raytestFaces(arrow, pObj->get(), hits)) {

			selectHitedFaces(hits, ahFaces, mode);

			for_each(hits.begin(), hits.end(), ptr_vfun1(delete_hit));

			return 1;
		}
		else
			return 0;
	}
	catch (const exception&)
	{
		for_each(hits.begin(), hits.end(), ptr_vfun1(delete_hit));
		ahFaces.clear();
		return 0;
	}
}

int CAsModeler::highlight
(
const CDbAcisEntityData& obj,
int subentity
)
{
	g_errorCode = 0;

	try
	{
		CAsObj* pObj = m_cache[obj];
		pObj->disp().highlight(subentity);
		return 1;
	}
	catch (const exception&)
	{
		return 0;
	}
}

int CAsModeler::unhighlight
(
const CDbAcisEntityData& obj,
int subentity
)
{
	g_errorCode = 0;

	try
	{
		CAsObj* pObj = m_cache[obj];
		pObj->disp().unhighlight(subentity);
		return 1;
	}
	catch (const exception&)
	{
		return 0;
	}
}

int CAsModeler::getFaces
(
const CDbAcisEntityData& obj,
xstd<HFACE>::vector& faces
)
{
	g_errorCode = 0;

	try
	{
		CAsObj* pObj = m_cache[obj];

		ENTITY_LIST el;
		checkOutcome(api_get_faces(pObj->get(), el));
		recast1(el, back_inserter(reinterpret_cast<xstd<FACE*>::vector&>(faces)));

		return 1;
	}
	catch (const exception&)
	{
		return 0;
	}
}

int CAsModeler::getEdges
(
const CDbAcisEntityData& obj,
xstd<HEDGE>::vector& edges
)
{
	g_errorCode = 0;

	try
	{
		CAsObj* pObj = m_cache[obj];

		ENTITY_LIST el;
		checkOutcome(api_get_edges(pObj->get(), el));
		recast1(el, back_inserter(reinterpret_cast<xstd<EDGE*>::vector&>(edges)));

		return 1;
	}
	catch (const exception&)
	{
		edges.clear();
		return 0;
	}
}

int CAsModeler::getEdges
(
HSUBENT hentity,
xstd<HEDGE>::vector& edges
)
{
	g_errorCode = 0;

	assert(m_cache.lock());

	try
	{
		ENTITY* pEntity = reinterpret_cast<ENTITY*>(hentity);

		ENTITY_LIST el;
		checkOutcome(api_get_edges(pEntity, el));
		recast1(el, back_inserter(reinterpret_cast<xstd<EDGE*>::vector&>(edges)));

		return 1;
	}
	catch (const exception&)
	{
		edges.clear();
		return 0;
	}
}

int CAsModeler::moveFaces
(
const CDbAcisEntityData& src,
CDbAcisEntityData& dest,
const std::vector<HFACE>& faces,
const icl::transf& mtx
)
{
	g_errorCode = 0;

    if (faces.empty())
        return 0;

	try
	{
		CAsCacheItem item = m_cache.detach(src);
		BODY* pBody = item.body();

		SPAtransf tr = acisTransf(mtx);
		SPAposition zero = SPAposition(0.,0.,0.);
		FACE** apFaces = reinterpret_cast<FACE**>(const_cast<int*>(faces.begin()));

		checkOutcome(api_move_faces(faces.size(), apFaces, tr, zero, zero));
		checkOutcome(api_change_body_trans(pBody, 0));

		checkEntity(pBody);

		CCharPtr sat(saveEntity(pBody));
		dest.setSat(sat.get());
		dest.setType(src.getType());

		m_cache.insert(dest, item);

		return 1;
	}
	catch (const exception&)
	{
		return 0;
	}
}

int CAsModeler::offsetFaces
(
const CDbAcisEntityData& src,
CDbAcisEntityData& dest,
const std::vector<HFACE>& faces,
double offset
)
{
	g_errorCode = 0;

    if (faces.empty())
        return 0;
	if (is_zero(offset))
		return copy(src, dest);

	try
	{

		CAsCacheItem item = m_cache.detach(src);
		BODY* pBody = item.body();

		SPAposition zero(0., 0., 0.);
		FACE** apFaces = reinterpret_cast<FACE**>(const_cast<int*>(faces.begin()));

		checkOutcome(api_offset_faces(faces.size(), apFaces, offset, zero, zero));

		// we need to check the body becasue in the certain case the local 
		// operation performed by the api funciton above can result an invalid 
		// body, which causes IntelliCAD to crash later. - SWH
		// 			  
		insanity_list *list = NULL;  
		checkOutcome(api_check_entity(pBody, list));  
		if (list) {
			int ne = list->count(ERROR_TYPE); 
			return 0;
		}
		checkOutcome(api_change_body_trans(pBody, 0));

		CCharPtr sat(saveEntity(pBody));
		dest.setSat(sat.get());
		dest.setType(src.getType());

		m_cache.insert(dest, item);

		return 1;
	}
	catch (const exception&)
	{
		return 0;
	}
}

int CAsModeler::taperFaces
(
const CDbAcisEntityData& src,
CDbAcisEntityData& dest,
const std::vector<HFACE>& faces,
const point& _root,
const gvector& _normal,
double angle
)
{
	g_errorCode = 0;

    if (faces.empty())
        return 0;
	if (is_zero(angle))
		return copy(src,dest);

	try
	{
		CAsCacheItem item = m_cache.detach(src);
		BODY* pBody = item.body();

		SPAposition zero(0.,0.,0.);
		SPAposition root = asPosition(_root);
		SPAunit_vector normal(_normal.x(),_normal.y(),_normal.z());
		FACE** apFaces = reinterpret_cast<FACE**>(const_cast<int*>(faces.begin()));

		checkOutcome(api_taper_faces(faces.size(), apFaces, root, normal, angle, zero, zero));
		checkOutcome(api_change_body_trans(pBody, 0));

		CCharPtr sat(saveEntity(pBody));
		dest.setSat(sat.get());
		dest.setType(src.getType());

		m_cache.insert(dest, item);

		return 1;
	}
	catch (const exception&)
	{
		return 0;
	}
}

int CAsModeler::rotateFaces
(
const CDbAcisEntityData& src,
CDbAcisEntityData& dest,
const std::vector<HFACE>& faces,
const point& _root,
const gvector& _dir,
double angle
)
{
	g_errorCode = 0;

    if (faces.empty())
        return 0;
	if (is_zero(angle))
		return copy(src,dest);
	if (_dir.norm2() < resabs2())
		return 0;

	try
	{
		CAsCacheItem item = m_cache.detach(src);

		SPAvector shift = asVector(_root - point(0.,0.,0.));
		SPAtransf tr = translate_transf(-shift) *
					rotate_transf(angle, asVector(_dir)) *
					translate_transf(shift);

		SPAposition zero(0.,0.,0.);
		FACE** apFaces = reinterpret_cast<FACE**>(const_cast<int*>(faces.begin()));

		checkOutcome(api_move_faces(faces.size(), apFaces, tr, zero, zero));

		CCharPtr sat(saveEntity(item.body()));
		dest.setSat(sat.get());
		dest.setType(src.getType());

		m_cache.insert(dest, item);

		return 1;
	}
	catch (const exception&)
	{
		return 0;
	}
}

int CAsModeler::deleteFaces
(
const CDbAcisEntityData& src,
CDbAcisEntityData& dest,
const std::vector<HFACE>& faces
)
{
	g_errorCode = 0;

    if (faces.empty())
        return 0;

	try
	{
		CAsCacheItem item = m_cache.detach(src);

		SPAposition zero(0., 0., 0.);
		FACE** apFaces = reinterpret_cast<FACE**>(const_cast<int*>(faces.begin()));
		checkOutcome(api_remove_faces(faces.size(), apFaces, zero, zero));

		CCharPtr sat(saveEntity(item.body()));
		dest.setSat(sat.get());
		dest.setType(src.getType());

		m_cache.insert(dest, item);

		return 1;
	}
	catch (const exception&)
	{
		return 0;
	}
}

int CAsModeler::copyFaces
(
const CDbAcisEntityData& obj,
const std::vector<HFACE>& faces,
xstd<CDbAcisEntityData>::vector& copies,
const icl::transf& mtx
)
{
	g_errorCode = 0;

    if (faces.empty())
        return 0;

	try
	{
		CAsObj* pObj = m_cache[obj];
		CBodyHolder hobj;
		{
			BODY* pCopy;
			checkOutcome(api_copy_body(pObj->get(), pCopy));
			hobj.set(pObj->get());
			pObj->set(pCopy);
		}

		int i;
		const int n = faces.size();
		CBodyArray hbodies = new CBodyPtr[n];
		for (i = 0; i < n; ++i)
		{
			FACE* face = reinterpret_cast<FACE*>(faces[i]);
			BODY* body;
			checkOutcome(api_unhook_face(face, body));
			hbodies[i] = body;

			checkOutcome(api_transform_entity(body, acisTransf(mtx)));
		}

		copies.resize(0);
		CDbAcisEntityData obj;
		for (i = 0; i < n; ++i)
		{
			BODY* body = static_cast<BODY*>(hbodies[i].get());
			CCharPtr sat(saveEntity(body));
			obj.setSat(sat.get());
			obj.setType(isPlanar(body)? CDbAcisEntityData::eRegion: CDbAcisEntityData::eBody);
			copies.push_back(obj);

			m_cache.insert(obj, hbodies[i].release());
		}

		return 1;
	}
	catch (const exception&)
	{
		copies.clear();
		return 0;
	}
}

/*********************************************************************
 *
 * Color an ACIS body.
 * 
 * Author: Stephen Hou
 * Date: May 12, 2004
 *
 *********************************************************************/
int CAsModeler::colorModelEntity(const CDbAcisEntityData& src,
								 int color,
								 CDbAcisEntityData& dest)
{
	g_errorCode = 0;

	try
	{
		CAsCacheItem item = m_cache.detach(src);
		BODY* pBody = item.body();

		if (!setBodyColor(pBody, color)) 
			return 0;

		//store result
		{
			CCharPtr sat(saveEntity(item.body()));
			dest.setSat(sat.get());
			dest.setType(src.getType());
		}

		m_cache.insert(dest, item);

		return 1;
	}
	catch (const exception&)
	{
		return 0;
	}
}

int CAsModeler::colorFaces
(
const CDbAcisEntityData& src,
CDbAcisEntityData& dest,
const std::vector<HFACE>& faces,
int color
)
{
	g_errorCode = 0;

    if (faces.empty())
        return 0;

	try
	{
		CAsCacheItem item = m_cache.detach(src);

		int i;
		const int n = faces.size();
		// Change color
		for (i = 0; i < n; ++i)
		{
			FACE* pFace = reinterpret_cast<FACE*>(faces[i]);
//			new ATTRIB_COL(pFace, color);
			ATTRIB*	pAttrib = pFace->attrib();

			bool attribFound = false;
			while (pAttrib) {
				if (is_ATTRIB_COL(pAttrib)) {
					attribFound = true;
					break;
				}
				else {
					if (pAttrib->next() != pAttrib)
						pAttrib = pAttrib->next();
					else
						break;
				}
			}
			if (attribFound) 
				static_cast<ATTRIB_COL*>(pAttrib)->set_colour(color);
			else 
				new ATTRIB_COL(pFace, color);

			// we also need to attach the color attribute to the edges because an edge
			// can be shared by mutiple than one faces with different color attributes. The
			// color of edges dislayed on the screen need to be the same as the color of
			// face changed. - SWH.
			//
			ENTITY_LIST edgelist;
			api_get_edges(pFace, edgelist);
			edgelist.init();
			for (ENTITY* pEdge = 0; pEdge = edgelist.next();) {
//				new ATTRIB_COL(pEdge, color);
				ATTRIB*	pAttrib = pEdge->attrib();

				bool attribFound = false;
				while (pAttrib) {
					if (is_ATTRIB_COL(pAttrib)) {
						attribFound = true;
						break;
					}
					else {
						if (pAttrib->next() != pAttrib)
							pAttrib = pAttrib->next();
						else
							break;
					}
				}
				if (attribFound) 
					static_cast<ATTRIB_COL*>(pAttrib)->set_colour(color);
				else 
					new ATTRIB_COL(pEdge, color);
			}
		}

		// Regenerate disply info
		item.obj()->display();
		for (i = 0; i < n; ++i)
		{
			item.obj()->disp().up(faces[i]);
		}

		// Store result
		{
			CCharPtr sat(saveEntity(item.body()));
			dest.setSat(sat.get());
			dest.setType(src.getType());
		}

		m_cache.update(dest, item);

		return 1;
	}
	catch (const exception&)
	{
		return 0;
	}
}

int CAsModeler::colorEdges
(
const CDbAcisEntityData& src,
CDbAcisEntityData& dest,
const std::vector<HEDGE>& edges,
int color
)
{
	g_errorCode = 0;

    if (edges.empty())
        return 0;

	try
	{
		CAsCacheItem item = m_cache.detach(src);

		int i;
		const int n = edges.size();

		// Change color
		for (i = 0; i < n; ++i)
		{
			EDGE* edge = reinterpret_cast<EDGE*>(edges[i]);
			new ATTRIB_COL(edge, color);
		}

		// Regenerate disply info
		item.obj()->display();
		for (i = 0; i < n; ++i)
		{
			item.obj()->disp().up(edges[i]);
		}

		// Save result
		{
			CCharPtr sat(saveEntity(item.body()));
			dest.setSat(sat.get());
			dest.setType(src.getType());
		}
		m_cache.update(dest, item);
		return 1;
	}
	catch (const exception&)
	{
		return 0;
	}
}

int CAsModeler::copyEdges
(
const CDbAcisEntityData& obj,
const std::vector<HEDGE>& edges,
xstd<geodata::curve*>::vector& curves,
const icl::transf& mtx
)
{
	g_errorCode = 0;

    if (edges.empty())
        return 0;

	try
	{
		curves.resize(0);

		//following pointer is not used,
		//but before passge through edges we should be sure, object exists
		CAsObj* pObj = m_cache[obj];

		xstd<HEDGE>::vector::const_iterator iedge = edges.begin();
		for (; iedge != edges.end(); ++iedge)
		{
			EDGE* oldEdge = reinterpret_cast<EDGE*>(*iedge);
			EDGE* newEdge;

			checkOutcome(api_edge(oldEdge, newEdge));
			CEntityHolder hedge(newEdge);

			if (!mtx.identity())
				checkOutcome(api_transform_entity(newEdge, acisTransf(mtx)));

			geodata::curve* crv = extr::curve(newEdge);
			if (!crv)
				continue;

			curves.push_back(crv);
		}

		return 1;
	}
	catch (const exception&)
	{
		for_each_delete(curves.begin(), curves.end());
		curves.clear();
		return 0;
	}
}

int CAsModeler::getNumberOfFaces
(
const CDbAcisEntityData& obj,
int& nfaces
)
{
	g_errorCode = 0;

	try
	{
		CAsObj* pObj = m_cache[obj];

		ENTITY_LIST faces;
		checkOutcome(api_get_faces(pObj->get(), faces));

		nfaces = faces.count();
		return 1;
	}
	catch (const exception&)
	{
		nfaces = 0;
		return 0;
	}
}

int CAsModeler::extrudeFacesAlongPath
(
const CDbAcisEntityData& src,
CDbAcisEntityData& dest,
const std::vector<HFACE>& hfaces,
const geodata::curve& _path
)
{
	g_errorCode = 0;

	try
	{
		CAsCacheItem item = m_cache.detach(src);
		BODY* pBody = item.body();

		CBodyPtr path = cstr::wire(_path);

		std::vector<FACE*> faces = reinterpret_cast<const std::vector<FACE*>&>(hfaces);
		if (!::extrude(faces, path))
			return 0;

		CCharPtr sat(saveEntity(pBody));
		dest.setSat(sat.get());
		dest.setType(src.getType());

		m_cache.insert(dest, item);

		return 1;
	}
	catch (const exception&)
	{
		return 0;
	}
}

int CAsModeler::extrudeFaces
(
const CDbAcisEntityData& src,
CDbAcisEntityData& dest,
const std::vector<HFACE>& hfaces,
double dist,
double taper
)
{
	g_errorCode = 0;

	try
	{
		CAsCache::iterator_t i = m_cache.find(src);

		std::vector<FACE*> faces = reinterpret_cast<const std::vector<FACE*>&>(hfaces);
		if (!::extrude(faces, dist, taper))
			return 0;

		CAsCacheItem item = m_cache.detach(i);

		CCharPtr sat(saveEntity(item.body()));
		dest.setSat(sat.get());
		dest.setType(src.getType());

		m_cache.insert(dest, item);

		return 1;
	}
	catch (const exception&)
	{
		return 0;
	}
}

int CAsModeler::imprint
(
const CDbAcisEntityData& blank,
const std::vector<geodata::curve*>&curves,
CDbAcisEntityData& dest
)
{
	g_errorCode = 0;
	BODY* pTool = NULL;
	try
	{
		CAsCacheItem blankItem = m_cache.detach(blank);
		for (register i = 0; i < curves.size(); i++) {
			pTool = cstr::wire(*curves[i]);
			checkOutcome(api_imprint(pTool, blankItem.body()));
			api_delent(pTool);
			pTool = NULL;
		}

		CCharPtr sat(saveEntity(blankItem.body()));
		dest.setSat(sat.get());
		dest.setType(blank.getType());

		m_cache.insert(dest, blankItem);

		return 1;
	}
	catch (const exception&)
	{
		if (pTool != NULL)
			api_delent(pTool);

		return 0;
	}
}


int CAsModeler::imprint
(
const CDbAcisEntityData& blank,
const CDbAcisEntityData& tool,
CDbAcisEntityData& dest,
bool erase
)
{
	g_errorCode = 0;

	try
	{
		CAsCacheItem blankItem = m_cache.detach(blank);
		CAsCacheItem toolItem;
		BODY* pTool;
		CBodyHolder hTool;
		if (erase)
		{
			toolItem = m_cache.detach(tool);
			pTool = toolItem.body();
		}
		else
		{
			CAsObj* pToolObj = m_cache[tool];
			checkOutcome(api_copy_body(pToolObj->get(), pTool));
			hTool.set(pTool);
		}

		checkOutcome(api_imprint(pTool, blankItem.body()));

		CCharPtr sat(saveEntity(blankItem.body()));
		dest.setSat(sat.get());
		dest.setType(blank.getType());
		
		m_cache.insert(dest, blankItem);

		return 1;
	}
	catch (const exception&)
	{
		return 0;
	}
}

int CAsModeler::separate
(
const CDbAcisEntityData& src,
xstd<CDbAcisEntityData>::vector& dest
)
{
	g_errorCode = 0;

	try
	{
		CAsCache::iterator_t pos = m_cache.find(src);
		BODY* body = pos->second.body();

		int nlump = 0;
		BODY** lumps;
		checkOutcome(api_separate_body(body, nlump, lumps));
		destraction_t<vfun10_t<void*> > free_lumps(ptr_vfun10(acis_free,(void*)lumps));

		CAsCacheItem item = m_cache.detach(pos);
		item.obj()->set(0);

		std::vector<BODY*> alumps;
		std::transform(lumps, lumps+nlump, back_inserter(alumps), ident_t<BODY*>());
		CBodyArrayHolder hlumps(alumps);

		dest.resize(0);
		CDbAcisEntityData obj;
		while (!alumps.empty())
		{
			CCharPtr sat(saveEntity(alumps.back()));
			obj.setSat(sat.get());
			obj.setType(src.getType());
			dest.push_back(obj);

			m_cache.insert(obj, alumps.back());
			alumps.pop_back();
		}
		return 1;
	}
	catch (const exception&)
	{
		dest.clear();
		return 0;
	}
}

int CAsModeler::clean
(
const CDbAcisEntityData& src,
CDbAcisEntityData& dest
)
{
	g_errorCode = 0;

	try
	{
		CAsCacheItem item = m_cache.detach(src);

		checkOutcome(api_clean_body(item.body()));

		CCharPtr sat(saveEntity(item.body()));
		dest.setSat(sat.get());
		dest.setType(src.getType());

		m_cache.insert(dest, item);

		return 1;
	}
	catch (const exception&)
	{
		return 0;
	}
}

int CAsModeler::shell
(
const CDbAcisEntityData& src,
CDbAcisEntityData& dest,
const std::vector<HFACE>& keptFaces,
double thickness
)
{
	g_errorCode = 0;

	try
	{
		CAsCacheItem item = m_cache.detach(src);
		BODY* body = item.body();

		std::vector<FACE*> removedFaces;
		{
			ENTITY_LIST allFaces;
			checkOutcome(api_get_faces(body, allFaces));

			allFaces.init();
			for (FACE* face; face = static_cast<FACE*>(allFaces.next());)
			{
				if (keptFaces.end() == find(keptFaces.begin(), keptFaces.end(), reinterpret_cast<HFACE>(face)))
					removedFaces.push_back(face);
			}
		}

		checkOutcome(api_hollow_body(body,
								     removedFaces.size(),
									 removedFaces.begin(),
									 thickness,
									 SPAposition(0.,0.,0.),
									 SPAposition(0.,0.,0.)));

		// Update sat-data
		{
			CCharPtr sat(saveEntity(body));
			dest.setSat(sat.get());
			dest.setType(src.getType());

			m_cache.insert(dest, item);
		}

		return 1;
	}
	catch (const exception&)
	{
		return 0;
	}
}



// Author:	Stephen W. Hou
// Date:	5/8/2003
//

class AsLoop : public std::list<geodata::curve*>
{
public:

	AsLoop() : m_pNormal(NULL), m_ErrorCode(1) {}
	virtual ~AsLoop() { delete m_pNormal; }

	void			setNormal(double x, double y, double z);
	SPAvector*		normal() const { return m_pNormal; }

	BODY*			toRegion();
	BODY*			toWire();

	int				getLastError() const { return m_ErrorCode; }

private:

	int				m_ErrorCode;
	SPAvector*		m_pNormal;
};


void AsLoop::setNormal(double x, double y, double z)
{
	if (!m_pNormal)
		m_pNormal = new SPAvector(x, y, z);
	else {
		m_pNormal->set_x(x);
		m_pNormal->set_y(y);
		m_pNormal->set_z(z);
	}
}


BODY* AsLoop::toRegion()
{
	BODY* pRegion = NULL;
	BODY* pBody = toWire();
	if (pBody != NULL) {
		m_ErrorCode = cstr::region(pBody/*, *m_pNormal*/);
		if (m_ErrorCode > 0) 
			pRegion = pBody;
	}
	return pRegion;
}
	

BODY* AsLoop::toWire() 
{
	BODY* pWire= NULL;
	EDGE** pEdges = NULL;
	try {
		// convert all curves to ACIS edges
		//
		std::vector<EDGE*> edges;
		geodata::curve* pCurve = NULL;
		std::list<geodata::curve*>::const_iterator  theIterator;
		for (theIterator = begin(); theIterator != end(); ++theIterator) {
			pCurve = *theIterator;

			std::vector<EDGE*> edgeArray;
			cstr::edges(*pCurve, edgeArray);

			for (register i = 0; i < edgeArray.size(); i++)
				edges.push_back(edgeArray[i]);
		}
		assert(edges.size() > 0);

		// create an ACIS wire
		//
		SPAposition start, end;
		pEdges = new EDGE* [edges.size()];
		for (register i = 0; i < edges.size(); i++) {
			pEdges[i] = static_cast<EDGE*>(edges[i]);
			start = pEdges[i]->start_pos();
			end = pEdges[i]->end_pos();
		}
		
		checkOutcome(api_make_ewire(edges.size(), pEdges, pWire));
		delete [] pEdges;
	}
	catch (const exception&) {
		if (pEdges)
			delete [] pEdges;
	}
	return pWire;
}


// To find a candidate loop of edges
//
// Author:	Stephen W. Hou
// Date:	5/7/2003

bool FindCandidateLoop(AsLoop &candidateLoop,
					   std::list<geodata::curve*> &candidateCurves,
					   const icl::point &loopStart,
					   const icl::point &currPosition)
{
	// find all adjcent curves and move them to a new list
	//
	icl::point start, end;
	geodata::curve* pCurrCurve = NULL; 
	std::list<geodata::curve*> adjcentCurves;
	std::list<geodata::curve*>::iterator theIterator = candidateCurves.begin();
	while (theIterator != candidateCurves.end()) {
		pCurrCurve = *theIterator;
		if (pCurrCurve->getStartPoint(start) && pCurrCurve->getEndPoint(end)) {
			if (currPosition == start || currPosition == end) {
				adjcentCurves.push_back(pCurrCurve);
				theIterator = candidateCurves.erase(theIterator);
			}
			else
				theIterator++;
		}
	}
	bool loopFound = false;
	if (adjcentCurves.size()) {
		// loop through all adjcent curves to test if they and their successor 
		// belong to the loop. The adjcent curve is removed from candidate curves
		// list when it is processed.
		//
		icl::point thisPosition;
		theIterator = adjcentCurves.begin();
		while (theIterator != adjcentCurves.end()) {
			pCurrCurve = *theIterator;
			if (pCurrCurve->getStartPoint(start) && pCurrCurve->getEndPoint(end)) {
				thisPosition = (start != currPosition) ? start : end;
				if (thisPosition == loopStart) { // the last curve of the loop is found
					candidateLoop.push_back(pCurrCurve);
					loopFound = true;
					break;
				}
				else {
					if (FindCandidateLoop(candidateLoop, candidateCurves, loopStart, thisPosition)) {
						candidateLoop.push_back(pCurrCurve);
						loopFound = true;
						break;
					}
				}
			}
			theIterator++;
		}
		// Now we need tp put back all adjcent curves back to the candidate 
		// curve list if no one belongs to the loop or all curves except the 
		// one belinging to the loop to the candidate curve list.
		//
		geodata::curve* pCurve = NULL; 
		for (theIterator = adjcentCurves.begin(); theIterator != adjcentCurves.end(); 
			++theIterator) {
			pCurve = *theIterator;
			if (pCurve != pCurrCurve) 
				candidateCurves.push_back(pCurve);
			else {
				if (!loopFound)
					candidateCurves.push_back(pCurve);
			}
		}
	}
	return loopFound;
}


// To find all loops of edges
//
// Author:	Stephen W. Hou
// Date:	5/7/2003

void FindAllLoops(const std::vector<geodata::curve*>& curves,
				  std::vector<AsLoop*> &loops)
{
	// form loops from all closed edges
	//
	std::list<geodata::curve*> openCurves;

	icl::gvector normal;
	geodata::curve* pCurve = NULL;
	AsLoop* pCandidateLoop = NULL;
	for (register i = 0; i < curves.size(); i++) {
		pCurve = curves[i];
		if (pCurve->isClosed()) {
			pCandidateLoop = new AsLoop;
			pCandidateLoop->push_back(pCurve);
			if (pCurve->getNormal(normal))
				pCandidateLoop->setNormal(normal.x(), normal.y(), normal.z());
			loops.push_back(pCandidateLoop);
		}
		else 
			openCurves.push_back(pCurve);
	}

	// form loops from open edges
	//
	if (openCurves.size()) {
		bool foundLoop;
		std::list<geodata::curve*>::iterator lIterator;
		do {
			foundLoop = false;
			pCandidateLoop = NULL;
			lIterator = openCurves.begin(); 
			while (lIterator != openCurves.end()) {
				pCurve = *lIterator;
				if (!pCandidateLoop)
					pCandidateLoop = new AsLoop;

				icl::point start, end;
				if (pCurve->getStartPoint(start) && pCurve->getEndPoint(end)) {
					// remove this curve first and add it to the loop if it belong
					// to a loop. Otherwise, it should be a dangling curve.
					//
					lIterator = openCurves.erase(lIterator);
					if (FindCandidateLoop(*pCandidateLoop, openCurves, start, end)) {
						// set normal if the curve has. When a loop is consistent of
						// more than one curve, we will arbitrarily use one as the normal
						// of the loop. That is, the normal of the resultant region will
						// be arbitrary.
						//
						if (pCurve->getNormal(normal))
							pCandidateLoop->setNormal(normal.x(), normal.y(), normal.z());

						pCandidateLoop->push_back(pCurve);
						pCandidateLoop->reverse();
						loops.push_back(pCandidateLoop);
						foundLoop = true;
						break;
					}
				}
				else
					lIterator++;
			}
			// delete the last candidate loop which is not closed
			if (!foundLoop) 
				delete pCandidateLoop;

		} while (foundLoop);
	}
}


// Construct a region from a single closed planar curve.
//
// Author:	Stephen W. Hou
// Date:	5/9/2003

int CAsModeler::region(const geodata::curve* pCurve, 
					   int color,
					   CDbAcisEntityData &region)
{
	int result = 0;
	if (!pCurve || !pCurve->isClosed())
		return result;

	try {
		gvector normal;
		if (pCurve->getNormal(normal)) {
			BODY* pBody = cstr::wire(*pCurve);
			// specify the second parameter to create single side region.
			//
			result = cstr::region(pBody, SPAvector(normal.x(), normal.y(), normal.z()));
			if (result > 0) {
				if (!setBodyColor(pBody, color))
					return 0;

				CCharPtr sat(saveEntity(pBody));
				region.setSat(sat.get());
				region.setType(CDbAcisEntityData::eRegion);
				m_cache.insert(region, pBody);
			}
		}
		return result;
	}
	catch (const exception&) {
		return result;
	}
}


// Creates a set of regions from given curves, which could be either open or
// closed but they must form closed loops. Only closed planar loops without
// self-intersections will create the regions. Only qualified curves that form
// closed loops are used to create the regions.
//
// Author:	Stephen W. Hou
// Date:	5/9/2003
//
int CAsModeler::region(const std::vector<geodata::curve*>& curves,
					   xstd<CDbAcisEntityData>::vector& aregions,
					   int color,
					   std::list<geodata::curve*> &qualifiedCurves)
{
	if (curves.empty())
		return 0;

	try {
		std::vector<AsLoop*> loops;
		FindAllLoops(curves, loops);

		ENTITY_LIST regions;
		if (loops.size() > 0) {
			for (register i = 0; i < loops.size(); i++) {
				AsLoop* pLoop = (AsLoop*)loops[i];
				BODY* pRegion = pLoop->toRegion();
				if (pRegion && setBodyColor(pRegion, color))
					regions.add(pRegion);
				
				geodata::curve* pCurve = NULL;
				std::list<geodata::curve*>::const_iterator  theIterator;
				for (theIterator = pLoop->begin(); theIterator != pLoop->end(); ++theIterator) {
					pCurve = *theIterator;
					qualifiedCurves.push_back(pCurve);
				}

				delete pLoop;
			}
			loops.clear();
		}
		else 
			return 0;

		if (regions.count() == 0) // no region is created
			return 0;

		aregions.reserve(regions.count());
		CDbAcisEntityData obj;
		for (register i = 0; i < regions.count(); i++) {
			CCharPtr sat(saveEntity((BODY*)regions[i]));
			obj.setSat(sat.get());
			obj.setType(CDbAcisEntityData::eRegion);
			aregions.push_back(obj);

			m_cache.insert(obj, (BODY*)regions[i]);
		}

		return 1;
	}
	catch (const exception&)
	{
		aregions.clear();
		return 0;
	}
}

int CAsModeler::check
(
const CDbAcisEntityData& obj,
xstd<icl::string>::vector& info
)
{
	g_errorCode = 0;

	const char* tempdir;
	if ((0 == (tempdir = getenv("temp"))) &&
		(0 == (tempdir = getenv("tmp"))))
		return 0;

	char filename[_MAX_PATH];
	strcpy(filename, tempdir);
	strcat(filename, tmpnam(0));
	strcat(filename, "$ic");

	try
	{
		{
			CAsObj* pObj = m_cache[obj];
			CFilePtr of(fopen(filename, "w+"));
			if (!of)
				throw exception();

			ENTITY_LIST pel;
			checkOutcome(api_check_entity(pObj->get(), &pel, of));

			if (fseek(of, SEEK_SET, 0))
				throw exception();

			info.push_back("");
			int c;
			while((c = fgetc(of)) != EOF)
			{
				info.back() += c;
				if (c == '\n')
					info.push_back("");
			}
			if (info.back().empty())
				info.pop_back();
		}

		remove(filename);
		return 1;
	}
	catch (const exception&)
	{
		info.clear();
		remove(filename);
		return 0;
	}
}

int CAsModeler::section
(
const CDbAcisEntityData& solid,
CDbAcisEntityData& region,
const icl::point& root,
const icl::gvector& normal
)
{
	g_errorCode = 0;

	try
	{
		CAsObj* pObj = m_cache[solid];

		CBodyPtr pCopy;
		checkOutcome(api_copy_body(pObj->get(), pCopy.init()));

		CBodyPtr pTool = createInfiniteSheet(root, normal);

		checkOutcome(api_intersect(pTool, pCopy));
		pTool.release();

		CCharPtr sat(saveEntity(pCopy));
		region.setSat(sat.get());
		region.setType(CDbAcisEntityData::eRegion);

		m_cache.insert(region, pCopy.release());

		return 1;
	}
	catch (const exception&)
	{
		return 0;
	}
}

int CAsModeler::slice
(
const CDbAcisEntityData& obj,
CDbAcisEntityData parts[2],
const icl::point& root,
const icl::gvector& normal,
const icl::point* ptest
)
{
	g_errorCode = 0;

	try
	{
		//get blank
		CAsCache::iterator_t it = m_cache.find(obj);
		BODY* pBlank = it->second.body();

		//create tool
		if (ptest)
		{
			//we need side, where test point lies
			CBodyPtr pTool = createHalfspace(root, normal, *ptest);

			//slice the blank
			checkOutcome(api_intersect(pTool, pBlank));
			pTool.release();
			CAsCacheItem item = m_cache.detach(it);

			if (!pBlank->lump())
			{
				parts[0].clear();
				parts[1].clear();
				return 1;
			}

			//store result
			{
				CCharPtr sat(saveEntity(pBlank));
				parts[0].setSat(sat.get());
				parts[0].setType(isPlanar(pBlank)?
									CDbAcisEntityData::eRegion:
									obj.getType()*CDbAcisEntityData::e3DSolid);
				parts[1].clear();
			}

			//update cache
			m_cache.insert(parts[0], item);

			return 1;
		}
		else
		{
			BODY* tools[2];
			//we need side, where test point lies
			createHalfspaces(tools[0], tools[1], root, normal);
			CBodyHolder htools[2] = {tools[0], tools[1]};

			BODY* blanks[2] = {pBlank, 0};
			checkOutcome(api_copy_body(pBlank, blanks[1]));
			CBodyHolder hblank1 = blanks[1];
			CAsCacheItem item;

			for (int i = 0; i < 2; ++i)
			{
				//slice the blank
				checkOutcome(api_intersect(tools[i], blanks[i]));
				htools[i].release();
				if (0 == i)
					item = m_cache.detach(it);

				if (!blanks[i]->lump())
				{
					parts[i].clear();
				}
				else
				{
					//store result
					CCharPtr sat(saveEntity(blanks[i]));
					parts[i].setSat(sat.get());
					parts[i].setType(isPlanar(blanks[i])?
									CDbAcisEntityData::eRegion:
									obj.getType()*CDbAcisEntityData::e3DSolid);
				}
			}

			//update cache
			m_cache.insert(parts[0], item);
			m_cache.insert(parts[1], hblank1.release());

			return 1;
		}
	}
	catch (const exception&)
	{
		parts[0].clear();
		parts[1].clear();
		return 0;
	}
}

int CAsModeler::extrude
(
const CDbAcisEntityData& src,
CDbAcisEntityData& dest,
double dist,
double taper,
int color
)
{
	g_errorCode = 0;

	try
	{
		CAsCache::iterator_t it = m_cache.find(src);
		BODY* pRegion = it->second.body();

		if (!isPlanar(pRegion))
			return 0;

		std::vector<FACE*> afaces;
		{
			ENTITY_LIST faces;
			checkOutcome(api_get_faces(pRegion, faces));
			recast<FACE>(faces, afaces);
		}

		if (!::extrude(afaces, dist, taper))
			return 0;
		
		if (!setBodyColor(pRegion, color))
			return 0;

		CAsCacheItem item = m_cache.detach(it);

		// create sat-data from new body
		CCharPtr sat(saveEntity(pRegion));
		dest.setSat(sat.get());
		dest.setType(CDbAcisEntityData::e3DSolid);

		m_cache.insert(dest, item);

		return 1;
	}
	catch (const exception&)
	{
		return 0;
	}
}

int CAsModeler::extrude
(
const CDbAcisEntityData& src,
CDbAcisEntityData& dest,
const geodata::curve& _path,
int color
)
{
	g_errorCode = 0;

	try
	{
		CAsCache::iterator_t it = m_cache.find(src);
		BODY* pRegion = it->second.body();

		if (!isPlanar(pRegion))
			return 0;

		CBodyPtr path = cstr::wire(_path);

		std::vector<FACE*> afaces;
		{
			ENTITY_LIST lfaces;
			checkOutcome(api_get_faces(pRegion, lfaces));
			recast<FACE>(lfaces, afaces);
		}
		if (!::extrude(afaces, path))
			return 0;

		if (!setBodyColor(pRegion, color))
			return 0;

		CAsCacheItem item = m_cache.detach(it);

		// create sat-data from new body
		CCharPtr sat(saveEntity(pRegion));
		dest.setSat(sat.get());
		dest.setType(CDbAcisEntityData::e3DSolid);

		m_cache.insert(dest, item);

		return 1;
	}
	catch (const exception&)
	{
		return 0;
	}
}

int CAsModeler::revolve
(
const CDbAcisEntityData& src,
CDbAcisEntityData& dest,
const icl::point& _root,
const icl::gvector& _dir,
double angle,
int color
)
{
	g_errorCode = 0;

	try
	{
		CAsCache::iterator_t it = m_cache.find(src);
		BODY* pRegion = it->second.body();

		if (!isPlanar(pRegion))
			return 0;

		SPAposition root = asPosition(_root);
		SPAvector dir = asVector(_dir);
		{
			PLANE* p = static_cast<PLANE*>(face_surface(pRegion));
			const SPAposition& rp = p->root_point();
			const SPAunit_vector& n = p->normal();

			root = 	proj_pt_to_plane(root, rp, n);

			SPAposition d(dir.x(), dir.y(), dir.z());
			SPAposition z(0., 0., 0.);
			dir = proj_pt_to_plane(d, rp, n) - proj_pt_to_plane(z, rp, n);
		}

		sweep_options opt;
		{
			opt.set_solid(1);
			opt.set_two_sided(FALSE);
			opt.set_sweep_angle(angle);
		}

		BODY* pSolid = 0;
		checkOutcome(api_sweep_with_options(pRegion, root, dir, &opt, pSolid));
		assert(pSolid == 0);

		if (!setBodyColor(pRegion, color))
			return 0;

		CAsCacheItem item = m_cache.detach(it);

		// create sat-data from new body
		CCharPtr sat(saveEntity(pRegion));
		dest.setSat(sat.get());
		dest.setType(CDbAcisEntityData::e3DSolid);

		m_cache.insert(dest, item);

		return 1;
	}
	catch (const exception&)
	{
		return 0;
	}
}

int CAsModeler::getArea
(
const CDbAcisEntityData& obj,
double& area
)
{
	g_errorCode = 0;

	try
	{
		CAsObj* pObj = m_cache[obj];
		BODY* body = pObj->get();

		double relprec = 0.01;
		double absprec = 0.0001;
		double achivedprec;
		int rc = 1;
		int i = 0;
		while (rc && i++ < 5)
		{
			checkOutcome(api_ent_area(body, relprec, area, achivedprec));
			if (achivedprec*area <= absprec)
				break;
			relprec = absprec/area;
		}
		return i <= 5;
	}
	catch (const exception&)
	{
		return 0;
	}
}

int CAsModeler::getSatInVersion
(
const CDbAcisEntityData& src,
icl::string& sat,
int ver
)
{
	g_errorCode = 0;

	try
	{
		int stored_major, stored_minor;
		checkOutcome(api_get_save_version(stored_major, stored_minor));

		CAsObj* pObj = m_cache[src];

		int major = ver/100;
		int minor = ver - major*100;
		checkOutcome(api_save_version(major, minor));

		CCharPtr _sat(saveEntity(pObj->get()));
		sat = _sat;

		checkOutcome(api_save_version(stored_major, stored_minor));

		return 1;
	}
	catch (const exception&)
	{
		sat.erase();
		return 0;
	}
}

int CAsModeler::getMassProperties
(
const std::vector<CDbAcisEntityData>& objs,
double& volume,
icl::point& center_of_gravity,
icl::ntype<double,3>& moments_of_inertia,
icl::ntype<double,3>& products_of_inertia,
icl::ntype<double,3>& radii_of_gyration,
icl::ntype<double,3>& principal_moments_of_inertia,
icl::ntype<icl::gvector,3>& principal_axes_of_inertia
)
{
	g_errorCode = 0;

	try
	{
		CBodyPtr combination;
		checkOutcome(api_body(combination.init()));

		for (int i = 0, n = objs.size(); i < n; i++)
		{
			CAsObj* pObj = m_cache[objs[i]];

			BODY* body;
			checkOutcome(api_copy_body(pObj->get(), body));

			checkOutcome(api_combine_body(body, combination));
		}

		SPAbox bb = get_body_box(combination);

		SPAposition root = bb.mid();
		SPAunit_vector normal(0.,0.,1.);
		SPAposition cofg;
		int selector = 0;//calculate all
		double rel_prec = 0.01;
		tensor inertia;
		double moments[3];
		SPAunit_vector axes[3];
		double ach_prec;

		checkOutcome(api_body_mass_pr(combination, root, normal,
					selector, rel_prec, volume,
					cofg, inertia, moments, axes, ach_prec));

		center_of_gravity = point(cofg.x(), cofg.y(), cofg.z());
		{
			moments_of_inertia[0] = inertia.element(0,0);
			moments_of_inertia[1] = inertia.element(1,1);
			moments_of_inertia[2] = inertia.element(2,2);
		}
		{
			products_of_inertia[0] = inertia.element(0,1);
			products_of_inertia[1] = inertia.element(1,2);
			products_of_inertia[2] = inertia.element(0,2);
		}
		principal_moments_of_inertia = moments;
		{
			principal_axes_of_inertia[0] = gvector(axes[0].x(), axes[0].y(), axes[0].z());
			principal_axes_of_inertia[1] = gvector(axes[1].x(), axes[1].y(), axes[1].z());
			principal_axes_of_inertia[2] = gvector(axes[2].x(), axes[2].y(), axes[2].z());
		}
		{
			if (!icadIsZero(volume)) {
				double rm = 1/volume;
				radii_of_gyration[0] = sqrt(moments_of_inertia[0]*rm);
				radii_of_gyration[1] = sqrt(moments_of_inertia[1]*rm);
				radii_of_gyration[2] = sqrt(moments_of_inertia[2]*rm);
			}
		}

		return 1;
	}
	catch (const exception&)
	{
		return 0;
	}
}

int CAsModeler::reassembleFromMesh
(
CDbAcisEntityData& data,
const std::vector<point>& points,
const std::vector<int>& faces
)
{
	g_errorCode = 0;

	try
	{
		//check mesh validity
		const int npoint = points.size();
		if (find_if(faces.begin(), faces.end(), bind2nd(greater<double>(), npoint)) != faces.end() ||
			find_if(faces.begin(), faces.end(), bind2nd(less<double>(), -npoint)) != faces.end())
			//there is incorrect vertex index
			return 0;

		const int nfaces = faces.size() / 4;
		if (faces.size() != nfaces*4)
			return 0;

		std::vector<int4> faces4;
		for (std::vector<int>::const_iterator i = faces.begin(); i != faces.end();)
		{
			faces4.push_back(int4(*reinterpret_cast<const int(*)[4]>(i)));
			i += 4;
		}

		CBodyPtr pBody(cstr::body(points, faces4));

		CCharPtr sat(saveEntity(pBody));
		data.setSat(sat.get());
		data.setType(CDbAcisEntityData::e3DSolid);

		m_cache.insert(data, pBody.release());

		return 1;
	}
	catch (const exception&)
	{
		return 0;
	}
}

struct CUtDouble
{
	double m_value;

	CUtDouble(){}

	CUtDouble(const CUtDouble& d)
	:m_value(d.m_value){}

	CUtDouble(double d)
	:m_value(d){}

	bool operator < (const CUtDouble& e) const
	{
		return icl::lt(m_value, e.m_value, icl::resabs());
	}
};

int CAsModeler::fillet
(
const CDbAcisEntityData& oldData,
CDbAcisEntityData& newData,
int color,
const std::vector<std::pair<HEDGE, double> >& edges_with_radius
)
{
	g_errorCode = 0;

	try
	{
		// sort edges by corresponding fillet radius
		std::vector<std::pair<HEDGE, double> >::const_iterator iedge;
		std::map<CUtDouble, std::list<EDGE*> > radius_with_edges;
		for (iedge = edges_with_radius.begin();
			 iedge != edges_with_radius.end(); ++iedge)
		{
			std::list<EDGE*>& edges = radius_with_edges[iedge->second];
			edges.push_back(reinterpret_cast<EDGE*>(iedge->first));
		}

		CAsCache::iterator_t it = m_cache.find(oldData);
		BODY* pObj = it->second.body();

		std::map<CUtDouble, std::list<EDGE*> >::const_iterator iradius;
		for (iradius = radius_with_edges.begin();
			 iradius != radius_with_edges.end(); ++iradius)
		{
			// transfere corresponded edges into ENTITY_LIST
			ENTITY_LIST edges;
			std::list<EDGE*>::const_iterator iedge;
			for (iedge = iradius->second.begin();
				 iedge != iradius->second.end(); ++iedge)
			{
				edges.add(*iedge);
			}

			// before calling modeler, check existance of the edges
			ENTITY_LIST all_edges;
			checkOutcome(api_get_edges(pObj, all_edges));

			edges.init();
			for (ENTITY* e = 0; e = edges.next();)
			{
				if (find(all_edges, e) == -1)
				{
					break;
				}
			}
			if (!e)
			{
				// all edges found, call modeler
				checkOutcome(api_blend_edges(edges, iradius->first.m_value));
			}
		}

		if (!setColorToNewFaces(pObj, color))
			return 0;

		CAsCacheItem item = m_cache.detach(it);

		//store result
		{
			CCharPtr sat(saveEntity(pObj));
			newData.setSat(sat.get());
			newData.setType(newData.getType());
		}

		//update cache
		m_cache.insert(newData, item);

		return 1;
	}
	catch (const exception&)
	{
		return 0;
	}
}

int CAsModeler::isEdgeAppropriateForBlending
(
const CDbAcisEntityData&,
HEDGE hedge,
bool& isAppropriate
)
{
	EDGE* pEdge = reinterpret_cast<EDGE*>(hedge);
	if (is_EDGE(pEdge))
	{
		isAppropriate = true;
	    ATTRIB*	pAttrib = pEdge->attrib();
	    while (pAttrib) 
		{
		    if (is_ATTRIB_BLEND(pAttrib))
				isAppropriate = false;
			else {
				if (pAttrib->next() != pAttrib)
					pAttrib = pAttrib->next();
				else
					break;
			}
		}
		return 1;
	}
	return 0;
}


int CAsModeler::chamfer
(
const CDbAcisEntityData& oldData,
CDbAcisEntityData& newData,
const std::vector<HEDGE>& chamferEdges,
double baseChamferDistance,
double otherChamferDistance,
int color
)
{
	g_errorCode = 0;

	try
	{
		const std::vector<EDGE*>& _chamferEdges = reinterpret_cast<
			const std::vector<EDGE*>& >(chamferEdges);
		ENTITY_LIST edges;
		recast<EDGE>(_chamferEdges, edges);

		CAsCache::iterator_t it = m_cache.find(oldData);
		BODY* pObj = it->second.body();

		ENTITY_LIST e1;
		api_get_edges(pObj, e1);

		std::vector<EDGE*>::const_iterator ie = _chamferEdges.begin();
		for (; ie != _chamferEdges.end(); ++ie)
			assert(find(e1, *ie) != -1);


		// all edges found, call modeler
		checkOutcome(api_chamfer_edges(edges,
									   baseChamferDistance,
									   otherChamferDistance));

		if (!setColorToNewFaces(pObj, color))
			return 0;

		//store result
		CCharPtr sat(saveEntity(pObj));
		newData.setSat(sat.get());
		newData.setType(newData.getType());

		//update cache
		CAsCacheItem item = m_cache.detach(it);
		m_cache.insert(newData, item);

		return 1;
	}
	catch (const exception&)
	{
		return 0;
	}
}


#endif //#ifndef BUILD_WITH_LIMITED_ACIS_SUPPORT
