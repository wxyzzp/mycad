// OptionsSnapDlgPic.cpp : implementation file
//

#include "stdafx.h"
#include "OptionsSnapDlgPic.h"
#include "icad.h"
#include "icadApi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionsSnapDlgPic

COptionsSnapDlgPic::COptionsSnapDlgPic()
{
}

COptionsSnapDlgPic::~COptionsSnapDlgPic()
{
}


BEGIN_MESSAGE_MAP(COptionsSnapDlgPic, CStatic)
	//{{AFX_MSG_MAP(COptionsSnapDlgPic)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsSnapDlgPic message handlers

void COptionsSnapDlgPic::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	Draw(&dc);
	// Do not call CStatic::OnPaint() for painting messages
}

void COptionsSnapDlgPic::Draw( CDC *pDC )
{
	CRect rect;
	POINT centerPt;
	POINT pt[5];
	struct resbuf rb;
	CPen pen, *savePen = NULL;
	CBrush brush;
	CPalette *savePal = NULL;
	BOOL bReleaseDC = FALSE;
	int iBkgColor = 256;
	
	// check pDC pointer in case it was called directly, not from WM_PAINT handler
	if( !pDC )
		pDC = GetDC();

	if( pDC )
		bReleaseDC = TRUE;
	else
		return;

	GetClientRect( &rect );
	if( m_iSize > rect.Height() || m_iSize > rect.Width() )
		return;
	
	centerPt.x = (rect.right - rect.left)/2;
	centerPt.y = (rect.bottom - rect.top)/2;

	if(SDS_getvar(NULL,DB_QBKGCOLOR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES) != RTNORM)
		iBkgColor = rb.resval.rint;

	if( pen.CreatePen(PS_SOLID, m_iThickness, SDS_PenColorFromACADColor( m_iColor )) &&
		brush.CreateSolidBrush(SDS_BrushColorFromACADColor(iBkgColor)) )
	{

		savePen = pDC->SelectObject( &pen );
		savePal = pDC->SelectPalette( SDS_CMainWindow->m_pPalette, TRUE ); //??
	
		pDC->FillRect( &rect, &brush );
		brush.DeleteObject();

		pt[0].x = centerPt.x + m_iSize; pt[0].y = centerPt.y + m_iSize;
		pt[1].x = centerPt.x - m_iSize; pt[1].y = pt[0].y;
		pt[2].x = pt[1].x; pt[2].y = centerPt.y - m_iSize;
		pt[3].x = pt[0].x; pt[3].y = pt[2].y;
		pt[4].x = pt[0].x; pt[4].y = pt[0].y;
		pDC->Polyline( pt, 5 );

		pDC->SelectPalette( savePal, TRUE ); 
		pDC->SelectObject( savePen );

		pen.DeleteObject();
	}

	if( bReleaseDC )
		ReleaseDC( pDC );
}
