// **********************************************
// LINEDRAWINGSURFACE.H
//
// Copyright (C) 1998 Visio Corporation. All rights reserved.
//
// Definition of CDrawSurface class
// 
// Originally class was only used to draw lines--sorry about the name of the .H file
//
//


#ifndef __DrawSurfaceH__
#define __DrawSurfaceH__

#ifndef _RASTERCONSTANTS_H
	#include "rasterconstants.h" /*DNT*/
#endif

// ************************************
// CLASS DEFINITION
//
// CDrawSurface
//
// Used to implement custom, fast drawing
//
class CDrawSurface
{
private:
	// --------------------------------
	// Inner helper class -- CPixelIterator
	//
	// CPixelIterator keeps track of a position
	// in the bitmap.  It hides information like
	// how many bits deep the bitmap is
	//
	// This is currently optimized for the line drawing algorithm
	// we are using, which is why it has the weird SetXDirection method
	//
	class CPixelIterator
	{
	public:
		CPixelIterator( CDrawSurface *pSurface );
		~CPixelIterator( void );
		
		// SetPos moves to an absolute pixel in the bitmap
		//
		inline void SetPos( int x, int y );
		
		// SetXDirection tells the iterator which direction to
		// advance.  Basically an optimization for the line drawing
		// algorithm we are using
		//
		inline void SetXDirection( int XDir );
		
		// Advance one pixel in the X or Y direction (accounting for
		// SetXDirection
		//
		inline void AdvanceX( void );
		inline void AdvanceY( void );
		
		// Set the pixel to this absolute color
		//
		inline void SetPixel( int color );
		
		// helper methods
	private:
		inline void copyPixel( int color );
		inline void xorPixel( int color );
		
		
	private:
		CDrawSurface *m_pSurface;
		
		// RasterWidth is the length of an entire scan line in bytes
		// This allows us to quickly move up or down scan-lines by simply
		// adding RasterWidth
		//
		int m_iRasterWidth;
		
		// Bits are the actual bits of the bitmap.  Assumed to be a 2D array
		//
		char *m_pBits;
		
		// The current position of the iterator, a pointer directly into Bits
		//
		char *m_pPosition;
		
		// This is not only the number of bytes for one pixel, but a positive
		// or negative based on the direction we are iterating (set by SetXDirection)
		//
		int m_iXAdvance;
		
	}; // end of CPixelIterator class declaration
	
	//
	// ------------------------------------------------------------
	
	// ******************************************************
	// CPixelIterator is a friend for speed purposes
	//
	friend CDrawSurface::CPixelIterator;
	
	// ----------------------------
	// CTORS and DTORS -- CDrawSurface
	//
public:
	CDrawSurface( int sizex, int sizey, void *bits, int bytesperpixel );
	virtual ~CDrawSurface( void );
		
public:
	bool SetDrawMode( RasterConstants::DrawMode mode )
	{
		m_DrawMode = mode;
		return true;
	}
	RasterConstants::DrawMode GetDrawMode( void )
	{
		return m_DrawMode;
	}
		
	// This implements the fast line algorithm
	//
	void LineDraw( int XStart, int YStart, int XEnd, int YEnd, int Color );
		
	// ------------------------
	// HELPER METHODS
	//
private:
	char *getBits( void )
	{
		return (char *)m_Bits;
	}
	char *getBits( int x, int y )
	{
		int iOffSet = (y * m_SizeX) + x;
		return ((char *)(m_Bits)+iOffSet);
	}
	int getWidth( void )
	{
		return m_SizeX;
	}
	int getHeight( void )
	{
		return m_SizeY;
	}
		
	inline void drawHorizontalRun( int RunLength, int Color );
	inline void drawVerticalRun(  int RunLength, int Color);
	
	// ---------------------------
	// INTERNAL DATA
	//
private:
		
	// The size of the bitmap
	int m_SizeX;
	int m_SizeY;
	
	// the actual bits
	void *m_Bits;
		
	// the byte depth
	int m_BytesPerPixel;
		
	// XOR or COPY??
	RasterConstants::DrawMode	m_DrawMode;
		
	// An iterator keeping track of the line drawing algorithm's position
	//
	CPixelIterator* m_pLineIterator;
		
};
	
#endif
