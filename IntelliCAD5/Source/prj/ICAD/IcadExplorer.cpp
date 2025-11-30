/* C:\DEV\PRJ\ICAD\ICADEXPLORER.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * Implementation of IntelliCAD Explorer functionality.
 *
 */

#include "icad.h"				/*DNT*/
#include "IcadExplorer.h"		/*DNT*/

#include "IcadApp.h"			/*DNT*/
#include "IcadChildWnd.h"		/*DNT*/
#include "IcadToolBar.h"		/*DNT*/
#include "ExplorerPropsTab.h"	/*DNT*/
#include "IcadCusButton.h"		/*DNT*/
#include "cmds.h"				/*DNT*/
#include "cmdsWBlock.h"			/*DNT*/
#include "CloneHelper.h"		/*DNT*/
#include "resource.hm"			/*DNT*/
#include "paths.h"				/*DNT*/
#include "Resbuf.h"				/*DNT*/
#include "viewport.h"			/*DNT*/
#include "IcadView.h"			/*DNT*/
#include "undo\UndoSaver.h"/*DNT*/

extern char  *cmd_FontSavePath;
void  cmd_OnOpenOK(HWND hDlg);
extern bool SDS_DontUpdateDisp;
//Bugzilla No. 78037 ; 29-04-2002
short cmd_renameXrefDependantTables(char *origname,char *newname,db_drawing *dbDrawing);
/** This global variable is used to save the state of the Block List Control (0=Report View, 1=IconView).
	It enables the state of this list control to be retrieved each time the Explorer is brought up.
	It is not persistent, and hence this will not be saved across sessions of ICAD.  This can be done
	using a new setvar, but we've decided not to do this for now.  6/11/98
*/
static int g_BlockState = 1;

/** This global variable is used a flag during enumulating font style and font languages.
	EBATECH(CNBR) 16/06/2001
*/
static bool	NowEnumulating = false;

/** Strings used in the Explorer.
*/
CString g_s_IEXP_LAYERS				= ResourceString(IDC_IEXP_LAYERS, "Layers");
CString g_s_IEXP_GENERAL			= ResourceString(IDC_IEXP_GENERAL, "  General  ");
CString g_s_IEXP_LINETYPES			= ResourceString(IDC_IEXP_LINETYPES, "Linetypes");
CString g_s_IEXP_STYLES				= ResourceString(IDC_IEXP_STYLES, "Text Styles");
CString g_s_IEXP_COORDSYSTEM		= ResourceString(IDC_IEXP_COORDSYSTEM, "Coordinate Systems");
CString g_s_IEXP_VIEWS				= ResourceString(IDC_IEXP_VIEWS, "Views");
CString g_s_IEXP_VIEWLENS			= ResourceString(IDC_IEXP_VIEWLENS, "  ViewLens  ");
CString g_s_IEXP_VIEWMODE			= ResourceString(IDC_IEXP_VIEWMODE, "  ViewMode  ");
CString g_s_IEXP_VIEWTARGET			= ResourceString(IDC_IEXP_VIEWTARGET, "  ViewTarget  ");
CString g_s_IEXP_BLOCKS				= ResourceString(IDC_IEXP_BLOCKS, "Blocks");
CString g_s_IEXP_DIMSTYS			= ResourceString(IDC_IEXP_DIMSTYS, "Dimension Styles");
CString g_s_IEXP_ITEMTYPES			= ResourceString(IDC_IEXP_ITEMTYPES, "Item Types");
CString g_s_IEXP_LAYER_NAME			= ResourceString(IDC_IEXP_LAYER_NAME, "Layer Name");
CString g_s_IEXP_COLOR				= ResourceString(IDC_IEXP_COLOR, "Color");
CString g_s_IEXP_LINETYPE			= ResourceString(IDC_IEXP_LINETYPE, "Linetype");
CString g_s_IEXP_ON_OFF				= ResourceString(IDC_IEXP_ON_OFF, "On/Off");
CString g_s_IEXP_LOCKED				= ResourceString(IDC_IEXP_LOCKED, "Locked");
CString g_s_IEXP_FROZEN				= ResourceString(IDC_IEXP_FROZEN, "All Viewports");
CString g_s_IEXP_LINEWEIGHT			= ResourceString(IDC_IEXP_LINEWEIGHT, "Lineweight");
CString g_s_IEXP_PLOT				= ResourceString(IDC_IEXP_PLOT, "Plot");
CString g_s_IEXP_VP_FREEZECUR		= ResourceString(IDC_IEXP_VP_FREEZECUR, "Current Viewport");
CString g_s_IEXP_VP_FREEZENEW		= ResourceString(IDC_IEXP_VP_FREEZENEW, "New Viewports");
CString g_s_IEXP_LINETYPE_NAME		= ResourceString(IDC_IEXP_LINETYPE_NAME, "Linetype Name");
CString g_s_IEXP_DESCRIPTIVE_TXT	= ResourceString(IDC_IEXP_DESCRIPTIVE_TXT, "Linetype Description");
CString g_s_IEXP_LINETYPE_DEF		= ResourceString(IDC_IEXP_LINETYPE_DEF, "Linetype Appearance");
CString g_s_IEXP_TEXT_STYLE_NAME	= ResourceString(IDC_IEXP_TEXT_STYLE_NAME, "Text Style Name");
CString g_s_IEXP_HEIGHT				= ResourceString(IDC_IEXP_HEIGHT, "Height");
CString g_s_IEXP_WIDTH_FACTOR		= ResourceString(IDC_IEXP_WIDTH_FACTOR, "Width Factor");
CString g_s_IEXP_OBLIQUE_ANGLE		= ResourceString(IDC_IEXP_OBLIQUE_ANGLE, "Oblique Angle");
CString g_s_IEXP_FONT_NAME			= ResourceString(IDC_IEXP_FONT_NAME, "Font Name");
CString g_s_IEXP_FONT_STYLE			= ResourceString(IDC_IEXP_FONT_STYLE, "Style");	// EBATECH(CNBR) Font Style -> Style
CString g_s_IEXP_BIGFONT_NAME		= ResourceString(IDC_IEXP_BIGFONT_NAME, "Bigfont");	// EBATECH(CNBR)
CString g_s_IEXP_FONT_LANGUAGE		= ResourceString(IDC_IEXP_FONT_LANGUAGE, "Language");	// EBAETCH(CNBR)
CString g_s_IEXP_TEXT_BACKWARDS		= ResourceString(IDC_IEXP_TEXT_BACKWARDS, "Backwards");
CString g_s_IEXP_TEXT_UPSIDEDOWN	= ResourceString(IDC_IEXP_TEXT_UPSIDEDOWN, "Upside down");
CString g_s_IEXP_TEXT_VERTICALLY	= ResourceString(IDC_IEXP_TEXT_VERTICALLY, "Vertically");
CString g_s_IEXP_UCS_NAME			= ResourceString(IDC_IEXP_UCS_NAME, "UCS Name");
CString g_s_IEXP_ORIGIN				= ResourceString(IDC_IEXP_ORIGIN, "Origin (WCS)");
CString g_s_IEXP_X_DIRECTION		= ResourceString(IDC_IEXP_X_DIRECTION, "X Axis Direction (WCS)");
CString g_s_IEXP_Y_DIRECTION		= ResourceString(IDC_IEXP_Y_DIRECTION, "Y Axis Direction (WCS)");
CString g_s_IEXP_VIEW_NAME			= ResourceString(IDC_IEXP_VIEW_NAME, "View Name");
CString g_s_IEXP_WIDTH				= ResourceString(IDC_IEXP_WIDTH, "Width");
CString g_s_IEXP_VIEW_DIRECTION		= ResourceString(IDC_IEXP_VIEW_DIRECTION, "View Direction");
CString g_s_IEXP_BLOCK_NAME			= ResourceString(IDC_IEXP_BLOCK_NAME, "Block Name");
CString g_s_IEXP_NUM_BLOCK_REF		= ResourceString(IDC_IEXP_NUM_BLOCK_REF, "Number of Block References");
CString g_s_IEXP_BLOCK_INSPOINT		= ResourceString(IDC_IEXP_BLOCK_INSPOINT, "Insertion Point");
CString g_s_IEXP_XREF				= ResourceString(IDC_IEXP_XREF, "Externally Referenced Drawing");
CString g_s_IEXP_DIMSTY_NAME		= ResourceString(IDC_IEXP_DIMSTY_NAME, "Dimension Style Name");
CString g_s_IEXP_ITEM_TYPE			= ResourceString(IDC_IEXP_ITEM_TYPE, "Item Type");
CString g_s_IEXP_NUM_ITEMS			= ResourceString(IDC_IEXP_NUM_ITEMS, "Number of Rows");

// ======================================================================================================================================================================
// ============================================================== GLOBAL STRUCTURES =====================================================================================
// ======================================================================================================================================================================
//

/// This structure will hold the Type, and the Name.
class IEXP_ItemDef
{
protected:
	char	*m_type,
			*m_name;
public:
	// Modified CyberAge AP 09/04/2000
	resbuf	 *m_rowPtr; // Reason: To fix slow layer problem.

	IEXP_ItemDef(): m_type(NULL), m_name(NULL){}
	IEXP_ItemDef(const char* cType, const char* cName): m_type(NULL), m_name(NULL)
	{
		m_type = (char *)new char[strlen(cType)+1];
		strcpy(m_type, cType);
		m_name = (char *)new char[strlen(cName)+1];
		strcpy(m_name, cName);
	}
	~IEXP_ItemDef()
	{
		delete[] m_type;
		delete[] m_name;
	}
	const char* name() const { return m_name; }
	const char* type() const { return m_type; }
	void setName(const char* cName)
	{
		int len = cName ? strlen(cName) : 0;
		if(m_name && (strlen(m_name) >= len))
		{
			strcpy(m_name, cName);
		}
		else
		{
			delete[] m_name;
			m_name = NULL;
			if(len)
			{
				m_name = new char[len + 1];
				strcpy(m_name, cName);
			}
		}
	}
} *IEXP_ItemDefPtr;

/** These structures are used to hold the old values of the (TYPE) being edited.
	each one is almost a duplicate of the tables in the drawing.
*/
struct IEXP_SaveTs {
	CString  fixht,
			 wfact,
			 obl,
			 fontpn,
			 fontstyle,
			 bigfontpn,	// EBATECH(CNBR)
			 language,	// EBAETCH(CNBR)
			 sampletext;
	int 	 textbk,
			 textup,
			 textvert,
			 flags,
			 tgf;
	sds_real widthf,
			 lhused,
			 oblangle;

} tssave;

struct IEXP_SaveLt {
	CString desc,
			ltdef;
	int 	flags,
			ndashes,
			patlen,
			aligncode;

} ltsave;

struct IEXP_SaveCs {
	CString 	origin,
				xdir,
				ydir;
	sds_point	orgpnt,
				xdirpnt,
				ydirpnt;
	int 		flags;
} cssave;

struct IEXP_SaveVw {
	CString height,
			width,
			center,
			twist,
			lenslen,
			fclip,
			bclip,
			target,
			targ2cam;
	int 	viewmd1,
			viewmd2,
			viewmd4,
			viewmd8,
			viewmd16;


} vwsave;

struct IEXP_SaveBlk {
	CString base;
} blksave;

// EBATECH(CNBR) 2003/3/31 Bugzilla#78471 Add lweight and plot. flags member has not been used.
struct IEXP_SaveLa {
	CString linetype;
	int 	color,
			flags,
			OnOff,
			freeze,
			lock,
			lweight,
			plot,
			vpnew;
} lasave;

/** This structure is needed for the Layer table, there is a difference in the parameters passed
	to build a layer as opposed to the regular layer table structure.
*/
struct dwg_latablink_exp {
	unsigned char	flags;
	char		   *name;
	short			color;
	char		   *ltname,
				   *desc;
} latab_exp;

struct IEXP_LTData {
	char  *Name,
		  *Desc;
};

struct IEXP_TabsData
	{
	const char	*Name,			// Name of property dialog (TABS).
				*Tab[4];		// Tab titles (TABS).
	int			TabCnt, 		// Number of tabs for dialog (TABS).
				TabTlen[4], 	// Hold the length of the tabs (TABS).
				ItemID[15]; 	// Item ID's in tab dialog (TABS)
	short		TabID[4];		// The Id of the tabs (TABS).
	RECT		Tabrect[4]; 	// Holds the rect size of the tabs (TABS).
	BOOL		Varies[15]; 	// The index of this flag value corresponds to the index of the (ITEMID) values, if (m_varies) variable is set to
								// true, then a true value is set if this field has more than one value or false if all values are the same.

	} IEXP_TABINFO[]=
	{
	// EBATECH(CNBR) -[ 2003/3/31 Bugzilla#78471 Add 3 Controls.
	{g_s_IEXP_LAYERS,
		{g_s_IEXP_GENERAL},
		1,{0},{EXP_LAYERNAME,IDC_BUTTON1,EXP_LINETYPE,EXP_LWEIGHT,EXP_ON,EXP_FREEZE,EXP_LOCK,EXP_PLOT,EXP_NEWVPFREEZE},
		{EXP_LAYER}}, 
	//{g_s_IEXP_LAYERS,
	//	{g_s_IEXP_GENERAL},
	//	1,{0},{EXP_LAYERNAME,IDC_BUTTON1,EXP_LINETYPE,EXP_ON,EXP_FREEZE,EXP_LOCK},
	//	{EXP_LAYER}}, 
	{g_s_IEXP_LINETYPES,
		{g_s_IEXP_GENERAL},
		1,{0},{EXP_LINE_NAME,EXP_LINE_DESC,EXP_LINE_DEF},
		{EXP_LINE}},
	{g_s_IEXP_STYLES,
		{g_s_IEXP_GENERAL},
		1,{0},{EXP_STYLE_NAME,EXP_STYLE_HEIGHT,EXP_STYLE_WIDTH,EXP_STYLE_OBLIQUE,
		// EBAETCH(CNBR) -[ Add Language and Bigfont
		//EXP_STYLE_FONT_NAME,EXP_STYLE_FONT_STYLE,
		EXP_STYLE_FONT_NAME,EXP_STYLE_FONT_STYLE,EXP_STYLE_FONT_LANGUAGE,EXP_STYLE_BIGFONT_NAME,
		// EBATECH(CNBR) ]-
		EXP_STYLE_BACKWARDS,EXP_STYLE_UPSIDE_DOWN,EXP_STYLE_VERTICALLY,
		EXP_STYLE_PREVIEW,EXP_STYLE_SAMPLE_TEXT},
		{EXP_STYLE}},
	{g_s_IEXP_COORDSYSTEM,
		{g_s_IEXP_GENERAL},
		1,{0},{EXP_UCS_NAME,EXP_UCS_ORIGIN,EXP_UCS_XDIR,EXP_UCS_YDIR},
		{EXP_UCS}},
	{g_s_IEXP_VIEWS,
		{g_s_IEXP_GENERAL,
		g_s_IEXP_VIEWLENS,
		g_s_IEXP_VIEWMODE,
		g_s_IEXP_VIEWTARGET},
		4,{0},{EXP_VIEW_NAME,EXP_VIEW_HEIGHT,EXP_VIEW_WIDTH,EXP_VIEW_CENTERPT,
		EXP_VIEW_TWIST,EXP_VIEW_LENSLG,EXP_VIEW_FRONTCL,EXP_VIEW_BACKCL,
		EXP_VIEW_MODE1,EXP_VIEW_MODE2,EXP_VIEW_MODE4,EXP_VIEW_MODE8,EXP_VIEW_MODE16,
		EXP_VIEW_TARGETPT,EXP_VIEW_DIRTARGET},
		{EXP_VIEW,EXP_VIEW_LENS,EXP_VIEW_MODE,EXP_VIEW_TARGET}},
	{g_s_IEXP_BLOCKS,
		{g_s_IEXP_GENERAL},
		1,{0},{EXP_BLOCK_NAME,EXP_BLOCK_BASEPT,EXP_BLOCK_EXTERNAL},
		{EXP_BLOCK}},
	{g_s_IEXP_DIMSTYS,
		{g_s_IEXP_GENERAL},
		1,{0},{0},{EXP_NOPROP}},
	{g_s_IEXP_ITEMTYPES,
		{g_s_IEXP_GENERAL},
		1,{0},{0},{0}}
	};


struct IEXP_OpenDocs {
	char				 *szDrawing;
	short				  nID;

	db_drawing			 *m_dbDrawing;			// Pointer to FILELINK struct.
	CIcadDoc			 *m_pIcadDoc;			// Pointer to the main MFC ICAD Doc.
	HTREEITEM			  hItem;	   // (VIEW).
	struct IEXP_OpenDocs *next;
} *IEXP_ODFirstNode,
  *IEXP_ODNode;



#define COLEDIT_NONE			0
#define COLEDIT_EDITBOX			1
#define COLEDIT_COMBOBOX		2
#define COLEDIT_DIALOG			3
#define COLEDIT_TOGGLE			4
#define COLEDIT_FONTCOMBOBOX	5
#define COLEDIT_STYLECOMBOBOX	6
#define COLEDIT_LANGCOMBOBOX	7	// EBATECH(CNBR)
#define COLEDIT_BIGFONTCOMBOBOX	8	// EBATECH(CNBR)
#define COLEDIT_LWCOMBOBOX		9	// EBATECH(CNBR)


struct IEXP_EntityColumn
	{
	char		*cType;
	short		IType;						// Type of view (VIEW).
	char		*resname;					// (VIEW).
	int			IconID,						// The ID of the Icon when not selected (VIEW).
				SIconID;					// The ID of the Icon when selected (VIEW).

	int			Cnt;						// Number of columns. (VIEW).
	// EBATECH(CNBR) 9->11
	//int			Order[9][2];			// [Order Number(VIEW),Column Length(VIEW)]
	//const char	*ColumnNames[9];		// Title of the columns (VIEW).
	//int			ColEditType[9];			// Type of Editing supported for the subitem
	int			Order[11][2];				// [Order Number(VIEW),Column Length(VIEW)]
	const char	*ColumnNames[11];			// Title of the columns (VIEW).
	int			ColEditType[11];			// Type of Editing supported for the subitem
	// EBATECH(CNBR) ]-

	HTREEITEM	hITem;						// (VIEW).
	int			IconOff,					// Icon offset number (VIEW).
				SIconOff;					// Icon offset number (VIEW).
	char		CurrentName[IC_ACADBUF];	// Name of the current row (VIEW)
	} IEXP_EntityCol[] =
	{
//	{cType,  IType,			resname,	IconID,		SIconID			Cnt,{order - column sizes and order}, column names.
	// EBATECH(CNBR) -[ 2003/3/31 Bugzilla#78471 Add 2 columns, Use macro.
	{"LAYER",IEXP_LATAB_IDX,"EXPLSTLAY",EXP_LAYER,	EXP_LAYER_1,
		10,{
		{IEXP_LA_NAME,140},
		{IEXP_LA_COLOR,70},
		{IEXP_LA_LINETYPE,99},
		{IEXP_LA_ON_OFF,50},
		{IEXP_LA_LOCKED,50},
		{IEXP_LA_FROZEN,50},
		{IEXP_LA_LINEWEIGHT,70},
		{IEXP_LA_PLOT,50},
		{IEXP_LA_VP_FREEZECUR,50},
		{IEXP_LA_VP_FREEZENEW,50}
		},
		{g_s_IEXP_LAYER_NAME,
		g_s_IEXP_COLOR,
		g_s_IEXP_LINETYPE,
		g_s_IEXP_ON_OFF,
		g_s_IEXP_LOCKED,
		g_s_IEXP_FROZEN,
		g_s_IEXP_LINEWEIGHT,
		g_s_IEXP_PLOT,
		g_s_IEXP_VP_FREEZECUR,
		g_s_IEXP_VP_FREEZENEW,
		NULL},
		{COLEDIT_NONE,
		COLEDIT_DIALOG,
		COLEDIT_DIALOG,
		COLEDIT_TOGGLE,
		COLEDIT_TOGGLE,
		COLEDIT_TOGGLE,
		COLEDIT_LWCOMBOBOX,
		COLEDIT_TOGGLE,
		COLEDIT_TOGGLE,
		COLEDIT_TOGGLE,
		0},
		0
	},
	// EBATECH(CNBR) ]-
	{"LTYPE",IEXP_LTTAB_IDX,"EXPLSTLT", EXP_LTYPE,	EXP_LTYPE_1,
		3,{{0,112},{1,150},{2,280}},
		{g_s_IEXP_LINETYPE_NAME,
		g_s_IEXP_DESCRIPTIVE_TXT,
		g_s_IEXP_LINETYPE_DEF,
		NULL},
		{COLEDIT_NONE,
		COLEDIT_EDITBOX,
		COLEDIT_NONE,
		0},
		0
	},
	{"STYLE",IEXP_TSTAB_IDX,"EXPLSTTS", EXP_STYLES,	EXP_STYLES_1,
		11,{
			{IEXP_TS_NAME,112},
			{IEXP_TS_HEIGHT,50},
			{IEXP_TS_WIDTH,50},
			{IEXP_TS_SLANT,50},
			{IEXP_TS_FONT,75},
			{IEXP_TS_STYLE,75},
			{IEXP_TS_LANG,75},
			{IEXP_TS_BIGFONT,75},
			{IEXP_TS_BACKWORD,50},
			{IEXP_TS_UPSIDEDOWN,50},
			{IEXP_TS_VERTICAL,50}},
		{g_s_IEXP_TEXT_STYLE_NAME,
		g_s_IEXP_HEIGHT,
		g_s_IEXP_WIDTH_FACTOR,
		g_s_IEXP_OBLIQUE_ANGLE,
		g_s_IEXP_FONT_NAME,
		g_s_IEXP_FONT_STYLE,
		g_s_IEXP_FONT_LANGUAGE,
		g_s_IEXP_BIGFONT_NAME,
		g_s_IEXP_TEXT_BACKWARDS,
		g_s_IEXP_TEXT_UPSIDEDOWN,
		g_s_IEXP_TEXT_VERTICALLY},
		{COLEDIT_NONE,
		COLEDIT_EDITBOX,
		COLEDIT_EDITBOX,
		COLEDIT_EDITBOX,
		COLEDIT_FONTCOMBOBOX,
		COLEDIT_STYLECOMBOBOX,
		COLEDIT_LANGCOMBOBOX,
		COLEDIT_BIGFONTCOMBOBOX,
		COLEDIT_TOGGLE,
		COLEDIT_TOGGLE,
		COLEDIT_TOGGLE},
		0
	},
	// EBATECH(CNBR) ]-
	{"UCS",	 IEXP_CSTAB_IDX,"EXPLSTUCS",EXP_UCS,	EXP_UCS_1,
		4,{{0,112},{1,112},{2,119},{3,119}},
		{g_s_IEXP_UCS_NAME,
		g_s_IEXP_ORIGIN,
		g_s_IEXP_X_DIRECTION,
		g_s_IEXP_Y_DIRECTION,
		NULL},
		{COLEDIT_NONE,
		COLEDIT_EDITBOX,
		COLEDIT_NONE,
		COLEDIT_NONE,
		0},
		0
	},
	{"VIEW", IEXP_VWTAB_IDX,"EXPLSTVW", EXP_VIEW,	EXP_VIEW_1,
		4,{{0,112},{1,98},{2,98},{3,224}},
		{g_s_IEXP_VIEW_NAME,
		g_s_IEXP_HEIGHT,
		g_s_IEXP_WIDTH,
		g_s_IEXP_VIEW_DIRECTION,
		NULL},
		{COLEDIT_NONE,
		COLEDIT_EDITBOX,
		COLEDIT_EDITBOX,
		COLEDIT_EDITBOX,
		0},
		0
	},
	{"BLOCK",IEXP_BNTAB_IDX,"EXPLSTBLK",EXP_BLOCKS,	EXP_BLOCKS_1,
		4,{{0,112},{1,50},{2,125},{3,225}},
		{g_s_IEXP_BLOCK_NAME,
		g_s_IEXP_NUM_BLOCK_REF,
		g_s_IEXP_BLOCK_INSPOINT,
		g_s_IEXP_XREF,
		NULL},
		{COLEDIT_NONE,
		COLEDIT_NONE,
		COLEDIT_EDITBOX,
		COLEDIT_DIALOG,
		0},
		0
	},
	{"DIMSTYLE",IEXP_DSTAB_IDX,"EXPLSTDS",EXP_DIMSTY,EXP_DIMSTY_1,
		1,{{0,112}},
		{g_s_IEXP_DIMSTY_NAME,NULL},
		{COLEDIT_NONE,0},
		0
	},
	{"",	 -1,			"",			0,			0,
		2,{{0,120},{1,112}},
		{g_s_IEXP_ITEM_TYPE,
		g_s_IEXP_NUM_ITEMS,
		NULL},
		{COLEDIT_NONE,
		COLEDIT_NONE,
		0},
		0
	}
	};

// ======================================================================================================================================================================
// ============================================================== GLOBAL VARIABLES ======================================================================================
// ======================================================================================================================================================================

LV_FINDINFO    IEXP_Findinfo;
UINT		   IEXP_Clipboard;
BOOL		   IEXP_InsertFlag,
			   IEXP_sorttype;
short		   IEXP_LayerCnt,
			   IEXP_Sortitem,
			   IEXP_LinetCnt,
			   IEXP_TStyleCnt,
			   IEXP_ViewCnt,
			   IEXP_UCSCnt,
			   IEXP_BlockCnt,
			   IEXP_DimStyleCnt;
int 		   IEXP_WindowIcon;
extern WNDPROC ICAD_OrigWndProcT;				   // Used to hold the default windows procedure for the tack button in the property edit box.
extern WNDPROC ICAD_OrigWndProcH;				   // Used to hold the default windows procedure for the help button in the property edit box.
short		   IEXP_CurrentIndx,				   // The current item selected in the (CTreeCtrl) in the left window.
			   IEXP_CurrentPIndx;
HWND		   IEXP_hWndView,					   // Global handle to the (CListrCtrl), the right window.
			   IEXP_hWndList,					   // Global handle to the (CTreeCtrl), the left window.
			   IEXP_hParent;					   // Global handle to the icad window.
char		   IEXP_szText[IC_ACADBUF], 				  // Global Text, used for any purpose.
			   IEXP_szTextA[IC_ACADBUF];
WNDPROC 	   IEXP_SubPaintProc;				   // Used to hold the default windows procedure for the message box.
bool		   IEXP_RegenOnUndo;
short		   IEXP_NumofTypes=sizeof(IEXP_EntityCol)/sizeof(IEXP_EntityCol[0]); // Total number of the items.
// The list of ID's for the main toolbar.
static UINT BASED_CODE IEXP_MainTBId[]={EXP_NEW_T,EXP_CURRENT_M,ID_SEPARATOR,EXP_CUT,EXP_COPY,EXP_PASTE,ID_SEPARATOR,EXP_DELETE_M,EXP_PROPERTIES,EXP_UCS_WORLD,EXP_PURGE,ID_SEPARATOR,EXP_REGEN,EXP_REGEN_SWITCH};
// The list of ID's for the layer toolbar.
static UINT BASED_CODE IEXP_LayerTBId[]={EXP_LAYER_ON,EXP_LAYER_FREEZE,EXP_LAYER_LOCK};
// The list of ID's for the blocks toolbar.
static UINT BASED_CODE IEXP_BlockTBId[]={EXP_BLOCK_IMAGES,EXP_BLOCK_DETAILS,ID_SEPARATOR,EXP_BLOCK_INSERT,EXP_BLOCK_FILE,EXP_BLOCK_SAVE,EXP_BLOCK_XREF};
// The indicator id for the status bar.
static UINT indicators[] ={ID_SEPARATOR};
bool IEXP_CdetailView::m_ShowXrefLayers = false;	//controls visibility of layers in xref files in explorer window. 
static int		IEXP_lwunits;						// 2003/3/31 Keep $LWDISPLAY for build LAYER/LineWeight DropDownList.

// =======================================================================================
// =========================	BLOCK HELPER FUNCTIONS	  ================================
// =======================================================================================

/*----------------------------------------------------------------------------*/
BOOL		IsBlockSubNested
(
const char		*BlockName,
db_insert	*Insert
)
{
	db_blocktabrec *bhip=NULL;
	db_handitem *fhip=NULL;
	char *EntName;
	int type;

	bhip = Insert->ret_bthip();
	if (bhip == NULL)
		return FALSE;

	for (bhip->get_firstentinblock(&fhip); fhip!=NULL; fhip=fhip->next)
	{
		type = fhip->ret_type();
		if (type == DB_INSERT)
		{
			fhip->get_2 (&EntName);
			if (EntName)
			{
				if (strsame(BlockName, EntName))
					return TRUE;
			}

			if (IsBlockSubNested (BlockName, (db_insert *)fhip))
				return TRUE;
		}
	}
	return FALSE;
}

/*----------------------------------------------------------------------------*/
BOOL	IsBlockNested
(
const char	*BlockName
)
{
	struct resbuf  *rb;
	db_handitem *fhip=NULL;
	db_blocktabrec *bhip=NULL;
	char *EntName;
	int type;

	rb=SDS_tblgetter("BLOCK"/*DNT*/,NULL,TRUE,IEXP_ODNode->m_dbDrawing);
	while(rb)
	{
		//Get the handitem for the block
		if (ic_assoc(rb,-1))
			return FALSE;
		bhip = (db_blocktabrec *)ic_rbassoc->resval.rlname[0];

		//for all entities in the block, check if it has a nested insert
		for (bhip->get_firstentinblock(&fhip); fhip!=NULL; fhip=fhip->next)
		{
			type = fhip->ret_type();
			if (type == DB_INSERT)
			{
				fhip->get_2 (&EntName);
				if (EntName)
				{
					if (strsame(BlockName, EntName))
						return TRUE;
				}

				if (IsBlockSubNested (BlockName, (db_insert *)fhip))
					return TRUE;
			}
		}

		sds_relrb(rb);
		rb=SDS_tblgetter("BLOCK"/*DNT*/,NULL,FALSE,IEXP_ODNode->m_dbDrawing);
	}

	return FALSE;
}

/*int GetBlockCount (char *BlockName, db_drawing *dbDrawing, BOOL bTopLevelOnly);

int GetSubNestedBlockCount (char *BlockName, db_handitem *Block, db_drawing *dbDrawing)
{
	db_handitem *bhip=NULL, *fhip=NULL;
	char *EntName, *ParName;
	int type;
	int Cnt = 0, CntSubNested = 0, ParCnt = 0;

	bhip = Block->ret_bthip();
	if (bhip == NULL)
		return Cnt;

	bhip->get_2 (&ParName);
	ParCnt = GetBlockCount (ParName, dbDrawing, TRUE);

	for (bhip->get_neg2(&fhip); fhip!=NULL; fhip=fhip->next)
	{
		type = fhip->ret_type();
		if (type == DB_INSERT)
		{
			fhip->get_2 (&EntName);
			if (EntName)
			{
				if (strsame(BlockName, EntName))
					Cnt+=ParCnt;
			}

			CntSubNested = GetSubNestedBlockCount (BlockName, fhip, dbDrawing);
			Cnt += CntSubNested;
		}
	}
	return Cnt;
}
*/

/*-------------------------------------------------------------------------*//**
*//*--------------------------------------------------------------------------*/
int			GetBlockCount
(
const char		*BlockName,
db_drawing	*dbDrawing,
BOOL		bTopLevelOnly
)
{
	struct resbuf  *rb;
	db_handitem *fhip=NULL;
	db_blocktabrec *bhip=NULL;
	db_handitem *temphip=NULL;
	char *EntName, *ParName;
	int type;
	int Cnt = 0, CntSubNested = 0, ParCnt = 0;

	if (!BlockName) return Cnt;

	//Find the number of inserts in this drawing
	while((temphip=dbDrawing->entnext_noxref(temphip))!=NULL)
	{
		if (temphip->ret_type() == DB_INSERT)
		{
			temphip->get_2(&EntName);
			if (!strsame(BlockName,EntName)) continue;
			Cnt++;
		}
	}

	if (bTopLevelOnly) return Cnt;

	//Now find the number of nested inserts of this block
	rb=SDS_tblgetter("BLOCK"/*DNT*/,NULL,TRUE,dbDrawing);
	while(rb)
	{
		//Get the handitem for the block
		if (ic_assoc(rb,-1))
		{
			sds_relrb(rb);
			return Cnt;
		}

		bhip = (db_blocktabrec *)ic_rbassoc->resval.rlname[0];
		bhip->get_2 (&ParName);
		ParCnt = GetBlockCount (ParName, dbDrawing, TRUE);

		//for all entities in the block, check if it has a nested insert
		for (bhip->get_firstentinblock(&fhip); fhip!=NULL; fhip=fhip->next)
		{
			type = fhip->ret_type();
			if (type == DB_INSERT)
			{
				fhip->get_2 (&EntName);
				if (EntName)
				{
					if (strsame(BlockName, EntName))
						Cnt+=ParCnt;
				}

				//CntSubNested = GetSubNestedBlockCount (BlockName, fhip, dbDrawing);
				//Cnt += CntSubNested;
			}
		}

		sds_relrb(rb);
		rb=SDS_tblgetter("BLOCK"/*DNT*/,NULL,FALSE,dbDrawing);
	}

	//Weird situation here.  Since this code has to do an SDS_tblgetter (which calls tblnext), it
	//messes up the next pointer in the table.	So if the caller of this function wants to find the
	//next block, it will not be found - the next pointer has been set to the end of the table.
	//So we rewind the table, then cycle through it until we find the Block that was passed into this
	//function intially.  Now the table is in the same condition as it was when we started.
	rb=SDS_tblgetter("BLOCK"/*DNT*/,NULL,TRUE,dbDrawing);
	while(rb)
	{
		//Get the handitem for the block
		if (ic_assoc(rb,-1))
		{
			sds_relrb(rb);
			return Cnt;
		}
		bhip = (db_blocktabrec *)ic_rbassoc->resval.rlname[0];
		bhip->get_2 (&ParName);
		if (strsame(BlockName, ParName))
		{
			sds_relrb(rb);
			break;
		}
		sds_relrb(rb);
		rb=SDS_tblgetter("BLOCK"/*DNT*/,NULL,FALSE,dbDrawing);
	}

	return Cnt;
}

/*-------------------------------------------------------------------------*//**
This function cleans up entities inside blocks - either deleting or changing
entities according to specified criteria. For example, if a layer has been
deleted, this function will walk through all blocks ensuring that all member
entities of these blocks that reference that layer are also deleted.

@author ?
@param	ItemType		 => Type of attribute of the block's members need cleanup - Layer, Linetype or Style
@param	ItemName		 => Value of the Attribute that needs cleanup
@param	ActionType		 => Type of cleanup - Change or Delete
@param	NewItemName		 => If type of Cleanup is Change, this is the new value of the attribute, else NULL
@param	drawing			 => The drawing being modified
@return ?
*//*--------------------------------------------------------------------------*/
BOOL		CleanupEntsInsideBlocks
(
int			ItemType,
CString		ItemName,
int			ActionType,
CString		NewItemName,
db_drawing	*drawing
)
{
	struct resbuf	*rb = NULL;
	struct resbuf	*rbb = NULL;
	struct resbuf	*rbt = NULL;
	db_handitem		*fhip=NULL;
	char			item[IC_ACADBUF];
	sds_name		ename;
	int				type;
	db_blocktabrec	*bhip=NULL;

	//Input validation
	if ((ItemType != IEXP_LATAB_IDX) &&
		(ItemType != IEXP_LTTAB_IDX) &&
		(ItemType != IEXP_TSTAB_IDX) &&
		(ItemType != IEXP_DSTAB_IDX)
		)
		return FALSE;

	if (ActionType == EXP_DEL_CHANGE)
		{
		if (NewItemName.IsEmpty())
			{
			ASSERT(0);
			ActionType = EXP_DEL_CONTINUE;
			}
		}

	//Walk all blocks in the drawing
	rb=SDS_tblgetter("BLOCK",NULL,TRUE,drawing);
	while(rb)
	{
		//Get the handitem for the block
		if (ic_assoc(rb,-1))
			return FALSE;
		bhip = (db_blocktabrec *)ic_rbassoc->resval.rlname[0];

		#ifdef DEBUG	//the name of the block is useful when debugging
		char *EntName, *ParName;
		bhip->get_2 (&ParName);
		#endif

		//for all entities in the block, check if it has a nested insert
		/*DG - 15.5.2002*/// Process subents including block&endblk entities as well.
		for(bhip->get_blockllends(&fhip, NULL); fhip; fhip = fhip->next)
//		for (bhip->get_firstentinblock(&fhip); fhip!=NULL; fhip=fhip->next)
		{
			#ifdef DEBUG
			fhip->get_2 (&EntName);
			#endif

			if (fhip->ret_deleted()) continue;	// skip deleted subentities

			switch (ItemType)
				{
				case IEXP_LATAB_IDX:
					fhip->get_8(item,255);
					type = 8;
					break;
				case IEXP_LTTAB_IDX:
					fhip->get_6(item,255);
					type = 6;
					break;
				case IEXP_TSTAB_IDX:
					fhip->get_7(item,255);
					type = 7;
 					break;
				case IEXP_DSTAB_IDX:
					switch(fhip->ret_type())
					{
					case DB_DIMENSION:
					case DB_LEADER:
					case DB_TOLERANCE:
						fhip->get_3(item,255);
						type = 3;
						break;
					default:
						item[0]=NULL;
						type=0;
					}
					break;
				default:
					item[0]=NULL;
				}

			if (strcmp(ItemName,item)) continue;

			/*DG - 15.5.2002*/// Process subents including block&endblk entities as well.
//			if( fhip->ret_type() == DB_ENDBLK )
//				continue;

			ename[1]=(long)drawing;
			ename[0]=(long)fhip;

			switch (ActionType)
				{
				case EXP_DEL_CONTINUE:
					sds_entdel(ename);
					break;
				case EXP_DEL_CHANGE:
					rbb=sds_entget(ename);
					for(rbt=rbb;rbt!=NULL && rbt->restype!=type;rbt=rbt->rbnext);
					if (rbt!=NULL)
						{
						rbt->resval.rstring=ic_realloc_char(rbt->resval.rstring, NewItemName.GetLength()+1);
						strcpy(rbt->resval.rstring,(LPCSTR)(NewItemName));
						sds_entmod(rbb);
						}
					sds_relrb (rbb);
					break;
				}
		}

		sds_relrb(rb);
		rb=SDS_tblgetter("BLOCK",NULL,FALSE,drawing);
	}

	return TRUE;
}

/*-------------------------------------------------------------------------*//**
This function extracts a Point from a string formatted with 3 comma/semicolon
separated numbers eg. num,num,num.

@author	?
@param	String	 =>
@param	pt		<=
@return	TRUE if success, and FALSE if string is of incorrect format
*//*--------------------------------------------------------------------------*/
BOOL		ExtractPointFromCString
(
CString		String,
sds_point	pt
)
{
	CString substring, remstring;
	int len, sublen;

	len = String.GetLength();

	substring = String.SpanExcluding (",;");
	sublen = substring.GetLength() + 1;
	//pt[0] = atof (substring);
	if (!ic_isnumstr(substring.GetBuffer(0), &pt[0]))
		 return FALSE;

	remstring = String.Right(len - sublen);
	substring = remstring.SpanExcluding (",;");
	sublen += substring.GetLength() + 1;
	//pt[1] = atof (substring);
	if (!ic_isnumstr(substring.GetBuffer(0), &pt[1]))
		 return FALSE;

	remstring = String.Right(len - sublen);
	//pt[2] = atof (remstring);
	if (!ic_isnumstr(remstring.GetBuffer(0), &pt[2]))
		 return FALSE;

	return TRUE;
}



// ======================================================================================================================================================================
// ======================================================= IEXP_CDETAILVIEW CLASS FUNCTIONS =============================================================================
// ======================================================================================================================================================================

IMPLEMENT_DYNCREATE(IEXP_CdetailView,CFrameWnd/*CWnd*/)

BEGIN_MESSAGE_MAP(IEXP_CdetailView, CFrameWnd/*CWnd*/)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_WM_CHAR()
END_MESSAGE_MAP()

/*-------------------------------------------------------------------------*//**
*//*--------------------------------------------------------------------------*/
afx_msg void	IEXP_CdetailView::OnChar
(
UINT			nChar,
UINT			nRepCnt,
UINT			nFlags)
{
	if (nChar==VK_ESCAPE)
	{
		GetParent()->GetParent()->SendMessage( WM_CHAR, nChar, 0);
	}
	else
	{
		CFrameWnd::OnChar(nChar, nRepCnt, nFlags);
	}
}

/*-------------------------------------------------------------------------*//**
*//*--------------------------------------------------------------------------*/
void	IEXP_CdetailView::OnClose
(
)
{
//	  m_pimagelist.DeleteImageList();
//	  CWnd::OnClose();
	CFrameWnd::OnClose();
}

/*-------------------------------------------------------------------------*//**
*//*--------------------------------------------------------------------------*/
void	IEXP_CdetailView::OnSetFocus
(
CWnd	*pOldWnd
)
{
//	  CWnd::OnSetFocus(pOldWnd);
	CFrameWnd::OnSetFocus(pOldWnd);
	m_Windowsideflag=FALSE;
}

/*-------------------------------------------------------------------------*//**
This function creates all the (TYPE) windows,adds the columns for
each (TYPE) then adds rows for each (TYPE) window.It also sets the
first window active.

@author	?
@param	lpCreateStruct	 => Points to a CREATESTRUCT structure that contains
							information about the CWnd object being created.
@return 0 : successful.
*//*--------------------------------------------------------------------------*/
int				IEXP_CdetailView::OnCreate
(
LPCREATESTRUCT	lpCreateStruct
)
{
	int			  index,
				  Item;
	LV_COLUMN	  lvC;			  // list view column structure
	LV_ITEM 	  lvI;		  	  // list view item structure
	struct resbuf rbb;
   	CBitmap		  bitmap;
	RECT		  rect;

	m_Explorer=NULL;
	IEXP_RegenOnUndo=FALSE;
	m_pBlockName = NULL;	/*DG - 6.5.2002*/

	// Call the default OnCreate function.
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1) return -1;

	// Set up the mask and default variables.
	lvI.mask			=LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
	lvI.state			=0;
	lvI.stateMask		=0;
	lvC.mask			=LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvC.fmt				=LVCFMT_LEFT;
	m_DlgTack=m_DlgState=FALSE;
	m_dataptr			=0;
	IEXP_CurrentIndx	=m_ItemIndex=0;
	rbb.resval.rstring	=NULL;
	m_selflag			=FALSE;
	m_regenstate		=FALSE;
	IEXP_Findinfo.flags =LVFI_STRING;
	m_EditDlg			=NULL;
	m_Purge				=FALSE;

	GetWindowRect(&rect);
	m_lpoint.x=rect.left;
	m_lpoint.y=rect.top;

	// This creates the static bar on both sides of the spliter window.
	if (!m_staticIvbtm.Create(NULL,SS_SIMPLE|SS_NOPREFIX|WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,IEXP_SBAR_BIV)) return -1;
	if (!m_staticIvbar.Create(NULL,SS_SUNKEN|SS_NOPREFIX|WS_CHILD|WS_VISIBLE|SS_LEFTNOWORDWRAP,CRect(0,0,0,0),this,IEXP_SBAR_IV)) return -1;

	// Create bitmap image list for the icon view for blocks.
	m_pimagelistI.Create(IEXP_ICON_BITMAP_WIDTH,IEXP_ICON_BITMAP_HEIGHT,TRUE,1,0);

	// Create bitmap image list for all of the report views.
	m_pimagelist.Create(IEXP_BITMAP_WIDTH,IEXP_BITMAP_HEIGHT,TRUE,1,0);
	// Load the bitmap and add it to the imagelist class.
	bitmap.LoadBitmap(EXP_CHECK);
	m_pimagelist.Add(&bitmap,(COLORREF)IEXP_BITMASK);
	bitmap.DeleteObject();

	for(index=0;index<(IEXP_NumofTypes-1);++index) {
   		bitmap.LoadBitmap(IEXP_EntityCol[index].IconID);
		IEXP_EntityCol[index].IconOff =m_pimagelist.Add(&bitmap,(COLORREF)0xFFFFFF);
		bitmap.DeleteObject();
   		bitmap.LoadBitmap(IEXP_EntityCol[index].SIconID);
		m_pimagelist.Add(&bitmap,(COLORREF)0xFFFFFF);
		bitmap.DeleteObject();
	}

	BOOL bOwnerDrawn;
	BOOL bAddImages;
	BOOL bEditLabels;
	int style;
	// There is a list view window for every (TYPE) so we must create the list view window for as many types there are.
	for(index=0;index<IEXP_NumofTypes;++index)
	{
		bOwnerDrawn = FALSE;
		bAddImages = TRUE;
		bEditLabels = TRUE;
		switch(index)
		{
			case IEXP_LATAB_IDX: bOwnerDrawn = TRUE;	break;
			case IEXP_LTTAB_IDX: bOwnerDrawn = TRUE;	break;
			case IEXP_TSTAB_IDX:						break;
			case IEXP_CSTAB_IDX:						break;
			case IEXP_VWTAB_IDX:						break;
			case IEXP_BNTAB_IDX: bAddImages = FALSE;	break;
			case IEXP_DSTAB_IDX:						break;
			case IEXP_XDATA_IDX: bEditLabels = FALSE;	break;
		}

		//All controls are now owner drawn.  Else, there were UI inconsistencies.  Default ListCtrls show the
		//entities highlighted in gray when the control has no focus.  OwnerDrawn ListCtrls show them in Blue
		//always which is what is desired.
		style = WS_VISIBLE|WS_CHILD|WS_BORDER|LVS_REPORT|LVS_SHOWSELALWAYS|LVS_OWNERDRAWFIXED;
		if (bEditLabels) style |= LVS_EDITLABELS;

		// Create the items list in the (CListCtrl)
		m_ListCtl[index].Create(style,CRect(0,0,0,0),this,IEXP_LISTVIEW);

		if (bAddImages)
			m_ListCtl[index].SetImageList(&m_pimagelist,LVSIL_SMALL);
	}

	// We must add the columns for every (TYPE) window that was created.
	for(index=0;index<IEXP_NumofTypes;++index) {
		for (Item=0;Item<IEXP_EntityCol[index].Cnt;++Item) {
			lvC.iSubItem= Item;
			lvC.cx		= (IEXP_EntityCol[index].Order[Item][IEXP_COLUMNWIDTH]);
			lvC.pszText = (char *) IEXP_EntityCol[index].ColumnNames[Item];
			if (m_ListCtl[index].InsertColumn(Item,&lvC)==-1) return NULL;
		}
	}
	if (!UpdateDataList()) return(-1);

	IEXP_CurrentPIndx=100;

	return 0;
}

/*-------------------------------------------------------------------------*//**
*//*--------------------------------------------------------------------------*/
BOOL	IEXP_CdetailView::UpdateDataList
(
void
)
{

	m_selflag			=FALSE;
	m_regenstate		=FALSE;
	m_dataptr			=0;
	IEXP_LayerCnt		=0;
	IEXP_LinetCnt		=0;
	IEXP_TStyleCnt		=0;
	IEXP_ViewCnt		=0;
	IEXP_UCSCnt 		=0;
	IEXP_BlockCnt		=0;
	IEXP_DimStyleCnt	=0;

	//First clean up any existing data in the list Ctrl so as to prevent memory leaks
	for(int fi1 = 0; fi1 < (IEXP_NumofTypes-1);++fi1)
	{
		for(int fi3 = 0, fi2 = (m_ListCtl[fi1].GetItemCount()); fi3 < fi2; ++fi3)
			delete (IEXP_ItemDef *)m_ListCtl[fi1].GetItemData(fi3);
	}

	IEXP_ItemDefPtr = NULL;

	for(fi1=0;fi1<IEXP_NumofTypes;++fi1)
	{
		m_ListCtl[fi1].DeleteAllItems();
	}

	m_pimagelistI.DeleteImageList();
	m_pimagelistI.Create(IEXP_ICON_BITMAP_WIDTH,IEXP_ICON_BITMAP_HEIGHT,TRUE,1,0);
	m_ListCtl[IEXP_BNTAB_IDX].SetImageList(NULL,LVSIL_NORMAL);


	if(IEXP_CurrentIndx >=0 && IEXP_CurrentIndx < 8)
	{
		//Now update the current list ctrl with the new items
		m_ListCtl[IEXP_CurrentIndx].initData(this, IEXP_CurrentIndx);
	}
	if(m_Explorer != NULL && IEXP_ODNode != NULL)
	{
		resbuf rm;
		SDS_getvar(NULL, DB_QREGENMODE, &rm, IEXP_ODNode->m_dbDrawing, NULL, NULL);
		m_Explorer->m_regenMode = rm.resval.rint;
		m_Explorer->m_MToolBar.GetToolBarCtrl().PressButton(EXP_REGEN_SWITCH, rm.resval.rint == 0 ? FALSE : TRUE);
	}
	return(TRUE);
}

void IEXP_CListCtrl::initData(IEXP_CdetailView* view, int Mindex)
{
	if(m_bInit)
		return;

	m_bInit = true;
	// Set up the mask and default variables.
	LV_ITEM 	   lvI; 	   	  // list view item structure
	lvI.mask			=LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
	lvI.state			=0;
	lvI.stateMask		=0;

	struct resbuf rbb;

	// EBATECH(CNBR) -[ 2003/3/31 Bugzilla#78471 Keep $LWDISPLAY for build LAYER/LINEWEIGHT DropDownListBox
	IEXP_lwunits = 1;
	if (SDS_getvar(NULL,DB_QLWUNITS,&rbb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES) == RTNORM)
		IEXP_lwunits = rbb.resval.rint;
	// EBATECH(CNBR) ]-

	// EBATECH(CNBR) 2003/4/30 Clear rbb to prevent unrespected string operation.
	rbb.resval.rstring = NULL;

	// A point to the parent in each (CListCtrl) .
	m_CdetailView=view;
	// Get the current item set.
	switch(IEXP_EntityCol[Mindex].IType)
	{
		case -1:		break;
		case IEXP_DSTAB_IDX: SDS_getvar(NULL,DB_QDIMSTYLE,&rbb,IEXP_ODNode->m_dbDrawing,NULL,NULL);  break;
		case IEXP_CSTAB_IDX: SDS_getvar(NULL,DB_QUCSNAME,&rbb,IEXP_ODNode->m_dbDrawing,NULL,NULL);	 break;
		case IEXP_TSTAB_IDX: SDS_getvar(NULL,DB_QTEXTSTYLE,&rbb,IEXP_ODNode->m_dbDrawing,NULL,NULL); break;
		case IEXP_LTTAB_IDX: SDS_getvar(NULL,DB_QCELTYPE,&rbb,IEXP_ODNode->m_dbDrawing,NULL,NULL);	 break;
		case IEXP_LATAB_IDX: SDS_getvar(NULL,DB_QCLAYER,&rbb,IEXP_ODNode->m_dbDrawing,NULL,NULL);	 break;
	}

	if (Mindex!=7)
	{
		short rindex=0;
		int index=0;
		struct resbuf* rb = SDS_tblgetter(IEXP_EntityCol[Mindex].cType,NULL,TRUE,IEXP_ODNode->m_dbDrawing);
		while(rb)
		{
			//Added Cybage AJK 10/12/2001 [
			//Reason : Fix for bug no 77922
			if (IEXP_CdetailView::m_ShowXrefLayers ==0)
			{
				if (strcmp(IEXP_EntityCol[Mindex].cType,"LAYER") == 0	&& ( ic_assoc(rb,2) == 0 ) && (strchr(ic_rbassoc->resval.rstring,IEXP_VER_BAR)) != NULL)
				{
					sds_relrb(rb);
					rb=SDS_tblgetter(IEXP_EntityCol[Mindex].cType,NULL,FALSE,IEXP_ODNode->m_dbDrawing);
					continue;
				}
			}
			//Added Cybage AJK 10/12/2001 DD/MM/YYYY ]
			lvI.iImage=-1;
			switch(IEXP_EntityCol[Mindex].IType)
			{
				case IEXP_DSTAB_IDX: if (view->DimStyleIns(rindex,index,Mindex,rbb.resval.rstring,lvI,rb))	++rindex; break;
				case IEXP_BNTAB_IDX: if (view->BlockIns(rindex,index,Mindex,rbb.resval.rstring,lvI,rb))		++rindex; break;
				case IEXP_VWTAB_IDX: if (view->ViewIns(rindex,index,Mindex,rbb.resval.rstring,lvI,rb))		++rindex; break;
				case IEXP_CSTAB_IDX: if (view->UcsIns(rindex,index,Mindex,rbb.resval.rstring,lvI,TRUE,rb))	++rindex; break;
				case IEXP_TSTAB_IDX: if (view->StyleIns(rindex,index,Mindex,rbb.resval.rstring,lvI,rb))		++rindex; break;
				case IEXP_LTTAB_IDX: if (view->LinetIns(&rindex,index,Mindex,rbb.resval.rstring,lvI,TRUE,rb)) ++rindex; break;
				case IEXP_LATAB_IDX: if (view->LayerIns(rindex,index,Mindex,rbb.resval.rstring,lvI,rb))		++rindex; break;
			}
			sds_relrb(rb);
			rb=SDS_tblgetter(IEXP_EntityCol[Mindex].cType,NULL,FALSE,IEXP_ODNode->m_dbDrawing);
			++index;
		};
		// free if set by getvar.
		if (rbb.resval.rstring)
		{
			IC_FREE(rbb.resval.rstring);			
			rbb.resval.rstring=NULL;
		}
	}
	else
	{
		if (!view->ItemListIns(lvI)) return;
	}
	// Disable the windows.
	EnableWindow(FALSE);
	ShowWindow(SW_HIDE);
}

BOOL IEXP_CListCtrl::DeleteAllItems()
{
	m_bInit = false;
	return CListCtrlEx::DeleteAllItems();
}


/*-------------------------------------------------------------------------*//**
*//*--------------------------------------------------------------------------*/
BOOL	IEXP_CdetailView::ItemListIns
(
LV_ITEM	lvI
)
{
	short fi1,
		  fi3;

	for(fi1=0;fi1<7;++fi1)
	{
		lvI.iImage	   =IEXP_EntityCol[fi1].IconOff;
		lvI.iItem	   =fi1;
		lvI.iSubItem   =0;
		lvI.lParam	   =0;
		lvI.pszText    = (char *) IEXP_TABINFO[fi1].Name;
		lvI.cchTextMax =IEXP_MAX_ITEMLEN;
		if (!m_dataptr) m_dataptr=lvI.lParam;
		if (m_ListCtl[7].InsertItem(&lvI)==-1) return(FALSE);
		switch(fi1)
		{
			case 0: fi3=IEXP_LayerCnt;	break;
			case 1: fi3=IEXP_LinetCnt;	break;
			case 2: fi3=IEXP_TStyleCnt; break;
			case 3: fi3=IEXP_UCSCnt;	break;
			case 4: fi3=IEXP_ViewCnt;	break;
			case 5: fi3=IEXP_BlockCnt;	break;
			case 6: fi3=IEXP_DimStyleCnt;break;
		}
		sprintf(IEXP_szText,"%d"/*DNT*/,fi3);
		m_ListCtl[7].SetItemText(fi1,IEXP_EntityCol[7].Order[1][IEXP_COL_ORDER],IEXP_szText);
	}
	return(TRUE);
}

/*-------------------------------------------------------------------------*//**
This function is called to insert a new View in the (CListCtrl).
(RowIns) is called from here to insert the View.

@author	?
@param	rindex	 =>
@param	index	 =>
@param	mindex	 =>
@param	string	 =>
@param	lvI		 =>
@param	*rb		 =>
@return TRUE  = successful, FALSE = failed.
*//*--------------------------------------------------------------------------*/
BOOL			IEXP_CdetailView::ViewIns
(
short			rindex,
short			index,
short			mindex,
char			*string,
LV_ITEM			lvI,
struct resbuf	*rb
)
{
	long		   lParam;
	struct resbuf *rbt;


	for(IEXP_ItemDefPtr=NULL,rbt=rb;rbt;rbt=rbt->rbnext) {
		switch(rbt->restype) {
			case 2:
				IEXP_ItemDefPtr = new IEXP_ItemDef(IEXP_EntityCol[mindex].cType, rbt->resval.rstring);
				lParam=(long)IEXP_ItemDefPtr;
				break;
			case 70:
				if (rbt->resval.rint&IEXP_DEL_FLG) {
					if (IEXP_ItemDefPtr) {
						delete IEXP_ItemDefPtr;
						IEXP_ItemDefPtr = NULL;
					}
					return(FALSE);
				}
				break;
		}
	}

	if (!IEXP_ItemDefPtr) return(FALSE);
	if (!RowIns(string,rindex,lParam,mindex,lvI,IEXP_ItemDefPtr->name())) return(FALSE);
	for(rbt=rb;rbt;rbt=rbt->rbnext) {
		switch(rbt->restype) {
			case 40:
				sprintf(IEXP_szText,"%G"/*DNT*/,rbt->resval.rreal);
				m_ListCtl[mindex].SetItemText(rindex,IEXP_EntityCol[mindex].Order[1][IEXP_COL_ORDER],IEXP_szText);
				break;
			case 41:
				sprintf(IEXP_szText,"%G"/*DNT*/,rbt->resval.rreal);
				m_ListCtl[mindex].SetItemText(rindex,IEXP_EntityCol[mindex].Order[2][IEXP_COL_ORDER],IEXP_szText);
				break;
			case 11:
				sprintf(IEXP_szText,"%G,%G,%G"/*DNT*/,rbt->resval.rpoint[0],rbt->resval.rpoint[1],rbt->resval.rpoint[2]);
				m_ListCtl[mindex].SetItemText(rindex,IEXP_EntityCol[mindex].Order[3][IEXP_COL_ORDER],IEXP_szText);
				break;
		}
	}
	++IEXP_ViewCnt;
	return(TRUE);
}

///////////////////////////////////////////////////////////////////////
// This function is called to insert a new UCS in the (CListCtrl).
// (RowIns) is called from here to insert the UCS.
//
// TRUE  = successful
// FALSE = failed.
//
BOOL IEXP_CdetailView::UcsIns(short rindex,short index,short mindex,char *string,LV_ITEM lvI,BOOL addbl,struct resbuf *rb) {
	long		   lParam;
	CString 	   tstr;
	struct resbuf *rbt;

	if (addbl!=2) {
		for(IEXP_ItemDefPtr=NULL,rbt=rb;rbt;rbt=rbt->rbnext) {
			switch(rbt->restype) {
				case 2:
					IEXP_ItemDefPtr = new IEXP_ItemDef(IEXP_EntityCol[mindex].cType, rbt->resval.rstring);
					lParam=(long)IEXP_ItemDefPtr;
					break;
				case 70:
					if (rbt->resval.rint&IEXP_DEL_FLG) {
						if (IEXP_ItemDefPtr) {
							delete IEXP_ItemDefPtr;
							IEXP_ItemDefPtr = NULL;
						}
						return(FALSE);
					}
					break;
			}
		}
		if (!RowIns(string,rindex,lParam,mindex,lvI,IEXP_ItemDefPtr->name())) return(FALSE);
		for(IEXP_ItemDefPtr=NULL,rbt=rb;rbt;rbt=rbt->rbnext) {
			switch(rbt->restype) {
				case 10:
					if(fabs(rbt->resval.rpoint[0])<CMD_FUZZ)rbt->resval.rpoint[0]=0.0;	//if very very near 0.0, print 0.0
					if(fabs(rbt->resval.rpoint[1])<CMD_FUZZ)rbt->resval.rpoint[1]=0.0;
					if(fabs(rbt->resval.rpoint[2])<CMD_FUZZ)rbt->resval.rpoint[2]=0.0;
					sds_rtos(rbt->resval.rpoint[0],-1,-1,IEXP_szText); tstr=IEXP_szText;  tstr+=IEXP_SCOMMA;
					sds_rtos(rbt->resval.rpoint[1],-1,-1,IEXP_szText); tstr+=IEXP_szText; tstr+=IEXP_SCOMMA;
					sds_rtos(rbt->resval.rpoint[2],-1,-1,IEXP_szText); tstr+=IEXP_szText;
					m_ListCtl[mindex].SetItemText(rindex,IEXP_EntityCol[mindex].Order[1][IEXP_COL_ORDER],tstr);
					break;
				case 11:
					if(fabs(rbt->resval.rpoint[0])<CMD_FUZZ)rbt->resval.rpoint[0]=0.0;	//if very very near 0.0, print 0.0
					if(fabs(rbt->resval.rpoint[1])<CMD_FUZZ)rbt->resval.rpoint[1]=0.0;
					if(fabs(rbt->resval.rpoint[2])<CMD_FUZZ)rbt->resval.rpoint[2]=0.0;
					sds_rtos(rbt->resval.rpoint[0],2,-1,IEXP_szText); tstr=IEXP_szText;  tstr+=IEXP_SCOMMA;
					sds_rtos(rbt->resval.rpoint[1],2,-1,IEXP_szText); tstr+=IEXP_szText; tstr+=IEXP_SCOMMA;
					sds_rtos(rbt->resval.rpoint[2],2,-1,IEXP_szText); tstr+=IEXP_szText;
					m_ListCtl[mindex].SetItemText(rindex,IEXP_EntityCol[mindex].Order[2][IEXP_COL_ORDER],tstr);
					break;
				case 12:
					if(fabs(rbt->resval.rpoint[0])<CMD_FUZZ)rbt->resval.rpoint[0]=0.0;	//if very very near 0.0, print 0.0
					if(fabs(rbt->resval.rpoint[1])<CMD_FUZZ)rbt->resval.rpoint[1]=0.0;
					if(fabs(rbt->resval.rpoint[2])<CMD_FUZZ)rbt->resval.rpoint[2]=0.0;
					sds_rtos(rbt->resval.rpoint[0],2,-1,IEXP_szText); tstr=IEXP_szText;  tstr+=IEXP_SCOMMA;
					sds_rtos(rbt->resval.rpoint[1],2,-1,IEXP_szText); tstr+=IEXP_szText; tstr+=IEXP_SCOMMA;
					sds_rtos(rbt->resval.rpoint[2],2,-1,IEXP_szText); tstr+=IEXP_szText;
					m_ListCtl[mindex].SetItemText(rindex,IEXP_EntityCol[mindex].Order[3][IEXP_COL_ORDER],tstr);
					break;
			}
		}
	}
	++IEXP_UCSCnt;
	return(TRUE);
}


///////////////////////////////////////////////////////////////////////
// This function is called to insert a new Text style in the (CListCtrl).
// (RowIns) is called from here to insert the Text style.
//
// TRUE  = successful
// FALSE = failed
//
BOOL IEXP_CdetailView::StyleIns(short rindex,short index,short mindex,char *string,LV_ITEM lvI,struct resbuf *rb) {
	long lParam;

	// get the table record, so we can get the values directly with accessors
	resbuf *tmprb = ic_ret_assoc(rb, -1);
	ASSERT(tmprb);
	if (NULL == tmprb){
		return(FALSE);
	}
	db_styletabrec *tabrec = (db_styletabrec *) tmprb->resval.rlname[0];

	// clean up and leave if the record is being deleted
	int flags;
	tabrec->get_70(&flags);

	if (flags & IEXP_DEL_FLG){
		return(FALSE);
	}
	// EBATECH(CNBR) -[ 2002/4/16 Remove shapefont from style list.
	if (flags & IC_SHAPEFILE_SHAPES ){
		return(FALSE);
	}
	// EBATECH(CNBR) ]-

	// get the style's name and create a row for it in the list of styles
	char *styleName;
	tabrec->get_2(&styleName);

	if (((CString) styleName).Find("|") != -1){
		return(FALSE);
	}
	IEXP_ItemDefPtr = new IEXP_ItemDef(IEXP_EntityCol[mindex].cType, styleName);
	if(IEXP_ItemDefPtr == NULL){
		return(FALSE);
	}

	lParam = (long) IEXP_ItemDefPtr;

	if (!RowIns(string, rindex, lParam, mindex, lvI, IEXP_ItemDefPtr->name())){
		delete IEXP_ItemDefPtr;
		return(FALSE);
	}

	// now get the values displayed in the columns in the list
	sds_real value;
	tabrec->get_40(&value);								// text height
	sprintf(IEXP_szText, "%G"/*DNT*/, value);
	m_ListCtl[mindex].SetItemText(rindex,
								  IEXP_EntityCol[mindex].Order[IEXP_TS_HEIGHT][IEXP_COL_ORDER],
								  IEXP_szText);

	tabrec->get_41(&value);								// width factor
	sprintf(IEXP_szText,"%G"/*DNT*/, value);
	m_ListCtl[mindex].SetItemText(rindex,
								  IEXP_EntityCol[mindex].Order[IEXP_TS_WIDTH][IEXP_COL_ORDER],
								  IEXP_szText);

	tabrec->get_50(&value);								// oblique angle
	sprintf(IEXP_szText,"%G"/*DNT*/,((180/IC_PI) * value));
	m_ListCtl[mindex].SetItemText(rindex,
								  IEXP_EntityCol[mindex].Order[IEXP_TS_SLANT][IEXP_COL_ORDER],
								  IEXP_szText);

	if (tabrec->hasTrueTypeFont()){						// ttf - display font family name,
														//		font style and language
		char strValue[IC_ACADBUF];
		if (tabrec->getFontFamily(strValue)){
			m_ListCtl[mindex].SetItemText(rindex,
										  IEXP_EntityCol[mindex].Order[IEXP_TS_FONT][IEXP_COL_ORDER],
										  strValue);
		}
		// EBATECH(CNBR) -[ remove font style name, it is never used.
		// to do - need to get the font style name properly - get the font style number and then
		//		   determine the style name from that and the enumeration of the styles
		long stylenumber;
		if (tabrec->getFontStyleNumber(&stylenumber)){
			CString cstext;
			//char fontStyle[IC_ACADBUF + 1];
			//strncpy (fontStyle, m_ListCtl[mindex].GetItemText(index, mindex), IC_ACADBUF);

			//CInPlaceFontStyleCombo* style = m_ListCtl[mindex].ShowInPlaceFontStyleCombo(
			//	index, mindex, fontStyle, stylenumber);
			CInPlaceFontStyleCombo* style = m_ListCtl[mindex].ShowInPlaceFontStyleCombo( index, mindex, stylenumber);
			// EBATECH(CNBR) ]-
			if( style ){
				int i = style->GetCount();

				// all font styles should be put in the correct order: regular, italics, bold, then
				// bold italics in ShowInPlaceFontStyleCombo, so I refer to the preferred
				// selection by number here.
				if (i == 1)	{
					style->SetCurSel(0);
					style->GetWindowText(cstext);
				}else if ((0x3000000 & stylenumber) == 0x3000000){	// Bold Italics
					style->SetCurSel(i-1);	// we want the last selection
					style->GetWindowText(cstext);
				}else if (0x2000000 & stylenumber){					// Bold
					if (i > 2){
						style->SetCurSel(2);	// with three or more selections, bold will be third
						style->GetWindowText(cstext);
					}else{
						style->SetCurSel(0);	// with two selections, it varies.
						style->GetWindowText(cstext);
						if (cstext.Find("Bold", -1) == -1 && cstext.Find("Black", -1) == -1){
							style->SetCurSel(1);
							style->GetWindowText(cstext);
						}
					}
				}else if (0x1000000 & stylenumber){					// Italics
					style->SetCurSel(1);		// italics should always be second
					style->GetWindowText(cstext);
				}else{												// Regular
					style->SetCurSel(0);		// regular comes first
					style->GetWindowText(cstext);
				}
				style->DeleteStyleItems();
				delete style;
			}
			m_ListCtl[mindex].SetItemText(rindex,
				IEXP_EntityCol[mindex].Order[IEXP_TS_STYLE][IEXP_COL_ORDER],
				cstext);

			// EBATECH(CNBR) -[ Set Language and Bigfont
			{
				int charSet = ((stylenumber & 0x0000ff00)>>8);
				int i, n;
				cstext = CString("");
				CInPlaceFontLangCombo* lang = m_ListCtl[mindex].ShowInPlaceFontLangCombo(index, mindex, stylenumber);
				if(lang){
					n = lang->GetCount();
					for( i = 0 ; i < n ; i++ ){
						if( lang->GetItemData(i) == charSet ){
							lang->SetCurSel(i);
							lang->GetWindowText(cstext);
							break;
						}
					}
					delete lang;
				}
				m_ListCtl[mindex].SetItemText(rindex,
					IEXP_EntityCol[mindex].Order[IEXP_TS_LANG][IEXP_COL_ORDER],
					cstext/**/);
			}
			m_ListCtl[mindex].SetItemText(rindex,
										  IEXP_EntityCol[mindex].Order[IEXP_TS_BIGFONT][IEXP_COL_ORDER],
										  ""/**/);
			// EBATECH(CNBR) ]-
		}else{
			m_ListCtl[mindex].SetItemText(rindex,
										  IEXP_EntityCol[mindex].Order[IEXP_TS_STYLE][IEXP_COL_ORDER],
										  "Regular");
			m_ListCtl[mindex].SetItemText(rindex,
										  IEXP_EntityCol[mindex].Order[IEXP_TS_LANG][IEXP_COL_ORDER],
										  ""/**/);
			m_ListCtl[mindex].SetItemText(rindex,
										  IEXP_EntityCol[mindex].Order[IEXP_TS_BIGFONT][IEXP_COL_ORDER],
										  ""/**/);
		}
	}else{	// shx - display font filename
		char *name;
		char buf[IC_ACADBUF];
		tabrec->get_3(buf, IC_ACADBUF - 1);							// primary font filename
		ic_setext(buf, ".shx");
		name = (strrchr(buf, '\\')) ? strrchr(buf, '\\') + 1 : buf;
		m_ListCtl[mindex].SetItemText(rindex,
			IEXP_EntityCol[mindex].Order[IEXP_TS_FONT][IEXP_COL_ORDER],
			name);
		// EBATECH(CNBR) -[ Set Style, Language and Bigfont
		m_ListCtl[mindex].SetItemText(rindex,
			IEXP_EntityCol[mindex].Order[IEXP_TS_STYLE][IEXP_COL_ORDER],
			""/**/);
		m_ListCtl[mindex].SetItemText(rindex,
			IEXP_EntityCol[mindex].Order[IEXP_TS_LANG][IEXP_COL_ORDER],
			""/**/);

		tabrec->get_4(buf, IC_ACADBUF - 1);	// bigfont filename
		if(strlen(buf)>0){
			ic_setext(buf, ".shx");
		}
		name = (strrchr(buf, '\\')) ? strrchr(buf, '\\') + 1 : buf;
		m_ListCtl[mindex].SetItemText(rindex,
			IEXP_EntityCol[mindex].Order[IEXP_TS_BIGFONT][IEXP_COL_ORDER],
			name);
		// EBATECH(CNBR)
	}

	tabrec->get_71(&flags);								// generation - backwards/upside down
	sprintf(IEXP_szText, "%s"/*DNT*/, ((flags & 2) ?	// backwards
		ResourceString(IDC_ICADEXPLORER_YES_13, "Yes") :
		ResourceString(IDC_ICADEXPLORER_NO_14, "No")));
	m_ListCtl[mindex].SetItemText(rindex,
								  IEXP_EntityCol[mindex].Order[IEXP_TS_BACKWORD][IEXP_COL_ORDER],
								  IEXP_szText);

	sprintf(IEXP_szText, "%s"/*DNT*/, ((flags & 4) ?	// upside down
		ResourceString(IDC_ICADEXPLORER_YES_13, "Yes") :
		ResourceString(IDC_ICADEXPLORER_NO_14, "No")));
	m_ListCtl[mindex].SetItemText(rindex,
								  IEXP_EntityCol[mindex].Order[IEXP_TS_UPSIDEDOWN][IEXP_COL_ORDER],
								  IEXP_szText);

	tabrec->get_70(&flags);								// vertical
	sprintf(IEXP_szText,"%s"/*DNT*/,((flags & 4) ?
		ResourceString(IDC_ICADEXPLORER_YES_13, "Yes") :
		ResourceString(IDC_ICADEXPLORER_NO_14, "No")));
	m_ListCtl[mindex].SetItemText(rindex,
								  IEXP_EntityCol[mindex].Order[IEXP_TS_VERTICAL][IEXP_COL_ORDER],
								  IEXP_szText);

	++IEXP_TStyleCnt;
	return(TRUE);
}

///////////////////////////////////////////////////////////////////////
// This function is called to insert a new Linetype in the (CListCtrl).
// (RowIns) is called from here to insert the Linetype.
//
// TRUE  = successful
// FALSE = failed.
//
BOOL IEXP_CdetailView::LinetIns(short *rindex,short index,short mindex,char *string,LV_ITEM lvI,BOOL addbl,struct resbuf *rb) {
	long		   lParam;
	struct resbuf *rbt;

	for(IEXP_ItemDefPtr=NULL,rbt=rb;rbt;rbt=rbt->rbnext) {
		switch(rbt->restype) {
			case 2:
				IEXP_ItemDefPtr = new IEXP_ItemDef(IEXP_EntityCol[mindex].cType, rbt->resval.rstring);
				lParam=(long)IEXP_ItemDefPtr;
				break;
			case 70:
				if (rbt->resval.rint&IEXP_DEL_FLG) {
					delete IEXP_ItemDefPtr;
					IEXP_ItemDefPtr = NULL;
					return FALSE;
				}
				break;
		}
	}
	if (ic_assoc(rb,3)) return(FALSE);
	// Im adding two to (index) for the (BYBLOCK and BYLAYER) types.
	if (!RowIns(string,((!addbl)?(*rindex):(*rindex)),lParam,mindex,lvI,IEXP_ItemDefPtr->name())) return(FALSE);
	m_ListCtl[mindex].SetItemText(((!addbl)?(*rindex):(*rindex)),IEXP_EntityCol[mindex].Order[1][IEXP_COL_ORDER],ic_rbassoc->resval.rstring);

	++IEXP_LinetCnt;
	return(TRUE);
}

///////////////////////////////////////////////////////////////////////
// This function is called to insert a new layer in the (CListCtrl).
// (RowIns) is called from here to insert the layer.
//
// TRUE  = successful
// FALSE = failed.
//
// EBATECH(CNBR) Bugzilla#78471 2003/3/31 Add 2 columns Lineweight(370) and Plottable(290)
//
BOOL IEXP_CdetailView::LayerIns(short rindex,short index,short mindex,char *string,LV_ITEM lvI,struct resbuf *rb) {
	long		   lParam;
	struct resbuf *rbt;
	db_viewport *pVP=NULL;

	for(IEXP_ItemDefPtr=NULL,rbt=rb;rbt;rbt=rbt->rbnext) {
		switch(rbt->restype) {
			case 2:
				IEXP_ItemDefPtr = new IEXP_ItemDef(IEXP_EntityCol[mindex].cType, rbt->resval.rstring);
				lParam=(long)IEXP_ItemDefPtr;
				break;
			case 70:
				if (rbt->resval.rint&IEXP_DEL_FLG && string && *string) {
					delete IEXP_ItemDefPtr;
					IEXP_ItemDefPtr = NULL;
					return FALSE;
				}
				break;
		}
	}
   	if (!RowIns(string,rindex,lParam,mindex,lvI,IEXP_ItemDefPtr->name())) return(FALSE);
	if (ic_assoc(rb,70)) return(FALSE);
	sprintf(IEXP_szText,"%s"/*DNT*/,(ic_rbassoc->resval.rint & IC_LAYER_LOCKED)?ResourceString(IDC_IEXP_YES, "Yes"):ResourceString(IDC_IEXP_NO, "No"));
	m_ListCtl[mindex].SetItemText(rindex,IEXP_EntityCol[mindex].Order[IEXP_LA_LOCKED][IEXP_COL_ORDER],IEXP_szText);
	sprintf(IEXP_szText,"%s"/*DNT*/,(ic_rbassoc->resval.rint & IC_LAYER_FROZEN)?ResourceString(IDC_IEXP_FROZEN_STR, "Frozen"):ResourceString(IDC_IEXP_THAWED_STR, "Thawed"));
	m_ListCtl[mindex].SetItemText(rindex,IEXP_EntityCol[mindex].Order[IEXP_LA_FROZEN][IEXP_COL_ORDER],IEXP_szText);
	pVP=IEXP_ODNode->m_dbDrawing->GetCurrentViewport();
	if (pVP)
	{
		ic_assoc(rb,-1);
		sprintf(IEXP_szText,"%s",(pVP->IsLayerVpLayerFrozen((db_layertabrec *)rb->resval.rlname[0]))?ResourceString(IDC_IEXP_FROZEN_STR, "Frozen"):ResourceString(IDC_IEXP_THAWED_STR, "Thawed"));
		m_ListCtl[mindex].SetItemText(rindex,IEXP_EntityCol[mindex].Order[IEXP_LA_VP_FREEZECUR][IEXP_COL_ORDER],IEXP_szText);
	}
	else
		m_ListCtl[mindex].SetItemText(rindex,IEXP_EntityCol[mindex].Order[IEXP_LA_VP_FREEZECUR][IEXP_COL_ORDER],ResourceString(IDC_ICADEXPLORER_5, "No Current Viewport"));

	sprintf(IEXP_szText,"%s",(ic_rbassoc->resval.rint & IC_LAYER_VPFROZEN)?ResourceString(IDC_IEXP_FROZEN_STR, "Frozen"):ResourceString(IDC_IEXP_THAWED_STR, "Thawed"));
	m_ListCtl[mindex].SetItemText(rindex,IEXP_EntityCol[mindex].Order[IEXP_LA_VP_FREEZENEW][IEXP_COL_ORDER],IEXP_szText);

	for(rbt=rb;rbt;rbt=rbt->rbnext) {
		switch(rbt->restype) {
			case 62:
				ic_colorstr((int)fabs(rbt->resval.rint),IEXP_szText);
				m_ListCtl[mindex].SetItemText(rindex,IEXP_EntityCol[mindex].Order[IEXP_LA_COLOR][IEXP_COL_ORDER],IEXP_szText);
				sprintf(IEXP_szText,"%s"/*DNT*/,(rbt->resval.rint<0)?ResourceString(IDC_ICADEXPLORER_OFF_16, "Off" ):ResourceString(IDC_ICADEXPLORER_ON_17, "On" ));
				m_ListCtl[mindex].SetItemText(rindex,IEXP_EntityCol[mindex].Order[IEXP_LA_ON_OFF][IEXP_COL_ORDER],IEXP_szText);
				break;
			case 6:
				m_ListCtl[mindex].SetItemText(rindex,IEXP_EntityCol[mindex].Order[IEXP_LA_LINETYPE][IEXP_COL_ORDER],rbt->resval.rstring);
				break;
			case 290:
				sprintf(IEXP_szText,"%s"/*DNT*/,(rbt->resval.rint!=0)?ResourceString(IDC_IEXP_YES, "Yes"):ResourceString(IDC_IEXP_NO, "No"));
				m_ListCtl[mindex].SetItemText(rindex,IEXP_EntityCol[mindex].Order[IEXP_LA_PLOT][IEXP_COL_ORDER],IEXP_szText);
				break;
			case 370:
				{
				int top = ( IEXP_lwunits == 1 ? IDC_COMBO_M_LWDEFAULT : IDC_COMBO_I_LWDEFAULT );
				sprintf(IEXP_szText,"Lweight %d", rbt->resval.rint );
				m_ListCtl[mindex].SetItemText(rindex,IEXP_EntityCol[mindex].Order[IEXP_LA_LINEWEIGHT][IEXP_COL_ORDER], ResourceString(top+db_lweight2index(rbt->resval.rint)+3, IEXP_szText ));
				}
				break;
		}
	}
	if (strisame(ResourceString(IDC_ICADEXPLORER_0_18, "0" ),IEXP_ItemDefPtr->name())) m_Layer_zero=rindex;
	++IEXP_LayerCnt;
	return(TRUE);
}

/*-------------------------------------------------------------------------*//**
This function is called to insert a new Block in the (CListCtrl).
(RowIns) is called from here to insert the Block.

@param	rindex	 =>
@param	index	 =>
@param	mindex	 =>
@param	string	 =>
@param	lvI		 =>
@param	rb		 =>
@return TRUE  = successful, FALSE = failed.
*//*--------------------------------------------------------------------------*/
BOOL			IEXP_CdetailView::BlockIns
(
short			rindex,
short			index,
short			mindex,
char			*string,
LV_ITEM			lvI,
struct resbuf	*rb
)
{
	long		   lParam;
	struct resbuf *rbt;
	short		   fi1;
	int 		   flag;
	CString 	   tstr = "0,0,0";
	CBitmap		  *cBitmap;

	lvI.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;

	for(IEXP_ItemDefPtr=NULL,rbt=rb;rbt;rbt=rbt->rbnext)
	{
		switch(rbt->restype)
		{
			case 2:
				if (*rbt->resval.rstring=='*') return(FALSE);
				IEXP_ItemDefPtr = new IEXP_ItemDef(IEXP_EntityCol[mindex].cType, rbt->resval.rstring);
				lParam=(long)IEXP_ItemDefPtr;
				break;
			case 10:
				sds_rtos(rbt->resval.rpoint[0],-1,-1,IEXP_szText); tstr=IEXP_szText;  tstr+=IEXP_SCOMMA;
				sds_rtos(rbt->resval.rpoint[1],-1,-1,IEXP_szText); tstr+=IEXP_szText; tstr+=IEXP_SCOMMA;
				sds_rtos(rbt->resval.rpoint[2],-1,-1,IEXP_szText); tstr+=IEXP_szText;
				break;
			case 70:
				flag=rbt->resval.rint;
				if (rbt->resval.rint&IEXP_DEL_FLG || rbt->resval.rint&0x10)
				{
					delete IEXP_ItemDefPtr;
					IEXP_ItemDefPtr = NULL;
					return FALSE;
				}
				break;
		}
	}

	lvI.iImage=rindex;
	if (!index) {
		lvI.mask|=LVIF_STATE;
		lvI.state=LVIS_SELECTED;
	}
	lvI.iItem	   =index;
	lvI.iSubItem   =0;
	lvI.lParam	   =lParam;
	lvI.pszText    =const_cast<char*>(IEXP_ItemDefPtr->name());
	lvI.cchTextMax =IEXP_MAX_ITEMLEN;
	if (!m_dataptr) m_dataptr=lvI.lParam;
	if (m_ListCtl[mindex].InsertItem(&lvI)==-1) return(FALSE);
	++IEXP_BlockCnt;

	//Get the Number of Block Inserts
	fi1 = GetBlockCount (IEXP_ItemDefPtr->name(), IEXP_ODNode->m_dbDrawing, TRUE);

	sprintf(IEXP_szText,"%d",fi1);
	m_ListCtl[mindex].SetItemText(rindex,IEXP_EntityCol[mindex].Order[1][IEXP_COL_ORDER],IEXP_szText);

	//Set the Block Insertion Point
	m_ListCtl[mindex].SetItemText(rindex,IEXP_EntityCol[mindex].Order[2][IEXP_COL_ORDER],tstr);

	//Set the XRef name if this block is an XRef
	if (flag&4)
	{
		ic_assoc(rb,1);
		sprintf(IEXP_szText,"%s"/*DNT*/,ic_rbassoc->resval.rstring);
		m_ListCtl[mindex].SetItemText(rindex,IEXP_EntityCol[mindex].Order[3][IEXP_COL_ORDER],IEXP_szText);

		//create bitmap of xref
		cBitmap=SDS_DrawBitmap(NULL,IEXP_szText,NULL,NULL,IEXP_ICON_BITMAP_WIDTH,IEXP_ICON_BITMAP_HEIGHT,TRUE);
	}
	else
		//create bitmap of block
		cBitmap=SDS_DrawBitmap(IEXP_ODNode->m_dbDrawing,NULL,IEXP_ItemDefPtr->name(),NULL,IEXP_ICON_BITMAP_WIDTH,IEXP_ICON_BITMAP_HEIGHT,TRUE);

	//Add the new bitmap image of the block
	m_pimagelistI.Add(cBitmap,(COLORREF)0xFFFFFF);
	m_ListCtl[IEXP_BNTAB_IDX].SetImageList(&m_pimagelistI,LVSIL_NORMAL);
	delete cBitmap;

	return(TRUE);
}


///////////////////////////////////////////////////////////////////////
// This function is called to insert a new DimStyle in the (CListCtrl).
// (RowIns) is called from here to insert the DimStyle.
//
// TRUE  = successful
// FALSE = failed.
//
BOOL IEXP_CdetailView::DimStyleIns(short rindex,short index,short mindex,char *string,LV_ITEM lvI,struct resbuf *rb) {
	long		   lParam;
	struct resbuf *rbt;

	for(IEXP_ItemDefPtr=NULL,rbt=rb;rbt;rbt=rbt->rbnext)
	{
		switch(rbt->restype)
		{
			case 2:
				IEXP_ItemDefPtr = new IEXP_ItemDef(IEXP_EntityCol[mindex].cType, rbt->resval.rstring);
				lParam=(long)IEXP_ItemDefPtr;
				break;
			case 70:
				if (rbt->resval.rint&IEXP_DEL_FLG)
				{
					delete IEXP_ItemDefPtr;
					IEXP_ItemDefPtr = NULL;
					return FALSE;
				}
				break;
		}
	}
	if (!RowIns(string,rindex,lParam,mindex,lvI,IEXP_ItemDefPtr->name())) return(FALSE);
	++IEXP_DimStyleCnt;
	return(TRUE);
}


///////////////////////////////////////////////////////////////////////
// This function is called to inserts a row in the (CListCtrl) structure
// and to insert the row in the list.
//
// TRUE  = successful
// FALSE = failed.
//
BOOL IEXP_CdetailView::RowIns(char *string,short index,long lparam,short mindex,LV_ITEM lvI,const char *name) {
	UINT mask;

	if (string && strisame(string,name)) {
		lvI.iImage=0;
		strcpy(IEXP_EntityCol[mindex].CurrentName,string);
	} else lvI.iImage=-1;
	if (!index) {
		mask=lvI.mask;
		lvI.mask|=LVIF_STATE;
		lvI.state=LVIS_SELECTED;
	}
	lvI.iItem	   =index;
	lvI.iSubItem   =0;
	lvI.lParam	   =lparam;
	lvI.pszText    =const_cast<char*>(name);
	lvI.cchTextMax =IEXP_MAX_ITEMLEN;
	if (!m_dataptr) m_dataptr=lvI.lParam;
	if (m_ListCtl[mindex].InsertItem(&lvI)==-1) return(FALSE);
	if (!index) lvI.mask=mask;
	return(TRUE);
}

///////////////////////////////////////////////////////////////////////
// This function sets the currently selected item to a current state.
// the only types that can be set are.
// (VIEWS),(UCS),(TEXT STYLES),(LINE TYPES),and (LAYERS).
//
// Nothing is returned if it failed.
//
void IEXP_CdetailView::CurrentItem() {
	struct resbuf	 rb,
					*rbb,
					*rbt;
	CString 		 String;
	int 			 ViewMode,
					 Index,
					 selcnt;
	sds_real		 ViewHight,
					 Vtwist;
	sds_point		 ViewCntr,
					 ViewDir,
					 viewdirold;
	struct gr_view	*view=SDS_CURGRVW;

	HTREEITEM			  hTItem;
	struct IEXP_OpenDocs *ODNode;

	if (m_Explorer->m_editmode)
		m_ListCtl[IEXP_CurrentIndx].EndLabelEditing();

	if (SDS_CURDWG!=IEXP_ODNode->m_dbDrawing) m_Explorer->ActivateCurrentView(IEXP_ODNode->m_dbDrawing);

	if (SDS_CURDWG!=IEXP_ODNode->m_dbDrawing) m_Explorer->ActivateCurrentView(IEXP_ODNode->m_dbDrawing);

	if (!(selcnt=m_ListCtl[IEXP_CurrentIndx].GetSelectedCount())) return;
	if (selcnt==-1) return;
	if (!(selcnt=m_ListCtl[IEXP_CurrentIndx].GetItemCount())) return;
	for(m_ItemIndex=selcnt;m_ItemIndex!=-1;--m_ItemIndex) {
		if (m_ListCtl[IEXP_CurrentIndx].GetItemState(m_ItemIndex,LVIS_SELECTED)) break;
	}
	if (IEXP_EntityCol[IEXP_CurrentIndx].IType!=IEXP_BNTAB_IDX)
	{
		if ((IEXP_CurrentIndx==5) || (IEXP_CurrentIndx==7)) return;
		// Check if the type is a layer, If so and it is frozen then return.
		if (IEXP_EntityCol[IEXP_CurrentIndx].IType==IEXP_LATAB_IDX)
		{
			IEXP_ItemDefPtr=(IEXP_ItemDef *)m_ListCtl[IEXP_CurrentIndx].GetItemData(m_ItemIndex);
			rbb=SDS_tblgetter(IEXP_EntityCol[IEXP_CurrentIndx].cType,IEXP_ItemDefPtr->name(),TRUE,IEXP_ODNode->m_dbDrawing);
			if (ic_assoc(rbb,70)) return;
			if ((ic_rbassoc->resval.rint & IC_LAYER_FROZEN))
			{
				sds_relrb(rbb);
				AfxMessageBox(ResourceString(IDC_ICADEXPLORER_CANNOT_SET_19, "Cannot set a frozen layer current." ));
				return;
			}
			if (strchr(IEXP_ItemDefPtr->name(),IEXP_VER_BAR))
			{
				sds_relrb(rbb);
				AfxMessageBox(ResourceString(IDC_ICADEXPLORER_CANNOT_SET_20, "Cannot set a XREF layer current." ));
				return;
			}
			if (ic_assoc(rbb,62)) return;
			if (ic_rbassoc->resval.rint < 0)
				AfxMessageBox(ResourceString(IDC_ICADEXPLORER_3, "Warning! The layer you are making current is turned off."));
			sds_relrb(rbb);
		}
		else
			IEXP_ItemDefPtr=(IEXP_ItemDef *)m_ListCtl[IEXP_CurrentIndx].GetItemData(m_ItemIndex);

		switch(IEXP_EntityCol[IEXP_CurrentIndx].IType) {
		   case IEXP_TSTAB_IDX:
				if (strchr(IEXP_ItemDefPtr->name(),IEXP_VER_BAR)) {
					AfxMessageBox(ResourceString(IDC_ICADEXPLORER_CANNOT_SET_21, "Cannot set a XREF text style current." ));
					return;
				}
		   case IEXP_LTTAB_IDX:
				if (strchr(IEXP_ItemDefPtr->name(),IEXP_VER_BAR)) {
					AfxMessageBox(ResourceString(IDC_ICADEXPLORER_CANNOT_SET_22, "Cannot set a XREF Linetype current." ));
					return;
				}
		   // Bugzilla No. 78088; 29/04/2002 [
		   case IEXP_DSTAB_IDX:
				if (strchr(IEXP_ItemDefPtr->name(),IEXP_VER_BAR)) {
					AfxMessageBox(ResourceString(IDC_ICADEXPLORER_CANNOT_SET_23, "Cannot set a XREF Dimension style current." ));
					return;
				}
			// Bugzilla No. 78088; 29/04/2002 ]
		   default:
				//
				//IEXP_Findinfo.psz   =IEXP_EntityCol[IEXP_CurrentIndx].CurrentName;
				//Index=m_ListCtl[IEXP_CurrentIndx].FindItem(&IEXP_Findinfo);
				//cannot depend on CurrentName above, instead find the item that has its image set.
				Index = m_ListCtl[IEXP_CurrentIndx].GetActiveItem ();
				// Set the previous items image to (NULL).
				m_ListCtl[IEXP_CurrentIndx].SetItem(Index,0,LVIF_IMAGE,NULL,-1,0,0,0);
				// Send an update to the previous items image.
				if (!m_ListCtl[IEXP_CurrentIndx].Update(Index)) return;
				String=m_ListCtl[IEXP_CurrentIndx].GetItemText(m_ItemIndex,0);
				strcpy(IEXP_EntityCol[IEXP_CurrentIndx].CurrentName,(char *)((LPCTSTR)String));
				// Set the current items image to the (CHECK) mark.
				if (!m_ListCtl[IEXP_CurrentIndx].SetItem(m_ItemIndex,0,LVIF_IMAGE,NULL,0,0,0,0)) return;
				if (m_DlgState) ::SendMessage(m_EditDlg->m_hWnd,IEXP_DBLCLK,(LPARAM)0,(WPARAM)0);
				break;
		}
	}
	switch(IEXP_EntityCol[IEXP_CurrentIndx].IType) {
	   case IEXP_VWTAB_IDX:
			IEXP_ItemDefPtr=(IEXP_ItemDef *)m_ListCtl[IEXP_CurrentIndx].GetItemData(m_ItemIndex);
			rbb=SDS_tblgetter(IEXP_EntityCol[IEXP_CurrentIndx].cType,IEXP_ItemDefPtr->name(),TRUE,IEXP_ODNode->m_dbDrawing);
			for(rbt=rbb;rbt;rbt=rbt->rbnext) {
				switch(rbt->restype) {
					case 50:
						Vtwist=rbt->resval.rreal;
						break;
					case 11:
						ic_ptcpy(ViewDir,rbt->resval.rpoint);
						break;
					case 71:
						ViewMode=rbt->resval.rint;
						break;
					case 40:
						ViewHight=rbt->resval.rreal;
						break;
					case 10:
						ic_ptcpy(ViewCntr,rbt->resval.rpoint);
						break;
				}
			}

			{	// This block of code was added to make sure we don't restore
				// MS views into PS and vice versa.
				int pspace=0;
				struct resbuf rb;
				SDS_getvar(NULL,DB_QTILEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				if(rb.resval.rint==0){
					SDS_getvar(NULL,DB_QCVPORT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
					if(rb.resval.rint==1) pspace=1;
				}
				if(ic_assoc(rbb,70)!=0) {
					sds_relrb(rbb);
					break;
				}
				if((ic_rbassoc->resval.rint&1) && !pspace){
					cmd_ErrorPrompt(CMD_ERR_NOPAPERVIEW,1);
					sds_relrb(rbb);
					break;
				}
				if(!(ic_rbassoc->resval.rint&1) && pspace){
					cmd_ErrorPrompt(CMD_ERR_NOMODELVIEW,1);
					sds_relrb(rbb);
					break;
				}
			}

			sds_relrb(rbb);
			// get the old viewdir
			rb.restype=RT3DPOINT;
			SDS_getvar(NULL,DB_QVIEWDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			ic_ptcpy(viewdirold, rb.resval.rpoint);

			// set the new viewdir & init the view structure
			rb.restype=RT3DPOINT;
			ic_ptcpy(rb.resval.rpoint,ViewDir);
			SDS_setvar(NULL,DB_QVIEWDIR,&rb,IEXP_ODNode->m_dbDrawing,&SDS_CURCFG,&SDS_CURSES,0);

			gr_initview(IEXP_ODNode->m_dbDrawing,&SDS_CURCFG,&view,NULL);
			// Convert the view center from rp to ucs.
			gr_rp2ucs(ViewCntr,ViewCntr,view);

			// now set viewdir back to what it was so we can undo everything in one step
			ic_ptcpy(rb.resval.rpoint,viewdirold);
			SDS_setvar(NULL,DB_QVIEWDIR,&rb,IEXP_ODNode->m_dbDrawing,&SDS_CURCFG,&SDS_CURSES,0);
			gr_initview(IEXP_ODNode->m_dbDrawing,&SDS_CURCFG,&view,NULL);

			SDS_ZoomIt(IEXP_ODNode->m_dbDrawing,2,ViewCntr,&ViewHight,&ViewMode,ViewDir,&Vtwist,NULL);
			break;
	   case IEXP_CSTAB_IDX:
		   String=m_ListCtl[IEXP_CurrentIndx].GetItemText(m_ItemIndex,0);
		   if (String==IEXP_WORLD) //i.e if String == "World"
			   cmd_ucsworld();
		   else
			   cmd_ucsrestore((char *)((LPCTSTR)String));
		   {
			   struct gr_view *view=SDS_CURGRVW;
			   gr_initview(IEXP_ODNode->m_dbDrawing,&SDS_CURCFG,&view,NULL);
			   sds_redraw(NULL,IC_REDRAW_DONTCARE);
		   }
		   break;
	   default:
		   String=m_ListCtl[IEXP_CurrentIndx].GetItemText(m_ItemIndex,0);
		   rb.restype=RTSTR;
		   rb.resval.rstring=(char *)((LPCTSTR)String);
		   switch(IEXP_EntityCol[IEXP_CurrentIndx].IType)
		   {
				case IEXP_BNTAB_IDX:
					if (m_Explorer->m_MainWindow->m_ExpReturnHwnd) {
						AfxMessageBox(ResourceString(IDC_ICADEXPLORER_CAN_T_DO_T_33, "This operation cannot be performed.  Please run Explorer directly from the Tools menu to perform this operation."));
						return;
					}
					if (IEXP_ODNode->m_dbDrawing!=SDS_CURDWG)
					{
						// Get the selected tree view item handle.
						hTItem=m_CItemList->m_TreeCtl.GetSelectedItem();
						// Get the parent of that handle.
						hTItem=m_CItemList->m_TreeCtl.GetParentItem(hTItem);
						// Search through the open documents link list for the drawing that the block is comming from.
						for(ODNode=IEXP_ODFirstNode;ODNode->hItem!=hTItem;ODNode=ODNode->next);
						// Get the item data from the selected block.
						IEXP_ItemDefPtr=(IEXP_ItemDef *)m_ListCtl[IEXP_CurrentIndx].GetItemData(m_ItemIndex);

						rbb=SDS_tblgetter(IEXP_EntityCol[IEXP_CurrentIndx].cType,IEXP_ItemDefPtr->name(),TRUE,IEXP_ODNode->m_dbDrawing);
						if (ic_assoc(rbb,-1)) return;

						// Call the function to insert the block from another drawing.
						if (!cmd_ItemInsert(ODNode->m_dbDrawing,SDS_CURDWG,(db_handitem *)ic_rbassoc->resval.rlname[0],NULL)) return;
					}
					m_Explorer->m_MainWindow->m_bExplorerState=FALSE;
					m_Explorer->m_MainWindow->EndModalState();
					m_Explorer->ShowWindow(SW_HIDE);
					if(m_Explorer->m_regenMode == 0)
						cmd_regenall();
					strcpy(IEXP_szText,rb.resval.rstring);
					sprintf(IEXP_szTextA,ResourceString(IDC_ICADEXPLORER__INSERT__S_23, "_insert;%s" ),rb.resval.rstring);
					SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)IEXP_szTextA);
					m_Explorer->m_MainWindow->m_bExpInsertState=TRUE;
					m_Explorer->m_CdetailView->m_MessageType=IEXP_EntityCol[IEXP_CurrentIndx].IType;
					m_BlockInsCreate=TRUE;
					break;
				case IEXP_TSTAB_IDX: sds_setvar("TEXTSTYLE"/*DNT*/,&rb); break;
				case IEXP_LTTAB_IDX: sds_setvar("CELTYPE"/*DNT*/,&rb);	 break;
				case IEXP_LATAB_IDX: sds_setvar("CLAYER"/*DNT*/,&rb);	 break;
				case IEXP_DSTAB_IDX: //sds_setvar("DIMSTYLE",&rb); break;
					rbb=SDS_tblgetter(IEXP_EntityCol[IEXP_CurrentIndx].cType,IEXP_ItemDefPtr->name(),TRUE,IEXP_ODNode->m_dbDrawing);
					cmd_setActiveDimStyle (rbb);
					sds_relrb (rbb);
					break;

		   }
	}
}

///////////////////////////////////////////////////////////////////////
// This function is for renaming the currently selected item.
//
// Nothing is returned if it failed.
//
void IEXP_CdetailView::RenameItem()
{
	short selcnt, ret;
	BOOL bFound = FALSE;

	// Get the number of rows for the curently selected (TYPE).
	//Bugzilla No. 78172 ; 07-10-2002 
	if (IEXP_CurrentIndx == 7) return;
	//if ((Index=(m_ListCtl[IEXP_CurrentIndx].GetSelectedCount()-1))==-1) return;
	if (!(selcnt=m_ListCtl[IEXP_CurrentIndx].GetSelectedCount())) return;
	if (selcnt==-1 || selcnt==0) return;
	if (!(selcnt=m_ListCtl[IEXP_CurrentIndx].GetItemCount())) return;

	//if more than one item was selected, we will rename the last selected item.
	for(m_ItemIndex=selcnt-1;m_ItemIndex!=-1;--m_ItemIndex)
	{
		if (ret = m_ListCtl[IEXP_CurrentIndx].GetItemState(m_ItemIndex,LVIS_SELECTED))
		{
			bFound = TRUE;
			break;
		}
	}
	if (!bFound)	//nothing was selected
		return;

	if (!m_ListCtl[IEXP_CurrentIndx].ValidateData (m_ItemIndex, 0))
		return;

	// DP: to allow edit names with IC_ACADNMLEN max len
	CEdit* pEdit = m_ListCtl[IEXP_CurrentIndx].EditLabel(m_ItemIndex);
	ASSERT(pEdit);
	//Bugzilla No. 78172 ; 07-10-2002 
	if(pEdit != NULL)
	pEdit->SetLimitText(IC_ACADNMLEN);
}
//Bugzilla No. 78162 ; 20-05-2002 [
void IEXP_CdetailView::IEXP_UpdateLineTypeName(struct resbuf * rbb)
{
	char szLayerName[IC_ACADBUF];
	char szLineTypeName[IC_ACADBUF];
	char szText[IC_ACADBUF];
	int len  = 0;
	IEXP_ItemDef *Temp_ItemDefPtr = NULL;

	if (ic_assoc(rbb,2)) return;
	strcpy(szLayerName,ic_rbassoc->resval.rstring);
	if (ic_assoc(rbb,6)) return;
	strcpy(szLineTypeName,ic_rbassoc->resval.rstring);

	int nItemCount = m_ListCtl[IEXP_LATAB_IDX].GetItemCount();
	for(int nItemIndex = 0; nItemIndex < nItemCount; nItemIndex++ )
	{
		len = m_ListCtl[IEXP_LATAB_IDX].GetItemText(nItemIndex,0,szText,IC_ACADBUF);
		if ( len == 0 )
			continue;
		if ( strisame(szText,szLayerName) != 1 )
			continue;
		//set the new name
		m_ListCtl[IEXP_LATAB_IDX].SetItemText(nItemIndex,2,szLineTypeName);
	}
}
//Bugzilla No. 78162 ; 20-05-2002 ]
/*-------------------------------------------------------------------------*//**
This function deletes the currently selected item(s).

Any entities that resied on a (LAYER) or have a (Linetype) or
(TEXT STYLE) that is being deleted, this function will call a
message box that allows you one to change the entites (LAYER,
Linetype, TEXT STYLE) to another (LAYER,Linetype,TEXT STYLE). or
just remove all the entities with the deleted attribute.

Nothing is returned if it failed.
*//*--------------------------------------------------------------------------*/
void	IEXP_CdetailView::RemoveItem
(
)
{
	db_handitem *handItem, *InsertItem;
	db_drawing *dbDrawing;
	short SaveIndex, retvalue, rettype=0;
	struct resbuf *rbb, *rbt, rb;
	IEXP_MessItem dMessDlg;
	IEXP_YYTOANC dYMessDlg;
	CString String, Stype;
	sds_name ename;
	int Index, TotalCnt;
	char *currenttextsty=NULL, *currentdimsty=NULL;
	BOOL bDeleted = FALSE;

	// Flag to indicate that an xref has been detached. In this case we need to update all the
	// data in the IntelliCAD Explorer (layers, dimstyles, etc.) at the end of this method.
	bool bXrefDetached = false;

	if (m_Explorer->m_MainWindow->m_ExpReturnHwnd)
	{
		AfxMessageBox(ResourceString(IDC_ICADEXPLORER_CAN_T_DO_T_33, "This operation cannot be performed.  Please run Explorer directly from the Tools menu to perform this operation."));
		return;
	}

	if (m_Explorer->m_editmode)
		m_ListCtl[IEXP_CurrentIndx].EndLabelEditing();

	if (IEXP_CurrentIndx==7)
		return;

	dMessDlg.m_Continue=ResourceString(IDC_ICADEXPLORER_CHANGE____34, "Change..."); dMessDlg.m_typeflag=0;
	dMessDlg.m_BtnTwo.Empty();	/*DG - 15.5.2002*/// Restore "Delete" caption (could be changed in removing linetype procedure).

	// Get a pointer to the entity linklist.
	dbDrawing=IEXP_ODNode->m_dbDrawing;
	SaveIndex=m_ItemIndex;

	// Get the number of rows for the curently selected (TYPE).
	Index=m_ItemIndex=(m_ListCtl[IEXP_CurrentIndx].GetItemCount()-1);

	// Count how many selected items there are.
	for(TotalCnt=0;m_ItemIndex!=-1;--m_ItemIndex)
	{
		if (m_ListCtl[IEXP_CurrentIndx].GetItemState(m_ItemIndex,LVIS_SELECTED)) ++TotalCnt;
	}

	m_ItemIndex=Index;

	if ((IEXP_EntityCol[IEXP_CurrentIndx].IType == IEXP_CSTAB_IDX ||
			IEXP_EntityCol[IEXP_CurrentIndx].IType == IEXP_VWTAB_IDX) &&
		TotalCnt>1)
	{
		sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER_ARE_YOU_SU_35, "Are you sure you want to delete %d items?" ),(m_ItemIndex+1));

		if (MessageBox(IEXP_szText,ResourceString(IDC_ICADEXPLORER_INTELLICAD_36, "IntelliCAD Explorer" ),MB_ICONQUESTION|MB_YESNO)==IDNO)
			return;
	}

	//BugZilla No. 78210; 24-06-2002  [
	switch(IEXP_EntityCol[IEXP_CurrentIndx].IType)
	{
	case IEXP_LATAB_IDX:
		sds_getvar("CLAYER"/*DNT*/,&rb);
		strcpy(IEXP_EntityCol[IEXP_CurrentIndx].CurrentName,rb.resval.rstring);
		break;
	case IEXP_LTTAB_IDX:
		sds_getvar("CELTYPE"/*DNT*/,&rb);
		strcpy(IEXP_EntityCol[IEXP_CurrentIndx].CurrentName,rb.resval.rstring);
		break;
	case IEXP_TSTAB_IDX:
		sds_getvar("TEXTSTYLE"/*DNT*/,&rb);
		strcpy(IEXP_EntityCol[IEXP_CurrentIndx].CurrentName,rb.resval.rstring);
		break;
	case IEXP_DSTAB_IDX:
		sds_getvar("DIMSTYLE"/*DNT*/,&rb);
		strcpy(IEXP_EntityCol[IEXP_CurrentIndx].CurrentName,rb.resval.rstring);
		break;
	}
	//BugZilla No. 78210; 24-06-2002  ]
	// This for loop
	for( ; m_ItemIndex != -1; --m_ItemIndex)
	{
		if (m_Windowsideflag)
			break;
		else if (!m_ListCtl[IEXP_CurrentIndx].GetItemState(m_ItemIndex,LVIS_SELECTED))
			continue;

		// This switch statment sets up the TABLE name and the name of the ITEM to be deleted,
		// and in some cases brings up a dialog to move the entities that relate to that item,
		// to another type.
		switch(IEXP_EntityCol[IEXP_CurrentIndx].IType)
		{
			case IEXP_LATAB_IDX: // Delete the layer.
			{
				IEXP_ItemDefPtr=(IEXP_ItemDef *)m_ListCtl[IEXP_CurrentIndx].GetItemData(m_ItemIndex);
				if (!lstrcmp(IEXP_ItemDefPtr->name(),ResourceString(IDC_ICADEXPLORER_0_18, "0" ))) {
					AfxMessageBox(ResourceString(IDC_ICADEXPLORER_CANNOT_REM_37, "Cannot remove layer 0." )); continue;
				} else if (strchr(IEXP_ItemDefPtr->name(),IEXP_VER_BAR)) {
					AfxMessageBox(ResourceString(IDC_ICADEXPLORER_CANNOT_DEL_38, "Cannot delete an XREF layer.")); continue;
				} else {
					dMessDlg.m_Newdlg.m_type=ResourceString(IDC_ICADEXPLORER_LAYER_39, "Layer" );
					sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER_LAYER__S_40, "Layer %s" ),IEXP_ItemDefPtr->name()); String=IEXP_szText; SetWindowText(String);
					sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER_YOU_HAVE_R_41, "You have requested that IntelliCAD delete the layer %s.  If you want to keep the entities that are currently on this layer you can choose to move them to another layer." ),IEXP_ItemDefPtr->name());
					dMessDlg.m_txtmes=IEXP_szText;
					sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER_DELETES_LA_42, "Deletes layer %s, and all entities on this layer. " ),IEXP_ItemDefPtr->name());
					dMessDlg.m_txtcnt=IEXP_szText;
					sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER_GIVES_YOU__43, "Gives you a choice of layers to which you can move all the entities currently on the layer %s.  Deletes the old layer after moving the entities." ),IEXP_ItemDefPtr->name());
					dMessDlg.m_txtchng=IEXP_szText;
					sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER_LEAVES_THE_44, "Leaves the layer %s, and the entities on it, as they are." ),IEXP_ItemDefPtr->name());
					dMessDlg.m_txtcancel=IEXP_szText;
					sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER_DELETE__S_45, "Delete %s" ),IEXP_ItemDefPtr->name());
					dMessDlg.m_title=IEXP_szText;
					if ((retvalue=dMessDlg.DoModal())==EXP_DEL_CANCEL) return;
					Stype=IEXP_LAYER;
				}
				break;
			}

			case IEXP_LTTAB_IDX: // Delete the Linetype.
			{
				IEXP_ItemDefPtr=(IEXP_ItemDef *)m_ListCtl[IEXP_CurrentIndx].GetItemData(m_ItemIndex);
				if (!lstrcmpi(IEXP_ItemDefPtr->name(),IEXP_BYBLOCK)) {
					AfxMessageBox(ResourceString(IDC_ICADEXPLORER_CANNOT_DEL_46, "Cannot delete linetype BYBLOCK." )); continue;
				} else if (!lstrcmpi(IEXP_ItemDefPtr->name(),IEXP_BYLAYER)) {
					AfxMessageBox(ResourceString(IDC_ICADEXPLORER_CANNOT_DEL_47, "Cannot delete linetype BYLAYER." )); continue;
				} else if (!lstrcmpi(IEXP_ItemDefPtr->name(),IEXP_CONTINUOUS)) {
					AfxMessageBox(ResourceString(IDC_ICADEXPLORER_CANNOT_DEL_48, "Cannot delete linetype CONTINUOUS." )); continue;
				} else if (!lstrcmpi(IEXP_ItemDefPtr->name(),IEXP_CONTINUOUS)) {
					AfxMessageBox(ResourceString(IDC_ICADEXPLORER_CANNOT_DEL_48, "Cannot delete linetype CONTINUOUS." )); continue;
				} else if (strchr(IEXP_ItemDefPtr->name(),IEXP_VER_BAR)) {
					AfxMessageBox(ResourceString(IDC_ICADEXPLORER_CANNOT_DEL_49, "Cannot delete a XREF linetype." )); continue;
				} else {
					/*DG - 15.5.2002*/// We should write "Replace" on the 1st button instead of "Delete" -
					// see dMessDlg.m_txtcnt for layers below.
					dMessDlg.m_BtnTwo = ResourceString(IDC_ICADEXPLORER_REPLACE_80, "Replace");
					/*DG - 15.5.2002*/// We must pass "LAYER", not "LAYERS".
					rbt = SDS_tblgetter(IEXP_LAYER, NULL, TRUE, IEXP_ODNode->m_dbDrawing);
//					rbt=SDS_tblgetter("LAYERS"/*DNT*/,NULL,TRUE,IEXP_ODNode->m_dbDrawing);
					//Bugzilla No. 78194 ; 10-06-2002
					char szLTName[IC_ACADBUF] = {0};
					while(rbt) {
						if (ic_assoc(rbt,6)) continue;
						if (strisame(ic_rbassoc->resval.rstring,IEXP_ItemDefPtr->name())) {
							if (ic_assoc(rbt,2)) continue;
							//Bugzilla No. 78194 ; 10-06-2002
							strcpy(szLTName,ic_rbassoc->resval.rstring);
							sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER_YOU_ARE_AB_51, "You are about to delete the linetype %s. Layer %s currently has this as its linetype." ),IEXP_ItemDefPtr->name(),ic_rbassoc->resval.rstring);
							dMessDlg.m_txtmes=IEXP_szText;
							sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER_ASSIGNS_TH_52, "Assigns the default linetype, %s, to the Layer %s." ),IEXP_CONTINUOUS,ic_rbassoc->resval.rstring);
							dMessDlg.m_txtcnt=IEXP_szText;
							sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER_GIVES_YOU__53, "Gives you a choice of linetypes to assign to layer %s, then deletes the linetype %s." ),ic_rbassoc->resval.rstring,IEXP_ItemDefPtr->name());
							dMessDlg.m_txtchng=IEXP_szText;
							sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER_CANCELS_DE_54, "Cancels deletion of the linetype %s." ),IEXP_ItemDefPtr->name());
							dMessDlg.m_txtcancel=IEXP_szText;
							//Bugzilla No. 78162 ; 20-05-2002
							sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER_DELETE__S_45, "Delete %s" ),IEXP_ItemDefPtr->name());
							dMessDlg.m_title=IEXP_szText;
							if ((retvalue=dMessDlg.DoModal())==EXP_DEL_CANCEL) return;
							if (retvalue==EXP_DEL_CHANGE) String=(LPCSTR)(dMessDlg.m_Newdlg.m_String); else String=IEXP_CONTINUOUS;
							//Bugzilla No. 78194 ; 10-06-2002
							//if ((rbb=sds_buildlist(RTDXF0,IEXP_LAYER,2,ic_rbassoc->resval.rstring,6,(LPCSTR)String,0))==NULL) return;
							if ((rbb=sds_buildlist(RTDXF0,IEXP_LAYER,2,szLTName,6,(LPCSTR)String,0))==NULL) return;
							if(IEXP_RegenOnUndo==FALSE) {
								SDS_SetUndo(IC_UNDO_REGEN_ON_UNDO,NULL,NULL,NULL,SDS_CURDWG);
								IEXP_RegenOnUndo=TRUE;
							}
							if (SDS_tblmesser(rbb,IC_TBLMESSER_MODIFY,IEXP_ODNode->m_dbDrawing)!=RTNORM) {sds_relrb(rbb);return;}
							//Bugzilla No. 78162 ; 20-05-2002
							IEXP_UpdateLineTypeName(rbb);
							sds_relrb(rbb);
						}
						sds_relrb(rbt);
						/*DG - 15.5.2002*/// We must pass "LAYER", not "LAYERS".
						rbt = SDS_tblgetter(IEXP_LAYER, NULL, FALSE, IEXP_ODNode->m_dbDrawing);
//						rbt=SDS_tblgetter(ResourceString(IDC_ICADEXPLORER_LAYERS_50, "LAYERS" ),NULL,FALSE,IEXP_ODNode->m_dbDrawing);
					}
					dMessDlg.m_BtnTwo.Empty();	/*DG - 15.5.2002*/// Restore "Delete" caption.
					dMessDlg.m_Newdlg.m_type=ResourceString(IDC_ICADEXPLORER_LINETYPE_55, "Linetype" );
					sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER_LINETYPE___56, "Linetype %s" ),IEXP_ItemDefPtr->name()); String=IEXP_szText; SetWindowText(String);
					sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER_YOU_ARE_AB_57, "You are about to delete the linetype %s. This will affect entities currently using this linetype." ),IEXP_ItemDefPtr->name());
					dMessDlg.m_txtmes=IEXP_szText;
					sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER_DELETES_AL_58, "Deletes all the entities currently using the linetype %s." ),IEXP_ItemDefPtr->name());
					dMessDlg.m_txtcnt=IEXP_szText;
					sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER_GIVES_YOU__59, "Gives you a choice of linetypes to assign to the entities now using %s, then deletes the old linetype." ),IEXP_ItemDefPtr->name());
					dMessDlg.m_txtchng=IEXP_szText;
					sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER_CANCELS_DE_54, "Cancels deletion of the linetype %s." ),IEXP_ItemDefPtr->name());
					dMessDlg.m_txtcancel=IEXP_szText;
					//Bugzilla No. 78162 ; 20-05-2002
					sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER_DELETE__S_45, "Delete %s" ),IEXP_ItemDefPtr->name());
					dMessDlg.m_title=IEXP_szText;
					if ((retvalue=dMessDlg.DoModal())==EXP_DEL_CANCEL) return;
					Stype=IEXP_LTYPE;
				}
				break;
			}

			case IEXP_TSTAB_IDX: // Delete the Textstyle.
			{
				sds_getvar("DIMTXSTY"/*DNT*/,&rb);
				currenttextsty=(char *)new char[strlen(rb.resval.rstring)+1];
				strcpy(currenttextsty,rb.resval.rstring);
				IC_FREE(rb.resval.rstring);

				IEXP_ItemDefPtr=(IEXP_ItemDef *)m_ListCtl[IEXP_CurrentIndx].GetItemData(m_ItemIndex);
				rbb=SDS_tblgetter(IEXP_EntityCol[IEXP_CurrentIndx].cType,NULL,TRUE,IEXP_ODNode->m_dbDrawing);
				if (ic_assoc(rbb,2)) continue;
				if (strisame(ic_rbassoc->resval.rstring,IEXP_ItemDefPtr->name())) {
					sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER_CANNOT_DEL_60, "Cannot delete text style %s." ),IEXP_ItemDefPtr->name());
					AfxMessageBox(IEXP_szText); continue;
				} else if (strchr(IEXP_ItemDefPtr->name(),IEXP_VER_BAR)) {
					AfxMessageBox(ResourceString(IDC_ICADEXPLORER_CANNOT_DEL_61, "Cannot delete an XREF text style." )); continue;
				} else {
					dMessDlg.m_Newdlg.m_type=ResourceString(IDC_ICADEXPLORER_TEXT_STYLE_62, "Text style" );
					sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER_TEXT_STYLE_63, "Text style %s" ),IEXP_ItemDefPtr->name()); String=IEXP_szText; SetWindowText(String);
					sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER_YOU_ARE_AB_64, "You are about to delete the text style %s. This will affect entities currently using this text style." ),IEXP_ItemDefPtr->name());
					dMessDlg.m_txtmes=IEXP_szText;
					sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER_DELETES_AL_65, "Deletes all entities currently using text style %s." ),IEXP_ItemDefPtr->name());
					dMessDlg.m_txtcnt=IEXP_szText;
					sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER_GIVES_YOU__66, "Gives you a choice of text styles to assign to the entities now using %s, then deletes the old text style." ),IEXP_ItemDefPtr->name());
					dMessDlg.m_txtchng=IEXP_szText;
					sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER_CANCELS_DE_67, "Cancels deletion of the text style %s." ),IEXP_ItemDefPtr->name());
					dMessDlg.m_txtcancel=IEXP_szText;
					//Bugzilla No. 78162 ; 20-05-2002
					sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER_DELETE__S_45, "Delete %s" ),IEXP_ItemDefPtr->name());
					dMessDlg.m_title=IEXP_szText;
					if ((retvalue=dMessDlg.DoModal())==EXP_DEL_CANCEL) return;
					Stype=IEXP_STYLE;
				}
				sds_relrb(rbb);
				break;
			}

			case IEXP_BNTAB_IDX: // Delete the block.
			{
				IEXP_ItemDefPtr = (IEXP_ItemDef *) m_ListCtl[IEXP_CurrentIndx].GetItemData(m_ItemIndex);
				dYMessDlg.m_bNested = FALSE;
				//IsBlockNested (IEXP_ItemDefPtr->name());
				//For future expansion of the dYMessDlg
				dYMessDlg.m_Blockname=IEXP_ItemDefPtr->name();

				if (rettype != EXP_YESTOALL)
				{
					if (ID_CANCEL == (rettype = dYMessDlg.DoModal()))
						return;

					switch (rettype)
					{
						case EXP_NO:
							continue;
							break;

						case EXP_YES:
						case EXP_YESTOALL:
							Stype = IEXP_BLOCK;
							retvalue = EXP_DEL_CONTINUE;
							break;
					}
				}

				// Detach in the case that the block is an xref.
				if (0 < cmd_xref_uncouple(IEXP_ItemDefPtr->name(), IC_UNCOUPLE_DETACH, false))
				{
					// Set flag to indicate that an xref has been detached.
					bXrefDetached = true;

					// Nothing more to do for this block, continue.
					continue;
				}
				else if (IsBlockNested(IEXP_ItemDefPtr->name()))
				{
					sprintf(IEXP_szText, ResourceString(IDC_ICADEXPLORER_4, "The block %s is a nested block.  It cannot be deleted."), IEXP_ItemDefPtr->name());
					MessageBox(IEXP_szText, ResourceString(IDC_ICADEXPLORER_INTELLICAD_36, "IntelliCAD Explorer"), MB_ICONEXCLAMATION | MB_OK);
					continue;
				}

				break;
			}

			case IEXP_VWTAB_IDX: // Delete the View.
			{
				Stype=IEXP_VIEW;
				IEXP_ItemDefPtr=(IEXP_ItemDef *)m_ListCtl[IEXP_CurrentIndx].GetItemData(m_ItemIndex);
				if (TotalCnt==1) {
					sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER_ARE_YOU_SU_68, "Are you sure you want to delete %s" ),IEXP_ItemDefPtr->name());
					if (MessageBox(IEXP_szText,ResourceString(IDC_ICADEXPLORER_INTELLICAD_36, "IntelliCAD Explorer" ),MB_ICONQUESTION|MB_YESNO)==IDNO) return;
				}
				break;
			}

			case IEXP_CSTAB_IDX: // Delete the UCS.
			{
				IEXP_ItemDefPtr=(IEXP_ItemDef *)m_ListCtl[IEXP_CurrentIndx].GetItemData(m_ItemIndex);
				if (!lstrcmp(IEXP_ItemDefPtr->name(),IEXP_WORLD)) {
					AfxMessageBox(ResourceString(IDC_ICADEXPLORER_CANNOT_DEL_69, "Cannot delete UCS WORLD." )); continue;
				} else {
					Stype=IEXP_UCS;
				}
				if (TotalCnt==1) {
					sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER_ARE_YOU_SU_68, "Are you sure you want to delete %s" ),IEXP_ItemDefPtr->name());
					if (MessageBox(IEXP_szText,ResourceString(IDC_ICADEXPLORER_INTELLICAD_36, "IntelliCAD Explorer" ),MB_ICONQUESTION|MB_YESNO)==IDNO) return;
				}
				break;
			}

			case IEXP_DSTAB_IDX: // Delete the Dimstyle.
			{
				sds_getvar("DIMSTYLE",&rb);
				currentdimsty=(char *)new char[strlen(rb.resval.rstring)+1];
				strcpy(currentdimsty,rb.resval.rstring);
				IC_FREE(rb.resval.rstring);

				IEXP_ItemDefPtr=(IEXP_ItemDef *)m_ListCtl[IEXP_CurrentIndx].GetItemData(m_ItemIndex);
				//Bugzilla No. 78070 ; 17-06-2002
				//rbb=SDS_tblgetter(IEXP_EntityCol[IEXP_CurrentIndx].cType,NULL,TRUE,IEXP_ODNode->m_dbDrawing);
				//if (ic_assoc(rbb,2)) continue;
				//if (strisame(ic_rbassoc->resval.rstring,IEXP_ItemDefPtr->name())) {
				if (strisame(currentdimsty,IEXP_ItemDefPtr->name())) {
					sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER_6, "Cannot delete dimension style %s."),IEXP_ItemDefPtr->name());
					AfxMessageBox(IEXP_szText); continue;
				} else if (strchr(IEXP_ItemDefPtr->name(),IEXP_VER_BAR)) {
					AfxMessageBox(ResourceString(IDC_ICADEXPLORER_7, "Cannot delete an XREF dimension style.")); continue;
				} else {
					dMessDlg.m_Newdlg.m_type="Dimension style";
					sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER_8, "Dimension style %s"),IEXP_ItemDefPtr->name()); String=IEXP_szText; SetWindowText(String);
					sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER_9, "You are about to delete the Dimension style %s. This will affect entities currently using this Dimension style."),IEXP_ItemDefPtr->name());
					dMessDlg.m_txtmes=IEXP_szText;
					sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER_10, "Deletes all entities currently using Dimension style %s."),IEXP_ItemDefPtr->name());
					dMessDlg.m_txtcnt=IEXP_szText;
					sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER_11, "Gives you a choice of Dimension styles to assign to the entities now using %s, then deletes the old Dimension style."),IEXP_ItemDefPtr->name());
					dMessDlg.m_txtchng=IEXP_szText;
					sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER_12, "Cancels deletion of the Dimension style %s."),IEXP_ItemDefPtr->name());
					dMessDlg.m_txtcancel=IEXP_szText;
					//Bugzilla No. 78162 ; 20-05-2002
					sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER_DELETE__S_45, "Delete %s" ),IEXP_ItemDefPtr->name());
					dMessDlg.m_title=IEXP_szText;
					if ((retvalue=dMessDlg.DoModal())==EXP_DEL_CANCEL) return;
					Stype=IEXP_DIMSTY;
				}
				//Bugzilla No. 78070 ; 17-06-2002
				//sds_relrb(rbb);
				break;
			}
		}

		// If an xref has been detached we don't need to delete the block anymore.
		if (!bXrefDetached)
		{
			// Build the resbuf list then call table messer to delete the ITEM.
			if (NULL == (rbb = sds_buildlist(RTDXF0,(LPCSTR) (Stype), 2, IEXP_ItemDefPtr->name(), 0)))
				return;

			if (IEXP_RegenOnUndo == FALSE)
				{
				SDS_SetUndo(IC_UNDO_REGEN_ON_UNDO,NULL,NULL,NULL,SDS_CURDWG);
				IEXP_RegenOnUndo=TRUE;
				}

			if (RTNORM != SDS_tblmesser(rbb, IC_TBLMESSER_DELETE, IEXP_ODNode->m_dbDrawing))
				{
				sds_relrb(rbb);
				return;
				}

			sds_relrb(rbb);
		}

		switch(IEXP_EntityCol[IEXP_CurrentIndx].IType)
			{
			case IEXP_LATAB_IDX: IEXP_LayerCnt--;  break;
			case IEXP_LTTAB_IDX: IEXP_LinetCnt--;  break;
			case IEXP_TSTAB_IDX: IEXP_TStyleCnt--; break;
			case IEXP_CSTAB_IDX: IEXP_UCSCnt--;    break;
			case IEXP_VWTAB_IDX: IEXP_ViewCnt--;   break;
			case IEXP_BNTAB_IDX: IEXP_BlockCnt--;  break;
			case IEXP_DSTAB_IDX: IEXP_DimStyleCnt--;break;
			}

		switch(IEXP_EntityCol[IEXP_CurrentIndx].IType)
			{
			case IEXP_BNTAB_IDX: // Delete the block.
				IEXP_Findinfo.psz	=IEXP_EntityCol[IEXP_CurrentIndx].CurrentName;
				Index=m_ListCtl[IEXP_CurrentIndx].FindItem(&IEXP_Findinfo);
				if (m_ItemIndex<Index)
					{
					String=m_ListCtl[IEXP_CurrentIndx].GetItemText(m_ItemIndex,0);
					strcpy(IEXP_EntityCol[IEXP_CurrentIndx].CurrentName,(char *)((LPCTSTR)String));
					}
				// Delete the block from the (CListCtrl).
				if (!m_ListCtl[IEXP_CurrentIndx].DeleteItem(m_ItemIndex))
					return;
				bDeleted = TRUE;
				break;
			default:
				// Delete the item from the (CListCtrl).
				rb.restype=RTSTR;
				if (!m_ListCtl[IEXP_CurrentIndx].DeleteItem(m_ItemIndex))
					return;
				bDeleted = TRUE;
				switch(IEXP_EntityCol[IEXP_CurrentIndx].IType)
					{
						case IEXP_TSTAB_IDX:
							if (strisame(currenttextsty,IEXP_ItemDefPtr->name()))
								{
								//if the current text style has been deleted, set STANDARD as current textstyle
								rb.resval.rstring=(char *)IEXP_STANDARD;
								sds_setvar("DIMTXSTY"/*DNT*/,&rb);
								}
							break;
						case IEXP_DSTAB_IDX:
							if (strisame(currentdimsty,IEXP_ItemDefPtr->name()))
								{
								//if the current dim style has been deleted, set STANDARD as current dimstyle
								//Since STANDARD is not guaranteed to be present, we get the first dimstyle as the standard
								//dimstyle.  We are guaranteed that there is at least one dimstyle
								rbb=SDS_tblgetter(IEXP_EntityCol[IEXP_CurrentIndx].cType,NULL,TRUE,IEXP_ODNode->m_dbDrawing);
								if (ic_assoc(rbb,2))
									continue;
								rb.resval.rstring=ic_realloc_char(rb.resval.rstring,(strlen(ic_rbassoc->resval.rstring)+1));
								strcpy(rb.resval.rstring, ic_rbassoc->resval.rstring);
								sds_setvar("DIMSTYLE",&rb);
								sds_relrb(rbb);
								}
							break;
					}

				if (strsame(IEXP_EntityCol[IEXP_CurrentIndx].CurrentName,IEXP_ItemDefPtr->name()))
					{
					rb.restype=RTSTR;
					IEXP_Findinfo.psz = NULL;
					switch(IEXP_EntityCol[IEXP_CurrentIndx].IType)
						{
						case IEXP_LATAB_IDX:
							IEXP_Findinfo.psz	=IEXP_ZERO;
							rb.resval.rstring=(char *)IEXP_ZERO;
							sds_setvar("CLAYER"/*DNT*/,&rb);
							break;
						case IEXP_LTTAB_IDX:
							IEXP_Findinfo.psz	=IEXP_CONTINUOUS;
							rb.resval.rstring=(char *)IEXP_CONTINUOUS;
							sds_setvar("CELTYPE"/*DNT*/,&rb);
							break;
						case IEXP_TSTAB_IDX:
							IEXP_Findinfo.psz	=IEXP_STANDARD;
							rb.resval.rstring=(char *)IEXP_STANDARD;
							sds_setvar("TEXTSTYLE"/*DNT*/,&rb);
							break;
						case IEXP_DSTAB_IDX:
							rbb=SDS_tblgetter(IEXP_EntityCol[IEXP_CurrentIndx].cType,NULL,TRUE,IEXP_ODNode->m_dbDrawing);
							if (ic_assoc(rbb,2)) continue;
							rb.resval.rstring=ic_realloc_char(rb.resval.rstring,(strlen(ic_rbassoc->resval.rstring)+1));
							strcpy((char *)rb.resval.rstring, ic_rbassoc->resval.rstring);
							IEXP_Findinfo.psz = rb.resval.rstring;
							sds_relrb(rbb);
							sds_setvar("DIMSTYLE"/*DNT*/,&rb);
							break;
						}
					if (IEXP_Findinfo.psz)
						{
						Index=m_ListCtl[IEXP_CurrentIndx].FindItem(&IEXP_Findinfo);
						strcpy(IEXP_EntityCol[IEXP_CurrentIndx].CurrentName,IEXP_Findinfo.psz);
						if (!m_ListCtl[IEXP_CurrentIndx].SetItem(Index,0,LVIF_IMAGE,NULL,0,0,0,0))
							return;
						}
					}
				break;
			}

		/* The following code is now done in the function CleanupEntsInsideBlocks - commenting it out for now.

		// Change all the entities that are on the layer being deleted and are part of a block definition to layer zero.
		struct resbuf *rbindex,
		BOOL		  DeleteInsert;
		if (IEXP_EntityCol[IEXP_CurrentIndx].IType!=IEXP_CSTAB_IDX && IEXP_EntityCol[IEXP_CurrentIndx].IType!=IEXP_VWTAB_IDX) {
			if ((rbindex=SDS_tblgetter(IEXP_EntityCol[IEXP_BNTAB_IDX].cType,NULL,TRUE,IEXP_ODNode->m_dbDrawing))!=NULL) {
				if (ic_assoc(rbindex,-2)) return;
				sds_name_set(ic_rbassoc->resval.rlname,ename);
				while(rbindex) {
					while(TRUE) {
						rbb=sds_entget(ename);
						for(DeleteInsert=FALSE,rbt=rbb;rbt!=NULL;rbt=rbt->rbnext) {
							switch(rbt->restype) {
								case 2:
									if (!DeleteInsert) break;
									if (strisame(IEXP_ItemDefPtr->name(),rbt->resval.rstring)) {
										SDS_DontUpdateDisp=TRUE;
										sds_entdel(ename);
										IEXP_ODNode->m_dbDrawing->set_nmainents(IEXP_ODNode->m_dbDrawing->ret_nmainents()+1);
										SDS_DontUpdateDisp=FALSE;
										m_regenstate=TRUE;
									}
									break;
								case 0:
									if (IEXP_EntityCol[IEXP_CurrentIndx].IType==IEXP_BNTAB_IDX) {
										if (strisame(ResourceString(IDC_ICADEXPLORER_INSERT_70, "insert" ),rbt->resval.rstring)) {
										   DeleteInsert=TRUE;
										}
									}
									break;
								case 8:
									if (IEXP_EntityCol[IEXP_CurrentIndx].IType==IEXP_LATAB_IDX) {
										if (strsame(IEXP_ItemDefPtr->name(),rbt->resval.rstring)) {
											rbt->resval.rstring=(char *)realloc(rbt->resval.rstring,(strlen(IEXP_ZERO)+1));
											strcpy(rbt->resval.rstring,IEXP_ZERO);
											sds_entmod(rbb);
										}
									}
									break;
								case 6:
									if (IEXP_EntityCol[IEXP_CurrentIndx].IType==IEXP_LTTAB_IDX) {
										if (strsame(IEXP_ItemDefPtr->name(),rbt->resval.rstring)) {
											rbt->resval.rstring=(char *)realloc(rbt->resval.rstring,(strlen(IEXP_CONTINUOUS)+1));
											strcpy(rbt->resval.rstring,IEXP_CONTINUOUS);
											sds_entmod(rbb);
										}
									}
									break;
								case 7:
									if (IEXP_EntityCol[IEXP_CurrentIndx].IType==IEXP_TSTAB_IDX) {
										struct resbuf *rbbdim;
										if (strsame(IEXP_ItemDefPtr->name(),rbt->resval.rstring)) {
											rbbdim=SDS_tblgetter(IEXP_EntityCol[IEXP_CurrentIndx].cType,NULL,TRUE,IEXP_ODNode->m_dbDrawing);
											if (ic_assoc(rbbdim,2)) continue;
											rbt->resval.rstring=(char *)realloc(rbt->resval.rstring,(strlen(ic_rbassoc->resval.rstring)+1));
											strcpy(rbt->resval.rstring, ic_rbassoc->resval.rstring);
											sds_relrb(rbbdim);
											sds_entmod(rbb);
										}
									}
									break;
								//TODO DIMSTYLE - is there a distyle assoc with a block that needs to be fixed here?
							}
						}
						IC_RELRB(rbb);
						if (sds_entnext_noxref(ename,ename)!=RTNORM) break;
					}
					IC_RELRB(rbb);
					IC_RELRB(rbindex);
					rbindex=SDS_tblgetter(IEXP_EntityCol[IEXP_BNTAB_IDX].cType,NULL,FALSE,IEXP_ODNode->m_dbDrawing);
					if (rbindex) sds_name_set(rbindex->resval.rlname,ename);
				}
			}
		}
		*/

		// Change all entities that have a link to the DIMSTYLE table
		if (IEXP_EntityCol[IEXP_CurrentIndx].IType==IEXP_TSTAB_IDX) {
			if ((rbb=sds_tblsearch("STYLE"/*DNT*/,IEXP_STANDARD,0))==NULL) return;
			if (ic_assoc(rbb,-1)) return;
			ic_encpy(ename,ic_rbassoc->resval.rlname);
			sds_relrb(rbb);
			rbb=SDS_tblgetter("DIMSTYLE"/*DNT*/,NULL,TRUE,IEXP_ODNode->m_dbDrawing);
			while(rbb) {
				for(rbt=rbb;rbt!=NULL && rbt->restype!=340;rbt=rbt->rbnext);
				if (rbt) {
					db_handitem   *handitem;
					char		  *string;
					handitem=(db_handitem *)((long)rbt->resval.rlname[0]);
					handitem->get_2(&string);
					if (strsame(IEXP_ItemDefPtr->name(),string)) {
						ic_encpy(rbt->resval.rlname,ename);
						sds_entmod(rbb);
					}
				}
				IC_RELRB(rbb);
				rbb=SDS_tblgetter("DIMSTYLE"/*DNT*/,NULL,FALSE,IEXP_ODNode->m_dbDrawing);
			}
		}

		ename[1]=(long)IEXP_ODNode->m_dbDrawing;
		handItem=NULL; rbb=NULL;
		while((handItem=dbDrawing->entnext_noxref(handItem))!=NULL)
		{
			switch(IEXP_EntityCol[IEXP_CurrentIndx].IType)
			{
				case IEXP_LATAB_IDX:
						ename[0]=(long)handItem;
						// Get the type of entity.
						handItem->get_0(IEXP_szText,255);
						// IF the entity is an attrib then change layer name to the inserts layer name.
						if (strsame(IEXP_szText,"ATTRIB")) {
							InsertItem->get_8(IEXP_szText,255);
							rbb=sds_entget(ename);
							for(rbt=rbb;rbt!=NULL && rbt->restype!=8;rbt=rbt->rbnext);
							if (rbt!=NULL) {
								rbt->resval.rstring=ic_realloc_char(rbt->resval.rstring,(strlen(IEXP_szText)+1));
								strcpy(rbt->resval.rstring,IEXP_szText);
								sds_entmod(rbb);
								sds_relrb(rbb);
								m_regenstate=TRUE;
							}
							continue;
						}
						// Save the ename of the entity if it is an insert.
						if (strsame(IEXP_szText,"INSERT")) InsertItem=handItem;
						handItem->get_8(IEXP_szText,255);
						if (!strsame(IEXP_ItemDefPtr->name(),IEXP_szText)) continue;
						rbb=sds_entget(ename);
						for(rbt=rbb;rbt!=NULL && rbt->restype!=8;rbt=rbt->rbnext);
						if (rbt!=NULL) {
							rbt->resval.rstring=ic_realloc_char(rbt->resval.rstring,dMessDlg.m_Newdlg.m_String.GetLength()+1);
							strcpy(rbt->resval.rstring,(LPCSTR)(dMessDlg.m_Newdlg.m_String));
						}
						break;
				case IEXP_LTTAB_IDX:
						handItem->get_6(IEXP_szText,255);
						if (!strsame(IEXP_ItemDefPtr->name(),IEXP_szText)) continue;
						ename[0]=(long)handItem;
						rbb=sds_entget(ename);
						for(rbt=rbb;rbt!=NULL && rbt->restype!=6;rbt=rbt->rbnext);
						if (rbt!=NULL) {
							rbt->resval.rstring=ic_realloc_char(rbt->resval.rstring,dMessDlg.m_Newdlg.m_String.GetLength()+1);
							strcpy(rbt->resval.rstring,(LPCSTR)(dMessDlg.m_Newdlg.m_String));
						}
						break;
				case IEXP_TSTAB_IDX:
						handItem->get_7(IEXP_szText,255);
						if (!strsame(IEXP_ItemDefPtr->name(),IEXP_szText)) continue;
						ename[0]=(long)handItem;
						rbb=sds_entget(ename);
						for(rbt=rbb;rbt!=NULL && rbt->restype!=7;rbt=rbt->rbnext);
						if (rbt!=NULL) {
							rbt->resval.rstring=ic_realloc_char(rbt->resval.rstring,dMessDlg.m_Newdlg.m_String.GetLength()+1);
							strcpy(rbt->resval.rstring,(LPCSTR)(dMessDlg.m_Newdlg.m_String));
						}
						break;
				case IEXP_BNTAB_IDX:
						handItem->get_2(IEXP_szText,255);
						if (!strsame(IEXP_ItemDefPtr->name(),IEXP_szText)) continue;
						ename[0]=(long)handItem;
						rbb=sds_entget(ename);
						for(rbt=rbb;rbt!=NULL && rbt->restype!=2;rbt=rbt->rbnext);
						if (rbt!=NULL) {
							rbt->resval.rstring=ic_realloc_char(rbt->resval.rstring,dMessDlg.m_Newdlg.m_String.GetLength()+1);
							strcpy(rbt->resval.rstring,(LPCSTR)(dMessDlg.m_Newdlg.m_String));
						}
						break;
				case IEXP_CSTAB_IDX:
				case IEXP_VWTAB_IDX:
						/* Really we have no entities to delete for Views or UCSs.
						handItem->get_2(IEXP_szText,255);
						if (!strsame(IEXP_ItemDefPtr->name(),IEXP_szText)) continue;
						ename[0]=(long)handItem;
						rbb=sds_entget(ename);
						for(rbt=rbb;rbt!=NULL && rbt->restype!=7;rbt=rbt->rbnext);
						if (rbt!=NULL) {
							rbt->resval.rstring=(char *)realloc(rbt->resval.rstring,dMessDlg.m_Newdlg.m_String.GetLength()+1);
							strcpy(rbt->resval.rstring,(LPCSTR)(dMessDlg.m_Newdlg.m_String));
						}
						*/
						break;
				case IEXP_DSTAB_IDX:
						// Save MTEXT from deleting/renaming DIMSTYLE.
						switch(handItem->ret_type())
						{
						case DB_DIMENSION:
						case DB_LEADER:
						case DB_TOLERANCE:
							break;
						default:
							continue;
						}
						handItem->get_3(IEXP_szText,255);
						if (!strsame(IEXP_ItemDefPtr->name(),IEXP_szText)) continue;
						ename[0]=(long)handItem;
						rbb=sds_entget(ename);
						for(rbt=rbb;rbt!=NULL && rbt->restype!=3;rbt=rbt->rbnext);
						if (rbt!=NULL)
							{
							rbt->resval.rstring=ic_realloc_char(rbt->resval.rstring,dMessDlg.m_Newdlg.m_String.GetLength()+1);
							strcpy(rbt->resval.rstring,(LPCSTR)(dMessDlg.m_Newdlg.m_String));
							}
						break;

					continue;
			}
			switch(retvalue)
			{
				case EXP_DEL_CONTINUE:
					sds_entdel(ename);
					break;
				case EXP_DEL_CHANGE:
					sds_entmod(rbb);
//					  sds_relrb(rbb);
					m_regenstate=TRUE;
					break;
			}
			IC_RELRB(rbb);
		}
		SDS_DontUpdateDisp=TRUE;
		if (CleanupEntsInsideBlocks (IEXP_EntityCol[IEXP_CurrentIndx].IType, IEXP_ItemDefPtr->name(), retvalue, dMessDlg.m_Newdlg.m_String, IEXP_ODNode->m_dbDrawing))
			m_regenstate=TRUE;
		SDS_DontUpdateDisp=FALSE;

		if (bDeleted)
		{
			//Delete the item data before deleting the block
			delete IEXP_ItemDefPtr;
			IEXP_ItemDefPtr = NULL;
		}
		bDeleted=FALSE;
	}

	if (currenttextsty)
		delete [] currenttextsty;

	if (currentdimsty)
		delete [] currentdimsty;

	m_ItemIndex=SaveIndex;

	if (bXrefDetached)
	{
		// Update other data like layers and dimstyles if an xref has been detached.
		UpdateDataList();
		m_ListCtl[IEXP_CurrentIndx].EnableWindow(TRUE);
		m_ListCtl[IEXP_CurrentIndx].ShowWindow(SW_SHOW);
	}
	if(m_regenstate && m_Explorer->m_regenMode)
	{
		cmd_regenall();
		m_regenstate = FALSE;
	}
}


void IEXP_CdetailView::SelectAll()
{
	short Index, Count;

	if (m_Explorer->m_editmode)
		m_ListCtl[IEXP_CurrentIndx].EndLabelEditing();

	Count = m_ListCtl[IEXP_CurrentIndx].GetItemCount();

	for(Index=0;Index<Count;++Index)
	{
		m_ListCtl[IEXP_CurrentIndx].SetItemState(Index, LVIS_SELECTED, LVIS_SELECTED);
	}

	return;
}

///////////////////////////////////////////////////////////////////////
// This function opens up a dialog that is related to the TYPE of ITEM
// selected and displays all information for editing.
//
// Nothing is returned if it failed.
//
void IEXP_CdetailView::PropertyDlg() {
	RECT		   rect;
	long		   xOff,
				   yOff;

	if (m_DlgState) return;
	m_DlgState=TRUE;
	if (m_EditDlg) delete m_EditDlg;
	m_EditDlg = new IEXP_EditDlg();
	m_EditDlg->m_CdetailView=this;
	m_EditDlg->m_dataptr	=m_dataptr;
	if (!m_EditDlg->Create(EXP_PROPDLG,this)) return;
	m_EditDlg->SetWindowText(IEXP_TABINFO[IEXP_CurrentIndx].Name);
	m_EditDlg->GetWindowRect(&rect);

	xOff=m_lpoint.x-rect.left;
	yOff=m_lpoint.y-rect.top;
	rect.left	+=xOff;
	rect.right	+=xOff;
	rect.top	+=yOff;
	rect.bottom +=yOff;

	m_EditDlg->MoveWindow(&rect);
	m_EditDlg->ShowWindow(SW_SHOW);
	m_EditDlg->UpdateWindow();
}

///////////////////////////////////////////////////////////////////////
// This function creates a new ITEM in the drawing,it uses tablemesser
// for the update so we can undo it.
//
// Nothing is if it failed.
//
void IEXP_CdetailView::InsertRow(UINT Id) {
   	LV_ITEM 	   lvI;
	int 		   retvalue;
	short		   Cnt,
				   offindex,
				   index,
				   rindex,
				   selcnt,
				   typeidx;
	char		  *ptr1,
				  *ptr2,
				   fs2[IC_ACADBUF],
				  *fcp1;
	struct resbuf  rb,
				  *rbb,
				  *newrbb,
				  *rbt;
	sds_real	   len;
	IEXP_LTDlg	   ltdlg;
	IEXP_LTList    ltlst;
	BOOL		   NameExist;
	//EBATECH(CNBR)-[ 2002/9/20 Default linetype file is dependent on $MEASUREMENT.
	int	measurement;
	SDS_getvar(NULL,DB_QMEASUREMENT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	measurement = rb.resval.rint;
	//EBATECH(CNBR)]-

	lvI.mask	  =LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
	lvI.state	  =0;
	lvI.stateMask =0;
	m_Newflag	  =TRUE;

	// The ID passed is differnt then the TYPE id so I need to make sure the type id is set.
	switch(Id) {
		case EXP_NEW_T: typeidx=IEXP_CurrentIndx;	break;
		case EXP_NEW_LAYER_M: typeidx=IEXP_LATAB_IDX; break;
		case EXP_NEW_LINET_M: typeidx=IEXP_LTTAB_IDX; break;
		case EXP_NEW_STYLE_M: typeidx=IEXP_TSTAB_IDX; break;
		case EXP_NEW_UCS_M:   typeidx=IEXP_CSTAB_IDX; break;
		case EXP_NEW_VIEW_M:  typeidx=IEXP_VWTAB_IDX; break;
		case EXP_NEW_BLOCK_M: typeidx=IEXP_BNTAB_IDX; break;
		case EXP_NEW_DIMSTY_M:typeidx=IEXP_DSTAB_IDX; break;
	}
	if (m_Explorer->m_MainWindow->m_ExpReturnHwnd && (typeidx==IEXP_BNTAB_IDX || typeidx==IEXP_VWTAB_IDX || typeidx==IEXP_CSTAB_IDX)) {
		AfxMessageBox(ResourceString(IDC_ICADEXPLORER_CAN_T_DO_T_33, "This operation cannot be performed.  Please run Explorer directly from the Tools menu to perform this operation."));
		return;
	}
	if (SDS_CURDWG!=IEXP_ODNode->m_dbDrawing) m_Explorer->ActivateCurrentView(IEXP_ODNode->m_dbDrawing);
	if (Id!=EXP_NEW_T && typeidx!=IEXP_CurrentIndx) {
		m_CItemList->m_TreeCtl.Select(IEXP_EntityCol[typeidx].hITem,TVGN_CARET);
		ChangeWindow((WPARAM)IEXP_CurrentIndx,(LPARAM)typeidx);
	}
	switch(typeidx) {
		case IEXP_LATAB_IDX:
				// Insert Layer. Build the resbuf link list of new (TYPE), Call tablemesser to insert the new (TYPE), set a pointer
				// to the layer table and, get the number of rows.
				strcpy(IEXP_szText,ResourceString(IDC_IEXP_NEW_LAYER, "NewLayer")); index=0;
				do {
					NameExist=FALSE;
					sprintf(IEXP_szText,"%s%d"/*DNT*/,ResourceString(IDC_IEXP_NEW_LAYER, "NewLayer"),(++index));
					if ((rbb=sds_buildlist(RTDXF0,IEXP_LAYER,2,IEXP_szText,62,ic_colornum(IEXP_COLOR_WHITE),6,IEXP_CONTINUOUS,0))==NULL) return;
					if (SDS_tblmesser(rbb,IC_TBLMESSER_MAKE,IEXP_ODNode->m_dbDrawing)!=RTNORM) NameExist=TRUE;
					sds_relrb(rbb);
				} while(NameExist);
				rbb=SDS_tblgetter(IEXP_EntityCol[IEXP_CurrentIndx].cType,IEXP_szText,TRUE,IEXP_ODNode->m_dbDrawing);
				Cnt=m_ListCtl[IEXP_CurrentIndx].GetItemCount();
				// Set this variable to null so it can be reset in the insert layer.
				m_dataptr=NULL;
				// Insert a new row for the Layer in the (CListCtrl).
				if (!LayerIns(Cnt,0,IEXP_CurrentIndx,NULL,lvI,rbb)) {sds_relrb(rbb); return;}
				sds_relrb(rbb);
				break;
		case IEXP_LTTAB_IDX: // Insert Linetype.
				// Display new linetype dialog for description, and definition.
				rindex=0; index=0;
				do {
					retvalue=ltdlg.DoModal();
					if (retvalue==EXP_BROWSE)
					{
						char fullFileName[IC_ACADBUF];
						fullFileName[0]=0;
						//EBATECH(CNBR)-[ 2002/9/20 Default linetype file is dependent on $MEASUREMENT.
						if ((sds_findfile( measurement == DB_ENGLISH_MEASUREINIT ?
							SDS_LTYPEFILE : SDS_ISOLTYPEFILE, &fullFileName[0]))!=RTNORM)
						{
							strcpy(&fullFileName[0], measurement == DB_ENGLISH_MEASUREINIT ?
								SDS_LTYPEFILE : SDS_ISOLTYPEFILE);
							rindex = 1;	//set this so that if neighter icad.lin nor icadiso.lin was not found, user can select linetype file
						}
						//if ((sds_findfile(SDS_LTYPEFILE,&fullFileName[0]))!=RTNORM)
						//{
						//	strcpy(&fullFileName[0],SDS_LTYPEFILE);
						//	rindex = 1;	//set this so that if icad.lin was not found, user can select linetype file
						//}
						//EBATECH(CNBR)]-
						do
						{
							if (rindex)
							{
								// EBATECH(CNBR) 2002/4/30 Disable TYPE button option+2
								if ((sds_getfiled(ResourceString(IDC_ICADEXPLORER_SELECT_LIN_73, "Select Linetype File" ),&fullFileName[0],ResourceString(IDC_ICADEXPLORER_LINETYPE_F_74, "Linetype File|lin" ),2,&rb))!=RTNORM)
								{
									rb.resval.rstring=NULL;
									return;
								}
								else
								{
									ltlst.m_Filename=rb.resval.rstring;
									IC_FREE(rb.resval.rstring);
								}
							}
							else
							{
								ltlst.m_Filename=&fullFileName[0];
							}
							if ((index=ltlst.DoModal())==IDCANCEL)
							{
								ltlst.m_ret=0;
								break ;
							}
							if (index==EXP_BROWSE2)
							{
								rindex=1;
								continue;
							}
							offindex=IEXP_LinetCnt;
							rindex=0;
							break;
						} while(TRUE);

						if (ltlst.m_ret==0)
							continue;

					} else break;
				} while(!ltlst.m_ret);

				if (retvalue==IDOK && ltdlg.m_Desc && ltdlg.m_Defstr) {
					// parse the definition string.
					for(len=0,index=1,ptr1=(char *)(LPCTSTR)ltdlg.m_Defstr;ptr2=strchr(ptr1,IEXP_COMMA);ptr1=ptr2+1,++index) {
						*ptr2=0; len+=fabs(atof(ptr1)); *ptr2=IEXP_COMMA;
					}
					if(*ptr1)	//D.Gavrilov. If the last symbol in ltdlg.m_Defstr is a ',' then we must decrement index.
						len+=fabs(atof(ptr1));
					else
						index--;
					strcpy(IEXP_szText,ResourceString(IDC_IEXP_NEW_LINETYPE, "NewLinetype")); rindex=0;
					do {
						NameExist=FALSE;
						sprintf(IEXP_szText,"%s%d"/*DNT*/,ResourceString(IDC_IEXP_NEW_LINETYPE, "NewLinetype"),(++rindex));
						if ((rbb=sds_buildlist(RTDXF0,IEXP_LTYPE,2,IEXP_szText,70,0,3,ltdlg.m_Desc,73,index,40,len,0))==NULL) return;
						for(rbt=rbb;rbt->rbnext;rbt=rbt->rbnext);
						for(index=1,ptr1=(char *)(LPCTSTR)ltdlg.m_Defstr;ptr2=strchr(ptr1,IEXP_COMMA);ptr1=ptr2+1,++index,rbt=rbt->rbnext) {
							*ptr2=0; rbt->rbnext=sds_newrb(49); rbt->rbnext->resval.rreal=atof(ptr1);
							*ptr2 = IEXP_COMMA;	//D.Gavrilov. We must do it to save a string as it was (for next do-loops)
						}
						if(*ptr1)	//D.Gavrilov. We should add the last 49 ONLY in this case.
						{
							rbt->rbnext = sds_newrb(49);
							rbt->rbnext->resval.rreal = atof(ptr1);
						}
						if (SDS_tblmesser(rbb,IC_TBLMESSER_MAKE,IEXP_ODNode->m_dbDrawing)!=RTNORM) NameExist=TRUE;
						sds_relrb(rbb);
					} while(NameExist);
					// Set linetype names in the (m_linetypes) variable.
					ltlst.m_linetypes.Empty();
					ltlst.m_linetypes=IEXP_szText;
				} else if (retvalue==IDCANCEL) return;
				for(ptr1=strtok((char *)((LPCTSTR)ltlst.m_linetypes),ResourceString(IDC_ICADEXPLORER___75, "," ));ptr1;ptr1=strtok(NULL,ResourceString(IDC_ICADEXPLORER___75, "," ))) {
					rbb=SDS_tblgetter(IEXP_EntityCol[IEXP_CurrentIndx].cType,ptr1,TRUE,IEXP_ODNode->m_dbDrawing);
					if (rbb) {
						if (ltlst.m_ret!=2) {
							Cnt=IEXP_LinetCnt;
							m_dataptr=NULL;
							// Insert a new row for the Linetype in the (CListCtrl).
							if (!LinetIns(&Cnt,0,IEXP_CurrentIndx,NULL,lvI,FALSE,rbb)) return;
						} else {
							IEXP_Findinfo.psz	=IEXP_szText;
							Cnt=m_ListCtl[IEXP_CurrentIndx].FindItem(&IEXP_Findinfo);
						}
						sds_relrb(rbb);
					}
				}
				break;
		case 2: // Insert Style.
				extern char *cmd_FontSavePath;
				fs2[0] = 0;
//				if (cmd_FontSavePath)
//					strcpy(fs2, cmd_FontSavePath);

				strcat(fs2, IEXP_DEFAULT_FONT);
				if (strrchr(fs2, IC_SLASH))
					{
					if (NULL != (fcp1 = strchr(fs2, ',')))
						db_astrncpy(&cmd_FontSavePath, fs2, fcp1 - fs2);
					else
						db_astrncpy(&cmd_FontSavePath, fs2, strlen(fs2) + 1);

					*(strrchr(cmd_FontSavePath, '\\') + 1) = 0;
					}

				strcpy(IEXP_szText, ResourceString(IDC_IEXP_NEW_STYLE, "NewStyle"));
				rindex = 0;
				do
					{
					NameExist = FALSE;
					sprintf(IEXP_szText, "%s%d"/*DNT*/, ResourceString(IDC_IEXP_NEW_STYLE, "NewStyle"), (++rindex));
					if ((rbb = sds_buildlist(RTDXF0, IEXP_STYLE,
											 2, IEXP_szText,
											 70, 0,
											 40, 0.0,
											 41, 1.0,
											 50, 0.0,
											 71, 0,
											 3, fs2,
											 4, ""/*DNT*/,	// EBATECH(CNBR) Bigfont
											 0)) == NULL)
						return;

					if (SDS_tblmesser(rbb, IC_TBLMESSER_MAKE, IEXP_ODNode->m_dbDrawing) != RTNORM)
						NameExist = TRUE;

					sds_relrb(rbb);
					}
				while(NameExist);

				rbb = SDS_tblgetter(IEXP_EntityCol[IEXP_CurrentIndx].cType,
									IEXP_szText,
									TRUE,
									IEXP_ODNode->m_dbDrawing);

				Cnt = IEXP_TStyleCnt; // ++IEXP_TStyleCnt;

				// Insert a new row for the Style in the (CListCtrl).
				if (!StyleIns(Cnt, 0, IEXP_CurrentIndx, NULL, lvI, rbb))
					return;

				sds_relrb(rbb);

				break;
		case 3: // Insert UCS.
				m_Explorer->m_MainWindow->m_bExplorerState=FALSE;
				m_Explorer->m_MainWindow->EndModalState();
				m_Explorer->ShowWindow(SW_HIDE);
				if(m_Explorer->m_regenMode == 0)
					cmd_regenall();

				SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)"._UCS"/*DNT*/);
				m_Explorer->m_MainWindow->m_bExpInsertState=TRUE;
				m_MessageType=typeidx;
				return;
		case 4: // Insert View.
				m_Explorer->m_MainWindow->m_bExplorerState=FALSE;
				m_Explorer->m_MainWindow->EndModalState();
				m_Explorer->ShowWindow(SW_HIDE);
				if(m_Explorer->m_regenMode == 0)
					cmd_regenall();

				SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)"._View"/*DNT*/);
				m_Explorer->m_MainWindow->m_bExpInsertState=TRUE;
				m_MessageType=typeidx;
				rindex=0;
				do {
				   NameExist=FALSE;
				   sprintf(IEXP_szText,"%s%d"/*DNT*/,ResourceString(IDC_IEXP_NEW_VIEW, "NewView"),(++rindex));
				   if ((rbb=sds_tblsearch(IEXP_VIEW,IEXP_szText,0))!=NULL) NameExist=TRUE;
				   sds_relrb(rbb);
				} while(NameExist);
				break;
		case 5: // Insert Block.
				if (IEXP_ODNode->m_dbDrawing!=SDS_CURDWG) {
					AfxMessageBox(ResourceString(IDC_ICADEXPLORER_CANNOT_INS_78, "Cannot insert a block in a non current drawing yet." ));
					return;
				}
				m_Explorer->m_MainWindow->m_bExplorerState=FALSE;
				m_Explorer->m_MainWindow->EndModalState();
				m_Explorer->ShowWindow(SW_HIDE);
				if(m_Explorer->m_regenMode == 0)
					cmd_regenall();

				rindex=0;
				do {
				   NameExist=FALSE;
				   sprintf(IEXP_szText,"%s%d"/*DNT*/,ResourceString(IDC_IEXP_NEW_BLOCK, "NewBlock"),(++rindex));
				   if ((rbb=sds_tblsearch(IEXP_BLOCK,IEXP_szText,0))!=NULL) NameExist=TRUE;
				   sds_relrb(rbb);
				} while(NameExist);

				char commandString[IC_ACADBUF];
				sprintf(commandString, "%s%s",	"._block;"/*DNT*/, IEXP_szText);
				SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)commandString);
				m_Explorer->m_MainWindow->m_bExpInsertState=TRUE;
				m_MessageType=typeidx;
				m_BlockInsCreate=FALSE;
				//if (index!=RTNORM) return;//compiler warning.  The only function that returns an RTXXXX is sds_relrb which always reutns RTNORM,	So this check makes no sense AM 4/16/98
				break;
		case 6: // Insert Dimstyle Copy the resbuf link from the selected dimstyle, Call tablemesser to insert it
				// to the dimstyle table and, get the number of rows.

				if (!(selcnt=m_ListCtl[IEXP_CurrentIndx].GetSelectedCount())) selcnt=0;
				if (!(selcnt=m_ListCtl[IEXP_CurrentIndx].GetItemCount())) break;
				for(index=selcnt;index!=-1;--index) {
					if (m_ListCtl[IEXP_CurrentIndx].GetItemState(index,LVIS_SELECTED|LVIS_FOCUSED)) break;
				}
				if (index == -1) index = 0;  //if none were selected, make a copy of the first one in the list.
				m_ListCtl[IEXP_CurrentIndx].GetItemText(index, 0, IEXP_szText, IC_ACADBUF);
				rbb=SDS_tblgetter(IEXP_EntityCol[IEXP_CurrentIndx].cType,IEXP_szText,TRUE,IEXP_ODNode->m_dbDrawing);
				newrbb=SDS_ResBufCopy(rbb);
				sds_relrb(rbb);
				if (ic_assoc(newrbb,2)) break;
				strcpy(IEXP_szText,ResourceString(IDC_IEXP_NEW_DIMSTY, "NewDimstyle"));
				do
				{
					NameExist=FALSE;
					sprintf(IEXP_szText,"%s%d",ResourceString(IDC_IEXP_NEW_DIMSTY, "NewDimstyle"),(++index));
					ic_rbassoc->resval.rstring=ic_realloc_char(ic_rbassoc->resval.rstring,(strlen(IEXP_szText)+1));
					strcpy (ic_rbassoc->resval.rstring,IEXP_szText);
					if (SDS_tblmesser(newrbb,IC_TBLMESSER_MAKE,IEXP_ODNode->m_dbDrawing)!=RTNORM) NameExist=TRUE;
				} while(NameExist);
				sds_relrb(newrbb);
				newrbb=SDS_tblgetter(IEXP_EntityCol[IEXP_CurrentIndx].cType,IEXP_szText,TRUE,IEXP_ODNode->m_dbDrawing);
				Cnt=m_ListCtl[IEXP_CurrentIndx].GetItemCount();
				m_dataptr=NULL; 			   // Set this variable to null so it can be reset in the insert layer.
				// Insert a new row for the Layer in the (CListCtrl).
				if (!DimStyleIns(Cnt,0,IEXP_CurrentIndx,NULL,lvI,newrbb))
				{
					sds_relrb(newrbb);
					return;
				}
				sds_relrb(newrbb);
				break;
	}
	if (typeidx!=5 || (typeidx==5 && !m_BlockState)) m_ListCtl[IEXP_CurrentIndx].SetFocus();
	if (typeidx!=5 || (typeidx==5 && !m_BlockState)) m_ListCtl[IEXP_CurrentIndx].EditLabel(Cnt);
}


void *CopytabItem(const char *name,HGLOBAL hGlobal,long *memsize,long *offptr) {
	void		  *memptr;
	struct resbuf *rbb=NULL;
	short		   linkcnt=0;

	rbb=SDS_tblgetter(IEXP_EntityCol[IEXP_CurrentIndx].cType,name,TRUE,IEXP_ODNode->m_dbDrawing);
	if (ic_assoc(rbb,-1)) goto err;
	*memsize+=sizeof(long);
	GlobalUnlock(hGlobal);
	if (((hGlobal=GlobalReAlloc(hGlobal,*memsize,GMEM_MOVEABLE|GMEM_ZEROINIT)))==NULL) goto err;
	memptr=GlobalLock(hGlobal);
	memcpy((((char *)memptr)+*offptr),(void *)&ic_rbassoc->resval.rlname[0],sizeof(ic_rbassoc->resval.rlname[0])); *offptr+=sizeof(ic_rbassoc->resval.rlname[0]);
	sds_relrb(rbb);
	return(memptr);
err :
	if (rbb) sds_relrb(rbb);
	return(NULL);
}

///////////////////////////////////////////////////////////////////////
// This function pastes the item(s) from the clipboard.
//
// the record format would be : [AAAA][BBBB][CCCC][XXX...]
//
// A = This is a long containing the pointer of the address to the main window.
// B = This is a long containing the total number of records.
// C = This is a long containing the type of record.
// X = The record data, this data varies on each type of record.
//
// Nothing is returnd.
//
void IEXP_CdetailView::PasteItem() {
	long		   typeflg,
				   cntindex,
				   offptr=0,
				   totalcnt;
	HANDLE		   clipdata;
	struct resbuf *rbb=NULL,
				  *rb,
				  *rbn;
   	LV_ITEM 	   lvI;
	short		   Cnt,
				   //rindex,
				   retvalue;
	IEXP_MessItem  dMessDlg;
	void		  *memptr;
	sds_name	   ename;

	db_handitem   *handItem,
				  *oldhandItem;
	db_drawing	  *dbDrawing;
	int 		   i1,
				   i2,
				   i3;
	sds_real	   r1,
				   r2,
				   r3,
				   r4,
				   r5,
				  *r6;
	char		   name[IC_ACADBUF];
	sds_point	   pt1,
				   pt2,
				   pt3;
	BOOL		   bDeleted = FALSE;
	long clipprocid, procid;

	if (m_Explorer->m_editmode)
		m_ListCtl[IEXP_CurrentIndx].EndLabelEditing();
	if (!OpenClipboard()) return;
	if (!(clipdata=GetClipboardData(IEXP_Clipboard))) goto out;

	memptr=GlobalLock(clipdata);

	dMessDlg.m_BtnTwo  =ResourceString(IDC_ICADEXPLORER_REPLACE_80, "Replace" );
	dMessDlg.m_Continue=ResourceString(IDC_ICADEXPLORER_SKIP_81, "Skip" );
	dMessDlg.m_typeflag=1;

	lvI.mask	  = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
	lvI.state	  = 0;
	lvI.stateMask = 0;
	lvI.iImage	  = -1;

	//*** Make sure we are pasting to the same instance of the app.
	if(NULL==memptr)
	{
		goto out;
	}
	if((*(long*)memptr)!=(long)m_Explorer->m_MainWindow)
	{
		sds_alert(ResourceString(IDC_ICADEXPLORER_ITEMS_IN_T_82, "Items in the clipboard may only be pasted into the same instance\nof the application that they were copied from." ));
		goto out;
	}
	offptr+=sizeof(long); //*** Step over the m_MainWindow
	totalcnt=(*(long *)(((char *)memptr)+offptr)); offptr+=sizeof(totalcnt);
	dbDrawing=(db_drawing  *) (*(long *)(((char *)memptr)+offptr)); offptr+=sizeof(ename[1]);
	clipprocid =(*(long *)(((char *)memptr)+offptr)); offptr+=sizeof(clipprocid);
	procid = GetCurrentProcessId();
	if (procid != clipprocid)
	{
		sds_alert(ResourceString(IDC_ICADEXPLORER_ITEMS_IN_T_82, "Items in the clipboard may only be pasted into the same instance\nof the application that they were copied from." ));
		goto out;
	}

	for(rbn=NULL,cntindex=0;cntindex<totalcnt;++cntindex,rbn=NULL) {
		// ename[0]=Entity, ename[1]=Database handle.
		handItem =(db_handitem *) (*(long *)(((char *)memptr)+offptr)); offptr+=sizeof(ename[0]);
		ename[0]=(long)handItem;
		ename[1]=(long)dbDrawing;
		typeflg=handItem->ret_type();
		handItem->get_2(name,IC_ACADBUF-1);
		switch(typeflg) {
			case DB_LAYERTABREC:
				if (oldhandItem=IEXP_ODNode->m_dbDrawing->findtabrec(DB_LAYERTAB,name,0))
				{
					if (oldhandItem->ret_deleted())
					{
						//if the found item was deleted, set it to undeleted, then copy over data
						oldhandItem->set_deleted(0);
						bDeleted = TRUE;
					}
					else
					{
						//Ask user whether to replace, etc.
						LOADSTRINGS_2(IDC_ICADEXPLORER_LAYER___84)
						sprintf(IEXP_szTextA, LOADEDSTRING, name, name);
						dMessDlg.m_txtmes=IEXP_szTextA;
						sprintf(IEXP_szTextA,ResourceString(IDC_ICADEXPLORER_REPLACES_T_88, "Replaces the attributes of layer %s." ),name);
						dMessDlg.m_txtcnt=IEXP_szTextA;
						sprintf(IEXP_szTextA,ResourceString(IDC_ICADEXPLORER_DOES_NOT_R_89, "Does not replace layer %s with new attributes." ),name);
						dMessDlg.m_txtchng=IEXP_szTextA;
						sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER_CANCELS_TH_90, "Cancels the paste layer operation." ));
						dMessDlg.m_txtcancel=IEXP_szText;
						sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER_PASTE__S_91, "Paste %s" ),name);
						dMessDlg.m_title=IEXP_szText;
						if ((retvalue=dMessDlg.DoModal())==EXP_DEL_CANCEL) goto out;
						if (retvalue==EXP_DEL_CHANGE) continue;
					}

					handItem->get_6(IEXP_szTextA,IC_ACADBUF-1);
					handItem->get_62(&i1);
					handItem->get_70(&i2);

					//TODO : Ensure that Ltype is in new drawing too.

					if ((rbb=sds_buildlist(RTDXF0,IEXP_LAYER,2,name,62,i1,6,IEXP_szTextA,70,i2,0))==NULL) return;
					if (SDS_tblmesser(rbb,IC_TBLMESSER_MODIFY,IEXP_ODNode->m_dbDrawing)!=RTNORM) {sds_relrb(rbb); rbb = NULL; return;}
					if (bDeleted)
					{
						Cnt=m_ListCtl[IEXP_LATAB_IDX].GetItemCount();
						LayerIns(Cnt,0,IEXP_LATAB_IDX,NULL,lvI,rbb);
					}
					sds_relrb(rbb); rbb = NULL;
					if (oldhandItem->ret_deleted())
						oldhandItem->set_deleted(0);
				}
				else
				{
					if (!cmd_ItemInsert(dbDrawing,IEXP_ODNode->m_dbDrawing,handItem,NULL)) return;
					rbb=SDS_tblgetter(IEXP_EntityCol[IEXP_LATAB_IDX].cType,name,TRUE,IEXP_ODNode->m_dbDrawing);
					Cnt=m_ListCtl[IEXP_LATAB_IDX].GetItemCount();
					if (!LayerIns(Cnt,0,IEXP_LATAB_IDX,NULL,lvI,rbb)) goto out;
					sds_relrb(rbb); rbb = NULL;
				}
				break;
			case DB_LTYPETABREC:
				if (oldhandItem=IEXP_ODNode->m_dbDrawing->findtabrec(DB_LTYPETAB,name,0))
				{
					if (oldhandItem->ret_deleted())
					{
						//if the found item was deleted, set it to undeleted, then copy over data
						oldhandItem->set_deleted(0);
						bDeleted = TRUE;
					}
					else
					{
						//Ask user whether to replace, etc.
					sprintf(IEXP_szTextA,ResourceString(IDC_ICADEXPLORER_LINETYPE___92, "Linetype \"%s\" already exists in the current drawing. You can replace the attributes of Linetype \"%s\" with the new attributes by selecting the Replace button, or skip by selecting the Skip button." ),name,name);
					dMessDlg.m_txtmes=IEXP_szTextA;
					sprintf(IEXP_szTextA,ResourceString(IDC_ICADEXPLORER_REPLACES_T_95, "Replaces the attributes of linetype %s." ),name);
					dMessDlg.m_txtcnt=IEXP_szTextA;
					sprintf(IEXP_szTextA,ResourceString(IDC_ICADEXPLORER_DOES_NOT_R_96, "Does not replace linetype %s with new attributes." ),name);
					dMessDlg.m_txtchng=IEXP_szTextA;
					sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER_CANCELS_TH_97, "Cancels the paste linetype operation." ));
					dMessDlg.m_txtcancel=IEXP_szText;
					sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER_PASTE__S_91, "Paste %s" ),name);
						dMessDlg.m_title=IEXP_szText;
						if ((retvalue=dMessDlg.DoModal())==EXP_DEL_CANCEL) goto out;
						if (retvalue==EXP_DEL_CHANGE) continue;
					}

					handItem->get_3(IEXP_szTextA,IC_ACADBUF-1);  //description
					handItem->get_70(&i2);
					handItem->get_73(&i1);
					handItem->get_40(&r1);

					r6= new sds_real [i1];
					memset(r6,0, sizeof(sds_real)*i1);
					handItem->get_49(r6,i1);

					if ((rbb=sds_buildlist(RTDXF0,IEXP_LTYPE,2,name,3,IEXP_szTextA,73,i1,70,i2,40,r1,0))==NULL) return;
					for(rb=rbb;rb->rbnext;rb=rb->rbnext);
					for(i3=0;i3<i1;++i3) { rb->rbnext=sds_newrb(49); rb->rbnext->resval.rreal=r6[i3]; rb=rb->rbnext;}
					IC_FREE(r6);

					if (SDS_tblmesser(rbb,IC_TBLMESSER_MODIFY,IEXP_ODNode->m_dbDrawing)!=RTNORM) {sds_relrb(rbb); rbb = NULL; return;}
					if (bDeleted)
					{
						Cnt=m_ListCtl[IEXP_LTTAB_IDX].GetItemCount();
						LinetIns(&Cnt,0,IEXP_LTTAB_IDX,NULL,lvI,FALSE,rbb);
					}
					sds_relrb(rbb); rbb = NULL;
					if (oldhandItem->ret_deleted())
						oldhandItem->set_deleted(0);
			   }
				else
				{
					if (!cmd_ItemInsert(dbDrawing,IEXP_ODNode->m_dbDrawing,handItem,NULL)) return;
					rbb=SDS_tblgetter(IEXP_EntityCol[IEXP_LTTAB_IDX].cType,name,TRUE,IEXP_ODNode->m_dbDrawing);
					Cnt=m_ListCtl[IEXP_LTTAB_IDX].GetItemCount();
					if (!LinetIns(&Cnt,0,IEXP_LTTAB_IDX,NULL,lvI,FALSE,rbb)) goto out;
					sds_relrb(rbb); rbb = NULL;
				}
				break;
			case DB_STYLETABREC:
				if (oldhandItem=IEXP_ODNode->m_dbDrawing->findtabrec(DB_STYLETAB,name,0))
				{
					if (oldhandItem->ret_deleted())
					{
						//if the found item was deleted, set it to undeleted, then copy over data
						oldhandItem->set_deleted(0);
						bDeleted = TRUE;
					}
					else
					{
						//Ask user whether to replace, etc.
						LOADSTRINGS_2(IDC_ICADEXPLORER__ALREADY_E_99)
						sprintf(IEXP_szTextA, LOADEDSTRING, name, name);
						dMessDlg.m_txtmes=IEXP_szTextA;

						sprintf(IEXP_szTextA,ResourceString(IDC_ICADEXPLORER_REPLACES__100, "Replaces the attributes of text style %s." ),name);
						dMessDlg.m_txtcnt=IEXP_szTextA;
						sprintf(IEXP_szTextA,ResourceString(IDC_ICADEXPLORER_DOES_NOT__101, "Does not replace text style %s with new attributes." ),name);
						dMessDlg.m_txtchng=IEXP_szTextA;
						sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER_CANCELS_T_102, "Cancels the paste text style operation." ));
						dMessDlg.m_txtcancel=IEXP_szText;
						sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER_PASTE__S_91, "Paste %s" ),name);
						dMessDlg.m_title=IEXP_szText;

						if ((retvalue=dMessDlg.DoModal())==EXP_DEL_CANCEL) goto out;
						if (retvalue==EXP_DEL_CHANGE) continue;
					}

					handItem->get_3(IEXP_szTextA,IC_ACADBUF-1);
					handItem->get_4(IEXP_szText,IC_ACADBUF-1); // EBATECH(CNBR)
					handItem->get_70(&i2);
					handItem->get_71(&i1);
					handItem->get_40(&r1);
					handItem->get_41(&r2);
					handItem->get_50(&r3);

					if ((rbb=sds_buildlist(
						RTDXF0,IEXP_STYLE,
						2,name,
						70,i2,
						40,r1,
						41,r2,
						50,r3,
						71,i1,
						3,IEXP_szTextA,
						4,IEXP_szText,0))==NULL) return; // EBATECH(CNBR)
					if (SDS_tblmesser(rbb,IC_TBLMESSER_MODIFY,IEXP_ODNode->m_dbDrawing)!=RTNORM) {sds_relrb(rbb); rbb = NULL;return;}
					// EBATECH(CNBR) ]- Now, The item has no TrueType Data.
					//if (bDeleted)
					//{
					//	Cnt=m_ListCtl[IEXP_TSTAB_IDX].GetItemCount();
					//	StyleIns(Cnt,0,IEXP_TSTAB_IDX,NULL,lvI,rbb);
					//}
					// EBATECH(CNBR) ]-
					sds_relrb(rbb); rbb = NULL;
					if (oldhandItem->ret_deleted())
						oldhandItem->set_deleted(0);
					// EBATECH(CNBR) ]- TrueType
					db_styletabrec *tabrec = (db_styletabrec *) handItem;
					long styleNumber;
					if( tabrec->hasTrueTypeFont()){
						tabrec->getFontFamily( IEXP_szTextA );
						tabrec->getFontStyleNumber( &styleNumber );
						if( tabrec = (db_styletabrec*)IEXP_ODNode->m_dbDrawing->findtabrec(DB_STYLETAB,name,1)){
							tabrec->setFontFamily( IEXP_szTextA, (db_drawing *)IEXP_ODNode->m_dbDrawing);
							tabrec->setFontStyleNumber(IEXP_szTextA, styleNumber, (db_drawing *)IEXP_ODNode->m_dbDrawing);
						}
					}else{
						if( tabrec = (db_styletabrec*)IEXP_ODNode->m_dbDrawing->findtabrec(DB_STYLETAB,name,1)){
							tabrec->ensureNoTrueTypeEED((db_drawing *)IEXP_ODNode->m_dbDrawing);
						}
					}
					rbb=SDS_tblgetter(IEXP_EntityCol[IEXP_TSTAB_IDX].cType,name,TRUE,IEXP_ODNode->m_dbDrawing);
					if (bDeleted)
					{
						Cnt=m_ListCtl[IEXP_TSTAB_IDX].GetItemCount();
						StyleIns(Cnt,0,IEXP_TSTAB_IDX,NULL,lvI,rbb);
					}
					sds_relrb(rbb); rbb = NULL;
					// EBATECH(CNBR) ]-
				}
				else
				{
					if (!cmd_ItemInsert(dbDrawing,IEXP_ODNode->m_dbDrawing,handItem,NULL)) return;
					// EBATECH(CNBR) -[ TrueType
					db_styletabrec *tabrec = (db_styletabrec *) handItem;
					long styleNumber;
					if( tabrec->hasTrueTypeFont()){
						tabrec->getFontFamily( IEXP_szTextA );
						tabrec->getFontStyleNumber( &styleNumber );
						if( tabrec = (db_styletabrec*)IEXP_ODNode->m_dbDrawing->findtabrec(DB_STYLETAB,name,1)){
							tabrec->setFontFamily( IEXP_szTextA, (db_drawing *)IEXP_ODNode->m_dbDrawing);
							tabrec->setFontStyleNumber(IEXP_szTextA, styleNumber, (db_drawing *)IEXP_ODNode->m_dbDrawing);
						}
					}else{
						if( tabrec = (db_styletabrec*)IEXP_ODNode->m_dbDrawing->findtabrec(DB_STYLETAB,name,1)){
							tabrec->ensureNoTrueTypeEED((db_drawing *)IEXP_ODNode->m_dbDrawing);
						}
					}
					// EBATECH(CNBR) ]-
					rbb=SDS_tblgetter(IEXP_EntityCol[IEXP_TSTAB_IDX].cType,name,TRUE,IEXP_ODNode->m_dbDrawing);
					Cnt=m_ListCtl[IEXP_TSTAB_IDX].GetItemCount();
					if (!StyleIns(Cnt,0,IEXP_TSTAB_IDX,NULL,lvI,rbb)) goto out;
					sds_relrb(rbb); rbb = NULL;
				}
				break;
			case DB_UCSTABREC:
				if (oldhandItem=IEXP_ODNode->m_dbDrawing->findtabrec(DB_UCSTAB,name,0))
				{
					if (oldhandItem->ret_deleted())
					{
						//if the found item was deleted, set it to undeleted, then copy over data
						oldhandItem->set_deleted(0);
						bDeleted = TRUE;
					}
					else
					{
						//Ask user whether to replace, etc.
						LOADSTRINGS_2(IDC_ICADEXPLORER__ALREADY__104)
						sprintf(IEXP_szTextA, LOADEDSTRING, name, name);
						dMessDlg.m_txtmes=IEXP_szTextA;

						sprintf(IEXP_szTextA,ResourceString(IDC_ICADEXPLORER_REPLACES__106, "Replaces the attributes of UCS %s." ),name);
						dMessDlg.m_txtcnt=IEXP_szTextA;
						sprintf(IEXP_szTextA,ResourceString(IDC_ICADEXPLORER_DOES_NOT__107, "Does not replace UCS %s with new attributes." ),name);
						dMessDlg.m_txtchng=IEXP_szTextA;
						sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER_CANCELS_T_108, "Cancels the paste UCS operation." ));
						dMessDlg.m_txtcancel=IEXP_szText;
						sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER_PASTE__S_91, "Paste %s" ),name);
						dMessDlg.m_title=IEXP_szText;

						if ((retvalue=dMessDlg.DoModal())==EXP_DEL_CANCEL) goto out;
						if (retvalue==EXP_DEL_CHANGE) continue;
					}

					handItem->get_70(&i2);
					handItem->get_10(pt1);
					handItem->get_11(pt2);
					handItem->get_12(pt3);

					if ((rbb=sds_buildlist(RTDXF0,IEXP_UCS,2,name,70,i2,10,pt1,11,pt2,12,pt3,0))==NULL) return;
					if (SDS_tblmesser(rbb,IC_TBLMESSER_MODIFY,IEXP_ODNode->m_dbDrawing)!=RTNORM) {sds_relrb(rbb); rbb = NULL;return;}
					if (bDeleted)
					{
						Cnt=m_ListCtl[IEXP_CSTAB_IDX].GetItemCount();
						UcsIns(Cnt,0,IEXP_CSTAB_IDX,NULL,lvI,FALSE,rbb);
					}
					sds_relrb(rbb); rbb = NULL;
					if (oldhandItem->ret_deleted())
						oldhandItem->set_deleted(0);
				}
				else
				{
					if (!cmd_ItemInsert(dbDrawing,IEXP_ODNode->m_dbDrawing,handItem,NULL)) return;
					rbb=SDS_tblgetter(IEXP_EntityCol[IEXP_CSTAB_IDX].cType,name,TRUE,IEXP_ODNode->m_dbDrawing);
					if (dbDrawing==SDS_CURDWG)
					{
						for(rbn=rbb;rbn && rbn->restype!=70;rbn=rbn->rbnext);
						if (!rbn) goto out;
						if (rbn->resval.rint&IEXP_DEL_FLG) {
							rbn->resval.rint^=IEXP_DEL_FLG;
							if (SDS_tblmesser(rbb,IC_TBLMESSER_DELETE,IEXP_ODNode->m_dbDrawing)!=RTNORM) goto out;
						}
					}
					Cnt=m_ListCtl[IEXP_CSTAB_IDX].GetItemCount();
					if (!UcsIns(Cnt,0,IEXP_CSTAB_IDX,NULL,lvI,FALSE,rbb)) goto out;
					sds_relrb(rbb); rbb = NULL;
				}
				break;
			case DB_VIEWTABREC:
				if (oldhandItem=IEXP_ODNode->m_dbDrawing->findtabrec(DB_VIEWTAB,name,0))
				{
					if (oldhandItem->ret_deleted())
					{
						//if the found item was deleted, set it to undeleted, then copy over data
						oldhandItem->set_deleted(0);
						bDeleted = TRUE;
					}
					else
					{
						//Ask user whether to replace, etc.
						LOADSTRINGS_2(IDC_ICADEXPLORER__ALREADY__110)
						sprintf(IEXP_szTextA, LOADEDSTRING, name, name);
						dMessDlg.m_txtmes=IEXP_szTextA;

						sprintf(IEXP_szTextA,ResourceString(IDC_ICADEXPLORER_REPLACES__112, "Replaces the attributes of view %s." ),name);
						dMessDlg.m_txtcnt=IEXP_szTextA;
						sprintf(IEXP_szTextA,ResourceString(IDC_ICADEXPLORER_DOES_NOT__113, "Does not replace view %s with new attributes." ),name);
						dMessDlg.m_txtchng=IEXP_szTextA;
						sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER_CANCELS_T_114, "Cancels the paste view operation." ));
						dMessDlg.m_txtcancel=IEXP_szText;
						sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER_PASTE__S_91, "Paste %s" ),name);
						dMessDlg.m_title=IEXP_szText;
						if ((retvalue=dMessDlg.DoModal())==EXP_DEL_CANCEL) goto out;
						if (retvalue==EXP_DEL_CHANGE) continue;
					}

					handItem->get_6(IEXP_szTextA,IC_ACADBUF-1);
					handItem->get_70(&i2);
					handItem->get_40(&r1);
					handItem->get_41(&r2);
					handItem->get_10(pt1);
					handItem->get_11(pt2);
					handItem->get_42(&r3);
					handItem->get_43(&r4);
					handItem->get_44(&r5);
					handItem->get_71(&i3);

					if ((rbb=sds_buildlist(RTDXF0,IEXP_VIEW,2,name,70,i2,40,r1,41,r2,10,pt1,11,pt2,12,pt3,42,r3,43,r4,44,r5,71,i3,0))==NULL) return;
					if (SDS_tblmesser(rbb,IC_TBLMESSER_MODIFY,IEXP_ODNode->m_dbDrawing)!=RTNORM) {sds_relrb(rbb); rbb = NULL;return;}
					if (bDeleted)
					{
						Cnt=m_ListCtl[IEXP_VWTAB_IDX].GetItemCount();
						ViewIns(Cnt,0,IEXP_VWTAB_IDX,NULL,lvI,rbb);
					}
					sds_relrb(rbb); rbb = NULL;
					if (oldhandItem->ret_deleted())
						oldhandItem->set_deleted(0);
				}
				else
				{
					if (!cmd_ItemInsert(dbDrawing,IEXP_ODNode->m_dbDrawing,handItem,NULL)) return;
					rbb=SDS_tblgetter(IEXP_EntityCol[IEXP_VWTAB_IDX].cType,name,TRUE,IEXP_ODNode->m_dbDrawing);
					Cnt=m_ListCtl[IEXP_VWTAB_IDX].GetItemCount();
					if (!ViewIns(Cnt,0,IEXP_VWTAB_IDX,NULL,lvI,rbb)) goto out;
					sds_relrb(rbb); rbb = NULL;
				}
				break;
			case DB_BLOCKTABREC:
				/*DG - 6.5.2002*/// we will use this pointer later in IEXP_CdetailView::Insertblock
				handItem->get_2(&m_pBlockName);
				if (oldhandItem=IEXP_ODNode->m_dbDrawing->findtabrec(DB_BLOCKTAB,name,0))
				{
					if (oldhandItem->ret_deleted())
					{
						//if the found item was deleted, set it to undeleted, then copy over data
						oldhandItem->set_deleted(0);
						bDeleted = TRUE;
					}
					else
					{
						//Ask user whether to replace, etc.
						sprintf(IEXP_szTextA,ResourceString(IDC_ICADEXPLORER__ALREADY__116, "Block %s already exists in the current drawing. You can replace the block %s with the new copy by selecting the (replace) button, or skip by selecting the (skip) button."),name,name);
						dMessDlg.m_txtmes=IEXP_szTextA;
						sprintf(IEXP_szTextA,ResourceString(IDC_ICADEXPLORER_REPLACES__119, "Replaces the block %s."),name);
						dMessDlg.m_txtcnt=IEXP_szTextA;
						sprintf(IEXP_szTextA,ResourceString(IDC_ICADEXPLORER_DOES_NOT__120, "Does not replace block %s with new block."),name);
						dMessDlg.m_txtchng=IEXP_szTextA;
						sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER_CANCELS_T_121, "Cancels the paste block operation."));
						dMessDlg.m_txtcancel=IEXP_szText;
						sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER_PASTE__S_91, "Paste %s"),name);
						dMessDlg.m_title=IEXP_szText;
						if ((retvalue=dMessDlg.DoModal())==EXP_DEL_CANCEL) goto out;
						if (retvalue==EXP_DEL_CHANGE) continue;
					}

					if (handItem->ret_deleted())
					{
						dbDrawing->delhanditem(NULL,handItem,2);
						cmd_ItemInsert(dbDrawing,IEXP_ODNode->m_dbDrawing,handItem,NULL);
						dbDrawing->delhanditem(NULL,handItem,2);
					} else
						cmd_ItemInsert(dbDrawing,IEXP_ODNode->m_dbDrawing,handItem,NULL);

					if (bDeleted)
					{
						Cnt=m_ListCtl[IEXP_BNTAB_IDX].GetItemCount();
						rbb=SDS_tblgetter(IEXP_EntityCol[IEXP_BNTAB_IDX].cType,name,TRUE,IEXP_ODNode->m_dbDrawing);
						BlockIns(Cnt,Cnt,IEXP_BNTAB_IDX,NULL,lvI,rbb);
						sds_relrb(rbb); rbb = NULL;
					}
					if (oldhandItem->ret_deleted())
						oldhandItem->set_deleted(0);
				}
				else
				{
					if (!cmd_ItemInsert(dbDrawing,IEXP_ODNode->m_dbDrawing,handItem,NULL)) return;
					if ((rbb=SDS_tblgetter(IEXP_EntityCol[IEXP_BNTAB_IDX].cType,name,TRUE,IEXP_ODNode->m_dbDrawing))==NULL) goto out;
					Cnt=m_ListCtl[IEXP_BNTAB_IDX].GetItemCount();
					if (!BlockIns(Cnt,Cnt,IEXP_BNTAB_IDX,NULL,lvI,rbb)) goto out;
					sds_relrb(rbb); rbb = NULL;

				}
				break;
			case DB_DIMSTYLETABREC:
				if (oldhandItem=IEXP_ODNode->m_dbDrawing->findtabrec(DB_DIMSTYLETAB,name,0))
				{
					if (oldhandItem->ret_deleted())
					{
						//if the found item was deleted, set it to undeleted, then copy over data
						oldhandItem->set_deleted(0);
						bDeleted = TRUE;
					}
					else
					{
						//Ask user whether to replace, etc.
						sprintf(IEXP_szTextA,ResourceString(IDC_ICADEXPLORER_13, "Dimension Style \"%s\" already exists in the current drawing. You can replace the attributes of Dimension Style \"%s\" with the new attributes by selecting the Replace button, or skip by selecting the Skip button."),name,name);
						dMessDlg.m_txtmes=IEXP_szTextA;

						sprintf(IEXP_szTextA,ResourceString(IDC_ICADEXPLORER_14, "Replaces the attributes of dimension style %s."),name);
						dMessDlg.m_txtcnt=IEXP_szTextA;
						sprintf(IEXP_szTextA,ResourceString(IDC_ICADEXPLORER_15, "Does not replace dimension style %s with new attributes."),name);
						dMessDlg.m_txtchng=IEXP_szTextA;
						sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER_16, "Cancels the paste dimension style operation."));
						dMessDlg.m_txtcancel=IEXP_szText;
						sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER_PASTE__S_91, "Paste %s"),name);
						dMessDlg.m_title=IEXP_szText;

						if ((retvalue=dMessDlg.DoModal())==EXP_DEL_CANCEL) goto out;
						if (retvalue==EXP_DEL_CHANGE) continue;
					}

					cmd_ItemInsert(dbDrawing,IEXP_ODNode->m_dbDrawing,handItem,NULL);

					if (bDeleted)
					{
						Cnt=m_ListCtl[IEXP_DSTAB_IDX].GetItemCount();
						rbb=SDS_tblgetter(IEXP_EntityCol[IEXP_DSTAB_IDX].cType,name,TRUE,IEXP_ODNode->m_dbDrawing);
						DimStyleIns(Cnt,0,IEXP_DSTAB_IDX,NULL,lvI,rbb);
						sds_relrb(rbb); rbb = NULL;
					}
					if (oldhandItem->ret_deleted())
						oldhandItem->set_deleted(0);
				}
				else
				{
					if (!cmd_ItemInsert(dbDrawing,IEXP_ODNode->m_dbDrawing,handItem,NULL)) return;
					rbb=SDS_tblgetter(IEXP_EntityCol[IEXP_DSTAB_IDX].cType,name,TRUE,IEXP_ODNode->m_dbDrawing);
					Cnt=m_ListCtl[IEXP_DSTAB_IDX].GetItemCount();
					if (!DimStyleIns(Cnt,0,IEXP_DSTAB_IDX,NULL,lvI,rbb)) goto out;
					sds_relrb(rbb); rbb = NULL;
				}
				break;

		}
	}

out :
	UpdateDataList();
	m_ListCtl[IEXP_CurrentIndx].EnableWindow(TRUE);
	m_ListCtl[IEXP_CurrentIndx].ShowWindow(SW_SHOW);
	GlobalUnlock(clipdata);
	CloseClipboard();
	if (rbb)
		sds_relrb(rbb);
	return;
}

///////////////////////////////////////////////////////////////////////
// This function copies the item(s) to the clipboard.
//
// the record format would be : [AAAA][BBBB][CCCC][XXX...]
//
// A = This is a long containing the pointer of the address to the main window.
// B = This is a long containing the total number of records.
// C = This is a long containing the type of record.
// X = The record data, this data varies on each type of record.
//
// Nothing is returnd.
//
void IEXP_CdetailView::CopyCutItem(BOOL type) {
	HGLOBAL   hGlobal;
	long	  totalCnt,
			  fi1;
	void	 *memptr;
	long	  typeflg,
			  memsize,
			  offptr=0;
	struct resbuf *rbb=NULL;
	long procid;

	if (IEXP_CurrentIndx==7) return;

	if (m_Explorer->m_editmode)
		m_ListCtl[IEXP_CurrentIndx].EndLabelEditing();

	// Get the total number of records.
	m_ItemIndex=(m_ListCtl[IEXP_CurrentIndx].GetItemCount()-1);
	// Get the number of records selected.
	totalCnt=(m_Windowsideflag)?m_ItemIndex:m_ListCtl[IEXP_CurrentIndx].GetSelectedCount();
	if (!totalCnt) return;
	// Decrement the number of selected records by one, if one is a permanent record type.
	switch(IEXP_EntityCol[IEXP_CurrentIndx].IType) {
		case IEXP_BNTAB_IDX:
		case IEXP_VWTAB_IDX:
			for(fi1=m_ItemIndex;fi1!=-1;--fi1) {
				if (!m_ListCtl[IEXP_CurrentIndx].GetItemState(fi1,LVIS_SELECTED)) continue;
				IEXP_ItemDefPtr=(IEXP_ItemDef *)m_ListCtl[IEXP_CurrentIndx].GetItemData(fi1);
			}
			break;
		case IEXP_LATAB_IDX:
			for(fi1=m_ItemIndex;fi1!=-1;--fi1) {
				if (!m_ListCtl[IEXP_CurrentIndx].GetItemState(fi1,LVIS_SELECTED)) continue;
				IEXP_ItemDefPtr=(IEXP_ItemDef *)m_ListCtl[IEXP_CurrentIndx].GetItemData(fi1);
				if (strsame(IEXP_ItemDefPtr->name(),ResourceString(IDC_ICADEXPLORER_0_18, "0" ))) {
					--totalCnt;
					AfxMessageBox(ResourceString(IDC_ICADEXPLORER_CANNOT_CO_122, "Cannot copy or cut layer 0" )); continue;// }
				}
				if (strchr(IEXP_ItemDefPtr->name(),IEXP_VER_BAR)) {
					--totalCnt;
					AfxMessageBox(ResourceString(IDC_ICADEXPLORER_CANNOT_CO_123, "Cannot Copy/Cut an XREF layer.")); continue;
				}
			}
			break;
		case IEXP_LTTAB_IDX:
			if (m_Windowsideflag) totalCnt -= 2;  //Tricky Bug - 46083.  If m_Windowsideflag - means, focus is
			//on the left pane - so copy all items on right pane.  there are 3 forbidden items for ltypes, only one
			//will be selected at most.  So reduce totalCnt correctly, we cannot copy 3 of them.
			for(fi1=m_ItemIndex;fi1!=-1;--fi1) {
				if (!m_ListCtl[IEXP_CurrentIndx].GetItemState(fi1,LVIS_SELECTED)) continue;
				IEXP_ItemDefPtr=(IEXP_ItemDef *)m_ListCtl[IEXP_CurrentIndx].GetItemData(fi1);
				if ((strisame(IEXP_ItemDefPtr->name(),IEXP_CONTINUOUS)) ||
					(strisame(IEXP_ItemDefPtr->name(),IEXP_BYBLOCK)) || (strisame(IEXP_ItemDefPtr->name(),IEXP_BYLAYER))) {
					if (!m_Windowsideflag)
						--totalCnt;
					AfxMessageBox(ResourceString(IDC_ICADEXPLORER_CANNOT_CO_124, "Cannot copy Linetypes BYBLOCK/BYLAYER/CONTINUOUS")); continue;
				}
				if (strchr(IEXP_ItemDefPtr->name(),IEXP_VER_BAR)) {
					--totalCnt;
					AfxMessageBox(ResourceString(IDC_ICADEXPLORER_CANNOT_CO_125, "Cannot Copy/Cut an XREF linetype.")); continue;
				}
			}
			break;
		case IEXP_CSTAB_IDX:
			for(fi1=m_ItemIndex;fi1!=-1;--fi1) {
				if (!m_ListCtl[IEXP_CurrentIndx].GetItemState(fi1,LVIS_SELECTED)) continue;
				IEXP_ItemDefPtr=(IEXP_ItemDef *)m_ListCtl[IEXP_CurrentIndx].GetItemData(fi1);
				if (strsame(IEXP_ItemDefPtr->name(),IEXP_WORLD)) {
					--totalCnt;
					AfxMessageBox(ResourceString(IDC_ICADEXPLORER_CANNOT_CO_126, "Cannot copy UCS World")); continue;
				}
			}
			break;
		case IEXP_TSTAB_IDX:
			for(fi1=m_ItemIndex;fi1!=-1;--fi1) {
				if (!m_ListCtl[IEXP_CurrentIndx].GetItemState(fi1,LVIS_SELECTED)) continue;
				IEXP_ItemDefPtr=(IEXP_ItemDef *)m_ListCtl[IEXP_CurrentIndx].GetItemData(fi1);
				if (strchr(IEXP_ItemDefPtr->name(),IEXP_VER_BAR)) {
					--totalCnt;
					AfxMessageBox(ResourceString(IDC_ICADEXPLORER_CANNOT_CO_127, "Cannot Copy/Cut an XREF text style.")); continue;
				}
				break;
			}
			break;
		case IEXP_DSTAB_IDX:
			for(fi1=m_ItemIndex;fi1!=-1;--fi1) {
				if (!m_ListCtl[IEXP_CurrentIndx].GetItemState(fi1,LVIS_SELECTED)) continue;
				IEXP_ItemDefPtr=(IEXP_ItemDef *)m_ListCtl[IEXP_CurrentIndx].GetItemData(fi1);
				if (strchr(IEXP_ItemDefPtr->name(),IEXP_VER_BAR)) {
					--totalCnt;
					// Bugzilla No. 78088; 29/04/2002
					AfxMessageBox(ResourceString(IDC_ICADEXPLORER_CANNOT_CO_128, "Cannot Copy/Cut an XREF dimension style.")); continue;
				}
				break;
			}
			break;
	}
	if (!totalCnt) return;
	if (!IEXP_ItemDefPtr) return;
	// Allocate memory of the total count and the record type.
	//*** Also alloc memory for the first element, the m_MainWindow.
	memsize=(sizeof(long)+sizeof(totalCnt)+sizeof(typeflg)+sizeof(procid));
	hGlobal=GlobalAlloc(GHND,memsize);
	memptr=GlobalLock(hGlobal);

	//Set the Main Window pointer
	(*(long*)memptr)=(long)m_Explorer->m_MainWindow;
	offptr+=sizeof(long); //*** Step over the m_MainWindow

	//Set the number of ents
	memcpy((((char *)memptr)+offptr),(void *)&totalCnt,sizeof(totalCnt));
	offptr+=sizeof(totalCnt);

	// Set the record type.
	rbb=SDS_tblgetter(IEXP_EntityCol[IEXP_CurrentIndx].cType,IEXP_ItemDefPtr->name(),TRUE,IEXP_ODNode->m_dbDrawing);
	if (ic_assoc(rbb,-1)) return;
	typeflg=(long)ic_rbassoc->resval.rlname[1];
	memcpy((((char *)memptr)+offptr),&typeflg,sizeof(typeflg));
	offptr+=sizeof(typeflg); //Step over the db_drawing
	if (rbb) sds_relrb (rbb);

	//Set the Process ID
	procid = GetCurrentProcessId();
	memcpy((((char *)memptr)+offptr),(void *)&procid,sizeof(procid));
	offptr+=sizeof(procid);

	// Step through every row for the type and check if it is selected, if so then call appropriate function to add the record info.
	for(m_ItemIndex;m_ItemIndex!=-1;--m_ItemIndex) {
		if (!m_Windowsideflag) {
			if (!m_ListCtl[IEXP_CurrentIndx].GetItemState(m_ItemIndex,LVIS_SELECTED)) continue;
		}
		switch(IEXP_EntityCol[IEXP_CurrentIndx].IType) {
			case IEXP_LATAB_IDX:
				IEXP_ItemDefPtr=(IEXP_ItemDef *)m_ListCtl[IEXP_CurrentIndx].GetItemData(m_ItemIndex);
				if (strsame(IEXP_ItemDefPtr->name(),ResourceString(IDC_ICADEXPLORER_0_18, "0" ))) continue;
				memptr=(void *)CopytabItem(IEXP_ItemDefPtr->name(),hGlobal,&memsize,&offptr);
				break;
			case IEXP_LTTAB_IDX:
				IEXP_ItemDefPtr=(IEXP_ItemDef *)m_ListCtl[IEXP_CurrentIndx].GetItemData(m_ItemIndex);
				if ((strisame(IEXP_ItemDefPtr->name(),IEXP_CONTINUOUS)) ||
					(strisame(IEXP_ItemDefPtr->name(),IEXP_BYBLOCK)) || (strisame(IEXP_ItemDefPtr->name(),IEXP_BYLAYER))) continue;
				if (strchr(IEXP_ItemDefPtr->name(),IEXP_VER_BAR)) continue;
				memptr=(void *)CopytabItem(IEXP_ItemDefPtr->name(),hGlobal,&memsize,&offptr);
				break;
			case IEXP_TSTAB_IDX:
				IEXP_ItemDefPtr=(IEXP_ItemDef *)m_ListCtl[IEXP_CurrentIndx].GetItemData(m_ItemIndex);
				if (strchr(IEXP_ItemDefPtr->name(),IEXP_VER_BAR)) continue;
				memptr=(void *)CopytabItem(IEXP_ItemDefPtr->name(),hGlobal,&memsize,&offptr);
				break;
			case IEXP_BNTAB_IDX:
				IEXP_ItemDefPtr=(IEXP_ItemDef *)m_ListCtl[IEXP_CurrentIndx].GetItemData(m_ItemIndex);
				memptr=(void *)CopytabItem(IEXP_ItemDefPtr->name(),hGlobal,&memsize,&offptr);
				break;
			case IEXP_VWTAB_IDX:
				IEXP_ItemDefPtr=(IEXP_ItemDef *)m_ListCtl[IEXP_CurrentIndx].GetItemData(m_ItemIndex);
				memptr=(void *)CopytabItem(IEXP_ItemDefPtr->name(),hGlobal,&memsize,&offptr);
				break;
			case IEXP_CSTAB_IDX:
				IEXP_ItemDefPtr=(IEXP_ItemDef *)m_ListCtl[IEXP_CurrentIndx].GetItemData(m_ItemIndex);
				if (strsame(IEXP_ItemDefPtr->name(),IEXP_WORLD)) continue;
				memptr=(void *)CopytabItem(IEXP_ItemDefPtr->name(),hGlobal,&memsize,&offptr);
				break;
			case IEXP_DSTAB_IDX:
				IEXP_ItemDefPtr=(IEXP_ItemDef *)m_ListCtl[IEXP_CurrentIndx].GetItemData(m_ItemIndex);
				if (strchr(IEXP_ItemDefPtr->name(),IEXP_VER_BAR)) continue;
				memptr=(void *)CopytabItem(IEXP_ItemDefPtr->name(),hGlobal,&memsize,&offptr);
				break;
		}
	}
	// Unlock the Global memory, open the clipboard, empty it,set the clipboard contents, and then close the clipboard.
	GlobalUnlock(hGlobal);
	OpenClipboard();
	EmptyClipboard();
	SetClipboardData(IEXP_Clipboard,hGlobal);
	CloseClipboard();
	m_selflag=FALSE;
	if (type) RemoveItem();
	return;
}


///////////////////////////////////////////////////////////////////////
// This function changes the ITEM list windows from one to the other
// by disableing the curent window and enableing the new window.
//
// OldIndex = This is the ID for the window being disabled.
// NewIndex = This is the ID for the window being enabled.
// IEXP_CurrentIndx = Hold the ID of the current window only one window
//					  can be active at one time.
//
// Nothing is returned.
//
void IEXP_CdetailView::ChangeWindow(WPARAM OldIndex,LPARAM NewIndex) {
	RECT	Crect;

	GetClientRect(&Crect);
	// Hide the current ITEM list window.
	if (OldIndex!=-1) {
		m_ListCtl[(OldIndex>=100)?7:OldIndex].EnableWindow(FALSE);
		m_ListCtl[(OldIndex>=100)?7:OldIndex].ShowWindow(SW_HIDE);
	}
	// Enable the new ITEM list window.
	if (NewIndex!=-1) {
		IEXP_CurrentIndx=(NewIndex>=100)?7:(short)NewIndex;
		m_ListCtl[IEXP_CurrentIndx].initData(this, IEXP_CurrentIndx);
		m_ListCtl[(NewIndex>=100)?7:NewIndex].EnableWindow();
		m_ListCtl[(NewIndex>=100)?7:NewIndex].ShowWindow(SW_SHOWNORMAL);
		m_staticIvbtm.MoveWindow(0,0,Crect.right,20);
		m_staticIvbar.MoveWindow(0,0,Crect.right,20);
		m_ListCtl[(NewIndex>=100)?7:NewIndex].MoveWindow(0,20,Crect.right,(Crect.bottom-20));
		if (m_ListCtl[(NewIndex>=100)?7:NewIndex].GetItemCount()) {
			m_dataptr=m_ListCtl[(NewIndex>=100)?7:NewIndex].GetItemData(0);
		} else m_dataptr=0;
	} else IEXP_CurrentIndx=0; // Set only if no windows are present.
	// Update the static bar title and the main window title.
	sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER__S_SETTIN_128, "%s settings in drawing - [%s]"),IEXP_TABINFO[IEXP_CurrentIndx].Name,IEXP_ODNode->szDrawing);

	CFont* pFont = new CFont();
	if (pFont)
	{
		pFont->CreateStockObject(DEFAULT_GUI_FONT);
		m_staticIvbar.SetFont(pFont);
	}
	m_staticIvbar.SetWindowText(IEXP_szText);
	sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER_DRAWING_E_129, "IntelliCAD Explorer - %s" ),IEXP_TABINFO[IEXP_CurrentIndx].Name);
	m_Explorer->SetWindowText(IEXP_szText);

	// Modified CyberAge AP 09/04/2000[
	if(IEXP_EntityCol[IEXP_CurrentIndx].IType != IEXP_LATAB_IDX || IEXP_Sortitem !=0)
		UpdateDataList1(TRUE);
	m_ListCtl[IEXP_CurrentIndx].SortItems(ListViewCompareProc,(LPARAM)IEXP_Sortitem);
	if(IEXP_EntityCol[IEXP_CurrentIndx].IType != IEXP_LATAB_IDX || IEXP_Sortitem !=0)
		UpdateDataList1(FALSE);
	// Modified CyberAge AP 09/04/2000]
	if (pFont) delete pFont;
}

///////////////////////////////////////////////////////////////////////
// The framework calls this member function after the window’s size has changed.
//
// I really don't know why this has to be done.
//
// nType : Specifies the type of resizing requested. This parameter can be one of the following values:
//		   SIZE_MAXIMIZED	Window has been maximized.
//		   SIZE_MINIMIZED	Window has been minimized.
//		   SIZE_RESTORED   Window has been resized, but neither SIZE_MINIMIZED nor SIZE_MAXIMIZED applies.
//		   SIZE_MAXHIDE   Message is sent to all pop-up windows when some other window is maximized.
//		   SIZE_MAXSHOW   Message is sent to all pop-up windows when some other window has been restored to its former size.
//	cx	 : Specifies the new width of the client area.
//	cy	 : Specifies the new height of the client area.
//
// Nothing is returned.
//
void IEXP_CdetailView::OnSize(UINT nType, int cx, int cy) {
	m_staticIvbtm.MoveWindow(0,0,cx,20);
	m_staticIvbar.MoveWindow(0,0,cx,20);
	if (IEXP_CurrentIndx!=-1) {
		m_ListCtl[IEXP_CurrentIndx].MoveWindow(0,20,cx,cy-20);
	}
//	  CWnd::OnSize(nType,cx,cy);
	CFrameWnd::OnSize(nType,cx,cy);
}


LRESULT IEXP_CdetailView::NotifyBlkHandler(UINT message, WPARAM wParam, LPARAM lParam) {
	struct IEXP_BlksList *blkptr=(struct IEXP_BlksList *)lParam;

	switch(wParam) {
		case IEXP_ITEMCHANGED:
			if (m_DlgState) {
				::SendMessage(m_EditDlg->m_hWnd,IEXP_CHANGEITEM,blkptr->index,blkptr->dataptr);
			} else m_dataptr=blkptr->dataptr;
			m_ItemIndex=blkptr->index;
			break;
	}
	return 0L;
}

//Bugzilla No. 78037 ; 29-04-2002 [
// This functiion updates the names of xref dependant tables when xref block is renamed.
// This function not only updates the name in list Ctrl Item text but also the name stored in
// each items specific Data.
void IEXP_CdetailView::IEXP_UpdateXrefName(char *origname,char * newname)
{
	int nTypeIndex = 0;
	int nItemIndex = 0;
	int nItemCount = 0;
	int len = 0;
	char szItemName[IC_ACADBUF];
	char szText[IC_ACADBUF];
	char *tok = NULL;

	for(nTypeIndex=0;nTypeIndex < (IEXP_NumofTypes-1); nTypeIndex++)
	{
		//Do not have xref entries for co-ordinate system table and
		// view table.
		if ( nTypeIndex == IEXP_CSTAB_IDX || nTypeIndex == IEXP_VWTAB_IDX )
			continue;

		nItemCount = m_ListCtl[nTypeIndex].GetItemCount();
		for(nItemIndex = 0; nItemIndex < nItemCount; nItemIndex++ )
		{
			len = m_ListCtl[nTypeIndex].GetItemText(nItemIndex,0,szText,IC_ACADBUF);
			//for xref name minimum length should be greater than 2.
			if ( len <= 2 )
				continue;

			//check if xref name
			tok = strtok(szText,CMD_XREF_TBLDIVIDERSTR);

			if ( tok == NULL )
				continue;
			//check if we have to update this one
			if ( strisame(tok,origname) != 1 )
				continue;

			tok = strtok(NULL,CMD_XREF_TBLDIVIDERSTR);
			if ( tok == NULL )
				continue;

			//make a new xref name
			sprintf(szItemName,"%s%s%s",newname,CMD_XREF_TBLDIVIDERSTR,tok);

			//set the new xref name
			m_ListCtl[nTypeIndex].SetItemText(nItemIndex,0,szItemName);

			//we also have to change the block name stored in data ptr per list item
			IEXP_ItemDefPtr = (IEXP_ItemDef *)m_ListCtl[nTypeIndex].GetItemData(nItemIndex);
			if ( IEXP_ItemDefPtr == NULL )
				continue;

			IEXP_ItemDefPtr->setName(szItemName);
		}
	}
}
//Bugzilla No. 78037 ; 29-04-2002 ]
///////////////////////////////////////////////////////////////////////
// This function is called whenever an activity is done in the current
// ITEM window, this function handles most of the processing if somthing
// is done.
//
// LVN_ITEMCHANGED	  = when another row is selected in the current window.
// 0xFFFFFFFB		  = when the user right clicks on the current window.
// LVN_BEGINLABELEDIT = when the user starts to edit the first item in a row.
// LVN_ENDLABELEDIT   = when the user hits enter when done editing.
// LVN_COLUMNCLICK	  = when the user clicks on a column heading, we do a sort.
//
// 0	: Normal return.
//
LRESULT IEXP_CdetailView::NotifyHandler(UINT message, WPARAM wParam, LPARAM lParam) {
	LV_DISPINFO   *pLvdi= (LV_DISPINFO *)lParam;
	NM_LISTVIEW   *pNm	= (NM_LISTVIEW *)lParam;
	LV_KEYDOWN	  *pnkd = (LV_KEYDOWN FAR *)lParam;
//	  IMAGEINFO 	 iImageInfo;

	CMenu		   menubar;
	CEdit		  *pEdit;
	short		   Index;
//	  POINT 		 Point;
	CPoint		   PickPoint;
	struct resbuf *rbb=NULL;
	db_drawing	  *dbDrawing;
	db_handitem   *dbhandItem;
	static char    EditTextName[IC_ACADBUF];
	CString 	   String;
	int colnum = 0;
	int Color = 0;
	int Flags = 0;
	BOOL bReEdit = FALSE;
	static BOOL bCheckCursorPoint = TRUE;
	bool needFullName = false;
	CString pathname;
	//Bugzilla No. 78037 ; 29-04-2002
	char origname[IC_ACADBUF];
	char * blkname = NULL;

	EditTextName[0] = '\0';
	if (wParam!=IEXP_LISTVIEW) return 0L;

	switch(pLvdi->hdr.code)	{
		case LVN_BEGINDRAG:

			// save the point picked in the PickPoint point class. Point is in view coords.
//			  PickPoint=pNm->ptAction;
//			  m_Explorer->m_pDragImage=m_ListCtl[IEXP_CurrentIndx].CreateDragImage(pNm->iItem,&pNm->ptAction);
//			  if (m_Explorer->m_pDragImage) {
//				  SetCapture();
//				  ::SetCursor(::LoadCursor(NULL,IDC_NO));
//				  ReleaseCapture();
//				  m_Explorer->m_pDragImage->GetImageInfo(0,&iImageInfo);
				// Get the position of the item selected. Pos=top left corner in view coords.
//				  m_ListCtl[IEXP_CurrentIndx].GetItemPosition(pNm->iItem,&Point);
//				  // Calculate the distance from the top left corner from point picked.
//					CPoint pt((PickPoint.x-Point.x),(PickPoint.y-Point.y));
//				  // Drag pick point.
//				  m_Explorer->m_pDragImage->BeginDrag(0,pt);
				// Begin drag point.
//				  m_Explorer->m_pDragImage->DragEnter((CWnd*)&m_Explorer,PickPoint);
//			  } else { delete m_Explorer->m_pDragImage; }

			break;
		case NM_SETFOCUS:
			m_Windowsideflag=FALSE;
			EditTextName[0] = '\0';
			break;
		case LVN_ITEMCHANGING:
			if (m_Explorer) {
				switch(IEXP_CurrentIndx) {
					case 0:
						m_Explorer->EnableTbB(&m_Explorer->m_LToolBar,EXP_LAYER_ON,TRUE);
						m_Explorer->EnableTbB(&m_Explorer->m_LToolBar,EXP_LAYER_FREEZE,TRUE);
						m_Explorer->EnableTbB(&m_Explorer->m_LToolBar,EXP_LAYER_LOCK,TRUE);
					case 1:
					case 2:
						m_Explorer->EnableTbB(&m_Explorer->m_MToolBar,EXP_CURRENT_M,TRUE);
						break;
				}
			}
			if (m_DlgState) {
				::SendMessage(m_EditDlg->m_hWnd,IEXP_CHANGINGITEM,(LPARAM)0,(WPARAM)0);
			} else m_dataptr=pNm->lParam;
			break;
		case LVN_ITEMCHANGED:
			if (pNm->uNewState==3 && pNm->iItem!=-1) {
				m_ItemIndex=pNm->iItem;
				if (!IEXP_CurrentIndx) m_Explorer->UpdLayerState(pNm->lParam,3);
				if (m_DlgState) {
					::SendMessage(m_EditDlg->m_hWnd,IEXP_CHANGEITEM,pNm->iItem,pNm->lParam);
				} else m_dataptr=pNm->lParam;
			}
			break;
		case 0xFFFFFFFB:
			if (IEXP_CurrentIndx==7) break;
			if (m_Explorer->m_editmode) break;
			if (menubar.LoadMenu(EXP_EXPLORER_MNU)) {
				CMenu* pPopup = menubar.GetSubMenu(0);
				GetCursorPos(&m_lpoint);
				pPopup->TrackPopupMenu(TPM_RIGHTBUTTON,m_lpoint.x,m_lpoint.y,m_Explorer);
			}
			break;
	   case LVN_KEYDOWN:
		   switch(pnkd->wVKey) {
			   case VK_DELETE: RemoveItem(); break;
			   case VK_F2: RenameItem(); break;
			   default:
				   if ((Index=GetKeyState(VK_CONTROL))&IEXP_HI_BIT_SHORT) {
					  switch(pnkd->wVKey) {
						  case		 'a':
						  case		 'A': SelectAll();	break;
						  case		 'x':
						  case		 'X': CopyCutItem(TRUE);  break;
						  case		 'c':
						  case		 'C': CopyCutItem(FALSE); break;
						  case		 'v':
						  case		 'V': PasteItem();	break;
						  case		 'z':
						  case		 'Z': AfxMessageBox(ResourceString(IDC_ICADEXPLORER_UNDO_NOT__130, "Undo not available" )); break; // TODO-XXX
						  case		 'y':
						  case		 'Y': AfxMessageBox(ResourceString(IDC_ICADEXPLORER_REDO_NOT__131, "Redo not available" )); break; // TODO-XXX
						  case VK_RETURN: PropertyDlg(); break;
					  }
				   }
				   break;
			}
			break;
	   case LVN_BEGINLABELEDIT:
		   POINT point;
		   int col;
		   GetCursorPos(&point);
		   ::ScreenToClient (m_ListCtl[IEXP_CurrentIndx].m_hWnd, &point);
		   PickPoint = point;
		   m_ListCtl[IEXP_CurrentIndx].HitTestEx(PickPoint, &col) ;
		   if (col !=0 && bCheckCursorPoint)	//end label edit only if the bCheckCursorPoint flag is true
			   {
		   		m_ListCtl[IEXP_CurrentIndx].EndLabelEditing();
 				break;
			   }
		   bCheckCursorPoint = TRUE;	//reset the flag.

			m_Newflag=FALSE;
			// Set the edit flag to true.
			m_Explorer->m_editmode=TRUE;
			// Get the handle to the edit control.
			pEdit = m_ListCtl[IEXP_CurrentIndx].GetEditControl();

			// Get The Text from the edit box.
			if (EditTextName[0] == '\0')
			{
				m_ListCtl[IEXP_CurrentIndx].GetItemText(pLvdi->item.iItem, 0, EditTextName, IC_ACADBUF);
			}
			if (!m_ListCtl[IEXP_CurrentIndx].ValidateData(pLvdi->item.iItem, 0))
				return (TRUE);

			// Limit the amount of text that the user can enter.
			// Limit the amount of text that the user can enter.
			/*D.G.*/// Use maximum 255 characters.
			if(_getmbcp())
				pEdit->SetLimitText((IC_ACADNMLEN - 1) * 2);
			else
				pEdit->SetLimitText(IC_ACADNMLEN - 1);
			break;

		case LVN_ENDLABELEDIT:
			if (!m_Explorer->m_editmode) break;
			if (!IEXP_ODNode) break;
			dbDrawing=IEXP_ODNode->m_dbDrawing;

			if ( pLvdi->item.pszText != NULL )
				ic_trim(pLvdi->item.pszText, "be");


			if (EditTextName[0] == '\0')
			{
				m_ListCtl[IEXP_CurrentIndx].GetItemText(pLvdi->item.iItem, 0, EditTextName, IC_ACADBUF);
			}

			if (pLvdi->item.iSubItem == 0) //this check is valid only for column 0.
			{
				if (pLvdi->item.pszText!=0 && ((rbb=SDS_tblgetter(IEXP_EntityCol[IEXP_CurrentIndx].cType,pLvdi->item.pszText,TRUE,IEXP_ODNode->m_dbDrawing))!=NULL))
				{
					sds_relrb(rbb);
					// If the string is the same then return;
					String=m_ListCtl[IEXP_CurrentIndx].GetItemText(pLvdi->item.iItem,0);
					if (strsame((char *)((LPCSTR)String),pLvdi->item.pszText)) return 0L;
					String=IEXP_TABINFO[IEXP_CurrentIndx].Name;
					String = String.Left((String.GetLength()-1));
					sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER_17, "A %s with the name you specified already exists. Please specify a different name."),String);
					AfxMessageBox(IEXP_szText);
					bCheckCursorPoint = FALSE;	//set this flag to false, informing the BEGINEEDITLABEL to not attempt to end lable editing
					//the above line is a strange bug.	When the AfxMessageBox is dismissed by the user, if the clickpoint of the OK button is over an existing
					//subitem, then a hit test in BEGINLABELEDIT will indicate that a sub item is to be eidted, when in fact we want the main
					//item to be edited.  So we set this flag - bCheckCursorPoint to indicate not to check the cursor point when determining
					//which item to edit.
					m_ListCtl[IEXP_CurrentIndx].SetFocus();
					m_ListCtl[IEXP_CurrentIndx].EditLabel(pLvdi->item.iItem);
					break;
				}
			}

			// If label editing wasn't cancelled and the text buffer is non-NULL...
			if (pLvdi->item.iItem==-1)
				break;
			else
			{
				if (pLvdi->item.pszText==NULL)
				{
					break;
				}
			}

			// Check for a valid Item name - only for column 0.  Other columns may have free format strings.
			colnum = pLvdi->item.iSubItem;
			if ((colnum == 0) && !ic_isvalidname(pLvdi->item.pszText))
			{
				//unknown -[
				//AfxMessageBox(ResourceString(IDC_ICADEXPLORER_THE_NAME__134, "The name can use up to 31 characters, including letters, numbers,\nhyphens, underscores, and dollar signs ($), but not spaces.\nPlease try again." ));
				CString	str;
				//Modified Cybage AJK 17/12/2001 [
				//Reason-Fix for bug no 77948 from Bugzilla
				//str.FormatMessage(ResourceString(IDC_WRONG_NAME_MESSAGE, "The name can use up to %1!d! characters, including letters, numbers,\nhyphens, underscores, and dollar signs ($), but not spaces.\nPlease try again."), IC_ACADNMLEN - 1);
				str.FormatMessage(ResourceString(IDC_WRONG_NAME_MESSAGE, "The name can use up to %1!d! characters, including letters, numbers,\nhyphens, underscores, and dollar signs ($), but not spaces and <>/"":\?*,=|'.\nPlease try again."), IC_ACADNMLEN - 1);
				//Modified Cybage AJK 17/12/2001 DD/MM/YYYY ]
				AfxMessageBox((LPCTSTR)str);
				//unknown ]-
				bCheckCursorPoint = FALSE;	//set this flag to false, informing the BEGINEEDITLABEL to not attempt to end lable editing
				m_ListCtl[IEXP_CurrentIndx].SetFocus();
				m_ListCtl[IEXP_CurrentIndx].EditLabel(pLvdi->item.iItem);
				break;
			}

			//Validity checking for all columns
			BOOL bValidateDouble;
			BOOL bValidatePoint;
			bValidateDouble=FALSE;
			bValidatePoint=FALSE;
			String=pLvdi->item.pszText;
			double NewReal;
			sds_point pt;

			switch(IEXP_EntityCol[IEXP_CurrentIndx].IType)
			{
				case IEXP_LATAB_IDX:	//nothing to validate
					break;
				case IEXP_LTTAB_IDX:	//nothing to validate
					break;
				case IEXP_TSTAB_IDX:
					switch (colnum)
					{
					case IEXP_TS_HEIGHT	: //IEXP_HEIGHT
					case IEXP_TS_WIDTH	: //IEXP_WIDTH_FACTOR
					case IEXP_TS_SLANT	: //IEXP_OBLIQUE_ANGLE

						bValidateDouble=TRUE;
						break;
					}
					break;

				case IEXP_CSTAB_IDX:
					switch (colnum)
					{
					case 1: //IEXP_ORIGIN,
					case 2: //IEXP_X_DIRECTION,
					case 3: //IEXP_Y_DIRECTION,
						bValidatePoint=TRUE;
						break;
					}
					break;

				case IEXP_VWTAB_IDX:
					switch (colnum)
					{
					case 1: //IEXP_HEIGHT,
					case 2: //IEXP_WIDTH,
						bValidateDouble=TRUE;
						break;
					case 3: //IEXP_VIEW_DIRECTION
						bValidatePoint=TRUE;
						break;
					}
					break;

				case IEXP_BNTAB_IDX:
					switch (colnum)
					{
					case 2: //IEXP_BLOCK_INSPOINT,
						bValidatePoint=TRUE;
					}
					break;
			}

			if (bValidateDouble)
			{
				if (!ic_isnumstr(String.GetBuffer(0), &NewReal))
				{
					AfxMessageBox(ResourceString(IDC_ICADEXPLORER_18, "Invalid data entry. Please enter a valid number."));
					bReEdit = TRUE;
					break;
				}
			}

			if (bValidatePoint)
			{
				if (!ExtractPointFromCString (String, pt))
				{
					AfxMessageBox(ResourceString(IDC_ICADEXPLORER_19, "Invalid data entry. Please enter a valid point - 3 valid numbers separated by commas or semicolons."));
					bReEdit = TRUE;
					break;
				}
			}


			// Build resbuf linklist for update.
			switch(IEXP_EntityCol[IEXP_CurrentIndx].IType)
			{
				case IEXP_DSTAB_IDX:
					dbhandItem=dbDrawing->findtabrec(DB_DIMSTYLETAB,EditTextName,1);
					break;
				case IEXP_BNTAB_IDX:
					dbhandItem=dbDrawing->findtabrec(DB_BLOCKTAB,EditTextName,1);
					break;
				case IEXP_VWTAB_IDX:
					dbhandItem=dbDrawing->findtabrec(DB_VIEWTAB,EditTextName,1);
					break;
				case IEXP_CSTAB_IDX:
					dbhandItem=dbDrawing->findtabrec(DB_UCSTAB,EditTextName,1);
					break;
				case IEXP_TSTAB_IDX:
					dbhandItem=dbDrawing->findtabrec(DB_STYLETAB,EditTextName,1);
					break;
				case IEXP_LTTAB_IDX:
					dbhandItem=dbDrawing->findtabrec(DB_LTYPETAB,EditTextName,1);
					break;
				case IEXP_LATAB_IDX:
					dbhandItem=dbDrawing->findtabrec(DB_LAYERTAB,EditTextName,1);
					break;
			}

			// Call tablemesser to update item in table, and update row.
			BOOL bNeedRegen, bNeedRedraw;
			bNeedRegen = FALSE;
			bNeedRedraw = FALSE;

			if(dbhandItem)
			{
				/*DG - 10.1.2002*/// Cache the change for undo.
				CUndoSaver	undoSaver(dbhandItem, dbDrawing);

				switch(IEXP_EntityCol[IEXP_CurrentIndx].IType)
				{
					case IEXP_LATAB_IDX:
						switch (colnum)
						{
						case IEXP_LA_NAME:
							// Sets the new layer name to the layer in the tables
							dbhandItem->set_2((char *)((LPCTSTR)String));
							break;
						case IEXP_LA_COLOR:
							Color = ic_colornum((char *)((LPCTSTR)String));
							dbhandItem->set_62(Color);
							bNeedRegen = TRUE;
							break;
						case IEXP_LA_LINETYPE:
							// Sets the new ltype name to the layer in the tables
							dbhandItem->set_6((char *)((LPCTSTR)String),dbDrawing);
							bNeedRegen = TRUE;
							break;
						case IEXP_LA_ON_OFF:
							// Negative color implies layer is off
							dbhandItem->get_62(&Color);
							if (String == ResourceString(IDC_ICADEXPLORER_OFF_16, "Off"))
								Color = -fabs(Color);
							else if (String == ResourceString(IDC_ICADEXPLORER_ON_17, "On"))
								Color = fabs(Color);
							dbhandItem->set_62(Color);
							bNeedRegen = TRUE;
							break;
						case IEXP_LA_LOCKED:
							// Flags & 4 implies layer is locked
							dbhandItem->get_70(&Flags);
							if (String == ResourceString(IDC_IEXP_YES, "Yes"))
								Flags |= IC_LAYER_LOCKED;
							else if (String == ResourceString(IDC_IEXP_NO, "No"))
								Flags &= ~IC_LAYER_LOCKED;
							dbhandItem->set_70(Flags);
							bNeedRegen = TRUE;
							break;
						case IEXP_LA_FROZEN:
							// Flags & 1 implies layer is frozen
							dbhandItem->get_70(&Flags);
							if (String == ResourceString(IDC_IEXP_FROZEN_STR, "Frozen"))
								Flags |= IC_LAYER_FROZEN;
							else if (String == ResourceString(IDC_IEXP_THAWED_STR, "Thawed"))
								Flags &= ~IC_LAYER_FROZEN;
							dbhandItem->set_70(Flags);
							bNeedRegen = TRUE;
							break;
						case IEXP_LA_VP_FREEZECUR:
							db_viewport *pVP;
							char *lname;
							pVP=dbDrawing->GetCurrentViewport();
							if (pVP)
								{
								bool bChanged=false;
								dbhandItem->get_2(&lname);
								if (String == ResourceString(IDC_IEXP_FROZEN_STR, "Frozen"))
									pVP->VpLayerFreezeLayer(lname, dbDrawing, &bChanged);
								else if (String == ResourceString(IDC_IEXP_THAWED_STR, "Thawed"))
									pVP->VpLayerThawLayer(lname, dbDrawing, &bChanged);
								if (bChanged)
									SDS_DBModified( IC_DBMOD_ENTITIES_MODIFIED);
							}
							bNeedRegen = TRUE;
							break;
						case IEXP_LA_VP_FREEZENEW:
							// Flags & 2 implies layer is frozen in all new viewports
							dbhandItem->get_70(&Flags);
							if (String == ResourceString(IDC_IEXP_FROZEN_STR, "Frozen"))
								Flags |= IC_LAYER_VPFROZEN;
							else if (String == ResourceString(IDC_IEXP_THAWED_STR, "Thawed"))
								Flags &= ~IC_LAYER_VPFROZEN;
							dbhandItem->set_70(Flags);
							break;
						case IEXP_LA_LINEWEIGHT:
							// Set Lineweight
							{
								int i, top;
								top = ( IEXP_lwunits == 1 ? IDC_COMBO_M_LWDEFAULT : IDC_COMBO_I_LWDEFAULT );
								for( i = 0 ; i < 27 ; i++ )
								{
									if( String == ResourceString(top+i, ""))
									{
										Flags = db_index2lweight(i-3);
										dbhandItem->set_370(Flags);
										bNeedRedraw = TRUE;
										break;
									}
								}
							}
							break;
						case IEXP_LA_PLOT:
							// Set Plot
							if (String == ResourceString(IDC_IEXP_YES, "Yes"))
								Flags = 1;
							else
								Flags = 0;
							dbhandItem->set_290(Flags);
							break;
						default:
							ASSERT(0);
							break;
						}
						break;

					case IEXP_LTTAB_IDX:
						switch (colnum)
						{
						case 0: //IEXP_LINETYPE_NAME
							dbhandItem->set_2((char *)((LPCTSTR)String));
							break;
						case 1: //IEXP_DESCRIPTIVE_TXT
							dbhandItem->set_3((char *)((LPCTSTR)String));
							break;
						case 2: //IEXP_LINETYPE_DEF - non-editable
						default:
							ASSERT(0);
							break;
						}
						break;

					case IEXP_TSTAB_IDX:
						switch (colnum)
						{
						case IEXP_TS_NAME:	//IEXP_TEXT_STYLE_NAME,
							dbhandItem->set_2((char *)((LPCTSTR)String));
							break;
						case IEXP_TS_HEIGHT: //IEXP_HEIGHT,
							dbhandItem->set_40(NewReal);
							bNeedRegen = TRUE;
							break;
						case IEXP_TS_WIDTH: //IEXP_WIDTH_FACTOR,
							dbhandItem->set_41(NewReal);
							bNeedRegen = TRUE;
							break;
						case IEXP_TS_SLANT: //IEXP_OBLIQUE_ANGLE,
							dbhandItem->set_50(NewReal*IC_PI/180.0);
							bNeedRegen = TRUE;
							break;
						case IEXP_TS_FONT: //IEXP_FONT_NAME,
						{
							char font[IC_ACADBUF];
							strcpy(font, (char*)((LPCTSTR)String)); // now font is *.shx or fontfamily
							bool haveTrueTypeFont;
							CString extension = String.Right(4);
							extension.TrimLeft();
							if ((0 == extension.CompareNoCase(".shx"/*DNT*/)) ||
								(0 == extension.CompareNoCase(".shp"/*DNT*/))){
									haveTrueTypeFont = false;
							}else{
									haveTrueTypeFont = true;
							}
							if (!haveTrueTypeFont)
							{
								// Set style table record.
								if(((db_styletabrec *) dbhandItem)->hasTrueTypeFont()){
									((db_styletabrec *) dbhandItem)->ensureNoTrueTypeEED(dbDrawing);
								}
								dbhandItem->set_3(font, dbDrawing);
								// Set right pane.
								m_ListCtl[IEXP_CurrentIndx].SetItemText(pLvdi->item.iItem, IEXP_TS_FONT, String);
								m_ListCtl[IEXP_CurrentIndx].SetItemText(pLvdi->item.iItem, IEXP_TS_STYLE, ""/*DNT*/);
								m_ListCtl[IEXP_CurrentIndx].SetItemText(pLvdi->item.iItem, IEXP_TS_LANG, ""/*DNT*/);
							}else{
								// Get filename and stylenumber from fontfamily.
								char fontfile[IC_ACADBUF];
								char globalName[IC_ACADBUF];
								long stylenumber;
								CString cstext;

								if(CTTFInfoGetter::get().getFontFileName(fontfile, font)){
									CTTFInfoGetter::get().getGlobalStyleByLocalName( font, stylenumber, globalName );
								}else{
									if( CTTFInfoGetter::get().getGlobalStyleByLocalName( font, stylenumber, globalName )){
										CTTFInfoGetter::makeFontFileName(globalName,fontfile);
									}
								}
								// Set style table record.
								dbhandItem->set_3(fontfile, dbDrawing);	// font name
								dbhandItem->set_4(""/*DNT*/, dbDrawing); // Bugzilla#78523 Ebatech(CNBR) 15/5/2003
								((db_styletabrec *) dbhandItem)->setFontFamily(font, dbDrawing);
								((db_styletabrec *) dbhandItem)->setFontStyleNumber(font,stylenumber,dbDrawing);
								// Set right pane
								m_ListCtl[IEXP_CurrentIndx].SetItemText(pLvdi->item.iItem, IEXP_TS_FONT, String);
								// Get Style String value.
								cstext = "";
								CInPlaceFontStyleCombo* style = (CInPlaceFontStyleCombo*)
									m_ListCtl[IEXP_CurrentIndx].ShowInPlaceFontStyleCombo(pLvdi->item.iItem,
									IEXP_TS_STYLE, stylenumber);
								if(style){
									style->SetCurSel(0);
									style->GetWindowText(cstext);
									delete style;
								}
								m_ListCtl[IEXP_CurrentIndx].SetItemText(pLvdi->item.iItem, IEXP_TS_STYLE, cstext);
								// Get Language String value.
								CInPlaceFontLangCombo* lang = (CInPlaceFontLangCombo*)
									m_ListCtl[IEXP_CurrentIndx].ShowInPlaceFontLangCombo(pLvdi->item.iItem,
									IEXP_TS_LANG, stylenumber );
								cstext = "";
								if(lang){
									lang->SetCurSel(0);
									lang->GetWindowText(cstext);
									delete lang;
								}
								m_ListCtl[IEXP_CurrentIndx].SetItemText(pLvdi->item.iItem, IEXP_TS_LANG, cstext);
								// Reset Bigfont value
								m_ListCtl[IEXP_CurrentIndx].SetItemText(pLvdi->item.iItem, IEXP_TS_BIGFONT, ""/*DNT*/);
								// EBATECH(CNBR) -[ Bugzilla#78523 Turn off Vertical flag when user choose TrueType font 2003/5/5
								int flag;
								dbhandItem->get_70(&flag);
								flag &= (~IC_SHAPEFILE_VERTICAL);
								dbhandItem->set_70( flag );
								m_ListCtl[IEXP_CurrentIndx].SetItemText(pLvdi->item.iItem, IEXP_TS_VERTICAL, ResourceString(IDC_IEXP_NO, "No"));
								// EBATECH(CNBR) ]-
							}
							//bNeedRegen = TRUE;
							break;
						}
						case IEXP_TS_STYLE: //IEXP_FONT_STYLE
						case IEXP_TS_LANG:
						{
							if( NowEnumulating == true ){
								break;
							}
							// make sure we only get called for a TrueType font
							CString tmpFontName = m_ListCtl[IEXP_CurrentIndx].GetItemText(pLvdi->item.iItem,IEXP_TS_FONT);
							CString tmpFontStyle = m_ListCtl[IEXP_CurrentIndx].GetItemText(pLvdi->item.iItem,IEXP_TS_STYLE);
							CString tmpFontLang = m_ListCtl[IEXP_CurrentIndx].GetItemText(pLvdi->item.iItem,IEXP_TS_LANG);
							if( colnum == IEXP_TS_STYLE ){
								tmpFontStyle = String;
							}else{
								tmpFontLang = String;
							}

							CString tmpExtension = tmpFontName.Right(IEXP_TS_FONT);
							tmpExtension.TrimLeft();

							ASSERT ((0 != tmpExtension.CompareNoCase(".shx"/*DNT*/)) &&
									(0 != tmpExtension.CompareNoCase(".shp"/*DNT*/)));

							if ((0 == tmpExtension.CompareNoCase(".shx"/*DNT*/)) ||
								(0 == tmpExtension.CompareNoCase(".shp"/*DNT*/)))
									break;

							// the style control is no longer available to us, so we have to
							// enumerate the styles for the current font and then get the
							// style number
							long styleNumber = GetFontStyleNumber(
								m_ListCtl, IEXP_CurrentIndx, pLvdi->item.iItem, tmpFontStyle, tmpFontLang);

							((db_styletabrec *) dbhandItem)->setFontStyleNumber(tmpFontName,
																				styleNumber,
																				dbDrawing);

							//bNeedRegen = TRUE;
							break;
						}
						case IEXP_TS_BIGFONT:
						{
							char bigfont[IC_ACADBUF];
							if(((db_styletabrec *)dbhandItem)->hasTrueTypeFont()){
								dbhandItem->set_3(IEXP_DEFAULT_FONT, dbDrawing);
								((db_styletabrec *) dbhandItem)->ensureNoTrueTypeEED(dbDrawing);
								m_ListCtl[IEXP_CurrentIndx].SetItemText(pLvdi->item.iItem, IEXP_TS_FONT, IEXP_DEFAULT_FONT);
							}
							if( String.Compare( ResourceString(IDC_EXPSTYLE_NO_BIGFONT, "(none)")) == 0){
								String = ""/*DNT*/;
							}
							strcpy(bigfont, (char*)((LPCTSTR)String)); // now font is *.shx or fontfamily
							dbhandItem->set_4(bigfont, dbDrawing);
							m_ListCtl[IEXP_CurrentIndx].SetItemText(pLvdi->item.iItem, IEXP_TS_BIGFONT, String);
							m_ListCtl[IEXP_CurrentIndx].SetItemText(pLvdi->item.iItem, IEXP_TS_STYLE, ""/*DNT*/);
							m_ListCtl[IEXP_CurrentIndx].SetItemText(pLvdi->item.iItem, IEXP_TS_LANG, ""/*DNT*/);
							//bNeedRegen = TRUE;
							break;
						}
						// EBATECH(CNBR) ]-
						// EBATECH(CNBR) -[ Use macro instead of immediate value 15/5/2003
						case IEXP_TS_BACKWORD:
							dbhandItem->get_71(&Flags);
							if (String == ResourceString(IDC_IEXP_YES, "Yes"))
								Flags = Flags | IC_SHAPEFILE_GEN_BACKWARD;
							else if (String == ResourceString(IDC_IEXP_NO, "No"))
								Flags = Flags ^ IC_SHAPEFILE_GEN_BACKWARD;
							dbhandItem->set_71(Flags);
							bNeedRegen = TRUE;
							break;
						case IEXP_TS_UPSIDEDOWN:
							dbhandItem->get_71(&Flags);
							if (String == ResourceString(IDC_IEXP_YES, "Yes"))
								Flags = Flags | IC_SHAPEFILE_GEN_UPSIDEDOWN;
							else if (String == ResourceString(IDC_IEXP_NO, "No"))
								Flags = Flags ^ IC_SHAPEFILE_GEN_UPSIDEDOWN;
							dbhandItem->set_71(Flags);
							bNeedRegen = TRUE;
							break;
						case IEXP_TS_VERTICAL:
							// Bugzilla#78523 When Style has truetype font, vertical flag must turn off.
							dbhandItem->get_70(&Flags);
							if( ((db_styletabrec *) dbhandItem)->hasTrueTypeFont() )
								Flags &= (~IC_SHAPEFILE_VERTICAL);
							else if (String == ResourceString(IDC_IEXP_YES, "Yes"))
								Flags |= IC_SHAPEFILE_VERTICAL;
							else if (String == ResourceString(IDC_IEXP_NO, "No"))
								Flags &= (~IC_SHAPEFILE_VERTICAL);
							dbhandItem->set_70(Flags);
							bNeedRegen = TRUE;
							break;
						default:
							ASSERT(0);
							break;
						}
						// EBATECH(CNBR) ]-
						break;

					case IEXP_CSTAB_IDX:
						switch (colnum)
						{
						case 0:	//IEXP_UCS_NAME,
							dbhandItem->set_2((char *)((LPCTSTR)String));
							break;
						case 1: //IEXP_ORIGIN,
							ExtractPointFromCString (String, pt);
							dbhandItem->set_10(pt);
							bNeedRegen = TRUE;
							break;
						case 2: //IEXP_X_DIRECTION,
							ExtractPointFromCString (String, pt);
							dbhandItem->set_11(pt);
							bNeedRegen = TRUE;
							break;
						case 3: //IEXP_Y_DIRECTION,
							ExtractPointFromCString (String, pt);
							dbhandItem->set_12(pt);
							bNeedRegen = TRUE;
							break;
						default:
							ASSERT(0);
							break;
						}
						break;

					case IEXP_VWTAB_IDX:
						switch (colnum)
						{
						case 0:	//IEXP_VIEW_NAME,
							dbhandItem->set_2((char *)((LPCTSTR)String));
							break;
						case 1: //IEXP_HEIGHT,
							dbhandItem->set_40(NewReal);
							bNeedRedraw = TRUE;
							break;
						case 2: //IEXP_WIDTH,
							dbhandItem->set_41(NewReal);
							bNeedRedraw = TRUE;
							break;
						case 3: //IEXP_VIEW_DIRECTION
							sds_point pt;
							ExtractPointFromCString (String, pt);
							dbhandItem->set_11(pt);
							bNeedRedraw = TRUE;
							break;
						default:
							ASSERT(0);
							break;
						}
						break;

					case IEXP_BNTAB_IDX:
						switch (colnum)
						{
						case 0:	//IEXP_BLOCK_NAME,
							// Sets the new block name to the block in the tables
							//Bugzilla No. 78037 ; 29-04-2002 [
							{
							dbhandItem->get_2(&blkname);
							// copy the previous block name in origname variable
							strcpy(origname,blkname);
							dbhandItem->set_2((char *)((LPCTSTR)String));
							break;
							}
							//Bugzilla No. 78037 ; 29-04-2002 ]
						case 2: //IEXP_BLOCK_INSPOINT,
							ExtractPointFromCString (String, pt);
							dbhandItem->set_10(pt);
							bNeedRegen = TRUE;
							break;
						case 3: //IEXP_XREF,
							//String = strrchr(pLvdi->item.pszText,IC_SLASH)+1;	//don't strip path name
							db_drawing *flp;
							db_handitem *handItem;
							sds_name ename;
							char *BlockName, *BlockInsertName;
							bool convertpl;
							resbuf plrb;
							SDS_getvar(NULL,DB_QPLINETYPE,&plrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
							if (plrb.resval.rint==2)
								convertpl=true;
							else
								convertpl=false;

							if((flp=SDS_CMainWindow->m_pFileIO.drw_openfile(NULL,(char *)((LPCTSTR)String),NULL,convertpl))==NULL)
							{
								AfxMessageBox(ResourceString(IDC_ICADEXPLORER_20, "Error opening the specified drawing."));
								bReEdit = TRUE;
								break;
							}
							dbhandItem->set_xrefdp(NULL);
							dbhandItem->set_xrefdp(flp);
							dbhandItem->set_looked4xref(1);
							dbhandItem->set_1((char *)((LPCTSTR)String));

							dbhandItem->get_2(&BlockName);
							ename[1]=(long)dbDrawing;
							handItem=NULL;
							while((handItem=dbDrawing->entnext_noxref(handItem))!=NULL)
							{
								if (handItem->ret_type() == DB_INSERT)
								{
									handItem->get_2(&BlockInsertName);
									if (!strsame(BlockName,BlockInsertName)) continue;
									ename[0] = (long)handItem;
									sds_entupd(ename);	//this reloads xref dependency
								}
							}

							if(RTNORM!=cmd_ReadAsBlock((char *)((LPCTSTR)String),BlockName,IC_INSMODE_XREF_UPDATE,flp))
								AfxMessageBox(ResourceString(IDC_ICADEXPLORER_21, "Failed to update the XRef."));

							//Now that the XRef has been changed, update all the other items in the list
							//Delete the existing bitmaps, and recreate them with the new xref.  Expensive!
							m_pimagelistI.DeleteImageList();
							m_pimagelistI.Create(IEXP_ICON_BITMAP_WIDTH,IEXP_ICON_BITMAP_HEIGHT,TRUE,1,0);
							m_ListCtl[IEXP_BNTAB_IDX].SetImageList(NULL,LVSIL_NORMAL);
							bNeedRegen = TRUE;
							break;
						default:
							ASSERT(0);
							break;
						}
						break;
					case IEXP_DSTAB_IDX:
						switch (colnum)
						{
						case 0:	//IEXP_DIM_STYLE_NAME,
							dbhandItem->set_2((char *)((LPCTSTR)String));
							break;
						}
						break;
				}	// switch(IEXP_EntityCol[IEXP_CurrentIndx].IType)

				/*DG - 10.1.2002*/// Cache the change for undo pushing regen_on_undo unit too.
				if(!IEXP_RegenOnUndo)
				{
					SDS_SetUndo(IC_UNDO_REGEN_ON_UNDO, NULL, NULL, NULL, dbDrawing);
					IEXP_RegenOnUndo = TRUE;
				}
				undoSaver.saveToUndo();


				/*D.G.*/// Call callback here if we haven't use sdsengine_entmod (where it's called)
				sds_name ename;
				ename[0] = (long)dbhandItem;
				ename[1] = (long)dbDrawing;
				SDS_CallUserCallbackFunc(SDS_CBENTMOD, (void *)ename, NULL, NULL);

				//Bugzilla No. 78037 ; 29-04-2002 [
				if ( IEXP_EntityCol[IEXP_CurrentIndx].IType == IEXP_BNTAB_IDX)
				{
					struct resbuf *elist=NULL;
					elist = sds_entget(ename);
					if ( elist != NULL )
					{
						if (ic_assoc(elist,70)==0 && (ic_rbassoc->resval.rint & IC_BLOCK_XREF ) )
						{
							cmd_renameXrefDependantTables(origname,(char *)(LPCTSTR)String,dbDrawing);
							IEXP_UpdateXrefName(origname,(char *)(LPCTSTR)String);
						}
						sds_relrb(elist);
					}
				}
				//Bugzilla No. 78037 ; 29-04-2002 ]

			}	// if (dbhandItem)

			// Update the layer name in the view.
			m_ListCtl[IEXP_CurrentIndx].SetItemText(pLvdi->item.iItem, colnum, (LPCTSTR)String);

			if (colnum==0)	//the name of the item has been changed - update it.
			{
				// Get the Itemdata from the view list.
				IEXP_ItemDefPtr=(IEXP_ItemDef *)m_ListCtl[IEXP_CurrentIndx].GetItemData(pLvdi->item.iItem);

				// Reset the name in the view list data pointer.
				IEXP_ItemDefPtr->setName(String);


				strcpy(IEXP_EntityCol[IEXP_CurrentIndx].CurrentName, (LPCTSTR)String);
			}

			// Set the edit flag to false.
			m_Explorer->m_editmode=FALSE;
			m_Newflag=(m_Newflag)?FALSE:m_Newflag;
			if (bNeedRegen && m_Explorer->m_regenMode)
				cmd_regenall();
			if (bNeedRedraw)
				sds_redraw(NULL,IC_REDRAW_DRAW);

			//set the document modified flag, so that these changes will get saved
			struct resbuf rb;
			SDS_getvar(NULL,DB_QDBMOD,&rb,IEXP_ODNode->m_dbDrawing,&SDS_CURCFG,&SDS_CURSES);
			rb.resval.rint |= IC_DBMOD_ENTITIES_MODIFIED;
			SDS_setvar(NULL,DB_QDBMOD,&rb,IEXP_ODNode->m_dbDrawing,&SDS_CURCFG,&SDS_CURSES,0);
			IEXP_ODNode->m_pIcadDoc->SetModifiedFlag(TRUE);
			break;

		case LVN_COLUMNCLICK:
			// The user clicked on one of the column headings - sort by this column.
			IEXP_sorttype=(IEXP_sorttype)?FALSE:TRUE;
			IEXP_Sortitem=(pNm->iSubItem);
			// Modified CyberAge AP 09/04/2000[
			UpdateDataList1(TRUE);
			m_ListCtl[IEXP_CurrentIndx].SortItems(ListViewCompareProc,(LPARAM)(pNm->iSubItem));
			UpdateDataList1(FALSE);
			// Modified CyberAge AP 09/04/2000]
			break;
		default:
			break;
	}

	if (bReEdit)
		m_ListCtl[IEXP_CurrentIndx].EditSubLabel (pLvdi->item.iItem,pLvdi->item.iSubItem);
	return 0L;
}

///////////////////////////////////////////////////////////////////////
// This is the sorting function, whenever a column heading is clicked
// then this function is called by the (SORTITEMS) functoin.
//
// The comparison function must return a negative value if the first
// item should precede the second, a positive value if the first item
// should follow the second, or zero if the two items are equivalent.
// The lParam1 and lParam2 parameters specify the item data for the two
// items being compared. The lParamSort parameter is the same as the
// dwData value.
//
// A return value is the result of the compare.
//
int CALLBACK IEXP_CdetailView::ListViewCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) {
	int 				 iResult=0,
						 iResultX;
	IEXP_ItemDef *itemptr;
	// Modified CyberAge AP 09/22/2000[
	// Reason : Fix to slow explorer problem
	// Previously it was taking more than 60sec. to explore 4000 layers,
	// now it is taking only 4.5 sec.

	IEXP_ItemDef *itemptr1;
	struct resbuf		*rb;
	double diff;

	itemptr1	=(IEXP_ItemDef *)lParam1;
	itemptr 	   =(IEXP_ItemDef *)lParam2;

	switch(IEXP_EntityCol[IEXP_CurrentIndx].IType) {
		case IEXP_DSTAB_IDX:
			switch(lParamSort) {
				case 0:
					if (ic_assoc(itemptr1->m_rowPtr,2)) break; else rb=ic_rbassoc;
					if (ic_assoc(itemptr->m_rowPtr,2)) break;
					iResult=lstrcmp(rb->resval.rstring,ic_rbassoc->resval.rstring); break;
				default:
					iResult=0; break;
			}
			break;
		case IEXP_BNTAB_IDX:
			switch(lParamSort) {
				case 0:
					iResult=lstrcmp(itemptr1->name(),itemptr->name()); break;
				default:
					iResult=0; break;
			}
			break;
		case IEXP_VWTAB_IDX:
			switch(lParamSort) {
				case 0:
					if (ic_assoc(itemptr1->m_rowPtr,2)) break; else rb=ic_rbassoc;
					if (ic_assoc(itemptr->m_rowPtr,2)) break;
					iResult=lstrcmp(rb->resval.rstring,ic_rbassoc->resval.rstring); break;
				case 1:
					if (ic_assoc(itemptr1->m_rowPtr,40)) break; else rb=ic_rbassoc;
					if (ic_assoc(itemptr->m_rowPtr,40)) break;
					diff = rb->resval.rreal-ic_rbassoc->resval.rreal;
					if (icadRealEqual(diff,0.0)) { iResult = 0; break; }
					iResult = (diff > 0.0) ? -1 : 1; break;
				case 2:
					if (ic_assoc(itemptr1->m_rowPtr,41)) break; else rb=ic_rbassoc;
					if (ic_assoc(itemptr->m_rowPtr,41)) break;
					diff = rb->resval.rreal-ic_rbassoc->resval.rreal;
					if (icadRealEqual(diff,0.0)) { iResult = 0; break; }
					iResult = (diff > 0.0) ? -1 : 1; break;
				case 3:
				default:
					iResult=0; break;
			}
			break;
		case IEXP_CSTAB_IDX:
			switch(lParamSort) {
				case 0:
					if (ic_assoc(itemptr1->m_rowPtr,2)) break; else rb=ic_rbassoc;
					if (ic_assoc(itemptr->m_rowPtr,2)) break;
					iResult=lstrcmp(rb->resval.rstring,ic_rbassoc->resval.rstring); break;
				default:
					iResult=0; break;
			}
			break;
		case IEXP_TSTAB_IDX:
			switch(lParamSort) {
				case IEXP_TS_NAME:
					if (ic_assoc(itemptr1->m_rowPtr,2)) break; else rb=ic_rbassoc;
					if (ic_assoc(itemptr->m_rowPtr,2)) break;
					iResult=lstrcmp(rb->resval.rstring,ic_rbassoc->resval.rstring); break;
				case IEXP_TS_HEIGHT:
					if (ic_assoc(itemptr1->m_rowPtr,40)) break; else rb=ic_rbassoc;
					if (ic_assoc(itemptr->m_rowPtr,40)) break;
					diff = rb->resval.rreal-ic_rbassoc->resval.rreal;
					if (icadRealEqual(diff,0.0)) { iResult = 0; break; }
					iResult = (diff > 0.0) ? -1 : 1; break;
				case IEXP_TS_WIDTH:
					if (ic_assoc(itemptr1->m_rowPtr,41)) break; else rb=ic_rbassoc;
					if (ic_assoc(itemptr->m_rowPtr,41)) break;
					diff = rb->resval.rreal-ic_rbassoc->resval.rreal;
					if (icadRealEqual(diff,0.0)) { iResult = 0; break; }
					iResult = (diff > 0.0) ? -1 : 1; break;
				case IEXP_TS_SLANT:
					if (ic_assoc(itemptr1->m_rowPtr,50)) break; else rb=ic_rbassoc;
					if (ic_assoc(itemptr->m_rowPtr,50)) break;
					diff = rb->resval.rreal-ic_rbassoc->resval.rreal;
					if (icadRealEqual(diff,0.0)) { iResult = 0; break; }
					iResult = (diff > 0.0) ? -1 : 1; break;
				case IEXP_TS_FONT:
// to do - finish this Need to sort properly by font name!	Can't use DXF 3 for TrueType fonts; need family name from eed instead.
					if (ic_assoc(itemptr1->m_rowPtr,3)) break; else rb=ic_rbassoc;
					if (ic_assoc(itemptr->m_rowPtr,3)) break;
					iResult=lstrcmp(rb->resval.rstring,ic_rbassoc->resval.rstring); break;
				case IEXP_TS_STYLE: // to do - finish this Need to sort properly by font style!
					break;
				case IEXP_TS_LANG: // to do - finish this Need to sort properly by font language!
					break;
				// EBATECH(CNBR) -[
				case IEXP_TS_BIGFONT:
					if (ic_assoc(itemptr1->m_rowPtr,4)) break; else rb=ic_rbassoc;
					if (ic_assoc(itemptr->m_rowPtr,4)) break;
					iResult=lstrcmp(rb->resval.rstring,ic_rbassoc->resval.rstring); break;
					break;
				// EBATECH(CNBR) ]-
				// EBATECH(CNBR) -[ 2003/5/15 Mistake Groupcode Backword/UpsideDown flag is 71.
				case IEXP_TS_BACKWORD:
					if (ic_assoc(itemptr1->m_rowPtr,71)) break; else rb=ic_rbassoc;
					if (ic_assoc(itemptr->m_rowPtr,71)) break;
					iResult=(rb->resval.rint & 2) - (ic_rbassoc->resval.rint & 2); break;
				case IEXP_TS_UPSIDEDOWN:
					if (ic_assoc(itemptr1->m_rowPtr,71)) break; else rb=ic_rbassoc;
					if (ic_assoc(itemptr->m_rowPtr,71)) break;
					iResult=(rb->resval.rint & 4) - (ic_rbassoc->resval.rint & 4); break;
				case IEXP_TS_VERTICAL:
					if (ic_assoc(itemptr1->m_rowPtr,70)) break; else rb=ic_rbassoc;
					if (ic_assoc(itemptr->m_rowPtr,70)) break;
					iResult=(rb->resval.rint & 4) - (ic_rbassoc->resval.rint & 4); break;
				default:
					iResult=0; break;
							break;
			}
			break;
		case IEXP_LTTAB_IDX:
			// This is for byblock and bylayer types.
			if (!lParam2) { iResult=1; break; }
			else if (!lParam1) { iResult=-1; break; }
			switch(lParamSort) {
				case 0:
					if (ic_assoc(itemptr1->m_rowPtr,2)) break; else rb=ic_rbassoc;
					if (ic_assoc(itemptr->m_rowPtr,2)) break;
					iResult=lstrcmp(rb->resval.rstring,ic_rbassoc->resval.rstring); break;
				case 1:
					if (ic_assoc(itemptr1->m_rowPtr,3)) break; else rb=ic_rbassoc;
					if (ic_assoc(itemptr->m_rowPtr,3)) break;
					iResult=lstrcmp(rb->resval.rstring,ic_rbassoc->resval.rstring); break;
				default:
					iResult=0; break;
			}
			break;
		case IEXP_LATAB_IDX:
			switch(lParamSort) {
				case IEXP_LA_NAME:
					iResult=lstrcmp(itemptr1->name(),itemptr->name()); break;
				case IEXP_LA_COLOR:
					if (ic_assoc(itemptr1->m_rowPtr,62)) break; else rb=ic_rbassoc;
					if (ic_assoc(itemptr->m_rowPtr,62)) break;

					iResult=abs(ic_rbassoc->resval.rint)-abs(rb->resval.rint);
					break;
				case IEXP_LA_LINETYPE:
					if (ic_assoc(itemptr1->m_rowPtr,6)) break; else rb=ic_rbassoc;
					if (ic_assoc(itemptr->m_rowPtr,6)) break;

					iResult=lstrcmp(rb->resval.rstring,ic_rbassoc->resval.rstring); break;
				case IEXP_LA_ON_OFF:
					if (ic_assoc(itemptr1->m_rowPtr,62)) break; else rb=ic_rbassoc;
					if (ic_assoc(itemptr->m_rowPtr,62)) break;

					iResult=(ic_rbassoc->resval.rint<0)?1:2;
					iResultX=(rb->resval.rint<0)?1:2;
					iResult-=iResultX; break;
				case IEXP_LA_LOCKED:
					if (ic_assoc(itemptr1->m_rowPtr,70)) break; else rb=ic_rbassoc;
					if (ic_assoc(itemptr->m_rowPtr,70)) break;

					iResult=(ic_rbassoc->resval.rint&IC_LAYER_LOCKED)-(rb->resval.rint&IC_LAYER_LOCKED); break;
				case IEXP_LA_FROZEN:
					if (ic_assoc(itemptr1->m_rowPtr,70)) break; else rb=ic_rbassoc;
					if (ic_assoc(itemptr->m_rowPtr,70)) break;

					iResult=(ic_rbassoc->resval.rint&IC_LAYER_FROZEN)-(rb->resval.rint&IC_LAYER_FROZEN); break;
				case IEXP_LA_VP_FREEZECUR:
					iResult=0; break;
				case IEXP_LA_VP_FREEZENEW:
					if (ic_assoc(itemptr1->m_rowPtr,70)) break; else rb=ic_rbassoc;
					if (ic_assoc(itemptr->m_rowPtr,70)) break;

					iResult=(ic_rbassoc->resval.rint&IC_LAYER_VPFROZEN)-(rb->resval.rint&IC_LAYER_VPFROZEN); break;
				case IEXP_LA_LINEWEIGHT:
					if (ic_assoc(itemptr1->m_rowPtr,370)) break; else rb=ic_rbassoc;
					if (ic_assoc(itemptr->m_rowPtr,370)) break;

					iResult=ic_rbassoc->resval.rint-rb->resval.rint; break;
				case IEXP_LA_PLOT:
					if (ic_assoc(itemptr1->m_rowPtr,290)) break; else rb=ic_rbassoc;
					if (ic_assoc(itemptr->m_rowPtr,290)) break;

					iResult=ic_rbassoc->resval.rint-rb->resval.rint; break;
				default:
					iResult=0; break;
			}
			break;
	}
// Modified CyberAge AP 09/22/2000]
	iResult*=((IEXP_sorttype)?-1:1);
	return(iResult);
}

///////////////////////////////////////////////////////////////////////
// This function handles all the messages sent to the (View class)
//
// message : Specifies the Windows message to be processed.
// wParam  : Provides additional information used in processing the
//			 message. The parameter value depends on the message.
// lParam  : Provides additional information used in processing the
//			 message. The parameter value depends on the message.
//
// a return code that WindowProc() windows function is returned.
//
LRESULT IEXP_CdetailView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) {
	CRect  rect;

	switch(message) {
//		  case WM_SETCURSOR:
//			  if (m_Explorer->m_pDragImage) {
//				  switch(HIWORD(lParam)) {
//					  case WM_LBUTTONUP:
//						//*** End the dragging of a menu item.
//						m_Explorer->m_pDragImage->EndDrag();
//						delete m_Explorer->m_pDragImage;
//	  					m_Explorer->m_pDragImage=NULL;
//						  ReleaseCapture();
//						  break;
//					  case WM_MOUSEMOVE:
//						  // Get the cursor position.
//						  GetCursorPos(&point);
//	  					// Transform the point to client coordinates of the tree control.
//						  CPoint cPt(point.x,point.y);
//						  m_ListCtl[IEXP_CurrentIndx].ScreenToClient(&cPt);
//						// Draw the item.
//						m_pDragImage->DragMove(cPt);
//						  m_pDragImage->DragShowNolock(TRUE);
//						  break;
//				  }
//			  }
//			  break;
		case IEXP_PROPERTY: PropertyDlg(); break;
		case IEXP_CHANGEROW:
			if (wParam>=600 && lParam>=600) break;
			if (wParam>=600) wParam=7;
			if (lParam>=600) lParam=7;
			ChangeWindow(wParam,lParam);
			switch(lParam) {
				case 0: // Layer was selected
					// Turn off the block toolbar.
					m_Explorer->BlockEnableM(FALSE);
					m_Explorer->m_BlkToolBar.ShowWindow(SW_HIDE);
					m_Explorer->m_LToolBar.ShowWindow(SW_SHOWNORMAL);
					// Turn on the layer toolbar.
					m_Explorer->LayerEnableM(FALSE); //turn it off first to fix a bug. If you switch from Drawing2 (layer) to Drawing 1(layer), if you don't turn off the layer toolbar, it will use Drawing2's settings - visretain could be wrong.
					m_Explorer->LayerEnableM(TRUE);
					// Disable the current button.
					m_Explorer->EnableTbB(&m_Explorer->m_MToolBar,EXP_CURRENT_M,TRUE);
					// Disable the world button.
					m_Explorer->EnableTbB(&m_Explorer->m_MToolBar,EXP_UCS_WORLD,FALSE);
					break;
				case 3: // Coordinate system was selected
					// Enable the world button
					m_Explorer->EnableTbB(&m_Explorer->m_MToolBar,EXP_UCS_WORLD,TRUE);
					// Disable the current button.
					m_Explorer->EnableTbB(&m_Explorer->m_MToolBar,EXP_CURRENT_M,TRUE);
					// Turn off the layer toolbar.
					m_Explorer->LayerEnableM(FALSE);
					m_Explorer->m_LToolBar.ShowWindow(SW_HIDE);
					// Turn off the block toolbar.
					m_Explorer->BlockEnableM(FALSE);
					m_Explorer->m_BlkToolBar.ShowWindow(SW_HIDE);
					break;
				case 4: // View was selected
					// Disable the world button.
					m_Explorer->EnableTbB(&m_Explorer->m_MToolBar,EXP_UCS_WORLD,FALSE);
					// Enable the current button.
					m_Explorer->EnableTbB(&m_Explorer->m_MToolBar,EXP_CURRENT_M,TRUE);
					// Turn off the layer toolbar.
					m_Explorer->LayerEnableM(FALSE);
					m_Explorer->m_LToolBar.ShowWindow(SW_HIDE);
					// Turn off the block toolbar.
					m_Explorer->BlockEnableM(FALSE);
					m_Explorer->m_BlkToolBar.ShowWindow(SW_HIDE);
					break;
				case 5: // Block was selected
					// Disable the world button.
					m_Explorer->EnableTbB(&m_Explorer->m_MToolBar,EXP_UCS_WORLD,FALSE);
					// Disable the current button.
					m_Explorer->EnableTbB(&m_Explorer->m_MToolBar,EXP_CURRENT_M,FALSE);
					// Turn off the layer toolbar.
					m_Explorer->LayerEnableM(FALSE);
					m_Explorer->m_LToolBar.ShowWindow(SW_HIDE);
					// Turn on the block toolbar.
					m_Explorer->BlockEnableM(TRUE);
					m_Explorer->m_BlkToolBar.ShowWindow(SW_SHOWNORMAL);
					// Update the block state.
					m_Explorer->UpdBlockState((m_BlockState)?EXP_BLOCK_IMAGES:EXP_BLOCK_DETAILS);
					break;
				default: // Linetype,Style was selected.
					// Diable the world button.
					m_Explorer->EnableTbB(&m_Explorer->m_MToolBar,EXP_UCS_WORLD,FALSE);
					// Enable the current button.
					m_Explorer->EnableTbB(&m_Explorer->m_MToolBar,EXP_CURRENT_M,TRUE);
					// Turn off the layer toolbar.
					m_Explorer->LayerEnableM(FALSE);
					m_Explorer->m_LToolBar.ShowWindow(SW_HIDE);
					// Turn off the block toolbar.
					m_Explorer->BlockEnableM(FALSE);
					m_Explorer->m_BlkToolBar.ShowWindow(SW_HIDE);
					break;
			}
			if (m_DlgState) {
				m_EditDlg->m_dataptr=m_dataptr;
				::SendMessage(m_EditDlg->m_hWnd,IEXP_CHANGEROW,wParam,lParam);
			}
			m_Explorer->RecalcLayout();
			break;
		case IEXP_WM_NOTIFY:
			return(NotifyBlkHandler(message,wParam,lParam));
		case WM_NOTIFY:
			return(NotifyHandler(message,wParam,lParam));
	}
	return CFrameWnd::WindowProc(message, wParam, lParam);
}

///////////////////////////////////////////////////////////////////////
// This function is called when one of buttons on the layer toolbar is
// pressed.
//
// Id : This is the id of the button pressed.
//
// Nothing is returned.
//
void IEXP_CdetailView::BlkListType(UINT Id) {
	short index;
	CToolBarCtrl &TCtrl=m_Explorer->m_LToolBar.GetToolBarCtrl();

	switch(Id) {
		case EXP_BLOCK_IMAGES:
			for(index=0;index<IEXP_EntityCol[IEXP_CurrentIndx].Cnt;++index) {
				IEXP_EntityCol[IEXP_CurrentIndx].Order[index][1]=m_ListCtl[IEXP_CurrentIndx].GetColumnWidth(index);
			}
			m_BlockState=g_BlockState=1;
			m_Explorer->UpdBlockState(Id);
			break;
		case EXP_BLOCK_DETAILS:
			m_BlkSpecialflg=2;
			m_BlockState=g_BlockState=0;
			::SendMessage(IEXP_hWndView,IEXP_CHANGEROW,IEXP_CurrentIndx,IEXP_CurrentIndx);
			m_Explorer->UpdBlockState(Id);
			break;
		case EXP_BLOCK_XREF:
			Insertblock(2);
			m_Explorer->UpdBlockState(0);
			break;
		case EXP_BLOCK_FILE:
			Insertblock(1);
			m_Explorer->UpdBlockState(0);
			break;
		case EXP_BLOCK_INSERT:
			Insertblock(0);
			break;
		case EXP_BLOCK_SAVE:
			SaveWblock();
			break;
   }
   m_BlkSpecialflg=FALSE;
}

///////////////////////////////////////////////////////////////////////
// This function is called when one of buttons on the layer toolbar is
// pressed.
//
// Id : This is the id of the button pressed.
//
// Nothing is returned.
//
void IEXP_CdetailView::LayerAtt(UINT Id) {
	BOOL		   pflg;
	short		   SelCnt,
				   TotCnt,
				   value;
	struct resbuf *rbb=NULL,
				  *rbt=NULL,
				  *rbn=NULL;//rbv;
	int 		   Column=-1;
	CMenu		  *Menu,
				  *SubMenu;
	short		   updatetype;

	Menu=m_Explorer->GetMenu();
	SubMenu=Menu->GetSubMenu(1);
	CToolBarCtrl &TCtrl=m_Explorer->m_LToolBar.GetToolBarCtrl();
	if (Id>=EXP_LAYER_ON && Id<=EXP_LAYER_LOCK) {
		pflg=(TCtrl.IsButtonChecked(Id))?TRUE:FALSE;
		updatetype=1;
	} else {
		pflg=(MF_CHECKED==SubMenu->GetMenuState(Id,MF_BYCOMMAND))?FALSE:TRUE;
		updatetype=3;
	}
	//Added Cybage AJK 10/12/2001 [
	//Reason : Fix for bug no 77922
	if(Id==EXP_LAYER_VISRETAIN_M)
	{
		if(!m_ShowXrefLayers)
			ShowXrefLayers();
		else
			RemoveXrefLayers();
	}
	//Added Cybage AJK 10/12/2001 DD/MM/YYYY  ]
	if (!m_ListCtl[IEXP_CurrentIndx].GetSelectedCount()) return;
	TotCnt=(m_ListCtl[IEXP_CurrentIndx].GetItemCount()-1);
	for(m_ItemIndex=-1,SelCnt=0;SelCnt!=-1;SelCnt=m_ItemIndex) {
		if (m_Windowsideflag) ++m_ItemIndex;
		else if ((m_ItemIndex=m_ListCtl[IEXP_CurrentIndx].GetNextItem(m_ItemIndex,LVNI_SELECTED))==-1) break;
		if (m_ItemIndex>TotCnt) break;

		switch(Id)
			{
			case EXP_LAYER_ON:
			case EXP_LAYER_ON_M:
				Column = 3;
				break;
			case EXP_LAYER_FREEZE:
			case EXP_LAYER_FREEZE_M:
				Column = 5;
				break;
			case EXP_LAYER_LOCK:
			case EXP_LAYER_LOCK_M:
				Column = 4;
				break;
			}
		if (!m_ListCtl[IEXP_CurrentIndx].ValidateData (m_ItemIndex, Column)) continue;
		IEXP_ItemDefPtr=(IEXP_ItemDef *)m_ListCtl[IEXP_CurrentIndx].GetItemData(m_ItemIndex);
		rbn=SDS_tblgetter(IEXP_EntityCol[IEXP_CurrentIndx].cType,IEXP_ItemDefPtr->name(),TRUE,IEXP_ODNode->m_dbDrawing);
		if (!(rbt=BuildDataList(&rbb,RTDXF0,IEXP_LAYER)) || !(BuildDataList(&rbt,2,IEXP_ItemDefPtr->name()))) break;
		switch(Id) {
			case EXP_LAYER_ON:
			case EXP_LAYER_ON_M:
				ic_assoc(rbn,62);
				value=((pflg)?-(ic_rbassoc->resval.rint):abs(ic_rbassoc->resval.rint));
				if (!BuildDataList(&rbt,62,value)) break;
				sprintf(IEXP_szText,"%s"/*DNT*/,(pflg)?ResourceString(IDC_ICADEXPLORER_OFF_16, "Off" ):ResourceString(IDC_ICADEXPLORER_ON_17, "On" ));
				m_ListCtl[IEXP_CurrentIndx].SetItemText(m_ItemIndex,IEXP_EntityCol[IEXP_CurrentIndx].Order[3][IEXP_COL_ORDER],IEXP_szText);
				break;
			case EXP_LAYER_FREEZE:
			case EXP_LAYER_FREEZE_M:
/*				  IEXP_Findinfo.psz   =IEXP_EntityCol[IEXP_CurrentIndx].CurrentName;
				Index=m_ListCtl[IEXP_CurrentIndx].FindItem(&IEXP_Findinfo);
				if (Index==m_ItemIndex) {
					AfxMessageBox(ResourceString(IDC_ICADEXPLORER_CANNOT_FR_135, "Cannot freeze a current layer." ));
					TCtrl.CheckButton(EXP_LAYER_FREEZE,FALSE);
					continue;
				} else {
*/				ic_assoc(rbn,70);
				value=((pflg)?(ic_rbassoc->resval.rint|0x01):(ic_rbassoc->resval.rint^0x01));
				if (!BuildDataList(&rbt,70,value)) break;
				sprintf(IEXP_szText,"%s"/*DNT*/,(pflg)?ResourceString(IDC_IEXP_FROZEN_STR, "Frozen"):ResourceString(IDC_IEXP_THAWED_STR, "Thawed"));
				m_ListCtl[IEXP_CurrentIndx].SetItemText(m_ItemIndex,IEXP_EntityCol[IEXP_CurrentIndx].Order[5][IEXP_COL_ORDER],IEXP_szText);
 // 			  }
				break;
			case EXP_LAYER_LOCK:
			case EXP_LAYER_LOCK_M:
				ic_assoc(rbn,70);
				value=((pflg)?(ic_rbassoc->resval.rint|0x04):(ic_rbassoc->resval.rint^0x04));
				if (!BuildDataList(&rbt,70,value)) break;
				sprintf(IEXP_szText,"%s"/*DNT*/,(pflg)?ResourceString(IDC_IEXP_YES, "Yes"):ResourceString(IDC_IEXP_NO, "No"));
				m_ListCtl[IEXP_CurrentIndx].SetItemText(m_ItemIndex,IEXP_EntityCol[IEXP_CurrentIndx].Order[4][IEXP_COL_ORDER],IEXP_szText);
				break;
			case EXP_LAYER_VISRETAIN_M:
				m_ShowXrefLayers = !m_ShowXrefLayers;				
    				break;

		}
		if(IEXP_RegenOnUndo==FALSE) {
			SDS_SetUndo(IC_UNDO_REGEN_ON_UNDO,NULL,NULL,NULL,SDS_CURDWG);
			IEXP_RegenOnUndo=TRUE;
		}
		m_regenstate=TRUE;
		if (rbb) { SDS_tblmesser(rbb,IC_TBLMESSER_MODIFY,IEXP_ODNode->m_dbDrawing); sds_relrb(rbb); rbb=NULL; }
		if (rbn) { sds_relrb(rbn); rbn=NULL; }
		m_Explorer->UpdLayerState((long)IEXP_ItemDefPtr,updatetype);
	}
	//Added Cybage AJK 10/12/2001 [
	//Reason : Fix for bug no 77922
	if(Id==EXP_LAYER_VISRETAIN_M)
	{
		m_ShowXrefLayers = pflg ? true : false;
		if(!pflg) SubMenu->CheckMenuItem(EXP_LAYER_VISRETAIN_M,MF_UNCHECKED );
		else SubMenu->CheckMenuItem(EXP_LAYER_VISRETAIN_M,MF_CHECKED);
	}
	//Added Cybage AJK 10/12/2001 DD/MM/YYYY  ]

	if(m_regenstate && m_Explorer->m_regenMode)
	{
		cmd_regenall();
		m_regenstate = FALSE;
	}
}

///////////////////////////////////////////////////////////////////////
// This function is  called when the help button is pressed.
// This calls the help window.
//
// Nothing is returned.
//
void IEXP_CdetailView::Onhelp(void) {
	CommandRefHelp( m_hWnd, HLP_EXPLORER);
	return;
}

void IEXP_CdetailView::UpdateBlkCurrent() {
	long  style;

	style=GetWindowLong(m_ListCtl[IEXP_CurrentIndx].m_hWnd,GWL_STYLE);
	style^=LVS_REPORT;
	style|=LVS_ICON;
	SetWindowLong(m_ListCtl[IEXP_CurrentIndx].m_hWnd,GWL_STYLE,style);

	m_ListCtl[IEXP_CurrentIndx].UpdateWindow();
}

void IEXP_CdetailView::SaveWblock() {
	struct resbuf rb;
	char		  fs1[80];
	short		  fi1,
				  fi2;

	fi2=(m_ListCtl[IEXP_CurrentIndx].GetItemCount()-1);
	if (fi2==-1) return;
	// Get the number of rows for the curently selected (TYPE).
	fi1=m_ListCtl[IEXP_CurrentIndx].GetSelectedCount();
	if (!fi1) return;

	for(;fi2!=-1;--fi2) {
		if (!m_ListCtl[IEXP_CurrentIndx].GetItemState(fi2,LVIS_SELECTED)) continue;
		IEXP_ItemDefPtr=(IEXP_ItemDef *)m_ListCtl[IEXP_CurrentIndx].GetItemData(fi2);
		// EBATECH(CNBR) 2002/4/30 Disable TYPE button option+2
		if(RTNORM!=SDS_GetFiled(ResourceString(IDC_ICADEXPLORER_SAVE_BLOC_137, "Save Block" ),IEXP_ItemDefPtr->name(),ResourceString(IDC_ICADEXPLORER_STANDARD__138, "Standard Drawing File|dwg" ),513+2,&rb,MAKEINTRESOURCE(IDD_OPENSAVE_PREVIEW),cmd_OnOpenOK)) return;
		if(rb.restype==RTSTR) {	strcpy(fs1,rb.resval.rstring); IC_FREE(rb.resval.rstring);	}
		ic_setext(fs1,".dwg"/*DNT*/);
		cmd_wblock2disk(fs1,IEXP_ItemDefPtr->name());
	}
}

//
// mode : 0 Insert of block.
//		: 1
//		: 2 Insert a drawing as block - XREF
//
void IEXP_CdetailView::Insertblock(short mode) {
	short		   fi1;
   	LV_ITEM 	   lvI;

	lvI.mask	  = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
	lvI.state	  = 0;
	lvI.stateMask = 0;

	if (m_Explorer->m_MainWindow->m_ExpReturnHwnd) {
		AfxMessageBox(ResourceString(IDC_ICADEXPLORER_CAN_T_DO_T_33, "This operation cannot be performed.  Please run Explorer directly from the Tools menu to perform this operation."));
		return;
	}

	if(IEXP_ODNode->m_dbDrawing != SDS_CURDWG)
	{
		/*DG - 6.5.2002*/// we allow inserting blocks from another drawing now when mode == 0
		if(mode)
		{
			AfxMessageBox(ResourceString(IDC_ICADEXPLORER_CANNOT_IN_140, "Cannot insert a block in a non current drawing." ));
			return;
		}
		else
		{
			CopyCutItem(FALSE);
			for(IEXP_ODNode = IEXP_ODFirstNode; IEXP_ODNode->m_dbDrawing != SDS_CURDWG; IEXP_ODNode = IEXP_ODNode->next)
				;
			PasteItem();
		}
	}
	if (!mode) {
		fi1=m_ListCtl[IEXP_CurrentIndx].GetSelectedCount();
		if (!fi1) return;
	}
	switch(mode) {
		case 2:
			m_Explorer->m_MainWindow->m_bExplorerState=FALSE;
			m_Explorer->m_MainWindow->EndModalState();
			m_Explorer->ShowWindow(SW_HIDE);
			if(m_Explorer->m_regenMode == 0)
				cmd_regenall();

			SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)"._xref;_ATTACH"/*DNT*/);
			m_BlockInsCreate=FALSE;
			break;
		case 1:
			m_Explorer->m_MainWindow->m_bExplorerState=FALSE;
			m_Explorer->m_MainWindow->EndModalState();
			m_Explorer->ShowWindow(SW_HIDE);
			if(m_Explorer->m_regenMode == 0)
				cmd_regenall();

			SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)"._insert;_BROWSE"/*DNT*/);
			m_BlockInsCreate=FALSE;
			break;
		case 0:
			// Get the number of rows for the curently selected (TYPE).
			m_ItemIndex=(m_ListCtl[IEXP_CurrentIndx].GetItemCount()-1);
			for(;m_ItemIndex!=-1;--m_ItemIndex) {
				/*DG - 6.5.2002*/// compare also with the last pasted block name if any (see IEXP_CdetailView::PasteItem)
				IEXP_ItemDefPtr = (IEXP_ItemDef*)m_ListCtl[IEXP_CurrentIndx].GetItemData(m_ItemIndex);
				if( (m_pBlockName && !strisame(m_pBlockName, IEXP_ItemDefPtr->name())) ||
					(!m_pBlockName && !m_ListCtl[IEXP_CurrentIndx].GetItemState(m_ItemIndex, LVIS_SELECTED)) )
					continue;
				m_pBlockName = NULL;
				m_Explorer->m_MainWindow->m_bExplorerState=FALSE;
				m_Explorer->m_MainWindow->EndModalState();
				m_Explorer->ShowWindow(SW_HIDE);
				if(m_Explorer->m_regenMode == 0)
					cmd_regenall();

				sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER__INSERT__S_23, "_insert;%s" ),IEXP_ItemDefPtr->name());
				SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)IEXP_szText);
				//Uggh!  This global var needs to be the name of the block so that when Explorer returns,
				//it can update the number of entities.
				strncpy(IEXP_szText,IEXP_ItemDefPtr->name(),IC_ACADBUF);
				m_BlockInsCreate=TRUE;
				break;
			}
			break;
	}
	m_Explorer->m_MainWindow->m_bExpInsertState=TRUE;
	m_MessageType=IEXP_BNTAB_IDX;
}

// ======================================================================================================================================================================
// ================================================== IEXP_NEWDLG CLASS FUNCTIONS =======================================================================================
// ======================================================================================================================================================================

BEGIN_MESSAGE_MAP(IEXP_Newdlg,CDialog)
	ON_WM_ACTIVATE()
	ON_LBN_DBLCLK(EXP_DEL_LIST,OnDblClk)
END_MESSAGE_MAP()

void IEXP_Newdlg::OnActivate(UINT nState,CWnd* pWndOther,BOOL bMinimized ) {
	CDialog::OnActivate(nState,pWndOther,bMinimized);

//	  if (nState==WA_INACTIVE) DestroyWindow();
}

afx_msg void IEXP_Newdlg::OnDblClk() {
	int Index;

	Index=((CListBox *)m_ListWnd)->GetCurSel();
	((CListBox *)m_ListWnd)->GetText(Index,m_String);

	m_state=TRUE;
	DestroyWindow();
	return;
}

///////////////////////////////////////////////////////////////////////
// This function is called right before the new dialog is displayed, this
// new dialog is used for listing all available items of a specifed (TYPE)
// in a stand alone list box. This dialog is called by the (IEXP_MessItem)
// class.
//
// TRUE  : on success.
// FALSE : on error.
//
BOOL IEXP_Newdlg::OnInitDialog() {
	RECT  rect;
	long  Width,
		  Height;
	struct resbuf *rb;

	CDialog::OnInitDialog();

	GetWindowRect(&rect);
	Width=rect.right-rect.left; Height=rect.bottom-rect.top;
	rect.top=m_top; rect.left=m_left; rect.bottom=m_top+Height; rect.right=m_left+Width;
	MoveWindow(&rect);

	sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER_NEW__S_143, "New %s" ),m_type);
	SetDlgItemText(EXP_DEL_LISTTXT,IEXP_szText);

	m_ListWnd=GetDlgItem(EXP_DEL_LIST);
//	Cnt=IEXP_ODNode->m_dbDrawing->tabdata[IEXP_EntityCol[IEXP_CurrentIndx].IType].nrecs;

	switch(IEXP_EntityCol[IEXP_CurrentIndx].IType) {
		//Bugzilla No. 78163 ; 20-05-2002
		case IEXP_DSTAB_IDX:
		case IEXP_TSTAB_IDX:
		case IEXP_LTTAB_IDX:
		case IEXP_LATAB_IDX:
			rb=SDS_tblgetter(IEXP_EntityCol[IEXP_CurrentIndx].cType,NULL,TRUE,IEXP_ODNode->m_dbDrawing);
			while(rb) {
				if (ic_assoc(rb,2)) return(FALSE);
				if (!strisame(IEXP_ItemDefPtr->name(),ic_rbassoc->resval.rstring)) {
					((CListBox *)m_ListWnd)->AddString(ic_rbassoc->resval.rstring);
				}
				sds_relrb(rb);
				rb=SDS_tblgetter(IEXP_EntityCol[IEXP_CurrentIndx].cType,NULL,FALSE,IEXP_ODNode->m_dbDrawing);
			};
			break;
//		  case IEXP_LTTAB_IDX:
//			lttabt=(struct dwg_lttablink *)IEXP_ODNode->m_dbDrawing->tabdata[IEXP_EntityCol[IEXP_CurrentIndx].IType].tabarray;
//			  for(Index=0;Index<Cnt;++Index) ((CListBox *)m_ListWnd)->AddString(lttabt[Index].name);
//			  break;
//		  case IEXP_TSTAB_IDX:
//			tstabt=(struct dwg_tstablink *)IEXP_ODNode->m_dbDrawing->tabdata[IEXP_EntityCol[IEXP_CurrentIndx].IType].tabarray;
//			  for(Index=0;Index<Cnt;++Index) ((CListBox *)m_ListWnd)->AddString(tstabt[Index].name);
//			  break;
	}

	return(TRUE);
}

// ======================================================================================================================================================================
// ================================================== IEXP_MESSITEM CLASS FUNCTIONS =====================================================================================
// ======================================================================================================================================================================

BEGIN_MESSAGE_MAP(IEXP_MessItem,CDialog)
	ON_WM_ACTIVATE()
	ON_BN_CLICKED(EXP_DEL_CONTINUE,OnContinue)
	ON_BN_CLICKED(EXP_DEL_CANCEL,OnCancel)
	ON_BN_CLICKED(EXP_DEL_CHANGE,OnMove)
	ON_WM_WINDOWPOSCHANGED()
END_MESSAGE_MAP()

void IEXP_MessItem::OnActivate(UINT nState,CWnd* pWndOther,BOOL bMinimized ) {
	CDialog::OnActivate(nState,pWndOther,bMinimized);

	if (nState==WA_ACTIVE && m_Newdlg.m_state) EndDialog(EXP_DEL_CHANGE);
}

void IEXP_MessItem::DoDataExchange(CDataExchange *pDX) {
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX,EXP_DEL_TEXT1,m_txtmes);
	DDX_Text(pDX,EXP_DEL_CONTXT,m_txtcnt);
	DDX_Text(pDX,EXP_DEL_CHNGTXT,m_txtchng);
	DDX_Text(pDX,EXP_DEL_CANCEL_TXT,m_txtcancel);
}

BOOL IEXP_MessItem::OnInitDialog() {
//	  CWnd *PicWnd;

	CDialog::OnInitDialog();

	m_Newdlg.m_Dlgflag=FALSE;
	m_Newdlg.m_state=FALSE;
	SetWindowText(m_title);
//	  PicWnd=GetDlgItem(EXP_DEL_PICTURE);
//	  IEXP_SubPaintProc=(WNDPROC)SetWindowLong(PicWnd->m_hWnd,GWL_WNDPROC,(long)PaintProc);
	if (!m_Continue.IsEmpty()) SetDlgItemText(EXP_DEL_CHANGE,m_Continue);
	if (!m_BtnTwo.IsEmpty()) SetDlgItemText(EXP_DEL_CONTINUE,m_BtnTwo);

	return(TRUE);
}

void IEXP_MessItem::OnContinue() { EndDialog(EXP_DEL_CONTINUE); }
void IEXP_MessItem::OnCancel() { EndDialog(EXP_DEL_CANCEL); }

void IEXP_MessItem::OnMove() {
   RECT 	   rect;
   CWnd 	  *PicWnd;

   //Bugzilla No. 78163 ; 12-05-2003
   if (!m_typeflag) {
	   if ( !m_Newdlg )
	   {
	   PicWnd=GetDlgItem(EXP_DEL_CHANGE);
	   PicWnd->GetWindowRect(&rect);
	   m_Newdlg.m_top =rect.bottom;
	   m_Newdlg.m_left=rect.left;
	   m_Newdlg.Create(EXP_DEL_NEWLIST,this);
	   m_Newdlg.m_Dlgflag=TRUE;
	   }
	   m_Newdlg.ShowWindow(SW_SHOWNORMAL);	   
   } else EndDialog(EXP_DEL_CHANGE);
}

void IEXP_MessItem::OnWindowPosChanged(WINDOWPOS* lpwndpos) {
   RECT 	   rect,
			   rectsize;
   CWnd 	  *PicWnd;

   if (!m_typeflag && m_Newdlg.m_Dlgflag) {
	   PicWnd=GetDlgItem(EXP_DEL_CHANGE);
	   PicWnd->GetWindowRect(&rect);
	   m_Newdlg.GetWindowRect(&rectsize);
	   m_Newdlg.MoveWindow(rect.left,rect.bottom,(rectsize.right-rectsize.left),(rectsize.bottom-rectsize.top));
   }
}

// ======================================================================================================================================================================
// ================================================== IEXP_LTList CLASS FUNCTIONS ========================================================================================
// ======================================================================================================================================================================

BEGIN_MESSAGE_MAP(IEXP_LTList,CDialog)
	ON_BN_CLICKED(EXP_BROWSE2,OnBrowse2)
END_MESSAGE_MAP()

void IEXP_LTList::CleanupDialog()
{
	short	index,totcnt;
	struct IEXP_LTData *DataPtr;

	totcnt=Item->GetItemCount();
	for(index=0;index<totcnt;++index)
	{
		DataPtr = (struct IEXP_LTData *)Item->GetItemData(index);
		if (DataPtr)
		{
			if (DataPtr->Desc) delete DataPtr->Desc;
			if (DataPtr->Name) delete DataPtr->Name;
			delete DataPtr;
		}
	}
	return;
}


void IEXP_LTList::OnBrowse2()
{
	CleanupDialog();
	EndDialog(EXP_BROWSE2);
}

void IEXP_LTList::OnCancel()
{
	CleanupDialog();
	CDialog::OnCancel();
}

void IEXP_LTList::OnOK() {
	int 	index,
			fi1,
			totcnt;
	struct resbuf *tmprb=NULL;

	m_linetypes.Empty();
	totcnt=Item->GetItemCount();
	for(fi1=index=0;index<totcnt;++index) {
		if (!Item->GetItemState(index,LVIS_SELECTED)) continue;
		Item->GetItemText(index,0,IEXP_szText,IC_ACADBUF-1);
		do {
			if((tmprb=sds_tblsearch("LTYPE"/*DNT*/,IEXP_szText,0))==NULL) {
				m_ret=1;
				if (cmd_ltload(IEXP_szText,(char *)((LPCTSTR)m_Filename),0)==RTNORM) ++fi1;
				break;
			} else {
				IC_RELRB(tmprb);	// Fix Memory leak 2003/8/28
				m_ret=0;
				break;
				// Modified Cybage PP 23/02/2001 [
				// Reason: Fix for bug # 51277
				/*IC_RELRB(tmprb);
				if(strsame("CONTINUOUS", IEXP_szText))	//D.Gavrilov
				{									// Don't ask for replacing.
					m_ret = 2;
					break;
				}
				sprintf(IEXP_szTextA,ResourceString(IDC_ICADEXPLORER_REPLACE_L_144, "Replace linetype %s" ),IEXP_szText);
				if ((AfxMessageBox(IEXP_szTextA,MB_YESNO))==IDYES) {
					m_ret=2;
					if (cmd_ltload(IEXP_szText,(char *)((LPCTSTR)m_Filename),0)==RTNORM) ++fi1;
					break;
				} else { m_ret=0; break; }*/
				// Modified Cybage PP 23/02/2001 ]
			}
		} while(m_ret);
		if (m_linetypes.IsEmpty()) {
			m_linetypes=IEXP_szText;
		} else {
			m_linetypes+=ResourceString(IDC_ICADEXPLORER___75, "," );
			m_linetypes+=IEXP_szText;
		}
	}
	CleanupDialog();
	EndDialog(IDOK);
}

LRESULT IEXP_LTList::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	CRect	rect;

	switch(message)
	{
		case WM_NOTIFY:
			return(ListNotifyHandler(message,wParam,lParam));
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

LRESULT IEXP_LTList::ListNotifyHandler(UINT message, WPARAM wParam, LPARAM lParam) {
	LV_DISPINFO   *pLvdi= (LV_DISPINFO *)lParam;
	NM_LISTVIEW   *pNm	= (NM_LISTVIEW *)lParam;
	LV_KEYDOWN	  *pnkd = (LV_KEYDOWN FAR *)lParam;
	short		   fi1,
				   index,
				   totcnt;
	CWnd		  *DlgItem;
	struct resbuf *tmprb=NULL;

	if (wParam!=EXP_LINELIST) return 0L;
	switch(pLvdi->hdr.code)	{
		case LVN_ITEMCHANGING:
			DlgItem=GetDlgItem(IDOK);
			DlgItem->EnableWindow(TRUE);
			break;
		case LVN_COLUMNCLICK:
			Item[0].SortItems(LtListProc,(LPARAM)(pNm->iSubItem));
			break;
		case NM_DBLCLK:
			m_linetypes.Empty();
			totcnt=Item->GetItemCount();
			for(fi1=index=0;index<totcnt;++index) {
				if (!Item->GetItemState(index,LVIS_SELECTED)) continue;
				Item->GetItemText(index,0,IEXP_szText,IC_ACADBUF-1);
				do {
					if((tmprb=sds_tblsearch("LTYPE"/*DNT*/,IEXP_szText,0))==NULL) {
						m_ret=1;
						if (cmd_ltload(IEXP_szText,(char *)((LPCTSTR)m_Filename),0)==RTNORM) ++fi1;
						break;
					} else {
						IC_RELRB(tmprb);
						sprintf(IEXP_szTextA,ResourceString(IDC_ICADEXPLORER_REPLACE_L_144, "Replace linetype %s" ),IEXP_szText);
						if ((AfxMessageBox(IEXP_szTextA,MB_YESNO))==IDYES) {
							m_ret=2;
							if (cmd_ltload(IEXP_szText,(char *)((LPCTSTR)m_Filename),0)==RTNORM) ++fi1;
							break;
						} else { m_ret=0; break; }
					}
				} while(m_ret);
				if (m_linetypes.IsEmpty()) {
					m_linetypes=IEXP_szText;
				}
			}
			CleanupDialog();
			EndDialog(fi1);
			break;
		default:
			break;
	}
	return 0L;
}

int CALLBACK IEXP_LTList::LtListProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) {
	char   *string1,
		   *string2;

	switch(lParamSort) {
		case 0:
			string1=((struct IEXP_LTData *)lParam1)->Name;
			string2=((struct IEXP_LTData *)lParam2)->Name;
			break;
		case 1:
			string1=((struct IEXP_LTData *)lParam1)->Desc;
			string2=((struct IEXP_LTData *)lParam2)->Desc;
			break;
	}
	return(lstrcmp(string1,string2));
}

BOOL IEXP_LTList::OnInitDialog() {
	FILE			   *lfilestream;
	int 				i,
						j,
						index;
	char				szNameDesc[IC_ACADBUF],
						szLineName[IC_ACADBUF];
	LV_ITEM 			lvI;
	struct IEXP_LTData *DataPtr;
	CWnd			   *DlgItem;

	CDialog::OnInitDialog();

	sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER_LINETYPE__145, "Linetype from file %s" ),m_Filename);
	SetWindowText(IEXP_szText);

	DlgItem=GetDlgItem(IDOK);
	//Check to make sure the file is readable
	if(_access(m_Filename,4)) {
		sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER_FILE__S_D_146, "File %s does not have read permissions." ),m_Filename);
		MessageBox(IEXP_szText,ResourceString(IDC_ICADEXPLORER_INTELLICAD_36, "IntelliCAD Explorer" ),MB_OK);
		return(TRUE);
	}
	//open the file in read mode
	if((lfilestream = fopen(m_Filename, "r"/*DNT*/)) == NULL ) {
		sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER_UNABLE_TO_148, "Unable to open file %s for reading." ),m_Filename);
		MessageBox(IEXP_szText,ResourceString(IDC_ICADEXPLORER_INTELLICAD_36, "IntelliCAD Explorer" ),MB_OK);
		return(TRUE);
	}
	Item=(CListCtrl *)GetDlgItem(EXP_LINELIST);
	index=0;
	Item->InsertColumn(0,ResourceString(IDC_ICADEXPLORER_LINETYPE__149, "Linetype Name" ),LVCFMT_LEFT,105);
	Item->InsertColumn(1,ResourceString(IDC_ICADEXPLORER_LINETYPE__150, "Linetype Description" ),LVCFMT_LEFT,270);
	lvI.mask=LVIF_TEXT | LVIF_PARAM;

	/*D.Gavrilov*/// We should add the CONTINUOUS linetype.
	sprintf(szLineName, "CONTINUOUS" /*DNT*/);
	sprintf(szNameDesc, "");
	if (NULL==(DataPtr=(struct IEXP_LTData *)new struct IEXP_LTData)) return(FALSE);
	if (NULL==(DataPtr->Name=(char *)new char[strlen(szLineName)+1])) return(FALSE);
	if (NULL==(DataPtr->Desc=(char *)new char[strlen(szNameDesc)+1])) return(FALSE);
	strcpy(DataPtr->Name,szLineName);
	strcpy(DataPtr->Desc,szNameDesc);
	lvI.lParam	   =(LPARAM)DataPtr;
	lvI.iItem	   =index;
	lvI.iSubItem   =0;
	lvI.pszText    =szLineName;
	Item->InsertItem(&lvI);
	Item->SetItemText(index,1,szNameDesc);
	index++;

	// read thru the lin file for the occurrence of the entered linetype
	while(ic_nfgets(IEXP_szText,IC_ACADBUF-1,lfilestream)) {
		if (IEXP_szText[0]=='*') {
			//get the description name
			for(i=1,j=0;(IEXP_szText[i]!=',')&&(IEXP_szText[i]!='\0');i++,j++){
				szLineName[j]=IEXP_szText[i];
			}
			szLineName[j]='\0';
			//get the line description
			for(i++,j=0;(IEXP_szText[i]!='\0');i++,j++){
				szNameDesc[j]=IEXP_szText[i];
			}
			szNameDesc[j]='\0';
			continue;
		} else if ((IEXP_szText[0]=='A')||(IEXP_szText[0]=='a')) {
			//get the pen up pen down description
			for(i=2,j=0;(IEXP_szText[i]!='\0');i++,j++);
		} else {
			continue;
		}

		if (NULL==(DataPtr=(struct IEXP_LTData *)new struct IEXP_LTData)) return(FALSE);
		if (NULL==(DataPtr->Name=(char *)new char[strlen(szLineName)+1])) return(FALSE);
		if (NULL==(DataPtr->Desc=(char *)new char[strlen(szNameDesc)+1])) return(FALSE);
		strcpy(DataPtr->Name,szLineName);
		strcpy(DataPtr->Desc,szNameDesc);

		lvI.lParam	   =(LPARAM)DataPtr;
		lvI.iItem	   =index;
		lvI.iSubItem   =0;
		lvI.pszText    =szLineName;
		Item->InsertItem(&lvI);
		Item->SetItemText(index,1,szNameDesc);
		++index;
	}
	fclose(lfilestream);
	DlgItem->EnableWindow(FALSE);

	return(TRUE);
}

// ======================================================================================================================================================================
// ================================================== IEXP_LTDLG CLASS FUNCTIONS ========================================================================================
// ======================================================================================================================================================================

BEGIN_MESSAGE_MAP(IEXP_LTDlg,IcadDialog)
	ON_EN_UPDATE(EXP_LTDEDIT,OnDefUpd)
	ON_BN_CLICKED(EXP_BROWSE,OnBrowse)
	ON_BN_CLICKED(IDHELP,OnHelp)
END_MESSAGE_MAP()

void IEXP_LTDlg::OnHelp() {
	CommandRefHelp( m_hWnd, HLP_EXPLAYERS);
}

void IEXP_LTDlg::OnBrowse() {
	 EndDialog(EXP_BROWSE);
}

BOOL IEXP_LTDlg::OnInitDialog() {
	CWnd *Item;

	IcadDialog::OnInitDialog();

	m_defstate=TRUE;
	Item=GetDlgItem(IDOK);
	Item->EnableWindow(FALSE);
	Item=GetDlgItem(EXP_LTDESCEDIT);
	Item->SetFocus();

	return(FALSE);
}

void IEXP_LTDlg::OnDefUpd() {
	CString  string;
	CWnd	*Item;
	int 	 length;

	GetDlgItemText(EXP_LTDEDIT,string);
	if (string.IsEmpty() && !m_defstate) {
		Item=GetDlgItem(IDOK);
		Item->EnableWindow(FALSE);
		m_defstate=TRUE;
	} else if (m_defstate) {
		Item=GetDlgItem(IDOK);
		Item->EnableWindow();
		m_defstate=FALSE;
	}
	if (!string.IsEmpty()) {
		if (string[0]=='-') {
			MessageBox(ResourceString(IDC_ICADEXPLORER_FIRST_VAL_151, "First value cannot be negative." ),ResourceString(IDC_ICADEXPLORER_INTELLICAD_36, "IntelliCAD Explorer" ),MB_ICONEXCLAMATION|MB_OK|MB_APPLMODAL);
			string.Empty();
			SetDlgItemText(EXP_LTDEDIT,string);
			return;
		}
		length=(string.GetLength()-1);
		if ((!isdigit((unsigned char) string[length])) && string[length]!='.' && string[length]!=',' && string[length]!='-') {
			string.SetAt(length,0);
			SetDlgItemText(EXP_LTDEDIT,string);
		}
	}
}

void IEXP_LTDlg::DoDataExchange(CDataExchange *pDX) {
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX,EXP_LTDESCEDIT,m_Desc);
	DDX_Text(pDX,EXP_LTDEDIT,m_Defstr);
}

// ======================================================================================================================================================================
// ================================================== IEXP_YYTOANOCAN CLASS FUNCTIONS ====================================================================================
// ======================================================================================================================================================================

BEGIN_MESSAGE_MAP(IEXP_YYTOANC ,CDialog)
	ON_BN_CLICKED(EXP_YES,OnYes)
	ON_BN_CLICKED(EXP_YESTOALL,OnYesToAll)
	ON_BN_CLICKED(EXP_NO,OnNo)
	ON_BN_CLICKED(ID_CANCEL,OnCanel)
END_MESSAGE_MAP()

void IEXP_YYTOANC::OnYes()		{ EndDialog(EXP_YES); }
void IEXP_YYTOANC::OnYesToAll() { EndDialog(EXP_YESTOALL); }
void IEXP_YYTOANC::OnNo()		{ EndDialog(EXP_NO); }
void IEXP_YYTOANC::OnCanel()	{ EndDialog(ID_CANCEL); }

BOOL IEXP_YYTOANC::OnInitDialog() {

	SetWindowText(ResourceString(IDC_ICADEXPLORER_CONFIRM_B_152, "Confirm Block Delete" ));
	if (m_bNested)
	{
		sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER_22, "The block %s is a nested block.  Are you sure you want to delete it?"),m_Blockname);
	}
	else
	{
		sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER_ARE_YOU_SU_68, "Are you sure you want to delete %s" ),m_Blockname);
	}

	SetDlgItemText(EXP_STATIC,IEXP_szText);

	return(TRUE);
}

// ======================================================================================================================================================================
// ================================================== IEXP_CLISTCTRL CLASS FUNCTIONS ====================================================================================
// ======================================================================================================================================================================

BEGIN_MESSAGE_MAP(IEXP_CListCtrl,CListCtrlEx)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

afx_msg void IEXP_CListCtrl::OnLButtonDblClk(UINT nFlags,CPoint point)
{
	int colnum = -1;
	if (nFlags==MK_LBUTTON)
	{
		if (HitTestEx(point, &colnum)!=-1)
		{
			if (colnum == 0)	//set current item only if user clicked on col 0
				m_CdetailView->CurrentItem();
			else
			{
				OnLButtonDown (nFlags,point);
				return;
			}
		}
	}

	CListCtrlEx::OnLButtonDblClk(nFlags,point);
}

afx_msg void IEXP_CListCtrl::OnLButtonDown (UINT nFlags,CPoint point)
	{
	int index;
	int colnum;
	int Color, OldColor;
	CStringList lstItems;

	CListCtrlEx::OnLButtonDown (nFlags,point);

	if (m_CdetailView->m_Explorer->m_editmode)
		EndLabelEditing();

	if( ( index = HitTestEx( point, &colnum )) != -1 )
		{
		UINT flag = LVIS_FOCUSED;
		if( !(GetItemState( index, flag ) & flag) )
			SetItemState( index, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

		if (colnum!=0)
			{
			if (!ValidateData (index, colnum))
				return;

			m_CdetailView->m_Explorer->m_editmode = TRUE; //set this to TRUE, since we are now going to edit
			}

		// Add check for LVS_EDITLABELS
		if(GetWindowLong(m_hWnd, GWL_STYLE) & LVS_EDITLABELS )
			{
			CString str1, str2;
			switch (IEXP_EntityCol[IEXP_CurrentIndx].ColEditType[colnum])
				{
				case COLEDIT_NONE:
					m_CdetailView->m_Explorer->m_editmode = FALSE;
					break;

				case COLEDIT_EDITBOX:
					m_CdetailView->m_Explorer->m_editmode = TRUE;
					EditSubLabel (index, colnum);
					break;

				case COLEDIT_COMBOBOX:
					m_CdetailView->m_Explorer->m_editmode = FALSE;
					break;

				case COLEDIT_FONTCOMBOBOX:
					switch (IEXP_EntityCol[IEXP_CurrentIndx].IType)
						{
						case IEXP_TSTAB_IDX:
							switch (colnum)
								{
								case IEXP_TS_FONT:					// font name
									{
									CString tmpFontName = GetItemText(index, colnum);
									tmpFontName.TrimLeft();
									if (!tmpFontName.IsEmpty())		// should never be empty
										{
										char fontName[IC_ACADBUF + 1];
										strncpy (fontName, GetItemText(index, colnum), IC_ACADBUF);
										ShowInPlaceFontNameCombo(index, colnum, fontName, (GET_FONT_TTF + GET_FONT_SHX));
										}
									else
										GetParent()->SetFocus();	// nothing here, shouldn't have focus!

									break;
									}

								default:
									ASSERT(0);										//should never be here
								}

							break;

						default:
							ASSERT(0);												//should never be here
						}
					break;

				case COLEDIT_STYLECOMBOBOX:
					switch (IEXP_EntityCol[IEXP_CurrentIndx].IType)
						{
						case IEXP_TSTAB_IDX:
							switch (colnum)
								{
								case IEXP_TS_STYLE:					// font style
									char fontStyle[IC_ACADBUF + 1];
									strncpy (fontStyle, GetItemText(index, colnum), IC_ACADBUF);
									if (!GetItemText(index, colnum).IsEmpty())
										{
										long stylenumber = 0;
										if (((CString) fontStyle) == "Italic")
											stylenumber += 0x01000000L;
										if (((CString) fontStyle) == "Bold")
											stylenumber += 0x02000000L;
										if (((CString) fontStyle) == "Bold Italic")
											stylenumber += 0x03000000L;
										CInPlaceFontStyleCombo* style = ShowInPlaceFontStyleCombo(index, colnum, stylenumber);
										if (style && style->FindStringExact(-1, fontStyle) != -1)
											style->SetCurSel(style->FindStringExact(-1, fontStyle));
										}
									else
										GetParent()->SetFocus();	// nothing here, shouldn't have focus!
									break;
								}

							break;

						default:
							ASSERT(0);												//should never be here
						}
					break;

					// EBATECH(CNBR) -[
				case COLEDIT_BIGFONTCOMBOBOX:
					switch (IEXP_EntityCol[IEXP_CurrentIndx].IType)
						{
						case IEXP_TSTAB_IDX:
							switch (colnum)
								{
								case IEXP_TS_BIGFONT:				// bigfont name
									{
									char fontName[IC_ACADBUF + 1];
									CString tmpFontName = GetItemText(index, colnum);

									tmpFontName.TrimLeft();
									strncpy (fontName, GetItemText(index, colnum), IC_ACADBUF);
									ShowInPlaceFontNameCombo(index, colnum, fontName, GET_BIGFONT_SHX);
									break;
									}

								default:
									ASSERT(0);										//should never be here
								}

							break;

						default:
							ASSERT(0);												//should never be here
						}
					break;

				case COLEDIT_LANGCOMBOBOX:
					switch (IEXP_EntityCol[IEXP_CurrentIndx].IType)
						{
						case IEXP_TSTAB_IDX:
							switch (colnum)
								{
								case IEXP_TS_LANG:							// font language (Not Yet)
									char fontLang[IC_ACADBUF + 1];
									strncpy (fontLang, GetItemText(index, colnum), IC_ACADBUF);
									if (!GetItemText(index, colnum).IsEmpty())
										{
										long stylenumber = 0;
										CInPlaceFontLangCombo* lang = ShowInPlaceFontLangCombo(index, colnum, stylenumber);
										if (lang && lang->FindStringExact(-1, fontLang) != -1)
											lang->SetCurSel(lang->FindStringExact(-1, fontLang));
										}
									else
										GetParent()->SetFocus();	// nothing here, shouldn't have focus!
									break;
								}

							break;

						default:
							ASSERT(0);												//should never be here
						}
					break;
					// EBATECH(CNBR) ]-
					// EBATECH(CNBR) -[ Layer|Lineweight
				case COLEDIT_LWCOMBOBOX:
					if(IEXP_EntityCol[IEXP_CurrentIndx].IType == IEXP_LATAB_IDX && colnum == IEXP_LA_LINEWEIGHT )
					{
						char LWeight[IC_ACADBUF + 1];
						strncpy(LWeight, GetItemText(index, colnum), IC_ACADBUF);
						if(!GetItemText(index, colnum).IsEmpty())
						{
							CInPlaceLWCombo* lw = ShowInPlaceLWCombo(index, colnum, IEXP_lwunits, 0, DB_DFWMASK);
							if (lw && lw->FindStringExact(-1, LWeight) != -1)
								lw->SetCurSel(lw->FindStringExact(-1, LWeight));
							else
								GetParent()->SetFocus();	// nothing here, shouldn't have focus!
							break;
						}
						break;
					}else{
						ASSERT(0);
					}
					break;
				case COLEDIT_DIALOG:
					switch (IEXP_EntityCol[IEXP_CurrentIndx].IType)
						{
						case IEXP_LATAB_IDX:
							if (colnum == IEXP_LA_COLOR)
								{
								strncpy (IEXP_szText, GetItemText(index, colnum), IC_ACADBUF);
								OldColor = ic_colornum((char *)IEXP_szText);

								if (SDS_GetColorDialog(OldColor,&Color,4)!=RTNORM)
									Color = OldColor;

								strncpy(IEXP_szText, (ic_colorstr (Color, NULL)), IC_ACADBUF);

								// Send Notification to parent of ListView ctrl
								LV_DISPINFO dispinfo;
								dispinfo.hdr.hwndFrom = m_hWnd;
								dispinfo.hdr.idFrom = GetDlgCtrlID();
								dispinfo.hdr.code = LVN_ENDLABELEDIT;
								dispinfo.item.mask = LVIF_TEXT;
								dispinfo.item.iItem = index;
								dispinfo.item.iSubItem = colnum;
								dispinfo.item.pszText = IEXP_szText;
								dispinfo.item.cchTextMax = strlen(IEXP_szText);
								GetParent()->SendMessage( WM_NOTIFY, GetDlgCtrlID(),(LPARAM)&dispinfo );
								}
							else if (colnum == IEXP_LA_LINETYPE)
								{
								short	Cnt, rindex=0;
								int		retval;
								char	*ptr1;
								struct resbuf *rbb, rb;
								IEXP_LTList    ltlst;
								LV_ITEM 	   lvI;
								lvI.mask	  =LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
								lvI.state	  =0;
								lvI.stateMask =0;
								//EBATECH(CNBR)-[ 2002/9/20 Default linetype file is dependent on $MEASUREMENT.
								int	measurement;
								SDS_getvar(NULL,DB_QMEASUREMENT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
								measurement = rb.resval.rint;
								//EBATECH(CNBR)]-
								do
									{
									*IEXP_szText=NULL;
									if (rindex)
										{
										// EBATECH(CNBR) 2002/4/30 Disable TYPE button option+2
										if ((sds_getfiled("Select Linetype File",IEXP_szText,"Linetype File|lin",2,&rb))==RTNORM)
											{
											strcpy(IEXP_szText,rb.resval.rstring);
											IC_FREE(rb.resval.rstring);
											}
										}
									else
									//EBATECH(CNBR)-[ 2002/9/20 Default linetype file is dependent on $MEASUREMENT.
										{
										sds_findfile( measurement == DB_ENGLISH_MEASUREINIT ?
											SDS_LTYPEFILE : SDS_ISOLTYPEFILE, IEXP_szText);
										//sds_findfile(SDS_LTYPEFILE,IEXP_szText);
										}
									//EBATECH(CNBR)]-
									if (*IEXP_szText == NULL)
										return;

									ltlst.m_Filename=IEXP_szText;
									if ((retval=ltlst.DoModal())==IDCANCEL)
										{
										ltlst.m_ret=0;
										return;
										}

									if (retval==EXP_BROWSE2)
										{
										rindex=1;
										continue;
										}

									break;
									}
								while(TRUE);

								//Add the linetype to the linetypes tab
								for(ptr1=strtok((char *)((LPCTSTR)ltlst.m_linetypes),",");ptr1;ptr1=strtok(NULL,","))
									{
									rbb=SDS_tblgetter(IEXP_EntityCol[IEXP_LTTAB_IDX].cType,ptr1,TRUE,IEXP_ODNode->m_dbDrawing);
									if (rbb)
										{
										if (ltlst.m_ret!=2)		//new style has to be inserted
											{
											Cnt=IEXP_LinetCnt;
											m_CdetailView->m_dataptr=NULL;
											//Bugzilla No. 78162 ; 20-05-2002 [
											if (ic_assoc(rbb,2) == 0)
											{
												IEXP_Findinfo.psz	= ic_rbassoc->resval.rstring;
												int Index=m_CdetailView->m_ListCtl[IEXP_LTTAB_IDX].FindItem(&IEXP_Findinfo);
												if ( Index == -1 )
												{
											// Insert a new row for the Linetype in the (CListCtrl).
											if (!m_CdetailView->LinetIns(&Cnt,0,IEXP_LTTAB_IDX,NULL,lvI,FALSE,rbb))
												return;
											}
												else
													Cnt = Index;
											}
											//Bugzilla No. 78162 ; 20-05-2002 ]
										}
										else	//existing style has to be retrieved
											{
											IEXP_Findinfo.psz	=IEXP_szText;
											Cnt=m_CdetailView->m_ListCtl[IEXP_LTTAB_IDX].FindItem(&IEXP_Findinfo);
											}

										sds_relrb(rbb);
										}
									}

								strncpy(IEXP_szText,m_CdetailView->m_ListCtl[IEXP_LTTAB_IDX].GetItemText(Cnt,0),IC_ACADBUF);

								// Send Notification to parent of ListView ctrl	to change the layer tab
								LV_DISPINFO dispinfo;
								dispinfo.hdr.hwndFrom = m_hWnd;
								dispinfo.hdr.idFrom = GetDlgCtrlID();
								dispinfo.hdr.code = LVN_ENDLABELEDIT;
								dispinfo.item.mask = LVIF_TEXT;
								dispinfo.item.iItem = index;
								dispinfo.item.iSubItem = colnum;
								dispinfo.item.pszText = IEXP_szText;
								dispinfo.item.cchTextMax = strlen(IEXP_szText);
								GetParent()->SendMessage( WM_NOTIFY, GetDlgCtrlID(),(LPARAM)&dispinfo );

								break;
								}
							else
								ASSERT(0);				//should never be here

							break;

						case IEXP_LTTAB_IDX:
							ASSERT(0);
							break;

						case IEXP_CSTAB_IDX:
						case IEXP_VWTAB_IDX:
							ASSERT(0);
							break;

						case IEXP_BNTAB_IDX:
							if (colnum == 3)
								{
								struct resbuf rb;
								char fs2[IC_ACADBUF];
								strncpy (fs2, GetItemText(index, colnum), IC_ACADBUF);

								if (*fs2 == NULL)
									break;	//this is not an xref, so do nothing

								if (strrchr(fs2,IC_SLASH))
									*(strrchr(fs2,IC_SLASH)+1) = 0;

 								// EBATECH {localize
 								//if(sds_getfiled("Insert Block",fs2,"Standard Drawing File|dwg,Drawing Exchange Format|dxf",0,&rb)==RTNORM)

								// EBATECH(CNBR) 2002/4/30 Disable TYPE button option+2
 								if(sds_getfiled(
 									ResourceString(IDC_INSERT_INSERT_BLOCK_4, "Insert Block" ),
 									fs2,
 									ResourceString(IDC_BLOCK_STANDARD_DRAWING__25, "Standard Drawing File|dwg,Drawing Exchange Format|dxf" ),
 									2,&rb)==RTNORM)
								//}
									{
									if(rb.restype==RTSTR && rb.resval.rstring)
										{
										strncpy(IEXP_szText, rb.resval.rstring, IC_ACADBUF);
										IC_FREE(rb.resval.rstring);
										}

									// Send Notification to parent of ListView ctrl
									LV_DISPINFO dispinfo;
									dispinfo.hdr.hwndFrom = m_hWnd;
									dispinfo.hdr.idFrom = GetDlgCtrlID();
									dispinfo.hdr.code = LVN_ENDLABELEDIT;
									dispinfo.item.mask = LVIF_TEXT;
									dispinfo.item.iItem = index;
									dispinfo.item.iSubItem = colnum;
									dispinfo.item.pszText = IEXP_szText;
									dispinfo.item.cchTextMax = strlen(IEXP_szText);
									GetParent()->SendMessage( WM_NOTIFY, GetDlgCtrlID(),(LPARAM)&dispinfo );
									}
								}
							else
								ASSERT(0);				//should never be here

							break;

						case IEXP_DSTAB_IDX:
						default:
							ASSERT(0);
							break;
						}
					break;

				case COLEDIT_TOGGLE:
					switch (IEXP_EntityCol[IEXP_CurrentIndx].IType)
						{
						case IEXP_LATAB_IDX:
							switch (colnum)
								{
								case IEXP_LA_ON_OFF:
									str1 = ResourceString(IDC_ICADEXPLORER_ON_17, "On");
									str2 = ResourceString(IDC_ICADEXPLORER_OFF_16, "Off");
									break;

								case IEXP_LA_LOCKED:
								case IEXP_LA_PLOT:
									str1 = ResourceString(IDC_IEXP_YES, "Yes");
									str2 = ResourceString(IDC_IEXP_NO, "No");
									break;

								case IEXP_LA_FROZEN:
								case IEXP_LA_VP_FREEZENEW:
								case IEXP_LA_VP_FREEZECUR:
									str1 = ResourceString(IDC_IEXP_FROZEN_STR, "Frozen");
									str2 = ResourceString(IDC_IEXP_THAWED_STR, "Thawed");
									break;

								default: // invalid - should never be here
									ASSERT(0);
									break;
								}
							break;

						case IEXP_TSTAB_IDX:
							switch (colnum)
								{
								// EBATECH(CNBR) Bugzilla#78471 2003/5/5 Explorer cannot switch vertical column
								case IEXP_TS_BACKWORD:
								case IEXP_TS_UPSIDEDOWN:
								case IEXP_TS_VERTICAL:
									str1 = ResourceString(IDC_IEXP_YES, "Yes");
									str2 = ResourceString(IDC_IEXP_NO, "No");
									break;
								default: // invalid - should never be here
									ASSERT(0);
									break;
								}
							break;

						case IEXP_LTTAB_IDX:
						case IEXP_CSTAB_IDX:
						case IEXP_VWTAB_IDX:
						case IEXP_BNTAB_IDX:
						case IEXP_DSTAB_IDX:
						default:	//invalid - should never be here
							ASSERT(0);
							break;
						}

						ToggleSubLabel (index, colnum, str1, str2);
						break;

				default:
					m_CdetailView->m_Explorer->m_editmode = FALSE;
					break;
				}
			}
		}
	}

//This function will find the active item in the current list control
//The active item is the item that has the check mark against it.
int IEXP_CListCtrl::GetActiveItem()
{
	int Index;
	int Item = -1;
	LV_ITEM lvi;
	lvi.mask	  =LVIF_IMAGE;
	lvi.state	  =LVIS_STATEIMAGEMASK;
	lvi.stateMask =LVIS_STATEIMAGEMASK;
	for (Index=0; Index<GetItemCount(); Index++)
	{
		lvi.iItem = Index;
		lvi.iSubItem = 0;
		if (GetItem(&lvi))
		{
			if (lvi.iImage==0)
			{
				Item = Index;
				break;
			}
		}
	}
	return Item;
}


//This function will validate the data being entered into the given row and column of the list control.
// returns TRUE if data is valid, and FALSE otherwise.
BOOL IEXP_CListCtrl::ValidateData (int nRow, int nCol)
{
	char name[IC_ACADBUF];
	strncpy (name, GetItemText(nRow, nCol), IC_ACADBUF);
	struct resbuf rb;


	if (strchr(name,IEXP_VER_BAR))
	{
		AfxMessageBox(ResourceString(IDC_ICADEXPLORER_23, "Cannot rename an XREF item."));
		return FALSE;
	}

	switch (IEXP_EntityCol[IEXP_CurrentIndx].IType)
	{
	case IEXP_LATAB_IDX:
		if (nCol == IEXP_LA_NAME)
		{
			if (!lstrcmp(name,"0"))
			{
				AfxMessageBox(ResourceString(IDC_ICADEXPLORER_CANNOT_REN_27, "Cannot rename layer 0. Specify a different layer."));
				return(FALSE);
			}
		}

		//if we have the currently active item
		if (GetActiveItem() == nRow)
		{
			if (nCol == IEXP_LA_FROZEN)
			{
				AfxMessageBox(ResourceString(IDC_ICADEXPLORER_CANNOT_FR_135, "Cannot freeze current layer. Specify a different layer."));
				return(FALSE);
			}
			if (nCol == IEXP_LA_ON_OFF)	
			{
				char text[4];
				strncpy (text, GetItemText(nRow, nCol), 4);
				if (strsame (text,ResourceString(IDC_ICADEXPLORER_ON_17, "On")))
					AfxMessageBox(ResourceString(IDC_ICADEXPLORER_1, "Warning!  You are turning off the current layer."));
				return(TRUE);
			}
		}

		if(SDS_getvar(NULL,DB_QTILEMODE,&rb,IEXP_ODNode->m_dbDrawing,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)
			break;

		if (rb.resval.rint == 1)
		{
			if ((nCol == IEXP_LA_VP_FREEZENEW) || (nCol == IEXP_LA_VP_FREEZECUR))
			{
				AfxMessageBox(ResourceString(IDC_ICADEXPLORER_2, "This property can only be set when working on a Layout tab. Switch to a Layout tab."));
				return (FALSE);
			}
		}


		break;
	case IEXP_LTTAB_IDX:
		if (nCol == 0)
		{
			if (!lstrcmpi(name,IEXP_BYBLOCK))
			{
				AfxMessageBox(ResourceString(IDC_ICADEXPLORER_CANNOT_REN_28, "Cannot rename linetype BYBLOCK."));
				return(FALSE);
			}
			else if (!lstrcmpi(name,IEXP_BYLAYER))
			{
				AfxMessageBox(ResourceString(IDC_ICADEXPLORER_CANNOT_REN_29, "Cannot rename linetype BYLAYER."));
				return(FALSE);
			}
			else if (!lstrcmpi(name,IEXP_CONTINUOUS))
			{
				AfxMessageBox(ResourceString(IDC_ICADEXPLORER_CANNOT_REN_30, "Cannot rename linetype CONTINUOUS."));
				return(FALSE);
			}
		}
		break;
	case IEXP_TSTAB_IDX:
		if (nCol == IEXP_TS_NAME)
		{
			if (!lstrcmpi(name,IEXP_STANDARD))
			{
				AfxMessageBox(ResourceString(IDC_ICADEXPLORER_CANNOT_REN_31, "Cannot rename Text style STANDARD. Specify a different Text style."));
				return(FALSE);
			}
		}
		break;
	case IEXP_CSTAB_IDX:
		if (nCol == 0)
		{
			if (!lstrcmp(name,IEXP_WORLD))
			{
				AfxMessageBox(ResourceString(IDC_ICADEXPLORER_CANNOT_REN_32, "Cannot rename UCS WORLD. Specify a different UCS."));
				return(FALSE);
			}
		}
		break;
	case IEXP_VWTAB_IDX:
		break;
	case IEXP_BNTAB_IDX:
		break;
	}
	return TRUE;

}


void IEXP_CListCtrl::DrawSubItem(int nItem, int nColumn, LV_COLUMN& lvc, CRect& rcCol, CDC *pDC)
{
	if ((IEXP_CurrentIndx == IEXP_LATAB_IDX) && (nColumn == 1 /*IEXP_COLOR*/))
	{
		int offset = pDC->GetTextExtent(_T(" "), 1 ).cx;

		rcCol.left = rcCol.right;
		rcCol.right += offset*8;
		lvc.cx -= offset*8;

		CRect rcColorBox = rcCol;
		rcColorBox.left += offset*2;
		rcColorBox.top += offset;
		rcColorBox.bottom -= offset;

		strncpy (IEXP_szText, GetItemText(nItem, nColumn), IC_ACADBUF);
		int Color = ic_colornum((char *)IEXP_szText);

		//Draw the Filled rectangle
		CPalette *savpal = pDC->SelectPalette(SDS_CMainWindow->m_pPalette,TRUE);

		CBrush	brush;
		brush.CreateSolidBrush(SDS_BrushColorFromACADColor(Color));
		pDC->FillRect(&rcColorBox, &brush);
		brush.DeleteObject();
		brush.CreateSolidBrush(RGB(0,0,0));
		pDC->FrameRect(&rcColorBox, &brush);
		brush.DeleteObject();

		pDC->SelectPalette(savpal,TRUE);

		//Draw the Text
		CListCtrlEx::DrawSubItem (nItem, nColumn, lvc, rcCol, pDC);
		return;
	}

	if ((IEXP_CurrentIndx == IEXP_LTTAB_IDX) && (nColumn == 2 /*IEXP_LINETYPE_DEF*/))
	{
		db_handitem *hip;
		CDC CmpDC;
		int offset = pDC->GetTextExtent(_T(" "), 1 ).cx;

		rcCol.left = rcCol.right;
		rcCol.right +=lvc.cx;
		rcCol.left += offset;
		rcCol.right -= offset;
		rcCol.top += offset;
		rcCol.bottom -= offset;


		// Calculate the length of the line to display.  To do this, we find the pattern length and muliply
		// it by the linetype scale.  Then multiply by 10, so we can show 10 reps of this pattern.
		struct resbuf *rb,*rbt,rbb;
		double size = 0.0;
		sds_point p1,p2;
		p1[0] = p1[1] = p1[2] = 0.0;
		p2[0] = p2[1] = p2[2] = 0.0;
		strncpy (IEXP_szText, GetItemText(nItem, 0), IC_ACADBUF);
		rb=SDS_tblgetter(IEXP_EntityCol[IEXP_LTTAB_IDX].cType,IEXP_szText,TRUE,IEXP_ODNode->m_dbDrawing);
		if (rb)
		{
			for(rbt=rb;rbt;rbt=rbt->rbnext)
			{
				if (rbt->restype==49)
					size += fabs(rbt->resval.rreal);
			}
			sds_relrb(rb);
		}

		if (icadRealEqual(size,0.0)) size = 1.0;

		SDS_getvar(NULL,DB_QLTSCALE,&rbb,IEXP_ODNode->m_dbDrawing,&SDS_CURCFG,&SDS_CURSES);
		if (icadRealEqual(rbb.resval.rreal,0.0))
			rbb.resval.rreal = 1.0;
		p2[0] = size * 10.0 * rbb.resval.rreal;

		//Create a line with the given linestyle
		strncpy (IEXP_szText, GetItemText(nItem, 0), IC_ACADBUF);
		hip=new db_line(IEXP_ODNode->m_dbDrawing);
		hip->set_10(p1);
		hip->set_11(p2);
		hip->set_6(IEXP_szText,IEXP_ODNode->m_dbDrawing);	//set linetype
		hip->set_62(248);									//set color to black

		//Draw the Filled rectangle
		CBitmap *Bitmap = SDS_DrawBitmap(IEXP_ODNode->m_dbDrawing,NULL,NULL,hip,rcCol.Width(),rcCol.Height(),FALSE);
		delete hip;

		if(CmpDC.CreateCompatibleDC(pDC))
		{
			CBitmap *OldBitmap=CmpDC.SelectObject(Bitmap);
			pDC->BitBlt(rcCol.left,rcCol.top,rcCol.Width(),rcCol.Height(),&CmpDC,0,0,SRCCOPY);
			delete CmpDC.SelectObject(OldBitmap);
		}
		return;
	}

	CListCtrlEx::DrawSubItem (nItem, nColumn, lvc, rcCol, pDC);
}


// ======================================================================================================================================================================
// ================================================== IEXP_EDITDLG CLASS FUNCTIONS ======================================================================================
// ======================================================================================================================================================================

BEGIN_MESSAGE_MAP(IEXP_EditDlg,CDialog)
	ON_WM_ACTIVATE()
	ON_WM_CLOSE()
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_BN_CLICKED(EXP_PROPHELP,OnHelpClick)
	ON_BN_CLICKED(EXP_PROPTACK,OnTackClick)
	ON_BN_DOUBLECLICKED(EXP_PROPTACK,OnTackDblClick)
END_MESSAGE_MAP()

int IEXP_EditDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CPaintDC dc(this);
	RECT	 rect;
	CString  String;
	SIZE	 size;
	short	 index,
			 entcnt;
	CMenu	*Menu;
	CWnd	*cwnd;

	m_SubDlg.m_dataptr	  =m_dataptr;
	m_SubDlg.m_oldindex   =IEXP_CurrentIndx;
	m_SubDlg.m_oldDlgIndex=-1;
	m_SubDlg.m_DlgIndex   =0;
	m_SubDlg.m_CdetailView=m_CdetailView;
	m_SubDlg.m_Inuse	  =FALSE;
	m_DlgIndex			  =0;

	// if this is the text style tab, make the container large enough to hold the tab
	if (IEXP_TSTAB_IDX == IEXP_EntityCol[IEXP_CurrentIndx].IType)
		{
		if (m_dataptr)
			{
			GetWindowRect(&rect);
			rect.bottom += IEXP_TEXT_STYLE_TAB_BOTTOM;
			rect.right += IEXP_TEXT_STYLE_TAB_RIGHT;
			MoveWindow(&rect);
			}

		m_SubDlg.m_inTextStyleProperties = true;		// need to track to make sure proper functions are called
		}
	if (IEXP_LATAB_IDX == IEXP_EntityCol[IEXP_CurrentIndx].IType) // 2003/3/31 EBATECH(CNBR) -[ LAYER Property also expand size.
		{
		if (m_dataptr)
			{
			GetWindowRect(&rect);
			rect.bottom += IEXP_LAYER_TAB_BOTTOM;
			rect.right += IEXP_LAYER_TAB_RIGHT;
			MoveWindow(&rect);
			}
		}

	for(entcnt=0;entcnt<(IEXP_NumofTypes-1);++entcnt)
	{
		GetClientRect(&rect);
		rect.bottom=rect.top+24; rect.top+=4; rect.left+=51; rect.right=rect.left;
		for(index=0;index<IEXP_TABINFO[entcnt].TabCnt;++index)
		{
			String=IEXP_TABINFO[entcnt].Tab[index];
			GetTextExtentPoint32(dc.m_ps.hdc,IEXP_TABINFO[entcnt].Tab[index],String.GetLength(),&size);
			IEXP_TABINFO[entcnt].TabTlen[index]=size.cx;
			rect.right+=size.cx;
			IEXP_TABINFO[entcnt].Tabrect[index].top   =rect.top;
			IEXP_TABINFO[entcnt].Tabrect[index].bottom=rect.bottom;
			IEXP_TABINFO[entcnt].Tabrect[index].left  =rect.left;
			IEXP_TABINFO[entcnt].Tabrect[index].right =rect.right;
			rect.left	=rect.right+2;
		}
	}
	if (!m_CdetailView->m_ListCtl[IEXP_CurrentIndx].GetSelectedCount()) m_dataptr=NULL;
	if (!m_SubDlg.Create(((m_dataptr)?IEXP_TABINFO[IEXP_CurrentIndx].TabID[0]:EXP_NOPROP),this)) { EndDialog(0); return(FALSE);	}

	m_SubDlg.ShowWindow(SW_SHOWNORMAL);
	m_SubDlg.GetWindowRect(&rect);
	ScreenToClient(&rect);
	if (m_dataptr) {
		for(entcnt=0;entcnt<(IEXP_NumofTypes-1);++entcnt) {
			for(index=0;index<IEXP_TABINFO[entcnt].TabCnt;++index) {
				IEXP_TABINFO[entcnt].Tabrect[index].bottom=(rect.top-6);
			}
		}
	}

	if (m_dataptr && IEXP_EntityCol[IEXP_CurrentIndx].IType==IEXP_LTTAB_IDX) {
		IEXP_ItemDefPtr=(IEXP_ItemDef *)m_dataptr;
		if ((cwnd=m_SubDlg.GetDlgItem(EXP_LINE_DEF))!=NULL) {
			if (strisame(IEXP_ItemDefPtr->name(),IEXP_CONTINUOUS) || strisame(IEXP_ItemDefPtr->name(),IEXP_BYLAYER) || strisame(IEXP_ItemDefPtr->name(),IEXP_BYBLOCK)) {
				cwnd->EnableWindow(FALSE);
			} else cwnd->EnableWindow(TRUE);
		}
	}

	// Make sure a right click will not bring up a menu.
	m_CdetailView->m_Explorer->m_editmode=TRUE;
	// Disable the menu.
	Menu=m_CdetailView->m_Explorer->GetMenu();
	Menu->EnableMenuItem(0,MF_BYPOSITION|MF_DISABLED|MF_GRAYED);
	Menu->EnableMenuItem(1,MF_BYPOSITION|MF_DISABLED|MF_GRAYED);
	Menu->EnableMenuItem(2,MF_BYPOSITION|MF_DISABLED|MF_GRAYED);
	// Turn off all the buttons on the layer toolbar.
	if (IEXP_CurrentIndx==0) {
		m_CdetailView->m_Explorer->EnableTbB(&m_CdetailView->m_Explorer->m_LToolBar,EXP_LAYER_ON,FALSE);
		m_CdetailView->m_Explorer->EnableTbB(&m_CdetailView->m_Explorer->m_LToolBar,EXP_LAYER_FREEZE,FALSE);
		m_CdetailView->m_Explorer->EnableTbB(&m_CdetailView->m_Explorer->m_LToolBar,EXP_LAYER_LOCK,FALSE);
	}
	// Turn off the buttons in the main toolbar.
	m_CdetailView->m_Explorer->EnableTbB(&m_CdetailView->m_Explorer->m_MToolBar,EXP_NEW_T,FALSE);
	m_CdetailView->m_Explorer->EnableTbB(&m_CdetailView->m_Explorer->m_MToolBar,EXP_CURRENT_M,FALSE);
	m_CdetailView->m_Explorer->EnableTbB(&m_CdetailView->m_Explorer->m_MToolBar,EXP_CUT,FALSE);
	m_CdetailView->m_Explorer->EnableTbB(&m_CdetailView->m_Explorer->m_MToolBar,EXP_COPY,FALSE);
	m_CdetailView->m_Explorer->EnableTbB(&m_CdetailView->m_Explorer->m_MToolBar,EXP_PASTE,FALSE);
	m_CdetailView->m_Explorer->EnableTbB(&m_CdetailView->m_Explorer->m_MToolBar,EXP_DELETE_M,FALSE);
	m_CdetailView->m_Explorer->EnableTbB(&m_CdetailView->m_Explorer->m_MToolBar,EXP_PROPERTIES,FALSE);
	m_CdetailView->m_Explorer->EnableTbB(&m_CdetailView->m_Explorer->m_MToolBar,EXP_UCS_WORLD,FALSE);
	m_CdetailView->m_Explorer->EnableTbB(&m_CdetailView->m_Explorer->m_MToolBar,EXP_PURGE,FALSE);
	return(CDialog::OnCreate(lpCreateStruct));
}

void IEXP_EditDlg::OnClose() {
	ShutdownDlg();
	CDialog::OnClose();
	return;
}

void IEXP_EditDlg::ShutdownDlg() {
	m_SubDlg.UpdateData(TRUE);
	m_SubDlg.SavePropertyValues();
	m_SubDlg.DestroyWindow();
	m_CdetailView->m_DlgState=FALSE; // m_DlgState is a flag set 1 = dialog is up, 0 = dialog is not up.
	m_CdetailView->m_DlgTack =FALSE; // m_DlgTack the toggle flag for the up and down states of the tack.
	CMenu	*Menu;

	// Set the flag to false so that a right click will bring up the menu.
	m_CdetailView->m_Explorer->m_editmode=FALSE;
	// Disable the menu.
	Menu=m_CdetailView->m_Explorer->GetMenu();
	Menu->EnableMenuItem(0,MF_BYPOSITION|MF_ENABLED);
	Menu->EnableMenuItem(1,MF_BYPOSITION|MF_ENABLED);
	Menu->EnableMenuItem(2,MF_BYPOSITION|MF_ENABLED);
	// Turn on all the buttons on the layer toolbar.
	if (IEXP_CurrentIndx==0) {
		m_CdetailView->m_Explorer->EnableTbB(&m_CdetailView->m_Explorer->m_LToolBar,EXP_LAYER_ON,TRUE);
		m_CdetailView->m_Explorer->EnableTbB(&m_CdetailView->m_Explorer->m_LToolBar,EXP_LAYER_FREEZE,TRUE);
		m_CdetailView->m_Explorer->EnableTbB(&m_CdetailView->m_Explorer->m_LToolBar,EXP_LAYER_LOCK,TRUE);
	}
	// Turn on the buttons in the main toolbar.
	m_CdetailView->m_Explorer->EnableTbB(&m_CdetailView->m_Explorer->m_MToolBar,EXP_NEW_T,TRUE);
	if (IEXP_CurrentIndx!=5) m_CdetailView->m_Explorer->EnableTbB(&m_CdetailView->m_Explorer->m_MToolBar,EXP_CURRENT_M,TRUE);
	m_CdetailView->m_Explorer->EnableTbB(&m_CdetailView->m_Explorer->m_MToolBar,EXP_CUT,TRUE);
	m_CdetailView->m_Explorer->EnableTbB(&m_CdetailView->m_Explorer->m_MToolBar,EXP_COPY,TRUE);
	m_CdetailView->m_Explorer->EnableTbB(&m_CdetailView->m_Explorer->m_MToolBar,EXP_PASTE,TRUE);
	m_CdetailView->m_Explorer->EnableTbB(&m_CdetailView->m_Explorer->m_MToolBar,EXP_DELETE_M,TRUE);
	m_CdetailView->m_Explorer->EnableTbB(&m_CdetailView->m_Explorer->m_MToolBar,EXP_PROPERTIES,TRUE);
	if (IEXP_CurrentIndx==3) m_CdetailView->m_Explorer->EnableTbB(&m_CdetailView->m_Explorer->m_MToolBar,EXP_UCS_WORLD,TRUE);
	m_CdetailView->m_Explorer->EnableTbB(&m_CdetailView->m_Explorer->m_MToolBar,EXP_PURGE,TRUE);
}

void IEXP_EditDlg::OnActivate(UINT nState,CWnd* pWndOther,BOOL bMinimized ) {
	if (nState==WA_INACTIVE && !m_SubDlg.m_Inuse) {
//		m_CdetailView->m_DlgState=FALSE;
//		m_CdetailView->m_DlgTack =FALSE;
//	} else if (nState==WA_INACTIVE && !m_SubDlg.m_Inuse) {
		if (m_CdetailView->m_DlgState && !m_CdetailView->m_DlgTack) {
//			  m_SubDlg.UpdateData(TRUE);
//			  m_SubDlg.SavePropertyValues();
//			  m_SubDlg.DestroyWindow();
//			m_CdetailView->m_DlgState=FALSE;
//			  m_CdetailView->m_DlgTack =FALSE;
			ShutdownDlg();
			EndDialog(0);
		}
	}
	CDialog::OnActivate(nState,pWndOther,bMinimized);
}

LRESULT IEXP_EditDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) {
	RECT	 rect;
	short	 index,
			 curidx;
	CString  cString;
	CWnd	*cwnd;
	int 	 Color;

	switch(message) {
		case WM_COMMAND:
			if (LOWORD(wParam)==IDC_BUTTON1) {
				m_SubDlg.m_Inuse=TRUE;
				if (SDS_GetColorDialog(m_SubDlg.m_color,&Color,4)==RTNORM) {
					if (IEXP_EntityCol[m_SubDlg.m_oldindex].IType==IEXP_LATAB_IDX) {
						if (Color==0 || Color==256) return(FALSE);
					}
					m_SubDlg.m_Inuse=FALSE;
					m_SubDlg.m_color=Color;
					m_SubDlg.m_colorVaries=FALSE;
					m_SubDlg.m_newcolor=TRUE;
					m_SubDlg.ColorButton(m_SubDlg.m_colorVaries);
				}
			} else if (wParam==1 && lParam==0) {
				return(FALSE);
			}
			break;
		case WM_LBUTTONDOWN:
			if (!m_dataptr) break;
			// EBATECH(CNBR) 2001-06-24 unsigned short -> long
			rect.right =(long)(short)LOWORD(lParam);
			rect.bottom=(long)(short)HIWORD(lParam);
			for(index=0;index<IEXP_TABINFO[IEXP_CurrentIndx].TabCnt;++index) {
				if (rect.right<IEXP_TABINFO[IEXP_CurrentIndx].Tabrect[index].right && rect.right>IEXP_TABINFO[IEXP_CurrentIndx].Tabrect[index].left &&
					rect.bottom>IEXP_TABINFO[IEXP_CurrentIndx].Tabrect[index].top && rect.bottom<IEXP_TABINFO[IEXP_CurrentIndx].Tabrect[index].bottom) {
					if (m_SubDlg.m_DlgIndex==index) break;
					m_SubDlg.UpdateData(TRUE);
					m_SubDlg.SavePropertyValues();

					m_SubDlg.m_oldDlgIndex=m_SubDlg.m_DlgIndex;
					m_SubDlg.m_DlgIndex=m_DlgIndex=index;
					m_SubDlg.DestroyWindow();
					m_SubDlg.Create(IEXP_TABINFO[IEXP_CurrentIndx].TabID[index],this);
					RedrawWindow();
					m_SubDlg.ShowWindow(SW_SHOWNORMAL);
					m_SubDlg.UpdateData(FALSE);

					if (m_dataptr && IEXP_EntityCol[IEXP_CurrentIndx].IType==IEXP_LATAB_IDX) {
						IEXP_ItemDefPtr=(IEXP_ItemDef *)m_dataptr;
						if ((cwnd=m_SubDlg.GetDlgItem(EXP_LINE_DEF))!=NULL) {
							if (strisame(IEXP_ItemDefPtr->name(),IEXP_CONTINUOUS) || strisame(IEXP_ItemDefPtr->name(),IEXP_BYLAYER) || strisame(IEXP_ItemDefPtr->name(),IEXP_BYBLOCK)) {
								cwnd->EnableWindow(FALSE);
							} else cwnd->EnableWindow(TRUE);
						}
					}
					break;
				}
			}
			break;
		case IEXP_DBLCLK:
			if (IEXP_EntityCol[IEXP_CurrentIndx].IType==IEXP_LTTAB_IDX) {
				if ((m_SubDlg.m_CItem=m_SubDlg.GetDlgItem(EXP_FREEZE))!=NULL) {
					curidx=m_CdetailView->m_ListCtl[IEXP_CurrentIndx].GetNextItem(-1,LVNI_SELECTED);
					cString=m_CdetailView->m_ListCtl[IEXP_CurrentIndx].GetItemText(curidx,0);
					if (strisame((char *)((LPCTSTR)cString),IEXP_EntityCol[IEXP_CurrentIndx].CurrentName)) {
						m_SubDlg.m_CItem->EnableWindow(FALSE);
					} else	{
						m_SubDlg.m_CItem->EnableWindow(TRUE);
					}
				}
			}
			break;
		case IEXP_CHANGINGITEM:
			m_SubDlg.UpdateData(TRUE);
			m_SubDlg.SavePropertyValues();
			break;
		case IEXP_CHANGEITEM:
			if ((lParam && (!m_SubDlg.m_dataptr || !m_dataptr)) || (!lParam && m_SubDlg.m_dataptr)) {
				m_SubDlg.DestroyWindow();
				m_SubDlg.m_dataptr =lParam;
				m_SubDlg.Create(((lParam)?IEXP_TABINFO[IEXP_CurrentIndx].TabID[0]:EXP_NOPROP),this);
				GetClientRect(&rect);
				InvalidateRect(&rect);
				m_SubDlg.ShowWindow(SW_SHOWNORMAL);
			} else m_SubDlg.m_dataptr=lParam;
			m_SubDlg.TableInfoConstruct();
			m_SubDlg.UpdateData(FALSE);
			m_SubDlg.ColorButton(FALSE);

			if (m_dataptr && IEXP_EntityCol[IEXP_CurrentIndx].IType==IEXP_LTTAB_IDX) {
				IEXP_ItemDefPtr=(IEXP_ItemDef *)m_SubDlg.m_dataptr;
				if ((cwnd=m_SubDlg.GetDlgItem(EXP_LINE_DEF))!=NULL) {
					if (strisame(IEXP_ItemDefPtr->name(),IEXP_CONTINUOUS) || strisame(IEXP_ItemDefPtr->name(),IEXP_BYLAYER) || strisame(IEXP_ItemDefPtr->name(),IEXP_BYBLOCK)) {
						cwnd->EnableWindow(FALSE);
					} else cwnd->EnableWindow(TRUE);
				}
			}
			break;
		case IEXP_CHANGEROW:
			SetWindowText(IEXP_TABINFO[IEXP_CurrentIndx].Name);
			m_SubDlg.m_DlgIndex=m_DlgIndex=0;
			m_SubDlg.m_oldDlgIndex=-1;
			m_SubDlg.DestroyWindow();
			m_SubDlg.m_dataptr =m_dataptr;
			m_SubDlg.m_oldindex=IEXP_CurrentIndx;
			m_SubDlg.Create(((m_dataptr)?IEXP_TABINFO[lParam].TabID[0]:EXP_NOPROP),this);
			GetClientRect(&rect);
			InvalidateRect(&rect);
			m_SubDlg.ShowWindow(SW_SHOWNORMAL);
			m_SubDlg.UpdateData(FALSE);

			if (m_dataptr && IEXP_EntityCol[IEXP_CurrentIndx].IType==IEXP_LTTAB_IDX) {
				IEXP_ItemDefPtr=(IEXP_ItemDef *)m_dataptr;
				if ((cwnd=m_SubDlg.GetDlgItem(EXP_LINE_DEF))!=NULL) {
					if (strisame(IEXP_ItemDefPtr->name(),IEXP_CONTINUOUS) || strisame(IEXP_ItemDefPtr->name(),IEXP_BYLAYER) || strisame(IEXP_ItemDefPtr->name(),IEXP_BYBLOCK)) {
						cwnd->EnableWindow(FALSE);
					} else cwnd->EnableWindow(TRUE);
				}
			}
			break;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

void IEXP_EditDlg::OnHelpClick() {
	switch(IEXP_EntityCol[IEXP_CurrentIndx].IType) {
		case IEXP_BNTAB_IDX:
			CommandRefHelp( m_hWnd, HLP_EXPBLOCKS);
			break;
		case IEXP_VWTAB_IDX:
			CommandRefHelp( m_hWnd, HLP_EXPVIEWS);
			break;
		case IEXP_CSTAB_IDX:
			CommandRefHelp( m_hWnd, HLP_EXPUCS);
			break;
		case IEXP_TSTAB_IDX:
			CommandRefHelp( m_hWnd, HLP_EXPFONTS);
			break;
		case IEXP_LTTAB_IDX:
			CommandRefHelp( m_hWnd, HLP_EXPLTYPES);
			break;
		case IEXP_LATAB_IDX:
			CommandRefHelp( m_hWnd, HLP_EXPLAYERS);
			break;
		case IEXP_DSTAB_IDX:
			CommandRefHelp( m_hWnd, HLP_EXPDIMS);
			break;
	}
}

void IEXP_EditDlg::OnTackClick() {
	m_CdetailView->m_DlgTack=(m_CdetailView->m_DlgTack)?FALSE:TRUE;
}

void IEXP_EditDlg::OnTackDblClick() {
	m_CdetailView->m_DlgTack=(m_CdetailView->m_DlgTack)?FALSE:TRUE;
}

BOOL IEXP_EditDlg::OnInitDialog() {
	long  nID;
	CWnd *hCurItem;

	for(hCurItem=this->GetWindow(GW_CHILD);hCurItem;hCurItem=hCurItem->GetNextWindow(GW_HWNDNEXT)) {
		nID=GetWindowLong(hCurItem->m_hWnd,GWL_ID);
		switch(nID) {
			case EXP_PROPTACK:
				ICAD_OrigWndProcT=(WNDPROC)SetWindowLong(hCurItem->m_hWnd,GWL_WNDPROC,(LONG)ICAD_Tbutton);
				hCurItem->SendMessage(ICAD_CUS_CREATE,0,0);
				break;
			case EXP_PROPHELP:
				ICAD_OrigWndProcH=(WNDPROC)SetWindowLong(hCurItem->m_hWnd,GWL_WNDPROC,(LONG)ICAD_Hbutton);
				break;
		}
	}
	return(TRUE);
}

void IEXP_EditDlg::OnPaint() {
	CPaintDC dc(this);
	RECT	 rect;
	CString  String;
	short	 index;
	short	 offset;

	dc.SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
	dc.SetBkMode(TRANSPARENT);

	GetClientRect(&rect);
	rect.left	+=5;
	rect.top	+=25;
	rect.right	-=5;
	rect.bottom -=5;
  	ICAD_BorderUp(rect,dc.m_ps.hdc);
	if (!m_dataptr) return;
	for(index=0;index<IEXP_TABINFO[IEXP_CurrentIndx].TabCnt;++index) {
		if (index!=m_DlgIndex) {
			ICAD_EraseBkGround(IEXP_TABINFO[IEXP_CurrentIndx].Tabrect[index],dc.m_ps.hdc);
			ICAD_TAB(IEXP_TABINFO[IEXP_CurrentIndx].Tabrect[index],dc.m_ps.hdc);
			String=IEXP_TABINFO[IEXP_CurrentIndx].Tab[index];
			offset=((IEXP_TABINFO[IEXP_CurrentIndx].Tabrect[index].right-
					 IEXP_TABINFO[IEXP_CurrentIndx].Tabrect[index].left)-IEXP_TABINFO[IEXP_CurrentIndx].TabTlen[index])/2;
			dc.TextOut((IEXP_TABINFO[IEXP_CurrentIndx].Tabrect[index].left+offset),
					   (IEXP_TABINFO[IEXP_CurrentIndx].Tabrect[index].top+4),String);
		}
	}

	rect.top	=IEXP_TABINFO[IEXP_CurrentIndx].Tabrect[m_DlgIndex].top-2;
	rect.bottom =IEXP_TABINFO[IEXP_CurrentIndx].Tabrect[m_DlgIndex].bottom+2;
	rect.left	=IEXP_TABINFO[IEXP_CurrentIndx].Tabrect[m_DlgIndex].left;
	rect.right	=IEXP_TABINFO[IEXP_CurrentIndx].Tabrect[m_DlgIndex].right+2;

	String=IEXP_TABINFO[IEXP_CurrentIndx].Tab[m_DlgIndex];
	ICAD_EraseBkGround(rect,dc.m_ps.hdc);
	ICAD_TAB(rect,dc.m_ps.hdc);
	offset=((rect.right-rect.left)-IEXP_TABINFO[IEXP_CurrentIndx].TabTlen[m_DlgIndex])/2;
	dc.TextOut((rect.left+offset),(rect.top+4),String);
}

// ======================================================================================================================================================================
// ================================================== IEXP_SUBDLG CLASS FUNCTIONS =======================================================================================
// ======================================================================================================================================================================
//
BEGIN_MESSAGE_MAP(IEXP_SubDlg,CDialog)
	ON_WM_PAINT()
END_MESSAGE_MAP()

/******************************************************************************
*+ IEXP_SubDlg::GetOldIndex - returns the value of m_oldindex
*
*/

inline short IEXP_SubDlg::GetOldIndex(
	void
	)
	{
	return m_oldindex;
	}

///////////////////////////////////////////////////////////////////////
// This function is called right before the subdialog of the edit dialog
// displays, the sub dialogs contains all the child items such as edit
// boxes,buttons,etc.
// The first part of this function subclasses the color button, and
// the data list.
// Theres quit a lot going on here after that to.
//
// TRUE  : on success.
// FALSE : on error.
//
BOOL IEXP_SubDlg::OnInitDialog() {
	LV_COLUMN  lvC;

	m_newcolor=TRUE;
	lvC.mask=LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvC.fmt	=LVCFMT_LEFT;
	if (!TableInfoConstruct()) return(FALSE);

	CDialog::OnInitDialog();
	return(TRUE);
}

BOOL IEXP_SubDlg::TableInfoConstruct()
{
	short		   index,
				   curidx,
				   fi1;
	CString 	   cString,
				   String;
	long		   lStyle;
	struct resbuf *rbb=NULL,
				  *rbt=NULL,
				  *rb=NULL,
				  *rbe;
	BOOL		   bflag,
				   bCurflag=FALSE;

	m_Vindex=m_CdetailView->m_ListCtl[IEXP_CurrentIndx].GetSelectedCount();
	if (!m_Vindex)
		return(FALSE);
	m_colorVaries=FALSE;
	m_varies=(m_Vindex>1)?TRUE:FALSE;
	for(fi1=0;fi1<15;++fi1)
		IEXP_TABINFO[IEXP_CurrentIndx].Varies[fi1]=FALSE;

	// This for loop goes through for every (ITEM) selected, if its one or if its many,
	// In each case a pointer is set to the first item selected,if there is more than
	// one (ITEM) selected then a different pointer is set to that (ITEM),then the items,
	// data is compared, if the values are different,Then a (varies) value is set in a
	// structure (IEXP_TABINFO), this structure holds all the dialog information
	// for all the (ITEM) types,such as keys,titles,etc..it also holds this (varies)
	// value witch in turn is used to figure out if there are many value for a certiain,
	// field.
	for(curidx=-1,index=0;index<m_Vindex;++index)
	{
		curidx=m_CdetailView->m_ListCtl[IEXP_CurrentIndx].GetNextItem(curidx,LVNI_SELECTED);
		cString=m_CdetailView->m_ListCtl[IEXP_CurrentIndx].GetItemText(curidx,0);
		if (curidx==-1) return(FALSE);
		switch(IEXP_EntityCol[IEXP_CurrentIndx].IType)
		{
		case IEXP_BNTAB_IDX:
			if (!index)
			{
				IC_RELRB(rbt);
				rbt=SDS_tblgetter(IEXP_EntityCol[IEXP_CurrentIndx].cType,(LPCTSTR)cString,TRUE,IEXP_ODNode->m_dbDrawing);
				continue;
			}
			IC_RELRB(rbb);
			rbb=SDS_tblgetter(IEXP_EntityCol[IEXP_CurrentIndx].cType,(LPCTSTR)cString,TRUE,IEXP_ODNode->m_dbDrawing);
			if (ic_assoc(rbt,10)) continue; else rb=ic_rbassoc;
			if (ic_assoc(rbb,10)) continue;
			IEXP_TABINFO[IEXP_CurrentIndx].Varies[1]=(ic_rbassoc->resval.rpoint[0]!=rb->resval.rpoint[0] ||
													  ic_rbassoc->resval.rpoint[1]!=rb->resval.rpoint[1] ||
													  ic_rbassoc->resval.rpoint[2]!=rb->resval.rpoint[2])?TRUE:IEXP_TABINFO[IEXP_CurrentIndx].Varies[1];
			break;

		case IEXP_VWTAB_IDX:
			if (!index)
			{
				IC_RELRB(rbt);
				rbt=SDS_tblgetter(IEXP_EntityCol[IEXP_CurrentIndx].cType,(LPCTSTR)cString,TRUE,IEXP_ODNode->m_dbDrawing);
				continue;
			}
			IC_RELRB(rbb);
			rbb=SDS_tblgetter(IEXP_EntityCol[IEXP_CurrentIndx].cType,(LPCTSTR)cString,TRUE,IEXP_ODNode->m_dbDrawing);
			if (ic_assoc(rbt,40)) continue; else rb=ic_rbassoc;
			if (ic_assoc(rbb,40)) continue;
			IEXP_TABINFO[IEXP_CurrentIndx].Varies[1]=(ic_rbassoc->resval.rreal!=rb->resval.rreal)?TRUE:IEXP_TABINFO[IEXP_CurrentIndx].Varies[1];

			if (ic_assoc(rbt,41)) continue; else rb=ic_rbassoc;
			if (ic_assoc(rbb,41)) continue;
			IEXP_TABINFO[IEXP_CurrentIndx].Varies[2]=(ic_rbassoc->resval.rreal!=rb->resval.rreal)?TRUE:IEXP_TABINFO[IEXP_CurrentIndx].Varies[2];

			if (ic_assoc(rbt,10)) continue; else rb=ic_rbassoc;
			if (ic_assoc(rbb,10)) continue;
			IEXP_TABINFO[IEXP_CurrentIndx].Varies[3]=(ic_rbassoc->resval.rpoint[0]!=rb->resval.rpoint[0] ||
													  ic_rbassoc->resval.rpoint[1]!=rb->resval.rpoint[1] ||
													  ic_rbassoc->resval.rpoint[2]!=rb->resval.rpoint[2])?TRUE:IEXP_TABINFO[IEXP_CurrentIndx].Varies[3];

			if (ic_assoc(rbt,50)) continue; else rb=ic_rbassoc;
			if (ic_assoc(rbb,50)) continue;
			IEXP_TABINFO[IEXP_CurrentIndx].Varies[4]=(ic_rbassoc->resval.rreal!=rb->resval.rreal)?TRUE:IEXP_TABINFO[IEXP_CurrentIndx].Varies[4];

			if (ic_assoc(rbt,42)) continue; else rb=ic_rbassoc;
			if (ic_assoc(rbb,42)) continue;
			IEXP_TABINFO[IEXP_CurrentIndx].Varies[5]=(ic_rbassoc->resval.rreal!=rb->resval.rreal)?TRUE:IEXP_TABINFO[IEXP_CurrentIndx].Varies[5];

			if (ic_assoc(rbt,43)) continue; else rb=ic_rbassoc;
			if (ic_assoc(rbb,43)) continue;
			IEXP_TABINFO[IEXP_CurrentIndx].Varies[6]=(ic_rbassoc->resval.rreal!=rb->resval.rreal)?TRUE:IEXP_TABINFO[IEXP_CurrentIndx].Varies[6];

			if (ic_assoc(rbt,44)) continue; else rb=ic_rbassoc;
			if (ic_assoc(rbb,44)) continue;
			IEXP_TABINFO[IEXP_CurrentIndx].Varies[7]=(ic_rbassoc->resval.rreal!=rb->resval.rreal)?TRUE:IEXP_TABINFO[IEXP_CurrentIndx].Varies[7];

			if (ic_assoc(rbt,71)) continue; else rb=ic_rbassoc;
			if (ic_assoc(rbb,71)) continue;
			IEXP_TABINFO[IEXP_CurrentIndx].Varies[8]=((ic_rbassoc->resval.rint&1)!=(rb->resval.rint&1))?TRUE:IEXP_TABINFO[IEXP_CurrentIndx].Varies[8];
			IEXP_TABINFO[IEXP_CurrentIndx].Varies[9]=((ic_rbassoc->resval.rint&2)!=(rb->resval.rint&2))?TRUE:IEXP_TABINFO[IEXP_CurrentIndx].Varies[9];
			IEXP_TABINFO[IEXP_CurrentIndx].Varies[10]=((ic_rbassoc->resval.rint&4)!=(rb->resval.rint&4))?TRUE:IEXP_TABINFO[IEXP_CurrentIndx].Varies[10];
			IEXP_TABINFO[IEXP_CurrentIndx].Varies[11]=((ic_rbassoc->resval.rint&8)!=(rb->resval.rint&8))?TRUE:IEXP_TABINFO[IEXP_CurrentIndx].Varies[11];
			IEXP_TABINFO[IEXP_CurrentIndx].Varies[12]=((ic_rbassoc->resval.rint&16)!=(rb->resval.rint&16))?TRUE:IEXP_TABINFO[IEXP_CurrentIndx].Varies[12];

			if (ic_assoc(rbt,12)) continue; else rb=ic_rbassoc;
			if (ic_assoc(rbb,12)) continue;
			IEXP_TABINFO[IEXP_CurrentIndx].Varies[13]=(ic_rbassoc->resval.rpoint[0]!=rb->resval.rpoint[0] ||
													   ic_rbassoc->resval.rpoint[1]!=rb->resval.rpoint[1] ||
													   ic_rbassoc->resval.rpoint[2]!=rb->resval.rpoint[2])?TRUE:IEXP_TABINFO[IEXP_CurrentIndx].Varies[13];

			if (ic_assoc(rbt,11)) continue; else rb=ic_rbassoc;
			if (ic_assoc(rbb,11)) continue;
			IEXP_TABINFO[IEXP_CurrentIndx].Varies[14]=(ic_rbassoc->resval.rpoint[0]!=rb->resval.rpoint[0] ||
													   ic_rbassoc->resval.rpoint[1]!=rb->resval.rpoint[1] ||
													   ic_rbassoc->resval.rpoint[2]!=rb->resval.rpoint[2])?TRUE:IEXP_TABINFO[IEXP_CurrentIndx].Varies[14];
			break;

		case IEXP_CSTAB_IDX:
			if (!index)
			{
 				IC_RELRB(rbt);
				rbt=SDS_tblgetter(IEXP_EntityCol[IEXP_CurrentIndx].cType,(LPCTSTR)cString,TRUE,IEXP_ODNode->m_dbDrawing);
				continue;
			}
			IC_RELRB(rbb);
			rbb=SDS_tblgetter(IEXP_EntityCol[IEXP_CurrentIndx].cType,(LPCTSTR)cString,TRUE,IEXP_ODNode->m_dbDrawing);
			if (ic_assoc(rbt,10)) continue; else rb=ic_rbassoc;
			if (ic_assoc(rbb,10)) continue;
			IEXP_TABINFO[IEXP_CurrentIndx].Varies[1]=(ic_rbassoc->resval.rpoint[0]!=rb->resval.rpoint[0] ||
													  ic_rbassoc->resval.rpoint[1]!=rb->resval.rpoint[1] ||
													  ic_rbassoc->resval.rpoint[2]!=rb->resval.rpoint[2])?TRUE:IEXP_TABINFO[IEXP_CurrentIndx].Varies[1];
			if (ic_assoc(rbt,11)) continue; else rb=ic_rbassoc;
			if (ic_assoc(rbb,11)) continue;
			IEXP_TABINFO[IEXP_CurrentIndx].Varies[2]=(ic_rbassoc->resval.rpoint[0]!=rb->resval.rpoint[0] ||
													  ic_rbassoc->resval.rpoint[1]!=rb->resval.rpoint[1] ||
													  ic_rbassoc->resval.rpoint[2]!=rb->resval.rpoint[2])?TRUE:IEXP_TABINFO[IEXP_CurrentIndx].Varies[2];
			if (ic_assoc(rbt,12)) continue; else rb=ic_rbassoc;
			if (ic_assoc(rbb,12)) continue;
			IEXP_TABINFO[IEXP_CurrentIndx].Varies[3]=(ic_rbassoc->resval.rpoint[0]!=rb->resval.rpoint[0] ||
													  ic_rbassoc->resval.rpoint[1]!=rb->resval.rpoint[1] ||
													  ic_rbassoc->resval.rpoint[2]!=rb->resval.rpoint[2])?TRUE:IEXP_TABINFO[IEXP_CurrentIndx].Varies[3];
			break;

		case IEXP_TSTAB_IDX:
			ASSERT(0);								// should be handled now by CExplorerPropsTab::GetTextStylesVariesInfo
			break;

		case IEXP_LTTAB_IDX:
			if(!m_dataptr)
				return TRUE;
			if(!index)
			{
				IC_RELRB(rbt);
				rbt = SDS_tblgetter(IEXP_EntityCol[IEXP_CurrentIndx].cType,(LPCTSTR)cString,TRUE,IEXP_ODNode->m_dbDrawing);
				continue;
			}
			IC_RELRB(rbb);
			rbb = SDS_tblgetter(IEXP_EntityCol[IEXP_CurrentIndx].cType,(LPCTSTR)cString,TRUE,IEXP_ODNode->m_dbDrawing);
			if(ic_assoc(rbt,3))
				continue;
			else rb = ic_rbassoc;
			if(ic_assoc(rbb,3))
				continue;
			IEXP_TABINFO[IEXP_CurrentIndx].Varies[1] = (stricmp(ic_rbassoc->resval.rstring,rb->resval.rstring))?TRUE:IEXP_TABINFO[IEXP_CurrentIndx].Varies[1];
			bflag = TRUE;
			if (ic_assoc(rbt,73))
				continue;
			else
				rb = ic_rbassoc;
			if(ic_assoc(rbb,73))
				continue;
			if(ic_rbassoc->resval.rint != rb->resval.rint)
				bflag=FALSE;
			for(rbe = rbt; rbe; rbe = rbe->rbnext)
			{
				switch(rbe->restype)
				{
				case 49:
					for(rb = rbb; rb; rb = rb->rbnext)
					{
						switch(rb->restype)
						{
							case 49:
								if (rb->resval.rint != rbe->resval.rint)
									bflag = FALSE;
								break;
						}
						if(!bflag)
							break;
					}
					break;
				}
				if(!bflag)
					break;
			}
			IEXP_TABINFO[IEXP_CurrentIndx].Varies[2]=(!bflag)?TRUE:IEXP_TABINFO[IEXP_CurrentIndx].Varies[2];
			break;

		case IEXP_LATAB_IDX:
			// Check layer name
			if (!bCurflag)
				bCurflag=(strsame(IEXP_EntityCol[IEXP_CurrentIndx].CurrentName,(LPCTSTR)cString))?TRUE:FALSE;
			if (!index)
			{
				IC_RELRB(rbt);
				rbt=SDS_tblgetter(IEXP_EntityCol[IEXP_CurrentIndx].cType,(LPCTSTR)cString,TRUE,IEXP_ODNode->m_dbDrawing);
				continue;
			}
			// Get Layer properties
			// DXF=62 Color and ON/OFF
			rbb=SDS_tblgetter(IEXP_EntityCol[IEXP_CurrentIndx].cType,(LPCTSTR)cString,TRUE,IEXP_ODNode->m_dbDrawing);
			if (ic_assoc(rbt,62)) continue; else rb=ic_rbassoc;
			if (ic_assoc(rbb,62)) continue;
			IEXP_TABINFO[IEXP_CurrentIndx].Varies[IEXP_PLA_COLOR]=(ic_rbassoc->resval.rint!=rb->resval.rint)?TRUE:IEXP_TABINFO[IEXP_CurrentIndx].Varies[IEXP_PLA_COLOR];
			IEXP_TABINFO[IEXP_CurrentIndx].Varies[IEXP_PLA_ON_OFF]=(((ic_rbassoc->resval.rint<0) && (rb->resval.rint>0)) ||
													  ((ic_rbassoc->resval.rint>0) && (rb->resval.rint<0)))?TRUE:IEXP_TABINFO[IEXP_CurrentIndx].Varies[IEXP_PLA_ON_OFF];
			// DXF=6 Linetype
			if (ic_assoc(rbt,6)) continue; else rb=ic_rbassoc;
			if (ic_assoc(rbb,6)) continue;
			IEXP_TABINFO[IEXP_CurrentIndx].Varies[IEXP_PLA_LINETYPE]=(stricmp(ic_rbassoc->resval.rstring,rb->resval.rstring))?TRUE:IEXP_TABINFO[IEXP_CurrentIndx].Varies[IEXP_PLA_LINETYPE];
			// DXF=70 +1=Freeze/Thaw +2=NewVPFreeze/Thaw +4=Lock/Unlock
			if (ic_assoc(rbt,70)) continue; else rb=ic_rbassoc;
			if (ic_assoc(rbb,70)) continue;
			IEXP_TABINFO[IEXP_CurrentIndx].Varies[IEXP_PLA_FROZEN]=((ic_rbassoc->resval.rint&IC_LAYER_FROZEN)!=(rb->resval.rint&IC_LAYER_FROZEN))?TRUE:IEXP_TABINFO[IEXP_CurrentIndx].Varies[IEXP_PLA_FROZEN];
			IEXP_TABINFO[IEXP_CurrentIndx].Varies[IEXP_PLA_LOCKED]=((ic_rbassoc->resval.rint&IC_LAYER_LOCKED)!=(rb->resval.rint&IC_LAYER_LOCKED))?TRUE:IEXP_TABINFO[IEXP_CurrentIndx].Varies[IEXP_PLA_LOCKED];
			IEXP_TABINFO[IEXP_CurrentIndx].Varies[IEXP_PLA_VPNEW]=((ic_rbassoc->resval.rint&IC_LAYER_VPFROZEN)!=(rb->resval.rint&IC_LAYER_VPFROZEN))?TRUE:IEXP_TABINFO[IEXP_CurrentIndx].Varies[IEXP_PLA_VPNEW];
			// DXF=370 Lineweight
			if (ic_assoc(rbt,370)) continue; else rb=ic_rbassoc;
			if (ic_assoc(rbb,370)) continue;
			IEXP_TABINFO[IEXP_CurrentIndx].Varies[IEXP_PLA_LINEWEIGHT]=(ic_rbassoc->resval.rint!=rb->resval.rint)?TRUE:IEXP_TABINFO[IEXP_CurrentIndx].Varies[IEXP_PLA_LINEWEIGHT];
			// DXF=290 Plottable
			if (ic_assoc(rbt,290)) continue; else rb=ic_rbassoc;
			if (ic_assoc(rbb,290)) continue;
			IEXP_TABINFO[IEXP_CurrentIndx].Varies[IEXP_PLA_PLOT]=(ic_rbassoc->resval.rint!=rb->resval.rint)?TRUE:IEXP_TABINFO[IEXP_CurrentIndx].Varies[IEXP_PLA_PLOT];
			break;
		}
	}

	switch(IEXP_EntityCol[IEXP_CurrentIndx].IType)
	{
		case IEXP_BNTAB_IDX:
			break;

		case IEXP_VWTAB_IDX:
			for(fi1=8;fi1<13;++fi1)
			{
				if (IEXP_TABINFO[IEXP_CurrentIndx].Varies[fi1])
				{
					m_CItem=GetDlgItem(IEXP_TABINFO[m_oldindex].ItemID[fi1]);
					if (m_CItem)
						{
						lStyle=GetWindowLong(m_CItem->m_hWnd,GWL_STYLE);
						lStyle=BS_AUTO3STATE|WS_CHILD|WS_VISIBLE|WS_TABSTOP;
						SetWindowLong(m_CItem->m_hWnd,GWL_STYLE,lStyle);
						}
				}
			}
			break;

		case IEXP_CSTAB_IDX:
			break;

		case IEXP_TSTAB_IDX:
			ASSERT(0);								// should be handled now by CExplorerPropsTab::GetTextStylesVariesInfo
			break;

		case IEXP_LTTAB_IDX:
			break;

		case IEXP_LATAB_IDX:
			m_CItem=GetDlgItem(EXP_FREEZE);
			IC_RELRB(rbb);
			rbb=SDS_tblgetter(IEXP_EntityCol[IEXP_LATAB_IDX].cType,(LPCTSTR)cString,TRUE,IEXP_ODNode->m_dbDrawing);
			for(fi1=IEXP_PLA_ON_OFF;fi1<=IEXP_PLA_VPNEW;++fi1)
			{
				if (IEXP_TABINFO[IEXP_CurrentIndx].Varies[fi1])
				{
					m_CItem=GetDlgItem(IEXP_TABINFO[m_oldindex].ItemID[fi1]);
					lStyle=GetWindowLong(m_CItem->m_hWnd,GWL_STYLE);
					lStyle=BS_AUTO3STATE|WS_CHILD|WS_VISIBLE|WS_TABSTOP;
					SetWindowLong(m_CItem->m_hWnd,GWL_STYLE,lStyle);
				}
			}
			if (bCurflag)
				m_CItem->EnableWindow(FALSE);
			else
				m_CItem->EnableWindow(TRUE);
			if (ic_assoc(rbb,62)) break;
			m_color=abs(ic_rbassoc->resval.rint);
			if (!m_ColorB)	 m_ColorB.SubclassDlgItem(IDC_BUTTON1,this);
			// Setup LineType Combobox
			if (!m_combobox) m_combobox.SubclassDlgItem(EXP_LINETYPE,this);
			m_combobox.ResetContent();
			IC_RELRB(rbb);
			rbb=SDS_tblgetter(IEXP_EntityCol[IEXP_LTTAB_IDX].cType,NULL,TRUE,IEXP_ODNode->m_dbDrawing);
			while(rbb)
			{
				if (!ic_assoc(rbb,2))
				{
					if (!strisame(ic_rbassoc->resval.rstring,IEXP_BYBLOCK) && !strisame(ic_rbassoc->resval.rstring,IEXP_BYLAYER))
					{
						if (!ic_assoc(rbb,70))
						{
							if (!(ic_rbassoc->resval.rint&IEXP_DEL_FLG) && !(ic_rbassoc->resval.rint&IEXP_XREF_FLG))
							{
								if (!ic_assoc(rbb,2))
								{
									m_combobox.AddString(ic_rbassoc->resval.rstring);
								}
							}
						}
					}
				}
				sds_relrb(rbb);
				rbb=SDS_tblgetter(IEXP_EntityCol[IEXP_LTTAB_IDX].cType,NULL,FALSE,IEXP_ODNode->m_dbDrawing);
			};
			// Setup Lineweight Combobox
			{
				int i, top, pos;
				if (!m_lwcombobox) m_lwcombobox.SubclassDlgItem(EXP_LWEIGHT,this);
				m_lwcombobox.ResetContent();
				top = ( IEXP_lwunits == 1 ? IDC_COMBO_M_LWDEFAULT : IDC_COMBO_I_LWDEFAULT );
				for( i = 0 ; i < 27 ; i++ )
				{
					if( i - 3 == DB_BBWEIGHT ||  i - 3 == DB_BLWEIGHT )
						continue;
					pos = m_lwcombobox.AddString(ResourceString(top+i, ""));
					m_lwcombobox.SetItemData(pos, db_index2lweight(i-3));
				}
			}
			break;
	}
	IC_RELRB(rbt);
	IC_RELRB(rbb);
	return(TRUE);
}

void IEXP_SubDlg::ColorButton(BOOL Varies) {
	CWnd   *CItem;
	HDC 	hDC,
			hCmpDC;
	RECT	rect;

	if (!m_dataptr) return;
	CItem=GetDlgItem(EXP_LAYER_COLOR);
	if (!CItem) return;
	hDC=::GetDC(CItem->m_hWnd);
	::GetClientRect(CItem->m_hWnd,&rect);
	if (Varies) {
		HBITMAP hBitmap=LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(COL_VARIES));
		hCmpDC=CreateCompatibleDC(hDC);
		HBITMAP hOldBitmap=(HBITMAP)SelectObject(hCmpDC,hBitmap);
		BitBlt(hDC,rect.left,rect.top,rect.right,rect.bottom,hCmpDC,rect.left,rect.top,SRCCOPY);
		SelectObject(hCmpDC,hOldBitmap);
		DeleteObject(hBitmap);
		DeleteDC(hCmpDC);
	} else {
		CBrush	brush;
		HPALETTE savpal=::SelectPalette(hDC,(HPALETTE)SDS_CMainWindow->m_pPalette->GetSafeHandle(),TRUE);
		brush.CreateSolidBrush(SDS_BrushColorFromACADColor(m_color));
		::FillRect(hDC,&rect,(HBRUSH)brush);
//		  ::SelectPalette(hDC,savpal,TRUE);
		brush.DeleteObject();
		m_CdetailView->m_regenstate=TRUE;
	}
	::ReleaseDC(CItem->m_hWnd,hDC);
	if(m_CdetailView->m_regenstate && m_CdetailView->m_Explorer->m_regenMode)
	{
		cmd_regenall();
		m_CdetailView->m_regenstate = FALSE;
	}
}

///////////////////////////////////////////////////////////////////////
// This function is called when the subdialog of an edit dialog is
// destroyed, the sub dialogs contains all the child items such as edit
// boxes,buttons,etc. In this function there is a case statment for each
// major (ITEM) type, such as (Layer,Linetypes,Text styles,etc.) in each
// case for each item in the subdialog we get the items text or state
// value, we then compare it to the original value that was saved in the
// (OnInitDialog) function, if the value has changed the it is add to
// the resbuf link list to be used to update the entity, if it has not
// changed then it is not added. at the end we call tablemesser to update
// the entity.
//
// Nothing is returned.
//
void IEXP_SubDlg::SaveTabChanges() {
	CWnd		  *CItem;
	int 		   Value,
				   Svalue;
	sds_point	   pt;
	char		  *ptr1,
				  *ptr2;
	struct resbuf *rbb=NULL,
				  *rbt=NULL,
				  *rb,
				  *rbn=NULL;
	short		   curidx,
				   index,
				   ret,
				   tblmode=IC_TBLMESSER_MODIFY;
	CString 	   cString;

	if (!m_dataptr) return;

	for(curidx=-1,index=0;index<m_Vindex;++index) {
		curidx=m_CdetailView->m_ListCtl[IEXP_CurrentIndx].GetNextItem(curidx,LVNI_SELECTED);
		if (curidx==-1) break;
		switch(IEXP_EntityCol[m_oldindex].IType) {
			case IEXP_BNTAB_IDX:
				IEXP_ItemDefPtr=(IEXP_ItemDef *)m_CdetailView->m_ListCtl[IEXP_CurrentIndx].GetItemData(curidx);
				rbn=SDS_tblgetter(IEXP_EntityCol[IEXP_CurrentIndx].cType,IEXP_ItemDefPtr->name(),TRUE,IEXP_ODNode->m_dbDrawing);
				if (!(rbt=BuildDataList(&rbb,RTDXF0,IEXP_BLOCK)) || !(BuildDataList(&rbt,2,IEXP_ItemDefPtr->name()))) break;
				GetDlgItemText(IEXP_TABINFO[m_oldindex].ItemID[1],IEXP_szText,64);
				if (!strisame(IEXP_szText,(LPCSTR)(blksave.base))) {
					m_CdetailView->m_ListCtl[IEXP_CurrentIndx].SetItemText(curidx,IEXP_EntityCol[m_oldindex].Order[2][IEXP_COL_ORDER],IEXP_szText);
					IEXP_strtopointr(IEXP_szText,pt);
					if (IEXP_szText) { BuildDataList(&rbt,10,pt); }
				}
				break;
			case IEXP_VWTAB_IDX:
				IEXP_ItemDefPtr=(IEXP_ItemDef *)m_CdetailView->m_ListCtl[IEXP_CurrentIndx].GetItemData(curidx);
				rbn=SDS_tblgetter(IEXP_EntityCol[IEXP_CurrentIndx].cType,IEXP_ItemDefPtr->name(),TRUE,IEXP_ODNode->m_dbDrawing);
				if (!(rbt=BuildDataList(&rbb,RTDXF0,IEXP_VIEW)) || !(BuildDataList(&rbt,2,IEXP_ItemDefPtr->name()))) break;
				switch(m_DlgIndex) {
					case 0:
						GetDlgItemText(IEXP_TABINFO[m_oldindex].ItemID[1],IEXP_szText,64);
						if (!strisame(IEXP_szText,(LPCSTR)(vwsave.height))) {
							m_CdetailView->m_ListCtl[IEXP_CurrentIndx].SetItemText(curidx,IEXP_EntityCol[m_oldindex].Order[1][IEXP_COL_ORDER],IEXP_szText);
							if (IEXP_szText) { if (!(BuildDataList(&rbt,40,atof(IEXP_szText)))) break; }
						}
						GetDlgItemText(IEXP_TABINFO[m_oldindex].ItemID[2],IEXP_szText,64);
						if (!strisame(IEXP_szText,(LPCSTR)(vwsave.width))) {
							m_CdetailView->m_ListCtl[IEXP_CurrentIndx].SetItemText(curidx,IEXP_EntityCol[m_oldindex].Order[2][IEXP_COL_ORDER],IEXP_szText);
							if (IEXP_szText) { if (!(BuildDataList(&rbt,41,atof(IEXP_szText)))) break; }
						}
						GetDlgItemText(IEXP_TABINFO[m_oldindex].ItemID[3],IEXP_szText,64); IEXP_strtopointr(IEXP_szText,pt);
						if (!strisame(IEXP_szText,(LPCSTR)(vwsave.center))) {
//							  m_CdetailView->m_ListCtl[IEXP_CurrentIndx].SetItemText(curidx,IEXP_EntityCol[m_oldindex].Order[3][IEXP_COL_ORDER],IEXP_szText);
							if (IEXP_szText) BuildDataList(&rbt,10,pt);
						}
						break;
					case 1:
						GetDlgItemText(IEXP_TABINFO[m_oldindex].ItemID[4],IEXP_szText,64);
						if (!strisame(IEXP_szText,(LPCSTR)(vwsave.twist))) {
							if (IEXP_szText) { if (!(BuildDataList(&rbt,50,atof(IEXP_szText)))) break; }
						}
						GetDlgItemText(IEXP_TABINFO[m_oldindex].ItemID[5],IEXP_szText,64);
						if (!strisame(IEXP_szText,(LPCSTR)(vwsave.lenslen))) {
							if (IEXP_szText) { if (!(BuildDataList(&rbt,42,atof(IEXP_szText)))) break; }
						}
						GetDlgItemText(IEXP_TABINFO[m_oldindex].ItemID[6],IEXP_szText,64);
						if (!strisame(IEXP_szText,(LPCSTR)(vwsave.fclip))) {
							if (IEXP_szText) { if (!(BuildDataList(&rbt,43,atof(IEXP_szText)))) break; }
						}
						GetDlgItemText(IEXP_TABINFO[m_oldindex].ItemID[7],IEXP_szText,64);
						if (!strisame(IEXP_szText,(LPCSTR)(vwsave.bclip))) {
							if (IEXP_szText) BuildDataList(&rbt,44,atof(IEXP_szText));
						}
						break;
					case 2:
						ic_assoc(rbn,71);
						Svalue=ic_rbassoc->resval.rint;

						CItem=GetDlgItem(IEXP_TABINFO[m_oldindex].ItemID[8]);
						Value=::SendMessage(CItem->m_hWnd,BM_GETCHECK,0,0);
						if (Value!=vwsave.viewmd1 && Value!=2) {
							Svalue=(Value)?Svalue|1:Svalue^1;
						}
						CItem=GetDlgItem(IEXP_TABINFO[m_oldindex].ItemID[9]);
						Value=::SendMessage(CItem->m_hWnd,BM_GETCHECK,0,0);
						if (Value!=vwsave.viewmd2 && Value!=2) {
							Svalue=(Value)?Svalue|2:Svalue^2;
						}
						CItem=GetDlgItem(IEXP_TABINFO[m_oldindex].ItemID[10]);
						Value=::SendMessage(CItem->m_hWnd,BM_GETCHECK,0,0);
						if (Value!=vwsave.viewmd4 && Value!=2) {
							Svalue=(Value)?Svalue|4:Svalue^4;
						}
						CItem=GetDlgItem(IEXP_TABINFO[m_oldindex].ItemID[11]);
						Value=::SendMessage(CItem->m_hWnd,BM_GETCHECK,0,0);
						if (Value!=vwsave.viewmd8 && Value!=2) {
							Svalue=(Value)?Svalue|8:Svalue^8;
						}
						CItem=GetDlgItem(IEXP_TABINFO[m_oldindex].ItemID[12]);
						Value=::SendMessage(CItem->m_hWnd,BM_GETCHECK,0,0);
						if (Value!=vwsave.viewmd16 && Value!=2) {
							Svalue=(Value)?Svalue|16:Svalue^16;
						}
						if (Svalue!=ic_rbassoc->resval.rint) BuildDataList(&rbt,71,Svalue);
						break;
					case 3:
						GetDlgItemText(IEXP_TABINFO[m_oldindex].ItemID[13],IEXP_szText,64);
						if (!strisame(IEXP_szText,(LPCSTR)(vwsave.target))) {
							IEXP_strtopointr(IEXP_szText,pt);
							if (IEXP_szText) { if (!(BuildDataList(&rbt,12,pt))) break; }
						}
						GetDlgItemText(IEXP_TABINFO[m_oldindex].ItemID[14],IEXP_szText,64);
						if (!strisame(IEXP_szText,(LPCSTR)(vwsave.targ2cam))) {
							m_CdetailView->m_ListCtl[IEXP_CurrentIndx].SetItemText(curidx,IEXP_EntityCol[m_oldindex].Order[3][IEXP_COL_ORDER],IEXP_szText);
							IEXP_strtopointr(IEXP_szText,pt);
							if (IEXP_szText) { if (!(BuildDataList(&rbt,11,pt))) break; }
						}
						break;
				}
				break;
			case IEXP_CSTAB_IDX:
				IEXP_ItemDefPtr=(IEXP_ItemDef *)m_CdetailView->m_ListCtl[IEXP_CurrentIndx].GetItemData(curidx);
				rbn=SDS_tblgetter(IEXP_EntityCol[IEXP_CurrentIndx].cType,IEXP_ItemDefPtr->name(),TRUE,IEXP_ODNode->m_dbDrawing);
				if (!(rbt=BuildDataList(&rbb,RTDXF0,IEXP_UCS)) || !(BuildDataList(&rbt,2,IEXP_ItemDefPtr->name()))) break;
				GetDlgItemText(IEXP_TABINFO[m_oldindex].ItemID[1],IEXP_szText,64);
				if (!strisame(IEXP_szText,(LPCSTR)(cssave.origin))) {
					m_CdetailView->m_ListCtl[IEXP_CurrentIndx].SetItemText(curidx,IEXP_EntityCol[m_oldindex].Order[1][IEXP_COL_ORDER],IEXP_szText);
					IEXP_strtopointr(IEXP_szText,pt);
					if (IEXP_szText) { if (!(BuildDataList(&rbt,10,pt))) break; }
				}
				GetDlgItemText(IEXP_TABINFO[m_oldindex].ItemID[2],IEXP_szText,64);
				if (!strisame(IEXP_szText,(LPCSTR)(cssave.xdir))) {
					m_CdetailView->m_ListCtl[IEXP_CurrentIndx].SetItemText(curidx,IEXP_EntityCol[m_oldindex].Order[2][IEXP_COL_ORDER],IEXP_szText);
					IEXP_strtopointr(IEXP_szText,pt);
					if (IEXP_szText) { if (!(BuildDataList(&rbt,11,pt))) break; }
				}
				GetDlgItemText(IEXP_TABINFO[m_oldindex].ItemID[3],IEXP_szText,64);
				if (!strisame(IEXP_szText,(LPCSTR)(cssave.ydir))) {
					m_CdetailView->m_ListCtl[IEXP_CurrentIndx].SetItemText(curidx,IEXP_EntityCol[m_oldindex].Order[3][IEXP_COL_ORDER],IEXP_szText);
					IEXP_strtopointr(IEXP_szText,pt);
					if (IEXP_szText) { if (!(BuildDataList(&rbt,12,pt))) break; }
				}
				break;
			case IEXP_LTTAB_IDX:
				if(!m_dataptr)
					return;
				// check for complex linetype
				GetDlgItemText(IEXP_TABINFO[m_oldindex].ItemID[2], IEXP_szText, 255);
				if(strchr(IEXP_szText, '[') || strchr(IEXP_szText, ']'))
					return;

				IEXP_ItemDefPtr = (IEXP_ItemDef*)m_CdetailView->m_ListCtl[IEXP_CurrentIndx].GetItemData(curidx);
				rbn = SDS_tblgetter(IEXP_EntityCol[IEXP_CurrentIndx].cType, IEXP_ItemDefPtr->name(), TRUE, IEXP_ODNode->m_dbDrawing);
				if(!(rbt = BuildDataList(&rbb, RTDXF0, IEXP_LTYPE)) || !(BuildDataList(&rbt, 2, IEXP_ItemDefPtr->name())))
					break;
				GetDlgItemText(IEXP_TABINFO[m_oldindex].ItemID[1], IEXP_szText, 64);
				if(!strisame(IEXP_szText,(LPCSTR)(ltsave.desc)))
				{
					m_CdetailView->m_ListCtl[IEXP_CurrentIndx].SetItemText(curidx,IEXP_EntityCol[m_oldindex].Order[1][IEXP_COL_ORDER], IEXP_szText);
					if(!(BuildDataList(&rbt, 3, (char *)IEXP_szText)))
						break;
				}
				// Cannot skip this becouse tablemesser needs this or it will zero out some of the fields.
				// if (strisame(IEXP_ItemDefPtr->name(),IEXP_CONTINUOUS)) return;
				if(!strisame(IEXP_ItemDefPtr->name(), IEXP_CONTINUOUS) && !strisame(IEXP_ItemDefPtr->name(), IEXP_BYLAYER) && !strisame(IEXP_ItemDefPtr->name(), IEXP_BYBLOCK))
				{
					GetDlgItemText(IEXP_TABINFO[m_oldindex].ItemID[2], IEXP_szText, 255);
					for(index = 1, ptr1 = IEXP_szText; ptr2 = strchr(ptr1, IEXP_COMMA); ptr1 = ptr2 + 1, ++index, rbt = rbt->rbnext)
					{
						*ptr2 = 0;
						rbt->rbnext = sds_newrb(49);
						rbt->rbnext->resval.rreal = atof(ptr1);
					}
					if(*ptr1)	//D.Gavrilov. We should add the last 49 ONLY in this case.
					{
						rbt->rbnext = sds_newrb(49);
						rbt->rbnext->resval.rreal = atof(ptr1);
					}
				}
				// if (!ic_assoc(rbb,2)) { if ((strsame(ic_rbassoc->resval.rstring,"BYBLOCK")) || (strsame(ic_rbassoc->resval.rstring,"BYLAYER"))) return; }
				break;
			case IEXP_LATAB_IDX:
				// Get layer name from property dialog box.
				IEXP_ItemDefPtr=(IEXP_ItemDef *)m_CdetailView->m_ListCtl[IEXP_CurrentIndx].GetItemData(curidx);
				// Get layer information (entity list) and match layer name.
				rbn=SDS_tblgetter(IEXP_EntityCol[IEXP_CurrentIndx].cType,IEXP_ItemDefPtr->name(),TRUE,IEXP_ODNode->m_dbDrawing);
				if (!(rbt=BuildDataList(&rbb,RTDXF0,IEXP_LAYER))) break;
				if (!(BuildDataList(&rbt,2,IEXP_ItemDefPtr->name()))) break;
				// Get linetype and set explorer column.
				GetDlgItemText(IEXP_TABINFO[m_oldindex].ItemID[IEXP_PLA_LINETYPE],IEXP_szText,64);
				if (!strisame(IEXP_szText,(LPCSTR)(lasave.linetype)) && !strisame(IEXP_szText,ResourceString(IDC_IEXP_VARIES_S, "Varies"))) {
					if (!(BuildDataList(&rbt,6,(char *)IEXP_szText))) break;
					m_CdetailView->m_ListCtl[IEXP_CurrentIndx].SetItemText(curidx,IEXP_EntityCol[m_oldindex].Order[IEXP_LA_LINETYPE][IEXP_COL_ORDER],IEXP_szText);
				}
				rb=NULL;
				// Get color, on/off and set explorer column.
				if (lasave.color!=m_color) {
					if (!(rb=BuildDataList(&rbt,62,m_color))) break;
					ic_colorstr(abs(m_color),IEXP_szText);
					m_CdetailView->m_ListCtl[IEXP_CurrentIndx].SetItemText(curidx,IEXP_EntityCol[m_oldindex].Order[IEXP_LA_COLOR][IEXP_COL_ORDER],IEXP_szText);
				}
				CItem=GetDlgItem(IEXP_TABINFO[m_oldindex].ItemID[IEXP_PLA_ON_OFF]);
				Value=::SendMessage(CItem->m_hWnd,BM_GETCHECK,0,20);
				if (Value!=lasave.OnOff && Value!=2) {
					if (rb) {
						rb->resval.rint=(!Value)?abs(rb->resval.rint):-(abs(rb->resval.rint));
					} else {
						ic_assoc(rbn,62);
						if (!BuildDataList(&rbt,62,(int)((!Value)?abs(ic_rbassoc->resval.rint):-(abs(ic_rbassoc->resval.rint))))) break;
					}
					sprintf(IEXP_szText,"%s"/*DNT*/,(Value)?ResourceString(IDC_ICADEXPLORER_OFF_16, "Off" ):ResourceString(IDC_ICADEXPLORER_ON_17, "On" ));
					m_CdetailView->m_ListCtl[IEXP_CurrentIndx].SetItemText(curidx,IEXP_EntityCol[m_oldindex].Order[IEXP_LA_ON_OFF][IEXP_COL_ORDER],IEXP_szText);
				}
				// Get Lweight 
				rb=NULL;
				{
					CComboBox* cWnd = (CComboBox*)GetDlgItem(IEXP_TABINFO[m_oldindex].ItemID[IEXP_PLA_LINEWEIGHT]);
					int lweight=lasave.lweight;
					GetDlgItemText(IEXP_TABINFO[m_oldindex].ItemID[IEXP_PLA_LINEWEIGHT],IEXP_szText,64);
					if( cWnd != NULL && cWnd->GetCurSel() != LB_ERR )
						lweight = (int)(cWnd->GetItemData(cWnd->GetCurSel()));
					if (lweight!=lasave.lweight && !strisame(IEXP_szText,ResourceString(IDC_IEXP_VARIES_S, "Varies"))) {
						if (!(BuildDataList(&rbt,370,lweight))) break;
						m_CdetailView->m_ListCtl[IEXP_CurrentIndx].SetItemText(curidx,IEXP_EntityCol[m_oldindex].Order[IEXP_LA_LINEWEIGHT][IEXP_COL_ORDER],IEXP_szText);
					}
				}
				// Plot Yes/No
				ic_assoc(rbn,290);
				rb=NULL; ret=ic_rbassoc->resval.rint;
				CItem=GetDlgItem(IEXP_TABINFO[m_oldindex].ItemID[IEXP_PLA_PLOT]);
				Value=::SendMessage(CItem->m_hWnd,BM_GETCHECK,0,0);
				if (Value!=lasave.plot && Value!=2) {
					if (!BuildDataList(&rbt,290,Value)) break;
					sprintf(IEXP_szText,"%s"/*DNT*/,(Value)?ResourceString(IDC_IEXP_YES, "Yes"):ResourceString(IDC_IEXP_NO, "No"));
					m_CdetailView->m_ListCtl[IEXP_CurrentIndx].SetItemText(curidx,IEXP_EntityCol[m_oldindex].Order[IEXP_LA_PLOT][IEXP_COL_ORDER],IEXP_szText);
				}
				// frozen or thawed
				ic_assoc(rbn,70);
				rb=NULL; ret=ic_rbassoc->resval.rint;
				CItem=GetDlgItem(IEXP_TABINFO[m_oldindex].ItemID[IEXP_PLA_FROZEN]);
				Value=::SendMessage(CItem->m_hWnd,BM_GETCHECK,0,0);
				if (Value!=lasave.freeze && Value!=2) {
					ret=((!Value)?ic_rbassoc->resval.rint^IC_LAYER_FROZEN:ic_rbassoc->resval.rint|IC_LAYER_FROZEN);
					if (!(rb=BuildDataList(&rbt,70,ret))) break;
					sprintf(IEXP_szText,"%s"/*DNT*/,(Value)?ResourceString(IDC_IEXP_FROZEN_STR, "Frozen"):ResourceString(IDC_IEXP_THAWED_STR, "Thawed"));
					m_CdetailView->m_ListCtl[IEXP_CurrentIndx].SetItemText(curidx,IEXP_EntityCol[m_oldindex].Order[IEXP_LA_FROZEN][IEXP_COL_ORDER],IEXP_szText);
				}
				// Lock or unlock
				CItem=GetDlgItem(IEXP_TABINFO[m_oldindex].ItemID[IEXP_PLA_LOCKED]);
				Value=::SendMessage(CItem->m_hWnd,BM_GETCHECK,0,0);
				if (Value!=lasave.lock && Value!=2) {
					if (rb) {
						rb->resval.rint=(!Value)?ret^IC_LAYER_LOCKED:ret|IC_LAYER_LOCKED;
					} else {
						if (!BuildDataList(&rbt,70,((!Value)?ret^IC_LAYER_LOCKED:ret|IC_LAYER_LOCKED))) break;
					}
					sprintf(IEXP_szText,"%s"/*DNT*/,(Value)?ResourceString(IDC_IEXP_YES, "Yes"):ResourceString(IDC_IEXP_NO, "No"));
					m_CdetailView->m_ListCtl[IEXP_CurrentIndx].SetItemText(curidx,IEXP_EntityCol[m_oldindex].Order[IEXP_LA_LOCKED][IEXP_COL_ORDER],IEXP_szText);
				}
				// New VP Freeze or Thaw
				CItem=GetDlgItem(IEXP_TABINFO[m_oldindex].ItemID[IEXP_PLA_VPNEW]);
				Value=::SendMessage(CItem->m_hWnd,BM_GETCHECK,0,0);
				if (Value!=lasave.lock && Value!=2) {
					if (rb) {
						rb->resval.rint=(!Value)?ret^IC_LAYER_VPFROZEN:ret|IC_LAYER_VPFROZEN;
					} else {
						if (!BuildDataList(&rbt,70,((!Value)?ret^IC_LAYER_VPFROZEN:ret|IC_LAYER_VPFROZEN))) break;
					}
					sprintf(IEXP_szText,"%s"/*DNT*/,(Value)?ResourceString(IDC_IEXP_FROZEN_STR, "Frozen"):ResourceString(IDC_IEXP_THAWED_STR, "Thawed"));
					m_CdetailView->m_ListCtl[IEXP_CurrentIndx].SetItemText(curidx,IEXP_EntityCol[m_oldindex].Order[IEXP_LA_VP_FREEZENEW][IEXP_COL_ORDER],IEXP_szText);
				}
				break;
		}
		if(IEXP_RegenOnUndo==FALSE) {
			SDS_SetUndo(IC_UNDO_REGEN_ON_UNDO,NULL,NULL,NULL,SDS_CURDWG);
			IEXP_RegenOnUndo=TRUE;
		}
		m_CdetailView->m_regenstate=TRUE;
		if (rbb) { ret=SDS_tblmesser(rbb,tblmode,IEXP_ODNode->m_dbDrawing); sds_relrb(rbb); }
		if (rbn) sds_relrb(rbn);
	}
	if(m_CdetailView->m_regenstate && m_CdetailView->m_Explorer->m_regenMode)
	{
		cmd_regenall();
		m_CdetailView->m_regenstate = FALSE;
	}
}

BOOL IEXP_SubDlg::OnCommand(WPARAM wParam,LPARAM lParam) {
	int 		   Color;

	switch(HIWORD(wParam)) {
		case BN_CLICKED:
			switch(LOWORD(wParam)) {
				case 1:
					return(FALSE);
				case IDC_BUTTON1:
					m_Inuse=TRUE;
					if (SDS_GetColorDialog(m_color,&Color,4)==RTNORM) {
						if (IEXP_EntityCol[m_oldindex].IType==IEXP_LATAB_IDX) {
							if (Color==0 || Color==256) return(FALSE);
						}
						m_Inuse=FALSE;
						m_color=Color;
						m_colorVaries=FALSE;
						m_newcolor=TRUE;
						ColorButton(m_colorVaries);
					}
					break;
			}
			break;
		default:
			break;
	}

	return CDialog::OnCommand(wParam,lParam);
}

void IEXP_SubDlg::DoDataExchange(CDataExchange *pDX) {
	int 		   Value;
	CString 	   String;
	struct resbuf *rbb=NULL,
				  *rb;
	int 		   ndash;
	CDialog::DoDataExchange(pDX);

	if (!m_dataptr) return;
	switch(IEXP_EntityCol[IEXP_CurrentIndx].IType) {
		case IEXP_BNTAB_IDX:
			IEXP_ItemDefPtr=(IEXP_ItemDef *)m_dataptr;
			IC_RELRB(rbb);
			rbb=SDS_tblgetter(IEXP_EntityCol[IEXP_CurrentIndx].cType,IEXP_ItemDefPtr->name(),TRUE,IEXP_ODNode->m_dbDrawing);
			for(rb=rbb;rb;rb=rb->rbnext) {
				switch(rb->restype) {
					case 1:
						ic_assoc(rb,70);
						String=rb->resval.rstring;
						DDX_Text(pDX,IEXP_TABINFO[IEXP_CurrentIndx].ItemID[2],String);
						break;
					case 2:
						if (m_varies) { sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER__D_BLOCKS_153, "%d Blocks" ),m_Vindex); String=IEXP_szText; } else String=rb->resval.rstring;
						DDX_Text(pDX,IEXP_TABINFO[IEXP_CurrentIndx].ItemID[0],String);
						break;
					case 10:
						if (m_varies && IEXP_TABINFO[IEXP_CurrentIndx].Varies[1]) String=ResourceString(IDC_IEXP_VARIES_S, "Varies");
						else { sprintf(IEXP_szText,ResourceString(DNT_ICADEXPLORER__G__G__G_12, "%G,%G,%G" ),rb->resval.rpoint[0],rb->resval.rpoint[1],rb->resval.rpoint[2]); String=IEXP_szText; }
						DDX_Text(pDX,IEXP_TABINFO[IEXP_CurrentIndx].ItemID[1],String);
						break;
				}
			}
			break;
		case IEXP_VWTAB_IDX:
			IEXP_ItemDefPtr=(IEXP_ItemDef *)m_dataptr;
			IC_RELRB(rbb);
			rbb=SDS_tblgetter(IEXP_EntityCol[IEXP_CurrentIndx].cType,IEXP_ItemDefPtr->name(),TRUE,IEXP_ODNode->m_dbDrawing);
			switch(m_DlgIndex) {
				case 0:
					for(rb=rbb;rb;rb=rb->rbnext) {
						switch(rb->restype) {
							case 2:
								if (m_varies) { sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER__D_VIEWS_154, "%d Views" ),m_Vindex); String=IEXP_szText; } else String=rb->resval.rstring;
								DDX_Text(pDX,IEXP_TABINFO[IEXP_CurrentIndx].ItemID[0],String);
								break;
							case 40:
								if (m_varies && IEXP_TABINFO[IEXP_CurrentIndx].Varies[1]) String=ResourceString(IDC_IEXP_VARIES_S, "Varies");
								else { sprintf(IEXP_szText,"%G"/*DNT*/,rb->resval.rreal); String=IEXP_szText; }
								if (!pDX->m_bSaveAndValidate) vwsave.height=String;
								DDX_Text(pDX,IEXP_TABINFO[IEXP_CurrentIndx].ItemID[1],String);
								break;
							case 41:
								if (m_varies && IEXP_TABINFO[IEXP_CurrentIndx].Varies[2]) String=ResourceString(IDC_IEXP_VARIES_S, "Varies");
								else { sprintf(IEXP_szText,"%G"/*DNT*/,rb->resval.rreal); String=IEXP_szText; }
								if (!pDX->m_bSaveAndValidate) vwsave.width=String;
								DDX_Text(pDX,IEXP_TABINFO[IEXP_CurrentIndx].ItemID[2],String);
								break;
							case 11:
								if (m_varies && IEXP_TABINFO[IEXP_CurrentIndx].Varies[3]) String=ResourceString(IDC_IEXP_VARIES_S, "Varies");
								else { sprintf(IEXP_szText,ResourceString(DNT_ICADEXPLORER__G__G__G_12, "%G,%G,%G" ),rb->resval.rpoint[0],rb->resval.rpoint[1],rb->resval.rpoint[2]); String=IEXP_szText; }
								if (!pDX->m_bSaveAndValidate) vwsave.center=String;
								DDX_Text(pDX,IEXP_TABINFO[IEXP_CurrentIndx].ItemID[3],String);
								break;
						}
					}
					break;
				case 1:
					for(rb=rbb;rb;rb=rb->rbnext) {
						switch(rb->restype) {
							case 50:
								if (m_varies && IEXP_TABINFO[IEXP_CurrentIndx].Varies[4]) String=ResourceString(IDC_IEXP_VARIES_S, "Varies");
								else { sprintf(IEXP_szText,"%G"/*DNT*/,rb->resval.rreal); String=IEXP_szText; }
								if (!pDX->m_bSaveAndValidate) vwsave.twist=String;
								DDX_Text(pDX,IEXP_TABINFO[IEXP_CurrentIndx].ItemID[4],String);
								break;
							case 42:
								if (m_varies && IEXP_TABINFO[IEXP_CurrentIndx].Varies[5]) String=ResourceString(IDC_IEXP_VARIES_S, "Varies");
								else { sprintf(IEXP_szText,"%G"/*DNT*/,rb->resval.rreal); String=IEXP_szText; }
								if (!pDX->m_bSaveAndValidate) vwsave.lenslen=String;
								DDX_Text(pDX,IEXP_TABINFO[IEXP_CurrentIndx].ItemID[5],String);
								break;
							case 43:
								if (m_varies && IEXP_TABINFO[IEXP_CurrentIndx].Varies[6]) String=ResourceString(IDC_IEXP_VARIES_S, "Varies");
								else { sprintf(IEXP_szText,"%G"/*DNT*/,rb->resval.rreal); String=IEXP_szText; }
								if (!pDX->m_bSaveAndValidate) vwsave.fclip=String;
								DDX_Text(pDX,IEXP_TABINFO[IEXP_CurrentIndx].ItemID[6],String);
								break;
							case 44:
								if (m_varies && IEXP_TABINFO[IEXP_CurrentIndx].Varies[7]) String=ResourceString(IDC_IEXP_VARIES_S, "Varies");
								else { sprintf(IEXP_szText,"%G"/*DNT*/,rb->resval.rreal); String=IEXP_szText; }
								if (!pDX->m_bSaveAndValidate) vwsave.bclip=String;
								DDX_Text(pDX,IEXP_TABINFO[IEXP_CurrentIndx].ItemID[7],String);
								break;
						}
					}
					break;
				case 2:
					ic_assoc(rbb,71);
					Value=(m_varies && IEXP_TABINFO[IEXP_CurrentIndx].Varies[8])?IEXP_VARIES_I:(ic_rbassoc->resval.rint&1)?1:0;
					if (!pDX->m_bSaveAndValidate) vwsave.viewmd1=Value;
					DDX_Check(pDX,IEXP_TABINFO[IEXP_CurrentIndx].ItemID[8],Value);

					Value=(m_varies && IEXP_TABINFO[IEXP_CurrentIndx].Varies[9])?IEXP_VARIES_I:(ic_rbassoc->resval.rint&2)?1:0;
					if (!pDX->m_bSaveAndValidate) vwsave.viewmd2=Value;
					DDX_Check(pDX,IEXP_TABINFO[IEXP_CurrentIndx].ItemID[9],Value);

					Value=(m_varies && IEXP_TABINFO[IEXP_CurrentIndx].Varies[10])?IEXP_VARIES_I:(ic_rbassoc->resval.rint&4)?1:0;
					if (!pDX->m_bSaveAndValidate) vwsave.viewmd4=Value;
					DDX_Check(pDX,IEXP_TABINFO[IEXP_CurrentIndx].ItemID[10],Value);

					Value=(m_varies && IEXP_TABINFO[IEXP_CurrentIndx].Varies[11])?IEXP_VARIES_I:(ic_rbassoc->resval.rint&8)?1:0;
					if (!pDX->m_bSaveAndValidate) vwsave.viewmd8=Value;
					DDX_Check(pDX,IEXP_TABINFO[IEXP_CurrentIndx].ItemID[11],Value);

					Value=(m_varies && IEXP_TABINFO[IEXP_CurrentIndx].Varies[12])?IEXP_VARIES_I:(ic_rbassoc->resval.rint&16)?1:0;
					if (!pDX->m_bSaveAndValidate) vwsave.viewmd16=Value;
					DDX_Check(pDX,IEXP_TABINFO[IEXP_CurrentIndx].ItemID[12],Value);
					break;
				case 3:
					for(rb=rbb;rb;rb=rb->rbnext) {
						switch(rb->restype) {
							case 12:
								if (m_varies && IEXP_TABINFO[IEXP_CurrentIndx].Varies[13]) String=ResourceString(IDC_IEXP_VARIES_S, "Varies");
								else { sprintf(IEXP_szText,ResourceString(DNT_ICADEXPLORER__G__G__G_12, "%G,%G,%G" ),rb->resval.rpoint[0],rb->resval.rpoint[1],rb->resval.rpoint[2]); String=IEXP_szText; }
								if (!pDX->m_bSaveAndValidate) vwsave.target=String;
								DDX_Text(pDX,IEXP_TABINFO[IEXP_CurrentIndx].ItemID[13],String);
								break;
							case 11:
								if (m_varies && IEXP_TABINFO[IEXP_CurrentIndx].Varies[14]) String=ResourceString(IDC_IEXP_VARIES_S, "Varies");
								else { sprintf(IEXP_szText,ResourceString(DNT_ICADEXPLORER__G__G__G_12, "%G,%G,%G" ),rb->resval.rpoint[0],rb->resval.rpoint[1],rb->resval.rpoint[2]); String=IEXP_szText; }
								if (!pDX->m_bSaveAndValidate) vwsave.targ2cam=String;
								DDX_Text(pDX,IEXP_TABINFO[IEXP_CurrentIndx].ItemID[14],String);
								break;
						}
					}
					break;
			}
			break;
		case IEXP_CSTAB_IDX:
			IEXP_ItemDefPtr=(IEXP_ItemDef *)m_dataptr;
			IC_RELRB(rbb);
			rbb=SDS_tblgetter(IEXP_EntityCol[IEXP_CurrentIndx].cType,IEXP_ItemDefPtr->name(),TRUE,IEXP_ODNode->m_dbDrawing);
			for(rb=rbb;rb;rb=rb->rbnext) {
				switch(rb->restype) {
					case 2:
						if (m_varies) { sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER__D_UCS_S_155, "%d UCS's" ),m_Vindex); String=IEXP_szText; } else String=rb->resval.rstring;
						DDX_Text(pDX,IEXP_TABINFO[IEXP_CurrentIndx].ItemID[0],String);
						break;
					case 10:
						if (m_varies && IEXP_TABINFO[IEXP_CurrentIndx].Varies[1]) String=ResourceString(IDC_IEXP_VARIES_S, "Varies");
						else { sprintf(IEXP_szText,ResourceString(DNT_ICADEXPLORER__G__G__G_12, "%G,%G,%G" ),rb->resval.rpoint[0],rb->resval.rpoint[1],rb->resval.rpoint[2]); String=IEXP_szText; }
						if (!pDX->m_bSaveAndValidate) cssave.origin=String;
						DDX_Text(pDX,IEXP_TABINFO[IEXP_CurrentIndx].ItemID[1],String);
						break;
					case 11:
						if (m_varies && IEXP_TABINFO[IEXP_CurrentIndx].Varies[2]) String=ResourceString(IDC_IEXP_VARIES_S, "Varies");
						else { sprintf(IEXP_szText,ResourceString(DNT_ICADEXPLORER__G__G__G_12, "%G,%G,%G" ),rb->resval.rpoint[0],rb->resval.rpoint[1],rb->resval.rpoint[2]); String=IEXP_szText; }
						if (!pDX->m_bSaveAndValidate) cssave.xdir=String;
						DDX_Text(pDX,IEXP_TABINFO[IEXP_CurrentIndx].ItemID[2],String);
						break;
					case 12:
						if (m_varies && IEXP_TABINFO[IEXP_CurrentIndx].Varies[3]) String=ResourceString(IDC_IEXP_VARIES_S, "Varies");
						else { sprintf(IEXP_szText,ResourceString(DNT_ICADEXPLORER__G__G__G_12, "%G,%G,%G" ),rb->resval.rpoint[0],rb->resval.rpoint[1],rb->resval.rpoint[2]); String=IEXP_szText; }
						if (!pDX->m_bSaveAndValidate) cssave.ydir=String;
						DDX_Text(pDX,IEXP_TABINFO[IEXP_CurrentIndx].ItemID[3],String);
						break;
				}
			}
			break;
		case IEXP_LTTAB_IDX:
			if (!m_dataptr)
				return;
			IEXP_ItemDefPtr = (IEXP_ItemDef*)m_dataptr;
			IC_RELRB(rbb);
			rbb = SDS_tblgetter(IEXP_EntityCol[IEXP_CurrentIndx].cType, IEXP_ItemDefPtr->name(), TRUE, IEXP_ODNode->m_dbDrawing);
			if(m_varies)
			{
				sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER__D_LINETY_157, "%d Linetypes" ), m_Vindex);
				String = IEXP_szText;
			}
			else
			{
				ic_assoc(rbb,2);
				String = ic_rbassoc->resval.rstring;
			}
			DDX_Text(pDX,IEXP_TABINFO[IEXP_CurrentIndx].ItemID[0], String);

			ic_assoc(rbb, 3);
			String = (m_varies && IEXP_TABINFO[IEXP_CurrentIndx].Varies[1])? ResourceString(IDC_IEXP_VARIES_S, "Varies") : ic_rbassoc->resval.rstring;
			if(!pDX->m_bSaveAndValidate)
				ltsave.desc = String;
			DDX_Text(pDX,IEXP_TABINFO[IEXP_CurrentIndx].ItemID[1],String);

			if(m_varies && IEXP_TABINFO[IEXP_CurrentIndx].Varies[1])
				String = ResourceString(IDC_IEXP_VARIES_S, "Varies");
			else
			{
				if(!pDX->m_bSaveAndValidate)
				{
					ic_assoc(rbb,73);
					ndash=ic_rbassoc->resval.rint;

					db_styletabrec* pStyle = NULL;
					sds_real offset[2] = {0.0, 0.0};
					sds_real scale = 1.0;
					sds_real angle = 0.0;
					int shapeCode = 0;
					char text[IC_ACADBUF] = "\0";
					bool bHaveShape = false;
					bool bHaveText = false;
					for(*IEXP_szText = 0, rb = rbb; rb; rb = rb->rbnext)
					{
						switch(rb->restype)
						{
						case 49:
							sprintf(IEXP_szTextA, ResourceString(IDC_ICADEXPLORER__F__15, "%f," ), rb->resval.rreal);
							strcat(IEXP_szText, IEXP_szTextA);
							break;
						case 74:
							// check for complex linetype
							if(rb->resval.rint == 4)
								bHaveShape = true;
							else
								if(rb->resval.rint == 2)
									bHaveText = true;
							break;
						case 75:
							if(bHaveShape)
								shapeCode = rb->resval.rint;
							break;
						case 340:
							pStyle = (db_styletabrec*)rb->resval.rlname[0];
							break;
						case 46:
							scale = rb->resval.rreal;
							break;
						case 50:
							angle = rb->resval.rreal;
							break;
						case 44:
							offset[0] = rb->resval.rreal;
							break;
						case 45:
							offset[1] = rb->resval.rreal;
							break;
						case 9:
							strcpy(text, rb->resval.rstring);
							break;
						}
						if(pStyle != NULL && ((bHaveShape && shapeCode != 0) ||
							(bHaveText && text[0] != '\0')))
						{
							if(!icadRealEqual(scale,1.0) || !icadAngleEqual(angle,0.0) || !icadRealEqual(offset[0],0.0) || !icadRealEqual(offset[1],0.0))
							{
								if(bHaveShape)
								{
									char* pFileName = NULL;
									pStyle->get_3(&pFileName);
									db_findNameByCode(text, shapeCode, pStyle->ret_font());
									sprintf(IEXP_szTextA, "[%s,%s,R=%4.2f,S=%4.2f,X=%4.2f,Y=%4.2f],",
										text, pFileName, angle, scale, offset[0], offset[1]);
								}
								else
									sprintf(IEXP_szTextA, "[\"%s\",%s,R=%4.2f,S=%4.2f,X=%4.2f,Y=%4.2f],",
										text, pStyle->retp_name(), angle, scale, offset[0], offset[1]);
							}
							else
							{
								if(bHaveShape)
								{
									char* pFileName = NULL;
									pStyle->get_3(&pFileName);
									db_findNameByCode(text, shapeCode, pStyle->ret_font());
									sprintf(IEXP_szTextA, "[%s,%s],",
										text, pFileName);
								}
								else
									sprintf(IEXP_szTextA, "[\"%s\",%s],",
										text, pStyle->retp_name());
							}
							strcat(IEXP_szText, IEXP_szTextA);
							bHaveText = false;
							bHaveShape = false;
							shapeCode = 0;
							text[0] = '\0';
						}
					}
					String = IEXP_szText;
				}
				else
					String = ResourceString(IDC_IEXP_VARIES_S, "Varies");
			}
			DDX_Text(pDX,IEXP_TABINFO[IEXP_CurrentIndx].ItemID[2],String);
			break;
		case IEXP_LATAB_IDX:
			IEXP_ItemDefPtr=(IEXP_ItemDef *)m_dataptr;
			IC_RELRB(rbb);
			rbb=SDS_tblgetter(IEXP_EntityCol[IEXP_CurrentIndx].cType,IEXP_ItemDefPtr->name(),TRUE,IEXP_ODNode->m_dbDrawing);
			for(rb=rbb;rb;rb=rb->rbnext) {
				switch(rb->restype) {
					case 2:
						if (m_varies) { sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER__D_LAYERS_158, "%d Layers" ),m_Vindex); String=IEXP_szText; } else String=rb->resval.rstring;
						DDX_Text(pDX,IEXP_TABINFO[IEXP_CurrentIndx].ItemID[IEXP_PLA_NAME],String);
						break;
					case 62:
						if (m_newcolor) m_color=abs(rb->resval.rint);
						if (!pDX->m_bSaveAndValidate) lasave.color=m_color;
						Value=(m_varies && IEXP_TABINFO[IEXP_CurrentIndx].Varies[IEXP_PLA_COLOR])?IEXP_VARIES_I:1;
						m_colorVaries=(Value==2)?TRUE:FALSE;
						ColorButton(m_colorVaries);

						Value=(m_varies && IEXP_TABINFO[IEXP_CurrentIndx].Varies[IEXP_PLA_ON_OFF])?IEXP_VARIES_I:((rb->resval.rint<0)?1:0);
						if (!pDX->m_bSaveAndValidate) lasave.OnOff=Value;
						DDX_Check(pDX,IEXP_TABINFO[IEXP_CurrentIndx].ItemID[IEXP_PLA_ON_OFF],Value);
						break;
					case 6:
						if (m_varies && IEXP_TABINFO[IEXP_CurrentIndx].Varies[IEXP_PLA_LINETYPE]) String=ResourceString(IDC_IEXP_VARIES_S, "Varies");
						else String=rb->resval.rstring;
						if (!pDX->m_bSaveAndValidate) lasave.linetype=rb->resval.rstring; else m_combobox.SelectString(-1,rb->resval.rstring);
						DDX_CBString(pDX,IEXP_TABINFO[IEXP_CurrentIndx].ItemID[IEXP_PLA_LINETYPE],String);
						break;
					case 70:
						Value=(m_varies && IEXP_TABINFO[IEXP_CurrentIndx].Varies[IEXP_PLA_FROZEN])?IEXP_VARIES_I:((rb->resval.rint&IC_LAYER_FROZEN)?1:0);
						if (!pDX->m_bSaveAndValidate) lasave.freeze=Value;
						DDX_Check(pDX,IEXP_TABINFO[IEXP_CurrentIndx].ItemID[IEXP_PLA_FROZEN],Value);

						Value=(m_varies && IEXP_TABINFO[IEXP_CurrentIndx].Varies[IEXP_PLA_LOCKED])?IEXP_VARIES_I:((rb->resval.rint&IC_LAYER_LOCKED)?1:0);
						if (!pDX->m_bSaveAndValidate) lasave.lock=Value;
						DDX_Check(pDX,IEXP_TABINFO[IEXP_CurrentIndx].ItemID[IEXP_PLA_LOCKED],Value);

						Value=(m_varies && IEXP_TABINFO[IEXP_CurrentIndx].Varies[IEXP_PLA_VPNEW])?IEXP_VARIES_I:((rb->resval.rint&IC_LAYER_VPFROZEN)?1:0);
						if (!pDX->m_bSaveAndValidate) lasave.vpnew=Value;
						DDX_Check(pDX,IEXP_TABINFO[IEXP_CurrentIndx].ItemID[IEXP_PLA_VPNEW],Value);
						break;
					case 290:
						Value=(m_varies && IEXP_TABINFO[IEXP_CurrentIndx].Varies[IEXP_PLA_PLOT])?IEXP_VARIES_I:rb->resval.rint;								if (!pDX->m_bSaveAndValidate) lasave.plot=Value;
						DDX_Check(pDX,IEXP_TABINFO[IEXP_CurrentIndx].ItemID[IEXP_PLA_PLOT],Value);
						break;
					case 370:
						if (m_varies && IEXP_TABINFO[IEXP_CurrentIndx].Varies[IEXP_PLA_LINEWEIGHT]) {
							String=ResourceString(IDC_IEXP_VARIES_S, "Varies");
						} else {
							int top = ( IEXP_lwunits == 1 ? IDC_COMBO_M_LWDEFAULT : IDC_COMBO_I_LWDEFAULT );
							String=ResourceString(top+db_lweight2index(rb->resval.rint)+3, 
								ResourceString(IDC_IEXP_VARIES_S, "Varies"));
						}
						if (!pDX->m_bSaveAndValidate) lasave.lweight=rb->resval.rint; 
							else m_lwcombobox.SelectString(-1,String);
						DDX_CBString(pDX,IEXP_TABINFO[IEXP_CurrentIndx].ItemID[IEXP_PLA_LINEWEIGHT],String);
						break;
				}
			}
			break;
	}
	IC_RELRB(rbb);
}

void IEXP_SubDlg::OnPaint() {
	CDialog::OnPaint();
	switch(IEXP_EntityCol[IEXP_CurrentIndx].IType) {
		case IEXP_LATAB_IDX: ColorButton(m_colorVaries); break;
	}
}




BEGIN_MESSAGE_MAP(CTreeCtrlEx, CTreeCtrl)
	//{{AFX_MSG_MAP(CTreeCtrlEx)
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTreeCtrlEx message handlers

afx_msg void CTreeCtrlEx::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	short		   Index;
	if ((nChar==VK_ESCAPE) || ((Index=GetKeyState(VK_CONTROL))&IEXP_HI_BIT_SHORT) )
	{
		GetParent()->SendMessage( WM_CHAR, nChar, 0);
	}
	else
	{
		CTreeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
	}
}

// ======================================================================================================================================================================
// ================================================== IEXP_CITEMLIST CLASS FUNCTIONS ====================================================================================
// ======================================================================================================================================================================

IMPLEMENT_DYNCREATE(IEXP_CItemList, CFrameWnd)

BEGIN_MESSAGE_MAP(IEXP_CItemList, CFrameWnd)
	ON_NOTIFY(TVN_SELCHANGED,IEXP_TREEVIEW,OnItmChanged)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_WM_CHAR()
END_MESSAGE_MAP()

LRESULT IEXP_CItemList::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) {
//	switch(message) {
//		case WM_NOTIFY:
//			  return (m_CdetailView->NotifyHandler(message,wParam,lParam));
//	}

	return CWnd::WindowProc(message, wParam, lParam);
}


afx_msg void IEXP_CItemList::OnChar(UINT nChar,UINT nRepCnt,UINT nFlags)
{
	short		   Index;

	if ((Index=GetKeyState(VK_CONTROL))&IEXP_HI_BIT_SHORT)
	{
	  switch(nChar)
	  {
		  case 'a':
		  case 'A': m_CdetailView->SelectAll();			break;
		  case 'x':
		  case 'X': m_CdetailView->CopyCutItem(TRUE);	break;
		  case 'c':
		  case 'C': m_CdetailView->CopyCutItem(FALSE);	break;
		  case 'v':
		  case 'V': m_CdetailView->PasteItem(); 		 break;
		  case 'z':
		  case 'Z': AfxMessageBox(ResourceString(IDC_ICADEXPLORER_UNDO_NOT__130, "Undo not available" )); break;
		  case 'y':
		  case 'Y': AfxMessageBox(ResourceString(IDC_ICADEXPLORER_REDO_NOT__131, "Redo not available" )); break;
	  }
	}

	if (nChar==VK_ESCAPE)
	{
		GetParent()->GetParent()->SendMessage( WM_CHAR, nChar, 0);
	}
	else
	{
		CFrameWnd::OnChar(nChar,nRepCnt,nFlags);
	}
}

void IEXP_CItemList::OnSetFocus(CWnd* pOldWnd)
{
	CFrameWnd::OnSetFocus(pOldWnd);
	m_CdetailView->m_Windowsideflag=TRUE;
}

///////////////////////////////////////////////////////////////////////
// This function is called when a Tree (ITEM) is selected.
//
//
// Nothing is returned.
//
void IEXP_CItemList::OnItmChanged(LPNMHDR pnmhdr, LRESULT *pResult) {
	NM_TREEVIEW *pnmtv=(NM_TREEVIEW *)pnmhdr;
	short fi1;
	HTREEITEM	 hItem;

	switch(pnmhdr->code)
	{
		case TVN_SELCHANGED:
			if (IEXP_hWndView)
			{
				if (m_Explorer->m_editmode)
				{
					m_CdetailView->m_ListCtl[IEXP_CurrentIndx].EndLabelEditing();
					m_Explorer->m_editmode = FALSE; //set this to FALSE, so we will not try to continue editing. Windows sends the
													//EndLabelEdit msg after the item has been switched.
				}
				if (pnmtv->itemNew.lParam>6)
				{
					for(IEXP_ODNode=IEXP_ODFirstNode;IEXP_ODNode->nID!=pnmtv->itemNew.lParam;IEXP_ODNode=IEXP_ODNode->next);
					m_CdetailView->UpdateDataList();
				}
				else
				{
					hItem=m_TreeCtl.GetParentItem(pnmtv->itemNew.hItem);
					if (IEXP_ODNode->hItem!=hItem)
					{
						for(IEXP_ODNode=IEXP_ODFirstNode;IEXP_ODNode->hItem!=hItem;IEXP_ODNode=IEXP_ODNode->next);
						m_CdetailView->UpdateDataList();
					}
				}
				hItem=m_TreeCtl.GetChildItem(IEXP_ODNode->hItem);
				for(fi1=0;fi1<7 && hItem;++fi1)
				{
					IEXP_EntityCol[fi1].hITem=hItem;
					hItem=m_TreeCtl.GetNextSiblingItem(IEXP_EntityCol[fi1].hITem);
				}
				::SendMessage(IEXP_hWndView,IEXP_CHANGEROW,pnmtv->itemOld.lParam,pnmtv->itemNew.lParam);
				break;
			}
	}
}

///////////////////////////////////////////////////////////////////////
// This function is called when the (CFrameWnd) is being created, the
// tree is being created here.
//
// pCreateStruct : Points to a CREATESTRUCT structure that contains information about the CWnd object being created.
//
// OnCreate must return 0 to continue the creation of the CWnd object. If the application returns -1, the window will be destroyed.
//
int IEXP_CItemList::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	CBitmap	bitmap;
	short	index;

   	if (CFrameWnd::OnCreate(lpCreateStruct)==-1) return -1;

	if (!m_staticTvbtm.Create(NULL,SS_LEFTNOWORDWRAP|SS_NOPREFIX|WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,IEXP_SBAR_BTV)) return -1;
	if (!m_staticTvbar.Create(NULL,SS_LEFTNOWORDWRAP|SS_SUNKEN|SS_NOPREFIX|WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,IEXP_SBAR_TV)) return -1;

	CFont* pFont = new CFont();
	if (pFont)
	{
		pFont->CreateStockObject(DEFAULT_GUI_FONT);
		m_staticTvbar.SetFont(pFont);
	}
	m_staticTvbar.SetWindowText(ResourceString(IDC_ICADEXPLORER_ELEMENTS_159, "Elements" ));
	if (pFont) delete pFont;

	// create the tree view control
	if (!m_TreeCtl.Create(WS_VISIBLE|WS_CHILD|WS_BORDER|TVS_HASLINES|TVS_HASBUTTONS|TVS_LINESATROOT,CRect(0,0,0,0),this,IEXP_TREEVIEW)) return -1;
	// Create the image list we will need
	if (!m_ImageList.Create(IEXP_BITMAP_WIDTH,IEXP_BITMAP_HEIGHT,TRUE,((IEXP_NumofTypes-1)*2)+1,0)) return -1;

	bitmap.LoadBitmap(EXP_DRAWING);
	IEXP_WindowIcon=m_ImageList.Add(&bitmap,(COLORREF)0xFFFFFF);
	bitmap.DeleteObject();

	for(index=0;index<(IEXP_NumofTypes-1);++index) {
   		bitmap.LoadBitmap(IEXP_EntityCol[index].IconID);
		IEXP_EntityCol[index].IconOff =m_ImageList.Add(&bitmap,(COLORREF)0xFFFFFF);
		bitmap.DeleteObject();
   		bitmap.LoadBitmap(IEXP_EntityCol[index].SIconID);
		IEXP_EntityCol[index].SIconOff=m_ImageList.Add(&bitmap,(COLORREF)0xFFFFFF);
		bitmap.DeleteObject();
	}

	// Associate the image list with the tree
	m_TreeCtl.SetImageList(&m_ImageList,TVSIL_NORMAL);
	// Add the items to the tree view
	if (!AddTreeViewItems()) return -1;
	// Select the top node in the tree control.
	m_TreeCtl.Select(IEXP_ODNode->hItem,TVGN_CARET);

	return 0;
}




///////////////////////////////////////////////////////////////////////
// This function adds all the types of ITEMS to the (CTreeCtrl)
// control.
//
// TRUE  : returned on success.
// FALSE : returned on error.
//
BOOL IEXP_CItemList::AddTreeViewItems()
{
	short	  index,
			  pIndex=100;
	UINT	  mask=0;
	HTREEITEM hItem;

  	CIcadChildWnd* pChild = (CIcadChildWnd*)SDS_CMainWindow->MDIGetActive(NULL);
	if(pChild==NULL) return(FALSE);
	CIcadDoc* pDoc = (CIcadDoc*)pChild->GetActiveDocument();
	if(pDoc==NULL) return(FALSE);
	CDocTemplate* pDocTemplate = pDoc->GetDocTemplate();
	POSITION pos = pDocTemplate->GetFirstDocPosition();
	CString strDwg;
	while(pos!=NULL)
	{
		pDoc = (CIcadDoc*)pDocTemplate->GetNextDoc(pos);
		IEXP_AddNewOpenDoc();
		IEXP_ODNode->m_dbDrawing=pDoc->m_dbDrawing;
		IEXP_ODNode->nID=pIndex;
		IEXP_ODNode->m_pIcadDoc=pDoc;
		strDwg=pDoc->GetPathName();
		if (strDwg.IsEmpty())
		{
			char fname[IC_PATHSIZE+IC_FILESIZE];
			strncpy(fname,pDoc->GetTitle(),sizeof(fname)-1);
			strDwg=fname;
		}
		IEXP_ODNode->szDrawing=new char[strDwg.GetLength()+1]; strcpy(IEXP_ODNode->szDrawing,(char *)((LPCTSTR)strDwg));
  		if ((IEXP_ODNode->hItem=AddOneItem((HTREEITEM)NULL,IEXP_ODNode->szDrawing,(HTREEITEM)TVI_ROOT,IEXP_WindowIcon,IEXP_WindowIcon,pIndex,mask))==NULL) return(FALSE);
		hItem=IEXP_ODNode->hItem;
		for(mask=0,index=0;index<(IEXP_NumofTypes-1);++index,mask=0)
		{
			if (index==7) mask=TVIF_CHILDREN;
	  		if ((hItem=AddOneItem(IEXP_ODNode->hItem,IEXP_TABINFO[index].Name,hItem,IEXP_EntityCol[index].IconOff,IEXP_EntityCol[index].IconOff,index,mask))==NULL) return(FALSE);
		}
		++pIndex;
	}
	//	Harald Krause 2000-07-21: Prot Error Bug solved (IEXP_ODNode == NULL)
	for(IEXP_ODNode=IEXP_ODFirstNode;IEXP_ODNode && IEXP_ODNode->m_dbDrawing!=SDS_CURDWG;IEXP_ODNode=IEXP_ODNode->next);
	UpdateEntityCOl();
	return(TRUE);
}


void IEXP_CItemList::UpdateEntityCOl(void) {
	HTREEITEM hItem;
	short	  fi1;

	if (IEXP_ODNode) {
		hItem=m_TreeCtl.GetChildItem(IEXP_ODNode->hItem);
		for(fi1=0;fi1<(IEXP_NumofTypes-1);++fi1) {
			if (!fi1) IEXP_EntityCol[fi1].hITem=hItem; else hItem=IEXP_EntityCol[fi1].hITem=m_TreeCtl.GetNextItem(hItem,TVGN_NEXT);
		}
	}
}

///////////////////////////////////////////////////////////////////////
// This function is called when an item to the (CTreeList) control
// is added.
//
// hParent	 : Handle of the inserted item’s parent.
// szText	 :
// hInsAfter : Handle of the item after which the new item is to be inserted.
// iImage	 : Index of the item’s image in the tree view control’s image list.
// SiImage	 : Index of the item’s selected image in the tree view control’s image list.
// index	 :
//
// HTREEITEM : Handle of the new item if successful; otherwise NULL.
//
HTREEITEM IEXP_CItemList::AddOneItem(HTREEITEM hParent,LPCTSTR szText,HTREEITEM hInsAfter,int iImage,int SiImage,short index,UINT mask) {
	HTREEITEM		hItem;
	TV_ITEM 		tvI;
	TV_INSERTSTRUCT tvIns;

	tvI.mask		   = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM | mask;
	tvI.pszText		   = (char *)szText;
	tvI.cchTextMax	   = lstrlen(szText);
	tvI.iImage		   = iImage;
	tvI.iSelectedImage = SiImage;
	tvI.lParam		   = index;
	tvIns.item		   = tvI;
	tvIns.hInsertAfter = hInsAfter;
	tvIns.hParent	   = hParent;
	hItem			   = m_TreeCtl.InsertItem(&tvIns);
	return (hItem);
}

///////////////////////////////////////////////////////////////////////
// The framework calls this member function after the window’s size
// has changed.
//
// nType : Specifies the type of resizing requested. This parameter can be one of the following values:
//			SIZE_MAXIMIZED	 Window has been maximized.
//			SIZE_MINIMIZED	 Window has been minimized.
//			SIZE_RESTORED	Window has been resized, but neither SIZE_MINIMIZED nor SIZE_MAXIMIZED applies.
//			SIZE_MAXHIDE   Message is sent to all pop-up windows when some other window is maximized.
//			SIZE_MAXSHOW   Message is sent to all pop-up windows when some other window has been restored to its former size.
// cx	 : Specifies the new width of the client area.
// cy	 : Specifies the new height of the client area.
//
// This part resizes the Tree Control to the size of CFrame window that it is inside of.
//
// Nothing is returned.
//
void IEXP_CItemList::OnSize(UINT nType, int cx, int cy) {
  	CFrameWnd::OnSize(nType, cx, cy);
	m_TreeCtl.MoveWindow(0,20,cx,(cy-20));
	m_staticTvbtm.MoveWindow(0,0,cx,20);
	m_staticTvbar.MoveWindow(0,0,cx,20);
}

// ======================================================================================================================================================================
// ================================================== IEXP_EXPLORER CLASS FUNCTIONS =====================================================================================
// ======================================================================================================================================================================

IMPLEMENT_DYNCREATE(IEXP_Explorer,CFrameWnd)

BEGIN_MESSAGE_MAP(IEXP_Explorer,CFrameWnd)
	ON_WM_QUERYNEWPALETTE()
	ON_WM_PALETTECHANGED()
	ON_COMMAND_RANGE(EXP_LAYER_ON,EXP_LAYER_LOCK,OnLayerAtt)
	ON_COMMAND_RANGE(EXP_LAYER_ON_M,EXP_LAYER_VISRETAIN_M,OnLayerAtt)
	ON_COMMAND_RANGE(EXP_BLOCK_DETAILS,EXP_BLOCK_XREF,OnBlkListType)
	ON_COMMAND_RANGE(EXP_NEW_LAYER_M,EXP_NEW_FORM_M,OnInsertRow)
	ON_COMMAND(EXP_DELETE_M,OnRemoveItem)
	ON_COMMAND(EXP_RENAME_M,OnRenameItem)
	ON_COMMAND(EXP_PROPERTIES,PropertyDlg)
	ON_COMMAND(EXP_UCS_WORLD,OnWorld)
	ON_COMMAND(EXP_NEW_T,OnNewItem)
	ON_COMMAND(EXP_COPY,OnCopyItem)
	ON_COMMAND(EXP_PASTE,OnPasteItem)
	ON_COMMAND(EXP_CUT,OnCutItem)
	ON_COMMAND(ICAD_EXP_HELP,OnHelp)
	ON_COMMAND(EXP_CURRENT_M,OnCurrent)
	ON_COMMAND(ID_VIEW_REDRAW,OnRedraw)
	ON_COMMAND(ID_VIEW_REGEN,OnRegen)
	ON_COMMAND(EXP_SELECT_ALL,OnSelectAll)
	ON_COMMAND(EXP_PURGE,OnPurge)
	ON_COMMAND(EXP_REGEN,OnRegen)
	ON_COMMAND(EXP_REGEN_SWITCH,OnRegenSwitch)
//	  ON_COMMAND(EXP_TOOLBAR,OnToolbarState)
	ON_WM_CONTEXTMENU()
	ON_WM_CLOSE()
	ON_WM_CHAR()
	ON_WM_HELPINFO()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONUP()
	ON_WM_NCLBUTTONDOWN()
END_MESSAGE_MAP()

afx_msg void IEXP_Explorer::OnRedraw()
{
	if (m_editmode)
		m_CdetailView->m_ListCtl[IEXP_CurrentIndx].EndLabelEditing();
	sds_redraw(NULL,IC_REDRAW_DRAW);
}

afx_msg void IEXP_Explorer::OnRegen()
{
	if (m_editmode)
		m_CdetailView->m_ListCtl[IEXP_CurrentIndx].EndLabelEditing();
	cmd_regenall();
}

afx_msg void IEXP_Explorer::OnRegenSwitch()
{
	if(IEXP_ODNode == NULL)
		return;

	if (m_editmode)
		m_CdetailView->m_ListCtl[IEXP_CurrentIndx].EndLabelEditing();
	resbuf rm;
	rm.restype = RTSHORT;
	if(m_regenMode == 1)
	{
		m_regenMode = 0;
		rm.resval.rint = 0;
		m_MToolBar.GetToolBarCtrl().PressButton(EXP_REGEN_SWITCH, FALSE);
	}
	else
	{
		m_regenMode = 1;
		rm.resval.rint = 1;
		m_MToolBar.GetToolBarCtrl().PressButton(EXP_REGEN_SWITCH, TRUE);
	}
	SDS_setvar(NULL, DB_QREGENMODE, &rm, IEXP_ODNode->m_dbDrawing, NULL, NULL, 0);
}

BOOL IEXP_Explorer::ActivateCurrentView(db_drawing *dp) {

  	CIcadChildWnd* pChild = (CIcadChildWnd*)SDS_CMainWindow->MDIGetActive(NULL);
	if(pChild==NULL) return(FALSE);
	CIcadDoc* pDoc = (CIcadDoc*)pChild->GetActiveDocument();
	if(pDoc==NULL) return(FALSE);
	CDocTemplate* pDocTemplate = pDoc->GetDocTemplate();
	POSITION pos = pDocTemplate->GetFirstDocPosition();
	while(pos!=NULL) {
		pDoc = (CIcadDoc*)pDocTemplate->GetNextDoc(pos);
		if (dp==pDoc->m_dbDrawing) {
			POSITION pos2=pDoc->GetFirstViewPosition();
			CIcadView *pView=(CIcadView *)pDoc->GetNextView(pos2);
			ASSERT_KINDOF(CIcadView, pView);

			pView->SetNewView(TRUE);
			((CIcadChildWnd*)pView->GetParentFrame())->SetWindowPos(&wndTop,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
			return(TRUE);
		}
	}
	return(FALSE);
}

afx_msg void IEXP_Explorer::OnNcLButtonDown(UINT nHitTest,CPoint point)
{
	if (nHitTest==HTMAXBUTTON) GetWindowRect(m_rectScrn);
	CFrameWnd::OnNcLButtonDown(nHitTest,point);
}

afx_msg void IEXP_Explorer::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_pDragImage)
	{
		// End the dragging of a menu item.
		m_pDragImage->EndDrag();
		delete m_pDragImage;
		m_pDragImage=NULL;
//		  ReleaseCapture();
	}
	CFrameWnd::OnLButtonUp(nFlags,point);
}

afx_msg BOOL IEXP_Explorer::OnSetCursor(CWnd* pWnd,UINT nHitTest,UINT message) {
	POINT  point;
	CPoint cPt;

	if (m_pDragImage) {
		// Get the cursor position.
		GetCursorPos(&point);
		// Transform the point to client coordinates of the tree control.
		cPt.x=point.x;
		cPt.y=point.y;
		// Draw the item.
		m_pDragImage->DragMove(cPt);
		m_pDragImage->DragShowNolock(TRUE);
//		  return(TRUE);
	}
	return(CFrameWnd::OnSetCursor(pWnd,nHitTest,message));
}

afx_msg void IEXP_Explorer::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar==VK_ESCAPE)
	{
		OnClose();
	}
	else
	{
		CFrameWnd::OnChar(nChar, nRepCnt, nFlags);
	}
}
BOOL IEXP_Explorer::DestroyWindow()
{
	m_MainWindow->m_pExplorer = NULL;
	for(short fi1=0;fi1<(IEXP_NumofTypes-1);++fi1)
	{
		for(short fi3=0, fi2 = m_CdetailView->m_ListCtl[fi1].GetItemCount();fi3<fi2;++fi3)
			delete (IEXP_ItemDef *)m_CdetailView->m_ListCtl[fi1].GetItemData(fi3);
	}

	IEXP_ItemDefPtr=NULL;
	if (m_CdetailView->m_pimagelistI) m_CdetailView->m_pimagelistI.DeleteImageList();
	if (m_CdetailView->m_pimagelist) m_CdetailView->m_pimagelist.DeleteImageList();
	if (m_CItemList->m_ImageList) m_CItemList->m_ImageList.DeleteImageList();
	IEXP_FreeOpenDoc();
	m_splitWnd.DestroyWindow();
	return CFrameWnd::DestroyWindow();
}
///////////////////////////////////////////////////////////////////////
// This function is called when the explorer is about to be terminated,
// so here we get all the information such as (width,height,position of
// sertain windows and such and we save them in setvars, that get
// stored in the regester database.
//
// Nothing is returned.
//
void IEXP_Explorer::OnClose() {
	struct resbuf rbb;
	int 		  width,
				  widthm;
	short		  index,
				  Cnt;
	char		  String[50],
				  IntStr[10];

	if (m_CdetailView->m_DlgState) {
		if (m_CdetailView->m_EditDlg->m_SubDlg.m_Inuse) return;
	}
	if (m_CdetailView->m_EditDlg) {
		delete m_CdetailView->m_EditDlg;
		m_CdetailView->m_EditDlg=NULL;
	}
	if (m_editmode)
	{
		m_CdetailView->m_ListCtl[IEXP_CurrentIndx].EndLabelEditing();
		m_editmode = FALSE;
	}

	// Put this in here for setvar stuff.
	if(m_MainWindow->m_ExpReturnHwnd) {
		::ExecuteUIAction( ICAD_WNDACTION_EXP_RETURN );
		m_MainWindow->m_ExpReturnHwnd=NULL;
	}

	m_MainWindow->m_bExplorerState=FALSE;
	m_MainWindow->m_bExpInsertState=FALSE;

	if (this->IsZoomed()) {
		rbb.resval.rint=2; // Window is maximized.
	} else if (this->IsIconic()) {
		rbb.resval.rint=0; // Window is Iconic.
	} else {
		rbb.resval.rint=1; // Window is minimized.
		GetWindowRect(m_rectScrn);
	}

	rbb.restype 		=RTPOINT;
	rbb.resval.rpoint[0]=m_rectScrn.left;
	rbb.resval.rpoint[1]=m_rectScrn.top;
	sds_setvar("WNDPTEXP"/*DNT*/,&rbb);

	rbb.resval.rpoint[0]=m_rectScrn.right;
	rbb.resval.rpoint[1]=m_rectScrn.bottom;
	sds_setvar("WNDSTEXP"/*DNT*/,&rbb);

	m_splitWnd.GetColumnInfo(0,width,widthm);
	rbb.restype 		=RTSHORT;
	rbb.resval.rint 	=width;
	sds_setvar("WNDBTEXP"/*DNT*/,&rbb);
	// Get the status bar state.
	rbb.restype 		=RTSHORT;
	rbb.resval.rint=m_StatusBar.IsWindowVisible(); sds_setvar("WNDLSTAT"/*DNT*/,&rbb);
	// Get the toolbar state.
	rbb.restype 		=RTSHORT;
	rbb.resval.rint=m_MToolBar.IsWindowVisible(); sds_setvar("WNDLTEXP"/*DNT*/,&rbb);

	rbb.restype=RTSTR;
	for(index=0;index<((IEXP_NumofTypes-1));++index) {
		for(Cnt=0;Cnt<IEXP_EntityCol[index].Cnt;++Cnt) {
			if (index==5 && m_CdetailView->m_BlockState==1) {
				sprintf(IntStr,"%d"/*DNT*/,IEXP_EntityCol[index].Order[Cnt][1]);
			} else {
				sprintf(IntStr,"%d"/*DNT*/,m_CdetailView->m_ListCtl[index].GetColumnWidth(Cnt));
			}
			if (!Cnt) strcpy(String,IntStr); else { strcat(String,IEXP_SCOMMA); strcat(String,IntStr); }
		}
		rbb.resval.rstring=String;
		sds_setvar(IEXP_EntityCol[index].resname,&rbb);
	}
	cmd_regenall();
	if (m_CdetailView->m_regenstate)
		SDS_SetUndo(IC_UNDO_REGEN_ON_REDO,NULL,NULL,NULL,SDS_CURDWG);
	// DP: invalidate last cursor position to prevent garbage cursor on the screen
	SDS_CURVIEW->m_LastCursorPos.x = -1;
	SDS_CURVIEW->m_LastCursorPos.y = -1;

	m_MainWindow->EndModalState();
	m_MainWindow->SetFocus();

	m_MainWindow->m_bIsAppActive=TRUE;

	CFrameWnd::OnClose();

	extern char *SDS_UndoCmdName;
	SDS_SetUndo(IC_UNDO_GROUP_END,(void *)SDS_UndoCmdName,NULL,NULL,SDS_CURDWG);

	if (SDS_CMainWindow->m_wndDimstyle.GetSafeHwnd())
		SDS_CMainWindow->m_wndDimstyle.UpdateDimStyleList();  

	extern bool SDS_InExplorer;
	SDS_InExplorer = false;
}

///////////////////////////////////////////////////////////////////////
// This function is called when a right click on the splitter window
// has been done, this brings  up a menu to select from.
//
// Nothing is returned
//
void IEXP_Explorer::OnContextMenu(CWnd* pWnd, CPoint pos) {
	CMenu menubar;
	CRect Srect,
		  Trect;
	CPoint Sclientpt(pos),
		   Tclientpt(pos);

	if (m_editmode) return;

	m_StatusBar.ScreenToClient(&Sclientpt);
	m_MToolBar.ScreenToClient(&Tclientpt);

	m_StatusBar.GetClientRect(Srect);
	m_MToolBar.GetClientRect(Trect);

	if ((Sclientpt.x>Srect.left && Sclientpt.x<Srect.right && Sclientpt.y>Srect.top && Sclientpt.y<Srect.bottom) ||
		(Tclientpt.x>Trect.left && Tclientpt.x<Trect.right && Tclientpt.y>Trect.top && Tclientpt.y<Trect.bottom)) {
		if (menubar.LoadMenu(EXP_EXPLORER_MNU)) {
			CMenu* pPopup = menubar.GetSubMenu(1);
			pPopup->TrackPopupMenu(TPM_RIGHTBUTTON,pos.x,pos.y,this);
		}
		return;
	}
	CFrameWnd::OnContextMenu(pWnd,pos);
}

///////////////////////////////////////////////////////////////////////
// This function is called when the (CFrameWnd) is being created, the
// "status bar,tool bar, and splitter window" are being created here.
//
// lpcs 	: A pointer to a Windows CREATESTRUCT structure.
// pContext : A pointer to a CCreateContext structure.
//
// TRUE  : returned on success.
// FALSE : returned on error.
//
BOOL IEXP_Explorer::OnCreateClient(LPCREATESTRUCT lpcs,CCreateContext* pContext) {
	CRect		  rectScrn,
				  rect,
				  crect;
	struct resbuf rbb;
	long		  style;
	short		  fi1,
				  fi2;

	CFrameWnd::OnCreateClient(lpcs,pContext);

	GetClientRect(rectScrn);

	IEXP_InsertFlag=FALSE;

	// Set the pointer to the listview window to NULL.
	IEXP_hWndView=NULL;
	// Set the point to the drag imagelist to NULL.
	m_pDragImage =NULL;

	// Create the status bar.
	if (!m_StatusBar.Create(this)) return(FALSE);
	// Set the indicators in it.
	if (!m_StatusBar.SetIndicators(indicators,sizeof(indicators)/sizeof(UINT)))	return(FALSE);
	// Get the status of the status bar.
	sds_getvar("WNDLSTAT"/*DNT*/,&rbb);
	// Disable the status bar if status is 0
	if (!rbb.resval.rint) m_StatusBar.ShowWindow(SW_HIDE);

	CToolBarCtrl *TbCtrl;

	// Create the main toolbar
   	if (!m_MToolBar.Create(this,WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_FLOATING | CBRS_SIZE_DYNAMIC)) return(FALSE);
#ifndef ICAD_OLDTOOLBARS
	TbCtrl = &m_MToolBar.GetToolBarCtrl();
	TbCtrl->ModifyStyle(0,TBSTYLE_FLAT);
	m_MToolBar.m_cxLeftBorder=11;
	m_MToolBar.m_cxRightBorder=3;
	m_MToolBar.m_bIsStyleFlat=TRUE;
	m_MToolBar.SetDllVersion();
#endif
	// Load the toolbar bitmap.
	if (!m_MToolBar.LoadBitmap(EXP_TOOLBAR_TBAR)) return(FALSE);
	// Set the toolbar button id's.
	if (!m_MToolBar.SetButtons(IEXP_MainTBId,sizeof(IEXP_MainTBId)/sizeof(UINT))) return(FALSE);
	// Set the toolbar title.
	m_MToolBar.SetWindowText(_T(ResourceString(IDC_ICADEXPLORER_STANDARD_165, "Standard" )));
	// Set the alignment of the toolbar.
	m_MToolBar.EnableDocking(CBRS_ALIGN_ANY);

	resbuf rm;
	SDS_getvar(NULL, DB_QREGENMODE, &rm, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
	m_regenMode = rm.resval.rint;
	m_MToolBar.GetToolBarCtrl().PressButton(EXP_REGEN_SWITCH, m_regenMode == 0 ? FALSE : TRUE);

	// Create the layer toolbar.
   	if (!m_LToolBar.Create(this,WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_FLOATING | CBRS_SIZE_DYNAMIC)) return(FALSE);
#ifndef ICAD_OLDTOOLBARS
	TbCtrl = &m_LToolBar.GetToolBarCtrl();
	TbCtrl->ModifyStyle(0,TBSTYLE_FLAT);
	m_LToolBar.m_cxLeftBorder=11;
	m_LToolBar.m_cxRightBorder=3;
	m_LToolBar.m_bIsStyleFlat=TRUE;
	m_LToolBar.SetDllVersion();
#endif
	// Load the toolbar bitmap.
	if (!m_LToolBar.LoadBitmap(EXP_LAYER_TBAR)) return(FALSE);
	// Set the toolbar button id's.
	if (!m_LToolBar.SetButtons(IEXP_LayerTBId,sizeof(IEXP_LayerTBId)/sizeof(UINT))) return(FALSE);
	// Set the style of the buttons to checkbox.
	for(fi2=0,fi1=(sizeof(IEXP_LayerTBId)/sizeof(UINT));fi2<fi1;++fi2) {
		style=m_LToolBar.GetButtonStyle(fi2);
		style|=TBBS_CHECKBOX;
		m_LToolBar.SetButtonStyle(fi2,style);
	}
	// Set the toolbar title.
	m_LToolBar.SetWindowText(_T(ResourceString(IDC_ICADEXPLORER_LAYER_39, "Layer" )));
	// Set the alignment of the toolbar.
	m_LToolBar.EnableDocking(CBRS_ALIGN_ANY);

	// Create the blocks toolbar.
   	if (!m_BlkToolBar.Create(this,WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_FLOATING | CBRS_SIZE_DYNAMIC)) return(FALSE);
#ifndef ICAD_OLDTOOLBARS
	TbCtrl = &m_BlkToolBar.GetToolBarCtrl();
	TbCtrl->ModifyStyle(0,TBSTYLE_FLAT);
	m_BlkToolBar.m_cxLeftBorder=11;
	m_BlkToolBar.m_cxRightBorder=3;
	m_BlkToolBar.m_bIsStyleFlat=TRUE;
	m_BlkToolBar.SetDllVersion();
#endif
	// Load the toolbar bitmap.
	if (!m_BlkToolBar.LoadBitmap(EXP_BLOCK_VIEWTB)) return(FALSE);
	// Set the toolbar button id's.
	if (!m_BlkToolBar.SetButtons(IEXP_BlockTBId,sizeof(IEXP_BlockTBId)/sizeof(UINT))) return(FALSE);
	// Set the style of the buttons to checkbox.
	for(fi2=0;fi2<2;++fi2) {
		style=m_BlkToolBar.GetButtonStyle(fi2);
		style|=TBBS_CHECKGROUP;
		m_BlkToolBar.SetButtonStyle(fi2,style);
	}
	// Set the toolbar title.
	m_BlkToolBar.SetWindowText(_T(ResourceString(IDC_ICADEXPLORER_BLOCK_166, "Block" )));
	// Set the alignment of the toolbar.
	m_BlkToolBar.EnableDocking(CBRS_ALIGN_ANY);

	// Set the docking flags on, and the docking position.
  	EnableDocking(CBRS_ALIGN_TOP);
	DockControlBar(&m_MToolBar,AFX_IDW_DOCKBAR_TOP);
 	RecalcLayout();
	m_MToolBar.GetWindowRect(&rect);
	rect.OffsetRect((rect.right-(rect.left+2)),0);
	DockControlBar(&m_LToolBar,AFX_IDW_DOCKBAR_TOP,&rect);
	DockControlBar(&m_BlkToolBar,AFX_IDW_DOCKBAR_TOP,&rect);

	// Hide the blocks toolbar.
	m_BlkToolBar.ShowWindow(SW_HIDE);
	sds_getvar("WNDLTEXP"/*DNT*/,&rbb);
	if (!rbb.resval.rint) {
		// Show the main toolbar.
		m_MToolBar.ShowWindow(SW_HIDE);
	}

	sds_getvar("WNDBTEXP"/*DNT*/,&rbb);
	if (rbb.resval.rint) rectScrn.right-=rbb.resval.rint; else { rectScrn.right-=200; rbb.resval.rint=200; }
	// Create the static bar.
	if (!m_splitWnd.CreateStatic(this,1,2)) return(FALSE);
	// Create the tree control view window.
	if (!m_splitWnd.CreateView(0,0,RUNTIME_CLASS(IEXP_CItemList),CSize(rbb.resval.rint,0),pContext)) return(FALSE);
	// Create the list controls view window.
	if (!m_splitWnd.CreateView(0,1,RUNTIME_CLASS(IEXP_CdetailView),CSize(rectScrn.right,0),pContext)) return(FALSE);

	// Get the Cwind for the tree control view window.
	m_CItemList   = (IEXP_CItemList *)	m_splitWnd.GetPane(0,0);
	// Get the Cwind for the list controls view window.
	m_CdetailView = (IEXP_CdetailView *)m_splitWnd.GetPane(0,1);

	// Make sure the tree control view window can access the list control view window and vise versa.
	m_CItemList->m_CdetailView=m_CdetailView;
	m_CdetailView->m_CItemList=m_CItemList;

	// Get the list controls view window handle.
	IEXP_hWndList=m_CItemList->m_hWnd;
	// Get the tree controls view window handle.
	IEXP_hWndView=m_CdetailView->m_hWnd;

	// Make sure the list controls view window can access the main explorer window.
	m_CItemList->m_Explorer=m_CdetailView->m_Explorer=this;
	m_editmode=FALSE;

	// Modified PK 09/08/2000
	// Reason : Fix to slow layer explorer problem
	// m_CdetailView->ChangeWindow((WPARAM)-1,(LPARAM)m_initialwin);					 // Made this change for new explorer ALP'O
	// Select the Corect child node in the tree control.
	m_CItemList->m_TreeCtl.Select(IEXP_EntityCol[m_initialwin].hITem,TVGN_CARET);	  // Made this change for new explorer ALP'O
	// Set focus to the first node in the tree control.
	m_CItemList->m_TreeCtl.SetFocus();												  // Made this change for new explorer ALP'O
	// Set the first item of the list contol to have focus.
	m_CdetailView->m_ListCtl[m_initialwin].EnsureVisible(0,FALSE);

	m_CdetailView->m_Windowsideflag=TRUE;
	// Set the blocks state to one for Icon mode as default.
	m_CdetailView->m_BlockState=g_BlockState;

	extern char *SDS_UndoCmdName;

		// force the normal commandthread bracket to close
	SDS_SetUndo( IC_UNDO_COMMAND_END,(void *)SDS_UndoCmdName,NULL,NULL,SDS_CURDWG);
		// then reopen a group for the dialog
	SDS_SetUndo(IC_UNDO_GROUP_BEGIN,(void *)SDS_UndoCmdName,NULL,NULL,SDS_CURDWG);

	extern bool SDS_InExplorer;
	SDS_InExplorer = true;

	return(TRUE);
}

void IEXP_Explorer::EnableTbB(CIcadToolBar *Toolbar,UINT id,BOOL type) {
	CToolBarCtrl &TCtrl=Toolbar->GetToolBarCtrl();

	TCtrl.EnableButton(id,type);
}


void IEXP_Explorer::OnToolbarState() {
//	  long	  style;

//	  style=m_BlkToolBar.GetWindowLong(GWL_STYLE);
//	  if (style&SW_HIDE)!=SW_HIDE) {
	if (m_BlkToolBar.IsWindowEnabled()) {
		m_BlkToolBar.ShowWindow(SW_HIDE);
	}
//	  style=m_LToolBar.GetWindowLong(GWL_STYLE);
//	  if (style&SW_HIDE)!=SW_HIDE) {
	if (m_LToolBar.IsWindowEnabled()) {
		m_LToolBar.ShowWindow(SW_HIDE);
	}
//	  style=m_MToolBar.GetWindowLong(GWL_STYLE);
//	  if (style&SW_HIDE)!=SW_HIDE) {
	if (m_MToolBar.IsWindowEnabled()) {
		m_MToolBar.ShowWindow(SW_HIDE);
	}
	RedrawWindow(NULL,NULL,RDW_UPDATENOW|RDW_INVALIDATE|RDW_ERASE);
//	  UpdateWindow();
	// I need to send the expolorer a message to repaint it self.
}

void IEXP_Explorer::UpdBlockState(int type) {
	CMenu		  *Menu,
				  *SubMenu;
	long		   style;
	struct resbuf *rb;
	CBitmap		  *cBitmap;
	short		   fi1=0;
	CDialog 	   MessageBox;
	CString 	   string;

	CToolBarCtrl &TCtrl=m_BlkToolBar.GetToolBarCtrl();
	rb=SDS_tblgetter(IEXP_EntityCol[IEXP_CurrentIndx].cType,NULL,TRUE,IEXP_ODNode->m_dbDrawing);
	Menu=GetMenu();
	SubMenu=Menu->GetSubMenu(1);
	SubMenu->CheckMenuItem(6,((type==EXP_BLOCK_IMAGES)?MF_CHECKED:MF_UNCHECKED)|MF_BYPOSITION);
	SubMenu->CheckMenuItem(7,((type==EXP_BLOCK_DETAILS)?MF_CHECKED:MF_UNCHECKED)|MF_BYPOSITION);
	if (!rb) {
		TCtrl.EnableButton(EXP_BLOCK_INSERT,FALSE);
		TCtrl.EnableButton(EXP_BLOCK_SAVE,FALSE);
	} else {
		TCtrl.EnableButton(EXP_BLOCK_INSERT,TRUE);
		TCtrl.EnableButton(EXP_BLOCK_SAVE,TRUE);
	}
	style=GetWindowLong(m_CdetailView->m_ListCtl[IEXP_CurrentIndx].m_hWnd,GWL_STYLE);
	switch(type) {
		case EXP_BLOCK_IMAGES:
			if (!m_CdetailView->m_ListCtl[IEXP_BNTAB_IDX].GetImageList(LVSIL_NORMAL)) {
				MessageBox.Create(IDD_MODLESS_MESSAGE_BOX,this);
				MessageBox.SetWindowText((LPCTSTR)(char *)ResourceString(IDC_ICADEXPLORER_LOADING_B_167, "Loading blocks list" ));
				MessageBox.ShowWindow(SW_SHOW);
				if(rb) sds_relrb(rb);
				rb=SDS_tblgetter(IEXP_EntityCol[IEXP_BNTAB_IDX].cType,NULL,TRUE,IEXP_ODNode->m_dbDrawing);
				while(rb)
				{
					if (ic_assoc(rb,70))
					{
						if(rb) sds_relrb(rb);
						return;
					}
					if (!(ic_rbassoc->resval.rint&IEXP_DEL_FLG))
					{
						BOOL bXref = FALSE;
						if (ic_rbassoc->resval.rint & IC_BLOCK_XREF) //this is an XREF
						{
							if (ic_assoc(rb,1)) //find the xref file name
							{
								if(rb) sds_relrb(rb);
								return;
							}
							bXref = TRUE;
						}
						else //this is a regular block
						{
							if (ic_assoc(rb,2)) //find the block name
							{
								if(rb) sds_relrb(rb);
								return;
							}
						}
						if (*ic_rbassoc->resval.rstring!='*')
						{
							sprintf(IEXP_szText,ResourceString(IDC_ICADEXPLORER_CREATING__168, "Creating blocks (%d)" ),++fi1);
							string=IEXP_szText;
							MessageBox.SetDlgItemText(IDD_TEXT_PROMPT,string);

							if (bXref)
								cBitmap=SDS_DrawBitmap(NULL,ic_rbassoc->resval.rstring,NULL,NULL,IEXP_ICON_BITMAP_WIDTH,IEXP_ICON_BITMAP_HEIGHT,TRUE);
							else
								cBitmap=SDS_DrawBitmap(IEXP_ODNode->m_dbDrawing,NULL,ic_rbassoc->resval.rstring,NULL,IEXP_ICON_BITMAP_WIDTH,IEXP_ICON_BITMAP_HEIGHT,TRUE);

							m_CdetailView->m_pimagelistI.Add(cBitmap,(COLORREF)0xFFFFFF);
							delete cBitmap;
						}
					}
					sds_relrb(rb);
					rb=SDS_tblgetter(IEXP_EntityCol[IEXP_BNTAB_IDX].cType,NULL,FALSE,IEXP_ODNode->m_dbDrawing);
				};
				m_CdetailView->m_ListCtl[IEXP_BNTAB_IDX].SetImageList(&m_CdetailView->m_pimagelistI,LVSIL_NORMAL);
				MessageBox.DestroyWindow();
			}
			TCtrl.CheckButton(EXP_BLOCK_IMAGES,TRUE);
			if ((style&LVS_REPORT)==LVS_REPORT) { style^=LVS_REPORT; style|=LVS_ICON; }
			break;
		case EXP_BLOCK_DETAILS:
			TCtrl.CheckButton(EXP_BLOCK_DETAILS,TRUE);
			if ((style&LVS_ICON)==LVS_ICON) { style^=LVS_ICON; style|=LVS_REPORT; }
			break;
	}
	if(rb) sds_relrb(rb);
	SetWindowLong(m_CdetailView->m_ListCtl[IEXP_CurrentIndx].m_hWnd,GWL_STYLE,style);
	m_CdetailView->m_ListCtl[IEXP_CurrentIndx].UpdateWindow();
}

void IEXP_Explorer::BlockEnableM(BOOL state) {
	CMenu  *Menu,
		   *SubMenu;

	Menu=GetMenu();
	SubMenu=Menu->GetSubMenu(1);
	if (!state) {
		if (SubMenu->GetMenuItemCount()==5) return;
		SubMenu->RemoveMenu(7,MF_BYPOSITION);
		SubMenu->RemoveMenu(6,MF_BYPOSITION);
		SubMenu->RemoveMenu(5,MF_BYPOSITION);
	} else {
		if (SubMenu->GetMenuItemCount()>5) return;
		SubMenu->AppendMenu(MF_SEPARATOR);
		SubMenu->AppendMenu(MF_STRING,EXP_BLOCK_IMAGES,ResourceString(IDC_ICADEXPLORER_IMAGES_169, "Images" ));
		SubMenu->AppendMenu(MF_STRING,EXP_BLOCK_DETAILS,ResourceString(IDC_ICADEXPLORER_DETAILS_170, "Details" ));
	}
	SubMenu=Menu->GetSubMenu(0);
	SubMenu->EnableMenuItem(6,((!state)?MF_ENABLED:MF_GRAYED)|MF_BYPOSITION);
}

void IEXP_Explorer::UpdLayerState(LPARAM Dataptr,short type) {
	CMenu		  *Menu,
				  *SubMenu;
	struct resbuf *rbb,
				   rb;

	CToolBarCtrl &TCtrl=m_LToolBar.GetToolBarCtrl();

	IEXP_ItemDefPtr=(IEXP_ItemDef *)Dataptr;
	rbb=SDS_tblgetter(IEXP_EntityCol[IEXP_CurrentIndx].cType,IEXP_ItemDefPtr->name(),TRUE,IEXP_ODNode->m_dbDrawing);
	Menu=GetMenu();
	SubMenu=Menu->GetSubMenu(1);

	if (ic_assoc(rbb,62)) return; else rb.resval.rint=ic_rbassoc->resval.rint;
	if (ic_assoc(rbb,70)) return;
	if (type&1) {
		SubMenu->CheckMenuItem(6,((rb.resval.rint<0)?MF_CHECKED:MF_UNCHECKED)|MF_BYPOSITION);
		SubMenu->CheckMenuItem(7,((ic_rbassoc->resval.rint&IC_LAYER_FROZEN)?MF_CHECKED:MF_UNCHECKED)|MF_BYPOSITION);
		SubMenu->CheckMenuItem(8,((ic_rbassoc->resval.rint&IC_LAYER_LOCKED)?MF_CHECKED:MF_UNCHECKED)|MF_BYPOSITION);
		SubMenu->CheckMenuItem(10,((IEXP_CdetailView::m_ShowXrefLayers)?MF_CHECKED:MF_UNCHECKED)|MF_BYPOSITION);	}
	if (type&2) {
		TCtrl.CheckButton(EXP_LAYER_ON,((rb.resval.rint<0)?TRUE:FALSE));
		TCtrl.CheckButton(EXP_LAYER_FREEZE,((ic_rbassoc->resval.rint&IC_LAYER_FROZEN)?TRUE:FALSE));
		TCtrl.CheckButton(EXP_LAYER_LOCK,((ic_rbassoc->resval.rint&IC_LAYER_LOCKED)?TRUE:FALSE));
	}
	if(rbb) sds_relrb(rbb);
}

void IEXP_Explorer::LayerEnableM(BOOL state) {
	CMenu		 *Menu,
				 *SubMenu;
	CToolBarCtrl &TCtrl=m_LToolBar.GetToolBarCtrl();

	Menu=GetMenu();
	SubMenu=Menu->GetSubMenu(1);
	if (!state) {
		if (SubMenu->GetMenuItemCount()==5) return;
		SubMenu->RemoveMenu(10,MF_BYPOSITION);
		SubMenu->RemoveMenu(9,MF_BYPOSITION);
		SubMenu->RemoveMenu(8,MF_BYPOSITION);
		SubMenu->RemoveMenu(7,MF_BYPOSITION);
		SubMenu->RemoveMenu(6,MF_BYPOSITION);
		SubMenu->RemoveMenu(5,MF_BYPOSITION);
	} else {
		if (SubMenu->GetMenuItemCount()>5) return;
		SubMenu->AppendMenu(MF_SEPARATOR);
		SubMenu->AppendMenu(MF_STRING|MF_GRAYED,EXP_LAYER_ON_M,ResourceString(IDC_ICADEXPLORER_ON_OFF_171, "On/Off" ));
		SubMenu->AppendMenu(MF_STRING|MF_GRAYED,EXP_LAYER_FREEZE_M,ResourceString(IDC_ICADEXPLORER_FREEZE_TH_172, "Freeze/Thaw" ));
		SubMenu->AppendMenu(MF_STRING|MF_GRAYED,EXP_LAYER_LOCK_M,ResourceString(IDC_ICADEXPLORER_LOCK_UNLO_173, "Lock/Unlock" ));
		SubMenu->AppendMenu(MF_SEPARATOR);
		SubMenu->AppendMenu(MF_STRING|MF_GRAYED,EXP_LAYER_VISRETAIN_M,ResourceString(IDC_ICADEXPLORER_VISRETAIN, "&XRef Layer Visibility"));

		SubMenu->CheckMenuItem(10,((IEXP_CdetailView::m_ShowXrefLayers)?MF_CHECKED:MF_UNCHECKED)|MF_BYPOSITION);

		TCtrl.EnableButton(EXP_LAYER_ON,FALSE);
		TCtrl.EnableButton(EXP_LAYER_FREEZE,FALSE);
		TCtrl.EnableButton(EXP_LAYER_LOCK,FALSE);
	}
}

void IEXP_Explorer::ShowExpolorer(void) {
	struct resbuf  rborg,
				   rbxdir,
				   rbydir,
				  *rbb;
	short		   Cnt,
				   Index,
				   sindex;
   	LV_ITEM 	   lvI;
	BOOL		   NameExist;
	CString 	   String;
	db_handitem   *handItem;
	short		   fi1;

	m_MainWindow->m_bExplorerState=TRUE;
	m_MainWindow->m_bExpInsertState=FALSE;
	IEXP_InsertFlag=TRUE;
	m_MainWindow->BeginModalState();
	ShowWindow(SW_SHOWNORMAL);

	lvI.mask	  =LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
	lvI.state	  =0;
	lvI.stateMask =0;

	switch(m_CdetailView->m_MessageType)
	{
		case IEXP_VWTAB_IDX:
			rbb=SDS_tblgetter(IEXP_EntityCol[IEXP_CurrentIndx].cType,IEXP_szText,TRUE,IEXP_ODNode->m_dbDrawing);
			Cnt=IEXP_ViewCnt;
			// Insert a new row for the View in the (CListCtrl).
			if (!m_CdetailView->ViewIns(Cnt,0,IEXP_CurrentIndx,NULL,lvI,rbb)) return;
			sds_relrb(rbb);
			//Set this newly created view to the current one
			m_CdetailView->m_ListCtl[IEXP_CurrentIndx].SetItemState( Cnt, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
			m_CdetailView->CurrentItem();
			break;

		case IEXP_BNTAB_IDX: // Insert Block.
			if (!m_CdetailView->m_BlockInsCreate)
			{
				//Since we have created a new block whose name we do not know, we have to delete the
				//entire control and recreate it.
				m_CdetailView->UpdateDataList();
				m_CdetailView->m_ListCtl[IEXP_CurrentIndx].EnableWindow(TRUE);
				m_CdetailView->m_ListCtl[IEXP_CurrentIndx].ShowWindow(SW_SHOW);
			}
			IEXP_Findinfo.psz	=IEXP_szText;
			Cnt=m_CdetailView->m_ListCtl[IEXP_CurrentIndx].FindItem(&IEXP_Findinfo);
			if (m_CdetailView->m_BlockInsCreate)
			{
				handItem=NULL; fi1=0;
				//Count the number of references of this block
				fi1 = GetBlockCount (IEXP_szText, IEXP_ODNode->m_dbDrawing, TRUE);
				sprintf(IEXP_szText,"%d"/*DNT*/,fi1);
				m_CdetailView->m_ListCtl[IEXP_BNTAB_IDX].SetItemText(Cnt,IEXP_EntityCol[IEXP_BNTAB_IDX].Order[1][IEXP_COL_ORDER],IEXP_szText);
			}
			break;

		case IEXP_CSTAB_IDX: // Insert UCS.
			SDS_getvar(NULL,DB_QUCSORG,&rborg,IEXP_ODNode->m_dbDrawing,NULL,NULL);
			SDS_getvar(NULL,DB_QUCSXDIR,&rbxdir,IEXP_ODNode->m_dbDrawing,NULL,NULL);
			SDS_getvar(NULL,DB_QUCSYDIR,&rbydir,IEXP_ODNode->m_dbDrawing,NULL,NULL);
			strcpy(IEXP_szText,ResourceString(IDC_IEXP_NEW_UCS, "NewUCS")); sindex=0;
			do {
				NameExist=FALSE;
				sprintf(IEXP_szText,"%s%d"/*DNT*/,ResourceString(IDC_IEXP_NEW_UCS, "NewUCS"),(++sindex));
				if ((rbb=sds_buildlist(RTDXF0,IEXP_UCS,2,IEXP_szText,70,0,10,rborg.resval.rpoint,11,rbxdir.resval.rpoint,12,rbydir.resval.rpoint,0))==NULL) return;
				if (SDS_tblmesser(rbb,IC_TBLMESSER_MAKE,IEXP_ODNode->m_dbDrawing)!=RTNORM) NameExist=TRUE;
				sds_relrb(rbb);
			} while(NameExist);
			rbb=SDS_tblgetter(IEXP_EntityCol[IEXP_CurrentIndx].cType,IEXP_szText,TRUE,IEXP_ODNode->m_dbDrawing);
			Cnt=IEXP_UCSCnt;
			// Insert a new row for the UCS in the (CListCtrl).
			strcpy(IEXP_szTextA,IEXP_szText);
			if (!m_CdetailView->UcsIns(Cnt,0,IEXP_CurrentIndx,NULL,lvI,FALSE,rbb)) return;
			sds_relrb(rbb);

			rborg.restype=RTSTR;
			rborg.resval.rstring=IEXP_szTextA;
			SDS_setvar(NULL,DB_QUCSNAME,&rborg,IEXP_ODNode->m_dbDrawing,&SDS_CURCFG,&SDS_CURSES,0);
			break;
	}

	switch(m_CdetailView->m_MessageType)
	{
		case IEXP_BNTAB_IDX:
			if (!m_CdetailView->m_BlockInsCreate)
			{
				m_CdetailView->m_ListCtl[IEXP_CurrentIndx].SetFocus();
				m_CdetailView->m_ListCtl[IEXP_CurrentIndx].EditLabel(Cnt);
			}
			break;

		case IEXP_VWTAB_IDX:
			IEXP_Findinfo.psz	=IEXP_EntityCol[IEXP_CurrentIndx].CurrentName;
			Index=m_CdetailView->m_ListCtl[IEXP_CurrentIndx].FindItem(&IEXP_Findinfo);
			// Take this out becouse lindy can't understand view's.
/*
			// Set the previous items image to (NULL).
			m_CdetailView->m_ListCtl[IEXP_CurrentIndx].SetItem(Index,0,LVIF_IMAGE,NULL,-1,0,0,0);
			// Send an update to the previous items image.
			if (!m_CdetailView->m_ListCtl[IEXP_CurrentIndx].Update(Index)) return;
*/
			// Edit the label
			m_CdetailView->m_ListCtl[IEXP_CurrentIndx].SetFocus();
			m_CdetailView->m_ListCtl[IEXP_CurrentIndx].EditLabel(Cnt);
			break;

		case IEXP_CSTAB_IDX:
			m_CdetailView->m_ListCtl[IEXP_CurrentIndx].SetFocus();
			m_CdetailView->m_ListCtl[IEXP_CurrentIndx].EditLabel(Cnt);

			IEXP_Findinfo.psz	=IEXP_EntityCol[IEXP_CurrentIndx].CurrentName;
			Index=m_CdetailView->m_ListCtl[IEXP_CurrentIndx].FindItem(&IEXP_Findinfo);
			// Set the previous items image to (NULL).
			m_CdetailView->m_ListCtl[IEXP_CurrentIndx].SetItem(Index,0,LVIF_IMAGE,NULL,-1,0,0,0);
			// Send an update to the previous items image.
			if (!m_CdetailView->m_ListCtl[IEXP_CurrentIndx].Update(Index)) return;
			String=m_CdetailView->m_ListCtl[IEXP_CurrentIndx].GetItemText(m_CdetailView->m_ItemIndex,0);
			strcpy(IEXP_EntityCol[IEXP_CurrentIndx].CurrentName,(char *)((LPCTSTR)String));
			// Set the current items image to the (CHECK) mark.
			if (!m_CdetailView->m_ListCtl[IEXP_CurrentIndx].SetItem(m_CdetailView->m_ItemIndex,0,LVIF_IMAGE,NULL,0,0,0,0)) return;
			break;
	}

	if (m_CdetailView->m_Purge)
	{
		//Since we have purged the drawing, we need to recreate the list controls
		m_CdetailView->UpdateDataList();
		m_CdetailView->m_ListCtl[IEXP_CurrentIndx].EnableWindow(TRUE);
		m_CdetailView->m_ListCtl[IEXP_CurrentIndx].ShowWindow(SW_SHOW);
	}

	m_CdetailView->m_Purge = FALSE;
	m_CdetailView->m_MessageType=0;
}


// ============================================================================
// ================== CExplorerPropsTab Support Functions =====================
// ============================================================================
//

/******************************************************************************
*+ CExplorerPropsTab::GetTextStyleVariesInfo - Sees if the Text Styles Properties values vary.
*
*  In the main IntelliCAD Explorer window, multiple text styles can be selected
*  when displaying properties.	It is possible that some values will vary
*  between the styles, so this needs to be indicated in the Properties tab.
*  This function determines whether or not any values vary.
*
*  NOTE:  Because of the global variables used by the original IntelliCAD Explorer
*		  code and the need for this function to access those variables, this
*		  function is in this module rather than in explorerpropstab.cpp.
*
*		  This function contains functionality removed from
*		  IEXP_SubDlg::TableInfoConstruct() and has been modified appropriately
*		  to support the CExplorerPropsTab class except for the IntelliCAD
*		  Explorer's global variables.
*/

bool CExplorerPropsTab::GetTextStylesVariesInfo(void)	// Sees if Text Styles Properties values vary.
{
	CString				styleName;
	db_styletabrec		*firstStyleRec, *curStyleRec;
	int					styleIndex;
	UINT				numSelectedStyles, loopIndex;

	// should be handling text styles, nothing else
	ASSERT(IEXP_TSTAB_IDX == IEXP_EntityCol[IEXP_CurrentIndx].IType);
	if (IEXP_TSTAB_IDX != IEXP_EntityCol[IEXP_CurrentIndx].IType)
		return false;

	// get the number of styles selected in the main dialog
	numSelectedStyles = m_CdetailView->m_ListCtl[IEXP_CurrentIndx].GetSelectedCount();
	m_Vindex = numSelectedStyles;				// may need count later in dialog

	if (0 == numSelectedStyles)
		return false;

	// Loop through all of the selected styles, keeping the values of the first style
	// and comparing the subsequent styles to them.  If any values vary, set the
	// appropriate flags so that we can display the varies information in the Properties
	// tab.
	for(styleIndex = -1, loopIndex = 0; loopIndex < numSelectedStyles; ++loopIndex)
		{
		// get the index of the first style in the main IntelliCAD Explorer list
		styleIndex = m_CdetailView->m_ListCtl[IEXP_CurrentIndx].GetNextItem(styleIndex, LVNI_SELECTED);	// get the first style
		if (-1 == styleIndex)					// didn't find any styles
			return false;

		// get and compare the values from the first and subsequent styles
		styleName = m_CdetailView->m_ListCtl[IEXP_CurrentIndx].GetItemText(styleIndex, 0);
		if (0 == loopIndex)						// first time through loop, save first style
			{
			firstStyleRec = (db_styletabrec*) IEXP_ODNode->m_dbDrawing->findtabrec(DB_STYLETAB,
																				   styleName,
																				   1);

			if (NULL == firstStyleRec)
				return false;

			GetTextStyleValues(firstStyleRec);
			continue;
			}

		curStyleRec = (db_styletabrec*) IEXP_ODNode->m_dbDrawing->findtabrec(DB_STYLETAB,
																			 styleName,
																			 1);

		if (NULL == curStyleRec)
			return false;

		CompareTextStyleValues(curStyleRec);
		}

	return true;
}

///////////////////////////////////////////////////////////////////////
// This function is called when the subdialog of an edit dialog is
// destroyed, the sub dialogs contains all the child items such as edit
// boxes,buttons,etc. In this function there is a case statment for each
// major (ITEM) type, such as (Layer,Linetypes,Text styles,etc.) in each
// case for each item in the subdialog we get the items text or state
// value, we then compare it to the original value that was saved in the
// (OnInitDialog) function, if the value has changed the it is add to
// the resbuf link list to be used to update the entity, if it has not
// changed then it is not added. at the end we call tablemesser to update
// the entity.
//
// Nothing is returned.
//
/******************************************************************************
*+ CExplorerPropsTab::RestoreTextStyleValues - Restores the Text Styles Properties values.
*
*  This function restores the values from the Text Styles Properties tab to the
*  list in the main IntelliCAD Explorer view.  At the same time, it updates the
*  table record for each style selected in the main IntelliCAD Explorer view.
*
*  If something goes wrong while building the resbuf list for modifying a
*  table record, we modify any values we already have in the list and then go
*  on to the next style.
*
*  NOTE:  Because of the global variables used by the original IntelliCAD Explorer
*		  code and the need for this function to access those variables, this
*		  function is in this module rather than in explorerpropstab.cpp.
*
*		  This function contains functionality removed from
*		  IEXP_SubDlg::SavePropertyValues() and has been modified appropriately
*		  to support the CExplorerPropsTab class except for the IntelliCAD Explorer's
*		  global variables.
*
*		  to do - This function should be broken down into two functions:  one
*		  that resets the values in the main IntelliCAD Explorer dialog, and one
*		  that modifies the table.	When this is done, the code that modifies
*		  the table should be able to be shared with the code that modifies the
*		  table in CTextStyleDlg::SaveTextStyle().
*
*/

void CExplorerPropsTab::RestoreTextStyleValues(void)
	{
	int				toggleVal;
	long			styleNumber;
	char			tmpBuf[IC_ACADBUF],
					fontFilename[IC_ACADBUF],		// to do - ensure this is the proper length - probably max of 30 or 31
					fontGlobalName[IC_ACADBUF];		// to do - ensure this is the proper length - probably max of 30 or 31
	resbuf			*rb,
					*flagrb = NULL,
					*tmpModTableInfo = NULL,
					*modTableInfo = NULL,
					*curTableInfo = NULL,
					*tmpModEEDInfo = NULL,
					*modEEDInfo = NULL;
	short			styleIndex,
					loopIndex,
					numSelectedStyles;

	// should be text styles tab
	ASSERT(IEXP_TSTAB_IDX == IEXP_EntityCol[m_oldindex].IType);
	if (IEXP_TSTAB_IDX != IEXP_EntityCol[m_oldindex].IType)
		return;

	if (!m_dataptr)								// something went wrong with the data
		return;

	// update each selected style
	numSelectedStyles = m_Vindex;
	for(styleIndex = -1, loopIndex = 0; loopIndex < numSelectedStyles; ++loopIndex)
		{
		// get the index of the selected style
		styleIndex = m_CdetailView->m_ListCtl[IEXP_CurrentIndx].GetNextItem(styleIndex, LVNI_SELECTED);
		if (styleIndex == -1)
			goto modifyTable;

		// start the resbuf list for updating the table record
		IEXP_ItemDefPtr = (IEXP_ItemDef *) m_CdetailView->m_ListCtl[IEXP_CurrentIndx].GetItemData(styleIndex);
		ASSERT(IEXP_ItemDefPtr);

		if (!(tmpModTableInfo = BuildDataList(&modTableInfo, RTDXF0, IEXP_STYLE)) ||
			!BuildDataList(&tmpModTableInfo, 2, IEXP_ItemDefPtr->name()))
				goto modifyTable;

		// update the values that have changed, both in the main IntelliCAD Explorer view
		// and in the table record
		if (m_newHeightStr != m_heightStr)
			{
			m_CdetailView->m_ListCtl[IEXP_CurrentIndx].SetItemText(styleIndex,
				IEXP_EntityCol[m_oldindex].Order[IEXP_TS_HEIGHT][IEXP_COL_ORDER], m_newHeightStr);
			if (!BuildDataList(&tmpModTableInfo, 40, atof(m_newHeightStr)))
				goto modifyTable;
			}

		if (m_newWidthStr != m_widthStr)
			{
			m_CdetailView->m_ListCtl[IEXP_CurrentIndx].SetItemText(styleIndex,
				IEXP_EntityCol[m_oldindex].Order[IEXP_TS_WIDTH][IEXP_COL_ORDER], m_newWidthStr);
			if (!BuildDataList(&tmpModTableInfo, 41, atof(m_newWidthStr)))
				goto modifyTable;
			}

		if (m_newObliqueAngleStr != m_obliqueAngleStr)
			{
			m_CdetailView->m_ListCtl[IEXP_CurrentIndx].SetItemText(styleIndex,
				IEXP_EntityCol[m_oldindex].Order[IEXP_TS_SLANT][IEXP_COL_ORDER], m_newObliqueAngleStr);
			if (!BuildDataList(&tmpModTableInfo, 50, (IC_PI/180) * atof(m_newObliqueAngleStr)))
				goto modifyTable;
			}

// to do - both the font name and font style need to be handled better - when we had a TTF and now don't or vice versa or we just have a different font of the same type
//		   also have to handle the *varies* situation
//		   also have to get the TTF filename if we have a TTF to save in DXF 3;
//		   what we'd have in m_newFontName would be the family name that goes in EED
// to do - somewhere along the line, the font attached to the style has to be updated with the proper info
// EBATECH(CNBR)
		if ((m_newFontName != "") && ((m_newHasTrueTypeFont && (m_newFontName != m_fontFamily)) ||
			(!m_newHasTrueTypeFont && (m_newFontName != m_fontFilename))))
			{
			m_CdetailView->m_ListCtl[IEXP_CurrentIndx].SetItemText(styleIndex,
				IEXP_EntityCol[m_oldindex].Order[IEXP_TS_FONT][IEXP_COL_ORDER],
				m_newFontName);

			// update the font filename - do BEFORE handling EED
			if (m_newHasTrueTypeFont)
				GetTextStyleTableInfo(fontFilename, fontGlobalName, &styleNumber);
			else
				strcpy(fontFilename, m_newFontName);

			if (!BuildDataList(&tmpModTableInfo, 3, fontFilename))
				goto modifyTable;

			// start the resbuf list for the EED
			// ALWAYS handle, whether for TTF or SHX/SHP
			if (NULL == modEEDInfo)
				{
				if (!(tmpModEEDInfo = BuildDataList(&modEEDInfo, -3, NULL)) ||
					!BuildDataList(&tmpModEEDInfo, 1001, "ACAD"/*DNT*/))
					goto modifyTable;

				// if the new font is not a TrueType font, the EED specification is done -
				// the current ACAD EED will be deleted; if the new font is a TrueType font,
				// we'll update all of the information - family name, weight, italic, and
				// pitch and family
				if (m_newHasTrueTypeFont)
					{
					if (!BuildDataList(&tmpModEEDInfo, 1000, fontGlobalName))
						goto modifyTable;

					if (!BuildDataList(&tmpModEEDInfo, 1071, styleNumber))
						goto modifyTable;
					}
				}
			}

		if (m_newHasTrueTypeFont && (m_newFontStyle != m_fontStyle))
			{
			CString cstext;
			m_fontStyleCtrl.GetWindowText(cstext);
			m_CdetailView->m_ListCtl[IEXP_CurrentIndx].SetItemText(styleIndex,
				IEXP_EntityCol[m_oldindex].Order[IEXP_TS_STYLE][IEXP_COL_ORDER],
				cstext);	// to do - get the real style name or set by the style number//EBATECH(CNBR) It's OK.
			// EBATECH(CNBR) -[
			m_fontLangCtrl.GetWindowText(cstext);
			m_CdetailView->m_ListCtl[IEXP_CurrentIndx].SetItemText(styleIndex,
				IEXP_EntityCol[m_oldindex].Order[IEXP_TS_LANG][IEXP_COL_ORDER],
				cstext);

			// if the EED information was handled by the font name, it's all done;
			// otherwise, we need to handle it here if the style changed
			if (NULL == modEEDInfo)
				{
				GetTextStyleTableInfo(fontFilename, fontGlobalName, &styleNumber);

				if (!BuildDataList(&tmpModTableInfo, 3, fontFilename))
					goto modifyTable;

				if (!(tmpModEEDInfo = BuildDataList(&modEEDInfo, -3, NULL)) ||
					!BuildDataList(&tmpModEEDInfo, 1001, "ACAD"/*DNT*/))
					goto modifyTable;

				if (!BuildDataList(&tmpModEEDInfo, 1000, fontGlobalName))
					goto modifyTable;

				if (!BuildDataList(&tmpModEEDInfo, 1071, styleNumber))
					goto modifyTable;
				}
			}
		if (!m_newHasTrueTypeFont && !m_hasTrueTypeFontVaries){
			m_CdetailView->m_ListCtl[IEXP_CurrentIndx].SetItemText(styleIndex,
				IEXP_EntityCol[m_oldindex].Order[IEXP_TS_STYLE][IEXP_COL_ORDER],
				"");
			m_CdetailView->m_ListCtl[IEXP_CurrentIndx].SetItemText(styleIndex,
				IEXP_EntityCol[m_oldindex].Order[IEXP_TS_LANG][IEXP_COL_ORDER],
				"");
		}
		// EBATECH(CNBR) -[
		if (m_newBigfontName != m_bigfontFilename)
			{
			if( !m_newHasTrueTypeFont )
				{
				m_CdetailView->m_ListCtl[IEXP_CurrentIndx].SetItemText(styleIndex,
					IEXP_EntityCol[m_oldindex].Order[IEXP_TS_STYLE][IEXP_COL_ORDER], "");
				m_CdetailView->m_ListCtl[IEXP_CurrentIndx].SetItemText(styleIndex,
					IEXP_EntityCol[m_oldindex].Order[IEXP_TS_LANG][IEXP_COL_ORDER], "");
				}
			m_CdetailView->m_ListCtl[IEXP_CurrentIndx].SetItemText(styleIndex,
				IEXP_EntityCol[m_oldindex].Order[IEXP_TS_BIGFONT][IEXP_COL_ORDER], m_newBigfontName);
			if (!BuildDataList(&tmpModTableInfo, 4, m_newBigfontName))
				goto modifyTable;
			}
		// EBATECH(CNBR) ]-

		// need the current style's table info for properly setting the toggles
		curTableInfo = SDS_tblgetter(IEXP_EntityCol[IEXP_CurrentIndx].cType, IEXP_ItemDefPtr->name(), TRUE, IEXP_ODNode->m_dbDrawing);
		ASSERT(curTableInfo);

		rb = ic_ret_assoc(curTableInfo, 71);
		if ((m_newBackwards != m_backwards) && (m_newBackwards != IEXP_VARIES_I))
			{
			sprintf(tmpBuf, "%s"/*DNT*/,
				m_newBackwards ? ResourceString(IDC_ICADEXPLORER_YES_13, "Yes") :
								 ResourceString(IDC_ICADEXPLORER_NO_14, "No"));

			m_CdetailView->m_ListCtl[IEXP_CurrentIndx].SetItemText(styleIndex,
				IEXP_EntityCol[m_oldindex].Order[IEXP_TS_BACKWORD][IEXP_COL_ORDER], tmpBuf);

			toggleVal = m_newBackwards ? rb->resval.rint | 0x02 : rb->resval.rint & 0xFD;
			if (!(flagrb = BuildDataList(&tmpModTableInfo, 71, toggleVal)))
				goto modifyTable;
			}
		else
			flagrb = NULL;

		if ((m_newUpsideDown != m_upsideDown) && (m_newUpsideDown != IEXP_VARIES_I))
			{
			sprintf(tmpBuf, "%s"/*DNT*/,
				m_newUpsideDown ? ResourceString(IDC_ICADEXPLORER_YES_13, "Yes") :
								  ResourceString(IDC_ICADEXPLORER_NO_14, "No"));

			m_CdetailView->m_ListCtl[IEXP_CurrentIndx].SetItemText(styleIndex,
				IEXP_EntityCol[m_oldindex].Order[IEXP_TS_UPSIDEDOWN][IEXP_COL_ORDER], tmpBuf);

			if (flagrb)
				{
				flagrb->resval.rint = m_newUpsideDown ? flagrb->resval.rint | 0x04 :
														flagrb->resval.rint ^ 0x04;
				}
			else
				{
				toggleVal = m_newUpsideDown ? rb->resval.rint | 0x04 : rb->resval.rint ^ 0xFD;
				if (!BuildDataList(&tmpModTableInfo, 71, toggleVal))
					goto modifyTable;
				}
			}

		rb = ic_ret_assoc(curTableInfo, 70);
		if ((m_newVertically != m_vertically) && (m_newVertically != IEXP_VARIES_I))
			{
			sprintf(tmpBuf, "%s"/*DNT*/,
				m_newVertically ? ResourceString(IDC_ICADEXPLORER_YES_13, "Yes") :
								  ResourceString(IDC_ICADEXPLORER_NO_14, "No"));

			m_CdetailView->m_ListCtl[IEXP_CurrentIndx].SetItemText(styleIndex,
				IEXP_EntityCol[m_oldindex].Order[IEXP_TS_VERTICAL][IEXP_COL_ORDER], tmpBuf);

			toggleVal = m_newVertically ? rb->resval.rint | 0x04 : rb->resval.rint ^ 0x04;
			if (!BuildDataList(&tmpModTableInfo, 70, toggleVal))
				goto modifyTable;
			}


modifyTable:

		if (IEXP_RegenOnUndo == FALSE)
			{
			SDS_SetUndo(IC_UNDO_REGEN_ON_UNDO, NULL, NULL, NULL, SDS_CURDWG);
			IEXP_RegenOnUndo = TRUE;
			}

		m_CdetailView->m_regenstate = TRUE;
		if (modTableInfo)
			{
			// add on the EED if this is a TrueType font
			if (modEEDInfo)
				appendRbLists(&modTableInfo, &modEEDInfo);

			SDS_tblmesser(modTableInfo, IC_TBLMESSER_MODIFY, IEXP_ODNode->m_dbDrawing);
			IC_RELRB(modTableInfo);
			}

		if (curTableInfo)
			IC_RELRB(curTableInfo);
		}	// end for

		if(m_CdetailView->m_regenstate && m_CdetailView->m_Explorer->m_regenMode)
		{
			cmd_regenall();
			m_CdetailView->m_regenstate = FALSE;
		}
	}



///////////////////////////////////////////////////////////////////////
// This function is called when the new button on the toolbar is
// selected.
//
// Nothing is returned.
//
void IEXP_Explorer::OnNewItem() 		   { m_CdetailView->InsertRow(EXP_NEW_T); }
void IEXP_Explorer::OnCutItem() 		   { m_CdetailView->m_selflag=TRUE; m_CdetailView->CopyCutItem(TRUE); }
void IEXP_Explorer::OnCopyItem()		   { m_CdetailView->m_selflag=TRUE; m_CdetailView->CopyCutItem(FALSE); }
void IEXP_Explorer::OnPasteItem()		   { m_CdetailView->PasteItem(); }
void IEXP_Explorer::OnHelp()			   { m_CdetailView->Onhelp(); }
void IEXP_Explorer::OnHelpInfo(HELPINFO* lpHelpInfo)		   { m_CdetailView->Onhelp(); }
void IEXP_Explorer::OnBlkListType(UINT Id) { m_CdetailView->BlkListType(Id); }
void IEXP_Explorer::OnCurrent() 		   { m_CdetailView->CurrentItem(); }
void IEXP_Explorer::OnRemoveItem()		   { m_CdetailView->RemoveItem(); }
void IEXP_Explorer::OnRenameItem()		   { m_CdetailView->RenameItem(); }
void IEXP_Explorer::OnSelectAll()		   { m_CdetailView->SelectAll(); }
void IEXP_Explorer::PropertyDlg()		   { m_CdetailView->PropertyDlg(); }
void IEXP_Explorer::OnInsertRow(UINT Id)   { m_CdetailView->InsertRow(Id); }
void IEXP_Explorer::OnLayerAtt(UINT Id)    { m_CdetailView->LayerAtt(Id); }
void IEXP_Explorer::OnWorld() {
	cmd_ucsworld();
	{
		struct gr_view *view=SDS_CURGRVW;
		int 			Index;

		IEXP_Findinfo.psz	=IEXP_EntityCol[IEXP_CurrentIndx].CurrentName;
		Index=m_CdetailView->m_ListCtl[IEXP_CurrentIndx].FindItem(&IEXP_Findinfo);
		// Set the previous items image to (NULL).
		m_CdetailView->m_ListCtl[IEXP_CurrentIndx].SetItem(Index,0,LVIF_IMAGE,NULL,-1,0,0,0);
		// Send an update to the previous items image.
		if (!m_CdetailView->m_ListCtl[IEXP_CurrentIndx].Update(Index)) return;

		gr_initview(IEXP_ODNode->m_dbDrawing,&SDS_CURCFG,&view,NULL);
		sds_redraw(NULL,IC_REDRAW_DONTCARE);
	}
}

void IEXP_Explorer::OnPurge()
{
	HTREEITEM			  hTItem;
	*IEXP_szText=0;
	CString str;

	if (m_MainWindow->m_ExpReturnHwnd) {
		AfxMessageBox(ResourceString(IDC_ICADEXPLORER_CAN_T_DO_T_33, "This operation cannot be performed.  Please run Explorer directly from the Tools menu to perform this operation."));
		return;
	}

	if (m_editmode)
		m_CdetailView->m_ListCtl[IEXP_CurrentIndx].EndLabelEditing();
	// Get the selected tree view item handle.
	hTItem=m_CItemList->m_TreeCtl.GetSelectedItem();
	if (hTItem)
	{
		str = m_CItemList->m_TreeCtl.GetItemText(hTItem);

		if		(str == ResourceString(IDC_IEXP_LAYERS, "Layers"))					strcpy(IEXP_szText, "_purge;_Layers");
		else if (str == ResourceString(IDC_IEXP_LINETYPES, "Linetypes"))			strcpy(IEXP_szText, "_purge;_LTypes");
		else if (str == ResourceString(IDC_IEXP_STYLES, "Text Styles"))					strcpy(IEXP_szText, "_purge;_STyles");
		else if (str == ResourceString(IDC_IEXP_COORDSYSTEM, "Coordinate Systems"))	; //cannot be purged
		else if (str == ResourceString(IDC_IEXP_VIEWS, "Views"))					; //cannot be purged
		else if (str == ResourceString(IDC_IEXP_BLOCKS, "Blocks"))					strcpy(IEXP_szText, "_purge;_Blocks");
		else if (str == ResourceString(IDC_IEXP_DIMSTYS, "Dimension Styles"))		strcpy(IEXP_szText, "_purge;_Dimstyles");
		else																		strcpy(IEXP_szText, "_purge;_All");
	}
	else
	{
		//nothing in the tree view is selected, but something in the list view is.
		switch (IEXP_EntityCol[IEXP_CurrentIndx].IType)
		{
			case IEXP_LATAB_IDX:
				strcpy(IEXP_szText, "_purge;_Layers");
				break;
			case IEXP_LTTAB_IDX:
				strcpy(IEXP_szText, "_purge;_LTypes");
				break;
			case IEXP_TSTAB_IDX:
				strcpy(IEXP_szText, "_purge;_STyles");
				break;
			case IEXP_BNTAB_IDX:
				strcpy(IEXP_szText, "_purge;_Blocks");
				break;
			case IEXP_DSTAB_IDX:
				strcpy(IEXP_szText, "_purge;_Dimstyles");
				break;
			case IEXP_CSTAB_IDX:
			case IEXP_VWTAB_IDX:
				break;
			default:
				strcpy(IEXP_szText, "_purge;_All");
				break;
		}
	}

	if (*IEXP_szText)
	{
		m_MainWindow->m_bExplorerState=FALSE;
		ShowWindow(SW_HIDE);
		if(m_regenMode == 0)
			cmd_regenall();
		m_MainWindow->EndModalState();
		//purge_drawing (IEXP_ODNode->m_dbDrawing, IEXP_szText);
		SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)IEXP_szText);
		m_MainWindow->m_bExpInsertState=TRUE;
		m_CdetailView->m_Purge=TRUE;
	}

	return;
}

// ======================================================================================================================================================================
// ====================================================== CMAINWINDOW CLASS FUNCTION ====================================================================================
// ======================================================================================================================================================================

///////////////////////////////////////////////////////////////////////
// This function is main entry for the explorer, this gets some
// information from a getvar vars to set the saved
// "Positions,width,etc." once that is done the explorer is created,
// and displayed
//
// Nothing is returned.
//
void CMainWindow::IcadExplorer(short index) {
	struct resbuf  rbb;
	short		   fi1,
				   Cnt;
	char		  *ptr1,
				  *ptr2;
	long		   lMaxStyle=0;
	CIcadApp	  *icadapp;

	m_pExplorer = new IEXP_Explorer();

	IEXP_hParent=this->m_hWnd;

	GetWindowRect(m_pExplorer->m_rectScrn);

	rbb.restype=RTSTR;
	for(fi1=0,rbb.resval.rstring=NULL;fi1<(IEXP_NumofTypes-1);++fi1) {
		sds_getvar(IEXP_EntityCol[fi1].resname,&rbb);
		if (!(*rbb.resval.rstring)) break;
		for(ptr2=ptr1=rbb.resval.rstring,Cnt=0;Cnt<IEXP_EntityCol[fi1].Cnt;++Cnt,ptr1=ptr2+1) {
			if ((ptr2=strchr(ptr1,IEXP_COMMA))==NULL) break;;
			*ptr2=0;
			IEXP_EntityCol[fi1].Order[Cnt][IEXP_COLUMNWIDTH]=atoi(ptr1);
		}
		IEXP_EntityCol[fi1].Order[Cnt][IEXP_COLUMNWIDTH]=atoi(ptr1);
		if (rbb.resval.rstring) { delete [] rbb.resval.rstring; rbb.resval.rstring=NULL; }
	}

	sds_getvar("WNDPTEXP"/*DNT*/,&rbb);
	if (rbb.resval.rpoint[0] || rbb.resval.rpoint[1]) {
		m_pExplorer->m_rectScrn.left  =(long)rbb.resval.rpoint[0];
		m_pExplorer->m_rectScrn.top   =(long)rbb.resval.rpoint[1];
		sds_getvar(ResourceString(IDC_ICADEXPLORER_WNDSTEXP_161, "WNDSTEXP" ),&rbb);
		m_pExplorer->m_rectScrn.right =(long)rbb.resval.rpoint[0];
		m_pExplorer->m_rectScrn.bottom=(long)rbb.resval.rpoint[1];
	} else {
		m_pExplorer->m_rectScrn.top   +=(m_pExplorer->m_rectScrn.bottom-m_pExplorer->m_rectScrn.top)/8;
		m_pExplorer->m_rectScrn.bottom-=(m_pExplorer->m_rectScrn.bottom-m_pExplorer->m_rectScrn.top)/8;
		m_pExplorer->m_rectScrn.left  +=(m_pExplorer->m_rectScrn.right-m_pExplorer->m_rectScrn.left)/8;
		m_pExplorer->m_rectScrn.right -=(m_pExplorer->m_rectScrn.right-m_pExplorer->m_rectScrn.left)/8;
	}
	sds_getvar("WNDLTEXP"/*DNT*/,&rbb);
	if (rbb.resval.rint==2) lMaxStyle=WS_MAXIMIZE;

	BeginModalState();
	m_pExplorer->m_MainWindow=this;
	m_pExplorer->m_initialwin=index;
	if (!(m_pExplorer->Create(NULL,ResourceString(IDC_ICADEXPLORER_TABLE_EXP_174, "Table Explorer" ),lMaxStyle|WS_OVERLAPPED|WS_CAPTION|WS_THICKFRAME|WS_SYSMENU|WS_MAXIMIZEBOX|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,m_pExplorer->m_rectScrn,this,MAKEINTRESOURCE(EXP_EXPLORER_MNU)))) return;
	m_pExplorer->ShowWindow(SW_SHOW);
	m_pExplorer->UpdateWindow();
	m_bExplorerState=TRUE;
	m_bExpInsertState=FALSE;
	icadapp=(CIcadApp *)AfxGetApp();
	IEXP_Clipboard=icadapp->m_uIcadClipFormat;
}

// ======================================================================================================================================================================
// ========================================================== GLOBAL FUNCTIONS ==========================================================================================
// ======================================================================================================================================================================

void IEXP_FreeOpenDoc(void) {
	struct IEXP_OpenDocs *Tnode;

	for(IEXP_ODNode=IEXP_ODFirstNode;IEXP_ODNode;IEXP_ODNode=Tnode) {
		Tnode=IEXP_ODNode->next;
		delete IEXP_ODNode->szDrawing;
		delete IEXP_ODNode;
	}
	IEXP_ODNode=IEXP_ODFirstNode=NULL;
}

void IEXP_AddNewOpenDoc(void) {
	struct IEXP_OpenDocs *Tnode;

	Tnode=(struct IEXP_OpenDocs *)new struct IEXP_OpenDocs;
	memset(Tnode,0,sizeof(struct IEXP_OpenDocs));
	if (IEXP_ODFirstNode) {
		for(IEXP_ODNode=IEXP_ODFirstNode;IEXP_ODNode && IEXP_ODNode->next;IEXP_ODNode=IEXP_ODNode->next);
		IEXP_ODNode->next=Tnode;
		IEXP_ODNode=IEXP_ODNode->next;
	} else {
		IEXP_ODFirstNode=IEXP_ODNode=Tnode;
	}
}

///////////////////////////////////////////////////////////////////////
// This function converts a string containing comma dilimited real
// values to a point type.
//
// TRUE  : on success.
// FALSE : on error.
//
BOOL IEXP_strtopointr(char *string,double point[3]) {
	char *ptr1,
		 *ptr2;

	if (!string || !*string) return(FALSE);

	ptr1=ptr2=string;
	if ((ptr2=strchr(ptr1,IEXP_COMMA))==NULL) return(FALSE);
	*ptr2=0;
	point[0]=atof(ptr1);
	ptr1=ptr2+1;
	if ((ptr2=strchr(ptr1,IEXP_COMMA))==NULL) return(FALSE);
	*ptr2=0;
	point[1]=atof(ptr1);
	ptr1=ptr2+1;
	point[2]=atof(ptr1);

	return(TRUE);
}

long
GetFontStyleNumber(
	IEXP_CListCtrl listControl[],
	short listIndex,
	int styleIndex,
	const char *styleName,
	const char *langName)	// EBATECH(CNBR) Add langName
{
	ASSERT(listControl && styleName && langName);
	long styleNumber = 34L;					// default for regular Arial, our default used elsewhere
	NowEnumulating = true;
	if (listControl && styleName && langName ){
		CInPlaceFontStyleCombo *styleCombo;
		styleCombo = listControl[listIndex].ShowInPlaceFontStyleCombo( styleIndex, IEXP_TS_STYLE );
		CInPlaceFontLangCombo *langCombo;
		langCombo = listControl[listIndex].ShowInPlaceFontLangCombo( styleIndex, IEXP_TS_LANG );

		if (styleCombo){
			int styleIndex = styleCombo->FindStringExact(-1, styleName);
			if (CB_ERR != styleIndex){
				StyleItem* styleItem;
				styleItem = (StyleItem *) styleCombo->GetItemDataPtr(styleIndex);
				if (styleItem && (-1 != (int) styleItem)){
					int charSet = styleItem->charSet;
					if(langCombo){
						int langIndex = langCombo->FindStringExact(-1, langName);
						if (CB_ERR != langIndex){
							charSet = langCombo->GetItemData(langIndex);
						}
					}
					styleNumber = CTTFInfoGetter::styleNumByFontParams(styleItem->weight, styleItem->italic, styleItem->pitchAndFamily, charSet);
				}
			}
		}
		if(styleCombo){	delete styleCombo; }
		if(langCombo){ delete langCombo; }
	}
	NowEnumulating = false;
	return styleNumber;
}

/*--------------------------------------------------------------------------*/

// Modified CyberAge AP 09/04/2000[
// Reason: To fix slow explorer problem.
BOOL	IEXP_CdetailView::UpdateDataList1(short SetFlag){

	switch(IEXP_EntityCol[IEXP_CurrentIndx].IType)
	{
		case IEXP_LATAB_IDX:
			InsertDataList(IEXP_LATAB_IDX,SetFlag);
			break;
		case IEXP_LTTAB_IDX:
			InsertDataList(IEXP_LTTAB_IDX,SetFlag);
			break;
		case IEXP_TSTAB_IDX:
			InsertDataList(IEXP_TSTAB_IDX,SetFlag);
			break;
		case IEXP_CSTAB_IDX:
			InsertDataList(IEXP_CSTAB_IDX,SetFlag);
			break;
		case IEXP_VWTAB_IDX:
			InsertDataList(IEXP_VWTAB_IDX,SetFlag);
			break;
		case IEXP_BNTAB_IDX:
			break;
		case IEXP_DSTAB_IDX:
			InsertDataList(IEXP_DSTAB_IDX,SetFlag);
			break;
	}
	return(TRUE);
}
// Modified CyberAge AP 09/04/2000]

/*-------------------------------------------------------------------------*/

// Modified CyberAge AP 09/22/2000[
// Reason: To fix slow explorer problem.
void IEXP_CdetailView::InsertDataList(int type, short SetFlag) {

	int iItemCount = m_ListCtl[type].GetItemCount();
	IEXP_ItemDef *itemptr;
	for(int i=0;i<iItemCount;i++)
	{
		itemptr=(IEXP_ItemDef *) m_ListCtl[type].GetItemData(i);
		if(SetFlag)
			itemptr->m_rowPtr=SDS_tblgetter(IEXP_EntityCol[type].cType,itemptr->name(),TRUE,IEXP_ODNode->m_dbDrawing);
		else
		{
			if(itemptr->m_rowPtr)
				sds_relrb(itemptr->m_rowPtr);
		}
		m_ListCtl[type].SetItemData( i, (long)itemptr );
	}
}
// Modified CyberAge AP 09/22/2000]
//Added Cybage AJK 10/12/2001 [
//Reason : Fix for bug no 77922
//Implementation for xref Visibility option of explorer
void IEXP_CdetailView::ShowXrefLayers()
{
	if(m_ListCtl[IEXP_CurrentIndx].GetItemCount() >= 1 &&
		m_ListCtl[IEXP_CurrentIndx].GetSelectedCount() < 1 )
	{
		m_ListCtl[IEXP_CurrentIndx].SetFocus();
		m_ListCtl[IEXP_CurrentIndx].SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
	}
	struct resbuf *tmprb;
	tmprb=SDS_tblgetter(IEXP_EntityCol[IEXP_CurrentIndx].cType,NULL,TRUE,IEXP_ODNode->m_dbDrawing);
	do{

		if(ic_assoc(tmprb,2)==0)
		{

			if(strchr(ic_rbassoc->resval.rstring,IEXP_VER_BAR))
			{
				LV_ITEM 	   lvI;
				lvI.mask	  =LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE | LVIF_STATE;
				lvI.state	  =0;
				lvI.stateMask =0;
				LayerIns(m_ListCtl[0].GetItemCount(),0,0,NULL,lvI,tmprb);
			}
		}
		if(tmprb!=NULL) {sds_relrb(tmprb);tmprb=NULL;}
		tmprb=SDS_tblgetter(IEXP_EntityCol[IEXP_CurrentIndx].cType,NULL,FALSE,IEXP_ODNode->m_dbDrawing);

	}while(tmprb);

}

void IEXP_CdetailView::RemoveXrefLayers()
{
	int Index=0;
	Index=m_ItemIndex=(m_ListCtl[0].GetItemCount()-1);
	for( ; m_ItemIndex != -1; --m_ItemIndex)
	{
		IEXP_ItemDefPtr=(IEXP_ItemDef  *)m_ListCtl[0].GetItemData(m_ItemIndex);
		if(strchr(IEXP_ItemDefPtr->name(),IEXP_VER_BAR))
		{

			if (!m_ListCtl[0].DeleteItem(m_ItemIndex))
				return;
			delete IEXP_ItemDefPtr;
			IEXP_ItemDefPtr = NULL;
			IEXP_LayerCnt--;

		}

	}

	if(m_ListCtl[IEXP_CurrentIndx].GetItemCount( ) >= 1 &&
		m_ListCtl[IEXP_CurrentIndx].GetSelectedCount() < 1 )
	{
		m_ListCtl[IEXP_CurrentIndx].SetFocus();
		m_ListCtl[IEXP_CurrentIndx].SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
	}
}
//Added Cybage AJK 10/12/2001 DD/MM/YYYY ]

