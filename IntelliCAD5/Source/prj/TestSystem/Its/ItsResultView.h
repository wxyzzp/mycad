#pragma once

enum
{
	eShowMessages = 1<<0,
	eShowFailures = 1<<1,
	eShowCrashes = 1<<2,
	eShowAll = eShowMessages|eShowFailures|eShowCrashes
};

class CItsResultView: public CEditView
{
protected:
	CItsResultView ();           // protected constructor used by dynamic creation
	virtual ~CItsResultView ();

	DECLARE_DYNCREATE(CItsResultView)

public:
	void updateView(int mode);

protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()

	int m_showStatus;
};
