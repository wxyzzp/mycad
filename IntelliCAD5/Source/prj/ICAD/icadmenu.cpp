/* File  : <DevDir>\source\prj\icad\IcadMenu.cpp
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 * Handling of all Menu customization - MNU files, ICM files etc.
 */

#include "Icad.h"/*DNT*/
#include "IcadChildWnd.h"/*DNT*/
#include "IcadCmdBar.h"/*DNT*/
#include "cmds.h"/*DNT*/
#include "IcadSplash.h"/*DNT*/
#include "IcadToolbarMain.h"/*DNT*/
#include "IcadCustom.h"/*DNT*/
#include "IcadMenu.h"/*DNT*/
#include "IcadApp.h"/*DNT*/
#include "IcadCusToolbar.h"/*DNT*/
#include "IcadMenuMain.h"/*DNT*/
#include "IcadView.h"/*DNT*/
#include "Preferences.h"/*DNT*/
#include "LicensedComponents.h"/*DNT*/

/////////////////////////////////////////////////////////////////////
//	Globals
/////////////////////////////////////////////////////////////////////

extern int			g_nMenus;			// Global value for number of menus
extern int			g_nMnuTbItems;		// Global value for number of commands
extern IcadMenus	g_IcadMenus[];		// Global array of menu definitions
extern IcadCommands	g_MnuTbItems[];		// Global command structure array.
extern CMainWindow*	SDS_CMainWindow;	// Global pointer to CMainWindow object

//This used to be ICAD_MN_ALL;  We switched to these new settings, so that menus of SDS apps
//would not be visible via OLE, or when no docs were open.  This was done mainly for ArchT.
const int g_DefaultMenuVisibility = ICAD_MN_LEV | ICAD_MN_OPN | ICAD_MN_CLI;  // old setting ICAD_MN_ALL;


/////////////////////////////////////////////////////////////////////
// CIcadMenu
/////////////////////////////////////////////////////////////////////

CIcadMenu::CIcadMenu
(
 CIcadApp*		pApp,
 CMainWindow*	pMain
) : m_pApp(pApp), m_pMain(pMain),
	m_pMenuArray(NULL), m_nMenuItems(0), m_idxModifyMenu(-1), m_idxEsnapMenu(-1), m_hMenu(NULL),
	m_bMenuModified(FALSE), m_bDisplayedWarning(FALSE), m_bIsEmbedMenu(FALSE), m_ctRef(1)

{
}

CIcadMenu::~CIcadMenu()
{
	FreeLoadedMenuList();
	DeleteMenuArray();
	DestroyIcadMenu(m_hMenu);
}

void
CIcadMenu::AddRef()
{
	// TODO - Decrement the reference count on how many documents reference this menu.
	++m_ctRef;
}

BOOL
CIcadMenu::Release()
{
	// Decrement the reference count on how many documents reference this menu.
	--m_ctRef;
	if(!m_ctRef && m_pMain && m_pMain->m_pMenuMain)
	{
		// If this was the last reference, delete this menu from the list.
		m_pMain->m_pMenuMain->RemoveFromList(this);		// NOTE: Must return immediately, will delete this.
		return TRUE;
	}
	return FALSE;
}

void
CIcadMenu::DeleteMenuArray()
{
	delete [] m_pMenuArray;
	m_pMenuArray = NULL;
}

bool
CIcadMenu::DestroyIcadMenu
(
 HMENU	hMenu
)
{
	return true;
/*
// ICAD does not destroy the menu attached to the frame automatically.
// All other detached menus should be destroyed.
// I will turn off the below as it seems to pass an incorrect handle to ::DestroyMenu().
	if (hMenu)
		{
		HMENU hSubMenu;
		int nSubMenuCount = ::GetMenuItemCount(hMenu);
		if (nSubMenuCount != -1)
			for (int nSubMenuIndex=0; nSubMenuIndex<nSubMenuCount; nSubMenuIndex++)
				{
				hSubMenu = ::GetSubMenu(hMenu, nSubMenuIndex);
				if (hSubMenu)
					DestroyIcadMenu(hSubMenu);
				}
		::DestroyMenu(hMenu);
		return true;
		}
	else
		return false;
*/
}

BOOL
CIcadMenu::LoadMenu
(
 LPCTSTR	pszMenuFile,
 BOOL		bAppend
)
{
// Passing NULL to pszMenuFile will load the menu saved in the registry
// (if it exists), if it doesn't exist the default menu is loaded.
	if(!pszMenuFile)
	{
	if(!CreateMenuArrayFromReg())
			CreateDefaultMenuArray();
	}
	else
	{
		m_strMenuFile = pszMenuFile;
		// Create the menu from file.
		if(m_strMenuFile.Find(".icm"/*DNT*/) != -1)
		{
			// Read the menu file (our format).
			if(!LoadFromICM(m_strMenuFile))
			{
				CString	strText;
				strText.Format(ResourceString(IDC_ICADMENU__IS_NOT_A_VALID_2, "The file \" )%s\" is not a valid ICAD Menu file" ), m_strMenuFile);
				AfxMessageBox(strText,NULL,MB_ICONEXCLAMATION);
				return FALSE;
			}
			m_bMenuModified = TRUE;
		}
		else
		{
			LPICMNUINFO	pMnuInfo = MnuToMnuInfo(pszMenuFile, bAppend);
			if(!pMnuInfo)
				return FALSE;
			MnuInfoToMenuArray(pMnuInfo, bAppend);
			FreeMnuInfo(pMnuInfo);
			m_bMenuModified = TRUE;
		}
	}

	UpdateVisibility();
	return TRUE;
}

BOOL
CIcadMenu::CreateMenuArrayFromReg()
{
	PreferenceKey	menuKey(HKEY_CURRENT_USER, "Menu", FALSE);

	// Load the menu items from the registry if they are there, else return FALSE.
	int		nBadRet = 0;
	HKEY	hKeyMenuRoot = menuKey, hKeyMenu = menuKey;

	if(!hKeyMenuRoot)
		return FALSE;

	// Get the number of menu items.
	DWORD	dwRetVal, dwSize = sizeof(int);
	CString	strKey;
	if(RegQueryValueEx(hKeyMenuRoot, "nMenuItems"/*DNT*/, NULL, &dwRetVal, (LPBYTE)&m_nMenuItems, (LPDWORD)&dwSize) != ERROR_SUCCESS)
	{
		nBadRet = (-__LINE__);
		goto badout;
	}

	// Get the list of loaded menu groups
	int		nStrLen, ct;
	for(ct = 1; ; ++ct)
	{
		strKey.Format("UnloadName%d"/*DNT*/, ct);
		if(RegQueryValueEx(hKeyMenuRoot, strKey, NULL, &dwRetVal, NULL, (LPDWORD)&nStrLen) == ERROR_SUCCESS)
		{
			char*	pszUnloadName = new char[++nStrLen];
			memset(pszUnloadName, 0, nStrLen);
			RegQueryValueEx(hKeyMenuRoot, strKey, NULL, &dwRetVal, (LPBYTE)pszUnloadName, (LPDWORD)&nStrLen);
			strKey.Format("FullPathName%d"/*DNT*/, ct);

			char*	pszFullPathName = NULL;
			if(RegQueryValueEx(hKeyMenuRoot, strKey, NULL, &dwRetVal, NULL, (LPDWORD)&nStrLen) == ERROR_SUCCESS)
			{
				pszFullPathName = new char[++nStrLen];
				RegQueryValueEx(hKeyMenuRoot, strKey, NULL, &dwRetVal, (LPBYTE)pszFullPathName, (LPDWORD)&nStrLen);
			}
			CString	strFullPathName = ""/*DNT*/;
			if(pszFullPathName)
				strFullPathName = pszFullPathName;
			AddMenuToLoadedMenuList(pszUnloadName, strFullPathName);

			delete [] pszFullPathName;
			delete [] pszUnloadName;
			continue;
		}
		break;
	}

	// If the menu array is not empty, empty it.
	DeleteMenuArray();

	// Create the menu array.
	LPTSTR	pTmpStr;
	m_pMenuArray = new ICMENUITEM[m_nMenuItems];

  	for(ct = 0; ct < m_nMenuItems; ++ct)
	{
		// Get the menu key
		hKeyMenu = menuKey;
		strKey.Format("MnuItem-%d"/*DNT*/, ct);
		if(RegOpenKeyEx(hKeyMenu, strKey, NULL, KEY_READ, &hKeyMenu) != ERROR_SUCCESS)
			continue;

		if(RegQueryValueEx(hKeyMenu, "Name"/*DNT*/, NULL, &dwRetVal, NULL, (LPDWORD)&nStrLen) == ERROR_SUCCESS)
		{
			pTmpStr = new char [++nStrLen];
			memset(pTmpStr,0,nStrLen);
			RegQueryValueEx(hKeyMenu, "Name"/*DNT*/, NULL, &dwRetVal, (LPBYTE)pTmpStr, (LPDWORD)&nStrLen);
			m_pMenuArray[ct].itemName = pTmpStr;
			IC_FREE(pTmpStr);
		}
		if(RegQueryValueEx(hKeyMenu, "TearOffName"/*DNT*/, NULL, &dwRetVal, NULL, (LPDWORD)&nStrLen) == ERROR_SUCCESS)
		{
			pTmpStr = new char [++nStrLen];
			memset(pTmpStr,0,nStrLen);
			RegQueryValueEx(hKeyMenu, "TearOffName"/*DNT*/, NULL, &dwRetVal, (LPBYTE)pTmpStr, (LPDWORD)&nStrLen);
			m_pMenuArray[ct].tearOffName = pTmpStr;
			IC_FREE(pTmpStr);
		}
		if(RegQueryValueEx(hKeyMenu, "Command"/*DNT*/, NULL, &dwRetVal, NULL, (LPDWORD)&nStrLen) == ERROR_SUCCESS)
		{
			pTmpStr = new char [++nStrLen];
			memset(pTmpStr,0,nStrLen);
			RegQueryValueEx(hKeyMenu, "Command"/*DNT*/, NULL, &dwRetVal, (LPBYTE)pTmpStr, (LPDWORD)&nStrLen);
			m_pMenuArray[ct].command = pTmpStr;
			IC_FREE(pTmpStr);
		}
		if(RegQueryValueEx(hKeyMenu, "HelpString"/*DNT*/, NULL, &dwRetVal, NULL, (LPDWORD)&nStrLen) == ERROR_SUCCESS)
		{
			pTmpStr = new char [++nStrLen];
			memset(pTmpStr,0,nStrLen);
			RegQueryValueEx(hKeyMenu, "HelpString"/*DNT*/, NULL, &dwRetVal, (LPBYTE)pTmpStr, (LPDWORD)&nStrLen);
			m_pMenuArray[ct].helpString = pTmpStr;
			IC_FREE(pTmpStr);
		}
		if(RegQueryValueEx(hKeyMenu, "ChekVar"/*DNT*/, NULL, &dwRetVal, NULL, (LPDWORD)&nStrLen) == ERROR_SUCCESS)
		{
			pTmpStr = new char [++nStrLen];
			memset(pTmpStr,0,nStrLen);
			RegQueryValueEx(hKeyMenu, "ChekVar"/*DNT*/, NULL, &dwRetVal, (LPBYTE)pTmpStr, (LPDWORD)&nStrLen);
			m_pMenuArray[ct].chekVar = pTmpStr;
			IC_FREE(pTmpStr);
		}
		if(RegQueryValueEx(hKeyMenu, "GrayVar"/*DNT*/, NULL, &dwRetVal, NULL, (LPDWORD)&nStrLen) == ERROR_SUCCESS)
		{
			pTmpStr = new char [++nStrLen];
			memset(pTmpStr,0,nStrLen);
			RegQueryValueEx(hKeyMenu, "GrayVar"/*DNT*/, NULL, &dwRetVal, (LPBYTE)pTmpStr, (LPDWORD)&nStrLen);
			m_pMenuArray[ct].grayVar = pTmpStr;
			IC_FREE(pTmpStr);
		}
		if(RegQueryValueEx(hKeyMenu, "FileName"/*DNT*/, NULL, &dwRetVal, NULL, (LPDWORD)&nStrLen) == ERROR_SUCCESS)
		{
			pTmpStr = new char [++nStrLen];
			memset(pTmpStr,0,nStrLen);
			RegQueryValueEx(hKeyMenu, "FileName"/*DNT*/, NULL, &dwRetVal, (LPBYTE)pTmpStr, (LPDWORD)&nStrLen);
			m_pMenuArray[ct].fileName = pTmpStr;
			IC_FREE(pTmpStr);
		}
		if(RegQueryValueEx(hKeyMenu, "Visibility"/*DNT*/, NULL, &dwRetVal, (LPBYTE)&(m_pMenuArray[ct].lflgVisibility), (LPDWORD)&dwSize) != ERROR_SUCCESS)
			m_pMenuArray[ct].lflgVisibility = 0L;
		if(RegQueryValueEx(hKeyMenu, "EntityVisibility"/*DNT*/, NULL, &dwRetVal, (LPBYTE)&(m_pMenuArray[ct].lflgEntVis), (LPDWORD)&dwSize) != ERROR_SUCCESS)
			m_pMenuArray[ct].lflgEntVis = 0L;
		dwSize = sizeof(short);
		if(RegQueryValueEx(hKeyMenu, "SubLevel"/*DNT*/, NULL, &dwRetVal, (LPBYTE)&(m_pMenuArray[ct].nSubLevel), (LPDWORD)&dwSize) != ERROR_SUCCESS)
			m_pMenuArray[ct].nSubLevel = 0;
		dwSize = sizeof(BOOL);
		if(RegQueryValueEx(hKeyMenu, "AddSpacerBefore"/*DNT*/, NULL, &dwRetVal, (LPBYTE)&(m_pMenuArray[ct].bAddSpacerBefore), (LPDWORD)&dwSize) != ERROR_SUCCESS)
			m_pMenuArray[ct].bAddSpacerBefore = FALSE;
		RegCloseKey(hKeyMenu);
	}	// for( thru menuitems

	return TRUE;

badout:
	CString	str;
	str.Format(ResourceString(IDC_ICADMENU_ERROR_CREATING_14, "Error creating menu (%d). Default menu will be loaded." ), (-nBadRet));
	AfxMessageBox(str);
	DeleteMenuArray();
	return FALSE;
}

void
CIcadMenu::CreateDefaultMenuArray()
{
	int			ct;
	IcadMenus*	pIcadMenus = g_IcadMenus;

	// There are no loaded menus in the default menu
	FreeLoadedMenuList();

	// Get a count of the number of total menu items so we can set up the array.
	m_nMenuItems = g_nMenus;
	for(ct = 0; ct < g_nMnuTbItems; ++ct)
	{
		// Exclude non-menu items and items without a ToolTip or Command string (spacers).
		if( g_MnuTbItems[ct].lMnuID  &&
			g_MnuTbItems[ct].ToolTip && *g_MnuTbItems[ct].ToolTip &&
			g_MnuTbItems[ct].Command && *g_MnuTbItems[ct].Command )
			++m_nMenuItems;
	}

	m_pMenuArray = new ICMENUITEM[m_nMenuItems];

	// Fill the elements of the array.
	int	ctRecur = 0, ctArrayItem = 0;

	for(ct = 0; ct < g_nMenus; ++ct)
	{
		if(pIcadMenus[ct].IsSubMenu)
			continue;
		m_pMenuArray[ctArrayItem].itemName.LoadString(ICADMENUS + ct);
		m_pMenuArray[ctArrayItem].tearOffName = pIcadMenus[ct].globalName;

		++ctArrayItem;

		CreateDefaultSubMenuItems(pIcadMenus[ct].lMenuID, &ctArrayItem, &ctRecur);
	}
}

int
IcadMenuPositionSort
(
 const void*	cmp1,
 const void*	cmp2
)
{
	return (*(IcadCommands**)cmp1)->Position - (*(IcadCommands**)cmp2)->Position;
}

void
CIcadMenu::CreateDefaultSubMenuItems
(
 long	lMnuID,
 int*	idxCurArrayItem,
 int*	ctRecur
)
{
	int				ct, ctItems, tmpIndex;
    CString			menuItem, toolTip, helpString;
	IcadCommands**	pSortedItems;	// Pointer to the sorted array of IcadCommands structs

	// Get the number of items for this submenu for the sort array.
	for(ct = 0, ctItems = 0; ct < g_nMnuTbItems; ++ct)
	{
		if(g_MnuTbItems[ct].lMnuID == lMnuID)
			++ctItems;
	}
	if(!ctItems)
		return;

	// Allocate and set the sort array
	pSortedItems = new IcadCommands * [ctItems];
	memset(pSortedItems ,0, sizeof(IcadCommands*)*ctItems);
	for(ct = 0, ctItems = 0; ct < g_nMnuTbItems; ++ct)
	{
		if(g_MnuTbItems[ct].lMnuID != lMnuID)
			continue;

		pSortedItems[ctItems] = &g_MnuTbItems[ct];

        tmpIndex = g_MnuTbItems[ct].resourceIndex;
        if(g_MnuTbItems[ct].pAcadTbStr)
		{
            menuItem.LoadString(tmpIndex++);
            pSortedItems[ctItems]->pLocalAcadTbStr = new char[menuItem.GetLength() + 1];
            strcpy(pSortedItems[ctItems]->pLocalAcadTbStr, menuItem);
        }
        if(g_MnuTbItems[ct].ToolTip)
		{
            toolTip.LoadString(tmpIndex++);
            pSortedItems[ctItems]->LocalToolTip = new char[toolTip.GetLength() + 1];
            strcpy(pSortedItems[ctItems]->LocalToolTip, toolTip);
        }
        if(g_MnuTbItems[ct].HelpString)
		{
            helpString.LoadString(tmpIndex);
            pSortedItems[ctItems]->LocalHelpString = new char[helpString.GetLength() + 1];
            strcpy(pSortedItems[ctItems]->LocalHelpString, helpString);
        }
		++ctItems;
	}

	qsort(pSortedItems, ctItems, sizeof(IcadCommands*), IcadMenuPositionSort);

	// Increment the recursion counter.
	++(*ctRecur);

	// Fill the array items
	for(ct = 0; ct < ctItems; ++ct)
	{
		// If the item is a sub-submenu
		if(pSortedItems[ct]->lSubMenuID)
		{
			if(*pSortedItems[ct]->LocalToolTip)
				m_pMenuArray[*idxCurArrayItem].itemName = pSortedItems[ct]->LocalToolTip;
			m_pMenuArray[*idxCurArrayItem].lflgVisibility = pSortedItems[ct]->lflgVisibility;
			m_pMenuArray[*idxCurArrayItem].nSubLevel = (*ctRecur);
 			//look up the popup index
 			for(int i = 0; i < g_nMenus; ++i)
 			{
 				if(g_IcadMenus[i].lMenuID==pSortedItems[ct]->lSubMenuID)
 					m_pMenuArray[*idxCurArrayItem].tearOffName=g_IcadMenus[i].globalName;
 			}

			++(*idxCurArrayItem);
			// Call this function recursively.
			CreateDefaultSubMenuItems(pSortedItems[ct]->lSubMenuID, idxCurArrayItem, ctRecur);
			continue;
		}

		// If the item is a separator
		if( (!pSortedItems[ct]->LocalToolTip || !*pSortedItems[ct]->LocalToolTip) &&
			(!pSortedItems[ct]->Command || !*pSortedItems[ct]->Command) )
		{
			m_pMenuArray[*idxCurArrayItem].bAddSpacerBefore = TRUE;
			continue;
		}

		if(pSortedItems[ct]->LocalToolTip && *pSortedItems[ct]->LocalToolTip)
			m_pMenuArray[*idxCurArrayItem].itemName = pSortedItems[ct]->LocalToolTip;

		if(pSortedItems[ct]->Command && *pSortedItems[ct]->Command)
			m_pMenuArray[*idxCurArrayItem].command = pSortedItems[ct]->Command;

		if(pSortedItems[ct]->LocalHelpString && *pSortedItems[ct]->LocalHelpString)
			m_pMenuArray[*idxCurArrayItem].helpString = pSortedItems[ct]->LocalHelpString;

		if(pSortedItems[ct]->strChekVar && *pSortedItems[ct]->strChekVar)
			m_pMenuArray[*idxCurArrayItem].chekVar = pSortedItems[ct]->strChekVar;

		if(pSortedItems[ct]->strGreyVar && *pSortedItems[ct]->strGreyVar)
			m_pMenuArray[*idxCurArrayItem].grayVar = pSortedItems[ct]->strGreyVar;

		m_pMenuArray[*idxCurArrayItem].lflgVisibility = pSortedItems[ct]->lflgVisibility;
		m_pMenuArray[*idxCurArrayItem].lflgEntVis = pSortedItems[ct]->lflgEntVis;
		m_pMenuArray[*idxCurArrayItem].nSubLevel = (*ctRecur);
		++(*idxCurArrayItem);
	}	// for(

	// Decrement the recursion counter.
	--(*ctRecur);

	// Free everything up and leave
    for(ct = 0; ct < ctItems; ++ct)
	{
        delete [] pSortedItems[ct]->pLocalAcadTbStr;
        delete [] pSortedItems[ct]->LocalToolTip;
        delete [] pSortedItems[ct]->LocalHelpString;
        pSortedItems[ct]->pLocalAcadTbStr = NULL;
        pSortedItems[ct]->LocalToolTip    = NULL;
        pSortedItems[ct]->LocalHelpString = NULL;
    }

	delete [] pSortedItems;
}

void
CIcadMenu::SaveToReg()
{
	PreferenceKey	menuKey(HKEY_CURRENT_USER, "Menu", FALSE);
	HKEY			hKeyMenuRoot = menuKey;
	// Return if the registry exists but the Modified flag has not been set;
	if(hKeyMenuRoot && !m_bMenuModified)
		return;
	menuKey.FlushKey(HKEY_CURRENT_USER, "Menu");
	// Get the registry ready to write to.
	PreferenceKey	MKey(HKEY_CURRENT_USER, "Menu");
	hKeyMenuRoot = MKey;

	DWORD	dwRetVal;
	if(!hKeyMenuRoot)
		return;

	// Write the number of menu items
	RegSetValueEx(hKeyMenuRoot, "nMenuItems"/*DNT*/, 0, REG_DWORD, (CONST BYTE*)&m_nMenuItems, sizeof(int));
	// Write the loaded menu groups
	int		ct;
	CString	strMenu, strValue;
	for(ct = 0; ct < m_theLoadedMenuList.GetSize(); ++ct )
	{
		CIcadLoadedMenuInfo*	pTheInfo = m_theLoadedMenuList[ct];
		if(pTheInfo)
		{
			strValue.Format("UnloadName%d"/*DNT*/, (ct + 1));
			strMenu = pTheInfo->GetGroupName();
			RegSetValueEx(hKeyMenuRoot, strValue, 0, REG_SZ, (CONST BYTE*)strMenu.GetBuffer(0), strMenu.GetLength());
			strValue.Format("FullPathName%d"/*DNT*/, (ct + 1));
			strMenu = pTheInfo->GetPathName();
			RegSetValueEx(hKeyMenuRoot, strValue, 0, REG_SZ, (CONST BYTE*)strMenu.GetBuffer(0), strMenu.GetLength());
		}
	}

	// Loop through the menu array and write the item information to the registry.
	HKEY	hKeyMenu = MKey;
	CString	strKey;
	for(ct = 0; ct < m_nMenuItems; ++ct)
	{
		hKeyMenu = MKey;
		// Create a key for the menu item
		strKey.Format("MnuItem-%d"/*DNT*/, ct);
		EditString	str;
		str = ResourceString(IDC_ICADMENU__15, "");
		if(RegCreateKeyEx(hKeyMenu, strKey, 0, str, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKeyMenu, &dwRetVal) != ERROR_SUCCESS)
			continue;

		// Write the ICMENUITEM structure info.
		if(!m_pMenuArray[ct].itemName.IsEmpty())
			RegSetValueEx(hKeyMenu, "Name"/*DNT*/, 0, REG_SZ, (CONST BYTE*)(LPCTSTR)m_pMenuArray[ct].itemName, m_pMenuArray[ct].itemName.GetLength() + 1);

		if(!m_pMenuArray[ct].tearOffName.IsEmpty())
			RegSetValueEx(hKeyMenu, "TearOffName"/*DNT*/, 0, REG_SZ, (CONST BYTE*)(LPCTSTR)m_pMenuArray[ct].tearOffName, m_pMenuArray[ct].tearOffName.GetLength() + 1);

		if(!m_pMenuArray[ct].command.IsEmpty())
			RegSetValueEx(hKeyMenu, "Command"/*DNT*/, 0, REG_SZ, (CONST BYTE*)(LPCTSTR)m_pMenuArray[ct].command, m_pMenuArray[ct].command.GetLength() + 1);

		if(!m_pMenuArray[ct].helpString.IsEmpty())
			RegSetValueEx(hKeyMenu, "HelpString"/*DNT*/, 0, REG_SZ, (CONST BYTE*)(LPCTSTR)m_pMenuArray[ct].helpString, m_pMenuArray[ct].helpString.GetLength() + 1);

		if(!m_pMenuArray[ct].chekVar.IsEmpty())
			RegSetValueEx(hKeyMenu, "ChekVar"/*DNT*/, 0, REG_SZ, (CONST BYTE*)(LPCTSTR)m_pMenuArray[ct].chekVar, m_pMenuArray[ct].chekVar.GetLength() + 1);

		if(!m_pMenuArray[ct].grayVar.IsEmpty())
			RegSetValueEx(hKeyMenu, "GrayVar"/*DNT*/, 0, REG_SZ, (CONST BYTE*)(LPCTSTR)m_pMenuArray[ct].grayVar, m_pMenuArray[ct].grayVar.GetLength() + 1);

		if(!m_pMenuArray[ct].fileName.IsEmpty())
			RegSetValueEx(hKeyMenu, "FileName"/*DNT*/, 0, REG_SZ, (CONST BYTE*)(LPCTSTR)m_pMenuArray[ct].fileName, m_pMenuArray[ct].fileName.GetLength() + 1);

		if(m_pMenuArray[ct].lflgVisibility)
			RegSetValueEx(hKeyMenu, "Visibility"/*DNT*/, 0, REG_DWORD, (CONST BYTE*)&(m_pMenuArray[ct].lflgVisibility), sizeof(int));

		if(m_pMenuArray[ct].lflgEntVis)
			RegSetValueEx(hKeyMenu, "EntityVisibility"/*DNT*/, 0, REG_DWORD, (CONST BYTE*)&(m_pMenuArray[ct].lflgEntVis), sizeof(int));

		if(m_pMenuArray[ct].nSubLevel)
			RegSetValueEx(hKeyMenu, "SubLevel"/*DNT*/, 0, REG_BINARY, (CONST BYTE*)&(m_pMenuArray[ct].nSubLevel), sizeof(short));

		if(m_pMenuArray[ct].bAddSpacerBefore)
			RegSetValueEx(hKeyMenu, "AddSpacerBefore"/*DNT*/, 0, REG_BINARY, (CONST BYTE*)&(m_pMenuArray[ct].bAddSpacerBefore), sizeof(BOOL));

		RegCloseKey(hKeyMenu);
	}	// for( thru menu items

	m_bMenuModified = FALSE;
}

HMENU
CIcadMenu::MenuCreator
(
 long lflgVisibility
)
{
	if(!m_pMenuArray)
		return NULL;

	int		ct, ct1, idxItem;
	HMENU	hSubMenu;
	int		nMenuArrayItems = m_nMenuItems;

	CMenu*	pMenu = new CMenu();
	pMenu->CreateMenu();

	// Index of the Modify Menu in the menu array (for ICAD_WNDACTION_RCLICK).
	m_idxEsnapMenu = m_idxModifyMenu = -1;

	// Get a count of the number of main menu items for the OLE In-Place Server menu-breaks.
	int		nMenus = 0;
	for(ct = 0; ct < g_nMenus; ++ct)
	{
		if(!g_IcadMenus[ct].IsSubMenu)
			++nMenus;
	}

	// Create the menu.
	for(ct = 0, idxItem = 0; idxItem < nMenuArrayItems; ++ct)
	{
		if(ct == 1 && (lflgVisibility & ICAD_MN_CLI) || (ct == (nMenus - 1) && (lflgVisibility & ICAD_MN_SIP)))
		{
			// Add two separators after the File popup for OLE Client,
			// or before the Help popup for OLE In-Place Server.
			pMenu->AppendMenu(MF_MENUBREAK);
			pMenu->AppendMenu(MF_MENUBREAK);
		}

		if(!m_pMenuArray[idxItem].tearOffName.CompareNoCase("pop0"/*DNT*/))
			// Set the index of the Shift right-click Menu in the menu array - in case it is a main popup menu.
			m_idxEsnapMenu = idxItem;

		if( !m_pMenuArray[idxItem].tearOffName.CompareNoCase("pop17"/*DNT*/) ||
			(!m_pMenuArray[idxItem].itemName.Compare(ResourceString(ICADMENUS_MODIFY, "&Modify")) &&
			  m_idxModifyMenu == -1) )
			// Set the index of the Modify Menu in the menu array (for ICAD_WNDACTION_RCLICK).
			m_idxModifyMenu = idxItem;

		// Create the sub-menu.
		ct1 = idxItem;
		if(!(hSubMenu = SubMenuCreator(&ct1, lflgVisibility, 0L)))
		{
			// Set the hidden flag for this empty submenu's parent.  I set this flag
			// for any submenu that doesn't get displayed for a quick reference to
			// the position of the submenus that do get displayed.  See the code for
			// setting tear-off menus in DoMenuRef().
			m_pMenuArray[idxItem].lflgVisibility |= ICAD_MN_HIDE;
			idxItem = ct1;
			continue;
		}

		// If the hidden flag is set, clear it because this submenu is not hidden.
		if(m_pMenuArray[idxItem].lflgVisibility & ICAD_MN_HIDE)
			m_pMenuArray[idxItem].lflgVisibility &= ~ICAD_MN_HIDE;

		// Create the submenu
		if(!m_pMenuArray[idxItem].itemName.IsEmpty())
		{
			if(!pMenu->AppendMenu(MF_POPUP, (UINT)hSubMenu, m_pMenuArray[idxItem].itemName))
				ASSERT(0);	// Not sure how to handle this case, but leave this here for debugging purposes.
		}
		idxItem = ct1;
	}	// for(

	UpdateCheckState(NULL, pMenu);
	HMENU	hMenu = pMenu->Detach();
	delete pMenu;
	return hMenu;
}

HMENU
CIcadMenu::SubMenuCreator
(
 int*	idxCurArrayItem,
 long	lflgVisibility,
 long	lflgEntVis
)
{
	/*D.G.*/// accessibility licensed components commands checking added

	if((*idxCurArrayItem) + 1 >= m_nMenuItems)
	{
		++(*idxCurArrayItem);
		return NULL;
	}

	int		nMenuArrayItems = m_nMenuItems, ct,ct1;
	short	nCurSubLevel = m_pMenuArray[(*idxCurArrayItem) + 1].nSubLevel;

	if(nCurSubLevel == m_pMenuArray[(*idxCurArrayItem)].nSubLevel)
	{
		++(*idxCurArrayItem);
		return NULL;
	}

	long	origVisibility = lflgVisibility;	//Save this original visibility flag for use in recursion - when creating submenus.
	BOOL	bRightClickOnly = (BOOL)(lflgVisibility & ICAD_MN_RCO);
	lflgVisibility &= ~ICAD_MN_RCO;
	
	BOOL	bTmpOsnapModeVis = (BOOL)(origVisibility & ICAD_MN_TOM);

	// Create the popup menu
	CMenu*	pMenu = new CMenu();
	pMenu->CreatePopupMenu();

	// Add the menu items
	ct = (*idxCurArrayItem) + 1;
	int		nFirstVisible = -1;
	BOOL	bPrevWasVis = TRUE;

	while(ct < nMenuArrayItems && m_pMenuArray[ct].nSubLevel >= nCurSubLevel)
	{
		// Check for accessibility of licensed components commands
		if(!CIcadApp::m_LicensedComponents.IsCommandAccessible(m_pMenuArray[ct].command))
		{
			bPrevWasVis = FALSE;
			++ct;
			continue;
		}

		// If the hidden flag is set, continue.
		if(m_pMenuArray[ct].lflgVisibility & ICAD_MN_HIDE)
		{
			bPrevWasVis = FALSE;
			++ct;
			continue;
		}

		// Check for the Right-Click Only flag.
		if(!bRightClickOnly && (m_pMenuArray[ct].lflgVisibility & ICAD_MN_RCO))
		{
			bPrevWasVis = FALSE;
			++ct;
			continue;
		}

		// Check for the Temporary-Osnap-Mode Only flag.
		if(bTmpOsnapModeVis ^ (m_pMenuArray[ct].lflgVisibility & ICAD_MN_TOM))
		{
			bPrevWasVis = FALSE;
			++ct;
			continue;
		}

		// Check the User Level visibility but ignore the rco flag.
		if(lflgVisibility && m_pMenuArray[ct].lflgVisibility && (lflgVisibility & m_pMenuArray[ct].lflgVisibility) != lflgVisibility)
		{
			bPrevWasVis = FALSE;
			++ct;
			continue;
		}

		// Check the Entity visibility.
		if(lflgEntVis && !(lflgEntVis & m_pMenuArray[ct].lflgEntVis))
		{
			bPrevWasVis = FALSE;
			++ct;
			continue;
		}

		if(lflgEntVis && (lflgEntVis & ICAD_MN_MUL) && (m_pMenuArray[ct].lflgEntVis & ICAD_MN_MUL))
		{
			// Continue if multiple items are selected and this item has the
			// multiple selection exclusion flag set.
			bPrevWasVis = FALSE;
			++ct;
			continue;
		}

		if(nFirstVisible == -1)
			nFirstVisible = ct;

		// If the bAddSpacerBefore flag is set.
		if(m_pMenuArray[ct].bAddSpacerBefore)
		{
			if(!m_pMenuArray[ct].itemName.Compare(ICAD_ENDSPACERSTR))
			{
				pMenu->AppendMenu(MF_SEPARATOR);
				++ct;
				continue;
			}
			else
			{
				// Do not display a spacer as the first item of a submenu if there is
				// an item before it that is not visible.
				if(ct != nFirstVisible || (ct == nFirstVisible && bPrevWasVis))
					pMenu->AppendMenu(MF_SEPARATOR);
			}
		}

		bPrevWasVis = TRUE;

		// If the item has a sub-submenu
		if(ct+1 < nMenuArrayItems  &&  m_pMenuArray[ct+1].nSubLevel == nCurSubLevel + 1)
		{
			// Set the index of the Entity Snap Menu in the menu array (for shift right-click).
			// Modified Cybage MM 05/11/2001 (DD/MM/YYYY)[
			// Reason: Fix for bug no 77908 from BugZilla		
			//if( !m_pMenuArray[ct].tearOffName.CompareNoCase("pop0"/*DNT*/)||
 			if( (!m_pMenuArray[ct].tearOffName.CompareNoCase("pop0"/*DNT*/) && m_pMenuArray[ct].nSubLevel == 0)||
			// Modified Cybage MM 05/11/2001 (DD/MM/YYYY)]
 				(!m_pMenuArray[ct].itemName.Compare(ResourceString(IDC_TBMNU_1008, "&Entity Snap" )) &&
 				m_idxEsnapMenu == -1))
				m_idxEsnapMenu = ct; 
			
			// Call this function recursively.
			ct1 = ct;
			HMENU	hSubMenu = SubMenuCreator(&ct1, origVisibility, lflgEntVis);

			if(!hSubMenu)	/*D.G.*/// This item shouldn't be visible in this case.
			{
				if(m_pMenuArray[ct].bAddSpacerBefore)	// also, remove a spacer in this case
				{
					UINT k = pMenu->GetMenuItemCount();
					pMenu->RemoveMenu(pMenu->GetMenuItemCount() - 1, MF_BYPOSITION);
				}
				ct = ct1;
				continue;
			}

			if(!pMenu->AppendMenu(MF_POPUP, (UINT)hSubMenu, m_pMenuArray[ct].itemName))
				ASSERT(0);		// Not sure how to handle this case, but leave this here for debugging purposes.

			ct = ct1;
			continue;
		}	// if the item has a submenu

		// Otherwise set the style flags and add the item.
		UINT	nFlags = MF_UNCHECKED | MF_STRING;
		CString	str = m_pMenuArray[ct].itemName;
		if(!str.IsEmpty() && str[0] == '~'/*DNT*/)
		{
			nFlags |= MF_GRAYED;
			str.TrimLeft('~'/*DNT*/);
		}
		else
			nFlags |= MF_ENABLED;

		if(!pMenu->AppendMenu(nFlags, (ct + TB_START_ID), str))
			ASSERT(0);		// Not sure how to handle this case, but leave this here for debugging purposes.
		++ct;
	}	// while(

	(*idxCurArrayItem) = ct;

	// Free everything up and leave
	HMENU	hMenu;
	if(pMenu->GetMenuItemCount() <= 0)
		hMenu = NULL;
	else
		hMenu = pMenu->Detach();
	delete pMenu;
	return hMenu;
}

void
CIcadMenu::UpdateCheckState
(
 char*	Setvar,
 CMenu*	pMenu
)
{
	if(!pMenu && m_pMain)
	{
		if(!(pMenu = m_pMain->GetMenu()))
			return;
	}

	if(!m_pMenuArray)
		return;

	int		Count, BitChk, NotChk, InnerCount = 0, charIdx;
	resbuf	rb;
	CString	varName;

	for(Count = 0; Count < m_nMenuItems; ++Count)
	{
		if(!m_pMenuArray[Count].chekVar.IsEmpty())
		{
			varName = m_pMenuArray[Count].chekVar;
			NotChk = varName[0] == '!'/*DNT*/ ? 1 : 0;
			if(NotChk)
				varName.TrimLeft('!'/*DNT*/);

			if((charIdx = varName.Find('&'/*DNT*/)) > 0)
			{
				BitChk = atoi(varName.Right(varName.GetLength() - charIdx - 1));
				varName = varName.Left(charIdx);
			}
			else
				BitChk = 0;

			if((!Setvar || !varName.CompareNoCase(Setvar)) && sds_getvar(varName, &rb) == RTNORM)
			{
				if(BitChk)
				{
					if(NotChk ? !(rb.resval.rint & BitChk) : rb.resval.rint & BitChk)
						pMenu->CheckMenuItem(TB_START_ID + Count, MF_CHECKED);
					else
						pMenu->CheckMenuItem(TB_START_ID + Count, MF_UNCHECKED);
				}
				else
				{
					if(NotChk ? !rb.resval.rint : rb.resval.rint)
						pMenu->CheckMenuItem(TB_START_ID + Count, MF_CHECKED);
					else
						pMenu->CheckMenuItem(TB_START_ID + Count, MF_UNCHECKED);
				}
			}
		}	// chekVar

		if(!m_pMenuArray[Count].grayVar.IsEmpty())
		{
			varName = m_pMenuArray[Count].grayVar;
			NotChk = varName[0] == '!'/*DNT*/ ? 1 : 0;
			if(NotChk)
				varName.TrimLeft('!'/*DNT*/);

			if((charIdx = varName.Find('&'/*DNT*/)) > 0)
			{
				BitChk = atoi(varName.Right(varName.GetLength() - charIdx - 1));
				varName = varName.Left(charIdx);
			}
			else
				BitChk = 0;

			if((!Setvar || !varName.CompareNoCase(Setvar)) && sds_getvar(varName, &rb) == RTNORM)
			{
				if(BitChk)
				{
					if(NotChk ? !(rb.resval.rint & BitChk) : rb.resval.rint & BitChk)
						pMenu->CheckMenuItem(TB_START_ID + Count, MF_ENABLED);
					else
						pMenu->CheckMenuItem(TB_START_ID + Count, MF_GRAYED);
				}
				else
				{
					if(NotChk ? !rb.resval.rint : rb.resval.rint)
						pMenu->CheckMenuItem(TB_START_ID + Count, MF_GRAYED);
					else
						pMenu->CheckMenuItem(TB_START_ID + Count, MF_ENABLED);
				}
			}
		}	// grayVar
	}	// for( thru menu items
}

HMENU
CIcadMenu::UpdateVisibility()
{
	resbuf	rb;
	long	lflgVisibility;

	if(m_pMain->m_nIsClosing)
		return NULL;

	if(m_bIsEmbedMenu)	// This gets set in CIcadDoc::GetDefaultMenu
		lflgVisibility = ICAD_MN_SEM;
	else
	{
		SDS_getvar(NULL, DB_QWNDLMDI, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
		if(!rb.resval.rint)
			lflgVisibility = ICAD_MN_CLS;
		else
			lflgVisibility = ICAD_MN_OPN;

		SDS_getvar(NULL, DB_QEXPLEVEL, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
		switch(rb.resval.rint)
		{
			case 1: lflgVisibility |= ICAD_MN_BEG; break;
			case 2: lflgVisibility |= ICAD_MN_INT; break;
			case 3: lflgVisibility |= ICAD_MN_EXP;
		}
	}

	HMENU	hMenu = MenuCreator(lflgVisibility);
	if(!hMenu)
		return NULL;

	m_hMenu = hMenu;
	CMenu*	pMainMenu = new CMenu;
	pMainMenu->Attach(hMenu);

	// Add the MRU File list to the menu.
	CIcadApp*	pApp = (CIcadApp*)AfxGetApp();
	CMenu*		pFileMenu = pMainMenu->GetSubMenu(0);

	// Only add the list if there is a "File" menu.
	CString		str;
	pMainMenu->GetMenuString(0, str, MF_BYPOSITION);
    if( !m_bIsEmbedMenu && pFileMenu &&
		(!str.CompareNoCase(ResourceString(IDC_ICADMENU_FILE_22, "File")) ||
		 !str.CompareNoCase(ResourceString(IDC_ICADMENU__FILE_23, "&File"))) )
	{
	    int		nInsertPos = pFileMenu->GetMenuItemCount() - 1;
	    CString	str1, str2;
	    int		ct = 0;
	    for( ; ; )
	    {
		    str1 = pApp->GetMRUString(ct);
		    if(str1.IsEmpty())
				break;
			if(str1.GetLength() > 30)
			{
				// If the string is too long add ... to the display string.
				int		nStrLen = str1.GetLength() + 1;
				char*	pszTmp = new char[nStrLen];
				memset(pszTmp, 0, nStrLen);
				strcpy(pszTmp, str1);
				ic_dotstr(pszTmp, 30, "b"/*DNT*/);
				str1 = pszTmp;
				delete [] pszTmp;
			}

		    str2.Format("&%d %s"/*DNT*/, ct + 1, str1);
		    pFileMenu->InsertMenu(nInsertPos, MF_BYPOSITION | MF_STRING, ICAD_MRUFILELIST_START + ct, str2);
		    ++nInsertPos;
		    ++ct;
	    }

	    if(ct)
			pFileMenu->InsertMenu(nInsertPos, MF_BYPOSITION | MF_SEPARATOR, ICAD_MRUFILELIST_START + ct);
    }

	hMenu = pMainMenu->GetSafeHmenu();
	pMainMenu->Detach();
	delete pMainMenu;
	return hMenu;
}

void
CIcadMenu::EnableAllItems
(
 BOOL	bEnable
)
{
	if(!m_pMain)
		return;

	CMenu*	pMainMenu = m_pMain->GetMenu();
	if(!pMainMenu)
		return;

	CMenu*	pMenu;
	int		nItems = pMainMenu->GetMenuItemCount();
	UINT	nEnableFlags = MF_BYPOSITION | MF_ENABLED;

	if(!bEnable)
		nEnableFlags = MF_BYPOSITION | MF_GRAYED;

	UINT	nID;
	int		nSubItems, ct, ct1;

	for(ct = 0; ct < nItems; ++ct)
	{
		if(!(pMenu = pMainMenu->GetSubMenu(ct)))
			continue;
		nSubItems = pMenu->GetMenuItemCount();
		for(ct1 = 0; ct1 < nSubItems; ++ct1)
		{
			nID = pMenu->GetMenuItemID(ct1);
			if(nID)
				pMenu->EnableMenuItem(ct1, nEnableFlags);

		}
	}
}

LPSTR
CIcadMenu::GetHelpString
(
 int	nID
)
{
	int	idxItem = nID - TB_START_ID;
	if(idxItem < 0 || idxItem > m_nMenuItems)
		return NULL;

	return (LPSTR)(LPCTSTR)m_pMenuArray[idxItem].helpString;
}

LPSTR
CIcadMenu::GetCommandString
(
 int	nID
)
{
	int	idxItem = nID - TB_START_ID;
	if(idxItem < 0 || idxItem > m_nMenuItems)
		return NULL;

	return (LPSTR)(LPCTSTR)m_pMenuArray[idxItem].command;
}

LRESULT
CMainWindow::OnSetMessageString
(
 WPARAM	wParam,
 LPARAM	lParam
)
{
	if(wParam >= TB_START_ID && wParam <= TB_END_ID)
	{
		// Menu items
		if(m_pMenuMain)
		{
			CIcadMenu*	pMenu = m_pMenuMain->GetCurrentMenu();
			if(pMenu)
				return CFrameWnd::OnSetMessageString(0, (LPARAM)pMenu->GetHelpString(wParam));
		}
	}
	else if(wParam >= ICAD_TBID_START && wParam <= ICAD_TBID_END)
	{
		// Message strings for toolbars get set in OnMouseMove because it works better there.
		return wParam;
	}
	else if(wParam >= ICAD_MRUFILELIST_START && wParam <= ICAD_MRUFILELIST_END)
	{
		// Most recent used file list menu item.
		return CFrameWnd::OnSetMessageString(0, (LPARAM)ResourceString(IDC_ICADMENU_OPENS_THIS_DOC_25, "Opens this document"));
	}

	return CFrameWnd::OnSetMessageString(wParam, lParam);
}

void
CMainWindow::IcadMenuExec
(
 UINT	nID
)
{
	if(m_bCustomize)
	{
		((CIcadCustom*)m_pIcadCustom)->m_CusToolbar.OnTbButtonPick(nID - TB_START_ID + ICAD_CUSTOOL_BTNID_START);
		return;
	}

	LPARAM	lParam = 0;
	if(m_pMenuMain)
	{
		CIcadMenu*	pMenu = m_pMenuMain->GetCurrentMenu();
		if(pMenu)
			lParam = (LPARAM)pMenu->GetCommandString(nID);
	}

	if(lParam)
		SDS_SendMessage(WM_MENUSELECT, 0, lParam);
}

void
CMainWindow::OnMRUPick
(
 UINT	nID
)
{
	CIcadApp*	pApp = (CIcadApp*)AfxGetApp();
	int			idxMRUFile = nID - ICAD_MRUFILELIST_START;
	CString		str(pApp->GetMRUString(idxMRUFile));

	if(str.IsEmpty())
		return;

	char	fname[IC_PATHSIZE + IC_FILESIZE];
	strncpy(fname, str, sizeof(fname));
	if(!access(fname, 0))
	{
		// If the file is found, open it.
		// Convert any \'s to /'s that may be in the path
		char*	cp1;
		for(cp1 = fname; *cp1; ++cp1)
		{
			if(*cp1 == '\\'/*DNT*/)
				*cp1='/'/*DNT*/;
			if(*cp1 == ' '/*DNT*/)
				*cp1 = '|'/*DNT*/;
		}
		str.Format("^C^C^C_OPEN;%s"/*DNT*/, fname);
		SDS_SendMessage(WM_MENUSELECT, 0, (LPARAM)str.GetBuffer(0));
	}
	else
	{
		// If the file is not found, delete it from the alias list.
		str.Format(ResourceString(IDC_ICADMENU____S___NCANNOT_27, "\'%s\'\nCannot open file.\nFile not found."), str);
		MessageBox(str, NULL, MB_OK | MB_ICONEXCLAMATION);
		pApp->RemoveMRUString(idxMRUFile);
		IcadWndAction(ICAD_WNDACTION_NEWMENUS);
	}
}

void
CMainWindow::ContextMenu
(
 CPoint	point
)
{
	int				ct, nStyle, nToolBarID;
	CIcadChildWnd*	pChild;

	if(pChild = (CIcadChildWnd*)MDIGetActive(NULL))
	{
		CIcadDoc*	pDoc;
		if(pDoc = (CIcadDoc*)(pChild->GetActiveDocument()))
		{
			COleClientItem*	pActiveItem = pDoc->GetInPlaceActiveItem(GetIcadView());
			if(pActiveItem)
				return;
		}
	}

	if(m_bPrintPreview)
		return;

	// Create the popup menu
	CMenu*	pMenu = new CMenu();
	pMenu->CreatePopupMenu();

	// Append the command bar
	if(m_pCmdBar && m_pCmdBar->IsWindowVisible())
		nStyle = MF_CHECKED;
	else
		nStyle = MF_UNCHECKED;

	pMenu->AppendMenu(MF_STRING | nStyle, ICAD_CONTEXTMENU_START, ResourceString(IDC_ICADMENU_COMMAND_BAR_28, "Command Bar"));

	// Append the status bar
	if(m_StatusBar.IsWindowVisible())
		nStyle = MF_CHECKED;
	else
		nStyle = MF_UNCHECKED;

	pMenu->AppendMenu(MF_STRING | nStyle, ICAD_CONTEXTMENU_START + 1, ResourceString(IDC_ICADMENU_STATUS_BAR_29, "Status Bar"));
	pMenu->AppendMenu(MF_SEPARATOR);

	// Append the toolbar names
	resbuf	rb;
	int		expmode;
    sds_getvar("EXPLEVEL"/*DNT*/, &rb);
	switch(rb.resval.rint)
	{
		case 1: expmode = 3; break;
		case 2: expmode = 7; break;
		case 3: expmode = 15;
	}

	CPtrList*		pToolBarList = ((CIcadToolBarMain*)m_pToolBarMain)->GetToolBarList();
	CIcadToolBar*	pToolBar;
	CString			str;
	POSITION		pos = pToolBarList->GetHeadPosition();

	for(ct = 0; pos; ++ct)
	{
		pToolBar = (CIcadToolBar*)pToolBarList->GetNext(pos);
		if(!pToolBar)
			continue;

		pToolBar->GetWindowText(str);
		if(pToolBar->m_bEmpty || str.IsEmpty())
			continue;

		nToolBarID = ICAD_CONTEXTMENU_START + 4 + ct;
		if(pToolBar->IsWindowVisible())
			nStyle = MF_CHECKED;
		else
			nStyle = MF_UNCHECKED;
		pMenu->AppendMenu(MF_STRING | nStyle,nToolBarID, str);
	}

	pMenu->AppendMenu(MF_SEPARATOR);
	pMenu->AppendMenu(MF_STRING, ICAD_CONTEXTMENU_START + 2, ResourceString(IDC_ICADMENU_TOOLBARS____31, "Toolbars..."));
	pMenu->AppendMenu(MF_STRING, ICAD_CONTEXTMENU_START + 3, ResourceString(IDC_ICADMENU_CUSTOMIZE____32, "Customize..."));

	// Display the menu
	pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y, this);

	// Delete the menu
	delete pMenu;
	return;
}

void
CMainWindow::OnContextMenuPick
(
 UINT	nId
)
{
	int	nIndex = nId - ICAD_CONTEXTMENU_START;
	switch(nIndex)
	{
	case 0 :
		ShowCmdBars();
		break;
	case 1 :
		ShowStatusBar();
		break;
	case 2 :
		IcadToolbars();
		break;
	case 3 :
		IcadCustomDia(1);
		break;
	default :
		if(nIndex >= 4)
		{
			int	ct = nIndex - 4;
			// Get a pointer to the toolbar.
			CPtrList*		pToolBarList = ((CIcadToolBarMain*)m_pToolBarMain)->GetToolBarList();
			CIcadToolBar*	pToolBar = (CIcadToolBar*)pToolBarList->GetAt(pToolBarList->FindIndex(ct));
			// Show or hide the toolbar.
			if(pToolBar->IsWindowVisible())
				ShowControlBar(pToolBar, FALSE, FALSE);
			else
				ShowControlBar(pToolBar, TRUE, FALSE);
		}
	}	// switch
}

void
CMainWindow::TextScreenMenu
(
 CPoint	point
)
{
	// Create the popup menu
	CMenu*	pMenu = new CMenu();
	pMenu->CreatePopupMenu();
	pMenu->AppendMenu(MF_STRING, ICAD_TEXTSCREENMENU_START, ResourceString(IDC_ICADMENU__COPY_HISTORY_33, "&Copy History"));
	pMenu->AppendMenu(MF_STRING, ICAD_TEXTSCREENMENU_START + 1, ResourceString(IDC_ICADMENU__PASTE_34, "&Paste"));
	pMenu->AppendMenu(MF_SEPARATOR);
	pMenu->AppendMenu(MF_STRING, ICAD_TEXTSCREENMENU_START + 2, ResourceString(IDC_ICADMENU__OPTIONS____35, "&Options..."));

	// Display the menu
	pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y, this);

	delete pMenu;
}

void
CMainWindow::OnTextScreenMenuPick
(
 UINT	nID
)
{
	CIcadApp*	pApp;

	switch(nID - ICAD_TEXTSCREENMENU_START)
	{
	case 0 :	// Copy History
		IcadWndAction(ICAD_WNDACTION_COPYHIST);
		break;
	case 1 :	// Paste
		// This paste only looks for CF_TEXT from the clipboard.
		pApp = (CIcadApp*)AfxGetApp();
		if(!pApp || !OpenClipboard())
			return;
		PasteClipboardText();
		CloseClipboard();
		break;
	case 2 :	// Options...
		IcadWndAction(ICAD_WNDACTION_CONFIG);
	}
}

void
CIcadMenu::DoMenuRef
(
 LPCTSTR	pszRef
)
{
	char*	pszRefNo = NULL, *pc1, *pc2;
	int		nPopRef = -1, nSubItem = -1, ct;
	CMenu*	pParentMenu;

	// Allocate a string that contains the n.n.n string.
	pc1 = (char*)pszRef;
	if(*pszRef == '$'/*DNT*/)
		++pc1;
	if(*pc1 != 'p'/*DNT*/ && *pc1 != 'P'/*DNT*/)
	{
		char	pFirstChar[2] = {*pc1, 0};
		if(!strpbrk(pFirstChar, "bBaAiImMsStT"/*DNT*/))
		{
			//CString str;
			//str.Format("Invalid Menu Reference string: \"%s\"",pszRef);
			//MessageBox(str);
		}
		return;
	}
	++pc1;

	if(!(pc2 = strchr(pszRef, '='/*DNT*/)))
	{
		//CString str;
		//str.Format("Invalid Menu Reference string: \"%s\"",pszRef);
		//MessageBox(str);
		return;
	}

	*pc2 = 0;
	pszRefNo = new char[strlen(pc1) + 1];
	strcpy(pszRefNo, pc1);
	*pc2 = '='/*DNT*/;

	// Decide which operation to perform from the character after the '='.
	CString		str, strOperation(pc2 + 1);
	strOperation.TrimRight();

	if( strOperation.IsEmpty() ||
		strOperation[0] == '~'/*DNT*/ || strOperation[0] == '!'/*DNT*/ ||
		strOperation[0] == '?'/*DNT*/ || strOperation[0] == '|'/*DNT*/ )
	{
		// Get the index of the referenced popup menu.
		if(!(pParentMenu = m_pMain->GetMenu()))
			goto out;

		for(ct = 0, pc1 = pszRefNo; pc2 = strchr(pc1, '.'/*DNT*/); ++ct)
		{
			*pc2 = 0;
			nSubItem = atoi(pc1);
			if(ct > 0)
				--nSubItem;
			else
			{
				// Check to see if the current mdi child has been maximized and placed the
				// window menu in the first position.
				MENUITEMINFO	mi;
				memset(&mi, 0, sizeof(MENUITEMINFO));
				mi.cbSize = sizeof(MENUITEMINFO);
				mi.fMask = MIIM_TYPE;
				::GetMenuItemInfo(pParentMenu->GetSafeHmenu(), 0, TRUE, &mi);
				if(!(mi.fType & MFT_BITMAP))
					--nSubItem;
			}
			*pc2 = '.';
			pc1 = pc2 + 1;
			pParentMenu = pParentMenu->GetSubMenu(nSubItem);
		}

		if(!ct)
		{
			AfxMessageBox(ResourceString(IDC_ICADMENU_CANNOT_CHANGE__36, "Cannot change the state of a menu title."));
			goto out;
		}

		nSubItem = (atoi(pc1) - 1);

		if(!pParentMenu)
		{
			AfxMessageBox(ResourceString(IDC_ICADMENU_INVALID_POPUP__37, "Invalid Popup Reference."));
			goto out;
		}

		// Now we have a pointer to the menu and an index, perform the desired operation.
		if(strOperation.IsEmpty())
		{
			pParentMenu->EnableMenuItem(nSubItem, MF_BYPOSITION | MF_ENABLED);
			pParentMenu->CheckMenuItem(nSubItem, MF_BYPOSITION | MF_UNCHECKED);
			goto out;
		}

		if(strOperation[0] == '?'/*DNT*/)
			// This is supposed to return whether or not the menu is partially loaded.
			goto out;

		if(strOperation[0] == '|'/*DNT*/)
		{
			// Execute the menu command.
			UINT	nID = pParentMenu->GetMenuItemID(nSubItem);
			LPARAM	lParam = (LPARAM)m_pMenuArray[nID - TB_START_ID].command.GetBuffer(0);
			if(nID > 0 && lParam)
				SDS_SendMessage(WM_MENUSELECT, 0, lParam);
			goto out;
		}

		// These two options can be combined in the string, so check for both.
		if(strOperation.Find('~'/*DNT*/) != -1)
			pParentMenu->EnableMenuItem(nSubItem, MF_BYPOSITION | MF_GRAYED);

		if(strOperation.Find('!'/*DNT*/) != -1)
			pParentMenu->CheckMenuItem(nSubItem, MF_BYPOSITION | MF_CHECKED);
	}
	else
	{
		// Get the index of the referenced popup menu.
		pc1 = pszRefNo;
		if(pc2 = strchr(pszRefNo, '.'/*DNT*/))
		{
			*pc2 = 0;
			nPopRef = atoi(pc1);
			*pc2 = '.'/*DNT*/;
			pc1 = pc2 + 1;
		}

		if(nPopRef == -1)
			nPopRef = atoi(pc1);

		if(strOperation[0] == '*'/*DNT*/)
		{
			if(!nPopRef)
			{
				// Display the designated "shift right-click" menu (pop0 from .mnu file).
				int	idxEsnapMenu = m_idxEsnapMenu;

				if(idxEsnapMenu == -1)
					goto out;
				CMenu*	pPopup = new CMenu();
				HMENU	hSubMenu = SubMenuCreator(&idxEsnapMenu, ICAD_MN_ALL, 0);
				if(hSubMenu)
				{
					pPopup->Attach(hSubMenu);
					POINT	pt;
					::GetCursorPos(&pt);
					pPopup->TrackPopupMenu(TPM_RIGHTBUTTON, pt.x, pt.y, m_pMain);
				}
				delete pPopup;
				goto out;
			}

			CMenu*	pMainMenu = m_pMain->GetMenu();
			if(!pMainMenu)
				goto out;

			// Check to see if the current mdi child has been maximized and placed the
			// window menu in the first position.
			MENUITEMINFO	mi;

			memset(&mi, 0, sizeof(MENUITEMINFO));
			mi.cbSize = sizeof(MENUITEMINFO);
			mi.fMask = MIIM_TYPE;
			::GetMenuItemInfo(pMainMenu->GetSafeHmenu(), 0, TRUE, &mi);

			if(!(mi.fType & MFT_BITMAP))
				--nPopRef;
			// Display the popup menu.
			CMenu*	pPopup = pMainMenu->GetSubMenu(nPopRef);
			if(!pPopup)
			{
				AfxMessageBox(ResourceString(IDC_ICADMENU_INVALID_POPUP__37, "Invalid Popup Reference."));
				goto out;
			}

			CRect	rectMenu;
			::GetMenuItemRect(m_pMain->GetSafeHwnd(), pMainMenu->GetSafeHmenu(), nPopRef, rectMenu);
			::HiliteMenuItem(m_pMain->GetSafeHwnd(), pMainMenu->GetSafeHmenu(), nPopRef, MF_BYPOSITION | MF_HILITE);

			pPopup->TrackPopupMenu(TPM_LEFTALIGN, rectMenu.left, rectMenu.bottom, m_pMain);

			::HiliteMenuItem(m_pMain->GetSafeHwnd(), pMainMenu->GetSafeHmenu(), nPopRef, MF_BYPOSITION | MF_UNHILITE);
		}
		else if(strOperation[0] == '+'/*DNT*/ || strOperation[0] == '-'/*DNT*/)
		{
			// ~| TODO - + loads a menu and - unloads a menu.
		}
		else
		{
			// If we make it here assume a request for a menu swap.
			int	idxRefItem, idxNewItem, nCurPopItem = 0;
			
			// Get the index of the requested new pop menu.
			for(idxNewItem = 0; idxNewItem < m_nMenuItems; ++idxNewItem)
			{
				if(!m_pMenuArray[idxNewItem].nSubLevel && !m_pMenuArray[idxNewItem].tearOffName.IsEmpty())
				{
					str.Format("%s.%s"/*DNT*/, m_pMenuArray[idxNewItem].fileName, m_pMenuArray[idxNewItem].itemName);
					if(!strOperation.CompareNoCase(str))
						break;
				}
			}

			// If the requested menu wasn't found display an error and return.
			if(idxNewItem >= m_nMenuItems)
			{
				str.Format(ResourceString(IDC_ICADMENU__IS_NOT_AN_AVA_40, "\"%s\" is not an available tear-off menu name."), strOperation);
				AfxMessageBox(str);
				goto out;
			}

			// Get the index of the referenced pop item.
			for(idxRefItem = 0; idxRefItem < m_nMenuItems; ++idxRefItem)
			{
				str.Format("POP%d"/*DNT*/, nPopRef);

				if(!m_pMenuArray[idxRefItem].nSubLevel && !m_pMenuArray[idxNewItem].tearOffName.IsEmpty())
				{
					if(!str.CompareNoCase(m_pMenuArray[idxNewItem].tearOffName))
						break;
				}
			}

			// If the referenced menu position wasn't found display an error and return.
			if(idxRefItem >= m_nMenuItems)
			{
				AfxMessageBox(ResourceString(IDC_ICADMENU_INVALID_POPUP__37, "Invalid Popup Reference."));
				goto out;
			}

			str.Format ("POP%d"/*DNT*/, nPopRef);

			//m_idxEsnapMenu = idxNewItem;
			//reset the tearoff name for the old one
			m_pMenuArray[idxRefItem].tearOffName.Empty();

			//set the tearoff name for the new menu - allocate if needed.
			m_pMenuArray[idxNewItem].tearOffName = str;

			// Set the hidden flag for the referenced pop item's items.
			for(ct = idxRefItem + 1; idxRefItem < m_nMenuItems && m_pMenuArray[ct].nSubLevel; ++ct)
				m_pMenuArray[ct].lflgVisibility |= ICAD_MN_HIDE;

			// Clear the hidden flag for the new submenu item's items.
			for(ct = idxNewItem + 1; idxNewItem < m_nMenuItems && m_pMenuArray[ct].nSubLevel; ++ct)
				m_pMenuArray[ct].lflgVisibility &= ~ICAD_MN_HIDE;

			// Create the new menu.
			m_pMain->IcadWndAction(ICAD_WNDACTION_NEWMENUS);
		}
	}

out:
	delete [] pszRefNo;
	return;
}

BOOL
CIcadMenu::LoadFromICM
(
 LPCTSTR	pszFileName
)
{
	if(!pszFileName || !strstr(pszFileName, ".icm"/*DNT*/))
		return FALSE;

	CStdioFile		fileRead;
	CFileException	fileException;
	if(!fileRead.Open(pszFileName, CFile::modeRead, &fileException))
		return FALSE;

	BOOL			bRet = TRUE;
	int				ct, nMenuItems;
	CString			str;
	int				charIdx;

	fileRead.ReadString(str);
	if(str.Compare("[IntelliCAD Custom Menu File]"/*DNT*/))
	{
		bRet = FALSE;
		goto out;
	}

	// Get the number of menu items
	fileRead.ReadString(str);
	str.TrimLeft();
	if(str.Find("nMenuItems="/*DNT*/))
	{
		bRet = FALSE;
		goto out;
	}
	if((charIdx = str.Find('='/*DNT*/) + 1) == str.GetLength() || (nMenuItems = atoi(str.Right(str.GetLength() - charIdx))) <= 0)
	{
		bRet = FALSE;
		goto out;
	}

	// Read through the file and set the item information in the menu array.
	DeleteMenuArray();
	m_nMenuItems = nMenuItems;
	m_pMenuArray = new ICMENUITEM[nMenuItems];
	ct = -1;
	while(fileRead.ReadString(str) && ct < nMenuItems)
	{
		str.TrimLeft();

		if(!str.Find("[MnuItem-"/*DNT*/))
		{	// New menu item.
			++ct;
			continue;
		}
		else if(!str.Find("Name="/*DNT*/))
		{
			if((charIdx = str.Find('='/*DNT*/) + 1) == str.GetLength())
				continue;
			m_pMenuArray[ct].itemName = str.Right(str.GetLength() - charIdx);
		}
		else if(!str.Find("TearOffName="/*DNT*/))
		{
			if((charIdx = str.Find('='/*DNT*/) + 1) == str.GetLength())
				continue;
			m_pMenuArray[ct].tearOffName = str.Right(str.GetLength() - charIdx);
		}
		else if(!str.Find("Command="/*DNT*/))
		{
			if((charIdx = str.Find('='/*DNT*/) + 1) == str.GetLength())
				continue;
			m_pMenuArray[ct].command = str.Right(str.GetLength() - charIdx);
		}
		else if(!str.Find("HelpString="/*DNT*/))
		{
			if((charIdx = str.Find('='/*DNT*/) + 1) == str.GetLength())
				continue;
			m_pMenuArray[ct].helpString = str.Right(str.GetLength() - charIdx);
		}
		else if(!str.Find("ChekVar="/*DNT*/))
		{
			if((charIdx = str.Find('='/*DNT*/) + 1) == str.GetLength())
				continue;
			m_pMenuArray[ct].chekVar = str.Right(str.GetLength() - charIdx);
		}
		else if(!str.Find("GrayVar="/*DNT*/))
		{
			if((charIdx = str.Find('='/*DNT*/) + 1) == str.GetLength())
				continue;
			m_pMenuArray[ct].grayVar = str.Right(str.GetLength() - charIdx);
		}
		else if(!str.Find("FileName="/*DNT*/))
		{
			if((charIdx = str.Find('='/*DNT*/) + 1) == str.GetLength())
				continue;
			m_pMenuArray[ct].fileName = str.Right(str.GetLength() - charIdx);
		}
		else if(!str.Find("Visibility="/*DNT*/))
		{
			if((charIdx = str.Find('='/*DNT*/) + 1) == str.GetLength())
				continue;
			m_pMenuArray[ct].lflgVisibility = atoi(str.Right(str.GetLength() - charIdx));
		}
		else if(!str.Find("EntityVisibility="/*DNT*/))
		{
			if((charIdx = str.Find('='/*DNT*/) + 1) == str.GetLength())
				continue;
			m_pMenuArray[ct].lflgEntVis = atoi(str.Right(str.GetLength() - charIdx));
		}
		else if(!str.Find("SubLevel="/*DNT*/))
		{
			if((charIdx = str.Find('='/*DNT*/) + 1) == str.GetLength())
				continue;
			m_pMenuArray[ct].nSubLevel = (short)atoi(str.Right(str.GetLength() - charIdx));
		}
		else if(!str.Find("AddSpacerBefore="/*DNT*/))
		{
			if((charIdx = str.Find('='/*DNT*/) + 1) == str.GetLength())
				continue;
			m_pMenuArray[ct].bAddSpacerBefore = (BOOL)atoi(str.Right(str.GetLength() - charIdx));
		}
	}	// while(

out:
	fileRead.Close();
	return bRet;
}

BOOL
CIcadMenu::SaveToICM
(
 LPCTSTR	pszFileName
)
{
	if(!pszFileName)
		return FALSE;

	// Make a local copy of the file name.
	char* pszFile = new char[lstrlen(pszFileName) + 5]; // +5 in case it doesn't include an extension.
	lstrcpy(pszFile, pszFileName);
	// Make sure the file name has the correct extension.
	if(!strstr(pszFile, ".icm"/*DNT*/))
	{
		char* pc1;
		if(pc1 = strrchr(pszFile, '.'))
			*pc1 = 0;
		ic_setext(pszFile, ".icm"/*DNT*/);
	}

	CStdioFile		fileSave;
	CFileException	fileException;
	CString			str;
	if(!fileSave.Open(pszFile, CFile::modeCreate | CFile::modeWrite, &fileException))
	{
		delete [] pszFile;
		return FALSE;
	}

	fileSave.WriteString(ResourceString(IDC_ICADCUSMENU__INTELLICAD_41, "[IntelliCAD Custom Menu File]\n" ));
	// Write the number of aliases
	str.Format(ResourceString(DNT_ICADCUSMENU__TNMENUITEM_42, "\tnMenuItems=%d\n" ), m_nMenuItems);
	fileSave.WriteString(str);

	// Loop through the menu array and write the item information to the file.
	int		ct;
	for(ct = 0; ct < m_nMenuItems; ++ct)
	{
		str.Format(ResourceString(DNT_ICADCUSMENU__MNUITEM__D_43, "[MnuItem-%d]\n" ), ct);
		fileSave.WriteString(str);

		// Write the ICMENUITEM structure info.
		if(!m_pMenuArray[ct].itemName.IsEmpty())
		{
			str.Format(ResourceString(DNT_ICADCUSMENU__TNAME__S_N_44, "\tName=%s\n"), m_pMenuArray[ct].itemName);
			fileSave.WriteString(str);
		}
		if(!m_pMenuArray[ct].tearOffName.IsEmpty())
		{
			str.Format(ResourceString(DNT_ICADCUSMENU__TTEAROFFNA_45, "\tTearOffName=%s\n"), m_pMenuArray[ct].tearOffName);
			fileSave.WriteString(str);
		}
		if(!m_pMenuArray[ct].command.IsEmpty())
		{
			str.Format(ResourceString(DNT_ICADCUSMENU__TCOMMAND___46, "\tCommand=%s\n"), m_pMenuArray[ct].command);
			fileSave.WriteString(str);
		}
		if(!m_pMenuArray[ct].helpString.IsEmpty())
		{
			str.Format(ResourceString(DNT_ICADCUSMENU__THELPSTRIN_47, "\tHelpString=%s\n"), m_pMenuArray[ct].helpString);
			fileSave.WriteString(str);
		}
		if(!m_pMenuArray[ct].chekVar.IsEmpty())
		{
			str.Format(ResourceString(DNT_ICADCUSMENU__TCHEKVAR___48, "\tChekVar=%s\n"), m_pMenuArray[ct].chekVar);
			fileSave.WriteString(str);
		}
		if(!m_pMenuArray[ct].grayVar.IsEmpty())
		{
			str.Format(ResourceString(DNT_ICADCUSMENU__TGRAYVAR___49, "\tGrayVar=%s\n"), m_pMenuArray[ct].grayVar);
			fileSave.WriteString(str);
		}
		if(!m_pMenuArray[ct].fileName.IsEmpty())
		{
			str.Format(ResourceString(DNT_ICADCUSMENU__TFILENAME__50, "\tFileName=%s\n"), m_pMenuArray[ct].fileName);
			fileSave.WriteString(str);
		}
		if(m_pMenuArray[ct].lflgVisibility)
		{
			str.Format(ResourceString(DNT_ICADCUSMENU__TVISIBILIT_51, "\tVisibility=%d\n"), m_pMenuArray[ct].lflgVisibility);
			fileSave.WriteString(str);
		}
		if(m_pMenuArray[ct].lflgEntVis)
		{
			str.Format(ResourceString(DNT_ICADCUSMENU__TENTITYVIS_52, "\tEntityVisibility=%d\n"), m_pMenuArray[ct].lflgEntVis);
			fileSave.WriteString(str);
		}
		if(m_pMenuArray[ct].nSubLevel)
		{
			str.Format(ResourceString(DNT_ICADCUSMENU__TSUBLEVEL__53, "\tSubLevel=%d\n"), m_pMenuArray[ct].nSubLevel);
			fileSave.WriteString(str);
		}
		if(m_pMenuArray[ct].bAddSpacerBefore)
		{
			str.Format(ResourceString(DNT_ICADCUSMENU__TADDSPACER_54, "\tAddSpacerBefore=%d\n"), m_pMenuArray[ct].bAddSpacerBefore);
			fileSave.WriteString(str);
		}
	}

	// Close the file.
	fileSave.Close();
	delete [] pszFile;
	return TRUE;
}


/////////////////////////////////////////////////////////////////////
//	Start of Menu Parsing and Dynamic Loading of .MNU file
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
//	MnuToMnuInfo takes a pointer to the file name of the AutoCAD mnu
//	file to open and returns a pointer to an ICMNUINFO structure or
//	NULL if unsuccessful.
/////////////////////////////////////////////////////////////////////

int
Icad_Menucompare
(
 const void*	arg1,
 const void*	arg2
)
{
	return _stricmp(*(char**)arg1, *(char**)arg2);
}

int
Icad_Findcompare
(
 char**	arg1,
 char**	arg2
)
{
	int	ret = _strnicmp(*arg1, *arg2, strlen(*arg1)),
		len = 0;
	if(!ret)
	{
		len = strlen(*arg1);
		if((*arg2)[len] == ' '/*DNT*/)
			return ret;
		else
			return -1;
	}
	return ret;
}


LPICMNUINFO
CIcadMenu::MnuToMnuInfo
(
 LPCTSTR	pszFileName,
 BOOL		bAppend
)
{
	CFile			fileMenu;
	CFileException	fileException;
	char			*pBuffer, *fcp1, *helpstrs, helps[101], **sortlist = NULL;
	LPCTSTR			fcp2;
	UINT			nRet = 0;
	DWORD			dwFileLen = 0;
	ICMNUINFO		*pMnuInfo, *pmiCur, *pmiBeg = NULL;
	BOOL			bRet = TRUE, bIsContextMenu = FALSE;
	int				sortcount = 0;

	if(!fileMenu.Open(pszFileName, CFile::modeRead, &fileException))
	{
		TRACE(ResourceString(IDC_ICADMENU_CAN_T_OPEN_FIL_56, "Can't open file %s, error = %u\n"), pszFileName, fileException.m_cause);
		return FALSE;
	}

	fileMenu.SeekToBegin();
	dwFileLen = fileMenu.GetLength();
	pBuffer = new char[dwFileLen + 1];
	if(!pBuffer)
	{
		TRACE(ResourceString(IDC_ICADMENU_OUT_OF_MEMORY__57, "Out of memory error.  Can't open file %s\n"), pszFileName);
		return FALSE;
	}

	nRet = fileMenu.Read(pBuffer, dwFileLen);
	if(nRet < dwFileLen)
		pBuffer[nRet] = 0;
	else
		pBuffer[dwFileLen] = 0;
	fileMenu.Close();

	// Set the value of the MenuGroup
	CString	strMenuGroup(pszFileName);
	char*	pszMenuGroup = strstr(pBuffer, "***MENUGROUP="/*DNT*/);

	if(pszMenuGroup)
	{
		while(*pszMenuGroup != '='/*DNT*/)
			++pszMenuGroup;
		++pszMenuGroup;
		char*	fcp1 = pszMenuGroup;
		while(*fcp1 != '\r'/*DNT*/ && *fcp1 != '\n'/*DNT*/ && *fcp1)
			++fcp1;
		char	cTmp = *fcp1;
		*fcp1 = 0;
		strMenuGroup = pszMenuGroup;
		*fcp1 = cTmp;
	}

	if(bAppend)
	{
		if(IsMenuGroupLoaded(strMenuGroup))
		{
			bRet = FALSE;
			goto out;
		}
	}

	// Set up the help string area to search.
	helpstrs = (char*)stristr(pBuffer, "***HELPSTRINGS"/*DNT*/);
	if(helpstrs)
	{
		char	*cp1,
				*fcp2 = (char*)stristr(helpstrs + 10, "***"/*DNT*/);
		if(fcp2)
			*fcp2 = 0;

		for(cp1 = helpstrs; *cp1; ++cp1)
		{
			if(*cp1 == '\n'/*DNT*/)
				++sortcount;
		}

		int	ct2;
		sortlist = new char* [sortcount];
		memset(sortlist,0,sizeof(char*)*sortcount);

		for(ct2 = 0, cp1 = helpstrs; *cp1; ++cp1)
		{
			if(*cp1 == '\n'/*DNT*/)
			{
				*cp1 = 0;
				sortlist[ct2] = cp1 + 1;
				++ct2;
			}
		}
		qsort((void*)sortlist, sortcount, sizeof(char*), Icad_Menucompare);
	}

	// VT 28-6-2002. Ignorecase. Reason: Fix for bug 78223 from BugZilla
	fcp1 = stristr(pBuffer, "***POP"/*DNT*/);

	while(fcp1 && *fcp1)
	{
		bIsContextMenu = FALSE;
		// Allocate a new MnuInfo structure.
		pMnuInfo = new ICMNUINFO;
		if(!pmiBeg)
			pmiCur = pmiBeg = pMnuInfo;
		else
			pmiCur = pmiCur->next = pMnuInfo;
		pmiCur->next = NULL;

		if(!pMnuInfo)
		{
			bRet = FALSE;
			goto out;
		}

		// Get the tear-off name, or POPn name.
		if(*fcp1 == '*'/*DNT*/)
			while(*fcp1 == '*'/*DNT*/)
				++fcp1;

		fcp2 = fcp1;
		while(*fcp1 != '\n'/*DNT*/)
			++fcp1;

		*fcp1 = 0;
		// Set the title.
		CString	str(fcp2);
		str.TrimRight();
		if(!str.IsEmpty())
		{
			pMnuInfo->tearOffName = str;
			if(!str.CompareNoCase("POP0"/*DNT*/) || !str.CompareNoCase("POP17"/*DNT*/))
				bIsContextMenu = TRUE;
		}

		*fcp1 = '\n'/*DNT*/;

		// Look up the help string.
		if(helpstrs)
		{
			char*	fcp3;
			int		idxEnd = 0;

			for(fcp3 = fcp1 + 1; *fcp3 && *fcp3 != '['/*DNT*/; ++fcp3, ++idxEnd)
				;
			if(*fcp3 == '['/*DNT*/ && idxEnd > 0)
			{
				char*	pHlplu = new char[idxEnd];
				strncpy(pHlplu, fcp1 + 1, idxEnd - 1);
				ic_trim(pHlplu, "be"/*DNT*/);

				char**	bsret;
				fcp3 = pHlplu;
				if( *pHlplu &&
					!!(bsret = (char**)bsearch(&fcp3, sortlist, sortcount, sizeof(char*),(int(*)(const void*, const void*))Icad_Findcompare))
					&& !!(fcp3 = *bsret) && !!(fcp3 = strchr(fcp3, '['/*DNT*/)) )
				{
					strncpy(helps, fcp3 + 1, sizeof(helps) - 1);
					if(fcp3 = strchr(helps, ']'/*DNT*/))
						*fcp3 = 0;
					pMnuInfo->helpString = helps;
				}
				delete [] pHlplu;
			}
		}

		// Get the menu title.
		while(*fcp1 && *fcp1 != '['/*DNT*/)
			++fcp1;
		if(!*fcp1)
		{
			pMnuInfo->tearOffName.Empty();
			delete pMnuInfo;
			pMnuInfo = NULL;
			if(pmiBeg && pmiCur == pmiBeg)
				pmiBeg = NULL;
			if(pmiBeg && pmiCur == pmiBeg->next)
				pmiBeg->next = NULL;
			if(pmiCur)
				pmiCur->next = NULL;
			goto out;
		}

		fcp2 = ++fcp1;
		while(*fcp1 != ']'/*DNT*/)
			++fcp1;
		*fcp1 = 0;

		// Step over unwanted characters.
		if(*fcp2 == '-'/*DNT*/ && *(fcp2 + 1) == '>'/*DNT*/)
			fcp2 += 2;

		// Set the title.
		int	nStrLen = strlen(fcp2);
		if(nStrLen > 0)
		{
			char*	pTitle = new char[nStrLen + 1];
			if(fcp2[0] == '/'/*DNT*/ && nStrLen > 1)
			{
				int		didnumic = 0;
				char	numic = fcp2[1];
				for(int fi1 = 0, fi2 = 2; nStrLen > fi2; ++fi1, ++fi2)
				{
					if(!didnumic && numic == fcp2[fi2])
					{
						pTitle[fi1] = '&'/*DNT*/;
						++fi1;
						didnumic = 1;
					}
					pTitle[fi1] = fcp2[fi2];
				}

				pTitle[fi1] = 0;
			}
			else
				strcpy(pTitle, fcp2);

			pMnuInfo->title = pTitle;
			delete [] pTitle;
		}

		if(!strMenuGroup.IsEmpty())
			pMnuInfo->fileName = strMenuGroup;

		pMnuInfo->bIsContextMenu = bIsContextMenu;

		*fcp1 = ']'/*DNT*/;
		++fcp1;
		int	ctRecur = 0;
		fcp1 = CreateMnuSubItems(strMenuGroup, fcp1, pmiCur, &ctRecur, helpstrs, sortlist, sortcount);
	}

out:
	if(bRet)
	{
		if(!bAppend)
			FreeLoadedMenuList();
		AddMenuToLoadedMenuList(strMenuGroup, pszFileName);
	}

	delete sortlist;
	delete [] pBuffer;

	if(!bRet && pmiBeg)
	{
		FreeMnuInfo(pmiBeg);
		pmiBeg = NULL;
	}

	return pmiBeg;
}

char*
CIcadMenu::CreateMnuSubItems
(
 LPCTSTR		pszFileName,
 LPCTSTR		pBuffer,
 LPICMNUINFO	pmiSubBeg,
 int*			ctRecur,
 LPCTSTR		helpstrs,
 char**			sortlist,
 int			sortcount
)
{
// The ctRecur variable is to pass a value back to this function telling it how many
// times to return before it is caught up with the correct recursion level.  This number
// will be greater than zero when an item contains a title similar to: "[<-<-...title]".

	if(!pBuffer || !*pBuffer)
		return NULL;

	BOOL	bDoSubMenu = FALSE, bEndOfSubMenu = FALSE;
	char	helps[101];

	ICMNUINFO	*pMnuInfo = NULL, *pmiCur;
	char		*fcp1, *fcp2, *fcp3 = NULL, cTmp, *fcpNextMenu;

	fcp1 = (char*)pBuffer;
	if(!pmiSubBeg)
		return fcp1;

	if(fcp1[0] == '<' && fcp1[1] == '-')
		return fcp1 + 2;

	// Setup the pointer to the next menu.
	if( !(!!(fcpNextMenu = strstr(fcp1, "**"/*DNT*/)) && fcpNextMenu[2] != '*'/*DNT*/) &&
		!(fcpNextMenu = stristr(fcp1, "***POP"/*DNT*/)) &&
		!!(fcpNextMenu = strstr(fcp1, "***"/*DNT*/)) )
		*fcpNextMenu = 0;

	while(fcp1 && *fcp1)
	{
		while(*fcp1 != '\n'/*DNT*/ && *fcp1)
			++fcp1;
		++fcp1;

		if(!*fcp1)
			break;

		if(fcpNextMenu && fcpNextMenu <= fcp1)
			return fcpNextMenu;

		// Create a buffer containing the entire title and command.
		int		idx;
		CString	strCur;
		strCur.Empty();

		for( ; ; )
		{
			fcp2 = fcp1;
			while(*fcp1 != '\n'/*DNT*/ && *fcp1)
				++fcp1;

			cTmp = *fcp1;
			*fcp1 = 0;
			strCur += fcp2;
			*fcp1 = cTmp;
			strCur.TrimRight();
			idx = strCur.GetLength();

			if(idx > 1 && strCur[idx - 1] == '+'/*DNT*/)
			{
				// If the last character on the line is a "+" continue the string
				// onto the next line.
				strCur = strCur.Left(idx - 1);
				if(!*fcp1)
					break;
				++fcp1;
			}
			else
				break;
		}

		strCur.TrimRight();
		if(strCur.IsEmpty())
			continue;

		// Look up the help string.
		*helps = 0;
		if(helpstrs)
		{
			int	idxEnd = strCur.Find('['/*DNT*/);
			if(idxEnd > 0)
			{
				char*	pHlplu = new char[idxEnd];
				strncpy(pHlplu, strCur, idxEnd - 1);
				ic_trim(pHlplu, "be"/*DNT*/);

				char	*fcp3 = pHlplu, **bsret;

				if( *pHlplu &&
					!!(bsret = (char**)bsearch(&fcp3, sortlist, sortcount, sizeof(char*), (int (*)(const void*, const void*))Icad_Findcompare))
					&& !!(fcp3 = *bsret) && !!(fcp3 = strchr(fcp3, '['/*DNT*/)) )
				{
					strncpy(helps, fcp3 + 1, sizeof(helps) - 1);
					if(fcp3 = strchr(helps, ']'/*DNT*/))
						*fcp3 = 0;
				}
				delete [] pHlplu;
			}
		}

		// Setup the pointers to parse the title string.
		int	idxEnd = strCur.GetLength();
		if((idx = strCur.Find('['/*DNT*/)) == -1)
		{
			if(!strnsame("//"/*DNT*/, strCur, 2))
			{
				// Allocate a new MnuInfo structure.
				pMnuInfo = new ICMNUINFO;
				if(!pmiSubBeg->pmiSub)
					pmiSubBeg->pmiSub = pmiCur = pMnuInfo;
				else
					pmiCur = pmiCur->next = pMnuInfo;

				pMnuInfo->bIsContextMenu = pmiSubBeg->bIsContextMenu;

				// Set the title
				pMnuInfo->title = strCur;
				// Set the command
				pMnuInfo->command = strCur;
				// Set the Help String
				if(*helps)
					pMnuInfo->helpString = helps;
			}
			continue;
		}

		int		idxEndTitle = strCur.Find(']'/*DNT*/);
		CString	strTitle, strCommand;

		strTitle = strCur.Mid(idx + 1, idxEndTitle - idx - 1);
		strCommand = strCur.Right(idxEnd - idxEndTitle - 1);

		// Allocate a new MnuInfo structure.
		pMnuInfo = new ICMNUINFO;
		if(!pmiSubBeg->pmiSub)
			pmiSubBeg->pmiSub = pmiCur = pMnuInfo;
		else
			pmiCur = pmiCur->next = pMnuInfo;

		pMnuInfo->bIsContextMenu = pmiSubBeg->bIsContextMenu;

		// Set the title.
		if(!strTitle.IsEmpty() && strTitle[0] == '-'/*DNT*/ && strTitle[1] == '>'/*DNT*/)
		{
			// The title has a submenu indicator.
			strTitle = strTitle.Right(strTitle.GetLength() - 2);
			bDoSubMenu = TRUE;
			//This parent item needs to inherit the tear-off name of it's parent.
			if(!pmiSubBeg->tearOffName.IsEmpty())
				pMnuInfo->tearOffName = pmiSubBeg->tearOffName;
		}
		else if(!strTitle.IsEmpty() && strTitle[0] == '<'/*DNT*/ && strTitle[1] == '-'/*DNT*/)
		{
			// The title has an end-of-submenu indicator.
			bEndOfSubMenu = TRUE;
			while(!strTitle.IsEmpty() && strTitle[0] == '<'/*DNT*/ && strTitle[1] == '-'/*DNT*/)
			{
				strTitle = strTitle.Right(strTitle.GetLength() - 2);
				++(*ctRecur);
			}
		}

		if(!strTitle.IsEmpty() && strTitle[0] == '$'/*DNT*/ && strTitle[1] == '('/*DNT*/)
		{
			// The title contains a DIESEL expression.
			int	nParen;
			idxEndTitle = strTitle.GetLength();

			for(nParen = 1, idx = 2; nParen > 0 && idx < idxEndTitle; ++idx)
			{
				if(strTitle[idx] == ')'/*DNT*/)
					--nParen;
				else
					if(strTitle[idx] == '('/*DNT*/)
						++nParen;
			}

			CString	strExpression;
			strExpression = strTitle.Left(idx);

			if(!strExpression.IsEmpty())
				pMnuInfo->expression = strExpression;

			strTitle = strTitle.Right(idxEndTitle - idx);
		}

		if(!strTitle.IsEmpty())
		{
			char*	pTitle = new char[strTitle.GetLength() + 1];

			if(strTitle[0] == '/' && strTitle.GetLength() > 1)
			{
				int		didnumic = 0;
				char	numic = strTitle[1];

				for(int fi1 = 0, fi2 = 2; strTitle.GetLength() > fi2; ++fi1, ++fi2)
				{
					if(!didnumic && numic == strTitle[fi2])
					{
						pTitle[fi1] = '&'/*DNT*/;
						++fi1;
						didnumic = 1;
					}
					pTitle[fi1] = strTitle[fi2];
				}
				pTitle[fi1] = 0;
			}
			else
				strcpy(pTitle, strTitle);

			pMnuInfo->title = pTitle;
			delete [] pTitle;
		}

		if(pszFileName)
			pMnuInfo->fileName = pszFileName;

		if(bDoSubMenu)
		{
			fcp1 = CreateMnuSubItems(pszFileName, fcp1, pMnuInfo, ctRecur, helpstrs, sortlist, sortcount);
			--(*ctRecur);
			if(*ctRecur > 0)
				return fcp1;

			bDoSubMenu = FALSE;
			continue;
		}

		// Set the command.
		if(!strCommand.IsEmpty())
			pMnuInfo->command = strCommand;

		// Set the Help String
		if(*helps)
			pMnuInfo->helpString = helps;

		if(bEndOfSubMenu)
			return fcp1;
	}

	return NULL;
}

void
CIcadMenu::FreeMnuInfo
(
 LPICMNUINFO	pmiBeg
)
{
	ICMNUINFO	*pMnuInfo, *pmiTmp = NULL;

	for(pMnuInfo = pmiBeg; pMnuInfo; pMnuInfo = pmiTmp)
	{
		FreeMnuInfo(pMnuInfo->pmiSub);
		pmiTmp = pMnuInfo->next;
		delete pMnuInfo;
	}
}

int
CIcadMenu::MnuInfoCount
(
 LPICMNUINFO	pmiBeg
)
{
	ICMNUINFO*	pMnuInfo;
	int			nItems = 0;

	for(pMnuInfo = pmiBeg; pMnuInfo; pMnuInfo = pMnuInfo->next)
	{
		if(pMnuInfo->pmiSub)
			nItems += MnuInfoCount(pMnuInfo->pmiSub);

		// Don't count spacers, unless it is an "Ending Submenu Spacer".
		if( pMnuInfo->title.IsEmpty() ||
			!pMnuInfo->title.Find("~--"/*DNT*/) || !pMnuInfo->title.Find("--"/*DNT*/) )
		{
			if(!pMnuInfo->next)
				++nItems;
		}
		else
			++nItems;
	}

	return nItems;
}

BOOL
CIcadMenu::UnloadMnuFromMenuArray
(
 LPCTSTR	pszFileName
)
{
	if(!pszFileName || !*pszFileName)
		return FALSE;

	LPICMENUITEM	lpNewArray;
	int				nMenuArrayItems = m_nMenuItems, nNewArrayItems = 0, idxCurItem = 0, ct;

	if(!IsMenuGroupLoaded(pszFileName))
		return FALSE;

	// Get a count of the number of menu items EXCLUDING the items belonging to the
	// menu specified in pszFileName.
	while(idxCurItem < nMenuArrayItems)
	{
		ct = UnloadSubItems(pszFileName, &idxCurItem, NULL, NULL);
		if(ct > 0)
			nNewArrayItems += ct + 1;
	}

	if(nNewArrayItems <= 0)
	{
		// Create the default menu and return if this unload removes all menus.
		RemoveMenuFromLoadedMenuList(pszFileName);
		DeleteMenuArray();
		CreateDefaultMenuArray();
		m_bMenuModified = TRUE;
		return TRUE;
	}

	// Allocate the new array.
	lpNewArray = new ICMENUITEM[nNewArrayItems];

	// Fill the elements of the array with all the menu data except the items
	// belonging to the menu specified in pszFileName.
	int	idxParentItem, idxNewItem = 0;
	idxParentItem = idxCurItem = 0;

	while(idxCurItem < nMenuArrayItems)
	{
		idxParentItem = idxCurItem;
		ct = UnloadSubItems(pszFileName, &idxCurItem, NULL, NULL);
		if(ct > 0)
		{
			idxCurItem = idxParentItem;
			CopyItem(&lpNewArray[idxNewItem], &m_pMenuArray[idxCurItem]);
			++idxNewItem;
			UnloadSubItems(pszFileName, &idxCurItem, lpNewArray, &idxNewItem);
		}
	}

	// Remove the group name from the list
	RemoveMenuFromLoadedMenuList(pszFileName);

	// Clean up.
	m_bMenuModified = TRUE;
	DeleteMenuArray();
	m_pMenuArray = lpNewArray;
	m_nMenuItems = nNewArrayItems;
	return TRUE;
}

int
CIcadMenu::UnloadSubItems
(
 LPCTSTR		pszFileName,
 int*			idxCurItem,
 LPICMENUITEM	lpNewArray,
 int*			idxNewItem
)
{
// When this function called with lpNewArray set to NULL, the number of sub items
// is returned and nothing is copied.  Therefore this function is used for both
// purposes (copying and counting).

	if(!pszFileName || !*pszFileName)
		return 0;

	CString			strFileName(pszFileName);
	int				nMenuArrayItems = m_nMenuItems, nArrayItems = 0, ct;

	// Get a count of the number of sub menu items for this menu EXCLUDING the items
	// belonging to the menu specified in pszFileName.
	++(*idxCurItem);

	short	nCurSubLevel = m_pMenuArray[*idxCurItem].nSubLevel;

	while(*idxCurItem < nMenuArrayItems && m_pMenuArray[*idxCurItem].nSubLevel >= nCurSubLevel)
	{
		if((*idxCurItem + 1 < nMenuArrayItems) && m_pMenuArray[*idxCurItem + 1].nSubLevel > nCurSubLevel)
		{
			int	idxParentItem = *idxCurItem;
			ct = UnloadSubItems(pszFileName, idxCurItem, NULL, NULL);
			if(ct > 0)
			{
				if(lpNewArray)
				{
					*idxCurItem = idxParentItem;
					CopyItem(&lpNewArray[*idxNewItem], &m_pMenuArray[*idxCurItem]);
					++(*idxNewItem);
					UnloadSubItems(pszFileName, idxCurItem, lpNewArray, idxNewItem);
				}
				nArrayItems += ct + 1;
			}
			continue;
		}

		if( m_pMenuArray[*idxCurItem].fileName.IsEmpty() ||
			strFileName.CompareNoCase(m_pMenuArray[*idxCurItem].fileName))
		{
			if(lpNewArray)
			{
				CopyItem(&lpNewArray[*idxNewItem], &m_pMenuArray[*idxCurItem]);
				++(*idxNewItem);
			}
			++nArrayItems;
		}
		++(*idxCurItem);
	}

	return nArrayItems;
}

void
CIcadMenu::MnuInfoToMenuArray
(
 LPICMNUINFO	pMnuInfo,
 BOOL			bAppend
)
{
	if(!pMnuInfo)
		return;

	LPICMNUINFO		pmiCur;
	LPICMENUITEM	lpMenuArray, lpOldArray;
	int				ct, nOldItems;

	if(!bAppend)
		DeleteMenuArray();
	else
	{
		nOldItems = m_nMenuItems;
		lpOldArray = m_pMenuArray;
	}

	// Get a count of the number of total menu items so we can set up the array.
	m_nMenuItems = MnuInfoCount(pMnuInfo);
	m_pMenuArray = new ICMENUITEM[m_nMenuItems];
	lpMenuArray = m_pMenuArray;

	// Fill the elements of the array.
	for(ct = 0, pmiCur = pMnuInfo; pmiCur && ct < m_nMenuItems; ++ct)
	{
		if(!pmiCur->title.IsEmpty())
			lpMenuArray[ct].itemName = pmiCur->title;

		if(!pmiCur->tearOffName.IsEmpty())
			lpMenuArray[ct].tearOffName = pmiCur->tearOffName;

		if(!pmiCur->fileName.IsEmpty())
			lpMenuArray[ct].fileName = pmiCur->fileName;

		lpMenuArray[ct].lflgVisibility = g_DefaultMenuVisibility;	// Default visibility.
		
		if(pmiCur->bIsContextMenu)
		{
			lpMenuArray[ct].lflgVisibility |= ICAD_MN_RCO;	// Default visibility for pop0.
			lpMenuArray[ct].lflgEntVis = ICAD_MN_ENT;		// Default entity visibility for pop0.
		}
		

		SubMenuItemsFromMnuInfo(pmiCur, &ct);
		pmiCur = pmiCur->next;
	}

	if(bAppend)
	{
		// When appending, we want to insert the appended menu before the Window and
		// Help menu items, if present.
		int				nNewItems = nOldItems + m_nMenuItems;
		LPICMENUITEM	lpNewArray = new ICMENUITEM[nNewItems];

		// Find the index of the Window or Help menu.
		for(ct = 0; ct < nOldItems; ++ct)
		{
			if(lpOldArray[ct].nSubLevel || lpOldArray[ct].itemName.IsEmpty())
				continue;

			// Extract the & character from the string.
			CString	str(lpOldArray[ct].itemName);
			int		idx = str.Find('&');

			if(idx != -1)
			{
				CString	str2(str);
				str = str2.Left(idx);
				str += str2.Right(str2.GetLength() - idx - 1);
			}

			if(!str.CompareNoCase(ResourceString(IDC_ICADMENU_WINDOW_68, "Window")) || !str.CompareNoCase(ResourceString(IDC_ICADMENU_HELP_69, "Help")))
				break;
		}

		if(ct > nOldItems)
			ct = nOldItems;		// Safety

		// Copy the old menu into the array
		int	i;
		for(i = ct; i--; )
			CopyItem(&lpNewArray[i], &lpOldArray[i]);

		// Catenate the new menu onto the old menu in the array.
		for(i = m_nMenuItems; i--; )
			CopyItem(&lpNewArray[i + ct], &lpMenuArray[i]);

		if(ct < nOldItems)
		{	// Catenate the Window and Help menus back into the array.
			for(i = 0; i < nOldItems - ct; ++i)
				CopyItem(&lpNewArray[i + ct + m_nMenuItems], &lpOldArray[i + ct]);
		}

		// Clean up.
		delete [] lpOldArray;
		delete [] m_pMenuArray;
		m_pMenuArray = lpNewArray;
		m_nMenuItems = nNewItems;
		m_bMenuModified = TRUE;
	}
}

void
CIcadMenu::SubMenuItemsFromMnuInfo
(
 LPICMNUINFO	pmiParent,
 int*			idxCurArrayItem
)
{
	static int	ctRecur;	// Recursion counter.

	if(!pmiParent)
		return;

	if(!m_pMenuArray)
		return;

	LPICMNUINFO	pmiSubBeg = pmiParent->pmiSub;
	if(!pmiSubBeg)
		return;

	int				ct = *idxCurArrayItem;

	// Fill the array items
	BOOL		bAddSpacerBefore = FALSE;
	LPICMNUINFO	pmiCur = pmiSubBeg;
	++ctRecur;

	while(pmiCur || bAddSpacerBefore == TRUE)
	{
		// The item is a spacer.
		if( pmiCur &&
			(pmiCur->title.IsEmpty() ||	!pmiCur->title.Find("--"/*DNT*/) || !pmiCur->title.Find("~--"/*DNT*/)) )
		{
			bAddSpacerBefore = TRUE;
			pmiCur = pmiCur->next;
			continue;
		}

		if(!pmiCur)
		{
			// This code will only be reached if the submenu ends on a spacer.
			++ct;
			if(ct >= m_nMenuItems)
				break;

			m_pMenuArray[ct].itemName = ICAD_ENDSPACERSTR;

			if(!pmiParent->fileName.IsEmpty())
				m_pMenuArray[ct].fileName = pmiParent->fileName;

			m_pMenuArray[ct].lflgVisibility = ICAD_MN_ALL;
			if(pmiParent->bIsContextMenu)
			{
				m_pMenuArray[ct].lflgVisibility |= ICAD_MN_RCO;	// Default visibility for pop0.
				m_pMenuArray[ct].lflgEntVis = ICAD_MN_ENT;		// Default entity visibility for pop0.
			}

			if(!pmiParent->tearOffName.IsEmpty())
			{
				// Set the hidden flag if the parent has a tear-off name other than POPn.
				// VT 28-6-2002. Ignorecase. Reason: Fix for bug 78223 from BugZilla
				if(pmiParent->tearOffName.Left(3).CompareNoCase("pop"/*DNT*/))
					m_pMenuArray[ct].lflgVisibility |= ICAD_MN_HIDE;
			}

			m_pMenuArray[ct].nSubLevel = ctRecur;
			m_pMenuArray[ct].bAddSpacerBefore = TRUE;
			bAddSpacerBefore = FALSE;
			continue;
		}

		if(++ct >= m_nMenuItems)
			break;

		if(!pmiCur->title.IsEmpty())
		{
			// Convert \\t characters to tab characters(\t).
			char*	pszName = NULL;
			int		nLen = ic_escconv((char*)(LPCTSTR)pmiCur->title, NULL, '\\'/*DNT*/, "t\t"/*DNT*/, 0, 0);

			if(nLen > -1)
			{
				pszName = new char[nLen + 1];
				ic_escconv((char*)(LPCTSTR)pmiCur->title, pszName, '\\'/*DNT*/, "t\t"/*DNT*/, 0, 0);
			}

			m_pMenuArray[ct].itemName = pszName;
			delete [] pszName;
		}

		if(!pmiCur->tearOffName.IsEmpty())
			m_pMenuArray[ct].tearOffName = pmiCur->tearOffName;

		if(!pmiCur->command.IsEmpty())
			m_pMenuArray[ct].command = pmiCur->command;

		if(!pmiCur->helpString.IsEmpty())
			m_pMenuArray[ct].helpString = pmiCur->helpString;

		if(!pmiCur->fileName.IsEmpty())
			m_pMenuArray[ct].fileName = pmiCur->fileName;

		m_pMenuArray[ct].lflgVisibility = g_DefaultMenuVisibility;	// Default visibility.
		// Modified Cybage MM 05/11/2001 (DD/MM/YYYY)[
		// Reason: Fix for bug no 77908 from BugZilla
		// Set the ICAD_MN_TOM flag on if the parent menu is a POP0 menu and the flag is not already on
		if(!pmiParent->tearOffName.CompareNoCase("pop0"/*DNT*/) && !(m_pMenuArray[ct].lflgVisibility & ICAD_MN_TOM))
			m_pMenuArray[ct].lflgVisibility|=ICAD_MN_TOM; 
		// Modified Cybage MM 05/11/2001 (DD/MM/YYYY)]			

		if(pmiCur->bIsContextMenu)
		{
			m_pMenuArray[ct].lflgVisibility |= ICAD_MN_RCO;	// Default visibility for pop0.
			m_pMenuArray[ct].lflgEntVis = ICAD_MN_ENT;		// Default entity visibility for pop0.
		}

		// Set the hidden flag if the parent has a tear-off name other than POPn.
		// VT 28-6-2002. Ignorecase. Reason: Fix for bug 78223 from BugZilla
		if(pmiParent->tearOffName.Left(3).CompareNoCase("pop"/*DNT*/))
			m_pMenuArray[ct].lflgVisibility |= ICAD_MN_HIDE;

		m_pMenuArray[ct].nSubLevel = ctRecur;
		m_pMenuArray[ct].bAddSpacerBefore = bAddSpacerBefore;
		bAddSpacerBefore = FALSE;

		// If the item has a sub-submenu
		if(pmiCur->pmiSub)
			// Call this function recursively.
			SubMenuItemsFromMnuInfo(pmiCur, &ct);

		pmiCur = pmiCur->next;
	}	// while(

	// Set the current array item counter for the calling function.
	*idxCurArrayItem = ct;
	--ctRecur;
}

void
CIcadMenu::CopyItem
(
 LPICMENUITEM	lpDest,
 LPICMENUITEM	lpSrc
)
{
	if(!lpDest || !lpSrc)
		return;

	if(!lpSrc->itemName.IsEmpty())
		lpDest->itemName = lpSrc->itemName;

	if(!lpSrc->tearOffName.IsEmpty())
		lpDest->tearOffName = lpSrc->tearOffName;

	if(!lpSrc->command.IsEmpty())
		lpDest->command = lpSrc->command;

	if(!lpSrc->helpString.IsEmpty())
		lpDest->helpString = lpSrc->helpString;

	if(!lpSrc->chekVar.IsEmpty())
		lpDest->chekVar = lpSrc->chekVar;

	if(!lpSrc->grayVar.IsEmpty())
		lpDest->grayVar = lpSrc->grayVar;

	if(!lpSrc->fileName.IsEmpty())
		lpDest->fileName = lpSrc->fileName;

	lpDest->lflgVisibility = lpSrc->lflgVisibility;
	lpDest->lflgEntVis = lpSrc->lflgEntVis;
	lpDest->nSubLevel = lpSrc->nSubLevel;
	lpDest->bAddSpacerBefore = lpSrc->bAddSpacerBefore;
}

BOOL
CIcadMenu::SetMenu
(
 BOOL	bDestroyOld
)
{
	if(!m_hMenu )
		return FALSE;

	CMenu*	pMenu = new CMenu();
	if(!pMenu)
		return FALSE;

	pMenu->Attach(m_hMenu);

	CMenu*	pWinMenu = new CMenu();
	if(!pWinMenu)
	{
		delete pMenu;
		return FALSE;
	}

	HMENU	hMenuWindow = m_pMain->GetWindowMenuPopup(m_hMenu);
	if(!hMenuWindow)
	{
		delete pWinMenu;
		pWinMenu = NULL;
	}
	else
		pWinMenu->Attach(hMenuWindow);

	CMenu*	pOldMenu = m_pMain->MDISetMenu(pMenu, pWinMenu);

	if(pOldMenu && bDestroyOld)
		pOldMenu->DestroyMenu();

	HMENU	hNewMenu = pMenu->Detach();

	// Clean-up.
	delete pMenu;

	if(pWinMenu)
	{
		pWinMenu->Detach();
		delete pWinMenu;
		pWinMenu = NULL;
	}
	m_pMain->DrawMenuBar();
	return TRUE;
}

bool
CIcadMenu::IsMenuGroupLoaded
(
 CString	strGroup
)
{
	bool	bRetval = false;
	for(int i = 0; i < m_theLoadedMenuList.GetSize(); ++i)
	{
		if(m_theLoadedMenuList[i]->MatchesGroup(strGroup))
		{
			bRetval = true;
			break;
		}
	}

	return bRetval;
}

bool
CIcadMenu::FreeLoadedMenuList()
{
	for(int i = 0; i < m_theLoadedMenuList.GetSize(); ++i)
	{
		delete m_theLoadedMenuList[i];
		m_theLoadedMenuList[i] = NULL;
	}
	m_theLoadedMenuList.RemoveAll();
	return true;
}

bool
CIcadMenu::AddMenuToLoadedMenuList
(
 CString	strGroup,
 CString	strPathName
)
{
	if(IsMenuGroupLoaded(strGroup))
		return true;

	CIcadLoadedMenuInfo*	pLoadedMenuInfo = new CIcadLoadedMenuInfo(strGroup, strPathName);

	m_theLoadedMenuList.Add(pLoadedMenuInfo);

	return true;
}


bool
CIcadMenu::RemoveMenuFromLoadedMenuList
(
 CString	strGroup
)
{
	bool	bRetval = false;
	int		i;
	for(i = 0; i < m_theLoadedMenuList.GetSize(); ++i)
	{
		if(m_theLoadedMenuList[i]->MatchesGroup(strGroup))
			break;
	}

	if(i < m_theLoadedMenuList.GetSize())
	{
		CIcadLoadedMenuInfo*	pInfo = m_theLoadedMenuList[i];
		m_theLoadedMenuList.RemoveAt(i);
		delete pInfo;
		bRetval = true;
	}

	return bRetval;
}

bool
CIcadMenu::LoadMNLFilesFromLoadedMenuList()
{
	for(int i = 0; i < m_theLoadedMenuList.GetSize(); ++i)
	{
		CString	strMenuPathName = m_theLoadedMenuList[i]->GetPathName();
		cmd_loadlispmnl(strMenuPathName);
	}

	return true;
}


/////////////////////////////////////////////////////////////////////
//	Registry helpers
/////////////////////////////////////////////////////////////////////

LONG
IcadRegDeleteKey
(
 HKEY		hKey,
 LPCTSTR	lpSubKey
)
{
// This function deletes the specified key AND all subkeys.  This
// function is reentrant.

	char	szSubKey[81];
	CString	strEnum;
	LONG	lRet;
	HKEY	hSubKey;
	DWORD	dwIndex = 0, dwStrSize;
	FILETIME	ft;

	if((lRet = RegOpenKeyEx(hKey, lpSubKey, 0, KEY_READ, &hSubKey)) != ERROR_SUCCESS)
		return lRet;

	dwStrSize = sizeof(szSubKey);

	while(RegEnumKeyEx(hSubKey, dwIndex, szSubKey, &dwStrSize, NULL, NULL, NULL, &ft) != ERROR_NO_MORE_ITEMS)
	{
		strEnum.Format("%s\\%s"/*DNT*/, lpSubKey, szSubKey);
		IcadRegDeleteKey(hKey, strEnum);
		dwStrSize = sizeof(szSubKey);	// Reset the size of the string buffer.
	}

	RegCloseKey(hSubKey);
	return RegDeleteKey(hKey, lpSubKey);
}

LONG
IcadCopyKeyAndValue
(
 LPCTSTR	lpFromKey,
 LPCTSTR	lpToKey,
 BOOL		bForceRemove
)
{
// This function deletes the specified key AND all subkeys.  This
// function is reentrant.

	char	szSubKey[81], szValue[256];
	CString	strEnumFrom, strEnumTo;
	LONG	lRet;
	HKEY	hFromKey, hToKey;
	DWORD	dwIndex = 0, dwStrSize, dwType;
	FILETIME	ft;

	if((lRet = RegOpenKeyEx(HKEY_CLASSES_ROOT, lpFromKey, 0, KEY_READ, &hFromKey)) != ERROR_SUCCESS)
		return lRet;

	if(bForceRemove && (RegOpenKeyEx(HKEY_CLASSES_ROOT, lpToKey, 0, KEY_READ, &hToKey) == ERROR_SUCCESS))
	{
		RegCloseKey(hToKey);
		IcadRegDeleteKey(HKEY_CLASSES_ROOT, lpToKey);
	}

	dwStrSize = sizeof(szValue);
	char*	pszVal = NULL;

	if(RegQueryValueEx(hFromKey, NULL, NULL, &dwType, (LPBYTE)szValue, &dwStrSize) == ERROR_SUCCESS)
		pszVal = szValue;

	IcadSetKeyAndValue((LPTSTR)lpToKey, NULL, pszVal);
	dwStrSize = sizeof(szSubKey);

	while(RegEnumKeyEx(hFromKey, dwIndex, szSubKey, &dwStrSize, NULL, NULL, NULL, &ft) != ERROR_NO_MORE_ITEMS)
	{
		strEnumFrom.Format("%s\\%s"/*DNT*/, lpFromKey, szSubKey);
		strEnumTo.Format("%s\\%s"/*DNT*/, lpToKey, szSubKey);
		IcadCopyKeyAndValue(strEnumFrom, strEnumTo, FALSE);
		dwStrSize = sizeof(szSubKey);	// Reset the size of the string buffer.
		++dwIndex;
	}

	RegCloseKey(hFromKey);
	return lRet;
}

// IcadSetKeyAndValue
// Purpose:
//  Helper registry function that creates
//  a key, sets a value, and closes that key.
//
// Parameters:
//  pszKey          LPTSTR to the ame of the key
//  pszSubkey       LPTSTR ro the name of a subkey
//  pszValue        LPTSTR to the value to store
//
// Return Value:
//  BOOL            TRUE if successful, FALSE otherwise.
//
BOOL
IcadSetKeyAndValue
(
 LPTSTR	pszKey,
 LPTSTR	pszSubkey,
 LPTSTR	pszValue
)
{
	HKEY	hKey;
	TCHAR	szKey[256];

	lstrcpy(szKey, pszKey);

	if(pszSubkey)
	{
		lstrcat(szKey, TEXT("\\"/*DNT*/));
		lstrcat(szKey, pszSubkey);
	}

	if(RegCreateKeyEx(HKEY_CLASSES_ROOT, szKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL) != ERROR_SUCCESS)
		return FALSE;

	if(pszValue)
	{
		RegSetValueEx(hKey, NULL, 0, REG_SZ, (BYTE *)pszValue, (lstrlen(pszValue) + 1) * sizeof(TCHAR));
	}

	RegCloseKey(hKey);
	return TRUE;
}

// IcadSetKeyAndValue2 - a more advanced version of IcadSetKeyAndValue
// Purpose:
//  Helper registry function that creates
//  a key, sets a value, and closes that key.
//
// Parameters:
//  Root            HKEY corresponding to the registry Root - use predefined symbols - HKEY_CLASSES_ROOT, HKEY_CURRENT_USER etc
//  pszKey          LPTSTR to the name of the key
//  pszSubkey       LPTSTR to the name of a subkey
//  pszValueName    LPTSTR to the name of value to store - if NULL, the "Default" name is used
//  ValueType	   Use predefined Registry value types - REG_SZ for string, REG_DWORD for DWORD etc.
//  Value           The address of the value to be stored
//
// Return Value:
//  BOOL            TRUE if successful, FALSE otherwise.
//

BOOL
IcadSetKeyAndValue2
(
 HKEY	Root,
 LPTSTR	pszKey,
 LPTSTR	pszSubkey,
 LPTSTR	pszValueName,
 DWORD	ValueType,
 BYTE*	Value
)
{
	HKEY	hKey;
	TCHAR	szKey[256];

	lstrcpy(szKey, pszKey);

	if(pszSubkey)
	{
		lstrcat(szKey, TEXT("\\"/*DNT*/));
		lstrcat(szKey, pszSubkey);
	}

	if(RegCreateKeyEx(Root, szKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL) != ERROR_SUCCESS)
		return FALSE;

	if(Value)
	{
		int	size;
		if(ValueType == REG_SZ)
			size = (lstrlen((LPTSTR)Value) + 1) * sizeof(TCHAR);
		else
			size = sizeof(Value);

		RegSetValueEx(hKey, pszValueName, 0, ValueType, Value, size);
	}

	RegCloseKey(hKey);
	return TRUE;
}
