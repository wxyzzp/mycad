// Radial dimension
// DIMFIT = 3

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
BOOL
cmd_DimRad3(
  db_drawing*   flp,
  sds_point txtbox[],
  sds_point box2[],
  CDimLine& dl,
  sds_real& angle,
  short     dragmode,
  const BOOL bIsDia
)
{
  int iDimtoh = plinkvar[DIMTOH].dval.ival;
  double dDimscale = plinkvar[DIMSCALE].dval.rval;
  double dDimasz   = plinkvar[DIMASZ].dval.rval * dDimscale;
  double ang;

  switch (SDS_AtNodeDrag)
    {
      // Create a new dimension
      case 0:
        {
		 //Modified Cybage SBD 17/12/2001 DD/MM/YYYY 
		 //Reason : Code restructured
		 //Modified Cybage SBD 13/11/2001 DD/MM/YYYY [
		 //Reason : Fix for Bug No. 77866 and 77867 from Bugzilla 
		 //if(bdimUpdate)
		 //{
		 //	cmd_DimRadExLeader(
         //          flp, txtbox, box2, dl, angle, bIsDia, dragmode);
		 //   break;
		 //}
		 //Modified Cybage SBD 13/11/2001 DD/MM/YYYY ]

         if(bdimUpdate==FALSE)
		 {
			 BOOL bIsInside = FALSE;
			 dl.arrow = FALSE;
			 
			 // only decide the position
			 if (plink->GenInitflg == 0)
			 {
				 // get angle
				 angle = sds_angle(plink->vertex, plink->textmidpt);
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
				 
				 sds_polar(plink->vertex, ang, plink->distance, plink->pt15);
				 
				 cmd_DimRadExLeader(
					 flp, txtbox, box2, dl, angle, bIsDia, dragmode);
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
					 // only decide the position
					 if (plink->GenInitflg == 0)
					 {
						 // arrow position
						 sds_polar(plink->pt13, (angle + IC_PI), plink->distance, plink->pt15);
						 if (bIsDia)
						 {
							 sds_polar(plink->pt13, angle, plink->distance, plink->defpt);
						 }
					 }
					 
					 DrawOppositeDimRad(angle, txtbox, dl);
				 }
				 else
				 {
					 // outside of the arc.
					 
					 // Get a near edge point of arc
					 GetNearEdgePoint(angle);
					 
					 cmd_DimRadExLeader(
						 flp, txtbox, box2, dl, angle, bIsDia, dragmode);
				 }
			 }
			 
			 break;
			}
		 }

      // DDIM command  //bdimUpdate==TRUE
      case 1:
      // drag the text position
      case 2:
        {
          angle = sds_angle(plink->vertex, plink->pt15);
          cmd_DimRadExLeader(
                    flp, txtbox, box2, dl, angle, bIsDia, dragmode);

          break;
        }
    }
  return TRUE;
}
