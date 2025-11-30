/////////////////////////////////////////////////////////////////////////////
// CIcadPreviewView class
#pragma once

class CIcadPreviewView : public CPreviewView
{
//	DECLARE_DYNCREATE(CIcadPreviewView)
	friend CIcadView;

public:
	void SetToolBar(CDialogBar* pToolBar)
	{
		m_pToolBar=pToolBar;
	}
	void SetPreviewState(CPrintPreviewState* pState)
	{
		m_pPreviewState=pState;
	}
	void PreviewClose()
	{
		OnPreviewClose();
	}
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView,
		CView* pDeactiveView)
	{
		//*** Do not call CPreviewView::OnActivateView() because it tries to set focus
		//*** to AFX_ID_PREVIEW_PRINT which we don't have.
		if (bActivate)
		{
			CWnd* pFocusWnd = GetFocus();
			if (pFocusWnd == NULL ||
				(m_pToolBar != NULL && !m_pToolBar->IsChild(pFocusWnd)))
			{
				// focus is not already on a toolbar button - set it to one
//				m_pToolBar->GetDlgItem(IDC_PRNPREV_PRINT)->SetFocus();
			}
		}
	}
protected:

private:

};

//IMPLEMENT_DYNCREATE(CIcadPreviewView,CPreviewView)
