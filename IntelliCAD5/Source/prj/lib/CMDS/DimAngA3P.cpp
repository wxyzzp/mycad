// =========================================================================================================================================
// ================================================================ Includes ===============================================================
// =========================================================================================================================================

#include "cmds.h"/*DNT*/
#include "dimensions.h"
#include "cmdsDimVars.h"
#include "IcadView.h"
#include "cmdsDimBlock.h"

static BOOL
DrawDimAng(
  struct cmd_dimeparlink*   plink, 
  db_drawing*   flp,
  short         dragmode,
  sds_point     ptExtL[],
  sds_point     ptDimL[],
  sds_point     ptt[],
  sds_point     vertex,
  sds_point     pt14,
  sds_point     pt13,
  sds_point     txtbox[],
  sds_point     ipt[],
  sds_real      anglex[],
  sds_real      fr1,
  sds_real      fr4,
  sds_real      fr5,
  short         interseccnt,
  char          text[],
  BOOL          ArrowFit,
  BOOL          OffFlgL13,
  BOOL          OffFlgL14,
  BOOL          addlineflag,
  BOOL          ArrowSideflg,
  bool          bDrawOutside,
  sds_real      dAngTih,
  sds_real      dAngToh
);

// =========================================================================================================================================
// =========================================================== External Variables ==========================================================
// =========================================================================================================================================

extern                        SDS_AtNodeDrag;
extern sds_point    dimzeropt; // Needed to put this variable here because it gets initialized.
extern char         cmd_string1[256], cmd_string2[256], *layername;
extern struct cmd_dimvars plinkvar[];

// ============================================================================================================================================================
// ========================================================= ANGULAR DIMENSION FUNCTIONS =====================================================================
// ============================================================================================================================================================
//
// EBATECH(watanabe)-[
void
GetDimAngIntersect(
  struct cmd_dimeparlink*   plink,
  sds_point txtbox[],
  sds_real  dAngTih,
  sds_point vertex,
  sds_real  fr1,
  sds_real  angS,
  sds_real  angE,
  sds_point xpt[],
  sds_real  angle13,
  sds_real  angle14,
  short&    interseccnt,
  sds_real  anglex[]
)
{
  BYTE*     bPtr;
  ads_point nbox[4];
  short fi1, fi2, fi3;

  // Get the points of intersections with the box and the arc.
  // xpt is an array of intersection points of the box with the arc. dIntvalue holds the values
  // for what type of intersection it was. Create the angles made by these intersections.

  // EBATECH(watanabe)-[
  //wordval=cmd_getboxinsersecArc(txtbox,0.0,plink->textmidpt,vertex,fr1,arlist[0],arlist[1],xpt,nbox);
  DWORD wordval = cmd_getboxinsersecArc(
              txtbox,
              dAngTih,
              plink->textmidpt,
              vertex,
              fr1,
              angS,
              angE,
              xpt,
              nbox);
  // ]-EBATECH(watanabe)

  // First angle.
  anglex[0]=angle13;
  for (bPtr=(BYTE *)&wordval,fi1=1,fi2=0;fi2<8;fi2+=2,++bPtr) {
      if (*bPtr==(byte)0xff) {                    // -1 : No intersections
          continue;
      } else if (*bPtr==(BYTE)0x0f) {             // 2 intersections on one line.
          anglex[fi1++]=sds_angle(vertex,xpt[fi2]);
          anglex[fi1++]=sds_angle(vertex,xpt[fi2+1]);
      } else if ((*bPtr&0x08) && (*bPtr&0x04)) {  // Intersection on second line and arc.
          anglex[fi1++]=sds_angle(vertex,xpt[fi2+1]);
      } else if ((*bPtr&0x01) && (*bPtr&0x02)) {  // Intersection on first line and arc.
          anglex[fi1++]=sds_angle(vertex,xpt[fi2]);
      }
  }

  // last angle.
  anglex[fi1]=angle14;

  // Make sure to make all angles greater then the angle13.
  for (fi2=0;fi2<fi1;++fi2) { if (anglex[fi2]<angle13) ic_normang(&angle13,&anglex[fi2]); }

  // Now sort the indexes of the angles becouse they will be out of order.
  for (interseccnt=fi1,fi2=0;fi2<fi1;++fi2) {
      for (fi3=fi2+1;fi3<fi1;++fi3) {
          if (anglex[fi3]<anglex[fi2]) {
              sds_real fr2=anglex[fi2];
              anglex[fi2]=anglex[fi3];
              anglex[fi3]=fr2;
          }
      }
  }
}

// EBATECH(watanabe)-[support DIMTAD
static double
GetGap()
{
  double dGap = plinkvar[DIMGAP].dval.rval;
  if (dGap < 0)
    {
      dGap *= 2;
    }
  dGap = fabs(dGap);
  dGap += plinkvar[DIMTXT].dval.rval / 2;
  dGap *= plinkvar[DIMSCALE].dval.rval;
  return dGap;
}

static bool
MoveDimAngTextIn(
  struct cmd_dimeparlink*   plink,
  sds_point                 txtbox[],
  sds_point                 vertex
)
{
  double dMove = 0.0;
  double dGap;
  double ang = sds_angle(   vertex, plink->textmidpt);
  if (plinkvar[DIMTIH].dval.ival)
    {
      /////// Under construction from here ///////
      // (The new plink->textmidpt must throw the prolong line
      //  from vertex to plink->textmidpt.)
      dGap = fabs(plinkvar[DIMGAP].dval.rval);
      double arcR = sds_distance(plink->pt16, vertex);
      double rx = (arcR + dGap) * cos(ang) + vertex[X];
      double ry = (arcR + dGap) * sin(ang) + vertex[Y];
      switch (plinkvar[DIMTAD].dval.ival)
        {
          case 1:
              if ((fabs(ang - DIM_90_DEG)  < CMD_FUZZ) ||
                  (fabs(ang - DIM_270_DEG) < CMD_FUZZ))
                {
                  plink->textmidpt[Y] += GetGap();
                  return true;
                }
              break;
          case 2:
          case 3:
              if (fabs(ang)  < CMD_FUZZ)
                {
                  plink->textmidpt[X] -= GetGap();
                }
              else if (ang < DIM_90_DEG - CMD_FUZZ)
                {
                  plink->textmidpt[X] = rx + txtbox[1][X];
                  plink->textmidpt[Y] = ry + txtbox[1][Y];
                }
              else if (fabs(ang - DIM_90_DEG)  < CMD_FUZZ)
                {
                  plink->textmidpt[Y] += GetGap();
                }
              else if (ang < IC_PI - CMD_FUZZ)
                {
                  plink->textmidpt[X] = rx - txtbox[1][X];
                  plink->textmidpt[Y] = ry + txtbox[1][Y];
                }
              else if (fabs(ang - IC_PI)  < CMD_FUZZ)
                {
                  plink->textmidpt[X] -= GetGap();
                }
              else if (plinkvar[DIMTAD].dval.ival == 2)
                {
                  if (ang < DIM_270_DEG - CMD_FUZZ)
                    {
                      plink->textmidpt[X] = rx - txtbox[1][X];
                      plink->textmidpt[Y] = ry - txtbox[1][Y];
                    }
                  else if (fabs(ang - DIM_270_DEG)  < CMD_FUZZ)
                    {
                      plink->textmidpt[Y] -= GetGap();
                    }
                  else
                    {
                      plink->textmidpt[X] = rx + txtbox[1][X];
                      plink->textmidpt[Y] = ry - txtbox[1][Y];
                    }
                }
              else
                {
                  // DIMTAD == 3
                  rx = (arcR - dGap) * cos(ang) + vertex[X];
                  ry = (arcR - dGap) * sin(ang) + vertex[Y];
                  if (ang < DIM_270_DEG - CMD_FUZZ)
                    {
                      plink->textmidpt[X] = rx + txtbox[1][X];
                      plink->textmidpt[Y] = ry + txtbox[1][Y];
                    }
                  else if (fabs(ang - DIM_270_DEG)  < CMD_FUZZ)
                    {
                      plink->textmidpt[Y] += GetGap();
                    }
                  else
                    {
                      plink->textmidpt[X] = rx - txtbox[1][X];
                      plink->textmidpt[Y] = ry + txtbox[1][Y];
                    }
                }
              return true;
        }
      /////// Under construction to here ///////
    }
  else
    {
      dGap = GetGap();
      switch (plinkvar[DIMTAD].dval.ival)
        {
          case 1:
          case 3:
              if (ang < IC_PI)
                {
                  dMove = dGap;
                }
              else
                {
                  dMove = -dGap;
                }
              break;
          case 2:
              dMove = dGap;
              break;
        }

      if (dMove)
        {
          double r = sds_distance(vertex, plink->textmidpt);
          r += dMove;
          sds_polar(vertex, ang, r, plink->textmidpt);
          return true;
        }
    }

  return false;
}

static bool
MoveDimAngTextOut(
  struct cmd_dimeparlink*   plink,
  sds_real                  dAngToh
)
{
  double dMove = 0.0;
  double dGap = GetGap();

  if (dAngToh > IC_PI * 2)
    {
      dAngToh -= IC_PI * 2;
    }
  if (dAngToh < 0)
    {
      dAngToh += IC_PI * 2;
    }

  if (plinkvar[DIMTOH].dval.ival)
    {
      switch (plinkvar[DIMTAD].dval.ival)
        {
          case 1:
          case 3:
              dMove = dGap;
              break;
          case 2:
              if (dAngToh < IC_PI)
                {
                  dMove = dGap;
                }
              else
                {
                  dMove = -dGap;
                }
              break;
        }

      if (dMove)
        {
          plink->textmidpt[Y] += dMove;
          return true;
        }
    }
  else
    {
      switch (plinkvar[DIMTAD].dval.ival)
        {
          case 1:
          case 3:
              if (dAngToh < IC_PI)
                {
                  dMove = -DIM_90_DEG;
                }
              else
                {
                  dMove = DIM_90_DEG;
                }
              break;
          case 2:
              dMove = DIM_90_DEG;
              break;
        }

      if (dMove)
        {
          dAngToh += dMove;
          ads_polar(plink->textmidpt, dAngToh, dGap, plink->textmidpt);
          return true;
        }
    }

  return false;
}
// ]-EBATECH(watanabe)

// Calculate the point B part for the opposit side of the first vector.
void
CalculateDimAngOutside(
  struct cmd_dimeparlink*   plink,
  sds_real  fr1,
  sds_real  fr4,
  sds_real  anglex[],
  sds_real  arlistcomp[],
  sds_real  angle13,
  sds_real  angle14,
  sds_point ptDimL[],
  sds_point txtbox[],
  sds_point vertex,
  sds_point pt13,
  sds_point pt14,
  short&    interseccnt,
  BOOL&     addlineflag,
  bool&     bDrawOutside,
  sds_real& dAngToh,
  BOOL      ArrowFit
)
{
  sds_point ptt[4];
  short     fi1, fi2;

  sds_real fr2 = 1 - ((((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)*2)*(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval))/(2*(fr1*fr1)));
  sds_real fr3 = sqrt(1 - fr2 * fr2);
  ptt[0][0]=((ptDimL[(!ArrowFit)?1:0][0]-vertex[0])*fr2-((ptDimL[(!ArrowFit)?1:0][1]-vertex[1])*fr3)*((interseccnt)?-1:1))+vertex[0];
  ptt[0][1]=((ptDimL[(!ArrowFit)?1:0][1]-vertex[1])*fr2+((ptDimL[(!ArrowFit)?1:0][0]-vertex[0])*fr3)*((interseccnt)?-1:1))+vertex[1];

  ic_normang(&angle13,&angle14);
  anglex[0]=angle13;
  anglex[1]=angle14;

  anglex[2]=sds_angle(vertex,ptt[0]);
  if (plink->flags&DIM_ANGULAR) {
      // Make sure that all the vectors angles are greater then anglex[2].
      ic_normang(&anglex[2],&arlistcomp[0]);
      ic_normang(&anglex[2],&arlistcomp[1]);
      ic_normang(&anglex[2],&arlistcomp[2]);
      ic_normang(&anglex[2],&arlistcomp[3]);
      // This example are two lines in the shape of a (V) the dashed lines are imaginary.
      //
      //   \        /             arlistcomp are the four possble angles made by all four vectors.
      //    \      /              anglex[2] Can be inbetween any of the four vectors,So I need to
      //     \    /               find out what vector is the closest going clockwise.
      //      \  /
      //       \/
      //        \
      //      /
      //          \
      //    /
      //            \
      // This finds the closest angle to anglex[2]. This works becouse all arlistcomp angles were made larger than anglex[2].
      for (fi2=0,fi1=1;fi1<4;++fi1) { if (arlistcomp[fi1]<arlistcomp[fi2]) fi2=fi1; }
      anglex[3]=arlistcomp[fi2];
  } else {
      anglex[3]=sds_angle(vertex,((ArrowFit)?pt13:pt14));
  }
  sds_polar(vertex,anglex[2],fr1,ptDimL[2]);

  // EBATECH(watanabe)-[
  //sds_polar(ptDimL[2],IC_PI,(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval),ptDimL[3]);
  //if (!plink->GenInitflg) sds_polar(ptDimL[2],IC_PI,txtbox[1][0],plink->textmidpt);
  sds_real dDimasz = plinkvar[DIMASZ].dval.rval * plinkvar[DIMSCALE].dval.rval;
  sds_point pt;
  if (plinkvar[DIMTOH].dval.ival)
    {
      dAngToh = 0;
      if (angle13 > IC_PI)
        {
          dAngToh = IC_PI;
        }
      sds_polar(ptDimL[2], dAngToh, dDimasz, ptDimL[3]);
      sds_point_set(ptDimL[3], pt);
      addlineflag = TRUE;
    }
  else
    {
      dAngToh = anglex[3] * 2;
      dAngToh -= fr4;
      sds_point_set(ptDimL[2], pt);
    }

  if (!plink->GenInitflg)
    {
      sds_polar(pt, dAngToh, txtbox[1][0], plink->textmidpt);
    }
  // ]-EBATECH(watanabe)

  // EBATECH(watanabe)-[support DIMTAD
  bool b = MoveDimAngTextOut(plink, dAngToh);
  if (b)
    {
      sds_point pp;
      if (plinkvar[DIMTOH].dval.ival)
        {
          sds_point_set(ptDimL[3], pp);
        }
      else
        {
          sds_point_set(ptDimL[2], pp);
          addlineflag = TRUE;
        }
      sds_polar(pp, dAngToh, txtbox[1][0] * 2, ptDimL[3]);
    }
  // EBATECH(watanabe)

  interseccnt=3;

  // EBATECH(watanabe)-[
  //addlineflag=TRUE;
  bDrawOutside = true;
  // ]-EBATECH(watanabe)
}

// ============================================================================================================================================================

//      (8 "LAYER2")
//      (10 6.96911 4.93152 0.0)
//      (11 7.2329 4.54474 0.0)
//      (12 0.0 0.0 0.0)
//      (70 165)
//      (1 "")
//      (52 0.0)
//      (53 0.0)
//      (54 0.0)
//      (51 0.0)
//      (210 0.0 0.0 1.0)
//      (3 "STANDARD")
//      (13 7.79567 7.58736 0.0)
//      (14 8.52471 3.39418 0.0)
//      (15 4.38067 2.88281 0.0)
//      (16 0.0 0.0 0.0)
//      (40 0.0)
//      (50 0.0)

BOOL cmd_Angular_A3P(struct cmd_dimeparlink *plink,db_drawing *flp,short dragmode) {
    sds_point      ptExtL[4]={{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0}},
                   ptDimL[4]={{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0}},
                   ptt[4]   ={{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0}},
                   xpt[8]   ={{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0}},
                   nbox[4]  ={{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0}},
                   vertex = {0.0, 0.0, 0.0},
                   pt14 = {0.0, 0.0, 0.0},
                   pt13 = {0.0, 0.0, 0.0},
                   txtbox[2] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}},
                   txtins = {0.0, 0.0, 0.0},
                   boxrect[4] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}},
                   ipt[2] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};
    sds_real       angle13 = 0.0,
                   angle14 = 0.0,
                   angle10 = 0.0,
                   angleof = 0.0,
                   arlistcomp[4] = {0.0, 0.0, 0.0, 0.0},
                   arlist[4] = {0.0, 0.0, 0.0, 0.0},
                   anglex[8] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
                   fr1 = 0.0,
                   fr2 = 0.0,
                   fr3 = 0.0,
                   fr4 = 0.0,
                   fr5 = 0.0,
                   cosofdeg = 0.0,
                   sinofdeg = 0.0;
    short          fi1 = 0,
                   fi2 = 0,
                   fi3 = 0,
                   interseccnt = 0,
                   Index13 = 0,
                   Index14 = 0;
    char           text[IC_ACADBUF] = ""/*DNT*/;
    BOOL           ArrowFit = FALSE,
                   OffFlgL13 = TRUE,
                   OffFlgL14 = TRUE,
                   Flipflag13 = TRUE,
                   Flipflag14 = TRUE,
                   addlineflag = FALSE,
                   ArrowSideflg = FALSE,
                   Switchangle = FALSE;
    db_handitem   *handitem = NULL;
    DWORD          wordval = 0;
    struct resbuf *rbb=NULL;
    int            ret = 0;
    BYTE          *bPtr = NULL;

    // If angular then (plink->ptt) are the end points of each of the two segments selected.
   if (plink->flags&DIM_ANGULAR) {
       ic_ptcpy(plink->ptt[0],plink->pt13);
       ic_ptcpy(plink->ptt[1],plink->pt14);
       ic_ptcpy(plink->ptt[2],plink->defpt);
       ic_ptcpy(plink->ptt[3],plink->pt15);
       if (ic_linexline(plink->ptt[0],plink->ptt[1],plink->ptt[2],plink->ptt[3],vertex)<0) return(FALSE);
    } else {
    // If 3 point angular then (plink->pt15) holds the vertex of the angular dim.
        ic_ptcpy(vertex,plink->pt15);
    }
    angle10=sds_angle(vertex,((plink->flags&DIM_ANGULAR)?plink->pt16:plink->defpt));

	// EBATECH(CNBR) -[ Bugzilla#78455 13-03-2003 DIMTMOVE=1,2
	if( plink->HomeText == TRUE || plinkvar[DIMTMOVE].dval.ival == 0 )
	{
		// If the plink->textmidpt is beging dragged then project the (plink->pt16 or plink->defpt) point the distance palink->textmidpt
		// and the plink->pt15 will make but at the same angle made by the old (plink->pt16 or plink->defpt) point. so that the new
		// (plink->pt16 or plink->defpt) is just moved on the same angle but at a new distance from the center.
		if (plink->DefValidflg || SDS_AtNodeDrag==2) {
			if (SDS_AtNodeDrag!=2) {
				fr1=sds_angle(vertex,plink->textmidpt);
				if (plink->flags&DIM_ANGULAR) {
					sds_polar(vertex,fr1,sds_distance(vertex,plink->pt16),plink->textmidpt);
				} else {
					sds_polar(vertex,fr1,sds_distance(vertex,plink->defpt),plink->textmidpt);
				}
			} else {
				if (plink->flags&DIM_ANGULAR) {
//					sds_polar(plink->pt15,angle10,sds_distance(plink->pt15,plink->textmidpt),plink->pt16);
					sds_polar(vertex,angle10,sds_distance(vertex/*plink->pt15*/,plink->textmidpt),plink->pt16);
				} else {
					sds_polar(vertex,angle10,sds_distance(plink->pt15,plink->textmidpt),plink->defpt);
				}
			}
		}
	}
	// EBATECH(CNBR) ]-

    // Get the angles.
    // pt13 : always the first point picked.
    // pt14 : always the seconed point picked.
    //
    //        _________________ (PT13) or (PT14)
    //       |     ____________ (DEFPT)
    //       |    |
    //       .----.----._______ (PT13) or (PT14)
    //        \       /
    //         \     /
    //          \   /
    //           \ /
    //            .____________ (PT15)
    //
    angle13=sds_angle(vertex,plink->pt13);
    if (plink->flags&DIM_ANGULAR) {
        if (sds_distance(plink->pt13,vertex)<CMD_FUZZ) {
            ic_ptcpy(pt13,vertex);
            angle13=sds_angle(vertex,plink->pt14);
        } else ic_ptcpy(pt13,plink->pt13);
    } else {
        ic_ptcpy(pt13,plink->pt13);
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // The problem here is what if the vertex and plink->pt14 are the same point. ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    if (plink->flags&DIM_ANGULAR) {
        angle14=sds_angle(plink->pt15,plink->defpt);
        if (sds_distance(plink->pt15,vertex)<CMD_FUZZ) ic_ptcpy(pt14,/*plink->pt14*/plink->pt15); else ic_ptcpy(pt14,plink->defpt);
    } else {
        angle14=sds_angle(vertex,plink->pt14);
        ic_ptcpy(pt14,plink->pt14);
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    // calculate the four angles that are made up by the two vectors
    //     __ Other side of Vector II
    //     \    |
    //          |   __ Vector I
    //       \  |  /
    //          | /
    //         \|/
    // --------- -----------
    //         /|\
    //          | \
    //       /  |  \__ Vector II
    //          |
    //     /    |
    //     ---- Other side of Vector I
    //
    if (plink->flags&DIM_ANGULAR) {
        // Calculate angles for all four vectors.
		// First segment picked.
        arlistcomp[0]=arlist[0]=angle13;
		// Seconed segment picked.
        arlistcomp[1]=arlist[1]=angle14;
		// Segment 180 degrees from first segment picked.
        arlistcomp[2]=arlist[2]=angle13+IC_PI;
		// Segment 180 degrees from seconed segment picked.
        arlistcomp[3]=arlist[3]=angle14+IC_PI;
        // Make sure that all the vectors angles are greater then angle13.
        ic_normang(&arlist[0],&arlist[1]);
        ic_normang(&arlist[0],&arlist[2]);
        ic_normang(&arlist[0],&arlist[3]);
        ic_normang(&arlist[0],&angle10);
        // Make sure the angles in arlistcomp are normalized.
        for (fi1=0;fi1<4;++fi1) ic_normang(&arlistcomp[fi1],NULL);
        // Sort the angles from lowest.
        qsort((void *)arlist,4,sizeof(sds_real),cmd_compare1);
        // Calculate which angle is angle10 between. so parse through all four angles.
        for (fi1=0;fi1<4;++fi1) {
            if (fi1==3) {
		        // If the arlist is the last angle and angle10 is greater than the last angle then break;
                if (angle10>arlist[fi1]) break;
            } else {
	            // If the angle is not the last angle and angle10 is between the current angle and the next angle.
                if (angle10>arlist[fi1] && angle10<arlist[fi1+1]) break;
            }
        }
        if (fi1<4) {
            if (fi1==3) {
	            // If the arlist is the last angle then set angle13 to the first and angle14 to the last angle.
                angle13=arlist[0];
                angle14=arlist[3];
            } else {
	            // If the arlist is not the last angle then set angle13 to the current arlist and angle14 to the next arlist.
                angle13=arlist[fi1];
                angle14=arlist[fi1+1];
            }
        }
        // Normalize the angles.
        ic_normang(&angle13,NULL);
        ic_normang(&angle14,NULL);
    }
    // Given three vectors find the closest counter clockwise vector to vector (C)=plink->defpt.
    // Example.
    //
    //     (C)  |
    //       \  |
    //        \ |
    //         \|
    // --------- ---------
    //         /|\
    //        / | \
    //       /  |  \
    //     (A)  |  (B)
    //
    //
    fr1=angle10; fr2=angle13; fr3=angle14;

    ic_normang(&fr2,&fr3);
    fr4=fr3-fr2;
    if (fr4<=IC_PI) {
        ic_normang(&fr2,&fr1);
        if (fr1>fr2 && fr1<fr3) {
            ArrowFit=TRUE;
            Switchangle=TRUE;
        } else {
            ArrowFit=FALSE;
            Switchangle=TRUE;
        }
    } else {
        fr4=fr3; fr3=fr2; fr2=fr4;
        ic_normang(&fr2,&fr1);
        ic_normang(&fr2,&fr3);
        if (fr1>fr2 && fr1<fr3) ArrowFit=FALSE; else ArrowFit=TRUE;
    }
/* ALPO SOMEWHAT WORKING OLD STUFF.
    fr1=angle10; fr2=angle13; fr3=angle14;
    // Change so that fr1 is greater then fr2.
    ic_normang(&fr2,&fr1);
    // Get the difference between angle fr1 and angle fr2.
    fr4=fr1-fr2;
    // Change so that fr1 is greater than fr3.
    ic_normang(&fr3,&fr1);
    // Get the difference between angle fr1 and angle fr3.
    fr5=fr1-fr3;
    // If fr4 = (the difference in angle13 and angle10). is less than fr5 = (the difference in angle14 and angle10).
    // This will tell us in a counter clockwise rotation which angle is smaller and larger, starting from 0 to 360 degrees.
    ArrowFit=(fr4<fr5)?TRUE:FALSE;
*/
    // Get the radius made by the vertex and defpt.
    fr1=sds_distance(vertex,((plink->flags&DIM_ANGULAR)?plink->pt16:plink->defpt));
    // Get the radius made by the vertex and pt13
    fr2=sds_distance(vertex,pt13);
    // Get the radius made by the vertex and pt14
    fr3=sds_distance(vertex,pt14);
    // Calculate other end of the extension lines.
    //       .         .
    //           __________ptDimL[0 or 1]
    //         \|_.__/_____ptDimL[1 or 0]
    //          \___/______ptExtL[1 or 3]
    //          |__________ptExtL[3 or 1]
    //            .
    //
    sds_polar(vertex,angle13,(fr1+((fr2<fr1)?(plinkvar[DIMEXE].dval.rval*plinkvar[DIMSCALE].dval.rval):-(plinkvar[DIMEXE].dval.rval*plinkvar[DIMSCALE].dval.rval))),ptExtL[1]);
    sds_polar(vertex,angle14,(fr1+((fr3<fr1)?(plinkvar[DIMEXE].dval.rval*plinkvar[DIMSCALE].dval.rval):-(plinkvar[DIMEXE].dval.rval*plinkvar[DIMSCALE].dval.rval))),ptExtL[3]);
    sds_polar(vertex,angle13,fr1,ptDimL[0]);
    sds_polar(vertex,angle14,fr1,ptDimL[1]);

    if (plink->flags&DIM_ANGULAR) {
        // If the anglular dimension is not on a cirlce,arc or 3point, then we need to create the four angle the lines that were picked
        // create. There are only four possable angles to be made by two line segments, but there are many type of combonations. This
        // drawing is trying to show just three of the combonations.
        //                           _____________ Exp arc 1.
        //                          /
        //         .______________________.
        //                    |
        //                    |
        //  Line A --\        |
        //            \       |        /-- Line B
        //             \      |       /
        //              \     |      /
        //               \    |  _____________________ Exp arc 2.
        //                \   | /
        //                 .______.
        //                    |
        //  ------------------.-------------------
        //                    |
        //               __________________ Line drawn by Exp arc 3.
        //              |     |
        //              | /   |   \
        //              |     |       _____ Line drawn by Exp arc 3.
        //              /     |     \
        //                    |
        //            /       |       \
        //                    |
        //          /         |         \
        //         ._____________________.
        //                          \________________ Exp arc 3.
        //
        // set ptt[0][0]=radius ptt[0][1]=start angle ptt[0][2]=end angle.
        ptt[0][0]=fr1; ptt[0][1]=(ArrowFit)?angle13:angle14; ptt[0][2]=(ArrowFit)?angle14:angle13;
        // If angle13 is the same
        if ((angle13+CMD_FUZZ)>=arlistcomp[1] && (angle13-CMD_FUZZ)<=arlistcomp[1]) {        // angle13=angle14a.
            Flipflag13=FALSE;
        } else if ((angle13+CMD_FUZZ)>=arlistcomp[2] && (angle13-CMD_FUZZ)<=arlistcomp[2]) { // angle13=angle13b.
            Flipflag14=FALSE;
        } else if (angle13==arlistcomp[3]) {                                                 // angle13=angle14b
            Flipflag13=FALSE;
        }
        // At this point there are only two vectors and for each vector a start point and end point.
        // plink->ptt[0] to plink->ptt[1] is one line segment and plink->ptt[2] to plink->ptt[3] is another, we already know
        // which one we are working on at this point but we don't know which end of the line segment is closest to the cross point or ptDimL[0].
        // so we set the index of the closest point. so Index13 could be one of the following (0 or 2) or (1 or 3).

        Index13=(sds_distance(ptDimL[0],plink->ptt[(Flipflag13)?0:2])<sds_distance(ptDimL[0],plink->ptt[(Flipflag13)?1:3]))?((Flipflag13)?0:2):((Flipflag13)?1:3);

        // Find the points where the arc and the line segments cross.
        ret=ic_segxseg(plink->ptt[(Flipflag13)?0:2],plink->ptt[(Flipflag13)?1:3],0,vertex,ptt[0],1,CMD_FUZZ,ptt[1],ptt[2]);
        // If the arc crossed the line segment then turn off the dimension line.
        if (ret==1 || ret==2) OffFlgL13=FALSE;
        else {
            // Set ptExtL[0] to the center of the angle. This is an example of one side of an angular dimension line.
            // This area will test whether the point is above or below the line segment.
            //             /
            //                Above line segment.
            //           /
            //           ___
            //         /    |
            //        /     |_ Line
            //       /      |
            //      /_______|
            //
            //    /
            //      Below line segment.
            //  /
            // .
            //
            // Is the point on the (Below line segment).
        	// icadDirectionEqual : returns 1 if vector p1-p2 is same direction as vector p3-p4, 0 otherwise
            if (icadDirectionEqual(vertex,plink->ptt[(Flipflag13)?0:2],vertex,ptDimL[0]) &&
                icadDirectionEqual(plink->ptt[(Flipflag13)?0:2],vertex,plink->ptt[(Flipflag13)?0:2],ptDimL[0])) {
                sds_polar(plink->ptt[Index13],(ptt[0][1]+IC_PI),(plinkvar[DIMEXO].dval.rval*plinkvar[DIMSCALE].dval.rval),ptExtL[0]);
            // Is the point on the (Above line segment).
            } else if (sds_distance(ptDimL[0],plink->ptt[Index13])<sds_distance(ptDimL[0],vertex)) {
                if (icadDirectionEqual(plink->ptt[(Flipflag13)?1:3],plink->ptt[(Flipflag13)?0:2],plink->ptt[(Flipflag13)?1:3],vertex) &&
                    icadDirectionEqual(plink->ptt[(Flipflag13)?0:2],plink->ptt[(Flipflag13)?1:3],plink->ptt[(Flipflag13)?0:2],vertex) &&
                    (!icadPointEqual(vertex,plink->ptt[(Flipflag13)?1:3]))) {
                    sds_polar(plink->ptt[Index13],ptt[0][(ArrowFit)?1:2],(plinkvar[DIMEXO].dval.rval*plinkvar[DIMSCALE].dval.rval),ptExtL[0]);
                } else {
                    sds_polar(plink->ptt[Index13],ptt[0][(ArrowFit)?1:2],(plinkvar[DIMEXO].dval.rval*plinkvar[DIMSCALE].dval.rval),ptExtL[0]);
                }
            // Then the point must be on another side of the center point.
            } else {
                // Calculate other end of the extension lines.
                //       .         .
                //
                //         \     /
                //          \___/______ptExtL[1 or 3]
                //          |__________ptExtL[3 or 1]
                //            .
                //
                if (icadDirectionEqual(plink->ptt[(Flipflag13)?1:3],plink->ptt[(Flipflag13)?0:2],plink->ptt[(Flipflag13)?1:3],vertex) &&
                    icadDirectionEqual(plink->ptt[(Flipflag13)?0:2],plink->ptt[(Flipflag13)?1:3],plink->ptt[(Flipflag13)?0:2],vertex)) {
                    sds_polar(plink->ptt[Index13],ptt[0][1],(plinkvar[DIMEXO].dval.rval*plinkvar[DIMSCALE].dval.rval),ptExtL[0]);
                } else {
                    sds_polar(vertex,ptt[0][1],(plinkvar[DIMEXO].dval.rval*plinkvar[DIMSCALE].dval.rval),ptExtL[0]);
                    sds_polar(vertex,ptt[0][1],(fr1+(plinkvar[DIMEXO].dval.rval*plinkvar[DIMSCALE].dval.rval)),ptExtL[1]);
                }
            }
        }
        // set ptt[0][0]=radius ptt[0][1]=start angle ptt[0][2]=end angle.
        ptt[0][0]=fr1; ptt[0][1]=(ArrowFit)?angle13:angle14; ptt[0][2]=(ArrowFit)?angle14:angle13;
        // If angle14 is the same
        if ((angle14+CMD_FUZZ)>=arlistcomp[1] && (angle14-CMD_FUZZ<=arlistcomp[1])) {
            Flipflag14=FALSE;
        } else if ((angle14+CMD_FUZZ)>=arlistcomp[3] && (angle14-CMD_FUZZ<=arlistcomp[3])) {
            Flipflag14=FALSE;
        }
        // At this point there are only two vectors and for each vector a start point and end point.
        // plink->ptt[0] to plink->ptt[1] is one line segment and plink->ptt[2] to plink->ptt[3] is another, we already know
        // which one we are working on at this point but we don't know which end of the line segment is closest to the cross point or ptDimL[0].
        // so we set the index of the closest point. so Index14 could be one of the following (0 or 2) or (1 or 3).
        Index14=(sds_distance(ptDimL[1],plink->ptt[(Flipflag14)?0:2])<sds_distance(ptDimL[1],plink->ptt[(Flipflag14)?1:3]))?((Flipflag14)?0:2):((Flipflag14)?1:3);
        // Find the points where the arc and the line segments cross.
        ret=ic_segxseg(plink->ptt[(Flipflag14)?0:2],plink->ptt[(Flipflag14)?1:3],0,vertex,ptt[0],1,CMD_FUZZ,ptt[1],ptt[2]);
        // If the arc crossed the line segment then turn off the dimension line.
        if (ret==1) OffFlgL14=FALSE;
        else {
            // Is the point on the (Below line segment).
            if (icadDirectionEqual(vertex,plink->ptt[(Flipflag14)?0:2],vertex,ptDimL[1]) &&
                icadDirectionEqual(plink->ptt[(Flipflag14)?0:2],vertex,plink->ptt[(Flipflag14)?0:2],ptDimL[1])) {
                sds_polar(plink->ptt[Index14],(ptt[0][(ArrowFit)?2:1]+IC_PI),(plinkvar[DIMEXO].dval.rval*plinkvar[DIMSCALE].dval.rval),ptExtL[2]);
            // Is the point on the (Above line segment).
            } else if (sds_distance(ptDimL[1],plink->ptt[Index14])<sds_distance(ptDimL[1],vertex)) {
                sds_polar(plink->ptt[Index14],ptt[0][(ArrowFit)?2:1],(plinkvar[DIMEXO].dval.rval*plinkvar[DIMSCALE].dval.rval),ptExtL[2]);
            } else {
                // Calculate other end of the extension lines.
                //       .         .
                //
                //         \     /
                //          \___/______ptExtL[1 or 3]
                //          |__________ptExtL[3 or 1]
                //            .
                //
                sds_polar(vertex,ptt[0][2],(plinkvar[DIMEXO].dval.rval*plinkvar[DIMSCALE].dval.rval),ptExtL[2]);
                sds_polar(vertex,ptt[0][2],(fr1+(plinkvar[DIMEXO].dval.rval*plinkvar[DIMSCALE].dval.rval)),ptExtL[3]);
            }
        }
        // Get the angle of the dimension depending on where the (plink->defpt).
        if (icadPointEqual(vertex,plink->pt16)) {
            angleof=cmd_CalculateAngle(vertex,plink->ptt[0],plink->ptt[2]);
        } else {
            angleof=cmd_CalculateAngle(vertex,ptDimL[0],ptDimL[1]);
        }
    } else {
        // Calculate the distance between the point picked and where the extension line would start.
        //       .---------._
        //                 __|- Distance=DIMEXO
        //         \     /
        //          \   /
        //           \ /
        //            .
        //
        sds_polar(pt13,((fr2<fr1)?angle13:angle13+IC_PI),(plinkvar[DIMEXO].dval.rval*plinkvar[DIMSCALE].dval.rval),ptExtL[0]);
        sds_polar(pt14,((fr3<fr1)?angle14:angle14+IC_PI),(plinkvar[DIMEXO].dval.rval*plinkvar[DIMSCALE].dval.rval),ptExtL[2]);
        angleof=cmd_CalculateAngle(vertex,pt13,pt14);
        if (!Switchangle) {
            angleof=(ArrowFit)?((IC_PI*2)-angleof):angleof;
        } else {
            angleof=(!ArrowFit)?((IC_PI*2)-angleof):angleof;
        }
    }
    // Set the value DIMVALUE to the angle of the dimension.
    plink->dimvalue=angleof;
    // calculate the midpoint of the arc so we know where the placement of the textmidpoint is going to be.
	 // EBATECH(CNBR) 2003-03-27 Fix to wrong dimention position when create or home text.
	if (SDS_AtNodeDrag!=2 && !plink->DefValidflg && !plink->GenInitflg && plink->HomeText /* dragmode == 1*/)
	{
			sds_polar(vertex,(((ArrowFit)?angle13:angle14)+(angleof/2)),fr1,plink->textmidpt);
	}
    interseccnt=0;

    // EBATECH(watanabe)-[
    bool bDrawOutside;
    bDrawOutside = false;
    sds_real dAngTih, dAngToh;
    if (plinkvar[DIMTIH].dval.ival)
      {
        dAngTih = 0.0;
      }
    else
      {
        dAngTih  = sds_angle(vertex, plink->textmidpt);
        dAngTih -= IC_PI / 2;
        if (dAngTih > IC_PI / 2)
          {
            dAngTih -= IC_PI;
          }
      }
    // ]-EBATECH(watanabe)

    // This will loop a seconed time if the text will not fit between the arrows or the text is on top of an arrow.
    for (fi1=0;fi1<4;++fi1) ic_ptcpy(ptt[fi1],dimzeropt);
    do {
        // Calculate the angle of the line made by point A and B. This is the arrow head.
        // And the ange made from the center point of the dimension and point B represented by line S.
        //
        //  Point A  .
        //      |   /       __ Line S
        //      |  /       /
        //      | /       /
        //      |________/
        //      /\      /
        //     /  \    /
        //    /    \  /.__________ Point B
        //   /      \/   .
        //  /       /      .
        //         /
        //        /
        //
        // Calculate the point B part for both side of the anglular dimsenion.
        fr2=1-((((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)*2)*((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)*2))/(2*(fr1*fr1)));
        fr3=sqrt((1-(fr2*fr2)));
        ptt[0][0]=((ptDimL[(!ArrowFit)?0:1][0]-vertex[0])*fr2+((ptDimL[(!ArrowFit)?0:1][1]-vertex[1])*fr3)*((interseccnt)?-1:1))+vertex[0];
        ptt[0][1]=((ptDimL[(!ArrowFit)?0:1][1]-vertex[1])*fr2-((ptDimL[(!ArrowFit)?0:1][0]-vertex[0])*fr3)*((interseccnt)?-1:1))+vertex[1];
        ptt[1][0]=((ptDimL[(!ArrowFit)?1:0][0]-vertex[0])*fr2-((ptDimL[(!ArrowFit)?1:0][1]-vertex[1])*fr3)*((interseccnt)?-1:1))+vertex[0];
        ptt[1][1]=((ptDimL[(!ArrowFit)?1:0][1]-vertex[1])*fr2+((ptDimL[(!ArrowFit)?1:0][0]-vertex[0])*fr3)*((interseccnt)?-1:1))+vertex[1];
        // Calculate the angle S for both sides of the dimension.
        arlist[0]=sds_angle(vertex,ptt[1]);
        arlist[1]=sds_angle(vertex,ptt[0]);
        // Calculate the point beyond B part for both side of the anglular dimsenion.
        fr2=1-(((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)*(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval))/(2*(fr1*fr1)));
        fr3=sqrt((1-(fr2*fr2)));
        ptt[0][0]=((ptDimL[(!ArrowFit)?0:1][0]-vertex[0])*fr2+((ptDimL[(!ArrowFit)?0:1][1]-vertex[1])*fr3)*((interseccnt)?-1:1))+vertex[0];
        ptt[0][1]=((ptDimL[(!ArrowFit)?0:1][1]-vertex[1])*fr2-((ptDimL[(!ArrowFit)?0:1][0]-vertex[0])*fr3)*((interseccnt)?-1:1))+vertex[1];
        ptt[1][0]=((ptDimL[(!ArrowFit)?1:0][0]-vertex[0])*fr2-((ptDimL[(!ArrowFit)?1:0][1]-vertex[1])*fr3)*((interseccnt)?-1:1))+vertex[0];
        ptt[1][1]=((ptDimL[(!ArrowFit)?1:0][1]-vertex[1])*fr2+((ptDimL[(!ArrowFit)?1:0][0]-vertex[0])*fr3)*((interseccnt)?-1:1))+vertex[1];
        // Calculate the angle S for both sides of the dimension.
        angle13=fr3=sds_angle(vertex,ptt[1]);
        angle14=fr2=sds_angle(vertex,ptt[0]);
        // Calculate the angle made by points A and B. for both side of the dimension.
        fr4=sds_angle(ptDimL[0],ptt[(!ArrowFit)?0:1]);
        fr5=sds_angle(ptDimL[1],ptt[(!ArrowFit)?1:0]);
        if (interseccnt && interseccnt<3) {
            // Make the arrows and arcs outside of the angle.
            fr2=1-((((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)*2)*((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)*2))/(2*(fr1*fr1)));
            fr3=sqrt((1-(fr2*fr2)));
            ptt[0][0]=(ptDimL[(!ArrowFit)?0:1][0]-vertex[0])*fr2-(ptDimL[(!ArrowFit)?0:1][1]-vertex[1])*fr3+vertex[0];
            ptt[0][1]=(ptDimL[(!ArrowFit)?0:1][1]-vertex[1])*fr2+(ptDimL[(!ArrowFit)?0:1][0]-vertex[0])*fr3+vertex[1];
            ptt[1][0]=(ptDimL[(!ArrowFit)?1:0][0]-vertex[0])*fr2+(ptDimL[(!ArrowFit)?1:0][1]-vertex[1])*fr3+vertex[0];
            ptt[1][1]=(ptDimL[(!ArrowFit)?1:0][1]-vertex[1])*fr2-(ptDimL[(!ArrowFit)?1:0][0]-vertex[0])*fr3+vertex[1];
            // Calculate the angle S for both sides of the dimension.
            anglex[0]=angle14;
            anglex[1]=sds_angle(vertex,ptt[0]);
            anglex[2]=sds_angle(vertex,ptt[1]);
            anglex[3]=angle13;
            interseccnt=3;

            // EBATECH(watanabe)-[support DIMTAD
            bool b = MoveDimAngTextIn(plink, txtbox, vertex);
            if (b)
              {
                anglex[4] = angle13;
                anglex[5] = angle14;
                interseccnt = 5;
              }
            // ]-EBATECH(watanabe)

            break;
        } else {
            // Generate string for dimension.
            cmd_genstring(plink,text,angleof,""/*DNT*/,FALSE,flp);
            // Set the value DIMVALUE to the width of the dimension.
            plink->dimvalue=angleof;
            // Buld the text insert.
            if ((rbb=sds_buildlist(RTDXF0,"MTEXT"/*DNT*/,1,text,10, plink->textmidpt, 50,0.0,40,plinkvar[DIMTXT].dval.rval*plinkvar[DIMSCALE].dval.rval,41,0.0,51,plink->hdir,7,plinkvar[DIMTXSTY].dval.cval,71,5,72,1,0))==NULL) return(RTERROR);
            // get the width and height of the text.
            sds_textbox(rbb,txtbox[0],txtbox[1]);
            // free resbuf link list.
            IC_RELRB(rbb);
            // Caluculate the distance buffer around the text.
            txtbox[1][0]+=(fabs(plinkvar[DIMGAP].dval.rval*plinkvar[DIMSCALE].dval.rval)*2); txtbox[1][1]+=(fabs(plinkvar[DIMGAP].dval.rval*plinkvar[DIMSCALE].dval.rval)*2);
            // save half the height and half the width of the textbox in txtcntr;
            ptt[0][0]=(txtbox[1][0]/2); ptt[0][1]=(txtbox[1][1]/2);
            // Move box (X) dir. txtcntr[0] and (Y) dir. txtcntr[1].
            txtbox[0][0]-=ptt[0][0]; txtbox[0][1]-=ptt[0][1]; txtbox[1][0]-=ptt[0][0]; txtbox[1][1]-=ptt[0][1];

            // EBATECH(watanabe)-[function
            //// Get the points of intersections with the box and the arc.
            //// xpt is an array of intersection points of the box with the arc. dIntvalue holds the values
            //// for what type of intersection it was. Create the angles made by these intersections.
            //
            //// EBATECH(watanabe)-[
            ////wordval=cmd_getboxinsersecArc(txtbox,0.0,plink->textmidpt,vertex,fr1,arlist[0],arlist[1],xpt,nbox);
            //wordval = cmd_getboxinsersecArc(
            //            txtbox,
            //            dAngTih,
            //            plink->textmidpt,
            //            vertex,
            //            fr1,
            //            arlist[0],
            //            arlist[1],
            //            xpt,
            //            nbox);
            //// ]-EBATECH(watanabe)
            //
            //// First angle.
            //anglex[0]=angle13;
            //for (bPtr=(BYTE *)&wordval,fi1=1,fi2=0;fi2<8;fi2+=2,++bPtr) {
            //    if (*bPtr==(byte)0xff) {                    // -1 : No intersections
            //        continue;
            //    } else if (*bPtr==(BYTE)0x0f) {             // 2 intersections on one line.
            //        anglex[fi1++]=sds_angle(vertex,xpt[fi2]);
            //        anglex[fi1++]=sds_angle(vertex,xpt[fi2+1]);
            //    } else if ((*bPtr&0x08) && (*bPtr&0x04)) {  // Intersection on second line and arc.
            //        anglex[fi1++]=sds_angle(vertex,xpt[fi2+1]);
            //    } else if ((*bPtr&0x01) && (*bPtr&0x02)) {  // Intersection on first line and arc.
            //        anglex[fi1++]=sds_angle(vertex,xpt[fi2]);
            //    }
            //}
            //// last angle.
            //anglex[fi1]=angle14;
            //// Make sure to make all angles greater then the angle13.
            //for (fi2=0;fi2<fi1;++fi2) { if (anglex[fi2]<angle13) ic_normang(&angle13,&anglex[fi2]); }
            //// Now sort the indexes of the angles becouse they will be out of order.
            //for (interseccnt=fi1,fi2=0;fi2<fi1;++fi2) {
            //    for (fi3=fi2+1;fi3<fi1;++fi3) {
            //        if (anglex[fi3]<anglex[fi2]) {
            //            fr2=anglex[fi2];
            //            anglex[fi2]=anglex[fi3];
            //            anglex[fi3]=fr2;
            //        }
            //    }
            //}
            GetDimAngIntersect(
                    plink, txtbox, dAngTih, vertex, fr1,
                    arlist[0], arlist[1],
                    xpt, angle13, angle14, interseccnt, anglex);
            // ]-EBATECH(watanabe)

            // EBATECH(watanabe)-[if radius is small, text moves to outside.
            sds_point ps, pe;
            sds_polar(vertex, angle13, fr1, ps);
            sds_polar(vertex, angle14, fr1, pe);
            sds_real dist1, dist2;
			
			//BugZilla No. 78102; 10-06-2002
				dist1 = sds_distance(ps, pe);
			
			if (plinkvar[DIMTIH].dval.ival)
                dist2 = fabs(txtbox[0][1] - txtbox[1][1]);
            else
                dist2 = fabs(txtbox[0][0] - txtbox[1][0]);
            
			// EBATECH(CNBR) -[ When arc is less than arrow size.
			sds_real dist3 = 2 * plinkvar[DIMASZ].dval.rval * plinkvar[DIMSCALE].dval.rval;
			if (dist1 < dist3)
			{
				// TODO : Arrow is outside. Draw dimension arc between lines.
				//        If DIMTMOVE=0, text moves below the first line.
				;
			}
			// EBATECH(CNBR) ]-
            if (dist1 < dist2)
              {
                if (!plink->DefValidflg && SDS_AtNodeDrag != 2)
                  {
					// EBATECH(CNBR) -[ DIMTMOVE=1,2 never move outside of angles.
					if( !plinkvar[DIMTMOVE].dval.ival )
					{
                    	CalculateDimAngOutside(
                        	plink, fr1, fr4, anglex, arlistcomp, angle13, angle14,
                        	ptDimL, txtbox, vertex, pt13, pt14, interseccnt,
                        	addlineflag, bDrawOutside, dAngToh, ArrowFit);
                    }
                    // EBATECH(CNBR) ]-
                    break;
                  }
              }

            // ]-EBATECH(watanabe)
        }
		// EBATECH(CNBR) -[ DIMTMOVE=1,2
		if( plinkvar[DIMTMOVE].dval.ival && interseccnt == 1 && SDS_AtNodeDrag == 2)
		{
			break;
		}
        // EBATECH(CNBR) ]-
        // EBATECH(watanabe)-[support DIMTAD
//        if (interseccnt==3) break;
        if (interseccnt == 3)
          {
            bool b = MoveDimAngTextIn(plink, txtbox, vertex);
            if (b)
              {
                anglex[1] = anglex[3];
                interseccnt = 1;
              }
            break;
          }
        // ]-EBATECH(watanabe)

        // EBATECH(watanabe)-[function
        //// Get the points of intersections with the box and the arc.
        //// xpt is an array of intersection points of the box with the arc. dIntvalue holds the values
        //// for what type of intersection it was. Create the angles made by these intersections.
        //
        //// EBATECH(watanabe)-[
        ////wordval=cmd_getboxinsersecArc(txtbox,0.0,plink->textmidpt,vertex,fr1,angle13,angle14,xpt,nbox);
        //wordval = cmd_getboxinsersecArc(
        //                txtbox,
        //                dAngTih,
        //                plink->textmidpt,
        //                vertex,
        //                fr1,
        //                angle13,
        //                angle14,
        //                xpt,
        //                nbox);
        //// ]-EBATECH(watanabe)
        //
        //// First angle.
        //anglex[0]=angle13;
        //for (bPtr=(BYTE *)&wordval,fi1=1,fi2=0;fi2<8;fi2+=2,++bPtr) {
        //    if (*bPtr==(byte)0xff) {                    // -1 : No intersections
        //        continue;
        //    } else if (*bPtr==(BYTE)0x0f) {             // 2 intersections on one line.
        //        anglex[fi1++]=sds_angle(vertex,xpt[fi2]);
        //        anglex[fi1++]=sds_angle(vertex,xpt[fi2+1]);
        //    } else if ((*bPtr&0x08) && (*bPtr&0x04)) {  // Intersection on second line and arc.
        //        anglex[fi1++]=sds_angle(vertex,xpt[fi2+1]);
        //    } else if ((*bPtr&0x01) && (*bPtr&0x02)) {  // Intersection on first line and arc.
        //        anglex[fi1++]=sds_angle(vertex,xpt[fi2]);
        //    }
        //}
        //// last angle.
        //anglex[fi1]=angle14;
        //// Make sure to make all angles greater then the angle13.
        //for (fi2=0;fi2<fi1;++fi2) { if (anglex[fi2]<angle13) ic_normang(&angle13,&anglex[fi2]); }
        //// Now sort the indexes of the angles becouse they will be out of order.
        //for (interseccnt=fi1,fi2=0;fi2<fi1;++fi2) {
        //    for (fi3=fi2+1;fi3<fi1;++fi3) {
        //        if (anglex[fi3]<anglex[fi2]) {
        //            fr2=anglex[fi2];
        //            anglex[fi2]=anglex[fi3];
        //            anglex[fi3]=fr2;
        //        }
        //    }
        //}
        GetDimAngIntersect(
                    plink, txtbox, dAngTih, vertex, fr1,
                    angle13, angle14,
                    xpt, angle13, angle14, interseccnt, anglex);
        // ]-EBATECH(watanabe)

        if (interseccnt!=3 && !plink->DefValidflg && SDS_AtNodeDrag!=2) {
            // EBATECH(watanabe)-[function
            //// Calculate the point B part for the opposit side of the first vector.
            //fr2=1-((((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)*2)*(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval))/(2*(fr1*fr1)));
            //fr3=sqrt((1-(fr2*fr2)));
            //ptt[0][0]=((ptDimL[(!ArrowFit)?1:0][0]-vertex[0])*fr2-((ptDimL[(!ArrowFit)?1:0][1]-vertex[1])*fr3)*((interseccnt)?-1:1))+vertex[0];
            //ptt[0][1]=((ptDimL[(!ArrowFit)?1:0][1]-vertex[1])*fr2+((ptDimL[(!ArrowFit)?1:0][0]-vertex[0])*fr3)*((interseccnt)?-1:1))+vertex[1];
            //
            //ic_normang(&angle13,&angle14);
            //anglex[0]=angle13;
            //anglex[1]=angle14;
            //
            //anglex[2]=sds_angle(vertex,ptt[0]);
            //if (plink->flags&DIM_ANGULAR) {
            //    // Make sure that all the vectors angles are greater then anglex[2].
            //    ic_normang(&anglex[2],&arlistcomp[0]);
            //    ic_normang(&anglex[2],&arlistcomp[1]);
            //    ic_normang(&anglex[2],&arlistcomp[2]);
            //    ic_normang(&anglex[2],&arlistcomp[3]);
            //    // This example are two lines in the shape of a (V) the dashed lines are imaginary.
            //    //
            //    //   \        /             arlistcomp are the four possble angles made by all four vectors.
            //    //    \      /              anglex[2] Can be inbetween any of the four vectors,So I need to
            //    //     \    /               find out what vector is the closest going clockwise.
            //    //      \  /
            //    //       \/
            //    //        \
            //    //      /
            //    //          \
            //    //    /
            //    //            \
            //    // This finds the closest angle to anglex[2]. This works becouse all arlistcomp angles were made larger than anglex[2].
            //    for (fi2=0,fi1=1;fi1<4;++fi1) { if (arlistcomp[fi1]<arlistcomp[fi2]) fi2=fi1; }
            //    anglex[3]=arlistcomp[fi2];
            //} else {
            //    anglex[3]=sds_angle(vertex,((ArrowFit)?pt13:pt14));
            //}
            //sds_polar(vertex,anglex[2],fr1,ptDimL[2]);
            //
            //// EBATECH(watanabe)-[
            ////sds_polar(ptDimL[2],IC_PI,(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval),ptDimL[3]);
            ////if (!plink->GenInitflg) sds_polar(ptDimL[2],IC_PI,txtbox[1][0],plink->textmidpt);
            //sds_real dDimasz = plinkvar[DIMASZ].dval.rval * plinkvar[DIMSCALE].dval.rval;
            //sds_point pt;
            //if (plinkvar[DIMTOH].dval.ival)
            //  {
            //    dAngToh = 0;
            //    if (angle13 > IC_PI)
            //      {
            //        dAngToh = IC_PI;
            //      }
            //    sds_polar(ptDimL[2], dAngToh, dDimasz, ptDimL[3]);
            //    sds_point_set(ptDimL[3], pt);
            //    addlineflag = TRUE;
            //  }
            //else
            //  {
            //    dAngToh = anglex[3] * 2;
            //    dAngToh -= fr4;
            //    sds_point_set(ptDimL[2], pt);
            //  }
            //
            //if (!plink->GenInitflg)
            //  {
            //    sds_polar(pt, dAngToh, txtbox[1][0], plink->textmidpt);
            //  }
            //// ]-EBATECH(watanabe)
            //
            //interseccnt=3;
            //
            //// EBATECH(watanabe)-[
            ////addlineflag=TRUE;
            //bDrawOutside = true;
            //// ]-EBATECH(watanabe)
			// EBATECH(CNBR) -[ DIMTMOVE=1,2 never move outside of angles.
			if( !plinkvar[DIMTMOVE].dval.ival )
			{
            	CalculateDimAngOutside(
                    plink, fr1, fr4, anglex, arlistcomp, angle13, angle14,
                    ptDimL, txtbox, vertex, pt13, pt14, interseccnt,
                    addlineflag, bDrawOutside, dAngToh, ArrowFit);
            }
            // EBATECH(CNBR) ]-
            // ]-EBATECH(watanabe)

            break;
        } else interseccnt=1;
    } while(TRUE);

    return DrawDimAng(
                plink, flp, dragmode, ptExtL, ptDimL, ptt, vertex,
                pt14, pt13, txtbox, ipt, anglex, fr1, fr4, fr5, interseccnt,
                text, ArrowFit, OffFlgL13, OffFlgL14, addlineflag,
                ArrowSideflg, bDrawOutside, dAngTih, dAngToh);
}

//	Check internal arrowhead blocks for existance
void CreateDimBlock(
  db_drawing*   flp
)
{
	if ((plinkvar[DIMSAH].dval.ival) && *plinkvar[DIMBLK1].dval.cval)
		strcpy(cmd_string1,plinkvar[DIMBLK1].dval.cval);
	else if (plinkvar[DIMBLK].dval.cval && *plinkvar[DIMBLK].dval.cval)
		strcpy(cmd_string1,plinkvar[DIMBLK].dval.cval);
	else
		strcpy(cmd_string1,DIM_CLOSED_FILLED);
	if( *cmd_string1 )
		checkBlock( cmd_string1, flp );

	if ((plinkvar[DIMSAH].dval.ival) && *plinkvar[DIMBLK2].dval.cval)
		strcpy(cmd_string2,plinkvar[DIMBLK2].dval.cval);
	else if (plinkvar[DIMBLK].dval.cval && *plinkvar[DIMBLK].dval.cval)
		strcpy(cmd_string2,plinkvar[DIMBLK].dval.cval);
	else
		strcpy(cmd_string2,DIM_CLOSED_FILLED);
	if( *cmd_string2 )
		checkBlock( cmd_string2, flp );
}

// Draw DIMENSION
static BOOL
DrawDimAng(
  struct cmd_dimeparlink*   plink,
  db_drawing*   flp,
  short         dragmode,
  sds_point     ptExtL[],
  sds_point     ptDimL[],
  sds_point     ptt[],
  sds_point     vertex,
  sds_point     pt14,
  sds_point     pt13,
  sds_point     txtbox[],
  sds_point     ipt[],
  sds_real      anglex[],
  sds_real      fr1,
  sds_real      fr4,
  sds_real      fr5,
  short         interseccnt,
  char          text[],
  BOOL          ArrowFit,
  BOOL          OffFlgL13,
  BOOL          OffFlgL14,
  BOOL          addlineflag,
  BOOL          ArrowSideflg,
  bool          bDrawOutside,
  sds_real      dAngTih,
  sds_real      dAngToh
)
{
    sds_point      txtins = {0.0, 0.0, 0.0},
                   boxrect[4] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0},
                                 {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};
    sds_real       cosofdeg = 0.0,
                   sinofdeg = 0.0;
    short          fi1 = 0;
    int            ret = 0;
    db_handitem   *handitem = NULL;
	//Bugzilla No. 78203 ; 17-10-2002
	int type = 0;
	int celweight = cmd_getCelweight();
    CreateDimBlock(flp);
    if (!dragmode && plinkvar[DIMASO].dval.ival) { if(RTNORM!=dwg_addblock(layername,256,"*D"/*DNT*/,1,ipt[0],NULL,flp)) goto exit; }

    // EBATECH(watanabe)-[
    //if (RTNORM!=cmd_dimaddmtext(layername,plinkvar[DIMCLRT].dval.ival,plink->textmidpt,plinkvar[DIMTXT].dval.rval*plinkvar[DIMSCALE].dval.rval,text,plink->textrot,0.0,plinkvar[DIMTXSTY].dval.cval,5,plink->pt210,flp,dragmode)) goto exit;
    double dAngle;
    if (bDrawOutside)
      {
        // DIMTOH
        dAngle = dAngToh;
      }
    else
      {
        // DIMTIH
        dAngle = dAngTih;
      }

    if (dAngle > IC_PI * 3 / 2)
      {
        dAngle -= IC_PI * 2;
      }

    if (dAngle > IC_PI / 2)
      {
        dAngle -= IC_PI;
      }

    ret = cmd_dimaddmtext(
            layername,
            plinkvar[DIMCLRT].dval.ival,
            plink->lweight,
            plink->textmidpt,
            plinkvar[DIMTXT].dval.rval * plinkvar[DIMSCALE].dval.rval,
            text,
            dAngle,
            0.0,
            plinkvar[DIMTXSTY].dval.cval,
            5,
            plink->pt210,
            flp,
            dragmode);
    if (RTNORM != ret)
      {
        goto exit;
      }
    // ]-EBATECH(watanabe)
	//Bugzilla No. 78203 ; 17-10-2002 [
	type=plink->flags;
    type &= ~DIM_TEXT_MOVED;
    type &= ~DIM_BIT_6;
	if ( type  == DIM_ANGULAR_3P )
	{
    if (RTNORM!=cmd_dimaddpoint(DIM_DEFPOINTS, plinkvar[DIMCLRE].dval.ival,celweight,pt13,flp,dragmode)) goto exit;
    if (RTNORM!=cmd_dimaddpoint(DIM_DEFPOINTS, plinkvar[DIMCLRE].dval.ival,celweight,pt14,flp,dragmode)) goto exit;
    if (RTNORM!=cmd_dimaddpoint(DIM_DEFPOINTS, plinkvar[DIMCLRE].dval.ival,celweight,vertex,flp,dragmode)) goto exit;
	}
	else
	{
		if (RTNORM!=cmd_dimaddpoint(DIM_DEFPOINTS, plinkvar[DIMCLRE].dval.ival,celweight,plink->defpt,flp,dragmode)) goto exit;
		if (RTNORM!=cmd_dimaddpoint(DIM_DEFPOINTS, plinkvar[DIMCLRE].dval.ival,celweight,plink->pt13,flp,dragmode)) goto exit;
		if (RTNORM!=cmd_dimaddpoint(DIM_DEFPOINTS, plinkvar[DIMCLRE].dval.ival,celweight,plink->pt14,flp,dragmode)) goto exit;
		if (RTNORM!=cmd_dimaddpoint(DIM_DEFPOINTS, plinkvar[DIMCLRE].dval.ival,celweight,plink->pt15,flp,dragmode)) goto exit;
	}
	//Bugzilla No. 78203 ; 17-10-2002 ]
    if (!plinkvar[((ArrowFit)?DIMSD1:DIMSD2)].dval.ival) {
        fr4-=IC_PI;
		sinofdeg=sin(fr4); cosofdeg=cos(fr4);

/*      if ((plinkvar[DIMSAH].dval.ival) && *plinkvar[DIMBLK1].dval.cval)
			strcpy(cmd_string1,plinkvar[DIMBLK1].dval.cval);
        else if (plinkvar[DIMBLK].dval.cval && *plinkvar[DIMBLK].dval.cval)
			strcpy(cmd_string1,plinkvar[DIMBLK].dval.cval);
		else
			strcpy(cmd_string1,DIM_CLOSED_FILLED);
*/
        cmd_MakeItemHead(ipt,cmd_string1,flp,dragmode,sinofdeg,cosofdeg,fr4, !ArrowSideflg,FALSE,ptDimL[0],0);
    }
    if (!plinkvar[((ArrowFit)?DIMSD2:DIMSD1)].dval.ival) {
		fr5-=IC_PI;
        sinofdeg=sin(fr5); cosofdeg=cos(fr5);

/*      if ((plinkvar[DIMSAH].dval.ival) && *plinkvar[DIMBLK2].dval.cval)
			strcpy(cmd_string2,plinkvar[DIMBLK2].dval.cval);
        else if (plinkvar[DIMBLK].dval.cval && *plinkvar[DIMBLK].dval.cval)
			strcpy(cmd_string2,plinkvar[DIMBLK].dval.cval);
		else
			strcpy(cmd_string2,DIM_CLOSED_FILLED);
*/
        cmd_MakeItemHead(ipt,cmd_string2,flp,dragmode,sinofdeg,cosofdeg,fr5, !ArrowSideflg,FALSE,ptDimL[1],0);
    }
    if (!plinkvar[((!(plink->flags&DIM_ANGULAR))?((ArrowFit)?DIMSE1:DIMSE2):DIMSE1)].dval.ival && OffFlgL13) {
        if (RTNORM!=cmd_dimaddline(layername,plinkvar[DIMCLRE].dval.ival,plinkvar[DIMLWE].dval.ival,ptExtL[0],ptExtL[1],flp,dragmode)) goto exit;
    }
    if (!plinkvar[((!(plink->flags&DIM_ANGULAR))?((ArrowFit)?DIMSE2:DIMSE1):DIMSE2)].dval.ival && OffFlgL14) {
        if (RTNORM!=cmd_dimaddline(layername,plinkvar[DIMCLRE].dval.ival,plinkvar[DIMLWE].dval.ival,ptExtL[2],ptExtL[3],flp,dragmode)) goto exit;
    }
    for (fi1=0;fi1<interseccnt;fi1+=2) {
        if (RTNORM!=cmd_dimaddarc(layername,plinkvar[DIMCLRD].dval.ival,plinkvar[DIMLWD].dval.ival,vertex,fr1,anglex[fi1],anglex[fi1+1],plink->pt210,flp,dragmode)) goto exit;
    }
    if (addlineflag) {
        if (RTNORM!=cmd_dimaddline(layername,plinkvar[DIMCLRD].dval.ival,plinkvar[DIMLWD].dval.ival,ptDimL[2],ptDimL[3],flp,dragmode)) goto exit;
    }

    if ((plinkvar[DIMGAP].dval.rval*plinkvar[DIMSCALE].dval.rval)<0.0) {
        // Here need to build box around the text.
        for (fi1=0;fi1<4;++fi1) ic_ptcpy(ptt[fi1],plink->textmidpt);

        // EBATECH(watanabe)-[
        //ptt[0][0]-=(txtbox[1][0]); ptt[2][0]-=(txtbox[1][0]);
        //ptt[1][0]+=(txtbox[1][0]); ptt[3][0]+=(txtbox[1][0]);
        //ptt[0][1]+=(txtbox[1][1]); ptt[1][1]+=(txtbox[1][1]);
        //ptt[2][1]-=(txtbox[1][1]); ptt[3][1]-=(txtbox[1][1]);
        sds_real x = txtbox[1][0];
        sds_real y = txtbox[1][1];

        ptt[0][0] += (-x) * cos(dAngle) - ( y) * sin(dAngle);
        ptt[0][1] += (-x) * sin(dAngle) + ( y) * cos(dAngle);

        ptt[1][0] += ( x) * cos(dAngle) - ( y) * sin(dAngle);
        ptt[1][1] += ( x) * sin(dAngle) + ( y) * cos(dAngle);

        ptt[2][0] += (-x) * cos(dAngle) - (-y) * sin(dAngle);
        ptt[2][1] += (-x) * sin(dAngle) + (-y) * cos(dAngle);

        ptt[3][0] += ( x) * cos(dAngle) - (-y) * sin(dAngle);
        ptt[3][1] += ( x) * sin(dAngle) + (-y) * cos(dAngle);
        // ]-EBATECH(watanabe)
        sinofdeg=sin(0.0); cosofdeg=cos(0.0);
        for (fi1=0;fi1<4;++fi1) cmd_ucstodim(txtins,ptt[fi1],boxrect[fi1],sinofdeg,cosofdeg);

        // EBATECH(watanabe)-[enable UCS
        //if (!cmd_drawbox(plinkvar[DIMCLRD].dval.ival,plinkvar[DIMLWD].dval.ival, ptt,flp,dragmode)) goto exit;
        ret = cmd_drawbox(plinkvar[DIMCLRD].dval.ival, plinkvar[DIMLWD].dval.ival, boxrect, flp, dragmode);
        if (!ret)
          {
            goto exit;
          }
        // ]-EBATECH(watanabe)
    }
    if (!dragmode && plinkvar[DIMASO].dval.ival) {
		if (RTERROR==dwg_addendblk(flp)) goto exit;
	    handitem=flp->ret_lastblockdef();
		handitem->get_2(plink->bname,sizeof(plink->bname)-1);
	}

    return(TRUE);

    exit:
        return(FALSE);
}
