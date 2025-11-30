/* LIB\SDS\SDS_GET.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * Getting Input Functions
 */

//** Includes
#include "Icad.h"/*DNT*/
#include "IcadDoc.h"/*DNT*/
#include "sdsthread.h"/*DNT*/
#include "sdsapplication.h"/*DNT*/
#include "lisp.h"/*DNT*/
#include "cmds.h"/*DNT*/
#include "threadcontroller.h"/*DNT*/
#include <mmsystem.h>
#include <dlgs.h>
#include "win32misc.h"/*DNT*/
#include "commandqueue.h"/*DNT*/
#include "IcadView.h"/*DNT*/
#include "IcadCursor.h"/*DNT*/
#include "IcadApp.h"/*DNT*/
// Modified PK 28/06/2000
#include "sdsthreadexception.h"/*DNT*/ // Reason: Modification in fix for bug 56908
#include "paths.h"/*DNT*/
#include "osnapLocker.h"/*DNT*/
#include "preferences.h"/*DNT*/
#include "icadgridiconsnap.h"

static int    SDS_ArbInputFlag;
extern int    SDS_LispPCount;
extern int    SDS_AtCmdLine;
extern int    SDS_IsDragging;
extern int    SDS_AtMouseMove;
extern char   SDS_cmdcharbuf[IC_ACADBUF];
extern int    SDS_userbreak;
extern sds_point SDS_PointDisplay;
extern int    SDS_PointDisplayMode;
extern int    SDS_SkipLastOrthoSet;
extern LONG   SDS_Inside3rdPartyFunc;
extern bool   SDS_DoingLispCode;
// Modified PK 28/06/2000
//extern struct SDS_dragvars *SDS_Tvars;  Reason : Modification in fix for bug 56908

//4M Spiros Item:27->
extern int GripRClickedFlag;
extern sds_point    SDS_GripRClicked;
//4M Spiros Item:27<-
sds_point SDS_PreGridPoint;
int       SDS_SavingAsR12;
bool      SDS_AtAngDist;
int       SDS_LastFilterIndex;
// Modified PK 31/05/2000
int calledfrom_cmd_setvar;	// Reason : Fix for bug no. 74219
// Modified PK 12/07/2000
int SDS_CalledFromgetfiled; // Reason : Fix for bug no. 47174

int SDS_PaintPreview(char *dwgfile,HWND hWnd);

UINT APIENTRY sds_getfiledHookProc(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam);
UINT APIENTRY sds_getfiledHookProcOldStyle(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam);

void SDS_SetOsnapCursor(char *snapstr,int *osnapmode,bool readvar);


#define MAXOSNAPS 26


// KEYWORD STUFF
static void splitIntoKeywordList(CString fs1, struct kword **keywordList);
struct kword {
    CString wholeKey;       // The whole keyword, including underscore if one.
    CString leftWithCaps;   // All letters from left to and including last uppercase.
    CString capsOnly;       // Only uppercase keyword short cut keys and underscores if one.

    struct kword *next;
};
#define SPACE					" "/*DNT*/
#define TILDE					"~"/*DNT*/
#define COMMA				    ","/*DNT*/
#define TILDECHR				'~'/*DNT*/
#define COMMACHR				','/*DNT*/
#define EMPTYSTR				""/*DNT*/
#define UNDERSCORE				"_"/*DNT*/
#define UNDERSCORECHR			'_'/*DNT*/
#define SPACETILDEUNDERSCORE	" ~_"/*DNT*/
#define SPACEUNDERSCORE			" _"/*DNT*/
#define TILEDUNDERSCORE			"~_"/*DNT*/
#define VERTICALBAR				"|"/*DNT*/

// OSNAP STUFF
struct osnapdata {
	char *name;
	char *psnapname;
	int osnapmode;
	int cursor;
};

struct osd2 {

public:
	osd2::osd2(void);
	osd2::~osd2(void);

	struct osnapdata data[MAXOSNAPS];
} ;


osd2::osd2(void)
{
	db_astrncpy(&data[0].name,ResourceString(IDC_SDS_GET_END_6,  "END"),-1);
	db_astrncpy(&data[0].psnapname,ResourceString(IDC_SDS_GET_ENDPOINT_7, "endpoint" ),-1);
	data[0].osnapmode=IC_OSMODE_END;
	data[0].cursor=IDC_ENDPOINT;
	db_astrncpy(&data[1].name,ResourceString(IDC_SDS_GET_MID_8,  "MID" ),-1);
	db_astrncpy(&data[1].psnapname,ResourceString(IDC_SDS_GET_MIDPOINT_9, "midpoint" ),-1);
	data[1].osnapmode=IC_OSMODE_MID;
	data[1].cursor=IDC_MIDPOINT;
	db_astrncpy(&data[2].name,ResourceString(IDC_SDS_GET_CEN_10, "CEN" ),-1);
	db_astrncpy(&data[2].psnapname,ResourceString(IDC_SDS_GET_CENTERPOINT_11, "centerpoint" ),-1);
	data[2].osnapmode=IC_OSMODE_CENTER;
	data[2].cursor=IDC_CENTER;
	db_astrncpy(&data[3].name,ResourceString(IDC_SDS_GET_NOD_12, "NOD" ),-1);
	db_astrncpy(&data[3].psnapname,ResourceString(IDC_SDS_GET_NODEPOINT_13, "nodepoint" ),-1);
	data[3].osnapmode=IC_OSMODE_NODE;
	data[3].cursor=IDC_NODE;
	db_astrncpy(&data[4].name,ResourceString(IDC_SDS_GET_QUA_14, "QUA" ),-1);
	db_astrncpy(&data[4].psnapname,ResourceString(IDC_SDS_GET_QUADRANT_15, "quadrant" ),-1);
	data[4].osnapmode=IC_OSMODE_QUADRANT;
	data[4].cursor=IDC_QUAD;
	db_astrncpy(&data[5].name,ResourceString(IDC_SDS_GET_INT_16, "INT" ),-1);
	db_astrncpy(&data[5].psnapname,ResourceString(IDC_SDS_GET_INTERSECTION_17, "intersection" ),-1);
	data[5].osnapmode=IC_OSMODE_INT;
	data[5].cursor=IDC_ICAD_OS_INT3D;
	db_astrncpy(&data[6].name,ResourceString(IDC_SDS_GET_INS_18, "INS" ),-1);
	db_astrncpy(&data[6].psnapname,ResourceString(IDC_SDS_GET_INSERTION_19, "insertion" ),-1);
	data[6].osnapmode=IC_OSMODE_INS;
	data[6].cursor=IDC_INSERT;
	db_astrncpy(&data[7].name,ResourceString(IDC_SDS_GET_PER_20, "PER" ),-1);
	db_astrncpy(&data[7].psnapname,ResourceString(IDC_SDS_GET_PERPENDICULAR_21, "perpendicular" ),-1);
	data[7].osnapmode=IC_OSMODE_PERP;
	data[7].cursor=IDC_PERPEN;
	db_astrncpy(&data[8].name,ResourceString(IDC_SDS_GET_TAN_22, "TAN" ),-1);
	db_astrncpy(&data[8].psnapname,ResourceString(IDC_SDS_GET_TANGENT_23, "tangent" ),-1);
	data[8].osnapmode=IC_OSMODE_TAN;
	data[8].cursor=IDC_TANGENT;
	db_astrncpy(&data[9].name,ResourceString(IDC_SDS_GET_NEA_24, "NEA" ),-1);
	db_astrncpy(&data[9].psnapname,ResourceString(IDC_SDS_GET_NEAREST_25, "nearest" ),-1);
	data[9].osnapmode=IC_OSMODE_NEAR;
	data[9].cursor=IDC_NEAR;
	db_astrncpy(&data[10].name,ResourceString(IDC_SDS_GET_QUI_26, "QUI" ),-1);
	db_astrncpy(&data[10].psnapname,ResourceString(IDC_SDS_GET_QUICK_27, "quick" ),-1);
	data[10].osnapmode=IC_OSMODE_QUICK;
	data[10].cursor=IDC_ICAD_SNAP;
	db_astrncpy(&data[11].name,ResourceString(IDC_SDS_GET_PLA_30, "PLA" ),-1);
	db_astrncpy(&data[11].psnapname,ResourceString(IDC_SDS_GET_PLAN_VIEW_INTER_33, "plan view intersection" ),-1);
	data[11].osnapmode=IC_OSMODE_PLAN;
	data[11].cursor=IDC_ICAD_OS_INT2D;
	db_astrncpy(&data[12].name,ResourceString(IDC_SDS_GET_NOD_12A, "POI" ),-1);
	db_astrncpy(&data[12].psnapname,ResourceString(IDC_SDS_GET_NODEPOINT_13, "nodepoint" ),-1);
	data[12].osnapmode=IC_OSMODE_NODE;
	data[12].cursor=IDC_NODE;
	//handle _ values since names may be different in localized versions.
	db_astrncpy(&data[13].name,"_END"/*DNT*/,-1);
	db_astrncpy(&data[13].psnapname,ResourceString(IDC_SDS_GET_ENDPOINT_7, "endpoint" ),-1);
	data[13].osnapmode=IC_OSMODE_END;
	data[13].cursor=IDC_ENDPOINT;
	db_astrncpy(&data[14].name,"_MID"/*DNT*/,-1);
	db_astrncpy(&data[14].psnapname,ResourceString(IDC_SDS_GET_MIDPOINT_9, "midpoint" ),-1);
	data[14].osnapmode=IC_OSMODE_MID;
	data[14].cursor=IDC_MIDPOINT;
	db_astrncpy(&data[15].name,"_CEN"/*DNT*/,-1);
	db_astrncpy(&data[15].psnapname,ResourceString(IDC_SDS_GET_CENTERPOINT_11, "centerpoint" ),-1);
	data[15].osnapmode=IC_OSMODE_CENTER;
	data[15].cursor=IDC_CENTER;
	db_astrncpy(&data[16].name,"_NOD"/*DNT*/,-1);
	db_astrncpy(&data[16].psnapname,ResourceString(IDC_SDS_GET_NODEPOINT_13, "nodepoint" ),-1);
	data[16].osnapmode=IC_OSMODE_NODE;
	data[16].cursor=IDC_NODE;
	db_astrncpy(&data[17].name,"_QUA"/*DNT*/,-1);
	db_astrncpy(&data[17].psnapname,ResourceString(IDC_SDS_GET_QUADRANT_15, "quadrant" ),-1);
	data[17].osnapmode=IC_OSMODE_QUADRANT;
	data[17].cursor=IDC_QUAD;
	db_astrncpy(&data[18].name, "_INT"/*DNT*/,-1);
	db_astrncpy(&data[18].psnapname,ResourceString(IDC_SDS_GET_INTERSECTION_17, "intersection" ),-1);
	data[18].osnapmode=IC_OSMODE_INT;
	data[18].cursor=IDC_ICAD_OS_INT3D;
	db_astrncpy(&data[19].name, "_INS"/*DNT*/,-1);
	db_astrncpy(&data[19].psnapname,ResourceString(IDC_SDS_GET_INSERTION_19, "insertion" ),-1);
	data[19].osnapmode=IC_OSMODE_INS;
	data[19].cursor=IDC_INSERT;
	db_astrncpy(&data[20].name, "_PER"/*DNT*/,-1);
	db_astrncpy(&data[20].psnapname,ResourceString(IDC_SDS_GET_PERPENDICULAR_21, "perpendicular" ),-1);
	data[20].osnapmode=IC_OSMODE_PERP;
	data[20].cursor=IDC_PERPEN;
	db_astrncpy(&data[21].name, "_TAN"/*DNT*/,-1);
	db_astrncpy(&data[21].psnapname,ResourceString(IDC_SDS_GET_TANGENT_23, "tangent" ),-1);
	data[21].osnapmode=IC_OSMODE_TAN;
	data[21].cursor=IDC_TANGENT;
	db_astrncpy(&data[22].name, "_NEA"/*DNT*/,-1);
	db_astrncpy(&data[22].psnapname,ResourceString(IDC_SDS_GET_NEAREST_25, "nearest" ),-1);
	data[22].osnapmode=IC_OSMODE_NEAR;
	data[22].cursor=IDC_NEAR;
	db_astrncpy(&data[23].name, "_QUI"/*DNT*/,-1);
	db_astrncpy(&data[23].psnapname,ResourceString(IDC_SDS_GET_QUICK_27, "quick" ),-1);
	data[23].osnapmode=IC_OSMODE_QUICK;
	data[23].cursor=IDC_ICAD_SNAP;
	db_astrncpy(&data[24].name, "_PLA"/*DNT*/,-1);
	db_astrncpy(&data[24].psnapname,ResourceString(IDC_SDS_GET_PLAN_VIEW_INTER_33, "plan view intersection" ),-1);
	data[24].osnapmode=IC_OSMODE_PLAN;
	data[24].cursor=IDC_ICAD_OS_INT2D;
	db_astrncpy(&data[25].name, "_POI"/*DNT*/,-1);
	db_astrncpy(&data[25].psnapname,ResourceString(IDC_SDS_GET_NODEPOINT_13, "nodepoint" ),-1);
	data[25].osnapmode=IC_OSMODE_NODE;
	data[25].cursor=IDC_NODE;
	//Disable VIS until we implement it.
	//db_astrncpy(&data[13].name,ResourceString(IDC_SDS_GET_VIS_28, "VIS" ),-1);
	//db_astrncpy(&data[13].psnapname,ResourceString(IDC_SDS_GET_VISIBLE_INTERSE_29, "visible intersection" ),-1);
	//data[13].osnapmode=IC_OSMODE_APP;
	//data[13].cursor=IDC_ICAD_OS_INTVS;
	//Disable VIS until we implement it.  Should this have been APP?
	//db_astrncpy(&data[14].name,ResourceString(IDC_SDS_GET_VIS_28, "VIS" ),-1);
	//db_astrncpy(&data[14].psnapname,ResourceString(IDC_SDS_GET_VISIBLE_INTERSE_29, "visible intersection" ),-1);
	//data[14].osnapmode=IC_OSMODE_PLAN;
	//data[14].cursor=IDC_ICAD_OS_INT2D;

}

osd2::~osd2(void)
{
	for (int i=0; i<MAXOSNAPS; i++) {
		delete(data[i].name);
		delete(data[i].psnapname);
	}
}

short findosnap(char *osname,struct osnapdata **theosnaprec)
// find the osnap in the table
{
	short i;
	static struct osd2 theosdata;

	if (osname[0] <= '9')
		return(RTERROR);

	if (strnisame(osname,"VIS"/*DNT*/, 3))
		{
		sds_printf (ResourceString(CMD_ERR_UNRECOGSNAP, "Unable to recognize entity snap mode/s."));
		return(RTERROR);
		}
	if (strnisame(osname,"APP"/*DNT*/, 3))
		{
		sds_printf (ResourceString(CMD_ERR_UNRECOGSNAP, "Unable to recognize entity snap mode/s."));
		return(RTERROR);
		}

	for (i=0; i<MAXOSNAPS; i++) {
		int compareLen = 3;

		//need to compare 4 characters if _ involved.
		if (osname[0] == '_')
			compareLen = 4;

		if (strnisame(theosdata.data[i].name,osname,compareLen)) {
// the special cases
// code said that not doing this broke "middle" on text
            if (strisame(osname,"PLAN"/*DNT*/) || strisame(osname,"_PLAN"/*DNT*/)) break;
            if (strisame(osname,"POINT"/*DNT*/) || strisame(osname,"_POINT"/*DNT*/)) break;
			if (strisame(osname,"INSERT"/*DNT*/) || strisame(osname,"_INSERT"/*DNT*/)) break;

			if ((strnisame(osname,"MID"/*DNT*/,3) || strnisame(osname,"_MID"/*DNT*/,4))  && (osname[compareLen]=='D' || osname[compareLen]=='d')) continue;

// code had no comments on these two
			if ((strnisame(osname,"PER"/*DNT*/,3) || strnisame(osname,"_PER"/*DNT*/,4)) && osname[compareLen]!=0 && osname[compareLen]!='P' && osname[compareLen]!='p') continue;
			if ((strnisame(osname,"QUI"/*DNT*/,3) || strnisame(osname,"_QUI"/*DNT*/,4)) && osname[compareLen]!=0 && osname[compareLen]!='C' && osname[compareLen]!='c') continue;
			*theosnaprec=&theosdata.data[i];
			return(RTNORM);
		}
	}
	return(RTERROR);
}

/****************************************************************************/
/******************** Getting Input Functions *******************************/
/****************************************************************************/

//** Global Vars
int  SDS_GetCornerMode;


//** Protos
int SDS_GetNumber(const char *prompt, int *resint, sds_real *resreal);
int SDS_GetAngDist(const sds_point ptStart, const char *szMsg, double *pdResult, getangdist_mode nMode);
int SDS_SetKeyWord(const char *str);

//** Functions

int		SDS_DidDraggenOnce;
BOOL	cmd_InsideSds_draggen;

// *****************************************
// This is an SDS External API
//
int sdsengine_draggen(const sds_name nmSelSet, const char *szDragMsg, int nCursor,
				int (*fnDragEnts)(sds_point ptCursorLoc, sds_matrix mxTransform),
				sds_point ptDestPoint) {


	ASSERT( OnEngineThread() );

    int ret,i,j;
    struct SDS_dragvars *tvars;
	//Modified PK 28/06/2000
	bool bWasException = false;
	SDSThreadException exception;
	class bracket
		{
	public:
		bracket()	{ ASSERT( !cmd_InsideSds_draggen); cmd_InsideSds_draggen = TRUE;}
		~bracket()	{ cmd_InsideSds_draggen = FALSE;}
		} bracketObject;

	if(SDS_CURDOC==NULL || SDS_CURDWG==NULL) return(RTERROR);


	// Save off the initget flags before something resets them
	//
	int iInitGetFlags =	SDSApplication::GetActiveApplication()->GetInputControlBits();

	//Modified PK 28/06/2000
	//tvars = SDS_Tvars = SDS_CMainWindow->m_pDragVarsCur; Reason: Modification in fix for bug 56908
	tvars = SDS_CMainWindow->m_pDragVarsCur;

    //if((SDS_CMainWindow->m_pDragVarsCur=(struct SDS_dragvars *)calloc(1,sizeof(struct SDS_dragvars)))==NULL)
	//	return(RTERROR);
	if((SDS_CMainWindow->m_pDragVarsCur= new struct SDS_dragvars )==NULL)
		return(RTERROR);
	memset(SDS_CMainWindow->m_pDragVarsCur,0,sizeof(struct SDS_dragvars));


    SDS_CMainWindow->m_pDragVarsCur->breakout=0;
    SDS_CMainWindow->m_pDragVarsCur->scnf=fnDragEnts;
    SDS_CMainWindow->m_pDragVarsCur->cursor=nCursor;
    ic_ptcpy(SDS_CMainWindow->m_pDragVarsCur->pt1,ptDestPoint);

    if(nmSelSet) {
		ic_encpy(SDS_CMainWindow->m_pDragVarsCur->nmSelSet,nmSelSet);
//		if(!cmd_InsideScaleCommand && !cmd_InsideRotateCommand && !cmd_InsideMoveCopyCommand) {
			POSITION pos=SDS_CURDOC->GetFirstViewPosition();
			while (pos!=NULL) {
				CIcadView *pView=(CIcadView *)SDS_CURDOC->GetNextView(pos);
				ASSERT_KINDOF(CIcadView, pView);

				SDS_CMainWindow->m_pDragVarsCur->vects=SDS_SSToVects(nmSelSet,SDS_CURDWG,pView->m_pCurDwgView);
				pView->m_pDragVects=SDS_CMainWindow->m_pDragVarsCur->vects;
			}
//		}
	} else {
		SDS_CMainWindow->m_pDragVarsCur->nmSelSet[0]=0L;
		SDS_CMainWindow->m_pDragVarsCur->nmSelSet[1]=0L;
	}

    for(i=0; i<=3; i++)
        for(j=0; j<=3; j++)
            SDS_CMainWindow->m_pDragVarsCur->matx[i][j]=0.0;
    for(i=0; i<=3; i++)
        SDS_CMainWindow->m_pDragVarsCur->matx[i][i]=1.0;


    SDS_IsDragging=1;
	switch(nCursor) {
		case 0: SDS_SetCursor(IDC_ICAD_CROSS);      break;
		case 1: SDS_SetCursor(IDC_EMPTY);			break;
		case 2: SDS_SetCursor(SDS_GetPickCursor()); break;
	}
	SDS_DidDraggenOnce=0;
	//Modified PK 28/06/2000 [
	//ret=SDS_AskForPoint( NULL,szDragMsg,ptDestPoint ); Reason: Modification in fix for bug 56908
	try
	{
		ret=SDS_AskForPoint( NULL,szDragMsg,ptDestPoint );
	}
	catch( SDSThreadException& theException )
	{
		bWasException = true;
		exception = theException;
		long ssct;
		sds_name ename;
		for(ssct=0L; sds_ssname(nmSelSet,ssct,ename)==RTNORM; ssct++)
			sds_redraw(ename,IC_REDRAW_UNHILITE);
	}
	//Modified PK 28/06/2000 ]
	if(ret==RTERROR)
		ret=RTCAN;

	// BUG FIX for Ketiv.  BUG #45683
	// If SDS_RSG_OTHER is passed to sds_initget(), then this should return
	// RTSTR instead of RTKWORD.  Fixing this deeper in SDS_AskForPoint is
	// SDS_AskingForTrouble, so I hacked this instead.
	//  See SDS_SetKeyWord() and SDS_ArbInputFlag for related information
	//
	if ((ret == RTKWORD) &&
		(SDS_ArbInputFlag))
		{
		ret = RTSTR;
		}
    SDS_IsDragging=0;

	// OH BOY...Isn't this a nice little uncommented hack
	// Apparently we mustn't send any of these messages until we get out
	// of ::MouseMove handler
	//

	// TODO TODO Remove permanently when determined this works
	//
//	while(SDS_AtMouseMove);

//	if(SDS_CMainWindow->m_pDragVarsCur->vects && SDS_DidDraggenOnce) SDS_DrawDragVectors(NULL);

	ExecuteUIAction( ICAD_WNDACTION_ENDDRAG );
	ExecuteUIAction( ICAD_WNDACTION_FREEVIEWRBS );

    delete SDS_CMainWindow->m_pDragVarsCur;
    SDS_CMainWindow->m_pDragVarsCur=tvars;

    sds_initget(0,""/*DNT*/); // Reset sds_initget()
	// Modified PK 28/06/2000
	if( bWasException ) // Reason: Modification in fix for bug 56908
		throw exception;
    return(ret);
}

// *****************************************
// This is an SDS External API
//

	// Bitflags for bsAllowed
#define	GRREAD_COORDINATES		0x1
#define GRREAD_KEYSTROKES		0x2
#define GRREAD_SETCURSOR		0x4
#define	GRREAD_NO_CONSOLE_BREAK	0x8

	// Options for user cursor
#define	GRREAD_NORMAL			0
#define	GRREAD_NO_CURSOR		1
#define	GRREAD_SELECTION_BOX	2


	// This class overrides the system cursor when sds_grread is active
	// The system cursor is stacked and restored when this is destroyed
class UserCursor : public IcadCursor
	{
public:
	UserCursor( int cursor);
	~UserCursor();
	UserCursor& operator =(const IcadCursor&other)
		{
		*((IcadCursor *)this) = other;
		return *this;
		}

	virtual int		SetType( int);
	protected:
		IcadCursor	*m_lastCursor;
		int			m_userMode;
	};


UserCursor::UserCursor( int userMode) : m_lastCursor(NULL), m_userMode( userMode)
	{
	if ( m_userMode != GRREAD_NORMAL )
		{
		CIcadApp	*app = GetApp();

								// save current cursor
		m_lastCursor = app->m_cursor;
		*this = *m_lastCursor;
		app->m_cursor = this;

		switch( m_userMode )
			{
			case GRREAD_NO_CURSOR:
                SetType(m_userMode);
                break;
			case GRREAD_SELECTION_BOX:
                // Gets a pick cursor based on PICKBOX variable.
                SetType(SDS_GetPickCursor());
				break;
			}
		}
	}


UserCursor::~UserCursor()
	{
								// restore previous cursor
	if ( m_lastCursor )
		{
		CIcadApp	*app = GetApp();

								// settings like cursorsize may have changed
		*m_lastCursor = *this;
		app->m_cursor = m_lastCursor;
		}
	}

int
UserCursor::SetType( int type)
	{
	return IcadCursor::SetType( type);
//	return m_lastCursor->SetType( type);
	}


int
sdsengine_grread(int bsAllowed, int *pnInputType, struct sds_resbuf *prbInputValue)
	{

	ASSERT( OnEngineThread() );

	if( pnInputType==NULL ||
		prbInputValue==NULL)
		{
		return(RTERROR);
		}

		// validate user cursor selection
	if ( bsAllowed & GRREAD_SETCURSOR )
		if ( !( 0 <= *pnInputType && *pnInputType <= 2 ))
			return(RTERROR);

    int retcode=RTNORM;

    // Our grread does NOT handle menu, button, or tablet selections!
    SDS_EventMask(((bsAllowed&1) ? SDS_EVM_MOUSEXYZ : 0) |
                     SDS_EVM_KEYCHAR|SDS_EVM_MOUSEBUTTON|SDS_EVM_POPUPCOMMAND);

	static int PendingGrread;
	static sds_point PendingPoint;

	if(PendingGrread)
		{
		PendingGrread=0;
		*pnInputType=12;
        prbInputValue->restype=RT3DPOINT;
        ic_ptcpy(prbInputValue->resval.rpoint,PendingPoint);
		return(retcode);
		}

    if( GetActiveCommandQueue()->IsEmpty() &&
		SDSApplication::GetActiveApplication()->IsCommandRunning() &&
		!IDS_GetThreadController()->GetNoThreadSwap())
		{

        // If there is no buffer list clear the script list.
		GetScriptQueue()->Flush();

        SDSApplication::GetActiveApplication()->SetCommandRunning( false );

        // Resume the application thread.
		// ****************************************
		// THREADSWAP !!!!!!!!!!!!!!!!!
		if ( SDSApplication::GetActiveApplication()->GetCommandThread()->IsCurrent() )
			{
			SDSApplication *pStartApp = SDSApplication::GetActiveApplication();
			SDSApplication *pStopApp = SDSApplication::GetActiveApplication();

			SDS_ThreadSwap( pStartApp->GetMainThread(), pStopApp->GetCommandThread());
			}
		}

    if( GetActiveCommandQueue()->IsEmpty() )
		{
		extern int SDS_NoCharEcho;
		extern int SDS_InsideGrread;
		SDS_NoCharEcho=1;
		SDS_InsideGrread=1;

		int cursorMode = GRREAD_NORMAL;
		if ( bsAllowed & GRREAD_SETCURSOR )
			cursorMode = *pnInputType;

								// push user cursor onto stack
		UserCursor		local( cursorMode);
        while(SDS_WaitLoop())
			{
			if ( IDS_IsIcadExiting() ||
                 sds_usrbrk() )
				{
				*pnInputType=2;
                retcode=RTCAN;
                prbInputValue->restype=RTSHORT;
                prbInputValue->resval.rint=13;
                break;
	            }

            if(SDS_cmdcharbuf[0])
				{
                *pnInputType=2;
                prbInputValue->restype=RTSHORT;
                prbInputValue->resval.rint=SDS_cmdcharbuf[0];
				SDS_cmdcharbuf[0]=0;
                break;
	            }


            if( GetActiveCommandQueue()->IsNotEmpty() )
				{
				break;
				}
			} // end of while()

		SDS_InsideGrread=0;
		SDS_NoCharEcho=0;

		}
	else
		{
        if(sds_usrbrk())
			{
			SDS_CancelAllPending(1);
            return(RTCAN);
	        }
		}

    if( GetActiveCommandQueue()->IsNotEmpty())
		{

        if( GetActiveCommandQueue()->PeekItem()->restype==RTSTR)
			{
            *pnInputType=2;
            prbInputValue->restype=RTSHORT;
			if(!GetActiveCommandQueue()->PeekItem()->resval.rstring[0])
				{
				prbInputValue->resval.rint='\r';
				}
			else
				{
				prbInputValue->resval.rint=GetActiveCommandQueue()->PeekItem()->resval.rstring[0];
				}

            SDS_FreeEventList(0);
			}
		else if(GetActiveCommandQueue()->PeekItem()->restype==RT3DPOINT)
			{

            *pnInputType=3;
            prbInputValue->restype=RT3DPOINT;
            ic_ptcpy(prbInputValue->resval.rpoint,GetActiveCommandQueue()->PeekItem()->resval.rpoint);
            SDS_FreeEventList(0);
			}
		else if(GetActiveCommandQueue()->PeekItem()->restype==SDS_RTDRAGPT)
			{

            *pnInputType=5;
            prbInputValue->restype=RT3DPOINT;
            ic_ptcpy(prbInputValue->resval.rpoint,GetActiveCommandQueue()->PeekItem()->resval.rpoint);
            SDS_FreeEventList(0);
			}
		else if(GetActiveCommandQueue()->PeekItem()->restype==SDS_EVM_AUXMENU)
			{
            *pnInputType=11;
			prbInputValue->resval.rint=GetActiveCommandQueue()->PeekItem()->resval.rint;



			GetActiveCommandQueue()->AdvanceNext();
			if( GetActiveCommandQueue()->IsNotEmpty() &&
				(GetActiveCommandQueue()->PeekItem()->restype==SDS_RTDRAGPT) )
				{
				PendingGrread=1;
				ic_ptcpy(PendingPoint,GetActiveCommandQueue()->PeekItem()->resval.rpoint);
				}
            SDS_FreeEventList(0);
		    }
		} // end of if (GetActiveCommandQueue()->IsNotEmpty())

    return(retcode);

	} // endf of sdsengine_grread()




// *****************************************
// This is an SDS External API
//
int sdsengine_getfiled(const char *szTitle, const char *szDefaultPath, const char *szExtension, int bsOptions, struct sds_resbuf *prbFileName)
	{
	return SDS_GetFiled(szTitle,szDefaultPath,szExtension,bsOptions,prbFileName,NULL,NULL);
	}

static int moduleVariable_bsOptions;

int SDS_GetFiled(
	const char *szTitle, const char *szDefaultPath, const char *szExtension, int bsOptions,
	struct sds_resbuf *prbFileName, const char* lpTemplateName, void (*pOnOkCallback)(HWND))
{
/*D.G.*/// Enabled for DBCS.

//*** This version of getfiled allows you to specify a dialog template to add to the
//*** open/save common dialog.  This function also accepts a pointer to a callback fuction
//*** that gets called when the Accept button is pressed.  ~| TODO - However this is not
//*** hooked up for NT 3.51 yet.
	//NOTE: this function checks for bit 5 (32) to see if you DON'T want verification
	//	dialog box if user picks and existing file instead of a new one (bit 0 must also be set)
//*** EBATECH(CNBR) 2002/4/24 for Multiple file selection
	//	bit 15 (+32768) allows to select multiple files.
	//	prbFileName fills as list of filenames if it suceeds.

    char szName[IC_PATHSIZE+IC_FILESIZE],szPath[IC_PATHSIZE],*fcp1,*fcp2,*fcp3,*allocstr=NULL,fc1,files[IC_ACADBUF*3];
	char* szFilenames = NULL;	// EBATECH(CNBR) for Multiple file selection
    OPENFILENAME ofnTemp;
    DWORD        Errval;
    BOOL         ret;
    int          len;
	OSVERSIONINFO osVer;

	moduleVariable_bsOptions=bsOptions;

	osVer.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
	GetVersionEx(&osVer);
    //*** Filter

	/*D.Gavrilov*/// We'll form a path accordingly to file extension and configuration path.
	enum tagFileType
	{
		DWG_DXF, SHX_SHP, XREF, MNU_MNS, PAT, DWT, OTHER
	} FileType = OTHER;

    //Modified Cybage VSB 13/11/2001 DD/MM/YYYY
	//Reason : Fix for Bug No. 77928 from Bugzilla [
	//if(szExtension==NULL || !*szExtension || strsame("*.*"/*DNT*/,szExtension)) {
	if(szExtension==NULL || !*szExtension || strsame("*.*"/*DNT*/,szExtension) || strsame("*"/*DNT*/,szExtension)) 
	{
		//Modified Cybage VSB 13/11/2001 DD/MM/YYYY ]
        strcpy(files,ResourceString(IDC_SDS_GET_ALL_FILES________0, "All files(*.*),*.*," ));
    } 
	else 
	{
		char *szExt; // a pointer to file extension
        db_astrncpy(&allocstr,(char *)szExtension,-1);
        for(*files=0,fcp2=fcp1=(char *)allocstr;fcp2!=NULL;fcp1=fcp2) 
		{
            if ((fcp2=_tcschr(fcp2,','))!=NULL) 
			{ 
				*fcp2=0; 
				fcp2=fcp2+1; 
			} 
			else 
			{
				fcp2=NULL;
			}

            len=strlen(files);
			if(NULL!=(fcp3=_tcschr(fcp1,'|'))) 
			{
				*fcp3=0;
				sprintf(files+len,"%s (%s),*.%s,"/*DNT*/,fcp1,fcp3+1,fcp3+1);
				szExt = fcp3+1;
			} 
			else 
			{
				sprintf(files+len,"*.%s,*.%s,"/*DNT*/,fcp1,fcp1);
				szExt = fcp1;
			}
			/*DNT*/// all these extensions
			if(strisame(szExt, "DWG") || strisame(szExt, "DXF") || strisame(szExt, "DWF"))
			{
				FileType = DWG_DXF;
			}
			else
			{
				if(strisame(szExt, "SHX") || strisame(szExt, "SHP"))
				{
					FileType = SHX_SHP;
				}
				else
				{
					if(strisame(szExt, "MNU") || strisame(szExt, "MNS"))
					{
						FileType = MNU_MNS;
					}
					else
					{
						if(strisame(szExt, "PAT"))
						{
							FileType = PAT;
						}
						else
						{
							if(strisame(szExt, "DWT"))
							{
								// Only set to the template directory if the DWG directory was not already found earlier in the list of extensions
								// This insures that if multiple extensions are specified that the DWG directory is selected
								// as a higher priority than the template directory
								if (FileType != DWG_DXF)
									FileType = DWT;
							}
						}
					}
				}
			}
        }
		delete allocstr;
		if(bsOptions & 1024)
		{
			FileType = XREF;
		}
    }

    for(fcp1=fcp2=(char *)files;fcp2!=NULL;) 
	{
        if ((fcp1=_tcschr(fcp2,','))!=NULL) 
		{
            *fcp1=0;
            if ((fcp2=_tcschr((fcp1+1),','))!=NULL) 
			{ 
				*fcp2=0; 
				fcp2=fcp2+1; 
			} 
			else 
			{
				fcp2=NULL;
			}
        } 
		else 
		{
			fcp2=NULL;
		}
    }
	// EBATECH(FUTA)-[ 2001-01-20
	// if ext argument is dwg or dxf,display image preview in dialog.
	if(!(bsOptions & 1) && (FileType==DWG_DXF))
	{
		lpTemplateName=MAKEINTRESOURCE(IDD_OPENSAVE_PREVIEW);
	}
	// ]-EBATECH(FUTA)

    if(szDefaultPath!=NULL) 
	{
		strncpy(szName,szDefaultPath,sizeof(szName)-1);
		char* cp1;
		for(cp1=szName; *cp1!=0; cp1++)
		{
			if(*cp1=='/') 
			{
				*cp1='\\';
			}

			if(*cp1=='|') 
			{
				*cp1=' ';
			}
			//EBATECH-[MBCS String
			if((_MBC_LEAD ==_mbbtype((unsigned char)*cp1,0)) && (_MBC_TRAIL==_mbbtype((unsigned char)*(cp1+1),1)))
			{
				cp1 ++;
			}
			//]-EBATECH
		}
    } 
	else 
	{
		*szName=0;
	}

    *szPath=0;
	if(bsOptions&16)
	{
        strncpy(szPath,szName,sizeof(szPath)-1);
		*szName=0;
	}
	else
	{
		//*** Separate path and name
		if((fcp1=_tcsrchr(szName,'\\'))!=NULL)
		{
			fcp1++;
			fc1=*fcp1;
			*fcp1=0;
			strncpy(szPath,szName,sizeof(szPath)-1);
			*fcp1=fc1;
			strncpy(szName,fcp1,sizeof(szName)-1);
		}
		else
		{
			/*D.Gavrilov*/// Take a path from the configuration.
			CString thePath, tempPath;
			extern char *cmd_DwgSavePath;
			switch(FileType)
			{
				case DWG_DXF :	if(strisame(szTitle, "Insert Block"))
									thePath = SDS_CMainWindow->m_paths->m_blocksPath;
								else
								{
									thePath = cmd_DwgSavePath ?
										  cmd_DwgSavePath :
										  SDS_CMainWindow->m_paths->m_drawingPath;
								}
								break;
				case SHX_SHP :	thePath = SDS_CMainWindow->m_paths->m_fontPath;
								break;
				case XREF    :	thePath = SDS_CMainWindow->m_paths->m_xrefPath;
								break;
				case MNU_MNS :	thePath = SDS_CMainWindow->m_paths->m_menuPath;
								break;
				case PAT     :	thePath = SDS_CMainWindow->m_paths->m_hatchPatPath;
								break;
				case DWT     :	thePath = SDS_CMainWindow->m_paths->m_templatesPath;
			}

			while(!thePath.IsEmpty())
			{
				int n = thePath.Find(';');
				if(n != -1)						// Take the first path.
				{
					tempPath = thePath.Right(thePath.GetLength() - n - 1);
					thePath = thePath.Left(n);

				}

				if(thePath.GetLength() && (thePath.ReverseFind('\\') == -1 || thePath.ReverseFind('\\') != thePath.GetLength() -1))
				{
					thePath += "\\";
				}

				if(access(thePath, 0) != -1)
				{
					strcpy(szPath, thePath);
					break;
				}
				else // if first path is not valid; look into remaining path.
					thePath = tempPath;
			}
		}
	}

    ofnTemp.lStructSize = sizeof(OPENFILENAME);
    ofnTemp.hwndOwner = GetActiveWindow();
    if(ofnTemp.hwndOwner==NULL || !::IsWindow(ofnTemp.hwndOwner))
	{
		ofnTemp.hwndOwner = IcadSharedGlobals::GetMainHWND();
	}

	if(lpTemplateName!=NULL) 
	{
		ofnTemp.hInstance = AfxGetApp()->m_hInstance;
	}
	else 
	{
		ofnTemp.hInstance = 0;
	}

    ofnTemp.lpstrFilter=(LPSTR)files;
	//Modified Cybage VSB 13/11/2001 DD/MM/YYYY
	//Reason : Fix for Bug No. 77928 from Bugzilla [
    if ( (!szExtension) || ( strsame("*.*"/*DNT*/,szExtension) || strsame("*"/*DNT*/,szExtension) ) )
	{
		ofnTemp.lpstrDefExt="";
	}
	else
	{
		ofnTemp.lpstrDefExt=szExtension;
	}
	//Modified Cybage VSB 13/11/2001 DD/MM/YYYY ]
    ofnTemp.lpstrCustomFilter = NULL;
    ofnTemp.nMaxCustFilter = 0;

	if(SDS_SavingAsR12) 
	{
		ofnTemp.nFilterIndex = 3; //*** extidx;
	} 
	else 
	{
		ofnTemp.nFilterIndex = 1; //*** extidx;
	}

    ofnTemp.lpstrFile = (LPSTR)szName;
    ofnTemp.nMaxFile = sizeof(szName);
    ofnTemp.lpstrFileTitle = NULL;
    ofnTemp.nMaxFileTitle = 0;
    ofnTemp.lpstrInitialDir = (LPSTR)szPath;
    ofnTemp.lpstrTitle = szTitle;
	//*** Set the flags
	// Add the ability to re-size the dialog
    ofnTemp.Flags = OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_ENABLEHOOK | OFN_NOCHANGEDIR | OFN_ENABLESIZING;
    if((bsOptions&1) && !(bsOptions&32)) 
	{
		ofnTemp.Flags |= (OFN_OVERWRITEPROMPT|OFN_NOREADONLYRETURN);
	}
    else 
	{
		ofnTemp.Flags |= OFN_FILEMUSTEXIST;
	}

    if(bsOptions&4)
	{
		ofnTemp.Flags |= OFN_EXTENSIONDIFFERENT;
	}

    if(bsOptions&256) 
	{
		ofnTemp.Flags |= OFN_NOVALIDATE;
	}

	if(lpTemplateName!=NULL)
	{
		ofnTemp.Flags |= OFN_ENABLETEMPLATE;
	}

	if(SDS_SavingAsR12) 
	{
		ofnTemp.Flags |= OFN_NOLONGNAMES;
	}

    ofnTemp.nFileOffset = 0;
    ofnTemp.nFileExtension = 0;
    if(lpTemplateName!=NULL)
	{
		ofnTemp.lCustData = (DWORD)pOnOkCallback;
	}
    else 
	{
		ofnTemp.lCustData = 0;
	}

	if(osVer.dwPlatformId==VER_PLATFORM_WIN32_WINDOWS ||
		(osVer.dwPlatformId==VER_PLATFORM_WIN32_NT && osVer.dwMajorVersion>=4))
	{
		//*** Windows 95 or NT 4.0
		ofnTemp.Flags|=OFN_EXPLORER;
 		ofnTemp.lpfnHook = sds_getfiledHookProc;
	}
	else
	{
 		ofnTemp.lpfnHook = sds_getfiledHookProcOldStyle;
	}
    ofnTemp.lpTemplateName = lpTemplateName;

	// EBATECH(CNBR) -[ 2002/4/24 Multiple file selection
	if( bsOptions & 32768 )
	{
		if( szFilenames = new char[IC_ACADBUF*3])
		{
			ofnTemp.lpstrFile = szFilenames;
			*szFilenames = 0;
			ofnTemp.nMaxFile = IC_ACADBUF*3;
			ofnTemp.Flags |= OFN_ALLOWMULTISELECT;
		}
	}
	// EBATECH(CNBR) ]-

	// HACK!!!  These common dialog functions are changing the locale settings, globally and
	// not setting them back.  We'll fix that!

	// Here's the actual dialog box call
    if(bsOptions&1) 
	{
        ret=GetSaveFileName(&ofnTemp);
    } 
	else 
	{
        ret=GetOpenFileName(&ofnTemp);
    }

	// HACK!!! Now reset locale.
	ic_setlocale();

	SDS_PaintPreview(NULL,NULL);
	SDS_LastFilterIndex=ofnTemp.nFilterIndex;
    if(ret!=TRUE) 
	{
        Errval=CommDlgExtendedError();
        if(Errval!=0) 
		{
			cmd_ErrorPrompt(CMD_ERR_GETOPENFILE,0,(void *)Errval);
        }
		// EBATECH(CNBR) -[ 2002/4/24 Multiple file selection (error case)
        if( szFilenames ) 
		{
			delete [] szFilenames;
		}
		// EBATECH(CNBR) ]-
        return(RTERROR);
    } 
	else 
	{
		// When multiple files is selected, szFilenames stores like this:
		// 'c:\\data\\job1\\\0file1.dwg\0file2.dwg\0\0'
		//                    ^- nFileOffset
		// When single file is selected, szFilenames stores like this:
		// 'c:\\autoexec.bat'
		//      ^-nFileOffset
		//
		if( ofnTemp.lpstrFile[ofnTemp.nFileOffset-1] == '\0' )
		{
			char *path=NULL,*file=NULL;
			struct resbuf *nb,*cp;
			if( strsame(szFilenames+strlen(szFilenames)-2, ":\\"))
			{
				path = strdup(szFilenames);
			}
			else
			{
				path = new char[strlen(szFilenames)+2];
				strcpy(path,szFilenames);
				strcat(path,"\\");
			}
			file = &szFilenames[ofnTemp.nFileOffset]; // here is 1st filename
			cp = prbFileName;
			cp->restype=RTLB;

			do{
				if((nb = sds_newrb(RTSTR)) == NULL )
				{
			        if( szFilenames ) delete [] szFilenames;
					if( path ) delete [] path ;
					return(RTERROR);
				}
				if((nb->resval.rstring = new char[ strlen(path) + strlen(file) + 1 ]) == NULL )
				{
			        if( szFilenames ) delete [] szFilenames ;
					if( path ) delete [] path ;
					return(RTERROR);
				}
				strcpy(nb->resval.rstring, path);
				if( strlen(file) > 0 )
				{
					strcat(nb->resval.rstring, file);
					file = file + strlen(file) + 1;
				}
				cp->rbnext = nb;
				cp = nb;
			}while( *file );

			cp->rbnext = sds_newrb(RTLE);
			if( szFilenames )
			{
				delete [] szFilenames ;
			}

			if( path )
			{
				delete [] path ;
			}

		// One file is selected. It's simply "c:\data\job\file.dwg".
		}
		else if( *(ofnTemp.lpstrFile) )
		{
			prbFileName->restype=RTSTR;
			if ((prbFileName->resval.rstring= new char [strlen(ofnTemp.lpstrFile)+1])==NULL)
			{
				delete [] szFilenames;
				return(RTERROR);
			}
			strcpy(prbFileName->resval.rstring, ofnTemp.lpstrFile );
			delete [] szFilenames;
		}
//		if(*szName!=0)
//		{
//			prbFileName->restype=RTSTR;
//			if ((prbFileName->resval.rstring=(char *)malloc(strlen(szName)+1))==NULL) return(RTERROR);
//			strcpy(prbFileName->resval.rstring,szName);
//		}
		// EBATECH(CNBR) ]-
        return(RTNORM);
	}
}

void maketimestring(CTime *tm,CString *str)
{
	int nHour,nYear;
    BOOL bPM;

	nHour=tm->GetHour();
	nYear=tm->GetYear();

	bPM=FALSE;
	if(nHour>=12)
	{
		bPM=TRUE;
		if (nHour>12) nHour-=12;
	}
	else if (nHour==0) nHour=12;

//	if (nYear>=2000) nYear-=2000;
//	else nYear-=1900;
#ifdef LOCALISE_ISSUE
// No check to see how default / locale date is to be formatted.
#endif
	str->Format("%d/%d/%d %d:%02d %s"/*DNT*/,tm->GetMonth(),tm->GetDay(),nYear,
		nHour,tm->GetMinute(),(bPM) ? ResourceString(IDC_SDS_GET_PM_1, "PM" ):ResourceString(IDC_SDS_GET_AM_2, "AM" ));
}

UINT APIENTRY sds_getfiledHookProc(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	static CString sstrPreviousFilePath;
	if(uiMsg==WM_NOTIFY)
	{
		LPOFNOTIFY lpOfn = (LPOFNOTIFY)lParam;
		if(lpOfn->hdr.code==CDN_INITDONE)
		{
			CWnd* pWnd = new CWnd();
			pWnd->Attach(GetParent(hdlg));
			pWnd->CenterWindow(SDS_CMainWindow);
			pWnd->ModifyStyleEx(WS_EX_CONTEXTHELP,0);
			pWnd->Detach();
			delete pWnd;
			sstrPreviousFilePath.Empty();
			HWND hItem;
			if(NULL!=(hItem=GetDlgItem(hdlg,IDC_PREVIEW_FILESIZE)))
			{
				SetWindowText(hItem,""/*DNT*/);
				hItem = GetDlgItem(hdlg,IDC_PREVIEW_CREATIONTIME);
				SetWindowText(hItem,""/*DNT*/);
				hItem = GetDlgItem(hdlg,IDC_PREVIEW_MODIFICATIONTIME);
				SetWindowText(hItem,""/*DNT*/);
				hItem = GetDlgItem(hdlg,IDC_PREVIEW_ACCESSTIME);
				SetWindowText(hItem,""/*DNT*/);
			}
			// comment out CUS_TOOLBAR_IMPORT_APPEND: import toolbar should append to existing by default 
			// EBATECH-[ Control Preview for paformance issue.
			else if(NULL!=(hItem=GetDlgItem(hdlg,CUS_SHORTCUTS_IMPORT_APPEND))
				|| NULL!=(hItem=GetDlgItem(hdlg,CUS_TOOLBAR_IMPORT_APPEND)))
			//	|| NULL!=(hItem=GetDlgItem(hdlg,CUS_MENU_IMPORT_APPEND)))
			// EBATECH(CNBR) ]-
			{
				//*** Initialize the check box as checked for these dialogs.
				SendMessage(hItem,BM_SETCHECK,BST_CHECKED,0);
			}

			if((NULL!=(hItem=GetDlgItem(hdlg,IDC_PREVIEW_CHKPASSWORD)))) {
				if ((moduleVariable_bsOptions & 1))		//  Only activate for Saveas dialog
					::EnableWindow(hItem,true);
				else
					::EnableWindow(hItem,false);		// Open dialog
			}

			if((moduleVariable_bsOptions&512) && (NULL!=(hItem=GetDlgItem(hdlg,IDC_PREVIEW_CHKREADONLY)))) {
				::EnableWindow(hItem,FALSE);
			}
// EBATECH-[ Control Preview for performance issue.
			if (NULL!=(hItem=GetDlgItem(hdlg,IDC_PREVIEW_CHKPREVIEW))) {
				::EnableWindow(hItem,TRUE);
				SendMessage(hItem,BM_SETCHECK,BST_CHECKED,0);
			}
// ]-EBATECH
			return TRUE;
		}
		if(lpOfn->hdr.code==CDN_FILEOK)
		{
			HWND hWnd = GetDlgItem(hdlg,IDC_PREVIEW_IMAGE);
			if(NULL!=hWnd)
			{
				SDS_PaintPreview(NULL,NULL);
			}
			int (*pOnOkCallback)(HWND) = (int (__cdecl*)(HWND))lpOfn->lpOFN->lCustData;
			if(pOnOkCallback!=NULL)
			{
				(*pOnOkCallback)(hdlg);
			}
			

#ifdef BUILD_WITH_VBA
			if (moduleVariable_bsOptions & 1)  // We do this only when we are saving a file.
			{
				// Get the File-name Extension.
				CString strCompleteFileName (lpOfn->lpOFN->lpstrFile);
				CString strFileExtension;
				ic_getext((LPTSTR)((LPCTSTR)strCompleteFileName), (LPTSTR)((LPCTSTR)strFileExtension));

				// If the extension is "dwg", then the file with the same name and ".VBI" extension 
				// is created. Check if one already exists.
				if (strFileExtension == _T(".dwg")/*DNT*/)
				{
					// Extract the file-name with PATH, but without EXTension.
					// and Append ".vbi" to it
					int extPos = strCompleteFileName.Find (_T(".dwg"/*DNT*/));
					CString strFileNameWithVbiExt;
					strFileNameWithVbiExt = strCompleteFileName.Left (extPos) + _T(".vbi"/*DNT*/);

					// Check if this file with vbi extension exists.
					if (!_taccess((LPCTSTR)strFileNameWithVbiExt, 0))
					{
						TCHAR strMessage[IC_ACADBUF];
						
						// Get only the file-name to display.
						int lastBackSlashIndex = strFileNameWithVbiExt.ReverseFind ('\\'/*DNT*/);
						CString strJustFileName;
						strJustFileName = strFileNameWithVbiExt.Mid(lastBackSlashIndex + 1);

						::wsprintf (strMessage, 
									ResourceString(IDC_SDS_GET__VBI_FILE_EXISTS__DIALOG, "A file with the name %s already exists.\nOverwrite this File?"/*DNT*/),
									(LPCTSTR)strJustFileName
									);
						
						int idResult = ::MessageBox(::GetParent(hdlg), strMessage, "Warning"/*DNT*/, 
									 MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2);

						if (idResult == IDNO)
						{
							// Remain in Dialog Box if user says "No".
							SetWindowLong(hdlg, DWL_MSGRESULT, TRUE);
							return TRUE;
						}

					}
				}
			}
#endif
			
			
			//Modified Cybage VSB 13/11/2001 DD/MM/YYYY
			//Reason : Fix for Bug No. 77928 from Bugzilla [
			if ( strcmp(lpOfn->lpOFN->lpstrDefExt,"" ) != 0)
			{
				// Modified PK 12/07/2000 [
				TCHAR szFilePath[MAX_PATH]; // Reason : Fix for bug no. 47174
				char ext[5];
				CommDlg_OpenSave_GetSpec(GetParent(hdlg),szFilePath,sizeof(szFilePath));
				ic_getext(szFilePath, ext);
				// EBATECH(CNBR) 2002/4/24 allow multiple select
				//Modified Cybage VSB 11/02/2002 DD/MM/YYYY
				//Reason : Fix for Bug No. 77928 from Bugzilla [
				//if(SDS_CalledFromgetfiled && strcmp(ext,".") != 0 && strstr(lpOfn->lpOFN->lpstrDefExt, ext+1) == NULL && !(moduleVariable_bsOptions & 4))
				//if(SDS_CalledFromgetfiled && strcmp(ext,".") != 0 && strstr(strupr((char *)lpOfn->lpOFN->lpstrDefExt), strupr(ext+1)) == NULL && !(moduleVariable_bsOptions & 4))
				//Modified Cybage VSB 11/02/2002 DD/MM/YYYY ]
				if(SDS_CalledFromgetfiled && strcmp(ext,".") != 0 && strstr(strupr((char *)lpOfn->lpOFN->lpstrDefExt), strupr(ext+1)) == NULL && !(moduleVariable_bsOptions & 4) && !(moduleVariable_bsOptions & 32768))
				// EBATECH(CNBR) ]-
				{
					MessageBox (hdlg, ResourceString(IDS_INVALIDFILENAME,"Invalid filename"), ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD" ), MB_OK);
					SetWindowLong(hdlg, DWL_MSGRESULT, TRUE);
					return TRUE;
				}
				// Modified PK 12/07/2000 ]
			}
			//Modified Cybage VSB 13/11/2001 DD/MM/YYYY ]

		}
		if(lpOfn->hdr.code==CDN_TYPECHANGE)
		{
			TCHAR szFilePath[MAX_PATH];
			if(CommDlg_OpenSave_GetSpec(GetParent(hdlg),szFilePath,sizeof(szFilePath))>0 &&
				*szFilePath!=0)
			{
				//*** Figure out the desired extension
				DWORD idx;
				char* fcp1;
				//*** The format of lpstrFilter is
				//*** Description\0*.ext\0Description2\0*.ext\0\0
				fcp1=(char*)lpOfn->lpOFN->lpstrFilter;
				for(idx=0; idx<lpOfn->lpOFN->nFilterIndex && *fcp1!=0; idx++)
				{
					if(idx>0)
					{
						//*** Walk fcp1 over the extension.
						while(*fcp1!=0)	fcp1++;
						fcp1++;
					}
					//*** Walk fcp1 over the description.
					while(*fcp1!=0) fcp1++;
					fcp1++;
				}
				if(*fcp1==0)
					return FALSE;

				if (idx == 1)	
				{				
					HWND hItem;
					if((NULL!=(hItem=GetDlgItem(hdlg,IDC_PREVIEW_CHKPASSWORD)))) 
						::EnableWindow(hItem,TRUE);
				}
				else
				{
					HWND hItem;
					if((NULL!=(hItem=GetDlgItem(hdlg,IDC_PREVIEW_CHKPASSWORD)))) 
						::EnableWindow(hItem,FALSE);
				}
				//*** Step over the *. characters.
				fcp1+=2;
				//*** Set the extension and string to the editbox
				ic_setext(szFilePath,fcp1);
				CommDlg_OpenSave_SetControlText(GetParent(hdlg),edt1,szFilePath);
			}
		}
		if(lpOfn->hdr.code==CDN_SELCHANGE)
		{
			HWND hWnd=GetDlgItem(hdlg,IDC_PREVIEW_IMAGE);
			if(NULL!=hWnd)
			{
				SDS_PaintPreview(NULL,NULL);
				TCHAR szFilePath[MAX_PATH];
				if(CommDlg_OpenSave_GetSpec(GetParent(hdlg),szFilePath,sizeof(szFilePath))>0 && 0!=(*szFilePath) &&
					CommDlg_OpenSave_GetFilePath(GetParent(hdlg),szFilePath,sizeof(szFilePath))>0)
				{
					if(0==access(szFilePath,00))
					{
						if(0!=sstrPreviousFilePath.Compare(szFilePath))
						{
							sstrPreviousFilePath=szFilePath;
							//*** Get the file information
							CString str;
							CFileStatus status;
							CFile fileCur;
							CFileException fileException;
							BOOL bWritable = FALSE;

							//*** Try and open the file read/write
							if(fileCur.Open(szFilePath,CFile::modeReadWrite | CFile::shareDenyNone,&fileException))
							{
								bWritable=TRUE;
							}
							//*** If read/write failed try read only.
							if(bWritable ||
								fileCur.Open(szFilePath,CFile::modeRead | CFile::shareDenyNone,&fileException))
							{
								fileCur.GetStatus(status);
								HWND hStatic = GetDlgItem(hdlg,IDC_PREVIEW_FILESIZE);
								str.Format("%d bytes"/*DNT*/,status.m_size);
								SetWindowText(hStatic,str);

//CREATION
								maketimestring(&status.m_ctime,&str);
								hStatic = GetDlgItem(hdlg,IDC_PREVIEW_CREATIONTIME);
								SetWindowText(hStatic,str);

//MODIFICATION
								maketimestring(&status.m_mtime,&str);
								hStatic = GetDlgItem(hdlg,IDC_PREVIEW_MODIFICATIONTIME);
								SetWindowText(hStatic,str);

//ACCESS
								CTime tm = CTime::GetCurrentTime();
								maketimestring(&tm,&str);
								hStatic = GetDlgItem(hdlg,IDC_PREVIEW_ACCESSTIME);
								SetWindowText(hStatic,str);

								//*** Set the read-only check if necessary.
								if(!(moduleVariable_bsOptions & 512)) {
									HWND hButton = GetDlgItem(hdlg,IDC_PREVIEW_CHKREADONLY);
//									if(!bWritable)
//									{
//										::SendMessage(hButton,BM_SETCHECK,BST_CHECKED,0);
//										::EnableWindow(hButton,FALSE);
//									}
//									else
//									{
										::EnableWindow(hButton,TRUE);
//										::SendMessage(hButton,BM_SETCHECK,BST_UNCHECKED,0);
//									}
								}
								//*** Paint the drawing image into the preview window.
								// EBATECH(CNBR) -[ 2002/5/6 Control Preview for paformance issue.
								// SDS_PaintPreview(szFilePath,hWnd);
								if( BST_CHECKED == IsDlgButtonChecked(hdlg, IDC_PREVIEW_CHKPREVIEW)){
									SDS_PaintPreview(szFilePath, hWnd);
								}
								// ]-EBATECH
							}
						}
					}
					else
					{
						sstrPreviousFilePath.Empty();
						HWND hStatic = GetDlgItem(hdlg,IDC_PREVIEW_FILESIZE);
						SetWindowText(hStatic,""/*DNT*/);
						hStatic = GetDlgItem(hdlg,IDC_PREVIEW_CREATIONTIME);
						SetWindowText(hStatic,""/*DNT*/);
						hStatic = GetDlgItem(hdlg,IDC_PREVIEW_MODIFICATIONTIME);
						SetWindowText(hStatic,""/*DNT*/);
						hStatic = GetDlgItem(hdlg,IDC_PREVIEW_ACCESSTIME);
						SetWindowText(hStatic,""/*DNT*/);
					}
				}
			}
		}
	}
	// EBATECH(CNBR) -[ 2002/5/6 Control Preview for performance issue.
	if( uiMsg == WM_COMMAND ){
		if(HIWORD(wParam) == BN_CLICKED && LOWORD(wParam) == IDC_PREVIEW_CHKPREVIEW ){
			HWND hWnd=GetDlgItem(hdlg,IDC_PREVIEW_IMAGE);
			if(NULL!=hWnd){
				if( BST_UNCHECKED == IsDlgButtonChecked(hdlg, IDC_PREVIEW_CHKPREVIEW)){
					InvalidateRect(hWnd,NULL,FALSE);
				}else{
					TCHAR szFilePath[MAX_PATH];
					if(CommDlg_OpenSave_GetSpec(GetParent(hdlg),szFilePath,sizeof(szFilePath))>0 && 0!=(*szFilePath) &&
						CommDlg_OpenSave_GetFilePath(GetParent(hdlg),szFilePath,sizeof(szFilePath))>0)
					{
						SDS_PaintPreview(szFilePath,hWnd);
					}
				}
			}
			return TRUE;
		}

		if(HIWORD(wParam) == BN_CLICKED && LOWORD(wParam) == IDC_PREVIEW_CHKPASSWORD )
		{
			if( BST_CHECKED == IsDlgButtonChecked(hdlg, IDC_PREVIEW_CHKPASSWORD))
			{
				cmd_bSaveWithPassWord = TRUE;
			}
			else
			{
				cmd_bSaveWithPassWord = FALSE;
			}

			return TRUE;
		}
	}
	// EBATECH(CNBR)
	return FALSE;
}

UINT APIENTRY sds_getfiledHookProcOldStyle(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	if(uiMsg==WM_INITDIALOG)
	{
		CWnd* pWnd = new CWnd();
		pWnd->Attach(hdlg);
		pWnd->CenterWindow(SDS_CMainWindow);
		pWnd->Detach();
		delete pWnd;
		return TRUE;
	}
	return FALSE;
}

// *****************************************
// This is an SDS External API
//
int
sdsengine_getcorner( const sds_point ptStart, const char *szCornerMsg, sds_point ptOpposite, bool bApplySnapping, bool bDirectCall )
{
    int ret = RTNORM;

	COsnapLocker protector( false );
	if( !bApplySnapping )
		protector.initialize();

	ASSERT( OnEngineThread() );

	if( !SDSApplication::GetEngineApplication()->GetMainThread()->IsCurrent() && bDirectCall && SDS_AtCmdLine )
	{
		// redirect call to the Engine posting a job there to prevent deadlock
		CIcadDoc *pDoc = SDS_CURDOC;
		if( !pDoc )
			return RTERROR;

		HRESULT hr = pDoc->AUTO_getcorner( ptStart, szCornerMsg, ptOpposite, bApplySnapping );
		if( FAILED( hr ) )
			ret = RTERROR;

		return ret;
	}

    SDS_GetCornerMode=1;
	// Modified CyberAge PP 09/22/2000 [ Reason: Fix for bug no. ITC 57514
	// ret=internalGetpoint((double *)ptStart,szCornerMsg,ptOpposite);
	ret=internalGetpoint((double *)ptStart,szCornerMsg,ptOpposite,FALSE);
	// Modified CyberAge PP 09/22/2000 ]

    SDS_GetCornerMode=0;

    sds_initget(0,""/*DNT*/); // Reset sds_initget()


    if(ret!=RTKWORD)
	{
		*SDSApplication::GetActiveApplication()->thekeyword=0;
	}
    return(ret);
}

// *****************************************
// This is an SDS External API
//
int sdsengine_getdist(const sds_point ptStart, const char *szDistMsg, double *pdDistance)
	{
	ASSERT( OnEngineThread() );

    return(SDS_GetAngDist(ptStart,szDistMsg,pdDistance,IC_GETANGDIST_GETDIST));
	}

// *****************************************
// This is an SDS External API
//
int
sdsengine_getangle(const sds_point ptStart, const char *szAngleMsg, double *pdRadians)
	{

	ASSERT( OnEngineThread() );

    return(SDS_GetAngDist(ptStart,szAngleMsg,pdRadians,IC_GETANGDIST_GETANGLE));
	}

int SDS_getangleneg(const sds_point ptStart, const char *szAngleMsg, double *pdRadians) {
    return(SDS_GetAngDist(ptStart,szAngleMsg,pdRadians,IC_GETANGDIST_GETANGLE_NEGOK_NORM));
}

int SDS_getanglebig(const sds_point ptStart, const char *szAngleMsg, double *pdRadians) {
    return(SDS_GetAngDist(ptStart,szAngleMsg,pdRadians,IC_GETANGDIST_GETANGLE_NEGOK_NONORM));
}

// *****************************************
// This is an SDS External API
//
int
sdsengine_getorient(const sds_point ptStart, const char *szOrientMsg, double *pdRadians)
	{

	ASSERT( OnEngineThread() );

    return(SDS_GetAngDist(ptStart,szOrientMsg,pdRadians,IC_GETANGDIST_GETORIENT));
	}

int SDS_getorientneg(const sds_point ptStart, const char *szOrientMsg, double *pdRadians) {
    return(SDS_GetAngDist(ptStart,szOrientMsg,pdRadians,IC_GETANGDIST_GETORIENT_NEGOK_NORM));
}

int SDS_getorientbig(const sds_point ptStart, const char *szOrientMsg, double *pdRadians) {
    return(SDS_GetAngDist(ptStart,szOrientMsg,pdRadians,IC_GETANGDIST_GETORIENT_NEGOK_NONORM));
}

int SDS_GetAngDist(const sds_point ptStart, const char *szMsg, double *pdResult, getangdist_mode nMode) {

    // modes are
    //  0 = getdist
    //  1 = getangle
    //  2 = getorient
    //  3 = getangle, preserving negative input allowed from keyboard to return neg value(-2pi to +2pi)
    //  4 = getorient, preserving negative input allowed from keyboard to return neg value(-2pi to +2pi)
	//	5 = getangle big, preserving sign AND size (no normalizing to -2pi to +2pi)
	//	6 = getorient big, preserving sign AND size (no normalizing to -2pi to +2pi)

	//NOTE: This f'n preserves LASTPOINT, resetting it to its former value on exit.

	ASSERT( OnEngineThread() );


    int ret,bitsets,gotpt=0,angct,ret2;
    char fs1[IC_ACADBUF],kword[IC_ACADBUF],*fcp1;
	sds_real fr1;
    sds_point pt1,pt2,lastpoint;
	struct resbuf rb;

	SDS_getvar(NULL,DB_QLASTPOINT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	ic_ptcpy(lastpoint,rb.resval.rpoint);
	SDS_AtAngDist=1;
	if(ptStart) {
		ic_ptcpy(SDS_PointDisplay,ptStart);
		extern int SDS_GetZScale;
		if(SDS_GetZScale) {
			SDS_PointDisplayMode=4;
		} else {
			if(nMode==IC_GETANGDIST_GETDIST) SDS_PointDisplayMode=2;
			else        SDS_PointDisplayMode=1;
		}
	}

    if(ptStart) {
        gotpt=1;  ic_ptcpy(pt2,ptStart);
    }

	CommandQueue SaveCommandQueue;
	CommandQueue SaveMenuQueue;

    if(	GetActiveCommandQueue()->IsCommandPause() )
		{
		GetActiveCommandQueue()->AdvanceNext();
		SaveCommandQueue.Copy( GetActiveCommandQueue() );
		GetActiveCommandQueue()->Flush();
    }
    if( GetMenuQueue()->IsCommandPause())
		{
		GetMenuQueue()->AdvanceNext();
		SaveMenuQueue.Copy( GetMenuQueue() );
		GetMenuQueue()->Flush();
	    }

    for(;;) {
        //*** Save the bitsets here because getpoint clears them.
        bitsets=SDSApplication::GetActiveApplication()->GetInputControlBits();
        strncpy(kword,SDSApplication::GetActiveApplication()->GetKeywords(),sizeof(kword)-1);
        // Make sure getpoint will take ANY input.
        SDSApplication::GetActiveApplication()->SetInputControlBits( bitsets | SDS_RSG_OTHER );

		if(gotpt) {
			if(nMode==IC_GETANGDIST_GETDIST) SDS_PointDisplayMode=2;
			else         SDS_PointDisplayMode=1;
		}
// Modified Cybage PP 16/03/2001 [dd/mm/yyyy]
// Reason: Fix for bug # 67790
ASKFORPOINT :
		ret=SDS_AskForPoint( (ptStart ? ptStart : (gotpt ? pt2 : NULL)),
                            ((gotpt && ptStart==NULL) ? ResourceString(IDC_SDS_GET__NSECOND_POINT___3, "\nSecond point: " ) : szMsg),pt1 );

		// Modified Cybage PP 16/03/2001[
		// Reason: Fix for bug # 67790
		extern bAskPointForArray ;
		if( bAskPointForArray && ret == RTNORM && pt2[0] == pt1[0] && pt2[1] == pt1[1] && pt2[2] == pt1[2] )
		{
			sds_printf( ResourceString(IDC_ERRORPROMPT_POSITIVE_VALUE_303, "Value must be positive and nonzero.") ) ;
			goto ASKFORPOINT ;
		}
		// Modified Cybage PP 16/03/2001 ]

        sds_initget(bitsets,kword);

        switch(ret) {
            case RTNORM:
                if(gotpt) {
                    gotpt=0;
                    switch(nMode) {
                        case IC_GETANGDIST_GETDIST:
                            if(SDSApplication::GetActiveApplication()->GetInputControlBits() & SDS_RSG_2D)
								{
								pt1[2]=pt2[2];
								}
                            *pdResult=sds_distance(pt2,pt1);
                            break;
                        case IC_GETANGDIST_GETANGLE:
						case IC_GETANGDIST_GETANGLE_NEGOK_NORM:
						case IC_GETANGDIST_GETANGLE_NEGOK_NONORM:
                            *pdResult=sds_angle(pt2,pt1);
							// angle was wrong in case CW direction
							// The following two lines breaks (getangle)
							// functionality. So I have commented them
							// out. This is very late in the I5 release
							// Like the day of the release, so we may
							// regret this?? MHB
							//if((SDS_getvar(NULL, DB_QANGDIR, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES) == RTNORM ? rb.resval.rint : 0))
							//	*pdResult = IC_TWOPI - *pdResult;
							
							// angbase should be taken acoount
							SDS_getvar(NULL,DB_QANGBASE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
                            *pdResult=*pdResult-rb.resval.rreal;

							//if user picked 2 pts, normalize it regardless
							ic_normang(pdResult,NULL);
                            break;
                        case IC_GETANGDIST_GETORIENT:
						case IC_GETANGDIST_GETORIENT_NEGOK_NORM:
						case IC_GETANGDIST_GETORIENT_NEGOK_NONORM:
                            *pdResult=sds_angle(pt2,pt1);
                            break;
                    }
                    if(icadRealEqual(*pdResult,0.0) &&
						(SDSApplication::GetActiveApplication()->GetInputControlBits() & SDS_RSG_NOZERO) ) {
						cmd_ErrorPrompt(CMD_ERR_NONZERO,0);
                        break;
                    }
                    if(*pdResult<0.0 &&
						(SDSApplication::GetActiveApplication()->GetInputControlBits() & SDS_RSG_NONEG) ) {
						cmd_ErrorPrompt(CMD_ERR_POSORZERO,0);
                        break;
                    }
                    goto out;
                }
                gotpt=1;
                ic_ptcpy(pt2,pt1);
                break;
            case RTSTR:
            case RTKWORD:
                sds_getinput(fs1);
				fr1=0.0;
				if(*fs1=='<') {
					ret2=RTNORM;
				} else {
					if (nMode==IC_GETANGDIST_GETDIST) ret2=sds_distof(fs1,-1,&fr1);
					else{
   						if (nMode==IC_GETANGDIST_GETANGLE) ret2=SDS_angtof_convert(1,fs1,-1,&fr1);
						else if (nMode==IC_GETANGDIST_GETORIENT) ret2=SDS_angtof_convert(0,fs1,-1,&fr1);
						//progesoft
						//else ret2=SDS_angtof_convert(2,fs1,-1,&fr1);
						else if ((nMode==IC_GETANGDIST_GETORIENT_NEGOK_NORM)||(nMode==IC_GETANGDIST_GETORIENT_NEGOK_NONORM)) ret2=SDS_angtof_convert(7,fs1,-1,&fr1);
						else if ((nMode==IC_GETANGDIST_GETANGLE_NEGOK_NORM)||(nMode==IC_GETANGDIST_GETANGLE_NEGOK_NONORM)) ret2=SDS_angtof_convert(2,fs1,-1,&fr1);
						//end progesoft
					}
				}

				if(ret2==RTNORM && *fs1 && (sds_isdigit((unsigned char) *fs1) || *fs1=='-' || *fs1=='.' || *fs1=='<' || !icadRealEqual(fr1,0.0)) ) {
                    switch(nMode) {
                        case IC_GETANGDIST_GETDIST:
                            sds_distof(fs1,-1,pdResult);
                            break;
                        default:

                            //***           <   - Use Current angular units and Current angle base and direction.
                            //***           <<  - Use Degrees angular units and Default (east,ccw) angle base
                            //***                 and direction.
                            //***           <<< - Use Current angular units and Default (east,ccw) angle base
                            //***                 and direction.
                            if(*fs1=='<') {
                                for(fcp1=fs1,angct=0; *fcp1=='<'; fcp1++,angct++);
                                strncpy(fs1,fcp1,sizeof(fs1)-1);
								if(angct==1)
									sds_angtof(fs1,-1,&fr1);
								else if(angct==2)
									SDS_angtof_convert(1,fs1,0,&fr1);//getorient
								else if(angct==3){
									SDS_angtof_convert(1,fs1,-1,&fr1);//getorient
								}
                            } else {
								if (nMode==IC_GETANGDIST_GETANGLE) SDS_angtof_convert(1,fs1,-1,&fr1);
								else if (nMode==IC_GETANGDIST_GETORIENT) SDS_angtof_convert(0,fs1,-1,&fr1);
								//progesoft
								//else ret2=SDS_angtof_convert(2,fs1,-1,&fr1);
								else if ((nMode==IC_GETANGDIST_GETORIENT_NEGOK_NORM)||(nMode==IC_GETANGDIST_GETORIENT_NEGOK_NONORM)) ret2=SDS_angtof_convert(7,fs1,-1,&fr1);
								else if ((nMode==IC_GETANGDIST_GETANGLE_NEGOK_NORM)||(nMode==IC_GETANGDIST_GETANGLE_NEGOK_NONORM)) ret2=SDS_angtof_convert(2,fs1,-1,&fr1);
								//end progesoft
								//sds_angtof(fs1,-1,pdResult);
							}
							/*if(angct>1){
								SDS_getvar(NULL,DB_QANGDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
								if(1==rb.resval.rint){
									SDS_getvar(NULL,DB_QANGBASE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
									*pdResult=rb.resval.rreal-*pdResult;
								}else{
									SDS_getvar(NULL,DB_QANGBASE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
									*pdResult-=rb.resval.rreal;
								}
								ic_normang(pdResult,NULL);
							}*/
							if(nMode==IC_GETANGDIST_GETANGLE_NEGOK_NORM || nMode==IC_GETANGDIST_GETORIENT_NEGOK_NORM){
								//make sure the result has absolute value<=IC_TWOPI
								if(fr1<0.0){
									for(;fr1<=-IC_TWOPI;fr1+=IC_TWOPI);
								}else{
									for(;fr1>=IC_TWOPI;fr1-=IC_TWOPI);
								}
							}
							*pdResult=fr1;
							break;
                    }
                    if(icadRealEqual(*pdResult,0.0) &&
						(SDSApplication::GetActiveApplication()->GetInputControlBits() & SDS_RSG_NOZERO) ) {
						if(SDSApplication::GetActiveApplication()->GetInputControlBits() & SDS_RSG_NONEG){
                            cmd_ErrorPrompt(CMD_ERR_POSITIVE,0);
						}else{
                            cmd_ErrorPrompt(CMD_ERR_NONZERO,0);
                        }
                        break;
                    }
                    if(*pdResult<0.0 &&
						(SDSApplication::GetActiveApplication()->GetInputControlBits() & SDS_RSG_NONEG) ) {
						if(SDSApplication::GetActiveApplication()->GetInputControlBits() & SDS_RSG_NOZERO){
                            cmd_ErrorPrompt(CMD_ERR_POSITIVE,0);
                        }else{
                            cmd_ErrorPrompt(CMD_ERR_POSORZERO,0);
                        }
                        break;
                    }
                    ret=RTNORM; goto out;
                }
                if(RTKWORD==(ret2=SDS_SetKeyWord(fs1))) { ret=RTKWORD; goto out; }
                if(ret2!=RTFAIL){
					if (nMode==IC_GETANGDIST_GETDIST) cmd_ErrorPrompt(CMD_ERR_UNRECOGENTRY,0);
					else cmd_ErrorPrompt(CMD_ERR_ANGLE,0);
				}
                break;
            case RTNONE:
                if(!(SDSApplication::GetActiveApplication()->GetInputControlBits() & SDS_RSG_NONULL))
					{
					goto out;
					}
				cmd_ErrorPrompt(CMD_ERR_NUMERIC,0);
            case RTCAN:
            case RTERROR:
                goto out;
        }
    }
    out: ;

	if(ret==RTCAN)
		{
		SaveMenuQueue.Flush();
		SaveCommandQueue.Flush();
		}

	if ( SaveCommandQueue.IsNotEmpty() )
		{
		GetActiveCommandQueue()->Copy( &SaveCommandQueue );
		}

	if( SaveMenuQueue.IsNotEmpty() )
		{
		GetMenuQueue()->Copy( &SaveMenuQueue );
		}

	if(ret==RTCAN)
		{
		GetActiveCommandQueue()->Flush();
		}

    sds_initget(0,""/*DNT*/); // Reset sds_initget()
    if(ret!=RTKWORD) *SDSApplication::GetActiveApplication()->thekeyword=0;
	SDS_PointDisplayMode=0;
	SDS_AtAngDist=0;

	rb.restype=RT3DPOINT;
	ic_ptcpy(rb.resval.rpoint,lastpoint);
	SDS_setvar(NULL,DB_QLASTPOINT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);

    return(ret);
}

// *****************************************
// This is an SDS External API
//
// TODOTHREAD -- must be changed to no depend on LoadedAppsAct
//
// engine call???
//
int
sdsengine_getinput(char *szEntry)
	{
    strcpy(szEntry,SDSApplication::GetActiveApplication()->thekeyword);


	// TODO Need to return RTERROR, RTKWORD, or RTSTR too.
	// I think... we are counting on RTNORM in the cmds?.cpp files.
    return(RTNORM);
	}

// *****************************************
// This is an SDS External API
//
int
sdsengine_getint(const char *szIntMsg, int *pnInteger)
	{

	ASSERT( OnEngineThread() );

    int ret;

    ret=SDS_GetNumber(szIntMsg,pnInteger,NULL);

	// Modified PK 31/05/2000
	calledfrom_cmd_setvar = 0;	// Reason : Fix for bug no. 74219

    sds_initget(0,""/*DNT*/); // Reset sds_initget()

    return(ret);
	}

// *****************************************
// This is an SDS External API
//
int
sdsengine_getreal(const char *szRealMsg, double *pdReal)
	{
	ASSERT( OnEngineThread() );

    int ret;

    ret=SDS_GetNumber(szRealMsg,NULL,pdReal);

    sds_initget(0,""/*DNT*/); // Reset sds_initget()

    return(ret);
	}



int
SDS_GetNumber(const char *prompt, int *resint, sds_real *resreal)
	{

	ASSERT( OnEngineThread() );

    //Modified SAU 17/05/00
    //int ret,ret2,nomsg;
	int ret,ret2,nomsg,nokeyword;
    char fs1[IC_ACADBUF];
    sds_real fr1;
	char *cptr;

    if((!resint && !resreal) || (resint && resreal)) return(RTERROR);

	CommandQueue SaveCommandQueue;
	CommandQueue SaveMenuQueue;



    if( GetActiveCommandQueue()->IsCommandPause() )
		{
		GetActiveCommandQueue()->AdvanceNext();
		SaveCommandQueue.Copy( GetActiveCommandQueue() );
		GetActiveCommandQueue()->Flush();
	    }
    if( GetMenuQueue()->IsCommandPause())
		{
		GetMenuQueue()->AdvanceNext();
		//EBATECH(FUTA) 2001-8-11 fix Menu Macro
		//SaveMenuQueue.Copy( GetActiveCommandQueue() );
		SaveMenuQueue.Copy( GetMenuQueue() );
		GetMenuQueue()->Flush();
	    }

    for(;;) {
        //Modified SAU 17/05/00
        //nomsg=0;
		nomsg=nokeyword=0;

        if(RTCAN==(ret=sds_getstring(0,prompt,fs1))) goto out;
        if(ret==RTERROR) goto out;
        if(*fs1==0)
			{
            if(SDSApplication::GetActiveApplication()->GetInputControlBits() & SDS_RSG_NONULL)
				{
				goto invalid;
				}
            else
				{
				ret=RTNONE;
				goto out;
				}
	        }

        if(resint)
		{
			//Modified SAU 17/05/00
			if (!(SDSApplication::GetActiveApplication()->GetInputControlBits() & SDS_RSG_OTHER))
			{
				//Modified SAU 29/04/00  [
				//Reason:(Bug Fixed)REGRESSION:(getint) doesn't
				//		  handle numbers with commas correctly.
				//if(strchr(fs1,'.')) {
				//Modified PK 31/05/2000 [
				if(strchr(fs1,'.') && !calledfrom_cmd_setvar)
				{
					// Reason : Fix for bug no. 74219
					cmd_ErrorPrompt(CMD_ERR_INTRANGE,0);
					nomsg=1;
					goto invalid;
				}
				//Modified PK 31/05/2000 ]

				for(int i=0;fs1[i]!='\0';i++)
				{
					if(fs1[0]=='-') continue;
					if(!isdigit((unsigned char) fs1[i]))
					{
						//Modified SAU 29/04/00  ]
						//Modified PK 31/05/2000 [
						//cmd_ErrorPrompt(CMD_ERR_INTRANGE,0); Reason : Fix for bug no. 74219
						//nomsg=1;
						//Modified PK 31/05/2000 ]
					goto invalid;
					}
				}

				fr1=atof(fs1);

				if(fr1>32767 || fr1<-32768)
				{
					cmd_ErrorPrompt(CMD_ERR_INTRANGE,0);
					nomsg=1;
					goto invalid;
				}

				fr1=atoi(fs1);
			}
			//Modified SAU 17/05/00   [
			//Reason:Bug fixed(47181)(getint) + (initget whatever-you-want) = incorrect answer
			else
			{
				if(ic_isnumstr(fs1,&fr1) &&
					(fr1!=HUGE_VAL && fr1!=HUGE_VAL && fr1<32767 && fr1>-32768))
					fr1=atoi(fs1);
				else
					goto invalid;
			}
           	//Modified SAU 17/05/00   ]
		}
        if(resreal){
			ret2=sds_distof(fs1,-1,&fr1);  //added for bug
            //Modified SAU 17/05/00
			//if(ret2!=RTNORM && !(SDSApplication::GetActiveApplication()->GetInputControlBits() & SDS_RSG_OTHER))
			if(ret2!=RTNORM)
				{
				goto invalid;
				}
			// The line above had been commented out, and no-one remembers why.  With it commented out,
			// sds_getreal() would not return RTKWORD, just RTNORM.   8 May, 1997
		}

        if(ret==RTNORM && icadRealEqual(fr1,0.0) && *fs1!='0') goto invalid;
        if((SDSApplication::GetActiveApplication()->GetInputControlBits() & SDS_RSG_NOZERO) &&
			ret==RTNORM &&
			icadRealEqual(fr1,0.0))
			{
			cmd_ErrorPrompt(CMD_ERR_NONZERO,0);
			//Modified SAU 17/05/00
			//nomsg=1;
			nomsg=nokeyword=1;
			goto invalid;
			}
        if((SDSApplication::GetActiveApplication()->GetInputControlBits() & SDS_RSG_NONEG) &&
			ret==RTNORM &&
			fr1<0.0)
			{
			if(SDSApplication::GetActiveApplication()->GetInputControlBits() & SDS_RSG_NOZERO)
				{
				cmd_ErrorPrompt(CMD_ERR_POSITIVE,0);
				}
			else
				{
				cmd_ErrorPrompt(CMD_ERR_POSORZERO,0);
				}
			//Modified SAU 17/05/00
			//nomsg=1;
			nomsg=nokeyword=1;
			goto invalid;
			}
        if(resint && strchr(fs1,'.')) goto invalid;

        if(resint)  *resint=atoi(fs1);
        if(resreal) {
			ret2=sds_distof(fs1,-1,resreal);
			if(ret2!=RTNORM &&
				!(SDSApplication::GetActiveApplication()->GetInputControlBits() & SDS_RSG_OTHER))
				{
				goto invalid;
				}
			// The line above had been commented out, and no-one remembers why.  With it commented out,
			// sds_getreal() would not return RTKWORD, just RTNORM.    8 May, 1997
		}

        goto out;

        invalid: ;
        if (ret2 != RTREJ)
            if(ret==RTNORM && RTKWORD==(ret2=SDS_SetKeyWord(fs1))) {
				//Modified SAU 17/05/00
				//{ ret=RTKWORD;  goto out;}
				if(!nokeyword){ ret=RTKWORD;  goto out;}
		    }
			//Modified PK 31/05/2000
		    //else if (resint) {	// 1-57629
		    else if (resint && calledfrom_cmd_setvar) {	// Reason : Fix for bug no. 74219
			    cptr=fs1;		// make sure we have only legal characters here,
							    // because atof/atoi stop at illegal ones rather than erroring
			    if ((*cptr)=='+' || (*cptr)=='-') cptr++;
			    while (*cptr) {
				    if ((*cptr)<'0' || (*cptr)>'9') {
					    cmd_ErrorPrompt(CMD_ERR_SYSVARVAL,0);
					    nomsg=1;
					    break;
				    }
				    cptr++;
			    }
		    }

        if(ret2!=RTFAIL && !nomsg)
			cmd_ErrorPrompt(CMD_ERR_UNRECOGENTRY,0);
    }
    if(ret!=RTKWORD) *SDSApplication::GetActiveApplication()->thekeyword=0;

	out: ;

	if(ret==RTCAN)
		{
		SaveCommandQueue.Flush();
		SaveMenuQueue.Flush();
		}

	if ( SaveCommandQueue.IsNotEmpty() )
		{
		GetActiveCommandQueue()->Copy( &SaveCommandQueue );
		}
	if(SaveMenuQueue.IsNotEmpty())
		{
		GetMenuQueue()->Copy( &SaveMenuQueue );
		}

    return(ret);
}

int SDS_SetKeyWord(const char *str) {
    char *cp1;
    int  index = -1;
    bool hasglobalKwords = FALSE;
	CString fs1, fs2, fs3, globalKwords;

    SDS_ArbInputFlag=0;

	fs1 = SDSApplication::GetActiveApplication()->GetKeywords();

	char *input = (char *) str;

	// Martin Waardenburg, 7 April 2000:
	// It is valid to have an input string that exactly matches a global string.
	// In order to have this work, we prepend an underscore and compare the prepended string to list of global strings.
	// This is necessary to fix a problem with translated versions of IntelliCAD where the command options for the
	// polyline command don't work. Somehow SDS_SetKeyWord is called twice with polyline, and maybe also other commands.
	// This causes exactly the right global string to be passed in here as input. Therefore it should be valid to have
	// exactly the global string as input.
	char inputPrependedWithUnderscore[IC_ACADBUF];

    if (UNDERSCORECHR != input[0])
	{
		inputPrependedWithUnderscore[0] = UNDERSCORECHR;
		inputPrependedWithUnderscore[1] = '\0';
	}
	else
		inputPrependedWithUnderscore[0] = '\0';


	strcat(inputPrependedWithUnderscore, input);

    // Bug fix 1-47165. If initget is given a string like
    // "One Two Three _1 _2 _3" everything to the right of the first underscore is
    // understood to be the globalized side of things. So if a user types in One,
    // initget should return 1, Two yields 2 etc. The '~' is unique to icad and allow
    // things to be hidden from the user both on the command line and the popup menus. The
	// tilde alone on the 'localized' side of the string is treated as a menu separater.
    // Here we are testing to see if there is a localized side to the string being passed in
    // and if so we are setting a flag and saving off the global keywords.
    TCHAR *pglobalKwords1 = strstr(fs1, SPACETILDEUNDERSCORE); // Find first of each.
    TCHAR *pglobalKwords2 = strstr(fs1, SPACEUNDERSCORE);

    if (pglobalKwords1 == NULL && pglobalKwords2 == NULL && (*fs1 == TILDECHR || *fs1 == UNDERSCORECHR))
        pglobalKwords1 = fs1.GetBuffer(fs1.GetLength());

    if (pglobalKwords1 != NULL || pglobalKwords2 != NULL) {
        TCHAR *pFirstUnderscored = NULL;
        // I know, I know, but I went brain dead. There must be a better way, but
        // I know I have all the situations covered here.
        if (pglobalKwords1 == NULL && pglobalKwords2 != NULL)
            pFirstUnderscored = pglobalKwords2;
        if (pglobalKwords2 == NULL && pglobalKwords1 != NULL)
            pFirstUnderscored = pglobalKwords1;
        if (pglobalKwords2 != NULL && pglobalKwords1 != NULL)
            pFirstUnderscored = (pglobalKwords1 < pglobalKwords2) ? pglobalKwords1 : pglobalKwords2;

        hasglobalKwords = TRUE;
		globalKwords = pFirstUnderscored + sizeof(TCHAR) * 1;

        // This part is to fix a problem that ArchT was having. As it turns out, acad's initget
        // NEVER returns an underscored string form, for example sds_getkword. They always strip
        // the underscore. However, at some point they prepend underscores to the global part of
        // initget strings. So the even something like "Aaa Bbb Ccc _Yes No Maybe" would still
        // treat 'No' and 'Maybe' as if it had a leading underscore. So here we pull the initget
        // string apart and prepend underscores to the strings to the right of the first ~_ or _
        // and rebuild the incomming string accordingly. In addition we now have a string 'globalKwords'
        // that only contain the 'globalized' form of the keywords.
        CString tmpGlobals = globalKwords;
        cp1 = _tcstok(tmpGlobals.GetBuffer(tmpGlobals.GetLength() + 1), SPACE);
        CString tmpWorkBuf;
        int firsttime = 1;
        while(cp1) {
            if (!firsttime) tmpWorkBuf += SPACE;

            if (strisame(cp1, TILEDUNDERSCORE)) {	// If it is '~_' and only '~_
                tmpWorkBuf += cp1;
            }else if (*cp1 == TILDECHR) {			// If it starts with a '~'
                if (*(cp1+1) == UNDERSCORECHR)
                    tmpWorkBuf += cp1;
                else {
                    tmpWorkBuf += TILEDUNDERSCORE;
                    tmpWorkBuf += cp1 + 1;
                }
            } else if (*cp1 == UNDERSCORECHR) {		// If it starts with a '_'
                tmpWorkBuf += TILDE;
                tmpWorkBuf += cp1;
            } else {								// None of the above.
                tmpWorkBuf += TILEDUNDERSCORE;
                tmpWorkBuf += cp1;
            }

            firsttime = 0;

            cp1 = _tcstok(NULL, SPACE);
        }

        // A little cleanup.
        pglobalKwords1 = pglobalKwords2 = NULL;
        globalKwords = tmpWorkBuf;

        // Now we build tmpWorkBuf filling it with the 'localized' portion, then appending
        // the processed 'globalized' part to it then assiging it back to fs1.
        tmpWorkBuf.Empty();
        firsttime = 1;
        cp1 = _tcstok(fs1.GetBuffer(fs1.GetLength() + 1), SPACE);
        while(cp1 != NULL && *cp1 != UNDERSCORECHR && !strnisame(cp1, TILEDUNDERSCORE, 2)) {
            tmpWorkBuf += firsttime ? EMPTYSTR : SPACE;
            tmpWorkBuf += cp1;
            firsttime = 0;
            cp1 = _tcstok(NULL, SPACE);
        }

		// Now repopulate fs1 with the doctor'd string.
        fs1.Empty();
        fs1 = tmpWorkBuf;
        fs1 += SPACE;
        fs1 += globalKwords; // Now fs1 is correct.

		globalKwords.Empty();
    }
    struct kword *keywordList = NULL;
    if (*fs1 != 0) {
        splitIntoKeywordList(fs1, &keywordList);
    }
    // If there aren't and globalized keywords, and the user entered something with
    // a leading underscore, remove it before looking for the keyword.
    if(!hasglobalKwords && *input == UNDERSCORECHR)
        input += 1 * sizeof(TCHAR);

    struct kword *walker = keywordList;
    int count = 0, match = 0, matchIndex = -1;

    if (*fs1 != 0 && *input != 0)
	{
        while (walker != NULL)
		{
			// The wholeKey matches input up to the length of the input. && All of leftWithCaps matches input.
			CString leftPart = walker->wholeKey.Left(_tcslen(input));
			bool subStrMatch = leftPart.CompareNoCase(input) == 0 &&
								//Bugzilla No. 77915 ; 15-04-2002
								(walker->leftWithCaps.GetLength() > 0) &&
							   _tcsnicmp(walker->leftWithCaps, input, walker->leftWithCaps.GetLength()) == 0;

			// Input matches capsOnly exactly (no case), or
			// Input matches leftWithCaps completely, or
			// Input matches the wholeKey completely, or
			// Input prepended with _ matches the wholeKey completely (case sensitive)
			// No case for ambiguity.
            if (!walker->capsOnly.CompareNoCase(input) ||
				!walker->leftWithCaps.CompareNoCase(input) ||
				!walker->wholeKey.CompareNoCase(input) ||
				!walker->wholeKey.Compare(inputPrependedWithUnderscore) ||
				subStrMatch)
			{
				match++;
				matchIndex = count;

				// This is necessary to get the right matchIndex count in the if statement below.
				if (!walker->wholeKey.Compare(inputPrependedWithUnderscore))
					input = inputPrependedWithUnderscore;
				break;
            }

            count++;
            walker = walker->next;
        }
    }

    if (match)
	{
        walker = keywordList;
        // If the user did NOT type in an underscored key word attempt, and we do have
		// a globalizes part to this initget string, we move the walking pointer to the
		// first 'globalized' keyword, then start looking.
        // Ultimately we want to return the globalized string remember.
        if (*input != UNDERSCORECHR && hasglobalKwords) {
            while(walker && walker->wholeKey[0] != UNDERSCORECHR)
                walker = walker->next;
		}
        for (int i = 0; i < matchIndex && walker != NULL; i++) {
            walker = walker->next;
			if (walker == NULL)
				break;
		}

		// If at this point walker is NULL, then we have an unbalanced keyword situation
		// where there are more 'localized' strings than 'globalized'. AutoCAD returns
		// an empty string in this situation. If the 'expert' system variable is == 0, then
		// we will also warn the user of the situation.
		if (walker == NULL) {
			sds_resbuf rb;
			SDS_getvar(NULL,DB_QEXPERT ,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			if(rb.resval.rint == 0) {
				walker = keywordList;
				for (int i = 0; i < matchIndex; i++) {
					walker = walker->next;
					if (walker == NULL) break;
				}
				if (walker != NULL)
					sds_printf(ResourceString(IDC_SDS_GET_NOTGLOBAL, "\nWarning: \'%s\' Has no global match in its initget string!\n\t\tEmpty string returned.\n"),walker->wholeKey);
			}
			// Since acad returns an empty string in this situation, we will too.
			fs2.Empty();

        } else {
			fs2 = (walker->wholeKey[0] == UNDERSCORECHR) ? walker->wholeKey.Right(walker->wholeKey.GetLength() -1) :
														   walker->wholeKey;
            match = 1;
        }

#ifdef MHBDEBUG
        sds_printf("***  NEW ALGORITHM -- Chose:  %s\n", walker->wholeKey);
#endif
    }

    // Clean up the linked list before we leave.
    if (keywordList != NULL) {
        struct kword *t = keywordList;
        while(t) {
            keywordList = t->next;
            delete(t);
            t = keywordList;
        }
    }

    if((SDS_IsDragging && match>1) || (*fs1 != 0 && *input == 0)) {
		cmd_ErrorPrompt(CMD_ERR_PLEASESPEC,0,(void *)fs2.GetBuffer(fs2.GetLength() + 1));
        return(RTFAIL);
	} else if(SDSApplication::GetActiveApplication()->GetInputControlBits() & SDS_RSG_OTHER)
		{
        if(*fs2)
			strcpy(SDSApplication::GetActiveApplication()->thekeyword,fs2);
        else {
            SDS_ArbInputFlag=1;
			if(*str==UNDERSCORECHR)
				strcpy(SDSApplication::GetActiveApplication()->thekeyword,str);
            else
				strcpy(SDSApplication::GetActiveApplication()->thekeyword,input);
        }
        return(RTKWORD);
    } else if(match==0) {
        return(RTERROR);
	// Because of situations like the 'circle, and donnut' prompts, we resolve conflicts by
	// by letting the last match match. In the circle command we have an initget prompt that
	// looks like:  "~RTT RadTanTan ~_RTT ~_RadTanTan" plus other options of course, The reason
	// for this is that the old algorithm didn't reduce ~_RadTanTan to _RTT before the comparison.
	// This new algorithm does, so it is possible to have more than one match. For that reason, I
	// am commenting out this if with the idea of possibly revisiting it later. For now in a TRUE
	// abiguous situation, the last match wins, However something like "LAyer LType" will not
	// recognize 'L' as a valid option.
	//
    //} else if(match>1) {
	//	cmd_ErrorPrompt(CMD_ERR_PLEASESPEC,0,(void *)fs2.GetBuffer(fs2.GetLength() + 1));
    //    return(RTFAIL);
	//
    } else if(match) {
        strcpy(SDSApplication::GetActiveApplication()->thekeyword,fs2);
        return(RTKWORD);
    } else {
        return(RTERROR);
    }
}

// *****************************************
// This is an SDS External API
//
//
int
sdsengine_prompt(const char *szPromptMsg)
	{
    sds_printf("%s"/*DNT*/,szPromptMsg);
    return(RTNORM);
	}

// *****************************************
// This is an SDS External API
//
// TODOTHREAD -- dependent on SDSApplication::GetActiveApplication()
//
int
sdsengine_getkword(const char *szKWordMsg, char *szKWord)
	{

	ASSERT( OnEngineThread() );

    char pmt[IC_ACADBUF * 2];
    int  ret,ret2;

    SDS_SetLastPrompt(szKWordMsg);

    if(szKWordMsg)
		{
		strcpy(pmt,szKWordMsg);
		}

    while(RTCAN!=(ret=sds_getstring(0,(szKWordMsg ? pmt : NULL),szKWord)))
		{

        if( ret==RTNORM &&
			!*szKWord )
			{
            if(SDSApplication::GetActiveApplication()->GetInputControlBits() & SDS_RSG_NONULL)
				{
                goto invalid;
				}
			else
				{
                ret=RTNONE;
                break;
	            }
		    }

        if(RTKWORD==(ret2=SDS_SetKeyWord(szKWord)))
			{
            sds_getinput(szKWord);
            goto out;
	        }

        if(ret==RTERROR)
			{
			return(ret);
			}

        invalid: ;
        if(ret2!=RTFAIL)
			{
			cmd_ErrorPrompt(CMD_ERR_UNRECOGENTRY,0);
			}
	    }

    out: ;

    sds_initget(0,""/*DNT*/); // Reset sds_initget()

    if(ret!=RTKWORD)
		{
		*SDSApplication::GetActiveApplication()->thekeyword=0;
		}

    return(ret);
	}

int SDS_PromptMenu(int Mode)
	{
	struct sds_resbuf rb;
	static char *LastKw;
	static int LastBs;

	SDS_getvar(NULL,DB_QPROMPTMENU,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if(!rb.resval.rint)
		{
		return(RTNORM);
		}

	if(Mode==-1)
		{  // Free the variables and destroy the window.
		if(LastKw)
			{
			delete [] LastKw;
			LastKw=NULL;
			}
		LastBs=0;
		if(SDS_CMainWindow->m_pPromptMenu==NULL)
			{
			return(RTNORM);
			}
		ExecuteUIAction( ICAD_WNDACTION_DELPMTM );
		return(RTNORM);
		}

	if( !SDSApplication::GetActiveApplication()->IsKeywordsEmpty() &&
		!(SDSApplication::GetActiveApplication()->GetInputControlBits() & SDS_RSG_NOPMENU) &&
		GetMenuQueue()->IsEmpty())
		{
		if(Mode)
			{
			/* Title can be different!, so always create new dialog
			if( LastKw &&
				strsame(SDSApplication::GetActiveApplication()->GetKeywords(),LastKw) &&
				SDSApplication::GetActiveApplication()->GetInputControlBits()==LastBs)
				{
				ExecuteUIAction( ICAD_WNDACTION_SHOWPMTM );
				}
			else
			*/
				{
				if(LastKw)
					{
					delete [] LastKw;
					}
				LastKw= new char [ strlen(SDSApplication::GetActiveApplication()->GetKeywords())+1];
				strcpy(LastKw,SDSApplication::GetActiveApplication()->GetKeywords());
				LastBs=SDSApplication::GetActiveApplication()->GetInputControlBits();
				ExecuteUIAction( ICAD_WNDACTION_DELPMTM );
				ExecuteUIAction( ICAD_WNDACTION_PROMPTM );
				}
			}
		else
			{
			ExecuteUIAction( ICAD_WNDACTION_HIDEPMTM );
			}
		}
	else
		{
		if ( Mode == 0 )
			{
			if(LastKw)
				{
				delete [] LastKw;
				LastKw=NULL;
				}
			LastBs=0;
			if(SDS_CMainWindow->m_pPromptMenu==NULL)
				{
				return(RTNORM);
				}
			ExecuteUIAction( ICAD_WNDACTION_DELPMTM );
			}
		}

	return(RTNORM);
	}


int ResbufIsLoneSpace(struct resbuf *rb)
{
	if (rb!=NULL &&
		rb->restype==RTSTR &&
		rb->resval.rstring &&
		rb->resval.rstring[0]==' '/*DNT*/ &&
		rb->resval.rstring[1]==0) return 1;
	return 0;
}


// extension of SDS_AskForPoint; converts angle from char* to double according to some settings
static int calcAngle(int angct, const char* szAngle, double* pAngle)
{
	if(angct == 1 || angct == 3)
	{
		if(RTNORM != sds_angtof(szAngle, -1, pAngle))
			return RTERROR;
	}
	else
		if(angct == 2)
		{
			if(RTNORM != sds_angtof(szAngle, 0, pAngle))
				return RTERROR;
		}
	if(angct > 1)
	{
		resbuf	angdir, angbase;
		SDS_getvar(NULL, DB_QANGDIR, &angdir, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
		SDS_getvar(NULL, DB_QANGBASE, &angbase, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
		if(angdir.resval.rint == 1)
			*pAngle = angbase.resval.rreal - *pAngle;
		else
			*pAngle -= angbase.resval.rreal;
		ic_normang(pAngle, NULL);
	}
	return RTNORM;
}


/*-------------------------------------------------------------------------*//**
This is one of the most important functions in the system.  It is where
the input queue almost always lands.  The main purpose of this function
is to turn input events into requested input by sds_getpoint() etc. or
to turn input into entries in the command buffer

@author
@param	ptStartPoint				=>	start point, for relative selection for example
@param	cszCommandPromptString		=>	command prompt
@param	ptResultPoint				=>	where
@param	internalCall				=>	Note default 'bool internalCall' arg added to fix bug #6430. This function is called
										internally from internalGetpoint() and externally from sds applications via sds_getpoint()
										Internal calls don't have to set internalCall because it defaults to TRUE. If sds_getpoint()
										calls this with a prior call to sds_initget() setting the SDS_RSG_OTHER flag, this will
										temporarily turn off Direct Distance Entry feature.
@return	RTxxx return codes such as RTNONE, RTCAN, RTERROR, RTNORM, RTKWORD
*//*--------------------------------------------------------------------------*/
int SDS_AskForPoint(const sds_point ptStartPoint, const char* cszCommandPromptString, sds_point ptResultPoint, bool internalCall)
{
	ASSERT(OnEngineThread());

	// Modified CyberAge PP 10/04/2000 [
	// Warning: Do not use this variable for any other purpose. ******
	// Reason: Fix for the ICAD.LSP bug reported by Peter Coburn.
	static bool	bNotToPrint = false;
	// Modified CyberAge PP 10/04/2000 ]

	// Modified PK 14/06/2000 [
	static int	osnapmodeold = 0, SnapstrFromQueue = 0;	// Reason : Fix for bug no. 65811
	static char	snapstrold[IC_SNAPSTR];
	COsnapStore	osnapStore;
	if(!SDS_IsDragging)
	{
		SnapstrFromQueue = 0;
		osnapmodeold = 0;
		*snapstrold = 0;
	}
	// Modified PK 14/06/2000 ]

#ifdef DEBUG
	// Helpful in the debugger to be able to look at the command queue
	CommandQueue*	pActiveCommandQueue = GetActiveCommandQueue();
#endif

    int			retcode, fi1, fi2, ret, ret2 = RTFAIL, hitone = 0, osnapmode = 0, draggedit = 0, angct, adj4world = 0, adj4offset = 0,
				pickedpt = 0, PMenuOn = 0, pendXYZ[3] = {0, 0, 0}, XYZmode = 0, fromwaitloop, typedstr = 0, bitsets, ortho, snapstyl;
    sds_point	lastpt, retpt, p1, p2, pt1, pendXYZpt, lastpt_2_restore;
    sds_real	fr1, fr2, fa1, fa2;
    char		*fcp1 = NULL, *fcp2 = NULL, fs1[IC_ACADBUF], keywords[IC_ACADBUF], snapstr[IC_SNAPSTR], psnapstr[IC_ACADBUF], xyzFilterPrompt[200];
    resbuf		rb, rb2, *trb = NULL;
	osnapdata*	osnaprec = NULL;
	//Bugzilla No. 78447 ; 18-03-2003 
	bool		bSetOsnapMode = true;
	CommandQueue	SaveMenuQueue, SaveCommandQueue;

	// HACK HACK HACK
	bool		SaveNoThreadSwap = IDS_GetThreadController()->GetNoThreadSwap();

	//NOTE: lastpt is used by ortho, as well as by osnap for TAN and PER.
	//	If ptStartPoint!=NULL, this is what we want to use as our lastpt for these
	SDS_getvar(NULL, DB_QLASTPOINT, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
	if(!ptStartPoint)
		ic_ptcpy(lastpt, rb.resval.rpoint);
	else
	{
		ic_ptcpy(lastpt_2_restore, rb.resval.rpoint);
		ic_ptcpy(lastpt, ptStartPoint);
		ic_ptcpy(rb.resval.rpoint, ptStartPoint);
		SDS_setvar(NULL, DB_QLASTPOINT, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES, 0);
	}

	rb.restype = rb2.restype = RTSHORT;
	rb.resval.rint = 0;
	rb2.resval.rint = 1;
	*psnapstr = 0;

	// Sometimes we swap threads right off. CommandRunning is set in sds_cmd(),
	// but I don't know why...transparent commands???
    if( GetActiveCommandQueue()->IsEmpty()
		&& SDSApplication::GetActiveApplication()->IsCommandRunning()
		&& !IDS_GetThreadController()->GetNoThreadSwap() )
	{
        // If there is no buffer list clear the script list.
		GetScriptQueue()->Flush();
        SDSApplication::GetActiveApplication()->SetCommandRunning(false);

        // Resume the application thread.
		// THREADSWAP !!!!!!!!!!!!!!!!!
		if(SDSApplication::GetActiveApplication()->GetCommandThread()->IsCurrent())
		{
			SDSApplication*	pApp = SDSApplication::GetActiveApplication();
			SDS_ThreadSwap(pApp->GetMainThread(), pApp->GetCommandThread());
		}
	}

    if(cszCommandPromptString && strcmp(cszCommandPromptString, "\n: "/*DNT*/))
        SDS_SetLastPrompt(cszCommandPromptString);

	SDS_getvar(NULL, DB_QMENUECHO, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
    // Don't print the prompt if dragging, ic_dragobj() will do it.
    if( cszCommandPromptString
		&& !ptStartPoint
		&& GetActiveCommandQueue()->IsEmpty()
		&& (GetMenuQueue()->IsEmpty() || GetMenuQueue()->IsCommandPause() || (!GetMenuQueue()->IsEmpty() && !(rb.resval.rint & 2))) )
		sds_printf("%s"/*DNT*/, cszCommandPromptString);

	// SDS_AtCmdLine is set if we are sitting in the command loop waiting for the user to type
	// something and nothing else is running (in theory)
	SDSApplication::GetActiveApplication()->SetAcceptParenOrCR(false);
	if(SDS_AtCmdLine && SDS_LispPCount > 0)
		SDSApplication::GetActiveApplication()->SetAcceptParenOrCR(true);

    // Check for osnap strings.
	// NOTES:
	// 1) If the 3rd parameter of SDS_SetOsnapCursor is TRUE, it reads from
	// the DWG header to see what the current OSNAP settings are and sets the cursor
	// based on that.  If FALSE, it sets the cursor based on what osnapmode is.
	// At this point, osnapmode has been initialized to 0 (above), and never changed,
	// so this means if we pass SDS_SetOsnapCursor FALSE right here, we're turning
	// osnaps off.  Actually, right now it's a no-op, as SDS_SetOsnapCursor doesn't restore
	// the cursor if osnaps are off.
	// 2) This code passes FALSE to SDS_SetOsnapCursor (see notes above about effect) when
	// we think we are at the command line and not running lisp.  Unfortunately, the setting
	// of these globals seems to be inexact (i.e. wrong).  In an SDS app, SDS_AtCmdLine is TRUE
	// after the initial call through here (I think this is a thread issue, where the main app
	// gets back to the command line, even though the SDS apps isn't).
	//
	// 3) In any case, BUG 45675 was that after the first iteration through, we were ignoring
	// the snaps.  That was because after the first try, we were passing FALSE here and not
	// getting the proper settings--i.e. turning them off (but it didn't turn off the cursor...)
	//  The fix is to not do this if we are not on an internal call.  In other words if sds_getpoint()
	// has been called from an SDS app, pass TRUE and get the osnap settings.  This fixes
	// this particular case.  I hope we don't get through here from something else...
	// 4) Long Term we need to reevaluate this.  I think it is a hack to have SDS_AskForPoint doing
	// this work.  Higher level code should be passing this setting in rather than rely on global
	// setting (especially in multi-threaded case)
	ASSERT(!osnapmode);
	SDS_SetOsnapCursor(snapstr, &osnapmode, !SDS_AtCmdLine || SDS_DoingLispCode || !internalCall);


	// ******************************************
	// BIG LOOP BIG LOOP BIG LOOP
	// search for "BIG LOOP" to find other end

	TopOfForLoop: ;

    for( ; ; )
	{
	    // We have to do a threadswap check here because of transparent commands.
		if( GetActiveCommandQueue()->IsEmpty()
			&& SDSApplication::GetActiveApplication()->IsCommandRunning()
			&& !IDS_GetThreadController()->GetNoThreadSwap() )
		{
			// If there is no buffer list clear the script list.
			GetScriptQueue()->Flush();
			SDSApplication::GetActiveApplication()->SetCommandRunning(false);

			// Resume the application thread.
			// THREADSWAP !!!!!!!!!!!!!!!!!
			if(SDSApplication::GetActiveApplication()->GetCommandThread()->IsCurrent())
			{
				SDSApplication*	pApp = SDSApplication::GetActiveApplication();
				SDS_ThreadSwap(pApp->GetMainThread(), pApp->GetCommandThread());
			}
		}

		typedstr = fromwaitloop = 0;

		// Set the event mask -- basically we don't want to get mouse moves
		/*D.G.*/// Disable some input during real-time operations.
		extern bool SDS_bDraggingSS;
//4M Bugzilla 77987 19/02/02->
/*
		if((SDS_CURVIEW && SDS_CURVIEW->m_pRealTime) || SDS_bDraggingSS)
	        SDS_EventMask(SDS_EVM_MOUSEXYZ | SDS_EVM_MOUSEBUTTON);
*/
		if((SDS_CURVIEW && SDS_CURVIEW->GetRTMotion() != NO) || SDS_bDraggingSS)
		{
//We leave behaviour for ZOOM_WHEEL intact
			if(SDS_CURVIEW->GetRTMotion() == ZOOM_WHEEL)
				SDS_EventMask(SDS_EVM_MOUSEXYZ | SDS_EVM_MOUSEBUTTON);
			else
				SDS_EventMask(SDS_EVM_KEYCHAR | SDS_EVM_MOUSEBUTTON | SDS_EVM_MENUCOMMAND | SDS_EVM_POPUPCOMMAND);
		}
//<-4M 19/02/02
		else
			SDS_EventMask(SDS_EVM_KEYCHAR | SDS_EVM_MOUSEBUTTON | SDS_EVM_MENUCOMMAND | SDS_EVM_POPUPCOMMAND);

		// If there is something in the command buffer, we'll try to process it
		// if not, the else case will try and get input
        if(GetActiveCommandQueue()->IsNotEmpty())
		{
			if(IDS_GetThreadController()->GetNoThreadSwap())
				goto dopause;

			// Eat nil, T or dragpoints!
			if( GetActiveCommandQueue()->PeekItem()->restype == SDS_RTDRAGPT
				|| GetActiveCommandQueue()->PeekItem()->restype == RTNIL
				|| GetActiveCommandQueue()->PeekItem()->restype == RTT )
			{
				GetActiveCommandQueue()->AdvanceNext();
				continue;
			}

			// Print the prompt now if CMDECHO
			SDS_getvar(NULL, DB_QCMDECHO, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
            if(rb.resval.rint)
			{
				sds_printf("%s"/*DNT*/, cszCommandPromptString);

				// If there is something in the command buffer, print it out too
				// so we know what is being processed
				if(GetActiveCommandQueue()->IsNotEmpty())
				{
					rb = *GetActiveCommandQueue()->PeekItem();
		            rb.rbnext = NULL;

					// Modified PK 14/06/2000 [
					SnapstrFromQueue = 0; // Reason : Fix for bug no. 65811
					if(rb.restype == RTSTR && rb.resval.rstring)
					{
						osnapdata*	osnaprecord = NULL;
						if(findosnap(rb.resval.rstring, &osnaprecord) == RTNORM)
							SnapstrFromQueue = 1;
					}
					// Modified PK 14/06/2000 ]

					// Modified PP 10/04/2000 [
					// Reason: Fix for ICAD.LSP bug reported by Peter Coburn.
					// Warning: Do not use this variable for any other purpose.
					if(!bNotToPrint)
					{
						CString	szNotToPrint(rb.resval.rstring);
						if(-1 == szNotToPrint.Find("(if (/= s::startup nil)(s::startup))(princ)"/*DNT*/))
							lsp_prtrecur(&rb, NULL, 0, 0);
						else
							bNotToPrint = true;
					}
					else
						lsp_prtrecur(&rb, NULL, 0, 0);
					// Modified PP 10/04/2000 ]
				}
				else
					// if there is nothing in the command buffer, go back and wait for something to show up.
					// We'll hopefully do a thread swap and allow something to get put in
					continue;
			}

			// ///////////////////////
			// '\\' is the PAUSE character in a sds_command parameter
			// A command is executing and we want to wait for the user to input something and then
			// go on executing the command
			//
            if(GetActiveCommandQueue()->IsCommandPause())
			{
				GetActiveCommandQueue()->AdvanceNext();
				SaveCommandQueue.Copy(GetActiveCommandQueue());
				GetActiveCommandQueue()->Flush();

				// ITC MHB -[ Bugzilla:77764 11/6/2003
				// The call below was preventing proper thread activation when
				// a user presses escape while paused in a sds_command() sequence
				// because of a PAUSE call.
				// This forces it to pause for input
				// IDS_GetThreadController()->SetNoThreadSwap(true);
				// ] - ITC MHB
				goto dopause;
			}

		}	// if(GetActiveCommandQueue()->IsNotEmpty())
		else
		{
			// This is the case where we found a PAUSE or nothing in the command buffer.  Either way, try to
			// get some input
            dopause: ;

			// Modified PK 14/06/2000 [
			if(SDS_IsDragging && SnapstrFromQueue)	// Reason : Fix for bug no. 65811
			{
				osnapmode = osnapmodeold;
				strcpy(snapstr, snapstrold);
				SnapstrFromQueue = osnapmodeold = 0;
				*snapstrold = 0;
			}
			// Modified PK 14/06/2000 ]

			// If ptStartPoint is passed in, we're going to be dragging
            if(ptStartPoint)
			{
                fromwaitloop = draggedit = 1;

                // Need to save the initget setting before we call SDS_dragobject,
                // because it calls sds_draggen which will reset it.
                bitsets = SDSApplication::GetActiveApplication()->GetInputControlBits();
                strncpy(keywords, SDSApplication::GetActiveApplication()->GetKeywords(), sizeof(keywords) - 1);

				SDS_getvar(NULL, DB_QDRAGMODE, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
                ret = SDS_dragobject(SDS_GetCornerMode ? 33 : 32, rb.resval.rint, (double*)ptStartPoint,
									 NULL, 0.0, cszCommandPromptString, NULL, retpt, NULL);

                // Now reset the initget settings we saved right before the call
				// to SDS_dragobject
                SDSApplication::GetActiveApplication()->SetInputControlBits(bitsets);
                SDSApplication::GetActiveApplication()->SetKeywords(keywords);
				//MHB
				/* SDSApplication::GetActiveApplication()->keywords = keywords;
				strncpy(SDSApplication::GetActiveApplication()->keywords, keywords, sizeof(SDSApplication::GetActiveApplication()->keywords) - 1); */

				// This processes the return from SDS_dragobject
                switch(ret)
				{
                case RTNONE:
                case RTCAN:
                    sds_initget(0, ""/*DNT*/);	// Reset sds_initget()
					*SDSApplication::GetActiveApplication()->thekeyword = 0;
                    SDS_SetCursor(IDC_ICAD_CROSS);
                    retcode = ret;
					goto exit;
                    break;
                case RTSTR:
                    ret = RTKWORD;
                case RTKWORD:
					trb = sds_buildlist(RTSTR, SDSApplication::GetActiveApplication()->thekeyword, 0);
					GetActiveCommandQueue()->AddItem(trb);
                    trb = NULL;
                    break;
                case RTNORM:
                    if( !internalCall
						&& (SDSApplication::GetActiveApplication()->GetInputControlBits() & SDS_RSG_OTHER)
						&& *SDSApplication::GetActiveApplication()->thekeyword )
					{
						ret = RTKWORD;
						trb = sds_buildlist(RTSTR, SDSApplication::GetActiveApplication()->thekeyword, 0);
						GetActiveCommandQueue()->AddItem(trb);
						trb = NULL;
						break;
					}
					pickedpt = 1;
					trb = sds_buildlist(RT3DPOINT, retpt, 0);
					GetActiveCommandQueue()->AddItem(trb);
					trb = NULL;
					break;
				default:
					sds_initget(0,""/*DNT*/);	// Reset sds_initget()
					*SDSApplication::GetActiveApplication()->thekeyword = 0;
					SDS_SetCursor(IDC_ICAD_CROSS);
					retcode = RTERROR;
					goto exit;
				}

			}	// if(ptStartPoint)
			else
			{	// no ptStartPoint, so we're not dragging
				//4M Spiros Item:27->
	            if(GripRClickedFlag == 2)
	            {
					GripRClickedFlag = 0;
					pickedpt = 1;
					ic_ptcpy(ptResultPoint, SDS_GripRClicked);
					SDS_getvar(NULL, DB_QELEVATION, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
					ptResultPoint[2] = rb.resval.rreal;
					retcode = RTNORM;
					break;
				}
				//4M Spiros Item:27<-
				if(!PMenuOn)
				{
					SDS_PromptMenu(1);
					PMenuOn = 1;
				}

				if(GetMenuQueue()->IsCommandPause())
				{
					GetMenuQueue()->AdvanceNext();
					SaveMenuQueue.Copy(GetMenuQueue());
					GetMenuQueue()->Flush();
				}


				// This calls to the input queue to get actual events!!!
				// This is the core of the core of SDS_AskForPoint
				// CORE LOOP
				// **************************************************************
				while(SDS_WaitLoop())
 				{
					//4M Bugzilla 77987 19/02/02->
					//We leave behaviour for ZOOM_WHEEL intact
					if( sds_usrbrk() ||
						(GetActiveCommandQueue()->IsNotEmpty() &&
						 SDS_CURVIEW && SDS_CURVIEW->GetRTMotion() != ZOOM_WHEEL &&
						 GetActiveCommandQueue()->PeekItem()->restype != SDS_RTDRAGPT && SDS_CURVIEW->GetRTMotion() != NO) )
 					{
 						SDS_userbreak = 0;
						*ptResultPoint = 0.0;
						sds_initget(0, ""/*DNT*/);	// Reset sds_initget()
						*SDSApplication::GetActiveApplication()->thekeyword = 0;
						SDS_SetCursor(IDC_ICAD_CROSS);
 						SDS_CancelAllPending(1);
						retcode = RTCAN;
						goto exit;
					}
					fromwaitloop = 1;

					// We'll loop until they cancel or something gets put into the command buffer
					// for us to process
					if(GetActiveCommandQueue()->IsNotEmpty())
						break;
				}
				// **************************************************************
				// End of CORE LOOP

				// AG: restore "OSMODE" system var
				osnapStore.restore();
				//Bugzilla No. 78447 ; 18-03-2003 [
				resbuf	rbOsmode;
				SDS_getvar(NULL, DB_QOSMODE, &rbOsmode, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
				if(bSetOsnapMode)
					osnapmode = rbOsmode.resval.rint;
				//Bugzilla No. 78447 ; 18-03-2003 ]

			}	// end of no ptStartPoint
		}	// else of if(GetActiveCommandQueue()->IsNotEmpty()) -- nothing in the command buffer


		// ***************************************************
		// From here on we are processing the input that
		// was set up farther up.  Basically the input has been put into the command
		// buffer.  So we're just looking at the command buffer and trying to decide if we have complete
		// enough input to return or we need to process it, maybe get some more input

		// --------------------------------------------------------
		// By this point, either we started with something in the command buffer, or we went
		// to SDS_WaitLoop and it put something in
		if(GetActiveCommandQueue()->IsEmpty())
		{
			*ptResultPoint = 0.0;
			sds_initget(0, ""/*DNT*/);	// Reset sds_initget()
			if(SDSApplication::GetEngineApplication())
			{
				SDS_SetCursor(IDC_ICAD_CROSS);
				SDS_FreeEventList(0);
			}
			retcode = RTCAN;
			goto exit;
		}

		resbuf*	pItem;
		pItem = GetActiveCommandQueue()->PeekItem();
		// -------------------------------------------
		// Now we start individually handling the various things in the command buffer
		//
        // First with a dragged point
		// If what we got was a drag point, clear it out and go back to
		// the top of the for(;;) loop and get something else

		if(pItem->restype == SDS_RTDRAGPT)
		{
			GetActiveCommandQueue()->AdvanceNext();
			continue;
		}

		SDS_PromptMenu(0);

		// ----------------------------------------
		// Process special types in command buffer
        // The ONLY way we could get any of the following types is if the commands are
        // comming from a sds_command(). So lets convert them to strings let them pass
        // through to the initget checker below.
        if( pItem->restype == RTLONG || pItem->restype == RTSHORT || pItem->restype == RTREAL ||
			pItem->restype == RTANG || pItem->restype == RTORINT )
		{
			switch(pItem->restype)
			{
			case RTLONG :
				sprintf(fs1, "%ld"/*DNT*/, pItem->resval.rlong);
				break;
			case RTSHORT :
				sprintf(fs1, "%i"/*DNT*/, pItem->resval.rint);
				break;
			case RTREAL :
				sprintf(fs1, "%f"/*DNT*/, pItem->resval.rreal);
				break;
			case RTANG :
				SDS_getvar(NULL, DB_QANGBASE, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
				fr1 = pItem->resval.rreal + rb.resval.rreal;
				sds_angtos(fr1, -1, -1, fs1);
				break;
			case RTORINT :
				sds_angtos(pItem->resval.rreal, -1, -1, fs1);
			}

			resbuf*	pNewItem = sds_newrb(RTSTR);
            if(!(pNewItem->resval.rstring = new char[ strlen(fs1) + 1]))
			{
				sds_initget(0, ""/*DNT*/);	// Reset sds_initget()
				SDS_SetCursor(IDC_ICAD_CROSS);
				retcode = RTERROR;
				goto exit;
			}
            strcpy(pNewItem->resval.rstring, fs1);

			// Replace the current head of the queue with the new string version
			GetActiveCommandQueue()->AdvanceNext();
			GetActiveCommandQueue()->AddItemToFront(pNewItem);
		}

		pItem = GetActiveCommandQueue()->PeekItem();	// the 1st item could change due to the previous block

		// ------------------------------------------------
		// Is it a cancel in the command buffer???
		// break out of the for( ; ; ) loop and we'll exit from this function
		if(pItem->restype == RTCAN)
		{
			GetActiveCommandQueue()->AdvanceNext();
			retcode = RTCAN;
			break;
		}


		// ----------------------------------------------
		// A 2D POINT.  If we got a point, let's break out now
		// so we can return it.
		if(pItem->restype == RTPOINT)
		{
			if(!fromwaitloop)
				SDS_SkipLastOrthoSet = 1;
			pickedpt = 1;
			ic_ptcpy(ptResultPoint, pItem->resval.rpoint);
			SDS_getvar(NULL, DB_QELEVATION, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
			ptResultPoint[2] = rb.resval.rreal;
			GetActiveCommandQueue()->AdvanceNext();
			retcode = RTNORM;
			break;
		}

		// -------------------------------------------
		// SDS_AtEntGet is setup by sds_entsel() and by sds_ssget()
		// if we got an entity and we're in one of these functions,
		// let's return out to those functions right from here
		extern int SDS_AtEntGet;
		if(SDS_AtEntGet && (pItem->restype == RTLB || pItem->restype == RTENAME || pItem->restype == RTPICKS))
		{
			retcode = RTENAME;
			break;
		}

		// -------------------------------------------
		// If we got a 3DPOINT we can also get out, but first
		// let's check some special modes to see if we have to adjust
		// the input
		if(pItem->restype == RT3DPOINT)
		{
			// In other words, this was in the command buffer when we started, not
			// just got from the input queue
			if(!fromwaitloop)
				SDS_SkipLastOrthoSet = 1;
			pickedpt = 1;

			ic_ptcpy(ptResultPoint, pItem->resval.rpoint);

			if(XYZmode)
			{
				//if user uses .xyz filtering, we'll need ORTHO so we can omit user-specified
				//	coords from those supplied by pick.  Ortho applies to picked ordinates only
				SDS_getvar(NULL, DB_QORTHOMODE, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
				ortho = rb.resval.rint;
				SDS_getvar(NULL, DB_QSNAPSTYL, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
				if(rb.resval.rint)
				{
					SDS_getvar(NULL, DB_QSNAPISOPAIR, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
					snapstyl = rb.resval.rint;
				}
				else
					snapstyl = -1;
				SDS_getvar(NULL, DB_QSNAPANG, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
				if( !SDS_SkipLastOrthoSet && (ortho || SDS_CMainWindow->m_fIsShiftDown) &&
					SDS_CMainWindow->m_pDragVarsCur && SDS_CMainWindow->m_pDragVarsCur->applyortho )
				{
					ic_ortho(lastpt, rb.resval.rreal, ptResultPoint, ptResultPoint, snapstyl);
					SDS_SkipLastOrthoSet = 1;
				}
			}
			GetActiveCommandQueue()->AdvanceNext();
            retcode = RTNORM;

			if(SDS_CMainWindow->m_pDragVarsCur && SDS_CMainWindow->m_pDragVarsCur->scnf && SDS_CMainWindow->m_pDragVarsCur->breakout)
			{
				retcode = SDS_CMainWindow->m_pDragVarsCur->breakout;
				SDS_CMainWindow->m_pDragVarsCur->breakout = 0;	// Just to be safe
			}

			// in any case, we always break out of the for( ; ; ) loop
			break;
		}	// if(pItem->restype == RT3DPOINT)


		// Next command buffer type is string.
		// *****************************************************
		// BUFFER RTSTR BUFFER RTSTR
		// search for "BUFFER RTSTR" to find the other end of this
		//
		// Watch for goto endOfRTSTR -- that label is at the end of this if() and still within the for( ; ; ) loop
        if(pItem->restype == RTSTR && pItem->resval.rstring)
		{
			int retVal = RTNORM;
			// If it is a NULL string
			if( !*pItem->resval.rstring ||
				(*pItem->resval.rstring == ' '/*DNT*/ &&
				 !(SDS_CMainWindow->m_pDragVarsCur && SDS_CMainWindow->m_pDragVarsCur->mode == -5)) )
			{
				if(SDSApplication::GetActiveApplication()->GetInputControlBits() & SDS_RSG_NONULL)
					goto endOfRTSTR;	// Continue processing
				GetActiveCommandQueue()->AdvanceNext();
				retcode = RTNONE;
				break;
			}

			ASSERT(strlen(pItem->resval.rstring ) < sizeof(fs1));
			strncpy(fs1, pItem->resval.rstring, sizeof(fs1) - 1);

			// Basically this means we're processing some kind of function already, so we have
			// special things to watch for
			if( (!SDS_AtCmdLine || SDS_DoingLispCode || SDS_Inside3rdPartyFunc) &&
				!(SDS_CMainWindow->m_pDragVarsCur && SDS_CMainWindow->m_pDragVarsCur->mode == -5) )
			{
				//Bugzilla No. 78464 ; 03-03-2003 [
				bool	bIsPendingInputBuffer = false;
				char	PendingInputBuffer[IC_ACADBUF], *pTransparentCmd;

				strcpy(PendingInputBuffer, fs1);
				pTransparentCmd = strchr(PendingInputBuffer, '\'');
				
				while(pTransparentCmd && *(pTransparentCmd + 1) != '_')
				{
					++pTransparentCmd;
					if(pTransparentCmd)
						pTransparentCmd = strchr(pTransparentCmd, '\'');
				}
				
				if(pTransparentCmd && *pTransparentCmd != *fs1)
				{					
					resbuf*	pNextItem = GetActiveCommandQueue()->PopItem();
					if(pNextItem->restype == RTSTR)
					{
						strcpy(pNextItem->resval.rstring, pTransparentCmd);
						strcpy(fs1, pTransparentCmd);
						*pTransparentCmd = 0;
						bIsPendingInputBuffer = true;
					}
					GetActiveCommandQueue()->AddItem(pNextItem);
				}
				//Bugzilla No. 78464 ; 03-03-2003 ]

				// A recursive command Lisp, Transparent, or eval.
				if( (*fs1 == '\''/*DNT*/ && strlen(fs1) > 1) ||
					(strnisame(fs1, "_.'", 3) || strnisame(fs1, "._'", 3) || strnisame(fs1, "_'", 2) || strnisame(fs1, ".'", 2)) ||
					*fs1 == '('/*DNT*/ || *fs1 == '!'/*DNT*/ )
				{
					GetActiveCommandQueue()->AdvanceNext();

					// This is to deal with spaces in script file and menus
					if(ResbufIsLoneSpace(GetActiveCommandQueue()->PeekItem()))
						GetActiveCommandQueue()->AdvanceNext();

					if(SDS_CMainWindow->m_pDragVarsCur)
						SDS_CMainWindow->m_pDragVarsCur->firstdrag = 0;

					ExecuteUIAction(ICAD_WNDACTION_DRAGOFF);

					if(SDS_CMainWindow->m_pDragVarsCur)
						SDS_CMainWindow->m_pDragVarsCur->firstdrag = 0;

					SDS_dragvars*	tvars = SDS_CMainWindow->m_pDragVarsCur;
					int				wasdragging = SDS_IsDragging;
					SDS_IsDragging = 0;

					// Save OSMODE and CMDNAMES, which DoOneCommand will change
					SDS_getvar(NULL, DB_QOSMODE, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
					int	oldosmode = rb.resval.rint;
					SDS_getvar(NULL, DB_QCMDNAMES, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);

					// Save keyword settings before calling DoOneCommand
					int				savebits = SDSApplication::GetActiveApplication()->GetInputControlBits();
					char			savkwords[IC_ACADBUF];
					strncpy(savkwords, SDSApplication::GetActiveApplication()->GetKeywords(), sizeof(savkwords) - 1);


					SDS_DoOneCommand(fs1, 1);


					// And restore keyword settings
					SDSApplication::GetActiveApplication()->SetInputControlBits(savebits);
					SDSApplication::GetActiveApplication()->SetKeywords(savkwords);

					SDS_setvar(NULL, DB_QCMDNAMES, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES, 0);
					delete [] rb.resval.rstring;

					SDS_CMainWindow->m_pDragVarsCur = tvars;
					SDS_IsDragging = wasdragging;

					if(SDS_AtEntGet)
						SDS_SetCursor(SDS_GetPickCursor());
					else
					{
						SDS_SetCursor(IDC_ICAD_CROSS);
						SDS_getvar(NULL, DB_QOSMODE, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
						SDS_SetOsnapCursor(snapstr, &osnapmode, oldosmode != rb.resval.rint);
					}

					ExecuteUIAction(ICAD_WNDACTION_DRAGON);

					// Print the prompt again.
					// If more in command queue then the prompt will be printed again
					// up at the top of the for(;;) loop when it prints the result of the
					// expression, so don't print it here or we'll get it twice.
					if(GetActiveCommandQueue()->IsEmpty())
					{
						if(*psnapstr)
							sds_printf(ResourceString(IDC_SDS_GET__NSNAP_TO__S_OF__4, "\nSnap to %s of: "), psnapstr);
						else
						{
							if(XYZmode)
								sds_printf(xyzFilterPrompt);
							else
								if(cszCommandPromptString)
									sds_printf("%s"/*DNT*/, cszCommandPromptString);
						}
					}

					//Bugzilla No. 78464 ; 03-03-2003 [
					if(bIsPendingInputBuffer)
					{
						strcpy(SDS_cmdcharbuf, PendingInputBuffer);
						sds_printf("%s"/*DNT*/, PendingInputBuffer);
						bIsPendingInputBuffer = false;
					}
					//Bugzilla No. 78464 ; 03-03-2003 ]

					// Make the prompt menu come up again.
					PMenuOn = 0;
					continue;
				}	// if( (*fs1 == '\''/*DNT*/ && ...

				typedstr = 1;

				// Just the LASTPOINT.
				if( (*fs1 == '@'/*DNT*/ && !*(fs1 + 1) || strsame(fs1, "_@"/*DNT*/)) &&
					 !(SDS_CMainWindow->m_pDragVarsCur && SDS_CMainWindow->m_pDragVarsCur->mode == -5) )
				{
					GetActiveCommandQueue()->AdvanceNext();
					// This is to deal with spaces in script file and menus
					if(ResbufIsLoneSpace(GetActiveCommandQueue()->PeekItem()))
						GetActiveCommandQueue()->AdvanceNext();
					ic_ptcpy(ptResultPoint, lastpt);
					retcode = RTNORM;
					break;
				}

				// Modified PK 20/04/2000 [
				// Reason: Fix for bug no. 74327
				// Check for osnap strings.
				hitone = 0;
				int params = 0;
				// AG: reset osnapmode first
				osnapmodeold = osnapmode;
				osnapmode = 0;

				fcp1 = strtok(fs1, ","/*DNT*/);
				while(fcp1)
				{
					if( strnisame(ResourceString(IDC_SDS_GET_NON_5, "NON"), fcp1, 3) ||
						strnisame("_NONE", fcp1, 3) )
					{
						*snapstr = *psnapstr = 0;
						osnapmode = IC_OSMODE_NONE;
						hitone = 1;
						SDS_SetCursor(IDC_ICAD_CROSS);
						break;	// AG: if IC_OSMODE_NONE occured in the string, there's nothing to do then
					}
					else
						if(findosnap(fcp1, &osnaprec) == RTNORM)
						{
							//Bugzilla No. 78447 ; 18-03-2003
							bSetOsnapMode = false;

							if(!hitone)
								*snapstr = *psnapstr = 0;
							if(*snapstr)
								strcat(snapstr, ","/*DNT*/);
							if(*psnapstr)
								strcat(psnapstr, ","/*DNT*/);

							strcat(snapstr, osnaprec->name);
							strcat(psnapstr, osnaprec->psnapname);
							osnapmode |= osnaprec->osnapmode;
							// Modified PK 14/06/2000 [
							osnapmodeold = osnapmode;	// Reason : Fix for bug no. 65811
							strcpy(snapstrold, snapstr);
							// Modified PK 14/06/2000 ]
							hitone = 1;
							SDS_SetCursor(osnaprec->cursor);
						}

					fcp1 = strtok(NULL, ","/*DNT*/);
					++params;
				}	// while(fcp1)

				int	inputbits = SDSApplication::GetActiveApplication()->GetInputControlBits();
				if((inputbits & SDS_RSG_2D) && SDS_PointDisplayMode == 2 && params == 3)	// Preeti
				{
					cmd_ErrorPrompt(CMD_ERR_UNRECOGENTRY, 0);
					if(cszCommandPromptString)
						sds_printf("%s"/*DNT*/, cszCommandPromptString);
					GetActiveCommandQueue()->AdvanceNext();
					continue;
				}
				// Modified PK 20/04/2000 ]

				if(hitone)
				{
					//AG: store an old value of "OSMODE" sysvar and replace it with a new one if input was successful
					osnapStore.store(osnapmode);
					//ExecuteUIAction(ICAD_WNDACTION_RESETOSNAPMANAGER);
					ExecuteUIAction(ICAD_WNDACTION_PROCESSOSNAPPOINT);

					if(strchr(psnapstr, ','))
						SDS_SetCursor(IDC_ICAD_SNAP);
					if(*psnapstr)
						sds_printf(ResourceString(IDC_SDS_GET__NSNAP_TO__S_OF__4, "\nSnap to %s of: "), psnapstr);
					else
						if(cszCommandPromptString)
							sds_printf("%s"/*DNT*/, cszCommandPromptString);

					GetActiveCommandQueue()->AdvanceNext();

					// This is to deal with spaces in script file and menus
					if(ResbufIsLoneSpace(GetActiveCommandQueue()->PeekItem()))
						GetActiveCommandQueue()->AdvanceNext();

					continue;
				}
				else
					osnapmode = osnapmodeold;	// restore osnapmode var if nothing relevant was found

				// Need do the .x, .y, and .z.
				if( (*fs1 == '.' && (strnisame(fs1 + 1, "X", 1) || strnisame(fs1 + 1, "Y", 1) || strnisame(fs1 + 1, "Z", 1))) &&
					!(SDS_CMainWindow->m_pDragVarsCur && SDS_CMainWindow->m_pDragVarsCur->mode == -5))
				{
					strupr(fs1);
					fi1 = fi2 = 0;
					for(fcp1 = fs1 + 1; *fcp1; ++fcp1)
					{
						switch(*fcp1)
						{
						case 'X'/*DNT*/:
							fi2 |= 1;
							XYZmode = pendXYZ[0] = 1;
							break;
						case 'Y'/*DNT*/:
							fi2 |= 2;
							XYZmode = pendXYZ[1] = 1;
							break;
						case 'Z'/*DNT*/:
							fi2 |= 4;
							XYZmode = pendXYZ[2] = 1;
							break;
						default:
							if(atoi(fcp1))
							{
								fi1 = 1;
								break;
							}
							else
								goto endOfRTSTR;	// Continue processing
						}
					}

					if(!fi1)
					{
						if(XYZmode)
						{
							//if user types in XYZ filter, after point selected or typed in the other
							// filters are set to 1.  If user types in a 2nd filter, we need to reset
							//	the ones they DON'T want to get back to 0
							if(pendXYZ[0] == 1 && !(fi2 & 1))
								pendXYZ[0] = 0;
							if(pendXYZ[1] == 1 && !(fi2 & 2))
								pendXYZ[1] = 0;
							if(pendXYZ[2] == 1 && !(fi2 & 4))
								pendXYZ[2] = 0;
						}

						GetActiveCommandQueue()->AdvanceNext();
						// This is to deal with spaces in script file and menus
						if(ResbufIsLoneSpace( GetActiveCommandQueue()->PeekItem()))
							GetActiveCommandQueue()->AdvanceNext();

						sprintf(xyzFilterPrompt, ResourceString(IDC_SDS_GET__NSELECT__S_OF__35, "\nSelect %s of: "), fs1 + 1);
						sds_printf(xyzFilterPrompt);
						continue;
					}
				}	// if( (*fs1 == '.' && ... -- .x.y.z filters

				// Get the angle and distance.
				if( (fcp1 = strchr(GetActiveCommandQueue()->PeekItem()->resval.rstring, '<'/*DNT*/)) &&
					!(SDS_CMainWindow->m_pDragVarsCur && SDS_CMainWindow->m_pDragVarsCur->mode == -5) )
				{
					fi1 = 0;
					if(GetActiveCommandQueue()->PeekItem()->resval.rstring[0] == '@'/*DNT*/)
						fi1 = 1;
					if(GetActiveCommandQueue()->PeekItem()->resval.rstring[0] == '*'/*DNT*/)
					{
						fi1 = 1;
						adj4world = 1;
					}
					else
						if(GetActiveCommandQueue()->PeekItem()->resval.rstring[1] == '*'/*DNT*/)
						{
							fi1 = 2;
							adj4world = 1;
						}

					strncpy(fs1, GetActiveCommandQueue()->PeekItem()->resval.rstring + fi1, sizeof(fs1) - 1);
					ic_trim(fs1, "bme"/*DNT*/);

					//get the starting point from which to do our offset
					if(GetActiveCommandQueue()->PeekItem()->resval.rstring[0] == '@'/*DNT*/)
					{
						ic_ptcpy(pt1, lastpt);
						if(adj4world)
						{
							//transform lastpt to wcs, then add on the wcs
							//	vector below.  leave adj4world flag ON so that
							//	result gets transformed back to UCS on the way out!
							resbuf	wcs, ucs;
							wcs.rbnext = ucs.rbnext = NULL;
							wcs.restype = ucs.restype = RTSHORT;
							wcs.resval.rint = 0;
							ucs.resval.rint = 1;
							sds_trans(pt1, &ucs, &wcs, 0, pt1);
						}
					}
					else
					{
						pt1[0] = pt1[1] = 0.0;
						if(!adj4world)
						{
							SDS_getvar(NULL, DB_QELEVATION, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
							pt1[2] = rb.resval.rreal;
						}
					}

					// Get dist
					fcp1 = strchr(fs1, '<'/*DNT*/);
					*fcp1 = 0;
					++fcp1;
					for(angct = 1; *fcp1 == '<'/*DNT*/; ++angct, ++fcp1)
						;

					if(RTNORM != (retVal = sds_distof(fs1, -1, &fr1)))
						goto endOfRTSTR;	// Continue processing

					// Now get ang and Z
					if(!*fcp1)
						goto endOfRTSTR;	// Continue processing

					if(fcp2 = strchr(fcp1, ','/*DNT*/))	// has Z
					{
						*fcp2 = 0;
						if(!*(fcp2 + 1))
							goto endOfRTSTR;	// Continue processing

						if(calcAngle(angct, fcp1, &fa1) != RTNORM)
							goto endOfRTSTR;	// Continue processing

						if(RTNORM != (retVal = sds_distof(fcp2 + 1, -1, &fa2)))
							goto endOfRTSTR;	// Continue processing

						pt1[2] += fa2;
						sds_polar(pt1, fa1, fr1, ptResultPoint);
						SDS_SkipLastOrthoSet = 1;

						GetActiveCommandQueue()->AdvanceNext();
						// This is to deal with spaces in script file and menus
						if(ResbufIsLoneSpace(GetActiveCommandQueue()->PeekItem()))
							GetActiveCommandQueue()->AdvanceNext();

						retcode = RTNORM;
						break;
					}	// if(fcp2 = strchr(fcp1, ','/*DNT*/))
					else
						if(fcp2 = strchr(fcp1, '<'/*DNT*/))	// has 2 angles
						{
							*fcp2 = 0;
							if(calcAngle(angct, fcp1, &fa1) != RTNORM)
								goto endOfRTSTR;	// Continue processing

							++fcp2;
							for(angct = 1; *fcp2 == '<'/*DNT*/; ++angct, ++fcp2)
								;

							if(calcAngle(angct, fcp2, &fa2) != RTNORM)
								goto endOfRTSTR;	// Continue processing

							// Calc the new point
							ptResultPoint[0] = pt1[0] + fr1 * cos(fa2) * cos(fa1);
							ptResultPoint[1] = pt1[1] + fr1 * cos(fa2) * sin(fa1);
							ptResultPoint[2] = pt1[2] + fr1 * sin(fa2);

							SDS_SkipLastOrthoSet = 1;

							GetActiveCommandQueue()->AdvanceNext();
							// This is to deal with spaces in script file and menus
							if(ResbufIsLoneSpace(GetActiveCommandQueue()->PeekItem()))
								GetActiveCommandQueue()->AdvanceNext();

							retcode = RTNORM;
							break;
						}
						else
						{	// has only one ang
							if(calcAngle(angct, fcp1, &fa1) != RTNORM)
								goto endOfRTSTR;	// Continue processing

							sds_polar(pt1, fa1, fr1, ptResultPoint);

							// NOTE: don't set orthoskip if we're still gonna ask for another point because
							// we're in .xyz mode
							if(!((pendXYZ[0] & 1) + (pendXYZ[1] & 1) + (pendXYZ[2] & 1)))
								SDS_SkipLastOrthoSet = 1;

							GetActiveCommandQueue()->AdvanceNext();
							// This is to deal with spaces in script file and menus
							if(ResbufIsLoneSpace(GetActiveCommandQueue()->PeekItem()))
								GetActiveCommandQueue()->AdvanceNext();

							retcode = RTNORM;
							break;
						}
				}	// if( (fcp1 = strchr(GetActiveCommandQueue()->PeekItem()->resval.rstring, '<'/*DNT*/)) && ...
			}	// if( (!SDS_AtCmdLine || ...

			// Must be a point entered with commas, get X, Y, and Z.
			if( (fcp1 = strchr(GetActiveCommandQueue()->PeekItem()->resval.rstring, ','/*DNT*/)) &&
				!(SDS_CMainWindow->m_pDragVarsCur && SDS_CMainWindow->m_pDragVarsCur->mode == -5) )
			{
				fi1 = 0;

				// Check for @1,2,3 (add offset)
				if(GetActiveCommandQueue()->PeekItem()->resval.rstring[0] == '@'/*DNT*/)
				{
					adj4offset = 1;
					fi1 = 1;
				}
				// Check for *1,2,3 (Convert to WCS)
				if(GetActiveCommandQueue()->PeekItem()->resval.rstring[0] == '*'/*DNT*/)
				{
					adj4world = 1;
					fi1 = 1;
				}

				strncpy(fs1, GetActiveCommandQueue()->PeekItem()->resval.rstring + fi1, sizeof(fs1) - 1);
				ic_trim(fs1, "bme"/*DNT*/);

				// Get X
				fcp1 = strchr(fs1, ',');
				*fcp1 = 0;
				if(RTNORM != (retVal = sds_distof(fs1, -1, &pt1[0])))
					goto endOfRTSTR;	// Continue processing

				// Get Y
				if(!*(fcp1 + 1))
				{
					if(cszCommandPromptString)
						sds_printf("%s"/*DNT*/, cszCommandPromptString);
					GetActiveCommandQueue()->AdvanceNext();
					// This is to deal with spaces in script file and menus
					if(ResbufIsLoneSpace(GetActiveCommandQueue()->PeekItem()))
						GetActiveCommandQueue()->AdvanceNext();
					continue;
				}
				fcp2 = strchr(fcp1 + 1, ','/*DNT*/);
				if(fcp2)
					*fcp2 = 0;

				if(RTNORM != (retVal = sds_distof(fcp1 + 1, -1, &pt1[1])))
					goto endOfRTSTR;	// Continue processing

				// Get Z
				if(!fcp2)
				{
					/*D.Gavrilov*/// If it's a relative coords then we should set non-entered coords to 0.0
					if(adj4offset)
						pt1[2] = 0.0;
					else
					{
						SDS_getvar(NULL, DB_QELEVATION, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
						pt1[2] = rb.resval.rreal;
					}
				}
				else
				{
					if(!*(fcp2 + 1))
						goto endOfRTSTR;	// Continue processing
					if(RTNORM != (retVal = sds_distof(fcp2 + 1, -1, &pt1[2])))
						goto endOfRTSTR;	// Continue processing
				}

				if(adj4offset)
				{
					ptResultPoint[0] = lastpt[0] + pt1[0];
					ptResultPoint[1] = lastpt[1] + pt1[1];
					ptResultPoint[2] = lastpt[2] + pt1[2];
				}
				else
					ic_ptcpy(ptResultPoint, pt1);

				SDS_SkipLastOrthoSet = 1;

				GetActiveCommandQueue()->AdvanceNext();

				// This is to deal with spaces in script file and menus
				if(ResbufIsLoneSpace(GetActiveCommandQueue()->PeekItem()))
					GetActiveCommandQueue()->AdvanceNext();

				retcode = RTNORM;
				break;
			}	// if( (fcp1 = strchr(GetActiveCommandQueue()->PeekItem()->resval.rstring, ','/*DNT*/)) &&
			else
				if(1 == (pendXYZ[0] & 1) + (pendXYZ[1] & 1) + (pendXYZ[2] & 1))
				{
					//if we only need one remaining coordinate from .xyz filtering...
					ic_trim(fs1, "bme"/*DNT*/);
					if(pendXYZ[0] == 1)
						fi1 = 0;
					else
						if(pendXYZ[1] == 1)
							fi1 = 1;
						else
							if(pendXYZ[2] == 1)
								fi1 = 2;

					if(RTNORM != (retVal = sds_distof(fs1, -1, &pt1[fi1])))
						goto endOfRTSTR;	// Continue processing

					ptResultPoint[fi1] = pt1[fi1];

					GetActiveCommandQueue()->AdvanceNext();
					// This is to deal with spaces in script file and menus
					if(ResbufIsLoneSpace(GetActiveCommandQueue()->PeekItem()))
						GetActiveCommandQueue()->AdvanceNext();

					retcode = RTNORM;
					break;
				}

			// Read the "GoDistDir" commands. TODO - how about localization?!
			if( toupper(*fs1) == 'G' && toupper(*(fs1 + 1)) == 'O' &&
				!(SDS_CMainWindow->m_pDragVarsCur && SDS_CMainWindow->m_pDragVarsCur->mode == -5) )
			{
				ic_ptcpy(p1, lastpt);
				fi1 = strlen(fs1);
				if(strisame(ResourceString(IDC_SDS_GET_UP_36, "UP"), fs1 + fi1 - 2))
				{
					*(fs1 + fi1 - 2) = 0;
					if(RTNORM != (retVal = sds_distof(fs1 + 2, -1, &fr1)))
						goto endOfRTSTR;	// Continue processing
					p1[2] += fr1;
				}
				else if(strisame(ResourceString(IDC_SDS_GET_DOWN_37, "DOWN"), fs1 + fi1 - 4))
				{
					*(fs1 + fi1 - 4) = 0;
					if(RTNORM != (retVal = sds_distof(fs1 + 2, -1, &fr1)))
						goto endOfRTSTR;	// Continue processing
					p1[2] -= fr1;
				}
				else if(strisame(ResourceString(IDC_SDS_GET_LEFT_38, "LEFT"), fs1 + fi1 - 4) ||
						strisame(ResourceString(IDC_SDS_GET_WEST_39, "WEST"), fs1 + fi1 - 4))
				{
					*(fs1 + fi1 - 4) = 0;
					if(RTNORM != (retVal = sds_distof(fs1 + 2, -1, &fr1)))
						goto endOfRTSTR;	// Continue processing
					sds_polar(p1, IC_PI, fr1, p1);
				}
				else if(strisame(ResourceString(IDC_SDS_GET_RIGHT_40, "RIGHT"), fs1 + fi1 - 5) ||
						strisame(ResourceString(IDC_SDS_GET_EAST_41, "EAST"), fs1 + fi1 - 4))
				{
					if(toupper(*(fs1 + fi1 - 5)) == 'R')
						*(fs1 + fi1 - 5) = 0;
					else
						*(fs1 + fi1 - 4) = 0;
					if(RTNORM != (retVal = sds_distof(fs1 + 2, -1, &fr1)))
						goto endOfRTSTR;	// Continue processing
					sds_polar(p1, 0.0, fr1, p1);
				}
				else if(strisame(ResourceString(IDC_SDS_GET_AWAY_42, "AWAY"), fs1 + fi1 - 4) ||
						strisame(ResourceString(IDC_SDS_GET_NORTH_43, "NORTH"), fs1 + fi1 - 5))
				{
					if(toupper(*(fs1 + fi1 - 4)) == 'A')
						*(fs1 + fi1 - 4) = 0;
					else
						*(fs1 + fi1 - 5) = 0;
					if(RTNORM != (retVal = sds_distof(fs1 + 2, -1, &fr1)))
						goto endOfRTSTR;	// Continue processing
					sds_polar(p1, IC_PI * 0.5, fr1, p1);
				}
				else if(strisame(ResourceString(IDC_SDS_GET_TOWARD_44, "TOWARD"), fs1 + fi1 - 6) ||
						strisame(ResourceString(IDC_SDS_GET_SOUTH_45, "SOUTH"), fs1 + fi1 - 5))
				{
					if(toupper(*(fs1 + fi1 - 6)) == 'T')
						*(fs1 + fi1 - 6) = 0;
					else
						*(fs1 + fi1 - 5) = 0;
					if(RTNORM != (retVal = sds_distof(fs1 + 2, -1, &fr1)))
						goto endOfRTSTR;	// Continue processing
					sds_polar(p1, IC_PI * 1.5, fr1, p1);
				}
				else
					goto endOfRTSTR;	// Continue processing

				ic_ptcpy(ptResultPoint, p1);

				GetActiveCommandQueue()->AdvanceNext();
				// This is to deal with spaces in script file and menus
				if(ResbufIsLoneSpace(GetActiveCommandQueue()->PeekItem()))
					GetActiveCommandQueue()->AdvanceNext();

				retcode = RTNORM;
				break;
			}	// if( toupper(*fs1) == 'G' && ... -- GoDistDir commands

			if( (cmd_InsideSds_draggen || ptStartPoint) &&
				SDS_CMainWindow->m_pDragVarsCur &&
				!cmd_InsideRotateCommand &&
				!cmd_InsideScaleCommand &&
				*GetActiveCommandQueue()->PeekItem()->resval.rstring &&
				!SDS_CMainWindow->m_pDragVarsCur->enternub &&
				!SDS_AtAngDist &&
				(sds_isdigit((unsigned char)*GetActiveCommandQueue()->PeekItem()->resval.rstring) ||
				 *GetActiveCommandQueue()->PeekItem()->resval.rstring == '-'/*DNT*/ ||
				 *GetActiveCommandQueue()->PeekItem()->resval.rstring == '.'/*DNT*/) &&
				RTNORM == sds_distof(GetActiveCommandQueue()->PeekItem()->resval.rstring, -1, &fr1) &&
				fr1 &&
				!(SDSApplication::GetActiveApplication()->GetInputControlBits() & SDS_RSG_OTHER))
			{	// Direct Distance Entering
				ic_ptcpy(p1, ptStartPoint ? ptStartPoint : SDS_CMainWindow->m_pDragVarsCur->pt1);
				p2[0] = SDS_CMainWindow->m_pDragVarsCur->LastDrag[0] - p1[0];
				p2[1] = SDS_CMainWindow->m_pDragVarsCur->LastDrag[1] - p1[1];
				p2[2] = SDS_CMainWindow->m_pDragVarsCur->LastDrag[2] - p1[2];
				fr2 = sqrt(p2[0] * p2[0] + p2[1] * p2[1] + p2[2] * p2[2]);

				if(icadRealEqual(fr2, 0.0))
				{
					ic_ptcpy(ptResultPoint, p1);
					GetActiveCommandQueue()->AdvanceNext();
					retcode = RTNORM;
					break;
				}
				ptResultPoint[0] = p1[0] + fr1 * p2[0] / fr2;
				ptResultPoint[1] = p1[1] + fr1 * p2[1] / fr2;
				ptResultPoint[2] = p1[2] + fr1 * p2[2] / fr2;

				GetActiveCommandQueue()->AdvanceNext();
				retcode = RTNORM;
				break;
			}


			// CONTINUE PROCESSING LABEL
			// This is where many things in the RTSTR processing jump to
			// to go on
			endOfRTSTR: ;

			// If we've now got a KEYWORD, we can return that immediately
			if(retVal == RTREJ)
				ret2 = retVal;
			else
			{
				if(RTKWORD == (ret2 = SDS_SetKeyWord(GetActiveCommandQueue()->PeekItem()->resval.rstring)))
				{
					GetActiveCommandQueue()->AdvanceNext();
					// This is to deal with spaces in script file and menus
					if(ResbufIsLoneSpace(GetActiveCommandQueue()->PeekItem()))
						GetActiveCommandQueue()->AdvanceNext();
					retcode = RTKWORD;
					break;
				}
			}
		}
		// end of BUFFER RTSTR BUFFER RTSTR BUFFER RTSTR
		// *****************************************************

        if(ret2 != RTFAIL)
			cmd_ErrorPrompt(CMD_ERR_PT, 0);

		sds_printf("%s"/*DNT*/, cszCommandPromptString);
		GetActiveCommandQueue()->AdvanceNext();

	}	// for( ; ; )
	//
	// end of BIG LOOP BIG LOOP BIG LOOP
	// ****************************************

	// AG: restore "OSMODE" sysvar
	osnapStore.restore();

	// From here on, we're probably headed to a return from SDS_AskForPoint
	// but there are a couple of cases where we jump back in to the for(;;) loop.
	// Look for TopOfForLoop

	// Need to adjust for the WCS of * is present
	if(adj4world)
	{
		resbuf	wcs, ucs;
		// Set up resbufs for sds_trans():
	    wcs.rbnext = ucs.rbnext = NULL;
		wcs.restype = ucs.restype = RTSHORT;
		wcs.resval.rint = 0;
		ucs.resval.rint = 1;
		sds_trans(ptResultPoint, &wcs, &ucs, 0, ptResultPoint);
	}

    // Save the LASTPOINT. - AutoCAD doesn't do this.  I found this out doing commands like LINE. JC ~|
    if(retcode == RTNORM)
	{
		COsnapManagerIcad*	pOsnapManager = ((CEngineApplication*)SDSApplication::GetEngineApplication())->getOsnapManager();
		if( !pOsnapManager->isLocked() &&
			((osnapmode && !(osnapmode & IC_OSMODE_OFF) && !draggedit && fromwaitloop && !typedstr) ||
			 (typedstr && osnapmode && !(osnapmode & IC_OSMODE_OFF) && *psnapstr)) )
		{
			int			osrc;
			CIcadView*	pView = SDS_CMainWindow->m_pCurView;
			ASSERT(pView);
			gr_view*	view = pView->m_pCurDwgView;
			ASSERT(view);
			//progesoft [
			// if is intersection snap active and flyoversnap doesn't intercept
			// any intersect point go with old procedure to select two entities
			osrc = pOsnapManager->getPoint(ptResultPoint);
			//if(!osrc) progesoft
			if(!osrc && !stristr(snapstr, ResourceString(IDC_SDS_OSNAP_INT_6, "INT")))
				osrc = RTERROR;
			else
				if(!osrc && stristr(snapstr, ResourceString(IDC_SDS_OSNAP_INT_6, "INT")))
					osrc = sds_osnap(pickedpt ? SDS_PreGridPoint : ptResultPoint, "INT", ptResultPoint);
			// ] progesoft
				else
					if(osrc == 1)
						osrc = RTNORM;
					else
						// use an old sds_osnap() way if fly-over snapping is disabled
						osrc = sds_osnap(pickedpt ? SDS_PreGridPoint : ptResultPoint, snapstr, ptResultPoint);

			if(osrc != RTNORM)
			{
				if(hitone || osrc == RTNONE)
				{
                    // Running osnaps need to update this string.
                    if(!*psnapstr)
					{
                        if(!osnaprec)
                            findosnap(snapstr, &osnaprec);
						if(osnaprec) // Bugzilla78625 Don't copy if findosnap is failed. 2003/9/17 Ebatech(cnbr)
	                       	strcpy(psnapstr, osnaprec->psnapname);
					}

					sds_printf(ResourceString(IDC_SDS_GET__NNO__S_FOUND_F_46, "\nNo %s found for selected point."), psnapstr);
					// Print the prompt again.
					if(cszCommandPromptString)
						sds_printf("%s"/*DNT*/, cszCommandPromptString);
					*snapstr = *psnapstr = 0;
					hitone = 0;
					SDS_SetOsnapCursor(snapstr, &osnapmode, TRUE);
					if(!osnapmode)
						SDS_SetCursor(IDC_ICAD_CROSS);
					goto TopOfForLoop;
				}
			}
			else
				if(ptStartPoint || SDS_IsDragging)
					SDS_SkipLastOrthoSet = 1;

			if(SDS_IsDragging && osrc == RTNORM && ((pendXYZ[0] & 1) + (pendXYZ[1] & 1) + (pendXYZ[2] & 1)))
				SDS_SkipLastOrthoSet = 0;

				//NOTE: don't skip orthoset if we're still gonna ask for another point becasue
				//	we're in .xyz mode
		}	// if( !pOsnapManager->isLocked() && ...

		// Check for point outside limits
		SDS_getvar(NULL, DB_QLIMCHECK, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
		if(rb.resval.rint && !(SDSApplication::GetActiveApplication()->GetInputControlBits() & SDS_RSG_NOLIM))
		{
			int	tilemode, cvport;
			if(SDS_getvar(NULL, DB_QTILEMODE, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES) != RTNORM)
			{
				retcode = RTERROR;
				goto exit;
			}
			tilemode = rb.resval.rint;
			if(SDS_getvar(NULL, DB_QCVPORT, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES) != RTNORM)
			{
				retcode = RTERROR;
				goto exit;
			}
			cvport = rb.resval.rint;

			SDS_getvar(NULL, (!tilemode && cvport == 1) ? DB_QP_LIMMIN : DB_QLIMMIN, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
			ic_ptcpy(p1, rb.resval.rpoint);
			SDS_getvar(NULL, (!tilemode && cvport == 1) ? DB_QP_LIMMAX : DB_QLIMMAX, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
			ic_ptcpy(p2, rb.resval.rpoint);
			rb.restype = rb2.restype = RTSHORT;
			rb.resval.rint = 0;
			rb2.resval.rint = 1;
			sds_trans(p1, &rb, &rb2, 0, p1);
			sds_trans(p2, &rb, &rb2, 0, p2);
			if(p1[0] > ptResultPoint[0] || p1[1] > ptResultPoint[1] || p2[0] < ptResultPoint[0] || p2[1] < ptResultPoint[1])
			{
				cmd_ErrorPrompt(CMD_ERR_PTOUTSIDELIMITS, 0);
				sds_printf("%s"/*DNT*/, cszCommandPromptString);
				goto TopOfForLoop;
			}
		}

		if(XYZmode)
		{
			if(pendXYZ[0] == 1)
			{
				pendXYZpt[0] = ptResultPoint[0];
				pendXYZ[0] = 2;
			}
			if(pendXYZ[1] == 1)
			{
				pendXYZpt[1] = ptResultPoint[1];
				pendXYZ[1] = 2;
			}
			if(pendXYZ[2] == 1)
			{
				pendXYZpt[2] = ptResultPoint[2];
				pendXYZ[2] = 2;
			}
			if(pendXYZ[0] == 2 && pendXYZ[1] == 2 && pendXYZ[2] == 2)
			{
				ic_ptcpy(ptResultPoint, pendXYZpt);

				// Don't understand why we EVER set to 0 here, but it certainly is wrong
				// if the point wasn't picked
				if(pickedpt)
					SDS_SkipLastOrthoSet = 0;
			}
			else
			{	//set all other ordinates to pending.  if user re-filters, we'll catch it above
				strcpy(xyzFilterPrompt, ResourceString(IDC_SDS_GET__NSTILL_NEED__47, "\nStill need "));
				if(!pendXYZ[0])
				{
					strcat(xyzFilterPrompt, "X" /*DNT*/);
					pendXYZ[0] = 1;
				}
				if(!pendXYZ[1])
				{
					strcat(xyzFilterPrompt, "Y" /*DNT*/);
					pendXYZ[1] = 1;
				}
				if(!pendXYZ[2])
				{
					strcat(xyzFilterPrompt, "Z" /*DNT*/);
					pendXYZ[2] = 1;
				}
				strcat(xyzFilterPrompt, ResourceString(IDC_SDS_GET__OF___48, " of: "));
				sds_printf(xyzFilterPrompt);

				*snapstr = *psnapstr = 0;
				hitone = 0;
				SDS_SetOsnapCursor(snapstr, &osnapmode, TRUE);
				if(!osnapmode)
					SDS_SetCursor(IDC_ICAD_CROSS);
				goto TopOfForLoop;
			}
		}	// if(XYZmode)

		SDS_DrawBlip(ptResultPoint);
		rb.restype = RT3DPOINT;
		ic_ptcpy(rb.resval.rpoint, ptResultPoint);
		SDS_setvar(NULL, DB_QLASTPOINT, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES, 0);
	}	// if(retcode == RTNORM)


	if(retcode == RTCAN)
	{
 		SaveMenuQueue.Flush();
		SaveCommandQueue.Flush();
	}

	if(SaveCommandQueue.IsNotEmpty())
		GetActiveCommandQueue()->Copy(&SaveCommandQueue);

	if(SaveMenuQueue.IsNotEmpty())
		GetMenuQueue()->Copy(&SaveMenuQueue);

	// If we're canceling, empty the command buffer
	if(retcode == RTCAN)
		GetActiveCommandQueue()->Flush();

    SDS_SetCursor(IDC_ICAD_CROSS);

	sds_initget(0, ""/*DNT*/);	// Reset sds_initget()

    if(retcode != RTKWORD && retcode != RTSTR)
		*SDSApplication::GetActiveApplication()->thekeyword = 0;


	exit:

	// HACK HACK HACK
	// Matches save of this global at the top of SDS_AskForPoint
	IDS_GetThreadController()->SetNoThreadSwap(SaveNoThreadSwap);

	// Restore Last Point if we error'd or canceled or something
	if(ptStartPoint && retcode != RTNORM)
	{
		ic_ptcpy(rb.resval.rpoint, lastpt_2_restore);
		rb.restype = RT3DPOINT;
		SDS_setvar(NULL, DB_QLASTPOINT, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES, 0);
	}

    return retcode;
}


// *****************************************
// This is an SDS External API
//
int
sdsengine_getpoint(const sds_point ptReference, const char *szPointMsg, sds_point ptPoint, bool bDirectCall)
{

	ASSERT( OnEngineThread() );

	int ret = RTNORM;

	if( !SDSApplication::GetEngineApplication()->GetMainThread()->IsCurrent() && bDirectCall && SDS_AtCmdLine )
	{
		// redirect call to the Engine posting a job there to prevent deadlock
		CIcadDoc *pDoc = SDS_CURDOC;
		if( !pDoc )
			return RTERROR;

		HRESULT hr = pDoc->AUTO_getpoint( ptReference, szPointMsg, ptPoint );
		if( FAILED( hr ) )
			ret = RTERROR;

		return ret;
	}

    return internalGetpoint(ptReference, szPointMsg, ptPoint, FALSE);;
}












// *******************************************************************
// *******************************************************************
// *****************************************
// This is an SDS External API
//
int
sdsengine_getstring(int swSpaces, const char *szStringMsg, char *szString, bool bDirectCall)
{

	COsnapLocker protector;

	ASSERT( OnEngineThread() );

    struct resbuf rb,*trb=NULL;
    sds_real fr1;
	char fs1[IC_ACADBUF];
    int batch=0, paus=0, ret = RTNORM;

	if( !SDSApplication::GetEngineApplication()->GetMainThread()->IsCurrent() && bDirectCall && SDS_AtCmdLine )
	{
		// redirect call to the Engine posting a job there to prevent deadlock
		CIcadDoc *pDoc = SDS_CURDOC;
		if( !pDoc )
			return RTERROR;

		HRESULT hr = pDoc->AUTO_getstring( swSpaces, szStringMsg, szString );
		if( FAILED( hr ) )
			ret = RTERROR;

		return ret;
	}

	// Helpful in the debugger to be able to look at the command queue
	//
	CommandQueue *pTempDebug = GetActiveCommandQueue();

    SDS_SetCursor(IDC_ICAD_CROSS);

    if( GetActiveCommandQueue()->IsNotEmpty() )
		{
		batch=1;
		}

    if( GetActiveCommandQueue()->IsEmpty() &&
		SDSApplication::GetActiveApplication()->IsCommandRunning() &&
		!IDS_GetThreadController()->GetNoThreadSwap())
		{

        // If there is no buffer list clear the script list.
		GetScriptQueue()->Flush();

        SDSApplication::GetActiveApplication()->SetCommandRunning( false );

        // Resume the application thread.
		// ****************************************
		// THREADSWAP !!!!!!!!!!!!!!!!!
		if ( SDSApplication::GetActiveApplication()->GetCommandThread()->IsCurrent() )
			{
			SDSApplication *pStartApp = SDSApplication::GetActiveApplication();
			SDSApplication *pStopApp = SDSApplication::GetActiveApplication();

			SDS_ThreadSwap( pStartApp->GetMainThread(), pStopApp->GetCommandThread());
			}
		}

    SDS_SetLastPrompt(szStringMsg);

	if ( swSpaces )
		{
	    SDSApplication::GetActiveApplication()->SetAcceptParenOrCR( true );
		}
	else
		{
	    SDSApplication::GetActiveApplication()->SetAcceptParenOrCR( false );
		}

    SDS_EventMask(SDS_EVM_KEYCHAR | SDS_EVM_MENUCOMMAND | SDS_EVM_POPUPCOMMAND);
    ret=RTERROR;

    for( ;; )
		{

	    // We have to do a threadswap check here because of transparent commands.
		if( GetActiveCommandQueue()->IsEmpty() &&
			SDSApplication::GetActiveApplication()->IsCommandRunning() &&
			!IDS_GetThreadController()->GetNoThreadSwap())
			{

			// If there is no buffer list clear the script list.
			GetScriptQueue()->Flush();

			SDSApplication::GetActiveApplication()->SetCommandRunning( false );

			// Resume the application thread.
			// ****************************************
			// THREADSWAP !!!!!!!!!!!!!!!!!
			if ( SDSApplication::GetActiveApplication()->GetCommandThread()->IsCurrent() )
				{
				SDSApplication *pStartApp = SDSApplication::GetActiveApplication();
				SDSApplication *pStopApp = SDSApplication::GetActiveApplication();

				SDS_ThreadSwap( pStartApp->GetMainThread(), pStopApp->GetCommandThread());
				}
			}

        if( GetActiveCommandQueue()->IsNotEmpty() )
			{

            if(sds_usrbrk())
				{
				SDS_CancelAllPending(1);
                return(RTCAN);
	            }

            if(IDS_GetThreadController()->GetNoThreadSwap())
				{
				goto dopause;
				}

			if( GetActiveCommandQueue()->PeekItem()->restype==RTNIL ) //Not sure why this is here
				{
				resbuf *pNewItem = sds_newrb( RTSTR );
				db_astrncpy(&pNewItem->resval.rstring,"nil"/*DNT*/,3);
				GetActiveCommandQueue()->AdvanceNext();
				GetActiveCommandQueue()->AddItemToFront( pNewItem );
				continue;
				}
			if( GetActiveCommandQueue()->PeekItem()->restype==RTT )
				{
				resbuf *pNewItem = sds_newrb( RTSTR );
				db_astrncpy(&pNewItem->resval.rstring,"T"/*DNT*/,3);
				GetActiveCommandQueue()->AdvanceNext();
				GetActiveCommandQueue()->AddItemToFront( pNewItem );
				continue;
				}

            // Eat nil, T or dragpoints!
            if( GetActiveCommandQueue()->PeekItem()->restype==SDS_RTDRAGPT )
				{
				GetActiveCommandQueue()->AdvanceNext();
				continue;
	            }

            if( GetActiveCommandQueue()->IsCommandPause())
				{
				//Added by Ron 01/13/00 to support "\\" as literal text?
					char tmpbuf[255];
					strcpy(tmpbuf, GetActiveCommandQueue()->PeekItem()->resval.rstring);
					SDS_getvar(NULL,DB_QTEXTEVAL,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
					if(rb.resval.rint==1){
						GetActiveCommandQueue()->AdvanceNext();
						goto dopause;
					}
	            }

		    }
		else // if(IsNotEmpty()
			{
            dopause: ;
            paus=1;

            if(szStringMsg)
				{
				sds_printf("%s"/*DNT*/,szStringMsg);
				}

			SDS_PromptMenu(1);

           	while(SDS_WaitLoop())
				{

               	if(sds_usrbrk())
					{
	               	SDS_userbreak=0;
					SDS_CancelAllPending(1);
                   	*szString=0;
                   	return(RTCAN);
               		}

               	if( GetActiveCommandQueue()->IsNotEmpty() )
					{
					break;
					}
            	}
			SDS_PromptMenu(0);
	        }

		// Check to see if the user entered a transparent command in responce
		// to a (getstring)
		if(GetActiveCommandQueue()->PeekItem()->restype==RTSTR)
			{

			// Modified CyberAge VSB 02/23/2001 [
			// Reason: Fix for Bug No. 58859 from Bugzilla
			//char tmpbuf[ 255 ];
			//strcpy(tmpbuf, GetActiveCommandQueue()->PeekItem()->resval.rstring);

			char tmpbuf[ IC_ACADBUF ];
			strncpy(tmpbuf, GetActiveCommandQueue()->PeekItem()->resval.rstring,sizeof(tmpbuf)-1);

			// Modified CyberAge VSB 02/23/2001 ]

			//Added by Ron 1/12/00 to fix bug 2946,57584
			//57586,58014 partially fixed
			//,59050,59247
			SDS_getvar(NULL,DB_QTEXTEVAL,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			//old line
			//if(*tmpbuf=='\'' || *tmpbuf=='(' || *tmpbuf=='!')
			//new line
			if((*tmpbuf=='\'' || *tmpbuf=='(' || *tmpbuf=='!' ) && (rb.resval.rint==1))
				{
				// Pull this command off the queue.
				GetActiveCommandQueue()->AdvanceNext();
				// This is to deal with spaces in script file and menus
				if(ResbufIsLoneSpace( GetActiveCommandQueue()->PeekItem() ))
					{
					GetActiveCommandQueue()->AdvanceNext();
					}
				// Save off the command name and restore it after doing this
				// transparent command.
				struct sds_resbuf rb;
				SDS_getvar(NULL,DB_QCMDNAMES,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

				// -----------------------------
				// Save keyword settings before calling DoOneCommand
				//
				int savebits=SDSApplication::GetActiveApplication()->GetInputControlBits();
				char savkwords[IC_ACADBUF];
				strncpy(savkwords,SDSApplication::GetActiveApplication()->GetKeywords(),sizeof(savkwords)-1);

				SDS_DoOneCommand(tmpbuf,1);

				// -----------------------------
				// And restore keyword settings
				//
				SDSApplication::GetActiveApplication()->SetInputControlBits( savebits );
				SDSApplication::GetActiveApplication()->SetKeywords(savkwords);

				SDS_setvar(NULL,DB_QCMDNAMES,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
				delete [] rb.resval.rstring;
				continue;
				}
			}
		SDS_getvar(NULL,DB_QCMDECHO,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
        if( rb.resval.rint &&
			batch &&
			!paus)
			{
			sds_printf("%s"/*DNT*/,szStringMsg);
			}


        if( GetActiveCommandQueue()->IsEmpty() )
			{
			ret=RTNONE;

	        // The ONLY way we could get any of the following types is if the commands are
	        // comming from a sds_command(). So lets convert them to strings and pass them on.
	        // The types should be converted by the calling get function back to numbers.
		    }
		else if( GetActiveCommandQueue()->PeekItem()->restype==RTSHORT ||
			     GetActiveCommandQueue()->PeekItem()->restype==RTREAL ||
	             GetActiveCommandQueue()->PeekItem()->restype==RTANG ||
		         GetActiveCommandQueue()->PeekItem()->restype==RTORINT)
			{
            if( GetActiveCommandQueue()->PeekItem()->restype==RTSHORT )
				{
                sprintf(szString,"%i"/*DNT*/,GetActiveCommandQueue()->PeekItem()->resval.rint);
				}
			else if( GetActiveCommandQueue()->PeekItem()->restype==RTORINT)
				{
                sds_angtos( GetActiveCommandQueue()->PeekItem()->resval.rreal,-1,-1,szString);
				}
			else if( GetActiveCommandQueue()->PeekItem()->restype==RTANG)
				{
				SDS_getvar(NULL,DB_QANGBASE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
                fr1=GetActiveCommandQueue()->PeekItem()->resval.rreal+rb.resval.rreal;
                sds_angtos(fr1,-1,-1,szString);
				}
			else
				{
                sprintf(szString,"%f"/*DNT*/,GetActiveCommandQueue()->PeekItem()->resval.rreal);
	            }
			GetActiveCommandQueue()->AdvanceNext();
            ret=RTNORM;
			}
		else if(GetActiveCommandQueue()->PeekItem()->restype==RTLONG)
			{
            sprintf(szString,"%ld"/*DNT*/,GetActiveCommandQueue()->PeekItem()->resval.rlong);
			GetActiveCommandQueue()->AdvanceNext();
            ret=RTNORM;
			}
		else if(GetActiveCommandQueue()->PeekItem()->restype==RT3DPOINT)
			{
            sprintf(szString,"%lf , %lf, %lf"/*DNT*/,GetActiveCommandQueue()->PeekItem()->resval.rpoint[0],
													 GetActiveCommandQueue()->PeekItem()->resval.rpoint[1],
													 GetActiveCommandQueue()->PeekItem()->resval.rpoint[2]);
			GetActiveCommandQueue()->AdvanceNext();
            ret=RTNORM;
			}
		else if(GetActiveCommandQueue()->PeekItem()->restype==RTPOINT)
			{
            sprintf(szString,"%lf , %lf"/*DNT*/,GetActiveCommandQueue()->PeekItem()->resval.rpoint[0],
													 GetActiveCommandQueue()->PeekItem()->resval.rpoint[1]);
			GetActiveCommandQueue()->AdvanceNext();
            ret=RTNORM;
			}
		else if(GetActiveCommandQueue()->PeekItem()->restype==RTCAN)
			{
			GetActiveCommandQueue()->AdvanceNext();
            ret=RTCAN;
			}
		else if(GetActiveCommandQueue()->PeekItem()->restype==RTSTR)
			{

			// A recursive command Lisp, Transparent, or eval.
		    if( *GetActiveCommandQueue()->PeekItem()->resval.rstring=='('/*DNT*/ ||
				*GetActiveCommandQueue()->PeekItem()->resval.rstring=='!'/*DNT*/ ||
				//Added by Ron 1/12/00
				*GetActiveCommandQueue()->PeekItem()->resval.rstring=='\''/*DNT*/) //Transparent command
				{
				SDS_getvar(NULL,DB_QTEXTEVAL,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

				if(rb.resval.rint==1)
					{
					SDS_getvar(NULL,DB_QCMDNAMES,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
					strncpy(fs1,GetActiveCommandQueue()->PeekItem()->resval.rstring,sizeof(fs1)-1);
					GetActiveCommandQueue()->AdvanceNext();

					// -----------------------------
					// Save keyword settings before calling DoOneCommand
					//
					int savebits=SDSApplication::GetActiveApplication()->GetInputControlBits();
					char savkwords[IC_ACADBUF];
					strncpy(savkwords,SDSApplication::GetActiveApplication()->GetKeywords(),sizeof(savkwords)-1);

					SDS_DoOneCommand(fs1,1);

					// -----------------------------
					// And restore keyword settings
					//
					SDSApplication::GetActiveApplication()->SetInputControlBits( savebits );
                    SDSApplication::GetActiveApplication()->SetKeywords(savkwords);

					SDS_setvar(NULL,DB_QCMDNAMES,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
					delete [] rb.resval.rstring;
	  				continue;
					}
				}

            if(!swSpaces)
				{
                if( GetActiveCommandQueue()->PeekItem()->restype==RTSTR &&
					GetActiveCommandQueue()->PeekItem()->resval.rstring[0]==' '/*DNT*/)
					{
                    *szString=0;
					GetActiveCommandQueue()->AdvanceNext();
					}
				else if( GetActiveCommandQueue()->PeekItem()->restype==RTSTR &&
						 GetActiveCommandQueue()->PeekItem()->resval.rstring[0]==SDS_USERSPACE)
					{
                    *szString=0;
					GetActiveCommandQueue()->AdvanceNext();
					}
				else
					{
				    // Modified CyberAge VSB 02/23/2001
					// Reason: Fix for Bug No. 58859 from Bugzilla
					//strcpy(szString,GetActiveCommandQueue()->PeekItem()->resval.rstring);
					strncpy(szString,GetActiveCommandQueue()->PeekItem()->resval.rstring,IC_ACADBUF-1);

					GetActiveCommandQueue()->AdvanceNext();

					// This is to deal with spaces in script file and menus
					if ( ResbufIsLoneSpace( GetActiveCommandQueue()->PeekItem() ) )
						{
						GetActiveCommandQueue()->AdvanceNext();
		                }
					}
		        }
			else // if (!swSpaces)
				{
                // This is to read the space bar from menus and script files.
				if(GetActiveCommandQueue()->PeekItem()->resval.rstring[0]==SDS_USERSPACE)
					{
					strcpy(szString," "/*DNT*/);
					GetActiveCommandQueue()->AdvanceNext();
					}
				else
					{
					// Modified CyberAge VSB 02/23/2001
					// Reason: Fix for Bug No. 58859 from Bugzilla
					//strcpy(szString,GetActiveCommandQueue()->PeekItem()->resval.rstring);
					strncpy(szString,GetActiveCommandQueue()->PeekItem()->resval.rstring,IC_ACADBUF-1);
					GetActiveCommandQueue()->AdvanceNext();
					if(!GetMenuQueue()->IsEmpty())
						{
						struct resbuf menuecho;
						SDS_getvar(NULL,DB_QMENUECHO,&menuecho,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
						const char *szMenuToken = NULL;
						while( ( (szMenuToken = GetMenuQueue()->PeekString()) != NULL) &&
							   szMenuToken[0]==' '/*DNT*/)
							{
							strcat( szString, szMenuToken );
							if(!(menuecho.resval.rint & IC_MENUECHO_SUPPRESS))
								{
								rb=*GetMenuQueue()->PeekItem();
								rb.rbnext=NULL;
								lsp_prtrecur(&rb,NULL,0,0);
								}
							GetMenuQueue()->AdvanceNext();

							szMenuToken = GetMenuQueue()->PeekString();
							if( (szMenuToken != NULL) &&
								(szMenuToken[0]==0) )
								{
								GetMenuQueue()->AdvanceNext();
								break;
								}

							szMenuToken = GetMenuQueue()->PeekString();
							if( (szMenuToken != NULL) &&
								(szMenuToken[0]!='\\'/*DNT*/) )
								{
								strcat(szString,szMenuToken);
								if(!(menuecho.resval.rint & IC_MENUECHO_SUPPRESS))
									{
									rb=*GetMenuQueue()->PeekItem();
									rb.rbnext=NULL;
									lsp_prtrecur(&rb,NULL,0,0);
									}
								GetMenuQueue()->AdvanceNext();
								}
							else
								{
								break;
								}
							} // end of while( !....)
						}
					else
						{
						while( ResbufIsLoneSpace( GetActiveCommandQueue()->PeekItem() ) )
							{
							strcat(szString, GetActiveCommandQueue()->PeekItem()->resval.rstring);

							GetActiveCommandQueue()->AdvanceNext();

							if(  ResbufIsLoneSpace( GetActiveCommandQueue()->PeekItem() ) )
								{
								continue;
								}



							if( GetActiveCommandQueue()->IsNotEmpty() &&
								GetActiveCommandQueue()->PeekItem()->restype==RTSTR &&
								GetActiveCommandQueue()->PeekItem()->resval.rstring[0]!='\\'/*DNT*/)
								{
								strcat(szString,GetActiveCommandQueue()->PeekItem()->resval.rstring);
								GetActiveCommandQueue()->AdvanceNext();
								}
							else
								{
								break;
								}
							} // end of while(
						}



					if(*szString && szString[strlen(szString)-1]==' '/*DNT*/)
						{

						if( GetActiveCommandQueue()->IsNotEmpty() &&
							GetActiveCommandQueue()->PeekItem()->restype==RTSTR &&
							GetActiveCommandQueue()->PeekItem()->resval.rstring[0]==0)
							{
							GetActiveCommandQueue()->AdvanceNext();
							}

						const char *szMenuToken = GetMenuQueue()->PeekString();
						if( (szMenuToken != NULL) &&
							szMenuToken[0]==0)
							{
							GetMenuQueue()->AdvanceNext();
							}
						}

					}
				}

            ret=RTNORM;
			// Modified CyberAge VSB 02/23/2001 [
			// Reason: Fix for Bug No. 58859 from Bugzilla
			if ( strlen( szString ) >= IC_ACADBUF-1)
				szString[IC_ACADBUF-1] = '\0';
			// Modified CyberAge VSB 02/23/2001 ]
	        }


		SDS_getvar(NULL,DB_QCMDECHO,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
        if(rb.resval.rint && batch && !paus) sds_printf("%s"/*DNT*/,szString);
        return(ret);


		} // end of for(;;) loop


} // end of sdsengine_getstring() function




// *****************************************
// This is an SDS External API
//
// TODOTHREAD -- is this thread relative?  app relative?
//
int
sdsengine_initget(int bsAllowed, const char *szKeyWordList)
	{
#if 1//rk
	if (!IDS_IsIcadExiting())
	{
		ASSERT( OnEngineThread() );

		SDSApplication::GetActiveApplication()->SetInputControlBits( bsAllowed );
		if(!szKeyWordList)
		{
			SDSApplication::GetActiveApplication()->ClearKeywords();
			return(RTNORM);
		}
		SDSApplication::GetActiveApplication()->SetKeywords((char*)szKeyWordList);
	}
    return(RTNORM);
#else
	ASSERT( OnEngineThread() );

    SDSApplication::GetActiveApplication()->SetInputControlBits( bsAllowed );
    if(!szKeyWordList)
		{
        SDSApplication::GetActiveApplication()->ClearKeywords();
        return(RTNORM);
	    }
    SDSApplication::GetActiveApplication()->SetKeywords((char*)szKeyWordList);
    return(RTNORM);
#endif
	}

static void
splitIntoKeywordList(CString fs1, struct kword **keywordList)
{
    ASSERT(!fs1.IsEmpty());
    char *kwd;
    CString p;
	int lastcap;
    bool hasComma;
    CString wordStr = fs1;
    struct kword *wkstructptr;

    *keywordList = new struct kword;
    (*keywordList)->next = NULL;
    wkstructptr = *keywordList;

    kwd = _tcstok(wordStr.GetBuffer(wordStr.GetLength() + 1),SPACE);
    hasComma = _tcschr(kwd, COMMACHR) != NULL;
    while(kwd) {
        char *vertBar;
        if((vertBar = _tcsstr(kwd, VERTICALBAR)) != NULL) {
            kwd = vertBar + 1;
        }
        wkstructptr->wholeKey = ((*kwd == TILDECHR && _tcslen(kwd) != 1) &&
                                 (!strisame(kwd, TILEDUNDERSCORE)) ) ? kwd + 1 : kwd;
		// We'll chop this off when we find the last upper case letter.
		wkstructptr->leftWithCaps = wkstructptr->wholeKey;

        int len=_tcslen(kwd);
		// This loop populates the leftWithCaps and capsOnly members with the proper
		// strings.
		if (kwd[0] == TILDECHR) kwd++;
		lastcap = -1;

        for (int i = 0; i < len; i++)
		{
			if (strisame(kwd, UNDERSCORE))
				continue;

			// If the letter is upper case, a digit in the first column (or second if it has an
			// underscore), an underscore in the first column, or a ',' comma character.
			if ((isupper((unsigned char) kwd[i]) ||
				(isdigit((unsigned char) kwd[i]) && i >= 0 && i < 2) ||
				(kwd[i] == UNDERSCORECHR && i >= 0 && i < 2) ||
				kwd[i] == COMMACHR))
			{
                wkstructptr->capsOnly += kwd[i];
				lastcap = i+1;
            }
		}

        // If both the capsOnly and the leftWithCaps are both empty, or they are both only an underscore,
        // Empty them.
		if (!wkstructptr->capsOnly.Compare(UNDERSCORE) || !wkstructptr->capsOnly.Compare(TILDE) || !wkstructptr->capsOnly.Compare(TILEDUNDERSCORE))
			wkstructptr->capsOnly.Empty();
		if (!wkstructptr->leftWithCaps.Compare(UNDERSCORE) || !wkstructptr->leftWithCaps.Compare(TILDE) || !wkstructptr->leftWithCaps.Compare(TILEDUNDERSCORE))
			wkstructptr->leftWithCaps.Empty();

		if(lastcap == -1 || (lastcap == 1 && wkstructptr->leftWithCaps[0] == UNDERSCORECHR))
			wkstructptr->capsOnly = wkstructptr->leftWithCaps;
		else
			wkstructptr->leftWithCaps = wkstructptr->leftWithCaps.Left(lastcap);

        if((wkstructptr->capsOnly.IsEmpty() && wkstructptr->leftWithCaps.IsEmpty())) {
            // Here we didn't have any thing that was picked up as an upper case letter. i.e _2.
            // So we will copy in the wholeKey because the whole key must be matched.
            if (wkstructptr->wholeKey.Compare(TILDE) && wkstructptr->wholeKey.Compare(TILEDUNDERSCORE)) {
                wkstructptr->leftWithCaps = wkstructptr->wholeKey;
                wkstructptr->capsOnly     = wkstructptr->wholeKey;
            }
        }

        if(hasComma) {
            int i, j;
            // First leave only the characters to the right of the comma in capsOnly
            i = wkstructptr->capsOnly.FindOneOf(COMMA);
            wkstructptr->capsOnly = wkstructptr->capsOnly.Right(wkstructptr->capsOnly.GetLength() - (i+1));

            // Now leave only the characters to the left of the comma in wholeKey
            i = wkstructptr->wholeKey.FindOneOf(COMMA);
            wkstructptr->wholeKey = wkstructptr->wholeKey.Left(i);

            // Leave only the characters to the left of the comma in leftWithCaps
            i = wkstructptr->leftWithCaps.FindOneOf(COMMA);
            wkstructptr->leftWithCaps = wkstructptr->leftWithCaps.Left(i);

            // Now find the keyword uppercase shortcut keys in the string.
            i = wkstructptr->leftWithCaps.Find(wkstructptr->capsOnly);
            // Make them all lower case.
            wkstructptr->leftWithCaps.MakeLower();
            // Now replace the leftWithCaps letters that match the ones in capsOnly to
            // to upper case.
            //Bugzilla No. 77915 ; 15-04-2002
			//for (j = 0; j < wkstructptr->capsOnly.GetLength(); j++)
			for (j = 0; j < wkstructptr->capsOnly.GetLength() && i>=0 ; j++)
                wkstructptr->leftWithCaps.SetAt(i++, wkstructptr->capsOnly[j]);

            // Now chop off the leftWithCaps off right after the upper case short cut letters.
            wkstructptr->leftWithCaps = wkstructptr->leftWithCaps.Left(i);
        }
        kwd = _tcstok(NULL,SPACE);
        if (kwd) {
            hasComma = _tcschr(kwd, COMMACHR) != NULL;
            wkstructptr->next = new struct kword;
            wkstructptr = wkstructptr->next;
            wkstructptr->next = NULL;
        }
    }
}

