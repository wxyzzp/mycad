// File: GeArrays.h
//
// CGeVoidPointerArray, CGeIntArray, CGeDoubleArray, CGePoint2dArray, CGePoint3dArray, CGeVector2dArray, CGeVector3dArray
//

#pragma once

#pragma warning (disable : 4251)	// TO DO: check if these array classes are exported

#include <vector>

class CGePoint2d;
class CGePoint3d;
class CGeVector2d;
class CGeVector3d;

typedef std::vector<void*>			CGeVoidPointerArray;
typedef std::vector<int>			CGeIntArray;
typedef std::vector<double>			CGeDoubleArray;
typedef std::vector<CGePoint2d>		CGePoint2dArray;
typedef std::vector<CGePoint3d>		CGePoint3dArray;
typedef std::vector<CGeVector2d>	CGeVector2dArray;
typedef std::vector<CGeVector3d>	CGeVector3dArray;
