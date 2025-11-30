// File  : 
// Author: Alexey Malov
#if _MSC_VER > 1000
#pragma once
#endif

#ifndef _ASDISPLAYOBJECT_H_
#define _ASDISPLAYOBJECT_H_

#include <list>
using std::list;
#include <vector>
#include "AsMesh.h"

class ostream;


// forward declaration
struct sds_dobjll;


enum EDisplayObjectType
{
    e3d				= 1<<0,	// not set -- points are 2D
    eClosed			= 1<<1,	// not set -- lines are open
    eFillable		= 1<<2,	// not set -- entity is not fillable
    eGdiTrueType	= 1<<3,	// not set -- points are entity other than TrueType text/mtext/etc. output by GDI
    eGdiFillpath	= 1<<4,	// this is a path of a SOLID hatch boundary or a TTF text contour (for filling by GDI)
    eHighlighted	= 1<<5, // display object should be highlighted
	eInvisible		= 1<<31 // invsible segment (for Acis internal usage)
};

class CAsDisplayObject
{
    friend class CAsObj;

    struct CNode
    {
        int m_entity;
        CNode():m_entity(0){}
        CNode(int e):m_entity(e){}

        virtual ~CNode(){}
    };
    struct CChain: public CNode
    {
        int m_type;
        int m_color;
        std::vector<point> m_pts;

        CChain():CNode(), m_type(0),m_color(0){}
        virtual ~CChain(){}
    };

    list<CNode*> m_nodes;

	typedef std::list<CNode*>::iterator dob_iterator;

public:
    void start(int);
    void end(int);

    void highlight(int);
    void unhighlight(int);

    void up(int);

public:
    // ctors
    CAsDisplayObject();
    CAsDisplayObject(const CAsDisplayObject&);
    // dtor
    virtual ~CAsDisplayObject();

    /*-------------------------------------------------------------------------*//**
    Delete display object list.
    *//*--------------------------------------------------------------------------*/
    void clear();
    /*-------------------------------------------------------------------------*//**
    Copy diplay object list.
    @param <= being filled list
    *//*--------------------------------------------------------------------------*/
	void copy(sds_dobjll*&);
    /*-------------------------------------------------------------------------*//**
    Tranform points in display objects list.
    @author Alexey Malov
    @param => transformation
    *//*--------------------------------------------------------------------------*/
    void transform(const SPAtransf&);
    /*-------------------------------------------------------------------------*//**
    Create display objects, from passed points.
    @author Alexey Malov
    @param => points
    @param => number of points
    @param => type of display object (DISP_OBJ_PTS_3D,DISP_OBJ_PTS_CLOSED,... see gr.h)
    @param => color
    *//*--------------------------------------------------------------------------*/
    void add
    (
    point aPoint[], 
    int nPoint, 
    int type, 
    int color
    );

	friend ostream & operator << (ostream & os, const CAsDisplayObject &asDO);
};

#endif
