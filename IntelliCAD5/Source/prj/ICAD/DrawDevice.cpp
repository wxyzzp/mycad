#include "icad.h"
#include "gr.h"
#include "DrawDevice.h"

void CDrawDevice::beginDraw(CDC* pDC, gr_view* pView)
{
	m_pDC = pDC;
	m_hdc = pDC->GetSafeHdc();

	if( pView )
	{
		m_lwdisplay = pView->lwdisplay;
		m_lwdefault = pView->lwdefault;
	}
	else
	{
		m_lwdisplay = m_lwdefault = 0;
	}
}

CDC* CDrawDevice::getCDC()
{
	if(m_pDC != NULL)
		return m_pDC;
	else
		return CDC::FromHandle(m_hdc);
}

HDC CDrawDevice::getHdc()
{
	return m_hdc;
}
