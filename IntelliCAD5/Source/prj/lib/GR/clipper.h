// \source\prj\lib\gr\clipper.h

#ifndef _CLIPPER_H
#define _CLIPPER_H

#include <list>
#include "db.h"
#include "matrix.h"
#include "line.h"
#include "polyline.h"
#include "extents.h"

const short CLIPPER_FRONT_PLANE	= 1;
const short CLIPPER_BACK_PLANE	= 2;

// clipping routines return codes
const short CLIPPER_RC_ERROR	= 0;	// no memory, etc.
const short CLIPPER_RC_DRAWALL	= 1;	// clipper's bounding volume contains whole the entity 
const short CLIPPER_RC_CLIP		= 2;	// clipper really clips the entity
const short CLIPPER_RC_DONTDRAW	= 3;	// whole the entity lies outside the clipper's bounding volume


class CFilter
{
	friend class CClipper;

	// methods
public:
	CFilter() : m_ClipPlanes(0)
	{
	}

	~CFilter()
	{
	}

private:
	void	Clear();
	bool	FillData(db_handitem* pSourceSpatialFilter, CMatrix4* pInsertTransformation);
	short	ClipLineByPlanes(CLine& LineToClip) const;

	// data
	CPolyLine		m_ClippingPolyline;			// clipping contour (polyline)
	CExtents		m_ContourExtents;			// extents of clipping contour in its UCS xy plane

	short			m_ClipPlanes;				// bit 0 = 1 if front plane exists;
												// bit 1 = 1 if back plane exists
	double			m_FrontPlaneDistance,		// signed distance from clipping contour ucs xy plane to the front clipping plane
					m_BackPlaneDistance;		// signed distance from clipping contour ucs xy plane to the back clipping plane

	CMatrix4		m_ClipperTransformation;	// transformation to clipping contour UCS
	bool			m_bIdentityTransformation;	// 'true' if m_ClipperTransformation is identity
};

typedef CFilter*			PFilter;
typedef std::list<PFilter>	PFiltersList;

typedef std::list<CLine>		LinesList;
typedef std::list<PPolyLine>	PPolylinesList;


class CClipper
{
	// methods
public:
	CClipper() : m_bNotEmpty(false)
	{
	}

	~CClipper()
	{
		Clear();
	}

	void	Clear();

	bool	IsNotEmpty()
	{
		return m_bNotEmpty;
	}

	bool	AddFilter(db_handitem* pSpatialFilterToAdd, CMatrix4* pInsertTransformation);
	void	RemoveLastFilter();

	bool	ClipLines(LinesList& Lines) const;
	bool	ClipPolylines(PPolylinesList& PPolylines) const;
	bool	ClipPolygons(PPolylinesList& PPolygons) const;

	short	CheckBoundingBox(const sds_point MinPoint, const sds_point MaxPoint) const;

private:
	short	ClipLineByFilter(const CLine& LineIn, const PFilter pFilter, LinesList& LinesOut) const;
	short	ClipPolygonByFilter(const CPolyLine& PolygonIn, const PFilter pFilter, PPolylinesList& PPolygonsOut) const;

	// data
private:
	bool			m_bNotEmpty;	// true if m_PFiltersList is not empty (i.e. we should clip) - for speed & encapsulation
	PFiltersList	m_PFiltersList;	// clipper data
};


// the helper
template <class PointerTo> inline void
FreePointersList(std::list<PointerTo>& List);


#endif
