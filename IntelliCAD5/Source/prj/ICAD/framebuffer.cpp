// F:\DEV\PRJ\ICAD\FRAMEBUFFER.CPP
// Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
// 
// Abstract
//
//	This implements the offscreen frame buffer class
// 
//

// THIS FILE HAS BEEN GLOBALIZED!
#include "Icad.h"/*DNT*/
#include "IcadApi.h"
#include "framebuffer.h"/*DNT*/
#include "DrawSurface.h"/*DNT*/

// ***********************
// The USE_DIBSECTION version is the fast one that mostly skips
// GDI
//
#define USE_DIBSECTION 1

// ******************************************
// CLASS IMPLEMENTATION
//
// class CFrameBuffer
//

// *****************************************
// CTOR
// At the end of this constructor, the frame buffer is NOT
// ready for drawing...The bitmap still needs to be created
//
CFrameBuffer::CFrameBuffer()
{
	m_iSizeX = 0;
	m_iSizeY = 0;
	m_pMemDC = NULL;
	m_hOldPen = NULL;
	m_hOldBrush = NULL;
	m_bmOld = NULL;
	m_MemDCBmp = NULL;
	
	m_pLineSurface = NULL;
}

// *********************
// DTOR
//
CFrameBuffer::~CFrameBuffer()
{
	Destroy();
}

// ********************************************
// This methods cleans up the GDI resources used
// by the frame buffer
//
// Currently only useful from the destructor
//
bool CFrameBuffer::Destroy()
{
	if(m_pMemDC) 
	{
		::SelectObject(GetHdc(), m_bmOld);
		DestroyBitmap();
		::DeleteObject(SelectObject(m_pMemDC->m_hDC, m_hOldPen));
		::DeleteObject(SelectObject(m_pMemDC->m_hDC, m_hOldBrush));
		delete m_pMemDC;
		m_pMemDC = NULL;
		m_hCreationPalette = NULL;
	}
	
	if(m_pLineSurface != NULL)
	{
		delete m_pLineSurface;
		m_pLineSurface = NULL;
	}
	return true;
}

// ******************************
// Create creates the original offscreen bitmap for
// this class
//
bool CFrameBuffer::Create(HDC hDC, HPALETTE hPalette, int cx, int cy)
{
	m_pMemDC = new CDC();
    //m_pMemDC->CreateCompatibleDC(pOnScreenDC);
	HDC hMemDC = ::CreateCompatibleDC(hDC);
	m_pMemDC->Attach(hMemDC);
	
	::SelectPalette(m_pMemDC->m_hDC, hPalette, TRUE);
	m_hOldPen = ::SelectObject(m_pMemDC->m_hDC, CreatePen(PS_SOLID, 1, RGB(255,255,255)));
	m_hOldBrush = ::SelectObject(m_pMemDC->m_hDC, CreateSolidBrush(RGB(255,255,255)));
	
	//*** Save the original bitmap object in the DC so we can set it back in the destructor.
    CreateBitmap(hDC, hPalette, cx, cy);
	m_hCreationPalette = hPalette;
	return true;
}

// ****************************************
// DestroyBitmap simply gets rid of the internal bitmap, for instance
// when we have to reinitialize because of a size change
//
bool CFrameBuffer::DestroyBitmap()
{
	if(m_MemDCBmp)
	{
		::DeleteObject(m_MemDCBmp);
	}
	m_MemDCBmp = NULL;
	m_pBits = NULL;
	return true;
}

bool CFrameBuffer::Recreate(HDC hDC, int cx, int cy)
{
	::SelectObject(GetHdc(), m_bmOld);
	DestroyBitmap();
    CreateBitmap(hDC, m_hCreationPalette, cx, cy);
	return true;
}

// ***************************************************
// The USE_DIBSECTION version is the fast one that mostly skips
// GDI
//
#if USE_DIBSECTION
// ***************************************************************
// USING DIBSECTION !!!!!!!!!!!!!!!!!!!!!!!!!
// ***************************************************************

bool CFrameBuffer::Clear()
{
	char *bufferPtr = (char *)this->m_pBits;
	int iSize = this->m_iSizeX * this->m_iSizeY * this->m_BytesPerPixel;
	
	int color = 0;
	// if BytesPerPixel is 1, we need to use a PaletteIndex
	//
	if ( this->m_BytesPerPixel == 1 )
	{
		color = PALETTEINDEX( 255 );
		::memset( bufferPtr, color, iSize );
	}
	else if ( this->m_BytesPerPixel == 2 )
	{
		color = ::SDS_RGB16FromACADColor( 256 );
		
		short *pLocPtr = (short *)bufferPtr;
		long i;
		for (i = 0; i < iSize / 2; i++, *(pLocPtr++) = color)
			;
	}
	else if ( this->m_BytesPerPixel == 3 )
	{
		color = ::SDS_RGBFromACADColor( 256 );
		char *pTemp = (char *)&color;
		
		char *endPtr = bufferPtr + iSize;
		char *pLocPtr = bufferPtr;
		while( pLocPtr < endPtr )
		{
			pLocPtr[0] = pTemp[2];
			pLocPtr[1] = pTemp[1];
			pLocPtr[2] = pTemp[0];
			pLocPtr += 3;
		}
	}
	else 
	{
		color = ::SDS_RGBFromACADColor( 256 );
		
		char *pTemp = (char *)&color;
		
		char *endPtr = bufferPtr + iSize;
		char *pLocPtr = bufferPtr;
		while( pLocPtr < endPtr )
		{
			pLocPtr[0] = pTemp[2];
			pLocPtr[1] = pTemp[1];
			pLocPtr[2] = pTemp[0];
			pLocPtr[3] = 0;
			pLocPtr += 4;
		}
	}
	
// The following code gives third-party apps access to the display buffer for the purposes
// of adding their own image data as a background
//Autodsys 022504 {{
/*		struct { 
			HDC hDC;
			int x;
			int y;
			int width;
			int height;
			HDC _this;
			void* m_MemDCBmp;
			int m_iSizeX;
			int m_iSizeY;
		} HDCInfo;
*/	
		m_HDCInfo.hDC    = NULL;
		m_HDCInfo.x      = 0;
		m_HDCInfo.y      = 0;
		m_HDCInfo.width  = this->m_iSizeX;
		m_HDCInfo.height = this->m_iSizeY;
		m_HDCInfo._this  = this->GetHdc();
		m_HDCInfo.m_MemDCBmp = this->m_pBits;
		m_HDCInfo.m_iSizeX   = this->m_iSizeX;
		m_HDCInfo.m_iSizeY   = this->m_iSizeY;
 
		SDS_CallUserCallbackFunc(SDS_CBFRAMEBUFFER,NULL,&m_HDCInfo,NULL);
//Autodsys 022504 }}
	return true;

}

struct BITMAPINFO_1TO1
{
	BITMAPINFOHEADER bmiHeader;
	RGBQUAD          bmiColors[256];
	
	// CTOR
	//
	BITMAPINFO_1TO1(const BITMAPINFOHEADER &bmihToCopy)
	{
		::CopyMemory(&bmiHeader, &bmihToCopy, sizeof(BITMAPINFOHEADER));
		::memset( bmiColors, 0, 256 );
		for (int i = 0; i < 256; i++)
		{
			bmiColors[i] = SDS_GetPaletteColorsRGBQUAD(i+1);
		}
	}
	
	operator BITMAPINFO*()
	{ 
		return (BITMAPINFO*)this; 
	}
};

#define ALIGNULONG(i) ((((i)+3) >> 2) << 2)
// *******************************************
// PRIVATE INTERNAL METHOD -- class CFrameBuffer
// 
// CreateBitmap
// 
// !!! THIS IS THE DIBSECTION VERSION
//
// Use when direct(aka fast) access to the bits is desired
//
HBITMAP CFrameBuffer::CreateBitmap(HDC hDC, HPALETTE hPalette, int cx, int cy)
{ 
	HBITMAP hRetval = NULL;
    BITMAPINFO bi;
    ZeroMemory(&bi, sizeof(BITMAPINFO));
	
	int iWidth  = ALIGNULONG(cx);  // DWORD align our width
	m_iSizeX = iWidth;
	m_iSizeY = cy;
	
	// Get the bits per pixel of the screen
	int iBitsPerPixel = getPixelDepth(hDC);
	
	// We'll just simplify down to a few common formats and let
	// GDI BitBlt deal with conversion in the odd cases
	if(iBitsPerPixel <= 8)
	{
		iBitsPerPixel = 8;
		m_BytesPerPixel = 1;
	}
	else 
		if( iBitsPerPixel <= 16)
		{
			iBitsPerPixel = 16;
			m_BytesPerPixel = 2;
		}
		else
			if(iBitsPerPixel <= 24)
			{
				iBitsPerPixel = 24;
				m_BytesPerPixel = 3;
			}
			else
			{
				iBitsPerPixel = 32;
				m_BytesPerPixel = 4;
			}
	
    // Set up a BITMAPINFO structure for use with CreateDIBSection()
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = iBitsPerPixel;
    bi.bmiHeader.biWidth = m_iSizeX;
    bi.bmiHeader.biHeight = -m_iSizeY;
    bi.bmiHeader.biSizeImage = m_iSizeX * m_iSizeY;
    bi.bmiHeader.biCompression = BI_RGB;
	
	BITMAPINFO_1TO1 bitmapinfo(bi.bmiHeader);
	m_hCreationPalette = hPalette;
	
    // Create the DIBSECTION to draw into, keeping track of the bits
    hRetval = ::CreateDIBSection(hDC, bitmapinfo, DIB_RGB_COLORS, (void**)&m_pBits, NULL, 0L);
	ASSERT(hRetval != NULL);
	
	m_MemDCBmp = hRetval;
    m_bmOld = (HBITMAP)::SelectObject(m_pMemDC->m_hDC, m_MemDCBmp);
	
	// Create a CDrawSurface for the bitmap
	// This will implement the fast drawing code
	if(m_pLineSurface != NULL)
	{
		delete m_pLineSurface;
		m_pLineSurface = NULL;
	}
	m_pLineSurface = new CDrawSurface(m_iSizeX, m_iSizeY, (void*)m_pBits, m_BytesPerPixel);
	ASSERT(m_pLineSurface != NULL );
	return hRetval;
}

#else  // USE_DIBSECTION

// ***************************************************************
// NOT USING DIBSECTION
// ***************************************************************

// !!! THIS IS THE COMPATIBLE BITMAP VERSION
// Use for normal GDI operations
bool CFrameBuffer::Clear()
{
	CBrush brush;
	brush.CreateSolidBrush(SDS_BrushColorFromACADColor(256));
	RECT rect;
	rect.left = 0;
	rect.top = 0;
	rect.right = m_iSizeX;
	rect.bottom = m_iSizeY;
	FillRect(&rect, brush);
	brush.DeleteObject();
	return true;
}

// *******************************************
// PRIVATE INTERNAL METHOD -- class CFrameBuffer
// 
// CreateBitmap
// 
// !!! THIS IS THE COMPATIBLE BITMAP VERSION
// Use for normal GDI operations
//
HBITMAP CFrameBuffer::CreateBitmap(HDC hDC, HPALETTE hPalette, int cx, int cy)
{
	m_iSizeX = cx;
	m_iSizeY = cy;
	HBITMAP hRetval = ::CreateCompatibleBitmap(hDC, cx, cy);

	m_MemDCBmp = hRetval;
    m_bmOld = (HBITMAP)::SelectObject(GetHdc(), m_MemDCBmp);
	return hRetval;
}

#endif  // ENDIF USE_DIBSECTION
// ***************************************************
// ***************************************************
// ***************************************************

bool CFrameBuffer::CopyBitsToDC(HDC hDC, int x, int y, int width, int height)
{
	::BitBlt(hDC, x, y, width, height, this->GetHdc(), x, y, SRCCOPY);
	return true;
}

bool CFrameBuffer::SetDrawMode(RasterConstants::DrawMode theDrawMode)
{
	if(m_pLineSurface != NULL)
		m_pLineSurface->SetDrawMode(theDrawMode);
	return true;
}

bool CFrameBuffer::Polyline(const POINT *lppt, int iPoints, int iLineColor)
{
	// HACK HACK HACK
	//
	int color;
	if(m_BytesPerPixel == 1)
		color = ::SDS_PenColorFromACADColor(iLineColor);
	else 
		if(m_BytesPerPixel == 2)
			color = ::SDS_RGB16FromACADColor(iLineColor);
		else 
			color = ::SDS_RGBFromACADColor(iLineColor);
	for(int i = 0; i < (iPoints - 1); i++)
		m_pLineSurface->LineDraw(lppt[i].x, lppt[i].y, lppt[i+1].x, lppt[i+1].y, color);
	return true;
}

int CFrameBuffer::getPixelDepth(HDC hDC)
{
	return ::GetDeviceCaps(hDC, BITSPIXEL);
}
