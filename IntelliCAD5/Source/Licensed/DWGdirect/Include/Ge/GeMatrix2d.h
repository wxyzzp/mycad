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



#ifndef OD_GE_MATRIX_2D_H
#define OD_GE_MATRIX_2D_H

#include "GeGbl.h"
#include "GePoint2d.h"

class OdGeVector2d;
class OdGeLine2d;
class OdGeTol;

#include "DD_PackPush.h"

/** Description:
    
    {group:OdGe_Classes} 
*/
class GE_TOOLKIT_EXPORT OdGeMatrix2d
{
public:
  OdGeMatrix2d();

  // The multiplicative identity.
  //
  static const OdGeMatrix2d  kIdentity;

  // Reset matrix.
  //
  OdGeMatrix2d& setToIdentity();

  // Multiplication.
  //
  OdGeMatrix2d  operator*  (const OdGeMatrix2d& mat) const;
  OdGeMatrix2d& operator*= (const OdGeMatrix2d& mat);
  OdGeMatrix2d& preMultBy  (const OdGeMatrix2d& leftSide);
  OdGeMatrix2d& postMultBy (const OdGeMatrix2d& rightSide);
  OdGeMatrix2d& setToProduct(const OdGeMatrix2d& mat1, const OdGeMatrix2d& mat2);

  // Multiplicative inverse.
  //
  OdGeMatrix2d& invert();
  OdGeMatrix2d  inverse() const;

  // Test if it is a singular matrix. A singular matrix is not invertable.
  //
  bool isSingular(const OdGeTol& tol = OdGeContext::gTol) const;

  // Matrix transposition.
  //
  OdGeMatrix2d& transposeIt();
  OdGeMatrix2d  transpose() const;

  // Tests for equivalence using the infinity norm.
  //
  bool operator == (const OdGeMatrix2d& mat) const;
  bool operator != (const OdGeMatrix2d& mat) const;
  bool isEqualTo(const OdGeMatrix2d& mat,
    const OdGeTol& tol = OdGeContext::gTol) const;

  // Test scaling effects of matrix
  //
  bool isUniScaledOrtho(const OdGeTol& tol = OdGeContext::gTol) const;
  bool isScaledOrtho(const OdGeTol& tol = OdGeContext::gTol) const;
  double scale(void) const;

  // Determinant
  //
  double det() const;

  // Set/retrieve translation.
  //
  OdGeMatrix2d& setTranslation(const OdGeVector2d& vec);
  OdGeVector2d  translation() const;

  // Retrieve scaling, rotation, mirror components
  //
  bool isConformal(double& scale, double& angle,
                   bool& isMirror, OdGeVector2d& reflex) const;


  // Set/get coordinate system
  //
  OdGeMatrix2d& setCoordSystem(const OdGePoint2d& origin, const OdGeVector2d& e0,
                               const OdGeVector2d& e1);
  void getCoordSystem(OdGePoint2d& origin, OdGeVector2d& e0, OdGeVector2d& e1) const;

  // Set the matrix to be a specified transformation
  //
  OdGeMatrix2d& setToTranslation(const OdGeVector2d& vec);
  OdGeMatrix2d& setToRotation(double angle, 
                              const OdGePoint2d& center = OdGePoint2d::kOrigin);
  OdGeMatrix2d& setToScaling(double scaleAll,
                             const OdGePoint2d& center = OdGePoint2d::kOrigin);
  OdGeMatrix2d& setToMirroring(const OdGePoint2d& pnt);
  OdGeMatrix2d& setToMirroring(const OdGeLine2d& line);
  OdGeMatrix2d& setToAlignCoordSys(const OdGePoint2d&  fromOrigin,
                               const OdGeVector2d& fromE0,
                               const OdGeVector2d& fromE1,
                               const OdGePoint2d&  toOrigin,
                               const OdGeVector2d& toE0,
                               const OdGeVector2d& toE1);

  // Functions that make a 2d transformation matrix using various approaches
  //
  static OdGeMatrix2d translation(const OdGeVector2d& vec);
  static OdGeMatrix2d rotation(double angle, 
                               const OdGePoint2d& center = OdGePoint2d::kOrigin);
  static OdGeMatrix2d scaling(double scaleAll, 
                              const OdGePoint2d& center = OdGePoint2d::kOrigin);
  static OdGeMatrix2d mirroring(const OdGePoint2d& pnt);
  static OdGeMatrix2d mirroring(const OdGeLine2d& line);
  static OdGeMatrix2d alignCoordSys(const OdGePoint2d& fromOrigin,
                                    const OdGeVector2d& fromE0,
                                    const OdGeVector2d& fromE1,
                                    const OdGePoint2d&  toOrigin,
                                    const OdGeVector2d& toE0,
                                    const OdGeVector2d& toE1);


  // For convenient access to the data.
  //
  const double* operator[] (int row) const { return entry[row]; }
  double* operator[] (int row) { return entry[row]; }

  double  operator() (int row, int column) const { return entry[row][column]; }
  double& operator() (int row, int column) { return entry[row][column]; }

  //         [row][column]
  double entry[3][3];
};

inline bool OdGeMatrix2d::operator==(const OdGeMatrix2d& mat) const
{
  return isEqualTo(mat);
}

#include "DD_PackPop.h"

#endif // OD_GE_MATRIX_2D_H


