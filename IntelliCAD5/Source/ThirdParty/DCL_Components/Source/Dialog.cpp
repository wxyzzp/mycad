/** FILENAME:     Dialog.cpp- Contains implementation of class CDialog.
* Copyright  (C) Copyright 1998-99.Visio Corporation.  All Rights Reserved.
*
*
*
* Abstract 
*	this class Maps the basic tiles dialog
*	to windows DialogBox
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
#include "resource.h"
#include "sds.h"
#include "dclprivate.h"
#include "Dialog.h"
#include "Main.h"
//Modified SAU 12/07/2000 
//Reason:REGRESSION:  is_cancel attribute is ignored and for Bug Fixed:45448
#include "Button.h"
#include "tree.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern Main g_Main;

/************************************************************************************
/* + Dialog::Dialog(CWnd* pParent)
*
*	standard constructor 
*
*
*
*/

Dialog::Dialog(CWnd* pParent /*=NULL*/)
	: CDialog(Dialog::IDD, pParent),m_IsModal(FALSE),handle(NULL)
	{
	m_DefCallBack = NULL ;
	m_nDoneDlgCalled = 0;
	m_pDiaCancelobj=NULL;
	//{{AFX_DATA_INIT(Dialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	}
/************************************************************************************
/* + Dialog::~Dialog ()
*	Destructor for the dialog class.
*
*
*
*
*/

Dialog::~Dialog()
	{
	if ( handle )
		delete handle;
	}

/************************************************************************************
/* + void Dialog::DoDataExchange(CDataExchange* pDX)
*
*	standard MFC function
*
*
*
*/

void Dialog::DoDataExchange(CDataExchange* pDX)
	{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Dialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	}


BEGIN_MESSAGE_MAP(Dialog, CDialog)
	//{{AFX_MSG_MAP(Dialog)
		// NOTE: the ClassWizard will add message map macros here
		//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/************************************************************************************
/* + int Dialog:: CreateOnScreenObject (CWnd* pParent)
*
*	creates the windows UI control
*	
*
*
*
*/

int Dialog:: CreateOnScreenObject (
								   CWnd* pParent // i:pointer to the parent window
								   )
	{
	int error;
	CString label;
	CWnd *pWnd =NULL;
	// code to run only if DLL
	// creater a rapper object for the main window of ICad
	#ifndef _TESTEXE

	pWnd = CWnd::FromHandle(g_Main.hParentHwnd);   
	#endif

	error = Create(IDD_DIALOGTEMPLATE,pWnd); // creates the dialog in modless state
	label = m_attriblist->get_Label();
	SetWindowText(label); // sets the title of the dialog
	
	if (error)
		return SDS_RTNORM;	
	else
		return SDS_RSERR;
	}
/************************************************************************************
/* + CWnd* Dialog::GetWindow()
*
*	returns the CWnd pointer for the screen object
*	
*
*
*/

CWnd* Dialog::GetWindow()
	{
	return ((CWnd*)this);
	}
/************************************************************************************
/* + int Dialog::set_ModalResult(int iValue)
*	Set the variable if dialog was terminated by mechanisms 	
*	other than "Pressing Cancel"
*
*
*
*/
int Dialog::set_ModalResult(int iValue)
	{
	m_nDoneDlgCalled = iValue;
	return SDS_RTNORM;
	}
/************************************************************************************
/* + int Dialog::get_ModalResult()
*	Read the value of m_nDoneDlgCalled.
*
*
*
*
*/

int Dialog::get_ModalResult()
	{
	return m_nDoneDlgCalled;
	}



/************************************************************************************
/* + int ToggleDefaultID(            // This function is used to toggle for allow_accept.   - 
*
*
*  This function is used to toggle for allow_accept.  
*
*
*/
int Dialog::ToggleDefaultID(            // This function is used to toggle for allow_accept.  
	               BOOL bSet    // i: TRUE: Set FALSE  : Reset 
				   )  

	{

	if ( bSet == TRUE )
		{
		SetDefID( m_nDefBtnID );
		}
	else
		{
		m_nDefBtnID = GetDefID();
		SetDefID(-1);
		}


	return SDS_RTNORM;

		
	}
//Modified SAU 12/07/2000 [
//Reason:REGRESSION:  is_cancel attribute is ignored and for Bug Fixed:45448
/***************************************************************************
/*void Set_DiaCancelobj(ScreenObject *DiaCancelobj)
* This function is used to set which button(screen object) is pressed
* when the user presses the cancel key (ESC). 
*/
void Dialog::Set_DiaCancelobj(ScreenObject *DiaCancelobj)
{
   m_pDiaCancelobj=DiaCancelobj;
}
/***************************************************************************
 void Dialog::OnCancel()
*This function is used to handle End dialog action when user press 
*Escape key
*/
void Dialog::OnCancel()
{
	if(m_pDiaCancelobj)
	{
		if(!m_nDoneDlgCalled)
		{
			/*DG - 6.2.2003*/// I added this try/catch because icad crashed sometimes during the following sending the BM_CLICK message
			// (it crashed before Button::OnClicked in windows/mfc code).
			/*DG - 11.2.2003*/// The problem was m_pDiaCancelobj wasn't valid, I fixed it (see main.cpp)
			// but I'm leaving this try/catch for safety.
			try
			{
				m_pDiaCancelobj->GetWindow()->SendMessage(BM_CLICK, 0, 0);
			}
			catch(...)
			{
				ASSERT(0);	// something wrong!
			}
		}
		// Modified AP CyberAge 10/04/2000
		//EndDialog(IDCANCEL);
		EndDialog(0);
	}
	
}

//Modified SAU 12/07/2000 ]
/****************************************************************************/
//MB 14-1-2000
int Dialog::SetTile (		// Sets the "value" attribute of the tile
		CString szControlKey,	// i:Key of the tile
		CString szControlInfo	// i:Data to store in the value attribute
		)
	{
	SetWindowText( szControlInfo );
	return SDS_RTNORM;
	}









