// File: GeKnotVector.h
//
// CGeKnotVector
//

#pragma once


#include "GePch.h"
#include "GeArrays.h"


class GE_API CGeInterval;


class GE_API CGeKnotVector
{
public:
	CGeKnotVector(double eps = 1.e-9);
	CGeKnotVector(int size, int growSize, double eps = 1.e-9);
	CGeKnotVector(int size, const double pKnots [], double eps = 1.e-9);
	CGeKnotVector(int plusMult, const CGeKnotVector& source);
	CGeKnotVector(const CGeKnotVector& source);
	CGeKnotVector(const CGeDoubleArray& source, double eps = 1.e-9);
	~CGeKnotVector();

	CGeKnotVector&	operator =			(const CGeKnotVector& source);
	CGeKnotVector&	operator =			(const CGeDoubleArray& source);
	double&			operator []			(int i);
	const double	operator []			(int i) const;
	bool			isEqualTo			(const CGeKnotVector& other) const;
	double			startParam			() const;
	double			endParam			() const;
	int				multiplicityAt		(int i) const;
	int				numIntervals		() const;
	int				getInterval			(int ord, double par, CGeInterval& interval) const;
	void			getDistinctKnots	(CGeDoubleArray& knots) const;
	bool			contains			(double param) const;
	bool			isOn				(double knot) const;
	CGeKnotVector&	reverse				();
	CGeKnotVector&	removeAt			(int i);
	CGeKnotVector&	removeSubVector		(int startIndex, int endIndex);
	CGeKnotVector&	insertAt			(int i, double u, int multiplicity = 1);
	CGeKnotVector&	insert				(double u);
	int				append				(double val);
	CGeKnotVector&	append				(CGeKnotVector& tail, double knotRatio = 0.0);
	int				split				(double par, CGeKnotVector* pKnotVector1, int multLast, CGeKnotVector* pKnotVector2, int multFirst) const;
	CGeKnotVector&	setRange			(double lower, double upper);
	double			tolerance			() const;
	CGeKnotVector&	setTolerance		(double tol);
	int				length				() const;
	bool			isEmpty				() const;
	int				logicalLength		() const;
	CGeKnotVector&	setLogicalLength	(int length);
	int				physicalLength		() const;
	CGeKnotVector&	setPhysicalLength	(int length);
	int				growLength			() const;
	CGeKnotVector&	setGrowLength		(int length);
	const double*	asArrayPtr			() const;
	double*			asArrayPtr			();
	CGeKnotVector&	set					(int size, const double pKnots [], double eps = 1.e-9);

private:
	double			m_tolerance;
	CGeDoubleArray	m_knots;

/* original code:
protected:
    AcGeDoubleArray    mData;
    double             mTolerance;

    Adesk::Boolean     isValid (int) const;*/
};
