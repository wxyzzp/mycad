// Radial dimension
// DIMFIT = 5

#include "cmds.h"/*DNT*/
#include "dimensions.h"/*DNT*/
#include "cmdsDimVars.h"/*DNT*/
#include "DimDiaRad.h"/*DNT*/

extern        SDS_AtNodeDrag;
extern struct cmd_dimvars plinkvar[];
extern struct cmd_dimeparlink* plink;

//////////////////////////////////////////////////////////////////////
static
double
GetYSide(
  sds_point     base1,
  sds_point     base2,
  sds_point     dest,
  const BOOL    bFlag
)
{
  double angle = sds_angle(base1, base2);
  if ((fabs(angle - IC_PI * 0.5) < CMD_FUZZ2) ||
      (fabs(angle - IC_PI * 1.5) < CMD_FUZZ2))
    {
      double a = base1[1];
      double b = base2[1];
      if (a > b)
        {
          // swap
          double temp = a;
          a = b;
          b = temp;
        }

      if (bFlag)
        {
          return a;
        }
      return b;
    }
  double dTan = tan(angle);
  double y = dTan * (dest[0] - base1[0]) + base1[1];
  return y;
}

//////////////////////////////////////////////////////////////////////
static
double
GetXSide(
  sds_point     base1,
  sds_point     base2,
  sds_point     dest,
  const BOOL    bFlag
)
{
  double angle = sds_angle(base1, base2);
  if ((fabs(angle)         < CMD_FUZZ2) ||
      (fabs(angle - IC_PI) < CMD_FUZZ2))
    {
      double a = base1[0];
      double b = base2[0];
      if (a > b)
        {
          // swap
          double temp = a;
          a = b;
          b = temp;
        }

      if (bFlag)
        {
          return a;
        }
      return b;
    }
  double dTan = 1 / tan(angle);
  double x = dTan * (dest[1] - base1[1]) + base1[0];
  return x;
}

//////////////////////////////////////////////////////////////////////
// recognize whether the specified position is in box, or not.
//  ____________
// | pt         |  pt
// |  x TRUE    |   x FALSE
// |____________|
static
BOOL
ExistInBox(
  sds_point     txtbox[],
  sds_point     pt
)
{
  int iDimtih = plinkvar[DIMTIH].dval.ival;
  double angle = sds_angle(plink->vertex, pt);
  double ang = GetTextRotation(iDimtih, angle);
  int i;

  sds_point ptp[4];
  ic_ptcpy(ptp[0], txtbox[0]);
  ic_ptcpy(ptp[1], txtbox[1]);
  ic_ptcpy(ptp[2], txtbox[0]);
  ic_ptcpy(ptp[3], txtbox[1]);
  ptp[0][1] = txtbox[1][1];
  ptp[3][1] = txtbox[0][1];

  // move to center
  for (i = 0; i < 4; ++i)
    {
      ptp[i][0] -= txtbox[1][0] / 2;
      ptp[i][1] -= txtbox[1][1] / 2;
      ptp[i][2] = 0;
    }

  sds_point nbox[4];
  double dCos = cos(ang);
  double dSin = sin(ang);
  for (i = 0; i < 4; ++i)
    {
      // rotate the box (angle) degrees.
      nbox[i][0] = ptp[i][0] * dCos - ptp[i][1] * dSin;
      nbox[i][1] = ptp[i][1] * dCos + ptp[i][0] * dSin;
      nbox[i][2] = 0;

      // Move the box to the correct location.
      ic_add(nbox[i], plink->textmidpt, nbox[i]);
    }

  double y01 = GetYSide(nbox[0], nbox[1], pt, FALSE);
  double y23 = GetYSide(nbox[2], nbox[3], pt, TRUE);
  if ((y23 <= pt[1]) &&
      (pt[1] <= y01))
    {
      double x02 = GetXSide(nbox[0], nbox[2], pt, TRUE);
      double x13 = GetXSide(nbox[1], nbox[3], pt, FALSE);
      if ((x02 <= pt[0]) &&
          (pt[0] <= x13))
        {
          return TRUE;
        }
    }

  return FALSE;
}

//////////////////////////////////////////////////////////////////////
// recognize whether the specified position is in box, or not.
//      ____________
//     |     ps     |   pe
//     |     x------|----x TRUE
//     |            |
// ps  |            |   pe
// x---|------------|----x TRUE
//     |____________|
// ps                  pe
// x---------------------x FALSE
//
static
BOOL
ExistInBox(
  sds_point     box2[],
  sds_point     ps,
  sds_point     pe
)
{
  int iDimtih = plinkvar[DIMTIH].dval.ival;
  BOOL b;

  b = ExistInBox(box2, ps);
  if (b)
    {
      return TRUE;
    }

  b = ExistInBox(box2, pe);
  if (b)
    {
      return TRUE;
    }

  sds_point ptt[4];
  double angle = sds_angle(ps, pe);
  double ang = GetTextRotation(iDimtih, angle);
  b = cmd_getboxintersecEx(
            box2, ang, plink->textmidpt,
            ps, pe, ptt);
  if (b)
    {
      return TRUE;
    }

  return FALSE;
}

//////////////////////////////////////////////////////////////////////
static
BOOL
ExistInBoxPt15(
  sds_point     box2[],
  const BOOL    bIsDia
)
{
  if (bIsDia)
    {
      // diametric

      //   (DIMASZ)
      // in  out   in
      //    |---|
      //    |  /
      //    | /
      //     /
      //    x-----
      //     \ 
      //      \ 
      //       \ 
      double dDimscale = plinkvar[DIMSCALE].dval.rval;
      double dDimasz   = plinkvar[DIMASZ].dval.rval * dDimscale;

      double ang = sds_angle(plink->vertex, plink->pt15);
      double d = sds_distance(plink->vertex, plink->pt15);
      d -= dDimasz;

      sds_point pt;
      sds_polar(plink->vertex, ang, d, pt);

      return ExistInBox(box2, plink->pt15, pt);
    }

  // radial

  // in  out   in
  //      |
  //      |  /
  //      | /
  //      |/
  //      x-----
  //       \ 
  //        \ 
  //         \ 
  return ExistInBox(box2, plink->pt15);
}

//////////////////////////////////////////////////////////////////////
//   (DIMASZ)
// in  out   in
//    |---|
//    |  /
//    | /
//     /
//    x-----
//     \ 
//      \ 
//       \ 
static
BOOL
ExistInBoxDefPt(
  sds_point     box2[],
  const BOOL    bIsDia
)
{
  double dDimscale = plinkvar[DIMSCALE].dval.rval;
  double dDimasz   = plinkvar[DIMASZ].dval.rval * dDimscale;
  double d, ang;
  sds_point pt;

  if (bIsDia)
    {
      ang = sds_angle(plink->vertex, plink->defpt);
      d  = sds_distance(plink->vertex, plink->defpt);
      d -= dDimasz;
    }
  else
    {
      ang = sds_angle(plink->vertex, plink->pt15);
      d = dDimasz;
    }

  sds_polar(plink->vertex, ang, d, pt);
  return ExistInBox(box2, plink->defpt, pt);
}

//////////////////////////////////////////////////////////////////////
BOOL
cmd_DimRad5(
  db_drawing*   flp,
  sds_point txtbox[],
  sds_point box2[],
  CDimLine& dl,
  sds_real& angle,
  short     dragmode,
  const BOOL bIsDia
)
{
  int iDimtih = plinkvar[DIMTIH].dval.ival;
  double dDimscale = plinkvar[DIMSCALE].dval.rval;
  double dDimasz   = plinkvar[DIMASZ].dval.rval * dDimscale;
  double d, ang;

  switch (SDS_AtNodeDrag)
    {
      // Create a new dimension
      case 0:
      // DDIM command
      case 1:
      // drag the text position
      case 2:
        {
          int iIsTextH = iDimtih;
          dl.arrow = TRUE;
          dl.showOut = TRUE;

          // get angle
          angle = sds_angle(plink->vertex, plink->pt15);
          ang = angle + IC_PI;

          d = txtbox[1][0] + dDimasz;
          if (d > plink->distance)
            {
              // radius is small
              dl.arrow = FALSE;
            }
          else
            {
              // check whether text position is in or out.
              BOOL b = ExistInBoxPt15(box2, bIsDia);
              if (b)
                {
                  dl.arrow = FALSE;
                }
              else
                {
                  BOOL b = ExistInBoxDefPt(box2, bIsDia);
                  if (b)
                    {
                      dl.arrow = FALSE;
                    }
                }
            }

          if (dl.arrow)
            {
              // center
              if (bIsDia)
                {
                  d = plink->distance - dDimasz;
                  sds_polar(plink->vertex, angle - IC_PI, d, dl.outS);
                }
              else
                {
                  ic_ptcpy(dl.outS, plink->vertex);
                }

              // Calculate the point DIMASZ * DIMSCALE closer to the center from the radius of the circle in dl.extS.
              sds_point pp;
              sds_polar(plink->vertex, angle, plink->distance - dDimasz, pp);

              // The following code gets the intersecion points ptt[0] and ptt[1] of
              // the dimension line crossing the dimension textbox which
              // sourounds the dimension text.
              sds_point ptt[4];
              ang = GetTextRotation(iIsTextH, angle);
              BOOL b = cmd_getboxintersecEx(
                        box2, ang, plink->textmidpt,
                        dl.outS, pp, ptt);
              if (b)
                {
                  // there was enough space for the text.

                  // radial
                  // pt15 /     defpt(center)
                  //     x----------x
                  //      \ 

                  // diametric
                  // pt15 /       center      \ defpt
                  //     x----------x----------x
                  //      \                   /

                  // Switch the points if the intersections are oppisite of each other.
                  double d0 = sds_distance(dl.outS, ptt[0]);
                  double d1 = sds_distance(dl.outS, ptt[1]);
                  if (d0 > d1)
                    {
                      // swap
                      ic_swap(ptt[0], ptt[1]);
                    }
                  ic_ptcpy(dl.outE, ptt[0]);

                  dl.showExt = TRUE;
                  ic_ptcpy(dl.extS, pp);
                  ic_ptcpy(dl.extE, ptt[1]);
                }
              else
                {
                  // there was not enough space.
                  ic_ptcpy(dl.outE, pp);
                }

              if (bIsDia)
                {
                  dl.showArw1 = TRUE;
                  ic_ptcpy(dl.arw1, plink->defpt);
                }
              else
                {
                  dl.showArw1 = FALSE;
                }
            }
          else
            {
              // radial
              // pt15 \      defpt(center)
              // ------x----------x
              //      /

              // diametric
              // pt15 \         center        / defpt
              // ------x----------x----------x------
              //      /                       \ 

              ang = angle;
              sds_polar(plink->pt15, ang, dDimasz,     dl.outS);
              sds_polar(plink->pt15, ang, dDimasz * 2, dl.outE);

              // draw center mark or line according to DIMTOFL
              DrawCenterMark(dl, flp, bIsDia, dragmode);
            }

          // show arrow
          dl.showArw2 = TRUE;
          ic_ptcpy(dl.arw2, plink->pt15);

          if (plinkvar[DIMSD1].dval.ival && plinkvar[DIMSD2].dval.ival)
            {
              // hide dimension line and arrow
              dl.showSd1  = FALSE;
              dl.showSd2  = FALSE;
              dl.showOut  = FALSE;
              dl.showLdr  = FALSE;
              dl.showExt  = FALSE;
              dl.showArw1 = FALSE;
              dl.showArw2 = FALSE;
              // show only center line when DIMTOFL is 1.
              break;
            }

          // Set the rotation of the text.
          dl.rot = GetTextRotation(iIsTextH, ang);

          break;
        }
    }

  return TRUE;
}
