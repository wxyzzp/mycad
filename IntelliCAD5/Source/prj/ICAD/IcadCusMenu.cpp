/* D:\DEV\PRJ\ICAD\ICADCUSMENU.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * IcadCusMenu.cpp : implementation file
 * 
 */ 


#include "Icad.h"/*DNT*/
#include "IcadDoc.h"/*DNT*/
#include "IcadCusMenu.h"/*DNT*/

#include "IcadChildWnd.h"/*DNT*/
#include "IcadToolbarMain.h"/*DNT*/
#include "IcadCusMenuAdvDlg.h"/*DNT*/
#include "IcadMenuMain.h"/*DNT*/
#include "IcadCustom.h"/*DNT*/
#include "IcadApp.h"/*DNT*/
#include "LicensedComponents.h"/*DNT*/

#ifdef _DEBUG

	#if !defined(USE_SMARTHEAP)
	#define new DEBUG_NEW
	#endif

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ICAD_CUSMNU_BEGIN     0
#define ICAD_CUSMNU_INSITM    1
#define ICAD_CUSMNU_INSSUB    2
#define ICAD_CUSMNU_INSSPC    3
#define	ICAD_CUSMNU_INSRCO	  4
#define	ICAD_CUSMNU_INSRCOSUB 5
#define ICAD_CUSMNU_DELETE    6
#define ICAD_CUSMNU_RENAME    7
#define	ICAD_CUSMNU_ADDENTR	  8
#define	ICAD_CUSMNU_ADDESCP	  9
#define	ICAD_CUSMNU_ADDWAIT	  10
#define ICAD_CUSMNU_END       11

#define	ICAD_SPACERSTRING ResourceString(IDC_ICADCUSMENU___SPACER___0, "--Spacer--" )

extern int g_nMenus;							// Global value for number of menus
extern int g_nMnuTbItems;						// Global value for number of commands
extern struct IcadMenus g_IcadMenus[];			// Global structure for menu definitions
extern struct IcadCommands g_MnuTbItems[];		// Global command structure

int IcadMenuPositionSort(const void *cmp1, const void *cmp2);

/////////////////////////////////////////////////////////////////////////////
// CIcadCusMenu property page
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CIcadCusMenu, CPropertyPage)

CIcadCusMenu::CIcadCusMenu() : CPropertyPage(CIcadCusMenu::IDD),
							   m_pImageList(NULL), m_pDragImage(NULL), m_hCurDragTarget(NULL),
							   m_hPrevCursor(NULL), m_pCurMenu(NULL)
{
	//{{AFX_DATA_INIT(CIcadCusMenu)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CIcadCusMenu::~CIcadCusMenu()
{
	if(m_pImageList)
		delete m_pImageList;
}

void
CIcadCusMenu::DoDataExchange
(
 CDataExchange*	pDX
)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIcadCusMenu)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CIcadCusMenu, CPropertyPage)
	//{{AFX_MSG_MAP(CIcadCusMenu)
	ON_NOTIFY(TVN_ENDLABELEDIT, CUS_MENU_TREE, OnEndLabelEditMenuTree)
	ON_BN_CLICKED(CUS_MENU_BTNINS, OnButtonInsertItem)
	ON_BN_CLICKED(CUS_MENU_BTNDEL, OnButtonDelete)
	ON_BN_CLICKED(CUS_MENU_BTNREN, OnButtonRename)
	ON_NOTIFY(TVN_SELCHANGED, CUS_MENU_TREE, OnSelChangedMenuTree)
	ON_EN_KILLFOCUS(CUS_MENU_EDITCMD, OnKillFocusCommandEdit)
	ON_BN_CLICKED(CUS_MENU_BTNADD, OnAddCommandToEdit)
	ON_WM_CONTEXTMENU()
	ON_NOTIFY(TVN_DELETEITEM, CUS_MENU_TREE, OnDeleteTreeItem)
	ON_NOTIFY(TVN_BEGINDRAG, CUS_MENU_TREE, OnBeginDragMenuTree)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_LBN_DBLCLK(CUS_MENU_LIST, OnAddCommandToEdit)
	ON_BN_CLICKED(CUS_MENU_BTNADV, OnButtonAdvanced)
	//}}AFX_MSG_MAP
	ON_COMMAND_RANGE(ICAD_CUSMNU_BEGIN,ICAD_CUSMNU_END,ContextMenuPick)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CIcadCusMenu helper functions and message handlers
/////////////////////////////////////////////////////////////////////////////

BOOL
CIcadCusMenu::OnInitDialog() 
{
	resbuf	rb;
	int		ct, index;
	
	CPropertyPage::OnInitDialog();

	m_strLoadLisp.Empty();
	m_bDestroyOld = TRUE;

	// Subclass the tree control to get the keyup and keydown messages.
	m_TreeCtrl.SubclassDlgItem(CUS_MENU_TREE, this);
	m_TreeCtrl.m_pParentDlg = this;

	// Setup the image list for the visibility of menu items.
	m_pImageList = new CImageList();
	m_pImageList->Create(IDB_ICAD_CUSMENU_VISINDICATOR, 16, 0, RGB(255,255,255));
	CTreeCtrl*	pTreeCtrl = (CTreeCtrl*)GetDlgItem(CUS_MENU_TREE);
	pTreeCtrl->SetImageList(m_pImageList, TVSIL_NORMAL);

	// Draw the bitmap into the Insert... button.
	DrawInsertButton();

	// Fill the tree list.
	sds_getvar("WNDLMDI"/*DNT*/, &rb);
	if(!rb.resval.rint)
		m_lflgVisibility = ICAD_MN_CLS;
	else
		m_lflgVisibility = ICAD_MN_OPN;

	sds_getvar("EXPLEVEL"/*DNT*/, &rb);
	switch(rb.resval.rint)
	{
		case 1: m_lflgVisibility |= ICAD_MN_BEG; break;
		case 2: m_lflgVisibility |= ICAD_MN_INT; break;
		case 3: m_lflgVisibility |= ICAD_MN_EXP;
	}

	m_pCurMenu = SDS_CMainWindow->GetCurrentMenu();

	if(m_pCurMenu)
	{
		int	nMenuItems = m_pCurMenu->m_nMenuItems;
		for(ct = 0, index = 0; ct < nMenuItems; ++ct, ++index)
			AddMenuItemsToTree(&index, NULL);
	}

	// Fill the command listbox.
	CListBox*	pList = (CListBox*)GetDlgItem(CUS_MENU_LIST);
	int			idxNew, ctStr;
	char		*pstr, *ptr1;
	CString		localizedToolTip;

	for(ct = 0; ct < g_nMnuTbItems; ++ct)
	{
		if(!*g_MnuTbItems[ct].ToolTip || !*g_MnuTbItems[ct].Command || !g_MnuTbItems[ct].resourceIndex)
			continue;

		/*D.G.*/// Check for the accessibility of the licensed components commands.
		if(!CIcadApp::m_LicensedComponents.IsCommandAccessible(g_MnuTbItems[ct].Command))
			continue;

		// The localized tooltip is stored one beyond the resource index, if the pAcadTbStr is available.
		int	resourceIndex = g_MnuTbItems[ct].resourceIndex + (g_MnuTbItems[ct].pAcadTbStr? 1 : 0);
		localizedToolTip.LoadString(resourceIndex);

		if(!*localizedToolTip)
			continue;

		int	localizedToolTipLength = strlen(localizedToolTip);
		pstr = new char[localizedToolTipLength + 1];
		for(ptr1 = pstr, ctStr = 0; ctStr < localizedToolTipLength; ++ctStr)
		{
			// Step over any occurance of &
			if(localizedToolTip[ctStr] == '&')
				continue;

			// If we hit a \t, terminate the string there.
			if(localizedToolTip[ctStr] == '\t')
				break;

			*ptr1 = localizedToolTip[ctStr];
			++ptr1;
		}

		*ptr1 = 0;
		if(LB_ERR == pList->FindStringExact(-1, pstr))
		{
			idxNew = pList->AddString(pstr);
			if(idxNew != LB_ERR && idxNew != LB_ERRSPACE)
				pList->SetItemDataPtr(idxNew, (void*)g_MnuTbItems[ct].Command);
		}

		delete [] pstr;
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void
CIcadCusMenu::DrawInsertButton()
{
// This function draws the down arrow bitmap into the Insert... button.

	CButton*	pInsButton = (CButton*)GetDlgItem(CUS_MENU_BTNINS);
	// Create off-screen device context.
	CDC			*pButtonDC = pInsButton->GetDC(),
				*pMemDC = new CDC();
	pMemDC->CreateCompatibleDC(pButtonDC);

	// Select the bitmap into the device context.
	CBitmap*	pbm = new CBitmap();
	pbm->LoadMappedBitmap(IDB_ICAD_CUSMENU_POPIND);
	CBitmap*	pbmOld = pMemDC->SelectObject(pbm);

	BITMAP		bms;
	pbm->GetBitmap(&bms);

	CRect		rect(0, 0, bms.bmWidth, bms.bmHeight);
	CBrush		cb(GetSysColor(COLOR_BTNFACE));
	pMemDC->FillRect(rect, &cb);
	CPoint		points[3];

	points[0].x = 66;  points[0].y = 10;
	points[1].x = 72;  points[1].y = 10;
	points[2].x = 69;  points[2].y = 13;

	DeleteObject(pMemDC->SelectObject(CreatePen(PS_SOLID, 1, GetSysColor(COLOR_WINDOWTEXT))));
	DeleteObject(pMemDC->SelectObject(CreateSolidBrush(GetSysColor(COLOR_WINDOWTEXT))));
	pMemDC->Polygon(points, 3);

	// Create the font for the button.
	CFont*		pFont = new CFont();
	// Set the font based on whether we are running under Windows95 or NT 3.5.
	OSVERSIONINFO	osVer;
	osVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osVer);

	if( osVer.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS ||
		(osVer.dwPlatformId == VER_PLATFORM_WIN32_NT && osVer.dwMajorVersion >= 4) )
		// Windows 95 or NT 4.0
		pFont->CreateStockObject(DEFAULT_GUI_FONT);
	else
		pFont->CreateStockObject(DEVICE_DEFAULT_FONT);

	CFont*		pOldFont = pMemDC->SelectObject(pFont);

	pMemDC->SetBkColor(GetSysColor(COLOR_BTNFACE));
	pMemDC->SetTextColor(GetSysColor(COLOR_WINDOWTEXT));

	// Make sure the text gets centered correctly.
	CRect		rectBmp;
	pInsButton->GetClientRect(rectBmp);
	CString		str(ResourceString(IDC_ICADCUSMENU_INSERT____3, "Insert..."));
	TEXTMETRIC	tm;
    pMemDC->GetTextMetrics(&tm);
	pMemDC->SetTextAlign(TA_CENTER | TA_TOP);

	int			nYPos = (((rectBmp.bottom - 6) / 2) - (tm.tmHeight / 2)) - 1;

	pMemDC->TextOut(((rectBmp.right - 18) / 2), nYPos, str);

	// Clean up.
	pMemDC->SelectObject(pOldFont);
	delete pFont;
	pMemDC->SelectObject(pbmOld);
	delete pMemDC;
	pInsButton->ReleaseDC(pButtonDC);

	// Draw the bitmap on the button.
	pInsButton->SetBitmap((HBITMAP)pbm->Detach());
	delete pbm;
}

void
CIcadCusMenu::AddMenuItemsToTree
(
 int*		idxCurArrayItem,
 HTREEITEM	hParent
)
{
	/*D.G.*/// accessibility licensed components commands checking added

	if(!m_pCurMenu)
		return;

	CMainWindow*	pMain = (CMainWindow*)SDS_CMainWindow;
	int				nMenuArrayItems = m_pCurMenu->m_nMenuItems,
					ct, ctItems = 0, ctVisItems = 0;
	LPICMENUITEM	lpMenuArray = m_pCurMenu->m_pMenuArray;

	if(((*idxCurArrayItem) + 1) >= nMenuArrayItems)
		return;

	// Clear the hidden flag for parent items.  This flag may have been set in MenuCreator.
//	lpMenuArray[(*idxCurArrayItem)].lflgVisibility &= ~ICAD_MN_HIDE; // Commented out to avoid troubles with reseting an autoloaded menu that is referenced by more than one drawing, hope this doesn't break anything else.

	short	nCurSubLevel = lpMenuArray[(*idxCurArrayItem) + 1].nSubLevel;
	if( lpMenuArray[(*idxCurArrayItem)].nSubLevel == nCurSubLevel &&
		!lpMenuArray[(*idxCurArrayItem)].itemName.IsEmpty())
	{
		if(!CIcadApp::m_LicensedComponents.IsCommandAccessible(lpMenuArray[(*idxCurArrayItem)].command))
			return;

		// Create the parent tree item and return.
		LPICMENUITEM	lpMenuItem = new ICMENUITEM;
		CopyMenuItem(lpMenuItem, &(lpMenuArray[(*idxCurArrayItem)]));

		HTREEITEM		hNewItem = AddOneItem(hParent, lpMenuArray[(*idxCurArrayItem)].itemName, ctItems, lpMenuItem);
		return;
	}

	/*D.G.*/// The following vars are added to fix the following bug:
			// An item is created before its subitems that have their subitems are checked
			// (for example, if the Raster is disabled then the "Image Enabler" item is created because
			// it has the "Support" subitem, but the "Support" item hasn't available subitems.).
	bool		HaveCommandItems = false,	// True if we have a subitem with an accessible command.
				ParentItemMustDie = true;	// True if the parent item has to be removed.
	HTREEITEM	hParentItem = NULL,			// The parent item to remove if need.
				hSpItem = NULL;				// Also, remove the spacer before the removed parent.

	for(ct = ((*idxCurArrayItem) + 1); ct < nMenuArrayItems && lpMenuArray[ct].nSubLevel >= nCurSubLevel; ++ct)
	{
		if(lpMenuArray[ct].nSubLevel == nCurSubLevel)
		{
			// Check for the accessibility of the licensed components commands.
			if(!CIcadApp::m_LicensedComponents.IsCommandAccessible(lpMenuArray[ct].command))
				continue;

			// Check if we have a subitem with a command.
			if(!HaveCommandItems && !lpMenuArray[ct].command.IsEmpty())
				HaveCommandItems = true;

			if((ctItems > 0) && lpMenuArray[ct].bAddSpacerBefore)	/*D.G.*/// (ctItems > 0) added: don't create a spacer for the 1'st item
				if(lpMenuArray[ct].itemName.Compare(ICAD_ENDSPACERSTR))
					++ctItems;

			++ctItems;

			// Increment the number of visible items also.
			if((m_lflgVisibility & lpMenuArray[ct].lflgVisibility) == m_lflgVisibility)
			{
				if((ctItems > 0) && lpMenuArray[ct].bAddSpacerBefore)	/*D.G.*/// (ctItems > 0) added: don't create a spacer for the 1'st item
					if(lpMenuArray[ct].itemName.Compare(ICAD_ENDSPACERSTR))
						++ctVisItems;

				++ctVisItems;
			}
		}
	}

	if(ctItems <= 0)
		return;

	if(HaveCommandItems)
		ParentItemMustDie = false;

	// Create the parent tree item.
	LPICMENUITEM	lpMenuItem = new ICMENUITEM;
	CopyMenuItem(lpMenuItem, &(lpMenuArray[(*idxCurArrayItem)]));

	// Set a default visibility for a root item that has visibile sub-items.
	if( !(lpMenuArray[(*idxCurArrayItem)].lflgVisibility & ICAD_MN_RCO) &&
		!lpMenuArray[(*idxCurArrayItem)].nSubLevel && ctVisItems > 0 )
		lpMenuItem->lflgVisibility = m_lflgVisibility;

	// Add the parent item.
	HTREEITEM	hNewItem = AddOneItem(hParent, lpMenuArray[*idxCurArrayItem].itemName, ctItems, lpMenuItem);
	hParentItem = hNewItem;

	// Add the child items.
	ct = 0;
	for(ct = 0; ct < ctItems; )
	{
		// Check for the accessibility of the licensed components commands.
		if(!CIcadApp::m_LicensedComponents.IsCommandAccessible(lpMenuArray[++(*idxCurArrayItem)].command))
			continue;

		// If the bAddSpacerBefore flag is set.
		if((ct > 0) && lpMenuArray[(*idxCurArrayItem)].bAddSpacerBefore)	/*D.G.*/// (ct > 0) added: don't create a spacer for the 1'st item
		{
			hSpItem = AddOneItem(hNewItem, ICAD_SPACERSTRING, 0, &(lpMenuArray[*idxCurArrayItem]));

			++ct;

			if(!lpMenuArray[(*idxCurArrayItem)].itemName.Compare(ICAD_ENDSPACERSTR))
				continue;

			if(ct >= ctItems) break;
		}

		// If the item is a parent of another branch.
		if(*idxCurArrayItem + 1 < nMenuArrayItems && lpMenuArray[(*idxCurArrayItem) + 1].nSubLevel == nCurSubLevel + 1)
		{					 
			// Call this function recursively.
			int	ctTmp = *idxCurArrayItem;
			AddMenuItemsToTree(&ctTmp, hNewItem);
			++ct;
			if(*idxCurArrayItem != ctTmp)
			{
				ParentItemMustDie = false;
				*idxCurArrayItem = ctTmp;
			}
			else
			{
				BOOL	Saved_bMenuModified;		// it set to TRUE in DeleteTreeItem but we needn't it now
				if(m_pCurMenu)
					Saved_bMenuModified = m_pCurMenu->m_bMenuModified;

				if(lpMenuArray[(*idxCurArrayItem)].bAddSpacerBefore && hSpItem)
					DeleteTreeItem(hSpItem);

				if(m_pCurMenu)
					m_pCurMenu->m_bMenuModified = Saved_bMenuModified;
		
				++(*idxCurArrayItem);
			}
			continue;
		}

		// Otherwise create the menu item structure and add the item.
		lpMenuItem = new ICMENUITEM;
		CopyMenuItem(lpMenuItem, &(lpMenuArray[*idxCurArrayItem]));

		if(!lpMenuItem->itemName.IsEmpty())
		{
			// Convert tab characters(\t) to \\t characters for display.
			char*	pszName = NULL;
			int		nLen = ic_escconv((char*)(LPCTSTR)lpMenuItem->itemName, NULL, '\\', ResourceString(IDC_ICADCUSMENU_T_T_4, "t\t"), 0, 1);

			if(nLen > -1)
			{
				pszName = new char[nLen + 1];
				ic_escconv((char*)(LPCTSTR)lpMenuItem->itemName, pszName, '\\', ResourceString(IDC_ICADCUSMENU_T_T_4, "t\t"), 0, 1);
			}

			lpMenuItem->itemName = pszName;
			delete [] pszName;
		}

		AddOneItem(hNewItem, lpMenuItem->itemName, 0, lpMenuItem);
		++ct;
	}

	if(ParentItemMustDie)
	{
		BOOL	Saved_bMenuModified;		// it set to TRUE in DeleteTreeItem but we needn't it now
		if(m_pCurMenu)
			Saved_bMenuModified = m_pCurMenu->m_bMenuModified;

		DeleteTreeItem(hParentItem);

		if(m_pCurMenu)
			m_pCurMenu->m_bMenuModified = Saved_bMenuModified;
	}
}

HTREEITEM
CIcadCusMenu::AddOneItem
(
 HTREEITEM		hParent,
 LPCTSTR		szText,
 int			cChildren,
 LPICMENUITEM	lpMenuItem
)
{
	TV_ITEM			tvItem;
	TV_INSERTSTRUCT	tvInsert;

	tvItem.mask           = TVIF_TEXT | TVIF_CHILDREN | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	tvItem.pszText	      = (char*)szText;
	tvItem.cchTextMax     = lstrlen(szText);
	tvItem.cChildren      = cChildren;

	CString			str(szText);

	if( (!str.Compare(ICAD_SPACERSTRING) && !lpMenuItem) ||
		(lpMenuItem && ((m_lflgVisibility & lpMenuItem->lflgVisibility) == m_lflgVisibility)) )
	{
		if(lpMenuItem->lflgVisibility & ICAD_MN_RCO)
			tvItem.iImage = tvItem.iSelectedImage = 3;
		else
			tvItem.iImage = tvItem.iSelectedImage = 1;
	}
	else
	{
		if(lpMenuItem->lflgVisibility & ICAD_MN_RCO)
			tvItem.iImage = tvItem.iSelectedImage = 2;
		else
			tvItem.iImage = tvItem.iSelectedImage = 0;
	}

	// Set the lParam member as a pointer to the ICMENUITEM structure for this menu item.
	if(!str.Compare(ICAD_SPACERSTRING) || !lpMenuItem)
		tvItem.lParam = NULL;
	else
		tvItem.lParam = (long)lpMenuItem;

	tvInsert.hParent      = hParent;
	tvInsert.hInsertAfter = TVI_LAST;
	tvInsert.item         = tvItem;

	CTreeCtrl*	pTreeCtrl = (CTreeCtrl*)GetDlgItem(CUS_MENU_TREE);

	return pTreeCtrl->InsertItem(&tvInsert);
}

void
CIcadCusMenu::CopyMenuItem
(
 LPICMENUITEM	lpDest,
 LPICMENUITEM	lpSrc
)
{
	if(m_pCurMenu)
		m_pCurMenu->CopyItem(lpDest, lpSrc);
}

void
CIcadCusMenu::CopyMenuItem
(
 LPICMENUITEM	lpDest,
 LPICMNUINFO	lpSrc
)
{
	if(!lpSrc->title.IsEmpty())
		lpDest->itemName = lpSrc->title;

	if(!lpSrc->tearOffName.IsEmpty())
		lpDest->tearOffName = lpSrc->tearOffName;

	if(!lpSrc->command.IsEmpty())
		lpDest->command = lpSrc->command;

	if(!lpSrc->fileName.IsEmpty())
		lpDest->fileName = lpSrc->fileName;

	lpDest->lflgVisibility = ICAD_MN_ALL;
	if(lpSrc->bIsContextMenu)
	{
		lpDest->lflgVisibility |= ICAD_MN_RCO;
		lpDest->lflgEntVis = ICAD_MN_ENT;
	}
}	

BOOL
CIcadCusMenu::PreTranslateMessage
(
 MSG*	pMsg
)
{
	if(pMsg->message == WM_KEYDOWN)
	{	
		if(pMsg->wParam == 13 || pMsg->wParam == 27)	// Enter or Esc keys
		{
			// If the ENTER key or the ESC key gets pressed while editing an item,
			// send the message to the editbox so the property sheet doesn't close.
			CTreeCtrl*	pTreeCtrl = (CTreeCtrl*)GetDlgItem(CUS_MENU_TREE);
			CEdit*		pEdit = pTreeCtrl->GetEditControl();

			if(pEdit)
			{
				::SendMessage(pEdit->GetSafeHwnd(), pMsg->message, pMsg->wParam, pMsg->lParam);
				return TRUE;
			}
		}
	}

	return CPropertyPage::PreTranslateMessage(pMsg);
}


void
CIcadCusMenu::OnContextMenu
(
 CWnd*	pWnd,
 CPoint	point
)
{
	CTreeCtrl*	pTreeCtrl = (CTreeCtrl*)GetDlgItem(CUS_MENU_TREE);
	CListBox*	pList = (CListBox*)GetDlgItem(CUS_MENU_LIST);

	if(pWnd != pTreeCtrl && pWnd != pList)
		return;

	// Create the popup menu
	CMenu*	pMenu = new CMenu();
	pMenu->CreatePopupMenu();

	if(pWnd == pTreeCtrl)
	{
		pMenu->AppendMenu(MF_STRING | MF_UNCHECKED, ICAD_CUSMNU_INSITM, ResourceString(IDC_ICADCUSMENU_INSERT_MENU__5, "Insert Menu Item"));
		pMenu->AppendMenu(MF_STRING | MF_UNCHECKED, ICAD_CUSMNU_INSSUB, ResourceString(IDC_ICADCUSMENU_INSERT_MENU__6, "Insert Menu Sub-Item"));
		pMenu->AppendMenu(MF_STRING | MF_UNCHECKED, ICAD_CUSMNU_INSSPC, ResourceString(IDC_ICADCUSMENU_INSERT_SPACE_7, "Insert Spacer"));
		pMenu->AppendMenu(MF_SEPARATOR);
		pMenu->AppendMenu(MF_STRING | MF_UNCHECKED, ICAD_CUSMNU_INSRCO, ResourceString(IDC_ICADCUSMENU_INSERT_CONTE_8, "Insert Context Menu Item"));
		pMenu->AppendMenu(MF_STRING | MF_UNCHECKED, ICAD_CUSMNU_INSRCOSUB, ResourceString(IDC_ICADCUSMENU_INSERT_CONTE_9, "Insert Context Menu Sub-Item"));
		pMenu->AppendMenu(MF_SEPARATOR);
		pMenu->AppendMenu(MF_STRING | MF_UNCHECKED, ICAD_CUSMNU_DELETE, ResourceString(IDC_ICADCUSMENU_DELETE_10, "Delete"));
		pMenu->AppendMenu(MF_STRING | MF_UNCHECKED, ICAD_CUSMNU_RENAME, ResourceString(IDC_ICADCUSMENU_RENAME_11, "Rename"));
	}
	else
	{
		UINT	nFlags = MF_STRING | MF_UNCHECKED;

		// If the Add Command button is not enabled gray the menu items.
		CButton*	pBtnAdd = (CButton*)GetDlgItem(CUS_MENU_BTNADD);

		if(!pBtnAdd->IsWindowEnabled())
			nFlags |= MF_GRAYED;

		pMenu->AppendMenu(nFlags, ICAD_CUSMNU_ADDENTR, ResourceString(IDC_ICADCUSMENU_ADD_ENTER_C_12, "Add Enter command"));
		pMenu->AppendMenu(nFlags, ICAD_CUSMNU_ADDESCP, ResourceString(IDC_ICADCUSMENU_ADD_ESCAPE__13, "Add Escape command"));
		pMenu->AppendMenu(nFlags, ICAD_CUSMNU_ADDWAIT, ResourceString(IDC_ICADCUSMENU_WAIT_FOR_IN_14, "Wait for input command"));
	}

	// Display the menu
	pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y, this);

	// Delete the menu
	delete pMenu;
}

void
CIcadCusMenu::ContextMenuPick
(
 UINT	nId
)
{
	CTreeCtrl*	pTreeCtrl = (CTreeCtrl*)GetDlgItem(CUS_MENU_TREE);
	if(!pTreeCtrl)
		return;

	HTREEITEM	hItem = pTreeCtrl->GetSelectedItem();

	if(nId == ICAD_CUSMNU_INSITM || nId == ICAD_CUSMNU_INSRCO)
	{
		HTREEITEM	hParent = pTreeCtrl->GetParentItem(hItem),
					hInsertAfter = pTreeCtrl->GetPrevSiblingItem(hItem);

		if(!hInsertAfter)
			hInsertAfter = TVI_FIRST;

		HTREEITEM	hNew = pTreeCtrl->InsertItem(ResourceString(IDC_ICADCUSMENU___15, " "), hParent, hInsertAfter);
		pTreeCtrl->SelectItem(hNew);

		CEdit*		pEdit = pTreeCtrl->EditLabel(hNew);
		pEdit->Clear();
	}
	else if(nId == ICAD_CUSMNU_INSSUB || nId == ICAD_CUSMNU_INSRCOSUB)
	{
		HTREEITEM	hNew;

		if(!pTreeCtrl->ItemHasChildren(hItem))
		{
			SaveCommandToItem(hItem);
			LPICMENUITEM	lpMenuItem = (LPICMENUITEM)pTreeCtrl->GetItemData(hItem);
			if(!lpMenuItem)
				return;

			if(!lpMenuItem->command.IsEmpty() || !lpMenuItem->helpString.IsEmpty())
			{
				CString	str1, str2;
				str2 = pTreeCtrl->GetItemText(hItem);
				str1.Format(ResourceString(IDC_ICADCUSMENU_THIS_WILL_T_16, "This will turn the item '%s' into a submenu heading,\nand will delete its Command and Help String information.\nDo you wish to continue?"), str2);

				if(MessageBox(str1, ResourceString(IDC_ICADCUSMENU_CONFIRM_SUB_18, "Confirm Submenu Insertion"), MB_YESNO | MB_ICONQUESTION) == IDNO)
					return;
			}

			// Set the cChildren counter for the parent item to one.
			TV_ITEM	tvItem;
			tvItem.mask = TVIF_CHILDREN | TVIF_HANDLE;
			tvItem.hItem = hItem;
			tvItem.cChildren = 1;

			pTreeCtrl->SetItem(&tvItem);

			// Clear the Command and HelpString editboxes to delete them from the
			// ICMENUITEM structure.
			GetDlgItem(CUS_MENU_EDITCMD)->SetWindowText(ResourceString(IDC_ICADCUSMENU__19, ""));
			GetDlgItem(CUS_MENU_EDITHELPSTR)->SetWindowText(ResourceString(IDC_ICADCUSMENU__19, ""));
			lpMenuItem->chekVar.Empty();
			lpMenuItem->grayVar.Empty();
			hNew = pTreeCtrl->InsertItem(ResourceString(IDC_ICADCUSMENU___15, " "), hItem, TVI_FIRST);
		}
		else
			hNew = pTreeCtrl->InsertItem(ResourceString(IDC_ICADCUSMENU___15, " "), hItem, TVI_LAST);

		// Edit the text for the new item.
		pTreeCtrl->SelectItem(hNew);
		CEdit*	pEdit = pTreeCtrl->EditLabel(hNew);
		pEdit->Clear();
	}
	else if(nId == ICAD_CUSMNU_INSSPC)
	{
		HTREEITEM	hParent = pTreeCtrl->GetParentItem(hItem);

		if(!hParent)
		{
			MessageBox(ResourceString(IDC_ICADCUSMENU_CANNOT_ADD__20, "Cannot add a spacer to the root menu."), NULL, MB_OK | MB_ICONEXCLAMATION);
			return;
		}

		LPICMENUITEM	lpMenuItem = (LPICMENUITEM)pTreeCtrl->GetItemData(hItem);

		if(!lpMenuItem)
		{
			// Cannot add a spacer before an existing spacer.
			MessageBox(ResourceString(IDC_ICADCUSMENU_CANNOT_ADD__21, "Cannot add a spacer immediately before an existing spacer."), NULL, MB_OK | MB_ICONEXCLAMATION);
			return;
		}

		HTREEITEM	hInsertAfter = pTreeCtrl->GetPrevSiblingItem(hItem);

		if(!hInsertAfter)
			hInsertAfter = TVI_FIRST;
		else
		{
			lpMenuItem = (LPICMENUITEM)pTreeCtrl->GetItemData(hInsertAfter);

			if(!lpMenuItem)
			{
				// Cannot add a spacer after an existing spacer.
				MessageBox(ResourceString(IDC_ICADCUSMENU_CANNOT_ADD__22, "Cannot add a spacer immediately after an existing spacer."), NULL, MB_OK | MB_ICONEXCLAMATION);
				return;
			}
		}

		int	nImage, nSelectedImage;
		pTreeCtrl->GetItemImage(hParent, nImage, nSelectedImage);
		pTreeCtrl->InsertItem(ICAD_SPACERSTRING, nImage, nSelectedImage, hParent, hInsertAfter);
	}
	else if(nId == ICAD_CUSMNU_DELETE)
	{
		int		nRet;
		CString	str1, str2;

		if(!hItem)
		{
			sds_alert(ResourceString(IDC_ICADCUSMENU_AN_ITEM_HAS_23, "An item has not yet been selected."));
			return;
		}

		str2 = pTreeCtrl->GetItemText(hItem);

		if(pTreeCtrl->ItemHasChildren(hItem))
		{
			str1.Format(ResourceString(IDC_ICADCUSMENU_ARE_YOU_SUR_24, "Are you sure you want to remove the submenu '%s' and all its contents?"), str2);
			nRet = MessageBox(str1, ResourceString(IDC_ICADCUSMENU_CONFIRM_SUB_25, "Confirm Submenu Delete" ), MB_YESNO | MB_ICONQUESTION);
		}
		else
		{
			str1.Format(ResourceString(IDC_ICADCUSMENU_ARE_YOU_SUR_26, "Are you sure you want to remove the item '%s'?"), str2);
			nRet = MessageBox(str1,ResourceString(IDC_ICADCUSMENU_CONFIRM_ITE_27, "Confirm Item Delete"), MB_YESNO | MB_ICONQUESTION);
		}

		if(nRet == IDNO)
			return;

		DeleteTreeItem(hItem);
	}
	else if(nId == ICAD_CUSMNU_RENAME)
	{
		if(!hItem)
		{
			sds_alert(ResourceString(IDC_ICADCUSMENU_AN_ITEM_HAS_23, "An item has not yet been selected."));
			return;
		}

		pTreeCtrl->EditLabel(hItem);
	}
	else if(nId == ICAD_CUSMNU_ADDENTR)
	{
		AddCommandToEdit(ResourceString(IDC_ICADCUSMENU___28, ";"));
	}
	else if(nId == ICAD_CUSMNU_ADDESCP)
	{
		AddCommandToEdit(ResourceString(IDC_ICADCUSMENU__C_29, "^C"));
	}
	else if(nId == ICAD_CUSMNU_ADDWAIT)
	{
		AddCommandToEdit(ResourceString(IDC_ICADCUSMENU____30, "\\"));
	}
}

void
CIcadCusMenu::DeleteTreeItem
(
 HTREEITEM	hItem
)
{
	CTreeCtrl*	pTreeCtrl = (CTreeCtrl*)GetDlgItem(CUS_MENU_TREE);
	if(!pTreeCtrl)
		return;

	HTREEITEM	hParent = pTreeCtrl->GetParentItem(hItem);
	if(hParent)
	{
		HTREEITEM	hNextSibling = pTreeCtrl->GetNextSiblingItem(hItem),
					hPrevSibling = pTreeCtrl->GetPrevSiblingItem(hItem);

		if(!hNextSibling && !hPrevSibling)
		{
			// Set the cChildren counter for the parent to zero.
			TV_ITEM	tvItem;
			tvItem.mask = TVIF_CHILDREN | TVIF_HANDLE;
			tvItem.hItem = hParent;
			tvItem.cChildren = 0;

			pTreeCtrl->SetItem(&tvItem);
		}
	}

	pTreeCtrl->DeleteItem(hItem);

	if(m_pCurMenu)
		m_pCurMenu->m_bMenuModified = TRUE;
}

void
CIcadCusMenu::OnEndLabelEditMenuTree
(
 NMHDR*		pNMHDR,
 LRESULT*	pResult
)
{
	TV_DISPINFO*	pDispInfo = (TV_DISPINFO*)pNMHDR;
	CTreeCtrl*		pTreeCtrl = (CTreeCtrl*)GetDlgItem(CUS_MENU_TREE);
	HTREEITEM		hItem = pTreeCtrl->GetSelectedItem();

	*pResult = 0;

	// If the edit was canceled.
	if(!pDispInfo->item.pszText)
	{
		// Delete the item if the string is " " (" " = new item).
		CString	str(pTreeCtrl->GetItemText(hItem));
		if(!str.Compare(ResourceString(IDC_ICADCUSMENU___15, " ")))
			DeleteTreeItem(hItem);
		return;
	}

	// Don't allow a blank entry for a name
	if(!*(pDispInfo->item.pszText))
	{
		MessageBox(ResourceString(IDC_ICADCUSMENU_ENTER_A_NAM_31, "Enter a name for the new item before continuing."), ResourceString(IDC_ICADCUSMENU_RENAME_11, "Rename"), MB_ICONEXCLAMATION);
		pTreeCtrl->EditLabel(hItem);
		return;
	}

	// Set the tree item's text to the new value.
	pTreeCtrl->SetItemText(hItem, pDispInfo->item.pszText);

	// Update the pszName member of the ICMENUITEM struct.
	LPICMENUITEM	lpMenuItem = (LPICMENUITEM)pTreeCtrl->GetItemData(hItem);
	if(!lpMenuItem)
	{
		// May be renaming a spacer, give this item a menuitem.
		lpMenuItem = InitNewMenuItem(hItem);
		pTreeCtrl->SetItemData(hItem, (DWORD)lpMenuItem);
		EnableCommandControls();
	}

	lpMenuItem->itemName = pDispInfo->item.pszText;
}

void
CIcadCusMenu::OnSelChangedMenuTree
(
 NMHDR*		pNMHDR,
 LRESULT*	pResult
)
{
	NM_TREEVIEW*	pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	*pResult = 0;

	// Save changes to the command for the old item before displaying the new item.
	SaveCommandToItem(pNMTreeView->itemOld.hItem);
	CEdit		*pEdit = (CEdit*)GetDlgItem(CUS_MENU_EDITCMD),
				*pEditHelp = (CEdit*)GetDlgItem(CUS_MENU_EDITHELPSTR);
	CTreeCtrl*	pTreeCtrl = (CTreeCtrl*)GetDlgItem(CUS_MENU_TREE);

	// Enable controls based on the type of item selected.
	if(!pNMTreeView->itemNew.lParam)
	{ 
		CString	str(pTreeCtrl->GetItemText(pNMTreeView->itemNew.hItem));
		if(!str.Compare(ICAD_SPACERSTRING))
		{
			// The item is a Spacer
			EnableCommandControls(FALSE);
			pEdit->SetWindowText(""/*DNT*/);
			pEditHelp->SetWindowText(""/*DNT*/);

			// Change the Tear-off Name editbox back to a Command editbox.
			CWnd*	pCmdTxt = GetDlgItem(CUS_MENU_TXTCMD);
			pCmdTxt->SetWindowText(ResourceString(IDC_ICADCUSMENU_COMMAND__32, "Command:"));
		}
		else
		{
			// The item must be a NEW item, allocate a ICMENUITEM struct for it.
			LPICMENUITEM	lpMenuItem = InitNewMenuItem(pNMTreeView->itemNew.hItem);
			pTreeCtrl->SetItemData(pNMTreeView->itemNew.hItem, (DWORD)lpMenuItem);

			// Set the image for this item.
			int	 idxImage = 0;
			if((m_lflgVisibility & lpMenuItem->lflgVisibility) == m_lflgVisibility)
				idxImage = 1;

			pTreeCtrl->SetItemImage(pNMTreeView->itemNew.hItem, idxImage, idxImage);
			CWnd*		pCmdTxt = GetDlgItem(CUS_MENU_TXTCMD);
			HTREEITEM	hParent = pTreeCtrl->GetParentItem(pNMTreeView->itemNew.hItem);

			if(!hParent)
			{
				EnableCommandControls(FALSE);
				// Change the Tear-off Name editbox back to a Command editbox.
				pCmdTxt->EnableWindow();
				pEdit->EnableWindow();
			}
			else
			{
				EnableCommandControls();
				// Change the Tear-off Name editbox back to a Command editbox.
				CWnd*	pCmdTxt = GetDlgItem(CUS_MENU_TXTCMD);
				pCmdTxt->SetWindowText(ResourceString(IDC_ICADCUSMENU_COMMAND__32, "Command:"));
			}

			pEdit->SetWindowText(""/*DNT*/);
			pEditHelp->SetWindowText(""/*DNT*/);
			m_pCurMenu->m_bMenuModified = TRUE;
		}

		return;
	}

	if( pTreeCtrl->ItemHasChildren(pNMTreeView->itemNew.hItem) ||
		!pTreeCtrl->GetParentItem(pNMTreeView->itemNew.hItem) )
	{
		// The item is a parent
		pEdit->SetWindowText(""/*DNT*/);
		pEditHelp->SetWindowText(""/*DNT*/);
		EnableCommandControls(FALSE);

		// Change the Command editbox to an enabled Tear-off Name editbox
		CWnd*	pCmdTxt = GetDlgItem(CUS_MENU_TXTCMD);
		pCmdTxt->EnableWindow();
		pEdit->EnableWindow();
		LPICMENUITEM	lpMenuItem = (LPICMENUITEM)pNMTreeView->itemNew.lParam;

		if(!lpMenuItem->tearOffName.IsEmpty())
			pEdit->SetWindowText(lpMenuItem->tearOffName);
		return;
	}

	// The item is a child item that can have a command and help string.
	LPICMENUITEM	lpMenuItem = (LPICMENUITEM)pNMTreeView->itemNew.lParam;
	EnableCommandControls();

	if(lpMenuItem->command.IsEmpty())
		pEdit->SetWindowText(""/*DNT*/);
	else
		pEdit->SetWindowText(lpMenuItem->command);

	if(lpMenuItem->helpString.IsEmpty())
		pEditHelp->SetWindowText(""/*DNT*/);
	else
		pEditHelp->SetWindowText(lpMenuItem->helpString);

	// Change the Tear-off Name editbox back to a Command editbox.
	CWnd*	pCmdTxt = GetDlgItem(CUS_MENU_TXTCMD);
	pCmdTxt->SetWindowText(ResourceString(IDC_ICADCUSMENU_COMMAND__32, "Command:"));
}
	
LPICMENUITEM
CIcadCusMenu::InitNewMenuItem
(
 HTREEITEM	hItem
)
{
	LPICMENUITEM	lpMenuItem = new ICMENUITEM;

	// Set the visibility of this item to the same visibility of it's parent.
	CTreeCtrl*		pTreeCtrl = (CTreeCtrl*)GetDlgItem(CUS_MENU_TREE);
	HTREEITEM		hParent = pTreeCtrl->GetParentItem(hItem);

	if(!hParent)
		lpMenuItem->lflgVisibility = m_lflgVisibility;
	else
	{
		LPICMENUITEM	lpParentMenuItem = (LPICMENUITEM)pTreeCtrl->GetItemData(hParent);
		lpMenuItem->lflgVisibility = lpParentMenuItem->lflgVisibility;
		lpMenuItem->lflgEntVis = lpParentMenuItem->lflgEntVis;
	}

	// Set the nSubLevel member.
	short		nCurSubLevel = 0;
	HTREEITEM	hTmp = hParent;

	while(hTmp)
	{
		++nCurSubLevel;
		hTmp = pTreeCtrl->GetParentItem(hTmp);
	}

	lpMenuItem->nSubLevel = nCurSubLevel;

	return lpMenuItem;
}

void
CIcadCusMenu::EnableCommandControls
(
 BOOL	bEnable
)
{
	GetDlgItem(CUS_MENU_EDITCMD)->EnableWindow(bEnable);
	GetDlgItem(CUS_MENU_TXTCMD)->EnableWindow(bEnable);
	GetDlgItem(CUS_MENU_EDITHELPSTR)->EnableWindow(bEnable);
	GetDlgItem(CUS_MENU_TXTHELPSTR)->EnableWindow(bEnable);
	GetDlgItem(CUS_MENU_BTNADD)->EnableWindow(bEnable);
	GetDlgItem(CUS_MENU_BTNADV)->EnableWindow(bEnable);
}

void
CIcadCusMenu::OnButtonInsertItem() 
{
	// Create the popup menu
	CMenu*	pMenu = new CMenu();
	pMenu->CreatePopupMenu();

	pMenu->AppendMenu(MF_STRING | MF_UNCHECKED, ICAD_CUSMNU_INSITM, ResourceString(IDC_ICADCUSMENU_MENU_ITEM_34, "Menu Item"));
	pMenu->AppendMenu(MF_STRING | MF_UNCHECKED, ICAD_CUSMNU_INSSUB, ResourceString(IDC_ICADCUSMENU_MENU_SUB_IT_35, "Menu Sub-Item"));
	pMenu->AppendMenu(MF_STRING | MF_UNCHECKED, ICAD_CUSMNU_INSSPC, ResourceString(IDC_ICADCUSMENU_SPACER_36, "Spacer"));
	pMenu->AppendMenu(MF_SEPARATOR);
	pMenu->AppendMenu(MF_STRING | MF_UNCHECKED, ICAD_CUSMNU_INSRCO, ResourceString(IDC_ICADCUSMENU_CONTEXT_MEN_37, "Context Menu Item"));
	pMenu->AppendMenu(MF_STRING | MF_UNCHECKED, ICAD_CUSMNU_INSRCOSUB, ResourceString(IDC_ICADCUSMENU_CONTEXT_MEN_38, "Context Menu Sub-Item"));

	CButton*	pButton = (CButton*)GetDlgItem(CUS_MENU_BTNINS);
	CRect		rectButton;
	pButton->GetWindowRect(rectButton);

	// Display the menu
	pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, rectButton.left, rectButton.bottom, this);
	// Delete the menu
	delete pMenu;
}

void
CIcadCusMenu::OnButtonDelete() 
{
	ContextMenuPick(ICAD_CUSMNU_DELETE);	
}
	
void
CIcadCusMenu::OnButtonRename() 
{
	ContextMenuPick(ICAD_CUSMNU_RENAME);	
}

void
CIcadCusMenu::OnKillFocusCommandEdit() 
{
	CTreeCtrl*	pTreeCtrl = (CTreeCtrl*)GetDlgItem(CUS_MENU_TREE);
	SaveCommandToItem(pTreeCtrl->GetSelectedItem());
}

void
CIcadCusMenu::SaveCommandToItem
(
 HTREEITEM	hItem
)
{
	if(!hItem)
		return;

	CTreeCtrl*	pTreeCtrl = (CTreeCtrl*)GetDlgItem(CUS_MENU_TREE);
	CEdit*		pEditCmd = (CEdit*)GetDlgItem(CUS_MENU_EDITCMD);

	if(!pEditCmd->IsWindowEnabled())
		return;

	BOOL	bTearOffName = FALSE;
	CEdit*	pEditHelp = (CEdit*)GetDlgItem(CUS_MENU_EDITHELPSTR);

	if(!pEditHelp->IsWindowEnabled())
		//*** If the help string editbox is grayed out, assume tear-off.
		bTearOffName = TRUE;

	// Get the pointer to the menu item info.
	LPICMENUITEM	lpMenuItem = (LPICMENUITEM)pTreeCtrl->GetItemData(hItem);
	if(!lpMenuItem)
		return;

	// Get the command string from the editbox.
	CString	str;
	pEditCmd->GetWindowText(str);

	// Modify the tear-off name.
	if( bTearOffName &&
		(str.Compare(lpMenuItem->tearOffName) || (lpMenuItem->tearOffName.IsEmpty() && !str.IsEmpty())) )
	{
		lpMenuItem->tearOffName = str;
		m_pCurMenu->m_bMenuModified = TRUE;
		return;
	}

	// Allocate and set the new data to the item.
	if(str.Compare(lpMenuItem->command) || (lpMenuItem->command.IsEmpty() && !str.IsEmpty()))
	{
		lpMenuItem->command = str;
		m_pCurMenu->m_bMenuModified = TRUE;
	}

	// Get the help string from the editbox.
	pEditHelp->GetWindowText(str);

	// Allocate and set the new data to the item.
	if(str.Compare(lpMenuItem->helpString) || (lpMenuItem->helpString.IsEmpty() && !str.IsEmpty()))
	{
		lpMenuItem->helpString = str;
		m_pCurMenu->m_bMenuModified = TRUE;
	}
}

void
CIcadCusMenu::OnAddCommandToEdit() 
{
	// If the Add Command button is not enabled return.
	CButton*	pBtnAdd = (CButton*)GetDlgItem(CUS_MENU_BTNADD);
	if(!pBtnAdd->IsWindowEnabled())
		return;

	// Get the command string from the pointer to the IcadCommand struct stored with the list item.
	CListBox*	pList = (CListBox*)GetDlgItem(CUS_MENU_LIST);
	CString		strCommand((LPCTSTR)pList->GetItemDataPtr(pList->GetCurSel()));

	// Add the string to the editbox.
	AddCommandToEdit(strCommand);
}

void
CIcadCusMenu::AddCommandToEdit
(
 LPCTSTR	strCommand
)
{
	// Add the string to the editbox.
	CEdit*	pEdit = (CEdit*)GetDlgItem(CUS_MENU_EDITCMD);
	if(!pEdit->IsWindowEnabled())
		return;

	CString	strEdit;
	pEdit->GetWindowText(strEdit);
	strEdit += strCommand;
	pEdit->SetWindowText(strEdit);
}

void
CIcadCusMenu::OnDeleteTreeItem
(
 NMHDR*		pNMHDR,
 LRESULT*	pResult
)
{
	*pResult = 0;

	NM_TREEVIEW*	pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	LPICMENUITEM	lpMenuItem = (LPICMENUITEM)pNMTreeView->itemOld.lParam;

	delete lpMenuItem;
}

void
CIcadCusMenu::OnExit() 
{
	// The original menu was grayed-out.  This call will restore it.
	if(m_pCurMenu)
		m_pCurMenu->EnableAllItems();

	// If the dialog was never initialized return.
	if(m_hWnd)
	{
		// Save any changes to the command and help strings.
		CTreeCtrl*	pTreeCtrl = (CTreeCtrl*)GetDlgItem(CUS_MENU_TREE);
		if(pTreeCtrl)
		{
			HTREEITEM	hItem = pTreeCtrl->GetSelectedItem();
			SaveCommandToItem(hItem);

			if(m_pCurMenu->m_bMenuModified)
			{
				// Delete the old menu array.
				m_pCurMenu->DeleteMenuArray();
				// Create a new menu array.
				CreateCustomMenuArray();
			}
		}
	}

	// Re-load menu.
	CMainWindow*	pMain = SDS_CMainWindow;
	if(pMain)
	{
//		pMain->IcadWndAction(ICAD_WNDACTION_NEWMENUS);	// Commented out because it was updating the toolbars also.
		// Update the menu.
		CIcadMenu*	pIcadMenu;
		if(pMain->m_pMenuMain && !!(pIcadMenu = pMain->m_pMenuMain->GetCurrentMenu()) && pIcadMenu->UpdateVisibility())
			pIcadMenu->SetMenu();
	}
}

void
CIcadCusMenu::CreateCustomMenuArray()
{
	CTreeCtrl*	pTreeCtrl = (CTreeCtrl*)GetDlgItem(CUS_MENU_TREE);
	if(!pTreeCtrl || !m_pCurMenu)
		return;

	// Get a count of the number of menu items MINUS SPACERS.
	m_pCurMenu->m_nMenuItems = GetTreeCount();
	m_pCurMenu->m_pMenuArray = new ICMENUITEM[m_pCurMenu->m_nMenuItems];

	LPICMENUITEM	lpMenuArray = m_pCurMenu->m_pMenuArray,
					lpMenuItem;

	// Fill the elements of the array.
	int			ct;
	HTREEITEM	hCurItem = pTreeCtrl->GetRootItem();

	for(ct = 0; hCurItem && ct < m_pCurMenu->m_nMenuItems; ++ct)
	{
		lpMenuItem = (LPICMENUITEM)pTreeCtrl->GetItemData(hCurItem);
		CopyMenuItem(&(lpMenuArray[ct]), lpMenuItem);
		CreateCustomSubMenuItems(hCurItem, &ct);
		hCurItem = pTreeCtrl->GetNextSiblingItem(hCurItem);
	}
}

void
CIcadCusMenu::CreateCustomSubMenuItems
(
 HTREEITEM	hParentItem,
 int*		idxCurArrayItem
)
{
	CMainWindow*	pMain = SDS_CMainWindow;
	CTreeCtrl*		pTreeCtrl = (CTreeCtrl*)GetDlgItem(CUS_MENU_TREE);

	if(!pTreeCtrl || !m_pCurMenu)
		return;

	LPICMENUITEM	lpMenuArray = m_pCurMenu->m_pMenuArray;

	if(!lpMenuArray)
		return;

	int			ct = *idxCurArrayItem;
	HTREEITEM	hCurItem = pTreeCtrl->GetChildItem(hParentItem);

	// Fill the array items
	BOOL			bAddSpacerBefore = FALSE;
	LPICMENUITEM	lpMenuItem;

	while(hCurItem && ct < m_pCurMenu->m_nMenuItems)
	{
		lpMenuItem = (LPICMENUITEM)pTreeCtrl->GetItemData(hCurItem);

		// The item is a spacer.
		if(!lpMenuItem)
		{
			bAddSpacerBefore = TRUE;
			HTREEITEM	hPrevItem = hCurItem;
			hCurItem = pTreeCtrl->GetNextSiblingItem(hCurItem);

			if(!hCurItem)
			{
				// This code will only be reached if the submenu ends on a spacer.
				++ct;
				HTREEITEM	hParentItem = pTreeCtrl->GetParentItem(hPrevItem);
				lpMenuItem = (LPICMENUITEM)pTreeCtrl->GetItemData(hParentItem);

				lpMenuArray[ct].itemName = ICAD_ENDSPACERSTR;

				lpMenuArray[ct].lflgVisibility = ICAD_MN_ALL;

				if(lpMenuItem)
				{
					lpMenuArray[ct].lflgVisibility = lpMenuItem->lflgVisibility;
					lpMenuArray[ct].lflgEntVis = lpMenuItem->lflgEntVis;
					lpMenuArray[ct].nSubLevel = lpMenuItem->nSubLevel + 1;

					// VT 28-6-2002. Ignorecase. Reason: Fix for bug 78223 from BugZilla
					//*** Set the hidden flag if the parent has a tear-off name other than POPn.
					if(!lpMenuItem->tearOffName.IsEmpty() && lpMenuItem->tearOffName.Left(3).CompareNoCase("pop"/*DNT*/))
						lpMenuArray[ct].lflgVisibility |= ICAD_MN_HIDE;
				}

				lpMenuArray[ct].bAddSpacerBefore = TRUE;
				bAddSpacerBefore = FALSE;
			}
			continue;
		}

		if(!lpMenuItem->itemName.IsEmpty())
		{
			// Convert \\t characters to tab characters(\t).
			char*	pszName = NULL;
			int		nLen = ic_escconv((char*)(LPCTSTR)lpMenuItem->itemName, NULL, '\\', ResourceString(IDC_ICADCUSMENU_T_T_4, "t\t"), 0, 0);

			if(nLen > -1)
			{
				pszName = new char[nLen+1];
				ic_escconv((char*)(LPCTSTR)lpMenuItem->itemName, pszName, '\\', ResourceString(IDC_ICADCUSMENU_T_T_4, "t\t"), 0, 0);
			}

			lpMenuItem->itemName = pszName;
			delete [] pszName;
		}

		CopyMenuItem(&(lpMenuArray[++ct]), lpMenuItem);

		lpMenuArray[ct].bAddSpacerBefore = bAddSpacerBefore;
		bAddSpacerBefore = FALSE;

		// VT 28-6-2002. Ignorecase. Reason: Fix for bug 78223 from BugZilla
		// The item may contain a tear-off name.
		if(!lpMenuItem->tearOffName.IsEmpty() && lpMenuItem->tearOffName.Left(3).CompareNoCase("pop"/*DNT*/))
			// Set the hidden flag if the parent has a tear-off name other than POPn.
			lpMenuArray[ct].lflgVisibility |= ICAD_MN_HIDE;

		// If the item has a sub-submenu
		if(pTreeCtrl->ItemHasChildren(hCurItem))
			// Call this function recursively.
			CreateCustomSubMenuItems(hCurItem,&ct);

		hCurItem = pTreeCtrl->GetNextSiblingItem(hCurItem);
	}

	// Set the current array item counter for the calling function.
	*idxCurArrayItem = ct;
}

UINT
CIcadCusMenu::GetTreeCount()
{
// This function gets a count of the number of items in the tree control
// that are NOT spacers.  Useful when setting up the custom menu array.

	CTreeCtrl*	pTreeCtrl = (CTreeCtrl*)GetDlgItem(CUS_MENU_TREE);
	UINT		ct = 0;
	LPICMENUITEM	lpMenuItem;
	HTREEITEM	hCurItem = pTreeCtrl->GetRootItem();

	while(hCurItem)
	{
		lpMenuItem = (LPICMENUITEM)pTreeCtrl->GetItemData(hCurItem);
		if(lpMenuItem)
			++ct;
		ct += GetTreeSubItemCount(hCurItem);
		hCurItem = pTreeCtrl->GetNextSiblingItem(hCurItem);
	}
	return ct;
}

UINT
CIcadCusMenu::GetTreeSubItemCount
(
 HTREEITEM	hParentItem
)
{
	UINT		ct = 0;
	CTreeCtrl*	pTreeCtrl = (CTreeCtrl*)GetDlgItem(CUS_MENU_TREE);

	if(!pTreeCtrl)
		return 0;

	HTREEITEM	hCurItem = pTreeCtrl->GetChildItem(hParentItem);

	//* Fill the array items
	LPICMENUITEM	lpMenuItem;

	while(hCurItem)
	{
		lpMenuItem = (LPICMENUITEM)pTreeCtrl->GetItemData(hCurItem);
		// The item is a spacer.
		if(lpMenuItem || !pTreeCtrl->GetNextSiblingItem(hCurItem))
			++ct;

		// If the item has a sub-submenu
		if(pTreeCtrl->ItemHasChildren(hCurItem))
			// Call this function recursively.
			ct += GetTreeSubItemCount(hCurItem);

		hCurItem = pTreeCtrl->GetNextSiblingItem(hCurItem);
	}
	return ct;
}

void
CIcadCusMenu::OnBeginDragMenuTree
(
 NMHDR*		pNMHDR,
 LRESULT*	pResult
)
{
	NM_TREEVIEW*	pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	// If the item was set for a cut or copy operation, clear it.
	if(m_TreeCtrl.m_hCutItem)
	{
		m_TreeCtrl.SetItemImage(m_TreeCtrl.m_hCutItem, m_TreeCtrl.m_nOrigImage, m_TreeCtrl.m_nOrigImage);
		m_TreeCtrl.m_hCutItem = NULL;
		m_TreeCtrl.m_nOrigImage = -1;
	}

	m_TreeCtrl.m_hCopyItem = NULL;

	// Begin the dragging of an item.
	m_pDragImage = m_TreeCtrl.CreateDragImage(pNMTreeView->itemNew.hItem);
	if(m_pDragImage)
	{
		m_hOrigDragItem = m_hCurDragTarget = pNMTreeView->itemNew.hItem;
		m_bCurDragItemHasKids = m_TreeCtrl.ItemHasChildren(m_hCurDragTarget);

		IMAGEINFO	ii;
		m_pDragImage->GetImageInfo(0, &ii);
		CPoint		pt(0, ii.rcImage.bottom * 3 / 4);

		m_pDragImage->BeginDrag(0, pt);
		pt = pNMTreeView->ptDrag;
		ClientToScreen(&pt);
		m_TreeCtrl.ScreenToClient(&pt);
		m_TreeCtrl.SelectItem(m_hCurDragTarget);
		m_pDragImage->DragEnter((CWnd*)&m_TreeCtrl, pt);
		SetCapture();
	}
	*pResult = 0;
}

void
CIcadCusMenu::OnMouseMove
(
 UINT	nFlags,
 CPoint	point
)
{
	if(m_pDragImage)
	{
		// Drag the menu item.
		CTreeCtrl*	pTreeCtrl = (CTreeCtrl*)GetDlgItem(CUS_MENU_TREE);

		// Transform the point to client coordinates of the tree control.
		CPoint	pt(point);
		ClientToScreen(&pt);
		pTreeCtrl->ScreenToClient(&pt);

		// Draw the menu item.
		m_pDragImage->DragMove(pt);
		m_pDragImage->DragShowNolock(FALSE);

		// Hilight the drop target.
		UINT		flags = TVHT_ONITEM;
		HTREEITEM	hCurItem = pTreeCtrl->HitTest(pt, &flags);

		if(hCurItem != m_hCurDragTarget)
		{
			pTreeCtrl->SelectDropTarget(NULL);
			BOOL	bCurItemIsChild = FALSE;
			if(m_bCurDragItemHasKids)
			{
				// Check to see if we are dragging over a child.
				HTREEITEM	hParentItem = hCurItem;
				while(!!(hParentItem = pTreeCtrl->GetParentItem(hParentItem)))
				{
					if(hParentItem == m_hOrigDragItem)
					{
						bCurItemIsChild = TRUE;
						break;
					}
				}
			}

			if( ((pTreeCtrl->GetParentItem(m_hOrigDragItem) && !m_bCurDragItemHasKids) &&
				!pTreeCtrl->GetParentItem(hCurItem)) ||
				(m_bCurDragItemHasKids && bCurItemIsChild) )
			{
				// If the item we are dragging doensn't have children, and the
				// item we are dragging over is a root item, display the no
				// drop cursor.
				if(!m_hPrevCursor)
					m_hPrevCursor = ::SetCursor(::LoadCursor(NULL, IDC_NO));
			}
			else if(m_hPrevCursor)
			{
				::SetCursor(m_hPrevCursor);
				m_hPrevCursor = NULL;
			}

			m_hCurDragTarget = hCurItem;
			pTreeCtrl->SelectDropTarget(hCurItem);
		}

		m_pDragImage->DragShowNolock(TRUE);
	}

	CPropertyPage::OnMouseMove(nFlags, point);
}

void
CIcadCusMenu::OnLButtonUp
(
 UINT	nFlags,
 CPoint	point
)
{
	if(m_pDragImage)
	{
		// End the dragging of a menu item.
		m_pDragImage->EndDrag();
		delete m_pDragImage;
		m_pDragImage = NULL;
		m_hCurDragTarget = NULL;
		ReleaseCapture();

		if(m_hPrevCursor)
			::SetCursor(m_hPrevCursor);

		// Transform the point to client coordinates of the tree control.
		CTreeCtrl*	pTreeCtrl = (CTreeCtrl*)GetDlgItem(CUS_MENU_TREE);
		CPoint		pt(point);
		ClientToScreen(&pt);
		pTreeCtrl->ScreenToClient(&pt);

		// Get the drop target item.
		UINT		flags = TVHT_ONITEM;
		HTREEITEM	hDropTarget = pTreeCtrl->HitTest(pt,&flags);

		if(hDropTarget)
		{
			BOOL	bCurItemIsChild = FALSE;
			if(m_bCurDragItemHasKids)
			{
				// Check to see if we are dragging over a child.
				HTREEITEM	hParentItem = hDropTarget;
				while(!!(hParentItem = pTreeCtrl->GetParentItem(hParentItem)))
				{
					if(hParentItem == m_hOrigDragItem)
					{
						bCurItemIsChild = TRUE;
						break;
					}
				}
			}

			pTreeCtrl->SelectDropTarget(NULL);

			if( ((pTreeCtrl->GetParentItem(m_hOrigDragItem) && !m_bCurDragItemHasKids) &&
				!pTreeCtrl->GetParentItem(hDropTarget)) ||
				(m_bCurDragItemHasKids && bCurItemIsChild) || m_hOrigDragItem==hDropTarget )
			{
				pTreeCtrl->SelectItem(m_hOrigDragItem);
			}
			else
			{
				// Copy the original item to the position above the drop target.
				HTREEITEM	hNewItem = CopyTreeItem(pTreeCtrl->GetPrevSiblingItem(hDropTarget), m_hOrigDragItem, pTreeCtrl->GetParentItem(hDropTarget));

				// Delete the original item (and it's children).
				DeleteTreeItem(m_hOrigDragItem);
				pTreeCtrl->SelectItem(hNewItem);
				m_hOrigDragItem = NULL;
			}
		}
	}

	CPropertyPage::OnLButtonUp(nFlags, point);
}

HTREEITEM
CIcadCusMenu::CopyTreeItem
(
 HTREEITEM	hInsertAfter,
 HTREEITEM	hOrigItem,
 HTREEITEM	hParentItem
)
{
	TV_INSERTSTRUCT	insert;
	memset(&insert, 0, sizeof(TV_INSERTSTRUCT));

	// Set the item's parent.
	insert.hParent = hParentItem;

	// Figure out where to insert the new item.
	insert.hInsertAfter = hInsertAfter;
	if(!insert.hInsertAfter)
		insert.hInsertAfter = TVI_FIRST;

	// Setup the TV_ITEM struct.
	insert.item.hItem = hOrigItem;
	insert.item.mask = (TVIF_CHILDREN | TVIF_HANDLE | TVIF_IMAGE | TVIF_PARAM | TVIF_SELECTEDIMAGE | TVIF_STATE);

	CTreeCtrl*	pTreeCtrl = (CTreeCtrl*)GetDlgItem(CUS_MENU_TREE);
	pTreeCtrl->GetItem(&insert.item);

	// Copy the MENUITEM structure (if there is one).
	LPICMENUITEM	lpNewMenuItem = NULL,
					lpOrigMenuItem = (LPICMENUITEM)insert.item.lParam;

	if(lpOrigMenuItem)
	{
		lpNewMenuItem = new ICMENUITEM;
		CopyMenuItem(lpNewMenuItem, lpOrigMenuItem);

		// Set the nSubLevel member (it may have changed).
		short		nCurSubLevel = 0;
		HTREEITEM	hTmp = hParentItem;

		while(hTmp)
		{
			++nCurSubLevel;
			hTmp=pTreeCtrl->GetParentItem(hTmp);
		}
		lpNewMenuItem->nSubLevel = nCurSubLevel;
	}

	insert.item.lParam = (LPARAM)lpNewMenuItem;

	// Insert the item.
	HTREEITEM	hNewItem = pTreeCtrl->InsertItem(&insert);
	pTreeCtrl->SetItemText(hNewItem, pTreeCtrl->GetItemText(hOrigItem));

	if(pTreeCtrl->ItemHasChildren(hOrigItem))
	{
		// Loop through the children and copy them to the new position.
		HTREEITEM	hItem = pTreeCtrl->GetChildItem(hOrigItem),
					hInsertAfter = NULL;

		while(hItem)
		{
			hInsertAfter = CopyTreeItem(hInsertAfter, hItem, hNewItem);
			hItem = pTreeCtrl->GetNextSiblingItem(hItem);
		}
	}

	return hNewItem;
}

void
CIcadCusMenu::OnButtonAdvanced()
{
	CTreeCtrl*	pTreeCtrl = (CTreeCtrl*)GetDlgItem(CUS_MENU_TREE);
	HTREEITEM	hItem = pTreeCtrl->GetSelectedItem();

	if(!hItem)
	{
		sds_alert(ResourceString(IDC_ICADCUSMENU_AN_ITEM_HAS_23, "An item has not yet been selected."));
		return;
	}

	LPICMENUITEM		lpMenuItem = (LPICMENUITEM)pTreeCtrl->GetItemData(hItem);
	CIcadCusMenuAdvDlg	dlgAdv(this, lpMenuItem);
	dlgAdv.DoModal();
}

BOOL
CIcadCusMenu::OnSetActive()
{
	BOOL	bRet = CPropertyPage::OnSetActive();
	if(m_bOKtoWarn)
		WarnUser();

	return bRet;
}

void
CIcadCusMenu::WarnUser()
{
	CMainWindow*	pMain = (CMainWindow*)SDS_CMainWindow;

	// Display a warning about auto loaded menus.
	if(pMain && m_pCurMenu && !m_pCurMenu->m_bDisplayedWarning && pMain->m_pMenuMain)
	{
		if(m_pCurMenu != pMain->m_pMenuMain->GetDefaultMenu())
		{
			MessageBox(ResourceString(IDC_ICADCUSMENU_THE_CURRENT_55, "The current menu was auto loaded when the drawing was opened.\nAny changes made to this menu will be lost when the drawing is\nclosed."),
					   ResourceString(IDC_ICADCUSMENU_WARNING_58, "Warning"),
					   MB_ICONWARNING | MB_OK);
			m_pCurMenu->m_bDisplayedWarning = TRUE;
		}
	}
}

void
CIcadCusMenu::OnReset()
{
	CMainWindow*	pMain = (CMainWindow*)SDS_CMainWindow;

	if(!pMain || !m_pCurMenu)
		return;

	((CTreeCtrl*)GetDlgItem(CUS_MENU_TREE))->DeleteAllItems();

	if(m_pCurMenu != pMain->m_pMenuMain->GetDefaultMenu())
	{
		// Set this menu to the default menu.
		if(!m_pCurMenu->Release())
			// If the menu wasn't released (because it is referenced by another drawing),
			// make sure it gets set back to pre-customize condition.
			m_pCurMenu->UpdateVisibility();

		m_pCurMenu = pMain->m_pMenuMain->GetDefaultMenu();

		// Update the pointer in the active document.
		CIcadChildWnd*	pChild = (CIcadChildWnd*)pMain->MDIGetActive(NULL);
		if(pChild)
		{
			CIcadDoc*	pDoc = (CIcadDoc*)(pChild->GetActiveDocument());
			if(pDoc)
				pDoc->m_pIcadMenu = m_pCurMenu;
		}
		pMain->m_pMenuMain->SetCurrentMenu(m_pCurMenu);
	}
	else
	{
		// This is the default menu, so reset it from the registry.
		m_pCurMenu->DeleteMenuArray();
		m_pCurMenu->CreateDefaultMenuArray();
		m_pCurMenu->m_strMenuFile.Empty();
		m_pCurMenu->m_bMenuModified = TRUE;
		m_strLoadLisp.Empty();
	}

	int	nMenuItems = m_pCurMenu->m_nMenuItems,
		ct;

	for(ct = 0; ct < nMenuItems; ++ct)
		AddMenuItemsToTree(&ct, NULL);

	CEdit*	pEdit = (CEdit*)GetDlgItem(CUS_MENU_EDITCMD);

	if(pEdit)
		pEdit->SetWindowText(""/*DNT*/);
}
