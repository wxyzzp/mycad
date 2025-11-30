/* 
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * functions that help generate arcs including circles and bulging polylines
 * 
 */ 

#include "gr.h"
#include "gr_view.h"
#include "pc2dispobj.h"

short gr_arc2pc(
    short    mode,
    sds_real r0,
    sds_real r1,
    sds_real r2,
    sds_real r3,
    sds_real r4,
    sds_real elev,
    short    cat,
    sds_real swid,
    sds_real ewid,
    short    ltsclwblk,
    sds_real pixsz,
    int      curvdispqual,
    gr_view *viewp) {
/*
**  An extension of gr_getdispobjs() and gr_textbox().
**  Uses viewp -- but just the pc chain members, not the values
**  set up by gr_getdispobjs() (since gr_textbox() never calls
**  that function).
**
**  Sets up viewp->pc and the related viewp values for an arc or
**  circle.
**
**  The five sds_real r? variables describe the arc or circle; variable
**  "mode" tells how:
**
**  mode     r0         r1        r2        r3        r4       description
**  ----  --------   --------   ------   ---------  -------  ----------------
**    0   center X   center Y   radius   start ang  end ang  (CCW arc/circle)
**    1     pt 0 X     pt 0 Y   pt 1 X   pt 1 Y     bulge    (Arc)
**    2   center X   center Y   radius       -         -     (Circle)
**
**  Angles are in radians.
**
**  The coordinates are expected to be ECS for the ARC, CIRCLE, or
**  2D POLYLINE involved (although we trick this function into doing
**  text font arcs in font vector units).
**
**  Variable elev is the elevation of the entity in its ECS.
**
**  When cat is 0, this function builds viewp->pc from the beginning.
**
**  When cat is 1, this function adds new chords to the end of
**  viewp->pc (via viewp->lupc) for POLYLINEs and TEXT.  WHEN
**  CONCATENATING, THIS FUNCTION DOES NOT STEP viewp->lupc TO THE NEXT
**  LINK FIRST. THE CODE GENERATING THE POLYLINE MUST DO IT, IF
**  APPROPRIATE.  It begins filling the viewp->pc llist at
**  viewp->lupc.
**
**  In all successful cases, this function leaves viewp->lupc pointing
**  at the last link it fills.
**
**  BE VERY CAREFUL WITH CALLS TO allocatePointsChain() IN HERE SO THAT
**  viewp->upc REMAINS CORRECT.  WHEN cat!=0, NOTE THAT WE'RE
**  FILLING THE CURRENT viewp->lupc AND WE DON'T NEED TO ACCOUNT
**  FOR IT IN THE allocatePointsChain() CALL.
**
**  swid and ewid are the starting and ending widths (NCS) for the whole
**  arc. The swid and ewid members in viewp->pc are calculated
**  (tapered) from the passed values.
**
**  ltsclwblk and pixsz were originally obtained from viewp (pixsz
**  came from an adjusted viewp->PixelHeight).  However, that assumes
**  that gr_getdispobjs() was called to set up viewp.  That's not
**  true when gr_textbox() is called (the chain is
**  gr_textbox()/gr_textgen()/gr_arc2pc()). So, I made them pass
**  parameters.  That way, gr_textgen() can pass them appropriately for
**  the different types of calls: ARC/CIRCLE/POINT, normal text
**  generation, and textbox determination.  For the same reason,
**  curvdispqual is a pass parameter, too (even though viewp has one, too).
**
**      ltsclwblk flags that linetypes scale with blocks; it's a
**      question of when to transform from NCS to UCS in here.
**
**      pixsz is the height of a pixel (positive) in whatever units the
**      arc is using during the current call (r0-4) (for resolution
**      purposes).
**
**      curvdispqual is our "circle zoom percent", used in determining
**      resolution.  See the resolution discussion below.
**
**  Note that this function puts the sum of the chords into the .d2go
**  of the first viewp->pc link filled and -1.0 into all links
**  thereafter.  This is fine for ARC, CIRCLE, and a 2D POLYLINE that
**  resets pattern generation with each segment. For a 2D POLYLINE with
**  continuous generation, the caller must add that first .d2go to the
**  one for the first link of the POLYLINE, and then set the current
**  segment's .d2go to -1.0 (unless this IS the 1st seg) so that it
**  looks like any other internal chord.  gr_doltype() looks for
**  non-neg .d2go's when deciding where to reset pattern generation.
**
**  To determine the resolution I experimented with ACAD.  At pixel
**  resolution 613x302, I found that for a circle, the number of
**  segments n followed n=5.6*sqrt(c*R/v), where v is VIEWSIZE at
**  regen time, c is "circle zoom percent" (our curvdispqual), and R
**  is the radius of the circle .  (I used a unit circle.)  n has a
**  minimum value of 8 but no apparent maximum (although we are going
**  to impose one, since we don't have a virtual screen).
**  Let r be the radius in pixels and s be the screen height in pixels.
**  Since R/v=r/s, n=5.6*sqrt(c*r/s)=5.6/sqrt(s)*sqrt(c*r).
**  Since s=302,
**
**    n=0.3222*sqrt(c*r)
**
**  This is the formula applied in this function.  ACAD ssems to make
**  sure that full circles always have vertices and the quadrant points
**  by making sure that n is a multiple of 4.  We follow suit.
**
**  Returns: gr_getdispobjs() codes.
*/
    int nchords,minnchords,maxnchords,cw,fi1;
	short rc;
    sds_real iang,dang,sn,cs,dx,dy,fract,dwid,ar1;
    sds_point ap[3];
    struct gr_pclink *dlink;
	sds_real arcstartang=0.0,arcendang=0.0;
	sds_real desiredang=0.0, desiredangstep=0.0;

    cw=rc=0;


    if (icadRealEqual(pixsz,0.0))
		{ 
		rc=-1; 
		goto out; 
		}
    if (pixsz<0.0) 
		{
		pixsz=-pixsz;
		}


    ap[0][2]=ap[1][2]=elev;  /* Just need to set these once. */

    if ((viewp->isprinting) && (curvdispqual < 1000))
		{
		curvdispqual=1000;  /* Force high res for printing. */
		}

    /* Make SURE we have a legit curvdispqual: */
    if (curvdispqual<1) 
		curvdispqual=1;
    else if (curvdispqual>20000) 
		curvdispqual=20000;


	maxnchords=400;  /* Max # of chords per arc or circle. */
	if ( curvdispqual > 1000 )
		{
		maxnchords *= (curvdispqual / 1000 );
		}

    /* Convert to CCW arc definition: */
    if (mode==2) {
        r3=0.0; r4=IC_TWOPI;
    } else {
        if (mode==1) {
            if (r4<0.0) cw=1;  /* CW arc seg from 2-pts-bulge def. */
            ap[0][0]=r0; ap[0][1]=r1; ap[1][0]=r2; ap[1][1]=r3;
            if ((fi1=ic_bulge2arc(ap[0],ap[1],r4,ap[2],&r2,&r3,&r4))==0) {
                r0=ap[2][0]; r1=ap[2][1];
				if (cw) {	// set proper start and end angles for arc, so that
							// we can use them to line up the start/end pie slices properly
					arcstartang=r4;
					arcendang=r3;
				}
				else {
					arcstartang=r3;
					arcendang=r4;
				}
			} else {
                if (fi1<0) r2=0.0;  /* Coincident pts; force a dot */
                else {  /* A line */
                    if (cat) {
                        if (viewp->allocatePointsChain(viewp->upc+1)) 
						{ 
							rc = -3; 
							goto out; 
						}
                        /* Leave viewp->lupc where it is. */
                    } else {
                        if (viewp->allocatePointsChain(2)) 
						{ 
							rc = -3; 
							goto out; 
						}
                        viewp->lupc=viewp->pc;
                    }
                    viewp->lupc->d2go=sqrt((r2-r0)*(r2-r0)+(r3-r1)*(r3-r1));
                    viewp->lupc->swid=swid;
                    viewp->lupc->ewid=ewid;
					viewp->lupc->startang=viewp->lupc->endang=IC_ANGLE_DONTCARE;
                    if (ltsclwblk) {
                        GR_PTCPY(viewp->lupc->pt,ap[0]);
                    } else {
                        gr_ncs2ucs(ap[0],viewp->lupc->pt,0,viewp);
                    }
                    viewp->lupc=viewp->lupc->next;
                    viewp->lupc->d2go=-1.0;
                    viewp->lupc->swid=ewid;
                    viewp->lupc->ewid=ewid;
					viewp->lupc->startang=viewp->lupc->endang=IC_ANGLE_DONTCARE;
                    if (ltsclwblk) {
                        GR_PTCPY(viewp->lupc->pt,ap[1]);
                    } else {
                        gr_ncs2ucs(ap[1],viewp->lupc->pt,0,viewp);
                    }

                    goto out;
                }
            }
        }
        ic_normang(&r3,&r4);
    }

	iang=r4-r3;

    if (r2<0.0) r2=-r2;

    /* Make the widths <= diameter: */
    ar1=2.0*r2;
    if (swid>ar1) swid=ar1;
    if (ewid>ar1) ewid=ar1;


    ar1=0.1*pixsz;
    if (iang*r2<ar1) {  /* Just a dot. */

        ap[0][0]=r0;
        ap[0][1]=r1;
        if (r2>=ar1) {
            ap[0][0]+=r2*cos(r3);
            ap[0][1]+=r2*sin(r3);
        }

        /*
        **  If !cat, just make one link in viewp->pc.
        **  Otherwise (POLYLINE), make it a 0-length chord so we
        **  don't mess up the segment-chaining logic in gr_getdispobjs().
        */
        if (cat) {
            if (viewp->allocatePointsChain(viewp->upc+1)) 
			{ 
				rc = -3; 
				goto out; 
			}
            /* Leave viewp->lupc where it is. */
        } else {
            if (viewp->allocatePointsChain(1)) 
			{ 
				rc = -3; 
				goto out; 
			}
            viewp->lupc=viewp->pc;
        }
        viewp->lupc->d2go=0.0;
        viewp->lupc->swid=swid;
        viewp->lupc->ewid=ewid;
//		viewp->lupc->startang=viewp->lupc->endang=IC_ANGLE_DONTCARE;
        if (ltsclwblk) {
            GR_PTCPY(viewp->lupc->pt,ap[0]);
        } else {
            gr_ncs2ucs(ap[0],viewp->lupc->pt,0,viewp);
        }
        if (cat) {
            viewp->lupc->next->d2go=-1.0;
            viewp->lupc->next->swid=swid;
            viewp->lupc->next->ewid=ewid;
			viewp->lupc->next->startang=viewp->lupc->next->endang=IC_ANGLE_DONTCARE;
            GR_PTCPY(viewp->lupc->next->pt,viewp->lupc->pt);
            viewp->lupc=viewp->lupc->next;
        }

    } else {

        /* Determine number of chords (see comments above): */

        ar1=0.3222*sqrt(curvdispqual*r2/pixsz);  /* For a full circle */

        /* Get minnchords, adjust for partial circle, and impose max and min: */
        fract=iang/IC_TWOPI;
        if ((minnchords=(short)(8.99*fract))<1) minnchords=1;
        ar1*=fract;  /* How many for this arc or circle. */

        if (ar1<minnchords) ar1=minnchords;
        else if (ar1>maxnchords) ar1=maxnchords;
        nchords=(short)ar1;
        /* For a full circle, use multiple of 4: */
        if (fabs(fract-1.0)<IC_ZRO && (fi1=nchords%4)!=0) nchords+=4-fi1;

        if (cat) {
            if (viewp->allocatePointsChain(viewp->upc+nchords)) 
			{ 
				rc = -3; 
				goto out; 
			}
            /* Leave viewp->lupc where it is. */
        } else {
            if (viewp->allocatePointsChain(nchords+1)) 
			{ 
				rc = -3; 
				goto out; 
			}
            viewp->lupc=viewp->pc;
        }

        dlink=viewp->lupc;  /* The initial link (where d2go for the */
                              /* whole arc is kept). */

        dwid=(ewid-swid)/nchords;  /* Change in width during each chord. */


        /*
        **  We want to draw CW arcs for 2-pt-bulge definitions with
        **  negative bulges.  Set the direction sensitive stuff:
        */
        if (cw) {
            ap[0][0]=r0+r2*cos(r4);
            ap[0][1]=r1+r2*sin(r4);

            dang=-iang/nchords;

            /* Determine end of 1st chord to get  1st dx and dy, */
            /* the length of a chord, and the sum of the chords: */
            dx=r0+r2*cos(r4+dang)-ap[0][0];
            dy=r1+r2*sin(r4+dang)-ap[0][1];
        } else {
            double cosr3=cos(r3);
			double sinr3=sin(r3);
			ap[0][0]=r0+r2*cosr3;
            ap[0][1]=r1+r2*sinr3;

            dang=iang/nchords;

            /* Determine end of 1st chord to get  1st dx and dy, */
            /* the length of a chord, and the sum of the chords: */
            dx=r0+r2*cos(r3+dang)-ap[0][0];
            dy=r1+r2*sin(r3+dang)-ap[0][1];
        }


        /* SET THE VALUES FOR THE 1ST PT BEFORE ENTERING THE LOOP: */
		viewp->lupc->swid=swid;
        viewp->lupc->ewid=swid+dwid;
// set the desired start angle for the first pie slice of the arc here
		desiredangstep=iang/nchords;
		if (cw) desiredangstep=-desiredangstep;
		viewp->lupc->startang=desiredang=arcstartang;
		desiredang+=desiredangstep;
		viewp->lupc->endang=desiredang;

        if (ltsclwblk) {
            GR_PTCPY(viewp->lupc->pt,ap[0]);
            dlink->d2go=nchords*sqrt(dx*dx+dy*dy);  /* Set it now. */
        } else {
            gr_ncs2ucs(ap[0],viewp->lupc->pt,0,viewp);
            dlink->d2go=0.0;  /* Init for summation. */
        }

        cs=cos(dang); sn=sin(dang);


		/* THE LOOP: */

        /* Do the endpoint of each chord.  Note that the last point */
        /* will have a bogus ewid, but it will either get buried */
        /* by the next POLYLINE seg or never be used. */
        for (fi1=0; fi1<nchords; fi1++,viewp->lupc=viewp->lupc->next) {
            /* Note the "fill the next and step to it" method here.  */
            /* When we're done, viewp->lupc will point to the last */
            /* one filled: */

            ap[1][0]=ap[0][0]+dx;
            ap[1][1]=ap[0][1]+dy;
            if (ltsclwblk) {
                GR_PTCPY(viewp->lupc->next->pt,ap[1]);
            } else {
                gr_ncs2ucs(ap[1],viewp->lupc->next->pt,0,viewp);
                dlink->d2go+=
                    GR_DIST(viewp->lupc->pt,viewp->lupc->next->pt);
            }
            viewp->lupc->next->d2go=-1.0;
            viewp->lupc->next->swid=viewp->lupc->ewid;
            viewp->lupc->next->ewid=viewp->lupc->next->swid+dwid;
			viewp->lupc->next->startang=desiredang;
// step the desired angle
			desiredang+=desiredangstep;
			viewp->lupc->next->endang=desiredang;
            ar1=dx*cs-dy*sn; dy=dy*cs+dx*sn; dx=ar1;
            GR_PTCPY(ap[0],ap[1]);
        }

    } /* End else not just a dot (actually making the chords). */

out:
    return rc;
}


