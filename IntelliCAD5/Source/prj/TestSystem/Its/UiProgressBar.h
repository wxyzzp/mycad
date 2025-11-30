#pragma once
#include <afxext.h>

class CUiProgressBar: public CStatusBar
{
    DECLARE_DYNCREATE(CUiProgressBar)

protected:
    CProgressCtrl m_progress;
    int m_progressLength;
    bool m_progressVisible;

public:
    CUiProgressBar(int = 100);
    virtual BOOL Create(CWnd*, DWORD = WS_CHILD | WS_VISIBLE | CBRS_BOTTOM, UINT = AFX_IDW_STATUS_BAR);

public:
    bool progressBarVisible() const
	{return m_progressVisible;}
    void setProgressLength(int length)
	{m_progressLength = length;}
    bool showProgressBar(bool = true);

    void setRange(int low, int high) 
	{m_progress.SetRange(low,high);}
    int setPos(int nPos)
	{return m_progress.SetPos(nPos);}
    int offsetPos(int nPos)
	{return m_progress.OffsetPos(nPos);}
    int setStep(int nStep)
	{return m_progress.SetStep(nStep);}
    int stepIt()
	{return m_progress.StepIt();}

private:
    void adjustProgressBarPosition();

// Generated message map functions
protected:
	//{{AFX_MSG(CStatusBarWithProgress)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
