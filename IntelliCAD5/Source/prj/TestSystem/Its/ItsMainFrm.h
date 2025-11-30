#pragma once
#include "UiProgressBar.h"
#include "ItsTestRunner.h"

class CItsResultView;
class CItsDoc;

class CItsMainFrame: public CFrameWnd
{
protected: // create from serialization only
	CItsMainFrame();
	DECLARE_DYNCREATE(CItsMainFrame)

public:
	virtual ~CItsMainFrame();
	
	CItsResultView* GetRightPane();

	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnRunTests();
	afx_msg void OnUpdateViewFailures(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewMessages(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewCrashes(CCmdUI* pCmdUI);
	afx_msg void OnViewCrashes();
	afx_msg void OnViewFailures();
	afx_msg void OnViewMessages();
	afx_msg void OnClearView();
	afx_msg void OnTestTerminate();
	afx_msg LRESULT OnAddFailure(WPARAM,LPARAM);
	afx_msg LRESULT OnAddCrash(WPARAM,LPARAM);
	afx_msg LRESULT OnAddMessage(WPARAM,LPARAM);
	afx_msg LRESULT OnProgressStep(WPARAM,LPARAM);
	afx_msg LRESULT OnDisableProgress(WPARAM,LPARAM);
	DECLARE_MESSAGE_MAP()

	CItsDoc* getDoc();

protected:
	CSplitterWnd m_wndSplitter;
	CUiProgressBar  m_wndProgressBar;
	CToolBar    m_wndToolBar;
	CReBar      m_wndReBar;

	CItsTestRunner m_testRunner;

	int m_showStatus;
};

