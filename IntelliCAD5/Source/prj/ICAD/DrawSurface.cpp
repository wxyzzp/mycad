#include "icad.h"
#include "DrawSurface.h"

// ******************************************************************
// CLASS IMPLEMENTATION CDrawSurface
//
// CLASS IMPLEMENTATION CDrawSurface::CPixelIterator

// CTOR
//
CDrawSurface::CPixelIterator::CPixelIterator( CDrawSurface *pDrawingSurface )
{
	this->m_pSurface = pDrawingSurface;
	this->m_pBits = pDrawingSurface->getBits();
	this->m_pPosition = this->m_pBits;
	this->m_iRasterWidth = pDrawingSurface->getWidth() * pDrawingSurface->m_BytesPerPixel;
}

// DTOR
//
CDrawSurface::CPixelIterator::~CPixelIterator( void )
{
	this->m_pSurface = NULL;
	this->m_pBits = NULL;
	this->m_pPosition = NULL;
}

// SetPos
//
void
CDrawSurface::CPixelIterator::SetPos( int x, int y )
{
	this->m_pPosition = this->m_pSurface->getBits();
	this->m_pPosition += x * this->m_pSurface->m_BytesPerPixel;
	this->m_pPosition += y * this->m_iRasterWidth; // m_iRasterWidth is pre-computed to account for BytesPerPixel
}

void
CDrawSurface::CPixelIterator::SetXDirection( int XDir )
{
	this->m_iXAdvance = XDir * this->m_pSurface->m_BytesPerPixel;
}

void
CDrawSurface::CPixelIterator::AdvanceX( void )
{
	this->m_pPosition += this->m_iXAdvance;
}

void
CDrawSurface::CPixelIterator::AdvanceY( void )
{
	this->m_pPosition += this->m_iRasterWidth;
}

// -------------------------
// Methods for setting pixels
//
// We need to do different things if we are in XOR, so I made separate methods.
// 
#ifdef INLINEDBELOW
void
CDrawSurface::CPixelIterator::copyPixel( int color )
{
	if ( this->m_pSurface->m_BytesPerPixel == 1 )
	{
		*this->m_pPosition = color;
	}
	else if ( this->m_pSurface->m_BytesPerPixel == 2 )
	{
		short *pWord = (short *)this->m_pPosition;
		*pWord = color;
	}
	else if ( this->m_pSurface->m_BytesPerPixel == 3 )
	{
		char *pTemp = (char *)&color;
		this->m_pPosition[0] = pTemp[2];
		this->m_pPosition[1] = pTemp[1];
		this->m_pPosition[2] = pTemp[0];
	}
	else if ( this->m_pSurface->m_BytesPerPixel == 4 )
	{
		char *pTemp = (char *)&color;
		this->m_pPosition[0] = pTemp[2];
		this->m_pPosition[1] = pTemp[1];
		this->m_pPosition[2] = pTemp[0];
	}
	else
	{
		// ACK ACK ACK!
		ASSERT( false );
	}
}

void
CDrawSurface::CPixelIterator::xorPixel( int color )
{
	if ( this->m_pSurface->m_BytesPerPixel == 1 )
	{
		*this->m_pPosition ^= color;
	}
	else if ( this->m_pSurface->m_BytesPerPixel == 2 )
	{
		short *pWord = (short *)this->m_pPosition;
		*pWord ^= color;
	}
	else if ( this->m_pSurface->m_BytesPerPixel == 3 )
	{
		char *pTemp = (char *)&color;
		this->m_pPosition[0] ^= pTemp[2];
		this->m_pPosition[1] ^= pTemp[1];
		this->m_pPosition[2] ^= pTemp[0];
	}
	else if ( this->m_pSurface->m_BytesPerPixel == 4 )
	{
		char *pTemp = (char *)&color;
		this->m_pPosition[0] ^= pTemp[2];
		this->m_pPosition[1] ^= pTemp[1];
		this->m_pPosition[2] ^= pTemp[0];
	}
	else
	{
		// ACK ACK ACK!
		ASSERT( false );
	}
	
}
#endif

void
CDrawSurface::CPixelIterator::SetPixel( int color )
{		 
	if ( this->m_pSurface->GetDrawMode() != RasterConstants::MODE_XOR )
	{
		//		copyPixel( color );
		if ( this->m_pSurface->m_BytesPerPixel == 1 )
		{
			*this->m_pPosition = color;
		}
		else if ( this->m_pSurface->m_BytesPerPixel == 2 )
		{
			//		short *pWord = (short *)this->m_pPosition;
			//		*pWord = color;
			*(short *)this->m_pPosition=color;
		}
		else if ( this->m_pSurface->m_BytesPerPixel == 3 )
		{
			char *pTemp = (char *)&color;
			this->m_pPosition[0] = pTemp[2];
			this->m_pPosition[1] = pTemp[1];
			this->m_pPosition[2] = pTemp[0];
		}
		else if ( this->m_pSurface->m_BytesPerPixel == 4 )
		{
			char *pTemp = (char *)&color;
			this->m_pPosition[0] = pTemp[2];
			this->m_pPosition[1] = pTemp[1];
			this->m_pPosition[2] = pTemp[0];
		}
		else
		{
			// ACK ACK ACK!
			ASSERT( false );
		}
	}
	else
	{
		//		xorPixel( color );
		if ( this->m_pSurface->m_BytesPerPixel == 1 )
		{
			*this->m_pPosition ^= color;
		}
		else if ( this->m_pSurface->m_BytesPerPixel == 2 )
		{
			//		short *pWord = (short *)this->m_pPosition;
			//		*pWord ^= color;
			*(short *)this->m_pPosition^=color;
		}
		else if ( this->m_pSurface->m_BytesPerPixel == 3 )
		{
			char *pTemp = (char *)&color;
			this->m_pPosition[0] ^= pTemp[2];
			this->m_pPosition[1] ^= pTemp[1];
			this->m_pPosition[2] ^= pTemp[0];
		}
		else if ( this->m_pSurface->m_BytesPerPixel == 4 )
		{
			char *pTemp = (char *)&color;
			this->m_pPosition[0] ^= pTemp[2];
			this->m_pPosition[1] ^= pTemp[1];
			this->m_pPosition[2] ^= pTemp[0];
		}
		else
		{
			// ACK ACK ACK!
			ASSERT( false );
		}
	}
}

// ************************************
// CLASS IMPLEMENTATION
// 
// CDrawSurface
//

// ************************************
// CTOR -- class CDrawSurface
//
CDrawSurface::CDrawSurface( int sizex, int sizey, void *bits, int bytesperpixel )
{
	m_SizeX = sizex;
	m_SizeY = sizey;
	m_Bits = bits;
	m_BytesPerPixel = bytesperpixel;
	
	m_pLineIterator = new CPixelIterator( this );
	
	m_DrawMode = RasterConstants::MODE_COPY;
}

// ***************************************
// DTOR -- class CDrawSurface
//
CDrawSurface::~CDrawSurface( void )
{
	delete m_pLineIterator;
	m_pLineIterator = NULL;
}

// *******************************************
// PRIVATE INTERNAL METHOD -- class CDrawSurface
//
// drawHorizontalRun
//
// Draws a horizontal run of pixels, then advances the bitmap pointer to
// the first pixel of the next run. 
void 
CDrawSurface::drawHorizontalRun( int RunLength, int Color)
{
	int i;
	
	for (i=0; i<RunLength; i++)
	{
		m_pLineIterator->SetPixel( Color );
		m_pLineIterator->AdvanceX( );		
	}
	/* Advance to the next scan line */
	m_pLineIterator->AdvanceY( );
}

// *******************************************
// PRIVATE INTERNAL METHOD -- class CDrawSurface
//
// drawVerticalRun
//
// Draws a vertical run of pixels, then advances the bitmap pointer to
//  the first pixel of the next run. 
void 
CDrawSurface::drawVerticalRun( int RunLength, int Color)
{
	int i;
	
	for (i=0; i<RunLength; i++)
	{
		m_pLineIterator->SetPixel( Color );
		m_pLineIterator->AdvanceY();
	}
	
	/* Advance to the next column */
	m_pLineIterator->AdvanceX();
}

// *******************************************
// PUBLIC METHOD -- class CDrawSurface
//
// LineDraw
//
//
// Draws a line between the specified endpoints in color Color. 
//
void 
CDrawSurface::LineDraw(int XStart, int YStart, int XEnd, int YEnd, int Color)
{
	int Temp, AdjUp, AdjDown, ErrorTerm, XDelta, YDelta;
	int WholeStep, InitialPixelCount, FinalPixelCount, i, RunLength;
	
	ASSERT( YStart >= 0 );
	ASSERT( YEnd >= 0 );
	ASSERT( YStart < this->getHeight() );
	ASSERT( YEnd < this->getHeight() );
	ASSERT( XStart >= 0 );
	ASSERT( XEnd >= 0 );
	ASSERT( XStart < this->getWidth() );
	ASSERT( XEnd < this->getWidth() );
	
	/* We'll always draw top to bottom, to reduce the number of cases we have to
	handle, and to make lines between the same endpoints draw the same pixels */
	if (YStart > YEnd) 
	{
		Temp = YStart;
		YStart = YEnd;
		YEnd = Temp;
		Temp = XStart;
		XStart = XEnd;
		XEnd = Temp;
	}
	
	/* Point to the bitmap address first pixel to draw */
	m_pLineIterator->SetPos( XStart, YStart );
	
	/* Figure out whether we're going left or right, and how far we're
	going horizontally */
	if ((XDelta = XEnd - XStart) < 0)
	{
		m_pLineIterator->SetXDirection( -1 );
		XDelta = -XDelta;
	}
	else
	{
		m_pLineIterator->SetXDirection( 1 );
	}
	/* Figure out how far we're going vertically */
	YDelta = YEnd - YStart;
	
	/* Special-case horizontal, vertical, and diagonal lines, for speed
	and to avoid nasty boundary conditions and division by 0 */
	if (XDelta == 0)
	{
		/* Vertical line */
		for (i=0; i<=YDelta; i++)
		{
			m_pLineIterator->SetPixel( Color );
			m_pLineIterator->AdvanceY();
		}
		return;
	}
	if (YDelta == 0)
	{
		/* Horizontal line */
		for (i=0; i<=XDelta; i++)
		{
			m_pLineIterator->SetPixel( Color );
			m_pLineIterator->AdvanceX() ;
		}
		return;
	}
	if (XDelta == YDelta)
	{
		/* Diagonal line */
		for (i=0; i<=XDelta; i++)
		{
			m_pLineIterator->SetPixel( Color );
			m_pLineIterator->AdvanceX();
			m_pLineIterator->AdvanceY();
		}
		return;
	}
	
	/* Determine whether the line is X or Y major, and handle accordingly */
	if (XDelta >= YDelta)
	{
		/* X major line */
		/* Minimum # of pixels in a run in this line */
		WholeStep = XDelta / YDelta;
		
		/* Error term adjust each time Y steps by 1; used to tell when one
		extra pixel should be drawn as part of a run, to account for
		fractional steps along the X axis per 1-pixel steps along Y */
		AdjUp = (XDelta % YDelta) * 2;
		
		/* Error term adjust when the error term turns over, used to factor
		out the X step made at that time */
		AdjDown = YDelta * 2;
		
		/* Initial error term; reflects an initial step of 0.5 along the Y
		axis */
		ErrorTerm = (XDelta % YDelta) - (YDelta * 2);
		
		/* The initial and last runs are partial, because Y advances only 0.5
		for these runs, rather than 1. Divide one full run, plus the
		initial pixel, between the initial and last runs */
		InitialPixelCount = (WholeStep / 2) + 1;
		FinalPixelCount = InitialPixelCount;
		
		/* If the basic run length is even and there's no fractional
		advance, we have one pixel that could go to either the initial
		or last partial run, which we'll arbitrarily allocate to the
		last run */
		if ((AdjUp == 0) && ((WholeStep & 0x01) == 0))
		{
			InitialPixelCount--;
		}
		/* If there're an odd number of pixels per run, we have 1 pixel that can't
		be allocated to either the initial or last partial run, so we'll add 0.5
		to error term so this pixel will be handled by the normal full-run loop */
		if ((WholeStep & 0x01) != 0)
		{
			ErrorTerm += YDelta;
		}
		/* Draw the first, partial run of pixels */
		drawHorizontalRun( InitialPixelCount, Color);
		/* Draw all full runs */
		for (i=0; i<(YDelta-1); i++)
		{
			RunLength = WholeStep;  /* run is at least this long */
									/* Advance the error term and add an extra pixel if the error
			term so indicates */
			if ((ErrorTerm += AdjUp) > 0)
			{
				RunLength++;
				ErrorTerm -= AdjDown;   /* reset the error term */
			}
			/* Draw this scan line's run */
			drawHorizontalRun( RunLength, Color);
		}
		/* Draw the final run of pixels */
		drawHorizontalRun( FinalPixelCount, Color);
		return;
	}
	else
	{
		/* Y major line */
		
		/* Minimum # of pixels in a run in this line */
		WholeStep = YDelta / XDelta;
		
		/* Error term adjust each time X steps by 1; used to tell when 1 extra
		pixel should be drawn as part of a run, to account for
		fractional steps along the Y axis per 1-pixel steps along X */
		AdjUp = (YDelta % XDelta) * 2;
		
		/* Error term adjust when the error term turns over, used to factor
		out the Y step made at that time */
		AdjDown = XDelta * 2;
		
		/* Initial error term; reflects initial step of 0.5 along the X axis */
		ErrorTerm = (YDelta % XDelta) - (XDelta * 2);
		
		/* The initial and last runs are partial, because X advances only 0.5
		for these runs, rather than 1. Divide one full run, plus the
		initial pixel, between the initial and last runs */
		InitialPixelCount = (WholeStep / 2) + 1;
		FinalPixelCount = InitialPixelCount;
		
		/* If the basic run length is even and there's no fractional advance, we
		have 1 pixel that could go to either the initial or last partial run,
		which we'll arbitrarily allocate to the last run */
		if ((AdjUp == 0) && ((WholeStep & 0x01) == 0))
		{
			InitialPixelCount--;
		}
		/* If there are an odd number of pixels per run, we have one pixel
		that can't be allocated to either the initial or last partial
		run, so we'll add 0.5 to the error term so this pixel will be
		handled by the normal full-run loop */
		if ((WholeStep & 0x01) != 0)
		{
			ErrorTerm += XDelta;
		}
		/* Draw the first, partial run of pixels */
		drawVerticalRun( InitialPixelCount, Color);
		
		/* Draw all full runs */
		for (i=0; i<(XDelta-1); i++)
		{
			RunLength = WholeStep;  /* run is at least this long */
									/* Advance the error term and add an extra pixel if the error
			term so indicates */
			if ((ErrorTerm += AdjUp) > 0)
			{
				RunLength++;
				ErrorTerm -= AdjDown;   /* reset the error term */
			}
			/* Draw this scan line's run */
			drawVerticalRun( RunLength, Color);
		}
		/* Draw the final run of pixels */
		drawVerticalRun( FinalPixelCount, Color);
		return;
	}
}

