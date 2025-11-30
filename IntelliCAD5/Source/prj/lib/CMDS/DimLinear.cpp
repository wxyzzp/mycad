// =========================================================================================================================================
// ================================================================ Includes ===============================================================
// =========================================================================================================================================

#include "cmds.h"/*DNT*/
#include "dimensions.h"
#include "cmdsDimVars.h"
#include "IcadView.h"
#include "cmdsDimBlock.h"
// EBATECH(watanabe)-[
//#include "cmdsNotUsed.h"
// ]-EBATECH(watanabe)

// EBATECH(watanabe)-[text rotation 
BOOL bTextFreeAng;
// ]-EBATECH(watanabe)

static void
CreateDimArrow(
 db_drawing*	flp,
 BOOL			dimsoxdflag
);

static BOOL
DrawDimLinear(
	struct cmd_dimeparlink*	plink,
	db_drawing*	flp,
	short 		dragmode,
	sds_point 	ptExtL[],
	sds_point 	ptDimL[],
	sds_point 	ptt[],
	sds_point 	box2[],
	sds_point 	boxrect[],
	sds_point 	ExtDimInt[],
	sds_point 	textIns,
	sds_real		cosofdeg,
	sds_real		sinofdeg,
	sds_real		TextRotation,
	char			text[],
	BOOL			dimsoxdflag,
	BOOL			dimDim2flag,
	BOOL			MakeCenterLine,
	BOOL			MakeExteraLine1,
	BOOL			MakeExteraLine2,
	BOOL			dimtad4Flg,
	BOOL			ArrowFit
);

// =========================================================================================================================================
// =========================================================== External Variables ==========================================================
// =========================================================================================================================================

extern				SDS_AtNodeDrag;
//extern BOOL		useStoredValues;
extern sds_point	dimzeropt; // Needed to put this variable here because it gets initialized.
extern char 		cmd_string[256], cmd_string1[256], cmd_string2[256], *layername;
extern struct cmd_dimvars plinkvar[];
// EBATECH(watanabe)-[
//extern struct cmd_dimlastpt lastdimpts;
// ]-EBATECH(watanabe)
extern struct cmd_dimeparlink* plink;

// EBATECH(watanabe)-[check valid blockname of arrow
BOOL
IsValidBlockName(
	int	iBlk
)
{
	char* fcp1 = strchr(plinkvar[iBlk].dval.cval, '_'/*DNT*/);
	if (fcp1 == NULL)
	{
		fcp1 = plinkvar[iBlk].dval.cval;
	}
	else
	{
		fcp1++;
	}

	if (strisame(plinkvar[iBlk].dval.cval,DIM_OBLIQUE)	 ||
		strisame(plinkvar[iBlk].dval.cval,DIM_ARCH_TICK) ||
		strisame(plinkvar[iBlk].dval.cval,DIM_INTEGRAL)  ||
		strisame(plinkvar[iBlk].dval.cval,DIM_NONE))
	{
		return TRUE;
	}

	return FALSE;
}

static void
CreateDimfitLeader1(
	sds_point 	ptDimL[],
	sds_point 	textIns,
	sds_point 	box2[],
	BOOL			bArrowFit
)
{
	// first point of leader
	sds_point ps, pe;
	if (bArrowFit)
	{
		ic_ptcpy(ps, ptDimL[0]);
		ic_ptcpy(pe, ptDimL[1]);
	}
	else
	{
		ic_ptcpy(ps, ptDimL[4]);
		ic_ptcpy(pe, ptDimL[5]);
	}
	ptDimL[6][0] = (ps[0] + pe[0]) / 2;
	ptDimL[6][1] = (ps[1] + pe[1]) / 2;

	// leader length
	double dLen;
	dLen	= plinkvar[DIMEXE].dval.rval;
	dLen += plinkvar[DIMDLI].dval.rval;
	dLen += plinkvar[DIMEXO].dval.rval;
	dLen *= plinkvar[DIMSCALE].dval.rval;

	// second point of leader
	double dAngle = DIM_90_DEG / 2 + DIM_90_DEG / 3;
	sds_polar(ptDimL[6], dAngle, dLen, ptDimL[7]);

	ic_ptcpy(ptDimL[8], ptDimL[7]);
	ic_ptcpy(ptDimL[9], ptDimL[7]);

	// last point of leader
	int iDimtad = plinkvar[DIMTAD].dval.ival;
	switch (iDimtad)
	{
		case 0:
			 //EBATECH(CNBR) 13-03-2003
			 ptDimL[9][0] += fabs(plinkvar[DIMGAP].dval.rval * plinkvar[DIMSCALE].dval.rval);
			 //ptDimL[9][0] += fabs(plinkvar[DIMGAP].dval.rval);
			break;
		default:
			break;
	}

	// text position
	ic_ptcpy(textIns, ptDimL[9]);
	textIns[0]	 += box2[1][0] / 2;
	switch (iDimtad)
	{
		case 0:
			break;
		default:
			ptDimL[9][0] += box2[1][0];
			break;
	}
}

static void
CreateDimfitLeader2(
	sds_point 	ptDimL[],
	sds_point 	textIns,
	sds_point 	box2[],
	sds_point 	txtbox[],
	BOOL			FlipDimXaxis,
	BOOL			SideofTextDrag,
	BOOL			Drag11point,
	sds_real		fr2,
	BOOL			bArrowFit
)
{
	double fr3 = DIM_90_DEG;

	// first point of leader
	sds_point ps, pe;
	if (bArrowFit)
	{
		ic_ptcpy(ps, ptDimL[0]);
		ic_ptcpy(pe, ptDimL[1]);
	}
	else
	{
		ic_ptcpy(ps, ptDimL[4]);
		ic_ptcpy(pe, ptDimL[5]);
	}
	ptDimL[6][0] = (ps[0] + pe[0]) / 2;
	ptDimL[6][1] = (ps[1] + pe[1]) / 2;

	// second point of leader
	double fr1;
	fr1  = plinkvar[DIMEXE].dval.rval;
	fr1 += plinkvar[DIMDLI].dval.rval;
	fr1 += plinkvar[DIMEXO].dval.rval;
	fr1 *= plinkvar[DIMSCALE].dval.rval;
	sds_polar(ptDimL[6], fr3, fr1, ptDimL[7]);

	ic_ptcpy(ptDimL[8], ptDimL[7]);

	fr1  = fabs(plinkvar[DIMGAP].dval.rval);
	fr1 *= plinkvar[DIMSCALE].dval.rval;
	if (Drag11point)
	{
		cmd_DimCAngle_FitText(FlipDimXaxis, SideofTextDrag, &fr3, fr2);

		// last point of leader
		double len = box2[1][0];
		int iDimtad = plinkvar[DIMTAD].dval.ival;
		switch (iDimtad)
		{
			case 0:
				len = fr1;
				break;
			default:
				break;
		}

		sds_polar(ptDimL[8], fr3, len, ptDimL[9]);
		ptDimL[9][0] -= ptDimL[8][0];
		ptDimL[9][1] -= ptDimL[8][1];
		ptDimL[9][2] -= ptDimL[8][2];

		// text position
		len = txtbox[1][0] / 2;
		switch (iDimtad)
		{
			case 0:
				len += fr1;
				break;
			default:
				break;
		}
		sds_polar(ptDimL[8], fr3, len, textIns);

		switch (iDimtad)
		{
			case 0:
				break;
			default:
				textIns[1] += txtbox[1][1] / 2;
				if (plinkvar[DIMGAP].dval.rval < 0)
				{
					textIns[1] += fr1;
				}
				textIns[1] = fabs(textIns[1]);
				break;
		}

		textIns[0] -= ptDimL[8][0];
		textIns[1] -= ptDimL[8][1];
		textIns[2] -= ptDimL[8][2];
	}
}
// ]-EBATECH(watanabe)

// SystemMetrix(Hiro)-[FIX: Dimension line is short when outside text is dragged.
static
BOOL
ExtendDimLine(
	sds_point 	textIns,
	sds_point 	txtbox[],
	const double	InterDist,
	const BOOL	SideofTextDrag,
	sds_point 	ptDimL[]
)
{
	ptDimL[3][0] = textIns[0];
	if (plinkvar[DIMTAD].dval.ival)
	{

		double d = -txtbox[1][0] / 2;
		if (SideofTextDrag)
		{
			d = -d;
		}

		ptDimL[3][0] += d;
	}
	else
	{
		ptDimL[3][0] += InterDist;
	}

	return TRUE;
}

static
BOOL
ExtendDimLineEx(
	sds_point 	textIns,
	sds_point 	txtbox[],
	sds_point 	ptExtL[],
	const BOOL	SideofTextDrag,
	sds_point 	ptDimL[]
)
{
	int i = 0;
	if (SideofTextDrag)
	{
		i = 2;
	}
	ptDimL[2][0] = ptExtL[i][0];

	BOOL bIsLeft = FALSE;
	if (textIns[0] < ptExtL[0][0])
	{
		bIsLeft = TRUE;
	}

	double dDimasz = plinkvar[DIMASZ].dval.rval * plinkvar[DIMSCALE].dval.rval;
	double dMin = dDimasz + txtbox[1][0] / 2;

	double d = fabs(textIns[0] - ptDimL[2][0]);
	if (d < dMin)
	{
		d = dMin;
	}

	if (!SideofTextDrag)
	{
		d = -d;
	}

	// set minimum length
	textIns[0] = ptDimL[2][0] + d;

	d = txtbox[1][0] / 2;
	if (plinkvar[DIMTAD].dval.ival)
	{
		if (bIsLeft)
		{
			d += txtbox[1][0];
			if (!SideofTextDrag)
			{
				d = -d;
			}
		}
		else
		{
			if (SideofTextDrag)
			{
				d = -d;
			}
		}
	}
	else
	{
		// DIMTAD = 0
		if (!bIsLeft)
		{
			d = -d;
		}
	}
	ptDimL[3][0] = textIns[0] + d;

	return TRUE;
}
// ]-SystemMetrix(Hiro) FIX: Dimension line is short when outside text is dragged.

// SystemMetrix(Hiro)-[FIX: When DIMTOH = 1, the outside dimension line is hide.
static
BOOL
ExtendDimLineOutside(
	sds_point 	txtbox[],
	const double	dlnang,
	const BOOL	SideofTextDrag,
	sds_point 	ptDimL[]
)
{
	if ((plinkvar[DIMTAD].dval.ival) ||
		(plinkvar[DIMTVP].dval.rval * plinkvar[DIMSCALE].dval.rval))
	{
		if ((!plinkvar[DIMTOH].dval.ival) ||
			(fabs(dlnang) < CMD_FUZZ) ||
			(fabs(dlnang - IC_PI) < CMD_FUZZ))
		{
			double d = txtbox[1][0];
			if (!SideofTextDrag)
			{
				d = -d;
			}
			ptDimL[3][0] += d;
		}
	}
	return TRUE;
}
// ]-SystemMetrix(Hiro) FIX: When DIMTOH = 1, the outside dimension line is hide.

// ============================================================================================================================================================
// ========================================================= LINEAR FUNCTIONS =================================================================================
// ============================================================================================================================================================
//
// This area has only the functions that create the linear dimensions,(DIMLINEAR,DIMALIGNED,DIM - HOR,DIM - VER,DIM - ALIGNED,
// DIM - ROTATED).
//
// ============================================================================================================================================================

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This funcion is called when a (Horizontal,Vertical,Rotated, or Aligned) dimension is being drawn.
//
// plink :
// flp		 :
// dragmode:
//
// RETURN : TRUE	If successful.
//			FALSE If error.
//
BOOL cmd_defaultdim(struct cmd_dimeparlink *plink,db_drawing *flp,short dragmode) {
	char			 text[IC_ACADBUF] = ""/*DNT*/,
					*fcp1 = NULL;
	short			 fi1 = 0,
					 fi2 = 0;
	sds_point		 ptExtL[6]={{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0}},
					 ptDimL[10]={{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0},
								 {0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0}},
					 ptt[6]={{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0}},
					 boxrect[4] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}},
					 textIns={0.0,0.0,0.0},
					 txtbox[2] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}},
					 ExtDimInt[2] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}},
					 MidOfBoxSides[2] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}},
					 pt10 = {0.0, 0.0, 0.0},
					 pt13 = {0.0, 0.0, 0.0},
					 pt14 = {0.0, 0.0, 0.0},
					 pt11 = {0.0, 0.0, 0.0},
					 ipt = {0.0, 0.0, 0.0};
	sds_real		 cosofdeg = 0.0,
					 sinofdeg = 0.0,
					 cosoftext = 0.0,
					 sinoftext = 0.0,
					 dlnang = 0.0,
					 AvailTwidth = 0.0,
					 Workangle = 0.0,
					 TextXDist = 0.0,
					 TextRotation = 0.0,
					 linearangle = 0.0,
					 fr1 = 0.0,
					 fr2 = 0.0,
					 fr3 = 0.0,
					 InterDist = 0.0;
	//Bugzilla No. 78113 ; 10-06-2002 [
	sds_real		 dlnang2 = 0.0;
	sds_real		 initialrot = plink->textrot;
	//Bugzilla No. 78113 ; 10-06-2002 ]
	// EBATECH(watanabe)-[
	//struct resbuf *rbb=NULL,
	//				 rb;
	struct resbuf *rbb=NULL;
	// ]-EBATECH(watanabe)
	db_handitem 	*handitem=NULL;
	BOOL			 ArrowFit 	 =TRUE,
					 TextFit		 =TRUE,
					 MakeCenterLine=FALSE,
					 MakeExteraLine1=FALSE,
					 MakeExteraLine2=FALSE,
					 FlipDimXaxis  =FALSE,
					 dimlinflag	 =FALSE,
					 dimsoxdflag	 =FALSE,
					 dimDim2flag	 =TRUE,
					 Drag11point	 =TRUE,
					 SideofTextDrag=TRUE,
					 // EBATECH(watanabe)-[FIX: leader length and text position
					 dimtad4Flg	 =FALSE,
					 moveTexgFlg	 =FALSE,
					 // ]-EBATECH(watanabe)
					 dimdleside1	 =FALSE,
					 dimdleside2	 =FALSE;

	// EBATECH(watanabe)-[
	int 	 iDimtad	 = plinkvar[DIMTAD].dval.ival;
	int 	 iDimtih	 = plinkvar[DIMTIH].dval.ival;
	int 	 iDimtoh	 = plinkvar[DIMTOH].dval.ival;
	double dDimscale = plinkvar[DIMSCALE].dval.rval;
	double dDimtvp	 = plinkvar[DIMTVP].dval.rval * dDimscale;
	double dDimgap	 = plinkvar[DIMGAP].dval.rval * dDimscale;
	double dDimtxt	=  plinkvar[DIMTXT].dval.rval * dDimscale;
	// ]-EBATECH(watanabe)
	// EBATECH(CNBR) -[ 13-03-2003 Add more helper variables.
	double dDimasz	=  plinkvar[DIMASZ].dval.rval * dDimscale;
	double dDimdle	=  plinkvar[DIMDLE].dval.rval * dDimscale;
	double dDimexe	=  plinkvar[DIMEXE].dval.rval * dDimscale;
	double dDimexo	=  plinkvar[DIMEXO].dval.rval * dDimscale;
	// ]- EBATECH(CNBR)
	//Modified Cybage SBD 25/02/2002 DD/MM/YYYY [
	//Reason : RollBack Change for Bug No. 77758 from Bugzilla 
	//extern bool InitializeTextRot;
	
	//Added Cybage VSB 13/11/2001 DD/MM/YYYY
	//Reason : Fix for Bug No.77929 from Bugzilla [
	//if(InitializeTextRot==TRUE ){
	//if(InitializeTextRot==TRUE && SDS_AtNodeDrag==0){
	//Added Cybage VSB 13/11/2001 DD/MM/YYYY ]
	//	plink->textrot=0.0;
	//}
	//InitializeTextRot=FALSE;
	
	if(!icadRealEqual(plink->textrot,0.0))		
		plink->txtangflag = TRUE;
	//else if(plink->textrot == 0.0)
	//	plink->txtangflag = FALSE;
	//Modified Cybage SBD 25/02/2002 DD/MM/YYYY ]


	// Given certain conditions (DIMDLE) will extend the dimension line beyond the extension lines (DIMDLE) distance.
	if (!plinkvar[DIMSAH].dval.ival && plinkvar[DIMBLK].dval.cval && *plinkvar[DIMBLK].dval.cval) {
		// EBATECH(watanabe)-[check valid blockname of arrow
		//if ((fcp1=strchr(plinkvar[DIMBLK].dval.cval,'_'/*DNT*/))==NULL) fcp1=plinkvar[DIMBLK].dval.cval; else fcp1=(fcp1+1);
		//if (strisame(plinkvar[DIMBLK].dval.cval,DIM_OBLIQUE) || strisame(plinkvar[DIMBLK].dval.cval,DIM_ARCH_TICK) ||
		//		strisame(plinkvar[DIMBLK].dval.cval,DIM_INTEGRAL) || strisame(plinkvar[DIMBLK].dval.cval,DIM_NONE)) {
		if (IsValidBlockName(DIMBLK)) {
		// ]-EBATECH(watanabe)
			 dimdleside1	 =TRUE;
			 dimdleside2	 =TRUE;
		}
	} else if (plinkvar[DIMSAH].dval.ival) {
		if (plinkvar[DIMBLK1].dval.cval && *plinkvar[DIMBLK1].dval.cval) {
			// EBATECH(watanabe)-[check valid blockname of arrow
			//if ((fcp1=strchr(plinkvar[DIMBLK1].dval.cval,'_'/*DNT*/))==NULL) fcp1=plinkvar[DIMBLK1].dval.cval; else fcp1=(fcp1+1);
			//if (strisame(plinkvar[DIMBLK1].dval.cval,DIM_OBLIQUE) || strisame(plinkvar[DIMBLK1].dval.cval,DIM_ARCH_TICK) ||
			//		strisame(plinkvar[DIMBLK1].dval.cval,DIM_INTEGRAL) || strisame(plinkvar[DIMBLK1].dval.cval,DIM_NONE)) {
			if (IsValidBlockName(DIMBLK1)) {
			// ]-EBATECH(watanabe)
				 dimdleside1	 =TRUE;
			}
		}
		if (plinkvar[DIMBLK2].dval.cval && *plinkvar[DIMBLK2].dval.cval) {
			// EBATECH(watanabe)-[check valid blockname of arrow
			//if ((fcp1=strchr(plinkvar[DIMBLK2].dval.cval,'_'/*DNT*/))==NULL) fcp1=plinkvar[DIMBLK2].dval.cval; else fcp1=(fcp1+1);
			//if (strisame(plinkvar[DIMBLK2].dval.cval,DIM_OBLIQUE) || strisame(plinkvar[DIMBLK2].dval.cval,DIM_ARCH_TICK) ||
			//		strisame(plinkvar[DIMBLK2].dval.cval,DIM_INTEGRAL) || strisame(plinkvar[DIMBLK2].dval.cval,DIM_NONE)) {
			if (IsValidBlockName(DIMBLK2)) {
			// ]-EBATECH(watanabe)
				 dimdleside2	 =TRUE;
			}
		}
	} else if (!icadRealEqual(plinkvar[DIMTSZ].dval.rval, 0.0)) {
		 dimdleside1	 =TRUE;
		 dimdleside2	 =TRUE;
	}
	//	(end of DIMDLE) check.

	//Bugzilla No. 78113 ; 10-06-2002 [
	//if (plink->flags & DIM_ALIGNED) dlnang=plink->dlnang=sds_angle(plink->pt13,plink->pt14); else dlnang=(plink->dlnang);
	if (plink->flags & DIM_ALIGNED) 
		dlnang = dlnang2 = plink->dlnang = sds_angle(plink->pt13,plink->pt14); 
	else 
		dlnang = dlnang2 = plink->dlnang;
	//Bugzilla No. 78113 ; 10-06-2002 ]

	// EBATECH(CNBR) -[ Bugzilla#78455 13-03-2003 Support DIMTMOVE
	if((plink->HomeText || !plinkvar[DIMTMOVE].dval.ival ) &&
		plinkvar[DIMJUST].dval.ival!=3 && plinkvar[DIMJUST].dval.ival!=4 && plinkvar[DIMUPT].dval.ival)
		ic_ptcpy(plink->textmidpt,plink->defpt);
	//if (plinkvar[DIMJUST].dval.ival!=3 && plinkvar[DIMJUST].dval.ival!=4 && plinkvar[DIMUPT].dval.ival) ic_ptcpy(plink->textmidpt,plink->defpt);
	// EBATECH(CNBR) ]-

	// EBATECH(CNBR) -[ Bugzilla#78182 2003/3/10 Set wrong DIMENSION DXF=70 Value
	if (SDS_AtNodeDrag==2 || (plinkvar[DIMUPT].dval.ival && plinkvar[DIMJUST].dval.ival!=3 && plinkvar[DIMJUST].dval.ival!=4))
		plink->flags |= DIM_TEXT_MOVED;
	//if ((SDS_AtNodeDrag==2 || (plinkvar[DIMUPT].dval.ival && plinkvar[DIMJUST].dval.ival!=3 && plinkvar[DIMJUST].dval.ival!=4)) &&
	//	((plink->flags & DIM_TEXT_MOVED)!=DIM_TEXT_MOVED)) plink->flags^=DIM_TEXT_MOVED;
	// EBATECH(CNBR) ]-

	// This do loop is for the (DIMLINEAR) command.
	do {
		// Save the angle to Workangle so we don't have to modify the dlnang angle.
		Workangle=dlnang;

		// The way this works is by changing the dimension points to an angle that will always be in the first (and/or) fourth quadrant.
		// Sometimes the way you pick the first and second points and the angle, will not transform the UCS cord. to DIM cord. only in the first
		// (and/or) second quadrant, if the 14[0] is in the seconed or third quadrant after doing the transformation subtract 180 deg.
		// from the angle. After the points are transformed to DIM cord. system the points will only be in the first (and/or) fourth quadrant
		// so the math on it will be simple, then transform the points back to the regular UCS cord system.
    //           |
    //           |
    //           |    I QUAD
    //           |
    //           |
    // ----------.----------
    //           |\____ This is the 13 or ptd[0] point.
    //           |
    //           |    IV QUAD
    //           |
    //           |
    //
		while (TRUE) {
			cosofdeg=cos(Workangle); sinofdeg=sin(Workangle);
			//Bugzilla No. 78110 ; 17-06-2002 [
			if(icadRealEqual(cosofdeg,0.0))
				cosofdeg = 0.0;
			if(icadRealEqual(sinofdeg,0.0))
				sinofdeg = 0.0;
			//Bugzilla No. 78110 ; 17-06-2002 ]
			// Transform the 14 point from UCS to DIM cord. system.
			cmd_ucstodim(plink->pt13,plink->pt14,pt14,sinofdeg,cosofdeg);
			if (pt14[0]<0.0) Workangle+=IC_PI; else break;
		}
		// Transform all points other then PT13 to a new UCS that I can work with.
		ic_ptcpy(pt13,dimzeropt);
		cmd_ucstodim(plink->pt13,plink->defpt,pt10,sinofdeg,cosofdeg);
		cmd_ucstodim(plink->pt13,plink->pt14,pt14,sinofdeg,cosofdeg);
		cmd_ucstodim(plink->pt13,plink->textmidpt,pt11,sinofdeg,cosofdeg);
//			if (plink->dimmode && pt13[0]<CMD_FUZZ && pt14[0]<CMD_FUZZ) return(FALSE);
		// Make sure that the height of the dimension is the same as the text midpoint if the text is being dragged.
		// EBATECH(CNBR) -[ Bugzilla#78455 13-03-2003 Support DIMTMOVE
		if(( plink->HomeText || !plinkvar[DIMTMOVE].dval.ival ) && 
			(SDS_AtNodeDrag==2 || (plinkvar[DIMUPT].dval.ival && plinkvar[DIMJUST].dval.ival!=3 && plinkvar[DIMJUST].dval.ival!=4)))
		//if (SDS_AtNodeDrag==2 || (plinkvar[DIMUPT].dval.ival && plinkvar[DIMJUST].dval.ival!=3 && plinkvar[DIMJUST].dval.ival!=4))
		// EBATECH(CNBR) ]-
		{
			pt10[1]=pt11[1];
			cmd_dimtoucs(plink->pt13,pt10,plink->defpt,sinofdeg,cosofdeg);
		}
		// This flips pt10 and pt14 along the x axis.
		//                                                                        _______ pt10
		//           |                                                      |    /
		//           |                                                      |---.----
		//           |                                                      |       |
		//           |       .____ pt14                                     |       |
		//           |       |                              FLIP            |       |
		// ----------.-------|--                             |    ----------.----------
		//           |\____ This is the 13 or ptd[0] point. \|/             |\____ This is the 13 or ptd[0] point.
		//           |       |                                              |       .
		//           |---.---                                               |        \_______ pt14
		//           |    \_____ pt10                                       |
		//           |                                                      |
		//
		if (pt10[1]<0.0) {
			pt10[1]*=(-1);
			pt14[1]*=(-1);
			pt11[1]*=(-1);
			FlipDimXaxis=TRUE;
		} else FlipDimXaxis=FALSE;
        //          (Y) 0.0
        //  last deg.|  deg  | last deg.
        //  _________|_______.__________.
        //           |       |
        //   90 deg. |  last | 90 deg.
        //           |  deg. |
        // ----------.------------------ (X)
        //           |       |
        //  last deg.|  0.0  | last deg.
        //           |  deg  |
        //           |       |
        //
		if (dimlinflag) break;
		if (!plink->dimmode) {
			if (pt10[0]<0.0) {
				if (pt10[1]<=pt14[1] && pt10[1]>=0.0) {
					linearangle=(IC_PI/2);
						if (!dragmode) plink->dimmode=2;
				}
			} else if (pt10[0]>pt14[0]) {
				if (pt10[1]<=fabs(pt14[1]) && pt10[1]>=0.0) {
					linearangle=(IC_PI/2);
					if (!dragmode) plink->dimmode=2;
				}
			} else if (pt10[1]>pt14[1] || pt10[1]<0.0) {
				linearangle=0.0;
				if (!dragmode) plink->dimmode=1;
			} else break;
			plink->dlnang+=linearangle;
			ic_normang(&plink->dlnang,NULL);
			dlnang=plink->dlnang;
			dimlinflag=TRUE;
		}
	} while(!plink->dimmode);
	// AvailTwidth is the width between the extension lines.and if the 13 point is the center then the 14 point will be the total disatance.
	AvailTwidth=pt14[0];
	// Copy PT14 to ptExtL[2] and ptExtL[3]
	for (fi1=2;fi1<4;++fi1) ic_ptcpy(ptExtL[fi1],pt14);
	// Make sure that the (Y) of both the extension lines are the same as pt10[1].
	ptExtL[1][1]=pt10[1]; ptExtL[3][1]=pt10[1];

	// NEW STUFF.----------------------
	ic_ptcpy(ptExtL[4],ptExtL[0]); ic_ptcpy(ptExtL[5],ptExtL[2]);

    // Set the DIMEXO offset for the beginning of the extension lines.
    //           _______________
    //          |               |
    //          |               |
    //          |               |
    //          .               .]----dimexo is the distance from the point picked to the begining of the extension line.
    //                                This is done width vertical,horizontal,angular, and aligned dimesnions.
	ptExtL[0][1]+=dDimexo;
	ptExtL[2][1]+=(pt10[1]<pt14[1])?-dDimexo:dDimexo;

	// Make sure the points of the dimension lines are set.
	for (fi1=0;fi1<2;++fi1) ic_ptcpy(ptDimL[fi1],ptExtL[1]);
	for (fi1=2;fi1<4;++fi1) ic_ptcpy(ptDimL[fi1],ptExtL[3]);
	ic_ptcpy(ExtDimInt[1],ptExtL[1]); ic_ptcpy(ExtDimInt[0],ptExtL[3]);
    // Set the DIMEXE offset for the end of the extension lines.
    //
    //          |_______________|]-----dimexe is the distance the dimension line is from the top of the extension line.
    //          |               |      This is done width vertical,horizontal,angular, and aligned dimesnions.
    //          |               |
    //
	ptExtL[1][1]+=dDimexe;
	ptExtL[3][1]+=(pt10[1]<pt14[1])?-dDimexe:dDimexe;

    // NEW STUFF --------------------------------------------------------------
    // Change a linear dimension to a obliqued dimension.
    //       /_________      ________/
    //      /                       /
    //     /                       /
    //    /                       /
    //   /                       /
    //  .                       .
	if (plink->elnang) {
		// This projects the oblique points for the first extension line.
		for (fi1=0;fi1<2;++fi1) {
			fr2=(sds_distance(ptExtL[fi1],ptExtL[4]));
			fr1=((fr2*sin(DIM_90_DEG))/sin(plink->elnang));
			sds_polar(ptExtL[4],plink->elnang,fr1,ptExtL[fi1]);
			fr2=(sds_distance(ptDimL[fi1],ptExtL[4]));
			fr1=((fr2*sin(DIM_90_DEG))/sin(plink->elnang));
			sds_polar(ptExtL[4],plink->elnang,fr1,ptDimL[fi1]);
		}
		ic_ptcpy(ExtDimInt[1],ptDimL[0]);
		// This projects the oblique points for the seconed extension line.
		for (fi1=2;fi1<4;++fi1) {
			fr2=(sds_distance(ptExtL[fi1],ptExtL[5]));
			fr1=((fr2*sin(DIM_90_DEG))/sin(plink->elnang));
			sds_polar(ptExtL[5],plink->elnang,fr1,ptExtL[fi1]);
			fr2=(sds_distance(ptDimL[fi1],ptExtL[5]));
			fr1=((fr2*sin(DIM_90_DEG))/sin(plink->elnang));
			sds_polar(ptExtL[5],plink->elnang,fr1,ptDimL[fi1]);
		}
		ic_ptcpy(ExtDimInt[0],ptDimL[2]);
		ic_ptcpy(pt14,ptDimL[2]);
	}
	// Make sure the text angle is aligned with the dimension line. If DIMTIH=0. Unless the user set the angle of the text (txtangflag)=TRUE.

	//Modified Cybage SBD 25/02/2002 DD/MM/YYYY [
	//Reason : Rollback Change for Bug No. 77758 from Bugzilla 
	if (!(plinkvar[DIMTIH].dval.ival) && !plink->txtangflag) plink->textrot=dlnang;
	//if(!(plinkvar[DIMTIH].dval.ival) && !(plinkvar[DIMTOH].dval.ival) && !plink->txtangflag) 
	//	plink->textrot=dlnang;

	//if(plink->textrot>0.0)
	//	plink->txtangflag=TRUE;
	//Modified Cybage SBD 25/02/2002 DD/MM/YYYY ]

	// Generate string for dimension.
	cmd_genstring(plink,text,AvailTwidth,""/*DNT*/,TRUE,flp);
	// Set the value DIMVALUE to the width of the dimension.
	plink->dimvalue=AvailTwidth;
	// Buld the text insert.

	// EBATECH(watanabe)-[
	if ((rbb=sds_buildlist(RTDXF0,"MTEXT"/*DNT*/,1,text,10, plink->textmidpt, 50,plink->textrot,40,dDimtxt,41,0.0,51,plink->hdir,7,plinkvar[DIMTXSTY].dval.cval,71,5,72,1,0))==NULL) return(RTERROR);
	//if ((rbb=sds_buildlist(RTDXF0,"MTEXT"/*DNT*/,1,text,10, plink->textmidpt, 50,0.0,40,dDimtxt,41,0.0,51,plink->hdir,7,plinkvar[DIMTXSTY].dval.cval,71,5,72,1,0))==NULL) return(RTERROR);
	// get the width and height of the text.
	sds_textbox(rbb,txtbox[0],txtbox[1]);
	// free resbuf link list.
	IC_RELRB(rbb);
	// Caluculate the distance buffer around the text.
	txtbox[1][0]+= fabs(dDimgap) * 2;	
	txtbox[1][1]+= fabs(dDimgap) * 2;

	// EBATECH(watanabe)-[debug DIMGAP
	rbb = sds_buildlist(
				RTDXF0, "MTEXT"/*DNT*/,
				1,		text,
				10, 	plink->textmidpt,
				50, 	0.0,	// always horizontal
				40, 	dDimtxt,
				41, 	0.0,
				51, 	plink->hdir,
				7,		plinkvar[DIMTXSTY].dval.cval,
				71, 	5,
				72, 	1,
				0);
	if (rbb == NULL)
		{
		return(RTERROR);
		}
	// get the width and height of the text.
	sds_point box2[2];
	sds_textbox(rbb, box2[0], box2[1]);

	// free resbuf link list.
	IC_RELRB(rbb);

	// Caluculate the distance buffer around the text.
	box2[1][0] += fabs(dDimgap) * 2;
	box2[1][1] += fabs(dDimgap) * 2;
	// ]-EBATECH(watanabe)

    // Set textIns to the center of the dimesnion line.
    //               __________This is the center of the dimension line, This value is stored in the txtins variable.
    //              /
    //      |______.______|
    //      |             |
    //      |             |
    //
	// EBATECH(CNBR) -[ Bugzilla#78455 13-03-2003 Support DIMTMOVE
	if( plink->HomeText || !plinkvar[DIMTMOVE].dval.ival )
	{
		textIns[1]=pt10[1];
		// If the (pt11) values are not zero then set the (textins) variable to the (pt11) (X) location.
		if ((SDS_AtNodeDrag==2 || (plinkvar[DIMUPT].dval.ival && plinkvar[DIMJUST].dval.ival!=3 && plinkvar[DIMJUST].dval.ival!=4) ||
			(plink->flags&DIM_TEXT_MOVED)) && (plink->textmidpt[0] || plink->textmidpt[1]))
			textIns[0]=pt11[0];
		else if (plink->elnang)
			textIns[0]=ptDimL[0][0]+(sds_distance(ptDimL[0],ptDimL[2])/2);
		else
			textIns[0]=(pt14[0]/2);
	}
	else
	{
		ic_ptcpy(textIns, pt11);
	}
	// EBATECH(CNBR) ]-

	// If DIMTIH is on then make sure that the interior text angle is at the same angle of the dimension.
	// EBATECH(CNBR) -[ Bugzilla#78455 13-03-2003 Support DIMTMOVE
	if( plink->HomeText || !plinkvar[DIMTMOVE].dval.ival )
	{
		if (plink->txtangflag)
			fr2=plink->textrot;
		else if (!plinkvar[DIMTOH].dval.ival && (ptExtL[1][0]>textIns[0] || ptExtL[3][0]<textIns[0]))
			fr2=0.0;
		else
			fr2=(plinkvar[DIMTIH].dval.ival)?(Workangle):0.0;
	}
	else
	{
		if (plink->txtangflag)
			fr2=plink->textrot;
		else if ( ptExtL[1][0]>textIns[0] || ptExtL[3][0]<textIns[0] )
			fr2 = plinkvar[DIMTOH].dval.ival ? Workangle : 0.0;
		else
			fr2 = plinkvar[DIMTIH].dval.ival ? Workangle : 0.0;
	}
	//if (plink->txtangflag)
	//	fr2=plink->textrot;
	//else if (!plinkvar[DIMTOH].dval.ival && (ptExtL[1][0]>textIns[0] || ptExtL[3][0]<textIns[0]))
	//	fr2=0.0;
	//else
	//	fr2=(plinkvar[DIMTIH].dval.ival)?(Workangle):0.0;
	// EBATECH(CNBR) ]-
	// If the text is going to be positioned by (DIMJUST)=1,2.
	// EBATECH(CNBR) -[ Bugzilla#78455 13-03-2003 Support DIMTMOVE
	if( plink->HomeText || !plinkvar[DIMTMOVE].dval.ival )
	{
		if ((plinkvar[DIMJUST].dval.ival==1 || plinkvar[DIMJUST].dval.ival==2) && (SDS_AtNodeDrag!=2 || !plinkvar[DIMUPT].dval.ival))
		{
			// The following code gets the intersecion points ptt[0] and ptt[1] of the dimension line crossing the dimension
			// textbox which sourounds the dimension text, then setting ptDimL[1] to ptt[0] and ptDimL[3] to ptt[1]. Then
			// Get the distance between the two points and set TextXDist to that distance.
			cmd_getboxintersec(txtbox,fr2,textIns,ptDimL[0],ptDimL[2],ptt,boxrect,&fi1,&fi2);
			// Get the middle point of the line on the right side of the box.
			//		boxrect[0]
			//    boxrect[0]
			//  /_______________________/ boxrect[1]
			//  |                       |
			//  |                       |
			//  |_______________________|
			//  \                       \ boxrect[3]
			//   boxrect[2]
			//
			MidOfBoxSides[0][0]=((boxrect[0][0]+boxrect[(fi1)?2:1][0])/2); MidOfBoxSides[0][1]=((boxrect[0][1]+boxrect[(fi1)?2:1][1])/2);
			MidOfBoxSides[1][0]=((boxrect[(fi1)?1:2][0]+boxrect[3][0])/2); MidOfBoxSides[1][1]=((boxrect[(fi1)?1:2][1]+boxrect[3][1])/2);
			// If the text can fit between the extension lines then make sure the text will be left or right justified in the dimension.
			if ((AvailTwidth>((dDimasz*2))) || (!(plinkvar[DIMTIH].dval.ival)/* && AvailTwidth>TextXDist*/))
			{
				switch(plinkvar[DIMJUST].dval.ival)
				{
				case 1:
					textIns[0]=(textIns[0]-MidOfBoxSides[(MidOfBoxSides[1][0]<MidOfBoxSides[0][0])?1:0][0]);
					break;
				case 2:
					textIns[0]=pt14[0]-(MidOfBoxSides[(MidOfBoxSides[1][0]<MidOfBoxSides[0][0])?0:1][0]-textIns[0]);
					break;
				}
			// If the text can not fit between the extension lines then make sure the text will be to the left or the right of the extension lines.
			}
			else
			{
				switch(plinkvar[DIMJUST].dval.ival)
				{
				case 1:
					textIns[0]=-dDimasz;
					break;
				case 2:
					textIns[0]=(pt14[0]+dDimasz);
					break;
				}
			}
		}
	}
	//EBATECH(CNBR)]-
	// Add this so that when the text had been dragged, and currently the other points are being dragged
	// make sure that the text midpoint is not moved.
//		if ((SDS_AtNodeDrag==3 || SDS_AtNodeDrag==4 || SDS_AtNodeDrag==5) && (plink->flags&DIM_TEXT_MOVED)==DIM_TEXT_MOVED)  {
//			cmd_ucstodim(plink->pt13,plink->textmidpt,textIns,sinofdeg,cosofdeg);
//			if (SDS_AtNodeDrag==3) textIns[1]=pt10[1];
//		}

	// EBATECH(CNBR) -[ Bugzilla#78455 13-03-2003 Support DIMTMOVE
	if( !plink->HomeText && plinkvar[DIMTMOVE].dval.ival )
	{
		// Initialize boxrect
		double ang = 0;
		if (iDimtih)
			ang = fr2;
		BOOL b = FALSE;
		cmd_getboxintersec( box2, ang, textIns, ptDimL[0], ptDimL[2], ptt, boxrect, &fi1, &fi2);
		if( fi1 == 3 && fi2 == 3 )
			b = TRUE;
		else // The case of dimension line and textbox is not parallel
			b = cmd_getboxintersecEx( box2, ang, textIns, ptDimL[0], ptDimL[2], ptt );
		// MakeCenterLine=TRUE to make the center line for the dimension line this is never turned off.
		if (plinkvar[DIMTOFL].dval.ival) MakeCenterLine=TRUE;
		// Dimension text is between extension lines.
		// Dimension line across the text box.
		if( ptDimL[0][0] < textIns[0] && textIns[0] < ptDimL[2][0] && b )
		{
			// Switch the points.
			if (ptt[0][0]>ptt[1][0])
			{
				ic_ptcpy(ptt[2],ptt[0]);
				ic_ptcpy(ptt[0],ptt[1]);
				ic_ptcpy(ptt[1],ptt[2]);
			}
			// No enough space to place arrows inside.
			if( AvailTwidth < dDimasz * 2.5 || ptt[0][0] - dDimasz < ptDimL[0][0] || ptt[1][0] + dDimasz > ptDimL[2][0] )
			{
NotEnoughArrowSpaceWhenDimtmoveIsNotZero:
				// Copy center line
				ic_ptcpy(ptDimL[4],ptDimL[0]);
				ic_ptcpy(ptDimL[5],ptDimL[2]);
				// Dimension lines are outside of extension lines.
				ArrowFit = FALSE;
				if (!dimdleside1 )
					ptDimL[0][0]-=dDimasz;
				ic_ptcpy(ptDimL[1],ptDimL[0]);
				if (!dimdleside1 )
					ptDimL[1][0]-=dDimasz;
				else
					ptDimL[1][0]-=dDimdle ;
				if (!dimdleside2 )
					ptDimL[2][0]+=dDimasz;
				ic_ptcpy(ptDimL[3],ptDimL[2]);
				if (!dimdleside2 )
					ptDimL[3][0]+=dDimasz;
				else
					ptDimL[3][0]+=dDimdle ;
			}
			else
			{
				// Copy the points to the (ptDimL) point vars.
				ic_ptcpy(ptDimL[1],ptt[0]);
				ic_ptcpy(ptDimL[3],ptt[1]);
				if(!( plink->flags & DIM_TEXT_MOVED ))
				{
					// If DIMTAD is on and DIMTIH is off or if dimjust is 2 or 4, then make the dimension line extend under the text.
					if (((plinkvar[DIMTAD].dval.ival==1 || (dDimtvp>0.0) ||
						(plinkvar[DIMTAD].dval.ival && !plink->dlnang)) && (!(plinkvar[DIMTIH].dval.ival) || !plink->dlnang)) ||
						(plinkvar[DIMJUST].dval.ival==3 || plinkvar[DIMJUST].dval.ival==4)) {
						ic_ptcpy(ptDimL[1],ptDimL[2]);
						dimDim2flag=FALSE;
					} else if (plinkvar[DIMTAD].dval.ival==2 || plinkvar[DIMTAD].dval.ival==3) {
						if (!icadRealEqual(plink->textrot,0.0)) {
							ic_ptcpy(ptDimL[1],ptDimL[2]);
							dimDim2flag=FALSE;
						}
					}
				}
			}
		}
		else
		{
			ic_ptcpy(ptDimL[1], ptDimL[0]);
			ic_ptcpy(ptDimL[3], ptDimL[2]);
			if( AvailTwidth < dDimasz * 2.5 )
				goto NotEnoughArrowSpaceWhenDimtmoveIsNotZero;
			ic_ptcpy(ptDimL[1],ptDimL[2]);
			dimDim2flag=FALSE;
			// Create leader line
			if( plinkvar[DIMTMOVE].dval.ival == 1 )
			{
				//---------------------------------------------------------------
				// TODO TODO : Create leader line(ptDimL[6 to 9]) without moving dimension text
				//----------------------------------------------------------------
			}
		}
		goto EndOfAdujstmentWhenDimtmoveIsNotZero;
	}

	// This loop is for the dragging of the text midpoint.
	for (;;) {
		// The following code gets the intersecion points ptt[0] and ptt[1] of the dimension line crossing the dimension
		// textbox which sourounds the dimension text, then setting ptDimL[1] to ptt[0] and ptDimL[3] to ptt[1]. Then
		// Get the distance between the two points and set TextXDist to that distance.
            //
            //      __________________ ptDimL[0]
            //     /               ___ ptDimL[2]
            //    /    ______     /
            //  |/____| TEXT |__|/
            //  |     /------\  |
            //  |    /        \ |
            //  |    ptt[0]    ptt[1] or ptt[0]
            //  |      or       |
            //  |    ptt[1]     |
            //

		// EBATECH(watanabe)-[debug DIMGAP
		//fi2=cmd_getboxintersec(txtbox,fr2,textIns,ptDimL[0],ptDimL[2],ptt,boxrect,&fi1,&fi2);
		double ang = 0;
		if (iDimtih)
			{
			ang = fr2;
			}
		/*fi2 = */cmd_getboxintersec(
					box2,
					ang,
					textIns,
					ptDimL[0],
					ptDimL[2],
					ptt, boxrect, &fi1, &fi2);
		// ]-EBATECH(watanabe)

		// Switch the points.
		if (ptt[0][0]>ptt[1][0]) { ic_ptcpy(ptt[2],ptt[0]); ic_ptcpy(ptt[0],ptt[1]); ic_ptcpy(ptt[1],ptt[2]); }

		// If dimension line crossed the text box through both lines.
		if (fi1==3 && fi2==3) {
			TextXDist=sds_distance(ptt[0],ptt[1]);
		} else {
			TextXDist=sds_distance(ptt[0],ptt[1]);
		}

		// If both intersections happend on the left side of the (Y) axis. Make sure the points are ajusted correctly.
		if (ptt[0][0]<ptExtL[1][0] && ptt[1][0]<ptExtL[1][0]) {
			// Copy the points to the (ptDimL) point vars.
			ic_ptcpy(ptDimL[1],ptDimL[0]); ic_ptcpy(ptDimL[3],ptDimL[2]);
		} else {
			// Copy the points to the (ptDimL) point vars.
			ic_ptcpy(ptDimL[1],ptt[0]); ic_ptcpy(ptDimL[3],ptt[1]);
		}
		if (Drag11point) {
			// The following is checking wether the text and arrows can fit in between the extension lines.
			// dDimasz			 = Arrow width.
			// TextXDist		 = The width of the text with the DIMGAP spacing.
			// AvailTwidth		 = Total width between the extension lines.
	            //
	            //        ___________________ AvailTWidth
	            //       |               |
	            //
	            //       |_______________|
	            //       |               |
	            //       |               |
	            //
			if (AvailTwidth>((dDimasz*2)+TextXDist)) {
				ptDimL[0][0]+=dDimasz;
				ptDimL[2][0]-=dDimasz;
			} else if (((plinkvar[DIMTAD].dval.ival!=2 &&	
				!(plinkvar[DIMTIH].dval.ival)) || plinkvar[DIMFIT].dval.ival==1) && AvailTwidth>TextXDist) {	// EBATECH(CNBR) DIMFIT==1 or DIMATFIT==1
				ArrowFit=FALSE;
			} else if (AvailTwidth>((dDimasz*2)+(dDimasz/2)) &&
				(plinkvar[DIMJUST].dval.ival!=1 && plinkvar[DIMJUST].dval.ival!=2)) {
				if (!(plinkvar[DIMTIX].dval.ival)) {
					if (plinkvar[DIMFIT].dval.ival>=3 && AvailTwidth>TextXDist) // EBATECH(CNBR) DIMFIT>=3 or DIMATFIT==3
					//if (plinkvar[DIMFIT].dval.ival==3 && AvailTwidth>TextXDist)
					{
						ArrowFit=FALSE;
					} else {
						TextFit=FALSE;
					}
				} else {
					ArrowFit=FALSE;
				}
			} else {
				ArrowFit=FALSE;
				TextFit=FALSE;
			}
		}
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// This section does most of the work when someone is draging the dimension text.
		//
		// Get the distance between the insertion point and intersection of the dimension line and the side of the text box.
		InterDist=sds_distance(ptt[0],textIns);
		// If the text midpoint is being dragged.
		if ((Drag11point && SDS_AtNodeDrag && (plink->flags&DIM_TEXT_MOVED)==DIM_TEXT_MOVED) ||
			(Drag11point && (SDS_AtNodeDrag==2 || (plinkvar[DIMUPT].dval.ival && plinkvar[DIMJUST].dval.ival!=3 && plinkvar[DIMJUST].dval.ival!=4))) ||
			((plinkvar[DIMJUST].dval.ival==1 || plinkvar[DIMJUST].dval.ival==2) && Drag11point)) {
			// If the left side of the text is to the left of the 13 point dimension line.
			if (ptt[((FlipDimXaxis)?1:0)][0]<=ptExtL[1][0]) {
				SideofTextDrag=FALSE;
				// If the text midpoint is to the right of the 13 point dimension line.
				if (textIns[0]>ptExtL[1][0]) {
					// If the text is being dragged between the dimension lines then make sure to place the text in the middle of the extension lines
					if (!TextFit) {
						textIns[0]=pt14[0]/2; TextFit=TRUE;
					} else {
						textIns[0]=(InterDist+dDimasz+(dDimasz/4));
					}
				// If the text midpoint is right on the 13 point dimension line.
				} else if (icadRealEqual(textIns[0],ptExtL[1][0])) {
					textIns[0]=pt14[0]+dDimasz+InterDist; TextFit=FALSE;
				// If the text midpoint is to the left of the 13 point dimension line.
				} else if (textIns[0]<ptExtL[1][0] && ptt[1][0]>=-dDimasz) {
					if (!(plinkvar[DIMTOH].dval.ival) || icadRealEqual(plink->dlnang,0.0)) {
						textIns[0]=-(dDimasz+InterDist);
					} else {
						textIns[0]=-(dDimasz+(txtbox[1][0]/2));
					}
					TextFit=FALSE;
				} else TextFit=FALSE;
				Drag11point=FALSE;
			// If the right side of the text is to the right of the 14 point dimension line.
			} else if (ptt[((FlipDimXaxis)?0:1)][0]>=pt14[0]) {
				// If the text midpoint is to the left of the 14 point dimension line.
				if (textIns[0]<pt14[0]) {
					// If the text is being dragged between the dimension lines then make sure to place the text in the middle of the extension lines
					if (!TextFit) {
						textIns[0]=pt14[0]/2; TextFit=TRUE;
					} else {
						textIns[0]=pt14[0]-(InterDist+dDimasz+(dDimasz/4));
					}
				} else if (textIns[0]<pt14[0] && ptt[1][0]>=(pt14[0]+dDimasz)) {
					textIns[0]=pt14[0]+dDimasz+InterDist; TextFit=FALSE;
				} else if (textIns[0]>=pt14[0] && ptt[((FlipDimXaxis)?1:0)][0]<=(pt14[0]+dDimasz)) {
					if (!(plinkvar[DIMTOH].dval.ival) || icadRealEqual(plink->dlnang,0.0)) {
						textIns[0]=pt14[0]+dDimasz+InterDist;
					} else {
						textIns[0]=pt14[0]+dDimasz+(txtbox[1][0]/2);
					}
					TextFit=FALSE;
				} else TextFit=FALSE;
				Drag11point=FALSE;
			} else {
				if (!TextFit) TextFit=TRUE;
				break;
			}
		} else break;
		// End of section.
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	}
	
	// Add this so that when the text had been dragged, and currently the other points are being dragged
	// make sure that the text midpoint is not moved.
	if ((SDS_AtNodeDrag==3 || SDS_AtNodeDrag==4 || SDS_AtNodeDrag==5) && (plink->flags&DIM_TEXT_MOVED)==DIM_TEXT_MOVED)
	{
		cmd_ucstodim(plink->pt13,plink->textmidpt,textIns,sinofdeg,cosofdeg);
		if (FlipDimXaxis) textIns[1]*=(-1);
		if (SDS_AtNodeDrag==3) textIns[1]=pt10[1];
	}

// TEST AREA /////////////////////////////////////////////////////////////////////////////////
//if (RTNORM!=cmd_dimaddline(layername,1,ptExtL[0],ptExtL[1],flp,dragmode)) goto exit;
//if (RTNORM!=cmd_dimaddline(layername,2,ptExtL[2],ptExtL[3],flp,dragmode)) goto exit;
//if (RTNORM!=cmd_dimaddline(layername,3,ptDimL[0],ptDimL[1],flp,dragmode)) goto exit;
//if (RTNORM!=cmd_dimaddline(layername,4,ptDimL[2],ptDimL[3],flp,dragmode)) goto exit;
//ic_ptcpy(ptt[0],dimzeropt); ptt[0][1]=30.0; ic_ptcpy(ptt[1],dimzeropt); ptt[1][0]=-30.0;
//if (RTNORM!=cmd_dimaddline(layername,5,dimzeropt,ptt[0],flp,dragmode)) goto exit;
//if (RTNORM!=cmd_dimaddline(layername,5,dimzeropt,ptt[1],flp,dragmode)) goto exit;
//ic_ptcpy(ptt[0],dimzeropt); ptt[0][1]=-30.0;
//if (RTNORM!=cmd_dimaddline(layername,5,dimzeropt,ptt[0],flp,dragmode)) goto exit;
//if (RTNORM!=cmd_dimaddmtext(layername,plinkvar[DIMCLRT].dval.ival, textIns, dDimtxt, text, TextRotation, 1.0, plinkvar[DIMSTYLE].dval.coval, 2, plink->pt210, flp, dragmode)) goto exit;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	// This section creates the correct positioning of the arrows, dimension lines and text based on the DIM variables and the point placment of the
	// dimension.
	//
	//
	//
	if (!ArrowFit && !TextFit) 
	{
        // This code does this exact type of dimension.
        //  ________ptDimL[1]
        // |  ______ptDimL[0]
        // | |     _ptDimL[4]
        // | |    |________________ptExtL[1]
        // | |   /|          ______ptDimL[2]
        // | |  //          |
        // |_|\|/_________|/|_TEXT
        //    /|         /|\  |_____ptDimL[3]
        //     |        / |
        //     | ptDimL[5]|
        //     |          |
        //
		ic_ptcpy(ptDimL[4],ptDimL[0]);
		ic_ptcpy(ptDimL[5],ptDimL[2]);
		if (textIns[0]<ptExtL[1][0]) {
			// Switch (ptDimL[2]) and (ptDimL[0]).
			ic_ptcpy(ptt[0],ptDimL[2]);
			ic_ptcpy(ptDimL[2],ptDimL[0]);
			ic_ptcpy(ptDimL[0],ptt[0]);
			// Only if DIMTSZ is > 0.0
			if (!dimdleside1 /*plinkvar[DIMTSZ].dval.rval*/) ptDimL[0][0]+=dDimasz;
			ic_ptcpy(ptDimL[1],ptDimL[0]);
			// Only if DIMTSZ is > 0.0
			if (!dimdleside1 /*plinkvar[DIMTSZ].dval.rval*/) ptDimL[1][0]+=dDimasz; else ptDimL[1][0]+=dDimdle;
			// Only if DIMTSZ is > 0.0
			if (!dimdleside2 /*plinkvar[DIMTSZ].dval.rval*/) ptDimL[2][0]-=dDimasz;
			ic_ptcpy(ptDimL[3],ptDimL[2]);
			// Only if DIMTSZ is > 0.0
			if (!dimdleside2 /*plinkvar[DIMTSZ].dval.rval*/) ptDimL[3][0]-=dDimasz; else ptDimL[3][0]-=dDimdle ;
		} else {
			// Only if DIMTSZ is > 0.0
			if (!dimdleside1 /*plinkvar[DIMTSZ].dval.rval*/) ptDimL[0][0]-=dDimasz;
			ic_ptcpy(ptDimL[1],ptDimL[0]);
			// Only if DIMTSZ is > 0.0
			if (!dimdleside1 /*plinkvar[DIMTSZ].dval.rval*/) ptDimL[1][0]-=dDimasz; else ptDimL[1][0]-=dDimdle ;
			// Only if DIMTSZ is > 0.0
			if (!dimdleside2 /*plinkvar[DIMTSZ].dval.rval*/) ptDimL[2][0]+=dDimasz;
			ic_ptcpy(ptDimL[3],ptDimL[2]);
			// Only if DIMTSZ is > 0.0
			if (!dimdleside2 /*plinkvar[DIMTSZ].dval.rval*/) ptDimL[3][0]+=dDimasz; else ptDimL[3][0]+=dDimdle ;
		}
		// MakeCenterLine=TRUE to make the center line for the dimension line this is never turned off.
		if (plinkvar[DIMTOFL].dval.ival) MakeCenterLine=TRUE;
		// If DIMTIX is on, don't move the text outside of the extension lines.
		if (!(plinkvar[DIMTIX].dval.ival)) {
			// If DIMTOH is on the make text horizontal outside of the extension lines.

			// EBATECH(watanabe)-[FIX: leader length and text position
			//if (!(plinkvar[DIMTOH].dval.ival) || !plink->dlnang) {
			if (!(plinkvar[DIMTOH].dval.ival)) {
			// ]-EBATECH(watanabe)

				// THIS IS NEW STUFF ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
				if (plinkvar[DIMFIT].dval.ival==4) {

					// EBATECH(watanabe)-[FIX: leader length and text position
					//ptDimL[6][0]=((ptDimL[4][0]+ptDimL[5][0])/2);
					//ptDimL[6][1]=((ptDimL[4][1]+ptDimL[5][1])/2);
					//fr2=plinkvar[DIMEXE].dval.rval+plinkvar[DIMDLI].dval.rval+plinkvar[DIMEXO].dval.rval;
					//sds_polar(ptDimL[6],((DIM_90_DEG/2)+(DIM_90_DEG/3)),fr2,ptDimL[7]);
					//for (fi1=8;fi1<10;++fi1) ic_ptcpy(ptDimL[fi1],ptDimL[7]);
					//ptDimL[9][0]+=dDimasz;
					//ic_ptcpy(textIns,ptDimL[9]);
					//textIns[0]+=(txtbox[1][0]/2);
					CreateDimfitLeader1(ptDimL, textIns, box2, FALSE);
					// ]-EBATECH(watanabe)

					MakeExteraLine1=TRUE;
					MakeExteraLine2=TRUE;
					MakeCenterLine=TRUE;
				// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
				} else {
					if (!Drag11point) {
						if (textIns[0]<ptExtL[1][0]) {

							// SystemMetrix(Hiro)-[FIX: Dimension line is short when outside text is dragged.
							//ptDimL[3][0]=textIns[0]+InterDist;
							ExtendDimLine(textIns, txtbox, InterDist, SideofTextDrag, ptDimL);
							// ]-SystemMetrix(Hiro) FIX: Dimension line is short when outside text is dragged.

						} else {
							if (!FlipDimXaxis && !SideofTextDrag) ptDimL[2][0]+=dDimasz;

							// SystemMetrix(Hiro)-[FIX: Dimension line is short when outside text is dragged.
							//ptDimL[3][0]=textIns[0]-InterDist;
							ExtendDimLine(textIns, txtbox, -InterDist, SideofTextDrag, ptDimL);
							// ]-SystemMetrix(Hiro) FIX: Dimension line is short when outside text is dragged.

						}
					} else {
						// Move the text insertion point to the new position.
						textIns[0]=ptDimL[3][0]+(txtbox[1][0]/2);
						// If DIMTAD is on then make the dimension line extend under the text.
						if (plinkvar[DIMTAD].dval.ival || dDimtvp) ptDimL[3][0]+=txtbox[1][0];
					}
				}
				// Set plink->textrot to dlnang so that the text is aligned with the dimension when the text is outside of the dimension.
				// Unless the user set the angle of the text (txtangflag)=TRUE.
				if (!plink->txtangflag) plink->textrot=plink->dlnang;
			} else {
				fr2=plink->dlnang;

				// THIS IS NEW STUFF ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
				if (plinkvar[DIMFIT].dval.ival==4) {

					// EBATECH(watanabe)-[FIX: leader length and text position
					//fr3=((DIM_90_DEG/2)+(DIM_90_DEG/3));
					//cmd_DimCAngle_Fit4(FlipDimXaxis,SideofTextDrag,&fr3,fr2);
					//
					//ptDimL[6][0]=((ptDimL[4][0]+ptDimL[5][0])/2);
					//ptDimL[6][1]=((ptDimL[4][1]+ptDimL[5][1])/2);
					//fr1=dDimexe+dDimdli+dDimexo;
					//sds_polar(ptDimL[6],fr3,fr1,ptDimL[7]);
					//
					//ic_ptcpy(ptDimL[8],ptDimL[7]);
					//
					//fr1=dDimasz;
					//if (Drag11point) {
					//		cmd_DimCAngle_FitText(FlipDimXaxis,SideofTextDrag,&fr3,fr2);
					//
					//		sds_polar(ptDimL[8],fr3,fr1,ptDimL[9]);
					//
					//		fr1+=(txtbox[1][0]/2);
					//		sds_polar(ptDimL[8],fr3,fr1,textIns);
					//}
					CreateDimfitLeader2(ptDimL, textIns, box2, txtbox, FlipDimXaxis, SideofTextDrag, Drag11point, Workangle, FALSE);
					dimtad4Flg = TRUE;
					// ]-EBATECH(watanabe)

					MakeExteraLine1=TRUE;
					MakeExteraLine2=TRUE;
					MakeCenterLine=TRUE;
				// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
				} else {
					cmd_DimCalcAngle(FlipDimXaxis,SideofTextDrag,&fr2);
					if (plink->dlnang) {
						//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						// This moves the text insertion point, so that the dimension line does not have to move this is done when moveing the
						// 13,10 or 14 points. not when draging the dimension text.
						if (Drag11point) {
							ic_ptcpy(ptDimL[6],ptDimL[3]);
							if (!plinkvar[DIMTAD].dval.ival&& !dDimtvp) {
								sds_polar(ptDimL[6],(fr2+IC_PI),dDimasz,ptDimL[7]);
								sds_polar(ptDimL[6],(fr2+IC_PI),((txtbox[1][0]/2)+dDimasz),textIns);
							} else {
								sds_polar(ptDimL[6],(fr2+IC_PI),txtbox[1][0],ptDimL[7]);
								sds_polar(ptDimL[6],(fr2+IC_PI),(txtbox[1][0]/2),textIns);
							}
						} else {
							sds_polar(textIns,fr2,(txtbox[1][0]/2),ptDimL[7]);
							sds_polar(textIns,fr2,((txtbox[1][0]/2)+dDimasz),ptDimL[6]);
							ic_ptcpy(ptDimL[3],ptDimL[6]);
						}
						// Update the (Y) for the dimension lines stuff.
						for (fi1=0;fi1<6;++fi1) ptDimL[fi1][1]=ptDimL[6][1];
						// Update the (Y) for the Extension line dimension line intersection points.
						ExtDimInt[0][1]=ptDimL[6][1]; ExtDimInt[1][1]=ptDimL[6][1];
						// Update the (Y) for the extension lines.
						ptExtL[1][1]=ptDimL[6][1]+dDimexo;
						ptExtL[3][1]=ptDimL[6][1]+((pt10[1]<pt14[1])?-dDimexo:dDimexo);
						// Update the (Y) for the 10 point.
						pt10[1]=ptDimL[6][1];
						if (fr2 && (plinkvar[DIMJUST].dval.ival!=3 && plinkvar[DIMJUST].dval.ival!=4)) MakeExteraLine1=TRUE;
					} else {
						// Move the text insertion point to the new position.
						textIns[0]=ptDimL[3][0]+(txtbox[1][0]/2);
					}

					// SystemMetrix(Hiro)-[FIX: When DIMTOH = 1, the outside dimension line is hide.
					ExtendDimLineOutside(txtbox, dlnang, SideofTextDrag, ptDimL);
					// ]-SystemMetrix(Hiro) FIX: When DIMTOH = 1, the outside dimension line is hide.
				}
				// If we are here then the dimension text is outside of the extension lines and DIMTOH=1 Draws text horizontally.
				// Unless the user set the angle of the text (txtangflag)=TRUE.
				if (!plink->txtangflag) plink->textrot=0.0;
			}
		} else if (plinkvar[DIMSOXD].dval.ival) {
			dimsoxdflag=TRUE;
		}
	} 
	else if ((!TextFit && (plinkvar[DIMJUST].dval.ival!=3 && plinkvar[DIMJUST].dval.ival!=4)) ||
		(textIns[0]<ptExtL[1][0] || textIns[0]>pt14[0])) 
	{
		// This code does this exact type of dimension.
		//
		// ptDimL[0]	 ______ptDimL[2]
		//		| 		|
		//	|/|_______\||_TEXT
		//	|\		 //|	|_____ptDimL[3]
		//	|		/  |
		//	| ptDimL[1]|
		//	|			 |
		//
		// Make sure that the ptDimL[0] and ptDimL[1] are at the extension lines.
		ptDimL[0][0]=ptExtL[1][0]; ptDimL[1][0]=pt14[0];
		// If the textIns(X) value is less then 0.0 this means that the text is being dragged to the left.
		if (textIns[0]<ptExtL[1][0]) {
			ic_ptcpy(ptDimL[2],ptDimL[0]);
		} else {
			ic_ptcpy(ptDimL[3],ptDimL[2]);
		}
		// Only if DIMTSZ is > 0.0
		if (!dimdleside1 && !dimdleside2 /*!plinkvar[DIMTSZ].dval.rval*/) {
			ptDimL[0][0]+=dDimasz;
			ptDimL[1][0]-=dDimasz;

			// EBATECH(watanabe)-[FIX: leader length and text position
			//ptDimL[3][0]+=dDimasz;
			if (plinkvar[DIMFIT].dval.ival != 4) {
				ptDimL[3][0] += dDimasz;
			}
			// ]-EBATECH(watanabe)

		} else {
			ptDimL[0][0]-=dDimdle ;
			ptDimL[3][0]+=dDimdle ;
		}
		// Get the distance between the insertion point and the new offset point.
		fr1=(ptDimL[3][0]-textIns[0]);
		// Move the text insertion point to the new postion.
		if (Drag11point) textIns[0]=ptDimL[3][0]+(txtbox[1][0]/2);
		// If DIMTOH is on the make text horizontal outside of the extension lines.

		//BugZilla No. 78236; 08-07-2002 [	
		// EBATECH(watanabe)-[FIX: leader length and text position
		if (!(plinkvar[DIMTOH].dval.ival) || !plink->dlnang) {
		//if (!(plinkvar[DIMTOH].dval.ival)) {
		// ]-EBATECH(watanabe)
		//BugZilla No. 78236; 08-07-2002 ]

			// THIS IS NEW STUFF ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			if (plinkvar[DIMFIT].dval.ival==4) {

				// EBATECH(watanabe)-[FIX: leader length and text position
				//ptDimL[6][0]=((ptDimL[0][0]+ptDimL[1][0])/2);
				//ptDimL[6][1]=((ptDimL[0][1]+ptDimL[1][1])/2);
				//fr2=plinkvar[DIMEXE].dval.rval+plinkvar[DIMDLI].dval.rval+plinkvar[DIMEXO].dval.rval;
				//sds_polar(ptDimL[6],((DIM_90_DEG/2)+(DIM_90_DEG/3)),fr2,ptDimL[7]);
				//for (fi1=8;fi1<10;++fi1) ic_ptcpy(ptDimL[fi1],ptDimL[7]);
				//ptDimL[9][0]+=dDimasz;
				//ic_ptcpy(textIns,ptDimL[9]);
				//
				//textIns[0]+=(txtbox[1][0]/2);
				CreateDimfitLeader1(ptDimL, textIns, box2, TRUE);
				// ]-EBATECH(watanabe)

				MakeExteraLine1=TRUE;
				MakeExteraLine2=TRUE;
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			} else {
				if (!dimdleside1 && !dimdleside2 /*!plinkvar[DIMTSZ].dval.rval*/) {
					if (!Drag11point) {
						// SystemMetrix(Hiro)-[FIX: Dimension line is short when outside text is dragged.
						//if (textIns[0]<ptExtL[1][0]) {
						//		ptDimL[3][0]=textIns[0]+InterDist;
						//} else {
						//		if (!FlipDimXaxis) ptDimL[2][0]+=dDimasz;
						//		ptDimL[3][0]=textIns[0]-InterDist;
						//}
						ExtendDimLineEx(textIns, txtbox, ptExtL, SideofTextDrag, ptDimL);
						// ]-SystemMetrix(Hiro) FIX: Dimension line is short when outside text is dragged.
					}
				}
			}
			// Set plinkvar->textrot to dlnang so that the text is aligned with the dimension when the text is outside of the dimension.
			// Unless the user set the angle of the text (txtangflag)=TRUE.
			if (!plink->txtangflag) plink->textrot=plink->dlnang;
			// If DIMTAD is on then make the dimension line extend under the text.

			// EBATECH(watanabe)-[FIX: leader length and text position
			//if (plinkvar[DIMTAD].dval.ival|| dDimtvp) ptDimL[3][0]+=txtbox[1][0];
			if (plinkvar[DIMFIT].dval.ival != 4)
			{
				if ((plinkvar[DIMTAD].dval.ival) ||
					(dDimtvp))
				{
					 ptDimL[3][0] += txtbox[1][0];
				}
			}
			// ]-EBATECH(watanabe)
		} else {
			// THIS IS NEW STUFF ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			if (plinkvar[DIMFIT].dval.ival==4) {

				// EBATECH(watanabe)-[FIX: leader length and text position
				//fr3=((DIM_90_DEG/2)+(DIM_90_DEG/3));
				//
				//cmd_DimCAngle_Fit4(FlipDimXaxis,SideofTextDrag,&fr3,fr2);
				//
				//ptDimL[6][0]=((ptDimL[0][0]+ptDimL[1][0])/2);
				//ptDimL[6][1]=((ptDimL[0][1]+ptDimL[1][1])/2);
				//fr1=dDimexe+dDimdli+dDimexo;
				//sds_polar(ptDimL[6],fr3,fr1,ptDimL[7]);
				//
				//ic_ptcpy(ptDimL[8],ptDimL[7]);
				//
				//fr1=dDimasz;
				//if (Drag11point) {
				//		cmd_DimCAngle_FitText(FlipDimXaxis,SideofTextDrag,&fr3,fr2);
				//		sds_polar(ptDimL[8],fr3,fr1,ptDimL[9]);
				//
				//		// EBATECH(watanabe)-[FIX: leader length and text position
				//		double len = fr1 + box2[1][0];
				//		sds_polar(ptDimL[8], fr3, len, ptDimL[9]);
				//		ptDimL[9][0] -= ptDimL[8][0];
				//		ptDimL[9][1] -= ptDimL[8][1];
				//		ptDimL[9][2] -= ptDimL[8][2];
				//		// ]-EBATECH(watanabe)
				//
				//		fr1+=(txtbox[1][0]/2);
				//		sds_polar(ptDimL[8],fr3,fr1,textIns);
				// 
				//		// EBATECH(watanabe)-[FIX: leader length and text position
				//		textIns[1] += txtbox[1][1] / 2;
				//		double dGap = 0;
				//		if (dDimgap < 0)
				//		{
				//			dGap = fabs(dDimgap);
				//		}
				//		textIns[1] += dGap;
				//		textIns[1] = fabs(textIns[1]);
				//
				//		textIns[0] -= ptDimL[8][0];
				//		textIns[1] -= ptDimL[8][1];
				//		textIns[2] -= ptDimL[8][2];
				//
				//		dimtad4Flg = TRUE;
				//		// ]-EBATECH(watanabe)
				//}
				CreateDimfitLeader2(ptDimL, textIns, box2, txtbox, FlipDimXaxis, SideofTextDrag, Drag11point, Workangle, TRUE);
				dimtad4Flg = TRUE;
				// ]-EBATECH(watanabe)

				MakeExteraLine1=TRUE;
				MakeExteraLine2=TRUE;
			} else {
                // This code creates this type of dim.
                // ptDimL[1]
                //   |     __________PtDimL[3] and ptDiml[6]
                //   |    |  ________ptDimL[7]
                //   |    | |
                //   |    |_|TEXT
                //   |__\/_________ptDimL[2]
                //      /\
                //     /  \
                //   \/_____________ptDimL[0]
                //    \     .
                //     \
                //      \
                //       .
                //
				fr2=plink->dlnang;
				cmd_DimCalcAngle(FlipDimXaxis,SideofTextDrag,&fr2);
				if (plink->dlnang) {
					//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					// This code works with draging the text or draging the other dim points.
					if (Drag11point) textIns[0]=ptDimL[3][0]+(txtbox[1][0]/2);
					//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					// This moves the text insertion point, so that the dimension line does not have to move this is done when moveing the
					// 13,10 or 14 points. not when draging the dimension text.
					if (Drag11point) {
						ic_ptcpy(ptDimL[6],ptDimL[3]);
						if (!plinkvar[DIMTAD].dval.ival&& !dDimtvp) {
							sds_polar(ptDimL[6],(fr2+IC_PI),dDimasz,ptDimL[7]);
							sds_polar(ptDimL[6],(fr2+IC_PI),((txtbox[1][0]/2)+dDimasz),textIns);
						} else {
							sds_polar(ptDimL[6],(fr2+IC_PI),txtbox[1][0],ptDimL[7]);
							sds_polar(ptDimL[6],(fr2+IC_PI),(txtbox[1][0]/2),textIns);
						}
					} else {
						sds_polar(textIns,fr2,(txtbox[1][0]/2),ptDimL[7]);
						sds_polar(textIns,fr2,((txtbox[1][0]/2)+dDimasz),ptDimL[6]);
						ic_ptcpy(ptDimL[3],ptDimL[6]);
					}
					// Update the (Y) for the dimension lines stuff.
					for (fi1=0;fi1<6;++fi1) ptDimL[fi1][1]=ptDimL[6][1];
					// Update the (Y) for the Extension line dimension line intersection points.
					ExtDimInt[0][1]=ptDimL[6][1]; ExtDimInt[1][1]=ptDimL[6][1];
					// Update the (Y) for the extension lines.
					ptExtL[1][1]=ptDimL[6][1]+dDimexo;
					ptExtL[3][1]=ptDimL[6][1]+((pt10[1]<pt14[1])?-dDimexo:dDimexo);
					// Update the (Y) for the 10 point.
					pt10[1]=ptDimL[6][1];
					if (fr2) MakeExteraLine1=TRUE;
				}
				// If we are here then the dimension text is outside of the extension lines and DIMTOH=1 Draws text horizontally.
				// Unless the user set the angle of the text (txtangflag)=TRUE.
				if (!plink->txtangflag) plink->textrot=0.0;

				// SystemMetrix(Hiro)-[FIX: When DIMTOH = 1, the outside dimension line is hide.
				ExtendDimLineOutside(txtbox, dlnang, SideofTextDrag, ptDimL);
				// ]-SystemMetrix(Hiro) FIX: When DIMTOH = 1, the outside dimension line is hide.
			}
		}
	}
	else if (!ArrowFit && (plinkvar[DIMJUST].dval.ival!=3 && plinkvar[DIMJUST].dval.ival!=4)) 
	{
        // This code does this exact type of dimension. This is only done when the DIMTIH flag is set to 0.
        //  ______ptDimL[1]
        // |  ____ptDimL[0]
        // | |   _ptDimL[4]
        // | |  |  ___ptDimL[5]
        // | |  | |     ________ptDimL[7]
        // |_|\||_|TEXT|__|/___
        //    /|         /|\|  |_____ptDimL[3]
        //     |        / | |
        //     | ptDimL[6]| |___ptDimL[2]
        //     |          |
        //
		ic_ptcpy(ptDimL[5],ptDimL[1]);
		ic_ptcpy(ptDimL[4],ptDimL[0]);
		// Only if DIMTSZ is > 0.0
		if (!dimdleside1 /*plinkvar[DIMTSZ].dval.rval*/) ptDimL[0][0]-=dDimasz;
		ic_ptcpy(ptDimL[1],ptDimL[0]);
		// Only if DIMTSZ is > 0.0
		if (!dimdleside1 /*!plinkvar[DIMTSZ].dval.rval*/) ptDimL[1][0]-=dDimasz; else ptDimL[1][0]-=dDimdle;
		ic_ptcpy(ptDimL[7],ptDimL[3]);
		ic_ptcpy(ptDimL[6],ptDimL[2]);
		// Only if DIMTSZ is > 0.0
		if (!dimdleside2 /*!plinkvar[DIMTSZ].dval.rval*/) ptDimL[2][0]+=dDimasz;
		ic_ptcpy(ptDimL[3],ptDimL[2]);
		// Only if DIMTSZ is > 0.0
		if (!dimdleside2 /*!plinkvar[DIMTSZ].dval.rval*/) ptDimL[3][0]+=dDimasz; else ptDimL[3][0]+=dDimdle;
		// This prevents the Center line and the Extra line from being drawn.
		if (plinkvar[DIMSOXD].dval.ival) {
			dimsoxdflag=TRUE;
		} else if (plinkvar[DIMFIT].dval.ival!=3 && plinkvar[DIMFIT].dval.ival!=1) {
			MakeCenterLine=TRUE;
			if ((!plinkvar[DIMTAD].dval.ival|| plinkvar[DIMTIH].dval.ival) && !(dDimtvp)) MakeExteraLine1=TRUE;
		}
		// If DIMTAD is on then make the dimension line extend under the text.
		if ((plinkvar[DIMTAD].dval.ival==1 || (dDimtvp>0.0)) && !(plinkvar[DIMTIH].dval.ival)) {
			ic_ptcpy(ptDimL[5],ptDimL[6]);
		} else if (plinkvar[DIMTAD].dval.ival==2 || plinkvar[DIMTAD].dval.ival==3) {
			if (!icadRealEqual(plink->textrot,0.0)) {
				ic_ptcpy(ptDimL[5],ptDimL[7]);
			} else {
				//BugZilla No. 78075; 18-08-2003 [
				int x = 0;
				
				// Find the point with greatest Y-coordinate value
				if((boxrect[0][1] > boxrect[1][1]) && (boxrect[0][1] > boxrect[2][1]) 
					&& (boxrect[0][1] > boxrect[3][1]))
					x = 0; 
				else if((boxrect[1][1] > boxrect[2][1]) && (boxrect[1][1] > boxrect[3][1])) 
					x = 1; 
				else if(boxrect[2][1] > boxrect[3][1])
					x = 2; 
				else
					x = 3; 

				fr1 = fabs(boxrect[x][1] - textIns[1]) + fabs(dDimgap);
				//fr1=fabs(textIns[1]-boxrect[1][1])+fabs(dDimgap);	
				//BugZilla No. 78075; 18-08-2003 ]
				if(plinkvar[DIMTAD].dval.ival==3 && (plink->flags & DIM_ALIGNED))
				{
					if((plink->pt13[0] > plink->pt14[0]) || 
						(icadRealEqual(plink->pt13[0],plink->pt14[0]) && 
						(plink->pt13[1] > plink->pt14[1])))
					{
						if(FlipDimXaxis)
							textIns[1]+=fr1;
						else
							textIns[1]-=fr1;
					}
					else
					{
						if(FlipDimXaxis)
							textIns[1]-=fr1;
						else
							textIns[1]+=fr1;
					}
				}
				else
					textIns[1]+=fr1;
				ic_ptcpy(ptDimL[1],ptDimL[2]);
				dimDim2flag=FALSE;
			}
		}
		if (!plink->txtangflag) {
			// If DIMTIH=1 then make sure the dimension text is horizontal otherwise aligned with the dimension.
			// Unless the user set the angle of the text (txtangflag)=TRUE.
			if (plinkvar[DIMTIH].dval.ival) plink->textrot=0.0; else plink->textrot=plink->dlnang;
		}
	}
	else
	{
        //     ____________ ptDimL[0]
        //    |   _________ ptDimL[1]
        //    |  |     ____ ptDimL[3]
        //    |  |    |   _ ptDimL[2]
        //    |  |    |  |
        //  |/|__|TEXT|__|\|
        //  |\            /|
        //  |              |
        //  |              |
        //  |              |
        //
		// Make the dimension lines extend byond the extension lines DIMDLE.
		if (dimdleside1 || dimdleside2 /*plinkvar[DIMTSZ].dval.rval*/) 
		{
			ptDimL[0][0]-=dDimasz; 
			if (dimdleside1) ptDimL[0][0]-=dDimdle ;
			ptDimL[2][0]+=dDimasz; 
			if (dimdleside2) ptDimL[2][0]+=dDimdle ;
		}
		// If DIMTAD is on and DIMTIH is off or if dimjust is 2 or 4, then make the dimension line extend under the text.
		if (((plinkvar[DIMTAD].dval.ival==1 || (dDimtvp>0.0) ||
			(plinkvar[DIMTAD].dval.ival && !plink->dlnang)) && (!(plinkvar[DIMTIH].dval.ival) || !plink->dlnang)) ||
			(plinkvar[DIMJUST].dval.ival==3 || plinkvar[DIMJUST].dval.ival==4)) {
			ic_ptcpy(ptDimL[1],ptDimL[2]);
			dimDim2flag=FALSE;
		} else if (plinkvar[DIMTAD].dval.ival==2 || plinkvar[DIMTAD].dval.ival==3) {
			if (!icadRealEqual(plink->textrot,0.0)) {
				ic_ptcpy(ptDimL[1],ptDimL[2]);
				dimDim2flag=FALSE;
			} else {
				//BugZilla No. 78075; 18-08-2003 [
				int x = 0;
				
				// Find the point with greatest Y-coordinate value
				if((boxrect[0][1] > boxrect[1][1]) && (boxrect[0][1] > boxrect[2][1]) 
					&& (boxrect[0][1] > boxrect[3][1]))
					x = 0; 
				else if((boxrect[1][1] > boxrect[2][1]) && (boxrect[1][1] > boxrect[3][1])) 
					x = 1; 
				else if(boxrect[2][1] > boxrect[3][1])
					x = 2; 
				else
					x = 3; 
				
				fr1 = fabs(boxrect[x][1] - textIns[1]) + fabs(dDimgap);
				//fr1=fabs(textIns[1]-boxrect[1][1])+fabs(dDimgap);
				//BugZilla No. 78075; 18-08-2003 ]
				if(plinkvar[DIMTAD].dval.ival==3 && (plink->flags & DIM_ALIGNED))
				{
					if((plink->pt13[0] > plink->pt14[0]) || 
						(icadRealEqual(plink->pt13[0],plink->pt14[0]) && 
						(plink->pt13[1] > plink->pt14[1])))
					{
						if(FlipDimXaxis)
							textIns[1]+=fr1;
						else
							textIns[1]-=fr1;
					}
					else
					{
						if(FlipDimXaxis)
							textIns[1]-=fr1;
						else
							textIns[1]+=fr1;
					}
				}
				else
					textIns[1]+=fr1;
				ic_ptcpy(ptDimL[1],ptDimL[2]);
				dimDim2flag=FALSE;
			}
		}
		// If the user did not set the angle of the text or if (DIMJUST)!=3,4.
		if (!plink->txtangflag && (plinkvar[DIMJUST].dval.ival!=3 && plinkvar[DIMJUST].dval.ival!=4)) {
			// If DIMTIH=1 then make sure the dimension text is horizontal otherwise aligned with the dimension.
			// Unless the user set the angle of the text (txtangflag)=TRUE.
			if (plinkvar[DIMTIH].dval.ival) plink->textrot=0.0; else plink->textrot=plink->dlnang;
		}
	//			if (plinkvar[DIMUPT].dval.ival && !plinkvar[DIMJUST].dval.ival) ArrowFit=!ArrowFit;
	}
	// End of section.
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// If the text is going to be positioned by (DIMJUST)=3,4.
	// if dimjust=3 then put the text center point above the first line picked which would be the 13 point line.
	// if dimjust=4 then put the text center point above the seconed line picked which would be the 14 point line.
    //  _              _
    // | |     or     | | ___ Text.
    // |_|            |_|
    //  |/____________\|
    //  |\            /|
    //  |              |
    //  |              |
    //
	if (SDS_AtNodeDrag!=2 && (!(plinkvar[DIMUPT].dval.ival) || plinkvar[DIMJUST].dval.ival==3 || plinkvar[DIMJUST].dval.ival==4)) {
		switch(plinkvar[DIMJUST].dval.ival) {
			case 3:
				textIns[0]=pt13[0];
				textIns[1]=ptExtL[1][1]+(txtbox[1][0]/2);
				
				// horizontal dimension (plink->dlnang = 0)
				if (icadRealEqual(plink->dlnang, 0.0))
					plink->textrot = DIM_90_DEG;
				else // vertical or rotated dimension
					plink->textrot = plink->dlnang - DIM_90_DEG;

				dlnang+=DIM_90_DEG;
				break;
			case 4:
				textIns[0]=pt14[0];
				textIns[1]=ptExtL[3][1]+((ptExtL[2][1]>ptExtL[3][1])?-(txtbox[1][0]/2):(txtbox[1][0]/2));
				
				// horizontal dimension (plink->dlnang = 0)
				if (icadRealEqual(plink->dlnang, 0.0))
					plink->textrot = DIM_90_DEG;
				else // vertical or rotated dimension
					plink->textrot = plink->dlnang - DIM_90_DEG;

				dlnang+=DIM_90_DEG;
				break;
		}
	}

EndOfAdujstmentWhenDimtmoveIsNotZero:	// EBATECH(CNBR) 13-03-2003 

	// When the text is aligned with the dimension, somtimes the text can be aligned but upside down, so I need to rotate it 180 degrees.
	TextRotation=plink->textrot;
	
	//Added Cybage VSB 13/11/2001 DD/MM/YYYY
	//Reason : Fix for Bug No. 77929 from Bugzilla [
	if ( plink->textrot == plink->dlnang )
		bTextFreeAng = TRUE;
	else
		bTextFreeAng = FALSE;
	//Added Cybage VSB 13/11/2001 DD/MM/YYYY ]

	// EBATECH(watanabe)-[text rotation
	//if (TextRotation && dlnang<DIM_270_DEG && dlnang>DIM_90_DEG) TextRotation+=IC_PI;
	if (bTextFreeAng)
		{
		if (!((iDimtih && TextFit) || (iDimtoh && !TextFit)))
			{
			TextRotation = dlnang;

			// SystemMetrix(Hiro)-[FIX:fuzz of angle
			//if ((dlnang < DIM_270_DEG) && (dlnang > DIM_90_DEG))
			if ((dlnang < DIM_270_DEG + CMD_FUZZ) &&
				(dlnang > DIM_90_DEG	+ CMD_FUZZ))
			// ]-SystemMetrix(Hiro) FIX:fuzz of angle
				{
				TextRotation -= IC_PI;
				}
			}
		}
	// ]-EBATECH(watanabe)

	// Flip dimension back to the original side of the (x) axis.
	if (FlipDimXaxis) {
		for (fi1=0;fi1<2;++fi1)  ExtDimInt[fi1][1]*=(-1);
		for (fi1=0;fi1<4;++fi1)  ptExtL[fi1][1]*=(-1);

		// EBATECH(watanabe)-[FIX: leader length and text position
		//for (fi1=0;fi1<10;++fi1) ptDimL[fi1][1]*=(-1);
		for (fi1 = 0; fi1 < 10; ++fi1)
			{
			if (fi1 == 9)
				{
				if (dimtad4Flg)
					{
					continue;
					}
				}
			ptDimL[fi1][1] *= -1;
			}
		// ]-EBATECH(watanabe)

		pt10[1]*=(-1);

		// EBATECH(watanabe)-[FIX: leader length and text position
		/*if ((plink->flags&DIM_TEXT_MOVED)!=DIM_TEXT_MOVED) *///textIns[1]*=(-1);
		if (dimtad4Flg == FALSE)
			{
			textIns[1] *= -1;
			}
		// ]-EBATECH(watanabe)

		if (!TextFit) {
			// Make sure the text is aligned with the dimension.
			cmd_getboxintersec(txtbox,0.0,textIns,ptDimL[0],ptDimL[2],ptt,boxrect,&fi1,&fi2);
		}
	} else {
		if (!TextFit) {
			// Make sure the text is aligned with the dimension.
			cmd_getboxintersec(txtbox,0.0,textIns,ptDimL[0],ptDimL[2],ptt,boxrect,&fi1,&fi2);
		}
	}
	sds_point tip;
	// DP: save Text Insertion Point in this case to restore him later
	if((SDS_AtNodeDrag == 4 || SDS_AtNodeDrag == 5) && (plink->flags & DIM_TEXT_MOVED))
		memcpy(tip, textIns, sizeof(sds_point));

	// Move Text above the dimension line.
	if( (plinkvar[DIMTAD].dval.ival &&
		 ((TextFit && (!plinkvar[DIMTIH].dval.ival || !plink->dlnang)) || !TextFit)) ||
		(dDimtvp > 0.0 &&
		 ((TextFit && !plinkvar[DIMTIH].dval.ival) || !TextFit)) )

	// EBATECH(watanabe)-[FIX: leader length and text position
		{
		moveTexgFlg = TRUE;
		}

	if (dimtad4Flg)
		{
		moveTexgFlg = FALSE;
		}

	if (moveTexgFlg)
	// ]-EBATECH(watanabe)
	{
		// If the dimension text is horizontal then just transform the point back to ucs and add to the (Y) then transform back to DIM. system.

		// EBATECH(watanabe)-[debug DIMGAP
		//if ((plinkvar[DIMTIH].dval.ival && TextFit) || (plinkvar[DIMTOH].dval.ival && !TextFit)) {
		double d = 0.0;
		if (dDimgap < 0)
			{
			// if drawing box around dimension text, add DIMGAP again
			d = fabs(dDimgap);
			}

		d += box2[1][1] / 2;
		if ((iDimtih &&  TextFit) ||
			(iDimtoh && !TextFit))
			{
		// ]-EBATECH(watanabe)

			cmd_dimtoucs(plink->pt13,textIns,ptt[0],sinofdeg,cosofdeg);
//				if (!(SDS_AtNodeDrag == 4 || SDS_AtNodeDrag == 5))	/*D.G.*/// We should align ins. point in any drag. mode.

				// EBATECH(watanabe)-[debug DIMGAP
				//if (dDimtvp && !plinkvar[DIMTAD].dval.ival) {
				//			ptt[0][1]+=(dDimtvp+ dDimgap/4;
				if (dDimtvp && !iDimtad) {
					d += dDimtvp;
				// ]-EBATECH(watanabe)

				} else {

					// EBATECH(watanabe)-[debug DIMGAP
					//if(Workangle > DIM_90_DEG && Workangle <= DIM_270_DEG  && !plinkvar[DIMTAD].dval.ival)	/*D.G.*/// DIMTAD checking added.
					//		ptt[0][1]-=((pt10[1]<0 && plinkvar[DIMTAD].dval.ival==2)?-(txtbox[1][1]/2+dDimgap/4):(txtbox[1][1]/2+dDimgap/4));
					//else
					//		ptt[0][1]+=((pt10[1]<0 && plinkvar[DIMTAD].dval.ival==2)?-(txtbox[1][1]/2+dDimgap/4):(txtbox[1][1]/2+dDimgap/4));

					if ((pt10[1] < 0) && (iDimtad == 2))
						{
						d = -d;
						}

					// SystemMetrix(Hiro)-[FIX:fuzz of angle
					//if ((Workangle >	DIM_90_DEG)  &&
					//		(Workangle <= DIM_270_DEG) &&
					//		!iDimtad)
					if ((Workangle > DIM_90_DEG  + CMD_FUZZ) &&
						(Workangle < DIM_270_DEG + CMD_FUZZ) &&
						!iDimtad)
					// ]-SystemMetrix(Hiro) FIX:fuzz of angle
						{
						d = -d;
						if ((pt10[1] < 0) && (iDimtad == 2))
							{
							d = -d;
							}
						}
					// ]-EBATECH(watanabe)

				}

			// EBATECH(watanabe)-[debug DIMGAP
			ptt[0][1] += d;
			// ]-EBATECH(watanabe)

			cmd_ucstodim(plink->pt13,ptt[0],textIns,sinofdeg,cosofdeg);
		// If the dimension text is aligned with the dimension then add to the (Y) in DIM. system.
		} else {
			ic_normang(&Workangle,NULL);
//				if (!(SDS_AtNodeDrag == 4 || SDS_AtNodeDrag == 5))	/*D.G.*/// We should align ins. point in any drag. mode.

				// SystemMetrix(Hiro)-[FIX:fuzz of angle
				//if (Workangle<=DIM_270_DEG && Workangle>(IC_PI/2)) {
				if ((Workangle < DIM_270_DEG + CMD_FUZZ) &&
					(Workangle > DIM_90_DEG  + CMD_FUZZ)) {
				// ]-SystemMetrix(Hiro) FIX:fuzz of angle

					// EBATECH(watanabe)-[debug DIMGAP
					//if (dDimtvp && !plinkvar[DIMTAD].dval.ival)
					//			textIns[1] -= dDimtvp + dDimgap / 4.0;
					if (dDimtvp && !iDimtad)
						{
						d += dDimtvp;
						d = -d;
						}
					// ]-EBATECH(watanabe)
					else
					// EBATECH(watanabe)-[debug DIMGAP
					//			textIns[1] -= (pt10[1] > 0.0	&&	plinkvar[DIMTAD].dval.ival == 2) ?
					//						- txtbox[1][1] / 2.0 + dDimgap / 4.0 :
					//							txtbox[1][1] / 2.0 + dDimgap / 4.0;
						{
						d = -d;
						if ((pt10[1] > 0.0) && (iDimtad == 2))
							{
							d = -d;
							}
						}
					// ]-EBATECH(watanabe)

				} else {

					// EBATECH(watanabe)-[debug DIMGAP
					//if (dDimtvp && !plinkvar[DIMTAD].dval.ival)
					//			textIns[1] += dDimtvp + dDimgap / 4.0;
					if (dDimtvp && !iDimtad)
						{
						d += dDimtvp;
						}
					// ]-EBATECH(watanabe)
					else
					// EBATECH(watanabe)-[debug DIMGAP
					//			textIns[1] += (pt10[1] < 0.0 && plinkvar[DIMTAD].dval.ival == 2) ?
					//						- txtbox[1][1] / 2.0 + dDimgap / 4.0 :
					//							txtbox[1][1] / 2.0 + dDimgap / 4.0;
						{
						if ((pt10[1] < 0.0) && (iDimtad == 2))
							{
							d = -d;
							}
						}
					// ]-EBATECH(watanabe)
				}

			// EBATECH(CNBR) -[ Skip moving text when already text is moved.
			if(( plink->HomeText || !plinkvar[DIMTMOVE].dval.ival ) && !icadRealEqual(textIns[1], pt10[1]+ d ))
			{
				// EBATECH(watanabe)-[debug DIMGAP
				textIns[1] += d;
				// ]-EBATECH(watanabe)
			}
			// EBATECH(CNBR) ]-

		}

	}

	// DP: restore Text Insertion Point
	if((SDS_AtNodeDrag == 4 || SDS_AtNodeDrag == 5) && (plink->flags & DIM_TEXT_MOVED))
		memcpy(textIns, tip, sizeof(sds_point));

	// Make sure that the 10 point's (X) is at the same (x) location as the 14 point. when the last dimension is made.
	if (!dragmode) {
		pt10[0]=pt14[0];
		cmd_dimtoucs(plink->pt13,pt10,plink->defpt,sinofdeg,cosofdeg);
	}
	//Bugzilla No. 78113 ; 10-06-2002 [
	if(dragmode == 1)
	{
		plink->dlnang = dlnang2;
		plink->textrot = initialrot;
	}
	//Bugzilla No. 78113 ; 10-06-2002 ]

	return DrawDimLinear(
				plink, flp, dragmode, ptExtL, ptDimL, ptt, box2,	
				boxrect, ExtDimInt, textIns, cosofdeg, sinofdeg,
				TextRotation, text, dimsoxdflag, dimDim2flag,
				MakeCenterLine, MakeExteraLine1, MakeExteraLine2,
				dimtad4Flg, ArrowFit);
}

//
// Check for arrowhead blocks and create if needs
static void
CreateDimArrow(
	db_drawing*	flp,
	BOOL			dimsoxdflag
)
{
//	if (!plinkvar[DIMSD1].dval.ival && !dimsoxdflag)
	{
		if ((plinkvar[DIMSAH].dval.ival) && *plinkvar[DIMBLK1].dval.cval) 
			strcpy(cmd_string1,plinkvar[DIMBLK1].dval.cval);
		else if (plinkvar[DIMBLK].dval.cval && *plinkvar[DIMBLK].dval.cval) 
			strcpy(cmd_string1,plinkvar[DIMBLK].dval.cval); 
		else 
			strcpy(cmd_string1,DIM_CLOSED_FILLED);

		if( *cmd_string1 )
			checkBlock( cmd_string1, flp );
		
	}
//	if (!plinkvar[DIMSD2].dval.ival && !dimsoxdflag)
	{
		if ((plinkvar[DIMSAH].dval.ival) && *plinkvar[DIMBLK2].dval.cval) 
			strcpy(cmd_string2,plinkvar[DIMBLK2].dval.cval);
		else if (plinkvar[DIMBLK].dval.cval && *plinkvar[DIMBLK].dval.cval) 
			strcpy(cmd_string2,plinkvar[DIMBLK].dval.cval); 
		else 
			strcpy(cmd_string2,DIM_CLOSED_FILLED);

		if( *cmd_string2 )
			checkBlock( cmd_string2, flp );

	}
}

// Draw DIMENSION
static BOOL
DrawDimLinear(
	struct cmd_dimeparlink*	plink,
	db_drawing*	flp,
	short 		dragmode,
	sds_point 	ptExtL[],
	sds_point 	ptDimL[],
	sds_point 	ptt[],
	sds_point 	box2[],
	sds_point 	boxrect[],
	sds_point 	ExtDimInt[],
	sds_point 	textIns,
	sds_real		cosofdeg,
	sds_real		sinofdeg,
	sds_real		TextRotation,
	char			text[],
	BOOL			dimsoxdflag,
	BOOL			dimDim2flag,
	BOOL			MakeCenterLine,
	BOOL			MakeExteraLine1,
	BOOL			MakeExteraLine2,
	BOOL			dimtad4Flg,
	BOOL			ArrowFit
)
{
	short			 fi1;
	sds_point		 ipt = {0.0, 0.0, 0.0};
	struct resbuf  rb;
	db_handitem 	*handitem=NULL;
	sds_real		 cosoftext = 0.0,
					 sinoftext = 0.0;
	short			celweight = cmd_getCelweight();

	CreateDimArrow(flp, dimsoxdflag);
	if(!dragmode && dragmode != -1 && plinkvar[DIMASO].dval.ival)
		if(RTNORM != dwg_addblock(layername, 256, "*D"/*DNT*/, 1, ipt, NULL, flp)) 
			goto exit;	

	if(RTNORM != cmd_dimaddpoint(DIM_DEFPOINTS, plinkvar[DIMCLRE].dval.ival, celweight, plink->pt13, flp, dragmode))
		goto exit;
	if(RTNORM != cmd_dimaddpoint(DIM_DEFPOINTS, plinkvar[DIMCLRE].dval.ival, celweight, plink->pt14, flp, dragmode))
		goto exit;

	if(!plinkvar[DIMSE1].dval.ival)
	{
		for(fi1=0;fi1<2;++fi1)	
			cmd_dimtoucs(plink->pt13,ptExtL[fi1],ptt[fi1],sinofdeg,cosofdeg);
		if(RTNORM!=cmd_dimaddline(layername,plinkvar[DIMCLRE].dval.ival,plinkvar[DIMLWE].dval.ival,ptt[0],ptt[1],flp,dragmode))	
			goto exit;
	}
	if(!plinkvar[DIMSE2].dval.ival)
	{
		for(fi1=2;fi1<4;++fi1)
			cmd_dimtoucs(plink->pt13,ptExtL[fi1],ptt[fi1],sinofdeg,cosofdeg);
		if(RTNORM!=cmd_dimaddline(layername,plinkvar[DIMCLRE].dval.ival,plinkvar[DIMLWE].dval.ival,ptt[2],ptt[3],flp,dragmode)) 
			goto exit;
	}
	if(!plinkvar[DIMSD1].dval.ival && !dimsoxdflag) 
	{
		for (fi1=0;fi1<2;++fi1) 
			cmd_dimtoucs(plink->pt13,ptDimL[fi1],ptt[fi1],sinofdeg,cosofdeg);
		if (RTNORM!=cmd_dimaddline(layername,plinkvar[DIMCLRD].dval.ival,plinkvar[DIMLWD].dval.ival,ptt[0],ptt[1],flp,dragmode)) 
			goto exit;

/*			if ((plinkvar[DIMSAH].dval.ival) && *plinkvar[DIMBLK1].dval.cval) 
			strcpy(cmd_string1,plinkvar[DIMBLK1].dval.cval);
		else if (plinkvar[DIMBLK].dval.cval && *plinkvar[DIMBLK].dval.cval) 
			strcpy(cmd_string1,plinkvar[DIMBLK].dval.cval); 
		else 
			strcpy(cmd_string1,DIM_CLOSED_FILLED); */
//			cmd_MakeItemHead(ExtDimInt,cmd_string,flp,dragmode,sinofdeg,cosofdeg,sds_angle(ptt[0],ptt[1]), !ArrowFit,FALSE,plink->pt13,0);
		cmd_MakeItemHead(ExtDimInt,cmd_string1,flp,dragmode,sinofdeg,cosofdeg,sds_angle(ptt[1],ptt[0]), !ArrowFit,FALSE,plink->pt13,0);
	}
	if(!plinkvar[DIMSD2].dval.ival && !dimsoxdflag) 
	{
			for (fi1=2;fi1<4;++fi1) 
				cmd_dimtoucs(plink->pt13,ptDimL[fi1],ptt[fi1],sinofdeg,cosofdeg);
		if(dimDim2flag) {	// EBATECH(watanabe) - Here was ignored.
			if (RTNORM!=cmd_dimaddline(layername,plinkvar[DIMCLRD].dval.ival,plinkvar[DIMLWD].dval.ival,ptt[2],ptt[3],flp,dragmode)) 
				goto exit;
		}
/*			if ((plinkvar[DIMSAH].dval.ival) && *plinkvar[DIMBLK2].dval.cval) 
			strcpy(cmd_string2,plinkvar[DIMBLK2].dval.cval);
		else if (plinkvar[DIMBLK].dval.cval && *plinkvar[DIMBLK].dval.cval) 
			strcpy(cmd_string2,plinkvar[DIMBLK].dval.cval); 
		else 
			strcpy(cmd_string2,DIM_CLOSED_FILLED); */
//			cmd_MakeItemHead(ExtDimInt,cmd_string,flp,dragmode,sinofdeg,cosofdeg,sds_angle(ptt[2],ptt[3]), ArrowFit,TRUE,plink->pt13,0);
		cmd_MakeItemHead(ExtDimInt,cmd_string2,flp,dragmode,sinofdeg,cosofdeg,sds_angle(ptt[3],ptt[2]), ArrowFit,TRUE,plink->pt13,0);
	}
	if(MakeCenterLine) 
	{
		for(fi1=4;fi1<6;++fi1)
			cmd_dimtoucs(plink->pt13,ptDimL[fi1],ptt[fi1],sinofdeg,cosofdeg);
		if(RTNORM!=cmd_dimaddline(layername,plinkvar[DIMCLRD].dval.ival,plinkvar[DIMLWD].dval.ival,ptt[4],ptt[5],flp,dragmode)) 
			goto exit;
	}
	if(MakeExteraLine1) 
	{
		for(fi1=6;fi1<8;++fi1) 
			cmd_dimtoucs(plink->pt13,ptDimL[fi1],ptt[fi1-2],sinofdeg,cosofdeg);
		if(RTNORM!=cmd_dimaddline(layername,plinkvar[DIMCLRD].dval.ival,plinkvar[DIMLWD].dval.ival,ptt[4],ptt[5],flp,dragmode)) 
			goto exit;
	}
	if (MakeExteraLine2) 
	{
		for (fi1=8;fi1<10;++fi1)
			cmd_dimtoucs(plink->pt13,ptDimL[fi1],ptt[fi1-4],sinofdeg,cosofdeg);

		// EBATECH(watanabe)-[FIX: leader length and text position
		if (dimtad4Flg)
			{
			ic_ptcpy(ptt[5], ptDimL[9]);
			ptt[5][0] += ptt[4][0];
			ptt[5][1] += ptt[4][1];
			ptt[5][2] += ptt[4][2];
			}
		// ]-EBATECH(watanabe)
		if(RTNORM!=cmd_dimaddline(layername,plinkvar[DIMCLRD].dval.ival,plinkvar[DIMLWD].dval.ival,ptt[4],ptt[5],flp,dragmode)) 
			goto exit;
	}

	cmd_dimtoucs(plink->pt13, textIns, plink->textmidpt, sinofdeg, cosofdeg);

	// EBATECH(watanabe)-[FIX: leader length and text position
	if (dimtad4Flg)
		{
		ic_ptcpy(plink->textmidpt, textIns);
		plink->textmidpt[0] += ptt[4][0];
		plink->textmidpt[1] += ptt[4][1];
		plink->textmidpt[2] += ptt[4][2];
		}
	// ]-EBATECH(watanabe)

	if(RTNORM != cmd_dimaddmtext(layername, plinkvar[DIMCLRT].dval.ival, plink->lweight, plink->textmidpt, plinkvar[DIMTXT].dval.rval * plinkvar[DIMSCALE].dval.rval, text, TextRotation, 0.0, plinkvar[DIMTXSTY].dval.cval, 5, plink->pt210, flp, dragmode))
		goto exit;
	if ((plinkvar[DIMGAP].dval.rval*plinkvar[DIMSCALE].dval.rval)<0.0) {
		// Here need to build box around the text.
		for (fi1=0;fi1<4;++fi1) ic_ptcpy(ptt[fi1],plink->textmidpt);

		// EBATECH(watanabe)-[debug DIMGAP
		//ptt[0][0]-=(txtbox[1][0]/2); ptt[2][0]-=(txtbox[1][0]/2);
		//ptt[1][0]+=(txtbox[1][0]/2); ptt[3][0]+=(txtbox[1][0]/2);
		//ptt[0][1]+=(txtbox[1][1]/2); ptt[1][1]+=(txtbox[1][1]/2);
		//ptt[2][1]-=(txtbox[1][1]/2); ptt[3][1]-=(txtbox[1][1]/2);
		ptt[0][0] -= box2[1][0] / 2; ptt[2][0] -= box2[1][0] / 2;
		ptt[1][0] += box2[1][0] / 2; ptt[3][0] += box2[1][0] / 2;
		ptt[0][1] += box2[1][1] / 2; ptt[1][1] += box2[1][1] / 2;
		ptt[2][1] -= box2[1][1] / 2; ptt[3][1] -= box2[1][1] / 2;
		// ]-EBATECH(watanabe)

		sinoftext=sin(0.0); cosoftext=cos(0.0);
		for (fi1=0;fi1<4;++fi1) cmd_ucstodim(plink->textmidpt,ptt[fi1],boxrect[fi1],sinoftext,cosoftext);
		sinoftext=sin(TextRotation); cosoftext=cos(TextRotation);
		for (fi1=0;fi1<4;++fi1) cmd_dimtoucs(plink->textmidpt,boxrect[fi1],ptt[fi1],sinoftext,cosoftext);
		if (!cmd_drawbox(plinkvar[DIMCLRD].dval.ival,plinkvar[DIMLWD].dval.ival,ptt,flp,dragmode)) goto exit;
	}
	if (!dragmode && dragmode!=-1 && plinkvar[DIMASO].dval.ival) {
		if (RTERROR==dwg_addendblk(flp)) goto exit;
		handitem=flp->ret_lastblockdef();
		handitem->get_2(plink->bname,sizeof(plink->bname)-1);
	}

	return(TRUE);
	exit:
		rb.restype		=RTSHORT;
		rb.resval.rint=OL_EMKANON;
		SDS_setvar("ERRNO"/*DNT*/,-1,&rb,flp,NULL,NULL,0);
		return(FALSE);
}

//////////////////////////////////////////////////////	//////////////////////////////////////////////////////////////////////////////////////////
// This function calculates angle of direction the dimesion lines should be written to the text.
//
void cmd_DimCAngle_FitText(BOOL FlipDimXaxis,BOOL SideofTextDrag,sds_real *fr1,sds_real fr2) {

	// EBATECH(watanabe)-[FIX: leader length and text position
	//sds_real fr3=((DIM_90_DEG/2)+(DIM_90_DEG/3)+fr2);
	//// Make sure that any angle in the first quadrent.
	//if (fr3<DIM_90_DEG && fr3>0.0) {
	//		if (FlipDimXaxis) {
	//			if (SideofTextDrag) {
	//				if (fr2==0.0) {
	//				 (*fr1)=IC_PI;
	//				} else {
	//				 (*fr1)=(IC_PI-(*fr1));
	//				}
	//			}
	//		} else {
	//			if (SideofTextDrag) (*fr1)=-(fr2); // Good
	//		}
	//// Make sure that any angle in the seconed quadrent is rotated correctly.
	//} else if (fr3>DIM_90_DEG && fr3<IC_PI) {
	//		if (!FlipDimXaxis) {
	//			if (SideofTextDrag) (*fr1)=((DIM_90_DEG-fr2)+DIM_90_DEG); // Good.
	//		} else {
	//			if (SideofTextDrag) (*fr1)=fr2; // Good.
	//		}
	//// Make sure that any angle in the third quadrent is rotated correctly.
	//} else if (fr3>(IC_PI-CMD_FUZZ) && fr3<DIM_270_DEG) {
	//		if (!FlipDimXaxis) {
	//			if (SideofTextDrag) (*fr1)=(DIM_2_PI-fr2); // Good.
	//		} else {
	//			if (SideofTextDrag) {
	//				// This is a special case when the dimension is rotated 180 degrees.
	//				if (fr2<(IC_PI+CMD_FUZZ) && fr2>(IC_PI-CMD_FUZZ)) {
	//					(*fr1)=fr2;
	//				} else {
	//					(*fr1)=fr2; // Good.
	//				}
	//			}
	//		}
	//// Make sure that any angle in the fourth quadrent is rotated correctly.
	//} else {
	//		if (FlipDimXaxis) {
	//			if (SideofTextDrag) (*fr1)=(DIM_270_DEG-((*fr1)-DIM_270_DEG));
	//		} else {
	//			if (SideofTextDrag) (*fr1)=-(fr2); // Good.
	//		}
	//}
	if (SideofTextDrag)
		{
		if (fr2 > IC_PI - CMD_FUZZ2)
			{
			// swap flag
			FlipDimXaxis = FlipDimXaxis ? FALSE : TRUE;
			}

		if (!FlipDimXaxis)
			{
			if ((fabs(fr2		 ) < CMD_FUZZ2) ||
				(fabs(fr2 - IC_PI) < CMD_FUZZ2))
				{
				(*fr1) = 0;
				}
			else
				{
				(*fr1) = IC_PI;
				}
			}
		else
			{
			(*fr1) = 0;
			}
		}
	// ]-EBATECH(watanabe)

	// normalize the angle.
	ic_normang(&(*fr1),NULL);
}

// EBATECH(watanabe)-[FIX: leader length and text position
//////////////////////////////////////////////////////////////////////
//// This function calculates angle of direction the dimesion lines should be written to the text.
////
//void cmd_DimCAngle_Fit4(BOOL FlipDimXaxis,BOOL SideofTextDrag,sds_real *fr1,sds_real fr2) {
//		// Make sure that any angle in the first quadrent.
//		if (fr2<=(DIM_90_DEG+CMD_FUZZ) && fr2>=0.0) {
//			if (FlipDimXaxis) { if (SideofTextDrag) (*fr1)=(IC_PI-(*fr1)); }
//		// Make sure that any angle in the seconed quadrent is rotated correctly.
//		} else if (fr2>DIM_90_DEG && fr2<=(IC_PI+CMD_FUZZ)) {
//			if (!FlipDimXaxis) {
//				if (SideofTextDrag) (*fr1)=(DIM_90_DEG+(DIM_90_DEG-(*fr1)));
//			} else {
//				if (SideofTextDrag) (*fr1)=(DIM_90_DEG+(DIM_90_DEG-(*fr1)));
//			}
//		// Make sure that any angle in the third quadrent is rotated correctly.
//		} else if (fr2>IC_PI && fr2<DIM_270_DEG) {
//			if (!FlipDimXaxis) { if (SideofTextDrag) (*fr1)=(*fr1); }
//
//		// Make sure that any angle in the fourth quadrent is rotated correctly.
//		} else {
//			if (FlipDimXaxis) {
//				if (SideofTextDrag) (*fr1)=(DIM_90_DEG+((*fr1)-DIM_90_DEG));
//			} else {
//				if (SideofTextDrag) (*fr1)=(*fr1);
//			}
//		}
//		// normalize the angle.
//		ic_normang(&(*fr1),NULL);
//}
// ]-EBATECH(watanabe)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This function calculates angle of direction the dimesion lines should be written to the text.
//
void cmd_DimCalcAngle(BOOL FlipDimXaxis,BOOL SideofTextDrag,sds_real *fr2) {
	if (FlipDimXaxis) {
		if (SideofTextDrag) (*fr2)+=(IC_PI);
	} else {
		// Make sure that any angle in the first quadrent.
		if (((*fr2)<=IC_PI/2) && (*fr2)>0.0) {
			if (SideofTextDrag) (*fr2)=(IC_PI-(*fr2)); else (*fr2)=((IC_PI*2)-(*fr2));
		// Make sure that any angle in the seconed quadrent is rotated correctly.
		} else if ((*fr2)>DIM_90_DEG && (*fr2)<=IC_PI) {
			if (SideofTextDrag) (*fr2)=DIM_270_DEG-((*fr2)-DIM_90_DEG); else (*fr2)=DIM_90_DEG-((*fr2)-DIM_90_DEG);
		// Make sure that any angle in the third quadrent is rotated correctly.
		} else if ((*fr2)>IC_PI && (*fr2)<DIM_270_DEG) {
			if (SideofTextDrag) (*fr2)=IC_PI-((*fr2)-IC_PI); else (*fr2)=(IC_PI*2)-((*fr2)-IC_PI);
		// Make sure that any angle in the fourth quadrent is rotated correctly.
		} else {
			if (SideofTextDrag) (*fr2)=DIM_270_DEG-((*fr2)-DIM_270_DEG); else (*fr2)=((IC_PI*2)-(*fr2));
		}
	}
	// normalize the angle.
	ic_normang(&(*fr2),NULL);
}
