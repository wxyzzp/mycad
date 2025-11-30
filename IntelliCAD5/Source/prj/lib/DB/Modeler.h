// File  :
// Author: Alexey Malov
#pragma once

#ifndef _MODELER_H_
#define _MODELER_H_
#include "DbExport.h"
#include "xstd.h"
#include <utility>
#include <memory>
#include "sds.h"
#undef T
#undef X
#undef Y
#include "DbAcisEntityData.h"

// Simple pair of geometry point and integer flag, used for passing snaping points
typedef std::pair<icl::point, int> snaping;

typedef int HEDGE;
typedef int HFACE;
typedef int HSUBENT;

// forward declarations
namespace geodata
{
struct curve;
};
namespace icl
{
class point;
class gvector;
class transf;
template <typename _Ty, int n>
struct ntype;
}


// The following two classes allow to restrict modeler functionality at run-time.
// Usage: access the single object with static CModelerLicense::get;
// set/get lincense level with CModelerLicense::setLicenseLevel/getLicenseLevel.
// License level is used when acis.dll is loaded and modeler is created (see CModeler::start).
// Note, building config macros (BUILD_WITH_ACIS_SUPPORT, etc.) can restrict licensing values, and vice versa.
// Note also, setLicenseLevel/getLicenseLevel methods are not exported for hackers pain.
// @author Dmitry Gavrilov
class CModelerLicense
{
public:
	enum ELicenseLevel {eNoSupport, eLimited, eFull};

	DB_CLASS static CModelerLicense* get();

	virtual void setLicenseLevel(ELicenseLevel level) = 0;
	virtual ELicenseLevel getLicenseLevel() const = 0;
};

class CModelerLicenseImpl : public CModelerLicense
{
public:
	virtual ~CModelerLicenseImpl() {}

private:
	friend CModelerLicense* CModelerLicense::get();

	CModelerLicenseImpl() : m_licenseLevel(eFull) {}
	void setLicenseLevel(ELicenseLevel level) { m_licenseLevel = level; }
	ELicenseLevel getLicenseLevel() const     { return m_licenseLevel; }

	ELicenseLevel	m_licenseLevel;
};


class DB_CLASS CModeler
{
protected:
    static CModeler* s_pModeler;

    // start modeler
    static int start();

public:
    // dtor
	virtual ~CModeler(){};

    /*-------------------------------------------------------------------------*//**
    Gets the modeler object.
    NOTE: the modeler is static object and should not be destroyed implicitly.
    @author Alexey Malov
    @return modeler object
    *//*--------------------------------------------------------------------------*/
    static CModeler* get();
    /*-------------------------------------------------------------------------*//**
    Check existance of modeler with full functionality.
	If existing modeler has limited or empty functionality, method return 0.
    @author Alexey Malov
    @return 1 - if exist modeler with full functionality; 0 - otherwise
    *//*--------------------------------------------------------------------------*/
	static int checkFullModeler();
    /*-------------------------------------------------------------------------*//**
    Check existance of modeler with limited functionality.
	If existing modeler has empty functionality, method return 0.
    @author Alexey Malov
    @return 1 - if exist any Acis modeler (full or viewer); 0 - otherwise
    *//*--------------------------------------------------------------------------*/
	static int checkLimitedModeler();
    /*-------------------------------------------------------------------------*//**
    Unload modeler's dll (modeler will be destroyed)
    @author Alexey Malov
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
	static int stop();

    // Methods:

    /*-------------------------------------------------------------------------*//**
    Checks: does created modeler supprt ACIS.
    @author Alexey Malov
    @return false - if existing modeler is just stub,
            true - if modeler is fully functional.
    *//*--------------------------------------------------------------------------*/
    virtual bool isStarted() = 0;
	/*-------------------------------------------------------------------------*//**
	Test modeler's functionality: can it view objects or not
	@author Alexey Malov
	@return true - if modeler can view objects; false - if modeler just stub without functionality at all
	*//*--------------------------------------------------------------------------*/
	virtual bool canView() = 0;
	/*-------------------------------------------------------------------------*//**
	Test modeler's functionality: can it create or modify objects or not.
	@author Alexey Malov
	@return true - if modeler can modify objects; false - otherwise
	*//*--------------------------------------------------------------------------*/
	virtual bool canModify() = 0;

    /*-------------------------------------------------------------------------*//**
    Free memory, allocated by modeler
    @author Alexey Malov
    @param => memory to deallocate
    *//*--------------------------------------------------------------------------*/
    virtual void free(void*) = 0;

	/*-------------------------------------------------------------------------*//**
	Clear all data stored in modeler.
	@author Alexey Malov
	*//*--------------------------------------------------------------------------*/
	virtual void clear() = 0;
    /*-------------------------------------------------------------------------*//**
    Create copy of object, identified by passed sat data, and return sat data of new object.
    (new sat data is the same, so it seems that creation and copying of real objects is not necessary.
    @author Alexey Malov
    @param pSrc => "object" to copy
    @param pDest <= new object sat data
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    virtual int copy
    (
    const CDbAcisEntityData&,
    CDbAcisEntityData&
    ) = 0;
    /*-------------------------------------------------------------------------*//**
    Transform object by passed matrix and return created sat-data.
    NOTE: ACIS needs transposed matrix,
    but now its responsibility of caller to transpose matrix before modeler call
    @author Alexey Malov
    @param => transformation
    @param => object to transform
    @param <= new sat-data
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    virtual int transform
    (
    const CDbAcisEntityData&,
    CDbAcisEntityData&,
    const icl::transf&
    ) = 0;
    /*-------------------------------------------------------------------------*//**
    Load sat-file, return sat-data of ACIS bodies from file
    @author Alexey Malov
    @param => filename
    @param => current color
    @param => current palette colors
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    virtual int loadSatFile
    (
    const char* sFilename,
    int currColor,
    const RGBQUAD* pCurrPaletteColors,
	xstd<CDbAcisEntityData>::vector&
    ) = 0;
    /*-------------------------------------------------------------------------*//**
    Store objects into sat file.
    @author Alexey Malov
    @param => filename
    @param => objects to save
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    virtual int saveSatFile
    (
    const char* sFilename,
    const std::vector<CDbAcisEntityData>& objs,
    const RGBQUAD* pCurrPaletteColors,
	int version = 0
    ) = 0;
    /*-------------------------------------------------------------------------*//**
    Creates display object list for passed sat-data
    @author Alexey Malov
    @param => "object" to visualize
    @param => number of isolines (to recieve objects as meshes, pass negative value)
    @param => tolerance value for: 1 - 'mesh mode' surface approximation tolerance; 2 - 'isoline mode' edge aproximation tolerance.
    @param <= display object list
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    virtual int display
    (
    const CDbAcisEntityData&,
    int isolines,
    double tolerance,
    sds_dobjll** ppSdsDispObjs
    ) = 0;
    /*-------------------------------------------------------------------------*//**
    Get some point from object.
    (Used as base point at command 'array' in the case of polar array without object rotation)
    @author Alexey Malov
    @param => sat-data
    @param <= point from object
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    virtual int getBasePoint
    (
    const CDbAcisEntityData&,
    icl::point&
    ) = 0;
    /*-------------------------------------------------------------------------*//**
    Get mesh representation of an object.
    @author Alexey Malov
    @param => object's sat-data
    @param <= mesh's points
    @param <= face definitions
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    virtual int getMeshRepresentation
    (
    const CDbAcisEntityData&,
    xstd<icl::point>::vector&,
    xstd<int>::vector&
    ) = 0;

    // Create Primitive Commands
    /*-------------------------------------------------------------------------*//**
    Create block.
    @author Alexey Malov
    @param => 1-st point (in UCS)
    @param => oppisite point (in UCS)
    @param => transformation from UCS to WCS
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    virtual int createBlock
    (
    const icl::point&,
    const icl::point&,
    const icl::transf&,
	int color,
	CDbAcisEntityData&
    ) = 0;
    /*-------------------------------------------------------------------------*//**
    Create sphere.
    @author Alexey Malov
    @param => center (in UCS)
    @param => radius
    @param => transformation from UCS to WCS
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    virtual int createSphere
    (
    const icl::point&,
    double,
    const icl::transf&,
	int color,
	CDbAcisEntityData&
    ) = 0;
    /*-------------------------------------------------------------------------*//**
    Create torus.
    @author Alexey Malov
    @param => center (in UCS)
    @param => major radius
    @param => minor radius
    @param => transformation from UCS to WCS
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    virtual int createTorus
    (
    const icl::point&,
    double,
    double,
    const icl::transf&,
	int color,
	CDbAcisEntityData&
    ) = 0;
    /*-------------------------------------------------------------------------*//**
    Create wedge.
    @author Alexey Malov
    @param => 1-st point in XY plane (in UCS)
    @param => 2-nd point in XY plane (in UCS)
    @param => height of the wedge
    @param => transformation from UCS to WCS
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    virtual int createWedge
    (
    const icl::point&,
    const icl::point&,
    double,
    const icl::transf&,
	int color,
	CDbAcisEntityData&
    ) = 0;
    /*-------------------------------------------------------------------------*//**
    Create cone.
    @author Alexey Malov
    @param => center of the bottom (in UCS)
    @param => radius of the bottom
    @param => center of the top (in UCS)
    @param => radius of the top
    @param => transformation from UCS to WCS
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    virtual int createCone
    (
    const icl::point&,
    double,
    const icl::point&,
    double,
    const icl::transf&,
	int color,
	CDbAcisEntityData&
    ) = 0;
    /*-------------------------------------------------------------------------*//**
    Create cone.
    @author Alexey Malov
    @param => center of the bottom (in UCS)
    @param => major axis of the bottom ellipse
    @param => ratio of axis lengths minor/major
    @param => center of the top (in UCS)
    @param => transformation from UCS to WCS
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    virtual int createCone
    (
    const icl::point&,
    const icl::gvector&,
    double,
    const icl::point&,
    const icl::transf&,
	int color,
	CDbAcisEntityData&
    ) = 0;
    /*-------------------------------------------------------------------------*//**
    Create cylinder.
    @author Alexey Malov
    @param => center of the bottom (in UCS)
    @param => radius
    @param => center of the top (in UCS)
    @param => transformation from UCS to WCS
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    virtual int createCylinder
    (
    const icl::point&,
    double,
    const icl::point& pthigh,
    const icl::transf&,
	int color,
	CDbAcisEntityData&
    ) = 0;
    /*-------------------------------------------------------------------------*//**
    Create cylinder.
    @author Alexey Malov
    @param => center of the bottom (in UCS)
    @param => major axis of the bottom ellipse
    @param => ratio of axis lengths minor/major
    @param => center of the top (in UCS)
    @param => transformation from UCS to WCS
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    virtual int createCylinder
    (
    const icl::point& bottom,
    const icl::gvector&,
    double ratio,
    const icl::point& top,
    const icl::transf&,
	int color,
	CDbAcisEntityData&
    ) = 0;
    /*-------------------------------------------------------------------------*//**
    Create dish.
    @author Alexey Malov
    @param => center (in UCS)
    @param => radius
    @param => transformation from UCS to WCS
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    virtual int createDish
    (
    const icl::point&,
    double,
    const icl::transf&,
	int color,
	CDbAcisEntityData&
    ) = 0;
    /*-------------------------------------------------------------------------*//**
    Create dome.
    @author Alexey Malov
    @param => center (in UCS)
    @param => radius
    @param => transformation from UCS to WCS
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    virtual int createDome
    (
    const icl::point&,
    double,
    const icl::transf&,
	int color,
	CDbAcisEntityData&
    ) = 0;
    /*-------------------------------------------------------------------------*//**
    Create pyramid.
    @author Alexey Malov
    @param => 1-st bottom point (in UCS)
    @param => 2-nd bottom point (in UCS)
    @param => 3-th bottom point (in UCS)
    @param => 4-th bottom point (in UCS)
    @param => 1-st top point (in UCS)
    @param => 2-nd top point (in UCS)
    @param => 3-th top point (in UCS)
    @param => 4-th top point (in UCS)
    @param => mode:
                0 - 4 points in base, 4 points in top
                1 - 4 points in base, 2 points in top (points can be equivalent)
                2 - 3 points in base, 1 point in top
                3 - 3 points in base, 3 points in top
    @param => transformation from UCS to WCS
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    virtual int createPyramid
    (
    const icl::point&,
    const icl::point&,
    const icl::point&,
    const icl::point&,
    const icl::point&,
    const icl::point&,
    const icl::point&,
    const icl::point&,
    int,
    const icl::transf&,
	int color,
	CDbAcisEntityData&
    ) = 0;
    /*-------------------------------------------------------------------------*//**
    Create ruled surface. Result object is BODY.
    @author Alexey Malov
    @param => first rail
    @param => second rail
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    virtual int createSurfaceRuled
    (
    const geodata::curve&,
    const geodata::curve&,
	int color,
	CDbAcisEntityData&
    ) = 0;
    /*-------------------------------------------------------------------------*//**
    Create surface, stretched on curves. Result object is BODY.
    @author Alexey Malov
    @param => array of curves
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    virtual int createSurfaceCurves
    (
    const std::vector<geodata::curve*>&,
	int color,
	CDbAcisEntityData&
    ) = 0;
    /*-------------------------------------------------------------------------*//**
    Create surface of revolution. Result object is BODY.
    @author Alexey Malov
    @param => profile curve
    @param => axis curve, can be LINE, XLINE, RAY, POLYLINE, LWPOLYLINE
    @param => start angle of revolution
    @param => angle of revolution
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    virtual int createSurfaceRevolved
    (
    const geodata::curve&,
    const geodata::curve&,
    double,
    double,
	int color,
	CDbAcisEntityData&
    ) = 0;
    /*-------------------------------------------------------------------------*//**
    Create surface of extrusion. Result object is BODY.
    @author Alexey Malov
    @param => profile curve
    @param => path curve
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    virtual int createSurfaceExtruded
    (
    const geodata::curve&,
    const geodata::curve&,
	int color,
	CDbAcisEntityData&
    ) = 0;
    /*-------------------------------------------------------------------------*//**
    Explode passed 3DSOLID into BODIES, corresponding faces of the solid.
    Method creates and delete corresponded objects.
    @author Alexey Malov
    @param => sat-data of the 3DSOLID
    @param => sat-data of created objects
    @param => flags for created objects, if true the corresponded object is REGION,
              else the object is BODY
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    virtual int explodeIntoBodies
    (
    const CDbAcisEntityData&,
    xstd<CDbAcisEntityData>::vector&
    ) = 0;
    /*-------------------------------------------------------------------------*//**
    Explode passed BODY/REGION into BODIES, corresponding faces,
    or into curves, corresponding edges.
    Method creates and delete corresponded objects.
    @author Alexey Malov
    @param => sat data of the 3DSOLID/BODY/REGION
    @param <= edges
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    virtual int explodeIntoCurves
    (
    const CDbAcisEntityData&,
    xstd<geodata::curve*>::vector&
    ) = 0;

    // fields correspond Acis BOOLTYPE
    enum eBoolType
    {
        eUnite = 0,
        eIntersect,
        eSubtract
    };

    /*-------------------------------------------------------------------------*//**
    General method for performing boolean union and intersection.
    @author Alexey Malov
    @param => operands
    @param <= result
    @param => code of the operation
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    virtual int boolean
    (
    const std::vector<CDbAcisEntityData>&,
    CDbAcisEntityData&,
    eBoolType
    ) = 0;
    /*-------------------------------------------------------------------------*//**
    Perform boolean union.
    @author Alexey Malov
    @param => operands
    @param <= result
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    virtual int unite
    (
    const std::vector<CDbAcisEntityData>&,
    CDbAcisEntityData&
    ) = 0;
    /*-------------------------------------------------------------------------*//**
    Perform boolean intersection.
    @author Alexey Malov
    @param => operands
    @param <= result
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    virtual int intersect
    (
    const std::vector<CDbAcisEntityData>&,
    CDbAcisEntityData&
    ) = 0;
    /*-------------------------------------------------------------------------*//**
    Perform boolean subtraction.
    @author Alexey Malov
    @param => objects to subtract from
    @param => objects to subtract
    @param <= result objects
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    virtual int subtract
    (
    const std::vector<CDbAcisEntityData>& blanks,
    const std::vector<CDbAcisEntityData>& tools,
    xstd<CDbAcisEntityData>::vector& result
    ) = 0;
    /*-------------------------------------------------------------------------*//**
    Perform check: is the passed curves connected.
    @author Alexey Malov
    @param <= result of the test
    @param => 1-st curve
    @param => 2-nd curve
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    virtual int isCurvesConnected
    (
    bool&,
    const geodata::curve&,
    const geodata::curve&
    ) = 0;
    /*-------------------------------------------------------------------------*//**
    Intersect objects from passed sets by pairs (one from first set, other from second set),
    returns these intersections and array of pairs, giving these intersections.
    @author Alexey Malov
    @param => first set of objects
    @param => second set of objects
    @param <= array of pairs, which are indices of objects, giving intersections
    @param <= array of intersections
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    virtual int interfere
    (
    const std::vector<CDbAcisEntityData>&,
    const std::vector<CDbAcisEntityData>&,
    xstd<std::pair<long, long> >::vector&,
    xstd<CDbAcisEntityData>::vector&
    ) = 0;
    /*-------------------------------------------------------------------------*//**
    Lock the cache. Forbid changes in the cache.
    Now i don't know whats is neccessary? keep entire CDbAcisEntityData unchangable or just Acis's BODY.
    @author Alexey Malov
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    virtual int lockDatabase() = 0;
    /*-------------------------------------------------------------------------*//**
    Unlock the cache.
    @author Alexey Malov
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    virtual int unlockDatabase() = 0;

	enum EEdgeSelectionMode
	{
		eOnly,
		eChain
	};
    /*-------------------------------------------------------------------------*//**
    Get the edges, intersected by the ray, from specified object.
    @author Alexey Malov
    @param
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    virtual int pickEdge
    (
    const CDbAcisEntityData&,
    const icl::point&,
    const icl::gvector&,
    double,
	int mode,
    xstd<HEDGE>::vector&
    ) = 0;
	virtual int pickLoop
	(
	const CDbAcisEntityData& obj,
	const icl::point& _root,
	const icl::gvector& _dir,
	double precision,
	HFACE hface,
	xstd<HEDGE>::vector& hedges
	) = 0;
	enum EFaceSelectionMode
	{
		eByEdge = 1 << 0,
		eByVertex = 1 << 1,
		eByFace = 1 << 2
	};
    /*-------------------------------------------------------------------------*//**
    Get the faces, intersected by the ray, from specified object.
    @author Alexey Malov
    @param
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    virtual int pickFace
    (
    const CDbAcisEntityData&,
    const icl::point&,
    const icl::gvector&,
    double,
	int mode,
    xstd<HFACE>::vector&
    ) = 0;
    /*-------------------------------------------------------------------------*//**
    Mark subentity as unhighlighted.
    @author Alexey Malov
    @param => the whole object
    @param => the unhighlighted subentity
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    virtual int unhighlight
    (
    const CDbAcisEntityData&,
    int subentity
    ) = 0;
    /*-------------------------------------------------------------------------*//**
    Mark subentity as highlighted.
    @author Alexey Malov
    @param => the whole object
    @param => the highlighted subentity
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    virtual int highlight
    (
    const CDbAcisEntityData&,
    int subentity
    ) = 0;
    /*-------------------------------------------------------------------------*//**
    Get the faces from the object.
    NOTE: for safe usage of this method, before its calling, call lockDatabase() and
    don't forget to call unlockDatabase() after usage.
    @author Alexey Malov
    @param => the object
    @param <= array of the faces
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    virtual int getFaces
    (
    const CDbAcisEntityData&,
    xstd<HFACE>::vector&
    ) = 0;
    /*-------------------------------------------------------------------------*//**
    Get the edges from the object.
    NOTE: for safe usage of this method, before its calling, call lockDatabase() and
    don't forget to call unlockDatabase() after usage.
    @author Alexey Malov
    @param => the object
    @param <= array of the edges
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    virtual int getEdges
    (
    const CDbAcisEntityData&,
    xstd<HEDGE>::vector&
    ) = 0;
    /*-------------------------------------------------------------------------*//**
    Get the edges belonging passed subentity.
    NOTE: for safe usage of this method, before its calling, call lockDatabase() and
    don't forget to call unlockDatabase() after usage.
    @author Alexey Malov
    @param => the subentity handle
    @param <= array of the edges
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
	virtual int getEdges
	(
	HSUBENT hentity,
    xstd<HEDGE>::vector& edges
	) = 0;
	/*-------------------------------------------------------------------------*//**
    Moves each given face with given transformation.
    @author Alexey Malov
    @param src => initial sat-data
    @param dest <= result sat-data
    @param faces => faces being tapered
    @param mtx => transformation matrix
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    virtual int moveFaces
    (
    const CDbAcisEntityData&,
    CDbAcisEntityData&,
    const std::vector<HFACE>&,
    const icl::transf&
    ) = 0;
    /*-------------------------------------------------------------------------*//**
    Tapers each given face about the intersection between the surface
    underlying the face and a given plane by a given draft angle.
    @author Alexey Malov
    @param dest <= result sat-data (memory alocated by Acis, to free it user should call CModeler::free()
    @param src => initial sat-data
    @param faces => faces being tapered
    @param root => draft plane point
    @param normal => draft plane normal
    @param angle => draft angle
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    virtual int taperFaces
    (
    const CDbAcisEntityData& src,
    CDbAcisEntityData& dest,
    const std::vector<HFACE>& faces,
    const icl::point& _root,
    const icl::gvector& _normal,
    double angle
    ) = 0;
    /*-------------------------------------------------------------------------*//**
    Offsets given faces in given body on given distance.
    @author Alexey Malov
    @param <= new object
    @param => object
    @param => ofsseted faces
    @param => offset distance
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    virtual int offsetFaces
    (
    const CDbAcisEntityData& src,
    CDbAcisEntityData& dest,
    const std::vector<HFACE>& faces,
    double offset
    ) = 0;
    /*-------------------------------------------------------------------------*//**
    Rotates given faces in given object (if object's geometry allows this)
    @author Alexey Malov
    @param <= new object
    @param => object
    @param => selected faces to rotate
    @param => point on the rotation axis (in WCS)
    @param => direction of rotation axis (in WCS)
    @param => angle of rotation (in what direction?)
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    virtual int rotateFaces
    (
    const CDbAcisEntityData& src,
    CDbAcisEntityData& dest,
    const std::vector<HFACE>& faces,
    const icl::point& root,
    const icl::gvector& dir,
    double angle
    ) = 0;
    /*-------------------------------------------------------------------------*//**
    Deletes given faces from given object (if object's geometry allows this)
    @author Alexey Malov
    @param <= new object
    @param => object
    @param => faces, to be deleted
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    virtual int deleteFaces
    (
    const CDbAcisEntityData& src,
    CDbAcisEntityData& dest,
    const std::vector<HFACE>& faces
    ) = 0;
    /*-------------------------------------------------------------------------*//**
    Applies given transformation to the given faces and returns its sat-data
    @author Alexey Malov
    @param => object, having faces
    @param => faces (source)
    @param <= faces (destination)
    @param <= flags, telling that i-th face is planar and should REGION
    @param => transformation
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    virtual int copyFaces
    (
    const CDbAcisEntityData&,
    const std::vector<HFACE>&,
    xstd<CDbAcisEntityData>::vector&,
    const icl::transf&
    ) = 0;
    /*-------------------------------------------------------------------------*//**
    Applies given transformation to the given edges and returns its as curves
    Copies given edges with given transformations into curves
    @author Alexey Malov
    @param => object, having edges
    @param => edges (source)
    @param <= curves (destination)
    @param => transformation
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    virtual int copyEdges
    (
    const CDbAcisEntityData& obj,
    const std::vector<HEDGE>& edges,
    xstd<geodata::curve*>::vector& curves,
    const icl::transf& mtx
    ) = 0;
    /*-------------------------------------------------------------------------*//**
    Change color of given ACIS entity(body)
    @author Stephen Hou
    @param => old body data
    @param => color
    @param <= optional new body data
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    virtual int colorModelEntity
    (
    const CDbAcisEntityData& oldModelData,
    int color,
    CDbAcisEntityData& newModelData
    ) = 0;
    /*-------------------------------------------------------------------------*//**
    Change color of given faces in given object
    @author Alexey Malov
    @param <= new object
    @param => old object
    @param => faces to color
    @param => color
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    virtual int colorFaces
    (
    const CDbAcisEntityData&,
    CDbAcisEntityData&,
    const std::vector<HFACE>&,
    int
    ) = 0;
    /*-------------------------------------------------------------------------*//**
    Change color of given edges in given object
    @author Alexey Malov
    @param <= new object
    @param => old object
    @param => edges to color
    @param => color
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    virtual int colorEdges
    (
    const CDbAcisEntityData&,
    CDbAcisEntityData&,
    const std::vector<HEDGE>&,
    int
    ) = 0;
    /*-------------------------------------------------------------------------*//**
    Calculates snaping points from the object,
    in correspondence with passed 'object snaping mode'.
    @author Alexey Malov
    @param => object
    @param => snaping mode
    @param => pick point
	@param => last point
    @param => view direction
    @param => snaping points
    @param => mode, corresponded to snaping point
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    virtual int getSnapingPoints
    (
    const CDbAcisEntityData& sat,
    int osmode,
    const icl::point& pick,
	const icl::point& last,
    const icl::gvector& dir,
	xstd<snaping>::vector& pnts
    ) = 0;
    /*-------------------------------------------------------------------------*//**
    Returns number of faces of the object.
    @author Alexey Malov
    @param => object
    @param <= number of faces
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    virtual int getNumberOfFaces
    (
    const CDbAcisEntityData&,
    int& nfaces
    ) = 0;
    /*-------------------------------------------------------------------------*//**
    Extrude given faces along given path.
    @author Alexey Malov
    @param <= new object
    @param => old object
    @param => selected faces of the object
    @param => extrusion path
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    virtual int extrudeFacesAlongPath
    (
    const CDbAcisEntityData&,
    CDbAcisEntityData&,
    const std::vector<HFACE>&,
    const geodata::curve&
    ) = 0;
    /*-------------------------------------------------------------------------*//**
    Extrude given faces on given distance with given taper angle.
    @author Alexey Malov
    @param <= new object
    @param => old object
    @param => selected faces of the object
    @param => extrusion distance
    @param => taper angle
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    virtual int extrudeFaces
    (
    const CDbAcisEntityData&,
    CDbAcisEntityData&,
    const std::vector<HFACE>&,
    double,
    double
    ) = 0;
    /*-------------------------------------------------------------------------*//**
    Imprints one Acis object into another.
    @author Alexey Malov
    @param <= result
    @param => blank
    @param => curves
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
	virtual int imprint
	(
	const CDbAcisEntityData& blank,
	const CDbAcisEntityData& tool,
	CDbAcisEntityData& dest,
	bool erase
	) = 0;

	virtual int imprint
	(
	const CDbAcisEntityData&,
	const std::vector<geodata::curve*>&,
	CDbAcisEntityData&
	) = 0;
    /*-------------------------------------------------------------------------*//**
    Separates, multiply connected object
    @author Alexey Malov
    @param <= new objects
    @param => initial object
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
	virtual int separate
	(
	const CDbAcisEntityData&,
    xstd<CDbAcisEntityData>::vector&
	) = 0;
    /*-------------------------------------------------------------------------*//**
    Removes all edges, faces and associated data, that are not necessary to support
    the topology of the object
    @author Alexey Malov
    @param <= new object
    @param => initial object
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
	virtual int clean
	(
	const CDbAcisEntityData&,
	CDbAcisEntityData&
	) = 0;
    /*-------------------------------------------------------------------------*//**
    Check object's geometry and topoplogy for errors.
    @author Alexey Malov
    @param => object to be checked
    @param <= error messages
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
	virtual int check
	(
	const CDbAcisEntityData&,
    xstd<icl::string>::vector&
	) = 0;
    /*-------------------------------------------------------------------------*//**
    Creates a thin walled solid shell from a object
    @author Alexey Malov
    @param <= new object
    @param => shelled object
    @param => faces from initial object to be omited
    @param => shelling distance
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    virtual int shell
    (
    const CDbAcisEntityData&,
    CDbAcisEntityData&,
    const std::vector<HFACE>&,
    double
    ) = 0;
    /*-------------------------------------------------------------------------*//**
    Creates a set of regions from given curves, which could be either open or
	closed but they must form closed loops. Only closed planar loops without
	self-intersections will create the regions. Only qualified curves that form
	closed loops are used to create the regions.

    @author Stephen W. Hou
    @param <= a list of curves
	@param => regions created
	@param => the curves used to create regions
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    virtual int region
    (
    const std::vector<geodata::curve*>&,
	xstd<CDbAcisEntityData>::vector&,
	int color,
	std::list<geodata::curve*> &qualifiedCurves
    ) = 0;
    /*-------------------------------------------------------------------------*//**
    Create a region from a closed planar curve.
    @author Stephen W. Hou
    @param <= a closed planar curve to create region
	@param => a region
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
	virtual int region
	(
	const geodata::curve* pCurve,
	int color,
	CDbAcisEntityData &region
    ) = 0;
	/*-------------------------------------------------------------------------*//**
	Gets planar section of the object.
	@author Alexey Malov
	@param => object for sectioning
	@param <= result
	@param => point on section plane
	@param => normal of the section plane
	@return 1 - for success; 0 - for error
	*//*--------------------------------------------------------------------------*/
	virtual int section
	(
	const CDbAcisEntityData&,
	CDbAcisEntityData&,
	const icl::point&,
	const icl::gvector&
	) = 0;
	/*-------------------------------------------------------------------------*//**
	Slice the object; if test point is given, then returned just one part, including the point.
	@author Alexey Malov
	@param => object for slicing
	@param <= sliced parts of initial object
	@param => point on the plane
	@param => normal of the plane
	@param => point, defining plane side, containing desired part of the object;
			  if 0, both parts are returned
	@return 1 - for success; 0 - for error
	*//*--------------------------------------------------------------------------*/
	virtual int slice
	(
	const CDbAcisEntityData&,
	CDbAcisEntityData[2],
	const icl::point&,
	const icl::gvector&,
	const icl::point*
	) = 0;
	/*-------------------------------------------------------------------------*//**
	Extrudes region on given distance, with tapering.
	@author Alexey Malov
	@param => object to extrude
	@param <= result
	@param => extrude distance
	@param => angle of tapering
	@return 1 - for success; 0 - for error
	*//*--------------------------------------------------------------------------*/
	virtual int extrude
	(
	const CDbAcisEntityData&,
	CDbAcisEntityData&,
	double dist,
	double taper,
	int color
	) = 0;
	/*-------------------------------------------------------------------------*//**
	Extrudes region along the path.
	@author Alexey Malov
	@param => object to extrude
	@param <= result
	@param => extruding path
	@return 1 - for success; 0 - for error
	*//*--------------------------------------------------------------------------*/
	virtual int extrude
	(
	const CDbAcisEntityData&,
	CDbAcisEntityData&,
	const geodata::curve& _path,
	int color
	) = 0;
	/*-------------------------------------------------------------------------*//**
	Creates solid of revolution from region.
	@author Alexey Malov
	@param => region to revolve
	@param <= result
	@param => point on axis of revolution
	@param => direction of the axis
	@param => angle of revolution
	@return 1 - for success; 0 - for error
	*//*--------------------------------------------------------------------------*/
	virtual int revolve
	(
	const CDbAcisEntityData&,
	CDbAcisEntityData&,
	const icl::point&,
	const icl::gvector&,
	double angle,
	int color
	) = 0;
	/*-------------------------------------------------------------------------*//**
	Gets array of grips (ends of edges and centers of spheres and circles).
	@author Alexey Malov
	@param => object
	@param <= array of grips
	@return 1 - for success; 0 - for error
	*//*--------------------------------------------------------------------------*/
	virtual int getGrips
	(
	const CDbAcisEntityData&,
	xstd<icl::point>::vector&
	) = 0;
	/*-------------------------------------------------------------------------*//**
	Gets the bounding box of entity, for cordinate system passed through w2u.
	@author Alexey Malov
	@param => object
	@param <= lowest point of the bounding box
	@param <= highest point of the bunding box
	@param => transformation from wcs (where entity defined) to the current user coordinate system
	@return 1 - for success; 0 - for error
	*//*--------------------------------------------------------------------------*/
	virtual int getBoundingBox
	(
	const CDbAcisEntityData&,
	icl::point& low,
	icl::point& high,
	const icl::transf& w2u
	) = 0;
	/*-------------------------------------------------------------------------*//**
	Calculates perimeter of the region. NOTE: appliable only for REGIONs.
	If given object is not REGION, returns 0.
	@author Alexey Malov
	@param => object
	@param <= perimeter
	@return 1 - for success; 0 - for error
	*//*--------------------------------------------------------------------------*/
	virtual int getPerimeter
	(
	const CDbAcisEntityData&,
	double&
	) = 0;
	/*-------------------------------------------------------------------------*//**
	Calculates area of the region. NOTE: appliable only for REGIONs.
	If given object is not REGION, returns 0.
	@author Alexey Malov
	@param => object
	@param <= area
	@return 1 - for success; 0 - for error
	*//*--------------------------------------------------------------------------*/
	virtual int getArea
	(
	const CDbAcisEntityData&,
	double&
	) = 0;
	/*-------------------------------------------------------------------------*//**
	Gets sat data in appropriate version for Acad14
	@author Alexey Malov
	@param => object
	@param <= sat-data in required version
	@param => required version
	@return 1 - for success; 0 - for error
	*//*--------------------------------------------------------------------------*/
	virtual int getSatInVersion
	(
	const CDbAcisEntityData&,
	icl::string&,
	int
	) = 0;
	/*-------------------------------------------------------------------------*//**
	Gets code of the last error.
	@author Alexey Malov
	@return 0 - if it was not error, otherwise nonzero error code
	*//*--------------------------------------------------------------------------*/
	virtual int getLastErrorCode() = 0;
	/*-------------------------------------------------------------------------*//**
	Gets error message, corresponding to error code, returned by getLastErrorCode.
	@author Alexey Malov
	@param => error code
	@param <= error message
	@return 1 - for success; 0 - for error
	*//*--------------------------------------------------------------------------*/
	virtual int getErrorMessage
	(
	int,
	icl::string&
	) = 0;
	/*-------------------------------------------------------------------------*//**
	Gets mass properties for system of bodies.
	@author Alexey Malov
	@param => objects
	@param <= volume
	@param <= center of gravity
	@param <= moments of inertia for WCS
	@param <= products of inertia for WCS
	@param <= radii of gyration
	@param <= principal moments
	@param <= principal axes
	@return 1 - for success; 0 - for error
	*//*--------------------------------------------------------------------------*/
	virtual int getMassProperties
	(
	const std::vector<CDbAcisEntityData>&,
	double& volume,
	icl::point& center_of_gravity,
	icl::ntype<double,3>& moments_of_inertia,
	icl::ntype<double,3>& products_of_inertia,
	icl::ntype<double,3>& radii_of_giration,
	icl::ntype<double,3>& principal_moments_of_inertia,
	icl::ntype<icl::gvector,3>& principal_axes_of_inertia
	) = 0;

	/*-------------------------------------------------------------------------*//**
	Creates solid from mesh
	@author Alexey Malov
	@param points => mesh vertices
	@param faces => face descriptions, indices of corresponded vertices, increased by 1
	@return 1 - for success; 0 - for error
	*//*--------------------------------------------------------------------------*/
	virtual int reassembleFromMesh
	(
	CDbAcisEntityData& data,
	const std::vector<icl::point>& points,
	const std::vector<int>& faces
	) = 0;

	/*-------------------------------------------------------------------------*//**
	Create blending on passed edges
	@author Alexey Malov
	@param in_data => data of initial objects
	@param out_data <= data of object with blended edges
	@param edges_wit_radius <= array of edges with corresponding radius for blending
	@return 1 - for success; 0 - for error
	*//*--------------------------------------------------------------------------*/
	virtual int fillet
    (
	const CDbAcisEntityData& in_data,
	CDbAcisEntityData& out_data,
	int color,
	const std::vector<std::pair<HEDGE, double> >& edges_with_radius
	) = 0;

	/*-------------------------------------------------------------------------*//**
	Test passed edge for possibity to blend this edge
	@author Alexey Malov
	@param => object, owing tested edge
	@param => edge identigier
	@param <= boolean value, defining is the edge approporiate for blending
	@return 1 - for success; 0 - for error
	*//*--------------------------------------------------------------------------*/
	virtual int isEdgeAppropriateForBlending
	(
	const CDbAcisEntityData&,
	HEDGE,
	bool& isAppropriate
	) = 0;

	virtual int chamfer
	(
   	const CDbAcisEntityData&,
	CDbAcisEntityData&,
	const std::vector<HEDGE>&,
	double baseChamferDistance,
	double otherChamferDistance,
	int color
	) = 0;
 };

#endif
