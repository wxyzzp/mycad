/** FILENAME:     ImageButton.cpp- Contains implementation of class ImageButton.
* Copyright  (C) Copyright 1998-99 Visio Corporation.  All Rights Reserved.
*
*
*
* Abstract
*       This file has all the implementation of the ImageButton class
*
*
* Audit Log
*
*
* Audit Trail
*
*
*/


#include "stdafx.h"
#include "ImageButton.h"
#include "sds.h"
#include "dclprivate.h"
#include "constants.h"
#include "dialog.h"
#include <math.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


BOOL bFlip = FALSE;

int bReason = 0;




/************************************************************************************
/* + ImageButton::ImageButton() -
*
*
*
*
*/
ImageButton::ImageButton()
	{
	//Modified Cybage SBD 17/09/2001 DD/MM/YYYY 
	//Reason : Fix for bug no.77838 from Bugzilla.
	m_bFlipCurrent = FALSE;

	m_dc = NULL;
	m_hBitmap	= NULL;

    }

/************************************************************************************
/* + ImageButton::~ImageButton() -
*
*
*
*/
ImageButton::~ImageButton()
	{

	if (m_dc != NULL)
	{
		::DeleteDC(m_dc);
		m_dc = NULL;
	}
	
	if (m_hBitmap != NULL)
	{
		::DeleteObject(m_hBitmap);
		m_hBitmap = NULL;
	}

}


BEGIN_MESSAGE_MAP(ImageButton, CButton)
	//{{AFX_MSG_MAP(ImageButton)

	ON_CONTROL_REFLECT(BN_DOUBLECLICKED, OnDoubleclicked)

	// Modified CyberAge PP 10/18/2000 Reason: Fix for bug no. 73998
	ON_WM_LBUTTONDOWN()

	//}}AFX_MSG_MAP
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

// Modified CyberAge PP 10/18/2000 [ Reason: Fix for bug no. 73998
afx_msg void ImageButton::OnLButtonDown(UINT nFlags, CPoint point)
{
    m_pointClicked = point;

	// TODO: Add your control notification handler code here
	// This event is trapped only when the DCL Dll is used.
#ifndef _TESTEXE
	try
		{
		if ( this->m_nType = IMAGEBUTTON_TILE )
			{
			TRACE("OnClicked of ImageButton called\n");
			bReason = 4;

			SetFocus();//2000-10-26  EBATECH(FUTA)
			// Get the parent of the tile.
			Dialog * pParent = static_cast<Dialog *>(GetParent());

			// the below code is used to fill the callback packet which is sent in the call back function
			if(pParent->m_IsModal )
				{
				// Set the call back packet.
				sds_callback_packet *cpkt = NULL;
				cpkt = GetCallBackPacket();
				cpkt->dialog = ((Dialog*)GetParent())->handle;
				cpkt->reason = SDS_CBR_SELECT ;

				char strValue[2] = "1";
				cpkt->value = strValue;

				// Get the DCL action attribute if set.
				ScreenObject *pScreen =(ScreenObject *)this;
				CString strAction = pScreen->m_attriblist->get_Action();

// Modified CyberAge PP 10/18/2000 [ Reason: Fix for bug no. 73998
				cpkt->x = m_pointClicked.x ;
				cpkt->y = m_pointClicked.y ;
// Modified CyberAge PP 10/18/2000 ]

				// call the call back function if call back is set
				if( m_FunctionPtr != NULL )
					{
					m_FunctionPtr(cpkt);
					}
				else if ( strAction.IsEmpty() == FALSE )
					{
					// If DCL action attribute was set, then execute the lisp expression set.
					pParent->m_DefCallBack(cpkt);
					}
				else if ( pParent->m_DefCallBack != NULL )
					{
					// Default call back...handled by IntelliCAD.
					pParent->m_DefCallBack(cpkt);
					}
				delete cpkt->tile;
				delete cpkt;
				}
			}
		}
	catch(...)
		{
		TRACE("Error while calling call back function for Image button click event\n");
		}
#endif

}
// Modified CyberAge PP 10/18/2000 ]

/************************************************************************************
/* + ImageButton::ImageButton(int nType, int nParentAlignment) -
*
*    Constructor of Image Button
*
*
*
*/
ImageButton::ImageButton(                                         // Constructor
						 int nType,                               // Type of tile (iamge or image_button).
						 int nParentAlignment
						 )
	{
    this->m_ParentAlignment = nParentAlignment;
	this->m_nType = nType;
	//Modified Cybage SBD 17/09/2001 DD/MM/YYYY 
	//Reason : Fix for bug no.77838 from Bugzilla.
	m_bFlipCurrent = FALSE;

	m_dc = NULL;
	m_hBitmap	= NULL;

    }



/************************************************************************************
/* + int ImageButton::CreateOnScreenObject (CWnd* pParent); -
*
*    Creates the windows UI control.This function is a virtual function in the
*   ScreenObjects class.
*
*/
int ImageButton::CreateOnScreenObject(                       // Create the UI control.
									  CWnd* pParent          // i: The pointer to the container control.
									  )
	{

	// Set the buttons styles.
	DWORD dwStyle = WS_CHILD|WS_VISIBLE|BS_OWNERDRAW|WS_CLIPSIBLINGS|WS_EX_TOPMOST|WS_EX_TRANSPARENT|WS_GROUP|CS_DBLCLKS;

	if ( m_nType == IMAGEBUTTON_TILE )
		{
		dwStyle |= BS_PUSHBUTTON;
		}

	if ( !m_attriblist->IsEnabled() )
		{
		// set style to enable
		dwStyle |= WS_DISABLED;
		}

	if ( m_attriblist->IsTabstop() )
		{
		// set style to tabstop
		dwStyle |=  WS_TABSTOP;
		}

	if ( m_nType == IMAGE_TILE )
		{

		dwStyle |= WS_DISABLED ;

		}

	CRect rect ;									// Rect variable to hold the actual display rect of the control.
	rect = PositionControl();						// function to calculate the actual display rect.

	CString Label;
	Label = SetMnemonic();							// Set the Mnemonic string.

	BOOL bReturn = Create(Label, dwStyle, rect, pParent,m_controlID.GetID());		// creates the UI contol

	return SDS_RTNORM;
	}



/************************************************************************************
/* + void ImageButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)  -
*
*    This is a overloaded function for an OWNERDRAWN button.The framework calls this
*   function whenever this control  has to be overdrawn.
*
*
*/
void ImageButton::DrawItem(										// This method draws the image tile.
						   LPDRAWITEMSTRUCT lpDrawItemStruct    // i: This structure is passed by the framework.
						   )
{
    // TODO: Add your code to draw the specified item

	// Get the clipping rectangle.
	CRect rect;
    rect = 	lpDrawItemStruct->rcItem ;

	int nPrevMode;

	if (m_hBitmap != NULL)
	{
		// If we are here, it means you have already called start_image, 
		// therefore you have drawn something in the bitmap, so, just 
		// do not draw anything else.

		if (bFlip == m_bFlipCurrent)
		{
			::BitBlt(lpDrawItemStruct->hDC, 
					 rect.left, rect.top, 
					 rect.right - rect.left, rect.bottom - rect.top, 
					 m_dc, rect.left, rect.top, SRCCOPY);
		}
		else
		{
			::BitBlt(lpDrawItemStruct->hDC, 
					 rect.left, rect.top, 
					 rect.right - rect.left, rect.bottom - rect.top, 
					 m_dc, rect.left, rect.top, NOTSRCCOPY);
		}
	}
	else
	{
		if (bFlip != m_bFlipCurrent)
			nPrevMode = ::SetROP2(lpDrawItemStruct->hDC, R2_NOTCOPYPEN);

		SetDefaultRectangle (lpDrawItemStruct->hDC, rect);

		if ( bFlip != m_bFlipCurrent )
			::SetROP2(lpDrawItemStruct->hDC, nPrevMode);
	}


	// Now draw the focus rectangle, if the window has the focus.
	if ( (ImageButton*)GetFocus()  == this && this->m_nType == IMAGEBUTTON_TILE )
	{
		// a focus rectangle is shown by setting alternate pixels.
		COLORREF clrFocusPixel;

		// From left to right(top of rectangle).
		for (int iLoop = rect.left + 1; iLoop <= rect.right - 1; iLoop = iLoop + 2)
		{
			clrFocusPixel = GetPixel(lpDrawItemStruct->hDC, iLoop, rect.left + 1);
			clrFocusPixel = GetFocalColor(clrFocusPixel);
			SetPixel(lpDrawItemStruct->hDC, iLoop, rect.left + 1, clrFocusPixel);
		}

		// From  top to bottom(left side of rectangle).
		for (iLoop = rect.top + 2; iLoop <= rect.bottom - 1; iLoop = iLoop + 2)
		{
			clrFocusPixel = GetPixel(lpDrawItemStruct->hDC, rect.top + 1,iLoop);
			clrFocusPixel = GetFocalColor(clrFocusPixel);
			SetPixel(lpDrawItemStruct->hDC, rect.top + 1, iLoop, clrFocusPixel);
		}

		// From left to right ( bottom of rectangle).
		for (iLoop = rect.left + 2; iLoop < rect.right - 2; iLoop = iLoop + 2)
		{
			clrFocusPixel = GetPixel(lpDrawItemStruct->hDC, iLoop, rect.bottom - 2);
			clrFocusPixel = GetFocalColor(clrFocusPixel);
			SetPixel(lpDrawItemStruct->hDC, iLoop, rect.bottom - 2, clrFocusPixel);
		}

		// From top to bottom ( right side of rectangle).
		for (iLoop = rect.top+2; iLoop < rect.bottom - 2; iLoop = iLoop + 2)
		{
			clrFocusPixel = GetPixel(lpDrawItemStruct->hDC, rect.right - 2, iLoop);
			clrFocusPixel = GetFocalColor(clrFocusPixel);
			SetPixel(lpDrawItemStruct->hDC, rect.right - 2, iLoop, clrFocusPixel);
		}

	}

	TRACE("******DRAWITEM CALLED*****\n");
}



/************************************************************************************
/* + COLORREF ImageButton::GetColor(CString &wszColor) -
*
*      Returns the corresponding COLOREF value for the  value set in the DCL file.
*
*
*
*/
COLORREF ImageButton::GetColor(                          // Returns the COLORREF value.
							   CString &wszColor         // i:Value as CString
							   )
	{

	CDC *pDC = ((CDialog*)GetParent())->GetDC();

	// These conditional structure returns the COLORREF value
	// corresponding to the string.

	// The focal value is also set, acording to the fill color.

    if ( atoi((LPCTSTR)wszColor) )
		{
		long lColor = atoi((LPCTSTR)wszColor);
		wszColor.Format("%d",lColor);
		TRACE("Color :::::%s\n",wszColor);
		}

	if ( wszColor == _T("dialog_line") )
		{
		LOGPEN logPen;
		CPen *pCurrPen = pDC->GetCurrentPen() ;
		pCurrPen->GetLogPen(&logPen);
		focalColor = RGB(255,255,255);
		return (logPen.lopnColor);
        }
	else if ( wszColor == _T("dialog_foreground") )
		{
		focalColor = RGB(255,255,255);
		return pDC->GetTextColor();
		}
	else if ( wszColor == _T("dialog_background") )
		{
		focalColor = RGB(255,255,255);
		return pDC->GetBkColor();
		}
	else if ( wszColor == _T("graphics_background") || wszColor == _T("-2") )
		{

		focalColor = RGB(255,255,255);

		#ifndef _TESTEXE
		HMODULE   hmodule = GetModuleHandle(NULL);
		ASSERT(hmodule);
		if(hmodule)
			{
			// Get the long value of background color.
			typedef int (* SDS_GETVAR)(const char *szSysVar,struct sds_resbuf*);
			SDS_GETVAR pfnsdsgetvar;
			int nBackColor;

			pfnsdsgetvar = (SDS_GETVAR)GetProcAddress(hmodule,"sds_getvar");
			ASSERT(pfnsdsgetvar);
			sds_resbuf rb;

			int nRet = pfnsdsgetvar(_T("BKGCOLOR"),&rb);

			if ( nRet == SDS_RTNORM )
				{
				nBackColor = rb.resval.rlong;
				}

			typedef int  (* getrgbvalue)(int nColor);
			getrgbvalue pfnGetRGBValue;
			pfnGetRGBValue = ( getrgbvalue)GetProcAddress(hmodule,"sds_getrgbvalue");
			ASSERT(pfnGetRGBValue);
			//Modified Cybage SBD 06/08/2001 DD/MM/YYYY [
			//Reason : Fix for Bug No. 77819 from Bugzilla
			//return 	RGB(255,255,255) -  pfnGetRGBValue(nBackColor);
			if(nBackColor==256) //black color
				return pfnGetRGBValue(nBackColor);
			else
				return RGB(255,255,255) -  pfnGetRGBValue(nBackColor);
			//Modified Cybage SBD 06/08/2001 DD/MM/YYYY ]

		}
		#endif
		return RGB(0,0,0);
		}
	else if ( wszColor == _T("black") || wszColor == _T("0") )
		{
		focalColor = RGB(255,255,255);
        return RGB(0,0,0);
		}
	else if ( wszColor == _T("red") || wszColor == _T("1"))
		{
		focalColor = RGB(255,255,0);
		return RGB(255,0,0);
		}
	else if ( wszColor == _T("yellow") || wszColor == _T("2"))
		{
		focalColor = RGB(255,255,255);
        return RGB(255,255,0);
		}
	else if ( wszColor == _T("green") || wszColor == _T("3"))
		{
		focalColor = RGB(0,0,0);
        return RGB(0,255,0);
		}
	else if ( wszColor == _T("cyan")  || wszColor == _T("4") )
		{
		focalColor = RGB(255,255,255);
		return RGB(0,255,255);
		}
	else if ( wszColor == _T("blue") || wszColor == _T("5") )
		{
		focalColor = RGB(255,255,255);
		return RGB(0,0,255);
		}
	else if ( wszColor == _T("magenta")  || wszColor == _T("6") )
		{
        focalColor = RGB(255,50,255);
		return RGB(255,0,255);
		}
	else if ( wszColor == _T("white")  || wszColor == _T("7") )
		{
        return RGB(255,255,255);
		}
	else if ( atoi((LPCTSTR)wszColor) != 0 )
		{
		// In case an integer value is given which is greater than above colors, then
		// Intellicad's "sds_getrgbvalue" function is called to get the
		// corresponding value for the integer.
		#ifndef _TESTEXE
		HMODULE   hmodule = GetModuleHandle(NULL);
		ASSERT(hmodule);
		if(hmodule)
			{
			typedef int  (* getrgbvalue)(int nColor);
			getrgbvalue pfnGetRGBValue;
			pfnGetRGBValue = ( getrgbvalue)GetProcAddress(hmodule,"sds_getrgbvalue");
			ASSERT(pfnGetRGBValue);
			//Modified Cybage SBD 06/08/2001 DD/MM/YYYY [
			//Reason : Fix for Bug No. 77819 from Bugzilla
			//return 	 RGB(255,255,255) - pfnGetRGBValue(atoi((LPCTSTR)wszColor));
			// Get the long value of background color.
			typedef int (* SDS_GETVAR)(const char *szSysVar,struct sds_resbuf*);
			SDS_GETVAR pfnsdsgetvar;
			int nBackColor;

			pfnsdsgetvar = (SDS_GETVAR)GetProcAddress(hmodule,"sds_getvar");
			ASSERT(pfnsdsgetvar);
			sds_resbuf rb;

			int nRet = pfnsdsgetvar(_T("BKGCOLOR"),&rb);

			if ( nRet == SDS_RTNORM )
			{
				nBackColor = rb.resval.rlong;
			}
			int nColor=atoi((LPCTSTR)wszColor) ;

			if( nColor > 256 ) //when value more than 256 show backgroundcolor as shown by AutoCAD
				return RGB(255,255,255) -  pfnGetRGBValue(nBackColor);
			else if( nColor== 256 ) //when black color
				return 	 RGB(0,0,0);
			else if( nColor== nBackColor ) //if set value equals the background color
				return 	 RGB(255,255,255) - pfnGetRGBValue(nColor);
			else
				return pfnGetRGBValue(nColor);
			//Modified Cybage SBD 06/08/2001 DD/MM/YYYY ]
		}
		#endif
		return RGB(255,255,255);
		}
	  else
		{
		// If an invalid value is present , then set the tile to a default color of
		// white.
		return RGB(255,255,255);
		}

	  ReleaseDC (pDC);
	  pDC = NULL;

	}



/************************************************************************************
/* + int ImageButton::DrawVectorImage(int nXVectStart, int nYVectStart, int nXVectEnd, int nYVectEnd, int nColor) -
*
*    This method draws a vector image.
*
*/
int ImageButton::DrawVectorImage(                               // Draw a vector image for the given co-ordinates.
								 int nXVectStart,               // i: Start position in X-cordinate.
								 int nYVectStart,				// i: Start position in Y-cordinate.
								 int nXVectEnd,					// i: End position in X-cordinate.
								 int nYVectEnd,					// i: End position in Y-cordinate.
								 int nColor						// i: Color of the fill image.
								 )
	{

	m_bFlipCurrent = FALSE;

	CString wszColor;
	wszColor.Format("%d",nColor);

	// Get the COLORREF equivalent of the color value passed.
	COLORREF clrTemp = focalColor;
  	COLORREF color = GetColor(wszColor);
	focalColor = clrTemp;

    HPEN hOldPen1;                    // Create pens.

	// Create a pen with the given color value.
	HPEN hPen = CreatePen(PS_SOLID,1,color);   

	// Select the pen to the metafile DC.
    hOldPen1= (HPEN)SelectObject(m_dc,hPen);

   TRACE("ImageButton::DrawVectorImage1.hOldPen1=%d\n",hOldPen1);

	// Draw the line to the metafile.
    MoveToEx(m_dc,nXVectStart,nYVectStart,NULL);
	LineTo(m_dc,nXVectEnd,nYVectEnd);

	// Select the old pen to the metafile.
    TRACE("ImageButton::DrawVectorImage2.hOldPen1=%d\n",hOldPen1);
    ::SelectObject(m_dc, hOldPen1);
    ::DeleteObject(hPen);

	//Modified Cybage SBD 03/12/2001 DD/MM/YYYY [
	//Reason : Fix for bug no.77941 from Bugzilla.
	RECT Rect;
	this->GetClientRect(&Rect);
	InvalidateRect(&Rect,TRUE);
	//Modified Cybage SBD 03/12/2001 DD/MM/YYYY ]

   	return SDS_RTNORM;

	}


/************************************************************************************
/* + int ImageButton::DrawFillImage(int nULCornerX, int nULCornerY, int nWidthLess1, int nHeightLess1, int nColor) -
*
*    This method does the drawing of the fill image to the control.
*
*
*
*/
int ImageButton::DrawFillImage						// Draw a fill image for the given co-ordinates.
							(int nULCornerX,		// i: Start position in X-cordinate.
							int nULCornerY,			// i: Start position in Y-cordinate.
							int nWidthLess1,		// i: End position in X-cordinate.
							int nHeightLess1,		// i: End position in Y-cordinate.
							int nColor				// i: Color of the fill image.
							)
	{

	m_bFlipCurrent = FALSE;
	
	CString wszColor;
	wszColor.Format("%d",nColor);

    // Get the COLORREF equivalent of the color value passed.
	COLORREF clrTemp = focalColor;
  	COLORREF color = GetColor(wszColor);

    focalColor = clrTemp;

	// Create a brush with the given color value.
	HBRUSH hBrush = CreateSolidBrush(color);

	// Select the brush to the metafile DC.
	HBRUSH hOldBrush1 = (HBRUSH)SelectObject(m_dc,hBrush);

	HPEN hPen = CreatePen(PS_SOLID,0,color);
	HPEN hOldPen = (HPEN)SelectObject(m_dc,hPen);

	SetBoundingRectangle(nULCornerX,
							nULCornerY,
							nWidthLess1,
							nHeightLess1);

    Rectangle( m_dc,nULCornerX,
                            nULCornerY,
							//2000-11-02 EBATECH(FUTA)[
							//nWidthLess1,
							//nHeightLess1);
							nULCornerX+nWidthLess1,
							nULCornerY+nHeightLess1);////2000-11-02 EBATECH(FUTA)]


	RECT Rect = { nULCornerX,
					  nULCornerY,
					  nWidthLess1,
					  nHeightLess1 };

	InvalidateRect(&Rect,TRUE);

    ::DeleteObject((HBRUSH)::SelectObject(m_dc, hOldBrush1));
    ::DeleteObject((HPEN)::SelectObject(m_dc, hOldPen));

    return SDS_RTNORM;

	}


/************************************************************************************
/* + COLOREF ImageButton::GetFocalColor(COLOREF clrPixelColor) -
*
*     This returns the color for the focus rectangle.
*
*
*
*/
COLORREF ImageButton::GetFocalColor(COLORREF clrPixelColor)
{

   if ( clrPixelColor == RGB(255,0,0) )
	   {
	   return RGB(0,255,0);
	   }
   else if ( clrPixelColor == RGB(255,255,0) )
	   {
	   return RGB(255,255,255);
	   }
   else if ( clrPixelColor == RGB(0,255,0) )
	   {
	   return RGB(0,0,0);
	   }
   else if ( clrPixelColor == RGB(0,255,255) )
	   {
	   return RGB(10,255,255);
	   }
   else if ( clrPixelColor == RGB(0,0,255) )
	   {
	   return RGB(255,255,255);
	   }
   else if ( clrPixelColor == RGB(255,0,255) )
	   {
	   return RGB(255,50,255);
	   }
   else if ( clrPixelColor == RGB(255,255,255) )
	   {
	   return RGB(10,10,10);
	   }
   else if ( clrPixelColor == RGB(0,0,0) )
	   {
	   return RGB(255,255,255);
	   }
   else
	   {
	   return RGB(0,0,0);
	   }

}




/*********************************************************************************************************************
/* void ImageButton::SetBoundingRectangle(int intnULCornerX,int nULCornerY,int nWidthLess1,int nHeightLess1 );
*
*		The co-ordiantes given for a drawing operation are validated , so that the draw operations do not
*  extend beyond the tile boundary.
*
*
*
*/
void ImageButton::SetBoundingRectangle(	int &nULCornerX,     // i: X-Co-ordinate.
										int &nULCornerY,     // i: Y-Co-ordinate.
										int &nWidthLess1,    // i: Width.
										int &nHeightLess1    // i: Hieght.
										)
	{

     CRect rectTileDim = PositionControl();

	 // Get the X,Y co-orduinate extents

     // Get the height and width of tile.
	 int nExtX = rectTileDim.right - rectTileDim.left ;
	 int nExtY = rectTileDim.bottom - rectTileDim.top ;


	 // See whether the x-co-ordinates are greater than the extreme end.
	 if ( nULCornerX > nExtX )
		 {
		 nULCornerX = nExtX ;
		 }

	 if ( nULCornerX <  0 )
		 {
		 nULCornerX = 0;
		 }

	 // See whether the y-co-ordinates are greater than the extreme end.
	 if ( nULCornerY > nExtY )
		 {
		 nULCornerY = nExtY  ;
		 }

	 if ( nULCornerY <  0 )
		 {
		 nULCornerY = 0;
		 }


	 // right and bottom co-ordiantes

	 if ( nWidthLess1 > nExtX )
		 {
		 nWidthLess1 = nExtX + 1;
		 }
	 else
		 {
          nWidthLess1 ++ ;
		 }

	 if ( nULCornerX <  0 )
		 {
		 nWidthLess1 = 0;
		 }

	 if ( nHeightLess1 > nExtY )
		 {
		 nHeightLess1 = nExtY + 1;
		 }

	 if ( nHeightLess1  <  0 )
		 {
		 nHeightLess1  = 0;
		 }
	 else
		 {
          nHeightLess1++;
		 }



	 TRACE("x1=%d,y1=%d,x2=%d,y2=%d\n",nULCornerX,
									 nULCornerY,
									 nWidthLess1,
									 nHeightLess1 );

	 TRACE("x1=%d,y1=%d,x2=%d,y2=%d\n",rectTileDim.left,
									 rectTileDim.top,
									 rectTileDim.right,
									 rectTileDim.bottom
									 );


	}



/************************************************************************************************
/* + int ImageButton::DrawSlideImage ( )
*
*      This function is implemented for drawing slide images on the image/image_button control.
*
*
*
*/
int ImageButton::DrawSlideImage (							// Draws a slide in the currently acive image.
								int nULCornerX,				// i : X coordinate of Upper Left Corner.
								int nULCornery,				// i : Y coordinate of Upper Left Corner.
								int nWidthLess1,			// i : Width relative from the Upper Left Corner.
								int nHeightLess1,			// i : Height relative from the Upper Left Corner.
								LPTSTR szSnapshotName		// i : Slide name or the library.
								)
	{

	m_bFlipCurrent = FALSE;

	CClientDC dc(this);
	RECT rect2DisplaySlideIn;

	// Set the rect boundary to in which the slide has to be drawn.
	rect2DisplaySlideIn.left = nULCornerX;
	rect2DisplaySlideIn.top = nULCornery;
	rect2DisplaySlideIn.right = nWidthLess1;
	rect2DisplaySlideIn.bottom = nHeightLess1;

	// When the Dll s used this section draws a slide.
	#ifndef _TESTEXE
	HMODULE   hmodule = GetModuleHandle(NULL);
	ASSERT(hmodule);
	if(hmodule)
		{
		typedef int (* ic_drawslideimage)(char*,HDC,RECT*) ;
		ic_drawslideimage pfnDrawSlide ;
		pfnDrawSlide = (ic_drawslideimage)GetProcAddress(hmodule,"IC_DrawSlideFile");
		ASSERT(pfnDrawSlide);

		// Call IntelliCAD's IC_DrawSlideFile to draw the slide image.
		if ( pfnDrawSlide(szSnapshotName, m_dc, &rect2DisplaySlideIn ) == SDS_RTERROR )
            {
			TRACE("Error: Drawslide image failed\n");
			}
		}

	#endif

	InvalidateRect(&rect2DisplaySlideIn,TRUE);

	return SDS_RTNORM;

	}



/************************************************************************************
/* + BOOL ImageButton::PreCreateWindow(CREATESTRUCT& cs)  -
*
*     This method is used to set certain window properties before the window creation.
*
*
*
*/
BOOL ImageButton::PreCreateWindow(                                      // MFC Framework method called prior to window creation.
								  CREATESTRUCT& cs                      // i: Structure  used to set window preoperties.
								  )
	{
	// TODO: Add your specialized code here and/or call the base class


    // This style is set, to enable this button get the paint event at the end, since the current tile
	// is an owner drawn object.
    cs.dwExStyle |= WS_EX_TOPMOST | WS_EX_TRANSPARENT;

	return CButton::PreCreateWindow(cs);
	}



/***********************************************************************************************************************
/* + void ImageButton::OnDoubleclicked()  -
*
*       Called when a user double clicks on this tile.This method in turn calls the lisp call back functions after
*   setting the callback packet values.
*
*
*/
void ImageButton::OnDoubleclicked()
	{

	// TODO: Add your control notification handler code here
	// This event is trapped only when the DCL Dll is used.


#ifndef _TESTEXE

	try
		{
		if ( this->m_nType = IMAGEBUTTON_TILE )
			{
			TRACE	("&&&&OnDoubleClick\n");
			bReason = 4;

		    // Get the parent of the tile.
			Dialog * pParent = static_cast<Dialog *>(GetParent());

			// The below code is used to fill the callback packet which is sent in the call back function
			if(pParent->m_IsModal )
				{
				// Set the call back packet.
				sds_callback_packet *cpkt = NULL;
				cpkt = GetCallBackPacket();
				cpkt->dialog = ((Dialog*)GetParent())->handle;
				cpkt->reason = SDS_CBR_DOUBLE_CLICK ;
//4M Item:4->
            cpkt->x=m_pointClicked.x;
            cpkt->y=m_pointClicked.y;
//<-4M Item:4

                char strValue[2] = "1";
				cpkt->value = strValue;

				// Get the DCL action attribute if set.
				ScreenObject *pScreen =(ScreenObject *)this;
				CString strAction = pScreen->m_attriblist->get_Action();

				// call the call back function it call back is set
				if(m_FunctionPtr != NULL)
					{
					m_FunctionPtr(cpkt);
					}
				else if ( strAction.IsEmpty() == FALSE )
					{
					// If DCL action attribute was set, then execute the lisp expression set.
					pParent->m_DefCallBack(cpkt);
					}
				else if(pParent->m_DefCallBack != NULL)
					{
					// Default call back...handled by IntelliCAD.
					pParent->m_DefCallBack(cpkt);
					}
				delete cpkt->tile;
				delete cpkt;
				}
			}
		}
	catch(...)
		{
		TRACE("Error while calling the call back function for image button double click event\n");
		}
#endif

	}



/************************************************************************************
/* + int ImageButton::ModeTile (int nTileState)
*
*	Executes functions like setting focus to a control , enabling or disabling a control
*	nTileState = 0  then enable window
*	nTileState = 1	disable window
*	nTileState = 2  set Focus
*/

int ImageButton::ModeTile ( // Executes functions like setting focus to a control , enabling or disabling a control etc.
		int nTileState	   // i:state of control
		)
	{
	CWnd * pWnd;
	pWnd = (CWnd *)(this);

	switch(nTileState)
		{
		case SDS_MODE_ENABLE:
			pWnd->EnableWindow();
			break;
		case SDS_MODE_DISABLE:
			pWnd->EnableWindow(FALSE);
			break;
		case SDS_MODE_SETFOCUS:
			pWnd->SetFocus();
			break;
        case SDS_MODE_FLIP:
			m_bFlipCurrent = !m_bFlipCurrent;
			bReason = 4;
			pWnd->Invalidate(TRUE);
			TRACE("$$$$$$$$--flipped--$$$$$$$$$\n");
			break;
//4M Item:7->
      case SDS_MODE_SETSEL:
			pWnd->Invalidate(TRUE);
			break;
//<-4M Item:7
        default:
			TRACE ("WRONG PARAMETER IN MODE TILE");
			break;
		}
	return SDS_RTNORM;
	}

/************************************************************************************
/* + int ImageButton::GetFlipColor(CString wszColor) -
*
*    Returns the COLORREF  value of for a flipped color.
*
*
*/
COLORREF ImageButton::GetFlipColor(                  // Returns a flip color .
								   CString wszColor  // i: Color to be flipped.
								   )
	{

	CDC *pDC = ((CDialog*)GetParent())->GetDC();


	// These conditional structure returns the COLORREF value
	// corresponding to the string.

	// The focal value is also set, acording to the fill color.

    if ( atoi((LPCTSTR)wszColor) )
		{
		long lColor = atoi((LPCTSTR)wszColor);
		wszColor.Format("%d",lColor);
		TRACE("Color :::::%s\n",wszColor);
		}

	if ( wszColor == _T("dialog_line") )
		{
        // TBD
		LOGPEN logPen;
		CPen *pCurrPen = pDC->GetCurrentPen() ;
		pCurrPen->GetLogPen(&logPen);

		focalColor = RGB(255,255,255);
		return (logPen.lopnColor);
        }
	else if ( wszColor == _T("dialog_foreground") )
		{
		focalColor = RGB(255,255,255);
		return pDC->GetTextColor();
		}
	else if ( wszColor == _T("dialog_background") )
		{
		focalColor = RGB(255,255,255);
		return pDC->GetBkColor();
		}
	else if ( wszColor == _T("graphics_background") )
		{
        // TBD
		focalColor = RGB(255,255,255);
		return RGB(255,255,255);
		}
	else if ( wszColor == _T("black") || wszColor == _T("0") )
		{
		focalColor = RGB(0,0,0);
		m_wszColor = "white";
        return RGB(255,255,255);
		}
	else if ( wszColor == _T("red") || wszColor == _T("1"))
		{
		focalColor = RGB(255,255,0);
		m_wszColor = "cyan";
		return RGB(0,255,255);
		}
	else if ( wszColor == _T("yellow") || wszColor == _T("2"))
		{
		focalColor = RGB(255,255,255);
		m_wszColor = "blue";
        return RGB(0,0,255);
		}
	else if ( wszColor == _T("green") || wszColor == _T("3"))
		{
		focalColor = RGB(0,0,0);
		m_wszColor = "magenta";
        return RGB(255,0,255);
		}
	else if ( wszColor == _T("cyan")  || wszColor == _T("4") )
		{
		focalColor = RGB(255,255,255);
		m_wszColor = "red";
		return RGB(255,0,0);
		}
	else if ( wszColor == _T("blue") || wszColor == _T("5") )
		{
		focalColor = RGB(255,255,255);
		m_wszColor = "yellow";
		return RGB(255,255,0);
		}
	else if ( wszColor == _T("magenta")  || wszColor == _T("6") )
		{
        focalColor = RGB(255,50,255);
		m_wszColor = "green";
		return RGB(0,255,0);
		}
	else if ( wszColor == _T("white")  || wszColor == _T("7") )
		{
		m_wszColor = "black";
        return RGB(0,0,0);
		}
	else if ( atoi((LPCTSTR)wszColor) != 0 )
		{
		// In case an integer value is given which is greater than above colors, then
		// Intellicad's "sds_getrgbvalue" function is called to get the
		// corresponding value for the integer.
		#ifndef _TESTEXE
		HMODULE   hmodule = GetModuleHandle(NULL);
		ASSERT(hmodule);
		if(hmodule)
			{
			typedef int  (* getrgbvalue)(int nColor);
			getrgbvalue pfnGetRGBValue;
			pfnGetRGBValue = ( getrgbvalue)GetProcAddress(hmodule,"sds_getrgbvalue");
			ASSERT(pfnGetRGBValue);
			int nColorRef = RGB ( 255,255,255);
			return ( nColorRef & pfnGetRGBValue(atoi((LPCTSTR)wszColor)));
		}
		#endif
		return RGB(0,0,0);
		}
	else
		{
		// If an invalid value is present , then set the tile to a default color of
		// white.
		return RGB(0,0,0);
		}


		ReleaseDC (pDC);
		pDC = NULL;


	}
//4M Item:3->
void ImageButton::Start()
{
	CRect rect;
	
	if (m_hBitmap == NULL)
	{
		CDC* pDC = this->GetDC();
			
		GetClientRect(&rect);
				
		m_dc		= ::CreateCompatibleDC (pDC->m_hDC);
		m_hBitmap	= ::CreateCompatibleBitmap (pDC->m_hDC, rect.Width(), rect.Height());

		::SelectObject (m_dc, m_hBitmap);

		ReleaseDC (pDC);
		pDC = NULL;
	}

	SetDefaultRectangle (m_dc, rect);

}

/********************************************************************************
 * Author:	Sachin Dange.
 *
 * Purpose:	To set the default Rectangle contents in the DC which is passed in 
 *			'hDC'. This function is called from the DRAWITEM handler and from the 
 *			place where the Bitmap DC needs to be initialised. The parameter 
 *			'rRect' contains the size of the rectangle to be drawn in the DC.
 *
 * Returns:	void
 ********************************************************************************/
void ImageButton::SetDefaultRectangle (HDC hDC, CRect rRect)
{

	COLORREF  color;
	HPEN hOldPen,hNewPen;             // To set the color of pen.
	HBRUSH hOldBrush,hNewBrush;       // To set the color of brush.

	// Get the color attribute of the tile.
	CString wszColorValue = m_wszColor;

    // {{ for flip
	if ( bFlip != m_bFlipCurrent )
		{
		 if ( (ImageButton*)GetFocus() != this || bReason == 4 )
			 {
			 m_nColor = GetFlipColor(wszColorValue);
			 TRACE("!@!@!Flipped!@!@! %s\n",wszColorValue);
			 bReason = 0;
			 }
		 }// flip }}
	 else
		 {
		 	 m_wszColor = this->m_attriblist->get_Color();
			 wszColorValue = m_wszColor ;
			 m_nColor = GetColor(wszColorValue);
			 TRACE("Indide got focus condition\n");
		 }

	color = m_nColor;

	TRACE("WSZCOLOR IS %s\n",wszColorValue);

    if ( wszColorValue == _T("dialog_background") )
	{
        color=GetSysColor(COLOR_ACTIVEBORDER);
		hNewBrush = CreateSolidBrush(color);
    }
	else
	{
		// The string wszColorValue is translated into a COLOREF value.
		// Create a new solid brush , with the current color.
		color = m_nColor;
		hNewBrush = CreateSolidBrush(color);
	}


	// Different pens are selected depending upon whether a tile is
	// image or image_button.
	switch(this->m_nType)
	{
		case IMAGEBUTTON_TILE :  // image_button tile.
			// Create a black pen.
			hNewPen = CreatePen(PS_SOLID,1,RGB(0,0,0));
			break;
		case IMAGE_TILE:       // image tile.
			// Create a white pen.
			hNewPen = CreatePen(PS_SOLID,1,color);
			TRACE("Image tile....switch called\n");
			break;
		default :
			ASSERT(FALSE);
	}


	// Select the newly created pen to the device context.
	hOldPen = (HPEN)SelectObject(hDC, hNewPen);

	// Select the newly created pen to the device context.
	hOldBrush = (HBRUSH)SelectObject(hDC, hNewBrush);

	::Rectangle (hDC, rRect.left, rRect.top, rRect.right, rRect.bottom);

	::DeleteObject((HGDIOBJ)::SelectObject(hDC, hOldPen));
	::DeleteObject((HGDIOBJ)::SelectObject(hDC, hOldBrush));
}
