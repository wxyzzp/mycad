/** FILENAME:     CtrlListAttributes.cpp- Contains implementation of class CCtrlListAttributes.
* Copyright  (C) Copyright 1998-99  Visio Corporation.  All Rights Reserved.
*
*
*
* Abstract Implementation of the dialog to show all the attributes of a
*           the selected tile.
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
#include "dcl.h"
#include "ctrllistattributes.h"
#include "dcldoc.h"
#include "treenode.h"

#define LISTTITLE _T("ListOfAttributes")
#define ATRRIB_NAME _T("Attribute Name")
#define ATRRIB_VALUE _T("Attribute Value")


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/************************************************************************************
/* + CCtrlListAttributes::CCtrlListAttributes(CWnd* pParent /*=NULL* - 
*
*    Constructor for the class
*
*
*
*/

CCtrlListAttributes::CCtrlListAttributes(CWnd* pParent /*=NULL*/)
	: CDialog(CCtrlListAttributes::IDD, pParent)
	{
	//{{AFX_DATA_INIT(CCtrlListAttributes)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	}


void CCtrlListAttributes::DoDataExchange(CDataExchange* pDX)
	{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCtrlListAttributes)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	}


BEGIN_MESSAGE_MAP(CCtrlListAttributes, CDialog)
	//{{AFX_MSG_MAP(CCtrlListAttributes)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/************************************************************************************
/* + CCtrlListAttributes::OnInitDialog()
*    All the initialisation of the dialog done here
*    Return Value : TRUE - Successful 
*                   FALSE - Unsuccessful
*   
*
*/

BOOL CCtrlListAttributes::OnInitDialog() 
	{
	CDialog::OnInitDialog();
	CFrameWnd* pFrame = (CFrameWnd*)AfxGetMainWnd();
	ASSERT( pFrame );
	CDCLDoc* pDoc = (CDCLDoc*)pFrame->GetActiveDocument();
	ASSERT( pDoc );
	ASSERT( pDoc->m_ptrCurrentNode );
	if ( pDoc->m_ptrCurrentNode->m_Tile.m_wszTileName.IsEmpty() )
		SetWindowText(LISTTITLE);	
	else
		SetWindowText(pDoc->m_ptrCurrentNode->m_Tile.m_wszTileName);
	
	CListCtrl* pListCtrl = (CListCtrl*)GetDlgItem(IDC_LIST_ATTRIBUTES);
	ASSERT( pListCtrl );
	if ( pListCtrl )
		{
		int nRet = InsertColumns(pListCtrl);
			if ( nRet == 0 )
				return FALSE;
		nRet = InitList(pListCtrl);
			if ( nRet == 0 )
				return FALSE;
		}
	return TRUE;  
	}

/************************************************************************************
/* +  CCtrlListAttributes::InsertColumns(CListCtrl * pListCtrl)
*     Add the columns to the ListCtrl- Attribute Name, Attribute Value
*
*     Return Value : 1 - Successful
*                    0 - Not Successful
*
*/

int CCtrlListAttributes::InsertColumns(                // Add columns to the list control
									   CListCtrl * pListCtrl // i : a pointer to the list control
									   )
	{
	ASSERT( pListCtrl );
	if ( pListCtrl )
		{
		RECT cRect;
		GetClientRect(&cRect);
		for ( int iCount = 0; iCount < 2; iCount++ )
			{
			LV_COLUMN lvc;
			lvc.mask=LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
			lvc.iSubItem = iCount;
			if ( iCount == 0 )
				{
				lvc.pszText = ATRRIB_NAME;
				}
			if ( iCount == 1 )
				{
				lvc.pszText = ATRRIB_VALUE;
				}
			lvc.cx = (cRect.right - cRect.left) / 2;
			lvc.fmt = LVCFMT_LEFT;
			int index = pListCtrl->InsertColumn(iCount,&lvc);
			}
		return 1;
		}
	return 0;
	}

/************************************************************************************
/* +  CCtrlListAttributes::InitList(CListCtrl * pListCtrll)
*     Initialises the List control with the attributes
*     Return values : 1 - Successful 
*                     0 - Unsuccessful
*
*
*/
int CCtrlListAttributes::InitList(						//  Initialise the list
								  CListCtrl * pListCtrl // i : pointer to the ListCtrl
								  )
	{
	ASSERT( pListCtrl );
	if ( pListCtrl )
		{
		//CString strInsert;
		CString strkey , strvalue;
		CFrameWnd* pFrame = (CFrameWnd*)AfxGetMainWnd();
		ASSERT( pFrame );
		CDCLDoc* pDoc = (CDCLDoc*)pFrame->GetActiveDocument();
		ASSERT( pDoc );
		//here add all the variables that are needed to show on the dialog to the document member m_MapName2Value
		POSITION position = pDoc->m_MapName2Value.GetStartPosition();
		for ( int i = 0; i < pDoc->m_MapName2Value.GetCount(); i++ )
			{
			pDoc->m_MapName2Value.GetNextAssoc(position,strkey,strvalue);
			LVITEM lv;
			lv.mask = LVIF_TEXT;
			lv.iItem = i;
			lv.iSubItem = 0;
			lv.pszText = _T(" ");
			lv.cchTextMax = 0;
			pListCtrl->InsertItem(&lv);
			for ( int j = 0; j < 2; j++ )
				{
				lv.pszText = NULL;
				lv.cchTextMax = 0;
				lv.iSubItem = j;
				if ( j == 0 )             //insert name
					{
					lv.pszText =  strkey.GetBuffer(strkey.GetLength());
					lv.cchTextMax = strkey.GetLength();
					}
				if ( j == 1 )           //insert value
					{
					lv.pszText =  strvalue.GetBuffer(strvalue.GetLength());
					lv.cchTextMax = strvalue.GetLength();
					}
				int nRes = pListCtrl->SetItem(&lv);
				ASSERT( nRes );
				}
			}
		return 1;
		}	
	return 0;
	}
