#ifndef __DDOpenGLH__
#define __DDOpenGLH__

#include "DrawDevice.h"
#include <GL\gl.h>
#include <GL\glu.h>

// Author: Denis Petrov
// Represents OpenGL draw device
class CDDOpenGL: public CDrawDevice
{
public:
	CDDOpenGL();
	virtual	~CDDOpenGL();

	virtual bool create(CDC* pDC, int flags = eDOUBLE_BUFFER, int width = 0, int height = 0, HPALETTE hPalette = NULL);
	virtual bool resize(CDC* pDC, int width, int height);
	virtual void destroy();
	virtual void beginDraw(CDC* pDC, struct gr_view* pView);
	virtual void endDraw();
	virtual bool is3D() const { return true; }
	virtual void clear();
	virtual void fillRect(const RECT& rect, int color);
	virtual bool copyToFront(int x, int y, int width, int height);

	virtual CDC* getCDC();
	virtual HDC getHdc();

	virtual int setDrawBuffer(EDrawBuffer mode);

	virtual bool SetDrawMode(RasterConstants::DrawMode mode);
	virtual RasterConstants::DrawMode GetDrawMode();
	virtual bool PolyBezier(const POINT *lppt, DWORD cPoints);
	virtual bool Polyline(const POINT *lppt, int iPoints);
	virtual bool Polygon(const POINT *lppt, int iPoints, bool onlyFill);
	virtual bool SetPixel(int x, int y, int color);
	virtual bool SetLineColorACAD(int color, int width = 1, RasterConstants::LineStyle style = RasterConstants::LS_SOLID);
	virtual bool SetLineColorRGB(int color, int width = 1, RasterConstants::LineStyle style = RasterConstants::LS_SOLID);
	virtual bool SetLineColorXOR(int color, int width = 1, RasterConstants::LineStyle style = RasterConstants::LS_SOLID);
	virtual bool SetFillColorACAD(int color);
	virtual bool SetFillColorRGB(int color);
	virtual bool SetFillColorXOR(int color);

	virtual bool SetLineWeight(int weight );
protected:
	HDC m_hdcForGL;
	HGLRC m_hglrc;
	int m_width;
	int m_height;

	bool m_bTesselatorActive;
	typedef void (CALLBACK *GLCB)();
	GLUtesselator* m_pTesselator;
	static void CALLBACK tesselatorError(GLenum code);

	static PFNGLADDSWAPHINTRECTWINPROC s_pglAddSwapHintRectWIN;
	// Each sub-class should verify that it's internals are in a valid state
	virtual bool isValid();
};

#endif
