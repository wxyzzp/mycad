/** FILENAME:     Dialog.h- Contains declarations of class CDialog.
* Copyright  (C) Copyright 1998-99 Visio Corporation.  All Rights Reserved.
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

#if !defined _DIALOG_H_
#define _DIALOG_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "ScreenObject.h"
#include "resource.h"


class Dialog : public CDialog ,public ScreenObject
{

	// Operations derived from base class ScreenObject
public:	
	virtual int CreateOnScreenObject ( // Creates the UI objects
		CWnd* pParent				// i:pointer to the parent window
		) ; 

	int ToggleDefaultID(			// This function is used to toggle for allow_accept.  
					   BOOL bSet	// i: TRUE: Set FALSE  : Reset 
					   );
	//Modified SAU 12/07/2000 [
	//Reason:REGRESSION:  is_cancel attribute is ignored and for Bug Fixed:45448
	void Set_DiaCancelobj(			//This function is used to set which button(screen object) is pressed
					   ScreenObject *DiaCancelobj //when the user presses the cancel key (ESC). 
					   );			
	
	void OnCancel();			//This function is used to handle End dialog action when user press 
								//Escape key
	//Modified SAU 12/07/2000 ]
	
//MB 14-1-2000
	virtual int SetTile (		// Sets the "value" attribute of the tile
		CString szControlKey,	// i:Key of the tile
		CString szControlInfo	// i:Data to store in the value attribute
		);

public:
	virtual ~Dialog();				// Destructor.
	int get_ModalResult();			// Get the m_nDoneDlgCalled variable. 
	int set_ModalResult(int iValue);// Set this variable if dialog did not terminate by pressing Cancel.
	CLIENTFUNC m_DefCallBack;		// the default callback function pointer
	CString DialogID;				// dialog ID
	_sds_hdlg* handle;				// store the pointer initialised in new_dialog,new_positioned_dialog()
									// so that the same will be passed in the callback packet.
	Dialog(CWnd* pParent = NULL);   // standard constructor
	virtual CWnd* GetWindow();		// returns the CWnd pointer for the screen object
	int m_IsModal;					// flag to indecate the state of dialog (modal or modless)
    
// Dialog Data
	//{{AFX_DATA(Dialog)
	enum { IDD = IDD_DIALOGTEMPLATE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Dialog)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Dialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	UINT m_nDefBtnID;               // Value used to store the default button ID. 
	int m_nDoneDlgCalled;			// Variable to decide how the dialog terminated.
	//Modified SAU 12/07/2000 
	//Reason:REGRESSION:  is_cancel attribute is ignored and for Bug Fixed:45448
	ScreenObject* m_pDiaCancelobj;; // value used to store screen object for is_cancel
};


#endif 
