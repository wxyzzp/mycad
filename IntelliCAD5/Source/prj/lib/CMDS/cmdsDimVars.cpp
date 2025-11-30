#include "cmds.h"/*DNT*/
#include "cmdsDimVars.h"
#include "IcadApi.h"

// DP: obsolete
//BOOL useStoredValues = FALSE;
//BOOL loadDefaults = TRUE;
char* layername = NULL;

// SystemMetrix(Hiro)-[Change the method of having data of DIMLDRBLK in DIMSTYLE and in EED(add)
static char strClosedFilled[] = "";
// ]-SystemMetrix(Hiro) Change the method of having data of DIMLDRBLK in DIMSTYLE and in EED(add)

// Indexes of plinkvar array are defined as constants in cmdsDimVars.h.

cmd_dimvars plinkvar[]=    {
 /* 0*/			{"DIMADEC"/*DNT*/ ,179,IDC_DIMENSIONS_DIMADEC ,RTSHORT, {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},	// 0
 /* 1*/			{"DIMALT"/*DNT*/  ,170,IDC_DIMENSIONS_DIMALT  ,RTSHORT, {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /* 2*/			{"DIMALTD"/*DNT*/ ,171,IDC_DIMENSIONS_DIMALTD ,RTSHORT, {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /* 3*/			{"DIMALTF"/*DNT*/ ,143,IDC_DIMENSIONS_DIMALTF ,RTREAL , {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /* 4*/			{"DIMALTTD"/*DNT*/,274,IDC_DIMENSIONS_DIMALTTD,RTSHORT, {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /* 5*/			{"DIMALTTZ"/*DNT*/,286,IDC_DIMENSIONS_DIMALTTZ,RTSHORT, {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /* 6*/			{"DIMALTU"/*DNT*/ ,273,IDC_DIMENSIONS_DIMALTU ,RTSHORT, {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /* 7*/			{"DIMALTZ"/*DNT*/ ,285,IDC_DIMENSIONS_DIMALTZ ,RTSHORT, {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /* 8*/			{"DIMAPOST"/*DNT*/,  4,IDC_DIMENSIONS_DIMAPOST,RTSTR  , {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /* 9*/			{"DIMASO"/*DNT*/  , 61,IDC_DIMENSIONS_DIMASO  ,RTSHORT, {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*10*/			{"DIMASZ"/*DNT*/  , 41,IDC_DIMENSIONS_DIMASZ  ,RTREAL , {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*11*/			{"DIMAUNIT"/*DNT*/,275,IDC_DIMENSIONS_DIMAUNIT,RTSHORT, {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*12*/			{"DIMBLK"/*DNT*/  ,  5,IDC_DIMENSIONS_DIMBLK  ,RTSTR  , {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*13*/			{"DIMBLK1"/*DNT*/ ,  6,IDC_DIMENSIONS_DIMBLK1 ,RTSTR  , {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*14*/			{"DIMBLK2"/*DNT*/ ,  7,IDC_DIMENSIONS_DIMBLK2 ,RTSTR  , {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*15*/			{"DIMCEN"/*DNT*/  ,141,IDC_DIMENSIONS_DIMCEN  ,RTREAL , {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*16*/			{"DIMCLRD"/*DNT*/ ,176,IDC_DIMENSIONS_DIMCLRD ,RTSHORT, {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*17*/			{"DIMCLRE"/*DNT*/ ,177,IDC_DIMENSIONS_DIMCLRE ,RTSHORT, {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*18*/			{"DIMCLRT"/*DNT*/ ,178,IDC_DIMENSIONS_DIMCLRT ,RTSHORT, {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*19*/			{"DIMDEC"/*DNT*/  ,271,IDC_DIMENSIONS_DIMDEC  ,RTSHORT, {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*20*/			{"DIMDLE"/*DNT*/  , 46,IDC_DIMENSIONS_DIMDLE  ,RTREAL , {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*21*/			{"DIMDLI"/*DNT*/  , 43,IDC_DIMENSIONS_DIMDLI  ,RTREAL , {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*22*/			{"DIMEXE"/*DNT*/  , 44,IDC_DIMENSIONS_DIMEXE  ,RTREAL , {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*23*/			{"DIMEXO"/*DNT*/  , 42,IDC_DIMENSIONS_DIMEXO  ,RTREAL , {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*24*/			{"DIMFIT"/*DNT*/  ,287,IDC_DIMENSIONS_DIMFIT  ,RTSHORT, {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*25*/			{"DIMGAP"/*DNT*/  ,147,IDC_DIMENSIONS_DIMGAP  ,RTREAL , {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*26*/			{"DIMJUST"/*DNT*/ ,280,IDC_DIMENSIONS_DIMJUST ,RTSHORT, {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*27*/			{"DIMLDRBLK"/*DNT*/,341,IDC_DIMENSIONS_DIMLDRBLK,RTSTR, {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*28*/			{"DIMLFAC"/*DNT*/ ,144,IDC_DIMENSIONS_DIMLFAC ,RTREAL , {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*29*/			{"DIMLIM"/*DNT*/  , 72,IDC_DIMENSIONS_DIMLIM  ,RTSHORT, {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*30*/			{"DIMPOST"/*DNT*/ ,  3,IDC_DIMENSIONS_DIMPOST ,RTSTR  , {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*31*/			{"DIMRND"/*DNT*/  , 45,IDC_DIMENSIONS_DIMRND  ,RTREAL , {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*32*/			{"DIMSAH"/*DNT*/  ,173,IDC_DIMENSIONS_DIMSAH  ,RTSHORT, {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*33*/			{"DIMSCALE"/*DNT*/, 40,IDC_DIMENSIONS_DIMSCALE,RTREAL , {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*34*/			{"DIMSD1"/*DNT*/  ,281,IDC_DIMENSIONS_DIMSD1  ,RTSHORT, {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*35*/			{"DIMSD2"/*DNT*/  ,282,IDC_DIMENSIONS_DIMSD2  ,RTSHORT, {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*36*/			{"DIMSE1"/*DNT*/  , 75,IDC_DIMENSIONS_DIMSE1  ,RTSHORT, {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*37*/			{"DIMSE2"/*DNT*/  , 76,IDC_DIMENSIONS_DIMSE2  ,RTSHORT, {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*38*/			{"DIMSHO"/*DNT*/  , 89,IDC_DIMENSIONS_DIMSHO  ,RTSHORT, {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*39*/			{"DIMSOXD"/*DNT*/ ,175,IDC_DIMENSIONS_DIMSOXD ,RTSHORT, {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*40*/			{"DIMSTYLE"/*DNT*/,  2,IDC_DIMENSIONS_DIMSTYLE,RTSTR  , {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*41*/			{"DIMTAD"/*DNT*/  , 77,IDC_DIMENSIONS_DIMTAD  ,RTSHORT, {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*42*/			{"DIMTDEC"/*DNT*/ ,272,IDC_DIMENSIONS_DIMTDEC ,RTSHORT, {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*43*/			{"DIMTFAC"/*DNT*/ ,146,IDC_DIMENSIONS_DIMTFAC ,RTREAL , {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*44*/			{"DIMTIH"/*DNT*/  , 73,IDC_DIMENSIONS_DIMTIH  ,RTSHORT, {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*45*/			{"DIMTIX"/*DNT*/  ,174,IDC_DIMENSIONS_DIMTIX  ,RTSHORT, {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*46*/			{"DIMTM"/*DNT*/   , 48,IDC_DIMENSIONS_DIMTM   ,RTREAL , {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*47*/			{"DIMTOFL"/*DNT*/ ,172,IDC_DIMENSIONS_DIMTOFL ,RTSHORT, {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*48*/			{"DIMTOH"/*DNT*/  , 74,IDC_DIMENSIONS_DIMTOH  ,RTSHORT, {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*49*/			{"DIMTOL"/*DNT*/  , 71,IDC_DIMENSIONS_DIMTOL  ,RTSHORT, {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*50*/			{"DIMTOLJ"/*DNT*/ ,283,IDC_DIMENSIONS_DIMTOLJ ,RTSHORT, {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*51*/			{"DIMTP"/*DNT*/   , 47,IDC_DIMENSIONS_DIMTP   ,RTREAL , {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*52*/			{"DIMTSZ"/*DNT*/  ,142,IDC_DIMENSIONS_DIMTSZ  ,RTREAL , {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*53*/			{"DIMTVP"/*DNT*/  ,145,IDC_DIMENSIONS_DIMTVP  ,RTREAL , {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*54*/			{"DIMTXSTY"/*DNT*/,340,IDC_DIMENSIONS_DIMTXSTY,RTSTR  , {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*55*/			{"DIMTXT"/*DNT*/  ,140,IDC_DIMENSIONS_DIMTXT  ,RTREAL , {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*56*/			{"DIMZIN"/*DNT*/ , 78,IDC_DIMENSIONS_DIMTZIN ,RTSHORT, {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*57*/			{"DIMUNIT"/*DNT*/ ,270,IDC_DIMENSIONS_DIMUNIT ,RTSHORT, {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*58*/			{"DIMUPT"/*DNT*/  ,288,IDC_DIMENSIONS_DIMUPT  ,RTSHORT, {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*59*/			{"DIMTZIN"/*DNT*/  ,284,IDC_DIMENSIONS_DIMZIN  ,RTSHORT, {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*60*/			{"DIMFRAC"/*DNT*/ ,276,IDC_DIMENSIONS_DIMFRAC ,RTSHORT, {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*61*/			{"DIMLUNIT"/*DNT*/,277,IDC_DIMENSIONS_DIMLUNIT,RTSHORT, {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*62*/			{"DIMATFIT"/*DNT*/,289,IDC_DIMENSIONS_DIMATFIT,RTSHORT, {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*63*/			{"DIMTMOVE"/*DNT*/,279,IDC_DIMENSIONS_DIMTMOVE,RTSHORT, {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*64*/			{"DIMAZIN"/*DNT*/ , 79,IDC_DIMENSIONS_DIMAZIN ,RTSHORT, {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*65*/			{"DIMALTRND"/*DNT*/ ,148,IDC_DIMENSIONS_DIMALTRND ,RTREAL, {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*66*/			{"DIMDSEP"/*DNT*/ ,278,IDC_DIMENSIONS_DIMDSEP ,RTSHORT, {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*67*/			{"DIMLWD"/*DNT*/ , 371,IDC_DIMENSIONS_DIMLWD ,RTSHORT, {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}},
 /*69*/			{"DIMLWE"/*DNT*/ , 372,IDC_DIMENSIONS_DIMLWE ,RTSHORT, {FALSE, 0.0, 0.0, 0, 0, NULL, NULL}}
};

// =======================================================================================================================================
// ========================================================= GENERIC FUNCTIONS ===========================================================
// =======================================================================================================================================
//
// This is for all generic functions, these function are called from any code block, and any dimension type.
//
// =======================================================================================================================================

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DIM_GETEEDVALUES
//
//
//
// return : TRUE on success.
//        : FALSE on fail.
//
void cmd_geteedvalues(db_handitem *elp,db_drawing *flp) {
    sds_name       ename = {0L, 0L};

	if( flp->ret_pn() == NULL )
		return;				// To prevent from reading plinkvars from a dummy drawing

	resbuf		  *elist=NULL,
                  *estyle=NULL,
                  *rb=NULL,
                  *rbt=NULL,
                   appname;
    int            type = 0;
    short          fi1 = 0;
    db_handitem   *handitem=NULL;

    appname.restype=RTSTR;
	appname.resval.rstring=DIM_ACADAPP;
	appname.rbnext=NULL;

	ename[0]=(long)elp;
	ename[1]=(long)flp;
    if ((elist=sds_entgetx(ename,&appname))==NULL)
		goto out;
    if (SDS_AtNodeDrag)
	{
        for (rb=elist; rb && rb->restype!=3; rb=rb->rbnext);
        if (rb && rb->resval.rstring) 
			{
            estyle=sds_tblsearch("DIMSTYLE"/*DNT*/,rb->resval.rstring,0);
            for (rbt=estyle; rbt; rbt=rbt->rbnext) 
				{
                for (fi1=0;fi1<DIM_LASTDIMVAR;++fi1) 
					{
					if (plinkvar[fi1].DXF==rbt->restype)
						break; 
					}
                if (fi1<DIM_LASTDIMVAR) 
					{
                    switch(plinkvar[fi1].type) 
						{
                        case RTSTR:
                            if (rbt->restype==340) 
								{
                                db_handitem*	handitem = (db_handitem *)((long)rbt->resval.rlname[0]);
                                char*		string = NULL;
                                handitem->get_2(&string);

                                if ((!plinkvar[fi1].Var && plinkvar[fi1].dval.cval) ||
                                    (plinkvar[fi1].Var && !plinkvar[fi1].dval.cval) || 
                                    (plinkvar[fi1].dval.cval && (stricmp(string,plinkvar[fi1].dval.cval)))) 
									{
                                    if (plinkvar[fi1].dval.cval)
										//Bugzilla No. 78104 ; 15-04-2002 
										delete[] plinkvar[fi1].dval.cval;
                                    plinkvar[fi1].dval.cval=(char *)new char[(strlen(string)+1)];
                                    strcpy(plinkvar[fi1].dval.cval,string);
                                    
									}
								}
                            // SystemMetrix(Hiro)-[Change the method of having data of DIMLDRBLK in DIMSTYLE and in EED(add)
                            else if (rbt->restype == 341)
                              {
                                // DIMLDRBLK

                                char* string = NULL;
                                db_handitem* handitem = (db_handitem *)((long)rbt->resval.rlname[0]);
                                if (handitem)
                                  {
                                    handitem->get_2(&string);
                                  }
                                else
                                  {
                                    // ClosedFilled
                                    string = strClosedFilled;
                                  }

                                if ((!plinkvar[fi1].Var &&  plinkvar[fi1].dval.cval) ||
                                    ( plinkvar[fi1].Var && !plinkvar[fi1].dval.cval) ||
                                    (plinkvar[fi1].dval.cval && (stricmp(string,plinkvar[fi1].dval.cval))))
                                  {
                                    if (plinkvar[fi1].dval.cval)
                                      {
                                        //Bugzilla No. 78104 ; 15-04-2002 
										delete[] plinkvar[fi1].dval.cval;
                                      }
                                    plinkvar[fi1].dval.cval = (char*)new char[(strlen(string) + 1)];
                                    strcpy(plinkvar[fi1].dval.cval, string);
                                  }
                              }
                            // ]-SystemMetrix(Hiro) Change the method of having data of DIMLDRBLK in DIMSTYLE and in EED(add)
							else 
								{
                                if ((!plinkvar[fi1].Var && plinkvar[fi1].dval.cval) ||
                                    (plinkvar[fi1].Var && !plinkvar[fi1].dval.cval) || 
									(plinkvar[fi1].dval.cval && (stricmp(rbt->resval.rstring,plinkvar[fi1].dval.cval)))) 
									{
                                    if (plinkvar[fi1].dval.cval) 
										//Bugzilla No. 78104 ; 15-04-2002 
										delete[] plinkvar[fi1].dval.cval;
                                    plinkvar[fi1].dval.cval=(char *)new char[(strlen(rbt->resval.rstring)+1)];
                                    strcpy(plinkvar[fi1].dval.cval,rbt->resval.rstring);
									}
								}
                            break;
                        case RTREAL:
                            if (fi1==32 && icadRealEqual(rbt->resval.rreal,0.0))  // do not override dimscale to 0.0
								rbt->resval.rreal=1.0;
							if (rbt->resval.rreal!=plinkvar[fi1].dval.rval) 
								plinkvar[fi1].dval.rval=rbt->resval.rreal;
                            break;
                        case RTSHORT:
                            if (rbt->resval.rint!=plinkvar[fi1].dval.ival) 
								plinkvar[fi1].dval.ival=rbt->resval.rint;
                            break;
						}
					}
				}
			}
		}

    for (rb=elist; rb && rb->restype!=-3; rb=rb->rbnext);
    if (!rb)
		goto out;
    for (rb=rb->rbnext; rb; rb=rb->rbnext)
		{
        if (rb->restype==AD_XD_STRING) 
			{
            if (strsame(rb->resval.rstring,DIM_DSTYLE))
				break;
			}
		}
    if (!rb)
		goto out;
    for (rb=rb->rbnext; rb && rb->restype!=AD_XD_CONTROL; rb=rb->rbnext); // Changed == to !=
    if (!rb)
		goto out;
    for (rb=rb->rbnext; rb; rb=rb->rbnext) 
		{
        type=rb->resval.rint;
        if (rb->rbnext)
			rb=rb->rbnext;
		else 
			break;
        for (fi1=0;fi1<DIM_LASTDIMVAR;++fi1) 
			{
            if (type==plinkvar[fi1].DXF) 
				{
                if (!plinkvar[fi1].dval.oride) 
					{
                    switch(plinkvar[fi1].type) 
						{
                        case RTSTR:
                            if (type==plinkvar[DIMTXSTY].DXF) 
								{
                                if (sds_handent(rb->resval.rstring,ename)==RTNORM) 
									{
                                    char *string = NULL;
                                    handitem=(db_handitem *)ename[0];
                                    handitem->get_2(&string);
                                    if (plinkvar[fi1].dval.cval && string != NULL) 
										{
                                        //Bugzilla No. 78104 ; 15-04-2002 
										delete[] plinkvar[fi1].dval.cval;
                                        plinkvar[fi1].dval.cval=(char *)new char[(strlen(string)+1)];
                                        strcpy(plinkvar[fi1].dval.cval,string);
										}
									}
								}
                            // SystemMetrix(Hiro)-[Change the method of having data of DIMLDRBLK in DIMSTYLE and in EED(add)
                            else if (type == plinkvar[DIMLDRBLK].DXF)
                              {
                                // Modified Cybage VSB 04/03/2002 
								// Reason: Fix for the bug 77969 from Bugzilla
								// if ( sds_handent(rb->resval.rstring, ename) == RTNORM)
								if (!strsame(rb->resval.rstring, "") && sds_handent(rb->resval.rstring, ename) == RTNORM)
                                  {
                                    char* string = NULL;
                                    handitem = (db_handitem*)ename[0];
                                    if (handitem)
                                      {
                                        handitem->get_2(&string);
                                      }
                                    else
                                      {
                                        string = strClosedFilled;
                                      }

                                    if (plinkvar[fi1].dval.cval && string != NULL)
                                      {
                                        //Bugzilla No. 78104 ; 15-04-2002 
										delete[] plinkvar[fi1].dval.cval;
                                        plinkvar[fi1].dval.cval = (char*)new char[(strlen(string)+1)];
                                        strcpy(plinkvar[fi1].dval.cval, string);
                                      }
                                  }
                              }
                            // ]-SystemMetrix(Hiro) Change the method of having data of DIMLDRBLK in DIMSTYLE and in EED(add)
							else 
								{
                                if (plinkvar[fi1].dval.cval) 
									//Bugzilla No. 78104 ; 15-04-2002 
									delete[] plinkvar[fi1].dval.cval;
                                plinkvar[fi1].dval.cval=(char *) new char[strlen(rb->resval.rstring)+1];
                                strcpy(plinkvar[fi1].dval.cval,rb->resval.rstring);
	                            }
                            break;
                        case RTREAL:
							if (fi1==DIMSCALE && icadRealEqual(rb->resval.rreal,0.0))  // do not override dimscale to 0.0
								rb->resval.rreal=1.0;
							plinkvar[fi1].dval.rval=rb->resval.rreal;
							break;
                        case RTSHORT:
							plinkvar[fi1].dval.ival =rb->resval.rint;
							break;
		                }
			        }
				else
					plinkvar[fi1].dval.oride=FALSE;
				}
			}
		}

  // EBATECH(watanabe)-[update dimfit from eed (dimoverride)
  int fit, atfit, tmove;
  atfit = plinkvar[DIMATFIT].dval.ival;
  tmove = plinkvar[DIMTMOVE].dval.ival;
  fit = atfit;
  if (tmove == 1)
    {
      fit = 4;
    }
  else if (tmove == 2)
    {
      fit = 5;
    }
  plinkvar[DIMFIT].dval.ival = fit;
  // ]-EBATECH(watanabe)

  // EBATECH(watanabe)-[dimuint dimfit from eed (dimoverride)
  int unit, lunit, frac;
  lunit = plinkvar[DIMLUNIT].dval.ival;
  frac  = plinkvar[DIMFRAC].dval.ival;
  unit = lunit;
  switch (unit)
    {
      case 4:
      case 5:
          if (frac == 2)
            {
              unit += 2;
            }
          break;
      case 6:
          unit += 2;
          break;
    }
  plinkvar[DIMUNIT].dval.ival = unit;
  // ]-EBATECH(watanabe)

out: ;
	IC_RELRB(elist);		//  was: 	if(elist) sds_relrb(elist);
	IC_RELRB(estyle);		//  was: 	if(elist) sds_relrb(elist);
}

// SystemMetrix(Hiro)-[Change the method of having data of DIMLDRBLK in DIMSTYLE and in EED
//////////////////////////////////////////////////////////////////////
// DIM_BUILDEEDLIST for string
static
BOOL
addEed(
  const  char*		str,
  const  short      i,
  struct resbuf*&   rb,
  const  BOOL       bIsString = FALSE
)
{
  rb->rbnext = sds_newrb(AD_XD_INT);
  rb = rb->rbnext;
  rb->resval.rint = plinkvar[i].DXF;

  int type = XDATA_CODE_DATABHANDLE;
  if (bIsString)
    {
      type = AD_XD_STRING;
    }
  rb->rbnext = sds_newrb(type);
  rb = rb->rbnext;
  //Bugzilla No. 61474 ; 24-06-2002   
  rb->resval.rstring = new char [ strlen(str)+1 ];
  strcpy(rb->resval.rstring, str);

  return TRUE;
}

//////////////////////////////////////////////////////////////////////
// DIM_BUILDEEDLIST for string
static
BOOL
cmd_buildEEdStrList(
  struct resbuf*    rbl,
  const  short      i,
  struct resbuf*&   rb
)
{
  struct resbuf* tmprb;
  switch (rbl->restype)
    {
      // dimldrblk
      case 341:
        {
          static char tblname[] = "BLOCK"/*DNT*/;

          char* string = NULL;
          db_handitem* handitem = (db_handitem*)((long)rbl->resval.rlname[0]);
          if (handitem)
            {
              handitem->get_2(&string);
            }
          else
            {
              string = strClosedFilled;
            }

          if ((!plinkvar[i].Var && plinkvar[i].dval.cval) ||
              (plinkvar[i].Var && !plinkvar[i].dval.cval) ||
              (plinkvar[i].dval.cval && (stricmp(string, plinkvar[i].dval.cval))))
            {
              BOOL bClosedFilled = TRUE;
              tmprb = sds_tblnext(tblname, 1);
              while (tmprb)
                {
                  if (ic_assoc(tmprb, 2) == 0)
                    {
						// Make sure string values are not NULL otherwise stricmp will crash!
						if( (ic_rbassoc->resval.rstring) && 
							(plinkvar[i].dval.cval) && 
							(stricmp(ic_rbassoc->resval.rstring, plinkvar[i].dval.cval) == 0) 
							)
                        {
                          if (ic_assoc(tmprb, 5) == 0)
                            {
                              addEed(ic_rbassoc->resval.rstring, i, rb);

                              IC_RELRB(tmprb);
                              bClosedFilled = FALSE;
                              break;
                            }
                        }
                    }

                  IC_RELRB(tmprb);

                  // Do NOT check for NULL here.  NULL is checked in the while().
                  tmprb = sds_tblnext(tblname, 0);
                }

              if (bClosedFilled)
                {
                  addEed("0"/*DNT*/, i, rb);
                }
            }

          break;
        }

      default:
        {
          if ((!plinkvar[i].Var && plinkvar[i].dval.cval) ||
              //Bugzilla No. 61474 ; 24-06-2002  
			  //(plinkvar[i].dval.cval && *plinkvar[i].dval.cval && stricmp(rbl->resval.rstring,plinkvar[i].dval.cval)))
			  (plinkvar[i].dval.cval && stricmp(rbl->resval.rstring,plinkvar[i].dval.cval)))
            {
              addEed(plinkvar[i].dval.cval, i, rb, TRUE);
            }
          break;
        }
    }

  return TRUE;
}
// ]-SystemMetrix(Hiro) Change the method of having data of DIMLDRBLK in DIMSTYLE and in EED

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DIM_BUILDEEDLIST
//
// BOOL bReadDimStyle (i,optional=FALSE) TRUE=Seek DIMSTYLE(GC=3) and compair dim variables.
//                    FALSE or No DIMSTYLE so, Use current DIMSTYLE and compair dim variables.
//
// return : TRUE on success.
//        : FALSE on fail.
//
void cmd_buildEEdList(struct resbuf *link, BOOL bReadDimStyle ) {
    struct resbuf *rb=NULL,
                  *rbt=NULL,
                  *rbl=NULL,
                  *rbb=NULL,
                  *tmprb=NULL,
                  *elist=NULL,
                   setgetrb;
    short          fi1 = 0,
                   fi2 = 0;

    for (rbb=link; rbb->rbnext && rbb->rbnext->restype!=-3; rbb=rbb->rbnext);
    if (!rbb->rbnext || rbb->rbnext->restype!=-3) {
        rb=rbb->rbnext=sds_newrb(-3);
    } else {
        for (; rbb->rbnext; rbb=rbb->rbnext) { if (rbb->restype==AD_XD_APPOBJHANDLE && strisame(rbb->resval.rstring,DIM_ACADAPP)) break; }
        for (; rbb->rbnext; rbb=rbb->rbnext) { if (rbb->restype==AD_XD_STRING && strisame(rbb->resval.rstring,DIM_DSTYLE)) break; }
        for (rbt=rbb; rbt; rbt=rbt->rbnext) {
            if (rbb->rbnext && rbb->rbnext->restype==AD_XD_APPOBJHANDLE) {
                rbl=rbt; rbt=rbt->rbnext; rbl->rbnext=NULL;
                sds_relrb(rbb->rbnext);
                break;
            }
        }
        rb=rbb;
    }
    if (rbb->restype!=AD_XD_APPOBJHANDLE) {
        rb->rbnext=sds_newrb(AD_XD_APPOBJHANDLE); 
		rb=rb->rbnext;		
		rb->resval.rstring= new char[strlen(DIM_ACADAPP)+1];		
		strcpy(rb->resval.rstring,DIM_ACADAPP);

        rb->rbnext=sds_newrb(AD_XD_STRING);
		rb=rb->rbnext;		
		rb->resval.rstring= new char[strlen(DIM_DSTYLE)+1];		
		strcpy(rb->resval.rstring,DIM_DSTYLE);
    }
    rb->rbnext=sds_newrb(AD_XD_CONTROL);
	rb=rb->rbnext;	
	rb->resval.rstring= new char[2];	
	strcpy(rb->resval.rstring,"{"/*DNT*/);
	// EBATECH(CNBR) - For modify.
	if( bReadDimStyle && ic_assoc(link,3)==0){
		setgetrb.restype=RTSTR;
		setgetrb.resval.rstring= new char [strlen(ic_rbassoc->resval.rstring)+1];
		strcpy(setgetrb.resval.rstring,ic_rbassoc->resval.rstring);
	} else {
		if (SDS_getvar(NULL,DB_QDIMSTYLE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) return;
	}
	// if (SDS_getvar(NULL,DB_QDIMSTYLE,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) return;
	// EBATECH(CNBR) ]-
    elist=sds_tblsearch("DIMSTYLE"/*DNT*/,setgetrb.resval.rstring,0);
    for (fi2=0,rbl=elist; rbl; rbl=rbl->rbnext) {
        for (fi1=0;fi1<DIM_LASTDIMVAR;++fi1) { if (plinkvar[fi1].DXF==rbl->restype) break; }
        if (fi1<DIM_LASTDIMVAR) {
            switch(plinkvar[fi1].type) {
                case RTSTR:
                    if (rbl->restype==340) {
                        db_handitem*	handitem = (db_handitem *)((long)rbl->resval.rlname[0]);
                        char*   string = NULL;
                        handitem->get_2(&string);
                        if ((!plinkvar[fi1].Var && plinkvar[fi1].dval.cval) || 
							(plinkvar[fi1].Var && !plinkvar[fi1].dval.cval) || 
							(plinkvar[fi1].dval.cval && (stricmp(string,plinkvar[fi1].dval.cval)))) {
                            tmprb=sds_tblnext("STYLE"/*DNT*/,1);
                            while(tmprb) {
                                if(ic_assoc(tmprb, 2))
									return;
								/*DG - 5.12.2001*/// Ignore letters case (because plinkvar[fi1].dval.cval is in upper case).
								//if (sds_wcmatch(ic_rbassoc->resval.rstring,plinkvar[fi1].dval.cval)==RTNORM) {
								// Make sure string values are not NULL otherwise strisame will crash!
								if ((ic_rbassoc->resval.rstring) && (plinkvar[fi1].dval.cval) && (strisame(ic_rbassoc->resval.rstring, plinkvar[fi1].dval.cval))) {
                                    rb->rbnext=sds_newrb(AD_XD_INT);  rb=rb->rbnext; rb->resval.rint =plinkvar[fi1].DXF;
                                    if(ic_assoc(tmprb,5))
										return;
                                    rb->rbnext = sds_newrb(AD_XD_STRING);
									rb = rb->rbnext;																		
                                    rb->resval.rstring = new char [strlen(ic_rbassoc->resval.rstring) + 1];
                                    strcpy(rb->resval.rstring, ic_rbassoc->resval.rstring);
	                                IC_RELRB(tmprb);
                                    break;
                                }
                                // Do NOT check for NULL here.  NULL is checked in the while().
                                IC_RELRB(tmprb);
                                tmprb = sds_tblnext("STYLE"/*DNT*/, 0);
                            }
                       }

                    // SystemMetrix(Hiro)-[Change the method of having data of DIMLDRBLK in DIMSTYLE and in EED
                    } else {
                        cmd_buildEEdStrList(rbl, fi1, rb);
                    }
                    // ]-SystemMetrix(Hiro) Change the method of having data of DIMLDRBLK in DIMSTYLE and in EED

                    break;
                case RTREAL:
                    if (rbl->resval.rreal!=plinkvar[fi1].dval.rval) {
                        rb->rbnext=sds_newrb(AD_XD_INT);  rb=rb->rbnext; rb->resval.rint =plinkvar[fi1].DXF;
                        rb->rbnext=sds_newrb(AD_XD_REAL); rb=rb->rbnext; rb->resval.rreal=plinkvar[fi1].dval.rval;
                    }
                    break;
                case RTSHORT:
                    if (rbl->resval.rint!=plinkvar[fi1].dval.ival) {

                        // EBATECH(watanabe)-[dimfit, dimunit
                        if ((plinkvar[fi1].Var) &&
							((stricmp(plinkvar[fi1].Var, "DIMFIT")  == 0) ||
                             (stricmp(plinkvar[fi1].Var, "DIMUNIT") == 0))) {
                            // ignore DIMFIT and DIMUNIT.
                            // because DIMFIT replaced by DIMATFIT and DIMTMOVE,
                            // DIMUNIT replaced by DIMLUNIT and DIMFRAC.
                            break;
                        }
                        // ]-EBATECH(watanabe)

                        rb->rbnext=sds_newrb(AD_XD_INT);  rb=rb->rbnext; rb->resval.rint =plinkvar[fi1].DXF;
                        rb->rbnext=sds_newrb(AD_XD_INT); rb=rb->rbnext; rb->resval.rint=plinkvar[fi1].dval.ival;
                    }
                    break;
            }
        }
    }

//bail: ;

	IC_RELRB(elist);
    IC_FREE(setgetrb.resval.rstring);
    if (rb->restype==AD_XD_CONTROL) {
		sds_relrb(rbb->rbnext);  // goes in here if no dimstyle overrides found, it appears
		rbb->rbnext=NULL;
    } else {
        rb->rbnext=sds_newrb(AD_XD_CONTROL);
		rb=rb->rbnext;		
		rb->resval.rstring= new char[2];		
		strcpy(rb->resval.rstring,"}"/*DNT*/);
    }

    if (rbt) rb->rbnext=rbt;
}

/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Set dimensioning system variables with plinkvar's values.
 * Returns:	None.
 ********************************************************************************/
void cmd_dimSetVars()
{
	resbuf	rb;
	rb.rbnext = NULL;

	for(int i = DIM_LASTDIMVAR; i--; )
	{
		switch(rb.restype = plinkvar[i].type)
		{
		case RTSHORT :
			// EBATECH(CNBR) -[ DIMDSEP is unique. 
			if( plinkvar[i].DXF == 278 )
			{
				rb.restype = RTSTR;
				rb.resval.rstring = new char [2];
				rb.resval.rstring[0] = plinkvar[i].dval.ival;
				rb.resval.rstring[1] = 0;
			}
			// EBATECH(CNBR) ]-
			else
			{
				rb.resval.rint = plinkvar[i].dval.ival;
			}
			break;
		case RTREAL  :
			rb.resval.rreal = plinkvar[i].dval.rval;
			break;
		case RTSTR   :
			if(plinkvar[i].dval.cval)
			{
				rb.resval.rstring = new char [strlen(plinkvar[i].dval.cval) + 1];
				strcpy(rb.resval.rstring, plinkvar[i].dval.cval);
			}
			else
				rb.resval.rstring = "";
		}
		sds_setvar(plinkvar[i].Var, &rb);
		if(rb.restype == RTSTR)
			IC_FREE(rb.resval.rstring);
	}
}

void copyStrToPlinkVar(resbuf* rb, short id, bool doStrupr = true)
{
	plinkvar[id].dval.cval = (char *)new char[strlen(rb->resval.rstring)+1];
	if (doStrupr)
		strcpy(plinkvar[id].dval.cval, strupr(rb->resval.rstring));
	else
		strcpy(plinkvar[id].dval.cval, rb->resval.rstring);
	IC_FREE(rb->resval.rstring);
	rb->resval.rstring = NULL;
}

void cmd_setplinkvar(db_drawing *flp)
{
    resbuf			rbb;
    db_sysvarlink	*sysvar = NULL;
 
    short			fi1 = 0;

    //if (!loadDefaults) {
    if (FALSE) {
        sysvar=db_Getsysvar();
		//EBATECH(CNBR) Bugzilla78443 DIMADEC and DIMAZIN
		plinkvar[DIMADEC].dval.ioval   =atoi(sysvar[DB_QDIMADEC].defval);
		//plinkvar[DIMADEC].dval.ioval   =atoi(objsysvar[DB_QDIMADEC].defval);
        plinkvar[DIMALT].dval.ioval    =atoi(sysvar[DB_QDIMALT].defval);
        plinkvar[DIMALTD].dval.ioval   =atoi(sysvar[DB_QDIMALTD].defval);
        plinkvar[DIMALTF].dval.roval  =atof(sysvar[DB_QDIMALTF].defval);
        plinkvar[DIMALTTD].dval.ioval  =atoi(sysvar[DB_QDIMALTTD].defval);
        plinkvar[DIMALTTZ].dval.ioval  =atoi(sysvar[DB_QDIMALTTZ].defval);
        plinkvar[DIMALTU].dval.ioval   =atoi(sysvar[DB_QDIMALTU].defval);
        plinkvar[DIMALTZ].dval.ioval   =atoi(sysvar[DB_QDIMALTZ].defval);
        plinkvar[DIMAPOST].dval.coval =sysvar[DB_QDIMAPOST].defval;
        plinkvar[DIMASO].dval.ioval    =atoi(sysvar[DB_QDIMASO].defval);
        plinkvar[DIMASZ].dval.roval   =atof(sysvar[DB_QDIMASZ].defval);
        // EBATECH(watanabe)-[dimatfit
        plinkvar[DIMATFIT].dval.ioval  =atoi(sysvar[DB_QDIMATFIT].defval);
        // ]-EBATECH(watanabe)
        plinkvar[DIMAUNIT].dval.ioval  =atoi(sysvar[DB_QDIMAUNIT].defval);
        plinkvar[DIMBLK].dval.coval   =sysvar[DB_QDIMBLK].defval;
        plinkvar[DIMBLK1].dval.coval  =sysvar[DB_QDIMBLK1].defval;
        plinkvar[DIMBLK2].dval.coval  =sysvar[DB_QDIMBLK2].defval;
        plinkvar[DIMCEN].dval.roval   =atof(sysvar[DB_QDIMCEN].defval);
        plinkvar[DIMCLRD].dval.ioval   =atoi(sysvar[DB_QDIMCLRD].defval);
        plinkvar[DIMCLRE].dval.ioval   =atoi(sysvar[DB_QDIMCLRE].defval);
        plinkvar[DIMCLRT].dval.ioval   =atoi(sysvar[DB_QDIMCLRT].defval);
        plinkvar[DIMDEC].dval.ioval    =atoi(sysvar[DB_QDIMDEC].defval);
        plinkvar[DIMDLE].dval.roval   =atof(sysvar[DB_QDIMDLE].defval);
        plinkvar[DIMDLI].dval.roval   =atof(sysvar[DB_QDIMDLI].defval);
        plinkvar[DIMEXE].dval.roval   =atof(sysvar[DB_QDIMEXE].defval);
        plinkvar[DIMEXO].dval.roval   =atof(sysvar[DB_QDIMEXO].defval);
        plinkvar[DIMFIT].dval.ioval    =atoi(sysvar[DB_QDIMFIT].defval);
        // EBATECH(watanabe)-[dimfrac
        plinkvar[DIMFRAC].dval.ioval   =atoi(sysvar[DB_QDIMFRAC].defval);
        // ]-EBATECH(watanabe)
        plinkvar[DIMGAP].dval.roval   =atof(sysvar[DB_QDIMGAP].defval);
        plinkvar[DIMJUST].dval.ioval   =atoi(sysvar[DB_QDIMJUST].defval);
        // EBATECH(watanabe)-[dimldrblk
        plinkvar[DIMLDRBLK].dval.coval=sysvar[DB_QDIMLDRBLK].defval;
        // ]-EBATECH(watanabe)
        plinkvar[DIMLFAC].dval.roval  =atof(sysvar[DB_QDIMLFAC].defval);
        plinkvar[DIMLIM].dval.ioval    =atoi(sysvar[DB_QDIMLIM].defval);
        // EBATECH(watanabe)-[dimlunit
        plinkvar[DIMLUNIT].dval.ioval  =atoi(sysvar[DB_QDIMLUNIT].defval);
        // ]-EBATECH(watanabe)
        plinkvar[DIMPOST].dval.coval  =sysvar[DB_QDIMPOST].defval;
        plinkvar[DIMRND].dval.roval   =atof(sysvar[DB_QDIMRND].defval);
        plinkvar[DIMSAH].dval.ioval    =atoi(sysvar[DB_QDIMSAH].defval);
        plinkvar[DIMSCALE].dval.roval =atof(sysvar[DB_QDIMSCALE].defval);
        plinkvar[DIMSD1].dval.ioval    =atoi(sysvar[DB_QDIMSD1].defval);
        plinkvar[DIMSD2].dval.ioval    =atoi(sysvar[DB_QDIMSD2].defval);
        plinkvar[DIMSE1].dval.ioval    =atoi(sysvar[DB_QDIMSE1].defval);
        plinkvar[DIMSE2].dval.ioval    =atoi(sysvar[DB_QDIMSE2].defval);
        plinkvar[DIMSHO].dval.ioval    =atoi(sysvar[DB_QDIMSHO].defval);
        plinkvar[DIMSOXD].dval.ioval   =atoi(sysvar[DB_QDIMSOXD].defval);
        plinkvar[DIMSTYLE].dval.coval =sysvar[DB_QDIMSTYLE].defval;
        plinkvar[DIMTAD].dval.ioval    =atoi(sysvar[DB_QDIMTAD].defval);
        plinkvar[DIMTDEC].dval.ioval   =atoi(sysvar[DB_QDIMTDEC].defval);
        plinkvar[DIMTFAC].dval.roval  =atof(sysvar[DB_QDIMTFAC].defval);
        plinkvar[DIMTIH].dval.ioval    =atoi(sysvar[DB_QDIMTIH].defval);
        plinkvar[DIMTIX].dval.ioval    =atoi(sysvar[DB_QDIMTIX].defval);
        plinkvar[DIMTM].dval.roval    =atof(sysvar[DB_QDIMTM].defval);
        // EBATECH(watanabe)-[dimtmove
        plinkvar[DIMTMOVE].dval.ioval =atoi(sysvar[DB_QDIMTMOVE].defval);
        // ]-EBATECH(watanabe)
        plinkvar[DIMTOFL].dval.ioval   =atoi(sysvar[DB_QDIMTOFL].defval);
        plinkvar[DIMTOH].dval.ioval    =atoi(sysvar[DB_QDIMTOH].defval);
        plinkvar[DIMTOL].dval.ioval    =atoi(sysvar[DB_QDIMTOL].defval);
        plinkvar[DIMTOLJ].dval.ioval   =atoi(sysvar[DB_QDIMTOLJ].defval);
        plinkvar[DIMTP].dval.roval    =atof(sysvar[DB_QDIMTP].defval);
        plinkvar[DIMTSZ].dval.roval   =atof(sysvar[DB_QDIMTSZ].defval);
        plinkvar[DIMTVP].dval.roval   =atof(sysvar[DB_QDIMTVP].defval);
        plinkvar[DIMTXSTY].dval.coval =sysvar[DB_QDIMTXSTY].defval;
        plinkvar[DIMTXT].dval.roval   =atof(sysvar[DB_QDIMTXT].defval);
        plinkvar[DIMZIN].dval.ioval    =atoi(sysvar[DB_QDIMZIN].defval);
        plinkvar[DIMAZIN].dval.ioval    =atoi(sysvar[DB_QDIMAZIN].defval); // Bugzila#78443 EBATECH(CNBR)
        plinkvar[DIMUNIT].dval.ioval   =atoi(sysvar[DB_QDIMUNIT].defval);
        plinkvar[DIMUPT].dval.ioval    =atoi(sysvar[DB_QDIMUPT].defval);
        plinkvar[DIMTZIN].dval.ioval   =atoi(sysvar[DB_QDIMTZIN].defval);
        // EBATECH(CNBR) -[ 24-03-2003 Bugzilla#78471 Support Lineweight
        plinkvar[DIMALTRND].dval.roval   =atof(sysvar[DB_QDIMALTRND].defval);
        plinkvar[DIMDSEP].dval.ioval    =0;
        plinkvar[DIMLWD].dval.ioval    =atoi(sysvar[DB_QDIMLWD].defval);
        plinkvar[DIMLWE].dval.ioval    =atoi(sysvar[DB_QDIMLWE].defval);
        // EBATECH(CNBR) ]-

    }

    //if (useStoredValues) {
    if (FALSE) {
        for (fi1=0;fi1<DIM_LASTDIMVAR;++fi1) {
            switch(plinkvar[fi1].type) {
                case RTSTR:
                    if (fi1==DIMSTYLE) break; // put this here because dimstyle is giving me a problem so it will be taken out temp. ALPO
                    plinkvar[fi1].dval.cval=(char *)new char[strlen(plinkvar[fi1].dval.coval)+1];
                    strcpy(plinkvar[fi1].dval.cval,plinkvar[fi1].dval.coval);
					strupr(plinkvar[fi1].dval.cval);
                    break;
                case RTREAL:  plinkvar[fi1].dval.rval=plinkvar[fi1].dval.roval; break;
                case RTSHORT: plinkvar[fi1].dval.ival =plinkvar[fi1].dval.ioval; break;
            }
        }
    } else {
		//EBATECH(CNBR) Bugzilla78443 DIMADEC and DIMAZIN
		SDS_getvar(NULL,DB_QDIMADEC,&rbb,flp,NULL,NULL); plinkvar[DIMADEC].dval.ival    =rbb.resval.rint;
		// note -- DIMADEC is in the objsysvar group and therefore cannot be gotten with the index like the others
        //SDS_getvar("DIMADEC"/*DNT*/,0,&rbb,flp,NULL,NULL); plinkvar[DIMADEC].dval.ival    =rbb.resval.rint;
        //EBATECH(CNBR) ]-
        SDS_getvar(NULL,DB_QDIMALT,&rbb,flp,NULL,NULL); plinkvar[DIMALT].dval.ival      =rbb.resval.rint;
        SDS_getvar(NULL,DB_QDIMALTD,&rbb,flp,NULL,NULL); plinkvar[DIMALTD].dval.ival    =rbb.resval.rint;
        SDS_getvar(NULL,DB_QDIMALTF,&rbb,flp,NULL,NULL); plinkvar[DIMALTF].dval.rval   =rbb.resval.rreal;
        SDS_getvar(NULL,DB_QDIMALTTD,&rbb,flp,NULL,NULL); plinkvar[DIMALTTD].dval.ival  =rbb.resval.rint;
        SDS_getvar(NULL,DB_QDIMALTTZ,&rbb,flp,NULL,NULL); plinkvar[DIMALTTZ].dval.ival  =rbb.resval.rint;
        SDS_getvar(NULL,DB_QDIMALTU,&rbb,flp,NULL,NULL); plinkvar[DIMALTU].dval.ival    =rbb.resval.rint;
        SDS_getvar(NULL,DB_QDIMALTZ,&rbb,flp,NULL,NULL); plinkvar[DIMALTZ].dval.ival    =rbb.resval.rint;
        SDS_getvar(NULL,DB_QDIMAPOST,&rbb,flp,NULL,NULL); copyStrToPlinkVar(&rbb, DIMAPOST, false); // plinkvar[DIMAPOST].dval.cval =rbb.resval.rstring;
        SDS_getvar(NULL,DB_QDIMASO,&rbb,flp,NULL,NULL); plinkvar[DIMASO].dval.ival      =rbb.resval.rint;
        SDS_getvar(NULL,DB_QDIMASZ,&rbb,flp,NULL,NULL); plinkvar[DIMASZ].dval.rval     =rbb.resval.rreal;
        // EBATECH(watanabe)-[dimatfit
        SDS_getvar(NULL,DB_QDIMATFIT,&rbb,flp,NULL,NULL); plinkvar[DIMATFIT].dval.ival =rbb.resval.rint;
        // ]-EBATECH(watanabe)
        SDS_getvar(NULL,DB_QDIMAUNIT,&rbb,flp,NULL,NULL); plinkvar[DIMAUNIT].dval.ival  =rbb.resval.rint;
        SDS_getvar(NULL,DB_QDIMBLK,&rbb,flp,NULL,NULL); copyStrToPlinkVar(&rbb, DIMBLK); // plinkvar[DIMBLK].dval.cval     =strupr(rbb.resval.rstring);
        SDS_getvar(NULL,DB_QDIMBLK1,&rbb,flp,NULL,NULL); copyStrToPlinkVar(&rbb, DIMBLK1); // plinkvar[DIMBLK1].dval.cval   =strupr(rbb.resval.rstring);
        SDS_getvar(NULL,DB_QDIMBLK2,&rbb,flp,NULL,NULL); copyStrToPlinkVar(&rbb, DIMBLK2); // plinkvar[DIMBLK2].dval.cval   =strupr(rbb.resval.rstring);
        SDS_getvar(NULL,DB_QDIMCEN,&rbb,flp,NULL,NULL); plinkvar[DIMCEN].dval.rval     =rbb.resval.rreal;
        SDS_getvar(NULL,DB_QDIMCLRD,&rbb,flp,NULL,NULL); plinkvar[DIMCLRD].dval.ival    =rbb.resval.rint;
        SDS_getvar(NULL,DB_QDIMCLRE,&rbb,flp,NULL,NULL); plinkvar[DIMCLRE].dval.ival    =rbb.resval.rint;
        SDS_getvar(NULL,DB_QDIMCLRT,&rbb,flp,NULL,NULL); plinkvar[DIMCLRT].dval.ival    =rbb.resval.rint;
        SDS_getvar(NULL,DB_QDIMDEC,&rbb,flp,NULL,NULL); plinkvar[DIMDEC].dval.ival      =rbb.resval.rint;
        SDS_getvar(NULL,DB_QDIMDLE,&rbb,flp,NULL,NULL); plinkvar[DIMDLE].dval.rval     =rbb.resval.rreal;
        SDS_getvar(NULL,DB_QDIMDLI,&rbb,flp,NULL,NULL); plinkvar[DIMDLI].dval.rval     =rbb.resval.rreal;
        SDS_getvar(NULL,DB_QDIMEXE,&rbb,flp,NULL,NULL); plinkvar[DIMEXE].dval.rval     =rbb.resval.rreal;
        SDS_getvar(NULL,DB_QDIMEXO,&rbb,flp,NULL,NULL); plinkvar[DIMEXO].dval.rval     =rbb.resval.rreal;
        SDS_getvar(NULL,DB_QDIMFIT,&rbb,flp,NULL,NULL); plinkvar[DIMFIT].dval.ival      =rbb.resval.rint;
        // EBATECH(watanabe)-[dimfrac
        SDS_getvar(NULL,DB_QDIMFRAC,&rbb,flp,NULL,NULL); plinkvar[DIMFRAC].dval.ival   =rbb.resval.rint;
        // ]-EBATECH(watanabe)
        SDS_getvar(NULL,DB_QDIMGAP,&rbb,flp,NULL,NULL); plinkvar[DIMGAP].dval.rval     =rbb.resval.rreal;
        SDS_getvar(NULL,DB_QDIMJUST,&rbb,flp,NULL,NULL); plinkvar[DIMJUST].dval.ival    =rbb.resval.rint;
        // EBATECH(watanabe)-[dimldrblk
        SDS_getvar(NULL,DB_QDIMLDRBLK,&rbb,flp,NULL,NULL); copyStrToPlinkVar(&rbb, DIMLDRBLK); // plinkvar[DIMLDRBLK].dval.cval=strupr(rbb.resval.rstring);
        // ]-EBATECH(watanabe)
        SDS_getvar(NULL,DB_QDIMLFAC,&rbb,flp,NULL,NULL); plinkvar[DIMLFAC].dval.rval   =rbb.resval.rreal;
        SDS_getvar(NULL,DB_QDIMLIM,&rbb,flp,NULL,NULL); plinkvar[DIMLIM].dval.ival      =rbb.resval.rint;
        // EBATECH(watanabe)-[dimlunit
        SDS_getvar(NULL,DB_QDIMLUNIT,&rbb,flp,NULL,NULL); plinkvar[DIMLUNIT].dval.ival =rbb.resval.rint;
        // ]-EBATECH(watanabe)
        // EBATECH(CNBR)-[ 2002/5/1 
        SDS_getvar(NULL,DB_QDIMPOST,&rbb,flp,NULL,NULL); copyStrToPlinkVar(&rbb, DIMPOST, false); // plinkvar[DIMPOST].dval.cval   =rbb.resval.rstring;
        //SDS_getvar(NULL,DB_QDIMPOST,&rbb,flp,NULL,NULL); copyStrToPlinkVar(&rbb, DIMAPOST, false); // plinkvar[DIMPOST].dval.cval   =rbb.resval.rstring;
        // EBATECH(CNBR) ]-
        SDS_getvar(NULL,DB_QDIMRND,&rbb,flp,NULL,NULL); plinkvar[DIMRND].dval.rval     =rbb.resval.rreal;
        SDS_getvar(NULL,DB_QDIMSAH,&rbb,flp,NULL,NULL); plinkvar[DIMSAH].dval.ival      =rbb.resval.rint;
        SDS_getvar(NULL,DB_QDIMSCALE,&rbb,flp,NULL,NULL); plinkvar[DIMSCALE].dval.rval =icadRealEqual(rbb.resval.rreal,0.0) ? 1.0 : rbb.resval.rreal;
        SDS_getvar(NULL,DB_QDIMSD1,&rbb,flp,NULL,NULL); plinkvar[DIMSD1].dval.ival      =rbb.resval.rint;
        SDS_getvar(NULL,DB_QDIMSD2,&rbb,flp,NULL,NULL); plinkvar[DIMSD2].dval.ival      =rbb.resval.rint;
        SDS_getvar(NULL,DB_QDIMSE1,&rbb,flp,NULL,NULL); plinkvar[DIMSE1].dval.ival      =rbb.resval.rint;
        SDS_getvar(NULL,DB_QDIMSE2,&rbb,flp,NULL,NULL); plinkvar[DIMSE2].dval.ival      =rbb.resval.rint;
        SDS_getvar(NULL,DB_QDIMSHO,&rbb,flp,NULL,NULL); plinkvar[DIMSHO].dval.ival      =rbb.resval.rint;
        SDS_getvar(NULL,DB_QDIMSOXD,&rbb,flp,NULL,NULL); plinkvar[DIMSOXD].dval.ival    =rbb.resval.rint;
        SDS_getvar(NULL,DB_QDIMSTYLE,&rbb,flp,NULL,NULL); copyStrToPlinkVar(&rbb, DIMSTYLE); // plinkvar[DIMSTYLE].dval.cval =strupr(rbb.resval.rstring);
        SDS_getvar(NULL,DB_QDIMTAD,&rbb,flp,NULL,NULL); plinkvar[DIMTAD].dval.ival      =rbb.resval.rint;
        SDS_getvar(NULL,DB_QDIMTDEC,&rbb,flp,NULL,NULL); plinkvar[DIMTDEC].dval.ival    =rbb.resval.rint;
        SDS_getvar(NULL,DB_QDIMTFAC,&rbb,flp,NULL,NULL); plinkvar[DIMTFAC].dval.rval   =rbb.resval.rreal;
        SDS_getvar(NULL,DB_QDIMTIH,&rbb,flp,NULL,NULL); plinkvar[DIMTIH].dval.ival      =rbb.resval.rint;
        SDS_getvar(NULL,DB_QDIMTIX,&rbb,flp,NULL,NULL); plinkvar[DIMTIX].dval.ival      =rbb.resval.rint;
        SDS_getvar(NULL,DB_QDIMTM,&rbb,flp,NULL,NULL); plinkvar[DIMTM].dval.rval       =rbb.resval.rreal;
        // EBATECH(watanabe)-[dimtmove
        SDS_getvar(NULL,DB_QDIMTMOVE,&rbb,flp,NULL,NULL); plinkvar[DIMTMOVE].dval.ival =rbb.resval.rint;
        // ]-EBATECH(watanabe)
        SDS_getvar(NULL,DB_QDIMTOFL,&rbb,flp,NULL,NULL); plinkvar[DIMTOFL].dval.ival    =rbb.resval.rint;
        SDS_getvar(NULL,DB_QDIMTOH,&rbb,flp,NULL,NULL); plinkvar[DIMTOH].dval.ival      =rbb.resval.rint;
        SDS_getvar(NULL,DB_QDIMTOL,&rbb,flp,NULL,NULL); plinkvar[DIMTOL].dval.ival      =rbb.resval.rint;
        SDS_getvar(NULL,DB_QDIMTOLJ,&rbb,flp,NULL,NULL); plinkvar[DIMTOLJ].dval.ival    =rbb.resval.rint;
        SDS_getvar(NULL,DB_QDIMTP,&rbb,flp,NULL,NULL); plinkvar[DIMTP].dval.rval       =rbb.resval.rreal;
        SDS_getvar(NULL,DB_QDIMTSZ,&rbb,flp,NULL,NULL); plinkvar[DIMTSZ].dval.rval     =rbb.resval.rreal;
        SDS_getvar(NULL,DB_QDIMTVP,&rbb,flp,NULL,NULL); plinkvar[DIMTVP].dval.rval     =rbb.resval.rreal;
        SDS_getvar(NULL,DB_QDIMTXSTY,&rbb,flp,NULL,NULL); copyStrToPlinkVar(&rbb, DIMTXSTY); // plinkvar[DIMTXSTY].dval.cval =strupr(rbb.resval.rstring);
        SDS_getvar(NULL,DB_QDIMTXT,&rbb,flp,NULL,NULL); plinkvar[DIMTXT].dval.rval     =rbb.resval.rreal;
        SDS_getvar(NULL,DB_QDIMZIN,&rbb,flp,NULL,NULL); plinkvar[DIMZIN].dval.ival      =rbb.resval.rint;
        SDS_getvar(NULL,DB_QDIMAZIN,&rbb,flp,NULL,NULL); plinkvar[DIMAZIN].dval.ival      =rbb.resval.rint;  // Bugzila#78443 EBATECH(CNBR)
        SDS_getvar(NULL,DB_QDIMUNIT,&rbb,flp,NULL,NULL); plinkvar[DIMUNIT].dval.ival    =rbb.resval.rint;
        SDS_getvar(NULL,DB_QDIMUPT,&rbb,flp,NULL,NULL); plinkvar[DIMUPT].dval.ival      =rbb.resval.rint;
        SDS_getvar(NULL,DB_QDIMTZIN,&rbb,flp,NULL,NULL); plinkvar[DIMTZIN].dval.ival    =rbb.resval.rint;
        // EBATECH(CNBR) -[ 24-03-2003 Bugzilla#78471 Support Lineweight
        SDS_getvar(NULL,DB_QDIMALTRND,&rbb,flp,NULL,NULL); plinkvar[DIMALTRND].dval.rval=rbb.resval.rreal;
        SDS_getvar(NULL,DB_QDIMDSEP,&rbb,flp,NULL,NULL);   plinkvar[DIMDSEP].dval.ival  =rbb.resval.rstring[0];
        IC_FREE(rbb.resval.rstring);
        SDS_getvar(NULL,DB_QDIMLWD,&rbb,flp,NULL,NULL);    plinkvar[DIMLWD].dval.ival   =rbb.resval.rint;
        SDS_getvar(NULL,DB_QDIMLWE,&rbb,flp,NULL,NULL);    plinkvar[DIMLWE].dval.ival   =rbb.resval.rint;
        // EBATECH(CNBR) ]-
    }
	// EBATECH-[case DIMASO=Off,dim Ent should be create on current layer.
	if (!plinkvar[DIMASO].dval.ival){
		SDS_getvar(NULL,DB_QCLAYER,&rbb,flp,&SDS_CURCFG,&SDS_CURSES); 
		layername = (char *)new char[strlen(rbb.resval.rstring)+1];
		strcpy(layername, rbb.resval.rstring);
		IC_FREE(rbb.resval.rstring);
		rbb.resval.rstring = NULL;
			//layername=rbb.resval.rstring;
	}// ]-EBATECH	

    //if (!loadDefaults) loadDefaults=TRUE;
}

short cmd_CleanUpDimVarStrings(void) {	// Frees the strings from the SDS_getvars above.

	//Bugzilla No. 78104 ; 15-04-2002 [
	if (plinkvar[DIMAPOST].dval.cval) { delete[] plinkvar[DIMAPOST].dval.cval;	plinkvar[DIMAPOST].dval.cval=NULL; }
    if (plinkvar[DIMBLK].dval.cval)   { delete[] plinkvar[DIMBLK].dval.cval;		plinkvar[DIMBLK].dval.cval=NULL;	}
    if (plinkvar[DIMBLK1].dval.cval)  { delete[] plinkvar[DIMBLK1].dval.cval;		plinkvar[DIMBLK1].dval.cval=NULL; }
    if (plinkvar[DIMBLK2].dval.cval)  { delete[] plinkvar[DIMBLK2].dval.cval;		plinkvar[DIMBLK2].dval.cval=NULL; }
    // EBATECH(watanabe)-[dimldrblk
    if (plinkvar[DIMLDRBLK].dval.cval){ delete[] plinkvar[DIMLDRBLK].dval.cval;	plinkvar[DIMLDRBLK].dval.cval=NULL; }
    // ]-EBATECH(watanabe)
    if (plinkvar[DIMTXSTY].dval.cval) { delete[] plinkvar[DIMTXSTY].dval.cval;	plinkvar[DIMTXSTY].dval.cval=NULL;}
    if (plinkvar[DIMPOST].dval.cval)  { delete[] plinkvar[DIMPOST].dval.cval;		plinkvar[DIMPOST].dval.cval=NULL; }
	if (plinkvar[DIMSTYLE].dval.cval) { delete[] plinkvar[DIMSTYLE].dval.cval;	plinkvar[DIMSTYLE].dval.cval=NULL;}

	if(layername) { delete[] layername;  layername =NULL; }
	
	//Bugzilla No. 78104 ; 15-04-2002 ]

	return RTNORM;	// ??
}

