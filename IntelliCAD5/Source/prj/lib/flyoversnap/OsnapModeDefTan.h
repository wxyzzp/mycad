#pragma once
#include "OsnapObjects.h"
#include "gvector.h"

/*-----------------------------------------------------------------------------
OsnapMarker class for deferred tangent mode
@author Alexey Malov
-------------------------------------------------------------------------------*/
class COsnapMarkerDefTan : public COsnapMarker
{
protected:
	friend class COsnapModeDefTan;
	COsnapMarkerDefTan()
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

class COsnapModeDefTan: public COsnapMode
{
public:
	COsnapModeDefTan();

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

	bool customCondition() const;

protected:
	virtual ~COsnapModeDefTan();

private:

	CString m_localModeString;
	CString m_globalModeString;
	CString m_tooltipString;
	COsnapMarkerDefTan* m_pMarker;

	long m_lRefCount;
};

//</alm>
