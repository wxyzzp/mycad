/* D:\ICADDEV\PRJ\LIB\GR\SPLINEUTILS.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * The purpose of this file is to provide a place for declarations of
 * spline conversion functions that use both IntelliCAD and GEO objects
 * 
 */ 

#ifndef SPLINEUTILS_H
#define SPLINEUTILS_H



GR_API RC GetSplineFrom_resbuf(
    sds_resbuf * rbSpline, //I: The linked list containing the spline data
	PSpline & pSpline,	   // O: The spline
	C3Point & ptNormal);   // O: The view plane upward normal

GR_API RC BuildSplineList(
	PSpline pSpline,			// I: The spline
	sds_resbuf ** rbSpline);	// O: The linked lis containing the spline data

GR_API RC CopyAttributes(
	sds_resbuf * rbFrom,	// In: The list to copy from
	sds_resbuf * rbTo);		// In: The list to copy To

#endif

