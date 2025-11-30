/* D:\ITCDEV\PRJ\ICAD\MTEXTSTYLEBOX.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 *
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

// MTextStyleBox.cpp : implementation file
//

#include "icad.h"

#include "MTextStyleBox.h"
#include "styletabrec.h"
#include "icadapi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMTextStyleBox
CMTextStyleBox::CMTextStyleBox()
{
}

CMTextStyleBox::~CMTextStyleBox()
{
}

void CMTextStyleBox::FillStyleComboBox()
{
	char defaultStyle[IC_ACADBUF] = { 0 };
	resbuf rb;
	if(SDS_getvar(NULL, DB_QTEXTSTYLE, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES) == RTNORM)
	{
		strcpy(defaultStyle, rb.resval.rstring);
		IC_FREE(rb.resval.rstring);
		rb.resval.rstring = NULL;
	}
	// get the STYLE table's index
	int tableIndex = db_tabrec2tabfn(db_str2hitype("STYLE"/*DNT*/));
	if(!db_is_tabrectype(tableIndex))
		return;
	
	// add the name of each style to the combo box
	char *styleName;
	db_styletabrec *styleRecord = (db_styletabrec *) (SDS_CURDWG->tblnext(tableIndex, TRUE));
	while (styleRecord)
	{
		styleRecord->get_2(&styleName);
		if(((CString) styleName).Find("|") == -1)
			AddString(styleName);
		styleRecord = (db_styletabrec *) (SDS_CURDWG->tblnext(tableIndex, FALSE));
	}
	
	// if there is no default style name, set to the first style;
	// otherwise, set to the default style name
	
	if((NULL == defaultStyle) || ('\0'/*DNT*/ == defaultStyle[0]))
		SetCurSel(0);
	else
	{
		int comboIndex = FindStringExact(-1, defaultStyle);
		if(CB_ERR == comboIndex)
			SetCurSel(0);
		else
			SetCurSel(comboIndex);
	}
}

void CMTextStyleBox::GetTextStyle()
{
	CString styleName;
	GetLBText(GetCurSel(), styleName);
	db_styletabrec *styleRec = (db_styletabrec *) SDS_CURDWG->findtabrec(DB_STYLETAB,
		styleName,
		1);
	// get the style's values from the record and display them
	char *tmpStr, tmpBuf[IC_ACADBUF];
	sds_real tmpReal;
	
	styleRec->get_2(&tmpStr);
	m_tableInfo.SetStyleName(tmpStr);
	
	bool hasTrueTypeFont = styleRec->hasTrueTypeFont();
	if (hasTrueTypeFont)
	{
		m_tableInfo.SetHasTrueTypeFont(true);
		
		styleRec->getFontFamily(tmpBuf);
		m_tableInfo.SetFontFamily(tmpBuf);
		
		long tmpLong;
		styleRec->getFontStyleNumber(&tmpLong);
		m_tableInfo.SetFontStyle(tmpLong);
	}
	
	styleRec->get_3(tmpBuf, IC_ACADBUF - 1);
	if (!hasTrueTypeFont)					// sometimes shx fonts don't have an extension,
		ic_setext(tmpBuf, ".shx"/*DNT*/);	// we need it for finding the font in combo boxes
	
	char* fontName = (strrchr(tmpBuf, '\\')) ? strrchr(tmpBuf, '\\') + 1 : tmpBuf;	// make sure we just have the font name - no path
	m_tableInfo.SetFontFilename(fontName);
	
	styleRec->get_40(&tmpReal);
	m_tableInfo.SetHeight(tmpReal);
	
}

BEGIN_MESSAGE_MAP(CMTextStyleBox, CComboBox)
	//{{AFX_MSG_MAP(CMTextStyleBox)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMTextStyleBox message handlers

