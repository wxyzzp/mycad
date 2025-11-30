/* F:\DEV\PRJ\LIB\AUTO\ICADPLOTCFG.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 
#ifndef ICADPLOTCFG_H_
#define ICADPLOTCFG_H_

#include <afxole.h>

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC OLE automation classes
#endif // _AFX_NO_OLE_SUPPORT


#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>			// MFC ODBC database classes
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>			// MFC DAO database classes
#endif // _AFX_NO_DAO_SUPPORT

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT


#include  <stdio.h>
#include  <math.h>
#include  <string.h>
#include  <ctype.h>
#include  <stdlib.h>
#include  <errno.h>

#ifdef WIN32	// include this before ADS includes 
#ifndef _AFXDLL // GAK - 11/3/97 - added to be MFC compatable
#include <windows.h>
#endif // AFXDLL
#include <io.h>
#define FNULL 0
#endif

#ifdef  STANDALONE
#define RTERROR 0
#define RTNORM 1
#define sds_alert(a) fprintf(stdout, a);
#endif

#define PAIR 2
#define SMALLBUF 20
#define MEDIUMBUF 100
#define IC_PENSBUF 256
#define PCPFILESIZE 50000

#define EOL        "\n"/*DNT*/
#define SPACECHR   ' '/*DNT*/
#define COMMACHR   ','/*DNT*/
#define EOSCHR     '\0'/*DNT*/
#define EQUALCHR   '='/*DNT*/
#define EQUALSTR   "="/*DNT*/
#define DASHCHR    '-'/*DNT*/
#define BRACKETCHR ']'/*DNT*/

///////////////////////////////////////////////////////////////////////////////////////
// Note for ease of knowing where you are, index [0] is always empty for
// color, penColor, and weight.
struct PenSettings{
    int    color;
    int    number;
	int    linetype;
	int    speed;
    double weight;
};

///////////////////////////////////////////////////////////////////////////////////////

class PlotCfg {

	public:

			 // Constructor fills in resonable defaults.
		     PlotCfg();
		     ~PlotCfg();
		int  importPCP( LPCTSTR FileName);
		int  exportPCP( LPCTSTR FileName);
		int  importICP( LPCTSTR FileName);
		int  exportICP( LPCTSTR FileName);
		int  writePlotConfigToReg();
		int  readPlotConfigFromReg();
		     // If there is a plot key in IntelliCAD's registry,
			 // this function returns true. Doesn't check all entries.
		BOOL hasRegEntries();
		
	private:

		BOOL fileExists(const char* FileName);

	public:

		// PCP header stuff.
		int   rotate;
		int   optimizelevel;

		double version;
		double origin[PAIR];
		double penwidth;

		//Bugzilla No. 62780 ; 23-09-2002 
        //char   scale[SMALLBUF];
        char   scale[MEDIUMBUF];
		char   units[SMALLBUF];
		char   hide[SMALLBUF];
		char   plotfile[IC_PENSBUF];
		char   filladjust[SMALLBUF];
		char   size[SMALLBUF];

		struct PenSettings pens[IC_PENSBUF];

		//registry/print/plot only - not pcp file
		PrintArea area;
		double fromX, fromY, toX, toY;
		char view[MEDIUMBUF];
		char header[MEDIUMBUF];
		char footer[MEDIUMBUF];
		BOOL onlyWindowArea;
		BOOL centerOnPage;
		// EBATECH(CNBR) 24-03-2003 Bugzilla#78471 Support Lineweight
		BOOL useLWeight;
		BOOL scaleLWeight;

		// psetup dialog settings
		char	m_printerName[CCHDEVICENAME];
		short	m_paperOrientation;
		short	m_paperSize;
		short	m_paperLength;
		short	m_paperWidth;
		short	m_paperSource;

		bool getSystemDefaultPrinterSettings();
};


#endif // ICADPLOTCFG_H_

