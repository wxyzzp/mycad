// File  : 
// Author: Alexey Malov
#include "stdafx.h"
#include "AsMeshManager.h"
#include <algorithm>

void CAsSimpleIndexedMeshManager::announce_counts
(
int,// npoly,
int,// nnode,
int// nref
)
{}

logical CAsSimpleIndexedMeshManager::need_indexed_polygons()
{
    return TRUE;
}

void* CAsSimpleIndexedMeshManager::announce_indexed_node
(
int inode,
const SPApar_pos&,
const SPAposition& p,
const SPAunit_vector&
)
{
    node n((double*)&p);
    std::vector<node>::iterator i = std::find(m_mesh.nodes().begin(), m_mesh.nodes().end(), n);
    if (i == m_mesh.nodes().end())
    {
        m_mesh.nodes().push_back(n);
        return reinterpret_cast<void*>(m_mesh.nodes().size());
    }
    else
    {
        return reinterpret_cast<void*>(i - m_mesh.nodes().begin()+1);
    }
}

void CAsSimpleIndexedMeshManager::start_indexed_polygon
(
int /*ipoly*/,
int /*npolynode*/, 
int /*ishare*/
)
{
    int4 face;
    m_mesh.faces().push_back(face);
    m_bFirstSubFace = true;
}

void CAsSimpleIndexedMeshManager::announce_indexed_polynode
(
ENTITY* /*pe*/,
int ipoly,
int i,
void* inode
)
{
    announce_indexed_polynode(ipoly, i, inode);
}

void CAsSimpleIndexedMeshManager::announce_indexed_polynode
(
int,
int, //i
void *pnode
)
{
    int inode = reinterpret_cast<int>(pnode);
    // Check size of current subface
    int i = m_mesh.faces().back().size();
    if (i == 4)
    {
        // Correct current subface: last edge will invisible
        int4& prevface = m_mesh.faces().back();
        prevface[3] *= -1;
        if (m_bFirstSubFace)
        {
            m_iFirstNode = prevface[0];
            m_bFirstSubFace = false;
        }

        // New subface
        int4 newface;
        // First edge
        newface[0] = -m_iFirstNode;
        newface[1] = -prevface[3];
        // Add new subface to list of vertex->face references
        m_mesh.vertex(m_iFirstNode-1).m_faces.push_back(m_mesh.faces().size());
        // Add subface
        m_mesh.faces().push_back(newface);
        i = 2;
    }
    m_mesh.faces().back()[i] = inode;
    m_mesh.vertex(inode-1).m_faces.push_back(m_mesh.faces().size()-1);
}

void CAsSimpleIndexedMeshManager::announce_indexed_polynode
(
ENTITY*, 
int ipoly, 
int i, 
void *id, 
const double&, 
const SPApar_pos&, 
const SPAposition&, 
const SPAunit_vector&)
{
    announce_indexed_polynode(ipoly, i, id);
}

