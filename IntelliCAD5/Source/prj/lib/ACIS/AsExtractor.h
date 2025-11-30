// File  : 
// Author: Alexey Malov
#if _MSC_VER > 1000
#pragma once
#endif

#ifndef _ASEXTRACTOR_H_
#define _ASEXTRACTOR_H_
#include <exception>

//forward declarations
namespace geodata
{
    struct curve;
    struct line;
    struct ellipse;
    struct spline;
}
namespace icl
{
    class point;
    class gvector;
}

/*-------------------------------------------------------------------------*//**
Functions for extracting curves data from Acis objects in form of geodata::curve.
*//*--------------------------------------------------------------------------*/
namespace extr
{
    /*-------------------------------------------------------------------------*//**
    Get geodata::curve from EDGE.
    @author Alexey Malov
    @param => edge
    @param <= geodata::curve
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    geodata::curve* curve
    (
    EDGE*
    )throw (std::exception);
    /*-------------------------------------------------------------------------*//**
    Gets geodata::curve from straight, with considering start, end parameters of the line.
    @author Alexey Malov
    @param => Acis's straight
    @param => start parameter on the straight
    @param => end parameter on the straight
    @param <= geodata::curve
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    geodata::curve* line
    (
    const ::straight&,
    double start,
    double end
    )throw (std::exception);
    /*-------------------------------------------------------------------------*//**
    Gets geodata::curve from ellipse, with considering start, end parameters of the ellipse.
    @author Alexey Malov
    @param => Acis's ellipse
    @param => start parameter on the ellipse
    @param => end parameter on the ellipse
    @param <= geodata::curve
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    geodata::curve* ellipse
    (
    const ::ellipse&,
    double start,
    double end
    )throw (std::exception);
    /*-------------------------------------------------------------------------*//**
    Gets geodata::curve from intcurve, with considering start, end parameters of the intcurve.
    @author Alexey Malov
    @param => Acis's incurve
    @param => start parameter on the incurve
    @param => end parameter on the incurve
    @param <= geodata::curve
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    geodata::curve* spline
    (
    const ::intcurve&,
    double start,
    double end
    )throw (std::exception);
    /*-------------------------------------------------------------------------*//**
    Gets geodata::point from position
    @author Alexey Malov
    @param <= geodata::point
    @param => position
    *//*--------------------------------------------------------------------------*/
    void point
    (
    icl::point&,
    const SPAposition&
    );
    /*-------------------------------------------------------------------------*//**
    Gets geodata::gvector from vector
    @author Alexey Malov
    @param <= geodata::gvector 
    @param => vector
    *//*--------------------------------------------------------------------------*/
    void gvector
    (
    icl::gvector&,
    const SPAvector&
    );
    /*-------------------------------------------------------------------------*//**
    Gets geodata::gvector from unit_vector
    @author Alexey Malov
    @param <= geodata::gvector 
    @param => unit_vector
    *//*--------------------------------------------------------------------------*/
    void gvector
    (
    icl::gvector&,
    const SPAunit_vector&
    );
}

#endif
