///////////////////////////////////////////////////////////////////////////////
// Copyright © 2002, Open Design Alliance Inc. ("Open Design") 
// 
// This software is owned by Open Design, and may only be incorporated into 
// application programs owned by members of Open Design subject to a signed 
// Membership Agreement and Supplemental Software License Agreement with 
// Open Design. The structure and organization of this Software are the valuable 
// trade secrets of Open Design and its suppliers. The Software is also protected 
// by copyright law and international treaty provisions. You agree not to 
// modify, adapt, translate, reverse engineer, decompile, disassemble or 
// otherwise attempt to discover the source code of the Software. Application 
// programs incorporating this software must include the following statment 
// with their copyright notices:
//
//      DWGdirect © 2002 by Open Design Alliance Inc. All rights reserved. 
//
// By use of this software, you acknowledge and accept the terms of this 
// agreement.
///////////////////////////////////////////////////////////////////////////////



#ifndef OD_GEVEC2D_H
#define OD_GEVEC2D_H

#include "GeGbl.h"
#include "GeTol.h"

#include "DD_PackPush.h"

class OdGeMatrix2d;

/** Description:
    
    {group:OdGe_Classes} 
*/
class GE_TOOLKIT_EXPORT OdGeVector2d
{
public:
    OdGeVector2d();
    OdGeVector2d(const OdGeVector2d& vec);
    OdGeVector2d(double x, double y);

    inline OdGeVector2d& operator=(const OdGeVector2d& vec);

    // The additive identity, x-axis, and y-axis.
    //
    static const   OdGeVector2d kIdentity;
    static const   OdGeVector2d kXAxis;
    static const   OdGeVector2d kYAxis;

    // Matrix multiplication.
    //
    friend GE_TOOLKIT_EXPORT OdGeVector2d operator*(const OdGeMatrix2d& mat, const OdGeVector2d& vec);
    OdGeVector2d&  setToProduct(const OdGeMatrix2d& mat, const OdGeVector2d& vec);
    OdGeVector2d&  transformBy(const OdGeMatrix2d& leftSide);
    OdGeVector2d&  rotateBy(double angle);
    OdGeVector2d&  mirror(const OdGeVector2d& line);


    // Scale multiplication.
    //
    OdGeVector2d   operator*(double scl) const;
    friend GE_TOOLKIT_EXPORT OdGeVector2d operator*(double scl, const OdGeVector2d& vec);
    OdGeVector2d&  operator*=(double scl);
    OdGeVector2d&  setToProduct(const OdGeVector2d& vec, double scl);
    OdGeVector2d   operator/(double scl) const;
    OdGeVector2d&  operator/=(double scl);

    // Addition and subtraction of vectors.
    //
    OdGeVector2d   operator+(const OdGeVector2d& vec) const;
    OdGeVector2d&  operator+=(const OdGeVector2d& vec);
    OdGeVector2d   operator-(const OdGeVector2d& vec) const;
    OdGeVector2d&  operator-=(const OdGeVector2d& vec);
    OdGeVector2d&  setToSum(const OdGeVector2d& vec1, const OdGeVector2d& vec2);
    OdGeVector2d   operator- () const;
    OdGeVector2d&  negate();

    // Perpendicular vector
    //
    OdGeVector2d   perpVector() const;

    // Angle argument.
    //
    double         angle() const;
    
    // Returns less angle between this vector and vec
    // This function is commutative, a.angleTo(b) == b.angleTo(a)
    double         angleTo(const OdGeVector2d& vec) const;

    // Returns angle between this vector and vec in counterclockwise
    // direction
    double         angleToCCW(const OdGeVector2d& vec) const;

    // Vector length operations.
    //
    OdGeVector2d   normal(const OdGeTol& tol = OdGeContext::gTol) const;
    OdGeVector2d&  normalize(const OdGeTol& tol = OdGeContext::gTol);
    OdGeVector2d&  normalize(const OdGeTol& tol, OdGeError& flag);
                        // Possible errors:  k0This.  Returns object unchanged on error.
    double         length() const;
    double         lengthSqrd() const;
    bool           isUnitLength(const OdGeTol& tol = OdGeContext::gTol) const;
    bool           isZeroLength(const OdGeTol& tol = OdGeContext::gTol) const;

    // Direction tests.
    //
    bool           isParallelTo(const OdGeVector2d& vec,
                                const OdGeTol& tol = OdGeContext::gTol) const;
    bool           isParallelTo(const OdGeVector2d& vec, const OdGeTol& tol,
                                OdGeError& flag) const;
                        // Possible errors:  k0This, k0Arg1.
                        // Returns false on error.
    bool           isCodirectionalTo(const OdGeVector2d& vec,
                                     const OdGeTol& tol=OdGeContext::gTol) const;
    bool           isCodirectionalTo(const OdGeVector2d& vec, const OdGeTol& tol,
                                     OdGeError& flag) const;
                        // Possible errors:  k0This, k0Arg1.
                        // Returns false on error.
    bool           isPerpendicularTo(const OdGeVector2d& vec,
                                     const OdGeTol& tol = OdGeContext::gTol) const;
    bool           isPerpendicularTo(const OdGeVector2d& vec, const OdGeTol& tol,
                                     OdGeError& flag) const;
                        // Possible errors:  k0This, k0Arg1.
                        // Returns false on error.

    // Dot product.
    //
    double         dotProduct(const OdGeVector2d& vec) const;

    // Z-projection of cross product
    double         crossProduct(const OdGeVector2d& vec) const;

    // Tests for equivalence using the Euclidean norm.
    //
    bool           operator==(const OdGeVector2d& vec) const;
    bool           operator!=(const OdGeVector2d& vec) const;
    bool           isEqualTo(const OdGeVector2d& vec,
                             const OdGeTol& tol = OdGeContext::gTol) const;

    // For convenient access to the data.
    //
    double         operator[](unsigned int i) const;
    double&        operator[](unsigned int i) ;
    OdGeVector2d&  set(double x, double y);

    // Convert to/from matrix form.
    //
    operator       OdGeMatrix2d() const;

    // Co-ordinates.
    //
    double         x, y;
};


// Creates the identity translation vector.
//
inline OdGeVector2d::OdGeVector2d() : x(0.0), y(0.0)
{}

inline OdGeVector2d::OdGeVector2d(const OdGeVector2d& src) : x(src.x), y(src.y)
{}

// Creates a vector intialized to  (xx, yy).
//
inline OdGeVector2d::OdGeVector2d(double xx, double yy) : x(xx), y(yy)
{}

inline OdGeVector2d& OdGeVector2d::operator=(const OdGeVector2d& vec)
{
  x = vec.x;
  y = vec.y;
  return *this;
}

inline OdGeVector2d& OdGeVector2d::setToProduct(const OdGeMatrix2d& mat, const OdGeVector2d& vec)
{
        *this = mat * vec;
        return *this;
}

inline OdGeVector2d& OdGeVector2d::transformBy(const OdGeMatrix2d& leftSide)
{
  return *this = leftSide * *this;
}

inline OdGeVector2d& OdGeVector2d::set(double xx, double yy)
{
  x = xx;
  y = yy;
  return *this;
}

// Z-projection of cross product
//
inline double OdGeVector2d::crossProduct(const OdGeVector2d& v) const
{
  return x * v.y - y * v.x;
}

inline double OdGeVector2d::angleToCCW(const OdGeVector2d& vec) const
{
  return crossProduct(vec) > 0.0 ? angleTo(vec) : -angleTo(vec);
}

#include "DD_PackPop.h"

#endif


