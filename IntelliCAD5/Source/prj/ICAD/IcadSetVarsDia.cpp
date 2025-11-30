/* F:\BLD\PRJ\ICAD\ICADSETVARSDIA.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 *
 */

//** Includes
#include "Icad.h"/*DNT*/
#include "IcadApp.h" /*DNT*/
#include "cmds.h"/*DNT*/
#include <float.h>
#include "resource.hm"/*DNT*/
#include "paths.h"/*DNT*/
#include "IcadDialog.h"/*DNT*/
#include "IcadExplorer.h"/*DNT*/
#include "IcadView.h"
#include "OptionsPropSheet.h"
#include "configure.h"
#include "UCSIcon.h"
#include "IcadLWCombo.h"
#include "IcadLWListBox.h"
#include "IcadLWComboBox.h"

//** Defines
#define SVAR_MM 69	//need this for the DV_LENSLENGTH
#define USER_ARROWS_SEPARATOR_INDEX 18
//** Protos
void svar_SubClassItems(CDialog *Cdlg);												//This is our main subclass function
LRESULT CALLBACK svar_ButtonProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);	//This is our Button Proc for processing button clicks
LRESULT CALLBACK svar_EditProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);	//This is our Edit Box Proc for processing changes in the edit box
LRESULT CALLBACK svar_StaticProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);	//This is our Static Proc for processing changes on any static field
LRESULT CALLBACK svar_ComboProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);	//This is our Combo Proc for processing changes on any static field
void svar_SpinMaster(HWND hItemWnd,int increment);

//Added Cybage AW 11-03-03
//Reason : Bug 78025 In Bugzilla.
void UpdateEditBoxes(WPARAM wParam, LPARAM lParam);

//** Externs
extern bool SDS_DontUpdateDisp;	// used by CleanupEntsInsideBlocks() in Delete nested entities.

//** Variables
int svar_iHitEnter;				// Set if the user hit enter
int svar_num_of_vars=0;			// Number of variables to search through

// This is the global variable to set what property sheet
// and child dialog will display when first bringing up
// the setvars.
// Possible values are as follows:

// AN_ANGULAR		DV_DISPLAY		EC_ATTRIBUTE   EM_CHAMFER
// AN_LINEAR		DV_ZOOM 		EC_DONUT	   EM_SPLINES
// AN_UCS			DV_NODES		EC_ELLIPSE	   EM_TRIM
//					DV_PROGRAMINT	EC_HATCH
// CI_GRID			DV_LIMITS		EC_INSERT	   TH_SHADE
// CI_SNAP							EC_PLINES	   TH_SURFACES
// CI_ENTSEL						EC_POINTS
// CI_LIMITS						EC_SHAPE
//									EC_SKETCH
//									EC_TEXT
//
//

int svar_ChildDialog=AN_LINEAR; // This is the global to restore the last page and child dialog


//globals for the tabs on the tab dialog
// DIM_ARROWS
// DIM_FORMAT
// DIM_LINES
// DIM_TEXT
// DIM_TOL //Disabled
// DIM_UNITS
// DIM_ALT //Disabled

int dim_TabDialog=DIM_ARROWS;


//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
// This is the struct that will contain all of the dialog variables, id's and
// type.
//
//	The format of this struct is as follows:
//
//	The first field is the icad setvar.
//
//	The third field is the resource ID from the dialog.
//
//	The fourth field is the type of dialog field to be set:
//		   Types:
//			  1 = Radio Buttons
//			  2 = Check Buttons
//			  3 = Edit Boxes
//			  4 = Special Cases
//			  5 = Static Text
//			  6 = Bitmaps
//			  7 = Color Boxes
//			  8 = ComboBox
//			  9 = Editable ComboBox for dimblk
//			 10 = ComboBox for DIMLWD and DIMLWE
//
//	The fifth field is the restype with some exceptions.
//	1) If the value is of restype RTREAL, you could have
//	   a distance which would be a RTREAL or an angle in
//	   which case you would enter RTANG in this field.
//	2) If the resource ID is a bitmap then this field is
//	   a zero.
//
//	The sixth field depends on the type and the restype.
//
//	1) If the type is 1(Radio) then the restype will be
//	   RTSHORT, and the Value will be the value when the
//	   ID is set to on.
//	2) If the type is 2(Check) then the restype will be
//	   RTSHORT, and the Value will be the bit set value
//	   for the on state of the ID.
//	3) If the type is set to 3(EDIT) then and the restype
//	   is RTPOINT or RT3DPOINT then the Value is the index
//	   of the point array corresponding to the ID.
//	4) Otherwise this value is zero.
//
//	The seventh field is the ID of the box to display the
//	the bitmap on. If not a bitmap then this should be zero.
//
//	/*D.G.*/ The last field is used in DimStyleSettings for flagging overridables.
//	TO DO.	Dynamic rubricating of overridables on the dialog should be implemented.
//
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

// EBATECH(CNBR) 2003/3/31 Add DIMALTRND,DIMLWD,DIMLWE and DIMDSEP.
// In addtion, Add svar_DlgItem::nGroup member to ease maintenance.(No const in OnPaint() methods.)

struct svar_DlgItem {
	LPCTSTR variable;
	int 	nGroup;
	int 	nID;
	int 	type;
	int 	ResType;
	int 	Value;
	int 	UpdImage;
	WNDPROC WndProc;
	bool	isDimVarOverriden;
};

struct svar_DlgItem *svar_CurDlgItem; //This is a pointer to the dialog vars struct
									  //We need this because the Dimension dialog
									  //and the svars dialog will use this same
									  //struct.

int svar_StupidSet(svar_DlgItem *tmpDi,HWND hWnd,short *i);
int svar_StrSet(svar_DlgItem *tmpDi,HWND hWnd);
int svar_StrGet(svar_DlgItem *tmpDi,HWND hWnd);

struct svar_DlgItem svar_DlgItemDims[] = {	/*D.G.*/// here is the index of the array which is used in
//This is for DIM_ALT						// svar_DrawingCfgDims::CheckOverriding and in OnPaint methods
	{"DIMALT"/*DNT*/	  ,DIM_ALT    ,DIM_ALT_DIMALT		   ,2,RTSHORT ,1  ,0	,NULL,	false},
	{"DIMALTD"/*DNT*/	  ,DIM_ALT    ,DIM_ALT_DIMALTD		   ,3,RTSHORT ,0  ,0	,NULL,	false},
	{"DIMALTF"/*DNT*/	  ,DIM_ALT    ,DIM_ALT_DIMALTF		   ,3,RTREAL  ,0  ,0	,NULL,	false},
	{"DIMALTTD"/*DNT*/	  ,DIM_ALT    ,DIM_ALT_DIMALTTD 	   ,3,RTSHORT ,0  ,0	,NULL,	false},
	{"DIMALTTZ"/*DNT*/	  ,DIM_ALT    ,DIM_ALT_DIMALTTZ1	   ,4,RTSHORT ,0  ,0	,NULL,	false},
	{"DIMALTTZ"/*DNT*/	  ,DIM_ALT    ,DIM_ALT_DIMALTTZ2	   ,4,RTSHORT ,0  ,0	,NULL,	false},
	{"DIMALTU"/*DNT*/	  ,DIM_ALT    ,DIM_ALT_DIMALTU		   ,8,RTSHORT ,1  ,0	,NULL,	false},
	{"DIMALTZ"/*DNT*/	  ,DIM_ALT    ,DIM_ALT_DIMALTZ1 	   ,4,RTSHORT ,0  ,0	,NULL,	false},
	{"DIMALTZ"/*DNT*/	  ,DIM_ALT    ,DIM_ALT_DIMALTZ2 	   ,4,RTSHORT ,0  ,0	,NULL,	false},
	{"DIMAPOST"/*DNT*/	  ,DIM_ALT    ,DIM_ALT_DIMAPOST1	   ,4,RTSTR   ,0  ,0	,NULL,	false},
	{"DIMAPOST"/*DNT*/	  ,DIM_ALT    ,DIM_ALT_DIMAPOST2	   ,4,RTSTR   ,0  ,0	,NULL,	false},
	{"DIMALTRND"/*DNT*/	  ,DIM_ALT    ,DIM_ALT_DIMALTRND	   ,3,RTREAL  ,0  ,0	,NULL,	false},
//This is for DIM_ARROWS
	{"DIMSAH"/*DNT*/	  ,DIM_ARROWS ,DIM_ARROWS_DIMSAH	   ,2,RTSHORT ,1  ,0	,NULL,	false},
	{"DIMASZ"/*DNT*/	  ,DIM_ARROWS ,DIM_ARROWS_DIMASZ	   ,3,RTREAL  ,0  ,0	,NULL,	false},
	{"DIMBLK1"/*DNT*/	  ,DIM_ARROWS ,DIM_ARROWS_START 	   ,9,RTSTR   ,0  ,DIM_ARROWS_PIC1	,NULL,	false},
	{"DIMBLK2"/*DNT*/	  ,DIM_ARROWS ,DIM_ARROWS_END		   ,9,RTSTR   ,0  ,DIM_ARROWS_PIC2	,NULL,	false},
	{"DIMTSZ"/*DNT*/	  ,DIM_ARROWS ,DIM_ARROWS_DIMTSZ	   ,3,RTREAL  ,0  ,0	,NULL,	false},
	{""/*DNT*/			  ,DIM_ARROWS ,DIM_ARROWS_PIC1		   ,6,0 	  ,0  ,0	,NULL,	false},
	{""/*DNT*/			  ,DIM_ARROWS ,DIM_ARROWS_PIC2		   ,6,0 	  ,0  ,0	,NULL,	false},
//This is for DIM_FORMAT
	{"DIMFIT"/*DNT*/	  ,DIM_FORMAT ,DIM_FORMAT_DIMFIT0	   ,1,RTSHORT ,0  ,0	,NULL,	false},
	{"DIMFIT"/*DNT*/	  ,DIM_FORMAT ,DIM_FORMAT_DIMFIT1	   ,1,RTSHORT ,1  ,0	,NULL,	false},
	{"DIMFIT"/*DNT*/	  ,DIM_FORMAT ,DIM_FORMAT_DIMFIT2	   ,1,RTSHORT ,2  ,0	,NULL,	false},
	{"DIMFIT"/*DNT*/	  ,DIM_FORMAT ,DIM_FORMAT_DIMFIT3	   ,1,RTSHORT ,3  ,0	,NULL,	false},
	{"DIMFIT"/*DNT*/	  ,DIM_FORMAT ,DIM_FORMAT_DIMFIT4	   ,1,RTSHORT ,4  ,0	,NULL,	false},
	{"DIMFIT"/*DNT*/	  ,DIM_FORMAT ,DIM_FORMAT_DIMFIT5	   ,1,RTSHORT ,5  ,0	,NULL,	false},
	{"DIMGAP"/*DNT*/	  ,DIM_FORMAT ,DIM_FORMAT_DIMGAP	   ,3,RTREAL  ,0  ,0	,NULL,	false},
//This is for DIM_GENERAL
//This is for DIM_LINES
	{"DIMCEN"/*DNT*/	  ,DIM_LINES  ,DIM_LINES_DIMCEN 	   ,3,RTREAL  ,0  ,0	,NULL,	false},
	{"DIMDLI"/*DNT*/	  ,DIM_LINES  ,DIM_LINES_DIMDLI 	   ,3,RTREAL  ,0  ,0	,NULL,	false},
	{"DIMDLE"/*DNT*/	  ,DIM_LINES  ,DIM_LINES_DIMDLE 	   ,3,RTREAL  ,0  ,0	,NULL,	false},
	{"DIMEXE"/*DNT*/	  ,DIM_LINES  ,DIM_LINES_DIMEXE 	   ,3,RTREAL  ,0  ,0	,NULL,	false},
	{"DIMEXO"/*DNT*/	  ,DIM_LINES  ,DIM_LINES_DIMEXO 	   ,3,RTREAL  ,0  ,0	,NULL,	false},
	{"DIMSD1"/*DNT*/	  ,DIM_LINES  ,DIM_LINES_DIMSD1 	   ,2,RTSHORT ,1  ,0	,NULL,	false},
	{"DIMSD2"/*DNT*/	  ,DIM_LINES  ,DIM_LINES_DIMSD2 	   ,2,RTSHORT ,1  ,0	,NULL,	false},
	{"DIMSE1"/*DNT*/	  ,DIM_LINES  ,DIM_LINES_DIMSE1 	   ,2,RTSHORT ,1  ,0	,NULL,	false},
	{"DIMSE2"/*DNT*/	  ,DIM_LINES  ,DIM_LINES_DIMSE2 	   ,2,RTSHORT ,1  ,0	,NULL,	false},
	{"DIMSOXD"/*DNT*/	  ,DIM_LINES  ,DIM_LINES_DIMSOXD	   ,2,RTSHORT ,1  ,0	,NULL,	false},
	{"DIMTOFL"/*DNT*/	  ,DIM_LINES  ,DIM_LINES_DIMTOFL	   ,2,RTSHORT ,1  ,0	,NULL,	false},
	{"DIMLWD"/*DNT*/	  ,DIM_LINES  ,DIM_LINES_DIMLWD		  ,10,RTSHORT ,0  ,0	,NULL,	false},
	{"DIMLWE"/*DNT*/	  ,DIM_LINES  ,DIM_LINES_DIMLWE		  ,10,RTSHORT ,0  ,0	,NULL,	false},
//This is for DIM_TEXT
	{"DIMJUST"/*DNT*/	  ,DIM_TEXT   ,DIM_TEXT_DIMJUST 	   ,8,RTSHORT ,0  ,0	,NULL,	false},
	{"DIMPOST"/*DNT*/	  ,DIM_TEXT   ,DIM_TEXT_DIMPOST1	   ,4,RTSTR   ,0  ,0	,NULL,	false},
	{"DIMPOST"/*DNT*/	  ,DIM_TEXT   ,DIM_TEXT_DIMPOST2	   ,4,RTSTR   ,0  ,0	,NULL,	false},
	{"DIMTAD"/*DNT*/	  ,DIM_TEXT   ,DIM_TEXT_DIMTAD		   ,8,RTSHORT ,0  ,0	,NULL,	false},
	{"DIMTIH"/*DNT*/	  ,DIM_TEXT   ,DIM_TEXT_DIMTIH		   ,2,RTSHORT ,1  ,0	,NULL,	false},
	{"DIMTIX"/*DNT*/	  ,DIM_TEXT   ,DIM_TEXT_DIMTIX		   ,2,RTSHORT ,1  ,0	,NULL,	false},
	{"DIMTOH"/*DNT*/	  ,DIM_TEXT   ,DIM_TEXT_DIMTOH		   ,2,RTSHORT ,1  ,0	,NULL,	false},
	{"DIMTXSTY"/*DNT*/	  ,DIM_TEXT   ,DIM_TEXT_DIMTXSTY	   ,8,RTSTR   ,0  ,0	,NULL,	false},
	{"DIMTXT"/*DNT*/	  ,DIM_TEXT   ,DIM_TEXT_DIMTXT		   ,3,RTREAL  ,0  ,0	,NULL,	false},
	{"DIMTVP"/*DNT*/	  ,DIM_TEXT   ,DIM_TEXT_DIMTVP		   ,3,RTREAL  ,0  ,0	,NULL,	false},
	{"DIMSS"			  ,DIM_TEXT   ,DIM_TEXT_SUPERSCRIPT	   ,2,RTSHORT ,1  ,0	,NULL,	false},
	{"DIMSSPOS"			  ,DIM_TEXT   ,DIM_TEXT_SUPERSCRIPTPOS ,3,RTSHORT ,0  ,0	,NULL,	false},

//This is for DIM_TOL
	{"DIMTDEC"/*DNT*/	  ,DIM_TOL    ,DIM_TOL_DIMTDEC		   ,3,RTSHORT ,0  ,0	,NULL,	false},
	{"DIMLIM"/*DNT*/	  ,DIM_TOL    ,DIM_TOL_DIMLIM		   ,2,RTSHORT ,1  ,0	,NULL,	false},
	{"DIMTFAC"/*DNT*/	  ,DIM_TOL    ,DIM_TOL_DIMTFAC		   ,3,RTREAL  ,0  ,0	,NULL,	false},
	{"DIMTM"/*DNT*/ 	  ,DIM_TOL    ,DIM_TOL_DIMTM		   ,3,RTREAL  ,0  ,0	,NULL,	false},
	{"DIMTOL"/*DNT*/	  ,DIM_TOL    ,DIM_TOL_DIMTOL		   ,2,RTSHORT ,1  ,0	,NULL,	false},
	{"DIMTOLJ"/*DNT*/	  ,DIM_TOL    ,DIM_TOL_DIMTOLJ		   ,8,RTSHORT ,0  ,0	,NULL,	false},
	{"DIMTP"/*DNT*/ 	  ,DIM_TOL    ,DIM_TOL_DIMTP		   ,3,RTREAL  ,0  ,0	,NULL,	false},
	{"DIMTZIN"/*DNT*/	  ,DIM_TOL    ,DIM_TOL_DIMTZIN		   ,4,RTSHORT ,1  ,0	,NULL,	false},
	{"DIMTZIN"/*DNT*/	  ,DIM_TOL    ,DIM_TOL_DIMTZIN2 	   ,4,RTSHORT ,1  ,0	,NULL,	false},
//This is for DIM_UNITS
	{"DIMAUNIT"/*DNT*/	  ,DIM_UNITS  ,DIM_UNITS_DIMAUNIT	   ,8,RTSHORT ,0  ,0	,NULL,	false},
	{"DIMLFAC"/*DNT*/	  ,DIM_UNITS  ,DIM_UNITS_DIMLFAC	   ,3,RTREAL  ,0  ,0	,NULL,	false},
	{"DIMDEC"/*DNT*/	  ,DIM_UNITS  ,DIM_UNITS_DIMDEC 	   ,3,RTSHORT ,0  ,0	,NULL,	false},
	{"DIMDSEP"/*DNT*/	  ,DIM_UNITS  ,DIM_UNITS_DIMDSEP	   ,3,RTSTR   ,0  ,0	,NULL,	false},
	{"DIMRND"/*DNT*/	  ,DIM_UNITS  ,DIM_UNITS_DIMRND 	   ,3,RTREAL  ,0  ,0	,NULL,	false},
	{"DIMSCALE"/*DNT*/	  ,DIM_UNITS  ,DIM_UNITS_DIMSCALE	   ,3,RTREAL  ,0  ,0	,NULL,	false},
	{"DIMUNIT"/*DNT*/	  ,DIM_UNITS  ,DIM_UNITS_DIMUNIT	   ,8,RTSHORT ,1  ,0	,NULL,	false},
	{"DIMZIN"/*DNT*/	  ,DIM_UNITS  ,DIM_UNITS_DIMZIN1	   ,4,RTSHORT ,0  ,0	,NULL,	false},
	{"DIMZIN"/*DNT*/	  ,DIM_UNITS  ,DIM_UNITS_DIMZIN2	   ,4,RTSHORT ,0  ,0	,NULL,	false},
	{"DIMADEC"/*DNT*/	  ,DIM_UNITS  ,DIM_UNITS_DIMADEC	   ,3,RTSHORT ,0  ,0	,NULL,	false},
	{"DIMAZIN"/*DNT*/	  ,DIM_UNITS  ,DIM_UNITS_DIMAZIN	   ,8,RTSHORT ,0  ,0	,NULL,	false},
	//Modified Cybage AW 16-02-01 [
	//reason : These line are added for implementing Leader selection CB on the dialog box
	{"DIMLDRBLK"	 ,DIM_ARROWS  ,LDR_ARROWS		,9,RTSTR   ,0  ,LDR_ARROWS_PIC	,NULL,	false},
	{"" 		   ,DIM_ARROWS ,LDR_ARROWS_PIC		   ,6,0 	  ,0  ,0	,NULL,	false},
	//Modified Cybage AW 16-02-01 ]
};

struct svar_DlgItem svar_DlgItemList[] = {
//This is for AN_ANGULAR
	{"AUNITS"/*DNT*/	  ,AN_ANGULAR      ,AN_AUNITS1	   ,1,RTSHORT  ,0	,AN_ANGDISPLAY	   ,NULL,	false},
	{"AUNITS"/*DNT*/	  ,AN_ANGULAR      ,AN_AUNITS2	   ,1,RTSHORT  ,1	,AN_ANGDISPLAY	   ,NULL,	false},
	{"AUNITS"/*DNT*/	  ,AN_ANGULAR      ,AN_AUNITS3	   ,1,RTSHORT  ,2	,AN_ANGDISPLAY	   ,NULL,	false},
	{"AUNITS"/*DNT*/	  ,AN_ANGULAR      ,AN_AUNITS4	   ,1,RTSHORT  ,3	,AN_ANGDISPLAY	   ,NULL,	false},
	{"AUNITS"/*DNT*/	  ,AN_ANGULAR      ,AN_AUNITS5	   ,1,RTSHORT  ,4	,AN_ANGDISPLAY	   ,NULL,	false},
	{"AUPREC"/*DNT*/	  ,AN_ANGULAR      ,AN_AUPREC	   ,3,RTSHORT  ,0	,AN_ANGDISPLAY	   ,NULL,	false},
	{"ANGBASE"/*DNT*/	  ,AN_ANGULAR      ,AN_ANGBASE	   ,3,RTANG    ,0	,AN_ANGDISPLAY2    ,NULL,	false},
	{"ANGDIR"/*DNT*/	  ,AN_ANGULAR      ,AN_ANGDIR1	   ,1,RTSHORT  ,1	,AN_ANGDISPLAY2    ,NULL,	false},
	{"ANGDIR"/*DNT*/	  ,AN_ANGULAR      ,AN_ANGDIR2	   ,1,RTSHORT  ,0	,AN_ANGDISPLAY2    ,NULL,	false},
	{"AUNITS"/*DNT*/	  ,AN_ANGULAR      ,AN_ANGDISPLAY   ,6,0 	   ,0	,0				   ,NULL,	false},
	{"ANGBASE"/*DNT*/	  ,AN_ANGULAR      ,AN_ANGDISPLAY2  ,6,0 	   ,0	,0				   ,NULL,	false},
//This is for AN_LINEAR
	{"LUNITS"/*DNT*/	  ,AN_LINEAR       ,AN_LUNITS1	   ,1,RTSHORT  ,1	,AN_LINEARDISPLAY  ,NULL,	false},
	{"LUNITS"/*DNT*/	  ,AN_LINEAR       ,AN_LUNITS2	   ,1,RTSHORT  ,2	,AN_LINEARDISPLAY  ,NULL,	false},
	{"LUNITS"/*DNT*/	  ,AN_LINEAR       ,AN_LUNITS3	   ,1,RTSHORT  ,3	,AN_LINEARDISPLAY  ,NULL,	false},
	{"LUNITS"/*DNT*/	  ,AN_LINEAR       ,AN_LUNITS4	   ,1,RTSHORT  ,4	,AN_LINEARDISPLAY  ,NULL,	false},
	{"LUNITS"/*DNT*/	  ,AN_LINEAR       ,AN_LUNITS5	   ,1,RTSHORT  ,5	,AN_LINEARDISPLAY  ,NULL,	false},
	{"LUNITS"/*DNT*/	  ,AN_LINEAR       ,AN_LUNITS6	   ,1,RTSHORT  ,6	,AN_LINEARDISPLAY  ,NULL,	false},
	{"LUPREC"/*DNT*/	  ,AN_LINEAR       ,AN_LUPREC	   ,3,RTSHORT  ,0	,AN_LINEARDISPLAY  ,NULL,	false},
	{"LUNITS"/*DNT*/	  ,AN_LINEAR       ,AN_LINEARDISPLAY,6,0 	   ,0	,0				   ,NULL,	false},
//This is for AN_TAB
	{"UNITMODE"/*DNT*/	  ,AN_TAB          ,AN_UNITMODE	   ,2,RTSHORT  ,1	,0				   ,NULL,	false},
//This is for AN_UCS
	{"UCSORG"/*DNT*/	  ,AN_UCS          ,AN_UCSORG1	   ,5,RT3DPOINT,0	,0				   ,NULL,	false},
	{"UCSORG"/*DNT*/	  ,AN_UCS          ,AN_UCSORG2	   ,5,RT3DPOINT,1	,0				   ,NULL,	false},
	{"UCSORG"/*DNT*/	  ,AN_UCS          ,AN_UCSORG3	   ,5,RT3DPOINT,2	,0				   ,NULL,	false},
	{"UCSXDIR"/*DNT*/	  ,AN_UCS          ,AN_UCSXDIR1	   ,5,RT3DPOINT,0	,0				   ,NULL,	false},
	{"UCSXDIR"/*DNT*/	  ,AN_UCS          ,AN_UCSXDIR2	   ,5,RT3DPOINT,1	,0				   ,NULL,	false},
	{"UCSXDIR"/*DNT*/	  ,AN_UCS          ,AN_UCSXDIR3	   ,5,RT3DPOINT,2	,0				   ,NULL,	false},
	{"UCSYDIR"/*DNT*/	  ,AN_UCS          ,AN_UCSYDIR1	   ,5,RT3DPOINT,0	,0				   ,NULL,	false},
	{"UCSYDIR"/*DNT*/	  ,AN_UCS          ,AN_UCSYDIR2	   ,5,RT3DPOINT,1	,0				   ,NULL,	false},
	{"UCSYDIR"/*DNT*/	  ,AN_UCS          ,AN_UCSYDIR3	   ,5,RT3DPOINT,2	,0				   ,NULL,	false},
	{"UCSICON"/*DNT*/	  ,AN_UCS          ,AN_UCSICON1	   ,1,RTSHORT  ,0	,0				   ,NULL,	false},
	{"UCSICON"/*DNT*/	  ,AN_UCS          ,AN_UCSICON2	   ,1,RTSHORT  ,1	,0				   ,NULL,	false},
	{"UCSICON"/*DNT*/	  ,AN_UCS          ,AN_UCSICON3	   ,1,RTSHORT  ,3	,0				   ,NULL,	false},
	{"UCSFOLLOW"/*DNT*/   ,AN_UCS          ,AN_UCSFOLLOW    ,2,RTSHORT  ,1	,0				   ,NULL,	false},
//This is for CI_GRID
//	  {"GRIDMODE"	 ,CI_GRID         ,CI_GRIDMODE	  ,2,RTSHORT  ,1   ,0				  ,NULL,	false},
	{"GRIDUNIT"/*DNT*/	  ,CI_GRID         ,CI_GRIDUNIT1    ,3,RTPOINT  ,0	,0				   ,NULL,	false},
	{"GRIDUNIT"/*DNT*/	  ,CI_GRID         ,CI_GRIDUNIT2    ,3,RTPOINT  ,1	,0				   ,NULL,	false},
	{"SNAPUNIT"/*DNT*/	  ,CI_GRID         ,CI_SNAPUNIT1    ,3,RTPOINT  ,0	,0				   ,NULL,	false},
	{"SNAPUNIT"/*DNT*/	  ,CI_GRID         ,CI_SNAPUNIT2    ,3,RTPOINT  ,1	,0				   ,NULL,	false},
	{"SNAPBASE"/*DNT*/	  ,CI_GRID         ,CI_SNAPBASE1    ,3,RTPOINT  ,0	,0				   ,NULL,	false},
	{"SNAPBASE"/*DNT*/	  ,CI_GRID         ,CI_SNAPBASE2    ,3,RTPOINT  ,1	,0				   ,NULL,	false},
	{"SNAPANG"/*DNT*/	  ,CI_GRID         ,CI_SNAPANG	   ,3,RTANG    ,0	,CI_ANGDISPLAY	   ,NULL,	false},
	{"SNAPANG"/*DNT*/	  ,CI_GRID         ,CI_ANGDISPLAY   ,6,0 	   ,0	,0				   ,NULL,	false},
	{"SNAPMODE"/*DNT*/	  ,CI_GRID         ,CI_SNAPMODE	   ,2,RTSHORT  ,1	,0				   ,NULL,	false},
//	  {""			   ,CI_GRID         ,CI_SYNCH		,2,RTSHORT	,0	 ,0 				,NULL,	false},
	{"SNAPSTYL"/*DNT*/	  ,CI_GRID         ,CI_SNAPSTYL	   ,2,RTSHORT  ,1	,0				   ,NULL,	false},
	{"SNAPISOPAIR"/*DNT*/ ,CI_GRID         ,CI_SNAPISOPAIR1 ,1,RTSHORT  ,1	,0				   ,NULL,	false},
	{"SNAPISOPAIR"/*DNT*/ ,CI_GRID         ,CI_SNAPISOPAIR2 ,1,RTSHORT  ,0	,0				   ,NULL,	false},
	{"SNAPISOPAIR"/*DNT*/ ,CI_GRID         ,CI_SNAPISOPAIR3 ,1,RTSHORT  ,2	,0				   ,NULL,	false},
//This is for CI_ENTSEL
	{"PICKADD"/*DNT*/	  ,CI_ENTSEL       ,CI_PICKADD	   ,2,RTSHORT  ,1	,0				   ,NULL,	false},
	{"PICKDRAG"/*DNT*/	  ,CI_ENTSEL       ,CI_PICKDRAG	   ,2,RTSHORT  ,1	,0				   ,NULL,	false},
	{"PICKSTYLE"/*DNT*/   ,CI_ENTSEL       ,CI_ENTGROUP	   ,2,RTSHORT  ,1	,0				   ,NULL,	false},
	{"PICKFIRST"/*DNT*/   ,CI_ENTSEL       ,CI_PPICK 	   ,2,RTSHORT  ,1	,0				   ,NULL,	false},
	{"PICKAUTO"/*DNT*/	  ,CI_ENTSEL       ,CI_PICKAUTO	   ,2,RTSHORT  ,1	,0				   ,NULL,	false},
	{"PICKBOX"/*DNT*/	  ,CI_ENTSEL       ,CI_PBSIZE	   ,3,RTSHORT  ,1	,CI_PBSIZE2 	   ,NULL,	false},
	{"PICKBOX"/*DNT*/	  ,CI_ENTSEL       ,CI_PBSIZE2	   ,6,0 	   ,0	,0				   ,NULL,	false},
	{"APERTURE"/*DNT*/	  ,CI_ENTSEL       ,CI_APERTURE	   ,3,RTSHORT  ,1	,CI_APERTURE2	   ,NULL,	false},
	{"PICKBOX"/*DNT*/	  ,CI_ENTSEL       ,CI_APERTURE2    ,6,0 	   ,0	,0				   ,NULL,	false},
//This is for CI_SNAP
	{"OSMODE"/*DNT*/	  ,CI_SNAP         ,CI_OSMODE0	   ,2,RTSHORT  ,16384,0 			   ,NULL,	false},
	{"OSMODE"/*DNT*/	  ,CI_SNAP         ,CI_OSMODE1	   ,2,RTSHORT  ,1	,0				   ,NULL,	false},
	{"OSMODE"/*DNT*/	  ,CI_SNAP         ,CI_OSMODE2	   ,2,RTSHORT  ,512 ,0				   ,NULL,	false},
	{"OSMODE"/*DNT*/	  ,CI_SNAP         ,CI_OSMODE3	   ,2,RTSHORT  ,128 ,0				   ,NULL,	false},
	{"OSMODE"/*DNT*/	  ,CI_SNAP         ,CI_OSMODE4	   ,2,RTSHORT  ,16	,0				   ,NULL,	false},
	{"OSMODE"/*DNT*/	  ,CI_SNAP         ,CI_OSMODE5	   ,2,RTSHORT  ,64	,0				   ,NULL,	false},
	{"OSMODE"/*DNT*/	  ,CI_SNAP         ,CI_OSMODE6	   ,2,RTSHORT  ,4	,0				   ,NULL,	false},
	{"OSMODE"/*DNT*/	  ,CI_SNAP         ,CI_OSMODE7	   ,2,RTSHORT  ,2	,0				   ,NULL,	false},
	{"OSMODE"/*DNT*/	  ,CI_SNAP         ,CI_OSMODE8	   ,2,RTSHORT  ,8	,0				   ,NULL,	false},
	{"OSMODE"/*DNT*/	  ,CI_SNAP         ,CI_OSMODE9	   ,2,RTSHORT  ,256 ,0				   ,NULL,	false},
	{"OSMODE"/*DNT*/	  ,CI_SNAP         ,CI_OSMODE11	   ,2,RTSHORT  ,1024,0				   ,NULL,	false},
//	  {"OSMODE" 	 ,CI_SNAP         ,CI_OSMODECLEAR  ,2,RTSHORT  ,0   ,0				  ,NULL,	false},  // This is a Push Button, but I'm lying and calling it a Check Box
//This is for CI_LIMITS
	{"LIMMAX"/*DNT*/	  ,CI_LIMITS       ,CI_LIMMAX1	   ,3,RTPOINT  ,0	,0				   ,NULL,	false},
	{"LIMMAX"/*DNT*/	  ,CI_LIMITS       ,CI_LIMMAX2	   ,3,RTPOINT  ,1	,0				   ,NULL,	false},
	{"LIMMIN"/*DNT*/	  ,CI_LIMITS       ,CI_LIMMIN1	   ,3,RTPOINT  ,0	,0				   ,NULL,	false},
	{"LIMMIN"/*DNT*/	  ,CI_LIMITS       ,CI_LIMMIN2	   ,3,RTPOINT  ,1	,0				   ,NULL,	false},
	{"LIMCHECK"/*DNT*/	  ,CI_LIMITS       ,CI_LIMCHECK	   ,2,RTSHORT  ,1	,0				   ,NULL,	false},
//This is for CI_TAB
	{"ORTHOMODE"/*DNT*/   ,CI_TAB          ,CI_ORTHOMODE    ,2,RTSHORT  ,1	,0				   ,NULL,	false},
	{"TABMODE"/*DNT*/	  ,CI_TAB          ,CI_TABMODE	   ,2,RTSHORT  ,1	,0				   ,NULL,	false},
	{"LIMCHECK"/*DNT*/	  ,CI_TAB          ,CI_LIMCHECK	   ,2,RTSHORT  ,1	,0				   ,NULL,	false},
//This is for DV_DISPLAY
	{"FILLMODE"/*DNT*/	  ,DV_DISPLAY      ,DV_FILLMODE	   ,2,RTSHORT  ,1	,0				   ,NULL,	false},
	{"QTEXTMODE"/*DNT*/   ,DV_DISPLAY      ,DV_QTEXTMODE    ,2,RTSHORT  ,1	,0				   ,NULL,	false},
	{"HIGHLIGHT"/*DNT*/   ,DV_DISPLAY      ,DV_HIGHLIGHT    ,2,RTSHORT  ,1	,0				   ,NULL,	false},
	{"BLIPMODE"/*DNT*/	  ,DV_DISPLAY      ,DV_BLIPMODE	   ,2,RTSHORT  ,1	,0				   ,NULL,	false},
	{"DRAGMODE"/*DNT*/	  ,DV_DISPLAY      ,DV_DRAGMODE1    ,1,RTSHORT  ,0	,0				   ,NULL,	false},
	{"DRAGMODE"/*DNT*/	  ,DV_DISPLAY      ,DV_DRAGMODE2    ,1,RTSHORT  ,1	,0				   ,NULL,	false},
	{"DRAGMODE"/*DNT*/	  ,DV_DISPLAY      ,DV_DRAGMODE3    ,1,RTSHORT  ,2	,0				   ,NULL,	false},
	{"UCSICON"/*DNT*/	  ,DV_DISPLAY      ,DV_UCSICON1	   ,1,RTSHORT  ,0	,0				   ,NULL,	false},
	{"UCSICON"/*DNT*/	  ,DV_DISPLAY      ,DV_UCSICON2	   ,1,RTSHORT  ,1	,0				   ,NULL,	false},
	{"UCSICON"/*DNT*/	  ,DV_DISPLAY      ,DV_UCSICON3	   ,1,RTSHORT  ,3	,0				   ,NULL,	false},
//This is for DV_ZOOM
	{"VIEWMODE"/*DNT*/	  ,DV_ZOOM         ,DV_VIEWMODE1    ,5,RTSHORT  ,1	,0				   ,NULL,	false},
	{"VIEWMODE"/*DNT*/	  ,DV_ZOOM         ,DV_VIEWMODE2    ,5,RTSHORT  ,2	,0				   ,NULL,	false},
	{"VIEWMODE"/*DNT*/	  ,DV_ZOOM         ,DV_VIEWMODE3    ,5,RTSHORT  ,4	,0				   ,NULL,	false},
	{"VIEWMODE"/*DNT*/	  ,DV_ZOOM         ,DV_VIEWMODE4    ,5,RTSHORT  ,8	,0				   ,NULL,	false},
	{"VIEWCTR"/*DNT*/	  ,DV_ZOOM         ,DV_VIEWCTR1	   ,5,RT3DPOINT,0	,0				   ,NULL,	false},
	{"VIEWCTR"/*DNT*/	  ,DV_ZOOM         ,DV_VIEWCTR2	   ,5,RT3DPOINT,1	,0				   ,NULL,	false},
	{"VIEWCTR"/*DNT*/	  ,DV_ZOOM         ,DV_VIEWCTR3	   ,5,RT3DPOINT,2	,0				   ,NULL,	false},
	{"VIEWDIR"/*DNT*/	  ,DV_ZOOM         ,DV_VIEWDIR1	   ,5,RT3DPOINT,0	,0				   ,NULL,	false},
	{"VIEWDIR"/*DNT*/	  ,DV_ZOOM         ,DV_VIEWDIR2	   ,5,RT3DPOINT,1	,0				   ,NULL,	false},
	{"VIEWDIR"/*DNT*/	  ,DV_ZOOM         ,DV_VIEWDIR3	   ,5,RT3DPOINT,2	,0				   ,NULL,	false},
	{"TARGET"/*DNT*/	  ,DV_ZOOM         ,DV_TARGET1	   ,5,RT3DPOINT,0	,0				   ,NULL,	false},
	{"TARGET"/*DNT*/	  ,DV_ZOOM         ,DV_TARGET2	   ,5,RT3DPOINT,1	,0				   ,NULL,	false},
	{"TARGET"/*DNT*/	  ,DV_ZOOM         ,DV_TARGET3	   ,5,RT3DPOINT,2	,0				   ,NULL,	false},
	{"FRONTZ"/*DNT*/	  ,DV_ZOOM         ,DV_FRONTZ	   ,5,RTREAL   ,0	,0				   ,NULL,	false},
	{"BACKZ"/*DNT*/ 	  ,DV_ZOOM         ,DV_BACKZ 	   ,5,RTREAL   ,0	,0				   ,NULL,	false},
	{"VIEWTWIST"/*DNT*/   ,DV_ZOOM         ,DV_VIEWTWIST    ,5,RTANG    ,0	,0				   ,NULL,	false},
	{"LENSLENGTH"/*DNT*/  ,DV_ZOOM         ,DV_LENSLENGTH   ,5,SVAR_MM  ,0	,0				   ,NULL,	false},
	{"VIEWSIZE"/*DNT*/	  ,DV_ZOOM         ,DV_VIEWSIZE	   ,5,RTREAL   ,0	,0				   ,NULL,	false},
//This is for DV_NODES
//	  {"GRIPS"		 ,DV_NODES        ,DV_GRIPS		  ,2,RTSHORT  ,1   ,0				  ,NULL,	false},
	{"GRIPBLOCK"/*DNT*/   ,DV_NODES        ,DV_GRIPBLOCK    ,2,RTSHORT  ,1	,0				   ,NULL,	false},
	{"GRIPSIZE"/*DNT*/	  ,DV_NODES        ,DV_GRIPSIZE	   ,3,RTSHORT  ,0	,DV_NODEDISPLAY    ,NULL,	false},
	{"GRIPSIZE"/*DNT*/	  ,DV_NODES        ,DV_NODEDISPLAY  ,6,0 	   ,0	,0				   ,NULL,	false},
	{"GRIPCOLOR"/*DNT*/   ,DV_NODES        ,DV_NONSELCOLOR  ,7,0 	   ,0	,0				   ,NULL,	false},
	{"GRIPHOT"/*DNT*/	  ,DV_NODES        ,DV_SELCOLOR	   ,7,0 	   ,0	,0				   ,NULL,	false},
//This is for DV_PROGRAMINT
	{"FILEDIA"/*DNT*/	  ,DV_PROGRAMINT   ,DV_FILEDIA	   ,2,RTSHORT  ,1	,0				   ,NULL,	false},
	{"CMDDIA"/*DNT*/	  ,DV_PROGRAMINT   ,DV_CMDDIA	   ,2,RTSHORT  ,1	,0				   ,NULL,	false},
	{"CMDECHO"/*DNT*/	  ,DV_PROGRAMINT   ,DV_CMDECHO	   ,2,RTSHORT  ,1	,0				   ,NULL,	false},
	{"MENUECHO"/*DNT*/	  ,DV_PROGRAMINT   ,DV_MENUECHO1    ,2,RTSHORT  ,1	,0				   ,NULL,	false},
	{"MENUECHO"/*DNT*/	  ,DV_PROGRAMINT   ,DV_MENUECHO2    ,2,RTSHORT  ,2	,0				   ,NULL,	false},
	{"UNDOCTL"/*DNT*/	  ,DV_PROGRAMINT   ,DV_UNDOCTL1	   ,2,RTSHORT  ,1	,0				   ,NULL,	false},
	{"UNDOCTL"/*DNT*/	  ,DV_PROGRAMINT   ,DV_UNDOCTL2	   ,2,RTSHORT  ,2	,0				   ,NULL,	false},
	{"UNDOCTL"/*DNT*/	  ,DV_PROGRAMINT   ,DV_UNDOCTL3	   ,2,RTSHORT  ,4	,0				   ,NULL,	false},
	{"TEXTEVAL"/*DNT*/	  ,DV_PROGRAMINT   ,DV_TEXTEVAL1    ,1,RTSHORT  ,0	,0				   ,NULL,	false},
	{"TEXTEVAL"/*DNT*/	  ,DV_PROGRAMINT   ,DV_TEXTEVAL2    ,1,RTSHORT  ,1	,0				   ,NULL,	false},
//This is for DV_TAB
	{"COORDS"/*DNT*/	  ,DV_TAB          ,DV_COORDS1	   ,1,RTSHORT  ,0	,0				   ,NULL,	false},
	{"COORDS"/*DNT*/	  ,DV_TAB          ,DV_COORDS2	   ,1,RTSHORT  ,1	,0				   ,NULL,	false},
	{"COORDS"/*DNT*/	  ,DV_TAB          ,DV_COORDS3	   ,1,RTSHORT  ,2	,0				   ,NULL,	false},
//This is for DV_LIMITS
	{"LIMMAX"/*DNT*/	  ,DV_LIMITS       ,DV_LIMMAX1	   ,3,RTPOINT  ,0	,0				   ,NULL,	false},
	{"LIMMAX"/*DNT*/	  ,DV_LIMITS       ,DV_LIMMAX2	   ,3,RTPOINT  ,1	,0				   ,NULL,	false},
	{"LIMMIN"/*DNT*/	  ,DV_LIMITS       ,DV_LIMMIN1	   ,3,RTPOINT  ,0	,0				   ,NULL,	false},
	{"LIMMIN"/*DNT*/	  ,DV_LIMITS       ,DV_LIMMIN2	   ,3,RTPOINT  ,1	,0				   ,NULL,	false},
	{"LIMCHECK"/*DNT*/	  ,DV_LIMITS       ,DV_LIMCHECK	   ,2,RTSHORT  ,1	,0				   ,NULL,	false},
//This is for EC_ATTRIBUTE
	{"AFLAGS"/*DNT*/	  ,EC_ATTRIBUTE    ,EC_AFLAGSI	   ,2,RTSHORT  ,1	,0				   ,NULL,	false},
	{"AFLAGS"/*DNT*/	  ,EC_ATTRIBUTE    ,EC_AFLAGSC	   ,2,RTSHORT  ,2	,0				   ,NULL,	false},
	{"AFLAGS"/*DNT*/	  ,EC_ATTRIBUTE    ,EC_AFLAGSV	   ,2,RTSHORT  ,4	,0				   ,NULL,	false},
	{"AFLAGS"/*DNT*/	  ,EC_ATTRIBUTE    ,EC_AFLAGSP	   ,2,RTSHORT  ,8	,0				   ,NULL,	false},
	{"ATTREQ"/*DNT*/	  ,EC_ATTRIBUTE    ,EC_ATTREQ1	   ,1,RTSHORT  ,0	,0				   ,NULL,	false},
	{"ATTREQ"/*DNT*/	  ,EC_ATTRIBUTE    ,EC_ATTREQ2	   ,1,RTSHORT  ,1	,0				   ,NULL,	false},
	{"ATTDIA"/*DNT*/	  ,EC_ATTRIBUTE    ,EC_ATTDIA1	   ,1,RTSHORT  ,0	,0				   ,NULL,	false},
	{"ATTDIA"/*DNT*/	  ,EC_ATTRIBUTE    ,EC_ATTDIA2	   ,1,RTSHORT  ,1	,0				   ,NULL,	false},
	{"ATTMODE"/*DNT*/	  ,EC_ATTRIBUTE    ,EC_ATTMODE1	   ,1,RTSHORT  ,2	,0				   ,NULL,	false},
	{"ATTMODE"/*DNT*/	  ,EC_ATTRIBUTE    ,EC_ATTMODE2	   ,1,RTSHORT  ,0	,0				   ,NULL,	false},
	{"ATTMODE"/*DNT*/	  ,EC_ATTRIBUTE    ,EC_ATTMODE3	   ,1,RTSHORT  ,1	,0				   ,NULL,	false},
//This is for EC_DONUT
	{"DONUTOD"/*DNT*/	  ,EC_DONUT        ,EC_DONUTOD	   ,3,RTREAL   ,0	,0				   ,NULL,	false},
	{"DONUTID"/*DNT*/	  ,EC_DONUT        ,EC_DONUTID	   ,3,RTREAL   ,0	,0				   ,NULL,	false},
//This is for EC_ELLIPSE
	{"PELLIPSE"/*DNT*/	  ,EC_ELLIPSE      ,EC_PELLIPSE1    ,1,RTSHORT  ,0	,0				   ,NULL,	false},
	{"PELLIPSE"/*DNT*/	  ,EC_ELLIPSE      ,EC_PELLIPSE2    ,1,RTSHORT  ,1	,0				   ,NULL,	false},
//This is for EC_HATCH
	{"HPANG"/*DNT*/ 	  ,EC_HATCH        ,EC_HPANG 	   ,3,RTANG    ,0	,0				   ,NULL,	false},
	{"HPDOUBLE"/*DNT*/	  ,EC_HATCH        ,EC_HPDOUBLE	   ,2,RTSHORT  ,1	,0				   ,NULL,	false},
	{"HPSPACE"/*DNT*/	  ,EC_HATCH        ,EC_HPSPACE	   ,3,RTREAL   ,0	,0				   ,NULL,	false},
	{"HPSCALE"/*DNT*/	  ,EC_HATCH        ,EC_HPSCALE	   ,3,RTREAL   ,0	,0				   ,NULL,	false},
	{"HPSTYLE"/*DNT*/	  ,EC_HATCH        ,EC_HPSTYLE1	   ,1,RTSHORT  ,0	,EC_HATCH_STYLE    ,NULL,	false},
	{"HPSTYLE"/*DNT*/	  ,EC_HATCH        ,EC_HPSTYLE2	   ,1,RTSHORT  ,1	,EC_HATCH_STYLE    ,NULL,	false},
	{"HPSTYLE"/*DNT*/	  ,EC_HATCH        ,EC_HPSTYLE3	   ,1,RTSHORT  ,2	,EC_HATCH_STYLE    ,NULL,	false},
	{"HPSTYLE"/*DNT*/	  ,EC_HATCH        ,EC_HATCH_STYLE  ,6,0 	   ,0	,0				   ,NULL,	false},
//This is for EC_INSERT
	{"INSNAME"/*DNT*/	  ,EC_INSERT       ,EC_INSNAME	   ,3,RTSTR    ,0	,0				   ,NULL,	false},
	{"INSBASE"/*DNT*/	  ,EC_INSERT       ,EC_INSBASE1	   ,3,RT3DPOINT,0	,0				   ,NULL,	false},
	{"INSBASE"/*DNT*/	  ,EC_INSERT       ,EC_INSBASE2	   ,3,RT3DPOINT,1	,0				   ,NULL,	false},
	{"INSBASE"/*DNT*/	  ,EC_INSERT       ,EC_INSBASE3	   ,3,RT3DPOINT,2	,0				   ,NULL,	false},
//This is for EC_PLINES
	{"PLINEWID"/*DNT*/	  ,EC_PLINES       ,EC_PLINEWID	   ,3,RTREAL   ,0	,0				   ,NULL,	false},
	{"TRACEWID"/*DNT*/	  ,EC_PLINES       ,EC_TRACEWID	   ,3,RTREAL   ,0	,0				   ,NULL,	false},
	{"PLINEGEN"/*DNT*/	  ,EC_PLINES       ,EC_PLINEGEN	   ,2,RTSHORT  ,1	,EC_LINETYPEDISP   ,NULL,	false},
	{"PLINEGEN"/*DNT*/	  ,EC_PLINES       ,EC_LINETYPEDISP ,6,0 	   ,0	,0				   ,NULL,	false},
//This is for EC_POINTS
	{"PDSIZE"/*DNT*/	  ,EC_POINTS       ,EC_PDSIZE	   ,3,RTREAL   ,0	,0				   ,NULL,	false},
	{"PDMODE"/*DNT*/	  ,EC_POINTS       ,EC_PDM0		   ,1,RTSHORT  ,0	,0				   ,NULL,	false},
	{"PDMODE"/*DNT*/	  ,EC_POINTS       ,EC_PDM1		   ,1,RTSHORT  ,1	,0				   ,NULL,	false},
	{"PDMODE"/*DNT*/	  ,EC_POINTS       ,EC_PDM2		   ,1,RTSHORT  ,2	,0				   ,NULL,	false},
	{"PDMODE"/*DNT*/	  ,EC_POINTS       ,EC_PDM3		   ,1,RTSHORT  ,3	,0				   ,NULL,	false},
	{"PDMODE"/*DNT*/	  ,EC_POINTS       ,EC_PDM4		   ,1,RTSHORT  ,4	,0				   ,NULL,	false},
	{"PDMODE"/*DNT*/	  ,EC_POINTS       ,EC_PDM32 	   ,1,RTSHORT  ,32	,0				   ,NULL,	false},
	{"PDMODE"/*DNT*/	  ,EC_POINTS       ,EC_PDM33 	   ,1,RTSHORT  ,33	,0				   ,NULL,	false},
	{"PDMODE"/*DNT*/	  ,EC_POINTS       ,EC_PDM34 	   ,1,RTSHORT  ,34	,0				   ,NULL,	false},
	{"PDMODE"/*DNT*/	  ,EC_POINTS       ,EC_PDM35 	   ,1,RTSHORT  ,35	,0				   ,NULL,	false},
	{"PDMODE"/*DNT*/	  ,EC_POINTS       ,EC_PDM36 	   ,1,RTSHORT  ,36	,0				   ,NULL,	false},
	{"PDMODE"/*DNT*/	  ,EC_POINTS       ,EC_PDM64 	   ,1,RTSHORT  ,64	,0				   ,NULL,	false},
	{"PDMODE"/*DNT*/	  ,EC_POINTS       ,EC_PDM65 	   ,1,RTSHORT  ,65	,0				   ,NULL,	false},
	{"PDMODE"/*DNT*/	  ,EC_POINTS       ,EC_PDM66 	   ,1,RTSHORT  ,66	,0				   ,NULL,	false},
	{"PDMODE"/*DNT*/	  ,EC_POINTS       ,EC_PDM67 	   ,1,RTSHORT  ,67	,0				   ,NULL,	false},
	{"PDMODE"/*DNT*/	  ,EC_POINTS       ,EC_PDM68 	   ,1,RTSHORT  ,68	,0				   ,NULL,	false},
	{"PDMODE"/*DNT*/	  ,EC_POINTS       ,EC_PDM96 	   ,1,RTSHORT  ,96	,0				   ,NULL,	false},
	{"PDMODE"/*DNT*/	  ,EC_POINTS       ,EC_PDM97 	   ,1,RTSHORT  ,97	,0				   ,NULL,	false},
	{"PDMODE"/*DNT*/	  ,EC_POINTS       ,EC_PDM98 	   ,1,RTSHORT  ,98	,0				   ,NULL,	false},
	{"PDMODE"/*DNT*/	  ,EC_POINTS       ,EC_PDM99 	   ,1,RTSHORT  ,99	,0				   ,NULL,	false},
	{"PDMODE"/*DNT*/	  ,EC_POINTS       ,EC_PDM100	   ,1,RTSHORT  ,100 ,0				   ,NULL,	false},
//This is for EC_SHAPE
	{"SHPNAME"/*DNT*/	  ,EC_SHAPE        ,EC_SHPNAME	   ,3,RTSTR    ,0	,0				   ,NULL,	false},
//This is for EC_SKETCH
	{"SKETCHINC"/*DNT*/   ,EC_SKETCH       ,EC_SKETCHINC    ,3,RTREAL   ,0	,0				   ,NULL,	false},
	{"SKPOLY"/*DNT*/	  ,EC_SKETCH       ,EC_SKPOLY1	   ,1,RTSHORT  ,1	,EC_SKETCHDISP	   ,NULL,	false},
	{"SKPOLY"/*DNT*/	  ,EC_SKETCH       ,EC_SKPOLY2	   ,1,RTSHORT  ,0	,EC_SKETCHDISP	   ,NULL,	false},
	{"SKPOLY"/*DNT*/	  ,EC_SKETCH       ,EC_SKETCHDISP   ,6,0 	   ,0	,0				   ,NULL,	false},
//This is for EC_TAB
	{"CELTSCALE"/*DNT*/   ,EC_TAB          ,EC_CELTSCALE    ,3,RTREAL   ,0	,0				   ,NULL,	false},
	{"CECOLOR"/*DNT*/	  ,EC_TAB          ,EC_COLOR 	   ,7,0 	   ,0	,0				   ,NULL,	false},
	{"LTSCALE"/*DNT*/	  ,EC_TAB          ,EC_LTSCALE	   ,3,RTREAL   ,0	,0				   ,NULL,	false},
//This is for EC_TEXT
	{"TEXTSIZE"/*DNT*/	  ,EC_TEXT         ,EC_TEXTSIZE	   ,3,RTREAL   ,0	,/*EC_TEXTDISP*/0  ,NULL,	false},
	{"TEXTSTYLE"/*DNT*/   ,EC_TEXT         ,EC_TEXTSTYLE    ,8,RTSTR    ,0	,/*EC_TEXTDISP*/0  ,NULL,	false},
//	  {""			 ,EC_TEXT         ,EC_TEXTDISP	  ,6,0		  ,0   ,0				  ,NULL,	false},
//This is for EM_CHAMFER
	{"TRIMMODE"/*DNT*/	  ,EM_CHAMFER      ,EM_TRIMMODE1    ,1,RTSHORT  ,1	,EM_CHAMDISPLAY    ,NULL,	false},
	{"TRIMMODE"/*DNT*/	  ,EM_CHAMFER      ,EM_TRIMMODE2    ,1,RTSHORT  ,0	,EM_CHAMDISPLAY    ,NULL,	false},
	{"CHAMMODE"/*DNT*/	  ,EM_CHAMFER      ,EM_CHAMMODE1    ,1,RTSHORT  ,0	,EM_CHAMDISPLAY    ,NULL,	false},
	{"CHAMMODE"/*DNT*/	  ,EM_CHAMFER      ,EM_CHAMMODE2    ,1,RTSHORT  ,1	,EM_CHAMDISPLAY    ,NULL,	false},
	{"CHAMFERA"/*DNT*/	  ,EM_CHAMFER      ,EM_CHAMFERA	   ,3,RTREAL   ,0	,0				   ,NULL,	false},
	{"CHAMFERB"/*DNT*/	  ,EM_CHAMFER      ,EM_CHAMFERB	   ,3,RTREAL   ,0	,0				   ,NULL,	false},
	{"CHAMFERC"/*DNT*/	  ,EM_CHAMFER      ,EM_CHAMFERC	   ,3,RTREAL   ,0	,0				   ,NULL,	false},
	{"CHAMFERD"/*DNT*/	  ,EM_CHAMFER      ,EM_CHAMFERD	   ,3,RTANG    ,0	,0				   ,NULL,	false},
	{"FILLETRAD"/*DNT*/   ,EM_CHAMFER      ,EM_FILLETRAD    ,3,RTREAL   ,0	,0				   ,NULL,	false},
	{"CHAMMODE"/*DNT*/	  ,EM_CHAMFER      ,EM_CHAMDISPLAY  ,6,0 	   ,0	,0				   ,NULL,	false},
//This is for EM_TAB
	{"MIRRTEXT"/*DNT*/	  ,EM_TAB          ,EM_MIRRTEXT1    ,1,RTSHORT  ,1	,EM_MIRRDISPLAY    ,NULL,	false},
	{"MIRRTEXT"/*DNT*/	  ,EM_TAB          ,EM_MIRRTEXT2    ,1,RTSHORT  ,0	,EM_MIRRDISPLAY    ,NULL,	false},
	{"OFFSETDIST"/*DNT*/  ,EM_TAB          ,EM_OFFSETDIST   ,3,RTREAL   ,0	,0				   ,NULL,	false},
	{"MIRRTEXT"/*DNT*/	  ,EM_TAB          ,EM_MIRRDISPLAY  ,6,0 	   ,0	,0				   ,NULL,	false},
//This is for EM_SPLINE
	{"SPLINESEGS"/*DNT*/  ,EM_SPLINESEGS   ,EM_SPLINESEGS   ,3,RTSHORT  ,0	,0				   ,NULL,	false},
	{"SPLINETYPE"/*DNT*/  ,EM_SPLINESEGS   ,EM_SPLINETYPE   ,3,RTSHORT  ,0	,0				   ,NULL,	false},
	{"SPLFRAME"/*DNT*/	  ,EM_SPLINESEGS   ,EM_SPLFRAME	   ,2,RTSHORT  ,1	,EM_DISPLAY_BMP    ,NULL,	false},
	{"SPLFRAME"/*DNT*/	  ,EM_SPLINESEGS   ,EM_DISPLAY_BMP  ,6,0 	   ,0	,0				   ,NULL,	false},
	{"FITTYPE"/*DNT*/	  ,EM_SPLINESEGS   ,EM_FITTYPE1	   ,1,RTSHORT  ,2	,EM_FIT_BMP 	   ,NULL,	false},
	{"FITTYPE"/*DNT*/	  ,EM_SPLINESEGS   ,EM_FITTYPE2	   ,1,RTSHORT  ,3	,EM_FIT_BMP 	   ,NULL,	false},
	{"FITTYPE"/*DNT*/	  ,EM_SPLINESEGS   ,EM_FIT_BMP	   ,6,0 	   ,0	,0				   ,NULL,	false},
//This is for EM_TRIM
	{"PROJMODE"/*DNT*/	  ,EM_TRIM         ,EM_PROJMODE1    ,1,RTSHORT  ,0	,0				   ,NULL,	false},
	{"PROJMODE"/*DNT*/	  ,EM_TRIM         ,EM_PROJMODE2    ,1,RTSHORT  ,1	,0				   ,NULL,	false},
	{"PROJMODE"/*DNT*/	  ,EM_TRIM         ,EM_PROJMODE3    ,1,RTSHORT  ,2	,0				   ,NULL,	false},
	{"EDGEMODE"/*DNT*/	  ,EM_TRIM         ,EM_EDGEMODE	   ,2,RTSHORT  ,1	,0				   ,NULL,	false},
//This is for TH_SHADE
	{"SHADEDGE"/*DNT*/	  ,TH_SHADE        ,TH_SHADEDGE1    ,1,RTSHORT  ,0	,0				   ,NULL,	false},
	{"SHADEDGE"/*DNT*/	  ,TH_SHADE        ,TH_SHADEDGE2    ,1,RTSHORT  ,1	,0				   ,NULL,	false},
	{"SHADEDGE"/*DNT*/	  ,TH_SHADE        ,TH_SHADEDGE3    ,1,RTSHORT  ,2	,0				   ,NULL,	false},
	{"SHADEDGE"/*DNT*/	  ,TH_SHADE        ,TH_SHADEDGE4    ,1,RTSHORT  ,3	,0				   ,NULL,	false},
//This is for TH_SURFACES
	{"SURFTAB1"/*DNT*/	  ,TH_SURFACES     ,TH_SURFTAB1	   ,3,RTSHORT  ,0	,0				   ,NULL,	false},
	{"SURFTAB2"/*DNT*/	  ,TH_SURFACES     ,TH_SURFTAB2	   ,3,RTSHORT  ,0	,0				   ,NULL,	false},
	{"SURFU"/*DNT*/ 	  ,TH_SURFACES     ,TH_SURFU 	   ,3,RTSHORT  ,0	,0				   ,NULL,	false},
	{"SURFV"/*DNT*/ 	  ,TH_SURFACES     ,TH_SURFV 	   ,3,RTSHORT  ,0	,0				   ,NULL,	false},
	{"SURFTYPE"/*DNT*/	  ,TH_SURFACES     ,TH_SURFTYPE1    ,1,RTSHORT  ,5	,0				   ,NULL,	false},
	{"SURFTYPE"/*DNT*/	  ,TH_SURFACES     ,TH_SURFTYPE2    ,1,RTSHORT  ,6	,0				   ,NULL,	false},
	{"SURFTYPE"/*DNT*/	  ,TH_SURFACES     ,TH_SURFTYPE3    ,1,RTSHORT  ,8	,0				   ,NULL,	false},
//This is for TH_TAB
	{"THICKNESS"/*DNT*/   ,TH_TAB          ,TH_THICKNESS    ,3,RTREAL   ,0	,0				   ,NULL,	false},
	{"ELEVATION"/*DNT*/   ,TH_TAB          ,TH_ELEVATION    ,3,RTREAL   ,0	,0				   ,NULL,	false},
	{"UCSFOLLOW"/*DNT*/   ,TH_TAB          ,TH_UCSFOLLOW    ,2,RTSHORT  ,1	,0				   ,NULL,	false}
};


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//****************************************************************************
// This section is for the Setvars property sheets and related dialogs
//****************************************************************************
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

//Added Cybage AW 11-03-03 [
//Reason : Bug 78025 In Bugzilla
void UpdateEditBoxes(WPARAM wParam, LPARAM lParam)
{
	struct svar_DlgItem *tmpDi=NULL;
	struct resbuf rb;
	char strVal[IC_ACADNMLEN];

	CString strTemp;
	int nFormat = 2;
	sds_getvar("LUNITS",&rb);
	if(rb.restype == SDS_RTSHORT)
		nFormat = rb.resval.rint;

	tmpDi=(struct svar_DlgItem *)GetWindowLong(HWND(lParam),GWL_USERDATA);
	if(tmpDi==NULL) return;
	sds_getvar(tmpDi->variable,&rb);

	::GetWindowText(HWND(lParam), strVal, ::GetWindowTextLength(HWND(lParam)) + 1);	//Extra 1 for NULL char

	switch(tmpDi->ResType)
	{
		case SDS_RTREAL:
			sds_distof(strVal,nFormat,&rb.resval.rreal);
			break;
		case SDS_RTSTR:
			strcpy(rb.resval.rstring, strVal);
			break;
		case SDS_RTSHORT:
			sds_distof(strVal,nFormat,&rb.resval.rreal);
			rb.resval.rint = (int)rb.resval.rreal;
			break;
	}
	sds_setvar(tmpDi->variable,&rb);
}
//Added Cybage AW 11-03-03 ]


//*****This function will control the spiners
void svar_SpinMaster(HWND hItemWnd,int increment) {
	struct resbuf rb;
	struct svar_DlgItem *tmpDi=NULL;
	HWND hItemWnd1;

	tmpDi=(struct svar_DlgItem *)GetWindowLong(hItemWnd,GWL_USERDATA);
	if(tmpDi==NULL) return;
	sds_getvar(tmpDi->variable,&rb);
	switch(tmpDi->ResType) {
		case RTSHORT:
			if(rb.resval.rint>SHRT_MIN&&rb.resval.rint<SHRT_MAX)
			{
				rb.resval.rint+=(-increment);
			}
			break;
		case RTREAL:
			if(fabs(rb.resval.rreal<DBL_MAX))
			{
				rb.resval.rreal+=(-increment*0.1);
				if(fabs(rb.resval.rreal)<CMD_FUZZ) rb.resval.rreal=0.0;
			}
			break;
		case RTPOINT:
		case RT3DPOINT:
			if(fabs(rb.resval.rreal<DBL_MAX))
			{
				rb.resval.rpoint[tmpDi->Value]+=(-increment*0.1);
				if(fabs(rb.resval.rpoint[tmpDi->Value])<CMD_FUZZ) rb.resval.rpoint[tmpDi->Value]=0.0;
			}
			break;
		case RTANG:
			if(fabs(rb.resval.rreal<DBL_MAX))
			{
				rb.resval.rreal+=(IC_PI/180)*(-increment);	//increment by degrees
				if(fabs(rb.resval.rreal)<CMD_FUZZ) rb.resval.rreal=0.0;
			}
			break;
	}
	sds_setvar(tmpDi->variable,&rb);
	::SendMessage(hItemWnd,WM_PAINT,0,0);
	if(tmpDi->UpdImage) { //Update the associated image
		hItemWnd1=GetDlgItem(GetParent(hItemWnd),tmpDi->UpdImage);
		SendMessage(hItemWnd1,WM_PAINT,0,0);
	}
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//************************************************************************
// This section controls the Drawing Units property page and its sub dialogs
//************************************************************************
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

//******This sub section will control the Angular sub dialog

class svar_AN_ANGULAR : public CDialog {
	public:
		BOOL OnInitDialog();
		svar_AN_ANGULAR():CDialog(AN_ANGULAR) {};
	protected:
		afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
		virtual void OnOK();
		virtual void OnCancel();
		afx_msg void OnSpinAngPrec(NMHDR *pNotifyStruct,LRESULT *result);
		afx_msg void OnSpinAngBase(NMHDR *pNotifyStruct,LRESULT *result);

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(svar_AN_ANGULAR,CDialog)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_AN_AUPREC,OnSpinAngPrec)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_AN_ANGBASE,OnSpinAngBase)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL svar_AN_ANGULAR::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_SETDIM);
	return(true);
}

void svar_AN_ANGULAR::OnCancel() {
	HWND hParentWnd;

	hParentWnd=::GetParent(m_hWnd);
	hParentWnd=::GetParent(hParentWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void svar_AN_ANGULAR::OnOK() {
	HWND hParentWnd;

	svar_iHitEnter=1;
	hParentWnd=::GetParent(m_hWnd);
	hParentWnd=::GetParent(hParentWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void svar_AN_ANGULAR::OnSpinAngPrec(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,AN_AUPREC),pNMUpDown->iDelta);
	*result=0;
}

void svar_AN_ANGULAR::OnSpinAngBase(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	resbuf rbDir;
	sds_getvar("ANGDIR",&rbDir);
	svar_SpinMaster(::GetDlgItem(m_hWnd,AN_ANGBASE), 
			(rbDir.resval.rint ? -pNMUpDown->iDelta : pNMUpDown->iDelta));
	*result=0;
}

BOOL svar_AN_ANGULAR::OnInitDialog() {
	svar_SubClassItems(this);
	return(CDialog::OnInitDialog());
}

//******This sub section will control the Linear sub dialog

class svar_AN_LINEAR : public CDialog {
	public:
		svar_AN_LINEAR():CDialog(AN_LINEAR) {};
		BOOL OnInitDialog();
	protected:
		afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
		virtual void OnOK();
		virtual void OnCancel();
		afx_msg void OnSpinLinPrec(NMHDR *pNotifyStruct,LRESULT *result);
				void FillCombo();

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(svar_AN_LINEAR,CDialog)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_AN_LUPREC,OnSpinLinPrec)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL svar_AN_LINEAR::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_SETDIM);
	return(true);
}

void svar_AN_LINEAR::OnCancel() {
	HWND hParentWnd;

	hParentWnd=::GetParent(m_hWnd);
	hParentWnd=::GetParent(hParentWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void svar_AN_LINEAR::OnOK() {
	HWND hParentWnd;

	svar_iHitEnter=1;
	hParentWnd=::GetParent(m_hWnd);
	hParentWnd=::GetParent(hParentWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void svar_AN_LINEAR::OnSpinLinPrec(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,AN_LUPREC),pNMUpDown->iDelta);
	*result=0;
}

BOOL svar_AN_LINEAR::OnInitDialog() {
	FillCombo();
	svar_SubClassItems(this);
	return(CDialog::OnInitDialog());
}

void svar_AN_LINEAR::FillCombo()
{
	CComboBox *pCombo = NULL;

	if (NULL == (pCombo = (CComboBox *) GetDlgItem(IDC_COMBO1)))
		return;

	pCombo->ResetContent();
	CString	comboOption;
	comboOption.LoadString(IDC_COMBO_PROJECTION_GROUP_1);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_PROJECTION_GROUP_2);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_PROJECTION_GROUP_3);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_PROJECTION_GROUP_4);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_PROJECTION_GROUP_5);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_PROJECTION_GROUP_6);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_PROJECTION_GROUP_7);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_PROJECTION_GROUP_8);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_PROJECTION_GROUP_9);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_PROJECTION_GROUP_10);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_PROJECTION_GROUP_11);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_PROJECTION_GROUP_12);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_PROJECTION_GROUP_13);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_PROJECTION_GROUP_14);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_PROJECTION_GROUP_15);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_PROJECTION_GROUP_16);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_PROJECTION_GROUP_17);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_PROJECTION_GROUP_18);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_PROJECTION_GROUP_19);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_PROJECTION_GROUP_20);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_PROJECTION_GROUP_21);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_PROJECTION_GROUP_22);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_PROJECTION_GROUP_23);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_PROJECTION_GROUP_24);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_PROJECTION_GROUP_25);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_PROJECTION_GROUP_26);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_PROJECTION_GROUP_27);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_PROJECTION_GROUP_28);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_PROJECTION_GROUP_29);
	pCombo->AddString(comboOption);
}

//******This sub section will control the Ucs sub dialog

class svar_AN_UCS : public CDialog {
	public:
		svar_AN_UCS();
		~svar_AN_UCS();
		BOOL OnInitDialog();
		int iUcsTest;
	protected:
		afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
		virtual void OnOK();
		virtual void OnCancel();

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(svar_AN_UCS,CDialog)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL svar_AN_UCS::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_SETDIM);
	return(true);
}

void svar_AN_UCS::OnCancel() {
	HWND hParentWnd;

	hParentWnd=::GetParent(m_hWnd);
	hParentWnd=::GetParent(hParentWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void svar_AN_UCS::OnOK() {
	HWND hParentWnd;

	svar_iHitEnter=1;
	hParentWnd=::GetParent(m_hWnd);
	hParentWnd=::GetParent(hParentWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

BOOL svar_AN_UCS::OnInitDialog() {
	svar_SubClassItems(this);
	return(CDialog::OnInitDialog());

}

svar_AN_UCS::svar_AN_UCS():CDialog(AN_UCS) {
	struct resbuf rb;
	SDS_getvar(NULL,DB_QUCSICON,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	iUcsTest=rb.resval.rint;
}

svar_AN_UCS::~svar_AN_UCS(){
/*	  struct resbuf rb;
	int tempvar;
	SDS_getvar(NULL,DB_QUCSICON,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if(rb.resval.rint!=iUcsTest){
		tempvar=rb.resval.rint; //save new value
		rb.resval.rint=iUcsTest;//restore orig value so we can paint out
		SDS_setvar(NULL,DB_QUCSICON,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
		if(SDS_CURDWG && SDS_CURVIEW) SDS_VarToTab(SDS_CURDWG,SDS_CURVIEW->m_pVportTabHip);
		SDS_DrawUcsIcon(NULL,NULL,NULL,1);	//paint out
		rb.resval.rint=tempvar; //restore correct value for repaint
		SDS_setvar(NULL,DB_QUCSICON,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
		if(SDS_CURDWG && SDS_CURVIEW){
			SDS_VarToTab(SDS_CURDWG,SDS_CURVIEW->m_pVportTabHip);
			if(rb.resval.rint&1)
				SDS_DrawUcsIcon(NULL,NULL,NULL,0);//paint in
		}

   }
*/}

//******This section will control the Display Units property page

class svar_AN_TAB : public CDialog {
	public:
		int 			m_AN_CATEGORIES_PRV,
						m_AN_CATEGORIES_CUR;
		svar_AN_ANGULAR m_AN_ANGULAR;
		svar_AN_LINEAR	m_AN_LINEAR;
		svar_AN_UCS 	m_AN_UCS;

		svar_AN_TAB() ;
		BOOL OnInitDialog();
	protected:
		afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
		virtual void OnOK();
		virtual void OnCancel();
		afx_msg void OnSelChange();
		virtual void DoDataExchange(CDataExchange *pDX);
		void		 FillCombo();

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(svar_AN_TAB,CDialog)
	ON_CBN_SELCHANGE(AN_CATEGORIES,OnSelChange)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL svar_AN_TAB::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_SETDIM);
	return(true);
}

void svar_AN_TAB::OnCancel() {
	HWND hParentWnd;

	hParentWnd=::GetParent(m_hWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void svar_AN_TAB::OnOK() {
	HWND hParentWnd;

	svar_iHitEnter=1;
	hParentWnd=::GetParent(m_hWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

BOOL svar_AN_TAB::OnInitDialog() {
	FillCombo();
	svar_SubClassItems(this);
	switch(svar_ChildDialog) {
		case AN_LINEAR:
			if (!m_AN_LINEAR) m_AN_LINEAR.Create(AN_LINEAR,this);
			m_AN_LINEAR.EnableWindow(TRUE);
			m_AN_LINEAR.ShowWindow(SW_SHOWNORMAL);
			m_AN_CATEGORIES_CUR=1;
			break;
		case AN_UCS:
			if (!m_AN_UCS) m_AN_UCS.Create(AN_UCS,this);
			m_AN_UCS.EnableWindow(TRUE);
			m_AN_UCS.ShowWindow(SW_SHOWNORMAL);
			m_AN_CATEGORIES_CUR=2;
			break;
		case AN_ANGULAR:
		default:
			if (!m_AN_ANGULAR) m_AN_ANGULAR.Create(AN_ANGULAR,this);
			m_AN_ANGULAR.EnableWindow(TRUE);
			m_AN_ANGULAR.ShowWindow(SW_SHOWNORMAL);
			m_AN_CATEGORIES_CUR=0;
			break;
	}
	return(CDialog::OnInitDialog());
}

void svar_AN_TAB::OnSelChange() {
	m_AN_CATEGORIES_PRV=m_AN_CATEGORIES_CUR;

	UpdateData(TRUE);
	if (m_AN_CATEGORIES_PRV==m_AN_CATEGORIES_CUR) return;

	// Disabler.
	switch(m_AN_CATEGORIES_PRV) {
		case 0:
			if (m_AN_ANGULAR) {
				m_AN_ANGULAR.EnableWindow(FALSE);
				m_AN_ANGULAR.ShowWindow(SW_HIDE);
			}
			break;
		case 1:
			if (m_AN_LINEAR) {
				m_AN_LINEAR.EnableWindow(FALSE);
				m_AN_LINEAR.ShowWindow(SW_HIDE);
			}
			break;
		case 2:
			if (m_AN_UCS) {
				m_AN_UCS.EnableWindow(FALSE);
				m_AN_UCS.ShowWindow(SW_HIDE);
			}
			break;
		default:
			m_AN_CATEGORIES_PRV=m_AN_CATEGORIES_CUR;
			break;
	}

	// Enabler/Creator.
	switch(m_AN_CATEGORIES_CUR) {
		case 0:
			if (!m_AN_ANGULAR) m_AN_ANGULAR.Create(AN_ANGULAR,this);
			m_AN_ANGULAR.EnableWindow(TRUE);
			m_AN_ANGULAR.ShowWindow(SW_SHOWNORMAL);
			svar_ChildDialog=AN_ANGULAR;
			break;
		case 1:
			if (!m_AN_LINEAR) m_AN_LINEAR.Create(AN_LINEAR,this);
			m_AN_LINEAR.EnableWindow(TRUE);
			m_AN_LINEAR.ShowWindow(SW_SHOWNORMAL);
			svar_ChildDialog=AN_LINEAR;
			break;
		case 2:
			if (!m_AN_UCS) m_AN_UCS.Create(AN_UCS,this);
			m_AN_UCS.EnableWindow(TRUE);
			m_AN_UCS.ShowWindow(SW_SHOWNORMAL);
			svar_ChildDialog=AN_UCS;
			break;
	}
}


svar_AN_TAB::svar_AN_TAB() : CDialog(AN_TAB) {
	m_AN_CATEGORIES_CUR=0;
	m_AN_CATEGORIES_PRV=0;
}

void svar_AN_TAB::DoDataExchange(CDataExchange *pDX) {
	DDX_CBIndex(pDX,AN_CATEGORIES,m_AN_CATEGORIES_CUR);
}

void svar_AN_TAB::FillCombo()
{
	CComboBox *pCombo = NULL;

	if (NULL == (pCombo = (CComboBox *) GetDlgItem(AN_CATEGORIES)))
		return;

	pCombo->ResetContent();
	CString	comboOption;
	comboOption.LoadString(IDC_COMBO_AN_CATEGORIES_1);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_AN_CATEGORIES_2);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_AN_CATEGORIES_3);
	pCombo->AddString(comboOption);
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//************************************************************************
// This section controls the Coordinate Input property page and its sub dialogs
//************************************************************************
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

//******This sub section will control the Grid sub dialog

class svar_CI_GRID : public CDialog {
	public:
		int iSnapMode,iGridMode,iGridTest,iIsoMode,iSnapTest;
		bool NeedRedraw;
		svar_CI_GRID();
		~svar_CI_GRID();
		BOOL OnInitDialog();
		double dGridTest[2];
	protected:
		afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
		virtual void OnOK();
		virtual void OnCancel();
		afx_msg void OnSynch();
		afx_msg void OnBN();
		afx_msg void OnSpinGrid1(NMHDR *pNotifyStruct,LRESULT *result);
		afx_msg void OnSpinGrid2(NMHDR *pNotifyStruct,LRESULT *result);
		afx_msg void OnSpinUnit1(NMHDR *pNotifyStruct,LRESULT *result);
		afx_msg void OnSpinUnit2(NMHDR *pNotifyStruct,LRESULT *result);
		afx_msg void OnSpinSnapBase1(NMHDR *pNotifyStruct,LRESULT *result);
		afx_msg void OnSpinSnapBase2(NMHDR *pNotifyStruct,LRESULT *result);
		afx_msg void OnSpinSnapAng(NMHDR *pNotifyStruct,LRESULT *result);
		virtual void DoDataExchange(CDataExchange *pDX);

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(svar_CI_GRID,CDialog)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_CI_GRIDUNIT1,OnSpinGrid1)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_CI_GRIDUNIT2,OnSpinGrid2)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_CI_SNAPUNIT1,OnSpinUnit1)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_CI_SNAPUNIT2,OnSpinUnit2)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_CI_SNAPBASE1,OnSpinSnapBase1)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_CI_SNAPBASE2,OnSpinSnapBase2)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_CI_SNAPANG,OnSpinSnapAng)
	ON_BN_CLICKED(CI_SYNCH,OnSynch)
	ON_BN_CLICKED(CI_SNAPMODE,OnBN)
	ON_BN_CLICKED(CI_GRIDMODE,OnBN)
	ON_BN_CLICKED(CI_SNAPSTYL,OnBN)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL svar_CI_GRID::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_SETTINGS);
	return(true);
}

void svar_CI_GRID::DoDataExchange(CDataExchange *pDX) {
	DDX_Check(pDX,CI_SNAPMODE,iSnapMode);
	DDX_Check(pDX,CI_GRIDMODE,iGridMode);
	DDX_Check(pDX,CI_SNAPSTYL,iIsoMode);
}

void svar_CI_GRID::OnCancel() {
	HWND hParentWnd;

	hParentWnd=::GetParent(m_hWnd);
	hParentWnd=::GetParent(hParentWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void svar_CI_GRID::OnOK() {
	HWND hParentWnd;

	svar_iHitEnter=1;
	hParentWnd=::GetParent(m_hWnd);
	hParentWnd=::GetParent(hParentWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void svar_CI_GRID::OnSynch() {
	struct resbuf rbSnap,rbGrid;

	SDS_getvar(NULL,DB_QSNAPUNIT,&rbSnap,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	SDS_getvar(NULL,DB_QGRIDUNIT,&rbGrid,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	rbGrid.resval.rpoint[0]=rbSnap.resval.rpoint[0];
	rbGrid.resval.rpoint[1]=rbSnap.resval.rpoint[1];
	sds_setvar("GRIDUNIT"/*DNT*/,&rbGrid);
	::SendMessage(::GetDlgItem(m_hWnd,CI_GRIDUNIT1),WM_PAINT,0,0);
	::SendMessage(::GetDlgItem(m_hWnd,CI_GRIDUNIT2),WM_PAINT,0,0);
}

void svar_CI_GRID::OnBN() {

	UpdateData(TRUE);
	if(iSnapMode || iGridMode) {
		::EnableWindow(::GetDlgItem(m_hWnd,CI_SNAPBASE1),TRUE);
		::EnableWindow(::GetDlgItem(m_hWnd,CI_SNAPBASE2),TRUE);
		::EnableWindow(::GetDlgItem(m_hWnd,CI_SNAPANG),TRUE);
		::EnableWindow(::GetDlgItem(m_hWnd,UPDN_CI_SNAPBASE1),TRUE);
		::EnableWindow(::GetDlgItem(m_hWnd,UPDN_CI_SNAPBASE2),TRUE);
		::EnableWindow(::GetDlgItem(m_hWnd,UPDN_CI_SNAPANG),TRUE);
	}else{
		::EnableWindow(::GetDlgItem(m_hWnd,CI_SNAPBASE1),FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd,CI_SNAPBASE2),FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd,CI_SNAPANG),FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd,UPDN_CI_SNAPBASE1),FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd,UPDN_CI_SNAPBASE2),FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd,UPDN_CI_SNAPANG),FALSE);
		struct resbuf rb;
		rb.restype=RTREAL;
		rb.resval.rreal=0.0;
		sds_setvar("SNAPANG"/*DNT*/,&rb);
	}
	if(iSnapMode && iGridMode) {
		::EnableWindow(::GetDlgItem(m_hWnd,CI_SYNCH),TRUE);
	}else{
		::EnableWindow(::GetDlgItem(m_hWnd,CI_SYNCH),FALSE);
	}
	if(iSnapMode){
		::EnableWindow(::GetDlgItem(m_hWnd,CI_SNAPUNIT1),TRUE);
		::EnableWindow(::GetDlgItem(m_hWnd,CI_SNAPUNIT2),TRUE);
		::EnableWindow(::GetDlgItem(m_hWnd,UPDN_CI_SNAPUNIT1),TRUE);
		::EnableWindow(::GetDlgItem(m_hWnd,UPDN_CI_SNAPUNIT2),TRUE);
	}else{
		::EnableWindow(::GetDlgItem(m_hWnd,CI_SNAPUNIT1),FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd,CI_SNAPUNIT2),FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd,UPDN_CI_SNAPUNIT1),FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd,UPDN_CI_SNAPUNIT2),FALSE);
	}
	if(iGridMode){
		::EnableWindow(::GetDlgItem(m_hWnd,CI_GRIDUNIT1),TRUE);
		::EnableWindow(::GetDlgItem(m_hWnd,CI_GRIDUNIT2),TRUE);
		::EnableWindow(::GetDlgItem(m_hWnd,UPDN_CI_GRIDUNIT1),TRUE);
		::EnableWindow(::GetDlgItem(m_hWnd,UPDN_CI_GRIDUNIT2),TRUE);
	}else{
		::EnableWindow(::GetDlgItem(m_hWnd,CI_GRIDUNIT1),FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd,CI_GRIDUNIT2),FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd,UPDN_CI_GRIDUNIT1),FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd,UPDN_CI_GRIDUNIT2),FALSE);
	}
	if(iIsoMode){
		::EnableWindow(::GetDlgItem(m_hWnd,CI_SNAPISOPAIR1),TRUE);
		::EnableWindow(::GetDlgItem(m_hWnd,CI_SNAPISOPAIR2),TRUE);
		::EnableWindow(::GetDlgItem(m_hWnd,CI_SNAPISOPAIR3),TRUE);
	}else{
		::EnableWindow(::GetDlgItem(m_hWnd,CI_SNAPISOPAIR1),FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd,CI_SNAPISOPAIR2),FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd,CI_SNAPISOPAIR3),FALSE);
	}
}

void svar_CI_GRID::OnSpinGrid1(NMHDR *pNotifyStruct,LRESULT *result) {			// Grid X Spacing
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
	struct resbuf rb;

	svar_SpinMaster(::GetDlgItem(m_hWnd,CI_GRIDUNIT1),pNMUpDown->iDelta);
	SDS_getvar(NULL,DB_QGRIDUNIT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	rb.resval.rpoint[1]=rb.resval.rpoint[0];
	sds_setvar("GRIDUNIT"/*DNT*/,&rb);
	::SendMessage(::GetDlgItem(m_hWnd,CI_GRIDUNIT2),WM_PAINT,0,0);
	*result=0;
}

void svar_CI_GRID::OnSpinGrid2(NMHDR *pNotifyStruct,LRESULT *result) {			// Grid Y Spacing
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,CI_GRIDUNIT2),pNMUpDown->iDelta);
	*result=0;
}

void svar_CI_GRID::OnSpinUnit1(NMHDR *pNotifyStruct,LRESULT *result) {			// Snap X Spacing
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;
	struct resbuf rb;

	svar_SpinMaster(::GetDlgItem(m_hWnd,CI_SNAPUNIT1),pNMUpDown->iDelta);
	SDS_getvar(NULL,DB_QSNAPUNIT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	rb.resval.rpoint[1]=rb.resval.rpoint[0];
	sds_setvar("SNAPUNIT"/*DNT*/,&rb);
	::SendMessage(::GetDlgItem(m_hWnd,CI_SNAPUNIT2),WM_PAINT,0,0);
	*result=0;
}

void svar_CI_GRID::OnSpinUnit2(NMHDR *pNotifyStruct,LRESULT *result) {			// Snap Y Spacing
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,CI_SNAPUNIT2),pNMUpDown->iDelta);
	*result=0;
}

void svar_CI_GRID::OnSpinSnapBase1(NMHDR *pNotifyStruct,LRESULT *result) {		// Grid X Origin
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,CI_SNAPBASE1),pNMUpDown->iDelta);
	*result=0;
}

void svar_CI_GRID::OnSpinSnapBase2(NMHDR *pNotifyStruct,LRESULT *result) {		// Grid Y Origin
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,CI_SNAPBASE2),pNMUpDown->iDelta);
	*result=0;
}

void svar_CI_GRID::OnSpinSnapAng(NMHDR *pNotifyStruct,LRESULT *result) {		// Grid Angle
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,CI_SNAPANG),pNMUpDown->iDelta);
	*result=0;
}

BOOL svar_CI_GRID::OnInitDialog() {
	UpdateData(FALSE);
	svar_CI_GRID::OnBN();

	svar_SubClassItems(this);
	CDialog::OnInitDialog();
	return(TRUE);
}

svar_CI_GRID::svar_CI_GRID():CDialog(CI_GRID){
	struct resbuf rb;

	SDS_getvar(NULL,DB_QSNAPSTYL,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	iIsoMode=rb.resval.rint;
	SDS_getvar(NULL,DB_QSNAPMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	iSnapMode=rb.resval.rint;
	SDS_getvar(NULL,DB_QGRIDUNIT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	dGridTest[0]=rb.resval.rpoint[0];
	dGridTest[1]=rb.resval.rpoint[1];
	SDS_getvar(NULL,DB_QGRIDMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	iGridMode=iGridTest=rb.resval.rint;
	SDS_getvar(NULL,DB_QSNAPANG,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	iSnapTest=rb.resval.rint;
	NeedRedraw=false;
}

svar_CI_GRID::~svar_CI_GRID(){
	struct resbuf rb;
	SDS_getvar(NULL,DB_QGRIDMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if(rb.resval.rint!=iGridTest) NeedRedraw=true;
	SDS_getvar(NULL,DB_QSNAPANG,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if(rb.resval.rint!=iSnapTest) NeedRedraw=true;
	SDS_getvar(NULL,DB_QGRIDUNIT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if(!icadRealEqual(rb.resval.rpoint[0],dGridTest[0])||!icadRealEqual(rb.resval.rpoint[1],dGridTest[1])) NeedRedraw=true;

	if(NeedRedraw) sds_redraw(NULL,IC_REDRAW_DONTCARE);
}

//******This sub section will control the Snap sub dialog

class svar_CI_SNAP : public CDialog {
	public:
		int iOsmode,iCheck;
		svar_CI_SNAP():CDialog(CI_SNAP) {};
		BOOL OnInitDialog();
	protected:
		afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
		virtual void OnOK();
		virtual void OnCancel();
		afx_msg void OnClicked();
		afx_msg void OnBNClear();
		afx_msg void OnBNOptions();
		afx_msg void OnBN();
		afx_msg void OnSel();
		virtual void DoDataExchange(CDataExchange *pDX);

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(svar_CI_SNAP,CDialog)
	ON_BN_CLICKED(CI_OSMODE0,OnSel)
	ON_BN_CLICKED(CI_OSMODE1,OnSel)
	ON_BN_CLICKED(CI_OSMODE2,OnSel)
	ON_BN_CLICKED(CI_OSMODE3,OnSel)
	ON_BN_CLICKED(CI_OSMODE4,OnSel)
	ON_BN_CLICKED(CI_OSMODE5,OnSel)
	ON_BN_CLICKED(CI_OSMODE6,OnSel)
	ON_BN_CLICKED(CI_OSMODE7,OnSel)
	ON_BN_CLICKED(CI_OSMODE8,OnSel)
	ON_BN_CLICKED(CI_OSMODE9,OnSel)
	ON_BN_CLICKED(CI_OSMODE11,OnSel)
	ON_BN_CLICKED(CI_OSMODE101,OnBN)
	ON_BN_CLICKED(CI_OSMODE102,OnBN)
	ON_BN_CLICKED(CI_OSMODE103,OnBN)
	ON_BN_CLICKED(CI_OSMODE10,OnClicked)
	ON_BN_CLICKED(CI_OSMODECLEAR,OnBNClear)
	ON_BN_CLICKED(CI_OSMODEOPTIONS,OnBNOptions)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL svar_CI_SNAP::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_SETTINGS);
	return(true);
}

void svar_CI_SNAP::DoDataExchange(CDataExchange *pDX) {
	DDX_Radio(pDX,CI_OSMODE102,iOsmode);
	DDX_Check(pDX,CI_OSMODE10,iCheck);
}

void svar_CI_SNAP::OnBNOptions()
{
	OptionsPropSheet dlg;
	dlg.setActivePage( OPTIONS_SNAPPING );
	dlg.DoModal();
}

void svar_CI_SNAP::OnBNClear() {
	HWND hWnd;
	struct resbuf rb;

	SDS_getvar(NULL,DB_QOSMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	rb.resval.rint=0;
	sds_setvar("OSMODE"/*DNT*/,&rb);
	hWnd=::GetDlgItem(m_hWnd,CI_OSMODE0);
	::SendMessage(hWnd,BM_SETCHECK,BST_UNCHECKED,0);
	hWnd=::GetDlgItem(m_hWnd,CI_OSMODE1);
	::SendMessage(hWnd,BM_SETCHECK,BST_UNCHECKED,0);
	hWnd=::GetDlgItem(m_hWnd,CI_OSMODE2);
	::SendMessage(hWnd,BM_SETCHECK,BST_UNCHECKED,0);
	hWnd=::GetDlgItem(m_hWnd,CI_OSMODE3);
	::SendMessage(hWnd,BM_SETCHECK,BST_UNCHECKED,0);
	hWnd=::GetDlgItem(m_hWnd,CI_OSMODE4);
	::SendMessage(hWnd,BM_SETCHECK,BST_UNCHECKED,0);
	hWnd=::GetDlgItem(m_hWnd,CI_OSMODE5);
	::SendMessage(hWnd,BM_SETCHECK,BST_UNCHECKED,0);
	hWnd=::GetDlgItem(m_hWnd,CI_OSMODE6);
	::SendMessage(hWnd,BM_SETCHECK,BST_UNCHECKED,0);
	hWnd=::GetDlgItem(m_hWnd,CI_OSMODE7);
	::SendMessage(hWnd,BM_SETCHECK,BST_UNCHECKED,0);
	hWnd=::GetDlgItem(m_hWnd,CI_OSMODE8);
	::SendMessage(hWnd,BM_SETCHECK,BST_UNCHECKED,0);
	hWnd=::GetDlgItem(m_hWnd,CI_OSMODE9);
	::SendMessage(hWnd,BM_SETCHECK,BST_UNCHECKED,0);
	hWnd=::GetDlgItem(m_hWnd,CI_OSMODE11);
	::SendMessage(hWnd,BM_SETCHECK,BST_UNCHECKED,0);
	::EnableWindow(::GetDlgItem(m_hWnd,CI_OSMODE11),FALSE);
	iCheck=0;
	UpdateData(false);
	OnClicked();
}

void svar_CI_SNAP::OnSel() {
	struct resbuf rb;

	SDS_getvar(NULL,DB_QOSMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
//4M Item:77->
/*
	if(rb.resval.rint==0 || rb.resval.rint==0x400) {
		rb.resval.rint&=(~0x400);
*/
	if(rb.resval.rint==0 || rb.resval.rint==IC_OSMODE_QUICK) {
		rb.resval.rint&=(~IC_OSMODE_QUICK);
//<-4M Item:77
		sds_setvar("OSMODE"/*DNT*/,&rb);
		::SendMessage(::GetDlgItem(m_hWnd,CI_OSMODE11),BM_SETCHECK,BST_UNCHECKED,0);
		::EnableWindow(::GetDlgItem(m_hWnd,CI_OSMODE11),FALSE);
	}else{
		::EnableWindow(::GetDlgItem(m_hWnd,CI_OSMODE11),TRUE);
	}
}

void svar_CI_SNAP::OnClicked() {
	struct resbuf rb;

	SDS_getvar(NULL,DB_QOSMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	UpdateData(TRUE);
	if(iCheck) {
		::EnableWindow(::GetDlgItem(m_hWnd,CI_OSMODE101),TRUE);
		::EnableWindow(::GetDlgItem(m_hWnd,CI_OSMODE102),TRUE);
		::EnableWindow(::GetDlgItem(m_hWnd,CI_OSMODE103),TRUE);
		rb.resval.rint&=0xE7DF;
		rb.resval.rint|=0x20;
		iOsmode=0;
		UpdateData(FALSE);
	}else{
		::EnableWindow(::GetDlgItem(m_hWnd,CI_OSMODE101),FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd,CI_OSMODE102),FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd,CI_OSMODE103),FALSE);
		rb.resval.rint&=0xE7DF;
	}
	sds_setvar("OSMODE"/*DNT*/,&rb);
}

void svar_CI_SNAP::OnCancel() {
	HWND hParentWnd;

	hParentWnd=::GetParent(m_hWnd);
	hParentWnd=::GetParent(hParentWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void svar_CI_SNAP::OnBN() {
	struct resbuf rb;

	UpdateData(TRUE);
	SDS_getvar(NULL,DB_QOSMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	rb.resval.rint&=0xE7DF;
	switch(iOsmode) {
		case 0:
			rb.resval.rint|=0x20;
			break;
		case 1:
			rb.resval.rint|=0x1000;
			break;
		case 2:
			rb.resval.rint|=0x800;
			break;
	}
	sds_setvar("OSMODE"/*DNT*/,&rb);

}

void svar_CI_SNAP::OnOK() {
	HWND hParentWnd;

	svar_iHitEnter=1;
	hParentWnd=::GetParent(m_hWnd);
	hParentWnd=::GetParent(hParentWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

BOOL svar_CI_SNAP::OnInitDialog() {
	struct resbuf rb;

	OnSel();

#ifndef BUILD_WITH_FLYOVERSNAP_SUPPORT
	::EnableWindow( ::GetDlgItem(m_hWnd,CI_OSMODEOPTIONS), FALSE );
#endif

	::EnableWindow(::GetDlgItem(m_hWnd,CI_OSMODE101),TRUE);
	::EnableWindow(::GetDlgItem(m_hWnd,CI_OSMODE102),TRUE);
	::EnableWindow(::GetDlgItem(m_hWnd,CI_OSMODE103),TRUE);

	SDS_getvar(NULL,DB_QOSMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if(rb.resval.rint&(0x20)) {
		iCheck=1;
		iOsmode=0;
		rb.resval.rint&=0xE7FF;
		sds_setvar("OSMODE"/*DNT*/,&rb);
	}else if(rb.resval.rint&(0x800)) {
		iCheck=1;
		iOsmode=2;
		rb.resval.rint&=0xEFDF;
		sds_setvar("OSMODE"/*DNT*/,&rb);
	}else if(rb.resval.rint&(0x1000)) {
		iCheck=1;
		iOsmode=1;
		rb.resval.rint&=0xF7DF;
		sds_setvar("OSMODE"/*DNT*/,&rb);
	}else{
		iCheck=0;
		::EnableWindow(::GetDlgItem(m_hWnd,CI_OSMODE101),FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd,CI_OSMODE102),FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd,CI_OSMODE103),FALSE);
	}

	svar_SubClassItems(this);

	CDialog::OnInitDialog();

	return(TRUE);
}

//******This sub section will control the Entity Selection sub dialog

class svar_CI_ENTSEL : public CDialog {
	public:
		svar_CI_ENTSEL():CDialog(CI_ENTSEL) {};
		BOOL OnInitDialog();
	protected:
		afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
		virtual void OnOK();
		virtual void OnCancel();
		afx_msg void OnSpinPBSize(NMHDR *pNotifyStruct,LRESULT *result);
		afx_msg void OnSpinAperture(NMHDR *pNotifyStruct,LRESULT *result);

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(svar_CI_ENTSEL,CDialog)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_CI_PBSIZE,OnSpinPBSize)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_CI_APERTURE,OnSpinAperture)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL svar_CI_ENTSEL::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_SETTINGS);
	return(true);
}

void svar_CI_ENTSEL::OnCancel() {
	HWND hParentWnd;

	hParentWnd=::GetParent(m_hWnd);
	hParentWnd=::GetParent(hParentWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void svar_CI_ENTSEL::OnOK() {
	HWND hParentWnd;

	svar_iHitEnter=1;
	hParentWnd=::GetParent(m_hWnd);
	hParentWnd=::GetParent(hParentWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void svar_CI_ENTSEL::OnSpinPBSize(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,CI_PBSIZE),pNMUpDown->iDelta);
	*result=0;
}

void svar_CI_ENTSEL::OnSpinAperture(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,CI_APERTURE),pNMUpDown->iDelta);
	*result=0;
}

BOOL svar_CI_ENTSEL::OnInitDialog() {
	svar_SubClassItems(this);
	CDialog::OnInitDialog();

	return(TRUE);
}

//******This section is for Limits
class svar_CI_LIMITS : public CDialog {
	public :
		BOOL b_CI_Pick;
		svar_CI_LIMITS();
		BOOL OnInitDialog();
	protected:
		afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
		virtual void OnOK();
		virtual void OnCancel();
		afx_msg void svar_CI_PICK();

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(svar_CI_LIMITS,CDialog)
	ON_BN_CLICKED(CI_LIMMAX3,svar_CI_PICK)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL svar_CI_LIMITS::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_SETTINGS);
	return(true);
}

void svar_CI_LIMITS::OnCancel() {
	HWND hParentWnd;

	hParentWnd=::GetParent(m_hWnd);
	hParentWnd=::GetParent(hParentWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void svar_CI_LIMITS::OnOK() {
	HWND hParentWnd;

	svar_iHitEnter=1;
	hParentWnd=::GetParent(m_hWnd);
	hParentWnd=::GetParent(hParentWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

BOOL svar_CI_LIMITS::OnInitDialog() {
	svar_SubClassItems(this);
	CDialog::OnInitDialog();
	return(TRUE);
}

svar_CI_LIMITS::svar_CI_LIMITS():CDialog(CI_LIMITS) {
	b_CI_Pick=0;
}

void svar_CI_LIMITS::svar_CI_PICK() {
	HWND hWnd;

	hWnd=::GetParent(m_hWnd);
	hWnd=::GetParent(hWnd);
	b_CI_Pick=1;
	::SendMessage(hWnd,WM_CLOSE,CI_LIMMAX3,0);
}

//******This section will control the Coordinate Input property page

class svar_CI_TAB : public CDialog {
	public:
		int 		m_CI_CATEGORIES_CUR,
					m_CI_CATEGORIES_PRV;
		svar_CI_GRID   m_CI_GRID;
		svar_CI_SNAP   m_CI_SNAP;
		svar_CI_ENTSEL m_CI_ENTSEL;
		svar_CI_LIMITS m_CI_LIMITS;

		svar_CI_TAB();
		BOOL OnInitDialog();

	protected:
		afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
		virtual void OnOK();
		virtual void OnCancel();
		afx_msg void OnSelChange();
		virtual void DoDataExchange(CDataExchange *pDX);
		void		 FillCombo();

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(svar_CI_TAB,CDialog)
	ON_CBN_SELCHANGE(CI_CATEGORIES,OnSelChange)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL svar_CI_TAB::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_SETTINGS);
	return(true);
}

void svar_CI_TAB::OnCancel() {
	HWND hParentWnd;

	hParentWnd=::GetParent(m_hWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void svar_CI_TAB::OnOK() {
	HWND hParentWnd;

	svar_iHitEnter=1;
	hParentWnd=::GetParent(m_hWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

BOOL svar_CI_TAB::OnInitDialog() {
	FillCombo();
	svar_SubClassItems(this);
	switch(svar_ChildDialog) {
		case CI_SNAP:
			if (!m_CI_SNAP) m_CI_SNAP.Create(CI_SNAP,this);
			m_CI_SNAP.EnableWindow(TRUE);
			m_CI_SNAP.ShowWindow(SW_SHOWNORMAL);
			m_CI_CATEGORIES_CUR=1;
			break;
		case CI_ENTSEL:
			if (!m_CI_ENTSEL) m_CI_ENTSEL.Create(CI_ENTSEL,this);
			m_CI_ENTSEL.EnableWindow(TRUE);
			m_CI_ENTSEL.ShowWindow(SW_SHOWNORMAL);
			m_CI_CATEGORIES_CUR=2;
			break;
		case CI_LIMITS:
			if (!m_CI_LIMITS) m_CI_LIMITS.Create(CI_LIMITS,this);
			m_CI_LIMITS.EnableWindow(TRUE);
			m_CI_LIMITS.ShowWindow(SW_SHOWNORMAL);
			m_CI_CATEGORIES_CUR=3;
			break;
		case CI_GRID:
		default:
			if (!m_CI_GRID) m_CI_GRID.Create(CI_GRID,this);
			m_CI_GRID.EnableWindow(TRUE);
			m_CI_GRID.ShowWindow(SW_SHOWNORMAL);
			m_CI_CATEGORIES_CUR=0;
			break;
	}
	return(CDialog::OnInitDialog());
}

void svar_CI_TAB::FillCombo()
{
	CComboBox*	pCombo;

	if (NULL == (pCombo = (CComboBox*)GetDlgItem(CI_CATEGORIES)))
		return;

	pCombo->ResetContent();
	CString	comboOption;
	comboOption.LoadString(IDC_COMBO_CI_CATEGORIES_1);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_CI_CATEGORIES_2);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_CI_CATEGORIES_3);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_CI_CATEGORIES_4);
	pCombo->AddString(comboOption);
}

void svar_CI_TAB::OnSelChange() {
	m_CI_CATEGORIES_PRV=m_CI_CATEGORIES_CUR;

	UpdateData(TRUE);;
	if (m_CI_CATEGORIES_PRV==m_CI_CATEGORIES_CUR) return;

	// Disabler.
	switch(m_CI_CATEGORIES_PRV) {
		case 0:
			if (m_CI_GRID) {
				m_CI_GRID.EnableWindow(FALSE);
				m_CI_GRID.ShowWindow(SW_HIDE);
			}
			break;
		case 1:
			if (m_CI_SNAP) {
				m_CI_SNAP.EnableWindow(FALSE);
				m_CI_SNAP.ShowWindow(SW_HIDE);
			}
			break;
		case 2:
			if (m_CI_ENTSEL) {
				m_CI_ENTSEL.EnableWindow(FALSE);
				m_CI_ENTSEL.ShowWindow(SW_HIDE);
			}
			break;
		case 3:
			if (m_CI_LIMITS) {
				m_CI_LIMITS.EnableWindow(FALSE);
				m_CI_LIMITS.ShowWindow(SW_HIDE);
			}
			break;
		default:
			m_CI_CATEGORIES_PRV=m_CI_CATEGORIES_CUR;
			break;
	}

	// Enabler/Creator.
	switch(m_CI_CATEGORIES_CUR) {
		case 0:
			if (!m_CI_GRID) m_CI_GRID.Create(CI_GRID,this);
			m_CI_GRID.EnableWindow(TRUE);
			m_CI_GRID.ShowWindow(SW_SHOWNORMAL);
			svar_ChildDialog=CI_GRID;
			break;
		case 1:
			if (!m_CI_SNAP) m_CI_SNAP.Create(CI_SNAP,this);
			m_CI_SNAP.EnableWindow(TRUE);
			m_CI_SNAP.ShowWindow(SW_SHOWNORMAL);
			svar_ChildDialog=CI_SNAP;
			break;
		case 2:
			if (!m_CI_ENTSEL) m_CI_ENTSEL.Create(CI_ENTSEL,this);
			m_CI_ENTSEL.EnableWindow(TRUE);
			m_CI_ENTSEL.ShowWindow(SW_SHOWNORMAL);
			svar_ChildDialog=CI_ENTSEL;
			break;
		case 3:
			if (!m_CI_LIMITS) m_CI_LIMITS.Create(CI_LIMITS,this);
			m_CI_LIMITS.EnableWindow(TRUE);
			m_CI_LIMITS.ShowWindow(SW_SHOWNORMAL);
			svar_ChildDialog=CI_LIMITS;
			break;
	}
}

void svar_CI_TAB::DoDataExchange(CDataExchange *pDX) {
	DDX_CBIndex(pDX,CI_CATEGORIES,m_CI_CATEGORIES_CUR);
}

svar_CI_TAB::svar_CI_TAB() : CDialog(CI_TAB) {
	m_CI_CATEGORIES_CUR=0;
	m_CI_CATEGORIES_PRV=0;
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//************************************************************************
// This section controls the Display property page and its sub dialogs
//************************************************************************
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

//******This sub section will control the Display sub dialog

class svar_DV_DISPLAY : public CDialog {
	public :
		svar_DV_DISPLAY():CDialog(DV_DISPLAY) {};
		BOOL OnInitDialog();
	protected:
		afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
		virtual void OnOK();
		virtual void OnCancel();

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(svar_DV_DISPLAY,CDialog)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL svar_DV_DISPLAY::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_SETTINGS);
	return(true);
}

void svar_DV_DISPLAY::OnCancel() {
	HWND hParentWnd;

	hParentWnd=::GetParent(m_hWnd);
	hParentWnd=::GetParent(hParentWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void svar_DV_DISPLAY::OnOK() {
	HWND hParentWnd;

	svar_iHitEnter=1;
	hParentWnd=::GetParent(m_hWnd);
	hParentWnd=::GetParent(hParentWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

BOOL svar_DV_DISPLAY::OnInitDialog() {
	svar_SubClassItems(this);
	CDialog::OnInitDialog();
	return(TRUE);
}

//******This sub section will control the ZOOM sub dialog

class svar_DV_ZOOM : public CDialog {
	public :
		svar_DV_ZOOM():CDialog(DV_ZOOM) { };
		BOOL OnInitDialog();
	protected:
		afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
		virtual void OnOK();
		virtual void OnCancel();

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(svar_DV_ZOOM,CDialog)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL svar_DV_ZOOM::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_SETTINGS);
	return(true);
}

void svar_DV_ZOOM::OnCancel() {
	HWND hParentWnd;

	hParentWnd=::GetParent(m_hWnd);
	hParentWnd=::GetParent(hParentWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void svar_DV_ZOOM::OnOK() {
	HWND hParentWnd;

	svar_iHitEnter=1;
	hParentWnd=::GetParent(m_hWnd);
	hParentWnd=::GetParent(hParentWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

BOOL svar_DV_ZOOM::OnInitDialog() {
	svar_SubClassItems(this);
	CDialog::OnInitDialog();
	return(TRUE);
}

//******This sub section will control the Nodes sub dialog

class svar_DV_NODES : public CDialog {
	public :
		int iDV_ColorHot,iGrips,iGripColor,iGripSize;
		svar_DV_NODES();
		BOOL OnInitDialog();
	protected:
		afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
		afx_msg void OnBN();
		virtual void OnOK();
		virtual void OnCancel();
		void DV_SetColor();
		void DV_SetColorHot();
		afx_msg void OnSpinGripSize(NMHDR *pNotifyStruct,LRESULT *result);
		virtual void DoDataExchange(CDataExchange *pDX);

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(svar_DV_NODES,CDialog)
	ON_COMMAND(DV_GRIPCOLOR,DV_SetColor)
	ON_COMMAND(DV_GRIPHOT,DV_SetColorHot)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_DV_GRIPSIZE,OnSpinGripSize)
	ON_WM_HELPINFO()
	ON_BN_CLICKED(DV_GRIPS,OnBN)
END_MESSAGE_MAP()

void svar_DV_NODES::OnBN(){
	UpdateData(true);
}

void svar_DV_NODES::DoDataExchange(CDataExchange *pDX) {
	DDX_Check(pDX,DV_GRIPS,iGrips);
}

BOOL svar_DV_NODES::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_SETTINGS);
	return(true);
}

void svar_DV_NODES::OnCancel() {
	HWND hParentWnd;

	hParentWnd=::GetParent(m_hWnd);
	hParentWnd=::GetParent(hParentWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void svar_DV_NODES::OnOK() {
	HWND hParentWnd;

	svar_iHitEnter=1;
	hParentWnd=::GetParent(m_hWnd);
	hParentWnd=::GetParent(hParentWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void svar_DV_NODES::OnSpinGripSize(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,DV_GRIPSIZE),pNMUpDown->iDelta);
	*result=0;
}

BOOL svar_DV_NODES::OnInitDialog() {
	svar_SubClassItems(this);
	CDialog::OnInitDialog();
	return(TRUE);
}

//***This will call the color dialog box
//	 and set the color variable
svar_DV_NODES:: svar_DV_NODES():CDialog(DV_NODES) {
	struct resbuf rb;

	SDS_getvar(NULL,DB_QGRIPSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	iGripSize=rb.resval.rint;
	SDS_getvar(NULL,DB_QGRIPCOLOR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	iGripColor=rb.resval.rint;
	SDS_getvar(NULL,DB_QGRIPS,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	iGrips=rb.resval.rint;
}

//******This section is for Limits
class svar_DV_LIMITS : public CDialog {
	public :
		BOOL b_DV_Pick;
		svar_DV_LIMITS();
		BOOL OnInitDialog();
	protected:
		afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
		virtual void OnOK();
		virtual void OnCancel();
		afx_msg void svar_DV_PICK();

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(svar_DV_LIMITS,CDialog)
	ON_BN_CLICKED(DV_LIMMAX3,svar_DV_PICK)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL svar_DV_LIMITS::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_SETTINGS);
	return(true);
}

void svar_DV_LIMITS::OnCancel() {
	HWND hParentWnd;

	hParentWnd=::GetParent(m_hWnd);
	hParentWnd=::GetParent(hParentWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void svar_DV_LIMITS::OnOK() {
	HWND hParentWnd;

	svar_iHitEnter=1;
	hParentWnd=::GetParent(m_hWnd);
	hParentWnd=::GetParent(hParentWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

BOOL svar_DV_LIMITS::OnInitDialog() {
	svar_SubClassItems(this);
	CDialog::OnInitDialog();
	return(TRUE);
}

svar_DV_LIMITS::svar_DV_LIMITS():CDialog(DV_LIMITS) {
	b_DV_Pick=0;
}

void svar_DV_LIMITS::svar_DV_PICK() {
	HWND hWnd;

	hWnd=::GetParent(m_hWnd);
	hWnd=::GetParent(hWnd);
	b_DV_Pick=1;
	::SendMessage(hWnd,WM_CLOSE,DV_LIMMAX3,0);
}


//******This section is for LWeight
class svar_DV_LWEIGHT : public CDialog {
	public:
	
		svar_DV_LWEIGHT();
		CStatic	m_CurrentLWeightControl;
		CIcadNewLWComboBox	m_DefLWeightComboBox;
		CIcadLWListBox	m_LWeightListBox;
		CSliderCtrl	m_DisplayScaleSlider;
		int		m_DisplayScaleFactor;
		BOOL	m_bDisplayLWeights;
		int m_CurrentLineWeight;
		int m_DefaultLWeight;
		bool m_bDisplayValuesInMM;
		void SetTextForCurrentLWeight();
		virtual BOOL OnInitDialog();

	protected:
		int m_SupportedLineWeightList[24];
		afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
		virtual void OnOK();
		virtual void OnCancel();
		afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
		afx_msg void OnRadioMillemeters();
		afx_msg void OnRadioInches();
		afx_msg void OnSelchangeLweightList();
		afx_msg void OnSelchangeDeflweightCombo();
		afx_msg void OnDisplayCheck();
		virtual void DoDataExchange(CDataExchange *pDX);

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(svar_DV_LWEIGHT, CDialog)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_CHECK_DISPLAY_LWEIGHT, OnDisplayCheck)
	ON_BN_CLICKED(IDC_RADIO_MILLEMETERS, OnRadioMillemeters)
	ON_BN_CLICKED(IDC_RADIO_INCHES, OnRadioInches)
	ON_LBN_SELCHANGE(IDC_LWEIGHT_LIST, OnSelchangeLweightList)
	ON_CBN_SELCHANGE(IDC_DEFLWEIGHT_COMBO, OnSelchangeDeflweightCombo)
END_MESSAGE_MAP()

BOOL svar_DV_LWEIGHT::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_SETTINGS);
	return(true);
}

void svar_DV_LWEIGHT::OnCancel() {
	HWND hParentWnd;

	hParentWnd=::GetParent(m_hWnd);
	hParentWnd=::GetParent(hParentWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void svar_DV_LWEIGHT::OnOK() {
	HWND hParentWnd;

	svar_iHitEnter=1;
	hParentWnd=::GetParent(m_hWnd);
	hParentWnd=::GetParent(hParentWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

svar_DV_LWEIGHT::svar_DV_LWEIGHT():CDialog(DV_LWEIGHT) {
	// Initialise the Supported Lineweight Array.

	m_SupportedLineWeightList[0] = 0; 
	m_SupportedLineWeightList[1] = 5; 
	m_SupportedLineWeightList[2] = 9;
	m_SupportedLineWeightList[3] = 13;
	m_SupportedLineWeightList[4] = 15;
	m_SupportedLineWeightList[5] = 18;
	m_SupportedLineWeightList[6] = 20;
	m_SupportedLineWeightList[7] = 25;
	m_SupportedLineWeightList[8] = 30;
	m_SupportedLineWeightList[9] = 35;
	m_SupportedLineWeightList[10] = 40;
	m_SupportedLineWeightList[11] = 50;
	m_SupportedLineWeightList[12] = 53;
	m_SupportedLineWeightList[13] = 60;
	m_SupportedLineWeightList[14] = 70;
	m_SupportedLineWeightList[15] = 80;
	m_SupportedLineWeightList[16] = 90;
	m_SupportedLineWeightList[17] = 100;
	m_SupportedLineWeightList[18] = 106;
	m_SupportedLineWeightList[19] = 120;
	m_SupportedLineWeightList[20] = 140;
	m_SupportedLineWeightList[21] = 158;
	m_SupportedLineWeightList[22] = 200;
	m_SupportedLineWeightList[23] = 211;
	
	// Set Default values
	m_DisplayScaleFactor = 8;
	m_bDisplayLWeights = FALSE;

	m_CurrentLineWeight = -1;
	m_bDisplayValuesInMM = true;
	m_DefaultLWeight = 158;

	// Display Lineweight (must do this before OnInitDialog)
	struct resbuf rb;
	if (SDS_getvar(NULL,DB_QLWDISPLAY,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES) == RTNORM)
		m_bDisplayLWeights = (rb.resval.rint == 1) ? TRUE : FALSE;
}

void svar_DV_LWEIGHT::DoDataExchange(CDataExchange* pDX)
{
//	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_CURRENT_LINEWEIGHT, m_CurrentLWeightControl);
	DDX_Control(pDX, IDC_DEFLWEIGHT_COMBO, m_DefLWeightComboBox);
	DDX_Control(pDX, IDC_LWEIGHT_LIST, m_LWeightListBox);
	DDX_Control(pDX, IDC_DISPLAYSCALE_SLIDER, m_DisplayScaleSlider);
	DDX_Slider(pDX, IDC_DISPLAYSCALE_SLIDER, m_DisplayScaleFactor);
	DDX_Check(pDX, IDC_CHECK_DISPLAY_LWEIGHT, m_bDisplayLWeights);
}


/////////////////////////////////////////////////////////////////////////////
// svar_DV_LWEIGHT message handlers

BOOL svar_DV_LWEIGHT::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	struct resbuf rb;

	// Get current values

	// Current Lineweight
	if (SDS_getvar(NULL,DB_QCELWEIGHT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES) == RTNORM)
		m_CurrentLineWeight = rb.resval.rint;

	// Lineweight Default
	if (SDS_getvar(NULL,DB_QLWDEFAULT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES) == RTNORM)
		m_DefaultLWeight = rb.resval.rint;

	// Lineweight Display Scale
	CIcadApp* pIcadApp = (CIcadApp*)AfxGetApp();
	if (pIcadApp != NULL)
		m_DisplayScaleFactor = pIcadApp->m_nLWDisplayScaleFactor;

	// Lineweight Units
	if (SDS_getvar(NULL,DB_QLWUNITS,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES) == RTNORM)
		m_bDisplayValuesInMM = (rb.resval.rint == 1) ? true : false;


	CWnd* pWndRadioButton = NULL;
	if (m_bDisplayValuesInMM)
		pWndRadioButton = GetDlgItem(IDC_RADIO_MILLEMETERS);
	else
		pWndRadioButton = GetDlgItem(IDC_RADIO_INCHES);
	
	CButton radioButtonForMMorIN;
	radioButtonForMMorIN.Attach(pWndRadioButton->m_hWnd);
	radioButtonForMMorIN.SetCheck(BST_CHECKED);
	radioButtonForMMorIN.Detach();
	
	m_DisplayScaleSlider.SetRange(2, 20, TRUE);
	m_DisplayScaleSlider.SetPageSize(2);
	m_DisplayScaleSlider.SetLineSize(1);
	m_DisplayScaleSlider.SetTicFreq(2);
	m_DisplayScaleSlider.SetPos(m_DisplayScaleFactor);


	m_LWeightListBox.AddLineWeight(-1);				  // ByLayer
	m_LWeightListBox.AddLineWeight(-2);				  // ByBlock
	m_LWeightListBox.AddLineWeight(m_DefaultLWeight); // Default.
	
	int iIndex; // Index in m_SupportedLineWeightList Array.
	int nComboBoxIndex = 0;
	for (iIndex = 0; iIndex < 24; iIndex++)
	{
		m_LWeightListBox.AddLineWeight(m_SupportedLineWeightList[iIndex]);
		m_DefLWeightComboBox.AddLineWeight(m_SupportedLineWeightList[iIndex]);
		
		if (m_SupportedLineWeightList[iIndex] == m_DefaultLWeight)
			nComboBoxIndex = iIndex;
	}

	m_LWeightListBox.m_nDisplayScaleFactor = m_DisplayScaleFactor;
	m_LWeightListBox.m_bDisplayValuesInMM = m_bDisplayValuesInMM;
	m_DefLWeightComboBox.m_nDisplayScaleFactor = m_DisplayScaleFactor;
	m_DefLWeightComboBox.m_bDisplayValuesInMM = m_bDisplayValuesInMM;

	m_DefLWeightComboBox.SetCurSel(nComboBoxIndex);

	// This will change to the current selected Lineweight.
	int negativeLWstart = -1;
	int selectedIndex = 0;
	bool bSelectionDone = false;
	for (iIndex = 0; iIndex < 3; iIndex++, negativeLWstart--)
		if (negativeLWstart == m_CurrentLineWeight)
		{
			selectedIndex = iIndex;
			bSelectionDone = true;
			break;
		}

	if (!bSelectionDone)
	{
		for (iIndex = 0; iIndex < 24; iIndex++)
			if (m_CurrentLineWeight == m_SupportedLineWeightList[iIndex])
			{
				selectedIndex = iIndex + 3;
				break;
			}
	}
		
	m_LWeightListBox.SetCurSel(selectedIndex);

	SetTextForCurrentLWeight();
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void svar_DV_LWEIGHT::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	
	int nID = pScrollBar->GetDlgCtrlID();
	if (nID == IDC_DISPLAYSCALE_SLIDER)
	{
		// Update dialog
		int nPos = m_DisplayScaleSlider.GetPos();
		m_LWeightListBox.m_nDisplayScaleFactor = nPos;
		m_DefLWeightComboBox.m_nDisplayScaleFactor = nPos;
		m_LWeightListBox.RedrawWindow();
		m_DefLWeightComboBox.RedrawWindow();

		// Set Lineweight Display Scale
		CIcadApp* pIcadApp = (CIcadApp*)AfxGetApp();
		if (pIcadApp != NULL)
			pIcadApp->m_nLWDisplayScaleFactor = nPos;
	}
	else
	{
		CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	}	
}

void svar_DV_LWEIGHT::OnRadioMillemeters() 
{
	// TODO: Add your control notification handler code here
	m_bDisplayValuesInMM = true;
	m_LWeightListBox.m_bDisplayValuesInMM = true;
	m_LWeightListBox.RedrawWindow();
	m_DefLWeightComboBox.m_bDisplayValuesInMM = true;
	m_DefLWeightComboBox.RedrawWindow();

	struct resbuf rb;
	rb.restype=RTSHORT;
	rb.resval.rint = (m_bDisplayValuesInMM) ? 1 : 0;
	sds_setvar("LWUNITS", &rb);

	SetTextForCurrentLWeight();
}

void svar_DV_LWEIGHT::OnRadioInches() 
{
	// TODO: Add your control notification handler code here
	m_bDisplayValuesInMM = false;
	m_LWeightListBox.m_bDisplayValuesInMM = false;
	m_LWeightListBox.RedrawWindow();
	m_DefLWeightComboBox.m_bDisplayValuesInMM = false;
	m_DefLWeightComboBox.RedrawWindow();

	struct resbuf rb;
	rb.restype=RTSHORT;
	rb.resval.rint = (m_bDisplayValuesInMM) ? 1 : 0;
	sds_setvar("LWUNITS", &rb);

	SetTextForCurrentLWeight();
}

void svar_DV_LWEIGHT::OnDisplayCheck() 
{
	m_bDisplayLWeights = !m_bDisplayLWeights;
	struct resbuf rb;
	rb.restype=RTSHORT;
	rb.resval.rint = (m_bDisplayLWeights) ? 1 : 0;
	sds_setvar("LWDISPLAY", &rb);
}

void svar_DV_LWEIGHT::SetTextForCurrentLWeight()
{
	CString strDisplay;

	if (m_CurrentLineWeight < 0)
	{
		switch (m_CurrentLineWeight)
		{
			case -1:
				strDisplay = "BYLAYER";
				break;

			case -2:
				strDisplay = "BYBLOCK";
				break;

			case -3:
				strDisplay = "Default";					
				break;
		}
	}
	else
	{
		if (m_bDisplayValuesInMM)
			strDisplay.Format("%.2f mm", (m_CurrentLineWeight * 1.0) / 100.0);
		else
			strDisplay.Format("%.3f\"", (m_CurrentLineWeight * 1.0) / 2540.0);
	}

	m_CurrentLWeightControl.SetWindowText(strDisplay);
}

void svar_DV_LWEIGHT::OnSelchangeLweightList() 
{
	// TODO: Add your control notification handler code here

	// Get the selected index and assign the corresponding weight.
	int iSelectedListIndex = m_LWeightListBox.GetCurSel();

	if (iSelectedListIndex < 3)
		m_CurrentLineWeight = -1 * (iSelectedListIndex + 1);
	else
		m_CurrentLineWeight = m_SupportedLineWeightList[iSelectedListIndex - 3];

	struct resbuf rb;
	rb.restype=RTSHORT;
	rb.resval.rint = m_CurrentLineWeight;
	sds_setvar("CELWEIGHT", &rb);

	SetTextForCurrentLWeight();
}

void svar_DV_LWEIGHT::OnSelchangeDeflweightCombo() 
{
	// TODO: Add your control notification handler code here

	m_DefaultLWeight = m_DefLWeightComboBox.GetCurrentDefaultLWeight();

	struct resbuf rb;
	rb.restype=RTSHORT;
	rb.resval.rint = m_DefaultLWeight;
	sds_setvar("LWDEFAULT", &rb);

	m_LWeightListBox.SetCurrentDefLWeight(m_DefaultLWeight);
	m_LWeightListBox.RedrawWindow();
}


//******This sub section will control the Programint sub dialog
class svar_DV_PROGRAMINT : public CDialog {
	public :
		svar_DV_PROGRAMINT():CDialog(DV_PROGRAMINT) {};
		BOOL OnInitDialog();
	protected:
		afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
		virtual void OnOK();
		virtual void OnCancel();

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(svar_DV_PROGRAMINT,CDialog)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL svar_DV_PROGRAMINT::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_SETTINGS);
	return(true);
}

void svar_DV_PROGRAMINT::OnCancel() {
	HWND hParentWnd;

	hParentWnd=::GetParent(m_hWnd);
	hParentWnd=::GetParent(hParentWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void svar_DV_PROGRAMINT::OnOK() {
	HWND hParentWnd;

	svar_iHitEnter=1;
	hParentWnd=::GetParent(m_hWnd);
	hParentWnd=::GetParent(hParentWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

BOOL svar_DV_PROGRAMINT::OnInitDialog() {
	svar_SubClassItems(this);
	CDialog::OnInitDialog();
	return(TRUE);
}

//******This section will control the Display Propertypage

class svar_DV_TAB : public CDialog {
	public:
		int 			   m_DV_CATEGORIES_CUR,
						   m_DV_CATEGORIES_PRV;
		svar_DV_DISPLAY    m_DV_DISPLAY;
		svar_DV_ZOOM	   m_DV_ZOOM;
		svar_DV_NODES	   m_DV_NODES;
		svar_DV_PROGRAMINT m_DV_PROGRAMINT;
		svar_DV_LIMITS	   m_DV_LIMITS;
		svar_DV_LWEIGHT	   m_DV_LWEIGHT;

		svar_DV_TAB() ;
		BOOL OnInitDialog();

	protected:
		afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
		virtual void OnOK();
		virtual void OnCancel();
		afx_msg void OnSelChange();
		virtual void DoDataExchange(CDataExchange *pDX);
		void		 FillCombo();

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(svar_DV_TAB,CDialog)
	ON_CBN_SELCHANGE(DV_CATEGORIES,OnSelChange)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL svar_DV_TAB::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_SETTINGS);
	return(true);
}

void svar_DV_TAB::OnCancel() {
	HWND hParentWnd;

	hParentWnd=::GetParent(m_hWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void svar_DV_TAB::OnOK() {
	HWND hParentWnd;

	svar_iHitEnter=1;
	hParentWnd=::GetParent(m_hWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

BOOL svar_DV_TAB::OnInitDialog() {
	FillCombo();
	svar_SubClassItems(this);
	switch(svar_ChildDialog) {
		case DV_LIMITS:
			if (!m_DV_LIMITS) m_DV_LIMITS.Create(DV_LIMITS,this);
			m_DV_LIMITS.EnableWindow(TRUE);
			m_DV_LIMITS.ShowWindow(SW_SHOWNORMAL);
			m_DV_CATEGORIES_CUR=1;
			break;
		case DV_LWEIGHT:
			if (!m_DV_LWEIGHT) m_DV_LWEIGHT.Create(DV_LWEIGHT,this);
			m_DV_LWEIGHT.EnableWindow(TRUE);
			m_DV_LWEIGHT.ShowWindow(SW_SHOWNORMAL);
			m_DV_CATEGORIES_CUR=2;
			break;
		case DV_NODES:
			if (!m_DV_NODES) m_DV_NODES.Create(DV_NODES,this);
			m_DV_NODES.EnableWindow(TRUE);
			m_DV_NODES.ShowWindow(SW_SHOWNORMAL);
			m_DV_CATEGORIES_CUR=3;
			break;
		case DV_PROGRAMINT:
			if (!m_DV_PROGRAMINT) m_DV_PROGRAMINT.Create(DV_PROGRAMINT,this);
			m_DV_PROGRAMINT.EnableWindow(TRUE);
			m_DV_PROGRAMINT.ShowWindow(SW_SHOWNORMAL);
			m_DV_CATEGORIES_CUR=4;
			break;
		case DV_ZOOM:
			if (!m_DV_ZOOM) m_DV_ZOOM.Create(DV_ZOOM,this);
			m_DV_ZOOM.EnableWindow(TRUE);
			m_DV_ZOOM.ShowWindow(SW_SHOWNORMAL);
			m_DV_CATEGORIES_CUR=5;
			break;
		case DV_DISPLAY:
		default:
			if (!m_DV_DISPLAY) m_DV_DISPLAY.Create(DV_DISPLAY,this);
			m_DV_DISPLAY.EnableWindow(TRUE);
			m_DV_DISPLAY.ShowWindow(SW_SHOWNORMAL);
			m_DV_CATEGORIES_CUR=0;
			break;
	}
	return(CDialog::OnInitDialog());
}

void svar_DV_TAB::FillCombo()
{
	CComboBox *pCombo = NULL;

	if (NULL == (pCombo = (CComboBox *) GetDlgItem(DV_CATEGORIES)))
		return;

	pCombo->ResetContent();
	CString	comboOption;
	comboOption.LoadString(IDC_COMBO_DV_CATEGORIES_1);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_DV_CATEGORIES_2);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_DV_CATEGORIES_3);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_DV_CATEGORIES_4);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_DV_CATEGORIES_5);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_DV_CATEGORIES_6);
	pCombo->AddString(comboOption);
}

void svar_DV_TAB::OnSelChange() {
	m_DV_CATEGORIES_PRV=m_DV_CATEGORIES_CUR;

	UpdateData(TRUE);;
	if (m_DV_CATEGORIES_PRV==m_DV_CATEGORIES_CUR) return;

	// Disabler.
	switch(m_DV_CATEGORIES_PRV) {
		case 0:
			if (m_DV_DISPLAY) {
				m_DV_DISPLAY.EnableWindow(FALSE);
				m_DV_DISPLAY.ShowWindow(SW_HIDE);
			}
			break;
		case 1:
			if (m_DV_LIMITS) {
				m_DV_LIMITS.EnableWindow(FALSE);
				m_DV_LIMITS.ShowWindow(SW_HIDE);
			}
			break;
		case 2:
			if (m_DV_LWEIGHT) {
				m_DV_LWEIGHT.EnableWindow(FALSE);
				m_DV_LWEIGHT.ShowWindow(SW_HIDE);
			}
			break;
		case 3:
			if (m_DV_NODES) {
				m_DV_NODES.EnableWindow(FALSE);
				m_DV_NODES.ShowWindow(SW_HIDE);
			}
			break;
		case 4:
			if (m_DV_PROGRAMINT) {
				m_DV_PROGRAMINT.EnableWindow(FALSE);
				m_DV_PROGRAMINT.ShowWindow(SW_HIDE);
			}
			break;
		case 5:
			if (m_DV_ZOOM) {
				m_DV_ZOOM.EnableWindow(FALSE);
				m_DV_ZOOM.ShowWindow(SW_HIDE);
			}
			break;
		default:
			m_DV_CATEGORIES_PRV=m_DV_CATEGORIES_CUR;
			break;
	}

	// Enabler/Creator.
	switch(m_DV_CATEGORIES_CUR) {
		case 0:
			if (!m_DV_DISPLAY) m_DV_DISPLAY.Create(DV_DISPLAY,this);
			m_DV_DISPLAY.EnableWindow(TRUE);
			m_DV_DISPLAY.ShowWindow(SW_SHOWNORMAL);
			svar_ChildDialog=DV_DISPLAY;
			break;
		case 1:
			if (!m_DV_LIMITS) m_DV_LIMITS.Create(DV_LIMITS,this);
			m_DV_LIMITS.EnableWindow(TRUE);
			m_DV_LIMITS.ShowWindow(SW_SHOWNORMAL);
			svar_ChildDialog=DV_LIMITS;
			break;
		case 2:
			if (!m_DV_LWEIGHT) m_DV_LWEIGHT.Create(DV_LWEIGHT,this);
			m_DV_LWEIGHT.EnableWindow(TRUE);
			m_DV_LWEIGHT.ShowWindow(SW_SHOWNORMAL);
			svar_ChildDialog=DV_LWEIGHT;
			break;
		case 3:
			if (!m_DV_NODES) m_DV_NODES.Create(DV_NODES,this);
			m_DV_NODES.EnableWindow(TRUE);
			m_DV_NODES.ShowWindow(SW_SHOWNORMAL);
			svar_ChildDialog=DV_NODES;
			break;
		case 4:
			if (!m_DV_PROGRAMINT) m_DV_PROGRAMINT.Create(DV_PROGRAMINT,this);
			m_DV_PROGRAMINT.EnableWindow(TRUE);
			m_DV_PROGRAMINT.ShowWindow(SW_SHOWNORMAL);
			svar_ChildDialog=DV_PROGRAMINT;
			break;
		case 5:
			if (!m_DV_ZOOM) m_DV_ZOOM.Create(DV_ZOOM,this);
			m_DV_ZOOM.EnableWindow(TRUE);
			m_DV_ZOOM.ShowWindow(SW_SHOWNORMAL);
			svar_ChildDialog=DV_ZOOM;
			break;
	}
	return;
}

void svar_DV_TAB::DoDataExchange(CDataExchange *pDX) {
	DDX_CBIndex(pDX,DV_CATEGORIES,m_DV_CATEGORIES_CUR);
}

svar_DV_TAB::svar_DV_TAB() : CDialog(DV_TAB) {
	m_DV_CATEGORIES_CUR=0;
	m_DV_CATEGORIES_PRV=0;
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//************************************************************************
// This section controls the Display property page and its sub dialogs
//************************************************************************
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

//******This sub section will control the Attribute sub dialog

class svar_EC_ATTRIBUTE : public CDialog {
	public:
		int iAttReq;
		svar_EC_ATTRIBUTE():CDialog(EC_ATTRIBUTE) {};
		BOOL OnInitDialog();
	protected:
		afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
		virtual void OnOK();
		virtual void OnCancel();
		afx_msg void OnAttReq();
		virtual void DoDataExchange(CDataExchange *pDX);

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(svar_EC_ATTRIBUTE,CDialog)
	ON_BN_CLICKED(EC_ATTREQ1,OnAttReq)
	ON_BN_CLICKED(EC_ATTREQ2,OnAttReq)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL svar_EC_ATTRIBUTE::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_SETTINGS);
	return(true);
}

void svar_EC_ATTRIBUTE::DoDataExchange(CDataExchange *pDX) {
	DDX_Radio(pDX,EC_ATTREQ1,iAttReq);
}

void svar_EC_ATTRIBUTE::OnAttReq() {

	UpdateData(TRUE);
	if(iAttReq) {
		::EnableWindow(::GetDlgItem(m_hWnd,EC_ATTDIA1),TRUE);
		::EnableWindow(::GetDlgItem(m_hWnd,EC_ATTDIA2),TRUE);
	}else{
		::EnableWindow(::GetDlgItem(m_hWnd,EC_ATTDIA1),FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd,EC_ATTDIA2),FALSE);
	}
}

void svar_EC_ATTRIBUTE::OnCancel() {
	HWND hParentWnd;

	hParentWnd=::GetParent(m_hWnd);
	hParentWnd=::GetParent(hParentWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void svar_EC_ATTRIBUTE::OnOK() {
	HWND hParentWnd;

	svar_iHitEnter=1;
	hParentWnd=::GetParent(m_hWnd);
	hParentWnd=::GetParent(hParentWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

BOOL svar_EC_ATTRIBUTE::OnInitDialog() {
	struct resbuf rb;

	SDS_getvar(NULL,DB_QATTREQ,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	iAttReq=rb.resval.rint;
	UpdateData(FALSE);
	OnAttReq();
	svar_SubClassItems(this);
	CDialog::OnInitDialog();
	return(TRUE);
}

//******This sub section will control the Donut sub dialog

class svar_EC_DONUT : public CDialog {
	public:
		svar_EC_DONUT():CDialog(EC_DONUT) {};
		BOOL OnInitDialog();
	protected:
		afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
		virtual void OnOK();
		virtual void OnCancel();
		afx_msg void OnSpinOD(NMHDR *pNotifyStruct,LRESULT *result);
		afx_msg void OnSpinID(NMHDR *pNotifyStruct,LRESULT *result);

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(svar_EC_DONUT,CDialog)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_EC_DONUTOD,OnSpinOD)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_EC_DONUTID,OnSpinID)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL svar_EC_DONUT::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_SETTINGS);
	return(true);
}

void svar_EC_DONUT::OnCancel() {
	HWND hParentWnd;

	hParentWnd=::GetParent(m_hWnd);
	hParentWnd=::GetParent(hParentWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void svar_EC_DONUT::OnOK() {
	HWND hParentWnd;

	svar_iHitEnter=1;
	hParentWnd=::GetParent(m_hWnd);
	hParentWnd=::GetParent(hParentWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void svar_EC_DONUT::OnSpinOD(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,EC_DONUTOD),pNMUpDown->iDelta);
	*result=0;
}

void svar_EC_DONUT::OnSpinID(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,EC_DONUTID),pNMUpDown->iDelta);
	*result=0;
}

BOOL svar_EC_DONUT::OnInitDialog() {
	svar_SubClassItems(this);
	CDialog::OnInitDialog();
	return(TRUE);
}

//******This sub section will control the Ucs sub dialog

class svar_EC_ELLIPSE : public CDialog {
	public:
		svar_EC_ELLIPSE():CDialog(EC_ELLIPSE) {};
		BOOL OnInitDialog();
	protected:
		afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
		virtual void OnOK();
		virtual void OnCancel();

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(svar_EC_ELLIPSE,CDialog)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL svar_EC_ELLIPSE::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_SETTINGS);
	return(true);
}

void svar_EC_ELLIPSE::OnCancel() {
	HWND hParentWnd;

	hParentWnd=::GetParent(m_hWnd);
	hParentWnd=::GetParent(hParentWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void svar_EC_ELLIPSE::OnOK() {
	HWND hParentWnd;

	svar_iHitEnter=1;
	hParentWnd=::GetParent(m_hWnd);
	hParentWnd=::GetParent(hParentWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

BOOL svar_EC_ELLIPSE::OnInitDialog() {
	svar_SubClassItems(this);
	return(CDialog::OnInitDialog());

}

//******This sub section will control the Hatch sub dialog

class svar_EC_HATCH : public CDialog {
	public:
		char szHPName[IC_ACADBUF];
		svar_EC_HATCH();
		BOOL OnInitDialog();
		void svar_FillHPList();
	protected:
		afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
		virtual void OnOK();
		virtual void OnCancel();
		afx_msg void OnDestroy();
		virtual void DoDataExchange(CDataExchange* pDX);
		afx_msg void OnSpinAng(NMHDR *pNotifyStruct,LRESULT *result);
		afx_msg void OnSpinSpace(NMHDR *pNotifyStruct,LRESULT *result);
		afx_msg void OnSpinScale(NMHDR *pNotifyStruct,LRESULT *result);

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(svar_EC_HATCH,CDialog)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_EC_HPANG,OnSpinAng)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_EC_HPSPACE,OnSpinSpace)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_EC_HPSCALE,OnSpinScale)
	ON_WM_DESTROY()
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL svar_EC_HATCH::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_SETTINGS);
	return(true);
}

void svar_EC_HATCH::OnCancel() {
	HWND hParentWnd;

	hParentWnd=::GetParent(m_hWnd);
	hParentWnd=::GetParent(hParentWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void svar_EC_HATCH::OnOK() {
	HWND hParentWnd;

	svar_iHitEnter=1;
	hParentWnd=::GetParent(m_hWnd);
	hParentWnd=::GetParent(hParentWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void svar_EC_HATCH::OnSpinAng(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,EC_HPANG),pNMUpDown->iDelta);
	*result=0;
}

void svar_EC_HATCH::OnSpinSpace(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,EC_HPSPACE),pNMUpDown->iDelta);
	*result=0;
}

void svar_EC_HATCH::OnSpinScale(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,EC_HPSCALE),pNMUpDown->iDelta);
	*result=0;
}

void svar_EC_HATCH::OnDestroy() {
	UpdateData(TRUE);
	CDialog::OnDestroy();
}

void svar_EC_HATCH::DoDataExchange(CDataExchange *pDX) {
	struct resbuf rb;
	int index;

	if(!pDX->m_bSaveAndValidate) { //To dialog
		SDS_getvar(NULL,DB_QHPNAME,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		strcpy(szHPName,rb.resval.rstring);
		if(rb.resval.rstring){ IC_FREE(rb.resval.rstring);rb.resval.rstring=NULL;}		

		if(strlen(szHPName)<1) strcpy(szHPName,ResourceString(IDC_ICADSETVARSDIA__NONE__109, "<None>" ));
		index=::SendMessage(::GetDlgItem(m_hWnd,EC_HPNAME),CB_FINDSTRINGEXACT,(WPARAM)-1,(LPARAM)szHPName);
		DDX_CBIndex(pDX,EC_HPNAME,index);
	}else{ //From dialog
		DDX_CBIndex(pDX,EC_HPNAME,index);
		::SendMessage(::GetDlgItem(m_hWnd,EC_HPNAME),CB_GETLBTEXT,(WPARAM)index,(LPARAM)szHPName);
		rb.restype=RTSTR;
		if(strisame(ResourceString(IDC_ICADSETVARSDIA__NONE__109, "<None>" ),szHPName)){			
			rb.resval.rstring= new char;
			strcpy(rb.resval.rstring,""/*DNT*/);
		}else{			
			rb.resval.rstring= new char [strlen(szHPName)+1];
			strcpy(rb.resval.rstring,szHPName);
		}
		sds_setvar("HPNAME"/*DNT*/,&rb);
		IC_FREE(rb.resval.rstring);  rb.resval.rstring=NULL;		
	}
}

BOOL svar_EC_HATCH::OnInitDialog() {
	svar_SubClassItems(this);
	CDialog::OnInitDialog();
	svar_FillHPList();
	UpdateData(false);
	return(TRUE);
}

void svar_EC_HATCH::svar_FillHPList() {
	HWND hItemWnd;
	char fs1[IC_ACADBUF],patnm[31],patstr[IC_ACADBUF],*fcp1;
	int ct;
	FILE *fptr=NULL;

	if(sds_findfile(SDS_PATTERNFILE,fs1)!=RTNORM) goto bail;
	if((fptr=fopen(fs1,"rt"/*DNT*/))==NULL) goto bail;

	strcpy(patstr,"*"/*DNT*/);
	for(ct=0; ic_nfgets(fs1,sizeof(fs1)-1,fptr)!=0; ct++) {
		if(*fs1!='*'/*DNT*/) continue;
		if((fcp1=strchr(fs1,','/*DNT*/))==NULL) continue;
		*fcp1=0;
		strncpy(patnm,fs1+1,sizeof(patnm)-1);
		hItemWnd=::GetDlgItem(m_hWnd,EC_HPNAME);
		::SendMessage(hItemWnd,CB_ADDSTRING,0,(LPARAM)patnm);
	}
	::SendMessage(hItemWnd,CB_ADDSTRING,0,(LPARAM)ResourceString(IDC_ICADSETVARSDIA__NONE__113, "<NONE>" ));

	bail:
	if(fptr!=NULL) fclose(fptr);
}

svar_EC_HATCH::svar_EC_HATCH():CDialog(EC_HATCH) {
}

//******This sub section will control the Insert sub dialog

class svar_EC_INSERT : public CDialog {
	public:
		BOOL b_EC_Pick;
		svar_EC_INSERT();
		BOOL OnInitDialog();
	protected:
		afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
		virtual void OnOK();
		virtual void OnCancel();
		afx_msg void svar_EC_PICK();
		afx_msg void OnSpinBase1(NMHDR *pNotifyStruct,LRESULT *result);
		afx_msg void OnSpinBase2(NMHDR *pNotifyStruct,LRESULT *result);
		afx_msg void OnSpinBase3(NMHDR *pNotifyStruct,LRESULT *result);

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(svar_EC_INSERT,CDialog)
	ON_BN_CLICKED(EC_INSBASE,svar_EC_PICK)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_EC_INSBASE1,OnSpinBase1)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_EC_INSBASE2,OnSpinBase2)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_EC_INSBASE3,OnSpinBase3)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL svar_EC_INSERT::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_SETTINGS);
	return(true);
}

void svar_EC_INSERT::OnSpinBase1(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,EC_INSBASE1),pNMUpDown->iDelta);
	*result=0;
}

void svar_EC_INSERT::OnSpinBase2(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,EC_INSBASE2),pNMUpDown->iDelta);
	*result=0;
}

void svar_EC_INSERT::OnSpinBase3(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,EC_INSBASE3),pNMUpDown->iDelta);
	*result=0;
}

void svar_EC_INSERT::OnCancel() {
	HWND hParentWnd;

	hParentWnd=::GetParent(m_hWnd);
	hParentWnd=::GetParent(hParentWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void svar_EC_INSERT::OnOK() {
	HWND hParentWnd;

	svar_iHitEnter=1;
	hParentWnd=::GetParent(m_hWnd);
	hParentWnd=::GetParent(hParentWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

BOOL svar_EC_INSERT::OnInitDialog() {
	svar_SubClassItems(this);
	CDialog::OnInitDialog();
	return(TRUE);
}

svar_EC_INSERT::svar_EC_INSERT():CDialog(EC_INSERT) {
	b_EC_Pick=0;
}

void svar_EC_INSERT::svar_EC_PICK() {
	HWND hWnd;

	hWnd=::GetParent(m_hWnd);
	hWnd=::GetParent(hWnd);
	b_EC_Pick=1;
	::SendMessage(hWnd,WM_CLOSE,EC_INSBASE,0);
}

//******This sub section will control the Plines sub dialog

class svar_EC_PLINES : public CDialog {
	public:
		svar_EC_PLINES():CDialog(EC_PLINES) {};
		BOOL OnInitDialog();
	protected:
		afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
		virtual void OnOK();
		virtual void OnCancel();
		afx_msg void OnSpinPlineWid(NMHDR *pNotifyStruct,LRESULT *result);
		afx_msg void OnSpinTraceWid(NMHDR *pNotifyStruct,LRESULT *result);

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(svar_EC_PLINES,CDialog)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_EC_PLINEWID,OnSpinPlineWid)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_EC_TRACEWID,OnSpinTraceWid)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL svar_EC_PLINES::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_SETTINGS);
	return(true);
}

void svar_EC_PLINES::OnCancel() {
	HWND hParentWnd;

	hParentWnd=::GetParent(m_hWnd);
	hParentWnd=::GetParent(hParentWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void svar_EC_PLINES::OnOK() {
	HWND hParentWnd;

	svar_iHitEnter=1;
	hParentWnd=::GetParent(m_hWnd);
	hParentWnd=::GetParent(hParentWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void svar_EC_PLINES::OnSpinPlineWid(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,EC_PLINEWID),pNMUpDown->iDelta);
	*result=0;
}

void svar_EC_PLINES::OnSpinTraceWid(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,EC_TRACEWID),pNMUpDown->iDelta);
	*result=0;
}

BOOL svar_EC_PLINES::OnInitDialog() {
	svar_SubClassItems(this);
	CDialog::OnInitDialog();
	return(TRUE);
}

//******This sub section will control the Points sub dialog

class svar_EC_POINTS : public CDialog {
	public:
		svar_EC_POINTS():CDialog(EC_POINTS) {};
		BOOL OnInitDialog();
		int i;
	protected:
		afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
		virtual void OnOK();
		virtual void OnCancel();
		afx_msg void OnClick();
		afx_msg void OnClickFive();
		afx_msg void OnClickAbb();
		afx_msg void OnClickPer();
		afx_msg void OnSpinPDSize(NMHDR *pNotifyStruct,LRESULT *result);

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(svar_EC_POINTS,CDialog)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_EC_PDSIZE,OnSpinPDSize)
	ON_BN_CLICKED(EC_PDM0  ,OnClick)
	ON_BN_CLICKED(EC_PDM1  ,OnClick)
	ON_BN_CLICKED(EC_PDM2  ,OnClick)
	ON_BN_CLICKED(EC_PDM3  ,OnClick)
	ON_BN_CLICKED(EC_PDM4  ,OnClick)
	ON_BN_CLICKED(EC_PDM32 ,OnClick)
	ON_BN_CLICKED(EC_PDM33 ,OnClick)
	ON_BN_CLICKED(EC_PDM34 ,OnClick)
	ON_BN_CLICKED(EC_PDM35 ,OnClick)
	ON_BN_CLICKED(EC_PDM36 ,OnClick)
	ON_BN_CLICKED(EC_PDM64 ,OnClick)
	ON_BN_CLICKED(EC_PDM65 ,OnClick)
	ON_BN_CLICKED(EC_PDM66 ,OnClick)
	ON_BN_CLICKED(EC_PDM67 ,OnClick)
	ON_BN_CLICKED(EC_PDM68 ,OnClick)
	ON_BN_CLICKED(EC_PDM96 ,OnClick)
	ON_BN_CLICKED(EC_PDM97 ,OnClick)
	ON_BN_CLICKED(EC_PDM98 ,OnClick)
	ON_BN_CLICKED(EC_PDM99 ,OnClick)
	ON_BN_CLICKED(EC_PDM100,OnClick)
	ON_BN_CLICKED(EC_FIVE,OnClickFive)
	ON_BN_CLICKED(EC_PERCENT,OnClickPer)
	ON_BN_CLICKED(EC_ABSOLUTE,OnClickAbb)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL svar_EC_POINTS::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_SETTINGS);
	return(true);
}

void svar_EC_POINTS::OnClickFive(){
	char szTmp[IC_ACADBUF];
	struct resbuf rb;

	rb.restype=RTREAL;
	rb.resval.rreal=0.0;
	sds_setvar("PDSIZE"/*DNT*/,&rb);
	sds_rtos(0.0,-1,-1,szTmp);
	::SetWindowText(::GetDlgItem(m_hWnd,EC_PDSIZE),szTmp);
}

void svar_EC_POINTS::OnClickAbb(){
	char szTmp[IC_ACADBUF];
	struct resbuf rb;

	rb.restype=RTREAL;
	rb.resval.rreal=1.0;
	sds_setvar("PDSIZE"/*DNT*/,&rb);
	sds_rtos(1.0,-1,-1,szTmp);
	::SetWindowText(::GetDlgItem(m_hWnd,EC_PDSIZE),szTmp);
}

void svar_EC_POINTS::OnClickPer(){
	char szTmp[IC_ACADBUF];
	struct resbuf rb;

	rb.restype=RTREAL;
	rb.resval.rreal=-1.0;
	sds_setvar("PDSIZE"/*DNT*/,&rb);
	sds_rtos(-1.0,-1,-1,szTmp);
	::SetWindowText(::GetDlgItem(m_hWnd,EC_PDSIZE),szTmp);
}

void svar_EC_POINTS::OnClick(){
	struct resbuf rb;

	SDS_getvar(NULL,DB_QPDMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if(rb.resval.rint<=1){
		::EnableWindow(::GetDlgItem(m_hWnd,EC_PDSIZE),false);
		::EnableWindow(::GetDlgItem(m_hWnd,UPDN_EC_PDSIZE),false);
		::EnableWindow(::GetDlgItem(m_hWnd,EC_FIVE),false);
		::EnableWindow(::GetDlgItem(m_hWnd,EC_ABSOLUTE),false);
		::EnableWindow(::GetDlgItem(m_hWnd,EC_PERCENT),false);
	}else{
		::EnableWindow(::GetDlgItem(m_hWnd,EC_PDSIZE),true);
		::EnableWindow(::GetDlgItem(m_hWnd,UPDN_EC_PDSIZE),true);
		::EnableWindow(::GetDlgItem(m_hWnd,EC_FIVE),true);
		::EnableWindow(::GetDlgItem(m_hWnd,EC_ABSOLUTE),true);
		::EnableWindow(::GetDlgItem(m_hWnd,EC_PERCENT),true);
	}
	SDS_getvar(NULL,DB_QPDSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if(rb.resval.rreal>0.0){
		::SendMessage(::GetDlgItem(m_hWnd,EC_FIVE),BM_SETCHECK,BST_UNCHECKED,0);
		::SendMessage(::GetDlgItem(m_hWnd,EC_ABSOLUTE),BM_SETCHECK,BST_CHECKED,0);
		::SendMessage(::GetDlgItem(m_hWnd,EC_PERCENT),BM_SETCHECK,BST_UNCHECKED,0);
	}else if(rb.resval.rreal<0.0){
		::SendMessage(::GetDlgItem(m_hWnd,EC_FIVE),BM_SETCHECK,BST_UNCHECKED,0);
		::SendMessage(::GetDlgItem(m_hWnd,EC_ABSOLUTE),BM_SETCHECK,BST_UNCHECKED,0);
		::SendMessage(::GetDlgItem(m_hWnd,EC_PERCENT),BM_SETCHECK,BST_CHECKED,0);
	}else{
		::SendMessage(::GetDlgItem(m_hWnd,EC_FIVE),BM_SETCHECK,BST_CHECKED,0);
		::SendMessage(::GetDlgItem(m_hWnd,EC_ABSOLUTE),BM_SETCHECK,BST_UNCHECKED,0);
		::SendMessage(::GetDlgItem(m_hWnd,EC_PERCENT),BM_SETCHECK,BST_UNCHECKED,0);
	}
}

void svar_EC_POINTS::OnCancel() {
	HWND hParentWnd;

	hParentWnd=::GetParent(m_hWnd);
	hParentWnd=::GetParent(hParentWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void svar_EC_POINTS::OnOK() {
	HWND hParentWnd;

	svar_iHitEnter=1;
	hParentWnd=::GetParent(m_hWnd);
	hParentWnd=::GetParent(hParentWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void svar_EC_POINTS::OnSpinPDSize(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,EC_PDSIZE),pNMUpDown->iDelta);
	*result=0;

	struct resbuf rb;
	SDS_getvar(NULL,DB_QPDSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if(rb.resval.rreal>0.0){
		::SendMessage(::GetDlgItem(m_hWnd,EC_FIVE),BM_SETCHECK,BST_UNCHECKED,0);
		::SendMessage(::GetDlgItem(m_hWnd,EC_ABSOLUTE),BM_SETCHECK,BST_CHECKED,0);
		::SendMessage(::GetDlgItem(m_hWnd,EC_PERCENT),BM_SETCHECK,BST_UNCHECKED,0);
	}else if(rb.resval.rreal<0.0){
		::SendMessage(::GetDlgItem(m_hWnd,EC_FIVE),BM_SETCHECK,BST_UNCHECKED,0);
		::SendMessage(::GetDlgItem(m_hWnd,EC_ABSOLUTE),BM_SETCHECK,BST_UNCHECKED,0);
		::SendMessage(::GetDlgItem(m_hWnd,EC_PERCENT),BM_SETCHECK,BST_CHECKED,0);
	}else{
		::SendMessage(::GetDlgItem(m_hWnd,EC_FIVE),BM_SETCHECK,BST_CHECKED,0);
		::SendMessage(::GetDlgItem(m_hWnd,EC_ABSOLUTE),BM_SETCHECK,BST_UNCHECKED,0);
		::SendMessage(::GetDlgItem(m_hWnd,EC_PERCENT),BM_SETCHECK,BST_UNCHECKED,0);
	}
}

BOOL svar_EC_POINTS::OnInitDialog() {
	svar_SubClassItems(this);
	CDialog::OnInitDialog();
	OnClick();
	i=69;
	return(TRUE);
}

//******This sub section will control the Sketch sub dialog

class svar_EC_SHAPE : public CDialog {
	public:
		svar_EC_SHAPE():CDialog(EC_SHAPE) {};
		BOOL OnInitDialog();
	protected:
		afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
		virtual void OnOK();
		virtual void OnCancel();

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(svar_EC_SHAPE,CDialog)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL svar_EC_SHAPE::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_SETTINGS);
	return(true);
}

void svar_EC_SHAPE::OnCancel() {
	HWND hParentWnd;

	hParentWnd=::GetParent(m_hWnd);
	hParentWnd=::GetParent(hParentWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void svar_EC_SHAPE::OnOK() {
	HWND hParentWnd;

	svar_iHitEnter=1;
	hParentWnd=::GetParent(m_hWnd);
	hParentWnd=::GetParent(hParentWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

BOOL svar_EC_SHAPE::OnInitDialog() {
	svar_SubClassItems(this);
	CDialog::OnInitDialog();
	return(TRUE);
}

//******This sub section will control the Sketch sub dialog

class svar_EC_SKETCH : public CDialog {
	public:
		svar_EC_SKETCH():CDialog(EC_SKETCH) {};
		BOOL OnInitDialog();
	protected:
		afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
		virtual void OnOK();
		virtual void OnCancel();
		afx_msg void OnSpinSketchInc(NMHDR *pNotifyStruct,LRESULT *result);

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(svar_EC_SKETCH,CDialog)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_EC_SKETCHINC,OnSpinSketchInc)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL svar_EC_SKETCH::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_SETTINGS);
	return(true);
}

void svar_EC_SKETCH::OnCancel() {
	HWND hParentWnd;

	hParentWnd=::GetParent(m_hWnd);
	hParentWnd=::GetParent(hParentWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void svar_EC_SKETCH::OnOK() {
	HWND hParentWnd;

	svar_iHitEnter=1;
	hParentWnd=::GetParent(m_hWnd);
	hParentWnd=::GetParent(hParentWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void svar_EC_SKETCH::OnSpinSketchInc(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,EC_SKETCHINC),pNMUpDown->iDelta);
	*result=0;
}

BOOL svar_EC_SKETCH::OnInitDialog() {
	svar_SubClassItems(this);
	CDialog::OnInitDialog();
	return(TRUE);
}

//******This sub section will control the Text sub dialog

class svar_EC_TEXT : public CDialog {
	public:
		svar_EC_TEXT():CDialog(EC_TEXT) {};
		BOOL OnInitDialog();
		CComboBox m_CTextStyle;
	protected:
		afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
		afx_msg void ReadStyles();
		afx_msg void OnExp();
		virtual void OnOK();
		virtual void OnCancel();
		afx_msg void OnSpinTextSize(NMHDR *pNotifyStruct,LRESULT *result);
		virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(svar_EC_TEXT,CDialog)
	ON_BN_CLICKED(EC_EXPLORE,OnExp)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_EC_TEXTSIZE,OnSpinTextSize)
	ON_COMMAND(ICAD_WNDACTION_EXP_RETURN,ReadStyles)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL svar_EC_TEXT::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_SETTINGS);
	return(true);
}

void svar_EC_TEXT::OnExp() {
	SDS_CMainWindow->m_ExpReturnHwnd=m_hWnd;
	SDS_CMainWindow->IcadExplorer(2);
}

void svar_EC_TEXT::DoDataExchange(CDataExchange *pDX) {
	DDX_Control(pDX,EC_TEXTSTYLE,m_CTextStyle);
}

void svar_EC_TEXT::OnCancel() {
	HWND hParentWnd;

	hParentWnd=::GetParent(m_hWnd);
	hParentWnd=::GetParent(hParentWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);

	extern HWND SDS_hWndExplorerCaller;
	SDS_hWndExplorerCaller = NULL;
}

void svar_EC_TEXT::OnOK() {
	HWND hParentWnd;

	svar_iHitEnter=1;
	hParentWnd=::GetParent(m_hWnd);
	hParentWnd=::GetParent(hParentWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);

	extern HWND SDS_hWndExplorerCaller;
	SDS_hWndExplorerCaller = NULL;
}

void svar_EC_TEXT::OnSpinTextSize(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,EC_TEXTSIZE),pNMUpDown->iDelta);
	*result=0;
}

void svar_EC_TEXT::ReadStyles() {
	CString csTmp;

	m_CTextStyle.ResetContent();
	SDS_StartTblRead("STYLE"/*DNT*/);
	while(SDS_TblReadHasMore()) {
  		csTmp=SDS_TblReadString();
		//Bugzilla No. 78070 ; 17-06-2002
		//if(csTmp.Find('|'/*DNT*/)>0) continue;
		if(csTmp.Find(CMD_XREF_TBLDIVIDER)>0) continue;
		m_CTextStyle.AddString(csTmp);
	}
}

BOOL svar_EC_TEXT::OnInitDialog() {
	extern HWND SDS_hWndExplorerCaller;
	SDS_hWndExplorerCaller = m_hWnd;

	svar_SubClassItems(this);
	CDialog::OnInitDialog();
	ReadStyles();
	return(TRUE);
}

//******This section will control the Entity Creation Propertypage

//This is the main property sheet
class svar_EC_TAB : public CDialog {
	public:
		int 			  m_EC_CATEGORIES_CUR,
						  m_EC_CATEGORIES_PRV;
		svar_EC_ATTRIBUTE m_EC_ATTRIBUTE;
		svar_EC_DONUT	  m_EC_DONUT;
		svar_EC_ELLIPSE   m_EC_ELLIPSE;
		svar_EC_HATCH	  m_EC_HATCH;
		svar_EC_INSERT	  m_EC_INSERT;
		svar_EC_PLINES	  m_EC_PLINES;
		svar_EC_POINTS	  m_EC_POINTS;
		svar_EC_SHAPE	  m_EC_SHAPE;
		svar_EC_SKETCH	  m_EC_SKETCH;
		svar_EC_TEXT	  m_EC_TEXT;
		CComboBox  m_CLayer,m_CLtype,m_CLweight;

		svar_EC_TAB() ;
		void OnSelColor();

		BOOL OnInitDialog();
	protected:
		afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
		virtual void OnOK();
		virtual void OnCancel();
		afx_msg void OnSelChange();
		afx_msg void OnLayerChange();
		afx_msg void OnLtypeChange();
        afx_msg void OnLweightChange();
		afx_msg void svar_FillLayer();
		afx_msg void svar_FillLineType();
		afx_msg void svar_FillLineweight();
		afx_msg void OnSpinCeScale(NMHDR *pNotifyStruct,LRESULT *result);
		afx_msg void OnSpinLtScale(NMHDR *pNotifyStruct,LRESULT *result);
		virtual void DoDataExchange(CDataExchange *pDX);
		void		 FillCombo();

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(svar_EC_TAB,CDialog)
	ON_CBN_SELCHANGE(EC_CLAYER,OnLayerChange)
	ON_CBN_SELCHANGE(EC_CELTYPE,OnLtypeChange)
	ON_CBN_SELCHANGE(EC_CELWEIGHT,OnLweightChange)
	ON_CBN_SELCHANGE(EC_CATEGORIES,OnSelChange)
	ON_COMMAND(EC_SELCOLOR,OnSelColor)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_EC_CELTSCALE,OnSpinCeScale)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_EC_LTSCALE,OnSpinLtScale)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL svar_EC_TAB::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_SETTINGS);
	return(true);
}

void svar_EC_TAB::OnCancel() {
	HWND hParentWnd;

	hParentWnd=::GetParent(m_hWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void svar_EC_TAB::OnOK() {
	HWND hParentWnd;

	svar_iHitEnter=1;
	hParentWnd=::GetParent(m_hWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void svar_EC_TAB::OnSpinCeScale(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,EC_CELTSCALE),pNMUpDown->iDelta);
	*result=0;
}

void svar_EC_TAB::OnSpinLtScale(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,EC_LTSCALE),pNMUpDown->iDelta);
	*result=0;
}

BOOL svar_EC_TAB::OnInitDialog() {
	struct resbuf rb;

	FillCombo();
	svar_SubClassItems(this);
	switch(svar_ChildDialog) {
		case EC_DONUT:
			if (!m_EC_DONUT) m_EC_DONUT.Create(EC_DONUT,this);
			m_EC_DONUT.EnableWindow(TRUE);
			m_EC_DONUT.ShowWindow(SW_SHOWNORMAL);
			m_EC_CATEGORIES_CUR=1;
			break;
		case EC_ELLIPSE:
			if (!m_EC_ELLIPSE) m_EC_ELLIPSE.Create(EC_ELLIPSE,this);
			m_EC_ELLIPSE.EnableWindow(TRUE);
			m_EC_ELLIPSE.ShowWindow(SW_SHOWNORMAL);
			m_EC_CATEGORIES_CUR=2;
			break;
		case EC_SKETCH:
			if (!m_EC_SKETCH) m_EC_SKETCH.Create(EC_SKETCH,this);
			m_EC_SKETCH.EnableWindow(TRUE);
			m_EC_SKETCH.ShowWindow(SW_SHOWNORMAL);
			m_EC_CATEGORIES_CUR=3;
			break;
		case EC_HATCH:
			if (!m_EC_HATCH) m_EC_HATCH.Create(EC_HATCH,this);
			m_EC_HATCH.EnableWindow(TRUE);
			m_EC_HATCH.ShowWindow(SW_SHOWNORMAL);
			m_EC_CATEGORIES_CUR=4;
			break;
		case EC_INSERT:
			if (!m_EC_INSERT) m_EC_INSERT.Create(EC_INSERT,this);
			m_EC_INSERT.EnableWindow(TRUE);
			m_EC_INSERT.ShowWindow(SW_SHOWNORMAL);
			m_EC_CATEGORIES_CUR=5;
			break;
		case EC_POINTS:
			if (!m_EC_POINTS) m_EC_POINTS.Create(EC_POINTS,this);
			m_EC_POINTS.EnableWindow(TRUE);
			m_EC_POINTS.ShowWindow(SW_SHOWNORMAL);
			m_EC_CATEGORIES_CUR=6;
			break;
		 case EC_PLINES:
			if (!m_EC_PLINES) m_EC_PLINES.Create(EC_PLINES,this);
			m_EC_PLINES.EnableWindow(TRUE);
			m_EC_PLINES.ShowWindow(SW_SHOWNORMAL);
			m_EC_CATEGORIES_CUR=7;
			break;
		case EC_SHAPE:
			if (!m_EC_SHAPE) m_EC_SHAPE.Create(EC_SHAPE,this);
			m_EC_SHAPE.EnableWindow(TRUE);
			m_EC_SHAPE.ShowWindow(SW_SHOWNORMAL);
			m_EC_CATEGORIES_CUR=8;
			break;
		case EC_TEXT:
			if (!m_EC_TEXT) m_EC_TEXT.Create(EC_TEXT,this);
			m_EC_TEXT.EnableWindow(TRUE);
			m_EC_TEXT.ShowWindow(SW_SHOWNORMAL);
			m_EC_CATEGORIES_CUR=9;
			break;
		case EC_ATTRIBUTE:
		default:
			if (!m_EC_ATTRIBUTE) m_EC_ATTRIBUTE.Create(EC_ATTRIBUTE,this);
			m_EC_ATTRIBUTE.EnableWindow(TRUE);
			m_EC_ATTRIBUTE.ShowWindow(SW_SHOWNORMAL);
			m_EC_CATEGORIES_CUR=0;
			break;
	}
	int ret=CDialog::OnInitDialog();

	svar_EC_TAB::svar_FillLayer();
	SDS_getvar(NULL,DB_QCLAYER,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	m_CLayer.SelectString(-1,rb.resval.rstring);
	if(rb.resval.rstring) { IC_FREE(rb.resval.rstring);  rb.resval.rstring=NULL; }	

	svar_EC_TAB::svar_FillLineType();
	SDS_getvar(NULL,DB_QCELTYPE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	m_CLtype.SelectString(-1,rb.resval.rstring);
	if(rb.resval.rstring) { IC_FREE(rb.resval.rstring);  rb.resval.rstring=NULL; }	

	svar_EC_TAB::svar_FillLineweight();
	int iWeight = -1, iPos = 0;
	if ( SDS_getvar(NULL,DB_QCELWEIGHT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES) == RTNORM)
		iWeight = rb.resval.rint;

	iPos = db_lweight2index(iWeight);
	m_CLweight.SetCurSel(iPos+3);

	return(ret);
}

void svar_EC_TAB::FillCombo()
{
	CComboBox *pCombo = NULL;

	if (NULL == (pCombo = (CComboBox *) GetDlgItem(EC_CATEGORIES)))
		return;

	pCombo->ResetContent();
	CString	comboOption;
	comboOption.LoadString(IDC_COMBO_EC_CATEGORIES_1);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_EC_CATEGORIES_2);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_EC_CATEGORIES_3);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_EC_CATEGORIES_4);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_EC_CATEGORIES_5);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_EC_CATEGORIES_6);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_EC_CATEGORIES_7);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_EC_CATEGORIES_8);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_EC_CATEGORIES_9);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_EC_CATEGORIES_10);
	pCombo->AddString(comboOption);
}

void svar_EC_TAB::svar_FillLayer() {
	CString csTmp;

	SDS_StartTblRead("LAYER"/*DNT*/);
	while(SDS_TblReadHasMore()) {
  		csTmp=SDS_TblReadString();
		//Bugzilla No. 78070 ; 17-06-2002
		//if(csTmp.Find('|'/*DNT*/)>0) continue;
		if(csTmp.Find(CMD_XREF_TBLDIVIDER)>0) continue;
		m_CLayer.AddString(csTmp);
	}
}

void svar_EC_TAB::svar_FillLineType() {
	CString csTmp;

	SDS_StartTblRead("LTYPE"/*DNT*/);
	while(SDS_TblReadHasMore()) {
  		csTmp=SDS_TblReadString();
		//Bugzilla No. 78070 ; 17-06-2002
		//if(csTmp.Find('|'/*DNT*/)>0) continue;
		if(csTmp.Find(CMD_XREF_TBLDIVIDER)>0) continue;
		m_CLtype.AddString(csTmp);
	}
}

void svar_EC_TAB::svar_FillLineweight() {
    CString csTmp;
    // switch message group depending on $LWUNITS
	resbuf rb;
	int top = 1, pos = 0;
	if( SDS_getvar(NULL,DB_QLWUNITS,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES) == RTNORM )
		top = rb.resval.rint;
	top = ( top == 1 ? IDC_COMBO_M_LWDEFAULT : IDC_COMBO_I_LWDEFAULT );

	m_CLweight.ResetContent();

	// No need to handle VARIES in this dialog
	for(int i = 0 ; i <= 26 ; i++ ) {
		// alternative message when resource string can't load.
		csTmp.Format("LWeight %d", i - 3);
		pos = m_CLweight.AddString(ResourceString(top+i, csTmp));
		m_CLweight.SetItemData(pos, db_index2lweight(i-3));
	}
}


void svar_EC_TAB::OnSelChange() {
	m_EC_CATEGORIES_PRV=m_EC_CATEGORIES_CUR;

	UpdateData(TRUE);;
	if (m_EC_CATEGORIES_PRV==m_EC_CATEGORIES_CUR) return;

	// Disabler.
	switch(m_EC_CATEGORIES_PRV) {
		case 0:
			if (m_EC_ATTRIBUTE) {
				m_EC_ATTRIBUTE.EnableWindow(FALSE);
				m_EC_ATTRIBUTE.ShowWindow(SW_HIDE);
			}
			break;
		case 1:
			if (m_EC_DONUT) {
				m_EC_DONUT.EnableWindow(FALSE);
				m_EC_DONUT.ShowWindow(SW_HIDE);
			}
			break;
		case 2:
			if (m_EC_ELLIPSE) {
				m_EC_ELLIPSE.EnableWindow(FALSE);
				m_EC_ELLIPSE.ShowWindow(SW_HIDE);
			}
			break;
		case 3:
			if (m_EC_SKETCH) {
				m_EC_SKETCH.EnableWindow(FALSE);
				m_EC_SKETCH.ShowWindow(SW_HIDE);
			}
			break;
		case 4:
			if (m_EC_HATCH) {
				m_EC_HATCH.EnableWindow(FALSE);
				m_EC_HATCH.ShowWindow(SW_HIDE);
			}
			break;
		case 5:
			if (m_EC_INSERT) {
				m_EC_INSERT.EnableWindow(FALSE);
				m_EC_INSERT.ShowWindow(SW_HIDE);
			}
			break;
		case 6:
			if (m_EC_POINTS) {
				m_EC_POINTS.EnableWindow(FALSE);
				m_EC_POINTS.ShowWindow(SW_HIDE);
			}
			break;
		case 7:
			if (m_EC_PLINES) {
				m_EC_PLINES.EnableWindow(FALSE);
				m_EC_PLINES.ShowWindow(SW_HIDE);
			}
			break;
		case 8:
			if (m_EC_SHAPE) {
				m_EC_SHAPE.EnableWindow(FALSE);
				m_EC_SHAPE.ShowWindow(SW_HIDE);
			}
			break;
		case 9:
			if (m_EC_TEXT) {
				m_EC_TEXT.EnableWindow(FALSE);
				m_EC_TEXT.ShowWindow(SW_HIDE);
			}
			break;
		default:
			m_EC_CATEGORIES_PRV=m_EC_CATEGORIES_CUR;
			break;
	}

	// Enabler/Creator.
	switch(m_EC_CATEGORIES_CUR) {
		case 0:
			if (!m_EC_ATTRIBUTE) m_EC_ATTRIBUTE.Create(EC_ATTRIBUTE,this);
			m_EC_ATTRIBUTE.EnableWindow(TRUE);
			m_EC_ATTRIBUTE.ShowWindow(SW_SHOWNORMAL);
			svar_ChildDialog=EC_ATTRIBUTE;
			break;
		case 1:
			if (!m_EC_DONUT) m_EC_DONUT.Create(EC_DONUT,this);
			m_EC_DONUT.EnableWindow(TRUE);
			m_EC_DONUT.ShowWindow(SW_SHOWNORMAL);
			svar_ChildDialog=EC_DONUT;
			break;
		case 2:
			if (!m_EC_ELLIPSE) m_EC_ELLIPSE.Create(EC_ELLIPSE,this);
			m_EC_ELLIPSE.EnableWindow(TRUE);
			m_EC_ELLIPSE.ShowWindow(SW_SHOWNORMAL);
			svar_ChildDialog=EC_ELLIPSE;
			break;
		case 3:
			if (!m_EC_SKETCH) m_EC_SKETCH.Create(EC_SKETCH,this);
			m_EC_SKETCH.EnableWindow(TRUE);
			m_EC_SKETCH.ShowWindow(SW_SHOWNORMAL);
			svar_ChildDialog=EC_SKETCH;
			break;
		case 4:
			if (!m_EC_HATCH) m_EC_HATCH.Create(EC_HATCH,this);
			m_EC_HATCH.EnableWindow(TRUE);
			m_EC_HATCH.ShowWindow(SW_SHOWNORMAL);
			svar_ChildDialog=EC_HATCH;
			break;
		case 5:
			if (!m_EC_INSERT) m_EC_INSERT.Create(EC_INSERT,this);
			m_EC_INSERT.EnableWindow(TRUE);
			m_EC_INSERT.ShowWindow(SW_SHOWNORMAL);
			svar_ChildDialog=EC_INSERT;
			break;
		case 6:
			if (!m_EC_POINTS) m_EC_POINTS.Create(EC_POINTS,this);
			m_EC_POINTS.EnableWindow(TRUE);
			m_EC_POINTS.ShowWindow(SW_SHOWNORMAL);
			svar_ChildDialog=EC_POINTS;
			break;
		case 7:
			if (!m_EC_PLINES) m_EC_PLINES.Create(EC_PLINES,this);
			m_EC_PLINES.EnableWindow(TRUE);
			m_EC_PLINES.ShowWindow(SW_SHOWNORMAL);
			svar_ChildDialog=EC_PLINES;
			break;
		case 8:
			if (!m_EC_SHAPE) m_EC_SHAPE.Create(EC_SHAPE,this);
			m_EC_SHAPE.EnableWindow(TRUE);
			m_EC_SHAPE.ShowWindow(SW_SHOWNORMAL);
			svar_ChildDialog=EC_SHAPE;
			break;
		case 9:
			if (!m_EC_TEXT) m_EC_TEXT.Create(EC_TEXT,this);
			m_EC_TEXT.EnableWindow(TRUE);
			m_EC_TEXT.ShowWindow(SW_SHOWNORMAL);
			svar_ChildDialog=EC_TEXT;
			break;
	}
	return;
}

void svar_EC_TAB::DoDataExchange(CDataExchange *pDX) {
	DDX_CBIndex(pDX,EC_CATEGORIES,m_EC_CATEGORIES_CUR);
	DDX_Control(pDX,EC_CLAYER,m_CLayer);
	DDX_Control(pDX,EC_CELTYPE,m_CLtype);
	DDX_Control(pDX,EC_CELWEIGHT,m_CLweight);
}

void svar_EC_TAB::OnLayerChange() {
	char szTmp[IC_ACADNMLEN+1];
	struct resbuf rb;

	rb.restype=RTSTR;
	::GetWindowText(::GetDlgItem(m_hWnd,EC_CLAYER),szTmp,IC_ACADNMLEN);
	rb.resval.rstring= new char [strlen(szTmp)+1];
	strcpy(rb.resval.rstring,szTmp);
	sds_setvar("CLAYER"/*DNT*/,&rb);
	if(rb.resval.rstring) { IC_FREE(rb.resval.rstring);  rb.resval.rstring=NULL;  }	
}

void svar_EC_TAB::OnLtypeChange() {
	char szTmp[IC_ACADNMLEN];
	struct resbuf rb;

	rb.restype=RTSTR;
	::GetWindowText(::GetDlgItem(m_hWnd,EC_CELTYPE),szTmp,IC_ACADNMLEN);
	rb.resval.rstring= new char [strlen(szTmp)+1];
	strcpy(rb.resval.rstring,szTmp);
	sds_setvar("CELTYPE"/*DNT*/,&rb);
	if(rb.resval.rstring) { IC_FREE(rb.resval.rstring);  rb.resval.rstring=NULL;  }	
}

void svar_EC_TAB::OnLweightChange() {
	struct resbuf rb;

	rb.restype=RTSHORT;
	int nIndex = 0, iData = 0;
	nIndex = m_CLweight.GetCurSel();
	iData = m_CLweight.GetItemData(nIndex);
	rb.resval.rint = iData;
	sds_setvar("CELWEIGHT"/*DNT*/,&rb);
}

svar_EC_TAB::svar_EC_TAB() : CDialog(EC_TAB) {
	m_EC_CATEGORIES_CUR=0;
	m_EC_CATEGORIES_PRV=0;
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//************************************************************************
// This section controls the Entity Modification property page and its sub dialogs
//************************************************************************
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

//******This section will control the Trim

class svar_EM_TRIM : public CDialog {
	public:
		svar_EM_TRIM():CDialog(EM_TRIM) {};
		BOOL OnInitDialog();
	protected:
		afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
		virtual void OnOK();
		virtual void OnCancel();

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(svar_EM_TRIM,CDialog)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL svar_EM_TRIM::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_SETTINGS);
	return(true);
}

void svar_EM_TRIM::OnCancel() {
	HWND hParentWnd;

	hParentWnd=::GetParent(m_hWnd);
	hParentWnd=::GetParent(hParentWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void svar_EM_TRIM::OnOK() {
	HWND hParentWnd;

	svar_iHitEnter=1;
	hParentWnd=::GetParent(m_hWnd);
	hParentWnd=::GetParent(hParentWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

BOOL svar_EM_TRIM::OnInitDialog() {
	svar_SubClassItems(this);
	CDialog::OnInitDialog();
	return(TRUE);
}

//******This section will control the Chamfer

class svar_EM_CHAMFER : public CDialog {
	public:
		BOOL b_EM_Pick;
		BOOL OnInitDialog();
		svar_EM_CHAMFER();
	protected:
		afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
		virtual void OnOK();
		virtual void OnCancel();
		afx_msg void OnSpinChamA(NMHDR *pNotifyStruct,LRESULT *result);
		afx_msg void OnSpinChamB(NMHDR *pNotifyStruct,LRESULT *result);
		afx_msg void OnSpinChamC(NMHDR *pNotifyStruct,LRESULT *result);
		afx_msg void OnSpinChamD(NMHDR *pNotifyStruct,LRESULT *result);
		afx_msg void OnSpinRad(NMHDR *pNotifyStruct,LRESULT *result);
		afx_msg void svar_EM_PICK();

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(svar_EM_CHAMFER,CDialog)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_EM_CHAMFERA,OnSpinChamA)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_EM_CHAMFERB,OnSpinChamB)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_EM_CHAMFERC,OnSpinChamC)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_EM_CHAMFERD,OnSpinChamD)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_EM_FILLETRAD,OnSpinRad)
	ON_BN_CLICKED(EM_FILRAD,svar_EM_PICK)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL svar_EM_CHAMFER::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_SETTINGS);
	return(true);
}

void svar_EM_CHAMFER::OnCancel() {
	HWND hParentWnd;

	hParentWnd=::GetParent(m_hWnd);
	hParentWnd=::GetParent(hParentWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void svar_EM_CHAMFER::OnOK() {
	HWND hParentWnd;

	svar_iHitEnter=1;
	hParentWnd=::GetParent(m_hWnd);
	hParentWnd=::GetParent(hParentWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void svar_EM_CHAMFER::svar_EM_PICK() {
	HWND hWnd;

	hWnd=::GetParent(m_hWnd);
	hWnd=::GetParent(hWnd);
	b_EM_Pick=1;
	::SendMessage(hWnd,WM_CLOSE,EM_FILRAD,0);
}

void svar_EM_CHAMFER::OnSpinRad(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,EM_FILLETRAD),pNMUpDown->iDelta);
	*result=0;
}

void svar_EM_CHAMFER::OnSpinChamA(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,EM_CHAMFERA),pNMUpDown->iDelta);
	*result=0;
}

void svar_EM_CHAMFER::OnSpinChamB(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,EM_CHAMFERB),pNMUpDown->iDelta);
	*result=0;
}

void svar_EM_CHAMFER::OnSpinChamC(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,EM_CHAMFERC),pNMUpDown->iDelta);
	*result=0;
}

void svar_EM_CHAMFER::OnSpinChamD(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,EM_CHAMFERD),pNMUpDown->iDelta);
	*result=0;
}

BOOL svar_EM_CHAMFER::OnInitDialog() {
	svar_SubClassItems(this);
	CDialog::OnInitDialog();
	return(TRUE);
}

svar_EM_CHAMFER::svar_EM_CHAMFER():CDialog(EM_CHAMFER) {
	b_EM_Pick=0;
}

//******This sub section will control the Splines sub dialog
class svar_EM_SPLINES : public CDialog {
	public:
		svar_EM_SPLINES():CDialog(EM_SPLINES) {};
		BOOL OnInitDialog();
	protected:
		afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
		virtual void OnOK();
		virtual void OnCancel();
		afx_msg void OnSpinSegs(NMHDR *pNotifyStruct,LRESULT *result);
		afx_msg void OnSpinType(NMHDR *pNotifyStruct,LRESULT *result);

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(svar_EM_SPLINES,CDialog)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_EM_SPLINESEGS,OnSpinSegs)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_EM_SPLINETYPE,OnSpinType)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL svar_EM_SPLINES::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_SETTINGS);
	return(true);
}

void svar_EM_SPLINES::OnCancel() {
	HWND hParentWnd;

	hParentWnd=::GetParent(m_hWnd);
	hParentWnd=::GetParent(hParentWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void svar_EM_SPLINES::OnOK() {
	HWND hParentWnd;

	svar_iHitEnter=1;
	hParentWnd=::GetParent(m_hWnd);
	hParentWnd=::GetParent(hParentWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void svar_EM_SPLINES::OnSpinSegs(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,EM_SPLINESEGS),pNMUpDown->iDelta);
	*result=0;
}

void svar_EM_SPLINES::OnSpinType(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,EM_SPLINETYPE),pNMUpDown->iDelta);
	*result=0;
}

BOOL svar_EM_SPLINES::OnInitDialog() {
	svar_SubClassItems(this);
	CDialog::OnInitDialog();
	return(TRUE);
}

//******This section will control the Entity Modification Propertypage

class svar_EM_TAB : public CDialog {
	public:
		int 				m_EM_CATEGORIES_CUR,
							m_EM_CATEGORIES_PRV;
		svar_EM_CHAMFER 	m_EM_CHAMFER;
		svar_EM_TRIM		m_EM_TRIM;
		svar_EM_SPLINES 	m_EM_SPLINES;

			 svar_EM_TAB();
		BOOL OnInitDialog();

	protected:
		afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
		virtual void OnOK();
		virtual void OnCancel();
		afx_msg void OnSelChange();
		afx_msg void OnSpinOffset(NMHDR *pNotifyStruct,LRESULT *result);
		virtual void DoDataExchange(CDataExchange *pDX);
		void		 FillCombo();

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(svar_EM_TAB,CDialog)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_EM_OFFSETDIST,OnSpinOffset)
	ON_CBN_SELCHANGE(EM_CATEGORIES,OnSelChange)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL svar_EM_TAB::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_SETTINGS);
	return(true);
}

void svar_EM_TAB::OnCancel() {
	HWND hParentWnd;

	hParentWnd=::GetParent(m_hWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void svar_EM_TAB::OnOK() {
	HWND hParentWnd;

	svar_iHitEnter=1;
	hParentWnd=::GetParent(m_hWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void svar_EM_TAB::OnSpinOffset(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,EM_OFFSETDIST),pNMUpDown->iDelta);
	*result=0;
}

BOOL svar_EM_TAB::OnInitDialog() {
	FillCombo();
	svar_SubClassItems(this);
	switch(svar_ChildDialog) {
		case EM_SPLINES:
			if (!m_EM_SPLINES) m_EM_SPLINES.Create(EM_SPLINES,this);
			m_EM_SPLINES.EnableWindow(TRUE);
			m_EM_SPLINES.ShowWindow(SW_SHOWNORMAL);
			m_EM_CATEGORIES_CUR=1;
			break;
		case EM_TRIM:
			if (!m_EM_TRIM) m_EM_TRIM.Create(EM_TRIM,this);
			m_EM_TRIM.EnableWindow(TRUE);
			m_EM_TRIM.ShowWindow(SW_SHOWNORMAL);
			m_EM_CATEGORIES_CUR=2;
			break;
		case EM_CHAMFER:
		default:
			if (!m_EM_CHAMFER) m_EM_CHAMFER.Create(EM_CHAMFER,this);
			m_EM_CHAMFER.EnableWindow(TRUE);
			m_EM_CHAMFER.ShowWindow(SW_SHOWNORMAL);
			m_EM_CATEGORIES_CUR=0;
			break;
	}
	return(CDialog::OnInitDialog());
}

void svar_EM_TAB::FillCombo()
{
	CComboBox *pCombo = NULL;

	if (NULL == (pCombo = (CComboBox *) GetDlgItem(EM_CATEGORIES)))
		return;

	pCombo->ResetContent();
	CString	comboOption;
	comboOption.LoadString(IDC_COMBO_EM_CATEGORIES_1);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_EM_CATEGORIES_2);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_EM_CATEGORIES_3);
	pCombo->AddString(comboOption);
}

void svar_EM_TAB::OnSelChange() {
	m_EM_CATEGORIES_PRV=m_EM_CATEGORIES_CUR;

	UpdateData(TRUE);;
	if (m_EM_CATEGORIES_PRV==m_EM_CATEGORIES_CUR) return;

	// Disabler.
	switch(m_EM_CATEGORIES_PRV) {
		case 0:
			if (m_EM_CHAMFER) {
				m_EM_CHAMFER.EnableWindow(FALSE);
				m_EM_CHAMFER.ShowWindow(SW_HIDE);
			}
			break;
		case 1:
			if (m_EM_SPLINES) {
				m_EM_SPLINES.EnableWindow(FALSE);
				m_EM_SPLINES.ShowWindow(SW_HIDE);
			}
			break;
		case 2:
			if (m_EM_TRIM) {
				m_EM_TRIM.EnableWindow(FALSE);
				m_EM_TRIM.ShowWindow(SW_HIDE);
			}
			break;
		default:
			m_EM_CATEGORIES_PRV=m_EM_CATEGORIES_CUR;
			break;
	}

	// Enabler/Creator.
	switch(m_EM_CATEGORIES_CUR) {
		case 0:
			if (!m_EM_CHAMFER) m_EM_CHAMFER.Create(EM_CHAMFER,this);
			m_EM_CHAMFER.EnableWindow(TRUE);
			m_EM_CHAMFER.ShowWindow(SW_SHOWNORMAL);
			svar_ChildDialog=EM_CHAMFER;
			break;
		case 1:
			if (!m_EM_SPLINES) m_EM_SPLINES.Create(EM_SPLINES,this);
			m_EM_SPLINES.EnableWindow(TRUE);
			m_EM_SPLINES.ShowWindow(SW_SHOWNORMAL);
			svar_ChildDialog=EM_SPLINES;
			break;
		case 2:
			if (!m_EM_TRIM) m_EM_TRIM.Create(EM_TRIM,this);
			m_EM_TRIM.EnableWindow(TRUE);
			m_EM_TRIM.ShowWindow(SW_SHOWNORMAL);
			svar_ChildDialog=EM_TRIM;
			break;
	}
	return;
}

svar_EM_TAB::svar_EM_TAB() : CDialog(EM_TAB) {
	m_EM_CATEGORIES_CUR=0;
	m_EM_CATEGORIES_PRV=0;
}

void svar_EM_TAB::DoDataExchange(CDataExchange *pDX) {
	DDX_CBIndex(pDX,EM_CATEGORIES,m_EM_CATEGORIES_CUR);
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//************************************************************************
// This section controls the 3D property page and its sub dialogs
//************************************************************************
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

//******This section will control the Shade

class svar_TH_SHADE : public CDialog {
	public:
		svar_TH_SHADE():CDialog(TH_SHADE) {};
		BOOL OnInitDialog();
	protected:
		afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
		virtual void OnOK();
		virtual void OnCancel();
		afx_msg void OnSpinDif(NMHDR *pNotifyStruct,LRESULT *result);

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(svar_TH_SHADE,CDialog)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_TH_SHADEDIF,OnSpinDif)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL svar_TH_SHADE::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_SETTINGS);
	return(true);
}

void svar_TH_SHADE::OnCancel() {
	HWND hParentWnd;

	hParentWnd=::GetParent(m_hWnd);
	hParentWnd=::GetParent(hParentWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void svar_TH_SHADE::OnOK() {
	HWND hParentWnd;

	svar_iHitEnter=1;
	hParentWnd=::GetParent(m_hWnd);
	hParentWnd=::GetParent(hParentWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void svar_TH_SHADE::OnSpinDif(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,TH_SHADEDIF),pNMUpDown->iDelta);
	*result=0;
}

BOOL svar_TH_SHADE::OnInitDialog() {
	svar_SubClassItems(this);
	CDialog::OnInitDialog();
	return(TRUE);
}

//******This section will control the Surfaces

class svar_TH_SURFACES : public CDialog {
	public:
		svar_TH_SURFACES(): CDialog(TH_SURFACES) {};
		BOOL OnInitDialog();
	protected:
		afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
		virtual void OnOK();
		virtual void OnCancel();
		afx_msg void OnSpinTab1(NMHDR *pNotifyStruct,LRESULT *result);
		afx_msg void OnSpinTab2(NMHDR *pNotifyStruct,LRESULT *result);
		afx_msg void OnSpinFU(NMHDR *pNotifyStruct,LRESULT *result);
		afx_msg void OnSpinFV(NMHDR *pNotifyStruct,LRESULT *result);

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(svar_TH_SURFACES,CDialog)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_TH_SURFTAB1,OnSpinTab1)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_TH_SURFTAB2,OnSpinTab2)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_TH_SURFU,OnSpinFU)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_TH_SURFV,OnSpinFV)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL svar_TH_SURFACES::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_SETTINGS);
	return(true);
}

void svar_TH_SURFACES::OnCancel() {
	HWND hParentWnd;

	hParentWnd=::GetParent(m_hWnd);
	hParentWnd=::GetParent(hParentWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void svar_TH_SURFACES::OnOK() {
	HWND hParentWnd;

	svar_iHitEnter=1;
	hParentWnd=::GetParent(m_hWnd);
	hParentWnd=::GetParent(hParentWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void svar_TH_SURFACES::OnSpinTab1(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,TH_SURFTAB1),pNMUpDown->iDelta);
	*result=0;
}

void svar_TH_SURFACES::OnSpinTab2(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,TH_SURFTAB2),pNMUpDown->iDelta);
	*result=0;
}

void svar_TH_SURFACES::OnSpinFU(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,TH_SURFU),pNMUpDown->iDelta);
	*result=0;
}

void svar_TH_SURFACES::OnSpinFV(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,TH_SURFV),pNMUpDown->iDelta);
	*result=0;
}

BOOL svar_TH_SURFACES::OnInitDialog() {
	svar_SubClassItems(this);
	CDialog::OnInitDialog();
	return(TRUE);
}

//******This section will control the 3D Settings Propertypage

class svar_TH_TAB : public CDialog {
	public:
		int 			m_TH_CATEGORIES_CUR,
						m_TH_CATEGORIES_PRV;

		svar_TH_SHADE	 m_TH_SHADE;
		svar_TH_SURFACES m_TH_SURFACES;

		svar_TH_TAB() ;
		BOOL OnInitDialog();
	protected:
		afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
		virtual void OnOK();
		virtual void OnCancel();
		afx_msg void OnSelChange();
		afx_msg void OnSpinThick(NMHDR *pNotifyStruct,LRESULT *result);
		afx_msg void OnSpinElev(NMHDR *pNotifyStruct,LRESULT *result);
		virtual void DoDataExchange(CDataExchange *pDX);
				void FillCombo();

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(svar_TH_TAB,CDialog)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_TH_THICKNESS,OnSpinThick)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_TH_ELEVATION,OnSpinElev)
	ON_CBN_SELCHANGE(TH_CATEGORIES,OnSelChange)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL svar_TH_TAB::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_SETTINGS);
	return(true);
}

void svar_TH_TAB::OnCancel() {
	HWND hParentWnd;

	hParentWnd=::GetParent(m_hWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void svar_TH_TAB::OnOK() {
	HWND hParentWnd;

	svar_iHitEnter=1;
	hParentWnd=::GetParent(m_hWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void svar_TH_TAB::OnSpinThick(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,TH_THICKNESS),pNMUpDown->iDelta);
	*result=0;
}

void svar_TH_TAB::OnSpinElev(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,TH_ELEVATION),pNMUpDown->iDelta);
	*result=0;
}

BOOL svar_TH_TAB::OnInitDialog() {
	FillCombo();
	svar_SubClassItems(this);
	switch(svar_ChildDialog) {
		case TH_SURFACES:
			if (!m_TH_SURFACES) m_TH_SURFACES.Create(TH_SURFACES,this);
			m_TH_SURFACES.EnableWindow(TRUE);
			m_TH_SURFACES.ShowWindow(SW_SHOWNORMAL);
			m_TH_CATEGORIES_CUR=1;
			break;
		case TH_SHADE:
		default:
			if (!m_TH_SHADE) m_TH_SHADE.Create(TH_SHADE,this);
			m_TH_SHADE.EnableWindow(TRUE);
			m_TH_SHADE.ShowWindow(SW_SHOWNORMAL);
			m_TH_CATEGORIES_CUR=0;
			break;
	}
	return(CDialog::OnInitDialog());
}

void svar_TH_TAB::FillCombo()
{
	CComboBox *pCombo = NULL;

	if (NULL == (pCombo = (CComboBox *) GetDlgItem(TH_CATEGORIES)))
		return;

	pCombo->ResetContent();
	CString	comboOption;
	comboOption.LoadString(IDC_COMBO_TH_CATEGORIES_1);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_TH_CATEGORIES_2);
	pCombo->AddString(comboOption);
}

void svar_TH_TAB::OnSelChange() {
	m_TH_CATEGORIES_PRV=m_TH_CATEGORIES_CUR;

	UpdateData(TRUE);;
	if (m_TH_CATEGORIES_PRV==m_TH_CATEGORIES_CUR) return;

	// Disabler.
	switch(m_TH_CATEGORIES_PRV) {
		case 0:
			if (m_TH_SHADE) {
				m_TH_SHADE.EnableWindow(FALSE);
				m_TH_SHADE.ShowWindow(SW_HIDE);
			}
			break;
		case 1:
			if (m_TH_SURFACES) {
				m_TH_SURFACES.EnableWindow(FALSE);
				m_TH_SURFACES.ShowWindow(SW_HIDE);
			}
			break;
		default:
			m_TH_CATEGORIES_PRV=m_TH_CATEGORIES_CUR;
			break;
	}

	// Enabler/Creator.
	switch(m_TH_CATEGORIES_CUR) {
		case 0:
			if (!m_TH_SHADE) m_TH_SHADE.Create(TH_SHADE,this);
			m_TH_SHADE.EnableWindow(TRUE);
			m_TH_SHADE.ShowWindow(SW_SHOWNORMAL);
			svar_ChildDialog=TH_SHADE;
			break;
		case 1:
			if (!m_TH_SURFACES) m_TH_SURFACES.Create(TH_SURFACES,this);
			m_TH_SURFACES.EnableWindow(TRUE);
			m_TH_SURFACES.ShowWindow(SW_SHOWNORMAL);
			svar_ChildDialog=TH_SURFACES;
			break;
	}
	return;
}

void svar_TH_TAB::DoDataExchange(CDataExchange *pDX) {
	DDX_CBIndex(pDX,TH_CATEGORIES,m_TH_CATEGORIES_CUR);
}

svar_TH_TAB::svar_TH_TAB() : CDialog(TH_TAB) {
	m_TH_CATEGORIES_CUR=0;
	m_TH_CATEGORIES_PRV=0;
}

//
// -----------------------This is for the Setvars property pages----------------------
//

class svar_DrawingCfg : public IcadDialog {
	public:
		int m_CurSel;
		CTabCtrl m_CTabCtrl;
		svar_AN_TAB m_AN_TAB;
		svar_CI_TAB m_CI_TAB;
		svar_DV_TAB m_DV_TAB;
		svar_EC_TAB m_EC_TAB;
		svar_EM_TAB m_EM_TAB;
		svar_TH_TAB m_TH_TAB;

		svar_DrawingCfg();
		BOOL OnInitDialog();
	protected:
		afx_msg void OnOK();
		afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
		afx_msg void OnHelp();
		virtual void DoDataExchange(CDataExchange *pDX);
	   	afx_msg void OnDestroy();
	 	afx_msg void OnTabChange(NMHDR *pNotifyStruct,LRESULT *result);

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(svar_DrawingCfg,CDialog)
	ON_BN_CLICKED(IDHELP,OnHelp)
	ON_WM_DESTROY()
	ON_NOTIFY(TCN_SELCHANGE,SVAR_TABS,OnTabChange)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

void svar_DrawingCfg::OnOK(){
	SDS_VarToTab(SDS_CURDWG,SDS_CURVIEW->m_pVportTabHip);
	CDialog::OnOK();
}

BOOL svar_DrawingCfg::OnHelpInfo(HELPINFO *pHel){
	OnHelp();
	return(true);
}

void svar_DrawingCfg::OnHelp() {
	CommandRefHelp( m_hWnd, HLP_SETTINGS);
}

void svar_DrawingCfg::DoDataExchange(CDataExchange *pDX) {
	DDX_Control(pDX,SVAR_TABS,m_CTabCtrl);
}

//This is for a tab change
void svar_DrawingCfg::OnTabChange(NMHDR *pNotifyStruct,LRESULT *result) {
	//Disable
	switch(m_CurSel) {
		case 0:
			m_AN_TAB.EnableWindow(FALSE);
			m_AN_TAB.ShowWindow(SW_HIDE);
			break;
		case 1:
			m_CI_TAB.EnableWindow(FALSE);
			m_CI_TAB.ShowWindow(SW_HIDE);
			break;
		case 2:
			m_DV_TAB.EnableWindow(FALSE);
			m_DV_TAB.ShowWindow(SW_HIDE);
			break;
		case 3:
			m_EC_TAB.EnableWindow(FALSE);
			m_EC_TAB.ShowWindow(SW_HIDE);
			break;
		case 4:
			m_EM_TAB.EnableWindow(FALSE);
			m_EM_TAB.ShowWindow(SW_HIDE);
			break;
		case 5:
			m_TH_TAB.EnableWindow(FALSE);
			m_TH_TAB.ShowWindow(SW_HIDE);
			break;
	}
	//Enable
	m_CurSel=m_CTabCtrl.GetCurSel(); //Get the current selection
	switch(m_CurSel) {
		case 0:
			if (!m_AN_TAB) m_AN_TAB.Create(AN_TAB,this);
			m_AN_TAB.EnableWindow(TRUE);
			m_AN_TAB.ShowWindow(SW_SHOWNORMAL);
			::SetFocus(m_AN_TAB.m_hWnd);
			break;
		case 1:
			if (!m_CI_TAB) m_CI_TAB.Create(CI_TAB,this);
			m_CI_TAB.EnableWindow(TRUE);
			m_CI_TAB.ShowWindow(SW_SHOWNORMAL);
			::SetFocus(m_CI_TAB.m_hWnd);
			break;
		case 2:
			if (!m_DV_TAB) m_DV_TAB.Create(DV_TAB,this);
			m_DV_TAB.EnableWindow(TRUE);
			m_DV_TAB.ShowWindow(SW_SHOWNORMAL);
			::SetFocus(m_DV_TAB.m_hWnd);
			break;
		case 3:
			if (!m_EC_TAB) m_EC_TAB.Create(EC_TAB,this);
			m_EC_TAB.EnableWindow(TRUE);
			m_EC_TAB.ShowWindow(SW_SHOWNORMAL);
			::SetFocus(m_EC_TAB.m_hWnd);
			break;
		case 4:
			if (!m_EM_TAB) m_EM_TAB.Create(EM_TAB,this);
			m_EM_TAB.EnableWindow(TRUE);
			m_EM_TAB.ShowWindow(SW_SHOWNORMAL);
			::SetFocus(m_EM_TAB.m_hWnd);
			break;
		case 5:
			if (!m_TH_TAB) m_TH_TAB.Create(TH_TAB,this);
			m_TH_TAB.EnableWindow(TRUE);
			m_TH_TAB.ShowWindow(SW_SHOWNORMAL);
			::SetFocus(m_TH_TAB.m_hWnd);
			break;
	}
}

void svar_DrawingCfg::OnDestroy() {
	switch(m_CTabCtrl.GetCurSel()) {
		case 0:
			switch(svar_ChildDialog) {
				case AN_ANGULAR:
				case AN_LINEAR:
				case AN_UCS:
					break;
				default:
					svar_ChildDialog=AN_ANGULAR;
			}
			break;
		case 1:
			switch(svar_ChildDialog) {
				case CI_ENTSEL:
				case CI_GRID:
				case CI_SNAP:
				case CI_LIMITS:
					break;
				default:
					svar_ChildDialog=CI_GRID;
			}
			break;
		case 2:
			switch(svar_ChildDialog) {
				case DV_DISPLAY:
				case DV_LIMITS:
				case DV_NODES:
				case DV_PROGRAMINT:
				case DV_ZOOM:
				case DV_LWEIGHT:
					break;
				default:
					svar_ChildDialog=DV_DISPLAY;
			}
			break;
		case 3:
			switch(svar_ChildDialog) {
				case EC_ATTRIBUTE:
				case EC_DONUT:
				case EC_ELLIPSE:
				case EC_HATCH:
				case EC_INSERT:
				case EC_PLINES:
				case EC_POINTS:
				case EC_SHAPE:
				case EC_SKETCH:
				case EC_TEXT:
					break;
				default:
					svar_ChildDialog=EC_ATTRIBUTE;
			}
			break;
		case 4:
			switch(svar_ChildDialog) {
				case EM_CHAMFER:
				case EM_SPLINES:
				case EM_TRIM:
					break;
				default:
					svar_ChildDialog=EM_CHAMFER;
			}
			break;
		case 5:
			switch(svar_ChildDialog) {
				case TH_SHADE:
				case TH_SURFACES:
					break;
				default:
					svar_ChildDialog=TH_SHADE;
			}
			break;
	}
	CDialog::OnDestroy();
}

BOOL svar_DrawingCfg::OnInitDialog() {
	int ret=IcadDialog::OnInitDialog();
	TC_ITEM TabCtrlItem;

	TabCtrlItem.mask=TCIF_TEXT;

	EditString	itemText;
	itemText = ResourceString(IDC_ICADSETVARSDIA_DRAWING_119, "Drawing Units" );
	TabCtrlItem.pszText=itemText;
	m_CTabCtrl.InsertItem(0,&TabCtrlItem);

	itemText = ResourceString(IDC_ICADSETVARSDIA_COORDIN_120, "Coordinate Input" );
	TabCtrlItem.pszText=itemText;
	m_CTabCtrl.InsertItem(1,&TabCtrlItem);

	itemText = ResourceString(IDC_ICADSETVARSDIA_DISPLAY_121, "Display" );
	TabCtrlItem.pszText=itemText;
	m_CTabCtrl.InsertItem(2,&TabCtrlItem);

	itemText = ResourceString(IDC_ICADSETVARSDIA_ENTITY__122, "Entity Creation" );
	TabCtrlItem.pszText=itemText;
	m_CTabCtrl.InsertItem(3,&TabCtrlItem);

	itemText = ResourceString(IDC_ICADSETVARSDIA_ENTITY__123, "Entity Modification" );
	TabCtrlItem.pszText=itemText;
	m_CTabCtrl.InsertItem(4,&TabCtrlItem);

	itemText = ResourceString(IDC_ICADSETVARSDIA_3D_SETT_124, "3D Settings" );
	TabCtrlItem.pszText=itemText;
	m_CTabCtrl.InsertItem(5,&TabCtrlItem);

	//Set the tab to first display
	switch(svar_ChildDialog) {
		case CI_ENTSEL:
		case CI_GRID:
		case CI_SNAP:
		case CI_LIMITS:
			m_CTabCtrl.SetCurSel(1);
			m_CurSel=1;
			if (!m_CI_TAB) m_CI_TAB.Create(CI_TAB,this);
			m_CI_TAB.EnableWindow(TRUE);
			m_CI_TAB.ShowWindow(SW_SHOWNORMAL);
			::SetFocus(m_CI_TAB.m_hWnd);
			break;
		case DV_DISPLAY:
		case DV_LIMITS:
		case DV_NODES:
		case DV_PROGRAMINT:
		case DV_ZOOM:
		case DV_LWEIGHT:
			m_CTabCtrl.SetCurSel(2);
			m_CurSel=2;
			if (!m_DV_TAB) m_DV_TAB.Create(DV_TAB,this);
			m_DV_TAB.EnableWindow(TRUE);
			m_DV_TAB.ShowWindow(SW_SHOWNORMAL);
			::SetFocus(m_DV_TAB.m_hWnd);
			break;
		case EC_ATTRIBUTE:
		case EC_DONUT:
		case EC_ELLIPSE:
		case EC_HATCH:
		case EC_INSERT:
		case EC_PLINES:
		case EC_POINTS:
		case EC_SHAPE:
		case EC_SKETCH:
		case EC_TEXT:
			m_CTabCtrl.SetCurSel(3);
			m_CurSel=3;
			if (!m_EC_TAB) m_EC_TAB.Create(EC_TAB,this);
			m_EC_TAB.EnableWindow(TRUE);
			m_EC_TAB.ShowWindow(SW_SHOWNORMAL);
			::SetFocus(m_EC_TAB.m_hWnd);
			break;
		case EM_CHAMFER:
		case EM_SPLINES:
		case EM_TRIM:
			m_CTabCtrl.SetCurSel(4);
			m_CurSel=4;
			if (!m_EM_TAB) m_EM_TAB.Create(EM_TAB,this);
			m_EM_TAB.EnableWindow(TRUE);
			m_EM_TAB.ShowWindow(SW_SHOWNORMAL);
			::SetFocus(m_EM_TAB.m_hWnd);
			break;
		case TH_SHADE:
		case TH_SURFACES:
			m_CTabCtrl.SetCurSel(5);
			m_CurSel=5;
			if (!m_TH_TAB) m_TH_TAB.Create(TH_TAB,this);
			m_TH_TAB.EnableWindow(TRUE);
			m_TH_TAB.ShowWindow(SW_SHOWNORMAL);
			::SetFocus(m_TH_TAB.m_hWnd);
			break;
		case AN_ANGULAR:
		case AN_LINEAR:
		case AN_UCS:
			m_CTabCtrl.SetCurSel(0);
			m_CurSel=0;
			if (!m_AN_TAB) m_AN_TAB.Create(AN_TAB,this);
			m_AN_TAB.EnableWindow(TRUE);
			m_AN_TAB.ShowWindow(SW_SHOWNORMAL);
			::SetFocus(m_AN_TAB.m_hWnd);
			break;
		default:
			m_CTabCtrl.SetCurSel(0);
			m_CurSel=0;
			if (!m_AN_TAB) m_AN_TAB.Create(AN_TAB,this);
			m_AN_TAB.EnableWindow(TRUE);
			m_AN_TAB.ShowWindow(SW_SHOWNORMAL);
			svar_ChildDialog=AN_ANGULAR;
			::SetFocus(m_AN_TAB.m_hWnd);
			break;
	}
	return(ret);
}

svar_DrawingCfg::svar_DrawingCfg():IcadDialog(SVAR_GENERAL) {
	svar_iHitEnter=0;
}

//=================================================================
//=================================================================

void CMainWindow::IcadSetvarsDia(void) {
	int ret,i,iTmp;
	struct resbuf rb;
	sds_point pt1,pt2;
	double tmp;
	char fs1[IC_ACADBUF],fs2[IC_ACADBUF],OldLayer[IC_ACADNMLEN+1],OldLtype[IC_ACADNMLEN+1];
	int OldLweight = -1, OldLwdefault = -1, OldLwdisplay = -1, OldLwunits = 1, OldLwDisplayScaleFactor = 8;


	// Lineweight Display Scale
	CIcadApp* pIcadApp = (CIcadApp*)AfxGetApp();
	if (pIcadApp != NULL)
		OldLwDisplayScaleFactor = pIcadApp->m_nLWDisplayScaleFactor;

	// Current Layer
	SDS_getvar(NULL,DB_QCLAYER,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	strcpy(OldLayer,rb.resval.rstring);
	if(rb.resval.rstring) { IC_FREE(rb.resval.rstring);  rb.resval.rstring=NULL; }	

	// Currentl Linetype
	SDS_getvar(NULL,DB_QCELTYPE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	strcpy(OldLtype,rb.resval.rstring);
	if(rb.resval.rstring) { IC_FREE(rb.resval.rstring);  rb.resval.rstring=NULL; }	

	// Current Lineweight
	SDS_getvar(NULL,DB_QCELWEIGHT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	OldLweight = rb.resval.rint;

	// Lineweight default
	SDS_getvar(NULL,DB_QLWDEFAULT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	OldLwdefault = rb.resval.rint;

	// Display Lineweight
	SDS_getvar(NULL,DB_QLWDISPLAY,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	OldLwdisplay = rb.resval.rint;

	// Lineweight units
	SDS_getvar(NULL,DB_QLWUNITS,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	OldLwunits = rb.resval.rint;


	svar_CurDlgItem=svar_DlgItemList; //set the pointer to the proper struct
	svar_num_of_vars=sizeof(svar_DlgItemList)/sizeof(svar_DlgItemList[0]); //get the num in the struct

	svar_DrawingCfg svar_Drawing;

	if(SDS_CURDWG==NULL) return;
	SDS_CURDWG->copyheader();
	SDS_CURSES.copybuf();
	SDS_CURCFG.copybuf();

	do {
		ret=svar_Drawing.DoModal();
		if((ret==IDCANCEL) && (svar_Drawing.m_DV_TAB.m_DV_LIMITS.b_DV_Pick==1)){
			svar_Drawing.m_DV_TAB.m_DV_LIMITS.b_DV_Pick=0;
			sds_initget(9|SDS_RSG_NODOCCHG,NULL);
			if(internalGetpoint(NULL,ResourceString(IDC_ICADSETVARSDIA__NSELEC_125, "\nSelect first corner: " ),pt1)!=RTNORM) continue;
			sds_initget(9|SDS_RSG_NODOCCHG,NULL);
			if(sds_getcorner(pt1,ResourceString(IDC_ICADSETVARSDIA__NSELEC_126, "\nSelect second corner: " ),pt2)!=RTNORM) continue;
			if(pt1[0]>pt2[0]) {  //put the left point in to pt1
				tmp=pt1[0];
				pt1[0]=pt2[0];
				pt2[0]=tmp;
			}
			if(pt1[1]>pt2[1]) {  //put the lower point in to pt1
				tmp=pt1[1];
				pt1[1]=pt2[1];
				pt2[1]=tmp;
			}
			rb.restype=RTPOINT;
			rb.resval.rpoint[0]=pt1[0];rb.resval.rpoint[1]=pt1[1];
			sds_setvar("LIMMIN"/*DNT*/,&rb);
			rb.resval.rpoint[0]=pt2[0];rb.resval.rpoint[1]=pt2[1];
			sds_setvar("LIMMAX"/*DNT*/,&rb);
		}else if((ret==IDCANCEL) && (svar_Drawing.m_CI_TAB.m_CI_LIMITS.b_CI_Pick==1)){
			svar_Drawing.m_CI_TAB.m_CI_LIMITS.b_CI_Pick=0;
			sds_initget(9|SDS_RSG_NODOCCHG,NULL);
			if(internalGetpoint(NULL,ResourceString(IDC_ICADSETVARSDIA__NSELEC_125, "\nSelect first corner: " ),pt1)!=RTNORM) continue;
			sds_initget(9|SDS_RSG_NODOCCHG,NULL);
			if(sds_getcorner(pt1,ResourceString(IDC_ICADSETVARSDIA__NSELEC_126, "\nSelect second corner: " ),pt2)!=RTNORM) continue;
			if(pt1[0]>pt2[0]) {  //put the left point in to pt1
				tmp=pt1[0];
				pt1[0]=pt2[0];
				pt2[0]=tmp;
			}
			if(pt1[1]>pt2[1]) {  //put the lower point in to pt1
				tmp=pt1[1];
				pt1[1]=pt2[1];
				pt2[1]=tmp;
			}
			rb.restype=RTPOINT;
			rb.resval.rpoint[0]=pt1[0];rb.resval.rpoint[1]=pt1[1];
			sds_setvar("LIMMIN"/*DNT*/,&rb);
			rb.resval.rpoint[0]=pt2[0];rb.resval.rpoint[1]=pt2[1];
			sds_setvar("LIMMAX"/*DNT*/,&rb);
		}else if((ret==IDCANCEL) && (svar_Drawing.m_EM_TAB.m_EM_CHAMFER.b_EM_Pick==1)) {
			svar_Drawing.m_EM_TAB.m_EM_CHAMFER.b_EM_Pick=0;
			SDS_getvar(NULL,DB_QFILLETRAD,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			sds_rtos(rb.resval.rreal,-1,-1,fs1);
			sprintf(fs2,ResourceString(IDC_ICADSETVARSDIA__NFILLE_127, "\nFillet radius <%s>: " ),fs1);
			sds_initget(5|SDS_RSG_NODOCCHG,NULL);
		   	if(sds_getdist(NULL,fs2,&tmp)!=RTNORM) continue;
			rb.restype=RTREAL;rb.rbnext=NULL;rb.resval.rreal=tmp;
			sds_setvar("FILLETRAD"/*DNT*/,&rb);
		}else if((ret==IDCANCEL) && (svar_Drawing.m_EC_TAB.m_EC_INSERT.b_EC_Pick==1)) {
			svar_Drawing.m_EC_TAB.m_EC_INSERT.b_EC_Pick=0;
			sds_initget(9|SDS_RSG_NODOCCHG,NULL);
			if(internalGetpoint(NULL,ResourceString(IDC_ICADSETVARSDIA__NSELEC_128, "\nSelect base point: " ),pt1)!=RTNORM) continue;
			rb.restype=RT3DPOINT;
			rb.resval.rpoint[0]=pt1[0];rb.resval.rpoint[1]=pt1[1];rb.resval.rpoint[2]=pt1[2];
			sds_setvar("INSBASE"/*DNT*/,&rb);
		}else if((ret==IDCANCEL) && (svar_iHitEnter==0)) {
			svar_Drawing.~svar_Drawing();
			rb.restype=RTSTR;
			rb.resval.rstring= new char [strlen(OldLayer)+1];
			strcpy(rb.resval.rstring,OldLayer);
			sds_setvar("CLAYER"/*DNT*/,&rb);
			if(rb.resval.rstring) { IC_FREE(rb.resval.rstring);  rb.resval.rstring=NULL; }			
			rb.resval.rstring= new char [strlen(OldLtype)+1];
			strcpy(rb.resval.rstring,OldLtype);
			sds_setvar("CELTYPE"/*DNT*/,&rb);
			if(rb.resval.rstring) { IC_FREE(rb.resval.rstring);  rb.resval.rstring=NULL; }			

			rb.restype=RTSHORT;
			rb.resval.rint = OldLweight;
			sds_setvar("CELWEIGHT", &rb);

			rb.resval.rint = OldLwdefault;
			sds_setvar("LWDEFAULT", &rb);

			rb.resval.rint = OldLwunits;
			sds_setvar("LWUNITS", &rb);

			rb.resval.rint = OldLwdisplay;
			sds_setvar("LWDISPLAY", &rb);

			if (pIcadApp != NULL)
				pIcadApp->m_nLWDisplayScaleFactor = OldLwDisplayScaleFactor;

			rb.resval.rint=svar_Drawing.m_DV_TAB.m_DV_NODES.iGripSize;
			sds_setvar("GRIPSIZE"/*DNT*/,&rb);
			rb.resval.rint=svar_Drawing.m_DV_TAB.m_DV_NODES.iGripColor;
			sds_setvar("GRIPCOLOR"/*DNT*/,&rb);

			SDS_CURDWG->restoreheader();
			SDS_CURSES.restorebuf();
			SDS_CURCFG.restorebuf();

			// AG: update status bar
			extern char *SDS_UpdateSetvar;
			SDS_UpdateSetvar=NULL;
			IcadWndAction( ICAD_WNDACTION_STATCHANGE );

			::ExecuteUIAction( ICAD_WNDACTION_UDSTCOL );
			break;
		} else {  //must be OK
			rb.restype=RTSHORT;
			rb.resval.rint=svar_Drawing.m_CI_TAB.m_CI_GRID.iGridMode;
			sds_setvar("GRIDMODE"/*DNT*/,&rb);
			rb.resval.rint=svar_Drawing.m_DV_TAB.m_DV_NODES.iGrips;
			sds_setvar("GRIPS"/*DNT*/,&rb);

			SDS_getvar(NULL,DB_QUCSICON,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			if(rb.resval.rint!=svar_Drawing.m_AN_TAB.m_AN_UCS.iUcsTest){
				iTmp=rb.resval.rint; //save new value
				rb.resval.rint=svar_Drawing.m_AN_TAB.m_AN_UCS.iUcsTest;//restore orig value so we can paint out
				SDS_setvar(NULL,DB_QUCSICON,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
				if(SDS_CURDWG && SDS_CURVIEW) SDS_VarToTab(SDS_CURDWG,SDS_CURVIEW->m_pVportTabHip);
				SDS_DrawUcsIcon(NULL,NULL,NULL,1);	//paint out
				rb.resval.rint=iTmp; //restore correct value for repaint
				SDS_setvar(NULL,DB_QUCSICON,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
				if(SDS_CURDWG && SDS_CURVIEW){
					SDS_VarToTab(SDS_CURDWG,SDS_CURVIEW->m_pVportTabHip);
					if(rb.resval.rint&1)
						SDS_DrawUcsIcon(NULL,NULL,NULL,0);//paint in
				}
			}

			// If the status of Lineweights have changed then perform a regen and update toolbars
			// Lineweight units
			SDS_getvar(NULL,DB_QLWDISPLAY,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			if (((pIcadApp != NULL) && (pIcadApp->m_nLWDisplayScaleFactor != OldLwDisplayScaleFactor)) ||
				(rb.resval.rint != OldLwdisplay)) {

				// regen lineweights
				cmd_regenall_lweight();

				// Update Lineweight Combo-box in Entity Properties Toolbar.
				if (SDS_CMainWindow->m_wndLWeight.GetSafeHwnd()) {
					SDS_CMainWindow->m_wndLWeight.m_DisplayScaleFactor = pIcadApp->m_nLWDisplayScaleFactor;
					SDS_CMainWindow->m_wndLWeight.UpdateLWeightList();
				}
			}
			break;
		}
	}while(1);

	//Modified Cybage AW 17-12-01 [
	//Reason : DIASTAT Variable functionality added
	//Bug no. 54233 from BugZilla
	if(ret==IDCANCEL)
	{
		struct resbuf rb1;
		rb1.restype = RTSHORT;
		rb1.resval.rint = 0;
		SDS_setvar(NULL,DB_QDIASTAT,&rb1,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);

	}
	//Modified Cybage AW 17-12-01 ]

	for(i=0;i<svar_num_of_vars;i++) {
		svar_CurDlgItem[i].WndProc=NULL;
	}

	SDS_CURDWG->freeheaderbufcopy();
	SDS_CURSES.freebufcopy();
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//****************************************************************************
// This section is for the Dim tabs and related dialogs
//****************************************************************************
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

//******This section will control the Dim Alt property page

class svar_DIM_ALT : public CDialog {
	private:
		//Added Cybage AW 11-03-03
		//Reason : Bug 78025 In Bugzilla.
		BOOL bOnchar;
	public:
		svar_DIM_ALT();
		BOOL OnInitDialog();
		void PaintPic();
		CStatic m_CAltPic;
		CBitmap *Bitmap,*OldBitmap;
	protected:
		afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
		afx_msg void OnSpinALTD(NMHDR *pNotifyStruct,LRESULT *result);
		afx_msg void OnSpinALTF(NMHDR *pNotifyStruct,LRESULT *result);
		afx_msg void OnSpinALTTD(NMHDR *pNotifyStruct,LRESULT *result);
		afx_msg void OnSpinALTRND(NMHDR *pNotifyStruct,LRESULT *result);
		virtual void OnOK();
		virtual void OnCancel();
		virtual BOOL OnCommand(WPARAM wParam,LPARAM lParam);
		virtual void DoDataExchange(CDataExchange* pDX);
		void		 FillCombo();
		afx_msg void OnPaint(void);

		//Added Cybage AW 11-03-03
		//Reason : Bug 78025 In Bugzilla.
		virtual BOOL PreTranslateMessage( MSG* pMsg );

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(svar_DIM_ALT,CDialog)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_DIM_ALT_DIMALTD,OnSpinALTD)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_DIM_ALT_DIMALTF,OnSpinALTF)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_DIM_ALT_DIMALTTD,OnSpinALTTD)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_DIM_ALT_DIMALTRND,OnSpinALTRND)
	ON_WM_HELPINFO()
	ON_WM_PAINT()
END_MESSAGE_MAP()

//Added Cybage AW 11-03-03 [
//Reason : Bug 78025 In Bugzilla.
BOOL svar_DIM_ALT::PreTranslateMessage(MSG* pMsg)
{
	bOnchar = FALSE;
	if(pMsg->message == WM_CHAR || pMsg->wParam == VK_DELETE)
		bOnchar = TRUE;
	return CDialog::PreTranslateMessage(pMsg);
}
//Added Cybage AW 11-03-03]

BOOL svar_DIM_ALT::OnCommand(WPARAM wParam,LPARAM lParam){

	if(HIWORD(wParam)==BN_CLICKED) {
		switch(LOWORD(wParam)) {
			case DIM_ALT_DIMALT:
			case DIM_ALT_DIMALTZ1:
			case DIM_ALT_DIMALTZ2:
			case DIM_ALT_DIMALTTZ1:
			case DIM_ALT_DIMALTTZ2:
				PaintPic();
				break;
		}
	}else if(HIWORD(wParam)==EN_KILLFOCUS) {
		switch(LOWORD(wParam)) {
			case DIM_ALT_DIMALTD:
			case DIM_ALT_DIMALTF:
			case DIM_ALT_DIMAPOST1:
			case DIM_ALT_DIMAPOST2:
			case DIM_ALT_DIMALTTD:
			case DIM_ALT_DIMALTRND:
				PaintPic();
				break;
		}
	}else if(HIWORD(wParam)==LBN_SELCHANGE) {
		switch(LOWORD(wParam)) {
			case DIM_ALT_DIMALTU:
				PaintPic();
				break;
		}
	}
	//Added Cybage AW 11-03-03 [
	//Reason : Bug 78025 In Bugzilla.
	else if(HIWORD(wParam)==EN_CHANGE && bOnchar)
	{
		switch(LOWORD(wParam))
		{
			case DIM_ALT_DIMALTD:
			case DIM_ALT_DIMALTF:
			case DIM_ALT_DIMALTTD:
			case DIM_ALT_DIMALTRND:
				UpdateEditBoxes(wParam, lParam);
				break;
			case DIM_ALT_DIMAPOST1:
			case DIM_ALT_DIMAPOST2:
				{
					struct svar_DlgItem *tmpDi=NULL;
					tmpDi=(struct svar_DlgItem *)GetWindowLong(HWND(lParam),GWL_USERDATA);
					if(tmpDi==NULL) break;
					svar_StrSet(tmpDi,HWND(lParam));
				}
				break;
		}
	}
	//Added Cybage AW 11-03-03]

	return(CDialog::OnCommand(wParam,lParam));
}


void svar_DIM_ALT::PaintPic(){
	OldBitmap=Bitmap;
	Bitmap=SDS_DimBitmap(320,300,7);
	m_CAltPic.SetBitmap((HBITMAP)Bitmap->GetSafeHandle());
	delete(OldBitmap);
}

void svar_DIM_ALT::DoDataExchange(CDataExchange *pDX) {
	DDX_Control(pDX,DIM_ALT_PIC,m_CAltPic);
}

BOOL svar_DIM_ALT::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_SETDIM);
	return(true);
}

void svar_DIM_ALT::OnSpinALTD(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,DIM_ALT_DIMALTD),pNMUpDown->iDelta);
	PaintPic();
	*result=0;
}

void svar_DIM_ALT::OnSpinALTF(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,DIM_ALT_DIMALTF),pNMUpDown->iDelta);
	PaintPic();
	*result=0;
}

void svar_DIM_ALT::OnSpinALTTD(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,DIM_ALT_DIMALTTD),pNMUpDown->iDelta);
	PaintPic();
	*result=0;
}

void svar_DIM_ALT::OnSpinALTRND(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,DIM_ALT_DIMALTRND),pNMUpDown->iDelta);
	PaintPic();
	*result=0;
}

void svar_DIM_ALT::OnOK() {
	HWND hParentWnd;

	svar_iHitEnter=1;
	hParentWnd=::GetParent(m_hWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void svar_DIM_ALT::OnCancel() {
	HWND hParentWnd;

	hParentWnd=::GetParent(m_hWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

BOOL svar_DIM_ALT::OnInitDialog() {

	//Added Cybage AW 11-03-03
	//Reason : Bug 78025 In Bugzilla.
	bOnchar = FALSE;

	FillCombo();
	svar_SubClassItems(this);
	CDialog::OnInitDialog();
	PaintPic();
	struct resbuf rb;
	sds_getvar("DIMALTZ",&rb);
	if(rb.resval.rint==0) {
		::SendMessage(::GetDlgItem(m_hWnd,DIM_ALT_DIMALTZ1),BM_SETCHECK,(WPARAM)0,0);
		::SendMessage(::GetDlgItem(m_hWnd,DIM_ALT_DIMALTZ2),BM_SETCHECK,(WPARAM)0,0);
	}else if(rb.resval.rint==4 || rb.resval.rint==5) {
		::SendMessage(::GetDlgItem(m_hWnd,DIM_ALT_DIMALTZ1),BM_SETCHECK,(WPARAM)1,0);
		::SendMessage(::GetDlgItem(m_hWnd,DIM_ALT_DIMALTZ2),BM_SETCHECK,(WPARAM)0,0);
	}else if(rb.resval.rint==8 || rb.resval.rint==10) {
		::SendMessage(::GetDlgItem(m_hWnd,DIM_ALT_DIMALTZ1),BM_SETCHECK,(WPARAM)0,0);
		::SendMessage(::GetDlgItem(m_hWnd,DIM_ALT_DIMALTZ2),BM_SETCHECK,(WPARAM)1,0);
	}else if(rb.resval.rint==12 || rb.resval.rint==15) {
		::SendMessage(::GetDlgItem(m_hWnd,DIM_ALT_DIMALTZ1),BM_SETCHECK,(WPARAM)1,0);
		::SendMessage(::GetDlgItem(m_hWnd,DIM_ALT_DIMALTZ2),BM_SETCHECK,(WPARAM)1,0);
	}
	sds_getvar("DIMALTTZ",&rb);
	if(rb.resval.rint==0) {
		::SendMessage(::GetDlgItem(m_hWnd,DIM_ALT_DIMALTTZ1),BM_SETCHECK,(WPARAM)0,0);
		::SendMessage(::GetDlgItem(m_hWnd,DIM_ALT_DIMALTTZ2),BM_SETCHECK,(WPARAM)0,0);
	}else if(rb.resval.rint==4 || rb.resval.rint==5) {
		::SendMessage(::GetDlgItem(m_hWnd,DIM_ALT_DIMALTTZ1),BM_SETCHECK,(WPARAM)1,0);
		::SendMessage(::GetDlgItem(m_hWnd,DIM_ALT_DIMALTTZ2),BM_SETCHECK,(WPARAM)0,0);
	}else if(rb.resval.rint==8 || rb.resval.rint==10) {
		::SendMessage(::GetDlgItem(m_hWnd,DIM_ALT_DIMALTTZ1),BM_SETCHECK,(WPARAM)0,0);
		::SendMessage(::GetDlgItem(m_hWnd,DIM_ALT_DIMALTTZ2),BM_SETCHECK,(WPARAM)1,0);
	}else if(rb.resval.rint==12 || rb.resval.rint==15) {
		::SendMessage(::GetDlgItem(m_hWnd,DIM_ALT_DIMALTTZ1),BM_SETCHECK,(WPARAM)1,0);
		::SendMessage(::GetDlgItem(m_hWnd,DIM_ALT_DIMALTTZ2),BM_SETCHECK,(WPARAM)1,0);
	}
	PaintPic();
	return(TRUE);
}

afx_msg void svar_DIM_ALT::OnPaint(void)
{
	CDialog::OnPaint();

	CWnd*		pDlgItemWnd;
	CRect		DlgItemRect;
	CClientDC	dc(this);
	CPen		*pOldPen,
				NewPen(PS_SOLID, 0, RGB(195, 50, 50));
	CBrush		*pOldBrush,
				NewBrush(RGB(195, 195, 195));

	if(!IsWindowVisible())
		return;

	pOldPen = dc.SelectObject(&NewPen);
	pOldBrush = dc.SelectObject(&NewBrush);

	for(int i=0;i<svar_num_of_vars;i++)
	{
		if( svar_DlgItemList[i].nGroup != DIM_ALT) continue;
		if(svar_DlgItemDims[i].isDimVarOverriden)
		{
			pDlgItemWnd = GetDlgItem(svar_DlgItemDims[i].nID);
			pDlgItemWnd->GetWindowRect(&DlgItemRect);
			ScreenToClient(&DlgItemRect);
			DlgItemRect.InflateRect(2, 1);
			dc.Rectangle(&DlgItemRect);
		}
	}

	dc.SelectObject(pOldPen);
	dc.SelectObject(pOldBrush);
}

void svar_DIM_ALT::FillCombo()
{
	CComboBox*	pCombo;

	if (NULL == (pCombo = (CComboBox*)GetDlgItem(DIM_ALT_DIMALTU)))
		return;

	pCombo->ResetContent();
	CString	comboOption;
	comboOption.LoadString(IDC_COMBO_DIM_ALT_DIMALTU_1);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_DIM_ALT_DIMALTU_2);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_DIM_ALT_DIMALTU_3);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_DIM_ALT_DIMALTU_4);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_DIM_ALT_DIMALTU_5);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_DIM_ALT_DIMALTU_6);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_DIM_ALT_DIMALTU_7);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_DIM_ALT_DIMALTU_8);
	pCombo->AddString(comboOption);
}

svar_DIM_ALT::svar_DIM_ALT():CDialog(DIM_ALT){
	Bitmap=OldBitmap=NULL;
}

//******This section will control the Dim Arrows property page

class svar_DIM_ARROWS : public CDialog {
	private:
		//Added Cybage AW 11-03-03
		//Reason : Bug 78025 In Bugzilla.
		BOOL bOnchar;

	public:
		~svar_DIM_ARROWS();
		svar_DIM_ARROWS();
		BOOL OnInitDialog();
		int iTicks;
		double dTickSize;
		void DimArrowsSetColor();
		void OnGrey();
		void TickSet();
		CComboBox m_StartCB,m_EndCB;
		//Modified Cybage AW 16-02-01
		CComboBox m_LdrCB;
		void PaintPic();
		CStatic m_CArrowsPic;
		CBitmap *Bitmap,*OldBitmap;
	protected:
		void OnTickClick();
		afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
		void OnLBDown(UINT nFlags,CPoint point);
		afx_msg void OnSpinTSZ(NMHDR *pNotifyStruct,LRESULT *result);
		afx_msg void OnSpinASZ(NMHDR *pNotifyStruct,LRESULT *result);
		afx_msg void OnLButtonDown(UINT nFlags,CPoint point);
		afx_msg void OnLButtonDblClk(UINT nFlags,CPoint point);
		//afx_msg void OnSah();
		afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
		virtual BOOL OnCommand(WPARAM wParam,LPARAM lParam);
		virtual void DoDataExchange(CDataExchange *pDX);
		virtual void OnCancel();
		virtual void OnOK();
		void		 FillCombo();
		// EBATECH(watanabe)-[dimldrblk
		////Modified Cybage AW 16-02-01 [
		//void		 InitialiseLeaderCB();
		bool setAllArrows(UINT nID);
		// ]-EBATECH(watanabe)
		afx_msg void OnPaint(void);

		//Added Cybage AW 11-03-03
		//Reason : Bug 78025 In Bugzilla.
		virtual BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(svar_DIM_ARROWS,CDialog)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_DIM_ARROWS_DIMTSZ,OnSpinTSZ)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_DIM_ARROWS_DIMASZ,OnSpinASZ)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SHOWWINDOW()
	//ON_BN_CLICKED(DIM_ARROWS_DIMSAH,OnSah)
	ON_WM_HELPINFO()
	ON_WM_PAINT()
END_MESSAGE_MAP()

//Added Cybage AW 11-03-03[
//Reason : Bug 78025 In Bugzilla.
BOOL svar_DIM_ARROWS::PreTranslateMessage(MSG* pMsg)
{
	bOnchar = FALSE;
	if(pMsg->message == WM_CHAR || pMsg->wParam == VK_DELETE)
		bOnchar = TRUE;
	return CDialog::PreTranslateMessage(pMsg);
}
//Added Cybage AW 11-03-03]

BOOL svar_DIM_ARROWS::OnCommand(WPARAM wParam,LPARAM lParam){

	if(HIWORD(wParam)==BN_CLICKED) {
		switch(LOWORD(wParam)) {
			case DIM_ARROWS_TICS:
				OnTickClick();
//			case DIM_ARROWS_DIMSAH:
//				PaintPic();
//				break;
		}
	}else if(HIWORD(wParam)==EN_KILLFOCUS) {
		switch(LOWORD(wParam)) {
			case DIM_ARROWS_DIMASZ:
			case DIM_ARROWS_DIMTSZ:
				PaintPic();
				break;
		}
	}else if(HIWORD(wParam)==CBN_SELCHANGE) {
		switch(LOWORD(wParam)) {
			case DIM_ARROWS_START:
			case DIM_ARROWS_END:
			//Modified Cybage AW 16-02-01
			case LDR_ARROWS:
				if(::SendMessage((HWND)lParam, CB_GETCURSEL, 0, 0) == USER_ARROWS_SEPARATOR_INDEX)
					::SendMessage((HWND)lParam, CB_SETCURSEL, (WPARAM)(USER_ARROWS_SEPARATOR_INDEX + 1), 0);
				PaintPic();
				break;
		}
	}else if(HIWORD(wParam)==CBN_EDITCHANGE) {
		switch(LOWORD(wParam)) {
			case DIM_ARROWS_START:
			case DIM_ARROWS_END:
			//Modified Cybage AW 16-02-01
			case LDR_ARROWS:
				PaintPic();
				break;
		}
	}
	//Added Cybage AW 11-03-03[
	//Reason : Bug 78025 In Bugzilla.
	else if(HIWORD(wParam)==EN_CHANGE && bOnchar)
	{
		switch(LOWORD(wParam))
		{
			case DIM_ARROWS_DIMASZ:
			case DIM_ARROWS_DIMTSZ:
				UpdateEditBoxes(wParam, lParam);
				break;
		}
	}
	//Added Cybage AW 11-03-03]

	return(CDialog::OnCommand(wParam,lParam));
}

void svar_DIM_ARROWS::PaintPic(){
	OldBitmap=Bitmap;
	Bitmap=SDS_DimBitmap(320,300,1);
	m_CArrowsPic.SetBitmap((HBITMAP)Bitmap->GetSafeHandle());
	delete(OldBitmap);
}

BOOL svar_DIM_ARROWS::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_SETDIM);
	return(true);
}

void svar_DIM_ARROWS::DoDataExchange(CDataExchange *pDX) {
	CString csTmp;

	DDX_Control(pDX,DIM_ARROWS_PIC,m_CArrowsPic);
	DDX_Control(pDX,DIM_ARROWS_START,m_StartCB);
	DDX_Control(pDX,DIM_ARROWS_END,m_EndCB);
	//Modified Cybage AW 16-02-01
	DDX_Control(pDX,LDR_ARROWS,m_LdrCB);
	DDX_Check(pDX,DIM_ARROWS_TICS,iTicks);
	//DDX_Check(pDX,DIM_ARROWS_DIMSAH,iSah);
	if(!pDX->m_bSaveAndValidate){
		if(sds_rtos(dTickSize,-1,-1,csTmp.GetBuffer(IC_ACADBUF))!=RTNORM) pDX->Fail();
		DDX_Text(pDX,DIM_ARROWS_DIMTSZ,csTmp);
	}else{
		DDX_Text(pDX,DIM_ARROWS_DIMTSZ,csTmp);
		if(sds_distof(csTmp,-1,&dTickSize)!=RTNORM) pDX->Fail();
	}
}

void svar_DIM_ARROWS::TickSet(){
	iTicks=0;
	UpdateData(false);
	OnTickClick();
}

void svar_DIM_ARROWS::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);
	if(bShow && !iTicks)
		GetDlgItem(DIM_ARROWS_START)->SetFocus();
}

void svar_DIM_ARROWS::OnTickClick(){
	struct resbuf rb;
	rb.restype=RTREAL;

	UpdateData(TRUE);
	if(iTicks==1) {
		//iSah=0;
		dTickSize=rb.resval.rreal=1.0;
	}else{
		dTickSize=rb.resval.rreal=0.0;
	}
	sds_setvar("DIMTSZ"/*DNT*/,&rb);
	UpdateData(FALSE);
	OnGrey();
}

void svar_DIM_ARROWS::OnGrey() {
	if(iTicks==0){
		::EnableWindow(::GetDlgItem(m_hWnd,DIM_ARROWS_DIMTSZ),FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd,UPDN_DIM_ARROWS_DIMTSZ),FALSE);
		//::EnableWindow(::GetDlgItem(m_hWnd,DIM_ARROWS_DIMSAH),TRUE);
		// EBATECH(watanabe)-[always enable
		::EnableWindow(::GetDlgItem(m_hWnd,DIM_ARROWS_DIMASZ),TRUE);	// dimasz
		::EnableWindow(::GetDlgItem(m_hWnd,UPDN_DIM_ARROWS_DIMASZ),TRUE);
		::EnableWindow(::GetDlgItem(m_hWnd,LDR_ARROWS),TRUE);			// dimldrblk
		// ]-EBATECH(watanabe)
		::EnableWindow(::GetDlgItem(m_hWnd,DIM_ARROWS_START),TRUE);
		::EnableWindow(::GetDlgItem(m_hWnd,DIM_ARROWS_END),TRUE);

	}else{
		//iArrowClick=0;
		::EnableWindow(::GetDlgItem(m_hWnd,DIM_ARROWS_DIMTSZ),TRUE);
		::EnableWindow(::GetDlgItem(m_hWnd,UPDN_DIM_ARROWS_DIMTSZ),TRUE);
		::EnableWindow(::GetDlgItem(m_hWnd,DIM_ARROWS_DIMSAH),FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd,DIM_ARROWS_START),FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd,DIM_ARROWS_END),FALSE);
		//Modified Cybage AW 16-02-01
		::EnableWindow(::GetDlgItem(m_hWnd,LDR_ARROWS),FALSE);

		::EnableWindow(::GetDlgItem(m_hWnd,DIM_ARROWS_DIMASZ),FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd,UPDN_DIM_ARROWS_DIMASZ),FALSE);
	}
}

void svar_DIM_ARROWS::OnLButtonDown(UINT nFlags,CPoint point) {
	OnLBDown(nFlags,point);
}

void svar_DIM_ARROWS::OnLButtonDblClk(UINT nFlags,CPoint point) {
	OnLBDown(nFlags,point);
}

void svar_DIM_ARROWS::OnLBDown(UINT nFlags,CPoint point) {
    RECT rect1,rect2,rect3;
	//Modified Cybage AW 16-02-01 
	RECT rect4;
	
    // EBATECH(watanabe)-[dimldrblk
    //if(iArrowClick==0) return;
    // ]-EBATECH(watanabe)
    ::GetWindowRect(m_hWnd,&rect1);
    ::GetWindowRect(::GetDlgItem(m_hWnd,DIM_ARROWS_PIC1),&rect2);
    ::GetWindowRect(::GetDlgItem(m_hWnd,DIM_ARROWS_PIC2),&rect3);
	//Modified Cybage AW 16-02-01 
	::GetWindowRect(::GetDlgItem(m_hWnd,LDR_ARROWS_PIC),&rect4);
	
    rect2.left-=rect1.left;
    rect2.top-=rect1.top;
    rect2.right-=rect1.left;
    rect2.bottom-=rect1.top;
    rect3.left-=rect1.left;
    rect3.top-=rect1.top;
    rect3.right-=rect1.left;
    rect3.bottom-=rect1.top;
	//Modified Cybage AW 16-02-01 [
	rect4.left-=rect1.left;
    rect4.top-=rect1.top;
    rect4.right-=rect1.left;
    rect4.bottom-=rect1.top;
	//Modified Cybage AW 16-02-01 ]
	
    if((point.x>rect2.left)&&(point.x<rect2.right)&&(point.y>rect2.top)&&(point.y<rect2.bottom)) {
        if(iTicks)
			return;
        if(m_StartCB.GetCurSel()==(m_StartCB.GetCount()-1)){
            m_StartCB.SetCurSel(0);
        }else{
            m_StartCB.SetCurSel(m_StartCB.GetCurSel()+1);
			if(m_StartCB.GetCurSel() == USER_ARROWS_SEPARATOR_INDEX)
	            m_StartCB.SetCurSel(USER_ARROWS_SEPARATOR_INDEX + 1);
        }
        ::SetFocus(::GetDlgItem(m_hWnd,DIM_ARROWS_START));
	m_StartCB.SendMessage(WM_LBUTTONUP);
        ::SendMessage(::GetDlgItem(m_hWnd,DIM_ARROWS_PIC1),WM_PAINT,0,0);
        PaintPic();
    }else if((point.x>rect3.left)&&(point.x<rect3.right)&&(point.y>rect3.top)&&(point.y<rect3.bottom)) {
        if(iTicks)
			return;
        if(m_EndCB.GetCurSel()==(m_EndCB.GetCount()-1)){
            m_EndCB.SetCurSel(0);
        }else{
		m_EndCB.SetCurSel(m_EndCB.GetCurSel()+1);
		if(m_EndCB.GetCurSel() == USER_ARROWS_SEPARATOR_INDEX)
	            m_EndCB.SetCurSel(USER_ARROWS_SEPARATOR_INDEX + 1);
        }
        ::SetFocus(::GetDlgItem(m_hWnd,DIM_ARROWS_END));
		m_EndCB.SendMessage(WM_LBUTTONUP);
        ::SendMessage(::GetDlgItem(m_hWnd,DIM_ARROWS_PIC2),WM_PAINT,0,0);
        PaintPic();
    }
	//Modified Cybage AW 16-02-01 [
	else if((point.x>rect4.left)&&(point.x<rect4.right)&&(point.y>rect4.top)&&(point.y<rect4.bottom)) {
        if(m_LdrCB.GetCurSel()==(m_LdrCB.GetCount()-1)){
            m_LdrCB.SetCurSel(0);
        }else{
		m_LdrCB.SetCurSel(m_LdrCB.GetCurSel()+1);
		if(m_LdrCB.GetCurSel() == USER_ARROWS_SEPARATOR_INDEX)
		m_LdrCB.SetCurSel(USER_ARROWS_SEPARATOR_INDEX + 1);
        }
        ::SetFocus(::GetDlgItem(m_hWnd,LDR_ARROWS));
		m_LdrCB.SendMessage(WM_LBUTTONUP);
        ::SendMessage(::GetDlgItem(m_hWnd,LDR_ARROWS_PIC),WM_PAINT,0,0);
        PaintPic();
    }
	//Modified Cybage AW 16-02-01 ]
}

void svar_DIM_ARROWS::OnSpinTSZ(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,DIM_ARROWS_DIMTSZ),pNMUpDown->iDelta);
	PaintPic();
	*result=0;
}

void svar_DIM_ARROWS::OnSpinASZ(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,DIM_ARROWS_DIMASZ),pNMUpDown->iDelta);
	PaintPic();
	*result=0;
}

void svar_DIM_ARROWS::OnOK() {
	HWND hParentWnd;

	svar_iHitEnter=1;
	hParentWnd=::GetParent(m_hWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void svar_DIM_ARROWS::OnCancel() {
	HWND hParentWnd;

	hParentWnd=::GetParent(m_hWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void svar_DIM_ARROWS::DimArrowsSetColor() {
	struct resbuf rb;
	int iTmp;

	SDS_getvar(NULL,DB_QDIMCLRD,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	SDS_GetColorDialog(rb.resval.rint,&iTmp,0);
	rb.resval.rint=iTmp;
	sds_setvar("DIMCLRD"/*DNT*/,&rb);
	PaintPic();
}

BOOL svar_DIM_ARROWS::OnInitDialog() {
	//Added Cybage AW 11-03-03
	//Reason : Bug 78025 In Bugzilla.
	bOnchar = FALSE;

	FillCombo();
	// EBATECH(watanabe)-[dimldrblk
	////Modified Cybage AW 16-02-01
	//InitialiseLeaderCB();
	// ]-EBATECH(watanabe)
	svar_SubClassItems(this);
	CDialog::OnInitDialog();
	OnGrey();
	PaintPic();
	return(TRUE);
}

// EBATECH(watanabe)-[dimldrblk
////Modified Cybage AW 16-02-01 [
////Reason : Initializes the Leader styles strings in the Leader CB
//void svar_DIM_ARROWS::InitialiseLeaderCB()
//{
//	  CComboBox *pCombo = (CComboBox *)GetDlgItem(LDR_ARROWS);
//	  if(!pCombo)
//		  return;
//
//	  char *chBlkName = new char[512];
//	  sds_name ename;
//	  ename[1] = (long)SDS_CURDWG;
//	  ((db_drawing*)ename[1])->get_dimldrblk(chBlkName);
//
//	  if(strisame(ResourceString(IDC_ICADSETVARSDIA_NONE_162, "NONE" ),chBlkName) ||
//		 strisame("_NONE",chBlkName) ) {
//		  pCombo->SendMessage(CB_SETCURSEL,(WPARAM)0,0);
//	  }else if(strisame(ResourceString(IDC_ICADSETVARSDIA_CLOSED_163, "CLOSED" ),chBlkName) ||
//		 strisame("_CLOSED",chBlkName) ) {
//		  pCombo->SendMessage(CB_SETCURSEL,(WPARAM)1,0);
//	  }else if(strisame(ResourceString(IDC_ICADSETVARSDIA_DOT_164, "DOT" ),chBlkName) ||
//		 strisame("_DOT",chBlkName) ) {
//		  pCombo->SendMessage(CB_SETCURSEL,(WPARAM)2,0);
//	  }else if(strisame("",chBlkName)) {
//		  pCombo->SendMessage(CB_SETCURSEL,(WPARAM)3,0);
//	  }else if(strisame(ResourceString(IDC_ICADSETVARSDIA_CLOSEDB_165, "CLOSEDBLANK" ),chBlkName) ||
//		 strisame("_CLOSEDBLANK",chBlkName) ) {
//		  pCombo->SendMessage(CB_SETCURSEL,(WPARAM)4,0);
//	  }else if(strisame(ResourceString(IDC_ICADSETVARSDIA_OBLIQUE_166, "OBLIQUE" ),chBlkName) ||
//		 strisame("_OBLIQUE",chBlkName) ) {
//		  pCombo->SendMessage(CB_SETCURSEL,(WPARAM)5,0);
//	  }else if(strisame(ResourceString(IDC_ICADSETVARSDIA_ARCHTIC_167, "ARCHTICK" ),chBlkName) ||
//		 strisame("_ARCHTICK",chBlkName) ) {
//		  pCombo->SendMessage(CB_SETCURSEL,(WPARAM)6,0);
//	  }else if(strisame(ResourceString(IDC_ICADSETVARSDIA_OPEN_168, "OPEN" ),chBlkName) ||
//		 strisame("_OPEN",chBlkName) ) {
//		  pCombo->SendMessage(CB_SETCURSEL,(WPARAM)7,0);
//	  }else if(strisame(ResourceString(IDC_ICADSETVARSDIA_ORIGIN_169, "ORIGIN" ),chBlkName) ||
//		 strisame("_ORIGIN",chBlkName) ) {
//		  pCombo->SendMessage(CB_SETCURSEL,(WPARAM)8,0);
//	  }else if(strisame(ResourceString(IDC_ICADSETVARSDIA_OPEN90_170, "OPEN90" ),chBlkName) ||
//		 strisame("_OPEN90",chBlkName) ) {
//		  pCombo->SendMessage(CB_SETCURSEL,(WPARAM)9,0);
//	  }else if(strisame(ResourceString(IDC_ICADSETVARSDIA_OPEN30_171, "OPEN30" ),chBlkName) ||
//		 strisame("_OPEN30",chBlkName) ) {
//		  pCombo->SendMessage(CB_SETCURSEL,(WPARAM)10,0);
//	  }else if(strisame(ResourceString(IDC_ICADSETVARSDIA_DOTBLAN_172, "DOTBLANK" ),chBlkName) ||
//		 strisame("_DOTBLANK",chBlkName) ) {
//		  pCombo->SendMessage(CB_SETCURSEL,(WPARAM)11,0);
//	  }else if(strisame(ResourceString(IDC_ICADSETVARSDIA_DOTSMAL_173, "DOTSMALL" ),chBlkName) ||
//		 strisame("_DOTSMALL",chBlkName) ) {
//		  pCombo->SendMessage(CB_SETCURSEL,(WPARAM)12,0);
//	  }else if(strisame(ResourceString(IDC_ICADSETVARSDIA_BOXFILL_174, "BOXFILLED" ),chBlkName) ||
//		 strisame("_BOXFILLED",chBlkName) ) {
//		  pCombo->SendMessage(CB_SETCURSEL,(WPARAM)13,0);
//	  }else if(strisame(ResourceString(IDC_ICADSETVARSDIA_BOXBLAN_175, "BOXBLANK" ),chBlkName) ||
//		 strisame("_BOXBLANK",chBlkName) ) {
//		  pCombo->SendMessage(CB_SETCURSEL,(WPARAM)14,0);
//	  }else if(strisame(ResourceString(IDC_ICADSETVARSDIA_DATUMFI_176, "DATUMFILLED" ),chBlkName) ||
//		 strisame("_DATUMFILLED",chBlkName) ) {
//		  pCombo->SendMessage(CB_SETCURSEL,(WPARAM)15,0);
//	  }else if(strisame(ResourceString(IDC_ICADSETVARSDIA_DATUMBL_177, "DATUMBLANK" ),chBlkName) ||
//		 strisame("_DATUMBLANK",chBlkName) ) {
//		  pCombo->SendMessage(CB_SETCURSEL,(WPARAM)16,0);
//	  }else if(strisame(ResourceString(IDC_ICADSETVARSDIA_INTEGRA_178, "INTEGRAL" ),chBlkName) ||
//		 strisame("_INTEGRAL",chBlkName) ) {
//		  pCombo->SendMessage(CB_SETCURSEL,(WPARAM)17,0);
//	  }else{
//		  if (chBlkName[0] == '_')
//			  pCombo->SetWindowText(chBlkName + sizeof(TCHAR));
//		  else
//			  pCombo->SetWindowText(chBlkName);
//	  }
//	HWND hItemWnd= ::GetDlgItem(m_hWnd,LDR_ARROWS_PIC);
//	::SendMessage(hItemWnd,WM_PAINT,0,0);
//
//	delete [] chBlkName;	/*D.G.*/// !!!
//}
////Modified Cybage AW 16-02-01 ]
// ]-EBATECH(watanabe)

// EBATECH(watanabe)-[dimldrblk
bool svar_DIM_ARROWS::setAllArrows(UINT nID)
{
	CComboBox *pCombo = (CComboBox*)GetDlgItem(nID);
	if (NULL == pCombo) return false;

	pCombo->ResetContent();
	CString comboOption;
	comboOption.LoadString(IDC_COMBO_DIM_ARROWS_START_1);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_DIM_ARROWS_START_2);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_DIM_ARROWS_START_3);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_DIM_ARROWS_START_4);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_DIM_ARROWS_START_5);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_DIM_ARROWS_START_6);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_DIM_ARROWS_START_7);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_DIM_ARROWS_START_8);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_DIM_ARROWS_START_9);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_DIM_ARROWS_START_10);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_DIM_ARROWS_START_11);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_DIM_ARROWS_START_12);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_DIM_ARROWS_START_13);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_DIM_ARROWS_START_14);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_DIM_ARROWS_START_15);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_DIM_ARROWS_START_16);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_DIM_ARROWS_START_17);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_DIM_ARROWS_START_18);
	pCombo->AddString(comboOption);
	db_drawing*	pDrw = SDS_CURDWG;
	db_handitem	*pFirstTabrec = NULL, *pLastTabrec = NULL;
	pDrw->get_tabrecllends(DB_BLOCKTABREC, &pFirstTabrec, &pLastTabrec);
	if(pFirstTabrec && pLastTabrec)
	{
		bool	bAddedSeparator = false;
		for(db_handitem* pCurTabrec = pFirstTabrec; pCurTabrec; pCurTabrec = pCurTabrec->next)
			if( ((db_blocktabrec*)pCurTabrec)->retp_name() &&
				*((db_blocktabrec*)pCurTabrec)->retp_name() != '*'/*DNT*/ &&	// don't allow anonymous blocks in the dialog
				!IsInternalArrow(((db_blocktabrec*)pCurTabrec)->retp_name()) )
			{
				if(!bAddedSeparator)
				{
					pCombo->AddString(ResourceString(IDC_ICADSETVARSDIA_USER_ARROW, "----- User Arrow(s) -----"));
					bAddedSeparator = true;
				}
				pCombo->AddString(((db_blocktabrec*)pCurTabrec)->retp_name());
			}
	}
	return true;
}
// ]-EBATECH(watanabe)

afx_msg void svar_DIM_ARROWS::OnPaint(void)
{
	CDialog::OnPaint();

	CWnd*		pDlgItemWnd;
	CRect		DlgItemRect;
	CClientDC	dc(this);
	CPen		*pOldPen,
				NewPen(PS_SOLID, 0, RGB(195, 50, 50));
	CBrush		*pOldBrush,
				NewBrush(RGB(195, 195, 195));

	if(!IsWindowVisible())
		return;

	pOldPen = dc.SelectObject(&NewPen);
	pOldBrush = dc.SelectObject(&NewBrush);

	for(int i=0;i<svar_num_of_vars;i++)
	{
		if( svar_DlgItemList[i].nGroup != DIM_ARROWS) continue;
		if(svar_DlgItemDims[i].isDimVarOverriden)
		{
			pDlgItemWnd = GetDlgItem(svar_DlgItemDims[i].nID);
			pDlgItemWnd->GetWindowRect(&DlgItemRect);
			ScreenToClient(&DlgItemRect);
			DlgItemRect.InflateRect(2, 1);
			dc.Rectangle(&DlgItemRect);
		}
	}

	dc.SelectObject(pOldPen);
	dc.SelectObject(pOldBrush);
}

void svar_DIM_ARROWS::FillCombo()
{
	// EBATECH(watanabe)-[dimldrblk
	//CComboBox *pCombo = NULL;
	//
	//if (NULL == (pCombo = (CComboBox *) GetDlgItem(DIM_ARROWS_START)))
	//	  return;
	//
	//pCombo->ResetContent();
	//CString comboOption;
	//comboOption.LoadString(IDC_COMBO_DIM_ARROWS_START_1);
	//pCombo->AddString(comboOption);
	//comboOption.LoadString(IDC_COMBO_DIM_ARROWS_START_2);
	//pCombo->AddString(comboOption);
	//comboOption.LoadString(IDC_COMBO_DIM_ARROWS_START_3);
	//pCombo->AddString(comboOption);
	//comboOption.LoadString(IDC_COMBO_DIM_ARROWS_START_4);
	//pCombo->AddString(comboOption);
	//comboOption.LoadString(IDC_COMBO_DIM_ARROWS_START_5);
	//pCombo->AddString(comboOption);
	//comboOption.LoadString(IDC_COMBO_DIM_ARROWS_START_6);
	//pCombo->AddString(comboOption);
	//comboOption.LoadString(IDC_COMBO_DIM_ARROWS_START_7);
	//pCombo->AddString(comboOption);
	//comboOption.LoadString(IDC_COMBO_DIM_ARROWS_START_8);
	//pCombo->AddString(comboOption);
	//comboOption.LoadString(IDC_COMBO_DIM_ARROWS_START_9);
	//pCombo->AddString(comboOption);
	//comboOption.LoadString(IDC_COMBO_DIM_ARROWS_START_10);
	//pCombo->AddString(comboOption);
	//comboOption.LoadString(IDC_COMBO_DIM_ARROWS_START_11);
	//pCombo->AddString(comboOption);
	//comboOption.LoadString(IDC_COMBO_DIM_ARROWS_START_12);
	//pCombo->AddString(comboOption);
	//comboOption.LoadString(IDC_COMBO_DIM_ARROWS_START_13);
	//pCombo->AddString(comboOption);
	//comboOption.LoadString(IDC_COMBO_DIM_ARROWS_START_14);
	//pCombo->AddString(comboOption);
	//comboOption.LoadString(IDC_COMBO_DIM_ARROWS_START_15);
	//pCombo->AddString(comboOption);
	//comboOption.LoadString(IDC_COMBO_DIM_ARROWS_START_16);
	//pCombo->AddString(comboOption);
	//comboOption.LoadString(IDC_COMBO_DIM_ARROWS_START_17);
	//pCombo->AddString(comboOption);
	//comboOption.LoadString(IDC_COMBO_DIM_ARROWS_START_18);
	//pCombo->AddString(comboOption);
	//
	//if (NULL == (pCombo = (CComboBox *) GetDlgItem(DIM_ARROWS_END)))
	//	  return;
	//
	//pCombo->ResetContent();
	//comboOption.LoadString(IDC_COMBO_DIM_ARROWS_END_1);
	//pCombo->AddString(comboOption);
	//comboOption.LoadString(IDC_COMBO_DIM_ARROWS_END_2);
	//pCombo->AddString(comboOption);
	//comboOption.LoadString(IDC_COMBO_DIM_ARROWS_END_3);
	//pCombo->AddString(comboOption);
	//comboOption.LoadString(IDC_COMBO_DIM_ARROWS_END_4);
	//pCombo->AddString(comboOption);
	//comboOption.LoadString(IDC_COMBO_DIM_ARROWS_END_5);
	//pCombo->AddString(comboOption);
	//comboOption.LoadString(IDC_COMBO_DIM_ARROWS_END_6);
	//pCombo->AddString(comboOption);
	//comboOption.LoadString(IDC_COMBO_DIM_ARROWS_END_7);
	//pCombo->AddString(comboOption);
	//comboOption.LoadString(IDC_COMBO_DIM_ARROWS_END_8);
	//pCombo->AddString(comboOption);
	//comboOption.LoadString(IDC_COMBO_DIM_ARROWS_END_9);
	//pCombo->AddString(comboOption);
	//comboOption.LoadString(IDC_COMBO_DIM_ARROWS_END_10);
	//pCombo->AddString(comboOption);
	//comboOption.LoadString(IDC_COMBO_DIM_ARROWS_END_11);
	//pCombo->AddString(comboOption);
	//comboOption.LoadString(IDC_COMBO_DIM_ARROWS_END_12);
	//pCombo->AddString(comboOption);
	//comboOption.LoadString(IDC_COMBO_DIM_ARROWS_END_13);
	//pCombo->AddString(comboOption);
	//comboOption.LoadString(IDC_COMBO_DIM_ARROWS_END_14);
	//pCombo->AddString(comboOption);
	//comboOption.LoadString(IDC_COMBO_DIM_ARROWS_END_15);
	//pCombo->AddString(comboOption);
	//comboOption.LoadString(IDC_COMBO_DIM_ARROWS_END_16);
	//pCombo->AddString(comboOption);
	//comboOption.LoadString(IDC_COMBO_DIM_ARROWS_END_17);
	//pCombo->AddString(comboOption);
	//comboOption.LoadString(IDC_COMBO_DIM_ARROWS_END_18);
	//pCombo->AddString(comboOption);
	//
	////Modified Cybage AW 16-02-01
	//if (NULL == (pCombo = (CComboBox *) GetDlgItem(LDR_ARROWS)))
	//	  return;
	//
	//pCombo->ResetContent();
	//comboOption.LoadString(IDC_COMBO_LDR_ARROWS_1);
	//pCombo->AddString(comboOption);
	//comboOption.LoadString(IDC_COMBO_LDR_ARROWS_2);
	//pCombo->AddString(comboOption);
	//comboOption.LoadString(IDC_COMBO_LDR_ARROWS_3);
	//pCombo->AddString(comboOption);
	//comboOption.LoadString(IDC_COMBO_LDR_ARROWS_4);
	//pCombo->AddString(comboOption);
	//comboOption.LoadString(IDC_COMBO_LDR_ARROWS_5);
	//pCombo->AddString(comboOption);
	//comboOption.LoadString(IDC_COMBO_LDR_ARROWS_6);
	//pCombo->AddString(comboOption);
	//comboOption.LoadString(IDC_COMBO_LDR_ARROWS_7);
	//pCombo->AddString(comboOption);
	//comboOption.LoadString(IDC_COMBO_LDR_ARROWS_8);
	//pCombo->AddString(comboOption);
	//comboOption.LoadString(IDC_COMBO_LDR_ARROWS_9);
	//pCombo->AddString(comboOption);
	//comboOption.LoadString(IDC_COMBO_LDR_ARROWS_10);
	//pCombo->AddString(comboOption);
	//comboOption.LoadString(IDC_COMBO_LDR_ARROWS_11);
	//pCombo->AddString(comboOption);
	//comboOption.LoadString(IDC_COMBO_LDR_ARROWS_12);
	//pCombo->AddString(comboOption);
	//comboOption.LoadString(IDC_COMBO_LDR_ARROWS_13);
	//pCombo->AddString(comboOption);
	//comboOption.LoadString(IDC_COMBO_LDR_ARROWS_14);
	//pCombo->AddString(comboOption);
	//comboOption.LoadString(IDC_COMBO_LDR_ARROWS_15);
	//pCombo->AddString(comboOption);
	//comboOption.LoadString(IDC_COMBO_LDR_ARROWS_16);
	//pCombo->AddString(comboOption);
	//comboOption.LoadString(IDC_COMBO_LDR_ARROWS_17);
	//pCombo->AddString(comboOption);
	//comboOption.LoadString(IDC_COMBO_LDR_ARROWS_18);
	//pCombo->AddString(comboOption);
	////]
	setAllArrows(DIM_ARROWS_START);
	setAllArrows(DIM_ARROWS_END);
	setAllArrows(LDR_ARROWS);
	// ]-EBATECH(watanabe)
}

svar_DIM_ARROWS::svar_DIM_ARROWS():CDialog(DIM_ARROWS) {
	struct resbuf rb;

	Bitmap=OldBitmap=NULL;

	SDS_getvar(NULL,DB_QDIMTSZ,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if(rb.resval.rreal){
		iTicks=1;
	}else{
		iTicks=0;
	}
	dTickSize=0.0;
}

svar_DIM_ARROWS::~svar_DIM_ARROWS() {
}

//******This section will control the Dim Format property page

class svar_DIM_FORMAT : public CDialog {
	private:
		//Added Cybage AW 11-03-03
		//Reason : Bug 78025 In Bugzilla.
		BOOL bOnchar;

	public:
		svar_DIM_FORMAT();
		BOOL OnInitDialog();
		void PaintPic();
		CStatic m_CFormatPic;
		CBitmap *Bitmap,*OldBitmap;
	protected:
		afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
		afx_msg void OnSpinGAP(NMHDR *pNotifyStruct,LRESULT *result);
		virtual void DoDataExchange(CDataExchange *pDX);
		virtual void OnOK();
		virtual void OnCancel();
		virtual BOOL OnCommand(WPARAM wParam,LPARAM lParam);
		afx_msg void OnPaint(void);

		//Added Cybage AW 11-03-03
		//Reason : Bug 78025 In Bugzilla.
		virtual BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(svar_DIM_FORMAT,CDialog)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_DIM_ARROWS_DIMGAP,OnSpinGAP)
	ON_WM_HELPINFO()
	ON_WM_PAINT()
END_MESSAGE_MAP()

//Added Cybage AW 11-03-03[
//Reason : Bug 78025 In Bugzilla.
BOOL svar_DIM_FORMAT::PreTranslateMessage(MSG* pMsg)
{
	bOnchar = FALSE;
	if(pMsg->message == WM_CHAR  || pMsg->wParam == VK_DELETE)
		bOnchar = TRUE;
	return CDialog::PreTranslateMessage(pMsg);
}
//Added Cybage AW 11-03-03]

void svar_DIM_FORMAT::DoDataExchange(CDataExchange *pDX){
	DDX_Control(pDX,DIM_FORMAT_PIC,m_CFormatPic);
}

BOOL svar_DIM_FORMAT::OnCommand(WPARAM wParam,LPARAM lParam){

	if(HIWORD(wParam)==BN_CLICKED) {
		switch(LOWORD(wParam)) {
			case DIM_FORMAT_DIMFIT0:
			case DIM_FORMAT_DIMFIT1:
			case DIM_FORMAT_DIMFIT2:
			case DIM_FORMAT_DIMFIT3:
			case DIM_FORMAT_DIMFIT4:
			case DIM_FORMAT_DIMFIT5:
				PaintPic();
				break;
		}
	}else if(HIWORD(wParam)==EN_KILLFOCUS) {
		switch(LOWORD(wParam)) {
			case DIM_FORMAT_DIMGAP:
				PaintPic();
				break;
		}
	}

	//Added Cybage AW 11-03-03[
	//Reason : Bug 78025 In Bugzilla.
	else if(HIWORD(wParam)==EN_CHANGE && bOnchar)
	{
		switch(LOWORD(wParam))
		{
			case DIM_FORMAT_DIMGAP:
				UpdateEditBoxes(wParam, lParam);
				break;
		}
	}
	//Added Cybage AW 11-03-03]

	return(CDialog::OnCommand(wParam,lParam));
}

void svar_DIM_FORMAT::PaintPic(){
	OldBitmap=Bitmap;
	Bitmap=SDS_DimBitmap(320,300,2);
	m_CFormatPic.SetBitmap((HBITMAP)Bitmap->GetSafeHandle());
	delete(OldBitmap);
}

BOOL svar_DIM_FORMAT::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_SETDIM);
	return(true);
}

void svar_DIM_FORMAT::OnSpinGAP(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,DIM_FORMAT_DIMGAP),pNMUpDown->iDelta);
	PaintPic();
	*result=0;
}

void svar_DIM_FORMAT::OnOK() {
	HWND hParentWnd;

	svar_iHitEnter=1;
	hParentWnd=::GetParent(m_hWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void svar_DIM_FORMAT::OnCancel() {
	HWND hParentWnd;

	hParentWnd=::GetParent(m_hWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

BOOL svar_DIM_FORMAT::OnInitDialog() {
	//Added Cybage AW 11-03-03
	//Reason : Bug 78025 In Bugzilla.
	bOnchar = FALSE;

	svar_SubClassItems(this);
	CDialog::OnInitDialog();
	PaintPic();
	return(TRUE);
}

afx_msg void svar_DIM_FORMAT::OnPaint(void)
{
	CDialog::OnPaint();

	CWnd*		pDlgItemWnd;
	CRect		DlgItemRect;
	CClientDC	dc(this);
	CPen		*pOldPen,
				NewPen(PS_SOLID, 0, RGB(195, 50, 50));
	CBrush		*pOldBrush,
				NewBrush(RGB(195, 195, 195));

	if(!IsWindowVisible())
		return;

	pOldPen = dc.SelectObject(&NewPen);
	pOldBrush = dc.SelectObject(&NewBrush);

	for(int i=0;i<svar_num_of_vars;i++)
	{
		if( svar_DlgItemList[i].nGroup != DIM_FORMAT) continue;
		if(svar_DlgItemDims[i].isDimVarOverriden)
		{
			pDlgItemWnd = GetDlgItem(svar_DlgItemDims[i].nID);
			pDlgItemWnd->GetWindowRect(&DlgItemRect);
			ScreenToClient(&DlgItemRect);
			DlgItemRect.InflateRect(2, 1);
			dc.Rectangle(&DlgItemRect);
		}
	}
	dc.SelectObject(pOldPen);
	dc.SelectObject(pOldBrush);
}

svar_DIM_FORMAT::svar_DIM_FORMAT():CDialog(DIM_FORMAT){
	Bitmap=OldBitmap=NULL;
}

//******This section will control the Dim Units property page

class svar_DIM_LINES : public CDialog {
	private:
		//Added Cybage AW 11-03-03
		//Reason : Bug 78025 In Bugzilla.
		BOOL bOnchar;

	public:
		svar_DIM_LINES();
		BOOL OnInitDialog();
		void OnCLRD();
		void OnCLRE();
		void PaintPic();
		void FillCombo();
		CStatic m_CLinesPic;
		CBitmap *Bitmap,*OldBitmap;
	protected:
		afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
		virtual void OnOK();
		virtual void OnCancel();
		afx_msg void OnSpinCEN(NMHDR *pNotifyStruct,LRESULT *result);
		afx_msg void OnSpinDLE(NMHDR *pNotifyStruct,LRESULT *result);
		afx_msg void OnSpinDLI(NMHDR *pNotifyStruct,LRESULT *result);
		afx_msg void OnSpinEXO(NMHDR *pNotifyStruct,LRESULT *result);
		afx_msg void OnSpinEXE(NMHDR *pNotifyStruct,LRESULT *result);
		virtual BOOL OnCommand(WPARAM wParam,LPARAM lParam);
		virtual void DoDataExchange(CDataExchange *pDX);
		afx_msg void OnPaint(void);

		//Added Cybage AW 11-03-03
		//Reason : Bug 78025 In Bugzilla.
		virtual BOOL PreTranslateMessage( MSG* pMsg );

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(svar_DIM_LINES,CDialog)
	ON_COMMAND(DIM_LINES_COLCLRD,OnCLRD)
	ON_COMMAND(DIM_LINES_COLCLRE,OnCLRE)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_DIM_LINES_DIMCEN,OnSpinCEN)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_DIM_LINES_DIMDLE,OnSpinDLE)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_DIM_LINES_DIMDLI,OnSpinDLI)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_DIM_LINES_DIMEXO,OnSpinEXO)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_DIM_LINES_DIMEXE,OnSpinEXE)
	ON_WM_HELPINFO()
	ON_WM_PAINT()
END_MESSAGE_MAP()

//Added Cybage AW 11-03-03[
//Reason : Bug 78025 In Bugzilla.
BOOL svar_DIM_LINES::PreTranslateMessage(MSG* pMsg)
{
	bOnchar = FALSE;
	if(pMsg->message == WM_CHAR  || pMsg->wParam == VK_DELETE)
		bOnchar = TRUE;
	return CDialog::PreTranslateMessage(pMsg);
}
//Added Cybage AW 11-03-03]

void svar_DIM_LINES::DoDataExchange(CDataExchange *pDX){
	DDX_Control(pDX,DIM_LINES_PIC,m_CLinesPic);
}

BOOL svar_DIM_LINES::OnCommand(WPARAM wParam,LPARAM lParam){

	if(HIWORD(wParam)==BN_CLICKED) {
		switch(LOWORD(wParam)) {
			case DIM_LINES_DIMSD1:
			case DIM_LINES_DIMSD2:
			case DIM_LINES_DIMSE1:
			case DIM_LINES_DIMSE2:
			case DIM_LINES_DIMSOXD:
			case DIM_LINES_DIMTOFL:
				PaintPic();
				break;
		}
	}else if(HIWORD(wParam)==EN_KILLFOCUS) {
		switch(LOWORD(wParam)) {
			case DIM_LINES_DIMCEN:
			case DIM_LINES_DIMDLI:
			case DIM_LINES_DIMEXO:
			case DIM_LINES_DIMDLE:
			case DIM_LINES_DIMEXE:
				PaintPic();
				break;
		}
	}else if(HIWORD(wParam)==LBN_SELCHANGE) {
		switch(LOWORD(wParam)) {
			case DIM_LINES_DIMLWD:
			case DIM_LINES_DIMLWE:
				PaintPic();
				break;
		}
	}
	//Added Cybage AW 11-03-03[
	//Reason : Bug 78025 In Bugzilla.
	else if(HIWORD(wParam)==EN_CHANGE && bOnchar)
	{
		switch(LOWORD(wParam))
		{
			case DIM_LINES_DIMCEN:
			case DIM_LINES_DIMDLI:
			case DIM_LINES_DIMEXO:
			case DIM_LINES_DIMDLE:
			case DIM_LINES_DIMEXE:
				UpdateEditBoxes(wParam, lParam);
				break;
		}
	}
	//Added Cybage AW 11-03-03]

	return(CDialog::OnCommand(wParam,lParam));
}

void svar_DIM_LINES::PaintPic(){
	OldBitmap=Bitmap;
	Bitmap=SDS_DimBitmap(320,300,3);
	m_CLinesPic.SetBitmap((HBITMAP)Bitmap->GetSafeHandle());
	delete(OldBitmap);
}

BOOL svar_DIM_LINES::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_SETDIM);
	return(true);
}

void svar_DIM_LINES::OnSpinCEN(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,DIM_LINES_DIMCEN),pNMUpDown->iDelta);
	PaintPic();
	*result=0;
}

void svar_DIM_LINES::OnSpinDLE(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,DIM_LINES_DIMDLE),pNMUpDown->iDelta);
	PaintPic();
	*result=0;
}

void svar_DIM_LINES::OnSpinDLI(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,DIM_LINES_DIMDLI),pNMUpDown->iDelta);
	PaintPic();
	*result=0;
}

void svar_DIM_LINES::OnSpinEXO(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,DIM_LINES_DIMEXO),pNMUpDown->iDelta);
	PaintPic();
	*result=0;
}

void svar_DIM_LINES::OnSpinEXE(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,DIM_LINES_DIMEXE),pNMUpDown->iDelta);
	PaintPic();
	*result=0;
}

//***This will call the color dialog box
//	 and set the color variable
void svar_DIM_LINES::OnCLRD() {
	struct resbuf rb;
	int iTmp;

	SDS_getvar(NULL,DB_QDIMCLRD,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	SDS_GetColorDialog(rb.resval.rint,&iTmp,0);
	rb.resval.rint=iTmp;
	sds_setvar("DIMCLRD"/*DNT*/,&rb);
	PaintPic();
}

//***This will call the color dialog box
//	 and set the color variable
void svar_DIM_LINES::OnCLRE() {
	struct resbuf rb;
	int iTmp;

	SDS_getvar(NULL,DB_QDIMCLRE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	SDS_GetColorDialog(rb.resval.rint,&iTmp,0);
	rb.resval.rint=iTmp;
	sds_setvar("DIMCLRE"/*DNT*/,&rb);
	PaintPic();
}

void svar_DIM_LINES::OnOK() {
	HWND hParentWnd;

	svar_iHitEnter=1;
	hParentWnd=::GetParent(m_hWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void svar_DIM_LINES::OnCancel() {
	HWND hParentWnd;

	hParentWnd=::GetParent(m_hWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void svar_DIM_LINES::FillCombo()
{
	CIcadLWComboBox *pCombo = NULL;
	resbuf rb;
	SDS_getvar(NULL,DB_QLWUNITS,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

	if (NULL == (pCombo = (CIcadLWComboBox *) GetDlgItem(DIM_LINES_DIMLWD)))
		return;

	pCombo->FillLWeight(rb.resval.rint, 0, ( DB_BLWMASK | DB_BBWMASK | DB_DFWMASK ));

	if (NULL == (pCombo = (CIcadLWComboBox *) GetDlgItem(DIM_LINES_DIMLWE)))
		return;

	pCombo->FillLWeight(rb.resval.rint, 0, ( DB_BLWMASK | DB_BBWMASK | DB_DFWMASK ));
}



BOOL svar_DIM_LINES::OnInitDialog() {
	//Added Cybage AW 11-03-03
	//Reason : Bug 78025 In Bugzilla.
	bOnchar = FALSE;
	FillCombo();
	svar_SubClassItems(this);
	CDialog::OnInitDialog();
	PaintPic();
	return(TRUE);
}

afx_msg void svar_DIM_LINES::OnPaint(void)
{
	CDialog::OnPaint();

	CWnd*		pDlgItemWnd;
	CRect		DlgItemRect;
	CClientDC	dc(this);
	CPen		*pOldPen,
				NewPen(PS_SOLID, 0, RGB(195, 50, 50));
	CBrush		*pOldBrush,
				NewBrush(RGB(195, 195, 195));

	if(!IsWindowVisible())
		return;

	pOldPen = dc.SelectObject(&NewPen);
	pOldBrush = dc.SelectObject(&NewBrush);

	for(int i=0;i<svar_num_of_vars;i++)
	{
		if( svar_DlgItemList[i].nGroup != DIM_LINES) continue;
		if(svar_DlgItemDims[i].isDimVarOverriden)
		{
			pDlgItemWnd = GetDlgItem(svar_DlgItemDims[i].nID);
			pDlgItemWnd->GetWindowRect(&DlgItemRect);
			ScreenToClient(&DlgItemRect);
			DlgItemRect.InflateRect(2, 1);
			dc.Rectangle(&DlgItemRect);
		}
	}
	dc.SelectObject(pOldPen);
	dc.SelectObject(pOldBrush);
}

svar_DIM_LINES::svar_DIM_LINES():CDialog(DIM_LINES){
	Bitmap=OldBitmap=NULL;
}

//******This section will control the Dim Text property page

class svar_DIM_TEXT : public CDialog {
	private:
		//Added Cybage AW 11-03-03
		//Reason : Bug 78025 In Bugzilla.
		BOOL bOnchar;

	public:
		svar_DIM_TEXT();
		BOOL OnInitDialog();
		void DimTextSetColor();
		CComboBox m_CTextStyle;
		void PaintPic();
		void OnGrey();
		CStatic m_CTextPic;
		CBitmap *Bitmap,*OldBitmap;
	protected:
		int iSS;
		void OnSuperscriptClick();

		afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
		afx_msg void ReadStyles();
		afx_msg void OnSpinTXT(NMHDR *pNotifyStruct,LRESULT *result);
		afx_msg void OnSpinTVP(NMHDR *pNotifyStruct,LRESULT *result);
		afx_msg void OnSpinSS(NMHDR *pNotifyStruct,LRESULT *result);
		afx_msg void OnExp();
		virtual void OnOK();
		virtual void OnCancel();
		virtual BOOL OnCommand(WPARAM wParam,LPARAM lParam);
		virtual void DoDataExchange(CDataExchange* pDX);
		void		 FillCombo();
		afx_msg void OnPaint(void);

		//Added Cybage AW 11-03-03
		//Reason : Bug 78025 In Bugzilla.
		virtual BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(svar_DIM_TEXT,CDialog)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_DIM_TEXT_DIMTXT,OnSpinTXT)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_DIM_TEXT_DIMTVP,OnSpinTVP)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_DIM_TEXT_SUPERSCRIPTPOS,OnSpinSS)
	ON_COMMAND(DIM_TEXT_DIMCLRT,DimTextSetColor)
	ON_COMMAND(ICAD_WNDACTION_EXP_RETURN,ReadStyles)
	ON_BN_CLICKED(DIM_TEXT_EXPLORER,OnExp)
	ON_WM_HELPINFO()
	ON_WM_PAINT()
END_MESSAGE_MAP()

//Added Cybage AW 11-03-03[
//Reason : Bug 78025 In Bugzilla.
BOOL svar_DIM_TEXT::PreTranslateMessage(MSG* pMsg)
{
	bOnchar = FALSE;
	if(pMsg->message == WM_CHAR  || pMsg->wParam == VK_DELETE)
		bOnchar = TRUE;
	return CDialog::PreTranslateMessage(pMsg);
}
//Added Cybage AW 11-03-03]

BOOL svar_DIM_TEXT::OnCommand(WPARAM wParam,LPARAM lParam){

	if(HIWORD(wParam)==BN_CLICKED) {
		switch(LOWORD(wParam)) {
			case DIM_TEXT_SUPERSCRIPT:
				OnSuperscriptClick();

			case DIM_TEXT_DIMTIH:
			case DIM_TEXT_DIMTOH:
			case DIM_TEXT_DIMTIX:


				PaintPic();
				break;
		}
	}else if(HIWORD(wParam)==EN_KILLFOCUS) {
		switch(LOWORD(wParam)) {
			case DIM_TEXT_DIMTXT:
			case DIM_TEXT_DIMPOST1:
			case DIM_TEXT_DIMPOST2:
			case DIM_TEXT_DIMTVP:

			case DIM_TEXT_SUPERSCRIPTPOS:

				PaintPic();
				break;
		}
	}else if(HIWORD(wParam)==LBN_SELCHANGE) {
		switch(LOWORD(wParam)) {
			case DIM_TEXT_DIMTXSTY:
			case DIM_TEXT_DIMJUST:
			case DIM_TEXT_DIMTAD:
				PaintPic();
				break;
		}
	}
	//Added Cybage AW 11-03-03[
	//Reason : Bug 78025 In Bugzilla.
	else if(HIWORD(wParam)==EN_CHANGE && bOnchar)
	{
		bOnchar = FALSE;
		switch(LOWORD(wParam))
		{
			case DIM_TEXT_DIMTXT:
			case DIM_TEXT_DIMTVP:
			case DIM_TEXT_SUPERSCRIPTPOS:
				UpdateEditBoxes(wParam, lParam);
				break;
			case DIM_TEXT_DIMPOST1:
			case DIM_TEXT_DIMPOST2:
				{
					struct svar_DlgItem *tmpDi=NULL;
					tmpDi=(struct svar_DlgItem *)GetWindowLong(HWND(lParam),GWL_USERDATA);
					if(tmpDi==NULL) break;
					svar_StrSet(tmpDi,HWND(lParam));
				}
				break;
		}
	}
	//Added Cybage AW 11-03-03]

	return(CDialog::OnCommand(wParam,lParam));
}

void svar_DIM_TEXT::PaintPic(){
	OldBitmap=Bitmap;
	Bitmap=SDS_DimBitmap(320,300,4);
	m_CTextPic.SetBitmap((HBITMAP)Bitmap->GetSafeHandle());
	delete(OldBitmap);
}

BOOL svar_DIM_TEXT::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_SETDIM);
	return(true);
}

void svar_DIM_TEXT::OnExp() {
	SDS_CMainWindow->m_ExpReturnHwnd=m_hWnd;
	SDS_CMainWindow->IcadExplorer(2);
}

void svar_DIM_TEXT::OnSpinTXT(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,DIM_TEXT_DIMTXT),pNMUpDown->iDelta);
	PaintPic();
	*result=0;
}

void svar_DIM_TEXT::OnSpinTVP(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,DIM_TEXT_DIMTVP),pNMUpDown->iDelta);
	PaintPic();
	*result=0;
}

void svar_DIM_TEXT::OnSpinSS(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,DIM_TEXT_SUPERSCRIPTPOS),pNMUpDown->iDelta);
	PaintPic();
	*result=0;
}

void svar_DIM_TEXT::OnOK() {
	svar_iHitEnter = 1;
	::SendMessage(::GetParent(m_hWnd), WM_CLOSE, 0, 0);

	extern HWND SDS_hWndExplorerCaller;
	SDS_hWndExplorerCaller = NULL;
}

void svar_DIM_TEXT::OnCancel() {
	::SendMessage(::GetParent(m_hWnd), WM_CLOSE, 0, 0);

	extern HWND SDS_hWndExplorerCaller;
	SDS_hWndExplorerCaller = NULL;
}

void svar_DIM_TEXT::DimTextSetColor() {
	struct resbuf rb;
	int iTmp;

	SDS_getvar(NULL,DB_QDIMCLRT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	SDS_GetColorDialog(rb.resval.rint,&iTmp,0);
	rb.resval.rint=iTmp;
	sds_setvar("DIMCLRT"/*DNT*/,&rb);
	PaintPic();
}

void svar_DIM_TEXT::DoDataExchange(CDataExchange *pDX) {
	DDX_Control(pDX,DIM_TEXT_DIMTXSTY,m_CTextStyle);
	DDX_Control(pDX,DIM_TEXT_PIC,m_CTextPic);
}

void svar_DIM_TEXT::OnGrey()
{
	if( iSS )
	{
		::EnableWindow(::GetDlgItem(m_hWnd,DIM_TEXT_SUPERSCRIPTPOS), TRUE );
	}
	else
	{
		::EnableWindow(::GetDlgItem(m_hWnd,DIM_TEXT_SUPERSCRIPTPOS), FALSE );
	}
}

void svar_DIM_TEXT::OnSuperscriptClick()
{
	struct resbuf rb;
	rb.restype = RTSHORT;
	iSS = rb.resval.rint = 0;

	if( ::SendMessage( ::GetDlgItem(m_hWnd,DIM_TEXT_SUPERSCRIPT), BM_GETCHECK, 0, 0 ) == BST_CHECKED )
		iSS = rb.resval.rint = 1;

	sds_setvar( "DIMSS", &rb );
	OnGrey();
}

void svar_DIM_TEXT::ReadStyles(){
	CString csTmp;

	m_CTextStyle.ResetContent();
	SDS_StartTblRead("STYLE"/*DNT*/);
	while(SDS_TblReadHasMore()) {
  		csTmp=SDS_TblReadString();
		//Bugzilla No. 78070 ; 17-06-2002
		//if(csTmp.Find('|'/*DNT*/)>0) continue;
		if(csTmp.Find(CMD_XREF_TBLDIVIDER)>0) continue;
		m_CTextStyle.AddString(csTmp);
	}
	PaintPic();
}

BOOL svar_DIM_TEXT::OnInitDialog() {
	//Added Cybage AW 11-03-03
	//Reason : Bug 78025 In Bugzilla.
	bOnchar = FALSE;

	extern HWND SDS_hWndExplorerCaller;
	SDS_hWndExplorerCaller = m_hWnd;

	FillCombo();
	svar_SubClassItems(this);
	CDialog::OnInitDialog();
	ReadStyles();
	OnGrey();
	PaintPic();
	return(TRUE);
}

void svar_DIM_TEXT::FillCombo()
{
	CComboBox *pCombo = NULL;

	if (NULL == (pCombo = (CComboBox *) GetDlgItem(DIM_TEXT_DIMJUST)))
		return;

	pCombo->ResetContent();
	CString	comboOption;
	comboOption.LoadString(IDC_COMBO_DIM_TEXT_DIMJUST_1);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_DIM_TEXT_DIMJUST_2);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_DIM_TEXT_DIMJUST_3);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_DIM_TEXT_DIMJUST_4);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_DIM_TEXT_DIMJUST_5);
	pCombo->AddString(comboOption);

	if (NULL == (pCombo = (CComboBox *) GetDlgItem(DIM_TEXT_DIMTAD)))
		return;

	pCombo->ResetContent();
	comboOption.LoadString(IDC_COMBO_DIM_TEXT_DIMTAD_1);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_DIM_TEXT_DIMTAD_2);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_DIM_TEXT_DIMTAD_3);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_DIM_TEXT_DIMTAD_4);
	pCombo->AddString(comboOption);
}

afx_msg void svar_DIM_TEXT::OnPaint(void)
{
	CDialog::OnPaint();

	CWnd*		pDlgItemWnd;
	CRect		DlgItemRect;
	CClientDC	dc(this);
	CPen		*pOldPen,
				NewPen(PS_SOLID, 0, RGB(195, 50, 50));
	CBrush		*pOldBrush,
				NewBrush(RGB(195, 195, 195));

	if(!IsWindowVisible())
		return;

	pOldPen = dc.SelectObject(&NewPen);
	pOldBrush = dc.SelectObject(&NewBrush);

	for(int i=0;i<svar_num_of_vars;i++)
	{
		if( svar_DlgItemList[i].nGroup != DIM_TEXT) continue;
		if(svar_DlgItemDims[i].isDimVarOverriden)
		{
			pDlgItemWnd = GetDlgItem(svar_DlgItemDims[i].nID);
			pDlgItemWnd->GetWindowRect(&DlgItemRect);
			ScreenToClient(&DlgItemRect);
			DlgItemRect.InflateRect(2, 1);
			dc.Rectangle(&DlgItemRect);
		}
	}
	dc.SelectObject(pOldPen);
	dc.SelectObject(pOldBrush);
}

svar_DIM_TEXT::svar_DIM_TEXT():CDialog(DIM_TEXT){
	Bitmap=OldBitmap=NULL;
	iSS = 0;
	struct resbuf rb;
	if( sds_getvar( "DIMSS", & rb ) == RTNORM )
		iSS = rb.resval.rint;
}

//******This section will control the Dim Units property page

class svar_DIM_TOL : public CDialog {
	private:
		//Added Cybage AW 11-03-03
		//Reason : Bug 78025 In Bugzilla.
		BOOL bOnchar;

	public:
		svar_DIM_TOL();
		BOOL OnInitDialog();
		void PaintPic();
		CStatic m_CTolPic;
		CBitmap *Bitmap,*OldBitmap;
	protected:
		afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
		afx_msg void OnSpinTDEC(NMHDR *pNotifyStruct,LRESULT *result);
		afx_msg void OnSpinTFAC(NMHDR *pNotifyStruct,LRESULT *result);
		afx_msg void OnSpinTM(NMHDR *pNotifyStruct,LRESULT *result);
		afx_msg void OnSpinTP(NMHDR *pNotifyStruct,LRESULT *result);
		virtual void OnCancel();
		virtual BOOL OnCommand(WPARAM wParam,LPARAM lParam);
		virtual void DoDataExchange(CDataExchange* pDX);
		virtual void OnOK();
		afx_msg void OnPaint(void);

		//Added Cybage AW 11-03-03
		//Reason : Bug 78025 In Bugzilla.
		virtual BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(svar_DIM_TOL,CDialog)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_DIM_TOL_DIMTDEC,OnSpinTDEC)
//	  ON_NOTIFY(UDN_DELTAPOS,UPDN_DIM_TOL_DIMTFAC,OnSpinTFAC)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_DIM_TOL_DIMTM,OnSpinTM)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_DIM_TOL_DIMTP,OnSpinTP)
	ON_WM_HELPINFO()
	ON_WM_PAINT()
END_MESSAGE_MAP()

//Added Cybage AW 11-03-03[
//Reason : Bug 78025 In Bugzilla.
BOOL svar_DIM_TOL::PreTranslateMessage(MSG* pMsg)
{
	bOnchar = FALSE;
	if(pMsg->message == WM_CHAR  || pMsg->wParam == VK_DELETE)
		bOnchar = TRUE;
	return CDialog::PreTranslateMessage(pMsg);
}
//Added Cybage AW 11-03-03]

BOOL svar_DIM_TOL::OnCommand(WPARAM wParam,LPARAM lParam){

	if(HIWORD(wParam)==BN_CLICKED) {
		switch(LOWORD(wParam)) {
			case DIM_TOL_DIMLIM:
			case DIM_TOL_DIMTOL:
			case DIM_TOL_DIMTZIN:
			case DIM_TOL_DIMTZIN2:
				// I have added this invalidate because it appears
				// that both the tol and lim checkbox can be checked
				// at the same time. In reality, it is NOT they are not
				// both checked, however the checkboxes are only updated
				// during a paint. Without this invalidate, one needed to
				// for instance open the text window over the dialog then
				// remove it and the other check will be removed. This
				// is an attempt to make it happen when the user actually
				// clicks the checkbox.
				Invalidate();
				PaintPic();
				break;
		}
	}else if(HIWORD(wParam)==EN_KILLFOCUS) {
		switch(LOWORD(wParam)) {
			case DIM_TOL_DIMTM:
			case DIM_TOL_DIMTP:
			case DIM_TOL_DIMTDEC:
//			  case DIM_TOL_DIMTFAC:
				PaintPic();
				break;
		}
	}else if(HIWORD(wParam)==LBN_SELCHANGE) {
//		  switch(LOWORD(wParam)) {
//			  case DIM_TOL_DIMTOLJ:
//				  PaintPic();
//				  break;
//		  }
	}
	//Added Cybage AW 11-03-03[
	//Reason : Bug 78025 In Bugzilla.
	else if(HIWORD(wParam)==EN_CHANGE && bOnchar)
	{
		switch(LOWORD(wParam))
		{
			case DIM_TOL_DIMTM:
			case DIM_TOL_DIMTP:
			case DIM_TOL_DIMTDEC:
				UpdateEditBoxes(wParam, lParam);
				break;
		}
	}
	//Added Cybage AW 11-03-03]

	return(CDialog::OnCommand(wParam,lParam));
}

void svar_DIM_TOL::PaintPic(){
	OldBitmap=Bitmap;
	Bitmap=SDS_DimBitmap(320,300,5);
	m_CTolPic.SetBitmap((HBITMAP)Bitmap->GetSafeHandle());
	delete(OldBitmap);
}

void svar_DIM_TOL::DoDataExchange(CDataExchange *pDX) {
	DDX_Control(pDX,DIM_TOL_PIC,m_CTolPic);
}

BOOL svar_DIM_TOL::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_SETDIM);
	return(true);
}

void svar_DIM_TOL::OnSpinTDEC(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,DIM_TOL_DIMTDEC),pNMUpDown->iDelta);
	PaintPic();
	*result=0;
}

void svar_DIM_TOL::OnSpinTFAC(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

//	  svar_SpinMaster(::GetDlgItem(m_hWnd,DIM_TOL_DIMTFAC),pNMUpDown->iDelta);
//	  PaintPic();
	*result=0;
}

void svar_DIM_TOL::OnSpinTM(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,DIM_TOL_DIMTM),pNMUpDown->iDelta);
	PaintPic();
	*result=0;
}

void svar_DIM_TOL::OnSpinTP(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,DIM_TOL_DIMTP),pNMUpDown->iDelta);
	PaintPic();
	*result=0;
}

void svar_DIM_TOL::OnOK() {
	HWND hParentWnd;

	svar_iHitEnter=1;
	hParentWnd=::GetParent(m_hWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void svar_DIM_TOL::OnCancel() {
	HWND hParentWnd;

	hParentWnd=::GetParent(m_hWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

BOOL svar_DIM_TOL::OnInitDialog() {
	//Added Cybage AW 11-03-03
	//Reason : Bug 78025 In Bugzilla.
	bOnchar = FALSE;

	svar_SubClassItems(this);
	CDialog::OnInitDialog();
	PaintPic();
	struct resbuf rb;
	sds_getvar("DIMTZIN",&rb);
	if(rb.resval.rint==0) {
		::SendMessage(::GetDlgItem(m_hWnd,DIM_TOL_DIMTZIN),BM_SETCHECK,(WPARAM)0,0);
		::SendMessage(::GetDlgItem(m_hWnd,DIM_TOL_DIMTZIN2),BM_SETCHECK,(WPARAM)0,0);
	}else if(rb.resval.rint==4 || rb.resval.rint==5) {
		::SendMessage(::GetDlgItem(m_hWnd,DIM_TOL_DIMTZIN),BM_SETCHECK,(WPARAM)1,0);
		::SendMessage(::GetDlgItem(m_hWnd,DIM_TOL_DIMTZIN2),BM_SETCHECK,(WPARAM)0,0);
	}else if(rb.resval.rint==8 || rb.resval.rint==10) {
		::SendMessage(::GetDlgItem(m_hWnd,DIM_TOL_DIMTZIN),BM_SETCHECK,(WPARAM)0,0);
		::SendMessage(::GetDlgItem(m_hWnd,DIM_TOL_DIMTZIN2),BM_SETCHECK,(WPARAM)1,0);
	}else if(rb.resval.rint==12 || rb.resval.rint==15) {
		::SendMessage(::GetDlgItem(m_hWnd,DIM_TOL_DIMTZIN),BM_SETCHECK,(WPARAM)1,0);
		::SendMessage(::GetDlgItem(m_hWnd,DIM_TOL_DIMTZIN2),BM_SETCHECK,(WPARAM)1,0);
	}
	PaintPic();
	return(TRUE);
}

afx_msg void svar_DIM_TOL::OnPaint(void)
{
	CDialog::OnPaint();

	CWnd*		pDlgItemWnd;
	CRect		DlgItemRect;
	CClientDC	dc(this);
	CPen		*pOldPen,
				NewPen(PS_SOLID, 0, RGB(195, 50, 50));
	CBrush		*pOldBrush,
				NewBrush(RGB(195, 195, 195));

	if(!IsWindowVisible())
		return;

	pOldPen = dc.SelectObject(&NewPen);
	pOldBrush = dc.SelectObject(&NewBrush);

	for(int i=0;i<svar_num_of_vars;i++)
	{
		if( svar_DlgItemList[i].nGroup != DIM_TOL) continue;
		// EBATECH-[ skip DIMTFAC(50) DIMTOLJ(53)
		if (svar_DlgItemList[i].nID == DIM_TOL_DIMTFAC || svar_DlgItemList[i].nID == DIM_TOL_DIMTOLJ) continue;
		// ]-EBATECH
		if(svar_DlgItemDims[i].isDimVarOverriden)
		{
			pDlgItemWnd = GetDlgItem(svar_DlgItemDims[i].nID);
			pDlgItemWnd->GetWindowRect(&DlgItemRect);
			ScreenToClient(&DlgItemRect);
			DlgItemRect.InflateRect(2, 1);
			dc.Rectangle(&DlgItemRect);
		}
	}

	dc.SelectObject(pOldPen);
	dc.SelectObject(pOldBrush);
}

svar_DIM_TOL::svar_DIM_TOL():CDialog(DIM_TOL){
	Bitmap=OldBitmap=NULL;
}

//******This section will control the Dim Units property page

class svar_DIM_UNITS : public CDialog {
	private:
		//Added Cybage AW 11-03-03
		//Reason : Bug 78025 In Bugzilla.
		BOOL bOnchar;

	public:
		svar_DIM_UNITS();
		BOOL OnInitDialog();
		void PaintPic();
		CStatic m_CUnitPic;
		CBitmap *Bitmap,*OldBitmap;
	protected:
		afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
		afx_msg void OnSpinDEC	(NMHDR *pNotifyStruct,LRESULT *result);
		// EBATECH(CNBR) Bugzilla#78443 DIMADEC & DIMAZIN
		afx_msg void OnSpinADEC	(NMHDR *pNotifyStruct,LRESULT *result);
		afx_msg void OnSpinRND(NMHDR *pNotifyStruct,LRESULT *result);
		afx_msg void OnSpinLFAC(NMHDR *pNotifyStruct,LRESULT *result);
		afx_msg void OnSpinSCALE(NMHDR *pNotifyStruct,LRESULT *result);
		virtual void OnOK();
		virtual void OnCancel();
		virtual BOOL OnCommand(WPARAM wParam,LPARAM lParam);
		virtual void DoDataExchange(CDataExchange* pDX);
		void		 FillCombo();
		afx_msg void OnPaint(void);

		//Added Cybage AW 11-03-03
		//Reason : Bug 78025 In Bugzilla.
		virtual BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(svar_DIM_UNITS,CDialog)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_DIM_UNITS_DIMDEC,OnSpinDEC)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_DIM_UNITS_DIMADEC,OnSpinADEC)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_DIM_UNITS_DIMRND,OnSpinRND)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_DIM_UNITS_DIMLFAC,OnSpinLFAC)
	ON_NOTIFY(UDN_DELTAPOS,UPDN_DIM_UNITS_DIMSCALE,OnSpinSCALE)
	ON_WM_HELPINFO()
	ON_WM_PAINT()
END_MESSAGE_MAP()

//Added Cybage AW 11-03-03[
//Reason : Bug 78025 In Bugzilla.
BOOL svar_DIM_UNITS::PreTranslateMessage(MSG* pMsg)
{
	bOnchar = FALSE;
	if(pMsg->message == WM_CHAR || pMsg->wParam == VK_DELETE)
		bOnchar = TRUE;
	return CDialog::PreTranslateMessage(pMsg);
}
//Added Cybage AW 11-03-03]

BOOL svar_DIM_UNITS::OnCommand(WPARAM wParam,LPARAM lParam){

	if(HIWORD(wParam)==BN_CLICKED) {
		switch(LOWORD(wParam)) {
			case DIM_UNITS_DIMZIN1:
			case DIM_UNITS_DIMZIN2:
				PaintPic();
				break;
		}
	}else if(HIWORD(wParam)==EN_KILLFOCUS) {
		switch(LOWORD(wParam)) {
			case DIM_UNITS_DIMDEC:
			case DIM_UNITS_DIMADEC:
			case DIM_UNITS_DIMRND:
			case DIM_UNITS_DIMSCALE:
			case DIM_UNITS_DIMLFAC:
				PaintPic();
				break;
		}
	}else if(HIWORD(wParam)==LBN_SELCHANGE) {
		switch(LOWORD(wParam)) {
			case DIM_UNITS_DIMUNIT:
			case DIM_UNITS_DIMAUNIT:
			case DIM_UNITS_DIMAZIN:
				PaintPic();
				break;
		}
	}
	//Added Cybage AW 11-03-03[
	//Reason : Bug 78025 In Bugzilla.
	else if(HIWORD(wParam)==EN_CHANGE && bOnchar)
	{
		switch(LOWORD(wParam))
		{
			case DIM_UNITS_DIMDEC:
			case DIM_UNITS_DIMADEC:
			case DIM_UNITS_DIMRND:
			case DIM_UNITS_DIMSCALE:
			case DIM_UNITS_DIMLFAC:
			case DIM_UNITS_DIMDSEP:
				UpdateEditBoxes(wParam, lParam);
				break;
		}
	}
	//Added Cybage AW 11-03-03]

	return(CDialog::OnCommand(wParam,lParam));
}


void svar_DIM_UNITS::PaintPic(){
	OldBitmap=Bitmap;
	Bitmap=SDS_DimBitmap(320,300,6);
	m_CUnitPic.SetBitmap((HBITMAP)Bitmap->GetSafeHandle());
	delete(OldBitmap);
}

void svar_DIM_UNITS::DoDataExchange(CDataExchange *pDX) {
	DDX_Control(pDX,DIM_UNITS_PIC,m_CUnitPic);
}

BOOL svar_DIM_UNITS::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_SETDIM);
	return(true);
}

void svar_DIM_UNITS::OnSpinDEC(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,DIM_UNITS_DIMDEC),pNMUpDown->iDelta);
	PaintPic();
	*result=0;
}

// EBATECH(CNBR) Bugzilla#78443 DIMADEC & DIMAZIN
void svar_DIM_UNITS::OnSpinADEC(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,DIM_UNITS_DIMADEC),pNMUpDown->iDelta);
	PaintPic();
	*result=0;
}
// EBATECH(CNBR) ]-

void svar_DIM_UNITS::OnSpinRND(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,DIM_UNITS_DIMRND),pNMUpDown->iDelta);
	PaintPic();
	*result=0;
}

void svar_DIM_UNITS::OnSpinLFAC(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,DIM_UNITS_DIMLFAC),pNMUpDown->iDelta);
	PaintPic();
	*result=0;
}

void svar_DIM_UNITS::OnSpinSCALE(NMHDR *pNotifyStruct,LRESULT *result) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNotifyStruct;

	svar_SpinMaster(::GetDlgItem(m_hWnd,DIM_UNITS_DIMSCALE),pNMUpDown->iDelta);
	PaintPic();
	*result=0;
}

void svar_DIM_UNITS::OnOK() {
	HWND hParentWnd;

	svar_iHitEnter=1;
	hParentWnd=::GetParent(m_hWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

void svar_DIM_UNITS::OnCancel() {
	HWND hParentWnd;

	hParentWnd=::GetParent(m_hWnd);
	::SendMessage(hParentWnd,WM_CLOSE,0,0);
}

BOOL svar_DIM_UNITS::OnInitDialog() {
	//Added Cybage AW 11-03-03
	//Reason : Bug 78025 In Bugzilla.
	bOnchar = FALSE;

	FillCombo();
	svar_SubClassItems(this);
	CDialog::OnInitDialog();
	PaintPic();

	struct resbuf rb;
	SDS_getvar(NULL,DB_QDIMUNIT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	// EBATECH(CNBR) 2003/2/6 DIMUNIT=8(Windows Desktop) affect Suppress Leading/Trail zeros.
	if(rb.resval.rint>3 && rb.resval.rint<8){
		SDS_getvar(NULL,DB_QDIMZIN,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		if(rb.resval.rint==0) {
			::SendMessage(::GetDlgItem(m_hWnd,DIM_UNITS_DIMZIN1),BM_SETCHECK,(WPARAM)1,0);
			::SendMessage(::GetDlgItem(m_hWnd,DIM_UNITS_DIMZIN2),BM_SETCHECK,(WPARAM)1,0);
		}else if(rb.resval.rint==1) {
			::SendMessage(::GetDlgItem(m_hWnd,DIM_UNITS_DIMZIN1),BM_SETCHECK,(WPARAM)0,0);
			::SendMessage(::GetDlgItem(m_hWnd,DIM_UNITS_DIMZIN2),BM_SETCHECK,(WPARAM)0,0);
		}else if(rb.resval.rint==2) {
			::SendMessage(::GetDlgItem(m_hWnd,DIM_UNITS_DIMZIN1),BM_SETCHECK,(WPARAM)0,0);
			::SendMessage(::GetDlgItem(m_hWnd,DIM_UNITS_DIMZIN2),BM_SETCHECK,(WPARAM)1,0);
		}else if(rb.resval.rint==3) {
			::SendMessage(::GetDlgItem(m_hWnd,DIM_UNITS_DIMZIN1),BM_SETCHECK,(WPARAM)1,0);
			::SendMessage(::GetDlgItem(m_hWnd,DIM_UNITS_DIMZIN2),BM_SETCHECK,(WPARAM)0,0);
		}
	}else{
		SDS_getvar(NULL,DB_QDIMZIN,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		if(rb.resval.rint&8) {
			::SendMessage(::GetDlgItem(m_hWnd,DIM_UNITS_DIMZIN2),BM_SETCHECK,(WPARAM)1,0);
		}
		if(rb.resval.rint&4) {
			::SendMessage(::GetDlgItem(m_hWnd,DIM_UNITS_DIMZIN1),BM_SETCHECK,(WPARAM)1,0);
		}
	}
	return(TRUE);
}

void svar_DIM_UNITS::FillCombo()
{
	CComboBox *pCombo = NULL;

	if (NULL == (pCombo = (CComboBox *) GetDlgItem(DIM_UNITS_DIMUNIT)))
		return;

	pCombo->ResetContent();
	CString	comboOption;
	comboOption.LoadString(IDC_COMBO_DIM_UNITS_DIMUNIT_1);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_DIM_UNITS_DIMUNIT_2);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_DIM_UNITS_DIMUNIT_3);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_DIM_UNITS_DIMUNIT_4);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_DIM_UNITS_DIMUNIT_5);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_DIM_UNITS_DIMUNIT_6);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_DIM_UNITS_DIMUNIT_7);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_DIM_UNITS_DIMUNIT_8);
	pCombo->AddString(comboOption);

	if (NULL == (pCombo = (CComboBox *) GetDlgItem(DIM_UNITS_DIMAUNIT)))
		return;

	pCombo->ResetContent();
	comboOption.LoadString(IDC_COMBO_DIM_UNITS_DIMAUNIT_1);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_DIM_UNITS_DIMAUNIT_2);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_DIM_UNITS_DIMAUNIT_3);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_DIM_UNITS_DIMAUNIT_4);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_DIM_UNITS_DIMAUNIT_5);
	pCombo->AddString(comboOption);

	// EBATECH(CNBR) Bugzilla#78443 DIMADEC & DIMAZIN
	if (NULL == (pCombo = (CComboBox *) GetDlgItem(DIM_UNITS_DIMAZIN)))
		return;

	pCombo->ResetContent();
	comboOption.LoadString(IDC_COMBO_DIM_UNITS_DIMAZIN_0);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_DIM_UNITS_DIMAZIN_1);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_DIM_UNITS_DIMAZIN_2);
	pCombo->AddString(comboOption);
	comboOption.LoadString(IDC_COMBO_DIM_UNITS_DIMAZIN_3);
	pCombo->AddString(comboOption);
	// EBATECH(CNBR) ]-

}

afx_msg void svar_DIM_UNITS::OnPaint(void)
{
	CDialog::OnPaint();

	CWnd*		pDlgItemWnd;
	CRect		DlgItemRect;
	CClientDC	dc(this);
	CPen		*pOldPen,
				NewPen(PS_SOLID, 0, RGB(195, 50, 50));
	CBrush		*pOldBrush,
				NewBrush(RGB(195, 195, 195));

	if(!IsWindowVisible())
		return;

	pOldPen = dc.SelectObject(&NewPen);
	pOldBrush = dc.SelectObject(&NewBrush);

	for(int i=0;i<svar_num_of_vars;i++)
	{
		if( svar_DlgItemList[i].nGroup != DIM_UNITS) continue;
		if(svar_DlgItemDims[i].isDimVarOverriden)
		{
			pDlgItemWnd = GetDlgItem(svar_DlgItemDims[i].nID);
			pDlgItemWnd->GetWindowRect(&DlgItemRect);
			ScreenToClient(&DlgItemRect);
			DlgItemRect.InflateRect(2, 1);
			dc.Rectangle(&DlgItemRect);
		}
	}
	dc.SelectObject(pOldPen);
	dc.SelectObject(pOldBrush);
}

svar_DIM_UNITS::svar_DIM_UNITS():CDialog(DIM_UNITS){
	Bitmap=OldBitmap=NULL;
}


//
// -----------------------This is for the Dims pages----------------------------
//

class svar_NewStyleDims : public CDialog {
	public:
		CString csStyleName;
		svar_NewStyleDims();
		BOOL OnInitDialog();
	protected:
		afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
		virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(svar_NewStyleDims,CDialog)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL svar_NewStyleDims::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_SETDIM);
	return(true);
}

BOOL svar_NewStyleDims::OnInitDialog() {
	CWnd *item;

	CDialog::OnInitDialog();

	item=GetDlgItem(DIM_NEW_STYLE);
	item->SetFocus();

	::SendMessage(::GetDlgItem(m_hWnd,DIM_NEW_STYLE),EM_SETSEL,0,-1);

	return(0);
}

void svar_NewStyleDims::DoDataExchange(CDataExchange *pDX) {
	DDX_Text(pDX,DIM_NEW_STYLE,csStyleName);
	csStyleName.TrimLeft();
	csStyleName.TrimRight();	
	if(pDX->m_bSaveAndValidate&&!ic_isvalidname(csStyleName. GetBuffer(IC_ACADNMLEN+1))){
		CString	str;	/*D.G.*/
		str.FormatMessage(ResourceString(IDC_WRONG_NAME_MESSAGE, "The name can use up to %1!d! characters, including letters, numbers,\nhyphens, underscores, and dollar signs ($), but not spaces.\nPlease try again."), IC_ACADNMLEN - 1);
		MessageBox((LPCTSTR)str, ResourceString(IDC_ICADSETVARSDIA_INVALID_132, "Invalid Name Entered"), MB_ICONERROR | MB_OK);
		pDX->Fail();
	}
}

svar_NewStyleDims::svar_NewStyleDims() : CDialog(DIM_NEW) {
}

//-----------------------------------------------------------------

class svar_RenameStyleDims : public CDialog {
	public:
		CString csOldStyleName,csNewStyleName;
		CComboBox m_OldStyle;
		svar_RenameStyleDims();
		BOOL OnInitDialog();
	protected:
		afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
		virtual void OnOK();
		virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(svar_RenameStyleDims,CDialog)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL svar_RenameStyleDims::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_SETDIM);
	return(true);
}

void svar_RenameStyleDims::OnOK() {

	if(UpdateData(TRUE)!=1) return;
	if(m_OldStyle.FindStringExact(-1,(LPCTSTR)csNewStyleName)!=CB_ERR) {
		MessageBox(ResourceString(IDC_ICADSETVARSDIA_STYLE_A_133, "Style already exists!" ),ResourceString(IDC_ICADSETVARSDIA_DIMENSI_134, "Dimension Style" ),MB_ICONEXCLAMATION|MB_OK|MB_APPLMODAL);
		return;
	}
	CDialog::OnOK();
}

BOOL svar_RenameStyleDims::OnInitDialog() {
	CWnd *item;
	struct resbuf rb;
	CString csTmp;

	SDS_getvar(NULL,DB_QDIMSTYLE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	csOldStyleName=rb.resval.rstring;
	if(rb.resval.rstring){IC_FREE(rb.resval.rstring);rb.resval.rstring=NULL;}
	
	CDialog::OnInitDialog();

	SDS_StartTblRead("DIMSTYLE"/*DNT*/);
	while(SDS_TblReadHasMore()) {
  		csTmp=SDS_TblReadString();
		//Bugzilla No. 78070 ; 17-06-2002
		//if(csTmp.Find('|'/*DNT*/)>0) continue;
		if(csTmp.Find(CMD_XREF_TBLDIVIDER)>0) continue;
		m_OldStyle.AddString(csTmp);
	}
	m_OldStyle.SelectString(-1,(LPCTSTR)csOldStyleName);

	item=GetDlgItem(DIM_RENAME_TO);
	item->SetFocus();

	::SendMessage(::GetDlgItem(m_hWnd,DIM_RENAME_TO),EM_SETSEL,0,-1);

	return(0);
}

void svar_RenameStyleDims::DoDataExchange(CDataExchange *pDX) {
	DDX_Text(pDX,DIM_RENAME_TO,csNewStyleName);
	if(pDX->m_bSaveAndValidate&&!ic_isvalidname(csNewStyleName.GetBuffer(IC_ACADNMLEN+1))){
		CString	str;	/*D.G.*/
		str.FormatMessage(ResourceString(IDC_WRONG_NAME_MESSAGE, "The name can use up to %1!d! characters, including letters, numbers,\nhyphens, underscores, and dollar signs ($), but not spaces.\nPlease try again."), IC_ACADNMLEN - 1);
		MessageBox((LPCTSTR)str, ResourceString(IDC_ICADSETVARSDIA_INVALID_132, "Invalid Name Entered"), MB_ICONERROR | MB_OK);
		pDX->Fail();
	}
	DDX_Text(pDX,DIM_RENAME_FROM,csOldStyleName);
	DDX_Control(pDX,DIM_RENAME_FROM,m_OldStyle);
}

svar_RenameStyleDims::svar_RenameStyleDims() : CDialog(DIM_RENAME) {

}

//--------------------------------------------------------------

class svar_DeleteStyleDims : public CDialog {
	public:
		CString csStyleName;
		CComboBox m_StyleName;
		svar_DeleteStyleDims();
		BOOL OnInitDialog();
	protected:
		afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
		virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(svar_DeleteStyleDims,CDialog)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL svar_DeleteStyleDims::OnHelpInfo(HELPINFO *pHel){
	CommandRefHelp( m_hWnd, HLP_SETDIM);
	return(true);
}

BOOL svar_DeleteStyleDims::OnInitDialog() {
	CWnd *item;
	char *tmp;
	struct resbuf rb;

	SDS_getvar(NULL,DB_QDIMSTYLE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	csStyleName=rb.resval.rstring;
	if(rb.resval.rstring){IC_FREE(rb.resval.rstring);rb.resval.rstring=NULL;}
	
	CDialog::OnInitDialog();

	SDS_StartTblRead("DIMSTYLE"/*DNT*/);
	while(SDS_TblReadHasMore()) {
		tmp=SDS_TblReadString();
		//Bugzilla No. 78070 ; 17-06-2002
		//if(csStyleName.CompareNoCase(tmp)) {
		//	  m_StyleName.AddString(tmp);
		//}
		if(strchr(tmp,CMD_XREF_TBLDIVIDER) != NULL || csStyleName.CompareNoCase(tmp) == 0 )
			continue;
			m_StyleName.AddString(tmp);
		}
	//Bugzilla No. 78070 ; 17-06-2002 [
	//m_StyleName.SetCurSel(0);

	//item=GetDlgItem(DIM_DELETE_DEL);
	//item->SetFocus();

	if ( m_StyleName.GetCount() == 0)
	{
		item=GetDlgItem(IDOK);
		item->EnableWindow(FALSE);
		item=GetDlgItem(IDCANCEL);
		item->SetFocus();
	}
	else
	{
	m_StyleName.SetCurSel(0);
	item=GetDlgItem(DIM_DELETE_DEL);
	item->SetFocus();
	}
	//Bugzilla No. 78070 ; 17-06-2002 ]

	return(0);
}

void svar_DeleteStyleDims::DoDataExchange(CDataExchange *pDX) {
	DDX_Text(pDX,DIM_DELETE_DEL,csStyleName);
	DDX_Control(pDX,DIM_DELETE_DEL,m_StyleName);
}

svar_DeleteStyleDims::svar_DeleteStyleDims() : CDialog(DIM_DELETE) {
}
//-----------------------------------------------------------------

class svar_DrawingCfgDims : public IcadDialog {
	public:
		int			m_CurSel;
		CString		csDimStyle;
		CComboBox	m_DimStyle;
		CTabCtrl	m_CTabCtrl;
		svar_DIM_ARROWS  m_DIM_ARROWS;
		svar_DIM_FORMAT  m_DIM_FORMAT;
		svar_DIM_LINES	 m_DIM_LINES;
		svar_DIM_TEXT	 m_DIM_TEXT;
		svar_DIM_TOL	 m_DIM_TOL;    //MHB
		svar_DIM_UNITS	 m_DIM_UNITS;
		svar_DIM_ALT	 m_DIM_ALT;    //MHB
		svar_NewStyleDims	 m_NewStyle;
		svar_RenameStyleDims m_RenameStyle;
		svar_DeleteStyleDims m_DeleteStyle;
		bool	m_bDimStyleOverriden;	/*D.G.*/

		svar_DrawingCfgDims(bool bDimensionProps = false);
		BOOL	OnInitDialog();
		void	Save2Style(char *pStyle,bool mode);
		void	CheckOverriding();		/*D.G.*/
		void	ClearOverriding();		/*D.G.*/
	protected:
		bool	m_bDimensionProps;		/*D.G.*/

	protected:
		afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
		afx_msg void OnHelp();
		afx_msg void OnSave();
		afx_msg void OnNew();
		afx_msg void OnReset();
		afx_msg void OnRename();
		afx_msg void OnDelete();
		afx_msg void dim_FillStyle();
		afx_msg void OnDimChange();
		virtual void OnOK();
		virtual void DoDataExchange(CDataExchange* pDX);
	 	afx_msg void OnTabChange(NMHDR *pNotifyStruct,LRESULT *result);
		void dim_SetDimValues();
		struct svar_DlgItem* GetPos(int nID);

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(svar_DrawingCfgDims, CDialog)
	ON_CBN_SELCHANGE(DIM_MAIN_DIMSTYLE,OnDimChange)
	ON_BN_CLICKED(IDHELP,OnHelp)
	ON_NOTIFY(TCN_SELCHANGE, DIM_GEN_TAB, OnTabChange)
	ON_BN_CLICKED(DIM_MAIN_RESET,OnReset)
	ON_BN_CLICKED(DIM_MAIN_NEW,OnNew)
	ON_BN_CLICKED(DIM_MAIN_SAVE,OnSave)
	ON_BN_CLICKED(DIM_MAIN_RENAME,OnRename)
	ON_BN_CLICKED(DIM_MAIN_DELETE,OnDelete)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL svar_DrawingCfgDims::OnHelpInfo(HELPINFO *pHel){
	OnHelp();
	return(true);
}

void svar_DrawingCfgDims::OnHelp() {
	CommandRefHelp( m_hWnd, HLP_SETDIM);
}

void svar_DrawingCfgDims::OnReset(){
	struct db_sysvarlink *sysvar=db_Getsysvar();
	int total_num=db_Getnvars(),i,sel;
	struct resbuf rb;

	for(i=0;i<total_num;i++) {
		if(strncmp(sysvar[i].name,"DIM"/*DMT*/, 3)) continue;
		rb.restype=sysvar[i].type;
		switch(sysvar[i].type){
			case RTREAL:
				rb.resval.rreal=atof(sysvar[i].defval);
				sds_setvar(sysvar[i].name,&rb);
				break;
			case RTSHORT:
				rb.resval.rint=atoi(sysvar[i].defval);
				sds_setvar(sysvar[i].name,&rb);
				break;
			case RTSTR:
				if(!strsame(sysvar[i].name,"DIMSTYLE"/*DNT*/)){
					rb.resval.rstring=sysvar[i].defval;
					sds_setvar(sysvar[i].name,&rb);
				}
				break;
			case RTLONG:
				rb.resval.rlong=atol(sysvar[i].defval);
				sds_setvar(sysvar[i].name,&rb);
				break;
		}
	}
	SDS_getvar(NULL,DB_QMEASUREMENT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if( rb.resval.rint == DB_METRIC_MEASUREINIT )
	{
		char* varname;
		sysvar = db_metric_sysvarlink();
		for (i=0; db_getvaralt( i, sysvar, &varname, &rb ) == RTNORM ; i++)
		{
			if( strnicmp(varname, "DIM"/*DMT*/, 3 ) == 0 &&
				stricmp(varname, "DIMSTYLE") != 0 )
				sds_setvar(varname,&rb);
			if( rb.restype == RTSTR )
				IC_FREE( rb.resval.rstring );				
		}
	}
	sel=m_CTabCtrl.GetCurSel(); //Get the current selection
	switch(sel) {
		case 0:
			::SendMessage(::GetDlgItem(m_DIM_ARROWS.m_hWnd,DIM_ARROWS_DIMASZ),WM_PAINT,0,0);
			::SendMessage(::GetDlgItem(m_DIM_ARROWS.m_hWnd,DIM_ARROWS_START),WM_PAINT,0,0);
			::SendMessage(::GetDlgItem(m_DIM_ARROWS.m_hWnd,DIM_ARROWS_END),WM_PAINT,0,0);
			::SendMessage(::GetDlgItem(m_DIM_ARROWS.m_hWnd,DIM_ARROWS_DIMTSZ),WM_PAINT,0,0);
			//Modified Cybage AW 16-02-01
			::SendMessage(::GetDlgItem(m_DIM_ARROWS.m_hWnd,LDR_ARROWS),WM_PAINT,0,0);

			m_DIM_ARROWS.TickSet();
			m_DIM_ARROWS.PaintPic();
			break;
		case 1:
			::SendMessage(::GetDlgItem(m_DIM_FORMAT.m_hWnd,DIM_FORMAT_DIMFIT0),WM_PAINT,0,0);
			::SendMessage(::GetDlgItem(m_DIM_FORMAT.m_hWnd,DIM_FORMAT_DIMFIT1),WM_PAINT,0,0);
			::SendMessage(::GetDlgItem(m_DIM_FORMAT.m_hWnd,DIM_FORMAT_DIMFIT2),WM_PAINT,0,0);
			::SendMessage(::GetDlgItem(m_DIM_FORMAT.m_hWnd,DIM_FORMAT_DIMFIT3),WM_PAINT,0,0);
			::SendMessage(::GetDlgItem(m_DIM_FORMAT.m_hWnd,DIM_FORMAT_DIMFIT4),WM_PAINT,0,0);
			::SendMessage(::GetDlgItem(m_DIM_FORMAT.m_hWnd,DIM_FORMAT_DIMFIT5),WM_PAINT,0,0);
			::SendMessage(::GetDlgItem(m_DIM_FORMAT.m_hWnd,DIM_FORMAT_DIMGAP),WM_PAINT,0,0);
			m_DIM_FORMAT.PaintPic();
			break;
		case 2:
			::SendMessage(::GetDlgItem(m_DIM_LINES.m_hWnd,DIM_LINES_DIMCEN),WM_PAINT,0,0);
			::SendMessage(::GetDlgItem(m_DIM_LINES.m_hWnd,DIM_LINES_DIMDLI),WM_PAINT,0,0);
			::SendMessage(::GetDlgItem(m_DIM_LINES.m_hWnd,DIM_LINES_DIMDLE),WM_PAINT,0,0);
			::SendMessage(::GetDlgItem(m_DIM_LINES.m_hWnd,DIM_LINES_DIMEXE),WM_PAINT,0,0);
			::SendMessage(::GetDlgItem(m_DIM_LINES.m_hWnd,DIM_LINES_DIMEXO),WM_PAINT,0,0);
			::SendMessage(::GetDlgItem(m_DIM_LINES.m_hWnd,DIM_LINES_DIMSD1),WM_PAINT,0,0);
			::SendMessage(::GetDlgItem(m_DIM_LINES.m_hWnd,DIM_LINES_DIMSD2),WM_PAINT,0,0);
			::SendMessage(::GetDlgItem(m_DIM_LINES.m_hWnd,DIM_LINES_DIMSE1),WM_PAINT,0,0);
			::SendMessage(::GetDlgItem(m_DIM_LINES.m_hWnd,DIM_LINES_DIMSE2),WM_PAINT,0,0);
			::SendMessage(::GetDlgItem(m_DIM_LINES.m_hWnd,DIM_LINES_DIMSOXD),WM_PAINT,0,0);
			::SendMessage(::GetDlgItem(m_DIM_LINES.m_hWnd,DIM_LINES_DIMTOFL),WM_PAINT,0,0);
			::SendMessage(::GetDlgItem(m_DIM_LINES.m_hWnd,DIM_LINES_DIMLWD),WM_PAINT,0,0);
			::SendMessage(::GetDlgItem(m_DIM_LINES.m_hWnd,DIM_LINES_DIMLWE),WM_PAINT,0,0);
			m_DIM_LINES.PaintPic();
			break;
		case 3:
			::SendMessage(::GetDlgItem(m_DIM_TEXT.m_hWnd,DIM_TEXT_DIMJUST),WM_PAINT,0,0);
			::SendMessage(::GetDlgItem(m_DIM_TEXT.m_hWnd,DIM_TEXT_DIMPOST1),WM_PAINT,0,0);
			::SendMessage(::GetDlgItem(m_DIM_TEXT.m_hWnd,DIM_TEXT_DIMPOST2),WM_PAINT,0,0);
			::SendMessage(::GetDlgItem(m_DIM_TEXT.m_hWnd,DIM_TEXT_DIMTAD),WM_PAINT,0,0);
			::SendMessage(::GetDlgItem(m_DIM_TEXT.m_hWnd,DIM_TEXT_DIMTIH),WM_PAINT,0,0);
			::SendMessage(::GetDlgItem(m_DIM_TEXT.m_hWnd,DIM_TEXT_DIMTIX),WM_PAINT,0,0);
			::SendMessage(::GetDlgItem(m_DIM_TEXT.m_hWnd,DIM_TEXT_DIMTOH),WM_PAINT,0,0);
			::SendMessage(::GetDlgItem(m_DIM_TEXT.m_hWnd,DIM_TEXT_DIMTXSTY),WM_PAINT,0,0);
			::SendMessage(::GetDlgItem(m_DIM_TEXT.m_hWnd,DIM_TEXT_DIMTXT),WM_PAINT,0,0);
			::SendMessage(::GetDlgItem(m_DIM_TEXT.m_hWnd,DIM_TEXT_DIMTVP),WM_PAINT,0,0);
			m_DIM_TEXT.PaintPic();
			break;
		case 4:
			::SendMessage(::GetDlgItem(m_DIM_TOL.m_hWnd,DIM_TOL_DIMTDEC),WM_PAINT,0,0);    //MHB
			::SendMessage(::GetDlgItem(m_DIM_TOL.m_hWnd,DIM_TOL_DIMLIM),WM_PAINT,0,0);
//			  ::SendMessage(::GetDlgItem(m_DIM_TOL.m_hWnd,DIM_TOL_DIMTFAC),WM_PAINT,0,0);
			::SendMessage(::GetDlgItem(m_DIM_TOL.m_hWnd,DIM_TOL_DIMTM),WM_PAINT,0,0);
			::SendMessage(::GetDlgItem(m_DIM_TOL.m_hWnd,DIM_TOL_DIMTOL),WM_PAINT,0,0);
//			  ::SendMessage(::GetDlgItem(m_DIM_TOL.m_hWnd,DIM_TOL_DIMTOLJ),WM_PAINT,0,0);
			::SendMessage(::GetDlgItem(m_DIM_TOL.m_hWnd,DIM_TOL_DIMTP),WM_PAINT,0,0);
			::SendMessage(::GetDlgItem(m_DIM_TOL.m_hWnd,DIM_TOL_DIMTZIN),WM_PAINT,0,0);
			::SendMessage(::GetDlgItem(m_DIM_TOL.m_hWnd,DIM_TOL_DIMTZIN2),WM_PAINT,0,0);
			m_DIM_TOL.PaintPic();
			break;
		case 5:
			::SendMessage(::GetDlgItem(m_DIM_UNITS.m_hWnd,DIM_UNITS_DIMAUNIT),WM_PAINT,0,0);
			::SendMessage(::GetDlgItem(m_DIM_UNITS.m_hWnd,DIM_UNITS_DIMLFAC),WM_PAINT,0,0);
			::SendMessage(::GetDlgItem(m_DIM_UNITS.m_hWnd,DIM_UNITS_DIMDEC),WM_PAINT,0,0);
			::SendMessage(::GetDlgItem(m_DIM_UNITS.m_hWnd,DIM_UNITS_DIMRND),WM_PAINT,0,0);
			::SendMessage(::GetDlgItem(m_DIM_UNITS.m_hWnd,DIM_UNITS_DIMSCALE),WM_PAINT,0,0);
			::SendMessage(::GetDlgItem(m_DIM_UNITS.m_hWnd,DIM_UNITS_DIMUNIT),WM_PAINT,0,0);
			::SendMessage(::GetDlgItem(m_DIM_UNITS.m_hWnd,DIM_UNITS_DIMZIN1),WM_PAINT,0,0);
			::SendMessage(::GetDlgItem(m_DIM_UNITS.m_hWnd,DIM_UNITS_DIMZIN2),WM_PAINT,0,0);
			// EBATECH(CNBR) -[ Bugzilla#78443 DIMADEC and DIMAZIN
			::SendMessage(::GetDlgItem(m_DIM_UNITS.m_hWnd,DIM_UNITS_DIMADEC),WM_PAINT,0,0);
			::SendMessage(::GetDlgItem(m_DIM_UNITS.m_hWnd,DIM_UNITS_DIMAZIN),WM_PAINT,0,0);
			// EBATECH(CNBR) ]-
			::SendMessage(::GetDlgItem(m_DIM_UNITS.m_hWnd,DIM_UNITS_DIMDSEP),WM_PAINT,0,0);
			m_DIM_UNITS.PaintPic();
			break;
		case 6:
			::SendMessage(::GetDlgItem(m_DIM_ALT.m_hWnd,DIM_ALT_DIMALT),WM_PAINT,0,0);		//MHB
			::SendMessage(::GetDlgItem(m_DIM_ALT.m_hWnd,DIM_ALT_DIMALTD),WM_PAINT,0,0);
			::SendMessage(::GetDlgItem(m_DIM_ALT.m_hWnd,DIM_ALT_DIMALTF),WM_PAINT,0,0);
			::SendMessage(::GetDlgItem(m_DIM_ALT.m_hWnd,DIM_ALT_DIMALTTD),WM_PAINT,0,0);
			::SendMessage(::GetDlgItem(m_DIM_ALT.m_hWnd,DIM_ALT_DIMALTTZ1),WM_PAINT,0,0);
			::SendMessage(::GetDlgItem(m_DIM_ALT.m_hWnd,DIM_ALT_DIMALTTZ2),WM_PAINT,0,0);
			::SendMessage(::GetDlgItem(m_DIM_ALT.m_hWnd,DIM_ALT_DIMALTU),WM_PAINT,0,0);
			::SendMessage(::GetDlgItem(m_DIM_ALT.m_hWnd,DIM_ALT_DIMALTZ1),WM_PAINT,0,0);
			::SendMessage(::GetDlgItem(m_DIM_ALT.m_hWnd,DIM_ALT_DIMALTZ2),WM_PAINT,0,0);
			::SendMessage(::GetDlgItem(m_DIM_ALT.m_hWnd,DIM_ALT_DIMAPOST1),WM_PAINT,0,0);
			::SendMessage(::GetDlgItem(m_DIM_ALT.m_hWnd,DIM_ALT_DIMAPOST2),WM_PAINT,0,0);
			::SendMessage(::GetDlgItem(m_DIM_ALT.m_hWnd,DIM_ALT_DIMALTRND),WM_PAINT,0,0);
			m_DIM_ALT.PaintPic();
			break;
	}

	CheckOverriding();
	if(!m_bDimStyleOverriden)
		ClearOverriding();
	else
	{
		m_DimStyle.InsertString(m_DimStyle.SelectString(-1,(LPCTSTR)csDimStyle) + 1,
								ResourceString(IDC_ICADSETVARSDIA_OVERRIDEN, "--- Overrides"));
		m_DimStyle.SetCurSel(m_DimStyle.SelectString(-1,(LPCTSTR)csDimStyle) + 1);
	}
	m_CTabCtrl.GetActiveWindow()->InvalidateRect(NULL);
	m_CTabCtrl.GetActiveWindow()->SendMessage(WM_PAINT);
}

void svar_DrawingCfgDims::DoDataExchange(CDataExchange *pDX) {

 	DDX_Control(pDX,DIM_GEN_TAB,m_CTabCtrl);
	DDX_Control(pDX,DIM_MAIN_DIMSTYLE,m_DimStyle);
	DDX_CBString(pDX,DIM_MAIN_DIMSTYLE,csDimStyle);
}

// Create internal block on the fly
bool createArrowBlock(const short qidx, const int ErrNo)
{
	struct resbuf rb;
	SDS_getvar(NULL, qidx, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
	char* szBlk = rb.resval.rstring;
	checkBlock(szBlk, SDS_CURDWG);
	if (szBlk[0]) {
		db_blocktabrec* theblk = (db_blocktabrec*)SDS_CURDWG->findtabrec(DB_BLOCKTAB, szBlk, 1);
		if (theblk == NULL) {
			cmd_ErrorPrompt(CMD_ERR_DIMBLKDOESNOTEXIST, 0);
			IC_FREE(rb.resval.rstring);	/*DG - 5.12.2001*/// For good style avoiding bad things.			
			return false;
		}
	}
	IC_FREE(rb.resval.rstring);	/*DG - 5.12.2001*/// For good style avoiding bad things.	
	return true;
}

void svar_DrawingCfgDims::OnOK() {
	// EBATECH(watanabe)-[dimldrblk
	// Create internal block on the fly
	if (createArrowBlock(DB_QDIMBLK,	CMD_ERR_DIMBLKDOESNOTEXIST )   == false) return;
	if (createArrowBlock(DB_QDIMBLK1,	CMD_ERR_DIMBLK1DOESNOTEXIST)   == false) return;
	if (createArrowBlock(DB_QDIMBLK2,	CMD_ERR_DIMBLK2DOESNOTEXIST)   == false) return;
	if (createArrowBlock(DB_QDIMLDRBLK, CMD_ERR_DIMLDRBLKDOESNOTEXIST) == false) return;
	// ]-EBATECH(watanabe)
	CDialog::OnOK();
}

BOOL svar_DrawingCfgDims::OnInitDialog() {
	int ret;

	if(m_bDimensionProps)
	{
		GetDlgItem(DIM_MAIN_NEW)->ShowWindow(SW_HIDE);
		GetDlgItem(DIM_MAIN_SAVE)->ShowWindow(SW_HIDE);
		GetDlgItem(DIM_MAIN_RENAME)->ShowWindow(SW_HIDE);
		GetDlgItem(DIM_MAIN_DELETE)->ShowWindow(SW_HIDE);
		GetDlgItem(DIM_MAIN_RESET)->ShowWindow(SW_HIDE);
	}

	svar_SubClassItems(this);
	ret=IcadDialog::OnInitDialog();

	ClearOverriding();
	CheckOverriding();

	dim_FillStyle();


	TC_ITEM TabCtrlItem;

	EditString	itemText;

	TabCtrlItem.mask=TCIF_TEXT;
	itemText = ResourceString( IDC_ICADSETVARSDIA_ARROWS_136, "Arrows");
	TabCtrlItem.pszText=itemText;
	m_CTabCtrl.InsertItem(0,&TabCtrlItem);

	itemText = ResourceString(IDC_ICADSETVARSDIA_FORMAT_137, "Format" );
	TabCtrlItem.pszText=itemText;
	m_CTabCtrl.InsertItem(1,&TabCtrlItem);

	itemText = ResourceString(IDC_ICADSETVARSDIA_LINES_138, "Lines" );
	TabCtrlItem.pszText=itemText;
	m_CTabCtrl.InsertItem(2,&TabCtrlItem);

	itemText = ResourceString(IDC_ICADSETVARSDIA_TEXT_139, "Text" );
	TabCtrlItem.pszText=itemText;
	m_CTabCtrl.InsertItem(3,&TabCtrlItem);

	itemText = ResourceString(IDC_ICADSETVARSDIA_TOL_185, "Tolerance" );
	TabCtrlItem.pszText=itemText;
	m_CTabCtrl.InsertItem(4,&TabCtrlItem);

	itemText = ResourceString(IDC_ICADSETVARSDIA_UNITS_140, "Units" );
	TabCtrlItem.pszText=itemText;
	m_CTabCtrl.InsertItem(5,&TabCtrlItem);

	itemText = ResourceString(IDC_ICADSETVARSDIA_ALT_186, "Alternate Units");
	TabCtrlItem.pszText=itemText;
	m_CTabCtrl.InsertItem(6,&TabCtrlItem);

	switch(dim_TabDialog) {
		case DIM_FORMAT:
			if (!m_DIM_FORMAT) m_DIM_FORMAT.Create(DIM_FORMAT,this);
			m_DIM_FORMAT.EnableWindow(TRUE);
			m_DIM_FORMAT.ShowWindow(SW_SHOWNORMAL);
			m_CurSel=1;
			m_CTabCtrl.SetCurSel(1);
			break;
		case DIM_LINES:
			if (!m_DIM_LINES) m_DIM_LINES.Create(DIM_LINES,this);
			m_DIM_LINES.EnableWindow(TRUE);
			m_DIM_LINES.ShowWindow(SW_SHOWNORMAL);
			m_CurSel=2;
			m_CTabCtrl.SetCurSel(2);
			break;
		case DIM_TEXT:
			if (!m_DIM_TEXT) m_DIM_TEXT.Create(DIM_TEXT,this);
			m_DIM_TEXT.EnableWindow(TRUE);
			m_DIM_TEXT.ShowWindow(SW_SHOWNORMAL);
			m_CurSel=3;
			m_CTabCtrl.SetCurSel(3);
			break;
		case DIM_TOL:
			if (!m_DIM_TOL) m_DIM_TOL.Create(DIM_TOL,this);
			m_DIM_TOL.EnableWindow(TRUE);
			m_DIM_TOL.ShowWindow(SW_SHOWNORMAL);
			m_CurSel=4;
			m_CTabCtrl.SetCurSel(4);
			break;
		case DIM_UNITS:
			if (!m_DIM_UNITS) m_DIM_UNITS.Create(DIM_UNITS,this);
			m_DIM_UNITS.EnableWindow(TRUE);
			m_DIM_UNITS.ShowWindow(SW_SHOWNORMAL);
			m_CurSel=5;
			m_CTabCtrl.SetCurSel(5);
			break;
		case DIM_ALT:
			if (!m_DIM_ALT) m_DIM_ALT.Create(DIM_ALT,this);
			m_DIM_ALT.EnableWindow(TRUE);
			m_DIM_ALT.ShowWindow(SW_SHOWNORMAL);
			m_CurSel=6;
			m_CTabCtrl.SetCurSel(6);
			break;
		default:
		case DIM_ARROWS:
			if (!m_DIM_ARROWS) m_DIM_ARROWS.Create(DIM_ARROWS,this);
			m_DIM_ARROWS.EnableWindow(TRUE);
			m_DIM_ARROWS.ShowWindow(SW_SHOWNORMAL);
			m_CurSel=0;
			m_CTabCtrl.SetCurSel(0);
			break;
	}
	return(ret);
}

struct svar_DlgItem* svar_DrawingCfgDims::GetPos(int nID)
{
	for(int index=0;index<=svar_num_of_vars;index++)
	{
		if( svar_DlgItemDims[index].nID == nID)
			return svar_DlgItemDims+index;
	}
	ASSERT(NULL);
	return NULL;
}

void svar_DrawingCfgDims::CheckOverriding()
{
	resbuf	*pRbStyle = NULL, *pRbStyleVar = NULL, *pRbSysVar = new resbuf;
	CString	Str;

	m_bDimStyleOverriden = false;

	SDS_StartTblRead("DIMSTYLE"/*DNT*/);
	while(SDS_TblReadHasMore())
	{
		Str = SDS_TblReadString();
		if(!csDimStyle.CompareNoCase(Str))
		{
			pRbStyle = sds_tblsearch("DIMSTYLE"/*DNT*/, Str, 0);
			for(pRbStyleVar = pRbStyle; pRbStyleVar != NULL; pRbStyleVar = pRbStyleVar->rbnext)
			{
				switch(pRbStyleVar->restype)
				{
					case 3:
						{
							sds_getvar("DIMPOST"/*DNT*/, pRbSysVar);
							CString	Str1 = pRbSysVar->resval.rstring,
									Str2 = pRbStyleVar->resval.rstring;
							if(Str1 != Str2)
							{
								Str1 = Str1.Left(Str1.Find('<'/*DNT*/));
								Str2 = Str2.Left(Str2.Find('<'/*DNT*/));
								if(Str1 != Str2)
									m_bDimStyleOverriden = GetPos(DIM_TEXT_DIMPOST1)->isDimVarOverriden = true;
								Str1 = pRbSysVar->resval.rstring;
								Str2 = pRbStyleVar->resval.rstring;
								Str1 = Str1.Right(Str1.GetLength() - Str1.Find('>'/*DNT*/) - 1);
								Str2 = Str2.Right(Str2.GetLength() - Str2.Find('>'/*DNT*/) - 1);
								if(Str1 != Str2)
									m_bDimStyleOverriden = GetPos(DIM_TEXT_DIMPOST2)->isDimVarOverriden = true;
							}
							IC_FREE(pRbSysVar->resval.rstring)
						}
						break;
					case 4:
						{
							sds_getvar("DIMAPOST"/*DNT*/, pRbSysVar);
							CString	Str1 = pRbSysVar->resval.rstring,
									Str2 = pRbStyleVar->resval.rstring;
							if(Str1 != Str2)
							{
								Str1 = Str1.Left(Str1.Find('['/*DNT*/));
								Str2 = Str2.Left(Str2.Find('['/*DNT*/));
								if(Str1 != Str2)
									m_bDimStyleOverriden = GetPos(DIM_ALT_DIMAPOST1)->isDimVarOverriden = true;
								Str1 = pRbSysVar->resval.rstring;
								Str2 = pRbStyleVar->resval.rstring;
								Str1 = Str1.Right(Str1.GetLength() - Str1.Find(']'/*DNT*/) - 1);
								Str2 = Str2.Right(Str2.GetLength() - Str2.Find(']'/*DNT*/) - 1);
								if(Str1 != Str2)
									m_bDimStyleOverriden = GetPos(DIM_ALT_DIMAPOST2)->isDimVarOverriden = true;
							}
							IC_FREE(pRbSysVar->resval.rstring)
						}
						break;
					case 5:
						sds_getvar("DIMBLK"/*DNT*/, pRbSysVar);
						if(strcmp(pRbSysVar->resval.rstring, pRbStyleVar->resval.rstring))
							m_bDimStyleOverriden = true;
						IC_FREE(pRbSysVar->resval.rstring)
						break;
					case 6:
						sds_getvar("DIMBLK1"/*DNT*/, pRbSysVar);
						if(strcmp(pRbSysVar->resval.rstring, pRbStyleVar->resval.rstring))
							m_bDimStyleOverriden = GetPos(DIM_ARROWS_START)->isDimVarOverriden = true;
						IC_FREE(pRbSysVar->resval.rstring)
						break;
					case 7:
						sds_getvar("DIMBLK2"/*DNT*/, pRbSysVar);
						if(strcmp(pRbSysVar->resval.rstring, pRbStyleVar->resval.rstring))
							m_bDimStyleOverriden = GetPos(DIM_ARROWS_END)->isDimVarOverriden = true;
						IC_FREE(pRbSysVar->resval.rstring)
						break;
					case 40:
						sds_getvar("DIMSCALE"/*DNT*/, pRbSysVar);
						if(pRbSysVar->resval.rreal != pRbStyleVar->resval.rreal)
							m_bDimStyleOverriden = GetPos(DIM_UNITS_DIMSCALE)->isDimVarOverriden = true;
						break;
					case 41:
						sds_getvar("DIMASZ"/*DNT*/, pRbSysVar);
						if(pRbSysVar->resval.rreal != pRbStyleVar->resval.rreal)
							m_bDimStyleOverriden = GetPos(DIM_ARROWS_DIMASZ)->isDimVarOverriden = true;
						break;
					case 42:
						sds_getvar("DIMEXO"/*DNT*/, pRbSysVar);
						if(pRbSysVar->resval.rreal != pRbStyleVar->resval.rreal)
							m_bDimStyleOverriden = GetPos(DIM_LINES_DIMEXO)->isDimVarOverriden = true;
						break;
					case 43:
						sds_getvar("DIMDLI"/*DNT*/, pRbSysVar);
						if(pRbSysVar->resval.rreal != pRbStyleVar->resval.rreal)
							m_bDimStyleOverriden = GetPos(DIM_LINES_DIMDLI)->isDimVarOverriden = true;
						break;
					case 44:
						sds_getvar("DIMEXE"/*DNT*/, pRbSysVar);
						if(pRbSysVar->resval.rreal != pRbStyleVar->resval.rreal)
							m_bDimStyleOverriden = GetPos(DIM_LINES_DIMEXE)->isDimVarOverriden = true;
						break;
					case 45:
						sds_getvar("DIMRND"/*DNT*/, pRbSysVar);
						if(pRbSysVar->resval.rreal != pRbStyleVar->resval.rreal)
							m_bDimStyleOverriden = GetPos(DIM_UNITS_DIMRND)->isDimVarOverriden = true;
						break;
					case 46:
						sds_getvar("DIMDLE"/*DNT*/, pRbSysVar);
						if(pRbSysVar->resval.rreal != pRbStyleVar->resval.rreal)
							m_bDimStyleOverriden = GetPos(DIM_LINES_DIMDLE)->isDimVarOverriden = true;
						break;
					case 47:
						sds_getvar("DIMTP"/*DNT*/, pRbSysVar);
						if(pRbSysVar->resval.rreal != pRbStyleVar->resval.rreal)
							m_bDimStyleOverriden = GetPos(DIM_TOL_DIMTP)->isDimVarOverriden = true;
						break;
					case 48:
						sds_getvar("DIMTM"/*DNT*/, pRbSysVar);
						if(pRbSysVar->resval.rreal != pRbStyleVar->resval.rreal)
							m_bDimStyleOverriden = GetPos(DIM_TOL_DIMTM)->isDimVarOverriden = true;
						break;
					case 140:
						sds_getvar("DIMTXT"/*DNT*/, pRbSysVar);
						if(pRbSysVar->resval.rreal != pRbStyleVar->resval.rreal)
							m_bDimStyleOverriden = GetPos(DIM_TEXT_DIMTXT)->isDimVarOverriden = true;
						break;
					case 141:
						sds_getvar("DIMCEN"/*DNT*/, pRbSysVar);
						if(pRbSysVar->resval.rreal != pRbStyleVar->resval.rreal)
							m_bDimStyleOverriden = GetPos(DIM_LINES_DIMCEN)->isDimVarOverriden = true;
						break;
					case 142:
						sds_getvar("DIMTSZ"/*DNT*/, pRbSysVar);
						if(pRbSysVar->resval.rreal != pRbStyleVar->resval.rreal)
							m_bDimStyleOverriden = GetPos(DIM_ARROWS_DIMTSZ)->isDimVarOverriden = true;
						break;
					case 143:
						sds_getvar("DIMALTF"/*DNT*/, pRbSysVar);
						if(pRbSysVar->resval.rreal != pRbStyleVar->resval.rreal)
							m_bDimStyleOverriden = GetPos(DIM_ALT_DIMALTF)->isDimVarOverriden = true;
						break;
					case 144:
						sds_getvar("DIMLFAC"/*DNT*/, pRbSysVar);
						if(pRbSysVar->resval.rreal != pRbStyleVar->resval.rreal)
							m_bDimStyleOverriden = GetPos(DIM_UNITS_DIMLFAC)->isDimVarOverriden = true;
						break;
					case 145:
						sds_getvar("DIMTVP"/*DNT*/, pRbSysVar);
						if(pRbSysVar->resval.rreal != pRbStyleVar->resval.rreal)
							m_bDimStyleOverriden = GetPos(DIM_TEXT_DIMTVP)->isDimVarOverriden = true;
						break;
					case 146:
						sds_getvar("DIMTFAC"/*DNT*/, pRbSysVar);
						if(pRbSysVar->resval.rreal != pRbStyleVar->resval.rreal)
							m_bDimStyleOverriden = GetPos(DIM_TOL_DIMTFAC)->isDimVarOverriden = true;
						break;
					case 147:
						sds_getvar("DIMGAP"/*DNT*/, pRbSysVar);
						if(pRbSysVar->resval.rreal != pRbStyleVar->resval.rreal)
							m_bDimStyleOverriden = GetPos(DIM_FORMAT_DIMGAP)->isDimVarOverriden = true;
						break;
					case 148:	// EBATECH(CNBR) 2003/3/31 Add DIMALTRND, DIMDSEP, DIMLWD, DIMLWE
						sds_getvar("DIMALTRND"/*DNT*/, pRbSysVar);
						if(pRbSysVar->resval.rreal != pRbStyleVar->resval.rreal)
							m_bDimStyleOverriden = GetPos(DIM_ALT_DIMALTRND)->isDimVarOverriden = true;
						break;
					case 71:
						sds_getvar("DIMTOL"/*DNT*/, pRbSysVar);
						if(pRbSysVar->resval.rint != pRbStyleVar->resval.rint)
							m_bDimStyleOverriden = GetPos(DIM_TOL_DIMTOL)->isDimVarOverriden = true;
						break;
					case 72:
						sds_getvar("DIMLIM"/*DNT*/, pRbSysVar);
						if(pRbSysVar->resval.rint != pRbStyleVar->resval.rint)
							m_bDimStyleOverriden = GetPos(DIM_TOL_DIMLIM)->isDimVarOverriden = true;
						break;
					case 73:
						sds_getvar("DIMTIH"/*DNT*/, pRbSysVar);
						if(pRbSysVar->resval.rint != pRbStyleVar->resval.rint)
							m_bDimStyleOverriden = GetPos(DIM_TEXT_DIMTIH)->isDimVarOverriden = true;
						break;
					case 74:
						sds_getvar("DIMTOH"/*DNT*/, pRbSysVar);
						if(pRbSysVar->resval.rint != pRbStyleVar->resval.rint)
							m_bDimStyleOverriden = GetPos(DIM_TEXT_DIMTOH)->isDimVarOverriden = true;
						break;
					case 75:
						sds_getvar("DIMSE1"/*DNT*/, pRbSysVar);
						if(pRbSysVar->resval.rint != pRbStyleVar->resval.rint)
							m_bDimStyleOverriden = GetPos(DIM_LINES_DIMSE1)->isDimVarOverriden = true;
						break;
					case 76:
						sds_getvar("DIMSE2"/*DNT*/, pRbSysVar);
						if(pRbSysVar->resval.rint != pRbStyleVar->resval.rint)
							m_bDimStyleOverriden = GetPos(DIM_LINES_DIMSE2)->isDimVarOverriden = true;
						break;
					case 77:
						sds_getvar("DIMTAD"/*DNT*/, pRbSysVar);
						if(pRbSysVar->resval.rint != pRbStyleVar->resval.rint)
							m_bDimStyleOverriden = GetPos(DIM_TEXT_DIMTAD)->isDimVarOverriden = true;
						break;
					case 78:
						sds_getvar("DIMZIN"/*DNT*/, pRbSysVar);
						if(pRbSysVar->resval.rint != pRbStyleVar->resval.rint)
						{
							int Tmp1 = pRbSysVar->resval.rint,
								Tmp2 = pRbStyleVar->resval.rint;
							switch(Tmp1)
							{
							/*DG - 30.11.2001*/// Fixed wrong processing of Tmp1/Tmp2 values.
							case  0:
								if(Tmp2 == 4 || Tmp2 == 5 || Tmp2 == 12 || Tmp2 == 15)
									GetPos(DIM_TOL_DIMTZIN)->isDimVarOverriden = true;
								if(Tmp2 == 8 || Tmp2 == 10 || Tmp2 == 12 || Tmp2 == 15)
									GetPos(DIM_TOL_DIMTZIN2)->isDimVarOverriden = true;
								break;
							case  4:
							case  5:
								if(Tmp2 == 0 || Tmp2 == 8 || Tmp2 == 10)
									GetPos(DIM_TOL_DIMTZIN)->isDimVarOverriden = true;
								if(Tmp2 == 8 || Tmp2 == 10 || Tmp2 == 12 || Tmp2 == 15)
									GetPos(DIM_TOL_DIMTZIN2)->isDimVarOverriden = true;
								break;
							case  8:
							case 10:
								if(Tmp2 == 4 || Tmp2 == 5 || Tmp2 == 12 || Tmp2 == 15)
									GetPos(DIM_TOL_DIMTZIN)->isDimVarOverriden = true;
								if(Tmp2 == 0 || Tmp2 == 4 || Tmp2 == 5)
									GetPos(DIM_TOL_DIMTZIN2)->isDimVarOverriden = true;
								break;
							case 12:
							case 15:
								if(Tmp2 == 0 || Tmp2 == 8 || Tmp2 == 10)
									GetPos(DIM_TOL_DIMTZIN)->isDimVarOverriden = true;
								if(Tmp2 == 0 || Tmp2 == 4 || Tmp2 == 5)
									GetPos(DIM_TOL_DIMTZIN2)->isDimVarOverriden = true;
							}
							m_bDimStyleOverriden = true;
						}
						break;
					case 79:	// EBATECH(CNBR) Bugzilla#78443 DIMADEC and DIMAZIN
						sds_getvar("DIMAZIN"/*DNT*/, pRbSysVar);
						if(pRbSysVar->resval.rint != pRbStyleVar->resval.rint)
							m_bDimStyleOverriden = GetPos(DIM_UNITS_DIMAZIN)->isDimVarOverriden = true;
						break;
					case 170:
						sds_getvar("DIMALT"/*DNT*/, pRbSysVar);
						if(pRbSysVar->resval.rint != pRbStyleVar->resval.rint)
							m_bDimStyleOverriden = GetPos(DIM_ALT_DIMALT)->isDimVarOverriden = true;
						break;
					case 171:
						sds_getvar("DIMALTD"/*DNT*/, pRbSysVar);
						if(pRbSysVar->resval.rint != pRbStyleVar->resval.rint)
							m_bDimStyleOverriden = GetPos(DIM_ALT_DIMALTD)->isDimVarOverriden = true;
						break;
					case 172:
						sds_getvar("DIMTOFL"/*DNT*/, pRbSysVar);
						if(pRbSysVar->resval.rint != pRbStyleVar->resval.rint)
							m_bDimStyleOverriden = GetPos(DIM_LINES_DIMTOFL)->isDimVarOverriden = true;
						break;
					case 173:
						sds_getvar("DIMSAH"/*DNT*/, pRbSysVar);
						if(pRbSysVar->resval.rint != pRbStyleVar->resval.rint)
							m_bDimStyleOverriden = GetPos(DIM_ARROWS_DIMSAH)->isDimVarOverriden = true;
						break;
					case 174:
						sds_getvar("DIMTIX"/*DNT*/, pRbSysVar);
						if(pRbSysVar->resval.rint != pRbStyleVar->resval.rint)
							m_bDimStyleOverriden = GetPos(DIM_TEXT_DIMTIX)->isDimVarOverriden = true;
						break;
					case 175:
						sds_getvar("DIMSOXD"/*DNT*/, pRbSysVar);
						if(pRbSysVar->resval.rint != pRbStyleVar->resval.rint)
							m_bDimStyleOverriden = GetPos(DIM_LINES_DIMSOXD)->isDimVarOverriden = true;
						break;
					case 176:
						sds_getvar("DIMCLRD"/*DNT*/, pRbSysVar);
						if(pRbSysVar->resval.rint != pRbStyleVar->resval.rint)
							m_bDimStyleOverriden = true;
						break;
					case 177:
						sds_getvar("DIMCLRE"/*DNT*/, pRbSysVar);
						if(pRbSysVar->resval.rint != pRbStyleVar->resval.rint)
							m_bDimStyleOverriden = true;
						break;
					case 178:
						sds_getvar("DIMCLRT"/*DNT*/, pRbSysVar);
						if(pRbSysVar->resval.rint != pRbStyleVar->resval.rint)
							m_bDimStyleOverriden = true;
						break;
					case 179:	// EBATECH(CNBR) Bugzilla#78443 DIMADEC and DIMAZIN
						sds_getvar("DIMADEC"/*DNT*/, pRbSysVar);
						if(pRbSysVar->resval.rint != pRbStyleVar->resval.rint)
							m_bDimStyleOverriden = GetPos(DIM_UNITS_DIMADEC)->isDimVarOverriden = true;
						break;
					case 270:
						sds_getvar("DIMUNIT"/*DNT*/, pRbSysVar);
						if(pRbSysVar->resval.rint != pRbStyleVar->resval.rint)
							m_bDimStyleOverriden = GetPos(DIM_UNITS_DIMUNIT)->isDimVarOverriden = true;
						break;
					case 271:
						sds_getvar("DIMDEC"/*DNT*/, pRbSysVar);
						if(pRbSysVar->resval.rint != pRbStyleVar->resval.rint)
							m_bDimStyleOverriden = GetPos(DIM_UNITS_DIMDEC)->isDimVarOverriden = true;
						break;
					case 272:
						sds_getvar("DIMTDEC"/*DNT*/, pRbSysVar);
						if(pRbSysVar->resval.rint != pRbStyleVar->resval.rint)
							m_bDimStyleOverriden = GetPos(DIM_TOL_DIMTDEC)->isDimVarOverriden = true;
						break;
					case 273:
						sds_getvar("DIMALTU"/*DNT*/, pRbSysVar);
						if(pRbSysVar->resval.rint != pRbStyleVar->resval.rint)
							m_bDimStyleOverriden = GetPos(DIM_ALT_DIMALTU)->isDimVarOverriden = true;
						break;
					case 274:
						sds_getvar("DIMALTTD"/*DNT*/, pRbSysVar);
						if(pRbSysVar->resval.rint != pRbStyleVar->resval.rint)
							m_bDimStyleOverriden = GetPos(DIM_ALT_DIMALTTD)->isDimVarOverriden = true;
						break;
					case 275:
						sds_getvar("DIMAUNIT"/*DNT*/, pRbSysVar);
						if(pRbSysVar->resval.rint != pRbStyleVar->resval.rint)
							m_bDimStyleOverriden = GetPos(DIM_UNITS_DIMAUNIT)->isDimVarOverriden = true;
						break;
					case 278:	// EBATECH(CNBR) 2003/3/31 Add DIMALTRND, DIMDSEP, DIMLWD, DIMLWE
								// NOTE: DIMDSEP is short in DIMSTYLE and XDATA, but SYSVAR is STRING.
						sds_getvar("DIMDSEP"/*DNT*/, pRbSysVar);
						if(pRbSysVar->resval.rstring[0] != pRbStyleVar->resval.rint)
							m_bDimStyleOverriden = GetPos(DIM_UNITS_DIMDSEP)->isDimVarOverriden = true;
						IC_FREE(pRbSysVar->resval.rstring)
						break;
					case 280:
						sds_getvar("DIMJUST"/*DNT*/, pRbSysVar);
						if(pRbSysVar->resval.rint != pRbStyleVar->resval.rint)
							m_bDimStyleOverriden = GetPos(DIM_TEXT_DIMJUST)->isDimVarOverriden = true;
						break;
					case 281:
						sds_getvar("DIMSD1"/*DNT*/, pRbSysVar);
						if(pRbSysVar->resval.rint != pRbStyleVar->resval.rint)
							m_bDimStyleOverriden = GetPos(DIM_LINES_DIMSD1)->isDimVarOverriden = true;
						break;
					case 282:
						sds_getvar("DIMSD2"/*DNT*/, pRbSysVar);
						if(pRbSysVar->resval.rint != pRbStyleVar->resval.rint)
							m_bDimStyleOverriden = GetPos(DIM_LINES_DIMSD2)->isDimVarOverriden = true;
						break;
					case 283:
						sds_getvar("DIMTOLJ"/*DNT*/, pRbSysVar);
						if(pRbSysVar->resval.rint != pRbStyleVar->resval.rint)
							m_bDimStyleOverriden = GetPos(DIM_TOL_DIMTOLJ)->isDimVarOverriden = true;
						break;
					case 284:
						sds_getvar("DIMTZIN"/*DNT*/, pRbSysVar);
						if(pRbSysVar->resval.rint != pRbStyleVar->resval.rint)
						{
							int Tmp1 = pRbSysVar->resval.rint,
								Tmp2 = pRbStyleVar->resval.rint;
							switch(Tmp1)
							{
							case  0:
								if(Tmp2 == 4 || Tmp2 == 5 || Tmp2 == 12 || Tmp2 == 15)
									GetPos(DIM_TOL_DIMTZIN)->isDimVarOverriden = true;
								if(Tmp2 == 8 || Tmp2 == 10 || Tmp2 == 12 || Tmp2 == 15)
									GetPos(DIM_TOL_DIMTZIN2)->isDimVarOverriden = true;
								break;
							case  4:
							case  5:
								if(Tmp2 == 0 || Tmp2 == 8 || Tmp2 == 10)
									GetPos(DIM_TOL_DIMTZIN)->isDimVarOverriden = true;
								if(Tmp2 == 8 || Tmp2 == 10 || Tmp2 == 12 || Tmp2 == 15)
									GetPos(DIM_TOL_DIMTZIN2)->isDimVarOverriden = true;
								break;
							case  8:
							case 10:
								if(Tmp2 == 4 || Tmp2 == 5 || Tmp2 == 12 || Tmp2 == 15)
									GetPos(DIM_TOL_DIMTZIN)->isDimVarOverriden = true;
								if(Tmp2 == 0 || Tmp2 == 4 || Tmp2 == 5)
									GetPos(DIM_TOL_DIMTZIN2)->isDimVarOverriden = true;
								break;
							case 12:
							case 15:
								if(Tmp2 == 0 || Tmp2 == 8 || Tmp2 == 10)
									GetPos(DIM_TOL_DIMTZIN)->isDimVarOverriden = true;
								if(Tmp2 == 0 || Tmp2 == 4 || Tmp2 == 5)
									GetPos(DIM_TOL_DIMTZIN2)->isDimVarOverriden = true;
							}
							m_bDimStyleOverriden = true;
						}
						break;
					case 285:
						sds_getvar("DIMALTZ"/*DNT*/, pRbSysVar);
						if(pRbSysVar->resval.rint != pRbStyleVar->resval.rint)
						{
							int Tmp1 = pRbSysVar->resval.rint,
								Tmp2 = pRbStyleVar->resval.rint;
							switch(Tmp1)
							{
							case  0:
								if(Tmp2 == 4 || Tmp2 == 5 || Tmp2 == 12 || Tmp2 == 15)
									GetPos(DIM_ALT_DIMALTZ1)->isDimVarOverriden = true;
								if(Tmp2 == 8 || Tmp2 == 10 || Tmp2 == 12 || Tmp2 == 15)
									GetPos(DIM_ALT_DIMALTZ2)->isDimVarOverriden = true;
								break;
							case  4:
							case  5:
								if(Tmp2 == 0 || Tmp2 == 8 || Tmp2 == 10)
									GetPos(DIM_ALT_DIMALTZ1)->isDimVarOverriden = true;
								if(Tmp2 == 8 || Tmp2 == 10 || Tmp2 == 12 || Tmp2 == 15)
									GetPos(DIM_ALT_DIMALTZ2)->isDimVarOverriden = true;
								break;
							case  8:
							case 10:
								if(Tmp2 == 4 || Tmp2 == 5 || Tmp2 == 12 || Tmp2 == 15)
									GetPos(DIM_ALT_DIMALTZ1)->isDimVarOverriden = true;
								if(Tmp2 == 0 || Tmp2 == 4 || Tmp2 == 5)
									GetPos(DIM_ALT_DIMALTZ2)->isDimVarOverriden = true;
								break;
							case 12:
							case 15:
								if(Tmp2 == 0 || Tmp2 == 8 || Tmp2 == 10)
									GetPos(DIM_ALT_DIMALTZ1)->isDimVarOverriden = true;
								if(Tmp2 == 0 || Tmp2 == 4 || Tmp2 == 5)
									GetPos(DIM_ALT_DIMALTZ2)->isDimVarOverriden = true;
							}
							m_bDimStyleOverriden = true;
						}
						break;
					case 286:
						sds_getvar("DIMALTTZ"/*DNT*/, pRbSysVar);
						if(pRbSysVar->resval.rint != pRbStyleVar->resval.rint)
						{
							int Tmp1 = pRbSysVar->resval.rint,
								Tmp2 = pRbStyleVar->resval.rint;
							switch(Tmp1)
							{
							case  0:
								if(Tmp2 == 4 || Tmp2 == 5 || Tmp2 == 12 || Tmp2 == 15)
									GetPos(DIM_ALT_DIMALTTZ1)->isDimVarOverriden = true;
								if(Tmp2 == 8 || Tmp2 == 10 || Tmp2 == 12 || Tmp2 == 15)
									GetPos(DIM_ALT_DIMALTTZ2)->isDimVarOverriden = true;
								break;
							case  4:
							case  5:
								if(Tmp2 == 0 || Tmp2 == 8 || Tmp2 == 10)
									GetPos(DIM_ALT_DIMALTTZ1)->isDimVarOverriden = true;
								if(Tmp2 == 8 || Tmp2 == 10 || Tmp2 == 12 || Tmp2 == 15)
									GetPos(DIM_ALT_DIMALTTZ2)->isDimVarOverriden = true;
								break;
							case  8:
							case 10:
								if(Tmp2 == 4 || Tmp2 == 5 || Tmp2 == 12 || Tmp2 == 15)
									GetPos(DIM_ALT_DIMALTTZ1)->isDimVarOverriden = true;
								if(Tmp2 == 0 || Tmp2 == 4 || Tmp2 == 5)
									GetPos(DIM_ALT_DIMALTTZ2)->isDimVarOverriden = true;
								break;
							case 12:
							case 15:
								if(Tmp2 == 0 || Tmp2 == 8 || Tmp2 == 10)
									GetPos(DIM_ALT_DIMALTTZ1)->isDimVarOverriden = true;
								if(Tmp2 == 0 || Tmp2 == 4 || Tmp2 == 5)
									GetPos(DIM_ALT_DIMALTTZ2)->isDimVarOverriden = true;
							}
							m_bDimStyleOverriden = true;
						}
						break;
					case 287:
						sds_getvar("DIMFIT"/*DNT*/, pRbSysVar);
						if(pRbSysVar->resval.rint != pRbStyleVar->resval.rint)
							m_bDimStyleOverriden											  =
							GetPos(DIM_FORMAT_DIMFIT0)[pRbSysVar->resval.rint].isDimVarOverriden	  =
							GetPos(DIM_FORMAT_DIMFIT0)[pRbStyleVar->resval.rint].isDimVarOverriden = true;
						break;
					case 288:
						sds_getvar("DIMUPT"/*DNT*/, pRbSysVar);
						if(pRbSysVar->resval.rint != pRbStyleVar->resval.rint)
							m_bDimStyleOverriden = true;
						break;
					case 340:
						sds_getvar("DIMTXSTY"/*DNT*/, pRbSysVar);
						{
							resbuf *pTmpRb1, *pTmpRb2;
							if((pTmpRb1 = sds_entget(pRbStyleVar->resval.rlname)) == NULL)
								break;
							for(pTmpRb2 = pTmpRb1; pTmpRb2 != NULL && pTmpRb2->restype != 2; pTmpRb2 = pTmpRb2->rbnext)
								;
							if(pTmpRb2 == NULL)
								break;
							if(strcmp(pRbSysVar->resval.rstring, pTmpRb2->resval.rstring))
								m_bDimStyleOverriden = GetPos(DIM_TEXT_DIMTXSTY)->isDimVarOverriden = true;
							IC_RELRB(pTmpRb1);
						}
						IC_FREE(pRbSysVar->resval.rstring)
						break;
					// EBATECH(watanabe)-[dimldrblk
					case 341:
						sds_getvar("DIMLDRBLK"/*DNT*/, pRbSysVar);

						// SystemMetrix(Hiro)-[Change the method of having data of DIMLDRBLK in DIMSTYLE and in EED
						//if(strcmp(pRbSysVar->resval.rstring, pRbStyleVar->resval.rstring))
						//	  m_bDimStyleOverriden = svar_DlgItemDims[67].isDimVarOverriden = true;
						{
							BOOL bIsDifferent = FALSE;
							resbuf *pTmpRb1 = sds_entget(pRbStyleVar->resval.rlname);
							if (pTmpRb1)
							  {
								resbuf *pTmpRb2 = pTmpRb1;
								while ((pTmpRb2 != NULL) && (pTmpRb2->restype != 2))
								  {
									pTmpRb2 = pTmpRb2->rbnext;
								  }

								if (pTmpRb2)
								  {
									if (stricmp(pRbSysVar->resval.rstring, pTmpRb2->resval.rstring))
									  {
										bIsDifferent = TRUE;
									  }
								  }
								IC_RELRB(pTmpRb1);
							  }
							else
							  {
								if (strlen(pRbSysVar->resval.rstring) > 0)
								  {
									bIsDifferent = TRUE;
								  }
							  }

							if (bIsDifferent)
							  {
								GetPos(LDR_ARROWS)->isDimVarOverriden = true;
								m_bDimStyleOverriden = true;
							  }
						}
						// ]-SystemMetrix(Hiro) Change the method of having data of DIMLDRBLK in DIMSTYLE and in EED

						IC_FREE(pRbSysVar->resval.rstring)
						break;
					// ]-EBATECH(watanabe)
					case 371:	// EBATECH(CNBR) 2003/3/31 Add DIMALTRND, DIMDSEP, DIMLWD, DIMLWE
						sds_getvar("DIMLWD"/*DNT*/, pRbSysVar);
						if(pRbSysVar->resval.rint != pRbStyleVar->resval.rint)
							m_bDimStyleOverriden = GetPos(DIM_LINES_DIMLWD)->isDimVarOverriden =true;
						break;
					case 372:	// EBATECH(CNBR) 2003/3/31 Add DIMALTRND, DIMDSEP, DIMLWD, DIMLWE
						sds_getvar("DIMLWE"/*DNT*/, pRbSysVar);
						if(pRbSysVar->resval.rint != pRbStyleVar->resval.rint)
							m_bDimStyleOverriden = GetPos(DIM_LINES_DIMLWE)->isDimVarOverriden =true;
						break;
				}
			}
			IC_RELRB(pRbStyle);
		}
	}
	IC_RELRB(pRbStyle);
	delete pRbSysVar;
}

void svar_DrawingCfgDims::ClearOverriding()
{
	resbuf rb;

	m_bDimStyleOverriden = false;

	for(int i = svar_num_of_vars; i--; )
		svar_DlgItemDims[i].isDimVarOverriden = false;

	m_DimStyle.DeleteString(m_DimStyle.FindString(-1, ResourceString(IDC_ICADSETVARSDIA_OVERRIDEN, "--- Overrides")));

	SDS_getvar(NULL, DB_QDIMSTYLE, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
	csDimStyle = rb.resval.rstring;
	if(rb.resval.rstring)
		IC_FREE(rb.resval.rstring);		

	m_DimStyle.SetCurSel(m_DimStyle.FindString(-1, csDimStyle));
}

void svar_DrawingCfgDims::dim_FillStyle() {
	CString csTmp;

	SDS_StartTblRead("DIMSTYLE"/*DNT*/);
	while(SDS_TblReadHasMore()) {
  		csTmp=SDS_TblReadString();
		//Bugzilla No. 78070 ; 17-06-2002
		//if(csTmp.Find('|'/*DNT*/)>0) continue;
		if(csTmp.Find(CMD_XREF_TBLDIVIDER)>0) continue;
		m_DimStyle.AddString(csTmp);
	}
	int CurSelIdx = m_DimStyle.SelectString(-1,(LPCTSTR)csDimStyle);

	if(m_bDimStyleOverriden)
	{
		m_DimStyle.InsertString(CurSelIdx + 1, ResourceString(IDC_ICADSETVARSDIA_OVERRIDEN, "--- Overrides"));
		m_DimStyle.SetCurSel(CurSelIdx + 1);
	}
}

void svar_DrawingCfgDims::OnDimChange() {

	resbuf	rb;
	bool	bNeedSetDimstyle = true;

	rb.restype = RTSTR;

	if(m_bDimStyleOverriden)
	{
		CString	strCurSel;
		m_DimStyle.GetLBText(m_DimStyle.GetCurSel(), strCurSel);
		if(strCurSel != ResourceString(IDC_ICADSETVARSDIA_OVERRIDEN, "--- Overrides"))
		{
			if(MessageBox(ResourceString(IDC_ICADSETVARSDIA_MESSAGE, "Setting another style current will discard your style overrides.\n\nContinue?"),
						  ResourceString(IDC_ICADSETVARSDIA_DIMENSI_134, "Dimension Style"),
						  MB_ICONEXCLAMATION|MB_OKCANCEL|MB_APPLMODAL) == IDOK)
			{
				rb.resval.rstring = new char [strCurSel.GetLength() + 1];
				strcpy(rb.resval.rstring, (LPCTSTR)strCurSel);
				sds_setvar("DIMSTYLE"/*DNT*/, &rb);
				IC_FREE(rb.resval.rstring);				
				bNeedSetDimstyle = false;

				ClearOverriding();
			}
			else
			{
				m_DimStyle.SetCurSel(m_DimStyle.FindString(-1, ResourceString(IDC_ICADSETVARSDIA_OVERRIDEN, "--- Overrides")));
				return;
			}
		}
	}

	UpdateData(TRUE);

	if(bNeedSetDimstyle)
	{
		rb.resval.rstring = new char [csDimStyle.GetLength() + 1];
		strcpy(rb.resval.rstring, (LPCTSTR)csDimStyle);
		sds_setvar("DIMSTYLE"/*DNT*/, &rb);
		IC_FREE(rb.resval.rstring);		
	}

	dim_SetDimValues();

	m_CTabCtrl.GetActiveWindow()->RedrawWindow(NULL, NULL, RDW_ERASE|RDW_INVALIDATE|RDW_INTERNALPAINT|RDW_ERASENOW|RDW_UPDATENOW|RDW_ALLCHILDREN);

	switch(m_CTabCtrl.GetCurSel()) {
		case 0:
			m_DIM_ARROWS.PaintPic();
			break;
		case 1:
			m_DIM_FORMAT.PaintPic();
			break;
		case 2:
			m_DIM_LINES.PaintPic();
			break;
		case 3:
			m_DIM_TEXT.PaintPic();
			break;
		case 4:
			m_DIM_TOL.PaintPic();
			break;
		case 5:
			m_DIM_UNITS.PaintPic();
			break;
		case 6:
			m_DIM_ALT.PaintPic();
			break;
	}
}

void svar_DrawingCfgDims::dim_SetDimValues()
{
	struct resbuf *pRbb=NULL,*pRb=NULL,*tmpRbb=NULL,*tmpRb=NULL;
	char *tmp;

	SDS_StartTblRead("DIMSTYLE"/*DNT*/);
	while(SDS_TblReadHasMore()) {
		tmp=SDS_TblReadString();
		if(!csDimStyle.CompareNoCase(tmp)) {
			pRbb=sds_tblsearch("DIMSTYLE"/*DNT*/,tmp,0);
			for(pRb=pRbb;pRb!=NULL;pRb=pRb->rbnext){
				switch(pRb->restype) {
					case 3:
						pRb->restype=RTSTR;
						sds_setvar("DIMPOST"/*DNT*/,pRb);
						break;
					case 4:
						pRb->restype=RTSTR;
						sds_setvar("DIMAPOST"/*DNT*/,pRb);
						break;
					case 5:
						pRb->restype=RTSTR;
						sds_setvar("DIMBLK"/*DNT*/,pRb);
						break;
					case 6:
						pRb->restype=RTSTR;
						sds_setvar("DIMBLK1"/*DNT*/,pRb);
						break;
					case 7:
						pRb->restype=RTSTR;
						sds_setvar("DIMBLK2"/*DNT*/,pRb);
						break;
					case 40:
						pRb->restype=RTREAL;
						sds_setvar("DIMSCALE"/*DNT*/,pRb);
						break;
					case 41:
						pRb->restype=RTREAL;
						sds_setvar("DIMASZ"/*DNT*/,pRb);
						break;
					case 42:
						pRb->restype=RTREAL;
						sds_setvar("DIMEXO"/*DNT*/,pRb);
						break;
					case 43:
						pRb->restype=RTREAL;
						sds_setvar("DIMDLI"/*DNT*/,pRb);
						break;
					case 44:
						pRb->restype=RTREAL;
						sds_setvar("DIMEXE"/*DNT*/,pRb);
						break;
					case 45:
						pRb->restype=RTREAL;
						sds_setvar("DIMRND"/*DNT*/,pRb);
						break;
					case 46:
						pRb->restype=RTREAL;
						sds_setvar("DIMDLE"/*DNT*/,pRb);
						break;
					case 47:
						pRb->restype=RTREAL;
						sds_setvar("DIMTP"/*DNT*/,pRb);
						break;
					case 48:
						pRb->restype=RTREAL;
						sds_setvar("DIMTM"/*DNT*/,pRb);
						break;
					case 140:
						pRb->restype=RTREAL;
						sds_setvar("DIMTXT"/*DNT*/,pRb);
						break;
					case 141:
						pRb->restype=RTREAL;
						sds_setvar("DIMCEN"/*DNT*/,pRb);
						break;
					case 142:
						pRb->restype=RTREAL;
						sds_setvar("DIMTSZ"/*DNT*/,pRb);
						break;
					case 143:
						pRb->restype=RTREAL;
						sds_setvar("DIMALTF"/*DNT*/,pRb);
						break;
					case 144:
						pRb->restype=RTREAL;
						sds_setvar("DIMLFAC"/*DNT*/,pRb);
						break;
					case 145:
						pRb->restype=RTREAL;
						sds_setvar("DIMTVP"/*DNT*/,pRb);
						break;
					case 146:
						pRb->restype=RTREAL;
						sds_setvar("DIMTFAC"/*DNT*/,pRb);
						break;
					case 147:
						pRb->restype=RTREAL;
						sds_setvar("DIMGAP"/*DNT*/,pRb);
						break;
					case 148:	// EBATECH(CNBR) 2003/3/31 DIMALTRND,DIMDSEP,DIMLWD,DIMLWE
						pRb->restype=RTREAL;
						sds_setvar("DIMALTRND"/*DNT*/,pRb);
						break;
					case 71:
						pRb->restype=RTSHORT;
						sds_setvar("DIMTOL"/*DNT*/,pRb);
						break;
					case 72:
						pRb->restype=RTSHORT;
						sds_setvar("DIMLIM"/*DNT*/,pRb);
						break;
					case 73:
						pRb->restype=RTSHORT;
						sds_setvar("DIMTIH"/*DNT*/,pRb);
						break;
					case 74:
						pRb->restype=RTSHORT;
						sds_setvar("DIMTOH"/*DNT*/,pRb);
						break;
					case 75:
						pRb->restype=RTSHORT;
						sds_setvar("DIMSE1"/*DNT*/,pRb);
						break;
					case 76:
						pRb->restype=RTSHORT;
						sds_setvar("DIMSE2"/*DNT*/,pRb);
						break;
					case 77:
						pRb->restype=RTSHORT;
						sds_setvar("DIMTAD"/*DNT*/,pRb);
						break;
					case 78:
						pRb->restype=RTSHORT;
						sds_setvar("DIMZIN"/*DNT*/,pRb);
						break;
					case 79:	// EBATECH(CNBR) -[ Bugzilla#78443 DIMADEC and DIMAZIN
						pRb->restype=RTSHORT;
						sds_setvar("DIMAZIN"/*DNT*/,pRb);
						break;	// EBATECH(CNBR) ]-
					case 170:
						pRb->restype=RTSHORT;
						sds_setvar("DIMALT"/*DNT*/,pRb);
						break;
					case 171:
						pRb->restype=RTSHORT;
						sds_setvar("DIMALTD"/*DNT*/,pRb);
						break;
					case 172:
						pRb->restype=RTSHORT;
						sds_setvar("DIMTOFL"/*DNT*/,pRb);
						break;
					case 173:
						pRb->restype=RTSHORT;
						sds_setvar("DIMSAH"/*DNT*/,pRb);
						break;
					case 174:
						pRb->restype=RTSHORT;
						sds_setvar("DIMTIX"/*DNT*/,pRb);
						break;
					case 175:
						pRb->restype=RTSHORT;
						sds_setvar("DIMSOXD"/*DNT*/,pRb);
						break;
					case 176:
						pRb->restype=RTSHORT;
						sds_setvar("DIMCLRD"/*DNT*/,pRb);
						break;
					case 177:
						pRb->restype=RTSHORT;
						sds_setvar("DIMCLRE"/*DNT*/,pRb);
						break;
					case 178:
						pRb->restype=RTSHORT;
						sds_setvar("DIMCLRT"/*DNT*/,pRb);
						break;
					case 179:	// EBATECH(CNBR) -[ Bugzilla#78443 DIMADEC and DIMAZIN
						pRb->restype=RTSHORT;
						sds_setvar("DIMADEC"/*DNT*/,pRb);
						break;	// EBATECH(CNBR) ]-
					case 270:
						pRb->restype=RTSHORT;
						sds_setvar("DIMUNIT"/*DNT*/,pRb);
						break;
					case 271:
						pRb->restype=RTSHORT;
						sds_setvar("DIMDEC"/*DNT*/,pRb);
						break;
					case 272:
						pRb->restype=RTSHORT;
						sds_setvar("DIMTDEC"/*DNT*/,pRb);
						break;
					case 273:
						pRb->restype=RTSHORT;
						sds_setvar("DIMALTU"/*DNT*/,pRb);
						break;
					case 274:
						pRb->restype=RTSHORT;
						sds_setvar("DIMALTTD"/*DNT*/,pRb);
						break;
					case 275:
						pRb->restype=RTSHORT;
						sds_setvar("DIMAUNIT"/*DNT*/,pRb);
						break;
					case 278:	// EBATECH(CNBR) 2003/3/31 DIMALTRND,DIMDSEP,DIMLWD,DIMLWE
						{		// NOTE: DIMDSEP is short in DIMSTYLE,XDATA but SYSVAR is STRING.
							char tmp[2];
							tmp[0] = pRb->resval.rint;
							tmp[1] = 0;
							tmpRbb = sds_buildlist( RTSTR, tmp, 0 );
							sds_setvar("DIMDSEP"/*DNT*/,tmpRbb);
							IC_RELRB(tmpRbb);
						}
						break;	// EBATECH(CNBR) 2003/3/31 DIMALTRND,DIMDSEP,DIMLWD,DIMLWE
					case 280:
						pRb->restype=RTSHORT;
						sds_setvar("DIMJUST"/*DNT*/,pRb);
						break;
					case 281:
						pRb->restype=RTSHORT;
						sds_setvar("DIMSD1"/*DNT*/,pRb);
						break;
					case 282:
						pRb->restype=RTSHORT;
						sds_setvar("DIMSD2"/*DNT*/,pRb);
						break;
					case 283:
						pRb->restype=RTSHORT;
						sds_setvar("DIMTOLJ"/*DNT*/,pRb);
						break;
					case 284:
						pRb->restype=RTSHORT;
						sds_setvar("DIMTZIN"/*DNT*/,pRb);
						break;
					case 285:
						pRb->restype=RTSHORT;
						sds_setvar("DIMALTZ"/*DNT*/,pRb);
						break;
					case 286:
						pRb->restype=RTSHORT;
						sds_setvar("DIMALTTZ"/*DNT*/,pRb);
						break;
					case 287:
						pRb->restype=RTSHORT;
						sds_setvar("DIMFIT"/*DNT*/,pRb);
						break;
					case 288:
						pRb->restype=RTSHORT;
						sds_setvar("DIMUPT"/*DNT*/,pRb);
						break;
					case 340:
						if((tmpRbb=sds_entget(pRb->resval.rlname))==NULL) break;
						for(tmpRb=tmpRbb;(tmpRb!=NULL)&&(tmpRb->restype!=2);tmpRb=tmpRb->rbnext);
						if(tmpRb==NULL) break;
						tmpRb->restype=RTSTR;
						sds_setvar("DIMTXSTY"/*DNT*/,tmpRb);
						IC_RELRB(tmpRbb);
						break;
					// EBATECH-[dimldrblk
					case 341:

					  // SystemMetrix(Hiro)-[Change the method of having data of DIMLDRBLK in DIMSTYLE and in EED
					  //  pRb->restype=RTSTR;
					  //  sds_setvar("DIMLDRBLK"/*DNT*/,pRb);
					  {
						BOOL bClosedFilled = TRUE;
						tmpRbb = sds_entget(pRb->resval.rlname);
						if (tmpRbb)
						  {
							tmpRb = tmpRbb;
							while ((tmpRb != NULL) && (tmpRb->restype != 2))
							  {
								tmpRb = tmpRb->rbnext;
							  }

							if (tmpRb)
							  {
								tmpRb->restype = RTSTR;
								sds_setvar("DIMLDRBLK"/*DNT*/, tmpRb);
								bClosedFilled = FALSE;
							  }
							IC_RELRB(tmpRbb);
						  }
						// if DIMLDRBLK is not set
						if (bClosedFilled)
						  {
							// set ClosedFilled
							tmpRbb = sds_buildlist(RTSTR, ""/*DNT*/, RTNONE);
							sds_setvar("DIMLDRBLK"/*DNT*/, tmpRbb);
							IC_RELRB(tmpRbb);
						  }
						break;
					  }
					  // ]-SystemMetrix(Hiro) Change the method of having data of DIMLDRBLK in DIMSTYLE and in EED
					case 371:	// EBATECH(CNBR) 2003/3/31 DIMALTRND,DIMDSEP,DIMLWD,DIMLWE
						pRb->restype=RTSHORT;
						sds_setvar("DIMLWD"/*DNT*/,pRb);
						break;
					case 372:	// EBATECH(CNBR) 2003/3/31 DIMALTRND,DIMDSEP,DIMLWD,DIMLWE
						pRb->restype=RTSHORT;
						sds_setvar("DIMLWE"/*DNT*/,pRb);
						break;

					// ]-EBATECH
				}
			}
			IC_RELRB(pRbb);
		}
	}
	IC_RELRB(pRbb);
	IC_RELRB(tmpRbb);
	// EBATECH(watanabe)-[dimldrblk
	////Modified Cybage AW 16-02-01 [
	//CComboBox *pCombo = (CComboBox *)GetDlgItem(LDR_ARROWS);
	//if(!pCombo)
	//	  return;
	//
	//int nIndex;
	//if((nIndex = pCombo->GetCurSel()) == CB_ERR)
	//	  return;
	//
	//CString CBLdrstr;
	//pCombo->GetLBText(nIndex, CBLdrstr);
	//
	//char *chBlkName = new char[512];
	//chBlkName = CBLdrstr.GetBuffer(CBLdrstr.GetLength());
	//
	//sds_name ename;
	//ename[1] = (long)SDS_CURDWG;
	//((db_drawing*)ename[1])->set_dimldrblk(chBlkName);
	//CBLdrstr.ReleaseBuffer();
	////Modified Cybage AW 16-02-01 ]

	// delete [] chBlkName;	/*D.G.*/// !!!
	// ]-EBATECH(watanabe)
}

void svar_DrawingCfgDims::OnRename() {
	struct resbuf *rb;
	char *szNew,*szOld;

	if(m_RenameStyle.DoModal()!=IDOK) return;
	m_RenameStyle.csNewStyleName.TrimLeft(); 
	m_RenameStyle.csNewStyleName.TrimRight(); 
	if(m_RenameStyle.csOldStyleName.IsEmpty()) return;
	if(m_RenameStyle.csNewStyleName.IsEmpty()) return;
	szNew=m_RenameStyle.csNewStyleName.GetBuffer(m_RenameStyle.csNewStyleName.GetLength()+1);
	szOld=m_RenameStyle.csOldStyleName.GetBuffer(m_RenameStyle.csOldStyleName.GetLength()+1);
	if((rb=sds_buildlist(RTDXF0,"DIMSTYLE"/*DNT*/,
							  2,szOld,
							  1,szNew,
							  0))==NULL) {
		cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
		return;
	}
	//Bugzilla No. 78070 ; 17-06-2002
	//SDS_tblmesser(rb,IC_TBLMESSER_RENAME,SDS_CURDWG);
	SDS_tblmesser(rb,IC_TBLMESSER_RENAME,SDS_CURDWG,true);
	if(rb) {sds_relrb(rb); rb=NULL;}
	csDimStyle=m_RenameStyle.csNewStyleName;
	m_DimStyle.ResetContent();
	svar_DrawingCfgDims::dim_FillStyle();
}
///////////////////////////////////////////
// I have subclassed IEXP_Newdlg here to override the OnInitDialog() and
// OnDblClk() for use within the DDIM dialog box.
class CDdim_IEXP_Newdlg : public IEXP_Newdlg {
	   protected:
		afx_msg BOOL OnInitDialog();
		afx_msg void OnDblClk();
	DECLARE_MESSAGE_MAP()
};
BEGIN_MESSAGE_MAP(CDdim_IEXP_Newdlg, IEXP_Newdlg)
	ON_LBN_DBLCLK(EXP_DEL_LIST,OnDblClk)
END_MESSAGE_MAP()

BOOL CDdim_IEXP_Newdlg::OnInitDialog()
	{
	RECT  rect;
	long  Width,
		  Height;

	CDialog::OnInitDialog();

	GetWindowRect(&rect);
	Width=rect.right-rect.left; Height=rect.bottom-rect.top;
	rect.top=m_top; rect.left=m_left; rect.bottom=m_top+Height; rect.right=m_left+Width;
	MoveWindow(&rect);

	char szTmp[IC_ACADBUF];
	sprintf(szTmp,ResourceString(IDC_ICADEXPLORER_NEW__S_143, "New %s"/*DNT*/ ),m_type);
	SetDlgItemText(EXP_DEL_LISTTXT,szTmp);

	m_ListWnd=GetDlgItem(EXP_DEL_LIST);

	struct resbuf *rb;
	rb=SDS_tblgetter("DIMSTYLE"/*DNT*/,NULL,TRUE,SDS_CURDWG);
	while(rb) {
		if (ic_assoc(rb,2))
			return(FALSE);
		if (!strisame(m_String,ic_rbassoc->resval.rstring)) {
			((CListBox *)m_ListWnd)->AddString(ic_rbassoc->resval.rstring);
		}
		sds_relrb(rb);
		rb=SDS_tblgetter("DIMSTYLE"/*DNT*/,NULL,FALSE,SDS_CURDWG);
	};

	return(TRUE);
	}

afx_msg void CDdim_IEXP_Newdlg::OnDblClk() {
	int Index;

	Index=((CListBox *)m_ListWnd)->GetCurSel();
	((CListBox *)m_ListWnd)->GetText(Index,m_String);

	m_state=TRUE;
	DestroyWindow();
	return;
}
///////////////////////////////////////////
// Here I have derived from IEXP_MessItem so I could override OnActivate()
// and OnButtonClick() of the Change... button
class CDdim_IEXP_MessItem : public IEXP_MessItem {
	public:
		CDdim_IEXP_Newdlg m_Ddim_Newdlg;
	protected:
		afx_msg void OnActivate(UINT nState,CWnd* pWndOther,BOOL bMinimized );
		afx_msg void OnButtonClicked();
	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CDdim_IEXP_MessItem, IEXP_MessItem)
	ON_WM_ACTIVATE()
	ON_BN_CLICKED(EXP_DEL_CHANGE, OnButtonClicked)
END_MESSAGE_MAP()

void CDdim_IEXP_MessItem::OnActivate(UINT nState,CWnd* pWndOther,BOOL bMinimized ) {
	CDialog::OnActivate(nState,pWndOther,bMinimized);

	if (nState==WA_ACTIVE && m_Ddim_Newdlg.m_state)
		EndDialog(EXP_DEL_CHANGE);
}

void CDdim_IEXP_MessItem::OnButtonClicked()
	{

	//Bugzilla No. 78163 ; 12-05-2003
	if(!m_Ddim_Newdlg )
	{
	CWnd *PicWnd = GetDlgItem(EXP_DEL_CHANGE);
	ASSERT(PicWnd != NULL);
	RECT rect;
	PicWnd->GetWindowRect(&rect);
	m_Ddim_Newdlg.m_top =rect.bottom;
	m_Ddim_Newdlg.m_left=rect.left;
	m_Ddim_Newdlg.Create(EXP_DEL_NEWLIST,this);
	m_Ddim_Newdlg.m_Dlgflag=TRUE;
	}
	m_Ddim_Newdlg.ShowWindow(SW_SHOWNORMAL);
	
	}

// NOTE: many string used below have not been localized, and are the same as
//		 many strings in icadexplorer.cpp which also has not been localized.
void svar_DrawingCfgDims::OnDelete() {
	TCHAR *szOld;
	CDdim_IEXP_MessItem dMessDlg;
	db_handitem* handItem=NULL;

	if(m_DeleteStyle.DoModal()!=IDOK) return;
	szOld=m_DeleteStyle.csStyleName.GetBuffer(m_DeleteStyle.csStyleName.GetLength()+1);

	int retvalue;
	struct sds_resbuf* rbb=NULL;
	sds_name dimStyleEname;
	TCHAR szTmp[IC_ACADBUF];
	TCHAR szStyleName[IC_ACADBUF];

	// EBATECH(CNBR) -[ 2003/3/31 Check this DIMSTYLE is used by any blocks.
	rbb=SDS_tblgetter("DIMSTYLE"/*DNT*/,szOld,TRUE,SDS_CURDWG);
	if( !rbb ) return;
	if (ic_assoc(rbb,2)){IC_RELRB(rbb) return;}
	_tcscpy(szStyleName, ic_rbassoc->resval.rstring);
	if (ic_assoc(rbb,-1)){IC_RELRB(rbb) return;}
	sds_name_set(ic_rbassoc->resval.rlname, dimStyleEname);
	IC_RELRB(rbb)

	//Bugzilla No. 78070 ; 17-06-2002
	//if (strisame(szStyleName,"STANDARD"/*DNT*/))
	/*	  {
		sprintf(szTmp,"Cannot delete dimension style %s.",szStyleName);
		AfxMessageBox(szTmp);
		return;
		}
	else
	if (strchr(szStyleName,'|'/*DNT* /))
		{
		AfxMessageBox("Cannot delete an XREF dimension style.");
		return;
	} else {*/
		dMessDlg.m_Ddim_Newdlg.m_state = FALSE;
		dMessDlg.m_Ddim_Newdlg.m_type="Dimension style";
		// Bugzilla No. 78215; 01/07/2002 [
		sprintf(szTmp,ResourceString(IDC_ICADEXPLORER_8, "Dimension style %s"),szStyleName);
		CString String=szTmp; SetWindowText(String);
		sprintf(szTmp,ResourceString(IDC_ICADEXPLORER_9, "You are about to delete the Dimension style %s. This will affect entities currently using this Dimension style."),szStyleName);
		dMessDlg.m_txtmes=szTmp;
		sprintf(szTmp,ResourceString(IDC_ICADEXPLORER_10, "Deletes all entities currently using Dimension style %s."),szStyleName);
		dMessDlg.m_txtcnt=szTmp;
		sprintf(szTmp,ResourceString(IDC_ICADEXPLORER_11, "Gives you a choice of Dimension styles to assign to the entities now using %s, then deletes the old Dimension style."),szStyleName);
		dMessDlg.m_txtchng=szTmp;
		sprintf(szTmp,ResourceString(IDC_ICADEXPLORER_12, "Cancels deletion of the Dimension style %s."),szStyleName);
		// Bugzilla No. 78215; 01/07/2002 ]
		dMessDlg.m_txtcancel=szTmp;
		dMessDlg.m_Ddim_Newdlg.m_String = szStyleName;
		//Bugzilla No. 78162 ; 20-05-2002
		sprintf(szTmp,ResourceString(IDC_ICADEXPLORER_DELETE__S_45, "Delete %s" ),szStyleName);
		dMessDlg.m_title=szTmp;
		if ((retvalue=dMessDlg.DoModal())==EXP_DEL_CANCEL )
			return;
	//}
	// Set up for undo.
	SDS_SetUndo(IC_UNDO_REGEN_ON_UNDO,NULL,NULL,NULL,SDS_CURDWG);

	sds_name ename;
	char* thisDimsStyle;
	ename[1] = (long)SDS_CURDWG;
	switch(retvalue)
	{
		case EXP_DEL_CONTINUE:
			{
			while((handItem=((db_drawing*)ename[1])->entnext_noxref(handItem))!=NULL)
				{
				// EBATECH(CNBR) -[ 2003/3/31 TOLERANCE and LEADER also uses DIMSTYLE.
				// We only care about dimensions
				//if (handItem->ret_type() == DB_DIMENSION)
				switch( handItem->ret_type() )
					{
				case DB_DIMENSION:
				case DB_TOLERANCE:
				case DB_LEADER:
					handItem->get_3(&thisDimsStyle);
					if(!strisame(szStyleName, thisDimsStyle)) continue;
					ename[0] = (long)handItem;
					sds_entdel(ename);
					}
				}
			// Now delete all the dimensions with this dimstyle, then
			// delete the style.
			// 2003/3/31 Remove dimstyle after deleting/renaming all rested block's entity.
			//sds_entdel(dimStyleEname);
				szStyleName[0] = 0;
			}
			break;
		case EXP_DEL_CHANGE:
			{
			while((handItem=((db_drawing*)ename[1])->entnext_noxref(handItem))!=NULL)
				{
				struct sds_resbuf* tmp;
				// EBATECH(CNBR) -[ 2003/3/31 TOLERANCE and LEADER also uses DIMSTYLE.
				// We only care about dimensions
				//if (handItem->ret_type() == DB_DIMENSION)
				switch( handItem->ret_type() )
					{
				case DB_DIMENSION:
				case DB_TOLERANCE:
				case DB_LEADER:
					handItem->get_3(&thisDimsStyle);
					if(!strisame(szStyleName, thisDimsStyle)) continue;
					ename[0] = (long)handItem;
					rbb = tmp = sds_entget(ename);
					while(tmp->restype != 3) tmp = tmp->rbnext;
					ASSERT(tmp != NULL);					
					tmp->resval.rstring=ic_realloc_char(tmp->resval.rstring, dMessDlg.m_Ddim_Newdlg.m_String.GetLength()+1);
					strcpy(tmp->resval.rstring,(LPCSTR)(dMessDlg.m_Ddim_Newdlg.m_String));
					sds_entmod(rbb);
					}
				}
			//sds_entdel(dimStyleEname); // 2003/3/31 Remove dimstyle after deleting/renaming all rested block's entity.
				strcpy(szStyleName, (LPCSTR)(dMessDlg.m_Ddim_Newdlg.m_String));
			}
			break;

	}

	// EBATECH(CNBR) -[ 2003/3/31 Corrupt the drawing for nested blocks which uses the dimension style.
	SDS_DontUpdateDisp = TRUE;
	CleanupEntsInsideBlocks(IEXP_DSTAB_IDX, szOld, retvalue, szStyleName, SDS_CURDWG);
	SDS_DontUpdateDisp = FALSE;
	sds_entdel(dimStyleEname);
	// EBATECH(CNBR) ]-

	//Bugzilla No. 78070 ; 17-06-2002 [
	m_DimStyle.ResetContent();
	svar_DrawingCfgDims::dim_FillStyle();
	m_DimStyle.SetCurSel(0);
	//]
	return;
}

void svar_DrawingCfgDims::OnSave() {
	UpdateData(TRUE);

	if(!m_bDimStyleOverriden)
		Save2Style(csDimStyle.GetBuffer(csDimStyle.GetLength() + 1), false);
	else
	{
		CString DimStyle;
		m_DimStyle.GetLBText(m_DimStyle.GetCurSel() - 1, DimStyle);
		Save2Style(DimStyle.GetBuffer(DimStyle.GetLength() + 1), false);
		ClearOverriding();
		m_CTabCtrl.GetActiveWindow()->InvalidateRect(NULL);
		m_CTabCtrl.GetActiveWindow()->SendMessage(WM_PAINT);
	}
}

void svar_DrawingCfgDims::OnNew() {
	struct resbuf rb;

	if(m_NewStyle.DoModal()!=IDOK) return;
	if(m_NewStyle.csStyleName.IsEmpty()) return;
	if(m_DimStyle.FindStringExact(-1,(LPCTSTR)m_NewStyle.csStyleName)!=CB_ERR) {
		MessageBox(ResourceString(IDC_ICADSETVARSDIA_STYLE_A_133, "Style already exists!" ),ResourceString(IDC_ICADSETVARSDIA_DIMENSI_134, "Dimension Style" ),MB_ICONEXCLAMATION|MB_OK|MB_APPLMODAL);
		return;
	}

	UpdateData(TRUE);

	Save2Style(m_NewStyle.csStyleName.GetBuffer(csDimStyle.GetLength()+1),true);

	m_DimStyle.AddString((LPCTSTR)m_NewStyle.csStyleName);

	if(!m_bDimStyleOverriden)
	{
		csDimStyle=m_NewStyle.csStyleName;
		m_DimStyle.SelectString(-1,(LPCTSTR)csDimStyle);

		rb.restype=RTSTR;
		rb.rbnext=NULL;
		rb.resval.rstring= new char [csDimStyle.GetLength()+1];
		strcpy(rb.resval.rstring,(LPCTSTR)csDimStyle);
		sds_setvar("DIMSTYLE"/*DNT*/,&rb);
		IC_FREE(rb.resval.rstring);
	}
}

void svar_DrawingCfgDims::Save2Style(char *pStyle,bool mode){
	// mode true=table make false=table mod

	char   *szDimPost=NULL,*szDimAPost=NULL,*szDimBlk=NULL,*szDimBlk1=NULL,*szDimBlk2=NULL;
	double rDimScale,rDimASZ,rDimEXO,rDimDLI,rDimEXE,rDimRND,rDimDLE,
		   rDimTP,rDimTM,rDimTXT,rDimCEN,rDimTSZ,rDimALTF,rDimLFAC,
		   rDimTVP,rDimTFAC,rDimGAP;
	short  sDimTOL,sDimLIM,sDimTIH,sDimTOH,sDimSE1,sDimSE2,sDimTAD,
		   sDimZIN,sDimALT,sDimALTD,sDimTOFL,sDimSAH,sDimTIX,sDimSOXD,sDimCLRD,
		   sDimCLRE,sDimCLRT,sDimUNIT,sDimDEC,sDimTDEC,sDimALTU,sDimALTTD,sDimAUNIT,
		   sDimJUST,sDimSD1,sDimSD2,sDimTOLJ,sDimTZIN,sDimALTZ,sDimALTTZ,sDimFIT,sDimUPT;
	short   sDimADEC, sDimAZIN;	// EBATECH(CNBR) Bugzilla#78443 DIMADEC and DIMAZIN
	double	rDimAltRnd; short	sDimDsep, sDimLwd, sDimLwe; // EBATECH(CNBR) 2003/3/31 Add this variable
	struct resbuf rb,*pRbb=NULL,*pRb;
	sds_name ename;
	short ret;

	// SystemMetrix(Hiro)-[Change the method of having data of DIMLDRBLK in DIMSTYLE and in EED
	sds_name eDimLdrBlk;
	sds_name_clear(eDimLdrBlk);
	SDS_getvar(NULL,DB_QDIMLDRBLK,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	pRbb = sds_tblsearch("BLOCK"/*DNT*/, rb.resval.rstring, 0);
	IC_FREE(rb.resval.rstring);
	pRb = pRbb;
	while ((pRb) && (pRb->restype != -1))
	  {
		pRb=pRb->rbnext;
	  }

	if (pRb)
	  {
		sds_name_set(pRb->resval.rlname, eDimLdrBlk);
	  }
	IC_RELRB(pRbb);
	// ]-SystemMetrix(Hiro) Change the method of having data of DIMLDRBLK in DIMSTYLE and in EED

	SDS_getvar(NULL,DB_QDIMPOST,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	szDimPost=rb.resval.rstring;
	SDS_getvar(NULL,DB_QDIMAPOST,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	szDimAPost=rb.resval.rstring;
	SDS_getvar(NULL,DB_QDIMBLK,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	szDimBlk=rb.resval.rstring;
	SDS_getvar(NULL,DB_QDIMBLK1,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	szDimBlk1=rb.resval.rstring;
	SDS_getvar(NULL,DB_QDIMBLK2,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	szDimBlk2=rb.resval.rstring;
	SDS_getvar(NULL,DB_QDIMTXSTY,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	pRbb=sds_tblsearch("STYLE"/*DNT*/,rb.resval.rstring,0);
	IC_FREE(rb.resval.rstring);
	for(pRb=pRbb;pRb!=NULL && pRb->restype!=-1;pRb=pRb->rbnext);
	if(pRb){
		ename[0]=pRb->resval.rlname[0];
		ename[1]=pRb->resval.rlname[1];
	}
	IC_RELRB(pRbb);
	SDS_getvar(NULL,DB_QDIMSCALE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	rDimScale=icadRealEqual(rb.resval.rreal,0.0) ? 1.0 : rb.resval.rreal;
	SDS_getvar(NULL,DB_QDIMASZ,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	rDimASZ=rb.resval.rreal;
	SDS_getvar(NULL,DB_QDIMEXO,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	rDimEXO=rb.resval.rreal;
	SDS_getvar(NULL,DB_QDIMDLI,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	rDimDLI=rb.resval.rreal;
	SDS_getvar(NULL,DB_QDIMEXE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	rDimEXE=rb.resval.rreal;
	SDS_getvar(NULL,DB_QDIMRND,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	rDimRND=rb.resval.rreal;
	SDS_getvar(NULL,DB_QDIMDLE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	rDimDLE=rb.resval.rreal;
	SDS_getvar(NULL,DB_QDIMTP,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	rDimTP=rb.resval.rreal;
	SDS_getvar(NULL,DB_QDIMTM,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	rDimTM=rb.resval.rreal;
	SDS_getvar(NULL,DB_QDIMTXT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	rDimTXT=rb.resval.rreal;
	SDS_getvar(NULL,DB_QDIMCEN,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	rDimCEN=rb.resval.rreal;
	SDS_getvar(NULL,DB_QDIMTSZ,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	rDimTSZ=rb.resval.rreal;
	SDS_getvar(NULL,DB_QDIMALTF,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	rDimALTF=rb.resval.rreal;
	SDS_getvar(NULL,DB_QDIMLFAC,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	rDimLFAC=rb.resval.rreal;
	SDS_getvar(NULL,DB_QDIMTVP,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	rDimTVP=rb.resval.rreal;
	SDS_getvar(NULL,DB_QDIMTFAC,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	rDimTFAC=rb.resval.rreal;
	SDS_getvar(NULL,DB_QDIMGAP,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	rDimGAP=rb.resval.rreal;
	SDS_getvar(NULL,DB_QDIMALTRND,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	rDimAltRnd=rb.resval.rreal;
	SDS_getvar(NULL,DB_QDIMTOL,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	sDimTOL=rb.resval.rint;
	SDS_getvar(NULL,DB_QDIMLIM,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	sDimLIM=rb.resval.rint;
	SDS_getvar(NULL,DB_QDIMTIH,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	sDimTIH=rb.resval.rint;
	SDS_getvar(NULL,DB_QDIMTOH,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	sDimTOH=rb.resval.rint;
	SDS_getvar(NULL,DB_QDIMSE1,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	sDimSE1=rb.resval.rint;
	SDS_getvar(NULL,DB_QDIMSE2,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	sDimSE2=rb.resval.rint;
	SDS_getvar(NULL,DB_QDIMTAD,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	sDimTAD=rb.resval.rint;
	SDS_getvar(NULL,DB_QDIMZIN,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	sDimZIN=rb.resval.rint;
	SDS_getvar(NULL,DB_QDIMALT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	sDimALT=rb.resval.rint;
	SDS_getvar(NULL,DB_QDIMALTD,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	sDimALTD=rb.resval.rint;
	SDS_getvar(NULL,DB_QDIMTOFL,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	sDimTOFL=rb.resval.rint;
	SDS_getvar(NULL,DB_QDIMSAH,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	sDimSAH=rb.resval.rint;
	SDS_getvar(NULL,DB_QDIMTIX,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	sDimTIX=rb.resval.rint;
	SDS_getvar(NULL,DB_QDIMSOXD,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	sDimSOXD=rb.resval.rint;
	SDS_getvar(NULL,DB_QDIMCLRD,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	sDimCLRD=rb.resval.rint;
	SDS_getvar(NULL,DB_QDIMCLRE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	sDimCLRE=rb.resval.rint;
	SDS_getvar(NULL,DB_QDIMCLRT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	sDimCLRT=rb.resval.rint;
	SDS_getvar(NULL,DB_QDIMUNIT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	sDimUNIT=rb.resval.rint;
	SDS_getvar(NULL,DB_QDIMDEC,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	sDimDEC=rb.resval.rint;
	SDS_getvar(NULL,DB_QDIMTDEC,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	sDimTDEC=rb.resval.rint;
	SDS_getvar(NULL,DB_QDIMALTU,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	sDimALTU=rb.resval.rint;
	SDS_getvar(NULL,DB_QDIMALTTD,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	sDimALTTD=rb.resval.rint;
	SDS_getvar(NULL,DB_QDIMAUNIT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	sDimAUNIT=rb.resval.rint;
	SDS_getvar(NULL,DB_QDIMDSEP,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if(rb.resval.rstring){
		sDimDsep=rb.resval.rstring[0];
		IC_FREE(rb.resval.rstring)
	}
	SDS_getvar(NULL,DB_QDIMJUST,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	sDimJUST=rb.resval.rint;
	SDS_getvar(NULL,DB_QDIMSD1,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	sDimSD1=rb.resval.rint;
	SDS_getvar(NULL,DB_QDIMSD2,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	sDimSD2=rb.resval.rint;
	SDS_getvar(NULL,DB_QDIMTOLJ,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	sDimTOLJ=rb.resval.rint;
	SDS_getvar(NULL,DB_QDIMTZIN,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	sDimTZIN=rb.resval.rint;
	SDS_getvar(NULL,DB_QDIMALTZ,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	sDimALTZ=rb.resval.rint;
	SDS_getvar(NULL,DB_QDIMALTTZ,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	sDimALTTZ=rb.resval.rint;
	SDS_getvar(NULL,DB_QDIMFIT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	sDimFIT=rb.resval.rint;
	SDS_getvar(NULL,DB_QDIMUPT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	sDimUPT=rb.resval.rint;
	// EBATECH(CNBR) -[ Bugzilla#78443 DIMADEC and DIMAZIN
	SDS_getvar(NULL, DB_QDIMADEC,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	sDimADEC=rb.resval.rint;
	SDS_getvar(NULL,DB_QDIMAZIN,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	sDimAZIN=rb.resval.rint;
	// EBATECH(CNBR) ]-
	SDS_getvar(NULL, DB_QDIMLWD,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	sDimLwd=rb.resval.rint;
	SDS_getvar(NULL, DB_QDIMLWE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	sDimLwe=rb.resval.rint;
	// Before we look up the arrowhead block in the block table,
	// make sure it is not an internal arrowhead type that gets
	// created on the fly.
	// EBATECH(watanabe)-[dimldrblk
	//checkBlock( szDimBlk, SDS_CURDWG ); // Create internal block on the fly
	//if ( szDimBlk[0] )
	//	  {
	//	  theblk=(db_blocktabrec *)SDS_CURDWG->findtabrec(DB_BLOCKTAB,szDimBlk,1);
	//	  if (theblk==NULL)
	//		  {
	//		  cmd_ErrorPrompt(CMD_ERR_DIMBLKDOESNOTEXIST,0);
	//		  goto bail;
	//		  }
	//	  }
	//checkBlock( szDimBlk1, SDS_CURDWG ); // Create internal block on the fly
	//if ( szDimBlk1[0] )
	//	  {
	//	  theblk=(db_blocktabrec *)SDS_CURDWG->findtabrec(DB_BLOCKTAB,szDimBlk1,1);
	//	  if (theblk==NULL)
	//		  {
	//		  cmd_ErrorPrompt(CMD_ERR_DIMBLK1DOESNOTEXIST,0);
	//		  goto bail;
	//		  }
	//	  }
	//checkBlock( szDimBlk2, SDS_CURDWG ); // Create internal block on the fly
	//if ( szDimBlk2[0] )
	//	  {
	//	  theblk=(db_blocktabrec *)SDS_CURDWG->findtabrec(DB_BLOCKTAB,szDimBlk2,1);
	//	  if (theblk==NULL)
	//		  {
	//		  cmd_ErrorPrompt(CMD_ERR_DIMBLK2DOESNOTEXIST,0);
	//		  goto bail;
	//		  }
	//	  }
	if (createArrowBlock(DB_QDIMBLK,	CMD_ERR_DIMBLKDOESNOTEXIST )   == false) goto bail;
	if (createArrowBlock(DB_QDIMBLK1,	CMD_ERR_DIMBLK1DOESNOTEXIST)   == false) goto bail;
	if (createArrowBlock(DB_QDIMBLK2,	CMD_ERR_DIMBLK2DOESNOTEXIST)   == false) goto bail;
	if (createArrowBlock(DB_QDIMLDRBLK, CMD_ERR_DIMLDRBLKDOESNOTEXIST) == false) goto bail;
	// ]-EBATECH(watanabe)

	if((pRbb=sds_buildlist(RTDXF0,"DIMSTYLE"/*DNT*/,
							   2,pStyle,
							  70,0,
							   3,szDimPost,
							   4,szDimAPost,
							   5,szDimBlk,
							   6,szDimBlk1,
							   7,szDimBlk2,
							  40,rDimScale,
							  41,rDimASZ,
							  42,rDimEXO,
							  43,rDimDLI,
							  44,rDimEXE,
							  45,rDimRND,
							  46,rDimDLE,
							  47,rDimTP,
							  48,rDimTM,
							 140,rDimTXT,
							 141,rDimCEN,
							 142,rDimTSZ,
							 143,rDimALTF,
							 144,rDimLFAC,
							 145,rDimTVP,
							 146,rDimTFAC,
							 147,rDimGAP,
							 148,rDimAltRnd,
							  71,sDimTOL,
							  72,sDimLIM,
							  73,sDimTIH,
							  74,sDimTOH,
							  75,sDimSE1,
							  76,sDimSE2,
							  77,sDimTAD,
							  78,sDimZIN,
							  79,sDimAZIN,	// Bugzilla#78443 DIMADEC and DIMAZIN
							 170,sDimALT,
							 171,sDimALTD,
							 172,sDimTOFL,
							 173,sDimSAH,
							 174,sDimTIX,
							 175,sDimSOXD,
							 176,sDimCLRD,
							 177,sDimCLRE,
							 178,sDimCLRT,
							 179,sDimADEC,	// Bugzilla#78443 DIMADEC and DIMAZIN
							  //**** r13
							 270,sDimUNIT,
							 271,sDimDEC,
							 272,sDimTDEC,
							 273,sDimALTU,
							 274,sDimALTTD,
							 275,sDimAUNIT,
							 278,sDimDsep,
							 280,sDimJUST,
							 281,sDimSD1,
							 282,sDimSD2,
							 283,sDimTOLJ,
							 284,sDimTZIN,
							 285,sDimALTZ,
							 286,sDimALTTZ,
							 287,sDimFIT,
							 288,sDimUPT,
							 340,ename,
							 // SystemMetrix(Hiro)-[Change the method of having data of DIMLDRBLK in DIMSTYLE and in EED
							 341,eDimLdrBlk,
							 // ]-SystemMetrix(Hiro) Change the method of having data of DIMLDRBLK in DIMSTYLE and in EED
							 371,sDimLwd,
							 372,sDimLwe,
								 0))==NULL){
		cmd_ErrorPrompt(CMD_ERR_MOREMEM,0);
		goto bail;
	}

	//put the table into the drawing
	ret=SDS_tblmesser(pRbb,(mode?IC_TBLMESSER_MAKE:IC_TBLMESSER_MODIFY),SDS_CURDWG);

bail:
	IC_RELRB(pRbb);
	// EBATECH(CNBR) -[ 2003/3/31 getvar()ed string buffer should uses free().
	IC_FREE(szDimPost)	//if(szDimPost)	delete szDimPost;
	IC_FREE(szDimAPost)	//if(szDimAPost)	delete szDimAPost;
	IC_FREE(szDimBlk)	//if(szDimBlk)	delete szDimBlk;
	IC_FREE(szDimBlk1)	//if(szDimBlk1)	delete szDimBlk1;
	IC_FREE(szDimBlk2)	//if(szDimBlk2)	delete szDimBlk2;
	// EBATECH(CNBR)
}

//This is for a tab change
void svar_DrawingCfgDims::OnTabChange(NMHDR *pNotifyStruct,LRESULT *result) {
	//Disable
	switch(m_CurSel) {
		case 0:
			m_DIM_ARROWS.EnableWindow(FALSE);
			m_DIM_ARROWS.ShowWindow(SW_HIDE);
			break;
		case 1:
			m_DIM_FORMAT.EnableWindow(FALSE);
			m_DIM_FORMAT.ShowWindow(SW_HIDE);
			break;
		case 2:
			m_DIM_LINES.EnableWindow(FALSE);
			m_DIM_LINES.ShowWindow(SW_HIDE);
			break;
		case 3:
			m_DIM_TEXT.EnableWindow(FALSE);
			m_DIM_TEXT.ShowWindow(SW_HIDE);
			break;
		case 4:
			m_DIM_TOL.EnableWindow(FALSE);
			m_DIM_TOL.ShowWindow(SW_HIDE);
			break;
		case 5:
			m_DIM_UNITS.EnableWindow(FALSE);
			m_DIM_UNITS.ShowWindow(SW_HIDE);
			break;
		case 6:
			m_DIM_ALT.EnableWindow(FALSE);
			m_DIM_ALT.ShowWindow(SW_HIDE);
			break;
	}
	//Enable
	m_CurSel=m_CTabCtrl.GetCurSel(); //Get the current selection
	switch(m_CurSel) {
		case 0:
			if (!m_DIM_ARROWS) m_DIM_ARROWS.Create(DIM_ARROWS,this);
			else							/*D.G.*/
				m_DIM_ARROWS.PaintPic();	// redraw a preview
			m_DIM_ARROWS.EnableWindow(TRUE);
			m_DIM_ARROWS.ShowWindow(SW_SHOWNORMAL);
			dim_TabDialog=DIM_ARROWS;
			break;
		case 1:
			if (!m_DIM_FORMAT) m_DIM_FORMAT.Create(DIM_FORMAT,this);
			else							/*D.G.*/
				m_DIM_FORMAT.PaintPic();	// redraw a preview
			m_DIM_FORMAT.EnableWindow(TRUE);
			m_DIM_FORMAT.ShowWindow(SW_SHOWNORMAL);
			dim_TabDialog=DIM_FORMAT;
			break;
		case 2:
			if (!m_DIM_LINES) m_DIM_LINES.Create(DIM_LINES,this);
			else							/*D.G.*/
				m_DIM_LINES.PaintPic();		// redraw a preview
			m_DIM_LINES.EnableWindow(TRUE);
			m_DIM_LINES.ShowWindow(SW_SHOWNORMAL);
			dim_TabDialog=DIM_LINES;
 			break;
		case 3:
			if (!m_DIM_TEXT) m_DIM_TEXT.Create(DIM_TEXT,this);
			else							/*D.G.*/
				m_DIM_TEXT.PaintPic();		// redraw a preview
			m_DIM_TEXT.EnableWindow(TRUE);
			m_DIM_TEXT.ShowWindow(SW_SHOWNORMAL);
			dim_TabDialog=DIM_TEXT;
			break;
		case 4:
			if (!m_DIM_TOL) m_DIM_TOL.Create(DIM_TOL,this);
			else							/*D.G.*/
				m_DIM_TOL.PaintPic();		// redraw a preview
			m_DIM_TOL.EnableWindow(TRUE);
			m_DIM_TOL.ShowWindow(SW_SHOWNORMAL);
			dim_TabDialog=DIM_TOL;
			break;
		case 5:
			if (!m_DIM_UNITS) m_DIM_UNITS.Create(DIM_UNITS,this);
			else							/*D.G.*/
				m_DIM_UNITS.PaintPic();		// redraw a preview
			m_DIM_UNITS.EnableWindow(TRUE);
			m_DIM_UNITS.ShowWindow(SW_SHOWNORMAL);
			dim_TabDialog=DIM_UNITS;
			break;
		case 6:
			if (!m_DIM_ALT) m_DIM_ALT.Create(DIM_ALT,this);
			else							/*D.G.*/
				m_DIM_ALT.PaintPic();		// redraw a preview
			m_DIM_ALT.EnableWindow(TRUE);
			m_DIM_ALT.ShowWindow(SW_SHOWNORMAL);
			dim_TabDialog=DIM_ALT;
			break;
	}
}

svar_DrawingCfgDims::svar_DrawingCfgDims(bool bDimensionProps /* = false*/) : IcadDialog(DIM_GENERAL) {
	struct resbuf rb;

	m_bDimensionProps = bDimensionProps;

	svar_iHitEnter=0;
	SDS_getvar(NULL,DB_QDIMSTYLE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	csDimStyle=rb.resval.rstring;
	if(rb.resval.rstring) {IC_FREE(rb.resval.rstring); rb.resval.rstring=NULL;}
}


//*************************************************************************************************
//this is called from the main
void cmd_setparlink(db_drawing* flp);
void cmd_geteedvalues(db_handitem* elp, db_drawing* flp);
void cmd_buildEEdList(resbuf* link, BOOL bReadDimStyle=FALSE);
//void cmd_buildEEdList(resbuf* link);
void cmd_dimSetVars();

void CMainWindow::IcadDimDia(bool bDimensionProps /*= false*/)
{
	extern cmd_elistll*	pDimItem;
	sds_name		dimEname;
	resbuf			rb,
					rbOldDimstyle, rbOldDimtxsty,
					rbOldDimadec;		//Bugzilla No. 78260; 26-08-2002
	db_drawing*		pDrw = SDS_CURDWG;

	if(!pDrw)
		return;

	pDrw->copyheader();
	SDS_CURSES.copybuf();

	/*DG - 5.12.2001*/// DIMSTYLE & DIMTXSTY are not stored in the header buffer.
	// So we will restore them personally.
	SDS_getvar(NULL, DB_QDIMSTYLE, &rbOldDimstyle, pDrw, &SDS_CURCFG, &SDS_CURSES);
	SDS_getvar(NULL, DB_QDIMTXSTY, &rbOldDimtxsty, pDrw, &SDS_CURCFG, &SDS_CURSES);

	svar_CurDlgItem = svar_DlgItemDims;											//set the pointer to the proper struct
	svar_num_of_vars = sizeof(svar_DlgItemDims) / sizeof(svar_DlgItemDims[0]);	//get the num in the struct

	if(bDimensionProps)
	{
		int			Saved_SDS_AtNodeDrag = SDS_AtNodeDrag,
					Save_UNDOCTL;
		resbuf		rbDimasoSaved;

		//Bugzilla No. 78260; 26-08-2002
		sds_getvar( "DIMADEC"/*DNT*/, &rbOldDimadec );

		// fill 'plinkvar'
		/*DG - 5.12.2001*/// DIMASO is stored in the header, but not in dimstyle.
		// So, save it's value, because cmd_dimSetVars (see below) will may change it.
		SDS_getvar(NULL, DB_QDIMASO, &rbDimasoSaved, pDrw, &SDS_CURCFG, &SDS_CURSES);
		sds_ssname(SDS_EditPropsSS, 0, dimEname);
		SDS_AtNodeDrag = 1;		// for taking values from dimstyle

		cmd_geteedvalues((db_handitem*)dimEname[0], pDrw);

		SDS_AtNodeDrag = Saved_SDS_AtNodeDrag;

		// copy 'plinkvar' to dim sys. vars
		SDS_getvar(NULL, DB_QUNDOCTL, &rb, pDrw, &SDS_CURCFG, &SDS_CURSES);
		Save_UNDOCTL = rb.resval.rint;
		rb.resval.rint &= ~IC_UNDOCTL_ON;
		SDS_setvar(NULL, DB_QUNDOCTL, &rb, pDrw, &SDS_CURCFG, &SDS_CURSES, 0);

		cmd_dimSetVars();

		SDS_setvar(NULL, DB_QDIMASO, &rbDimasoSaved, pDrw, &SDS_CURCFG, &SDS_CURSES, 0);

		rb.resval.rint = Save_UNDOCTL;
		SDS_setvar(NULL, DB_QUNDOCTL, &rb, pDrw, &SDS_CURCFG, &SDS_CURSES, 0);
	}

	svar_DrawingCfgDims		svar_DrawingDims(bDimensionProps);
	int						ret = svar_DrawingDims.DoModal();

	if(bDimensionProps)
	{
		if(ret == IDOK)
		{
			// copy dim sys. vars to 'plinkvar'
			cmd_setparlink(pDrw);
			// set dimstyle directly to the dimension resbufs list
			SDS_getvar(NULL, DB_QDIMSTYLE, &rb, pDrw, &SDS_CURCFG, &SDS_CURSES);
			if(!ic_assoc(pDimItem->entlst, 3))
			{
				ic_rbassoc->resval.rstring = ic_realloc_char(ic_rbassoc->resval.rstring, strlen(rb.resval.rstring) + 1);
				strcpy(ic_rbassoc->resval.rstring, rb.resval.rstring);
			}
			IC_FREE(rb.resval.rstring);
			// attach eed to the edited dimension resbuf list
			resbuf* prb;
			for(prb = pDimItem->entlst; prb->rbnext && prb->rbnext->restype != -3; prb = prb->rbnext)
				;
			if(prb->rbnext)
			{
				IC_RELRB(prb->rbnext);
				prb->rbnext = NULL;
			}
			cmd_buildEEdList(pDimItem->entlst);

			//Bugzilla No. 78260; 26-08-2002
			sds_setvar( "DIMADEC"/*DNT*/, &rbOldDimadec );

			// entmod the dimension
			((db_handitem*)(pDimItem->ename[0]))->clear_eed();
			sds_entmod(pDimItem->entlst);
		}

		// see notes about handseed below
		SDS_getvar(NULL, DB_QHANDSEED, &rb, pDrw, &SDS_CURCFG, &SDS_CURSES);
		pDrw->restoreheader();
		SDS_CURSES.restorebuf();
		SDS_setvar(NULL, DB_QHANDSEED, &rb, pDrw, &SDS_CURCFG, &SDS_CURSES, 0);
		IC_FREE(rb.resval.rstring);

		SDS_setvar(NULL, DB_QDIMSTYLE, &rbOldDimstyle, pDrw, &SDS_CURCFG, &SDS_CURSES, 0);
		SDS_setvar(NULL, DB_QDIMTXSTY, &rbOldDimtxsty, pDrw, &SDS_CURCFG, &SDS_CURSES, 0);

		//Bugzilla No. 78260; 26-08-2002
		sds_setvar( "DIMADEC"/*DNT*/, &rbOldDimadec );

	}
	else
	{
		if(ret == IDCANCEL && !svar_iHitEnter)
		{
			// preserve handseed, because if handseed has changed we need to keep that change,
			// because it is probably due to new dimstyles being added
			SDS_getvar(NULL, DB_QHANDSEED, &rb, pDrw, &SDS_CURCFG, &SDS_CURSES);
			pDrw->restoreheader();
			SDS_CURSES.restorebuf();
			SDS_setvar(NULL, DB_QHANDSEED, &rb, pDrw, &SDS_CURCFG, &SDS_CURSES, 0);
			IC_FREE(rb.resval.rstring);

			SDS_setvar(NULL, DB_QDIMSTYLE, &rbOldDimstyle, pDrw, &SDS_CURCFG, &SDS_CURSES, 0);
			SDS_setvar(NULL, DB_QDIMTXSTY, &rbOldDimtxsty, pDrw, &SDS_CURCFG, &SDS_CURSES, 0);

			//Modified Cybage AW 17-12-01 [
			//Reason : DIASTAT Variable functionality added
			//Bug no. 54233 from BugZilla
			struct resbuf rb1;
			rb1.restype = RTSHORT;
			rb1.resval.rint = 0;
			SDS_setvar(NULL,DB_QDIASTAT,&rb1,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
			//Modified Cybage AW 17-12-01 ]
		}
	}

	delete svar_DrawingDims.m_DIM_ALT.Bitmap;
	delete svar_DrawingDims.m_DIM_ARROWS.Bitmap;
	delete svar_DrawingDims.m_DIM_FORMAT.Bitmap;
	delete svar_DrawingDims.m_DIM_LINES.Bitmap;
	delete svar_DrawingDims.m_DIM_TEXT.Bitmap;
	delete svar_DrawingDims.m_DIM_TOL.Bitmap;
	delete svar_DrawingDims.m_DIM_UNITS.Bitmap;

	for(int i = svar_num_of_vars; i--; )
		svar_CurDlgItem[i].WndProc = NULL;

	pDrw->freeheaderbufcopy();
	SDS_CURSES.freebufcopy();
	IC_FREE(rbOldDimstyle.resval.rstring);
	IC_FREE(rbOldDimtxsty.resval.rstring);	
}

//--------------------------------------------------------------------

void svar_SubClassItems(CDialog *Cdlg) {
	_TCHAR fs1[81];
	CWnd   *hCurItem;
	int Wid,i;


	if((hCurItem=Cdlg->GetWindow(GW_CHILD))==NULL) return;
	do {
		GetClassName(hCurItem->m_hWnd,fs1,80);
		if (_tcsicmp(fs1,"Edit"/*DNT*/)==0) {
			Wid=GetWindowLong(hCurItem->m_hWnd,GWL_ID);
			for(i=0;i<svar_num_of_vars;i++) {
				if(svar_CurDlgItem[i].nID==Wid) {
					svar_CurDlgItem[i].WndProc=(WNDPROC)SetWindowLong(hCurItem->m_hWnd,GWL_WNDPROC,(LONG)svar_EditProc);
					break;
				}
			}
		} else if (_tcsicmp(fs1,"Button"/*DNT*/)==0) {
			Wid=GetWindowLong(hCurItem->m_hWnd,GWL_ID);
			for(i=0;i<svar_num_of_vars;i++) {
				if(svar_CurDlgItem[i].nID==Wid) {
					svar_CurDlgItem[i].WndProc=(WNDPROC)SetWindowLong(hCurItem->m_hWnd,GWL_WNDPROC,(LONG)svar_ButtonProc);
					break;
				}
			}
		} else if (_tcsicmp(fs1,"Static"/*DNT*/)==0) {
			Wid=GetWindowLong(hCurItem->m_hWnd,GWL_ID);
			for(i=0;i<svar_num_of_vars;i++) {
				if(svar_CurDlgItem[i].nID==Wid) {
					svar_CurDlgItem[i].WndProc=(WNDPROC)SetWindowLong(hCurItem->m_hWnd,GWL_WNDPROC,(LONG)svar_StaticProc);
					break;
				}
			}
		} else if (_tcsicmp(fs1,"ComboBox"/*DNT*/)==0) {
			Wid=GetWindowLong(hCurItem->m_hWnd,GWL_ID);
			for(i=0;i<svar_num_of_vars;i++) {
				if(svar_CurDlgItem[i].nID==Wid) {
					svar_CurDlgItem[i].WndProc=(WNDPROC)SetWindowLong(hCurItem->m_hWnd,GWL_WNDPROC,(LONG)svar_ComboProc);
					break;
				}
			}
		}
	} while((hCurItem=hCurItem->GetNextWindow(GW_HWNDNEXT))!=NULL);
	return;
}

// ComboBox()
LRESULT CALLBACK svar_ComboProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam) {
	LRESULT ret;
	HWND hItemWnd;
	TCHAR szTmp[IC_ACADBUF];
	int Wid,index,i;
	struct resbuf rb;
	struct svar_DlgItem *tmpDi=NULL;

	Wid=GetWindowLong(hWnd,GWL_ID);
	switch(uMsg) {
		case WM_KEYUP:
		case WM_DESTROY:
		case WM_WINDOWPOSCHANGING:
		case WM_LBUTTONUP:
			tmpDi=(struct svar_DlgItem *)GetWindowLong(hWnd,GWL_USERDATA);
			if(tmpDi==NULL) break;
			index=SendMessage(hWnd,CB_GETCURSEL,0,0);
			if(tmpDi->type == 9 && index == USER_ARROWS_SEPARATOR_INDEX)
			{
				::SendMessage(hWnd, CB_SHOWDROPDOWN, (WPARAM)TRUE, 0);
				::SendMessage(hWnd, CB_SETCURSEL, (WPARAM)(USER_ARROWS_SEPARATOR_INDEX + 1), 0);
				break;
			}
			sds_getvar(tmpDi->variable,&rb);
			switch(rb.restype) {
				case RTSTR:
					if(rb.resval.rstring) {IC_FREE(rb.resval.rstring); rb.resval.rstring=NULL;}
					if(tmpDi->type==9) {

						switch(index){
							case 0:
								strcpy(szTmp,"_NONE"/*DNT*/);
								break;
							case 1:
								strcpy(szTmp,"_CLOSED"/*DNT*/);
								break;
							case 2:
								strcpy(szTmp,"_DOT"/*DNT*/);
								break;
							case 3:
								strcpy(szTmp,""/*DNT*/);
								break;
							case 4:
								strcpy(szTmp,"_CLOSEDBLANK"/*DNT*/);
								break;
							case 5:
								strcpy(szTmp,"_OBLIQUE"/*DNT*/);
								break;
							case 6:
								strcpy(szTmp,"_ARCHTICK"/*DNT*/);
								break;
							case 7:
								strcpy(szTmp,"_OPEN"/*DNT*/);
								break;
							case 8:
								strcpy(szTmp,"_ORIGIN"/*DNT*/);
								break;
							case 9:
								strcpy(szTmp,"_OPEN90"/*DNT*/);
								break;
							case 10:
								strcpy(szTmp,"_OPEN30"/*DNT*/);
								break;
							case 11:
								strcpy(szTmp,"_DOTBLANK"/*DNT*/);
								break;
							case 12:
								strcpy(szTmp,"_DOTSMALL"/*DNT*/);
								break;
							case 13:
								strcpy(szTmp,"_BOXFILLED"/*DNT*/);
								break;
							case 14:
								strcpy(szTmp,"_BOXBLANK"/*DNT*/);
								break;
							case 15:
								strcpy(szTmp,"_DATUMFILLED"/*DNT*/);
								break;
							case 16:
								strcpy(szTmp,"_DATUMBLANK"/*DNT*/);
								break;
							case 17:
								strcpy(szTmp,"_INTEGRAL"/*DNT*/);
								break;
							case CB_ERR:
							default:
								GetWindowText(hWnd,szTmp,sizeof(szTmp)-1);
								break;
						}
						int	comboId = GetDlgCtrlID(hWnd);
						if(comboId != LDR_ARROWS && uMsg != WM_DESTROY)
						{
							rb.resval.rstring = szTmp;
							HWND	hOtherCombo = GetDlgItem(GetParent(hWnd), comboId == DIM_ARROWS_START ? DIM_ARROWS_END : DIM_ARROWS_START);
							resbuf	dimsahRb;
							sds_getvar("DIMSAH"/*DNT*/, &dimsahRb);
							if(dimsahRb.resval.rint)
							{
								if(index == SendMessage(hOtherCombo, CB_GETCURSEL, 0, 0))
								{
									sds_setvar("DIMBLK"/*DNT*/, &rb);
									dimsahRb.resval.rint = 0;
									sds_setvar("DIMSAH"/*DNT*/, &dimsahRb);
								}
								else
				                    			sds_setvar(tmpDi->variable, &rb);
							}
							else
							{
								if(comboId == DIM_ARROWS_START)
								{
									sds_setvar("DIMBLK"/*DNT*/, &rb);
									SendMessage(hOtherCombo, CB_SETCURSEL, (WPARAM)index, 0);
									SendMessage(GetDlgItem(GetParent(hWnd), DIM_ARROWS_PIC2), WM_PAINT, 0, 0);
								}
								else
								{
									sds_setvar(tmpDi->variable, &rb);
									dimsahRb.resval.rint = 1;
									sds_setvar("DIMSAH"/*DNT*/, &dimsahRb);
								}
							}
							break;
						}
					}else{
						SendMessage(hWnd,CB_GETLBTEXT,(WPARAM)index,(LPARAM)szTmp);
					}
					rb.resval.rstring=szTmp;
					sds_setvar(tmpDi->variable,&rb);
					break;
				case RTSHORT:
					if(tmpDi->type==10) { // For DIMLWD and DIMLWE
						rb.resval.rint=db_index2lweight(index-3);
					}else{				  // the Rest
						rb.resval.rint=index+tmpDi->Value;
					}
					sds_setvar(tmpDi->variable,&rb);
					break;
				case RTREAL:
					break;
				case RTPOINT:
				case RT3DPOINT:
					break;
			}

			// EBATECH(watanabe)-[comment dimldrblk
			//if(GetDlgCtrlID(hWnd) == LDR_ARROWS)
			//{
			//	  index = SendMessage(hWnd,CB_GETCURSEL,0,0);
			//	  switch(index)
			//	  {
			//		  case 0:
			//			  strcpy(szTmp,"_NONE"/*DNT*/);
			//			  break;
			//		  case 1:
			//			  strcpy(szTmp,"_CLOSED"/*DNT*/);
			//			  break;
			//		  case 2:
			//			  strcpy(szTmp,"_DOT"/*DNT*/);
			//			  break;
			//		  case 3:
			//			  strcpy(szTmp,""/*DNT*/);
			//			  break;
			//		  case 4:
			//			  strcpy(szTmp,"_CLOSEDBLANK"/*DNT*/);
			//			  break;
			//		  case 5:
			//			  strcpy(szTmp,"_OBLIQUE"/*DNT*/);
			//			  break;
			//		  case 6:
			//			  strcpy(szTmp,"_ARCHTICK"/*DNT*/);
			//			  break;
			//		  case 7:
			//			  strcpy(szTmp,"_OPEN"/*DNT*/);
			//			  break;
			//		  case 8:
			//			  strcpy(szTmp,"_ORIGIN"/*DNT*/);
			//			  break;
			//		  case 9:
			//			  strcpy(szTmp,"_OPEN90"/*DNT*/);
			//			  break;
			//		  case 10:
			//			  strcpy(szTmp,"_OPEN30"/*DNT*/);
			//			  break;
			//		  case 11:
			//			  strcpy(szTmp,"_DOTBLANK"/*DNT*/);
			//			  break;
			//		  case 12:
			//			  strcpy(szTmp,"_DOTSMALL"/*DNT*/);
			//			  break;
			//		  case 13:
			//			  strcpy(szTmp,"_BOXFILLED"/*DNT*/);
			//			  break;
			//		  case 14:
			//			  strcpy(szTmp,"_BOXBLANK"/*DNT*/);
			//			  break;
			//		  case 15:
			//			  strcpy(szTmp,"_DATUMFILLED"/*DNT*/);
			//			  break;
			//		  case 16:
			//			  strcpy(szTmp,"_DATUMBLANK"/*DNT*/);
			//			  break;
			//		  case 17:
			//			  strcpy(szTmp,"_INTEGRAL"/*DNT*/);
			//			  break;
			//		  case CB_ERR:
			//		  default:
			//			  GetWindowText(hWnd,szTmp,sizeof(szTmp)-1);
			//			  break;
			//	  }
			//	  sds_name ename;
			//	  ename[1] = (long)SDS_CURDWG;
			//	  ((db_drawing*)ename[1])->set_dimldrblk(szTmp);
			//}
			// ]-EBATECH(watanabe)

		case WM_PAINT:
			tmpDi=(struct svar_DlgItem *)GetWindowLong(hWnd,GWL_USERDATA);
			if(tmpDi==NULL){
				for(i=0;i<svar_num_of_vars;i++) {
					if(svar_CurDlgItem[i].nID==Wid) {
						tmpDi=&svar_CurDlgItem[i];
						SetWindowLong(hWnd,GWL_USERDATA,(long)tmpDi);
						break;
					}
				}
				if(tmpDi==NULL) break;
			 }
			sds_getvar(tmpDi->variable,&rb);
			switch(rb.restype) {
				case RTSTR:
					if(tmpDi->type==9) {

						/*D.G.*/// Set both values of comboboxes to DIMBLK's value if DIMSAH is OFF.
						resbuf		ResBuf;
						// EBATECH(watanabe)-[dimldrblk
						//sds_getvar("DIMSAH"/*DNT*/, &ResBuf);
						//if(!ResBuf.resval.rint)
						//{
						//	  IC_FREE(rb.resval.rstring)
						//	  sds_getvar("DIMBLK"/*DNT*/, &rb);
						//}
						if (GetDlgCtrlID(hWnd) != LDR_ARROWS) {
							sds_getvar("DIMSAH"/*DNT*/, &ResBuf);
							if(!ResBuf.resval.rint) {
								IC_FREE(rb.resval.rstring)
								sds_getvar("DIMBLK"/*DNT*/, &rb);
							}
						}
						// ]-EBATECH(watanabe)

						strcpy(szTmp,/*rb.resval.rstring=='_'/DNT/?rb.resval.rstring+1:*/rb.resval.rstring);

						if(strisame(ResourceString(IDC_ICADSETVARSDIA_NONE_162, "NONE" ),szTmp) ||
						   strisame("_NONE"/*DNT*/,szTmp) ) {
							SendMessage(hWnd,CB_SETCURSEL,(WPARAM)0,0);
						}else if(strisame(ResourceString(IDC_ICADSETVARSDIA_CLOSED_163, "CLOSED" ),szTmp) ||
						   strisame("_CLOSED"/*DNT*/,szTmp) ) {
							SendMessage(hWnd,CB_SETCURSEL,(WPARAM)1,0);
						}else if(strisame(ResourceString(IDC_ICADSETVARSDIA_DOT_164, "DOT" ),szTmp) ||
						   strisame("_DOT"/*DNT*/,szTmp) ) {
							SendMessage(hWnd,CB_SETCURSEL,(WPARAM)2,0);
						}else if(strisame(""/*DNT*/,szTmp)) {
							SendMessage(hWnd,CB_SETCURSEL,(WPARAM)3,0);
						}else if(strisame(ResourceString(IDC_ICADSETVARSDIA_CLOSEDB_165, "CLOSEDBLANK" ),szTmp) ||
						   strisame("_CLOSEDBLANK"/*DNT*/,szTmp) ) {
							SendMessage(hWnd,CB_SETCURSEL,(WPARAM)4,0);
						}else if(strisame(ResourceString(IDC_ICADSETVARSDIA_OBLIQUE_166, "OBLIQUE" ),szTmp) ||
						   strisame("_OBLIQUE"/*DNT*/,szTmp) ) {
							SendMessage(hWnd,CB_SETCURSEL,(WPARAM)5,0);
						}else if(strisame(ResourceString(IDC_ICADSETVARSDIA_ARCHTIC_167, "ARCHTICK" ),szTmp) ||
						   strisame("_ARCHTICK"/*DNT*/,szTmp) ) {
							SendMessage(hWnd,CB_SETCURSEL,(WPARAM)6,0);
						}else if(strisame(ResourceString(IDC_ICADSETVARSDIA_OPEN_168, "OPEN" ),szTmp) ||
						   strisame("_OPEN"/*DNT*/,szTmp) ) {
							SendMessage(hWnd,CB_SETCURSEL,(WPARAM)7,0);
						}else if(strisame(ResourceString(IDC_ICADSETVARSDIA_ORIGIN_169, "ORIGIN" ),szTmp) ||
						   strisame("_ORIGIN"/*DNT*/,szTmp) ) {
							SendMessage(hWnd,CB_SETCURSEL,(WPARAM)8,0);
						}else if(strisame(ResourceString(IDC_ICADSETVARSDIA_OPEN90_170, "OPEN90" ),szTmp) ||
						   strisame("_OPEN90"/*DNT*/,szTmp) ) {
							SendMessage(hWnd,CB_SETCURSEL,(WPARAM)9,0);
						}else if(strisame(ResourceString(IDC_ICADSETVARSDIA_OPEN30_171, "OPEN30" ),szTmp) ||
						   strisame("_OPEN30"/*DNT*/,szTmp) ) {
							SendMessage(hWnd,CB_SETCURSEL,(WPARAM)10,0);
						}else if(strisame(ResourceString(IDC_ICADSETVARSDIA_DOTBLAN_172, "DOTBLANK" ),szTmp) ||
						   strisame("_DOTBLANK"/*DNT*/,szTmp) ) {
							SendMessage(hWnd,CB_SETCURSEL,(WPARAM)11,0);
						}else if(strisame(ResourceString(IDC_ICADSETVARSDIA_DOTSMAL_173, "DOTSMALL" ),szTmp) ||
						   strisame("_DOTSMALL"/*DNT*/,szTmp) ) {
							SendMessage(hWnd,CB_SETCURSEL,(WPARAM)12,0);
						}else if(strisame(ResourceString(IDC_ICADSETVARSDIA_BOXFILL_174, "BOXFILLED" ),szTmp) ||
						   strisame("_BOXFILLED"/*DNT*/,szTmp) ) {
							SendMessage(hWnd,CB_SETCURSEL,(WPARAM)13,0);
						}else if(strisame(ResourceString(IDC_ICADSETVARSDIA_BOXBLAN_175, "BOXBLANK" ),szTmp) ||
						   strisame("_BOXBLANK"/*DNT*/,szTmp) ) {
							SendMessage(hWnd,CB_SETCURSEL,(WPARAM)14,0);
						}else if(strisame(ResourceString(IDC_ICADSETVARSDIA_DATUMFI_176, "DATUMFILLED" ),szTmp) ||
						   strisame("_DATUMFILLED"/*DNT*/,szTmp) ) {
							SendMessage(hWnd,CB_SETCURSEL,(WPARAM)15,0);
						}else if(strisame(ResourceString(IDC_ICADSETVARSDIA_DATUMBL_177, "DATUMBLANK" ),szTmp) ||
						   strisame("_DATUMBLANK"/*DNT*/,szTmp) ) {
							SendMessage(hWnd,CB_SETCURSEL,(WPARAM)16,0);
						}else if(strisame(ResourceString(IDC_ICADSETVARSDIA_INTEGRA_178, "INTEGRAL" ),szTmp) ||
						   strisame("_INTEGRAL"/*DNT*/,szTmp) ) {
							SendMessage(hWnd,CB_SETCURSEL,(WPARAM)17,0);
						}else{
							if (szTmp[0] == '_'/*DNT*/)
								SetWindowText(hWnd, szTmp + sizeof(TCHAR));
							else
								SendMessage(hWnd,
											CB_SETCURSEL,
											(WPARAM)SendMessage(hWnd, CB_FINDSTRINGEXACT, (WPARAM)USER_ARROWS_SEPARATOR_INDEX, (LPARAM)szTmp),
											0);

						}
					}else{
						index=SendMessage(hWnd,CB_FINDSTRINGEXACT,(WPARAM)-1,(LPARAM)rb.resval.rstring);
						SendMessage(hWnd,CB_SETCURSEL,(WPARAM)index,0);
					}
					if (rb.resval.rstring) {
						IC_FREE(rb.resval.rstring);
						rb.resval.rstring=NULL;
					}
					break;
				case RTSHORT:
					if(tmpDi->type==10) {
						SendMessage(hWnd,CB_SETCURSEL,(WPARAM)(db_lweight2index(rb.resval.rint)+3),0);
					} else {
						SendMessage(hWnd,CB_SETCURSEL,(WPARAM)rb.resval.rint-(tmpDi->Value),0);
					}
					break;
				case RTREAL:
					break;
				case RTPOINT:
				case RT3DPOINT:
					break;
			}

			//Modified Cybage AW 16-02-01 [
			if(GetDlgCtrlID(hWnd) == LDR_ARROWS)
			{
				hItemWnd=GetDlgItem(GetParent(hWnd),LDR_ARROWS_PIC);
				SendMessage(hItemWnd,WM_PAINT,0,0);
			}
			//Modified Cybage AW 16-02-01 ]
			if(tmpDi->UpdImage) {
				hItemWnd=GetDlgItem(GetParent(hWnd),tmpDi->UpdImage);
				SendMessage(hItemWnd,WM_PAINT,0,0);
			}
			break;
	}
	for(i=0;i<svar_num_of_vars;i++) {
		if(svar_CurDlgItem[i].nID==Wid) {
			ret=CallWindowProc((WNDPROC)(long (__stdcall *)(void))svar_CurDlgItem[i].WndProc,hWnd,uMsg,wParam,lParam);
			break;
		}
	}
	return(ret);
}

// ButtonProc()
LRESULT CALLBACK svar_ButtonProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam) {
	LRESULT ret;
	int Wid,i;
	struct resbuf rb;
	struct svar_DlgItem *tmpDi=NULL;
	HWND hItemWnd;
	bool bUpdate_AN_ANGBASE = false; 
	Wid=GetWindowLong(hWnd,GWL_ID);
	switch(uMsg) {
		case WM_LBUTTONDBLCLK:
		case WM_LBUTTONDOWN:
			tmpDi=(struct svar_DlgItem *)GetWindowLong(hWnd,GWL_USERDATA);
			if(tmpDi==NULL) break;
			sds_getvar(tmpDi->variable,&rb);
			if(tmpDi->type==1) {	// Radio button
				rb.resval.rint=tmpDi->Value;
			}else if(tmpDi->type==2) {
				if(!SendMessage(hWnd,BM_GETCHECK,0,0)) {
					rb.resval.rint|=tmpDi->Value;
				}else{
					rb.resval.rint&=(~tmpDi->Value);
				}
			}else if(tmpDi->type==4) {
				svar_StupidSet(tmpDi,hWnd,&rb.resval.rint);
			}
			sds_setvar(tmpDi->variable,&rb);
			//Update images if necessary
			if(tmpDi->UpdImage) {
				hItemWnd=GetDlgItem(GetParent(hWnd),tmpDi->UpdImage);
				SendMessage(hItemWnd,WM_PAINT,0,0);
			}
			// Update AN_ANGBASE edittext control
			if(tmpDi->nID == AN_ANGDIR1 || tmpDi->nID == AN_ANGDIR2)
				bUpdate_AN_ANGBASE = true;
			break;
		case WM_PAINT:
			tmpDi=(struct svar_DlgItem *)GetWindowLong(hWnd,GWL_USERDATA);
			if(tmpDi==NULL) {
				for(i=0;i<svar_num_of_vars;i++) {
					if(svar_CurDlgItem[i].nID==Wid) {
						tmpDi=&svar_CurDlgItem[i];
						break;
					}
				}
				if(tmpDi!=NULL) SetWindowLong(hWnd,GWL_USERDATA,(long)tmpDi);
				else break;
			}
			sds_getvar(tmpDi->variable,&rb);
			if(rb.restype!=RTSHORT) break;

			if(tmpDi->type==1) {	// Radio button
				if(rb.resval.rint==tmpDi->Value) {
					SendMessage((HWND)hWnd,BM_SETCHECK,(WPARAM)1,0);
				}else{
					SendMessage((HWND)hWnd,BM_SETCHECK,(WPARAM)0,0);
				}
			}else if(tmpDi->type==2) {
				SendMessage((HWND)hWnd,BM_SETCHECK,(WPARAM)((rb.resval.rint&tmpDi->Value)!=0),0);
			}
			break;
	}
	for(i=0;i<svar_num_of_vars;i++) {
		if(svar_CurDlgItem[i].nID==Wid) {
			ret=CallWindowProc((WNDPROC)(long (__stdcall *)(void))svar_CurDlgItem[i].WndProc,hWnd,uMsg,wParam,lParam);
			break;
		}
	}
	// Update AN_ANGBASE edittext control (when user select AN_ANGDIR1 or AN_ANGDIR2)
	if(bUpdate_AN_ANGBASE)
	{
		hItemWnd=GetDlgItem(GetParent(hWnd), AN_ANGBASE);
		if(hItemWnd)
			SendMessage(hItemWnd,WM_PAINT,0,0);
	}
	return(ret);
}

//Edit Box Proc
LRESULT CALLBACK svar_EditProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam) {
	LRESULT ret;
	char szTmp[256];
	int Wid,i;
	struct resbuf rb;
	struct svar_DlgItem *tmpDi=NULL;
	HWND hItemWnd;

	Wid=GetWindowLong(hWnd,GWL_ID);
	switch(uMsg) {
		case WM_KILLFOCUS:
			tmpDi=(struct svar_DlgItem *)GetWindowLong(hWnd,GWL_USERDATA);
			if(tmpDi==NULL) break;
			GetWindowText(hWnd,szTmp,sizeof(szTmp)-1);
			sds_getvar(tmpDi->variable,&rb);
			switch(rb.restype) {
				case RTSTR:
					if(tmpDi->type==4) {
						svar_StrSet(tmpDi,hWnd);
						if(rb.resval.rstring){IC_FREE(rb.resval.rstring);rb.resval.rstring=NULL;}
						break;
					}
					if(rb.resval.rstring){IC_FREE(rb.resval.rstring);rb.resval.rstring=NULL;}					
					rb.resval.rstring=szTmp;
					sds_setvar(tmpDi->variable,&rb);
					break;
				case RTSHORT:
					rb.resval.rint=atoi(szTmp);
					sds_setvar(tmpDi->variable,&rb);
					break;
				case RTREAL:
					switch(tmpDi->ResType) {
						case RTANG:
							if(Wid==AN_ANGBASE) {
//								  SDS_angtof_convert(1,szTmp,-1,&rb.resval.rreal);
								SDS_angtof_convert(5,szTmp,-1,&rb.resval.rreal);
							}else{
								sds_angtof(szTmp,-1,&rb.resval.rreal);
							}
							break;
						default:
							//special case for scales must always be in decimal format
							if(Wid==EC_CELTSCALE||Wid==EC_LTSCALE||Wid==DIM_TOL_DIMTFAC||Wid==DIM_UNITS_DIMSCALE/*||Wid==DIM_UNITS_DIMLFAC*/||Wid==DIM_ALT_DIMALTF){  //TODO FACETRES,CMLSCALE
								sds_distof(szTmp,2,&rb.resval.rreal);
							}else{
								sds_distof(szTmp,-1,&rb.resval.rreal);
							}
							if(Wid==EC_PDSIZE){
								if(rb.resval.rreal>0.0){
									SendMessage(GetDlgItem(GetParent(hWnd),EC_FIVE),BM_SETCHECK,BST_UNCHECKED,0);
									SendMessage(GetDlgItem(GetParent(hWnd),EC_ABSOLUTE),BM_SETCHECK,BST_CHECKED,0);
									SendMessage(GetDlgItem(GetParent(hWnd),EC_PERCENT),BM_SETCHECK,BST_UNCHECKED,0);
								}else if(rb.resval.rreal<0.0){
									SendMessage(GetDlgItem(GetParent(hWnd),EC_FIVE),BM_SETCHECK,BST_UNCHECKED,0);
									SendMessage(GetDlgItem(GetParent(hWnd),EC_ABSOLUTE),BM_SETCHECK,BST_UNCHECKED,0);
									SendMessage(GetDlgItem(GetParent(hWnd),EC_PERCENT),BM_SETCHECK,BST_CHECKED,0);
								}else{
									SendMessage(GetDlgItem(GetParent(hWnd),EC_FIVE),BM_SETCHECK,BST_CHECKED,0);
									SendMessage(GetDlgItem(GetParent(hWnd),EC_ABSOLUTE),BM_SETCHECK,BST_UNCHECKED,0);
									SendMessage(GetDlgItem(GetParent(hWnd),EC_PERCENT),BM_SETCHECK,BST_UNCHECKED,0);
								}
							}
							break;
					}
					sds_setvar(tmpDi->variable,&rb);
					break;
				case RTPOINT:
				case RT3DPOINT:
					sds_distof(szTmp,-1,&rb.resval.rpoint[tmpDi->Value]);
					switch(Wid){
						case CI_SNAPUNIT1:
							rb.resval.rpoint[1]=rb.resval.rpoint[0];
							SetWindowText(GetDlgItem(GetParent(hWnd),CI_SNAPUNIT2),szTmp);
							break;
						case CI_GRIDUNIT1:
							rb.resval.rpoint[1]=rb.resval.rpoint[0];
							SetWindowText(GetDlgItem(GetParent(hWnd),CI_GRIDUNIT2),szTmp);
							break;
					}
					sds_setvar(tmpDi->variable,&rb);
					break;
			}
			// Let it fall through to and repaint with the new setvar.
		case WM_PAINT:
			//Update images if necessary
			tmpDi=(struct svar_DlgItem *)GetWindowLong(hWnd,GWL_USERDATA);
			if(tmpDi && tmpDi->UpdImage) {
				hItemWnd=GetDlgItem(GetParent(hWnd),tmpDi->UpdImage);
				SendMessage(hItemWnd,WM_PAINT,0,0);
			}
			if(tmpDi==NULL) {
			   for(i=0;i<svar_num_of_vars;i++) {
					if(svar_CurDlgItem[i].nID==Wid) {
						tmpDi=&svar_CurDlgItem[i];
						break;
					}
				}
				if(tmpDi!=NULL) SetWindowLong(hWnd,GWL_USERDATA,(long)tmpDi);
				else break;
			}
			sds_getvar(tmpDi->variable,&rb);
			switch(rb.restype) {
				case RTSTR:
					if(tmpDi->type==4) {
						svar_StrGet(tmpDi,hWnd);
						if(rb.resval.rstring){IC_FREE(rb.resval.rstring);rb.resval.rstring=NULL;}
						break;
					}
					SetWindowText(hWnd,rb.resval.rstring);
					if(rb.resval.rstring){IC_FREE(rb.resval.rstring);rb.resval.rstring=NULL;}
					break;
				case RTSHORT:
					itoa(rb.resval.rint,szTmp,10);
					SetWindowText(hWnd,szTmp);
					break;
				case RTREAL:
					switch(tmpDi->ResType) {
						case RTANG:
							if(Wid==AN_ANGBASE) {
								resbuf rbDir;
								sds_getvar("ANGDIR",&rbDir);
								if(rbDir.resval.rint) 
									rb.resval.rreal = IC_TWOPI - rb.resval.rreal;

								sds_angtos_absolute(rb.resval.rreal,-1,-1,szTmp);
							}else{
								sds_angtos(rb.resval.rreal,-1,-1,szTmp);
							}
							break;
						default:
							//special case for scales must always be in decimal format
							if(Wid==EC_CELTSCALE||Wid==EC_LTSCALE||Wid==DIM_TOL_DIMTFAC||Wid==DIM_UNITS_DIMSCALE/*||Wid==DIM_UNITS_DIMLFAC*/||Wid==DIM_ALT_DIMALTF){  //TODO FACETRES,CMLSCALE
								sds_rtos(rb.resval.rreal,2,-1,szTmp);
							}else{
								sds_rtos(rb.resval.rreal,-1,-1,szTmp);
							}
							break;
					}
					SetWindowText(hWnd,szTmp);
					break;
				case RTPOINT:
				case RT3DPOINT:
					sds_rtos(rb.resval.rpoint[tmpDi->Value],-1,-1,szTmp);
					SetWindowText(hWnd,szTmp);
					break;
			}
			break;
	}
	for(i=0;i<svar_num_of_vars;i++) {
		if(svar_CurDlgItem[i].nID==Wid) {
			ret=CallWindowProc((WNDPROC)(long (__stdcall *)(void))svar_CurDlgItem[i].WndProc,hWnd,uMsg,wParam,lParam);
			break;
		}
	}
	return(ret);
}


//This will update the images
int svar_UpdatImage( LPCTSTR varname,int UpdImage,HWND hWnd) {
	HDC 	 hCmpDC,hDC;
	HBITMAP  hOldBitmap,hBitmap;
	RECT	 rect;
	CBrush brush;
	char szTmp[IC_ACADBUF];
	const double disp=32.33203126;
	struct resbuf rb;
	int cenpt[2],pt1,pt2,pt3,pt4,index;
	double angpt[2];
	short sTmp;
	sds_real ang;

	switch(UpdImage) {
		case AN_ANGDISPLAY:
			sds_angtos(disp,-1,-1,szTmp);
			SetWindowText(hWnd,szTmp);
			hWnd=GetParent(hWnd);
			hWnd=GetDlgItem(hWnd,AN_ANGBASE);
			SendMessage(hWnd,WM_PAINT,0,0);
			break;
		case AN_ANGDISPLAY2:
			GetClientRect(hWnd,&rect);
			hDC=GetDC(hWnd);
			cenpt[0]=(rect.right-rect.left)/2;
			cenpt[1]=(rect.bottom-rect.top)/2;
			pt1=cenpt[0]-30;
			pt2=cenpt[1]-30;
			pt3=cenpt[0]+30;
			pt4=cenpt[1]+30;
			::Ellipse(hDC,pt1,pt2,pt3,pt4); //This will draw the outer ring
			MoveToEx(hDC,pt1,cenpt[1],NULL); //this is the 9 o'clock tick
			LineTo(hDC,pt1+6,cenpt[1]);
			MoveToEx(hDC,cenpt[0],pt2,NULL); //this is the 12 o'clock tick
			LineTo(hDC,cenpt[0],pt2+6);
			MoveToEx(hDC,pt3,cenpt[1],NULL); //this is the 3 o'clock tick
			LineTo(hDC,pt3-6,cenpt[1]);
			MoveToEx(hDC,cenpt[0],pt4,NULL); //this is the 6 o'clock tick
			LineTo(hDC,cenpt[0],pt4-6);
			::Ellipse(hDC,cenpt[0]-2,cenpt[1]-2,cenpt[0]+2,cenpt[1]+2); //This will draw the center dot
			MoveToEx(hDC,cenpt[0],cenpt[1],NULL);

			ang = SDS_getvar(NULL,DB_QANGBASE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES) == RTNORM ? rb.resval.rreal : 0.;
			angpt[0]=cenpt[0]+(20*cos(ang));  //calc the angle
			angpt[1]=cenpt[1]-(20*sin(ang)); // ANGBASE: [0 - 2PI]
			angpt[0]+=0.5;
			angpt[1]+=0.5;
			if(icadRealEqual(cenpt[1],angpt[1]))angpt[1]=cenpt[1];
			LineTo(hDC,(int)angpt[0],(int)angpt[1]); //draw the angle
			ReleaseDC(hWnd,hDC);
			break;
		case AN_LINEARDISPLAY:
			sds_rtos(disp,-1,-1,szTmp);
			SetWindowText(hWnd,szTmp);
			break;
		case CI_ANGDISPLAY:
			GetClientRect(hWnd,&rect);
			hDC=GetDC(hWnd);
			cenpt[0]=(rect.right-rect.left)/2; //find the center point
			cenpt[1]=(rect.bottom-rect.top)/2;
			pt1=cenpt[0]-20; //define the size of the circle
			pt2=cenpt[1]-20;
			pt3=cenpt[0]+20;
			pt4=cenpt[1]+20;
			::Ellipse(hDC,pt1,pt2,pt3,pt4); //This will draw the outer ring
			MoveToEx(hDC,pt1,cenpt[1],NULL); //this is the 9 o'clock tick
			LineTo(hDC,pt1+6,cenpt[1]);
			MoveToEx(hDC,cenpt[0],pt2,NULL); //this is the 12 o'clock tick
			LineTo(hDC,cenpt[0],pt2+6);
			MoveToEx(hDC,pt3,cenpt[1],NULL); //this is the 3 o'clock tick
			LineTo(hDC,pt3-6,cenpt[1]);
			MoveToEx(hDC,cenpt[0],pt4,NULL); //this is the 6 o'clock tick
			LineTo(hDC,cenpt[0],pt4-6);
			::Ellipse(hDC,cenpt[0]-2,cenpt[1]-2,cenpt[0]+2,cenpt[1]+2); //This will draw the center dot
			MoveToEx(hDC,cenpt[0],cenpt[1],NULL);
			
			ang = SDS_getvar(NULL,DB_QSNAPANG,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES) == RTNORM ? rb.resval.rreal : 0.;
			angpt[0]=cenpt[0]+(10*cos(ang));  //calc the angle
			SDS_getvar(NULL,DB_QANGDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES); //if the angle is counterclockwise reverse the points
			if(rb.resval.rint){
				angpt[1]=cenpt[1]+(10*sin(ang));
			}else{
				angpt[1]=cenpt[1]-(10*sin(ang));
			}
			angpt[0]+=0.5;
			angpt[1]+=0.5;
			LineTo(hDC,(int)angpt[0],(int)angpt[1]); //draw the angle
			ReleaseDC(hWnd,hDC);
			break;
		case CI_PBSIZE2:  //This will draw a centered square
			{
				SDS_getvar(NULL,DB_QPICKBOX,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				if(rb.resval.rint>25) rb.resval.rint=25;
				GetClientRect(hWnd,&rect);
				hDC=GetDC(hWnd);
				HPALETTE savpal=::SelectPalette(hDC,(HPALETTE)SDS_CMainWindow->m_pPalette->GetSafeHandle(),TRUE);
				brush.CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
				rect.top+=3;rect.bottom-=3;rect.left+=3;rect.right-=3;
				FillRect(hDC,&rect,(HBRUSH)brush);
				rect.top-=3;rect.bottom+=3;rect.left-=3;rect.right+=3;
				brush.DeleteObject();

				cenpt[0]=(rect.right-rect.left)/2;
				cenpt[1]=(rect.bottom-rect.top)/2;
				rect.top=cenpt[1]-(rb.resval.rint);
				rect.bottom=cenpt[1]+(rb.resval.rint);
				rect.left=cenpt[0]-(rb.resval.rint);
				rect.right=cenpt[0]+(rb.resval.rint);

				MoveToEx(hDC,rect.left,rect.bottom,NULL);
				LineTo(hDC,rect.left,rect.top);
				MoveToEx(hDC,rect.left,rect.top,NULL);
				LineTo(hDC,rect.right,rect.top);
				MoveToEx(hDC,rect.right,rect.top,NULL);
				LineTo(hDC,rect.right,rect.bottom);
				MoveToEx(hDC,rect.right,rect.bottom,NULL);
				LineTo(hDC,rect.left,rect.bottom);

//				::SelectPalette(hDC,savpal,TRUE);
				ReleaseDC(hWnd,hDC);
				break;
			}
		case CI_APERTURE2:	//This will draw a centered square
			{
				SDS_getvar(NULL,DB_QAPERTURE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				if(rb.resval.rint>25) rb.resval.rint=25;
				GetClientRect(hWnd,&rect);
				hDC=GetDC(hWnd);
				HPALETTE savpal=::SelectPalette(hDC,(HPALETTE)SDS_CMainWindow->m_pPalette->GetSafeHandle(),TRUE);
				brush.CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
				rect.top+=3;rect.bottom-=3;rect.left+=3;rect.right-=3;
				FillRect(hDC,&rect,(HBRUSH)brush);
				rect.top-=3;rect.bottom+=3;rect.left-=3;rect.right+=3;
				brush.DeleteObject();

				cenpt[0]=(rect.right-rect.left)/2;
				cenpt[1]=(rect.bottom-rect.top)/2;
				rect.top=cenpt[1]-(rb.resval.rint);
				rect.bottom=cenpt[1]+(rb.resval.rint);
				rect.left=cenpt[0]-(rb.resval.rint);
				rect.right=cenpt[0]+(rb.resval.rint);

				MoveToEx(hDC,rect.left,rect.bottom,NULL);
				LineTo(hDC,rect.left,rect.top);
				MoveToEx(hDC,rect.left,rect.top,NULL);
				LineTo(hDC,rect.right,rect.top);
				MoveToEx(hDC,rect.right,rect.top,NULL);
				LineTo(hDC,rect.right,rect.bottom);
				MoveToEx(hDC,rect.right,rect.bottom,NULL);
				LineTo(hDC,rect.left,rect.bottom);

//				::SelectPalette(hDC,savpal,TRUE);
				ReleaseDC(hWnd,hDC);
				break;
			}
		case DV_NODEDISPLAY:  //This will draw a centered square
			{
				SDS_getvar(NULL,DB_QGRIPSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				int fi1=rb.resval.rint;
				struct resbuf gripcolor;
				SDS_getvar(NULL,DB_QGRIPCOLOR,&gripcolor,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				if(fi1>20) fi1=20;
				GetClientRect(hWnd,&rect);
				hDC=GetDC(hWnd);
				HPALETTE savpal=::SelectPalette(hDC,(HPALETTE)SDS_CMainWindow->m_pPalette->GetSafeHandle(),TRUE);
				brush.CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
				rect.top+=3;rect.bottom-=3;rect.left+=3;rect.right-=3;
				FillRect(hDC,&rect,(HBRUSH)brush);
				rect.top-=3;rect.bottom+=3;rect.left-=3;rect.right+=3;
				brush.DeleteObject();

				cenpt[0]=(rect.right-rect.left)/2;
				cenpt[1]=(rect.bottom-rect.top)/2;
				rect.top=cenpt[1]-(fi1);
				rect.bottom=cenpt[1]+(fi1);
				rect.left=cenpt[0]-(fi1);
				rect.right=cenpt[0]+(fi1);

				brush.CreateSolidBrush(SDS_BrushColorFromACADColor(gripcolor.resval.rint));
				FillRect(hDC,&rect,(HBRUSH)brush);
				if(rb.resval.rint==1) {
					SetPixel(hDC,cenpt[0],cenpt[1],SDS_PenColorFromACADColor(gripcolor.resval.rint));
				} else if(rb.resval.rint>20) {
					HGDIOBJ SavObj1=SelectObject(hDC,CreatePen(PS_DOT,1,SDS_PenColorFromACADColor(-1)));
					HGDIOBJ SavObj2=SelectObject(hDC,(HBRUSH)brush);
					Rectangle(hDC,rect.left,rect.top,rect.right,rect.bottom);
					Rectangle(hDC,rect.left+2,rect.top+2,rect.right-2,rect.bottom-2);
					DeleteObject(SelectObject(hDC,SavObj1));
					SelectObject(hDC,SavObj2);
				}
				brush.DeleteObject();
//				savpal=::SelectPalette(hDC,savpal,TRUE);
				ReleaseDC(hWnd,hDC);
				break;
			}
		case DV_NONSELCOLOR:
		case DV_SELCOLOR:
		   sds_getvar(varname,&rb);
			if(rb.resval.rint==0) {
				hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(EC_COLOR_BMP));
				hDC=GetDC(hWnd);
				hCmpDC=CreateCompatibleDC(hDC);
				GetClientRect(hWnd,&rect);
				hOldBitmap=(HBITMAP)SelectObject(hCmpDC,hBitmap);
				StretchBlt(hDC,rect.left+1,rect.top+1,rect.right-1,rect.bottom-1,hCmpDC,rect.left,rect.top,70,20,SRCCOPY);
				SelectObject(hCmpDC,hOldBitmap);
				DeleteObject(hBitmap);
				DeleteDC(hCmpDC);
				ReleaseDC(hWnd,hDC);
			}else if(rb.resval.rint==256) {
				hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(EC_COLOR_BMP256));
				hDC=GetDC(hWnd);
				hCmpDC=CreateCompatibleDC(hDC);
				GetClientRect(hWnd,&rect);
				hOldBitmap=(HBITMAP)SelectObject(hCmpDC,hBitmap);
				StretchBlt(hDC,rect.left+1,rect.top+1,rect.right-1,rect.bottom-1,hCmpDC,rect.left,rect.top,70,20,SRCCOPY);
				SelectObject(hCmpDC,hOldBitmap);
				DeleteObject(hBitmap);
				DeleteDC(hCmpDC);
				ReleaseDC(hWnd,hDC);
			}else if(rb.resval.rint>0 && rb.resval.rint<256){
				hDC=::GetDC(hWnd);
				HPALETTE savpal=::SelectPalette(hDC,(HPALETTE)SDS_CMainWindow->m_pPalette->GetSafeHandle(),TRUE);
				brush.CreateSolidBrush(SDS_BrushColorFromACADColor(rb.resval.rint));
				::GetClientRect(hWnd,&rect);
				rect.left+=1;rect.top+=1;rect.right-=1;rect.bottom-=1;
				::FillRect(hDC,&rect,(HBRUSH)brush);
				brush.DeleteObject();
//				::SelectPalette(hDC,savpal,TRUE);
				::ReleaseDC(hWnd,hDC);
			}
			break;
		case EC_HATCH_STYLE:
			sds_getvar(varname,&rb);
			if(rb.resval.rint==0){
				hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(EC_HATCH_STANDARD));
			}else if(rb.resval.rint==1){
				hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(EC_HATCH_OUTER));
			}else if(rb.resval.rint==2){
				hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(EC_HATCH_ALL));
			}
			hDC=GetDC(hWnd);
			hCmpDC=CreateCompatibleDC(hDC);
			GetClientRect(hWnd,&rect);
			hOldBitmap=(HBITMAP)SelectObject(hCmpDC,hBitmap);
			StretchBlt(hDC,rect.left,rect.top,rect.right,rect.bottom,hCmpDC,rect.left,rect.top,60,48,SRCCOPY);
			SelectObject(hCmpDC,hOldBitmap);
			DeleteObject(hBitmap);
			DeleteDC(hCmpDC);
			ReleaseDC(hWnd,hDC);
			break;
		case EC_LINETYPEDISP:
			sds_getvar(varname,&rb);
			if(rb.resval.rint){
				hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(EC_PLINES1));
			}else{
				hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(EC_PLINES2));
			}
			hDC=GetDC(hWnd);
			hCmpDC=CreateCompatibleDC(hDC);
			GetClientRect(hWnd,&rect);
			hOldBitmap=(HBITMAP)SelectObject(hCmpDC,hBitmap);
			StretchBlt(hDC,rect.left,rect.top,rect.right,rect.bottom,hCmpDC,rect.left,rect.top,112,39,SRCCOPY);
			SelectObject(hCmpDC,hOldBitmap);
			DeleteObject(hBitmap);
			DeleteDC(hCmpDC);
			ReleaseDC(hWnd,hDC);
			break;
		case EC_SKETCHDISP:
			sds_getvar(varname,&rb);
			if(rb.resval.rint){
				hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(EC_SKPOLY2));
			}else{
				hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(EC_SKPOLY1));
			}
			hDC=GetDC(hWnd);
			hCmpDC=CreateCompatibleDC(hDC);
			GetClientRect(hWnd,&rect);
			hOldBitmap=(HBITMAP)SelectObject(hCmpDC,hBitmap);
			StretchBlt(hDC,rect.left,rect.top,rect.right,rect.bottom,hCmpDC,rect.left,rect.top,112,39,SRCCOPY);
			SelectObject(hCmpDC,hOldBitmap);
			DeleteObject(hBitmap);
			DeleteDC(hCmpDC);
			ReleaseDC(hWnd,hDC);
			break;
/*		  case EC_TEXTDISP:
			{
			struct resbuf *pRbb=NULL,*pRb;
			SDS_getvar(NULL,DB_QTEXTSTYLE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			pRbb=sds_tblsearch("STYLE"DNT,rb.resval.rstring,0);
			if(rb.resval.rstring) free(rb.resval.rstring);
			if(pRbb==NULL) break;
			for(pRb=pRbb;pRb!=NULL&&pRb->restype!=3;pRb=pRb->rbnext);
			strcpy(szTmp,pRb->resval.rstring);
			for(pRb=pRbb;pRb!=NULL&&pRb->restype!=4;pRb=pRb->rbnext);
			GetClientRect(hWnd,&rect);
			Bitmap=SDS_DrawDiskFont(rect.right,rect.bottom,szTmp,pRb->resval.rstring);
			hDC=GetDC(hWnd);
			CDC CmpDC;
			CmpDC.CreateCompatibleDC(CmpDC.FromHandle(hDC));
			CBitmap *OldBitmap=CmpDC.SelectObject(Bitmap);
			BitBlt(hDC,rect.left,rect.top,rect.right,rect.bottom,(HDC)CmpDC,rect.left,rect.top,SRCCOPY);
			delete CmpDC.SelectObject(OldBitmap);
			ReleaseDC(hWnd,hDC);
			IC_RELRB(pRbb);
			}
			break;
*/
		case EC_COLOR:
			sds_getvar(varname,&rb);
			sTmp=ic_colornum(rb.resval.rstring);
			if(rb.resval.rstring){IC_FREE(rb.resval.rstring);rb.resval.rstring=NULL;}
			if(sTmp==0) {
				hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(EC_COLOR_BMP));
				hDC=GetDC(hWnd);
				hCmpDC=CreateCompatibleDC(hDC);
				GetClientRect(hWnd,&rect);
				hOldBitmap=(HBITMAP)SelectObject(hCmpDC,hBitmap);
				StretchBlt(hDC,rect.left+1,rect.top+1,rect.right-1,rect.bottom,hCmpDC,0,0,70,20,SRCCOPY);
				SelectObject(hCmpDC,hOldBitmap);
				DeleteObject(hBitmap);
				DeleteDC(hCmpDC);
				ReleaseDC(hWnd,hDC);
			}else if(sTmp==256) {
				hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(EC_COLOR_BMP256));
				hDC=GetDC(hWnd);
				hCmpDC=CreateCompatibleDC(hDC);
				GetClientRect(hWnd,&rect);
				hOldBitmap=(HBITMAP)SelectObject(hCmpDC,hBitmap);
				StretchBlt(hDC,rect.left+1,rect.top+1,rect.right-1,rect.bottom,hCmpDC,0,0,70,20,SRCCOPY);
				SelectObject(hCmpDC,hOldBitmap);
				DeleteObject(hBitmap);
				DeleteDC(hCmpDC);
				ReleaseDC(hWnd,hDC);
			}else if(sTmp>0 && sTmp<256){
				hDC=::GetDC(hWnd);
				HPALETTE savpal=::SelectPalette(hDC,(HPALETTE)SDS_CMainWindow->m_pPalette->GetSafeHandle(),TRUE);
				brush.CreateSolidBrush(SDS_BrushColorFromACADColor(sTmp));
				::GetClientRect(hWnd,&rect);
				rect.left+=1;rect.top+=1;rect.right+=1;rect.bottom+=1;
				::FillRect(hDC,&rect,(HBRUSH)brush);
				brush.DeleteObject();
//				::SelectPalette(hDC,savpal,TRUE);
				::ReleaseDC(hWnd,hDC);
			}
			break;
		case EM_FIT_BMP:
			sds_getvar(varname,&rb);
			if(rb.resval.rint==2){
				hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(EM_QUAD));
			}else{
				hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(EM_CUBIC));
			}
			hDC=GetDC(hWnd);
			hCmpDC=CreateCompatibleDC(hDC);
			GetClientRect(hWnd,&rect);
			hOldBitmap=(HBITMAP)SelectObject(hCmpDC,hBitmap);
			StretchBlt(hDC,rect.left,rect.top,rect.right,rect.bottom,hCmpDC,rect.left,rect.top,79,38,SRCCOPY);
			SelectObject(hCmpDC,hOldBitmap);
			DeleteObject(hBitmap);
			DeleteDC(hCmpDC);
			ReleaseDC(hWnd,hDC);
			break;
		case EM_DISPLAY_BMP:
			sds_getvar(varname,&rb);
			if(rb.resval.rint){
				hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(EM_USEFRAME));
			}else{
				hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(EM_NOFRAME));
			}
			hDC=GetDC(hWnd);
			hCmpDC=CreateCompatibleDC(hDC);
			GetClientRect(hWnd,&rect);
			hOldBitmap=(HBITMAP)SelectObject(hCmpDC,hBitmap);
			StretchBlt(hDC,rect.left,rect.top,rect.right,rect.bottom,hCmpDC,rect.left,rect.top,79,38,SRCCOPY);
			SelectObject(hCmpDC,hOldBitmap);
			DeleteObject(hBitmap);
			DeleteDC(hCmpDC);
			ReleaseDC(hWnd,hDC);
			break;
		case EM_MIRRDISPLAY:
			sds_getvar(varname,&rb);
			if(rb.resval.rint){
				hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(EM_MIRRTRUE));
			}else{
				hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(EM_MIRRREAD));
			}
			hDC=GetDC(hWnd);
			hCmpDC=CreateCompatibleDC(hDC);
			GetClientRect(hWnd,&rect);
			hOldBitmap=(HBITMAP)SelectObject(hCmpDC,hBitmap);
			StretchBlt(hDC,rect.left,rect.top,rect.right,rect.bottom,hCmpDC,rect.left,rect.top,64,48,SRCCOPY);
			SelectObject(hCmpDC,hOldBitmap);
			DeleteObject(hBitmap);
			DeleteDC(hCmpDC);
			ReleaseDC(hWnd,hDC);
			break;
		case EM_CHAMDISPLAY:
			sds_getvar(varname,&rb);
			if(rb.resval.rint){
				SDS_getvar(NULL,DB_QTRIMMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				if(rb.resval.rint){
					hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(EM_CHAMFER4));
				}else{
					hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(EM_CHAMFER2));
				}
			}else{
				SDS_getvar(NULL,DB_QTRIMMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				if(rb.resval.rint){
					hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(EM_CHAMFER3));
				}else{
					hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(EM_CHAMFER1));
				}
			}
			hDC=GetDC(hWnd);
			hCmpDC=CreateCompatibleDC(hDC);
			GetClientRect(hWnd,&rect);
			hOldBitmap=(HBITMAP)SelectObject(hCmpDC,hBitmap);
			StretchBlt(hDC,rect.left,rect.top,rect.right,rect.bottom,hCmpDC,rect.left,rect.top,144,52,SRCCOPY);
			SelectObject(hCmpDC,hOldBitmap);
			DeleteObject(hBitmap);
			DeleteDC(hCmpDC);
			ReleaseDC(hWnd,hDC);
			break;
		case DIM_ARROWS_PIC1:
			{
			/*DG - 14.11.2002*/// Let actual custom arrow block bitmaps be drawn instead of "custom" ones.
			CBitmap*	pCusArrBmp = NULL;
			GetClientRect(hWnd, &rect);
			index=SendMessage(GetDlgItem(GetParent(hWnd),DIM_ARROWS_START),CB_GETCURSEL,0,0);
			switch(index){
				case 0:
					hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(DIMARW_NONE_1ST));
					break;
				case 1:
					hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(DIMARW_CLOSED_1ST));
					break;
				case 2:
					hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(DIMARW_DOT_1ST));
					break;
				case 3:
					hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(DIMARW_CLOSEDFILLED_1ST));
					break;
				case 4:
					hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(DIMARW_CLOSEDBLANK_1ST));
					break;
				case 5:
					hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(DIMARW_OBLIQUE_1ST));
					break;
				case 6:
					hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(DIMARW_ARCHTICK_1ST));
					break;
				case 7:
					hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(DIMARW_OPEN_1ST));
					break;
				case 8:
					hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(DIMARW_ORIGIN_1ST));
					break;
				case 9:
					hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(DIMARW_OPEN90_1ST));
					break;
				case 10:
					hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(DIMARW_OPEN30_1ST));
					break;
				case 11:
					hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(DIMARW_DOTBLANK_1ST));
					break;
				case 12:
					hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(DIMARW_DOTSMALL_1ST));
					break;
				case 13:
					hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(DIMARW_BOXFILLED_1ST));
					break;
				case 14:
					hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(DIMARW_BOXBLANK_1ST));
					break;
				case 15:
					hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(DIMARW_DATUMFILLED_1ST));
					break;
				case 16:
					hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(DIMARW_DATUMBLANK_1ST));
					break;
				case 17:
					hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(DIMARW_INTEGRAL_1ST));
					break;
					case CB_ERR:
					default:
						char	blockName[256];
						if(GetWindowText(GetDlgItem(GetParent(hWnd), DIM_ARROWS_START), blockName, sizeof(blockName) - 1))
							pCusArrBmp = SDS_DrawBitmap(SDS_CURDWG, NULL, blockName, NULL, rect.right, rect.bottom, TRUE);
						hBitmap = pCusArrBmp ? (HBITMAP)*pCusArrBmp : LoadBitmap(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(DIMARW_CUSTOM_1ST));
						break;
				}
				hDC=GetDC(hWnd);
				hCmpDC=CreateCompatibleDC(hDC);
				GetClientRect(hWnd,&rect);
				hOldBitmap=(HBITMAP)SelectObject(hCmpDC,hBitmap);
				if(pCusArrBmp)
					StretchBlt(hDC, 0, 0, rect.right, rect.bottom, hCmpDC, 0, 0, rect.right, rect.bottom, NOTSRCCOPY);
				else
					StretchBlt(hDC, 0, 0, rect.right, rect.bottom, hCmpDC, 0, 0, 55, 29, SRCCOPY);
				SelectObject(hCmpDC, hOldBitmap);
				if(pCusArrBmp)
					delete pCusArrBmp;
				else
					DeleteObject(hBitmap);
				DeleteDC(hCmpDC);
				ReleaseDC(hWnd,hDC);
				break;
			}
        case DIM_ARROWS_PIC2:
			{
				/*DG - 14.11.2002*/// Let actual custom arrow block bitmaps be drawn instead of "custom" ones.
				CBitmap*	pCusArrBmp = NULL;
				GetClientRect(hWnd, &rect);
				index=SendMessage(GetDlgItem(GetParent(hWnd),DIM_ARROWS_END),CB_GETCURSEL,0,0);
				switch(index){
					case 0:
						hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(DIMARW_NONE_2ND));
						break;
					case 1:
						hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(DIMARW_CLOSED_2ND));
						break;
					case 2:
						hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(DIMARW_DOT_2ND));
						break;
					case 3:
						hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(DIMARW_CLOSEDFILLED_2ND));
						break;
					case 4:
						hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(DIMARW_CLOSEDBLANK_2ND));
						break;
					case 5:
						hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(DIMARW_OBLIQUE_2ND));
						break;
					case 6:
						hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(DIMARW_ARCHTICK_2ND));
						break;
					case 7:
						hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(DIMARW_OPEN_2ND));
						break;
					case 8:
						hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(DIMARW_ORIGIN_2ND));
						break;
					case 9:
						hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(DIMARW_OPEN90_2ND));
						break;
					case 10:
						hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(DIMARW_OPEN30_2ND));
						break;
					case 11:
						hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(DIMARW_DOTBLANK_2ND));
						break;
					case 12:
						hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(DIMARW_DOTSMALL_2ND));
						break;
					case 13:
						hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(DIMARW_BOXFILLED_2ND));
						break;
					case 14:
						hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(DIMARW_BOXBLANK_2ND));
						break;
					case 15:
						hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(DIMARW_DATUMFILLED_2ND));
						break;
					case 16:
						hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(DIMARW_DATUMBLANK_2ND));
						break;
					case 17:
						hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(DIMARW_INTEGRAL_2ND));
						break;
					case CB_ERR:
					default:
						char	blockName[256];
						if(GetWindowText(GetDlgItem(GetParent(hWnd), DIM_ARROWS_END), blockName, sizeof(blockName) - 1))
							pCusArrBmp = SDS_DrawBitmap(SDS_CURDWG, NULL, blockName, NULL, rect.right, rect.bottom, TRUE);
						hBitmap = pCusArrBmp ? (HBITMAP)*pCusArrBmp : LoadBitmap(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(DIMARW_CUSTOM_2ND));
						break;
				}
				hDC=GetDC(hWnd);
				hCmpDC=CreateCompatibleDC(hDC);
				GetClientRect(hWnd,&rect);
				hOldBitmap=(HBITMAP)SelectObject(hCmpDC,hBitmap);
				if(pCusArrBmp)
					StretchBlt(hDC, 0, 0, rect.right, rect.bottom, hCmpDC, 0, 0, rect.right, rect.bottom, NOTSRCCOPY);
				else
					StretchBlt(hDC, 0, 0, rect.right, rect.bottom, hCmpDC, 0, 0, 55, 29, SRCCOPY);
				SelectObject(hCmpDC, hOldBitmap);
				if(pCusArrBmp)
					delete pCusArrBmp;
				else
					DeleteObject(hBitmap);
				DeleteDC(hCmpDC);
				ReleaseDC(hWnd,hDC);
				break;
			}
		//Modified Cybage AW 16-02-01 [
		case LDR_ARROWS_PIC:
			{
				/*DG - 14.11.2002*/// Let actual custom arrow block bitmaps be drawn instead of "custom" ones.
				CBitmap*	pCusArrBmp = NULL;
				GetClientRect(hWnd, &rect);
				index=SendMessage(GetDlgItem(GetParent(hWnd),LDR_ARROWS),CB_GETCURSEL,0,0);
				switch(index){
					case 0:
						hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(LDRARW_NONE));
						break;
					case 1:
						hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(LDRARW_CLOSED));
						break;
					case 2:
						hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(LDRARW_DOT));
						break;
					case 3:
						hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(LDRARW_CLOSEDFILLED));
						break;
					case 4:
						hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(LDRARW_CLOSEDBLANK));
						break;
					case 5:
						hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(LDRARW_OBLIQUE));
						break;
					case 6:
						hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(LDRARW_ARCHTICK));
						break;
					case 7:
						hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(LDRARW_OPEN));
						break;
					case 8:
						hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(LDRARW_ORIGIN));
						break;
					case 9:
						hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(LDRARW_OPEN90));
						break;
					case 10:
						hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(LDRARW_OPEN30));
						break;
					case 11:
						hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(LDRARW_DOTBLANK));
						break;
					case 12:
						hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(LDRARW_DOTSMALL));
						break;
					case 13:
						hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(LDRARW_BOXFILLED));
						break;
					case 14:
						hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(LDRARW_BOXBLANK));
						break;
					case 15:
						hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(LDRARW_DATUMFILLED));
						break;
					case 16:
						hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(LDRARW_DATUMBLANK));
						break;
					case 17:
						hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(LDRARW_INTEGRAL));
						break;
					case CB_ERR:
					default:
						char	blockName[256];
						if(GetWindowText(GetDlgItem(GetParent(hWnd), LDR_ARROWS), blockName, sizeof(blockName) - 1))
							pCusArrBmp = SDS_DrawBitmap(SDS_CURDWG, NULL, blockName, NULL, rect.right, rect.bottom, TRUE);
						hBitmap = pCusArrBmp ? (HBITMAP)*pCusArrBmp : LoadBitmap(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(LDRARW_CUSTOM));
						break;
				}
				hDC=GetDC(hWnd);
				hCmpDC=CreateCompatibleDC(hDC);
				GetClientRect(hWnd,&rect);
				hOldBitmap=(HBITMAP)SelectObject(hCmpDC,hBitmap);
				if(pCusArrBmp)
					StretchBlt(hDC, 0, 0, rect.right, rect.bottom, hCmpDC, 0, 0, rect.right, rect.bottom, NOTSRCCOPY);
				else
					StretchBlt(hDC, 0, 0, rect.right, rect.bottom, hCmpDC, 0, 0, 55, 29, SRCCOPY);
				SelectObject(hCmpDC, hOldBitmap);
				if(pCusArrBmp)
					delete pCusArrBmp;
				else
					DeleteObject(hBitmap);
				DeleteDC(hCmpDC);
				ReleaseDC(hWnd,hDC);
				break;
			}
			//Modified Cybage AW 16-02-01 ]

    }
    return(RTNORM);
}


void svar_DV_NODES::DV_SetColor() {
	struct resbuf rb;
	int iTmp;
	HWND hItemWnd;

	SDS_getvar(NULL,DB_QGRIPCOLOR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	SDS_GetColorDialog(rb.resval.rint,&iTmp,3);
	if(iTmp==0) {
		rb.resval.rint=7; //white
	}else if(iTmp==256) {
		rb.resval.rint=249; //black
	}else{
		rb.resval.rint=iTmp;
	}
	sds_setvar("GRIPCOLOR"/*DNT*/,&rb);
	hItemWnd=::GetDlgItem(m_hWnd,DV_NONSELCOLOR);
	svar_UpdatImage("GRIPCOLOR"/*DNT*/,DV_NONSELCOLOR,hItemWnd);
	::SendMessage(hItemWnd,WM_PAINT,0,0);
	hItemWnd=::GetDlgItem(m_hWnd,DV_NODEDISPLAY);
	svar_UpdatImage("GRIPCOLOR"/*DNT*/,DV_NODEDISPLAY,hItemWnd);
	::SendMessage(hItemWnd,WM_PAINT,0,0);
}

void svar_DV_NODES::DV_SetColorHot() {
	struct resbuf rb;
	int iTmp;
	HWND hItemWnd;

	SDS_getvar(NULL,DB_QGRIPHOT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	SDS_GetColorDialog(rb.resval.rint,&iTmp,3);
	if(iTmp==0) {
		rb.resval.rint=7; //white
	}else if(iTmp==256) {
		rb.resval.rint=249; //black
	}else{
		rb.resval.rint=iTmp;
	}
	sds_setvar("GRIPHOT"/*DNT*/,&rb);
	hItemWnd=::GetDlgItem(m_hWnd,DV_SELCOLOR);
	svar_UpdatImage("GRIPHOT"/*DNT*/,DV_SELCOLOR,hItemWnd);
	::SendMessage(hItemWnd,WM_PAINT,0,0);
}

void svar_EC_TAB::OnSelColor() {
	struct resbuf rb;
	int iTmp;
	char resstr[20];
	HWND hItemWnd;

	SDS_getvar(NULL,DB_QCECOLOR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	SDS_GetColorDialog(ic_colornum(rb.resval.rstring),&iTmp,0);
	//ic_colorstr(iTmp,resstr);AVH: don't convert to a named color
	if(rb.resval.rstring)
		{IC_FREE(rb.resval.rstring);rb.resval.rstring=NULL;}

	sprintf(resstr,"%d"/*DNT*/,iTmp);
	rb.resval.rstring=resstr;

	//AVH if anything is wrong with the colorindex(eg color >256) , setvar just fails to update.No extra check needed.
	sds_setvar("CECOLOR"/*DNT*/,&rb);
	hItemWnd=::GetDlgItem(m_hWnd,EC_COLOR);
	svar_UpdatImage("CECOLOR"/*DNT*/,EC_COLOR,hItemWnd);
	::SendMessage(hItemWnd,WM_PAINT,0,0);
}

// Static Proc()
LRESULT CALLBACK svar_StaticProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam) {
	LRESULT ret;
	char szTmp[256];
	int Wid,i;
	struct resbuf rb;
	struct svar_DlgItem *tmpDi=NULL;

	Wid=GetWindowLong(hWnd,GWL_ID);
	switch(uMsg) {
		case WM_PAINT:
		case WM_INITIALUPDATE:
			tmpDi=(struct svar_DlgItem *)GetWindowLong(hWnd,GWL_USERDATA);
			if(tmpDi==NULL) {
			   for(i=0;i<svar_num_of_vars;i++) {
					if(svar_CurDlgItem[i].nID==Wid) {
						tmpDi=&svar_CurDlgItem[i];
						break;
					}
				}
				if(tmpDi!=NULL) SetWindowLong(hWnd,GWL_USERDATA,(long)tmpDi);
				else break;
			}

			if(tmpDi->type==6 || tmpDi->type==7) {
				svar_UpdatImage(tmpDi->variable,tmpDi->nID,hWnd);
			}else{
				sds_getvar(tmpDi->variable,&rb);
				switch(rb.restype){
					case RTSTR:
						if(*rb.resval.rstring!=0) {
							SetWindowText(hWnd,rb.resval.rstring);
							if(rb.resval.rstring){IC_FREE(rb.resval.rstring);rb.resval.rstring=NULL;}
						}
						break;
					case RTSHORT:
						if(tmpDi->Value&rb.resval.rint){
							SetWindowText(hWnd,ResourceString(IDC_ICADSETVARSDIA_ON_179, "ON" ));
						}else{
							SetWindowText(hWnd,ResourceString(IDC_ICADSETVARSDIA_OFF_180, "OFF" ));
						}
						break;
					case RTREAL:
						switch(tmpDi->ResType){
							case RTREAL:
								sds_rtos(rb.resval.rreal,-1,-1,szTmp);
								break;
							case RTANG:
								sds_angtos(rb.resval.rreal,-1,-1,szTmp);
								break;
							case SVAR_MM:
								sds_rtos(rb.resval.rreal,2,-1,szTmp);
								break;
						}
						SetWindowText(hWnd,szTmp);
						break;
					case RTPOINT:
					case RT3DPOINT:
						sds_rtos(rb.resval.rpoint[tmpDi->Value],-1,-1,szTmp);
						SetWindowText(hWnd,szTmp);
						break;
				}
			}
	}
	for(i=0;i<svar_num_of_vars;i++) {
		if(svar_CurDlgItem[i].nID==Wid) {
			ret=CallWindowProc((WNDPROC)(long (__stdcall *)(void))svar_CurDlgItem[i].WndProc,hWnd,uMsg,wParam,lParam);
			break;
		}
	}
	return(ret);
}

int svar_StupidSet(svar_DlgItem *tmpDi,HWND hWnd,short *i) {
	HWND hWndChild,hWndCur;
	struct resbuf rb;
	int dimunit;

	switch(tmpDi->nID) {
		case DIM_TOL_DIMTZIN:
			SDS_getvar(NULL,DB_QDIMTZIN,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

			hWndCur=hWnd;	// Checkbox being changed.
			hWndChild=GetDlgItem(GetParent(hWnd),DIM_TOL_DIMTZIN2); // Other checkbox.

			// If neither is checked we are turning ON suppression of leading zeros.
			if((!SendMessage(hWndCur,BM_GETCHECK,0,0))&&(!SendMessage(hWndChild,BM_GETCHECK,0,0))) {
				*i=4;
			// Suppression of trailing zeros is ON, Turning ON leading zero suppression.
			}else if((!SendMessage(hWndCur,BM_GETCHECK,0,0))&&(SendMessage(hWndChild,BM_GETCHECK,0,0))) {
				*i=rb.resval.rint + 4;
			// Suppression of trailing zeros is OFF, Turning OFF leading zero suppression.
			}else if((SendMessage(hWndCur,BM_GETCHECK,0,0))&&(!SendMessage(hWndChild,BM_GETCHECK,0,0))) {
				*i=0;
			// Suppression of trailing zeros is ON, Turning OFF leading zero suppression.
			}else if((SendMessage(hWndCur,BM_GETCHECK,0,0))&&(SendMessage(hWndChild,BM_GETCHECK,0,0))) {
				*i=rb.resval.rint >= 4 ? rb.resval.rint-4 : 0;
			}
			break;
		case DIM_TOL_DIMTZIN2:
			SDS_getvar(NULL,DB_QDIMTZIN,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

			hWndChild=hWnd;  // Checkbox being changed.
			hWndCur=GetDlgItem(GetParent(hWnd),DIM_TOL_DIMTZIN);	// Other checkbox.

			// If neither is checked we are turning ON suppression of trailing zeros.
			if((!SendMessage(hWndCur,BM_GETCHECK,0,0))&&(!SendMessage(hWndChild,BM_GETCHECK,0,0))) {
				*i=8;
			// Suppression of leading zeros is OFF. Turning OFF trailing zero suppression.
			}else if((!SendMessage(hWndCur,BM_GETCHECK,0,0))&&(SendMessage(hWndChild,BM_GETCHECK,0,0))) {
				*i=rb.resval.rint >= 8 ?  rb.resval.rint - 8 : 0;
			// Suppression of leading zeros is ON. Turning ON trailing zero suppression.
			}else if((SendMessage(hWndCur,BM_GETCHECK,0,0))&&(!SendMessage(hWndChild,BM_GETCHECK,0,0))) {
				*i=rb.resval.rint +8;
			// Suppression of leading zeros is ON. Turning OFF	trailing zero suppression.
			}else if((SendMessage(hWndCur,BM_GETCHECK,0,0))&&(SendMessage(hWndChild,BM_GETCHECK,0,0))) {
				*i=rb.resval.rint >= 8 ? rb.resval.rint-8 : 0;
			}
			break;
		case DIM_ALT_DIMALTZ1:
			SDS_getvar(NULL,DB_QDIMALTZ,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

			hWndCur=hWnd;
			hWndChild=GetDlgItem(GetParent(hWnd),DIM_ALT_DIMALTZ2);
			if((!SendMessage(hWndCur,BM_GETCHECK,0,0))&&(!SendMessage(hWndChild,BM_GETCHECK,0,0))) {
				*i=4;
			}else if((!SendMessage(hWndCur,BM_GETCHECK,0,0))&&(SendMessage(hWndChild,BM_GETCHECK,0,0))) {
				*i=rb.resval.rint + 4;
			}else if((SendMessage(hWndCur,BM_GETCHECK,0,0))&&(!SendMessage(hWndChild,BM_GETCHECK,0,0))) {
				*i=0;
			}else if((SendMessage(hWndCur,BM_GETCHECK,0,0))&&(SendMessage(hWndChild,BM_GETCHECK,0,0))) {
				*i=rb.resval.rint >= 4 ? rb.resval.rint-4 : 0;
			}
			break;
		case DIM_ALT_DIMALTZ2:
			SDS_getvar(NULL,DB_QDIMALTZ,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

			hWndChild=hWnd;
			hWndCur=GetDlgItem(GetParent(hWnd),DIM_ALT_DIMALTZ1);
			// If neither is checked we are turning ON suppression of trailing zeros.
			if((!SendMessage(hWndCur,BM_GETCHECK,0,0))&&(!SendMessage(hWndChild,BM_GETCHECK,0,0))) {
				*i=8;
			// Suppression of leading zeros is OFF. Turning OFF trailing zero suppression.
			}else if((!SendMessage(hWndCur,BM_GETCHECK,0,0))&&(SendMessage(hWndChild,BM_GETCHECK,0,0))) {
				*i=rb.resval.rint >= 8 ?  rb.resval.rint - 8 : 0;
			// Suppression of leading zeros is ON. Turning ON trailing zero suppression.
			}else if((SendMessage(hWndCur,BM_GETCHECK,0,0))&&(!SendMessage(hWndChild,BM_GETCHECK,0,0))) {
				*i=rb.resval.rint +8;
			// Suppression of leading zeros is ON. Turning OFF	trailing zero suppression.
			}else if((SendMessage(hWndCur,BM_GETCHECK,0,0))&&(SendMessage(hWndChild,BM_GETCHECK,0,0))) {
				*i=rb.resval.rint >= 8 ? rb.resval.rint-8 : 0;
			}
			break;
		case DIM_ALT_DIMALTTZ1:
			SDS_getvar(NULL,DB_QDIMALTTZ,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

			hWndCur=hWnd;
			hWndChild=GetDlgItem(GetParent(hWnd),DIM_ALT_DIMALTTZ2);
			if((!SendMessage(hWndCur,BM_GETCHECK,0,0))&&(!SendMessage(hWndChild,BM_GETCHECK,0,0))) {
				*i=4;
			}else if((!SendMessage(hWndCur,BM_GETCHECK,0,0))&&(SendMessage(hWndChild,BM_GETCHECK,0,0))) {
				*i=rb.resval.rint + 4;
			}else if((SendMessage(hWndCur,BM_GETCHECK,0,0))&&(!SendMessage(hWndChild,BM_GETCHECK,0,0))) {
				*i=0;
			}else if((SendMessage(hWndCur,BM_GETCHECK,0,0))&&(SendMessage(hWndChild,BM_GETCHECK,0,0))) {
				*i=rb.resval.rint >= 4 ? rb.resval.rint-4 : 0;
			}
			break;
		case DIM_ALT_DIMALTTZ2:
			SDS_getvar(NULL,DB_QDIMALTTZ,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

			hWndChild=hWnd;
			hWndCur=GetDlgItem(GetParent(hWnd),DIM_ALT_DIMALTTZ1);
			if((!SendMessage(hWndCur,BM_GETCHECK,0,0))&&(!SendMessage(hWndChild,BM_GETCHECK,0,0))) {
				*i=8;
			}else if((!SendMessage(hWndCur,BM_GETCHECK,0,0))&&(SendMessage(hWndChild,BM_GETCHECK,0,0))) {
				*i=rb.resval.rint >= 8 ?  rb.resval.rint - 8 : 0;
			}else if((SendMessage(hWndCur,BM_GETCHECK,0,0))&&(!SendMessage(hWndChild,BM_GETCHECK,0,0))) {
				*i=rb.resval.rint +8;
			}else if((SendMessage(hWndCur,BM_GETCHECK,0,0))&&(SendMessage(hWndChild,BM_GETCHECK,0,0))) {
				*i=rb.resval.rint >= 8 ? rb.resval.rint-8 : 0;
			}
			break;
		case DIM_UNITS_DIMZIN1:
			// EBATECH(CNBR) -[ 2003/2/6 DIMUNIT=8(Windows Desktop) affect Suppress Leading/Trail zeros.
			SDS_getvar(NULL,DB_QDIMUNIT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			dimunit = rb.resval.rint;
			hWndCur=hWnd;
			hWndChild=GetDlgItem(GetParent(hWnd),DIM_UNITS_DIMZIN2);
			if((!SendMessage(hWndCur,BM_GETCHECK,0,0))&&(!SendMessage(hWndChild,BM_GETCHECK,0,0))) {
				*i=(dimunit>3 && dimunit<8) ? 3 : 4;
			}else if((!SendMessage(hWndCur,BM_GETCHECK,0,0))&&(SendMessage(hWndChild,BM_GETCHECK,0,0))) {
				*i=(dimunit>3 && dimunit<8) ? 0 : 12;
			}else if((SendMessage(hWndCur,BM_GETCHECK,0,0))&&(!SendMessage(hWndChild,BM_GETCHECK,0,0))) {
				*i=(dimunit>3 && dimunit<8) ? 1 : 0;
			}else if((SendMessage(hWndCur,BM_GETCHECK,0,0))&&(SendMessage(hWndChild,BM_GETCHECK,0,0))) {
				*i=(dimunit>3 && dimunit<8) ? 2 : 8;
			}
			break;
		case DIM_UNITS_DIMZIN2:
			SDS_getvar(NULL,DB_QDIMUNIT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			dimunit = rb.resval.rint;
			hWndChild=hWnd;
			hWndCur=GetDlgItem(GetParent(hWnd),DIM_UNITS_DIMZIN1);
			if((!SendMessage(hWndCur,BM_GETCHECK,0,0))&&(!SendMessage(hWndChild,BM_GETCHECK,0,0))) {
				*i=(dimunit>3 && dimunit<8) ? 2 : 8;
			}else if((!SendMessage(hWndCur,BM_GETCHECK,0,0))&&(SendMessage(hWndChild,BM_GETCHECK,0,0))) {
				*i=(dimunit>3 && dimunit<8) ? 1 : 0;
			}else if((SendMessage(hWndCur,BM_GETCHECK,0,0))&&(!SendMessage(hWndChild,BM_GETCHECK,0,0))) {
				*i=(dimunit>3 && dimunit<8) ? 0 : 12;
			}else if((SendMessage(hWndCur,BM_GETCHECK,0,0))&&(SendMessage(hWndChild,BM_GETCHECK,0,0))) {
				*i=(dimunit>3 && dimunit<8) ? 3 : 4;
			}
			break;
			// EBATECH(CNBR) ]-
	}
	return(RTNORM);
}

int svar_StrSet(svar_DlgItem *tmpDi,HWND hWnd) {
    char szPre[IC_ACADBUF],szSuf[IC_ACADBUF];
    struct resbuf rb;
    HWND hWndOther;
    BOOL flag=false;

    sds_getvar(tmpDi->variable,&rb);
    switch(tmpDi->nID) {
        case DIM_TEXT_DIMPOST1:
            hWndOther=GetDlgItem(GetParent(hWnd),DIM_TEXT_DIMPOST2);
            GetWindowText(hWnd,szPre,sizeof(szPre)-1);
            GetWindowText(hWndOther,szSuf,sizeof(szSuf)-1);
            flag=false;
			strcat(szPre,"<>"/*DNT*/);
            break;
        case DIM_TEXT_DIMPOST2:
            hWndOther=GetDlgItem(GetParent(hWnd),DIM_TEXT_DIMPOST1);
            GetWindowText(hWndOther,szPre,sizeof(szPre)-1);
            GetWindowText(hWnd,szSuf,sizeof(szSuf)-1);
            flag=false;
			strcat(szPre,"<>"/*DNT*/);
            break;
        case DIM_ALT_DIMAPOST1:
            hWndOther=GetDlgItem(GetParent(hWnd),DIM_ALT_DIMAPOST2);
            GetWindowText(hWnd,szPre,sizeof(szPre)-1);
            GetWindowText(hWndOther,szSuf,sizeof(szSuf)-1);
            flag=true;
			strcat(szPre,"[]"/*DNT*/);
            break;
        case DIM_ALT_DIMAPOST2:
            hWndOther=GetDlgItem(GetParent(hWnd),DIM_ALT_DIMAPOST1);
            GetWindowText(hWndOther,szPre,sizeof(szPre)-1);
            GetWindowText(hWnd,szSuf,sizeof(szSuf)-1);
            flag=true;
			strcat(szPre,"[]"/*DNT*/);
            break;
    }
    strcat(szPre,szSuf);
    
    if(strlen(szPre)>15){
		
		//Bugzilla No. 78199 ; 09-12-2002 
		struct svar_DlgItem *tmpDi=(struct svar_DlgItem *)GetWindowLong(hWnd,GWL_USERDATA);
		if(tmpDi==NULL) return false;
		svar_StrGet(tmpDi,hWnd);

        if(flag==false){
            ::MessageBox(::GetParent(::GetParent(hWnd)),ResourceString(IDC_ICADSETVARSDIA_THE_COM_182, "The combined length of prefix and suffix is greater than allowed by DIMPOST. Truncating." ),ResourceString(IDC_ICADSETVARSDIA_POSSIBL_183, "Possible Loss of Data" ),MB_OK|MB_ICONWARNING|MB_TASKMODAL);
        }else{
            ::MessageBox(::GetParent(::GetParent(hWnd)),ResourceString(IDC_ICADSETVARSDIA_THE_COM_184, "The combined length of prefix and suffix is greater than allowed by DIMAPOST. Truncating." ),ResourceString(IDC_ICADSETVARSDIA_POSSIBL_183, "Possible Loss of Data" ),MB_OK|MB_ICONWARNING|MB_TASKMODAL);
        }

		return(RTNORM);

    }
	//Bugzilla No. 78199 ; 09-12-2002 
	rb.resval.rstring=ic_realloc_char((char *)rb.resval.rstring,strlen(szPre)+1);
    strcpy(rb.resval.rstring,szPre);
    sds_setvar(tmpDi->variable,&rb);
    if(rb.resval.rstring){IC_FREE(rb.resval.rstring);rb.resval.rstring=NULL;}
    return(RTNORM);
}

// EBATECH(CNBR) -[ 2002/5/1 Use _tcsstr() for MBCS String
int svar_StrGet(svar_DlgItem *tmpDi,HWND hWnd) {
	char *szTmp = NULL;
	struct resbuf rb;

	sds_getvar(tmpDi->variable,&rb);
	//Bugzilla No. 78193 ; 10-06-2002 [
	switch(tmpDi->nID) {
		case DIM_TEXT_DIMPOST1:
			if( szTmp = _tcsstr(rb.resval.rstring, "<>"/*DNT*/)){
				*szTmp = '\0';
			SetWindowText(hWnd,rb.resval.rstring);
			}
			//Bugzilla No. 78193 ; 12-05-2003
			if(rb.resval.rstring[0] == '\0')
				SetWindowText(hWnd,rb.resval.rstring);
			break;
		case DIM_TEXT_DIMPOST2:
			if( szTmp = _tcsstr(rb.resval.rstring, "<>"/*DNT*/)){
				SetWindowText(hWnd,szTmp+2);
			}else{
				//SetWindowText(hWnd,""/*DNT*/);
				SetWindowText(hWnd,rb.resval.rstring);
			}
			break;
		case DIM_ALT_DIMAPOST1:
			if( szTmp = _tcsstr(rb.resval.rstring, "[]"/*DNT*/)){
				*szTmp = '\0';
			SetWindowText(hWnd,rb.resval.rstring);
			}
			//Bugzilla No. 78193 ; 12-05-2003
			if(rb.resval.rstring[0] == '\0')
				SetWindowText(hWnd,rb.resval.rstring);
			break;
		case DIM_ALT_DIMAPOST2:
			if( szTmp = _tcsstr(rb.resval.rstring, "[]"/*DNT*/)){
				SetWindowText(hWnd,szTmp+2);
			}else{
				//SetWindowText(hWnd,""/*DNT*/);
				SetWindowText(hWnd,rb.resval.rstring);
			}
			break;
	}
	//Bugzilla No. 78193 ; 10-06-2002 ]
	if(rb.resval.rstring){IC_FREE(rb.resval.rstring);rb.resval.rstring=NULL;}
	return(RTNORM);
}
#if 0 // next code not used ...
int svar_StrGet(svar_DlgItem *tmpDi,HWND hWnd) {
	char *szTmp;
	struct resbuf rb;
	int i;

	sds_getvar(tmpDi->variable,&rb);
	switch(tmpDi->nID) {
		case DIM_TEXT_DIMPOST1:
			szTmp=rb.resval.rstring;
			for(i=0;(szTmp[i]!='<'/*DNT*/)&&(szTmp[i]!=0);i++);
			szTmp[i]=0;
			SetWindowText(hWnd,szTmp);
			break;
		case DIM_TEXT_DIMPOST2:
			szTmp=rb.resval.rstring;
			for(i=0;(szTmp[i]!='>'/*DNT*/)&&(szTmp[i]!=0);i++);
			if(szTmp[i]!=0) {
				szTmp+=i+1;
				SetWindowText(hWnd,szTmp);
			}else{
				SetWindowText(hWnd,""/*DNT*/);
			}
			break;
		case DIM_ALT_DIMAPOST1:
			szTmp=rb.resval.rstring;
			for(i=0;(szTmp[i]!='['/*DNT*/)&&(szTmp[i]!=0);i++);
			szTmp[i]=0;
			SetWindowText(hWnd,szTmp);
			break;
		case DIM_ALT_DIMAPOST2:
			szTmp=rb.resval.rstring;
			for(i=0;(szTmp[i]!=']'/*DNT*/)&&(szTmp[i]!=0);i++);
			if(szTmp[i]!=0) {
				szTmp+=i+1;
				SetWindowText(hWnd,szTmp);
			}else{
				SetWindowText(hWnd,""/*DNT*/);
			}
			break;
	}
	if(rb.resval.rstring){IC_FREE(rb.resval.rstring);rb.resval.rstring=NULL;}
	return(RTNORM);
}
#endif
// EBATECH(CNBR) ]-
