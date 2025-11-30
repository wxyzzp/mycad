// File  :
// Author: Alexey Malov
#if _MSC_VER > 1000
#pragma once
#endif

#ifndef _ASMODELER_H_
#define _ASMODELER_H_

#define AS_USE_SDS 0

#include "Acis.h"
#include "Modeler.h"
#include "AsCache.h"
#include "AsObject.h"

class CAsViewer: public CModeler
{
    friend ACIS_API CModeler* createViewer();
    friend class CAsCache;

protected:
   	enum EAcisModules
	{
		eNone		= 0,
		// CAsViewer initializes:
		eBase		= 0x0001,
		eLaw		= 0x0002,
		eKernel		= 0x0004,
		eSpline		= 0x0008,
		eFaceter	= 0x0010,
		eRendering	= 0x0020,
		// CAsModeler initializes:
		eGenericAttributes	= 0x000040,
		eIntersectors		= 0x000080,
		eConstructors		= 0x000100,
		eSweeping			= 0x000200,
		eSkinning			= 0x000400,
		eCovering			= 0x000800,
		eBooleans			= 0x001000,
		eLocalOps			= 0x002000,
		eFaceRemoval		= 0x004000,
		eEulerOps			= 0x008000,
		eShelling			= 0x010000,
		eBlending			= 0x020000
	};

// data:
    CAsCache m_cache;
	static long m_initedAcisModules;

    //ctor
    CAsViewer() throw (exception);

    /*-------------------------------------------------------------------------*//**
    Create BODY from sat data
    @author Alexey Malov
    @param => sat data
    @return BODY - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    CAsObj* create(const CAsCache::key_t&);

	static void initializeViewerAcisDlls() throw (exception);
	static void terminateViewerAcisDlls();

public:
    //dtor
    virtual ~CAsViewer();

    // Methods:
    /*-------------------------------------------------------------------------*//**
    Free memory, allocated by modeler
    @author Alexey Malov
    @param => memory to deallocate
    *//*--------------------------------------------------------------------------*/
    virtual void free(void*);
	/*-------------------------------------------------------------------------*//**
	Clear all data stored in modeler.
	@author Alexey Malov
	*//*--------------------------------------------------------------------------*/
	virtual void clear();
	/*-------------------------------------------------------------------------*//**
	Can the modeler diplay and query Acis objects.
	@author Alexey Malov
	@return true
	*//*--------------------------------------------------------------------------*/
	virtual bool canView() {return true;}
    virtual bool isStarted() {return true;}
	/*-------------------------------------------------------------------------*//**
	Can the modeler create and edit Acis objects.
	@author Alexey Malov
	@return false
	*//*--------------------------------------------------------------------------*/
	virtual bool canModify() {return false;}

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
    );
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
    );
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
	);
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
	);
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
	);
	/*-------------------------------------------------------------------------*//**
	Gets code of the last error.
	@author Alexey Malov
	@return 0 - if it was not error, otherwise nonzero error code
	*//*--------------------------------------------------------------------------*/
	virtual int getLastErrorCode();
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
	);

	// empty implementation
    virtual int copy
    (
    const CDbAcisEntityData&,
    CDbAcisEntityData&
    ){return 0;}
    virtual int transform
    (
    const CDbAcisEntityData&,
    CDbAcisEntityData&,
    const icl::transf&
    ){return 0;}
    virtual int loadSatFile
    (
    const char* sFilename,
    int currColor,
    const RGBQUAD* pCurrPaletteColors,
	xstd<CDbAcisEntityData>::vector&
    ){return 0;}
    virtual int saveSatFile
    (
    const char* sFilename,
    const std::vector<CDbAcisEntityData>&,
    const RGBQUAD* pCurrPaletteColors,
	int version = 0
    ){return 0;}
    virtual int getBasePoint
    (
    const CDbAcisEntityData&,
    icl::point&
    ){return 0;}
    virtual int getMeshRepresentation
    (
    const CDbAcisEntityData&,
    xstd<icl::point>::vector&,
    xstd<int>::vector&
    ){return 0;}
    virtual int createBlock
    (
    const icl::point&,
    const icl::point&,
    const icl::transf&,
	int color,
	CDbAcisEntityData&
    ){return 0;}
    virtual int createSphere
    (
    const icl::point&,
    double,
    const icl::transf&,
	int color,
	CDbAcisEntityData&
    ){return 0;}
    virtual int createTorus
    (
    const icl::point&,
    double,
    double,
    const icl::transf&,
	int color,
	CDbAcisEntityData&
    ){return 0;}
    virtual int createWedge
    (
    const icl::point&,
    const icl::point&,
    double,
    const icl::transf&,
	int color,
	CDbAcisEntityData&
    ){return 0;}
    virtual int createCone
    (
    const icl::point&,
    double,
    const icl::point&,
    double,
    const icl::transf&,
	int color,
	CDbAcisEntityData&
    ){return 0;}
    virtual int createCone
    (
    const icl::point&,
    const icl::gvector&,
    double,
    const icl::point&,
    const icl::transf&,
	int color,
	CDbAcisEntityData&
    ){return 0;}
    virtual int createCylinder
    (
    const icl::point&,
    double,
    const icl::point& pthigh,
    const icl::transf&,
	int color,
	CDbAcisEntityData&
    ){return 0;}
    virtual int createCylinder
    (
    const icl::point& bottom,
    const icl::gvector&,
    double ratio,
    const icl::point& top,
    const icl::transf&,
	int color,
	CDbAcisEntityData&
    ){return 0;}
    virtual int createDish
    (
    const icl::point&,
    double,
    const icl::transf&,
	int color,
	CDbAcisEntityData&
    ){return 0;}
    virtual int createDome
    (
    const icl::point&,
    double,
    const icl::transf&,
	int color,
	CDbAcisEntityData&
    ){return 0;}
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
    ){return 0;}
    virtual int createSurfaceRuled
    (
    const geodata::curve&,
    const geodata::curve&,
	int color,
	CDbAcisEntityData&
    ){return 0;}
    virtual int createSurfaceCurves
    (
    const std::vector<geodata::curve*>&,
	int color,
	CDbAcisEntityData&
    ){return 0;}
    virtual int createSurfaceRevolved
    (
    const geodata::curve&,
    const geodata::curve&,
    double,
    double,
	int color,
	CDbAcisEntityData&
    ){return 0;}
    virtual int createSurfaceExtruded
    (
    const geodata::curve&,
    const geodata::curve&,
	int color,
	CDbAcisEntityData&
    ){return 0;}
    virtual int explodeIntoBodies
    (
    const CDbAcisEntityData&,
    xstd<CDbAcisEntityData>::vector&
    ){return 0;}
    virtual int explodeIntoCurves
    (
    const CDbAcisEntityData&,
    xstd<geodata::curve*>::vector&
    ){return 0;}
    virtual int boolean
    (
    const std::vector<CDbAcisEntityData>&,
    CDbAcisEntityData&,
	eBoolType
    ){return 0;}
    virtual int unite
    (
    const std::vector<CDbAcisEntityData>&,
    CDbAcisEntityData&
    ){return 0;}
    virtual int intersect
    (
    const std::vector<CDbAcisEntityData>&,
    CDbAcisEntityData&
    ){return 0;}
    virtual int subtract
    (
    const std::vector<CDbAcisEntityData>& blanks,
    const std::vector<CDbAcisEntityData>& tools,
    xstd<CDbAcisEntityData>::vector& result
    ){return 0;}
    virtual int isCurvesConnected
    (
    bool&,
    const geodata::curve&,
    const geodata::curve&
    ){return 0;}
    virtual int interfere
    (
    const std::vector<CDbAcisEntityData>&,
    const std::vector<CDbAcisEntityData>&,
    xstd<std::pair<long, long> >::vector&,
    xstd<CDbAcisEntityData>::vector&
    ){return 0;}
    virtual int lockDatabase(){return 0;}
    virtual int unlockDatabase(){return 0;}
    virtual int pickEdge
    (
    const CDbAcisEntityData&,
    const icl::point&,
    const icl::gvector&,
    double,
	int,
    xstd<HEDGE>::vector&
    ){return 0;}
	int pickLoop
	(
	const CDbAcisEntityData& obj,
	const icl::point& _root,
	const icl::gvector& _dir,
	double precision,
	HFACE hface,
	xstd<HEDGE>::vector& hedges
	){return 0;}
    virtual int pickFace
    (
    const CDbAcisEntityData&,
    const icl::point&,
    const icl::gvector&,
    double,
	int,
    xstd<HFACE>::vector&
    ){return 0;}
    virtual int unhighlight
    (
    const CDbAcisEntityData&,
    int subentity
    ){return 0;}
    virtual int highlight
    (
    const CDbAcisEntityData&,
    int subentity
    ){return 0;}
    virtual int getFaces
    (
    const CDbAcisEntityData&,
    xstd<HFACE>::vector&
    ){return 0;}
    virtual int getEdges
    (
    const CDbAcisEntityData&,
    xstd<HEDGE>::vector&
    ){return 0;}
	virtual int getEdges
	(
	HSUBENT hentity,
    xstd<HEDGE>::vector& edges
	){return 0;}
    virtual int moveFaces
    (
    const CDbAcisEntityData&,
    CDbAcisEntityData&,
    const std::vector<HFACE>&,
    const icl::transf&
    ){return 0;}
    virtual int taperFaces
    (
    const CDbAcisEntityData& src,
    CDbAcisEntityData& dest,
    const std::vector<HFACE>& faces,
    const icl::point& _root,
    const icl::gvector& _normal,
    double angle
    ){return 0;}
    virtual int offsetFaces
    (
    const CDbAcisEntityData& src,
    CDbAcisEntityData& dest,
    const std::vector<HFACE>& faces,
    double offset
    ){return 0;}
    virtual int rotateFaces
    (
    const CDbAcisEntityData& src,
    CDbAcisEntityData& dest,
    const std::vector<HFACE>& faces,
    const icl::point& root,
    const icl::gvector& dir,
    double angle
    ){return 0;}
    virtual int deleteFaces
    (
    const CDbAcisEntityData& src,
    CDbAcisEntityData& dest,
    const std::vector<HFACE>& faces
    ){return 0;}
    virtual int copyFaces
    (
    const CDbAcisEntityData&,
    const std::vector<HFACE>&,
    xstd<CDbAcisEntityData>::vector&,
    const icl::transf&
    ){return 0;}
    virtual int copyEdges
    (
    const CDbAcisEntityData& obj,
    const std::vector<HEDGE>& edges,
    xstd<geodata::curve*>::vector& curves,
    const icl::transf& mtx
    ){return 0;}
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
    ){ return 0; }
    virtual int colorFaces
    (
    const CDbAcisEntityData&,
    CDbAcisEntityData&,
    const std::vector<HFACE>&,
    int
    ){return 0;}
    virtual int colorEdges
    (
    const CDbAcisEntityData&,
    CDbAcisEntityData&,
    const std::vector<HEDGE>&,
    int
    ){return 0;}
    virtual int getNumberOfFaces
    (
    const CDbAcisEntityData&,
    int& nfaces
    ){return 0;}
    virtual int extrudeFacesAlongPath
    (
    const CDbAcisEntityData&,
    CDbAcisEntityData&,
    const std::vector<HFACE>&,
    const geodata::curve&
    ){return 0;}
    virtual int extrudeFaces
    (
    const CDbAcisEntityData&,
    CDbAcisEntityData&,
    const std::vector<HFACE>&,
    double,
    double
    ){return 0;}
	virtual int imprint
	(
	const CDbAcisEntityData&,
	const std::vector<geodata::curve*>&,
	CDbAcisEntityData&
	){return 0;}
	virtual int imprint
	(
	const CDbAcisEntityData& blank,
	const CDbAcisEntityData& tool,
	CDbAcisEntityData& dest,
	bool erase
	){return 0;}
	virtual int separate
	(
	const CDbAcisEntityData&,
    xstd<CDbAcisEntityData>::vector&
	){return 0;}
	virtual int clean
	(
	const CDbAcisEntityData&,
	CDbAcisEntityData&
	){return 0;}
	virtual int check
	(
	const CDbAcisEntityData&,
    xstd<icl::string>::vector&
	){return 0;}
    virtual int shell
    (
    const CDbAcisEntityData&,
    CDbAcisEntityData&,
    const std::vector<HFACE>&,
    double
    ){return 0;}
    virtual int region
    (
    const std::vector<geodata::curve*>&,
	xstd<CDbAcisEntityData>::vector&,
	int color,
	std::list<geodata::curve*> &curveInLoops
    ){return 0;}
	virtual int region
	(
	const geodata::curve* pCurve, 
	int color,
	CDbAcisEntityData &region
    ){return 0;}
	virtual int section
	(
	const CDbAcisEntityData&,
	CDbAcisEntityData&,
	const icl::point&,
	const icl::gvector&
	){return 0;}
	virtual int slice
	(
	const CDbAcisEntityData&,
	CDbAcisEntityData[2],
	const icl::point&,
	const icl::gvector&,
	const icl::point*
	){return 0;}
	virtual int extrude
	(
	const CDbAcisEntityData&,
	CDbAcisEntityData&,
	double dist,
	double taper,
	int color
	){return 0;}
	virtual int extrude
	(
	const CDbAcisEntityData&,
	CDbAcisEntityData&,
	const geodata::curve& _path,
	int color
	){return 0;}
	virtual int revolve
	(
	const CDbAcisEntityData&,
	CDbAcisEntityData&,
	const icl::point&,
	const icl::gvector&,
	double angle,
	int color
	){return 0;}
	virtual int getArea
	(
	const CDbAcisEntityData&,
	double&
	){return 0;}
	virtual int getSatInVersion
	(
	const CDbAcisEntityData&,
	icl::string&,
	int
	){return 0;}
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
	){return 0;}
	virtual int reassembleFromMesh
	(
	CDbAcisEntityData& data,
	const std::vector<icl::point>& points,
	const std::vector<int>& faces
	){return 0;}
	virtual int fillet
    (
	const CDbAcisEntityData& in_data,
	CDbAcisEntityData& out_data,
	int color,
	const std::vector<std::pair<HEDGE, double> >& edges_with_radius
	){return 0;}
	virtual int isEdgeAppropriateForBlending
	(
	const CDbAcisEntityData&,
	HEDGE,
	bool& isAppropriate
	){return 0;}
	virtual int chamfer
	(
   	const CDbAcisEntityData&,
	CDbAcisEntityData&,
	const std::vector<HEDGE>&,
	double baseChamferDistance,
	double otherChamferDistance,
	int color
	){return 0;}
};

#ifndef BUILD_WITH_LIMITED_ACIS_SUPPORT

class CAsModeler: public CAsViewer
{
protected:
    friend ACIS_API CModeler* createModeler();
	friend void safeInitializeModelerAcisDlls();
	friend LONG DelayLoadDllExceptionFilter(PEXCEPTION_POINTERS pep);
    // ctor
    CAsModeler() throw (exception);

	static void initializeModelerAcisDlls();
	static void terminateModelerAcisDlls();

public:
    // dtor
    virtual ~CAsModeler();

    /*-------------------------------------------------------------------------*//**
    Checks: does created modeler supprt ACIS.
    @author Alexey Malov
    @return false - if existing modeler is just stub,
            true - if modeler is fully functional.
    *//*--------------------------------------------------------------------------*/
    virtual bool isStarted() {return true;}
	/*-------------------------------------------------------------------------*//**
	Can the modeler create and edit Acis objects.
	@author Alexey Malov
	@return true
	*//*--------------------------------------------------------------------------*/
	virtual bool canModify() {return true;}
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
    );
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
    );
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
    );
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
    const std::vector<CDbAcisEntityData>&,
    const RGBQUAD* pCurrPaletteColors,
	int version = 0
    );
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
    );
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
    );

    // Create Primitive Commands
    /*-------------------------------------------------------------------------*//**
    Create block.
    @author Alexey Malov
    @param => 1-st point (in UCS)
    @param => opposite point (in UCS)
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
    );
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
    );
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
    );
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
    );
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
    );
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
    );
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
    );
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
    );
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
    );
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
    );
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
    );
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
    );
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
    );
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
    );
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
    );
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
    );
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
    );

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
    );
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
    );
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
    );
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
    );
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
    );
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
    );
    /*-------------------------------------------------------------------------*//**
    Lock the cache. Forbid changes in the cache.
    Now i don't know whats is neccessary? keep entire CDbAcisEntityData unchangable or just Acis's BODY.
    @author Alexey Malov
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    virtual int lockDatabase();
    /*-------------------------------------------------------------------------*//**
    Unlock the cache.
    @author Alexey Malov
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    virtual int unlockDatabase();
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
	int,
    xstd<HEDGE>::vector&
    );
    /*-------------------------------------------------------------------------*//**
    Get the edges, intersected by the ray, from specified object.
    @author Alexey Malov
    @param
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
	int pickLoop
	(
	const CDbAcisEntityData& obj,
	const icl::point& _root,
	const icl::gvector& _dir,
	double precision,
	HFACE hface,
	xstd<HEDGE>::vector& hedges
	);
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
	int,
    xstd<HFACE>::vector&
    );
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
    );
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
    );
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
    );
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
    );
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
	);
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
    );
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
    );
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
    );
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
    );
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
    );
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
    );
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
    );
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
    );
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
    );
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
    );
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
    );
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
    );
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
    );
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
	);

	virtual int imprint
	(
	const CDbAcisEntityData&,
	const std::vector<geodata::curve*>&,
	CDbAcisEntityData&
	);
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
	);
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
	);
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
	);
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
    );
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
    const std::vector<geodata::curve*>& curves,
	xstd<CDbAcisEntityData>::vector& regions,
	int color,
	std::list<geodata::curve*> &qualifiedCurves
    );
    /*-------------------------------------------------------------------------*//**
    Creates a region from a closed planar curve that is not self-intersected.
    @author Stephen W. Hou
    @param
    @return 1 - for success; 0 - for error
    *//*--------------------------------------------------------------------------*/
    virtual int region
    (
    const geodata::curve* pCurve, 
	int color,
	CDbAcisEntityData &region
	);
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
	);
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
	);
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
	);
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
	);
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
	double taper,
	int color
	);
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
	);
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
	);
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
	);

	virtual int reassembleFromMesh
	(
	CDbAcisEntityData& data,
	const std::vector<point>& points,
	const std::vector<int>& faces
	);

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
	);

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
	);

	virtual int chamfer
	(
   	const CDbAcisEntityData&,
	CDbAcisEntityData&,
	const std::vector<HEDGE>&,
	double baseChamferDistance,
	double otherChamferDistance,
	int color
	);
};

#endif BUILD_WITH_LIMITED_ACIS_SUPPORT

extern err_mess_type g_errorCode;

#endif
