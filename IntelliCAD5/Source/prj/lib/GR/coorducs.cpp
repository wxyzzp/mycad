/* 
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * functions to get various coordinate systems
 * 
 */ 

#include "gr.h"
#include "gr_view.h"


short gr_getucs(
    sds_point     org,
    sds_point     xdir,
    sds_point     ydir,
    sds_point     zdir,
    gr_viewvars *viewvarsp,
    db_drawing   *dp) {
/*
**  Reads UCSORG, UCSXDIR, and UCSYDIR from dp->header (via
**  db_qgetvar()) -- or uses the corresponding data from *viewvars,
**  if viewvarsp is non-NULL -- and sets the appropriate pass
**  parameters.  Any of them except dp can be NULL if they are not
**  wanted.  dp is needed for the db_qgetvar() calls.
**
**  Returns:
**       0 : OK
**      -1 : Bad pass params, or db_qgetvar() failed
*/
    char *headerbuf;
    short inps,rc,fi1,fi2;
    sds_real ar1;
    sds_point ucs[4];


    rc=inps=0;

    if (dp!=NULL) inps=dp->inpspace();

    if (viewvarsp==NULL) {
        if (dp==NULL || (headerbuf=dp->ret_headerbuf())==NULL ||
            db_qgetvar((inps) ? DB_QP_UCSORG  : DB_QUCSORG ,headerbuf,ucs[0],DB_3DPOINT,0) ||
            db_qgetvar((inps) ? DB_QP_UCSXDIR : DB_QUCSXDIR,headerbuf,ucs[1],DB_3DPOINT,0) ||
            db_qgetvar((inps) ? DB_QP_UCSYDIR : DB_QUCSYDIR,headerbuf,ucs[2],DB_3DPOINT,0))
                { rc=-2; goto out; }
    } else {
        GR_PTCPY(ucs[0],viewvarsp->ucsorg);
        GR_PTCPY(ucs[1],viewvarsp->ucsxdir);
        GR_PTCPY(ucs[2],viewvarsp->ucsydir);
    }

    /* Get the unit UCS axes; use fi2 to signal when to default to WCS: */
    fi2=0;
    for (fi1=1; fi1<3 && !fi2; fi1++) {  /* Unitize the X- and Y-axes. */
        if (icadRealEqual((ar1=GR_LEN(ucs[fi1])),0.0)) {
            fi2=1;
        } else if (ar1!=1.0) {
            GR_UNITIZE(ucs[fi1],ar1);
        }
    }

    /* Cross X with Y to get Z-axis: */
    if (!fi2) {
        ucs[3][0]=ucs[1][1]*ucs[2][2]-ucs[1][2]*ucs[2][1];
        ucs[3][1]=ucs[1][2]*ucs[2][0]-ucs[1][0]*ucs[2][2];
        ucs[3][2]=ucs[1][0]*ucs[2][1]-ucs[1][1]*ucs[2][0];
        if ((ar1=GR_LEN(ucs[3]))<IC_ZRO) {
            fi2=1;
        } else if (ar1!=1.0) {
            GR_UNITIZE(ucs[3],ar1);
        }
    }

    if (!fi2) {
        /* Cross Z with X to make SURE Y-axis is normal to the others: */
        ucs[2][0]=ucs[3][1]*ucs[1][2]-ucs[3][2]*ucs[1][1];
        ucs[2][1]=ucs[3][2]*ucs[1][0]-ucs[3][0]*ucs[1][2];
        ucs[2][2]=ucs[3][0]*ucs[1][1]-ucs[3][1]*ucs[1][0];
    } else {
        ucs[1][1]=ucs[1][2]=ucs[2][0]=ucs[2][2]=ucs[3][0]=ucs[3][1]=0.0;
        ucs[1][0]=ucs[2][1]=ucs[3][2]=1.0;
    }

out:
    if (rc) {
        memset(ucs,0,sizeof(ucs));
        ucs[1][0]=ucs[2][1]=ucs[3][2]=1.0;
    }

    if (org !=NULL) { GR_PTCPY( org,ucs[0]); }
    if (xdir!=NULL) { GR_PTCPY(xdir,ucs[1]); }
    if (ydir!=NULL) { GR_PTCPY(ydir,ucs[2]); }
    if (zdir!=NULL) { GR_PTCPY(zdir,ucs[3]); }

    return rc;
}


GR_API short gr_twist4ucs(db_drawing *dp, sds_real *dontset) {
/*
**  Adjusts VIEWTWIST (for dp) such that the UCS axes will appear
**  "right side up" on the RP plane.  Commands like VPOINT adjust VIEWDIR
**  and then often need to set VIEWTWIST such that the UCS appears
**  "right side up".
**
**  This means that the projection of the UCS Z-axis on the screen (RP)
**  will point upward -- unless VIEWDIR is perpendicular to the UCS
**  XY-plane.
**
**  If VIEWDIR is perpendicular to the XY-plane in a system,
**  the RP X-axis generated will be that system's +X-axis if
**  VIEWDIR and the system's Z-axis are in the same direction,
**  or the system's -X-axis if VIEWDIR and the system's Z-axis
**  are in opposite directions.
**
**  This is accomplished as follows.  We determine two possible RP X-axes
**  in UCS coordinates: one by crossing the WCS Z-axis (according to
**  the UCS) with VIEWDIR and one by crossing the UCS Z-axis with
**  VIEWDIR.  The former will make the WCS look "right side up" and is
**  the true 0-twist RP X-axis; the latter will make the UCS look "right
**  side up" (which is what we want).  We then set VIEWTWIST to whatever
**  CW angle will make the true 0-twist RP X-axis coincide with the
**  UCS-generated one.
**
**  NOTE: *dontset!=NULL, f'n places result there and DOESN'T set
**          the viewtwist var in the dp
**  Returns:
**       0 : OK
**      -1 : db_getvar() or gr_getucs() error
**      -2 : VIEWDIR is (0,0,0)
**      -3 : db_setvar() error
**
*/
    short rc;
    sds_real ar1;
    sds_point ucsaxis[3],rpx[2],ap1;
    struct resbuf rb;


    rc=0; rb.rbnext=NULL;


    if (gr_getucs(NULL,ucsaxis[0],ucsaxis[1],ucsaxis[2],NULL,dp))
        { rc=-1; goto out; }

    if (db_getvar(NULL,DB_QVIEWDIR,&rb,dp,NULL,NULL)!=RTNORM)
        { rc=-1; goto out; }
    if (icadRealEqual((ar1=GR_LEN(rb.resval.rpoint)),0.0)) { rc=-2; goto out; }
    GR_UNITIZE(rb.resval.rpoint,ar1);

    /*
    **  Where would the WCS put the RP X-axis (WCSZ x VIEWDIR)?
    **  Remember that WCS axes matrix in the UCS is the transpose
    **  of the UCS axes matrix in the WCS:
    */
    rpx[0][0]=ucsaxis[1][2]*rb.resval.rpoint[2]-
              ucsaxis[2][2]*rb.resval.rpoint[1];
    rpx[0][1]=ucsaxis[2][2]*rb.resval.rpoint[0]-
              ucsaxis[0][2]*rb.resval.rpoint[2];
    rpx[0][2]=ucsaxis[0][2]*rb.resval.rpoint[1]-
              ucsaxis[1][2]*rb.resval.rpoint[0];
    if ((ar1=GR_LEN(rpx[0]))<IC_ZRO) {
        /* Dot WCSZ with VIEWDIR; positive means same direction: */
        if (ucsaxis[0][2]*rb.resval.rpoint[0]+
            ucsaxis[1][2]*rb.resval.rpoint[1]+
            ucsaxis[2][2]*rb.resval.rpoint[2]>0.0) {  /* Take +WCSX */

            rpx[0][0]= ucsaxis[0][0];
            rpx[0][1]= ucsaxis[1][0];
            rpx[0][2]= ucsaxis[2][0];
        } else {  /* Take -WCSX */
            rpx[0][0]=-ucsaxis[0][0];
            rpx[0][1]=-ucsaxis[1][0];
            rpx[0][2]=-ucsaxis[2][0];
        }
    } else if (ar1!=1.0) {
        GR_UNITIZE(rpx[0],ar1);
    }

    /*
    **  Where would the UCS put the RP X-axis (UCSZ x VIEWDIR)?
    */
    rpx[1][0]=-rb.resval.rpoint[1];
    rpx[1][1]= rb.resval.rpoint[0];
    rpx[1][2]=0.0;
    if ((ar1=GR_LEN(rpx[1]))<IC_ZRO) {  /* Take + or - UCSX. */
        rpx[1][1]=0.0; rpx[1][0]=(rb.resval.rpoint[2]>0.0) ? 1.0 : -1.0;
    } else if (ar1!=1.0) {
        GR_UNITIZE(rpx[1],ar1);
    }

    /* Find the angle between the rpx[] axes: */
    ar1=rpx[0][0]*rpx[1][0]+rpx[0][1]*rpx[1][1]+rpx[0][2]*rpx[1][2];
    if (ar1>1.0) ar1=1.0; else if (ar1<-1.0) ar1=-1.0;
    ar1=acos(ar1);

    /*
    **  If rpx[0] x rpx[1] is not the zero vector, see if it's in the
    **  +VIEWDIR or -VIEWDIR direction.  If plus, set ar1=IC_TWOPI-ar1:
    */
    ap1[0]=rpx[0][1]*rpx[1][2]-rpx[0][2]*rpx[1][1];
    ap1[1]=rpx[0][2]*rpx[1][0]-rpx[0][0]*rpx[1][2];
    ap1[2]=rpx[0][0]*rpx[1][1]-rpx[0][1]*rpx[1][0];
    if (ap1[0]*rb.resval.rpoint[0]+
        ap1[1]*rb.resval.rpoint[1]+
        ap1[2]*rb.resval.rpoint[2]>0.0) ar1=IC_TWOPI-ar1;

    /* ar1 is the correct VIEWTWIST; */
    if(dontset!=NULL)
        *dontset=ar1;
    else{
        rb.restype=RTREAL; rb.resval.rreal=ar1;
        if (db_setvar(NULL,DB_QVIEWTWIST,&rb,dp,NULL,NULL,0)!=RTNORM) rc=-3;
    }

out:

    return rc;
}


