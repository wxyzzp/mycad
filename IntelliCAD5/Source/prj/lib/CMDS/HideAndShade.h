
#ifndef __HideAndShadeH__
#define __HideAndShadeH__


#pragma once


/*DG - 31.1.2003*/// Comment out or remove the following defining to disable thin lines rendering.
// It requires OpenGL version 1.1 or higher.
#define THIN_LINES

/*DG - 31.1.2003*/// Comment out or remove the following defining to disable hide&shade in modelspace with TILEMODE turned OFF.
#define HIDESHADE_IN_TILEMODE_OFF_MSPACE


////////////////////////////////////////////////////////////////////////////////////
// CHLRenderer

class CHLRenderer
{
protected:
	enum ImageType { kNormal = 0, kHiddenLine, kShaded};

	CHLRenderer(CIcadView* pView);
	virtual ~CHLRenderer();

#ifdef THIN_LINES
	static bool			isOpenGLv11orHigher();
#endif

	void				ComputeImageBound();
	int					CreateDisplayList(UINT nList);
	bool				SetDCPixelFormat(HDC hDC, DWORD dwFlags);
	int					SetOpenGLState();

	RECT				m_Bound;
	int					m_tilemode;
	CIcadView*			m_pView;
	ImageType			m_ImageType;

private:
	virtual void		setGlViewport();
	virtual HDC			GetTargetDC() const = 0;
	virtual int			GetBackgroundColorIndex() const = 0;
	virtual int			GetDrawingColorIndex(int colorIndex) const { return colorIndex; }
};


////////////////////////////////////////////////////////////////////////////////////
// CWindowHLRenderer

class CWindowHLRenderer : public CHLRenderer
{
public:
	CWindowHLRenderer(CIcadView *pView);
	virtual ~CWindowHLRenderer();

	virtual int		render();

protected:
	bool			Initialize();
	int				RenderSense();

	virtual HDC		GetTargetDC() const { return m_ghDC; }
	virtual int		GetBackgroundColorIndex() const { return 255; }

	HDC				m_ghDC;
 	HGLRC			m_hRC;
};


////////////////////////////////////////////////////////////////////////////////////
// CBitmapHLRenderer

class CBitmapHLRenderer : public CHLRenderer
{
public:

	CBitmapHLRenderer(CIcadView *pView, CDC* pDC, CPrintInfo* pInfo = NULL);
	virtual ~CBitmapHLRenderer();

	virtual int		render();

private:
	virtual void	setGlViewport();
	virtual HDC		GetTargetDC() const { return m_hMemDC; }
	virtual int		GetBackgroundColorIndex() const { return m_pPrintInfo ? 6 : 255; }
	virtual int		GetDrawingColorIndex(int colorIndex) const { return m_pPrintInfo ? 255 : colorIndex; }

	bool			Initialize();
	int				RenderSense();
	void			Cleanup();

	HDC				m_hOldDC,
					m_hMemDC;
	HGLRC			m_hOldRC,
					m_hMemRC;
	BITMAPINFO		m_bmi;
	LPVOID			m_pBitmapBits;
	HBITMAP			m_hDib;
	HGDIOBJ			m_hOldDib;
	CSize			m_szPage;
	CDC*			m_pDC;
	CPrintInfo*		m_pPrintInfo;
};


#endif // __HideAndShadeH__
