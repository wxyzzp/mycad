// Radial dimension
// DIMFIT = 4

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
GetSide(
  const double  angle,
  sds_point     base,
  sds_point     dest
)
{
  double dTan = tan(angle + IC_PI / 2);
  double y = dTan * (dest[0] - base[0]) + base[1];
  return y;
}

//////////////////////////////////////////////////////////////////////
static
BOOL
cmd_DimRadExLeaderHorizontal(
  db_drawing*   flp,
  sds_point txtbox[],
  sds_point box2[],
  CDimLine& dl,
  sds_real& angle,
  const BOOL bIsDia,
  short     dragmode
)
{
  int iDimtih = plinkvar[DIMTIH].dval.ival;
  int iDimtoh = plinkvar[DIMTOH].dval.ival;
  int iDimtad = plinkvar[DIMTAD].dval.ival;
  double dDimscale = plinkvar[DIMSCALE].dval.rval;
  double dDimasz   = plinkvar[DIMASZ].dval.rval * dDimscale;
  double d, ang;

  BOOL bInside = FALSE;

  //      |        |
  //  OUT |   IN   | OUT
  //      |        |
  // defpt.------->. pt15
  //      |        |
  //      |        |
  //
  if (IsLeftSide(angle))
    {
      // left side
      if (plink->pt15[0] < plink->textmidpt[0])
        {
          dl.arrow = TRUE;
          if (plink->textmidpt[0] < plink->defpt[0])
            {
              bInside  = TRUE;
            }
        }
    }
  else
    {
      // right side
      if (plink->textmidpt[0] < plink->pt15[0])
        {
          dl.arrow = TRUE;
          if (plink->defpt[0] < plink->textmidpt[0])
            {
              bInside  = TRUE;
            }
        }
    }

  // get the edge point of dimension line
  sds_point pp = {0, 0, 0};
  pp[0] = plink->textmidpt[0];
  pp[1] = plink->vertex[1];

  ang = angle;
  if (dl.arrow)
    {
      ang += IC_PI;

      dl.showArw1 = FALSE;
    }
  else
    {
      // draw center mark or line according to DIMTOFL
      DrawCenterMark(dl, flp, bIsDia, dragmode);
    }

  double dist = sds_distance(plink->pt15, pp);
  double dist1 = dDimasz * 2;
  dist1 += txtbox[1][0] / 2;
  if (dist < dist1)
    {
      // dimension line length must have DIMASZ * 2 at least
      sds_polar(plink->pt15, ang, dist1, pp);
    }

  sds_polar(plink->pt15, ang, dDimasz, dl.outS);
  ic_ptcpy(dl.outE, pp);

  // direction of leader
  BOOL bIsLeft = FALSE;
  if (plink->textmidpt[0] < plink->pt15[0])
    {
      bIsLeft = TRUE;
    }

  // draw leader (horizontal)
  d = txtbox[1][0] / 2;
  if (bIsLeft)
    {
      d = -d;
    }

  // move text position
  ic_ptcpy(plink->textmidpt, pp);
  if (iDimtad == 0)
    {
      dl.outE[0] -= d;
    }
  else
    {
      dl.outE[0] += d;
      plink->textmidpt[1] += GetUpperDimgap();
    }

  dl.showSd1 = FALSE;
  dl.showSd2 = FALSE;
  dl.showLdr = FALSE;
  if (plinkvar[DIMSD1].dval.ival && plinkvar[DIMSD2].dval.ival)
    {
      // hide dimension line
      dl.showOut  = FALSE;

      // hide arrow
      dl.showArw2 = FALSE;
    }
  else
    {
      // show dimension line
      dl.showOut = TRUE;

      // show arrow 2 only
      dl.showArw2 = TRUE;
      ic_ptcpy(dl.arw2, plink->pt15);
    }

  int iIsTextH = iDimtoh;
  if (bInside)
    {
      iIsTextH = iDimtih;
    }

  // Set the rotation of the text.
  dl.rot = GetTextRotation(iIsTextH, ang);

  return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL
cmd_DimRadExLeader(
  db_drawing*   flp,
  sds_point txtbox[],
  sds_point box2[],
  CDimLine& dl,
  sds_real& angle,
  const BOOL bIsDia,
  short     dragmode
)
{
  dl.arrow = FALSE;

  // Bugzilla No: 78204 ; 29/07/2002 [
  sds_real temp = plink->pt15[2];
  plink->pt15[2] = 0.0;
  // Bugzilla No: 78204 ; 29/07/2002 ]
  // dimension line is horizontal
  if (IsHorizontal(angle))
    {
      return cmd_DimRadExLeaderHorizontal(
                    flp, txtbox, box2, dl, angle, bIsDia, dragmode);
    }

  int iDimtih = plinkvar[DIMTIH].dval.ival;
  int iDimtoh = plinkvar[DIMTOH].dval.ival;
  int iDimtad = plinkvar[DIMTAD].dval.ival;
  double dDimscale = plinkvar[DIMSCALE].dval.rval;
  double dDimasz   = plinkvar[DIMASZ].dval.rval * dDimscale;
  double d, ang;

  int iIsTextH = iDimtoh;
  BOOL bInside = FALSE;

  //   \        \ 
  //    \        \       ____
  // OUT \    IN  \ OUT /
  //      \        \   /
  //       \        \.  <--pt15
  //        \       /\ 
  //         \     /  \ 
  //          \   /    \ 
  //    defpt-->./      \ 
  //            \        \ 
  //
  double y1 = GetSide(angle, plink->pt15,  plink->textmidpt);
  double y2 = GetSide(angle, plink->defpt, plink->textmidpt);
  d = plink->textmidpt[1];
  if (plink->pt15[1] < plink->vertex[1])
    {
      d = -d;
      y1 = -y1;
      y2 = -y2;
    }

  if (d < y1)
    {
      // inside
      dl.arrow = TRUE;
      if (d > y2)
        {
          iIsTextH = iDimtih;
          bInside  = TRUE;
        }
    }

  // draw dimension line
  sds_point to2;
  double angA = angle + IC_PI / 2;
  if (iIsTextH)
    {
      angA = 0;
    }

  sds_polar(plink->textmidpt, angA, 1, to2);

  sds_point pp;
  // Bugzilla No: 78204 ; 29/07/2002 [
  pp[0]=pp[1]=pp[2]=0.0;	
  plink->vertex[2] = 0.0;
  sds_inters(plink->pt15, plink->vertex, plink->textmidpt, to2, 0, pp);
  plink->vertex[2] = plink->pt13[2];
  // Bugzilla No: 78204 ; 29/07/2002 ]

  if (iIsTextH)
    {
      if (dl.arrow)
        {
          double y = GetSide(angle, plink->pt15, pp);
          d = plink->textmidpt[1];
          if (plink->pt15[1] < plink->vertex[1])
            {
              d = -d;
              y = -y;
            }

          if (d > y)
            {
              dl.arrow = FALSE;
            }
        }
      else
        {
          double y = GetSide(angle, plink->pt15, pp);
          d = plink->textmidpt[1];
          if (plink->pt15[1] < plink->vertex[1])
            {
              d = -d;
              y = -y;
            }

          if (d < y)
            {
              dl.arrow = TRUE;
            }
        }
    }

  if (dl.arrow)
    {
      ang = angle + IC_PI;

      dl.showArw1 = FALSE;
    }
  else
    {
      ang = angle;

      // draw center mark or line according to DIMTOFL
      DrawCenterMark(dl, flp, bIsDia, dragmode);
    }

  double dist = sds_distance(plink->pt15, pp);
  d = dDimasz * 2;
  if (iIsTextH == 0)
    {
      d += txtbox[1][0] / 2;
    }

  if (dist < d)
    {
      // dimension line length must have DIMASZ * 2 at least
      sds_polar(plink->pt15, ang, d, pp);
    }

  sds_polar(plink->pt15, ang, dDimasz, dl.outS);
  ic_ptcpy(dl.outE, pp);

  // draw leader (horizontal)
  ic_ptcpy(dl.ldrS, dl.outE);
  ic_ptcpy(dl.ldrE, dl.outE);

  BOOL bIsLeft = FALSE;

  if (iIsTextH)
    {
      double ang2 = sds_angle(plink->vertex, plink->textmidpt);
      double ang3 = angle - ang2;
      if ((fabs(ang3)         < CMD_FUZZ) ||
          (fabs(ang3 + IC_PI) < CMD_FUZZ))
        {
          ic_ptcpy(plink->textmidpt, pp);
		  // Bugzilla No: 78204 ; 29/07/2002   
		  plink->pt14[1] = plink->textmidpt[1]; 
          ang2 = sds_angle(plink->pt15, pp);
          if (IsLeftSide(ang2))
            {
              if (!bInside)
                {
                  bIsLeft = TRUE;
                }
            }
          else
            {
              if (bInside)
                {
                  bIsLeft = TRUE;
                }
            }
        }
      else if (plink->textmidpt[0] < pp[0])
        {
          bIsLeft = TRUE;
        }

      dist = sds_distance(pp, plink->textmidpt);
    }
  else
    {
      dist = sds_distance(pp, plink->pt15);
    }

  if (iDimtad == 0)
    {
      if (iIsTextH)
        {
          //        ______
          //    ___|      |
          //   /   |______|
          //  /
          // /

          dl.showLdr = FALSE;

          double dist1 = 0;
          if (HasHook(ang))
            {
              dl.showLdr = TRUE;
              dist1 = dDimasz;
            }

          double dist2 = txtbox[1][0] / 2;
          if (bIsLeft)
            {
              dist1 = -dist1;
              dist2 = -dist2;
            }

          d = dist1 + dist2;
          if (dist < fabs(d))
            {
              dl.ldrE[0] += dist1;
              plink->textmidpt[0] = pp[0] + d;
            }
          else
            {
              dl.ldrE[0] = plink->textmidpt[0] - dist2;
            }
          plink->textmidpt[1] = pp[1];
        }
      else
        {
          //       /\ 
          //      /  \ 
          //     /    \ 
          //    /     /
          //   /     /
          //   \    /
          //    \  /
          //   / \/
          //  /
          // /

          dl.showLdr = FALSE;

          d = dDimasz * 2 + txtbox[1][0] / 2;
          if (dist < fabs(d))
            {
              sds_polar(plink->pt15, ang, d, plink->textmidpt);
              d = dDimasz * 2;
              sds_polar(plink->pt15, ang, d, dl.outE);
            }
          else
            {
              d = dist - txtbox[1][0] / 2;
              sds_polar(plink->pt15, ang, d, dl.outE);
              ic_ptcpy(plink->textmidpt, pp);
            }
        }
    }   // DIMTAD == 0
  else
    {
      // DIMTAD != 0

      if (iIsTextH)
        {
          //        ______
          //       |      |
          //       |______|
          //    ___________
          //   /
          //  /
          // /

          dl.showLdr = TRUE;

          d = txtbox[1][0] / 2;
          if (bIsLeft)
            {
              d = -d;
            }

          if (dist < fabs(d))
            {
              dl.ldrE[0] = pp[0] + d * 2;
              plink->textmidpt[0] = pp[0] + d;
            }
          else
            {
              dl.ldrE[0] = plink->textmidpt[0] + d;
            }
          plink->textmidpt[1]  = pp[1];
          plink->textmidpt[1] += GetUpperDimgap();
        }
      else
        {
          //     /\ 
          //    /  \ 
          //   /    \ 
          //  /     /
          // /     / /
          // \    / /
          //  \  / /
          //   \/ /
          //     /
          //    /
          //   /

          dl.showLdr = FALSE;

          d = dDimasz * 2 + txtbox[1][0] / 2;
          if (dist < fabs(d))
            {
              sds_polar(plink->pt15, ang, d, plink->textmidpt);
              d = dDimasz * 2 + txtbox[1][0];
              sds_polar(plink->pt15, ang, d, dl.outE);
            }
          else
            {
              d = dist + txtbox[1][0] / 2;
              sds_polar(plink->pt15, ang, d, dl.outE);
              ic_ptcpy(plink->textmidpt, pp);
            }

          double angB = angle + IC_PI / 2;
          if (IsLeftSide(angle))
            {
              angB = angle - IC_PI / 2;
            }
          sds_polar(plink->textmidpt, angB, GetUpperDimgap(), plink->textmidpt);
        }
    }   // DIMTAD != 0

  dl.showSd1 = FALSE;
  dl.showSd2 = FALSE;
  if (plinkvar[DIMSD1].dval.ival && plinkvar[DIMSD2].dval.ival)
    {
      // hide dimension line
      dl.showOut  = FALSE;
      dl.showLdr  = FALSE;

      // hide arrow
      dl.showArw2 = FALSE;
    }
  else
    {
      // show dimension line
      dl.showOut  = TRUE;

      dl.showArw2 = TRUE;
      ic_ptcpy(dl.arw2, plink->pt15);
    }

  // Set the rotation of the text.
  dl.rot = GetTextRotation(iIsTextH, ang);
  // Bugzilla No: 78204 ; 29/07/2002   
  plink->pt15[2] = temp; 
  return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL
cmd_DimRad4(
  db_drawing*   flp,
  sds_point txtbox[],
  sds_point box2[],
  CDimLine& dl,
  sds_real& angle,
  short     dragmode,
  const BOOL bIsDia
)
{
  switch (SDS_AtNodeDrag)
    {
      // Create a new dimension
      case 0:
        {
          break;
        }

      // DDIM command
      case 1:
      // drag the text position
      case 2:
        {
          // angle is fixed
          angle = sds_angle(plink->vertex, plink->pt15);
          break;
        }
    }

  cmd_DimRadExLeader(flp, txtbox, box2, dl, angle, bIsDia, dragmode);

  return TRUE;
}
