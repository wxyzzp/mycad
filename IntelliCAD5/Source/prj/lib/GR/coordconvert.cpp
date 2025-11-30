/*
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * functions that convert one coordinate system to another
 *
 */

#include "gr.h"
#include "gr_view.h"

extern gr_view gr_defview;

void gr_ncs2ucs(
    sds_point        sour,  /* Source pt (NCS -- ECS or WCS) */
    sds_point        dest,  /* Destination point (UCS) */
    short            disp,  /* Vector transformation (sour and dest are */
                            /*   vectors) */
    gr_view *viewp) {
/*
**  This function is an extension of gr_getdispobjs() (via
**  gr_gensolid(), etc.); we may use it as an "in-line" function. Note that
**  this function uses viewp.
**
**  It applies the transformed axes (viewp->xfp[]) set up by
**  gr_getdispobjs(). It takes sour from the NCS into the UCS.
**  The source coords are used as magnitudes in the directions of the
**  transformed axes.
**
**  sour and dest MUST NOT refer to the same point.
*/
	assert(sour != dest);
    sds_point ap1;
#ifdef _TRANSFORMATION_AS_ARRAY_
    if (viewp->ncs_ucs==IC_TRANS_NO_SHORTCUTS) {
		dest[0]=sour[0]*viewp->xfp[1][0]+
                sour[1]*viewp->xfp[2][0]+
                sour[2]*viewp->xfp[3][0];
        dest[1]=sour[0]*viewp->xfp[1][1]+
                sour[1]*viewp->xfp[2][1]+
                sour[2]*viewp->xfp[3][1];
        dest[2]=sour[0]*viewp->xfp[1][2]+
                sour[1]*viewp->xfp[2][2]+
                sour[2]*viewp->xfp[3][2];
    }
    else {
        memcpy(dest,sour,3*sizeof(double));
//		dest[0]=sour[0];
//		dest[1]=sour[1];
//		dest[2]=sour[2];
    }

    if (!disp) {
        if (viewp->ncs_ucs!=IC_TRANS_NO_SCALE_ROTATE_TRANSLATE) {
            dest[0]+=viewp->xfp[0][0];
            dest[1]+=viewp->xfp[0][1];
            dest[2]+=viewp->xfp[0][2];
        }
        if (viewp->ucsextmode>-1) {  /* Update the UCS entity extent. */
            if (viewp->ucsextmode==0) {
                GR_PTCPY(viewp->ucsext[0],dest);
                GR_PTCPY(viewp->ucsext[1],dest);
                viewp->ucsextmode=1;
            } else {
                if      (viewp->ucsext[0][0]>dest[0]) viewp->ucsext[0][0]=dest[0];
                else if (viewp->ucsext[1][0]<dest[0]) viewp->ucsext[1][0]=dest[0];
                if      (viewp->ucsext[0][1]>dest[1]) viewp->ucsext[0][1]=dest[1];
                else if (viewp->ucsext[1][1]<dest[1]) viewp->ucsext[1][1]=dest[1];
                if      (viewp->ucsext[0][2]>dest[2]) viewp->ucsext[0][2]=dest[2];
                else if (viewp->ucsext[1][2]<dest[2]) viewp->ucsext[1][2]=dest[2];
            }
            if (viewp->dothick) {
                ap1[0]=dest[0]+viewp->xfp[4][0];
                ap1[1]=dest[1]+viewp->xfp[4][1];
                ap1[2]=dest[2]+viewp->xfp[4][2];
                if      (viewp->ucsext[0][0]>ap1[0]) viewp->ucsext[0][0]=ap1[0];
                else if (viewp->ucsext[1][0]<ap1[0]) viewp->ucsext[1][0]=ap1[0];
                if      (viewp->ucsext[0][1]>ap1[1]) viewp->ucsext[0][1]=ap1[1];
                else if (viewp->ucsext[1][1]<ap1[1]) viewp->ucsext[1][1]=ap1[1];
                if      (viewp->ucsext[0][2]>ap1[2]) viewp->ucsext[0][2]=ap1[2];
                else if (viewp->ucsext[1][2]<ap1[2]) viewp->ucsext[1][2]=ap1[2];
            }
        }
    }
#else
    if(viewp->ncs_ucs == IC_TRANS_NO_SHORTCUTS)
		viewp->m_transformation.multiplyRightUsing33(dest, sour);
    else
        memcpy(dest, sour, 3 * sizeof(double));

    if(!disp)
	{
        if(viewp->ncs_ucs != IC_TRANS_NO_SCALE_ROTATE_TRANSLATE)
		{
			dest[0] += viewp->m_transformation(0,3);
			dest[1] += viewp->m_transformation(1,3);
			dest[2] += viewp->m_transformation(2,3);
        }
        if(viewp->ucsextmode>-1)
		{  /* Update the UCS entity extent. */
            if (viewp->ucsextmode ==0 )
			{
                GR_PTCPY(viewp->ucsext[0], dest);
                GR_PTCPY(viewp->ucsext[1], dest);
                viewp->ucsextmode = 1;
            }
			else
			{
                if      (viewp->ucsext[0][0]>dest[0]) viewp->ucsext[0][0]=dest[0];
                else if (viewp->ucsext[1][0]<dest[0]) viewp->ucsext[1][0]=dest[0];
                if      (viewp->ucsext[0][1]>dest[1]) viewp->ucsext[0][1]=dest[1];
                else if (viewp->ucsext[1][1]<dest[1]) viewp->ucsext[1][1]=dest[1];
                if      (viewp->ucsext[0][2]>dest[2]) viewp->ucsext[0][2]=dest[2];
                else if (viewp->ucsext[1][2]<dest[2]) viewp->ucsext[1][2]=dest[2];
            }
            if (viewp->dothick) {
                ap1[0]=dest[0]+viewp->m_thickness[0];
                ap1[1]=dest[1]+viewp->m_thickness[1];
                ap1[2]=dest[2]+viewp->m_thickness[2];
                if      (viewp->ucsext[0][0]>ap1[0]) viewp->ucsext[0][0]=ap1[0];
                else if (viewp->ucsext[1][0]<ap1[0]) viewp->ucsext[1][0]=ap1[0];
                if      (viewp->ucsext[0][1]>ap1[1]) viewp->ucsext[0][1]=ap1[1];
                else if (viewp->ucsext[1][1]<ap1[1]) viewp->ucsext[1][1]=ap1[1];
                if      (viewp->ucsext[0][2]>ap1[2]) viewp->ucsext[0][2]=ap1[2];
                else if (viewp->ucsext[1][2]<ap1[2]) viewp->ucsext[1][2]=ap1[2];
            }
        }
    }
#endif
}

void gr_ucs2ncs(
    sds_point        sour,   /* Source pt (UCS) */
    sds_point        dest,   /* Destination point (NCS -- ECS or WCS) */
    short            disp,   /* Vector xform (sour and dest are vectors) */
    gr_view *viewp) {
/*
**  An extension of gr_getdispobjs() (via gr_gensolid()); we may
**  use it as an "in-line" function. Note that this function uses viewp.
**
**  This function is currently used in gr_gensolid() to convert
**  2D POLYLINE segments back to the NCS to determine width
**  vectors (which are then coverted back to the UCS).
**
**  It applies the UCS axes ACCORDING TO THE NCS (viewp->ixfp[]) set up by
**  gr_setupixfp().  It takes sour from the UCS into the NCS.
**  The source coords are used as magnitudes in the directions of the
**  transformed axes.
**
**  NOTE:  Prior to calling this function, the caller must make sure
**  that gr_getdispobjs() has set up viewp->xfp[] AND that
**  gr_setupixfp() has been called. (For speed purposes, we're only
**  setting up viewp->ixfp[] and using this function when 2D POLYLINEs
**  with widths are encountered.)
**
**  sour and dest MUST NOT refer to the same point.
*/
	assert(sour != dest);
#ifdef _TRANSFORMATION_AS_ARRAY_
	if (viewp->ncs_ucs==IC_TRANS_NO_SHORTCUTS) {
		dest[0]=sour[0]*viewp->ixfp[1][0]+
                sour[1]*viewp->ixfp[2][0]+
                sour[2]*viewp->ixfp[3][0];
        dest[1]=sour[0]*viewp->ixfp[1][1]+
                sour[1]*viewp->ixfp[2][1]+
                sour[2]*viewp->ixfp[3][1];
        dest[2]=sour[0]*viewp->ixfp[1][2]+
                sour[1]*viewp->ixfp[2][2]+
                sour[2]*viewp->ixfp[3][2];
    }
    else {
		memcpy(dest,sour,3*sizeof(double));
//		dest[0]=sour[0];
//		dest[1]=sour[1];
//		dest[2]=sour[2];
    }
    if (disp || viewp->ncs_ucs==IC_TRANS_NO_SCALE_ROTATE_TRANSLATE) return;
    dest[0]+=viewp->ixfp[0][0];
    dest[1]+=viewp->ixfp[0][1];
    dest[2]+=viewp->ixfp[0][2];
#else
	if (viewp->ncs_ucs == IC_TRANS_NO_SHORTCUTS)
		viewp->m_transformationInverse.multiplyRightUsing33(dest, sour);
    else
		memcpy(dest,sour,3*sizeof(double));
    if(!disp && viewp->ncs_ucs != IC_TRANS_NO_SCALE_ROTATE_TRANSLATE)
	{
		dest[0] += viewp->m_transformationInverse(0,3);
		dest[1] += viewp->m_transformationInverse(1,3);
		dest[2] += viewp->m_transformationInverse(2,3);
	}
#endif
}

GR_API short gr_ucs2rp(
    sds_point sour,
    sds_point dest,
    gr_view *viewp) {
/*
**  Takes a source UCS point and projects it onto the real projection
**  plane.  Looks at viewp->mode to decide whether it's a
**  central (perspective) or  orthogonal projection.
**
**  sour and dest may refer to the same point.
**
**  MAKE SURE YOU CALL gr_initview() BEFORE CALLING THIS
**  FUNCTION.  (The call has been omitted here for speed.)
**
**  Returns:
**       0 : OK
**      -1 : sour does not project onto the plane because sour is
**             behind the camera (perspective).
*/
    short rc;
    sds_real ar1;
    sds_point ap1;


    rc=0;

    if (viewp==NULL || icadRealEqual(viewp->viewsize,0.0)) viewp=&gr_defview;

    /* See p_mode==2 in gr_getdispobjs() for an explanation of why */
    /* we're avoiding the projection here: */
    if (viewp->noucs2rp) { GR_PTCPY(dest,sour); goto out; }

    if (viewp->mode&1) {  /* Central (perspective) */
        /* Get ray from sour to camera: */
        ap1[0]=viewp->camera[0]-sour[0];
        ap1[1]=viewp->camera[1]-sour[1];
        ap1[2]=viewp->camera[2]-sour[2];

        /*
        **  Get directed distance along viewing direction and divide
        **  the viewing distance by it.  The product of that and the
        **  negative of the sour-to-camera vector gives the
        **  projection vector.  If the signed distance <=0.0,
        **  sour is behind the camera:
        */
        if (fabs((ar1=ap1[0]*viewp->rpaxis[2][0]+
                 ap1[1]*viewp->rpaxis[2][1]+
                 ap1[2]*viewp->rpaxis[2][2]))<IC_ZRO) {

            ic_ptcpy(dest,sour); rc=-1; goto out;
        } else if (ar1<0.0) rc=-1;

        ar1=viewp->viewdist/ar1;

        /* Get the position vector of the point's projection */
        /* on the plane: */
        ap1[0]=viewp->camera[0]-ar1*ap1[0]-viewp->target[0];
        ap1[1]=viewp->camera[1]-ar1*ap1[1]-viewp->target[1];
        ap1[2]=viewp->camera[2]-ar1*ap1[2]-viewp->target[2];

        /* Dot position vector ap1 with the axis vectors to get the */
        /* directed distance (i.e., the X and Y coords in the */
        /* plane system): */
        dest[0]=ap1[0]*viewp->rpaxis[0][0]+
                ap1[1]*viewp->rpaxis[0][1]+
                ap1[2]*viewp->rpaxis[0][2];
        dest[1]=ap1[0]*viewp->rpaxis[1][0]+
                ap1[1]*viewp->rpaxis[1][1]+
                ap1[2]*viewp->rpaxis[1][2];
    } else {  /* Orthogonal (non-perspective) */
        if (viewp->rp_ucs==2) {  /* RP and UCS are congruent */
            dest[0]=sour[0]; dest[1]=sour[1];
        } else {
			/*
			// DP: old realization, contains useless operations
            // Get the directed distance of pt from plane.
            ar1=(sour[0]-viewp->target[0])*viewp->rpaxis[2][0]+
                (sour[1]-viewp->target[1])*viewp->rpaxis[2][1]+
                (sour[2]-viewp->target[2])*viewp->rpaxis[2][2];

            // Get the position vector of the point's projection on the plane:
            ap1[1]=sour[1]-ar1*viewp->rpaxis[2][1]-viewp->target[1];
            ap1[0]=sour[0]-ar1*viewp->rpaxis[2][0]-viewp->target[0];
            ap1[2]=sour[2]-ar1*viewp->rpaxis[2][2]-viewp->target[2];
            // Dot position vector ap1 with the axis vectors to get the directed distance
			// (i.e., the X and Y coords in the plane system):
            dest[0]=ap1[0]*viewp->rpaxis[0][0]+
                    ap1[1]*viewp->rpaxis[0][1]+
                    ap1[2]*viewp->rpaxis[0][2];
            dest[1]=ap1[0]*viewp->rpaxis[1][0]+
                    ap1[1]*viewp->rpaxis[1][1]+
                    ap1[2]*viewp->rpaxis[1][2];
			*/
			ap1[0] = sour[0] - viewp->target[0];
			ap1[1] = sour[1] - viewp->target[1];
			ap1[2] = sour[2] - viewp->target[2];
            dest[0]= ap1[0] * viewp->rpaxis[0][0] + ap1[1] * viewp->rpaxis[0][1] + ap1[2] * viewp->rpaxis[0][2];
            dest[1]= ap1[0] * viewp->rpaxis[1][0] + ap1[1] * viewp->rpaxis[1][1] + ap1[2] * viewp->rpaxis[1][2];
        }
    }

    dest[2]=0.0;

out:
    return rc;
}


GR_API short gr_rp2ucs(
    sds_point        sour,
    sds_point        dest,
    gr_view *viewp) {
/*
**  Takes a source point in the real projection plane system and
**  determines the corresponding point in the current UCS at the current
**  elevation.  When that's not possible (the projection ray is
**  perpendicular to the UCS Z-axis), the UCS coordinates of the point
**  on the projection point are used.
**
**  Looks at viewp->mode to decide whether it's a central (perspective)
**  or orthogonal projection.
**
**  sour and dest may refer to the same point.
**
**  MAKE SURE YOU CALL gr_initview() BEFORE CALLING THIS
**  FUNCTION.  (The call has been omitted here for speed.)
**
**  Returns:
**       0 : OK
**      -1 : Can't determine a pt at the current elevation;
**             dest set to UCS coords of the pt on the projection plane.
*/
    sds_real raylen,ar1;
    sds_point ap1,ray;

    if (viewp==NULL || icadRealEqual(viewp->viewsize,0.0))
    {
        viewp=&gr_defview;
    }

    if (!(viewp->mode&1) && viewp->rp_ucs==2)
    {
        /* Orthogonal, and RP and UCS are congruent. */
        dest[0]=sour[0]; dest[1]=sour[1]; dest[2]=viewp->elevation;

    }
    else
    {
        /* Get the UCS coords of the projection on the plane: */
        ap1[0]=viewp->target[0]+
            sour[0]*viewp->rpaxis[0][0]+
            sour[1]*viewp->rpaxis[1][0];
        ap1[1]=viewp->target[1]+
            sour[0]*viewp->rpaxis[0][1]+
            sour[1]*viewp->rpaxis[1][1];
        ap1[2]=viewp->target[2]+
            sour[0]*viewp->rpaxis[0][2]+
            sour[1]*viewp->rpaxis[1][2];

        /* Get the projection ray direction in the UCS: */
        if (viewp->mode&1) {  /* Central (perspective) */
            ray[0]=viewp->camera[0]-ap1[0];
            ray[1]=viewp->camera[1]-ap1[1];
            ray[2]=viewp->camera[2]-ap1[2];
        } else {  /* Orthogonal (non-perspective) */
            ray[0]=viewp->rpaxis[2][0];
            ray[1]=viewp->rpaxis[2][1];
            ray[2]=viewp->rpaxis[2][2];
        }

        raylen=sqrt(ray[0]*ray[0]+ray[1]*ray[1]+ray[2]*ray[2]);

        /* Find the point along the line through ap1 in the direction of ray */
        /* that has the current elevation: */
        if (fabs(ray[2])<=IC_ZRO*raylen) {  /* Can't; just take ap1. */
            GR_PTCPY(dest,ap1);
            return -1;
        } else {
            ar1=(viewp->elevation-ap1[2])/ray[2];
            dest[0]=ar1*ray[0]+ap1[0];
            dest[1]=ar1*ray[1]+ap1[1];
            dest[2]=viewp->elevation;
        }

    }

    return 0;
}

GR_API void gr_rp2pix(
    gr_view *p_viewp,
    sds_real         p_rx,
    sds_real         p_ry,
    int             *p_ixp,
    int             *p_iyp) {
/*
**  Converts a point in real projection plane coordinates to the
**  equivalent point in pixels coordinates, based on the parameters set
**  in *p_viewp.
**
**  The Windows drawing functions don't seem to like pixel values
**  outside of the range of a short (+or- 16K), so there's some code
**  in here to limit the results to that range.  The idea is that
**  any endpoint of a line that crosses the screen that has a pixel
**  coordinate beyond the 16K range can be moved toward the center of the
**  screen until it is just inside that limit wihout shifting
**  the resulting image too badly.  The code in here is written
**  for speed, not beauty, so it may look a little awkward.
**
**  MAKE SURE YOU CALL gr_initview() BEFORE CALLING THIS
**  FUNCTION.  (The call has been omitted here for speed.)
*/
    sds_real x1,y1,adx,ady;
	//Modified Cybage AW 30/04/2001[
	//Reason : For zooming the solid entities
    static sds_real maxval=15000;  /* A little under 16K, for safety. */
	//static sds_real maxval=2.75e+14;
	//Modified Cybage AW 30/04/2001]

    if (p_viewp==NULL || icadRealEqual(p_viewp->viewsize,0.0)) p_viewp=&gr_defview;

    /* Get pixel offset (and its abs val) from center of screen: */
//4M Item:99->
//Prints upside down
/*
    adx=fabs((x1=(p_rx-p_viewp->GetCenterProjectionX()) * p_viewp->GetReciprocalOfPixelWidth()));
    ady=fabs((y1=(p_ry-p_viewp->GetCenterProjectionY()) * p_viewp->GetReciprocalOfPixelHeight()));
*/
    if (p_viewp->isprinting==2){
       adx=fabs((x1=-(p_rx-p_viewp->GetCenterProjectionX()) * p_viewp->GetReciprocalOfPixelWidth()));
       ady=fabs((y1=-(p_ry-p_viewp->GetCenterProjectionY()) * p_viewp->GetReciprocalOfPixelHeight()));
    }
    else{
       adx=fabs((x1=(p_rx-p_viewp->GetCenterProjectionX()) * p_viewp->GetReciprocalOfPixelWidth()));
       ady=fabs((y1=(p_ry-p_viewp->GetCenterProjectionY()) * p_viewp->GetReciprocalOfPixelHeight()));
    }
//<-4M

    if (adx>maxval) {
        if (ady>maxval) {  // Both outside
            if (adx>=ady) {  // X is farther out
                y1*=maxval/adx; x1=(x1>0.0) ? maxval : -maxval;
            } else {  // Y is farther out
                x1*=maxval/ady; y1=(y1>0.0) ? maxval : -maxval;
            }
        } else {  // Only X outside
            y1*=maxval/adx; x1=(x1>0.0) ? maxval : -maxval;
        }
    } else if (ady>maxval) {  // Only Y outside
        x1*=maxval/ady; y1=(y1>0.0) ? maxval : -maxval;
    }

#if 0
	//**** OLD CODE: TURNED OFF *****
    *p_ixp=(int)(x1+p_viewp->midwin[0]);
    *p_iyp=(int)(y1+p_viewp->midwin[1]);
#else
	//**** NEW CODE: Use the same algorithm as gr_vect_rp2pix() *****
	//The above code inside #if does the same thing as the below assembly code with one exception:
	//C++ casting from double to int discards the decimal portion of the double while
	//the assembler version does round-off.
	int ix1, iy1;

	_asm
	{
		fld		qword ptr x1
		fistp	dword ptr ix1
		fld		qword ptr y1
		fistp	dword ptr iy1
	}

    *p_ixp= ix1+p_viewp->midwin[0];
    *p_iyp= iy1+p_viewp->midwin[1];
#endif
}

GR_API void gr_pix2rp(
    gr_view *p_viewp,
    int              p_ix,
    int              p_iy,
    sds_real        *p_rxp,
    sds_real        *p_ryp) {
/*
**  Converts a point in pixel coordinates to the equivalent point
**  in real projection plane coordinates, based on the parameters set
**  in *p_viewp.
**
**  MAKE SURE YOU CALL gr_initview() BEFORE CALLING THIS
**  FUNCTION.  (The call has been omitted here for speed.)
*/

    if (p_viewp==NULL || icadRealEqual(p_viewp->viewsize,0.0)) p_viewp=&gr_defview;

    *p_rxp=(p_ix-p_viewp->midwin[0])*p_viewp->GetPixelWidth()+p_viewp->GetCenterProjectionX();
    *p_ryp=(p_iy-p_viewp->midwin[1])*p_viewp->GetPixelHeight()+p_viewp->GetCenterProjectionY();
}


// ****************************************************
// gr_clip_vector
//
//
//
// Returns 0 if normal vector
// Returns 1 if completely outside
// Returns 2 if we clip it
//
inline int
gr_clip_vector( const gr_real_rectangle &cliprect, double *px1,double *py1,double *px2,double *py2 )
{
	// Give the stack a break for speed
	//
	static unsigned char outcode1,outcode2;
	static BOOL done;
	static double t;
ASSERT(ic_isvalid_real(*px1));
ASSERT(ic_isvalid_real(*py1));
ASSERT(ic_isvalid_real(*px2));
ASSERT(ic_isvalid_real(*py2));

	done=FALSE;

	// default is completely inside the rectangle
	//
	int retval = 0;

	do
	{
	/* set outcodes for the two points */
	outcode1=outcode2=0;
	/* changed these comparisons because, for example, if x1 is less than the min
	   it cannot also be greater than the max, so we re-order for fewer comparisons */
	if      ( *px1 < cliprect.xmin )
		{
		outcode1 |= 1;
		}
    else if ( *px1 > cliprect.xmax )
		{
		outcode1 |= 2;
		}
    if      ( *py1 < cliprect.ymin )
		{
		outcode1 |= 4;
		}
    else if ( *py1 > cliprect.ymax)
		{
		outcode1 |= 8;
		}
    if      ( *px2 < cliprect.xmin )
		{
		outcode2 |= 1;
		}
    else if ( *px2 > cliprect.xmax )
		{
		outcode2 |= 2;
		}
    if      ( *py2 < cliprect.ymin )
		{
		outcode2 |= 4;
		}
    else if ( *py2 > cliprect.ymax)
		{
		outcode2 |= 8;
		}

    if (outcode1 & outcode2)
		{
		done=TRUE;
		retval = 1;	// Completely off the screen.  Can only happen first time.
		}
    else
		{  /* possible accept */
		if ((outcode1 | outcode2)==0)
			{
			done=TRUE;
			}
		else
			{
				retval = 2; // we're going to clip somehow

				// Because we already checked outcode1 AND 2 being 0, or both
				// being non-zero, at this point we assume only 1 is 0.
				//
				/* subdivide line since at most one endpoint is inside */
			if (outcode1==0)
				{
				if  (outcode2 & 8)
					{
					*px2 += (cliprect.ymax - *py2) *
							((*px1-*px2)/(*py1-*py2));
					*py2 = cliprect.ymax;
			        }
				else if (outcode2 & 4)
					{
					*px2 += (cliprect.ymin - *py2) *
							((*px1-*px2)/(*py1-*py2));
					*py2 = cliprect.ymin;
			        }
				else if (outcode2 & 2)
					{
					*py2 += (cliprect.xmax - *px2) *
						((*py1-*py2)/(*px1-*px2));
		            *px2 = cliprect.xmax;
					}
		        else if (outcode2 & 1)
					{
					*py2 += (cliprect.xmin - *px2) *
							((*py1-*py2)/(*px1-*px2));
					*px2 = cliprect.xmin;
					}
		        }
			else // means outcode2 == 0
				{
				if  (outcode1 & 8)
					{
					*px1 += (cliprect.ymax - *py1) *
							((*px2-*px1)/(*py2-*py1));
					*py1 = cliprect.ymax;
			        }
				else if (outcode1 & 4)
					{
					*px1 += (cliprect.ymin - *py1) *
							((*px2-*px1)/(*py2-*py1));
					*py1 = cliprect.ymin;
			        }
				else if (outcode1 & 2)
					{
					*py1 += (cliprect.xmax - *px1) *
						((*py2-*py1)/(*px2-*px1));
		            *px1 = cliprect.xmax;
					}
		        else if (outcode1 & 1)
					{
					*py1 += (cliprect.xmin - *px1) *
							((*py2-*py1)/(*px2-*px1));
					*px1 = cliprect.xmin;
					}
				}
			} /* end of line subdivision */
		}   /* end of possible accept */
	} while (!done);


	return( retval );
}


GR_API int gr_vect_rp2pix(
    sds_real        *p_rp0,
    sds_real        *p_rp1,
    int             *p_ip,
    gr_view *p_viewp) {
/*
**  Converts a vector in real projection plane coordinates to the
**  equivalent vector in pixels coordinates, based on the parameters set
**  in *p_viewp.
**
**  This function maintains the terminal point from the most recent
**  call (p_rp1) in a static for use in chaining.
**
**  p_rp0 :
**    non-NULL : Ptr to an array of at least 2 reals (X,Y of initial pt).
**        NULL : Uses p_rp1 from the previous call.
**
**  p_rp1 :
**    non-NULL : Ptr to an array of at least 2 reals (X,Y of terminal pt).
**        NULL : Just saves p_rp0 as the "previous point".
**
**  p_ip : Ptr to an array of at least 4 ints (X0,Y0,X1,Y1 of result pts).
**         (Can be NULL if you're just setting the "previous point".)
**
**  You can use this function in two ways:
**
**    (1) Always pass it both points (no NULL pass parameters).
**
**    (2) Init the "previous point" with gr_vect_rp2pix(p0,NULL,NULL,NULL)
**        and then keep calling it with gr_vect_rp2pix(NULL,p1,&intarray,viewp)
**        to do a chain of vectors (and to close, if necessary).
**
** // Changed to use real clipping rather than pseudo-cropping technique.
** // Code is more accurate and slightly faster
** //
**  Why can't we just use gr_rp2pix() for drawing vectors?  Here's
**  the problem.  The Windows drawing functions don't seem to like
**  pixel values outside of the range of a short (+or- 16K), so,
**  when pixel values are generated outside that range, they have
**  to be brought inside, if possible.  It's a cropping problem.
**
**  The following algorithm is used with speed in mind. If the vector
**  is completely off-screen, we return +1.  Otherwise,
**  if both endpoints are within the safety zone, they are transformed
**  and returned as they are, and the windows line function can worry
**  about the cropping.  Otherwise, we crop to the window edges and
**  transform.
**
**  MAKE SURE YOU CALL gr_initview() BEFORE CALLING THIS
**  FUNCTION.  (The call has been omitted here for speed.)
**
**  Returns:
**      -1 : Improper calling parameters
**       0 : OK to use the result (p_ip).
**      +1 : The entire vector is off-screen.  (Don't use the result
**             (p_ip); don't draw anything).
**      +2 : The line was cropped to the window.
*/
    /* For speed, give the stack a break: */
    static int rc;
    static sds_real prevp[2];


    rc=0;


    if (p_rp1==NULL)
		{
        if (p_rp0==NULL)
			{
			rc=-1;
			}
        goto out;
	    }
    if (p_rp0==NULL)
		{
		p_rp0=prevp;
		}
    if (p_ip==NULL)
		{
		rc=-1;
		goto out;
		}
    if (p_viewp==NULL || icadRealEqual(p_viewp->viewsize,0.0))
		{
		p_viewp=&gr_defview;
		}


	static sds_real x1,y1,x2,y2;
	x1 = p_rp0[0];
	y1 = p_rp0[1];
	x2 = p_rp1[0];
	y2 = p_rp1[1];

	rc = gr_clip_vector( p_viewp->GetClipRectangle(), &x1, &y1, &x2, &y2 );

	int ix1,iy1,ix2,iy2;

	if ( (rc == 0) || (rc == 2) )
		{
		// Something showed up
		// Generate pixel coords:

		// ********************************************************************************
		// OPTIMIZED SECTION
		//
		// Original unoptimized version
		//
	    // p_ip[0]=(int)(p_viewp->midwin[0]+
		//    (x1 - p_viewp->GetCenterProjectionX()) * p_viewp->GetReciprocalOfPixelWidth() );
	    // p_ip[1] = (int)(p_viewp->midwin[1]+
		//    (y1 - p_viewp->GetCenterProjectionY()) * p_viewp->GetReciprocalOfPixelHeight() );
	    // p_ip[2] = (int)(p_viewp->midwin[0]+
	    //    (x2 - p_viewp->GetCenterProjectionX()) * p_viewp->GetReciprocalOfPixelWidth() );
	    // p_ip[3]=(int)(p_viewp->midwin[1]+
		//    (y2 - p_viewp->GetCenterProjectionY()) * p_viewp->GetReciprocalOfPixelHeight() );

//4M Item:99->
// Prints upside down
/*
		x1 = (x1 - p_viewp->GetCenterProjectionX()) * p_viewp->GetReciprocalOfPixelWidth();
		y1 = (y1 - p_viewp->GetCenterProjectionY()) * p_viewp->GetReciprocalOfPixelHeight();
	    x2 = (x2 - p_viewp->GetCenterProjectionX()) * p_viewp->GetReciprocalOfPixelWidth();
		y2 = (y2 - p_viewp->GetCenterProjectionY()) * p_viewp->GetReciprocalOfPixelHeight();
*/
      if (p_viewp->isprinting==2){
		   x1 = -(x1 - p_viewp->GetCenterProjectionX()) * p_viewp->GetReciprocalOfPixelWidth();
		   y1 = -(y1 - p_viewp->GetCenterProjectionY()) * p_viewp->GetReciprocalOfPixelHeight();
	      x2 = -(x2 - p_viewp->GetCenterProjectionX()) * p_viewp->GetReciprocalOfPixelWidth();
		   y2 = -(y2 - p_viewp->GetCenterProjectionY()) * p_viewp->GetReciprocalOfPixelHeight();
      }
      else{
		   x1 = (x1 - p_viewp->GetCenterProjectionX()) * p_viewp->GetReciprocalOfPixelWidth();
		   y1 = (y1 - p_viewp->GetCenterProjectionY()) * p_viewp->GetReciprocalOfPixelHeight();
	      x2 = (x2 - p_viewp->GetCenterProjectionX()) * p_viewp->GetReciprocalOfPixelWidth();
		   y2 = (y2 - p_viewp->GetCenterProjectionY()) * p_viewp->GetReciprocalOfPixelHeight();
      }
//<-4M

        // **********************
		// INLINE ASSEMBLY!!!
		// gr_vect_rp2pix is one of the most called functions in ICAD, since it is called about twice
		// for every point in a drawing.  Profiling indicated that the biggest bottleneck in this function
		// was the (int) cast, which resulted in call to Microsoft's ftol() function.  ftol() was taking 80%
		// of the time in gr_vect_rp2pix -- over 4x as much as the clipping above (which is inlined)
		//
		// This code improves the overall performance of gr_vect_rp2pix from 512 cycles per call
		// to 116 cycles per call
		//
		// Basically this assembly is equivalent to
		// ix1 = (int)x1;
		// iy1 = (int)y1;
		// ix2 = (int)x2;
		// iy2 = (int)y2;
		//
		// fld -- floating point load -- loads a double into a FPU register
		// fistp -- floating point to integer store pointer -- stores a double into a 32-bit integer
		//

		_asm
			{
			fld		qword ptr x1
			fistp	dword ptr ix1
			fld		qword ptr y1
			fistp	dword ptr iy1
			fld		qword ptr x2
			fistp	dword ptr ix2
			fld		qword ptr y2
			fistp	dword ptr iy2
			}


		p_ip[0] = ix1 + p_viewp->midwin[0];
		p_ip[1] = iy1 + p_viewp->midwin[1];
		p_ip[2] = ix2 + p_viewp->midwin[0];
		p_ip[3] = iy2 + p_viewp->midwin[1];

		//
		// END OF OPTIMIZED SECTION
		// ********************************************************************************
		}
out:
    if (rc>-1) {
        if (p_rp1==NULL) {
            if (p_rp0!=NULL) {
				prevp[0]=p_rp0[0];
				prevp[1]=p_rp0[1];
			}
        } else {
			prevp[0]=p_rp1[0];
			prevp[1]=p_rp1[1];
		}
    }
    return rc;
}

