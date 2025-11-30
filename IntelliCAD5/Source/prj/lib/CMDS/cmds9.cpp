/* D:\DEV\PRJ\LIB\CMDS\CMDS9.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

//*********************************************************************
//*  Cmds9.cpp                                                        *
//*  Copyright (C) 1994,1995,1996,1997,1998 by Visio Corporation      *
//*                                                                   *
//*********************************************************************
//*********************************************************************
//*********************************************************************

//** Includes
#include "cmds.h"/*DNT*/
#include "Icad.h"/*DNT*/
#include "drw.h"/*DNT*/
#include "commandqueue.h"/*DNT*/
#include "IcadApi.h"/*DNT*/
#include "Modeler.h"/*DNT*/
#include "IcadTextScrn.h"/*DNT*/
#include "transf.h"
using icl::transf;
#include "gvector.h"
using icl::gvector;
using icl::point;
#include "CmdQueryTools.h"//ucs2wcs()
#include "splines.h"	// for using of gr_get_ellipse_edata
//Bugzilla No 78016 18-03-2002 
#include "fixup1005.h"
#include "dimensions.h"
//BugZilla No. 78419; 31-03-2003	
#include "cmdsDimBlock.h"
//Bugzilla::50537;
#include "sdsapplication.h"
//** Protos
short cmd_entlister(short argmode); //main function for cmd_list and cmd_dblister
//point printing f'ns moved to header
short cmd_mirrorpt(sds_point defpt, sds_point unorm, sds_point before, sds_point after);
short cmd_mirrorss(sds_name ss, sds_point defpt, sds_point norm, short delold);
short cmd_mirror(void);
short cmd_mirror3d(void);
short cmd_3dmesh(void);

extern int SDS_PointDisplayMode;
extern sds_point SDS_PointDisplay;
extern int SDS_AtNodeDrag;
extern struct cmd_drag_globalpacket cmd_drag_gpak;

//Add by IntelliKorea 2001/5/31
extern void SDS_xformarc(sds_point oldextru, sds_point newextru, sds_matrix wmat, resbuf *ecs, resbuf *ucs, resbuf *center,
				 resbuf *radius, resbuf *start, resbuf *end);
//IntelliKorea 2001/5/31

//====================================================================
//*********************** Beginning of functions *********************
//====================================================================
//====================================================================


// The mirroring commands:

short cmd_mirrorpt(sds_point defpt, sds_point unorm, sds_point before,
    sds_point after) {
/*
**  Sets ResourceString(IDC_CMDS9_AFTER_12, "after" ) to the reflection of ResourceString(IDC_CMDS9_BEFORE_13, "before" ) through the
**  plane that has unit normal unorm and passes through defpt.
**
**  EXPECTS THE CALLER TO HAVE ENSURED THAT unorm IS A UNIT VECTOR.
**
**  ResourceString(IDC_CMDS9_BEFORE_13, "before" ) and ResourceString(IDC_CMDS9_AFTER_12, "after" ) can be the same variable.
**
**  Used by the mirroring commands.
**
**  Returns:
**       0 : OK
**      -1 : Calling error (something NULL)
*/
    sds_real ar1;


    if (defpt==NULL || unorm==NULL || before==NULL || after==NULL)
        return -1;

    /* Get twice the directed length of projection of vect from defpt */
    /* to "before" in norm's direction: */
    ar1=2.0*((before[0]-defpt[0])*unorm[0]+
             (before[1]-defpt[1])*unorm[1]+
             (before[2]-defpt[2])*unorm[2]);

    /* Subtract twice the projection from "before" to get "after": */
    after[0]=before[0]-ar1*unorm[0];
    after[1]=before[1]-ar1*unorm[1];
    after[2]=before[2]-ar1*unorm[2];

    return 0;
} 

// SystemMetrix(watanabe)-[mirror LEADER entity
//////////////////////////////////////////////////////////////////////
// convert from wcs to ucs
static
BOOL
wcs2ucs(
  sds_point ptSrc,
  sds_point ptDest
)
{
  struct resbuf wcs, ucs;
  wcs.restype = RTSHORT;
  ucs.restype = RTSHORT;
  wcs.resval.rint = 0;
  ucs.resval.rint = 1;

  sds_trans(ptSrc, &wcs, &ucs, 0, ptDest);

  return TRUE;
}

//////////////////////////////////////////////////////////////////////
// convert from ucs to wcs
static
BOOL
ucs2wcs(
  sds_point ptSrc,
  sds_point ptDest
)
{
  struct resbuf wcs, ucs;
  wcs.restype = RTSHORT;
  ucs.restype = RTSHORT;
  wcs.resval.rint = 0;
  ucs.resval.rint = 1;

  sds_trans(ptSrc, &ucs, &wcs, 0, ptDest);

  return TRUE;
}

//////////////////////////////////////////////////////////////////////
static
BOOL
MirrorLeader(
  struct resbuf*    elist,
  sds_point         defpt,
  sds_point         unorm
)
{
  struct resbuf* rb;
  for (rb = elist; rb != NULL; rb = rb->rbnext)
    {
      switch (rb->restype)
        {
		  //BugZilla No. 78419; 31-03-2003 [	
          case 211:
		  {
			  sds_point pt;
			  struct resbuf rbwcs, rbucs;
			  cmd_initialrbcs(NULL,&rbwcs,&rbucs,SDS_CURDWG);
			   
			  sds_trans(rb->resval.rpoint, &rbwcs,&rbucs, 1, pt);

			  sds_real len = 2.0 * (pt[0] * unorm[0] + pt[1] * unorm[1] + pt[2] * unorm[2]);
  
			  pt[0] = pt[0] - len * unorm[0];
			  pt[1] = pt[1] - len * unorm[1];
			  pt[2] = pt[2] - len * unorm[2];

			  sds_trans(pt, &rbucs, &rbwcs, 1, rb->resval.rpoint);
			  break;
		  } 
          case  10:
		  //BugZilla No. 78419; 31-03-2003 ]	
          case 212:
          case 213:
            {
              sds_point pt;
              wcs2ucs(rb->resval.rpoint, pt);
              cmd_mirrorpt(defpt, unorm, pt, pt);
              ucs2wcs(pt, rb->resval.rpoint);
              break;
            }
        }
    }

  return TRUE;
}

//////////////////////////////////////////////////////////////////////
// Get entity name of leader's annotation
static
BOOL
GetLeaderAnnotation(
  struct resbuf*    elist,
  sds_name&         anno
)
{
  struct resbuf* rb;
  for (rb = elist; rb != NULL; rb = rb->rbnext)
    {
      switch (rb->restype)
        {
          // annotation
          case 340:
            {
              // remove annotation
              sds_name_set(rb->resval.rlname, anno);
              return TRUE;
            }
        }
    }

  return FALSE;
}

//////////////////////////////////////////////////////////////////////
// class for replacing annotation of leader
class CAnnoMap
{
public:
  BOOL
  Lookup(
    const sds_name  key,
    sds_name&       value
  );

  void
  SetAt(
    const sds_name  key,
    const sds_name  value
  );

  int
  GetCount(
  );

  void
  GetNextAssoc(
    POSITION&   pos,
    sds_name&   key,
    sds_name&   value
  );

public:
  CMap<double, double, double, double&> m_map;

private:
  union u
    {
      double    d;
      sds_name  n;
    };
};

//////////////////////////////////////////////////////////////////////
// map count (size)
int
CAnnoMap::GetCount()
{
  return m_map.GetCount();
}

//////////////////////////////////////////////////////////////////////
// find key and value
BOOL
CAnnoMap::Lookup(
  const sds_name    key,
  sds_name&         value
)
{
  u uKey, uValue;
  sds_name_set(key, uKey.n);
  BOOL b = m_map.Lookup(uKey.d, uValue.d);
  if (b)
    {
      sds_name_set(uValue.n, value);
    }
  return b;
}

//////////////////////////////////////////////////////////////////////
// set value with key
void
CAnnoMap::SetAt(
  const sds_name    key,
  const sds_name    value
)
{
  u uKey, uValue;
  sds_name_set(key,   uKey.n);
  sds_name_set(value, uValue.n);
  m_map.SetAt(uKey.d, uValue.d);
}

//////////////////////////////////////////////////////////////////////
void
CAnnoMap::GetNextAssoc(
  POSITION&     pos,
  sds_name&     key,
  sds_name&     value
)
{
  u uKey, uValue;
  m_map.GetNextAssoc(pos, uKey.d, uValue.d);
  sds_name_set(uKey.n,   key);
  sds_name_set(uValue.n, value);
}

//////////////////////////////////////////////////////////////////////
// replace entity name for reactor
static
BOOL
ReplaceReactor(
  struct resbuf*    allapps,
  CAnnoMap&         mapLeader,
  CAnnoMap&         mapAnno,
  CAnnoMap&         mapRelease
)
{
  POSITION pos;
  struct resbuf* rb;
  sds_name oldname, newname;
  sds_name leader, anno;

  // replace annotation in leader
  pos = mapLeader.m_map.GetStartPosition();
  while (pos)
    {
      mapLeader.GetNextAssoc(pos, oldname, newname);
      rb = sds_entgetx(newname, allapps);
      if (rb)
        {
          if (ic_assoc(rb, 340) == 0)
            {
              if (mapAnno.Lookup(ic_rbassoc->resval.rlname, anno))
                {
                  sds_name_set(anno, ic_rbassoc->resval.rlname);
                  int irt = sds_entmod(rb);
                }
            }
          IC_RELRB(rb);
        }
    }

  // release annotation in leader
  sds_name_clear(leader);
  pos = mapRelease.m_map.GetStartPosition();
  while (pos)
    {
      mapRelease.GetNextAssoc(pos, oldname, newname);
      rb = sds_entgetx(newname, allapps);
      if (rb)
        {
          if (ic_assoc(rb, 340) == 0)
            {
              sds_name_set(leader, ic_rbassoc->resval.rlname);
              int irt = sds_entmod(rb);
            }
          IC_RELRB(rb);
        }
    }

  // replace reactor in annotation
  pos = mapAnno.m_map.GetStartPosition();
  while (pos)
    {
      mapAnno.GetNextAssoc(pos, oldname, newname);
      rb = sds_entgetx(newname, allapps);
      if (rb)
        {
          if (ic_assoc(rb, 330) == 0)
            {
              if (mapLeader.Lookup(ic_rbassoc->resval.rlname, leader))
                {
                  sds_name_set(leader, ic_rbassoc->resval.rlname);
                  int irt = sds_entmod(rb);
                }
            }
          IC_RELRB(rb);
        }
    }

  return TRUE;
}

//////////////////////////////////////////////////////////////////////
// release reactor from leader's annotation (mtext, tolerance, block)
static
BOOL
ReleaseSoftPointer(
  struct resbuf*    rb,
  CAnnoMap&         mapLeader
)
{
  sds_name name;
  BOOL b = mapLeader.Lookup(rb->resval.rlname, name);
  if (b == FALSE)
    {
      sds_name temp;
      sds_name_clear(temp);
      sds_name_set(temp, rb->resval.rlname);
    }

  return TRUE;
}
// ]-SystemMetrix(watanabe) mirror LEADER entity

short cmd_mirrorss(sds_name ss, sds_point defpt, sds_point norm, short delold) {
/*
**  Creates new entities that are the mirror images of the entities
**  in selection set ss.  The mirroring plane passes through
**  defpt and has normal direction norm.  
**
**  Has a keep-on-truckin' philosophy: doesn't bail if entget/entmake
**  errors occur.
**
**  Returns:
**       0 : OK
**      -1 : Calling error (something NULL)
**      -2 : norm is the zero vector
*/
    short complexent,ok2make,mirrtext,setcolor,setlt,rc,fi1;
    long ssidx;
	long ssLen;
	int savprog=0;
    sds_real ar[10];
    sds_name ename;
    sds_point unorm,oldextru,newextru,oldplextru,newplextru,ap[10];
    struct resbuf *elist,allapps,*rbp[10],*trbp1,rb,wcs,ucs,ecs;
    struct resbuf *typelink,*extrulink,savcolor,savlt;
	int enttype;

    // SystemMetrix(watanabe)-[mirror LEADER entity
    CAnnoMap mapLeader, mapAnno, mapRelease;
    // ]-SystemMetrix(watanabe) mirror LEADER entity

    //Bugzilla No 78016 18-03-2002 
	ReactorFixups reactorFixup;

	//Bugzilla No. 77993 ; date : 18-03-2002
	int SavedSDS_AtNodeDrag;
	SavedSDS_AtNodeDrag = SDS_AtNodeDrag;
	
    rc=setcolor=setlt=0; elist=NULL;
    allapps.rbnext=NULL; allapps.restype=RTSTR; 
	allapps.resval.rstring=(char *)(LPCTSTR)ResourceString(DNT_CMDS9___14, "*" );
    rb.rbnext=savcolor.rbnext=savlt.rbnext=NULL;
    oldplextru[0]=oldplextru[1]=0.0; oldplextru[2]=1.0;
    ic_ptcpy(newplextru,oldplextru);

    /*
    **  Save color and ltype, and set BYLAYER as the current value
    **  for both.  (When an entity has BYLAYER for one of them,
    **  that group doesn't show up in the entget; then the entmake
    **  uses the current value.)
    */
    rb.restype=RTSTR; rb.resval.rstring=ic_bylayer;
    savcolor.restype=RTNONE; savcolor.resval.rstring=NULL;
    if (SDS_getvar(NULL,DB_QCECOLOR,&savcolor,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)==RTNORM &&
        sds_setvar("CECOLOR"/*DNT*/,&rb)==RTNORM) setcolor=1;
    savlt.restype=RTNONE; savlt.resval.rstring=NULL;
    if (SDS_getvar(NULL,DB_QCELTYPE,&savlt,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)==RTNORM &&
        sds_setvar("CELTYPE"/*DNT*/,&rb)==RTNORM) setlt=1;
    rb.restype=RTNONE; rb.resval.rstring=NULL;

    /* Set up resbufs for ucs/ecs sds_trans() transformations: */
    wcs.rbnext=ucs.rbnext=ecs.rbnext=NULL;
    wcs.restype=RTSHORT; wcs.resval.rint=0;
    ucs.restype=RTSHORT; ucs.resval.rint=1;
    ecs.restype=RT3DPOINT;


    if (ss==NULL || defpt==NULL || norm==NULL) { rc=-1; goto out; }

    if (icadRealEqual((ar[0]=sqrt(norm[0]*norm[0]+norm[1]*norm[1]+norm[2]*norm[2])), 0.0))
        { rc=-2; goto out; }
    unorm[0]=norm[0]/ar[0]; unorm[1]=norm[1]/ar[0]; unorm[2]=norm[2]/ar[0];

    mirrtext=(SDS_getvar(NULL,DB_QMIRRTEXT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)==RTNORM) ? rb.resval.rint : 1;

	SDS_ProgressStart();
	savprog=0;
	sds_sslength(ss,&ssLen);


    // SystemMetrix(watanabe)-[mirror LEADER entity
    for (ssidx = 0L; sds_ssname(ss, ssidx, ename) == RTNORM; ssidx++)
      {
        elist = sds_entgetx(ename, &allapps);
        if (elist)
          {
            enttype = IC_TYPE_FROM_ENAME(ename);
            switch (enttype)
              {
                case DB_LEADER:
                  {
                    BOOL bCopy = FALSE;
                    sds_name anno;
                    if (GetLeaderAnnotation(elist, anno) && anno[0])
                      {
                        // find annotation in selection set
                        int irt = sds_ssmemb(anno, ss);
                        if (irt == RTNORM)
                          {
                            // add to annotation list
                            bCopy = TRUE;
                          }
                      }

                    if (bCopy)
                      {
                        // entity name to replase soft pointer
                        mapLeader.SetAt(ename, ename);
                        // entity name for annotation
                        mapAnno.SetAt(anno, anno);
                      }
                    else
                      {
                        // entity name to release soft pointer
                        mapRelease.SetAt(ename, ename);
                      }

                    break;
                  }
              }
            IC_RELRB(elist);
          }
      }
    // ]-SystemMetrix(watanabe) mirror LEADER entity

    //Bugzilla No 78016 18-03-2002 
	SDS_CURDWG->SetCopyingFlag(true);
    for (ssidx=0L; sds_ssname(ss,ssidx,ename)==RTNORM; ssidx++) {

		if (savprog+5<=((int)(((double)ssidx/ssLen)*100))) {
			SDS_ProgressPercent((int)(((double)ssidx/ssLen)*100));
		}
		if (sds_usrbrk()) { rc=(RTCAN); goto out; }

        /* Cancel any complex entity in progress. */
        sds_entmake(NULL); complexent=0;

        do {  /* At least once; loop for a complex entity. */

            ok2make=1;

            if (elist!=NULL) { sds_relrb(elist); elist=NULL; }
            if ((elist=sds_entgetx(ename,&allapps))==NULL) break;
			enttype=IC_TYPE_FROM_ENAME(ename);

            /* Get type and extrusion data: */
            typelink=extrulink=NULL;
            oldextru[0]=oldextru[1]=newextru[0]=newextru[1]=0.0;
            oldextru[2]=newextru[2]=1.0;
            for (trbp1=elist; trbp1!=NULL; trbp1=trbp1->rbnext) {
                if      (trbp1->restype==  0)  typelink=trbp1;
                else if (trbp1->restype==210) {
                    extrulink=trbp1;
                    ic_ptcpy(oldextru,extrulink->resval.rpoint);
                    /* Make sure it's unit length: */
                    ar[0]=sqrt(oldextru[0]*oldextru[0]+
                               oldextru[1]*oldextru[1]+
                               oldextru[2]*oldextru[2]);
                    if (!icadRealEqual(ar[0],0.0)) {
                        if (ar[0]!=1.0) {
                            oldextru[0]/=ar[0];
                            oldextru[1]/=ar[0];
                            oldextru[2]/=ar[0];
                        }
                    } else { oldextru[0]=oldextru[1]=0.0; oldextru[2]=1.0; }
                }
            }
            if (typelink==NULL || typelink->resval.rstring==NULL) break;

            if (extrulink!=NULL) {
                /* Mirror (0,0,0) and the oldextru to get newextru: */
                ap[0][0]=ap[0][1]=ap[0][2]=0.0;
                sds_trans(ap[0]   ,&wcs,&ucs,0,ap[2]);
                sds_trans(oldextru,&wcs,&ucs,0,ap[3]);
                cmd_mirrorpt(defpt,unorm,ap[2],ap[2]);
                cmd_mirrorpt(defpt,unorm,ap[3],ap[3]);
                sds_trans(ap[2],&ucs,&wcs,0,ap[0]);
                sds_trans(ap[3],&ucs,&wcs,0,ap[1]);
                newextru[0]=ap[1][0]-ap[0][0];
                newextru[1]=ap[1][1]-ap[0][1];
                newextru[2]=ap[1][2]-ap[0][2];
                /* Make sure it's unit length: */
                ar[0]=sqrt(newextru[0]*newextru[0]+
                           newextru[1]*newextru[1]+
                           newextru[2]*newextru[2]);
                if (!icadRealEqual(ar[0],0.0)) {
                    if (!icadRealEqual(ar[0],1.0)) {
                        newextru[0]/=ar[0];
                        newextru[1]/=ar[0];
                        newextru[2]/=ar[0];
                    }
                } else { newextru[0]=newextru[1]=0.0; newextru[2]=1.0; }

                /* No reason we can't set the new extrusion right now: */
                ic_ptcpy(extrulink->resval.rpoint,newextru);
            }

            /* Save the old and new extrusions for the VERTEXs */
            /* (only 2D VERTEXs will use them): */
            if (enttype==DB_POLYLINE || enttype==DB_LWPOLYLINE) {
                ic_ptcpy(oldplextru,oldextru);
                ic_ptcpy(newplextru,newextru);
            }


            /* Do each case: */

            memset(rbp,0,sizeof(rbp));

            if (enttype==DB_LINE) {

                for (trbp1=elist; trbp1!=NULL; trbp1=trbp1->rbnext) {
                    switch (trbp1->restype) {
                        case  10: rbp[0]=trbp1; break;  /* 1st pt */
                        case  11: rbp[1]=trbp1; break;  /* 2nd pt */
                    }
                }
                for (fi1=0; fi1<2 && rbp[fi1]!=NULL; fi1++);
                if (fi1<2) break;  /* Expected group not found. */

                /* Both pts are in the WCS: */
                sds_trans(rbp[0]->resval.rpoint,&wcs,&ucs,0,ap[0]);
                cmd_mirrorpt(defpt,unorm,ap[0],ap[0]);
                sds_trans(ap[0],&ucs,&wcs,0,rbp[0]->resval.rpoint);
                sds_trans(rbp[1]->resval.rpoint,&wcs,&ucs,0,ap[0]);
                cmd_mirrorpt(defpt,unorm,ap[0],ap[0]);
                sds_trans(ap[0],&ucs,&wcs,0,rbp[1]->resval.rpoint);

            } else if (enttype==DB_POINT) {

                for (trbp1=elist; trbp1!=NULL; trbp1=trbp1->rbnext) {
                    switch (trbp1->restype) {
                        case  10: rbp[0]=trbp1; break;  /* Point */
                        case  50: rbp[1]=trbp1; break;  /* Rotation */
                    }
                }
                for (fi1=0; fi1<2 && rbp[fi1]!=NULL; fi1++);
                if (fi1<2) break;  /* Expected group not found. */

                /* Point is in the WCS: */
                sds_trans(rbp[0]->resval.rpoint,&wcs,&ucs,0,ap[0]);
                cmd_mirrorpt(defpt,unorm,ap[0],ap[0]);
                sds_trans(ap[0],&ucs,&wcs,0,rbp[0]->resval.rpoint);

                /*
                **  Rotation.
                **
                **  Interesting.  ACAD does not adjust the 50-group value,
                **  so, the mirrored copy doesn't seem to be oriented correctly.
                **  Okay.  We won't, either.  (But, if we ever want to,
                **  rbp[1] points to the 50-link.  (Maybe it's because it's
                **  not really a rotation.  It's "angle of X-axis for the
                **  UCS in effect when the POINT was drawn".)
                */

            } else if (enttype==DB_CIRCLE) {

                for (trbp1=elist; trbp1!=NULL &&
                    trbp1->restype!=10; trbp1=trbp1->rbnext);
                if (trbp1==NULL) break;  /* Expected group not found. */

                /* newcent */
                ic_ptcpy(ecs.resval.rpoint,oldextru);
                sds_trans(trbp1->resval.rpoint,&ecs,&ucs,0,ap[0]);
                cmd_mirrorpt(defpt,unorm,ap[0],ap[0]);
                ic_ptcpy(ecs.resval.rpoint,newextru);
                sds_trans(ap[0],&ucs,&ecs,0,trbp1->resval.rpoint);

            } else if (enttype==DB_ELLIPSE) {
                for (trbp1=elist; trbp1!=NULL; trbp1=trbp1->rbnext) {
                    switch (trbp1->restype) {
                        case 10: rbp[0]=trbp1; break;  /* Center  */
                        case 11: rbp[1]=trbp1; break;  /* Axis Vector */
                        case 41: rbp[2]=trbp1; break;  /* Start Param */
                        case 42: rbp[3]=trbp1; break;  /* End Param */
                    }
                }
                for (fi1=0; fi1<4 && rbp[fi1]!=NULL; fi1++);
                if (fi1<4) break;  /* Expected group not found. */
 
                /* new center */
                sds_trans(rbp[0]->resval.rpoint,&wcs,&ucs,0,ap[0]);
                cmd_mirrorpt(defpt,unorm,ap[0],ap[0]);
                sds_trans(ap[0],&ucs,&wcs,0,ap[1]);//ap[1] is new center
				ap[0][0]=rbp[0]->resval.rpoint[0]+rbp[1]->resval.rpoint[0];
				ap[0][1]=rbp[0]->resval.rpoint[1]+rbp[1]->resval.rpoint[1];
				ap[0][2]=rbp[0]->resval.rpoint[2]+rbp[1]->resval.rpoint[2];
				sds_trans(ap[0],&wcs,&ucs,0,ap[2]);
                cmd_mirrorpt(defpt,unorm,ap[2],ap[2]);
				sds_trans(ap[2],&ucs,&wcs,0,ap[0]);	//ap[0] is new start pt
				//set new center
				ic_ptcpy(rbp[0]->resval.rpoint,ap[1]);
				//set new axis vector
				rbp[1]->resval.rpoint[0]=ap[0][0]-ap[1][0];
				rbp[1]->resval.rpoint[1]=ap[0][1]-ap[1][1];
				rbp[1]->resval.rpoint[2]=ap[0][2]-ap[1][2];
				if (!icadAngleEqual(rbp[2]->resval.rreal,0.0) || 
					!icadAngleEqual(rbp[3]->resval.rreal,IC_TWOPI)){
					//adjust start & end params
					ar[0]=IC_TWOPI-rbp[2]->resval.rreal;
					ar[1]=IC_TWOPI-rbp[3]->resval.rreal;
					rbp[2]->resval.rreal=ar[1];
					rbp[3]->resval.rreal=ar[0];
				}

            } else if (enttype==DB_SHAPE) {
                sds_point newinspt;

                for (trbp1=elist; trbp1!=NULL; trbp1=trbp1->rbnext) {
                    switch (trbp1->restype) {
                        case 10: rbp[0]=trbp1; break;  /* Ins pt */
                        case 41: rbp[1]=trbp1; break;  /* X scale */
                        case 50: rbp[2]=trbp1; break;  /* Rotation */
                    }
                }
                for (fi1=0; fi1<3 && rbp[fi1]!=NULL; fi1++);
                if (fi1<3) break;  /* Expected group not found. */

                /* Rotation (do it 1st before we change the other params) */
                /*
                **  Determine new rotation by mirroring the ins pt
                **  and a pt 1 unit away in the rotation direction:
                */
                ap[0][0]=rbp[0]->resval.rpoint[0]+
                    cos(rbp[2]->resval.rreal);
                ap[0][1]=rbp[0]->resval.rpoint[1]+
                    sin(rbp[2]->resval.rreal);
                ap[0][2]=rbp[0]->resval.rpoint[2];
                ic_ptcpy(ecs.resval.rpoint,oldextru);
                sds_trans(rbp[0]->resval.rpoint,&ecs,&ucs,0,ap[1]);
                sds_trans(ap[0],&ecs,&ucs,0,ap[2]);
                cmd_mirrorpt(defpt,unorm,ap[1],ap[1]);
                cmd_mirrorpt(defpt,unorm,ap[2],ap[2]);
                ic_ptcpy(ecs.resval.rpoint,newextru);
                sds_trans(ap[1],&ucs,&ecs,0,newinspt);
                sds_trans(ap[2],&ucs,&ecs,0,ap[0]);
                /* Get angle backward (rot pt to ins pt) to correct for */
                /* negating the X scale later: */
                rbp[2]->resval.rreal=sds_angle(ap[0],newinspt);

                /* Ins pt */
                ic_ptcpy(rbp[0]->resval.rpoint,newinspt);

                /* Negate the X scale: */
                rbp[1]->resval.rreal=-rbp[1]->resval.rreal;

            } else if (enttype==DB_TEXT || enttype==DB_ATTDEF || enttype==DB_ATTRIB) {

                short att,defjust,flip;
                sds_real newrot,tbw,ar1;
                sds_point new10,new11,tb[2],right,up;

                att=(toupper(*typelink->resval.rstring)=='A');  /* ATTDEF or ATTRIB */

                for (trbp1=elist; trbp1!=NULL; trbp1=trbp1->rbnext) {
                    switch (trbp1->restype) {
                        case 10: rbp[0]=trbp1; break;  /* Ins pt */
                        case 11: rbp[1]=trbp1; break;  /* Align pt */
                        case 50: rbp[2]=trbp1; break;  /* Rotation */
                        case 71: rbp[3]=trbp1; break;  /* Generation */
                        case 72: rbp[4]=trbp1; break;  /* Halign */
                        case 73: case 74:              /* Valign */
                            if (trbp1->restype==73+att) rbp[5]=trbp1;
                            break;
                    }
                }
                for (fi1=0; fi1<6 && rbp[fi1]!=NULL; fi1++);
                if (fi1<6) break;  /* Expected group not found. */

                if (sds_textbox(elist,tb[0],tb[1])!=RTNORM) break;
                tbw=tb[1][0]-tb[0][0];

                defjust=(!rbp[4]->resval.rint && !rbp[5]->resval.rint);

                ic_normang(&rbp[2]->resval.rreal,NULL);  /* Make sure */

                /*
                **  Determine new rotation by mirroring the ins pt
                **  and a pt 1 unit away in the rotation direction:
                */
                ap[0][0]=rbp[0]->resval.rpoint[0]+
                    cos(rbp[2]->resval.rreal);
                ap[0][1]=rbp[0]->resval.rpoint[1]+
                    sin(rbp[2]->resval.rreal);
                ap[0][2]=rbp[0]->resval.rpoint[2];
                ic_ptcpy(ecs.resval.rpoint,oldextru);
                sds_trans(rbp[0]->resval.rpoint,&ecs,&ucs,0,ap[1]);
                sds_trans(ap[0],&ecs,&ucs,0,ap[2]);
                cmd_mirrorpt(defpt,unorm,ap[1],ap[1]);
                cmd_mirrorpt(defpt,unorm,ap[2],ap[2]);
                ic_ptcpy(ecs.resval.rpoint,newextru);
                sds_trans(ap[1],&ucs,&ecs,0,new10);
                sds_trans(ap[2],&ucs,&ecs,0,ap[0]);
                newrot=sds_angle(new10,ap[0]);
                ic_normang(&newrot,NULL);  /* Precaution */

                /* Align pt */
                if (defjust) {  /* For lower left, 11-pt must be (0,0,elev). */
                    new11[0]=new11[1]=0.0;
                    new11[2]=new10[2];
                } else {
                    ic_ptcpy(ecs.resval.rpoint,oldextru);
                    sds_trans(rbp[1]->resval.rpoint,&ecs,&ucs,0,ap[0]);
                    cmd_mirrorpt(defpt,unorm,ap[0],ap[0]);
                    ic_ptcpy(ecs.resval.rpoint,newextru);
                    sds_trans(ap[0],&ucs,&ecs,0,new11);
                }

                if (mirrtext) {

                    /* Toggle the "backward" flag: */
                    rbp[3]->resval.rint^=2;

                    /* Flip the rotation to correct for toggling the */
                    /* "backward" flag above: */
                    newrot+=IC_PI; ic_normang(&newrot,NULL);

                } else {

                    /*
                    **  Not sure we'll ever perfect this one, but here
                    **  are the empirical rules from testing ACAD.
                    **  The text is not reversed (mirrored in X) and always
                    **  keeps the same sense (right side up or upside
                    **  down).  I haven't been able to discover how the
                    **  border cases (90 deg and 270 deg) are judged,
                    **  so my calls will be different than ACAD's.
                    **
                    **  When mirroring does NOT require flipping the
                    **  rotation to preserve its sense,
                    **  the 10 and 11 points are mirrored and then
                    **  adjusted vertically (in the text system) such that
                    **  the new pt is just as far from the top of the
                    **  text box as the original was from the bottom.
                    **
                    **  When mirroring DOES require flipping the rotation,
                    **  the 10 and 11 points are mirrored.  Only the
                    **  right and left justifications are corrected;
                    **  they are pushed one text box width in the opposite
                    **  direction.
                    */

                    flip=(cos(rbp[2]->resval.rreal)*cos(newrot)<0.0);

                    if (flip) { newrot+=IC_PI; ic_normang(&newrot,NULL); }

                    /* Make the "right" and "up" unit vectors for newrot: */
                    right[0]=cos(newrot); right[1]=sin(newrot);
                    up[0]=-right[1]; up[1]=right[0];
                    right[2]=up[2]=0.0;
                    /*
                    **  The following change was made purely as an
                    **  empirical fix.  It seems that when the "mirrored
                    **  in X" flag is set, the "right" vector is reversed
                    **  (but "up" remains unaffected).
                    **  There's no similar change in "up" when the
                    **  "mirrored in Y" flag is set:
                    */
                    if (rbp[3]->resval.rint&2)
                        { right[0]=-right[0]; right[1]=-right[1]; }

                    if (flip) {  /* Correct points horizontally. */

                        /* Move the 10-pt left by one text box width: */
                        new10[0]-=tbw*right[0];
                        new10[1]-=tbw*right[1];

                        /* Move the 11-pt, if necessary: */
                        if (!defjust) {

                            /* If left, it mirrored right; move it left: */
                            if (rbp[4]->resval.rint==0) {
                                new11[0]-=tbw*right[0]; new11[1]-=tbw*right[1];
                            } else if (rbp[4]->resval.rint==2 ||
                                       rbp[4]->resval.rint==3 ||
                                       rbp[4]->resval.rint==5) {
                                /* If right, aligned, or fit, it mirrored */
                                /* left; move it right: */

                                new11[0]+=tbw*right[0]; new11[1]+=tbw*right[1];
                            }
                        }

                    } else {  /* Not flipping; correct points vertically. */

                        /*
                        ** The original 10-pt is -tb[0][1] units above the
                        ** text box bottom.  The new one will have mirrored
                        ** to that distance below the top.  Get the distance
                        ** to move it down, and move it.
                        */
                        ar1=tb[1][1]+tb[0][1];
                        new10[0]-=ar1*up[0]; new10[1]-=ar1*up[1];

                        /* Move the 11-pt, if necessary: */
                        if (!defjust) {

                            /* Xform original 11-pt to text box system: */
                            ap[0][0]=rbp[1]->resval.rpoint[0]-
                                     rbp[0]->resval.rpoint[0];
                            ap[0][1]=rbp[1]->resval.rpoint[1]-
                                     rbp[0]->resval.rpoint[1];
                            ap[1][0]=ap[0][0]*cos(rbp[2]->resval.rreal)+
                                     ap[0][1]*sin(rbp[2]->resval.rreal);
                            ap[1][1]=ap[0][1]*cos(rbp[2]->resval.rreal)-
                                     ap[0][0]*sin(rbp[2]->resval.rreal);

                            /*
                            **  Get distance to move up.
                            **  Tricky to see.  If text box top is at T,
                            **  text box bottom is at B, and the orig
                            **  pt is at Y (in the text box system),
                            **  then it must have mirrored to
                            **  T-(Y-B).  To move back to where it belongs,
                            **  it needs to go up by Y-(T-(Y-B)), or
                            **  2Y-T-B:
                            */
                            ar1=2.0*ap[1][1]-tb[0][1]-tb[1][1];

                            new11[0]+=ar1*up[0]; new11[1]+=ar1*up[1];
                        }
                    }
                }

                /* Ins pt */
                ic_ptcpy(rbp[0]->resval.rpoint,new10);

                /* Align pt */
                ic_ptcpy(rbp[1]->resval.rpoint,new11);

                rbp[2]->resval.rreal=newrot;

            } else if (enttype==DB_MTEXT){

                sds_point new10,new11;

                for (trbp1=elist; trbp1!=NULL; trbp1=trbp1->rbnext) {
                    switch (trbp1->restype) {
                        case 10: rbp[0]=trbp1; break;  /* Ins pt */
                        case 11: rbp[1]=trbp1; break;  /* Dir Vect */
                        case 71: rbp[3]=trbp1; break;  /* Justify */
                        case 72: rbp[4]=trbp1; break;  /* Direction */
						case 41: rbp[6]=trbp1; break;  /* Rectang width */
                        case 50: rbp[7]=trbp1; break;  /* Rotation (was [2] on text ent)*/
						case 210:rbp[8]=trbp1; break;  /* extru */

                        // SystemMetrix(watanabe)-[mirror LEADER entity
                        case 330:
                            ReleaseSoftPointer(trbp1, mapLeader);
                            break;
                        // ]-SystemMetrix(watanabe) mirror LEADER entity

                    }
                }
                for (fi1=0; fi1<7; fi1++){
					if (fi1==2|| fi1==5) continue;
					if (rbp[fi1]==NULL)break;
				}
                if (fi1<6) break;  /* Expected group not found. */


                /*
                **  Determine new rotation by mirroring the ins pt
                **  and a pt 1 unit away in the rotation direction:
                */
				ap[0][0]=rbp[0]->resval.rpoint[0]+rbp[1]->resval.rpoint[0];
				ap[0][1]=rbp[0]->resval.rpoint[1]+rbp[1]->resval.rpoint[1];
				ap[0][2]=rbp[0]->resval.rpoint[2]+rbp[1]->resval.rpoint[2];
                sds_trans(rbp[0]->resval.rpoint,&wcs,&ucs,0,ap[1]);
                sds_trans(ap[0],&wcs,&ucs,0,ap[2]);
                cmd_mirrorpt(defpt,unorm,ap[1],ap[1]);
                cmd_mirrorpt(defpt,unorm,ap[2],ap[2]);
                sds_trans(ap[1],&ucs,&wcs,0,new10);
                sds_trans(ap[2],&ucs,&wcs,0,ap[0]);
				new11[0]=ap[0][0]-new10[0];
				new11[1]=ap[0][1]-new10[1];
				new11[2]=ap[0][2]-new10[2];


                if (!mirrtext) {
					if (fabs(unorm[0])>fabs(unorm[1])){
						//switch right justify to left
						if (0==(rbp[3]->resval.rint%3))rbp[3]->resval.rint-=2;
						//switch left justify to right
						else if (1==(rbp[3]->resval.rint%3))rbp[3]->resval.rint+=2;
						//flip the x direction vector
						new11[0]=-new11[0];
						new11[1]=-new11[1];
						new11[2]=-new11[2];
					}else{
						//switch top justify to bottom
						if (rbp[3]->resval.rint<4)rbp[3]->resval.rint+=6;
						else if (rbp[3]->resval.rint>6)rbp[3]->resval.rint-=6;
					}
                }else{
					if (rbp[8]==NULL)break;
					rbp[8]->resval.rpoint[0]=-rbp[8]->resval.rpoint[0];
					rbp[8]->resval.rpoint[1]=-rbp[8]->resval.rpoint[1];
					rbp[8]->resval.rpoint[2]=-rbp[8]->resval.rpoint[2];
				}
				if (NULL!=rbp[7]){
					//omit 50 group, it's duplicate of 11 and too hard to calc
					for (trbp1=elist; trbp1!=NULL; trbp1=trbp1->rbnext) {
						if (trbp1->rbnext==rbp[7]){
							trbp1->rbnext=rbp[7]->rbnext;
							rbp[7]->rbnext=NULL;
							sds_relrb(rbp[7]);
							rbp[7]=trbp1=NULL;
							break;
						}
					}
				}

                /* Ins pt */
                ic_ptcpy(rbp[0]->resval.rpoint,new10);

                /* Align pt */
                ic_ptcpy(rbp[1]->resval.rpoint,new11);

            } else if (enttype==DB_ARC) {
                sds_real iang,newsa,newea;
                sds_point newcent;

                for (trbp1=elist; trbp1!=NULL; trbp1=trbp1->rbnext) {
                    switch (trbp1->restype) {
                        case 10: rbp[0]=trbp1; break;  /* Center */
                        case 40: rbp[1]=trbp1; break;  /* Radius */
                        case 50: rbp[2]=trbp1; break;  /* Start angle */
                        case 51: rbp[3]=trbp1; break;  /* End angle */
                    }
                }
                for (fi1=0; fi1<4 && rbp[fi1]!=NULL; fi1++);
                if (fi1<4) break;  /* Expected group not found. */

                /* newcent */
                ic_ptcpy(ecs.resval.rpoint,oldextru);
                sds_trans(rbp[0]->resval.rpoint,&ecs,&ucs,0,ap[0]);
                cmd_mirrorpt(defpt,unorm,ap[0],ap[0]);
                ic_ptcpy(ecs.resval.rpoint,newextru);
                sds_trans(ap[0],&ucs,&ecs,0,newcent);

                /* iang (stays constant) */
                ar[0]=rbp[2]->resval.rreal; ar[1]=rbp[3]->resval.rreal;
                ic_normang(ar,ar+1); iang=ar[1]-ar[0];

                /* newsa (remember: start/end reverse upon mirroring, so use) */
                /* the END point) */
                ap[0][0]=rbp[0]->resval.rpoint[0]+
                    rbp[1]->resval.rreal*cos(rbp[3]->resval.rreal);
                ap[0][1]=rbp[0]->resval.rpoint[1]+
                    rbp[1]->resval.rreal*sin(rbp[3]->resval.rreal);
                ap[0][2]=rbp[0]->resval.rpoint[2];
                ic_ptcpy(ecs.resval.rpoint,oldextru);
                sds_trans(ap[0],&ecs,&ucs,0,ap[1]);
                cmd_mirrorpt(defpt,unorm,ap[1],ap[1]);
                ic_ptcpy(ecs.resval.rpoint,newextru);
                sds_trans(ap[1],&ucs,&ecs,0,ap[0]);
                newsa=sds_angle(newcent,ap[0]);

                /* newea */
                newea=newsa+iang; ic_normang(&newea,NULL);

                /* Set the new vals: */
                ic_ptcpy(rbp[0]->resval.rpoint,newcent);
                rbp[2]->resval.rreal=newsa;
                rbp[3]->resval.rreal=newea;

            } else if (enttype==DB_TRACE || enttype==DB_SOLID || enttype==DB_3DFACE) {

                short face;

                face=(*typelink->resval.rstring=='3');

                for (trbp1=elist; trbp1!=NULL; trbp1=trbp1->rbnext) {
                    switch (trbp1->restype) {
                        case 10: rbp[0]=trbp1; break;
                        case 11: rbp[1]=trbp1; break;
                        case 12: rbp[2]=trbp1; break;
                        case 13: rbp[3]=trbp1; break;
                    }
                }
                for (fi1=0; fi1<4 && rbp[fi1]!=NULL; fi1++);
                if (fi1<4) break;  /* Expected group not found. */

                for (fi1=0; fi1<4; fi1++) {
                    if (face) {
                        sds_trans(rbp[fi1]->resval.rpoint,&wcs,&ucs,0,ap[0]);
                    } else {
                        ic_ptcpy(ecs.resval.rpoint,oldextru);
                        sds_trans(rbp[fi1]->resval.rpoint,&ecs,&ucs,0,ap[0]);
                    }
                    cmd_mirrorpt(defpt,unorm,ap[0],ap[0]);
                    if (face) {
                        sds_trans(ap[0],&ucs,&wcs,0,rbp[fi1]->resval.rpoint);
                    } else {
                        ic_ptcpy(ecs.resval.rpoint,newextru);
                        sds_trans(ap[0],&ucs,&ecs,0,rbp[fi1]->resval.rpoint);
                    }
                }

         //
         //      DIMENSION NOT EVEN CLOSE; STUDY THE PROBLEM IN DETAIL LATER.
         //      (ACAD CREATES A NEW ANONYMOUS BLOCK WHEN MIRRORING.)
		//Add by SMR for supporting DB_HATCH mirror. 2001/5/31
          } else if (enttype==DB_HATCH) {
			bool first = true;
			resbuf *insertpt, *patternbase = NULL, *patternoffset = NULL;
			resbuf *patternlineangle = NULL;

            for (trbp1=elist; trbp1!=NULL; trbp1=trbp1->rbnext) {
                switch (trbp1->restype) {
	               case  10: // Pt (OCS):
						if (first) {
							insertpt = trbp1;
							first = false;
						}

						ic_ptcpy(ecs.resval.rpoint,oldextru);
						sds_trans(trbp1->resval.rpoint,&ecs,&ucs,0,ap[0]);
						//sds_xformpt(ap[0],ap[0],wmat);
						cmd_mirrorpt(defpt,unorm,ap[0],ap[0]);
						ic_ptcpy(ecs.resval.rpoint,newextru);
						sds_trans(ap[0],&ucs,&ecs,0,trbp1->resval.rpoint);
						break;

					case 91: //Num Boundary paths
						{
							long numPaths = trbp1->resval.rlong;
							for (long i = 0; trbp1 && (i < numPaths); i++)
							{
								trbp1 = trbp1->rbnext;

								// first item must be a flag so we can tell if it's a polyline
								if ((trbp1 == NULL) || (trbp1->restype != 92)) {
									ASSERT(false);
									break;
								}

								int hatchflag = trbp1->resval.rint;

								if (hatchflag & IC_HATCH_PATHFLAG_POLYLINE) {		//polyline edge

									// 3 items in polyline that are not multiple entries
									// multi item entries are set under 93
									for (int j = 0; j < 3; j++)
									{
										trbp1 = trbp1->rbnext;

										if (trbp1 == NULL)
											break;

										if (trbp1->restype == 93)
										{
											//check each vertex and bulge
											long numvertices = trbp1->resval.rlong;
											for (long k = 0; k < numvertices && trbp1; k++)
											{
												trbp1 = trbp1->rbnext;
												// next value must be vertex loc - 10
												if ((trbp1 == NULL) || (trbp1->restype != 10))
													break;

												ic_ptcpy(ecs.resval.rpoint,oldextru);
												sds_trans(trbp1->resval.rpoint,&ecs,&ucs,0,ap[0]);
												//sds_xformpt(ap[0],ap[0],wmat);
												cmd_mirrorpt(defpt,unorm,ap[0],ap[0]);
												ic_ptcpy(ecs.resval.rpoint,newextru);
												sds_trans(ap[0],&ucs,&ecs,0,trbp1->resval.rpoint);

												// if bulge is specified it should be next value otherwise set to 0;
												if (trbp1->rbnext != NULL && trbp1->rbnext->restype == 42)
												{
													//bulge doesn't appear to be needed so just skip it
													trbp1 = trbp1->rbnext;
												}
											}
										}

									}// for polyline items

								} // if polyline

								else {				//non polyline edge
									trbp1 = trbp1->rbnext;
									// next item must be number of edges in path
									if ((trbp1 == NULL) || (trbp1->restype != 93)) {
										ASSERT(false);
										break;
									}

									long numedgetypes = trbp1->resval.rlong;
									for (long j = 0; trbp1 && (j < numedgetypes); j++)
									{
										trbp1 = trbp1->rbnext;
										// next item must be edgetype
										if ((trbp1 == NULL) || (trbp1->restype != 72)) {
											ASSERT(false);
											break;
										}

										switch (trbp1->resval.rint)
										{
										case IC_HATCH_PATHTYPE_LINE: //line
											{
												// 2 values in line
												for (int k = 0; trbp1 && (k < 2); k++) {
													trbp1 = trbp1->rbnext;
													if (trbp1 == NULL)
														break;

													switch (trbp1->restype) {
													case 10:
														ic_ptcpy(ecs.resval.rpoint,oldextru);
														sds_trans(trbp1->resval.rpoint,&ecs,&ucs,0,ap[0]);
														//sds_xformpt(ap[0],ap[0],wmat);
														cmd_mirrorpt(defpt,unorm,ap[0],ap[0]);
														ic_ptcpy(ecs.resval.rpoint,newextru);
														sds_trans(ap[0],&ucs,&ecs,0,trbp1->resval.rpoint);
														break;

													case 11:
														ic_ptcpy(ecs.resval.rpoint,oldextru);
														sds_trans(trbp1->resval.rpoint,&ecs,&ucs,0,ap[0]);
														//sds_xformpt(ap[0],ap[0],wmat);
														cmd_mirrorpt(defpt,unorm,ap[0],ap[0]);
														ic_ptcpy(ecs.resval.rpoint,newextru);
														sds_trans(ap[0],&ucs,&ecs,0,trbp1->resval.rpoint);
														break;

													default:
														ASSERT(false);
													}
												}
											}
											break;

											//Temp block. SMR 2001/5/31
										case IC_HATCH_PATHTYPE_CIRCARC: //circular arc
											{
												// 5 values in circular arc
												for (int k = 0; trbp1 && (k < 5); k++)
												{
													trbp1 = trbp1->rbnext;
													if (trbp1 == NULL)
														break;

													switch (trbp1->restype) {
													case 10:
														rbp[0]=trbp1;  // Center
														break;

													case 40:
														rbp[1]=trbp1;  // Radius
														break;

													case 50:
														rbp[2]=trbp1;  // Start angle
														break;

													case 51:
														rbp[3]=trbp1;  // End angle
														break;

													case 73:
														break; // do nothing

													default:
														ASSERT(false);
													}
												}

												// Create mirror transformation
												sds_matrix mtx;
												bool ok;
												ic_mirrorPlane(defpt, norm, ok, mtx);
												if (!ok)
													break;
												SDS_xformarc(oldextru, newextru, mtx, &ecs, &ucs, rbp[0], rbp[1], rbp[2],rbp[3]);
											}
											break;//End of SMR. Temp 2001/5/31

										case IC_HATCH_PATHTYPE_ELLIPARC: // elliptical arc
											{
												// 6 values in ellipitical arc
												for (int k = 0; trbp1 && (k < 6); k++)
												{
													trbp1 = trbp1->rbnext;
													if (trbp1 == NULL)
														break;

													switch (trbp1->restype) {
													case 10:			//center
														rbp[0]=trbp1;
														break;

													case 11:			//vector
														rbp[1]=trbp1;
														break;

													case 40:			//ratio
														rbp[2]=trbp1;
														break;

													case 50:
														rbp[3]=trbp1;	//start angle
														break;

													case 51:
														rbp[4]=trbp1;	//end angle
														break;

													case 73:			//ccw flag - do nothing
														break;

													default:
														ASSERT(false);
													}
												}

												// new center - ap[7]
												ic_ptcpy(ecs.resval.rpoint,oldextru);
												sds_trans(rbp[0]->resval.rpoint,&wcs,&ucs,0,ap[0]);
												//sds_xformpt(ap[0],ap[0],wmat);
												cmd_mirrorpt(defpt,unorm,ap[0],ap[0]);
												ic_ptcpy(ecs.resval.rpoint,newextru);
												sds_trans(ap[0],&ucs,&wcs,0,ap[7]);

												//new axis vector				;
												ap[1][0]=rbp[0]->resval.rpoint[0]+rbp[1]->resval.rpoint[0];
												ap[1][1]=rbp[0]->resval.rpoint[1]+rbp[1]->resval.rpoint[1];
												ap[1][2]=rbp[0]->resval.rpoint[2]+rbp[1]->resval.rpoint[2];
												ic_ptcpy(ecs.resval.rpoint,oldextru);
												sds_trans(ap[1],&wcs,&ucs,0,ap[2]);
												//sds_xformpt(ap[2],ap[2],wmat);
												cmd_mirrorpt(defpt,unorm,ap[2],ap[2]);
												ic_ptcpy(ecs.resval.rpoint,newextru);
												sds_trans(ap[2],&ucs,&wcs,0,ap[8]);
												rbp[1]->resval.rpoint[0]=ap[8][0]-ap[7][0];
												rbp[1]->resval.rpoint[1]=ap[8][1]-ap[7][1];
												rbp[1]->resval.rpoint[2]=ap[8][2]-ap[7][2];
												ic_ptcpy(rbp[0]->resval.rpoint,ap[7]);
											}
											break;

										case IC_HATCH_PATHTYPE_SPLINE: // spline
											{
												long numctrlpts = 0;
												long numknots = 0;
												int rational = 0;

												// 5 fixed values in spline arc
												for (int k = 0; trbp1 && (k < 5); k++)
												{
													trbp1 = trbp1->rbnext;
													if (trbp1 == NULL)
														break;

													switch (trbp1->restype) {
														case 73:
															rational = trbp1->resval.rint;
															break;

														case 95:
															numknots = trbp1->resval.rlong;
															break;

														case 96:
															numctrlpts = trbp1->resval.rlong;
															break;

														// don't care about other values.
													}
												}

												//bypass knot values
												for (k = 0; trbp1 && (k < numknots); k++)
												{
													trbp1 = trbp1->rbnext;
													if (trbp1 == NULL || trbp1->restype != 40)  {
														ASSERT(false);
														break;
													}
												}

												//set control points
												for (k = 0; trbp1 && (k < numctrlpts); k++) {
													trbp1 = trbp1->rbnext;
													if (trbp1 == NULL || trbp1->restype != 10) {
														ASSERT(false);
														break;
													}

													ic_ptcpy(ecs.resval.rpoint,oldextru);
													sds_trans(trbp1->resval.rpoint,&wcs,&ucs,0,ap[0]);
													//sds_xformpt(ap[0],ap[0],wmat);
													cmd_mirrorpt(defpt,unorm,ap[0],ap[0]);
													ic_ptcpy(ecs.resval.rpoint,newextru);
													sds_trans(ap[0],&ucs,&wcs,0,trbp1->resval.rpoint);
												}

												//bypass weight values only if rational
												if (rational)
												{
													for (k = 0; trbp1 && k < numctrlpts; k++) {
														trbp1 = trbp1->rbnext;
														if (trbp1 == NULL || trbp1->restype != 42) {
															ASSERT(false);
															break;
														}
													}
												}
											}
											break;

										default: // undefined
											ASSERT(false);
										}

									}// for each edge
								}// else not polyline

								// bypass boundary objects
								trbp1 = trbp1->rbnext;
								if (trbp1 == NULL || trbp1->restype != 97)  {
									ASSERT(false);
									break;
								}

								int numobjs = trbp1->resval.rlong;

								for (int j = 0; j < numobjs; j++)
								{
									trbp1 = trbp1->rbnext;
									if (trbp1 == NULL || trbp1->restype != 330) {
										ASSERT(false);
										break;
									}
								}
							} //for boundary path
						} //case 91
						break;

					case 52: {
							double patternAng = trbp1->resval.rreal;

							// Rotation (do it 1st before we change the other params)
							//
							//  Determine new rotation by xforming the ins pt
							//  and a pt 1 unit away in the rotation direction:
							//
							ap[0][0] = insertpt->resval.rpoint[0] + cos(patternAng);
							ap[0][1] = insertpt->resval.rpoint[1] + sin(patternAng);
							ap[0][2] = insertpt->resval.rpoint[2];
							ic_ptcpy(ecs.resval.rpoint,oldextru);
							sds_trans(insertpt->resval.rpoint,&ecs,&ucs,0,ap[1]);
							sds_trans(ap[0],&ecs,&ucs,0,ap[2]);
							//sds_xformpt(ap[1],ap[1],wmat);
							cmd_mirrorpt(defpt,unorm,ap[1],ap[1]);
							//sds_xformpt(ap[2],ap[2],wmat);
							cmd_mirrorpt(defpt,unorm,ap[2],ap[2]);
							ic_ptcpy(ecs.resval.rpoint,newextru);
							sds_trans(ap[1],&ucs,&ecs,0,ap[7]);
							sds_trans(ap[2],&ucs,&ecs,0,ap[0]);
							trbp1->resval.rreal=sds_angle(ap[7],ap[0]);
						}
						break;

					case 41:
					case 49:
				        trbp1->resval.rreal *= /*xscale Temp SMR */1.0;
						break;

					case 53:
						patternlineangle = trbp1;
						break;

					case 43:
						patternbase = trbp1;
						break;

					case 44:	//calcs rotation and base
						{
							if (!patternbase)
								break;

							sds_point base, unitaway;

							base[0] = patternbase->resval.rreal;
							base[1] = trbp1->resval.rreal;
							base[2] = 0;

							// Rotation - Determine new rotation by xforming the ins pt
							// and a pt 1 unit away in the rotation direction:
							unitaway[0] = base[0] + cos(patternlineangle->resval.rreal);
							unitaway[1] = base[1] + sin(patternlineangle->resval.rreal);
							unitaway[2] = 0;

							ic_ptcpy(ecs.resval.rpoint,oldextru);
							sds_trans(base,&ecs,&ucs,0,ap[1]);
							sds_trans(unitaway,&ecs,&ucs,0,ap[2]);
							//sds_xformpt(ap[1],ap[1],wmat);
							cmd_mirrorpt(defpt,unorm,ap[1],ap[1]);
							//sds_xformpt(ap[2],ap[2],wmat);
							cmd_mirrorpt(defpt,unorm,ap[2],ap[2]);
							ic_ptcpy(ecs.resval.rpoint,newextru);
							sds_trans(ap[1],&ucs,&ecs,0,base);
							sds_trans(ap[2],&ucs,&ecs,0,unitaway);

							patternbase->resval.rreal = base[0];
							trbp1->resval.rreal = base[1];

							patternlineangle->resval.rreal=sds_angle(base,unitaway);
						}
						break;

					case 45:
						patternoffset = trbp1;
						break;

					case 46:
						if (!patternoffset)
							break;

						//since this is a vector - apply the transform to it, then apply transform to 0,0,0
						//then subtract the 0,0,0 result from the vector transform to get the correct offset.
						ap[1][0] = patternoffset->resval.rreal;
						ap[1][1] = trbp1->resval.rreal;
						ap[1][2] = oldextru[2];
						ic_ptcpy(ecs.resval.rpoint,oldextru);
						sds_trans(ap[1],&ecs,&ucs,0,ap[0]);
						//sds_xformpt(ap[0],ap[0],wmat);
						cmd_mirrorpt(defpt,unorm,ap[0],ap[0]);						
						ic_ptcpy(ecs.resval.rpoint,newextru);
						sds_trans(ap[0],&ucs,&ecs,0,ap[2]);

						// transform 0,0,0
						ap[3][0] = 0;
						ap[3][1] = 0;
						ap[3][2] = 0;
						ic_ptcpy(ecs.resval.rpoint,oldextru);
						sds_trans(ap[3],&ecs,&ucs,0,ap[4]);
						//sds_xformpt(ap[4],ap[4],wmat);
						cmd_mirrorpt(defpt,unorm,ap[4],ap[4]);
						ic_ptcpy(ecs.resval.rpoint,newextru);
						sds_trans(ap[4],&ucs,&ecs,0,ap[3]);

						patternoffset->resval.rreal = ap[2][0] - ap[3][0];
						trbp1->resval.rreal = ap[2][1] - ap[3][1];
						break;

				}//switch

		  }
		  //End of add. SMR 2001/5/31
         
          } else if (enttype==DB_DIMENSION) {
         
         
         
              for (trbp1=elist; trbp1!=NULL; trbp1=trbp1->rbnext) {
                  switch (trbp1->restype) {
                      case 70: rbp[0]=trbp1; break;  /* DIM type */
                      case 10: rbp[1]=trbp1; break;  /* WCS */
                      case 13: rbp[2]=trbp1; break;  /* WCS */
                      case 14: rbp[3]=trbp1; break;  /* WCS */
                      case 15: rbp[4]=trbp1; break;  /* WCS */
                      case 11: rbp[5]=trbp1; break;  /* ECS */
                      case 12: rbp[6]=trbp1; break;  /* ECS */
                      case 16: rbp[7]=trbp1; break;  /* ECS */
					  case 50: rbp[8]=trbp1; break;	 /* UCS */
                  }
              }
              if (rbp[0]==NULL) break;  /* Expected group not found. */
              /* NULL the ones not needed in each type: */
              switch (rbp[0]->resval.rint&7) {
                  case 0: case 1: case 6:        rbp[4]=rbp[7]=NULL; break;
                  case 3: case 4:  rbp[2]=rbp[3]=rbp[7]=rbp[8]=NULL; break;
                  case 5:                        rbp[7]=rbp[8]=NULL; break;
              }
         
              /* Do the WCS points: */
              for (fi1=1; fi1<5; fi1++) {
                  if (rbp[fi1]==NULL) continue;
                  sds_trans(rbp[fi1]->resval.rpoint,&wcs,&ucs,0,ap[0]);
                  cmd_mirrorpt(defpt,unorm,ap[0],ap[0]);
                  sds_trans(ap[0],&ucs,&wcs,0,rbp[fi1]->resval.rpoint);
              }
         
              /* Do the ECS points: */
              for (fi1=5; fi1<8; fi1++) {
                  if (rbp[fi1]==NULL) continue;
				  if (6==fi1)continue;
                  ic_ptcpy(ecs.resval.rpoint,oldextru);
                  sds_trans(rbp[fi1]->resval.rpoint,&ecs,&ucs,0,ap[0]);
                  cmd_mirrorpt(defpt,unorm,ap[0],ap[0]);
                  ic_ptcpy(ecs.resval.rpoint,newextru);
                  sds_trans(ap[0],&ucs,&ecs,0,rbp[fi1]->resval.rpoint);
              }
			  /* Do the UCS angle: */
			  if (rbp[8]){
				  ap[0][0]=cos(rbp[8]->resval.rreal);
				  ap[0][1]=sin(rbp[8]->resval.rreal);
				  ap[0][2]=0.0;
				  ap[1][0]=ap[1][1]=ap[1][2]=0.0;
				  cmd_mirrorpt(ap[1],unorm,ap[0],ap[0]);
				  rbp[8]->resval.rreal=atan2(ap[0][1],ap[0][0]);
			  }

         
            } else if (enttype==DB_POLYLINE) {

                for (trbp1=elist; trbp1!=NULL; trbp1=trbp1->rbnext) {
                    switch (trbp1->restype) {
                        case 10: rbp[0]=trbp1; break;  /* (0,0,elev) */
                        case 70: rbp[1]=trbp1; break;  /* Flags */
                    }
                }
                for (fi1=0; fi1<2 && rbp[fi1]!=NULL; fi1++);
                if (fi1<2) break;  /* Expected group not found. */

                if (!(rbp[1]->resval.rint & (IC_PLINE_POLYFACEMESH | IC_PLINE_3DMESH | IC_PLINE_3DPLINE))) {  /* 2D */
                    /* The (0,0,elev) pt should transform to the proper */
                    /* elevation, but we should zero X and Y: */
                    ic_ptcpy(ecs.resval.rpoint,oldextru);
                    sds_trans(rbp[0]->resval.rpoint,&ecs,&ucs,0,ap[0]);
                    cmd_mirrorpt(defpt,unorm,ap[0],ap[0]);
                    ic_ptcpy(ecs.resval.rpoint,newextru);
                    sds_trans(ap[0],&ucs,&ecs,0,rbp[0]->resval.rpoint);
                    rbp[0]->resval.rpoint[0]=rbp[0]->resval.rpoint[1]=0.0;
                }

                complexent=1;

            } else if (enttype==DB_VERTEX) {

                for (trbp1=elist; trbp1!=NULL; trbp1=trbp1->rbnext) {
                    switch (trbp1->restype) {
                        case 10: rbp[0]=trbp1; break;  /* Point */
                        case 42: rbp[1]=trbp1; break;  /* Bulge */
                        case 70: rbp[2]=trbp1; break;  /* Flags */
                        case 50: rbp[3]=trbp1; break;  /* Tangent dir */
                    }
                }
                for (fi1=0; fi1<4 && rbp[fi1]!=NULL; fi1++);
                if (fi1<4) break;  /* Expected group not found. */

                if (rbp[2]->resval.rint & (IC_VERTEX_FACELIST | IC_VERTEX_3DMESHVERT | IC_VERTEX_3DPLINE)) {  /* Some type of 3D */

                    if (!(rbp[2]->resval.rint & IC_VERTEX_FACELIST) ||
                         (rbp[2]->resval.rint & IC_VERTEX_3DMESHVERT)) {
                        /* A 3D (WCS) point (not a PFACE face def) */

                        sds_trans(rbp[0]->resval.rpoint,&wcs,&ucs,0,ap[0]);
                        cmd_mirrorpt(defpt,unorm,ap[0],ap[0]);
                        sds_trans(ap[0],&ucs,&wcs,0,rbp[0]->resval.rpoint);
                    }

                } else {  /* 2D */
                    sds_point newinspt;

                    /* Tangent direction (do it 1st before we change the pt) */
                    /*
                    **  Determine new direction by mirroring the pt
                    **  and a pt 1 unit away in the tangent direction:
                    */

                    fi1=((rbp[2]->resval.rint & IC_VERTEX_HASTANGENT)!=0);  /* Is the 50 valid? */

                    ic_ptcpy(ecs.resval.rpoint,oldplextru);
                    sds_trans(rbp[0]->resval.rpoint,&ecs,&ucs,0,ap[1]);
                    cmd_mirrorpt(defpt,unorm,ap[1],ap[1]);
                    if (fi1) {
                        ap[0][0]=rbp[0]->resval.rpoint[0]+
                            cos(rbp[3]->resval.rreal);
                        ap[0][1]=rbp[0]->resval.rpoint[1]+
                            sin(rbp[3]->resval.rreal);
                        ap[0][2]=rbp[0]->resval.rpoint[2];
                        sds_trans(ap[0],&ecs,&ucs,0,ap[2]);
                        cmd_mirrorpt(defpt,unorm,ap[2],ap[2]);
                    }
                    ic_ptcpy(ecs.resval.rpoint,newplextru);
                    sds_trans(ap[1],&ucs,&ecs,0,newinspt);
                    if (fi1) {
                        sds_trans(ap[2],&ucs,&ecs,0,ap[0]);
                        rbp[3]->resval.rreal=sds_angle(newinspt,ap[0]);
                    }

                    /* Take the mirrored pt: */
                    ic_ptcpy(rbp[0]->resval.rpoint,newinspt);

                    /* Negate the bulge: */
                    if (rbp[1]->resval.rreal!=0)
                        rbp[1]->resval.rreal=-rbp[1]->resval.rreal;
                }

            } else if (enttype==DB_SEQEND) {

                complexent=0;

            } else if (enttype==DB_LWPOLYLINE) {
                for (trbp1=elist; trbp1!=NULL; trbp1=trbp1->rbnext) {
                    switch (trbp1->restype) {
                        case 38: rbp[0]=trbp1; break;  /* elev */
                        case 70: rbp[1]=trbp1; break;  /* Flags */
                    }
                }
                for (fi1=0; fi1<2 && rbp[fi1]!=NULL; fi1++);
                if (fi1<2) break;  /* Expected group not found. */

				/* The (0,0,elev) pt should transform to the proper */
				/* elevation, but we should zero X and Y: */
				ic_ptcpy(ecs.resval.rpoint,oldextru);
				sds_point temppt;
				temppt[0]=temppt[1]=0.0;
				temppt[2]=rbp[0]->resval.rreal;
				sds_trans(temppt,&ecs,&ucs,0,ap[0]);
				cmd_mirrorpt(defpt,unorm,ap[0],ap[0]);
				ic_ptcpy(ecs.resval.rpoint,newextru);
				sds_trans(ap[0],&ucs,&ecs,0,temppt);
				rbp[0]->resval.rreal=temppt[2];

// now do the verts

				resbuf *thevert=ic_ret_assoc(elist,10);
				do 
					{
					rbp[0]=thevert;
					while (thevert->restype!=42) // bulge
						thevert=thevert->rbnext;
					rbp[1]=thevert;
					while (thevert && thevert->restype!=10)
						thevert=thevert->rbnext;

	                sds_point newinspt;

	                ic_ptcpy(ecs.resval.rpoint,oldplextru);
		            sds_trans(rbp[0]->resval.rpoint,&ecs,&ucs,0,ap[1]);
			        cmd_mirrorpt(defpt,unorm,ap[1],ap[1]);
				    ic_ptcpy(ecs.resval.rpoint,newplextru);
					sds_trans(ap[1],&ucs,&ecs,0,newinspt);

	                /* Take the mirrored pt: */
		            ic_ptcpy(rbp[0]->resval.rpoint,newinspt);

			        /* Negate the bulge: */
				    if (rbp[1]->resval.rreal!=0)
					    rbp[1]->resval.rreal=-rbp[1]->resval.rreal;
	                }
				while (thevert);
            } else if (enttype==DB_INSERT) {
                sds_point newinspt;

                for (trbp1=elist; trbp1!=NULL; trbp1=trbp1->rbnext) {
                    switch (trbp1->restype) {
                        case 10: rbp[0]=trbp1; break;  /* Ins pt */
                        case 41: rbp[1]=trbp1; break;  /* X scale */
                        case 50: rbp[2]=trbp1; break;  /* Rotation */
                        case 66: rbp[3]=trbp1; break;  /* Atts follow */
                                                       /*   MAY NOT BE PRESENT */

                        // SystemMetrix(watanabe)-[mirror LEADER entity
                        case 330:
                            ReleaseSoftPointer(trbp1, mapLeader);
                            break;
                        // ]-SystemMetrix(watanabe) mirror LEADER entity
                    }
                }
                /* 1 less because the last one may not be present: */
                for (fi1=0; fi1<3 && rbp[fi1]!=NULL; fi1++);
                if (fi1<3) break;  /* Expected group not found. */

                /* Rotation (do it 1st before we change the other params) */
                /*
                **  Determine new rotation by mirroring the ins pt
                **  and a pt 1 unit away in the rotation direction:
                */
                ap[0][0]=rbp[0]->resval.rpoint[0]+
                    cos(rbp[2]->resval.rreal);
                ap[0][1]=rbp[0]->resval.rpoint[1]+
                    sin(rbp[2]->resval.rreal);
                ap[0][2]=rbp[0]->resval.rpoint[2];
                ic_ptcpy(ecs.resval.rpoint,oldextru);
                sds_trans(rbp[0]->resval.rpoint,&ecs,&ucs,0,ap[1]);
                sds_trans(ap[0],&ecs,&ucs,0,ap[2]);
                cmd_mirrorpt(defpt,unorm,ap[1],ap[1]);
                cmd_mirrorpt(defpt,unorm,ap[2],ap[2]);
                ic_ptcpy(ecs.resval.rpoint,newextru);
                sds_trans(ap[1],&ucs,&ecs,0,newinspt);
                sds_trans(ap[2],&ucs,&ecs,0,ap[0]);
                /* Get angle backward (rot pt to ins pt) to correct for */
                /* negating the X scale later: */
                rbp[2]->resval.rreal=sds_angle(ap[0],newinspt);

                /* Ins pt */
                ic_ptcpy(rbp[0]->resval.rpoint,newinspt);

                /* Negate the X scale: */
                rbp[1]->resval.rreal=-rbp[1]->resval.rreal;

                /* Check for "ATTRIBs follow" (complex entity): */
                complexent=(rbp[3]!=NULL && rbp[3]->resval.rint);

            } else if (enttype==DB_TOLERANCE) {

                for (trbp1=elist; trbp1!=NULL; trbp1=trbp1->rbnext) {
                    switch (trbp1->restype) {
                        case  10: rbp[0]=trbp1; break;  /* 1st pt */
                        case  11: rbp[1]=trbp1; break;  /* 2nd pt */
                        case 210: rbp[2]=trbp1; break;  /* 210 */

                        // SystemMetrix(watanabe)-[mirror LEADER entity
                        case 330:
                            ReleaseSoftPointer(trbp1, mapLeader);
                            break;
                        // ]-SystemMetrix(watanabe) mirror LEADER entity
                    }
                }
                for (fi1=0; fi1<2 && rbp[fi1]!=NULL; fi1++);
                if (fi1<2) break;  /* Expected group not found. */

                /* Pt (WCS): */
                sds_trans(rbp[0]->resval.rpoint,&wcs,&ucs,0,ap[0]);
                cmd_mirrorpt(defpt,unorm,ap[0],ap[0]);
                sds_trans(ap[0],&ucs,&wcs,0,rbp[0]->resval.rpoint);

                /* Mirror (0,0,0) and the old vector to get the new vector: */
                ap[0][0]=ap[0][1]=ap[0][2]=0.0;
                sds_trans(ap[0]                ,&wcs,&ucs,0,ap[2]);
                sds_trans(rbp[1]->resval.rpoint,&wcs,&ucs,0,ap[3]);
                cmd_mirrorpt(defpt,unorm,ap[2],ap[2]);
                cmd_mirrorpt(defpt,unorm,ap[3],ap[3]);
                sds_trans(ap[2],&ucs,&wcs,0,ap[0]);
                sds_trans(ap[3],&ucs,&wcs,0,ap[1]);
                rbp[1]->resval.rpoint[0]=ap[1][0]-ap[0][0];
                rbp[1]->resval.rpoint[1]=ap[1][1]-ap[0][1];
                rbp[1]->resval.rpoint[2]=ap[1][2]-ap[0][2];
                /* Make sure it's unit length: */
                ar[0]=sqrt(rbp[1]->resval.rpoint[0]*rbp[1]->resval.rpoint[0]+
                           rbp[1]->resval.rpoint[1]*rbp[1]->resval.rpoint[1]+
                           rbp[1]->resval.rpoint[2]*rbp[1]->resval.rpoint[2]);
                if (!icadRealEqual(ar[0],0.0)) {
                    if (!icadRealEqual(ar[0],1.0)) {
                        rbp[1]->resval.rpoint[0]/=ar[0];
                        rbp[1]->resval.rpoint[1]/=ar[0];
                        rbp[1]->resval.rpoint[2]/=ar[0];
                    }
                } else {  /* Default to (1,0,0) */
                    rbp[1]->resval.rpoint[0]=1.0;
                    rbp[1]->resval.rpoint[1]=rbp[1]->resval.rpoint[2]=0.0;
                }
                
                if (mirrtext) {
                    if (rbp[2]==NULL)break;
				    rbp[2]->resval.rpoint[0]=-rbp[2]->resval.rpoint[0];
				    rbp[2]->resval.rpoint[1]=-rbp[2]->resval.rpoint[1];
				    rbp[2]->resval.rpoint[2]=-rbp[2]->resval.rpoint[2];
                }
            
            } else if (enttype==DB_RAY || enttype==DB_XLINE) {

                for (trbp1=elist; trbp1!=NULL; trbp1=trbp1->rbnext) {
                    switch (trbp1->restype) {
                        case  10: rbp[0]=trbp1; break;  /* 1st pt */
                        case  11: rbp[1]=trbp1; break;  /* 2nd pt */
                    }
                }
                for (fi1=0; fi1<2 && rbp[fi1]!=NULL; fi1++);
                if (fi1<2) break;  /* Expected group not found. */

                /* Pt (WCS): */
                sds_trans(rbp[0]->resval.rpoint,&wcs,&ucs,0,ap[0]);
                cmd_mirrorpt(defpt,unorm,ap[0],ap[0]);
                sds_trans(ap[0],&ucs,&wcs,0,rbp[0]->resval.rpoint);

                /* Mirror (0,0,0) and the old vector to get the new vector: */
                ap[0][0]=ap[0][1]=ap[0][2]=0.0;
                sds_trans(ap[0]                ,&wcs,&ucs,0,ap[2]);
                sds_trans(rbp[1]->resval.rpoint,&wcs,&ucs,0,ap[3]);
                cmd_mirrorpt(defpt,unorm,ap[2],ap[2]);
                cmd_mirrorpt(defpt,unorm,ap[3],ap[3]);
                sds_trans(ap[2],&ucs,&wcs,0,ap[0]);
                sds_trans(ap[3],&ucs,&wcs,0,ap[1]);
                rbp[1]->resval.rpoint[0]=ap[1][0]-ap[0][0];
                rbp[1]->resval.rpoint[1]=ap[1][1]-ap[0][1];
                rbp[1]->resval.rpoint[2]=ap[1][2]-ap[0][2];
                /* Make sure it's unit length: */
                ar[0]=sqrt(rbp[1]->resval.rpoint[0]*rbp[1]->resval.rpoint[0]+
                           rbp[1]->resval.rpoint[1]*rbp[1]->resval.rpoint[1]+
                           rbp[1]->resval.rpoint[2]*rbp[1]->resval.rpoint[2]);
                if (!icadRealEqual(ar[0],0.0)) {
                    if (!icadRealEqual(ar[0],1.0)) {
                        rbp[1]->resval.rpoint[0]/=ar[0];
                        rbp[1]->resval.rpoint[1]/=ar[0];
                        rbp[1]->resval.rpoint[2]/=ar[0];
                    }
                } else {  /* Default to (1,0,0) */
                    rbp[1]->resval.rpoint[0]=1.0;
                    rbp[1]->resval.rpoint[1]=rbp[1]->resval.rpoint[2]=0.0;
                }

            } else if (enttype==DB_SPLINE) {

                for (trbp1=elist; trbp1!=NULL; trbp1=trbp1->rbnext) {
                    switch (trbp1->restype) {
                        case  10: 
                        case  11:/* Pt (WCS): */
								sds_trans(trbp1->resval.rpoint,&wcs,&ucs,0,ap[0]);
								cmd_mirrorpt(defpt,unorm,ap[0],ap[0]);
								sds_trans(ap[0],&ucs,&wcs,0,trbp1->resval.rpoint);
								break;
						case  12:
						case  13:
								/* Mirror (0,0,0) and the old vector to get the new vector: */
								ap[0][0]=ap[0][1]=ap[0][2]=0.0;
								sds_trans(ap[0]                ,&wcs,&ucs,0,ap[2]);
								sds_trans(trbp1->resval.rpoint,&wcs,&ucs,0,ap[3]);
								cmd_mirrorpt(defpt,unorm,ap[2],ap[2]);
								cmd_mirrorpt(defpt,unorm,ap[3],ap[3]);
								sds_trans(ap[2],&ucs,&wcs,0,ap[0]);
								sds_trans(ap[3],&ucs,&wcs,0,ap[1]);
								trbp1->resval.rpoint[0]=ap[1][0]-ap[0][0];
								trbp1->resval.rpoint[1]=ap[1][1]-ap[0][1];
								trbp1->resval.rpoint[2]=ap[1][2]-ap[0][2];
								/* Make sure it's unit length: */
								ar[0]=sqrt(trbp1->resval.rpoint[0]*trbp1->resval.rpoint[0]+
										   trbp1->resval.rpoint[1]*trbp1->resval.rpoint[1]+
										   trbp1->resval.rpoint[2]*trbp1->resval.rpoint[2]);
								if (!icadRealEqual(ar[0],0.0)) {
									if (!icadRealEqual(ar[0],1.0)) {
										trbp1->resval.rpoint[0]/=ar[0];
										trbp1->resval.rpoint[1]/=ar[0];
										trbp1->resval.rpoint[2]/=ar[0];
									} /*D.Gavrilov*/// Dont't make a zero vectors non-zero (no default!).
								}/* else {  // Default to (1,0,0)
									trbp1->resval.rpoint[0]=1.0;
									trbp1->resval.rpoint[1]=trbp1->resval.rpoint[2]=0.0;
								}*/
								break;
                    }
                }

            } 
            else if ((enttype == DB_3DSOLID) || 
                     (enttype == DB_REGION) ||
                     (enttype == DB_BODY))
            { 
                //<ALM>
                do
                {
                    // Create mirror transformation
#if 0
                    sds_matrix mtx;
                    bool ok;
                    ic_mirrorPlane(defpt, norm, ok, mtx);
                    if (!ok)
                        break;

                    // Reduce sds_matrix to Acis form
                    ic_transpose(mtx);
#else
                    transf tr;
                    {
                        transf u2w = ucs2wcs();
                        point wcs_root = point(defpt)*u2w;
                        gvector wcs_normal = gvector(norm)*u2w;
                        gvector shift = wcs_root - point(0.,0.,0.);
                        
                        tr = icl::translation(-shift) * 
                             icl::reflection(wcs_normal) * 
                             icl::translation(shift);
                    }

#endif
                    // Get object's data
                    resbuf*	pSatRb = ic_ret_assoc(elist, 1);
                    if (!pSatRb)
                        break;

                    CDbAcisEntityData newObj;
					CDbAcisEntity* pEntity = reinterpret_cast<CDbAcisEntity*>(ename[0]);

                    // Transform
                    if (CModeler::get()->transform(pEntity->getData(), newObj, tr))
                    {
                        // Change resbuf data
						char*& sat = pSatRb->resval.rstring;
                        IC_FREE(sat);
                        sat = new char [newObj.getSat().size()+1];
                        strcpy(sat, newObj.getSat().c_str());
                    }
                }
                while (false);
                //</ALM>
            }
            // SystemMetrix(watanabe)-[mirror LEADER entity
            else if (enttype == DB_LEADER)
              {
                MirrorLeader(elist, defpt, unorm);
              }
            // ]-SystemMetrix(watanabe) mirror LEADER entity
            else ok2make=0;

            /* Fix the EED points that mirror: */
            for (trbp1=elist; trbp1!=NULL && trbp1->restype!=-3; trbp1=
                trbp1->rbnext);
            for (; trbp1!=NULL; trbp1=trbp1->rbnext) {
                switch (trbp1->restype) {
                    case 1011:  /* A WCS point */
                        sds_trans(trbp1->resval.rpoint,&wcs,&ucs,0,ap[0]);
                        cmd_mirrorpt(defpt,unorm,ap[0],ap[0]);
                        sds_trans(ap[0],&ucs,&wcs,0,trbp1->resval.rpoint);
                        break;
                    case 1012: case 1013:  /* WCS vectors */
                        /* Mirror (0,0,0) and the old vector to get the new: */
                        ap[0][0]=ap[0][1]=ap[0][2]=0.0;
                        sds_trans(ap[0],&wcs,&ucs,0,ap[2]);
                        sds_trans(trbp1->resval.rpoint,&wcs,&ucs,0,ap[3]);
                        cmd_mirrorpt(defpt,unorm,ap[2],ap[2]);
                        cmd_mirrorpt(defpt,unorm,ap[3],ap[3]);
                        sds_trans(ap[2],&ucs,&wcs,0,ap[0]);
                        sds_trans(ap[3],&ucs,&wcs,0,ap[1]);
                        trbp1->resval.rpoint[0]=ap[1][0]-ap[0][0];
                        trbp1->resval.rpoint[1]=ap[1][1]-ap[0][1];
                        trbp1->resval.rpoint[2]=ap[1][2]-ap[0][2];
                        break;
                }
            }

            /*
            **  Do an entmod when ResourceString(IDC_CMDS9_DELETING_OLD_42, "deleting old" ); this maintains the
            **  same handles (which is what ACAD does):
            */
            if (ok2make){
				if (delold){
					sds_entmod(elist);
				}else{
					if (enttype==DB_DIMENSION){
						//remove the 2 group from the llist
						for (trbp1=elist; trbp1!=NULL; trbp1=trbp1->rbnext){
							if (trbp1->rbnext!=NULL && trbp1->rbnext->restype==2){
								rbp[9]=trbp1->rbnext;
								trbp1->rbnext=rbp[9]->rbnext;
								rbp[9]->rbnext=NULL;
								sds_relrb(rbp[9]);
								rbp[9]=trbp1=NULL;
								break;
							}
						}
						/*ic_assoc(elist,2);
						ic_rbassoc->resval.rstring[2]=0;//set null after *D
						ic_rbassoc=NULL;*/
					}
					
					//Bugzilla No. 77993 ; date : 18-03-2002
					SDS_AtNodeDrag = 1;					
					if (sds_entmake(elist)!=RTNORM) { rc=RTERROR; goto out; }
					SDS_AtNodeDrag = SavedSDS_AtNodeDrag;
					//Bugzilla No. 77993 ; date : 18-03-2002

					sds_redraw(ename,IC_REDRAW_UNHILITE);//redraw old ent
				}

                // SystemMetrix(watanabe)-[mirror LEADER entity
                sds_name newname;
                sds_entlast(newname);

                sds_name anno, leader;
                if (mapLeader.Lookup(ename, leader))
                  {
                    // replace leader
                    mapLeader.SetAt(ename, newname);
                  }
                else if (mapRelease.Lookup(ename, leader))
                  {
                    // replace for releasing soft pointer
                    mapRelease.SetAt(ename, newname);
                  }
                else if (mapAnno.Lookup(ename, anno))
                  {
                    // replace annotation
                    mapAnno.SetAt(ename, newname);
                  }
                // ]-SystemMetrix(watanabe) mirror LEADER entity
				//Bugzilla No 78016 18-03-2002  [
				db_handitem *oldEntity = (db_handitem*) ename[0];
				// old entity may be a seqend so need to get the polyline
				if (oldEntity->ret_type() == DB_SEQEND)
				((db_seqend *)oldEntity)->get_mainent(&oldEntity);
				
				db_handitem *newEntity = (db_handitem*)newname[0];

				reactorFixup.AddHanditem(newEntity);
				reactorFixup.AddHandleMap(oldEntity->RetHandle(),newEntity->RetHandle());
				//Bugzilla No 78016 18-03-2002  ]
			}
            if (complexent && sds_entnext_noxref(ename,ename)!=RTNORM) break;

        } while (complexent);

    }  /* End for each member of ss */

    //Bugzilla No 78016 18-03-2002  [
	if(!delold)
	reactorFixup.ApplyFixups(SDS_CURDWG);
	//Bugzilla No 78016 18-03-2002  ]
    // SystemMetrix(watanabe)-[mirror LEADER entity
    ReplaceReactor(&allapps, mapLeader, mapAnno, mapRelease);
    // ]-SystemMetrix(watanabe) mirror LEADER entity

out:
	SDS_ProgressStop();

    //Bugzilla No 78016 18-03-2002 
	SDS_CURDWG->SetCopyingFlag(false);
    if (elist!=NULL) { sds_relrb(elist); elist=NULL; }

    //Bugzilla No. 77993 ; date : 18-03-2002
	/* Restore saved node drag value */
	SDS_AtNodeDrag = SavedSDS_AtNodeDrag;

	/* Restore color and ltype: */
    if (savcolor.restype==RTSTR && savcolor.resval.rstring!=NULL) {
        if (setcolor) sds_setvar("CECOLOR"/*DNT*/,&savcolor);
        IC_FREE(savcolor.resval.rstring); savcolor.resval.rstring=NULL;
        savcolor.restype=RTNONE;
    }
    if (savlt.restype==RTSTR && savlt.resval.rstring!=NULL) {
        if (setlt) sds_setvar("CELTYPE"/*DNT*/,&savlt);
        IC_FREE(savlt.resval.rstring); savlt.resval.rstring=NULL;
        savlt.restype=RTNONE;
    }

    return rc;
}

short cmd_mirror(void) { 

    if (cmd_iswindowopen() == RTERROR) return RTERROR;			

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    char		fs1[81];
    short		delold	= 0,		// Delete the old entities?
				gotss	= 0,		// Do we have a selection set?
				ok;
	RT			rc		= RTNORM;
    long		fl1		= 0L,
				fl2		= 0L;
    sds_point	p0,
				p1,
				defpt,
				norm;
    sds_name	ss,
				ent;
	char		msg=0;
    struct cmd_drag_globalpacket *gpak = &cmd_drag_gpak;
	RT ret;
	bool userNotified = false;
	long removedCount = 0;

    gpak->reference = 0; gpak->refval = 0.0; gpak->rband = 1;


    rc=RTNORM; delold=0; gotss=0;

    if (sds_pmtssget(ResourceString(IDC_CMDS9__NSELECT_ENTITIES_43, "\nSelect entities to mirror: " ),NULL,NULL,NULL,NULL,ss,0)!=RTNORM) { rc=RTCAN; goto out; }
    gotss=1;	

	sds_sslength(ss,&fl1);
	// Remove proxy entities from the selection set.
	for(fl2=0L;sds_ssname(ss,fl2,ent)==RTNORM;fl2++) 
	{
		if (IC_TYPE_FROM_ENAME(ent) == DB_ACAD_PROXY_ENTITY) 
		{
			if (userNotified == false)
			{
				userNotified = true;
				sds_printf(ResourceString(CMD_ERR_DWGPROXY, "Proxy entities can not be edited." ));
			}
			removedCount++;
			sds_redraw(ent,IC_REDRAW_UNHILITE);
			sds_ssdel(ent, ss);
		}
	}
	if (removedCount)
	{
		sprintf(fs1, "%s%d", ResourceString(IDC_ICADSELECTDIA__ENTITIES__8, " Entities removed="), removedCount);
		sds_printf(fs1);
	}

    if (sds_sslength(ss,&fl1)!=RTNORM) { rc=RTERROR; goto out; }

    if (fl1<1L) goto out;
	
	//TODO - this is temp code to filter out viewports for release 1.0
	for (fl2=0L;fl2<fl1;fl2++){
		sds_ssname(ss,fl2,ent);
		if (DB_VIEWPORT==IC_TYPE_FROM_ENAME(ent)){
			fl2--;
			fl1--;
			sds_ssdel(ent,ss);
			sds_redraw(ent,IC_REDRAW_UNHILITE);
			msg=1;
		}
	}
	if (1==msg && fl1>0L)sds_printf(ResourceString(IDC_CMDS9__NUNABLE_TO_MIRRO_44, "\nUnable to mirror viewports." ));	
	//if we filtered out all the vports and there's nothing left...
	if (fl1<1L){
		//leave gotss as 1 even though it's an empty set
		cmd_ErrorPrompt(CMD_ERR_SELECTION,0);
		rc=RTNORM;
		goto out;
	}
	//**END TEMP CODE

	//Blocked by IntelliKorea for supporting BHATCH mirror. 2001/5/31
	/*if (RemoveHatchFromSS(ss) == RTNIL)
		{
		rc = -1;
		goto out;
		}*/
	//IntelliKorea. 2001/5/31

    if (sds_initget(SDS_RSG_NODOCCHG,NULL)!=RTNORM) { rc=RTERROR; goto out; }
    if (sds_getpoint(NULL,ResourceString(IDC_CMDS9__NSTART_OF_MIRROR_45, "\nStart of mirror line: " ),p0)!=RTNORM)
        { rc=RTCAN; goto out; }

	ic_ptcpy(gpak->pt1,p0);

    do {
        ok=1;
		SDS_PointDisplayMode=5;
		ic_ptcpy(SDS_PointDisplay,p0);
		if (sds_initget(SDS_RSG_NODOCCHG,NULL)!=RTNORM) { rc=RTERROR; goto out; }
		if (cmd_drag(CMD_MIRROR_DRAG, ss, ResourceString(IDC_CMDS9__NEND_OF_MIRROR_L_46, "\nEnd of mirror line: " ), 
			NULL, p1, fs1) != RTNORM) {
			SDS_PointDisplayMode = 0;
			rc = RTCAN;	goto out;
		}
		SDS_PointDisplayMode=0;
        if (icadPointEqual(p0,p1))
            { ok=0; cmd_ErrorPrompt(CMD_ERR_DIFFPTS,0); }
    } while (!ok);

    for (;;) {
//    	sds_grdraw(p0,p1,-1,0);

		if (sds_initget(SDS_RSG_NODOCCHG | RSG_OTHER,ResourceString(IDC_CMDS9_YES_DELETE_ENTITI_47, "Yes-Delete_entities|Yes No-Keep_entities|No ~_Yes ~_No" ))!=RTNORM) { rc=RTERROR; goto out; }
		rc=sds_getkword(ResourceString(IDC_CMDS9__NDELETE_THE_ORIG_48, "\nDelete the original entities? <N> " ),fs1);
		if (rc==RTERROR || rc==RTCAN) {
			goto out;
		} else if (rc==RTNONE) {
			rc=RTNORM; break;
		} else if (rc==RTNORM) {
			if (strisame(fs1,"YES"/*DNT*/ )) {
				delold=1; break;
			} else if (strisame(fs1,"NO"/*DNT*/ )) {
				delold=0; break;
			} else {
				cmd_ErrorPrompt(CMD_ERR_YESNO,0);		// "Yes or No, please"
				continue;
			}
		}
	} 

    /* Get the plane defpt and norm in the UCS: */
    defpt[0]=(p0[0]+p1[0])/2.0;
    defpt[1]=(p0[1]+p1[1])/2.0;
	//BugZilla No. 78419; 31-03-2003 [
    //defpt[2]=(p0[1]+p1[2])/2.0;
	defpt[2]=(p0[2]+p1[2])/2.0;
	//BugZilla No. 78419; 31-03-2003 ]
    norm[0]=p1[1]-p0[1]; norm[1]=p0[0]-p1[0]; norm[2]=0.0;

    // This is for dimensions.
    SDS_AtNodeDrag=1;
    cmd_mirrorss(ss,defpt,norm,delold);
    // This is for dimensions.
    SDS_AtNodeDrag=0;

out:
	if (rc<0){							// if RTCAN or RTERROR...
		for (fl2=0L;fl2<fl1;fl2++){
			sds_ssname(ss,fl2,ent);		// go through the selection set
			sds_redraw(ent,IC_REDRAW_UNHILITE);		// and unhighlight each entity.
		}
	}
    if (gotss) {						// if there is a selection set
		sds_ssfree(ss);					// free it.
	}
    return rc;
}

short cmd_mirror3d(void)   {

    if (cmd_iswindowopen() == RTERROR) return RTERROR;			
	if (cmd_isinpspace() == RTERROR) return RTERROR;			
	
	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    char fs1[IC_ACADBUF];
    short delold,gotss,ntoget,getoptsagain,ok;
	RT	rc;
    int fint1;
    long fl1;
    sds_point ap[10],defpt,norm,delta[2];
    sds_name ss,ename;
    struct resbuf *elist,sgrb;

    static short prevok;
    static sds_point prevdefpt,prevnorm={ 1.0, 0.0, 0.0 };


    rc=RTNORM; delold=ntoget=0; gotss=0; elist=NULL;
    /* Set plane to safe default: */
    defpt[0]=defpt[1]=defpt[2]=norm[1]=norm[2]=0.0; norm[0]=1.0;
    sgrb.rbnext=NULL;


    if (sds_pmtssget(NULL,NULL,NULL,NULL,NULL,ss,0)!=RTNORM) { rc=RTCAN; goto out; }
    gotss=1;
    if (sds_sslength(ss,&fl1)!=RTNORM) { rc=RTERROR; goto out; }
    if (fl1<1L) goto out;

	if (RemoveHatchFromSS(ss) == RTNIL)
		goto out;

    /* Most of the rest of the code just sets up defpt and norm for */
    /* the call to cmd_mirrorss(). */


    do {  /* A way to repeat when "Last" fails because there IS no prev. */

        getoptsagain=0;

        if (sds_initget(SDS_RSG_NODOCCHG,ResourceString(IDC_CMDS9_ENTITY_LAST_VIEW__51, "Entity Last View Z_axis|Zaxis X-Y_plane|XY Y-Z_plane|YZ Z-X_plane|ZX 3_points|3points ~_Entity ~_Last ~_View ~_Zaxis ~_XY ~_YZ ~_ZX ~_3points" ))!=RTNORM) { rc=RTERROR; goto out; }
        fint1=sds_getpoint(NULL,ResourceString(IDC_CMDS9__NDEFINE_MIRROR_P_52, "\nDefine mirror plane by:  Entity/Last/View/Zaxis/XY/YZ/ZX/<3points>: " ),
            ap[0]);
        if (fint1==RTKWORD) {
            *fs1=0;
            sds_getinput(fs1);

            if (strisame(fs1,"ENTITY"/*DNT*/ )) {
                struct resbuf *typelink,*p10link,*p12link,*flagslink,*trbp1;
                struct resbuf wcs,ucs,ecs;

                /* Set up resbufs for sds_trans(): */
                wcs.rbnext=ucs.rbnext=ecs.rbnext=NULL;
                wcs.restype=ucs.restype=RTSHORT; ecs.restype=RT3DPOINT;
                wcs.resval.rint=0; ucs.resval.rint=1;
                ecs.resval.rpoint[0]=ecs.resval.rpoint[1]=0.0;
                ecs.resval.rpoint[2]=1.0;

                do {

                    ok=1;

					if (sds_initget(SDS_RSG_NODOCCHG,NULL)!=RTNORM) { rc=RTERROR; goto out; }
                    fint1=sds_entsel(ResourceString(IDC_CMDS9__NPICK_AN_ENTITY__54, "\nPick an entity that has an extrusion direction: " ),ename,ap[0]);
                    if (fint1==RTCAN) { rc=RTCAN; goto out; }
                    if (fint1!=RTNORM) {
                        ok=0; cmd_ErrorPrompt(CMD_ERR_NOENT,0);
                    } else {
                        if (elist!=NULL) { sds_relrb(elist); elist=NULL; }
                        if ((elist=sds_entget(ename))==NULL) { rc=RTERROR; goto out; }
                        typelink=p10link=p12link=flagslink=NULL;
                        norm[0]=norm[1]=0.0; norm[2]=1.0;
                        for (trbp1=elist; trbp1!=NULL; trbp1=trbp1->rbnext) {
                            switch (trbp1->restype) {
                                case   0:  typelink=trbp1; break;
                                case  10:   p10link=trbp1; break;
                                case  12:   p12link=trbp1; break;
                                case  70: flagslink=trbp1; break;
                                case 210:
                                    ic_ptcpy(ecs.resval.rpoint,
                                        trbp1->resval.rpoint);
                                    sds_trans(trbp1->resval.rpoint,&wcs,
                                        &ucs,1,norm);
                                    break;
                            }
                        }
                        if (typelink==NULL) ok=0;
                        else {
                            if (stristr("|LINE|POINT|CIRCLE|SHAPE|TEXT|ARC|TRACE|SOLID|INSERT|ATTDEF|ATTRIB|"/*DNT*/,
                                typelink->resval.rstring)!=NULL) {

                                if (p10link==NULL) {
                                    ok=0;
                                } else {
                                    if (strisame(typelink->resval.rstring,db_hitype2str(DB_LINE)) ||
                                        strisame(typelink->resval.rstring,"POINT"/*DNT*/ )) {

                                        sds_trans(p10link->resval.rpoint,&wcs,
                                            &ucs,0,defpt);
                                    } else {
                                        sds_trans(p10link->resval.rpoint,&ecs,
                                            &ucs,0,defpt);
                                    }
                                }
                            } else if (strisame(typelink->resval.rstring, "POLYLINE"/*DNT*/ )) {

                                if (p10link==NULL || flagslink==NULL ||
                                    flagslink->resval.rint & (IC_PLINE_POLYFACEMESH | IC_PLINE_3DMESH | IC_PLINE_3DPLINE)) {

                                    ok=0;
                                } else {
                                    sds_trans(p10link->resval.rpoint,&ecs,
                                        &ucs,0,defpt);
                                }
                            } else if (strisame(typelink->resval.rstring, "DIMENSION"/*DNT*/ )) {

                                if (p12link==NULL) {
                                    ok=0;
                                } else {
                                    sds_trans(p12link->resval.rpoint,&ecs,
                                        &ucs,0,defpt);
                                }
                            } else ok=0;
                        }

                        if (!ok) cmd_ErrorPrompt(CMD_ERR_ENTTYPE,0);
                    }

                } while (!ok);

            } else if (strisame(fs1,"LAST"/*DNT*/ )) {
                if (prevok) {
                    ic_ptcpy(defpt,prevdefpt); ic_ptcpy(norm,prevnorm);
                } else {
					cmd_ErrorPrompt(CMD_ERR_NOPLANES,0);
                    getoptsagain=1;
                }
            } else if (strisame(fs1,"ZAXIS"/*DNT*/ )) {
                sds_initget(SDS_RSG_NODOCCHG | RSG_NONULL,NULL);
                if (sds_getpoint(NULL,ResourceString(IDC_CMDS9__NPOINT_ON_PLANE__59, "\nPoint on plane: " ),defpt)!=RTNORM)
                    { rc=RTCAN; goto out; }
                do {
                    ok=1;
                    sds_initget(SDS_RSG_NODOCCHG | RSG_NONULL,NULL);
                    if (sds_getpoint(defpt,ResourceString(IDC_CMDS9__NPOINT_ON_Z_AXIS_60, "\nPoint on Z-axis (normal) of the plane: " ),
                        ap[0])!=RTNORM) { rc=RTCAN; goto out; }
                    if (icadPointEqual(defpt,ap[0])) {

                        ok=0;
						cmd_ErrorPrompt(CMD_ERR_DIFFPTS,0);
                    }
                } while (!ok);
                norm[0]=ap[0][0]-defpt[0];
                norm[1]=ap[0][1]-defpt[1];
                norm[2]=ap[0][2]-defpt[2];
            } else if (strisame(fs1,"VIEW"/*DNT*/ )) {
                sds_initget(SDS_RSG_NODOCCHG,NULL);
                fint1=sds_getpoint(NULL,ResourceString(IDC_CMDS9__NPOINT_ON_VIEW_P_62, "\nPoint on view plane <0,0,0>: " ),defpt);
                if (fint1==RTNONE) ap[0][0]=ap[0][1]=ap[0][2]=0.0;
                else if (fint1!=RTNORM) { rc=RTCAN; goto out; }
                if (SDS_getvar(NULL,DB_QVIEWDIR,&sgrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)==RTNORM) {
                    ic_ptcpy(norm,sgrb.resval.rpoint);
                } else {
                    norm[0]=norm[1]=0.0; norm[2]=1.0;  /* Should never */
                }
            } else if (strisame(fs1,"XY"/*DNT*/ )) {
                sds_initget(SDS_RSG_NODOCCHG,NULL);
                fint1=sds_getpoint(NULL,ResourceString(IDC_CMDS9__NPOINT_ON_XY_PLA_64, "\nPoint on XY plane <0,0,0>: " ),defpt);
                if (fint1==RTNONE) ap[0][0]=ap[0][1]=ap[0][2]=0.0;
                else if (fint1!=RTNORM) { rc=RTCAN; goto out; }
                norm[0]=norm[1]=0.0; norm[2]=1.0;
            } else if (strisame(fs1,"YZ"/*DNT*/ )) {
                sds_initget(SDS_RSG_NODOCCHG,NULL);
                fint1=sds_getpoint(NULL,ResourceString(IDC_CMDS9__NPOINT_ON_YZ_PLA_66, "\nPoint on YZ plane <0,0,0>: " ),defpt);
                if (fint1==RTNONE) ap[0][0]=ap[0][1]=ap[0][2]=0.0;
                else if (fint1!=RTNORM) { rc=RTCAN; goto out; }
                norm[1]=norm[2]=0.0; norm[0]=1.0;
            } else if (strisame(fs1,"ZX"/*DNT*/ )) {
                sds_initget(SDS_RSG_NODOCCHG,NULL);
                fint1=sds_getpoint(NULL,ResourceString(IDC_CMDS9__NPOINT_ON_ZX_PLA_68, "\nPoint on ZX plane <0,0,0>: " ),defpt);
                if (fint1==RTNONE) ap[0][0]=ap[0][1]=ap[0][2]=0.0;
                else if (fint1!=RTNORM) { rc=RTCAN; goto out; }
                norm[0]=norm[2]=0.0; norm[1]=1.0;
            } else if (strisame(fs1,"3POINTS"/*DNT*/ )) {
                ntoget=3;
            } else { rc=RTERROR; goto out; }
        } else if (fint1==RTNONE) {  /* User just pressed ENTER */
            ntoget=3;
        } else if (fint1==RTNORM) {  /* User picked 1st point. */
            ntoget=2;
        } else { rc=RTCAN; goto out; }

        if (ntoget) {  /* Picked a point, or entered "3points". */
            if (ntoget>2) {
                sds_initget(SDS_RSG_NODOCCHG | RSG_NONULL,NULL);
                if (sds_getpoint(NULL,ResourceString(IDC_CMDS9__NFIRST_POINT_ON__70, "\nFirst point on plane: " ),ap[0])!=RTNORM)
                    { rc=RTCAN; goto out; }
            }
            do {
                ok=1;
                sds_initget(SDS_RSG_NODOCCHG | RSG_NONULL,NULL);
                if (sds_getpoint(ap[0],ResourceString(IDC_CMDS9__NSECOND_POINT_ON_71, "\nSecond point on plane: " ),ap[1])!=RTNORM)
                    { rc=RTCAN; goto out; }
                delta[0][0]=ap[1][0]-ap[0][0];
                delta[0][1]=ap[1][1]-ap[0][1];
                delta[0][2]=ap[1][2]-ap[0][2];
                if (icadRealEqual(delta[0][0],0.0) && icadRealEqual(delta[0][1],0.0) && icadRealEqual(delta[0][2],0.0)) {
					cmd_ErrorPrompt(CMD_ERR_DIFFPTS,0);
                    ok=0;
                }
            } while (!ok);
            do {
                ok=1;
                sds_initget(SDS_RSG_NODOCCHG | RSG_NONULL,NULL);
                if (sds_getpoint(ap[0],ResourceString(IDC_CMDS9__NTHIRD_POINT_ON__72, "\nThird point on plane: " ),ap[2])!=RTNORM)
                    { rc=RTCAN; goto out; }
                delta[1][0]=ap[2][0]-ap[0][0];
                delta[1][1]=ap[2][1]-ap[0][1];
                delta[1][2]=ap[2][2]-ap[0][2];
                norm[0]=delta[0][1]*delta[1][2]-delta[0][2]*delta[1][1];
                norm[1]=delta[0][2]*delta[1][0]-delta[0][0]*delta[1][2];
                norm[2]=delta[0][0]*delta[1][1]-delta[0][1]*delta[1][0];
                if (icadRealEqual(norm[0],0.0) && icadRealEqual(norm[1],0.0) && icadRealEqual(norm[2],0.0)) {
                    cmd_ErrorPrompt(CMD_ERR_ONSAMELINE,0);
                    norm[0]=1.0;
                    ok=0;
                }
            } while (!ok);

            ic_ptcpy(defpt,ap[0]);
        }

    } while (getoptsagain);

    for (;;) {
		sds_initget(SDS_RSG_NODOCCHG | SDS_RSG_OTHER,ResourceString(IDC_CMDS9_YES_DELETE_ENTITI_47, "Yes-Delete_entities|Yes No-Keep_entities|No ~_Yes ~_No" ));
		rc=sds_getkword(ResourceString(IDC_CMDS9__NDELETE_ORIGINAL_73, "\nDelete original entities? <N> " ),fs1);
		if (rc==RTERROR || rc==RTCAN) {
			goto out;
		} else if (rc==RTNONE) {
			rc=RTNORM; break;
		} else if (rc==RTNORM) {
			if (strisame(fs1,"YES"/*DNT*/ )) {
				delold=1; break;
			} else if (strisame(fs1,"NO"/*DNT*/ )) {
				delold=0; break;
			} else {
				cmd_ErrorPrompt(CMD_ERR_YESNO,0);		// "Yes or No, please"
				continue;
			}
		}
	} 

    cmd_mirrorss(ss,defpt,norm,delold);
    ic_ptcpy(prevdefpt,defpt); ic_ptcpy(prevnorm,norm); prevok=1;


out:
    if (gotss) sds_ssfree(ss);
    if (elist!=NULL) { sds_relrb(elist); elist=NULL; }

    return rc;
}



// Commands PFACE and 3DMESH


short cmd_pface(void) {
/*
**  PFACEs are quite complex.
**
**  The user enters one or more points, then defines ZERO or more faces
**  by specifying point numbers (the first point specified is 1).
**  (You can have a PFACE with points but no faces!  During
**  face-defining, the user can enter a color and/or layer for that
**  face.
**
**  In the database, the POLYLINE entity stores the number of points
**  (71) and the number of user-specified faces (72) (although ACAD
**  documentation says they aren't used).  The point-specifying VERTEXs
**  follow.  After them come the face-specifying VERTEXs.
**
**  Each face-specifying VERTEX can hold the definition of a CLOSED face
**  that has up to 4 point-specifiers in group codes 71-74.  Note that
**  a user-defined face with more that 4 points must be broken up
**  into several face-specifying VERTEXs in the database.  That is, the
**  user and the database have different definitions of what a face is.
**  Each database face (1 face-specifying VERTEX) can have it own
**  color (62) and layer (8).  A negative point number means that
**  the edge that follows is invisible.
**
**  Basically, when the user specifies a color and/or layer, the new
**  definitions apply to the current USER-DEFINED face and beyond,
**  until he changes them again.
**
**  The process of breaking up user-defined faces into vertex-defined
**  faces works like this.  For 1-4 points, one face-specifying VERTEX
**  is used.  For more than 4 points, the first 4 are thrown into
**  the first face-specifying VERTEX -- with the last point # being
**  negative to specify invisible closure to the first point --
**  and subsequent edges are defined as 3-point faces with first and
**  last edges invisible for the rest of the user-defined face.
**
**  Examples:
**
**      A 7-point face with all edges visible:
**
**          User enters points as 1 2 3 4 5 6 7
**
**          The 4 face-specifying VERTEXs' 71-74 groups:
**
**              71 72 73 74
**
**               1  2  3 -4
**              -1  4 -5  0
**              -1  5 -6  0
**              -1  6  7  0
**
**      A 7-point face with odd edges invisible:
**
**          User enters points as -1 2 -3 4 -5 6 -7
**
**          The 4 face-specifying VERTEXs' 71-74 groups:
**
**              71 72 73 74
**
**              -1  2 -3 -4
**              -1  4 -5  0
**              -1 -5 -6  0
**              -1  6 -7  0
*/

    if (cmd_iswindowopen() == RTERROR) return RTERROR;			

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    char pmpt[81],fs1[81],layer[IC_ACADNMLEN];
    short fn,maxfn,thispn,color,blcolor,bbcolor,fidx,firstnum;
    short savnum,ok,rc,fi1;
    int fint1,pn,maxpn;
    sds_point ap1,pt210;
    struct resbuf *elist,*g10,*g7x[4],*trbp1,rb;
    struct resbuf rbucs,rbwcs;

    /* Llist of points: */
    struct ptlink {
        sds_point pt;
        struct ptlink *next;
    } *ptll,*lastpt,*tpt;

    struct ptnumlink {
        short num;
        struct ptnumlink *next;
    } *pnll,*lastpn,*tpn;

    /* Llist of user-defined faces, each with a llist of point numbers: */
    struct facelink {
        short             color;
        char             *layer;
        struct ptnumlink *pnll;
        struct facelink  *next;
    } *facell,*lastface,*tface;


    rc=RTNORM; elist=NULL; blcolor=256; bbcolor=0;
    ptll=lastpt=NULL;
    pnll=lastpn=NULL;
    facell=lastface=NULL;

	//get the current ucs
    rbucs.restype=RT3DPOINT;
    SDS_getvar(NULL,DB_QUCSXDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
    ic_ptcpy(ap1,rb.resval.rpoint);
    SDS_getvar(NULL,DB_QUCSYDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
    ic_crossproduct(ap1,rb.resval.rpoint,pt210);
    //setup the wcs
    rbucs.restype=rbwcs.restype=RTSHORT;
    rbucs.resval.rint=1;
    rbwcs.resval.rint=0;

    /* Set color and layer to the current values: */
    color=blcolor;
    rb.resval.rstring=NULL;
    if (SDS_getvar(NULL,DB_QCECOLOR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)==RTNORM && rb.resval.rstring!=NULL)
        color=ic_colornum(rb.resval.rstring);
    if (rb.resval.rstring!=NULL)
        { IC_FREE(rb.resval.rstring); rb.resval.rstring=NULL; }
    *layer='0'; layer[1]=0;
    if (SDS_getvar(NULL,DB_QCLAYER,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)==RTNORM && rb.resval.rstring!=NULL)
        strncpy(layer,rb.resval.rstring,IC_ACADNMLEN);
    if (rb.resval.rstring!=NULL)
        { IC_FREE(rb.resval.rstring); rb.resval.rstring=NULL; }


    /* Get the points: */
    pn=1;
    for (;;) {
		if (pn==1)
			sprintf(pmpt,ResourceString(IDC_CMDS9__NFIRST_VERTEX_FO_74, "\nFirst vertex for polyface mesh: " ));
		else
			sprintf(pmpt,ResourceString(IDC_CMDS9__NMESH_VERTEX__D__75, "\nMesh vertex %d: " ),pn);
        if ((fi1=sds_getpoint(NULL,pmpt,ap1))==RTCAN) { rc=RTCAN; goto out; }
        if (fi1!=RTNORM) break;        
		if ((tpt= new struct ptlink )==NULL)
            { rc=RTERROR; goto out; }
		memset(tpt,0,sizeof(struct ptlink));
        ic_ptcpy(tpt->pt,ap1);
        if (ptll==NULL) ptll=tpt; else lastpt->next=tpt;
        lastpt=tpt;
        pn++;
    }

    if ((maxpn=pn-1)<1) goto out;


    /* Get the user-defined faces: */
    fn=1;
    for (;;) {  /* Each face */

        pn=1;
        for (;;) {  /* Each point number */
            sds_initget(0,ResourceString(IDC_CMDS9_COLOR_LAYER_76, "Color Layer ~_Color ~_Layer" ));
            sprintf(pmpt,ResourceString(IDC_CMDS9__NCOLOR___LAYER___77, "\nColor/Layer/<Face %d, vertex %d>: " ),fn,pn);
            if ((fi1=sds_getint(pmpt,&fint1))==RTCAN) { rc=RTCAN; goto out; }
            thispn=(short)fint1;
            if (fi1==RTNONE) break;
            if (fi1==RTKWORD) {
                sds_getinput(pmpt);
				if (strisame(pmpt, "COLOR"/*DNT*/)) {
                    do {
                        ok=1;
                        if      (color==blcolor)
                            strncpy(fs1,ic_bylayer,sizeof(fs1)-1);
                        else if (color==bbcolor)
                            strncpy(fs1,ic_byblock,sizeof(fs1)-1);
                        else if (color>0 && color<8)
                            sprintf(fs1,"%d (%s)"/*DNT*/,color,ic_colorstr(color,NULL));
                        else sprintf(fs1,"%d"/*DNT*/,color);
                        sprintf(pmpt,ResourceString(IDC_CMDS9__NNEW_COLOR___S___78, "\nNew color <%s>: " ),fs1);
						sds_initget(SDS_RSG_OTHER, ResourceString(IDC_CMDS9_INITGET_BYLA_55, "BYLAYER|BYLayer BYBLOCK|BYBlock ~ Red Yellow Green Cyan Blue Magenta White ~_BYLayer ~_BYBlock ~_ ~_Red ~_Yellow ~_Green ~_Cyan ~_Blue ~_Magenta ~_White"));
                        if (sds_getkword(pmpt,fs1)==RTCAN)
                            { rc=RTCAN; goto out; }
                        if (!*fs1) { fi1=color; break; }
                        if (*fs1=='-' || *fs1=='+' ||
                            (*fs1>='0' && *fs1<='9')) {

                            if ((fi1=atoi(fs1))<1 || fi1>255) {
                                cmd_ErrorPrompt(CMD_ERR_COLOR1255,0);
                                ok=0;
                            }
                        } else fi1=ic_colornum(fs1);
                    } while (!ok);
                    color=fi1;
                } else {  /* Layer */
                    do {
                        ok=1;
                        sprintf(pmpt,ResourceString(IDC_CMDS9__NNEW_LAYER___S___79, "\nNew layer <%s>: " ),layer);
                        if ((fi1=sds_getstring(0,pmpt,fs1))==RTCAN)
                            { rc=RTCAN; goto out; }
                        if (!*fs1)
                            { strncpy(fs1,layer,sizeof(fs1)-1); break; }
                        if ((elist=sds_tblsearch("LAYER"/*DNT*/,fs1,0))!=NULL) {
                            IC_RELRB(elist);
                        } else {
                            sds_printf(ResourceString(IDC_CMDS9__NCANNOT_FIND_LAY_81, "\nCannot find layer %s." ),fs1);		
                            ok=0;
                        }
                    } while (!ok);
                    strncpy(layer,fs1,sizeof(layer)-1);
                }
                continue;
            }

            if (!thispn) {
				cmd_ErrorPrompt(CMD_ERR_SELECTION,0);// This says "Unable to use that selection.  Please pick again."
				// If that's not close enough, we'll have to add another error message.
                // sds_printf("\nInvalid vertex."); 
				continue;
            } else if (thispn<-maxpn || thispn>maxpn) {
                sds_printf(ResourceString(IDC_CMDS9__NPLEASE_ENTER_AN_82, "\nPlease enter an integer between %d and %d, or an option keyword." ),-maxpn,maxpn);
                continue;
            }

            if ((tpn= new struct ptnumlink )==NULL) { rc=RTERROR; goto out; }
			memset(tpn, 0, sizeof(struct ptnumlink));
            tpn->num=thispn;
            if (pnll==NULL) pnll=tpn; else lastpn->next=tpn;
            lastpn=tpn;

            pn++;

        }  /* End for each point number */

        if (pnll==NULL) break;

        if ((tface= new struct facelink )==NULL) { rc=RTERROR; goto out; }
		memset(tface,0,sizeof(struct facelink));
        tface->color=color;
        if ((tface->layer= new char [strlen(layer)+1])==NULL)
            { rc=RTERROR; goto out; }
        strcpy(tface->layer,layer);
        tface->pnll=pnll; pnll=NULL;
        if (facell==NULL) facell=tface; else lastface->next=tface;
        lastface=tface;

        fn++;

    }  /* End for each face */

    maxfn=fn-1;

    ap1[0]=ap1[1]=ap1[2]=0.0;  /* Used as a dummy below. */


    /* Make the POLYLINE: */
    IC_RELRB(elist);
    if ((elist=sds_buildlist(
        RTDXF0,"POLYLINE"/*DNT*/,
            66,1,
            70,64,
            10,ap1,
            71,maxpn,
            72,maxfn,
           210,pt210,
             0))==NULL) { rc=RTERROR; goto out; }
    if (sds_entmake(elist)!=RTNORM) { rc=RTERROR; goto out; }


    /* Make the point VERTEXs. */
    /* Create one VERTEX elist, find the point link, and keep changing */
    /* the point as we entmake each VERTEX: */
    IC_RELRB(elist);
    if ((elist=sds_buildlist(
        RTDXF0,"VERTEX"/*DNT*/,
            70,192,
            10,ap1,  /* A dummy to create this link. */
             0))==NULL) { rc=RTERROR; goto out; }
    g10=elist;
    for (tpt=ptll; tpt!=NULL; tpt=tpt->next) {
        sds_trans(tpt->pt,&rbucs,&rbwcs,0,g10->rbnext->rbnext->resval.rpoint);
        if (sds_entmake(elist)!=RTNORM) { rc=RTERROR; goto out; }
    }


    /* Make the face VERTEXs: */
    for (tface=facell; tface!=NULL; tface=tface->next) {
        /* For each user-defined face. */

        if (tface->pnll==NULL) continue;  /* No points?  Should never. */

        /* Create one VERTEX elist, find the 71-74 links, and keep changing */
        /* their values as we entmake each VERTEX: */
        IC_RELRB(elist);
		/*D.Gavrilov*/// Why we shouldn't include color if it's the BYLAYER?
		// So, I've commented it out.
/*        if (color==blcolor) {  // Don't include color (62)
            if ((elist=sds_buildlist(
                RTDXF0,"VERTEX",
                     8,tface->layer,
                    70,128,
                    10,ap1,
                    71,0,
                    72,0,
                    73,0,
                    74,0,
                     0))==NULL) { rc=RTERROR; goto out; }
        } else {  // Include color (62)*/
            if ((elist=sds_buildlist(
                RTDXF0,"VERTEX" /*DNT*/,
                     8,tface->layer,
                    62,tface->color,
                    70,128,
                    10,ap1,
                    71,0,
                    72,0,
                    73,0,
                    74,0,
                     0))==NULL) { rc=RTERROR; goto out; }
//        }
        g7x[0]=g7x[1]=g7x[2]=g7x[3]=NULL;
        for (trbp1=elist; trbp1!=NULL; trbp1=trbp1->rbnext)
            if ((fi1=trbp1->restype-71)>-1 && fi1<4) g7x[fi1]=trbp1;
        if (g7x[0]==NULL || g7x[1]==NULL || g7x[2]==NULL || g7x[3]==NULL)
            { rc=RTERROR; goto out; }

        firstnum=-abs(tface->pnll->num);  /* Needed when breaking up */
                                          /* user-defined faces that have */
                                          /* more than 4 points. */

        /* Make each face-def VERTEX for this user-defined face: */
        for (tpn=tface->pnll,fidx=0; tpn!=NULL; fidx++) {

            for (fi1=0; fi1<4; fi1++) {  /* For each 71-74 group. */
                if (fidx<1) {  /* Build 1st face-def VERTEX (up to 4 pts). */
                    if (tpn==NULL) g7x[fi1]->resval.rint=0;
                    else {
                        if (fi1>2) {
                            g7x[fi1]->resval.rint=(tpn->next==NULL) ?
                                tpn->num : -abs(tpn->num);
                            savnum=tpn->num;
                        } else g7x[fi1]->resval.rint=tpn->num;
                    }
                    if (tpn!=NULL) tpn=tpn->next;
                } else {  /* Build a triangular face-def VERTEX */
                    if (fi1>2 || tpn==NULL) g7x[fi1]->resval.rint=0;
                    else if (fi1==0)        g7x[fi1]->resval.rint=firstnum;
                    else if (fi1==1)        g7x[fi1]->resval.rint=savnum;
                    else if (fi1==2) {
                        g7x[fi1]->resval.rint=(tpn->next==NULL) ?
                            tpn->num : -abs(tpn->num);
                        savnum=tpn->num;
                        if (tpn!=NULL) tpn=tpn->next;
                    }
                }
            }

            if (sds_entmake(elist)!=RTNORM) { rc=RTERROR; goto out; }

        }  /* End for each face-def VERTEX. */

    }  /* End for each user-defined face. */


    /* Make the SEQEND: */
    IC_RELRB(elist);
    if ((elist=sds_buildlist(
        RTDXF0,"SEQEND"/*DNT*/,
             0))==NULL) { rc=RTERROR; goto out; }
    if (sds_entmake(elist)!=RTNORM) { rc=RTERROR; goto out; }

out:
    IC_RELRB(elist);
    while (ptll!=NULL) { tpt=ptll->next; delete ptll; ptll=tpt; }
    while (pnll!=NULL) { tpn=pnll->next; delete pnll; pnll=tpn; }
    while (facell!=NULL) {
        if (facell->layer!=NULL) IC_FREE(facell->layer);
        while (facell->pnll!=NULL)
            { tpn=facell->pnll->next; delete facell->pnll; facell->pnll=tpn; }
        tface=facell->next; delete facell; facell=tface;
    }

    return rc;
}

//=====================================================================================

short cmd_3dmesh(void) {

    if (cmd_iswindowopen() == RTERROR) return RTERROR;			

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    char pmpt[41];
    short row,col,ok,fi1;
	RT	rc;	
    short sz[2];  /* 0 : row (M); 1 : col (N) */
    int fint1;
    sds_point ap1;
    struct resbuf *elist,*ptlink,rbucs,rbwcs;

    struct pointlink {
        sds_point pt;
        struct pointlink *next;
    } *ptll,*lastptll,*tptll;

    rc=RTNORM; ptll=lastptll=NULL; elist=NULL;
    ap1[0]=ap1[1]=ap1[2]=0.0;

	rbucs.restype=rbwcs.restype=RTSHORT;
	rbucs.resval.rint=1;
	rbwcs.resval.rint=0;

    /* Get the M and N sizes: */
    for (fi1=0; fi1<2; fi1++) {
        do {
            ok=1;
            if (sds_getint((fi1) ? ResourceString(IDC_CMDS9__NVERTICES_ALONG__84, "\nVertices along secondary mesh axis (between 2 and 256): " ) : ResourceString(IDC_CMDS9__NNUMBER_OF_VERTI_85, "\nNumber of vertices along primary mesh axis (between 2 and 256): " ),
                &fint1)!=RTNORM) { rc=RTCAN; goto out; }
            sz[fi1]=(short)fint1;
            if (sz[fi1]<2 || sz[fi1]>256) {
                ok=0;
				cmd_ErrorPrompt(CMD_ERR_MESHSIZE,0);
            }
        } while (!ok);
    }

    /* Gather the points: */
    for (row=0; row<sz[0]; row++) {
        for (col=0; col<sz[1]; col++) {
            if ((tptll= new struct pointlink )==NULL) { rc=RTERROR; goto out; }
			memset(tptll,0,sizeof(struct pointlink));
            if (ptll==NULL) ptll=tptll; else lastptll->next=tptll;
            lastptll=tptll;
            sprintf(pmpt,ResourceString(IDC_CMDS9__NVERTEX___D___D__86, "\nVertex (%d, %d): " ),(row+1),(col+1));
            sds_initget(1,NULL);
            if (sds_getpoint(NULL,pmpt,lastptll->pt)!=RTNORM)
                { rc=RTCAN; goto out; }
        }
    }

    /* Make the POLYLINE: */
    if ((elist=sds_buildlist(
        RTDXF0,"POLYLINE"/*DNT*/,
            66,1,
            70,16,
            10,ap1,
            71,sz[0],
            72,sz[1],
             0))==NULL) { rc=RTERROR; goto out; }
    if (sds_entmake(elist)!=RTNORM) { rc=RTERROR; goto out; }

    /* Create one VERTEX elist, find the point link, and keep changing */
    /* the point as we entmake each VERTEX: */
    sds_relrb(elist);
    if ((elist=sds_buildlist(
        RTDXF0,"VERTEX"/*DNT*/,
            70,64,
            10,ap1,  /* A dummy to create this link. */
             0))==NULL) { rc=RTERROR; goto out; }
    ptlink=elist;
    while (ptlink!=NULL && ptlink->restype!=10) ptlink=ptlink->rbnext;
    if (ptlink==NULL) { rc=RTERROR; goto out; }
    tptll=ptll;
    for (row=0; row<sz[0]; row++) {
        for (col=0; col<sz[1]; col++) {
			sds_trans(tptll->pt,&rbucs,&rbwcs,0,ptlink->resval.rpoint);
            //ic_ptcpy(ptlink->resval.rpoint,tptll->pt);
            tptll=tptll->next;
            if (sds_entmake(elist)!=RTNORM) { rc=RTERROR; goto out; }
        }
    }

    /* Make the SEQEND: */
    sds_relrb(elist);
    if ((elist=sds_buildlist(
        RTDXF0,"SEQEND"/*DNT*/,
             0))==NULL) { rc=RTERROR; goto out; }
    if (sds_entmake(elist)!=RTNORM) { rc=RTERROR; goto out; }

out:
    while (ptll!=NULL) { tptll=ptll->next; delete ptll; ptll=tptll; }
    if (elist!=NULL) { sds_relrb(elist); elist=NULL; }
    sds_entmake(NULL);  /* Just in case need to cancel one in progress. */
    return rc;
}


//============================================================================================
			
void cmd_prtstr (LPCTSTR fststr, LPCTSTR secstr) {

    if (secstr!=NULL)sds_printf("\n%25s"/*DNT*/,secstr);					
    if (fststr!=NULL)sds_printf("  %s"/*DNT*/,fststr);		
}
//============================================================================================
			
void cmd_prttsty (LPCTSTR fststr, LPCTSTR secstr) {

    sds_name txtstyle;
    sds_resbuf *txtinfo;
    sds_handent(fststr, txtstyle);
    txtinfo = sds_entget(txtstyle);
    while(txtinfo != NULL && txtinfo->restype != 2)
        txtinfo = txtinfo->rbnext;
    ASSERT(txtinfo != NULL);

    if (secstr!=NULL)sds_printf("\n%25s"/*DNT*/,secstr);					
    if (fststr!=NULL)sds_printf("  %s"/*DNT*/,txtinfo->resval.rstring);	
	
	//BugZilla:78448; Added while fixing this bug but is not the cause of the bug [
	IC_RELRB(txtinfo);
	//BugZilla:78448; ]

}
//============================================================================================

void cmd_prtint (int tmpint, LPCTSTR secstr) {

    char ival[IC_ACADBUF];

	itoa(tmpint,ival,10);
	sds_printf("\n%25s"/*DNT*/,secstr);		
	sds_printf("  %s"/*DNT*/,ival);		
}

//============================================================================================

void cmd_prtlong (long tmpint, LPCTSTR secstr) {

    char ival[IC_ACADBUF];

	ltoa(tmpint,ival,10);
	sds_printf("\n%25s"/*DNT*/,secstr);		
	sds_printf("  %s"/*DNT*/,ival);		
}


//============================================================================================

void cmd_prtarea (sds_real tmpreal, LPCTSTR secstr) {
    struct resbuf rb;
    char rval[50];

	if (fabs(tmpreal)<CMD_FUZZ)tmpreal=0.0;  //if very very near 0.0, print 0.0

	SDS_getvar(NULL,DB_QLUNITS,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if (rb.resval.rint==3||rb.resval.rint==4) {
        cmd_rtos_area(tmpreal,-1,-1,rval);
    }else{
        sds_rtos(tmpreal,-1,-1,rval);
    }
	sds_printf("\n%25s"/*DNT*/,secstr);		
	if (rb.resval.rint==3||rb.resval.rint==4) {
        sds_printf(ResourceString(IDC_CMDS9____S_SQ__87, "  %s sq." ),rval);		
	}else{
        sds_printf("  %s"/*DNT*/,rval);		
    }
}


//============================================================================================

void cmd_prtreal (sds_real tmpreal, LPCTSTR secstr) {
    char rval[30];

	if (fabs(tmpreal)<CMD_FUZZ)tmpreal=0.0;  //if very very near 0.0, print 0.0

    sds_rtos(tmpreal,-1,-1,rval);
	sds_printf("\n%25s"/*DNT*/,secstr);		
    sds_printf("  %s"/*DNT*/,rval);		
}


//============================================================================================

void cmd_prtscale (sds_real tmpreal, LPCTSTR secstr) {
    char rval[30];

	if (fabs(tmpreal)<CMD_FUZZ)tmpreal=0.0;  //if very very near 0.0, print 0.0

    sds_rtos(tmpreal,2,-1,rval);
	sds_printf("\n%25s"/*DNT*/,secstr);		
    sds_printf("  %s"/*DNT*/,rval);		
}


//============================================================================================

void cmd_prtangle (sds_real tmpreal, LPCTSTR secstr) {

    char rval[50];

	if (fabs(tmpreal)<CMD_FUZZ)tmpreal=0.0;//angles in specifies range, so
										//tolerance is only 1.0e-10	

	sds_angtos(tmpreal,-1,-1,rval);
	sds_printf("\n%25s"/*DNT*/,secstr);		
	sds_printf("  %s"/*DNT*/,rval);		
}


//============================================================================================

void cmd_prtpt (sds_point tmppt,LPCTSTR secstr) {

    char xval[50],yval[50],zval[50];
	struct resbuf rb;

	SDS_getvar(NULL,DB_QLUNITS,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

	if (rb.resval.rint!=IC_UNITS_SCIENTIFIC) {
		if (fabs(tmppt[0])<CMD_FUZZ)tmppt[0]=0.0;	//if very very near 0.0, print 0.0
		if (fabs(tmppt[1])<CMD_FUZZ)tmppt[1]=0.0;
		if (fabs(tmppt[2])<CMD_FUZZ)tmppt[2]=0.0;
	}
	sds_rtos(tmppt[0],-1,-1,xval);
	sds_rtos(tmppt[1],-1,-1,yval);
	sds_rtos(tmppt[2],-1,-1,zval);
	sds_printf("\n%25s"/*DNT*/,secstr);									
	sds_printf(ResourceString(IDC_CMDS9___X_____S__Y______88, "  X=   %s  Y=   %s  Z=   %s" ),xval,yval,zval);		
}
// Modified Cybage VSB  11/03/2002 DD/MM/YYYY [
// Reference: Bug 78024 from Bugzilla
// Description: DIMCLRD,DIMCLRE,DIMCLRT does not accepts color string like red, blue   
//				and  does not show them as color strings during List command.
/**
// Function name	: cmd_prtcolor
// Description	    : Use to print color string during List command.
// Return type		: void cmd_prtcolor 
// Argument         : int tmpint	: color number
// Argument         : LPCTSTR secstr: coreespoding variable string like Dimclrd, Dimclrt etc..
*/
void cmd_prtcolor (int tmpint, LPCTSTR secstr) {

	char szTemp[IC_ACADBUF];
	if (secstr!=NULL) 
		sds_printf("\n%25s"/*DNT*/,secstr);

	if ( tmpint == 0 || tmpint == 256 )
		sds_printf("  %s"/*DNT*/,strupr(ic_colorstr(tmpint,0)));
	else 
		if ( tmpint  >= 1 && tmpint  <= 7 )
		{
			sprintf(szTemp,"%d (%s)",tmpint,strlwr(ic_colorstr(tmpint,0)));
			sds_printf("  %s"/*DNT*/,szTemp);
		}
		else
		{	
			itoa(tmpint,szTemp,10);
			sds_printf("  %s"/*DNT*/,szTemp);
		}			
}
// Modified Cybage VSB  11/03/2002 DD/MM/YYYY ]
//==============================================================================================

void cmd_prt2dpt (sds_point tmppt, char *secstr) {

    char xval[50],yval[50];
	struct resbuf rb;

	SDS_getvar(NULL,DB_QLUNITS,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

	if (rb.resval.rint!=IC_UNITS_SCIENTIFIC) {
		if (fabs(tmppt[0])<CMD_FUZZ)tmppt[0]=0.0;	//if very very near 0.0, print 0.0
		if (fabs(tmppt[1])<CMD_FUZZ)tmppt[1]=0.0;
	}
	sds_rtos(tmppt[0],-1,-1,xval);
	sds_rtos(tmppt[1],-1,-1,yval);
	sds_printf("\n%25s"/*DNT*/,secstr);									
	sds_printf(ResourceString(IDC_CMDS9___X_____S__Y______89, "  X=   %s  Y=   %s" ),xval,yval);		
}

//==============================================================================================

short cmd_dblist(void) {
    if (cmd_iswindowopen() == RTERROR) return RTERROR;			

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    return(cmd_entlister(1));
}

//==============================================================================================

short cmd_list(void) {
    if (cmd_iswindowopen() == RTERROR) return RTERROR;			
	
	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    return(cmd_entlister(0));
}

//============================================================================================
//Bugzilla No 78004 18-03-2002 
int cmd_print_overrides(sds_name &ent1,int scrollcur, int scrollmax);

short cmd_entlister(short argmode) {
	//NOTE: List and DBList need to set PERIMETER variable.
	static char scroll_on,sort;
	sds_name ssname, ent1,ent2;
	sds_point startpoint,endpoint,cenpoint,tmppt;
	char enttype[IC_ACADBUF],etype[IC_ACADBUF],tmpstr[IC_ACADBUF],tmpstr1[IC_ACADBUF];
	struct resbuf *erb,*erb2,*rbcur,rb,rbucs,rbwcs,rbent,rbfilter,*erbx; 
	short endflag=0,plflag,vtxcnt=0,nverts;
	RT	rc=0;
	int scrollmax,scrollcur,tmpint=0,tmpint2=0,attcnt=0,listidx=0,entidx=1,pl_nct=0,fi1;
	RT	ret;
	long i;
	double degang=0.0,x=0.0,y=0.0,z=0.0;
    sds_real rr,sa,ea,area=0.0,perimeter=0.0,fr1,baseang4ucs,pl_elev;

    erbx=erb2=erb=NULL;

	//set up ucs's for transforming points
    rbucs.rbnext=rbwcs.rbnext=rbent.rbnext=NULL;
	rbucs.restype=rbwcs.restype=RTSHORT;
	rbent.restype=RTENAME;
	rbucs.resval.rint=1;rbwcs.resval.rint=0;

	if (argmode) {
		if (sds_pmtssget(NULL,ResourceString(IDC_CMDS9_X_90, "X" ),NULL,NULL,NULL,ssname,0,true)!=RTNORM) return(RTERROR);
		sort=0;
	} else {
		do{
			if (scroll_on){
				if (sort){
					{	/*D.Gavrilov*/// These and the following {} brackets are for avoiding of
						// repeated definition of strings compiler error in LOADSTRINGS_5 macro.
						LOAD_COMMAND_OPTIONS_5(IDC_CMDS9_PAGINATE_SEQUENTI_91)
						sds_initget(0, LOADEDSTRING);
					}
					{
						LOAD_COMMAND_OPTIONS_5(IDC_CMDS9__PAGINATE_PAGINAT_93)
						ret=sds_pmtssget(ResourceString(IDC_CMDS9__NPAGINATE___SEQU_92, "\nPAginate/Sequential/Tracking/<Select entities to list>: " ), LOADEDSTRING, NULL,NULL,NULL,ssname,0);
					}
				}else{
					{
						LOAD_COMMAND_OPTIONS_5(IDC_CMDS9_PAGINATE_SORT_TRA_94)
						sds_initget(0, LOADEDSTRING);
					}
					{
						LOAD_COMMAND_OPTIONS_5(IDC_CMDS9__PAGINATE_PAGINAT_96)
						ret=sds_pmtssget(ResourceString(IDC_CMDS9__NPAGINATE___SORT_95, "\nPAginate/Sort/Tracking/<Select entities to list>: " ), LOADEDSTRING, NULL,NULL,NULL,ssname,0);
					}
				}
			}else{
				if (sort){
					{
						LOAD_COMMAND_OPTIONS_5(IDC_CMDS9_SCROLL_SEQUENTIAL_97)
						sds_initget(0, LOADEDSTRING);
					}
					{
						LOAD_COMMAND_OPTIONS_5(IDC_CMDS9__SCROLL_SCROLL_SE_99)
						ret=sds_pmtssget(ResourceString(IDC_CMDS9__NSCROLL___SEQUEN_98, "\nSCroll/SEquential/Tracking/<Select entities to list>: " ), LOADEDSTRING, NULL,NULL,NULL,ssname,0);
					}
				}else{
					{
						LOAD_COMMAND_OPTIONS_5(IDC_CMDS9_SCROLL_SORT_TRAC_100)
						sds_initget(0, LOADEDSTRING);
					}
                    {
						LOAD_COMMAND_OPTIONS_5(IDC_CMDS9__SCROLL_SCROLL_S_102)
						ret=sds_pmtssget(ResourceString(IDC_CMDS9__NSCROLL___SORT__101, "\nSCroll/SOrt/Tracking/<Select entities to list>: " ), LOADEDSTRING,NULL,NULL,NULL,ssname,0);
                    }
				}
			}
			if (ret==RTERROR || RTCAN==ret)return(ret);
			if (ret==RTKWORD){
				sds_getinput(tmpstr);
				strupr(tmpstr);
				if (strisame(tmpstr,"TRACKING"/*DNT*/ )){
					SDS_getvar(NULL,DB_QSCRLHIST,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
					sds_initget(RSG_NOZERO+RSG_NONEG,NULL);
					sprintf(tmpstr,ResourceString(IDC_CMDS9__NNUMBER_OF_COMM_104, "\nNumber of command lines to track <%i>: " ),rb.resval.rint);
					ret=sds_getint(tmpstr,&scrollmax);
					if (ret==RTNONE){ret=RTKWORD;continue;}
					if (ret==RTCAN || ret==RTERROR)return(ret);
					if (ret==RTNORM){
						ret=RTKWORD;
						rb.resval.rint=scrollmax;
						sds_setvar("SCRLHIST"/*DNT*/,&rb);
					}
				}else if (strisame(tmpstr,"SCROLL"/*DNT*/ )){
					scroll_on=1;
					sds_printf(ResourceString(IDC_CMDS9__NSCROLL_MODE__107, "\nScroll mode." ));
				}else if (strisame(tmpstr,"PAGINATE"/*DNT*/ )){
					sds_printf(ResourceString(IDC_CMDS9__NPAGINATE_MODE__109, "\nPaginate mode." ));
					scroll_on=0;
				}else if (strisame(tmpstr,"SORT"/*DNT*/ )){
					sds_printf(ResourceString(IDC_CMDS9__NSORT_MODE__111, "\nSort mode." ));
					sort=1;
				}else if (strisame(tmpstr,"SEQUENTIAL"/*DNT*/ )){
					sds_printf(ResourceString(IDC_CMDS9__NSEQUENTIAL_MOD_113, "\nSequential mode." ));
					sort=0;
				}
			}
		}while(ret==RTKWORD);
	}
	if (scroll_on){
		SDS_getvar(NULL,DB_QSCRLHIST,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		scrollmax=rb.resval.rint;
	}
	scrollcur=0;

	//this loop for sorting list by ent type.  If not sorting, break after 1st pass
		//Bugzilla No 78046 25-03-2002 
		for (listidx=0;listidx<=27;listidx++){  
		entidx=0;
		if (!sort && listidx>0) break;
		//This loop is for stepping through the selections in the order selected.  If
		//	doing a dblist, items listed newest first, oldest last
		for (i=0L;RTNORM==sds_ssname(ssname,i,ent1);i++){
            baseang4ucs=0;
			if (sds_usrbrk()) { rc=(RTCAN);goto bail; }
			
			if (erb!=NULL){sds_relrb(erb);erb=NULL;}
			erb=sds_entget(ent1);
			sds_textscr();
			if (!scroll_on){
				SDS_getvar(NULL,DB_QSCRLHIST,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				scrollmax=min(SDS_CMainWindow->m_pTextScrn->m_pPromptWnd->m_pPromptWndList->m_nLinesPerPage,rb.resval.rint);
			    scrollmax-=3;
            } 
			// Modified PK 14/06/2000 [
			//if (erb == NULL)	 {
			//	sds_printf("Null"/*DNT*/);
			//	rc=(RTERROR);
			//	goto bail;
			//}//** End If
			
			if (erb == NULL) // Reason : Fix for bug no.58497
				continue;
			// Modified PK 14/06/2000 ]
			ic_assoc(erb,0);
			if (sort){
				if (!strsame(ic_rbassoc->resval.rstring,"VERTEX"/*DNT*/ ) &&
				   !strsame(ic_rbassoc->resval.rstring,"SEQEND"/*DNT*/ )	&&
				   !strsame(ic_rbassoc->resval.rstring,"ATTRIB"/*DNT*/ )){
					switch (listidx){
						case  0: if (!strisame(ic_rbassoc->resval.rstring,"POINT"/*DNT*/ ))     continue;break;
						case  1: if (!strisame(ic_rbassoc->resval.rstring,db_hitype2str(DB_LINE)))      continue;break;
						case  2: if (!strisame(ic_rbassoc->resval.rstring,"RAY"/*DNT*/ ))       continue;break;
						case  3: if (!strisame(ic_rbassoc->resval.rstring,"XLINE"/*DNT*/ ))     continue;break;
						case  4: if (!strisame(ic_rbassoc->resval.rstring,"ARC"/*DNT*/ ))       continue;break;
						case  5: if (!strisame(ic_rbassoc->resval.rstring,"CIRCLE"/*DNT*/ ))    continue;break;
						case  6: if (!strisame(ic_rbassoc->resval.rstring,"ELLIPSE"/*DNT*/ ))   continue;break;
						case  7: if (!strisame(ic_rbassoc->resval.rstring,"TEXT"/*DNT*/ ))      continue;break;
						case  8: if (!strisame(ic_rbassoc->resval.rstring,"SOLID"/*DNT*/ ))     continue;break;
						case  9: if (!strisame(ic_rbassoc->resval.rstring,"3DFACE"/*DNT*/ ))    continue;break;
						case 10: if (!strisame(ic_rbassoc->resval.rstring,"TRACE"/*DNT*/ ))     continue;break;
						case 11: if (!strisame(ic_rbassoc->resval.rstring,"POLYLINE"/*DNT*/ ))  continue;break;
						case 12: if (!strisame(ic_rbassoc->resval.rstring,"INSERT"/*DNT*/ ))    continue;break;
						case 13: if (!strisame(ic_rbassoc->resval.rstring,"DIMENSION"/*DNT*/ )) continue;break;
						case 14: if (!strisame(ic_rbassoc->resval.rstring,"ATTDEF"/*DNT*/))    continue;break;
						case 15: if (!strisame(ic_rbassoc->resval.rstring,"VIEWPORT"/*DNT*/ ))  continue;break;
						case 16: if (!strisame(ic_rbassoc->resval.rstring,"3DSOLID"/*DNT*/ ))   continue;break;
						case 17: if (!strisame(ic_rbassoc->resval.rstring,"MLINE"/*DNT*/))     continue;break;
						case 18: if (!strisame(ic_rbassoc->resval.rstring,"MTEXT"/*DNT*/ ))     continue;break;
						case 19: if (!strisame(ic_rbassoc->resval.rstring,"REGION"/*DNT*/ ))    continue;break;
						case 20: if (!strisame(ic_rbassoc->resval.rstring,"SPLINE"/*DNT*/ ))    continue;break;
						case 21: if (!strisame(ic_rbassoc->resval.rstring,"BODY"/*DNT*/ ))      continue;break;
						case 22: if (!strisame(ic_rbassoc->resval.rstring,"TOLERANCE"/*DNT*/ )) continue;break;
						case 23: if (!strisame(ic_rbassoc->resval.rstring,"OLE2FRAME"/*DNT*/ )) continue;break;
						case 24: if (!strisame(ic_rbassoc->resval.rstring,"LWPOLYLINE"/*DNT*/ )) continue;break;
						case 25: if (!strisame(ic_rbassoc->resval.rstring,"IMAGE"/*DNT*/ )) continue;break; 
						//Bugzilla No 78004 18-03-2002 
						case 26: if (!strisame(ic_rbassoc->resval.rstring,"LEADER"/*DNT*/ )) continue;break; 
						//Bugzilla No 78046 25-03-2002 
						case 27: if (!strisame(ic_rbassoc->resval.rstring,"HATCH"/*DNT*/ )) continue;break; 
                    }
					entidx++;
				}
			}else{
					 if (strisame(ic_rbassoc->resval.rstring,"POINT"/*DNT*/ ))	 listidx=  0;
				else if (strisame(ic_rbassoc->resval.rstring,db_hitype2str(DB_LINE)))	 listidx=  1;
				else if (strisame(ic_rbassoc->resval.rstring,"RAY"/*DNT*/ ))		 listidx=  2;
				else if (strisame(ic_rbassoc->resval.rstring,"XLINE"/*DNT*/ ))	 listidx=  3;
				else if (strisame(ic_rbassoc->resval.rstring,"ARC"/*DNT*/ ))		 listidx=  4;
				else if (strisame(ic_rbassoc->resval.rstring,"CIRCLE"/*DNT*/ ))	 listidx=  5;
				else if (strisame(ic_rbassoc->resval.rstring,"ELLIPSE"/*DNT*/ ))  listidx=  6;
				else if (strisame(ic_rbassoc->resval.rstring,"TEXT"/*DNT*/ ))	 listidx=  7;
				else if (strisame(ic_rbassoc->resval.rstring,"SOLID"/*DNT*/ ))	 listidx=  8;
				else if (strisame(ic_rbassoc->resval.rstring,"3DFACE"/*DNT*/ ))	 listidx=  9;
				else if (strisame(ic_rbassoc->resval.rstring,"TRACE"/*DNT*/ ))	 listidx= 10;
				else if (strisame(ic_rbassoc->resval.rstring,"POLYLINE"/*DNT*/ )) listidx= 11;
				else if (strisame(ic_rbassoc->resval.rstring,"INSERT"/*DNT*/ ))	 listidx= 12;
				else if (strisame(ic_rbassoc->resval.rstring,"DIMENSION"/*DNT*/ ))listidx= 13;
				else if (strisame(ic_rbassoc->resval.rstring,"ATTDEF"/*DNT*/ ))	 listidx= 14;
				else if (strisame(ic_rbassoc->resval.rstring,"VIEWPORT"/*DNT*/ )) listidx= 15;
				else if (strisame(ic_rbassoc->resval.rstring,"3DSOLID"/*DNT*/ ))  listidx= 16;
				else if (strisame(ic_rbassoc->resval.rstring,"MLINE"/*DNT*/ ))	 listidx= 17;
				else if (strisame(ic_rbassoc->resval.rstring,"MTEXT"/*DNT*/ ))	 listidx= 18;
				else if (strisame(ic_rbassoc->resval.rstring,"REGION"/*DNT*/ ))	 listidx= 19;
				else if (strisame(ic_rbassoc->resval.rstring,"SPLINE"/*DNT*/ ))	 listidx= 20;
				else if (strisame(ic_rbassoc->resval.rstring,"BODY"/*DNT*/ ))	 listidx= 21;
				else if (strisame(ic_rbassoc->resval.rstring,"TOLERANCE"/*DNT*/ ))listidx= 22;
				else if (strisame(ic_rbassoc->resval.rstring,"OLE2FRAME"/*DNT*/ ))listidx= 23;
				else if (strisame(ic_rbassoc->resval.rstring,"LWPOLYLINE"/*DNT*/ ))listidx= 24;
				else if (strisame(ic_rbassoc->resval.rstring,"IMAGE"/*DNT*/ ))listidx= 25; 
				//Bugzilla No 78004 18-03-2002 
				else if (strisame(ic_rbassoc->resval.rstring,"LEADER"/*DNT*/ ))listidx= 26;
				//Bugzilla No 78046 25-03-2002
				else if (strisame(ic_rbassoc->resval.rstring,"HATCH"/*DNT*/ ))listidx= 27;
                else listidx=99;
            }
					//setup resbuf for coord transformations if necessary
			if (!strsame(ic_rbassoc->resval.rstring,"VERTEX"/*DNT*/ ) && !strsame(ic_rbassoc->resval.rstring,"SEQEND"/*DNT*/ )){
					ic_encpy(rbent.resval.rlname,ent1);
			}

    		//This loop is for stepping through the different types of selections.
    		endflag = 0;
    		while (endflag == 0) {
    			if (sds_usrbrk()) { rc=(RTCAN);goto bail; }  //check for break    
    			sds_printf("\n"/*DNT*/);
    			
    			//**  Entity Type	
				if (erb == NULL)   {     
    				sds_printf("Null"/*DNT*/); 
    				rc=(RTERROR);        
    				goto bail;           
    			}//** End If             
    			
				//Expand on the generic name
    			ic_assoc(erb,0); 
    			strncpy(etype,ic_rbassoc->resval.rstring,sizeof(etype)-1);
    			strncpy(enttype,ic_rbassoc->resval.rstring,sizeof(enttype)-1);
    			strlwr(enttype);
				enttype[0]=sds_toupper(enttype[0]);
    			if (stricmp(etype,"3DFACE"/*DNT*/ ) == 0)strcpy(enttype,ResourceString(IDC_CMDS9_3D_FACE_121, "3D Face" ));
    			else if (stricmp(etype,"SEQEND" ) == 0){
					if (vtxcnt>0)strcpy(enttype,ResourceString(IDC_CMDS9_END_POLYLINE_122, "End Polyline" ));
					if (attcnt>0)strcpy(enttype,ResourceString(IDC_CMDS9_END_INSERT_OF_BL_123, "End Insert of Block" ));
				}else if (stricmp(etype,"INSERT"/*DNT*/ ) == 0)strcpy(enttype,ResourceString(IDC_CMDS9_INSERT_OF_BLOCK_124, "Insert of Block" ));
				else if (strisame(etype,"ATTDEF"/*DNT*/ )) strcpy(enttype,ResourceString(IDC_CMDS9_ATTRIBUTE_DEFINI_125, "Attribute Definition" ));
				else if (strisame(etype,"ATTRIB"/*DNT*/ )) strcpy(enttype,ResourceString(IDC_CMDS9_BLOCK_ATTRIBUTE_126, "Block Attribute" ));
				else if (strisame(etype,"VERTEX"/*DNT*/ )) {
					if (plflag&64)strcpy(enttype,ResourceString(IDC_CMDS9_POLYFACE_VERTEX_127, "Polyface Vertex" ));
					else if (plflag&(8+16))strcpy(enttype,ResourceString(IDC_CMDS9_3D_VERTEX_128, "3D Vertex" ));
				}else if (strsame(etype,"DIMENSION"/*DNT*/ )){
					ic_assoc(erb,70);
					pl_nct=ic_rbassoc->resval.rint;
					/*if (pl_nct&32)*/  pl_nct&=~DIM_BIT_6;
					/*if (pl_nct&64)*/  pl_nct&=~DIM_XTYPEORD_DEF;
					/*if (pl_nct&128)*/ pl_nct&=~DIM_TEXT_MOVED;
					switch (pl_nct){
						case DIM_ROT_HOR_VER: ic_assoc(erb,50);
								if (icadAngleEqual(ic_rbassoc->resval.rreal,0.0))
									strcpy(enttype,ResourceString(IDC_CMDS9_HORIZONTAL_DIMEN_129, "Horizontal Dimension" ));
								else if (icadAngleEqual(ic_rbassoc->resval.rreal,IC_PI/2.0))
									strcpy(enttype,ResourceString(IDC_CMDS9_VERTICAL_DIMENSI_130, "Vertical Dimension" ));
								else
									strcpy(enttype,ResourceString(IDC_CMDS9_ROTATED_DIMENSIO_131, "Rotated Dimension" ));
								break;
						case DIM_ALIGNED: strcpy(enttype,ResourceString(IDC_CMDS9_ALIGNED_DIMENSIO_132, "Aligned Dimension" ));
								break;
						case DIM_ANGULAR: strcpy(enttype,ResourceString(IDC_CMDS9_ANGULAR_DIMENSIO_133, "Angular Dimension" ));
								break;
						case DIM_DIAMETER: strcpy(enttype,ResourceString(IDC_CMDS9_DIAMETER_DIMENSI_134, "Diameter Dimension" ));
								break;
						case DIM_RADIUS: strcpy(enttype,ResourceString(IDC_CMDS9_RADIUS_DIMENSION_135, "Radius Dimension" ));
								break;
						case DIM_ANGULAR_3P: strcpy(enttype,ResourceString(IDC_CMDS9_3PT_ANGULAR_DIME_136, "3pt Angular Dimension" ));
								break;
						case DIM_Y_ORDINATE: ic_assoc(erb,70);
					            if (ic_rbassoc->resval.rint&DIM_XTYPEORD_DEF){
								    strcpy(enttype,ResourceString(IDC_CMDS9_ORDINATE_X_DIMEN_137, "Ordinate X Dimension" ));
                                }else{
									strcpy(enttype,ResourceString(IDC_CMDS9_ORDINATE_Y_DIMEN_138, "Ordinate Y Dimension" ));
								}
                                break;
					}
				}
				//print line stating entity name and number, if appropriate
				if (strsame(etype,"SEQEND"/*DNT*/ )){
					sprintf(tmpstr,ResourceString(IDC_CMDS9__N________S_____139, "\n------ %s    " ),enttype);
				}else if (strsame(etype,"VERTEX"/*DNT*/ )){
					vtxcnt++;
					if (plflag & 16){
						if (pl_nct>0)sprintf(tmpstr,ResourceString(IDC_CMDS9__N__________MESH_140, "\n--------- Mesh Vertex (%i,%i) " ),(int)(vtxcnt-1)/pl_nct,((vtxcnt-1)%pl_nct));
						else sprintf(tmpstr,ResourceString(IDC_CMDS9__N_____________M_141, "\n------------ Mesh Vertex " ));
					}else if (plflag & 64){
						sprintf(tmpstr,ResourceString(IDC_CMDS9__N__________POLY_142, "\n----------Polyface Vertex " ));
					}else if (1==vtxcnt){
						if (plflag&1)sprintf(tmpstr,ResourceString(IDC_CMDS9__N____________S__143, "\n---------- %s %i (Start/End) " ),enttype,vtxcnt);
						else sprintf(tmpstr,ResourceString(IDC_CMDS9__N____________S__144, "\n---------- %s %i (Start) " ),enttype,vtxcnt);
					}else if (vtxcnt==nverts && 0==(plflag&1)){
						sprintf(tmpstr,ResourceString(IDC_CMDS9__N____________S__145, "\n---------- %s %i (End) " ),enttype,vtxcnt); 
					}else{
						sprintf(tmpstr,ResourceString(IDC_CMDS9__N____________S__146, "\n---------- %s %i " ),enttype,vtxcnt);
					}

				}else if (strsame(etype,"ATTRIB"/*DNT*/ )){
					attcnt++;
					sprintf(tmpstr,ResourceString(IDC_CMDS9__N____________S__146, "\n---------- %s %i " ),enttype,attcnt);
				}else{
					if (sort)
						sprintf(tmpstr,ResourceString(IDC_CMDS9__N________S__I__147, "\n------ %s %i " ),enttype,entidx);
					else
						sprintf(tmpstr,ResourceString(IDC_CMDS9__N____________S__148, "\n---------- %s " ),enttype);
				}
				while(strlen(tmpstr)<68){strcat(tmpstr,ResourceString(IDC_CMDS9___149, "-" ));}
    			sds_printf(tmpstr);
				if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
				for (rbcur=erb;rbcur!=NULL;rbcur=rbcur->rbnext){
    				//**Layer
    				if (0==vtxcnt && 8==rbcur->restype) {
    					cmd_prtstr(rbcur->resval.rstring,ResourceString(IDC_CMDS9_LAYER__150, "Layer:" ));
						if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    				}
     				//**Color
    				if (0==vtxcnt && 62==rbcur->restype) {
    					ic_colorstr(rbcur->resval.rint,tmpstr);
    					if ((rbcur->resval.rint>=0&&rbcur->resval.rint<=7)||rbcur->resval.rint==256){
                            sprintf(tmpstr1,"%d (%s)"/*DNT*/,rbcur->resval.rint,tmpstr);
                            cmd_prtstr(tmpstr1,ResourceString(IDC_CMDS9_COLOR__151, "Color:" ));
                        }else{
                            cmd_prtstr(tmpstr,ResourceString(IDC_CMDS9_COLOR__151, "Color:" ));
                        }
						if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    				}
    				//**LineType
    				if (0==vtxcnt && 6==rbcur->restype) {
    					cmd_prtstr(rbcur->resval.rstring,ResourceString(IDC_CMDS9_LINETYPE__152, "Linetype:" ));
						if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    				}
    				//**Entity Handle
    				if (5==rbcur->restype) {
    					cmd_prtstr(rbcur->resval.rstring,ResourceString(IDC_CMDS9_HANDLE__153, "Handle:" ));
						if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    				}
    				//**Text Style
    				if (7==rbcur->restype) {
    					cmd_prtstr(rbcur->resval.rstring,ResourceString(IDC_CMDS9_TEXT_STYLE__154, "Text style:" ));
						if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    				}
					//Bugzilla No 78004 18-03-2002 
					if (3==rbcur->restype && (13==listidx || 26==listidx)) {                                 
						cmd_prtstr(rbcur->resval.rstring,ResourceString(IDC_CMDS9_DIMENSION_STYLE__155, "Dimension style:" ));     
						if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
					}                                                        
					//**Paperspace Modelspace
    				if (0==vtxcnt && 67==rbcur->restype) {
						cmd_prtstr(rbcur->resval.rint ? ResourceString(IDC_CMDS9_PAPER_156, "Paper" ) : ResourceString(IDC_CMDS9_MODEL_157, "Model" ),ResourceString(IDC_CMDS9_CURRENT_SPACE__158, "Current space:" ));
						if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    				}
					// EBATECH(CNBR) -[ 24-03-2003 Bugzilla#78471 Support Lineweight
    				//** Lineweight
    				if (0==vtxcnt && 370==rbcur->restype) {
    					cmd_prtint(rbcur->resval.rint,ResourceString(IDC_CMDS9_LWEIGHT_175, "Lineweight:"));
						if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    				}
    				// EBATECH(CNBR) ]-
				}
    			//**Thickness	
    			if (0==vtxcnt && ic_assoc(erb,39)==0 && 
					!icadRealEqual(ic_rbassoc->resval.rreal,0.0)) {
    				cmd_prtreal(ic_rbassoc->resval.rreal,ResourceString(IDC_CMDS9_THICKNESS__159, "Thickness:" ));
					if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    			}
				if (listidx>1 || (listidx<2 && ic_rbassoc->resval.rreal>0.0)){
					//for all ents w/extru or pt & line with thickness...
					if (0==ic_assoc(erb,210)){
						sds_trans(ic_rbassoc->resval.rpoint,&rbwcs,&rbucs,1,ic_rbassoc->resval.rpoint);
						if (!icadRealEqual(1.0,ic_rbassoc->resval.rpoint[2])){
							cmd_prtpt(ic_rbassoc->resval.rpoint,ResourceString(IDC_CMDS9_EXTRUSION_RELATI_160, "Extrusion relative to UCS:" ));
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
						}
					}
				}
    			
    			//======These are ent specific types======//
    			
				switch (listidx){
				case 99:
                    endflag=1;
                    break;
                	//if ent is a dimension
				case 13: {
					fi1=0;
                    for (rbcur=erb;rbcur!=NULL;rbcur=rbcur->rbnext){
						//Note: style name (group 3) at top of f'n
						if (50==rbcur->restype && strsame(enttype,ResourceString(IDC_CMDS9_ROTATED_DIMENSIO_131, "Rotated Dimension" ))){
							cmd_prtangle(rbcur->resval.rreal,ResourceString(IDC_CMDS9_DIMENSION_ANGLE__161, "Dimension angle:" ));
						}
    					if (11==rbcur->restype) {
							sds_trans(rbcur->resval.rpoint,&rbent,&rbucs,0,startpoint);
    						cmd_prtpt(startpoint,ResourceString(IDC_CMDS9_DIM_TEXT_POINT__162, "Dim Text point:" ));
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    					}
						if (pl_nct<=2 || pl_nct==5){
							//linear or angular dim
    						if (50==rbcur->restype && !icadAngleEqual(rbcur->resval.rreal,0.0) &&
								!icadAngleEqual(rbcur->resval.rreal,IC_PI)){
    							cmd_prtreal(rbcur->resval.rreal,ResourceString(IDC_CMDS9_ROTATION_ANGLE__163, "Rotation Angle:" ));
								if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    						}
    						if (10==rbcur->restype) {
                                if (pl_nct==5) {
								    sds_trans(rbcur->resval.rpoint,&rbwcs,&rbucs,0,startpoint);
    							    cmd_prtpt(startpoint,ResourceString(IDC_CMDS9_1ST_DIM_DEFINITI_164, "1st Dim definition point:" ));
								    if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
								}else{
                                    sds_trans(rbcur->resval.rpoint,&rbwcs,&rbucs,0,startpoint);
    							    cmd_prtpt(startpoint,ResourceString(IDC_CMDS9_DIM_DEFINITION_P_165, "Dim definition point:" ));
								    if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    						    }
                            }
    						if (13==rbcur->restype) {
								sds_trans(rbcur->resval.rpoint,&rbwcs,&rbucs,0,startpoint);
    							cmd_prtpt(startpoint,ResourceString(IDC_CMDS9_2ND_DIM_DEFINITI_166, "2nd Dim definition point:" ));
								if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    						}
    						if (14==rbcur->restype) {
								sds_trans(rbcur->resval.rpoint,&rbwcs,&rbucs,0,startpoint);
    							cmd_prtpt(startpoint,ResourceString(IDC_CMDS9_1ST_DIM_DEFINITI_164, "1st Dim definition point:" ));
								if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    						}
    						if (15==rbcur->restype){
                                if (pl_nct==5) {
								    sds_trans(rbcur->resval.rpoint,&rbwcs,&rbucs,0,startpoint);
    							    cmd_prtpt(startpoint,ResourceString(IDC_CMDS9_DIM_ARC_CENTER_P_167, "Dim Arc center point:" ));
								    if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    						    }else if (pl_nct==2){
								    sds_trans(rbcur->resval.rpoint,&rbwcs,&rbucs,0,startpoint);
    							    cmd_prtpt(startpoint,ResourceString(IDC_CMDS9_2ND_DIM_DEFINITI_166, "2nd Dim definition point:" ));
								    if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    						    }else if (pl_nct == 3 || pl_nct == 4){
								    sds_trans(rbcur->resval.rpoint,&rbwcs,&rbucs,0,startpoint);
    							    cmd_prtpt(startpoint,ResourceString(IDC_CMDS9_DIM_DEFINITION_P_165, "Dim definition point:" ));
								    if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    						    }
						    }
                        }else if (pl_nct==3 || pl_nct==4){
							//radius or diameter
    						if (10==rbcur->restype) {
								sds_trans(rbcur->resval.rpoint,&rbwcs,&rbucs,0,startpoint);
								if (pl_nct==3)
    								cmd_prtpt(startpoint,ResourceString(IDC_CMDS9_1ST_DIM_DEFINITI_164, "1st Dim definition point:" ));
								else
    								cmd_prtpt(startpoint,ResourceString(IDC_CMDS9_DIM_CENTER_POINT_168, "Dim Center point:" ));
								if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    						}
    						if (15==rbcur->restype) {
								sds_trans(rbcur->resval.rpoint,&rbwcs,&rbucs,0,startpoint);
								if (pl_nct==3)
    								cmd_prtpt(startpoint,ResourceString(IDC_CMDS9_2ND_DIM_DEFINITI_166, "2nd Dim definition point:" ));
								else
    								cmd_prtpt(startpoint,ResourceString(IDC_CMDS9_DIM_DEFINITION_P_165, "Dim definition point:" ));
								if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    						}
						}else if (pl_nct==6){
							//ordinate dim
    						if (13==rbcur->restype) {
								sds_trans(rbcur->resval.rpoint,&rbwcs,&rbucs,0,startpoint);
    							cmd_prtpt(startpoint,ResourceString(IDC_CMDS9_FEATURE_DEFINITI_169, "Feature definition point:" ));
								if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    						}
    						if (14==rbcur->restype) {
								sds_trans(rbcur->resval.rpoint,&rbwcs,&rbucs,0,startpoint);
    							cmd_prtpt(startpoint,ResourceString(IDC_CMDS9_LEADER_DEFINITIO_170, "Leader definition point:" ));
								if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    						}
						}
						if (1==rbcur->restype && *rbcur->resval.rstring){
							fi1=1;
							cmd_prtstr(rbcur->resval.rstring,ResourceString(IDC_CMDS9_USER_INPUT_TEXT__171, "User input text:" ));
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
						}
							
						if (53==rbcur->restype && !icadAngleEqual(rbcur->resval.rreal,0.0)){
							cmd_prtangle(rbcur->resval.rreal,ResourceString(IDC_CMDS9_TEXT_ROTATION_AN_172, "Text rotation angle:" ));
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
						}
							
					}
					if (0==fi1){
						cmd_prtstr(ResourceString(IDC_CMDS9_AS_MEASURED_173, "As measured" ),ResourceString(IDC_CMDS9_TEXT__174, "Text:" ));
						if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
					}
					//Bugzilla No 78004 18-03-2002 [
					//The code from here is moved inside function cmd_print_overrides as it needs to be 
					//called from multiple places
					rc=cmd_print_overrides(ent1,scrollcur,scrollmax);           
					if(rc!=RTNORM) goto bail; 
					//Bugzilla No 78004 18-03-2002 ]
                    endflag=1;
                    break;
					}
    			//If ent is a line
				case 1: {	
     				//**Start Point
    				if (ic_assoc(erb,10)==0) {
						sds_trans(ic_rbassoc->resval.rpoint,&rbwcs,&rbucs,0,startpoint);
    					cmd_prtpt(startpoint,ResourceString(IDC_CMDS9_FROM_POINT__233, "From point:" ));
						if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    				}
     				//**End Point
    				if (ic_assoc(erb,11)==0) {
						sds_trans(ic_rbassoc->resval.rpoint,&rbwcs,&rbucs,0,endpoint);
    					cmd_prtpt(endpoint,ResourceString(IDC_CMDS9_TO_POINT__234, "To point:" ));
						if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    				}
      				//** Line Length
      				cmd_prtreal(sds_distance(startpoint,endpoint),ResourceString(IDC_CMDS9_LENGTH__235, "Length:" ));
					if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    				//** Calculate & display angle
    				degang=(sds_angle(startpoint,endpoint));
    				cmd_prtangle(degang,ResourceString(IDC_CMDS9_ANGLE_IN_UCS_XY__236, "Angle in UCS XY plane:" ));
					if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    				//** Calculate & Display Delta
    				tmppt[0]=endpoint[0]-startpoint[0];  
    				tmppt[1]=endpoint[1]-startpoint[1];
    				tmppt[2]=endpoint[2]-startpoint[2];
					if (fabs(tmppt[2])>CMD_FUZZ && (fabs(tmppt[0])>CMD_FUZZ || fabs(tmppt[1])>CMD_FUZZ)){
						degang=atan(tmppt[2]/sqrt(tmppt[0]*tmppt[0]+tmppt[1]*tmppt[1]));
    					cmd_prtangle(degang,ResourceString(IDC_CMDS9_ANGLE_FROM_UCS_X_237, "Angle from UCS XY plane:" ));
						if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
					}
     				cmd_prtpt(tmppt,ResourceString(IDC_CMDS9_DELTA_VALUES__238, "Delta values:" ));
					if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    				endflag=1;
					break;
				}
    			// If ent is a 3DFace
				case 9: {
					for (rbcur=erb;rbcur!=NULL;rbcur=rbcur->rbnext){
     					//**First Corner
    					if (10==rbcur->restype) {
							sds_trans(rbcur->resval.rpoint,&rbwcs,&rbucs,0,tmppt);
    						cmd_prtpt(tmppt,ResourceString(IDC_CMDS9_FIRST_CORNER__239, "First corner:" ));
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    					}
    	 				//**Second Corner
    					if (11==rbcur->restype) {
 							sds_trans(rbcur->resval.rpoint,&rbwcs,&rbucs,0,tmppt);
   							cmd_prtpt(tmppt,ResourceString(IDC_CMDS9_SECOND_CORNER__240, "Second corner:" ));
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    					}
     					//**Third Corner
    					if (12==rbcur->restype) {
							ic_ptcpy(startpoint,rbcur->resval.rpoint);
							sds_trans(rbcur->resval.rpoint,&rbwcs,&rbucs,0,tmppt);
    						cmd_prtpt(tmppt,ResourceString(IDC_CMDS9_THIRD_CORNER__241, "Third corner:" ));
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    					}
     					//**Fourth Corner
    					if (13==rbcur->restype) {
    						if ((icadPointEqual(startpoint,rbcur->resval.rpoint))==0) {
								sds_trans(rbcur->resval.rpoint,&rbwcs,&rbucs,0,tmppt);
    							cmd_prtpt(tmppt,ResourceString(IDC_CMDS9_FOURTH_CORNER__242, "Fourth corner:" ));
								if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    						}
    					}
					}
					sa=ea=0.0;
					if (db_edata(ent1,NULL,NULL,&sa,&ea)==0){
						if (sa>0.0){
							cmd_prtreal(sa,ResourceString(IDC_CMDS9_PERIMETER__243, "Perimeter:" ));
							perimeter=sa;
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
						}
						if (ea>0.0){
							cmd_prtarea(ea,ResourceString(IDC_CMDS9_AREA__244, "Area:" ));
							area=ea;
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
						}
					}
    				endflag=1;
					break;
				}
    			//If ent is a Arc
				case 4: {
					for (rbcur=erb;rbcur!=NULL;rbcur=rbcur->rbnext){
    					//** Radius
    					if (40==rbcur->restype) {
    						rr=rbcur->resval.rreal;
    						cmd_prtreal(rr,ResourceString(IDC_CMDS9_RADIUS__245, "Radius:" ));
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    					}
    					//** Center Point
    					if (10==rbcur->restype) {
							ic_ptcpy(cenpoint,rbcur->resval.rpoint);
							sds_trans(cenpoint,&rbent,&rbucs,0,tmppt);
    						cmd_prtpt(tmppt,ResourceString(IDC_CMDS9_CENTER_POINT__246, "Center Point:" ));
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    					}
    					//** Starting Point                                                 
    					if (50==rbcur->restype)sa=rbcur->resval.rreal;
    					if (51==rbcur->restype)ea=rbcur->resval.rreal;
					}
    				sds_polar(cenpoint,sa,rr,tmppt);
					sds_trans(tmppt,&rbent,&rbucs,0,tmppt);
    				cmd_prtpt(tmppt,ResourceString(IDC_CMDS9_START_POINT__247, "Start Point:" ));         
					if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    				//** Ending Point
					sds_polar(cenpoint,ea,rr,tmppt);         
					sds_trans(tmppt,&rbent,&rbucs,0,tmppt);
    				cmd_prtpt(tmppt,ResourceString(IDC_CMDS9_END_POINT__248, "End Point:" ));
					if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
					ic_normang(&sa,&ea);
					fr1=(ea-sa)*rr;
					//**Arc Length
					cmd_prtreal(fr1,ResourceString(IDC_CMDS9_ARC_LENGTH__249, "Arc Length:" ));
					if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
					//Now we need to display start and end angles.  If we're not 
					//looking along extrusion vector of arc, just list included angle
					//Get extru vector...
					if (!ic_assoc(erb,210)){
						ic_ptcpy(tmppt,ic_rbassoc->resval.rpoint);
						fr1=tmppt[0]*tmppt[0]+tmppt[1]*tmppt[1]+tmppt[2]*tmppt[2];
						if (fabs(fr1-1.0) > 2.0*CMD_FUZZ){  //approx (1+CMD_FUZZ) squared...
							tmppt[0]/=fr1;
							tmppt[1]/=fr1;
							tmppt[2]/=fr1;
						}
					}else{
						tmppt[0]=tmppt[1]=0.0;
						tmppt[2]=1.0;
					}
					//get ucs's Zdir
					SDS_getvar(NULL,DB_QUCSXDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
					ic_ptcpy(cenpoint,rb.resval.rpoint);
					SDS_getvar(NULL,DB_QUCSYDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
					startpoint[0]=cenpoint[1]*rb.resval.rpoint[2]-cenpoint[2]*rb.resval.rpoint[1];
					startpoint[1]=-cenpoint[0]*rb.resval.rpoint[2]+cenpoint[2]*rb.resval.rpoint[0];
					startpoint[2]=cenpoint[0]*rb.resval.rpoint[1]-cenpoint[1]*rb.resval.rpoint[0];
					
					if (icadPointEqual(startpoint,tmppt)){
						//if the arc's extrusion dir is same as the current ucs...
						sds_trans(cenpoint,&rbwcs,&rbucs,1,startpoint);
						fr1=atan2(startpoint[1],startpoint[0]);
						sa-=fr1;
						ea-=fr1;
						ic_normang(&sa,NULL);
						ic_normang(&ea,NULL);
    					//** Start Angle
    					cmd_prtangle(sa,ResourceString(IDC_CMDS9_START_ANGLE__250, "Start angle:" ));
						if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    					//** End Angle
						ic_normang(&ea,NULL);//reset end angle
    					cmd_prtangle(ea,ResourceString(IDC_CMDS9_END_ANGLE__251, "End angle:" ));
						if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
					}else{
						cmd_prtstr(ResourceString(IDC_CMDS9_ARC_NOT_PARALLEL_252, "Arc not parallel to UCS XY plane." ),ResourceString(IDC_CMDS9_NOTE__253, "Note:" ));
						if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
						ic_normang(&sa,&ea);
						cmd_prtangle(ea-sa,ResourceString(IDC_CMDS9_INCLUDED_ANGLE__254, "Included angle:" ));
						if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
					}
    				endflag=1;
					break;
				}
     			//If ent is a Circle
				case 5: {
    				if (ic_assoc(erb,10)==0) {
						sds_trans(ic_rbassoc->resval.rpoint,&rbent,&rbucs,0,cenpoint);
						cmd_prtpt(cenpoint,ResourceString(IDC_CMDS9_CENTER_POINT__255, "Center point:" ));
						if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
					}
    				if (ic_assoc(erb,40)==0) {
    					cmd_prtreal(ic_rbassoc->resval.rreal,ResourceString(IDC_CMDS9_RADIUS__245, "Radius:" ));
						if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    					cmd_prtreal((IC_TWOPI*ic_rbassoc->resval.rreal),ResourceString(IDC_CMDS9_CIRCUMFERENCE__256, "Circumference:" ));
						if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    					cmd_prtarea((IC_PI*ic_rbassoc->resval.rreal*ic_rbassoc->resval.rreal),ResourceString(IDC_CMDS9_AREA__244, "Area:" ));
						if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
						perimeter=IC_TWOPI*ic_rbassoc->resval.rreal;
						area=IC_PI*ic_rbassoc->resval.rreal*ic_rbassoc->resval.rreal;
    				}
    				endflag=1;
					break;
				}
				//I ent is an ellipse...
				case 6: {
                    sds_point minor, ext;
					for (rbcur=erb;rbcur!=NULL;rbcur=rbcur->rbnext){
						if (10==rbcur->restype)ic_ptcpy(cenpoint,rbcur->resval.rpoint);
						if (11==rbcur->restype)ic_ptcpy(startpoint,rbcur->resval.rpoint);
						if (40==rbcur->restype)rr=rbcur->resval.rreal;
						if (41==rbcur->restype)sa=rbcur->resval.rreal;
						if (42==rbcur->restype)ea=rbcur->resval.rreal;
                        if (210==rbcur->restype)ic_ptcpy(ext,rbcur->resval.rpoint);
					}
					sds_trans(cenpoint,&rbwcs,&rbucs,0,tmppt);
					cmd_prtpt(tmppt,ResourceString(IDC_CMDS9_CENTER_POINT__255, "Center point:" ));
					if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
					sds_trans(startpoint,&rbwcs,&rbucs,1,tmppt);
					cmd_prtpt(tmppt,ResourceString(IDC_CMDS9_MAJOR_AXIS_VECTO_257, "Major Axis vector:" ));
					if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;

					endpoint[0]=rr*startpoint[0];
					endpoint[1]=rr*startpoint[1];
					endpoint[2]=rr*startpoint[2];
                    sds_trans(ext,&rbucs,&rbwcs, 1,ext);
                    ic_crossproduct(ext, endpoint, minor);
                    sds_trans(minor,&rbwcs,&rbucs,1,tmppt);

					cmd_prtpt(tmppt,ResourceString(IDC_CMDS9_MINOR_AXIS_VECTO_258, "Minor Axis vector:" ));
					if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
					cmd_prtreal(rr,ResourceString(IDC_CMDS9_MAJOR_MINOR_AXIS_259, "Major/Minor axis ratio:" ));
					if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
					//let fr1 be semimajor axis length
					fr1=sqrt(startpoint[0]*startpoint[0]+startpoint[1]*startpoint[1]+startpoint[2]*startpoint[2]);
					ic_angpar(sa,&sa,fr1,(fr1*rr),1);
					ic_angpar(ea,&ea,fr1,(fr1*rr),1);
					
					if(fabs(sa) > IC_ZRO || (fabs(ea - IC_TWOPI) > IC_ZRO && fabs(sa) > IC_ZRO))
					{
						//get start and end points
						tmppt[0]=cenpoint[0]+startpoint[0]*cos(sa)+endpoint[0]*sin(sa);
						tmppt[1]=cenpoint[1]+startpoint[1]*cos(sa)+endpoint[1]*sin(sa);
						tmppt[2]=cenpoint[2]+startpoint[2]*cos(sa)+endpoint[2]*sin(sa);
						sds_trans(tmppt,&rbwcs,&rbucs,0,tmppt);
						cmd_prtpt(tmppt,ResourceString(IDC_CMDS9_START_POINT__247, "Start Point:" ));
						if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
						tmppt[0]=cenpoint[0]+startpoint[0]*cos(ea)+endpoint[0]*sin(ea);
						tmppt[1]=cenpoint[1]+startpoint[1]*cos(ea)+endpoint[1]*sin(ea);
						tmppt[2]=cenpoint[2]+startpoint[2]*cos(ea)+endpoint[2]*sin(ea);
						sds_trans(tmppt,&rbwcs,&rbucs,0,tmppt);
						cmd_prtpt(tmppt,ResourceString(IDC_CMDS9_END_POINT__248, "End Point:" ));
						if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
						cmd_prtangle(sa,ResourceString(IDC_CMDS9_START_ANGLE__261, "Start Angle:" ));
						if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
						cmd_prtangle(ea,ResourceString(IDC_CMDS9_END_ANGLE__262, "End Angle:" ));
						if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
					}

					/*DG - 21.11.2001*/// Calculate and output area and length of ellipse.
					area = 0.0;
					double	len = 0.0;
					if(gr_get_ellipse_edata(erb, &len, &area))
					{
						rc = RTERROR;
						goto bail;
					}
					if(area)
					{
    					cmd_prtarea(area, ResourceString(IDC_CMDS9_AREA__244, "Area:"));
						if(RTNORM != (rc = cmd_entlister_scrollchk(&scrollcur, scrollmax)))
							goto bail;
					}
					cmd_prtreal(len, ResourceString(IDC_CMDS9_ELLIPTICAL_ARC_L_260, "Elliptical Arc Length:"));
					if(RTNORM != (rc = cmd_entlister_scrollchk(&scrollcur, scrollmax)))
						goto bail;

    				endflag=1;
					break;
				}
    			//If ent is a Polyline or its subent...
				case 11: 
					{
					if (strsame(etype,"POLYLINE"/*DNT*/ ))
						{
						plflag=0;
						if ((ic_assoc(erb,70))==0)
							{
							plflag=ic_rbassoc->resval.rint;
							if (plflag & IC_PLINE_3DMESH)
								{
								if (ic_rbassoc->resval.rint & IC_PLINE_CLOSED) 
									{
									cmd_prtstr(ResourceString(IDC_CMDS9_CLOSED_IN_THE_M__263, "Closed in the M direction" ),ResourceString(IDC_CMDS9_POLYLINE_FLAGS__264, "Polyline Flags:" ));                   
									if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
									}
								else
									{
									cmd_prtstr(ResourceString(IDC_CMDS9_OPEN_IN_THE_M_DI_265, "Open in the M direction" ),ResourceString(IDC_CMDS9_POLYLINE_FLAGS__264, "Polyline Flags:" ));                   
									if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
									}
								}
							else
								{
								if (ic_rbassoc->resval.rint & IC_PLINE_CLOSED) 
									{
									cmd_prtstr(ResourceString(IDC_CMDS9_CLOSED_266, "Closed" ),ResourceString(IDC_CMDS9_POLYLINE_FLAGS__264, "Polyline Flags:" ));                   
									if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
									}
								else
									{
									cmd_prtstr(ResourceString(IDC_CMDS9_OPEN_267, "Open" ),ResourceString(IDC_CMDS9_POLYLINE_FLAGS__264, "Polyline Flags:" ));                   
									if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
									}
								}
							if (plflag & IC_PLINE_CURVEFIT) 
								{
								cmd_prtstr(ResourceString(IDC_CMDS9_CURVE_FIT_VERTIC_268, "Curve fit vertices have been added" ),ResourceString(IDC_CMDS9_POLYLINE_FLAGS__264, "Polyline Flags:" ));                    
								if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
								}
							if (plflag & IC_PLINE_SPLINEFIT) 
								{
								cmd_prtstr(ResourceString(IDC_CMDS9_SPLINE_FIT_VERTI_269, "Spline fit vertices have been added" ),ResourceString(IDC_CMDS9_POLYLINE_FLAGS__264, "Polyline Flags:" ));       
								if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
								}
							if (plflag & IC_PLINE_3DPLINE) 
								{
								cmd_prtstr(ResourceString(IDC_CMDS9_3D_POLYLINE_270, "3D Polyline" ),ResourceString(IDC_CMDS9_POLYLINE_FLAGS__264, "Polyline Flags:" ));        
								if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
								}
							if (plflag & IC_PLINE_3DMESH) 
								{
								cmd_prtstr(ResourceString(IDC_CMDS9_3D_POLYLINE_MESH_271, "3D Polyline mesh" ),ResourceString(IDC_CMDS9_POLYLINE_FLAGS__264, "Polyline Flags:" ));     
								if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
								if (0==ic_assoc(erb,72))pl_nct=ic_rbassoc->resval.rint;
								}
							if (plflag & IC_PLINE_CLOSEDN) 
								{
								cmd_prtstr(ResourceString(IDC_CMDS9_CLOSED_IN_THE_N__272, "Closed in the N direction" ),ResourceString(IDC_CMDS9_POLYLINE_FLAGS__264, "Polyline Flags:" ));
								if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
								}
							else if (IC_PLINE_3DMESH==(plflag & (IC_PLINE_3DMESH | IC_PLINE_CLOSEDN)))
								{
								cmd_prtstr(ResourceString(IDC_CMDS9_OPEN_IN_THE_N_DI_273, "Open in the N direction" ),ResourceString(IDC_CMDS9_POLYLINE_FLAGS__264, "Polyline Flags:" ));
								if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
								}
							if (plflag & IC_PLINE_POLYFACEMESH) 
								{
								cmd_prtstr(ResourceString(IDC_CMDS9_POLYFACE_MESH_274, "Polyface mesh" ),ResourceString(IDC_CMDS9_POLYLINE_FLAGS__264, "Polyline Flags:" ));
								if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
								}
							if (plflag & IC_PLINE_CONTINUELT) 
								{
								cmd_prtstr(ResourceString(IDC_CMDS9_CONTINUOUSLY_DIS_275, "Continuously displayed linetype" ),ResourceString(IDC_CMDS9_POLYLINE_FLAGS__264, "Polyline Flags:" ));     
								if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
								} 
							}
						if (0==(plflag & (IC_PLINE_3DPLINE | IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH)))
							{
							for (rbcur=erb;rbcur!=NULL;rbcur=rbcur->rbnext)
								{
								if (rbcur->restype==40)
									{
    								//Starting width
    								cmd_prtreal(rbcur->resval.rreal,ResourceString(IDC_CMDS9_DEFAULT_STARTING_276, "Default Starting width:" ));
									if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))
										goto bail;
									}
								else if (rbcur->restype==41)
									{
    								//Ending width
    								cmd_prtreal(rbcur->resval.rreal,ResourceString(IDC_CMDS9_DEFAULT_ENDING_W_277, "Default Ending width:" ));
									if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))
										goto bail;
									}
								else if (rbcur->restype==10)
									{
									if (0==(plflag&(IC_PLINE_3DPLINE | IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH)))pl_elev=rbcur->resval.rpoint[2];
									sds_trans(rbcur->resval.rpoint,&rbent,&rbucs,0,tmppt);
									}
								else if (rbcur->restype==75)
									{
									if (plflag & IC_PLINE_SPLINEFIT)
										{
										if (rbcur->resval.rint==6)
											cmd_prtstr(ResourceString(IDC_CMDS9_CUBIC_278, "Cubic" ),ResourceString(IDC_CMDS9_FIT_TYPE__279, "Fit type:" ));
										else if (rbcur->resval.rint==2)
											cmd_prtstr(ResourceString(IDC_CMDS9_QUADRATIC_280, "Quadratic" ),ResourceString(IDC_CMDS9_FIT_TYPE__279, "Fit type:" ));
										}
									else if (rbcur->resval.rint==8)
										{
										cmd_prtstr(ResourceString(IDC_CMDS9_BEZIER_281, "Bezier" ),ResourceString(IDC_CMDS9_SURFACE_TYPE__282, "Surface type:" ));
										}
									}
								}
							}
    					//Check if extrusion direction parallel to Z axis for
						//	current UCS

						if ((plflag & (IC_PLINE_3DPLINE | IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH)))
							{
							//NOTE: for 2D pline, tmpint will flag if extru direction matches UCS Z dir
							if (!ic_assoc(erb,210))
								{
								ic_ptcpy(cenpoint,ic_rbassoc->resval.rpoint);
								fr1=cenpoint[0]*cenpoint[0]+cenpoint[1]*cenpoint[1]+cenpoint[2]*cenpoint[2];
								if (fabs(fr1-1.0) > 2.0*CMD_FUZZ)  //approx (1+CMD_FUZZ) squared...
									{
									cenpoint[0]/=fr1;
									cenpoint[1]/=fr1;
									cenpoint[2]/=fr1;
									}
								}
							else
								{
								cenpoint[0]=cenpoint[1]=0.0;
								cenpoint[2]=1.0;
								}
							//get ucs's Zdir
							SDS_getvar(NULL,DB_QUCSXDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
							ic_ptcpy(tmppt,rb.resval.rpoint);
							SDS_getvar(NULL,DB_QUCSYDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
							startpoint[0]=tmppt[1]*rb.resval.rpoint[2]-tmppt[2]*rb.resval.rpoint[1];
							startpoint[1]=-tmppt[0]*rb.resval.rpoint[2]+tmppt[2]*rb.resval.rpoint[0];
							startpoint[2]=tmppt[0]*rb.resval.rpoint[1]-tmppt[1]*rb.resval.rpoint[0];
							if (icadPointEqual(startpoint,cenpoint))
								tmpint=1;
								//NOTE: tmppt contains ucs X direction
							else
								tmpint=0;
							}
						else
							{
							tmpint=0;
							}

						vtxcnt=nverts=0;//we'll use vtxcnt int in display before next vtx
						sa=ea=0.0;
						if (db_edata(ent1,NULL,&nverts,&sa,&ea)==0)
							{
							if (nverts>0)
								{
								cmd_prtint(nverts,ResourceString(IDC_CMDS9_NUMBER_OF_VERTIC_283, "Number of vertices:" ));
								if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))
									goto bail;
								}
							if (sa>0.0)
								{
								cmd_prtreal(sa,ResourceString(IDC_CMDS9_TOTAL_LENGTH__284, "Total length:" ));
								perimeter=sa;
								if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))
									goto bail;
								
								}
							if (ea>0.0)
								{
								cmd_prtarea(ea,ResourceString(IDC_CMDS9_TOTAL_AREA__285, "Total area:" ));
								area=ea;
								if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))
									goto bail;
								}
							}
    					sds_entnext_noxref(ent1,ent1);
						IC_RELRB(erb);
    					erb=sds_entget(ent1);
						}
					else if (strsame(etype,"VERTEX"/*DNT*/ ))
						{
    					//flags
    					if (ic_assoc(erb,70)==0) 
							{
							if (ic_rbassoc->resval.rint & IC_VERTEX_FROMFIT)
    							cmd_prtstr(NULL,ResourceString(IDC_CMDS9_CURVE_FITTED_VER_286, "Curve-fitted vertex." ));
							else if (ic_rbassoc->resval.rint & IC_VERTEX_SPLINEVERTEX)
								cmd_prtstr(NULL,ResourceString(IDC_CMDS9_SPLINE_FITTED_VE_287, "Spline-fitted vertex." ));
							else if (ic_rbassoc->resval.rint & IC_VERTEX_SPLINEFRAME)
								cmd_prtstr(NULL,ResourceString(IDC_CMDS9_SPLINE_FIT_CONTR_288, "Spline fit control vertex." ));
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    						}
						if (IC_VERTEX_FACELIST==(ic_rbassoc->resval.rint & (IC_VERTEX_3DMESHVERT | IC_VERTEX_FACELIST)))
							{
							cmd_prtstr(NULL,ResourceString(IDC_CMDS9_FACE_DEFINITION__289, "Face definition vertex." ));
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))
								goto bail;
							for (erb2=erb;erb2!=NULL;erb2=erb2->rbnext)
								{
								if (erb2->restype==71 && erb2->resval.rint!=0)
									cmd_prtint(erb2->resval.rint,(erb2->resval.rint<0)?ResourceString(IDC_CMDS9_INVISIBLE_FIRST__290, "Invisible First Vertex:" ):ResourceString(IDC_CMDS9_FIRST_VERTEX__291, "First Vertex:" ));
								if (erb2->restype==72 && erb2->resval.rint!=0)
									cmd_prtint(erb2->resval.rint,(erb2->resval.rint<0)?ResourceString(IDC_CMDS9_INVISIBLE_SECOND_292, "Invisible Second Vertex:" ):ResourceString(IDC_CMDS9_SECOND_VERTEX__293, "Second Vertex:" ));
								if (erb2->restype==73 && erb2->resval.rint!=0)
									cmd_prtint(erb2->resval.rint,(erb2->resval.rint<0)?ResourceString(IDC_CMDS9_INVISIBLE_THIRD__294, "Invisible Third Vertex:" ):ResourceString(IDC_CMDS9_THIRD_VERTEX__295, "Third Vertex:" ));
								if (erb2->restype==74 && erb2->resval.rint!=0)
									cmd_prtint(erb2->resval.rint,(erb2->resval.rint<0)?ResourceString(IDC_CMDS9_INVISIBLE_FOURTH_296, "Invisible Fourth Vertex:" ):ResourceString(IDC_CMDS9_FOURTH_VERTEX__297, "Fourth Vertex:" ));
								if (erb2->restype>=71 && erb2->restype<=74 && erb2->resval.rint!=0)
									{
									if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))
										{
										erb2=NULL;
										goto bail;
										}
									}
								}
							}
						else
							{
							//for vertices which don't define a 3dface
    						if (ic_assoc(erb,10)==0) 
								{
								if (plflag & (IC_PLINE_3DPLINE | IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH))
									{
									sds_trans(ic_rbassoc->resval.rpoint,&rbwcs,&rbucs,0,startpoint);
									}
								else
									{
									ic_rbassoc->resval.rpoint[2]=pl_elev;
									sds_trans(ic_rbassoc->resval.rpoint,&rbent,&rbucs,0,startpoint);
									}
    							cmd_prtpt(startpoint,ResourceString(IDC_CMDS9_LOCATION__298, "Location:" ));
								if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))
									goto bail;
    							}
							if (!(plflag & (IC_PLINE_3DPLINE | IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH)))
								{
								//starting width
    							if (ic_assoc(erb,40)==0) 
									{
    								cmd_prtreal(ic_rbassoc->resval.rreal,ResourceString(IDC_CMDS9_STARTING_WIDTH__299, "Starting width:" ));
									if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))
										goto bail;
    								}
    							//ending width
    							if (ic_assoc(erb,41)==0) 
									{
    								cmd_prtreal(ic_rbassoc->resval.rreal,ResourceString(IDC_CMDS9_ENDING_WIDTH__300, "Ending width:" ));
									if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))
										goto bail;
    								}
   								//bulge
   								ic_assoc(erb,42);
   								cmd_prtreal(ic_rbassoc->resval.rreal, ResourceString(IDC_CMDS9_BULGE__301,"Bulge:"));
								sds_entnext_noxref(ent1,ent2);
								if ((plflag & IC_PLINE_CLOSED) || IC_TYPE_FROM_ENAME(ent2)!=DB_SEQEND) 
									{
									//	if next ent is not a seqend on open pline

									//radius
									if (erb2!=NULL)
										{
										sds_relrb(erb2);
										erb2=NULL;
										}
									if ((erb2=sds_entget(ent2))==NULL)
										{
										rc=RTERROR;
										goto bail;
										}
									ic_assoc(erb2,10);
									if (plflag & (IC_PLINE_3DPLINE | IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH))
										{
										sds_trans(ic_rbassoc->resval.rpoint,&rbwcs,&rbucs,0,endpoint);
										}
									else
										{
										sds_trans(ic_rbassoc->resval.rpoint,&rbent,&rbucs,0,endpoint);
										}
									if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))
										goto bail;
									// get the bulge
									ic_assoc(erb,42);
									if (!icadRealEqual(ic_rbassoc->resval.rreal,0.0) && 0==ic_bulge2arc(startpoint,endpoint,ic_rbassoc->resval.rreal,cenpoint,&rr,&sa,&ea))
										{
										if (tmpint)//if extru direction ok...
											{
											ic_normang(&sa,NULL);
											ic_normang(&ea,NULL);

// removed this angle-swapping code.  It does seem to make the data more accurate,
// but does not report them the same way ACAD does, which (possibly) is always CCW
//											if (ic_rbassoc->resval.rreal<0.0){
//												cmd_prtangle(ea,"Start Angle:");
//												if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
//												cmd_prtangle(sa,"End Angle:");
//												if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
//											}else{
											cmd_prtangle(ea,ResourceString(IDC_CMDS9_START_ANGLE__261, "Start Angle:" ));
											if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))
												goto bail;
											cmd_prtangle(ea,ResourceString(IDC_CMDS9_END_ANGLE__262, "End Angle:" ));
											if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))
												goto bail;
//											}
										}
										cmd_prtreal(rr,ResourceString(IDC_CMDS9_RADIUS__245, "Radius:" ));
										if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))
											goto bail;
										cenpoint[2]=pl_elev;
										cmd_prtpt(cenpoint,ResourceString(IDC_CMDS9_CENTER_POINT__255, "Center point:" ));
										if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))
											goto bail;
										}
									}
								}
							}
						IC_RELRB(erb);
						IC_RELRB(erb2);//can't use as next erb if stepping around closed pline
						
						sds_entnext_noxref(ent1,ent1);
    					erb=sds_entget(ent1);
    				//If ent is a Seqend marker
    					}
					else if ((stricmp(etype,"SEQEND"/*DNT*/)) == 0) 
						{
    					if (ic_assoc(erb,-2)==0) 
							{
		//                    cmd_prtstr(ic_rbassoc->resval.rstring,"Sequence end for:");
    						}
						if (vtxcnt>0)
							cmd_prtint(vtxcnt,ResourceString(IDC_CMDS9_NUMBER_OF_VERTIC_283, "Number of vertices:" ));
						if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))
							goto bail;
						vtxcnt=0;
						pl_nct=0;
    					endflag=1;
						}
					break;
					}
				// If ent is an Image 
				case 25:
					{
						if (ic_assoc(erb,10)==0) {
							sds_trans(ic_rbassoc->resval.rpoint,&rbwcs,&rbucs,0,startpoint);
    						cmd_prtpt(startpoint,ResourceString(IDC_CMDS9_START_POINT__302, "Start point:" ));
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    					}
						if (ic_assoc(erb,281)==0) {
							cmd_prtint(ic_rbassoc->resval.rint,ResourceString(IDC_CMDS9_BRIGHTNESS, "Brightness:" ));
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    					}
						if (ic_assoc(erb,282)==0) {
							cmd_prtint(ic_rbassoc->resval.rint,ResourceString(IDC_CMDS9_CONTRAST, "Contrast:" ));
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    					}
						if (ic_assoc(erb,283)==0) {
							cmd_prtint(ic_rbassoc->resval.rint,ResourceString(IDC_CMDS9_FADE, "Fade:" ));
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    					}
						if (ic_assoc(erb,70)==0) {
							if (ic_rbassoc->resval.rint & IC_IMAGE_DISPPROPS_SHOWIMAGE)
								cmd_prtstr("Yes" ,ResourceString(IDC_CMDS9_SHOW_IMAGE, "Show image:" ));
							else cmd_prtstr("No" ,ResourceString(IDC_CMDS9_SHOW_IMAGE, "Show image:" ));
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
							if (ic_rbassoc->resval.rint & IC_IMAGE_DISPPROPS_SHOWNONALIGNED)
								cmd_prtstr("Yes" ,ResourceString(IDC_CMDS9_SHOW_NON_ORTHO, "Show non-ortho:" ));
							else cmd_prtstr("No" ,ResourceString(IDC_CMDS9_SHOW_NON_ORTHO, "Show non-ortho:" ));
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
							if (ic_rbassoc->resval.rint & IC_IMAGE_DISPPROPS_TRANSON)
								cmd_prtstr("On" ,ResourceString(IDC_CMDS9_TRANSPARENCY,	"Transparency:" ));
							else cmd_prtstr("Off" ,ResourceString(IDC_CMDS9_TRANSPARENCY, "Transparency:" ));
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
							if (ic_rbassoc->resval.rint & IC_IMAGE_DISPPROPS_USECLIP)
								cmd_prtstr("On" ,ResourceString(IDC_CMDS9_CLIPPING_BOUNDARY, "Clipping boundary:" ));
							else cmd_prtstr("Off" ,ResourceString(IDC_CMDS9_CLIPPING_BOUNDARY, "Clipping boundary:" ));
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
						}
						if (ic_assoc(erb,280)==0) {
							if(ic_rbassoc->resval.rint == 0)
    							cmd_prtstr("Off",ResourceString(IDC_CMDS9_CLIPPING_STATE, "Clipping state:" ));
							else cmd_prtstr("On",ResourceString(IDC_CMDS9_CLIPPING_STATE, "Clipping state:" ));
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    					}
						if (ic_assoc(erb,71)==0) {
							// EBATECH {2001/9/20 localize.
							if(ic_rbassoc->resval.rint == 1)
								cmd_prtstr(
								    ResourceString(IDC_CMDS9_RECTANGULAR,"Rectangular"),
								    ResourceString(IDC_CMDS9_CLIP_BOUNDARY_TYPE, "Clip boundary type:" )
								  );
							else cmd_prtstr(
								    ResourceString(IDC_CMDS9_POLYGONAL,"Polygonal"),
									ResourceString(IDC_CMDS9_CLIP_BOUNDARY_TYPE,	"Clip boundary type:" )
							     );
							// }EBATECH
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    					}
					endflag=1;
					break;
				}
					//Bugzilla No 78004 18-03-2002  [
					//if ent is a leader
				case 26:{
						sds_point dirpt;
						
						if (ic_assoc(erb,210)==0)	//Normal
						{
							sds_trans(ic_rbassoc->resval.rpoint,&rbwcs,&rbucs,0,dirpt);
							cmd_prtpt(dirpt,ResourceString(IDC_CMDS9_LEADER_NORMAL	,	 "Normal  :"));
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
						}
						
						if (ic_assoc(erb,211)==0)	//Horizontal Direction
						{
							sds_trans(ic_rbassoc->resval.rpoint,&rbwcs,&rbucs,0,dirpt);
							cmd_prtpt(dirpt,ResourceString(IDC_CMDS9_LEADER_HOR_DIR	, "Horizontal Direction :"));
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
						}
						
						//vertexes
						for (rbcur=erb;rbcur!=NULL;rbcur=rbcur->rbnext){
							if (10==rbcur->restype) {
								 
                                    sds_trans(rbcur->resval.rpoint,&rbwcs,&rbucs,0,startpoint);
    							    cmd_prtpt(startpoint,ResourceString(IDC_CMDS9_DIM_DEFINITION_P_165, "Dim definition point:" ));
								    if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    						    }
                        	}
					

						if (ic_assoc(erb,75)==0) //Hook Line
						{
							(ic_rbassoc->resval.rint==1)? cmd_prtstr("On" ,ResourceString(IDC_CMDS9_LEADER_HOOK_LINE	, "Hook Line:")): cmd_prtstr("Off" ,ResourceString(IDC_CMDS9_LEADER_HOOK_LINE	, "Hook Line:"));
							
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
						}
						
						if (ic_assoc(erb,71)==0) //Arrow
						{
							(ic_rbassoc->resval.rint==1)? cmd_prtstr("On" ,ResourceString(IDC_CMDS9_LEADER_ARROW ,  "Arrow :")): cmd_prtstr("Off" ,ResourceString(IDC_CMDS9_LEADER_ARROW , "Arrow :"));
							
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
						}
						
						if (ic_assoc(erb,72)==0) //Path Type
						{
							(ic_rbassoc->resval.rint==0)? cmd_prtstr("Straight" ,ResourceString(IDC_CMDS9_LEADER_PATH_TYPE	, "Path Type :")): cmd_prtstr("Spline" ,ResourceString(IDC_CMDS9_LEADER_PATH_TYPE	, "Path Type :"));
							
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
						}
						
						
						if (ic_assoc(erb,340)==0)	//Association
						{			
							char sztemp[IC_ACADBUF];						
							struct sds_resbuf*elist=sds_entget(ic_rbassoc->resval.rlname);
							if(elist)
							{
								if(ic_assoc(elist,0)==0)
								{
									strcpy(sztemp,ic_rbassoc->resval.rstring );								
									if(ic_assoc(elist,5)==0)
									{
										sprintf(sztemp,"%s			%s %s" ,sztemp,ResourceString(IDC_CMDS9_HANDLE__153, "Handle:" ),ic_rbassoc->resval.rstring );
										cmd_prtstr(sztemp, ResourceString(IDC_CMDS9_LEADER_ASSOCIATION , "Associated To :"));
									}
								}
								//BugZilla:78448; Added while fixing this bug but is not the cause of the bug [
								IC_RELRB(elist);
								//BugZilla:78448; ]
							}
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
						}
						
					rc=cmd_print_overrides(ent1,scrollcur,scrollmax);           
					if(rc!=RTNORM) goto bail; 
					endflag=1;
					break;
					}
					//Bugzilla No 78004 18-03-2002
					//Bugzilla No 78046 25-03-2002
					//if ent is a hatch
				case 27:{
						
						
						if (ic_assoc(erb,2)==0)	//pattern name
						{
							cmd_prtstr(ic_rbassoc->resval.rstring,ResourceString(IDC_CMDS9_PAT_NAME__321 , "Pattern name:"));
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
						}
						
						if (ic_assoc(erb,52)==0)	//pattern angle
						{
							cmd_prtangle(ic_rbassoc->resval.rreal,ResourceString(IDC_CMDS9_PATTERN_ANGLE__328, "Pattern angle:" ));
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
						}
						if (ic_assoc(erb,41)==0)	//pattern scale
						{
							cmd_prtscale(ic_rbassoc->resval.rreal,ResourceString(IDC_CMDS9_PATTERN_SCALE__327, "Pattern scale:" ));
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
						}
						if (ic_assoc(erb,71)==0) //associative?
						{
							if(ic_rbassoc->resval.rint==1)
								cmd_prtstr(" ",ResourceString(IDC_CMDS9_ASSOCIATIVE	,	"Associative "));
							else
								cmd_prtstr(" ",ResourceString(IDC_CMDS9_NON_ASSOCIATIVE	,"non-Associative "));
							
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
						}
					
					endflag=1;
					break;
					}
					//Bugzilla No 78046 25-03-2002
				
				//If ent is a lightweight polyline
				case 24: 
					{
					resbuf *trb;
					sds_real elev=0.0,startwidth,endwidth;
					int numpts,flag;
					trb=ic_ret_assoc(erb,38);
					if (trb)
						elev=trb->resval.rreal;
					trb=ic_ret_assoc(erb,70);
					if (trb)
						flag=trb->resval.rint;
					trb=ic_ret_assoc(erb,90);
					if (trb)
						numpts=trb->resval.rint;

					if (flag & IC_PLINE_CLOSED)
						cmd_prtstr(ResourceString(IDC_CMDS9_CLOSED_266, "Closed" ),ResourceString(IDC_CMDS9_POLYLINE_FLAGS__264, "Polyline Flags:" ));                   
					else cmd_prtstr(ResourceString(IDC_CMDS9_OPEN_267, "Open" ),ResourceString(IDC_CMDS9_POLYLINE_FLAGS__264, "Polyline Flags:" ));                   
					if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;

					bool printwidths=false;
					trb=ic_ret_assoc(erb,10);
					while (trb)
						{
						if (trb->restype==40 && !icadRealEqual(trb->resval.rreal,0.0))
							printwidths=true;
						if (trb->restype==41 && !icadRealEqual(trb->resval.rreal,0.0))
							printwidths=true;
						trb=trb->rbnext;
						}

					//Modified Cybage AW 01-12-00 [
					//Reason : Enhanced LIST command does not show correct 
					//information (Area, Perimeter)  
					
					if (ic_assoc(erb,40)==0) 
					{
						perimeter = area =0.0;
						ret=db_edata(ent1,NULL,NULL,&perimeter,&area);
	   					cmd_prtarea(area,ResourceString(IDC_CMDS9_AREA__244, "Area:" ));
						if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
						cmd_prtreal(perimeter,ResourceString(IDC_CMDS9_PERIMETER__243, "Perimeter:" ));
						if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
	   				}
					//Modified Cybage AW 01-12-00 ]	

					trb=ic_ret_assoc(erb,10);
					for (int i=0; i<numpts; i++)
						{
						sds_point temppt,transpt;
						temppt[0]=trb->resval.rpoint[0];
						temppt[1]=trb->resval.rpoint[1];
						temppt[2]=elev;
						sds_trans(temppt,&rbent,&rbucs,0,transpt);
    					cmd_prtpt(transpt,ResourceString(IDC_CMDS9_LOCATION__298, "Location:" ));
						if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
						trb=trb->rbnext;

						startwidth=trb->resval.rreal;
						trb=trb->rbnext;
						endwidth=trb->resval.rreal;
						trb=trb->rbnext;
						if (printwidths || !icadRealEqual(startwidth,0.0) || !icadRealEqual(endwidth,0.0))
							{
							printwidths=true;
   							cmd_prtreal(startwidth,ResourceString(IDC_CMDS9_STARTING_WIDTH__299, "Starting width:" ));
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    						cmd_prtreal(endwidth,ResourceString(IDC_CMDS9_ENDING_WIDTH__300, "Ending width:" ));
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
							}

						if (!icadRealEqual(trb->resval.rreal,0.0)) // the bulge
							{
							cmd_prtreal(trb->resval.rreal, ResourceString(IDC_CMDS9_BULGE__301,"Bulge:"));
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
							}
						trb=trb->rbnext;
						}
					endflag=1;
					break;
				}
    			//If ent is a Ray
				case 2: {
    				if (ic_assoc(erb,10)==0) {
						sds_trans(ic_rbassoc->resval.rpoint,&rbwcs,&rbucs,0,startpoint);
    					cmd_prtpt(startpoint,ResourceString(IDC_CMDS9_START_POINT__302, "Start point:" ));
						if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    				}
    				if (ic_assoc(erb,11)==0) {
						sds_trans(ic_rbassoc->resval.rpoint,&rbwcs,&rbucs,1,endpoint);
    					cmd_prtpt(endpoint,ResourceString(IDC_CMDS9_UNIT_DIRECTION__303, "Unit direction:" ));
						if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
						if (!icadRealEqual(endpoint[0],0.0) && !icadRealEqual(endpoint[1],0.0)){
                            cmd_prtangle(atan2(endpoint[1],endpoint[0]),ResourceString(IDC_CMDS9_ANGLE_IN_XY_PLAN_304, "Angle in XY plane:" ));
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
						}
    				}
    				endflag=1;
					break;
				}
				//If ent is an Xline
				case 3: {
    				if (ic_assoc(erb,10)==0) {
						sds_trans(ic_rbassoc->resval.rpoint,&rbwcs,&rbucs,0,startpoint);
    					cmd_prtpt(startpoint,ResourceString(IDC_CMDS9_START_POINT__302, "Start point:" ));
						if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    				}
    				if (ic_assoc(erb,11)==0) {
						sds_trans(ic_rbassoc->resval.rpoint,&rbwcs,&rbucs,1,endpoint);
    					cmd_prtpt(endpoint,ResourceString(IDC_CMDS9_UNIT_DIRECTION__303, "Unit direction:" ));
						if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
						if (!icadRealEqual(endpoint[0],0.0) && !icadRealEqual(endpoint[1],0.0)){
                            cmd_prtangle(atan2(endpoint[1],endpoint[0]),ResourceString(IDC_CMDS9_ANGLE_IN_XY_PLAN_304, "Angle in XY plane:" ));
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
						}
    				}
    				endflag=1;
					break;
				}
    			//If ent is an Insert
				case 12: {
					if (strsame(etype,"INSERT"/*DNT*/ )){
						endflag=1;
						for (rbcur=erb;rbcur!=NULL;rbcur=rbcur->rbnext){
							if (2==rbcur->restype){
								strncpy(tmpstr,rbcur->resval.rstring,sizeof(tmpstr)-1);
								if (strnsame(tmpstr,"*X"/*DNT*/,2))
									cmd_prtstr(NULL,ResourceString(IDC_CMDS9_BLOCK_CREATED_BY_306, "Block created by hatching." ));
								else if (strnsame(tmpstr,"*D"/*DNT*/ ,2))
									cmd_prtstr(NULL,ResourceString(IDC_CMDS9_BLOCK_CREATED_BY_308, "Block created by dimensioning." ));
								else if (strnsame(tmpstr,"*U"/*DNT*/ ,2) || strnsame(tmpstr,"*B"/*DNT*/ ,2))	/*D.G.*/// 'B' code added for compatibility with German ACAD (CR1606).
									cmd_prtstr(tmpstr,ResourceString(IDC_CMDS9_ANONYMOUS_BLOCK__310, "Anonymous block name:" ));
								else
    								cmd_prtstr(tmpstr,ResourceString(IDC_CMDS9_BLOCK_NAME__311, "Block name:" ));
								if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
								if (erb2!=NULL){sds_relrb(erb2);erb2=NULL;}
                               if (NULL==(erb2=sds_tblsearch("BLOCK"/*DNT*/,tmpstr,0))){rc=RTERROR;goto bail;}
								ic_assoc(erb2,70);
								if (ic_rbassoc->resval.rint&4){
									cmd_prtstr(ResourceString(IDC_CMDS9_BLOCK_IS_AN_EXTE_313, "Block is an external reference." ),ResourceString(IDC_CMDS9_NOTE__314, "NOTE:" ));
									if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
								}
								sds_relrb(erb2);erb2=NULL;
							}
							if (10==rbcur->restype){
								sds_trans(rbcur->resval.rpoint,&rbent,&rbucs,0,startpoint);
    							cmd_prtpt(startpoint,ResourceString(IDC_CMDS9_INSERTION_POINT__315, "Insertion point:" ));
								if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    						}
							if (41==rbcur->restype){
    							cmd_prtscale(rbcur->resval.rreal,ResourceString(IDC_CMDS9_X_SCALE_FACTOR__316, "X scale factor:" ));
								if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    						}
							if (42==rbcur->restype){
    							cmd_prtscale(rbcur->resval.rreal,ResourceString(IDC_CMDS9_Y_SCALE_FACTOR__317, "Y scale factor:" ));
								if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    						}
							if (43==rbcur->restype){
    							cmd_prtscale(rbcur->resval.rreal,ResourceString(IDC_CMDS9_Z_SCALE_FACTOR__318, "Z scale factor:" ));    
								if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    						}                                                              
							if (50==rbcur->restype){
								tmppt[0]=1.0;
								tmppt[1]=tmppt[2]=0.0;
								sds_trans(tmppt,&rbucs,&rbent,1,tmppt);
								fr1=atan2(tmppt[1],tmppt[0]);
								ic_normang(&fr1,&rbcur->resval.rreal);
    							cmd_prtangle(rbcur->resval.rreal-fr1,ResourceString(IDC_CMDS9_ROTATION_ANGLE__319, "Rotation angle:" ));
 								if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
   							}
							if (66==rbcur->restype && 1==rbcur->resval.rint)endflag=0;
						}

                        char *szTmp=NULL;
                        rbfilter.restype=RTSTR;
                        rbfilter.resval.rstring=(char *)(LPCTSTR)"ACAD"/*DNT*/ ;
						rbfilter.rbnext=NULL;
                        erbx=sds_entgetx(ent1,&rbfilter);
                        for (rbcur=erbx;rbcur!=NULL&&rbcur->restype!=1000;rbcur=rbcur->rbnext);
    				    if ((rbcur)&&(strsame("HATCH"/*DNT*/ ,rbcur->resval.rstring))){
                            for (rbcur=rbcur->rbnext;rbcur!=NULL&&rbcur->restype!=1000;rbcur=rbcur->rbnext);
                            if (rbcur){
                                szTmp=strchr(rbcur->resval.rstring,',');  
                                if (szTmp==NULL){
                                    //Bugzilla No 78046 25-03-2002
									cmd_prtstr(rbcur->resval.rstring,ResourceString(IDC_CMDS9_PAT_NAME__321	,	"Pattern name:"));
								    if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
                                }else{
                                    *szTmp='\0'/*DNT*/;
                                    szTmp++;
                                    *szTmp='\0'/*DNT*/;
                                    szTmp++;
                                    //Bugzilla No 78046 25-03-2002
									cmd_prtstr(rbcur->resval.rstring,ResourceString(IDC_CMDS9_PAT_NAME__321	, "Pattern name:"));
								    if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
                                    if (*szTmp=='O'){
                                        cmd_prtstr(ResourceString(IDC_CMDS9_OUTER_322, "Outer" ),ResourceString(IDC_CMDS9_BOUNDARIES__323, "Boundaries:" ));
				    				    if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
                                    }else if (*szTmp=='I'){
                                        cmd_prtstr(ResourceString(IDC_CMDS9_INNER_324, "Inner" ),ResourceString(IDC_CMDS9_BOUNDARIES__323, "Boundaries:" ));
			    					    if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
                                    }else if (*szTmp=='U'){
                                        cmd_prtstr(ResourceString(IDC_CMDS9_STANDARD_LINES_P_325, "Standard lines pattern" ),ResourceString(IDC_CMDS9___176, " " ));
		    						    if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
                                        for (rbcur=rbcur->rbnext;rbcur!=NULL&&rbcur->restype!=1070;rbcur=rbcur->rbnext);
                                        if ((rbcur)&&(rbcur->resval.rint)){
                                            cmd_prtstr(ResourceString(IDC_CMDS9_CROSS_HATCHED_326, "Cross-hatched" ),ResourceString(IDC_CMDS9_BOUNDARIES__323, "Boundaries:" ));
        								    if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
                                        }
                                    }
                                }
                            }
                            for (rbcur=erbx;rbcur!=NULL&&rbcur->restype!=1040;rbcur=rbcur->rbnext);
    				        if (rbcur){
                                cmd_prtscale(rbcur->resval.rreal,ResourceString(IDC_CMDS9_PATTERN_SCALE__327, "Pattern scale:" ));
							    if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    				            if (rbcur->rbnext){
       				                cmd_prtangle(rbcur->rbnext->resval.rreal,ResourceString(IDC_CMDS9_PATTERN_ANGLE__328, "Pattern angle:" ));
							        if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
                                }
                            }
                        }
                        IC_RELRB(erbx);
						attcnt=0;
    					sds_entnext_noxref(ent1,ent1);            
    					erb=sds_entget(ent1);              
                    } else if ((stricmp(etype,"ATTRIB"/*DNT*/)) == 0) {
						for (rbcur=erb;rbcur!=NULL;rbcur=rbcur->rbnext){
							if (10==rbcur->restype) {                                     
								sds_trans(rbcur->resval.rpoint,&rbent,&rbucs,0,startpoint);
								cmd_prtpt(startpoint,ResourceString(IDC_CMDS9_TEXT_START_POINT_329, "Text start point:" )); 
								if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
							}                                                               
							if (1==rbcur->restype) {  
								cmd_prtstr(rbcur->resval.rstring,ResourceString(IDC_CMDS9_VALUE__330, "Value:" ));   
								if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
							}
							if (2==rbcur->restype) {                               
								cmd_prtstr(rbcur->resval.rstring,ResourceString(IDC_CMDS9_TAG__331, "Tag:" ));    
								if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
							}   
							//Modified Cybage AW 17-09-01 [
							//Reason : If multiple attribute flags are set, all should get listed
							//in the list command
							//Bug # 77844 from Bugzilla	
							/*
							if (70==rbcur->restype) {   
								if (rbcur->resval.rint & 1) {
									cmd_prtstr(ResourceString(IDC_CMDS9_INVISIBLE_332, "Invisible" ),ResourceString(IDC_CMDS9_ATTRIBUTE_FLAGS__333, "Attribute Flags:" ));                           
									if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
								}else if (rbcur->resval.rint & 2) {
									cmd_prtstr(ResourceString(IDC_CMDS9_CONSTANT_334, "Constant" ),ResourceString(IDC_CMDS9_ATTRIBUTE_FLAGS__333, "Attribute Flags:" ));   
									if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
								}else if (rbcur->resval.rint & 4) {                            
									cmd_prtstr(ResourceString(IDC_CMDS9_VERIFICATION_REQ_335, "Verification required" ),ResourceString(IDC_CMDS9_ATTRIBUTE_FLAGS__333, "Attribute Flags:" ));                 
									if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
								}else if (rbcur->resval.rint & 8) {                              
									cmd_prtstr(ResourceString(IDC_CMDS9_PRESET__NO_PROMP_336, "Preset (no prompting)" ),ResourceString(IDC_CMDS9_ATTRIBUTE_FLAGS__333, "Attribute Flags:" ));    
									if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
								}                                                     
							} 
							*/
							if (70==rbcur->restype) {   
							CString str;
							str.Empty();
							sds_printf("\n%25s"/*DNT*/,ResourceString(IDC_CMDS9_ATTRIBUTE_FLAGS__333, "Attribute Flags:" ));
							if (!(rbcur->resval.rint | 0)) {
								sds_printf("  %s", ResourceString(IDC_CMDS9_NORMAL_350, "Normal" ));
								if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
							}
							if (rbcur->resval.rint & IC_ATTRIB_INVIS) {
								sds_printf("  %s", ResourceString(IDC_CMDS9_INVISIBLE_332, "Invisible" ));
								if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
							}
							if (rbcur->resval.rint & IC_ATTRIB_CONST) {
								sds_printf("  %s", ResourceString(IDC_CMDS9_CONSTANT_334, "Constant" ));   
								if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
							}
							if (rbcur->resval.rint & IC_ATTRIB_VERIFY) {                            
								sds_printf("  %s", ResourceString(IDC_CMDS9_VERIFICATION_REQ_335, "Verification required" ));                 
								if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
							}
							if (rbcur->resval.rint & IC_ATTRIB_PRESET) {                              
								sds_printf("  %s", ResourceString(IDC_CMDS9_PRESET__NO_PROMP_336, "Preset (no prompting)" ));    
								if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
							}                                                     
						}
						//Modified Cybage AW 17-09-01 ]
							if (73==rbcur->restype) {                           
								cmd_prtint(rbcur->resval.rint,ResourceString(IDC_CMDS9_FIELD_LENGTH__337, "Field length:" ));   
								if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
							} 
							//style moved to top of fn
							//if (7==rbcur->restype) {                                     
							//	cmd_prtstr(rbcur->resval.rstring,"Text style:");     
							//	if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
							//}                                                               
							if (50==rbcur->restype) {                                
								tmppt[0]=1.0;
								tmppt[1]=tmppt[2]=0.0;
								sds_trans(tmppt,&rbucs,&rbent,1,tmppt);
								fr1=atan2(tmppt[1],tmppt[0]);
								ic_normang(&fr1,&rbcur->resval.rreal);
								cmd_prtangle(rbcur->resval.rreal-fr1,ResourceString(IDC_CMDS9_TEXT_ROTATION__338, "Text rotation:" ));   
								if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
							}                                                          
							if (41==rbcur->restype && !icadRealEqual(rbcur->resval.rreal,1.0)) {                                   
								cmd_prtscale(rbcur->resval.rreal,ResourceString(IDC_CMDS9_RELATIVE_X_SCALE_339, "Relative X scale factor:" ));   
								if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
							}                                                             
							if (51==rbcur->restype && !icadRealEqual(rbcur->resval.rreal,0.0)) {                                             
								cmd_prtangle(rbcur->resval.rreal,ResourceString(IDC_CMDS9_OBLIQUE_ANGLE__340, "Oblique angle:" ));   
								if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
							}                                                                       
							if (71==rbcur->restype /*&& 0!=rbcur->resval.rint*/) {                                   
                                ic_assoc(erb,7);
                                struct resbuf *tmprb=NULL;
                                if ((tmprb=sds_tblsearch("STYLE"/*DNT*/,ic_rbassoc->resval.rstring,0))!=NULL){
                                    ic_assoc(tmprb,70);
                                    if (ic_rbassoc->resval.rint&4){
                                        if (rbcur->resval.rint!=0){
                                            if (rbcur->resval.rint&2){
							                    if (rbcur->resval.rint&4){
                                                    cmd_prtstr(ResourceString(IDC_CMDS9_BACKWARD__UPSIDE_342, "Backward ,Upside-down and Vertical" ),ResourceString(IDC_CMDS9_GENERATION__343, "Generation:" ));         
							                    }else{
                                                    cmd_prtstr(ResourceString(IDC_CMDS9_BACKWARD_AND_VER_344, "Backward and Vertical" ),ResourceString(IDC_CMDS9_GENERATION__343, "Generation:" ));         
                                                }
                                            }else{
							                    if (rbcur->resval.rint&4){
    								                cmd_prtstr(ResourceString(IDC_CMDS9_UPSIDE_DOWN_AND__345, "Upside-down and Vertical" ),ResourceString(IDC_CMDS9_GENERATION__343, "Generation:" ));      
                                                }
                                            }
							            }else{	
                               	            cmd_prtstr(ResourceString(IDC_CMDS9_VERTICAL_346, "Vertical" ),ResourceString(IDC_CMDS9_GENERATION__343, "Generation:" ));    
							            }
                                    }else{
                                        if (rbcur->resval.rint!=0){
                                            if (rbcur->resval.rint&2){
							                    if (rbcur->resval.rint&4){
                                                    cmd_prtstr(ResourceString(IDC_CMDS9_BACKWARD_AND_UPS_347, "Backward and Upside-down" ),ResourceString(IDC_CMDS9_GENERATION__343, "Generation:" ));         
							                    }else{
                                                    cmd_prtstr(ResourceString(IDC_CMDS9_BACKWARD_348, "Backward" ),ResourceString(IDC_CMDS9_GENERATION__343, "Generation:" ));         
                                                }
                                            }else{
							                    if (rbcur->resval.rint&4){
    								                cmd_prtstr(ResourceString(IDC_CMDS9_UPSIDE_DOWN_349, "Upside-down" ),ResourceString(IDC_CMDS9_GENERATION__343, "Generation:" ));      
                                                }
                                            }
							            }else{	
                               	            cmd_prtstr(ResourceString(IDC_CMDS9_NORMAL_350, "Normal" ),ResourceString(IDC_CMDS9_GENERATION__343, "Generation:" ));    
							            }
							        }
                                    if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
						        }
                                IC_RELRB(tmprb);
							}                                                            
							if (73==rbcur->restype) {                                            
								switch(rbcur->resval.rint) {                                  
									case 0:                                                        
										cmd_prtstr(ResourceString(IDC_CMDS9_BASELINE_351, "Baseline" ),ResourceString(IDC_CMDS9_VERTICAL_ALIGNME_352, "Vertical alignment:" ));                        
										break;                                                     
									case 1:                                                        
										cmd_prtstr(ResourceString(IDC_CMDS9_BOTTOM_353, "Bottom" ),ResourceString(IDC_CMDS9_VERTICAL_ALIGNME_352, "Vertical alignment:" ));                      
										break;                                                     
									case 2:                                                        
										cmd_prtstr(ResourceString(IDC_CMDS9_MIDDLE_354, "Middle" ),ResourceString(IDC_CMDS9_VERTICAL_ALIGNME_352, "Vertical alignment:" ));                    
										break;                                                     
									case 3:                                                        
										cmd_prtstr(ResourceString(IDC_CMDS9_TOP_355, "Top" ),ResourceString(IDC_CMDS9_VERTICAL_ALIGNME_352, "Vertical alignment:" ));                    
										break;         
									default:                                                       
										cmd_prtstr(ResourceString(IDC_CMDS9_BASELINE_351, "Baseline" ),ResourceString(IDC_CMDS9_VERTICAL_ALIGNME_352, "Vertical alignment:" ));                        
										break;                                                     
								}                                                                  
								if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
							}                                                                      
							if (72==rbcur->restype) {                                          
								switch(rbcur->resval.rint) {                                
									case 0:                                                      
										cmd_prtstr(ResourceString(IDC_CMDS9_LEFT_356, "Left" ),ResourceString(IDC_CMDS9_HORIZONTAL_ALIGN_357, "Horizontal alignment:" ));                      
										break;                                                   
									case 1:                                                      
										cmd_prtstr(ResourceString(IDC_CMDS9_CENTER_358, "Center" ),ResourceString(IDC_CMDS9_HORIZONTAL_ALIGN_357, "Horizontal alignment:" ));                    
										break;                                                   
									case 2:                                                      
										cmd_prtstr(ResourceString(IDC_CMDS9_RIGHT_359, "Right" ),ResourceString(IDC_CMDS9_HORIZONTAL_ALIGN_357, "Horizontal alignment:" ));                  
										break;
									//Modified Cybage AW 24-12-01 [
									//Reason : Wrong Prompts Displayed for attribute justification in the list command
									//Bug # 77951 from Bugzilla	
									/*
									case 3:                                              
										cmd_prtstr(ResourceString(IDC_CMDS9_RIGHT_359, "Right" ),ResourceString(IDC_CMDS9_HORIZONTAL_ALIGN_357, "Horizontal alignment:" ));   
										break;                                           
									case 4:                                              
										cmd_prtstr(ResourceString(IDC_CMDS9_RIGHT_359, "Right" ),ResourceString(IDC_CMDS9_HORIZONTAL_ALIGN_357, "Horizontal alignment:" ));   
										break;                                           
									case 5:                                              
										cmd_prtstr(ResourceString(IDC_CMDS9_RIGHT_359, "Right" ),ResourceString(IDC_CMDS9_HORIZONTAL_ALIGN_357, "Horizontal alignment:" ));    
										break;                                           
									*/
									case 3:                                              
										cmd_prtstr(ResourceString(IDC_CMDS9_ALIGNED_369, "Aligned" ),ResourceString(IDC_CMDS9_HORIZONTAL_ALIGN_357, "Horizontal alignment:" ));   
										break;                                           
									case 4:                                              
										cmd_prtstr(ResourceString(IDC_CMDS9_MIDDLE_354, "Middle" ),ResourceString(IDC_CMDS9_HORIZONTAL_ALIGN_357, "Horizontal alignment:" ));   
										break;                                           
									case 5:                                              
										cmd_prtstr(ResourceString(IDC_CMDS9_FITTED_370, "Fitted" ),ResourceString(IDC_CMDS9_HORIZONTAL_ALIGN_357, "Horizontal alignment:" ));    
										break;
									//Modified Cybage AW 24-12-01 ]	
									default:                                                     
										cmd_prtstr(ResourceString(IDC_CMDS9_LEFT_356, "Left" ),ResourceString(IDC_CMDS9_HORIZONTAL_ALIGN_357, "Horizontal alignment:" ));                      
										break;                                                   
								}                                                                
								if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
							}   
						}
    					sds_entnext_noxref(ent1,ent1);
						sds_relrb(erb);erb=NULL;
    					erb=sds_entget(ent1);       
    				} else if ((stricmp(etype,"SEQEND"/*DNT*/)) == 0) {
    					//if (ic_assoc(erb,-2)==0) {
		                //      cmd_prtstr(ic_rbassoc->resval.rstring,"Sequence end for:");
    					//}
						if (attcnt>0){
							cmd_prtint(attcnt,ResourceString(IDC_CMDS9_NUMBER_OF_ATTRIB_360, "Number of attributes:" ));
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
						}
						attcnt=0;
    					endflag=1;
					}
					break;
				}
    			//If ent is an Point                              
				case 0: {
    				if ((ic_assoc(erb,10)) == 0) {
						sds_trans(ic_rbassoc->resval.rpoint,&rbwcs,&rbucs,0,startpoint);
    					cmd_prtpt(startpoint,ResourceString(IDC_CMDS9_POINT__361, "Point:" ));
						if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    				}                                                          
					if ((ic_assoc(erb,50)) == 0) {
						cmd_prtangle(ic_rbassoc->resval.rreal,ResourceString(IDC_CMDS9_ANGLE_OF_X_AXIS__362, "Angle of X axis:" ));
						if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
					}
					endflag=1;
    				break;
				}
    			//If ent is a Trace or Solid                              
				case 10:
				case 8: {
					for (rbcur=erb;rbcur!=NULL;rbcur=rbcur->rbnext){
						if (10==rbcur->restype){
							sds_trans(rbcur->resval.rpoint,&rbent,&rbucs,0,startpoint);
    						cmd_prtpt(startpoint,ResourceString(IDC_CMDS9_FIRST_CORNER__239, "First corner:" ));                      
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    					}
						if (11==rbcur->restype){
							sds_trans(rbcur->resval.rpoint,&rbent,&rbucs,0,startpoint);
							cmd_prtpt(startpoint,ResourceString(IDC_CMDS9_SECOND_CORNER__240, "Second corner:" ));             
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
						}                                                              
						if (12==rbcur->restype){
							ic_ptcpy(tmppt,rbcur->resval.rpoint);
							sds_trans(rbcur->resval.rpoint,&rbent,&rbucs,0,startpoint);
							cmd_prtpt(startpoint,ResourceString(IDC_CMDS9_THIRD_CORNER__241, "Third corner:" ));      
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
						}                                                       
						if (13==rbcur->restype) {                            
							if (icadPointEqual(rbcur->resval.rpoint,tmppt)==0) {
								sds_trans(rbcur->resval.rpoint,&rbent,&rbucs,0,startpoint);
								cmd_prtpt(startpoint,ResourceString(IDC_CMDS9_FOURTH_CORNER__242, "Fourth corner:" ));     
								if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
							}
						}
					}
					if (db_edata(ent1,NULL,NULL,&sa,&ea)==0){
						ea=sa=0.0;
						if (ea>0.0){
							cmd_prtreal(ea,ResourceString(IDC_CMDS9_PERIMETER__243, "Perimeter:" ));
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
							perimeter=ea;
						}
						if (sa>0.0){
							cmd_prtarea(sa,ResourceString(IDC_CMDS9_AREA__244, "Area:" ));
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
							area=sa;
						}
					}
    				endflag=1;
    				break;
				}
    			//If ent is an Text                              
				case 7: {
					ic_assoc(erb,72);
                    tmpint=ic_rbassoc->resval.rint;
                    ic_assoc(erb,73);
					if (3==tmpint || 5==tmpint){
						ic_assoc(erb,10);
						sds_trans(ic_rbassoc->resval.rpoint,&rbent,&rbucs,0,startpoint);
						if (3==tmpint)cmd_prtpt(startpoint,ResourceString(IDC_CMDS9_FIRST_ALIGNMENT__363, "First alignment point:" ));
						else cmd_prtpt(startpoint,ResourceString(IDC_CMDS9_FIRST_FIT_POINT__364, "First fit point:" ));
						if (0==ic_assoc(erb,11)){
							sds_trans(ic_rbassoc->resval.rpoint,&rbent,&rbucs,0,startpoint);
							if (3==tmpint)cmd_prtpt(startpoint,ResourceString(IDC_CMDS9_SECOND_ALIGNMENT_365, "Second alignment point:" ));
							else cmd_prtpt(startpoint,ResourceString(IDC_CMDS9_SECOND_FIT_POINT_366, "Second fit point:" ));
						}
                    }else if (ic_rbassoc->resval.rint||tmpint){
                        if ((ic_assoc(erb,11))==0) {                                     
						    sds_trans(ic_rbassoc->resval.rpoint,&rbent,&rbucs,0,startpoint);
						    cmd_prtpt(startpoint,ResourceString(IDC_CMDS9_INSERTION_POINT__315, "Insertion point:" )); 
						    if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
					    }                                                               
                    }else{
                        if ((ic_assoc(erb,10))==0) {                                     
						    sds_trans(ic_rbassoc->resval.rpoint,&rbent,&rbucs,0,startpoint);
						    cmd_prtpt(startpoint,ResourceString(IDC_CMDS9_INSERTION_POINT__315, "Insertion point:" )); 
						    if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
					    }                                                               
					}
					for (rbcur=erb;rbcur!=NULL;rbcur=rbcur->rbnext){
						if (40==rbcur->restype) {                                     
							cmd_prtreal(rbcur->resval.rreal,ResourceString(IDC_CMDS9_TEXT_HEIGHT__367, "Text height:" )); 
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
						}                                                               
						if (41==rbcur->restype && rbcur->resval.rreal!=1.0) {                                     
							cmd_prtscale(rbcur->resval.rreal,ResourceString(IDC_CMDS9_RELATIVE_X_SCALE_339, "Relative X scale factor:" )); 
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
						}                                                               
						if (1==rbcur->restype) {                                      
							cmd_prtstr(rbcur->resval.rstring,ResourceString(IDC_CMDS9_TEXT_VALUE__368, "Text value:" ));        
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
						}                                                                
						if (50==rbcur->restype) {                                    
							tmppt[0]=1.0;
							tmppt[1]=tmppt[2]=0.0;
							sds_trans(tmppt,&rbucs,&rbent,1,tmppt);
							fr1=atan2(tmppt[1],tmppt[0]);
							ic_normang(&fr1,&rbcur->resval.rreal);
							cmd_prtangle(rbcur->resval.rreal-fr1,ResourceString(IDC_CMDS9_ROTATION_ANGLE__319, "Rotation angle:" ));     
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
						}                                                             
						if (51==rbcur->restype && !icadRealEqual(rbcur->resval.rreal,0.0)) {                                     
							cmd_prtangle(rbcur->resval.rreal,ResourceString(IDC_CMDS9_OBLIQUE_ANGLE__340, "Oblique angle:" ));    
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
						} 
						//7 moved to top of f'n
						//if (7==rbcur->restype) {                                          
						//	cmd_prtstr(rbcur->resval.rstring,"Style:");          
						//	if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
						//}                                                                    
						if (71==rbcur->restype) {                                   
                            ic_assoc(erb,7);
                            struct resbuf *tmprb=NULL;
                            if ((tmprb=sds_tblsearch("STYLE"/*DNT*/,ic_rbassoc->resval.rstring,0))!=NULL){
                                ic_assoc(tmprb,70);
                                if (ic_rbassoc->resval.rint&4){
                                    if (rbcur->resval.rint!=0){
                                        if (rbcur->resval.rint&2){
							                if (rbcur->resval.rint&4){
                                                cmd_prtstr(ResourceString(IDC_CMDS9_BACKWARD__UPSIDE_342, "Backward ,Upside-down and Vertical" ),ResourceString(IDC_CMDS9_GENERATION__343, "Generation:" ));         
							                }else{
                                                cmd_prtstr(ResourceString(IDC_CMDS9_BACKWARD_AND_VER_344, "Backward and Vertical" ),ResourceString(IDC_CMDS9_GENERATION__343, "Generation:" ));         
                                            }
                                        }else{
							                if (rbcur->resval.rint&4){
    								            cmd_prtstr(ResourceString(IDC_CMDS9_UPSIDE_DOWN_AND__345, "Upside-down and Vertical" ),ResourceString(IDC_CMDS9_GENERATION__343, "Generation:" ));      
                                            }
                                        }
							        }else{	
                               	        cmd_prtstr(ResourceString(IDC_CMDS9_VERTICAL_346, "Vertical" ),ResourceString(IDC_CMDS9_GENERATION__343, "Generation:" ));    
							        }
                                }else{
                                    if (rbcur->resval.rint!=0){
                                        if (rbcur->resval.rint&2){
							                if (rbcur->resval.rint&4){
                                                cmd_prtstr(ResourceString(IDC_CMDS9_BACKWARD_AND_UPS_347, "Backward and Upside-down" ),ResourceString(IDC_CMDS9_GENERATION__343, "Generation:" ));         
							                }else{
                                                cmd_prtstr(ResourceString(IDC_CMDS9_BACKWARD_348, "Backward" ),ResourceString(IDC_CMDS9_GENERATION__343, "Generation:" ));         
                                            }
                                        }else{
							                if (rbcur->resval.rint&4){
    								            cmd_prtstr(ResourceString(IDC_CMDS9_UPSIDE_DOWN_349, "Upside-down" ),ResourceString(IDC_CMDS9_GENERATION__343, "Generation:" ));      
                                            }
                                        }
							        }else{	
                               	        cmd_prtstr(ResourceString(IDC_CMDS9_NORMAL_350, "Normal" ),ResourceString(IDC_CMDS9_GENERATION__343, "Generation:" ));    
							        }
							    }
                                if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
						    }
                            IC_RELRB(tmprb);
                        } 
					}
					if (tmpint==3 || tmpint==5)
						tmpint=1;
					else{
						tmpint=0;
						if ((ic_assoc(erb,73))==0) {                                            
							switch(ic_rbassoc->resval.rint) {                                  
								case 0:                                                        
									cmd_prtstr(ResourceString(IDC_CMDS9_BASELINE_351, "Baseline" ),ResourceString(IDC_CMDS9_VERTICAL_ALIGNME_352, "Vertical alignment:" ));                        
									tmpint=1;
									break;                                                     
								case 1:                                                        
									cmd_prtstr(ResourceString(IDC_CMDS9_BOTTOM_353, "Bottom" ),ResourceString(IDC_CMDS9_VERTICAL_ALIGNME_352, "Vertical alignment:" ));                      
									break;                                                     
								case 2:                                                        
									cmd_prtstr(ResourceString(IDC_CMDS9_MIDDLE_354, "Middle" ),ResourceString(IDC_CMDS9_VERTICAL_ALIGNME_352, "Vertical alignment:" ));                    
									break;                                                     
								case 3:                                                        
									cmd_prtstr(ResourceString(IDC_CMDS9_TOP_355, "Top" ),ResourceString(IDC_CMDS9_VERTICAL_ALIGNME_352, "Vertical alignment:" ));                    
									break;         
								default:                                                       
									cmd_prtstr(ResourceString(IDC_CMDS9_BASELINE_351, "Baseline" ),ResourceString(IDC_CMDS9_VERTICAL_ALIGNME_352, "Vertical alignment:" ));                        
									tmpint=1;
									break;                                                     
							}                                                                  
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
						}  
					}	
					if ((ic_assoc(erb,72))==0) {                                          
						switch(ic_rbassoc->resval.rint) {                                
							case 0:                                                      
								cmd_prtstr(ResourceString(IDC_CMDS9_LEFT_356, "Left" ),ResourceString(IDC_CMDS9_HORIZONTAL_ALIGN_357, "Horizontal alignment:" ));                      
								break;                                                   
							case 1:                                                      
								cmd_prtstr(ResourceString(IDC_CMDS9_CENTER_358, "Center" ),ResourceString(IDC_CMDS9_HORIZONTAL_ALIGN_357, "Horizontal alignment:" ));                    
								break;                                                   
							case 2:                                                      
								cmd_prtstr(ResourceString(IDC_CMDS9_RIGHT_359, "Right" ),ResourceString(IDC_CMDS9_HORIZONTAL_ALIGN_357, "Horizontal alignment:" ));                  
								break;                                                   
							case 3:                                              
								if (tmpint==1) {
									cmd_prtstr(ResourceString(IDC_CMDS9_ALIGNED_369, "Aligned" ),ResourceString(IDC_CMDS9_HORIZONTAL_ALIGN_357, "Horizontal alignment:" )); }    
								break;                                           
							case 4:                                              
								if (tmpint==1) {
									cmd_prtstr(ResourceString(IDC_CMDS9_MIDDLE_354, "Middle" ),ResourceString(IDC_CMDS9_HORIZONTAL_ALIGN_357, "Horizontal alignment:" )); }    
								break;                                           
							case 5:                                              
								if (tmpint==1) {
									cmd_prtstr(ResourceString(IDC_CMDS9_FITTED_370, "Fitted" ),ResourceString(IDC_CMDS9_HORIZONTAL_ALIGN_357, "Horizontal alignment:" )); }    
								break;                                           
							default:                                                     
								cmd_prtstr(ResourceString(IDC_CMDS9_LEFT_356, "Left" ),ResourceString(IDC_CMDS9_HORIZONTAL_ALIGN_357, "Horizontal alignment:" ));                      
								break;                                                   
						}                                                                
						if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
					}                                                                    
					endflag=1;
					break;
				}
    			//If ent is an AttDef                              
				case 14: {
					ic_assoc(erb,72);
                    tmpint=ic_rbassoc->resval.rint;
                    ic_assoc(erb,74);
					if (3==tmpint || 5==tmpint){
						ic_assoc(erb,10);
						sds_trans(ic_rbassoc->resval.rpoint,&rbent,&rbucs,0,startpoint);
						if (3==tmpint)cmd_prtpt(startpoint,ResourceString(IDC_CMDS9_FIRST_ALIGNMENT__363, "First alignment point:" ));
						else cmd_prtpt(startpoint,ResourceString(IDC_CMDS9_FIRST_FIT_POINT__364, "First fit point:" ));
						if (0==ic_assoc(erb,11)){
							sds_trans(ic_rbassoc->resval.rpoint,&rbent,&rbucs,0,startpoint);
							if (3==tmpint)cmd_prtpt(startpoint,ResourceString(IDC_CMDS9_SECOND_ALIGNMENT_365, "Second alignment point:" ));
							else cmd_prtpt(startpoint,ResourceString(IDC_CMDS9_SECOND_FIT_POINT_366, "Second fit point:" ));
						}
                    }else if (ic_rbassoc->resval.rint||tmpint){
                        if ((ic_assoc(erb,11))==0) {                                     
						    sds_trans(ic_rbassoc->resval.rpoint,&rbent,&rbucs,0,startpoint);
						    cmd_prtpt(startpoint,ResourceString(IDC_CMDS9_INSERTION_POINT__315, "Insertion point:" )); 
						    if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
					    }                                                               
                    }else{
                        if ((ic_assoc(erb,10))==0) {                                     
						    sds_trans(ic_rbassoc->resval.rpoint,&rbent,&rbucs,0,startpoint);
						    cmd_prtpt(startpoint,ResourceString(IDC_CMDS9_INSERTION_POINT__315, "Insertion point:" )); 
						    if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
					    }                                                               
					}
					for (rbcur=erb;rbcur!=NULL;rbcur=rbcur->rbnext){
						if (40==rbcur->restype) {                                     
							cmd_prtreal(rbcur->resval.rreal,ResourceString(IDC_CMDS9_TEXT_HEIGHT__367, "Text height:" )); 
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
						}                                                               
						if (2==rbcur->restype) {                               
							cmd_prtstr(rbcur->resval.rstring,ResourceString(IDC_CMDS9_TAG__331, "Tag:" ));    
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
						}                                                        
						if (3==rbcur->restype) {  
							//Modified Cybage AW 17-09-01 [
							//Reason : For Constant (Fixed) attribute flag, Promt should be empty in the listing
							//Bug # 77844 from Bugzilla	
							struct resbuf *buf = erb;
							ic_assoc(buf, 70);
							if(ic_rbassoc->resval.rint & 2)
								cmd_prtstr("",ResourceString(IDC_CMDS9_PROMPT__371, "Prompt:" ));
							else	//Modified Cybage AW 17-09-01 ]
								cmd_prtstr(rbcur->resval.rstring,ResourceString(IDC_CMDS9_PROMPT__371, "Prompt:" ));   
							
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
						}
						if (1==rbcur->restype) {  
							cmd_prtstr(rbcur->resval.rstring,ResourceString(IDC_CMDS9_DEFAULT_VALUE__372, "Default Value:" ));   
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
						}
						//Modified Cybage AW 17-09-01 [
						//Reason : If multiple attribute flags are set, all should get listed
						//in the list command
						//Bug # 77844 from Bugzilla	
						/*
						if (70==rbcur->restype) {   
							if (rbcur->resval.rint & 1) {
								cmd_prtstr(ResourceString(IDC_CMDS9_INVISIBLE_332, "Invisible" ),ResourceString(IDC_CMDS9_ATTRIBUTE_FLAGS__333, "Attribute Flags:" ));                           
								if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
							}else if (rbcur->resval.rint & 2) {
								cmd_prtstr(ResourceString(IDC_CMDS9_CONSTANT_334, "Constant" ),ResourceString(IDC_CMDS9_ATTRIBUTE_FLAGS__333, "Attribute Flags:" ));   
								if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
							}else if (rbcur->resval.rint & 4) {                            
								cmd_prtstr(ResourceString(IDC_CMDS9_VERIFICATION_REQ_335, "Verification required" ),ResourceString(IDC_CMDS9_ATTRIBUTE_FLAGS__333, "Attribute Flags:" ));                 
								if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
							}else if (rbcur->resval.rint & 8) {                              
								cmd_prtstr(ResourceString(IDC_CMDS9_PRESET__NO_PROMP_336, "Preset (no prompting)" ),ResourceString(IDC_CMDS9_ATTRIBUTE_FLAGS__333, "Attribute Flags:" ));    
								if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
							}                                                     
						} 
						*/
						if (70==rbcur->restype) {   
							CString str;
							str.Empty();
							sds_printf("\n%25s"/*DNT*/,ResourceString(IDC_CMDS9_ATTRIBUTE_FLAGS__333, "Attribute Flags:" ));
							if (!(rbcur->resval.rint | 0)) {
								sds_printf("  %s", ResourceString(IDC_CMDS9_NORMAL_350, "Normal" ));
								if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
							}
							if (rbcur->resval.rint & IC_ATTDEF_INVIS) {
								sds_printf("  %s", ResourceString(IDC_CMDS9_INVISIBLE_332, "Invisible" ));
								if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
							}if (rbcur->resval.rint & IC_ATTDEF_CONST) {
								sds_printf("  %s", ResourceString(IDC_CMDS9_CONSTANT_334, "Constant" ));   
								if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
							}if (rbcur->resval.rint & IC_ATTDEF_VERIFY) {                            
								sds_printf("  %s", ResourceString(IDC_CMDS9_VERIFICATION_REQ_335, "Verification required" ));                 
								if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
							}if (rbcur->resval.rint & IC_ATTDEF_PRESET) {                              
								sds_printf("  %s", ResourceString(IDC_CMDS9_PRESET__NO_PROMP_336, "Preset (no prompting)" ));    
								if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
							}                                                     
						}
						//Modified Cybage AW 17-09-01 ]
						if (73==rbcur->restype) {                           
							cmd_prtint(rbcur->resval.rint,ResourceString(IDC_CMDS9_FIELD_LENGTH__337, "Field length:" ));   
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
						} 
						//style moved to top of f'n
						//if (7==rbcur->restype) {                                     
						//	cmd_prtstr(rbcur->resval.rstring,"Text style:");     
						//	if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
						//}                                                               
						if (50==rbcur->restype) {                                
							tmppt[0]=1.0;
							tmppt[1]=tmppt[2]=0.0;
							sds_trans(tmppt,&rbucs,&rbent,1,tmppt);
							fr1=atan2(tmppt[1],tmppt[0]);
							ic_normang(&fr1,&rbcur->resval.rreal);
							cmd_prtangle(rbcur->resval.rreal-fr1,ResourceString(IDC_CMDS9_TEXT_ROTATION__338, "Text rotation:" ));   
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
						}                                                          
						if (41==rbcur->restype && !icadRealEqual(rbcur->resval.rreal,1.0)) {                                   
							cmd_prtscale(rbcur->resval.rreal,ResourceString(IDC_CMDS9_RELATIVE_X_SCALE_339, "Relative X scale factor:" ));   
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
						}                                                             
						if (51==rbcur->restype && !icadRealEqual(rbcur->resval.rreal,0.0)) {                                             
							cmd_prtangle(rbcur->resval.rreal,ResourceString(IDC_CMDS9_OBLIQUE_ANGLE__340, "Oblique angle:" ));   
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
						}                                                                       
						if (71==rbcur->restype/* && 0!=rbcur->resval.rint*/) {                                   
                            ic_assoc(erb,7);
                            struct resbuf *tmprb=NULL;
                            if ((tmprb=sds_tblsearch("STYLE"/*DNT*/,ic_rbassoc->resval.rstring,0))!=NULL){
                                ic_assoc(tmprb,70);
                                if (ic_rbassoc->resval.rint&4){
                                    if (rbcur->resval.rint!=0){
                                        if (rbcur->resval.rint&2){
							                if (rbcur->resval.rint&4){
                                                cmd_prtstr(ResourceString(IDC_CMDS9_BACKWARD__UPSIDE_342, "Backward ,Upside-down and Vertical" ),ResourceString(IDC_CMDS9_GENERATION__343, "Generation:" ));         
							                }else{
                                                cmd_prtstr(ResourceString(IDC_CMDS9_BACKWARD_AND_VER_344, "Backward and Vertical" ),ResourceString(IDC_CMDS9_GENERATION__343, "Generation:" ));         
                                            }
                                        }else{
							                if (rbcur->resval.rint&4){
    								            cmd_prtstr(ResourceString(IDC_CMDS9_UPSIDE_DOWN_AND__345, "Upside-down and Vertical" ),ResourceString(IDC_CMDS9_GENERATION__343, "Generation:" ));      
                                            }
                                        }
							        }else{	
                               	        cmd_prtstr(ResourceString(IDC_CMDS9_VERTICAL_346, "Vertical" ),ResourceString(IDC_CMDS9_GENERATION__343, "Generation:" ));    
							        }
                                }else{
                                    if (rbcur->resval.rint!=0){
                                        if (rbcur->resval.rint&2){
							                if (rbcur->resval.rint&4){
                                                cmd_prtstr(ResourceString(IDC_CMDS9_BACKWARD_AND_UPS_347, "Backward and Upside-down" ),ResourceString(IDC_CMDS9_GENERATION__343, "Generation:" ));         
							                }else{
                                                cmd_prtstr(ResourceString(IDC_CMDS9_BACKWARD_348, "Backward" ),ResourceString(IDC_CMDS9_GENERATION__343, "Generation:" ));         
                                            }
                                        }else{
							                if (rbcur->resval.rint&4){
    								            cmd_prtstr(ResourceString(IDC_CMDS9_UPSIDE_DOWN_349, "Upside-down" ),ResourceString(IDC_CMDS9_GENERATION__343, "Generation:" ));      
                                            }
                                        }
							        }else{	
                               	        cmd_prtstr(ResourceString(IDC_CMDS9_NORMAL_350, "Normal" ),ResourceString(IDC_CMDS9_GENERATION__343, "Generation:" ));    
							        }
							    }
                                if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
						    }
                            IC_RELRB(tmprb);
						}   
					}
					tmpint=0;
					if ((ic_assoc(erb,74))==0) {                                            
						switch(ic_rbassoc->resval.rint) {                                  
							case 0:                                                        
								cmd_prtstr(ResourceString(IDC_CMDS9_BASELINE_351, "Baseline" ),ResourceString(IDC_CMDS9_VERTICAL_ALIGNME_352, "Vertical alignment:" ));                        
								tmpint=1;
								break;                                                     
							case 1:                                                        
								cmd_prtstr(ResourceString(IDC_CMDS9_BOTTOM_353, "Bottom" ),ResourceString(IDC_CMDS9_VERTICAL_ALIGNME_352, "Vertical alignment:" ));                      
								break;                                                     
							case 2:                                                        
								cmd_prtstr(ResourceString(IDC_CMDS9_MIDDLE_354, "Middle" ),ResourceString(IDC_CMDS9_VERTICAL_ALIGNME_352, "Vertical alignment:" ));                    
								break;                                                     
							case 3:                                                        
								cmd_prtstr(ResourceString(IDC_CMDS9_TOP_355, "Top" ),ResourceString(IDC_CMDS9_VERTICAL_ALIGNME_352, "Vertical alignment:" ));                    
								break;         
							default:                                                       
								cmd_prtstr(ResourceString(IDC_CMDS9_BASELINE_351, "Baseline" ),ResourceString(IDC_CMDS9_VERTICAL_ALIGNME_352, "Vertical alignment:" ));                        
								tmpint=1;
								break;                                                     
						}                                                                  
						if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
					}                                                                      
					if ((ic_assoc(erb,72))==0) {                                          
						switch(ic_rbassoc->resval.rint) {                                
							case 0:                                                      
								cmd_prtstr(ResourceString(IDC_CMDS9_LEFT_356, "Left" ),ResourceString(IDC_CMDS9_HORIZONTAL_ALIGN_357, "Horizontal alignment:" ));                      
								break;                                                   
							case 1:                                                      
								cmd_prtstr(ResourceString(IDC_CMDS9_CENTER_358, "Center" ),ResourceString(IDC_CMDS9_HORIZONTAL_ALIGN_357, "Horizontal alignment:" ));                    
								break;                                                   
							case 2:                                                      
								cmd_prtstr(ResourceString(IDC_CMDS9_RIGHT_359, "Right" ),ResourceString(IDC_CMDS9_HORIZONTAL_ALIGN_357, "Horizontal alignment:" ));                  
								break;
							//Modified Cybage AW 24-12-01 [
							//Reason : Wrong Prompts Displayed for attribute justification in the list command
							//Bug # 77951 from Bugzilla
							/*	
							case 3:                                              
								if (tmpint==1) {
									cmd_prtstr(ResourceString(IDC_CMDS9_RIGHT_359, "Right" ),ResourceString(IDC_CMDS9_HORIZONTAL_ALIGN_357, "Horizontal alignment:" )); }    
								break;                                           
							case 4:                                              
								if (tmpint==1) {
									cmd_prtstr(ResourceString(IDC_CMDS9_RIGHT_359, "Right" ),ResourceString(IDC_CMDS9_HORIZONTAL_ALIGN_357, "Horizontal alignment:" )); }    
								break;                                           
							case 5:                                              
								if (tmpint==1) {
									cmd_prtstr(ResourceString(IDC_CMDS9_RIGHT_359, "Right" ),ResourceString(IDC_CMDS9_HORIZONTAL_ALIGN_357, "Horizontal alignment:" )); }    
								break;                                           
							*/
							case 3:                                              
								if (tmpint==1) {
									cmd_prtstr(ResourceString(IDC_CMDS9_ALIGNED_369, "Aligned" ),ResourceString(IDC_CMDS9_HORIZONTAL_ALIGN_357, "Horizontal alignment:" )); }    
								break;                                           
							case 4:                                              
								if (tmpint==1) {
									cmd_prtstr(ResourceString(IDC_CMDS9_MIDDLE_354, "Middle" ),ResourceString(IDC_CMDS9_HORIZONTAL_ALIGN_357, "Horizontal alignment:" )); }    
								break;                                           
							case 5:                                              
								if (tmpint==1) {
									cmd_prtstr(ResourceString(IDC_CMDS9_FITTED_370, "Fitted" ),ResourceString(IDC_CMDS9_HORIZONTAL_ALIGN_357, "Horizontal alignment:" )); }    
								break;
							//Modified Cybage AW 24-12-01 ]
							default:                                                     
								cmd_prtstr(ResourceString(IDC_CMDS9_LEFT_356, "Left" ),ResourceString(IDC_CMDS9_HORIZONTAL_ALIGN_357, "Horizontal alignment:" ));                      
								break;                                                   
						}                                                                
						if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
					}                                                                    
    				endflag=1;
					break;
				} //case 14
				case 15: {
					if (strsame(etype,"VIEWPORT"/*DNT*/ )){
    					if (ic_assoc(erb,10)==0) {
    						cmd_prtpt(ic_rbassoc->resval.rpoint,ResourceString(IDC_CMDS9_CENTER_POINT__255, "Center point:" ));
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    					}
    					if (ic_assoc(erb,40)==0) {
    						cmd_prtreal(ic_rbassoc->resval.rreal,ResourceString(IDC_CMDS9_WIDTH__373, "Width:" ));
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    					}
    					if (ic_assoc(erb,41)==0) {
    						cmd_prtreal(ic_rbassoc->resval.rreal,ResourceString(IDC_CMDS9_HEIGHT__374, "Height:" ));
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    					}
						if ((ic_assoc(erb,69))==0) {                           
							cmd_prtint(ic_rbassoc->resval.rint,ResourceString(IDC_CMDS9_ID__375, "ID:" ));   
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
						}                                                    
						if ((ic_assoc(erb,68))==0) {                                            
							switch(ic_rbassoc->resval.rint) {                                  
								case 0:                                                        
									cmd_prtstr(ResourceString(IDC_CMDS9_OFF_376, "Off" ),ResourceString(IDC_CMDS9_VIEWPORT_DISPLAY_377, "Viewport display:" ));                        
									break;                                                     
								default:                                                        
									cmd_prtstr(ResourceString(IDC_CMDS9_ON_378, "On" ),ResourceString(IDC_CMDS9_VIEWPORT_DISPLAY_377, "Viewport display:" ));                      
									break;                                                     
							}                                                                  
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
						}   
					}
	   				endflag=1;
					break;
				} //case 15	
				case 16:
                {
					//<alm>
					if (CModeler::get()->canView())
					{
						// 3DSOLID
						CDbAcisEntity* pObj = reinterpret_cast<CDbAcisEntity*>(ent1[0]);
						point high, low;
						if (CModeler::get()->getBoundingBox(pObj->getData(), low, high, wcs2ucs()))
						{
							cmd_prtstr("", ResourceString(IDC_CMDS9_BOUNDINGBOX, "Bounding Box:"));
							cmd_prtpt(low, ResourceString(IDC_CMDS9_LOWERBOUND, "Lower Bound:"));
							cmd_prtpt(high, ResourceString(IDC_CMDS9_HIGHERBOUND, "Upper Bound:"));
						}
    				}
					//</alm>
	   				endflag=1;
					break;
				} //case 16	
				case 17: {
     				//**Start Point
    				if (ic_assoc(erb,10)==0) {
						sds_trans(ic_rbassoc->resval.rpoint,&rbwcs,&rbucs,0,ic_rbassoc->resval.rpoint);
    					cmd_prtpt(ic_rbassoc->resval.rpoint,ResourceString(IDC_CMDS9_START_POINT__302, "Start point:" ));
						if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    				}
					if (strsame(etype,"MLINE"/*DNT*/)){
                        //**Justification 
                        if ((ic_assoc(erb,70))==0) {                                            
						    switch(ic_rbassoc->resval.rint) {                                  
							    case 0:                                                        
								    cmd_prtstr(ResourceString(IDC_CMDS9_TOP_355, "Top" ),ResourceString(IDC_CMDS9_JUSTIFICATION__382, "Justification:" ));                        
							        break;
							    case 1:                                                        
								    cmd_prtstr(ResourceString(IDC_CMDS9_ZERO_383, "Zero" ),ResourceString(IDC_CMDS9_JUSTIFICATION__382, "Justification:" ));                        
							        break;
							    case 2:                                                        
								    cmd_prtstr(ResourceString(IDC_CMDS9_BOTTOM_353, "Bottom" ),ResourceString(IDC_CMDS9_JUSTIFICATION__382, "Justification:" ));                        
							        break;
                            }                                                                  
						    if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
					    }                                                                      
                        //**Open or Closed 
                        if ((ic_assoc(erb,71))==0) {                                            
						    switch(ic_rbassoc->resval.rint) {                                  
							    case 1:                                                        
								    cmd_prtstr(ResourceString(IDC_CMDS9_OPEN_267, "Open" ),ResourceString(IDC_CMDS9_OPEN_OR_CLOSED__384, "Open or closed:" ));                        
							        break;
							    case 3:                                                        
								    cmd_prtstr(ResourceString(IDC_CMDS9_CLOSED_266, "Closed" ),ResourceString(IDC_CMDS9_OPEN_OR_CLOSED__384, "Open or closed:" ));                        
							        break;
                            }                                                                  
						    if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
					    }                                                                      
                        //**Scale 
                        if ((ic_assoc(erb,40))==0) {                                            
    					    cmd_prtscale(ic_rbassoc->resval.rreal,ResourceString(IDC_CMDS9_SCALE_FACTOR__385, "Scale factor:" ));
						    if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
					    }                                                                      
                        //**Style 
                        if ((ic_assoc(erb,2))==0) {                                            
    					    cmd_prtstr(ic_rbassoc->resval.rstring,ResourceString(IDC_CMDS9_STYLE__386, "Style:" ));
						    if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
					    }                                                                      
    					tmpint=-1;
                        for (rbcur=erb;rbcur!=NULL;rbcur=rbcur->rbnext){
                            switch(rbcur->restype){
                                case 11:
     				                //**Vertex Point
					                sds_trans(rbcur->resval.rpoint,&rbwcs,&rbucs,0,rbcur->resval.rpoint);
                                    tmpint++;
   					                sprintf(tmpstr,ResourceString(IDC_CMDS9_VERTEX__I__387, "Vertex %i:" ),tmpint);
                                    cmd_prtpt(rbcur->resval.rpoint,tmpstr);
					                if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
                                    break;
                                case 12:
     				                //**Direction vector of segment
					                sds_trans(rbcur->resval.rpoint,&rbwcs,&rbucs,1,rbcur->resval.rpoint);
   					                sprintf(tmpstr,ResourceString(IDC_CMDS9_SEGMENT_VECTOR___388, "Segment vector %i:" ),tmpint);
                                    cmd_prtpt(rbcur->resval.rpoint,tmpstr);
					                if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
                                    break;
                                case 13:
     				                //**Direction vector of miter 
					                sds_trans(rbcur->resval.rpoint,&rbwcs,&rbucs,1,rbcur->resval.rpoint);
   					                sprintf(tmpstr,ResourceString(IDC_CMDS9_MITER_VECTOR__I__389, "Miter vector %i:" ),tmpint);
                                    cmd_prtpt(rbcur->resval.rpoint,tmpstr);
					                if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
                                    break;
                                case 75:
                                    tmpint2=rbcur->resval.rint;
                                    while(tmpint2!=0){
                                        rbcur=rbcur->rbnext;
    					                cmd_prtarea(rbcur->resval.rreal,ResourceString(IDC_CMDS9_AREA_FILL_PARAME_390, "Area fill parameter:" ));
					                    if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
                                        tmpint2--;    
                                    }
                            }
                        }
					}
	   				endflag=1;
					break;
				} //case 17	
				case 18: {
					if (strsame(etype,"MTEXT"/*DNT*/ )){
     				    //**Location Point
    				    if (ic_assoc(erb,10)==0) {
						    sds_trans(ic_rbassoc->resval.rpoint,&rbwcs,&rbucs,0,ic_rbassoc->resval.rpoint);
    					    cmd_prtpt(ic_rbassoc->resval.rpoint,ResourceString(IDC_CMDS9_LOCATION__298, "Location:" ));
						    if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    				    }
                        //**Width
    				    if (ic_assoc(erb,41)==0) {
    					    cmd_prtreal(ic_rbassoc->resval.rreal,ResourceString(IDC_CMDS9_WIDTH__373, "Width:" ));
						    if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    				    }
    				    //**Normal
//                        if (ic_assoc(erb,210)==0) {
//						    sds_trans(ic_rbassoc->resval.rpoint,&rbwcs,&rbucs,1,ic_rbassoc->resval.rpoint);
//   					    cmd_prtpt(ic_rbassoc->resval.rpoint,"Normal:");
//						    if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
//    				    }
                        //**Rotation
    				    if (ic_assoc(erb,50)==0) {
    					    cmd_prtangle(ic_rbassoc->resval.rreal,ResourceString(IDC_CMDS9_ROTATION__391, "Rotation:" ));
						    if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    				    }
                        //**Height
    				    if (ic_assoc(erb,40)==0) {
    					    cmd_prtreal(ic_rbassoc->resval.rreal,ResourceString(IDC_CMDS9_HEIGHT__374, "Height:" ));
						    if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    				    }
                        //**Attachment point
					    if ((ic_assoc(erb,71))==0) {                                            
						    switch(ic_rbassoc->resval.rint) {                                  
							    case 1:                                                        
								    cmd_prtstr(ResourceString(IDC_CMDS9_TOP_LEFT_392, "Top left" ),ResourceString(IDC_CMDS9_ATTACHMENT_POINT_393, "Attachment point:" ));                        
							        break;
                                case 2:                                                        
								    cmd_prtstr(ResourceString(IDC_CMDS9_TOP_CENTER_394, "Top center" ),ResourceString(IDC_CMDS9_ATTACHMENT_POINT_393, "Attachment point:" ));                        
							        break;
							    case 3:                                                        
								    cmd_prtstr(ResourceString(IDC_CMDS9_TOP_RIGHT_395, "Top right" ),ResourceString(IDC_CMDS9_ATTACHMENT_POINT_393, "Attachment point:" ));                        
							        break;
							    case 4:                                                        
								    cmd_prtstr(ResourceString(IDC_CMDS9_MIDDLE_LEFT_396, "Middle left" ),ResourceString(IDC_CMDS9_ATTACHMENT_POINT_393, "Attachment point:" ));                        
							        break;
							    case 5:                                                        
								    cmd_prtstr(ResourceString(IDC_CMDS9_MIDDLE_CENTER_397, "Middle center" ),ResourceString(IDC_CMDS9_ATTACHMENT_POINT_393, "Attachment point:" ));                        
							        break;
							    case 6:                                                        
								    cmd_prtstr(ResourceString(IDC_CMDS9_MIDDLE_RIGHT_398, "Middle right" ),ResourceString(IDC_CMDS9_ATTACHMENT_POINT_393, "Attachment point:" ));                        
							        break;
							    case 7:                                                        
								    cmd_prtstr(ResourceString(IDC_CMDS9_BOTTOM_LEFT_399, "Bottom left" ),ResourceString(IDC_CMDS9_ATTACHMENT_POINT_393, "Attachment point:" ));                        
							        break;
							    case 8:                                                        
								    cmd_prtstr(ResourceString(IDC_CMDS9_BOTTOM_CENTER_400, "Bottom center" ),ResourceString(IDC_CMDS9_ATTACHMENT_POINT_393, "Attachment point:" ));                        
							        break;
							    case 9:                                                        
								    cmd_prtstr(ResourceString(IDC_CMDS9_BOTTOM_RIGHT_401, "Bottom right" ),ResourceString(IDC_CMDS9_ATTACHMENT_POINT_393, "Attachment point:" ));                        
							        break;
                            }                                                                  
						    if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
					    }                                                                      
                        //**Flow direction
					    if ((ic_assoc(erb,72))==0) {                                            
						    switch(ic_rbassoc->resval.rint) {                                  
							    case 1:                                                        
								    cmd_prtstr(ResourceString(IDC_CMDS9_LEFT_TO_RIGHT_402, "Left to right" ),ResourceString(IDC_CMDS9_FLOW_DIRECTION__403, "Flow direction:" ));                        
							        break;
							    case 2:                                                        
								    cmd_prtstr(ResourceString(IDC_CMDS9_RIGHT_TO_LEFT_404, "Right to left" ),ResourceString(IDC_CMDS9_FLOW_DIRECTION__403, "Flow direction:" ));                        
							        break;
							    case 3:                                                        
								    cmd_prtstr(ResourceString(IDC_CMDS9_TOP_TO_BOTTOM_405, "Top to bottom" ),ResourceString(IDC_CMDS9_FLOW_DIRECTION__403, "Flow direction:" ));                        
							        break;
//							    case 4:                                                        
//								    cmd_prtstr(ResourceString(IDC_CMDS9_BOTTOM_TO_TOP_406, "Bottom to top" ),ResourceString(IDC_CMDS9_FLOW_DIRECTION__403, "Flow direction:" ));                        
//							        break;
							    case 5:                                                        
								    cmd_prtstr(ResourceString(IDC_CMDS9_BYSTYLE_407, "ByStyle" ),ResourceString(IDC_CMDS9_FLOW_DIRECTION__403, "Flow direction:" ));                        
							        break;
                            }                                                                  
						    if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
					    }                                                                      
                        //**Text string
					    if ((ic_assoc(erb,1))==0) {                                            
						    cmd_prtstr(ic_rbassoc->resval.rstring,ResourceString(IDC_CMDS9_CONTENTS__408, "Contents:" ));                        
                        }
					}
	   				endflag=1;
					break;
				} //case 18	
				case 19: {
					//<alm>
					if (CModeler::get()->canView())
					{ 
						// REGION
						CDbAcisEntity* pObj = reinterpret_cast<CDbAcisEntity*>(ent1[0]);
						if (CModeler::get()->canModify())
						{
							double area;
							if (CModeler::get()->getArea(pObj->getData(), area))
								cmd_prtarea(area, ResourceString(IDC_CMDS9_AREA__244, "Area:"));
						}

						double perimeter;
						if (CModeler::get()->getPerimeter(pObj->getData(), perimeter))
							cmd_prtreal(perimeter, ResourceString(IDC_CMDS9_PERIMETER__243, "Perimeter:"));

						point high, low;
						if (CModeler::get()->getBoundingBox(pObj->getData(), low, high, wcs2ucs()))
						{
							cmd_prtstr("", ResourceString(IDC_CMDS9_BOUNDINGBOX, "Bounding Box:"));
							cmd_prtpt(low, ResourceString(IDC_CMDS9_LOWERBOUND, "Lower Bound:"));
							cmd_prtpt(high, ResourceString(IDC_CMDS9_HIGHERBOUND, "Upper Bound:"));
						}
    				}
					//</alm>
	   				endflag=1;
					break;
				} //case 19	
				case 20: {
                    if (strsame(etype,"SPLINE"/*DNT*/ )){
						char szTmp[IC_ACADBUF];
                        //Degree
                        for (rbcur=erb;rbcur!=NULL&&rbcur->restype!=71;rbcur=rbcur->rbnext);
			            if (rbcur){
                            cmd_prtint(rbcur->resval.rint+1,ResourceString(IDC_CMDS9_ORDER__409, "Order:" ));
					        if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
                        }
						//Properties
                        for (rbcur=erb;rbcur!=NULL&&rbcur->restype!=70;rbcur=rbcur->rbnext);
                        if (rbcur){ 
                            *szTmp='\0';
                            if (rbcur->resval.rint&1){
						        strcat(szTmp,ResourceString(IDC_CMDS9_CLOSED__410, "Closed," ));
                            }else{	
						        strcat(szTmp,ResourceString(IDC_CMDS9_OPEN__411, "Open," ));
                            }    
                            if (rbcur->resval.rint&2){
						        strcat(szTmp,ResourceString(IDC_CMDS9_PERIODIC__412, "Periodic," ));
                            }else{	
						        strcat(szTmp,ResourceString(IDC_CMDS9_NON_PERIODIC__413, "Non-Periodic," ));
                            }    
                            if (rbcur->resval.rint&4){
						        strcat(szTmp,ResourceString(IDC_CMDS9_RATIONAL__414, "Rational," ));
                            }else{	
						        strcat(szTmp,ResourceString(IDC_CMDS9_NON_RATIONAL__415, "Non-Rational," ));
                            }    
                            if (rbcur->resval.rint&8){
						        strcat(szTmp,ResourceString(IDC_CMDS9_PLANAR__416, "Planar," ));
                            }else{	
						        strcat(szTmp,ResourceString(IDC_CMDS9_NON_PLANAR__417, "Non-Planar," ));
                            }    
                            if (rbcur->resval.rint&16){
						        strcat(szTmp,ResourceString(IDC_CMDS9_LINEAR_418, "Linear" ));
                            }else{	
						        strcat(szTmp,ResourceString(IDC_CMDS9_NON_LINEAR_419, "Non-Linear" ));
                            }    
                            cmd_prtstr(szTmp,ResourceString(IDC_CMDS9_PROPERTIES__420, "Properties:" )); 
					        if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
                        }                     
                        //Knots
                        for (rbcur=erb;rbcur!=NULL&&rbcur->restype!=72;rbcur=rbcur->rbnext);
			            if (rbcur){
                            cmd_prtint(rbcur->resval.rint,ResourceString(IDC_CMDS9_NUMBER_OF_KNOTS__421, "Number of knots:" ));
					        if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
                        }
                        if (rbcur->resval.rint>0){
                            for (rbcur=erb;rbcur!=NULL&&rbcur->restype!=40;rbcur=rbcur->rbnext);
                            while(rbcur){
                                cmd_prtreal(rbcur->resval.rreal,ResourceString(IDC_CMDS9_KNOT_VALUE__422, "Knot value:" ));
					            if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
                                for (rbcur=rbcur->rbnext;rbcur!=NULL&&rbcur->restype!=40;rbcur=rbcur->rbnext);
                            }
                            for (rbcur=erb;rbcur!=NULL&&rbcur->restype!=42;rbcur=rbcur->rbnext);
                            if (rbcur){
                                cmd_prtreal(rbcur->resval.rreal,ResourceString(IDC_CMDS9_KNOT_TOLERANCE__423, "Knot tolerance:" ));
					            if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
                            }
                        }
                        //Controls
                        for (rbcur=erb;rbcur!=NULL&&rbcur->restype!=73;rbcur=rbcur->rbnext);
                        if (rbcur){
    			            cmd_prtint(rbcur->resval.rint,ResourceString(IDC_CMDS9_NUMBER_OF_CONTRO_424, "Number of control points:" ));
	    				    if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
                        }
                        if (rbcur->resval.rint>0){
                            for (rbcur=erb;rbcur!=NULL&&rbcur->restype!=10;rbcur=rbcur->rbnext);
                            while(rbcur){
						        sds_trans(rbcur->resval.rpoint,&rbwcs,&rbucs,0,rbcur->resval.rpoint);
    					        cmd_prtpt(rbcur->resval.rpoint,ResourceString(IDC_CMDS9_CONTROL_POINT__425, "Control point:" ));
					            if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
                                for (rbcur=rbcur->rbnext;rbcur!=NULL&&rbcur->restype!=10;rbcur=rbcur->rbnext);
                            }
                            for (rbcur=erb;rbcur!=NULL&&rbcur->restype!=43;rbcur=rbcur->rbnext);
                            if (rbcur){
                                cmd_prtreal(rbcur->resval.rreal,ResourceString(IDC_CMDS9_CONTROL_TOLERANC_426, "Control tolerance:" ));
	    				        if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
                            }
                        }
                        //Fit
                        for (rbcur=erb;rbcur!=NULL&&rbcur->restype!=74;rbcur=rbcur->rbnext);
                        if (rbcur){
    			            cmd_prtint(rbcur->resval.rint,ResourceString(IDC_CMDS9_NUMBER_OF_FIT_PO_427, "Number of fit points:" ));
	    				    if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
                        }
                        if (rbcur->resval.rint>0){
                            for (rbcur=erb;rbcur!=NULL&&rbcur->restype!=11;rbcur=rbcur->rbnext);
                            while(rbcur){
						        sds_trans(rbcur->resval.rpoint,&rbwcs,&rbucs,0,rbcur->resval.rpoint);
    					        cmd_prtpt(rbcur->resval.rpoint,ResourceString(IDC_CMDS9_FIT_POINT__428, "Fit point:" ));
					            if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
                                for (rbcur=rbcur->rbnext;rbcur!=NULL&&rbcur->restype!=11;rbcur=rbcur->rbnext);
                            }
                            for (rbcur=erb;rbcur!=NULL&&rbcur->restype!=44;rbcur=rbcur->rbnext);
                            if (rbcur){
                                cmd_prtreal(rbcur->resval.rreal,ResourceString(IDC_CMDS9_FIT_TOLERANCE__429, "Fit tolerance:" ));
				    	        if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
                            }
					    }
                    }
	   				endflag=1;
					break;
				} //case 20	
				case 21: {
					//<alm>
					if (CModeler::get()->canView())
					{
						// BODY
						CDbAcisEntity* pObj = reinterpret_cast<CDbAcisEntity*>(ent1[0]);
						point high, low;
						if (CModeler::get()->getBoundingBox(pObj->getData(), low, high, wcs2ucs()))
						{
							cmd_prtstr("", ResourceString(IDC_CMDS9_BOUNDINGBOX, "Bounding Box:"));
							cmd_prtpt(low, ResourceString(IDC_CMDS9_LOWERBOUND, "Lower Bound:"));
							cmd_prtpt(high, ResourceString(IDC_CMDS9_HIGHERBOUND, "Upper Bound:"));
						}
    				}
					//</alm>
	   				endflag=1;
					break;
				} //case 21	
				case 22: {
					if (strsame(etype,"TOLERANCE"/*DNT*/ )){
                        //Bugzilla No. 78060 ; 25-03-2002 [
						//**Location
						if (ic_assoc(erb,10)==0) {
    						cmd_prtpt(ic_rbassoc->resval.rpoint,ResourceString(IDC_CMDS9_LOCATION__298, "Location:" ));
							if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))
								goto bail;
    					}
						//**Text string
					    if ((ic_assoc(erb,1))==0) {                                            
						    cmd_prtstr(ic_rbassoc->resval.rstring,ResourceString(IDC_CMDS9_TEXT__174, "Text:" ));                        
                        }
						//**Dimension Style
						if ((ic_assoc(erb,3))==0) {                                            
    					    cmd_prtstr(ic_rbassoc->resval.rstring,ResourceString(IDC_CMDS9_DIMSTYLE__430, "DimStyle:" ));
						    if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
					    }						
						//**Dimension Overrides
						if ( RTNORM != cmd_print_overrides(ent1,scrollcur,scrollmax) )
							goto bail; 
						//Bugzilla No.78060 ;25-03-2002 ]                                                                      
					}
	   				endflag=1;
					break;
				} //case 22	
				case 23: {
					if (strsame(etype,"OLE2FRAME"/*DNT*/ )){
					    for (rbcur=erb;rbcur!=NULL;rbcur=rbcur->rbnext){
						    if (10==rbcur->restype){
							    sds_trans(rbcur->resval.rpoint,&rbent,&rbucs,0,startpoint);
    						    cmd_prtpt(startpoint,ResourceString(IDC_CMDS9_UPPER_LEFT_CORNE_431, "Upper left corner:" ));                      
							    if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
    					    }
						    if (11==rbcur->restype){
							    sds_trans(rbcur->resval.rpoint,&rbent,&rbucs,0,startpoint);
							    cmd_prtpt(startpoint,ResourceString(IDC_CMDS9_LOWER_RIGHT_CORN_432, "Lower right corner:" ));             
							    if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
						    }                                                              
						    if (3==rbcur->restype){
							    cmd_prtstr(rbcur->resval.rstring,ResourceString(IDC_CMDS9_USER_TYPE__433, "User type:" ));             
							    if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
						    }                                                              
						    if (71==rbcur->restype){
							    if (rbcur->resval.rint==0){
                                    cmd_prtstr(ResourceString(IDC_CMDS9_LINK_434, "Link" ),ResourceString(IDC_CMDS9_OLE_OBJECT_TYPE__435, "OLE Object Type:" ));             
                                }else if (rbcur->resval.rint==1){
                                    cmd_prtstr(ResourceString(IDC_CMDS9_EMBEDDED_436, "Embedded" ),ResourceString(IDC_CMDS9_OLE_OBJECT_TYPE__435, "OLE Object Type:" ));             
                                }else if (rbcur->resval.rint==2){
                                    cmd_prtstr(ResourceString(IDC_CMDS9_STATIC_437, "Static" ),ResourceString(IDC_CMDS9_OLE_OBJECT_TYPE__435, "OLE Object Type:" ));             
                                }
							    if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))goto bail;
						    }                                                              
					    }
					}
	   				endflag=1;
					break;
				} //case 23	
				} //switch
    		}//** While	
			if (erb!=NULL){sds_relrb(erb);erb=NULL;}
		} //** End For
		//If it ended normaly
		if (!sort)listidx=1;
	}//for each type of entity

    rc=(RTNORM);
bail:
	rb.restype=RTREAL;
	rb.rbnext=NULL;
	if (area>0.0){
		rb.resval.rreal=area;
		sds_setvar("AREA"/*DNT*/,&rb);
	}
	if (perimeter>0.0){
		rb.resval.rreal=perimeter;
		sds_setvar("PERIMETER"/*DNT*/,&rb);
	}
    IC_RELRB(erbx);
    if (erb!=NULL){sds_relrb(erb);erb=NULL;}
	if (erb2!=NULL){sds_relrb(erb2);erb2=NULL;}
	for (i=0L;RTNORM==sds_ssname(ssname,i,ent1);i++){
		sds_redraw(ent1,IC_REDRAW_UNHILITE);
	}
	sds_ssfree(ssname);
//	sds_graphscr();
	return(rc);
}

//Bugzilla No 78004 18-03-2002 [
//This function prints the dimension and leader overrides
int cmd_print_overrides(sds_name &ent1,int scrollcur, int scrollmax)
{

			struct resbuf *rbcur,rbfilter,*erbx; 
			rbcur=erbx=NULL;
			RT	rc=0;
			//*** This will be for altered dims
                    rbfilter.restype=RTSTR;
                    rbfilter.resval.rstring="ACAD"/*DNT*/;
			// terminate this 1-entry list.
			// Code that uses this later walks the list, rather than assuming 1 entry only.
					rbfilter.rbnext=NULL;

					erbx=sds_entgetx(ent1,&rbfilter);
                    for (rbcur=erbx;rbcur!=NULL&&rbcur->restype!=1070;rbcur=rbcur->rbnext);
                    if (rbcur!=NULL) {
                        cmd_prtstr(ResourceString(IDC_CMDS9___176, " " ),ResourceString(IDC_CMDS9_ALTERED_VARIABLE_177, "Altered variables:" ));
                    }else{
                        cmd_prtstr(ResourceString(IDC_CMDS9_NONE_178, "None" ),ResourceString(IDC_CMDS9_ALTERED_VARIABLE_177, "Altered variables:" ));
                    }
				    if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))return rc;
                    for (;rbcur!=NULL;rbcur=rbcur->rbnext){
                        if (rbcur->restype==1070){       
                            switch(rbcur->resval.rint){
				                case   3: 
						            cmd_prtstr(rbcur->rbnext->resval.rstring,ResourceString(IDC_CMDS9_DIMPOST__179, "DIMPOST:" ));
				                    break;
				                case   4: 
						            cmd_prtstr(rbcur->rbnext->resval.rstring,ResourceString(IDC_CMDS9_DIMAPOST__180, "DIMAPOST:" ));
				                    break;
				                case   5: 
						            // Modified Cybage VSB 04/03/2002 [
									// Reason: Fix for the bug 77969 from Bugzilla
									if ( strlen(rbcur->rbnext->resval.rstring) == 0 )
										cmd_prtstr(ResourceString(IDC_COMBO_LDR_ARROWS_4, "Arrow, filled"),ResourceString(IDC_CMDS9_DIMBLK__181, "DIMBLK:" ));
									else
						            cmd_prtstr(rbcur->rbnext->resval.rstring,ResourceString(IDC_CMDS9_DIMBLK__181, "DIMBLK:" ));
									// Modified Cybage VSB 04/03/2002 ]
				                    break;
				                case   6: 
									// Modified Cybage VSB 04/03/2002 [
									// Reason: Fix for the bug 77969 from Bugzilla
									if ( strlen(rbcur->rbnext->resval.rstring) ==  0 )
										cmd_prtstr(ResourceString(IDC_COMBO_LDR_ARROWS_4, "Arrow, filled"),ResourceString(IDC_CMDS9_DIMBLK1__182, "DIMBLK1:" ));
									else
						            cmd_prtstr(rbcur->rbnext->resval.rstring,ResourceString(IDC_CMDS9_DIMBLK1__182, "DIMBLK1:" ));
				                    // Modified Cybage VSB 04/03/2002 ]
				                    break;
				                case   7: 
						            // Modified Cybage VSB 04/03/2002 [
									// Reason: Fix for the bug 77969 from Bugzilla
									if ( strlen(rbcur->rbnext->resval.rstring) ==  0 )
										cmd_prtstr(ResourceString(IDC_COMBO_LDR_ARROWS_4, "Arrow, filled"),ResourceString(IDC_CMDS9_DIMBLK2__183, "DIMBLK2:" ));
									else
						            cmd_prtstr(rbcur->rbnext->resval.rstring,ResourceString(IDC_CMDS9_DIMBLK2__183, "DIMBLK2:" ));
									// Modified Cybage VSB 04/03/2002 ]
				                    break;
				                case  40: 
    				                cmd_prtscale(rbcur->rbnext->resval.rreal,ResourceString(IDC_CMDS9_DIMSCALE__184, "DIMSCALE:" ));
						            break;
				                case  41: 
    				                cmd_prtreal(rbcur->rbnext->resval.rreal,ResourceString(IDC_CMDS9_DIMASZ__185, "DIMASZ:" ));
						            break;
				                case  42: 
    				                cmd_prtreal(rbcur->rbnext->resval.rreal,ResourceString(IDC_CMDS9_DIMEXO__186, "DIMEXO:" ));
						            break;
				                case  43: 
    				                cmd_prtreal(rbcur->rbnext->resval.rreal,ResourceString(IDC_CMDS9_DIMDLI__187, "DIMDLI:" ));
						            break;
				                case  44: 
    				                cmd_prtreal(rbcur->rbnext->resval.rreal,ResourceString(IDC_CMDS9_DIMEXE__188, "DIMEXE:" ));
						            break;
				                case  45: 
    				                cmd_prtreal(rbcur->rbnext->resval.rreal,ResourceString(IDC_CMDS9_DIMRND__189, "DIMRND:" ));
						            break;
				                case  46: 
    				                cmd_prtreal(rbcur->rbnext->resval.rreal,ResourceString(IDC_CMDS9_DIMDLE__190, "DIMDLE:" ));
						            break;
				                case  47: 
    				                cmd_prtreal(rbcur->rbnext->resval.rreal,ResourceString(IDC_CMDS9_DIMTP__191, "DIMTP:" ));
						            break;
				                case  48: 
    				                cmd_prtreal(rbcur->rbnext->resval.rreal,ResourceString(IDC_CMDS9_DIMTM__192, "DIMTM:" ));
						            break;
				                case 140: 
    				                cmd_prtreal(rbcur->rbnext->resval.rreal,ResourceString(IDC_CMDS9_DIMTXT__193, "DIMTXT:" ));
						            break;
				                case 141: 
    				                cmd_prtreal(rbcur->rbnext->resval.rreal,ResourceString(IDC_CMDS9_DIMCEN__194, "DIMCEN:" ));
						            break;
				                case 142: 
    				                cmd_prtreal(rbcur->rbnext->resval.rreal,ResourceString(IDC_CMDS9_DIMTSZ__195, "DIMTSZ:" ));
						            break;
				                case 143: 
    				                cmd_prtreal(rbcur->rbnext->resval.rreal,ResourceString(IDC_CMDS9_DIMALTF__196, "DIMALTF:" ));
						            break;
				                case 144: 
    				                cmd_prtreal(rbcur->rbnext->resval.rreal,ResourceString(IDC_CMDS9_DIMLFAC__197, "DIMLFAC:" ));
						            break;
				                case 145: 
    				                cmd_prtreal(rbcur->rbnext->resval.rreal,ResourceString(IDC_CMDS9_DIMTVP__198, "DIMTVP:" ));
						            break;
				                case 146: 
    				                cmd_prtreal(rbcur->rbnext->resval.rreal,ResourceString(IDC_CMDS9_DIMTFAC__199, "DIMTFAC:" ));
						            break;
				                case 147: 
    				                cmd_prtreal(rbcur->rbnext->resval.rreal,ResourceString(IDC_CMDS9_DIMGAP__200, "DIMGAP:" ));
						            break;
						        // EBATECH(CNBR) -[ 2003/3/31 DIMALTRND
				                case 148: 
    				                cmd_prtreal(rbcur->rbnext->resval.rreal,ResourceString(IDC_CMDS9_DIMALTRND, "DIMALTRND:" ));
						            break;
						        // EBATECH(CNBR) ]-
				                case  71: 
							        cmd_prtint(rbcur->rbnext->resval.rint,ResourceString(IDC_CMDS9_DIMTOL__201, "DIMTOL:" ));
						            break;
				                case  72: 
							        cmd_prtint(rbcur->rbnext->resval.rint,ResourceString(IDC_CMDS9_DIMLIM__202, "DIMLIM:" ));
						            break;
				                case  73: 
							        cmd_prtint(rbcur->rbnext->resval.rint,ResourceString(IDC_CMDS9_DIMTIH__203, "DIMTIH:" ));
						            break;
				                case  74: 
							        cmd_prtint(rbcur->rbnext->resval.rint,ResourceString(IDC_CMDS9_DIMTOH__204, "DIMTOH:" ));
						            break;
				                case  75: 
							        cmd_prtint(rbcur->rbnext->resval.rint,ResourceString(IDC_CMDS9_DIMSE1__205, "DIMSE1:" ));
						            break;
				                case  76: 
							        cmd_prtint(rbcur->rbnext->resval.rint,ResourceString(IDC_CMDS9_DIMSE2__206, "DIMSE2:" ));
						            break;
				                case  77: 
							        cmd_prtint(rbcur->rbnext->resval.rint,ResourceString(IDC_CMDS9_DIMTAD__207, "DIMTAD:" ));
						            break;
				                case  78: 
							        cmd_prtint(rbcur->rbnext->resval.rint,ResourceString(IDC_CMDS9_DIMZIN__208, "DIMZIN:" ));
						            break;
						        // EBATECH(CNBR) -[ Bugzilla#78443 2003/2/7 DIMAZIN
				                case  79: 
							        cmd_prtint(rbcur->rbnext->resval.rint,ResourceString(IDC_CMDS9_DIMAZIN__208a, "DIMAZIN:" ));
						            break;
						        // EBATECH(CNBR) ]-
				                case 170: 
							        cmd_prtint(rbcur->rbnext->resval.rint,ResourceString(IDC_CMDS9_DIMALT__209, "DIMALT:" ));
						            break;
				                case 171: 
							        cmd_prtint(rbcur->rbnext->resval.rint,ResourceString(IDC_CMDS9_DIMALTD__210, "DIMALTD:" ));
						            break;
				                case 172: 
							        cmd_prtint(rbcur->rbnext->resval.rint,ResourceString(IDC_CMDS9_DIMTOFL__211, "DIMTOFL:" ));
						            break;
				                case 173: 
							        cmd_prtint(rbcur->rbnext->resval.rint,ResourceString(IDC_CMDS9_DIMSAH__212, "DIMSAH:" ));
						            break;
				                case 174: 
							        cmd_prtint(rbcur->rbnext->resval.rint,ResourceString(IDC_CMDS9_DIMTIX__213, "DIMTIX:" ));
						            break;
				                case 175: 
							        cmd_prtint(rbcur->rbnext->resval.rint,ResourceString(IDC_CMDS9_DIMSOXD__214, "DIMSOXD:" ));
						            break;
				                case 176: 
									// Modified Cybage VSB  11/03/2002 DD/MM/YYYY [
									// Reference: Bug 78024 from Bugzilla
									cmd_prtcolor(rbcur->rbnext->resval.rint,ResourceString(IDC_CMDS9_DIMCLRD__215, "DIMCLRD:" ));										
						            break;
				                case 177: 
									cmd_prtcolor(rbcur->rbnext->resval.rint,ResourceString(IDC_CMDS9_DIMCLRE__216, "DIMCLRE:" ));
						            break;
				                case 178: 
									cmd_prtcolor(rbcur->rbnext->resval.rint,ResourceString(IDC_CMDS9_DIMCLRT__217, "DIMCLRT:" ));
									// Modified Cybage VSB  11/03/2002 DD/MM/YYYY ]
						            break;
						        // EBATECH(CNBR) -[ Bugzilla#78443 2003/2/7 DIMADEC
				                case 179: 
									cmd_prtint(rbcur->rbnext->resval.rint,ResourceString(IDC_CMDS9_DIMADEC__208b, "DIMADEC:" ));
						            break;
								// EBATECH(CNBR) ]-
				                // EBATECH(watanabe)-[dimunit
				                //case 270: 
							    //    cmd_prtint(rbcur->rbnext->resval.rint,ResourceString(IDC_CMDS9_DIMUNIT__218, "DIMUNIT:" ));
						        //    break;
				                // EBATECH(watanabe)
				                case 271: 
							        cmd_prtint(rbcur->rbnext->resval.rint,ResourceString(IDC_CMDS9_DIMDEC__219, "DIMDEC:" ));
						            break;
				                case 272: 
							        cmd_prtint(rbcur->rbnext->resval.rint,ResourceString(IDC_CMDS9_DIMTDEC__220, "DIMTDEC:" ));
						            break;
				                case 273: 
							        cmd_prtint(rbcur->rbnext->resval.rint,ResourceString(IDC_CMDS9_DIMALTU__221, "DIMALTU:" ));
						            break;
				                case 274: 
							        cmd_prtint(rbcur->rbnext->resval.rint,ResourceString(IDC_CMDS9_DIMALTTD__222, "DIMALTTD:" ));
						            break;
				                case 275: 
							        cmd_prtint(rbcur->rbnext->resval.rint,ResourceString(IDC_CMDS9_DIMAUNIT__223, "DIMAUNIT:" ));
						            break;
				                // EBATECH(watanabe)-[dimfrac
				                case 276:
							        cmd_prtint(rbcur->rbnext->resval.rint,ResourceString(IDC_CMDS9_DIMFRAC__218a, "DIMFRAC:" ));
						            break;
				                // ]-EBATECH(watanabe)
				                // EBATECH(watanabe)-[dimlunit
				                case 277:
							        cmd_prtint(rbcur->rbnext->resval.rint,ResourceString(IDC_CMDS9_DIMLUNIT__218b, "DIMLUNIT:" ));
						            break;
				                // ]-EBATECH(watanabe)
						        // EBATECH(CNBR) -[ 2003/3/31 DIMDSEP
				                case 278: 
							        cmd_prtint(rbcur->rbnext->resval.rint,ResourceString(IDC_CMDS9_DIMDSEP, "DIMDSEP:" ));
						            break;
						        // EBATECH(CNBR) ]-
				                // EBATECH(watanabe)-[dimtmove
				                case 279:
							        cmd_prtint(rbcur->rbnext->resval.rint,ResourceString(IDC_CMDS9_DIMTMOVE__231b, "DIMTMOVE:" ));
						            break;
				                // ]-EBATECH(watanabe)
				                case 280: 
							        cmd_prtint(rbcur->rbnext->resval.rint,ResourceString(IDC_CMDS9_DIMJUST__224, "DIMJUST:" ));
						            break;
				                case 281: 
							        cmd_prtint(rbcur->rbnext->resval.rint,ResourceString(IDC_CMDS9_DIMSD1__225, "DIMSD1:" ));
						            break;
				                case 282: 
							        cmd_prtint(rbcur->rbnext->resval.rint,ResourceString(IDC_CMDS9_DIMSD2__226, "DIMSD2:" ));
						            break;
				                case 283: 
							        cmd_prtint(rbcur->rbnext->resval.rint,ResourceString(IDC_CMDS9_DIMTOLJ__227, "DIMTOLJ:" ));
						            break;
				                case 284: 
							        cmd_prtint(rbcur->rbnext->resval.rint,ResourceString(IDC_CMDS9_DIMTZIN__228, "DIMTZIN:" ));
						            break;
				                case 285: 
							        cmd_prtint(rbcur->rbnext->resval.rint,ResourceString(IDC_CMDS9_DIMALTZ__229, "DIMALTZ:" ));
						            break;
				                case 286: 
							        cmd_prtint(rbcur->rbnext->resval.rint,ResourceString(IDC_CMDS9_DIMALTTZ__230, "DIMALTTZ:" ));
						            break;
				                // EBATECH(watanabe)-[dimfit
				                //case 287: 
							    //    cmd_prtint(rbcur->rbnext->resval.rint,ResourceString(IDC_CMDS9_DIMFIT__231, "DIMFIT:" ));
						        //    break;
				                // ]-EBATECH(watanabe)
				                case 288: 
							        cmd_prtint(rbcur->rbnext->resval.rint,ResourceString(IDC_CMDS9_DIMUPT__232, "DIMUPT:" ));
						            break;
				                // EBATECH(watanabe)-[dimatfit
				                case 289:
							        cmd_prtint(rbcur->rbnext->resval.rint,ResourceString(IDC_CMDS9_DIMATFIT__231a, "DIMATFIT:" ));
						            break;
				                // ]-EBATECH(watanabe)
				                case 340: 
							        cmd_prttsty(rbcur->rbnext->resval.rstring,ResourceString(IDC_CMDS9_DIMTXSTY__232a, "DIMTXSTY:" ));
						            break;
				                // EBATECH(watanabe)-[dimldrblk
				                case 341:
							        // Modified Cybage VSB 04/03/2002 [
									// Reason: Fix for the bug 77969 from Bugzilla
									if ( strlen(rbcur->rbnext->resval.rstring) ==  0 )
										cmd_prtstr(ResourceString(IDC_COMBO_LDR_ARROWS_4, "Arrow, filled"),ResourceString(IDC_CMDS9_DIMLDRBLK__183a, "DIMLDRBLK:"));										
									else
							        cmd_prttsty(rbcur->rbnext->resval.rstring,ResourceString(IDC_CMDS9_DIMLDRBLK__183a, "DIMLDRBLK:"));
										
									// Modified Cybage VSB 04/03/2002 
				                // ]-EBATECH(watanabe)
									break;
								// EBATECH(CNBR) -[ 24-03-2003 Bugzilla#78471 Support Lineweight
								case 371:
							        cmd_prtint(rbcur->rbnext->resval.rint,ResourceString(IDC_CMDS9_DIMLWD, "DIMLWD:" ));
						            break;
								case 372:
							        cmd_prtint(rbcur->rbnext->resval.rint,ResourceString(IDC_CMDS9_DIMLWE, "DIMLWE:" ));
						            break;
								// EBATECH(CNBR) ]-
                            }
       					    if (RTNORM!=(rc=cmd_entlister_scrollchk(&scrollcur,scrollmax)))return rc;
                            if (rbcur->rbnext->rbnext!=NULL) rbcur=rbcur->rbnext;
                        }
                    }
    				IC_RELRB(erbx);
					return RTNORM;

}
//Bugzilla No 78004 18-03-2002  ]
int cmd_entlister_scrollchk(int *scrollcur, int scrollmax)
	{

	if ( GetActiveCommandQueue()->IsNotEmpty() || 
		 GetMenuQueue()->IsNotEmpty() || 
		 SDSApplication::GetActiveApplication()->IsCommandRunning()
		 )
		{
		return(RTNORM);// running script 
		}

	(*scrollcur)++;
	if (*scrollcur<(scrollmax))
		{
		return(RTNORM);
		}
	*scrollcur=0;

	char fs1[IC_ACADNMLEN];
	sds_initget(0,NULL);
	return(sds_getstring(0,ResourceString(IDC_CMDS9__NPRESS_ENTER_TO_440, "\nPress ENTER to continue..." ),fs1));
	}


