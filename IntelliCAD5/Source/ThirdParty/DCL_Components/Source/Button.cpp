/** FILENAME:     Button.cpp- Contains implementation of class CButton.
* Copyright  (C) Copyright 1998-99  Visio Corporation.  All Rights Reserved.
*
*
*
* Abstract 
*		this class Maps the basic tiles (Button , RadioButton, Toggle)	
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
#include "Button.h"
#include "dclprivate.h"
#include "sds.h"
#include "Constants.h"
#include "Dialog.h"
#include "main.h"

#include "ItcLanguage.h" //EBATECH

extern Main g_Main;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/************************************************************************************
/* + Button::Button - 
*
*	constructor
*
*
*
*/
Button::Button()
	{
	
	}

/************************************************************************************
/* + Button::Button(int typ , int ParentAlignment)
*
*	overloaded constructor which takes the type of control 
*	
*
*
*/

Button::Button(
			   int type,			// i:type of basic tile (button , radoibutton , toggle)
			   int ParentAlignment	// i:parent alignment
			   ):m_bType(type)
	{
	m_ParentAlignment = ParentAlignment;

	
	}

/************************************************************************************
/* + Button::~Button - 
*
*	Destructor
*
*
*
*/

Button::~Button()
	{
	}




BEGIN_MESSAGE_MAP(Button, CButton)
	//{{AFX_MSG_MAP(Button)
	ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
	ON_WM_GETDLGCODE()
	ON_WM_KEYDOWN()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/************************************************************************************
/* + int Button:: CreateOnScreenObj (CWnd* pParen )- 
*	creates the windows UI control
*	
*
*
*
*/

int Button:: CreateOnScreenObject(
								  CWnd* pParent //i:pointer to the parent window
								  )
	{
	int error;
	DWORD dwStyle = WS_CHILD|WS_VISIBLE;		// variable used to set the control style
	ValueString = m_attriblist->get_ValueString();
	if (m_bType == PUSHBUTTON)					// check for type of button if push button
		{

			dwStyle = dwStyle|WS_GROUP;
		if (m_attriblist->IsDefault())
			{
			// set style to default
			dwStyle = dwStyle | BS_DEFPUSHBUTTON;
			}

		else
			{
			dwStyle= dwStyle | BS_PUSHBUTTON;
			}

		if (!m_attriblist->IsEnabled())
			{
			// set style to enable
			dwStyle = dwStyle | WS_DISABLED;
			}

		if (m_attriblist->IsTabstop())
			{
			// set style to tabstop
			dwStyle = dwStyle | WS_TABSTOP;
			}
		
		}

	else if (m_bType == RADIOBUTTON)	// check for type of button if radio
		{
		dwStyle = dwStyle | BS_AUTORADIOBUTTON;
		if (!m_attriblist->IsEnabled())
			{
			// set style to enable
			dwStyle = dwStyle | WS_DISABLED;
			}

		
		if ( m_nIsFirstRadioBtn == 1 )
			{
			// The tab stop property is added here to tab
			// to a radio button, when non of the buttons in the group
			// are selected. 
			dwStyle = dwStyle | WS_GROUP | WS_TABSTOP ;
			}
		
		}

	else if(m_bType == CHECKBOX)		// check for type of button if check box
		{
		dwStyle = dwStyle | BS_AUTOCHECKBOX|WS_GROUP;
		if(!m_attriblist->IsEnabled())
			{
			// set style to enable
			dwStyle = dwStyle|WS_DISABLED;
			}

		if(m_attriblist->IsTabstop())
			{
			// set style to tabstop
			dwStyle = dwStyle | WS_TABSTOP;
			}
		}

	CRect rect ;									// Rect variable to hold the actual display rect of the control
	rect = PositionControl();						// function to calculate the actual display rect

	CString Label = SetMnemonic() ;				// function to set the Mnemonic (it adds an "&" in the label to the specified char)
	UINT nID = m_controlID.GetID();
	error = Create(Label, dwStyle, rect, pParent,nID);	// creates the UI contol


	
	if (error)
		{
		// EBATECH(CNBR) 2001-05-05 Fontface needs localize. -[
		CString wszResourceString;
		wszResourceString.LoadString(ID_DCL_DEFAULT_FONTFACE);   // Load the resource string.
		m_ComponentFont->CreatePointFont(DIALOG_FONT_POINTSIZE,wszResourceString,NULL);	// create font
		//m_ComponentFont->CreatePointFont(80,_T("MS Sans Serif"),NULL);	// create font
		// EBATECH(CNBR) ]-
		((CWnd*)this)->SetFont(m_ComponentFont,TRUE);						// set font to the control
		if (m_bType == PUSHBUTTON)	
			{
			// if the button is default button then the focus is on it
			if (m_attriblist->IsDefault())
				{
				SetFocus();
				Dialog* pDialog = static_cast<Dialog*>(GetParent());
				ASSERT( pDialog );
				pDialog->SetDefID(nID);		
				}
			if ( m_attriblist->IsDefault() )// || m_attriblist->IsCanceled() )
				{

				int nRet = m_attriblist->FindAttribute(CString(_T("action")));
				if ( nRet == SDS_RTERROR )
				{ 
					//TileAttributes* pAttribute = new TileAttributes(_T("action"),_T("s"),_T("done_dialog"));
					m_attriblist->AddAttribute(_T("action"),_T("s"),_T("(done_dialog 1)"));
					}
				}
			if ( m_attriblist->IsCanceled() )// || m_attriblist->IsCanceled() )
				{

				int nRet = m_attriblist->FindAttribute(CString(_T("action")));
				if ( nRet == SDS_RTERROR )
					{
					//TileAttributes* pAttribute = new TileAttributes(_T("action"),_T("s"),_T("done_dialog"));
					// m_attriblist->AddAttribute(_T("action"),_T("s"),_T("(done_dialog 0)"));
					}
					//Modified SAU 11/07/2000 [
					else if( m_attriblist->get_Action()==CString("(done_dialog 0)"))
                    m_attriblist->AddAttribute(_T("action"),_T("s"),_T(""));    
					//Modified SAU 11/07/2000 ]	
				}
			}
		else if (m_bType == CHECKBOX)
			{
			if( m_attriblist->get_ValueString() == CString("1") || m_nCheckThisRadioBtn )
				SetCheck (true);
			}
		else
			{
			// set the state of button if it is radoi or check button.
			if( m_attriblist->get_ValueString() == CString("1") || (m_nCheckThisRadioBtn == 1) )
				{
				SetCheck (true);
				}
			}
			
		return SDS_RTNORM;	
		}

	else
		return SDS_RSERR;

	}
//Modified SAU 11/07/2000 [
//Reason:REGRESSION:  is_cancel attribute is ignored and for Bug Fixed:45448
/************************************************************************************
/* + CWnd* Button::GetWindow()
*
*	returns the CWnd pointer for the screen object
*	
*
*
*/

CWnd* Button::GetWindow()
	{
	return ((CWnd*)this);
	}
//Modified SAU 11/07/2000 ]
/************************************************************************************

/* + void Button::OnClicked()
*
*	command handler for button clicked maped to reasion SDS_CBR_SELECT
*	in the callback function
*
*
*/
void Button::OnClicked() 
	{
	// TODO: Add your control notification handler code here

	static BOOL bFireMessage = TRUE;

	if ( bFireMessage == TRUE )
		{ 
		Dialog * pParent = static_cast<Dialog *>(GetParent());
		SetFocus();
		// check if the dialog box is modal if modal then only the call back function will be called
		if ( m_bType == RADIOBUTTON)
			{
			bFireMessage = FALSE;
			SendMessage(BM_CLICK);
			}

		try
			{
			if(pParent->m_IsModal )
				{
				// Code for unchecking a disabled radio button.

				TreeNode *pTreeNode = GetParentNodeOfTile();
				HWND hWnd = pTreeNode->m_Tile.get_ScreenObject()->hRadioToRemoveCheck; 

				if ( hWnd != NULL )
					{
					LRESULT lResult = ::SendMessage(hWnd,BM_SETCHECK,BST_UNCHECKED,0);
					pTreeNode->m_Tile.get_ScreenObject()->hRadioToRemoveCheck = NULL;
					}

				// Code end

				sds_callback_packet *cpkt = NULL;
			
				cpkt = GetCallBackPacket();    
				ScreenObject *pScreen =(ScreenObject *)this;
				CString strAction = m_attriblist->get_Action();
				
				cpkt->dialog = ((Dialog*)GetParent())->handle;
				cpkt->reason = SDS_CBR_SELECT;
				cpkt->value = _T("1");


				if ( m_nActionSetThruParent == 1 )
					{
					// {{{ Gurudev - DT: 8/5/99 6:38:10 PM
					// The value for call back of buttons has been chaged to key of the tile.
					// when action is set thru the parent.
					cpkt->value = (LPTSTR)(LPCTSTR)this->m_attriblist->get_Key() ;
					// Gurudev - Dt:  8/5/99 6:38:16 PM }}}  
					}
				else
					{
					if (m_bType != PUSHBUTTON)
						{	
						char value [5];
						itoa(GetCheck() ,value,5);
						cpkt->value = value;
						}
					}
					// call the call back function if call back is set
				if(m_FunctionPtr != NULL)
					{
						m_FunctionPtr(cpkt);
					}
				// call the call back function set in the dcl file if no call back is set using lisp.
				else if ( strAction.IsEmpty() == FALSE )
					{
//MB 23-12-1999	
					if ( pParent->m_DefCallBack )
						pParent->m_DefCallBack(cpkt);
					//Modified SAU 12/07/2000 [
					//Reason:the system ok_button is no longer being treated as 
					//a default button
					else if(m_attriblist->IsDefault()==1)  
						dlg_done_dialog(pParent->handle,1);  
					//Modified SAU 12/07/2000 ]
					}
				// if callback is not set in either dcl or lisp use the default set in lisp .
				else if(pParent->m_DefCallBack != NULL) 
					pParent->m_DefCallBack(cpkt);
			    //Modified SAU 12/07/2000  [
			    //Reason1:the system cancel_button is no longer being treated as a retirement button
				//Reason2:Escape will close all nested DCL dialogs
				if (this->m_IsCancelBtn==TRUE && cpkt->dialog->p!=0)
				{
					if(pParent->m_DefCallBack != NULL){
						CString prevAction	= this->m_attriblist->get_Action();
						m_attriblist->AddAttribute(_T("action"),_T("s"),_T("(done_dialog 0)"));  
						pParent->m_DefCallBack(cpkt);
						m_attriblist->AddAttribute(_T("action"),_T("s"),prevAction);  
					}
					else 
					dlg_done_dialog(pParent->handle,0); 
				}
                //Modified SAU 12/07/2000   ]  
			      

				// delete the callback packet
				delete cpkt->tile;
				delete cpkt;
				}
				// to stop a modeless dialog
			else
				{
				if ( pParent )
					{
					#ifndef _TESTEXE
						dlg_done_dialog(pParent->handle,0);
					#endif
					pParent->DestroyWindow();
					}
				}
			}
		catch(...)
			{
			DWORD dw = ::GetLastError();
			TRACE("?????????DWORD%d",dw);
			}
		}
	else
		{
		bFireMessage = TRUE;
		} 

	}
	


/************************************************************************************
/* + virtual int GetTile (CString & szControlInfo , int lInfoMaxLength)	
*
*	// Returns the "value" attribute of the tile
*
*
*
*/

int Button::GetTile (		     // Returns the "value" attribute of the tile
		CString & szControlInfo, // o:Date contaned in value attribute
		int lInfoMaxLength)		 // o: Size of data
	{
	if (m_bType == PUSHBUTTON)					// check gor type of button
		{
		szControlInfo = ValueString;
		}
	else 
		{
//		int state =	GetState();
		int state = GetCheck();
		if(state ==1)
			{
			szControlInfo = CString("1");
			}
		else
			{
			szControlInfo = CString("0");
			}
		}

		return SDS_RTNORM;	
	}
		

/************************************************************************************
/* + int Button::ModeTile (int nTileState) 
*
*	Executes functions like setting focus to a control , enabling or disabling a control 
*	nTileState = 0  then enable window
*	nTileState = 1	disable window
*	nTileState = 2  set Focus
*/

int Button::ModeTile ( // Executes functions like setting focus to a control , enabling or disabling a control etc.
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
			if ( m_bType == RADIOBUTTON && this->GetCheck() == 1 )
				{
				TreeNode *pTreeNode = GetParentNodeOfTile();
				pTreeNode->m_Tile.get_ScreenObject()->hRadioToRemoveCheck = this->m_hWnd ;
				}
				
			pWind->EnableWindow(FALSE);
			break;
		case SDS_MODE_SETFOCUS:
			pWind->SetFocus();
			if(m_bType != PUSHBUTTON)
				SetCheck(1);
			break;
		default:
			TRACE ("WRONG PARAMETER IN MODE TILE");
			break;
		}
	return SDS_RTNORM;
	}

/************************************************************************************
/* + int Button::InitFocus() 
*	this function is usedto set the porperty initial_focus of dialog - 
*
*
*
*
*/

int Button::InitFocus() 
	{
	// SET THE FOCUS ON THE WINDOW
	SetFocus();
	return SDS_RTNORM;
	}

/************************************************************************************
/* + int ScreenObject ::SetTile (CString szControlKey,CString szControlInfo)	
*
*	Sets the "value" attribute of the tile
*
*
*
*/

int Button ::SetTile (		// Sets the "value" attribute of the tile
		CString szControlKey,	// i:Key of the tile
		CString szControlInfo	// i:Data to store in the value attribute
		)
	{
	if (m_bType == PUSHBUTTON)					// check gor type of button
		{
		ValueString =szControlInfo ;
		}
	else 
		{
		if(szControlInfo  == "1")
			{
			// {{{ Gurudev - DT: 8/5/99 5:04:52 PM
			UnCheckAllSiblingRadioBtn();
			SetCheck(1);
			//Gurudev - Dt:  8/5/99 5:04:55 PM }}} 
				
			}
		else if(szControlInfo  == "0")
			{
			SetCheck(0);
			}
		}
	return SDS_RTNORM;
	}

/************************************************************************************
/* + UINT Button::OnGetDlgCode( ) 
*	Handle the arrow keys within the application.
*
*
*
*
*/

UINT Button::OnGetDlgCode() 
	{
	// TODO: Add your message handler code here and/or call default	
	//return CButton::OnGetDlgCode();
	DWORD dStyle = GetStyle();
	if ( m_bType == RADIOBUTTON )
		{
		return DLGC_BUTTON|DLGC_RADIOBUTTON|DLGC_WANTARROWS;
		}
	if ( m_bType == PUSHBUTTON )
		{
		if ( (dStyle & BS_DEFPUSHBUTTON) == BS_DEFPUSHBUTTON)
			{
			return DLGC_BUTTON|DLGC_DEFPUSHBUTTON;
			}
		else
			{
			return DLGC_BUTTON|DLGC_UNDEFPUSHBUTTON;
			}
		}
	else
		{
		return DLGC_BUTTON;
		}

	}
/************************************************************************************
/* + void Button::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags )
*	To move in a cyclic fashion in a group of controls.
*
*
*
*
*/

void Button::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
	{
	// TODO: Add your message handler code here and/or call default
	
	HWND hParent = GetParent()->m_hWnd;
	ASSERT(hParent);
	
	// TODO: Add your message handler code here and/or call default
	if ( (nChar == VK_UP) || (nChar == VK_LEFT) )
		{
		HWND hwndNext = ::GetNextDlgGroupItem(hParent,m_hWnd,TRUE);
		::SendMessage(hwndNext,BM_CLICK,0,0);
		}
	else if ( (nChar == VK_DOWN) || (nChar == VK_RIGHT))
		{
		HWND hwndNext = ::GetNextDlgGroupItem(hParent,m_hWnd,FALSE);
		::SendMessage(hwndNext,BM_CLICK,0,0);
		}	
	CButton::OnKeyDown(nChar, nRepCnt, nFlags);
	}
/************************************************************************************
/* + void Button::OnSetFocus(CWnd* pOldWnd)
*	Put a check in a radio_button if its value is set to 1.
*
*
*
*
*/

void Button::OnSetFocus(CWnd* pOldWnd) 
	{
	
		CButton::OnSetFocus(pOldWnd);
		
		
	// TODO: Add your message handler code here

	}








/************************************************************************************
/* + int Button::UnCheckAllSiblingRadioBtn - 
*	Loop and Uncheck all the radio buttons in this parent container.
*
*
*
*
*/
 
int Button::UnCheckAllSiblingRadioBtn()    // UnCheck all the radio buttons.
	{
	ScreenObject* pScreenObject = (ScreenObject*)this;
	ASSERT( pScreenObject );
	if ( pScreenObject )
		{
		TreeNode* pParentNode = pScreenObject->GetParentNodeOfTile();
		ASSERT( pParentNode );
		if ( pParentNode )
			{
			for ( int iIndex = 0; iIndex < pParentNode->GetChildCount(); iIndex++ )
				{
				TreeNode* pTempNode = pParentNode->GetChild(iIndex);
				ASSERT( pTempNode );
				if ( pTempNode )
					{
					if ( pTempNode->m_Tile.get_RenderableTileName() == _T("radio_button") )
						{
						ScreenObject* pTempScreen = pTempNode->m_Tile.get_ScreenObject();
						ASSERT( pTempScreen );
						if ( pTempScreen )
							{
							Button* pButton = static_cast<Button*>(pTempScreen);
							ASSERT( pButton );
							pButton->SetCheck(0);
							}
						}
					}

				}
			}
		}

	return SDS_RTNORM;
	}
