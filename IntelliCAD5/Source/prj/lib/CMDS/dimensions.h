#ifndef __dimensionsH__
#define __dimensionsH__
//4M Item:129->
#include "configure.h"
//<-4M Item:129

struct cmd_dimeparlink
{
	/********************************************************************************
	 * Author:	Dmitry Gavrilov.
	 * Purpose:	Safe initialization constructor.
	 * Returns:	None.
	 ********************************************************************************/
	cmd_dimeparlink()
	{
		//Bugzilla No. 77887 ; 03-06-2002
		distance = sangle = eangle = angle = dimfontheight =
		leadlen = hdir = dimvalue = dlnang = elnang = textrot = alttextrot =
		vertex[0] = vertex[1] = vertex[2] = 0.0;
		DB_PTCPY(ptt[0], vertex);
		DB_PTCPY(ptt[1], vertex);
		DB_PTCPY(ptt[2], vertex);
		DB_PTCPY(ptt[3], vertex);
		DB_PTCPY(pt210, vertex);
		DB_PTCPY(defpt, vertex);
		DB_PTCPY(textmidpt, vertex);
		DB_PTCPY(blkipt, vertex);
		DB_PTCPY(pt13, vertex);
		DB_PTCPY(pt14, vertex);
		DB_PTCPY(pt15, vertex);
		DB_PTCPY(pt16, vertex);
		//Bugzilla No. 77887 ; 03-06-2002
		dimtfac = pt210[2] = 1.0;
		HomeText = txtangflag = GenInitflg = DefValidflg = Setparflag = FALSE;
		BeginDrag = TRUE;
		flags = dimmode = 0;

		dimstyle = NULL;
		pLayerName = NULL;
		text[0] = bname[0] = (char)'\0x0';
	}

    // all the values for the dimension.
    sds_point   ptt[4],
                pt210,
                vertex;
    sds_real    distance,
                sangle,
                eangle,
                angle,
				//Bugzilla No. 77887 ; 03-06-2002
				dimtfac,
				dimfontheight;
    sds_point   defpt,
                textmidpt,
                blkipt,
                pt13,
                pt14,
                pt15,
                pt16;
    sds_real    leadlen,
                hdir,
                dimvalue,
                dlnang,
                elnang,
                textrot,
                alttextrot;
    BOOL        txtangflag,
                GenInitflg,
                DefValidflg,
                BeginDrag,
                HomeText,	// EBATECH(CNBR) Bugzilla#78455 13-03-2003 
                Setparflag;
    int         flags,
                dimmode;
    int         lweight;	// EBATECH(CNBR) Bugzilla#78455 13-03-2003 
    char        text[256],      // Without mtext formatting codes.
                *dimstyle,      // With mtext formating codes
				*pLayerName,
                bname[IC_ACADNMLEN];
};

DWORD	cmd_getboxinsersecArc(sds_point *txtbox,sds_real anglec,sds_point txtins,sds_point center,sds_real radius,sds_real sAngle,sds_real eAngle,sds_point *intpt,sds_point *nbox);
BOOL	cmd_getboxintersec(sds_point *txtbox,sds_real anglec,sds_point mpt,sds_point pt1,sds_point pt2,sds_point *intpt,sds_point *nbox,short *FirstLVal,short *SecondLVal);

// EBATECH(watanbe)-[
BOOL
cmd_getboxintersecEx(
  sds_point     txtbox[],
  const double  angle,
  sds_point     mpt,
  sds_point     pt1,
  sds_point     pt2,
  sds_point     intpt[]
);
// ]-EBATECH(watanbe)

// EBATECH(CNBR) 2003/3/31 Support lineweight
int	cmd_getCelweight(void);
int		cmd_dimaddline(char *szLayer, short sColor,short sLweight, sds_point pt10,sds_point pt11,db_drawing *pFlp,short dragmode,short flags = 0);
int		cmd_dimaddpoint(char *szLayer, short sColor,short sLweight,sds_point pt10,db_drawing *pFlp,short dragmode);
BOOL	cmd_defaultdim(struct cmd_dimeparlink *plink,db_drawing *flp,short dragmode);
BOOL	cmd_Ordinate(struct cmd_dimeparlink *plink,db_drawing *flp,short dragmode);
BOOL	cmd_Angular_A3P(struct cmd_dimeparlink *plink,db_drawing *flp,short dragmode);
BOOL	cmd_Diam_Radius(struct cmd_dimeparlink *plink,db_drawing *flp,short dragmode);
//void	cmd_dimtoucs(sds_point oragin,sds_point ucspoint,sds_point dimpoint,sds_real sinofang,sds_real cosofang);
void	cmd_ucstodim(sds_point oragin,sds_point ucspoint,sds_point dimpoint,sds_real sinofang,sds_real cosofang);
BOOL	cmd_drawbox(short dimclrd,short dimlwd, sds_point *xpt,db_drawing *flp,short dragmode);
BOOL	cmd_MakeItemHead(sds_point pt[4],char *ItemName,db_drawing *flp,short dragmode,sds_real sinofdeg,sds_real cosofdeg,sds_real rotAng, BOOL dirflag,BOOL sideflag,sds_point ptdef,int color);
void	cmd_dimtoucs(sds_point oragin,sds_point ucspoint,sds_point dimpoint,sds_real sinofang,sds_real cosofang);
void	cmd_ucstodim(sds_point oragin,sds_point ucspoint,sds_point dimpoint,sds_real sinofang,sds_real cosofang);
void	cmd_DimCalcAngle(BOOL FlipDimXaxis,BOOL SideofTextDrag,sds_real *fr2);
short	cmd_DimLinearFunc(short mode,db_drawing *flp);
short	cmd_DimContinueFunc(short mode,db_drawing *flp);
short	cmd_dimordinate(db_drawing *flp);
short	cmd_dimangular(BOOL mode,db_drawing *flp);
short	cmd_DimArcOrCircleFunc(short mode,db_drawing *flp);
short	cmd_DimEditFunc(short mode);
short	cmd_DimStyleFunc(short mode,db_drawing *flp);
long	cmd_dimstyle_list(void);
short	cmd_DrawLeader(int iDoTrueLeaderEnt);
void	cmd_setparlink(db_drawing *flp);
sds_real cmd_CalculateAngle(sds_point center,sds_point firstver,sds_point secondver);
int		cmd_compare1(const void *arg1,const void *arg2);
void	cmd_initialize(bool bReset = false);
short	cmd_doverride(db_drawing *flp);
void	cmd_DimCAngle_Fit4(BOOL FlipDimXaxis,BOOL SideofTextDrag,sds_real *fr1,sds_real fr2);
void	cmd_DimCAngle_FitText(BOOL FlipDimXaxis,BOOL SideofTextDrag,sds_real *fr1,sds_real fr2);
void	cmd_CenterMark(sds_point vertex,db_drawing *flp,short dragmode);
void	cmd_savelast(struct cmd_dimeparlink *ptr,struct gr_view *view);
int		cmd_dimaddinsert(char *szLayer,short sColor,short sLweight,sds_point pt10, sds_real rScale, sds_real rRotAngle, char *Iname,db_drawing *pFlp,short dragmode);

// EBATECH(watanabe)-[copy from old 'dimensions.cpp' file
void cmd_genstring(struct cmd_dimeparlink *plink,char *text,sds_real minwidth,LPCTSTR ext,short mode,db_drawing *flp);
void xformpt(sds_point from, sds_point to, sds_matrix mat);

// =========================================================================================================================================
// ================================================================ Defines ================================================================
// =========================================================================================================================================
#define GC_DIMJUST      280
#define GC_DIMTMOVE     279

#define DIM_ONE_SIXTH 0.1666666666667
#define DIM_COSPI4DEG     0.7071067811865
#define DIM_SINPI4DEG     -(0.7071067811865)
#define DIM_15_DEG        0.2617993877991
#define DIM_345_DEG       6.02138591938
#define DIM_90_DEG        1.570796326795
#define DIM_270_DEG       4.712388980385
#define DIM_165_DEG       2.879793265791
#define DIM_195_DEG       3.403392041389
#define DIM_2_PI          6.28318530718

#define DIM_ROT_HOR_VER   0
#define DIM_ALIGNED       1
#define DIM_ANGULAR       2
#define DIM_DIAMETER      3
#define DIM_RADIUS        4
#define DIM_ANGULAR_3P    5
//BugZilla No. 78153; 03-06-2002
//#define DIM_x_ORDINATE    6
//#define DIM_Y_ORDINATE    70
#define DIM_Y_ORDINATE    6
#define DIM_x_ORDINATE    70
#define DIM_XTYPEORD_DEF  64
#define DIM_XTYPEORD_UDEF 192
#define DIM_TEXT_MOVED    128
#define DIM_BIT_6         32

#define DIM_DEFPOINTS     "DEFPOINTS"/*DNT*/
#define DIM_LAYER_0       "0"/*DNT*/
#define DIM_ACADAPP       "ACAD"/*DNT*/
#define DIM_DSTYLE        "DSTYLE"/*DNT*/
#define DIM_STANDARD      "STANDARD"/*DNT*/

#define DIM_NONE          "_NONE"/*DNT*/
#define DIM_CLOSED        "_CLOSED"/*DNT*/
#define DIM_DOT           "_DOT"/*DNT*/
#define DIM_CLOSED_FILLED ""/*DNT*/
#define DIM_CLOSED_BLANK  "_CLOSEDBLANK"/*DNT*/
#define DIM_OBLIQUE       "_OBLIQUE"/*DNT*/
#define DIM_ARCH_TICK     "_ARCHTICK"/*DNT*/
#define DIM_OPEN          "_OPEN"/*DNT*/
#define DIM_ORIGIN_IND    "_ORIGIN"/*DNT*/
#define DIM_RIGHT_ANGLE   "_OPEN90"/*DNT*/
#define DIM_OPEN_30       "_OPEN30"/*DNT*/
#define DIM_DOT_BLANKED   "_DOTBLANK"/*DNT*/
#define DIM_DOT_SMALL     "_DOTSMALL"/*DNT*/
#define DIM_BOX_FILLED    "_BOXFILLED"/*DNT*/
#define DIM_BOX           "_BOXBLANK"/*DNT*/
#define DIM_DATUMFILLED   "_DATUMFILLED"/*DNT*/
#define DIM_DATUMBLANK    "_DATUMBLANK"/*DNT*/
#define DIM_INTEGRAL      "_INTEGRAL"/*DNT*/

#define	X	SDS_X
#define	Y	SDS_Y

//BugZilla No. 78155; 27-05-2002 [
#define lastdimpts SDS_CURDOC->lastdimpts
// =========================================================================================================================================
// =========================================================== Global structure's ==========================================================
// =========================================================================================================================================

//BugZilla No. 78155; 27-05-2002
//Moved this global structure, now is part of CIcadDoc
/*struct cmd_dimlastpt {

	/********************************************************************************
	 * Author:	Dmitry Gavrilov.
	 * Purpose:	Safe initialization constructor.
	 * Returns:	None.
	 ******************************************************************************* /
	cmd_dimlastpt()
	{
		ename[0] = ename[1] = 0L;
		r50 = r52 = elev = pt10[0] = pt10[1] = pt10[2] = 0.0;
		DB_PTCPY(pt13, pt10);
		DB_PTCPY(pt14, pt10);
		DB_PTCPY(pt15, pt10);
		DB_PTCPY(pt16, pt10);
		DB_PTCPY(pt210, pt10);
		DB_PTCPY(startpt, pt10);
		DB_PTCPY(startptother, pt10);
		DB_PTCPY(vertex, pt10);
		DB_PTCPY(ucsaxis[0], pt10);
		DB_PTCPY(ucsaxis[1], pt10);
		DB_PTCPY(ucsaxis[2], pt10);
		DB_PTCPY(ucsorg, pt10);
		pt210[2] = ucsaxis[0][0] = ucsaxis[1][1] = ucsaxis[2][2] = 1.0;
	}

    sds_name  ename;
    sds_real  r50,
              r52;
    int       i70;
    sds_point pt10,
              pt13,
              pt14,
              pt15,
              pt16,
              pt210,
              startpt,
              startptother,
              vertex;
    sds_point ucsaxis[3],
              ucsorg;
    sds_real  elev;
};
// ]-EBATECH(watanabe)
*/
//BugZilla No. 78155; 27-05-2002 ]

//BugZilla No. 78197 ; 01-07-2002
double
GetUpperDimgap(
);

#endif
