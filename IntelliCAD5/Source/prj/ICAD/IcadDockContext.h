//	IcadDockContext.h : header file
//	Copyright (C) 1994,1995,1996,1997 by Visio Corporation.
//

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CIcadDockContext

class CIcadDockContext : public CDockContext
{

//*** Member variables
public:

protected:

//*** Member functions
public:
	CIcadDockContext(CControlBar* pBar) : CDockContext(pBar) {}
	~CIcadDockContext() {}

	virtual void StartDrag(CPoint pt);
	BOOL Track();
	void EndDrag();

protected:

};
