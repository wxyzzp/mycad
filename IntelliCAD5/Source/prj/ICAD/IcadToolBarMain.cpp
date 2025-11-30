/* File  : <DevDir>\source\prj\icad\IcadToolBarMain.cpp
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 * implementation file
 */

#include "Icad.h"/*DNT*/
#include "IcadToolBarMain.h"/*DNT*/
#include "IcadMenu.h"/*DNT*/
#include "Preferences.h"/*DNT*/
#include "IcadApp.h"/*DNT*/
#include "LicensedComponents.h"/*DNT*/
#include "IcadDef.h"
#include "win32misc.h"
#include "xml.h"/*DNT*/
#ifdef XML_TOOLBAR
#include "xmldoc.h"/*DNT*/
#endif

#ifdef _DEBUG

#ifndef USE_SMARTHEAP
 #define new DEBUG_NEW
#endif

#endif // _DEBUG

extern IcadToolBars	g_IcadToolBars[];
extern int			g_NumOfToolbars;
extern IcadCommands	g_MnuTbItems[];
extern int			g_nMnuTbItems;


HBITMAP LoadBitmapFromResource(HINSTANCE hInst, HRSRC hRsrc, int xd, int yd)
{
	HGLOBAL hglb;
	if ((hglb = LoadResource(hInst, hRsrc)) == NULL)
		return NULL;

	LPBITMAPINFOHEADER lpBitmap = (LPBITMAPINFOHEADER)LockResource(hglb);
	if (lpBitmap == NULL)
		return NULL;

	// make copy of BITMAPINFOHEADER so we can modify the color table
	int nColorTableSize;

	if (lpBitmap->biBitCount<16)
		nColorTableSize=(1 << (lpBitmap->biBitCount));
	else
		nColorTableSize=lpBitmap->biClrUsed;


	int nWidth = (int)lpBitmap->biWidth;
	int nHeight = (int)lpBitmap->biHeight;
	HDC hDCScreen = ::GetDC(NULL);
	HBITMAP hbm = ::CreateCompatibleBitmap(hDCScreen, nWidth, nHeight);

	if (hbm != NULL)
	{
		HDC hDCGlyphs = ::CreateCompatibleDC(hDCScreen);
		HBITMAP hbmOld = (HBITMAP)::SelectObject(hDCGlyphs, hbm);

		LPBYTE lpBits;
		lpBits = (LPBYTE)(lpBitmap + 1);
		lpBits += nColorTableSize * sizeof(RGBQUAD);

		StretchDIBits(hDCGlyphs, 0, 0, xd, yd, 0, 0, nWidth, nHeight,
			lpBits, (LPBITMAPINFO)lpBitmap, DIB_RGB_COLORS, SRCCOPY);
		SelectObject(hDCGlyphs, hbmOld);
		::DeleteDC(hDCGlyphs);
	}
	::ReleaseDC(NULL, hDCScreen);

	// free copy of bitmap info struct and resource itself
	::FreeResource(hglb);

	return hbm;
}


/////////////////////////////////////////////////////////////////////////////
// CIcadToolBarMain
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CIcadToolBarMain, CCmdTarget)

CIcadToolBarMain::CIcadToolBarMain() :	m_pToolBarList(NULL), m_pCreateBmpList(NULL), m_idxCurButtonDown(-1),
										m_pToolBarCurBtnDown(NULL), m_pFlyOutCurBtnDown(NULL), m_pToolBarCtrlCurBtnDown(NULL),
										m_bCustomize(FALSE), m_pHelpStrOutputWnd(NULL), m_bTbModified(FALSE),
										m_pCusTbDlg(NULL), m_pTbCtrlInsertMark(NULL),m_pibCurToolTip(NULL)
{
	m_rectInsertMark.SetRectEmpty();
}

CIcadToolBarMain::~CIcadToolBarMain()
{
	SetToolBarList(NULL);
	DeleteBmpList();
}

BEGIN_MESSAGE_MAP(CIcadToolBarMain, CCmdTarget)
	//{{AFX_MSG_MAP(CIcadToolBarMain)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// CIcadToolBarMain ToolBar (pointer) list functions

CPtrList*
CIcadToolBarMain::GetToolBarList()
{
	return m_pToolBarList;
}

void
CIcadToolBarMain::SetToolBarList
(
 CPtrList*	pToolBarList
)
{
	if(m_pToolBarList)
	{
		if(!m_pToolBarList->IsEmpty())
		{
			POSITION	pos = m_pToolBarList->GetHeadPosition();
			while(pos)
			{
				CIcadToolBar*	pToolBar = (CIcadToolBar*)m_pToolBarList->GetNext(pos);
				if(!pToolBar)
					continue;
				delete pToolBar;
			}
		}
		delete m_pToolBarList;
	}
	m_pToolBarList = pToolBarList;
}

int
CIcadToolBarMain::AddToBmpList
(
 CBitmap*	pBitmap
)
{
	if(!pBitmap)
		return -1;

	if(!m_pCreateBmpList)
		m_pCreateBmpList = new CPtrList();

	m_pCreateBmpList->AddTail(pBitmap);

	return m_pCreateBmpList->GetCount() - 1;
}

void
CIcadToolBarMain::DeleteBmpList()
{
	// Delete the flyout bitmaps.
	if(m_pCreateBmpList)
	{
		POSITION	pos = m_pCreateBmpList->GetHeadPosition();
		while(pos)
		{
			CBitmap*	pBitmap = (CBitmap*)m_pCreateBmpList->GetNext(pos);
			HBITMAP pBmp=(HBITMAP)pBitmap->Detach();
			if (pBmp) 
				if(!::DeleteObject(pBmp))	/*DG - 1.10.2002*/// TO DO: don't delete pBitmap and dont remove it from the list
					ASSERT(0);				// if it's not deleted by DeleteObject. Now it works, but I added the assert.
			delete pBitmap;
		}
		delete m_pCreateBmpList;
		m_pCreateBmpList = NULL;
	}
}

CWnd*
CIcadToolBarMain::GetHelpStringOutputWnd()
{
	return m_pHelpStrOutputWnd;
}

void
CIcadToolBarMain::SetHelpStringOutputWnd
(
 CWnd*	pWnd
)
{
	m_pHelpStrOutputWnd = pWnd;
}

CIcadTBbutton*
CIcadToolBarMain::GetCurToolTipPtr()
{
	return m_pibCurToolTip;
}

void
CIcadToolBarMain::SetCurToolTipPtr
(
 CIcadTBbutton*	pibCurToolTip
)
{
	m_pibCurToolTip = pibCurToolTip;
}

BOOL
CIcadToolBarMain::IsModified()
{
	return m_bTbModified;
}

void
CIcadToolBarMain::SetModifiedFlag
(
 BOOL	bModified
)
{
	m_bTbModified = bModified;
}

// CIcadToolBarMain Create functions

BOOL
CIcadToolBarMain::CreateFromRegistry
(
 LPICTOOLBARCREATESTRUCT	lpCreateToolBar
)
{
	int		nBadRet;
	HKEY	hKeyTb;
	CString	strKey, strButtonKey, strFlyOutKey;
	char*	pstrText;
	int		ct;
	//long	lDefID;
	long_double	lDefID;
	UINT	nID;

	if(lpCreateToolBar)
	{
		m_pParentFrame = lpCreateToolBar->pParentFrame;
		m_bToolTips = lpCreateToolBar->bToolTips;
		m_bLargeButtons = lpCreateToolBar->bLargeButtons;
		m_bColorButtons = lpCreateToolBar->bColorButtons;
	}

	PreferenceKey	toolbarKey(HKEY_CURRENT_USER, "ToolBars", FALSE);
	HKEY	hKeyTbRoot = toolbarKey;
	DWORD	dwRetVal, dwSize;

	if(!hKeyTbRoot)
		return FALSE;

	// Get the number of toolbars.
	CPtrList*	pTbList = new CPtrList();
	int			nToolBars;
	dwSize = sizeof(int);
	if(RegQueryValueEx(hKeyTbRoot, "nBars"/*DNT*/, NULL, &dwRetVal, (LPBYTE)&nToolBars, (LPDWORD)&dwSize) != ERROR_SUCCESS)
	{
		nBadRet = -__LINE__;
		goto badout;
	}

	// Create the toolbars.
	for(ct = 0; ct < nToolBars; ++ct)
	{
		// Get the toolbar key
		strKey.Format("ToolBar-%d"/*DNT*/, ct);
		if(RegOpenKeyEx(hKeyTbRoot, strKey, 0, KEY_READ, &hKeyTb) != ERROR_SUCCESS)
			continue;

		// Get the name of the toolbar.
		pstrText = NULL;
		int	nStrLen;
		if(RegQueryValueEx(hKeyTb, "Name"/*DNT*/, NULL, &dwRetVal, NULL, (LPDWORD)&nStrLen) == ERROR_SUCCESS)
		{
			pstrText = new char[++nStrLen];
			memset(pstrText, 0, nStrLen);
			RegQueryValueEx(hKeyTb, "Name"/*DNT*/, NULL, &dwRetVal, (LPBYTE)pstrText, (LPDWORD)&nStrLen);
		}

		// Get the toolbar ID the the toolbar was created with.
		dwSize = sizeof(UINT);
		if(RegQueryValueEx(hKeyTb, "ToolBarID"/*DNT*/, NULL, &dwRetVal, (LPBYTE)&nID, &dwSize) != ERROR_SUCCESS)
			nID = 0;

		// Get the default toolbar ID for reseting the buttons in Customize.
		//dwSize = sizeof(long);
		dwSize = sizeof(long_double);
		if(RegQueryValueEx(hKeyTb, "DefID"/*DNT*/, NULL, &dwRetVal, (LPBYTE)&lDefID, &dwSize) != ERROR_SUCCESS)
			lDefID = 0;

		// Create a new toolbar.
		CIcadToolBar*	pNewToolBar = CreateNewToolBar(nID, lDefID, pstrText);
		if(!pNewToolBar)
		{
			delete [] pstrText;
			continue;
		}

		pTbList->AddTail(pNewToolBar);
		delete [] pstrText;
		pstrText = NULL;

		// Get the mnu name that the toolbar was created from.
		if(RegQueryValueEx(hKeyTb, "FileName"/*DNT*/, NULL, &dwRetVal, NULL, (LPDWORD)&nStrLen) == ERROR_SUCCESS)
		{
			pstrText = new char[++nStrLen];
			memset(pstrText, 0, nStrLen);
			RegQueryValueEx(hKeyTb, "FileName"/*DNT*/, NULL, &dwRetVal, (LPBYTE)pstrText, (LPDWORD)&nStrLen);
			pNewToolBar->m_strMnuFileName = pstrText;
			delete [] pstrText;
		}

		// Get the unload name for the toolbar.
		pstrText = NULL;
		if(RegQueryValueEx(hKeyTb, "UnloadName"/*DNT*/, NULL, &dwRetVal, NULL, (LPDWORD)&nStrLen) == ERROR_SUCCESS)
		{
			pstrText = new char[++nStrLen];
			memset(pstrText, 0, nStrLen);
			RegQueryValueEx(hKeyTb, "UnloadName"/*DNT*/, NULL, &dwRetVal, (LPBYTE)pstrText, (LPDWORD)&nStrLen);
			pNewToolBar->m_strGroupName = pstrText;
			delete [] pstrText;
		}

		// Add the buttons to the toolbar.
		if((nBadRet = pNewToolBar->LoadButtonsFromReg(strKey, hKeyTb)) < 0)
			goto badout;

		// Enable docking and hide the toolbar.
		pNewToolBar->EnableDocking(CBRS_ALIGN_ANY);
		RegCloseKey(hKeyTb);
	}	// for(

	SetToolBarList(pTbList);
	// Now that the toolbars and buttons are created set the visibility (ENABLED, GRAYED, or HIDDEN).
	// This should be fast enough to be worth looping through the structure again.
	SetButtonVisibility(lpCreateToolBar->lflgVisibility, FALSE);

	return TRUE;

badout:

	CString	str;
	str.Format(ResourceString(IDC_ICADTOOLBARMAIN_ERROR_CR_2, "Error creating toolbar (%d). Default toolbars will be loaded."), -nBadRet);
	m_pParentFrame->MessageBox(str);

	// Free the toolbars and the toolbar items in the pointer list.
	if(!pTbList->IsEmpty())
	{
		POSITION	pos = pTbList->GetHeadPosition();
		while(pos)
		{
			CIcadToolBar*	pToolBar = (CIcadToolBar*)pTbList->GetNext(pos);
			delete pToolBar;
		}
		pTbList->RemoveAll();
	}

	return FALSE;
}

#ifdef XML_TOOLBAR
void
CIcadToolBarMain::SaveToXML(CString& fName, CMapPtrToPtr* pIncludedBars)
{
	XMLDocument doc;
	CPtrList*	pToolBarList = GetToolBarList();
	POSITION	pos = pToolBarList->GetHeadPosition();
	void* tmpPtr;

#ifdef XML_TOOLBAR_AS_DEFAULT
	if(fName.IsEmpty())
		fName = "e:\\toolbar.xml";
#error "need to define system variable with default toolbar.xml or undefine XML_TOOLBAR_AS_DEFAULT in IcadToolBar.h"
#endif

	doc.setFile(XMLString(fName), false);
	doc.rootNode().name() = "settings";
	XMLNode* pTbsNode = new XMLNode("toolbars");
	doc.rootNode().addChild(pTbsNode);

	// Loop through the toolbars and write the toolbar and button information to the node.
	for(int j, ct = 0; pos; ++ct)
	{
		CIcadToolBar*	pToolBar = (CIcadToolBar*)pToolBarList->GetNext(pos);

		if(!pToolBar)
			continue;

		if(pIncludedBars && pIncludedBars->Lookup(pToolBar, tmpPtr))
			continue;

		XMLNode* pCurTB = new XMLNode("ToolBar");
		pTbsNode->addChild(pCurTB);

		pToolBar->SaveToXML(*pCurTB);
	}	// for

	XMLString stateDocked;

	SerializeDockedState(stateDocked, true /* bStoring */, pIncludedBars);
	pTbsNode->addAttribute("DockedBarsState", stateDocked);

	doc.write();

	SetModifiedFlag(FALSE);
}

void addPredefinedMacrosToXML(XMLDocument& doc)
{
#ifdef BUILD_WITH_VBA
	doc.addPredefinedMacro("BUILD_WITH_VBA", "1");
#endif
#ifdef BUILD_WITH_RENDERING_SUPPORT
	doc.addPredefinedMacro("BUILD_WITH_RENDERING_SUPPORT", "1");
#endif
#ifdef BUILD_WITH_RASTER_SUPPORT
	doc.addPredefinedMacro("BUILD_WITH_RASTER_SUPPORT", "1");
#endif
#ifdef BUILD_WITH_WORD_SPELLCHECK
	doc.addPredefinedMacro("BUILD_WITH_WORD_SPELLCHECK", "1");
#endif
#ifdef BUILD_WITH_INSO_SPELLCHECK
	doc.addPredefinedMacro("BUILD_WITH_INSO_SPELLCHECK", "1");
#endif
#ifdef BUILD_WITH_ACIS_SUPPORT
	doc.addPredefinedMacro("BUILD_WITH_ACIS_SUPPORT", "1");
	#ifdef BUILD_WITH_LIMITED_ACIS_SUPPORT
		doc.addPredefinedMacro("BUILD_WITH_LIMITED_ACIS_SUPPORT", "1");
	#endif
#endif

#ifdef BUILD_WITH_XRM_SUPPORT
	doc.addPredefinedMacro("BUILD_WITH_XRM_SUPPORT", "1");
#endif
#ifdef BUILD_WITH_QRENDERING_SUPPORT
	doc.addPredefinedMacro("BUILD_WITH_QRENDERING_SUPPORT", "1");
#endif
#ifdef BUILD_WITH_FLYOVERSNAP_SUPPORT
	doc.addPredefinedMacro("BUILD_WITH_FLYOVERSNAP_SUPPORT", "1");
#endif
}

BOOL
CIcadToolBarMain::ReadFromXML(LPCTSTR lpszFile)
{
	XMLDocument doc;

	doc.setFile(lpszFile);
	
	XMLNode* pToolBarsNode = &doc.rootNode();
	
	if(!pToolBarsNode)
		return FALSE;

	if(!(pToolBarsNode = pToolBarsNode->childNode("toolbars")))
		return FALSE;

	CPtrList*	pTbList = new CPtrList();
	XMLNode::childIterator toolbarIt = pToolBarsNode->childBegin();
	XMLNode* pCurTB;
	int nBadRet;
	UINT toolBarID;
	//long defID;
	long_double defID;
	XMLString name;
	SetToolBarList(pTbList);

	for(; toolbarIt != pToolBarsNode->childEnd(); ++toolbarIt)
	{
		pCurTB = *toolbarIt;
		// Get the name, ID, defID of the toolbar.
		toolBarID = pCurTB->attribute("ToolBarID")->toInt();
		//defID = pCurTB->attribute("DefID")->toInt();
		defID = pCurTB->attribute("DefID")->tolong_double();
		name = *pCurTB->attribute("Name");
		
		// Create a new toolbar.
		CIcadToolBar*	pNewToolBar = CreateNewToolBar(toolBarID, defID, name.c_str());
		if(!pNewToolBar)
			continue;
		pTbList->AddTail(pNewToolBar);
		if((nBadRet = pNewToolBar->ReadFromXML(**toolbarIt)) < 0)
		{
			delete pNewToolBar;
			// Free the toolbars and the toolbar items in the pointer list.
			if(!pTbList->IsEmpty())
			{
				POSITION	pos = pTbList->GetHeadPosition();
				while(pos)
				{
					CIcadToolBar*	pToolBar = (CIcadToolBar*)pTbList->GetNext(pos);
					delete pToolBar;
				}
				pTbList->RemoveAll();
			}
			return FALSE;
		}
	}	// for(

	XMLString* pDockedState = pToolBarsNode->attribute("DockedBarsState");
	if(pDockedState && pDockedState->length())
		SerializeDockedState(*pDockedState, false /* bStoring */);

	OnVisibilityChanged();
	m_pParentFrame->RecalcLayout();


	return TRUE;
}

#ifdef XML_TOOLBAR_AS_DEFAULT
BOOL
CIcadToolBarMain::ReadFromXML
(
 LPICTOOLBARCREATESTRUCT	lpCreateToolBar
)
{
	BOOL res;

	if(lpCreateToolBar)
	{
		m_pParentFrame = lpCreateToolBar->pParentFrame;
		m_bToolTips = lpCreateToolBar->bToolTips;
		m_bLargeButtons = lpCreateToolBar->bLargeButtons;
		m_bColorButtons = lpCreateToolBar->bColorButtons;
	}

#error "need to define system variable with default toolbar.xml or undefine XML_TOOLBAR_AS_DEFAULT in IcadToolBar.h"
	if((res = ReadFromXML("e:\\toolbar.xml")) == TRUE)
		// Now that the toolbars and buttons are created set the visibility (ENABLED, GRAYED, or HIDDEN).
		// This should be fast enough to be worth looping through the structure again.
		SetButtonVisibility(lpCreateToolBar->lflgVisibility, FALSE);

	return res;
}
#endif
#endif

CIcadToolBar*
CIcadToolBarMain::CreateNewToolBar
(
 int		idxToolBar,
 //long		lDefID,
 long_double		lDefID,
 LPCTSTR	pszTitle
)
{
	// Create a new toolbar.
	CIcadToolBar*	pNewToolBar = new CIcadToolBar();
	if(!pNewToolBar)
		return NULL;

	pNewToolBar->m_pTbMain = this;
	pNewToolBar->m_nID = IDR_ICAD_TOOLBAR0 + idxToolBar;
	pNewToolBar->m_lDefID = lDefID;
	if(idxToolBar == 14 || idxToolBar == 3)
		pNewToolBar->Create(m_pParentFrame, WS_CHILD | CBRS_TOP  | CBRS_HIDE_INPLACE, pNewToolBar->m_nID);
	else
		pNewToolBar->Create(m_pParentFrame, WS_CHILD | CBRS_TOP | CBRS_SIZE_DYNAMIC | CBRS_HIDE_INPLACE, pNewToolBar->m_nID);

	CToolBarCtrl*	pTbCtrl = &(pNewToolBar->GetToolBarCtrl());

	if(m_bToolTips)
		pNewToolBar->SetBarStyle(pNewToolBar->GetBarStyle() | CBRS_TOOLTIPS);

#ifndef ICAD_OLDTOOLBARS
	pTbCtrl->ModifyStyle(0, TBSTYLE_FLAT);
	pNewToolBar->m_cyTopBorder = 4;
	pNewToolBar->m_cyBottomBorder = 0;
	pNewToolBar->m_cxLeftBorder = 10;
	pNewToolBar->m_cxRightBorder = 3;
	pNewToolBar->m_bIsStyleFlat = TRUE;
#endif

	pNewToolBar->SetDllVersion();
	pNewToolBar->SetWindowText(pszTitle);
	pNewToolBar->SetOwner(m_pParentFrame);

	return pNewToolBar;
}

BOOL
CIcadToolBarMain::CreateDefault
(
 LPICTOOLBARCREATESTRUCT	lpCreateToolBar
)
{
	short			ctOuter, ctStart = 0;
	IcadToolBars*	pToolBars = g_IcadToolBars;
	int				idxToolBar = 0;

	if(lpCreateToolBar)
	{
		m_pParentFrame = lpCreateToolBar->pParentFrame;
		m_bToolTips = lpCreateToolBar->bToolTips;
		m_bLargeButtons = lpCreateToolBar->bLargeButtons;
		m_bColorButtons = lpCreateToolBar->bColorButtons;
	}

	CPtrList*	pTbList = new CPtrList();
CString		tmpToolBarName;

	for(ctOuter = 0; ctOuter < g_NumOfToolbars; ++ctOuter)
	{
		if(pToolBars[ctOuter].FlyOut)
			continue;

		// Create a new toolbar.
tmpToolBarName.LoadString(ICADTOOLBARS + ctOuter);

		CIcadToolBar*	pNewToolBar = CreateNewToolBar(idxToolBar++, pToolBars[ctOuter].lToolID, LPCTSTR(tmpToolBarName));

		if(!pNewToolBar)
			continue;

		// Add the buttons to the toolbar.
		if(!CreateDefaultButtons(pNewToolBar, pNewToolBar->m_lDefID))
		{
			delete pNewToolBar;
			continue;
		}

		pTbList->AddTail(pNewToolBar);
		// Enable docking and hide the toolbar.
		pNewToolBar->EnableDocking(CBRS_ALIGN_ANY);
	}

	SetToolBarList(pTbList);

	// Now that the toolbars and buttons are created set the visibility (ENABLED, GRAYED, or HIDDEN).
	// This should be fast enough to be worth looping through the structure again.
	SetButtonVisibility(lpCreateToolBar->lflgVisibility, FALSE);

	return TRUE;
}

BOOL
CIcadToolBarMain::CreateDefaultButtons
(
 CIcadToolBar*	pToolBar,
 //long			lToolID
 long_double			lToolID
)
{
	int				ct, nButtons, idxButton, tmpIndex;
	LPTBBUTTON		pTbButton;
	CToolBarCtrl*	pToolBarCtrl = &(pToolBar->GetToolBarCtrl());

	// Set the sizes for the buttons for this toolbar.
	if(m_bLargeButtons)
	{
		pToolBarCtrl->SetBitmapSize(CSize(ICAD_LG_IMGX, ICAD_LG_IMGY));
		pToolBarCtrl->SetButtonSize(CSize(ICAD_LG_BUTTONX, ICAD_LG_BUTTONY));
	}
	else
	{
		pToolBarCtrl->SetBitmapSize(CSize(ICAD_SM_IMGX, ICAD_SM_IMGY));
		pToolBarCtrl->SetButtonSize(CSize(ICAD_SM_BUTTONX, ICAD_SM_BUTTONY));
	}

	// Loop through once to get a count of all buttons for this toolbar.
	for(ct = 0, nButtons = 0; ct < g_nMnuTbItems; ++ct)
	{
		if(!g_MnuTbItems[ct].lToolID)
			continue;

		if((lToolID & g_MnuTbItems[ct].lToolID) && (g_MnuTbItems[ct].lflgVisibility & ICAD_MN_LEV))
			// Only count buttons that will be visible based on a user level.
			++nButtons;
	}

	if(nButtons <= 0)
		return FALSE;

	// Allocate the array of button structures.
	pTbButton = new TBBUTTON[nButtons];

	// Fill the array of button structures.
	for(ct = 0, idxButton = 0; ct < g_nMnuTbItems; ++ct)
	{
		if(!g_MnuTbItems[ct].lToolID)
			continue;

		if((lToolID & g_MnuTbItems[ct].lToolID) && (g_MnuTbItems[ct].lflgVisibility & ICAD_MN_LEV))
		{
			// If the item is a separator add it and continue.
			if(!(*g_MnuTbItems[ct].ToolTip) && !(*g_MnuTbItems[ct].Command))
			{
				pTbButton[idxButton].iBitmap = ICAD_SEP_WIDTH;
				pTbButton[idxButton].idCommand = 0;
				pTbButton[idxButton].fsState = TBSTATE_ENABLED;
				pTbButton[idxButton].fsStyle = TBSTYLE_SEP;
				pTbButton[idxButton].dwData = 0;
				pTbButton[idxButton].iString = 0;
				++idxButton;
				continue;
			}

			// Initialize the CIcadTBbutton structure.
			CIcadTBbutton*	picButton = new CIcadTBbutton;
			picButton->m_idxFlyOutImage = -1;
			picButton->m_bAddSpacerBefore = FALSE;
			picButton->m_bChgParentBtnImg = TRUE;

			// Use the dwData member of the TBBUTTON structure as a pointer to the
			// CIcadTBbutton structure.
			pTbButton[idxButton].dwData = (DWORD)picButton;

			if(!g_MnuTbItems[ct].lFlyOutID)
			{
				// The button is NOT a flyout.
				// Set the IcadButton information
				picButton->m_idToolBar = 0;	// TODO ~|

tmpIndex = g_MnuTbItems[ct].resourceIndex + (g_MnuTbItems[ct].pAcadTbStr? 1 : 0);

				if(g_MnuTbItems[ct].ToolTip)
					picButton->m_toolTip.LoadString(tmpIndex++);

				if(g_MnuTbItems[ct].HelpString)
					picButton->m_helpString.LoadString(tmpIndex);

				if(g_MnuTbItems[ct].Command)
					picButton->m_command = g_MnuTbItems[ct].Command;

				if(g_MnuTbItems[ct].strChekVar)
					picButton->m_pushVar = g_MnuTbItems[ct].strChekVar;

				if(g_MnuTbItems[ct].strGreyVar)
					picButton->m_grayVar = g_MnuTbItems[ct].strGreyVar;

				picButton->m_BMPIdSmColor = g_MnuTbItems[ct].BMPIdSmColor;
				picButton->m_BMPIdLgColor = g_MnuTbItems[ct].BMPIdLgColor;
				picButton->m_BMPIdSmBW = g_MnuTbItems[ct].BMPIdSmBW;
				picButton->m_BMPIdLgBW = g_MnuTbItems[ct].BMPIdLgBW;
				picButton->m_lflgVisibility = g_MnuTbItems[ct].lflgVisibility;

				// Load the bitmap for this button.
				pTbButton[idxButton].iBitmap = AddIcadBitmap(picButton, 0);
			}
			else
			{
				// The button is a flyout.
				// Set the value of m_bChgParentBtnImg for the parent button of this flyout.
				int	ctToolBars;

				for(ctToolBars = 0; ctToolBars < g_NumOfToolbars; ++ctToolBars)
				{
					if(g_IcadToolBars[ctToolBars].lToolID & g_MnuTbItems[ct].lFlyOutID)
						break;
				}

				picButton->m_bChgParentBtnImg = g_IcadToolBars[ctToolBars].OtherIcon;

				// Create the CIcadTBbutton linked list that defines the buttons for the flyout.
				int				ctFly;
				CIcadTBbutton	*pibNewFlyOut, *pibCur;

				for(ctFly = 0; ctFly < g_nMnuTbItems; ++ctFly)
				{
					if(!g_MnuTbItems[ctFly].lToolID)
						continue;

					if(!(g_MnuTbItems[ct].lFlyOutID & g_MnuTbItems[ctFly].lToolID))
						continue;

					// Set the Icad button information.
					pibNewFlyOut = new CIcadTBbutton;
					pibNewFlyOut->m_bAddSpacerBefore = FALSE;
					pibNewFlyOut->m_bChgParentBtnImg = TRUE;
					pibNewFlyOut->m_idToolBar = 0;

tmpIndex = g_MnuTbItems[ctFly].resourceIndex + (g_MnuTbItems[ctFly].pAcadTbStr? 1 : 0);

					if(g_MnuTbItems[ctFly].ToolTip)
						pibNewFlyOut->m_toolTip.LoadString(tmpIndex++);

					if(g_MnuTbItems[ctFly].HelpString)
						pibNewFlyOut->m_helpString.LoadString(tmpIndex);

					if(g_MnuTbItems[ctFly].Command)
						pibNewFlyOut->m_command = g_MnuTbItems[ctFly].Command;

					if(g_MnuTbItems[ctFly].strChekVar)
						pibNewFlyOut->m_pushVar = g_MnuTbItems[ctFly].strChekVar;

					if(g_MnuTbItems[ctFly].strGreyVar)
						pibNewFlyOut->m_grayVar = g_MnuTbItems[ctFly].strGreyVar;

					pibNewFlyOut->m_BMPIdSmColor = g_MnuTbItems[ctFly].BMPIdSmColor;
					pibNewFlyOut->m_BMPIdLgColor = g_MnuTbItems[ctFly].BMPIdLgColor;
					pibNewFlyOut->m_BMPIdSmBW = g_MnuTbItems[ctFly].BMPIdSmBW;
					pibNewFlyOut->m_BMPIdLgBW = g_MnuTbItems[ctFly].BMPIdLgBW;
					pibNewFlyOut->m_lflgVisibility = g_MnuTbItems[ctFly].lflgVisibility;
					pibNewFlyOut->m_pibFlyOut = NULL;
					pibNewFlyOut->m_pibCurFlyOut = NULL;

					// Add the flyout bitmap to the toolbar
					pibNewFlyOut->m_idxFlyOutImage = AddIcadBitmap(pibNewFlyOut, 1);

					if(!picButton->m_pibFlyOut)
						picButton->m_pibFlyOut = pibCur = pibNewFlyOut;
					else
						pibCur = pibCur->m_pibFlyOut = pibNewFlyOut;
				}

				picButton->m_pibCurFlyOut = picButton->m_pibFlyOut;
				pTbButton[idxButton].iBitmap = picButton->m_pibCurFlyOut->m_idxFlyOutImage;
			}

			pTbButton[idxButton].idCommand = GetNewTbID();
			pTbButton[idxButton].fsState = TBSTATE_ENABLED;
			pTbButton[idxButton].fsStyle = TBSTYLE_BUTTON;
			pTbButton[idxButton].iString = 0;
			++idxButton;
		}
	}

	AddToolBarBitmap(pToolBar, NULL);
	pToolBarCtrl->AddButtons(nButtons, pTbButton);
	delete pTbButton;
	return TRUE;
}

CBitmap*
CIcadToolBarMain::CreateToolBarBitmap()
{
// This function creates one bitmap containing several images stored in m_pCreateBmpList.

	BOOL		bRet = TRUE;
	CBitmap*	pBitmap = NULL;
	POSITION	pos = NULL;
	int			idx = 0, nWidth = ICAD_SM_IMGX, nHeight = ICAD_SM_IMGY;
	CBitmap*	pbmMem = NULL;

	if(!m_pCreateBmpList)
		return NULL;

	int			nButtons = m_pCreateBmpList->GetCount();

	// Create the mother bitmap.
	CBitmap		*pbmOld = NULL, *pbmOld2 = NULL;	/*DG - 1.10.2002*/// We will store old bmp of pMemDC2 in pbmOld2.

	CDC			*pDC = m_pParentFrame->GetDC(),
				*pMemDC = new CDC(),
				*pMemDC2 = new CDC();

	if(!pMemDC->CreateCompatibleDC(pDC) || !pMemDC2->CreateCompatibleDC(pDC))
	{
		bRet = FALSE;
		goto out;
	}

	pbmMem = new CBitmap();

	if(m_bLargeButtons)
	{
		nWidth = ICAD_LG_IMGX;
		nHeight = ICAD_LG_IMGY;
	}

	pbmMem->CreateCompatibleBitmap(pDC, nWidth * nButtons, nHeight);
	pbmOld = pMemDC->SelectObject(pbmMem);

	// BitBlt all the bitmaps into the mother bitmap.
	pos = m_pCreateBmpList->GetHeadPosition();

	for(idx = 0; pos; ++idx)
	{
		pBitmap = (CBitmap*)m_pCreateBmpList->GetNext(pos);
		if(idx)
			pMemDC2->SelectObject(pBitmap);
		else
			pbmOld2 = pMemDC2->SelectObject(pBitmap);
		pMemDC->BitBlt(nWidth * idx, 0, nWidth, nHeight, pMemDC2, 0, 0, SRCCOPY);
	}

	pMemDC->SelectObject(pbmOld);
	pMemDC2->SelectObject(pbmOld2);

out:
	DeleteBmpList();

	if(pDC)
		m_pParentFrame->ReleaseDC(pDC);

	delete pMemDC;
	delete pMemDC2;

	if(!bRet && pbmMem)
	{
		delete pbmMem;
		pbmMem = NULL;
	}

	return pbmMem;
}

BOOL
CIcadToolBarMain::CreateToolBarFromList
(
 LPCTSTR	lpszFile,
 LPCTSTR	lpszGroupName,
 CPtrList*	ptrlist,
 LPCTSTR	title,
 int		xpos,
 int		ypos,
 int		placement,
 int		nrows,
 BOOL		show
)
{
	BOOL			bRet = TRUE;
	CPtrList		ButtonList;
	CIcadTBbutton*	picButton;

	// Pointer to the list of existing toolbars.  The toolbars we create from the
	// file will be added to this list.
	CPtrList*	pTbList = GetToolBarList();

	// Create a new toolbar.
	CIcadToolBar*	pToolBar = NULL;
	UINT			idxToolBar=0;
	POSITION		pos = pTbList->GetHeadPosition();

	while(pos)
	{
		pToolBar = (CIcadToolBar*)pTbList->GetAt(pos);
		if(!pToolBar)
			continue;

		idxToolBar = max(idxToolBar, pToolBar->m_nID - IDR_ICAD_TOOLBAR0);
		pTbList->GetNext(pos);
	}

	if(pTbList->GetCount() > 0)
		++idxToolBar;

	CIcadToolBar*	pNewToolBar = CreateNewToolBar(idxToolBar, 0, title);

	if(!pNewToolBar)
		return FALSE;

	CToolBarCtrl*	pTbCtrl = &(pNewToolBar->GetToolBarCtrl());

	// Enable docking for the toolbar.
	pNewToolBar->EnableDocking(CBRS_ALIGN_ANY);
	pNewToolBar->m_strMnuFileName = lpszFile;
	pNewToolBar->m_strGroupName = lpszGroupName;

	// Look for a resource dll and load it.
	char*	pszResDLL = new char[strlen(lpszFile) + 5]; // +5 just in case an extension isn't specified.
	strcpy(pszResDLL, lpszFile);
	ic_setext(pszResDLL, "dll"/*DNT*/);

	HINSTANCE hResDLL = LoadLibraryEx(pszResDLL,NULL,LOAD_LIBRARY_AS_DATAFILE);
	//HINSTANCE	hResDLL = LoadLibrary(pszResDLL);
	delete [] pszResDLL;

	LPTBBUTTON	pTbButton = NULL;
	// Set the sizes for the buttons for this toolbar.
	if(m_bLargeButtons)
	{
		pTbCtrl->SetBitmapSize(CSize(ICAD_LG_IMGX, ICAD_LG_IMGY));
		pTbCtrl->SetButtonSize(CSize(ICAD_LG_BUTTONX, ICAD_LG_BUTTONY));
	}
	else
	{
		pTbCtrl->SetBitmapSize(CSize(ICAD_SM_IMGX, ICAD_SM_IMGY));
		pTbCtrl->SetButtonSize(CSize(ICAD_SM_BUTTONX, ICAD_SM_BUTTONY));
	}

	int	idxButtons = 0,
		nButtons = ptrlist->GetCount();

	// Make room for the spacers.
	pos = ptrlist->GetHeadPosition();
	while(pos)
	{
		picButton = (CIcadTBbutton*)ptrlist->GetNext(pos);
		if(picButton->m_bAddSpacerBefore)
			++nButtons;
	}

	if(nButtons <= 0)
	{
		bRet = FALSE;
		goto out;
	}

	pNewToolBar->m_bEmpty = FALSE;

	// Allocate the array of button structures.
	pTbButton = new TBBUTTON[nButtons];

	while(!ptrlist->IsEmpty())
	{

		picButton = (CIcadTBbutton*)ptrlist->GetHead();
		ptrlist->RemoveHead();

		picButton->m_idToolBar = IDR_ICAD_TOOLBAR0 + idxToolBar;

		int	iBitmap;

		if(picButton->m_pibFlyOut)
		{
			// If the button is a flyout, create the flyout bitmaps.
			CIcadTBbutton*	pibCur;
			for(pibCur = picButton->m_pibFlyOut; pibCur; pibCur = pibCur->m_pibFlyOut)
				pibCur->m_idxFlyOutImage = AddIcadBitmap(pibCur, 1, NULL, NULL, lpszFile, hResDLL);

			if (picButton->m_bChgParentBtnImg)
				iBitmap = picButton->m_pibCurFlyOut->m_idxFlyOutImage;
			else
				iBitmap = AddIcadBitmap(picButton, 1, NULL, NULL, lpszFile, hResDLL);
		}
		else
			// The button is NOT a flyout.
			// Load the bitmap for the button.
			iBitmap = AddIcadBitmap(picButton, 0, NULL, NULL, lpszFile, hResDLL);

		if(picButton->m_bAddSpacerBefore)
		{
			pTbButton[idxButtons].iBitmap = ICAD_SEP_WIDTH;
			pTbButton[idxButtons].idCommand = 0;
			pTbButton[idxButtons].fsState = TBSTATE_ENABLED;
			pTbButton[idxButtons].fsStyle = TBSTYLE_SEP;
			pTbButton[idxButtons].dwData = 0;
			pTbButton[idxButtons].iString = 0;
			++idxButtons;
		}

		if(iBitmap == -1)
		{
			// User canceled when a bitmap couldn't be found.
			delete pNewToolBar;
			FreeIcadButtonData(picButton);

			for(idxButtons--; idxButtons >= 0; --idxButtons)
			{
				picButton = (CIcadTBbutton*)pTbButton[idxButtons].dwData;
				FreeIcadButtonData(picButton);
			}

			delete pTbButton;

			POSITION	pos = ptrlist->GetHeadPosition();
			while(pos)
			{
				picButton = (CIcadTBbutton*)ptrlist->GetNext(pos);
				FreeIcadButtonData(picButton);
			}

			ptrlist->RemoveAll();
			bRet = FALSE;
			goto out;
		}

		pTbButton[idxButtons].iBitmap = iBitmap;
		pTbButton[idxButtons].idCommand = GetNewTbID();
		pTbButton[idxButtons].fsState = TBSTATE_ENABLED;
		pTbButton[idxButtons].fsStyle = TBSTYLE_BUTTON;
		pTbButton[idxButtons].dwData = (DWORD)picButton;
		pTbButton[idxButtons].iString = 0;
		++idxButtons;
	}

	AddToolBarBitmap(pNewToolBar, NULL);
	pTbCtrl->AddButtons(nButtons, pTbButton);
	delete pTbButton;

	if(placement == -1)
	{
		CPoint	point(xpos, ypos);
		m_pParentFrame->FloatControlBar(pNewToolBar, point);

		if(nrows > 1)
			pNewToolBar->UpdateLayout(nrows);
	}
	else
		m_pParentFrame->DockControlBar(pNewToolBar, placement);

	if(show)
		// The toolbar is hidden when created, show it now if specified.
		m_pParentFrame->ShowControlBar(pNewToolBar, TRUE, TRUE);

	pTbList->AddTail(pNewToolBar);
	SetModifiedFlag(TRUE);

	ptrlist->RemoveAll();

out:
	if(hResDLL)
		FreeLibrary(hResDLL);

	return bRet;
}

int
Icad_TBcompare
(
 const void*	arg1,
 const void*	arg2
)
{
	return _stricmp(*(char**)arg1, *(char**)arg2);
}

int
Icad_TBFindcompare
(
 const void*	arg1,
 const void*	arg2
)
{
	return _strnicmp(*((char**)arg1), *(char**)arg2, strlen(*(char**)arg1));
}

BOOL
CIcadToolBarMain::CreateFromMnu
(
 LPCTSTR	lpszFile,
 BOOL		bDisplayMessages,
 BOOL		bAppend
)
{
	CFile			fileMenu;
	CFileException	fileException;
	char			*pBuffer = NULL, *fcp2 = NULL, *pszMenuGroup,
					*helpstrs = NULL, *fcp1 = NULL, **sortlist = NULL;
	int				sortcount = 0;
	BOOL			bRet = TRUE;
	UINT			nRet = 0;
	DWORD			dwFileLen;
	CPtrList		ButtonList, *pToolBarList;
	POSITION		pos = NULL;

	BeginWaitCursor();

	if(!bAppend)
		// Delete the existing toolbars
		SetToolBarList(NULL);

	CString	strMenuGroup(lpszFile),
			strGroupName;

	// Open the .mnu file and find the first toolbar.
	if(!fileMenu.Open(lpszFile, CFile::modeRead, &fileException))
	{
		TRACE(ResourceString(IDC_ICADTOOLBARMAIN_CAN_T_OP_4, "Can't open file %s, error = %u\n"), lpszFile, fileException.m_cause);
		bRet = FALSE;
		goto out;
	}

	fileMenu.SeekToBegin();
	dwFileLen = fileMenu.GetLength();
	pBuffer = new char[dwFileLen + 1];
	if(!pBuffer)
	{
		bRet = FALSE;
		goto out;
	}

	nRet = fileMenu.Read(pBuffer, dwFileLen);

	// Close the file.
	fileMenu.Close();

	if(nRet < dwFileLen)
		pBuffer[nRet] = 0;
	else
		pBuffer[dwFileLen] = 0;

	// Set the value of the MenuGroup
	if(pszMenuGroup = strstr(pBuffer, "***MENUGROUP="/*DNT*/))
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

	// If this menugroup is already loaded, don't load it again
	pToolBarList = GetToolBarList();
	if(!pToolBarList)
	{
		pToolBarList = new CPtrList();
		SetToolBarList(pToolBarList);
	}
	pos = pToolBarList->GetHeadPosition();

	while(pos)
	{
		CIcadToolBar*	pToolBar = (CIcadToolBar*)pToolBarList->GetNext(pos);
		if(!pToolBar)
			continue;
		
		/*D.G.*/// Remember, we adding a submenu name to a group name after the '.'
		// (see in CIcadToolBarMain::ParseMenuInBuffer).
		strGroupName = pToolBar->m_strGroupName;
		int	idxChar = strGroupName.Find('.'/*DNT*/);
		if(idxChar > -1)
			strGroupName = strGroupName.Left(idxChar);

		if(!strGroupName.CompareNoCase(strMenuGroup))
		{
			bRet = FALSE;
			goto out;
		}
	}

	fcp1 = (char*)stristr(pBuffer, "***TOOLBARS"/*DNT*/);

	if(!fcp1)
	{
		if(bDisplayMessages)
			sds_alert(ResourceString(IDC_ICADTOOLBARMAIN_NO_TOOLB_7, "No toolbars found in file to import\n\"***TOOLBARS\" statement is missing."));

		bRet = FALSE;
		goto out;
	}

	fcp1 += 11;

	// Set up the help string area to search.
	helpstrs = (char*)stristr(pBuffer, "***HELPSTRINGS"/*DNT*/);
	sortlist = NULL;
	sortcount = 0;

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
		sortlist = new char *[sortcount];
		memset(sortlist,0,sortcount*sizeof (char *) );

		for(ct2 = 0, cp1 = helpstrs; *cp1; ++cp1)
		{
			if(*cp1 == '\n'/*DNT*/)
			{
				*cp1 = 0;
				sortlist[ct2] = cp1 + 1;
				++ct2;
			}
		}

		qsort((void*)sortlist, sortcount, sizeof(char*), Icad_TBcompare);
	}

	ParseMenuInBuffer(lpszFile, strMenuGroup, fcp1, helpstrs, sortlist, sortcount, NULL, &ButtonList, NULL);

out:
	delete [] pBuffer;
	delete [] sortlist;
	OnVisibilityChanged();
	EndWaitCursor();
	m_pParentFrame->RecalcLayout();

	return bRet;
}

BOOL
CIcadToolBarMain::ParseMenuInBuffer
(
 LPCTSTR		lpszFile,
 LPCTSTR		lpszGroupName,
 char*			pBuffer,
 char*			helpstrs,
 char**			sortlist,
 int			sortcount,
 char*			pSubmenu,
 CPtrList*		pButtonList,
 CIcadTBbutton*	foButtons
)
{
	char			*fcp1 = NULL, *fcp2 = NULL, *tbname = NULL,
					fs1[IC_ACADBUF], title[51], helps[101];
	BOOL			bRet = TRUE, show;
	int				whattype, xpos, ypos, nrows, placement, hadspacer;
	CIcadTBbutton	*picButton, *CurFo;
	CString			strGroupName;

	*title = 0;

	if(!pSubmenu)
		fcp1 = pBuffer;
	else
	{
		// Search fo the submenu.
		fcp1 = strchr(pSubmenu, '.');
		sprintf(fs1, "**%s"/*DNT*/, fcp1 ? fcp1 + 1 : pSubmenu);
		int	len = strlen(fs1);
		fcp1 = stristr(pBuffer, fs1);

		if(!fcp1) 
		{
			bRet = FALSE;
			goto out;
		}

		//Bug fix 49757 - In ArchT, we were finding **FLOORS/CEILING, when we really wanted **FLOORS
		//So check if the found string has a whitespace at the end, if not, keep searching until we find one that does.
		if(!isspace((unsigned char)fcp1[len]))
		{
			do
			{
				fcp2 = stristr(fcp1, fs1);
				fcp1 = fcp2 + 1; //start next search at the found substring + 1
			} while(!isspace((unsigned char)fcp2[len]));
			fcp1 = fcp2;
		}

		if(!fcp1)
		{
			bRet = FALSE;
			goto out;
		}
		strGroupName.Format("%s.%s"/*DNT*/, lpszGroupName, fs1 + 2);
	}

	while(*fcp1)
	{
		for(int ct = 0;
			!(*fcp1 == 0 || *fcp1 == '\n'/*DNT*/ || *fcp1 == '\r'/*DNT*/) && ct < sizeof(fs1) - 1;
			++fcp1, ++ct)
			fs1[ct] = *fcp1;

		if(*fcp1)
			++fcp1;
		fs1[ct] = 0;

		// Blank line.
		if(!*fs1)
			continue;

		// Out of the toolbar area of the menu.
		if(strsame(fs1, "***"/*DNT*/))
			goto out;

		// Check for "**TOOLBAR_NAME"
		if(strnsame(fs1, "**"/*DNT*/, 2))
		{
			// Moving to the next one, build the last.
			if(pSubmenu)
			{
				if(!*title)
					continue;
				else
				{
					bRet = TRUE;
					goto out;
				}
			}

			if(!pButtonList->IsEmpty())
			{
				if(!CreateToolBarFromList(lpszFile, strGroupName, pButtonList, title, xpos, ypos, placement, nrows, show))
					return FALSE;
			}

			strGroupName.Format("%s.%s"/*DNT*/, lpszGroupName, fs1 + 2);
			continue;
		}

		char*	fcp3 = strchr(fs1, '['/*DNT*/);
		if(!fcp3)
			continue;

		*fcp3 = 0;
		++fcp3;

		char	*fcp4 = strchr(fcp3, '('/*DNT*/);
		if(fcp4)
			*fcp4 = 0;

		if(stristr(fcp3, "Toolbar"/*DNT*/))
		{
			whattype = 1;
			hadspacer = 0;
		}
		else if(stristr(fcp3, "Button"/*DNT*/))
			whattype = 2;
		else if(stristr(fcp3, "Flyout"/*DNT*/))
			whattype = 3;
		else if(fcp3[0] == '-'/*DNT*/ && fcp3[1] == '-'/*DNT*/ && fcp3[2] == ']'/*DNT*/)
		{
			hadspacer = 1;	// Spacer
			continue;
		}
		else
			continue;

		if(fcp4)
			*fcp4 = '('/*DNT*/;

		char*	cmdstr = strchr(fcp3, ']'/*DNT*/);
		if(cmdstr)
			++cmdstr;
		else
			continue;

		fcp4 = strchr(fcp3, '('/*DNT*/);
		if(!fcp4)
			continue;
		++fcp4;

		char	cp = *(cmdstr - 1);
		*(cmdstr - 1) = 0;

		char	*fcp5 = strrchr(fcp4, ')'/*DNT*/);

		*(cmdstr - 1) = cp;
		if(!fcp5)
			continue;
		*fcp5 = 0;

		switch(whattype)
		{
			case 1 :	   // Toolbar
				// Defaults
				strcpy(title, ResourceString(IDC_ICADTOOLBARMAIN_DEFAULT_16, "Default toolbar title"));
				placement = -1;	// Floating
				show = TRUE;	// Show/hide
				xpos = ypos = 0;
				nrows = 1;

				// Title
				if(!(fcp5 = strtok(fcp4, ","/*DNT*/)))
					break;

				ic_trim(fcp5, "be"/*DNT*/);
				if(*fcp5 == '\"'/*DNT*/)
					++fcp5;

				strncpy(title, fcp5, sizeof(title) - 1);

				if(title[strlen(title) - 1] == '\"'/*DNT*/)
					title[strlen(title) - 1] = 0;

				// Placement
				if(!(fcp5 = strtok(NULL, ","/*DNT*/)))
					break;
				ic_trim(fcp5, "be"/*DNT*/);
// Support for underscore'd english.
				if(strisame(fcp5, ResourceString(IDC_ICADTOOLBARMAIN_TOP_19, "Top")) || strisame(fcp5, "_Top"/*DNT*/))
					placement = AFX_IDW_DOCKBAR_TOP;
				else if(strisame(fcp5, ResourceString(IDC_ICADTOOLBARMAIN_BOTTOM_20, "Bottom")) || strisame(fcp5, "_Bottom"/*DNT*/))
					placement = AFX_IDW_DOCKBAR_BOTTOM;
				else if(strisame(fcp5, ResourceString(IDC_ICADTOOLBARMAIN_LEFT_21, "Left")) || strisame(fcp5, "_Left"/*DNT*/))
					placement = AFX_IDW_DOCKBAR_LEFT;
				else if(strisame(fcp5, ResourceString(IDC_ICADTOOLBARMAIN_RIGHT_22, "Right")) || strisame(fcp5, "_Right"/*DNT*/))
					placement = AFX_IDW_DOCKBAR_RIGHT;

				// State (Hidden/Show)
				if(!(fcp5 = strtok(NULL, ","/*DNT*/)))
					break;
				ic_trim(fcp5, "be"/*DNT*/);
				if(strisame(fcp5, ResourceString(IDC_ICADTOOLBARMAIN_HIDE_23, "Hide")) || strisame(fcp5, "_Hide"/*DNT*/))
					show = FALSE;

				// X position
				if(!(fcp5 = strtok(NULL, ","/*DNT*/)))
					break;
				ic_trim(fcp5, "be"/*DNT*/);
				xpos = atoi(fcp5);

				// Y position
				if(!(fcp5 = strtok(NULL, ","/*DNT*/)))
					break;
				ic_trim(fcp5, "be"/*DNT*/);
				ypos = atoi(fcp5);

				// Number of row
				if(!(fcp5 = strtok(NULL, ","/*DNT*/)))
					break;
				ic_trim(fcp5, "be"/*DNT*/);
				nrows = atoi(fcp5);

				break;

			case 2 :// Button
			case 3 :// Flyout
				picButton = new CIcadTBbutton;
				picButton->m_BMPIdSmColor = IDB_ICAD_GENERIC;
				picButton->m_BMPIdSmBW = IDB_ICAD_BWS_GENERIC;
				picButton->m_BMPIdLgColor = IDB_ICAD_LG_GENERIC;
				picButton->m_BMPIdLgBW = IDB_ICAD_BWL_GENERIC;
				picButton->m_bChgParentBtnImg = FALSE;
				picButton->m_lflgVisibility = g_DefaultMenuVisibility;
				picButton->m_bAddSpacerBefore = hadspacer ? TRUE : FALSE;
				hadspacer = 0;

				// Help string.
				if(helpstrs)
				{
					ic_trim(fs1, "be"/*DNT*/);
					char**	bsret;
					fcp5 = fs1;

					if( *fs1 &&
						(bsret = (char**)bsearch(&fcp5, sortlist, sortcount, sizeof(char*), Icad_TBFindcompare))
						&& (fcp5 = *bsret) && (fcp5 = strchr(fcp5, '[')))
					{
						strncpy(helps, fcp5 + 1, sizeof(helps) - 1);
						if(fcp5 = strchr(helps, ']'/*DNT*/))
							*fcp5 = 0;
						picButton->m_helpString = helps;
					}
				}

				// Command
				if(cmdstr)
				{
					char*	tmpCmd = new char[strlen(cmdstr) + 1];
					strcpy(tmpCmd, cmdstr);

					while(tmpCmd[strlen(tmpCmd) - 1] == '+'/*DNT*/)
					{
						tmpCmd[strlen(tmpCmd) - 1] = 0;
						CString	cs = tmpCmd;
						if(*fcp1 == '\r'/*DNT*/ || *fcp1 == '\n'/*DNT*/)
							++fcp1;
						while(*fcp1)
						{
							if(*fcp1 == '\r'/*DNT*/ || *fcp1 == '\n'/*DNT*/)
							{
								++fcp1;
								break;
							}
							cs += *fcp1;
							++fcp1;
						}
						delete [] tmpCmd;
						tmpCmd = new char[strlen(cs) + 1];
						strcpy(tmpCmd, cs);
					}
					picButton->m_command = tmpCmd;
					delete [] tmpCmd;
				}

				// Tool tip.
				char*	fcp6;

				for(fcp5 = fcp4, fcp6 = fcp5 + 1; *fcp6; ++fcp6)
				{
					if(*fcp6 == '\"'/*DNT*/ && *(fcp6 - 1) != '\\'/*DNT*/)
					{
						*fcp6 = 0;
						break;
					}
				}

				fcp4 = fcp6 + 1;
				ic_trim(fcp5, "be"/*DNT*/);
				if(*fcp5 == '\"'/*DNT*/)
					++fcp5;

				picButton->m_toolTip = fcp5;

				if(picButton->m_toolTip[picButton->m_toolTip.GetLength() - 1] == '\"'/*DNT*/)
					picButton->m_toolTip.TrimRight('\"'/*DNT*/);

				// Icon 16x16
				fcp5 = strtok(fcp4, ","/*DNT*/);
				if(fcp5)
				{
					ic_trim(fcp5, "be"/*DNT*/);

					// Ignore quotes if they exist.
					if(*fcp5 == '\"'/*DNT*/)
						++fcp5;

					int	nLen = strlen(fcp5);
					if(fcp5[nLen - 1] == '\"'/*DNT*/)
						fcp5[nLen-1] = 0;

					ic_trim(fcp5, "be"/*DNT*/);

					char*	pszBmpName;
					if(pszBmpName = strrchr(fcp5, '\\'/*DNT*/))
						++pszBmpName;
					else
						pszBmpName = fcp5;

					if(!strstr(pszBmpName, ".bmp"/*DNT*/) && strstr(pszBmpName, "ICON_"/*DNT*/))
					{
						char	*fcp6 = strrchr(fcp5, '_'/*DNT*/);
						if(fcp6)
						{
							++fcp6;
							int	ct;
							for(ct = 0; ct < g_nMnuTbItems; ++ct)
							{
								if(!g_MnuTbItems[ct].pAcadTbStr)
									continue;

								if(strisame(g_MnuTbItems[ct].pAcadTbStr, fcp6))
								{
									picButton->m_BMPIdSmColor = g_MnuTbItems[ct].BMPIdSmColor;
									picButton->m_BMPIdLgColor = g_MnuTbItems[ct].BMPIdLgColor;
									picButton->m_BMPIdSmBW = g_MnuTbItems[ct].BMPIdSmBW;
									picButton->m_BMPIdLgBW = g_MnuTbItems[ct].BMPIdLgBW;
									break;
								}
							}
						}
					}
					//Modified Cybage SBD 25/02/2002 DD/MM/YYYY [
					//Reason : Fix for Bug No. 77936 from Bugzilla 
					//picButton->m_smBwBmpPath = picButton->m_smColorBmpPath = fcp5;
					else if(stristr(pszBmpName, ".bmp"/*DNT*/)) 
						picButton->m_smBwBmpPath = picButton->m_smColorBmpPath = fcp5;
					else if(atoi(pszBmpName))
					{
						picButton->m_BMPIdSmColor = atoi(pszBmpName);
						picButton->m_BMPIdSmBW = atoi(pszBmpName);
					}
					//Modified Cybage SBD 25/02/2002 DD/MM/YYYY ]

					// Added By ShokuY@Cosmo 2002-8-10
					//Reason: Regression bug fix for Bug No. 77936 from Bugzilla
					else
					{
						picButton->m_smBwBmpPath = picButton->m_smColorBmpPath = fcp5;
					}
					// Added By Cosmo ShokuY 2002-8-10
				}

				// Icon 32x32
				fcp5 = strtok(NULL, ","/*DNT*/);
				if(fcp5)
				{
					ic_trim(fcp5, "be"/*DNT*/);
					// Ignore quotes if they exist.
					if(*fcp5 == '\"'/*DNT*/)
						++fcp5;

					int	nLen = strlen(fcp5);
					if(fcp5[nLen - 1] == '\"'/*DNT*/)
						fcp5[nLen - 1] = 0;

					ic_trim(fcp5, "be"/*DNT*/);

					char*	pszBmpName;
					if(pszBmpName = strrchr(fcp5, '\\'/*DNT*/))
						++pszBmpName;
					else
						pszBmpName = fcp5;

					if(!strstr(pszBmpName, ".bmp"/*DNT*/) && strstr(pszBmpName, "ICON_"/*DNT*/))
					{
						char	*fcp6 = strrchr(fcp5, '_'/*DNT*/);
						if(fcp6)
						{
							++fcp6;
							int	ct;
							for(ct = 0; ct < g_nMnuTbItems; ++ct)
							{
								if(!g_MnuTbItems[ct].pAcadTbStr)
									continue;

								if(strisame(g_MnuTbItems[ct].pAcadTbStr, fcp6))
								{
									picButton->m_BMPIdLgColor = g_MnuTbItems[ct].BMPIdLgColor;
									picButton->m_BMPIdLgBW = g_MnuTbItems[ct].BMPIdLgBW;
									break;
								}
							}
						}
					}
					//Modified Cybage SBD 25/02/2002 DD/MM/YYYY [
					//Reason : Fix for Bug No. 77936 from Bugzilla 
					//picButton->m_lgBwBmpPath = picButton->m_lgColorBmpPath = fcp5;
					else if(stristr(pszBmpName, ".bmp"/*DNT*/))
						picButton->m_lgBwBmpPath = picButton->m_lgColorBmpPath = fcp5;
					else if(atoi(pszBmpName))
					{
						picButton->m_BMPIdLgColor = atoi(pszBmpName);
						picButton->m_BMPIdLgBW = atoi(pszBmpName);
					}
					//Modified Cybage SBD 25/02/2002 DD/MM/YYYY ]

					// Added By ShokuY@Cosmo 2002-8-10
					//Reason: Regression bug fix for Bug No. 77936 from Bugzilla
					else
					{
						picButton->m_lgBwBmpPath = picButton->m_lgColorBmpPath = fcp5;
					}
					// Added By Cosmo ShokuY 2002-8-10
				}

				if(whattype == 3)
				{
					// Icon Swap (OwnIcon/OtherIcon)
					fcp5 = strtok(NULL, ","/*DNT*/);
					if(fcp5)
					{
						ic_trim(fcp5, "be"/*DNT*/);
						if( strisame(fcp5, ResourceString(IDC_ICADTOOLBARMAIN_OTHERIC_26, "OtherIcon")) ||
							strisame(fcp5, "_OtherIcon"/*DNT*/) || pSubmenu)	/*D.Gavrilov*/// "_OtherIcon" too
							picButton->m_bChgParentBtnImg = TRUE;
					}

					// Flyout name
					fcp5 = strtok(NULL, ","/*DNT*/);
					if(fcp5)
					{
						ic_trim(fcp5, "be"/*DNT*/);
						if(*fcp5 == '_'/*DNT*/)
							++fcp5;

						// Call this function recursivly to do flyouts.
						ParseMenuInBuffer(lpszFile, lpszGroupName, pBuffer, helpstrs, sortlist, sortcount, fcp5, pButtonList, picButton);

						// Set the current flyout to the first one in the list.
						picButton->m_pibCurFlyOut = picButton->m_pibFlyOut;
						if (pSubmenu)
							picButton->m_pibFlyOut=NULL;
					}
				}

				if(pSubmenu && foButtons)
				{
					if(!foButtons->m_pibFlyOut)
						foButtons->m_pibFlyOut = CurFo = picButton;
					else
						CurFo = CurFo->m_pibFlyOut = picButton;
				}
				else
					pButtonList->AddTail(picButton);

				break;
		}	// switch(whattype)

	}	// while(*fcp1)

out:
	// Build the last toolbar here.
	if(!pSubmenu && !pButtonList->IsEmpty())
		CreateToolBarFromList(lpszFile, strGroupName, pButtonList, title, xpos, ypos, placement, nrows, show);

	return bRet;
}


// CIcadToolBarMain Button manipulation functions

int
CIcadToolBarMain::GetVisibleButtonCount
(
 CToolBarCtrl*	pToolBarCtrl
)
{
	int			ct, nVisibleButtons = 0,
				nButtons = pToolBarCtrl->GetButtonCount();
	TBBUTTON	tbButton;

	for(ct = 0; ct < nButtons; ++ct)
	{
		pToolBarCtrl->GetButton(ct, &tbButton);
		if(!(tbButton.fsState & TBSTATE_HIDDEN))
			++nVisibleButtons;
	}

	return nVisibleButtons;
}

int
CIcadToolBarMain::GetFirstVisibleButtonIndex
(
 CToolBarCtrl*	pToolBarCtrl
)
{
	int			ct,
				nButtons = pToolBarCtrl->GetButtonCount();
	TBBUTTON	tbButton;

	for(ct = 0; ct < nButtons; ++ct)
	{
		pToolBarCtrl->GetButton(ct, &tbButton);
		if(!(tbButton.fsState & TBSTATE_HIDDEN))
			break;
	}

	return ct;
}

int
CIcadToolBarMain::GetLastVisibleButtonIndex
(
 CToolBarCtrl*	pToolBarCtrl
)
{
	int			ct, idxLastVisibleButton = 0,
				nButtons = pToolBarCtrl->GetButtonCount();
	TBBUTTON	tbButton;

	for(ct = 0; ct < nButtons; ++ct)
	{
		pToolBarCtrl->GetButton(ct, &tbButton);
		if(!(tbButton.fsState & TBSTATE_HIDDEN))
			idxLastVisibleButton = ct;
	}

	return idxLastVisibleButton;
}

int
CIcadToolBarMain::GetPrevVisibleButtonIndex
(
 CToolBarCtrl*	pToolBarCtrl,
 int			idxButton
)
{
	int			ct;
	TBBUTTON	tbButton;

	for(ct = idxButton - 1; ct > 0; --ct)
	{
		pToolBarCtrl->GetButton(ct, &tbButton);
		if(!(tbButton.fsState & TBSTATE_HIDDEN))
			break;
	}

	return ct;
}

int
CIcadToolBarMain::GetNextVisibleButtonIndex
(
 CToolBarCtrl*	pToolBarCtrl,
 int			idxButton
)
{
	int			ct,
				nButtons = pToolBarCtrl->GetButtonCount();
	TBBUTTON	tbButton;

	for(ct = idxButton + 1; ct < nButtons; ++ct)
	{
		pToolBarCtrl->GetButton(ct, &tbButton);
		if(!(tbButton.fsState & TBSTATE_HIDDEN))
			break;
	}

	return ct;
}

int
CIcadToolBarMain::AddIcadBitmap
(
 CIcadTBbutton*	pibButton,
 BOOL			bFlyOut,
 CIcadToolBar*	pToolBar,
 CBitmap**		ppBmp,
 LPCTSTR		pszAltBmpPath,
 HINSTANCE		hResDLL
)
{
	// Get the bitmap ID.
	int		nRet = 0, nBitmapID;
	char*	pstrBitmap = NULL;

	if(m_bLargeButtons && m_bColorButtons)
	{
		nBitmapID = pibButton->m_BMPIdLgColor;
		if(!pibButton->m_lgColorBmpPath.IsEmpty())
			pstrBitmap = pibButton->m_lgColorBmpPath.GetBuffer(0);
	}
	else if(m_bLargeButtons && !m_bColorButtons)
	{
		nBitmapID = pibButton->m_BMPIdLgBW;
		if(!pibButton->m_lgBwBmpPath.IsEmpty())
			pstrBitmap = pibButton->m_lgBwBmpPath.GetBuffer(0);
	}
	else if(!m_bLargeButtons && m_bColorButtons)
	{
		nBitmapID = pibButton->m_BMPIdSmColor;
		if(!pibButton->m_smColorBmpPath.IsEmpty())
			pstrBitmap = pibButton->m_smColorBmpPath.GetBuffer(0);
	}
	else
	{
		nBitmapID = pibButton->m_BMPIdSmBW;
		if(!pibButton->m_smBwBmpPath.IsEmpty())
			pstrBitmap = pibButton->m_smBwBmpPath.GetBuffer(0);
	}

	char		szBitmapPath[IC_ACADBUF];
	CBitmap*	pbmBitmap = NULL;
	BOOL		bLoadedFromDLL = FALSE, bLoadFromFile = FALSE;

	*szBitmapPath = 0;
	if(pstrBitmap)
	{
		char	szExt[10];
		ic_getext(pstrBitmap, szExt);

		int		nWidth, nHeight;
		if(m_bLargeButtons)
		{
			nWidth = ICAD_LG_IMGX;
			nHeight = ICAD_LG_IMGY;
		}
		else
		{
			nWidth = ICAD_SM_IMGX;
			nHeight = ICAD_SM_IMGY;
		}

		if(!szExt[1] && hResDLL)
		{
			// If the string does not include an extention, attempt loading from the resource dll.
			HBITMAP	hBmp = (HBITMAP)LoadImage(hResDLL, pstrBitmap, IMAGE_BITMAP, nWidth, nHeight, LR_LOADMAP3DCOLORS);

			if(!hBmp)
			{
				HRSRC imgBMP=FindResource(hResDLL,pstrBitmap,RT_BITMAP);
				if (imgBMP)
					hBmp = LoadBitmapFromResource(hResDLL,imgBMP,nWidth,nHeight);
				else
					hBmp = (HBITMAP)LoadBitmap(hResDLL, pstrBitmap);
				if(!hBmp)
					ASSERT(0);
			}

			if(hBmp)
			{
				pbmBitmap = new CBitmap();
				pbmBitmap->Attach(hBmp);
				bLoadedFromDLL = TRUE;
			}
		}

		if(!bLoadedFromDLL)
		{
			// Add the bmp extention and try to find the file.
			strncpy(szBitmapPath, pstrBitmap, sizeof(szBitmapPath) - 1);
			ic_setext(szBitmapPath, "bmp"/*DNT*/);
		}

		// If a filename is passed, make sure it is valid.
		if(!bLoadedFromDLL && pszAltBmpPath)
		{
			// First try the path the mnu was loaded from.
			CString	str(pszAltBmpPath);
			int		idxChar = str.ReverseFind('\\'/*DNT*/);

			if(idxChar > 0)
				str = str.Left(idxChar);

			str += "\\"/*DNT*/;
			str += szBitmapPath;
			if(sds_findfile(str.GetBuffer(0), szBitmapPath) == RTNORM)
				bLoadFromFile = TRUE;
		}

		// Second, try the search path.
		if(!bLoadedFromDLL && !bLoadFromFile)
		{
			if(sds_findfile(szBitmapPath, szBitmapPath) == RTNORM)
				bLoadFromFile = TRUE;
		}

		// If everything failed, return an error.
		if(!bLoadFromFile && !bLoadedFromDLL && m_bCustomize)
		{
			CString	str;
			LOADSTRINGS_2(IDC_ICADTOOLBARMAIN_COULD_N_29)
			str.Format(LOADEDSTRING, pstrBitmap);
			if(m_pParentFrame->MessageBox(str, ResourceString(IDC_ICADTOOLBARMAIN_CUSTOM__36, "Custom Toolbars"), MB_ICONWARNING | MB_OKCANCEL) == IDCANCEL)
				return -1;
		}

		if(bLoadFromFile)
		{
			// Setup the compatible device context
			HBITMAP	hBmp = (HBITMAP)LoadImage(NULL, szBitmapPath, IMAGE_BITMAP, nWidth, nHeight, LR_LOADFROMFILE | LR_LOADMAP3DCOLORS);

			if(hBmp)
			{
				pbmBitmap = new CBitmap();
				pbmBitmap->Attach(hBmp);
			}
			else
			{
				CString	str;
				str.Format(ResourceString(IDC_ICADTOOLBARMAIN_ERROR_R_37, "Error reading file \"%s\"."), szBitmapPath);
				sds_alert(str);
			}
		}
	}

	// Check to see if the bitmap needs the flyout indicator.
	if(!bFlyOut)
	{
		if(!pbmBitmap)
		{
			pbmBitmap = new CBitmap();
			pbmBitmap->LoadMappedBitmap(nBitmapID);
		}
	}
	else
	{
		// Create off-screen device context.
		CDC*	pMemDC = new CDC();
		if(!pMemDC->CreateCompatibleDC(NULL))
		{
			delete pMemDC;

			if(pbmBitmap)
			{
				delete pbmBitmap;
				pbmBitmap = NULL;
			}

			nRet = -__LINE__;
			goto badout;
		}

		// Select the button bitmap into the device context.
		if(!pbmBitmap)
		{
			pbmBitmap = new CBitmap();
			pbmBitmap->LoadMappedBitmap(nBitmapID);
		}

		CBitmap*	pbmOld = pMemDC->SelectObject(pbmBitmap);
		// Draw the flyout indicator.
		POINT		pt[3];

		if(m_bLargeButtons)
		{
			pt[0].x = ICAD_LG_IMGX - 1;  pt[0].y = ICAD_LG_IMGY - 5;
			pt[1].x = ICAD_LG_IMGX - 1;  pt[1].y = ICAD_LG_IMGY - 1;
			pt[2].x = ICAD_LG_IMGX - 5;  pt[2].y = ICAD_LG_IMGY - 1;
		}
		else
		{
			pt[0].x = ICAD_SM_IMGX - 1;  pt[0].y = ICAD_SM_IMGY - 3;
			pt[1].x = ICAD_SM_IMGX - 1;  pt[1].y = ICAD_SM_IMGY - 1;
			pt[2].x = ICAD_SM_IMGX - 3;  pt[2].y = ICAD_SM_IMGY - 1;
		}

		COLORREF	color;
		if(m_bColorButtons)
			// Set the pen color of color buttons to the button text system color.
			color = GetSysColor(COLOR_BTNTEXT);
		else
			// Set the pen color to black for black/white toolbars.
			color = RGB(0, 0, 0);

		CPen*	pPen = new CPen(PS_SOLID, 0, color);
		CPen*	pOldPen = pMemDC->SelectObject(pPen);
		CBrush*	pBrush = new CBrush(color);
		CBrush*	pOldBrush = pMemDC->SelectObject(pBrush);

		pMemDC->Polygon(pt, 3);

		// Clean up
		pMemDC->SelectObject(pbmOld);
		pMemDC->SelectObject(pOldBrush);
		delete pBrush;
		pMemDC->SelectObject(pOldPen);
		delete pPen;
		delete pMemDC;
	}

	if(nBitmapID < 0)
	{
		nRet = -__LINE__;
		goto badout;
	}

	if(!ppBmp)
	{
		if(!pToolBar)
			// Add the bitmap to the bitmap list.
			nBitmapID = AddToBmpList(pbmBitmap);
	}
	else
	{
		// Return the bitmap to the calling function.
		*ppBmp = pbmBitmap;
		nBitmapID = 0;
	}

	if(pToolBar)
	{
		// Add the image to the toolbar, then delete the bitmap
		AddToolBarBitmap(pToolBar, NULL, pbmBitmap, &nBitmapID);
		if(pbmBitmap)
		{
			delete pbmBitmap;
			pbmBitmap = NULL;
		}
	}

	return nBitmapID;

badout:
	CString	str;
	str.Format(ResourceString(IDC_ICADTOOLBARMAIN_ERROR_W_39, "Error while reading file \"%s\".\nSystem resources have been exhausted, cannot continue.  The\nsystem may become unstable.  Please shut down or reset to the\ndefault toolbars to safely continue. (%d)"), szBitmapPath, nRet);
	sds_alert(str);
	return -1;
}

BOOL
CIcadToolBarMain::AddToolBarBitmap
(
 CIcadToolBar*	pToolBar,
 CIcadFlyOut*	pFlyOut,
 CBitmap*		pBmp,
 int*			nBmpID
)
{
	HBITMAP	hBitmap = NULL;

	if(!pBmp)
	{
		// Create the mother bitmap from the bitmap list in ToolBarMain.
		pBmp = CreateToolBarBitmap();
		if(!pBmp)
			return FALSE;

		hBitmap = (HBITMAP)pBmp->Detach();
		delete pBmp;
		pBmp = NULL;
	}
	else
	{
		HBITMAP	hbmImageWell = NULL;

		if(pToolBar)
			hbmImageWell = pToolBar->GetImageWell();
		else
			if(pFlyOut)
				hbmImageWell = pFlyOut->m_hbmImageWell;

		// Add the image to the mother bitmap.
		CBitmap*	pbmOrig = NULL;

		if(hbmImageWell)
		{
			pbmOrig = new CBitmap();
			pbmOrig->Attach(hbmImageWell);
		}

		int		nWidth = 0, nHeight = 0;
		BITMAP	bitmapOrig, bitmap;
		memset(&bitmapOrig, 0, sizeof(BITMAP));
		memset(&bitmap, 0, sizeof(BITMAP));

		if(pbmOrig)
			pbmOrig->GetBitmap(&bitmapOrig);

		pBmp->GetBitmap(&bitmap);
		nWidth = bitmapOrig.bmWidth + bitmap.bmWidth;

		if(bitmapOrig.bmHeight > 0)
			nHeight = bitmapOrig.bmHeight;
		else
			nHeight = bitmap.bmHeight;

		CDC		*pMemDestDC = new CDC(),
				*pMemSrcDC = new CDC(),
				*pDC = m_pParentFrame->GetDC();

		if(pMemDestDC->CreateCompatibleDC(pDC) && pMemSrcDC->CreateCompatibleDC(pDC))
		{
			CBitmap*	pbmNew = new CBitmap();
			pbmNew->CreateCompatibleBitmap(pDC, nWidth, nHeight);
			CBitmap		*pbmOldDest = pMemDestDC->SelectObject(pbmNew),
						*pbmOldSrc = NULL;

			if(pbmOrig)
			{
				// Copy in the previous selected bitmap.
				pbmOldSrc = pMemSrcDC->SelectObject(pbmOrig);
				pMemDestDC->BitBlt(0, 0, bitmapOrig.bmWidth, nHeight, pMemSrcDC, 0, 0, SRCCOPY);
				pMemSrcDC->SelectObject(pbmOldSrc);
			}

			// Copy in the new bitmap.
			pbmOldSrc = pMemSrcDC->SelectObject(pBmp);
			pMemDestDC->BitBlt(bitmapOrig.bmWidth, 0, bitmap.bmWidth, nHeight, pMemSrcDC, 0, 0, SRCCOPY);
			pMemDestDC->SelectObject(pbmOldDest);
			pMemSrcDC->SelectObject(pbmOldSrc);
			hBitmap = (HBITMAP)pbmNew->Detach();
			delete pbmNew;

			// Set the bitmap ID number
			if(nBmpID)
				*nBmpID = nWidth / bitmap.bmWidth - 1;
		}

		delete pMemSrcDC;
		delete pMemDestDC;
		m_pParentFrame->ReleaseDC(pDC);

		if(pbmOrig)
		{
			pbmOrig->Detach();
			delete pbmOrig;
		}
	}

	if(!hBitmap)
		return FALSE;

	BOOL	bRet = FALSE;

	if(pToolBar)
		bRet = pToolBar->SetBitmap(hBitmap);
	else
		if(pFlyOut)
			bRet = pFlyOut->SetBitmap(hBitmap);

	return bRet;
}

void
CIcadToolBarMain::SetButtonVisibility
(
 long	lflgVisibility,
 BOOL	bShow
)
{
	/*D.G.*/// accessibility licensed components commands checking added
	//TO DO: maybe some optimization enable for fly-outs

	CIcadToolBar*	pToolBar;
	int				nButtons = 0, idxButton;
	BOOL			bGray = FALSE;

	m_lflgVisibility = lflgVisibility;

	if(lflgVisibility & ICAD_MN_CLS)
	{
		// Turn the closed mdi bit off and set a flag to gray.
		lflgVisibility &= ~ICAD_MN_CLS;
		bGray = TRUE;
	}

	CPtrList*	pToolBarList = GetToolBarList();
	POSITION	pos = pToolBarList->GetHeadPosition();
	if(!pos)
		return;

	pToolBar = (CIcadToolBar*)pToolBarList->GetAt(pos);

	while(pos)
	{
		pToolBar = (CIcadToolBar*)pToolBarList->GetNext(pos);
		if(!pToolBar)
			continue;

		CToolBarCtrl&	TbCtrl = pToolBar->GetToolBarCtrl();

		// Loop through the buttons and if there is a separator get the next visible
		// button and set its m_bAddSpacerBefore variable, then delete the separator.
		int				ct, idxNextVisBtn;
		TBBUTTON		tbButton;
		CIcadTBbutton*	picButton;

		idxButton = GetFirstVisibleButtonIndex(&TbCtrl);
		nButtons = GetLastVisibleButtonIndex(&TbCtrl);

		for(ct = 0; ct <= nButtons && nButtons >= idxButton; ++ct)
		{
			TbCtrl.GetButton(idxButton, &tbButton);
			if(tbButton.fsStyle & TBSTYLE_SEP)
			{
        // TECNOBIT(Todeschini) 2003.13.05 -[
        // Separators with dwData pointing to a CIcadTBbutton class with ICAD_MN_CTRL flag in
        // m_lflgVisibility member variable, is a button with controls (e.g. combobox), so it must not be removed
        if(!tbButton.dwData || ((CIcadTBbutton*)tbButton.dwData)->m_lflgVisibility & ICAD_MN_CTRL == 0) 
        {
        // ]- TECNOBIT
				  idxNextVisBtn = GetNextVisibleButtonIndex(&TbCtrl, idxButton);
				  TbCtrl.GetButton(idxNextVisBtn, &tbButton);
				  picButton = (CIcadTBbutton*)tbButton.dwData;
				  ASSERT(picButton);	// Should never happen
				  picButton->m_bAddSpacerBefore = TRUE;
				  TbCtrl.DeleteButton(idxButton);
				  idxButton = idxNextVisBtn;
				  continue;
        }
			}
			idxButton = GetNextVisibleButtonIndex(&TbCtrl, idxButton);
		}

		// Change the button visibility and add any necessary spacers.
		nButtons = TbCtrl.GetButtonCount();

		for(idxButton = 0; idxButton < nButtons; ++idxButton)
		{
			TbCtrl.GetButton(idxButton, &tbButton);
			picButton = (CIcadTBbutton*)tbButton.dwData;
			if(!picButton)
				continue;

			if(picButton->m_pibCurFlyOut)
			{
				// The button IS a flyout.
				// Make sure that more than one button will be visible.  If not,
				// change the bitmap to reflect the button will not fly-out.
				int				nVisFlyButtons = 0;
				CIcadTBbutton*	pibCur;

				for(pibCur = picButton->m_pibFlyOut; pibCur; pibCur = pibCur->m_pibFlyOut)
				{
					if( (lflgVisibility & pibCur->m_lflgVisibility) == lflgVisibility &&
						CIcadApp::m_LicensedComponents.IsCommandAccessible(pibCur->m_command) )
						++nVisFlyButtons;
				}

				if(nVisFlyButtons <= 0)
				{
					TbCtrl.HideButton(tbButton.idCommand, TRUE);
					continue;
				}

				if(nVisFlyButtons == 1)
				{
					// Set the current flyout button pointer to the visible button.
					for(pibCur = picButton->m_pibFlyOut; pibCur; pibCur = pibCur->m_pibFlyOut)
					{
						if( (lflgVisibility & pibCur->m_lflgVisibility) == lflgVisibility &&
							CIcadApp::m_LicensedComponents.IsCommandAccessible(pibCur->m_command) )
							break;
					}

					picButton->m_pibCurFlyOut = pibCur;

					// Set the bitmap to a non-flyout bitmap.
					picButton->m_idxFlyOutImage = AddIcadBitmap(pibCur, 0, pToolBar);
					tbButton.iBitmap = picButton->m_idxFlyOutImage;
				}
				else if( (lflgVisibility & picButton->m_pibCurFlyOut->m_lflgVisibility) != lflgVisibility ||
						 !CIcadApp::m_LicensedComponents.IsCommandAccessible(picButton->m_pibCurFlyOut->m_command) )
				{
					// If the pointer to the current button is no longer visible,
					// set the pointer to the first visible button in the list.
					for(pibCur = picButton->m_pibFlyOut; pibCur; pibCur = pibCur->m_pibFlyOut)
					{
						if( (lflgVisibility & pibCur->m_lflgVisibility) == lflgVisibility &&
							CIcadApp::m_LicensedComponents.IsCommandAccessible(pibCur->m_command) )
							break;
					}

					picButton->m_pibCurFlyOut = pibCur;
					tbButton.iBitmap = picButton->m_pibCurFlyOut->m_idxFlyOutImage;
				}
				else
					tbButton.iBitmap = picButton->m_pibCurFlyOut->m_idxFlyOutImage;

				tbButton.fsState = TBSTATE_ENABLED;
				TbCtrl.DeleteButton(idxButton);
				TbCtrl.InsertButton(idxButton, &tbButton);

				// Does the button get grayed out when no MDI window is active?
				if(bGray && !(picButton->m_pibCurFlyOut->m_lflgVisibility & ICAD_MN_CLS))
					TbCtrl.EnableButton(tbButton.idCommand, FALSE);
				else
					TbCtrl.EnableButton(tbButton.idCommand);
			}
			else
			{
				// The button is NOT a flyout.
				if( (lflgVisibility & picButton->m_lflgVisibility) != lflgVisibility ||
					!CIcadApp::m_LicensedComponents.IsCommandAccessible(picButton->m_command) )
				{
					TbCtrl.HideButton(tbButton.idCommand, TRUE);
					continue;
				}

				// Make the button visible.
				TbCtrl.HideButton(tbButton.idCommand, FALSE);
				// Does the button get grayed out when no MDI window is active?
				if(bGray && !(picButton->m_lflgVisibility & ICAD_MN_CLS))
					TbCtrl.EnableButton(tbButton.idCommand, FALSE);
				else
					TbCtrl.EnableButton(tbButton.idCommand);
			}

			// If the m_bAddSpacerBefore member is TRUE add a separator before the button.
			if(picButton->m_bAddSpacerBefore && idxButton>GetFirstVisibleButtonIndex(&TbCtrl))
			{
				tbButton.iBitmap = ICAD_SEP_WIDTH;
				tbButton.idCommand = 0;
				tbButton.fsState = TBSTATE_ENABLED;
				tbButton.fsStyle = TBSTYLE_SEP;
				tbButton.dwData = 0;
				tbButton.iString = 0;
				TbCtrl.InsertButton(idxButton, &tbButton);
				++idxButton;	// Increment the counter because we just added a button.
				++nButtons;
			}
		}
		//Zhifeng.Cheng Add:Make them gray If no drawing
		if(SDS_CMainWindow->m_wndLayers.m_hWnd)
			SDS_CMainWindow->m_wndLayers.EnableWindow(!bGray);
		if(SDS_CMainWindow->m_wndColor.m_hWnd)
			SDS_CMainWindow->m_wndColor.EnableWindow(!bGray);
		if(SDS_CMainWindow->m_wndLType.m_hWnd)
			SDS_CMainWindow->m_wndLType.EnableWindow(!bGray);
		if(SDS_CMainWindow->m_wndLWeight.m_hWnd)
			SDS_CMainWindow->m_wndLWeight.EnableWindow(!bGray);
		if(SDS_CMainWindow->m_wndDimstyle.m_hWnd)
			SDS_CMainWindow->m_wndDimstyle.EnableWindow(!bGray);
		//Add End

		if(!GetVisibleButtonCount(&TbCtrl))
		{
			if(bShow || pToolBar->IsWindowVisible())	/*D.G.*/// use || instead of &&
				m_pParentFrame->ShowControlBar(pToolBar, FALSE, FALSE);

			pToolBar->m_bEmpty = TRUE;
		}
		else
			pToolBar->m_bEmpty = FALSE;

		// Update the size
		pToolBar->UpdateLayout();
	}

	m_pParentFrame->RecalcLayout();
}

// CIcadToolBarMain Functions called from CMainWindow::IcadWndAction
// (Reactor functions "If you will")

BOOL
CIcadToolBarMain::CopyButtonInfo
(
 CIcadTBbutton**	ppibDest,
 CIcadTBbutton*		pibSource
)
{
	if(!ppibDest || !pibSource)
		return FALSE;

	*ppibDest = NULL;

	CIcadTBbutton	*pibNewBeg = NULL, *pibNewCur = NULL, *pibCur = NULL, *pibTmp;

	for(pibCur = pibSource; pibCur; pibCur = pibCur->m_pibFlyOut)
	{
		if(!(pibTmp = new CIcadTBbutton))
			return FALSE;

		if(!pibNewBeg)
			pibNewBeg = pibNewCur = pibTmp;
		else
			pibNewCur = pibNewCur->m_pibFlyOut = pibTmp;

		// Copy the elements of the structure, including the strings.
		pibNewCur->m_idToolBar = pibCur->m_idToolBar;

		if(!pibCur->m_toolTip.IsEmpty())
			pibNewCur->m_toolTip = pibCur->m_toolTip;

		if(!pibCur->m_helpString.IsEmpty())
			pibNewCur->m_helpString = pibCur->m_helpString;

		if(!pibCur->m_command.IsEmpty())
			pibNewCur->m_command = pibCur->m_command;

		if(!pibCur->m_pushVar.IsEmpty())
			pibNewCur->m_pushVar = pibCur->m_pushVar;

		if(!pibCur->m_grayVar.IsEmpty())
			pibNewCur->m_grayVar = pibCur->m_grayVar;

		pibNewCur->m_BMPIdSmColor = pibCur->m_BMPIdSmColor;
		pibNewCur->m_BMPIdLgColor = pibCur->m_BMPIdLgColor;
		pibNewCur->m_BMPIdSmBW = pibCur->m_BMPIdSmBW;
		pibNewCur->m_BMPIdLgBW = pibCur->m_BMPIdLgBW;
		pibNewCur->m_lflgVisibility = pibCur->m_lflgVisibility;
		pibNewCur->m_idxFlyOutImage = pibCur->m_idxFlyOutImage;
		pibNewCur->m_bAddSpacerBefore = pibCur->m_bAddSpacerBefore;
		pibNewCur->m_bChgParentBtnImg = pibCur->m_bChgParentBtnImg;

		if(!pibCur->m_smColorBmpPath.IsEmpty())
			pibNewCur->m_smColorBmpPath = pibCur->m_smColorBmpPath;

		if(!pibCur->m_lgColorBmpPath.IsEmpty())
			pibNewCur->m_lgColorBmpPath = pibCur->m_lgColorBmpPath;

		if(!pibCur->m_smBwBmpPath.IsEmpty())
			pibNewCur->m_smBwBmpPath = pibCur->m_smBwBmpPath;

		if(!pibCur->m_lgBwBmpPath.IsEmpty())
			pibNewCur->m_lgBwBmpPath = pibCur->m_lgBwBmpPath;

	}	// for(

	// Set the pointer to the correct current flyout.
	if(pibSource->m_pibCurFlyOut)
	{
		for(pibCur = pibSource, pibNewCur = pibNewBeg;
			pibCur && pibNewCur;
			pibCur = pibCur->m_pibFlyOut, pibNewCur = pibNewCur->m_pibFlyOut)
		{
			if(pibSource->m_pibCurFlyOut == pibCur)
			{
				pibNewBeg->m_pibCurFlyOut = pibNewCur;
				break;
			}
		}

	}

	*ppibDest = pibNewBeg;

	return TRUE;
}

void
CIcadToolBarMain::UpdateBitmaps()
{
	BeginWaitCursor();

	CPtrList*	pToolBarList = GetToolBarList();
	HINSTANCE	hResDLL = NULL;
	char*		pszAltPath = NULL;
	POSITION	pos = pToolBarList->GetHeadPosition();

	while(pos)
	{
		// Load the new bitmaps in the order the buttons are expecting, that way
		// we don't need to recreate the buttons.
		CIcadToolBar*	pToolBar = (CIcadToolBar*)pToolBarList->GetNext(pos);

		if(!pToolBar)
			continue;

		CToolBarCtrl&	TbCtrl = pToolBar->GetToolBarCtrl();
		int				nRows = pToolBar->GetButtonRows();

		if(!pToolBar->m_strMnuFileName.IsEmpty())
		{
			pszAltPath = pToolBar->m_strMnuFileName.GetBuffer(0);

			// Look for a resource dll and load it.
			char*	pszResDLL = new char[pToolBar->m_strMnuFileName.GetLength() + 5];	// +5 just in case an extension isn't specified.
			strcpy(pszResDLL, pToolBar->m_strMnuFileName);
			ic_setext(pszResDLL, "dll"/*DNT*/);
			hResDLL = LoadLibraryEx(pszResDLL,NULL,LOAD_LIBRARY_AS_DATAFILE);
			delete [] pszResDLL;
		}

		// Set the sizes for the buttons for this toolbar.
		if(m_bLargeButtons)
		{
			TbCtrl.SetBitmapSize(CSize(ICAD_LG_IMGX, ICAD_LG_IMGY));
			TbCtrl.SetButtonSize(CSize(ICAD_LG_BUTTONX, ICAD_LG_BUTTONY));
		}
		else
		{
			TbCtrl.SetBitmapSize(CSize(ICAD_SM_IMGX, ICAD_SM_IMGY));
			TbCtrl.SetButtonSize(CSize(ICAD_SM_BUTTONX, ICAD_SM_BUTTONY));
		}

		// Loop through the buttons, add the updated image to the bitmap list,
		// and change the bitmap index in the button.
		int				nButtons = TbCtrl.GetButtonCount(),
						ct;
		CIcadTBbutton	*picButton, *pibTmp;
		TBBUTTON		tbButton;

		for(ct = 0; ct < nButtons; ++ct)
		{
			TbCtrl.GetButton(ct, &tbButton);
			if(tbButton.fsStyle & TBSTYLE_SEP)
				// Ignore spacers.
				continue;

			if(!(picButton = (CIcadTBbutton*)tbButton.dwData))
				continue;

			if(picButton->m_pibFlyOut)
			{
				// Don't forget the flyouts.
				for(pibTmp = picButton->m_pibFlyOut; pibTmp; pibTmp = pibTmp->m_pibFlyOut)
					pibTmp->m_idxFlyOutImage = AddIcadBitmap(pibTmp, 1, NULL, NULL, pszAltPath, hResDLL);

				if (picButton->m_bChgParentBtnImg)
					pToolBar->ChangeBitmap(tbButton.idCommand, picButton->m_pibCurFlyOut->m_idxFlyOutImage);
				else
					pToolBar->ChangeBitmap(tbButton.idCommand, AddIcadBitmap(picButton, 0, NULL, NULL, pszAltPath, hResDLL));
			}
			else
				pToolBar->ChangeBitmap(tbButton.idCommand, AddIcadBitmap(picButton, 0, NULL, NULL, pszAltPath, hResDLL));
		}

		// Add the new toolbar bitmap
		AddToolBarBitmap(pToolBar, NULL);

		// Update the appearance of the toolbar.
		pToolBar->RedrawWindow();

		// Maintain the number of rows for a floating toolbar.
		pToolBar->UpdateLayout(nRows);

		// Free the resource dll.
		if(hResDLL)
		{
			FreeLibrary(hResDLL);
			hResDLL = NULL;
		}

		pszAltPath = NULL;
	}

	// Update the appearance of all the toolbars.
	m_pParentFrame->RecalcLayout();
	EndWaitCursor();
}

void
CIcadToolBarMain::OnToolTipsChanged()
{
	CPtrList*	pToolBarList = GetToolBarList();
	POSITION	pos = pToolBarList->GetHeadPosition();

	while(pos)
	{
		CIcadToolBar*	pToolBar = (CIcadToolBar*)pToolBarList->GetNext(pos);
		if(!pToolBar)
			continue;

		CToolBarCtrl&	TbCtrl = pToolBar->GetToolBarCtrl();

		// Update the tooltips style
		if(m_bToolTips==TRUE)
			pToolBar->SetBarStyle(pToolBar->GetBarStyle() | CBRS_TOOLTIPS);
		else
			pToolBar->SetBarStyle(pToolBar->GetBarStyle() & (~CBRS_TOOLTIPS));
	}
}

void
CIcadToolBarMain::OnButtonStateChanged()
{
	int				ctButton, nButtons, BitChk, NotChk, charIdx;
	CToolBarCtrl*	pToolBarCtrl;
	TBBUTTON		tbButton;
	CIcadTBbutton*	picButton;
	CString			varName;
	resbuf			rb;
	CPtrList*		pToolBarList = GetToolBarList();
	POSITION		pos = pToolBarList->GetHeadPosition();

	while(pos)
	{
		CIcadToolBar*	pToolBar = (CIcadToolBar*)pToolBarList->GetNext(pos);
		if(!pToolBar)
			continue;

		pToolBarCtrl = &(pToolBar->GetToolBarCtrl());

		// Loop through the buttons for each toolbar and check for changed
		// button states.
		nButtons = pToolBarCtrl->GetButtonCount();

		for(ctButton = 0; ctButton < nButtons; ++ctButton)
		{
			pToolBarCtrl->GetButton(ctButton, &tbButton);
			if(!(picButton = (CIcadTBbutton*)tbButton.dwData))
				continue;

			if(picButton->m_pibCurFlyOut && picButton->m_bChgParentBtnImg)
				picButton = picButton->m_pibCurFlyOut;

			if(!picButton->m_pushVar.IsEmpty())
			{
				varName = picButton->m_pushVar;
				NotChk = varName[0] == '!'/*DNT*/ ? 1 : 0;
				if(NotChk)
					varName.TrimLeft('!'/*DNT*/);

				if((charIdx = varName.Find('&'/*DNT*/)) >= 0)
				{
					BitChk = atoi(varName.Right(varName.GetLength() - charIdx - 1));
					varName = varName.Left(charIdx);
				}
				else
					BitChk = 0;

				if(sds_getvar(varName, &rb) == RTNORM)
				{
					if(BitChk)
					{
						if(NotChk ? rb.resval.rint & BitChk : !(rb.resval.rint & BitChk))
							pToolBarCtrl->PressButton(tbButton.idCommand, FALSE);
						else
							pToolBarCtrl->PressButton(tbButton.idCommand);
					}
					else
					{
						if(NotChk ? rb.resval.rint : !rb.resval.rint)
							pToolBarCtrl->PressButton(tbButton.idCommand, FALSE);
						else
							pToolBarCtrl->PressButton(tbButton.idCommand);
					}
				}
			}	// m_pushVar

			if(!picButton->m_grayVar.IsEmpty())
			{
				varName = picButton->m_grayVar;
				NotChk = varName[0] == '!'/*DNT*/ ? 1 : 0;
				if(NotChk)
					varName.TrimLeft('!'/*DNT*/);

				if((charIdx = varName.Find('&'/*DNT*/)) >= 0)
				{
					BitChk = atoi(varName.Right(varName.GetLength() - charIdx - 1));
					varName = varName.Left(charIdx);
				}
				else
					BitChk = 0;

				if(sds_getvar(varName, &rb) == RTNORM)
				{
					if(BitChk)
					{
						if(NotChk ? rb.resval.rint & BitChk : !(rb.resval.rint & BitChk))
						{
							// Gray out the button and unpress it if it is pressed.
							pToolBarCtrl->EnableButton(tbButton.idCommand, FALSE);
							pToolBarCtrl->PressButton(tbButton.idCommand, FALSE);
						}
						else
							pToolBarCtrl->EnableButton(tbButton.idCommand);
					}
					else
					{
						if(NotChk ? rb.resval.rint : !rb.resval.rint)
							pToolBarCtrl->EnableButton(tbButton.idCommand);
						else
						{
							// Gray out the button and unpress it if it is pressed.
							pToolBarCtrl->EnableButton(tbButton.idCommand, FALSE);
							pToolBarCtrl->PressButton(tbButton.idCommand, FALSE);
						}
					}
				}
			}	// m_grayVar
		}	// for(
	}	// while(
}

void
CIcadToolBarMain::OnVisibilityChanged()
{
	resbuf	rb;
	long	lflgVisibility;

sds_getvar("WNDLMDI"/*DNT*/, &rb);
	if(!rb.resval.rint)
		lflgVisibility = ICAD_MN_CLS;
	else
		lflgVisibility = ICAD_MN_OPN;

	sds_getvar("EXPLEVEL"/*DNT*/, &rb);

	switch(rb.resval.rint)
	{
		case 1: lflgVisibility |= ICAD_MN_BEG; break;
		case 2: lflgVisibility |= ICAD_MN_INT; break;
		case 3: lflgVisibility |= ICAD_MN_EXP;
	}

	SetButtonVisibility(lflgVisibility, TRUE);
}

// CIcadToolBarMain Functions called to save toolbar information to the
// registry.

void
CIcadToolBarMain::SaveToRegistry
(
 BOOL	bInPlace
)
{
	PreferenceKey	toolbarKey( HKEY_CURRENT_USER, "ToolBars"/*DNT*/, FALSE);
	HKEY			hKeyTbRoot = toolbarKey;

	CString prefixProfileName = ResourceString(IDS_REGISTRY_PROFILES, "Profiles");
	CString profName;
	GetCurrentRegistryProfileName(profName.GetBuffer(IC_ACADBUF), IC_ACADBUF);
	profName.ReleaseBuffer();
	prefixProfileName += _T("\\") + profName;

	if(bInPlace)
	{
		PreferenceKey inPlaceBarsKey( HKEY_CURRENT_USER, _T("InPlaceBars"));
		inPlaceBarsKey.FlushKey(HKEY_CURRENT_USER, _T("InPlaceBars"));

		// SaveBarState() uses this name:  m_pszRegistryKey + m_pszProfileName (= "Companyname\\IntelliCAD" and "")
		m_pParentFrame->SaveBarState(prefixProfileName + _T("\\InPlaceBars\\ControlBar"/*DNT*/));
		return;
	}

	// Return if the registry exists but the Modified flag has not been set;
	if(hKeyTbRoot)
	{
		if(!IsModified())
		{
			// Save the positioning of the toolbars and bail.
			PreferenceKey ControlBarsKey(HKEY_CURRENT_USER, _T("ControlBars"));
			ControlBarsKey.FlushKey(HKEY_CURRENT_USER, _T("ControlBars"));
			m_pParentFrame->SaveBarState(prefixProfileName + _T("\\ControlBars\\ControlBar")/*DNT*/);
			return;
		}
	}

	// Get the registry ready to write to.
	toolbarKey.FlushKey(HKEY_CURRENT_USER, "ToolBars"/*DNT*/);

	PreferenceKey	TbKey(HKEY_CURRENT_USER, "ToolBars"/*DNT*/);
	DWORD			dwRetVal;

	hKeyTbRoot = TbKey;

	if(!hKeyTbRoot)
		return;

	// Write the number of toolbars
	CPtrList*	pToolBarList = GetToolBarList();
	int			nToolBars = pToolBarList->GetCount();

	RegSetValueEx(hKeyTbRoot, "nBars"/*DNT*/, 0, REG_DWORD, (CONST BYTE*)&nToolBars, sizeof(int));

	// Loop through the toolbars and write the toolbar and button information to the registry.
	POSITION	pos = pToolBarList->GetHeadPosition();
	HKEY		hKeyTb, hKeyFlyOut;
	CString		strKey, strButtonKey, strFlyOutKey, strText;
	int			ct, idxButton, nButtons;

	for(ct = 0; pos; ++ct)
	{
		CIcadToolBar*	pToolBar = (CIcadToolBar*)pToolBarList->GetNext(pos);

		if(!pToolBar)
			continue;

		// Create a key for the toolbar
		strKey.Format("ToolBar-%d"/*DNT*/, ct);

		CToolBarCtrl*	pTbCtrl = &(pToolBar->GetToolBarCtrl());

		if(RegCreateKeyEx(hKeyTbRoot, strKey, 0, ""/*DNT*/, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKeyTb, &dwRetVal) != ERROR_SUCCESS)
			continue;

		// Write the name of the toolbar.
		pToolBar->GetWindowText(strText);
		RegSetValueEx(hKeyTb, "Name"/*DNT*/, 0, REG_SZ, (CONST BYTE*)strText.GetBuffer(0), strText.GetLength());

		// Write the name of the mnu file the toolbar was created from.
		if(!pToolBar->m_strMnuFileName.IsEmpty())
			RegSetValueEx(hKeyTb, "FileName"/*DNT*/, 0, REG_SZ, (CONST BYTE*)pToolBar->m_strMnuFileName.GetBuffer(0), pToolBar->m_strMnuFileName.GetLength());

		// Write the unload name of the toolbar.
		if(!pToolBar->m_strGroupName.IsEmpty())
			RegSetValueEx(hKeyTb, "UnloadName"/*DNT*/, 0, REG_SZ, (CONST BYTE*)pToolBar->m_strGroupName.GetBuffer(0), pToolBar->m_strGroupName.GetLength());

		// Write the ID that the toolbar was created with.
		UINT	nToolBarID = pToolBar->m_nID - IDR_ICAD_TOOLBAR0;
		RegSetValueEx(hKeyTb, "ToolBarID"/*DNT*/, 0, REG_DWORD, (CONST BYTE*)&nToolBarID, sizeof(UINT));

		// Write the default toolbar struct ID.
		//RegSetValueEx(hKeyTb, "DefID"/*DNT*/, 0, REG_DWORD, (CONST BYTE*)&pToolBar->m_lDefID, sizeof(long));
		RegSetValueEx(hKeyTb, "DefID"/*DNT*/, 0, REG_QWORD, (CONST BYTE*)&pToolBar->m_lDefID, sizeof(long_double));

		// Write the IcadButton information.
		TBBUTTON		tbButton;
		CIcadTBbutton*	picButton;

		// Loop through the buttons and if there is a separator get the next visible
		// button and set its m_bAddSpacerBefore variable, then delete the separator.
		int	ctSpacer, idxNextVisBtn;

		idxButton = GetFirstVisibleButtonIndex(pTbCtrl);
		nButtons = GetLastVisibleButtonIndex(pTbCtrl);

		for(ctSpacer = 0; ctSpacer <= nButtons && nButtons >= idxButton; ++ctSpacer)
		{
			pTbCtrl->GetButton(idxButton, &tbButton);
			if(tbButton.fsStyle & TBSTYLE_SEP)
			{
				idxNextVisBtn = GetNextVisibleButtonIndex(pTbCtrl, idxButton);
				pTbCtrl->GetButton(idxNextVisBtn, &tbButton);
				picButton = (CIcadTBbutton*)tbButton.dwData;
				ASSERT(picButton);	// Should never happen
				picButton->m_bAddSpacerBefore = TRUE;
				pTbCtrl->DeleteButton(idxButton);
				--nButtons;
				idxButton = idxNextVisBtn - 1;
			}
			else
			{
				picButton = (CIcadTBbutton*)tbButton.dwData;
				if(picButton)
					picButton->m_bAddSpacerBefore = FALSE;
			}

			idxButton = GetNextVisibleButtonIndex(pTbCtrl, idxButton);
		}

		// This variable is for figuring out the total number of buttons including hidden spacers.
		nButtons = pTbCtrl->GetButtonCount();
		int	nTotalButtons = nButtons;

		for(idxButton = 0; idxButton < nButtons; ++idxButton)
		{
			pTbCtrl->GetButton(idxButton, &tbButton);
			if(!(picButton = (CIcadTBbutton*)tbButton.dwData))
			{
				--nTotalButtons;	// Decrement the counter here because it will get
									// incremented if the m_bAddSpacerBefore is TRUE.
				continue;
			}

			if(picButton->m_bAddSpacerBefore)
				++nTotalButtons;

			picButton->saveToReg(hKeyTb, idxButton);

			if(picButton->m_pibFlyOut)
			{
				// Write FlyOut information
				// Create a key for the flyout
				strFlyOutKey.Format("Button#%dFlyOut"/*DNT*/, idxButton);

				if(RegCreateKeyEx(hKeyTb, strFlyOutKey, 0, ""/*DNT*/, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKeyFlyOut, &dwRetVal) != ERROR_SUCCESS)
					continue;

				int				nFlyButtons = 0, idxParentCur = 0;
				CIcadTBbutton*	pibCur;

				for(pibCur = picButton->m_pibFlyOut; pibCur; pibCur = pibCur->m_pibFlyOut)
				{
					if(pibCur == picButton->m_pibCurFlyOut)
						idxParentCur = nFlyButtons;

					// Write the IcadButton information for this flyout button.
					pibCur->saveToReg(hKeyFlyOut, nFlyButtons);
					++nFlyButtons;

					//if this button has a CurFlyOut, then it is a nested toolbar - save it to the registry.
					if(pibCur->m_pibCurFlyOut)
					{
						CString	strNestedFlyOutKey;
						HKEY	hKeyNestedFlyOut;

						//create a sub key for the nested flyout
						strNestedFlyOutKey.Format("Button#%dNestedFlyOut"/*DNT*/, nFlyButtons);

						if(RegCreateKeyEx(hKeyFlyOut, strNestedFlyOutKey, 0, ""/*DNT*/, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKeyNestedFlyOut, &dwRetVal) != ERROR_SUCCESS)
							continue;

						CIcadTBbutton*	pibNest;
						pibNest = pibCur->m_pibCurFlyOut;

						if(pibNest->m_pibFlyOut)
						{
							// Write FlyOut information
							// Create a key for the flyout
							int				nNestedFlyButtons = 0;
							CIcadTBbutton*	pibCur;

							for(pibCur = pibNest; pibCur; pibCur = pibCur->m_pibFlyOut)
							{
								// Write the IcadButton information for this flyout button.
								pibCur->saveToReg(hKeyNestedFlyOut, nNestedFlyButtons);
								++nNestedFlyButtons;
							}

							// Write the number of buttons for the nested flyout 
							RegSetValueEx(hKeyNestedFlyOut, "nButtons"/*DNT*/, 0, REG_DWORD, (CONST BYTE*)&nNestedFlyButtons, sizeof(int));
							RegCloseKey(hKeyNestedFlyOut);
						}
					}
				}	// for(

				// Write the number of buttons for the flyout and the index of the
				// flyout button that the parent button is currently pointing at.
				RegSetValueEx(hKeyFlyOut, "nButtons"/*DNT*/, 0, REG_DWORD, (CONST BYTE*)&nFlyButtons, sizeof(int));
				RegSetValueEx(hKeyFlyOut, "idxParentCur"/*DNT*/, 0, REG_DWORD, (CONST BYTE*)&idxParentCur, sizeof(int));
				RegCloseKey(hKeyFlyOut);
			}
		}	// for(

		// Write the total number of buttons including hidden spacers.
		RegSetValueEx(hKeyTb, "nButtons"/*DNT*/, 0, REG_DWORD, (CONST BYTE*)&nTotalButtons, sizeof(int));
		RegCloseKey(hKeyTb);
	}	// for(

	// Save the positioning of the toolbars.
	PreferenceKey ControlBarsKey( HKEY_CURRENT_USER, _T("ControlBars"));
	ControlBarsKey.FlushKey(HKEY_CURRENT_USER, _T("ControlBars"));
	m_pParentFrame->SaveBarState( prefixProfileName + _T("\\ControlBars\\ControlBar"/*DNT*/));
	SetModifiedFlag(FALSE);
}


// CIcadToolBarMain Indirect message handler functions.  These functions get
// called from the owner frames message handler functions.

BOOL
CIcadToolBarMain::OnToolTipText
(
 UINT		nID,
 NMHDR*		pNMHDR,
 LRESULT*	pResult
)
{

	TOOLTIPTEXT*	pTTT = (TOOLTIPTEXT*)pNMHDR;
	CIcadTBbutton*	pibCurToolTip = GetCurToolTipPtr();

	if(pibCurToolTip && !pibCurToolTip->m_toolTip.IsEmpty())
		lstrcpyn(pTTT->szText, pibCurToolTip->m_toolTip, _countof(pTTT->szText));

	return TRUE;
}

BOOL
CIcadToolBarMain::OnToolTipTextW
(
 UINT		nID,
 NMHDR*		pNMHDR,
 LRESULT*	pResult
)
{
	// This function is for Windows NT UNICODE tooltips.
	TOOLTIPTEXTW*	pTTT = (TOOLTIPTEXTW*)pNMHDR;
	CIcadTBbutton*	pibCurToolTip = GetCurToolTipPtr();

	if(pibCurToolTip && !pibCurToolTip->m_toolTip.IsEmpty())
		_mbstowcsz(pTTT->szText, pibCurToolTip->m_toolTip, _countof(pTTT->szText));

	return TRUE;
}


// CIcadToolBarMain customization functions
//

int
CIcadToolBarMain::AddDraggedButton
(
 CIcadToolBarCtrl*	pFromTbCtrl,
 CIcadToolBar*		pToolBar,
 CIcadFlyOut*		pFlyOut,
 CPoint				ptScrn
)
{
// This function returns the index of the new button that was added to either a toolbar or a flyout.

	CToolBarCtrl*	pToTbCtrl = NULL;

	if(pToolBar)
		pToTbCtrl = &(pToolBar->GetToolBarCtrl());
	else
		if(pFlyOut)
			pToTbCtrl = (CToolBarCtrl*)pFlyOut;

	// Un-press the drag button
	pFromTbCtrl->PressButton(pFromTbCtrl->m_CurDragButton.idCommand, FALSE);

	// Use the CIcadToolBarCtrl::m_CurDragButton structure to create the new button.
	int	idxItems = pFromTbCtrl->m_CurDragButton.idCommand - ICAD_CUSTOOL_BTNID_START;

	// ~| TODO - This system for creating button IDs will work great untill the
	// end of the range is reached.  Then button IDs could possibly get duplicated.
	pFromTbCtrl->m_CurDragButton.idCommand = GetNewTbID();

	// Fill in the CIcadTBbutton structure to be associated with the new button.
	CIcadTBbutton*	picButton = new CIcadTBbutton;
	picButton->m_idToolBar = 0;	// TODO ~|
	picButton->m_toolTip = g_MnuTbItems[idxItems].ToolTip;
	picButton->m_helpString = g_MnuTbItems[idxItems].HelpString;
	picButton->m_command = g_MnuTbItems[idxItems].Command;
	picButton->m_BMPIdSmColor = g_MnuTbItems[idxItems].BMPIdSmColor;
	picButton->m_BMPIdLgColor = g_MnuTbItems[idxItems].BMPIdLgColor;
	picButton->m_BMPIdSmBW = g_MnuTbItems[idxItems].BMPIdSmBW;
	picButton->m_BMPIdLgBW = g_MnuTbItems[idxItems].BMPIdLgBW;

	// Make a flyout button added with customize always visible.
	picButton->m_lflgVisibility = ICAD_MN_ALL;
	picButton->m_idxFlyOutImage = -1;
	picButton->m_bAddSpacerBefore = FALSE;
	picButton->m_bChgParentBtnImg = TRUE;
	pFromTbCtrl->m_CurDragButton.dwData = (DWORD)picButton;

	// Add the bitmap to the toolbar with the pressed state set.
	if(pToolBar)
		pFromTbCtrl->m_CurDragButton.iBitmap = AddIcadBitmap(picButton, 0, pToolBar);
	else
		if(pFlyOut)
		{
			CBitmap*	pBmp = NULL;
			AddIcadBitmap(picButton, 0, NULL, &pBmp);
			if(pBmp)
			{
				int	nBmpID = 0;
				AddToolBarBitmap(NULL, pFlyOut, pBmp, &nBmpID);
				pFromTbCtrl->m_CurDragButton.iBitmap = nBmpID;
				delete pBmp;
			}
		}

	pFromTbCtrl->m_CurDragButton.fsState = TBSTATE_ENABLED | TBSTATE_PRESSED;

	// Get the index of the existing button that the new button was dropped on.
	CPoint	ptToolBar(ptScrn);
	pToTbCtrl->ScreenToClient(&ptToolBar);
	BOOL	bVert = FALSE;
	int		nInsertAt = 1;
	int		idxButton = QueryInsertAt(pToolBar, pFlyOut, ptToolBar, &bVert, &nInsertAt);

	if(idxButton < 0)
		// If QueryInsertAt failed the toolbar is probably new.
		idxButton = 0;

	int		nButtons = pToTbCtrl->GetButtonCount(),
			nRows;

	if(!pToolBar)
		nRows = pToTbCtrl->GetRows();
	else
		nRows = pToolBar->GetButtonRows();

	if(GetVisibleButtonCount(pToTbCtrl) == nRows && bVert)
		// Force vertical layout when requested.
		++nRows;

	if(nInsertAt == ICAD_INSERTON)
	{
		// The button was dropped on a spacer, make sure the next visible
		// button's spacer flag is turned off.
		int	idxNextVisibleButton = GetNextVisibleButtonIndex(pToTbCtrl, idxButton);

		if(idxNextVisibleButton != -1)
		{
			TBBUTTON	tbButton;
			pToTbCtrl->GetButton(idxNextVisibleButton, &tbButton);
			if(tbButton.dwData)
				((CIcadTBbutton*)tbButton.dwData)->m_bAddSpacerBefore = FALSE;
		}

		// Delete the spacer
		pToTbCtrl->DeleteButton(idxButton);
	}
	else if(nInsertAt == ICAD_INSERTAFTER)
	{
		++idxButton;
	}
	else if(nInsertAt == ICAD_INSERTBEFORE)
	{
		// Make sure the spacer flags are updated.
		TBBUTTON	tbButton;
		pToTbCtrl->GetButton(idxButton, &tbButton);
		if(tbButton.dwData)
			((CIcadTBbutton*)tbButton.dwData)->m_bAddSpacerBefore = FALSE;
	}

	// Insert the button.
	if(idxButton >= nButtons)
	{
		// If the button was dropped onto the end use AddButtons to insert the button.
		pToTbCtrl->AddButtons(1, &pFromTbCtrl->m_CurDragButton);
		idxButton = nButtons;
	}
	else
		pToTbCtrl->InsertButton(idxButton, &pFromTbCtrl->m_CurDragButton);

	pFromTbCtrl->m_CurDragButton.dwData = 0;

	// Redraw the toolbar.
	if(pToolBar)
		pToolBar->UpdateLayout(nRows);

	return idxButton;
}

int
CIcadToolBarMain::MoveDraggedButton
(
 CIcadToolBar*	pToolBar,
 CIcadFlyOut*	pFlyOut,
 CPoint			point,
 int			idxDragButton
)
{
// This function returns the index of the button after it has been moved or -1 if a button is deleted.

	CToolBarCtrl*	pTbCtrl = NULL;

	if(pToolBar)
		pTbCtrl = &(pToolBar->GetToolBarCtrl());
	else
		if(pFlyOut)
			pTbCtrl = (CToolBarCtrl*)pFlyOut;

	if(!pTbCtrl)
		return -1;

	CRect	rect;
	pTbCtrl->GetClientRect(rect);

	// If the button was dragged off the toolbar delete it.
	if(!rect.PtInRect(point))
	{
		// Before deleting the button check for spacers.
		DeleteExcessSpacers(pTbCtrl, &idxDragButton, NULL);
		TBBUTTON		tbButton;
		pTbCtrl->GetButton(idxDragButton, &tbButton);
		CIcadTBbutton*	picButton = (CIcadTBbutton*)tbButton.dwData;

		if(picButton->m_pibFlyOut)
			// Set the m_pibFlyOut member to NULL so that only this button is deleted.
			picButton->m_pibFlyOut = NULL;

		FreeIcadButtonData(picButton);
		pTbCtrl->DeleteButton(idxDragButton);
		return -1;
	}

	// Get the index of the button that the dragging button was dropped on.
	BOOL	bVert = FALSE;
	int		nInsertAt;
	int		idxButton = QueryInsertAt(pToolBar, pFlyOut, point, &bVert, &nInsertAt);

	if(idxButton < 0)
		return -1;

	int		nButtons = pTbCtrl->GetButtonCount(),
			nRows;

	if(!pToolBar)
		nRows = pTbCtrl->GetRows();
	else
		nRows = pToolBar->GetButtonRows();

	if(idxButton == idxDragButton)
		return idxButton;

	TBBUTTON	tbDragButton;
	pTbCtrl->GetButton(idxDragButton, &tbDragButton);
	((CIcadTBbutton*)tbDragButton.dwData)->m_bAddSpacerBefore = FALSE;

	// Figure out where the button was dropped.
	if(nInsertAt == ICAD_INSERTON)
	{
		// The button was dropped on a spacer, delete the spacer.
		if(idxDragButton < idxButton)
		{
			int	idxNextVisibleButton = GetNextVisibleButtonIndex(pTbCtrl, idxButton);
			if(idxNextVisibleButton != -1)
			{
				TBBUTTON	tbButton;
				pTbCtrl->GetButton(idxNextVisibleButton, &tbButton);
				if(tbButton.dwData)
					((CIcadTBbutton*)tbButton.dwData)->m_bAddSpacerBefore = FALSE;
			}
		}

		pTbCtrl->DeleteButton(idxButton);
		if(idxDragButton > idxButton)
			--idxDragButton;
	}
	else if(nInsertAt == ICAD_INSERTAFTER)
	{
		++idxButton;
	}
	else if(nInsertAt == ICAD_INSERTBEFORE)
	{
		// Make sure the spacer flags are updated.
		TBBUTTON	tbButton;
		pTbCtrl->GetButton(idxButton, &tbButton);
		if(tbButton.dwData)
			((CIcadTBbutton*)tbButton.dwData)->m_bAddSpacerBefore = FALSE;
	}

	// Set the values for the button info
	TBBUTTON	tbButton;
	tbButton.iBitmap = tbDragButton.iBitmap;
	tbButton.idCommand = tbDragButton.idCommand;
	tbButton.fsState = TBSTATE_ENABLED | TBSTATE_PRESSED;
	tbButton.fsStyle = TBSTYLE_BUTTON;
	tbButton.dwData = tbDragButton.dwData;
	tbButton.iString = 0;

	// Before deleting the button check for spacers.
	DeleteExcessSpacers(pTbCtrl, &idxDragButton, &idxButton);

	// Delete the drag button from the old location.
	pTbCtrl->DeleteButton(idxDragButton);

	if(idxButton > idxDragButton)
		--idxButton;

	// Insert the drag button into the new location.
	if(idxButton >= nButtons)
	{
		// If the button was dropped onto the end use AddButtons to insert the button.
		pTbCtrl->AddButtons(1, &tbButton);
		idxButton = nButtons;
	}
	else
		pTbCtrl->InsertButton(idxButton, &tbButton);

	// Redraw the toolbar
	if(pToolBar)
		pToolBar->UpdateLayout(nRows);

	return idxButton;
}


void
CIcadToolBarMain::DeleteExcessSpacers
(
 CToolBarCtrl*	pToolBarCtrl,
 int*			idxDragButton,
 int*			idxInsertButton
)
{
// This function checks for spacers before and after idxDragButton and deletes them
// if they are unnecessary for customization.  Pointers to the index variables of
// the drag and insert buttons are passed because this function decrements them if
// a spacer in front of them gets deleted.

	TBBUTTON	tbButtonSpacerCheck;
	int			idxPrevVisibleButton = GetPrevVisibleButtonIndex(pToolBarCtrl, *idxDragButton);

	memset(&tbButtonSpacerCheck, 0, sizeof(TBBUTTON));

	pToolBarCtrl->GetButton(idxPrevVisibleButton, &tbButtonSpacerCheck);

	// Is there a spacer before the drag button?
	if(tbButtonSpacerCheck.fsStyle & TBSTYLE_SEP)
	{
		// If there is a spacer before the button and the drag button is the
		// last visible button, delete the spacer.
		if(*idxDragButton == GetLastVisibleButtonIndex(pToolBarCtrl))
		{
			pToolBarCtrl->DeleteButton(idxPrevVisibleButton);
			--(*idxDragButton);
		}
		else
		{
			// If there is a spacer before and after the drag button, delete the spacer
			// after the button.
			int	idxNextVisibleButton = GetNextVisibleButtonIndex(pToolBarCtrl, *idxDragButton);
			memset(&tbButtonSpacerCheck, 0, sizeof(TBBUTTON));
			pToolBarCtrl->GetButton(idxNextVisibleButton, &tbButtonSpacerCheck);
			if(tbButtonSpacerCheck.fsStyle & TBSTYLE_SEP)
			{
				pToolBarCtrl->DeleteButton(idxNextVisibleButton);
				if(idxInsertButton && *idxInsertButton > *idxDragButton)
					--(*idxInsertButton);
			}
		}
	}
	else
	{
		// Is there only a spacer AFTER the button?
		int	idxNextVisibleButton = GetNextVisibleButtonIndex(pToolBarCtrl, *idxDragButton);
		memset(&tbButtonSpacerCheck, 0, sizeof(TBBUTTON));
		pToolBarCtrl->GetButton(idxNextVisibleButton, &tbButtonSpacerCheck);
		if(tbButtonSpacerCheck.fsStyle & TBSTYLE_SEP)
		{
			// If there is a spacer after the button and the drag button is the
			// first visible button, delete the spacer.
			if(*idxDragButton == GetFirstVisibleButtonIndex(pToolBarCtrl))
			{
				pToolBarCtrl->DeleteButton(idxNextVisibleButton);
				if(idxInsertButton)
					--(*idxInsertButton);
			}
		}
	}
}

int
CIcadToolBarMain::GetButtonFromPt
(
 CToolBarCtrl*	pToolBarCtrl,
 CPoint			point,
 LPTBBUTTON		lpButton
)
{
	// This function gets passed a pointer to a CToolBarCtrl, a point, and a pointer
	// to the TBBUTTON structure to be filled by the call to GetButton; and returns
	// the index of the button found at point, or -1 if there was an error.

	CRect		rect;
	int			ct;
	TBBUTTON	tbButton;

	if(!pToolBarCtrl)
		return -1;

	int	nButtons = pToolBarCtrl->GetButtonCount();

	for(ct = 0; ct < nButtons; ++ct)
	{
		pToolBarCtrl->GetButton(ct, &tbButton);

		// If the button is not visible because of user level, continue.
		if(tbButton.fsState & TBSTATE_HIDDEN)
			continue;
		if(!pToolBarCtrl->GetItemRect(ct, rect))
			return -1;
		if(rect.PtInRect(point))
			break;
	}

	if(ct == nButtons)
		return -1;

	if(lpButton)
	{
		if(!pToolBarCtrl->GetButton(ct, lpButton))
			return -1;
	}

	return ct;
}

int
CIcadToolBarMain::GetClosestButtonFromPt
(
 CToolBarCtrl*	pToolBarCtrl,
 CPoint			point
)
{
// This function is very similar to GetButtonFromPt() except this function always
// returns a button index.  The index that is returned is the index of the button
// who's center point is closest to "point".

	CPoint		ptToolBar(point);
	CRect		rect;
	int			ct, idxButton = 0,
				nButtons = pToolBarCtrl->GetButtonCount();
	CPoint		ptCenter;
	CSize		szDist;
	TBBUTTON	tbButton;
	double		dTmp,
				dSmallestDist = 100000.0;

	for(ct = 0; ct < nButtons; ++ct)
	{
		pToolBarCtrl->GetButton(ct, &tbButton);

		// If the button is not visible because of user level, continue.
		if(tbButton.fsState & TBSTATE_HIDDEN)
			continue;

		// Figure out the distance between the drop point and the center of the button.
		pToolBarCtrl->GetItemRect(ct, rect);
		ptCenter = rect.CenterPoint();
		szDist = ptCenter - ptToolBar;
		dTmp = sqrt((double)(szDist.cx * szDist.cx + szDist.cy * szDist.cy));

		// The button with a center point closest to the drop point wins.
		if(dTmp < dSmallestDist)
		{
			dSmallestDist = dTmp;
			idxButton = ct;
		}
	}

	return idxButton;
}

void
CIcadToolBarMain::OnCustButtonDown
(
 CIcadToolBar*		pToolBar,
 CIcadFlyOut*		pFlyOut,
 CIcadToolBarCtrl*	pToolBarCtrl,
 int				idxButton
)
{
	TBBUTTON		tbButton;
	CToolBarCtrl*	pTbCtrlCurBtnDown = NULL;

	if(m_pToolBarCurBtnDown)
		pTbCtrlCurBtnDown = &(m_pToolBarCurBtnDown->GetToolBarCtrl());
	else
		if(m_pFlyOutCurBtnDown)
			pTbCtrlCurBtnDown = (CToolBarCtrl*)m_pFlyOutCurBtnDown;
		else
			if(m_pToolBarCtrlCurBtnDown)
				pTbCtrlCurBtnDown = m_pToolBarCtrlCurBtnDown;

	if(pTbCtrlCurBtnDown)
	{
		pTbCtrlCurBtnDown->GetButton(m_idxCurButtonDown, &tbButton);
		pTbCtrlCurBtnDown->PressButton(tbButton.idCommand, FALSE);

		// Save any changes made in the customize editboxes.
		if(tbButton.dwData)
		{
			CIcadTBbutton*	picButton = (CIcadTBbutton*)tbButton.dwData;

			if(picButton->m_pibCurFlyOut)
				picButton = picButton->m_pibCurFlyOut;

			if(m_pCusTbDlg)
			{
				CEdit	*pEditToolTip = (CEdit*)m_pCusTbDlg->GetDlgItem(CUS_TOOLBAR_EDITTOOLTIP),
						*pEditHelpStr = (CEdit*)m_pCusTbDlg->GetDlgItem(CUS_TOOLBAR_EDITHELPSTR),
						*pEditCommand = (CEdit*)m_pCusTbDlg->GetDlgItem(CUS_TOOLBAR_EDITCOMMAND);

				pEditToolTip->GetWindowText(picButton->m_toolTip);
				pEditHelpStr->GetWindowText(picButton->m_helpString);
				pEditCommand->GetWindowText(picButton->m_command);
			}
		}

		if(m_pFlyOutCurBtnDown && m_pFlyOutCurBtnDown != pFlyOut)
		{
			m_pFlyOutCurBtnDown->m_pPopup->DestroyWindow();
			((CIcadToolBar*)m_pFlyOutCurBtnDown->m_pTbOwner)->m_pFlyOut = NULL;
			delete m_pFlyOutCurBtnDown;
			m_pFlyOutCurBtnDown = NULL;
		}
	}

	// Set the index and the pointer to the current pressed button.
	m_idxCurButtonDown = idxButton;
	m_pToolBarCurBtnDown = pToolBar;
	m_pFlyOutCurBtnDown = pFlyOut;
	m_pToolBarCtrlCurBtnDown = pToolBarCtrl;
}

void
CIcadToolBarMain::DisplayCustData
(
 CIcadTBbutton*	picButton
)
{
	if(!m_pCusTbDlg)
		return;

	CEdit	*pEditToolTip = (CEdit*)m_pCusTbDlg->GetDlgItem(CUS_TOOLBAR_EDITTOOLTIP),
			*pEditHelpStr = (CEdit*)m_pCusTbDlg->GetDlgItem(CUS_TOOLBAR_EDITHELPSTR),
			*pEditCommand = (CEdit*)m_pCusTbDlg->GetDlgItem(CUS_TOOLBAR_EDITCOMMAND);

	// Enable the editboxes
	if(!pEditToolTip->IsWindowEnabled())
	{
		pEditToolTip->EnableWindow();
		pEditHelpStr->EnableWindow();
		pEditCommand->EnableWindow();
	}

	if(picButton->m_pibCurFlyOut)
		picButton = picButton->m_pibCurFlyOut;

	// Set the text in the editboxes.
	pEditToolTip->SetWindowText(picButton->m_toolTip);
	pEditHelpStr->SetWindowText(picButton->m_helpString);
	pEditCommand->SetWindowText(picButton->m_command);

	// Enable the Advanced... button.
	CButton*	pAdvButton = (CButton*)m_pCusTbDlg->GetDlgItem(CUS_TOOLBAR_BTNADV);
	pAdvButton->EnableWindow();
}

void
CIcadToolBarMain::FreeIcadButtonData
(
 CIcadTBbutton*	picButton
)
{
// This function gets called recursively to free flyout button information.

	if(!picButton)
		return;

	CIcadTBbutton	*picCur, *picNext;

	if(picButton->m_pibFlyOut)
	{
		for(picCur = picButton->m_pibFlyOut; picCur; picCur = picNext)
		{
			picNext = picCur->m_pibFlyOut;
			picCur->m_pibFlyOut = NULL;
			if(picCur->m_pibCurFlyOut)
				FreeIcadButtonData(picCur->m_pibCurFlyOut);
			FreeIcadButtonData(picCur);
		}
	}

	delete picButton;
}

int
CIcadToolBarMain::GetNewTbID()
{
// This function returns a unique button ID.

	static int	ctID;

	int	nRetID = ctID + ICAD_TBID_START;

	++ctID;

	if(nRetID > ICAD_TBID_END)
	{
		// Reset the counter if the end of the range has been reached.
		// ~| TODO - If This happens it is possible that button IDs will
		// be duplicated, which can become a problem only if the buttons
		// with the duplicate IDs are in the same toolbar.
		nRetID = ICAD_TBID_START;
		ctID = 0;
	}

	return nRetID;
}

int
CIcadToolBarMain::QueryInsertAt
(
 CIcadToolBar*	pToolBar,
 CIcadFlyOut*	pFlyOut,
 CPoint			point,
 BOOL*			pbVert,
 int*			pnInsertAt
)
{
// Given a pointer to a CToolBarCtrl and a point in client area, this function
// sets pbVert to TRUE if the layout of the toolbar is vertical or FALSE if
// horizontal.  pnInsertAt gets set to ICAD_INSERTAFTER if the point is after
// the button, ICAD_INSERTBEFORE if it is before, or ICAD_INSERTON if on a
// spacer.  The return value is the index of the button if pbVert and pnInsertAt
// were set with valid values, otherwise returns -1.

	CToolBarCtrl*	pTbCtrl = NULL;

	if(pToolBar)
		pTbCtrl = &(pToolBar->GetToolBarCtrl());
	else
		pTbCtrl = (CToolBarCtrl*)pFlyOut;

	if(!pTbCtrl)
		return -1;

	CRect	rectClient;
	pTbCtrl->GetClientRect(rectClient);
	if(!rectClient.PtInRect(point))
	{
		*pnInsertAt = ICAD_INSERTAFTER;
		*pbVert = FALSE;
		return -1;
	}

	TBBUTTON	tbButton;
	int			idxButton = GetButtonFromPt(pTbCtrl, point, &tbButton);

	if(idxButton < 0 || !(tbButton.fsState & TBSTATE_ENABLED))
	{
		// Figure out the closest visible button.
		if((idxButton = GetClosestButtonFromPt(pTbCtrl, point)) < 0)
			return -1;

		pTbCtrl->GetButton(idxButton, &tbButton);
	}

	CRect	rectButton;
	if(!pTbCtrl->GetItemRect(idxButton, rectButton))
		return -1;

	int		nVisButtons = GetVisibleButtonCount(pTbCtrl);
	CPoint	ptMid = rectButton.CenterPoint();
	BOOL	bVert = FALSE;

	if(nVisButtons <= 1)
	{
		if(pToolBar && !pToolBar->IsFloating())
		{
			CRect	rectWnd;
			pToolBar->GetWindowRect(rectWnd);
			if(rectWnd.Height() > rectWnd.Width())
				bVert = TRUE;
		}
		else
		{
			// Figure out which side of the button the point is closest to.
			int	nMinX = min(point.x - rectButton.left, rectButton.right - point.x),
				nMinY = min(point.y - rectButton.top, rectButton.bottom - point.y);

			if(nMinY < nMinX)
				bVert = TRUE;
		}
	}
	else
		if(pTbCtrl->GetRows() == nVisButtons)
			bVert = TRUE;

	if(tbButton.fsStyle & TBSTYLE_SEP)
	{
		// Insert on a spacer.
		*pnInsertAt = ICAD_INSERTON;
		*pbVert = bVert;
		return idxButton;
	}

	if(bVert)
	{
		// Vertical toolbar
		if(point.y < ptMid.y)
			// Insert before the button
			*pnInsertAt = ICAD_INSERTBEFORE;
		else
			// Insert after the button
			*pnInsertAt = ICAD_INSERTAFTER;
	}
	else
	{
		// Horizontal toolbar
		if(point.x < ptMid.x)
			// Insert before the button
			*pnInsertAt = ICAD_INSERTBEFORE;
		else
			// Insert after the button
			*pnInsertAt = ICAD_INSERTAFTER;
	}

	*pbVert = bVert;

	return idxButton;
}

BOOL
CIcadToolBarMain::DrawInsertMark
(
 CIcadToolBar*	pToolBar,
 CIcadFlyOut*	pFlyOut,
 CPoint			point,
 BOOL			bInsAsFlyOut
)
{
	BOOL	bVert;
	int		nInsertAt,
			idxButton = QueryInsertAt(pToolBar, pFlyOut, point, &bVert, &nInsertAt);

	// Return if QueryInsertAt() failed or if adding a flyout on a spacer.
	if(idxButton < 0 || (bInsAsFlyOut && nInsertAt == ICAD_INSERTON))
	{
		if(!m_rectInsertMark.IsRectEmpty())
		{
			if(m_pTbCtrlInsertMark)
			{
				m_pTbCtrlInsertMark->InvalidateRect(m_rectInsertMark);
				m_pTbCtrlInsertMark = NULL;
			}
			m_rectInsertMark.SetRectEmpty();
		}
		return FALSE;
	}

	CToolBarCtrl*	pTbCtrl = NULL;

	if(pToolBar)
		pTbCtrl = &(pToolBar->GetToolBarCtrl());
	else
		pTbCtrl = (CToolBarCtrl*)pFlyOut;

	CClientDC	dc(pTbCtrl);
	CRect		rectButton, rectMark;

	pTbCtrl->GetItemRect(idxButton, rectButton);

	CPoint		ptMid = rectButton.CenterPoint();

	COLORREF	color = ::GetSysColor(COLOR_BTNTEXT);
	CBrush		brush(color);

	if(bInsAsFlyOut)
	{
		// Set nInsertAt
		nInsertAt = ICAD_INSERTAFTER;
		// Show the mark on the adjacent side.
		bVert = !bVert;
	}

	if(bVert)
	{
		// Vertical toolbar
		rectMark.left = rectButton.left;
		rectMark.right = rectButton.right;

		switch(nInsertAt)
		{
			case ICAD_INSERTON :
				// Draw the mark over a spacer.
				rectMark.top = ptMid.y - 3;
				rectMark.bottom = ptMid.y + 3;
				break;

			case ICAD_INSERTBEFORE :
				// Draw the mark before the button
				rectMark.top = rectButton.top - 3;
				rectMark.bottom = rectButton.top + 3;
				break;

			case ICAD_INSERTAFTER :
				// Draw the mark after the button
				rectMark.top = rectButton.bottom - 3;
				rectMark.bottom=rectButton.bottom + 3;
				break;
		}

		if(rectMark == m_rectInsertMark)
			return FALSE;

		// Invalidate the old rect before drawing the new mark.
		if(m_pTbCtrlInsertMark)
			m_pTbCtrlInsertMark->RedrawWindow(m_rectInsertMark);

		// Draw the left side
		dc.FillSolidRect(rectMark.left, rectMark.top, 1, rectMark.Height(), color);
		dc.FillSolidRect(rectMark.left + 1, rectMark.top + 1, 1, rectMark.Height() - 2, color);
		// Draw the right side
		dc.FillSolidRect(rectMark.right - 1, rectMark.top, 1, rectMark.Height(), color);
		dc.FillSolidRect(rectMark.right - 2, rectMark.top + 1, 1, rectMark.Height() - 2, color);

		dc.FillSolidRect(rectMark.left, rectMark.top + rectMark.Height() / 2 - 1, rectMark.Width() - 1, 2, color);
	}
	else
	{
		// Horizontal toolbar
		rectMark.top = rectButton.top;
		rectMark.bottom = rectButton.bottom;

		switch(nInsertAt)
		{
			case ICAD_INSERTON :
				// Draw the mark over a spacer.
				rectMark.left = ptMid.x - 3;
				rectMark.right = ptMid.x + 3;
				break;

			case ICAD_INSERTBEFORE :
				// Draw the mark before the button
				rectMark.left = rectButton.left - 3;
				rectMark.right=rectButton.left + 3;
				break;

			case ICAD_INSERTAFTER :
				// Draw the mark after the button
				rectMark.left = rectButton.right - 3;
				rectMark.right = rectButton.right + 3;
				break;
		}

		if(rectMark == m_rectInsertMark)
			return FALSE;

		// Invalidate the old rect before drawing the new mark.
		if(m_pTbCtrlInsertMark)
			m_pTbCtrlInsertMark->RedrawWindow(m_rectInsertMark);

		// Draw the top
		dc.FillSolidRect(rectMark.left, rectMark.top, rectMark.Width(), 1, color);
		dc.FillSolidRect(rectMark.left + 1, rectMark.top + 1, rectMark.Width() - 2, 1, color);
		// Draw the bottom
		dc.FillSolidRect(rectMark.left, rectMark.bottom - 1, rectMark.Width(), 1, color);
		dc.FillSolidRect(rectMark.left + 1, rectMark.bottom - 2, rectMark.Width() - 2, 1, color);

		dc.FillSolidRect(rectMark.left + rectMark.Width() / 2 - 1, rectMark.top, 2, rectMark.Height(), color);
	}

	m_rectInsertMark = rectMark;
	m_pTbCtrlInsertMark = pTbCtrl;

	return TRUE;
}

void
CIcadToolBarMain::SerializeDockedState
(
 CArchive& ar,
 CMapPtrToPtr* pExcludedBars
)
{
	CDockState state;
	CPtrArray tmpHiden;
	
	if(ar.IsStoring())
	{
		m_pParentFrame->GetDockState(state);

		if(pExcludedBars)
		{
			int i;
			void* tmpPtr;
			
			for(i = state.m_arrBarInfo.GetSize(); --i >= 0;)
			{
				CControlBar* pBar = static_cast<CControlBarInfo*>(state.m_arrBarInfo[i])->m_pBar;
		
				if(pExcludedBars && !pExcludedBars->Lookup(static_cast<CControlBarInfo*>(state.m_arrBarInfo[i])->m_pBar, tmpPtr))
					continue;
				
				tmpHiden.Add(state.m_arrBarInfo[i]);
				state.m_arrBarInfo.RemoveAt(i);
			}
		}
	}
	
	state.Serialize(ar);
	
	if(!ar.IsStoring())
		m_pParentFrame->SetDockState(state);
	else
		state.m_arrBarInfo.Append(tmpHiden);
}

void
CIcadToolBarMain::SerializeDockedState
(
 XMLString& str,
 bool bStoring,
 CMapPtrToPtr* pExcludedBars
 )
{
	CMemFile memFile;// Temporary memory file
	CArchive ar(&memFile, bStoring ? CArchive::store : CArchive::load);
	DWORD sz;
	BYTE* pBuf;

	if(bStoring)
	{
		SerializeDockedState(ar, pExcludedBars);
		ar.Close();// flush data from archive

		// Now memFile filled with serialized data
		sz = memFile.GetLength();		
		pBuf = memFile.Detach();
		
		if(sz && pBuf)
		{
			str.dump((char*)pBuf, sz);// dump clears buffer
			delete [] pBuf;// CMemFile alloced memory via malloc
		}
		else
			str.resize(0);
	}
	else
	{
		// approximate size of memory to be restored
		sz = str.length()/2;
		ASSERT(sz > 0);
		pBuf = new unsigned char[sz];
		sz = str.pickup((char*)pBuf, sz);// pickup data. Now buffer filled
		memFile.Attach(pBuf, sz); // Assign buffer to memFile
		SerializeDockedState(ar, pExcludedBars); // Restore state
		memFile.Detach();		  // detach memory (we allocated so we freed memory)
		delete pBuf;
	}
}
