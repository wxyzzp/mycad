// File  : 
// Author: Alexey Malov
#include "stdafx.h"
#include "AsTools.h"
#undef T
#include "xstd.h"
#include "AsMesh.h"
#include "AsExtractor.h"
#include "AsConstructor.h"
#include "matrix.h"
#include "transf.h"
#include "AsAutoPtr.h"
#include "gvector.h"
using icl::gvector;
using icl::point;
#include "geomutil.hxx"
#include <algorithm>
using std::find;
using std::sort;
#include "AsAttribMarker.h"
#include "AsSatStream.h"
#include "AsDisplayObject.h"

#ifdef _TEST
#include "testunit/assert.h"
#else
#include <cassert>
#endif


const int BYLAYER_COLOR = 256;

err_mess_type g_errorCode = 0;

Map<rgb_color, int> ColorMapCache;


/******************************************************************
 * 
 * Find the closest color.
 *
 * Author: Stephen Hou
 * Date: June 6, 2004
 *
 ******************************************************************/
int findClosestColorIndex(const RGBQUAD* pCurrPaletteColors,
                          double rgbRed, double rgbGreen, double rgbBlue)
{
    int color = BYLAYER_COLOR;
    double rgbDiff, minRGBDiff = 256.;
    double r = rgbRed * 255.;
    double g = rgbGreen * 255.;
    double b = rgbBlue * 255.;
    for (register i = 0; i < 256; i++) {
        rgbDiff = (fabs(r - (double)pCurrPaletteColors[i].rgbRed) +
                   fabs(g - (double)pCurrPaletteColors[i].rgbGreen) +
                   fabs(b - (double)pCurrPaletteColors[i].rgbBlue)) / 3.0;
        if (rgbDiff < minRGBDiff) {
            minRGBDiff = rgbDiff;
            color = i + 1;
        }
    }
    return color;
}

bool _checkOutcome(const outcome& result) throw (exception)
{
	g_errorCode = result.error_number();
    if (!result.ok())
    {
#ifdef _DEBUG
        DUMP2("Internal modeler error %d - %s\n", g_errorCode,
                                                  find_err_mess(g_errorCode));
#endif
		throw exception();
    }

    return true;
}


void checkEntity(ENTITY* e) throw (exception)
{
	ENTITY_LIST pel;
	FILE* f = 0;

#if 0
	CFilePtr _f(fopen("checkentity.txt", "w")); f = _f;
#endif

	checkOutcome(api_check_entity(e, &pel, f));

	if (pel.count())
	{
		throw exception();
	}
}

char* saveEntity(ENTITY* e) throw (exception)
{
	CAsSatStream osat;
	ENTITY_LIST el; el.add(e);
	checkOutcome(api_save_entity_list_file(&osat, el));
	osat.write_char('\x0');

	return osat.data();
}



/******************************************************************
 * 
 * Set the color and the faces and edges of an ACIS body if they don't
 * have color attributes attached and convert all RGB color values to 
 * color index.
 *
 * Author: Stephen Hou
 * Date: June 6, 2004
 *
 ******************************************************************/
bool setBodyColor(BODY* pBody, int currColor, const RGBQUAD* pCurrPaletteColors)
{
	bool success = true;
	try {
		ATTRIB*	pAttrib = NULL;
		ENTITY_LIST faces, edges;
		checkOutcome(api_get_faces(pBody, faces));
		for (register i = 0; i < faces.count(); i++) {
			FACE* pFace = static_cast<FACE*>(faces[i]);
			pAttrib = pFace->attrib();
            int faceColor = currColor;

            bool attribFound = false;
			while (pAttrib) {
				if (is_ATTRIB_RGB(pAttrib)) {
                    ATTRIB_RGB* pATTRIB_RGB = static_cast<ATTRIB_RGB*>(pAttrib);
                    if (pCurrPaletteColors) {
                        if (!ColorMapCache.Find(pATTRIB_RGB->color(), faceColor)) {
                            faceColor = findClosestColorIndex(pCurrPaletteColors,
                                                              pATTRIB_RGB->color().red(), 
                                                              pATTRIB_RGB->color().green(), 
                                                              pATTRIB_RGB->color().blue());
                            ColorMapCache.Add(pATTRIB_RGB->color(), faceColor);
                        }
                    }
                    pATTRIB_RGB->lose();

                    new ATTRIB_COL(pFace, faceColor);
                    attribFound = true;
					break;
				}
                else if (is_ATTRIB_COL(pAttrib)) {
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
            if (!attribFound) 
			    new ATTRIB_COL(pFace, faceColor);
		}

		// set color attributes of edges
		//
		checkOutcome(api_get_edges(pBody, edges));
		for (i = 0; i < edges.count(); i++) {
			EDGE* pEdge = static_cast<EDGE*>(edges[i]);
			pAttrib = pEdge->attrib();
            int edgeColor = currColor;

			bool attribFound = false;
			while (pAttrib) {
				if (is_ATTRIB_RGB(pAttrib)) {
                    ATTRIB_RGB* pATTRIB_RGB = static_cast<ATTRIB_RGB*>(pAttrib);
                    if (pCurrPaletteColors) {
                        if (!ColorMapCache.Find(pATTRIB_RGB->color(), edgeColor)) {
                            edgeColor = findClosestColorIndex(pCurrPaletteColors,
                                                              pATTRIB_RGB->color().red(), 
                                                              pATTRIB_RGB->color().green(), 
                                                              pATTRIB_RGB->color().blue());
                            ColorMapCache.Add(pATTRIB_RGB->color(), edgeColor);
                        }
                    }
                    pATTRIB_RGB->lose();

			        new ATTRIB_COL(pEdge, edgeColor);
                    attribFound = true;
                    break;
				}
                else if (is_ATTRIB_COL(pAttrib)) {
                    attribFound = true;
                    break;
                }
				else {
					if (pAttrib->next() != pAttrib)
						pAttrib = pAttrib->next();
					else
						break;
				}
                if (!attribFound) 
                    new ATTRIB_COL(pEdge, edgeColor);
			}
		}
	}
	catch (const exception&)
	{
		success = false;
	}	
	return success;
}


/******************************************************************
 * 
 * Map the color index of ACIS body to GB values
 *
 * Author: Stephen Hou
 * Date: June 8, 2004
 *
 ******************************************************************/
bool mapBodyColorsToRGBValues(BODY* pBody, const RGBQUAD* pCurrPaletteColors)
{
    if (!pCurrPaletteColors)
        return false;

    bool success = true;
	try {
		ATTRIB*	pAttrib = NULL;
		ENTITY_LIST faces, edges;
		checkOutcome(api_get_faces(pBody, faces));
		for (register i = 0; i < faces.count(); i++) {
			FACE* pFace = static_cast<FACE*>(faces[i]);
			pAttrib = pFace->attrib();

			while (pAttrib) {
				if (is_ATTRIB_COL(pAttrib)) {
                    int color = static_cast<ATTRIB_COL*>(pAttrib)->colour();
		            RGBQUAD rgbColor = pCurrPaletteColors[color - 1];
                    pAttrib->lose();

                    rgb_color rgbValue(((double)rgbColor.rgbRed) / 255.,
                                       ((double)rgbColor.rgbGreen) / 255.,
                                       ((double)rgbColor.rgbBlue) / 255.);

                    // add color mapping to the cache if it doesn't exist
                    ColorMapCache.Add(rgbValue, color, false);

                    // attach normalized rgb colors as a attribute.
                    new ATTRIB_RGB(pFace, rgbValue);
					break;
				}
				else {
					if (pAttrib->next() != pAttrib)
						pAttrib = pAttrib->next();
					else
						break;
				}
			}
		}

		// set color attributes of edges
		//
		checkOutcome(api_get_edges(pBody, edges));
		for (i = 0; i < edges.count(); i++) {
			EDGE* pEdge = static_cast<EDGE*>(edges[i]);
			pAttrib = pEdge->attrib();
			while (pAttrib) {
				if (is_ATTRIB_RGB(pAttrib)) {
                    int color = static_cast<ATTRIB_COL*>(pAttrib)->colour();
		            RGBQUAD rgbColor = pCurrPaletteColors[color - 1];
                    pAttrib->lose();

                    rgb_color rgbValue(((double)rgbColor.rgbRed) / 255.,
                                       ((double)rgbColor.rgbGreen) / 255.,
                                       ((double)rgbColor.rgbBlue) / 255.);

                    // add color mapping to the cache if it doesn't exist
                    ColorMapCache.Add(rgbValue, color, false);

                    // attach normalized rgb colors as a attribute.
                    // attach normalized rgb colors as a attribute.
                    new ATTRIB_RGB(pEdge, rgbValue);
                    break;
				}
				else {
					if (pAttrib->next() != pAttrib)
						pAttrib = pAttrib->next();
					else
						break;
				}
			}
		}
	}
	catch (const exception&)
	{
		success = false;
	}	
	return success;
}


/******************************************************************
 * 
 * Set the color of an ACIS body.
 *
 * Author: Stephen Hou
 * Date: May 7, 2004
 *
 ******************************************************************/
bool setBodyColor(BODY* pBody, int color)
{
	bool success = true;
	try {
		ATTRIB*	pAttrib = NULL;
		ENTITY_LIST faces, edges;
		checkOutcome(api_get_faces(pBody, faces));
		for (register i = 0; i < faces.count(); i++) {
			FACE* pFace = static_cast<FACE*>(faces[i]);
			pAttrib = pFace->attrib();

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
		}

		// set color attributes of edges
		//
		checkOutcome(api_get_edges(pBody, edges));
		for (i = 0; i < edges.count(); i++) {
			EDGE* pEdge = static_cast<EDGE*>(edges[i]);
			pAttrib = pEdge->attrib();

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
	catch (const exception&)
	{
		success = false;
	}	
	return success;
}


/******************************************************************
 * 
 * Set the color of new colorless faces and edges.
 *
 * Author: Stephen Hou
 * Date: May 10, 2004
 *
 ******************************************************************/
bool setColorToNewFaces(BODY* pBody, int color)
{
	bool success = true;
	try {
		ATTRIB*	pAttrib = NULL;
		ENTITY_LIST faces, edges;
		checkOutcome(api_get_faces(pBody, faces));
		for (register i = 0; i < faces.count(); i++) {
			FACE* pFace = static_cast<FACE*>(faces[i]);
			pAttrib = pFace->attrib();

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
			if (!attribFound) 
				new ATTRIB_COL(pFace, color);
		}

		// set color attributes of edges
		//
		checkOutcome(api_get_edges(pBody, edges));
		for (i = 0; i < edges.count(); i++) {
			EDGE* pEdge = static_cast<EDGE*>(edges[i]);
			pAttrib = pEdge->attrib();

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
			if (!attribFound) 
				new ATTRIB_COL(pEdge, color);
		}
	}
	catch (const exception&)
	{
		success = false;
	}	
	return success;
}

int getEntityColor(ENTITY* pEntity)
{
    ENTITY* pEnt = pEntity;
    while (pEnt)
    {
	    ATTRIB*	pAttrib = pEnt->attrib();
	    while (pAttrib)
	    {
		    if(is_ATTRIB_COL(pAttrib))
			    return static_cast<ATTRIB_COL*>(pAttrib)->colour();
			else {
				if (pAttrib->next() != pAttrib)
					pAttrib = pAttrib->next();
				else
					break;
			}
	    }

        pEnt = pEnt->owner();
    }
	return BYLAYER_COLOR;
}


SPAtransf acisTransf
(
const icl::transf& ict
)
throw (exception)
{
/*D.G.*/// Code is from BA's acisTransformFromBwTransform function.

    // Convert 4 * 3 matrix to ACIS transf.
    // We care about translation, rotation, mirror and uniform scale.
    // We don't care about non-uniform scaling and shear.

#define cbrt(x) ((x) > 0.0 ?  pow((double) (x), 1.0/3.0) :      \
                ((x) < 0.0 ? -pow((double)-(x), 1.0/3.0) : 0.0))

    // Get rows of matrix
    const icl::matrix& icm = ict.affine();
    SPAvector  row0(icm(0,0), icm(0,1), icm(0,2)),
               row1(icm(1,0), icm(1,1), icm(1,2)),
               row2(icm(2,0), icm(2,1), icm(2,2));

    const icl::gvector& itrans = ict.translation();
    SPAvector transl(itrans.x(), itrans.y(), itrans.z());

    // Upper 3x3 submatrix
    SPAmatrix  mat(row0, row1, row2);
    double  det = mat.determinant();
    if (is_zero(det))
        throw exception();   // Invalid transformation matrix

    // Remove uniform scale factor from 3x3 submatrix
    double  scaleFactor = fabs(cbrt(det));
    mat *= (1.0 / scaleFactor);
    det = mat.determinant();    // Determinant should now be +1 or -1

    // Reflection if determinant is negative
    int bReflectFlag = (det < 0.0);

    // No rotation if diagonal elements all 1 in absolute value.
    int bRotateFlag = 1;
    if( is_zero(1.0 - fabs(mat.element(0, 0))) &&
        is_zero(1.0 - fabs(mat.element(1, 1))) &&
        is_zero(1.0 - fabs(mat.element(2, 2))) )
        bRotateFlag = 0;

    // Create acis transformation matrix
    return restore_transf(mat, transl, scaleFactor, bRotateFlag, bReflectFlag, 0);

#undef cbrt
}


bool isPlanar(BODY* pBody)
{
    ENTITY_LIST faces;
    api_get_faces(pBody, faces);
    faces.init();

    const surface* p1 = 0;
    for (int i = 0, n = faces.count(); i < n; i++)
    {
        FACE* pFace = static_cast<FACE*>(faces[i]);
        if (pFace->geometry()->identity() != PLANE_TYPE)
            return false;

        const surface& p2 = pFace->geometry()->equation();
        if (!p1)
            p1 = &p2;
        else if (*p1 != p2)
            return false;
    }
    return true;
}

void generateEdgesToDraw
(
FACE* pFace,
int nIsolines,
std::vector<EDGE*>& edges,
int& nBoundaryEdges
)
throw (exception)
{
	ENTITY_LIST edgelist;
    api_get_edges(pFace, edgelist);
    nBoundaryEdges = edgelist.count();
	
	recast<EDGE>(edgelist, edges);
	edgelist.clear();

    int res = 0;

    if (!is_planar_face(pFace))
    {
        SPAtransf tr;
        checkOutcome(api_face_nu_nv_isolines(nIsolines, nIsolines, pFace, tr, &edgelist));
		int color = getEntityColor(pFace);
		for (register i = 0; i < edgelist.count(); i++) {
			EDGE* pEdge = static_cast<EDGE*>(edgelist[i]);
			new ATTRIB_COL(pEdge, color);
		}
		recast<EDGE>(edgelist, edges);
    }
}

void generateDisplayObjectsFromEdges
(
 std::vector<EDGE*>& edges,
int nBoundaryEdges,
double dPrecision,
CAsDisplayObject& dispObj
)
throw (exception)
{
    API_BEGIN
	try
	{
		for (int nEdges = edges.size(); nEdges--;)
		{
			EDGE* pEdge = static_cast<EDGE*>(edges[nEdges]);
			if (!pEdge->geometry())
				continue;

			bool bFirstUse = 0 == find_attrib(pEdge, CAsAttrib_TYPE, CAsAttribMarker_TYPE);
			if (bFirstUse)
				new CAsAttribMarker(pEdge);

			const curve& edgeCurve = pEdge->geometry()->equation();

			SPAinterval range = pEdge->param_range();
			if (range.empty())
				continue;
			if (pEdge->sense() == REVERSED)
				range.negate();

			int nMaxPoints = 100;
			position_array aPoints; aPoints.SetSize(nMaxPoints);
			std::vector<double> aParams(nMaxPoints);
			int nPoints;

			do
			{
				checkOutcome(api_facet_curve(edgeCurve, range.start_pt(), range.end_pt(), dPrecision, nMaxPoints, nPoints, aPoints.GetBuffer(), aParams.begin()));

				if (nPoints >= nMaxPoints)
				{
					nMaxPoints *= 2;
					aPoints.SetSize(nMaxPoints);
					aParams.resize(nMaxPoints);
					continue;
				}
				break;
			}
			while (true);

			int type = (is_closed_curve(pEdge)? 3: 1);
			if (!bFirstUse)
				type |= eInvisible;

			int color = getEntityColor(pEdge);

			int iedge = -1;
			if (nEdges < nBoundaryEdges)
				iedge = reinterpret_cast<int>(pEdge);
            
			dispObj.start(iedge);
			dispObj.add((point*)aPoints.GetBuffer(), nPoints, type, color);
			dispObj.end(iedge);
		}	// for( thru edges
	}
	catch (const exception&)
	{
		sys_error(API_FAILED);
	}
    _API_END
	_checkOutcome(result);
}

void detachMarkers(ENTITY_LIST& ents)
{
	ents.init();
	for (ENTITY* ent = 0; ent = ents.next();)
	{
		ATTRIB* pAttrib = find_attrib(ent, CAsAttrib_TYPE, CAsAttribMarker_TYPE);
		if (pAttrib)
			pAttrib->lose();
	}
}

bool contain
(
const SPAinterval& range,
double p,
double period // = 0.
)
{
	return contain(range.start_pt(), range.end_pt(), p, period);
}

bool contain
(
double start,
double end,
double p,
double period //= 0.
)
{
    // adjust parameter to the range
    if (period > SPAresabs)
    {
        while (p < start - SPAresabs)
            p += period;
        while (p > end + SPAresabs)
            p -= period;
    }

    if (p > start - SPAresabs &&
        p < end + SPAresabs)
        return true;
    else
        return false;
}

double resabs2()
{
    static double s_resabs2 = SPAresabs*SPAresabs;
    return s_resabs2;
}

#ifndef BUILD_WITH_LIMITED_ACIS_SUPPORT
ray rayBox
(
const SPAbox& bb,
const SPAposition& rpos,
const SPAunit_vector& dir,
double prec
)
{
	double r;
	{
		double x = bb.x_range().length();
		double y = bb.y_range().length();
		double z = bb.z_range().length();
		r = sqrt(x*x + y*y + z*z)*0.5 + 1.;
	}
	SPAposition c = bb.mid();
	SPAposition ppos = c-r*dir;
	SPAposition root = proj_pt_to_plane(rpos, ppos, dir);

	return ray(root, dir, prec);
}

bool raytestFaces
(
ray& arrow,
BODY* body,
std::vector<hit*>& hits
)
throw (exception)
{
	// performs ray-box test first to improve performance
	//
	if (!rayxbox(arrow,*body->bound()))
		return false;

	ENTITY_LIST faces;
	checkOutcome(api_get_faces(body, faces));

	faces.init();
	for (FACE* face; face = static_cast<FACE*>(faces.next());)
	{
		SURFACE* pSurface = face->geometry();
		if (pSurface->identity() == PLANE_TYPE) { 
			double angle = ((PLANE*)pSurface)->normal() % arrow.direction;
			// skip the face if its normal direction is the same as ray or
			// perpendicular to the ray.
			//
			if (face->sense() == FORWARD && angle >= 0.0 || 
				face->sense() == REVERSED && angle <= 0.0)
				continue;
		}

		hit* h = raytest_face(arrow, face);
		while (h)
		{
			hits.push_back(h);
			h = h->next;
		}
	}
	return (hits.size() > 0);
}

static
bool compareHits(hit* h1, hit* h2)
{
    assert(h1 && h2);
    if (h1->ray_param < h2->ray_param)
        return true;
    else
        return false;
}

void selectHitedFaces
(
std::vector<hit*>& hits, 
xstd<HFACE>::vector& hfaces,
int mode
)
throw (exception)
{
	if (!hits.empty())
	{
		ENTITY_LIST faces;
		
		sort(hits.begin(), hits.end(), compareHits);

		hit_iterator it = hits.begin();
		for (; it != hits.end(); ++it)
		{
			ENTITY* ent = (*it)->entity_hit;
			if (is_FACE(ent))
			{
				if (mode == CModeler::eByEdge)
					continue;

				HFACE f = reinterpret_cast<HFACE>(ent);
				if (hfaces.end() == find(hfaces.begin(), hfaces.end(), f))
					hfaces.push_back(f);
			}
			else if (is_VERTEX(ent))
			{
				if (mode == CModeler::eByEdge)
					continue;

				faces.clear();
				checkOutcome(api_get_faces(ent, faces));
				faces.init();
				for (ENTITY* face = 0; face = faces.next();)
				{
					HFACE f = reinterpret_cast<HFACE>(face);
					if (hfaces.end() == find(hfaces.begin(), hfaces.end(), f))
						hfaces.push_back(f);
				}
			}
			else if (is_EDGE(ent))
			{
				faces.clear();
				checkOutcome(api_get_faces(ent, faces));
				faces.init();
				for (ENTITY* face = 0; face = faces.next();)
				{
					HFACE f = reinterpret_cast<HFACE>(face);
					if (hfaces.end() == find(hfaces.begin(), hfaces.end(), f))
						hfaces.push_back(f);
				}
			}
		}
	}
}

SPAposition getStartPoint
(
BODY* pWire
)
{
    assert(is_wire_body(pWire));

    return pWire->lump()->shell()->wire()->coedge()->start_pos();
}

SPAposition getEndPoint
(
BODY* pWire
)
{
    assert(is_wire_body(pWire));

    COEDGE* pCoedge = pWire->lump()->shell()->wire()->coedge();
    COEDGE* pFirstCoedge = pCoedge;
    do
    {
        if (pCoedge == pCoedge->next() ||
            pCoedge->next() == pFirstCoedge)
            break;
    }
    while (pCoedge = pCoedge->next());
	if (!pCoedge)
		throw exception();

    return pCoedge->end_pos();
}

int disassembleIntoSheets
(
BODY* pBody,
std::vector<BODY*>& sheets
)
throw (exception)
{
    // Make body copy
    BODY* pWorkBody;
    checkOutcome(api_copy_body(pBody, pWorkBody));
    CEntityHolder hbody(pWorkBody);
    pBody = 0;

    // Apply transformation to geometry
	checkOutcome(api_change_body_trans(pWorkBody, 0));

	// Get all faces
	ENTITY_LIST  faces;
	checkOutcome(api_get_faces(pWorkBody, faces));

    int i;
    int n = faces.count();
    std::vector<CEntityHolder> hsheets(n);

	for (i = 0; i < n; i++)
    {
        // Unhook each face from the body
        BODY* pFaceBody = 0;
        checkOutcome(api_unhook_face(static_cast<FACE*>(faces[i]), pFaceBody));
        hsheets[i].set(pFaceBody);

        checkOutcome(api_body_to_2d(pFaceBody));
    }

    // Delete body
	if (pWorkBody)
	{
		if (pWorkBody->lump() && pWorkBody->lump()->shell())
		    pWorkBody->lump()->shell()->set_face(0);
	}

    sheets.resize(n);
    for (i = 0; i < n; i++)
        sheets[i] = static_cast<BODY*>(hsheets[i].release());

    return 1;
}

int disassembleIntoCurves
(
FACE* pFace,
xstd<geodata::curve*>::vector& crvs
)
throw (exception)
{
    ENTITY_LIST edges;
    checkOutcome(api_get_edges(pFace, edges));

    for (int i = 0, n = edges.count(); i < n; i++)
    {
        EDGE* edge = static_cast<EDGE*>(edges[i]);
        geodata::curve* crv = extr::curve(edge);

        if (crv)
            crvs.push_back(crv);
    }

    return 1;
}

BODY* createInfiniteSheet
(
const point& root,
const gvector& normal
)
throw (exception)
{
	BODY* pSheet = 0;
	API_BEGIN
	try
	{
		SPAunit_vector n(normal.x(), normal.y(), normal.z());
		SURFACE* pSurface = new PLANE(asPosition(root), n);
		
		LOOP* pLoop = 0;
		FACE* pNextFace = 0;
		FACE* pFace = new FACE(pLoop, pNextFace, pSurface, FORWARD);
		
		pFace->set_sides(DOUBLE_SIDED);
		pFace->set_cont(BOTH_OUTSIDE);

		pSheet = cstr::body(pFace);
	}
	catch (const exception&)
	{
		sys_error(API_FAILED);
	}
	_API_END
	_checkOutcome(result);

	return pSheet;
}

BODY* createHalfspace
(
const icl::point& root,
const icl::gvector& normal,
const icl::point& testpt
)
throw (exception)
{
	SPAunit_vector n(normal.x(), normal.y(), normal.z());
	double dist = side_of_plane(asPosition(root), n, asPosition(testpt));
	if (is_zero(dist))
		throw exception();

	BODY* pHs = 0;

	API_BEGIN
	try
	{
		SURFACE* pSurface = new PLANE(asPosition(root), n);
		
		REVBIT sense = (dist > SPAresabs)? REVERSED: FORWARD;
		FACE* pFace = new FACE(0, 0, pSurface, sense);
		pFace->set_sides(SINGLE_SIDED);

		pHs = cstr::body(pFace);
	}
	catch (const exception&)
	{
		sys_error(API_FAILED);
	}
	_API_END
	_checkOutcome(result);

	return pHs;
}

void createHalfspaces
(
BODY*& pHs1,
BODY*& pHs2,
const icl::point& root,
const icl::gvector& normal
)
throw (exception)
{
	API_BEGIN
	try
	{
		SPAunit_vector n(normal.x(), normal.y(), normal.z());
		SURFACE* pSurface = new PLANE(asPosition(root), n);
		
		FACE* pFace1 = new FACE(0, 0, pSurface, FORWARD);
		pFace1->set_sides(SINGLE_SIDED);
		FACE* pFace2 = new FACE(0, 0, pSurface, REVERSED);
		pFace2->set_sides(SINGLE_SIDED);

		pHs1 = cstr::body(pFace1);
		pHs2 = cstr::body(pFace2);
	}
	catch (const exception&)
	{
		sys_error(API_FAILED);
	}
	_API_END
	_checkOutcome(result);
}

int extrude
(
std::vector<FACE*>& faces,
BODY* path
)
throw (exception)
{
	if (faces.empty())
		return 0;

	// attach attributes to faces
	FACE* face = getFirstFaceFromSet(faces);
	if (!face)
		return 0;

    sweep_options opt;
	{
		opt.set_solid(1);
		opt.set_two_sided(FALSE);
	}

	ENTITY* pOwner = solid(face);
    BODY* pBody = 0;
	while (face)
	{
		PLANE* p;
		{
			assert(is_PLANE(face->geometry()));
			p = static_cast<PLANE*>(face->geometry());
		}

        // adjust extrusion path to the face
        BODY* pPath;
		adjustPathWithPlane(path, p, pPath);
		CEntityHolder hPath(pPath);

        // sweep
        checkOutcome(api_sweep_with_options(face, pPath, &opt, pBody));
        assert(pBody==0);

        checkEntity(pOwner);

		ENTITY_LIST facesl;
		checkOutcome(api_get_faces(pOwner, facesl));
		face = getNextFaceFromSet(facesl, faces);
    }

	return 1;
}

int adjustPathWithPlane
(
BODY* path,
PLANE* p,
BODY*& adpath
)
throw (exception)
{
	adpath = 0;

    SPAposition start = getStartPoint(path);
	SPAposition end = getEndPoint(path);

    SPAposition root = p->root_point();
    SPAunit_vector normal = p->normal();
    double dstart = dist_pt_to_plane(start, root, normal);
    double dend = dist_pt_to_plane(end, root, normal);

    SPAtransf shift;
    if (dstart < dend)
    {
        SPAposition pstart = proj_pt_to_plane(start, root, normal);
        shift = translate_transf(pstart-start);
    }
    else
    {
        SPAposition pend = proj_pt_to_plane(end, root, normal);
        shift = translate_transf(pend-end);
    }

    checkOutcome(api_copy_body(path, adpath));
	CBodyHolder hadpath(adpath);
    checkOutcome(api_transform_entity(adpath, shift));
	hadpath.release();

	return 1;
}


int extrude
(
std::vector<FACE*>& faces,
double distance,
double taper
)
throw (exception)
{
	if (faces.empty())
		return 0;

	// attach attributes to faces
	FACE* face = getFirstFaceFromSet(faces);
	if (!face)
		return 0;

    sweep_options opt;
    {
        opt.set_solid(1);
        opt.set_two_sided(FALSE);
     
        taper = reduce_to_range(SPAinterval(-pi(), pi()), pi2(), taper);
        opt.set_draft_angle(taper);
    }

	ENTITY* pOwner = solid(face);
	BODY* pBody = 0;
	while (face)
	{
		// start extruding
        checkOutcome(api_sweep_with_options(face, distance, &opt, pBody));
        assert(pBody==0);

		checkEntity(pOwner);

		// find face to extrude by attrib
		ENTITY_LIST facesl;
		checkOutcome(api_get_faces(pOwner, facesl));
		face = getNextFaceFromSet(facesl, faces);
    }
	
	return 1;
}

/*-------------------------------------------------------------------------*//**
Mark [0,n) faces in array by ATTRIB_GEN_POINTER, and return last.
@author Alexey Malov
@param <=> array of faces
@return null if array is empty; face pointer - for success.
*//*--------------------------------------------------------------------------*/
FACE* getFirstFaceFromSet
(
std::vector<FACE*>& faces
)
{
	if (faces.empty())
		return 0;

	for (int i = 0, n = faces.size()-1; i < n ; i++)
	{
		new CAsAttribMarker(faces[i]);
	}

	FACE* face = faces.back();
	faces.pop_back();
	return face;
}

/*-------------------------------------------------------------------------*//**
Find a face from 'set', which marked by value from 'faces'.
If suxh face is found, delete its mark from 'faces' and return the face.
@author Alexey Malov
@param => set of faces
@param <=> set of marks
@return 0 - if array is empty, or face was not found; pointer to found face - for success.
*//*--------------------------------------------------------------------------*/
FACE* getNextFaceFromSet
(
ENTITY_LIST& set,
std::vector<FACE*>& faces
)
{
	if (faces.empty())
		return 0;

	FACE* face = 0;
	set.init();
	for (face = 0; face = static_cast<FACE*>(set.next());)
	{
		ATTRIB* pAttrib = find_attrib(face, CAsAttrib_TYPE, CAsAttribMarker_TYPE);
		if (0 != pAttrib)
		{
			pAttrib->lose();
			return face;
		}
	}
	return face;
}

SURFACE* face_surface(BODY* b)
{
	LUMP* l = b->lump();
	if (l == 0)
		return 0;
	SHELL* s = l->shell();
	if (s == 0)
		return 0;
	FACE* f = s->face();
	if (f == 0)
		return 0;
	return f->geometry();
}

BODY* solid(FACE* f)
{
	if (f->shell() == 0 ||
		f->shell()->lump() == 0)
		return 0;
	return f->shell()->lump()->body();
}

WIRE* wire(BODY* pBody)
{
	if (!pBody ||
		!pBody->lump())
		return 0;
	if (0 != pBody->lump()->next())
		return 0;
	if (!pBody->lump()->shell() || 
		!pBody->lump()->shell()->wire())
		return 0;
	return pBody->lump()->shell()->wire();
}


CDbAcisEntityData::EType typeOfResult
(
CDbAcisEntityData::EType t1,
CDbAcisEntityData::EType t2,
BOOL_TYPE o
)
{
	switch (o)
	{
	case UNION:
	case NONREG_UNION:
		return t1 + t2;
	case INTERSECTION:
	case NONREG_INTERSECTION:
		return t1 * t2;
	case SUBTRACTION:
	case NONREG_SUBTRACTION:
		return t1 - t2;
	default:
		assert(false);
	}
	return CDbAcisEntityData::eUndefined;
}

void append
(
ENTITY_LIST& dest,
const ENTITY_LIST& src
)
{
	src.init();
	for (ENTITY* e = 0; e = src.next();)
	{
		dest.add(e);
	}
}

int find
(
const ENTITY_LIST& elist,
const ENTITY* v
)
{
	int i = 0;
	elist.init();
	for (ENTITY* e = 0; e = elist.next(); i++)
	{
		if (e == v)
		{
			return i;
		}

	}
	return -1;
}


#endif //#ifndef BUILD_WITH_LIMITED_ACIS_SUPPORT

#if defined(_DEBUG)
void printDbgMessage(const char *szFormat,...)
{
    const int nBuff = 1024;
    char szBuff[nBuff];
    
    va_list args;
    va_start(args, szFormat);
    _vsnprintf (szBuff, nBuff, szFormat, args);
    va_end (args);

    OutputDebugString(szBuff);
}
#endif

#if defined(_DEBUG) && !defined(_COVERAGE)
void save(ENTITY* pe1, ...)
{
	const char* tempdir;
	if ((0 == (tempdir = getenv("temp"))) &&
		(0 == (tempdir = getenv("tmp"))))
		return;

	char filename[_MAX_PATH];
	strcpy(filename, tempdir);
	strcat(filename, "\\icasmodel");
	strcat(filename, tmpnam(0));
    CFilePtr of(fopen(filename, "w"));
    if (!of)
        return;

    ENTITY_LIST el;
    ENTITY* pe = pe1;

    va_list ents;
    va_start(ents, pe1);
    while (pe)
    {
        el.add(pe);
        pe = va_arg(ents, ENTITY*);
    }
    va_end(ents);

    checkOutcome(api_save_entity_list(of, TRUE, el));
}
#endif

#if _UNUSED

int orient
(
BODY* wires[],
const bool o[],
int nwires,
bool closed
)
throw (exception)
{
    if (nwires < 2)
        return 1;
    
    int i;
    for (i = 0; i < nwires-1; i++)
    {
        int icurr = i;
        int inext = i+1;

        for (int j = 0; j < 4; j++)
        {
            if (getEndPoint(wires[icurr]) == getStartPoint(wires[inext]))
                break;

            if (j == 0 || j == 2)
            {
                checkOutcome(api_reverse_wire(wires[inext]));
                continue;
            }
            if (i || !checkOutcome(api_reverse_wire(wires[icurr])))
                return 0;
        }
        if (j>=4)
            return 0; // ends of
    }

    if (closed)
    {
        if (getEndPoint(wires[nwires-1]) != getStartPoint(wires[0]))
			return 0;
    }

    for (i = 0; i < nwires; i++)
    {
        if (!o[i])
        {
            checkOutcome(api_reverse_wire(wires[i]));
        }
    }

    return 1;
}

int checkStatusList(check_status_list* sl)
{
    while(sl != NULL)
    {
        switch(sl->status())
        {
            case check_irregular:
            break;
            case check_self_intersects:
            break;
            case check_bad_closure:
            break;
            case check_bs3_null:
            break;
            case check_bs3_coi_verts:
            break;
            case check_bad_degeneracies:
            break;
            case check_untreatable_singularity:
            break;
            case check_non_G0:
            break;
            case check_non_G1:
            break;
            case check_non_G2:
            break;
            case check_non_C1:
            break;
            case check_unknown:
            ;
        }
        sl = sl->next();  
    }

    return 1;
}

bool isBoxFarThan
(
const box& bb, 
const position& root, 
const unit_vector& dir, 
double distance
)
{
    ray ry(root, dir);
    if (rayxbox(ry, bb))
        return false;

    position p[12];
    unit_vector dr[3];
    double n[3];
      
    vector d = bb.high() - bb.low();
    // XY plane
    dr[0] = z_axis;
    n[0] = d.z();
    p[0] = bb.low();
    p[1] = p[0] + d.x()*x_axis;
    p[2] = p[1] + d.y()*y_axis;
    p[3] = p[2] - d.x()*x_axis;

    // XZ plane
    dr[1] = y_axis;
    n[1] = d.y();
    p[4] = bb.low();
    p[5] = p[4] + d.x()*x_axis;
    p[6] = p[5] + d.z()*z_axis;
    p[7] = p[6] - d.x()*x_axis;

    // YZ plane
    dr[2] = x_axis;
    n[2] = d.x();
    p[8] = bb.low();
    p[9] = p[8] + d.y()*y_axis;
    p[10] = p[9] + d.z()*z_axis;
    p[11] = p[10] - d.y()*y_axis;

    for (int i = 0; i < 12; i++)
    {
        if (is_zero(n[i/4]))
        {
            i += 3;
            continue;
        }

        double p1, p2;
        double dist = distanceBetweenStraights(root, dir, p[i], dr[i/4], p1, p2);

        if (p2 < -SPAresabs)
            dist = (root - p[i]).len();
        else if (p2 > n[i>>2]+SPAresabs)
            dist = (root - (p[i]+dr[i>>2])).len();

        if (dist < distance)
            return false;
    }

    return true;
}

double distanceBetweenStraights
(
const position& p1,
const unit_vector& d1,
const position& p2,
const unit_vector& d2,
double& param1,
double& param2
)
{
    vector diff = p1 - p2;
    double d2diff = d2%diff;
    double d1d2 = d1%d2;

    double det = 1. - d1d2*d1d2;
    
    if (is_zero(det))
    {
        param1 = 0.;
        param2 = d2diff;

        return (p1 + d1*param1 - p2 + d2*param2).len();
    }

    double d1diff = d1%diff;

    double det1 = -d1diff + d2diff*d1d2;
    double det2 =  d2diff - d1diff*d1d2;

	double rdet = 1/det;
    param1 = det1*rdet;
    param2 = det2*rdet;

    return ((p1 + d1*param1) - (p2 + d2*param2)).len();
}

int getPointOnBody
(
BODY* pLump,
FACE*& pFace,
position& pt
)
{
	assert(pLump);

	ENTITY_LIST faces;
	checkOutcome(api_get_faces(pLump, faces));

	faces.init();
	ENTITY* face = faces.next();
	if (!face)
		return 0;

	if (pFace)
	{
		do
		{
			if (face == pFace)
				break;
		}
		while (face = faces.next());
		face = faces.next();
	}
	pFace = static_cast<FACE*>(face);

	return find_interior_point(pFace, pt);
}

VERTEX* getStart(COEDGE* pCoedge)
{
    return pCoedge->start(FORWARD);
}

VERTEX* getEnd(COEDGE* pCoedge)
{
    return pCoedge->end(FORWARD);
}

void setStart(COEDGE* pCoedge, VERTEX* pVertex)
{
    EDGE* pEdge = pCoedge->edge();
    if (pCoedge->sense() == FORWARD)
        pEdge->set_start(pVertex);
    else
        pEdge->set_end(pVertex);
}

void setEnd(COEDGE* pCoedge, VERTEX* pVertex)
{
    EDGE* pEdge = pCoedge->edge();
    if (pCoedge->sense() == FORWARD)
        pEdge->set_end(pVertex);
    else
        pEdge->set_start(pVertex);
}
#endif

#ifdef _TEST
#include "./Tests/TsCrasher.h"
#include <windows.h>
#if _USE_TREADS
static
bool crash()
{
	HANDLE hWorking = OpenEvent(EVENT_ALL_ACCESS, FALSE, "ICAS_WORKING");
	if (0 == hWorking)
		return false;
	if (WAIT_TIMEOUT == WaitForSingleObject(hWorking, 0))
	{
		CloseHandle(hWorking);
		return false;
	}
	CloseHandle(hWorking);

	//send message about possible exception
	HANDLE hPossibleException = OpenEvent(EVENT_ALL_ACCESS, FALSE, "ICAS_POSSIBLEEXCEPTION");
	HANDLE hAccepted = OpenEvent(EVENT_ALL_ACCESS, FALSE, "ICAS_ACCEPTED");

	SetEvent(hPossibleException);
	WaitForSingleObject(hAccepted, INFINITE);

	CloseHandle(hPossibleException);
	CloseHandle(hAccepted);

	//need to generate exception?
	enum {eGenerate, eDontGenerate};
	HANDLE ahException[2];
	ahException[eGenerate] = OpenEvent(EVENT_ALL_ACCESS, FALSE, "ICAS_GENERATEEXCEPTION");
	ahException[eDontGenerate] = OpenEvent(EVENT_ALL_ACCESS, FALSE, "ICAS_DONTGENERATEEXCEPTION");
	
	DWORD dwIndex = WaitForMultipleObjects(2, ahException, FALSE, INFINITE);
	dwIndex -= WAIT_OBJECT_0;

	CloseHandle(ahException[eGenerate]);
	CloseHandle(ahException[eDontGenerate]);

	return dwIndex == eGenerate;
}
#else
bool crash()
{
	return g_crasher.possibleException();
}
#endif

void throwException() throw (exception)
{
	if (crash())
		throw exception();
}

void sysError()
{
	if (crash())
		sys_error(API_FAILED);
}
#endif
