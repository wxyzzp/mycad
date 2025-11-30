/* PRJ\ICAD\INPUTEDITBOXDLG.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * $Revision: 1.2 $ $Date: 2001/07/15 09:05:06 $ 
 * 
 * Abstract
 * 
 * Implements a generic edit box in a dialog that can be displayed to
 * obtain a value from the user.
 *
 * The function allows you to supply the dialog title, the edit box label,
 * and a validation object.
 *
 * Due to time constraints, the dialog class only deals with a CString;
 * but it could easily be extended to deal with ints, longs, sds_reals,
 * etc.
 * 
 */ 


#include "icad.h"				/*DNT*/
#include "ValueValidator.h"		/*DNT*/
#include "InputEditBoxDlg.h"	/*DNT*/

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CInputEditBoxDlg construction/destruction

/******************************************************************************
*+ CInputEditBoxDlg::CInputEditBoxDlg - standard constructor
*
*/

CInputEditBoxDlg::CInputEditBoxDlg(
	CWnd* pParent /*=NULL*/)					// i - pointer to the parent window
	: CDialog(CInputEditBoxDlg::IDD, pParent)
	{

	// allow the wizard to do any initialization it wants to do
	//{{AFX_DATA_INIT(CInputEditBoxDlg)
	//}}AFX_DATA_INIT

	// initialize everything to what we really want
	m_title		= ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD");
	m_label		= ResourceString(IDC_CMDS9_VALUE__330, "Value:");
	m_value		= ""/*DNT*/;

	m_validator	= NULL;
	}



/////////////////////////////////////////////////////////////////////////////
// CInputEditBoxDlg setters/getters

/******************************************************************************
*+ CInputEditBoxDlg::SetTitle - set the title of the dialog
*
*/

void CInputEditBoxDlg::SetTitle(
	const CString& title)						// i - title of the dialog
	{
	m_title = title;
	}

/******************************************************************************
*+ CInputEditBoxDlg::SetLabel - set the label for the edit box
*
*/

void CInputEditBoxDlg::SetLabel(
	const CString& label)						// i - label of the edit box
	{
	m_label = label;
	}

/******************************************************************************
*+ CInputEditBoxDlg::SetValue - set the value for the edit box
*
*/

void CInputEditBoxDlg::SetValue(
	const CString& value)						// i - value in the edit box
	{
	m_value = value;
	}

/******************************************************************************
*+ CInputEditBoxDlg::SetValidator - set the validation object
*
*/

void CInputEditBoxDlg::SetValidator(
	CValueValidator *validator)					// i - the validator
	{
	m_validator = validator;
	}

/******************************************************************************
*+ CInputEditBoxDlg::SetValidator - set the validation object
*
*/

CString& CInputEditBoxDlg::GetValue(
	void)
	{
	return m_value;
	}



/////////////////////////////////////////////////////////////////////////////
// CInputEditBoxDlg implementation

/******************************************************************************
*+ CInputEditBoxDlg::DoDataExchange - initializes the label and value controls
*
*/

void CInputEditBoxDlg::DoDataExchange(
	CDataExchange* pDX)							// i - pointer to the data exchange object
	{
	CDialog::DoDataExchange(pDX);

	// need to init controls before accessing their member functions
	//{{AFX_DATA_MAP(CInputEditBoxDlg)
	DDX_Control(pDX, INPUT_LABEL, m_labelCtrl);
	DDX_Control(pDX, INPUT_VALUE, m_valueCtrl);
	//}}AFX_DATA_MAP
	}



/////////////////////////////////////////////////////////////////////////////
// CInputEditBoxDlg message handlers

BEGIN_MESSAGE_MAP(CInputEditBoxDlg, CDialog)
	//{{AFX_MSG_MAP(CInputEditBoxDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/******************************************************************************
*+ CInputEditBoxDlg::OnInitDialog - initialize the values for the dialog
*
*/

BOOL CInputEditBoxDlg::OnInitDialog(
	) 
	{
	CDialog::OnInitDialog();
	
	// set the dialog's title and the edit box's label
	this->SetWindowText(m_title);
	m_labelCtrl.SetWindowText(m_label);
	
	// display the current value in the edit box, set focus to it, and select it
	m_valueCtrl.SetWindowText(m_value);
	m_valueCtrl.SetFocus();
	m_valueCtrl.SetSel(0, -1);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	}

/******************************************************************************
*+ CInputEditBoxDlg::OnOK - handles when the OK button is pressed
*
*/

void CInputEditBoxDlg::OnOK() 
	{
	// get the value from the edit box
	m_valueCtrl.GetWindowText(m_value);

	// if there's a validator, validate the value;
	// if the value isn't valid, return to the edit box and select the value
	if (m_validator)
		{
		m_validator->SetValue(m_value);
		if (!m_validator->ValidateValue())
			{
			m_valueCtrl.SetFocus();
			m_valueCtrl.SetSel(0, -1);
			return;
			}
		}
	
	CDialog::OnOK();
	}


