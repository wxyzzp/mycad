/* G:\ICADDEV\PRJ\LIB\GR\GR3.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * functions to determine if entity is selected
 * 
 */ 

#include "gr.h"
#include "constnts.h"
#include "macros.h"

short gr_segxbox(
    sds_real  *p0,
    sds_real  *p1,
    sds_point *box) {
/*
**  An extention of gr_selected().  Determines whether or not the
**  line segment specified by p0 and p1 crosses the box
**  (entent box) specified by "box".  (A seg that is entirely within
**  the box yields TRUE (1).)
**
**  p0 and p1 are sds_real pointers instead of sds_points so that
**  they can point into 2D sources (like the display object chains).
**
**  box must be an array of 2 sds_points, set up like an extent, as follows:
**
**    box[0] : Lower left  corner (low  X and Y)
**    box[1] : Upper right corner (high X and Y)
**
**  2D, of course.  (The Z's in box[0] and box[1] are not used.)
**
**  Returns 1 or 0 (Yes or No); does not give the caller any
**  actual intersection points.
*/
    short xyidx,lhidx;
    sds_real ar1;
    sds_point segext[2],ap1;


    /* Don't waste time checking the pass parameters in an extention */
    /* function like this. */

    /* Get the seg extent: */
    if (p0[0]<=p1[0]) { segext[0][0]=p0[0]; segext[1][0]=p1[0]; }
    else              { segext[0][0]=p1[0]; segext[1][0]=p0[0]; }
    if (p0[1]<=p1[1]) { segext[0][1]=p0[1]; segext[1][1]=p1[1]; }
    else              { segext[0][1]=p1[1]; segext[1][1]=p0[1]; }

    /* If the seg extent is entirely inside... */
    if (segext[0][0]>=box[0][0] && segext[1][0]<=box[1][0] &&
        segext[0][1]>=box[0][1] && segext[1][1]<=box[1][1]) return 1;

    /* If the seg extent is entirely outside... */
    if (segext[0][0]>box[1][0] || segext[1][0]<box[0][0] ||
        segext[0][1]>box[1][1] || segext[1][1]<box[0][1]) return 0;

    ap1[0]=p1[0]-p0[0]; ap1[1]=p1[1]-p0[1];  /* p0 to p1 vector */

    /* Check the 4 sides of box: */
    for (xyidx=0; xyidx<2; xyidx++) {  /* 0:X  1:Y */
        if (icadRealEqual(ap1[xyidx],0.0)) continue;  /* Parallel to wall */
        for (lhidx=0; lhidx<2; lhidx++) {  /* 0:Low  1: High */
            /* Can't hit the wall if the 2 pts are on the same */
            /* side of the wall: */
            if ((p0[xyidx]<box[lhidx][xyidx] &&
                 p1[xyidx]<box[lhidx][xyidx]) ||
                (p0[xyidx]>box[lhidx][xyidx] &&
                 p1[xyidx]>box[lhidx][xyidx])) continue;

            /* Okay, the intersection is on the seg.  Where does it */
            /* hit the wall in the OTHER coord? */
            ar1=(box[lhidx][xyidx]-p0[xyidx])*ap1[!xyidx]/ap1[xyidx]+
                p0[!xyidx];

            /* Is that within the OTHER range? */
            if (ar1>=box[0][!xyidx] && ar1<=box[1][!xyidx]) return 1;
        }
    }

    return 0;
}


short gr_segxseg(
    sds_real *p0,
    sds_real *p1,
    sds_real *p2,
    sds_real *p3) {
/*
**  An extention of gr_selected().  Determines whether or not the
**  line segment specified by p0 and p1 crosses the line segment specified
**  by p2 and p3.
**
**  This should be faster than ic_linxlin() because we are interested
**  in IF the segments cross -- not if and WHERE.
**
**  The points are specified by sds_real pointers instead of sds_points
**  so that they can point into 2D sources (like the display object chains).
**
**  2D, of course.
**
**  Returns 1 or 0 (Yes or No); does not give the caller the
**  actual intersection point.
*/
    sds_real ar1,ar2,dx,dy;


    /* Don't waste time checking the pass parameters in an extention */
    /* function like this. */

    /* If the segs are completely separate, they can't cross: */
    if (p0[0]<=p1[0]) {
        if (p2[0]<=p3[0]) {
            if (p0[0]>p3[0] || p1[0]<p2[0]) return 0;
        } else {
            if (p0[0]>p2[0] || p1[0]<p3[0]) return 0;
        }
    } else {
        if (p2[0]<=p3[0]) {
            if (p1[0]>p3[0] || p0[0]<p2[0]) return 0;
        } else {
            if (p1[0]>p2[0] || p0[0]<p3[0]) return 0;
        }
    }
    if (p0[1]<=p1[1]) {
        if (p2[1]<=p3[1]) {
            if (p0[1]>p3[1] || p1[1]<p2[1]) return 0;
        } else {
            if (p0[1]>p2[1] || p1[1]<p3[1]) return 0;
        }
    } else {
        if (p2[1]<=p3[1]) {
            if (p1[1]>p3[1] || p0[1]<p2[1]) return 0;
        } else {
            if (p1[1]>p2[1] || p0[1]<p3[1]) return 0;
        }
    }


    /* They cross if and only if each seg's points lie on opposite */
    /* sides of the other seg's line (hint: cross products): */

    dx=p3[0]-p2[0]; dy=p3[1]-p2[1]; if (icadRealEqual(dx,0.0) && icadRealEqual(dy,0.0)) return 0;
    ar1=(p0[0]-p2[0])*dy-(p0[1]-p2[1])*dx;
    ar2=(p1[0]-p2[0])*dy-(p1[1]-p2[1])*dx;
    if ((ar1>0.0 && ar2>0.0) || (ar1<0.0 && ar2<0.0)) return 0;

    dx=p1[0]-p0[0]; dy=p1[1]-p0[1]; if (icadRealEqual(dx,0.0) && icadRealEqual(dy,0.0)) return 0;
    ar1=(p2[0]-p0[0])*dy-(p2[1]-p0[1])*dx;
    ar2=(p3[0]-p0[0])*dy-(p3[1]-p0[1])*dx;
    if ((ar1>0.0 && ar2>0.0) || (ar1<0.0 && ar2<0.0)) return 0;

    return 1;
}


GR_API short gr_selected(
    db_handitem *enthip,
    struct gr_selectorlink *sl) {
/*
**  Determines as rapidly as possible whether or not the entity
**  described by enthip is selected by the selector described
**  by sl.
**
**  The entlink MUST have display objects.  If the entity's extent
**  is not set for some reason, the local one gets set here (because
**  we need it in here for the logic).
**
**  Call gr_initselector() first to allocate and set up sl.
**
**  Returns:
**      +1 : Selected
**       0 : Not selected
*/
    short seld,somein,p0idx,p1idx,last0,closed,fi1,fi2;
    int enttype,fint1;
    sds_real ar[2],r2;
    sds_point ap[2],entextent[2];
    struct gr_displayobject *dop,*tdop1;
    struct ic_pointsll *ptsll,*tpp1,*tpp2;


    ptsll=NULL;

    seld=0;  /* Default in case a ruling is never made for some reason. */

    if (enthip==NULL || sl==NULL || sl->def==NULL || sl->extent==NULL)
        { seld=0; goto out; }

    enttype=enthip->ret_type();

    dop=(gr_displayobject *)enthip->ret_disp();  /* For simplicity */
    if (dop==NULL) { seld=0; goto out; }

    /* Init the local copy from the entity: */
    enthip->get_extent(entextent[0],entextent[1]);

    /* If it appears to be unset, set the local one now: */
	//BugZilla No. 78202 ; 17-06-2002
	//Modified fix
	if (icadRealEqual(entextent[0][0],0.0) && 
		icadRealEqual(entextent[0][1],0.0) && 
		icadRealEqual(entextent[0][2],0.0) &&
        icadRealEqual(entextent[1][0],0.0) && 
		icadRealEqual(entextent[1][1],0.0) && 
		icadRealEqual(entextent[1][2],0.0))
	{

        fi1=1;  /* Getting 1st pt */
        for (tdop1=dop; tdop1!=NULL; tdop1=tdop1->next) {
            if (tdop1->type&'\x01' || tdop1->npts<1 ||
                tdop1->chain==NULL) continue;
            last0=2*(tdop1->npts-1);
            for (p0idx=0; p0idx<=last0; p0idx+=2) {
                if (fi1) {
                    entextent[0][0]=entextent[1][0]=tdop1->chain[p0idx];
                    entextent[0][1]=entextent[1][1]=tdop1->chain[p0idx+1];
                    fi1=0;
                } else {
                    if (entextent[0][0]>tdop1->chain[p0idx])
                        entextent[0][0]=tdop1->chain[p0idx];
                    else if (entextent[1][0]<tdop1->chain[p0idx])
                             entextent[1][0]=tdop1->chain[p0idx];
                    if (entextent[0][1]>tdop1->chain[p0idx+1])
                        entextent[0][1]=tdop1->chain[p0idx+1];
                    else if (entextent[1][1]<tdop1->chain[p0idx+1])
                             entextent[1][1]=tdop1->chain[p0idx+1];
                }
            }
        }
    }

    /* If the ent extent and selector extent are separate, definitely OUT: */
    if ((entextent[0][0]>sl->extent[1][0] ||
         entextent[1][0]<sl->extent[0][0] ||
         entextent[0][1]>sl->extent[1][1] ||
         entextent[1][1]<sl->extent[0][1])) {

        seld=(sl->mode=='O'); goto out;
    }

    /*
    **  If the ent extent is completely inside the selector extent,
    **  we don't know anything (except for Window).  However --
    **  for all types except Point -- if the selector extent is
    **  completely inside the ent entent, we can't have Inside:
    */
    if (sl->type!='P' && sl->mode=='I' &&
        sl->extent[0][0]>entextent[0][0] &&
        sl->extent[1][0]<entextent[1][0] &&
        sl->extent[0][1]>entextent[0][1] &&
        sl->extent[1][1]<entextent[1][1]) { seld=0; goto out; }


    switch (sl->type) {

      case 'W':  /* Window; just use the extent (not the def). */

        /* If entextent is completely inside sl->extent, then the */
        /* whole entity is in: */
        // Added FUZZ to fix 1-46167.
        if (((entextent[0][0]>=sl->extent[0][0] || FEQUAL(sl->extent[0][0],entextent[0][0], FUZZ_GEN)) &&
             (entextent[1][0]<=sl->extent[1][0] || FEQUAL(sl->extent[0][0],entextent[0][0], FUZZ_GEN)) &&
             (entextent[0][1]>=sl->extent[0][1] || FEQUAL(sl->extent[0][0],entextent[0][0], FUZZ_GEN)) &&
             (entextent[1][1]<=sl->extent[1][1] || FEQUAL(sl->extent[0][0],entextent[0][0], FUZZ_GEN)) ) ) {

          seld=(sl->mode!='O'); goto out;
        }

        /* That's the ONLY way the whole entity can be in: */
        if (sl->mode=='I') { seld=0; goto out; }

        /* See if even one pt is in: */
        for (tdop1=dop; tdop1!=NULL; tdop1=tdop1->next) {
          if (tdop1->type&'\x01' || tdop1->chain==NULL) continue;
          last0=2*(tdop1->npts-1);
          for (p0idx=0; p0idx<=last0 &&
            (tdop1->chain[p0idx]  <sl->extent[0][0] ||
             tdop1->chain[p0idx]  >sl->extent[1][0] ||
             tdop1->chain[p0idx+1]<sl->extent[0][1] ||
             tdop1->chain[p0idx+1]>sl->extent[1][1]); p0idx+=2);
          if (p0idx<=last0) { seld=(sl->mode=='C'); goto out; }
        }

        /* No pts are in -- but we may have a disp obj segment that cuts */
        /* through the window: */
        for (tdop1=dop; tdop1!=NULL; tdop1=tdop1->next) {  /* Each disp obj */
          if (tdop1->type&'\x01' || tdop1->chain==NULL ||
            tdop1->npts<2) continue;
          closed=((tdop1->type&'\x02')!=0);
          last0=2*(tdop1->npts-2+closed);
          for (p0idx=p1idx=0; p0idx<=last0; p0idx+=2) {  /* Each seg */
            p1idx=(closed && p0idx>=last0) ? 0 : p0idx+2;

            if (gr_segxbox(tdop1->chain+p0idx,tdop1->chain+p1idx,
                sl->extent)) { seld=(sl->mode=='C'); goto out; }
          }
        }

        /* Still here?  No pts inside and no disp obj seg crosses */
        /* an extent seg: */
        seld=(sl->mode=='O');

        break;

      case 'C':  /* Circle */

        r2=sl->def->next->pt[0]*sl->def->next->pt[0];  /* Radius squared */

        /* If ent extent is completely inside the circle, the ent */
        /* is completely inside.  (If the ent extent corners are completely */
        /* outside the circle, we don't know anything: the circle may be */
        /* inside the ent extent.): */
        ap[0][0]=entextent[0][0]-sl->def->pt[0]; ap[0][0]*=ap[0][0];
        ap[0][1]=entextent[0][1]-sl->def->pt[1]; ap[0][1]*=ap[0][1];
        ap[1][0]=entextent[1][0]-sl->def->pt[0]; ap[1][0]*=ap[1][0];
        ap[1][1]=entextent[1][1]-sl->def->pt[1]; ap[1][1]*=ap[1][1];
        if (ap[0][0]+ap[0][1]<=r2 && ap[1][0]+ap[0][1]<=r2 &&
            ap[1][0]+ap[1][1]<=r2 && ap[0][0]+ap[1][1]<=r2)
              { seld=(sl->mode!='O'); goto out; }

        /* Check the disp obj pts; do the in-or-out thing. */
        /* Note that finding one of each (a pt that's in and a pt */
        /* that's out) guarantees that we exit the moment it is */
        /* discovered: */
        somein=0;
        for (tdop1=dop; tdop1!=NULL; tdop1=tdop1->next) {
          if (tdop1->type&'\x01' || tdop1->chain==NULL) continue;
          last0=2*(tdop1->npts-1);
          for (p0idx=0; p0idx<=last0; p0idx+=2) {
            ap[0][0]=tdop1->chain[p0idx]  -sl->def->pt[0];
            ap[0][1]=tdop1->chain[p0idx+1]-sl->def->pt[1];
            if (ap[0][0]*ap[0][0]+ap[0][1]*ap[0][1]<=r2) {
              if (sl->mode=='C') { seld=1; goto out; }
              if (sl->mode=='O') { seld=0; goto out; }
              somein=1;
            } else {
              if (sl->mode=='I') { seld=0; goto out; }
            }
          }
        }
        /* Note that they are either all in or all out at this point. */
        if (somein) { seld=(sl->mode!='O'); goto out; }

        /* Still here?  The disp obj pts are all out. */

        /* Can't have Inside, then: */
        if (sl->mode=='I') { seld=0; goto out; }

        /*
        **  However, a seg may cut through the circle.  For each seg,
        **  find the pt on the seg's line that is closest to the center
        **  of the circle and see if it is on the seg AND inside the
        **  circle:
        */
        for (tdop1=dop; tdop1!=NULL; tdop1=tdop1->next) {  /* Each disp obj */
          if (tdop1->type&'\x01' || tdop1->chain==NULL ||
            tdop1->npts<2) continue;
          closed=((tdop1->type&'\x02')!=0);
          last0=2*(tdop1->npts-2+closed);
          for (p0idx=p1idx=0; p0idx<=last0; p0idx+=2) {  /* Each seg */
            p1idx=(closed && p0idx>=last0) ? 0 : p0idx+2;

            /* First, if the seg's extent is entirely outside of the */
            /* circle's extent, it can't cut the circle: */
            if (tdop1->chain[p0idx]<=tdop1->chain[p1idx]) {
                if (tdop1->chain[p0idx]>sl->extent[1][0] ||
                    tdop1->chain[p1idx]<sl->extent[0][0]) continue;
            } else {
                if (tdop1->chain[p1idx]>sl->extent[1][0] ||
                    tdop1->chain[p0idx]<sl->extent[0][0]) continue;
            }
            if (tdop1->chain[p0idx+1]<=tdop1->chain[p1idx+1]) {
                if (tdop1->chain[p0idx+1]>sl->extent[1][1] ||
                    tdop1->chain[p1idx+1]<sl->extent[0][1]) continue;
            } else {
                if (tdop1->chain[p1idx+1]>sl->extent[1][1] ||
                    tdop1->chain[p0idx+1]<sl->extent[0][1]) continue;
            }

            /* OK.  Do it the hard way: */

            /* Get unit vector from p0 to p1 (and keep the length): */
            ap[0][0]=tdop1->chain[p1idx]  -tdop1->chain[p0idx];
            ap[0][1]=tdop1->chain[p1idx+1]-tdop1->chain[p0idx+1];
            if (icadRealEqual((ar[0]=sqrt(ap[0][0]*ap[0][0]+ap[0][1]*ap[0][1])),0.0))
                continue;
            ap[0][0]/=ar[0]; ap[0][1]/=ar[0];

            /* Get the directed length of the projection of the vector */
            /* from p0 to the circle's center onto line p0p1: */
            ar[1]=(sl->def->pt[0]-tdop1->chain[p0idx])  *ap[0][0]+
                  (sl->def->pt[1]-tdop1->chain[p0idx+1])*ap[0][1];

            /* Does it specify a pt on the seg?: */
            if (ar[1]<0.0 || ar[1]>ar[0]) continue;  /* Nope */

            /* It's on the seg. */      
            /* Generate the pt (which is the pt on the seg closest */
            /* to the center of the circle) and see if it's inside: */
            ap[1][0]=tdop1->chain[p0idx]  +ar[1]*ap[0][0];
            ap[1][1]=tdop1->chain[p0idx+1]+ar[1]*ap[0][1];
            ap[0][0]=ap[1][0]-sl->def->pt[0];
            ap[0][1]=ap[1][1]-sl->def->pt[1];
            if (ap[0][0]*ap[0][0]+ap[0][1]*ap[0][1]<=r2)
                { seld=(sl->mode=='C'); goto out; }
          }
        }

        /* Still here?  Completely out: */
        seld=(sl->mode=='O');

        break;

      case 'G':  /* polyGon */

        /* Check the disp obj pts; do the in-or-out thing. */
        /* Note that finding one of each (a pt that's in and a pt */
        /* that's out) guarantees that we exit the moment it is */
        /* discovered: */
        somein=0;
        for (tdop1=dop; tdop1!=NULL; tdop1=tdop1->next) {
          if (tdop1->type&'\x01' || tdop1->chain==NULL) continue;
          last0=2*(tdop1->npts-1);
          for (p0idx=0; p0idx<=last0; p0idx+=2) {
            if (ic_inorout(sl->def,sl->extent[0][0],sl->extent[0][1],
              sl->extent[1][0],sl->extent[1][1],
              tdop1->chain[p0idx],tdop1->chain[p0idx+1])) {

              if (sl->mode=='C') { seld=1; goto out; }
              if (sl->mode=='O') { seld=0; goto out; }
              somein=1;
            } else {
              if (sl->mode=='I') { seld=0; goto out; }
            }
          }
        }
        /* Note that they are either all in or all out at this point: */
        if (somein) {
          /* If all in, it's still possible for the ent SEGS to cross */
          /* this kind of selector, so we can't conclude the entity is In: */
          if (sl->mode=='C') { seld=1; goto out; }
          if (sl->mode=='O') { seld=0; goto out; }
        } else {
          /* If they're all out, can't have Inside: */
          if (sl->mode=='I') { seld=0; goto out; }
        }

        /* Now they're either all in and we're looking for Inside, */
        /* or all out and we're looking for Crossing or Outside. */
        /* Either way, we need to know if any disp obj segs actually */
        /* cross a selector seg: */

        for (tpp1=sl->def; tpp1!=NULL; tpp1=tpp1->next) {  /* Each selector seg */
          tpp2=(tpp1->next==NULL) ? sl->def : tpp1->next;

          /* If the sel seg doesn't cross the ent extent box, we can't */
          /* have a crossing: */
          if (!gr_segxbox(tpp1->pt,tpp2->pt,entextent)) continue;

          /* Check each disp obj seg against the sel seg: */
          for (tdop1=dop; tdop1!=NULL; tdop1=tdop1->next) {  /* Each disp obj */
            if (tdop1->type&'\x01' || tdop1->chain==NULL ||
              tdop1->npts<2) continue;
            closed=((tdop1->type&'\x02')!=0);
            last0=2*(tdop1->npts-2+closed);
            for (p0idx=p1idx=0; p0idx<=last0; p0idx+=2) {  /* Each seg */
              p1idx=(closed && p0idx>=last0) ? 0 : p0idx+2;
              if (gr_segxseg(tpp1->pt,tpp2->pt,
                tdop1->chain+p0idx,tdop1->chain+p1idx))
                  { seld=(sl->mode=='C'); goto out; }
            }
          }
        }

        /* Still here?  No actual crossing.  So, with what we know from */
        /* the comment above the "for" loop... */
        seld=(somein || sl->mode=='O');

        break;

      case 'F':  /* Fence */

        for (tpp1=sl->def; tpp1->next!=NULL; tpp1=tpp1->next) {  /* Each selector seg */

          /* If the sl seg doesn't cross through the ent extent box, */
          /* it can't cross the ent: */
          if (!gr_segxbox(tpp1->pt,tpp1->next->pt,entextent)) continue;

          /* Check each disp obj seg: */
          for (tdop1=dop; tdop1!=NULL; tdop1=tdop1->next) {  /* Each disp obj */
            if (tdop1->type&'\x01' || tdop1->chain==NULL ||
              tdop1->npts<2) continue;
            closed=((tdop1->type&'\x02')!=0);
            last0=2*(tdop1->npts-2+closed);
            for (p0idx=p1idx=0; p0idx<=last0; p0idx+=2) {  /* Each seg */
              p1idx=(closed && p0idx>=last0) ? 0 : p0idx+2;
              if (gr_segxseg(tpp1->pt,tpp1->next->pt,
                tdop1->chain+p0idx,tdop1->chain+p1idx))
                  { seld=(sl->mode!='O'); goto out; }
            }
          }
        }

        /* Still here?  Completely out (not crossing): */
        seld=(sl->mode=='O');

        break;

      case 'P':  /* Point */
        /*
        **  For now, we're requiring a closed entity that has just
        **  one closed display object.  That means that entities whose
        **  display objects show non-solid linetypes or thicknesses are
        **  always considered Outside.
        */
		
		/*D.G.*/// Support for mtext added.

        fint1=0;  /* Closed, non-mesh POLYLINE? */
        if (enttype==DB_LWPOLYLINE)
		{
			enthip->get_70(&fint1); fint1=((fint1&0x01)==1);
		}
		if(enttype==DB_POLYLINE)
        {
			enthip->get_70(&fint1); fint1=((fint1&0x51)==1);
		}
        if ((dop->next == NULL || enttype == DB_MTEXT) &&
			(dop->type&'\x02') && dop->npts>2 &&
            !(dop->type&'\x01') && dop->chain!=NULL &&
           (fint1              ||
            enttype==DB_MTEXT  ||
            enttype==DB_CIRCLE ||
            enttype==DB_TRACE  ||
            enttype==DB_SOLID  ||
            enttype==DB_3DFACE ||
            enttype==DB_ELLIPSE)) {  /* Closed entity */

	        /*D.G.*/// Use a cycle for mtext.
			for (tdop1 = dop; tdop1 && !seld; tdop1 = tdop1->next)
			{
				/* Allocate ptsll for ic_inorout() ("No memory" shows up as */
				/* "not selected".)  Note that since we know the number of */
				/* links at allocation time, we can do ONE allocation and */
				/* use it as an array here, but link it up for ic_inorout(): */
				if (ptsll!=NULL) { delete ptsll; ptsll=NULL; }
				if ((ptsll= new struct ic_pointsll [dop->npts] )==NULL) goto out;
				memset(ptsll,0,sizeof(struct ic_pointsll)* dop->npts);

				/* Fill ptsll from the disp obj (and link it): */
				for (fi1=fi2=0; fi1<tdop1->npts; fi1++,fi2+=2) {
					ptsll[fi1].pt[0]=tdop1->chain[fi2];
					ptsll[fi1].pt[1]=tdop1->chain[fi2+1];
					if (fi1) ptsll[fi1-1].next=ptsll+fi1;
				}

				fi1=ic_inorout(ptsll,entextent[0][0],entextent[0][1],
					entextent[1][0],entextent[1][1],sl->def->pt[0],
					sl->def->pt[1]);

				seld=(fi1==+1) ? (sl->mode!='O') : (sl->mode=='O');
			}

        } else {  /* Open entity, so OUT. */
          seld=(sl->mode=='O');
        }

        break;

    }  /* End switch (sl->type) */


out:
    if (ptsll!=NULL) delete ptsll;

    return seld;
}



