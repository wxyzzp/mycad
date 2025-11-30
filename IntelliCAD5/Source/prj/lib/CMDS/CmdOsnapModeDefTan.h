#pragma once
#include "OsnapObjects.h"
#include "gvector.h"

/*-----------------------------------------------------------------------------
OsnapMarker class for deferred tangent mode
@author Alexey Malov
-------------------------------------------------------------------------------*/
class CCmdOsnapMarkerDefTan : public COsnapMarker
{
protected:
	friend class CCmdOsnapModeDefTan;
	CCmdOsnapMarkerDefTan()
		:m_lRefCount(1)
	{}
	/*----------------------------------------------------------------------------*//**
	Sets a point (in pixels) for futher use
	@param  pt => point to be set
	*//*----------------------------------------------------------------------------*/
	virtual void setLocation( CPoint &pt )
	{ m_pixPt = pt; }
	
	/*----------------------------------------------------------------------------*//**
	Sets size of marker (in pixels) for futher use
	@param  iSize => size to be set
	@param  iThickness => thickness to be set
	*//*----------------------------------------------------------------------------*/
	virtual void setSize( int iSize, int iThickness )
	{ m_iSize = iSize; m_iThickness = iThickness; }

	/*----------------------------------------------------------------------------*//**
	Draws a marker to the given DC,should be overriden in descendants for actual drawing
	@param  pDC => DC object to be drawn to
	*//*----------------------------------------------------------------------------*/
	virtual int Draw( CDC *pDC) const;

	virtual long AddRef()
	{return InterlockedIncrement( &m_lRefCount );}

	virtual void Release()
	{	
		if( !InterlockedDecrement( &m_lRefCount ) )
			delete this;
	}

private:
	CPoint m_pixPt;
	int m_iSize;
	int m_iThickness;

	long m_lRefCount;
};

class CCmdOsnapModeDefTan: public COsnapMode
{
public:
	struct SOsnapArc
	{
		icl::point m_center;
		icl::gvector m_normal;
		icl::gvector m_axis;
		double m_start;
		double m_end;

		void init
		(
		const icl::point& center,
		const icl::gvector& normal,
		const icl::gvector& axis,
		double start,
		double end
		)
		{m_center = center; m_normal = normal; m_axis = axis;
		m_start = start; m_end = end;}

		bool operator == (const SOsnapArc& a) const
		{return (m_center == a.m_center) && (m_axis == a.m_axis) &&
		(m_normal == a.m_normal) && 
		(fabs(m_start - a.m_start) < icl::resabs()) && 
		(fabs(m_end - a.m_end) < icl::resabs());}
				
	};

public:
	CCmdOsnapModeDefTan();

	/*----------------------------------------------------------------------------*//**
	Returns localized description of this mode
	@author Andrey Grafov
	*//*----------------------------------------------------------------------------*/
	virtual const char* localModeString() const;

	/*----------------------------------------------------------------------------*//**
	Returns global description of this mode
	@author Andrey Grafov
	*//*----------------------------------------------------------------------------*/
	virtual const char* globalModeString() const;

	/*----------------------------------------------------------------------------*//**
	Returns a string which will be used as tooltip string while getting input
	@author Andrey Grafov
	*//*----------------------------------------------------------------------------*/
	virtual const char* tooltipString() const;

	/*----------------------------------------------------------------------------*//**
	Returns pointer of marker object which is used for this mode
	@author Andrey Grafov
	*//*----------------------------------------------------------------------------*/
	virtual COsnapMarker *marker() const;


	virtual long AddRef();
	virtual void Release();

	/*----------------------------------------------------------------------------*//**
	Collects snapping point for this mode with given parameters
	@author Andrey Grafov
	@param inPtucs => cursor position point (in UCS)
	@param ptAp1ucs => first corner of apperture box (in UCS)
	@param ptAp2ucs => second corner of apperture box (in UCS)
	@param ppPointList <= resbuf containing list of all points found (in UCS)
	@return TRUE if success
	*//*----------------------------------------------------------------------------*/
	virtual int getOsnapPoints
	(
	resbuf *elist, 
	sds_point inPtucs, 
	sds_point ptAp1ucs, 
	sds_point ptAp2ucs, 
	sds_resbuf **ppPointList 
	);

	int getSnapedArc
	(
	const icl::point& pnt,
	icl::point& center, 
	icl::gvector& normal,
	icl::gvector& axis, 
	double& start, 
	double& end
	) const;

	int getSnapedArc
	(
	const icl::point& pnt,
	SOsnapArc& arc
	) const;

	void addSnapedArc
	(
	const icl::point& center, 
	const icl::gvector& normal,
	const icl::gvector& axis, 
	const double& start, 
	const double& end
	);

	void reset()
	{m_arcs.clear();}

	bool customCondition() const;

	void enable();
	void disable();

protected:
	virtual ~CCmdOsnapModeDefTan();

private:
	std::vector<SOsnapArc> m_arcs;

	CString m_localModeString;
	CString m_globalModeString;
	CString m_tooltipString;
	CCmdOsnapMarkerDefTan* m_pMarker;

	bool m_bEnabled;

	long m_lRefCount;
};

//</alm>
