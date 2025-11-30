// File  : 
// Author: Alexey Malov
#if _MSC_VER > 1000
#pragma once
#endif

#ifndef _ASOBJECT_H_
#define _ASOBJECT_H_

#include "AsDisplayObject.h"
#include "xstd.h"

// forward declarations
struct sds_dobjll;
namespace icl
{
    class point;
}

/*-------------------------------------------------------------------------*//**
Class represents Acis BODY, 
and have base functionality to maintain it in IntelliCad.
@author Alexey Malov
*//*--------------------------------------------------------------------------*/
class CAsObj
{
    friend class CAsModeler;

    // Acis data:
    BODY* m_pObj;
    // Data for visualization
    CAsDisplayObject m_oDispObj;
   
    // Visualization type
    enum
    {
        eNone, // for initialization
        eIsolines, // visualize faces by edges and isolines
        eMesh // visualize body by mesh.
    };
    int m_mode;
    // Number of isolines (only for isoline mode)
    int m_nIsolines;
    // Approximation precision
    double m_dPrecision;

	// Copying disallowed
	CAsObj(const CAsObj&){}

protected:
    /*-------------------------------------------------------------------------*//**
    Generate display objects, in accordance with set options (m_mode, m_nIsolines, m_dPrecision)
    @author Alexey Malov
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    void display() throw (exception);
    /*-------------------------------------------------------------------------*//**
    Generate display objects for mesh mode.
    NOTE: with usage of standard MESH_MANAGER and POLYGON_POINT_MESH, 
    generated display objects don't consider invisible edges, but correctly used for 'hide'
    @author Alexey Malov
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    void displayMesh() throw (exception);
#if _UNUSED
    /*-------------------------------------------------------------------------*//**
    Generate display objects for mesh mode (another variant, with differences noted in 'NOTEs').
    NOTE: with usage of CAsSimpleIndexedMeshManger and CAsSimpleIndexedMesh, 
    generated display objects _consider invisible edges, but incorrectly used for 'hide'
    @author Alexey Malov
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    int displayMesh1();
#endif
    /*-------------------------------------------------------------------------*//**
    Generates display objects for isoline mode.
    @author Alexey Malov
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    void displayIsoLines() throw (exception);

public:
    // ctor
    CAsObj();
    // dtor
    virtual ~CAsObj();

    // get-set Acis object
    BODY* get() const;
    BODY*& set();
    void set(BODY*);

    /*-------------------------------------------------------------------------*//**
    Transform object (with visualisation data).
    @author Alexey Malov
    @param => transformation
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    void transform(const SPAtransf&) throw (exception);
    /*-------------------------------------------------------------------------*//**
    Generate visualisation data (display objects), in accordance with passed parameters:
    nIsolines <=0 means mesh mode
               >  means isoline mode
    @author Alexey Malov
    @param <= list of display objects
    @param => number of isolines to generate on one face
    @param => precision of approximation
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/                                                                             
    void display
    (
    sds_dobjll*&, 
    int, 
    double
    ) throw (exception);
    /*-------------------------------------------------------------------------*//**
    Gets some point on the object.
    @author Alexey Malov
    @param <= point
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    int getBasePoint(icl::point&);
    /*-------------------------------------------------------------------------*//**
    Get mesh representation of the object.
    @author Alexey Malov
    @param <= points of the mesh
    @param <= number of the points
    @param <= faces of the mesh (vertices coded by 'index' in point array; 
                                 index is the (number in array)+1, 
                                 negative index means invisible edge
    @param <= number of faces
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    int getMeshRepresentation
    (
    xstd<icl::point>::vector& aPoints,
    xstd<int>::vector& aFaces
    ) const throw (exception);

    CAsDisplayObject& disp();

    SPAbox getBoundingBox() const;
#if 0
    EDGE* getClosestEdge(const SPAposition&, const SPAunit_vector&);
#endif // 0
};

#endif
