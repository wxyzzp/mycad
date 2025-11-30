#include "gr.h"
#include "2d.h"

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
)
{
	ax2 -= ax1;
	ay2 -= ay1;
	bx2 = bx1 - bx2;
	by2 = by1 - by2;
	bx1 -= ax1;
	by1 -= ay1;
	double d = ax2 * by2 - ay2 * bx2;
	if(isEqualZeroWithin(d))
		return 0;

	ax1 = (bx1 * by2 - by1 * bx2) / d;
	ay1 = (ax2 * by1 - ay2 * bx1) / d;
	if(ax1 >= 0.0 && ax1 <= 1.0 && ay1 >= 0.0 && ay1 <= 1.0)
		return 1;
	return 0;
}

CRect2D::CRect2D
(
const double& left, 
const double& bottom, 
const double& right, 
const double& top
): m_left(left), m_bottom(bottom), m_right(right), m_top(top)
{
}

int CRect2D::isIntersected
(
const double& x, 
const double& y
) const
{
	return m_left <= x && x <= m_right && m_bottom <= y && y <= m_top;
}

int CRect2D::isIntersected
(
const double& x1, 
const double& y1, 
const double& x2, 
const double& y2
) const
{
	int place1 = 0;
	int place2 = 0;
	double in;
	if(x1 < m_left)
		place1 |= 0x01;
	else
		if(x1 > m_right)
			place1 |= 0x04;
	if(y1 < m_bottom)
		place1 |= 0x02;
	else
		if(y1 > m_top)
			place1 |= 0x08;

	if(x2 < m_left)
		place2 |= 0x01;
	else
		if(x2 > m_right)
			place2 |= 0x04;
	if(y2 < m_bottom)
		place2 |= 0x02;
	else
		if(y2 > m_top)
			place2 |= 0x08;

	if((place1 & place2) != 0)
		return 0;

	if(place1 == 0 || place2 == 0)
		return 1;

	if(!isEqualZeroWithin(x2 - x1))
	{
		in = y1 + (m_left - x1) * (y2 - y1) / (x2 - x1);
		if(m_bottom <= in && in <= m_top)
			return 1;
		
		in = y1 + (m_right - x1) * (y2 - y1) / (x2 - x1);
		if(m_bottom <= in && in <= m_top)
			return 1;
	}
	if(!isEqualZeroWithin(y2 - y1))
	{
		in = x1 + (m_bottom - y1) * (x2 - x1) / (y2 - y1);
		if(m_left <= in && in <= m_right)
			return 1;
		
		in = x1 + (m_top - y1) * (x2 - x1) / (y2 - y1);
		if(m_left <= in && in <= m_right)
			return 1;
	}
	return 0;
}

CCircle2D::CCircle2D
(
const double& cx, 
const double& cy, 
const double& r
): m_cx(cx), m_cy(cy), m_r2(r * r)
{
}

int CCircle2D::isIntersected
(
const double& x, 
const double& y
) const
{
	return (x - m_cx) * (x - m_cx) + (y - m_cy) * (y - m_cy) <= m_r2;
}

int CCircle2D::isIntersected
(
const double& x1, 
const double& y1, 
const double& x2, 
const double& y2
) const
{
	double x, y, l, p;
	x = x2 - x1;
	y = y2 - y1;
	l = sqrt(x * x + y * y);
	if(isEqualZeroWithin(l))
		return isIntersected(x1, y1);
	x /= l;
	y /= l;
	p = x * (m_cx - x1) + y * (m_cy - y1);
	if(p < 0.0 || p > l)
		return 0;

	x = x1 + p * x;
	y = y1 + p * y;
	return isIntersected(x, y);
}

CLinepieces2D::CLinepieces2D
(
int nPoints, 
double* pPoints
): m_nPoints(nPoints), m_pPoints(pPoints)
{
}

CLinepieces2D::~CLinepieces2D()
{
	delete[] m_pPoints;
}

int CLinepieces2D::isIntersected
(
const double& x, 
const double& y
) const
{
	for(int i = 2 * (m_nPoints - 2); i >= 0; i -= 2)
		if(isEqualZeroWithin((x - m_pPoints[i]) * (m_pPoints[i + 3] - m_pPoints[i + 1]) 
			- (y - m_pPoints[i + 1]) * (m_pPoints[i + 2] - m_pPoints[i])))
			if(!isEqualZeroWithin(m_pPoints[i + 2] - m_pPoints[i]))
			{
				if(m_pPoints[i + 2] > m_pPoints[i])
				{
					if(m_pPoints[i] <= x && x <= m_pPoints[i + 2])
						return true;
					else
						continue;
				}
				else
				{
					if(m_pPoints[i + 2] <= x && x <= m_pPoints[i])
						return true;
					else
						continue;
				}
			}
			else
			{
				if(m_pPoints[i + 3] > m_pPoints[i + 1])
				{
					if(m_pPoints[i + 1] <= y && y <= m_pPoints[i + 3])
						return true;
					else
						continue;
				}
				else
				{
					if(m_pPoints[i + 3] <= y && y <= m_pPoints[i + 1])
						return true;
					else
						continue;
				}
			}
	return 0;
}

int CLinepieces2D::isIntersected
(
const double& x1, 
const double& y1, 
const double& x2, 
const double& y2
) const
{
	for(int i = 2 * (m_nPoints - 2); i >= 0; i -= 2)
		if(::isIntersected(m_pPoints[i], m_pPoints[i + 1], m_pPoints[i + 2], m_pPoints[i + 3], x1, y1, x2, y2))
			return 1;
	return 0;
}

CPolygon2D::CPolygon2D
(
int nPoints, 
double* pPoints
): CLinepieces2D(nPoints, pPoints)
{
}

CPolygon2D::~CPolygon2D()
{
}

int CPolygon2D::isIntersected
(
const double& x, 
const double& y
) const
{
	int nIntersections = 0;
	for(int i = 2 * (m_nPoints - 2); i >= 0; i -= 2)
	{
		if(isEqualZeroWithin(m_pPoints[i + 1] - m_pPoints[i + 3]))
		{
			if(isEqualZeroWithin(m_pPoints[i + 1] - y))
			{
				if(m_pPoints[i + 2] > m_pPoints[i])
				{
					if(m_pPoints[i] <= x && x <= m_pPoints[i + 2])
						return true;
					else
						continue;
				}
				else
				{
					if(m_pPoints[i + 2] <= x && x <= m_pPoints[i])
						return true;
					else
						continue;
				}
			}
			else
				continue;
		}
		if(m_pPoints[i + 1] > y && m_pPoints[i + 3] > y)
			continue;
		if(m_pPoints[i + 1] < y && m_pPoints[i + 3] < y)
			continue;

		if(!isEqualZeroWithin(m_pPoints[i + 1] - y) && !isEqualZeroWithin(m_pPoints[i + 3] - y))
		{
			if(m_pPoints[i] + (y - m_pPoints[i + 1]) * (m_pPoints[i + 2] - m_pPoints[i]) / (m_pPoints[i + 3] - m_pPoints[i + 1]) >= x)
				++nIntersections;
		}
		else
		{
			if(m_pPoints[i + 1] > m_pPoints[i + 3])
			{
				if(isEqualZeroWithin(m_pPoints[i + 1] - y) && m_pPoints[i] >= x)
					++nIntersections;
			}
			else
			{
				if(isEqualZeroWithin(m_pPoints[i + 3] - y) && m_pPoints[i + 2] >= x)
					++nIntersections;
			}
		}
	}

	if(isEqualZeroWithin(m_pPoints[1] - m_pPoints[2 * m_nPoints - 1]))
	{
		if(isEqualZeroWithin(m_pPoints[1] - y))
		{
			if(m_pPoints[2 * m_nPoints - 2] > m_pPoints[0])
			{
				if(m_pPoints[0] <= x && x <= m_pPoints[2 * m_nPoints - 2])
					return true;
			}
			else
			{
				if(m_pPoints[2 * m_nPoints - 2] <= x && x <= m_pPoints[0])
					return true;
			}
		}
	}
	if(m_pPoints[1] > y && m_pPoints[2 * m_nPoints - 1] > y)
		return nIntersections & 1;
	if(m_pPoints[1] < y && m_pPoints[2 * m_nPoints - 1] < y)
		return nIntersections & 1;
	
	if(!isEqualZeroWithin(m_pPoints[1] - y) && !isEqualZeroWithin(m_pPoints[2 * m_nPoints - 1] - y))
	{
		if(m_pPoints[0] + (y - m_pPoints[1]) * (m_pPoints[2 * m_nPoints - 2] - m_pPoints[0]) / (m_pPoints[2 * m_nPoints - 1] - m_pPoints[1]) >= x)
			++nIntersections;
	}
	else
	{
		if(m_pPoints[1] > m_pPoints[2 * m_nPoints - 1])
		{
			if(isEqualZeroWithin(m_pPoints[1] - y) && m_pPoints[0] >= x)
				++nIntersections;
		}
		else
		{
			if(isEqualZeroWithin(m_pPoints[2 * m_nPoints - 1] - y) && m_pPoints[2 * m_nPoints - 2] >= x)
				++nIntersections;
		}
	}

	return nIntersections & 1;
}




