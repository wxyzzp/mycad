#include "icad.h"
#include "DDOpenGL.h"
#include <GL\glu.h>
#include "IcadApi.h"
#include "gr.h"
#include "cmds.h"

PFNGLADDSWAPHINTRECTWINPROC CDDOpenGL::s_pglAddSwapHintRectWIN = NULL;

void CALLBACK CDDOpenGL::tesselatorError(GLenum code)
{
	ASSERT(FALSE);
}

CDDOpenGL::CDDOpenGL():
m_hglrc(NULL), m_pTesselator(NULL), m_bTesselatorActive(false)
{
	m_hdcForGL = NULL;

}

CDDOpenGL::~CDDOpenGL()
{
	destroy();
}

bool CDDOpenGL::create(CDC* pDC, int flags, int width, int height, HPALETTE hPalette)
{
	m_hdc = m_hdcForGL = pDC->GetSafeHdc();

	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);  
	pfd.nVersion = 1;						
	pfd.dwFlags	= PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cDepthBits = 32;
	pfd.iLayerType = PFD_MAIN_PLANE;

	pfd.cColorBits	= 24;
	pfd.cAlphaBits = 8;
		
	int nPixelFormat = ChoosePixelFormat(m_hdc, &pfd);
	if(nPixelFormat == 0)
	{
		ASSERT(false);
		return false;
	}
	
	if(!SetPixelFormat(m_hdc, nPixelFormat, &pfd))
	{
		ASSERT(false);
		return false;
	}
	
	m_hglrc = wglCreateContext(m_hdc);
	if(!m_hglrc)
	{
		ASSERT(false);
		return false;
	}

	if(m_hglrc != wglGetCurrentContext())
	{
		if(wglMakeCurrent(getHdc(), m_hglrc) == false)
		{
			ASSERT(false);
			return false;
		}
	}

	ASSERT(m_pTesselator == NULL);
	m_pTesselator = gluNewTess();
	gluTessCallback(m_pTesselator, GLU_TESS_ERROR, (GLCB)tesselatorError);  
	gluTessCallback(m_pTesselator, GLU_TESS_BEGIN, (GLCB)glBegin);  
	gluTessCallback(m_pTesselator, GLU_TESS_VERTEX, (GLCB)glVertex3dv);  
	gluTessCallback(m_pTesselator, GLU_TESS_END, (GLCB)glEnd);  

	const GLubyte* pExt = glGetString(GL_VENDOR);
	pExt = glGetString(GL_RENDERER);
	pExt = glGetString(GL_VERSION);
	pExt = glGetString(GL_EXTENSIONS);
	if(s_pglAddSwapHintRectWIN == NULL && pExt != NULL)
	{
		char* pWinHint = strstr((char*)pExt, "GL_WIN_swap_hint");
		if(pWinHint != NULL)
			s_pglAddSwapHintRectWIN = (PFNGLADDSWAPHINTRECTWINPROC)wglGetProcAddress("glAddSwapHintRectWIN");
	}
	return true;
}

void CDDOpenGL::destroy()
{
	if(m_hglrc) 
	{
		gluDeleteTess(m_pTesselator);
		m_pTesselator = NULL;

		// Delete rendering context.
		if(m_hglrc == wglGetCurrentContext())
			wglMakeCurrent(NULL, NULL); 
		wglDeleteContext(m_hglrc);
		m_hglrc = NULL;
	}
}

void CDDOpenGL::beginDraw(CDC* pDC, struct gr_view* pView)
{
	CDrawDevice::beginDraw(pDC, pView);

	if(m_hglrc != wglGetCurrentContext())
	{
		// Make the rendering context m_hrc current
		if(wglMakeCurrent(getHdc(), m_hglrc) == false)
		{
			ASSERT(false);
			return;
		}
	}
	if(pView == NULL)
		return;

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	//glDepthFunc(GL_ALWAYS);
	//glDepthMask(GL_FALSE);

	glEnable(GL_COLOR_LOGIC_OP);
	glDrawBuffer(GL_BACK_LEFT);

	glViewport(0, 0, m_width, m_height);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-(double)m_width / (double)m_height, (double)m_width / (double)m_height, -1.0, 1.0, 1.0, -1.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if(pView != NULL)
	{
		ASSERT((pView->mode & 0x01) == 0);

		struct resbuf rb;
		CD3 bbmin, bbmax, c, target;
		SDS_getvar(NULL, DB_QEXTMIN, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
		bbmin = rb.resval.rpoint;
		SDS_getvar(NULL, DB_QEXTMAX, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
		bbmax = rb.resval.rpoint;
		SDS_getvar(NULL, DB_QVIEWCTR, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
		target = rb.resval.rpoint;
		cmd_elev_correct(((bbmin + bbmax) * 0.5)[2], target, 1, NULL);
		if(bbmin[0] > target[0])
			bbmin[0] = target[0];
		if(bbmin[1] > target[1])
			bbmin[1] = target[1];
		if(bbmax[0] < target[0])
			bbmax[0] = target[0];
		if(bbmax[1] < target[1])
			bbmax[1] = target[1];

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		//glOrtho(- 0.5 * pView->viewsize * (double)m_width / (double)m_height, 
		//	0.5 * pView->viewsize * (double)m_width / (double)m_height, -0.5 * pView->viewsize, 0.5 * pView->viewsize, pView->viewsize, -pView->viewsize);
		glOrtho(- 0.5 * pView->viewsize * (double)m_width / (double)m_height, 
			0.5 * pView->viewsize * (double)m_width / (double)m_height, -0.5 * pView->viewsize, 0.5 * pView->viewsize, 
			0.0, 3.0 * (bbmax - bbmin).norm() + 2.0 * pView->viewdist);
		glMatrixMode(GL_MODELVIEW);
		double m[16];
		memset(m, 0, 16 * sizeof(double));
		m[0] = pView->rpaxis[0][0];
		m[1] = pView->rpaxis[1][0];
		m[2] = pView->rpaxis[2][0];

		m[4] = pView->rpaxis[0][1];
		m[5] = pView->rpaxis[1][1];
		m[6] = pView->rpaxis[2][1];

		m[8] = pView->rpaxis[0][2];
		m[9] = pView->rpaxis[1][2];
		m[10] = pView->rpaxis[2][2];

		m[12] = 0.0;
		m[13] = 0.0;
		m[14] = - 2.0 * ((bbmax - bbmin).norm() + pView->viewdist);
		//sds_point offset;
		//pView->GetCenterProjection(offset);
		//m[12] = - m[0] * offset[0] - m[1] * offset[1] - m[2] * offset[2];
		//m[13] = - m[4] * offset[0] - m[5] * offset[1] - m[6] * offset[2];
		//m[14] = - 2.0 * ((bbmax - bbmin).norm() + pView->viewdist) - m[8] * offset[0] - m[9] * offset[1] - m[10] * offset[2];

		m[15] = 1.0;
		glLoadMatrixd((double*)&m[0]);
		glTranslated(-target[0], -target[1], -target[2]);
	}

	// axes
	//glBegin(GL_LINES);
	//glColor3f(1.0f, 0.0f, 0.0f);
	//glVertex3f(0.0f, 0.0f, 0.0f);
	//glVertex3f(0.5f, 0.0f, 0.0f);

	//glColor3f(0.0f, 1.0f, 0.0f);
	//glVertex3f(0.0f, 0.0f, 0.0f);
	//glVertex3f(0.0f, 0.5f, 0.0f);

	//glColor3f(0.0f, 0.0f, 1.0f);
	//glVertex3f(0.0f, 0.0f, 0.0f);
	//glVertex3f(0.0f, 0.0f, 0.5f);
	//glEnd();
}

void CDDOpenGL::endDraw()
{
	glFinish();
	//SwapBuffers(getHdc());

	if(wglMakeCurrent(NULL, NULL) == false)
	{
		ASSERT(false);
	}

	CDrawDevice::endDraw();
}

bool CDDOpenGL::resize(CDC* pDC, int width, int height)
{
	m_width = width;
	m_height = height;
	return true;
}

void CDDOpenGL::clear() 
{
}

void CDDOpenGL::fillRect(const RECT& rect, int color) 
{
}

bool CDDOpenGL::copyToFront(int x, int y, int width, int height)
{
	glFinish();
/*
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glDrawBuffer(GL_FRONT);
	glRasterPos3f(2.0f * GLfloat(x) / GLfloat(m_width) - 1.0f, 2.0f * GLfloat(m_height - height - y) / GLfloat(m_height) - 1.0f, 0.0f);
	//glRasterPos3f(- 1.0f, - 1.0f, 0.0f);
	glReadBuffer(GL_BACK);
	glCopyPixels(x, m_height - height - y, width, height, GL_COLOR);
	//glCopyPixels(0, 0, m_width, m_height, GL_COLOR);

	glDrawBuffer(GL_BACK);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
*/

	if(s_pglAddSwapHintRectWIN)
		s_pglAddSwapHintRectWIN(x, m_height - height - y, width, height);
	SwapBuffers(getHdc());
	return false; 
}

int CDDOpenGL::setDrawBuffer(EDrawBuffer mode)
{
	if(mode == eFRONT)
	{
		glDrawBuffer(GL_FRONT_LEFT);
		return eBACK;
	}
	else
		if(mode == eBACK)
		{
			glDrawBuffer(GL_BACK_LEFT);
			return eFRONT;
		}
	return 0;
}

CDC* CDDOpenGL::getCDC()
{
	if(m_pDC != NULL)
		return m_pDC;
	else
		return CDC::FromHandle(m_hdcForGL);
}

HDC CDDOpenGL::getHdc()
{
	if(m_hdc != NULL)
		return m_hdc;
	else
		return m_hdcForGL;
}

bool CDDOpenGL::SetDrawMode(RasterConstants::DrawMode mode)
{
	if(mode == RasterConstants::DrawMode::MODE_COPY)
	{
		glLogicOp(GL_COPY);
	}
	else
		if(mode == RasterConstants::DrawMode::MODE_XOR)
		{
			glLogicOp(GL_XOR);
		}
	return true;
}

RasterConstants::DrawMode CDDOpenGL::GetDrawMode()
{
	return RasterConstants::DrawMode::MODE_NONE;
}

bool CDDOpenGL::PolyBezier(const POINT *lppt, DWORD cPoints)
{
	return false;
}

bool CDDOpenGL::Polyline(const POINT *lppt, int iPoints)
{
	glVertexPointer(3, GL_DOUBLE, 0, (GLvoid*)lppt);
	glDrawArrays(GL_LINE_STRIP, 0, iPoints);
	return true;
}

bool CDDOpenGL::Polygon(const POINT *lppt, int iPoints, bool onlyFill)
{
	if(onlyFill)
	{
		glVertexPointer(3, GL_DOUBLE, 0, (GLvoid*)lppt);
		glDrawArrays(GL_LINE_LOOP, 0, iPoints);
	}
	else
	{
		if(iPoints > 0)
		{
			glVertexPointer(3, GL_DOUBLE, 0, (GLvoid*)lppt);
			glDrawArrays(GL_POLYGON, 0, iPoints);
		}
		else
		{
			if(iPoints == 0)
			{
				if(!m_bTesselatorActive)
				{
					gluTessBeginPolygon(m_pTesselator, NULL); 
					m_bTesselatorActive = true;
				}
				else
				{
					gluTessEndPolygon(m_pTesselator); 
					m_bTesselatorActive = false;;
				}
			}
			else
			{
			GLdouble* pPoints = (GLdouble*)lppt;
			gluTessBeginContour(m_pTesselator);
			for(int i = 0; i < abs(iPoints); ++i)
			{
				gluTessVertex(m_pTesselator, pPoints, pPoints); 
				pPoints += 3;
			}
			gluTessEndContour(m_pTesselator);
			}
		}
	}
	return true;
}

bool CDDOpenGL::SetPixel(int x, int y, int color)
{
	//glMatrixMode(GL_MODELVIEW);
	//glPushMatrix();
	//glLoadIdentity();

	//glRasterpos()
	//glDrawPixels(1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &color);
	//glPopMatrix();
	return false;
}

bool CDDOpenGL::SetLineColorACAD(int color, int width/* = 1*/, RasterConstants::LineStyle style/* = RasterConstants::LS_SOLID*/)
{
	COLORREF c = (COLORREF)::SDS_PenColorFromACADColor(color);
	glColor3f((float)GetRValue(c) / 255.0f, (float)GetGValue(c) / 255.0f, (float)GetBValue(c) / 255.0f);

	glLineWidth((GLfloat)width);
	if(style == RasterConstants::LS_SOLID)
	{
		glDisable(GL_LINE_STIPPLE);
		glDisable(GL_POLYGON_STIPPLE);
	}
	else
	{
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(1, 0x0F0F);
		glEnable(GL_POLYGON_STIPPLE);
		GLubyte ps[16] = {0x0F, 0x0F, 0x0F, 0x0F,
			0xF0, 0xF0, 0xF0, 0xF0,
			0x0F, 0x0F, 0x0F, 0x0F,
			0xF0, 0xF0, 0xF0, 0xF0};
		glPolygonStipple(ps);
	}
	return true;
}

bool CDDOpenGL::SetLineColorRGB(int color, int width/* = 1*/, RasterConstants::LineStyle style/* = RasterConstants::LS_SOLID*/)
{
	COLORREF c = (COLORREF)color;
	glColor3f((float)GetRValue(c) / 255.0f, (float)GetGValue(c) / 255.0f, (float)GetBValue(c) / 255.0f);

	glLineWidth((GLfloat)width);
	if(style == RasterConstants::LS_SOLID)
	{
		glDisable(GL_LINE_STIPPLE);
		glDisable(GL_POLYGON_STIPPLE);
	}
	else
	{
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(1, 0x0F0F);
		glEnable(GL_POLYGON_STIPPLE);
		GLubyte ps[16] = {0x0F, 0x0F, 0x0F, 0x0F,
			0xF0, 0xF0, 0xF0, 0xF0,
			0x0F, 0x0F, 0x0F, 0x0F,
			0xF0, 0xF0, 0xF0, 0xF0};
		glPolygonStipple(ps);
	}
	return true;
}

bool CDDOpenGL::SetLineColorXOR(int color, int width/* = 1*/, RasterConstants::LineStyle style/* = RasterConstants::LS_SOLID*/)
{
	COLORREF c = (COLORREF)::SDS_XORPenColorFromACADColor(color);
	glColor3f((float)GetRValue(c) / 255.0f, (float)GetGValue(c) / 255.0f, (float)GetBValue(c) / 255.0f);

	glLineWidth((GLfloat)width);
	if(style == RasterConstants::LS_SOLID)
	{
		glDisable(GL_LINE_STIPPLE);
		glDisable(GL_POLYGON_STIPPLE);
	}
	else
	{
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(1, 0x0F0F);
		glEnable(GL_POLYGON_STIPPLE);
		GLubyte ps[16] = {0x0F, 0x0F, 0x0F, 0x0F,
			0xF0, 0xF0, 0xF0, 0xF0,
			0x0F, 0x0F, 0x0F, 0x0F,
			0xF0, 0xF0, 0xF0, 0xF0};
		glPolygonStipple(ps);
	}
	return true;
}

bool CDDOpenGL::SetFillColorACAD(int color)
{
	return true;
}

bool CDDOpenGL::SetFillColorRGB(int color)
{
	return true;
}

bool CDDOpenGL::SetFillColorXOR(int color)
{
	return true;
}

bool CDDOpenGL::isValid()
{
	return true;
}

bool CDDOpenGL::SetLineWeight(int weight )
{
	return false;
}
/*
bool CDDOpenGL::SetLineWeight(int weight )
{
	if( m_lwdisplay )
	{
		if( weight < 0 )
			weight = m_lwdefault;
		if( weight <= m_lwdefault )
			weight = 1;
		else if ( m_lwdefault > 0 )	
			weight = (weight / m_lwdefault) + 1;

		glLineWidth((GLfloat)weight);
		return true;
	}
	return false;
}
*/