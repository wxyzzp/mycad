/* G:\ICADDEV\PRJ\LIB\AUTO\PLOTPROPERTIES.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

// PlotProperties.cpp : Implementation of PlotProperties

#include "stdafx.h"
#include "PlotProperties.h"
#include "icadmain.h"
#include "icadview.h"

typedef	CComQIPtr< IIcadPlotterPen, &IID_IIcadPlotterPen >			QIIcadPlotterPen;
typedef CComQIPtr< IIcadPlotProperties, &IID_IIcadPlotProperties>	QIIcadPlotProperties;


IIcadPlotManager *
CreatePlotManager()
	{
	IIcadPlotManager*	object = new CComObject< PlotManager>;
	object->AddRef();
	return object;
	}


IIcadPlotProperties *
CreatePlotProperties()
	{
	IIcadPlotProperties*	object = new CComObject< PlotProperties>;
	object->AddRef();
	return object;
	}


// **********************
// PlotterPen

STDMETHODIMP PlotterPen::get_Weight( double *weight)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if (!weight)
		return E_INVALIDARG;

	*weight = m_plotConfig->pens[m_currentPen].weight;
	return S_OK;
}

STDMETHODIMP PlotterPen::put_Weight( double weight)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	m_plotConfig->pens[m_currentPen].weight = weight;
	return S_OK;
}

STDMETHODIMP PlotterPen::get_Number( short *number)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if (!number)
		return E_INVALIDARG;

	*number = m_plotConfig->pens[m_currentPen].number;
	return S_OK;
}

STDMETHODIMP PlotterPen::put_Number( short number)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	m_plotConfig->pens[m_currentPen].number = number;
	return S_OK;
}

STDMETHODIMP PlotterPen::get_LineType( short *type)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if (!type)
		return E_INVALIDARG;

	*type = m_plotConfig->pens[m_currentPen].linetype;

	return S_OK;
}

STDMETHODIMP PlotterPen::put_LineType( short type)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	m_plotConfig->pens[m_currentPen].linetype = type;

	return S_OK;
}


// **********************
// PlotterPen

STDMETHODIMP PlotterPens::get_Count( short *count)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*count = 256;
	return S_OK;
}

STDMETHODIMP PlotterPens::get_Item( VARIANT index, IIcadPlotterPen* *pInterface)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	PlotterPen *pen = new CComObject<PlotterPen>;

	if ( !pen )
		return E_OUTOFMEMORY;

	DWORD ref = pen->AddRef();
	int	i = 0;
	switch ( index.vt )
		{
		case VT_I2:
			i = index.iVal;
			break;
		case VT_I4:
			i = index.iVal;
			break;
		}

	pen->SetPlotConfig(m_plotConfig);
	pen->SetCurrentPen(i);

	*pInterface = pen;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// PlotProperties


STDMETHODIMP PlotProperties::get_PenWidth(float * pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if (!pVal)
		return E_INVALIDARG;

	*pVal = (float) m_plotConfig->penwidth;

	return S_OK;
}


STDMETHODIMP PlotProperties::put_PenWidth(float newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	m_plotConfig->penwidth = newVal;

	return S_OK;
}

STDMETHODIMP PlotProperties::get_Scale(BSTR * pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	CString scaleStr(m_plotConfig->scale);

	*pVal = scaleStr.AllocSysString();

	return S_OK;
}

STDMETHODIMP PlotProperties::put_Scale(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	CString scaleStr(newVal);

	_tcscpy(m_plotConfig->scale, scaleStr.GetBuffer(0));

	return S_OK;
}

STDMETHODIMP PlotProperties::get_Rotate(short * pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if (!pVal)
		return E_INVALIDARG;

	*pVal = m_plotConfig->rotate;

	return S_OK;
}

STDMETHODIMP PlotProperties::put_Rotate(short newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	m_plotConfig->rotate = newVal;

	return S_OK;
}

STDMETHODIMP PlotProperties::get_OptimizeLevel(short * pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if (!pVal)
		return E_INVALIDARG;
	
	*pVal = m_plotConfig->optimizelevel;

	return S_OK;
}

STDMETHODIMP PlotProperties::put_OptimizeLevel(short newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	m_plotConfig->optimizelevel = newVal;

	return S_OK;
}

STDMETHODIMP PlotProperties::get_Version(float * pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if (!pVal)
		return E_INVALIDARG;

	*pVal = (float) m_plotConfig->version;

	return S_OK;
}

STDMETHODIMP PlotProperties::get_Units(BSTR * pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	CString unitStr(m_plotConfig->units);

	*pVal = unitStr.AllocSysString();

	return S_OK;
}

STDMETHODIMP PlotProperties::put_Units(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	CString unitStr(newVal);

	_tcscpy(m_plotConfig->units, unitStr.GetBuffer(0));

	return S_OK;
}

STDMETHODIMP PlotProperties::get_OriginX(double * pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if (!pVal)
		return E_INVALIDARG;

	*pVal = m_plotConfig->origin[0];

	return S_OK;
}

STDMETHODIMP PlotProperties::put_OriginX(double newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	m_plotConfig->origin[0] = newVal;

	return S_OK;
}

STDMETHODIMP PlotProperties::get_OriginY(double * pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if (!pVal)
		return E_INVALIDARG;

	*pVal = m_plotConfig->origin[1];

	return S_OK;
}

STDMETHODIMP PlotProperties::put_OriginY(double newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	m_plotConfig->origin[1] = newVal;

	return S_OK;
}

STDMETHODIMP PlotProperties::get_Pens(IIcadPlotterPens* *pInterface)
	{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	PlotterPens *pens  = new CComObject<PlotterPens>;
	if ( !pens )
		return E_OUTOFMEMORY;

	pens->SetPlotConfig(m_plotConfig);

	*pInterface = pens;

	DWORD ref = (*pInterface)->AddRef();
	return S_OK;
	}

// EBATECH(CNBR) -[ 24-03-2003 Bugzilla#78471 Support Lineweight
STDMETHODIMP PlotProperties::get_UseLWeight(BOOL * pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = m_plotConfig->useLWeight;

	return S_OK;
}

STDMETHODIMP PlotProperties::put_UseLWeight(BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	m_plotConfig->useLWeight = newVal;

	return S_OK;
}

STDMETHODIMP PlotProperties::get_ScaleLWeight(BOOL * pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = m_plotConfig->scaleLWeight;

	return S_OK;
}

STDMETHODIMP PlotProperties::put_ScaleLWeight(BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	m_plotConfig->scaleLWeight = newVal;

	return S_OK;
}
// EBATECH(CNBR) ]-

	// **********************
	// PlotManager

PlotManager::PlotManager() : m_plotConfig(PlotCfg()), m_properties(NULL)
	{
	HRESULT	result =  ImportRegPlotProperties();
//	ASSERT( SUCCEEDED( result) );
	}


PlotManager::~PlotManager()
	{
	HRESULT	result =  ExportRegPlotProperties();
	ASSERT( SUCCEEDED( result) );
	}


STDMETHODIMP PlotManager::get_PlotProperties( IIcadPlotProperties* *pInterface)
	{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

		// create initial properties, if necessary
	if ( !m_properties )
		{
		PlotProperties *properties = new CComObject<PlotProperties>;
		if (!properties)
			return E_OUTOFMEMORY;

		properties->SetPlotConfig(m_plotConfig);

		m_properties = properties;
		}

		// return new interface pointer
	*pInterface = m_properties;
	DWORD ref = (*pInterface)->AddRef();
	return S_OK;
	}

STDMETHODIMP PlotManager::ImportPlotProperties(BSTR filename)
	{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	CString file (filename);
	m_plotConfig->importPCP(file);

	::SysFreeString(filename);

	return S_OK;
	}


STDMETHODIMP PlotManager::ExportPlotProperties(BSTR filename)
	{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	CString file (filename);
	m_plotConfig->exportPCP(file);

	::SysFreeString(filename);

	return S_OK;
	}

STDMETHODIMP PlotManager::ImportRegPlotProperties()
	{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if ( m_plotConfig->readPlotConfigFromReg() != RTNORM )
		return E_FAIL;

	return S_OK;
	}

STDMETHODIMP PlotManager::ExportRegPlotProperties()
	{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if ( m_plotConfig->writePlotConfigToReg() != RTNORM )
		return E_FAIL;

	return S_OK;
	}

STDMETHODIMP PlotManager::get_RegPlotProperties(IIcadPlotProperties * *pInterface)
	{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	PlotProperties *properties = new CComObject<PlotProperties>;
	if (!properties)
		return E_OUTOFMEMORY;

	properties->SetPlotConfig(m_plotConfig);

	*pInterface = properties;

	DWORD ref = (*pInterface)->AddRef();

	return S_OK;
	}


STDMETHODIMP PlotManager::Print()
{
	/*DG - 28.11.2001*/// Implemented the method.
	CWnd*		pMainWnd = AfxGetMainWnd();
    CView*		pWinView = ((CMDIFrameWnd*)pMainWnd)->GetActiveFrame()->GetActiveView();
	CIcadView*	pIcadView = (CIcadView*)((CMainWindow*)pMainWnd)->m_pCurView;
	if(!pIcadView || pWinView != pIcadView)
		return E_FAIL;

	pIcadView->FilePrint(FALSE);

	return S_OK;
}

STDMETHODIMP PlotManager::Preview()
{
	/*DG - 28.11.2001*/// Implemented the method.
	CWnd*		pMainWnd = AfxGetMainWnd();
    CView*		pWinView = ((CMDIFrameWnd*)pMainWnd)->GetActiveFrame()->GetActiveView();
	CIcadView*	pIcadView = (CIcadView*)((CMainWindow*)pMainWnd)->m_pCurView;
	if(!pIcadView || pWinView != pIcadView || ((CMainWindow*)pMainWnd)->m_bPrintPreview)
		return E_FAIL;

	pIcadView->FilePrintPreview();

	return S_OK;
}

STDMETHODIMP PlotManager::get_Area(PrintArea * pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if (!pVal)
		return E_INVALIDARG;

	*pVal = m_plotConfig->area;

	return S_OK;
}

STDMETHODIMP PlotManager::put_Area(PrintArea newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	m_plotConfig->area = newVal;

	return S_OK;
}

STDMETHODIMP PlotManager::get_HideLines(BOOL * pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	//Bugzilla::78482; 14-04-03;
	if ( strcmp(m_plotConfig->hide, "_N" ) == 0 )
		*pVal = FALSE;
	else
		*pVal = TRUE;

	return S_OK;
}

STDMETHODIMP PlotManager::put_HideLines(BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	//Bugzilla::78482; 14-04-03;
	if ( newVal == FALSE )
		strcpy(m_plotConfig->hide, "_N");
	else
		strcpy(m_plotConfig->hide, "_Y");

	return S_OK;
}


STDMETHODIMP PlotManager::get_PaperOrientation(Orientation * pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	//Bugzilla::78482; 14-04-03;
	if (!pVal)
		return E_INVALIDARG;
	
	if ( m_plotConfig->m_paperOrientation == 2 )
		*pVal = Landscape;
	else 
		*pVal = Portrait;	

	return S_OK;
}

STDMETHODIMP PlotManager::put_PaperOrientation(Orientation newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	//Bugzilla::78482; 14-04-03;
	if ( newVal == Portrait ) 
		m_plotConfig->m_paperOrientation = 1;	
	else if ( newVal == Landscape ) 
		m_plotConfig->m_paperOrientation = 2;	

	return S_OK;
}

STDMETHODIMP PlotManager::get_Header(BSTR * pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	CString headerStr(m_plotConfig->header);

	*pVal = headerStr.AllocSysString();

	return S_OK;
}

STDMETHODIMP PlotManager::put_Header(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	CString headerStr(newVal);
	_tcscpy(m_plotConfig->header, headerStr.GetBuffer(0));
	
	return S_OK;
}

STDMETHODIMP PlotManager::get_Footer(BSTR * pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	CString footerStr(m_plotConfig->footer);
	*pVal = footerStr.AllocSysString();

	return S_OK;
}

STDMETHODIMP PlotManager::put_Footer(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	CString footerStr(newVal);
	_tcscpy(m_plotConfig->footer, footerStr.GetBuffer(0));

	return S_OK;
}

STDMETHODIMP PlotManager::get_OnlyWindowArea(BOOL * pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if (!pVal)
		return E_INVALIDARG;

	*pVal = m_plotConfig->onlyWindowArea;

	return S_OK;
}

STDMETHODIMP PlotManager::put_OnlyWindowArea(BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	m_plotConfig->onlyWindowArea = newVal;

	return S_OK;
}

STDMETHODIMP PlotManager::get_CenterOnPage(BOOL * pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if (!pVal)
		return E_INVALIDARG;

	*pVal = m_plotConfig->centerOnPage;

	return S_OK;
}

STDMETHODIMP PlotManager::put_CenterOnPage(BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	m_plotConfig->centerOnPage = newVal;

	return S_OK;
}


STDMETHODIMP PlotManager::get_View(BSTR * pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	CString viewStr(m_plotConfig->view);
	*pVal = viewStr.AllocSysString();

	return S_OK;
}

STDMETHODIMP PlotManager::put_View(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	CString viewStr(newVal);
	_tcscpy(m_plotConfig->view, viewStr.GetBuffer(0));

	return S_OK;
}

STDMETHODIMP PlotManager::GetWindow(double *fromX, double *fromY, double *toX, double *toY)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if (!fromX || !fromY || !toX || !toY)
		return E_INVALIDARG;

	*fromX = m_plotConfig->fromX;
	*fromY = m_plotConfig->fromY;
	*toX = m_plotConfig->toX;
	*toY = m_plotConfig->toY;

	return S_OK;
}

STDMETHODIMP PlotManager::SetWindow(double fromX, double fromY, double toX, double toY)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	m_plotConfig->fromX = fromX;
	m_plotConfig->fromY = fromY;
	m_plotConfig->toX = toX;
	m_plotConfig->toY = toY;

	return S_OK;
}

STDMETHODIMP PlotManager::GetPaperSize(double * length, double * width)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	//Bugzilla::78482; 14-04-03;
	*length =	m_plotConfig->m_paperLength;
	*width	=	m_plotConfig->m_paperWidth;

	return S_OK;
}

STDMETHODIMP PlotManager::SetPaperSize(double length, double width)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	//Bugzilla::78482; 14-04-03;
	m_plotConfig->m_paperLength = length;
	m_plotConfig->m_paperWidth	= width;

	return S_OK;
}


bool PlotManager::getSystemDefaultPrinterSettings()
{
	return m_plotConfig->getSystemDefaultPrinterSettings();
}


// Bugzilla:78647;	Date:05-12-03.
#ifdef USE_DEFAULT_PRINTER_THROUGH_INTERFACE
STDMETHODIMP PlotManager::GetDefaultPrinterSettings()
{
	bool bReturnValue = m_plotConfig->getSystemDefaultPrinterSettings();

	return bReturnValue ? S_OK : E_FAIL;
}
#endif
// Bugzilla:78647;	Date:05-12-03.
