#ifndef __DDPlaneBufferH__
#define __DDPlaneBufferH__

#ifndef _RASTERCONSTANTS_H
	#include "rasterconstants.h"
#endif
#include "DrawDevice.h"

class CFrameBuffer;

// Redesigned by Denis Petrov
// Description: This is base class for output using GDI API
class CDDPlaneBuffer: public CDrawDevice
{
public:
	CDDPlaneBuffer();
	virtual ~CDDPlaneBuffer();

	virtual bool create(CDC* pDC, int flags = eDOUBLE_BUFFER, int width = 0, int height = 0, HPALETTE hPalette = NULL);
	virtual bool resize(CDC* pDC, int width, int height);
	virtual void destroy();
	virtual void beginDraw(CDC* pDC, gr_view* pView);
	virtual void endDraw();

	virtual bool copyToFront(int x, int y, int width, int height);
	virtual void clear();
	virtual void fillRect(const RECT& rect, int color);
	virtual HPALETTE selectPalette(HPALETTE hPalette, BOOL bForceBackground);
	virtual UINT realizePalette();
	virtual CDC* getCDC();
	virtual int setDrawBuffer(EDrawBuffer mode);

	//Added by Vitaly Spirin (to use in drawDisplayObjects)
	virtual HDC getHdc(); 
	virtual bool SetDrawMode(RasterConstants::DrawMode mode);
	virtual RasterConstants::DrawMode  GetDrawMode( void );
/********************************************************************************
 * Author:	Dmitry Gavrilov
 *
 * Purpose:	Draw one or more Bezier curves defined by "cPoints" points in "lppt" array.
 *
 * Returns:	True in success and false in failure.
 ********************************************************************************/
	virtual bool PolyBezier(const POINT* lppt, DWORD cPoints);
	virtual bool Polyline( const POINT *lppt, int iPoints  );
	//D.Gavrilov The last parameter onlyFill was added to the following function
	// for non-filling solid hatch paths.
	virtual bool Polygon(const POINT *lppt, int iPoints, bool onlyFill);
	virtual bool TextOut(int xCoord, int yCoord, POINT BltArea[2], const CFont *font, int ACADcolor, const char *text);
	virtual bool SetPixel(int x, int y, int color);
	virtual bool SetLineColorACAD(int color, int width = 1, RasterConstants::LineStyle style = RasterConstants::LS_SOLID);
	virtual bool SetLineColorRGB(int color, int width = 1, RasterConstants::LineStyle style = RasterConstants::LS_SOLID);
	virtual bool SetLineColorXOR(int color, int width = 1, RasterConstants::LineStyle style = RasterConstants::LS_SOLID);
	virtual bool SetFillColorACAD(int color);
	virtual bool SetFillColorRGB(int color);
	virtual bool SetFillColorXOR(int color);

	virtual bool SetLineWeight(int weight );
	// Virtual internal operations
protected:
	// Each sub-class should verify that it's internals are in a valid state
	//
	virtual bool isValid( void );

	bool useGDI() const;
	
	bool	setPen(int color, int width, int style);
	bool	setBrush(int color);
	
	bool	checkGDIDeferred(bool bPolygon = false);
	// Internal data
	//
protected:
	HDC m_hdcToUse;

	HDC m_initialHdc;
	HPEN m_hInitialPen;
	HBRUSH m_hInitialBrush;
	int m_initialROP2;

	CFrameBuffer* m_pFrameBuffer;

	// If we set the draw mode, don't do the GDI ROP2 operation until we HAVE to
	// because it is slow
	//
	bool		m_bGDIROP2Deferred;
	bool		m_bGDIPenDeferred;
	bool		m_bGDIBrushDeferred;
	
	int		m_iLineColor;
	int		m_iLineWidth;
	int		m_iLineStyle;
	
	int		m_iFillColor;
	int			m_iBrushColor;		// aka GDI BRUSH color

	RasterConstants::DrawMode		m_iDrawMode;
};

#endif
