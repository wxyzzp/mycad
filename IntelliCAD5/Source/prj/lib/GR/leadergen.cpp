/* 
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * functions to generate leader
 * 
 */ 

#include "gr.h"
#include "DoBase.h"
#include "splines.h"
#include "dimstyletabrec.h"
#include "linetypegen.h"
#include "pc2dispobj.h"
#include  "DbMText.h"

// EBATECH(watanabe)-[arrow type
bool isStartOrigin(const char* blockname)
{
    if (strisame(blockname, "_DOTSMALL"/*DNT*/) ||
        strisame(blockname, "_SMALL"/*DNT*/   ) ||
        strisame(blockname, "_NONE"/*DNT*/    ) ||
        strisame(blockname, "_OBLIQUE"/*DNT*/ ) ||
        strisame(blockname, "_INTEGRAL"/*DNT*/) ||
        strisame(blockname, "_ARCHTICK"/*DNT*/))
    {
      return true;
    }
    return false;
}
// ]-EBATECH(watanabe)

// SystemMetrix(watanabe)-[FIX:The wrong direction of underline of mirrored leader
//////////////////////////////////////////////////////////////////////
// If angle is left side
// return value : TRUE  <- angle is left  side
//                FALSE <- angle is right side
BOOL
IsLeftSide(
  double    angle
)
{
  while (angle < 0)
    {
      angle += IC_TWOPI;
    }

  while (angle > IC_TWOPI)
    {
      angle -= IC_TWOPI;
    }

  if (angle < IC_PI * 1.5 && angle > IC_PI * 0.5)
    {
      return TRUE;
    }

  return FALSE;
}
// ]-SystemMetrix(watanabe) FIX:The wrong direction of underline of mirrored leader

// EBATECH(watanabe)-[get hook dir
static void
GetHookDir(
  // SystemMetrix(watanabe)-[FIX:The wrong direction of underline of mirrored leader
  db_drawing*   dp,
  resbuf*       wcsrb,
  resbuf*       ecsrb,
  // ]-SystemMetrix(watanabe) FIX:The wrong direction of underline of mirrored leader
  sds_point     wcsdest[],
  const int     nwcsverts,
  sds_point&    wcshookdir
)
{
  if (nwcsverts < 2)
    {
      return;
    }

  // SystemMetrix(watanabe)-[FIX:The wrong direction of underline of mirrored leader
  //double d0 = wcsdest[nwcsverts - 2][0];
  //double d1 = wcsdest[nwcsverts - 1][0];
  //if (d0 > d1)
  // rotate points
  sds_point pt0, pt1;
  dp->trans(wcsdest[nwcsverts - 2], wcsrb, ecsrb, 0, pt0, NULL);
  dp->trans(wcsdest[nwcsverts - 1], wcsrb, ecsrb, 0, pt1, NULL);

  // rotate x dir
  sds_point xdir;
  dp->trans(wcshookdir, wcsrb, ecsrb, 1, xdir, NULL);

  // angle between first point and second point
  double dx, dy;
  double angle0 = 0;
  dx = pt1[0] - pt0[0];
  dy = pt1[1] - pt0[1];
  if (icadRealEqual(dx,0.0) && icadRealEqual(dy,0.0))
    {
    }
  else
    {
      angle0 = atan2(dy, dx);
    }

  // angle of x direction
  double anglex = 0;
  dx = xdir[0];
  dy = xdir[1];
  if (icadRealEqual(dx,0.0) && icadRealEqual(dy,0.0))
    {
    }
  else
    {
      anglex = atan2(dy, dx);
    }

  double angle = angle0 - anglex;

  if (IsLeftSide(angle))
  // ]-SystemMetrix(watanabe) FIX:The wrong direction of underline of mirrored leader
    {
      wcshookdir[0] = -wcshookdir[0];
      wcshookdir[1] = -wcshookdir[1];
      wcshookdir[2] = -wcshookdir[2];
    }
}
// ]-EBATECH(watanabe)

int gr_leader2doll(
    struct gr_displayobject **begdopp,
    struct gr_displayobject **enddopp,
    short                     projmode,  /* Projection mode for recursion call to gr_getdipsobjs(). */
    gr_view           *viewp,
    db_drawing               *dp,
	CDC						 *dc) {
/*
**  An extension of gr_getdispobjs().
**  Allocates a display objects llist for LEADER. *begdopp is pointed
**  to the first link; *enddopp is pointed to the last link.
**
**  Minimal error-checking is done here; that's up to gr_getdispobjs().
**
**  Returns: gr_getdispobjs() codes.
**
** History
** EBATECH(CNBR) 24-03-2003 Bugzilla#78471 Support Lineweight DIMLWD(371)
*/
    // EBATECH(watanabe)-[dimldrblk
    //char *dimblk,*dimblk1,*blockname;
    char blockname[256];
    // ]-EBATECH(watanabe)
    short dimclrd,linecolor;
    // EBATECH(watanabe)-[dimldrblk
    //int dimsah,nwcsverts,nwcsdests,didx,splined,reversehook;
    int nwcsverts,nwcsdests,didx,splined,reversehook;
    // ]-EBATECH(watanabe)
    int arrowok,arrowenabled,chopidx,createdwith,rc,fi1;
    short savcolor;
    sds_real resocoeff,dimasz,sdimasz,cshookang,arrowrot,csarrowrot,snarrowrot;
    sds_real dimscale,dimgap,ar1;
    sds_point *wcsvert,*wcsdest,wcshookdir,ecsarrowtip,wcsarrowvect,ap1,ap2;
    resbuf appacadrb,wcsrb,ecsrb,*eedrbll,*trbp1;
    db_dimstyletabrec *dship;
	db_handitem *annothip,*bthip,*arrowhip,*thip1;
    db_leader *lp;
    gr_displayobject *doll[2],*tbegdo,*tenddo;
    gr_pclink *prevpcp,*thispcp;
    sds_point laxis[3];  /* The LEADER coordinate system axes in the WCS. */
    sds_real mtextboxdim[2];  /* MTEXT box width and height. */
    bool bMtextBox = false; /* Suceed get MTEXT box or not. */
    // EBATECH(watanabe)-[add
    int dimtad = 0;
    // ]-EBATECH(watanabe)
    int savlweight, dimlwd = DB_BBWEIGHT;	/* Lineweight */

    {
        /*
        **  Okay.  There's so much going on in here and I keep adding
        **  "if"s around stuff so often that I'm going to init everything
        **  but temps to safe defaults this time to make it hard for garbage to
        **  survive.  Don't "goto out" in here.
        */
        rc=0;
        // EBATECH(watanabe)-[dimldrblk
        //dimblk=dimblk1=blockname=NULL;
        //linecolor=dimclrd=7; dimsah=nwcsverts=nwcsdests=splined=reversehook=0;
        linecolor=dimclrd=7; nwcsverts=nwcsdests=splined=reversehook=0;
        // ]-EBATECH(watanabe)
        arrowok=arrowenabled=1; chopidx=0; createdwith=3;
        resocoeff=csarrowrot=dimscale=1.0; dimgap=0.09; dimasz=sdimasz=0.18;
        arrowrot=snarrowrot=0.0;
        wcsvert=wcsdest=NULL;
        wcshookdir[0]=1.0; wcshookdir[1]=wcshookdir[2]=0.0;
        ecsarrowtip[0]=ecsarrowtip[1]=ecsarrowtip[2]=0.0;
        wcsarrowvect[0]=1.0; wcsarrowvect[1]=0.0; wcsarrowvect[2]=0.0;
        eedrbll=NULL; 
		dship=NULL;
		annothip=bthip=arrowhip=NULL;
        doll[0]=doll[1]=tbegdo=tenddo=NULL;
        mtextboxdim[0]=mtextboxdim[1]=0.0;

        savcolor=viewp->color;
		savlweight=viewp->lweight;

        cshookang=cos(IC_PI/12.0);  /* cos(15 deg) -- for determining when */
                                    /* to add hook-lines. */

        lp=(db_leader *)viewp->ment;

        /* Set the ACAD app name (and never relrb it): */
        appacadrb.rbnext=NULL;
        appacadrb.restype=RTSTR;
        appacadrb.resval.rstring="ACAD";

        /* Set up the two trans() resbufs: */
        wcsrb.rbnext=NULL; wcsrb.restype=0;
        ecsrb.rbnext=NULL; ecsrb.restype=RT3DPOINT;
            ecsrb.resval.rpoint[0]=ecsrb.resval.rpoint[1]=0.0;
            ecsrb.resval.rpoint[2]=1.0;
    }


          /* GET STUFF WE'LL NEED FROM THE LEADER ENTITY */

    /* Set DIMSTYLE rec ptr: */
    viewp->ment->get_3((db_handitem **)&dship);
    /* If NULL, try for STANDARD: */
    if (dship==NULL)
		{
		dship=(db_dimstyletabrec *)dp->findtabrec(DB_DIMSTYLETAB,gr_str_standard,1);
		}

    /* Get a pointer to the array data (don't free!): */
    wcsvert=lp->retp_10array(); viewp->ment->get_76(&nwcsverts);
    if (wcsvert==NULL || nwcsverts<2) goto out;

    /* The MTEXT box dimensions: */
    viewp->ment->get_40(mtextboxdim+1);
    viewp->ment->get_41(mtextboxdim);

    /* Arrow enabled? */
    viewp->ment->get_71(&arrowenabled); arrowenabled=(arrowenabled!=0);

    /* Splined? */
    viewp->ment->get_72(&splined); splined=(splined!=0);

    /* What type of entity was it created with: */
    viewp->ment->get_73(&createdwith);

    /* Hook direction opposite hdir? */
    viewp->ment->get_74(&reversehook);

    /* wcshookdir: */
    viewp->ment->get_211(laxis[0]);
    if (icadRealEqual((ar1=GR_LEN(laxis[0])),0.0)) laxis[0][0]=1.0;
    else if (!icadRealEqual(ar1,1.0)) GR_UNITIZE(laxis[0],ar1);
    GR_PTCPY(wcshookdir,laxis[0]);

    // EBATECH(watanabe)-[get hook dir
    //if (reversehook) {
    //    wcshookdir[0]=-wcshookdir[0];
    //    wcshookdir[1]=-wcshookdir[1];
    //    wcshookdir[2]=-wcshookdir[2];
    //}
    // ]-EBATECH(watanabe)

    /* extru: */
    viewp->ment->get_210(laxis[2]);

    /* The associated annotation entity: */
    viewp->ment->get_340(&annothip);

    // EBATECH(watanabe)-[add
    /* The actual MTEXT size: */
    char str[256];
    // EBATECH(watanabe)-[after undo, annothip is NULL.
    //annothip->get_0(str, 255);
    str[0] = '\0';
    if (annothip) annothip->get_0(str, 255);
    // ]-EBATECH(watanabe)
    if (strsame(str, "MTEXT")) {
        sds_point p0, p1;
        CDbMText* mtext = (CDbMText*)annothip;
        if( mtext->ret_disp())
            bMtextBox = true;
#ifdef _USE_DISP_OBJ_PTS_3D_
//#pragma message("_USE_DISP_OBJ_PTS_3D_ defined in leadergen.cpp")
		CD3 minPoint, maxPoint;
		CDoBase::getPlanarBoundingBox(viewp, mtext, minPoint, maxPoint);
		memcpy(p0, (sds_real*)minPoint, 3 * sizeof(sds_real));
		memcpy(p1, (sds_real*)maxPoint, 3 * sizeof(sds_real));
#else
		mtext->get_extent(p0, p1);
#endif
        mtextboxdim[0] = fabs(p0[0] - p1[0]);
        mtextboxdim[1] = fabs(p0[1] - p1[1]);
    }
    // ]-EBATECH(watanabe)


                     /* SET UP THE LEADER AXES */

    fi1=0;  /* Set to 1 if we have to use a default.  (Should never.) */

    /* laxis[1] (Y) is laxis[2] (extru) x laxis[0] (horiz dir): */
    laxis[1][0]=laxis[2][1]*laxis[0][2]-laxis[2][2]*laxis[0][1];
    laxis[1][1]=laxis[2][2]*laxis[0][0]-laxis[2][0]*laxis[0][2];
    laxis[1][2]=laxis[2][0]*laxis[0][1]-laxis[2][1]*laxis[0][0];
    if (icadRealEqual((ar1=GR_LEN(laxis[1])),0.0)) fi1=1;
    else if (!icadRealEqual(ar1,1.0)) GR_UNITIZE(laxis[1],ar1);

    if (!fi1) {  /* Correct the extrusion (X x Y). */
        laxis[2][0]=laxis[0][1]*laxis[1][2]-laxis[0][2]*laxis[1][1];
        laxis[2][1]=laxis[0][2]*laxis[1][0]-laxis[0][0]*laxis[1][2];
        laxis[2][2]=laxis[0][0]*laxis[1][1]-laxis[0][1]*laxis[1][0];
        if (icadRealEqual((ar1=GR_LEN(laxis[2])),0.0)) fi1=1;
        else if (!icadRealEqual(ar1,1.0)) GR_UNITIZE(laxis[2],ar1);
    }

    if (fi1) {
        laxis[0][1]=laxis[0][2]=laxis[1][0]=laxis[1][2]=laxis[2][0]=laxis[2][1]=0.0;
        laxis[0][0]=laxis[1][1]=laxis[2][2]=1.0;
    }

    GR_PTCPY(ecsrb.resval.rpoint,laxis[2]);  /* Corrected extru for ECS */


                 /* SET VALUES FROM THE DIMSTYLE: */

    if (dship!=NULL) {
        // EBATECH(watanabe)-[dimldrblk
        //dship->get_5(  &dimblk);
        //dship->get_6(  &dimblk1);

        // SystemMetrix(Hiro)-[Change the method of having data of DIMLDRBLK in DIMSTYLE and in EED
        //dship->get_341(  blockname, 255);
        dship->get_341name(blockname, 256);
        // ]-SystemMetrix(Hiro) Change the method of having data of DIMLDRBLK in DIMSTYLE and in EED
        // ]-EBATECH(watanabe)
        dship->get_40( &dimscale);
        dship->get_41( &dimasz);
        dship->get_147(&dimgap);
        // EBATECH(watanabe)-[dimldrblk
        //dship->get_173(&dimsah);
        // ]-EBATECH(watanabe)
        // EBATECH(watanabe)-[add
        dship->get_77(&dimtad);
        // ]-EBATECH(watanabe)
        int	tempi;
		dship->get_176(&tempi);
		dimclrd=shortCast(tempi);
		dship->get_371(&dimlwd);
    }


              /* OVERRIDE DIMSTYLE VALUES USING EED */

    eedrbll=viewp->ment->get_eed(&appacadrb,NULL);

    trbp1=eedrbll; while (trbp1!=NULL && (trbp1->restype!=1000 ||
        stricmp(trbp1->resval.rstring,"DSTYLE"))) trbp1=trbp1->rbnext;
    /* Matching { for the closing one below */
    for (; trbp1!=NULL && (trbp1->restype!=1002 ||
        *trbp1->resval.rstring!='}'); trbp1=trbp1->rbnext) {

        if (trbp1->restype!=1070 || trbp1->rbnext==NULL) continue;

        fi1=trbp1->resval.rint; trbp1=trbp1->rbnext;
        switch (fi1) {
            // EBATECH(watanabe)-[dimldrblk
            //case   5:
            //    if (trbp1->restype!=1000) break;
            //    dimblk=trbp1->resval.rstring;
            //    break;
            //case   6:
            //    if (trbp1->restype!=1000) break;
            //    dimblk1=trbp1->resval.rstring;
            case 341:
                if (trbp1->restype != 1005) break;

                // SystemMetrix(Hiro)-[Change the method of having data of DIMLDRBLK in DIMSTYLE and in EED
                //if (strlen(trbp1->resval.rstring) == 0) {
                //    blockname[0] = '\0';
                //} else {
                blockname[0] = '\0';
                if (strlen(trbp1->resval.rstring)) {
                // ]-SystemMetrix(Hiro) Change the method of having data of DIMLDRBLK in DIMSTYLE and in EED

                    thip1 = dp->handent(trbp1->resval.rstring);
                    if (thip1 != NULL) {
                        // Bugzilla::78558; 24/06/2003; [
						//if (thip1->ret_type() == DB_BLOCKTABREC) {
                        //    db_blocktabrec* tship = (db_blocktabrec*)thip1;
                        //    if (tship != NULL) {
                        //        tship->get_2(blockname, 255);
                        //    }
                        //}
						assert(thip1->ret_type() == DB_BLOCKTABREC);
						thip1->get_2(blockname, 255);
						// Bugzilla::78558; 24/06/2003; ]
                    }
                }
            // ]-EBATECH(watanabe)
                break;
            case  40:
                if (trbp1->restype!=1040) break;
                dimscale=trbp1->resval.rreal;
                break;
            case  41:
                if (trbp1->restype!=1040) break;
                dimasz=trbp1->resval.rreal;
                break;
            // EBATECH(watanabe)-[add
            case 77:
                if (trbp1->restype!=1070) break;
                dimtad=trbp1->resval.rint;
                break;
            // ]-EBATECH(watanabe)
            case  147:
                if (trbp1->restype!=1040) break;
                dimgap=trbp1->resval.rreal;
                break;
            // EBATECH(watanabe)-[dimldrblk
            //case 173:
            //    if (trbp1->restype!=1070) break;
            //    dimsah=trbp1->resval.rint;
            //    break;
            // ]-EBATECH(watanabe)
            case 176:
                if (trbp1->restype!=1070) break;
                dimclrd=trbp1->resval.rint;
                break;
            case 371:
                if (trbp1->restype!=1070) break;
                dimlwd=trbp1->resval.rint;
                break;
        }
    }

    /* Precautions: */
    if (dimasz<0.0) dimasz=-dimasz;
    if (dimscale<0.0) dimscale=-dimscale;
    else if (icadRealEqual(dimscale,0.0)) dimscale=1.0;  /* ACAD allows 0.0, so don't change the setter. */

    sdimasz=dimasz*fabs(dimscale);  /* Scaled dimasz */

    /*
    **  Resolve the color.  If DIMCLRD is BYBLOCK, use the 77-group.
    */
    linecolor=dimclrd;

    // SystemMetrix(watanabe)-[FIX: If leader 77-group is not defined, leader color is white.
    // The color to use if leader's DIMCLRD = BYBLOCK is not defined, OTD returns 255.
    // In future, if ODT will be return the valid value, this modification can be removed.
    //viewp->ment->get_77(&fi1);
    //if (linecolor==DB_BBCOLOR) linecolor=shortCast(fi1);
    if (linecolor == DB_BBCOLOR) {
        viewp->ment->get_62((int*)&linecolor);
    }
    // ]-SystemMetrix(watanabe) FIX: If leader 77-group is not defined, leader color is white.

    if (linecolor<DB_BBCOLOR || linecolor>DB_BLCOLOR) linecolor=DB_BLCOLOR;
    if      (linecolor==DB_BLCOLOR) linecolor=viewp->blcolor;
    else if (linecolor==DB_BBCOLOR) linecolor=viewp->bbcolor;

	// EBATECH(CNBR) -[ 24-03-2003 Bugzilla#78471 Support Lineweight
	if( dimlwd == DB_BBWEIGHT )
		viewp->ment->get_370(&dimlwd);
	if( dimlwd == DB_BBWEIGHT )
		dimlwd = viewp->bblweight;
	if( dimlwd == DB_BLWEIGHT )
		dimlwd = viewp->bllweight;
	if(!db_is_validweight(dimlwd, DB_DFWMASK))
		dimlwd = DB_DFWEIGHT;
	// EBATECH(CNBR) ]-

    if (arrowenabled) {
        // EBATECH(watanabe)-[dimldrblk
        ///*
        //**  Determine what kind of "arrow" we're going to draw (if everything
        //**  works out).  Set blockname to NULL for SOLID, or to dimblk or
        //**  dimblk1.  ACAD uses the following logic:
        //**
        //**  DIMBLK set  DIMBLK1 set  DIMSAH  Result
        //**  ----------  -----------  ------  -------
        //**       *           -          1    DIMBLK
        //**       *           -          0    DIMBLK
        //**       *           *          0    DIMBLK
        //**       *           *          1    DIMBLK1
        //**       -           *          1    DIMBLK1
        //**       -           *          0    SOLID
        //**       -           -          0    SOLID
        //**       -           -          1    SOLID
        //*/
        //fi1=(dimblk1!=NULL && *dimblk1 && dimsah);
        //
        ////Modified Cybage AW 07-02-01 [
        ////Reason : Fix for bug related to Leaders reported by Surya Sarda
        ///*
        //blockname=(dimblk!=NULL && *dimblk) ?
        //    ((fi1) ? dimblk1 : dimblk) :
        //    ((fi1) ? dimblk1 : NULL);
        //*/
        //char *chBlkName = new char[512];
        //dp->get_dimldrblk(chBlkName);   
        //blockname = chBlkName;
        ////Modified Cybage AW 07-02-01 ]

        ///* Set the pointer to the block table record (or NULL): */
        //bthip=(blockname==NULL) ? NULL : dp->findtabrec(DB_BLOCKTAB,blockname,1);
        bthip = NULL;
        if (strlen(blockname) > 0)
          {
            bthip = dp->findtabrec(DB_BLOCKTAB,blockname,1);
          }
        // ]-EBATECH(watanabe)
    }

    /*
    **  Allocate and set wcsdest[] for the line segs to draw --
    **  via the spline generator, or by taking wcsvert[] and adding
    **  a hookline joint, if necessary:
    */
    if (splined) {
        /*
        **  Call the "fit" spline generator, using default deg, ftol,
        **  and tandir[0].
        */

        ap1[0]=ap1[1]=ap1[2]=0.0;  /* tandir[0] (and maybe tandir[1], too) */

        /* See SPLINE in gr_getdispobjs() for explanation of resocoeff: */
        resocoeff=1.0;
        resocoeff*=sqrt(fabs(viewp->absxsclp * viewp->GetReciprocalOfPixelHeight() ) );

        rc=gr_splinegen2(3,nwcsverts,wcsvert,1.0E-10,ap1,
            (createdwith<2) ? wcshookdir : ap1, FALSE, resocoeff,&nwcsdests,&wcsdest,
            NULL,NULL,NULL,NULL);
        if (rc) { rc=(rc==-2) ? -3 : -1; goto out; }

        // EBATECH(watanabe)-[get hook dir
        // SystemMetrix(watanabe)-[FIX:The wrong direction of underline of mirrored leader
        //GetHookDir(wcsdest, nwcsverts, wcshookdir);
        GetHookDir(dp, &wcsrb, &ecsrb, wcsdest, nwcsverts, wcshookdir);
        // ]-SystemMetrix(watanabe) FIX:The wrong direction of underline of mirrored leader
        // ]-EBATECH(watanabe)
    } else {
        /* Make wcsdest for nwcsverts+1 elements, in case we need to generate */
        /* a hook: */
        if ((wcsdest= new sds_point [nwcsverts+1] )==NULL)
            { rc=-3; goto out; }
		memset(wcsdest,0,sizeof(sds_point)*(nwcsverts+1));

        nwcsdests=nwcsverts;  /* Will be incremented later if it has a hook. */

        /* Get the verts from the database: */
        viewp->ment->get_10(wcsdest,nwcsverts);

        // EBATECH(watanabe)-[get hook dir
        // SystemMetrix(watanabe)-[FIX:The wrong direction of underline of mirrored leader
        //GetHookDir(wcsdest, nwcsverts, wcshookdir);
        GetHookDir(dp, &wcsrb, &ecsrb, wcsdest, nwcsverts, wcshookdir);
        // ]-SystemMetrix(watanabe) FIX:The wrong direction of underline of mirrored leader
        // ]-EBATECH(watanabe)

        /* Get the last vector and unitize it: */
        ap1[0]=wcsdest[nwcsverts-1][0]-wcsdest[nwcsverts-2][0];
        ap1[1]=wcsdest[nwcsverts-1][1]-wcsdest[nwcsverts-2][1];
        ap1[2]=wcsdest[nwcsverts-1][2]-wcsdest[nwcsverts-2][2];
        if (icadRealEqual((ar1=GR_LEN(ap1)),0.0)) ap1[0]=1.0;
        else if (!icadRealEqual(ar1,1.0)) GR_UNITIZE(ap1,ar1);

        /* Does this LEADER have an associated MTEXT or TOLERANCE, and is */
        /* the angle it makes with wcshookdir less than the minimum hook-angle? */
        // EBATECH(watanabe)-[
        ///* (TODO: May want to actually check annothip someday (when we */
        ///* have it) instead of createdwith.) */
        //if (createdwith<2 && ap1[0]*wcshookdir[0]+ap1[1]*wcshookdir[1]+
        //    ap1[2]*wcshookdir[2]<cshookang) {
        //    /* Copy the last element down one slot and fix the hook joint: */
        //    GR_PTCPY(wcsdest[nwcsverts],wcsdest[nwcsverts-1]);
        //    wcsdest[nwcsverts-1][0]-=sdimasz*wcshookdir[0];
        //    wcsdest[nwcsverts-1][1]-=sdimasz*wcshookdir[1];
        //    wcsdest[nwcsverts-1][2]-=sdimasz*wcshookdir[2];
        //    nwcsdests++;
        if (annothip != NULL) {
            if (!annothip->ret_deleted()) {
                sds_real d = ap1[0] * wcshookdir[0] +
                             ap1[1] * wcshookdir[1] +
                             ap1[2] * wcshookdir[2];

                // has hook line
                if (createdwith < 2) {
                    // over 15 degree
                    if (d < cshookang) {
                        GR_PTCPY(wcsdest[nwcsverts], wcsdest[nwcsverts - 1]);
                        wcsdest[nwcsverts - 1][0] -= sdimasz * wcshookdir[0];
                        wcsdest[nwcsverts - 1][1] -= sdimasz * wcshookdir[1];
                        wcsdest[nwcsverts - 1][2] -= sdimasz * wcshookdir[2];
                        nwcsdests++;
                    }
                }

                if (strsame(str, "MTEXT")) {
                    if (dimtad != 0) {
                        sds_real width = mtextboxdim[0];
                        width += fabs(dimgap * dimscale);

                        // under 15 degree
                        if (d > cshookang) {
                            // add a line
                            GR_PTCPY(wcsdest[nwcsdests], wcsdest[nwcsdests - 1]);
                            nwcsdests++;
                        }

                        // extend the last line
                        wcsdest[nwcsdests - 1][0] += width * wcshookdir[0];
                        wcsdest[nwcsdests - 1][1] += width * wcshookdir[1];
                        wcsdest[nwcsdests - 1][2] += width * wcshookdir[2];
                    }
                }
            }
        // ]-EBATECH(watanabe)
        }
    }

    if (!arrowenabled) arrowok=0;
    else {
        /*
        **  We need to decide if the first segment of the LEADER is long
        **  enough so that generating an arrow is appropriate.  The
        **  deciding distance seems to be 2.0*sdimasz.  For a splined
        **  LEADER, we have to look at the original wcsvert[] array (not
        **  wcsdest[], which has all of the interpolation points).
        **  (At least that's how ACAD seems to do it.)
        **  For a line-segment LEADER, we have to use the wcsdest[] array, which
        **  may have been adjusted to create a hookline (changing the
        **  length of the first segment, for a LEADER with only 2 points).
        **
        **  The "arrow vector" (wcsarrowvect) must use wcsdest[] for line-seg
        **  case for the same reason as above (the hookline), but for
        **  splined LEADERs we need the point on the spline that's sdimasz
        **  units away from the tip (measured linearly -- not along the
        **  curve).  We'll settle for the last interior wcsdest[] point
        **  that's still within sdimasz of the tip, since that's all that
        **  high school math allows.  (Besides: why waste time in a spline
        **  interpolator just for this?)  The proper idx to cut at is
        **  kept in chopidx (because we'll need it again).
        */
        if (splined) {
            for (chopidx=1; chopidx<nwcsdests; chopidx++) {
                ap1[0]=wcsdest[0][0]-wcsdest[chopidx][0];
                ap1[1]=wcsdest[0][1]-wcsdest[chopidx][1];
                ap1[2]=wcsdest[0][2]-wcsdest[chopidx][2];
                if (GR_LEN(ap1)>=sdimasz) break;
            }
            if (chopidx>1) chopidx--;
            wcsarrowvect[0]=wcsdest[0][0]-wcsdest[chopidx][0];
            wcsarrowvect[1]=wcsdest[0][1]-wcsdest[chopidx][1];
            wcsarrowvect[2]=wcsdest[0][2]-wcsdest[chopidx][2];
            ap1[0]=wcsvert[0][0]-wcsvert[1][0];  /* That's right: the wcsvert[] array. */
            ap1[1]=wcsvert[0][1]-wcsvert[1][1];
            ap1[2]=wcsvert[0][2]-wcsvert[1][2];
            ar1=GR_LEN(ap1);
        } else {
            wcsarrowvect[0]=wcsdest[0][0]-wcsdest[1][0];
            wcsarrowvect[1]=wcsdest[0][1]-wcsdest[1][1];
            wcsarrowvect[2]=wcsdest[0][2]-wcsdest[1][2];
            ar1=GR_LEN(wcsarrowvect);
        }
        arrowok=(ar1>2.0*sdimasz);
    }

    /*
    **  If there's room for an arrow, we need to trim off the first
    **  sdimasz units of the LEADER.  (We HAVE to do it if the "arrow"
    **  is going to be an INSERT; it doesn't hurt if it's going to be
    **  a SOLID.)  That's easy for the line-seg form; for the splined
    **  form, we need the idx of that point that's just inside sdimasz
    **  of the tip (chopidx).  We've already found that.  Just
    **  zero it if there's no room for and arrow:
    */
    // EBATECH(watanabe)-[arrow type
    //if (arrowok && !splined && (ar1=GR_LEN(wcsarrowvect))>0.0) {
    ar1 = GR_LEN(wcsarrowvect);
    if (!isStartOrigin(blockname) && arrowok && !splined && ar1 > 0.0) {
    // ]-EBATECH(watanabe)
        /* Move the 1st point back sdimasz. */
        ar1=sdimasz/ar1;
        wcsdest[0][0]-=wcsarrowvect[0]*ar1;
        wcsdest[0][1]-=wcsarrowvect[1]*ar1;
        wcsdest[0][2]-=wcsarrowvect[2]*ar1;
    }
    if (!arrowok || !splined) chopidx=0;


                         /* THE LINE-WORK */

    viewp->color=linecolor;  /* These new entities just don't like to use */
    viewp->lweight=dimlwd;   /* the entity color and lineweight. */

    /* Note the use of chopidx to decide where to start drawing */
    /* (chopping off the first part of a splined LEADER, if necessary). */

    if (viewp->allocatePointsChain(nwcsdests-chopidx)) 
	{ 
		rc = -3; 
		goto out; 
	}
    viewp->pc->d2go=0.0;
    for (prevpcp=NULL,thispcp=viewp->pc,didx=chopidx; didx<
        nwcsdests; prevpcp=thispcp,thispcp=thispcp->next,didx++) {

        if (viewp->ltsclwblk) GR_PTCPY(thispcp->pt,wcsdest[didx]);
        else gr_ncs2ucs(wcsdest[didx],thispcp->pt,0,viewp);
        thispcp->swid=thispcp->ewid=0.0;
		thispcp->startang=thispcp->endang=IC_ANGLE_DONTCARE;
        /* Set d2go: */
        if (prevpcp!=NULL) {
            /* A subtlety: apparently the linetype starts over for each */
            /* segment of a line-leader: */

            ar1=GR_DIST(prevpcp->pt,thispcp->pt);
            if (splined) viewp->pc->d2go+=ar1; else prevpcp->d2go=ar1;
            thispcp->d2go=-1.0;  /* Safe for either, essential for splined. */
        }

        viewp->lupc=thispcp;
    }

    if ((rc=gr_doltype( (db_entity*)viewp->ment, &tbegdo,&tenddo,viewp,dp,dc))!=0) goto out;

    if (tbegdo!=NULL && tenddo!=NULL) {
        if (doll[0]==NULL) doll[0]=tbegdo; else doll[1]->next=tbegdo;
        doll[1]=tenddo;
    }


                           /* MTEXT BOX */

    /*
    **  TODO: This is NOT the way to do this.  We shouldn't take
    **  the LEADER entity's word for the box's dimensions (mtextboxdim[]
    **  from DXF 40 & 41).  We should get the size and position
    **  from the associated MTEXT itself (DXF 340).  We can't do that
    **  at the moment (because MarComp's not giving us the 340 right
    **  now), so -- we take our best shot (using the 40 & 41).
    **  Things are going to be off, because of discrepancies in font
    **  between us and ACAD and because of LEADERs that have been
    **  moved away from their MTEXTs.
    */
    /* 2004/4/10 EBATECH(CNBR) Bugzilla 78741
        Partialy fixed. Don't draw MTEXT BOX when fail to get mtextboxdim.
    */
    if (createdwith==0 && dimgap<0.0 && bMtextBox) {  /* Associated with an MTEXT */

        /* Adjust mtextboxdim[] by 2.0*fabs(dimgap): */
        // EBATECH(watanabe)-[changed
        //mtextboxdim[0]+=(ar1=2.0*fabs(dimgap)); mtextboxdim[1]+=ar1;
        ar1 = 2.0 * fabs(dimgap * dimscale);
        mtextboxdim[0] += ar1;
        mtextboxdim[1] += ar1;
        // ]-EBATECH(watanabe)

        /* Init pc: */
        if (viewp->allocatePointsChain(5)) 
		{ 
			rc=-3; 
			goto out; 
		}
        viewp->lupc=viewp->pc;

        ar1=0.5*mtextboxdim[1];

        // EBATECH(watanabe)-[changed
        //ap1[0]=wcsvert[nwcsverts-1][0]-ar1*laxis[1][0];
        //ap1[1]=wcsvert[nwcsverts-1][1]-ar1*laxis[1][1];
        //ap1[2]=wcsvert[nwcsverts-1][2]-ar1*laxis[1][2];
        GR_PTCPY(ap1, wcsvert[nwcsverts - 1]);
        if (dimtad == 0) {
            ap1[0] -= ar1 * laxis[1][0];
            ap1[1] -= ar1 * laxis[1][1];
            ap1[2] -= ar1 * laxis[1][2];
        }
        // ]-EBATECH(watanabe)
        if (viewp->ltsclwblk) GR_PTCPY(viewp->lupc->pt,ap1);
        else            gr_ncs2ucs(ap1,viewp->lupc->pt,0,viewp);

        // EBATECH(watanabe)-[changed
        //ap1[0]+=mtextboxdim[0]*laxis[0][0];
        //ap1[1]+=mtextboxdim[0]*laxis[0][1];
        //ap1[2]+=mtextboxdim[0]*laxis[0][2];
        ap1[0] += mtextboxdim[0] * wcshookdir[0];
        ap1[1] += mtextboxdim[0] * wcshookdir[1];
        ap1[2] += mtextboxdim[0] * wcshookdir[2];
        // ]-EBATECH(watanabe)
        if (viewp->ltsclwblk) GR_PTCPY(viewp->lupc->next->pt,ap1);
        else            gr_ncs2ucs(ap1,viewp->lupc->next->pt,0,viewp);
        viewp->lupc->d2go=GR_DIST(viewp->lupc->pt,viewp->lupc->next->pt);
        viewp->lupc->swid=viewp->lupc->ewid=0.0;
        viewp->lupc->startang=viewp->lupc->endang=IC_ANGLE_DONTCARE;
        viewp->lupc=viewp->lupc->next;

        ap1[0]+=mtextboxdim[1]*laxis[1][0];
        ap1[1]+=mtextboxdim[1]*laxis[1][1];
        ap1[2]+=mtextboxdim[1]*laxis[1][2];
        if (viewp->ltsclwblk) GR_PTCPY(viewp->lupc->next->pt,ap1);
        else            gr_ncs2ucs(ap1,viewp->lupc->next->pt,0,viewp);
        viewp->lupc->d2go=GR_DIST(viewp->lupc->pt,viewp->lupc->next->pt);
        viewp->lupc->swid=viewp->lupc->ewid=0.0;
        viewp->lupc->startang=viewp->lupc->endang=IC_ANGLE_DONTCARE;
        viewp->lupc=viewp->lupc->next;

        // EBATECH(watanabe)-[changed
        //ap1[0]-=mtextboxdim[0]*laxis[0][0];
        //ap1[1]-=mtextboxdim[0]*laxis[0][1];
        //ap1[2]-=mtextboxdim[0]*laxis[0][2];
        ap1[0] -= mtextboxdim[0] * wcshookdir[0];
        ap1[1] -= mtextboxdim[0] * wcshookdir[1];
        ap1[2] -= mtextboxdim[0] * wcshookdir[2];
        // ]-EBATECH(watanabe)
        if (viewp->ltsclwblk) GR_PTCPY(viewp->lupc->next->pt,ap1);
        else            gr_ncs2ucs(ap1,viewp->lupc->next->pt,0,viewp);
        viewp->lupc->d2go=GR_DIST(viewp->lupc->pt,viewp->lupc->next->pt);
        viewp->lupc->swid=viewp->lupc->ewid=0.0;
        viewp->lupc->startang=viewp->lupc->endang=IC_ANGLE_DONTCARE;
        viewp->lupc=viewp->lupc->next;

        GR_PTCPY(viewp->lupc->next->pt,viewp->pc->pt);
        viewp->lupc->d2go=GR_DIST(viewp->lupc->pt,viewp->lupc->next->pt);
        viewp->lupc->swid=viewp->lupc->ewid=0.0;
        viewp->lupc->startang=viewp->lupc->endang=IC_ANGLE_DONTCARE;
        viewp->lupc=viewp->lupc->next;

        viewp->lupc->d2go=-1.0;
        viewp->lupc->swid=viewp->lupc->ewid=0.0;
        viewp->lupc->startang=viewp->lupc->endang=IC_ANGLE_DONTCARE;

        if ((rc=gr_doltype( (db_entity*)viewp->ment, &tbegdo,&tenddo,viewp,dp,dc))!=0) goto out;

        if (tbegdo!=NULL && tenddo!=NULL) {
            if (doll[0]==NULL) doll[0]=tbegdo; else doll[1]->next=tbegdo;
            doll[1]=tenddo;
        }

    }

                       /* THE ARROW OR BLOCK */

    /*
    **  We're going to set up a db_handitem (SOLID or INSERT) and make
    **  a recursion call to gr_getdispobjs() to get the display objects
    **  for the "arrow".  Both of these entities use the ECS that
    **  has the same extrusion direction as the LEADER.  We need the
    **  "arrow direction angle" (or its trig vals) in that ECS.
    **
    **  WARNING:
    **
    **    MUST BE DONE LAST because it changes viewp->ment, viewp->xfa[],
    **    etc.  In fact, this can screw up other things.  We had a
    **    problem with nentselp calls to gr_getdispobjs() concerning
    **    LEADERs.  The code at the end of gr_getdispobjs() that
    **    captures the nentselp data was counting on things being
    **    set up for LEADER -- not the SOLID or INSERT used in
    **    making the LEADER.  We fixed it in that function, but it
    **    made things ugly.
    */
    if (arrowok) {  /* 1st seg long enough for "arrow". */

        /* Transform wcsarrowvect into the ECS (as a vector!): */
        dp->trans(wcsarrowvect,&wcsrb,&ecsrb,1,ap1,NULL);

        /* This vector SHOULD be parallel to the ECS XY-plane, but */
        /* don't make such an assumption.  Get the rotation from the */
        /* X and Y components: */
        csarrowrot=ap1[0]; snarrowrot=ap1[1];
        if (icadRealEqual((ar1=sqrt(csarrowrot*csarrowrot+snarrowrot*snarrowrot)),0.0))
            { csarrowrot=1.0; snarrowrot=0.0; }
        else if (!icadRealEqual(ar1,1.0)) { csarrowrot/=ar1; snarrowrot/=ar1; }
        arrowrot=atan2(snarrowrot,csarrowrot);

        /* Both use the ECS version of the 1st pt, too. */
        /* (Use wcsvert[0]; wcsdest[0] may have been moved to chop the */
        /* first segment.) */
        dp->trans(wcsvert[0],&wcsrb,&ecsrb,1,ecsarrowtip,NULL);

        /* Create the proper entity and set the commom parameters: */
        if (bthip==NULL) arrowhip=new db_solid(dp);
        else             arrowhip=new db_insert(dp);
        arrowhip->set_ltypehip(viewp->ment->ret_ltypehip());
        arrowhip->set_layerhip(viewp->ment->ret_layerhip());
        viewp->ment->get_48(&ar1); arrowhip->set_48(ar1);
        viewp->ment->get_60(&fi1); arrowhip->set_60(fi1);
                                   arrowhip->set_62(linecolor);
        viewp->ment->get_67(&fi1); arrowhip->set_67(fi1);

        /* These are common to both, in this case, too: */
        arrowhip->set_10(ecsarrowtip);
        arrowhip->set_210(ecsrb.resval.rpoint);

        if (bthip==NULL) {  /* The arrow is a SOLID */

            /*
            **  Set pts 11-13 (10 was set for the tip to ecsarrowtip above).
            **  The arrow has to be sdimasz long and 1/3 sdimasz wide,
            **  and rotated by arrowrot about ecsarrowtip.  We'll build it
            **  with it's tip at (0,0,0) pointing along the +X-axis
            **  and then rotate and translate it into place.
            */

            /* 2nd quadrant pt: */
            ap1[0]=-sdimasz; ap1[1]=sdimasz/6.0;
            ap2[0]=ap1[0]*csarrowrot-ap1[1]*snarrowrot+ecsarrowtip[0];
            ap2[1]=ap1[1]*csarrowrot+ap1[0]*snarrowrot+ecsarrowtip[1];
            ap2[2]=ecsarrowtip[2];
            arrowhip->set_11(ap2);

            /* 3nd quadrant pt: */
            ap1[1]=-ap1[1];
            ap2[0]=ap1[0]*csarrowrot-ap1[1]*snarrowrot+ecsarrowtip[0];
            ap2[1]=ap1[1]*csarrowrot+ap1[0]*snarrowrot+ecsarrowtip[1];
            arrowhip->set_12(ap2);
            arrowhip->set_13(ap2);

        } else {  /* It's an INSERT. */

            /* Come to think of it, we don't really need local bthip in this */
            /* function, since the following set_2() does the findtabrec(). */
            /* Let's leave it, though; this way, we're checking for the */
            /* existence of the block before we get here. */

            arrowhip->set_2(blockname,dp);
            arrowhip->set_41(sdimasz);
            arrowhip->set_42(sdimasz);
            arrowhip->set_43(sdimasz);
            arrowhip->set_50(arrowrot);

        }

        /* The recursion call to gr_getdispobjs(): */
        thip1=arrowhip;  /* Don't let gr_getdispobjs() NULL arrowhip! */
                         /* We need to delete it in "out". */
        if (arrowhip!=NULL) 
		{
#ifndef _BIG_TRANSFORMATION_COMPUTATION_
			sds_point ucsaxes[3];
			sds_point ucsorigin;
			memcpy(ucsaxes, viewp->ucsaxis, 3 * sizeof(sds_point));
			memcpy(ucsorigin, viewp->ucsorg, sizeof(sds_point));
			memset(viewp->ucsaxis, 0, 3 * sizeof(sds_point));
			memset(viewp->ucsorg, 0, sizeof(sds_point));
			viewp->ucsaxis[0][0] = viewp->ucsaxis[1][1] = viewp->ucsaxis[2][2] = 1.0;
#endif
			if(arrowhip->ret_type() == DB_SOLID)
				((db_solid*)arrowhip)->set_39(0.); // set thickness

            gr_getdispobjs(dp,NULL,&thip1,&tbegdo,&tenddo,NULL,dp,viewp,projmode,dc);
            if (tbegdo!=NULL && tenddo!=NULL) {
                if (doll[0]==NULL) doll[0]=tbegdo; 
				else doll[1]->next=tbegdo;
                doll[1]=tenddo;
				while(tbegdo) {
					tbegdo->SetSourceEntity(lp); // set right (existing) entity 
					tbegdo = tbegdo->next;
				}

            }
#ifndef _BIG_TRANSFORMATION_COMPUTATION_
			memcpy(viewp->ucsaxis, ucsaxes, 3 * sizeof(sds_point));
			memcpy(viewp->ucsorg, ucsorigin, sizeof(sds_point));
#endif
        }

    }


out:
    if (rc && doll[0]!=NULL) {
        gr_freedisplayobjectll(doll[0]);
        doll[0]=doll[1]=NULL;
    }
    if (eedrbll!=NULL) sds_relrb(eedrbll);

    if (wcsdest!=NULL) IC_FREE(wcsdest);

    delete arrowhip;

    if (begdopp!=NULL) *begdopp=doll[0];
    if (enddopp!=NULL) *enddopp=doll[1];

    viewp->color=savcolor;
	viewp->lweight=savlweight;

    return rc;
}


