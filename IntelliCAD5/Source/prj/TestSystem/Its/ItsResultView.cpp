#include "stdafx.h"
#include "ItsApp.h"
#include "ItsResultView.h"
#include "ItsDoc.h"
#include "ItsResult.h"
#include <string>
#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CItsResultView, CEditView)

CItsResultView::CItsResultView()
:m_showStatus(eShowAll)
{
}

CItsResultView::~CItsResultView()
{
}


BEGIN_MESSAGE_MAP(CItsResultView, CEditView)
	ON_WM_CREATE()
END_MESSAGE_MAP()

void CItsResultView::OnDraw(CDC* pDC)
{
	CItsDoc* pDoc = (CItsDoc*)GetDocument();

	const std::vector<CItsResult*>& results = pDoc->getResults();

	std::string text;
	std::vector<CItsResult*>::const_iterator r = results.begin();
	for (; r != results.end(); ++r)
	{
		text += (*r)->toString();
	}

	GetEditCtrl().SetWindowText(text.c_str());
}

void CItsResultView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	// TODO: Add your specialized code here and/or call the base class
	CItsDoc* pDoc = (CItsDoc*)GetDocument();

	const std::vector<CItsResult*>& results = pDoc->getResults();

	std::string text;
	std::vector<CItsResult*>::const_iterator r = results.begin();
	for (; r != results.end(); ++r)
	{
		if (((m_showStatus & eShowFailures) && dynamic_cast<CItsFailure*>(*r)) ||
			((m_showStatus & eShowCrashes) && dynamic_cast<CItsCrash*>(*r)) ||
			((m_showStatus & eShowMessages) && dynamic_cast<CItsMessage*>(*r)))
			text += (*r)->toString();
	}

	GetEditCtrl().SetWindowText(text.c_str());
}

int CItsResultView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CEditView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	GetEditCtrl().SetReadOnly();
	
	return 0;
}

void CItsResultView::updateView(int mode)
{
	m_showStatus = mode;
	OnUpdate(0, 0, 0);
}
