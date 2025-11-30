// File  : 
// Author: Alexey Malov
#if _MSC_VER > 1000
#pragma once
#endif

#ifndef _ASMESH_H_
#define _ASMESH_H_

#include "gvector.h"
using icl::point;
#include "ntype.h"
using icl::ntype;
#include <vector>
#include <memory.h>

// following templates doesn't conserns meshing, it's just tools for data presentation.
// it's enough universaly, but used only here, so it probably should be moved to another place, but yet...

/*-------------------------------------------------------------------------*//**
Array of points.
*//*--------------------------------------------------------------------------*/
template <int _m>
struct npoint: public ntype<point, _m>
{
    // ctors
    npoint()
    :ntype<point, _m>()
    {}
    npoint(point d[_m])
    :ntype<point, _m>(d)
    {}
    npoint(const npoint& d3)
    :ntype<point, _m>(d3)
    {}
};

struct node: public point
{
    // faces, including the the vertex
    std::vector<int> m_faces;

    // ctors
    node()
    :point()
    {}
    node(double d[3])
    :point(d)
    {}
    node(const node& n)
    :point(n), m_faces(n.m_faces)
    {}
};

/*-------------------------------------------------------------------------*//**
Structure, representing polyface mesh face.
Face defined by its vertices, 
which defined by number, 
calculated from placement of corresponded point in array of whole mesh vertices.
Absolute value of the number is index in array + 1, 
then if edge, started in the vertex is invisible -> number is negative.
*//*--------------------------------------------------------------------------*/
struct int4: public ntype<int, 4>
{
    // ctors
    int4()
    :ntype<int, 4>()
    {
        m_a[0] = m_a[1] = m_a[2] = m_a[3] = 0;
    }
    int4(const int i[4])
    :ntype<int, 4>(i)
    {}
    int4(const int4& i4)
    :ntype<int, 4>(i4)
    {}

    // get size of face
    int size() const
    {
        int n = 4;
        (m_a[3] || --n);
        (m_a[2] || --n);
        (m_a[1] || --n);
        (m_a[0] || --n);
        return n;
    }
};

/*-------------------------------------------------------------------------*//**
Simple implemetation of mesh, where facets can be triangles or quadrangles.
*//*--------------------------------------------------------------------------*/
class CAsSimpleIndexedMesh
{
protected: //data
    // geometry (points)
    std::vector<node> m_nodes;
    // topology (faces)
    std::vector<int4> m_faces;

public:
    // ctor
    CAsSimpleIndexedMesh()
    {}

    //accessors
    point& point(int i)
    {
        return m_nodes[i];
    }
    node& vertex(int i)
    {
        return m_nodes[i];
    }
    int4& face(int i)
    {
        return m_faces[i];
    }
    int point(int i, int j) const
    {
        return m_faces[i][j];
    }
    std::vector<int4>& faces()
    {
        return m_faces;
    }
    std::vector<::node>& nodes()
    {
        return m_nodes;
    }

    void transform(const SPAtransf&);

    // correct mesh, to be properly visualized at ICad
    void makeInvisibleEdges();
};

/*-------------------------------------------------------------------------*//**
Functions for creation mesh representaion of several classic polyhedra: 
- pyramid of several forms
- wedge.
*//*--------------------------------------------------------------------------*/
namespace mesh
{
    /*-------------------------------------------------------------------------*//**
    Create pyramid with 4 points in base and on top. 
    It's assumed, i-th base pointed with i-th top point.
       *--------*
      /        /|
     /        / |
    *--------*  *
    |        | /
    |        |/
    *--------*
    @param => base points
    @param => top points
    @param <= points of the mesh
    @param <= faces of the mesh
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    int pyramid
    (
    const npoint<4>&,
    const npoint<4>&,
    std::vector<point>& points,
    std::vector<int4>& faces
    )
	throw (exception);
    /*-------------------------------------------------------------------------*//**
    Create pyramid with 4 point in base and 2 (or 1) point on top.
    It's assumed, 0-th,3-th base point connected with 0-th top point, 
    accordingly 1-th,2-th base point connected with 1-th top point
       *--*-----*
      /    \   /
     /      \ /
    *--------*
    @param => base points
    @param => top points
    @param <= points of the mesh
    @param <= faces of the mesh
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    int pyramid
    (
    const npoint<4>&,
    const npoint<2>&,
    std::vector<point>& points,
    std::vector<int4>& faces
    )
	throw (exception);
    /*-------------------------------------------------------------------------*//**
    Create pyramid with 3 points in base and on top.
        *
       / \
      /   \
     /     \
    *-------*
    |       |
    |       |
    *-------*
    @param => base points
    @param => top points
    @param <= points of the mesh
    @param <= faces of the mesh
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    int pyramid
    (
    const npoint<3>&,
    const npoint<3>&,
    std::vector<point>& points,
    std::vector<int4>& faces
    )
	throw (exception);
    /*-------------------------------------------------------------------------*//**
    Create pyramid with 3 point in base and 1 point on top (tetrahedron).
        *-------*
       / \     /
      /   \   /
     /     \ /
    *-------*
    @param => base points
    @param => top points
    @param <= points of the mesh
    @param <= faces of the mesh
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    int pyramid
    (
    const npoint<3>&,
    const npoint<1>&,
    std::vector<point>& points,
    std::vector<int4>& faces
    )
	throw (exception);
    /*-------------------------------------------------------------------------*//**
    Create wedge.
       *
      / \
     /   \
    *     \
    |\     \
    | \     *
    |  \   /
    |   \ /
    *----*
    @param => first corner of the wedge
    @param => opposite corner of the wedge
    @param => height of the wedge
    @param <= mesh vertices
    @param <= mesh faces
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    int wedge
    (
    const point&, 
    const point&, 
    double, 
    std::vector<point>&, 
    std::vector<int4>&
    );
};

#endif
