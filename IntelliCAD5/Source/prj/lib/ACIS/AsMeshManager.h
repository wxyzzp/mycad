// File  : 
// Author: Alexey Malov
#if _MSC_VER > 1000
#pragma once
#endif

#ifndef _ASMESHMANAGER_H_
#define _ASMESHMANAGER_H_

#include "AsMesh.h"

/*-------------------------------------------------------------------------*//**
Mesh manager, generating SimpleIndexedMesh.
@author Alexey Malov
*//*--------------------------------------------------------------------------*/
class CAsSimpleIndexedMeshManager: public MESH_MANAGER
{
protected: //data
    CAsSimpleIndexedMesh m_mesh;

    // flags
    bool m_bFirstSubFace;

    int m_iFirstNode;

public:
    //ctor
    CAsSimpleIndexedMeshManager()
    :MESH_MANAGER(),
    m_bFirstSubFace(true),
    m_iFirstNode(0)
    {}

    virtual void announce_counts
    (
    int npoly,
    int nnode,
    int nref
    );

    virtual logical need_indexed_polygons();

    virtual void *announce_indexed_node
    (
    int inode,
    const SPApar_pos &uv,
    const SPAposition &,
    const SPAunit_vector &
    );
    
    virtual void start_indexed_polygon
    (
    int ipoly,
    int npolynode, 
    int ishare = -2
    );

    virtual void announce_indexed_polynode
    (
    ENTITY*,
    int,
    int,
    void*
    );

    virtual void announce_indexed_polynode
    (
    int ipoly,
    int i,
    void* pnode
    );

    virtual void announce_indexed_polynode
    (
    ENTITY *E,              // Entity (actually, COEDGE or null) along the
                            // edge following the polynode.
    int ipoly,              // 0-based polygon index. This is the
                            //  same as the immediately preceding call to
                            //  start_indexed_polygon().
    int i,                  // 0-based counter within the polygon.  This
                            //  increments sequentially on successive calls.
    void *id,               // Node identifer as previously received from
                            // announce_indexed_node
    const double &edge_tpar,  //If the node lies on an edge ten returns tpar otherwise garbage;
    const SPApar_pos &uv,      // parametric coordinates
    const SPAposition &,      // cartesian coordinates
    const SPAunit_vector &    // surface normal
    );

    CAsSimpleIndexedMesh& mesh() 
    {
        return m_mesh;
    }
};

#endif
