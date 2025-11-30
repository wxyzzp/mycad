/** FILENAME:     Slider.h- Contains declarations of class CSlider.
* Copyright  (C) Copyright 1998-99 Visio Corporation.  All Rights Reserved.
*
*
*
* Abstract 
*	this class Maps the basic slider tile
*	to windows control
*
*
*Audit Log 
*
*
*Audit Trail
*
*
*/

#if !defined _SLIDER_H_
#define _SLIDER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ScreenObject.h"


class Slider : public CScrollBar ,public ScreenObject
{

public:
	Slider(); // Construction
	Slider(int Parent_layout);
	virtual int ModeTile ( // Executes functions like setting focus to a control , enabling or disabling a control etc.
		int nTileState	   // i:state of control
		);
		virtual int GetTile (		// Returns the "value" attribute of the tile
		CString & szControlInfo,// o:Date contaned in value attribute
		int lInfoMaxLength	// o: Size of data
		);
	virtual int SetTile (		// Sets the "value" attribute of the tile
		CString szControlKey,	// i:Key of the tile
		CString szControlInfo	// i:Data to store in the value attribute
		);
	virtual int InitFocus();    // set the focus on the control


// Attributes
public:

// Operations
protected:
// Operations derived from base class ScreenObject
	virtual CRect PositionControl(); // finds the actual display area
public:	
	virtual int CreateOnScreenObject ( // Creates the UI objects
		CWnd* pParent				// i:pointer to the parent window
		); 
	virtual int DimensionsTile ( // Returns the size of the control
		int * nWidthLess1,		 // o:width of the Tile
		int * nHeightLess1		 // o:height of the Tile
		); 
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Slider)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~Slider();

	// Generated message map functions
protected:
	int m_bigIncrement; // inctement used in message  pagedown or page up
	int m_smallIncrement; // increment used in message line up or down
	int m_MinVal;			// min value of slider
	int m_MaxVal;			// max value of slider
	int nCurrentPos;		// current position of slider
	//{{AFX_MSG(Slider)
	afx_msg void VScroll(UINT nSBCode, UINT nPos);
	afx_msg void HScroll(UINT nSBCode, UINT nPos);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


#endif 
