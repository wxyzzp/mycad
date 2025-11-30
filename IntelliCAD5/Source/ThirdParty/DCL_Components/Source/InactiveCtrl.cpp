/** FILENAME:     InactiveCtrl.cpp- Contains implementation of class CInactiveCtrl.
* Copyright  (C) Copyright 1998-99 Visio Corporation.  All Rights Reserved.
*
*
*
* Abstract 
*		this class Maps the basic tiles Row,Column,Boxed_row,Boxed_column,radio_row	
*	radoi_column,Boxed_radoi_row and Boxed_radoi_column to windows control 
*
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
#include "InactiveCtrl.h"
#include "sds.h"
#include "resource.h"

#include "ItcLanguage.h" //EBATECH

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/************************************************************************************
/* + InactiveCtrl::InactiveCtrl()
*
*	Construction
*
*
*
*/

InactiveCtrl::InactiveCtrl()
	{
	
	}

/************************************************************************************
/* + InactiveCtrl::~InactiveCtrl()
*
*	Destructor
*
*
*
*/

InactiveCtrl::~InactiveCtrl()
	{
	}


BEGIN_MESSAGE_MAP(InactiveCtrl, CButton)
	//{{AFX_MSG_MAP(InactiveCtrl)
	ON_WM_GETDLGCODE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/************************************************************************************
/* + InactiveCtrl::InactiveCtrl(int RowColumn,int IsBoxed,int IsRadio,int IsConcatenate,int IsParagraph)		
*	Overloaded Constructor
*	
*	
*	
*	
*	
*/

InactiveCtrl::InactiveCtrl(
						   int RowColumn ,	// i:value which determines weather row or column (true if row else false)
						   int IsBoxed,		// i:value which determines weather boxed
						   int IsRadio,		// i:value which determines weather grouped
						   int IsConcatenate,// i:value which determines weather two text can be concatenated
						   int IsParagraph	 // i:value which determines weather the tile is Paragraph
						   )
	{
	m_IsParagraph = IsParagraph;
	m_IsConcatenate = IsConcatenate;
	m_IsBoxed = IsBoxed;
	m_RadioFlag = IsRadio;
	m_RowColumn = RowColumn;
	}
/************************************************************************************
/* + int InactiveCtrl:: set_IsParagraph( int paragraph )
*
*	method to set m_IsParagraph member
*	
*
*
*/

int InactiveCtrl:: set_IsParagraph(
								   int paragraph // i:value of member
								   )
	{
	m_IsParagraph = paragraph;
	return SDS_RTNORM;
	}
/************************************************************************************
/* + int InactiveCtrl::get_IsParagraph()	
*
*	method to get m_IsParagraph member
*	o:return the value of m_IsParagraph member
*
*
*/

int InactiveCtrl::get_IsParagraph()	 
	{
	return m_IsParagraph;
	}
/************************************************************************************
/* + int InactiveCtrl::set_IsConcatenate(int Concatenate)
*
*	method to set m_IsConcatenate member
*	
*
*
*/

int InactiveCtrl::set_IsConcatenate(
									int Concatenate // i:value of member
									)
	{
	m_IsConcatenate = Concatenate;
	return SDS_RTNORM;	
	}
/************************************************************************************
/* + int InactiveCtrl::get_IsConcatenate(); 
*
*	method to get m_IsConcatenate member
*	o:returns the value
*
*
*/
int InactiveCtrl::get_IsConcatenate()
	{
	return m_IsConcatenate;
	}
/************************************************************************************
/* + int set_IsBoxed( int Boxed)
*
*	method to set m_IsBoxed member
*	i:value of member
*
*
*/

int InactiveCtrl::set_IsBoxed(				// method to set m_IsBoxed member.
							  int Boxed		// i:value of member.
							  )
	{
	m_IsBoxed = Boxed;
	return SDS_RTNORM;
	}
/************************************************************************************
/* + int InactiveCtrl::get_IsBoxed()	
*
*	method to get m_IsBoxed member
*	o:returns value of m_IsBoxed member
* 
*
*/

int InactiveCtrl::get_IsBoxed()
	{
	return m_IsBoxed;
	}
/************************************************************************************
/* + int InactiveCtrl::set_RadioFlag(int RadioFlag)
*
*	method to set m_RadioFlag member
*	
*
*
*/

int InactiveCtrl::set_RadioFlag(
								int RadioFlag // i:value of member
								)
	{
	m_RadioFlag = RadioFlag;
	return SDS_RTNORM; 
	}
/************************************************************************************
/* + int InactiveCtrl::get_RadioFlag()	
*
*	method to get m_RadioFlag member
*	o:returns the value 
*
*
*/

int InactiveCtrl::get_RadioFlag()
	{
	return m_RadioFlag;
	}
/************************************************************************************
/* + int InactiveCtrl::set_RowColumn(int RowColumn) 
*
*	method to set m_RowColumn member
*	
*
*
*/

int InactiveCtrl::set_RowColumn(
								int RowColumn // i:value of member
								)
	{
	m_RowColumn= RowColumn;
	return SDS_RTNORM;
	}
/************************************************************************************
/* + int InactiveCtrl::get_RowColumn()	
*
*	method to get m_RowColumn member.
*
*
*
*/

int InactiveCtrl::get_RowColumn()
	{
	return m_RowColumn;
	}


/************************************************************************************
/* + int InactiveCtrl:: CreateOnScreenObj (CWnd* pParent) 
*
*	creates the windows UI control
*	i:pointer to the parent window
*
*	
*
*/

int InactiveCtrl:: CreateOnScreenObject (
										 CWnd* pParent	// i:pointer to the parent window
										 )
	{
	int error;
	CString Label;
	DWORD dwStyle = WS_CHILD|WS_VISIBLE|WS_GROUP;//|WS_EX_TRANSPARENT;

	ValueString = m_attriblist->get_ValueString();
	if (m_IsBoxed)
		{
		// set style to enable
		dwStyle = dwStyle|BS_GROUPBOX;
	
		Label = m_attriblist->get_Label();
		}
	else
	{
		if(Label.IsEmpty())
			return SDS_RTNORM;
	}
	
	CRect rect ; 
	rect = PositionControl();	// get actual diaplay position of control
	
	Label.TrimLeft(); // removes the blank spaces to the left of the given label
	// if label is empty then pass null to create as 1st parameter else pass the string
	if(Label.IsEmpty())
		error = Create(NULL, dwStyle, rect, pParent, m_controlID.GetID());
	else
		{
		// EBATECH(CNBR) 2001-05-05 Fontface needs localize. -[
		CString wszResourceString;
		wszResourceString.LoadString(ID_DCL_DEFAULT_FONTFACE);   // Load the resource string.
		int i=m_ComponentFont->CreatePointFont(DIALOG_FONT_POINTSIZE,wszResourceString,NULL);
		//int i=m_ComponentFont->CreatePointFont(80,_T("MS Sans Serif"),NULL);
		// EBATECH(CNBR) ]-
		error = Create(Label, dwStyle, rect, pParent, m_controlID.GetID());
		if (error)
			{
			// if window is created then set font
			SetFont(m_ComponentFont,TRUE);
			return SDS_RTNORM;
			}
		}
	if (error)
		return SDS_RTNORM;	
	else
		return SDS_RSERR;
	}
/************************************************************************************
/* + CRect InactiveCtrl:: PositionControl()
*	Returns the DisplayArea.
*	allways the display area is equal to its actual area
*
*
*
*/
CRect InactiveCtrl:: PositionControl()
	{
	return m_DisplayArea;
	}


/************************************************************************************
/* + int InactiveCtrl::ModeTile ( int nTileState)
*	Executes functions like setting focus to a control , enabling or disabling a control etc.
*	nTileState = 0  then enable window
*	nTileState = 1	disable window
*	nTileState = 2  set Focus
*
*
*
*/

int InactiveCtrl::ModeTile ( // Executes functions like setting focus to a control , enabling or disabling a control etc.
		int nTileState	   // i:state of control
		)
	{
	return SDS_RTNORM;
	}

/************************************************************************************
/* + int InactiveCtrl:: GetTile (CString & szControlInfo, int lInfoMaxLength)
*
*	Returns the "value" attribute of the tile
*
*
*
*/

int InactiveCtrl:: GetTile (		// Returns the "value" attribute of the tile
		CString & szControlInfo,// o:Date contaned in value attribute
		int lInfoMaxLength	// o: Size of data
		)
	{
	//  if size of data requested is greater then 0 
	//  then return value ValueString  which holdes value attribute of the control
	if(lInfoMaxLength>0)
		{
		szControlInfo = ValueString ;
		return SDS_RTNORM ; 
		}
	return SDS_RTERROR ; 
	}
/************************************************************************************
/* + BOOL InactiveCtrl::PreCreateWindow(CREATESTRUCT& cs - 
*
* MFC FUNCTION used to set extended style 
*
*
*
*/

BOOL InactiveCtrl::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class
	cs.dwExStyle |=  WS_EX_TRANSPARENT; 
	
	return CButton::PreCreateWindow(cs);
}


UINT InactiveCtrl::OnGetDlgCode() 
{
	// TODO: Add your message handler code here and/or call default
	
	//return CButton::OnGetDlgCode();
	return DLGC_STATIC;
}
