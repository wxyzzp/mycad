/* D:\DEV\PRJ\ICAD\ICADACCELEDIT.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 
// THIS FILE HAS BEEN GLOBALIZED!

// CIcadAccelEdit.cpp : implementation file
//

#include "Icad.h"/*DNT*/
#include "IcadAccelEdit.h"/*DNT*/
#include "IcadCusKeyboard.h"/*DNT*/

#ifdef _DEBUG
	#if !defined( USE_SMARTHEAP )
	#define new DEBUG_NEW
	#endif

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIcadAccelEdit

CIcadAccelEdit::CIcadAccelEdit()
{
}

CIcadAccelEdit::~CIcadAccelEdit()
{
}


BEGIN_MESSAGE_MAP(CIcadAccelEdit, CEdit)
	//{{AFX_MSG_MAP(CIcadAccelEdit)
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_KEYUP()
	ON_CONTROL_REFLECT(EN_SETFOCUS, OnSetfocus)
	ON_WM_SYSKEYDOWN()
	ON_WM_SYSCHAR()
	ON_WM_SYSKEYUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIcadAccelEdit message handlers

void CIcadAccelEdit::OnSetfocus() 
{
	m_cKeyDownBits=0;	
}

void CIcadAccelEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	//*** Return if the previous key state bit is set to 1 (the key is down before the
	//*** call).
	if(nFlags & 0x00004000) return;
	if(nChar==VK_SHIFT) m_cKeyDownBits|=ICAD_ACCEL_SHIFTBIT;
	else if(nChar==VK_CONTROL) m_cKeyDownBits|=ICAD_ACCEL_CTRLBIT;
	else if(nChar==VK_MENU)	m_cKeyDownBits|=ICAD_ACCEL_ALTBIT;
	else CreateString(nChar);
}

void CIcadAccelEdit::CreateString(UINT nChar) 
{
	CListBox* pAccelListBox = (CListBox*)m_pParentDlg->GetDlgItem(CUS_KEYBOARD_LISTDEFKEYS);
	//*** If there is already something in the accelerator editbox, assume that the
	//*** item was previously added to the listbox by this function and delete it.
	CString strText;
	GetWindowText(strText);
	if(!strText.IsEmpty()) 
	{
		((CIcadCusKeyboard*)m_pParentDlg)->DeleteAccelListItem(pAccelListBox->GetCurSel());
	}
	//*** Convert the current keys down to a string.
	ACCEL accel;
	CString strAccel;
	accel.fVirt=FVIRTKEY;
	if(m_cKeyDownBits & ICAD_ACCEL_ALTBIT) accel.fVirt|=FALT;
	if(m_cKeyDownBits & ICAD_ACCEL_CTRLBIT) accel.fVirt|=FCONTROL;
	if(m_cKeyDownBits & ICAD_ACCEL_SHIFTBIT) accel.fVirt|=FSHIFT;
	accel.key=nChar;
	accel.cmd=0;
	((CIcadCusKeyboard*)m_pParentDlg)->AccelToString(&accel,strAccel);
	//*** Using the following two function calls instead of SetWindowText() will
	//*** move the caret to the end of the string.
	SetSel(0,-1);
	ReplaceSel(strAccel);
	if(strAccel.IsEmpty()) return;
	//*** Try to find a match that already exists in the list.
	CPtrList* pAccelList = SDS_CMainWindow->m_pAccelList;
	int nItems = pAccelListBox->GetCount();
	ICACCEL* picAccel;
	int ct;
	for(ct=0; ct<nItems; ct++)
	{
		picAccel = (ICACCEL*)pAccelListBox->GetItemDataPtr(ct);
		if(picAccel==NULL) continue;
		if(picAccel->accel.fVirt==accel.fVirt && picAccel->accel.key==accel.key) break;
		picAccel=NULL;
	}
	if(ct<nItems)
	{
		//*** Found a match, highlight it in the list and return.
		pAccelListBox->SetCurSel(ct);
		((CIcadCusKeyboard*)m_pParentDlg)->OnSelChangeDefKeysList();
		return;
	}
	//*** No match, add it to the list.
	picAccel = new ICACCEL;
	memset(picAccel,0,sizeof(ICACCEL));
	picAccel->accel.fVirt=accel.fVirt;
	picAccel->accel.key=accel.key;
	pAccelList->AddTail(picAccel);
	int idxNewItem = pAccelListBox->AddString(strAccel);
	pAccelListBox->SetItemDataPtr(idxNewItem,picAccel);
	pAccelListBox->SetCurSel(idxNewItem);
	//*** Set the current accelerator pointer in the parent dialog.
	((CIcadCusKeyboard*)m_pParentDlg)->m_pCurAccel=picAccel;
	//*** Enable the Delete button.
	(m_pParentDlg->GetDlgItem(CUS_KEYBOARD_BTNDELETE))->EnableWindow();
	//*** Enable the Add Command button because that should be the next step.
	(m_pParentDlg->GetDlgItem(CUS_KEYBOARD_BTNADDCMD))->EnableWindow();
}

void CIcadAccelEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	//*** Override this function because I am dealing with all the keyboard input.
	return;
}

void CIcadAccelEdit::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if(nChar==VK_SHIFT) m_cKeyDownBits&=(~ICAD_ACCEL_SHIFTBIT);
	else if(nChar==VK_CONTROL) m_cKeyDownBits&=(~ICAD_ACCEL_CTRLBIT);
	else if(nChar==VK_MENU)	m_cKeyDownBits&=(~ICAD_ACCEL_ALTBIT);
}

void CIcadAccelEdit::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	OnKeyDown(nChar,nRepCnt,nFlags);
}

void CIcadAccelEdit::OnSysChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	OnChar(nChar,nRepCnt,nFlags);
}

void CIcadAccelEdit::OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	OnKeyUp(nChar,nRepCnt,nFlags);
}


