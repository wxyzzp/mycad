// PRJ\ICAD\OsnapMarkerStd.h
// COsnapMarkerStd classes interface

#pragma once
#include "OsnapObjects.h"

/*-----------------------------------------------------------------------------
This is is a base class for Osnap marker objects
@author Andrey Grafov
-------------------------------------------------------------------------------*/

class COsnapMarkerStd : public COsnapMarker
{
public:
	COsnapMarkerStd() : m_lRefCount(1) {};

	/*----------------------------------------------------------------------------*//**
	Sets a point (in pixels) for futher use
	@author Andrey Grafov
	@param  pt => point to be set
	*//*----------------------------------------------------------------------------*/
	virtual void setLocation( CPoint &pt ){ m_pixPt = pt; }
	
	/*----------------------------------------------------------------------------*//**
	Sets size of marker (in pixels) for futher use
	@author Andrey Grafov
	@param  iSize => size to be set
	@param  iThickness => thickness to be set
	*//*----------------------------------------------------------------------------*/
	virtual void setSize( int iSize, int iThickness ){ m_iSize = iSize; m_iThickness = iThickness; }

	/*----------------------------------------------------------------------------*//**
	Draws a marker to the given DC,should be overriden in descendants for actual drawing
	@author Andrey Grafov
	@param  pDC => DC object to be drawn to
	*//*----------------------------------------------------------------------------*/
	virtual int Draw( CDC *pDC) const;

	virtual long AddRef();
	virtual void Release();

protected:
	CPoint m_pixPt;
	int m_iSize;
	int m_iThickness;

private:
	long m_lRefCount;
};

///////////////////////////////////////////////////////////////////////////////////////////////
/*-----------------------------------------------------------------------------
OsnapMarker class for "END" mode (end)
@author Andrey Grafov
-------------------------------------------------------------------------------*/
class COsnapMarkerStdEnd : public COsnapMarkerStd
{
protected:
	virtual int Draw( CDC *pDC) const;
};

/*-----------------------------------------------------------------------------
OsnapMarker class for "MID" mode (middle)
@author Andrey Grafov
-------------------------------------------------------------------------------*/
class COsnapMarkerStdMid : public COsnapMarkerStd
{
protected:
	virtual int Draw( CDC *pDC) const;
};

/*-----------------------------------------------------------------------------
OsnapMarker class for "CEN" mode (center)
@author Andrey Grafov
-------------------------------------------------------------------------------*/
class COsnapMarkerStdCen : public COsnapMarkerStd
{
protected:
	virtual int Draw( CDC *pDC) const;
};

/*-----------------------------------------------------------------------------
OsnapMarker class for "NOD" mode (node)
@author Andrey Grafov
-------------------------------------------------------------------------------*/
class COsnapMarkerStdNod : public COsnapMarkerStd
{
protected:
	virtual int Draw( CDC *pDC) const;
};

/*-----------------------------------------------------------------------------
OsnapMarker class for "QUA" mode (quadrant)
@author Andrey Grafov
-------------------------------------------------------------------------------*/
class COsnapMarkerStdQua : public COsnapMarkerStd
{
protected:
	virtual int Draw( CDC *pDC) const;
};

/*-----------------------------------------------------------------------------
OsnapMarker class for "INT" mode (intersection)
@author Andrey Grafov
-------------------------------------------------------------------------------*/
class COsnapMarkerStdInt : public COsnapMarkerStd
{
protected:
	virtual int Draw( CDC *pDC) const;
};

/*-----------------------------------------------------------------------------
OsnapMarker class for "INS" mode (insertion)
@author Andrey Grafov
-------------------------------------------------------------------------------*/
class COsnapMarkerStdIns : public COsnapMarkerStd
{
protected:
	virtual int Draw( CDC *pDC) const;
};

/*-----------------------------------------------------------------------------
OsnapMarker class for "PER" mode (perpendicular)
@author Andrey Grafov
-------------------------------------------------------------------------------*/
class COsnapMarkerStdPer : public COsnapMarkerStd
{
protected:
	virtual int Draw( CDC *pDC) const;
};

/*-----------------------------------------------------------------------------
OsnapMarker class for "TAN" mode (tangent)
@author Andrey Grafov
-------------------------------------------------------------------------------*/
class COsnapMarkerStdTan : public COsnapMarkerStd
{
protected:
	virtual int Draw( CDC *pDC) const;
};

/*-----------------------------------------------------------------------------
OsnapMarker class for "NEA" mode (nearest)
@author Andrey Grafov
-------------------------------------------------------------------------------*/
class COsnapMarkerStdNea : public COsnapMarkerStd
{
protected:
	virtual int Draw( CDC *pDC) const;
};

/*-----------------------------------------------------------------------------
OsnapMarker class for "QUI" mode (quick - AG: what should it be?????????)
@author Andrey Grafov
-------------------------------------------------------------------------------*/
class COsnapMarkerStdQui : public COsnapMarkerStd
{
/*protected:
	virtual int Draw( CDC *pDC) const;*/
};

/*-----------------------------------------------------------------------------
OsnapMarker class for "APP" mode (apparent intersection - AG: what should it be?????????)
@author Andrey Grafov
-------------------------------------------------------------------------------*/
class COsnapMarkerStdApp : public COsnapMarkerStd
{
protected:
	virtual int Draw( CDC *pDC) const;
};

/*-----------------------------------------------------------------------------
OsnapMarker class for "PLA" mode (plan intersection - differs from ACAD
EXTENSION mode - 4096)
@author Andrey Grafov
-------------------------------------------------------------------------------*/
class COsnapMarkerStdPla : public COsnapMarkerStd
{
protected:
	virtual int Draw( CDC *pDC) const;
};

/*-----------------------------------------------------------------------------
OsnapMarker class for "PAR" mode (parallel - not implemented in Icad yet)
@author Andrey Grafov
-------------------------------------------------------------------------------*/
class COsnapMarkerStdPar : public COsnapMarkerStd
{
protected:
	virtual int Draw( CDC *pDC) const;
};
