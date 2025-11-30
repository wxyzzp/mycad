/* 
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Wide PC functions
 * 
 */ 

#include "gr.h"
#include "pc2dispobj.h"
#include "gr_view.h"

extern gr_view gr_defview;


void findanglewidthvec(double angle,sds_real *widthvec, sds_real *anglewidthvec)
{
	sds_real dotprod;
	
	// if we want to use the angle, compute the width vector along the angle
	anglewidthvec[0]=cos(angle)/2.0;
	anglewidthvec[1]=sin(angle)/2.0;
	anglewidthvec[2]=widthvec[2];
// take the dot product to determine whether the startang is in the same direction
// as our width vector, or the opposite.
	dotprod	=	cos(angle)*widthvec[0]
			   +sin(angle)*widthvec[1];
	if (dotprod<0.0) {
		anglewidthvec[0]=-anglewidthvec[0];
		anglewidthvec[1]=-anglewidthvec[1];
	}
}


short gr_usesquare(
    sds_point vert,   /* The original pc chain vertex where the segs join */
    sds_point i0,     /* One of the calculated intersection pts */
    sds_point i1,     /* The other  calculated intersection pt */
    sds_real  wid0,   /* Width on one       side of the junction. */
    sds_real  wid1) { /* Width on the other side of the junction. */
/*
**  An extension of gr_getwidepc().  Determines when to use square-ends
**  at a junction instead of the calculated intersection points.
**
**  See gr_getwidepc() if you need to know more about the pass
**  parameters.
**
**  The current rules -- use square ends if any of the following is true:
**
**      Either width is 0.0 (within fuzz compared to the other).
**
**		The widths are not identical  // MR added
**
**      Either intersection pt is more than twice the largest width
**      away from the pc chain vertex.
**
**      The implied vertex (midpoint of the line connecting the
**      intersection points) is more that half the largest width
**      away from the pc chain vertex.
**
**  Pass parameters not checked for errors; that's up to the caller.
**
**  Returns 1 if square ends should be used and 0 if the intersections
**  should be used.
*/
    sds_real dbigwid,hbigwid;
    sds_point ap1;

	if (wid0!=wid1) return 1;

	if (wid0<=IC_ZRO*wid1 || wid1<=IC_ZRO*wid0) return 1;

    hbigwid=0.25*(dbigwid=2.0*((wid0>=wid1) ? wid0 : wid1));

    if (GR_DIST(i0,vert)>dbigwid || GR_DIST(i1,vert)>dbigwid) return 1;

    ap1[0]=0.5*(i0[0]+i1[0]);
    ap1[1]=0.5*(i0[1]+i1[1]);
    ap1[2]=0.5*(i0[2]+i1[2]);

    if (GR_DIST(ap1,vert)>hbigwid) return 1;

    return 0;
}


short gr_getwidepc(
    struct gr_pclink        *begpclp,
    struct gr_pclink        *endpclp,
    struct gr_pclink        *ilink,
    long                     nintern,
    short                    closed,
    sds_point                dotdir,
    struct gr_wideseglink  **wsapp,
    gr_view          *viewp) {
/*
**  This function is an extension of gr_gensolid() (which is an
**  extension of gr_getdispobjs()).  It generates widths for
**  pc chains with widths.  Note that most of gr_gensolid's
**  pass parameters are passed to THIS function.  See gr_gensolid()
**  for their definitions.
**
**  This functions walks the pc chain and creates and returns an array
**  of struct gr_wideseglinks for the caller (via wsapp).  It creates one
**  element (specifying the four corners) for each segment described by
**  the pc chain.
**
**  wsapp is expected to contain the address of a ptr to a struct
**  gr_wideseglink.
**
**  When possible, smooth joints are constructed between adjacent
**  segments (el and er of one seg coincide with sl and sr of the next).
**  When this is not possible (or not practical), square ends of the
**  appropriate widths are generated.
**
**  gr_gensolid can use the *wsapp array to generate the corresponding
**  display objects.
**
**  The coordinates used in *wsapp are always UCS.
**
**  The number of elements allocated is nintern+2 --  one for every
**  segment of pc chain.
**
**  Parameter dotdir is used for creating a dot with width.
**  It's a vector in the pc chain's system indicating the direction
**  the pc chain was traveling when the dot was generated in gr_doltype().
**  For a dash, begpclp and endpclp will have non-coincident points,
**  indicating direction.  But for a dot, begpclp and entpclp will be
**  the have coincident points, yielding do direction and therefore
**  no way to decide how to add width.  That's where dotdir comes in.
**  gr_doltype DOES know the direction and can pass it to gr_gensolid(),
**  which can pass it here.  We use dotdir only for dots; it can be NULL
**  for dashes.
**
**  Remember: The pc chain is explicitly closed for closed entities.
**
**  CALLER'S RESPONSIBILITY TO FREE *wsapp, OF COURSE.
**
**  Error checking is at a minimum; the caller should make sure that
**  all pass parameters are valid for this function.
**
**  Returns:
**      0 : OK
**     -1 : No memory
*/
    int	  seg0idx,lastseg0idx,ssidx;
	short usesq,rc,fi1;
    sds_real len2[2],begwid,endwid,ar1;
    sds_point ap[10],begncspt,pt0,pt1;
	struct gr_pclink *pclp[3];
    struct gr_wideseglink *wsap;
	sds_point segvec,tempsegvec;
	/*
    **  The following vars are for square ends.  Each holds 4 pts that
    **  represent a segment with square ends (in the NCS):
    */
    struct gr_wideseglink sqend[2],begsqend,endsqend;


    rc=0; wsap=NULL;

    if (viewp==NULL || icadRealEqual(viewp->viewsize,0.0)) viewp=&gr_defview;

    /* Note that nintern can be used as the idx of the last segment. */

    if ((wsap= new struct gr_wideseglink [nintern+1] )==NULL) { rc=-1; goto out; }
	memset(wsap,0,sizeof(struct gr_wideseglink)*(nintern+1));

    if (nintern<1 || ilink==NULL) {  /* One seg; square ends. */
        sds_real wbegewid;

        /* (A dot falls into this category.) */

        /*
        **  Using a working var for begpclp's ewid so we can change
        **  it for a dot without changing the caller's value.
        */
        wbegewid=begpclp->ewid;

        /* In this simple case, it's less trouble to work in the UCS. */

        /* Get the UCS pts: */
        if (viewp->ltsclwblk) 
		{
			gr_ncs2ucs(begpclp->pt,pt0,0,viewp);
			gr_ncs2ucs(endpclp->pt,pt1,0,viewp);
        } 
		else 
		{
            GR_PTCPY(pt0,begpclp->pt);
            GR_PTCPY(pt1,endpclp->pt);
        }

        /* Get the UCS seg vector: */
        tempsegvec[0]=pt1[0]-pt0[0];
        tempsegvec[1]=pt1[1]-pt0[1];
        tempsegvec[2]=pt1[2]-pt0[2];

        gr_ucs2ncs(tempsegvec,segvec,1,viewp);  /* Transform to NCS. */

        /* Get the len: */
        if (icadRealEqual((ar1=GR_LEN(segvec)),0.0) && dotdir!=NULL) {
            /* Oops!  A dot.  Try to use dotdir. */

            /* Get the direction vector in the NCS: */
            if (viewp->ltsclwblk) {
                GR_PTCPY(segvec,dotdir);
            } else {
                gr_ucs2ncs(dotdir,segvec,1,viewp);
            }
            ar1=GR_LEN(segvec);  /* Try again. */

            /*
            **  For a point, we don't want to use begpclp->ewid.
            **  (It's set for the endwidth of the pc segment that
            **  gr_doltype() is currently working on.)  Force
            **  the end width to be equal to the start width.
            **  (We're still producing a trapezoid; gr_gensolid()
            **  will make a flattened rectangle or a flattened
            **  hexahedron.  We just don't want to use begpclp->ewid
            **  for the end line or panel.  Coincident linework
            **  is going to be created, of course.  Can't see going
            **  to great trouble to make a "dot" a special case
            **  at this time.)
            */
            wbegewid=begpclp->swid;
        }

        if (icadRealEqual(ar1,0.0)) {  /* STILL no direction.  Put a dot. */
            GR_PTCPY(wsap[0].sr,pt0); GR_PTCPY(wsap[0].sl,pt0);
            GR_PTCPY(wsap[0].er,pt0); GR_PTCPY(wsap[0].el,pt0);
        } else {
            /* Adjust to 0.5 units long: */
            ar1*=2.0; segvec[0]/=ar1; segvec[1]/=ar1; segvec[2]/=ar1;

            /* Rotate 90 deg CW: */
            ar1=segvec[0]; segvec[0]=segvec[1]; segvec[1]=-ar1;

			sds_point widthvec;
			sds_point anglewidthvec;

			gr_ncs2ucs(segvec,widthvec,1,viewp);	/* Transform back to UCS. */

            /* widthvec is now the UCS vector representing 0.5 units */
            /* of width in the NCS.  Use it to generate the */
            /* 4 corners in the UCS: */
//            for (fi1=0; fi1<3; fi1++) {
//                wsap[0].sr[fi1]=pt0[fi1]+begpclp->swid*widthvec[fi1];
//                wsap[0].sl[fi1]=pt0[fi1]-begpclp->swid*widthvec[fi1];
//                wsap[0].er[fi1]=pt1[fi1]+wbegewid*widthvec[fi1];
//                wsap[0].el[fi1]=pt1[fi1]-wbegewid*widthvec[fi1];
//            }

			sds_real *widthvectouse=widthvec;
			/* widthvec is now the NCS vector representing 0.5 units */
               /* of width.  Use it to generate the 4 square-end corners */
               /* in the NCS: */
			if (begpclp->startang!=IC_ANGLE_DONTCARE) {
				findanglewidthvec(begpclp->startang,widthvec,anglewidthvec);
				widthvectouse=anglewidthvec;
			}
			for (fi1=0; fi1<3; fi1++) {
				wsap[0].sr[fi1]=pt0[fi1]+begpclp->swid*widthvectouse[fi1];
				wsap[0].sl[fi1]=pt0[fi1]-begpclp->swid*widthvectouse[fi1];
	        }

			widthvectouse=widthvec;
			if (begpclp->endang!=IC_ANGLE_DONTCARE) {
				findanglewidthvec(begpclp->endang,widthvec,anglewidthvec);
				widthvectouse=anglewidthvec;
			}
			for (fi1=0; fi1<3; fi1++) {
                wsap[0].er[fi1]=pt1[fi1]+wbegewid*widthvectouse[fi1];
                wsap[0].el[fi1]=pt1[fi1]-wbegewid*widthvectouse[fi1];
	        }
        }

        goto out;
    }

    /* More than 1 seg, if execution gets to here. */

    /*
    **  Walk the pc chain THREE pts at a time and determine the
    **  inner junctions.  Watch out for 0-length segments.
    **  Now it's better to work in the NCS (since we want to do
    **  2D line-line intersections).
    */

    /* For each interior junction -- that is, for each pair of */
    /* adjacent segments (excluding 1st/last): */
    for (seg0idx=0,lastseg0idx=nintern-1; seg0idx<=lastseg0idx; seg0idx++) {
        if (seg0idx==0) { pclp[0]=begpclp; pclp[1]=pclp[2]=ilink; }
        else            { pclp[0]=pclp[1]; pclp[1]=pclp[2]; }
        pclp[2]=(seg0idx>=lastseg0idx) ? endpclp : pclp[2]->next;

        /* For each of the 2 segs being examined: */
        for (ssidx=0; ssidx<2; ssidx++) {

            /* Get the NCS pts for this seg (pt0 and pt1): */
            if (viewp->ltsclwblk) {
                GR_PTCPY(pt0,pclp[ssidx]->pt);
                GR_PTCPY(pt1,pclp[ssidx+1]->pt);
            } else {
                gr_ucs2ncs(pclp[ssidx]->pt  ,pt0,0,viewp);
                gr_ucs2ncs(pclp[ssidx+1]->pt,pt1,0,viewp);
            }
            /* Get the NCS seg vector: */
            segvec[0]=pt1[0]-pt0[0];
            segvec[1]=pt1[1]-pt0[1];
            segvec[2]=pt1[2]-pt0[2];

            len2[ssidx]=2.0*GR_LEN(segvec);  /* Get TWICE its length. */

            /* Set the NCS square-end pts: */
            if (len2[ssidx]<=0.0) {  /* 0-length seg; just take pt0. */
                GR_PTCPY(sqend[ssidx].sr,pt0);
                GR_PTCPY(sqend[ssidx].sl,pt0);
                GR_PTCPY(sqend[ssidx].er,pt0);
                GR_PTCPY(sqend[ssidx].el,pt0);
            } else {
                /* Adjust length to 0.5 units: */
                segvec[0]/=len2[ssidx];
                segvec[1]/=len2[ssidx];
                segvec[2]/=len2[ssidx];

				sds_point widthvec,anglewidthvec;
                /* Rotate 90 deg CW: */
                widthvec[2] =  segvec[2];
				widthvec[1] = -segvec[0];
				widthvec[0] =  segvec[1];

				sds_real *widthvectouse=widthvec;
				/* widthvec is now the NCS vector representing 0.5 units */
                /* of width.  Use it to generate the 4 square-end corners */
                /* in the NCS: */
				if (pclp[ssidx]->startang!=IC_ANGLE_DONTCARE) {
					findanglewidthvec(pclp[ssidx]->startang,widthvec,anglewidthvec);
					widthvectouse=anglewidthvec;
				}
				for (fi1=0; fi1<3; fi1++) {
                    sqend[ssidx].sr[fi1]=pt0[fi1]+pclp[ssidx]->swid*widthvectouse[fi1];
	                sqend[ssidx].sl[fi1]=pt0[fi1]-pclp[ssidx]->swid*widthvectouse[fi1];
		        }

				widthvectouse=widthvec;
				if (pclp[ssidx]->endang!=IC_ANGLE_DONTCARE) {
					findanglewidthvec(pclp[ssidx]->endang,widthvec,anglewidthvec);
					widthvectouse=anglewidthvec;
				}
				for (fi1=0; fi1<3; fi1++) {
                    sqend[ssidx].er[fi1]=pt1[fi1]+pclp[ssidx]->ewid*widthvectouse[fi1];
	                sqend[ssidx].el[fi1]=pt1[fi1]-pclp[ssidx]->ewid*widthvectouse[fi1];
		        }

			}
			/*
            **  Capture data for the terminals.  We will later need the
            **  following for the 1st and last pts in the chain (in the NCS):
            **  the pt; the terminal width; the square-end pts.
            */
            if (seg0idx<1 && ssidx<1) {
                GR_PTCPY(begncspt,pt0);
                begwid=pclp[0]->swid;
                memcpy(&begsqend,sqend,sizeof(begsqend));
            }
            if (seg0idx>=lastseg0idx && ssidx>0) {
                /* (Actually, we don't need an endncspt, since it should */
                /* be coincident with begncspt for a closed chain -- */
                /* which is the only time it would be used. */
                endwid=pclp[1]->ewid;
                memcpy(&endsqend,sqend+1,sizeof(endsqend));
            }

        }  /* End for each of the 2 segs being examined. */

        /* NOTE THAT pt0 IS NOW THE PT THAT JOINS THE 2 SEGS (NCS). */

        /* Now the NCS square-ends (sqend[]) have been generated */
        /* for both segments. */

        /* If either seg is 0-length, take the square ends (at least */
        /* 2 pts of which have been set for 0 width): */

        usesq=0;  /* Flag for when to use the square ends. */

        if (len2[0]<=0.0 || len2[1]<=0.0) usesq=1;
        else {  /* Get the intersections and decide whether to use them. */
            /* Get the right-edge and left-edge intersections: */
            if (ic_linexline(sqend[0].sr,sqend[0].er,
                             sqend[1].sr,sqend[1].er,ap[2])<0 ||
                ic_linexline(sqend[0].sl,sqend[0].el,
                             sqend[1].sl,sqend[1].el,ap[3])<0) usesq=1;
            else usesq=gr_usesquare(pt0,ap[2],ap[3],pclp[0]->ewid,
                pclp[1]->swid);
        }

        if (usesq) 
		{  /* Use the square ends */
			gr_ncs2ucs(sqend[0].er,wsap[seg0idx  ].er,0,viewp);
			gr_ncs2ucs(sqend[0].el,wsap[seg0idx  ].el,0,viewp);
			gr_ncs2ucs(sqend[1].sr,wsap[seg0idx+1].sr,0,viewp);
			gr_ncs2ucs(sqend[1].sl,wsap[seg0idx+1].sl,0,viewp);
        } 
		else 
		{  /* Use the intersections */
			gr_ncs2ucs(ap[2],wsap[seg0idx].er,0,viewp);
			gr_ncs2ucs(ap[3],wsap[seg0idx].el,0,viewp);

            GR_PTCPY(wsap[seg0idx+1].sr,wsap[seg0idx].er);
            GR_PTCPY(wsap[seg0idx+1].sl,wsap[seg0idx].el);
        }

    }  /* End for each pair of adjacent segments (excluding 1st/last). */

    /* We've determined the internal joints and captured the data for */
    /* the ends.  Now determine the ends which may form a joint, */
    /* if closed). */

    usesq=!closed;
    if (!usesq) { /* Get the intersections and decide whether to use them. */
        /* Get the right-edge and left-edge intersections: */
        if (ic_linexline(begsqend.sr,begsqend.er,
                         endsqend.sr,endsqend.er,pt0)<0 ||
            ic_linexline(begsqend.sl,begsqend.el,
                         endsqend.sl,endsqend.el,pt1)<0) usesq=1;
        else usesq=gr_usesquare(begncspt,pt0,pt1,begwid,endwid);
    }

    if (usesq) 
	{  /* NOT AN else! */
		gr_ncs2ucs(begsqend.sr,wsap[0].sr,0,viewp);
		gr_ncs2ucs(begsqend.sl,wsap[0].sl,0,viewp);
		gr_ncs2ucs(endsqend.er,wsap[nintern].er,0,viewp);
		gr_ncs2ucs(endsqend.el,wsap[nintern].el,0,viewp);
    } 
	else 
	{
		gr_ncs2ucs(pt0,wsap[0].sr,0,viewp);
		gr_ncs2ucs(pt1,wsap[0].sl,0,viewp);

        GR_PTCPY(wsap[nintern].er,wsap[0].sr);
        GR_PTCPY(wsap[nintern].el,wsap[0].sl);
    }

out:
    if (rc && wsap!=NULL) { IC_FREE(wsap); wsap=NULL; }
    if (wsapp!=NULL) *wsapp=wsap;
    return rc;
}


