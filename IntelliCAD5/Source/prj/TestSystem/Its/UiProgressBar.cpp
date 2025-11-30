#include "stdafx.H"
#include "UiProgressBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ID_STATUS_PROGRESS  17234

IMPLEMENT_DYNCREATE(CUiProgressBar,CStatusBar)

BEGIN_MESSAGE_MAP(CUiProgressBar,CStatusBar)
	//{{AFX_MSG_MAP(CUiProgressBar)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CUiProgressBar::CUiProgressBar(int length)
:
m_progressLength(length),
m_progressVisible(false)
{}

BOOL CUiProgressBar::Create(CWnd *pParentWnd, DWORD dwStyle, UINT nID)
{
    // Default creation
    BOOL bOK = CStatusBar::Create(pParentWnd,dwStyle,nID);
    if(bOK)
    {
        // Also create the progress bar
        m_progress.Create(WS_CHILD | WS_EX_STATICEDGE | PBS_SMOOTH,CRect(0,0,m_progressLength,10),this,ID_STATUS_PROGRESS);
    }

    return bOK;
}

bool CUiProgressBar::showProgressBar(bool bShow)
{
    // Save old visible status
    bool bOldVisible = m_progressVisible;

    if((bOldVisible != bShow) && ::IsWindow(m_progress.m_hWnd))
    {
        // Show/hide
        m_progress.ShowWindow(bShow ? SW_SHOWNA : SW_HIDE);
        m_progressVisible = bShow;

        // If just shown, make sure it's in the right position
        if(bShow)
        {
            adjustProgressBarPosition();
            RedrawWindow(NULL,NULL,RDW_INVALIDATE | RDW_UPDATENOW);
        }
    }

    return bOldVisible;
}

void CUiProgressBar::adjustProgressBarPosition()
{
    // Make sure the progress bar is created
    if(!::IsWindow(m_progress.m_hWnd))
        return;

    // Find out the size of the text in first pane
    CString str_pane_text;
    CRect   progress_rect;
    GetItemRect(0,progress_rect);
    GetPaneText(0,str_pane_text);
    
    // Measure the size of the text in the first pane
    CClientDC temp(this);
    CFont *pOldFont =temp.SelectObject(GetFont());

    progress_rect.left  =temp.GetTextExtent(str_pane_text).cx + 10;
    progress_rect.right =progress_rect.left + m_progressLength;
    progress_rect.InflateRect(0,-1);

    temp.SelectObject(pOldFont);

    // Now adjust the size of the progrss control
    m_progress.SetWindowPos(NULL,progress_rect.left,
                                 progress_rect.top,
                                 progress_rect.Width(),
                                 progress_rect.Height(),SWP_NOZORDER | SWP_NOACTIVATE);
}

void CUiProgressBar::OnSize(UINT nType, int cx, int cy) 
{
    // Default sizing
	CStatusBar::OnSize(nType, cx, cy);
	
    if(m_progressVisible)
        adjustProgressBarPosition();
}
