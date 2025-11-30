// =========================================================================================================================================
// ================================================================ Includes ===============================================================
// =========================================================================================================================================

#include "cmds.h"/*DNT*/
#include "dimensions.h"/*DNT*/
#include "cmdsDimVars.h"/*DNT*/
#include "cmdsDimBlock.h"/*DNT*/
// EBATECH(watanabe)-[ 
#include "DimDiaRad.h"/*DNT*/
// ]-EBATECH(watanabe)

// =========================================================================================================================================
// =========================================================== External Variables ==========================================================
// =========================================================================================================================================

extern                        SDS_AtNodeDrag;
extern char         cmd_string1[256], cmd_string2[256], *layername;
extern struct cmd_dimvars plinkvar[];
extern struct cmd_dimeparlink* plink;
//Modified Cybage SBD 13/11/2001 DD/MM/YYYY 
//Reason : Fix for Bug No. 77866 and 77867 from Bugzilla 
extern BOOL bdimUpdate;

// EBATECH(watanabe)-[this code is not used.
#ifdef REMOVED
BOOL cmd_Diam_Radius_Rad(
  db_drawing*   flp,
  sds_point ExtDimInt[],
  sds_point boxrect[],
  sds_point txtbox[],
  sds_point ptt[],
  sds_point ptDLine[],
  sds_real& angle,
  sds_real& TextRotation,
  BOOL&     ArrowFit,
  BOOL&     MakeExteraLine,
  BOOL&     dimDim2flag,
  BOOL&     ArrowFlag1,
  BOOL&     TextFit,
  BOOL&     applyHozText,
  short     dragmode
);

BOOL cmd_Diam_Radius_Dia(
  db_drawing*   flp,
  sds_point ExtDimInt[],
  sds_point boxrect[],
  sds_point txtbox[],
  sds_point ptt[],
  sds_point ptDLine[],
  sds_real& angle,
  sds_real& TextRotation,
  BOOL&     ArrowFit,
  BOOL&     MakeExteraLine,
  BOOL&     dimDim2flag,
  BOOL&     ArrowFlag1,
  BOOL&     TextFit,
  BOOL&     applyHozText,
  short     dragmode
);
#endif //REMOVED
// ]-EBATECH(watanabe)

// ============================================================================================================================================================
// ========================================================= DIAMETER RADIUS FUNCTIONS ========================================================================
// ============================================================================================================================================================
//
//
//
//
// ============================================================================================================================================================
// 2003/3/31 Ebatech(CNBR) : cmd_dimaddXXXX() Add sLweight to support lineweight.

//////////////////////////////////////////////////////////////////////
// draw dimension in block
static
BOOL
DrawInBlock(
  CDimLine&     dl,
  const double  angle,
  char          text[],
  sds_point     box2[],
  sds_point     ipt[],
  db_drawing*   flp,
  const short   dragmode
)
{
  double dDimscale = plinkvar[DIMSCALE].dval.rval;
  double dDimgap   = plinkvar[DIMGAP].dval.rval * dDimscale;
  double dDimtxt   = plinkvar[DIMTXT].dval.rval * dDimscale;
  int iDimclrd  = plinkvar[DIMCLRD].dval.ival;
  int iDimlwd  = plinkvar[DIMLWD].dval.ival;
  int celweight = cmd_getCelweight();
  int irt;

  // first line
  if (dl.showSd1)
    {
      irt = cmd_dimaddline(
                    layername, iDimclrd, iDimlwd,
                    dl.sd1S, dl.sd1E, flp, dragmode);
      if (irt != RTNORM)
        {
          return FALSE;
        }
    }

  // second line
  if (dl.showSd2)
    {
      irt = cmd_dimaddline(
                    layername, iDimclrd, iDimlwd,
                    dl.sd2S, dl.sd2E, flp, dragmode);
      if (irt != RTNORM)
        {
          return FALSE;
        }
    }

  // outside line
  if (dl.showOut)
    {
      irt = cmd_dimaddline(
                    layername, iDimclrd, iDimlwd,
                    dl.outS, dl.outE, flp, dragmode);
      if (irt != RTNORM)
        {
          return FALSE;
        }
    }

  // leader
  if (dl.showLdr)
    {
      irt = cmd_dimaddline(
                    layername, iDimclrd, iDimlwd,
                    dl.ldrS, dl.ldrE, flp, dragmode);
      if (irt != RTNORM)
        {
          return FALSE;
        }
    }

  // extended line
  if (dl.showExt)
    {
      irt = cmd_dimaddline(
                    layername, iDimclrd, iDimlwd,
                    dl.extS, dl.extE, flp, dragmode);
      if (irt != RTNORM)
        {
          return FALSE;
        }
    }

  // center line according to DIMTOFL
  if (dl.showTofl)
    {
      irt = cmd_dimaddline(
                    layername, iDimclrd, iDimlwd,
                    dl.toflS, dl.toflE, flp, dragmode);
      if (irt != RTNORM)
        {
          return FALSE;
        }
    }

  // Get the sin and cos of the angle.
  double dCos = cos(angle);
  double dSin = sin(angle);

  // first arrow
  if (dl.showArw1)
    {
      // SystemMetrix(Hiro)-[FIX: direction of dim block is opposite
      double dAngle = angle;
      if (cmd_string1)
        {
          if (!dl.arrow)
            {
              dAngle += PI;
            }
        }
      // ]-SystemMetrix(Hiro) FIX: direction of dim block is opposite

      cmd_MakeItemHead(
                    ipt, cmd_string1, flp, dragmode, dSin, dCos,
      // SystemMetrix(Hiro)-[FIX: direction of dim block is opposite
                    //angle - IC_PI, !dl.arrow, TRUE, dl.arw1, 0);
                    dAngle - IC_PI, !dl.arrow, TRUE, dl.arw1, 0);
      // ]-SystemMetrix(Hiro) FIX: direction of dim block is opposite
    }

  // second arrow
  if (dl.showArw2)
    {
      // SystemMetrix(Hiro)-[FIX: direction of dim block is opposite
      double dAngle = angle;
      if (cmd_string2)
        {
          if (dl.arrow)
            {
              dAngle += PI;
            }
        }
      // ]-SystemMetrix(Hiro) FIX: direction of dim block is opposite

     cmd_MakeItemHead(
                    ipt, cmd_string2, flp, dragmode, dSin, dCos,
      // SystemMetrix(Hiro)-[FIX: direction of dim block is opposite
                    //angle - IC_PI, dl.arrow, FALSE, dl.arw2, 0);
                    dAngle - IC_PI, dl.arrow, FALSE, dl.arw2, 0);
      // ]-SystemMetrix(Hiro) FIX: direction of dim block is opposite
    }

  // mtext
  irt = cmd_dimaddmtext(
                layername, plinkvar[DIMCLRT].dval.ival, plink->lweight, plink->textmidpt,
                dDimtxt, text, dl.rot, 0.0, plinkvar[DIMTXSTY].dval.cval,
                5, plink->pt210, flp, dragmode);
  if (irt != RTNORM)
    {
      return FALSE;
    }

  // defpoint for point 10
  irt = cmd_dimaddpoint(DIM_DEFPOINTS, iDimclrd, celweight, plink->defpt, flp, dragmode);
  if (irt != RTNORM)
    {
      return FALSE;
    }

  // defpoint for point 15
  irt = cmd_dimaddpoint(DIM_DEFPOINTS, iDimclrd, celweight, plink->pt15, flp, dragmode);
  if (irt != RTNORM)
    {
      return FALSE;
    }

  // box around text
  if (dDimgap < 0.0)
    {
      sds_point boxrect[4];
      sds_point ptt[4];

      int i;
      for (i = 0; i < 4; ++i)
        {
          ic_ptcpy(ptt[i], plink->textmidpt);
        }
      ptt[0][0] -= box2[1][0] / 2; ptt[2][0] -= box2[1][0] / 2;
      ptt[1][0] += box2[1][0] / 2; ptt[3][0] += box2[1][0] / 2;
      ptt[0][1] += box2[1][1] / 2; ptt[1][1] += box2[1][1] / 2;
      ptt[2][1] -= box2[1][1] / 2; ptt[3][1] -= box2[1][1] / 2;

      for (i = 0; i < 4; ++i)
        {
          dSin = sin(0.0);
          dCos = cos(0.0);
          cmd_ucstodim(plink->textmidpt, ptt[i], boxrect[i], dSin, dCos);

          dSin = sin(dl.rot);
          dCos = cos(dl.rot);
          cmd_dimtoucs(plink->textmidpt, boxrect[i], ptt[i], dSin, dCos);
        }

      irt = cmd_drawbox(iDimclrd, iDimlwd, ptt, flp, dragmode);
      if (!irt)
        {
          return FALSE;
        }
    }

  return TRUE;
}

//////////////////////////////////////////////////////////////////////
// get mtext box positions
BOOL
GetMtextBox(
  const char    text[],
  const double  angle,
  sds_point     mtext[]
)
{
  double dDimscale = plinkvar[DIMSCALE].dval.rval;
  double dDimgap   = plinkvar[DIMGAP].dval.rval * dDimscale;
  double dDimtxt   = plinkvar[DIMTXT].dval.rval * dDimscale;

  // Build the mtext insert.
  struct resbuf* rb = sds_buildlist(
                RTDXF0, "MTEXT"/*DNT*/,
                1,      text,
                10,     plink->textmidpt,
                50,     angle,
                40,     dDimtxt,
                41,     0.0,
                51,     plink->hdir,
                7,      plinkvar[DIMTXSTY].dval.cval,
                71,     5,
                72,     1,
                0);
  if (rb == NULL)
    {
      return FALSE;
    }

  // get the width and height of the text.
  sds_textbox(rb, mtext[0], mtext[1]);

  // free resbuf link list.
  IC_RELRB(rb);

  // Caluculate the distance buffer around the text.
  mtext[1][0] += fabs(dDimgap) * 2;
  mtext[1][1] += fabs(dDimgap) * 2;

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MODE = 0,1
// These are the manditory group codes that will be saved every time a radius or diameter dimesion is created. using
// the commands (DIMRADIUS,DIMDIAMETER,DIM RAD,DIM DIA). Example follows.
// --------------------------------------------------------------------------------------------------------
// (8 . "0")                    = Layer name.
// (10 4.41661 5.08314 0.0)     = Center of the circle or arc.
// (11 6.2475 1.32583 0.0)      = Middle point of dimension text.
// (12 0.0 0.0 0.0)             = Insertion point for clones of a dimension. for now 0,0,0
// (70 . 164)                   = Dimension type. and other flags.
// (1 . "")                     = Dimension text explicitly entered by the user.
// (52 . 0.0)                   = Always 0,0
// (53 . 0.0)                   = Rotation angle of dimension text.
// (54 . 0.0)                   = Always 0,0
// (51 . 0.0)                   = Always 0,0
// (210 0.0 0.0 1.0)            = Always 0,0,1
// (3 . "STANDARD")             = Dimension style name.
// (13 0.0 0.0 0.0)             = Always 0,0 Not used in DIA,RAD
// (14 0.0 0.0 0.0)             = Always 0,0 Not used in DIA,RAD
// (15 4.84018 3.59002 0.0)     = Point where line crosses circle or arc. Definition point.
// (16 0.0 0.0 0.0)             = Always 0,0 Not used in DIA,RAD
// (40 . 0.0)                   = Leader length for DIA,RAD
// (50 . 0.0)                   = Always 0,0 Not used in DIA,RAD


BOOL
cmd_Diam_Radius(
  struct cmd_dimeparlink*   plink,
  db_drawing*   flp,
  short         dragmode
)
{
  char      text[IC_ACADBUF] = ""/*DNT*/;
  sds_point ipt[2] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};
  sds_point txtbox[2] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};
  double    angle = 0.0;
  CDimLine  dl;
  int irt = 0;
  double dDimscale = plinkvar[DIMSCALE].dval.rval;
  double dDimasz   = plinkvar[DIMASZ].dval.rval * dDimscale;
  double dDimgap   = plinkvar[DIMGAP].dval.rval * dDimscale;
  int    iDimfit   = plinkvar[DIMFIT].dval.ival;

  BOOL bIsDia;
  if (plink->flags & DIM_RADIUS)
    {
      // radial dimension
      bIsDia = FALSE;
    }
  else
    {
      // diametric dimension
      bIsDia = TRUE;
    }


	// Bugzilla No: 78204 ; 29/07/2002   
	plink->textmidpt[2] = plink->pt14[2] = 0.0;

  switch (SDS_AtNodeDrag)
    {
      // create new dimension
      case 0:
		{
			//Modified Cybage SBD 17/12/2001 DD/MM/YYYY [
			//Reason : Code restructured
			//Modified Cybage SBD 13/11/2001 DD/MM/YYYY [
			//Reason : Fix for Bug No. 77866 and 77867 from Bugzilla 
			//if(bdimUpdate)
			//{
			//	angle = sds_angle(plink->defpt, plink->pt15);
			//	break;
			//}
			//Modified Cybage SBD 13/11/2001 DD/MM/YYYY ]
			if( bdimUpdate==FALSE )
			{
				// EBATECH(CNBR) -[ Bugzilla#78455 13-03-2003 DIMTMOVE=2
				if( plink->HomeText == TRUE || plinkvar[DIMTMOVE].dval.ival != 2 )
					ic_ptcpy(plink->textmidpt, plink->pt14);
				// EBATECH(CNBR) ]-	
				if (plink->GenInitflg)
				{
					// before decision

					// center position
					ic_ptcpy(plink->defpt, plink->pt13);

					// get angle
					// EBATECH(CNBR) -[ Bugzilla#78455 13-03-2003 DIMTMOVE=2
					if( plink->HomeText == FALSE && plinkvar[DIMTMOVE].dval.ival == 2 )
					{
						angle = plink->angle;
					}
					else
					// EBATECH(CNBR) ]-	
					{
						angle = sds_angle(plink->defpt, plink->textmidpt);
					}

					// get point for group code 15
					sds_polar(plink->defpt, angle, plink->distance, plink->pt15);

					// if diameter
					if (bIsDia)
					{
						// move definition point from center to arrow position
						sds_polar(plink->defpt, (angle - IC_PI), plink->distance, plink->defpt);
					}
				}
				else
				{
					// EBATECH(CNBR) -[ Bugzilla#78455 13-03-2003 DIMTMOVE=2
					if( plink->HomeText == FALSE && plinkvar[DIMTMOVE].dval.ival == 2 )
					{
						angle = plink->angle;
					}
					// EBATECH(CNBR) ]-	
					else
					{
						// position was decided

						// if diameter
						if (bIsDia)
						{
							// move definition point from center to arrow position
							double ang = sds_angle(plink->pt13, plink->pt15);
							ang -= IC_PI;
							sds_polar(plink->pt13, ang, plink->distance, plink->defpt);
						}

						// get angle
						angle = sds_angle(plink->defpt, plink->pt15);
					}
				}
				//break;
			}
			break;
		}

      // DDIM command //bdimUpdate==TRUE
      case 1:
        {
          angle = sds_angle(plink->defpt, plink->pt15);
          break;
        }

      // drag the text position
      case 2:
		{
			// EBATECH(CNBR) -[ Bugzilla#78455 13-03-2003 DIMTMOVE=2
			if( plink->HomeText == TRUE || plinkvar[DIMTMOVE].dval.ival != 2 )
			{
				if (bIsDia)
				{
					// if diameter

					sds_point pp;
					ic_midpoint(plink->defpt, plink->pt15, pp);
					angle = sds_angle(pp, plink->textmidpt);

					// Bugzilla No : 78038 ; 01/04/2002 [
					// Previously following code was executed only for DIMFIT = 0 1 2
					
					// if text position and center position between defpt and pt15 is near
					double d = sds_distance(pp, plink->textmidpt);
					if (d < CMD_FUZZ2)
					{
						// horizontal
						angle = 0;
					}
					// Bugzilla No : 78038 ; 01/04/2002 ]
					
					// move arrow positions according to the decided position
					d = sds_distance(plink->defpt, plink->pt15) / 2;
					sds_polar(pp, angle,         d, plink->pt15); 
					sds_polar(pp, angle - IC_PI, d, plink->defpt);
					
				}
				else
				{
					// if radial

					angle = sds_angle(plink->defpt, plink->textmidpt);
					// Bugzilla No : 78038 ; 01/04/2002 [
					// Previously following code was executed only for DIMFIT = 0 1 2

					// move arrow positions according to the decided position
					double d = sds_distance(plink->defpt, plink->pt15);
					sds_polar(plink->defpt, angle, d, plink->pt15);

					// Bugzilla No : 78038 ; 01/04/2002 ]

				}
			}
			break;
			// EBATECH(CNBR) ]-
		}
	}

  // distance from center to arrow position
  double dist = sds_distance(plink->defpt, plink->pt15);
  plink->distance = dist;
  if (bIsDia)
    {
      plink->distance /= 2;
    }

  // Set (plink->dimvalue) to the (radius) or (diameter).
  plink->dimvalue = dist;

  // Generate string for dimension.
  char prefix[2];
  if (bIsDia)
    {
      strcpy(prefix, "D"/*DNT*/);
    }
  else
    {
      strcpy(prefix, "R"/*DNT*/);
    }
  cmd_genstring(plink, text, plink->dimvalue, prefix, TRUE, flp);

  // get mtext box positions
  GetMtextBox(text, plink->textrot, txtbox);

  sds_point box2[2];
  GetMtextBox(text, 0, box2);

  // Set (plink->vertex) to the center point
  if (bIsDia)
    {
      ic_midpoint(plink->defpt, plink->pt15, plink->vertex);
    }
  else
    {
      ic_ptcpy(plink->vertex, plink->defpt);
    }

  // Check if the blocks for internal arrowheads exist
  CreateDimBlock(flp);

  // This starts the creation of the block,
  // it needs to be here becouse of the center marks.
  if (!dragmode && plinkvar[DIMASO].dval.ival)
    {
      irt = dwg_addblock(layername, 256, "*D"/*DNT*/, 1, ipt[0], NULL, flp);
      if (irt != RTNORM)
        {
          return FALSE;
        }
    }

  // calculate points for dimension lines and arrows
  cmd_Diam_Radius_Rad(flp, txtbox, box2, dl, angle, dragmode, bIsDia);

  // draw dimension in block
  DrawInBlock(dl, angle, text, box2, ipt, flp, dragmode);

  // create end block
  if (!dragmode && plinkvar[DIMASO].dval.ival)
    {
      irt = dwg_addendblk(flp);
      if (irt == RTERROR)
        {
          return FALSE;
        }
      db_handitem* handitem = flp->ret_lastblockdef();
      handitem->get_2(plink->bname, sizeof(plink->bname) - 1);
    }

  return TRUE;
}

// EBATECH(watanabe)-[this code is not used.
#ifdef REMOVED
BOOL cmd_Diam_Radius(struct cmd_dimeparlink *plink,db_drawing *flp,short dragmode) {
    char			text[IC_ACADBUF] = ""/*DNT*/;
    sds_point		ipt[2] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}},
					ExtDimInt[2] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}},
					boxrect[4] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}},
					txtbox[2] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}},
					ptt[4] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}},
					ptDLine[8] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0},
								{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0,0.0}};
//                   vertex;
    db_handitem   *handitem=NULL;
    sds_real       angle = 0.0,
                   fr1 = 0.0,
                   fr2 = 0.0,
                   fr3 = 0.0,
                   fr4 = 0.0,
                   TextRotation = 0.0,
                   cosofdeg = 0.0,
                   sinofdeg = 0.0;
    struct resbuf *rbb=NULL;
    short          fi1 = 0,
                   fi2 = 0;
    BOOL           ArrowFit      =TRUE,
                   MakeExteraLine=FALSE,
                   dimDim2flag   =TRUE,
                   ArrowFlag1    =TRUE,
                   TextFit       =TRUE,
                   Otherside     =FALSE,
                   // EBATECH(watanabe)-[ignore pt12Valid
                   //pt12Valid     =FALSE,
                   // ]-EBATECH(watanabe)
                   applyHozText  =TRUE;

//    if (/*plinkvar[DIM_D    IMFIT].dval.ival==4 || */plinkvar[DIMFIT].dval.ival==5) pt12Valid=TRUE;
    if ((SDS_AtNodeDrag && plink->BeginDrag) || (plink->BeginDrag && sds_distance(plink->defpt,plink->pt15))) {
        plink->distance=sds_distance(plink->defpt,plink->pt15);
        plink->distance=(plink->flags&DIM_RADIUS)?plink->distance:(plink->distance/2);

        if (plink->flags&DIM_RADIUS) {
            plink->angle=sds_angle(plink->defpt,plink->pt15);
        } else {
            ptt[0][0]=(plink->defpt[0]+plink->pt15[0])/2;
            ptt[0][1]=(plink->defpt[1]+plink->pt15[1])/2;
            plink->angle=sds_angle(ptt[0],plink->pt15);
        }
    }

    // Set (plink->dimvalue) to the (radius) or (diameter).
    plink->dimvalue=(plink->flags&DIM_RADIUS)?plink->distance:(plink->distance*2);
    // Generate string for dimension.
    cmd_genstring(plink,text,plink->dimvalue,((plink->flags&DIM_RADIUS)?"R"/*DNT*/ :"D"/*DNT*/ ),TRUE,flp);
    // Buld the text insert.
    if ((rbb=sds_buildlist(RTDXF0,"MTEXT"/*DNT*/,1,text,10, plink->textmidpt, 50,0.0,40,plinkvar[DIMTXT].dval.rval*plinkvar[DIMSCALE].dval.rval,41,0.0,51,plink->hdir,7,plinkvar[DIMTXSTY].dval.cval,71,5,72,1,0))==NULL) return(RTERROR);
    // get the width and height of the text.
    sds_textbox(rbb,txtbox[0],txtbox[1]);
    // free resbuf link list.
    IC_RELRB(rbb);
    // Caluculate the distance buffer around the text.
    txtbox[1][0]+=(fabs(plinkvar[DIMGAP].dval.rval*plinkvar[DIMSCALE].dval.rval)*2); txtbox[1][1]+=(fabs(plinkvar[DIMGAP].dval.rval*plinkvar[DIMSCALE].dval.rval)*2);
    // find the point that is at the (angle) and (distance) from pt13 and put it in pt15.
    // If plink->GenInitflg is true then the 13 and 14 points are being used otherwise it's the default points.
    if (plink->GenInitflg || SDS_AtNodeDrag) {
        if (plink->flags&DIM_RADIUS) {
            if (SDS_AtNodeDrag==1 || SDS_AtNodeDrag==6) {
                angle=sds_angle(plink->defpt,plink->textmidpt);
                // Get the point at the end of the line segment from the circle.
                sds_polar(plink->textmidpt,((angle<DIM_270_DEG && angle>DIM_90_DEG)?0.0:IC_PI),((txtbox[1][0]/2)+fabs(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)),ptt[0]);
                fr1=sds_distance(plink->defpt,ptt[0]);
                angle=sds_angle(plink->defpt,plink->pt15);
                if (plinkvar[DIMFIT].dval.ival!=4 && plinkvar[DIMFIT].dval.ival!=5) {
                    sds_polar(plink->defpt,angle,fr1,plink->textmidpt);
                }
            }
            if (SDS_AtNodeDrag) {
                ic_ptcpy(plink->pt13,plink->defpt);
                ic_ptcpy(plink->pt14,plink->textmidpt);
            }
            if (plinkvar[DIMFIT].dval.ival==4 || plinkvar[DIMFIT].dval.ival==5) {
                angle=plink->angle;
            } else {
                // EBATECH(watanabe)-[ignore pt12Valid
                //angle=sds_angle(plink->pt13,((pt12Valid)?plink->blkipt:plink->pt14));
                angle = sds_angle(plink->pt13, plink->pt14);
                // ]-EBATECH(watanabe)
            }
            sds_polar(plink->pt13,angle,plink->distance,plink->pt15);
        } else {
            if ((SDS_AtNodeDrag==1 || SDS_AtNodeDrag==3 || SDS_AtNodeDrag==6) && plinkvar[DIMFIT].dval.ival!=4 && plinkvar[DIMFIT].dval.ival!=5 && !(plink->flags&DIM_DIAMETER)) {
/*
                if (Otherside && SDS_AtNodeDrag==6) {

                    angle=sds_angle(plink->defpt,plink->pt15);
                    // Get the point at the end of the line segment from the circle.
                    sds_polar(plink->textmidpt,((angle<DIM_270_DEG && angle>DIM_90_DEG)?0.0:IC_PI),((txtbox[1][0]/2)+fabs(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)),plink->pt14);
                    // Get the angle of the diameter dimension.
                    angle=sds_angle(plink->defpt,plink->pt14);
                    // Calculate the center point of the circle.
                    sds_polar(plink->defpt,angle,plink->distance,plink->pt13);
                    // Get the angle the the plink->pt15 or plink->defpt point makes.
                    angle=sds_angle(plink->pt13,plink->pt15);
                    // Calculate the plink->defpt point.
                    sds_polar(plink->pt13,(angle-IC_PI),plink->distance,plink->defpt);
                    // Calculate the plink->pt15 point.
                    sds_polar(plink->pt13,angle,plink->distance,plink->pt15);
                    // Get the distance from the center of the circle to the plink->pt14 point.
                    fr1=sds_distance(plink->pt13,plink->pt14);
                    // Calculate the plink->pt14 point.
                    sds_polar(plink->pt13,angle,fr1,plink->pt14);

                } else {
*/
                plink->vertex[0]=(plink->defpt[0]+plink->pt15[0])/2;
                plink->vertex[1]=(plink->defpt[1]+plink->pt15[1])/2;
                fr1=sds_distance(plink->vertex,plink->textmidpt);
                Otherside=(sds_distance(plink->vertex,plink->pt15)>fr1);
                if (Otherside) {
                    angle=sds_angle(plink->defpt,plink->pt15);
                    sds_polar(plink->textmidpt,((angle<DIM_270_DEG && angle>DIM_90_DEG)?IC_PI:0.0),((txtbox[1][0]/2)+fabs(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)),plink->pt14);
                    // Get the angle of the diameter dimension.
                    angle=sds_angle(((SDS_AtNodeDrag==3)?plink->pt15:plink->defpt),plink->pt14);
                    // Calculate the center point of the circle.
                    sds_polar(((SDS_AtNodeDrag==3)?plink->pt15:plink->defpt),((SDS_AtNodeDrag==3)?(angle-IC_PI):angle),plink->distance,plink->pt13);
                    // Get the angle the the plink->pt15 or plink->defpt point makes.
                    angle=sds_angle(plink->pt13,((SDS_AtNodeDrag==3)?plink->defpt:plink->pt15));
                    // Make sure the angle is pointing in the wright direction.
                    if (SDS_AtNodeDrag==3) angle+=IC_PI;
                    // Calculate the plink->defpt point.
                    sds_polar(plink->pt13,(angle-IC_PI),plink->distance,plink->defpt);
                    // Calculate the plink->pt15 point.
                    sds_polar(plink->pt13,angle,plink->distance,plink->pt15);
                    // Get the distance from the center of the circle to the plink->pt14 point.
                    fr1=sds_distance(plink->pt13,plink->pt14);
                    // Calculate the plink->pt14 point.
                    sds_polar(plink->pt13,angle,fr1,plink->pt14);
                } else {
                    angle=sds_angle(plink->defpt,plink->textmidpt);
                    // Get the point at the end of the line segment from the circle.
                    sds_polar(plink->textmidpt,((angle<DIM_270_DEG && angle>DIM_90_DEG)?0.0:IC_PI),((txtbox[1][0]/2)+fabs(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)),plink->pt14);
                    // Get the angle of the diameter dimension.
                    angle=sds_angle(((SDS_AtNodeDrag==3)?plink->pt15:plink->defpt),plink->pt14);
                    // Calculate the center point of the circle.
                    sds_polar(((SDS_AtNodeDrag==3)?plink->pt15:plink->defpt),((SDS_AtNodeDrag==3)?(angle-IC_PI):angle),plink->distance,plink->pt13);
                    // Get the angle the the plink->pt15 or plink->defpt point makes.
                    angle=sds_angle(plink->pt13,((SDS_AtNodeDrag==3)?plink->defpt:plink->pt15));
                    // Make sure the angle is pointing in the wright direction.
                    if (SDS_AtNodeDrag==3) angle+=IC_PI;
                    // Calculate the plink->defpt point.
                    sds_polar(plink->pt13,(angle-IC_PI),plink->distance,plink->defpt);
                    // Calculate the plink->pt15 point.
                    sds_polar(plink->pt13,angle,plink->distance,plink->pt15);
                    // Get the distance from the center of the circle to the plink->pt14 point.
                    fr1=sds_distance(plink->pt13,plink->pt14);
                    // Calculate the plink->pt14 point.
                    sds_polar(plink->pt13,angle,fr1,plink->pt14);
                }
                    // This is just a BANDAID.
                    ////////////////////////////////////////////////////////////
//                }
            } else if (SDS_AtNodeDrag) {
                // Calculate the center point of the circle.
                plink->pt13[0]=plink->defpt[0]+((plink->pt15[0]-plink->defpt[0])/2);
                plink->pt13[1]=plink->defpt[1]+((plink->pt15[1]-plink->defpt[1])/2);
                ic_ptcpy(plink->pt14,plink->textmidpt);

                if (plinkvar[DIMFIT].dval.ival==4 || plinkvar[DIMFIT].dval.ival==5) {
                    angle=plink->angle;
                } else {
                    // EBATECH(watanabe)-[ignore pt12Valid
                    //angle=sds_angle(plink->pt13,((pt12Valid)?plink->blkipt:plink->pt14));
                    angle = sds_angle(plink->pt13, plink->pt14);
                    // ]-EBATECH(watanabe)
                }

                sds_polar(plink->pt13,angle,plink->distance,plink->pt15);
            } else {
                if (plinkvar[DIMFIT].dval.ival==4 || plinkvar[DIMFIT].dval.ival==5) {
                    angle=plink->angle;
                } else {
                    // EBATECH(watanabe)-[ignore pt12Valid
                    //angle=sds_angle(plink->pt13,((pt12Valid)?plink->blkipt:plink->pt14));
                    angle=sds_angle(plink->pt13, plink->pt14);
                    // ]-EBATECH(watanabe)
                }
                sds_polar(plink->pt13,angle,plink->distance,plink->pt15);
            }
        }
    } else {
        angle=sds_angle(plink->defpt,plink->pt15);
    }

// Test area. /////////////////////////////////////////////////////
//if (RTNORM!=cmd_dimaddmtext(layername,3,plink->pt13,(plinkvar[DIMTXT].dval.rval*plinkvar[DIMSCALE].dval.rval),"13",TextRotation,0.0,0.0,plinkvar[DIMTXSTY].dval.cval,1,2,plink->pt13,plink->pt210,flp,dragmode)) goto exit;
//if (RTNORM!=cmd_dimaddmtext(layername,3,plink->pt14,(plinkvar[DIMTXT].dval.rval*plinkvar[DIMSCALE].dval.rval),"14",TextRotation,0.0,0.0,plinkvar[DIMTXSTY].dval.cval,1,2,plink->pt14,plink->pt210,flp,dragmode)) goto exit;
//if (RTNORM!=cmd_dimaddmtext(layername,2,plink->defpt,(plinkvar[DIMTXT].dval.rval*plinkvar[DIMSCALE].dval.rval),"10",TextRotation,0.0,0.0,plinkvar[DIMTXSTY].dval.cval,1,2,plink->defpt,plink->pt210,flp,dragmode)) goto exit;
//if (RTNORM!=cmd_dimaddmtext(layername,2,plink->pt15,(plinkvar[DIMTXT].dval.rval*plinkvar[DIMSCALE].dval.rval),"15",TextRotation,0.0,0.0,plinkvar[DIMTXSTY].dval.cval,1,2,plink->pt15,plink->pt210,flp,dragmode)) goto exit;
//if (RTNORM!=cmd_dimaddmtext(layername,7,plink->textmidpt,(plinkvar[DIMTXT].dval.rval*plinkvar[DIMSCALE].dval.rval),"11",TextRotation,0.0,0.0,plinkvar[DIMTXSTY].dval.cval,1,2,plink->textmidpt,plink->pt210,flp,dragmode)) goto exit;
///////////////////////////////////////////////////////////////////

    // I need to calculate a new text midpoint when an (ENTMAKE) is being done.
    if (((angle>=plink->sangle || angle<=plink->eangle) || (!plink->sangle && !plink->eangle)) && (plinkvar[DIMFIT].dval.ival!=3)) {
        // huhhdkd This is for somthing else.
    } else {
        if (plink->flags&DIM_RADIUS) {
            ic_ptcpy(plink->vertex,plink->defpt);
        } else {
            plink->vertex[0]=(plink->defpt[0]+plink->pt15[0])/2;
            plink->vertex[1]=(plink->defpt[1]+plink->pt15[1])/2;
        }
        fr1=sds_distance(plink->vertex,plink->textmidpt);
        Otherside=(sds_distance(plink->vertex,plink->pt15)>fr1);
        ic_ptcpy(ptt[0],plink->textmidpt);
        if (angle<DIM_270_DEG && angle>DIM_90_DEG) {
            ptt[0][0]+=((Otherside)?-1:1)*((txtbox[1][0]/2)+(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval));
        } else {
            ptt[0][0]-=((Otherside)?-1:1)*((txtbox[1][0]/2)+(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval));
        }
        fr1=sds_distance(plink->pt15,ptt[0]);
        if (
                (sds_distance(plink->vertex,plink->textmidpt) < plink->distance &&
                plinkvar[DIMTIH].dval.ival) ||
                (sds_distance(plink->vertex,plink->textmidpt) > plink->distance &&
                plinkvar[DIMTOH].dval.ival)
            )
        sds_polar(plink->pt15,((Otherside)?(angle+IC_PI):angle),fr1,plink->textmidpt);
    }

    // Calculate the leadlen when dragmode==1, becouse the 14 point is only used when the dim is being dragged.
    if (dragmode==1) plink->leadlen=sds_distance(plink->pt15,plink->pt14);
    // Get the sin and cos of the angle.
    sinofdeg=sin(angle); cosofdeg=cos(angle);
    // Calculate the angle of the text.
    TextRotation=(!plink->txtangflag && !plinkvar[DIMTIH].dval.ival)?(angle):plink->textrot;
    
//	Check if the blocks for internal arrowheads exist
	if (plinkvar[DIMBLK].dval.cval && *plinkvar[DIMBLK].dval.cval)
		strcpy(cmd_string1,plinkvar[DIMBLK].dval.cval);
	else if ((plinkvar[DIMSAH].dval.ival) && *plinkvar[DIMBLK1].dval.cval)
		strcpy(cmd_string1,plinkvar[DIMBLK1].dval.cval);
	else strcpy(cmd_string1,DIM_CLOSED_FILLED);

	if( *cmd_string1 )
		checkBlock( cmd_string1, flp );


	if (plinkvar[DIMBLK].dval.cval && *plinkvar[DIMBLK].dval.cval)
		strcpy(cmd_string2,plinkvar[DIMBLK].dval.cval);
	else if ((plinkvar[DIMSAH].dval.ival) && *plinkvar[DIMBLK2].dval.cval)
		strcpy(cmd_string2,plinkvar[DIMBLK2].dval.cval);
	else strcpy(cmd_string2,DIM_CLOSED_FILLED);

	if( *cmd_string2 )
		checkBlock( cmd_string2, flp );

	
	
	// This starts the creation of the block, it needs to be here becouse of the center marks.
    if (!dragmode && plinkvar[DIMASO].dval.ival) {
        if(RTNORM!=dwg_addblock(layername,256,"*D"/*DNT*/,1,ipt[0],NULL,flp)) goto exit;
    }

    // For (RADIUS) ===========================================================================================================================================
    if (plink->flags&DIM_RADIUS) {
        cmd_Diam_Radius_Rad(
                    flp, ExtDimInt, boxrect, txtbox, ptt, ptDLine, angle,
                    TextRotation, ArrowFit, MakeExteraLine, dimDim2flag,
                    ArrowFlag1, TextFit, applyHozText, dragmode);
    // For (DIAMETER) =========================================================================================================================================
    } else {
        cmd_Diam_Radius_Dia(
                    flp, ExtDimInt, boxrect, txtbox, ptt, ptDLine, angle,
                    TextRotation, ArrowFit, MakeExteraLine, dimDim2flag,
                    ArrowFlag1, TextFit, applyHozText, dragmode);
    }
    // If DIMTOH is on and we are outside the circle OR
    // if DIMTIH is on and we are inside the circle...
    applyHozText = (sds_distance(plink->vertex,plink->textmidpt) < plink->distance &&
                    plinkvar[DIMTIH].dval.ival) ||
                   (sds_distance(plink->vertex,plink->textmidpt) > plink->distance &&
                    plinkvar[DIMTOH].dval.ival);
    // When the text is aligned with the dimension, somtimes the text can be aligned but upside down, so I need to rotate it 180 degrees.
    if (!plink->txtangflag && TextRotation && angle<DIM_270_DEG && angle>DIM_90_DEG && !applyHozText) TextRotation+=IC_PI;
    // Draw DIMENSION .........................................................................................................................................
    // Do only on a Diameter.
    if (plinkvar[DIMFIT].dval.ival==0 && plinkvar[DIMTAD].dval.ival!=0) {
            if (RTNORM!=cmd_dimaddline(layername,plinkvar[DIMCLRD].dval.ival,ptDLine[0],plink->pt15,flp,dragmode)) goto exit;
    } else {
        if (!plinkvar[DIMSD1].dval.ival) {
            if (RTNORM!=cmd_dimaddline(layername,plinkvar[DIMCLRD].dval.ival,ptDLine[0],ptDLine[1],flp,dragmode)) goto exit;
        }
        if (!plinkvar[DIMSD2].dval.ival && dimDim2flag) {
            if (RTNORM!=cmd_dimaddline(layername,plinkvar[DIMCLRD].dval.ival,ptDLine[2],ptDLine[3],flp,dragmode)) goto exit;
        }
    }
    if (!plinkvar[DIMSD1].dval.ival && !(plink->flags&DIM_RADIUS) && ArrowFlag1) {

/*        if (plinkvar[DIMBLK].dval.cval && *plinkvar[DIMBLK].dval.cval) {
            strcpy(cmd_string1,plinkvar[DIMBLK].dval.cval);
        } else if ((plinkvar[DIMSAH].dval.ival) && *plinkvar[DIMBLK1].dval.cval) {
            strcpy(cmd_string1,plinkvar[DIMBLK1].dval.cval);
        } else strcpy(cmd_string1,DIM_CLOSED_FILLED);
*/
        cmd_MakeItemHead(ipt,cmd_string1,flp,dragmode,sinofdeg,cosofdeg,angle - IC_PI/*sds_angle(ptDLine[0],ptDLine[1])*/, !ArrowFit,TRUE,ExtDimInt[0],0);
    }
    if (!plinkvar[DIMSD2].dval.ival) {
/*        if (plinkvar[DIMBLK].dval.cval && *plinkvar[DIMBLK].dval.cval)
            strcpy(cmd_string2,plinkvar[DIMBLK].dval.cval);
        else if ((plinkvar[DIMSAH].dval.ival) && *plinkvar[DIMBLK2].dval.cval)
            strcpy(cmd_string2,plinkvar[DIMBLK2].dval.cval);
        else strcpy(cmd_string2,DIM_CLOSED_FILLED);
*/
        cmd_MakeItemHead(ipt,cmd_string2,flp,dragmode,sinofdeg,cosofdeg,angle - IC_PI/*sds_angle(ptDLine[2],ptDLine[3])*/, ArrowFit,FALSE,ExtDimInt[1],0);
    }
    if (MakeExteraLine) {
        if (RTNORM!=cmd_dimaddline(layername,plinkvar[DIMCLRD].dval.ival,ptDLine[4],ptDLine[5],flp,dragmode)) goto exit;
    }
    // If DIMTAD is on, up the Y coordinate of the dimension by the proper amount
    if (plinkvar[DIMTAD].dval.ival!=0) {
        sds_point tmpTextPt;
        ic_ptcpy(tmpTextPt,plink->textmidpt);
        sds_real tangle = sds_angle(ptDLine[0],plink->pt15);
        sds_real distance = (txtbox[1][1]/2)+((plinkvar[DIMGAP].dval.rval*plinkvar[DIMSCALE].dval.rval)/4);
        bool z = tangle<DIM_270_DEG && tangle>DIM_90_DEG;
        sds_polar(plink->textmidpt,tangle + IC_PI/2,tangle<DIM_270_DEG && tangle>DIM_90_DEG  ? -distance : distance ,tmpTextPt);
        if (RTNORM!=cmd_dimaddmtext(layername,plinkvar[DIMCLRT].dval.ival,tmpTextPt,plinkvar[DIMTXT].dval.rval*plinkvar[DIMSCALE].dval.rval,text,TextRotation,0.0,plinkvar[DIMTXSTY].dval.cval,5,plink->pt210,flp,dragmode)) goto exit;
    } else
        if (RTNORM!=cmd_dimaddmtext(layername,plinkvar[DIMCLRT].dval.ival,plink->textmidpt,plinkvar[DIMTXT].dval.rval*plinkvar[DIMSCALE].dval.rval,text,TextRotation,0.0,plinkvar[DIMTXSTY].dval.cval,5,plink->pt210,flp,dragmode)) goto exit;

    if ((plinkvar[DIMGAP].dval.rval*plinkvar[DIMSCALE].dval.rval)<0.0) {
        // Here need to build box around the text.
        for (fi1=0;fi1<4;++fi1) ic_ptcpy(ptt[fi1],plink->textmidpt);
        ptt[0][0]-=(txtbox[1][0]/2); ptt[2][0]-=(txtbox[1][0]/2);
        ptt[1][0]+=(txtbox[1][0]/2); ptt[3][0]+=(txtbox[1][0]/2);
        ptt[0][1]+=(txtbox[1][1]/2); ptt[1][1]+=(txtbox[1][1]/2);
        ptt[2][1]-=(txtbox[1][1]/2); ptt[3][1]-=(txtbox[1][1]/2);
        sinofdeg=sin(0.0); cosofdeg=cos(0.0);
        for (fi1=0;fi1<4;++fi1) cmd_ucstodim(plink->textmidpt,ptt[fi1],boxrect[fi1],sinofdeg,cosofdeg);
        sinofdeg=sin(TextRotation); cosofdeg=cos(TextRotation);
        for (fi1=0;fi1<4;++fi1) cmd_dimtoucs(plink->textmidpt,boxrect[fi1],ptt[fi1],sinofdeg,cosofdeg);
        if (!cmd_drawbox(plinkvar[DIMCLRD].dval.ival,ptt,flp,dragmode)) goto exit;
    }
    if (!dragmode && plinkvar[DIMASO].dval.ival) {
		if (RTERROR==dwg_addendblk(flp)) goto exit;
	    handitem=flp->ret_lastblockdef();
		handitem->get_2(plink->bname,sizeof(plink->bname)-1);
	}
    // End of Draw DIMENSION...................................................................................................................................

    return(TRUE);
    exit:
        return(FALSE);
}
// ]-EBATECH(watanabe)
#endif //REMOVED
