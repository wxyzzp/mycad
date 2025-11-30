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



#ifndef OD_GE_MATRIX_3D_H
#define OD_GE_MATRIX_3D_H

#include "GeGbl.h"
#include "GeMatrix2d.h"
#include "GePoint3d.h"

class OdGeLine3d;
class OdGeVector3d;
class OdGePlane;
class OdGeTol;
class OdGeScale3d;

#include "DD_PackPush.h"

/** Description:
    
    {group:OdGe_Classes} 
*/
class GE_TOOLKIT_EXPORT OdGeMatrix3d
{
public:
  OdGeMatrix3d();

  // The multiplicative identity.
  //
  static const OdGeMatrix3d kIdentity;

  // Reset matrix.
  //
  OdGeMatrix3d& setToIdentity();
  void validateZero(const OdGeTol& tol = OdGeContext::gTol);  // for correcting in case that Zero is corrupted

  // Multiplication.
  //
  OdGeMatrix3d  operator*   (const OdGeMatrix3d& mat) const;
  OdGeMatrix3d& operator*=  (const OdGeMatrix3d& mat);
  OdGeMatrix3d& preMultBy   (const OdGeMatrix3d& leftSide);
  OdGeMatrix3d& postMultBy  (const OdGeMatrix3d& rightSide);
  OdGeMatrix3d& setToProduct(const OdGeMatrix3d& mat1, const OdGeMatrix3d& mat2);

  // Multiplicative inverse.
  //
  OdGeMatrix3d& invert();
  OdGeMatrix3d  inverse() const;

  // Test if it is a singular matrix.  A singular matrix is not invertable.
  //
  bool isSingular(const OdGeTol& tol = OdGeContext::gTol) const;

  // Matrix transposition.
  //
  OdGeMatrix3d& transposeIt();
  OdGeMatrix3d  transpose() const;

  // Tests for equivalence using the infinity norm.
  //
  bool operator==(const OdGeMatrix3d& mat) const;
  bool operator!=(const OdGeMatrix3d& mat) const;
  bool isEqualTo(const OdGeMatrix3d& mat, const OdGeTol& tol = OdGeContext::gTol) const;

  // Test scaling effects of matrix
  //
  bool isUniScaledOrtho(const OdGeTol& tol = OdGeContext::gTol) const;
  bool isScaledOrtho   (const OdGeTol& tol = OdGeContext::gTol) const;

  bool isPerspective(const OdGeTol& tol = OdGeContext::gTol) const;

  // Determinant
  //
  double det() const;

  // Set/retrieve translation.
  //
  OdGeMatrix3d& setTranslation(const OdGeVector3d& vec);
  OdGeVector3d  translation() const;

  // Set/get coordinate system
  //
  OdGeMatrix3d& setCoordSystem(const OdGePoint3d& origin, const OdGeVector3d& xAxis,
                               const OdGeVector3d& yAxis, const OdGeVector3d& zAxis);
  void getCoordSystem(OdGePoint3d& origin, OdGeVector3d& xAxis, 
                      OdGeVector3d& yAxis, OdGeVector3d& zAxis) const;
  OdGePoint3d getCsOrigin() const;
  OdGeVector3d getCsXAxis() const;
  OdGeVector3d getCsYAxis() const;
  OdGeVector3d getCsZAxis() const;

  // Set the matrix to be a specified transformation
  //
  OdGeMatrix3d& setToTranslation(const OdGeVector3d& vec);
  OdGeMatrix3d& setToRotation(double angle, const OdGeVector3d& axis,
                              const OdGePoint3d& center = OdGePoint3d::kOrigin);
  OdGeMatrix3d& setToScaling(double scaleAll, const OdGePoint3d& center = OdGePoint3d::kOrigin);
  OdGeMatrix3d& setToScaling(const OdGeScale3d& scale, const OdGePoint3d& center = OdGePoint3d::kOrigin);
  OdGeMatrix3d& setToMirroring(const OdGePlane& pln);
  OdGeMatrix3d& setToMirroring(const OdGePoint3d& pnt);
  OdGeMatrix3d& setToMirroring(const OdGeLine3d& line);
  OdGeMatrix3d& setToProjection(const OdGePlane& projectionPlane,
                                const OdGeVector3d& projectDir);
  OdGeMatrix3d& setToAlignCoordSys(const OdGePoint3d& fromOrigin,
                                   const OdGeVector3d& fromXAxis,
                                   const OdGeVector3d& fromYAxis,
                                   const OdGeVector3d& fromZAxis,
                                   const OdGePoint3d& toOrigin,
                                   const OdGeVector3d& toXAxis,
                                   const OdGeVector3d& toYAxis,
                                   const OdGeVector3d& toZAxis);

  OdGeMatrix3d& setToWorldToPlane(const OdGeVector3d& normal);
  OdGeMatrix3d& setToWorldToPlane(const OdGePlane& plane);
  OdGeMatrix3d& setToPlaneToWorld(const OdGeVector3d& normal);
  OdGeMatrix3d& setToPlaneToWorld(const OdGePlane& plane);

  // Similar to above, but creates matrix on the stack.
  //
  static OdGeMatrix3d translation(const OdGeVector3d& vec);
  static OdGeMatrix3d rotation(double angle, const OdGeVector3d& axis,
                               const OdGePoint3d& center = OdGePoint3d::kOrigin);
  static OdGeMatrix3d scaling(double scaleAll, const OdGePoint3d& center = OdGePoint3d::kOrigin);
  static OdGeMatrix3d scaling(const OdGeScale3d& scale, const OdGePoint3d& center = OdGePoint3d::kOrigin);
  static OdGeMatrix3d mirroring(const OdGePlane& pln);
  static OdGeMatrix3d mirroring(const OdGePoint3d& pnt);
  static OdGeMatrix3d mirroring(const OdGeLine3d& line);
  static OdGeMatrix3d projection(const OdGePlane& projectionPlane,
                                 const OdGeVector3d& projectDir);
  static OdGeMatrix3d alignCoordSys(const OdGePoint3d&  fromOrigin,
                                    const OdGeVector3d& fromXAxis,
                                    const OdGeVector3d& fromYAxis,
                                    const OdGeVector3d& fromZAxis,
                                    const OdGePoint3d&  toOrigin,
                                    const OdGeVector3d& toXAxis,
                                    const OdGeVector3d& toYAxis,
                                    const OdGeVector3d& toZAxis);

  static OdGeMatrix3d worldToPlane(const OdGeVector3d& normal);
  static OdGeMatrix3d worldToPlane(const OdGePlane&);
  static OdGeMatrix3d planeToWorld(const OdGeVector3d& normal);
  static OdGeMatrix3d planeToWorld(const OdGePlane&);

// Get the length of the MAXIMUM column of the
// 3x3 portion of the matrix.
  //
  double scale(void) const;

  double norm() const;

  OdGeMatrix2d convertToLocal(OdGeVector3d& normal, double& elev) const;

  bool inverse(OdGeMatrix3d& inv, double tol) const;

  // For convenient access to the data.
  //
  const double* operator[] (int row) const { return entry[row]; }
  double* operator[] (int row) { return entry[row]; }

  double operator() (int row, int column) const { return entry[row][column]; }
  double& operator() (int row, int column) { return entry[row][column]; }
  
  // The components of the matrix.
  //
  double entry[4][4];    // [row][column]

private:

  void pivot(int, OdGeMatrix3d&);
  int pivotIndex(int) const;
  void swapRows(int, int, OdGeMatrix3d&);
};

inline bool OdGeMatrix3d::operator==(const OdGeMatrix3d& mat) const
{
  return isEqualTo(mat);
}

inline bool OdGeMatrix3d::operator!=(const OdGeMatrix3d& mat) const
{
  return !isEqualTo(mat);
}

#include "DD_PackPop.h"

#endif // OD_GE_MATRIX_3D_H


