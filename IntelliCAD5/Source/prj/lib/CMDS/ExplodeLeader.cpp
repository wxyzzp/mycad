// EXPLODE command for LEADER

// SystemMetrix(watanabe)-[explode LEADER

#include "cmds.h"/*DNT*/
#include "IcadApi.h"/*DNT*/
#include "dimstyletabrec.h"/*DNT*/
#include "Explode.h"/*DNT*/
#include "DbMText.h"/*DNT*/
#include "IcadView.h"
#include "DoBase.h"

//////////////////////////////////////////////////////////////////////
static
BOOL
CreateEcs(
  struct resbuf&    rb,
  sds_point         pt
)
{
  rb.restype = RT3DPOINT;
  ic_ptcpy(rb.resval.rpoint, pt);
  return TRUE;
}

//////////////////////////////////////////////////////////////////////
static
BOOL
CreateWcs(
  struct resbuf&    rb
)
{
  rb.restype = RTSHORT;
  rb.resval.rint = 0;
  return TRUE;
}

//////////////////////////////////////////////////////////////////////
static
double
LeaderAtan2(
  const double  dy,
  const double  dx
)
{
  double angle = 0;
  if (icadRealEqual(dx,0.0) && icadRealEqual(dy,0.0))
    {
    }
  else
    {
      angle = atan2(dy, dx);
    }
  return angle;
}

//////////////////////////////////////////////////////////////////////
BOOL
IsStartOrigin(
  const char*   blockname
)
{
  if (strisame(blockname, "_DOTSMALL"/*DNT*/) ||
      strisame(blockname, "_SMALL"/*DNT*/   ) ||
      strisame(blockname, "_NONE"/*DNT*/    ) ||
      strisame(blockname, "_OBLIQUE"/*DNT*/ ) ||
      strisame(blockname, "_INTEGRAL"/*DNT*/) ||
      strisame(blockname, "_ARCHTICK"/*DNT*/))
    {
      return TRUE;
    }

  return FALSE;
}

//////////////////////////////////////////////////////////////////////
// class for dimension style value on leader
class CLeaderDimstyle
{
public:
  CLeaderDimstyle()
    {
      blockname[0] = '\0';
      dDimscale = 1.0;
      dDimasz   = 0.18;
      dDimgap   = 0.09;
      iDimtad   = 0;
      iDimclrd  = 0;
      xdir[0]   = 1;
      xdir[1]   = 0;
      xdir[2]   = 0;
      ecs[0]    = 0;
      ecs[1]    = 0;
      ecs[2]    = 1;
      mtextbox[0] = 1;
      mtextbox[1] = 0;
      mtextbox[2] = 0;
      bHasAnno    = FALSE;
      bIsMtext    = FALSE;
      bShowArrow  = TRUE;
      bIsSpline   = FALSE;
    };

  BOOL
  GetLeaderDimstyle(
    db_leader*    leader
  );

public:
  char      blockname[260];
  double    dDimscale;
  double    dDimasz;
  double    dDimgap;
  int       iDimtad;
  int       iDimclrd;
  sds_point xdir;
  sds_point ecs;
  sds_point mtextbox;
  BOOL      bHasAnno;
  BOOL      bIsMtext;
  BOOL      bIsSpline;
  BOOL      bShowArrow;
};

BOOL
CLeaderDimstyle::GetLeaderDimstyle(
  db_leader*    leader
)
{
  // annotation
  db_handitem*  annothip;
  leader->get_340(&annothip);

  // Get actual MTEXT size
  char str[256];
  str[0] = '\0';
  if (annothip)
    {
      annothip->get_0(str, 255);
      bHasAnno = TRUE;

      if (strsame(str, "MTEXT"/*DNT*/))
        {
          bIsMtext = TRUE;

          sds_point p0, p1;
          CDbMText* mtext = (CDbMText*)annothip;
#ifdef _USE_DISP_OBJ_PTS_3D_
//#pragma message("_USE_DISP_OBJ_PTS_3D_ defined in ExplodeLeader.cpp")
		  CD3 minPoint, maxPoint;
		  CDoBase::getPlanarBoundingBox(SDS_CURGRVW, mtext, minPoint, maxPoint);
		  memcpy(p0, (sds_real*)minPoint, 3 * sizeof(sds_real));
		  memcpy(p1, (sds_real*)maxPoint, 3 * sizeof(sds_real));
#else
          mtext->get_extent(p0, p1);
#endif
          mtextbox[0] = fabs(p0[0] - p1[0]);
          mtextbox[1] = fabs(p0[1] - p1[1]);
        }
    }

  // entity coordinate system
  leader->get_210(ecs);

  // x direction of leader
  leader->get_211(xdir);

  // show arrow
  leader->get_71(&bShowArrow);

  // spline
  leader->get_72(&bIsSpline);

  // get system variables from dimension style
  db_dimstyletabrec* pDstyle;
  leader->get_3((db_handitem**)&pDstyle);
  if (pDstyle != NULL)
    {
      // SystemMetrix(Hiro)-[Change the method of having data of DIMLDRBLK in DIMSTYLE and in EED
      //pDstyle->get_341(blockname, 255);
      pDstyle->get_341name(blockname, 255);
      // ]-SystemMetrix(Hiro) Change the method of having data of DIMLDRBLK in DIMSTYLE and in EED
      pDstyle->get_40( &dDimscale);
      pDstyle->get_41( &dDimasz);
      pDstyle->get_77( &iDimtad);
      pDstyle->get_147(&dDimgap);
      pDstyle->get_176(&iDimclrd);
    }

  // Set the ACAD app name
  struct resbuf rbApp;
  rbApp.restype = RTSTR;
  rbApp.rbnext = NULL;
  rbApp.resval.rstring = "ACAD"/*DNT*/;

  // get system variables from extended entity data (eed)
  struct resbuf* rbEed;
  rbEed = leader->get_eed(&rbApp, NULL);

  // find "DSTYLE" in eed
  struct resbuf* rb = rbEed;
  while (rb != NULL &&
        (rb->restype != 1000 || stricmp(rb->resval.rstring, "DSTYLE"/*DNT*/)))
    {
      rb = rb->rbnext;
    }

  // Matching { for the closing one below
  for (; rb != NULL &&
         (rb->restype!=1002 || *rb->resval.rstring != '}');
         rb = rb->rbnext)
    {
      if (rb->restype != 1070 || rb->rbnext == NULL)
        {
          continue;
        }

      int i = rb->resval.rint;
      rb = rb->rbnext;
      switch (i)
        {
          case 341:
            {
              if (rb->restype != 1005)
                {
                  break;
                }

              if (strlen(rb->resval.rstring) == 0)
                {
                  blockname[0] = '\0';
                }
              else
                {
                  db_handitem* pHand;
                  pHand = SDS_CURDWG->handent(rb->resval.rstring);
                  if (pHand != NULL)
                    {
                      // Bugzilla::78558; 24/06/2003; [
					  //if (pHand->ret_type() == DB_BLOCKTABREC)
                      //  {
                      //    db_blocktabrec* pBlock = (db_blocktabrec*)pHand;
                      //    if (pBlock != NULL)
                      //      {
                      //        pBlock->get_2(blockname, 255);
                      //      }
                      //  }
					  assert(pHand->ret_type() == DB_BLOCK);
					  pHand->get_2(blockname, 255);
					  // Bugzilla::78558; 24/06/2003; ]

                    }
                }
              break;
            }

        case  40:
            if (rb->restype != 1040)
              {
                break;
              }
            dDimscale = rb->resval.rreal;
            break;

        case  41:
            if (rb->restype != 1040)
              {
                break;
              }
            dDimasz = rb->resval.rreal;
            break;

        case 77:
            if (rb->restype != 1070)
              {
                break;
              }
            iDimtad = rb->resval.rint;
            break;

        case  147:
            if (rb->restype != 1040)
              {
                break;
              }
            dDimgap = rb->resval.rreal;
            break;

        case 176:
            if (rb->restype != 1070)
              {
                break;
              }
            iDimclrd = rb->resval.rint;
            break;
        }
    }

  return TRUE;
}

//////////////////////////////////////////////////////////////////////
// get common group code from leader
static
BOOL
GetCommonGroupCode(
  struct resbuf*    rb,
  sds_point         pt210,
  struct resbuf*&   rbCur
)
{
  switch (rb->restype)
    {
      // extrude
      case 210:
          ic_ptcpy(pt210, rb->resval.rpoint);
          return TRUE;

      // Linetype
      case 6:
          rbCur->rbnext = sds_buildlist(6, rb->resval.rstring, 0);
          break;

      // Ltscale
      case 48:
          rbCur->rbnext = sds_buildlist(48, rb->resval.rreal, 0);
          break;

      // color
      case 62:
          rbCur->rbnext = sds_buildlist(rb->restype, rb->resval.rint, 0);
          break;

      default:
          return TRUE;
    }

  if (rbCur->rbnext == NULL)
    {
      return FALSE;
    }
  rbCur = rbCur->rbnext;

  return TRUE;
}

//////////////////////////////////////////////////////////////////////
// get angle of leader
static
double
GetRotation(
  struct resbuf*    rbBegin,
  sds_point pt0,
  sds_point pt1
)
{
  // ecs
  struct resbuf rbecs;
  ic_assoc(rbBegin, 210);
  CreateEcs(rbecs, ic_rbassoc->resval.rpoint);

  // wcs
  struct resbuf rbwcs;
  CreateWcs(rbwcs);

  // get rotation
  sds_trans(pt0, &rbwcs, &rbecs, 0, pt0);
  sds_trans(pt1, &rbwcs, &rbecs, 0, pt1);

  double angle = LeaderAtan2(pt1[1] - pt0[1], pt1[0] - pt0[0]);
  return angle;
}

//////////////////////////////////////////////////////////////////////
// move start point
static
BOOL
MoveStartPoint(
  CLeaderDimstyle   ld,
  const int         iNum,
  sds_point*&       pts
)
{
  // if arrow is hidden
  if (ld.bShowArrow == FALSE)
    {
      // do nothing
      return FALSE;
    }

  // if spline type
  //Bugzilla No 78096 15-04-2002 [
  /*if (ld.bIsSpline)
    {
      // do nothing
      //return FALSE;
    }*/
  //Bugzilla No 78096 15-04-2002 ]

  if (IsStartOrigin(ld.blockname))
    {
      // do nothing
      return FALSE;
    }

  // if length from first point to second point is shorter than DIMASZ
  sds_point pt0, pt1;
  ic_ptcpy(pt0, pts[0]);
  ic_ptcpy(pt1, pts[1]);
  double dist1 = sds_distance(pt0, pt1);
  double dist2 = ld.dDimasz * ld.dDimscale;
  if (dist1 < dist2)
    {
      // hide arrow
      return FALSE;
    }

  sds_point vec;
  ic_sub(pt0, pt1, vec);

  ic_unitize(vec);
  vec[0] *= dist2;
  vec[1] *= dist2;
  vec[2] *= dist2;

  ic_add(pt0, vec, pts[0]);

  return TRUE;
}
//Bugzilla No 78096 15-04-2002 [
//commented out this function and added a new version for creating lwpline alongwith spline creation function
//////////////////////////////////////////////////////////////////////
// add leader lines
//  - This version is convert to LINE entity, not to POLYLINE.
//  - This version does not support spline type.
/*static
BOOL
AddLeaderLines(
  CLeaderDimstyle   ld,
  struct resbuf*    rbBegin,
  const int         iNum,
  sds_point*        pts,
  sds_name          ss
)
{
  // move start point
  MoveStartPoint(ld, iNum, pts);

  // first point
  sds_point pt0;
  ic_ptcpy(pt0, pts[0]);

  // add lines
  sds_point pt1;
  for (int i = 1; i < iNum; i++)
    {
      ic_ptcpy(pt1, pts[i]);

      // LINE entity
      struct resbuf* rb =sds_buildlist(
                RTDXF0, db_hitype2str(DB_LINE),
                10, pt0,
                11, pt1,
                0);
      if (rb)
        {
          struct resbuf* rbStart = rb;
          while (rb->rbnext)
            {
              rb = rb->rbnext;
            }
          struct resbuf* rbLast = rb;
          rb->rbnext = rbBegin;

          int irt = sds_entmake(rbStart);
          if (irt == RTNORM)
            {
              sds_name name;
              sds_entlast(name);

              SDS_ssadd(name, ss, ss);

              ic_ptcpy(pt0, pt1);
            }

          // free
          rbLast->rbnext = NULL;
          IC_RELRB(rb);
        }
    } // end of loop over verts

  return TRUE;
}*/

//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// add leader polyline
//  - Modified version to  convert to LWPOLYLINE entity.
static
BOOL
AddLeaderPLine(
  CLeaderDimstyle   ld,
  struct resbuf*    rbBegin,
  const int         iNum,
  sds_point*        pts,
  sds_name          ss
)
{
  // move start point
  MoveStartPoint(ld, iNum, pts);
  
  struct resbuf* rb =sds_buildlist(
		  RTDXF0, db_hitype2str(DB_LWPOLYLINE),
		  90, iNum,
		  70,0,
		  0);

	sds_point plinept;
	struct resbuf* rbPrev = rb;
	struct resbuf* rlist=rb->rbnext;
	struct resbuf* rbCur;
	for(int i=0;i<iNum ; i++)
	   {
	        ic_ptcpy(plinept,pts[i]);
	        rbCur = sds_buildlist(10, plinept, RTNONE);
			rbPrev->rbnext = rbCur;
	        rbPrev = rbCur;
}
		rbPrev->rbnext=rlist;
	if (rb)
	  {
		  struct resbuf* rbStart = rb;
		  while (rb->rbnext)
		  {
			  rb = rb->rbnext;
		  }
		  struct resbuf* rbLast = rb;
		  rb->rbnext = rbBegin;
		  
		  int irt = sds_entmake(rbStart);
		  if (irt == RTNORM)
		  {
			  sds_name name;
			  sds_entlast(name);
			  
			  SDS_ssadd(name, ss, ss);
		  }
		  
		  // free
		  rbLast->rbnext = NULL;
		  IC_RELRB(rb);
	  }
	  

	  
	  return TRUE;
}
//Bugzilla No 78096 15-04-2002 ]
//////////////////////////////////////////////////////////////////////
// add leader arrow
static
BOOL
AddLeaderArrow(
  CLeaderDimstyle   ld,
  struct resbuf*    rbBegin,
  const int         iNum,
  sds_point*        pts,
  sds_name          ss
)
{
  // the arrow is not displayed
  if (ld.bShowArrow == FALSE)
    {
      return TRUE;
    }

  sds_point pt0;
  ic_ptcpy(pt0, pts[0]);

  sds_point pt1;
  ic_ptcpy(pt1, pts[1]);

  // INSERT (block reference) entity
  double dSize = ld.dDimscale * ld.dDimasz;
  double dRot  = GetRotation(rbBegin, pt1, pt0);

  struct resbuf* rb;
  if (strlen(ld.blockname) > 0)
    {
      // block reference
      rb = sds_buildlist(
                RTDXF0, db_hitype2str(DB_INSERT),
                2,      ld.blockname,
                10,     pt0,
                41,     dSize,
                42,     dSize,
                43,     dSize,
                50,     dRot,
                0);
    }
  else
    {
      // solid
      sds_point sp1, sp2;
      sp1[0] = -dSize;
      sp1[1] = dSize / 6;
      sp1[2] = 0;

      sp2[0] = -dSize;
      sp2[1] = -dSize / 6;
      sp2[2] = 0;

      // rotation
      double dx, dy;
      dx = cos(dRot) * sp1[0] - sin(dRot) * sp1[1];
      dy = sin(dRot) * sp1[0] + cos(dRot) * sp1[1];
      sp1[0] = dx;
      sp1[1] = dy;
      dx = cos(dRot) * sp2[0] - sin(dRot) * sp2[1];
      dy = sin(dRot) * sp2[0] + cos(dRot) * sp2[1];
      sp2[0] = dx;
      sp2[1] = dy;

      // move
      ic_add(sp1, pt0, sp1);
      ic_add(sp2, pt0, sp2);

      rb = sds_buildlist(
                RTDXF0, "SOLID"/*DNT*/,
                10,     pt0,
                11,     sp1,
                12,     sp2,
                13,     sp2,
                0);
    }

  if (rb)
    {
      struct resbuf* rbStart = rb;
      while (rb->rbnext)
        {
          rb = rb->rbnext;
        }
      struct resbuf* rbLast = rb;
      rb->rbnext = rbBegin;

      int irt = sds_entmake(rbStart);
      if (irt == RTNORM)
        {
          sds_name name;
          sds_entlast(name);

          SDS_ssadd(name, ss, ss);
        }

      // free
      rbLast->rbnext = NULL;
      IC_RELRB(rb);
    }

  return TRUE;
}

  //Bugzilla No 78002 01-04-2002 [
  static
	  BOOL
	  AddLeaderBox(
	  CLeaderDimstyle   ld,
	  struct resbuf*    rbBegin,
	  const int         iNum,
	  sds_point*        pts,
	  sds_name          ss,
	  db_leader*&		leader
	  )
  {
	  // leader last point
	  sds_point lastpt;
	  sds_point  boxpts[4];
	  
	  int hookflag;
	  leader->get_74(&hookflag);
	  
	  ic_ptcpy(lastpt, pts[iNum-1]);
	  
	  
	  sds_point p1,p2;
	  db_handitem* hip;
	  leader->get_340(&hip);
	  CDbMText* mtext = (CDbMText*)hip;
#ifdef _USE_DISP_OBJ_PTS_3D_
//#pragma message("_USE_DISP_OBJ_PTS_3D_ defined in ExplodeLeader.cpp")
	  CD3 minPoint, maxPoint;
	  CDoBase::getPlanarBoundingBox(SDS_CURGRVW, mtext, minPoint, maxPoint);
	  memcpy(p1, (sds_real*)minPoint, 3 * sizeof(sds_real));
	  memcpy(p2, (sds_real*)maxPoint, 3 * sizeof(sds_real));
#else
	  mtext->get_extent(p1, p2);
#endif
	  sds_real txtheight = fabs(p1[1] - p2[1]);
	  sds_real txtwidth  = fabs(p1[0] - p2[0]);
	  
	  sds_real height=txtheight + 2.0 * fabs(ld.dDimgap * ld.dDimscale);
	  sds_real width=txtwidth + 2.0 * fabs(ld.dDimgap * ld.dDimscale);

	  	  
	  ic_ptcpy(boxpts[1],lastpt);
	  
	  if(ld.iDimtad == 0)
		  boxpts[1][1]-=height/2;
	  
	  
	  ic_ptcpy(boxpts[0],boxpts[1]);
	  boxpts[0][1]+=height;
	  
	  if(hookflag==1)
		  width=-width;
	  
	  ic_ptcpy(boxpts[2],boxpts[1]);
	  boxpts[2][0]+=width;
	  
	  ic_ptcpy(boxpts[3],boxpts[0]);
	  boxpts[3][0]+=width;
	  
	  struct resbuf* rb =sds_buildlist(
		  RTDXF0, db_hitype2str(DB_LWPOLYLINE),
		  90, 4,
		  10, boxpts[0],
		  10, boxpts[1],
		  10, boxpts[2],
		  10, boxpts[3],
		  70,1,
		  0);
	  
	  
	  
	  if (rb)
	  {
		  struct resbuf* rbStart = rb;
		  while (rb->rbnext)
		  {
			  rb = rb->rbnext;
		  }
		  struct resbuf* rbLast = rb;
		  rb->rbnext = rbBegin;
		  
		  int irt = sds_entmake(rbStart);
		  if (irt == RTNORM)
		  {
			  sds_name name;
			  sds_entlast(name);
			  
			  SDS_ssadd(name, ss, ss);
		  }
		  
		  // free
		  rbLast->rbnext = NULL;
		  IC_RELRB(rb);
	  }
	  
	  
	  
	  return TRUE;
	  }
	//Bugzilla No 78002 01-04-2002 ]
   		
//Bugzilla No 78096 15-04-2002 [
   static
	  BOOL
	  AddLeaderSpline(
	  CLeaderDimstyle   ld,
	  struct resbuf*    rbBegin,
	  const int         iNum,
	  sds_point*        pts,
	  sds_name          ss,
	  db_leader*&		leader
	   )
  {
	   // move start point
	   MoveStartPoint(ld, iNum, pts);

	   sds_point splinept;
	   sds_point extru;
	   leader->get_210(extru);

	   resbuf *rb= sds_buildlist(RTDXF0, db_hitype2str(DB_SPLINE),
					  73, 0,
					  72,0,
					  74, iNum,
					  70, 8,
					  71,3,
					  210,extru,
					  0);


       struct resbuf* rbPrev = rb;
	   struct resbuf* rlist=rb->rbnext;
       struct resbuf* rbCur;
       for(int i=0;i<iNum; i++)
	   {
		    ic_ptcpy(splinept,pts[i]);
	        rbCur = sds_buildlist(11, splinept, RTNONE);
            rbPrev->rbnext = rbCur;
            rbPrev = rbCur;
       }
	  rbPrev->rbnext=rlist;
	  if (rb)
	  {
		  struct resbuf* rbStart = rb;
		  while (rb->rbnext)
		  {
			  rb = rb->rbnext;
		  }
		  struct resbuf* rbLast = rb;
		  rb->rbnext = rbBegin;
		  
		  int irt = sds_entmake(rbStart);
		  if (irt == RTNORM)
		  {
			  sds_name name;
			  sds_entlast(name);
			  
			  SDS_ssadd(name, ss, ss);
		  }
   		
		  // free
		  rbLast->rbnext = NULL;
		  IC_RELRB(rb);
	  }

	   

	  return TRUE;
}
		
//Bugzilla No 78096 15-04-2002 ]
	   

	   
//////////////////////////////////////////////////////////////////////
// Get angle between start point and second point
static
double
GetLeaderAngle(
  CLeaderDimstyle   ld,
  const int         iNum,
  sds_point*        pts
)
{
  double angle = 0;

  sds_point pt0, pt1;
  ic_ptcpy(pt0, pts[iNum - 2]);
  ic_ptcpy(pt1, pts[iNum - 1]);

  // ecs
  struct resbuf rbecs;
  CreateEcs(rbecs, ld.ecs);

  // wcs
  struct resbuf rbwcs;
  CreateWcs(rbwcs);

  // rotate points
  sds_trans(pt0, &rbwcs, &rbecs, 0, pt0);
  sds_trans(pt1, &rbwcs, &rbecs, 0, pt1);

  // angle between first point and second point
  double angle0 = LeaderAtan2(pt1[1] - pt0[1], pt1[0] - pt0[0]);

  // rotate x dir
  sds_point xdir2;
  sds_trans(ld.xdir, &rbwcs, &rbecs, 1, xdir2);

  // angle of x direction
  double anglex = LeaderAtan2(xdir2[1], xdir2[0]);

  angle = angle0 - anglex;

  return angle;
}

//////////////////////////////////////////////////////////////////////
// extend hook line
static
BOOL
ExtendHookLine(
  db_leader*        leader,
  CLeaderDimstyle   ld,
  int&              iNum,
  sds_point*&       pts
)
{
  double angle = GetLeaderAngle(ld, iNum, pts);
  BOOL bIsLeftSide = IsLeftSide(angle);
  BOOL bHasHook    = HasHook(angle);
  if (bHasHook)
    {
      // move last point according to DIMASZ

      // add a last point
      IC_FREE(pts);

      iNum++;
      pts = new sds_point[ iNum + 1] ;
	  memset(pts,0,sizeof(sds_point)*(iNum + 1));
      leader->get_10(pts, iNum - 1);

      ic_ptcpy(pts[iNum - 1], pts[iNum - 2]);

      // line length
      double dSize = -ld.dDimasz * ld.dDimscale;
      if (bIsLeftSide)
        {
          dSize = -dSize;
        }

      // move last point
      sds_point ptHook;
      ic_ptcpy(ptHook, ld.xdir);
      ptHook[0] *= dSize;
      ptHook[1] *= dSize;
      ptHook[2] *= dSize;

      ic_add(pts[iNum - 2], ptHook, pts[iNum - 2]);
    }

  if (ld.bIsMtext)
    {
      if (ld.dDimgap < 0)
        {
          // draw box around mtext

          // In future, here must be created.
        }

	  //Bugzilla No 78002 01-04-2002
	  //if (ld.iDimtad)
      if (ld.iDimtad && ld.dDimgap>=0)
        {
          double dSize = ld.mtextbox[0];
          dSize += fabs(ld.dDimgap * ld.dDimscale);
          if (bIsLeftSide)
            {
              dSize = -dSize;
            }

          // move last point
          sds_point ptHook;
          ic_ptcpy(ptHook, ld.xdir);
          ptHook[0] *= dSize;
          ptHook[1] *= dSize;
          ptHook[2] *= dSize;

          // extend the last line
          ic_add(pts[iNum - 1], ptHook, pts[iNum - 1]);
        }
    }

  return TRUE;
}

//////////////////////////////////////////////////////////////////////
static
BOOL
XplodeLeader(
  db_leader*        leader,
  sds_name          ename,
  struct resbuf*    rbBegin
)
{
  int iNum;
  leader->get_76(&iNum);
  if (iNum == 0)
    {
      return FALSE;
    }

  // get vertexs  
  sds_point* pts = new sds_point [iNum + 1];
  memset(pts,0,sizeof(sds_point)*(iNum + 1));
  if (pts == NULL)
    {
      return FALSE;
    }
  leader->get_10(pts, iNum);

  // get system variables for leader
  CLeaderDimstyle ld;
  ld.GetLeaderDimstyle(leader);

  // direction of under line (hook)
  if (ld.bHasAnno)
    {
      ExtendHookLine(leader, ld, iNum, pts);
    }

  // Set the Undo so we corectly redraw the entity Undo.
  SDS_SetUndo(IC_UNDO_ENTITY_REDRAW, (void*)ename[0], NULL, NULL, SDS_CURDWG);

  struct resbuf* rbCommon = rbBegin;
  struct resbuf* rbCur    = rbBegin;
  while (rbCur->rbnext)
    {
      rbCur = rbCur->rbnext;
    }

  // clear selection set
  sds_name ss;
  sds_ssadd(NULL, NULL, ss);

  // add arrow
  AddLeaderArrow(ld, rbBegin, iNum, pts, ss);

  // add lines
  //Bugzilla No 78096 15-04-2002 [
  //AddLeaderLines(ld, rbBegin, iNum, pts, ss);
  int isSpline;
  leader->get_72(&isSpline);
  if(isSpline==0)
	AddLeaderPLine(ld, rbBegin, iNum, pts, ss);
  else
	AddLeaderSpline(ld, rbBegin, iNum, pts, ss,leader);
  //Bugzilla No 78096 15-04-2002 ]
  
  //Bugzilla No 78002 01-04-2002 [
  db_handitem* hip;
  leader->get_340(&hip);
  
  if(hip && ld.dDimgap <0 && (DB_MTEXT==hip->ret_type()))
   AddLeaderBox(ld, rbBegin, iNum, pts, ss,leader);
  //Bugzilla No 78002 01-04-2002 ]



  // if new entities are added
  long lNum = sds_sslength(ss, &lNum);
  sds_sslength(ss, &lNum);
  if (lNum > 0)
    {
      // remove LEADER entity
      sds_entdel(ename);

      // set undo
      for (int i = 0; i < lNum; i++)
        {
          sds_name name;
          int irt = sds_ssname(ss, i, name);
          if (irt == RTNORM)
            {
              // Set the Undo so we corectly redraw the entity Redo.
              SDS_SetUndo(IC_UNDO_ENTITY_REDRAW, (void*)name[0], NULL, NULL, SDS_CURDWG);
              sds_redraw(name, IC_REDRAW_DRAW);
            }
        }
    }

  // free
  sds_ssfree(ss);
  IC_FREE(pts);

  return TRUE;
}

//////////////////////////////////////////////////////////////////////
// EXPLODE command for LEADER entity
BOOL
ExplodeLeader(
  sds_name          ename,
  struct resbuf*    elist,
  const int         psflag
)
{
  db_leader *leader = (db_leader*)ename[0];

  // LAYER
  if (ic_assoc(elist, 8) != 0)
    {
      return FALSE;
    }

  struct resbuf* rbBegin = sds_buildlist(
                8,  ic_rbassoc->resval.rstring,
                67, psflag,
                0);
  if (rbBegin == NULL)
    {
      return FALSE;
    }

  // walk elcur to end of llist
  struct resbuf* rbCur = rbBegin;
  while (rbCur->rbnext)
    {
      rbCur = rbCur->rbnext;
    }

  // extrude
  sds_point pt210;
  pt210[0] = pt210[1] = 0.0;
  pt210[2] = 1.0;

  // now loop and pick off necessary data from entity
  struct resbuf* rb;
  BOOL bRet = TRUE;
  for (rb = elist; rb != NULL; rb = rb->rbnext)
    {
      bRet = GetCommonGroupCode(rb, pt210, rbCur);
      if (bRet == FALSE)
        {
          break;
        }
    }
  IC_RELRB(rb);

  if (bRet)
    {
      // extrude
      rbCur->rbnext = sds_buildlist(210, pt210, 0);
      if (rbCur->rbnext)
        {
          XplodeLeader(leader, ename, rbBegin);
        }
    }

  IC_RELRB(rbBegin);

  return TRUE;
}
// ]-SystemMetrix(watanabe) explode LEADER

