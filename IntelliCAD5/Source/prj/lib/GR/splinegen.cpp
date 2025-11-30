/* 
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * code to generate splines
 * 
 */ 

#include "gr.h"

int gr_gg3spline(
    int       *m,
    sds_point *vertices,
    int        n,
    sds_point *q,
    int       *span_size) {
/*
**  Based on Georg Glaeser's cubic_spline_curve(), p.238  in FAST ALGORITHMS
**  FOR 3D-GRAPHICS, 1994 (Springer-Verlag).  I changed variable
**  types and used OUR means instead of his for such things as
**  memory management and IC_ZRO.
**
**  This one seems ALMOST right for scenario 2 (fit), but there's
**  no provision for terminal tangent directions.
**
**  Wouldn't compile as printed in the book.  Had mismatched
**  parentheses (and an unused local variable).
**
**  Not really impressed with this guy's documentation.  Here are my
**  conclusions about the pass parameters, based on what the code
**  seems to do (and his meager comments):
**
**    Results:
**
**      m : The number of points assigned in the "vertices" array.
**
**      vertices : An array of pts ALREADY ALLOCATED large enough
**                 to handle the results.
**
**    Given:
**
**      n : The size of the q array.  (Looks like it needs to be >=3.)
**
**      q : An array of fit points (with n valid elements).
**
**      span_size : An ARRAY of ints giving the number of pts wanted
**                  for each span.  Appears to need an array of
**                  n-1 of them, as you'd expect, since a "span"
**                  appears to be the arch between 2 adjacent
**                  control pts.
**
**  Glaeser's comments are preceded by "GG:".
**
**  Returns gr_getdispobjs() codes.
*/
    int i,i1,imax,k,*size,nsa,rc;
    sds_real *a,*b,*c,*d,*h0,*h1,*h2,*h3,*hi_a,t,dt,**space;
    sds_point *v;


    rc=0; space=NULL; nsa=8;

    /* Allocate it HIS way, just in case -- an array of ptrs pointing */
    /* into one contiguous chunk of memory: */
    if ((space= new sds_real * [ nsa ])==NULL ||
        (space[0]= new sds_real [nsa*n] )==NULL)
            { rc=-3; goto out; }
    for (i=1; i<nsa; i++) space[i]=space[i-1]+n;

    h0=space[0]; h1=space[1]; h2=space[2]; h3=space[3];

    for (k=0; k<3; k++) {  /* GG: X,Y,Z */

        a=space[4]; b=space[5]; c=space[6]; d=space[7];

        /* GG: Trigonal system. */
        for (i=0; i<n; i++) d[i]=q[i][k];
        for (i=0,imax=n-2; i<imax; i++)
            { h3[i]=3.0*(d[i+2]-2.0*d[i+1]+d[i]); h2[i]=1.0; }
        h2[n-3]=0.0;

        /* GG: Dissolution of the system. */
        a[0]=4.0; h1[0]=h3[0]/a[0];
        for (i=1,i1=0,imax=n-2; i<imax; i++,i1++)
            { h0[i1]=h2[i1]/a[i1]; a[i]=4.0-h0[i1]; h1[i]=(h3[i]-h1[i1])/a[i]; }
        b[n-3]=h1[n-3];
        for (i=n-4; i>=0; i--) b[i]=h1[i]-h0[i]*b[i+1];
        for (i=n-2; i>=1; i--) b[i]=b[i-1];
        b[0]=b[n-1]=0.0;
        hi_a=a+n-1;
        for (; a<hi_a; a++,b++,c++,d++)
            { *c=*(d+1)-*d-(2.0**b+*(b+1))/3.0; *a=(*(b+1)-*b)/3.0; }

        /* GG: Store the current components of the vertices of the spline. */
        v=vertices; size=span_size;
        a=space[4]; b=space[5]; c=space[6]; d=space[7];
        for (; a<hi_a; a++,b++,c++,d++) {
            dt=1.0/ *size++;  /* Keep the space between the / and *! */
            for (t=0.0; t<1.0-IC_ZRO; t+=dt) (*v++)[k]=((*a*t+*b)*t+*c)*t+*d;
        }
        (*v++)[k]=*d;
    }
    *m=v-vertices;

out:
    if (space!=NULL) {
        if (space[0]!=NULL) delete space[0];
        IC_FREE(space);
    }

    return rc;
}


//  Trying to get Georg Glaeser's b_spline code to work for
//  scenario 1 (control pts, not fit pts).  Actually, we need
//  a NURBS routine.
//
//  Here's my shot.  Note the memset I used to zero the b[][] array,
//  similar to Rick's solution.  Neither mine nor Rick's quite
//  works right.  Going back to the "fit" routine (above) for both
//  scenarios, for now.
//
//
//  /*F*/
//  void gr_ggbsplinecoeff(
//      sds_real **b,
//      sds_real   t,
//      int        n,
//      int        k,
//      int       *c,
//      int       i1) {
//  /*
//  **  Based on Georg Glaeser's b_spline_coeff(), p.244  in FAST ALGORITHMS
//  **  FOR 3D-GRAPHICS, 1994 (Springer-Verlag).
//  **
//  **  This function is an extension of gr_ggbspline() below.
//  */
//      sds_real *b_ij;
//      int i,j,*ci,*cj;
//      sds_real *max_b=&b[n+1][0];
//      int delta_b=b[1]-b[0];
//
//      ci=&c[i1];
//      for (b_ij=&b[i1][1]; b_ij<max_b; b_ij+=delta_b)
//          *b_ij=(*ci<t && t<=*++ci) ? 1.0 : 0.0;
//      for (j=1; j<k; j++) {
//          i=i1; ci=&c[i1]; cj=ci+j;
//          for (b_ij=&b[i][j+1]; b_ij<max_b; b_ij+=delta_b) {
//              /*
//              **  WARNING:  Here's a suspicious line. *b_ij is a real
//              **  number; I've had trouble in the past with the
//              **  relational test truncating to an int first. (This
//              **  meant that relational expression "(realno)" was false
//              **  for fabs(realno)<1.0.)  I've added "!=0.0" between the
//              **  closing parentheses in the test below.
//              */
//              if ((*b_ij=b[i][j])!=0.0) *b_ij*=(t-*ci)/(*cj-*ci);
//
//              i++; ci++; cj++;
//
//              /* HERE IT IS AGAIN!  VERY SUSPICIOUS.  Again, I've added "!=0.0" */
//              if (b[i][j]!=0.0) *b_ij+=(*cj-t)/(*cj-*ci)*b[i][j];
//          }
//      }
//  }
//
//
//  /*F*/
//  int gr_ggbspline(
//      int       *no_of_vertices,
//      sds_point *curve,
//      int        n,
//      sds_point *q,
//      int       *span_size,
//      int        k) {
//  /*
//  **  Based on Georg Glaeser's b_spline_curve(), p.245  in FAST ALGORITHMS
//  **  FOR 3D-GRAPHICS, 1994 (Springer-Verlag).  I changed variable
//  **  types and used OUR means instead of his for such things as
//  **  memory management and IC_ZRO.
//  **
//  **
//  **  Not really impressed with this guy's documentation.  Here are my
//  **  conclusions about the pass parameters, based on what the code
//  **  seems to do (and his meager comments):
//  **
//  **    Results:
//  **
//  **      no_of_vertices : The number of points assigned in the "curve" array.
//  **
//  **      curve : An array of pts ALREADY ALLOCATED large enough
//  **              to handle the results.
//  **
//  **    Given:
//  **
//  **      n : The size of the q array.  (Looks like it needs to be >=3.)
//  **
//  **      q : An array of control points (with n valid elements).
//  **
//  **      span_size : An ARRAY of ints giving the number of pts wanted
//  **                  for each span.  Appears to need an array of
//  **                  n-1 of them, as you'd expect, since a "span"
//  **                  appears to be the arch between 2 adjacent
//  **                  control pts.
//  **
//  **      k : Georg says it's "the number of control points that have an
//  **          influence on the points of the curve.  I think that's the
//  **          "degree", or it MAY be degree+2.  Not sure.
//  **
//  **  Glaeser's comments are preceded by "GG:".
//  **
//  **  Returns gr_getdispobjs() codes.
//  */
//      int i,j,i1,i2,i3,*c,bsize,rc;
//      sds_real t,dt,r,**b;
//      sds_point *curve0=curve,*q1;
//
//
//      b=NULL; c=NULL; rc=0; bsize=(n+1)*(k+1)*sizeof(sds_real);
//
//      /* Allocate c and b. */
//      /* Allocate b HIS way, just in case -- an array of ptrs pointing */
//      /* into one contiguous chunk of memory: */
//      if ((c=        (int *)malloc((n+k)*sizeof(int)))           ==NULL ||
//          (b=  (sds_real **)malloc((n+1)*sizeof(sds_real *)))    ==NULL ||
//          (b[0]=(sds_real *)malloc(bsize))==NULL)
//              { rc=-3; goto out; }
//      for (i=1; i<=n; i++) b[i]=b[i-1]+k+1;
//      memset(b[0],0,bsize);
//
//
//      /* GG: B-spline constants: */
//      i=0; j=1;
//      while (i<k  ) c[i++]=0;
//      while (i<n  ) c[i++]=j++;
//      while (i<n+k) c[i++]=j;
//      i=k-1; i1=i-(k-1); i2=i+(k-1);
//      for (; i<n+1; i++,i1++,i2++) {
//          t=c[i]+IC_ZRO;
//          dt=(c[i+1]-t)/(*span_size)-IC_ZRO;
//          if (i1<0) i1=0;
//          if (i2>n) i2=n;
//          for (j=0; j<*span_size; j++,t+=dt,curve++) {
//              if (t==IC_ZRO) {
//                  GR_PTCPY(*curve,q[0]);
//              } else {
//                  if (j==0 && dt>IC_ZRO) gr_ggbsplinecoeff(b,t,n-1,k,c,i1);
//                  (*curve)[0]=(*curve)[1]=(*curve)[2]=0.0;
//                  for (i3=i1,q1=q+i1; i3<i2; i3++,q1++) {
//                      if ((r=b[i3][k])!=0.0) {
//                          (*curve)[0]+=r*(*q1)[0];
//                          (*curve)[1]+=r*(*q1)[1];
//                          (*curve)[2]+=r*(*q1)[2];
//                      }
//                  }
//              }
//          }
//          span_size++;
//      }
//      GR_PTCPY(*curve,q[n-1]);
//      *no_of_vertices=++curve-curve0;
//
//  out:
//      if (c!=NULL) free(c);
//      if (b!=NULL) {
//          if (b[0]!=NULL) free(b[0]);
//          free(b);
//      }
//
//      return rc;
//  }



//  Rick's version from PEDIT.  It ALMOST works.  Looks like he
//  had trouble with the b[][] array not being fully initialized, too.
//  (See the "for" loop he added at the beginning of the coefficients
//  function.)
//
//  If we keep this, need to handle the second function's return code better.
//
//
//  void gr_ggbsplinecoeff(sds_real **b, sds_real t, int n, int k, int *c, int i1){
//      sds_real *b_ij,*max_b;
//      int i,j,*ci,*cj,delta_b;
//
//      max_b=&b[n+1][0];
//      delta_b=b[1]-b[0];
//
//      for(i=1;i<n+2;i++)
//          for(j=1;j<=k;j++)
//              b[i][j]=0.0;
//
//      ci=&c[i1];
//      for(b_ij=&b[i1][1];b_ij<max_b;b_ij+=delta_b)
//          *b_ij=(*ci<t && t<= *++ci)? 1 : 0;
//
//      for(j=1;j<k;j++){
//          i=i1;
//          ci=&c[i1];
//          cj=ci+j;
//          for(b_ij=&b[i][j+1];b_ij<max_b;b_ij+=delta_b){
//              if((*b_ij=b[i][j]))
//                  *b_ij*=(t-*ci)/(*cj-*ci);
//              i++,ci++;cj++;
//              if(b[i][j])
//                  *b_ij+=(*cj-t)/(*cj - *ci) * b[i][j];
//          }   //end for b_ij
//      }   //end for j
//      /*
//      This is the "readble" code
//      for(i=i1;i<=n;i++)
//          if(c[i]<t && t<c[i+1])b[i][1]=1;
//          else b[i][1]=0;
//      for(j=2;j<=k;j++){
//          for(i=i1;i<=n;i++)
//              b[i][j]=(t-c[i])/(c[i+j-1]-c[i])*b[i][j-1]+
//                  (c[i+j]-t)/(c[i+j]-c[i+1])*b[i+1][j-1];
//      }
//      */
//  }
//  int gr_ggbspline(int *no_of_vertices, sds_point *curve,int n,
//                              sds_point *q,int *span_size,int k){
//      int *c,i,j,i1,i2,i3,orig_no_of_vertices;
//      sds_real t, dt, r,**b;
//      sds_point *curve0,*q1;
//      int ret=RTNORM;
//
//      #define Zero_vec(v) v[0]=v[1]=v[2]=0.0
//
//      curve0=curve;
//      orig_no_of_vertices=*no_of_vertices;
//
//      c=(int *)calloc(n+k+1,sizeof(int));
//      b=(sds_real **)calloc(n+1,sizeof(sds_real *));
//      b[0]=(sds_real *)calloc(n+1,(k+1)*sizeof(sds_real));
//      for(i=1;i<n+1;i++)
//          b[i]=b[i-1]+k+1;
//
//      //B-spline constants
//      i=0;j=1;
//      while(i<k)
//          c[i++]=0;
//      while(i<n)
//          c[i++]=j++;
//      while(i<n+k)
//          c[i++]=j;
//      i=k-1;i1=i-(k-1);i2=i+(k-1);
//      for(;i<n+1;i++,i1++,i2++,span_size++){
//          t=c[i]+IC_ZRO;
//          dt=(c[i+1]-t)/(*span_size)-IC_ZRO;
//          if (i1<0) i1=0;
//          if (i2>n) i2=n;
//          for(j=0;j< *span_size;j++,t+=dt,curve++){
//              if((curve-curve0+1)>orig_no_of_vertices){ret=RTERROR;goto exit;}
//              if(t==IC_ZRO){
//                  ic_ptcpy(*curve,q[0]);
//              }else{
//                  if(j==0 || dt>IC_ZRO)
//                      gr_ggbsplinecoeff(b,t,n-1,k,c,i1);
//                  Zero_vec(*curve);
//                  for(i3=i1,q1=q+i1;i3<i2;i3++,q1++){
//                      if((r=b[i3][k])!=0.0){
//                          (*curve)[0]+=r*(*q1)[0];
//                          (*curve)[1]+=r*(*q1)[1];
//                          (*curve)[2]+=r*(*q1)[2];
//                      }   //end if r
//                  }   //end if i3
//              }   //end if t
//          }   //end for j
//      }   //end for i
//      ic_ptcpy(*curve,q[n-1]);
//      *no_of_vertices=++curve-curve0;
//
//  exit:
//      free(c);
//      free(b[0]);
//      free(b);
//      #undef Zero_vec
//      return (ret==RTNORM) ? 0 : -3;
//  }

#if !defined(GR_USEEXTERNALSPLINE)
/*
**                        SPLINE GENERATION
**
**  For splines, what we need are a couple of algorithms that take
**  spline definition data and create an array of vertices that we can
**  use to create display objects.  All we need back is a simple chain
**  with enough vertices so that the spline appears smooth at the
**  current resolution -- but doesn't have many more than needed.  For
**  example, if two adjacent definition points are to be joined by a
**  path that will only be 10 pixels long, we should probably generate
**  only two or three intermediate points.  If the path is 300 pixels
**  long, we might need a couple dozen or more.  (You can see this
**  in AutoCAD by zooming far out, so that the whole object is just
**  a speck, and then regening and zooming back in.  The spline
**  will be very segmented until you regen again.)
**
**  SPLINE entities as stored in DWG files seems to follow two
**  possible scenarios.  With the exception of degree, the two
**  scenarios have mutually exclusive data.  It therefore makes sense
**  to have TWO spline generation functions, to avoid confusion.
**  Each needs to take the "given" data we know about a spline and
**  the current screen resolution, and allocate an array of vertices
**  for the caller to use in generating display objects.
**
**
**    Scenario 1 (probably ACAD's NURBS form):
**
**      Degree.
**
**      Flag bits specifying "closed", "periodic", and "rational".
**        In ACAD, it appears that all closed SPLINEs are scenario 1 --
**        and they are EXPLICITLY closed: the first and last
**        control points are coincident.
**
**      An array of knots and a single knot tolerance value.
**
**      An array of control points, an array of weights
**        (one weight for each control point), and a single control
**        point tolerance value.
**
**    Scenario 2 (probably ACAD's "quadratic" form):
**
**      Degree.
**
**      An array of fit points and a single fit tolerance value.
**
**      Beginning and ending tangent direction vectors.
**
**      Even though this scenario has no control points or knot
**      values stored in the drawing, ACAD seems to CALCULATE
**      them somehow from the fit points for the sake of entget.
**      Therefore, if possible -- in addition to generating an array
**      of display vertices -- we need the scenario 2 function
**      to generate an array of control points and an array of knot
**      values.  THIS IS NOT ESSENTIAL TO DISPLAY OBJECT GENERATION,
**      BUT OUR entget FUNCTION COULD SURE USE THEM.
**
**  Here are the two functions as we've implemented them.  They are
**  currently calling a function based on an algorithm by a man named
**  Georg Glaeser; that function is included here for reference.
**  (Currently, his routine is called from gr_splinegen2(), and
**  gr_splinegen1() calls gr_splinegen2().)   We need to have the guts
**  of these functions replaced by routines that match ACAD's two
**  spline types.
**
**  The only two typedefs used here are as follows:
**
**      typedef double sds_real;
**      typedef sds_real sds_point[3];
**
**  In Glaeser's function (gr_gg3spline()), there's also an occurrence
**  of one of our #defines:
**
**      #define IC_ZRO 1.0E-10
**
**  Please use malloc/calloc/free instead of C++'s new/delete for
**  memory allocation.
**
**  NOTE: BEFORE USING ANY OF THE PASS PARAMETERS THAT ARE POINTERS,
**  PLEASE CHECK THEM FOR NULL, JUST FOR SAFETY.
*/

int gr_splinegen1(

  /* Given: */           /* DXF code */

    int         deg,     /*    71    */
    int         flags,   /*    70    */

    int         nknots,  /*    72    */
    sds_real   *knot,    /*    40    */
    sds_real    ktol,    /*    42    */

    int         ncpts,   /*    73    */
    sds_point  *cpt,     /*    10    */
    sds_real   *wt,      /*    41    */
    sds_real    ctol,    /*    43    */

    sds_real   resocoeff,

  /* Results: */

    int        *nverts_p,
    sds_point **vertex_pp) {
/*
**                     THE SCENARIO-1 FUNCTION
**
**        deg : Degree.
**      flags : A bit-coded int, assigned as follows:
**
**                xxxxxRPC
**                76543210
**
**                C (bit 0, value 1) : Closed
**                P (bit 1, value 2) : Periodic
**                R (bit 2, value 4) : Rational (at least 1 non-1.0 wt?)
**
**     nknots : The number of knot values (knot).
**       knot : Array of knot values (nknots elements).
**       ktol : Knot tolerance
**
**      ncpts : The number of control points (cpt) and the number
**                of associated weights (wt).
**        cpt : Array of control points (ncpts elements).
**         wt : Array of weights for each control point (ncpts elements).
**       ctol : Control point tolerance.
**
**  resocoeff : See description in gr_splinegen2().
**
**   nverts_p : This function must set *nverts_p to the number of elements
**                allocated for the *vertex_pp array.  (nverts_p is the address
**                of the caller's int variable for the number of
**                vertices.)
**  vertex_pp : This function must allocate and assign an array of
**                points and assign the address of that array to *vertex_pp.
**                (vertex_pp is the address of the caller's pointer to an
**                sds_point.)
**
**  Returns:
**       0 : OK
**      -1 : Bad calling parameter(s)
**      -2 : Insufficient memory
**      -3 : ?  (Invent any necessary ones; please keep them negative.)
*/
    sds_point btandir,etandir;

    /* Set the tangent directions as zero-vectors (unspecified): */
    btandir[0]=btandir[1]=btandir[2]=0.0;
    etandir[0]=etandir[1]=etandir[2]=0.0;

    /* Just calls gr_splinegen2() for now, passing the control points */
    /* as fit points: */
    return gr_splinegen2(
               deg,
               ncpts,     /* nfpts */
               cpt,       /* fpt */
               ctol,      /* ftol */
               btandir,
               etandir,
               resocoeff,
               nverts_p,
               vertex_pp,
               NULL,      /* ncpts */
               NULL,      /* cpt */
               NULL,      /* nknots */
               NULL);     /* knot */
}


int gr_splinegen2(

  /* Given: */              /* DXF code */

    int         deg,        /*    71    */

    int         nfpts,      /*    74    */
    sds_point  *fpt,        /*    11    */
    sds_real    ftol,       /*    44    */

    sds_point   btandir,    /*    12    */
    sds_point   etandir,    /*    13    */

    sds_real    resocoeff,

  /* Results: */

    int        *nverts_p,
    sds_point **vertex_pp,

    int        *ncpts_p,    /*    73    */
    sds_point **cpt_pp,     /*    10    */

    int        *nknots_p,   /*    72    */
    sds_real  **knot_pp) {  /*    40    */
/*
**                     THE SCENARIO-2 FUNCTION
**
**        deg : Degree.
**
**      nfpts : The number of fit points (fpt).
**        fpt : Array of fit points (nfpts elements).
**       ftol : Fit tolerance.
**
**    btandir : The beginning tangent direction vector.
**                The zero-vector means "unspecified".
**    etandir : The ending    tangent direction vector.
**                The zero-vector means "unspecified".
**
**  resocoeff : Resolution coefficient.  Basically, the square root
**                of the number of pixels per drawing unit at the
**                current zoom (adjusted for the effect of scaling of
**                blocks containing the entity).  We use it in the
**                formula
**
**                  n = (int)(resocoeff*sqrt(d));
**
**                where d is the distance between two adjacent
**                definition points in the spline's native coordinate
**                system, and n is the number of points to generate
**                between them.
**
**                This or some similar formula should probably
**                be used; it's similar to ACAD's circle resolution
**                formula.
**
**   nverts_p : This function must set *nverts_p to the number of elements
**                allocated for the *vertex_pp array.  (nverts_p is the address
**                of the caller's int variable for the number of
**                vertices.)
**  vertex_pp : This function must allocate and assign an array of
**                points and assign the address of that array to *vertex_pp.
**                (vertex_pp is the address of the caller's pointer to an
**                sds_point.)
**
**    ncpts_p : If possible, this function should set *ncpts_p to the number
**                of elements  allocated for the *cpt_pp array.  (ncpts_p is
**                the address of the caller's int variable for the
**                number of control points.)
**     cpt_pp : If possible, this function should allocate and assign
**                an array of sds_points and assign the address of that
**                array to *cpt_pp.  (cpt_pp is the address of the caller's
**                pointer to an sds_point.)
**
**   nknots_p : If possible, this function should set *nknots_p to the number
**                of elements allocated for the *knot_pp array.  (nknots_p is
**                the address of the caller's int variable for the
**                number of knot values.)
**    knot_pp : If possible, this function should allocate and assign
**                an array of sds_reals and assign the address of that
**                array to *knot_pp.  (knot_pp is the address of the caller's
**                pointer to an sds_real.)
**
**  Returns:
**       0 : OK
**      -1 : Bad calling parameter(s)
**      -2 : Insufficient memory
**      -3 : ?  (Invent any necessary ones; please keep them negative.)
*/
    int rc,*span_size,maxperspan,pidx,fint1;
    sds_real delta[3];


    rc=0; span_size=NULL;
    maxperspan=20;  /* Limit the number of points per span (for safety). */


    /* Check pass parameters (Glaeser seems to need nfpts>=3): */
    if (nfpts<3 || fpt==NULL || resocoeff<=0.0 || nverts_p==NULL ||
        vertex_pp==NULL) { rc=-1; goto out; }

    /* Init the vertex vars: */
    *nverts_p=0; *vertex_pp=NULL;

    /* Glaeser's algorithm doesn't use deg, btandir, or etandir. */

    /* We don't know how to calculate the control points or knots, so... */
    /* (Good form anyway: init to safe values.) */
    if (ncpts_p !=NULL) *ncpts_p =0;  if (cpt_pp !=NULL) *cpt_pp =NULL;
    if (nknots_p!=NULL) *nknots_p=0;  if (knot_pp!=NULL) *knot_pp=NULL;

    /* Prepare Glaeser's "span_size" array (nfpts-1 elements): */
    if ((span_size= new int [nfpts-1] )==NULL)
        { rc=-2; goto out; }
	memset(span_size,0,sizeof(int)*(nfpts-1));

    /* Assign span_size[] according to resolution: */
    for (pidx=1; pidx<nfpts; pidx++) {
        fint1=pidx-1;
        delta[0]=fpt[pidx][0]-fpt[fint1][0];
        delta[1]=fpt[pidx][1]-fpt[fint1][1];
        delta[2]=fpt[pidx][2]-fpt[fint1][2];
        span_size[fint1]=(int)(resocoeff*
            pow(delta[0]*delta[0]+delta[1]*delta[1]+delta[2]*delta[2],0.25));
        if      (span_size[fint1]<1)          span_size[fint1]=1;
        else if (span_size[fint1]>maxperspan) span_size[fint1]=maxperspan;
    }

    /* Glaeser expects the destination array to already be allocated.  */
    /* He fills it and tells us how many elements he used -- so we have */
    /* to make the large enough for any possibility: */
    if (((*vertex_pp)= new sds_point [nfpts*maxperspan] )==NULL) { rc=-2; goto out; }
	memset(*vertex_pp,0,sizeof(sds_point)*nfpts*maxperspan);

    /* Okay.  Call Glaeser's routine: */
    if (gr_gg3spline(nverts_p,*vertex_pp,nfpts,fpt,span_size)) { rc=-2; goto out; }


out:   /* I like only ONE exit from a function.  Easier to clean up. */

    if (span_size!=NULL) IC_FREE(span_size);

    if (rc) {
        if (nverts_p!=NULL) *nverts_p=0;
        if (vertex_pp!=NULL && *vertex_pp!=NULL)
            { IC_FREE(*vertex_pp); *vertex_pp=NULL; }
    }

    return rc;
}
#endif



