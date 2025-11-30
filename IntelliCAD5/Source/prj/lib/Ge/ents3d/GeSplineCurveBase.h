// File: GeSplineCurveBase.h
//
// CGeSplineCurveBase
//

#pragma once


#include "GeContext.h"
#include "GeKnotVector.h"


class GE_API CGeSplineCurveBase
{
public:
	virtual ~CGeSplineCurveBase();

    bool	isRational		() const;
    int		degree			() const;
    int		order			() const;
    int		numKnots		() const;
    int		numControlPoints() const;
    int		continuityAtKnot(int i, const CGeTolerance& tol = CGeContext::s_defTol) const;
    double	startParam		() const;
    double	endParam		() const;
    bool	hasFitData		() const;
    double	knotAt			(int i) const;
    const CGeKnotVector& knots() const;

private:
	bool			m_bRational;
	int				m_degree;
	int				m_order;
	bool			m_bHasFitData;
	CGeKnotVector	m_knots;
};
