/* CIRCLE.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * Supports the circle and donut commands
 *
 */
// THIS FILE HAS BEEN GLOBALIZED!
//** Includes
#include "cmds.h"/*DNT*/
#include "Icad.h"/*DNT*/
#include "Geometry.h"/*DNT*/
#include "curve.h"/*DNT*/
#include "GeoConvert.h"/*DNT*/
#include "line_or_circle.h"/*DNT*/
#include "TanCircs.h"/*DNT*/
#include "IcadView.h"


// **********
// GLOBALS
//
extern double SDS_CorrectElevation;
extern bool	  SDS_ElevationChanged;
extern signed char SDS_OSNAP_TANPER_IGNORE;



// ************************
// HELPER FUNCTIONS
//
extern 
int cmd_ent_to_2pt
(
 sds_point pickpt,
 sds_name ent1,
 sds_point ep1,
 sds_point ep2,
 int* type,
 sds_resbuf* rbent
);


void setCenterPoint(sds_point,sds_point,sds_point,sds_point,
					sds_point *,sds_real *,sds_point,sds_point);
					

static 
int cmd_circle2pt_tan_per
(
 sds_point pt1,
 sds_point pt2,
 const unsigned char tan_per1,
 const unsigned char tan_per2
 )
{
	//  pt1 and pt2 are points on the diameter of a circle. based upon
	//	bit 0 being PER snap and bit 1 being TAN snap, adjust these points for proper
	//	construction of line.  PER has higher priority.
	//NOTEs:	pt1 and pt2 are UCS point

	//Returns:  RTNORM if successful geometric sol'n found
	//			RTNONE if no solution found
	//			RTERROR otherwise


	sds_resbuf rb,rbent1,rbent2,rbucs;
	sds_real fr1,fr2,fr3,fr5,fr6; //fr4,
	sds_point p1,p2,p3,p4,lp1,lp2,p210,screenpt1,screenpt2,p5,p6,p7;
	sds_name ss1,ss2,ent1,ent2;
	struct gr_view *view=SDS_CURGRVW;
	int ret=RTNORM,fi1,type1,type2,done=0,reverse_ents=0;
	long fl1,fl2;

	ic_ptcpy(lp1,pt1);
	ic_ptcpy(lp2,pt2);

	if(view==NULL) return(RTERROR);
	rbucs.restype=RTSHORT;
	rbucs.resval.rint=1;

	ss1[0]=ss1[1]=ss2[0]=ss2[1]=0L;

	// Get UCS
	// X axis - p1
	if (SDS_getvar(NULL,
		DB_QUCSXDIR,
		&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES) != RTNORM)
	{
		ret=RTERROR;
		goto exit;
	}
	ic_ptcpy(p1,rb.resval.rpoint);
	// Y acis - p2
	if (SDS_getvar(NULL,
		DB_QUCSYDIR,
		&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES) != RTNORM)
	{
		ret=RTERROR;
		goto exit;
	}
	ic_ptcpy(p2,rb.resval.rpoint);
	// Z axis - p210
	p210[0]=(p1[1]*p2[2])-(p2[1]*p1[2]);
	p210[1]=(p1[2]*p2[0])-(p2[2]*p1[0]);
	p210[2]=(p1[0]*p2[1])-(p2[0]*p1[1]);

	// Query VIEWSIZE
	if (SDS_getvar(NULL,
		DB_QVIEWSIZE,
		&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
	{
		ret=RTERROR;
		goto exit;
	}
	fr1=rb.resval.rreal;
	// Query SDCREENSIZE
	if (SDS_getvar(NULL,
		DB_QSCREENSIZE,
		&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
	{
		ret=RTERROR;
		goto exit;
	}
	fr1/=rb.resval.rpoint[1];
	// Query APERTURE
	if (SDS_getvar(NULL,
		DB_QAPERTURE,
		&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
	{
		ret=RTERROR;
		goto exit;
	}
	fr1*=rb.resval.rint;
	// Now fr1 is (VIEWSIZE/SCREENSIZE[1])*APERTURE
	
	for (fi1 = 0; fi1 < 2; fi1++)
	{
		if (fi1 == 0)
		{
			gr_ucs2rp(lp1,p1,view);
			ic_ptcpy(screenpt1,p1);
		}
		else
		{
			gr_ucs2rp(lp2,p1,view);
			ic_ptcpy(screenpt2,p1);
		}

		p3[0]=p1[0]-fr1;
		p3[1]=p1[1]-fr1;
		p4[0]=p1[0]+fr1;
		p4[1]=p1[1]+fr1;
		p3[2]=p4[2]=0.0;

		gr_rp2ucs(p3,p1,view);
		gr_rp2ucs(p4,p2,view);

		if (0 == fi1)
		{
			if ((ret = sds_pmtssget(NULL,"C"/*DNT*/,p1,p2,NULL,ss1,0))!=RTNORM)
			{
				ret=RTNONE;
				goto exit;
			}
			sds_sslength(ss1,&fl1);
		}
		else
		{
			if ((ret=sds_pmtssget(NULL,"C"/*DNT*/,p1,p2,NULL,ss2,0))!=RTNORM)
			{
				ret = RTNONE;
				goto exit;
			}
			sds_sslength(ss2,&fl1);
		}
		if (fl1 == 0L) 
		{
            ret=RTNONE;
            goto exit;
        }
	}

	for (fl1=0L; RTNORM==sds_ssname(ss1,fl1,ent1); fl1++)
	{
		if (RTNORM != cmd_ent_to_2pt(lp1,ent1,p1,p2,&type1,&rbent1))
			continue;

		for (fl2=0L; RTNORM==sds_ssname(ss2,fl2,ent2); fl2++)
		{
			if (RTNORM != cmd_ent_to_2pt(lp2,ent2,p3,p4,&type2,&rbent2))
				continue;

			//Error checks
			//If both are lines, make sure they are parallel
			if (type1==0 && type2==0)
			{
				sds_point u,v;
				ic_unitvec (p1,p2,u); 
				ic_unitvec (p3,p4,v);
				if (fabs((ic_dotproduct(u,v)) - 1.0) > IC_ZRO)
					continue;
			}

			//if one is a line and the other is a circle, make sure the line is not touching the circle
			if (type1==0 && type2!=0)
			{
				if(-1==ic_ptlndist(p1,p2,p3,&fr1,p7)) 
					continue;
				if (fabs(fr1 - p4[0]) < IC_ZRO) 
					continue;
			}
			if (type1!=0 && type2==0)
			{
				if(-1==ic_ptlndist(p3,p4,p1,&fr1,p7)) 
					continue;
				if (fabs(fr1 - p2[0]) < IC_ZRO) 
					continue;
			}

			//at this point, we have the two ents we want to work with, so check for osnaps
			if ((tan_per2&2) && (tan_per1&2))
			{
				if (type1==0 && type2==0) //Circle tangent to 2 lines
				{
					if(-1==ic_ptlndist(pt1,p3,p4,&fr1,pt2)) 
						continue;
					done = TRUE;
				}
				else if (type1==0 && type2!=0)  //Circle tangent to 1 line and 1 circle/arc
				{
					sds_point u;
					//distance from center of circle to line
					if(-1==ic_ptlndist(p3,p1,p2,&fr1,pt1)) 
						continue;
					if(-1==ic_ptlndist(pt2,p1,p2,&fr2,p7)) 
						continue;
					//calculate the new dia - depends on where pt2 is
					if (fr1 > fr2)
						fr3 = fr1 - p4[0];
					else
						fr3 = fr1 + p4[0];
					ic_unitvec (pt1, p3, u);
					pt2[0] = u[0]*fr3 + pt1[0];
					pt2[1] = u[1]*fr3 + pt1[1];
					pt2[2] = u[2]*fr3 + pt1[2];
					done = TRUE;
				}
				else if (type1!=0 && type2==0)  //Circle tangent to  1 circle/arc and 1 line - same case as above, but reversed
				{
					sds_point u;
					//distance from center of circle to line
					if(-1==ic_ptlndist(p1,p3,p4,&fr1,pt2)) 
						continue;
					if(-1==ic_ptlndist(pt1,p3,p4,&fr2,p7)) 
						continue;
					//calculate the new dia - depends on where pt2 is
					if (fr1 > fr2)
						fr3 = fr1 - p2[0];
					else
						fr3 = fr1 + p2[0];
					ic_unitvec (pt2, p1, u);
					pt1[0] = u[0]*fr3 + pt2[0];
					pt1[1] = u[1]*fr3 + pt2[1];
					pt1[2] = u[2]*fr3 + pt2[2];
					done = TRUE;
				}
				else if (type1!=0 && type2!=0)  //Circle tangent to  2 circles
				{
					//distance between the centers of the two circles
					fr1 = sds_distance(p1,p3);
					fr2 = (fr1 - p2[0] - p4[0]) / 2.0;
					if (fr2 < IC_ZRO) 
						continue;
					sds_point u;
					ic_unitvec (p1, p3, u);

					//Calculate pt1 - there are 2 possible solutions
					p5[0] = p1[0] - u[0]*p2[0];
					p5[1] = p1[1] - u[1]*p2[0];
					p5[2] = p1[2] - u[2]*p2[0];
					p6[0] = p1[0] + u[0]*p2[0];
					p6[1] = p1[1] + u[1]*p2[0];
					p6[2] = p1[2] + u[2]*p2[0];
					fr5 = sds_distance(pt1,p5);
					fr6 = sds_distance(pt1,p6);
					if (fr5 < fr6)
						ic_ptcpy (pt1, p5);
					else
						ic_ptcpy (pt1, p6);

					//Calculate pt2 - there are 2 possible solutions
					p5[0] = p3[0] - u[0]*p4[0];
					p5[1] = p3[1] - u[1]*p4[0];
					p5[2] = p3[2] - u[2]*p4[0];
					p6[0] = p3[0] + u[0]*p4[0];
					p6[1] = p3[1] + u[1]*p4[0];
					p6[2] = p3[2] + u[2]*p4[0];
					fr5 = sds_distance(pt2,p5);
					fr6 = sds_distance(pt2,p6);
					if (fr5 < fr6)
						ic_ptcpy (pt2, p5);
					else
						ic_ptcpy (pt2, p6);
					done = TRUE;
				}
			}
			else
			{
				continue;
			}
		}//for sset2
	}//for sset1

	exit:

	if (ss1[0] != 0L)
		sds_ssfree(ss1);
	if (ss2[0] != 0L)
		sds_ssfree(ss2);
	
	if(done)
		return(RTNORM);
	else
	{
		if(ret!=RTNORM)
			return(ret);
		else	//if ret==RTNORM but we didn't find a geometric solution, return RTNONE
			return(RTNONE);
	}
}


// Note:  In the following four CircTan routines, it is assumed
// that the input has already been checked and sorted so that 
// the input parameters are of the type stated in the header.
// Inside these routines, we will extract the appropriate data
// and hand it to the routine that calculates the result.

// construct a circle tangent to three lines 
static 
int CircTanLLL
(
 line_or_circle lorc1, // assumed to be a line
 line_or_circle lorc2, // assumed to be a line
 line_or_circle lorc3, // assumed to be a line
 sds_point newcen,
 double* newrad
)
{
	int rc;
	sds_point L1A, L1B, picL1;
	sds_point L2A, L2B, picL2;
	sds_point L3A, L3B, picL3;
	
	bool par12 = lorc1.isParallelTo(lorc2);
	bool par13 = lorc1.isParallelTo(lorc3);
	bool par23 = lorc2.isParallelTo(lorc3);
	
	int numParallel = 0;
	if (par12) numParallel++;
	if (par13) numParallel++;
	if (par23) numParallel++;
	
	if (numParallel == 0)
	{  // begin case of no two lines parallel
		lorc1.get_start_pt(L1A);
		lorc1.get_end_pt(L1B);
		lorc1.get_pick_pt(picL1);
		
		lorc2.get_start_pt(L2A);
		lorc2.get_end_pt(L2B);
		lorc2.get_pick_pt(picL2);
		
		lorc3.get_start_pt(L3A);
		lorc3.get_end_pt(L3B);
		lorc3.get_pick_pt(picL3);
		
		rc = CirTan3Lines(L1A, L1B, picL1,
			L2A, L2B, picL2,
			L3A, L3B, picL3,
			newcen, newrad);
		
	}  //   end case of no two lines parallel
	else if (numParallel == 1)
	{  // begin case of one pair of parallel lines
		// swap, if necessary, so that lorc1 and lorc2 are parallel
		// and then call a function to handle it.
		if (par12)
		{  // don't need to do anything here
		}
		else if (par13)
		{
			lorc2.swap(lorc3);
		}
		else if (par23)
		{
			lorc1.swap(lorc3);
		}

		// Now call a special-purpose function to construct the tangent circle
		rc = CircTanParaLLL(lorc1, lorc2, lorc3, newcen, newrad);
                             
	}  //   end case of one pair of parallel lines
	else
	{  // begin case of all lines parallel
		rc = 0; 
	}  //   end case of all lines parallel
	
	return rc;
}

// construct a circle tangent to two lines and a circle
static 
int CircTanLLC
(
 line_or_circle lorcL1, // assumed to be a line
 line_or_circle lorcL2, // assumed to be a line
 line_or_circle lorcC, // assumed to be a circle
 sds_point newcen,
 double* newrad)
{
	sds_point L1A, L1B;
	sds_point L2A, L2B;
	sds_point cen;
	double radius;
	sds_point picL1, picL2, picC;
	bool rc;

	lorcL1.get_start_pt(L1A);
	lorcL1.get_end_pt(L1B);
	lorcL1.get_pick_pt(picL1);

	lorcL2.get_start_pt(L2A);
	lorcL2.get_end_pt(L2B);
	lorcL2.get_pick_pt(picL2);

	lorcC.get_center(cen);
	lorcC.get_radius(radius);
	lorcC.get_pick_pt(picC);


	rc = CirTan2Lines1Cir(L1A, L1B, 
		                  L2A, L2B, 
						  cen, radius, 
						  picL1, picL2, picC, 
						  newcen, newrad);
	return rc;
}

// construct a circle tangent to one line and two circles
static int CircTanLCC(line_or_circle lorcL, // assumed to be a line
					  line_or_circle lorcC1, // assumed to be a circle
					  line_or_circle lorcC2, // assumed to be a circle
					  sds_point newcen,
					  double * newrad)
{
	sds_point cen1, cen2, LA, LB, pikC1, pikC2, pikL;
	double r1, r2;
	bool rc;

	lorcL.get_start_pt(LA);
	lorcL.get_end_pt(LB);
	lorcL.get_pick_pt(pikL);

	lorcC1.get_center(cen1);
	lorcC1.get_radius(r1);
	lorcC1.get_pick_pt(pikC1);

	lorcC2.get_center(cen2);
	lorcC2.get_radius(r2);
	lorcC2.get_pick_pt(pikC2);

	rc = CirTan2Circles1Line(cen1, r1,
		                     cen2, r2,
							 LA, LB,
							 pikC1, pikC2, pikL,
							 newcen, newrad);


	return rc;
}

// construct a circle tangent to three circles
static int CircTanCCC(line_or_circle lorc1, // assumed to be a circle
					  line_or_circle lorc2, // assumed to be a circle
					  line_or_circle lorc3, // assumed to be a circle
					  sds_point newcen,
					  double * newrad)
{
	bool rc = false;

	sds_point cen1, cen2, cen3, pic1, pic2, pic3;
	double r1, r2, r3;

	lorc1.get_center(cen1);
	lorc1.get_radius(r1);
	lorc1.get_pick_pt(pic1);

	lorc2.get_center(cen2);
	lorc2.get_radius(r2);
	lorc2.get_pick_pt(pic2);

	lorc3.get_center(cen3);
	lorc3.get_radius(r3);
	lorc3.get_pick_pt(pic3);

	rc = CirTan3Circles(cen1, r1,
		                cen2, r2,
						cen3, r3,
						pic1, pic2, pic3,
						newcen, newrad);
	return rc;
}

// construct a circle tangent to three lines 
static 
int CircTanLL
(
 sds_point pt, // assumed to be a line
 line_or_circle lorc1, // assumed to be a line
 line_or_circle lorc2, // assumed to be a line
 sds_point newcen,
 double* newrad
)
{
	return CircTan2Lines(pt, lorc1, lorc2, newcen, newrad);
#if 0
	int rc;
	sds_point L1A, L1B, picL1;
	sds_point L2A, L2B, picL2;
	
	bool par12 = lorc1.isParallelTo(lorc2);
	
	int numParallel = 0;
	if (par12) numParallel++;
	
	if (numParallel == 0)
	{  // begin case of no two lines parallel
		lorc1.get_start_pt(L1A);
		lorc1.get_end_pt(L1B);
		lorc1.get_pick_pt(picL1);
		
		lorc2.get_start_pt(L2A);
		lorc2.get_end_pt(L2B);
		lorc2.get_pick_pt(picL2);
#if 0		
		lorc3.get_start_pt(L3A);
		lorc3.get_end_pt(L3B);
		lorc3.get_pick_pt(picL3);
#endif	
		rc = CirTan3Lines(L1A, L1B, picL1,
			L2A, L2B, picL2,
			L3A, L3B, picL3,
			newcen, newrad);
		
	}  //   end case of no two lines parallel
	else if (numParallel == 1)
	{  // begin case of one pair of parallel lines
		// swap, if necessary, so that lorc1 and lorc2 are parallel
		// and then call a function to handle it.
		if (par12)
		{  // don't need to do anything here
		}
		else if (par13)
		{
			lorc2.swap(lorc3);
		}
		else if (par23)
		{
			lorc1.swap(lorc3);
		}

		// Now call a special-purpose function to construct the tangent circle
		rc = CircTanParaLLL(lorc1, lorc2, lorc3, newcen, newrad);
                             
	}  //   end case of one pair of parallel lines
	else
	{  // begin case of all lines parallel
		rc = 0; 
	}  //   end case of all lines parallel
	
	return rc;
#endif
}

static 
int CircTanLC
(
 sds_point pt, // assumed to be a line
 line_or_circle lorc1, // assumed to be a line
 line_or_circle lorc2, // assumed to be a line
 sds_point newcen,
 double* newrad
)
{
	return CircTanLineCircle(pt, lorc1, lorc2, newcen, newrad);
}

static 
int CircTanCC
(
 sds_point pt, // assumed to be a line
 line_or_circle lorc1, // assumed to be a line
 line_or_circle lorc2, // assumed to be a line
 sds_point newcen,
 double* newrad
)
{
	return CircTan2Circles(pt, lorc1, lorc2, newcen, newrad);
}

// very specialized function to count the number of lines and circles
static void Count(line_or_circle & lorc1,
				  line_or_circle & lorc2,
				  line_or_circle & lorc3,
				  int & numLines,
				  int & numCircles)
{
	numLines = 0;
	numCircles = 0;
	
	// we will not use type[0]
	line_or_circle_type type[4];
	lorc1.get_type(type[1]);
	lorc2.get_type(type[2]);
	lorc3.get_type(type[3]);
	for (int i = 1; i <= 3; i++)
	{
		switch(type[i])
		{
			case line:
				numLines++;
				break;
			case arc:
			case circle:
				numCircles++;
		}
	}
}

// very specialized function to count the number of lines and circles
static void Count(line_or_circle & lorc1,
				  line_or_circle & lorc2,
				  int & numLines,
				  int & numCircles)
{
	numLines = 0;
	numCircles = 0;
	
	line_or_circle_type type[2];
	lorc1.get_type(type[0]);
	lorc2.get_type(type[1]);

	for (int i = 0; i < 2; i++)
	{
		if (type[i] == line)
			numLines++;
		else if (type[i] == arc || type[i] == circle)
			numCircles++;
	}
}

// swap the input as needed so that lines will appear first
static LinesFirst(int numLines,
				  line_or_circle & lorc1,
				  line_or_circle & lorc2,
				  line_or_circle & lorc3)
{
	line_or_circle_type t1, t2, t3;
	lorc1.get_type(t1);
	lorc2.get_type(t2);
	lorc3.get_type(t3);

	switch (numLines)
	{
		case 0:
			// nothing to do, it is CCC
			break;
		case 1:
			// find the line and move it to the front if it isn't there already
			if (t2 == line)
				lorc2.swap(lorc1);
			else if(t3 == line)
				lorc3.swap(lorc1);
			break;
		case 2:
			// find the circle and move it to the back if it isn't there already
			if ((t1 == circle) || (t1 == arc))
				lorc1.swap(lorc3);
			else if ((t2 == circle) || (t2 == arc))
				lorc2.swap(lorc3);

			break;
		case 3:
			// nothing to do, it is LLL
			break;
		default:
			// this case should not occur, but if it does, do nothing
			break;
	}
}

// swap the input as needed so that lines will appear first
static LinesFirst(int numLines,
				  line_or_circle & lorc1,
				  line_or_circle & lorc2)
{
	line_or_circle_type t1, t2;
	lorc1.get_type(t1);
	lorc2.get_type(t2);

	switch (numLines)
	{
		case 0:
			// nothing to do, it is CC
			break;
		case 1:
			// find the line and move it to the front if it isn't there already
			if (t2 == line)
				lorc2.swap(lorc1);
			break;
		case 2:
			// nothing to do, it is LLL
			break;
		default:
			// this case should not occur, but if it does, do nothing
			break;
	}
}

// convert the first element in a selection set into two-point form
static int sds_name_to_2pt(const sds_name nmSelSet, // I:  The selection set
						   sds_point pickpt, // I: The pick point
						   sds_point ep1, // O: The 1st point in the two point form
						   sds_point ep2, // O: The 2nd point in the two point form
						   int * type, // O: The type code of the output entity
						   sds_resbuf * rbent // O: The resbuf for the output entity
						   )
{
	long lSetIndex = 0L;
	sds_name nmEntity;
	int retval = sds_ssname(nmSelSet, lSetIndex, nmEntity);
	if (retval==RTNORM)
	{
		retval = cmd_ent_to_2pt(pickpt, nmEntity, ep1, ep2, type, rbent);
	}
	return retval;
}

// this function returns true if p and q point either in the
// same direction or the opposite direction, within tolerance
bool SameOrOpposite(sds_point p, sds_point q)
{
	sds_point r;

	// just check whether all the components of the cross product are near zero.
	ic_crossproduct(p,q,r);
	if ( (fabs(r[0])<IC_ZRO) && (fabs(r[1])<IC_ZRO) && (fabs(r[2])<IC_ZRO) )
		return true;
	else
		return false;
}

// return true only if all the following are true
// 1.  The input data all have types line, arc, or circle
//
// 2.  For arcs and circles, the extrusion vectors must point in the same or
//     opposite direction as the Z axis of the current UCS
//
// 3.  For lines, the lines must all be contained in a plane that is parallel
//     to the XY plane of the current UCS.
bool areValid(int num, // the number of elements in the array
			  line_or_circle * lorc) // the array of elements)
{
	bool valid = true; // innocent until proven guilty
	int i;
	line_or_circle_type * type = new line_or_circle_type[num];

    sds_point UcsZ;
	sds_point extrusion;
    SdsGetUcsZ(UcsZ);

    i = 0;
	while (valid && (i < num))
	{  // begin while
		lorc[i].get_type(type[i]);
		switch (type[i])
		{  // begin switch (type[i])
			case line:
				// check that the line lies in a plane || UCS XY plane
				// which is equivalent to checking that its direction
				// vector is orthogonal to UcsZ
				sds_point A,B,C;
				lorc[i].get_start_pt(A);
				lorc[i].get_end_pt(B);
				ic_sub(A,B,C);
				if (fabs(ic_dotproduct(C,UcsZ)) > IC_ZRO)
					valid = false;
				break;

			case arc:
			case circle:
				lorc[i].get_extrusion(extrusion);
				if (!SameOrOpposite(extrusion,UcsZ))
					valid = false;
				break;

			case other:
			default:
				valid = false;
				break;

		}  //   end switch (type[i])

		i++;
	}  //   end while


    delete [] type;
	return valid;
}

// assuming the circles all have a common extrusion vector
// which is checked outside this routine,
// convert the pick points to the ECS of the circles
void prepCCC(line_or_circle & lorc1, // a circle or circular arc
			 line_or_circle & lorc2, // a circle or circular arc
			 line_or_circle & lorc3) // a circle or circular arc
{
	 sds_point UcsZ;
	 SdsGetUcsZ(UcsZ);

     sds_point picC1, ecspicC1;
	 sds_point picC2, ecspicC2;
	 sds_point picC3, ecspicC3;
	 lorc1.get_pick_pt(picC1);
	 lorc2.get_pick_pt(picC2);
	 lorc3.get_pick_pt(picC3);

	 SdsUcsToEcs(picC1, ecspicC1, UcsZ);
     SdsUcsToEcs(picC2, ecspicC2, UcsZ);
     SdsUcsToEcs(picC3, ecspicC3, UcsZ);

	 lorc1.set_pick_pt(ecspicC1);
	 lorc2.set_pick_pt(ecspicC2);
	 lorc3.set_pick_pt(ecspicC3);
}

void prepLLL(line_or_circle & lorc1,
			 line_or_circle & lorc2,
			 line_or_circle & lorc3)
{
	lorc1.WcsToUcs();
	lorc2.WcsToUcs();
	lorc3.WcsToUcs();
}

void postLLL(line_or_circle lorc, // Input Can be any one of the three
			 sds_point newcen)  // Input and output
{
	sds_point start_pt;
	lorc.get_start_pt(start_pt);
	newcen[2] = start_pt[2];

	sds_point UcsZ;
	SdsGetUcsZ(UcsZ);
	SdsUcsToEcs(newcen, newcen, UcsZ);
}

void prepLLC(line_or_circle & lorc1, 
			 line_or_circle & lorc2, 
			 line_or_circle & lorc3)
{
	lorc1.WcsToUcs();
	lorc2.WcsToUcs();

	// convert the center of the circle to the UCS
	lorc3.EcsToUcs();
}

// does the same as postLLL
void postLLC(line_or_circle lorc,
			 sds_point newcen)
{
	postLLL(lorc, newcen);
}

void prepLCC(line_or_circle & lorc1,
			 line_or_circle & lorc2,
			 line_or_circle & lorc3)
{
	lorc1.WcsToUcs();

	lorc2.EcsToUcs();
	lorc3.EcsToUcs();
}

void postLCC(line_or_circle lorc,
			 sds_point newcen)
{
	postLLL(lorc, newcen);
}

// assuming the circles all have a common extrusion vector
// which is checked outside this routine,
// convert the pick points to the ECS of the circles
#if 1
void prepCC(line_or_circle & lorc1, // a circle or circular arc
			 line_or_circle & lorc2) // a circle or circular arc
{
	 sds_point UcsZ;
	 SdsGetUcsZ(UcsZ);

     sds_point picC1, ecspicC1;
	 sds_point picC2, ecspicC2;

	 lorc1.get_pick_pt(picC1);
	 lorc2.get_pick_pt(picC2);

	 SdsUcsToEcs(picC1, ecspicC1, UcsZ);
     SdsUcsToEcs(picC2, ecspicC2, UcsZ);

	 lorc1.set_pick_pt(ecspicC1);
	 lorc2.set_pick_pt(ecspicC2);
}
#else
void prepCC(line_or_circle & lorc1,
			line_or_circle & lorc2)
{
	lorc1.EcsToUcs();
	lorc2.EcsToUcs();
}
#endif

void prepLL(line_or_circle & lorc1,
			 line_or_circle & lorc2)
{
	lorc1.WcsToUcs();
	lorc2.WcsToUcs();
}

void prepLC(line_or_circle & lorc1, 
			line_or_circle & lorc2)
{
	lorc1.WcsToUcs();

	// convert the center of the circle to the UCS
	lorc2.EcsToUcs();
}

// does the same as postLLL
void postLL(line_or_circle lorc,
			sds_point newcen)
{
	postLLL(lorc, newcen);
}

void postLC(line_or_circle lorc,
			sds_point newcen)
{
	postLLL(lorc, newcen);
}

void postCC(line_or_circle lorc,
			sds_point newcen)
{
	postLLL(lorc, newcen);
}

// the following code will use the line_or_circle class.  This function
// was cloned from cmd_circle3pt_tan_per.
static int cmd_circle3pt_tan_per
(
 sds_point pt1,
 sds_point pt2,
 sds_point pt3,
 const unsigned char tan_per1,
 const unsigned char tan_per2,
 const unsigned char tan_per3,sds_point newcen, double *newrad)
{
	//  pt1, pt2 and pt3 are 3 points on the circumference of a circle. based upon
	//	bit 0 being PER snap and bit 1 being TAN snap, adjust these points for proper
	//	construction of line.  PER has higher priority.
	//NOTEs:	pt1 and pt2 are UCS point
	
	//Returns:  RTNORM if successful geometric sol'n found
	//			RTNONE if no solution found
	//			RTERROR otherwise
	// newcen is the new center of the circle caluclated by this routine
	// newrad is the radius of the new circle.
	
	
	sds_resbuf rb,rbent1,rbent2,rbent3,rbucs;
	sds_real fr1;//fr2,fr3,fr5,fr6; //fr4,
	sds_point p1,p2,p3,p4,p5,p6,lp1,lp2,lp3,p210,screenpt1,screenpt2;
	sds_name ss1,ss2,ss3;
	struct gr_view *view=SDS_CURGRVW;
	int ret=RTNORM,fi1,type1,type2,type3,done=0,reverse_ents=0;
	long fl1;
	
	ic_ptcpy(lp1,pt1);
	ic_ptcpy(lp2,pt2);
	ic_ptcpy(lp3,pt3);
	
	if (view==NULL) return(RTERROR);
	rbucs.restype=RTSHORT;
	rbucs.resval.rint=1;
	
	ss1[0]=ss1[1]=ss2[0]=ss2[1]=0L;
	
	if (SDS_getvar(NULL,
		DB_QUCSXDIR,
		&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
	{
		ret=RTERROR;
		goto exit;
	}
	ic_ptcpy(p1,rb.resval.rpoint);
	if (SDS_getvar(NULL,
		DB_QUCSYDIR,
		&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
	{
		ret=RTERROR;
		goto exit;
	}
	ic_ptcpy(p2,rb.resval.rpoint);

	p210[0]=(p1[1]*p2[2])-(p2[1]*p1[2]);
	p210[1]=(p1[2]*p2[0])-(p2[2]*p1[0]);
	p210[2]=(p1[0]*p2[1])-(p2[0]*p1[1]);
	
	if (SDS_getvar(NULL,
		DB_QVIEWSIZE,
		&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
	{
		ret=RTERROR;
		goto exit;
	}
	fr1=rb.resval.rreal;

	if (SDS_getvar(NULL,
		DB_QSCREENSIZE,
		&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
	{
		ret=RTERROR;
		goto exit;
	}
	fr1/=rb.resval.rpoint[1];

	if (SDS_getvar(NULL,
		DB_QAPERTURE,
		&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
	{
		ret=RTERROR;
		goto exit;
	}
	fr1*=rb.resval.rint;

	for(fi1=0;fi1<3;fi1++)
	{
		if(fi1==0)
		{
			gr_ucs2rp(lp1,p1,view);
			ic_ptcpy(screenpt1,p1);
		}
		else if (fi1==1)
		{
			gr_ucs2rp(lp2,p1,view);
			ic_ptcpy(screenpt2,p1);
		}
		else
		{
			gr_ucs2rp(lp3,p1,view);
			ic_ptcpy(screenpt2,p1);
		}
		p3[0]=p1[0]-fr1;
		p3[1]=p1[1]-fr1;
		p4[0]=p1[0]+fr1;
		p4[1]=p1[1]+fr1;
		p3[2]=p4[2]=0.0;
		gr_rp2ucs(p3,p1,view);
		gr_rp2ucs(p4,p2,view);
		if (0==fi1)
		{
			if ((ret=sds_pmtssget(NULL,"C"/*DNT*/,p1,p2,NULL,ss1,0))!=RTNORM)
			{
				ret=RTNONE;
				goto exit;
			}
			sds_sslength(ss1,&fl1);
		}
		else if (1==fi1) 
		{
			if ((ret=sds_pmtssget(NULL,"C"/*DNT*/,p1,p2,NULL,ss2,0))!=RTNORM)
			{
				ret=RTNONE;
				goto exit;
			}
			sds_sslength(ss2,&fl1);
		}
		else 
		{
			if ((ret=sds_pmtssget(NULL,"C"/*DNT*/,p1,p2,NULL,ss3,0))!=RTNORM)
			{
				ret=RTNONE;
				goto exit;
			}
			sds_sslength(ss3,&fl1);
		}
		if (fl1==0L) 
		{
			ret=RTNONE;
			goto exit;
		}
	}
	int rc1, rc2, rc3;
	
	rc1 = sds_name_to_2pt(ss1, lp1, p1, p2, &type1, &rbent1);
	rc2 = sds_name_to_2pt(ss2, lp2, p3, p4, &type2, &rbent2);
	rc3 = sds_name_to_2pt(ss3, lp3, p5, p6, &type3, &rbent3);
	
	
	if ((rc1==RTNORM) && (rc2==RTNORM) && (rc3==RTNORM) &&
		(tan_per2&2) && (tan_per1&2) && (tan_per3&2))
	{  // begin case of all tan_per values being TAN snap
		
		// convert the data to the line or circle form
		line_or_circle lorc1(p1, p2, type1, lp1, rbent1);
		line_or_circle lorc2(p3, p4, type2, lp2, rbent2);									
		line_or_circle lorc3(p5, p6, type3, lp3, rbent3);
		
		// count the number of line and circle entities
		int numLines;
		int numCircles;

		Count(lorc1, lorc2, lorc3, numLines, numCircles);

		if (numLines + numCircles == 3)
		{
			// first sort so that lines appear first
			LinesFirst(numLines, lorc1, lorc2, lorc3);
			// have this array for convenience in the following
			line_or_circle lorc[3];
			lorc[0] = lorc1;
			lorc[1] = lorc2;
			lorc[2] = lorc3;
			if (areValid(3,lorc))
			{
				switch (numCircles)
				{
					case 0:
						prepLLL(lorc1, lorc2, lorc3);
						done = CircTanLLL(lorc1, lorc2, lorc3, newcen, newrad);
						if (done)
							postLLL(lorc1, newcen);
						break;
					case 1:
						prepLLC(lorc1, lorc2, lorc3);
						done = CircTanLLC(lorc1, lorc2, lorc3, newcen, newrad);
						if (done)
						   postLLC(lorc1, newcen);
						break;
					case 2:
						prepLCC(lorc1, lorc2, lorc3);
						done = CircTanLCC(lorc1, lorc2, lorc3, newcen, newrad);
						if (done)
							postLCC(lorc1, newcen);
						break;
					case 3:
						prepCCC(lorc1,lorc2,lorc3);
						done = CircTanCCC(lorc1, lorc2, lorc3, newcen, newrad);
						break;
					default:
						done = FALSE;
				}
			}
		}
	}   //  end case of all tan_per values being TAN snap
				
exit:
		if(ss1[0]!=0L)sds_ssfree(ss1);
		if(ss2[0]!=0L)sds_ssfree(ss2);
		if(ss3[0]!=0L)sds_ssfree(ss3);
		if(done)
			return(RTNORM);
		else
		{
			if(ret!=RTNORM)
				return(ret);
			else	//if ret==RTNORM but we didn't find a geometric solution, return RTNONE
				return(RTNONE);
		}
}

static int cmd_circle3pt_tan_per
(
 sds_point pt1,
 sds_point pt2,
 sds_point pt3,
 const unsigned char tan_per2,
 const unsigned char tan_per3,
 sds_point newcen, double *newrad)
{
	//  pt1, pt2 and pt3 are 3 points on the circumference of a circle. based upon
	//	bit 0 being PER snap and bit 1 being TAN snap, adjust these points for proper
	//	construction of line.  PER has higher priority.
	//NOTEs:	pt1 and pt2 are UCS point
	
	//Returns:  RTNORM if successful geometric sol'n found
	//			RTNONE if no solution found
	//			RTERROR otherwise
	// newcen is the new center of the circle caluclated by this routine
	// newrad is the radius of the new circle.
	
	
	sds_resbuf rb,rbent2,rbent3,rbucs;
	sds_real fr1;//fr2,fr3,fr5,fr6; //fr4,
	sds_point p1,p2,p3,p4,p5,p6,lp1,lp2,lp3,p210,screenpt2;
	sds_name ss1,ss2,ss3;
	struct gr_view *view=SDS_CURGRVW;
	int ret=RTNORM,fi1,type2,type3,done=0,reverse_ents=0;
	long fl1;
	
	ic_ptcpy(lp1,pt1);
	ic_ptcpy(lp2,pt2);
	ic_ptcpy(lp3,pt3);
	
	if (view==NULL) return(RTERROR);
	rbucs.restype=RTSHORT;
	rbucs.resval.rint=1;
	
	ss1[0]=ss1[1]=ss2[0]=ss2[1]=0L;
	
	if (SDS_getvar(NULL,
		DB_QUCSXDIR,
		&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
	{
		ret=RTERROR;
		goto exit;
	}
	ic_ptcpy(p1,rb.resval.rpoint);
	if (SDS_getvar(NULL,
		DB_QUCSYDIR,
		&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
	{
		ret=RTERROR;
		goto exit;
	}
	ic_ptcpy(p2,rb.resval.rpoint);

	p210[0]=(p1[1]*p2[2])-(p2[1]*p1[2]);
	p210[1]=(p1[2]*p2[0])-(p2[2]*p1[0]);
	p210[2]=(p1[0]*p2[1])-(p2[0]*p1[1]);
	
	if (SDS_getvar(NULL,
		DB_QVIEWSIZE,
		&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
	{
		ret=RTERROR;
		goto exit;
	}
	fr1=rb.resval.rreal;

	if (SDS_getvar(NULL,
		DB_QSCREENSIZE,
		&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
	{
		ret=RTERROR;
		goto exit;
	}
	fr1/=rb.resval.rpoint[1];

	if (SDS_getvar(NULL,
		DB_QAPERTURE,
		&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
	{
		ret=RTERROR;
		goto exit;
	}
	fr1*=rb.resval.rint;

	for(fi1=1;fi1<3;fi1++)
	{
		if (fi1==1)
		{
			gr_ucs2rp(lp2,p1,view);
			ic_ptcpy(screenpt2,p1);
		}
		else
		{
			gr_ucs2rp(lp3,p1,view);
			ic_ptcpy(screenpt2,p1);
		}
		p3[0]=p1[0]-fr1;
		p3[1]=p1[1]-fr1;
		p4[0]=p1[0]+fr1;
		p4[1]=p1[1]+fr1;
		p3[2]=p4[2]=0.0;
		gr_rp2ucs(p3,p1,view);
		gr_rp2ucs(p4,p2,view);
		
		if (1==fi1)
		{
			if ((ret=sds_pmtssget(NULL,"C"/*DNT*/,p1,p2,NULL,ss2,0))!=RTNORM)
			{
				ret=RTNONE;
				goto exit;
			}
			sds_sslength(ss2,&fl1);
		}
		else
		{
			if ((ret=sds_pmtssget(NULL,"C"/*DNT*/,p1,p2,NULL,ss3,0))!=RTNORM)
			{
				ret=RTNONE;
				goto exit;
			}
			sds_sslength(ss3,&fl1);
		}
		if (fl1==0L) 
		{
			ret=RTNONE;
			goto exit;
		}
	}

	int rc2, rc3;
	rc2=sds_name_to_2pt(ss2, lp2, p3, p4, &type2, &rbent2);
	rc3=sds_name_to_2pt(ss3, lp3, p5, p6, &type3, &rbent3);

	if ((rc2==RTNORM) && (rc3==RTNORM) &&
		(tan_per2&2) && (tan_per3&2))
	{  // begin case of all tan_per values being TAN snap
		
		// convert the data to the line or circle form
		line_or_circle lorc2(p3, p4, type2, lp2, rbent2);									
		line_or_circle lorc3(p5, p6, type3, lp3, rbent3);
		
		// count the number of line and circle entities
		int numLines;
		int numCircles;

		Count(lorc2, lorc3, numLines, numCircles);

		if (numLines + numCircles == 2)
		{
			// first sort so that lines appear first
			LinesFirst(numLines, lorc2, lorc3);
			// have this array for convenience in the following
			line_or_circle lorc[2];
			lorc[0] = lorc2;
			lorc[1] = lorc3;
			if (areValid(2,lorc))
			{
				switch (numCircles)
				{
					case 0:
						prepLL(lorc2, lorc3);
						done = CircTanLL(pt1, lorc2, lorc3, newcen, newrad);
						if (done)
							postLL(lorc2, newcen);
						break;
					case 1:
						prepLC(lorc2, lorc3);
						done = CircTanLC(pt1, lorc2, lorc3, newcen, newrad);
						if (done)
						   postLC(lorc2, newcen);
						break;
					case 2:
						prepCC(lorc2, lorc3);
						done = CircTanCC(pt1, lorc2, lorc3, newcen, newrad);
						break;
					default:
						done = FALSE;
				}
			}
		}
	}   //  end case of all tan_per values being TAN snap
				
exit:
		if(ss2[0]!=0L)sds_ssfree(ss2);
		if(ss3[0]!=0L)sds_ssfree(ss3);
		if(done)
			return(RTNORM);
		else
		{
			if(ret!=RTNORM)
				return(ret);
			else	//if ret==RTNORM but we didn't find a geometric solution, return RTNONE
				return(RTNONE);
		}
}

static int 
cmd_linerayxlineray(const sds_point p0,const sds_point p1, int type1,
                        const sds_point p2,const sds_point p3, int type2,
                        sds_point xpt) 
{
	//basically, this is just ic_linexline except that the lines may be xlines
	// or rays.  returns same as ic_linexline.
	//For rays and xlines, the 11 pt is vector converted to point

	int ret;

	ret=ic_linexline(p0,p1,p2,p3,xpt);
	if(ret==-1)return(ret);
	if(type1==2) ret|=1;
	if(type2==2) ret|=2;
	if(ret==3)return(ret);

	int dim1,dim2;

	if(type1==1)
	{
		if(fabs(p1[0]-p0[0])>fabs(p1[1]-p1[1]))
		{
			if(fabs(p1[0]-p0[0])>fabs(p1[2]-p1[2]))
				dim1=0;
			else 
				dim1=2;
		}
		else
		{
			if(fabs(p1[1]-p0[1])>fabs(p1[2]-p1[2]))
				dim1=1;
			else 
				dim1=2;
		}
		if ((p1[dim1]-p0[dim1])*(xpt[dim1]-p0[dim1])>=0.0)
			ret|=1;
		if (ret==3)
			return(ret);
	}
	if (type2==1)
	{
		if (fabs(p3[0]-p2[0])>fabs(p3[1]-p3[1]))
		{
			if(fabs(p3[0]-p2[0])>fabs(p3[2]-p3[2]))
				dim2=0;
			else
				dim2=2;
		}
		else
		{
			if (fabs(p3[1]-p2[1])>fabs(p3[2]-p3[2]))
				dim2=1;
			else
				dim2=2;
		}
		if ((p3[dim1]-p2[dim1])*(xpt[dim1]-p2[dim1])>=0.0)
			ret|=2;
	}
	return(ret);
}

short getplinettrdata(sds_point ptsel1, sds_name ent1, resbuf *elist1, resbuf *rbwcs, resbuf *rbucs, resbuf *rbnew,
	short *type1, sds_point p1, sds_point p2)
{
	short plineflag;
	sds_name etemp1,etemp2;
	long fl1, fl2;
	resbuf *rbp1=NULL;
	sds_real bulge;
	int exactvert;
	sds_point vertpt1,vertpt2;

	ic_assoc(elist1,70);
	plineflag=ic_rbassoc->resval.rint;
	if (plineflag & (IC_PLINE_POLYFACEMESH | IC_PLINE_3DMESH))
		return(RTERROR);

	fl1=cmd_pl_vtx_no(ent1,ptsel1,1,&exactvert);
	if (sds_entnext_noxref(ent1,etemp1)!=RTNORM)
		return(RTERROR);

	for (fl2=1; fl2<fl1; fl2++)
	{
		if (sds_entnext_noxref(etemp1,etemp1)!=RTNORM)
			return(RTERROR);
	}
	if ((rbp1=sds_entget(etemp1))==NULL)
		return(RTERROR);

	ic_assoc(rbp1,42);
	bulge=ic_rbassoc->resval.rreal;
	ic_assoc(rbp1,10);
	if (fabs(bulge)<CMD_FUZZ)
	{
		*type1=0;
		if(plineflag & IC_PLINE_3DPLINE)
			sds_trans(ic_rbassoc->resval.rpoint,rbwcs,rbucs,0,p1);
		else
			sds_trans(ic_rbassoc->resval.rpoint,rbnew,rbucs,0,p1);
	}
	else
	{
		*type1=3;
		ic_ptcpy(vertpt1,ic_rbassoc->resval.rpoint);
	}
		FreeResbufIfNotNull(&rbp1);
	cmd_pline_entnext(etemp1,etemp2,plineflag);
	if ((rbp1=sds_entget(etemp2))==NULL)
		return(RTERROR);
	ic_assoc(rbp1,10);
	ic_ptcpy(vertpt2,ic_rbassoc->resval.rpoint);
	if((*type1)==0)
	{
		if(plineflag & IC_PLINE_3DPLINE)
			sds_trans(vertpt2,rbwcs,rbucs,0,p2);
		else
			sds_trans(vertpt2,rbnew,rbucs,0,p2);
	}
	else
		ic_bulge2arc(vertpt1,vertpt2,bulge,p1,p2,&p2[1],&p2[2]);
	return RTNORM;
}


short getlwplinettrdata(sds_point ptsel1, sds_name ent1, resbuf *rbucs, resbuf *rbnew,
	short *type1, sds_point p1, sds_point p2)
{
	long fl1, fl2;
	sds_real bulge;
	int exactvert;
	sds_point vertpt1,vertpt2;
	db_lwpolyline *lwpl=(db_lwpolyline *)ent1[0];
	sds_point tpt;

	fl1=cmd_lwpl_vtx_no(ent1,ptsel1,1,&exactvert);
	lwpl->get_42(&bulge,fl1);
	lwpl->get_10(tpt,fl1);
	if (fabs(bulge)<CMD_FUZZ)
	{
		*type1=0;
		sds_trans(tpt,rbnew,rbucs,0,p1);
	}
	else
	{
		*type1=3;
		ic_ptcpy(vertpt1,tpt);
	}
	fl2=cmd_lwpline_entnext(fl1,lwpl);
	lwpl->get_10(vertpt2,fl2);
	if((*type1)==0)
		sds_trans(vertpt2,rbnew,rbucs,0,p2);
	else
		ic_bulge2arc(vertpt1,vertpt2,bulge,p1,p2,&p2[1],&p2[2]);
	return RTNORM;
}


// Find the total distance between the tangent points and picked points
//
static double
findTanPt2PickPtDis(sds_point center1, 
					sds_real radius1,
					sds_point center2,		// the center of candidate circle
					sds_real radius2,		// the radius of candidate circle
					sds_point pickedPoint)	// the pick point
{
	// compute the distance between the tangent point and the picked point.
	//
	sds_point tanPoint;
	tanPoint[2] = pickedPoint[2];
	double dis = sds_distance(center1,center2);
	ASSERT(!icadIsZero(dis));

	if (dis < radius2 && radius2 > radius1) { // the circle1 is inside the circle2
		dis = radius2 / dis;
		tanPoint[0] = center2[0] + dis * (center1[0] - center2[0]);
		tanPoint[1] = center2[1] + dis * (center1[1] - center2[1]);
	}
	else { // the circle2 is inside or outside the circle1
		dis = radius1 / dis;
		tanPoint[0] = center1[0] + dis * (center2[0] - center1[0]);
		tanPoint[1] = center1[1] + dis * (center2[1] - center1[1]);
	}
	return sds_distance(pickedPoint,tanPoint);
}

// ************************
// HELPER FUNCTION
//
//***********support for circle f'n****************************
static int 
cmd_circle_ttr(sds_point ptsel1, sds_name ent1,struct resbuf *elist1,
					 sds_point ptsel2, sds_name ent2,struct resbuf *elist2,sds_real rr,sds_point cc)
{
	//Given elists elist1 & elist2 for entities ent1 and ent2, calcs cc for a tangent circle.
	//Ents were selected at pts p1 & p2.  All ents must be LINE,TRACE,ARC,
	//	2DPOLYLINE,or CIRCLE.  Calling f'n must perform checks on etype,
	//	and make the circle.  All ecs ents must have extru parallel to current ucs.
	//Store new center of circle in cc
	//NOTE: pts returned in ecs for the new arc.  The calling f'n must assure
	//	that all ents are wcs or that their ecs is parallel to current ucs.

	//Break down into 3 Cases:  Line-Line, Line-Arc/Circ, and Arc/Circ-Arc/Circ
	//Plines are either line or arc (segment chosen).  Traces are lines.

	char etype1[IC_ACADNMLEN],etype2[IC_ACADNMLEN];
	sds_point p1,p2,p3,p4,p5,p6,p7,p8,p9,cp,pickpt1,pickpt2;
	long fl1,fl2;
	short fi1,type1,type2;	 //type=0 for line-type ent, =1 for ray, =2 for xline, =3 for arc-type ent, =4 for circle
	struct resbuf *rbp1=NULL,rbnew,rbwcs,rbucs;
	sds_real fr1,fr2,dist,dis1,dis2,diff,elevation;

	SDS_getvar(NULL,DB_QELEVATION,&rbwcs,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	elevation=rbwcs.resval.rreal;
	SDS_getvar(NULL,DB_QUCSXDIR,&rbwcs,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	ic_ptcpy(p1,rbwcs.resval.rpoint);
	SDS_getvar(NULL,DB_QUCSYDIR,&rbwcs,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	//cross the two to get extrusion direction for new line
	rbnew.resval.rpoint[0]=(p1[1]*rbwcs.resval.rpoint[2])-(p1[2]*rbwcs.resval.rpoint[1]);
	rbnew.resval.rpoint[1]=(p1[2]*rbwcs.resval.rpoint[0])-(p1[0]*rbwcs.resval.rpoint[2]);
	rbnew.resval.rpoint[2]=(p1[0]*rbwcs.resval.rpoint[1])-(p1[1]*rbwcs.resval.rpoint[0]);
	rbnew.restype=RT3DPOINT;
	rbwcs.restype=rbucs.restype=RTSHORT;
	rbwcs.resval.rint=0;
	rbucs.resval.rint=1;

	p5[0]=p5[1]=0.0;
	p5[2]=elevation;
	sds_trans(p5,&rbucs,&rbnew,0,p5);
	elevation=p5[2];


	ic_assoc(elist1,0);
	strncpy(etype1,ic_rbassoc->resval.rstring,sizeof(etype1)-1);
	ic_assoc(elist2,0);
	strncpy(etype2,ic_rbassoc->resval.rstring,sizeof(etype2)-1);

	//just collect data in standard 2-point format (pt-pt or pt-(rr,sa,ea))
	//we already know that ecs ents have extru parallel to current ucs
	//convert all point data for lines to UCS, for arcs leave in ECS
	if (strsame(etype1,"POLYLINE"/*DNT*/))
		if (getplinettrdata(ptsel1,ent1,elist1,&rbwcs, &rbucs, &rbnew, &type1,p1,p2)==RTERROR)
			return RTERROR;

	if (strsame(etype2,"POLYLINE"/*DNT*/))
		if (getplinettrdata(ptsel2,ent2,elist2,&rbwcs, &rbucs, &rbnew, &type2,p3,p4)==RTERROR)
			return RTERROR;

	if (strsame(etype1,"LWPOLYLINE"/*DNT*/))
		if (getlwplinettrdata(ptsel1,ent1,&rbucs, &rbnew, &type1,p1,p2)==RTERROR)
			return RTERROR;

	if (strsame(etype2,"LWPOLYLINE"/*DNT*/))
		if (getlwplinettrdata(ptsel2,ent2, &rbucs, &rbnew, &type2,p3,p4)==RTERROR)
			return RTERROR;

	if ((strsame(etype1,"ARC"/*DNT*/))||(strsame(etype1,"CIRCLE"/*DNT*/)))
	{
		ic_assoc(elist1,10);
		ic_ptcpy(p1,ic_rbassoc->resval.rpoint);
		ic_assoc(elist1,40);
		p2[0]=ic_rbassoc->resval.rreal;
		if(strsame(etype1,"ARC"/*DNT*/))
		{
			type1=3;
			ic_assoc(elist1,50);
			p2[1]=ic_rbassoc->resval.rreal;
			ic_assoc(elist1,51);
			p2[2]=ic_rbassoc->resval.rreal;
		}
		else
		{
			type1=4;
			p2[1]=0.0;
			p2[2]=IC_TWOPI;
		}
	}
	if((strsame(etype2,"ARC"/*DNT*/))||(strsame(etype2,"CIRCLE"/*DNT*/)))
	{
		ic_assoc(elist2,10);
		ic_ptcpy(p3,ic_rbassoc->resval.rpoint);
		ic_assoc(elist2,40);
		p4[0]=ic_rbassoc->resval.rreal;
		if(strsame(etype2,"ARC"/*DNT*/))
		{
			type2=3;
			ic_assoc(elist2,50);
			p4[1]=ic_rbassoc->resval.rreal;
			ic_assoc(elist2,51);
			p4[2]=ic_rbassoc->resval.rreal;
		}
		else
		{
			type2=4;
			p4[1]=0.0;
			p4[2]=IC_TWOPI;
		}
	}
	if(RTNORM==sds_wcmatch(etype1,"LINE,TRACE,XLINE,RAY"/*DNT*/))
	{
		type1=0;
		ic_assoc(elist1,10);
		ic_ptcpy(p1,ic_rbassoc->resval.rpoint);
		//on a trace, 10&11 are starting pts, 12&13 ending pts
		if(strsame(etype1,"TRACE"/*DNT*/))
		{
			ic_assoc(elist1,11);
			p1[0]=0.5*(p1[0]+ic_rbassoc->resval.rpoint[0]);
			p1[1]=0.5*(p1[1]+ic_rbassoc->resval.rpoint[1]);
			p1[2]=0.5*(p1[2]+ic_rbassoc->resval.rpoint[2]);
			ic_assoc(elist1,12);
		}
		else
		{
			ic_assoc(elist1,11);
		}
		ic_ptcpy(p2,ic_rbassoc->resval.rpoint);
		if(strsame(etype1,"TRACE"/*DNT*/))
		{
			ic_assoc(elist1,13);
			p2[0]=0.5*(p2[0]+ic_rbassoc->resval.rpoint[0]);
			p2[1]=0.5*(p2[1]+ic_rbassoc->resval.rpoint[1]);
			p2[2]=0.5*(p2[2]+ic_rbassoc->resval.rpoint[2]);
		}
		if(strsame(etype1,"RAY"/*DNT*/) || strsame(etype1,"XLINE"/*DNT*/))
		{
			if(strsame(etype1,"RAY"/*DNT*/))type1=1;
			else type1=2;
			p2[0]+=p1[0];
			p2[1]+=p1[1];
			p2[2]+=p1[2];
		}
		if (!strsame(etype1,"TRACE"/*DNT*/))
		{	//all of these except trace are wcs...
			sds_trans(p1,&rbwcs,&rbucs,0,p1);
			sds_trans(p2,&rbwcs,&rbucs,0,p2);
		}
		else
		{
			sds_trans(p1,&rbnew,&rbucs,0,p1);
			sds_trans(p2,&rbnew,&rbucs,0,p2);
		}
	}
	if(RTNORM==sds_wcmatch(etype2,"LINE,TRACE,XLINE,RAY"/*DNT*/))
	{
		type2=0;
		ic_assoc(elist2,10);
		ic_ptcpy(p3,ic_rbassoc->resval.rpoint);
		//on a trace, 10&11 are starting pts, 12&13 ending pts
		if(strsame(etype2,"TRACE"/*DNT*/))
		{
			ic_assoc(elist2,11);
			p3[0]=0.5*(p3[0]+ic_rbassoc->resval.rpoint[0]);
			p3[1]=0.5*(p3[1]+ic_rbassoc->resval.rpoint[1]);
			p3[2]=0.5*(p3[2]+ic_rbassoc->resval.rpoint[2]);
			ic_assoc(elist2,12);
		}
		else
		{
			ic_assoc(elist2,11);
		}
		ic_ptcpy(p4,ic_rbassoc->resval.rpoint);
		if(strsame(etype2,"TRACE"/*DNT*/))
		{
			ic_assoc(elist2,13);
			p4[0]=0.5*(p4[0]+ic_rbassoc->resval.rpoint[0]);
			p4[1]=0.5*(p4[1]+ic_rbassoc->resval.rpoint[1]);
			p4[2]=0.5*(p4[2]+ic_rbassoc->resval.rpoint[2]);
		}
		if(strsame(etype2,"RAY"/*DNT*/) || strsame(etype2,"XLINE"/*DNT*/))
		{
			if(strsame(etype2,"RAY"/*DNT*/))
				type2=1;
			else
				type2=2;
			p4[0]+=p3[0];
			p4[1]+=p3[1];
			p4[2]+=p3[2];
		}
		if(!strsame(etype2,"TRACE"/*DNT*/))
		{	//all of these except trace are wcs...
			sds_trans(p3,&rbwcs,&rbucs,0,p3);
			sds_trans(p4,&rbwcs,&rbucs,0,p4);
		}
		else
		{
			sds_trans(p3,&rbnew,&rbucs,0,p3);
			sds_trans(p4,&rbnew,&rbucs,0,p4);
		}
	}
	//Now we've collected all the data we need about the entities - do the calcs

	diff=dist=-1.0;
	if(type1<3 && type2<3)
	{
		//all line pts are in ucs coords
		//check to see that the lines are all planar in current ucs
		if(!icadRealEqual(p1[2],p2[2]) || !icadRealEqual(p3[2],p4[2]) ||
			!icadRealEqual(p1[2],p3[2]))
			goto exit;
		sds_trans(p1,&rbucs,&rbnew,0,p5);
		elevation=p5[2];//get elevation for circle
	}
	if(type1<3)
		ic_ptlndist(ptsel1,p1,p2,&fr1,ptsel1);
	if(type2<3)
		ic_ptlndist(ptsel2,p3,p4,&fr1,ptsel2);
	if((type1<3)&&(type2<3))
	{
		if(!cmd_offset_linepts(p1,p2,rr,p5,p6) && !cmd_offset_linepts(p3,p4,rr,p7,p8))
		{
			setCenterPoint(p5,p6,p7,p8,&cp,&dist,ptsel1,ptsel2);
			cmd_offset_linepts(p1,p2,-rr,p5,p6);
			setCenterPoint(p5,p6,p7,p8,&cp,&dist,ptsel1,ptsel2);
			cmd_offset_linepts(p3,p4,-rr,p7,p8);
			setCenterPoint(p5,p6,p7,p8,&cp,&dist,ptsel1,ptsel2);
			cmd_offset_linepts(p1,p2,rr,p5,p6);
			setCenterPoint(p5,p6,p7,p8,&cp,&dist,ptsel1,ptsel2);
			sds_trans(cp,&rbucs,&rbnew,0,cp);
		}
	}
	else if((type1<3)&&(type2>2)||
			 ((type1>2)  && (type2<3)))
	{
		//line & arc/circle
		if(type1>2)
		{	//make it so p1&p2 define line
			ic_ptcpy(p5,p1);
			ic_ptcpy(p6,p2);
			ic_ptcpy(p1,p3);
			ic_ptcpy(p2,p4);
			ic_ptcpy(p3,p5);
			ic_ptcpy(p4,p6);
			fi1=type2;
			type2=type1;
			type1=fi1;
		}
		rbwcs.resval.rint=1;
		sds_trans(ptsel1,&rbwcs,&rbnew,0,pickpt1);//ucs->ecs
		rbwcs.resval.rint=0;
		elevation=p3[2];
		fl2=1;
		while(1)
		{
			if(cmd_offset_linepts(p1,p2,(rr*fl2),p5,p6)==0)
			{
				sds_trans(p5,&rbucs,&rbnew,0,p5);
				sds_trans(p6,&rbucs,&rbnew,0,p6);
				fl1=ic_linexarc(p5,p6,p3,(p4[0]+rr),p4[1],p4[2],p7,p8);
				if(fl1!=-1)
				{
					if(type1==2)
						fl1|=5;
					else
						if(type1==1)
						{
							if(icadDirectionEqual(p5,p6,p5,p7))
								fi1|=1;
							if(icadDirectionEqual(p5,p6,p5,p8))
								fi1|=4;
						}
					if(((type2==4)&&(fl1&1))||((type2==3)&&((fl1&3)==3)))
					{
						fr1=sds_distance(p7,pickpt1);
						if((dist==-1.0)||(fr1<dist))
						{
							ic_ptcpy(cp,p7);
							dist=fr1;
						}
					}
					if(((type2==4)&&(fl1&4))||((type2==3)&&((fl1&12)==12)))
					{
						fr1=sds_distance(p8,pickpt1);
						if((dist==-1.0)||(fr1<dist))
						{
							ic_ptcpy(cp,p8);
							dist=fr1;
						}
					}
				}
				if(p4[0]>rr)
				{
					fl1=ic_linexarc(p5,p6,p3,(p4[0]-rr),p4[1],p4[2],p7,p8);
					if(fl1!=-1)
					{
						if(type1==2)fl1|=5;
						else if(type1==1)
						{
							if(icadDirectionEqual(p5,p6,p5,p7))fi1|=1;
							if(icadDirectionEqual(p5,p6,p5,p8))fi1|=4;
						}
						if(((type2==4)&&(fl1&1))||((type2==3)&&((fl1&3)==3)))
						{
							fr1=findTanPt2PickPtDis(p3,p4[0],p7,rr,pickpt1);
							if((dist==-1.0)||(fr1<dist))
							{
								ic_ptcpy(cp,p7);
								dist=fr1;
							}
						}
						if(((type2==4)&&(fl1&4))||((type2==3)&&((fl1&12)==12)))
						{
							fr1=findTanPt2PickPtDis(p3,p4[0],p8,rr,pickpt1);
							if((dist==-1.0)||(fr1<dist))
							{
								ic_ptcpy(cp,p8);
								dist=fr1;
							}
						}
					}
				}
			}
			if(fl2==1)
				fl2=-1;
			else
				break;
		}
	}
	else if(type1>2 && type2>2)
	{
		//two arcs
		sds_trans(ptsel1,&rbucs,&rbnew,0,pickpt1);
		sds_trans(ptsel2,&rbucs,&rbnew,0,pickpt2);
		double centerDistance = sds_distance(p1, p3);
		bool disjoint = (centerDistance - p2[0] - p4[0] > 0.0) ? true : false;
		for(fl2=1;fl2<6;fl2++)
		{
			if(fl2==1) // the candidate circle is outside any selected circle
			{
				p8[0]=p2[0]+rr;
				p9[0]=p4[0]+rr;
			}
			else if(fl2==2) // the first circle is inside or enclose the candidate circle
			{
				if (disjoint) { 
					if (rr < p2[0])
						continue;
					p8[0]=rr-p2[0];
				}
				else {
					if(p2[0]<rr)
						continue;
					p8[0]=p2[0]-rr;
				}
				p9[0]=p4[0]+rr;    // this is not necessary if the order is not changed.
			}
			else if(fl2==3) // the second circle is inside or enclose the candidate circle
			{
				if (disjoint) {
					if (rr < p4[0])
						continue;
					p9[0]=rr-p4[0];
				}
				else {
					if(p4[0]<rr)
						continue;
					p9[0]=p4[0]-rr;
				}
				p8[0]=p2[0]+rr;
			}
			else if(fl2==4) // the candidate circle encloses both the selected circles.
			{
				if (rr > p2[0] && rr > p4[0]) {
					p8[0] = rr - p2[0];
					p9[0] = rr - p4[0];
				}
			}
			else if(fl2==5) { // the candidate circle is enclosed by both the circles
				if (!disjoint && p2[0] > rr && p4[0] > rr) {
					p8[0] = p2[0] - rr;
					p9[0] = p4[0] - rr;
				}
			}
			if(1==fl2)
				elevation=p1[2];
			fl1=ic_arcxarc(p1,p8[0],p2[1],p2[2],p3,p9[0],p4[1],p4[2],p5,p6);
			if(fl1!=-1)
			{
				if((((type1==3)&&(fl1&1))||(type1==4))&&
				   (((type2==3)&&(fl1&2))||(type2==4)))
				{
					// compute the total distance between the tangent points and the picked 
					// points and use it as the criteria to find a pair of tangent points that 
					// are closest to the select points. If the total distance is identical, 
					// we compare the variation of the distances to pick one with minimum value. 
					// Since two tangent points with radius doesn't uniquely define a 
					// circle, it can still have more than one solutions in the certain cases
					// (i.e. a large circle tangent to two smaller circles). In those cases, 
					// we just use the first center point returned to define the circle..
					//
					dis1 = findTanPt2PickPtDis(p1, p2[0], p5, rr, pickpt1);
					dis2 = findTanPt2PickPtDis(p3, p4[0], p5, rr, pickpt2);
					fr1 = dis1 + dis2;
					fr2 = fabs(dis1 - dis2);
					if((dist==-1.0)||(fr1<dist))
					{
						ic_ptcpy(cp,p5);
						dist=fr1;
					}
					else if (icadRealEqual(fr1, dist)) {
						if (diff==-1.0||(fr2<diff)) { 
							ic_ptcpy(cp,p5);
							diff=fr2;
						}
					}
				}
				if (!icadPointEqual(p5, p6)) { 
					if((((type1==3)&&(fl1&4))||(type1==4))&&
					   (((type2==3)&&(fl1&8))||(type2==4)))
					{
						// compute the total distance between the tangent points and the picked 
						// points and use it as the criteria to find a pair of tangent points that 
						// are closest to the select points. If the total distance is identical, 
						// we compare the variation of the distances to pick one with minimum value. 
						// Since two tangent points with radius doesn't uniquely define a 
						// circle, it can still have more than one solutions in the certain cases
						// (i.e. a large circle tangent to two smaller circles). In those cases, 
						// we just use the first center point returned to define the circle..
						//
						dis1 = findTanPt2PickPtDis(p1, p2[0], p6, rr, pickpt1);
						dis2 = findTanPt2PickPtDis(p3, p4[0], p6, rr, pickpt2);
						fr1 = dis1 + dis2;
						fr2 = fabs(dis1 - dis2);
						if((dist==-1.0)||(fr1<dist))
						{
							ic_ptcpy(cp,p6);
							dist=fr1;
						}
						else if (icadRealEqual(fr1, dist)) {
							if (diff==-1.0||(fr2<diff)) { 
								ic_ptcpy(cp,p6);
								diff=fr2;
							}
						}
					}
				}
			}
		}
	}
	exit:
	//now if we got a point & distance, make the circle
	if(rbp1!=NULL)
	{
		sds_relrb(rbp1);rbp1=NULL;
	}
	if(dist!=-1.0) // dist was initialized to -1.0
	{
		// the total distance between pick points and tangent points are 
		// not equal to -1.0, which means  a candidate circle has been 
		// found.
        //
		ic_ptcpy(cc,cp);
		cc[2]=elevation;
		return(RTNORM);
	}
	else
	{
		cmd_ErrorPrompt(CMD_ERR_NOCIRCLE,0);
		return(RTERROR);
	}
}

// *******************
// COMMAND FUNCTION
//

short cmd_circle(void) 
{
//*** This function draws a circle of any size.
//***
//***  2P - Specifies size of circle by 2 endpoints of diameter.
//***  3P - Specifies size of circle by 3 points on circumference.
//***   D - Enters diameter instead of radius.
//***   R - Enters radius (the default)
//***   A - Closes an Arc into a circle.
//*** TTR/RadTanTan - Specifies size of circle by two tangent points and radius.
//***		NOTE: Icad's prompting order differs from Acad's.  We support both.
//***
//***
//*** RETURNS:  RTNORM - Success.
//***            RTCAN - User cancel.
//***          RTERROR - RTERROR returned from an ads function.
//***

    if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    char fs1[IC_ACADBUF],fs2[IC_ACADBUF];
    struct resbuf setgetrb,*rslt,*newent,*elist1,*elist2,rb,rblast,rbucs,rbwcs,rbecs;
	struct resbuf *rbtemp;
    sds_point pt1,pt2,pt3,pt4,pt210;
	sds_name ent1,ent2;
    sds_real curelev,defrad,newrad,newelev, sa, ea;
    short ret;
	int got1,ttr,repeatmode=0;
    long rc=0L;
	db_handitem *elp=NULL;
	signed char tan_per1, tan_per2, tan_per3;
	bool bBasic3PointCircle;

	elist1=elist2=newent=rslt=NULL;
	//we need circle's extrusion direction before we can make it....
    if (SDS_getvar(NULL,
		DB_QUCSXDIR,
		&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
	{
		rc=(-__LINE__);
		goto bail; 
	}
	ic_ptcpy(pt1,setgetrb.resval.rpoint);
    
	if (SDS_getvar(NULL,
		DB_QUCSYDIR,
		&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
	{
		rc=(-__LINE__);
		goto bail; 
	}
	ic_ptcpy(pt2,setgetrb.resval.rpoint);

	pt210[0]=(pt1[1]*pt2[2])-(pt2[1]*pt1[2]);
	pt210[1]=(pt2[0]*pt1[2])-(pt1[0]*pt2[2]);
	pt210[2]=(pt1[0]*pt2[1])-(pt2[0]*pt1[1]);

	rbucs.restype=rbwcs.restype=RTSHORT;
	rbucs.resval.rint=1;
	rbwcs.resval.rint=0;
	rbecs.restype=RT3DPOINT;
	ic_ptcpy(rbecs.resval.rpoint,pt210);

    //*** Get current default circle radius.
    if (SDS_getvar(NULL,
		DB_QCIRCLERAD,
		&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
	{
		rc=(-__LINE__); 
		goto bail; 
	}
    defrad=setgetrb.resval.rreal;

	//*** Get current elevation to check for valid point values.
	if (SDS_getvar(NULL,
		DB_QELEVATION,
		&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) 
	{ 
		rc=(-__LINE__); 
		goto bail; 
	}
	curelev=newelev=setgetrb.resval.rreal;

    for(;;) 
	{
		SDS_OSNAP_TANPER_IGNORE = 0;
		// <alm: enter center point or keyword>
		if(!repeatmode)
		{
			if(sds_initget(SDS_RSG_NONULL,ResourceString(IDC_CIRCLE_INITGET__TTR__RTT_0, "~TTR ~RTT 2_Point|2Point 3_Point|3Point Radius-Tangent-Tangent|RTT Turn_arc_into_circle|Arc ~ Multiple_circles|Multiple ~_TTR ~_RTT ~_2Point ~_3Point ~_RadTanTan ~_Arc ~_ ~_Multiple" ))!=RTNORM) 
			{
				rc=(-__LINE__);
				goto bail;
			}
			ret=internalGetpoint(NULL,ResourceString(IDC_CIRCLE__N2POINT_3POINT_R_1, "\n2Point/3Point/RadTanTan/Arc/Multiple/<Center of circle>: " ),pt1);
		}
		else
		{
			if (sds_initget(SDS_RSG_NONULL,ResourceString(IDC_CIRCLE_INITGET__TTR__RTT_2, "~TTR ~RTT 2_Point|2Point 3_Point|3Point Radius-Tangent-Tangent|RTT Turn_arc_into_circle|Arc ~_TTR ~_RTT ~_2Point ~_3Point ~_RadTanTan ~_Arc" ) )!=RTNORM) 
			{
				rc=(-__LINE__);
				goto bail;
			}
			ret=internalGetpoint(NULL,ResourceString(IDC_CIRCLE__N2POINT_3POINT_R_3, "\n2Point/3Point/RadTanTan/Arc/<Center of circle>: " ),pt1);
		}

		if(ret==RTERROR)
		{
            rc=(-__LINE__); 
			goto bail;
        } 
		else if((ret==RTCAN) || (ret==RTNONE)) 
		{
            rc=(-1L); goto bail;
        }
		else if(ret==RTNORM) 
		{
			// <alm: center point was picked>
            //*** Set LASTCMDPT,ISLASTCMDPT variable.
            rblast.restype=RT3DPOINT;
            ic_ptcpy(rblast.resval.rpoint,pt1);
			SDS_setvar(NULL,DB_QLASTCMDPT,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
			rblast.restype=RTSHORT;
            rblast.resval.rint=1;
			SDS_setvar(NULL,DB_QISLASTCMDPT,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
			if(!icadRealEqual(pt1[2],curelev))
			{
				SDS_CorrectElevation=curelev;
				SDS_ElevationChanged=TRUE;
				newelev=setgetrb.resval.rreal=pt1[2];
				setgetrb.restype=RTREAL;
				sds_setvar("ELEVATION"/*DNT*/,&setgetrb);
			}
            break;
        }
		else if(ret==RTKWORD)
		{
			// <alm: some option was chosen>
			bBasic3PointCircle = FALSE;
			SDS_OSNAP_TANPER_IGNORE = -1;
			if (sds_getinput(fs1)!=RTNORM)
			{
				rc=(-__LINE__);
				goto bail;
			}
            if(strisame(fs1,"3POINT"/*DNT*/)) 
			{
				// 3Point mode
                for(;;) 
				{
                    if(sds_initget(0,NULL)!=RTNORM)
					{
                       rc=(-__LINE__);
                       goto bail;
                    }
					//<alm: pick first point>
                    if((ret=internalGetpoint(NULL,ResourceString(IDC_CIRCLE__NFIRST_POINT_ON__5, "\nFirst point on circle: " ),pt1))==RTERROR) 
					{
                        rc=(-__LINE__);
						goto bail;
                    }
					else if(ret==RTCAN)
					{
                        rc=(-1L);
						goto bail;
                    }
					else if(ret==RTNORM) 
					{
						if(!icadRealEqual(pt1[2],curelev))
						{
							SDS_CorrectElevation=curelev;
							SDS_ElevationChanged=TRUE;
							newelev=setgetrb.resval.rreal=pt1[2];
							setgetrb.restype=RTREAL;
							sds_setvar("ELEVATION"/*DNT*/,&setgetrb);
						}
						break;
                    }
					else
					{
                        cmd_ErrorPrompt(CMD_ERR_PT,0);
                        continue;
                    }
                }

				if(SDS_OSNAP_TANPER_IGNORE > 0)
				{
					//get ignored snap settings for 1st point...
					struct resbuf rb;
					SDS_getvar(NULL,DB_QOSMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

					if (SDS_OSNAP_TANPER_IGNORE == 3)
						tan_per1 = 2;
					else if (SDS_OSNAP_TANPER_IGNORE == 1)
						tan_per1 = (rb.resval.rint&128) == 128 ? 1 : 0;
					else
						tan_per1 = SDS_OSNAP_TANPER_IGNORE;

					SDS_OSNAP_TANPER_IGNORE = -1;
				}
				else
				{
					tan_per1=SDS_OSNAP_TANPER_IGNORE=0;
				}

                for(;;) 
				{
					SDS_OSNAP_TANPER_IGNORE = -1;
                    if(sds_initget(0,NULL)!=RTNORM) 
					{
                       rc=(-__LINE__);
                       goto bail;
                    }
					// pick second point
                    if((ret=internalGetpoint(pt1,ResourceString(IDC_CIRCLE__NSECOND_POINT___6, "\nSecond point: " ),pt2))==RTERROR) 
					{
                        rc=(-__LINE__); goto bail;
                    }
					else if(ret==RTCAN) 
					{
                        rc=(-1L); goto bail;
                    }
					else if(ret==RTNORM) 
					{
                        if(!icadRealEqual(pt2[2],pt1[2]) && !icadRealEqual(pt2[2],curelev))
						{
                            cmd_ErrorPrompt(CMD_ERR_2DPT,0);
                            continue;
                        }
                        pt2[2]=pt1[2];
                        //***
                        break;
                    }
					else
					{
                        cmd_ErrorPrompt(CMD_ERR_PT,0);
                        continue;
                    }
                }

				if(SDS_OSNAP_TANPER_IGNORE > 0)
				{
					//get ignored snap settings for 1st point...
					struct resbuf rb;
					SDS_getvar(NULL,DB_QOSMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

					if (SDS_OSNAP_TANPER_IGNORE == 3)
						tan_per2 = 2;
					else if (SDS_OSNAP_TANPER_IGNORE == 1)
						tan_per2 = (rb.resval.rint&128) == 128 ? 1 : 0;
					else
						tan_per2 = SDS_OSNAP_TANPER_IGNORE;

					SDS_OSNAP_TANPER_IGNORE = -1;
				}
				else
				{
					tan_per2=SDS_OSNAP_TANPER_IGNORE=0;
				}

                for(;;) 
				{
					// pick last (3rd) point
					SDS_OSNAP_TANPER_IGNORE = -1;
					if(sds_initget(SDS_RSG_OTHER,NULL)!=RTNORM)
					{
                       rc=(-__LINE__);
                       goto bail;
                    }
                    SDS_getvar(NULL,DB_QDRAGMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
                    IC_RELRB(rslt);
                    if((ret=SDS_dragobject(4,rb.resval.rint,pt1,pt2,0L,ResourceString(IDC_CIRCLE__NTHIRD_POINT___7, "\nThird point: " ),&rslt,pt3,NULL))==RTERROR) 
					{
                        rc=(-__LINE__); goto bail;
                    }
					else if(ret==RTCAN)
					{
                        rc=(-1L); goto bail;
                    }
					else if(ret==RTNORM)
					{
                        if(!icadRealEqual(pt3[2],pt1[2]) && !icadRealEqual(pt3[2],curelev))
						{
                            cmd_ErrorPrompt(CMD_ERR_2DPT,0);
                            continue;
                        }

						if(rslt==NULL)
						{
							cmd_ErrorPrompt(CMD_ERR_ONSAMELINE,0);
							goto bail;
						}

                        pt3[2]=pt1[2];
						tan_per3 = SDS_OSNAP_TANPER_IGNORE;
                        //*** Create entity.

						if ((tan_per1>0  && tan_per2<=0 && tan_per3<=0) ||  //PPL, PPC, 
							(tan_per1<=0 && tan_per2>0  && tan_per3<=0) ||
							(tan_per1<=0 && tan_per2<=0 && tan_per3>0 ))
						{
							SDS_OSNAP_TANPER_IGNORE=0;
							if     (1==(tan_per1&3))sds_osnap(pt1,"PER"/*DNT*/,pt1);
							else if(2==(tan_per1&3))sds_osnap(pt1,"TAN"/*DNT*/,pt1);
							else if(3==(tan_per1&3))sds_osnap(pt1,"TAN,PER"/*DNT*/,pt1);
							ic_3pt2arc(pt1, pt2, pt3, pt4, &newrad, &sa, &ea);
						}
						else if ((tan_per1<=0 && tan_per2>0  && tan_per3>0 ) ||	//PLL, PLC, PCC
								 (tan_per1>0  && tan_per2<=0 && tan_per3>0 ) ||
								 (tan_per1>0  && tan_per2>0  && tan_per3<=0))
						{
							if (tan_per1<=0)//PXX
								ret = cmd_circle3pt_tan_per(pt1, pt2, pt3, tan_per2, tan_per3, pt4, &newrad);
							else if (tan_per2<=0)//XPX
								ret = cmd_circle3pt_tan_per(pt2, pt1, pt3, tan_per1, tan_per3, pt4, &newrad);
							else if (tan_per3<=0)//XXP
								ret = cmd_circle3pt_tan_per(pt3, pt1, pt2, tan_per1, tan_per2, pt4, &newrad);
							else
								assert(false);

							if (ret!=RTNORM && ret!=RTNONE)
								return ret;
							if (ret==RTNONE)
							{
								cmd_ErrorPrompt(CMD_ERR_BADGEOMETRY,0);
								ret=RTNORM;
								continue;
							}
#if 0
							// one point + two tangent entities
							ret = cmd_circle3pt_tan_per(pt1,pt2,pt3,tan_per1,tan_per2,tan_per3, pt4, &newrad);
#endif	 
						}
						else if (tan_per1>0 && tan_per2>0 && tan_per3>0) //LCC, LLC, LLL, CCC
						{
							IC_RELRB(rslt);
							ret=cmd_circle3pt_tan_per(pt1,pt2,pt3,tan_per1,tan_per2,tan_per3, pt4, &newrad);
							if(ret!=RTNORM && ret!=RTNONE) return(ret);
							if(ret==RTNONE)
							{
								cmd_ErrorPrompt(CMD_ERR_BADGEOMETRY,0);
								ret=RTNORM;
								continue;
							}
						}
						else 
						{
							newent=sds_buildlist(RTDXF0,"CIRCLE"/*DNT*/,210,pt210,0);
							newent->rbnext->rbnext=rslt;
							bBasic3PointCircle = TRUE;
						}

						if (!bBasic3PointCircle)
	                        newent=sds_buildlist(RTDXF0,"CIRCLE"/*DNT*/,10,pt4,40,newrad,210,pt210,0);

						if((rc=sds_entmake(newent))!=RTNORM) 
						{
							cmd_ErrorPrompt(CMD_ERR_CREATEENT,0);
							rc=(-__LINE__); 
							goto bail;
						}
						if(repeatmode)
						{
							elp=new db_circle(SDS_CURDWG);
							ic_assoc(newent,10);
							elp->set_10(ic_rbassoc->resval.rpoint);
							elp->set_210(pt210);
							ic_assoc(newent,40);
							elp->set_40(ic_rbassoc->resval.rreal);
						}
						ic_assoc(newent,10);
						rbtemp=ic_rbassoc;
						while(repeatmode && rc==RTNORM)
						{
							SDS_getvar(NULL,DB_QDRAGMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
							ent2[0]=(long)elp;
							ent2[1]=(long)SDS_CURDWG;
                            rc=SDS_dragobject(-1,rb.resval.rint,(sds_real *)ent2,(double *)elp->retp_10(),
											 0.0,ResourceString(IDC_CIRCLE__NCENTER_OF_CIRCL_8, "\nCenter of circle: " ),NULL,pt2,NULL);
							if(rc==RTNORM)
							{
								ic_ptcpy(pt3,pt2); //for use as lastpt (keep in ucs)
								sds_trans(pt2,&rbucs,&rbecs,0,pt2);
								ic_ptcpy(rbtemp->resval.rpoint,pt2);
								sds_entmake(newent);	//make a 2nd ent to drag around
							}
						}
						if(elp)
						{
							delete(elp);
							elp=NULL;
						}

                        //*** Save new radius to CIRCLERAD system variable.
                        if (ic_assoc(newent,40)!=0) 
						{
							rc=(-__LINE__);
							goto bail;
						}
                        
						defrad=ic_rbassoc->resval.rreal;
                        setgetrb.restype=RTREAL;
                        setgetrb.resval.rreal=defrad;
                        if (sds_setvar("CIRCLERAD"/*DNT*/,&setgetrb)!=RTNORM)
						{
							rc=(-__LINE__);
							goto bail;
						}
						//ic_assoc(newent,10);
						//ic_ptcpy(pt1,ic_rbassoc->resval.rpoint);
                        IC_RELRB(newent);
                        //*** Set LASTCMDPT,ISLASTCMDPT,LASTCMDANG variable.
                        ic_ptcpy(rblast.resval.rpoint,pt3);
                        rblast.restype=RT3DPOINT;
						SDS_setvar(NULL,DB_QLASTCMDPT,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
                        rblast.restype=RTREAL;
                        rblast.resval.rreal=sds_angle(pt1,pt3);
						SDS_setvar(NULL,DB_QLASTCMDANG,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
						rblast.restype=RTSHORT;
                        rblast.resval.rint=1;
						SDS_setvar(NULL,DB_QISLASTCMDPT,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
                        goto bail;
                    }
					else
					{
                        cmd_ErrorPrompt(CMD_ERR_PT,0);
                        continue;
                    }
                }
            }
			else if(strisame(fs1,"2POINT"/*DNT*/))
			{
                for(;;)
				{
					SDS_OSNAP_TANPER_IGNORE=-1;
                    if(sds_initget(0,NULL)!=RTNORM)
					{
                       rc=(-__LINE__);
                       goto bail;
                    }
                    if((ret=internalGetpoint(NULL,ResourceString(IDC_CIRCLE__NFIRST_POINT_ON_10, "\nFirst point on diameter: " ),pt1))==RTERROR)
					{
                        rc=(-__LINE__); goto bail;
                    }
					else if(ret==RTCAN)
					{
                        rc=(-1L); goto bail;
                    }
					else if(ret==RTNORM)
					{
						if(!icadRealEqual(pt1[2],curelev))
						{
							SDS_CorrectElevation=curelev;
							SDS_ElevationChanged=TRUE;
							newelev=setgetrb.resval.rreal=pt1[2];
							setgetrb.restype=RTREAL;
							sds_setvar("ELEVATION"/*DNT*/,&setgetrb);
						}
                        break;
                    }
					else
					{
						cmd_ErrorPrompt(CMD_ERR_PT,0);
                        continue;
                    }
                }

				if(SDS_OSNAP_TANPER_IGNORE > 0)
				{
					//get ignored snap settings for 1st point...
					struct resbuf rb;
					SDS_getvar(NULL,DB_QOSMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

					if (SDS_OSNAP_TANPER_IGNORE == 3)
						tan_per1 = 2;
					else if (SDS_OSNAP_TANPER_IGNORE == 1)
						tan_per1 = (rb.resval.rint&128) == 128 ? 1 : 0;
					else
						tan_per1 = SDS_OSNAP_TANPER_IGNORE;

					SDS_OSNAP_TANPER_IGNORE = -1;
				}
				else
				{
					tan_per1=SDS_OSNAP_TANPER_IGNORE=0;
				}

                for(;;)
				{
                    if(sds_initget(SDS_RSG_OTHER,NULL)!=RTNORM)
					{
                       rc=(-__LINE__);
                       goto bail;
                    }
                    SDS_getvar(NULL,DB_QDRAGMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
                    IC_RELRB(rslt);
                    if((ret=SDS_dragobject(3,rb.resval.rint,pt1,pt2,0L,ResourceString(IDC_CIRCLE__NSECOND_POINT_O_11, "\nSecond point on diameter: " ),&rslt,pt2,NULL))==RTERROR)
					{
                        rc=(-__LINE__); goto bail;
                    }
					else if(ret==RTCAN)
					{
                        rc=(-1L); goto bail;
                    }
					else if(ret==RTNORM)
					{
                        if(!icadRealEqual(pt2[2],pt1[2]) && !icadRealEqual(pt2[2],curelev))
						{
                            cmd_ErrorPrompt(CMD_ERR_2DPT,0);
                            continue;
                        }
                        //*** Create entity.
						if(tan_per1>0 && SDS_OSNAP_TANPER_IGNORE<=0)
						{
							SDS_OSNAP_TANPER_IGNORE=0;
							if     (1==(tan_per1&3))sds_osnap(pt1,"PER"/*DNT*/,pt1);
							else if(2==(tan_per1&3))sds_osnap(pt1,"TAN"/*DNT*/,pt1);
							else if(3==(tan_per1&3))sds_osnap(pt1,"TAN,PER"/*DNT*/,pt1);
						}
						else if(tan_per1>0 && SDS_OSNAP_TANPER_IGNORE>0)
						{
							ret=cmd_circle2pt_tan_per(pt1,pt2,tan_per1,SDS_OSNAP_TANPER_IGNORE);
							if(ret!=RTNORM && ret!=RTNONE) return(ret);
							if(ret==RTNONE)
							{
								cmd_ErrorPrompt(CMD_ERR_BADGEOMETRY,0);
								ret=RTNORM;
								continue;
							}
						}

						rb.restype=RTSHORT;rblast.restype=RT3DPOINT;
						rb.resval.rint=1;ic_ptcpy(rblast.resval.rpoint,pt210);
						sds_trans(pt1,&rb,&rblast,0,pt1);
						sds_trans(pt2,&rb,&rblast,0,pt2);
						pt3[0] = (pt1[0]+pt2[0])/2.0;
						pt3[1] = (pt1[1]+pt2[1])/2.0;
						pt3[2] = (pt1[2]+pt2[2])/2.0;
						newrad=sds_distance(pt3,pt2);

                        newent=sds_buildlist(RTDXF0,"CIRCLE"/*DNT*/,10,pt3,40,newrad,210,pt210,0);
						if((rc=sds_entmake(newent))!=RTNORM)
						{
							cmd_ErrorPrompt(CMD_ERR_CREATEENT,0);
							rc=(-__LINE__); goto bail;
						}
						if(repeatmode)
						{
							elp=new db_circle(SDS_CURDWG);
							ic_assoc(newent,10);
							elp->set_10(ic_rbassoc->resval.rpoint);
							elp->set_210(pt210);
							ic_assoc(newent,40);
							elp->set_40(ic_rbassoc->resval.rreal);
						}
						ic_assoc(newent,10);
						rbtemp=ic_rbassoc;
						while(repeatmode && rc==RTNORM)
						{
							SDS_getvar(NULL,DB_QDRAGMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
							ent2[0]=(long)elp;
							ent2[1]=(long)SDS_CURDWG;
                            rc=SDS_dragobject(-1,rb.resval.rint,(sds_real *)ent2,(double *)elp->retp_10(),
										 0.0,ResourceString(IDC_CIRCLE__NCENTER_OF_CIRCL_8, "\nCenter of circle: " ),NULL,pt2,NULL);
							if(rc==RTNORM)
							{
								ic_ptcpy(pt3,pt2); //for use as lastpt (keep in ucs)
								sds_trans(pt2,&rbucs,&rbecs,0,pt2);
								ic_ptcpy(rbtemp->resval.rpoint,pt2);
								sds_entmake(newent);	//make a 2nd ent to drag around
							}
						}
						if(elp){delete(elp);elp=NULL;}

                        //*** Save new radius to CIRCLERAD system variable.
                        if(ic_assoc(newent,40)!=0) { rc=(-__LINE__); goto bail; }
                        defrad=ic_rbassoc->resval.rreal;
                        setgetrb.restype=RTREAL;
                        setgetrb.resval.rreal=defrad;
                        if(sds_setvar("CIRCLERAD"/*DNT*/,&setgetrb)!=RTNORM) { rc=(-__LINE__); goto bail; }
                        IC_RELRB(newent);
                        //*** Set LASTCMDPT,LASTCMDANG,ISLASTCMDPT variable.
                        rblast.restype=RT3DPOINT;
                        ic_ptcpy(rblast.resval.rpoint,pt2);
						SDS_setvar(NULL,DB_QLASTCMDPT,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
						rblast.restype=RTREAL;
                        rblast.resval.rreal=sds_angle(pt1,pt3);
						SDS_setvar(NULL,DB_QLASTCMDANG,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
                        rblast.restype=RTSHORT;
                        rblast.resval.rint=1;
						SDS_setvar(NULL,DB_QISLASTCMDPT,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
                        goto bail;
                    }
					else
					{
                        cmd_ErrorPrompt(CMD_ERR_2DPT,0);
                        continue;
                    }
                }
			}
			else if(strisame(fs1,"MULTIPLE"/*DNT*/))
			{
				repeatmode=1;
				continue;
			}
			else if(strisame(fs1,"ARC"/*DNT*/))
			{
				//convert an arc into a circle
				for(;;)
				{
					if ((ret=sds_entsel(ResourceString(IDC_CIRCLE__NSELECT_ARC_TO__14, "\nSelect arc to convert to circle: " ),ent1,pt1))!=RTNORM)
					{
						if(ret==RTERROR)
						{
							SDS_getvar(NULL,DB_QERRNO,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
							if(setgetrb.resval.rint==OL_ENTSELPICK)
							{
								cmd_ErrorPrompt(CMD_ERR_ARC,0);
								continue;
							}
							else if(setgetrb.resval.rint==OL_ENTSELNULL)
							{
								rc=0L;
							}
						}
						if(ret==RTCAN)rc=(-1L);
						goto bail;
					}
					rb.restype=RTSTR;rb.resval.rstring="*"/*DNT*/;
					if(elist1!=NULL){sds_relrb(elist1);elist1=NULL;}
					if((elist1=sds_entgetx(ent1,&rb))==NULL){rc=-2;goto bail;}
					ic_assoc(elist1,0);
					if (!strsame(ic_rbassoc->resval.rstring,"ARC"/*DNT*/))
						cmd_ErrorPrompt(CMD_ERR_ARC,0);
					else
						break;
				}
				if(newent!=NULL){sds_relrb(newent);newent=NULL;}
				
				//ic_assoc(elist1,0);
				newent=ic_rbassoc;
                if((newent->resval.rstring=ic_realloc_char(newent->resval.rstring,strlen("CIRCLE"/*DNT*/)+1))==NULL) 
				{
                    cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
                    rc=(-2); goto bail;
                }
				strcpy(newent->resval.rstring,"CIRCLE"/*DNT*/);
				
				for(got1=50;got1<52;got1++)
				{
					for(newent=elist1;(newent->rbnext->restype!=got1)&&(newent->rbnext!=NULL);newent=newent->rbnext);
					
					if(newent->rbnext->restype==got1)
					{
						rslt=newent->rbnext;
						newent->rbnext=newent->rbnext->rbnext;
						rslt->rbnext=NULL;
						sds_relrb(rslt);
						rslt=NULL;
					}
				}
				newent=NULL;

				rb.restype=RTSHORT;rblast.restype=RT3DPOINT;
				rb.resval.rint=1;ic_ptcpy(rblast.resval.rpoint,pt210);
				ic_assoc(elist1,10);
				sds_trans(ic_rbassoc->resval.rpoint,&rblast,&rb,0,pt1);

				do
				{
					if(sds_entmake(elist1)!=RTNORM)
					{
						cmd_ErrorPrompt(CMD_ERR_CREATEENT,0);
						rc=(-__LINE__); goto bail;
					}
					if(!repeatmode)break;
					if((rc=internalGetpoint(pt1,ResourceString(IDC_CIRCLE__NCENTER_OF_CIRCL_8, "\nCenter of circle: " ),pt2))==RTNORM)
					{
						ic_assoc(elist1,10);
						sds_trans(pt2,&rbucs,&rbecs,0,ic_rbassoc->resval.rpoint);
						ic_ptcpy(pt2,pt1);
					}
				}
				while(rc==RTNORM);
				//if(sds_entmake(elist1)!=RTNORM){rc=(-2L);goto bail;}
				if(sds_entdel(ent1)!=RTNORM){rc=(-2L);goto bail;}
				
				sds_entlast(ent1);
				sds_redraw(ent1,IC_REDRAW_DRAW);
                rblast.restype=RT3DPOINT;
                ic_ptcpy(rblast.resval.rpoint,pt1);
				SDS_setvar(NULL,DB_QLASTCMDPT,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
				ic_assoc(elist1,10);
                rblast.restype=RTREAL;
                rblast.resval.rreal=sds_angle(ic_rbassoc->resval.rpoint,pt1);
				SDS_setvar(NULL,DB_QLASTCMDANG,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
				rblast.restype=RTSHORT;
                rblast.resval.rint=1;
				SDS_setvar(NULL,DB_QISLASTCMDPT,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
				if(elist1!=NULL){sds_relrb(elist1);elist1=NULL;}
				return(RTNORM);
            }
			else if((strisame(fs1,"RADTANTAN"/*DNT*/)) ||
                      (strisame(fs1,"TTR"/*DNT*/)) || 
                      (strisame(fs1,"RTT"/*DNT*/)))
			{
				//if we want radius first, get it
				if(strisame(fs1,"RADTANTAN"/*DNT*/))
                    ttr=0;
				else
					ttr=1;
				if(!ttr)
				{
					get_radius:	  //WARNING: TTR mode jumps in here
					while(1)
					{
						SDS_getvar(NULL,DB_QCIRCLERAD,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
						if(setgetrb.resval.rreal>0.0)
						{
							sds_rtos(setgetrb.resval.rreal,-1,-1,fs2);
							sprintf(fs1,ResourceString(IDC_CIRCLE__NRADIUS_OF_CIRC_18, "\nRadius of circle <%s> : " ),fs2);
						}
						else
						{
							sprintf(fs1,ResourceString(IDC_CIRCLE__NRADIUS_OF_CIRC_19, "\nRadius of circle: " ));
						}
						sds_initget(0,NULL);
						ret=sds_getdist(NULL,fs1,&newrad);
						if(ret==RTNONE)
						{
							if(icadRealEqual(setgetrb.resval.rreal,0.0))
							{
								cmd_ErrorPrompt(CMD_ERR_POSITIVE,0);
								continue;
							}
							else
							{
								newrad=setgetrb.resval.rreal;
								break;
							}
						}
						else if(ret==RTNORM)
						{
							setgetrb.resval.rreal=newrad;
							if(sds_setvar("CIRCLERAD"/*DNT*/,&setgetrb)!=RTNORM){rc=(-__LINE__);goto bail;}
							break;
						}
						else if(ret==RTCAN)
						{
							rc=(-1L);
							goto bail;
						}
						else
						{
							cmd_ErrorPrompt(CMD_ERR_DIST,0);
							continue;
						}
					}
					if(ttr)goto got_both; //WARNING: TTR mode jumps out here
				}
				get_tangents:
				got1=0;
                for(;;) 
				{
					if(!got1)
						ret=sds_entsel(ResourceString(IDC_CIRCLE__NSELECT_FIRST_T_20, "\nSelect first tangent point: " ),ent1,pt1);
					else
						ret=sds_entsel(ResourceString(IDC_CIRCLE__NSELECT_SECOND__21, "\nSelect second tangent point: " ),ent2,pt2);
                    if(ret==RTERROR) 
					{
						SDS_getvar(NULL,DB_QERRNO,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
						if(rb.resval.rint==OL_ENTSELPICK)
						{
							cmd_ErrorPrompt(CMD_ERR_SELECTION,0);
							continue;
						}
						else
						{
	                        rc=(-1L); goto bail;
						}
                    }
					else if(ret==RTCAN)
					{
                        rc=(-1L); goto bail;
                    }
					else if(ret==RTNORM)
					{
						if(got1)
						{
							if(elist2!=NULL){sds_relrb(elist2);elist2=NULL;}
							if((elist2=sds_entget(ent2))==NULL){rc=(-__LINE__);goto bail;}
							ic_assoc(elist2,0);
							if(sds_wcmatch(ic_rbassoc->resval.rstring,"LINE,ARC,CIRCLE,POLYLINE,LWPOLYLINE,TRACE,RAY,XLINE"/*DNT*/)!=RTNORM)
							{
								cmd_ErrorPrompt(CMD_ERR_ENTTYPE,0);
								continue;
							}
							if(strsame(ic_rbassoc->resval.rstring,"POLYLINE"/*DNT*/))
							{
								ic_assoc(elist2,70);
								if(ic_rbassoc->resval.rint&(IC_PLINE_3DPLINE | IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH))
								{
									cmd_ErrorPrompt(CMD_ERR_2DPOLYLINE,0);
									continue;
								}
								ic_assoc(elist2,210);
								if(!icadPointEqual(ic_rbassoc->resval.rpoint,pt210))
								{
									cmd_ErrorPrompt(CMD_ERR_NOTINUCS,0);
									continue;
								}
							}
							else if(strsame(ic_rbassoc->resval.rstring,"LWPOLYLINE"/*DNT*/))
							{
								ic_assoc(elist2,210);
								if(!icadPointEqual(ic_rbassoc->resval.rpoint,pt210))
								{
									cmd_ErrorPrompt(CMD_ERR_NOTINUCS,0);
									continue;
								}
							}
							else if(strsame(ic_rbassoc->resval.rstring,"CIRCLE"/*DNT*/) ||
									 strsame(ic_rbassoc->resval.rstring,"TRACE"/*DNT*/)){
								ic_assoc(elist2,210);
								if(!icadPointEqual(ic_rbassoc->resval.rpoint,pt210))
								{
									cmd_ErrorPrompt(CMD_ERR_NOTINUCS,0);
									continue;
								}
							}
							break;
						}
						else
						{
							if(elist1!=NULL){sds_relrb(elist1);elist1=NULL;}
							if((elist1=sds_entget(ent1))==NULL){rc=(-__LINE__);goto bail;}
							ic_assoc(elist1,0);
							if(sds_wcmatch(ic_rbassoc->resval.rstring,"LINE,ARC,CIRCLE,LWPOLYLINE,POLYLINE,TRACE,RAY,XLINE"/*DNT*/)!=RTNORM)
							{
								cmd_ErrorPrompt(CMD_ERR_ENTTYPE,0);
								continue;
							}
							if(strsame(ic_rbassoc->resval.rstring,"POLYLINE"/*DNT*/))
							{
								ic_assoc(elist1,70);
								if(ic_rbassoc->resval.rint&(IC_PLINE_3DPLINE | IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH))
								{
									cmd_ErrorPrompt(CMD_ERR_2DPOLYLINE,0);
									continue;
								}
								ic_assoc(elist1,210);
								if(!icadPointEqual(ic_rbassoc->resval.rpoint,pt210))
								{
									cmd_ErrorPrompt(CMD_ERR_NOTINUCS,0);
									continue;
								}
							}
							else if(strsame(ic_rbassoc->resval.rstring,"LWPOLYLINE"/*DNT*/))
							{
								ic_assoc(elist1,210);
								if(!icadPointEqual(ic_rbassoc->resval.rpoint,pt210))
								{
									cmd_ErrorPrompt(CMD_ERR_NOTINUCS,0);
									continue;
								}
							}
							else if(strsame(ic_rbassoc->resval.rstring,"CIRCLE"/*DNT*/) ||
									 strsame(ic_rbassoc->resval.rstring,"TRACE"/*DNT*/))
							{
								ic_assoc(elist1,210);
								if(!icadPointEqual(ic_rbassoc->resval.rpoint,pt210))
								{
									cmd_ErrorPrompt(CMD_ERR_NOTINUCS,0);
									continue;
								}
							}
							got1=1;
							continue;
						}
                    }
					else
					{
                        cmd_ErrorPrompt(CMD_ERR_PT,0);
                        continue;
                    }
                }
				if(ttr)goto get_radius;
				got_both:
				//Now we have both ents(LINE,ARC,CIRCLE,PLINE), their elists,
				//& selected pts. Try to construct a circle from them.
				if((rc=cmd_circle_ttr(pt1,ent1,elist1,pt2,ent2,elist2,newrad,pt1))!=RTNORM)
				{
					goto bail;
				}
				//note: pt1 is in ecs
				if(elist1!=NULL){sds_relrb(elist1);elist1=NULL;}
				ic_ptcpy(rblast.resval.rpoint,pt1);


				elist1=sds_buildlist(RTDXF0,"CIRCLE"/*DNT*/,10,pt1,40,newrad,210,pt210,0);
				if((rc=sds_entmake(elist1))!=RTNORM)
					goto bail;
                rblast.restype=RT3DPOINT;
                ic_ptcpy(rblast.resval.rpoint,pt2);
				SDS_setvar(NULL,DB_QLASTCMDPT,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
                rblast.restype=RTREAL;
                rblast.resval.rreal=sds_angle(pt1,pt2);
				SDS_setvar(NULL,DB_QLASTCMDANG,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
				rblast.restype=RTSHORT;
                rblast.resval.rint=1;
				SDS_setvar(NULL,DB_QISLASTCMDPT,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
				if(elist1!=NULL){sds_relrb(elist1);elist1=NULL;}
				if(elist2!=NULL){sds_relrb(elist2);elist2=NULL;}
				if(repeatmode)
				{
					ttr=0;
					goto get_tangents;
				}
				else
					return(ret);//return rather than making ent below
            }
			else
			{
                cmd_ErrorPrompt(CMD_ERR_PTKWORD,0);
                continue;
            }
        }
		else
		{
            cmd_ErrorPrompt(CMD_ERR_PTKWORD,0);
            continue;
        }
    }

    //*** Get the radius or diameter.
    for(;;)
	{
		if(sds_initget(0,ResourceString(IDC_CIRCLE_INITGET_DIAMETER_22, "Diameter ~_Diameter" ))!=RTNORM) 
		{ 
            rc=(-__LINE__); 
            goto bail; 
        }
		if(sds_rtos(defrad,-1,-1,fs2)!=RTNORM)
		{
            rc=(-__LINE__); 
            goto bail; 
        }
		if(defrad>0.0)
			sprintf(fs1,ResourceString(IDC_CIRCLE__NDIAMETER__RADI_23, "\nDiameter/<Radius> <%s>: " ),fs2);
		else
			sprintf(fs1,ResourceString(IDC_CIRCLE__NDIAMETER__RADI_24, "\nDiameter/<Radius>: " ));
        SDS_getvar(NULL,DB_QDRAGMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

		SDS_OSNAP_TANPER_IGNORE=-1;

		if (rslt!=NULL){sds_relrb(rslt);rslt=NULL;}
        ret=SDS_dragobject(1,rb.resval.rint,pt1,pt2,0L,fs1,&rslt,pt2,fs2);
        
		if(ret==RTERROR)
		{
            rc=(-__LINE__); goto bail;
        }
		else if(ret==RTCAN)
		{
            rc=(-1L); goto bail;
        }
		else if(ret==RTNORM)
		{
			if(SDS_OSNAP_TANPER_IGNORE > 0)
			{
	            rblast.restype=RT3DPOINT;
		        ic_ptcpy(rblast.resval.rpoint,pt1);
				SDS_setvar(NULL,DB_QLASTPOINT,&rblast,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);

				sds_point p;
				SDS_OSNAP_TANPER_IGNORE=0;
				sds_osnap(pt2,"PER"/*DNT*/,p);

				newrad=ic_dist2d(pt1, p);
			}
			else
			{
				ic_assoc(rslt,40);
		        newrad=ic_rbassoc->resval.rreal;
			}

            //*** Create entity.
			if(newrad<=0.0)
			{
				cmd_ErrorPrompt(CMD_ERR_POSNOZERO,0);
				continue;
			}
            break;
        }
		else if(ret==RTNONE && defrad>0.0)
		{
            newrad=defrad;
            break;
        }
		else if(ret==RTKWORD || ret==RTSTR)
		{
            if(sds_getinput(fs1)!=RTNORM)
				return(RTCAN);
			if(strisame(fs1,"DIAMETER"/*DNT*/))
			{
                for(;;)
				{
                    if(sds_initget(SDS_RSG_OTHER,NULL)!=RTNORM)
					{
                       rc=(-__LINE__);
                       goto bail;
                    }
                    if(sds_rtos((defrad*2.0),-1,-1,fs2)!=RTNORM)
					{
						rc=(-__LINE__);
						goto bail;
					}
                    if(defrad>0.0)
						sprintf(fs1,ResourceString(IDC_CIRCLE__NDIAMETER_OF_CI_25, "\nDiameter of circle <%s>: " ),fs2);
                    else
						sprintf(fs1,ResourceString(IDC_CIRCLE__NDIAMETER_OF_CI_26, "\nDiameter of circle: " ));
					if(rslt!=NULL){sds_relrb(rslt);rslt=NULL;}
                    SDS_getvar(NULL,DB_QDRAGMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
                    ret=SDS_dragobject(20,rb.resval.rint,pt1,pt2,0L,fs1,&rslt,pt2,fs2);
                    if(ret==RTERROR)
					{
                        rc=(-__LINE__); goto bail;
                    }
					else if(ret==RTCAN)
					{
                        rc=(-1L); goto bail;
                    }
					else if(ret==RTNORM)
					{
						ic_assoc(rslt,40);
						newrad=ic_rbassoc->resval.rreal;

						if(newrad<=0.0)
						{
							cmd_ErrorPrompt(CMD_ERR_POSNOZERO,0);
							continue;
						}
                        break;
                    }
					else if(ret==RTNONE && defrad>0.0)
					{
                        newrad=defrad;
                        break;
                    }
					else if(ret==RTSTR && ic_isnumstr(fs2,&newrad))
					{
                        if(newrad<0.0)
						{
							cmd_ErrorPrompt(CMD_ERR_POSNOZERO,0);
                            continue;
                        }
                        newrad/=2.0;
                        break;
                    }
					else
					{
						cmd_ErrorPrompt(CMD_ERR_DIST2NDPT,0);
                        continue;
                    }
                }
                break;
            }
			else if(ic_isnumstr(fs2,&newrad))
			{
                if(newrad<0.0)
				{
                    cmd_ErrorPrompt(CMD_ERR_POSNOZERO,0);
                    continue;
                }
                break;
            }
			else
			{
				cmd_ErrorPrompt(CMD_ERR_RADDCIRC,0);
                continue;
            }
        }
		else
		{
			cmd_ErrorPrompt(CMD_ERR_RADDCIRC,0);
            continue;
        }
    }
	if(rslt!=NULL){sds_relrb(rslt);rslt=NULL;}
    //*** Create the entity.
	rb.restype=RTSHORT;rblast.restype=RT3DPOINT;
	rb.resval.rint=1;ic_ptcpy(rblast.resval.rpoint,pt210);
	sds_trans(pt1,&rb,&rblast,0,pt3);

    if((newent=sds_buildlist(RTDXF0,"CIRCLE"/*DNT*/,10,pt3,40,newrad,210,pt210,0))==NULL)
	{
        cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
        rc=(-1L); goto bail;
    }
	if((rc=sds_entmake(newent))!=RTNORM)
	{
		cmd_ErrorPrompt(CMD_ERR_CREATEENT,0);
		rc=(-__LINE__); goto bail;
	}
	if(repeatmode)
	{
		elp=new db_circle(SDS_CURDWG);
		ic_assoc(newent,10);
		elp->set_10(ic_rbassoc->resval.rpoint);
		elp->set_210(pt210);
		ic_assoc(newent,40);
		elp->set_40(ic_rbassoc->resval.rreal);
	}
	ic_assoc(newent,10);
	rbtemp=ic_rbassoc;
	while(repeatmode && rc==RTNORM)
	{
		SDS_getvar(NULL,DB_QDRAGMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		ent2[0]=(long)elp;
		ent2[1]=(long)SDS_CURDWG;
        rc=SDS_dragobject(-1,rb.resval.rint,(sds_real *)ent2,(double *)elp->retp_10(),
					 0.0,ResourceString(IDC_CIRCLE__NCENTER_OF_CIRCL_8, "\nCenter of circle: " ),NULL,pt2,NULL);
		if(rc==RTNORM)
		{
			ic_ptcpy(pt3,pt2); //for use as lastpt (keep in ucs)
			sds_trans(pt2,&rbucs,&rbecs,0,pt2);
			ic_ptcpy(rbtemp->resval.rpoint,pt2);
			sds_entmake(newent);	//make a 2nd ent to drag around
		}
	}
	if (elp){delete(elp);elp=NULL;}
    IC_RELRB(newent);
    //*** Save new radius to CIRCLERAD system variable.
    setgetrb.restype=RTREAL;
    setgetrb.resval.rreal=newrad;
    if (sds_setvar("CIRCLERAD"/*DNT*/,&setgetrb)!=RTNORM) { rc=(-__LINE__); goto bail; }

bail:
	SDS_OSNAP_TANPER_IGNORE = 0;

	if(elist1!=NULL){sds_relrb(elist1);elist1=NULL;}
	if(elist2!=NULL){sds_relrb(elist2);elist2=NULL;}
    IC_RELRB(newent);
	if(SDS_ElevationChanged)
	{
		rblast.restype=RTREAL;
		rblast.resval.rreal=curelev;
		sds_setvar("ELEVATION"/*DNT*/,&rblast);
		SDS_ElevationChanged=FALSE;
	}
    if(rc==0L) return(RTNORM);
    if(rc==(-1L)) return(RTCAN);
    if(rc<(-1L)) CMD_INTERNAL_MSG(rc);
    
	return(RTERROR);
}


// *****************
// COMMAND FUNCTION
//
short cmd_doughnut(void)
{
    if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    return(cmd_donut());
}

// *****************
// COMMAND FUNCTION
//
short cmd_donut(void)
 {
//*** This function draws filled circles and rings.
//*** RETURNS:  0 - Success.
//***          -1 - User cancel.
//***          -2 - RTERROR returned from an ads function.
//*** NOTE: This function still needs to "drag" the donut when prompting
//***       for a center point. ~|
//***

    if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	char fs1[IC_ACADBUF],str[81],str2[81];
    struct resbuf setgetrb,*newent,rb,rb2,*rslt,*elist1,*elist2;
    sds_point pt1,pt2,pt3,pt210,ptemp;
    sds_real wid,dist,insidedia,outsidedia,curelev;
    int ret,got1;
	sds_name ent1,ent2;
    short rc=0;

	newent=rslt=elist1=elist2=NULL;

	//we need circle's extrusion direction before we can make it....
    if(SDS_getvar(NULL,DB_QUCSXDIR,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { rc=(-__LINE__); goto bail; }
	ic_ptcpy(pt1,setgetrb.resval.rpoint);
    if(SDS_getvar(NULL,DB_QUCSYDIR,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { rc=(-__LINE__); goto bail; }
	ic_ptcpy(pt2,setgetrb.resval.rpoint);
	pt210[0]=(pt1[1]*pt2[2])-(pt2[1]*pt1[2]);
	pt210[1]=(pt2[0]*pt1[2])-(pt1[0]*pt2[2]);
	pt210[2]=(pt1[0]*pt2[1])-(pt2[0]*pt1[1]);


	//*** Get the default inside diameter for a donut.
    if(SDS_getvar(NULL,DB_QDONUTID,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { rc=(-2); goto bail; }
    insidedia=setgetrb.resval.rreal;
    if(SDS_getvar(NULL,DB_QDONUTOD,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { rc=(-2); goto bail; }
    outsidedia=setgetrb.resval.rreal;
    sds_rtos(insidedia,-1,-1,str);

    //*** Get the inside diameter.
    for(;;)
	{
		if(sds_initget(0,ResourceString(IDC_CIRCLE_INITGET_2_POINT__27, "~TTR ~RTT ~RadTanTan 2_point|2Point 3_point|3Point Radius_Tangent_Tangent|RTT ~_RadTanTan ~_RadTanTan ~_RadTanTan ~_2Point ~_3Point ~_RadTanTan" ))!=RTNORM)
		{ 
            rc=(-2); 
            goto bail; 
        }
		sprintf(fs1,ResourceString(IDC_CIRCLE__N2POINT_3POINT__28, "\n2Point/3Point/RadTanTan/<Inside diameter of donut> <%s>: " ),str);
		ret=sds_getdist(NULL,fs1,&insidedia);
        if(ret==RTERROR)
		{
            rc=(-2); goto bail;
        }
		else if(ret==RTCAN)
		{
            rc=(-1); goto bail;
        }
		else if(ret==RTNONE)
		{
            break;
        }
		else if(ret==RTNORM)
		{
            //*** Set DONUTID system variable.
            setgetrb.restype=RTREAL;
            setgetrb.resval.rreal=insidedia;
            if(sds_setvar("DONUTID"/*DNT*/,&setgetrb)!=RTNORM) return(-2);
            break;
		}
		else if(ret==RTKWORD)
		{
			//save input for use later.  First, get width
            if(sds_getinput(fs1)!=RTNORM) { rc=(-__LINE__); goto bail; }

			//get the donut's width
			sds_rtos(0.5*fabs(outsidedia-insidedia),-1,-1,str);
			sprintf(str2,ResourceString(IDC_CIRCLE__NWIDTH_OF_DONUT_29, "\nWidth of donut <%s>: " ),str);
			sds_initget(0,NULL);
			ret=sds_getdist(NULL,str2,&wid);
			if(ret!=RTNONE && ret!=RTNORM){rc=-1;goto bail;}
			if(ret==RTNONE)wid=fabs(outsidedia-insidedia)/2.0;

            if(strisame(fs1,"3POINT"/*DNT*/))
			{
                for(;;)
				{
                    if(sds_initget(0,NULL)!=RTNORM)
					{
                       rc=(-__LINE__);
                       goto bail;
                    }
                    if((ret=internalGetpoint(NULL,ResourceString(IDC_CIRCLE__NFIRST_POINT_ON_30, "\nFirst point on donut: " ),pt1))==RTERROR) {
                        rc=(-__LINE__); goto bail;
                    }
					else if(ret==RTCAN)
					{
                        rc=(-1L); goto bail;
                    }
					else if(ret==RTNORM)
					{
                        //*** Get current elevation to check for valid point values.
                        if(SDS_getvar(NULL,DB_QELEVATION,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { rc=(-__LINE__); goto bail; }
                        curelev=setgetrb.resval.rreal;
                        //***
                        break;
                    }
					else
					{
                        cmd_ErrorPrompt(CMD_ERR_PT,0);
                        continue;
                    }
                }
                for(;;)
				{
                    if(sds_initget(0,NULL)!=RTNORM)
					{
                       rc=(-__LINE__);
                       goto bail;
                    }
                    if((ret=internalGetpoint(NULL,ResourceString(IDC_CIRCLE__NSECOND_POINT___6, "\nSecond point: " ),pt2))==RTERROR)
					{
                        rc=(-__LINE__); goto bail;
                    }
					else if(ret==RTCAN)
					{
                        rc=(-1L); goto bail;
                    }
					else if(ret==RTNORM)
					{
                        if(!icadRealEqual(pt2[2],pt1[2]) && !icadRealEqual(pt2[2],curelev))
						{
                            cmd_ErrorPrompt(CMD_ERR_2DPT,0);
                            continue;
                        }
                        //***
                        break;
                    }
					else
					{
                        cmd_ErrorPrompt(CMD_ERR_PT,0);
                        continue;
                    }
                }
                for(;;)
				{
                    if(sds_initget(RSG_OTHER,NULL)!=RTNORM)
					{
                       rc=(-__LINE__);
                       goto bail;
                    }
                    SDS_getvar(NULL,DB_QDRAGMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
                    if((ret=SDS_dragobject(4,rb.resval.rint,pt1,pt2,0L,ResourceString(IDC_CIRCLE__NTHIRD_POINT___7, "\nThird point: " ),&rslt,pt3,NULL))==RTERROR)
					{
                        rc=(-__LINE__); goto bail;
                    }
					else if(ret==RTCAN)
					{
                        rc=(-1L); goto bail;
                    }
					else if(ret==RTNORM)
					{
                        if(!icadRealEqual(pt3[2],pt1[2]) && !icadRealEqual(pt2[2],curelev))
						{
                            cmd_ErrorPrompt(CMD_ERR_2DPT,0);
                            continue;
                        }
						break;
                    }
					else
					{
                        cmd_ErrorPrompt(CMD_ERR_PT,0);
                        continue;
                    }
                }
            }
			else if(strisame(fs1,"2POINT"/*DNT*/))
			{
                for(;;)
				{
                    if(sds_initget(0,NULL)!=RTNORM)
					{
                       rc=(-__LINE__);
                       goto bail;
                    }
                    if((ret=internalGetpoint(NULL,ResourceString(IDC_CIRCLE__NFIRST_POINT_ON_10, "\nFirst point on diameter: " ),pt1))==RTERROR)
					{
                        rc=(-__LINE__); goto bail;
                    }
					else if(ret==RTCAN)
					{
                        rc=(-1L); goto bail;
                    }
					else if(ret==RTNORM)
					{
                        //*** Get current elevation to check for valid point values.
                        if(SDS_getvar(NULL,DB_QELEVATION,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { rc=(-__LINE__); goto bail; }
                        curelev=setgetrb.resval.rreal;
                        break;
                    }
					else
					{
						cmd_ErrorPrompt(CMD_ERR_PT,0);
                        continue;
                    }
                }
                for(;;)
				{
                    if(sds_initget(RSG_OTHER,NULL)!=RTNORM)
					{
                       rc=(-__LINE__);
                       goto bail;
                    }
                    SDS_getvar(NULL,DB_QDRAGMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
                    if((ret=SDS_dragobject(3,rb.resval.rint,pt1,pt2,0L,ResourceString(IDC_CIRCLE__NSECOND_POINT_O_11, "\nSecond point on diameter: " ),&rslt,pt2,NULL))==RTERROR)
					{
                        rc=(-__LINE__); goto bail;
                    }
					else if(ret==RTCAN)
					{
                        rc=(-1L); goto bail;
                    }
					else if(ret==RTNORM)
					{
                        if(!icadRealEqual(pt2[2],pt1[2]) && !icadRealEqual(pt2[2],curelev))
						{
                            cmd_ErrorPrompt(CMD_ERR_2DPT,0);
                            continue;
                        }
						break;
					}
				}
			}
			else if(strisame(fs1,"RADTANTAN"/*DNT*/))
			{
				//this method uses inside & outside diameters rather than
				//	radius to pline of 0 width
				while(1)
				{
					sds_rtos((outsidedia+insidedia)/2.0,-1,-1,str);
					sprintf(fs1,ResourceString(IDC_CIRCLE__NDIAMETER_OF_DO_31, "\nDiameter of donut <%s> : " ),str);
					sds_initget(0,NULL);
					ret=sds_getdist(NULL,fs1,&pt3[0]);
					if(ret==RTNONE)
					{
						//re-calc because width may be different
						insidedia=max((insidedia+outsidedia)/2.0-wid,0.0);
						outsidedia=(insidedia+outsidedia)/2.0+wid;
						if(outsidedia<=0.0)
						{
							cmd_ErrorPrompt(CMD_ERR_POSITIVE,0);
							continue;
						}
						break;
					}
					else if(ret==RTNORM)
					{
						insidedia=pt3[0]-wid;
						outsidedia=pt3[0]+wid;
						break;
					}
					else if(ret==RTCAN)
					{
						rc=(-1L);
						goto bail;
					}
					else
					{
						cmd_ErrorPrompt(CMD_ERR_DIST,0);
						continue;
					}
				}
				get_again:
				got1=0;
                for(;;)
				{
					if(!got1)
						ret=sds_entsel(ResourceString(IDC_CIRCLE__NSELECT_FIRST_T_20, "\nSelect first tangent point: " ),ent1,pt1);
					else
						ret=sds_entsel(ResourceString(IDC_CIRCLE__NSELECT_SECOND__21, "\nSelect second tangent point: " ),ent2,pt2);
                    if(ret==RTERROR)
					{
						SDS_getvar(NULL,DB_QERRNO,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
						if(rb.resval.rint==OL_ENTSELPICK)
						{
							cmd_ErrorPrompt(CMD_ERR_SELECTION,0);
							continue;
						}
						else
						{
	                        rc=(-1L); goto bail;
						}
                    }
					else if(ret==RTCAN)
					{
                        rc=(-1L); goto bail;
                    }
					else if(ret==RTNORM)
					{
						if(got1)
						{
							if(elist2!=NULL){sds_relrb(elist2);elist2=NULL;}
							if((elist2=sds_entget(ent2))==NULL){rc=(-__LINE__);goto bail;}
							ic_assoc(elist2,0);
							if(sds_wcmatch(ic_rbassoc->resval.rstring,"LINE,ARC,CIRCLE,POLYLINE,LWPOLYLINE,TRACE,RAY,XLINE"/*DNT*/)!=RTNORM)
							{
								cmd_ErrorPrompt(CMD_ERR_ENTTYPE,0);
								continue;
							}
							if(strsame(ic_rbassoc->resval.rstring,"POLYLINE"/*DNT*/))
							{
								ic_assoc(elist2,70);
								if(ic_rbassoc->resval.rint & (IC_PLINE_3DPLINE | IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH))
								{
									cmd_ErrorPrompt(CMD_ERR_2DPOLYLINE,0);
									continue;
								}
								ic_assoc(elist1,210);
								if(!icadPointEqual(ic_rbassoc->resval.rpoint,pt210))
								{
									cmd_ErrorPrompt(CMD_ERR_NOTINUCS,0);
									continue;
								}
							}
							else if(strsame(ic_rbassoc->resval.rstring,"LWPOLYLINE"/*DNT*/))
							{
								ic_assoc(elist1,210);
								if(!icadPointEqual(ic_rbassoc->resval.rpoint,pt210))
								{
									cmd_ErrorPrompt(CMD_ERR_NOTINUCS,0);
									continue;
								}
							}
							else if(strsame(ic_rbassoc->resval.rstring,"CIRCLE"/*DNT*/) ||
									 strsame(ic_rbassoc->resval.rstring,"TRACE"/*DNT*/))
							{
								ic_assoc(elist1,210);
								if(!icadPointEqual(ic_rbassoc->resval.rpoint,pt210))
								{
									cmd_ErrorPrompt(CMD_ERR_NOTINUCS,0);
									continue;
								}
							}
							//*** Get current elevation to check for valid point values.
							if(SDS_getvar(NULL,DB_QELEVATION,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { rc=(-__LINE__); goto bail; }
							curelev=setgetrb.resval.rreal;
							break;
						}
						else
						{
							if(elist1!=NULL){sds_relrb(elist1);elist1=NULL;}
							if((elist1=sds_entget(ent1))==NULL){rc=(-__LINE__);goto bail;}
							ic_assoc(elist1,0);
							if(sds_wcmatch(ic_rbassoc->resval.rstring,"LINE,ARC,CIRCLE,POLYLINE,LWPOLYLINE,TRACE,RAY,XLINE"/*DNT*/)!=RTNORM)
							{
								cmd_ErrorPrompt(CMD_ERR_ENTTYPE,0);
								continue;
							}
							if(strsame(ic_rbassoc->resval.rstring,"POLYLINE"/*DNT*/))
							{
								ic_assoc(elist1,70);
								if(ic_rbassoc->resval.rint&(IC_PLINE_3DPLINE | IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH))
								{
									cmd_ErrorPrompt(CMD_ERR_2DPOLYLINE,0);
									continue;
								}
								ic_assoc(elist1,210);
								if(!icadPointEqual(ic_rbassoc->resval.rpoint,pt210))
								{
									cmd_ErrorPrompt(CMD_ERR_NOTINUCS,0);
									continue;
								}
							}
							else if(strsame(ic_rbassoc->resval.rstring,"LWPOLYLINE"/*DNT*/))
							{
								ic_assoc(elist1,210);
								if(!icadPointEqual(ic_rbassoc->resval.rpoint,pt210))
								{
									cmd_ErrorPrompt(CMD_ERR_NOTINUCS,0);
									continue;
								}
							}
							else if(strsame(ic_rbassoc->resval.rstring,"CIRCLE"/*DNT*/) ||
									 strsame(ic_rbassoc->resval.rstring,"TRACE"/*DNT*/))
							{
								ic_assoc(elist1,210);
								if(!icadPointEqual(ic_rbassoc->resval.rpoint,pt210))
								{
									cmd_ErrorPrompt(CMD_ERR_NOTINUCS,0);
									continue;
								}
							}
							got1=1;
							continue;
						}
                    }
					else
					{
                        cmd_ErrorPrompt(CMD_ERR_PT,0);
                        continue;
                    }
                }
				//Now we have both ents(LINE,ARC,CIRCLE,PLINE,XLINE,RAY), their elists,
				//& selected pts. Try to construct a circle from them.
				//notes:-insidedia is really radius specified by user
				//		-add width to radius we pass so that we have outer tangency
				if(cmd_circle_ttr(pt1,ent1,elist1,pt2,ent2,elist2,outsidedia/2.0,pt1)!=RTNORM)
				{
					goto get_again;
				}
				//pt1 is in ecs for entmake
				pt1[2]=curelev;
				//we now have the center as pt1 and the radius as newrad
				IC_RELRB(rslt);
				rslt=sds_buildlist(10,pt1,40,(insidedia+outsidedia)/4.0,0);
				IC_RELRB(elist1);
				IC_RELRB(elist2);
			}	//RadTanTan
            //*** Create entity.
			sds_entmake(NULL);
			IC_RELRB(newent);
			if(SDS_getvar(NULL,DB_QPLINETYPE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { rc=(-__LINE__); goto bail; }
			int plinetype=setgetrb.resval.rint;
			if (plinetype==0)
			{
				newent=sds_buildlist(RTDXF0,"POLYLINE"/*DNT*/,66,1,70,1,40,wid,41,wid,210,pt210,0);
				if((rc=sds_entmake(newent))!=RTNORM)
				{
					cmd_ErrorPrompt(CMD_ERR_CREATEENT,0);
					rc=(-__LINE__); goto bail;
				}
				ic_assoc(rslt,10);
				ic_ptcpy(pt1,ic_rbassoc->resval.rpoint);
				//group 40 contains median radius of donut
				ic_assoc(rslt,40);
				pt3[0]=ic_rbassoc->resval.rreal;
				IC_RELRB(newent);
				pt1[0]-=pt3[0];
				ic_ptcpy(ptemp,pt1);
				newent=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,10,pt1,40,wid,41,wid,42,1.0,70,0,0);
				if((rc=sds_entmake(newent))!=RTNORM) 
				{
					cmd_ErrorPrompt(CMD_ERR_CREATEENT,0);
					rc=(-__LINE__); goto bail;
				}
				pt1[0]+=2.0*pt3[0];
				ic_assoc(newent,10);
				ic_ptcpy(ic_rbassoc->resval.rpoint,pt1);
				if((rc=sds_entmake(newent))!=RTNORM) 
				{
					cmd_ErrorPrompt(CMD_ERR_CREATEENT,0);
					rc=(-__LINE__); goto bail;
				}
				IC_RELRB(newent);
				newent=sds_buildlist(RTDXF0,"SEQEND"/*DNT*/,0);
				if((rc=sds_entmake(newent))!=RTNORM) 
				{
					cmd_ErrorPrompt(CMD_ERR_CREATEENT,0);
					rc=(-__LINE__); goto bail;
				}
			}
			else
			{
				resbuf *trb=newent=sds_buildlist(RTDXF0,"LWPOLYLINE"/*DNT*/,43,wid,210,pt210,90,2,70,1,0);
				while (trb->rbnext!=NULL)
					trb=trb->rbnext;
				ic_assoc(rslt,10);
				ic_ptcpy(pt1,ic_rbassoc->resval.rpoint);
				//group 40 contains median radius of donut
				ic_assoc(rslt,40);
				pt3[0]=ic_rbassoc->resval.rreal;
				pt1[0]-=pt3[0];
				ic_ptcpy(ptemp,pt1);
				trb->rbnext=sds_buildlist(10,pt1,40,wid,41,wid,42,1.0,0);
				while (trb->rbnext!=NULL)
					trb=trb->rbnext;
				pt1[0]+=2.0*pt3[0];
				trb->rbnext=sds_buildlist(10,pt1,40,wid,41,wid,42,1.0,0);
				if((rc=sds_entmake(newent))!=RTNORM) 
				{
					cmd_ErrorPrompt(CMD_ERR_CREATEENT,0);
					rc=(-__LINE__);
					goto bail;
				}
			}
			rb.rbnext=NULL;
			rb.restype=RTREAL;
			rb.resval.rreal=2.0*pt3[0]+wid;
			if(RTNORM!=sds_setvar("DONUTOD"/*DNT*/,&rb)){rc=-1;goto bail;}
			rb.resval.rreal=2.0*pt3[0]-wid;
			if(RTNORM!=sds_setvar("DONUTID"/*DNT*/,&rb)){rc=-1;goto bail;}
			goto bail;	//if using kword method, don't repeat
        }
		else
		{  //end RTKWORD check
			cmd_ErrorPrompt(CMD_ERR_DIST2PTS,0);
            continue;
        }
    }

	//*** Get the default outside diameter for a donut.
	if(SDS_getvar(NULL,DB_QDONUTOD,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { rc=(-2); goto bail; }
	outsidedia=setgetrb.resval.rreal;
    sds_rtos(outsidedia,-1,-1,str);
    sprintf(fs1,ResourceString(IDC_CIRCLE__NOUTSIDE_DIAMET_32, "\nOutside diameter of donut <%s>: " ),str);

    //*** Get the outside diameter.
    for(;;)
	{
        if(sds_initget(RSG_OTHER,NULL)!=RTNORM)
		{
           rc=(-2);
           goto bail;
        }
        if((ret=sds_getdist(NULL,fs1,&outsidedia))==RTERROR)
		{
            rc=(-2); goto bail;
        }
		else if(ret==RTCAN)
		{
            rc=(-1); goto bail;
		}
		else if(ret!=RTNONE && ret!=RTNORM)
		{
            cmd_ErrorPrompt(CMD_ERR_DIST2PTS,0);
            continue;
        }
		break;
    }
	if (ret==RTNORM)
	{
		//*** Set DONUTOD system variable.
		setgetrb.restype=RTREAL;
		setgetrb.resval.rreal=outsidedia;
		if(sds_setvar("DONUTOD"/*DNT*/,&setgetrb)!=RTNORM) return(-2);
	}
	if(outsidedia<insidedia)
	{
		wid=outsidedia;
		outsidedia=insidedia;
		insidedia=wid;
	}

    //*** Get the center point.
    for(;;) 
	{
        pt1[0]=0.0; pt1[1]=0.0; pt1[2]=0.0;
        pt3[0]=insidedia; pt3[1]=outsidedia; pt3[2]=0.0;
        for(;;) 
		{
            SDS_getvar(NULL,DB_QDRAGMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
            if(sds_initget(/*SDS_RSG_NOCANCEL*/0,"`")!=RTNORM) 
			{
				rc=(-2);
				goto bail;
			}
            if((ret=SDS_dragobject(22,rb.resval.rint,pt3,NULL,0.0,ResourceString(IDC_CIRCLE__NCENTER_OF_DONU_33, "\nCenter of donut: " ),NULL,pt1,NULL))==RTERROR)
			{
                rc=(-2);
				goto bail;
			}
			else if (ret==RTCAN)
			{
                rc=(-1);
				goto bail;
			}
			else if (ret==RTNONE) 
			{
                rc=0;
				goto bail;
			}
			else if (ret==RTNORM) 
			{
                //*** Get current elevation to check for valid Z value.
                if(SDS_getvar(NULL,DB_QELEVATION,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { rc=(-2); goto bail; }
                //*** Set the Z value to the current elevation.
                if(icadRealEqual(pt1[2],0.0))
                    pt1[2]=setgetrb.resval.rreal;
                break;
			}
			else 
			{
                cmd_ErrorPrompt(CMD_ERR_2DPT,0);
                continue;
			}
		}
        //*** Create entity.
        wid=((outsidedia/2.0)-(insidedia/2.0));
        dist=((insidedia/2.0)+(wid/2.0));
		rb.restype=RTSHORT;
		rb.resval.rint=1;
		rb2.restype=RT3DPOINT;
		ic_ptcpy(rb2.resval.rpoint,pt210);
		sds_trans(pt1,&rb,&rb2,0,ptemp);

		if(SDS_getvar(NULL,DB_QPLINETYPE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) { rc=(-__LINE__); goto bail; }
		int plinetype=setgetrb.resval.rint;
		if (plinetype==0)
		{
			newent=sds_buildlist(RTDXF0,"POLYLINE"/*DNT*/,66,1,10,ptemp,70,1,40,wid,41,wid,210,pt210,0);
			if(sds_entmake(newent)!=RTNORM)
			{
				cmd_ErrorPrompt(CMD_ERR_CREATEDONUT,0);
				IC_RELRB(newent); { rc=(-2); goto bail; }
			}
			IC_RELRB(newent);
			//*** First vertex.
			sds_polar(pt1,IC_PI,dist,pt2);
			sds_trans(pt2,&rb,&rb2,0,ptemp);
			newent=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,10,ptemp,40,wid,41,wid,42,1.0,0);
			if(sds_entmake(newent)!=RTNORM)
			{
				cmd_ErrorPrompt(CMD_ERR_CREATEDONUT,0);
				IC_RELRB(newent); { rc=(-2); goto bail; }
			}
			IC_RELRB(newent);
			//*** Second vertex.
			sds_polar(pt1,0.0,dist,pt2);
			sds_trans(pt2,&rb,&rb2,0,ptemp);
			newent=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,10,ptemp,40,wid,41,wid,42,1.0,0);
			if(sds_entmake(newent)!=RTNORM) 
			{
				cmd_ErrorPrompt(CMD_ERR_CREATEDONUT,0);
				IC_RELRB(newent); { rc=(-2); goto bail; }
			}
			IC_RELRB(newent);
			//*** Sequence end.
			newent=sds_buildlist(RTDXF0,"SEQEND"/*DNT*/,0);
			if(sds_entmake(newent)!=RTNORM) 
			{
				cmd_ErrorPrompt(CMD_ERR_CREATEDONUT,0);
				IC_RELRB(newent); { rc=(-2); goto bail; }
			}
			IC_RELRB(newent);
		}
		else
		{
			resbuf *trb=newent=sds_buildlist(RTDXF0,"LWPOLYLINE"/*DNT*/,70,1,43,wid,210,pt210,0,90,2,0);
			while (trb->rbnext!=NULL)
				trb=trb->rbnext;
			//*** First vertex.
			sds_polar(pt1,IC_PI,dist,pt2);
			sds_trans(pt2,&rb,&rb2,0,ptemp);
			trb->rbnext=sds_buildlist(10,ptemp,40,wid,41,wid,42,1.0,0);
			while (trb->rbnext!=NULL)
				trb=trb->rbnext;
			//*** Second vertex.
			sds_polar(pt1,0.0,dist,pt2);
			sds_trans(pt2,&rb,&rb2,0,ptemp);
			trb->rbnext=sds_buildlist(10,ptemp,40,wid,41,wid,42,1.0,0);
			if((rc=sds_entmake(newent))!=RTNORM) 
			{
				cmd_ErrorPrompt(CMD_ERR_CREATEENT,0);
				rc=(-__LINE__);
				goto bail;
			}
		}
		IC_RELRB(newent);
	} // end of for

    bail:
	IC_RELRB(newent);
	IC_RELRB(rslt);
	IC_RELRB(elist1);
	IC_RELRB(elist2);
    return(rc);
}

/********************************************************************************
 * Author:	Mohan Nayak.
 *
 * Purpose:	To set the centre point of the circle.
 *
 * Returns:	void
 ********************************************************************************/

void setCenterPoint(sds_point p1, sds_point p2, sds_point p3, sds_point p4,
					sds_point *centrepoint,sds_real *dist,sds_point psel1,sds_point psel2)
					
{

	sds_real fr1;
	sds_point p9;

	if (cmd_linerayxlineray (p1,p2,/*type1*/2,p3,p4,/*type2*/2,p9) == 3)
	{
		fr1 = sds_distance (psel1, p9) + sds_distance(psel2, p9);
		if (*dist == -1.0 || fr1 < *dist)
		{
			ic_ptcpy(*centrepoint, p9);
			*dist = fr1;
		}
	}

}