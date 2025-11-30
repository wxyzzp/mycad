// PRJ\ICAD\OsnapMarkerStd.cpp
// Implementaion of OsnapMarkerStd classes

#include "stdAfx.h"

#include "OsnapMarkerStd.h"
//#include "icad.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


int COsnapMarkerStd::Draw( CDC *pDC ) const
{
	// AG: this is a stub; should be overriden in descendants
	return TRUE;
}

long COsnapMarkerStd::AddRef()
{
	return InterlockedIncrement( &m_lRefCount );
}

void COsnapMarkerStd::Release()
{
	if( !InterlockedDecrement( &m_lRefCount ) )
		delete this;
}

////////////////////////////////////////////////////////////////////////////////////
int COsnapMarkerStdEnd::Draw( CDC *pDC ) const
{
	POINT pt[5];
	pt[0].x = m_pixPt.x + m_iSize; pt[0].y = m_pixPt.y + m_iSize;
	pt[1].x = m_pixPt.x - m_iSize; pt[1].y = pt[0].y;
	pt[2].x = pt[1].x; pt[2].y = m_pixPt.y - m_iSize;
	pt[3].x = pt[0].x; pt[3].y = pt[2].y;
	pt[4].x = pt[0].x; pt[4].y = pt[0].y;
	pDC->Polyline( pt, 5 );
	return TRUE;
}

int COsnapMarkerStdMid::Draw( CDC *pDC ) const
{
	POINT pt[4];
	pt[0].x = m_pixPt.x + m_iSize; pt[0].y = m_pixPt.y + m_iSize;
	pt[1].x = m_pixPt.x - m_iSize; pt[1].y = m_pixPt.y + m_iSize;
	pt[2].x = m_pixPt.x;		pt[2].y = m_pixPt.y - m_iSize;
	pt[3].x = pt[0].x; pt[3].y = pt[0].y;
	pDC->Polyline( pt, 4 );
	return TRUE;
}

int COsnapMarkerStdCen::Draw( CDC *pDC ) const
{
	POINT pt[2];
	pt[0].x = m_pixPt.x + m_iSize; pt[0].y = m_pixPt.y + m_iSize;
	pt[1].x = m_pixPt.x - m_iSize; pt[1].y = m_pixPt.y - m_iSize;
	pDC->Arc(pt[0].x, pt[0].y, pt[1].x, pt[1].y, pt[0].x, pt[0].y, pt[0].x, pt[0].y );
	return TRUE;
}

int COsnapMarkerStdNod::Draw( CDC *pDC ) const
{
	POINT pt[2];
	pt[0].x = m_pixPt.x + m_iSize; pt[0].y = m_pixPt.y + m_iSize;
	pt[1].x = m_pixPt.x - m_iSize; pt[1].y = m_pixPt.y - m_iSize;
	pDC->Ellipse( pt[0].x, pt[0].y, pt[1].x, pt[1].y );
	pDC->Polyline( pt, 2 );
	pt[0].y = m_pixPt.y - m_iSize;
	pt[1].y = m_pixPt.y + m_iSize;
	pDC->Polyline( pt, 2 );
	return TRUE;

}

int COsnapMarkerStdQua::Draw( CDC *pDC ) const
{
	POINT pt[5];
	pt[0].x = m_pixPt.x; pt[0].y = m_pixPt.y + m_iSize;
	pt[1].x = m_pixPt.x - m_iSize; pt[1].y = m_pixPt.y;
	pt[2].x = m_pixPt.x; pt[2].y = m_pixPt.y - m_iSize;
	pt[3].x = m_pixPt.x + m_iSize; pt[3].y = m_pixPt.y;
	pt[4].x = pt[0].x; pt[4].y = pt[0].y;
	pDC-> Polyline( pt, 5 );
	return TRUE;
}

int COsnapMarkerStdInt::Draw( CDC *pDC ) const
{
	POINT pt[2];
	pt[0].x = m_pixPt.x + m_iSize; pt[0].y = m_pixPt.y + m_iSize;
	pt[1].x = m_pixPt.x - m_iSize; pt[1].y = m_pixPt.y - m_iSize;
	pDC->Polyline( pt, 2 );
	pt[0].y = m_pixPt.y - m_iSize;
	pt[1].y = m_pixPt.y + m_iSize;
	pDC->Polyline( pt, 2 );
	return TRUE;
}

int COsnapMarkerStdIns::Draw( CDC *pDC ) const
{
	POINT pt[9];
	int half = m_iSize/2;
	pt[0].x = m_pixPt.x + half; pt[0].y = m_pixPt.y + m_iSize;
	pt[1].x = m_pixPt.x - m_iSize; pt[1].y = m_pixPt.y + m_iSize;
	pt[2].x = m_pixPt.x - m_iSize; pt[2].y = m_pixPt.y - half;
	pt[3].x = m_pixPt.x - half; pt[3].y = m_pixPt.y - half;
	pt[4].x = m_pixPt.x - half; pt[4].y = m_pixPt.y - m_iSize;
	pt[5].x = m_pixPt.x + m_iSize; pt[5].y = m_pixPt.y - m_iSize;
	pt[6].x = m_pixPt.x + m_iSize; pt[6].y = m_pixPt.y + half;
	pt[7].x = m_pixPt.x + half; pt[7].y = m_pixPt.y + half;
	pt[8].x = pt[0].x; pt[8].y = pt[0].y;
	pDC-> Polyline( pt, 9 );
	return TRUE;
}

int COsnapMarkerStdPer::Draw( CDC *pDC ) const
{
	POINT pt[3];
	pt[0].x = m_pixPt.x + m_iSize; pt[0].y = m_pixPt.y + m_iSize;
	pt[1].x = m_pixPt.x - m_iSize; pt[1].y = m_pixPt.y + m_iSize;
	pt[2].x = m_pixPt.x - m_iSize; pt[2].y = m_pixPt.y - m_iSize;
	pDC-> Polyline( pt, 3 );
	pt[0].x = m_pixPt.x; pt[0].y -= m_iThickness;
	pt[1].x = pt[0].x; pt[1].y = m_pixPt.y;
	pt[2].x += m_iThickness; pt[2].y = m_pixPt.y;
	pDC-> Polyline( pt, 3 );
	return TRUE;
}

int COsnapMarkerStdTan::Draw( CDC *pDC ) const
{
	POINT pt[2];
	pt[0].x = m_pixPt.x + m_iSize; pt[0].y = m_pixPt.y + m_iSize;
	pt[1].x = m_pixPt.x - m_iSize; pt[1].y = m_pixPt.y - m_iSize;
	pDC->Arc(pt[0].x, pt[0].y, pt[1].x, pt[1].y, pt[0].x, pt[0].y, pt[0].x, pt[0].y );
	pt[0].y = pt[1].y = pt[1].y - m_iThickness;
	pDC->Polyline( pt, 2 );
	return TRUE;
}

int COsnapMarkerStdNea::Draw( CDC *pDC ) const
{
	POINT pt[5];
	pt[0].x = m_pixPt.x + m_iSize; pt[0].y = m_pixPt.y + m_iSize;
	pt[1].x = m_pixPt.x - m_iSize; pt[1].y = m_pixPt.y - m_iSize;
	pt[2].x = pt[0].x; pt[2].y = pt[1].y;
	pt[3].x = pt[1].x; pt[3].y = pt[0].y;
	pt[4].x = pt[0].x; pt[4].y = pt[0].y;
	pDC-> Polyline( pt, 5 );
	return TRUE;
}

int COsnapMarkerStdApp::Draw( CDC *pDC ) const
{
	POINT pt[5];
	pt[0].x = m_pixPt.x + m_iSize; pt[0].y = m_pixPt.y + m_iSize;
	pt[1].x = m_pixPt.x - m_iSize; pt[1].y = pt[0].y;
	pt[2].x = pt[1].x; pt[2].y = m_pixPt.y - m_iSize;
	pt[3].x = pt[0].x; pt[3].y = pt[2].y;
	pt[4].x = pt[0].x; pt[4].y = pt[0].y;
	pDC->Polyline( pt, 5 );
	pt[1].y = pt[2].y;
	pDC->Polyline( pt, 2 );
	pt[3].y = pt[4].y;
	pDC->Polyline( (pt+3), 2 );
	return TRUE;
}

int COsnapMarkerStdPla::Draw( CDC *pDC ) const
{
	POINT pt[2];
	int half = m_iSize/2;
	pt[0].x = m_pixPt.x - m_iSize; pt[0].y = m_pixPt.y;
	pt[1].x = m_pixPt.x; pt[1].y = pt[0].y;
	pDC->Polyline( pt, 2 );
	pt[0].x = m_pixPt.x + half;
	pt[1].x = m_pixPt.x + m_iSize;
	pDC->Polyline( pt, 2 );
	return TRUE;
}

int COsnapMarkerStdPar::Draw( CDC *pDC ) const
{
	POINT pt[2];
	int quater = m_iSize/4;
	pt[0].x = m_pixPt.x + m_iSize + quater; pt[0].y = m_pixPt.y + m_iSize + quater;
	pt[1].x = m_pixPt.x - m_iSize + quater; pt[1].y = m_pixPt.y - m_iSize + quater;
	pDC->Polyline( pt, 2 );
	pt[0].x = m_pixPt.x + m_iSize - quater; pt[0].y = m_pixPt.y + m_iSize - quater;
	pt[1].x = m_pixPt.x - m_iSize - quater; pt[1].y = m_pixPt.y - m_iSize - quater;
	pDC->Polyline( pt, 2 );
	return TRUE;
}
