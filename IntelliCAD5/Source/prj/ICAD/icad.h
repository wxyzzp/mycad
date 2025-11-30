/* G:\ICADDEV\PRJ\ICAD\ICAD.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

#ifndef _ICAD_H_
#define _ICAD_H_

#include "stdafx.h"


//#define	_ICAD_NO_DEBUG_CRT	// to run against non-Debug ArchT
#ifdef	_ICAD_NO_DEBUG_CRT
	#define	_AFX_NO_DEBUG_CRT
	#ifdef	_DEBUG
		#define	_ICAD_REDEFINE_DEBUG
		#undef	_DEBUG
	#endif
#endif


	// remove these only if AFX will redefine
#ifndef	__AFX_H__
#undef	ASSERT
#undef	VERIFY
#endif

/*
#include <afxwin.h> 		// MFC core and standard components
#include <afxconv.h>
#include <afxext.h> 		// MFC extensions
#include <afxcmn.h>			// MFC support for Windows 95 Common Controls
#include <afxpriv.h>
#include <afxole.h>
#include <afxdao.h>
#include <afxodlgs.h>
#include <afxtempl.h>
#include <afxadv.h>
#include <afxcview.h>
#include <afxctl.h>
#include <afxodlgs.h>
#include <afxmt.h>
#include <assert.h>
*/
	// This include is probably dangerous... using an unpublished function
#include <..\src\afximpl.h> //*** For GetOpenFile()
#include <afxpriv2.h>

#ifdef	_ICAD_REDEFINE_DEBUG
#define	_DEBUG	1
#endif


#define	X	SDS_X
#define	Y	SDS_Y
#define	Z	SDS_Z

//#include "resource.h"
#include "IcadMain.h"
//#include "IcadApi.h"
//#include "IcadView.h"
//#include "icadlib.h"
#include "res\icadrc2.h"

#include "sds.h"
	// This is defined in SDS.H and conflicts with STL template definitions
	// It should only be used by SDS apps
#undef	X
#undef	Y
#undef	Z
#undef	T

//#include "IcadDoc.h"
#include "icadres.h"

class db_handitem;
class db_drawing;

class CIcadDoc;
class CIcadView;

//** Defines for getting current drawing, config and session.
extern CMainWindow *SDS_CMainWindow;

#define SDS_CURDWG	(SDS_CMainWindow==NULL ? NULL : (SDS_CMainWindow->m_pCurDoc==NULL ? NULL : (SDS_CMainWindow->CurrentDrawing())))
#define SDS_CURSES	(SDS_CMainWindow->m_CurConfig)
#define SDS_CURCFG	(SDS_CMainWindow->m_CurSession)
#define SDS_CURVIEW (SDS_CMainWindow==NULL ? NULL : SDS_CMainWindow->m_pCurView)
#define SDS_CURGRVW (SDS_CMainWindow==NULL ? NULL : (SDS_CMainWindow->m_pCurView==NULL ? NULL : (SDS_CMainWindow->m_pCurView->m_pCurDwgView)))
#define SDS_CURDOC	(SDS_CMainWindow==NULL ? NULL : SDS_CMainWindow->m_pCurDoc)

#define SDS_ISVALIDDRAWING(pDrw) (SDS_CMainWindow==NULL ? NULL : SDS_CMainWindow->IsValidDrawing(pDrw))

// EBATECH(CNBR) 2001-06-24 -[ OEM App.
extern RGBQUAD	SDS_PaletteColors[256];
#define SDS_CURPALLET (( SDS_CMainWindow && SDS_CMainWindow->m_pCurDoc && SDS_CMainWindow->m_pCurDoc->m_DocPalette ) ? SDS_CMainWindow->m_pCurDoc->m_DocPalette : SDS_PaletteColors )
// EBATECH(CNBR) ]-

	//** Vars

extern int	  SDS_MultibleMode;
extern db_drawing *SDS_ExtdwgBeg,*SDS_ExtdwgCur;
extern int	  SDS_ExternalQuery;
extern int	  lsp_cmdhasmore;
extern int	  SDS_DrawInOneWindow;
extern int	  SDS_IgnoreMenuStar;
extern bool   SDS_DontDrawVPGuts;
extern char  *SDS_UpdateSetvar;
extern db_handitem *SDS_LastHandItem;
extern bool   SDS_WasFromPipeLine;
#include "lispvars.h"
extern char cmd_dview_target;	//*** For Dview command, 2=rotate target, 1= (default)rotate camera, 0= value on exit so
								//		dialog won't change for call from any other cmd

	// Use EditString for those cases where Windows APIs are mistyped to expect
	// LPTSTR but really only need LPCTSTR
class EditString : public CString
	{
public:
	CString		operator =(LPCTSTR string)	{ return CString::operator=( string);}
				operator LPTSTR()			{ return const_cast<LPTSTR>((LPCSTR)*this);}
	};


/*D.Gavrilov*/// The following macros are used for loading and concatenating
// resource strings which are the parts of a one string. It's for localising
// (Loc@le cannot process strings longer than 255 symbols).

#define LOADSTRINGS_2(id)	CString s_part1, s_part2, s_all;	\
							s_part1.LoadString(id##__part1);	\
							s_part2.LoadString(id##__part2);	\
							s_all = s_part1 + s_part2;
#define LOADSTRINGS_3(id)	CString s_part1, s_part2, s_part3, s_all;	\
							s_part1.LoadString(id##__part1);			\
							s_part2.LoadString(id##__part2);			\
							s_part3.LoadString(id##__part3);			\
							s_all = s_part1 + s_part2 + s_part3;
#define LOADSTRINGS_4(id)	CString s_part1, s_part2, s_part3, s_part4, s_all;	\
							s_part1.LoadString(id##__part1);					\
							s_part2.LoadString(id##__part2);					\
							s_part3.LoadString(id##__part3);					\
							s_part4.LoadString(id##__part4);					\
							s_all = s_part1 + s_part2 + s_part3 + s_part4;
#define LOADSTRINGS_5(id)	CString s_part1, s_part2, s_part3, s_part4, s_part5, s_all;	\
							s_part1.LoadString(id##__part1);							\
							s_part2.LoadString(id##__part2);							\
							s_part3.LoadString(id##__part3);							\
							s_part4.LoadString(id##__part4);							\
							s_part5.LoadString(id##__part5);							\
							s_all = s_part1 + s_part2 + s_part3 + s_part4 + s_part5;
// Note this stores the strings in reverse order.
// Used to load saveas types in saveas dialog dropdown.
#define LOADFILETYPES_9(id)	CString s_part9, s_part8, s_part7, s_part6, s_part5, s_part4, s_part3, s_part2, s_part1, s_all;	\
							s_part1.LoadString(id##__part1);							\
							s_part2.LoadString(id##__part2);							\
							s_part3.LoadString(id##__part3);							\
							s_part4.LoadString(id##__part4);							\
							s_part5.LoadString(id##__part5);							\
							s_part6.LoadString(id##__part6);							\
							s_part7.LoadString(id##__part7);							\
							s_part8.LoadString(id##__part8);							\
							s_part9.LoadString(id##__part9);							\
							s_all = s_part9 + s_part8 + s_part7 + s_part6 + s_part5 + s_part4 + s_part3 + s_part2 + s_part1;
#define LOAD_COMMAND_OPTIONS_2(id)	CString s_part1, s_part2, s_all;	\
									s_part1.LoadString(id##__part1);	\
									s_part2.LoadString(id##__part2);	\
									s_all = s_part1 + " " + s_part2;
#define LOAD_COMMAND_OPTIONS_3(id)	CString s_part1, s_part2, s_part3, s_all;	\
									s_part1.LoadString(id##__part1);			\
									s_part2.LoadString(id##__part2);			\
									s_part3.LoadString(id##__part3);			\
									s_all = s_part1 + " " + s_part2 + " " + s_part3;
#define LOAD_COMMAND_OPTIONS_4(id)	CString s_part1, s_part2, s_part3, s_part4, s_all;	\
									s_part1.LoadString(id##__part1);					\
									s_part2.LoadString(id##__part2);					\
									s_part3.LoadString(id##__part3);					\
									s_part4.LoadString(id##__part4);					\
									s_all = s_part1 + " " + s_part2 + " " + s_part3 + " " + s_part4;
#define LOAD_COMMAND_OPTIONS_5(id)	CString s_part1, s_part2, s_part3, s_part4, s_part5, s_all;	\
									s_part1.LoadString(id##__part1);							\
									s_part2.LoadString(id##__part2);							\
									s_part3.LoadString(id##__part3);							\
									s_part4.LoadString(id##__part4);							\
									s_part5.LoadString(id##__part5);							\
									s_all = s_part1 + " " + s_part2 + " " + s_part3 + " " + s_part4 + " " + s_part5;

#define LOADEDSTRING	s_all


/*DG - 1.2.2003*/// This define allows to draw all pspace (layout) viewports content into bitmap in the SDS_DrawBitmap function
// (which is used in sds_bmpout and drawing thumbnail bitmaps creating at least).
#define DRAW_PSPACE_VPORTS

#ifdef DRAW_PSPACE_VPORTS
 /*DG - 5.2.2003*/// This define allows to draw the drawing using the current rendering statuses (hide/shade/normal) of the current view
 // (if TILEMODE is ON) and VIEWPORTs (if TILEMODE is OFF and CVPORT > 1) into bitmap in the SDS_DrawBitmap function
 // (which is used in sds_bmpout and drawing thumbnail bitmaps creating at least).
 // It allows also to set render mode (dxf281) of the current VIEWPORT using HIDE/SHADE commands.
 #define BMP_USE_HIDESHADE_STATUS
#endif


#endif // _ICAD_H_
