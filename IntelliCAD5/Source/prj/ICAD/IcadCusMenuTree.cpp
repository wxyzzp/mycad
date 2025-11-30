/* D:\DEV\PRJ\ICAD\ICADCUSMENUTREE.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

// IcadCusMenuTree.cpp : implementation file
//

#include "Icad.h"/*DNT*/
#include "IcadCusMenuTree.h"/*DNT*/
#include "IcadCusMenu.h"/*DNT*/
#include "IcadAccelerator.h"/*DNT*/

#ifdef _DEBUG

	#if !defined(USE_SMARTHEAP)
	#define new DEBUG_NEW
	#endif

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIcadCusMenuTree

CIcadCusMenuTree::CIcadCusMenuTree()
{
	m_cKeyDownBits=0;
	m_nOrigImage=(-1);
	m_hCutItem=NULL;
	m_hCopyItem=NULL;
}

CIcadCusMenuTree::~CIcadCusMenuTree()
{
}


BEGIN_MESSAGE_MAP(CIcadCusMenuTree, CTreeCtrl)
	//{{AFX_MSG_MAP(CIcadCusMenuTree)
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_CHAR()
	ON_WM_SYSKEYDOWN()
	ON_WM_SYSKEYUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIcadCusMenuTree message handlers

void CIcadCusMenuTree::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	//*** Return if the previous key state bit is set to 1 (the key is down before the
	//*** call).
	if(nFlags & 0x00004000) return;
	if(nChar==VK_CONTROL)
	{
		m_cKeyDownBits|=ICAD_ACCEL_CTRLBIT;
		return;
	}
	if(nChar==VK_SHIFT) m_cKeyDownBits|=ICAD_ACCEL_SHIFTBIT;
	else if(nChar==VK_MENU)	m_cKeyDownBits|=ICAD_ACCEL_ALTBIT;
	CTreeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CIcadCusMenuTree::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if(m_cKeyDownBits==ICAD_ACCEL_CTRLBIT) return;	
	CTreeCtrl::OnChar(nChar, nRepCnt, nFlags);
}

void CIcadCusMenuTree::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if(m_cKeyDownBits==ICAD_ACCEL_CTRLBIT)
	{
		if(nChar==VK_X)	//*** Cut
		{
			if(m_hCutItem!=NULL)
			{
				SetItemImage(m_hCutItem,m_nOrigImage,m_nOrigImage);
				m_hCutItem=NULL;
				m_nOrigImage=(-1);
			}
			m_hCopyItem=NULL;
			m_hCutItem=GetSelectedItem();
			if(m_hCutItem!=NULL)
			{
				int nSelImage;
				GetItemImage(m_hCutItem,m_nOrigImage,nSelImage);
				SetItemImage(m_hCutItem,4,4);
			}
		}
		else if(nChar==VK_C)	//*** Copy
		{
			if(m_hCutItem!=NULL)
			{
				SetItemImage(m_hCutItem,m_nOrigImage,m_nOrigImage);
				m_hCutItem=NULL;
				m_nOrigImage=(-1);
			}
			m_hCopyItem=GetSelectedItem();
		}
		else if(nChar==VK_V && (m_hCutItem!=NULL || m_hCopyItem!=NULL))	//*** Paste
		{
			HTREEITEM hOrigItem=NULL;
			if(m_hCutItem!=NULL)
			{
				SetItemImage(m_hCutItem,m_nOrigImage,m_nOrigImage);
				hOrigItem=m_hCutItem;
			}
			else hOrigItem=m_hCopyItem;

			HTREEITEM hDropTarget = GetSelectedItem();
			if(hDropTarget!=NULL && hOrigItem!=NULL)
			{
				//*** Copy the original item to the position above the drop target.
				HTREEITEM hNewItem = ((CIcadCusMenu*)m_pParentDlg)->CopyTreeItem(GetPrevSiblingItem(hDropTarget),
					hOrigItem,GetParentItem(hDropTarget));
				SelectItem(hNewItem);
			}
			if(m_hCutItem!=NULL)
			{
				//*** Delete the original item (and it's children).
				((CIcadCusMenu*)m_pParentDlg)->DeleteTreeItem(m_hCutItem);
				m_hCutItem=NULL;
				m_nOrigImage=(-1);
			}
			m_hCopyItem=NULL;
		}
		return;
	}
	if(nChar==VK_CONTROL)
	{
		m_cKeyDownBits&=(~ICAD_ACCEL_CTRLBIT);
		return;
	}
	if(nChar==VK_SHIFT) m_cKeyDownBits&=(~ICAD_ACCEL_SHIFTBIT);
	else if(nChar==VK_MENU)	m_cKeyDownBits&=(~ICAD_ACCEL_ALTBIT);
	CTreeCtrl::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CIcadCusMenuTree::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	OnKeyDown(nChar, nRepCnt, nFlags);
	CTreeCtrl::OnSysKeyDown(nChar, nRepCnt, nFlags);
}

void CIcadCusMenuTree::OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	OnKeyUp(nChar, nRepCnt, nFlags);
	CTreeCtrl::OnSysKeyUp(nChar, nRepCnt, nFlags);
}


