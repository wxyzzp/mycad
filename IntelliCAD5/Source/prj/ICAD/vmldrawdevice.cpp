/* VMLDRAWDEVICE.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Implementation of VMLDrawDevice class for outputting the VML
 * file format
 * 
 */ 

#include "icad.h"			/* DNT */
#include <fstream.h>
#include "VMLDrawDevice.h"	/* DNT */


// *********************************
// CLASS IMPLEMENTATION
//
// class VMLDrawDevice
//
//



// *********************************
// CTOR -- class VMLDrawDevice
//

VMLDrawDevice::VMLDrawDevice( LPCTSTR lpFileName )
	{
	m_pfile = new ofstream( lpFileName );
	if ( m_pfile != NULL )
		{
		writeHeader();
		}
	}

VMLDrawDevice::~VMLDrawDevice( void )
	{

	if ( m_pfile != NULL )
		{
		writeEnd();
		m_pfile->close();
		delete m_pfile;
		m_pfile = NULL;
		}

	}


static const char *szHeader = "<html xmlns:v=\"urn:schemas-microsoft-com:vml\">\r\n" \
							 "<head>\r\n" \
							 "<object id=\"VMLRender\" classid=\"CLSID:10072CEC-8CC1-11D1-986E-00A0C955B42E\">" \
							 "</object>\r\n" \
							 "<style>\r\n" \
							 "v\\:* { behavior: url(#VMLRender); }\r\n" \
							 "</style>\r\n" \
							 "</head>\r\n" \
							 "<body>\r\n";		/*DNT*/

static const char *szStartGroup = "<v:group id=\"AN00001_\" style=\"width:8in;height:8in;\"\r\n" \
							"coordSize=\"1600,1600\">\r\n"; /*DNT*/

static const char *szEnd = "</group>\r\n</body>\r\n</html>\r\n";	  /*DNT*/

// **********************************
// internal accessors
// for use by this class and its children
//

bool
VMLDrawDevice::writeHeader( void )
	{
	*m_pfile << szHeader;
	*m_pfile << szStartGroup;
	return true;
	}

bool
VMLDrawDevice::writeEnd( void )
	{
	*m_pfile << szEnd;
	return true;
	}




// ****************************************
// validation method -- internal
//
//
bool
VMLDrawDevice::isValid( void )
	{
	bool bRetval = false;
	if ( CHECKSTRUCTPTR( this ) )
		{
		bRetval = true;
		}

	return bRetval;
	}


// **************************************************
// PUBLIC INTERFACE -- class VMLDrawDevice
//
// overrides of BaseDrawDevice functionality
//

//Added by Vitaly Spirin (to use in drawDisplayObjects)
HDC  
VMLDrawDevice::getHdc(void)
{
    return NULL;
}
										 
bool
VMLDrawDevice::SetDrawMode( RasterConstants::DrawMode drawmode )
	{
	if ( drawmode != m_iDrawMode )
		{
		m_iDrawMode = drawmode;
		}

	return true;
	}


RasterConstants::DrawMode
VMLDrawDevice::GetDrawMode( void )
	{
	return m_iDrawMode;
	}

bool 
VMLDrawDevice::Polyline( const POINT *lppt, int iPoints  )
	{
	
	*m_pfile << "<v:shape style=\"width:1600;height:1600\"\r\n";
	*m_pfile << "path=\"nf m ";
	*m_pfile << lppt[0].x << "," << lppt[0].y;
	*m_pfile <<" l";

	for( int i = 1; i < iPoints; i++ )
		{
		*m_pfile << " " << lppt[i].x << "," << lppt[i].y;
		}
	*m_pfile << " e\"/>\r\n";
	return true;
	}

bool 
VMLDrawDevice::Polygon( const POINT *lppt, int iPoints, bool onlyFill  )
	{
	// NOTE:  The only difference between Polyline and Polygon is the nf command
	// in the path string
	//
	*m_pfile << "<v:shape style=\"width:1600;height:1600\"\r\n";
	*m_pfile << "path=\"m ";
	*m_pfile << lppt[0].x << "," << lppt[0].y;
	*m_pfile <<" l";

	for( int i = 1; i < iPoints; i++ )
		{
		*m_pfile << " " << lppt[i].x << "," << lppt[i].y;
		}
	*m_pfile << " e\"/>\r\n";

	return true;
	}

/********************************************************************************
 * Author:	Dmitry Gavrilov
 *
 * Purpose:	Output one or more Bezier curves defined by "cPoints" points in "lppt" array.
 *
 * Returns:	True.
 ********************************************************************************/
bool 
VMLDrawDevice::PolyBezier
(
 const POINT*	lppt,
 DWORD			cPoints
)
{
// TO DO. This function works right as Polyline now outputting all points of Beziers including controls.

	*m_pfile << "<v:shape style=\"width:1600;height:1600\"\r\n";
	*m_pfile << "path=\"nf m ";
	*m_pfile << lppt[0].x << "," << lppt[0].y;
	*m_pfile <<" l";

	for(DWORD i = 1; i < cPoints; i++)
		*m_pfile << " " << lppt[i].x << "," << lppt[i].y;
	
	*m_pfile << " e\"/>\r\n";

	return true;
}

bool 
VMLDrawDevice::TextOut( const int xCoord, const int yCoord, POINT BltArea[2], const CFont *font, int ACADcolor, const char *text )
	{
	return true;
	}

bool
VMLDrawDevice::SetPixel( int x, int y, int color )
	{
	return true;
	}

bool
VMLDrawDevice::setLineColor( int color, int width, RasterConstants::LineStyle style )
	{
	if ( (color != m_iLineColor ) ||
		 (width != m_iLineWidth ) ||
		 (style != m_iLineStyle ) )
		{
		m_iLineColor = color;
		m_iLineWidth = width;
		m_iLineStyle = style;
		}

	return true;
	}

bool
VMLDrawDevice::SetLineColorACAD( int color, int width, RasterConstants::LineStyle style )
	{
	int iPenColor = color;
	return setLineColor( iPenColor, width, style );
	}

bool
VMLDrawDevice::SetLineColorRGB( int color, int width, RasterConstants::LineStyle style )
	{
	return setLineColor( color, width, style );
	}
bool
VMLDrawDevice::SetLineColorXOR( int color, int width, RasterConstants::LineStyle style )
	{
	int iPenColor = color;
	return setLineColor( iPenColor, width, style );
	}

bool 
VMLDrawDevice::setFillColor( int color )
	{
	if ( (color != m_iFillColor ) )
		{
		m_iFillColor = color;
		}
	return true;
	}

bool 
VMLDrawDevice::SetFillColorACAD( int color )
	{
	int iBrushColor = color;
	return setFillColor( iBrushColor );
	}


bool 
VMLDrawDevice::SetFillColorRGB( int color )
	{
	return setFillColor( color );
	}

bool 
VMLDrawDevice::SetFillColorXOR( int color )
	{
	int iBrushColor = color;
	return setFillColor( iBrushColor );
	}



