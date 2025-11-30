/* D:\ICADDEV\PRJ\LIB\ICADLIB\VECUTILS.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * This file contains the implementation of some basic vector utilities.
 * 
 */ 

#include "sds.h"
#include <math.h>
#include "..\..\Icad\IcadUtil.h"

//-----------------------------------------------------------------

// calculates the vector pt2+pt1
void ic_add (sds_point pt1, sds_point pt2, sds_point pt3)
	{
	pt3[0] = pt2[0] + pt1[0];
	pt3[1] = pt2[1] + pt1[1];
	pt3[2] = pt2[2] + pt1[2];
	}

//-----------------------------------------------------------------

//calculates the vector pt2-pt1
void ic_sub (sds_point pt1, sds_point pt2, sds_point pt3) 
	{
	pt3[0] = pt2[0] - pt1[0];
	pt3[1] = pt2[1] - pt1[1];
	pt3[2] = pt2[2] - pt1[2];
	}

//-----------------------------------------------------------------

//calculates the unit vector (pt2-pt1)/|pt2-pt1|
void ic_unitvec (sds_point pt1, sds_point pt2, sds_point pt3) 
	{
	double d = sqrt((pt2[0] - pt1[0])*(pt2[0] - pt1[0]) +
					(pt2[1] - pt1[1])*(pt2[1] - pt1[1]) +
					(pt2[2] - pt1[2])*(pt2[2] - pt1[2]));

	if (icadRealEqual(d,0.0)) return;
	pt3[0] = (pt2[0] - pt1[0])/d;
	pt3[1] = (pt2[1] - pt1[1])/d;
	pt3[2] = (pt2[2] - pt1[2])/d;
	}

//-----------------------------------------------------------------

// this function interchanges p and q
void ic_swap (sds_point & p, sds_point & q)
	{
	sds_point tmp;
	for (int i = 0; i < 3; i++)
		{
		tmp[i] = p[i];
		p[i] = q[i];
		q[i] = tmp[i];
		}
	}

//-----------------------------------------------------------------

// this function finds the midpoint of a and b
void ic_midpoint(sds_point a, sds_point b, sds_point mid)
	{
	mid[0] = 0.5*(a[0] + b[0]);
	mid[1] = 0.5*(a[1] + b[1]);
	mid[2] = 0.5*(a[2] + b[2]);
	}

#define IC_ZRO           1.0E-10

int ic_pt_equal(sds_point p1, sds_point p2) 
	{
	
	if(fabs(p2[0])<=IC_ZRO)
		{
		if(fabs(p1[0])>IC_ZRO)return(0);
		}
	else if(fabs((p1[0]/p2[0])-1.0) >= IC_ZRO)
		{
		return(0);
		}
	if(fabs(p2[1])<=IC_ZRO)
		{
		if(fabs(p1[1])>IC_ZRO)return(0);
		}
	else if(fabs((p1[1]/p2[1])-1.0) >= IC_ZRO)
		{
		return(0);
		}
	if(fabs(p2[2])<=IC_ZRO)
		{
		if(fabs(p1[2])>IC_ZRO)return(0);
		}
	else if(fabs((p1[2]/p2[2])-1.0) >= IC_ZRO)
		{
		return(0);
		}
	return(1);
	}

double ic_veclength(sds_point p)
	{
	return sqrt(p[0]*p[0] + p[1]*p[1] + p[2]*p[2]);
	}

void ic_unitize(sds_point p)
	{
	double d = ic_veclength(p);
	if (d > IC_ZRO)
		{
		p[0] /= d;
		p[1] /= d;
		p[2] /= d;
		}
	else
		{
		p[0] = 0.0;
		p[1] = 0.0;
		p[2] = 0.0;
		}
	}

