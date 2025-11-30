/* G:\ICADDEV\PRJ\LIB\DB\DBTOHATCHLINK.CPP
* Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
* 
* Abstract
* 
* <TODO : Add Abstract here>
* 
*/ 

#include "db.h"
#include "DbToHatchLink.h"

bool DbToHatchlink::compatible(db_lwpolyline * PL1, // I
							   db_hatchplinelink * PL2) // I
	{
	bool isCompatible; 

	// The two input objects are compatible if and only if
	// 1.  They are either both closed or both not closed.
	// 2.  They have the same number of vertices.

	// we are not going to be fancy here, just do it by brute force

	int PL1flags;
	int PL2flags;

	PL1->get_70(&PL1flags);
	PL2flags = PL2->flags;

	bool PL1isclosed = PL1flags & 1;
	bool PL2isclosed = PL2flags & 1;

	int  PL1nverts;
	PL1->get_90(&PL1nverts);
	int  PL2nverts = PL2->nverts;

	if ((PL1isclosed == PL2isclosed) && 
		(PL1nverts == PL2nverts))
		isCompatible = true;
	else
		isCompatible = false;

	return isCompatible;
	}

bool DbToHatchlink::compatible(db_polyline * PL1, // I
							   db_hatchplinelink * PL2) // I
	{
	bool isCompatible;

	// The two input objects are compatible if and only if
	// 1.  They are either both closed or both not closed.
	// 2.  They have the same number of vertices.

	int PL1flags;
	int PL2flags;

	PL1->get_70(&PL1flags);
	PL2flags = PL2->flags;

	bool PL1isclosed = PL1flags & 1;
	bool PL2isclosed = PL2flags & 1;

	int PL1nverts;
	PL1->count_ordinary_vertices(&PL1nverts);
	int PL2nverts = PL2->nverts;

	if ((PL1isclosed == PL2isclosed) &&
		(PL1nverts == PL2nverts))
		isCompatible = true;
	else
		isCompatible = false;

	return isCompatible;
	}

bool DbToHatchlink::compatible(db_spline * SPL1, // I
							   db_hatchsplinelink * SPL2) // I
	{
	bool isCompatible = true;
	
	int SPL1deg;
	SPL1->get_71(&SPL1deg);

	int SPL2deg = SPL2->deg;
	
	int SPL1nknots;
	SPL1->get_72(&SPL1nknots);

	int SPL2nknots = SPL2->nknots;
	
	int SPL1ncpts;
	SPL1->get_72(&SPL1ncpts);
	int SPL2ncpts = SPL2->ncpts;
	
	int SPL1flags;
	SPL1->get_70(&SPL1flags);
	int SPL2flags = SPL2->flags;

	int SPL1rational = SPL1flags & 4;
	int SPL2rational = SPL2flags & 4;
	
	int SPL1periodic = SPL1flags & 2;
	int SPL2periodic = SPL2flags & 2;
	
	if ( (SPL1deg == SPL2deg) &&
		(SPL1nknots == SPL2nknots) &&
		(SPL1ncpts == SPL2ncpts) &&
		(SPL1rational == SPL2rational) &&
		(SPL1periodic == SPL2periodic) )
		isCompatible = true;
	else
		isCompatible = false;

	return isCompatible;
	}


RC DbToHatchlink::LineToHatchlink(
												   db_line  * L1, // I
												   db_hatchlinelink * L2) // IO
	{
	RC rc;
	
	sds_point StartPt;
	sds_point EndPt;
	
	if(L1->get_10(StartPt) && L1->get_11(EndPt))
		{
		
		L2->pt[0][0] = StartPt[0];
		L2->pt[0][1] = StartPt[1];
		
		L2->pt[1][0] = EndPt[0];
		L2->pt[1][1] = EndPt[1];
		
		rc = SUCCESS;
		}
	else
		{
		rc = FAILURE;
		}
	
	
	return rc;
	}

RC DbToHatchlink::ArcToHatchlink(
												  db_arc * Arc1, // I
												  db_hatcharclink * Arc2) //IO
	{
	RC rc = SUCCESS;

	sds_point Center;
	sds_real radius;
	sds_real startAng;
	sds_real endAng;

	Arc1->get_10(Center);
	Arc1->get_40(&radius);
	Arc1->get_50(&startAng);
	Arc1->get_51(&endAng);

	Arc2->cent[0] = Center[0];
	Arc2->cent[1] = Center[1];
	Arc2->rad = radius;
	Arc2->ang[0] = startAng;
	Arc2->ang[1] = endAng;

	return rc;
	}

RC DbToHatchlink::EllarcToHatchlink(
													 db_ellipse * Ellarc1, // I
													 db_hatchellarclink * Ellarc2) //IO
	{
	RC rc = SUCCESS;
	sds_point Center;
	sds_point SmaVect;
	sds_real Min_Maj;
	sds_real startParm;
	sds_real endParm;

	Ellarc1->get_10(Center);
	Ellarc1->get_11(SmaVect);
	Ellarc1->get_40(&Min_Maj);
	Ellarc1->get_41(&startParm);
	Ellarc1->get_42(&endParm);

	Ellarc2->cent[0] = Center[0];
	Ellarc2->cent[1] = Center[1];
	Ellarc2->smavect[0] = SmaVect[0];
	Ellarc2->smavect[1] = SmaVect[1];
	Ellarc2->min_maj = Min_Maj;
	Ellarc2->param[0] = startParm;
	Ellarc2->param[1] = endParm;

	return rc;
	}

RC DbToHatchlink::SplineToHatchlink(
													 db_spline * Spl1, // I
													 db_hatchsplinelink * Spl2) //IO
	{
	RC rc = FAILURE;

    if (compatible(Spl1, Spl2))
		{   // begin updating Spl2 based upon Spl1

			Spl1->get_40array(Spl2->knot, Spl2->nknots); 

		// update the control points and weights, if applicable

			sds_point * P = new sds_point[Spl2->ncpts];
			Spl1->get_10(P, Spl2->ncpts);

			sds_real * w = new sds_real[Spl2->ncpts];
			Spl1->get_41array(w, Spl2->ncpts);

			for (int i = 0; i < Spl2->ncpts; i++)
				{   // begin i-loop
				Spl2->control[i].pt[0] = P[i][0];
				Spl2->control[i].pt[1] = P[i][1];
				Spl2->control[i].wt = w[i];
				}   //   end i-loop

			delete [] P;
			delete [] w;
			rc = SUCCESS;

		}   //   end updating Spl2 based upon Spl1
	else
		rc = FAILURE;

	return rc;
	}

RC DbToHatchlink::PolylineToHatchlink(
													   db_polyline * PL1, // I
													   db_hatchplinelink * PL2) // IO
	{
	RC rc;
	if (compatible(PL1, PL2))
		{   // begin case of PL1 and PL2 compatible

    	bool done = false;
		int flags;
		int i = 0; // vertex index
		sds_point p;
		double b;
		db_handitem * hip = PL1;

		while (!done)
			{   // begin while (!done)
			hip = hip->next;
			if (hip != NULL)
				{   // begin case of hip != NULL
				if (hip->ret_type() == DB_VERTEX)
					{   //  begin case of == DB_VERTEX
					hip->get_70(&flags);
					// only process this vertex if it is NOT a splineframe vertex
					if (!(flags & IC_VERTEX_SPLINEFRAME))
						{
						hip->get_10(p);
						hip->get_42(&b);
						PL2->vert[i].pt[0] = p[0];
						PL2->vert[i].pt[1] = p[1];
						PL2->vert[i].bulge = b;
						++i;
						}
					}   //    end case of == DB_VERTEX
				else
					{   //  begin case of != DB_VERTEX
					// quit as soon as we encounter a non-DB_VERTREX
					done = true;
					}   //    end case of != DB_VERTEX
                }   //   end case of hip != NULL
			else
				{   // begin case of hip == NULL
				// quit as soon as we encounter a NULL hip
				done = true;
				}   //   end case of hip == NULL

			}   //   end while (!done)

		rc = SUCCESS;
		}   //  end case of PL1 and PL2 compatible
	else
		{
		rc = FAILURE;
		}
	return rc;
	}

RC DbToHatchlink::LWPolylineToHatchlink(
														 db_lwpolyline * PL1, // I
														 db_hatchplinelink * PL2) //IO
	{
	RC rc;
	sds_point p;
	sds_real b;
	bool hasBulges;
	
	if ( compatible(PL1, PL2) )
		{ // begin updating PL2 based upon PL1
		hasBulges = PL1->hasbulges();
		for (int i = 0; i < PL2->nverts; i++)
			{
			PL1->get_10(p, i);
			PL2->vert[i].pt[0] = p[0];
			PL2->vert[i].pt[1] = p[1];
			if (hasBulges)
				{
				PL1->get_42(&b,i);
				PL2->vert[i].bulge = b;
				}
			else
				PL2->vert[i].bulge = 0.0;
			}
		rc = SUCCESS;
		} //   end updating PL2 based upon PL1
	else
		{
		rc = FAILURE;
		}
	
	return rc;
	}

RC DbToHatchlink::CircleToHatchlink(db_circle * C,
													 db_hatchplinelink * PL)
	{
	RC rc;
	if ( PL->nverts == 2) 
		{
		sds_point center;
		sds_real radius;

		C->get_10(center);
		C->get_40(&radius);

		double x0 = center[0] + radius;
		double y0 = center[1];

		double x1 = center[0] - radius;
		double y1 = center[1];

		PL->vert[0].pt[0] = x0;
		PL->vert[0].pt[1] = y0;
		PL->vert[0].bulge = 1.0;

		PL->vert[1].pt[0] = x1;
		PL->vert[1].pt[1] = y1;
		PL->vert[1].bulge = 1.0;
		
		rc = SUCCESS;
		}
	else
		{
		rc = FAILURE;
		}


	return rc;
	}



