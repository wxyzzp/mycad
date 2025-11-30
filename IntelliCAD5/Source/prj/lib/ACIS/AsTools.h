// File  : 
// Author: Alexey Malov
#pragma once
#ifndef _ASTOOLS_H_
#define _ASTOOLS_H_

#include "sds.h" //sds_matrix
#undef T
#include "xstd.h"
#include <exception>
#include "Modeler.h"
#include <iterator> //back_insert_iterator

//forward declaration
class CAsDisplayObject;
namespace geodata
{
struct curve;
}
namespace icl
{
class transf;
class point;
class gvector;
}

/******************************************************************
 * 
 * A varying-length Map class that implement associative containers 
 * with unique keys. The Key object have to implemente "==" operator.
 *
 * Author: Stephen Hou
 * Date: June 8, 2004
 *
 ******************************************************************/
template<class Key, class Value>
class Map
{
public:

    Map() {};

    // Add a value by key. The value will not be inserted if 
    // the key is found.
    //
    void    Add(const Key &key, const Value &value, bool verify = true);

    // Delete a value by a key. The value will not be removed from
    // the container if the key is not found.
    //
    bool    Delete(const Key &key);

    // Remove all elements from the container.
    //
    void    Clear() { m_Keys.clear(); m_Values.clear(); }

    // Return true if the container is empty.
    //
    bool    IsEmpty() const { return !m_Keys.size(); }

    // Find a value by a key and return true and the value if the key
    // is found.
    //
    bool    Find(const Key &key, Value &value) const;

private:

    std::vector<Key>    m_Keys;
    std::vector<Value>  m_Values;   
};


template <class Key, class Value> 
inline void Map<Key, Value>::Add(const Key &key, const Value &val, bool verify)
{
    Value value;
    if (verify && Find(key, value))
        return;

    m_Keys.push_back(key);
    m_Values.push_back(val);
}

template <class Key, class Value>
inline bool Map<Key, Value>::Delete(const Key &key)
{
    int index = -1;
    for (register i = 0; i < m_Keys.size(); i++) {
        if (m_Keys[i] == key) {
            index = i;
            break;
        }
    }
    if (index != -1) {
        m_Keys.erase(theVector.begin() + index);
        m_Values.erase(theVector.begin() + index);
        return true;
    }
    else 
        return false;
}

template <class Key, class Value>
inline bool Map<Key, Value>::Find(const Key &key, Value &val) const
{
    bool found = false;
    for (register i = 0; i < m_Keys.size(); i++) {
        if (m_Keys[i] == key) {
            val = m_Values[i];
            found = true;
        }
    }
    return found;
}


/*-------------------------------------------------------------------------*//**
Helper function to check 'outcome' value returned by Acis api functions.
In case of error function print message at MsDev output window (in Debug mode).
@param => value returned by Acis api function
@return true - for success; false - for error
*//*--------------------------------------------------------------------------*/
bool _checkOutcome(const outcome&) throw (std::exception);

#ifndef _TEST
#define checkOutcome(_X) _checkOutcome(_X)
#else
void throwException() throw (exception);

#define checkOutcome(_X) (throwException(),\
						 _checkOutcome(_X))
#endif

#ifndef _COVERAGE
#define _API_END API_END
#else
void sysError();
#define _API_END sysError(); API_END
#endif

extern const int BYLAYER_COLOR /*= 256*/;

/*-------------------------------------------------------------------------*//**
Set the color of body.
@author Stephen Hou
@param => body
@param => color value
@return TRUE if function succees..
*//*--------------------------------------------------------------------------*/
bool setBodyColor(BODY*, int color);

/*-------------------------------------------------------------------------*//**
Set the color and the faces and edges of an ACIS body if they don't
have color attributes attached and convert all RGB color values to 
color index.
@author Stephen Hou
@param => body
@param => color value
@return TRUE if function succees..
*//*--------------------------------------------------------------------------*/
bool setBodyColor(BODY*, int color, const RGBQUAD* pCurrPaletteColors);

/*-------------------------------------------------------------------------*//**
Map the colors attached to body to RGB values acconding to provided palette 
colors.
@author Stephen Hou
@param => body
@param => current palette colors
@return TRUE if function succees..
*//*--------------------------------------------------------------------------*/
bool mapBodyColorsToRGBValues(BODY* pBody, const RGBQUAD* pCurrPaletteColors);

/*-------------------------------------------------------------------------*//**
Set the color of all new faces of a body.
@author Stephen Hou
@param => body
@param => color value
@return TRUE if function succees..
*//*--------------------------------------------------------------------------*/
bool setColorToNewFaces(BODY*, int color);

/*-------------------------------------------------------------------------*//**
Finds ATTRIB_COL attached with entity and returned attrib value (color).
If there is not such attrib returns BYLAYER_COLOR.
@author Dmitry Gavrilov
@param => entity
@return color value.
*//*--------------------------------------------------------------------------*/
int getEntityColor(ENTITY*);

/*-------------------------------------------------------------------------*//**
Casting of sds_point to Acis position.
@param => sds_point to cast
@return sds_point as position reference
*//*--------------------------------------------------------------------------*/
inline 
SPAposition& asPosition(icl::point& p)
{
    return *(reinterpret_cast<SPAposition*>(&p));
}
inline
const SPAposition& asPosition(const icl::point& p)
{
    return *(reinterpret_cast<const SPAposition*>(&p));
}
/*-------------------------------------------------------------------------*//**
Casting of sds_point to Acis vector.
@param => sds_point to cast
@return sds_point as vector reference
*//*--------------------------------------------------------------------------*/
inline
SPAvector& asVector(icl::gvector& v)
{
    return *(reinterpret_cast<SPAvector*>(&v));
}
inline
const SPAvector& asVector(const icl::gvector& v)
{
    return *(reinterpret_cast<const SPAvector*>(&v));
}
/********************************************************************************
 * Author:  Dmitry Gavrilov.
 * Purpose: Create an ACIS transformation object from the given sds matrix.
 * Returns: 'true' in success and 'false' if the given sds matrix is wrong.
 ********************************************************************************/
SPAtransf acisTransf
(
const icl::transf&
)
throw (std::exception);

void checkEntity
(
ENTITY*
)
throw (std::exception);

char* saveEntity
(
ENTITY*
)
throw (std::exception);

template <typename _I>
void for_each_delete
(
_I _F,
_I _L
)
{
	for (; _F != _L; ++_F)
		delete *_F;
}

/*-------------------------------------------------------------------------*//**
Check is the body planar (all faces placed on the same plane).
@param => BODY to check
@return true - for planar body; false - if else
*//*--------------------------------------------------------------------------*/
bool isPlanar(BODY*);
/*-------------------------------------------------------------------------*//**
Creates display object, representing passed edges.
@param => edges to viualize
@param => number of first edges in list, corresponding face's boundary
@param => edge approximation precision
@param <= display object
@return 1 - for success; 0 - for error
*//*--------------------------------------------------------------------------*/
void generateDisplayObjectsFromEdges
(
std::vector<EDGE*>& edges,
int nBoundaryEdges,
double dPrecision,
CAsDisplayObject& dispObj
) throw (exception);
/*-------------------------------------------------------------------------*//**
Creates edges, defining visual presentation of passed face.
NOTE: generated edges, should be deleted by caller, EXCEPT edges from boundary.
@param => face to visualize
@param => defines visualiation mode: <= 0 - draw by mesh; > 0 - draw by isoilnes
@param <= genrated edges
@param <= number of boundary edges, thay placed first at entity list. (THEY SHOULDN'T BE DELETED)
@return 1 - for success; 0 - for error
*//*--------------------------------------------------------------------------*/
void generateEdgesToDraw
(
FACE* pFace,
int nIsolines,
std::vector<EDGE*>& edges,
int& nBoundaryEdges
)throw (exception);
/*-------------------------------------------------------------------------*//**
Gets start point of whole wire.
@param => wire to analyse
@param <= found position
*//*--------------------------------------------------------------------------*/
SPAposition getStartPoint
(
BODY* pWire
);

/*-------------------------------------------------------------------------*//**
Gets end point of whole wire.
@param => wire to analyse
@param <= found position
*//*--------------------------------------------------------------------------*/
SPAposition getEndPoint
(
BODY* pWire
);

/*-------------------------------------------------------------------------*//**
Orient wires in order, defined by array of bools. If all bools are 'true', 
then orientation of all wires should be identical.
NOTE: It's assumed, wires form path (closed or not).
@param <=> array of wires
@param => array of bolls, defined wire orientation
@param => number of wires
@param => flag tells, that path from wires is closed.
@return 1 - for success; 0 - for error
*//*--------------------------------------------------------------------------*/
int orient
(
BODY* wires[],
const bool o[],
int nwires,
bool closed
);
/*-------------------------------------------------------------------------*//**
Gets start position of coedge.
@param => coedge
@return vertex
*//*--------------------------------------------------------------------------*/
VERTEX* getStart(COEDGE*);
/*-------------------------------------------------------------------------*//**
Gets end position of coedge.
@param => coedge
@return vertex
*//*--------------------------------------------------------------------------*/
VERTEX* getEnd(COEDGE*);
/*-------------------------------------------------------------------------*//**
Set start position of coedge.
@param <=> coedge
@param => vertex
*//*--------------------------------------------------------------------------*/
void setStart(COEDGE*, VERTEX*);
/*-------------------------------------------------------------------------*//**
Set end position of coedge.
@param <=> coedge
@param => vertex
*//*--------------------------------------------------------------------------*/
void setEnd(COEDGE*, VERTEX*);
/*-------------------------------------------------------------------------*//**
Detach all solid's faces and return its in array.
@param <=> solid
@param <= faces
@return 1 - for success; 0 - for error
*//*--------------------------------------------------------------------------*/
int disassembleIntoSheets
(
BODY*, 
std::vector<BODY*>&
) throw (exception);
/*-------------------------------------------------------------------------*//**
Create dwg-curves (in the active dwg file), corresponding face boundary curves.
@param => face
@param => layer name (on which, curves should be created)
@param => color of the new curves
@return 1 - for success; 0 - for error
*//*--------------------------------------------------------------------------*/
int explodeIntoCurves
(
FACE*,
const char*,
int
);
/*-------------------------------------------------------------------------*//**
Create geodata::curves, corresponding face's boundary.
@param => face
@param <= geodata::curves
@return 1 - for success; 0 - for error
*//*--------------------------------------------------------------------------*/
int disassembleIntoCurves
(
FACE*,
xstd<geodata::curve*>::vector&
) throw (exception);
/*-------------------------------------------------------------------------*//**
Returns 2PI value.
*//*--------------------------------------------------------------------------*/
inline
double pi2()
{
    return 2*3.1415926535897932384626433832795;
}
/*-------------------------------------------------------------------------*//**
Returns PI value
*//*--------------------------------------------------------------------------*/
inline
double pi()
{
    return 3.1415926535897932384626433832795;
}
/*-------------------------------------------------------------------------*//**
Check belonging of the value, to interval, considering that interval can be periodical
@author Alexey Malov
@param => interval
@param => point to test
@param => period
@return true - if belongs; false - if not
*//*--------------------------------------------------------------------------*/
bool contain
(
const SPAinterval& range,
double p,
double period = 0.
);
/*-------------------------------------------------------------------------*//**
Check belonging of the value, to interval, considering that interval can be periodical
@author Alexey Malov
@param => start
@param => end
@param => point to test
@param => period
@return true - if belongs; false - if not
*//*--------------------------------------------------------------------------*/
bool contain
(
double start,
double end,
double value,
double period = 0.
);

int findClosestPoint
(
const curve& crv,
const SPAinterval& range,
const SPAposition& root,
const SPAunit_vector& dir,
double& param
);

bool isBoxFarThan
(
const SPAbox& bb, 
const SPAposition& root, 
const SPAunit_vector& dir, 
double distance2
);

double distanceBetweenStraights
(
const SPAposition& p1,
const SPAunit_vector& d1,
const SPAposition& p2,
const SPAunit_vector& d2,
double& param1,
double& param2
);

BODY* createInfiniteSheet
(
const icl::point&, 
const icl::gvector&
) throw (exception);

BODY* createHalfspace
(
const icl::point&, 
const icl::gvector&,
const icl::point&
) throw (exception);

void createHalfspaces
(
BODY*&,
BODY*&,
const icl::point&, 
const icl::gvector&
) throw (exception);

int getPointOnBody
(
BODY* pLump,
FACE*& pFace,
SPAposition& pt
);

template <typename type>
void recast
(
const ENTITY_LIST& el,
std::vector<type*>& ea
)
{
	int i = ea.size();
	ea.resize(ea.size() + el.count());
	el.init();
	for (type* e = 0; e = static_cast<type*>(el.next());)
		ea[i++] = e;
}

template <typename type>
void recast1
(
const ENTITY_LIST& el,
std::back_insert_iterator<type> out
)
{
	el.init();
	for (type::value_type e = 0; e = static_cast<type::value_type>(el.next());)
		out++ = e;
}

template <typename type>
void recast
(
const std::vector<type*>& ea,
ENTITY_LIST& el
)
{
	std::vector<type*>::const_iterator i = ea.begin();
	for (; i != ea.end(); ++i)
		el.add(*i);
}

/*-------------------------------------------------------------------------*//**
Move the path by its end onto the plane.
@author Alexey Malov
@param => initial wire
@param => plane
@param <= moved (adjusted) wire
@return 1 - for success; 0 - for error
*//*--------------------------------------------------------------------------*/
int adjustPathWithPlane
(
BODY* path,
PLANE* p,
BODY*& adpath
) throw (exception);

/*-------------------------------------------------------------------------*//**
Extrudes the faces along its normals to specified distance with given taper angle.
@author Alexey Malov
@param => faces to be extruded
@param => extrusion distance
@param => taper angle
@return 1 - for success; 0 - for error
*//*--------------------------------------------------------------------------*/
int extrude
(
std::vector<FACE*>& faces,
double distance,
double taper
) throw (exception);

/*-------------------------------------------------------------------------*//**
Extrudes the faces along given path.
@author Alexey Malov
@param => faces to be extruded
@param => extrusion path
@return 1 - for success; 0 - for error
*//*--------------------------------------------------------------------------*/
int extrude
(
std::vector<FACE*>& faces,
BODY* path
) throw (exception);

void detachMarkers(ENTITY_LIST&);

SURFACE* face_surface(BODY*);
BODY* solid(FACE*);
WIRE* wire(BODY*);

FACE* getFirstFaceFromSet
(
std::vector<FACE*>& faces
);

FACE* getNextFaceFromSet
(
ENTITY_LIST& set,
std::vector<FACE*>& faces
);

double resabs2();

/*-------------------------------------------------------------------------*//**
Create ray passing through rpos in direction dir, 
if ray's straight intersects box, then ray intersects box 2 times
@author Alexey Malov
@param => box
@param => position on the ray
@param => ray's direction
@param => ray's radius
@return ray
*//*--------------------------------------------------------------------------*/
ray rayBox
(
const SPAbox& bb,
const SPAposition& rpos,
const SPAunit_vector& dir,
double prec
);

typedef std::vector<hit*>::iterator hit_iterator;

/*-------------------------------------------------------------------------*//**
Intersect faces of the body with ray and returns all hits in array.
@author Alexey Malov
@param => ray
@param => object
@param <= hits
@return true if it is hit and false otherwise
*//*--------------------------------------------------------------------------*/
bool raytestFaces
(
ray& arrow,
BODY* body,
std::vector<hit*>& hits
)
throw (std::exception);

void selectHitedFaces
(
std::vector<hit*>& hits, 
xstd<HFACE>::vector& hfaces,
int mode
)
throw (std::exception);

void append
(
ENTITY_LIST& dest,
const ENTITY_LIST& src
);

int find
(
const ENTITY_LIST&,
const ENTITY*
);

#ifndef BUILD_WITH_LIMITED_ACIS_SUPPORT
CDbAcisEntityData::EType typeOfResult
(
CDbAcisEntityData::EType t1,
CDbAcisEntityData::EType t2,
BOOL_TYPE o
);
#endif

#ifdef _DEBUG
/*-------------------------------------------------------------------------*//**
Prints message to MsDev output window.
@param => format string
@param => parameters to print
*//*--------------------------------------------------------------------------*/
void printDbgMessage(const char *szFormat,...);

#define DUMP0(mess) \
    do{ printDbgMessage(mess); }while(0)
#define DUMP1(format, arg1) \
    do{ printDbgMessage(format, arg1); }while(0)
#define DUMP2(format, arg1, arg2) \
    do{ printDbgMessage(format, arg1, arg2); }while(0)

void save(ENTITY* pe1, ...);

#else

#define DUMP0(mess)
#define DUMP1(format, arg1)
#define DUMP2(format, arg1, arg2)

#endif

#endif
