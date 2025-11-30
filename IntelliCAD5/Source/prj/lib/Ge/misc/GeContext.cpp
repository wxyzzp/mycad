// File: GeContext.cpp
//

#include "GeContext.h"


CGeTolerance CGeContext::s_defTol;


void (*CGeContext::s_pErrorFunc)() = 0;
void (*CGeContext::s_pOrthoVector)(const CGeVector3d& inputVector, CGeVector3d& resultVector) = 0;
void* (*CGeContext::s_oAllocMem)(size_t size) = 0;
void (*CGeContext::s_pFreeMem)(void* pBuffer) = 0;
