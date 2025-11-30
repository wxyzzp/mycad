/* D:\ICADDEV\PRJ\ICAD\ICADENTITYRENDERER.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 *
 */

#include "Icad.h"/*DNT*/
#include "IcadDoc.h"/*DNT*/
#include "IcadEntityRenderer.h"/*DNT*/
#include "IcadRaster.h"/*DNT*/
#include "IcadPrintDia.h"/*DNT*/
#include "UCSIcon.h"
#include "icadgridiconsnap.h"

#include "gr.h"
#include "DoBase.h"

#include "cmds.h"/*DNT*/
#include "PrintDialog.h"/*DNT*/
#include "styletabrec.h"/*DNT*/
#include "vxtabrec.h"/*DNT*/
#include "IcadView.h"/*DNT*/
#include "TrueTypeUtils.h"/*DNT*/
#include "DbMText.h"
#include "irasdef.h"
#include "cvisioraster.h"
#include "objects.h"
#include "DbSortentsTable.h"
#include "cmdsDraworder.h"

#ifdef BMP_USE_HIDESHADE_STATUS
 #include "hideandshade.h"
#endif

extern char 		* (*Ras_pfnGetLastError) ();
extern CVisioRaster * (*Ras_newRaster) ();
extern void (*Ras_deleteRaster)(CVisioRaster *);
BOOL Ras_ProgressUpdate(short nPos);
void Ras_rp2pix(
	struct gr_view *p_viewp,
	sds_real		 p_rx,
	sds_real		 p_ry,
	double			*p_ixp,
	double			*p_iyp);

#if defined(USE_SMARTHEAP)
	#include "smrtheap.h"/*DNT*/
#endif

// globals that formerly were in icaddraw.cpp
extern bool SDS_DontDrawVPGuts;
extern bool SDS_DrawVPGutsOnly;
extern bool SDS_DontBitBlt;
extern RGBQUAD SDS_PaletteColors[];

extern void	getViewPortClippingRectangle(CIcadView *pView,
										 db_viewport *pViewport,
										 gr_view *pgrView,
										 gr_view *pgrViewVP,
										 sds_point ptMin,
										 sds_point ptMax);
// Constructors

// The default constructor
CIcadEntityRenderer::CIcadEntityRenderer()
{
	ASSERT(FALSE);
}

CIcadEntityRenderer::CIcadEntityRenderer(CIcadDoc* pDoc, CIcadView* pIcadView)
{
	ASSERT(pDoc != NULL);

	m_pDoc = pDoc;
	m_pView = pIcadView;
	m_pDrawing = pDoc->m_dbDrawing;
	m_pDD = NULL;
	m_pUCS = new CUCSIcon(m_pDrawing);
	m_pGrid = new CGrid(m_pDrawing);

	resbuf var;
	SDS_getvar(NULL, DB_QFILLMODE, &var, m_pDrawing, &SDS_CURCFG, &SDS_CURSES);
	m_fillmode = var.resval.rint;
	SDS_getvar(NULL, DB_QTILEMODE, &var, m_pDrawing, &SDS_CURCFG, &SDS_CURSES);
	m_tilemode = var.resval.rint;
	SDS_getvar(NULL, DB_QCVPORT, &var, m_pDrawing, &SDS_CURCFG, &SDS_CURSES);
	m_cvport = var.resval.rint;

	if(SDS_getvar("SORTENTS"/*DNT*/, 0, &var, m_pDrawing, &SDS_CURCFG, &SDS_CURSES) == RTNORM)
		m_sortents = var.resval.rint;
	else
		m_sortents = 0;

	sds_name sdsTableName;
	m_pSortingTable = NULL;
	if(isSortingOn())
		if(m_tilemode == 1 || (m_tilemode == 0 && m_cvport != 1))
		{
			if(cmd_getSortEntsTable(sdsTableName, m_pDrawing->ret_stockhand(DB_SHI_BLKREC_MSPACE)))
				m_pSortingTable = (CDbSortentsTable*)sdsTableName[0];
		}
		else
		{
			if(cmd_getSortEntsTable(sdsTableName, m_pDrawing->ret_stockhand(DB_SHI_BLKREC_PSPACE)))
				m_pSortingTable = (CDbSortentsTable*)sdsTableName[0];
		}
}

CIcadEntityRenderer::CIcadEntityRenderer(db_drawing* pDrawing, CIcadView* pIcadView)
{
	ASSERT(pDrawing != NULL);
	ASSERT(pIcadView != NULL);

	m_pDoc = NULL;
	m_pView = pIcadView;
	m_pDrawing = pDrawing;
	m_pDD = NULL;
	m_pUCS = new CUCSIcon(pDrawing);
	m_pGrid = new CGrid(pDrawing);

	resbuf var;
	SDS_getvar(NULL, DB_QFILLMODE, &var, m_pDrawing, &SDS_CURCFG, &SDS_CURSES);
	m_fillmode = var.resval.rint;
	SDS_getvar(NULL, DB_QTILEMODE, &var, m_pDrawing, &SDS_CURCFG, &SDS_CURSES);
	m_tilemode = var.resval.rint;
	SDS_getvar(NULL, DB_QCVPORT, &var, m_pDrawing, &SDS_CURCFG, &SDS_CURSES);
	m_cvport = var.resval.rint;

	ASSERT(m_fillmode == m_pView->m_pCurDwgView->fillmode);

	if(SDS_getvar("SORTENTS"/*DNT*/, 0, &var, m_pDrawing, &SDS_CURCFG, &SDS_CURSES) == RTNORM)
		m_sortents = var.resval.rint;
	else
		m_sortents = 0;

	sds_name sdsTableName;
	m_pSortingTable = NULL;
	if(isSortingOn())
		if(m_tilemode == 1 || (m_tilemode == 0 && m_cvport != 1))
		{
			if(cmd_getSortEntsTable(sdsTableName, m_pDrawing->ret_stockhand(DB_SHI_BLKREC_MSPACE)))
				m_pSortingTable = (CDbSortentsTable*)sdsTableName[0];
		}
		else
		{
			if(cmd_getSortEntsTable(sdsTableName, m_pDrawing->ret_stockhand(DB_SHI_BLKREC_PSPACE)))
				m_pSortingTable = (CDbSortentsTable*)sdsTableName[0];
		}
}

// The Destructor
CIcadEntityRenderer::~CIcadEntityRenderer()
{
	delete m_pUCS;
	delete m_pGrid;
}

// Accessor functions
CIcadView* CIcadEntityRenderer::getIcadView()
{
	return m_pView;
}

int CIcadEntityRenderer::calcDrawmode(int highlight,
									  int xor,
									  int undraw)
{
	int drawmode=1;

	if(highlight==0 && xor==0 && undraw==0)
	{
		drawmode=1;
	}
	else if(highlight==0 && xor==0 && undraw==1)
	{
		drawmode=2;
	}
	else if(highlight==1 && xor==0 && undraw==0)
	{
		drawmode=3;
	}
	else if(highlight==0 && xor==0 && undraw==0)
	{
		drawmode=4;
	}

	return drawmode;
}

int CIcadEntityRenderer::drawGrid()
{
	ASSERT(m_pView != NULL);
	ASSERT(m_pDrawing != NULL);
	ASSERT(m_pUCS != NULL);

	m_pDD = m_pView->GetDrawDevice();
	CDC* pDC = m_pView->GetDC();
	CPalette* pPrevPalette = pDC->SelectPalette(SDS_CMainWindow->m_pPalette, TRUE);
	if(m_pView->m_pClipRectDC)
		pDC->IntersectClipRect(m_pView->m_pClipRectDC);

	m_pDD->beginDraw(pDC, m_pView->m_pCurDwgView);
	int result = m_pGrid->draw(m_pView->m_pCurDwgView, m_pDD, m_pView);
	int drawBuffer = m_pDD->setDrawBuffer(CDrawDevice::eFRONT);

	result = m_pGrid->draw(m_pView->m_pCurDwgView, m_pDD, m_pView);

	m_pDD->setDrawBuffer((CDrawDevice::EDrawBuffer)drawBuffer);
	m_pDD->endDraw();
	m_pView->ReleaseDrawDevice(m_pDD);
	m_pView->ReleaseDC(pDC);
	return result;
}

int CIcadEntityRenderer::drawUCSIcon(sds_point vdir, const sds_real* sclfact, int unpaint)
{
	ASSERT(m_pView != NULL);
	ASSERT(m_pDrawing != NULL);
	ASSERT(m_pUCS != NULL);
	m_pDD = m_pView->GetDrawDevice();
	CDC* pDC = m_pView->GetDC();
	m_pDD->beginDraw(pDC, m_pView->m_pCurDwgView);

	int result = m_pUCS->draw(vdir, sclfact, unpaint, m_pDD, m_pView);

	m_pDD->endDraw();
	m_pView->ReleaseDrawDevice(m_pDD);
	m_pView->ReleaseDC(pDC);
	return result;
}

int CIcadEntityRenderer::drawVecs(const resbuf* pList, sds_matrix transform)
{
	CMatrix4 t;
	if(transform != NULL)
	{
		t(0, 0) = transform[0][0];
		t(0, 1) = transform[0][1];
		t(0, 2) = transform[0][2];
		t(0, 3) = transform[0][3];
		t(1, 0) = transform[1][0];
		t(1, 1) = transform[1][1];
		t(1, 2) = transform[1][2];
		t(1, 3) = transform[1][3];
		t(2, 0) = transform[2][0];
		t(2, 1) = transform[2][1];
		t(2, 2) = transform[2][2];
		t(2, 3) = transform[2][3];
		t(3, 3) = 1.0;
	}
	CD3 segment[2];
	CD3 projectedSegment[2];
	POINT pixelsSegment[2];
	int color = 7;
	int hilite = 0;

	struct resbuf* pCur;
	POSITION pos = m_pDoc->GetFirstViewPosition();
	bool bDrawToAllViews = m_pView == NULL ? true : false;
	while(pos != NULL)
	{
		if(bDrawToAllViews)
			m_pView = (CIcadView*)m_pDoc->GetNextView(pos);
		ASSERT_KINDOF(CIcadView, m_pView);
		ASSERT(!m_pView->IsPrinting());
		ASSERT(m_pView->m_pCurDwgView != NULL);

		m_pDD = m_pView->GetDrawDevice();
		CDC* pDC = m_pView->GetDC();
		m_pDD->beginDraw(pDC, m_pView->m_pCurDwgView);
		if(m_pDD->is3D())
			m_pDD->setDrawBuffer(CDrawDevice::eFRONT);

		for(pCur = (struct resbuf*)pList; pCur != NULL; pCur = pCur->rbnext)
		{
			if(pCur->restype == RT3DPOINT || pCur->restype==RTPOINT)
			{
				if(pCur->rbnext == NULL || (pCur->rbnext->restype != RT3DPOINT && pCur->rbnext->restype != RTPOINT))
				{
					m_pDD->endDraw();
					m_pView->ReleaseDrawDevice(m_pDD);
					m_pView->ReleaseDC(pDC);
					return RTERROR;
				}

				if(transform == NULL)
				{
					segment[0] = pCur->resval.rpoint;
					segment[1] = pCur->rbnext->resval.rpoint;
				}
				else
				{
					t.multiplyRight(segment[0], pCur->resval.rpoint);
					t.multiplyRight(segment[1], pCur->rbnext->resval.rpoint);
				}

				int rop;
				if(color == -1)
				{
					rop = R2_XORPEN;
					m_pDD->SetDrawMode(RasterConstants::MODE_XOR);
				}
				else
				{
					rop = R2_COPYPEN;
					m_pDD->SetDrawMode(RasterConstants::MODE_COPY);
				}
				if(hilite)
				{
					if(SDS_PaletteColors[255].rgbRed == 255 && SDS_PaletteColors[255].rgbGreen == 255 && SDS_PaletteColors[255].rgbBlue == 255)
						m_pDD->SetLineColorACAD(-1, 1, RasterConstants::LS_DOT);
					else
						m_pDD->SetLineColorACAD(256, 1, RasterConstants::LS_DOT);
				}
				else
					if(R2_XORPEN == rop)
						m_pDD->SetLineColorXOR(color, 1);
					else
						m_pDD->SetLineColorACAD(color, 1);

				gr_ucs2rp((double*)segment[0], projectedSegment[0], m_pView->m_pCurDwgView);
				gr_ucs2rp((double*)segment[1], projectedSegment[1], m_pView->m_pCurDwgView);
				int result = gr_vect_rp2pix(projectedSegment[0], projectedSegment[1], (int*)pixelsSegment, m_pView->m_pCurDwgView);
				// D.Gavrilov draw etc. only in these cases
				if(result == 0 || result == 2)
				{
					if(m_pDD->is3D())
						m_pDD->Polyline((POINT*)segment, 2);
					else
						m_pDD->Polyline(pixelsSegment, 2);

					// Update blit region
					//
					int i;
					for(i = 0; i < 2; ++i)
					{
						if(m_pView->m_RectBitBlt==0)
						{
							m_pView->m_RectBitBltPt[0].x = pixelsSegment[i].x;
							m_pView->m_RectBitBltPt[0].y = pixelsSegment[i].y;
							m_pView->m_RectBitBltPt[1].x = pixelsSegment[i].x;
							m_pView->m_RectBitBltPt[1].y = pixelsSegment[i].y;
							m_pView->m_RectBitBlt = 1;
						}
						else
						{
							if(m_pView->m_RectBitBltPt[0].x > pixelsSegment[i].x)
								m_pView->m_RectBitBltPt[0].x = pixelsSegment[i].x;
							if(m_pView->m_RectBitBltPt[0].y > pixelsSegment[i].y)
								m_pView->m_RectBitBltPt[0].y = pixelsSegment[i].y;
							if(m_pView->m_RectBitBltPt[1].x < pixelsSegment[i].x)
								m_pView->m_RectBitBltPt[1].x = pixelsSegment[i].x;
							if(m_pView->m_RectBitBltPt[1].y < pixelsSegment[i].y)
								m_pView->m_RectBitBltPt[1].y = pixelsSegment[i].y;
						}
					}
				}
				/* Step to 2nd point and continue */
				pCur = pCur->rbnext;
			}
			else
				if(pCur->restype == RTSHORT)
				{
					color = pCur->resval.rint;
					// Now set color and hilite for sds_grdraw():
					if(color < -254)
					{
						color=-1;
						hilite=1;
					}
					else
						if(color < 0)
						{	// -254 through -1
							color = -color;
							hilite=1;
						}
						else
							if(color < 255)
							{	// 0 through 254 (see quirks)
								hilite = 0;
							}
							else
							{  /* greater than 254 */
								color = -1;
								hilite = 0;
							}
				}
				else
				{
					m_pDD->endDraw();
					m_pView->ReleaseDrawDevice(m_pDD);
					m_pView->ReleaseDC(pDC);
					return RTERROR;
				}
		}

		CRect r;
		m_pView->GetClientRect(&r);

		if(m_pView->m_RectBitBltPt[0].x < 0)
			m_pView->m_RectBitBltPt[0].x = 0;
		if(m_pView->m_RectBitBltPt[0].y < 0)
			m_pView->m_RectBitBltPt[0].y = 0;
		if(m_pView->m_RectBitBltPt[1].x > r.Width())
			m_pView->m_RectBitBltPt[1].x = r.Width();
		if(m_pView->m_RectBitBltPt[1].y > r.Height())
			m_pView->m_RectBitBltPt[1].y = r.Height();
		m_pView->m_RectBitBlt = 0;

		m_pDD->copyToFront(m_pView->m_RectBitBltPt[0].x, m_pView->m_RectBitBltPt[0].y,
			m_pView->m_RectBitBltPt[1].x - m_pView->m_RectBitBltPt[0].x + 1,
			m_pView->m_RectBitBltPt[1].y - m_pView->m_RectBitBltPt[0].y + 1);

		m_pDD->endDraw();
		m_pView->ReleaseDrawDevice(m_pDD);
		m_pView->ReleaseDC(pDC);
		if(!bDrawToAllViews)
			break;
	}
	return RTNORM;
}

int CIcadEntityRenderer::drawLine(const sds_point from, const sds_point to, int color, int highlight, int xor, bool bToAllBuffers)
{
	CD3 projectedSegment[2];
	POINT pixelsSegment[2];
	POSITION pos = m_pDoc->GetFirstViewPosition();
	bool bDrawToScreen;
	bool bDrawToAllViews = m_pView == NULL ? true : false;
	while(pos != NULL)
	{
		bDrawToScreen = (bToAllBuffers && !SDS_DontBitBlt);
		if(bDrawToAllViews)
			m_pView = (CIcadView*)m_pDoc->GetNextView(pos);

		ASSERT_KINDOF(CIcadView, m_pView);
		ASSERT(!m_pView->IsPrinting());
		ASSERT(m_pView->m_pCurDwgView != NULL);

		m_pDD = m_pView->GetDrawDevice();
		ASSERT(m_pDD->IsValid());
		CDC* pDC = m_pView->GetDC();
		m_pDD->beginDraw(pDC, m_pView->m_pCurDwgView);

		// Is the following line really necessary???
		//	Shouldn't this have been set when the DC was created??
		//
		//SelectPalette(pDC->GetSafeHdc(), (HPALETTE)SDS_CMainWindow->m_pPalette->GetSafeHandle(), TRUE);
		int rop;
		if(color == -1 || xor)
		{
			rop = R2_XORPEN;
			m_pDD->SetDrawMode(RasterConstants::MODE_XOR);
		}
		else
		{
			rop = R2_COPYPEN;
			m_pDD->SetDrawMode(RasterConstants::MODE_COPY);
		}
		if(highlight)
		{
			if(SDS_PaletteColors[255].rgbRed == 255 && SDS_PaletteColors[255].rgbGreen == 255 && SDS_PaletteColors[255].rgbBlue == 255)
				m_pDD->SetLineColorACAD(-1, 1, RasterConstants::LS_DOT);
			else
				m_pDD->SetLineColorACAD(256, 1, RasterConstants::LS_DOT);
		}
		else
			if(R2_XORPEN == rop)
				m_pDD->SetLineColorXOR(color, 1);
			else
				m_pDD->SetLineColorACAD(color, 1);

		gr_ucs2rp((double*)to, projectedSegment[0], m_pView->m_pCurDwgView);
		gr_ucs2rp((double*)from, projectedSegment[1], m_pView->m_pCurDwgView);
		int rc = gr_vect_rp2pix(projectedSegment[0], projectedSegment[1], (int*)pixelsSegment, m_pView->m_pCurDwgView);
		// D.Gavrilov draw etc. only in these cases
		if(rc == 0 || rc == 2)
		{
			if(m_pDD->is3D())
			{
				projectedSegment[0] = from;
				projectedSegment[1] = to;
/*
				sds_real a00 = 0.5 * m_pView->m_pCurDwgView->viewsize * (double)m_pView->m_iWinX / (double)m_pView->m_iWinY;
				sds_real a11 = 0.5 * m_pView->m_pCurDwgView->viewsize;
				projectedSegment[0][0] = a00 * from[0];
				projectedSegment[0][1] = a11 * from[1];
				projectedSegment[0][2] = from[2];
				projectedSegment[1][0] = a00 * to[0];
				projectedSegment[1][1] = a11 * to[1];
				projectedSegment[1][2] = to[2];
*/
				m_pDD->Polyline((POINT*)projectedSegment, 2);
				bDrawToScreen = true;
			}
			else
				m_pDD->Polyline(pixelsSegment, 2);
			if(bDrawToScreen)
			{
				//gr_ucs2rp((double*)to, projectedSegment[0], m_pView->m_pCurDwgView);
				//gr_ucs2rp((double*)from, projectedSegment[1], m_pView->m_pCurDwgView);
				int drawBuffer = m_pDD->setDrawBuffer(CDrawDevice::eFRONT);
				if(m_pDD->is3D())
					m_pDD->Polyline((POINT*)projectedSegment, 2);
				else
					m_pDD->Polyline(pixelsSegment, 2);
				m_pDD->setDrawBuffer((CDrawDevice::EDrawBuffer)drawBuffer);
			}

			// Update blit region
			//
			for(int i = 0; i < 2; ++i)
			{
				if(m_pView->m_RectBitBlt==0)
				{
					m_pView->m_RectBitBltPt[0].x = pixelsSegment[i].x;
					m_pView->m_RectBitBltPt[0].y = pixelsSegment[i].y;
					m_pView->m_RectBitBltPt[1].x = pixelsSegment[i].x;
					m_pView->m_RectBitBltPt[1].y = pixelsSegment[i].y;
					m_pView->m_RectBitBlt = 1;
				}
				else
				{
					if(m_pView->m_RectBitBltPt[0].x > pixelsSegment[i].x)
						m_pView->m_RectBitBltPt[0].x = pixelsSegment[i].x;
					if(m_pView->m_RectBitBltPt[0].y > pixelsSegment[i].y)
						m_pView->m_RectBitBltPt[0].y = pixelsSegment[i].y;
					if(m_pView->m_RectBitBltPt[1].x < pixelsSegment[i].x)
						m_pView->m_RectBitBltPt[1].x = pixelsSegment[i].x;
					if(m_pView->m_RectBitBltPt[1].y < pixelsSegment[i].y)
						m_pView->m_RectBitBltPt[1].y = pixelsSegment[i].y;
				}
			}
		}
		m_pDD->endDraw();
		m_pView->ReleaseDrawDevice(m_pDD);
		m_pView->ReleaseDC(pDC);

		if(!bDrawToAllViews)
			break;
	}
	return RTNORM;
}

int CIcadEntityRenderer::drawEntitySpecial(struct gr_displayobject*& pSpecialDO,
		CDrawDevice* pDrawDevice,
		db_drawing* pDrawing,
		struct gr_view* gView,
		int hl,
		int xor,
		int undraw)
{
	CIcadView* pView = getIcadView();
	db_handitem* pEntity = pSpecialDO->GetSourceEntity();

	if(pEntity->ret_type() == DB_SOLID || pEntity->ret_type() == DB_TRACE || 
		pEntity->ret_type() == DB_POLYLINE || pEntity->ret_type() == DB_LWPOLYLINE)
	{
		CD3 zAxis((double*)&(gView->rpaxis[2])), zUCS(pSpecialDO->chain);
		if((zUCS % zAxis).isNullWithin())
		{
			if(pSpecialDO->type & DISP_OBJ_PTS_FILLABLE)
				return 1;
			for(struct gr_displayobject* pCurDO = pSpecialDO; pCurDO != NULL && pCurDO->GetSourceEntity() == pEntity; pCurDO = pCurDO->next)
				pCurDO->type |= DISP_OBJ_PTS_FILLABLE;
		}
		else
		{
			if((pSpecialDO->type & DISP_OBJ_PTS_FILLABLE) == 0)
				return 1;
			for(struct gr_displayobject* pCurDO = pSpecialDO; pCurDO != NULL && pCurDO->GetSourceEntity() == pEntity; pCurDO = pCurDO->next)
				pCurDO->type &= ~DISP_OBJ_PTS_FILLABLE;
		}
		return 1;
	}

	if(!Ras_InitRaster())
		return 0;

	int drawmode = calcDrawmode(hl, xor, undraw);
	int rasterQuality;
	char name[IC_ACADBUF];
	CVisioRaster* pRaster = NULL;
	IRas::ErrorCode err;
	int dispprops;
	int wasloaded = 0;
	sds_point ipt,imagesz, uvect, vvect;
	struct resbuf* elist;
	BOOL save_bMapToRGBColor;

	db_imagedef* pImageDef;
	pEntity->get_340((db_handitem**)&pImageDef);

	pEntity->get_10(ipt);
	pEntity->get_70(&dispprops);
	pEntity->get_13(imagesz);
	pEntity->get_11(uvect);
	pEntity->get_12(vvect);
	char* pError;
	pError = Ras_pfnGetLastError();

	if(pImageDef == NULL || !(dispprops & 1))
		return 0;

	char* pFileName;
	pImageDef->get_1(&pFileName);
	pRaster = (CVisioRaster*)pImageDef->ret_imagedisp();
	// Check to see if the image was loaded the last time the drawing was saved,
	// or regened. Allows for setting the 280 programmatically on or off too.
	pImageDef->get_280(&wasloaded);
	// It the image is not loaded, load it.
	if(pRaster == NULL && wasloaded)
	{
		if(pImageDef->ret_looked4image())
			return 0;

		if(RTNORM != sds_findfile(pFileName, name))
		{
			if(NULL == strchr(pFileName, '\\') || RTNORM != sds_findfile(strchr(pFileName, '\\') + 1, name))
			{
				sds_printf(ResourceString(IDC_ICADRASTER__N___UNABLE_T_5, "\n-- Unable to locate image file %s. --\n" ), pFileName);
				// Hitachi fix to let them know when an image was not found.
				pImageDef->set_280(0);
				return 1;	// I'm changing this to return 1 instead of 0, so the ASSERT() below the call to this
							// function doesn't fire. The user has been warned via the histor window
							// that this image file could not be found, so we will return 1, instead
			}
		}
		// Only set this if the image was found.
		pImageDef->set_looked4image(1);
		// Create a new raster image
		pRaster = Ras_newRaster();
		if(pRaster == NULL)
		{
			// EBATECH(CNBR) 2003/6/1 IDC_ICADRASTER_PROBLEM_LOAD_10 has 3 arguments.
			char * szError;
			szError = Ras_pfnGetLastError();

			sds_printf(ResourceString(IDC_ICADRASTER_PROBLEM_LOAD_10, "Problem loading image %s.\n(%i) %s" ), pFileName, pError, szError);
			//sds_printf(ResourceString(IDC_ICADRASTER_PROBLEM_LOAD_10, "Problem loading image %s.\n %s" ), pFileName, pError);
			// EBATECH(CNBR) ]-
			return 0;
		}
		// Set the units
		pRaster->setUnits(IRas::Inch);

		char dllpath[IC_ACADBUF];
		if(RTNORM != sds_findfile(SDS_PROGRAMFILE, dllpath))
		{
			char * szError;
			szError = Ras_pfnGetLastError();

			sds_printf(ResourceString(IDC_ICADRASTER_PROBLEM_LOAD_10, "Problem loading image %s.\n(%i) %s" ), pFileName, pError, szError);
			delete pRaster;
			pImageDef->set_imagedisp(NULL);
			return 0;
		}
		if(strrchr(dllpath, '\\'))
			*strrchr(dllpath, '\\') = 0;
		SDS_ProgressStart();
		sds_grtext(-2, ResourceString(IDC_ICADRASTER_LOADING_IMAGE_6, "Loading image..." ), 0);
		if(IRas::eSuccess != pRaster->loadImage(name, Ras_ProgressUpdate, dllpath))
		{
			char * szError;
			szError = Ras_pfnGetLastError();

			sds_printf(ResourceString(IDC_ICADRASTER_PROBLEM_LOAD_10, "Problem loading image %s.\n(%i) %s" ), pFileName, pError, szError);
			SDS_ProgressStop();
			delete pRaster;
			pImageDef->set_imagedisp(NULL);
			return 0;
		}
		sds_grtext(-2, ResourceString(IDC_ICADRASTER__7, "" ), 0);
		SDS_ProgressStop();
	}

	if(pRaster == NULL)
				return 1;	// I'm changing this to return 1 instead of 0, so the ASSERT() below the call to this
							// function doesn't fire. The user has been warned via the histor window
							// that this image file could not be found, so we will return 1, instead

	pImageDef->set_imagedisp((void*)pRaster);

	// Bug# 6408 Hitachi bug fix.
	// MHB Assuming the frame has already been drawn.
	// Hitachi guys sitting right beside me when I made this change.
	if(!pRaster->isLoaded())
		return 0;

	sds_point ptRasterLL, ptRasterUL, ptRasterUR, ptRasterLR, ptViewLL, ptViewUR;

	ASSERT(pSpecialDO->npts == 4);
	ic_ptcpy(ptRasterLL, &pSpecialDO->chain[0]);
	ic_ptcpy(ptRasterLR, &pSpecialDO->chain[3]);
	ic_ptcpy(ptRasterUR, &pSpecialDO->chain[6]);
	ic_ptcpy(ptRasterUL, &pSpecialDO->chain[9]);

	//When setting the extent, calculate the minx, miny and maxx and maxy.	The extent is a rectangle
	//with axes parallel to the x & y axes.  It is not the LL and UR corner of the raster rectangle.

	//NOTE : we will set the extent only if it has not already been set.  The ent extent is maintained by
	//the display system (SDS_UpdateEntDisp) and is updated everytime the object is displayed, rotated, or
	//view changes are made.  IMO, we do not need to be setting the extent here at all, but will continue
	//to set it if it has never been set.  AM 12/16/98
	sds_point entextent[2];
	pEntity->get_extent(entextent[0],entextent[1]);
	// If it appears to be unset, set the local extents now
	if (isEqualZeroWithin(entextent[0][0]) && 
		isEqualZeroWithin(entextent[0][1]) && 
		isEqualZeroWithin(entextent[0][2]) &&
		isEqualZeroWithin(entextent[1][0]) && 
		isEqualZeroWithin(entextent[1][1]) && 
		isEqualZeroWithin(entextent[1][2]))
	{
		sds_point ptMin, ptMax;
		int i;
		for(i=0; i<3; i++)
		{
			ptMin[i] = min (ptRasterLL[i], min(ptRasterLR[i], min(ptRasterUR[i], ptRasterUL[i])));
			ptMax[i] = max (ptRasterLL[i], max(ptRasterLR[i], max(ptRasterUR[i], ptRasterUL[i])));
		}
		pEntity->set_extent(ptMin,ptMax);
	}

	gr_ucs2rp(ptRasterLL, ptRasterLL, gView);
	Ras_rp2pix(gView, ptRasterLL[0], ptRasterLL[1], &ptRasterLL[0], &ptRasterLL[1]);
	gr_ucs2rp(ptRasterLR, ptRasterLR, gView);
	Ras_rp2pix(gView, ptRasterLR[0], ptRasterLR[1], &ptRasterLR[0], &ptRasterLR[1]);
	gr_ucs2rp(ptRasterUL, ptRasterUL, gView);
	Ras_rp2pix(gView, ptRasterUL[0], ptRasterUL[1], &ptRasterUL[0], &ptRasterUL[1]);
	gr_ucs2rp(ptRasterUR,ptRasterUR,gView);
	Ras_rp2pix(gView,ptRasterUR[0],ptRasterUR[1],&ptRasterUR[0],&ptRasterUR[1]);

	ptViewLL[0] = 0;
	ptViewLL[1] = pView->m_iWinY;
	ptViewUR[0] = pView->m_iWinX;
	ptViewUR[1] = 0;
	elist = pEntity->entgetx(NULL, pDrawing);

	// Bug fix #6405
	// Get the current raster quality value from the 70 group code of the AcDbRasterVariables
	// class (acad class)
	if(pView->IsPrinting())
		rasterQuality = 1;
	else
		rasterQuality = Ras_GetRasterQuality();

	//Set the MapToRGBColor on, so that Hitachi always gets an RGB COLORREF.  Bug #8239.
	save_bMapToRGBColor = SDS_CMainWindow->m_bMapToRGBColor;
	SDS_CMainWindow->m_bMapToRGBColor = TRUE;

	err = pRaster->drawRaster(*pDrawDevice->getCDC(),
		ptRasterLL,
		ptRasterUL,
		ptRasterUR,
		ptRasterLR,
		ptViewLL,
		ptViewUR,
		SDS_PenColorFromACADColor(256),
		SDS_PenColorFromACADColor(pSpecialDO->color),
		drawmode,
		rasterQuality == 1 ? IRas::High : IRas::Draft,
		elist);

	sds_relrb(elist);
	elist=NULL;
	SDS_CMainWindow->m_bMapToRGBColor = save_bMapToRGBColor;

	if(IRas::eSuccess != err && !pView->IsPrinting())
	{
		delete pRaster;
		pImageDef->set_imagedisp(NULL);
		return 0;
	}

	if(Ras_GetDisplayImageFramesFlag() == 0)
	{
		// skip all IMAGE frame display objects
		while(pSpecialDO->next != NULL && pSpecialDO->next->GetSourceEntity()->ret_type() == DB_IMAGE)
			pSpecialDO = pSpecialDO->next;
	}
	return 1;
}

void CIcadEntityRenderer::drawImageObject(db_handitem *elp,
										  struct gr_view *gView,
										  db_drawing *flp,
										  int hl,
										  int xor,
										  int undraw)
{
	CIcadView *pView = getIcadView();
	int drawmode=CIcadEntityRenderer::calcDrawmode( hl, xor, undraw );

	struct gr_displayobject *beg=NULL,*end=NULL;

	int  layerState = -1, layerColor;
	db_layertabrec *pLayer = (db_layertabrec *)elp->ret_layerhip();
	pLayer->get_70(&layerState);
	pLayer->get_62(&layerColor);

	if (layerState & 0x1 || layerColor < 0) 	// Frozen  || Color < 0 == off.
		return;

	int iRasterMonoColor = 7;
	if( pView->m_fUseRegenList &&
		elp->ret_disp() )
	{
		beg=(struct gr_displayobject *)elp->ret_disp();
		iRasterMonoColor = beg->color;
	}
	else
	{
		db_handitem *telp = elp;
		SDS_getdispobjs(flp,&telp,&beg,&end,NULL,flp,gView,1);
		if (beg != NULL)
		{
			iRasterMonoColor = beg->color;
		}
		gr_freedisplayobjectll(beg);
	}

	if(pView->GetRealTime() == NULL)
		Ras_PaintRaster(pView,elp,gView,drawmode,iRasterMonoColor);
}

void CIcadEntityRenderer::drawOLE2Frame(db_handitem *elp,
										CDrawDevice *pDrawDevice,
										struct gr_view *gView)
{
	CIcadView *pView = getIcadView();

	int  layerState = -1, layerColor;
	db_layertabrec *pLayer = (db_layertabrec *)elp->ret_layerhip();
	pLayer->get_70(&layerState);
	pLayer->get_62(&layerColor);

	if (layerState & 0x1 || layerColor < 0) 	// Frozen  || Color < 0 == off.
		return;

	if(pView->m_bIsTempView)
		return;

	if(!elp->ret_deleted() &&
		(NULL!=pView->m_pViewsDoc))
	{
		CRect rect(0,0,0,0);
		pView->m_pViewsDoc->m_pViewForRedraw=pView;
		pView->m_pViewsDoc->m_pHandItemForRedraw=elp;
		//pView->m_pViewsDoc->m_pDCForRedraw=pView->GetTargetDeviceCDC();
		pView->m_pViewsDoc->m_pDCForRedraw=pDrawDevice->getCDC();
		pView->m_pViewsDoc->m_pGviewForRedraw=gView;
		
		SendMessage(IcadSharedGlobals::GetMainHWND(),WM_COMMAND,ICAD_WNDACTION_UDOLEOBJ,0L);
	}
}

bool CIcadEntityRenderer::IsPrinting()
{
	return (m_pView->IsPrinting());
}

void CIcadEntityRenderer::setDrawDevice(CDrawDevice* pDD)
{
	m_pDD = pDD;
}

void CIcadEntityRenderer::drawPrintViewport(int cvport)
{
	int vport;
	db_handitem* pRec = NULL;
	for(pRec = m_pDrawing->tblnext(DB_VXTABREC,1); pRec != NULL; pRec = pRec->next)
	{
		((db_vxtabrec*)pRec)->ret_vpehip()->get_69(&vport);
		if(vport == cvport)
			break;
	}

	struct gr_viewvars viewvars;
	db_viewport* pVPToUse;
	db_handitem* pInitialVP = m_pView->m_pVportTabHip;
	if(pRec)
	{
		m_pView->m_pVportTabHip = ((db_vxtabrec*)pRec)->ret_vpehip();
		SDS_ViewToVars2Use(m_pView, m_pDrawing, &viewvars);
		pVPToUse = (db_viewport*)m_pView->m_pVportTabHip;
	}
	m_pView->m_pVportTabHip = pInitialVP;

	gr_view* gViewMSVP = NULL;
	gr_initview(m_pDrawing, &SDS_CURCFG, &gViewMSVP, &viewvars);
	DrawDrawing(1, pVPToUse, gViewMSVP);
	gr_freeviewll(gViewMSVP);
}

void CIcadEntityRenderer::drawViewPort(db_viewport *pViewport,
									   struct gr_view* gView,
									   POINT BltArea[2],
									   int *FstPt,
									   int xor,
									   int hl,
									   int undraw)
{
/*DG - 5.2.2003*/// Now the function draws viewport's internals at first (if necessary) and then - viewport's boundary (if necessary).

	CIcadView *pView = getIcadView();
	struct gr_view* gViewVP = NULL;
	int pspaceflag;

	int width = 1;

	// Don't draw in TILEMODE
	if (m_tilemode)
		return;

	// Don't draw the PS Viewport.
	pViewport->get_69(&pspaceflag);
	if(pspaceflag==1)
		return;

	if(undraw && !SDS_DontDrawVPGuts)
		SDS_ClearViewPort(pViewport,pView);

	if(!SDS_DontDrawVPGuts && !undraw && !pViewport->ret_deleted())
	{
		// If we are drawing the VPE that is the active viewport then
		// we don't need to trick the view or set the clipping rect.
		SDS_SetClipInDC(pView,pViewport,FALSE);

#ifdef DRAW_PSPACE_VPORTS
		/*DG - 1.2.2003*/// If SDS_CMainWindow->m_bUsingTempView is 'true', then we came from SDS_DrawBitmap,
		// so pView->m_pCurDwgView's attributes don't meet the current viewport entity's ones.
		if(pViewport != (db_viewport*)pView->m_pVportTabHip || SDS_CMainWindow->m_bUsingTempView)
#else
		if(pViewport != (db_viewport*)pView->m_pVportTabHip)
#endif
		{
			// Don't mess with the view settings for the paperspace viewport.
			SDS_VPeedToView(m_pDrawing, pViewport, &gViewVP, pView);
			// transfer is printing flag for DEFPOINTS processing
			gViewVP->isprinting = gView->isprinting;
		}
		else
		{
			gViewVP = pView->m_pCurDwgView;
		}

		int savereg = pView->m_fUseRegenList;
		// TODO - we're fooling the regenlist to always do a regen.  we need to check
		// if view has changed for this view/viewport
		pView->m_fUseRegenList=0;

		// We have to set the the current variables so the DrawGrid function will have
		// the correct vars.  We have to set the Vars back after PaintTread Call.
		SDS_TabToVar(m_pDrawing, pViewport);

		sds_point ptMin,ptMax;
		getViewPortClippingRectangle(pView, pViewport, gView, gViewVP, ptMin, ptMax);
   		gViewVP->SetVPortClipRectangle(ptMin, ptMax);

#ifdef BMP_USE_HIDESHADE_STATUS
		extern char*	SDS_PreviewFilename;
		short	renderStatus = 0;
		pViewport->get_281(&renderStatus);
		if( SDS_CMainWindow->m_bUsingTempView &&
			(renderStatus == IC_RENDERMODE_HIDDENLINE || renderStatus == IC_RENDERMODE_FLAT_WITH_WIREFRAME) &&
			!SDS_PreviewFilename )
		{
			db_handitem*	pSavedVportTabHip = pView->m_pVportTabHip;
			pView->m_pVportTabHip = pViewport;
			CBitmapHLRenderer hlRenderer(pView, m_pDD->getCDC());
			hlRenderer.render();
			pView->m_pVportTabHip = pSavedVportTabHip;
		}
		else
#endif
			CIcadEntityRenderer::DrawDrawing(1, pViewport, gViewVP);

		gViewVP->ClearVPortClipRectangle( );
		SDS_TabToVar(m_pDrawing, pView->m_pVportTabHip);

		pView->m_fUseRegenList=savereg;

		// Bug fix 4398 Scary
		SDS_SetClipInDC(pView,pView->m_pVportTabHip,FALSE);

#ifdef DRAW_PSPACE_VPORTS
		if(pViewport != (db_viewport*)pView->m_pVportTabHip || SDS_CMainWindow->m_bUsingTempView)
#else
		if(pViewport != (db_viewport*)pView->m_pVportTabHip)
#endif
		{
			// Reset the view here.
			// Free the temp gr_view.
			gr_freeviewll(gViewVP);
		}
		gViewVP = NULL;
	} // if(!SDS_DontDrawVPGuts && !undraw && !pViewport->ret_deleted())

	// Need to make sure we ALWAYS have the view vars set to the PS viewport.
	if(!SDS_DrawVPGutsOnly)
	{
		bool bDrawFrame = true;
		if(pView->IsPrinting())
		{
			/*DG - 11.6.2002*/// dxf290 (plottability flag) of layers is supported since dxf 2000.
			db_layertabrec* layp;
			if(layp = (db_layertabrec*)pViewport->ret_layerhip())
			{
				if(layp->ret_plottable() == 0)
					bDrawFrame = false;
			}
		}
		if(bDrawFrame)
		{
			struct gr_viewvars viewvars;
			db_handitem* savhip = pView->m_pVportTabHip;
			pView->m_pVportTabHip = m_pDrawing->tblnext(DB_VXTAB,1);
			pView->m_pVportTabHip = ((db_vxtabrec*)pView->m_pVportTabHip)->ret_vpehip();
			SDS_ViewToVars2Use(pView, m_pDrawing, &viewvars);
			pView->m_pVportTabHip = savhip;
			gr_initview(m_pDrawing, &SDS_CURCFG, &gViewVP, &viewvars);

			// *************************
			// TODO TODO TODO
			// Should get the paperspace viewport and use it to determine
			// VPLAYER data for this viewport
			SDS_SetClipInDC(pView, pViewport, TRUE);
			CIcadEntityRenderer::DrawEntity(pViewport, gViewVP, BltArea, FstPt, xor, hl, undraw, 256, 1, NULL, width);
			gr_freeviewll(gViewVP);
		}
	} // if(!SDS_DrawVPGutsOnly)
}

// ****************************
// PRIVATE METHOD -- class CIcadEntityRenderer
//
// handles VPLAYER
//
bool CIcadEntityRenderer::entityIsOnVpLayerFrozenLayer(db_handitem *pEntity,
													   db_viewport *pViewport,
													   db_drawing *pDrawing)
{
	if ( pViewport != NULL )
	{
		ASSERT( pViewport->ret_type() == DB_VIEWPORT );
		if ( pViewport->ret_type() == DB_VIEWPORT )
		{
			db_layertabrec *pLayer = (db_layertabrec *)pEntity->ret_layerhip();

			// Well, not everything has a layer.  In particular IMAGEDEFs...
			// ASSERT( pLayer != NULL );
			if ( pLayer != NULL )
			{
				if ( pViewport->IsLayerVpLayerFrozen( pLayer ) )
					return true;
			}
		}
	}
	return false;
}

void CIcadEntityRenderer::drawEntity(db_handitem* pEntity,
									 int mode,
									 bool bRedraw,
									 int width)
{
	// These two lines were added to prevent drawing of entities inside block definition in a MODEL SPACE
	// TO DO: add more precise checking and corresponded action for parent block
	if(pEntity->ret_type() != DB_SEQEND && pEntity->m_parentBlock != NULL)
		// we don't want to draw entities which are in the block definition
		return;

	if(pEntity->ret_type() == DB_SEQEND)
		((db_seqend*)pEntity)->get_mainent(&pEntity);

	int hl, xor, undraw;
	switch(abs(mode))
	{
	case 1:
		hl=0; xor=0; undraw=0; break;
	case 2:
		hl=0; xor=0; undraw=1; break;
	case 3:
		hl=1; xor=0; undraw=0; break;
	case 4:
		hl=0; xor=0; undraw=0; break;
	case 5:
		hl=0; xor=1; undraw=0; break;
	default:
		ASSERT(FALSE);
		return;
	}

	int FstPt = 1;
	POINT BltArea[2];
	CDC* pDC = NULL;
	struct gr_view* pViewData;
	db_handitem* pCurEntity;
	POSITION pos;
	bool bDrawInAllViews = m_pView == NULL ? true : false;
	if(bDrawInAllViews)
		pos = m_pDoc->GetFirstViewPosition();
	while(pos != NULL)
	{
		if(bDrawInAllViews)
			m_pView = (CIcadView*)m_pDoc->GetNextView(pos);
		ASSERT(m_pView != NULL);

		if(!m_pView->IsKindOf(RUNTIME_CLASS(CIcadView)))
			break;

		pViewData = m_pView->m_pCurDwgView;
		ASSERT(pViewData != NULL);

		m_pDD = m_pView->GetDrawDevice();
		pDC = m_pView->GetDC();
		m_pDD->beginDraw(pDC, pViewData);
		if(m_pDD->is3D())
			m_pDD->setDrawBuffer(CDrawDevice::eFRONT);
		pCurEntity = pEntity;
		do
		{
			// TODO -- get rid of BAD check for ModelSpace
			//
			if((m_pView->m_pClipRectDC) && (pCurEntity->ret_type() != DB_VIEWPORT))
			{	// We must be in MS if we have a clip rect.
				db_handitem* vphip;
				int fi1;
				struct gr_view* gViewVP = NULL;
				for(vphip = m_pDrawing->tblnext(DB_VXTABREC,1); vphip != NULL; vphip = vphip->next)
				{
					if (vphip->ret_deleted() || ((db_vxtabrec*)vphip)->ret_vpehip() == NULL)
						continue;

					((db_vxtabrec*)vphip)->ret_vpehip()->get_69(&fi1);
                    if (fi1 == 1)
                        continue;

					db_viewport* pViewport = (db_viewport*)((db_vxtabrec*)vphip)->ret_vpehip();
					ASSERT(pViewport->ret_type() == DB_VIEWPORT);
					// VPLAYER code
					//
					if(entityIsOnVpLayerFrozenLayer(pCurEntity, pViewport, m_pDrawing))
						continue;
					if(gViewVP)
					{
						gr_freeviewll(gViewVP);
						gViewVP=NULL;
					}
					SDS_VPeedToView(m_pDrawing, pViewport, &gViewVP, m_pView);
					SDS_SetClipInDC(m_pView, pViewport, FALSE);

					sds_point ptMin,ptMax;
					getViewPortClippingRectangle(m_pView, pViewport, pViewData, gViewVP, ptMin, ptMax);
					gViewVP->SetVPortClipRectangle(ptMin, ptMax);

					int width = 1;
					DrawEntity(pCurEntity, gViewVP, BltArea, &FstPt, xor, hl, undraw, 256, 0, pViewport, width);

					gViewVP->ClearVPortClipRectangle( );
					if(SDS_CMainWindow->m_bPrintPreview)
						break;
				}
				if(gViewVP)
				{
					gr_freeviewll(gViewVP);
					gViewVP = NULL;
				}
				SDS_SetClipInDC(m_pView, m_pView->m_pVportTabHip, FALSE);
			}
			else
			{
				// !!!!!!!!TODO TODO TODO
				// if tilemode == 0
				// Should get the paperspace viewport and use it to check VpLayerData
				//
				DrawEntity(pCurEntity, m_pView->m_pCurDwgView, BltArea, &FstPt, xor, hl, undraw, 256, 0, NULL, width);
			}

			if(FstPt == 0)
			{
				if(bRedraw && !SDS_DontBitBlt)
				{
					m_pDD->copyToFront(BltArea[0].x, BltArea[0].y, BltArea[1].x - BltArea[0].x + 1, BltArea[1].y - BltArea[0].y + 1);
				}
				else
				{
					m_pView->m_RectBitBlt=1;
					if(m_pView->m_RectBitBltPt[0].x > BltArea[0].x)
						m_pView->m_RectBitBltPt[0].x=BltArea[0].x;
					if(m_pView->m_RectBitBltPt[0].y > BltArea[0].y)
						m_pView->m_RectBitBltPt[0].y=BltArea[0].y;
					if(m_pView->m_RectBitBltPt[1].x < BltArea[1].x)
						m_pView->m_RectBitBltPt[1].x=BltArea[1].x;
					if(m_pView->m_RectBitBltPt[1].y < BltArea[1].y)
						m_pView->m_RectBitBltPt[1].y=BltArea[1].y;
				}
			}

			if(mode < 0)
				break;
			pCurEntity = pCurEntity->next;
		} while(pCurEntity && pCurEntity->ret_type() == DB_ATTRIB);

		m_pDD->endDraw();
		m_pView->ReleaseDC(pDC);
		m_pView->ReleaseDrawDevice(m_pDD);
		m_pDD = NULL;
		if(!bDrawInAllViews)
			break;
	}
}
// *****************************
// PUBLIC METHOD -- class CIcadEntityRenderer
//
// DrawEntity
//
// Main entry point to this class
//
void CIcadEntityRenderer::DrawEntity(db_handitem* pEntity,
									 struct gr_view* gView,
									 POINT BltArea[2],
									 int* FstPt,
									 int xor,
									 int hl,
									 int undraw,
									 int undrawcol,
									 int norecurs,
									 db_viewport* pViewportForVpLayerSettings,
									 int width)
{
	ASSERT(m_pDrawing != NULL);
	ASSERT(m_pView != NULL);
	ASSERT(m_pDD != NULL);
	ASSERT(pEntity != NULL);

	int enttype = pEntity->ret_type();
	if(enttype == DB_SEQEND || enttype == DB_VERTEX)
		return;

	if(m_pView->IsPrinting())
	{
		/*DG - 11.6.2002*/// dxf290 (plottability flag) of layers is supported since dxf 2000.
		db_layertabrec* layp;
		if(layp = (db_layertabrec*)pEntity->ret_layerhip())
		{
			if(layp->ret_plottable() == 0 && (pEntity->ret_type() != DB_VIEWPORT))
				return;
		}
		//width control was taken out.	We will now need to map width for each pen.
		//		if(NULL!=SDS_CMainWindow->m_pIcadPrintDlg)
		//			{
		//			width=SDS_CMainWindow->m_pIcadPrintDlg->m_penMapTab.m_LineWeight;
		//			}
	}

	//if(enttype == DB_IMAGE)
	//{
	//	if(m_pView->m_bIsTempView)
	//		return;
	//	drawImageObject(pEntity, gView, m_pDrawing, hl, xor, undraw );
	//
	//	// if we're not supposed to display the frames, no more work to do
	//	// on this entity
	//	if (Ras_GetDisplayImageFramesFlag() == 0)
	//		return;
	//}
	//else
		if(enttype == DB_OLE2FRAME)
		{
			drawOLE2Frame(pEntity, m_pDD, gView);
			return;
		}
		else
			if(enttype == DB_VIEWPORT)
			{
				int flag;
				pEntity->get_68(&flag);
				if(flag == 1 && !m_pView->IsPrinting())
					width = 3;
				if(!norecurs && m_pView->m_pViewsDoc && flag)
				{
					drawViewPort((db_viewport*)pEntity, gView, BltArea, FstPt, xor, hl, undraw);
					return;
				}
			}

	bool bMustRegen = false;
	if(pViewportForVpLayerSettings != NULL)
	{
		if(pViewportForVpLayerSettings->GetVpLayerFrozenLayerCount( ) > 0)
		{
			if((enttype == DB_INSERT) || (enttype == DB_DIMENSION) || (enttype == DB_ATTRIB))
				bMustRegen = true;
		}
	}

	bool bRedraw;
	if(m_pView->m_fUseRegenList &&
		!bMustRegen &&
		enttype != DB_XLINE &&
		enttype != DB_VIEWPORT &&
		enttype != DB_RAY &&
		!m_pView->IsPrinting() &&
		pEntity->ret_disp())
	{
		bRedraw = true;
	}
	else
	{
		bRedraw = false;
	}

	CEntityRenderer::DrawEntity(pEntity,
		m_pDD,
		m_pDrawing,
		gView,
		BltArea,
		FstPt,
		xor,
		hl,
		undraw,
		undrawcol,
		m_fillmode,
		norecurs,
		pViewportForVpLayerSettings,
		bRedraw,
		width);
}

#define ACTIVE_VIEWPORT_LAST_DRAWN   // uses when cmd_InsideRegen==1 and TILEMODE == 0 and PSLTSCALE == 1
#define _DRAWORDER_
int CIcadEntityRenderer::DrawDrawing(int iDoingVP,
									 db_viewport* pViewport,
									 struct gr_view* gViewVP)
{
	CIcadView*	pView = getIcadView();
	bool		isPrinting = pView->IsPrinting(),
				bReleaseDevices = false;	/*DG - 1.2.2003*/// 'true' if we should release pDC and m_pDD. It's necessary when m_pDD != 0 and iDoingVP == 0 at function entry.
	CDC*		pDC = NULL;
	if(m_pDD == NULL && !iDoingVP)
	{
		bReleaseDevices = true;
		m_pDD = pView->GetDrawDevice();
		ASSERT(m_pDD->IsValid());
		if(!isPrinting)
		{
			pDC = pView->GetDC();
			m_pDD->beginDraw(pDC, pView->m_pCurDwgView);
		}
		// NOTE :: ClearScreen here causes flicker, change to Clear the buffer
		//
		m_pDD->clear();
	}

	db_drawing* pDrawing = m_pDrawing;
	ASSERT(pView != NULL);
	ASSERT(pDrawing != NULL);
	ASSERT(pView->m_pCurDwgView != NULL);

	db_entity* pEntity;
	int BltTime;
	CRect rect(0, 0, pView->m_iWinX, pView->m_iWinY);
	struct resbuf rbtemp;
	struct gr_view* gView;
	struct gr_view* gViewPSVP=NULL;
	sds_point extent[2];
	int type;

	// Set up the view we're going to use
	if (gViewVP != NULL)
		gView = gViewVP;
	else
		gView = pView->m_pCurDwgView;

	if(IsWindow(pView->m_hWnd))
		pView->GetClientRect(&rect);
	sds_point ptLwrLeft, ptUprRight;

	//*** Get the SCREENSIZE.
	gr_pix2rp(gView,0,pView->m_iWinY,&ptLwrLeft[0],&ptLwrLeft[1]);
	gr_pix2rp(gView,pView->m_iWinX,0,&ptUprRight[0],&ptUprRight[1]);

	// This should be a good assumption
	ASSERT(m_tilemode == 1 ? (pViewport == NULL) : TRUE);

	// Make a PS viewport view structure.
	if(!iDoingVP && !m_tilemode && m_cvport != 1 && !cmd_InsideRedrawAll)
	{
		struct gr_viewvars viewvarsp;
		db_handitem *savhip = pView->m_pVportTabHip;
		pView->m_pVportTabHip = pDrawing->tblnext(DB_VXTAB, 1);
		if(pView->m_pVportTabHip)
		{
			pView->m_pVportTabHip = ((db_vxtabrec *)pView->m_pVportTabHip)->ret_vpehip();
			SDS_ViewToVars2Use(pView, pDrawing, &viewvarsp);
		}
		pView->m_pVportTabHip = savhip;
		gr_initview(pDrawing, &SDS_CURCFG, &gViewPSVP, &viewvarsp);
	}

	BltTime=0;
	// need current viewport if printing in MS tile mode 0
	if(!iDoingVP && isPrinting && !m_tilemode && m_cvport != 1)
	{
		drawPrintViewport(m_cvport);
		gr_freeviewll(gViewPSVP);
		pView->ReleaseDrawDevice(m_pDD);
		m_pDD = NULL;
		return RTNORM;
	}

#ifdef ACTIVE_VIEWPORT_LAST_DRAWN
	SDS_getvar(NULL,DB_QPSLTSCALE,&rbtemp,pDrawing,&SDS_CURCFG,&SDS_CURSES);
	int psltscale = rbtemp.resval.rint;
	db_entity* pActiveViewport = NULL;
#endif

	for(pEntity = (db_entity*)pDrawing->entnext_noxref(NULL); pEntity != NULL; pEntity = (db_entity*)pDrawing->entnext_noxref(pEntity), ++BltTime)
	{
		// EBATECH(CNBR) 2002/11/30  -[ Control draworder  #SORTENTSISDICVAR#
		if( pEntity->ret_ent_deleted())
			continue;
#ifdef _DRAWORDER_
		db_entity* pOriginalEntity = NULL;
		if(m_pSortingTable != NULL)
		{
			db_objhandle entityHandle;
			if(m_pSortingTable->find(pEntity->RetHandle(), entityHandle))
			{
				pOriginalEntity = pEntity;
				pEntity = (db_entity*)m_pDrawing->handent(entityHandle);
				if(pEntity == NULL || pEntity->ret_ent_deleted())
				{
					ASSERT(pEntity != NULL);
					pEntity = pOriginalEntity;
					pOriginalEntity = NULL;
				}
			}
		}
#endif

		// VPLAYER code
		//
		if(entityIsOnVpLayerFrozenLayer(pEntity, pViewport, pDrawing))
			continue;

		if(pEntity->ret_ent_pspace())
		{
			//	Don't draw paperspace entities in tilemode
			if(iDoingVP || m_tilemode)
				continue;
		}
		else
		{
			// Don't draw mspace entities in paperspace
			if(!iDoingVP && !m_tilemode)
				continue;
		}

		type = pEntity->ret_type();

#ifdef ACTIVE_VIEWPORT_LAST_DRAWN
		if(cmd_InsideRegen && type == DB_VIEWPORT && m_tilemode == 0 && psltscale)
		{
			if(cmd_InsideRedrawAll)
			{
				int id;
				pEntity->get_69(&id); 
				if(SDS_CURDOC && SDS_CURDOC->m_nLastMsVport == id)
				{
					pActiveViewport = pEntity;
					continue;
				}
			}
			else
			{
				int flag;
				pEntity->get_68(&flag); 
				if(flag == 1)
				{
					pActiveViewport = pEntity;
					continue;
				}
			}
		}
#endif

		if(pView->m_fUseRegenList && (type!=DB_IMAGE) && (type!=DB_RAY) && (type!=DB_XLINE) &&
			(pEntity->ret_disp() || (type==DB_OLE2FRAME)))
		{
#ifdef _USE_DISP_OBJ_PTS_3D_
//#pragma message("_USE_DISP_OBJ_PTS_3D_ defined in IcadEntityRenderer.cpp")
			CD3 minPoint, maxPoint;
			CDoBase::getPlanarBoundingBox((pEntity->ret_ent_pspace() && gViewPSVP) ? gViewPSVP : gView, pEntity, minPoint, maxPoint);
			memcpy(extent[0], (sds_real*)minPoint, 3 * sizeof(sds_real));
			memcpy(extent[1], (sds_real*)maxPoint, 3 * sizeof(sds_real));
#else
			pEntity->get_extent(extent[0], extent[1]);
#endif
			// This expression seems a bit excessive
			if(!iDoingVP && IsPrinting() &&
				(ptLwrLeft[1]>extent[1][1] || ptUprRight[0]<extent[0][0] ||
				ptUprRight[1]<extent[0][1] || ptLwrLeft[0]>extent[1][0]))
				continue;
		}
		// Here's the final drawing of the entity
		DrawEntity(pEntity, (pEntity->ret_ent_pspace() && gViewPSVP) ? gViewPSVP : gView,
			NULL, NULL, 0, 0, 0, 0, 0, pViewport, 1);
#ifdef _DRAWORDER_
		if(pOriginalEntity != NULL)
			pEntity = pOriginalEntity;
#endif
		if(BltTime >= 120000 && !isPrinting)
		{
			BltTime = 0;
#ifndef _USE_OPENGL_
			m_pDD->copyToFront(0, 0, rect.Width(), rect.Height());
#endif
		}
	}

#ifdef ACTIVE_VIEWPORT_LAST_DRAWN
	if(pActiveViewport)
	{
		// Here's the final drawing of the entity
		DrawEntity(pActiveViewport, (pActiveViewport->ret_ent_pspace() && gViewPSVP) ? gViewPSVP : gView,
			NULL, NULL, 0, 0, 0, 0, 0, pViewport, 1);
	}
#endif

	bool bDrawnUCS = false;
	if(!isPrinting && !pView->m_bMakingSld && !SDS_CMainWindow->m_bUsingTempView)
	{

		ASSERT(m_pUCS != NULL);
		if(!pViewport)
		{
#ifdef _SDS_UCSICON_
			if(SDS_DrawUcsIcon(NULL, NULL, pView, 0) == SDS_RTNORM)
				bDrawnUCS = true;
#else
			m_pUCS->draw(NULL, NULL, 0, m_pDD, pView);
#endif
		}
		else
		{
			int fi1;
			pViewport->get_68(&fi1);
			if(fi1 == 1)
#ifdef _SDS_UCSICON_
				if(SDS_DrawUcsIcon(NULL,NULL,pView,0) == SDS_RTNORM)
					bDrawnUCS = true;
#else
				m_pUCS->draw(NULL, NULL, 0, m_pDD, pView);
#endif
		}
		ASSERT(m_pGrid != NULL);
#ifdef _SDS_GRID_
		SDS_DrawGrid(pView, gView);
#else
		m_pGrid->draw(gView, m_pDD, pView);
#endif
	}

	if(!isPrinting && !pView->m_bMakingSld)
	{
		if(!bDrawnUCS )
			m_pDD->copyToFront(0, 0, rect.Width(), rect.Height());
	}

	if(gViewPSVP)
	{
		gr_freeviewll(gViewPSVP);
		gViewPSVP=NULL;
	}

	ASSERT(m_pDD->IsValid());
	if(bReleaseDevices && !iDoingVP)
	{
		if(!isPrinting)
		{
			m_pDD->endDraw();
			pView->ReleaseDC(pDC);
		}
		pView->ReleaseDrawDevice(m_pDD);
		m_pDD = NULL;
	}
	return(RTNORM);
}

void CIcadEntityRenderer::SetDcFillColorMem(int color,
											int hl,
											int rop,
											CDrawDevice* pDevice)
{
	ASSERT(pDevice != NULL);

	if(m_pView->m_FillLastCol == color &&
		m_pView->m_FillLastHl == hl &&
		m_pView->m_FillLastRop == rop && pDevice == NULL)
		return;
	m_pView->m_FillLastCol = color;
	m_pView->m_FillLastHl = hl;
	m_pView->m_FillLastRop = rop;

	if(hl)
	{
		if(SDS_PaletteColors[255].rgbRed == 255 && SDS_PaletteColors[255].rgbGreen == 255 && SDS_PaletteColors[255].rgbBlue == 255)
			pDevice->SetFillColorXOR(255);
		else
			pDevice->SetFillColorXOR(-1);
	}
	else
		if(R2_XORPEN == rop)
			pDevice->SetFillColorXOR(color);
		else
			pDevice->SetFillColorACAD(color);
}

void CIcadEntityRenderer::SetDcColorMem(int color,
										int width,
										int hl,
										int rop,
										CDrawDevice* pDevice)
{
	ASSERT(pDevice != NULL);
	if(m_pView->m_LastCol == color &&
		m_pView->m_LastHl == hl &&
		m_pView->m_LastRop == rop &&
		m_pView->m_LastWid == width && pDevice == NULL)
		return;
	m_pView->m_LastCol = color;
	m_pView->m_LastHl = hl;
	m_pView->m_LastRop = rop;
	m_pView->m_LastWid = width;

	if(hl)
	{
		if(SDS_PaletteColors[255].rgbRed == 255 && SDS_PaletteColors[255].rgbGreen == 255 && SDS_PaletteColors[255].rgbBlue == 255)
			pDevice->SetLineColorACAD(-1, width, RasterConstants::LS_DOT);
		else
			pDevice->SetLineColorACAD(256, width, RasterConstants::LS_DOT);
	}
	else
		if(R2_XORPEN == rop)
			pDevice->SetLineColorXOR(color, width);
		else
			pDevice->SetLineColorACAD(color, width);
}

void CIcadEntityRenderer::drawDisplayObjects(struct gr_displayobject *beg,
											 int width,
											 CDrawDevice *pDrawDevice,
											 db_drawing *flp,
											 struct gr_view *gView,
											 POINT BltArea[2],
											 int *FstPt,
											 int xor,
											 int hl,
											 int undraw,
											 int undrawcol,
											 int fillmode)
{
	CIcadView * pView = getIcadView();
	struct gr_displayobject *tdo1;

	for (tdo1=beg; tdo1!=NULL; tdo1=tdo1->next)
	{
		int face,ndims;
		if (tdo1->npts<2 ||
			tdo1->chain==NULL)
		{
			continue;
		}
		ndims=((tdo1->type&0x01)!=0)+2;
		face=((tdo1->type&0x02)!=0);

		int rop;
		if((tdo1->color==-1 || xor) &&
			!undraw)
		{
			rop = R2_XORPEN;
			pDrawDevice->SetDrawMode( RasterConstants::MODE_XOR );
		}
		else
		{
			rop = R2_COPYPEN;
			pDrawDevice->SetDrawMode( RasterConstants::MODE_COPY );
		}

		// Now draw it!
		if((tdo1->type&0x04)!=0 &&
			fillmode)

		{ // Solid Fill
			CEntityRenderer::outputPolygon( ndims, face, tdo1,
				pDrawDevice,
				gView,
				BltArea, FstPt,
				undraw, undrawcol, rop,
				hl, width, (tdo1->type&0x10) == 0);

		} // end of if((tdo1->type&0x04)!=0 && fillmode)
		else
		{
			CEntityRenderer::outputPolyline( ndims, face, tdo1,
				pDrawDevice,
				gView,
				BltArea, FstPt,
				undraw, undrawcol, rop,
				hl, width );
		}		// end of else case -- not filling
	}			// end of loop over items to draw
}

/******************************************************************************
*+ DrawGDITrueTypeTextBox
*
*  Draws a box around the specified TrueType text.
*
*  Useful for making sure we are getting the correct text box for selecting
*  TrueType text.
*
*  WARNING!!!!!!!	This function is designed to work correctly only with
*					TrueType text that is drawn by GDI with TextOut().
*
*					This function does not check to make sure that the
*					specified text is actually TrueType text drawn by GDI.
*					If the specified text isn't TrueType text drawn by GDI,
*					the results will probably be incorrect.  The function
*					expects that the text will have only one display object
*					and that the display object will consist of the four
*					points that define the text box.
*
*/

void DrawGDITrueTypeTextBox(db_handitem *text,
							CDC *dc,
							gr_view *view)
{
	sds_point pt1, pt2, pt3, pt4, pt5;

	// get the points defining the text box
	if (!GetGDITrueTypeTextBox(text, pt1, pt2, pt3, pt4, pt5))
		return;

	// convert the points to the real projection plane
	// and then to pixels
	sds_point tmpPt1, tmpPt2, tmpPt3, tmpPt4, tmpPt5;
	gr_ucs2rp(pt1, tmpPt1, view);
	gr_ucs2rp(pt2, tmpPt2, view);
	gr_ucs2rp(pt3, tmpPt3, view);
	gr_ucs2rp(pt4, tmpPt4, view);
	gr_ucs2rp(pt5, tmpPt5, view);

	int pt1X, pt1Y, pt2X, pt2Y, pt3X, pt3Y, pt4X, pt4Y, pt5X, pt5Y;
	gr_rp2pix(view, tmpPt1[0], tmpPt1[1], &pt1X, &pt1Y);
	gr_rp2pix(view, tmpPt2[0], tmpPt2[1], &pt2X, &pt2Y);
	gr_rp2pix(view, tmpPt3[0], tmpPt3[1], &pt3X, &pt3Y);
	gr_rp2pix(view, tmpPt4[0], tmpPt4[1], &pt4X, &pt4Y);
	gr_rp2pix(view, tmpPt5[0], tmpPt5[1], &pt5X, &pt5Y);

	// draw the box
	dc->MoveTo(pt1X, pt1Y);
	dc->LineTo(pt2X, pt2Y);
	dc->LineTo(pt3X, pt3Y);
	dc->LineTo(pt4X, pt4Y);
	dc->LineTo(pt5X, pt5Y);
}

/******************************************************************************
*+ GetGDITrueTypeTextBox
*
*  Gets the text box for a piece of TrueType text that was drawn by GDI with
*  TextOut().
*
*  The output points define the box in a counterclockwise direction.  Pt1
*  would be the lower left point if the box were aligned with the X and Y
*  axes in the World Coordinate System.
*
*  WARNING!!!!!!!	This function is designed to work correctly only with
*					TrueType text that is drawn by GDI with TextOut().
*
*					This function does not check to make sure that the
*					specified text is actually TrueType text drawn by GDI.
*					If the specified text isn't TrueType text drawn by GDI,
*					the results will probably be incorrect.  The function
*					expects that the text will have only one display object
*					and that the display object will consist of the four
*					points that define the text box.
*
*/

bool GetGDITrueTypeTextBox(db_handitem *text,
						   sds_point pt1,
						   sds_point pt2,
						   sds_point pt3,
						   sds_point pt4,
						   sds_point pt5)
{
	ASSERT(text && pt1 && pt2 && pt3 && pt4 && pt5);

	if ((NULL == text) ||
		(NULL == pt1) ||
		(NULL == pt2) ||
		(NULL == pt3) ||
		(NULL == pt4) ||
		(NULL == pt5))
		return false;

	// make sure the entity type is correct
	int entType = text->ret_type();

	ASSERT((DB_TEXT == entType) ||
		(DB_ATTDEF == entType) ||
		(DB_ATTRIB == entType) ||
		(DB_MTEXT == entType));

	gr_displayobject *dispObjs;
	if (DB_TEXT == entType)
		dispObjs = (gr_displayobject *) ((db_text *) text)->ret_disp();
	else if (DB_ATTDEF == entType)
		dispObjs = (gr_displayobject *) ((db_attdef *) text)->ret_disp();
	else if (DB_ATTRIB == entType)
		dispObjs = (gr_displayobject *) ((db_attrib *) text)->ret_disp();
	else if (DB_MTEXT == entType)
		dispObjs = (gr_displayobject *) ((CDbMText *) text)->ret_disp();
	else
		return false;

	if (NULL == dispObjs)			// there won't be dispObjs if the entity is deleted or otherwise not visible
		return false;

	pt1[0] = dispObjs->chain[0];
	pt1[1] = dispObjs->chain[1];
	pt1[2] = 0.0;

	pt2[0] = dispObjs->chain[2];
	pt2[1] = dispObjs->chain[3];
	pt2[2] = 0.0;

	pt3[0] = dispObjs->chain[4];
	pt3[1] = dispObjs->chain[5];
	pt3[2] = 0.0;

	pt4[0] = dispObjs->chain[6];
	pt4[1] = dispObjs->chain[7];
	pt4[2] = 0.0;

	pt5[0] = dispObjs->chain[8];
	pt5[1] = dispObjs->chain[9];
	pt5[2] = 0.0;

	return true;
}



