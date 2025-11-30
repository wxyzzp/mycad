/* C:\ICADDEV\PRJ\ICAD\PRINTERDEVICE.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here> 
 * 
 */ 

#include "Icad.h"
#include "IcadApi.h"
#include "IcadATL.h"
#include "PrinterDrawDevice.h"
#include "Preferences.h"

//Static member initialization
const CString CDDPrinter::m_RegKey		= _T("UsePath"/*DNT*/);

CDDPrinter::CDDPrinter(IIcadPlotProperties* pProperties)
: m_properties(pProperties), m_scale(1), m_bUsePath(false),
 m_useLWeight(false), m_scaleLWeight(false)
{
	// TBD Initialize the plot manager during printing
	HRESULT	result = S_OK;

	m_properties->get_Pens(&m_pens);
	ASSERT(m_pens);
	
	//set m_convertToInches if values are in mm
	BSTR unitsBstr;
	result = m_properties->get_Units(&unitsBstr);
	
	if(SUCCEEDED(result) && unitsBstr)
	{
		CString units = unitsBstr;
		if (units == "_I"/*DNT*/)
			m_convertToInches = false;
		else
			m_convertToInches = true;
		
		SysFreeString(unitsBstr);
	}

	OSVERSIONINFO osvi;
	memset(&osvi, 0, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osvi);
	
	TRACE("\nMicrosoft Windows %s %d.%d (Build %d)", (VER_PLATFORM_WIN32_NT == osvi.dwPlatformId ? "NT" : (VER_PLATFORM_WIN32_WINDOWS == osvi.dwPlatformId ? "95" : "WIN32") ),osvi.dwMajorVersion, osvi.dwMinorVersion, osvi.dwBuildNumber & 0xFFFF);
	
	if (VER_PLATFORM_WIN32_WINDOWS == osvi.dwPlatformId)  //VER_PLATFORM_WIN32_NT
	{
		//If ICAD is running on WIN95/98
		PreferenceKey		configKey(HKEY_CURRENT_USER, "Config", FALSE);
		HKEY hPathFlagKey = configKey;
		if (hPathFlagKey != NULL)
		{
			DWORD dwValue, dwType, dwSize;
			dwSize = sizeof(DWORD);
			if (ERROR_SUCCESS == RegQueryValueEx(hPathFlagKey, m_RegKey, NULL ,&dwType,(LPBYTE)&dwValue,(LPDWORD)&dwSize))
			{
				ASSERT(REG_DWORD == dwType);
				ASSERT(sizeof(dwValue) == dwSize);
				if (REG_DWORD == dwType && sizeof(dwValue) == dwSize)
					m_bUsePath = !!dwValue;  //Diable VC++ warning.
			}
			//			RegCloseKey(hPathFlagKey);
		}
	}
	TRACE("\nFlag for using GDI paths for rendering: %d", m_bUsePath);

	// EBATECH(CNBR) 24-03-2003 Bugzilla#78471 Support Lineweight
	int bVal;	
	result = m_properties->get_UseLWeight(&bVal);
	if(SUCCEEDED(result))
		m_useLWeight = ( bVal != 0 ? true : false );
	result = m_properties->get_ScaleLWeight(&bVal);
	if(SUCCEEDED(result))
		m_scaleLWeight = ( bVal != 0 ? true : false );
	// EBATECH(CNBR) ]- 	
}

bool CDDPrinter::create(CDC* pDC, int flags, int width, int height, HPALETTE hPalette)
{
	ASSERT(!(flags & eDOUBLE_BUFFER));
	ASSERT(flags & eSINGLE_BUFFER);

	m_bPreview = (pDC->m_hDC != pDC->m_hAttribDC);
	
	CSize size(pDC->GetDeviceCaps(LOGPIXELSX), pDC->GetDeviceCaps( LOGPIXELSY));
	// assume square
	ASSERT(size.cx == size.cy);
	
	pDC->LPtoDP(&size);
	m_scale = (float)size.cx;
	return true;
}

bool CDDPrinter::resize(CDC* pDC, int width, int height)
{
	return false;
}

void CDDPrinter::beginDraw(CDC* pDC, gr_view* pView)
{
	CDDPlaneBuffer::beginDraw(pDC, pView);
	m_hdcToUse = m_hdc;
}

int CDDPrinter::setDrawBuffer(EDrawBuffer mode)
{
	ASSERT(FALSE);
	return eFRONT;
}

bool CDDPrinter::Polyline(const POINT *lppt, int iPoints)
{
	ASSERT(iPoints > 0 && NULL != lppt);
	
	if ( lppt[0].x EQ lppt[ iPoints-1].x && lppt[0].y EQ lppt[ iPoints-1].y )
	{
		if ( iPoints > 2 )
		{	
			HDC	hDC = getHdc();
			HBRUSH	brush = (HBRUSH)::GetStockObject( NULL_BRUSH);
			
			HBRUSH hOldBrush = (HBRUSH) ::SelectObject( hDC, brush);
			bool bSuccess = Polygon( lppt, iPoints, false);  //Must draw outlines
			::SelectObject( hDC, hOldBrush);
			return bSuccess;
			
		}
		else
		{
			//If this is a preview and two identical points are passed to Polyline().
			if (m_bPreview && m_iLineWidth < 10)
				return SetPixel(lppt[0].x, lppt[0].y, m_iLineColor);
		}
	}
	
	if (m_bUsePath)
		::BeginPath(getHdc());
	
	if ( iPoints > 1 )
		VERIFY( ::Polyline( getHdc(), lppt, iPoints) );
	
	// DP: Purpose and reason of this statement is not clear, commented because of solidarity with base class
	// Now deal with GDIs reluctance to draw the last point
	//
	//::MoveToEx( getHdc(), lppt[iPoints-1].x, lppt[iPoints-1].y, NULL );
	//::LineTo( getHdc(), lppt[iPoints-1].x, lppt[iPoints-1].y+1);
	
	if (m_bUsePath)
	{
		::EndPath(getHdc());
		::StrokePath(getHdc());
	}
	
	return true;
}
  
bool CDDPrinter::Polygon(const POINT *lppt, int iPoints, bool onlyFill)
{
	if (m_bUsePath)
	{
		::BeginPath(getHdc());
		::Polygon( getHdc(), lppt, iPoints );
		::EndPath(getHdc());
		if ( onlyFill )
			::FillPath( getHdc());
		else
			::StrokePath(getHdc());
	}
	else
	{
#define ONLY_FILL_POLYGON
#if defined(ONLY_FILL_POLYGON)
		HPEN hOldPen=NULL;
		if (onlyFill)
			hOldPen = (HPEN)::SelectObject(getHdc(), ::GetStockObject(NULL_PEN));
		::Polygon( getHdc(), lppt, iPoints );
		if (onlyFill && hOldPen)
			::SelectObject(getHdc(), hOldPen);
		
#else
		::Polygon( getHdc(), lppt, iPoints );
#endif
	}
	
	return true;
}

bool CDDPrinter::PolyBezier(const POINT* lppt, DWORD cPoints)
{
	return !!::PolyBezier(getHdc(), lppt, cPoints);
}

static COLORREF
MapPrinterColor(int ACADcolor)
{
	// Check for drawing white on white paper.
	if( SDS_CMainWindow->m_pIcadPrintDlg && ACADcolor EQ 7 )
	{
		RGBQUAD	rgb = SDS_GetPaletteColorsRGBQUAD( ACADcolor);
		
		if ( rgb.rgbRed EQ 255 && rgb.rgbGreen EQ 255 && rgb.rgbBlue EQ 255 )
			return RGB(0,0,0);
	}
	
	return SDS_BrushColorFromACADColor( ACADcolor );
}

bool CDDPrinter::TextOut(const int xCoord, const int yCoord, POINT BltArea[2], const CFont *font, int ACADcolor, const char *text)
{
	if ( m_iLineStyle != RasterConstants::LS_SOLID)
	{
		//		this->checkGDIDeferred();
		
		int color;
		color = ::SDS_RGBFromACADColor( 7 );
		COLORREF oldTextColor = ::SetTextColor( getHdc(), color);
		
		BeginPath( getHdc());
		::TextOut( getHdc(), xCoord, yCoord, text, strlen(text));
		EndPath( getHdc());
		
		HPEN newPen = ::CreatePen(PS_DOT, 1, color);
		HPEN oldPen = (HPEN) ::SelectObject( getHdc(), newPen);
		
		int oldBackMode = ::SetBkMode ( getHdc(), OPAQUE) ;
		
		StrokePath( getHdc());
		
		::SelectObject( getHdc(), oldPen);
		::DeleteObject(newPen);
		::SetBkMode( getHdc(), oldBackMode);
		::SetTextColor( getHdc(), oldTextColor);
	}
	else
	{
		// HACK HACK HACK -- taken from OffscreenFrameBuffer::Polyline() and modified
		//
		COLORREF color;
		GetPenColor( m_pens, &ACADcolor, NULL);
		color = MapPrinterColor( ACADcolor );
		
		::SetPolyFillMode( getHdc(), 2);
		
		COLORREF oldTextColor = ::SetTextColor( getHdc(), color);
		::TextOut( getHdc(), xCoord, yCoord, text, strlen(text));
		::SetTextColor( getHdc(), oldTextColor);
	}
	
	return true;
}

static bool
MapPenColor(IIcadPlotterPens* pens, int* color, double* weight)
{
	HRESULT	result;
	short	count;
	
	// pen found, get the attributes
	result = pens->get_Count( &count);
	if ( SUCCEEDED(result) && *color <= count )
	{
		CComPtr< IIcadPlotterPen>	pen;
		VARIANT	index;
		short	mapNumber;
		double	mapWeight;
		
		index.vt = VT_I2;
		index.iVal = *color;
		
		result = pens->get_Item( index, &pen);
		if ( SUCCEEDED( result) )
			result = pen->get_Number( &mapNumber);
		if ( SUCCEEDED( result) )
			result = pen->get_Weight( &mapWeight);
		
								// override the incoming properties
		if ( SUCCEEDED( result) )
		{
			if ( color )
				*color = mapNumber;
			if ( weight )
				*weight = mapWeight;
			return true;
		}
	}
	
	return false;
}

bool CDDPrinter::GetPenColor(IIcadPlotterPens* pens, int *color, double *weight)
{
	bool result = MapPenColor(pens, color, weight);
	
	//Modified SY 7/9/2000 [
	//Reason : ICAD crashes when printing in metric
	if (weight && result && m_convertToInches) 
		//	if (result && m_convertToInches && weight)
		//Modified By SY 7/9/2000 ]
		
		*weight = MmToInches( *weight);
	
	return result;
}

bool CDDPrinter::SetLineColorACAD(int color, int width, RasterConstants::LineStyle style)
{
	if( color < 1 || color > 256 )
	{
		color=256;
		ASSERT( !"SetLineColorACAD: bad color");
	}
	// attempt to map the color via the pen mapping
	double	weight;
	
	// This is to check to see if the printer supports color or not, if not return.
	if ( GetPenColor( m_pens, &color, &weight) )
		width = (int)(weight * m_scale);
	
	// Check for drawing white on white paper.
	if( SDS_CMainWindow->m_pIcadPrintDlg && color EQ 7 )
	{
		RGBQUAD	rgb = SDS_GetPaletteColorsRGBQUAD( color);
		
		if ( rgb.rgbRed EQ 255 && rgb.rgbGreen EQ 255 && rgb.rgbBlue EQ 255 )
		{
			int penstyle = PS_SOLID;
			switch( style )
			{
			case RasterConstants::LS_SOLID:
				penstyle = PS_SOLID;
				break;
			case RasterConstants::LS_DOT:
				penstyle = PS_DOT;
				break;
			default:
				ASSERT( false );
				penstyle = PS_SOLID;
				break;
			}
			
			setPen(RGB(0, 0, 0), width, penstyle);			
			m_iLineColor = color;
			m_iLineWidth = width;
			m_iLineStyle = style;
			
			//setLineColor( RGB(0,0,0), width, style );
			return true;
		}
	}
	
	int iPenColor = ::SDS_PenColorFromACADColor( color );

	if(iPenColor != m_iLineColor || width != m_iLineWidth || style != m_iLineStyle)
	{
		int penstyle = PS_SOLID;
		switch( style )
		{
		case RasterConstants::LS_SOLID:
			penstyle = PS_SOLID;
			break;
		case RasterConstants::LS_DOT:
			penstyle = PS_DOT;
			break;
		default:
			ASSERT( false );
			penstyle = PS_SOLID;
			break;
		}
		
		setPen(iPenColor, width, penstyle);			
		m_iLineColor = iPenColor;
		m_iLineWidth = width;
		m_iLineStyle = style;
	}
	return true;
	//return setLineColor( iPenColor, width, style );
}

bool CDDPrinter::SetFillColorACAD(int color)
{
	// attempt to map the color via the pen mapping
	double	weight;
	int		width;
	
	if ( GetPenColor( m_pens, &color, &weight) )
		width = (int)(weight * m_scale);
	
	// Check for drawing white on white paper.
	if( SDS_CMainWindow->m_pIcadPrintDlg && (color EQ 7) )
	{
		RGBQUAD	rgb = SDS_GetPaletteColorsRGBQUAD( color);
		
		if ( rgb.rgbRed EQ 255 && rgb.rgbGreen EQ 255 && rgb.rgbBlue EQ 255 )
		{
			setBrush(color);
			m_iFillColor = color;
			//setFillColor( RGB(0,0,0));
			return true;
		}
	}
	
	int iBrushColor = ::SDS_BrushColorFromACADColor( color );
	if(iBrushColor != m_iFillColor)
	{
		setBrush(iBrushColor);
		m_iFillColor = iBrushColor;
	}
	return true;
	//return setFillColor( iBrushColor );
}

bool CDDPrinter::SetLineWeight(int weight )
{
	int color, width, penstyle;

	if(m_useLWeight)
	{
		if( weight < 0 )
			weight = m_lwdefault;
		if( weight <= 0 )
			width = 1;
		else if(( width = m_scale * weight / 2540.0) < 1 )
			width = 1;

		if( m_iLineWidth != width )
		{
			if( SDS_CMainWindow->m_pIcadPrintDlg && m_iLineColor EQ 7 )
				color = RGB(0,0,0);
			else
				color = m_iLineColor;

			m_iLineWidth = width;

			switch( m_iLineStyle )
			{
			case RasterConstants::LS_SOLID:
				penstyle = PS_SOLID;
				break;
			case RasterConstants::LS_DOT:
				penstyle = PS_DOT;
				break;
			default:
				ASSERT( false );
				penstyle = PS_SOLID;
				break;
			}
			setPen(color, width, penstyle);
			return true;
		}
	}
	return false;
}
