/** FILENAME:     Text.cpp- Contains implementation of class CText.
* Copyright  (C) Copyright 1998-99  Visio Corporation.  All Rights Reserved.
*
*
*
* Abstract 
*	this class Maps the basic tiles (text,text_part,error_tile	)
*	to windows control
*
*
*Audit Log 
*
*
*Audit Trail
*
*
*/


#include "stdafx.h"
#include "Text.h"
#include "sds.h"
#include "constants.h"
#include "resource.h"

#include "ItcLanguage.h" //EBATECH

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/************************************************************************************
/* + Text::Text - 
*
*	Construction
*
*
*
*/

Text::Text()
	{
	}

/************************************************************************************
/* + Text::Text(int nType, int ParentAlignmen - 
*
*	overloaded constructor
*
*
*
*/

Text::Text(int Type,			// i:type weather text text_part or errtile
		   int ParentAlignment)	// i:parent alignment
	{
	nType = Type;
	m_ParentAlignment = ParentAlignment;
	}
/************************************************************************************
/* + Text::~Text - 
*
*	Destructor
*
*
*
*/

Text::~Text()
	{
	}


BEGIN_MESSAGE_MAP(Text, CStatic)
	//{{AFX_MSG_MAP(Text)
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/************************************************************************************
/* + void Text::OnKillFocus(CWnd* pNewWnd - 
*
*	command handler for text ctrl KillFocus maped to reasion SDS_CBR_LOST_FOCUS
*	in the callback function
*
*
*
*/

void Text::OnKillFocus(CWnd* pNewWnd) 
	{
	CStatic::OnKillFocus(pNewWnd);
	
	// TODO: Add your message handler code here
	
	}
/************************************************************************************
/* + void Text::OnSetFocus(CWnd* pOldWnd - 
*
*	command handler for text ctrl SetFocus maped to reasion SDS_CBR_SELECT
*	in the callback function
*
*
*
*/

void Text::OnSetFocus(CWnd* pOldWnd) 
	{
	CStatic::OnSetFocus(pOldWnd);
	
	// TODO: Add your message handler code here
	
	}

/************************************************************************************
/* + int Text:: CreateOnScreenObj (CWnd* pParen - 
*
*	creates the windows UI control
*	
*
*
*
*/

int Text:: CreateOnScreenObject (
								 CWnd* pParent	// i:pointer to the parent window
								 )
	{
	int error;
	DWORD dwStyle = WS_CHILD|WS_VISIBLE; // set the windows style for control

	if (!m_attriblist->IsEnabled())
		{
		// set style to enable
		dwStyle = dwStyle|WS_DISABLED;
		}
	if (m_attriblist->IsTabstop())
		{
		// set style to tabstop
		dwStyle = dwStyle|WS_TABSTOP;
		}

	switch(m_attriblist->get_Alignment())
		{
		case ALIGN_LEFT:
		case ALIGN_DEFAULT:
			dwStyle = dwStyle| ES_LEFT ;
			break;
		case ALIGN_RIGHT:
			dwStyle = dwStyle| ES_RIGHT ;
			break;
		case ALIGN_CENTERED:
			dwStyle = dwStyle| ES_CENTER;
			break;
		case ALIGN_BOTTOM:
		case ALIGN_TOP:
			dwStyle = dwStyle| ES_LEFT ;
			break;
        default:
			ASSERT(FALSE);
		}
	
	CString Label = m_attriblist->get_ValueString ();
	if(Label.IsEmpty ())
		Label= m_attriblist->get_Label();
	CRect rect = PositionControl(); // get the display rect
	//rect.right -=6;
	error = Create(Label, dwStyle, rect, pParent, m_controlID.GetID()); // create the static control
	if (error)
		{
		if(!m_attriblist->IsBold()) // set font depending upon attribute
			{
			// EBATECH(CNBR) 2001-05-05 Fontface needs localize. -[
			CString wszResourceString;
			wszResourceString.LoadString(ID_DCL_DEFAULT_FONTFACE);   // Load the resource string.
			m_ComponentFont->CreatePointFont(DIALOG_FONT_POINTSIZE,wszResourceString,NULL);	// create font
			//m_ComponentFont->CreatePointFont(80,_T("MS Sans Serif"),NULL);	// create font
			// EBATECH(CNBR) ]-
			SetFont(m_ComponentFont,TRUE);

			DWORD dwNewStyle = dwStyle ^ WS_TABSTOP;
			this->ModifyStyle(WS_TABSTOP,dwNewStyle); 

			}
		
		return SDS_RTNORM;	
	}
	else
		return SDS_RSERR;
	}

/************************************************************************************
/* + int Text::ModeTile (int nTileState)
*
*
*
*
*
*/

int Text::ModeTile ( // Executes functions like setting focus to a control , enabling or disabling a control etc.
		int nTileState	   // i:state of control
		)
	{
	CWnd * pWind;
	pWind = (CWnd *)(this);

	switch(nTileState)
		{
		case SDS_MODE_ENABLE:
			pWind->EnableWindow();
			break;
		case SDS_MODE_DISABLE:
			pWind->EnableWindow(FALSE);
			break;
		case SDS_MODE_SETFOCUS:
			pWind->SetFocus();
			break;
		default:
			TRACE ("WRONG PARAMETER IN MODE TILE");
			break;
		}
	return SDS_RTNORM;
	}

/************************************************************************************
/* + int Text:: GetTile (CString & szControlInfo,int lInfoMaxLength)
*
*
*
*
*
*/

int Text:: GetTile (		// Returns the "value" attribute of the tile
		CString & szControlInfo,// o:Date contaned in value attribute
		int lInfoMaxLength	// o: Size of data
		)
	{
	
	if(lInfoMaxLength>0)
		{
		GetWindowText(szControlInfo) ;
		return SDS_RTNORM ; 
		}
	return SDS_RTERROR ; 
	}

/************************************************************************************
/* + int Text::SetTile (CString szControlKey,CString szControlInfo)		
*	
*
*
*
*
*/

int Text::SetTile (		// Sets the "value" attribute of the tile
		CString szControlKey,	// i:Key of the tile
		CString szControlInfo	// i:Data to store in the value attribute
		)
	{
	SetWindowText(szControlInfo);
	return SDS_RTNORM;
	}
