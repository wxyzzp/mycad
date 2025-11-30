//	IcadRectTracker.h : header file
//	Copyright (C) 1994,1995,1996,1997 by Visio Corporation.
//

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CIcadRectTracker

class CIcadRectTracker : public CRectTracker
{

//*** Member variables
public:

protected:
	CPen* m_pSolidPen;
	CPen* m_pDottedPen;
	HBRUSH m_hatchBrush;

//*** Member functions
public:
	CIcadRectTracker();
	~CIcadRectTracker();

	void Draw(CDC* pDC);

protected:

};
