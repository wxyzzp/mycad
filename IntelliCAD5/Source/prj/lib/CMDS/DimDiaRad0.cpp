// Radial dimension
// DIMFIT = 0, 1, 2

#include "cmds.h"/*DNT*/
#include "dimensions.h"/*DNT*/
#include "cmdsDimVars.h"/*DNT*/
#include "DimDiaRad.h"/*DNT*/

extern        SDS_AtNodeDrag;
extern struct cmd_dimvars plinkvar[];
extern struct cmd_dimeparlink* plink;
//Modified Cybage SBD 13/11/2001 DD/MM/YYYY 
//Reason : Fix for Bug No. 77866 and 77867 from Bugzilla 
extern BOOL bdimUpdate;


//////////////////////////////////////////////////////////////////////
// Move text according to angle and system variables
static
BOOL
MoveDimTextPos(
  const double  angle,
  sds_point     txtbox[],
  sds_point     textmidpt,
  CDimLine&     dl
)
{
  int iDimtad = plinkvar[DIMTAD].dval.ival;
  int iDimtoh = plinkvar[DIMTOH].dval.ival;
  double dDimscale = plinkvar[DIMSCALE].dval.rval;
  double dDimasz   = plinkvar[DIMASZ].dval.rval * dDimscale;

  dl.showLdr = FALSE;

  // angle
  double ang = angle;
  double angB = IC_PI / 2;
  if (iDimtoh)
    {
      ang = 0;
      if (IsLeftSide(angle))
        {
          ang = IC_PI;
        }
    }
  else
    {
      angB += angle;
      if (IsLeftSide(angle))
        {
          angB = angle - IC_PI / 2;
        }
    }

  // extend leader
  double d0 = txtbox[1][0] / 2; // text position
  double d1;    // edge of leader
  if (iDimtad)
    {
      d1 = txtbox[1][0];

      if (iDimtoh)
        {
          //     ______
          //    |      |
          //    |______|
          //    ________
          //   /
          //  /
          // /
          dl.showLdr = TRUE;
        }
      else
        {
          //     /\ 
          //    /  \ 
          //   /    \ 
          //  \    / /
          //   \  / /
          //    \/ /
          //      /
          //     /
          //    /
          //   /
          dl.showLdr = TRUE;
        }
    }
  else
    {
      // if DIMTAD == 0

      d1 = 0;

      // if text is horizontal
      if (iDimtoh)
        {
          //       ______
          //    __|      |
          //   /  |______|
          //  /
          // /

          // if angle is over 15 degree
          if (HasHook(angle))
            {
              // leader has hook (leader line must be prolonged)
              d0 += dDimasz;
              d1 += dDimasz;

              dl.showLdr = TRUE;
            }
        }
      else
        {
          //      /\ 
          //     /  \ 
          //    /    \ 
          //   \    /
          //    /  /
          //   / \/
          //  /
          // /
        }
    }

  if (dl.showOut)
    {
      // extend leader
      sds_point* pt = &(dl.outE);
      if (dl.showLdr)
        {
          pt = &(dl.ldrE);
        }
      sds_polar(*pt, ang, d1, *pt);
    }
  else
    {
      // hide dimension line
      dl.showLdr = FALSE;
    }

  // text position
  sds_polar(textmidpt, ang, d0, textmidpt);

  if (iDimtad)
    {
      // move up text position
      double dGap = GetUpperDimgap();
      sds_polar(textmidpt, angB, dGap, textmidpt);
    }

  return TRUE;
}

//////////////////////////////////////////////////////////////////////
// Move text according to angle and system variables
static
BOOL
MoveTextPosOnDimLine(
  const double  angle,
  sds_point     txtbox[],
  sds_point     textmidpt
)
{
  int iDimtad = plinkvar[DIMTAD].dval.ival;
  int iDimtoh = plinkvar[DIMTOH].dval.ival;
  double dDimscale = plinkvar[DIMSCALE].dval.rval;
  double dDimasz   = plinkvar[DIMASZ].dval.rval * dDimscale;

  double d = GetUpperDimgap();
  if (iDimtoh)
    {
      // DIMTOH = 1
      double width = txtbox[1][0] / 2;
      if (iDimtad)
        {
          textmidpt[1] -= d;
        }
      else
        {
          if (HasHook(angle))
            {
              width += dDimasz;
            }
        }

      if (IsLeftSide(angle))
        {
          width = -width;
        }
      textmidpt[0] -= width;
    }
  else
    {
      // DIMTOH = 0

      if (iDimtad)
        {
          // left side
          double ang = IC_PI / 2;
          if (IsLeftSide(angle))
            {
              ang = -ang;
            }
          ang = angle - ang;
          sds_polar(textmidpt, ang, d, textmidpt);
        }
      else
        {
          // do nothing
        }

      double width = txtbox[1][0] / 2;
      width += dDimasz;
      sds_polar(textmidpt, angle - IC_PI, width, textmidpt);
    }

  return TRUE;
}

//////////////////////////////////////////////////////////////////////
// draw dimension for opposite side of arc.
//            ______
//           /      \ 
//          |     /  |
//          |    /   |
//              /    |
//             /    \ 
//  _____     /
// |     |   /
// |_____|  /
// ________/
BOOL
DrawOppositeDimRad(
  const double  angle,
  sds_point     txtbox[],
  CDimLine&     dl
)
{
  int iDimtoh = plinkvar[DIMTOH].dval.ival;
  double dDimscale = plinkvar[DIMSCALE].dval.rval;
  double dDimasz   = plinkvar[DIMASZ].dval.rval * dDimscale;
  double d, ang;

  dl.arrow = FALSE;
  dl.showArw1 = FALSE;
  dl.showSd1 = FALSE;
  dl.showSd2 = FALSE;
  if (plinkvar[DIMSD1].dval.ival && plinkvar[DIMSD2].dval.ival)
    {
      // hide dimension line and arrow
      dl.showOut  = FALSE;
      dl.showArw2 = FALSE;
    }
  else
    {
      ang = angle + IC_PI;

      // show dimension line
      dl.showOut = TRUE;
      d = plink->distance - dDimasz;
      sds_polar(plink->vertex, ang, d, dl.outS);
      ic_ptcpy(dl.outE, plink->textmidpt);

      ic_ptcpy(dl.ldrS, dl.outE);
      ic_ptcpy(dl.ldrE, dl.outE);

      // show arrow2 only
      dl.showArw2 = TRUE;
      d = plink->distance;
      sds_polar(plink->vertex, ang, d, dl.arw2);
    }

  // Set the rotation of the text.
  dl.rot = GetTextRotation(iDimtoh, angle);

  // Moving the text to the correct position.
  MoveDimTextPos(angle, txtbox, plink->textmidpt, dl);

  return TRUE;
}

//////////////////////////////////////////////////////////////////////
// draw dimension line in circle
// These are what the points look like now.
//
//  - Radial
//            __ pt15
//      _____/
//     /    /____ dl.outS
//    |    /  
//    |   .___|__ defpt,dl.sd1S
//    |       |
//     \_____/
//
//  - Diametric
//          __ pt15
//    _____/
//   /    /____ dl.sd2S
//  |    /  |
//  |   /   |
//  |  /    |
//   \.____/
//    defpt,dl.sd1S
static
BOOL
DrawInCircle(
  CDimLine& dl,
  sds_point ptt[],
  sds_real  angle,
  const BOOL bIsDia
)
{
  int iDimtih = plinkvar[DIMTIH].dval.ival;
  int iDimtad = plinkvar[DIMTAD].dval.ival;
  double ang;
  double d;

  sds_polar(plink->vertex, angle, plink->distance, plink->pt15);

  // move up text position
  BOOL bSingle = TRUE;
  if (iDimtad && !iDimtih)
    {
      d = GetUpperDimgap();

      // left side
      ang = IC_PI / 2;
      if (IsLeftSide(angle))
        {
          ang = -ang;
        }
      ang += angle;
      sds_polar(plink->textmidpt, ang, d, plink->textmidpt);
    }
  else
    {
      bSingle = FALSE;

      // Switch the points if the intersections are oppisite of each other.
      double d0 = sds_distance(dl.sd1S, ptt[0]);
      double d1 = sds_distance(dl.sd1S, ptt[1]);
      if (d0 > d1)
        {
          // swap
          ic_swap(ptt[0], ptt[1]);
        }
      ic_ptcpy(dl.sd1E, ptt[0]);

      if (bIsDia)
        {
          ic_ptcpy(dl.sd2E, ptt[1]);
        }
      else
        {
          ic_ptcpy(dl.outE, ptt[1]);
        }
    }

  // set flag for creating of dimension line and arrow
  dl.showLdr = FALSE;
  if (plinkvar[DIMSD1].dval.ival && plinkvar[DIMSD2].dval.ival)
    {
      // hide dimension line and arrow
      dl.showSd1  = FALSE;
      dl.showSd2  = FALSE;
      dl.showOut  = FALSE;
      dl.showArw1 = FALSE;
      dl.showArw2 = FALSE;
    }
  else
    {
      if (bSingle)
        {
          if (bIsDia)
            {
              dl.showOut  = TRUE;
              dl.showSd1  = FALSE;
              dl.showSd2  = FALSE;
              dl.showArw1 = TRUE;
              dl.showArw2 = TRUE;
            }
          else
            {
              dl.showOut  = TRUE;
              dl.showSd1  = FALSE;
              dl.showSd2  = FALSE;
              dl.showArw1 = FALSE;
              dl.showArw2 = TRUE;
            }
        }
      else
        {
          if (bIsDia)
            {
              dl.showOut = FALSE;

              // arrow 1
              if (!plinkvar[DIMSD1].dval.ival)
                {
                  dl.showSd1  = TRUE;
                  dl.showArw1 = TRUE;
                }
              else
                {
                  dl.showSd1  = FALSE;
                  dl.showArw1 = FALSE;
                }

              // arrow 2
              if (!plinkvar[DIMSD2].dval.ival)
                {
                  dl.showSd2  = TRUE;
                  dl.showArw2 = TRUE;
                }
              else
                {
                  dl.showSd2  = FALSE;
                  dl.showArw2 = FALSE;
                }
            }
          else
            {
              dl.showOut  = TRUE;

              // arrow 1
              if (!plinkvar[DIMSD1].dval.ival)
                {
                  dl.showSd1  = TRUE;
                }
              else
                {
                  dl.showSd1  = FALSE;
                }

              dl.showArw1 = FALSE;
              dl.showSd2  = FALSE;
              dl.showArw2 = TRUE;
            }
        }
    }

  // arrow 1
  if (dl.showArw1)
    {
      sds_polar(plink->vertex, angle - IC_PI, plink->distance, dl.arw1);
    }

  // arrow 2
  if (dl.showArw2)
    {
      ic_ptcpy(dl.arw2, plink->pt15);
    }

  // Set the rotation of the text.
  dl.rot = GetTextRotation(iDimtih, angle);

  return TRUE;
}

//////////////////////////////////////////////////////////////////////
// recognize whether test and arrow is in or out
static
BOOL
CheckArrowFit(
  sds_real  angle,
  sds_point box2[],
  sds_point ptt[],
  CDimLine& dl
)
{
  int iDimtih = plinkvar[DIMTIH].dval.ival;
  double dDimscale = plinkvar[DIMSCALE].dval.rval;
  double dDimasz   = plinkvar[DIMASZ].dval.rval * dDimscale;
  double ang;
  double d;

  // The following code gets the intersecion points ptt[0] and ptt[1] of
  // the dimension line crossing the dimension textbox which
  // sourounds the dimension text.
  ang = GetTextRotation(iDimtih, angle);
  BOOL b = cmd_getboxintersecEx(
        box2, ang, plink->textmidpt,
        dl.sd1S, dl.sd2S, ptt);
  if (b)
    {
      // there was enough space for the text.

      // Get the distance the text cuts the dimension line.
      d = sds_distance(ptt[0], ptt[1]);

      // Add the distance the arrows would take up.
      d += dDimasz * 2;

      // If the distance of the dimension line is less then the text (and/or),
      // the arrows set dl.arrow to FALSE.
      if (d > plink->dimvalue)
        {
          dl.arrow = FALSE;
          dl.text  = FALSE;
        }
    }
  else
    {
      // there was not enough space.
      dl.text  = FALSE;
      dl.arrow = FALSE;
    }

  return TRUE;
}

//////////////////////////////////////////////////////////////////////
// draw dimension for outside
static
BOOL
DrawOutsideCircle(
  db_drawing*   flp,
  sds_point     txtbox[],
  CDimLine&     dl,
  const double  angle,
  short         dragmode,
  const BOOL    bIsDia
)
{
  int iDimtoh = plinkvar[DIMTOH].dval.ival;
  double dDimscale = plinkvar[DIMSCALE].dval.rval;
  double dDimasz   = plinkvar[DIMASZ].dval.rval * dDimscale;
  double d;

  // If the arrows don't fit with the text then neither will fit.
  // Text :out
  // Arrow:out
  dl.arrow = FALSE;
  dl.showSd1 = FALSE;
  dl.showSd2 = FALSE;
  if (plinkvar[DIMSD1].dval.ival && plinkvar[DIMSD2].dval.ival)
    {
      // hide dimension line
      dl.showOut  = FALSE;
      dl.showArw2 = FALSE;
    }
  else
    {
      // show dimension line
      dl.showOut  = TRUE;
      dl.showArw2 = TRUE;
      ic_ptcpy(dl.arw2, plink->pt15);
    }

  // Creating the exterior lines.

  // DIMASZ DIMASZ
  // |----|----|
  // |    |    |
  // |   /|    |
  // |  / |    |
  // | /  |    |
  //  /   |    |
  // -----------
  //  \ 
  //   \ 
  //    \ 
  //     \ 

  d  = plink->distance + dDimasz;
  sds_polar(plink->vertex, angle, d, dl.outS);

  d  = plink->distance + dDimasz * 2;
  sds_polar(plink->vertex, angle, d, dl.outE);

  // Creating th exterior lines. This creates the horizontal line and moves the text to the correct pos.
  ic_ptcpy(dl.ldrS, dl.outE);
  ic_ptcpy(dl.ldrE, dl.outE);
  ic_ptcpy(plink->textmidpt, dl.outE);

  // draw center mark or line according to DIMTOFL
  DrawCenterMark(dl, flp, bIsDia, dragmode);

  // Set the rotation of the text.
  dl.rot = GetTextRotation(iDimtoh, angle);

  // Moving the text to the correct position.
  MoveDimTextPos(angle, txtbox, plink->textmidpt, dl);

  return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL
cmd_DimRad0(
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
  int iDimtoh = plinkvar[DIMTOH].dval.ival;
  int iDimtad = plinkvar[DIMTAD].dval.ival;
  double dDimscale = plinkvar[DIMSCALE].dval.rval;
  double dDimasz   = plinkvar[DIMASZ].dval.rval * dDimscale;
  double d, ang;
  double dist;

  BOOL bIsOut = TRUE;
  BOOL bIsOpposite = FALSE;
  sds_point ptt[4];

  switch (SDS_AtNodeDrag)
    {
      // Create a new dimension
      case 0:
        {
		  //Modified Cybage SBD 17/12/2001 DD/MM/YYYY 
		  //Reason : Code restructured
		  //Modified Cybage SBD 13/11/2001 DD/MM/YYYY [
		  //Reason : Fix for Bug No. 77866 and 77867 from Bugzilla 
		  if(bdimUpdate==FALSE)
		  {
			  // only decide the position
			  if (plink->GenInitflg == 0)
				{
				  angle = sds_angle(plink->vertex, plink->pt14);
				}

			  // If angle is on the arc or angle is in a circle.
			  // If a circle is selected, eangle is set to 360 degree.
			  ang = angle;
			  if (plink->eangle > IC_PI * 2)
				{
				  if (plink->eangle > (ang + IC_PI * 2))
					{
					  ang += IC_PI * 2;
					}
				}

			  if (ang >= plink->sangle && ang <= plink->eangle)
				{
				  // angle is in arc angle

				  d = plink->distance - dDimasz;
				  sds_polar(plink->vertex, angle, d, dl.sd2S);
				  if (bIsDia)
					{
					  // text position is center
					  ic_ptcpy(plink->textmidpt, plink->vertex);

					  sds_polar(plink->vertex, angle - IC_PI, d, dl.sd1S);

					  // for DIMTAD > 0
					  ic_ptcpy(dl.outS, dl.sd2S);
					  ic_ptcpy(dl.outE, dl.sd1S);
					}
				  else
					{
					  // text position is middle point from center to edge
					  sds_polar(plink->vertex, angle, plink->distance / 2, plink->textmidpt);

					  ic_ptcpy(dl.sd1S, plink->vertex);
					  ic_ptcpy(dl.outS, dl.sd2S);
					}

				  // for DIMTAD > 0
				  ic_ptcpy(dl.outE, dl.sd1S);

				  if (plink->distance < txtbox[1][0])
					{
					  dl.text  = FALSE;
					  dl.arrow = FALSE;
					}
				  else
					{
					  CheckArrowFit(angle, box2, ptt, dl);
					}

				  // If The text and arrow fit.
				  if (dl.text && dl.arrow)
					{
					  // Text :in
					  // Arrow:in
					  bIsOut = FALSE;

					  // draw dimension line in circle
					  DrawInCircle(dl, ptt, angle, bIsDia);
					}
				}
			  else
				{
				  // If the angle is outside of the arc.
				  ang  = angle;
				  double angS = plink->sangle + IC_PI;
				  double angE = plink->eangle + IC_PI;
				  if (angE > IC_PI * 2)
					{
					  if (angE > (ang + IC_PI * 2))
						{
						  ang += IC_PI * 2;
						}
					}

				  // opposite arc angle
				  if (ang >= angS && ang <= angE)
					{
					  bIsOut = FALSE;

					  // only decide the position
					  if (plink->GenInitflg == 0)
						{
						  // arrow position
						  sds_polar(plink->vertex, (angle + IC_PI), plink->distance, plink->pt15);
						}

					  DrawOppositeDimRad(angle, txtbox, dl);

					  sds_polar(plink->vertex, angle - IC_PI, plink->distance, plink->pt15);
					}
				  else
					{
					  // outside of the arc.

					  // Get a near edge point of arc
					  GetNearEdgePoint(angle);
					}
				}

			  break;
			}
        }

      // DDIM command //bdimUpdate==TRUE
      case 1:
        {
          // text is in circle
          dist = sds_distance(plink->vertex, plink->textmidpt);
          if (dist < plink->distance)
            {
              // check in or out
              // move text position on leader
              sds_point pp;
              ic_ptcpy(pp, plink->textmidpt);
              if (iDimtad && iDimtih)
                {
                  pp[1] -= GetUpperDimgap();
                }

              d = sds_distance(pp, plink->vertex);
              if (d > CMD_FUZZ2)
                {
                  double angS = sds_angle(plink->vertex, pp);
                  double angE = sds_angle(plink->vertex, plink->pt15);

                  // opposite arc angle
                  if (fabs(angS - angE) > CMD_FUZZ2)
                    {
                      bIsOpposite = TRUE;
                      break;
                    }
                }

              // for DIMTAD > 0
              ic_ptcpy(dl.outE, dl.sd1S);

              if (plink->distance < txtbox[1][0])
                {
                  dl.text  = FALSE;
                  dl.arrow = FALSE;
                }
              else
                {
                  // move text position on dimension line
                  if (iDimtad && !iDimtih)
                    {
                      d = GetUpperDimgap();

                      // left side
                      ang = IC_PI / 2;
                      if (IsLeftSide(angle))
                        {
                          ang = -ang;
                        }
                      ang = angle - ang;
                      sds_polar(plink->textmidpt, ang, d, plink->textmidpt);
                    }

                  d = plink->distance - dDimasz;
                  sds_polar(plink->vertex, angle, d, dl.sd2S);
                  if (bIsDia)
                    {
                      // text position is center
                      ic_ptcpy(plink->textmidpt, plink->vertex);

                      sds_polar(plink->vertex, angle - IC_PI, d, dl.sd1S);

                      // for DIMTAD > 0
                      ic_ptcpy(dl.outS, dl.sd2S);
                      ic_ptcpy(dl.outE, dl.sd1S);
                    }
                  else
                    {
                      // text position is middle point from center to edge
                      sds_polar(plink->vertex, angle, plink->distance / 2, plink->textmidpt);

                      ic_ptcpy(dl.sd1S, plink->vertex);
                      ic_ptcpy(dl.outS, dl.sd2S);
                    }

                  CheckArrowFit(angle, box2, ptt, dl);
                }

              // If The text and arrow fit.
              if (dl.text && dl.arrow)
                {
                  // Text :in
                  // Arrow:in
                  bIsOut = FALSE;

                  // draw dimension line in circle
                  DrawInCircle(dl, ptt, angle, bIsDia);
                }
            }
          else
            {
              // move text position on dimension line
              sds_point pp;
              ic_ptcpy(pp, plink->textmidpt);
              MoveTextPosOnDimLine(angle, txtbox, pp);

              // If the angle is outside of the arc.
              double angS = sds_angle(plink->vertex, pp);
              double angE = sds_angle(plink->vertex, plink->pt15);

              // opposite arc angle
              if (fabs(angS - angE) > CMD_FUZZ2)
                {
                  bIsOpposite = TRUE;
                  break;
                }
              else
                {
                  // outside of the arc.

                  // Get a near edge point of arc
                  d = plink->distance;
                  plink->pt15[0] = d * cos(angle) + plink->vertex[0];
                  plink->pt15[1] = d * sin(angle) + plink->vertex[1];
                  plink->pt15[2] = plink->vertex[2];
                }
            }

          break;
        }

      // drag the text position
      case 2:
        {
          sds_polar(plink->vertex, angle, plink->distance, plink->pt15);

          // text is in circle
          dist = sds_distance(plink->vertex, plink->textmidpt);
          if (dist < plink->distance)
            {
              d = plink->distance - dDimasz;
              sds_polar(plink->vertex, angle, d, dl.sd2S);
              if (bIsDia)
                {
                  // text position is center
                  ic_ptcpy(plink->textmidpt, plink->vertex);

                  sds_polar(plink->vertex, angle - IC_PI, d, dl.sd1S);

                  // for DIMTAD > 0
                  ic_ptcpy(dl.outS, dl.sd2S);
                  ic_ptcpy(dl.outE, dl.sd1S);
                }
              else
                {
                  // text position is middle point from center to edge
                  sds_polar(plink->vertex, angle, plink->distance / 2, plink->textmidpt);

                  ic_ptcpy(dl.sd1S, plink->vertex);
                  ic_ptcpy(dl.outS, dl.sd2S);
                }

              // for DIMTAD > 0
              ic_ptcpy(dl.outE, dl.sd1S);

              if (plink->distance < txtbox[1][0])
                {
                  dl.text  = FALSE;
                  dl.arrow = FALSE;
                }
              else
                {
                  CheckArrowFit(angle, box2, ptt, dl);
                }

              // If The text and arrow fit.
              if (dl.text && dl.arrow)
                {
                  // Text :in
                  // Arrow:in
                  bIsOut = FALSE;

                  // draw dimension line in circle
                  DrawInCircle(dl, ptt, angle, bIsDia);
                }
            }

          break;
        }

      default:
          return FALSE;
    }

  // draw dimension for opposite side
  if (bIsOpposite)
    {
      bIsOut = FALSE;

      // move text position on leader
      if (iDimtad && iDimtoh)
        {
          plink->textmidpt[1] -= GetUpperDimgap();
        }

      cmd_DimRadExLeader(
        flp, txtbox, box2, dl, angle, bIsDia, dragmode);
    }

  // draw dimension for outside
  if (bIsOut)
    {
      DrawOutsideCircle(flp, txtbox, dl, angle, dragmode, bIsDia);
    }

  return TRUE;
}
