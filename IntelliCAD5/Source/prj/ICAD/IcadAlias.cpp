/* D:\DEV\PRJ\ICAD\ICADALIAS.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 
// THIS FILE HAS BEEN GLOBALIZED!

// *********************************************************************
// *  ICADALIAS.C                                               *
// *  Copyright (C) 1994,1995, 1996, 1997, 1998 by Visio, Inc.               *
// *                                                                   *
// *********************************************************************

// ** Includes
#include "Icad.h"/*DNT*/
#include "IcadApi.h"
#include "IntCmds.h"
#include "IcadPrivate.h"

struct SDS_AliasList SDS_pAliasCmdList[] = { 
//   These are in alphabetical order by the alias (not the command name).
//
//  After changing these, delete the registry
//  or select Customize -> Aliases -> Reset
//  to see your changes in the program.
//
//   What the				Index to localized				Command that				Index to localized	Prompt assoc'd     Not   AutoCAD                
//   user types				alias							gets called					command				with this          Sure  compatibility mode     
//---------------------------------------------------------------------------																				
//
   {"3A"/*DNT*/           ,IDC_ICADALIAS_3A_0,				"_3DARRAY"/*DNT*/          ,CMDSTR_3DARRAY      ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"3DFACE"/*DNT*/       ,IDC_ICADALIAS_3DFACE_2,			"_FACE"/*DNT*/             ,CMDSTR_FACE         ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"3DLINE"/*DNT*/       ,IDC_ICADALIAS_3DLINE_2,			"_LINE"/*DNT*/             ,CMDSTR_LINE         ,NULL,NULL               ,0 ,0 ,0},
   {"3DMESH"/*DNT*/       ,IDC_ICADALIAS_3DMESH_4,			"_MESH"/*DNT*/             ,CMDSTR_MESH         ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"3F"/*DNT*/           ,IDC_ICADALIAS_3F_6,				"_FACE"/*DNT*/             ,CMDSTR_FACE         ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"3P"/*DNT*/           ,IDC_ICADALIAS_3P_7,				"_3DPOLY"/*DNT*/           ,CMDSTR_3DPOLY       ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
//
   {"A"/*DNT*/            ,IDC_ICADALIAS_A_9,				"_ARC"/*DNT*/              ,CMDSTR_ARC          ,NULL,NULL               ,0 ,0 ,0          },
   {"AA"/*DNT*/           ,IDC_ICADALIAS_AA_10,				"_AREA"/*DNT*/             ,CMDSTR_AREA         ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"AD"/*DNT*/           ,IDC_ICADALIAS_AD_11,				"_ATTDISP"/*DNT*/          ,CMDSTR_ATTDISP      ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"AI_PROPCHK"/*DNT*/   ,IDC_ICADALIAS_AI_PROPCHK_17,		"_ENTPROP"/*DNT*/          ,CMDSTR_ENTPROP      ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"AL"/*DNT*/           ,IDC_ICADALIAS_AL_23,				"_ALIGN"/*DNT*/            ,CMDSTR_ALIGN        ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"AP"/*DNT*/           ,IDC_ICADALIAS_AP_25,				"_APERTURE"/*DNT*/         ,CMDSTR_APERTURE     ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"AR"/*DNT*/           ,IDC_ICADALIAS_AR_27,				"_ARRAY"/*DNT*/            ,CMDSTR_ARRAY        ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"-AT"/*DNT*/          ,IDC_ICADALIAS__AT_28,			"_ATTDEF"/*DNT*/           ,CMDSTR_ATTDEF       ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"AT"/*DNT*/           ,IDC_ICADALIAS_AT_29,				"_DDATTDEF"/*DNT*/         ,CMDSTR_DDATTDEF     ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"-ATE"/*DNT*/         ,IDC_ICADALIAS__ATE_30,			"_ATTEDIT"/*DNT*/          ,CMDSTR_ATTEDIT      ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"ATE"/*DNT*/          ,IDC_ICADALIAS_ATE_32,			"_DDATTE"/*DNT*/           ,CMDSTR_DDATTE       ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"AX"/*DNT*/           ,IDC_ICADALIAS_AX_33,				"_ATTEXT"/*DNT*/           ,CMDSTR_ATTEXT       ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
//
   {"-B"/*DNT*/           ,IDC_ICADALIAS__B_35,				"_BLOCK"/*DNT*/            ,CMDSTR_BLOCK        ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"B"/*DNT*/            ,IDC_ICADALIAS_B_36,				"_EXPBLOCKS"/*DNT*/        ,CMDSTR_EXPBLOCKS    ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"BA"/*DNT*/           ,IDC_ICADALIAS_BA_38,				"_BASE"/*DNT*/             ,CMDSTR_BASE         ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"BACKGROUND"/*DNT*/   ,IDC_ICADALIAS_BACKGROUND_40,		"_BACKGROUNDS"/*DNT*/      ,CMDSTR_BACKGROUNDS  ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"BAGEL"/*DNT*/        ,IDC_ICADALIAS_BAGEL_42,			"_DONUT"/*DNT*/            ,CMDSTR_DONUT        ,NULL,NULL               ,0 ,0 ,0},
   {"BM"/*DNT*/           ,IDC_ICADALIAS_BM_43,				"_BLIPMODE"/*DNT*/         ,CMDSTR_BLIPMODE     ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"BMAKE"/*DNT*/        ,IDC_ICADALIAS_BMAKE_45,			"_EXPBLOCKS"/*DNT*/        ,CMDSTR_EXPBLOCKS    ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"-BO"/*DNT*/          ,IDC_ICADALIAS__BO_46,			"_-BOUNDARY"/*DNT*/        ,CMDSTR_DASH_BOUNDARY ,NULL,NULL              ,0 ,0 ,SDS_CMDACAD},
   {"BO"/*DNT*/           ,IDC_ICADALIAS_BO_48,				"_BOUNDARY"/*DNT*/          ,CMDSTR_BOUNDARY     ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"BPOLY"/*DNT*/		  ,IDC_ICADALIAS_BPOLY_47,			"_BPOLY"/*DNT*/            ,CMDSTR_BOUNDARY     ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},  
   {"BR"/*DNT*/           ,IDC_ICADALIAS_BR_50,				"_BREAK"/*DNT*/            ,CMDSTR_BREAK        ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
//
   {"C"/*DNT*/            ,IDC_ICADALIAS_C_49,				"_CIRCLE"/*DNT*/           ,CMDSTR_CIRCLE       ,NULL,NULL               ,0 ,0 ,0},
   {"-CH"/*DNT*/          ,IDC_ICADALIAS__CH_51,			"_CHANGE"/*DNT*/           ,CMDSTR_CHANGE       ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"CH"/*DNT*/           ,IDC_ICADALIAS_CH_53,				"_DDCHPROP"/*DNT*/         ,CMDSTR_DDCHPROP     ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"CHA"/*DNT*/          ,IDC_ICADALIAS_CHA_54,			"_CHAMFER"/*DNT*/          ,CMDSTR_CHAMFER      ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"CL"/*DNT*/           ,IDC_ICADALIAS_CL_55,				"_COPYLINK"/*DNT*/         ,CMDSTR_COPYLINK     ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"CLOSEALL"/*DNT*/     ,IDC_ICADALIAS_CLOSEALL_57,		"_WCLOSEALL"/*DNT*/        ,CMDSTR_WCLOSEALL    ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"CO"/*DNT*/           ,IDC_ICADALIAS_CO_59,				"_COPY"/*DNT*/             ,CMDSTR_COPY         ,NULL,NULL               ,0 ,0 ,0},
   {"COL"/*DNT*/          ,IDC_ICADALIAS_COL_60,			"_SETCOLOR"/*DNT*/         ,CMDSTR_SETCOLOR     ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"CP"/*DNT*/           ,IDC_ICADALIAS_CP_62,				"_COPY"/*DNT*/             ,CMDSTR_COPY         ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
//
   {"D"/*DNT*/            ,IDC_ICADALIAS_D_63,				"_SETDIM"/*DNT*/           ,CMDSTR_SETDIM       ,NULL,NULL               ,0 ,0 ,0},
   {"DAL"/*DNT*/          ,IDC_ICADALIAS_DAL_65,			"_DIMALIGNED"/*DNT*/       ,CMDSTR_DIMALIGNED   ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"DAN"/*DNT*/          ,IDC_ICADALIAS_DAN_66,			"_DIMANGULAR"/*DNT*/       ,CMDSTR_DIMANGULAR   ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"DBA"/*DNT*/          ,IDC_ICADALIAS_DBA_67,			"_DIMBASELINE"/*DNT*/      ,CMDSTR_DIMBASELINE  ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"DCE"/*DNT*/          ,IDC_ICADALIAS_DCE_68,			"_DIMCENTER"/*DNT*/        ,CMDSTR_DIMCENTER    ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"DCO"/*DNT*/          ,IDC_ICADALIAS_DCO_69,			"_DIMCONTINUE"/*DNT*/      ,CMDSTR_DIMCONTINUE  ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"DDCOLOR"/*DNT*/      ,IDC_ICADALIAS_DDCOLOR_70,		"_SETCOLOR"/*DNT*/         ,CMDSTR_SETCOLOR     ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"DDESNAP"/*DNT*/      ,IDC_ICADALIAS_DDESNAP_71,		"_SETESNAP"/*DNT*/         ,CMDSTR_SETESNAP     ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"DDI"/*DNT*/          ,IDC_ICADALIAS_DDI_73,			"_DIMDIAMETER"/*DNT*/      ,CMDSTR_DIMDIAMETER  ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"DDIM"/*DNT*/         ,IDC_ICADALIAS_DDIM_74,			"_SETDIM"/*DNT*/           ,CMDSTR_SETDIM       ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"DDLMODES"/*DNT*/     ,IDC_ICADALIAS_DDLMODES_75,		"_EXPLAYERS"/*DNT*/        ,CMDSTR_EXPLAYERS    ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"DDLTYPE"/*DNT*/      ,IDC_ICADALIAS_DDLTYPE_77,		"_EXPLTYPES"/*DNT*/        ,CMDSTR_EXPLTYPES    ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"DDMODIFY"/*DNT*/     ,IDC_ICADALIAS_DDMODIFY_79,		"_ENTPROP"/*DNT*/          ,CMDSTR_ENTPROP      ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"DDNEW"/*DNT*/        ,IDC_ICADALIAS_DDNEW_80,			"_NEWWIZ"/*DNT*/           ,CMDSTR_NEWWIZ       ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"DDOSNAP"/*DNT*/      ,IDC_ICADALIAS_DDOSNAP_82,		"_SETESNAP"/*DNT*/         ,CMDSTR_SETESNAP     ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"DDSTYLE"/*DNT*/      ,IDC_ICADALIAS_DDSTYLE_83,		"_EXPFONTS"/*DNT*/         ,CMDSTR_EXPFONTS     ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"DDUCS"/*DNT*/        ,IDC_ICADALIAS_DDUCS_85,			"_EXPUCS"/*DNT*/           ,CMDSTR_EXPUCS       ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"DDUCSP"/*DNT*/       ,IDC_ICADALIAS_DDUCSP_87,			"_SETUCS"/*DNT*/           ,CMDSTR_SETUCS       ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"DDVIEW"/*DNT*/       ,IDC_ICADALIAS_DDVIEW_89,			"_EXPVIEWS"/*DNT*/         ,CMDSTR_EXPVIEWS     ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"DDVPOINT"/*DNT*/     ,IDC_ICADALIAS_DDVPOINT_91,		"_VIEWCTL"/*DNT*/          ,CMDSTR_VIEWCTL      ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},  // Used to call SETVPOINT (preset viewpoints), but this is more accurate.
   {"DED"/*DNT*/          ,IDC_ICADALIAS_DED_93,			"_DIMEDIT"/*DNT*/          ,CMDSTR_DIMEDIT      ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
//   {"DEL"/*DNT*/        ,IDC_ICADALIAS_DEL_94,			"_DEL"/*DNT*/            ,CMDSTR_DEL          ,NULL,"Files to erase: ",0 ,0 ,SDS_CMDACAD},
   {"DI"/*DNT*/           ,IDC_ICADALIAS_DI_96,				"_DIST"/*DNT*/             ,CMDSTR_DIST         ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},  // DISTANCE is a variable - don't make an alias using it.
   {"DIM"/*DNT*/          ,IDC_ICADALIAS_DIM_98,			"_DIMENSION"/*DNT*/        ,CMDSTR_DIMENSION    ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"DIMALI"/*DNT*/       ,IDC_ICADALIAS_DIMALI_100,		"_DIMALIGNED"/*DNT*/       ,CMDSTR_DIMALIGNED   ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"DIMANG"/*DNT*/       ,IDC_ICADALIAS_DIMANG_101,		"_DIMANGULAR"/*DNT*/       ,CMDSTR_DIMANGULAR   ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"DIMBASE"/*DNT*/      ,IDC_ICADALIAS_DIMBASE_102,		"_DIMBASELINE"/*DNT*/      ,CMDSTR_DIMBASELINE  ,NULL,NULL               ,0 ,0 ,0},
   {"DIMCONT"/*DNT*/      ,IDC_ICADALIAS_DIMCONT_103,		"_DIMCONTINUE"/*DNT*/      ,CMDSTR_DIMCONTINUE  ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"DIMDIA"/*DNT*/       ,IDC_ICADALIAS_DIMDIA_104,		"_DIMDIAMETER"/*DNT*/      ,CMDSTR_DIMDIAMETER  ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"DIMED"/*DNT*/        ,IDC_ICADALIAS_DIMED_105,			"_DIMEDIT"/*DNT*/          ,CMDSTR_DIMEDIT      ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"DIMLIN"/*DNT*/       ,IDC_ICADALIAS_DIMLIN_106,		"_DIMLINEAR"/*DNT*/        ,CMDSTR_DIMLINEAR    ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"DIMORD"/*DNT*/       ,IDC_ICADALIAS_DIMORD_107,		"_DIMORDINATE"/*DNT*/      ,CMDSTR_DIMORDINATE  ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"DIMOVER"/*DNT*/      ,IDC_ICADALIAS_DIMOVER_108,		"_DIMOVERRIDE"/*DNT*/      ,CMDSTR_DIMOVERRIDE  ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"DIMRAD"/*DNT*/       ,IDC_ICADALIAS_DIMRAD_109,		"_DIMRADIUS"/*DNT*/        ,CMDSTR_DIMRADIUS    ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"DIMSTY"/*DNT*/       ,IDC_ICADALIAS_DIMSTY_110,		"_DIMSTYLE"/*DNT*/         ,CMDSTR_DIMSTYLE     ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"DIMTED"/*DNT*/       ,IDC_ICADALIAS_DIMTED_111,		"_DIMTEDIT"/*DNT*/         ,CMDSTR_DIMTEDIT     ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"DIV"/*DNT*/          ,IDC_ICADALIAS_DIV_112,			"_DIVIDE"/*DNT*/           ,CMDSTR_DIVIDE       ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
//   {"DIR"/*DNT*/        ,IDC_ICADALIAS_DIR_113,			"_DIR"/*DNT*/            ,CMDSTR_DIR          ,NULL,"Files to list: " ,0 ,0 ,SDS_CMDACAD},
   {"DLI"/*DNT*/          ,IDC_ICADALIAS_DLI_115,			"_DIMLINEAR"/*DNT*/        ,CMDSTR_DIMLINEAR    ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"DL"/*DNT*/           ,IDC_ICADALIAS_DL_116,			"_MLINE"/*DNT*/            ,CMDSTR_MLINE        ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   // EBATECH(watanabe)-[FIX: the mistake order
   //{"DLINE"/*DNT*/        ,IDC_ICADALIAS_MLINE_117,			"_MLINE"/*DNT*/            ,CMDSTR_MLINE        ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   //{"DO"/*DNT*/           ,IDC_ICADALIAS_DLINE_118,			"_DONUT"/*DNT*/            ,CMDSTR_DONUT        ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},  // DONUT and DOUGHNUT are both commands.  No need to alias.
   //{"DOR"/*DNT*/          ,IDC_ICADALIAS_DO_119,			"_DIMORDINATE"/*DNT*/      ,CMDSTR_DIMORDINATE  ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   //{"DOV"/*DNT*/          ,IDC_ICADALIAS_DOR_120,			"_DIMOVERRIDE"/*DNT*/      ,CMDSTR_DIMOVERRIDE  ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   //{"DRA"/*DNT*/          ,IDC_ICADALIAS_DOV_121,			"_DIMRADIUS"/*DNT*/        ,CMDSTR_DIMRADIUS    ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   //{"DS"/*DNT*/           ,IDC_ICADALIAS_DRA_122,			"_EXPDIMSTYLES"/*DNT*/     ,CMDSTR_EXPDIMSTYLES ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"DLINE"/*DNT*/        ,IDC_ICADALIAS_DLINE_118,			"_MLINE"/*DNT*/            ,CMDSTR_MLINE        ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"DO"/*DNT*/           ,IDC_ICADALIAS_DO_119,			"_DONUT"/*DNT*/            ,CMDSTR_DONUT        ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},  // DONUT and DOUGHNUT are both commands.  No need to alias.
   {"DOR"/*DNT*/          ,IDC_ICADALIAS_DOR_120,			"_DIMORDINATE"/*DNT*/      ,CMDSTR_DIMORDINATE  ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"DOV"/*DNT*/          ,IDC_ICADALIAS_DOV_121,			"_DIMOVERRIDE"/*DNT*/      ,CMDSTR_DIMOVERRIDE  ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"DRA"/*DNT*/          ,IDC_ICADALIAS_DRA_122,			"_DIMRADIUS"/*DNT*/        ,CMDSTR_DIMRADIUS    ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"DS"/*DNT*/           ,IDC_ICADALIAS_DS,				"_EXPDIMSTYLES"/*DNT*/     ,CMDSTR_EXPDIMSTYLES ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   //]-EBATECH(watanabe)

   {"DST"/*DNT*/          ,IDC_ICADALIAS_DST_123,			"_DIMSTYLE"/*DNT*/         ,CMDSTR_DIMSTYLE     ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"DT"/*DNT*/           ,IDC_ICADALIAS_DT_124,			"_DTEXT"/*DNT*/            ,CMDSTR_DTEXT        ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"DV"/*DNT*/           ,IDC_ICADALIAS_DV_125,			"_DVIEW"/*DNT*/            ,CMDSTR_DVIEW        ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"DN"/*DNT*/           ,IDC_ICADALIAS_DN_127,			"_DXFIN"/*DNT*/            ,CMDSTR_DXFIN        ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"DX"/*DNT*/           ,IDC_ICADALIAS_DX_129,			"_DXFOUT"/*DNT*/           ,CMDSTR_DXFOUT       ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
//
   {"E"/*DNT*/            ,IDC_ICADALIAS_E_131,				"_ERASE"/*DNT*/            ,CMDSTR_ERASE        ,NULL,NULL               ,0 ,0 ,0},
   {"ED"/*DNT*/           ,IDC_ICADALIAS_ED_133,			"_DDEDIT"/*DNT*/           ,CMDSTR_DDEDIT       ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"EL"/*DNT*/           ,IDC_ICADALIAS_EL_135,			"_ELLIPSE"/*DNT*/          ,CMDSTR_ELLIPSE      ,NULL,NULL               ,0 ,0 ,0},
   {"EX"/*DNT*/           ,IDC_ICADALIAS_EX_137,			"_EXTEND"/*DNT*/           ,CMDSTR_EXTEND       ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"EXP"/*DNT*/          ,IDC_ICADALIAS_EXP_138,			"_EXPORT"/*DNT*/           ,CMDSTR_EXPORT       ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"EXT"/*DNT*/          ,IDC_ICADALIAS_EXT_140,			"_EXTRUDE"/*DNT*/          ,CMDSTR_EXTRUDE      ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"EXPLORER"/*DNT*/     ,IDC_ICADALIAS_EXPLORER_142,		"_EXPLAYERS"/*DNT*/        ,CMDSTR_EXPLAYERS    ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"EXPSTYLE"/*DNT*/     ,IDC_ICADALIAS_EXPSTYLE_143,		"_EXPFONTS"/*DNT*/         ,CMDSTR_EXPFONTS     ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"EXPSTYLES"/*DNT*/    ,IDC_ICADALIAS_EXPSTYLES_144,		"_EXPFONTS"/*DNT*/         ,CMDSTR_EXPFONTS     ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
//
   {"FI"/*DNT*/           ,IDC_ICADALIAS_FI_145,			"_FILTER"/*DNT*/           ,CMDSTR_FILTER       ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"F"/*DNT*/            ,IDC_ICADALIAS_F_147,				"_FILLET"/*DNT*/           ,CMDSTR_FILLET       ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
//
   {"G"/*DNT*/            ,IDC_ICADALIAS_G_148,				"_GRID"/*DNT*/             ,CMDSTR_GRID         ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"GR"/*DNT*/           ,IDC_ICADALIAS_GR_149,			"_DDGRIPS"/*DNT*/          ,CMDSTR_DDGRIPS      ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
//
   {"H"/*DNT*/            ,IDC_ICADALIAS_H_151,				"_HATCH"/*DNT*/            ,CMDSTR_HATCH        ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"HE"/*DNT*/           ,IDC_ICADALIAS_HE_152,			"_HATCHEDIT"/*DNT*/        ,CMDSTR_HATCHEDIT    ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"HI"/*DNT*/           ,IDC_ICADALIAS_HI_154,			"_HIDE"/*DNT*/             ,CMDSTR_HIDE         ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
//
   {"I"/*DNT*/            ,IDC_ICADALIAS_I_155,				"_DDINSERT"/*DNT*/         ,CMDSTR_DDINSERT     ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"IAD"/*DNT*/          ,IDC_ICADALIAS_IAD_156,			"_IMAGEADJUST"/*DNT*/      ,CMDSTR_IMAGEADJUST  ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"IAT"/*DNT*/          ,IDC_ICADALIAS_IAT_158,			"_IMAGEATTACH"/*DNT*/      ,CMDSTR_IMAGEATTACH  ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"ICL"/*DNT*/          ,IDC_ICADALIAS_ICL_160,			"_IMAGECLIP"/*DNT*/        ,CMDSTR_IMAGECLIP    ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"ID"/*DNT*/           ,IDC_ICADALIAS_ID_162,			"_IDPOINT"/*DNT*/          ,CMDSTR_IDPOINT      ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
//   {"IM"/*DNT*/         ,IDC_ICADALIAS_IM_164,			"_IMAGE"/*DNT*/          ,CMDSTR_IMAGE        ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
//   {"-IM"/*DNT*/        ,IDC_ICADALIAS__IM_166,			"_IMAGE"/*DNT*/          ,CMDSTR_IMAGE        ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"IMAGE"/*DNT*/        ,IDC_ICADALIAS_IMAGE_165,			"_OPENIMAGE"/*DNT*/        ,CMDSTR_OPENIMAGE    ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"IMP"/*DNT*/          ,IDC_ICADALIAS_IMP_168,			"_OPEN"/*DNT*/             ,CMDSTR_OPEN       ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"IMPORT"/*DNT*/       ,IDC_ICADALIAS_IMPORT_169,		"_OPEN"/*DNT*/             ,CMDSTR_OPEN       ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"IN"/*DNT*/           ,IDC_ICADALIAS_IN_170,			"_INTERSECT"/*DNT*/        ,CMDSTR_INTERSECT    ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"INF"/*DNT*/          ,IDC_ICADALIAS_INF_172,			"_INTERFERE"/*DNT*/        ,CMDSTR_INTERFERE    ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"IO"/*DNT*/           ,IDC_ICADALIAS_IO_174,			"_INSERTOBJ"/*DNT*/        ,CMDSTR_INSERTOBJ    ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"IS"/*DNT*/           ,IDC_ICADALIAS_IS_176,			"_ISOPLANE"/*DNT*/         ,CMDSTR_ISOPLANE     ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
//
   {"L"/*DNT*/            ,IDC_ICADALIAS_L_178,				"_LINE"/*DNT*/             ,CMDSTR_LINE         ,NULL,NULL               ,0 ,0 ,0},
   {"LA"/*DNT*/           ,IDC_ICADALIAS_LA_180,			"_EXPLAYERS"/*DNT*/        ,CMDSTR_EXPLAYERS    ,NULL,NULL               ,0 ,0 ,0},
   {"-LA"/*DNT*/          ,IDC_ICADALIAS__LA_181,			"_LAYER"/*DNT*/            ,CMDSTR_LAYER        ,NULL,NULL               ,0 ,0 ,0},
   {"LE"/*DNT*/           ,IDC_ICADALIAS_LE_182,			"_LEADER"/*DNT*/           ,CMDSTR_LEADER       ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"LEAD"/*DNT*/         ,IDC_ICADALIAS_LEAD_183,			"_LEADER"/*DNT*/           ,CMDSTR_LEADER       ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"LENGTHEN"/*DNT*/     ,IDC_ICADALIAS_LENGTHEN_184,		"_EDITLEN"/*DNT*/          ,CMDSTR_EDITLEN      ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"LEN"/*DNT*/          ,IDC_ICADALIAS_LEN_186,			"_EDITLEN"/*DNT*/          ,CMDSTR_EDITLEN      ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"LI"/*DNT*/           ,IDC_ICADALIAS_LI_187,			"_LIST"/*DNT*/             ,CMDSTR_LIST         ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"LIGHT"/*DNT*/        ,IDC_ICADALIAS_LIGHT_188,			"_LIGHTING"/*DNT*/         ,CMDSTR_LIGHTING     ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"LS"/*DNT*/           ,IDC_ICADALIAS_LS_190,			"_LIST"/*DNT*/             ,CMDSTR_LIST         ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"LT"/*DNT*/           ,IDC_ICADALIAS_LT_191,			"_EXPLTYPES"/*DNT*/        ,CMDSTR_EXPLTYPES    ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"-LT"/*DNT*/          ,IDC_ICADALIAS__LT_192,			"_LINETYPE"/*DNT*/         ,CMDSTR_LINETYPE     ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"LTS"/*DNT*/          ,IDC_ICADALIAS_LTS_194,			"_LTSCALE"/*DNT*/		  ,CMDSTR_LTSCALE      ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
//
   {"M"/*DNT*/            ,IDC_ICADALIAS_M_196,				"_MOVE"/*DNT*/             ,CMDSTR_MOVE         ,NULL,NULL               ,0 ,0 ,0},
   {"MA"/*DNT*/           ,IDC_ICADALIAS_MA_198,			"_MATCHPROP"/*DNT*/        ,CMDSTR_MATCHPROP    ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"MAKEPREVIEW"/*DNT*/  ,IDC_ICADALIAS_MAKEPREVIEW_200,	"_PPREVIEW"/*DNT*/         ,CMDSTR_PPREVIEW     ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"MI"/*DNT*/           ,IDC_ICADALIAS_MI_202,			"_MIRROR"/*DNT*/           ,CMDSTR_MIRROR       ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
//   {"ML"/*DNT*/         ,IDC_ICADALIAS_ML_204,			"_MULTILINE"/*DNT*/      ,CMDSTR_MULTILINE    ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"MO"/*DNT*/           ,IDC_ICADALIAS_MO_206,			"_ENTPROP"/*DNT*/          ,CMDSTR_ENTPROP      ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"MS"/*DNT*/           ,IDC_ICADALIAS_MS_207,			"_MSPACE"/*DNT*/           ,CMDSTR_MSPACE       ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"MSLIDE"/*DNT*/       ,IDC_ICADALIAS_MSLIDE_209,		"_MSNAPSHOT"/*DNT*/        ,CMDSTR_MSNAPSHOT    ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"MT"/*DNT*/           ,IDC_ICADALIAS_MT_211,			"_MTEXT"/*DNT*/            ,CMDSTR_DDMTEXT      ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD}, // EBATECH(CNBR)
   {"MTPROP"/*DNT*/       ,IDC_ICADALIAS_MTPROP_212,		"_ENTPROP"/*DNT*/          ,CMDSTR_ENTPROP      ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"MV"/*DNT*/           ,IDC_ICADALIAS_MV_213,			"_MVIEW"/*DNT*/            ,CMDSTR_MVIEW        ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
//
   {"N"/*DNT*/            ,IDC_ICADALIAS_N_215,				"_NEW"/*DNT*/              ,CMDSTR_NEW          ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
//
   {"O"/*DNT*/            ,IDC_ICADALIAS_O_217,				"_PARALLEL"/*DNT*/         ,CMDSTR_PARALLEL     ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"OFFSET"/*DNT*/       ,IDC_ICADALIAS_OFFSET_219,		"_PARALLEL"/*DNT*/         ,CMDSTR_PARALLEL     ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"OO"/*DNT*/           ,IDC_ICADALIAS_OO_220,			"_UNDELETE"/*DNT*/         ,CMDSTR_UNDELETE     ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"OOPS"/*DNT*/         ,IDC_ICADALIAS_OOPS_222,			"_UNDELETE"/*DNT*/         ,CMDSTR_UNDELETE     ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"OP"/*DNT*/           ,IDC_ICADALIAS_OP_223,			"_OPEN"/*DNT*/             ,CMDSTR_OPEN         ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"OPTIONS"/*DNT*/      ,IDC_ICADALIAS_OPTIONS_225,		"_CONFIG"/*DNT*/           ,CMDSTR_CONFIG       ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"OR"/*DNT*/           ,IDC_ICADALIAS_OR_226,			"_ORTHOGONAL"/*DNT*/       ,CMDSTR_ORTHOGONAL   ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"ORTHO"/*DNT*/        ,IDC_ICADALIAS_ORTHO_228,			"_ORTHOGONAL"/*DNT*/       ,CMDSTR_ORTHOGONAL   ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"OS"/*DNT*/           ,IDC_ICADALIAS_OS_229,			"_SETESNAP"/*DNT*/         ,CMDSTR_SETESNAP     ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"-OS"/*DNT*/          ,IDC_ICADALIAS__OS_230,			"_ESNAP"/*DNT*/            ,CMDSTR_ESNAP        ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"-OSNAP"/*DNT*/       ,IDC_ICADALIAS__OSNAP_232,			"_ESNAP"/*DNT*/            ,CMDSTR_ESNAP        ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"OSNAP"/*DNT*/        ,IDC_ICADALIAS_OSNAP_233,			"_ESNAP"/*DNT*/            ,CMDSTR_ESNAP        ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
//
   {"P"/*DNT*/            ,IDC_ICADALIAS_P_234,				"_PAN"/*DNT*/              ,CMDSTR_PAN          ,NULL,NULL               ,0 ,0 ,0},
   {"-P"/*DNT*/           ,IDC_ICADALIAS__P_236,			"_PAN"/*DNT*/              ,CMDSTR_PAN          ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"PA"/*DNT*/           ,IDC_ICADALIAS_PA_237,			"_PASTESPEC"/*DNT*/        ,CMDSTR_PASTESPEC    ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"PE"/*DNT*/           ,IDC_ICADALIAS_PE_239,			"_EDITPLINE"/*DNT*/        ,CMDSTR_EDITPLINE    ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"PEDIT"/*DNT*/        ,IDC_ICADALIAS_PEDIT_241,			"_EDITPLINE"/*DNT*/        ,CMDSTR_EDITPLINE    ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"PL"/*DNT*/           ,IDC_ICADALIAS_PL_242,			"_POLYLINE"/*DNT*/         ,CMDSTR_POLYLINE     ,NULL,NULL               ,0 ,0 ,0},
   {"PLINE"/*DNT*/        ,IDC_ICADALIAS_PLINE_244,			"_POLYLINE"/*DNT*/         ,CMDSTR_POLYLINE     ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"PO"/*DNT*/           ,IDC_ICADALIAS_PO_245,			"_POINT"/*DNT*/            ,CMDSTR_POINT        ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"POL"/*DNT*/          ,IDC_ICADALIAS_POL_247,			"_POLYGON"/*DNT*/          ,CMDSTR_POLYGON      ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"PR"/*DNT*/           ,IDC_ICADALIAS_PR_248,			"_CONFIG"/*DNT*/           ,CMDSTR_CONFIG       ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"PRE"/*DNT*/          ,IDC_ICADALIAS_PRE_249,			"_PPREVIEW"/*DNT*/         ,CMDSTR_PPREVIEW     ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"PREFERENCES"/*DNT*/  ,IDC_ICADALIAS_PREFERENCES_250,	"_CONFIG"/*DNT*/           ,CMDSTR_CONFIG       ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"PREVIEW"/*DNT*/      ,IDC_ICADALIAS_PREVIEW_251,		"_PPREVIEW"/*DNT*/         ,CMDSTR_PPREVIEW     ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"PS"/*DNT*/           ,IDC_ICADALIAS_PS_252,			"_PSPACE"/*DNT*/           ,CMDSTR_PSPACE       ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"PU"/*DNT*/           ,IDC_ICADALIAS_PU_254,			"_PURGE"/*DNT*/            ,CMDSTR_PURGE        ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
//
   {"QT"/*DNT*/           ,IDC_ICADALIAS_QT_256,			"_QTEXT"/*DNT*/            ,CMDSTR_QTEXT        ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
//
   {"R"/*DNT*/            ,IDC_ICADALIAS_R_258,				"_REDRAW"/*DNT*/           ,CMDSTR_REDRAW       ,NULL,NULL               ,0 ,0 ,0},
   {"RA"/*DNT*/           ,IDC_ICADALIAS_RA_259,			"_REDRAWALL"/*DNT*/        ,CMDSTR_REDRAWALL    ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"RCONFIG"/*DNT*/      ,IDC_ICADALIAS_RCONFIG_260,		"_CONFIG"/*DNT*/           ,CMDSTR_CONFIG       ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"RE"/*DNT*/           ,IDC_ICADALIAS_RE_261,			"_REGEN"/*DNT*/            ,CMDSTR_REGEN        ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"REA"/*DNT*/          ,IDC_ICADALIAS_REA_262,			"_REGENALL"/*DNT*/         ,CMDSTR_REGENALL     ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"REC"/*DNT*/          ,IDC_ICADALIAS_REC_263,			"_RECTANGLE"/*DNT*/        ,CMDSTR_RECTANGLE    ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"RECT"/*DNT*/         ,IDC_ICADALIAS_RECT_265,			"_RECTANGLE"/*DNT*/        ,CMDSTR_RECTANGLE    ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"RECTANG"/*DNT*/      ,IDC_ICADALIAS_RECTANG_266,		"_RECTANGLE"/*DNT*/        ,CMDSTR_RECTANGLE    ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"REG"/*DNT*/          ,IDC_ICADALIAS_REG_267,			"_REGION"/*DNT*/           ,CMDSTR_REGION       ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"REN"/*DNT*/          ,IDC_ICADALIAS_REN_269,			"_DDRENAME"/*DNT*/         ,CMDSTR_DDRENAME     ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"-REN"/*DNT*/         ,IDC_ICADALIAS__REN_271,			"_RENAME"/*DNT*/           ,CMDSTR_RENAME       ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"REV"/*DNT*/          ,IDC_ICADALIAS_REV_272,			"_REVOLVE"/*DNT*/          ,CMDSTR_REVOLVE      ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"RI"/*DNT*/           ,IDC_ICADALIAS_RI_274,			"_REINIT"/*DNT*/           ,CMDSTR_REINIT       ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"RM"/*DNT*/           ,IDC_ICADALIAS_RM_276,			"_DDRMODES"/*DNT*/         ,CMDSTR_DDRMODES     ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"RMAT"/*DNT*/         ,IDC_ICADALIAS_RMAT_278,			"_MATERIALS"/*DNT*/        ,CMDSTR_MATERIALS    ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"RO"/*DNT*/           ,IDC_ICADALIAS_RO_280,			"_ROTATE"/*DNT*/           ,CMDSTR_ROTATE       ,NULL,NULL               ,0 ,0 ,0},
   {"RPREF"/*DNT*/        ,IDC_ICADALIAS_RPREF_281,			"_SETRENDER"/*DNT*/        ,CMDSTR_SETRENDER    ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"RR"/*DNT*/           ,IDC_ICADALIAS_RR_283,			"_RENDER"/*DNT*/           ,CMDSTR_RENDER       ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
//
   {"S"/*DNT*/            ,IDC_ICADALIAS_S_287,				"_STRETCH"/*DNT*/          ,CMDSTR_STRETCH      ,NULL,NULL               ,0 ,0 ,0},
   {"SA"/*DNT*/           ,IDC_ICADALIAS_SA_289,			"_SAVE"/*DNT*/             ,CMDSTR_SAVE         ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"SC"/*DNT*/           ,IDC_ICADALIAS_SC_290,			"_SCALE"/*DNT*/            ,CMDSTR_SCALE        ,NULL,NULL               ,0 ,0 ,0},
   {"SCR"/*DNT*/          ,IDC_ICADALIAS_SCR_291,			"_SCRIPT"/*DNT*/           ,CMDSTR_SCRIPT       ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"SE"/*DNT*/           ,IDC_ICADALIAS_SE_293,			"_DDSELECT"/*DNT*/         ,CMDSTR_DDSELECT     ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"SEC"/*DNT*/          ,IDC_ICADALIAS_SEC_294,			"_SECTION"/*DNT*/          ,CMDSTR_SECTION      ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"SELGRIP"/*DNT*/      ,IDC_ICADALIAS_SELGRIP_296,		"_SELGRIPS"/*DNT*/         ,CMDSTR_SELGRIPS     ,NULL,NULL               ,0 ,0 ,0},
   {"SET"/*DNT*/          ,IDC_ICADALIAS_SET_297,			"_SETVAR"/*DNT*/           ,CMDSTR_SETVAR       ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"SH"/*DNT*/           ,IDC_ICADALIAS_SH_299,				""/*DNT*/                  ,CMDSTR_SH           ,NULL,"DOS Command: "    ,0 ,1 ,SDS_CMDACAD},
   {"SHA"/*DNT*/          ,IDC_ICADALIAS_SHA_302,			"_SHADE"/*DNT*/            ,CMDSTR_SHADE        ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"SHELL"/*DNT*/        ,IDC_ICADALIAS_SHELL_304,			""/*DNT*/                  ,CMDSTR_SHELL        ,NULL,"DOS Command: "    ,4 ,1 ,SDS_CMDACAD},
   {"SKETCH"/*DNT*/       ,IDC_ICADALIAS_SKETCH_305,		"_FREEHAND"/*DNT*/         ,CMDSTR_FREEHAND     ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"SL"/*DNT*/           ,IDC_ICADALIAS_SL_307,			"_SLICE"/*DNT*/            ,CMDSTR_SLICE        ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"SN"/*DNT*/           ,IDC_ICADALIAS_SN_309,			"_SNAP"/*DNT*/             ,CMDSTR_SNAP         ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"SO"/*DNT*/           ,IDC_ICADALIAS_SO_310,			"_PLANE"/*DNT*/            ,CMDSTR_PLANE        ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"SOLID"/*DNT*/        ,IDC_ICADALIAS_SOLID_312,			"_PLANE"/*DNT*/            ,CMDSTR_PLANE        ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"SP"/*DNT*/           ,IDC_ICADALIAS_SP_313,			"_SPELL"/*DNT*/            ,CMDSTR_SPELL        ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"SPL"/*DNT*/          ,IDC_ICADALIAS_SPL_315,			"_SPLINE"/*DNT*/           ,CMDSTR_SPLINE       ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"SPE"/*DNT*/          ,IDC_ICADALIAS_SPE_317,			"_SPLINEDIT"/*DNT*/        ,CMDSTR_SPLINEDIT    ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"START"/*DNT*/        ,IDC_ICADALIAS_START_319,			""/*DNT*/                  ,CMDSTR_STARTSHELL   ,NULL,"Application to start: "   ,0 ,1 ,SDS_CMDACAD},
   {"ST"/*DNT*/           ,IDC_ICADALIAS_ST_321,			"_FONT"/*DNT*/             ,CMDSTR_FONT         ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"STYLE"/*DNT*/        ,IDC_ICADALIAS_STYLE_323,			"_FONT"/*DNT*/             ,CMDSTR_FONT         ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"SU"/*DNT*/           ,IDC_ICADALIAS_SU_324,				"_SUBTRACT"/*DNT*/         ,CMDSTR_SUBTRACT     ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
//
   {"T"/*DNT*/            ,IDC_ICADALIAS_T_326,				"_MTEXT"/*DNT*/            ,CMDSTR_DDMTEXT      ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD}, // EBATECH(CNBR)
   {"-T"/*DNT*/           ,IDC_ICADALIAS__T_327,			"_TEXT"/*DNT*/             ,CMDSTR_TEXT         ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"TA"/*DNT*/           ,IDC_ICADALIAS_TA_328,			"_TABLET"/*DNT*/           ,CMDSTR_TABLET       ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"TI"/*DNT*/           ,IDC_ICADALIAS_TI_330,			"_TIME"/*DNT*/             ,CMDSTR_TIME         ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"TO"/*DNT*/           ,IDC_ICADALIAS_TO_332,			"_TBCONFIG"/*DNT*/         ,CMDSTR_TBCONFIG     ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"TOL"/*DNT*/          ,IDC_ICADALIAS_TOL_334,			"_TOLERANCE"/*DNT*/        ,CMDSTR_TOLERANCE    ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"TOR"/*DNT*/          ,IDC_ICADALIAS_TOR_336,			"_TORUS"/*DNT*/            ,CMDSTR_TORUS        ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"TR"/*DNT*/           ,IDC_ICADALIAS_TR_337,			"_TRIM"/*DNT*/             ,CMDSTR_TRIM         ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"TX"/*DNT*/           ,IDC_ICADALIAS_TX_338,			"_TEXT"/*DNT*/             ,CMDSTR_TEXT         ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
//
//  NOTE: "U"/*DNT*/ and "hUNDO"/*DNT*/ are both commands - tat's why there is no alias for them.
   {"UC"/*DNT*/           ,IDC_ICADALIAS_UC_339,			"_EXPUCS"/*DNT*/           ,CMDSTR_EXPUCS       ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"UCP"/*DNT*/          ,IDC_ICADALIAS_UCP_340,			"_SETUCS"/*DNT*/           ,CMDSTR_SETUCS       ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"UN"/*DNT*/           ,IDC_ICADALIAS_UN_341,			"_DDUNITS"/*DNT*/          ,CMDSTR_DDUNITS      ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"-UN"/*DNT*/          ,IDC_ICADALIAS__UN_342,			"_UNITS"/*DNT*/            ,CMDSTR_UNITS        ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"UNI"/*DNT*/          ,IDC_ICADALIAS_UNI_344,			"_UNION"/*DNT*/            ,CMDSTR_UNION        ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
//
   {"V"/*DNT*/            ,IDC_ICADALIAS_V_346,				"_EXPVIEWS"/*DNT*/         ,CMDSTR_EXPVIEWS     ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"-V"/*DNT*/           ,IDC_ICADALIAS__V_347,			"_VIEW"/*DNT*/             ,CMDSTR_VIEW         ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"VBAIDE"/*DNT*/       ,IDC_ICADALIAS_VBAIDE_349,		"_VBA"/*DNT*/              ,CMDSTR_VBA          ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"VL"/*DNT*/           ,IDC_ICADALIAS_VL_351,			"_VPLAYER"/*DNT*/          ,CMDSTR_VPLAYER      ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"-VP"/*DNT*/          ,IDC_ICADALIAS__VP_353,			"_VIEWPOINT"/*DNT*/        ,CMDSTR_VIEWPOINT    ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"VP"/*DNT*/           ,IDC_ICADALIAS_VP_355,			"_VIEWCTL"/*DNT*/          ,CMDSTR_VIEWCTL      ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD}, // Or should this call SETVPOINT?
   {"VPOINT"/*DNT*/       ,IDC_ICADALIAS_VPOINT_356,		"_VIEWPOINT"/*DNT*/        ,CMDSTR_VIEWPOINT    ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"VPORT"/*DNT*/        ,IDC_ICADALIAS_VPORT_357,			"_VPORTS"/*DNT*/           ,CMDSTR_VPORTS       ,NULL,NULL               ,0 ,0 ,0},
   {"VS"/*DNT*/           ,IDC_ICADALIAS_VS_359,			"_VSNAPSHOT"/*DNT*/        ,CMDSTR_VSNAPSHOT    ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"VSLIDE"/*DNT*/       ,IDC_ICADALIAS_VSLIDE_361,		"_VSNAPSHOT"/*DNT*/        ,CMDSTR_VSNAPSHOT    ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"VW"/*DNT*/           ,IDC_ICADALIAS_VW_362,			"_VPORTS"/*DNT*/           ,CMDSTR_VPORTS       ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
//
   {"W"/*DNT*/            ,IDC_ICADALIAS_W_363,				"_WBLOCK"/*DNT*/           ,CMDSTR_WBLOCK       ,NULL,NULL               ,0 ,0 ,0},
   {"WE"/*DNT*/           ,IDC_ICADALIAS_WE_365,			"_WEDGE"/*DNT*/            ,CMDSTR_WEDGE        ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"WI"/*DNT*/           ,IDC_ICADALIAS_WI_366,			"_WMFIN"/*DNT*/            ,CMDSTR_WMFIN        ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"WO"/*DNT*/           ,IDC_ICADALIAS_WO_368,			"_WMFOUT"/*DNT*/           ,CMDSTR_WMFOUT       ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
//
   {"X"/*DNT*/            ,IDC_ICADALIAS_X_370,				"_EXPLODE"/*DNT*/          ,CMDSTR_EXPLODE      ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"XA"/*DNT*/           ,IDC_ICADALIAS_XA_371,			"_XREF"/*DNT*/             ,CMDSTR_XREF         ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"XATTACH"/*DNT*/      ,IDC_ICADALIAS_XATTACH_373,		"_XREF"/*DNT*/             ,CMDSTR_XREF         ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"XB"/*DNT*/           ,IDC_ICADALIAS_XB_374,			"_EXPBLOCKS"/*DNT*/        ,CMDSTR_EXPBLOCKS    ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"-XB"/*DNT*/          ,IDC_ICADALIAS__XB_375,			"_XBIND"/*DNT*/            ,CMDSTR_XBIND        ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
//   {"XC"/*DNT*/         ,IDC_ICADALIAS_XC_377,			"_XCLIP"/*DNT*/          ,CMDSTR_XCLIP        ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"CLIP"/*DNT*/         ,IDC_ICADALIAS_CLIP_378,			"_XCLIP"/*DNT*/            ,CMDSTR_XCLIP        ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"XL"/*DNT*/           ,IDC_ICADALIAS_XL_380,			"_INFLINE"/*DNT*/          ,CMDSTR_INFLINE      ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"XLINE"/*DNT*/        ,IDC_ICADALIAS_XLINE_382,			"_INFLINE"/*DNT*/          ,CMDSTR_INFLINE      ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"XR"/*DNT*/           ,IDC_ICADALIAS_XR_383,			"_EXPBLOCKS"/*DNT*/        ,CMDSTR_EXPBLOCKS    ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
   {"-XR"/*DNT*/          ,IDC_ICADALIAS__XR_384,			"_XREF"/*DNT*/             ,CMDSTR_XREF         ,NULL,NULL               ,0 ,0 ,SDS_CMDACAD},
//
   {"Z"/*DNT*/            ,IDC_ICADALIAS_Z_385,				"_ZOOM"/*DNT*/             ,CMDSTR_ZOOM         ,NULL,NULL               ,0 ,0 ,0},
};


int SDS_nAliasCmdList=sizeof(SDS_pAliasCmdList)/sizeof(SDS_pAliasCmdList[0]);
struct SafeAliasList  *SDS_AliasListBeg=NULL;

// ** Functions

int SDS_WriteAlias(char *Fname) {
    FILE     *fptr;
    struct   SDS_AliasList *Atmp;
    CString  cmd;

    if((fptr=fopen(Fname,"w"/*DNT*/))==NULL) return(RTERROR);

	fprintf(fptr,ResourceString(IDC_ICADALIAS___INTELLICAD_387, "; IntelliCAD Generated PGP file.\n"/*DNT*/));

	for(Atmp=SDS_AliasListBeg; Atmp!=NULL; Atmp=Atmp->next) {

        // Load the string from the rc file.
        cmd.LoadString(Atmp->cmdIndex);

		if(Atmp->prompt) {
			if(Atmp->crmode) {
				fprintf(fptr,"\n%s,%s,0,*%s,%i"/*DNT*/,Atmp->alias,LPCTSTR(cmd),Atmp->prompt,Atmp->retval);
			} else {
				fprintf(fptr,"\n%s,%s,0,%s,%i"/*DNT*/,Atmp->alias,LPCTSTR(cmd),Atmp->prompt,Atmp->retval);
			}
		} else {
			fprintf(fptr,"\n%s,*%s"/*DNT*/,Atmp->alias,Atmp->LName);
		}
	}
	fclose(fptr);
	return(RTNORM);
}

int SDS_ReadAlias(char *Fname) {

    char     fs1[IC_ACADBUF],*fcp1,*fcp2;
    FILE     *fptr;
    struct   SafeAliasList *tmp,*cur;
    int    IsAlias;

	for(cur=SDS_AliasListBeg; cur!=NULL && cur->next!=NULL; cur=cur->next);

    if(RTNORM!=sds_findfile(Fname,fs1)) return(RTERROR);

    if ((fptr=fopen(fs1,"rt"/*DNT*/))==NULL) return(RTERROR);

    while (ic_nfgets(fs1,sizeof(fs1)-1,fptr)) {
        IsAlias=0;
        ic_trim(fs1,"be"/*DNT*/);
        if(*fs1==';'/*DNT*/ || *fs1==','/*DNT*/ || (fcp1=strchr(fs1,','/*DNT*/))==NULL) continue;
        *fcp1=0;
        ic_trim(fcp1+1,"b"/*DNT*/);
        if(*(fcp1+1)=='*'/*DNT*/) IsAlias=1;
        
        if ((tmp=new SafeAliasList)==NULL) 
			return(RTERROR);
        if ((tmp->alias=new char [strlen(fs1)+1])==NULL) {
            delete(tmp); return(RTERROR);
        }
        strcpy(tmp->alias,fs1); ic_trim(tmp->alias,"be"/*DNT*/);

        if(IsAlias) {
			if ((tmp->LName= new char[strlen(fcp1+1)+1])==NULL) goto badcmd;
            strcpy(tmp->LName,fcp1+2);  ic_trim(tmp->LName,"be"/*DNT*/);
            if(!(*tmp->LName)) goto badcmd;
        } else {       
            // Get the command     
            if((fcp2=strchr(fcp1+1,','/*DNT*/))==NULL) goto badcmd;
            *fcp2=0;
			if((tmp->LName= new char [strlen(fcp1+1)+1])==NULL) goto badcmd;
            strcpy(tmp->LName,fcp1+1); ic_trim(tmp->LName,"be"/*DNT*/);

            // Skip over the memory.
            if((fcp1=strchr(fcp2+1,','/*DNT*/))==NULL) goto badcmd;
            *fcp1=0;

            // Get the prompt.
            if((fcp2=strchr(fcp1+1,','/*DNT*/))==NULL) goto badcmd;
            *fcp2=0;
			if((tmp->prompt= new char [strlen(fcp1+1)+1])==NULL) goto badcmd;
            strcpy(tmp->prompt,(*(fcp1+1)=='*'/*DNT*/) ? fcp1+2 : fcp1+1);
            if(*(fcp1+1)=='*'/*DNT*/) tmp->crmode=1; 

            // Get the return value.
            tmp->retval=atoi(fcp1+1);
            
            goto out;

            badcmd: ;
            delete tmp;
            continue;
        }

        out: ;
        
        if(!SDS_AliasListBeg) SDS_AliasListBeg=cur=tmp;
        else cur=cur->next=tmp;
    }
    
    fclose(fptr);

    return(RTNORM);
}

int SDS_DefaultAlias(void) {

    struct   SafeAliasList *tmp,*cur;
    CString  Cmd, localAliasString;
	for(cur=SDS_AliasListBeg; cur!=NULL && cur->next!=NULL; cur=cur->next);
    
	for(int fi1=0; fi1<SDS_nAliasCmdList; fi1++) {
		Cmd.LoadString(SDS_pAliasCmdList[fi1].cmdIndex);
		localAliasString.LoadString(SDS_pAliasCmdList[fi1].localAliasIndex);

        if ( (tmp=new SafeAliasList) ==NULL) 
			return(RTERROR);

		/* Martin Waardenburg, 26 April 2000: This will result in only English aliases.
        if(SDS_pAliasCmdList[fi1].alias)  
			db_astrncpy(&tmp->alias,SDS_pAliasCmdList[fi1].alias,(short)strlen(SDS_pAliasCmdList[fi1].alias)+1); 
		*/

		// Martin Waardenburg, 26 April 2000: Use local alias.
        if (LPCTSTR(localAliasString))  
			db_astrncpy(&tmp->alias, (char *) LPCTSTR(localAliasString), (short) localAliasString.GetLength() + 1); 

        if(LPCTSTR(Cmd))  
			db_astrncpy(&tmp->LName,(char*)LPCTSTR(Cmd),(short)Cmd.GetLength()+1); 

        if(SDS_pAliasCmdList[fi1].GName)  
			db_astrncpy(&tmp->GName,SDS_pAliasCmdList[fi1].GName,(short)strlen(SDS_pAliasCmdList[fi1].GName)+1); 

		if(SDS_pAliasCmdList[fi1].prompt) 
			db_astrncpy(&tmp->prompt,SDS_pAliasCmdList[fi1].prompt,(short)strlen(SDS_pAliasCmdList[fi1].prompt)+1); 

		tmp->retval=SDS_pAliasCmdList[fi1].retval; 
		tmp->crmode=SDS_pAliasCmdList[fi1].crmode; 
        
        if(!SDS_AliasListBeg) SDS_AliasListBeg=cur=tmp;
        else cur=cur->next=tmp;
    }    
    return(RTNORM);
}

int SDS_FreeAlias(void) {
    struct SDS_AliasList *tmp,*cur;

    for(cur=SDS_AliasListBeg; cur!=NULL; ) {
        tmp=cur; cur=cur->next;
        tmp->cmdIndex = -1;
        delete tmp->alias;
        tmp->alias = NULL;
		tmp->localAliasIndex = -1;
        if (tmp->GName != NULL) {
            delete tmp->GName;
            tmp->GName = NULL;
        }
        if (tmp->LName != NULL) {
            delete tmp->LName;
            tmp->LName = NULL;
        }
        delete tmp->prompt;
        tmp->prompt = NULL;

        delete tmp;
        tmp = NULL;
    }

    SDS_AliasListBeg=NULL;

    return(RTNORM);
}



