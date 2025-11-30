#include "stdafx.h"
#include "ItsApp.h"
#include "ItsDoc.h"
#include "ItsMainFrm.h"
#include "ItsTestTree.h"
#include "ItsResultView.h"
#include "ItsResult.h"
#include "testunit/registry.h"
#include "testunit/collection.h"
#include "testunit/test.h"
using namespace testunit;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CItsMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CItsMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND(ID_RUN_TESTS, OnRunTests)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FAILURES, OnUpdateViewFailures)
	ON_UPDATE_COMMAND_UI(ID_VIEW_MESSAGES, OnUpdateViewMessages)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CRASHES, OnUpdateViewCrashes)
	ON_COMMAND(ID_VIEW_CRASHES, OnViewCrashes)
	ON_COMMAND(ID_VIEW_FAILURES, OnViewFailures)
	ON_COMMAND(ID_VIEW_MESSAGES, OnViewMessages)
	ON_COMMAND(ID_CLEAR_VIEW, OnClearView)
	ON_COMMAND(ID_TEST_TERMINATE, OnTestTerminate)
	ON_MESSAGE(ID_ADD_FAILURE, OnAddFailure)
	ON_MESSAGE(ID_ADD_CRASH, OnAddCrash)
	ON_MESSAGE(ID_ADD_MESSAGE, OnAddMessage)
	ON_MESSAGE(ID_PROGRESS_STEP, OnProgressStep)
	ON_MESSAGE(ID_PROGRESS_DISABLE, OnDisableProgress)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR
};

CItsMainFrame::CItsMainFrame()
{
	m_showStatus = eShowAll;
	m_bAutoMenuEnable  = FALSE;
}

CItsMainFrame::~CItsMainFrame()
{}

int CItsMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	if (!m_wndReBar.Create(this) ||
		!m_wndReBar.AddBar(&m_wndToolBar))
	{
		TRACE0("Failed to create rebar\n");
		return -1;      // fail to create
	}
	if (!m_wndProgressBar.Create(this) ||
		!m_wndProgressBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	{
		CRect rect;
		GetClientRect(&rect);
		m_wndProgressBar.setProgressLength(0.5*rect.Width());
	}

	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY);

	m_testRunner.setWindow(m_hWnd);

	return 0;
}

BOOL CItsMainFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/,
	CCreateContext* pContext)
{
	// create splitter window
	if (!m_wndSplitter.CreateStatic(this, 1, 2))
		return FALSE;

	CRect	clientRec;
	GetClientRect(&clientRec);
	if (!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CItsTestTree), CSize(clientRec.Width() / 3, 0), pContext) ||
		!m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CItsResultView), CSize(clientRec.Width() / 3 * 2, 0), pContext))
	{
		m_wndSplitter.DestroyWindow();
		return FALSE;
	}

	return TRUE;
}

CItsResultView* CItsMainFrame::GetRightPane()
{
	CWnd* pWnd = m_wndSplitter.GetPane(0, 1);
	return DYNAMIC_DOWNCAST(CItsResultView, pWnd);
}

static 
unsigned int numberOfTests(test* t)
{
	if (t == 0)
		return 0;

	unsigned int count = 1;

	if (t->has_subtests())
	{
		collection_base* c = static_cast<collection_base*>(t);
		for (test* it = c->first(); it; it = c->next())
			count += numberOfTests(it);
	}

	return count;
}

void CItsMainFrame::OnRunTests() 
{
	using namespace testunit;

	CItsDoc* pDoc = (CItsDoc*)GetRightPane()->GetDocument();
	if (pDoc == NULL)
		return;

	test* t = 0;
	{
		CWnd* pWnd = (CItsTestTree*)m_wndSplitter.GetPane(0, 0);
		CItsTestTree* pTests = DYNAMIC_DOWNCAST(CItsTestTree, pWnd);
		TVITEM item;
		CTreeCtrl& tree = pTests->GetTreeCtrl();
		item.hItem = tree.GetSelectedItem();
		item.mask = TVIF_HANDLE;

		if (!tree.GetItem(&item))
			return;

		t = reinterpret_cast<testunit::test*>(item.lParam);
	}

	if (t == 0)
		return;

	m_wndProgressBar.showProgressBar();
	m_wndProgressBar.setRange(0, numberOfTests(t));
	m_wndProgressBar.setStep(1);
	m_wndProgressBar.setPos(0);

	m_testRunner.run(t);
}

void CItsMainFrame::OnUpdateViewFailures(CCmdUI* pCmdUI) 
{
	if (m_showStatus & eShowFailures)
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);

	pCmdUI->Enable(TRUE);
}

void CItsMainFrame::OnUpdateViewMessages(CCmdUI* pCmdUI) 
{
	if (m_showStatus & eShowMessages)
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);

	pCmdUI->Enable(TRUE);
}

void CItsMainFrame::OnUpdateViewCrashes(CCmdUI* pCmdUI) 
{
	if (m_showStatus & eShowCrashes)
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);

	pCmdUI->Enable(TRUE);
}

void CItsMainFrame::OnViewCrashes() 
{
	m_showStatus ^= eShowCrashes;
	GetRightPane()->updateView(m_showStatus);
}

void CItsMainFrame::OnViewFailures() 
{
	m_showStatus ^= eShowFailures;
	GetRightPane()->updateView(m_showStatus);
}

void CItsMainFrame::OnViewMessages() 
{
	m_showStatus ^= eShowMessages;
	GetRightPane()->updateView(m_showStatus);
}

void CItsMainFrame::OnClearView() 
{
	CItsResultView* view = GetRightPane();
	view->SetWindowText(NULL);
	CItsDoc* doc = (CItsDoc*)view->GetDocument();
	doc->clear();
}

CItsDoc* CItsMainFrame::getDoc()
{
	CItsResultView* view = GetRightPane();
	return (CItsDoc*)view->GetDocument();
}

LRESULT CItsMainFrame::OnAddFailure(WPARAM,LPARAM lp)
{
	failure* f = reinterpret_cast<failure*>(lp);
	getDoc()->addFailure(new CItsFailure(*f));
	return 0;
}

LRESULT CItsMainFrame::OnAddCrash(WPARAM,LPARAM lp)
{
	crash* c = reinterpret_cast<crash*>(lp);
	getDoc()->addCrash(new CItsCrash(*c));
	return 0;
}

LRESULT CItsMainFrame::OnAddMessage(WPARAM,LPARAM lp)
{
	const char* mess = reinterpret_cast<const char*>(lp);
	getDoc()->addMessage(new CItsMessage(mess));
	return 0;
}

LRESULT CItsMainFrame::OnProgressStep(WPARAM,LPARAM)
{
	m_wndProgressBar.stepIt();
	return 0;
}

LRESULT CItsMainFrame::OnDisableProgress(WPARAM,LPARAM)
{
	m_wndProgressBar.showProgressBar(false);
	return 0;
}

void CItsMainFrame::OnTestTerminate()
{
	m_testRunner.stop();
	m_wndProgressBar.showProgressBar(false);
}
