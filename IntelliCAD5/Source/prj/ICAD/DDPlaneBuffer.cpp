#include "icad.h"
#include "IcadApi.h"
#include "DDPlaneBuffer.h"
#include "framebuffer.h"

#include "IcadApp.h"

CDDPlaneBuffer::CDDPlaneBuffer()
{
	m_pFrameBuffer = NULL;
	m_hdcToUse = NULL;

	m_initialHdc = NULL;
	m_hInitialPen = NULL;
	m_hInitialBrush = NULL;
	m_initialROP2 = 0;
	// Initialize these to values that are nonsense for actual drawing
	//
	m_iLineColor = -2;
	m_iLineWidth = -4;
	m_iFillColor = -2;

	m_iLineStyle = RasterConstants::LS_SOLID;
	m_iDrawMode = RasterConstants::MODE_COPY;

	m_bGDIROP2Deferred = false;
	m_bGDIPenDeferred = false;
	m_bGDIBrushDeferred = false;
}

CDDPlaneBuffer::~CDDPlaneBuffer( void )
{
	m_iLineColor = -5;
	m_iLineWidth = -2;
	m_iLineStyle = RasterConstants::LS_NONE;
	
	m_iFillColor = -5;
	m_iDrawMode = RasterConstants::MODE_NONE;

	destroy();
}

// ****************************************
// validation method -- internal
//
bool CDDPlaneBuffer::isValid()
{
	bool bRetval = false;
	if(CHECKSTRUCTPTR(this))
	{
		//if(m_pFrameBuffer != NULL)
		{
			bRetval = true;
		}
	}
	
	return bRetval;
}

// **************************************************
// PUBLIC INTERFACE -- class CDDPlaneBuffer
//
// overrides of CDrawDevice functionality
//
bool CDDPlaneBuffer::create(CDC* pDC, int flags, int width, int height, HPALETTE hPalette)
{
	ASSERT(flags & eSINGLE_BUFFER || width != 0);
	ASSERT(flags & eSINGLE_BUFFER || height != 0);
	ASSERT(flags & eSINGLE_BUFFER || hPalette != NULL);

	m_hdcToUse = NULL;
	if(flags & eDOUBLE_BUFFER)
	{
		m_pFrameBuffer = new CFrameBuffer;
		if(m_pFrameBuffer->Create(pDC->GetSafeHdc(), hPalette, width, height))
			m_hdcToUse = m_pFrameBuffer->GetHdc();
		else
			return false;
	}
	return true;
}

bool CDDPlaneBuffer::resize(CDC* pDC, int width, int height) 
{
	if(m_pFrameBuffer != NULL)
	{
		if(m_hdcToUse == m_pFrameBuffer->GetHdc())
		{
			if(m_pFrameBuffer->Recreate(pDC->GetSafeHdc(), width, height))
			{
				m_hdcToUse = m_pFrameBuffer->GetHdc();
				return true;
			}
			else
				return false;
		}
		else
			return m_pFrameBuffer->Recreate(pDC->GetSafeHdc(), width, height);
	}
	else
		return false;
}

void CDDPlaneBuffer::destroy()
{
	delete m_pFrameBuffer;
	m_pFrameBuffer = NULL;
	m_hdcToUse = NULL;
}

void CDDPlaneBuffer::beginDraw(CDC* pDC, gr_view* pView)
{
	CDrawDevice::beginDraw(pDC, pView);
	if(m_hdcToUse == NULL)
		m_hdcToUse = m_hdc;

	if(m_initialHdc == NULL)
	{
		m_initialHdc = m_hdc;

		HPEN hPen = CreatePen(PS_SOLID, 0, RGB(0,0,0));
		if(hPen)
			m_hInitialPen = (HPEN)::SelectObject(m_initialHdc, hPen);
		HBRUSH hBrush = CreateSolidBrush(RGB(0,0,0));
		if (hBrush)
			m_hInitialBrush = (HBRUSH)::SelectObject(m_initialHdc, hBrush);
		ASSERT(NULL != m_hInitialPen && NULL != m_hInitialBrush);

		m_initialROP2 = GetROP2(m_initialHdc);
	}
}

void CDDPlaneBuffer::endDraw()
{
	if(m_initialHdc != NULL)
	{
		SetROP2(m_initialHdc, m_initialROP2);
		m_initialROP2 = 0;
		
		::DeleteObject(::SelectObject(m_initialHdc, m_hInitialPen));
		m_hInitialPen = NULL;
		::DeleteObject(::SelectObject(m_initialHdc, m_hInitialBrush));
		m_hInitialBrush = NULL;

		m_initialHdc = NULL;
	}
	CDrawDevice::endDraw();
}

bool CDDPlaneBuffer::copyToFront(int x, int y, int width, int height)
{
	if(m_pFrameBuffer != NULL)
		return m_pFrameBuffer->CopyBitsToDC(m_hdc, x, y, width, height);
	else
		return false;
}

void CDDPlaneBuffer::clear()
{
	if(m_pFrameBuffer != NULL)
		m_pFrameBuffer->Clear();
}

void CDDPlaneBuffer::fillRect(const RECT& rect, int color)
{
	COLORREF c = ::SetBkColor(getHdc(), (COLORREF)SDS_BrushColorFromACADColor(color));
	::ExtTextOut(getHdc(), 0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);
	::SetBkColor(getHdc(), c);
}

HPALETTE CDDPlaneBuffer::selectPalette(HPALETTE hPalette, BOOL bForceBackground) 
{
	return ::SelectPalette(getHdc(), hPalette, bForceBackground);
}

UINT CDDPlaneBuffer::realizePalette()
{
	return ::RealizePalette(getHdc());
}

CDC* CDDPlaneBuffer::getCDC()
{
	if(m_pFrameBuffer != NULL && m_pFrameBuffer->GetHdc() == m_hdcToUse)
		return m_pFrameBuffer->GetCDC();
	else
		return m_pDC;
}

int CDDPlaneBuffer::setDrawBuffer(EDrawBuffer mode)
{
	HDC hdc = m_hdcToUse;
	if(mode == eBACK && m_pFrameBuffer != NULL)
		m_hdcToUse = m_pFrameBuffer->GetHdc();
	else
	{
		ASSERT(mode == eFRONT);
		m_hdcToUse = m_hdc;
	}
	m_bGDIROP2Deferred = true;
	m_bGDIPenDeferred = true;
	m_bGDIBrushDeferred = true;
	if(m_pFrameBuffer != NULL && hdc == m_pFrameBuffer->GetHdc())
		return eBACK;
	else
		return eFRONT;
}

bool CDDPlaneBuffer::useGDI() const
{ 
	// DP: it seems that internal polyline output method is not faster than GDI one
	// DP: but the mix of these two methods can produce garbage on the screen
	// DP: because of that let's try to always use GDI functions
	// return m_pFrameBuffer == NULL || m_pFrameBuffer->useGDI() || m_hdcToUse != m_pFrameBuffer->GetHdc(); 
	return true;
}

//Added by Vitaly Spirin (to use in drawDisplayObjects)
HDC CDDPlaneBuffer::getHdc()
{
	//return m_pFrameBuffer->GetHdc();
	return m_hdcToUse;
}

bool CDDPlaneBuffer::setPen(int color, int width, int style)
{
	HPEN hPen;
	if(width <= 1)
		hPen = ::CreatePen(style, width, color);
	else
	{
		LOGBRUSH brush = {BS_SOLID, color, 0};
		//use round caps and joins, to avoid printing problems
		//in the future this could be a pc2/registry setting
		style |= PS_GEOMETRIC | PS_ENDCAP_ROUND | PS_JOIN_ROUND;
		hPen = ::ExtCreatePen(style, width, &brush, 0, NULL); 
	}
	ASSERT(hPen != NULL);
	::DeleteObject(::SelectObject(getHdc(), hPen));
	
	return true;
}

bool CDDPlaneBuffer::setBrush(int color)
{
	HBRUSH hBrush = ::CreateSolidBrush(color);
	ASSERT(hBrush != NULL);
	
	::DeleteObject(::SelectObject(getHdc(), hBrush));
	return true;
}

bool CDDPlaneBuffer::checkGDIDeferred(bool bPolygon)
{
	if(m_bGDIROP2Deferred)
	{
		m_bGDIROP2Deferred = false;
		
		int ropmode = R2_COPYPEN;
		switch(m_iDrawMode)
		{
		case RasterConstants::MODE_COPY:
			break;
		case RasterConstants::MODE_XOR:
			ropmode = R2_XORPEN;
			break;
		default:
			ASSERT(false);
			break;
		}
		
		::SetROP2(getHdc(), ropmode);
	}
	if(bPolygon)
	{
		if(m_bGDIBrushDeferred)
		{
			m_bGDIBrushDeferred = false;
			m_bGDIPenDeferred = true;
			
			setBrush(this->m_iBrushColor);
			int penstyle = PS_SOLID;
			switch(m_iLineStyle)
			{
			case RasterConstants::LS_SOLID:
				break;
			case RasterConstants::LS_DOT:
				penstyle = PS_DOT;
				break;
			default:
				ASSERT(false);
				break;
			}
			
			if(penstyle == PS_DOT)
				setPen(m_iLineColor, 0, penstyle);
			else
				setPen(m_iBrushColor, 0, penstyle);
		}
	}
	else
	{
		if(m_bGDIPenDeferred)
		{
			m_bGDIBrushDeferred = true;
			m_bGDIPenDeferred = false;
			
			int penstyle = PS_SOLID;
			switch( this->m_iLineStyle )
			{
			case RasterConstants::LS_SOLID:
				break;
			case RasterConstants::LS_DOT:
				penstyle = PS_DOT;
				break;
			default:
				ASSERT(false);
				break;
			}
			
			int color;
			color = ::SDS_PenColorFromACADColor(m_iLineColor);
			setPen(color, m_iLineWidth, penstyle );
		}
	}
	return true;
}
										 
bool CDDPlaneBuffer::SetDrawMode(RasterConstants::DrawMode drawmode)
{
	m_bGDIROP2Deferred = true;
	if(drawmode != m_iDrawMode)
	{
		// When changing draw mode, reset the colors so they will be recalculated
		//
		m_iLineColor = -4;
		m_iFillColor = -4;
		
		m_iDrawMode = drawmode;
	}
	if(m_pFrameBuffer != NULL)
		m_pFrameBuffer->SetDrawMode(drawmode);
	return true;
}

RasterConstants::DrawMode CDDPlaneBuffer::GetDrawMode()
{
	return m_iDrawMode;
}

bool CDDPlaneBuffer::Polyline(const POINT *lppt, int iPoints)
{
	if(useGDI() || (m_iLineStyle != RasterConstants::LS_SOLID) || (m_iLineWidth != 1))
	{
		checkGDIDeferred();
		::Polyline(getHdc(), lppt, iPoints);
		// Now deal with GDIs reluctance to draw the last point
		// (Don't do it for GEOMETRIC lines)
		//
		if(m_iLineWidth EQ 1)
		{
			::MoveToEx(getHdc(), lppt[iPoints-1].x, lppt[iPoints-1].y, NULL);
			::LineTo(getHdc(), lppt[iPoints-1].x, lppt[iPoints-1].y+1);
		}
	}
	else
		m_pFrameBuffer->Polyline(lppt, iPoints, m_iLineColor);
	return true;
}

/*D.Gavrilov*/// The last parameter onlyFill was added to the following function
// for non-filling solid hatch paths.
bool CDDPlaneBuffer::Polygon(const POINT *lppt, int iPoints, bool onlyFill)
{
	m_bGDIBrushDeferred = true;	// HACK ... 
	checkGDIDeferred(onlyFill); /*D.Gavrilov*/// don't fill if drawing a path of a solid hatch
	::Polygon(getHdc(), lppt, iPoints);
	return true;
}

bool CDDPlaneBuffer::PolyBezier(const POINT* lppt, DWORD cPoints)
{
	m_bGDIBrushDeferred = true;		// HACK ...
	checkGDIDeferred(false); // don't fill if drawing a path of a TTF text contour
	return !!::PolyBezier(getHdc(), lppt, cPoints);
}

bool CDDPlaneBuffer::TextOut(int xCoord, int yCoord, POINT BltArea[2], const CFont *font, int ACADcolor, const char *text )
{
	int	color;
	if (m_iDrawMode EQ RasterConstants::MODE_XOR)
		color = -ACADcolor;
	else
		color = ACADcolor;
	
	int	xMin = xCoord, yMin = yCoord;
	int	xMax = xCoord, yMax = yCoord;
	
	if (m_iLineStyle != RasterConstants::LS_SOLID)
	{
		//this->checkGDIDeferred();
		CSize		szText;
		TEXTMETRIC	metrics;
		
		int color;
		color = ::SDS_RGBFromACADColor( 7 );
		COLORREF oldTextColor = ::SetTextColor(getHdc(), color);
		
		HFONT hOldFont = (HFONT) ::SelectObject(getHdc(), (HFONT)(*font));
		
		HPEN newPen = ::CreatePen(PS_DOT, 1, color);
		HPEN oldPen = (HPEN) ::SelectObject(getHdc(), newPen);
		
		COLORREF rgbColor = GetBkColor(getHdc());
		
		HBRUSH hHilightBrush = ::CreateHatchBrush(HS_DIAGCROSS, GetBkColor(getHdc()));
		HBRUSH hOldBrush = (HBRUSH) ::SelectObject(getHdc(), hHilightBrush);
		
		ASSERT(hHilightBrush && hOldBrush);
		
		//::SetPolyFillMode(this->GetHdc(), 0);
		
		GetTextExtentPoint32(getHdc(), text, _tcslen( text), &szText);
		GetTextMetrics(getHdc(), &metrics);
		
		int oldBackMode = ::SetBkMode (getHdc(), TRANSPARENT) ;
		
		BeginPath(getHdc());
		::TextOut(getHdc(), xCoord, yCoord, text, strlen(text));
		EndPath(getHdc());
		
		FillPath(getHdc());
		
		if (::SelectObject(getHdc(), hOldBrush))
			::DeleteObject(hHilightBrush);
		else
			ASSERT(FALSE);
		
		::SelectObject(getHdc(), hOldFont);
		
		::SelectObject(getHdc(), oldPen);
		::DeleteObject(newPen);
		::SetBkMode(getHdc(), oldBackMode);
		::SetTextColor(getHdc(), oldTextColor);
		
		yMin = yCoord - metrics.tmAscent;
		xMax = xMin + szText.cx;
		yMax = yMin + szText.cy;
		
	}
	else
	{
								// create offscreen bitmap and metrics DC
		CDC		offscreen;
		offscreen.CreateCompatibleDC( CDC::FromHandle( getHdc()));
		
		// compute required size of bitmap
		CFont	*oldFont = offscreen.SelectObject( const_cast<CFont *>(font));
		CSize	size = offscreen.GetTextExtent( text, _tcslen( text));
		
		TEXTMETRIC	metrics;
		offscreen.GetTextMetrics( &metrics);
		
		if ( ACADcolor < 0 )
		{
			// XOR dragging requires offscreen bitmap
			// restrict bitmap to size of offscreen bitmap
			if ( size.cx > m_pFrameBuffer->width() )
				size.cx = m_pFrameBuffer->width();
			if ( size.cy > m_pFrameBuffer->height() )
				size.cy = m_pFrameBuffer->height();
			
			// create and attach bitmap
			CBitmap	bitmap;
			bitmap.CreateCompatibleBitmap( &offscreen, size.cx, size.cy);
			CBitmap		*oldBitmap = offscreen.SelectObject( &bitmap);
			HPALETTE	oldPalette = NULL;
			
			// set text colors... watch out for palette devices
			offscreen.SetBkMode( OPAQUE);
			if (m_pFrameBuffer->bytesPerPixel() EQ 1 )
			{
				oldPalette = ::SelectPalette( offscreen, m_pFrameBuffer->hCreationPalette(), TRUE);
				offscreen.SetTextColor( PALETTEINDEX(0));
				offscreen.SetBkColor( PALETTEINDEX(255));
			}
			else
			{
				offscreen.SetTextColor( RGB( 0,0,0));
				offscreen.SetBkColor( RGB( 255,255,255));
			}
			
			/*D.G.*/// It's for clearing a place around rotated text (BRX 1792).
			// It works only for the Black and the White background colors.
			offscreen.FloodFill(0, 0, m_pFrameBuffer->bytesPerPixel() == 1 ? PALETTEINDEX(255) : RGB(255, 255, 255));
			
			// draw text to top left of bitmap
			offscreen.SetTextAlign(TA_LEFT + TA_TOP);
			offscreen.TextOut( 0, 0, text);
			
#if	0
			for ( int i = 0; i < 8; i++ )
				for ( int j = 0; j < 8; j++ )
				{
					const int scale = 10;
					CRect	square( CPoint( i*scale, j*scale), CSize( scale,scale));
					CBrush	brush( PALETTEINDEX( 8*i + j));
					offscreen.FillRect( square, &brush);
				}
#endif
				
				
				// draw bitmap to screen ... adjust baseline to yCoord
				yMin = yCoord - metrics.tmAscent;
				xMax = xMin + size.cx;
				yMax = yMin + size.cy;
				BitBlt( getHdc(), xCoord, yCoord - metrics.tmAscent, size.cx, size.cy, offscreen, 0, 0, SRCINVERT); 
				
				offscreen.SelectObject( oldBitmap);
				if ( oldPalette )
					::SelectPalette( offscreen, oldPalette, TRUE);
		}
		else
		{
			int color = ::SDS_RGBFromACADColor(  ACADcolor );
			
			::SetPolyFillMode(getHdc(), 2);
			
			COLORREF oldTextColor = ::SetTextColor(getHdc(), color);
			::TextOut(getHdc(), xCoord, yCoord, text, strlen(text));
			::SetTextColor(getHdc(), oldTextColor);
			
			yMin = yCoord - metrics.tmAscent;
			xMax = xMin + size.cx;
			yMax = yMin + size.cy;
		}
		
		offscreen.SelectObject( oldFont);
	}
	
	// update the blt area extremes with the text size
	if ( BltArea )
	{
		if ( xMin < BltArea[0].x )
			BltArea[0].x = xMin;
		if ( xMin > BltArea[1].x )
			BltArea[1].x = xMin;
		if ( yMin < BltArea[0].y )
			BltArea[0].y = yMin;
		if ( yMin > BltArea[1].y )
			BltArea[1].y = yMin;
		
		if ( xMax < BltArea[0].x )
			BltArea[0].x = xMax;
		if ( xMax > BltArea[1].x )
			BltArea[1].x = xMax;
		if ( yMax < BltArea[0].y )
			BltArea[0].y = yMax;
		if ( yMax > BltArea[1].y )
			BltArea[1].y = yMax;
	}
	
	return true;
}

bool CDDPlaneBuffer::SetPixel(int x, int y, int color)
{
	checkGDIDeferred();
	::SetPixel(getHdc(), x, y, color);
	return true;
}

bool CDDPlaneBuffer::SetLineColorRGB(int color, int width, RasterConstants::LineStyle style)
{
	if ((color != m_iLineColor) || (width != m_iLineWidth) ||	(style != m_iLineStyle))
	{
		m_bGDIPenDeferred = true;
		
		m_iLineColor = color;
		m_iLineWidth = width;
		m_iLineStyle = style;
	}
	return true;
}

bool CDDPlaneBuffer::SetLineColorACAD(int color, int width, RasterConstants::LineStyle style)
{
	if((color != m_iLineColor) || (width != m_iLineWidth) || (style != m_iLineStyle))
	{
		m_bGDIPenDeferred = true;
		
		m_iLineColor = color;
		m_iLineWidth = width;
		m_iLineStyle = style;
	}
	return true;
}

bool CDDPlaneBuffer::SetLineColorXOR(int color, int width, RasterConstants::LineStyle style)
{
	//if((color != m_iLineColor) || (width != m_iLineWidth) || (style != m_iLineStyle))
	if(((m_iLineColor != 7) && (m_iLineColor != 256) && (m_iLineColor != 248)) || 
		(width != m_iLineWidth) || (style != m_iLineStyle))
	{
		m_bGDIPenDeferred = true;
		if(m_pFrameBuffer->bytesPerPixel() == 1 )
		{
			m_iLineColor = 248; // was 7 ^ 255;
		}
		else
		{
			extern RGBQUAD SDS_PaletteColors[256];
			if((SDS_PaletteColors[255].rgbRed < 10 ) &&
				(SDS_PaletteColors[255].rgbGreen < 10) &&
				(SDS_PaletteColors[255].rgbBlue < 10) )
			{
				m_iLineColor = 7;
			}
			else
			{
				m_iLineColor = 256;
			}
		}
		m_iLineWidth = width;
		m_iLineStyle = style;
	}
	return true;
}

bool CDDPlaneBuffer::SetFillColorRGB(int color)
{
	if(color != m_iFillColor)
	{
		m_iBrushColor = color;
		m_bGDIBrushDeferred = true;

		m_iFillColor = color;
	}
	return true;
}

bool CDDPlaneBuffer::SetFillColorACAD(int color)
{
	if(color != m_iFillColor)
	{
		m_iBrushColor = ::SDS_BrushColorFromACADColor(color);
		m_bGDIBrushDeferred = true;
		
		m_iFillColor = color;
	}
	return true;
}

bool CDDPlaneBuffer::SetFillColorXOR(int color)
{
	if(color != m_iFillColor)
	{
		if(m_pFrameBuffer != NULL && m_pFrameBuffer->bytesPerPixel() == 1)
			m_iBrushColor = PALETTEINDEX((color - 1) ^ 255);
		else
			m_iBrushColor = ::SDS_XORBrushColorFromACADColor( color );
		m_bGDIBrushDeferred = true;

		m_iFillColor = color;
	}
	return true;
}

bool CDDPlaneBuffer::SetLineWeight(int weight )
{
	if( m_lwdisplay )
	{
		// Modified By Sachin....LWEIGHT Dialog
		
		// This implementation is wrong; therefore it is commented out.
		// It does not take into account the "Display Scale". 
		/*!!!
		if( weight < 0 )
			weight = m_lwdefault;
		if( weight <= m_lwdefault )
			weight = 1;
		else if ( m_lwdefault > 0 )	
			weight = (weight / m_lwdefault) + 1;
		//!!!*/

		CIcadApp* pIcadApp = (CIcadApp*)AfxGetApp();
		int nScaleFactor = pIcadApp->m_nLWDisplayScaleFactor;

		if( weight < 0 )
			weight = m_lwdefault;

		int nLineWidth = 1;

		// calculate actual line-width to be shown. (range = 1 to 42 pixels max)
		double temp = ((double)(nScaleFactor * weight) / 100.0) + 0.5;
		
		if (temp < 1.0)
			nLineWidth = 1;
		else
			nLineWidth = (int)floor(temp);
		
		weight = nLineWidth;
		// End implementation...

		if( weight != m_iLineWidth )
		{
			m_bGDIPenDeferred = true;
			m_iLineWidth = weight;
			return true;
		}
	}
	return false;
}
