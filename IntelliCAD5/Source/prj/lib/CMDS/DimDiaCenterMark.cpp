// =========================================================================================================================================
// ================================================================ Includes ===============================================================
// =========================================================================================================================================

#include "cmds.h"/*DNT*/
#include "dimensions.h"
#include "cmdsDimVars.h"

// =========================================================================================================================================
// =========================================================== External Variables ==========================================================
// =========================================================================================================================================

extern char         *layername;
extern struct cmd_dimvars plinkvar[];
extern struct cmd_dimeparlink* plink;

// ============================================================================================================================================================
// ========================================================= DIAMETER RADIUS FUNCTIONS ========================================================================
// ============================================================================================================================================================
// NOTE : 2003/3/31 Center mark color is DIMCLRE so lineweight is DIMLWE.

void cmd_CenterMark(sds_point vertexpt,db_drawing *flp,short dragmode) {
    sds_point pt1 = {0.0, 0.0, 0.0},
              pt2 = {0.0, 0.0, 0.0};
    short     fi1 = 0;
	//Bugzilla No 78167 05-08-2002
	sds_real  fr1 = 0.0;

    for (fi1=0;fi1<2;fi1++) {
        ic_ptcpy(pt1,vertexpt); ic_ptcpy(pt2,vertexpt);
        switch(fi1) {
            case 0: // Draw horizontal center LINE
                pt1[0]-=(fabs(plinkvar[DIMCEN].dval.rval)*plinkvar[DIMSCALE].dval.rval);
                pt2[0]+=(fabs(plinkvar[DIMCEN].dval.rval)*plinkvar[DIMSCALE].dval.rval);
                break;
            case 1: // Draw vertical center LINE
                pt1[1]+=(fabs(plinkvar[DIMCEN].dval.rval)*plinkvar[DIMSCALE].dval.rval);
                pt2[1]-=(fabs(plinkvar[DIMCEN].dval.rval)*plinkvar[DIMSCALE].dval.rval);
                break;
            default :
                return;
        }
        cmd_dimaddline(layername,plinkvar[DIMCLRE].dval.ival,plinkvar[DIMLWE].dval.ival,pt1,pt2,flp,dragmode);
    }
    if (plinkvar[DIMCEN].dval.rval<0.0) {
        plinkvar[DIMCEN].dval.rval=(-plinkvar[DIMCEN].dval.rval);
		//Bugzilla No 78167 09-12-2002
		fr1 = (plinkvar[DIMCEN].dval.rval*plinkvar[DIMSCALE].dval.rval)*2.0 ;
        // Draw center MARKS
        for (fi1=0;fi1<4;fi1++) {
            ic_ptcpy(pt1,vertexpt); ic_ptcpy(pt2,vertexpt);
            switch(fi1) {
                case 0: // Draw left horizontal center MARK
                    pt1[0]-=(plink->distance+(plinkvar[DIMCEN].dval.rval*plinkvar[DIMSCALE].dval.rval));
                    pt2[0]-=((plinkvar[DIMCEN].dval.rval*plinkvar[DIMSCALE].dval.rval)*2.0);
                    break;
                case 1: // Draw right horizontal center MARK
                    pt1[0]+=(plink->distance+(plinkvar[DIMCEN].dval.rval*plinkvar[DIMSCALE].dval.rval));
                    pt2[0]+=((plinkvar[DIMCEN].dval.rval*plinkvar[DIMSCALE].dval.rval)*2.0);
                    break;
                case 2: // Draw top vertical center MARK
                    pt1[1]+=(plink->distance+(plinkvar[DIMCEN].dval.rval*plinkvar[DIMSCALE].dval.rval));
                    pt2[1]+=((plinkvar[DIMCEN].dval.rval*plinkvar[DIMSCALE].dval.rval)*2.0);
                    break;
                case 3: // Draw bottom vertical center MARK
                    pt1[1]-=(plink->distance+(plinkvar[DIMCEN].dval.rval*plinkvar[DIMSCALE].dval.rval));
                    pt2[1]-=((plinkvar[DIMCEN].dval.rval*plinkvar[DIMSCALE].dval.rval)*2.0);
                    break;
                default :
					//Bugzilla No 63543 13-04-2002
					plinkvar[DIMCEN].dval.rval=(-plinkvar[DIMCEN].dval.rval);
                    return;
            }
            
			//Bugzilla No 78167 05-08-2002
			if ( fi1 == 0 )
			{
				//fr1 = sds_distance( vertexpt, pt2 ); 
				if ( fr1 > plink->distance && !icadRealEqual(fr1, plink->distance) )
				break;
			}
            cmd_dimaddline(layername,plinkvar[DIMCLRE].dval.ival,plinkvar[DIMLWE].dval.ival,pt1,pt2,flp,dragmode);
        }
		//Bugzilla No 63543 13-04-2002
		plinkvar[DIMCEN].dval.rval=(-plinkvar[DIMCEN].dval.rval);
    }
}
