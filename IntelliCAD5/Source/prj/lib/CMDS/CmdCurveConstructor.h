#pragma once
#ifndef _CMDCURVECONSTRUCTOR_H_
#define _CMDCURVECONSTRUCTOR_H_

// forward declarations
namespace geodata
{
    struct curve;
    struct line;
    struct ray;
    struct ellipse;
    struct ispline;
    struct aspline;
    struct pline;
}

/*-------------------------------------------------------------------------*//**
Functions to create DWG curves.
*//*--------------------------------------------------------------------------*/
namespace dbcstr
{
    /*-------------------------------------------------------------------------*//**
    Create DWG curve from passed geodata::curve with specified layer and color
    @author Alexey Malov
    @param => geometry data for creation of curve
    @param => layer name
    @param = color
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    int create
    (
    geodata::curve& crv,
    const char* szLayer = 0,
    int color = -1
    );
    /*-------------------------------------------------------------------------*//**
    Create RAY from geodata::ray with specified layer and color
    @author Alexey Malov
    @param => geometry data for creation of curve
    @param => layer name
    @param = color
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    int ray
    (
    geodata::ray&,
    const char*,
    int
    );
    /*-------------------------------------------------------------------------*//**
    Create LINE from geodata::line with specified layer and color
    @author Alexey Malov
    @param => geometry data for creation of curve
    @param => layer name
    @param = color
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    int line
    (
    geodata::line&,
    const char*,
    int
    );
    /*-------------------------------------------------------------------------*//**
    Create ELLIPSE|CIRCLE|ARC from geodata::ellipse with specified layer and color
    @author Alexey Malov
    @param => geometry data for creation of curve
    @param => layer name
    @param = color
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    int ellipse
    (
    geodata::ellipse&,
    const char*,
    int
    );
    /*-------------------------------------------------------------------------*//**
    Create SPLINE from geodata::ispline with specified layer and color
    @author Alexey Malov
    @param => geometry data for creation of curve
    @param => layer name
    @param = color
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    int spline
    (
    geodata::ispline&,
    const char*,
    int
    );
    /*-------------------------------------------------------------------------*//**
    Create SPLINE from geodata::aspline with specified layer and color
    @author Alexey Malov
    @param => geometry data for creation of curve
    @param => layer name
    @param = color
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    int spline
    (
    geodata::aspline&,
    const char*,
    int
    );
    /*-------------------------------------------------------------------------*//**
    Create POLYLINE|LWPOLYLIINE from geodata::polyline with specified layer and color
    @author Alexey Malov
    @param => geometry data for creation of curve
    @param => layer name
    @param = color
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    int pline
    (
    geodata::pline&,
    const char*,
    int
    );
    /*-------------------------------------------------------------------------*//**
    Create LWPOLYLINE from geodata::polyline with specified layer and color
    @author Alexey Malov
    @param => geometry data for creation of curve
    @param => layer name
    @param = color
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    int lwpline
    (
    geodata::pline&,
    const char*,
    int
    );
    /*-------------------------------------------------------------------------*//**
    Create POLYLINE from geodata::polyline with specified layer and color
    @author Alexey Malov
    @param => geometry data for creation of curve
    @param => layer name
    @param = color
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    int pline3d
    (
    geodata::pline&,
    const char*,
    int
    );
};
#endif
