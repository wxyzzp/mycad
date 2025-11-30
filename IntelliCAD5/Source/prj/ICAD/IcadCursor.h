#ifndef __IcadCursorH__
#define __IcadCursorH__

#include "entityrenderer.h"

class IcadCursor
	{
public:
	IcadCursor() { readFromRegistry(); }
	static IcadCursor& AppCursor();

	bool	CursorAt( CPoint&, class IcadView *);
	bool	AlwaysUseCrosshair( bool);
	bool	GetCrosshairState() const		{ return m_alwaysUseCrosshair; }
	bool	isEnableGripsAttraction()  const { return m_bEnableGripsAttraction; }
	bool	EnableGripsAttraction(bool state);
	bool	SetVisible( bool);
	//int		GetType() const { return m_mode; }

	void	SetCrossHairSize( short percentOfScreen);
	short	GetCrossHairSize() const		{ return m_sizePercent;}
	void	SetAttractionDistance(int points);
	int		GetAttractionDistance() const	{ return m_AttractionDistance; }
	int		DrawCursor(CPoint point, int curtype, CIcadView *pView);

#ifndef _SDS_CURSOR_
	virtual void DrawCrosshair(CPoint point, struct gr_view* pViewData, bool always, CDrawDevice* pDrawDevice);
	void DrawPickbox(CPoint point, int curtype, CDrawDevice* pDrawDevice, struct gr_view* pViewData);
#else
	virtual void	DrawCrosshair( CPoint point, struct gr_view *view, CDC *cdc, bool always);
#endif
	virtual int		SetType( int);

	void	readFromRegistry();

protected:
	short	m_sizePercent;
	//int		m_mode;
	bool	m_alwaysUseCrosshair;
	bool	m_bEnableGripsAttraction;
	int		m_AttractionDistance;
    UINT    m_idcCursor; //*** ID for the cursor to be displayed in the view window.
	};


/*    //*** Cursor member variables
    HCURSOR m_prevCursor;
    UINT    m_idcCursor; //*** ID for the cursor to be displayed in the view window.
    short   m_CursorOn;
    short   m_CursorType;
    short   m_LastCursorType;
    CPoint  m_LastCursorPos;
	bool	m_bCrosshairCleared;
*/

#endif
