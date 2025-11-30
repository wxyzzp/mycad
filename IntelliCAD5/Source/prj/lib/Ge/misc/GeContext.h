// File: GeContext.h
//
// CGeContext
//

#pragma once


#include "GeTolerance.h"


class GE_API CGeVector3d;
class GE_API CGeTolerance;


struct GE_API CGeContext
{
	static CGeTolerance	s_defTol;

	static void  (*s_pErrorFunc)	();
	static void  (*s_pOrthoVector)	(const CGeVector3d& inputVector, CGeVector3d& resultVector);
	static void* (*s_oAllocMem)		(size_t size);
	static void  (*s_pFreeMem)		(void* pBuffer);
};
