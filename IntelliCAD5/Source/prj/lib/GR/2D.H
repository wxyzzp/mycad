#ifndef __2dH__
#define __2dH__

/*------------------------------------------------------------------------*//**
@author Denis Petrov
@return 1 if two segments intersected
*//*-------------------------------------------------------------------------*/
int isIntersected
(
double ax1, 
double ay1, 
double ax2, 
double ay2,
double bx1, 
double by1, 
double bx2, 
double by2
);

/*------------------------------------------------------------------------*//**
@author Denis Petrov
Base class for plane selectors
*//*-------------------------------------------------------------------------*/
class CObject2D
{
public:
	virtual int isIntersected
		(
		const double& x, 
		const double& y
		) const = 0;
	virtual int isIntersected
		(
		const double& x1, 
		const double& y1, 
		const double& x2, 
		const double& y2
		) const = 0;
};

/*------------------------------------------------------------------------*//**
@author Denis Petrov
Rectangle
*//*-------------------------------------------------------------------------*/
class CRect2D: public CObject2D
{
public:
	CRect2D
		(
		const double& left, 
		const double& bottom, 
		const double& right, 
		const double& top
		);
	virtual int isIntersected
		(
		const double& x, 
		const double& y
		) const;
	virtual int isIntersected
		(
		const double& x1, 
		const double& y1, 
		const double& x2, 
		const double& y2
		) const;
protected:
	double m_left;
	double m_bottom;
	double m_right;
	double m_top;
};

/*------------------------------------------------------------------------*//**
@author Denis Petrov
Circle
*//*-------------------------------------------------------------------------*/
class CCircle2D: public CObject2D
{
public:
	CCircle2D
		(
		const double& cx, 
		const double& cy, 
		const double& r
		);
	virtual int isIntersected
		(
		const double& x, 
		const double& y
		) const;
	virtual int isIntersected
		(
		const double& x1, 
		const double& y1, 
		const double& x2, 
		const double& y2
		) const;
protected:
	double m_cx;
	double m_cy;
	double m_r2;
};

/*------------------------------------------------------------------------*//**
@author Denis Petrov
Collection of segments also known as polyline
*//*-------------------------------------------------------------------------*/
class CLinepieces2D: public CObject2D
{
public:
	CLinepieces2D
		(
		int nPoints, 
		double* pPoints
		);
	virtual ~CLinepieces2D();
	virtual int isIntersected
		(
		const double& x, 
		const double& y
		) const;
	virtual int isIntersected
		(
		const double& x1, 
		const double& y1, 
		const double& x2, 
		const double& y2
		) const;
protected:
	double* m_pPoints;
	int m_nPoints;
};

/*------------------------------------------------------------------------*//**
@author Denis Petrov
Closed collection of line segments, interpretating as polygon
*//*-------------------------------------------------------------------------*/
class CPolygon2D: public CLinepieces2D
{
public:
	CPolygon2D
		(
		int nPoints, 
		double* pPoints
		);
	virtual ~CPolygon2D();
	virtual int isIntersected
		(
		const double& x, 
		const double& y
		) const;
	virtual int isIntersected
		(
		const double& x1, 
		const double& y1, 
		const double& x2, 
		const double& y2
		) const	{ return CLinepieces2D::isIntersected(x1, y1, x2, y2); }
};

#endif
