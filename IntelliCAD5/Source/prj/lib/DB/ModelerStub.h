#pragma once
#ifndef _MODELERSTUB_H_
#define _MODELERSTUB_H_
#include "Modeler.h"

class CModelerStub: protected CModeler
{
public:
    virtual bool isStarted() {return false;}
	virtual bool canView() {return false;}
	virtual bool canModify() {return false;}

    virtual void free(void*)
	{}
	virtual void clear()
	{}
    virtual int copy
    (
    const CDbAcisEntityData&,
    CDbAcisEntityData&
    )
	{return 0;}
    virtual int transform
    (
    const CDbAcisEntityData&,
    CDbAcisEntityData&,
    const icl::transf&
    )
	{return 0;}
    virtual int loadSatFile
    (
    const char* sFilename,
    int currColor,
    const RGBQUAD* pCurrPaletteColors,
	xstd<CDbAcisEntityData>::vector&
    )
	{return 0;}
    virtual int saveSatFile
    (
    const char* sFilename,
    const std::vector<CDbAcisEntityData>& objs,
    const RGBQUAD* pCurrPaletteColors,
	int version = 0
    )
	{return 0;}
    virtual int display
    (
    const CDbAcisEntityData&,
    int isolines,
    double tolerance,
    sds_dobjll** ppSdsDispObjs
    )
	{return 0;}
    virtual int getBasePoint
    (
    const CDbAcisEntityData&,
    icl::point&
    )
	{return 0;}
    virtual int getMeshRepresentation
    (
    const CDbAcisEntityData&,
    xstd<icl::point>::vector&,
    xstd<int>::vector&
    )
	{return 0;}
    virtual int createBlock
    (
    const icl::point&,
    const icl::point&,
    const icl::transf&,
	int color,
	CDbAcisEntityData&
    )
	{return 0;}
    virtual int createSphere
    (
    const icl::point&,
    double,
    const icl::transf&,
	int color,
	CDbAcisEntityData&
    )
	{return 0;}
    virtual int createTorus
    (
    const icl::point&,
    double,
    double,
    const icl::transf&,
	int color,
	CDbAcisEntityData&
    )
	{return 0;}
    virtual int createWedge
    (
    const icl::point&,
    const icl::point&,
    double,
    const icl::transf&,
	int color,
	CDbAcisEntityData&
    )
	{return 0;}
    virtual int createCone
    (
    const icl::point&,
    double,
    const icl::point&,
    double,
    const icl::transf&,
	int color,
	CDbAcisEntityData&
    )
	{return 0;}
    virtual int createCone
    (
    const icl::point&,
    const icl::gvector&,
    double,
    const icl::point&,
    const icl::transf&,
	int color,
	CDbAcisEntityData&
    )
	{return 0;}
    virtual int createCylinder
    (
    const icl::point&,
    double,
    const icl::point& pthigh,
    const icl::transf&,
	int color,
	CDbAcisEntityData&
    )
	{return 0;}
    virtual int createCylinder
    (
    const icl::point& bottom,
    const icl::gvector&,
    double ratio,
    const icl::point& top,
    const icl::transf&,
	int color,
	CDbAcisEntityData&
    )
	{return 0;}
    virtual int createDish
    (
    const icl::point&,
    double,
    const icl::transf&,
	int color,
	CDbAcisEntityData&
    )
	{return 0;}
    virtual int createDome
    (
    const icl::point&,
    double,
    const icl::transf&,
	int color,
	CDbAcisEntityData&
    )
	{return 0;}
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
    )
	{return 0;}
    virtual int createSurfaceRuled
    (
    const geodata::curve&,
    const geodata::curve&,
	int color,
	CDbAcisEntityData&
    )
	{return 0;}
    virtual int createSurfaceCurves
    (
    const std::vector<geodata::curve*>&,
	int color,
	CDbAcisEntityData&
    )
	{return 0;}
    virtual int createSurfaceRevolved
    (
    const geodata::curve&,
    const geodata::curve&,
    double,
    double,
	int color,
	CDbAcisEntityData&
    )
	{return 0;}
    virtual int createSurfaceExtruded
    (
    const geodata::curve&,
    const geodata::curve&,
	int color,
	CDbAcisEntityData&
    )
	{return 0;}
    virtual int explodeIntoBodies
    (
    const CDbAcisEntityData&,
    xstd<CDbAcisEntityData>::vector&
    )
	{return 0;}
    virtual int explodeIntoCurves
    (
    const CDbAcisEntityData&,
    xstd<geodata::curve*>::vector&
    )
	{return 0;}
    virtual int boolean
    (
    const std::vector<CDbAcisEntityData>&,
    CDbAcisEntityData&,
    eBoolType
    )
	{return 0;}
    virtual int unite
    (
    const std::vector<CDbAcisEntityData>&,
    CDbAcisEntityData&
    )
	{return 0;}
    virtual int intersect
    (
    const std::vector<CDbAcisEntityData>&,
    CDbAcisEntityData&
    )
	{return 0;}
    virtual int subtract
    (
    const std::vector<CDbAcisEntityData>& blanks,
    const std::vector<CDbAcisEntityData>& tools,
    xstd<CDbAcisEntityData>::vector& result
    )
	{return 0;}
    virtual int isCurvesConnected
    (
    bool&,
    const geodata::curve&,
    const geodata::curve&
    )
	{return 0;}
    virtual int interfere
    (
    const std::vector<CDbAcisEntityData>&,
    const std::vector<CDbAcisEntityData>&,
    xstd<std::pair<long, long> >::vector&,
    xstd<CDbAcisEntityData>::vector&
    )
	{return 0;}
    virtual int lockDatabase()
	{return 0;}
    virtual int unlockDatabase()
	{return 0;}
    virtual int pickEdge
    (
    const CDbAcisEntityData&,
    const icl::point&,
    const icl::gvector&,
    double,
	int,
    xstd<HEDGE>::vector&
    )
	{return 0;}
	virtual int pickLoop
	(
	const CDbAcisEntityData&,
	const icl::point&,
	const icl::gvector&,
	double,
	HFACE,
	xstd<HEDGE>::vector&
	){return 0;}
    virtual int pickFace
    (
    const CDbAcisEntityData&,
    const icl::point&,
    const icl::gvector&,
    double,
	int,
    xstd<HFACE>::vector&
    )
	{return 0;}
    virtual int unhighlight
    (
    const CDbAcisEntityData&,
    int subentity
    )
	{return 0;}
    virtual int highlight
    (
    const CDbAcisEntityData&,
    int subentity
    )
	{return 0;}
    virtual int getFaces
    (
    const CDbAcisEntityData&,
    xstd<HFACE>::vector&
    )
	{return 0;}
    virtual int getEdges
    (
    const CDbAcisEntityData&,
    xstd<HEDGE>::vector&
    )
	{return 0;}
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
    )
	{return 0;}
    virtual int taperFaces
    (
    const CDbAcisEntityData& src,
    CDbAcisEntityData& dest,
    const std::vector<HFACE>& faces,
    const icl::point& _root,
    const icl::gvector& _normal,
    double angle
    )
	{return 0;}
    virtual int offsetFaces
    (
    const CDbAcisEntityData& src,
    CDbAcisEntityData& dest,
    const std::vector<HFACE>& faces,
    double offset
    )
	{return 0;}
    virtual int rotateFaces
    (
    const CDbAcisEntityData& src,
    CDbAcisEntityData& dest,
    const std::vector<HFACE>& faces,
    const icl::point& root,
    const icl::gvector& dir,
    double angle
    )
	{return 0;}
    virtual int deleteFaces
    (
    const CDbAcisEntityData& src,
    CDbAcisEntityData& dest,
    const std::vector<HFACE>& faces
    )
	{return 0;}
    virtual int copyFaces
    (
    const CDbAcisEntityData&,
    const std::vector<HFACE>&,
    xstd<CDbAcisEntityData>::vector&,
    const icl::transf&
    )
	{return 0;}
    virtual int copyEdges
    (
    const CDbAcisEntityData& obj,
    const std::vector<HEDGE>& edges,
    xstd<geodata::curve*>::vector& curves,
    const icl::transf& mtx
    )
	{return 0;}
    virtual int colorModelEntity
    (
    const CDbAcisEntityData& oldModelData,
    int color,
    CDbAcisEntityData& newModelData
    ) { return 0; }
    virtual int colorFaces
    (
    const CDbAcisEntityData&,
    CDbAcisEntityData&,
    const std::vector<HFACE>&,
    int
    )
	{return 0;}
    virtual int colorEdges
    (
    const CDbAcisEntityData&,
    CDbAcisEntityData&,
    const std::vector<HEDGE>&,
    int
    )
	{return 0;}
    virtual int getSnapingPoints
    (
    const CDbAcisEntityData& sat,
    int osmode,
    const icl::point& pick,
	const icl::point& last,
    const icl::gvector& dir,
	xstd<snaping>::vector& pnts
    )
	{return 0;}
    virtual int getNumberOfFaces
    (
    const CDbAcisEntityData&,
    int& nfaces
    )
	{return 0;}
    virtual int extrudeFacesAlongPath
    (
    const CDbAcisEntityData&,
    CDbAcisEntityData&,
    const std::vector<HFACE>&,
    const geodata::curve&
    )
	{return 0;}
    virtual int extrudeFaces
    (
    const CDbAcisEntityData&,
    CDbAcisEntityData&,
    const std::vector<HFACE>&,
    double,
    double
    )
	{return 0;}
	virtual int imprint
	(
	const CDbAcisEntityData&,
	const std::vector<geodata::curve*>&,
	CDbAcisEntityData&
	)
	{return 0;}
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
	)
	{return 0;}
	virtual int clean
	(
	const CDbAcisEntityData&,
	CDbAcisEntityData&
	)
	{return 0;}
	virtual int check
	(
	const CDbAcisEntityData&,
    xstd<icl::string>::vector&
	)
	{return 0;}
    virtual int shell
    (
    const CDbAcisEntityData&,
    CDbAcisEntityData&,
    const std::vector<HFACE>&,
    double
    )
	{return 0;}
    virtual int region
    (
    const std::vector<geodata::curve*>&,
	xstd<CDbAcisEntityData>::vector&,
	int color,
	std::list<geodata::curve*> &qualifiedCurves
    )
	{return 0;}
	virtual int region
	(
	const geodata::curve* pCurve,
	int color,
	CDbAcisEntityData &region
    ) {return 0; }
	virtual int section
	(
	const CDbAcisEntityData&,
	CDbAcisEntityData&,
	const icl::point&,
	const icl::gvector&
	)
	{return 0;}
	virtual int slice
	(
	const CDbAcisEntityData&,
	CDbAcisEntityData[2],
	const icl::point&,
	const icl::gvector&,
	const icl::point*
	)
	{return 0;}
	virtual int extrude
	(
	const CDbAcisEntityData&,
	CDbAcisEntityData&,
	double dist,
	double taper,
	int color
	)
	{return 0;}
	virtual int extrude
	(
	const CDbAcisEntityData&,
	CDbAcisEntityData&,
	const geodata::curve& _path,
	int color
	)
	{return 0;}
	virtual int revolve
	(
	const CDbAcisEntityData&,
	CDbAcisEntityData&,
	const icl::point&,
	const icl::gvector&,
	double angle,
	int color
	)
	{return 0;}
	virtual int getGrips
	(
	const CDbAcisEntityData&,
	xstd<icl::point>::vector&
	)
	{return 0;}
	virtual int getBoundingBox
	(
	const CDbAcisEntityData&,
	icl::point& low,
	icl::point& high,
	const icl::transf& w2u
	)
	{return 0;}
	virtual int getPerimeter
	(
	const CDbAcisEntityData&,
	double&
	)
	{return 0;}
	virtual int getArea
	(
	const CDbAcisEntityData&,
	double&
	)
	{return 0;}
	virtual int getSatInVersion
	(
	const CDbAcisEntityData&,
	icl::string&,
	int
	)
	{return 0;}
	virtual int getLastErrorCode()
	{return 0;}
	virtual int getErrorMessage
	(
	int,
	icl::string&
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
	const CDbAcisEntityData&,
	CDbAcisEntityData&,
	int color,
	const std::vector<std::pair<HEDGE, double> >&
	){return 0;}
	virtual int isEdgeAppropriateForBlending
	(
	const CDbAcisEntityData&,
	HEDGE,
	bool&
	){return 0;}
	int chamfer
	(
   	const CDbAcisEntityData&,
	CDbAcisEntityData&,
	const std::vector<HEDGE>&,
	double baseChamferDistance,
	double otherChamferDistance,
	int color
	){return 0;}
};

#endif
