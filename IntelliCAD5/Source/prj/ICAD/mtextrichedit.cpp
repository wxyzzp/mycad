/* D:\ITCDEV\PRJ\ICAD\MTEXTRICHEDIT.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

// MTextRichEdit.cpp : implementation file
//
#include "icad.h"
#include "icadapi.h"
#include "MTextRichEdit.h"
#include "MTextPropSheet.h"
#include "gr.h"
#include "DoFont.h"
#include "TTFInfoGetter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CMTextRichEdit
CMTextRichEdit::CMTextRichEdit()
{
}

CMTextRichEdit::~CMTextRichEdit()
{
}

BEGIN_MESSAGE_MAP(CMTextRichEdit, CRichEditCtrl)
	//{{AFX_MSG_MAP(CMTextRichEdit)
	ON_WM_KEYDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYUP()
	// Bugzilla No. 78034; 23-12-2002 
	//ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMTextRichEdit message handlers
void CMTextRichEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CRichEditCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
	((CMTextPropSheet*) GetParent())->UpdateChar();	
}

void CMTextRichEdit::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{	
	CRichEditCtrl::OnKeyUp(nChar, nRepCnt, nFlags);
	if(!((CMTextPropSheet*)GetParent())->m_textTab.m_undo.IsWindowEnabled() && 
		(BOOL)::SendMessage(m_hWnd, EM_CANUNDO, 0, 0))
		((CMTextPropSheet*) GetParent())->m_textTab.m_undo.EnableWindow(TRUE);

	// Bugzilla No. 78437; 30-01-2003
	if ((nChar == VK_INSERT && (GetKeyState(VK_SHIFT) < 0))	// To trap 'SHIFT + INSERT'
		|| (nChar == 'V' && (GetKeyState(VK_CONTROL) < 0)))	// To trap 'CTRL + V'
		((CMTextPropSheet*) GetParent())->OnPaste();
}

void CMTextRichEdit::OnMouseMove(UINT nFlags, CPoint point) 
{	
	CRichEditCtrl::OnMouseMove(nFlags, point);
	if(GetFocus() == this)
		((CMTextPropSheet*)GetParent())->UpdateChar();
}

// Bugzilla No. 78034; 23-12-2002 [
/*void CMTextRichEdit::OnKillFocus(CWnd* pNewWnd) 
{
	((CMTextPropSheet*)GetParent())->UpdateChar();
	CRichEditCtrl::OnKillFocus(pNewWnd);	
}*/
// Bugzilla No. 78034; 23-12-2002 ]

bool CMTextRichEdit::init(SDS_mTextData* pMTextData)
{
	// Increase bytes allowed from 32K default
	LimitText(1048576);
	// Bugzilla No. 78437; 30-01-2003
	SetEventMask(GetEventMask() | ENM_SELCHANGE);

	SetWindowText(pMTextData->text);
	
	CString tmptext, tmpfont;
	bool tmpbold = false, tmpitalic = false;
	double tmpheight;
	short tmpcharset = DEFAULT_CHARSET; // EBATECH(CNBR)
	short tmppitch = 0; // EBATECH(CNBR)
	CHARFORMAT cf;
	GetWindowText(tmptext);
	
	//I edit the text in the richeditctrl by the following: I insert the text into a string,
	//find the location of what I'm looking for, then store the appropriate information in
	//variables.  I then select the formatting information and delete it from the richeditctrl.
	//I then set the charformat as indicated by the variables.  I use GetWindowText occasionally
	//to make sure the two match up.
	SetSel(0, GetTextLength());
	cf.dwMask = CFM_SIZE ;

	//DP: old realization commented
	//cf.yHeight = (long) (m_pMTextData->rTextHeight * 1000);
	ConvertTextHeightToTwips(pMTextData->rTextHeight, cf.yHeight);
	SetSelectionCharFormat(cf);

	CString height;
	height.Format("%.4f", pMTextData->rTextHeight);
	// Bugzilla No. 78034; 23-12-2002 [
	if (((CMTextPropSheet*)GetParent())->GetFontSizeCombo()->FindStringExact(0, height) == -1)
	{
		int index = ((CMTextPropSheet*)GetParent())->GetFontSizeCombo()->AddString(height);
		((CMTextPropSheet *)GetParent())->GetFontSizeCombo()->SetItemData(index,cf.yHeight);
		((CMTextPropSheet*)GetParent())->GetFontSizeCombo()->SetCurSel(index);
	}
	// Bugzilla No. 78034; 23-12-2002 ]

	int erase, eraseend;
	int index = tmptext.Find("\\P", 0);
	while (index != -1)
	{
		SetSel(index, index + 2);
		ReplaceSel("\r\n");
		GetWindowText(tmptext);
		index = tmptext.Find("\\P", 0);
	}
	
	//find all formatting information for fontname, bold, and italics.
	index = tmptext.Find("\\f", 0);
	while (index != -1)
	{
		//store, and delete, the first set of formatting information from the string.
		tmptext.Delete(index, 2);
		int end = tmptext.Find("|", index);
		if (end != -1)
		{
			tmpfont = tmptext.Mid(index, end - index);
			// EBATECH(CNBR) -[ 23/06/2001
			// \fFontname|b0|i0|c128|p34;
			//Modified Cybage AW 30/04/2001[
			//Reason : To avoid the crash when different fonts are used
			//if(tmpfont.Find("TXT.shx") != -1)
			//{
			//	end = tmptext.Find(";", index);
			//}
			//tmpfont = tmptext.Mid(index, end - index);
			//Modified Cybage AW 30/04/2001]

			tmptext.Delete(index, end - index + 2);
			tmpbold = true;
			tmpitalic = true;
			if (tmptext.GetAt(index) == '0')
				tmpbold = false;
			if (tmptext.GetAt(index + 3) == '0')
				tmpitalic = false;
			// EBATECH(CNBR) get charset
			end = tmptext.Find("|c",index);
			if( end != -1 )
				tmpcharset = atoi(tmptext.GetBuffer(0)+(index+end+2));
			else
				tmpcharset = ANSI_CHARSET;
			// EBATECH(CNBR) get pitchandfamily
			end = tmptext.Find("|p",index);
			if( end != -1 )
				tmppitch = atoi(tmptext.GetBuffer(0)+(index+end+2));
			else
				tmppitch = DEFAULT_PITCH | FF_DONTCARE;
			// EBATECH(CNBR) okay to get charset and pitchandfamily
			end = tmptext.Find(";", index);
		}
		else
		{
			end = tmptext.Find(";", index);
			tmpfont = tmptext.Mid(index, end - index);
		}
		if (end == -1)
			break;		// error
		tmptext.Delete(index, end - index + 1);
		
		end = tmptext.Find("\\f", index);
		if (end < index)
			//end = tmptext.GetLength() + 1;
			end = tmptext.GetLength();
		
		//here we erase that which has been erased in tmptext.
		GetWindowText(tmptext);
		erase = tmptext.Find("\\f", 0);
		eraseend = tmptext.Find(";", erase);
		
		//this condition is for the extra inserted space before // in most formatting info.
		//when it is there, I must modify the counters appropriately.
		
		//Modified Cybage AW 30/04/2001[
		//Reason : Bug : The spaces between the words are deleted in the text editor.
		/*
		if (erase != 0 && tmptext.GetAt(erase - 1) == ' ')
		{
		erase--;
		index--;
		end--;
		}
		*/
		//Modified Cybage AW 30/04/2001]
		SetSel(erase, eraseend + 1);
		ReplaceSel("");
		
		//find the length of text the previous formatting info affected, then insert the
		//appropriate charformatting.
		
		//SetSel(index, end - 1);
		SetSel(index, end);
		
		GetSelectionCharFormat(cf);
		cf.dwMask = CFM_BOLD | CFM_ITALIC | CFM_CHARSET | CFM_FACE; // EBATECH(CNBR)
		if (cf.dwEffects & CFE_BOLD)
		{
			if (!tmpbold)
				cf.dwEffects ^= CFE_BOLD;
		}
		else
		{
			if (tmpbold)
				cf.dwEffects ^= CFE_BOLD;
		}
		if (cf.dwEffects & CFE_ITALIC)
		{
			if (!tmpitalic)
				cf.dwEffects ^= CFE_ITALIC;
		}
		else
		{
			if (tmpitalic)
				cf.dwEffects ^= CFE_ITALIC;
		}
		_tcscpy (cf.szFaceName, tmpfont);
		cf.bCharSet = (BYTE)tmpcharset;		// EBATECH(CNBR)
		cf.bPitchAndFamily = (BYTE)tmppitch; // EBATECH(CNBR)
		SetSelectionCharFormat(cf);
		GetWindowText(tmptext);
		
		//find the next set of formatting information
		index = tmptext.Find("\\f", 0);
	}
	
	//find all formatting information for text height.
	index = tmptext.Find("\\H", 0);
	while (index != -1)
	{
		tmptext.Delete(index, 2);
		int end = tmptext.Find(";", index);
		if (end == -1)
			break;		// error
		tmpheight = atof(tmptext.Mid(index, end - index));
		// Bugzilla No. 78034; 23-12-2002 [
		long tempHeight;
		ConvertTextHeightToTwips(tmpheight,tempHeight);

		height.Format("%.4f", tmpheight);
		if (((CMTextPropSheet*)GetParent())->GetFontSizeCombo()->FindStringExact(0, height) == -1)
		{
			int index = ((CMTextPropSheet*)GetParent())->GetFontSizeCombo()->AddString(height);
			((CMTextPropSheet *)GetParent())->GetFontSizeCombo()->SetItemData(index,tempHeight);
		}
		// Bugzilla No. 78034; 23-12-2002 ]
		tmptext.Delete(index, end - index + 1);
		
		end = tmptext.Find("\\H", index);
		if (end < index)
			//end = tmptext.GetLength() + 1;
			end = tmptext.GetLength();
		
		GetWindowText(tmptext);
		erase = tmptext.Find("\\H", 0);
		eraseend = tmptext.Find(";", erase);
		
		//Modified Cybage AW 30/04/2001[
		//Reason : Bug : The spaces between the words are deleted in the text editor.
		/*
		if (erase != 0 && tmptext.GetAt(erase - 1) == ' ')	
		{
		erase--;	
		index--;
		end--;
		}
		*/
		//Modified Cybage AW 30/04/2001]
		
		SetSel(erase, eraseend + 1);
		ReplaceSel("");
		
		//SetSel(index, end - 1);
		SetSel(index, end);
		
		GetSelectionCharFormat(cf);
		cf.dwMask = CFM_SIZE;
		
		//DP: old realization commented
		//cf.yHeight = (int) ((tmpheight / 0.2) * 200 + 0.5);
		ConvertTextHeightToTwips(tmpheight, cf.yHeight);
				
		SetSelectionCharFormat(cf);
		GetWindowText(tmptext);
		
		index = tmptext.Find("\\H", 0);
	}
	
	//find all formatting information for underlining.
	index = tmptext.Find("\\L", 0);
	while (index != -1)
	{
		tmptext.Delete(index, 2);
		int end = tmptext.Find("\\l", index);
		if (end < index)
			end = tmptext.GetLength() + 1;
		GetWindowText(tmptext);
		erase = tmptext.Find("\\L", 0);
		eraseend = tmptext.Find("\\l", 0);
		SetSel(erase, erase + 2);
		ReplaceSel("");
		SetSel(eraseend - 2, eraseend);
		ReplaceSel("");
		
		//SetSel(index, end - 1);
		SetSel(index, end);
		
		GetSelectionCharFormat(cf);
		cf.dwMask = CFM_UNDERLINE;
		cf.dwEffects = CFE_UNDERLINE;
		SetSelectionCharFormat(cf);
		GetWindowText(tmptext);
		
		index = tmptext.Find("\\L", 0);
	}
	
	SetSel(0, 0);
	UpdateData(true);
	((CMTextPropSheet*)GetParent())->UpdateChar();

	return true;
}

DWORD CMTextRichEdit::GetSelectionCharFormat( CHARFORMAT& cf ) const
{
	DWORD dwMask = CRichEditCtrl::GetSelectionCharFormat(cf);
	if(!((CMTextPropSheet *)GetParent())->m_bPasting)
		cf.yHeight = long(cf.yHeight / ((CMTextPropSheet *)GetParent())->GetZoom() + 0.5);
	return dwMask;
}

BOOL CMTextRichEdit::SetSelectionCharFormat( CHARFORMAT& cf )
{
	long savedHeight = cf.yHeight;	
	cf.yHeight = long(cf.yHeight * ((CMTextPropSheet *)GetParent())->GetZoom() + 0.5);
	BOOL ok = CRichEditCtrl::SetSelectionCharFormat(cf);
	cf.yHeight = savedHeight;	
	return ok;
}

DWORD CMTextRichEdit::GetDefaultCharFormat( CHARFORMAT& cf ) const
{
	DWORD dwMask = CRichEditCtrl::GetDefaultCharFormat(cf);
	cf.yHeight = long(cf.yHeight / ((CMTextPropSheet *)GetParent())->GetZoom() + 0.5);
	return dwMask;
}

BOOL CMTextRichEdit::SetDefaultCharFormat( CHARFORMAT& cf )
{
	long savedHeight = cf.yHeight;	
	cf.yHeight = long(cf.yHeight * ((CMTextPropSheet *)GetParent())->GetZoom() + 0.5);
	BOOL ok = CRichEditCtrl::SetDefaultCharFormat(cf);
	cf.yHeight = savedHeight;	
	return ok;
}

bool CMTextRichEdit::scaleText(double zoomCoeff)
{
	BOOL isLocked = LockWindowUpdate();

	long beginSel, endSel;
	GetSel(beginSel, endSel);

	CHARFORMAT cf;
	long begin = 0, index = 0, tmpHeight;
	long textLen = GetTextLength();

	SetSel(0, 0);
	CRichEditCtrl::GetSelectionCharFormat(cf);
	long height = cf.yHeight; 
	
	while (index <= textLen)
	{
		SetSel(index, ((index >= textLen) ? -1 : index + 1));
		CRichEditCtrl::GetSelectionCharFormat(cf);
		if(height != cf.yHeight)
		{
			tmpHeight = cf.yHeight;

			SetSel(begin, index);
			cf.dwMask = CFM_SIZE;
			cf.yHeight = long(height*zoomCoeff + 0.5); 
			CRichEditCtrl::SetSelectionCharFormat(cf);
			
			height = tmpHeight;
			begin = index;
		}
		++index; 
	}
	
	SetSel(begin, -1);
	cf.dwMask = CFM_SIZE;
	cf.yHeight = long(height*zoomCoeff + 0.5); 
	CRichEditCtrl::SetSelectionCharFormat(cf);

	CRichEditCtrl::GetDefaultCharFormat(cf);
	cf.dwMask = CFM_SIZE | CFM_OFFSET;
	cf.yHeight = long(cf.yHeight*zoomCoeff + 0.5); 
	CRichEditCtrl::SetDefaultCharFormat(cf);

	SetSel(beginSel, endSel);
	if(isLocked)
		UnlockWindowUpdate();
	return true;
}

// beginIndex - index of first scalled letter (0-based)
// endIndex - index of last scalled letter (0-based)
// scaleText(1,1,x) - only second letter will be scalled
bool CMTextRichEdit::scaleText(long beginIndex, long endIndex, double zoomCoeff)
{
	BOOL isLocked = LockWindowUpdate();

	long beginSel, endSel;
	GetSel(beginSel, endSel);

	CHARFORMAT cf;

	long textLen = GetTextLength();
	if(endIndex > textLen)
		endIndex = textLen;

	SetSel(beginIndex+1, beginIndex+1);
	CRichEditCtrl::GetSelectionCharFormat(cf);
	long height = cf.yHeight; 
	
	long begin = beginIndex, tmpHeight;
	for(long index = beginIndex + 1; index <= endIndex; ++index)
	{
		SetSel(index, ((index >= textLen) ? -1 : index + 1));
		CRichEditCtrl::GetSelectionCharFormat(cf);
		if(height != cf.yHeight)
		{
			tmpHeight = cf.yHeight;

			SetSel(begin, index);
			cf.dwMask = CFM_SIZE;
			cf.yHeight = long(height*zoomCoeff + 0.5); 
			CRichEditCtrl::SetSelectionCharFormat(cf);
			
			height = tmpHeight;
			begin = index;
		}
	}
	
	SetSel(begin, (endIndex==textLen ? -1 : endIndex + 1));
	cf.dwMask = CFM_SIZE;
	cf.yHeight = long(height*zoomCoeff + 0.5); 
	CRichEditCtrl::SetSelectionCharFormat(cf);

	SetSel(beginSel, endSel);
	if(isLocked)
		UnlockWindowUpdate();
	return true;

}
