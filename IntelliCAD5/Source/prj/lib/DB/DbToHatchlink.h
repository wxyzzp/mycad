/* G:\ICADDEV\PRJ\LIB\DB\DBTOHATCHLINK.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract:  
 * 
 * The DbToHatchlink class provides routines that modify an existing hatchlink entity
 * in place according to the corresponding db_entity.  The existing hatchlink entity
 * must be compatible with the corresponding db_entity.
 * 
 */ 

#ifndef _INC_DBTOHATCHLINK
#define _INC_DBTOHATCHLINK

#include "Geometry.h" // for RC

class db_line;
class db_arc;
class db_ellipse;
class db_lwpolyline;
class db_polyline;
class db_spline;


class db_hatchlinelink;
class db_hatcharclink;
class db_hatchellarclink;
class db_hatchplinelink;
class db_hatchsplinelink;


class DB_CLASS DbToHatchlink
	{

	private:

	bool compatible(db_lwpolyline * LWPolyline, // I
		            db_hatchplinelink * HatchPlineLink); // I

	bool compatible(db_polyline * Polyline, // I
		            db_hatchplinelink * HatchPlineLink); // I

	bool compatible(db_spline * Spline, // I
		            db_hatchsplinelink * HatchSplineLink); // I

    public:

	RC LineToHatchlink(db_line  * Line, // I
										db_hatchlinelink * HatchLineLink); // IO
	
	RC ArcToHatchlink(db_arc * Arc, // I
									   db_hatcharclink * HatchArcLink); //IO

	RC EllarcToHatchlink(db_ellipse * Ellarc, // I
		                                  db_hatchellarclink * HatchEllarcLink); //IO

	RC SplineToHatchlink(db_spline * Spline, // I
		                                  db_hatchsplinelink * HatchSplineLink); //IO

	RC PolylineToHatchlink(db_polyline * Polyline, // I
		                                    db_hatchplinelink * HatchPlineLink); // IO

	RC LWPolylineToHatchlink(db_lwpolyline * LWPolyline, // I
		                                      db_hatchplinelink * HatchPlineLink); //IO

	RC CircleToHatchlink(db_circle * C, // I
										  db_hatchplinelink * PL); // IO	
		                       
					
	};


#endif // _INC_DBTOHATCHLINK

