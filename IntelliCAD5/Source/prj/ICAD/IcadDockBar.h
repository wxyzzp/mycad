//	IcadDockBar.h : header file
//	Copyright (C) 1994,1995,1996,1997 by Visio Corporation.
//

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CIcadDockBar

class CIcadDockBar : public CDockBar
{

//*** Member variables
public:

protected:

//*** Member functions
public:
	CIcadDockBar(BOOL bFloating = FALSE) : CDockBar(bFloating) {}
	~CIcadDockBar() {}
	void DockControlBar(CControlBar* pBar, LPCRECT lpRect = NULL);
	int Insert(CControlBar* pBarIns, CRect rect, CPoint ptMid);
	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);

protected:

};
