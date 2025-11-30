// **********************************************
// FRAMEBUFFER.H
//
// Copyright (C) 1998 Visio Corporation. All rights reserved.
//
// Definition of CFrameBuffer class
// 
#ifndef __FrameBufferH__
#define __FrameBufferH__

#ifndef _RASTERCONSTANTS_H
	#include "rasterconstants.h" /*DNT*/
#endif
// -------------------------
// This class maintains state and implements methods
// for drawing into a frame buffer.  It "knows" about
// bits and bytes in the FrameBuffer
//
// It is used for operations normally done by GDI
//
class CDrawSurface;

//Autodsys 070504 {{
#ifndef FRAMEBUFF_HDC
#define HDCSTRUCT __declspec (dllexport)
#else
#define HDCSTRUCT __declspec (dllimport)
#endif
struct HDCSTRUCT HDCInfo{ 
HDC hDC;
int x;
int y;
int width;
int height;
HDC _this;
void* m_MemDCBmp;
int m_iSizeX;
int m_iSizeY;
};
//Autodsys 070504 }}

// *************************************************
// CLASS DECLARATION -- CFrameBuffer
//
// This represents IntelliCAD's offscreen bitmap
// to which we draw, then blit to the screen
//
class CFrameBuffer
{
public:
	CFrameBuffer();
	~CFrameBuffer();
	
	// PUBLIC OPERATIONS AND ACCESSORS
public:
	HDCInfo m_HDCInfo;//Autodsys 070504
	CDC* GetCDC() {	return this->m_pMemDC; }
	HDC GetHdc() { return this->GetCDC()->GetSafeHdc();	}

	bool Recreate(HDC hDC, int cx, int cy);
	bool Create(HDC hDC, HPALETTE hPalette, int cx, int cy);
	bool CopyBitsToDC(HDC hDC, int x, int y, int width, int height);
	
	bool Clear();
	bool SetDrawMode(RasterConstants::DrawMode drawmode);
	bool useGDI() const { return m_pLineSurface == NULL; }
	bool Polyline(const POINT *lppt, int iPoints, int iLineColor);

	int width() const { return m_iSizeX; }
	int height() const { return m_iSizeY; }
	int bytesPerPixel() const { return m_BytesPerPixel; }
	HPALETTE hCreationPalette() const { return m_hCreationPalette; }
	
	// HELPER METHODS
private:
	// Destroy is basically used by the destructor
	bool Destroy();
	
	int getPixelDepth(HDC hDC);
	
	// CreateBitmap hides the actual details of the bitmap creation...i.e.  
	// Device Compatible or Device Independent (DIBSECTION)
	HBITMAP CreateBitmap(HDC hDC, HPALETTE hPalette, int cx, int cy);
	
	// This cleans up the GDI resources before we reinitialize -- for example
	// when the size changes
	bool DestroyBitmap();
	
	// INTERNAL STATE
private:
	//*** Off screen device context member variables
	int			m_iSizeX;
	int			m_iSizeY;
	CDC*		m_pMemDC;
	HGDIOBJ		m_hOldPen;
	HGDIOBJ		m_hOldBrush;
	HBITMAP		m_bmOld;
	HBITMAP		m_MemDCBmp;
	HPALETTE	m_hCreationPalette;
	
	int			m_BytesPerPixel;
	LPBYTE		m_pBits;    // Global pointer to the bits in our DIBSECTION
	
	// This is the fast drawing class
	//
	CDrawSurface* m_pLineSurface;
};

#endif

