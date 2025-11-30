/** FILENAME:     MainFrm.cpp- Contains implementation of class CMainFrm.
* Copyright  (C) Copyright 1998-99 Visio Corporation.  All Rights Reserved.
*
*
*
* Abstract Override OnCreateClient to create the SplitterWnd and the respective views
*
*
*
*Audit Log 
*
*
*Audit Trail
*
*
*/


#include "stdafx.h"
#include "dcl.h"
#include "MainFrm.h"
#include "dclview.h"
#include "dcldialoglist.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/************************************************************************************
/* + CMainFrame::CMainFrame - 
*     Constructor
*
*
*
*
*/

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
}	


/************************************************************************************
/* + CMainFrame::~CMainFrame - 
*  Destructor
*
*
*
*
*/

CMainFrame::~CMainFrame()
	{
	}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
		{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
		}


	return 0;
	}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
	{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
	}


#ifdef _DEBUG
void CMainFrame::AssertValid() const
	{
	CFrameWnd::AssertValid();
	}

void CMainFrame::Dump(CDumpContext& dc) const	
	{
	CFrameWnd::Dump(dc);
	}

#endif //_DEBUG


/************************************************************************************
/* + CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
*	Code to create the static splitter and the views for it
*
*
*
*
*/

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
	{
	// TODO: Add your specialized code here and/or call the base class
	
	if (!m_wndSplitter.CreateStatic(this, 1, 2))
		{
		TRACE0("Failed to CreateStaticSplitter\n");
		return FALSE;
		}
	m_wndSplitter.SetColumnInfo( 0, 100, 0 );
	m_wndSplitter.SetColumnInfo( 1, 500, 0 );
	m_wndSplitter.SetRowInfo( 0, 400, 0 );

	if (!m_wndSplitter.CreateView(0, 0,
    	RUNTIME_CLASS(CDCLDialogList), CSize(250, 25), pContext))
		{
		TRACE0("Failed to create second pane\n");
		return FALSE;
		}
	
	if (!m_wndSplitter.CreateView(0, 1,
      RUNTIME_CLASS(CDCLView), CSize(0, 300), pContext))
		{
		TRACE0("Failed to create second pane\n");
		return FALSE;
		}

	// activate the Dialog View
	SetActiveView((CView*)m_wndSplitter.GetPane(0,1));
	return TRUE;

	}
