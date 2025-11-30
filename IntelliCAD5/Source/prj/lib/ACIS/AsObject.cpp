// File  : 
// Author: Alexey Malov
#include "stdafx.h"
#include "AsObject.h"
#include "AsMeshManager.h"
#include "AsTools.h"
#include "AsAutoPtr.h"
#include "gvector.h"
using icl::gvector;
using icl::point;
#include "sds.h"
#undef T
#ifdef _TEST
#include "testunit/assert.h"
#else
#include <cassert>
#endif

// statics
static
int setEntityRefinement(ENTITY*);

CAsObj::CAsObj()
:
m_pObj(0),
m_mode(eNone),
m_nIsolines(0),
m_dPrecision(0.)
{}

#if 0
CAsObj::CAsObj(const CAsObj& src)
:
m_oDispObj(src.m_oDispObj),
m_mode(src.m_mode),
m_nIsolines(src.m_nIsolines),
m_dPrecision(src.m_dPrecision)
{
    checkOutcome(api_copy_entity(src.m_pObj, m_pObj));
}
#endif

CAsObj::~CAsObj()
{
    if (m_pObj)
        api_del_entity(m_pObj);
}

BODY* CAsObj::get() const
{
    return m_pObj;
}

void CAsObj::set(BODY* p)
{
    m_pObj = p;
}


void CAsObj::transform(const SPAtransf& tr) throw (exception)
{
    checkOutcome(api_transform_entity(m_pObj, tr));
    m_oDispObj.transform(tr);
}

void CAsObj::display
(
sds_dobjll*& pDisp, 
int nIsolines,
double dPrecision
) throw (exception)
{
    int newmode = (nIsolines > 0)? eIsolines: eMesh;
    if (m_mode == newmode && 
       (m_mode != eIsolines || m_nIsolines == nIsolines) &&
        !m_oDispObj.m_nodes.empty())
	{
		if (dPrecision < SPAresabs)
		{
			m_oDispObj.copy(pDisp);
			return;
		}

		double d = fabs(m_dPrecision/dPrecision);
		if (d < 2. && d > 0.2)
		{
		    m_oDispObj.copy(pDisp);
			return;
		}
	}

    m_mode = newmode;
    m_nIsolines = nIsolines;
    m_dPrecision = fabs(dPrecision);
    m_oDispObj.clear();

    display();
    m_oDispObj.copy(pDisp);
}

void CAsObj::display() throw (exception)
{
	if (m_mode == eIsolines)
	{
        // isolines generating case
		displayIsoLines();

        // if there is transformation, apply it
        if (m_pObj->transform())
        {
            m_oDispObj.transform(m_pObj->transform()->transform());
        }

	}
	else if (m_mode == eMesh)
	{
        // mesh generating case
		displayMesh();
	}
}

void CAsObj::displayIsoLines() throw (exception)
{
    ENTITY_LIST faces;
    checkOutcome(api_get_faces(m_pObj, faces));

    m_oDispObj.start(reinterpret_cast<int>(m_pObj));
	{
		std::vector<EDGE*> edges;
		faces.init();
		for (int i = 0, n = faces.count(); i < n; i++, edges.clear())
		{
			FACE* pFace = static_cast<FACE*>(faces[i]);
        
			int nBoundaryEdges = 0;
			generateEdgesToDraw(pFace, m_nIsolines, edges, nBoundaryEdges);
			std::vector<CEdgeHolder> hedges;
			attach<EDGE>(hedges, edges.begin()+nBoundaryEdges, edges.end());

			m_oDispObj.start(reinterpret_cast<int>(pFace));

			generateDisplayObjectsFromEdges(edges, nBoundaryEdges, m_dPrecision, m_oDispObj);

			m_oDispObj.end(reinterpret_cast<int>(pFace));
		}
		{
			ENTITY_LIST edges;
			checkOutcome(api_get_edges(m_pObj, edges));
			detachMarkers(edges);
		}
	}
    m_oDispObj.end(reinterpret_cast<int>(m_pObj));
}

void CAsObj::displayMesh() throw (exception)
{
	// set a refinement for faceting
    API_BEGIN
	try
	{
		REFINEMENT* pRefinement;
		checkOutcome(api_get_default_refinement(pRefinement));
		THolder<REFINEMENT, TLose<REFINEMENT> > hrefinement(pRefinement);

		pRefinement->set_surface_tol(m_dPrecision);
		pRefinement->set_adjust_mode(AF_ADJUST_NONE);
		pRefinement->set_triang_mode(AF_TRIANG_NONE);

		checkOutcome(api_set_default_refinement(pRefinement));
		pRefinement->lose();
		hrefinement.release();
	}
	catch (const exception&)
	{
		sys_error(API_FAILED);
	}
    _API_END
    _checkOutcome(result);

	// facet the entity
	checkOutcome(api_facet_entity(m_pObj));

    ENTITY_LIST faces;
    checkOutcome(api_get_faces(m_pObj, faces));

    int type = 3;//DISP_OBJ_PTS_3D|DISP_OBJ_PTS_CLOSED;
    for (int i = 0, n = faces.count(); i < n; i++)
    {
        FACE* pFace = static_cast<FACE*>(faces[i]);

		// get facets
        POLYGON_POINT_MESH* pmesh;
		checkOutcome(api_get_face_facets(pFace, pmesh));

        if (!pmesh)
            continue;
        
        TAutoPtr<POLYGON_POINT_MESH> pMesh(pmesh);

        int color = getEntityColor(pFace);

        m_oDispObj.start(reinterpret_cast<int>(pFace));

        POLYGON* pPolygon = 0;
		for (pPolygon = pMesh->first(); pPolygon ; pPolygon = pPolygon->next())
		{
            int nPoint = pPolygon->count();
            std::vector<point> points(nPoint);

            POLYGON_VERTEX* pVertex = pPolygon->first();
            for (int j = 0; pVertex; pVertex = pVertex->next(), j++)
                pVertex->point(asPosition(points[j]));

            m_oDispObj.add(points.begin(), points.size(), type, color);
		}	// for( thru polygons

        m_oDispObj.end(reinterpret_cast<int>(pFace));
	}
}

#if _UNUSED
int CAsObj::displayMesh1()
{
    MESH_MANAGER* pPreviousMeshManager;
    if (!checkOutcome(api_get_mesh_manager(pPreviousMeshManager)))
        return 0;

    int rc = 0;
    do // fictive
    {
        CAsSimpleIndexedMeshManager mman;
        if (!checkOutcome(api_set_mesh_manager(&mman)))
            break;

        REFINEMENT *pRefinement = 0;
        if (!checkOutcome(api_create_refinement(pRefinement)) || !pRefinement)
            break;

        pRefinement->set_surface_tol(0.);
        pRefinement->set_normal_tol(15.);
        pRefinement->set_surf_mode(AF_SURF_ALL);
        pRefinement->set_adjust_mode(AF_ADJUST_ALL);
        pRefinement->set_grid_mode(AF_GRID_TO_EDGES);
        pRefinement->set_triang_mode(AF_TRIANG_NONE);

        // Set refinement
        if (!checkOutcome(api_set_entity_refinement(m_pObj, pRefinement)))
            break;

        pRefinement->lose();
        pRefinement = 0;

	    // facet the entity
	    if (!api_facet_entity(m_pObj).ok())
		    break;

        CAsSimpleIndexedMesh& mesh = mman.mesh();

        if (m_pObj->transform())
        {
            mesh.transform(m_pObj->transform()->transform());
        }

        mesh.makeInvisibleEdges();

        for (int i = 0, n = mesh.faces().size(); i < n; i++)
        {
            const int4& face = mesh.face(i);
            npoint<4> points;
            int ipoint = 0;
            bool visibleEdge = false;
            for (int j = 0, m = face.size(); j < m; j++)
            {
                if (face[j] > 0)
                {
                    visibleEdge = true;
                }
                if (visibleEdge)
                {
                    points[ipoint++] = mesh.point(abs(face[j])-1);
                }
                if (face[j] < 0)
                {
                    visibleEdge = false;
                }
                if ((!visibleEdge || j == m-1) && 
                    (ipoint > 0))
                {
                    int type;
                    if (ipoint == m)
                    {
                        type = 3; //DISP_OBJ_PTS_3D|DISP_OBJ_PTS_CLOSED
                    }
                    else
                    {
                        type = 1; //DISP_OBJ_PTS_3D
                        if (j == m-1 && visibleEdge)
                        {
                            points[ipoint++] = mesh.point(abs(face[0])-1);
                        }
                    }

                    m_oDispObj.add(points.m_a, ipoint, type, BYLAYER_COLOR);
                    ipoint = 0;
                }
            }
        }

        rc = 1;
    }
    while (false);

    if (!checkOutcome(api_set_mesh_manager(pPreviousMeshManager)))
        return 0;
    if (!checkOutcome(api_set_entity_refinement(m_pObj)))
        return 0;

    return rc;
}
#endif

int CAsObj::getBasePoint(point& pnt)
{
    if (m_oDispObj.m_nodes.empty())
        display();

    std::list<CAsDisplayObject::CNode*>::const_iterator inode = 
                                                    m_oDispObj.m_nodes.begin();

    for (; inode != m_oDispObj.m_nodes.end(); ++inode)
    {
        if (!(*inode)->m_entity)
        {
            CAsDisplayObject::CChain* pChain = 
                                static_cast<CAsDisplayObject::CChain*>(*inode);
            if (pChain->m_pts.size())
            {
                pnt = pChain->m_pts[0];
                return 1;
            }
        }
    }

    return 0;
}

int CAsObj::getMeshRepresentation
(
xstd<point>::vector& points,
xstd<int>::vector& faces
) const
throw (exception)
{
    // Change mesh manager
    MESH_MANAGER* pPreviousMeshManager;
    checkOutcome(api_get_mesh_manager(pPreviousMeshManager));

    TAutoPtr<CAsSimpleIndexedMeshManager> 
                            pMeshManager(new CAsSimpleIndexedMeshManager);
    {
        checkOutcome(api_set_mesh_manager(pMeshManager));


        // Set new entity refinement
        if (!setEntityRefinement(m_pObj))
            return 0;
    }

	// Facet the entity
	checkOutcome(api_facet_entity(m_pObj));

    // Form mesh
    CAsSimpleIndexedMesh& mesh = pMeshManager->mesh();
    {
        if (m_pObj->transform())
            mesh.transform(m_pObj->transform()->transform());

        mesh.makeInvisibleEdges();

        int nfaces = mesh.faces().size();
        int npoints = mesh.nodes().size();
        if (nfaces == 0 || npoints == 0)
            return 0;

        faces.resize(4*nfaces);
        points.resize(npoints);

        int i;
        for (i = 0; i < npoints; ++i)
            points[i] = mesh.point(i);

        for (i = 0; i < nfaces; ++i)
        {
            int* face = &faces[4*i];
            const int4& _face = mesh.face(i);
            for (int j = 0; j < 4; j++)
                face[j] = _face[j];
        }
    }

    // Change mesh manager and entity refinement back
    checkOutcome(api_set_mesh_manager(pPreviousMeshManager));
    checkOutcome(api_set_entity_refinement(m_pObj));

    return 1;
}


SPAbox CAsObj::getBoundingBox() const
{
    return get_body_box(m_pObj);
}

CAsDisplayObject& CAsObj::disp()
{
    return m_oDispObj;
}

#if 0
EDGE* CAsObj::getClosestEdge
(
const SPAposition& root,
const SPAunit_vector& dir
)
{
    std::list<CAsDisplayObject::CNode*>::iterator inode = m_oDispObj.m_nodes.begin();

    int iedge = 0;
    double dist = -1.;

    for (; inode != m_oDispObj.m_nodes.end(); ++inode)
    {
        int ientity = (*inode)->m_entity;
        if (ientity == -1)
        {
            ++inode;
            assert((*inode)->m_entity == 0);
            ++inode;
            assert((*inode)->m_entity == -1);

            continue;
        }
        if (ientity != 0)
            continue;

        CAsDisplayObject::CChain* chain = static_cast<CAsDisplayObject::CChain*>(*inode);

        int n = chain->m_pts.size();
        int i1 = 0;
        int i2 = 1;
        if (chain->m_type & eClosed)
        {
            i1 = n-1;
            i2 = 0;
        }

        std::vector<point>& pts = chain->m_pts;

        for (; i2 < n; i1 = i2++)
        {
            double p1, p2;
            gvector diff = pts[i2] - pts[i1];
            unit_vector ref = normalise(asVector(diff));
            double d = distanceBetweenStraights(root, dir, asPosition(pts[i1]), ref, p1, p2);

            if (p2 < -resabs)
            {
                d = (root - pts[i1]).len();
            }
            else if (p2*p2 > diff.len_sq() + resabs2())
            {
                d = (root - pts[i2]).len();
            }
           
            if (dist < 0. || d < dist)
            {
                iedge = (*(--inode))->m_entity; inode++;
                dist = d;
            }
        }
    }

    return reinterpret_cast<EDGE*>(iedge);
}
#endif
static
int setEntityRefinement(ENTITY* pObj)
{
    TAutoPtr<REFINEMENT,TLose<REFINEMENT> > pRefinement;
    checkOutcome(api_create_refinement(pRefinement.init()));

    pRefinement->set_surface_tol(0.);
    pRefinement->set_normal_tol(15.);
    pRefinement->set_surf_mode(AF_SURF_ALL);
    pRefinement->set_adjust_mode(AF_ADJUST_ALL);
    pRefinement->set_grid_mode(AF_GRID_TO_EDGES);
    pRefinement->set_triang_mode(AF_TRIANG_NONE);

    // Set refinement
    checkOutcome(api_set_entity_refinement(pObj, pRefinement));

    return 1;
}
