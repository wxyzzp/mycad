/* F:\BLD\PRJ\LIB\SDS\SDS5.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Misc Functions that can all be stand alone 
 * 
 */ 

//** Includes

// 6/98 including icad.h doesn't work for the dwgcnvt - therefore I only include it when SDS_SDSSTANDALONE
// isn't defined

#if defined(SDS_SDSSTANDALONE)
	#error
	#include <windows.h>
	#include "db.h"/*DNT*/
	#include "icadlib.h"/*DNT*/
	#include "sds.h"/*DNT*/
#else
	#include "db.h"/*DNT*/
//	#include "Icad.h"/*DNT*/
#endif

char *SDS_wcstrtok(char *strToken,const char *strDelimit);
int sds_real_equal(sds_real r1, sds_real r2);
sds_real sds_dist2d (sds_point p1,sds_point p2);



//** Functions



/****************************************************************************/
/********************* Geometry Functions ***********************************/
/****************************************************************************/

// *****************************************
// This is an SDS External API
//
double 
sdsengine_angle(const sds_point ptStart, const sds_point ptEnd) 
	{
    sds_real dx,dy, angle;

	if( ptStart==NULL || 
		ptEnd==NULL ) 
		{
		return(0.0);
		}

    dx=ptEnd[0]-ptStart[0]; dy=ptEnd[1]-ptStart[1];
    if (icadRealEqual(dx,0.0) && icadRealEqual(dy,0.0)) 
		{
		return 0.0;
		}
    if ((angle=atan2(dy,dx))<0.0) 
		{
		angle+=IC_TWOPI;
		}
    return angle;
	}

// *****************************************
// This is an SDS External API
//
double 
sdsengine_distance(const sds_point ptFrom, const sds_point ptTo) 
	{
    sds_real dx,dy,dz;

	if( ptFrom==NULL || 
		ptTo==NULL ) 
		{
		return(0.0);
		}

    dx=ptTo[0]-ptFrom[0]; 
	dy=ptTo[1]-ptFrom[1]; 
	dz=ptTo[2]-ptFrom[2];

    return sqrt(dx*dx+dy*dy+dz*dz);
	}

sds_real sds_dist2d (sds_point p1,sds_point p2){
	return(sqrt((p2[0]-p1[0])*(p2[0]-p1[0])+(p2[1]-p1[1])*(p2[1]-p1[1])));
}

int sds_real_equal(sds_real r1, sds_real r2) {
	//returns 1 if values are approx equl, 0 otheriwse
	if(fabs(r2)<=IC_ZRO){
		if(fabs(r1)>IC_ZRO)return(0);
	}else if(fabs((r1/r2)-1.0) > IC_ZRO)return(0);
	return(1);
}


// *****************************************
// This is an SDS External API
//
void  
sdsengine_polar(const sds_point ptPolarCtr, double dAngle, double dDistance, sds_point ptPoint) 
	{
	ic_polar(ptPolarCtr, dAngle, dDistance, ptPoint);
	}


/****************************************************************************/
/***************** Char Type Handling Functions *****************************/
/****************************************************************************/
//
// These APIs are so simple (just direct calls to C runtime) no
// need to have engine versions
//

// *****************************************
// This is an SDS External API
//
int sds_isalnum(int nASCIIValue) 
	{ 
	return isalnum(nASCIIValue); 
	}

// *****************************************
// This is an SDS External API
//
int sds_isalpha(int nASCIIValue) 
	{ 
	return isalpha(nASCIIValue); 
	}

// *****************************************
// This is an SDS External API
//
int sds_iscntrl(int nASCIIValue) 
	{ 
	return iscntrl(nASCIIValue); 
	}

// *****************************************
// This is an SDS External API
//
int sds_isdigit(int nASCIIValue) 
	{ 
	return isdigit(nASCIIValue); 
	}

// *****************************************
// This is an SDS External API
//
int sds_isgraph(int nASCIIValue) 
	{ 
	return isgraph(nASCIIValue); 
	}

// *****************************************
// This is an SDS External API
//
int sds_islower(int nASCIIValue) 
	{ 
	return islower(nASCIIValue); 
	}

// *****************************************
// This is an SDS External API
//
int sds_isprint(int nASCIIValue) 
	{ 
	return isprint(nASCIIValue); 
	}

// *****************************************
// This is an SDS External API
//
int sds_ispunct(int nASCIIValue) 
	{ 
	return ispunct(nASCIIValue); 
	}

// *****************************************
// This is an SDS External API
//
int sds_isspace(int nASCIIValue) 
	{ 
	return isspace(nASCIIValue); 
	}

// *****************************************
// This is an SDS External API
//
int sds_isupper(int nASCIIValue) 
	{ 
	return isupper(nASCIIValue); 
	}

// *****************************************
// This is an SDS External API
//
int sds_isxdigit(int nASCIIValue) 
	{ 
	return isxdigit(nASCIIValue); 
	}

int sds_tolower(int nASCIIValue)  
	{ 
	return tolower(nASCIIValue); 
	}

int sds_toupper(int nASCIIValue)  
	{ 
	return toupper(nASCIIValue); 
	}




// ************************************************************************
// ************************************************************************
// ************************************************************************
// ************************************************************************
// ************************************************************************
// ************************************************************************
#if defined(SDS_SDSSTANDALONE)

	int sds_getvar(const char *szSysVar, struct sds_resbuf *prbVarInfo) {
		return(RTNORM);
	}

	int sds_setvar(const char *szSysVar, const struct sds_resbuf *prbVarInfo) {
		return(RTNORM);
	}

	int sds_command(int nRType, ...) {
		return(RTNORM);
	}

#endif


