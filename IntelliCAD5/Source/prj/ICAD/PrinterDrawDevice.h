/* C:\ICADDEV\PRJ\ICAD\PRINTERDEVICE.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#ifndef	_PRINTERDEVICE_H
#define _PRINTERDEVICE_H

#include "DDPlaneBuffer.h"
#include "IcadATL.h"
namespace ICAD_AUTOMATION
{
#include "IcadTLB.h"
}
using namespace ICAD_AUTOMATION;

// Redesigned by Denis Petrov
// Description: This class handles output to printer using GDI API
class CDDPrinter: public CDDPlaneBuffer
{
public:
	CDDPrinter(IIcadPlotProperties* properties);

	virtual bool create(CDC* pDC, int flags = eDOUBLE_BUFFER, int width = 0, int height = 0, HPALETTE hPalette = NULL);
	virtual bool resize(CDC* pDC, int width, int height);
	virtual void beginDraw(CDC* pDC, gr_view* pView);

	virtual int setDrawBuffer(EDrawBuffer mode);

	virtual bool SetLineColorACAD(int color, int width = 1, RasterConstants::LineStyle style = RasterConstants::LS_SOLID);
	virtual bool SetFillColorACAD(int color);
/********************************************************************************
 * Author:	Dmitry Gavrilov
 *
 * Purpose:	Draw one or more Bezier curves defined by "cPoints" points in "lppt" array.
 *
 * Returns:	True in success and false in failure.
 ********************************************************************************/
	virtual bool PolyBezier(const POINT* lppt, DWORD cPoints);
	virtual bool Polyline(const POINT *lppt, int iPoints);
	virtual bool Polygon(const POINT *lppt, int iPoints, bool onlyFill);
	virtual bool TextOut(const int xCoord, const int yCoord, POINT BltArea[2], const CFont *font, int ACADcolor, const char *text);

	virtual bool SetLineWeight(int weight );
	
protected:
	CComPtr<IIcadPlotProperties>	m_properties;
	CComPtr<IIcadPlotterPens>		m_pens;
	float							m_scale;
	bool							m_convertToInches;
	bool							m_bPreview;
	bool							m_bUsePath;	//Use GDI Path for redering.

	// EBATECH(CNBR) 24-03-2003 Bugzilla#78471 Support Lineweight
	bool							m_useLWeight;
	bool							m_scaleLWeight;
	
	bool GetPenColor(IIcadPlotterPens* pens, int* color, double* weight);
	
	const static CString			m_RegKey;
};

#endif  // _PRINTERDEVICE_H

