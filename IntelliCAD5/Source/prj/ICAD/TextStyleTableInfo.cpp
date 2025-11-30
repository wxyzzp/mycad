/* PRJ\ICAD\TEXTSTYLETABLEINFO.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 *
 * $Revision: 1.2 $ $Date: 2001/07/24 14:59:02 $
 *
 * Abstract
 *
 * Support class for dealing with text styles.
 *
 */

#include "Icad.h"/*DNT*/
#include "IcadView.h"
#include "TextStyleTableInfo.h"/*DNT*/



/////////////////////////////////////////////////////////////////////////////
// CTextStyleDlg construction / destruction


/******************************************************************************
*+ CTextStyleTableInfo::CTextStyleTableInfo - constructor
*
*/

CTextStyleTableInfo::CTextStyleTableInfo()
	{
	m_drawing			= SDS_CURDWG;

	m_height			= 0.0;
	m_width				= 1.0;
	m_obliqueAngle		= 0.0;

	m_backwards			= FALSE;
	m_upsideDown		= FALSE;
	m_vertically		= FALSE;

	m_styleName			= _T(""/*DNT*/);
	m_fontFilename		= _T(""/*DNT*/);
	m_fontFamily		= _T(""/*DNT*/);
	m_bigfontFilename	= _T(""/*DNT*/);	// CNBR

	m_fontStyle			= 34;			// default to Arial Regular

	m_hasTrueTypeFont	= false;
	}

/******************************************************************************
*+ CTextStyleTableInfo::~TextStyleTableInfo - destructor
*
*/

CTextStyleTableInfo::~CTextStyleTableInfo()
	{
	}

/******************************************************************************
*+ GetIcadTargetDeviceCDC - gets the target device CDC for the main windows CIcadView
*
*/

CDC *GetIcadTargetDeviceCDC(
	void)
	{
	CDC *dc = NULL;

	// we want the target device for the main window's CIcadView
	CIcadView *icadView = SDS_CMainWindow->GetIcadView();
	if (icadView)
		{
		ASSERT_KINDOF(CIcadView, icadView);
		dc = icadView->GetTargetDeviceCDC();
		}

	ASSERT(NULL != dc);
	return dc;
	}



