/* D:\ICADDEV\PRJ\LIB\GR\RAYXLINEGEN.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Code needed to generate rays and xlines
 * 
 */ 

#include "gr.h"
#include "gr_view.h"

GR_API short gr_rayxlinegenpts(
    sds_point     p0,     /* Def pt (10) */
    sds_point     vect,   /* Direction vector (11) */
    short         type,   /* DB_RAY or DB_XLINE */
    sds_point     res0,
    sds_point     res1,
    gr_view     *viewp,
    db_drawing   *dp) {
/*
**  This function is an extension of gr_getdispobjs() for RAY and XLINE.
**  All points and the vector are WCS.
**
**  Given a point p0 and direction vector vect, this function sets
**  points res0 and res1 to WCS points that can be used to generate
**  the display object for this entity.  For an XLINE, this
**  is done by finding the point on the WCS line that projects nearest
**  to the center of the screen and determining the points that project
**  one screen diagonal away from it in each direction.  For a RAY,
**  we add the further restriction that neither point can
**  lie in the -vect direction from p0.
**
**  If vect is parallel to VIEWDIR, the res pts are set 1 unit away from
**  p0 (and project on top of it, of course).
**
**  viewp must point to a gr_view struct with the current view data and
**  xfp[] axes (that have been set up by gr_initview() and
**  gr_getdispobjs()).
**
**  Either res pt can be passed as NULL if it is not wanted.
**
**  (Remember that the RP is the XY-plane of the DCS.)
**
**  Returns:
**      +1 : Completely off screen; don't bother generating.
**       0 : OK
**      -1 : Calling error (p0, vect, or viewp is NULL)
**      -2 : vect is the zero vector -- undetermined direction
*/
    short rc;
    sds_real ar[3],ext,rplen,rpscrdiag,nearsrd;
    sds_point res[2],uv,ap[6];
    struct resbuf ucs,dcs;


    rc=0;

    if (p0==NULL || vect==NULL || viewp==NULL) { rc=-1; goto out; }

    ucs.rbnext =dcs.rbnext=NULL;
    ucs.restype=dcs.restype=RTSHORT;
    ucs.resval.rint=1; dcs.resval.rint=2;


    /* Get the length of the screen diagonal in the RP: */
    ar[0]=(sds_real)viewp->midwin[0];
    ar[1]=(sds_real)viewp->midwin[1];
    rpscrdiag=2.0*sqrt(ar[0]*ar[0]+ar[1]*ar[1])*fabs(viewp->GetPixelHeight());

    /* Get the WCS unit vector: */
    GR_PTCPY(uv,vect);
    if (icadRealEqual((ar[0]=GR_LEN(uv)),0.0)) { rc=-2; goto out; }
    GR_UNITIZE(uv,ar[0]);

    /* Get the pt 1 unit away from p0 in vect's direction and transform */
    /* it and p0 to the DCS: */
    ap[0][0]=p0[0]+uv[0]; ap[0][1]=p0[1]+uv[1]; ap[0][2]=p0[2]+uv[2];
    gr_ncs2ucs(p0   ,ap[1],0,viewp);
    gr_ncs2ucs(ap[0],ap[2],0,viewp);
    if (dp!=NULL) {
        dp->trans(ap[1],&ucs,&dcs,0,ap[1],NULL);
        dp->trans(ap[2],&ucs,&dcs,0,ap[2],NULL);
    }

    /* Get the DCS direction vector (not unit): */
    ap[3][0]=ap[2][0]-ap[1][0];
    ap[3][1]=ap[2][1]-ap[1][1];
    ap[3][2]=ap[2][2]-ap[1][2];

    /* Get the length of its projection on the RP plane: */
    if (icadRealEqual((rplen=sqrt(ap[3][0]*ap[3][0]+ap[3][1]*ap[3][1])),0.0)) {
        /* VIEWDIR and vect are parallel; just generate pts 1 unit from p0: */
        res[0][0]=p0[0]-uv[0]; res[0][1]=p0[1]-uv[1]; res[0][2]=p0[2]-uv[2];
        res[1][0]=p0[0]+uv[0]; res[1][1]=p0[1]+uv[1]; res[1][2]=p0[2]+uv[2];
        goto out;
    }

    /*
    **  Form the RP vector from the def pt to viewp->CenterProjection and find
    **  the directed length of its projection onto the RP direction
    **  vector:
    */
    ar[1]=(viewp->GetCenterProjectionX()-ap[1][0])*ap[3][0]/rplen+
          (viewp->GetCenterProjectionY()-ap[1][1])*ap[3][1]/rplen;

    /* Find the signed relative distance from the 1st pt to the 2nd that */
    /* this represents: */
    nearsrd=ar[1]/rplen;

    /* Use this factor to find the pt on the DCS line that projects */
    /* on the RP closest to view->CenterProjection: */
    ap[4][0]=ap[1][0]+nearsrd*ap[3][0];
    ap[4][1]=ap[1][1]+nearsrd*ap[3][1];
    ap[4][2]=ap[1][2]+nearsrd*ap[3][2];

    /*
    **  If this pt is more than half a screen diagonal from viewp->CenterProjection
    **  on the RP, we don't need to generate this entity.  For safety,
    **  add a little extra: compare the square of the distance with
    **  0.5*rpscrdiag*rpscrdiag instead of 0.25*...; this will use
    **  sqrt(2.0)/2.0 (0.707) screen-diagonals.
    */
    if ((ap[4][0]-viewp->GetCenterProjectionX())*(ap[4][0]-viewp->GetCenterProjectionX())+
        (ap[4][1]-viewp->GetCenterProjectionY())*(ap[4][1]-viewp->GetCenterProjectionY())>
        0.5*rpscrdiag*rpscrdiag) { rc=+1; goto out; }

    /* Get extension length (the length on the original line that */
    /* projects as at least half a screen diagonal length on the RP). */
    /* (Let's use 0.7 for safety, as above.) */
    ext=0.7*rpscrdiag/rplen;

    /*
    **  Get the signed distance parameters for the result pts.
    **  (Note that nearsrd is a signed distance factor -- not just
    **  a relative one -- in the WCS, because we used two points
    **  with unit separation at the beginning of this function.)
    */
    ar[1]=nearsrd-ext; ar[2]=nearsrd+ext;

    /* If it's a RAY and both pts are in the negative direction from p0, */
    /* don't bother generating (it's completely off screen): */
    if (type==DB_RAY && ar[1]<0.0 && ar[2]<0.0) { rc=+1; goto out; }

    /* Generate the 2 pts: */
    if (type==DB_RAY && ar[1]<0.0) ar[1]=0.0;
    res[0][0]=p0[0]+ar[1]*uv[0];
    res[0][1]=p0[1]+ar[1]*uv[1];
    res[0][2]=p0[2]+ar[1]*uv[2];
    if (type==DB_RAY && ar[2]<0.0) ar[2]=0.0;
    res[1][0]=p0[0]+ar[2]*uv[0];
    res[1][1]=p0[1]+ar[2]*uv[1];
    res[1][2]=p0[2]+ar[2]*uv[2];


out:
    if (rc) {
        if (p0!=NULL) {
            GR_PTCPY(res[0],p0); GR_PTCPY(res[1],p0);
        } else {
            res[0][0]=res[0][1]=res[0][2]=res[1][0]=res[1][1]=res[1][2]=0.0;
        }
    }

    if (res0!=NULL) { GR_PTCPY(res0,res[0]); }
    if (res1!=NULL) { GR_PTCPY(res1,res[1]); }

    return rc;
}

