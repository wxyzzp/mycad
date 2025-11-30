/* D:\ICADDEV\PRJ\ICAD\PRINTSCALEVIEWTAB.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *	
 * Abstract
 *	
 * Scale/View tab on print dialog
 *	
 */ 

// PrintScaleViewTab.cpp : implementation file
//

#include "icad.h"/*DNT*/
#include "PrintScaleViewTab.h"/*DNT*/
#include "DbLayout.h"
#include "cmdsLayout.h"
#include "IcadApp.h"
#include "PrintDialog.h"/*DNT*/
#include "PlotProperties.h"/*DNT*/
#include "IcadView.h"
#include "Preferences.h"

#define PRN_OPT_INCHES		0
#define PRN_OPT_MM			1


/////////////////////////////////////////////////////////////////////////////
// PrintScaleViewTab dialog


PrintScaleViewTab::PrintScaleViewTab(CWnd* pParent /*=NULL*/)
	: CPropertyPage(PrintScaleViewTab::IDD), m_plotProperties(NULL), m_bWasPicked(false)
{
	sds_ssadd(NULL,NULL,m_ssPrintEnt);
	m_ViewList = new CPtrList();

	//{{AFX_DATA_INIT(PrintScaleViewTab)
	m_View = PRN_OPT_CURRENTVIEW;
	m_SelMode = -1;
	m_Scale = PRN_OPT_INCHES;
	m_ClipMode = FALSE;
	m_FitMode = TRUE;
//	m_winFromX = _T("");
//	m_winFromY = _T("");
//	m_winToX = _T("");
//	m_winToY = _T("");
//	m_DrawingUnits = _T("");
//	m_PrintedUnits = _T("");
	m_ViewName = _T("");
	//}}AFX_DATA_INIT

	//*** Initialize these strings to 0,0.
	char fs1[IC_ACADBUF];
	if(sds_rtos(0.0,-1,-1,fs1)==RTNORM)
	{
		m_winFromX = fs1; 
		m_winFromY = fs1;
		m_winToX = fs1; 
		m_winToY = fs1;
	}

	//*** Initialize these strings to 1.
	if(sds_rtos(1.0,-1,-1,fs1)==RTNORM)
	{
		m_DrawingUnits = fs1;
		m_PrintedUnits = fs1;
	}

	m_drawUnitScale = 1.0;
	m_printUnitScale = 1.0;
	m_bPreviewWasPressed = false;
	m_bApplyWasPressed = false;
	pLayout = NULL;
}


PrintScaleViewTab::~PrintScaleViewTab()
{
	sds_ssfree(m_ssPrintEnt);
	sds_relrb(pLayout);
	if (m_ViewList)
		delete m_ViewList;

}

void PrintScaleViewTab::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PrintScaleViewTab)
	DDX_Radio(pDX, PRN_VIEW, m_View);
	DDX_Radio(pDX, PRN_SELMODE, m_SelMode);
	DDX_Radio(pDX, PRN_UNITS, m_Scale);
	DDX_Check(pDX, PRN_CLIPWINDOW, m_ClipMode);
	DDX_Check(pDX, PRN_SCL_FIT, m_FitMode);
	DDX_Text(pDX, PRN_FROM_X, m_winFromX);
	DDX_Text(pDX, PRN_FROM_Y, m_winFromY);
	DDX_Text(pDX, PRN_TO_X, m_winToX);
	DDX_Text(pDX, PRN_TO_Y, m_winToY);
	DDX_Text(pDX, PRN_PLOTUNITS, m_DrawingUnits);
	DDX_Text(pDX, PRN_PRINTUNITS, m_PrintedUnits);
	DDX_CBString(pDX, PRN_VIEWLIST, m_ViewName);
	//}}AFX_DATA_MAP
}

void PrintScaleViewTab::setWindow(double x1, double y1, double x2, double y2)
{
	m_winX1 = x1;
	m_winY1 = y1;
	m_winX2 = x2;
	m_winY2 = y2;
	m_bWasPicked = true;
}

//un-hightlights and frees the m_ssPrintEnt
void PrintScaleViewTab::RemovePrintEnts ()
{
	sds_name ename;
	for(long ssct=0L; sds_ssname(m_ssPrintEnt,ssct,ename)==RTNORM; ssct++)
	{
		//*** Un-hilight the selected entities.
		sds_redraw(ename,IC_REDRAW_UNHILITE);
	}
	sds_ssfree(m_ssPrintEnt);
	sds_ssadd(NULL,NULL,m_ssPrintEnt);
}


void PrintScaleViewTab::ResetViewList()
{
	struct sds_resbuf* pRb=NULL;

	if(!m_ViewList->IsEmpty())
	{
		POSITION pos = m_ViewList->GetHeadPosition();
		while(pos!=NULL)
		{
			pRb = (struct sds_resbuf*)m_ViewList->GetNext(pos);
			IC_RELRB(pRb);
		}
		m_ViewList->RemoveAll();
	}
}


sds_resbuf* PrintScaleViewTab::GetCurViewRb()
{
	struct sds_resbuf* pRb = NULL;
	struct sds_resbuf* pRbTmp;
	POSITION pos = m_ViewList->GetHeadPosition();
	for(pRb=NULL; pos!=NULL; pRb=NULL)
	{
		pRbTmp = (struct sds_resbuf*)m_ViewList->GetNext(pos);
		if(ic_assoc(pRbTmp,2)!=0)
		{
			continue;
		}
		if(m_ViewName.Compare(ic_rbassoc->resval.rstring)==0)
		{
			pRb=pRbTmp;
			break;
		}
	}
	return pRb;
}

BOOL PrintScaleViewTab::GetPrintWindow(CIcadView* pView, sds_point ptLwrLeft,
	sds_point ptUprRight)
{
	if(NULL==pView)
	{
		pView = SDS_CMainWindow->GetIcadView();
		CRect rect;
		pView->GetClientRect(rect);
		pView->m_sizePrintView=rect.Size();
	}

	struct sds_resbuf rb;
	sds_real rViewSize,rViewWidth;
	sds_point ptViewCtr;


	db_drawing *flp=NULL;
	CIcadDoc* pDoc = pView->GetDocument();
	if(NULL!=pDoc)
	{
		flp=pDoc->m_dbDrawing;
	}

	int tilemode,cvport;
	SDS_getvar(NULL,DB_QTILEMODE,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
	tilemode=rb.resval.rint;
	SDS_getvar(NULL,DB_QCVPORT,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
	cvport=rb.resval.rint;

	switch(m_View)
	{
		case PRN_OPT_CURRENTVIEW:
			{
				sds_point ptScreenSize;

				//*** Get the SCREENSIZE.
				ptScreenSize[0]=pView->m_sizePrintView.cx;
				ptScreenSize[1]=pView->m_sizePrintView.cy;
				ptScreenSize[2]=0.0;

				if(tilemode==0) {
					SDS_getvar(NULL,DB_QVIEWSIZE,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
					rViewSize=rb.resval.rreal;
					SDS_getvar(NULL,DB_QVIEWCTR,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
					ic_ptcpy(ptViewCtr,rb.resval.rpoint);
				} else {
					//*** Get the current view size.
					pView->m_pVportTabHip->get_40(&rViewSize); // VIEWSIZE
					//*** Get the current view center.
					pView->m_pVportTabHip->get_12(ptViewCtr); // VIEWCTR
				}

				//*** Calculate the From point.
				rViewWidth=(rViewSize*(ptScreenSize[0]/ptScreenSize[1]));
				ptLwrLeft[0]=(ptViewCtr[0]-(rViewWidth/2.0));
				ptUprRight[0]=(ptViewCtr[0]+(rViewWidth/2.0));
				//*** Calculate the To point.
				ptLwrLeft[1]=(ptViewCtr[1]-(rViewSize/2.0));
				ptUprRight[1]=(ptViewCtr[1]+(rViewSize/2.0));

				//*** Make sure the Z gets zero'd before returning.
				ptLwrLeft[2]=0.0; ptUprRight[2]=0.0;
			}
			break;
		case PRN_OPT_SAVEDVIEW:
			{
				struct sds_resbuf* pRb;
				pRb = GetCurViewRb();
				if(NULL==pRb)
					break;

				//*** Get the height and width of the view.
				if(ic_assoc(pRb,40)==0) rViewSize=ic_rbassoc->resval.rreal;
				if(ic_assoc(pRb,41)==0) rViewWidth=ic_rbassoc->resval.rreal;
				//*** Get the center point of the view.
				if(ic_assoc(pRb,10)==0) ic_ptcpy(ptViewCtr,ic_rbassoc->resval.rpoint);
				//*** Calculate the From point.
				ptLwrLeft[0]=(ptViewCtr[0]-(rViewWidth/2.0));
				ptUprRight[0]=(ptViewCtr[0]+(rViewWidth/2.0));
				//*** Calculate the To point.
				ptLwrLeft[1]=(ptViewCtr[1]-(rViewSize/2.0));
				ptUprRight[1]=(ptViewCtr[1]+(rViewSize/2.0));

				//*** Make sure the Z gets zero'd before returning.
				ptLwrLeft[2]=0.0; ptUprRight[2]=0.0;

				pView->SavePrintViewInfo(pRb);
			}
			break;
		case PRN_OPT_EXTENTS:
			{
				int tilemode,cvport;
				sds_point ptTmpLwrLeft,ptTmpUprRight;

				//*** Make sure the Z gets initialized to zeros.
				ptLwrLeft[2]=0.0; ptUprRight[2]=0.0;
				ptTmpLwrLeft[2]=0.0; ptTmpUprRight[2]=0.0;

				SDS_getvar(NULL,DB_QTILEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				tilemode=rb.resval.rint;
				SDS_getvar(NULL,DB_QCVPORT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				cvport=rb.resval.rint;
				SDS_getvar(NULL,((tilemode==0 && cvport==1) ? DB_QP_EXTMIN : DB_QEXTMIN),&rb,flp,&SDS_CURCFG,&SDS_CURSES);
				ic_ptcpy(ptTmpLwrLeft,rb.resval.rpoint);
				SDS_getvar(NULL,((tilemode==0 && cvport==1) ? DB_QP_EXTMAX : DB_QEXTMAX),&rb,flp,&SDS_CURCFG,&SDS_CURSES);
				ic_ptcpy(ptTmpUprRight,rb.resval.rpoint);

				SDS_rpextents(ptTmpLwrLeft,ptTmpUprRight,ptLwrLeft,ptUprRight,NULL);

				//*** Expand the extents area by 5 percent.
				double dOffsetX = ((ptUprRight[0]-ptLwrLeft[0])*1.095);
				dOffsetX-=(ptUprRight[0]-ptLwrLeft[0]);
				double dOffsetY = ((ptUprRight[1]-ptLwrLeft[1])*1.095);
				dOffsetY-=(ptUprRight[1]-ptLwrLeft[1]);
				ptUprRight[0]+=dOffsetX;
				ptUprRight[1]+=dOffsetY;
				ptLwrLeft[0]-=dOffsetX;
				ptLwrLeft[1]-=dOffsetY;
			}
			break;
		case PRN_OPT_LIMITS:
			{
				int tilemode,cvport;

				SDS_getvar(NULL,DB_QTILEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				tilemode=rb.resval.rint;
				SDS_getvar(NULL,DB_QCVPORT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				cvport=rb.resval.rint;

				SDS_getvar(NULL,((tilemode==0 && cvport==1) ? DB_QP_LIMMIN : DB_QLIMMIN),&rb,flp,&SDS_CURCFG,&SDS_CURSES);
				ic_ptcpy(ptLwrLeft,rb.resval.rpoint);
				SDS_getvar(NULL,((tilemode==0 && cvport==1) ? DB_QP_LIMMAX : DB_QLIMMAX),&rb,flp,&SDS_CURCFG,&SDS_CURSES);
				ic_ptcpy(ptUprRight,rb.resval.rpoint);

				//*** Make sure the Z gets zero'd before calling gr_ucs2rp.
				ptLwrLeft[2]=0.0; ptUprRight[2]=0.0;

				gr_ucs2rp(ptLwrLeft,ptLwrLeft,pView->m_pCurDwgView);
				gr_ucs2rp(ptUprRight,ptUprRight,pView->m_pCurDwgView);
			}
			break;
		case PRN_OPT_WINDOW:
			{
				sds_point ptFrom,ptTo;

				//*** Convert the string from the editboxes into points.
				sds_distof(m_winFromX,-1,&ptFrom[0]);
				sds_distof(m_winFromY,-1,&ptFrom[1]);
				sds_distof(m_winToX,-1,&ptTo[0]);
				sds_distof(m_winToY,-1,&ptTo[1]);
				//*** Make sure the Z gets zero'd before calling gr_ucs2rp.
				ptTo[2]=0.0; ptFrom[2]=0.0;
				//*** Setup m_ptLwrLeft as lower left corner and m_ptUprRight as upper right corner.
				gr_ucs2rp(ptFrom,ptFrom,pView->m_pCurDwgView);
				gr_ucs2rp(ptTo,ptTo,pView->m_pCurDwgView);
				ptLwrLeft[0]=min(ptFrom[0],ptTo[0]);
				ptLwrLeft[1]=min(ptFrom[1],ptTo[1]);
				ptUprRight[0]=max(ptFrom[0],ptTo[0]);
				ptUprRight[1]=max(ptFrom[1],ptTo[1]);
				//*** Make sure the Z gets zero'd before we return.
				ptLwrLeft[2]=0.0; ptUprRight[2]=0.0;
			}
			break;
		default:
			return FALSE;
	}
	return TRUE;
}


BOOL PrintScaleViewTab::UpdateFitSize(CDC* pDC)
{
	if(m_FitMode == 0)
	{
		//*** Don't change the values if NOT fitmode.
		return FALSE;
	}
	//*** Fill the print info struct
	ICPRINTINFO pi;
	memset(&pi,0,sizeof(pi));
	CPrintDialog pd(TRUE);
	pd.GetDefaults();
	CDC* pDefaultDC = NULL;
	if(NULL==pDC)
	{
		pDefaultDC = new CDC();
		if(!((CIcadApp*)AfxGetApp())->GetPrinterDC(*pDefaultDC))
		{
			return FALSE;
		}
		pDC = pDefaultDC;
	}
	pi.pDC = pDC;
	if(!GetParent() || !((CPrintDialogSheet*)GetParent())->GetCurrentPrintInfo(&pi))
	{
		if(pDefaultDC)
			delete pDefaultDC;
		return FALSE;
	}
	//*** Convert device units to inches (Width only, for m_strPrintedUnits)
	int nPixPerInchX = pDC->GetDeviceCaps(LOGPIXELSX);
	double dPrinterWidthMmIn = (pi.dAdjustedWidth/((double)nPixPerInchX));
	if(m_Scale!=PRN_OPT_INCHES)
	{
		dPrinterWidthMmIn*=25.4; //*** Convert Inches to Millimeters.
	}

	//*** Calculate the width of the printer in drawing units
	double dPrinterWidthDU = pi.dAdjustedWidth/pi.dPixPerDU;

	dPrinterWidthDU /= dPrinterWidthMmIn;
	dPrinterWidthMmIn = 1.;

	char fs1[IC_ACADBUF];
	if(sds_rtos(dPrinterWidthMmIn,-1,-1,fs1)==RTNORM)
	{
		m_PrintedUnits=fs1;
	}
	if(sds_rtos(dPrinterWidthDU,-1,-1,fs1)==RTNORM)
	{
		m_DrawingUnits=fs1;
	}

	UpdateData(FALSE);
	if(NULL!=pDefaultDC)
	{
		delete pDefaultDC;
	}
	return TRUE;
}


void PrintScaleViewTab::SetPaperSize()
{
	CString strPaperSize;
	CDC dc;
	if(!((CIcadApp*)AfxGetApp())->GetPrinterDC(dc)) return;
	double nPixPerInchX = (double)dc.GetDeviceCaps(LOGPIXELSX);
	double nPixPerInchY = (double)dc.GetDeviceCaps(LOGPIXELSY);
	double dInchesX = (double)dc.GetDeviceCaps(PHYSICALWIDTH);
	dInchesX /= nPixPerInchX;
	double dInchesY = (double)dc.GetDeviceCaps(PHYSICALHEIGHT);
	dInchesY /= nPixPerInchY;
	if(m_Scale==PRN_OPT_INCHES)
	{
		strPaperSize.Format(ResourceString(IDC_ICADPRINTDIA___2F_X___2F_2, "%.2f x %.2f" ),dInchesX,dInchesY);
	}
	else
	{
		double dMMX=dInchesX*25.4;
		double dMMY=dInchesY*25.4;
		strPaperSize.Format(ResourceString(IDC_ICADPRINTDIA___2F_X___2F_2, "%.2f x %.2f" ),dMMX,dMMY);
	}
	::SetWindowText(::GetDlgItem(GetSafeHwnd(),PRN_SCL_PAPER),strPaperSize);
	UpdateFitSize(&dc);
}


void PrintScaleViewTab::LoadPlotSettings()
{
	HRESULT result = -1;

	if (m_plotProperties)
	{
		//units
		CString units;
		BSTR unitsBstr;

		result = m_plotProperties->get_Units(&unitsBstr);

		if (SUCCEEDED(result) && unitsBstr)
		{
			units = unitsBstr;
			
			if (units == "_I"/*DNT*/)
				m_Scale = PRN_OPT_INCHES;
			
			else
				m_Scale = PRN_OPT_MM;

			SysFreeString(unitsBstr);
		}

		// page size just use defaults for system printers.

		//scale
		CString scale;
		BSTR scaleBstr;

		result = m_plotProperties->get_Scale(&scaleBstr);

		if (SUCCEEDED(result) && scaleBstr)
		{
			scale = scaleBstr;
			SysFreeString(scaleBstr);

			if (scale == "_F"/*DNT*/)
				m_FitMode = TRUE;

			else
			{
				double value;
				char str[IC_ACADBUF];

				m_FitMode = FALSE;

				int separatorPos = scale.Find('=');
				
				//convert to proper units and precision
				sds_distof(scale.Left(separatorPos), -1, &value);
				sds_rtos(value, -1, -1, str);
				m_PrintedUnits = str;
				
				sds_distof(scale.Right(scale.GetLength() - separatorPos - 1), -1, &value);
				sds_rtos(value, -1, -1, str);
				m_DrawingUnits = str;
			}
		}

		UpdateData(FALSE);
	}
}


void PrintScaleViewTab::SavePlotSettings()
{
	HRESULT result = -1;

	UpdateData(TRUE);

	if (m_plotProperties)
	{
		CString units;
		// Bugzilla No. 78323; 07/10/2002 [
		BSTR unitsBstr;
		m_plotProperties->get_Units(&unitsBstr);
		CString fileUnits = unitsBstr;
		SysFreeString(unitsBstr);
		// Bugzilla No. 78323; 07/10/2002 ]

		//units
		if (m_Scale == PRN_OPT_INCHES)
			units = "_I"/*DNT*/;
		else
			units = "_M"/*DNT*/;

		BSTR bstr = units.AllocSysString();
		m_plotProperties->put_Units(bstr);
		SysFreeString(bstr);

		// Bugzilla No. 78323; 07/10/2002 [
		if(fileUnits != units)
		{
			double conversionFactor;

			if (units == "_I")
				//mm to inches
				conversionFactor = 1 / 25.4;	
			else
				//inches to mm
				conversionFactor = 25.4;

			// Change the pen width in the file
			float penWidth;
			m_plotProperties->get_PenWidth(&penWidth);
			m_plotProperties->put_PenWidth(penWidth * conversionFactor);
		}
		// Bugzilla No. 78323; 07/10/2002 ]
		
		// page size is just set to max for system printers. We'll handle this at a lower level
		// so that if we do decide to do something here we can pass in floats instead of strings.

		//scale
		CString scale;

		if (m_FitMode)
			scale = "_F"/*DNT*/;

		else
		{
			double value;
			char str[IC_ACADBUF];

			sds_distof(m_PrintedUnits, -1, &value);
			sds_rtos(value, -1, -1, str);

			scale = str;
			scale += "=";

			sds_distof(m_DrawingUnits, -1, &value);
			sds_rtos(value, -1, -1, str);

			scale += str;
		}

		bstr = scale.AllocSysString();
		m_plotProperties->put_Scale(bstr);
		SysFreeString(bstr);
	}

}

void PrintScaleViewTab::LoadRegSettings(bool bFromLayout)
{
	// DP: load plot settings from current LAYOUT object
	sds_name layout;
	CopyList();
	if(bFromLayout && cmd_getCurrentLayout(layout))
	{
		CDbLayout* pCurLayout = (CDbLayout*)layout[0];
		switch(pCurLayout->plotType())
		{
		case CDbLayout::eDisplay: // Last screen display
			m_View = PRN_OPT_CURRENTVIEW;
			break;
		case CDbLayout::eExtents: // Drawing extents
			m_View = PRN_OPT_EXTENTS;
			break;
		case CDbLayout::eLimits: //	Drawing limits
			m_View = PRN_OPT_LIMITS;
			break;
		case CDbLayout::eView: // Last view
			if (((CComboBox*)GetDlgItem(PRN_VIEWLIST))->SelectString(-1, pCurLayout->plotViewName()) >= 0)
				m_View = PRN_OPT_SAVEDVIEW;		
			else //string not found in list set to current view
				m_View = PRN_OPT_CURRENTVIEW;
			break;
		case CDbLayout::eWindow: // Last window
			m_View = PRN_OPT_WINDOW;
			break;
		case CDbLayout::eLayout: // Layout information
			m_View = PRN_OPT_CURRENTVIEW;
			break;
		default:
			ASSERT(FALSE);
		}

		double fromX, fromY, toX, toY;
		pCurLayout->getPlotWindowArea(fromX, fromY, toX, toY);
		char value[IC_ACADBUF];
		sds_rtos(fromX, -1, -1, value);
		m_winFromX = value;
		sds_rtos(fromY, -1, -1, value);
		m_winFromY = value;
		sds_rtos(toX, -1, -1, value);
		m_winToX = value;
		sds_rtos(toY, -1, -1, value);
		m_winToY = value;

		if(pCurLayout->plotPaperUnits() == CDbLayout::eInches)
			m_Scale = PRN_OPT_INCHES;
		else
			m_Scale = PRN_OPT_MM;

		if(pCurLayout->scaleType() == CDbLayout::eToFit)
			m_FitMode = TRUE;
		else
		{
			m_FitMode = FALSE;
			double value;
			char str[IC_ACADBUF];

			pCurLayout->get_142(&value);
			sds_rtos(value, -1, -1, str);
			m_PrintedUnits = str;
				
			pCurLayout->get_143(&value);
			sds_rtos(value, -1, -1, str);
			m_DrawingUnits = str;
		}

		UpdateData(FALSE);
		// Don't call LoadPlotSettings() here becouse m_Scale and m_FitMode variables will be overridden. 
		return;
	}

	HRESULT result;
	if (m_plotMgr)
	{
		PrintArea area;
		result = m_plotMgr->get_Area(&area);
		
		if (SUCCEEDED(result))
		{
			switch (area)
			{
			case CurrentView:
				m_View = PRN_OPT_CURRENTVIEW;
				break;

			case SavedView:
				{
					//get selected view
					BSTR viewStr;
					m_plotMgr->get_View(&viewStr);
					CString view(viewStr);
					SysFreeString(viewStr);

					//check to ensure view is in drawing
					CComboBox* viewListBox = (CComboBox*)GetDlgItem(PRN_VIEWLIST);

					if (viewListBox->SelectString(-1, view) >= 0)
					{
						m_View = PRN_OPT_SAVEDVIEW;		
					}
					else //string not found in list set to current view
					{
						m_View = PRN_OPT_CURRENTVIEW;
					}
				}
				break;

			case Extents:
				m_View = PRN_OPT_EXTENTS;
				break;

			case Limits:
				m_View = PRN_OPT_LIMITS;
				break;

			case Window:
				m_View = PRN_OPT_WINDOW;
				break;
			}

			// load window boxes even if disabled
			double fromX, fromY, toX, toY;
			result = m_plotMgr->GetWindow(&fromX, &fromY, &toX, &toY);

			if (SUCCEEDED(result))
			{
				char str[IC_ACADBUF];

				sds_rtos(fromX, -1, -1, str);
				m_winFromX = str;

				sds_rtos(fromY, -1, -1, str);
				m_winFromY = str;

				sds_rtos(toX, -1, -1, str);
				m_winToX = str;

				sds_rtos(toY, -1, -1, str);
				m_winToY = str;
			}

		}
	}

	//LoadPlotSettings sets data in config file and calls update data for us
	LoadPlotSettings();

	//Note any appropriate fields should be enabled/disabled after this call
}

// Ebatech(CNBR) -[ Layout:Standard Scale (DXF=75) is enum value.
int  PrintScaleViewTab::getScaleType( double printedUnits, double drawingUnits )
{
	CDbLayout::EPlotScaleType code = CDbLayout::eToFit; // Scaled to Fit * Default *

	if( m_Scale == PRN_OPT_MM )
	{
		if( icadRealEqual( drawingUnits, 1.0 ) )
		{
			if( icadRealEqual( printedUnits, 2.0 ) )
				code = CDbLayout::e2_1;   // 2:1
			else if( icadRealEqual( printedUnits,	4.0 ) )
				code = CDbLayout::e4_1;   // 4:1
			else if( icadRealEqual( printedUnits,	8.0 ) )
				code = CDbLayout::e8_1;   // 8:1
			else if( icadRealEqual( printedUnits,  10.0 ) )
				code = CDbLayout::e10_1;  // 10:1
			else if( icadRealEqual( printedUnits, 100.0 ) )
				code = CDbLayout::e100_1; // 100:1
			else if( icadRealEqual( printedUnits, 1000.0 ) )
				code = CDbLayout::e1000_1;// 1000:1,
			else
				code = CDbLayout::eCustom; //User-entry
		}
		else if( icadRealEqual( printedUnits, 1.0 ) )
		{
			if( icadRealEqual( drawingUnits, 100.0 ) )
				code = CDbLayout::e1_100; // 1:100
			else if( icadRealEqual( drawingUnits,  50.0 ) )
				code = CDbLayout::e1_50;  // 1:50
			else if( icadRealEqual( drawingUnits,  40.0 ) )
				code = CDbLayout::e1_40;  // 1:40
			else if( icadRealEqual( drawingUnits,  30.0 ) )
				code = CDbLayout::e1_30;  // 1:30
			else if( icadRealEqual( drawingUnits,  20.0 ) )
				code = CDbLayout::e1_20;  // 1:20
			else if( icadRealEqual( drawingUnits,  16.0 ) )
				code = CDbLayout::e1_16;  // 1:16
			else if( icadRealEqual( drawingUnits,  10.0 ) )
				code = CDbLayout::e1_10;  // 1:10
			else if( icadRealEqual( drawingUnits,	8.0 ) )
				code = CDbLayout::e1_8;   // 1:8
			else if( icadRealEqual( drawingUnits,	4.0 ) )
				code = CDbLayout::e1_4;   // 1:4
			else if( icadRealEqual( drawingUnits,	2.0 ) )
				code = CDbLayout::e1_2;   // 1:2
			if( icadRealEqual( drawingUnits, 1.0 ) )
				code = CDbLayout::e1_1;   // 1:1
			else
				code = CDbLayout::eCustom; //User-entry
		}
		else
			code = CDbLayout::eCustom; //User-entry
	}
	else if( m_Scale == PRN_OPT_INCHES )
	{
		if( icadRealEqual( printedUnits, 1.0 ) )
		{
			if( icadRealEqual( drawingUnits, 1536.0 ) )
				code = CDbLayout::e1_128i; // 1/128"=1'
			else if( icadRealEqual( drawingUnits, 768.0 ) )
				code = CDbLayout::e1_64i;  // 1/64"=1'
			else if( icadRealEqual( drawingUnits, 384.0 ) )
				code = CDbLayout::e1_32i;  // 1/32"=1'
			else if( icadRealEqual( drawingUnits, 192.0 ) )
				code = CDbLayout::e1_16i;  // 1/16"=1'
			else if( icadRealEqual( drawingUnits, 128.0 ) )
				code = CDbLayout::e3_32i;  // 3/32"=1'
			else if( icadRealEqual( drawingUnits,  96.0 ) )
				code = CDbLayout::e1_8i;   // 1/8"=1'
			else if( icadRealEqual( drawingUnits,  64.0 ) )
				code = CDbLayout::e3_16i;  // 3/16"=1'
			else if( icadRealEqual( drawingUnits,  48.0 ) )
				code = CDbLayout::e1_4i;   // 1/4"=1'
			else if( icadRealEqual( drawingUnits,  32.0 ) )
				code = CDbLayout::e3_8i;   // 3/8"=1'
			else if( icadRealEqual( drawingUnits,  24.0 ) )
				code = CDbLayout::e1_2i;  // 1/2"=1'
			else if( icadRealEqual( drawingUnits,  16.0 ) )
				code = CDbLayout::e3_4i;  // 3/4"=1'
			else if( icadRealEqual( drawingUnits,  12.0 ) )
				code = CDbLayout::e1_1i;  // 1"=1'
			else if( icadRealEqual( drawingUnits,  4.0 ) )
				code = CDbLayout::e3_1i;  // 3"=1'
			else if( icadRealEqual( drawingUnits, 2.0 ) )
				code = CDbLayout::e6_1i;  // 6"=1'
			else if( icadRealEqual( drawingUnits, 1.0 ) )
				code = CDbLayout::e1i_1i; // 1'=1'
			else
				code = CDbLayout::eCustom; //User-entry
		}
		else	
				code = CDbLayout::eCustom; //User-entry
	}
	return (int)code;
}
// Ebatech(CNBR) ]-

void PrintScaleViewTab::SaveRegSettings(bool bToLayout)
{
	//SavePlotSettings sets data in config file and calls update data for us
	SavePlotSettings();

	CString view = "";
	if (m_View == PRN_OPT_SAVEDVIEW)
		((CComboBox*)GetDlgItem(PRN_VIEWLIST))->GetWindowText(view);
	
	double fromX, fromY, toX, toY;
	sds_distof(m_winFromX, -1, &fromX);
	sds_distof(m_winFromY, -1, &fromY);
	sds_distof(m_winToX, -1, &toX);
	sds_distof(m_winToY, -1, &toY);

	if (m_plotMgr)
	{
		switch (m_View)
		{
		case PRN_OPT_CURRENTVIEW:
			m_plotMgr->put_Area(CurrentView);
			break;

		case PRN_OPT_SAVEDVIEW:
			m_plotMgr->put_Area(SavedView);
			break;

		case PRN_OPT_EXTENTS:
			m_plotMgr->put_Area(Extents);
			break;

		case PRN_OPT_LIMITS:
			m_plotMgr->put_Area(Limits);
			break;

		case PRN_OPT_WINDOW:
			m_plotMgr->put_Area(Window);
			break;
		}

		BSTR bstr = view.AllocSysString();
		m_plotMgr->put_View(bstr);		
		SysFreeString(bstr);
		m_plotMgr->SetWindow(fromX, fromY, toX, toY);
	}

	// DP: save plot settings into current LAYOUT object
	if(bToLayout)
	{
		CDbLayout::EPlotType type;
		switch (m_View)
		{
		case PRN_OPT_CURRENTVIEW:
			type = CDbLayout::eDisplay;
			break;
		case PRN_OPT_SAVEDVIEW:
			type = CDbLayout::eView;
			break;
		case PRN_OPT_EXTENTS:
			type = CDbLayout::eExtents;
			break;
		case PRN_OPT_LIMITS:
			type = CDbLayout::eLimits;
			break;
		case PRN_OPT_WINDOW:
			type = CDbLayout::eWindow;
			break;
		default:
			ASSERT(FALSE);
		}
		
		double printedUnits, drawingUnits;
		CDbLayout::EPlotScaleType scale;
		if(m_FitMode)
			scale = CDbLayout::eToFit;
		else
		{
			sds_distof(m_PrintedUnits, -1, &printedUnits);
			sds_distof(m_DrawingUnits, -1, &drawingUnits);
			// Ebatech(CNBR) Layout Standard Scale (DXF=75) is not initialized when m_FitMode is 0.
			scale = (CDbLayout::EPlotScaleType)getScaleType( printedUnits, drawingUnits );
		}
		
		sds_name layout;
		if(!cmd_getCurrentLayout(layout))
		{
			ASSERT(FALSE);
			return;
		}
		struct resbuf* pLayout = sds_entget(layout);
		struct resbuf* pCurPair = pLayout;
		while(pCurPair != NULL)
		{
			switch(pCurPair->restype)
			{
			case 6:
				IC_FREE(pCurPair->resval.rstring);
				pCurPair->resval.rstring = new char [ view.GetLength() + 1];
				strcpy(pCurPair->resval.rstring, (LPCTSTR)view); 
				break;
			case 72:
				if(m_Scale == PRN_OPT_INCHES)
					pCurPair->resval.rint = (int)CDbLayout::eInches;
				else
					pCurPair->resval.rint = (int)CDbLayout::eMillimeters;
				break;
			case 74:
				pCurPair->resval.rint = (int)type;
				break;
			case 75:
				pCurPair->resval.rint = (int)scale;
				break;
			case 48:
				pCurPair->resval.rreal = fromX;
				break;
			case 49:
				pCurPair->resval.rreal = fromY;
				break;
			case 140:
				pCurPair->resval.rreal = toX;
				break;
			case 141:
				pCurPair->resval.rreal = toY;
				break;
			case 142:
				pCurPair->resval.rreal = printedUnits;
				break;
			case 143:
				pCurPair->resval.rreal = drawingUnits;
				break;
			}
			pCurPair = pCurPair->rbnext;
		}
		sds_entmod(pLayout);
		sds_relrb(pLayout);
	}
}

BEGIN_MESSAGE_MAP(PrintScaleViewTab, CPropertyPage)
	//{{AFX_MSG_MAP(PrintScaleViewTab)
	ON_BN_CLICKED(PRN_VIEW, OnViewRadio)
	ON_CBN_SELCHANGE(PRN_VIEWLIST, OnSelchangeViewlist)
	ON_BN_CLICKED(PRN_UNITS, OnUnitsChanged)
	ON_BN_CLICKED(PRN_SCL_FIT, OnFitToPage)
	ON_BN_CLICKED(PRN_SELECT_AREA, OnSelectArea)
	ON_BN_CLICKED(PRN_VIEW2, OnViewRadio)
	ON_BN_CLICKED(PRN_VIEW3, OnViewRadio)
	ON_BN_CLICKED(PRN_VIEW4, OnViewRadio)
	ON_BN_CLICKED(PRN_VIEW5, OnViewRadio)
	ON_BN_CLICKED(PRN_UNITS2, OnUnitsChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PrintScaleViewTab message handlers

void PrintScaleViewTab::OnViewRadio() 
{
	UpdateData(TRUE);

	if(m_View==PRN_OPT_EXTENTS) ::EnableWindow(::GetDlgItem(m_hWnd,PRN_CLIPWINDOW),FALSE);
	else ::EnableWindow(::GetDlgItem(m_hWnd,PRN_CLIPWINDOW),TRUE);

	if(m_View==PRN_OPT_SAVEDVIEW) ::EnableWindow(::GetDlgItem(m_hWnd,PRN_VIEWLIST),TRUE);
	else ::EnableWindow(::GetDlgItem(m_hWnd,PRN_VIEWLIST),FALSE);

	if(m_View==PRN_OPT_WINDOW)
	{
		::EnableWindow(::GetDlgItem(m_hWnd,PRN_FROM_X),TRUE);
		::EnableWindow(::GetDlgItem(m_hWnd,PRN_FROM_Y),TRUE);
		::EnableWindow(::GetDlgItem(m_hWnd,PRN_TO_X),TRUE);
		::EnableWindow(::GetDlgItem(m_hWnd,PRN_TO_Y),TRUE);
		::EnableWindow(::GetDlgItem(m_hWnd,PRN_STATIC1),TRUE);
		::EnableWindow(::GetDlgItem(m_hWnd,PRN_STATIC2),TRUE);
		::EnableWindow(::GetDlgItem(m_hWnd,PRN_STATIC3),TRUE);
		::EnableWindow(::GetDlgItem(m_hWnd,PRN_STATIC4),TRUE);
		/*if(!m_bPreview)*/ ::EnableWindow(::GetDlgItem(m_hWnd,PRN_SELECT_AREA),TRUE);
	}
	else
	{
		::EnableWindow(::GetDlgItem(m_hWnd,PRN_FROM_X),FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd,PRN_FROM_Y),FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd,PRN_TO_X),FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd,PRN_TO_Y),FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd,PRN_STATIC1),FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd,PRN_STATIC2),FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd,PRN_STATIC3),FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd,PRN_STATIC4),FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd,PRN_SELECT_AREA),FALSE);
	}
	UpdateFitSize();
}

void PrintScaleViewTab::OnSelchangeViewlist() 
{
	UpdateData(TRUE);
	UpdateFitSize();
}

void PrintScaleViewTab::OnUnitsChanged() 
{
	int oldScale = m_Scale;
	UpdateData(TRUE);

	if (oldScale == m_Scale) 
		return;	
	
	double printedUnits = 1.;
	sds_distof(m_PrintedUnits, -1, &printedUnits);

	CString strPrintUnit;

	if(m_Scale==PRN_OPT_INCHES)
	{
		strPrintUnit=_T(ResourceString(IDC_ICADPRINTDIA_PRINTED_IN_20, "Printed Inches" ));
		printedUnits /= 25.4;
	}
	else
	{
		strPrintUnit=_T(ResourceString(IDC_ICADPRINTDIA_PRINTED_MI_21, "Printed Millimeters" ));
		printedUnits *= 25.4;
	}
	char fs1[100];
	if(sds_rtos(printedUnits,-1,-1,fs1)==RTNORM)
		m_PrintedUnits = fs1;
	UpdateData(FALSE);	

	::SetWindowText(::GetDlgItem(GetSafeHwnd(),PRN_TXT_PRN_UNITS),strPrintUnit);
	SetPaperSize();

	CPrintDialogSheet*parent = (CPrintDialogSheet*) GetParent();
	parent->SetUnits(m_Scale);
}

void PrintScaleViewTab::OnFitToPage() 
{
	UpdateData(TRUE);
	if(m_FitMode == 0)
	{
		::EnableWindow(::GetDlgItem(m_hWnd,PRN_PLOTUNITS),TRUE);
		::EnableWindow(::GetDlgItem(m_hWnd,PRN_PRINTUNITS),TRUE);
		//*** Initialize these strings to 1.
/*		char fs1[IC_ACADBUF];
		if(sds_rtos(m_drawUnitScale,-1,-1,fs1)==RTNORM)
		{
			m_DrawingUnits=fs1;
		}
		if(sds_rtos(m_printUnitScale,-1,-1,fs1)==RTNORM)
		{
			m_PrintedUnits=fs1;
		}
*/		UpdateData(FALSE);
	}
	else
	{
		::EnableWindow(::GetDlgItem(m_hWnd,PRN_PLOTUNITS),FALSE);
		::EnableWindow(::GetDlgItem(m_hWnd,PRN_PRINTUNITS),FALSE);
		UpdateFitSize();
	}
}

void PrintScaleViewTab::OnSelectArea() 
{
	UpdateData(TRUE);

	CPrintDialogSheet *parent = (CPrintDialogSheet*) GetParent();
	parent->SaveRegSettings();
	parent->EndDialog(PRN_SELECT_AREA);
}

BOOL PrintScaleViewTab::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	long ssct;
	if(sds_sslength(m_ssPrintEnt,&ssct)==RTNORM && ssct>0L)
	{
		::EnableWindow(::GetDlgItem(GetSafeHwnd(),PRN_SELMODES),TRUE);
	}
	else
	{
		::EnableWindow(::GetDlgItem(GetSafeHwnd(),PRN_SELMODES),FALSE);
	}

	if(m_ViewList->IsEmpty())
	{
		//*** If there aren't any views defined in the drawing gray out the
		//*** Saved view option.
		if(m_View==PRN_OPT_SAVEDVIEW)
		{
			m_View=PRN_OPT_CURRENTVIEW;
			UpdateData(FALSE);
		}
		::EnableWindow(::GetDlgItem(GetSafeHwnd(),PRN_VIEW4),FALSE);
		::EnableWindow(::GetDlgItem(GetSafeHwnd(),PRN_VIEWLIST),FALSE);
	}
	else
	{
		struct sds_resbuf* pRb;
		CComboBox* pViewListBox = (CComboBox*)GetDlgItem(PRN_VIEWLIST);
		if(pViewListBox!=NULL)
		{
			POSITION pos = m_ViewList->GetHeadPosition();
			while(pos!=NULL && pViewListBox!=NULL)
			{
				pRb = (struct sds_resbuf*) m_ViewList->GetNext(pos);
				if(ic_assoc(pRb,2)==0) pViewListBox->AddString(ic_rbassoc->resval.rstring);
			}
			if(m_ViewName.IsEmpty() ||
				LB_ERR==pViewListBox->SelectString(-1,m_ViewName))
			{
				pViewListBox->SetCurSel(0);
			}
		}
	}

	initSettings();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

bool PrintScaleViewTab::initSettings() 
{
	// Restory previouse settings from REGISTRY if we returned to print dialog from print preview dialog.
	// (especially it is necessary if UserPreference::s_bSaveToLayout is off)
	CPrintDialogSheet* pPrintDlg = (CPrintDialogSheet*) GetParent();
	ASSERT(pPrintDlg);
	LoadRegSettings(!pPrintDlg->isRestorySettingsFromRegistry());

	if(m_bWasPicked)
	{
		char value[IC_ACADBUF];
		sds_rtos(m_winX1, -1, -1, value);
		m_winFromX = value;
		sds_rtos(m_winY1, -1, -1, value);
		m_winFromY = value;
		sds_rtos(m_winX2, -1, -1, value);
		m_winToX = value;
		sds_rtos(m_winY2, -1, -1, value);
		m_winToY = value;
		m_bWasPicked = false;
		UpdateData(FALSE);
	}
	//*** Initialize controls
	OnUnitsChanged();
	OnViewRadio();
	OnFitToPage();
	UpdateFitSize();

	return true;
}

BOOL PrintScaleViewTab::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	int wNotifyCode = HIWORD(wParam); // notification code 
	int wID = LOWORD(wParam);		  // item, control, or accelerator identifier 
//	HWND hwndCtl = (HWND) lParam;	   // handle of control 	

	// don't know the #defined version of this message.
	if (wNotifyCode == 0)
	{
		// want to send escape codes to parent
		// wID 2 == ESC 
		// wID 1 == Enter from Edit/Combo box
		if ((wID == 2) || (wID == 1))
			return GetParent()->SendMessage(WM_COMMAND, wParam, lParam);
	}
	
	return CPropertyPage::OnCommand(wParam, lParam);
}

void PrintScaleViewTab::SetDefaults()
{
	m_View=PRN_OPT_CURRENTVIEW;
	m_Scale=PRN_OPT_INCHES;
	m_FitMode=1;
	m_ClipMode=0;

	//*** Tell pen map page to change scale units.
	SetUnits();

	//*** Initialize these strings to 0,0.
	char fs1[IC_ACADBUF];
	if(sds_rtos(0.0,-1,-1,fs1)==RTNORM)
	{
		m_winFromX = fs1; 
		m_winFromY = fs1;
		m_winToX = fs1; 
		m_winToY = fs1;
	}

	//*** Initialize these strings to 1.
	if(sds_rtos(1.0,-1,-1,fs1)==RTNORM)
	{
		m_DrawingUnits = fs1;
		m_PrintedUnits = fs1;
	}

	m_drawUnitScale = 1.0;
	m_printUnitScale = 1.0;

	UpdateData(FALSE);

	OnViewRadio();
	OnFitToPage();
}

// EBATECH(CNBR) -[ Bugzilla:78569 Tell PenMap tab to change pen units.
void PrintScaleViewTab::SetUnits()
{
	CPrintDialogSheet *parent = (CPrintDialogSheet *) GetParent();
	parent->SetUnits(m_Scale);
}
// EBATECH(CNBR) ]-


/********************************************************************************
 * Author:	Sachin Dange.
 *
 * Purpose:	To copy the previous settings of the print property sheet
 *
 * Returns:	void
 ********************************************************************************/

void PrintScaleViewTab::CopyList()
{
		
	if (m_bPreviewWasPressed && !m_bApplyWasPressed)
		return;
	
	sds_name layout;
	if(!m_bApplyWasPressed)
	{
	if (cmd_getCurrentLayout(layout))
		pLayout = sds_entget(layout);
	}

}
