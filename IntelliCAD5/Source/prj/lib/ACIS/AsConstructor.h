// File  : 
// Author: Alexey Malov
#if _MSC_VER > 1000
#pragma once
#endif

#ifndef _ASCONSTRUCTOR_H_
#define _ASCONSTRUCTOR_H_

#include <vector>

// forward declarations
struct int4;
namespace icl
{
    class point;
}
namespace geodata
{
    struct curve;
    struct line;
    struct pline;
    struct ellipse;
    struct ispline;
    struct aspline;
}

/*-------------------------------------------------------------------------*//**
Namespace unite functions, constructing Acis objects.
*//*--------------------------------------------------------------------------*/
namespace cstr
{
    /*-------------------------------------------------------------------------*//**
    Create straight from geodata::line.
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    int line
    (
    const geodata::curve&,
    straight&
    )throw (exception);
    /*-------------------------------------------------------------------------*//**
    Create PLANE from planar LOOP. Plane normal is directed in left-hand-rule of loop direction.
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    PLANE* plane
    (
    const LOOP*
    )throw (exception);
    /*-------------------------------------------------------------------------*//**
    Create wire from abstract geodata::curve 
    (make curve type identification, calls proper function).
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    BODY* wire
    (
    const geodata::curve&
    )throw (exception);
	/*-------------------------------------------------------------------------*//**
	Transform wire into array of edges. Wire is destroyed.
	@author Alexey Malov
	@param => initial wire
	@param <= array of edges
	@return 1 - for success; 0 - for error
	*//*--------------------------------------------------------------------------*/
	void edges
	(
	BODY* wire,
	std::vector<EDGE*>&
	)throw (exception);
	/*-------------------------------------------------------------------------*//**
	General function for geting edges from curve; array can appear if curve is polyline.
	@author Alexey Malov
	@param => initial curve
	@param <= array, with added edges, corresponding the curve
	@return 1 - for success; 0 - for error
	*//*--------------------------------------------------------------------------*/
	void edges
	(
	const geodata::curve&,
	std::vector<EDGE*>&
	)throw (exception);
    /*-------------------------------------------------------------------------*//**
    Create edge from interpolating spline, described by geodata::ispline.
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    EDGE* edge
    (
    const geodata::ispline&
    )throw (exception);
    /*-------------------------------------------------------------------------*//**
    Create edge from approximating spline, described by geodata::aspline.
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    EDGE* edge
    (
    const geodata::aspline&
    )throw (exception);
    /*-------------------------------------------------------------------------*//**
    Create edge from ellispe (arc, circe), described by geodata::ellipse.
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    EDGE* edge
    (
    const geodata::ellipse&
    )throw (exception);
    /*-------------------------------------------------------------------------*//**
    Create straight edge from 2 points.
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    EDGE* edge
    (
    const SPAposition&,
    const SPAposition&
    )throw (exception);
    /*-------------------------------------------------------------------------*//**
    Create straight edge from 2 vertices.
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    EDGE* edge
    (
    VERTEX*, 
    VERTEX*
    )throw (exception);
    /*-------------------------------------------------------------------------*//**
    Create edge from straight.
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    EDGE* edge
    (
    const geodata::line&
    )throw (exception);
    /*-------------------------------------------------------------------------*//**
    Create edge from abstract geodata::curve 
    (make curve type identification, calls proper function).
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    EDGE* edge
    (
    const geodata::curve&
    )throw (exception);
    /*-------------------------------------------------------------------------*//**
    Create planar face, from mesh facet.
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    FACE* face
    (
    const std::vector<VERTEX*>&,
    const int4& face
    )throw (exception);
    /*-------------------------------------------------------------------------*//**
    Cerate Acis BODY from mesh representation.
    @author Alexey Malov
    @param => mesh vertices
    @param => mesh faces
    @param <= body
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    BODY* body
    (
    const std::vector<icl::point>&,
    const std::vector<int4>&
    )throw (exception);
    /*-------------------------------------------------------------------------*//**
    Create BODY, from ENTITY. ENTITY can be FACE, SHELL, LUMP, BODY.
    Add to passed ENTITY missing topology items.
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    BODY* body
    (
    ENTITY*
    )throw (exception);

    /*-------------------------------------------------------------------------*//**
    Construct a region from a planar closed wire. The function check if the
    wire is self-intersected, planal, and closed internally. The first BODY
    parameter contains the wire for creating a region. If the operation is 
    successful, The wire in the body will be replaced with a region and the 
    body will represent a region. The second parameter is option for specifing
    the desired normal direction and single or double side of the region. If
    the parameter value is provided, it will create a single side region. 
    Otherwise, it will create a double side region. NOTE: boolean operations
    do not work with single regions. 
 
    The function returns 1 it succeeds and one of the following error codes when
    it fails:

          0 - general failure
		 -1 - non-planar wire
         -2 - open wire
         -3 - self-intersected wire

	Author:	Stephen W. Hou
	Date:	5/8/2003
    *//*--------------------------------------------------------------------------*/
	int region(BODY* pBody,				// In: The wire body; Out: The region body
			   const SPAvector &refNormal = *(SPAvector*)NULL  // Optional desired normal direction
			  ) throw (exception);
};

#endif
