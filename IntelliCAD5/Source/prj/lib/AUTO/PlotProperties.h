/* G:\ICADDEV\PRJ\LIB\AUTO\PLOTPROPERTIES.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

// PlotProperties.h : Declaration of the PlotProperties

#ifndef __PLOTPROPERTIES_H_
#define __PLOTPROPERTIES_H_

#include "IcadDoc.h"
#include "IcadRes.h"       // main symbols
#include "Autorc.h"			// automation symbols
#include "AutoDLL.h"
#include "icadplotcfg.h"
#include "SmartPointer.h"
#include "IcadApp.h"	// see comments in PlotManager about artifical friendship

namespace ICAD_AUTOMATION
	{

/////////////////////////////////////////////////////////////////////////////
// PlotterPen
class ATL_NO_VTABLE PlotterPen : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<PlotterPen, &CLSID_PlotterPen>,
	public IDispatchImpl<IIcadPlotterPen, &IID_IIcadPlotterPen, &LIBID_IntelliCAD>
	{
public:
	PlotterPen() : m_currentPen(0)
		{
		}

	HRESULT SetPlotConfig(SmartPointer<PlotCfg> plotConfig) {m_plotConfig = plotConfig; return S_OK;}
	HRESULT SetCurrentPen(int index) 
		{
		m_currentPen = index; // don't adjust c++ arrays because plotCfg array actually starts at 1
		return S_OK;
		}

	BEGIN_COM_MAP(PlotterPen)
		COM_INTERFACE_ENTRY(IIcadPlotterPen)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()

public:
	// IIcadPlotterPen
	STDMETHOD(get_Weight)( double *weight);
	STDMETHOD(put_Weight)( double weight);
	STDMETHOD(get_Number)( short *number);
	STDMETHOD(put_Number)( short number);
	STDMETHOD(get_LineType)( short *type);
	STDMETHOD(put_LineType)( short type);
	
protected:
	SmartPointer<PlotCfg> m_plotConfig;	
	int m_currentPen;
	};

/////////////////////////////////////////////////////////////////////////////
// PlotterPen
class ATL_NO_VTABLE PlotterPens : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IIcadPlotterPens, &IID_IIcadPlotterPens, &LIBID_IntelliCAD>
	{
public:
	PlotterPens()
		{
		}

	HRESULT SetPlotConfig(SmartPointer<PlotCfg> plotConfig) {m_plotConfig = plotConfig; return S_OK;}

	BEGIN_COM_MAP(PlotterPens)
		COM_INTERFACE_ENTRY(IIcadPlotterPens)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()

public:
	// IIcadPlotterPens
	STDMETHOD(get_Count)( short *count);
	STDMETHOD(get_Item)( VARIANT index, IIcadPlotterPen* *pInterface);

protected:
	SmartPointer<PlotCfg> m_plotConfig;	
	};

/////////////////////////////////////////////////////////////////////////////
// PlotProperties
class PlotProperties : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<PlotProperties, &CLSID_PlotProperties>,
	public IDispatchImpl<IIcadPlotProperties, &IID_IIcadPlotProperties, &LIBID_IntelliCAD>
{
public:
	PlotProperties()
		{
		}

	HRESULT SetPlotConfig(SmartPointer<PlotCfg> plotConfig) {m_plotConfig = plotConfig; return S_OK;}

	BEGIN_COM_MAP(PlotProperties)
		COM_INTERFACE_ENTRY(IIcadPlotProperties)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()

public:
	// IIcadPlotProperties
	STDMETHOD(get_Pens)(/*[out, retval]*/ IIcadPlotterPens* *pVal);
	STDMETHOD(get_OriginX)(/*[out, retval]*/ double *pVal);
	STDMETHOD(put_OriginX)(/*[in]*/ double newVal);
	STDMETHOD(get_OriginY)(/*[out, retval]*/ double *pVal);
	STDMETHOD(put_OriginY)(/*[in]*/ double newVal);
	STDMETHOD(get_Units)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Units)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_Version)(/*[out, retval]*/ float *pVal);
	STDMETHOD(get_OptimizeLevel)(/*[out, retval]*/ short *pVal);
	STDMETHOD(put_OptimizeLevel)(/*[in]*/ short newVal);
	STDMETHOD(get_Rotate)(/*[out, retval]*/ short *pVal);
	STDMETHOD(put_Rotate)(/*[in]*/ short newVal);
	STDMETHOD(get_Scale)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Scale)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_PenWidth)(/*[out, retval]*/ float *pVal);
	STDMETHOD(put_PenWidth)(/*[in]*/ float newVal);
	// EBATECH(CNBR) -[ 24-03-2003 Bugzilla#78471 Support Lineweight
	STDMETHOD(get_UseLWeight)(/*[out, retval]*/ BOOL *pVal);
	STDMETHOD(put_UseLWeight)(/*[in]*/ BOOL newVal);
	STDMETHOD(get_ScaleLWeight)(/*[out, retval]*/ BOOL *pVal);
	STDMETHOD(put_ScaleLWeight)(/*[in]*/ BOOL newVal);
	// EBATECH(CNBR) ]-

protected:
	SmartPointer<PlotCfg> m_plotConfig;
};


/////////////////////////////////////////////////////////////////////////////
// PlotManager


// Bugzilla:78647;	Date:05-12-03
//-----------------------------------------
//******* Modified by Sachin Dange 
//*** Uncomment the following line, if you want the call through the interface "IIcadPlotManager".
//#define USE_DEFAULT_PRINTER_THROUGH_INTERFACE
//-----------------------------------------
// Bugzilla:78647;	Date:05-12-03

class ATL_NO_VTABLE PlotManager : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<PlotProperties, &CLSID_PlotProperties>,
	public IDispatchImpl<IIcadPlotManager, &IID_IIcadPlotManager, &LIBID_IntelliCAD>,
	public IIcadPlotRegManager
	{

	#define USE_PLOTCFG_DIRECTLY							// TO DO: add/implement according props of IIcadPlotManager,
	friend bool CIcadApp::getPrinterSettingsFromPlotcfg();	// remove this ugly friendship and remove this define of
	friend bool CIcadApp::putPrinterSettingsToPlotcfg();	// USE_PLOTCFG_DIRECTLY (changing the friends too, of course)

public:
	PlotManager();
	~PlotManager();
	
	BEGIN_COM_MAP(PlotManager)
		COM_INTERFACE_ENTRY(IIcadPlotManager)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(IIcadPlotRegManager)
	END_COM_MAP()

public:
	STDMETHOD(get_CenterOnPage)(/*[out, retval]*/ BOOL *pVal);
	STDMETHOD(put_CenterOnPage)(/*[in]*/ BOOL newVal);
	STDMETHOD(SetPaperSize)(double length, double width);
	STDMETHOD(GetPaperSize)(double *length, double *width);

	// IIcadPlotManager
	STDMETHOD(ExportPlotProperties)(BSTR filename);
	STDMETHOD(get_PlotProperties)( IIcadPlotProperties* *pInterface);
	STDMETHOD(ImportPlotProperties)( BSTR filename);
	STDMETHOD(SetWindow)(double fromX, double fromY, double toX, double toY);
	STDMETHOD(GetWindow)(double *fromX, double *fromY, double *toX, double *toY);
	STDMETHOD(get_View)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_View)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_OnlyWindowArea)(/*[out, retval]*/ BOOL *pVal);
	STDMETHOD(put_OnlyWindowArea)(/*[in]*/ BOOL newVal);
	STDMETHOD(get_Footer)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Footer)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_Header)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Header)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_PaperOrientation)(/*[out, retval]*/ Orientation *pVal);
	STDMETHOD(put_PaperOrientation)(/*[in]*/ Orientation newVal);
	STDMETHOD(get_HideLines)(/*[out, retval]*/ BOOL *pVal);
	STDMETHOD(put_HideLines)(/*[in]*/ BOOL newVal);
	STDMETHOD(get_Area)(/*[out, retval]*/ PrintArea *pVal);
	STDMETHOD(put_Area)(/*[in]*/ PrintArea newVal);
	STDMETHOD(Preview)();
	STDMETHOD(Print)();

	
	// Bugzilla:78647;	Date:17-11-03.
#ifdef USE_DEFAULT_PRINTER_THROUGH_INTERFACE
	STDMETHOD(GetDefaultPrinterSettings)();
#endif
	// Bugzilla:78647;	Date:17-11-03.

	// IIcadPlotRegManager
	STDMETHOD(get_RegPlotProperties)(IIcadPlotProperties* *pVal);
	STDMETHOD(ImportRegPlotProperties)();
	STDMETHOD(ExportRegPlotProperties)();

	AUTO_DECLSPEC bool getSystemDefaultPrinterSettings();

protected:
	SmartPointer<PlotCfg>			m_plotConfig;
	CComPtr< IIcadPlotProperties>	m_properties;
	};

	}

#endif //__PLOTPROPERTIES_H_


