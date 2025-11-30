// EBATECH(watanabe)-[this code is not used.
#ifdef REMOVED

// =========================================================================================================================================
// ================================================================ Includes ===============================================================
// =========================================================================================================================================

#include "cmds.h"/*DNT*/
#include "dimensions.h"
#include "cmdsDimVars.h"

// =========================================================================================================================================
// =========================================================== External Variables ==========================================================
// =========================================================================================================================================

extern struct cmd_dimvars plinkvar[];
extern struct cmd_dimeparlink* plink;

// ============================================================================================================================================================
// ========================================================= DIAMETER RADIUS FUNCTIONS ========================================================================
// ============================================================================================================================================================

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
)
{
    sds_real       fr1 = 0.0,
                   fr2 = 0.0,
                   fr3 = 0.0,
                   fr4 = 0.0;
    short          fi1 = 0,
                   fi2 = 0;

        // find the point that is at the (angle)+(IC_PI) and distance from pt13 and put it in defpt.
        // If plink->GenInitflg is true then the 13 and 14 points are being used otherwise it's the default points.
        if (plink->GenInitflg || SDS_AtNodeDrag) {
            sds_polar(plink->pt13,(angle+IC_PI),plink->distance,plink->defpt);
            // If (DIMFIT==5) make sure that (plink->textmidpt) is the same as pt14.
            if (plinkvar[DIMFIT].dval.ival==5) {
                ic_ptcpy(plink->textmidpt,plink->pt14);
            } else {
                // Copy plink->pt13 for the insertion point for the text.
                ic_ptcpy(plink->textmidpt,plink->pt13);
            }
        }
        // Calculate the point ((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)) closer to the center from the radius of the circle in ptDLine[1].
        sds_polar(plink->defpt,angle,fabs(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval),ptDLine[0]);
        // Calculate the point ((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)) closer to the center from the radius of the circle in ptDLine[0].
        sds_polar(plink->pt15,(angle+IC_PI),fabs(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval),ptDLine[2]);
        // If angle is on the arc or angle is in a circle.
        if (((angle>=plink->sangle || angle<=plink->eangle) || (!plink->sangle && !plink->eangle)) &&
            (plinkvar[DIMFIT].dval.ival!=3 && plinkvar[DIMFIT].dval.ival!=4)) {
            // The following code gets the intersecion points ptt[0] and ptt[1] of the dimension line crossing the dimension
            // textbox which sourounds the dimension text.
            /*fi2=*/cmd_getboxintersec(txtbox,TextRotation,plink->textmidpt,ptDLine[0],ptDLine[2],ptt,boxrect,&fi1,&fi2);
            // If (fi2=TRUE) then there was enough room for the text.
            if (fi2==3 || fi1==3) {
                // Get the distance the text cuts the dimension line.
                fr1=sds_distance(ptt[0],ptt[1]);
                // Add the distance the arrows would take up.
                fr1+=((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)*4);
                // Check if the arrows and the text will fit in the dimension line.
                if (fr1>plink->dimvalue) {
                    // If DIMFIT==1 then just set the arrows to false and put the text in.
                    if (plinkvar[DIMFIT].dval.ival==1) {
                        ArrowFit=FALSE;
                    } else {
                        TextFit=FALSE;
                        if (((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)*4)>plink->dimvalue)  ArrowFit=FALSE;
                    }
                }
            } else {
                TextFit=FALSE;
                fr1=((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)*4);
                if (fr1>plink->dimvalue || (plinkvar[DIMFIT].dval.ival==1)) ArrowFit=FALSE;
            }
            // These are what the points look like now.
            //            __ pt15,
            //      _____/___ ptDLine[2]
            //     /    /____ ptDLine[3]
            //    |    /  |
            //    |   .   |
            //    |  /____|__ ptDLine[1]
            //     \/____/
            //     /|________ defpt,ptDLine[0]
            //
            if (TextFit && ArrowFit) {
                // Switch the points if the intersections are oppisite of each other.
                if (fabs(ptDLine[0][0]-ptt[0][0])>fabs(ptDLine[0][0]-ptt[1][0])) { ic_ptcpy(ptt[2],ptt[0]); ic_ptcpy(ptt[0],ptt[1]); ic_ptcpy(ptt[1],ptt[2]); }
                ic_ptcpy(ptDLine[1],ptt[0]); ic_ptcpy(ptDLine[3],ptt[1]);
            } else if (ArrowFit) {
                // If The text and arrow fit.
                TextRotation=(!plink->textrot && !plinkvar[DIMTOH].dval.ival)?(angle):plink->textrot;
                ic_ptcpy(ptDLine[1],ptDLine[2]);
                ic_ptcpy(ptDLine[2],plink->pt15);
                sds_polar(ptDLine[2],angle,fabs(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval),ptDLine[3]);
                // If (DIMFIT==5) make sure that (plink->textmidpt) is not calcualted.
                if (plinkvar[DIMFIT].dval.ival!=5) {
                    if (!plinkvar[DIMTOH].dval.ival) {
                        // Moveing the text to the correct position.
                        sds_polar(ptDLine[3],angle,(txtbox[1][0]/2),plink->textmidpt);
                    } else {
                        // Creating th exterior lines. This creates the horizontal line and moves the text to the correct pos.
                        MakeExteraLine=TRUE;
                        ic_ptcpy(ptDLine[4],ptDLine[3]); ic_ptcpy(ptDLine[5],ptDLine[4]);
                        ptDLine[5][0]-=((angle<DIM_270_DEG && angle>DIM_90_DEG)?(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval):-(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval));
                        // Moveing the text to the correct position.
                        ic_ptcpy(plink->textmidpt,ptDLine[5]);
                        plink->textmidpt[0]-=((angle<DIM_270_DEG && angle>DIM_90_DEG)?(txtbox[1][0]/2):-(txtbox[1][0]/2));
                    }
                } else dimDim2flag=FALSE;
            // If the Text fits but the arrows don't.
            } else if (TextFit) {
                // create points for the center mark.
                sds_polar(plink->defpt,angle,plink->distance,ptt[0]);
                cmd_CenterMark(ptt[0],flp,dragmode);
                // Creating th exterior lines 1.
                sds_polar(plink->pt15,angle,fabs(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval),ptDLine[0]);
                sds_polar(ptDLine[0],angle,fabs(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval),ptDLine[1]);
                // Creating th exterior lines 2.
                sds_polar(plink->defpt,(angle+IC_PI),fabs(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval),ptDLine[2]);
                sds_polar(ptDLine[2],(angle+IC_PI),fabs(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval),ptDLine[3]);
            } else {
                // create points for the center mark.
                sds_polar(plink->defpt,angle,plink->distance,ptt[0]);
                cmd_CenterMark(ptt[0],flp,dragmode);
                // Turn off opposit side arrow.
                ArrowFlag1=FALSE;
                // Set the rotation of the text.
                TextRotation=(!plink->textrot && !plinkvar[DIMTOH].dval.ival)?(angle):plink->textrot;
                // Creating th exterior lines.
                sds_polar(plink->pt15,angle,fabs(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval),ptDLine[0]);
                sds_polar(ptDLine[0],angle,fabs(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval),ptDLine[1]);
                // If (DIMFIT==5) make sure that (plink->textmidpt) is not calcualted.
                if (plinkvar[DIMFIT].dval.ival!=5) {
                    if (!plinkvar[DIMTOH].dval.ival) {
                        dimDim2flag=FALSE;
                        // Moveing the text to the correct position.
                        sds_polar(ptDLine[1],angle,(txtbox[1][0]/2),plink->textmidpt);
                    } else {
                        // Creating th exterior lines. This creates the horizontal line and moves the text to the correct pos.
                        ic_ptcpy(ptDLine[2],ptDLine[1]);
                        ic_ptcpy(ptDLine[3],ptDLine[2]);
                        ptDLine[3][0]-=((angle<DIM_270_DEG && angle>DIM_90_DEG)?(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval):-(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval));
                        // Moveing the text to the correct position.
                        ic_ptcpy(plink->textmidpt,ptDLine[3]);
                        plink->textmidpt[0]-=((angle<DIM_270_DEG && angle>DIM_90_DEG)?(txtbox[1][0]/2):-(txtbox[1][0]/2));
                    }
                } else dimDim2flag=FALSE;
            }
            ic_ptcpy(ExtDimInt[0],plink->defpt);
            ic_ptcpy(ExtDimInt[1],plink->pt15);
        // If the angle is outside of the arc.
        } else {
            if (plink->GenInitflg || SDS_AtNodeDrag) ic_ptcpy(plink->textmidpt,plink->pt14);
            // Make sure that angle is greater then plink->sangle.
            if (plink->sangle) ic_normang(&plink->sangle,&angle);
            if (angle>=plink->eangle && (plink->sangle || plink->eangle)) {
                ic_ptcpy(ptDLine[1],plink->defpt);
            } else {
                ic_ptcpy(ptDLine[1],plink->pt15);
                // Setup center mark points.
                sds_polar(plink->defpt,angle,plink->distance,ptt[0]);
                cmd_CenterMark(ptt[0],flp,dragmode);
            }
            dimDim2flag=FALSE;
            ArrowFit=(sds_distance(plink->textmidpt,plink->defpt)<plink->dimvalue)?TRUE:FALSE;
            // If the dimension is being done on an arc and is at the opened part of the arc. Make sure that ArrowFit
            // is false so that the arrow won't be reverse when dragged whin the radius of the arc.
            if (plink->eangle && plink->sangle && angle>=plink->eangle && ArrowFit) ArrowFit=0;
            if (ArrowFit) { fr1=angle; angle+=IC_PI; ic_normang(&angle,&angle); } 
            // If DIMTOH is on and we are outside the circle OR
            // if DIMTIH is on and we are inside the circle...
            applyHozText = (sds_distance(plink->vertex,plink->textmidpt) < plink->distance &&
                            plinkvar[DIMTIH].dval.ival) ||
                           (sds_distance(plink->vertex,plink->textmidpt) > plink->distance &&
                            plinkvar[DIMTOH].dval.ival);
            if (applyHozText) {
                TextRotation=(!plink->textrot)?0.0:plink->textrot;
                MakeExteraLine=TRUE;
                if (plinkvar[DIMFIT].dval.ival==4) {
                    //            ______ angle (fr2)
                    //      _____/
                    //     /     \
                    //    |    /  |
                    //    |   .   |
                    //    |  /    |
                    //     \/____/
                    // TEXT/_____________angle (fr1)
                    //
                    // fr1 = The angle made by the center of the circle and (ptDLine[1]).
                    fr1=sds_angle(plink->defpt,ptDLine[1]);
                    // fr2 = The angle made  by fr1 + 180 degrees.
                    fr2=fr1+IC_PI;
                    // fr3 = The angle made by the center of the circle and the center of the text.
                    fr3=sds_angle(plink->defpt,plink->textmidpt);
                    // Make sure that fr3 is greater then fr1.
                    ic_normang(&fr1,&fr3);

                    if (fr1>IC_PI) {
                        sds_polar(plink->textmidpt,((fr3>fr2)?0.0:IC_PI),(txtbox[1][0]/2),ptDLine[4]);
                    } else if (fr1<IC_PI) {
                        sds_polar(plink->textmidpt,((fr3<fr2)?0.0:IC_PI),(txtbox[1][0]/2),ptDLine[4]);
                    }

                    fi1=ic_linexline(plink->defpt,ptDLine[1],plink->textmidpt,ptDLine[4],ptt[0]);
                    // Make sure that the distance from the tip of the arrow outwoard is (2*DIMASZ), This forces the distance.
                    if (sds_distance(ptDLine[1],ptt[0])<((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)*2)) {
                        sds_polar(ptDLine[1],angle,((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)*2),ptt[0]);
                        ic_ptcpy(ptDLine[5],ptt[0]);
                        plink->textmidpt[1]=ptt[0][1];
                        if (fr1>IC_PI) {
                            sds_polar(plink->textmidpt,((fr3>fr2)?0.0:IC_PI),(txtbox[1][0]/2),ptDLine[4]);
                        } else if (fr1<IC_PI) {
                            sds_polar(plink->textmidpt,((fr3<fr2)?0.0:IC_PI),(txtbox[1][0]/2),ptDLine[4]);
                        }
                    } else {
                         ic_ptcpy(ptDLine[5],ptt[0]);
                    }
                    // (ptt[0]) is where vector (plink->defpt,ptDLine[1]) and vector (plink->textmidpt,ptDLine[4]) cross.
                    ic_ptcpy(ptDLine[0],ptt[0]);
                    //
                    //                  /------ angle (fr1)
                    //                 /
                    //                /
                    //               /
                    //  SOMETEXT----- ----- point where vector of (fr1) and textmidpt meet.
                    //
                    // Get the distance from the center of the text and where the point made by the vector from angle (fr1) and the vector made by
                    // the text midpoit meet.
                    fr4=sds_distance(plink->textmidpt,ptt[0]);
                    if (fr3<fr2) {
                    // Above the angle fr1.
                        if (fr4<(((txtbox[1][0]/2)+plinkvar[DIMASZ].dval.rval)*plinkvar[DIMSCALE].dval.rval)) {
                            if (fr1>IC_PI && fr1<DIM_270_DEG) {
                                sds_polar(ptDLine[5],0.0,(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval),ptDLine[4]);
                                sds_polar(ptDLine[4],0.0,(txtbox[1][0]/2),plink->textmidpt);
                            } else {
                                sds_polar(ptDLine[5],IC_PI,(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval),ptDLine[4]);
                                sds_polar(ptDLine[4],IC_PI,(txtbox[1][0]/2),plink->textmidpt);
                            }
                        }
                    } else {
                    // Below angle fr1.
                        if (fr4<(((txtbox[1][0]/2)+plinkvar[DIMASZ].dval.rval)*plinkvar[DIMSCALE].dval.rval)) {
                            if (fr1>IC_PI && fr1<DIM_270_DEG) {
                                sds_polar(ptDLine[5],IC_PI,(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval),ptDLine[4]);
                                sds_polar(ptDLine[4],IC_PI,(txtbox[1][0]/2),plink->textmidpt);
                            } else {
                                sds_polar(ptDLine[5],0.0,(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval),ptDLine[4]);
                                sds_polar(ptDLine[4],0.0,(txtbox[1][0]/2),plink->textmidpt);
                            }
                        }
                    }
                    // Calculate the direction the arrow should be faceing.
                    fr4=sds_angle(ptDLine[1],ptt[0]);
                    ArrowFit=(fr4>(fr1-CMD_FUZZ) && fr4<(fr1+CMD_FUZZ))?FALSE:TRUE;
                    ArrowFlag1=FALSE;

/* ~THIS IS SOME OLD CODE 03-17-98
                    sds_polar(plink->textmidpt,((angle<DIM_270_DEG && angle>DIM_90_DEG)?0.0:IC_PI),(txtbox[1][0]/2),ptDLine[4]);
                    fi1=ic_linexline(plink->defpt,ptDLine[1],plink->textmidpt,ptDLine[4],ptt[0]);
                    ic_ptcpy(ptDLine[0],ptt[0]);
                    // Make sure that the distance from the tip of the arrow outwoard is (2*DIMASZ), This forces the distance.
                    if (sds_distance(ptDLine[1],ptDLine[0])<((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)*2)) {
                        sds_polar(ptDLine[1],angle,((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)*2),ptDLine[0]);
                        ic_ptcpy(ptDLine[5],ptDLine[0]);
                        plink->textmidpt[1]=ptDLine[0][1];
                        sds_polar(plink->textmidpt,(0.0+IC_PI),(txtbox[1][0]/2),ptDLine[4]);
                    } else {
                         ic_ptcpy(ptDLine[5],ptt[0]);
                    }
                    // Make sure that the distance from the text to the line is at least (DIMASZ)
                    if (sds_distance(ptDLine[4],ptDLine[5])<(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)) {
                        sds_polar(ptDLine[5],0.0,(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval),ptDLine[4]);
                        sds_polar(ptDLine[4],0.0,(txtbox[1][0]/2),plink->textmidpt);
                    }
*/



                } else {
                    ArrowFlag1=FALSE;
//                    if (plink->GenInitflg || SDS_AtNodeDrag) {
                        if (angle<DIM_270_DEG && angle>DIM_90_DEG) {
                            plink->textmidpt[0]-=((txtbox[1][0]/2)+(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval));
                        } else {
                            plink->textmidpt[0]+=((txtbox[1][0]/2)+(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval));
                        }
//                    }
                    sds_polar(plink->textmidpt,((angle<DIM_270_DEG && angle>DIM_90_DEG)?0.0:IC_PI),(txtbox[1][0]/2),ptDLine[4]);
                    ic_ptcpy(ptDLine[5],ptDLine[4]);

                    // Extend the line under the dimtext id dimtad != 0;
                    if (plinkvar[DIMTAD].dval.ival!=0) 
                        ptDLine[4][0] += (angle<DIM_270_DEG && angle>DIM_90_DEG)? -1*fabs(txtbox[1][0] - txtbox[0][0]): fabs(txtbox[1][0] - txtbox[0][0]) ;

                    ptDLine[5][0]-=((angle<DIM_270_DEG && angle>DIM_90_DEG)?-(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval):(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval));
                    // Make sure that the distance from the tip of the arrow outwoard is (2*DIMASZ), This forces the distance.
                    if (sds_distance(ptDLine[1],ptDLine[5])<((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)*2)) {
                        sds_polar(ptDLine[1],angle,((plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval)*2),ptDLine[0]);
                        ic_ptcpy(ptDLine[5],ptDLine[0]);
                        sds_polar(ptDLine[5],((angle<DIM_270_DEG && angle>DIM_90_DEG)?IC_PI:0.0),(plinkvar[DIMASZ].dval.rval*plinkvar[DIMSCALE].dval.rval),ptDLine[4]);
                        ic_ptcpy(plink->textmidpt,ptDLine[4]);
                        sds_polar(ptDLine[4],((angle<DIM_270_DEG && angle>DIM_90_DEG)?IC_PI:0.0),(txtbox[1][0]/2),plink->textmidpt);
                    } else {
                        ic_ptcpy(ptDLine[0],ptDLine[5]);
                    }
                }
            // If DIMTOH is off then make the dimension line the same as (angle).
            } else {
                //ArrowFlag1 = FALSE;
                //TextRotation = angle;
                //sds_polar(plink->textmidpt,(angle+IC_PI),(txtbox[1][0]/2),ptDLine[0]);
                ArrowFlag1=FALSE;//MHB
                TextRotation=(!plink->textrot)?angle:plink->textrot;
                if (plinkvar[DIMTAD].dval.ival!=0) {
                    sds_polar(plink->textmidpt,(angle+IC_PI),(txtbox[0][0]- (txtbox[1][0]/2)),ptDLine[0]);
                } else
                    sds_polar(plink->textmidpt,(angle+IC_PI),(txtbox[1][0]/2),ptDLine[0]);
            }
            ic_ptcpy(ExtDimInt[1],ptDLine[1]);
        }

    return(TRUE);
}

#endif // REMOVED
// ]-EBATECH(watanabe)
