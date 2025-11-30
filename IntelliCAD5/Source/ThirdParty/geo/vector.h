#ifndef _VECTOR_H
#define _VECTOR_H 


#ifndef THREE_D
#define THREE_D
#endif // THREE_D
#include "macros.h"
#include "point.h"
#include "array.h"


//  The class CVector defines a general n-dimension double vector.
//
//  Author: Stephen Hou
//  Date:	3/28/2003
//

class GEO_CLASS CVector : public CRealArray
{
public:

	CVector() {}
	CVector(int dim, double  initValue);
	CVector(int dim) { SetSize(dim); }
	CVector(const CRealArray &array);
	CVector(const CVector &vector);
	virtual ~CVector() {}

	// Returns magnitude of vector
	//
	double			Magnitude() const;

	// Returns magnitude & normalize
	//
	double			MagnitudeNormalize(); 

	// Find the subset of this vector which is a linear interpolation of the 
	// members of this vector
	//
    void			Subset(int size, CVector &subset);

	// Refine a subset of this set. We will try to double the points of the 
	// input subset. The newly inserted points will be those from this vector, 
	// which are: 
    //            
	//     1. inside the right interval
    //     2. nearest to the middle of the interval
	//
    void			RefineSubset(CVector &subset);

	CVector&		Normalized(); 
	CVector			Normalize() const;

	// Build a linear uniform vector of dimention = num
	//
	CVector&		LinearUniform(int num);

	// Return a vector that is projection of this vector to another vector
	//
	CVector			ProjectTo(const CVector &vector);

	// Inserts the values of a vector into this vector if the value doesn't 
	// exist.
	//
	CVector&		Insert(const CVector &vector);

	// Combine given vector into this vector with given tolerance. It is 
	// similar to insert, only won't insert points which are closer than tol.
	//
	CVector&		Combine(const CVector &vector, double  tol = 0.05);

	// Remove a number of numbers starting (including) the given index
	//
	CVector&		ShiftLeftFrom(int index, int how_much=1);

	// Find the index of the vector component which is of mininum value
	//
    int				FindSmallestComponent(int startIndex = 0);

	bool			IsCompatibleTo(const CVector &vector) const { return (Size() == vector.Size()); }

	// Assign operators
	//
	CVector&		operator=(const CVector &vector);

	// Changes the values of all components to given value
	//
	CVector&		operator=(double value);

	// vector addition/subtraction. The function throws a CIncompatibleException
	// exception if vector are not comparible.
	//
	CVector 		operator+(const CVector &vector) const;
	CVector&		operator+=(const CVector &vector);
	CVector 		operator-(const CVector &vector) const;
	CVector&		operator-=(const CVector &vector);

	// Vector dot product. The function throws a CIncompatibleException
	// exception if vector are not comparible.
	//
	double			operator*(const CVector &vector) const;

	// Multiplied all component by the given value
	//
	CVector&		    operator*=(double value);
	CVector 		    operator*(double value) const;
	GEO_FRIEND CVector operator*(double value, const CVector &vector);

	// Divided by d and replaced the given value
	//
	CVector&		operator/=(double  value);
	CVector 		operator/(double  value) const;

	// Compare two vectors for equality
	//
	bool			operator==(const CVector &vector) const;
    bool			operator!=(const CVector &vector) const { return !(*this == vector); }

	double&			operator[](int index);
	double			operator[](int index) const;

protected:

	// Swap input vector with this vector
	//
	void			Swap(CVector &vertor);
};


typedef CVector*	PVector;


//  The class C3Vector defines a 3-dimension double vector.
//
//  Author: Stephen Hou
//  Date:	4/4/2003
//

class GEO_CLASS C3Vector : public CVector
{
public:

	// Default constructor
	//
	C3Vector() : CVector(3) {}

	// Construct constructor.  
	//
	C3Vector(const CVector &vector) : CVector(vector) { ASSERT(Size() == 3); }

	// Construct a 3D vector from a 3D point
	//
	C3Vector(const C3Point &pt);

	C3Vector(double x, double y, double z);

	void			Set(double x, double y, double z);
	double			X() const { return m_rEntries[0]; }
	double			Y() const { return m_rEntries[1]; }
	double			Z() const { return m_rEntries[2]; }

	double			GetLength() const { return Magnitude(); }

	C3Vector&		RotatedBy(double angle, const C3Vector &axis);

	double			GetAngleTo(const C3Vector &vector) const;
	double			GetAngleTo(const C3Vector &vector, const C3Vector &refVector) const;

	double			GetDistanceTo(const C3Vector &vector) const;

    BOOL            IsPerpendicularTo(const C3Vector &vector) const;
    BOOL            IsParallelTo(const C3Vector &vector) const;

	// Move a point along a vector
	//
	C3Point			    operator+(const C3Point &pt) const;	
	GEO_FRIEND C3Point	operator+(const C3Point &pt, const C3Vector &vector);	

	GEO_FRIEND C3Point	operator-(const C3Point &pt, const C3Vector &vector);	
	C3Point			    operator-(const C3Point &pt) const;

	// Negates all component
	//
	C3Vector&		operator-();
	C3Vector		Negate() const;

	// Assign a 3D vector to this vector. If the dimension of input vector
	// is greater than 3, only the first three components are taken acount.
	//
	C3Vector&		operator=(const C3Vector &vector);
	C3Vector&		operator=(const C3Point &pt);

	// Vector cross product. The function throws a CIncompatibleException
	// exception if vector are not comparible.
	//
	C3Vector		operator^(const C3Vector &vector) const;
	C3Point			AsPoint() const { return C3Point(X(), Y(), Z()); }
};


typedef C3Vector*	P3Vector;


inline CVector CVector::operator+(const CVector &vector) const
{ CVector thisVector(*this); return thisVector += vector; }

inline CVector CVector::operator-(const CVector &vector) const
{ CVector thisVector(*this); return thisVector -= vector; }

inline CVector CVector::operator*(double value) const
{ CVector thisVector(*this); return thisVector *= value;}

inline CVector operator*(double value, const CVector &vector) { return vector * value; }

inline CVector CVector::operator/(double  value) const
{ CVector thisVector(*this); return thisVector /= value; }

inline double& CVector::operator[](int index)
{ ASSERT(index >= 0 && index < Size()); return m_rEntries[index]; }

inline double CVector::operator[](int index) const
{ ASSERT(index >= 0 && index < Size()); return m_rEntries[index]; }

inline CVector& CVector::operator=(const CVector &vector)
{ CRealArray::operator=(vector); return *this; }

inline void	CVector::Swap(CVector &v) 
{ SWAP(m_rEntries, v.m_rEntries, double*); SWAP(m_nAllocated, v.m_nAllocated, int);
  SWAP(m_nSize, v.m_nSize, int); }


// C3Vector inline functions
//
inline C3Vector::C3Vector(double x, double y, double z) : CVector(3) { Set(x, y, z); }

inline void C3Vector::Set(double x, double y, double z)
{ ASSERT(m_rEntries != NULL); m_rEntries[0] = x; m_rEntries[1] = y; m_rEntries[2] = z;}

inline C3Vector C3Vector::Negate() const 
{ ASSERT(m_rEntries != NULL); return C3Vector(-m_rEntries[0], -m_rEntries[1], -m_rEntries[2]); }

inline C3Vector& C3Vector::operator=(const C3Vector &vector)
{ CVector::operator=(vector); return *this; }

inline C3Vector& C3Vector::operator=(const C3Point &pt)
{ CVector::operator=(C3Vector(pt.X(), pt.Y(), pt.Z())); return *this; }

inline BOOL C3Vector::IsPerpendicularTo(const C3Vector &vector) const
{ return (FZERO(operator*(vector), FUZZ_GEN)); }

inline BOOL C3Vector::IsParallelTo(const C3Vector &vector) const
{ return (FZERO(fabs(operator*(vector) - 1.0), FUZZ_GEN)); }


#endif // _VECTOR_H
