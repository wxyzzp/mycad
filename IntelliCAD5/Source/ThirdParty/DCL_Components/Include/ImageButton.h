/** FILENAME:     ImageButton.h- Contains declarations of class ImageButton.
* Copyright  (C) Copyright 1998-99  Visio Corporation.  All Rights Reserved.
*
*
*
* Abstract
*      This class implements the image and image button functinalites.
*
*
* Audit Log
*
*
* Audit Trail
*
*
*/


#if !defined  _IMAGEBUTTON_H_
#define _IMAGEBUTTON_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ImageButton.h : header file
//

#include "ScreenObject.h"


/////////////////////////////////////////////////////////////////////////////
// ImageButton window


class ImageButton : public CButton,public ScreenObject
{
// Construction
public:

	// Constructors.
	ImageButton();

	ImageButton(int nType,				// i: Type of tile:image or image_button.
				int nParentAlignment    // i: Aligment required.
				);


// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ImageButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~ImageButton();

public:
   void Start();

	// Executes functions like setting focus to a control , enabling or disabling a control etc.
	int ModeTile (int nTileState);		// i : state of control

	// Draws a slide in the currently acive image.
	int DrawSlideImage (							
					int nULCornerX,				// i : X coordinate of Upper Left Corner.
					int nULCornery,				// i : Y coordinate of Upper Left Corner.
					int nWidthLess1,			// i : Width relative from the Upper Left Corner.
					int nHeightLess1,			// i : Height relative from the Upper Left Corner.
					LPTSTR szSnapshotName		// i : Slide name or the library.
					);

	// Draw a fill image for the given co-ordinates.
	int DrawFillImage (                    
					int nULCornerX,        // i: Start position in X-cordinate.
					int nULCornerY,        // i: Start position in Y-cordinate.
					int nWidthLess1,       // i: End position in X-cordinate.
					int nHeightLess1,      // i: End position in Y-cordinate.
					int nColor             // i: Color of the fill image.
					);

	// Draw a vector image for the given co-ordinates.
	int DrawVectorImage (                  
					int nXVectStart,	   // i: Start position in X-cordinate.
					int nYVectStart,	   // i: Start position in Y-cordinate.
					int nXVectEnd,		   // i: End position in X-cordinate.
					int nYVectEnd,		   // i: End position in Y-cordinate.
					int nColor             // i: Color of the fill image.
					);

	// Creates the UI objects
	virtual int CreateOnScreenObject (CWnd* pParent);	// i:pointer to the parent window


// Attributes
private:
		int m_nType;				// parameter which discribes the type of tile (image,image_button)
		CString  m_wszColor;        // The current color of the tile.
		int m_nColor;               // The current color of the tile.

		//Modified Cybage SBD 17/09/2001 DD/MM/YYYY 
		//Reason : Fix for bug no.77838 from Bugzilla.
	BOOL m_bFlipCurrent;

	    HDC m_dc;
		HBITMAP m_hBitmap;


private:
		void SetDefaultRectangle (HDC hDC, CRect rRect);

	// Returns a flip color .
	COLORREF GetFlipColor(CString wszColor);    // i: Color to be flipped.

	void SetBoundingRectangle(
			int& nULCornerX,
			int& nULCornerY,
			int& nWidthLess1,
			int& nHeightLess1
			);

	COLORREF GetFocalColor(COLORREF clrPixelColor);
	
	// Returns the COLORREF value.
	COLORREF GetColor(CString &wszColor);		// i:Value	as CString
				

	  // Generated message map functions
protected:
	//{{AFX_MSG(ImageButton)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDoubleclicked();
	//}}AFX_MSG

   CPoint m_pointClicked;

    // The color of the focus rectangle when the tile is having the focus.
	COLORREF focalColor;                  

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMAGEBUTTON_H__97BCFC27_222E_11D3_8815_0080C8006398__INCLUDED_)

