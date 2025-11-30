// File: GeInterval.h
//
// CGeInterval
//

#pragma once


#include "GePch.h"


class GE_API CGeInterval
{
public:
	CGeInterval(double tol = 1.e-12);
	CGeInterval(const CGeInterval& source);
	CGeInterval(double lower, double upper, double tol = 1.e-12);
	CGeInterval(bool bBoundedBelow, double bound, double tol = 1.e-12);
	~CGeInterval();

	double			lowerBound			() const;
	double			upperBound			() const;
	double			element				() const;
	void			getBounds			(double& lower, double& upper) const;
	double			length				() const;
	double			tolerance			() const;
	CGeInterval&	set					(double lower, double upper);
	CGeInterval&	set					(bool bBoundedBelow, double bound);
	CGeInterval&	set					();
	CGeInterval&	setUpper			(double upper);
	CGeInterval&	setLower			(double lower);
	CGeInterval&	setTolerance		(double tol);
	void			getMerge			(const CGeInterval& otherInterval, CGeInterval& result) const;
	int				subtract			(const CGeInterval& otherInterval, CGeInterval& leftInterval, CGeInterval& rightInterval) const;
	bool			intersectWith		(const CGeInterval& otherInterval, CGeInterval& result) const;
	bool			isBounded			() const;
	bool			isBoundedAbove		() const;
	bool			isBoundedBelow		() const;
	bool			isUnBounded			() const;
	bool			isSingleton			() const;
	bool			isDisjoint			(const CGeInterval& otherInterval) const;
	bool			contains			(const CGeInterval& otherInterval) const;
	bool			contains			(double val) const;
	bool			isContinuousAtUpper	(const CGeInterval& otherInterval) const;
	bool			isOverlapAtUpper	(const CGeInterval& otherInterval, CGeInterval& overlap) const;
	bool			operator ==			(const CGeInterval& otherInterval) const;
	bool			operator !=			(const CGeInterval& otherInterval) const;
	bool			isEqualAtUpper		(const CGeInterval& otherInterval) const;
	bool			isEqualAtUpper		(double value) const;
	bool			isEqualAtLower		(const CGeInterval& otherInterval) const;
	bool			isEqualAtLower		(double value) const;
	bool			isPeriodicallyOn	(double period, double& val);
	bool			operator >			(double val) const;
	bool			operator >			(const CGeInterval& otherInterval) const;
	bool			operator >=			(double val) const;
	bool			operator >=			(const CGeInterval& otherInterval) const;
	bool			operator <			(double val) const;
	bool			operator <			(const CGeInterval& otherInterval) const;
	bool			operator <=			(double val) const;
	bool			operator <=			(const CGeInterval& otherInterval) const;
	CGeInterval&	operator =			(const CGeInterval& source);

	friend GE_API bool	operator <		(double val, const CGeInterval& interval);
	friend GE_API bool	operator >=		(double val, const CGeInterval& interval);
	friend GE_API bool	operator >		(double val, const CGeInterval& interval);
	friend GE_API bool	operator <=		(double val, const CGeInterval& interval);

private:
	double	m_min, m_max;
	double	m_tolerance;

/* original code:
protected:
	friend class AcGeImpInterval;

	AcGeImpInterval  *mpImpInt;

	// Construct object from its corresponding implementation object.
	AcGeInterval (AcGeImpInterval&, int);

private:
	int              mDelInt;*/
};
