/** FILENAME:     EditBox.cpp- Contains implementation of class CEditBox.
* Copyright  (C) Copyright 1998-99  Visio Corporation.  All Rights Reserved.
*
*
*
* Abstract 
*	this class Maps the basic tiles edit
*	to windows controls(cobination of CStatic and CEditBox )
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
#include "EditBox.h"
#include "main.h"
#include "sds.h"
#include "dclprivate.h"
#include "Dialog.h"

#include "ItcLanguage.h" //EBATECH

extern Main g_Main;

#undef UINT_MAX 
#define UINT_MAX  0


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



long FAR PASCAL SubEditWndProc(HWND hWnd, unsigned int wMessage, WORD wParam, LONG lParam)
	{
	switch ( wMessage ) 
		{
		case WM_CUT:
		case WM_KEYDOWN:
		case WM_PASTE:
		case WM_CHAR:
		// Do nothing here.
			break;

		default:
			return CallWindowProc (g_Main.lpSubClassProc, hWnd, wMessage, wParam,lParam );
		}
	return 1;

	}


/************************************************************************************
/* + EditBox::EditBox - 
*
*	Construction
*
*
*
*/

EditBox::EditBox()
	{
	}
/************************************************************************************
/* + EditBox::EditBox(Parent_layou - 
*
*	Construction
*
*
*
*/

EditBox::EditBox( int Parent_layout)	// layout of the parent window
	{
	m_ParentAlignment = Parent_layout;
	}
/************************************************************************************
/* + EditBox::~EditBox - 
*
*
*	Destruction
*
*
*/

EditBox::~EditBox()
	{
	}


BEGIN_MESSAGE_MAP(EditBox, CEdit)
	//{{AFX_MSG_MAP(EditBox)
	ON_CONTROL_REFLECT(EN_KILLFOCUS, OnKillfocus)
	ON_CONTROL_REFLECT(EN_SETFOCUS, OnSetfocus)
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/************************************************************************************
/* + void EditBox::OnKillfocus( - 
*
*	command handler for editbox KillFocus maped to reasion SDS_CBR_LOST_FOCUS
*	in the callback function
*
*
*
*/

void EditBox::OnKillfocus() 
	{
	// TODO: Add your control notification handler code here

	ProcessCALLBACKFunction(SDS_CBR_LOST_FOCUS);

	}

/************************************************************************************
/* + int EditBox:: CreateOnScreenObj (CWnd* pParen - 
*
*	creates the windows UI control
*	
*
*
*
*/

int EditBox:: CreateOnScreenObject (
									CWnd* pParent	// i:pointer to the parent window
									)
	{
	int error;

	DWORD dwStyle =  WS_CHILD | WS_VISIBLE | WS_BORDER | WS_GROUP | ES_AUTOHSCROLL | ES_MULTILINE | ES_WANTRETURN;
	
	DWORD dwStyleLabel = WS_CHILD|WS_VISIBLE;
	int editwidth ;
	
	CString Password = m_attriblist->get_PasswordChar();
	// check for property Password if set set style ES_PASSWORDS if property is not set it will return "ALT+Home " 
	// Don't use right close double quote mark  EBATECH 
	if(Password != CString("") && Password.IsEmpty() == FALSE )
	{
		dwStyle = dwStyle|ES_PASSWORD;
	}

	if (!m_attriblist->IsEnabled())
		{
		// set style to enable
		dwStyle = dwStyle|WS_DISABLED;
		dwStyleLabel = dwStyleLabel|WS_DISABLED;
		}

	if (m_attriblist->IsTabstop())
		{
		// set style to tabstop
		dwStyle = dwStyle|WS_TABSTOP;
		}
	if(!m_attriblist->IsAllowAccept())
		{
		dwStyle = dwStyle;
		}

	CRect rectedit, rectstatic ; 
	// EBATECH(CNBR) 2001-05-05 Fontface needs localize. -[
	CString wszResourceString;
	wszResourceString.LoadString(ID_DCL_DEFAULT_FONTFACE);   // Load the resource string.
	m_ComponentFont->CreatePointFont(DIALOG_FONT_POINTSIZE,wszResourceString,NULL); // create font
	//m_ComponentFont->CreatePointFont(80,_T("MS Sans Serif"),NULL); // create font
	// EBATECH(CNBR) ]-
	rectstatic = rectedit = PositionControl(); // get actual diaplay position of control
	
	if(!m_attriblist->get_Label().IsEmpty())	// if lable is given then add Cstatic control along with edit control
		{
		// calculate position of static control
		CSize size;
		CDC dc;
		dc.CreateIC ("DISPLAY",NULL,NULL,NULL);
		size = dc.GetOutputTextExtent(m_attriblist->get_Label());
		int static_width = 	size.cx+8; //(m_attriblist->get_Label().GetLength()) * (LOWORD(GetDialogBaseUnits()-1.1))  ;
		rectstatic.right = rectstatic.left + static_width;
		rectedit.left = rectstatic.right ;
		CString Label =SetMnemonic() ;
		rectstatic.top += 4;
		error = n_Title.Create(Label,dwStyleLabel, rectstatic, pParent); // create static UI control 
		if (error)
			n_Title.SetFont(m_ComponentFont,TRUE);		// set font
		editwidth= m_attriblist->get_editWidth () * 6 +12;
		// if edit width is given the edit control will be aligned from right with width equal to edit width
		if(!(m_attriblist->IsFromDefault ()))
			{
			rectedit.left = rectedit.right - editwidth;
			}
	
		}
	else
		{
		// label is not given but edit width is given 
		editwidth= m_attriblist->get_editWidth () * 6  + 12;
		// if edit width is given the edit control will be aligned from right with width equal to edit width
		if(!m_attriblist->IsFromDefault ())
			{
			rectedit.left = rectedit.right - editwidth;
			}

		}
	

	
	error = Create(dwStyle, rectedit, pParent, m_controlID.GetID());	// create ui edit control 
	
	if (error)
		{
		// Set the Password Attribute
		if(Password != CString("") && Password.IsEmpty() == FALSE )
		{
			TCHAR passChar;
			// set the password char if not given the default is "*"
			if(Password.IsEmpty ())
				passChar = '*';
			else
				passChar = Password[0];
			SetPasswordChar (passChar); // this MFC function sets the password char
		}
		SetWindowText(m_attriblist->get_ValueString());	//set value of edit control
		SetFont(m_ComponentFont,TRUE);					// set font for edit control
		// set the edit limit
		int editlimit =m_attriblist->get_editLimit ();
		if(editlimit>0)
			{
			SetLimitText(editlimit); // this MFC funcction sets the edit limit
			}
		else
			{
			 //SetLimitText(0);
			g_Main.lpSubClassProc = (WNDPROC)SetWindowLong(m_hWnd,GWL_WNDPROC, (LONG)SubEditWndProc);

			}
		
		return SDS_RTNORM;	
		}
	else
		return SDS_RSERR;
	}

/************************************************************************************
/* + int EditBox::ModeTile (  int nTileState ) 
*
*	Executes functions like setting focus to a control , enabling or disabling a control
*	nTileState = 0  then enable window
*	nTileState = 1	disable window
*	nTileState = 2  set Focus
*
*
*/

int EditBox::ModeTile ( // Executes functions like setting focus to a control , enabling or disabling a control etc.
		int nTileState	   // i:state of control
		)
	{
	CWnd * pWind;
	pWind = (CWnd *)(this);

	switch(nTileState)
		{
		case SDS_MODE_ENABLE:
//MB 12-30-1999
			if ( n_Title ) n_Title.EnableWindow(); 

			pWind->EnableWindow();
			break;
		case SDS_MODE_DISABLE:
//MB 12-30-1999
			if ( n_Title ) n_Title.EnableWindow(FALSE); 

			pWind->EnableWindow(FALSE);
			break;
		case SDS_MODE_SETFOCUS:
			pWind->SetFocus();
			break;
		case SDS_MODE_SETSEL:
			pWind->SetFocus();
			SendMessage(EM_SETSEL,0,-1);
			break;
		default:
			TRACE ("WRONG PARAMETER IN MODE TILE");
			break;
		}
	return SDS_RTNORM;
	}


/************************************************************************************
/* + int EditBox:: GetTile ( CString & szControlInfo , int lInfoMaxLength)
*
*	Returns the "value" attribute of the tile
*
*
*
*/

int EditBox:: GetTile (		// Returns the "value" attribute of the tile
		CString & szControlInfo,// o:Date contaned in value attribute
		int lInfoMaxLength	// o: Size of data
		)
	{
	//  if size of data requested is greater then 0 
	//  then get the text in the edit control and return
	if(lInfoMaxLength>0)
		{
		GetWindowText(szControlInfo) ;
		return SDS_RTNORM ; 
		}
	return SDS_RTERROR ; 
	}

/************************************************************************************
/* + int EditBox::SetTile (	CString szControlKey,CString szControlInfo)
*
*	Sets the "value" attribute of the tile
*
*
*
*/

int EditBox::SetTile (		// Sets the "value" attribute of the tile
		CString szControlKey,	// i:Key of the tile
		CString szControlInfo	// i:Data to store in the value attribute
		)
	{
	// set the text of edit control 
	SetWindowText(szControlInfo);
	return SDS_RTNORM;
	}

/************************************************************************************
/* + int EditBox ::InitFocus - 
*
*	this function is usedto set the porperty initial_focus of dialog
*
*
*
*/

int EditBox ::InitFocus()
	{
	// SET THE FOCUS ON THE WINDOW
	SetFocus();
	return SDS_RTNORM;
	}



void EditBox::OnSetfocus() 
	{
	// TODO: Add your control notification handler code here


    if ( this->m_attriblist->IsAllowAccept() == false )
		{
		Dialog * pParent = static_cast<Dialog *>(GetParent());
		pParent->GetDefID();
		pParent->ToggleDefaultID(FALSE);
		}
        
	
	}


/********************************************************************************
 * Author:	Sachin Dange.
 *
 * Purpose:	To map the WM_CHAR message for the Edit Box. We are checking for the 
 *			ENTER key in this function.
 *
 * Returns:	void
 ********************************************************************************/
void EditBox::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_RETURN)
	{
		ProcessCALLBACKFunction(SDS_CBR_SELECT);
	}

	// call the base class version.
	CEdit::OnChar(nChar, nRepCnt, nFlags);

}


/********************************************************************************
 * Author:	Sachin Dange.
 *
 * Purpose:	To Call the CallBack Function mapped against the "action_tile" 
 *			function of the Edit Box control.
 *
 * Parameter: reason -> int.
 *				This parameter is one of the following values for the 
 *				"edit_box" tile:
 *					--> SDS_CBR_SELECT.
 *					--> SDS_CBR_LOST_FOCUS.
 *
 * Returns:	void
 ********************************************************************************/
void EditBox::ProcessCALLBACKFunction (int reason)
{
	Dialog * pParent = static_cast<Dialog *>(GetParent());
	
	static int session=0;

	if (session==0)
	{
		session=1;
		if ( this->m_attriblist->IsAllowAccept() )
		{
			pParent->ToggleDefaultID(TRUE);
		}
		
		// check if the dialog box is modal if modal then only the call back function will be called
		if(pParent->m_IsModal )
		{
			try
			{
				sds_callback_packet *cpkt = NULL;
				cpkt = GetCallBackPacket(); // this function creates an default call back packet
				cpkt->dialog = ((Dialog*)GetParent())->handle;
				cpkt->reason = reason;
				char szValue [256] ;
				GetWindowText(szValue,255);
				cpkt->value = szValue;
				ScreenObject *pScreen =(ScreenObject *)this;
				CString strAction = pScreen->m_attriblist->get_Action();
				
				// call the call back function if call back is set
				if(m_FunctionPtr != NULL)
					m_FunctionPtr(cpkt);
				// call the call back function set in the dcl file if no call back is set using lisp.
				else if ( strAction.IsEmpty() == FALSE )
				{
					pParent->m_DefCallBack(cpkt);
				}
				// if callback is not set in either dcl or lisp use the default set in lisp .
				else if(pParent->m_DefCallBack != NULL)
					pParent->m_DefCallBack(cpkt);

				// delete the callback packet
				delete cpkt->tile;
				delete cpkt;
			}
			catch (...)
			{
				TRACE ("FAILED IN CALL BACK FOR EDIT");
				return;
			}
		}
		
		session=0;
	}

}

