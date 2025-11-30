/* File  : <DevDir>\source\prj\icad\IcadToolbar.cpp
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 */
// This is the file that handles Toolbars

#include "Icad.h"/*DNT*/
#include "IcadDoc.h"/*DNT*/
#include "IcadApi.h"/*DNT*/
#include "IcadTBDlg.h"/*DNT*/
#include "IcadToolbarMain.h"/*DNT*/
#include "IcadCustom.h"/*DNT*/
#include "IcadMenuTBDef.H"/*DNT*/
#include "IcadDockContext.H"/*DNT*/
#include "cmds.h"/*DNT*/
#include "IcadDef.h"
#include "win32misc.h"


#include "configure.h"
#ifdef XML_TOOLBAR
#include "xmlnode.h"
#endif

/////////////////////////////////////////////////////////////////////////
// Toolbar functions belonging to the CMainWindow class. ~|
/////////////////////////////////////////////////////////////////////////

BOOL
CMainWindow::LoadToolbars()
{
	long	lflgVisibility;
	resbuf	rb;
	ICTOOLBARCREATESTRUCT	tbc;

	memset(&tbc, 0, sizeof(ICTOOLBARCREATESTRUCT));

	// Send all messages to this frame window.
	tbc.pParentFrame = this;

	// Set the tooltips.
	SDS_getvar(NULL, DB_QTOOLTIPS, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
	tbc.bToolTips = (BOOL)rb.resval.rint;

	// Large or small buttons.
	SDS_getvar(NULL, DB_QTBSIZE, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
	tbc.bLargeButtons = (BOOL)rb.resval.rint;

	// Color or black and white.
	SDS_getvar(NULL, DB_QTBCOLOR, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
	tbc.bColorButtons = (BOOL)rb.resval.rint;

	// Set the visibility of the toolbar buttons.
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

	// Create the toolbars
	tbc.lflgVisibility = lflgVisibility;
	m_pToolBarMain = new CIcadToolBarMain();
	((CIcadToolBarMain*)m_pToolBarMain)->SetHelpStringOutputWnd(&m_StatusBar);
#ifdef XML_TOOLBAR_AS_DEFAULT
	// XML* - VT, 6-6-02
	// Enable docking for the frame window
	EnableDocking(CBRS_ALIGN_ANY);

	if(((CIcadToolBarMain*)m_pToolBarMain)->CreateFromXML(&tbc))
		return FALSE;// Do not reset toolbar state!
#endif
	BOOL		ret=TRUE;
#ifdef XML_TOOLBAR_AS_DEFAULT
	// Disable docking for the frame window
EnableDocking(0);
#endif
	if(!((CIcadToolBarMain*)m_pToolBarMain)->CreateFromRegistry(&tbc))
	{
		((CIcadToolBarMain*)m_pToolBarMain)->CreateDefault(&tbc);
		ret = FALSE;
	}

	// Enable docking for the frame window
EnableDocking(CBRS_ALIGN_ANY);

	// If the registry does not exist, display default toolbars and return FALSE.
	CWinApp*	pApp = AfxGetApp();

	CString prefixProfileName = ResourceString(IDS_REGISTRY_PROFILES, "Profiles");
	CString profName;
	GetCurrentRegistryProfileName(profName.GetBuffer(IC_ACADBUF), IC_ACADBUF);
	profName.ReleaseBuffer();
	prefixProfileName += _T("\\") + profName;
    // GetProfileInt() uses m_pszRegistryKey + m_pszProfileName (= "ITC\\IntelliCAD" and "")
	if( !pApp->GetProfileInt(prefixProfileName + _T("\\ControlBars\\ControlBar-Summary")/*DNT*/, "Bars"/*DNT*/, 0) &&
		!pApp->GetProfileInt(prefixProfileName + _T("\\ControlBars\\ControlBar-Summary")/*DNT*/, "ScreenCX"/*DNT*/, 0) &&
		!pApp->GetProfileInt(prefixProfileName + _T("\\ControlBars\\ControlBar-Summary")/*DNT*/, "ScreenCY"/*DNT*/, 0) )
		ret = FALSE;
	else
		ret = TRUE;

	if( ret )
		return ret;

	CPtrList*	pToolBarList = ((CIcadToolBarMain*)m_pToolBarMain)->GetToolBarList();
	POSITION	pos = pToolBarList->GetHeadPosition();
	int			ct;

	for(ct = 0; pos; ++ct)
	{
		CIcadToolBar*	pToolBar = (CIcadToolBar*)pToolBarList->GetNext(pos);
		if(!pToolBar)
			continue;

#ifndef BUILD_EBATECH_JAPAN
		// ITC original toolbar layout.
		if(ct > 2 && ct != 9 && ct != 13)
		{
			DockControlBar(pToolBar, AFX_IDW_DOCKBAR_TOP);
			ShowControlBar(pToolBar, FALSE,FALSE);
			continue;
		}
		else
			if(ct == 9)
				DockControlBar(pToolBar, AFX_IDW_DOCKBAR_RIGHT);
			else
				DockControlBar(pToolBar, AFX_IDW_DOCKBAR_TOP);

		ShowControlBar(pToolBar, TRUE, FALSE);
#else
		// This is EBATECH-JAPAN original toolbar layout.
		// require 'IcadMenuTBDef.h'
		BOOL bShow = TRUE;
		CRect rect;
		CControlBar* pToolBarPre;
		CFrameWnd* pFrame;
		switch (ct){
		case 0:
			DockControlBar(pToolBar,AFX_IDW_DOCKBAR_TOP);
			break;
		case 1:
			DockControlBar(pToolBar,AFX_IDW_DOCKBAR_RIGHT);
			break;
		case 2:
			DockControlBar(pToolBar,AFX_IDW_DOCKBAR_LEFT);
			break;
		case 3:
			DockControlBar(pToolBar,AFX_IDW_DOCKBAR_LEFT);
			break;
		case 4:
			DockControlBar(pToolBar,AFX_IDW_DOCKBAR_BOTTOM);
			pToolBarPre = pToolBar;
			break;
		case 5:
			pFrame = pToolBarPre->GetDockingFrame();
			pFrame->RecalcLayout();
			pToolBarPre->GetWindowRect(&rect);
			rect.OffsetRect(1,1);
			DockControlBar(pToolBar,AFX_IDW_DOCKBAR_BOTTOM,&rect);
			break;
		default:
			DockControlBar(pToolBar,AFX_IDW_DOCKBAR_TOP);
			bShow = FALSE;
			break;
		}
		ShowControlBar(pToolBar,bShow,FALSE);
#endif  // BUILD_EBATECH_JAPAN
	}

	return ret;
}


// Message handlers for tooltips and help strings ~|

BOOL
CMainWindow::OnToolTipText
(
 UINT		nID,
 NMHDR*		pNMHDR,
 LRESULT*	pResult
)
{
	TOOLTIPTEXTW*	pTTT = (TOOLTIPTEXTW*)pNMHDR;
	return ((CIcadToolBarMain*)m_pToolBarMain)->OnToolTipText(nID, pNMHDR, pResult);
}

BOOL
CMainWindow::OnToolTipTextW
(
 UINT		nID,
 NMHDR*		pNMHDR,
 LRESULT*	pResult
)
{
	TOOLTIPTEXTW*	pTTT = (TOOLTIPTEXTW*)pNMHDR;
	return ((CIcadToolBarMain*)m_pToolBarMain)->OnToolTipTextW(nID, pNMHDR, pResult);
}


// Handlers for Customization ~|

afx_msg void
CMainWindow::QueryDelete
(
 UINT		nID,
 NMHDR*		pNotifyStruct,
 LRESULT*	result
)
{
	*result = TRUE;
}

afx_msg void
CMainWindow::QueryInsert
(
 UINT		nID,
 NMHDR*		pNotifyStruct,
 LRESULT*	result
)
{
	*result = TRUE;
}

afx_msg void
CMainWindow::BeginDrag
(
 UINT		nID,
 NMHDR*		pNotifyStruct,
 LRESULT*	result
)
{
	*result = TRUE;
}

afx_msg void
CMainWindow::EndDrag
(
 UINT		nID,
 NMHDR*		pNotifyStruct,
 LRESULT*	result
)
{
	*result = TRUE;
}

afx_msg void
CMainWindow::BeginAdjust
(
 UINT		nID,
 NMHDR*		pNotifyStruct,
 LRESULT*	result
)
{
	*result = TRUE;
}

afx_msg void
CMainWindow::ToolBarChange
(
 UINT		nID,
 NMHDR*		pNotifyStruct,
 LRESULT*	result
)
{
	*result = TRUE;
}


/////////////////////////////////////////////////////////////////////////
// Class Function Definitions for CIcadToolbar ~|
/////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CIcadToolBar, CToolBar)

BEGIN_MESSAGE_MAP(CIcadToolBar, CToolBar)
	//{{AFX_MSG_MAP(CIcadToolBar)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_ERASEBKGND()
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_NCPAINT()
	ON_WM_NCCALCSIZE()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CIcadToolBar::CIcadToolBar() :	m_pFlyOut(NULL), m_bEmpty(TRUE), m_bflgButtonDown(FALSE), m_bShiftCust(FALSE),
								m_bJustGotDblClk(FALSE), m_bPressedOnLBtnDn(FALSE), m_prevCursor(NULL),
								m_bInNcPaint(FALSE), m_pTbMain(NULL), m_nID(0), m_bIsStyleFlat(FALSE)
{
	memset(&m_dllVersion, 0, sizeof(m_dllVersion));
}

CIcadToolBar::~CIcadToolBar()
{
	// Delete the button structures.
	CToolBarCtrl&	ToolBarCtrl = GetToolBarCtrl();
	if(!ToolBarCtrl)
		return;

	int				nButtons = ToolBarCtrl.GetButtonCount(), ct;
	TBBUTTON		tbButton;
	CIcadTBbutton*	picButton;

	for(ct = 0; ct < nButtons; ++ct)
	{
		ToolBarCtrl.GetButton(ct, &tbButton);
		picButton = (CIcadTBbutton*)tbButton.dwData;

		if(picButton && m_pTbMain)
			m_pTbMain->FreeIcadButtonData(picButton);
	}

	// Delete the last flyout
	delete m_pFlyOut;
}

LRESULT
CIcadToolBar::WindowProc
(
 UINT	message,
 WPARAM	wParam,
 LPARAM	lParam
)
{
	if(m_pTbMain && message == WM_NOTIFY)
	{
		LRESULT	lResult;
		if(((LPNMHDR)lParam)->code == TTN_NEEDTEXT)
			m_pTbMain->OnToolTipText((int)wParam, (LPNMHDR)lParam, &lResult);
		else
			if(((LPNMHDR)lParam)->code == TTN_NEEDTEXTW)
				m_pTbMain->OnToolTipTextW((int)wParam, (LPNMHDR)lParam, &lResult);
	}

	return CToolBar::WindowProc(message, wParam, lParam);
}

void
CIcadToolBar::OnUpdateCmdUI
(
 CFrameWnd*	pTarget,
 BOOL		bDisableIfNoHndler
)
{
	return;
}

int
CIcadToolBar::LoadButtonsFromReg
(
 LPCTSTR	pstrTbKey,
 HKEY		hKeyTb
)
{
// If an error occurs in this function it returns the negative line number that the
// error occured on.  Otherwise return 0 for success.

	int			nBadRet;
	LPTBBUTTON	pTbButton = NULL;
	CToolBarCtrl*	pTbCtrl = &(GetToolBarCtrl());

	if(!m_pTbMain)
		return 0;

	HINSTANCE	hResDLL = NULL;
	char*		pszAltPath = NULL;

	if(!m_strMnuFileName.IsEmpty())
	{
		pszAltPath = m_strMnuFileName.GetBuffer(0);
		// Look for a resource dll and load it.
		char*	pszResDLL = new char[m_strMnuFileName.GetLength() + 5];	// +5 just in case an extension isn't specified.
		strcpy(pszResDLL, m_strMnuFileName);
		ic_setext(pszResDLL, "dll"/*DNT*/);
		hResDLL = LoadLibraryEx(pszResDLL,NULL,LOAD_LIBRARY_AS_DATAFILE);
		//hResDLL = LoadLibrary(pszResDLL);
		delete [] pszResDLL;
	}

	// Set the sizes for the buttons for this toolbar.
	if(m_pTbMain->m_bLargeButtons)
	{
		pTbCtrl->SetBitmapSize(CSize(ICAD_LG_IMGX, ICAD_LG_IMGY));
		pTbCtrl->SetButtonSize(CSize(ICAD_LG_BUTTONX, ICAD_LG_BUTTONY));
	}
	else
	{
		pTbCtrl->SetBitmapSize(CSize(ICAD_SM_IMGX, ICAD_SM_IMGY));
		pTbCtrl->SetButtonSize(CSize(ICAD_SM_BUTTONX, ICAD_SM_BUTTONY));
	}

	// Get the number of buttons for this toolbar.
	DWORD	dwRetVal, dwSize;
	int		nButtons;
	dwSize = sizeof(int);

	if(RegQueryValueEx(hKeyTb, "nButtons"/*DNT*/, NULL, &dwRetVal, (LPBYTE)&nButtons, &dwSize) != ERROR_SUCCESS)
	{
		nBadRet = -__LINE__;
		goto badout;
	}

	if(nButtons <= 0)
		return 0;

	// Allocate the array of button structures.
	pTbButton = new TBBUTTON[nButtons];
	memset(pTbButton, 0, sizeof(TBBUTTON) * nButtons);

	// Fill the array of button structures.
	// It is easy to get confused on the functionality of the following two variables.
	// ct keeps a count of the button number being read from the registry and
	// idxButtons is the index of the tbButton structure in the pTbButton array
	// that is currently being filled with the info from the registry.  These separate
	// counters are necessary because of the (pain in the ass) spacers that are
	// hidden because of the (pain in the ass) user levels.

	int				ct, idxButtons;
	CIcadTBbutton*	picButton;

	for(ct = 0, idxButtons = 0; ct < nButtons && idxButtons < nButtons; ++ct)
	{
		// Get the IcadButton information.
		picButton = new CIcadTBbutton;
		if(!picButton->readFromReg(hKeyTb, ct))
		{
			// If there is an error getting the info for this button, skip it and go
			// on to the next button.
			if(picButton)
				delete picButton;
			continue;
		}

		int	iBitmap;
		if(picButton->m_pibFlyOut)
		{
			// The button is a flyout, get the flyout button information.
			// Open the flyout key.
			CString	strFlyOutKey;
			HKEY	hKeyFlyOut;

			strFlyOutKey.Format("Button#%dFlyOut", ct);
			if(RegOpenKeyEx(hKeyTb, strFlyOutKey, 0, KEY_READ, &hKeyFlyOut) != ERROR_SUCCESS)
				picButton->m_pibFlyOut = NULL;
			else
			{
				// Get the number of flyout buttons
				int	nFlyOutButtons;
				dwSize = sizeof(int);
				if(RegQueryValueEx(hKeyFlyOut, "nButtons"/*DNT*/, NULL, &dwRetVal, (LPBYTE)&nFlyOutButtons, &dwSize) != ERROR_SUCCESS)
					picButton->m_pibFlyOut = NULL;
				else
				{
					// Get the index of the flyout button that the parent button is
					// currently pointing at.
					int	idxParentCur;
					if(RegQueryValueEx(hKeyFlyOut, "idxParentCur"/*DNT*/, NULL, &dwRetVal, (LPBYTE)&idxParentCur, &dwSize) != ERROR_SUCCESS)
						idxParentCur = 0;

					// Create the linked list of IcadButton structures for the buttons of this flyout.
					CIcadTBbutton*	pibCur = picButton;
					int				ctFlyOut;

					for(ctFlyOut = 0; ctFlyOut < nFlyOutButtons; ++ctFlyOut)
					{
						pibCur->m_pibFlyOut = new CIcadTBbutton;
						if(!pibCur->m_pibFlyOut->readFromReg(hKeyFlyOut, ctFlyOut))
							// If there is an error skip this button and go on to the next one.
							delete pibCur->m_pibFlyOut;
						else
						{
							pibCur = pibCur->m_pibFlyOut;
							pibCur->m_idxFlyOutImage = m_pTbMain->AddIcadBitmap(pibCur, 1, NULL, NULL, pszAltPath, hResDLL);

							if(ctFlyOut == idxParentCur)
								picButton->m_pibCurFlyOut = pibCur;
						}

						pibCur->m_pibFlyOut = NULL;

						//if this button has a CurFlyOut, then it is a nested toolbar - load it.
						if (pibCur->m_pibCurFlyOut)
						{
							CString	strNestedFlyOutKey;
							HKEY	hKeyNestedFlyOut;

							//open the sub key for the nested flyout
							strNestedFlyOutKey.Format("Button#%dNestedFlyOut"/*DNT*/, ctFlyOut + 1);
							if(RegOpenKeyEx(hKeyFlyOut, strNestedFlyOutKey, 0, KEY_READ, &hKeyNestedFlyOut) != ERROR_SUCCESS)
							{
								pibCur->m_pibCurFlyOut = NULL;
								continue;
							}

							// Get the number of nested flyout buttons
							int	nNestedFlyOutButtons = 0;
							dwSize = sizeof(int);
							if(RegQueryValueEx(hKeyNestedFlyOut, "nButtons"/*DNT*/, NULL, &dwRetVal, (LPBYTE)&nNestedFlyOutButtons, &dwSize) != ERROR_SUCCESS)
							{
								pibCur->m_pibCurFlyOut = NULL;
								continue;
							}

							//Load each of the buttons, creating new ones as required
							CIcadTBbutton	*pibNest = pibCur, *pibNew;
							int				ctNestedFlyOut;

							for(ctNestedFlyOut = 0; ctNestedFlyOut < nNestedFlyOutButtons; ++ctNestedFlyOut)
							{
								if(!ctNestedFlyOut)
									pibNest->m_pibCurFlyOut = pibNew = new CIcadTBbutton;
								else
									pibNest->m_pibFlyOut = pibNew = new CIcadTBbutton;

								if(!pibNew->readFromReg(hKeyNestedFlyOut, ctNestedFlyOut))
								{
									// If there is an error skip this button and go on to the next one.
									if(!ctNestedFlyOut)
										pibNest->m_pibCurFlyOut = NULL;
									else
										pibNest->m_pibFlyOut = NULL;
									delete pibNew;
								}
								else
								{
									pibNew->m_idxFlyOutImage = m_pTbMain->AddIcadBitmap(pibNew, 1, NULL, NULL, pszAltPath, hResDLL);
									pibNest = pibNew;
								}
							}

							pibNest = pibCur->m_pibCurFlyOut;

							RegCloseKey(hKeyNestedFlyOut);
						}

					}	// for(

					if (picButton->m_bChgParentBtnImg || !pszAltPath)
						iBitmap = picButton->m_pibCurFlyOut->m_idxFlyOutImage;
					else
						iBitmap = m_pTbMain->AddIcadBitmap(picButton, 1, NULL, NULL, pszAltPath, hResDLL);
				}
				RegCloseKey(hKeyFlyOut);
			}
		}

		// This is not an else because if there is an error creating the flyout we
		// can still add the button.
		if(!picButton->m_pibFlyOut)
			// The button is NOT a flyout. Load the bitmap for the button.
			iBitmap = m_pTbMain->AddIcadBitmap(picButton, 0, NULL, NULL, pszAltPath, hResDLL);

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

		pTbButton[idxButtons].iBitmap = iBitmap;
		pTbButton[idxButtons].idCommand = m_pTbMain->GetNewTbID();
		pTbButton[idxButtons].fsState = TBSTATE_ENABLED;
		pTbButton[idxButtons].fsStyle = TBSTYLE_BUTTON;
		pTbButton[idxButtons].dwData = (DWORD)picButton;
		pTbButton[idxButtons].iString = 0;
		++idxButtons;
	}

	if(hResDLL)
	{
		FreeLibrary(hResDLL);
		hResDLL = NULL;
	}
	m_pTbMain->AddToolBarBitmap(this, NULL);
	pTbCtrl->AddButtons(nButtons, pTbButton);
	delete [] pTbButton;
	return 0;

badout:
	if(hResDLL)
	{
		FreeLibrary(hResDLL);
		hResDLL = NULL;
	}

	return nBadRet;
}

#ifdef XML_TOOLBAR
int CIcadToolBar::ReadFromXML(XMLNode& node)
{
	int ret = 0;

	// Get the mnu name that the toolbar was created from.
	m_strMnuFileName = node.attribute("FileName")->c_str();
	
	// Get the unload name for the toolbar.
	m_strGroupName = node.attribute("UnloadName")->c_str();

	EnableDocking(CBRS_ALIGN_ANY);
	
	// Add the buttons to the toolbar.
	if((ret = LoadButtonsFromXML(node)) < 0)
		return ret;

	// Load docking information
	XMLString name = *node.attribute("Docking");

	if(name == "Floating")
	{
		CPoint point(node.attribute("FloatX")->toInt(), node.attribute("FloatY")->toInt());
		int nRow = node.attribute("Rows")->toInt();

		m_pTbMain->m_pParentFrame->FloatControlBar(this, point);
		if(nRow > 1)
			UpdateLayout(nRow);
	} else {
		int placement, i;
		static struct {
			TCHAR* name;
			int placementID;
		} placeDefs[] =
		{	"Top",		AFX_IDW_DOCKBAR_TOP
		,	"Bottom",	AFX_IDW_DOCKBAR_BOTTOM
		,	"Left",		AFX_IDW_DOCKBAR_LEFT
		,	"Right",	AFX_IDW_DOCKBAR_RIGHT
		};

		placement = AFX_IDW_DOCKBAR_TOP;
		for(i = sizeof(placeDefs)/sizeof(placeDefs[0]); i--;)
		{
			if(XMLString(placeDefs[i].name) == name)
			{
				placement = placeDefs[i].placementID;
				break;
			}
		}

		m_pTbMain->m_pParentFrame->DockControlBar(this, placement);
	}

	// Load Visibility flag
	if(*node.attribute("Visibility") == "Show")
		m_pTbMain->m_pParentFrame->ShowControlBar(this, TRUE, TRUE);

	return 0;
}

void
CIcadToolBar::SaveToXML(XMLNode& node)
{
	CString strText;
	CToolBarCtrl*	pTbCtrl = &GetToolBarCtrl();

	GetWindowText(strText);
	node.addAttribute("Name", XMLString(strText));
	
	// Write the name of the mnu file the toolbar was created from.
	if(!m_strMnuFileName.IsEmpty())
		node.addAttribute("FileName", XMLString(m_strMnuFileName.GetBuffer(0)));
	
	// Write the unload name of the toolbar.
	if(!m_strGroupName.IsEmpty())
		node.addAttribute("UnloadName", XMLString(m_strGroupName.GetBuffer(0)));
	
	// Write the ID that the toolbar was created with.
	node.addAttribute("ToolBarID", XMLString().assign("%i", m_nID - IDR_ICAD_TOOLBAR0));
	
	// Write the default toolbar struct ID.
	//node.addAttribute("DefID", XMLString().assign("%li", m_lDefID));
	node.addAttribute("DefID", XMLString().assign("%I64d", m_lDefID));

	// Write docking information
	XMLString dockType;

	if(IsFloating())
	{
		RECT	tbRect;

		dockType = "Floating";
		GetWindowRect(&tbRect);
		node.addAttribute("FloatX", XMLString().assign("%i", tbRect.left));
		node.addAttribute("FloatY", XMLString().assign("%i", tbRect.top));
		node.addAttribute("Rows", XMLString().assign("%i", GetToolBarCtrl().GetRows()));
	}
	else
	{
//		CControlBarInfo ctrlInfo;

		dockType = "Top";
	}
	node.addAttribute("Docking", dockType);
	node.addAttribute("Visibility", IsVisible() ? "Show" : "Hide");
	
	// This variable is for figuring out the total number of buttons including hidden spacers.
	int nButtons = pTbCtrl->GetButtonCount();
	// Write the IcadButton information.
	TBBUTTON		tbButton;
	CIcadTBbutton*	picButton;
	XMLNode* pButton;
	bool bAddSpacerBefore = false;
	
	for(int idxButton = 0; idxButton < nButtons; ++idxButton)
	{
		pTbCtrl->GetButton(idxButton, &tbButton);
		if(tbButton.fsStyle & TBSTYLE_SEP)
		{
			bAddSpacerBefore = true;
			continue;
		}
		if(!(picButton = (CIcadTBbutton*)tbButton.dwData))
			continue;
		
		if(bAddSpacerBefore)
		{
			picButton = (CIcadTBbutton*)tbButton.dwData;
			ASSERT(picButton);	// Should never happen
			picButton->m_bAddSpacerBefore = TRUE;
			bAddSpacerBefore = false;
		}
		pButton = new XMLNode("button");
		node.addChild(pButton);
		picButton->saveToXML(*pButton, true);
	}
}

class CBitmapLoadHook
{
public:
	CBitmapLoadHook(HINSTANCE hResDLL, char* path, CIcadToolBarMain* pTBM): m_pTBM(pTBM), m_Path(path), m_hResDLL(hResDLL) {}
	int load(CIcadTBbutton* pButton) { return m_pTBM->AddIcadBitmap(pButton, 0, NULL, NULL, m_Path, m_hResDLL); }

private:
	HINSTANCE m_hResDLL;
	char* m_Path;
	CIcadToolBarMain* m_pTBM;
};

int
CIcadToolBar::LoadButtonsFromXML
(
 XMLNode& node
)
{
	LPTBBUTTON	pTbButton = NULL;
	CToolBarCtrl*	pTbCtrl = &(GetToolBarCtrl());

	if(!m_pTbMain)
		return 0;

	HINSTANCE	hResDLL = NULL;
	char*		pszAltPath = NULL;

	if(!m_strMnuFileName.IsEmpty())
	{
		pszAltPath = m_strMnuFileName.GetBuffer(0);
		// Look for a resource dll and load it.
		char*	pszResDLL = new char[m_strMnuFileName.GetLength() + 5];	// +5 just in case an extension isn't specified.
		strcpy(pszResDLL, m_strMnuFileName);
		ic_setext(pszResDLL, "dll"/*DNT*/);
		hResDLL = LoadLibrary(pszResDLL);
		delete [] pszResDLL;
	}

	CBitmapLoadHook bmpLoader(hResDLL, pszAltPath, m_pTbMain);

	// Set the sizes for the buttons for this toolbar.
	if(m_pTbMain->m_bLargeButtons)
	{
		pTbCtrl->SetBitmapSize(CSize(ICAD_LG_IMGX, ICAD_LG_IMGY));
		pTbCtrl->SetButtonSize(CSize(ICAD_LG_BUTTONX, ICAD_LG_BUTTONY));
	}
	else
	{
		pTbCtrl->SetBitmapSize(CSize(ICAD_SM_IMGX, ICAD_SM_IMGY));
		pTbCtrl->SetButtonSize(CSize(ICAD_SM_BUTTONX, ICAD_SM_BUTTONY));
	}

	// Get the number of buttons for this toolbar.
	int		nButton = node.nChild();
	pTbButton = new TBBUTTON[nButton*2];
	memset(pTbButton, 0, sizeof(TBBUTTON) * nButton*2);

	int idxButton = 0;
	CIcadTBbutton* picButton;
	XMLNode::childIterator it = node.childBegin();

	for(; it != node.childEnd(); ++it)
	{
		// Get the IcadButton information.
		int	iBitmap;

		picButton = new CIcadTBbutton;
		if((iBitmap = picButton->readFromXML(**it, bmpLoader)) < 0)
		{
			// If there is an error getting the info for this button, skip it and go
			// on to the next button.
			delete picButton;
			continue;
		}

		if(picButton->m_bAddSpacerBefore)
		{
			pTbButton[idxButton].iBitmap = ICAD_SEP_WIDTH;
			pTbButton[idxButton].idCommand = 0;
			pTbButton[idxButton].fsState = TBSTATE_ENABLED;
			pTbButton[idxButton].fsStyle = TBSTYLE_SEP;
			pTbButton[idxButton].dwData = 0;
			pTbButton[idxButton].iString = 0;
			++idxButton;
		}

		pTbButton[idxButton].iBitmap = iBitmap;
		pTbButton[idxButton].idCommand = m_pTbMain->GetNewTbID();
		pTbButton[idxButton].fsState = TBSTATE_ENABLED;
		pTbButton[idxButton].fsStyle = TBSTYLE_BUTTON;
		pTbButton[idxButton].dwData = (DWORD)picButton;
		pTbButton[idxButton].iString = 0;
		++idxButton;
	}

	m_pTbMain->AddToolBarBitmap(this, NULL);
	pTbCtrl->AddButtons(idxButton, pTbButton);
	delete [] pTbButton;

	return 0;
}
#endif

BOOL
CIcadToolBar::ChangeBitmap
(
 int	idCommand,
 int	iBitmap
)
{
	return DefWindowProc(TB_CHANGEBITMAP, idCommand, (LPARAM)MAKELPARAM(iBitmap, 0));
}

void
CIcadToolBar::OnLButtonDblClk
(
 UINT	nFlags,
 CPoint	point
)
{
	m_bJustGotDblClk = TRUE;
	return;
}

void
CIcadToolBar::OnLButtonDown
(
 UINT	nFlags,
 CPoint	point
)
{
	if(!m_pTbMain)
	{
		CToolBar::OnLButtonDown(nFlags, point);
		return;
	}

	TBBUTTON		tbButton;
	CToolBarCtrl&	ToolBarCtrl = GetToolBarCtrl();

	m_idxCurDragButton = m_pTbMain->GetButtonFromPt(&ToolBarCtrl, point, &tbButton);

	if(nFlags == (MK_SHIFT | MK_LBUTTON))
		m_bShiftCust = TRUE;

	if(!m_pTbMain->m_bCustomize && !m_bShiftCust && m_idxCurDragButton != -1)
	{
		// Do this check BEFORE the button gets pressed (duh).
		if(ToolBarCtrl.GetState(tbButton.idCommand) & TBSTATE_PRESSED)
			m_bPressedOnLBtnDn = TRUE;
		else
			m_bPressedOnLBtnDn = FALSE;
	}

	CToolBar::OnLButtonDown(nFlags, point);

	if(m_idxCurDragButton == -1)
		return;

	if(!m_pTbMain->m_bCustomize && !m_bShiftCust)
	{
		CreateFlyOut(point);
		m_bflgButtonDown = TRUE;
		return;
	}

	// If the customize dialog is displayed get the button located at the point the
	// left button was pressed at.
	if(tbButton.fsStyle & TBSTYLE_SEP)
	{
		m_idxCurDragButton = -1;
		return;
	}

	m_pTbMain->OnCustButtonDown(this, NULL, NULL, m_idxCurDragButton);
	ToolBarCtrl.PressButton(tbButton.idCommand);

	// Display the information for this button in the customize dialog.
	if(!m_bShiftCust)
		m_pTbMain->DisplayCustData((CIcadTBbutton*)tbButton.dwData);

	m_prevCursor = (::SetCursor(AfxGetApp()->LoadCursor(IDC_CUSTTOOLBAR)));
	m_ptStartDrag = point;
}

void
CIcadToolBar::OnLButtonUp
(
 UINT	nFlags,
 CPoint	point
)
{
	if(!m_pTbMain)
	{
		CToolBar::OnLButtonUp(nFlags, point);
		return;
	}

	if(m_bJustGotDblClk)
	{
		m_bJustGotDblClk = FALSE;
		return;
	}

	CToolBar::OnLButtonUp(nFlags, point);

	TBBUTTON		tbButton;
	CToolBarCtrl*	pToolBarCtrl = &(GetToolBarCtrl());

	if(!m_pTbMain->m_bCustomize && !m_bShiftCust)
	{
		if(!m_bflgButtonDown)
			return;

		m_bflgButtonDown = FALSE;

		// Get the picked button info.
		int	idxButton = m_pTbMain->GetButtonFromPt(pToolBarCtrl, point, &tbButton);

		if(idxButton == -1 || (tbButton.fsStyle & TBSTYLE_SEP))
			return;

		if(!(tbButton.fsState & TBSTATE_ENABLED))
		{
			// Play a sound if the user clicked on a grayed-out toolbar button.
			cmd_PlaySound(1);
			return;
		}

		CIcadTBbutton*	picButton = (CIcadTBbutton*)tbButton.dwData;

		if(picButton->m_pibCurFlyOut)
			picButton = picButton->m_pibCurFlyOut;

		// Send the ICAD command
		SDS_SendMessage(WM_MENUSELECT, 0, (LPARAM)picButton->m_command.GetBuffer(0));

		if(m_bPressedOnLBtnDn)
		{
			// Keep the button pressed down.
			pToolBarCtrl->GetButton(m_pTbMain->m_idxCurButtonDown, &tbButton);
			pToolBarCtrl->PressButton(tbButton.idCommand);
		}

		return;
	}

	if(!m_bShiftCust)
	{
		// Keep the button pressed down.
		pToolBarCtrl->GetButton(m_pTbMain->m_idxCurButtonDown, &tbButton);
		pToolBarCtrl->PressButton(tbButton.idCommand);
	}

	m_pTbMain->DrawInsertMark(NULL, NULL, point);

	// Return if a button was not being dragged.
if(!m_prevCursor)
		return;

	// Set the cursor back to what is was before OnLButtonDown.
	::SetCursor(m_prevCursor);
	m_prevCursor = NULL;

	// If the button was not moved more than 2 pixels return (It was pressed not dragged).
	CSize	szDist(abs(m_ptStartDrag.x - point.x), abs(m_ptStartDrag.y - point.y));

	if(szDist.cx <= 2 && szDist.cy <= 2)
		return;

	// If the button was not moved more than half the width of a button insert a spacer.
	int	nBtnSizeX, nBtnSizeY;

	if(m_pTbMain->m_bLargeButtons)
	{
		nBtnSizeX = ICAD_LG_BUTTONX;
		nBtnSizeY = ICAD_LG_BUTTONY;
	}
	else
	{
		nBtnSizeX = ICAD_SM_BUTTONX;
		nBtnSizeY = ICAD_SM_BUTTONY;
	}

	int			idxButton, nRows = GetButtonRows();
	CFrameWnd*	pTbFrame = GetParentFrame();

	if(szDist.cx <= nBtnSizeX / 2 && szDist.cy <= nBtnSizeY / 2)
	{
		// Decide whether to insert the spacer before or after the button.
		int	nButtons = pToolBarCtrl->GetButtonCount(),
			nStartVal, nEndVal;
		idxButton = m_idxCurDragButton;

		if(m_pTbMain->GetVisibleButtonCount(pToolBarCtrl) == nRows)	// Vertical layout
		{
			nStartVal = m_ptStartDrag.y;
			nEndVal = point.y;
		}
		else
		{
			nStartVal = m_ptStartDrag.x;
			nEndVal = point.x;
		}

		if(nEndVal < nStartVal)	// Insert the spacer after the button
		{
			// We don't want to put a spacer on the end of a toolbar.
			if(idxButton == m_pTbMain->GetLastVisibleButtonIndex(pToolBarCtrl))
				return;

			// If there is already a spacer after the button do not insert another one.
			int	idxNextVisibleButton = m_pTbMain->GetNextVisibleButtonIndex(pToolBarCtrl, idxButton);
			pToolBarCtrl->GetButton(idxNextVisibleButton, &tbButton);
			++idxButton;
		}
		else	// Insert the spacer before the button
		{
			// We don't want a leading spacer on a toolbar.
			if(idxButton == m_pTbMain->GetFirstVisibleButtonIndex(pToolBarCtrl))
				return;

			// If there is already a spacer before the button do not insert another one.
			int	idxPrevVisibleButton = m_pTbMain->GetPrevVisibleButtonIndex(pToolBarCtrl, idxButton);
			pToolBarCtrl->GetButton(idxPrevVisibleButton, &tbButton);
		}

		if(tbButton.fsStyle & TBSTYLE_SEP)
			return;

		// Set the m_bAddSpacerBefore flag to TRUE.
		pToolBarCtrl->GetButton(idxButton, &tbButton);
		CIcadTBbutton*	picButton = (CIcadTBbutton*)tbButton.dwData;

		if(picButton)
			picButton->m_bAddSpacerBefore = TRUE;

		// Set the spacer information.
		tbButton.iBitmap = ICAD_SEP_WIDTH;
		tbButton.idCommand = 0;
		tbButton.fsState = TBSTATE_ENABLED;
		tbButton.fsStyle = TBSTYLE_SEP;
		tbButton.dwData = 0;
		tbButton.iString = 0;
		pToolBarCtrl->InsertButton(idxButton, &tbButton);
		++idxButton;
		UpdateLayout(nRows);
	}
	else
	{
		// Move the button to the new location in the toolbar.
		if((idxButton = m_pTbMain->MoveDraggedButton(this, NULL, point, m_idxCurDragButton)) == -1)
		{
			// The button was deleted.
			// Clear and disable the editboxes in the customize toolbar dialog.
			if(m_pTbMain->m_pCusTbDlg)
				m_pTbMain->m_pCusTbDlg->ResetEditboxes();

			if(!pToolBarCtrl->GetButtonCount())
			{
				// Destroy this toolbar.
				CPtrList*		pToolBarList = m_pTbMain->GetToolBarList();
				CIcadToolBar*	pToolBar;
				POSITION		pos = pToolBarList->GetHeadPosition();

				while(pos)
				{
					pToolBar = (CIcadToolBar*)pToolBarList->GetAt(pos);
					if(!pToolBar)
						continue;

					if(pToolBar == this)
						break;

					pToolBarList->GetNext(pos);
				}

				if(!pos)
					return;

				// Set the customize variables in main to invalid before deleting.
				m_pTbMain->m_idxCurButtonDown = -1;
				m_pTbMain->m_pToolBarCurBtnDown = NULL;
				m_pTbMain->m_pFlyOutCurBtnDown = NULL;

				CFrameWnd*	pFrame = GetParentFrame();
				CWnd*		pMainFrame = m_pTbMain->m_pParentFrame;
				pToolBarList->RemoveAt(pos);

				// We MUST return after the delete because this class is being deleted!
				delete this;
				return;
			}
			else if(!m_pTbMain->GetVisibleButtonCount(pToolBarCtrl))
			{
				m_pTbMain->m_pParentFrame->ShowControlBar(this, FALSE, FALSE);
				m_bEmpty = TRUE;
			}

			// Redraw the toolbar.
			UpdateLayout();
			// So the customize variable in main gets set to invalid.
			pToolBarCtrl = NULL;
		}
		else
		{
			if(m_bShiftCust)
			{
				// Don't want the button to be pressed in this case.
				pToolBarCtrl->GetButton(idxButton, &tbButton);
				pToolBarCtrl->PressButton(tbButton.idCommand, FALSE);
			}
		}
	}

	// Set the customize variables in main.
	m_pTbMain->m_idxCurButtonDown = idxButton;
	m_pTbMain->m_pToolBarCurBtnDown = this;
	if(!pToolBarCtrl)
		// If the button was deleted.
		m_pTbMain->m_pToolBarCurBtnDown = NULL;
	else
		m_pTbMain->m_pFlyOutCurBtnDown = NULL;

	m_bShiftCust = FALSE;

	if(pTbFrame)
		pTbFrame->RecalcLayout();
}

void
CIcadToolBar::OnRButtonDown
(
 UINT	nFlags,
 CPoint	point
)
{
	CToolBar::OnRButtonDown(nFlags, point);

	if(SDS_CURDOC && SDS_CURDOC->IsInPlaceActive())
		return;

	if(SDS_CMainWindow)
	{
		CPoint	ptMain(point);
		ClientToScreen(&ptMain);
		SDS_CMainWindow->ContextMenu(ptMain);
	}
}

CSize
CIcadToolBar::CalcFixedLayout
(
 BOOL	bStretch,
 BOOL	bHorz
)
{
	DWORD	dwMode = bStretch ? LM_STRETCH : 0;
	dwMode |= bHorz ? LM_HORZ : 0;

	return CalcLayout(dwMode);
}

CSize
CIcadToolBar::CalcDynamicLayout
(
 int	nLength,
 DWORD	dwMode
)
{

/*D.Gavrilov*/// don't delete a flyout (in OnCustButtonDown)
//	if(m_pTbMain && m_pTbMain->m_bCustomize && m_pFlyOut)
//		m_pTbMain->OnCustButtonDown(NULL, NULL, NULL, -1);

	if( nLength == -1 && !(dwMode & LM_MRUWIDTH) && !(dwMode & LM_COMMIT) &&
		((dwMode & LM_HORZDOCK) || (dwMode & LM_VERTDOCK)) )
		return CalcFixedLayout(dwMode & LM_STRETCH, dwMode & LM_HORZDOCK);

	CSize sizeRet = CalcLayout(dwMode, nLength);
	return sizeRet;
}


struct _AFX_CONTROLPOS
{
	int		nIndex,
			nID;
	CRect	rectOldPos;
};

CSize
CIcadToolBar::CalcLayout
(
 DWORD	dwMode,
 int	nLength
)
{
	ASSERT_VALID(this);
	ASSERT(::IsWindow(m_hWnd));
	if(dwMode & LM_HORZDOCK)
		ASSERT(dwMode & LM_HORZ);

	int			nCount;
	TBBUTTON*	pData;
	CSize		sizeResult(0, 0);

	// Load Buttons
	{
		nCount = DefWindowProc(TB_BUTTONCOUNT, 0, 0);
		if(nCount)
		{
			int	i;
			pData = new TBBUTTON[nCount];
			for(i = 0; i < nCount; ++i)
				_GetButton(i, &pData[i]);
		}
	}

	if(nCount > 0)
	{
		if(!(m_dwStyle & CBRS_SIZE_FIXED))
		{
			BOOL	bDynamic = m_dwStyle & CBRS_SIZE_DYNAMIC;

			if(bDynamic && (dwMode & LM_MRUWIDTH))
				SizeToolBar(pData, nCount, m_nMRUWidth);
			else if(bDynamic && (dwMode & LM_HORZDOCK))
				SizeToolBar(pData, nCount, 32767);
			else if(bDynamic && (dwMode & LM_VERTDOCK))
				SizeToolBar(pData, nCount, 0);
			else if(bDynamic && nLength != -1)
			{
				CRect	rect;
				rect.SetRectEmpty();
				CalcInsideRect(rect, dwMode & LM_HORZ);
				BOOL	bVert = (dwMode & LM_LENGTHY);
				int		nLen = nLength + (bVert ? rect.Height() : rect.Width());

				SizeToolBar(pData, nCount, nLen, bVert);
			}
			else if(bDynamic && (m_dwStyle & CBRS_FLOATING))
				SizeToolBar(pData, nCount, m_nMRUWidth);
			else
				SizeToolBar(pData, nCount, (dwMode & LM_HORZ) ? 32767 : 0);
		}

		sizeResult = CalcSize(pData, nCount);

		if(dwMode & LM_COMMIT)
		{
			_AFX_CONTROLPOS*	pControl = NULL;
			int					nControlCount = 0;
			BOOL				bIsDelayed = m_bDelayedButtonLayout;
			m_bDelayedButtonLayout = FALSE;

			for(int i = 0; i < nCount; ++i)
			{
				if((pData[i].fsStyle & TBSTYLE_SEP) && pData[i].idCommand)
					++nControlCount;
			}

			if(nControlCount > 0)
			{
				pControl = new _AFX_CONTROLPOS[nControlCount];
				nControlCount = 0;

				for(int i = 0; i < nCount; ++i)
				{
					if((pData[i].fsStyle & TBSTYLE_SEP) && pData[i].idCommand)
					{
						pControl[nControlCount].nIndex = i;
						pControl[nControlCount].nID = pData[i].idCommand;

						CRect	rect;
						GetItemRect(i, &rect);
						ClientToScreen(&rect);
						pControl[nControlCount].rectOldPos = rect;

						++nControlCount;
					}
				}
			}

			if((m_dwStyle & CBRS_FLOATING) && (m_dwStyle & CBRS_SIZE_DYNAMIC))
				m_nMRUWidth = sizeResult.cx;

			for(i = 0; i < nCount; ++i)
				_SetButton(i, &pData[i]);

			if(nControlCount > 0)
			{
				for(int i = 0; i < nControlCount; ++i)
				{
					CWnd*	pWnd = GetDlgItem(pControl[i].nID);
					if(pWnd)
					{
						CRect	rect;
						pWnd->GetWindowRect(&rect);
						CPoint	pt = rect.TopLeft() - pControl[i].rectOldPos.TopLeft();
						GetItemRect(pControl[i].nIndex, &rect);
						pt += rect.TopLeft();
						pWnd->SetWindowPos(NULL, pt.x, pt.y, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
					}
				}
				delete[] pControl;
			}
			m_bDelayedButtonLayout = bIsDelayed;
		}
		delete[] pData;
	}

	//BLOCK: Adjust Margins
	{
		CRect	rect;
		rect.SetRectEmpty();
		CalcInsideRect(rect, (dwMode & LM_HORZ));
		sizeResult.cy -= rect.Height();
		sizeResult.cx -= rect.Width();
		CSize	size = CControlBar::CalcFixedLayout(dwMode & LM_STRETCH, dwMode & LM_HORZ);
		sizeResult.cx = max(sizeResult.cx, size.cx);
		sizeResult.cy = max(sizeResult.cy, size.cy);
	}

	return sizeResult;
}

#ifdef _MAC
 #define CX_OVERLAP  1
#else
 #define CX_OVERLAP  0
#endif

CSize
CIcadToolBar::CalcSize
(
 TBBUTTON*	pData,
 int		nCount
)
{
	ASSERT(pData && nCount > 0);

	CPoint	cur(0, 0);
	CSize	sizeResult(0, 0);

	for(int i = 0; i < nCount; ++i)
	{
		if(pData[i].fsState & TBSTATE_HIDDEN)
			continue;

		if(pData[i].fsStyle & TBSTYLE_SEP)
		{
			// A separator represents either a height or width
			if(pData[i].fsState & TBSTATE_WRAP)
			{
				if(!m_bIsStyleFlat || (m_dllVersion.dwMajorVersion == 4 && m_dllVersion.dwMinorVersion <= 70))
					sizeResult.cy = max(cur.y + m_sizeButton.cy + pData[i].iBitmap * 2 / 3, sizeResult.cy);
				else
					sizeResult.cy = max(cur.y + m_sizeButton.cy + pData[i].iBitmap * 1, sizeResult.cy);
			}
			else
				sizeResult.cx = max(cur.x + pData[i].iBitmap, sizeResult.cx);
		}
		else
		{
			sizeResult.cx = max(cur.x + m_sizeButton.cx, sizeResult.cx);
			sizeResult.cy = max(cur.y + m_sizeButton.cy, sizeResult.cy);
		}

		if(pData[i].fsStyle & TBSTYLE_SEP)
			cur.x += pData[i].iBitmap;
		else
			cur.x += m_sizeButton.cx - CX_OVERLAP;

		if(pData[i].fsState & TBSTATE_WRAP)
		{
			cur.x = 0;
			cur.y += m_sizeButton.cy;
			if(pData[i].fsStyle & TBSTYLE_SEP)
			{
				if(!m_bIsStyleFlat || (m_dllVersion.dwMajorVersion == 4 && m_dllVersion.dwMinorVersion <= 70))
					cur.y += pData[i].iBitmap * 2 / 3;
				else
					cur.y += pData[i].iBitmap * 1;
			}
		}
	}

	return sizeResult;
}

void
CIcadToolBar::_GetButton
(
 int		nIndex,
 TBBUTTON*	pButton
)
{
	CIcadToolBar*	pBar = (CIcadToolBar*)this;
	VERIFY(pBar->DefWindowProc(TB_GETBUTTON, nIndex, (LPARAM)pButton));
	// TBSTATE_ENABLED == TBBS_DISABLED so invert it
	pButton->fsState ^= TBSTATE_ENABLED;
}

void
CIcadToolBar::_SetButton
(
 int		nIndex,
 TBBUTTON*	pButton
)
{
	// get original button state
	TBBUTTON	button;
	VERIFY(DefWindowProc(TB_GETBUTTON, nIndex, (LPARAM)&button));

	// prepare for old/new button comparsion
	button.bReserved[0] = 0;
	button.bReserved[1] = 0;
	// TBSTATE_ENABLED == TBBS_DISABLED so invert it
	pButton->fsState ^= TBSTATE_ENABLED;
	pButton->bReserved[0] = 0;
	pButton->bReserved[1] = 0;

	// nothing to do if they are the same
	if(memcmp(pButton, &button, sizeof(TBBUTTON)))
	{
		// don't redraw everything while setting the button
		DWORD	dwStyle = GetStyle();
		ModifyStyle(WS_VISIBLE, 0);
		VERIFY(DefWindowProc(TB_DELETEBUTTON, nIndex, 0));
		VERIFY(DefWindowProc(TB_INSERTBUTTON, nIndex, (LPARAM)pButton));
		ModifyStyle(0, dwStyle & WS_VISIBLE);

		// invalidate appropriate parts
		if( ((pButton->fsStyle ^ button.fsStyle) & TBSTYLE_SEP) ||
			((pButton->fsStyle & TBSTYLE_SEP) && pButton->iBitmap != button.iBitmap) )
			// changing a separator
			Invalidate(FALSE);
		else
		{
			// invalidate just the button
			CRect	rect;
			if(DefWindowProc(TB_GETITEMRECT, nIndex, (LPARAM)&rect))
				InvalidateRect(rect, FALSE);	// don't erase background
		}
	}
}

// input CRect should be client rectangle size
void
CIcadToolBar::CalcInsideRect
(
 CRect&	rect,
 BOOL	bHorz
)
{
	ASSERT_VALID(this);
	DWORD	dwStyle = m_dwStyle;

	if(dwStyle & CBRS_BORDER_LEFT)
		rect.left += 2;	//afxData.cxBorder2;
	if(dwStyle & CBRS_BORDER_TOP)
		rect.top += 2;	//afxData.cyBorder2;
	if(dwStyle & CBRS_BORDER_RIGHT)
		rect.right -= 2;	//afxData.cxBorder2;
	if(dwStyle & CBRS_BORDER_BOTTOM)
		rect.bottom -= 2;	//afxData.cyBorder2;

	// inset the top and bottom.
	if(bHorz)
	{
		rect.left += m_cxLeftBorder;
		rect.top += m_cyTopBorder;
		rect.right -= m_cxRightBorder;
		rect.bottom -= m_cyBottomBorder;
	}
	else
	{
		rect.left += m_cyTopBorder;
		rect.top += m_cxLeftBorder;
		rect.right -= m_cyBottomBorder;
		rect.bottom -= m_cxRightBorder;
	}

	if(m_bIsStyleFlat && IsFloating())
		// Remove the space occupied by the resize bar on docked toolbars.
		rect.left -= 8;
}

void
CIcadToolBar::OnMouseMove
(
 UINT	nFlags,
 CPoint	point
)
{
	if(!m_pTbMain)
	{
		CToolBar::OnMouseMove(nFlags, point);
		return;
	}

	TBBUTTON		tbButton;
	CToolBarCtrl*	pToolBarCtrl = &(GetToolBarCtrl());
	BOOL			bShiftCust = FALSE;

	if(m_bShiftCust)
	{
		if(nFlags != (MK_SHIFT | MK_LBUTTON))
		{
			m_bShiftCust = FALSE;
			m_pTbMain->DrawInsertMark(NULL, NULL, point);
			::SetCursor(m_prevCursor);
			m_prevCursor = NULL;
		}
	}

	if(m_pTbMain->m_bCustomize || m_bShiftCust)
	{
		if(nFlags & MK_LBUTTON)
		{
			CRect	rectClient;
			GetClientRect(rectClient);
			if(rectClient.PtInRect(point))
				::SetCursor(AfxGetApp()->LoadCursor(IDC_CUSTTOOLBAR));
			else
				::SetCursor(AfxGetApp()->LoadCursor(IDC_CUSTTOOLBARDEL));

			m_pTbMain->DrawInsertMark(this, NULL, point);
			return;
		}
		CreateFlyOut(point);
	}

	int	idxButton = m_pTbMain->GetButtonFromPt(pToolBarCtrl, point, &tbButton);
	if(idxButton == -1 || (tbButton.fsStyle & TBSTYLE_SEP) || !(tbButton.fsState & TBSTATE_ENABLED) || nFlags)
		m_pTbMain->SetCurToolTipPtr(NULL);
	else
	{
		CIcadTBbutton*	picButton = (CIcadTBbutton*)tbButton.dwData;

		if(picButton->m_pibCurFlyOut)
			picButton = picButton->m_pibCurFlyOut;

		m_pTbMain->SetCurToolTipPtr(picButton);
		CWnd*	pWnd = m_pTbMain->GetHelpStringOutputWnd();
		if(pWnd)
			pWnd->SetWindowText(picButton->m_helpString);
	}

	CToolBar::OnMouseMove(nFlags, point);
}

BOOL
CIcadToolBar::CreateFlyOut
(
 CPoint	point
)
{
	// Get the index of the button picked.
	CToolBarCtrl&	ToolBarCtrl = GetToolBarCtrl();
	TBBUTTON		tbButton;
	int				idxButton = m_pTbMain->GetButtonFromPt(&ToolBarCtrl, point, &tbButton);

	if(idxButton == -1 || (tbButton.fsStyle & TBSTYLE_SEP) || !(tbButton.fsState & TBSTATE_ENABLED))
		return FALSE;

	CIcadTBbutton*	picButton = (CIcadTBbutton*)tbButton.dwData;

	// Return if the picked button is not a flyout.
	if(!picButton || !picButton->m_pibFlyOut)
		return FALSE;

	CRect	rectButton;
	GetItemRect(idxButton, &rectButton);

	// Make sure more than one button is visible for this flyout.
	CIcadTBbutton*	picCur;
	int				nButtons;
	long			lflgVisibility = m_pTbMain->m_lflgVisibility;

	for(picCur = picButton->m_pibFlyOut, nButtons = 0; picCur; picCur = picCur->m_pibFlyOut)
	{
		if((lflgVisibility & picCur->m_lflgVisibility) == lflgVisibility)
			++nButtons;
	}

	if(nButtons <= 1)
		return FALSE;

	// Figure out the size of the flyout.
	CRect	rectFlyOut(rectButton), rectToolBar, rectDeskTop;
	GetWindowRect(rectToolBar);

	SystemParametersInfo(SPI_GETWORKAREA, 0, (PVOID)&rectDeskTop, 0);
	ScreenToClient(rectDeskTop);

	BOOL	bReverseImages = FALSE;
	int		nBtnSize;

	if(rectToolBar.Height() < rectToolBar.Width())	// Create a vertical flyout
	{
		if(m_pTbMain->m_bLargeButtons)
			nBtnSize = ICAD_LG_BUTTONY;
		else
			nBtnSize = ICAD_SM_BUTTONY;

		if(nBtnSize * nButtons >= rectDeskTop.bottom)
		{
			rectFlyOut.InflateRect(0, nBtnSize * (nButtons - 1), 0, 0);	/*D.Gavrilov*///"- 1" for removing an extra place
			bReverseImages = TRUE;
		}
		else
		{
			rectFlyOut.InflateRect(0, 0, 0, nBtnSize * (nButtons - 1));	/*D.Gavrilov*///"- 1" for removing an extra place
			bReverseImages = FALSE;
		}
	}
	else	// Create a horizontal flyout.
	{
		if(m_pTbMain->m_bLargeButtons)
			nBtnSize = ICAD_LG_BUTTONX;
		else
			nBtnSize = ICAD_SM_BUTTONX;

		if(nBtnSize * nButtons >= rectDeskTop.right)
		{
			rectFlyOut.InflateRect(nBtnSize * (nButtons - 1), 0, 0, 0);	/*D.Gavrilov*///"- 1" for removing an extra place
			bReverseImages = TRUE;
		}
		else
		{
			rectFlyOut.InflateRect(0, 0, nBtnSize * (nButtons - 1), 0);	/*D.Gavrilov*///"- 1" for removing an extra place
			bReverseImages = FALSE;
		}
	}

	// Create the flyout.
	ClientToScreen(rectFlyOut);
	m_pFlyOut = new CIcadFlyOut();
	m_pFlyOut->m_idxParentButton = idxButton;
	m_pFlyOut->m_pTbOwner = this;
	m_pFlyOut->m_pTbMain = m_pTbMain;
	m_pFlyOut->m_strMnuFileName = m_strMnuFileName;
	m_pFlyOut->Create(rectFlyOut, picButton, bReverseImages);

	return TRUE;
}

// From MFC source BARDOCK.CPP 10/30/96 12:41 PM
void
CIcadToolBar::EnableDocking
(
 DWORD	dwDockStyle
)
{
// Always call this function instead of CControlBar::EnableDocking so we
// can use our CDocContext.

	// Call the base class if this is called from explorer.
	if(!m_pTbMain)
	{
		CToolBar::EnableDocking(dwDockStyle);
		return;
	}

	// must be CBRS_ALIGN_XXX or CBRS_FLOAT_MULTI only
	ASSERT(!(dwDockStyle & ~(CBRS_ALIGN_ANY | CBRS_FLOAT_MULTI)));
	// CBRS_SIZE_DYNAMIC toolbar cannot have the CBRS_FLOAT_MULTI style
	ASSERT(!(dwDockStyle & CBRS_FLOAT_MULTI) || !(m_dwStyle & CBRS_SIZE_DYNAMIC));

	m_dwDockStyle = dwDockStyle;
	if(!m_pDockContext)
		m_pDockContext = new CIcadDockContext(this);

	// permanently wire the bar's owner to its current parent
	if(!m_hWndOwner)
		m_hWndOwner = ::GetParent(m_hWnd);
}

BOOL
CIcadToolBar::OnEraseBkgnd
(
 CDC*	pDC
)
{
	BOOL	bRet = CToolBar::OnEraseBkgnd(pDC);

	if(!m_bIsStyleFlat)
		return bRet;

	// Loop through the toolbar and paint the spacers.
	CToolBarCtrl&	ToolBarCtrl = GetToolBarCtrl();

	if(!ToolBarCtrl.GetSafeHwnd())
		return bRet;

	int	nButtons = ToolBarCtrl.GetButtonCount(),
		nVisibleButtons = nButtons;

	if(m_pTbMain)
		nVisibleButtons = m_pTbMain->GetVisibleButtonCount(&ToolBarCtrl);

	if(nButtons < 1 || nVisibleButtons < 1)
		return bRet;

	int			nRows = GetButtonRows(), idxFirstVisBtn = 0;
	TBBUTTON	tbButton;
	CPen		*pHilitePen = new CPen(PS_SOLID, 0, GetSysColor(COLOR_BTNHILIGHT)),
				*pShadowPen = new CPen(PS_SOLID, 0, GetSysColor(COLOR_BTNSHADOW)),
				*pOldPen = (CPen*)pDC->SelectObject(pHilitePen);
	CRect		rect, rectPrev, rectNext, rectWnd;

	if(m_pTbMain)
		idxFirstVisBtn = m_pTbMain->GetFirstVisibleButtonIndex(&ToolBarCtrl);

	GetWindowRect(rectWnd);
	ToolBarCtrl.GetItemRect(idxFirstVisBtn, rect);

	if(m_bInNcPaint)
	{
		ScreenToClient(rectWnd);
		rect.OffsetRect(-rectWnd.left, -rectWnd.top);
	}

	int		nPos, ct;
	// Figure out the layout.
	BOOL	bVert = FALSE;

	if(nRows > 1)
	{
		if(nRows == nVisibleButtons)
			bVert = TRUE;
	}
	else
	{
		if(rectWnd.Height() > rectWnd.Width())
			bVert = TRUE;
	}

	if(!IsFloating())
	{
		// Draw the resize bar.
		if(bVert)
		{
			// Vertical toolbar
			nPos = rect.top - m_cxLeftBorder + 3;

			for(ct = 0; ct < 2; ++ct)
			{
				pDC->MoveTo(rect.left - 2, nPos + 2);
				pDC->LineTo(rect.left - 2, nPos);
				pDC->LineTo(rect.right, nPos);
				pDC->SelectObject(pShadowPen);
				pDC->LineTo(rect.right, nPos + 2);
				pDC->LineTo(rect.left - 2, nPos + 2);
				pDC->SelectObject(pHilitePen);
				nPos += 4;
			}
		}
		else
		{
			// Horizontal toolbar
			nPos = rect.left - m_cxLeftBorder + 1;

			for(ct = 0; ct < 2; ++ct)
			{
				pDC->MoveTo(nPos + 1, rect.bottom);
				pDC->LineTo(nPos + 1, rect.top - 2);
				pDC->LineTo(nPos + 3, rect.top - 2);
				pDC->SelectObject(pShadowPen);
				pDC->MoveTo(nPos + 3, rect.top - 1);
				pDC->LineTo(nPos + 3, rect.bottom + 1);
				pDC->LineTo(nPos + 1, rect.bottom + 1);
				pDC->SelectObject(pHilitePen);
				nPos += 4;
			}
		}
	}

	// Only draw separators for IE 3.0 version of comctl32.dll
	if(m_dllVersion.dwMajorVersion == 4 && m_dllVersion.dwMinorVersion <= 70)
	{
		CRect	rectClient;
		GetClientRect(rectClient);

		for(ct = 0; ct < nButtons; ++ct)
		{
			ToolBarCtrl.GetButton(ct, &tbButton);
			if(!(tbButton.fsStyle & TBSTYLE_SEP))
				continue;

			pDC->SelectObject(pShadowPen);
			if(tbButton.fsState & TBSTATE_WRAP)
			{
				// Draw horizontal lines
				int	idxPrev, idxNext;
				if(!m_pTbMain)
				{
					idxPrev = ct - 1;
					idxNext = ct + 1;
				}
				else
				{
					idxPrev = m_pTbMain->GetPrevVisibleButtonIndex(&ToolBarCtrl, ct);
					idxNext = m_pTbMain->GetNextVisibleButtonIndex(&ToolBarCtrl, ct);
				}

				ToolBarCtrl.GetItemRect(idxPrev, rectPrev);
				ToolBarCtrl.GetItemRect(idxNext, rectNext);

				if(m_bInNcPaint)
				{
					rectPrev.OffsetRect(-rectWnd.left, -rectWnd.top);
					rectNext.OffsetRect(-rectWnd.left, -rectWnd.top);
				}

				nPos = rectPrev.bottom + (rectNext.top - rectPrev.bottom) / 2 - 1;

				if(bVert)
				{
					pDC->MoveTo(rectPrev.left, nPos);
					pDC->LineTo(rectPrev.right-1, nPos);
					pDC->SelectObject(pHilitePen);
					pDC->MoveTo(rectPrev.left, nPos + 1);
					pDC->LineTo(rectPrev.right-1, nPos + 1);
				}
				else
				{
					pDC->MoveTo(rectClient.left, nPos);
					pDC->LineTo(rectClient.right, nPos);
					pDC->SelectObject(pHilitePen);
					pDC->MoveTo(rectClient.left, nPos + 1);
					pDC->LineTo(rectClient.right, nPos + 1);
				}
			}
			else
			{
				// Draw vertical lines on a horizontal or floating toolbar
				ToolBarCtrl.GetItemRect(ct, rect);
				if(m_bInNcPaint)
					rect.OffsetRect(-rectWnd.left, -rectWnd.top);

				nPos = rect.left + rect.Width() / 2 - 1;
				pDC->MoveTo(nPos, rect.top);
				pDC->LineTo(nPos, rect.bottom);
				pDC->SelectObject(pHilitePen);
				pDC->MoveTo(nPos + 1, rect.top);
				pDC->LineTo(nPos + 1, rect.bottom);
			}
		}
	}

	pDC->SelectObject(pOldPen);
	delete pHilitePen;
	delete pShadowPen;

	return bRet;
}

void
CIcadToolBar::OnNcPaint()
{
	m_bInNcPaint = TRUE;
	if(!m_bIsStyleFlat || !m_pTbMain)
		EraseNonClient();
	else
	{
		// get window DC that is clipped to the non-client area
		CWindowDC	dc(this);
		CRect		rectClient, rectWindow;

		GetClientRect(rectClient);
		GetWindowRect(rectWindow);
		ScreenToClient(rectWindow);
		rectClient.OffsetRect(-rectWindow.left, -rectWindow.top);
		dc.ExcludeClipRect(rectClient);

		// draw borders in non-client area
		rectWindow.OffsetRect(-rectWindow.left, -rectWindow.top);

		CRect		rectDraw(rectWindow);
		COLORREF	clrHilite = ::GetSysColor(COLOR_BTNHILIGHT),
					clrShadow = ::GetSysColor(COLOR_BTNSHADOW),
					clrFace = ::GetSysColor(COLOR_BTNFACE);
		int			cxBorder = 1, cyBorder = 1,	cxBorder2 = 2, cyBorder2 = 2;
		DWORD		dwStyle = m_dwStyle;

		if(dwStyle & CBRS_BORDER_ANY)
		{
			// prepare for dark lines
			ASSERT(!rectDraw.top && !rectDraw.left);

			COLORREF	clr = clrHilite;

			// draw left and top
			if(dwStyle & CBRS_BORDER_LEFT)
				dc.FillSolidRect(0, rectDraw.top, cxBorder, rectDraw.Height(), clr);
			if(dwStyle & CBRS_BORDER_TOP)
				dc.FillSolidRect(0, 0, rectDraw.right, cyBorder, clr);

			clr = clrShadow;
			// draw right and bottom
			if(dwStyle & CBRS_BORDER_RIGHT)
				dc.FillSolidRect(rectDraw.right, rectDraw.top, -cxBorder, rectDraw.Height(), clr);
			if(dwStyle & CBRS_BORDER_BOTTOM)
				dc.FillSolidRect(0, rectDraw.bottom, rectDraw.right, -cyBorder, clr);

			if(dwStyle & CBRS_BORDER_LEFT)
				rectDraw.left += cxBorder;
			if(dwStyle & CBRS_BORDER_TOP)
				rectDraw.top += cyBorder;
			if(dwStyle & CBRS_BORDER_RIGHT)
				rectDraw.right -= cxBorder;
			if(dwStyle & CBRS_BORDER_BOTTOM)
				rectDraw.bottom -= cyBorder;
		}

		// erase parts not drawn
		dc.IntersectClipRect(rectDraw);
		SendMessage(WM_ERASEBKGND, (WPARAM)dc.m_hDC);
	}

	m_bInNcPaint = FALSE;
}

void
CIcadToolBar::OnWindowPosChanging
(
 WINDOWPOS FAR*	lpwndpos
)
{
	if(!m_bIsStyleFlat)
	{
		CToolBar::OnWindowPosChanging(lpwndpos);
		return;
	}

	if(!(lpwndpos->flags & SWP_NOMOVE))
	{	// if moved:
		CRect		rc;		// Fill rectangle with..
		GetWindowRect(&rc); // ..my (toolbar) rectangle.
		CFrameWnd*	pParent = GetParentFrame();	// get parent (dock bar/frame) win..
		pParent->ScreenToClient(&rc);			// .. and convert to parent coords

		// Ask parent window to paint the area beneath my old location.
		// Typically, this is just solid grey.
		//
		pParent->InvalidateRect(&rc); // paint old rectangle

		// Now paint my non-client area at the new location.
		// This is the extra bit of border space surrounding the buttons.
		// Without this, you will still have a partial display bug (try it!)
		//
		PostMessage(WM_NCPAINT);
	}
}

void
CIcadToolBar::OnNcCalcSize
(
 BOOL					bCalcValidRects,
 NCCALCSIZE_PARAMS FAR*	lpncsp
)
{
	if(!m_bIsStyleFlat)
	{
		CToolBar::OnNcCalcSize(bCalcValidRects, lpncsp);
		return;
	}

	// calculate border space (will add to top/bottom, subtract from right/bottom)
	CRect	rect;
	rect.SetRectEmpty();
	BOOL	bHorz = !!(m_dwStyle & CBRS_ORIENT_HORZ);
	CalcInsideRect(rect, bHorz);
	ASSERT(rect.top >= 2);

	// adjust non-client area for border space
	lpncsp->rgrc[0].left += rect.left;
	lpncsp->rgrc[0].top += rect.top - 2;
	lpncsp->rgrc[0].right += rect.right;
	lpncsp->rgrc[0].bottom += rect.bottom;
}

void
CIcadToolBar::UpdateLayout
(
 int	nRows
)
{
	if(IsWindowVisible() || nRows)	// Don't worry about visibility if nRows is specified.
	{
		if(IsFloating())
		{
			// Resize a floating toolbar to reflect added or deleted buttons.
			CFrameWnd*	pFrame = GetParentFrame();
			if(pFrame)
			{
				if(!nRows || !SetRows(nRows))
					CalcDynamicLayout(-1, LM_HORZ | LM_COMMIT);
				pFrame->RecalcLayout();
			}
		}
		else
		{
			// Resize a vertically docked toolbar (docked horizontal is fine).
			CRect	rect;
			GetWindowRect(rect);
			if(rect.Height() > rect.Width())
				CalcDynamicLayout(-1, LM_VERTDOCK | LM_COMMIT);
		}
	}
}

int
CIcadToolBar::GetButtonRows()
{
// This function counts the number of buttons with the TBSTATE_WRAP style set.
// This is different than CToolBarCtrl::GetRows() in that spacers are NOT counted
// as a row.  CToolBarCtrl::GetRows() should not be used.

	CToolBarCtrl*	pTbCtrl = &GetToolBarCtrl();
	// Only count spacers as a row when the toolbar has a vertical layout.
	int	nRows = pTbCtrl->GetRows(),
		nButtons = pTbCtrl->GetButtonCount(),
		nVisButtons = nButtons;

	if(m_pTbMain)
		nVisButtons = m_pTbMain->GetVisibleButtonCount(pTbCtrl);

	if(nVisButtons == 1)
		return 1;

	if(m_pTbMain && nRows == nVisButtons)
		return nRows;

	int			i;
	TBBUTTON	tbButton;

	for(i = 0, nRows = 0; i < nButtons; ++i)
	{
		_GetButton(i, &tbButton);
		if(!(tbButton.fsState & TBSTATE_HIDDEN) && (tbButton.fsState & TBSTATE_WRAP))
			++nRows;
	}

	return nRows + 1;
}

BOOL
CIcadToolBar::SetRows
(
 int	nRows
)
{
	BOOL		bRet = FALSE;
	TBBUTTON*	pData = NULL;
	int			nButtons = 0, nCurRows = 0;
	CSize		sizeMax, sizeMin, sizeMid;

	// Load Buttons
	nButtons = DefWindowProc(TB_BUTTONCOUNT, 0, 0);
	if(nButtons)
	{
		pData = new TBBUTTON[nButtons];
		for(int i = 0; i < nButtons; ++i)
			_GetButton(i, &pData[i]);
	}

	if(nButtons < 1)
		goto out;

	// Wrap ToolBar vertically
	nCurRows = WrapToolBar(pData, nButtons, 0);
	sizeMin = CalcSize(pData, nButtons);

	if(nCurRows == nRows)
	{
		sizeMid = sizeMin;
		bRet = TRUE;
		goto out;
	}

	// Wrap ToolBar horizontally
	nCurRows = WrapToolBar(pData, nButtons, 32767);
	sizeMax = CalcSize(pData, nButtons);

	if(nCurRows == nRows)
	{
		sizeMid = sizeMax;
		bRet = TRUE;
		goto out;
	}

	while(sizeMin.cx < sizeMax.cx)
	{
		sizeMid.cx = (sizeMin.cx + sizeMax.cx) / 2;
		nCurRows = WrapToolBar(pData, nButtons, sizeMid.cx);
		sizeMid = CalcSize(pData, nButtons);

		if(nCurRows > nRows)
		{
			if(sizeMin == sizeMid)
			{
				WrapToolBar(pData, nButtons, sizeMax.cx);
				goto out;
			}
			sizeMin = sizeMid;
		}
		else if(nCurRows < nRows)
		{
			sizeMax = sizeMid;
		}
		else
		{
			bRet = TRUE;
			goto out;
		}
	}

out:
	if(bRet)
	{
		// Resize the toolbar.
		CRect	rect;
		rect.SetRectEmpty();
		CalcInsideRect(rect, LM_HORZ);
		CalcDynamicLayout(sizeMid.cx - rect.Width(), LM_HORZ | LM_COMMIT);
	}

	delete pData;

	return bRet;
}

BOOL
CIcadToolBar::SetDllVersion()
{
	BOOL		bRet = TRUE;
	HINSTANCE	hinstDll;
	HRESULT		hr;
	//Load the DLL.
	if(!(hinstDll = LoadLibrary("comctl32.dll"/*DNT*/)))
		return FALSE;

	DLLGETVERSIONPROC	pDllGetVersion;
	/*
	You must get this function explicitly because the DLL might not implement
	the function. Depending upon the DLL, the lack of implementation of the
	function may be a version marker in itself.
	*/
	pDllGetVersion = (DLLGETVERSIONPROC)GetProcAddress(hinstDll, TEXT("DllGetVersion"/*DNT*/));

	if(!pDllGetVersion)
	{
		bRet = FALSE;
		goto out;
	}

	memset(&m_dllVersion, 0, sizeof(m_dllVersion));
	m_dllVersion.cbSize = sizeof(m_dllVersion);

	hr = (*pDllGetVersion)(&m_dllVersion);

	if(!SUCCEEDED(hr))
	{
		bRet = FALSE;
		goto out;
	}

out:
   FreeLibrary(hinstDll);
   return bRet;
}


/////////////////////////////////////////////////////////////////////////
// CIcadTBbutton methods
/////////////////////////////////////////////////////////////////////////

void
CIcadTBbutton::saveToReg
(
 HKEY	hKey,
 int	idxButton
)
{
	CString	strButtonKey;
	strButtonKey.Format("Button#%d"/*DNT*/, idxButton);

	// Write the IcadButton structure
	RegSetValueEx(hKey, strButtonKey, 0, REG_BINARY, (CONST BYTE*)&m_data4reg, sizeof(m_data4reg));

	// Write the strings associated with the IcadButton structure.
	if(!m_toolTip.IsEmpty())
	{
		strButtonKey.Format("Button#%dToolTip"/*DNT*/, idxButton);
		RegSetValueEx(hKey, strButtonKey, 0, REG_SZ, (CONST BYTE*)(LPCTSTR)m_toolTip, m_toolTip.GetLength());
	}

	if(!m_helpString.IsEmpty())
	{
		strButtonKey.Format("Button#%dHelpString"/*DNT*/, idxButton);
		RegSetValueEx(hKey, strButtonKey, 0, REG_SZ, (CONST BYTE*)(LPCTSTR)m_helpString, m_helpString.GetLength());
	}

	if(!m_command.IsEmpty())
	{
		strButtonKey.Format("Button#%dCommand"/*DNT*/, idxButton);
		RegSetValueEx(hKey, strButtonKey, 0, REG_SZ, (CONST BYTE*)(LPCTSTR)m_command, m_command.GetLength());
	}

	if(!m_pushVar.IsEmpty())
	{
		strButtonKey.Format("Button#%dPushVar"/*DNT*/, idxButton);
		RegSetValueEx(hKey, strButtonKey, 0, REG_SZ, (CONST BYTE*)(LPCTSTR)m_pushVar, m_pushVar.GetLength());
	}

	if(!m_grayVar.IsEmpty())
	{
		strButtonKey.Format("Button#%dGrayVar"/*DNT*/, idxButton);
		RegSetValueEx(hKey, strButtonKey, 0, REG_SZ, (CONST BYTE*)(LPCTSTR)m_grayVar, m_grayVar.GetLength());
	}

	if(!m_smColorBmpPath.IsEmpty())
	{
		strButtonKey.Format("Button#%dBMPSmColorPath"/*DNT*/, idxButton);
		RegSetValueEx(hKey, strButtonKey, 0, REG_SZ, (CONST BYTE*)(LPCTSTR)m_smColorBmpPath, m_smColorBmpPath.GetLength());
	}

	if(!m_lgColorBmpPath.IsEmpty())
	{
		strButtonKey.Format("Button#%dBMPLgColorPath"/*DNT*/, idxButton);
		RegSetValueEx(hKey, strButtonKey, 0, REG_SZ, (CONST BYTE*)(LPCTSTR)m_lgColorBmpPath, m_lgColorBmpPath.GetLength());
	}

	if(!m_smBwBmpPath.IsEmpty())
	{
		strButtonKey.Format("Button#%dBMPSmBWPath"/*DNT*/, idxButton);
		RegSetValueEx(hKey, strButtonKey, 0, REG_SZ, (CONST BYTE*)(LPCTSTR)m_smBwBmpPath, m_smBwBmpPath.GetLength());
	}

	if(!m_lgBwBmpPath.IsEmpty())
	{
		strButtonKey.Format("Button#%dBMPLgBWPath"/*DNT*/, idxButton);
		RegSetValueEx(hKey, strButtonKey, 0, REG_SZ, (CONST BYTE*)(LPCTSTR)m_lgBwBmpPath, m_lgBwBmpPath.GetLength());
	}
}

#ifdef XML_TOOLBAR
static bool addNonEmptyAttribute(XMLNode& node, const char* name, const CString& attr)
{
	return (!attr.IsEmpty()) ? node.addAttribute(name, XMLString(attr)) : true;
}

void
CIcadTBbutton::saveToXML(XMLNode& node, bool bSaveFlyOut)
{
	XMLNode* pNode;

	// Write the IcadButton structure
	//node.addAttribute("BinContent", XMLString().dump((const char*)&m_data4reg, sizeof(m_data4reg)));

	// Bitmap data
	{
		pNode = new XMLNode("Bitmap");
		node.addChild(pNode);

		pNode->addAttribute("IDSmallColor", XMLString().assign("%i", m_BMPIdSmColor));
		pNode->addAttribute("IDLargeColor", XMLString().assign("%i", m_BMPIdLgColor));
		pNode->addAttribute("IDSmallBW", XMLString().assign("%i", m_BMPIdSmBW));
		pNode->addAttribute("IDLargeBW", XMLString().assign("%i", m_BMPIdLgBW));
		addNonEmptyAttribute(*pNode, "PathSmallColor", m_smColorBmpPath);
		addNonEmptyAttribute(*pNode, "PathLargeColor", m_lgColorBmpPath);
		addNonEmptyAttribute(*pNode, "PathSmallBW", m_smBwBmpPath);
		addNonEmptyAttribute(*pNode, "PathLargeBW", m_lgBwBmpPath);
	}

	// Write the strings associated with the IcadButton structure.
	addNonEmptyAttribute(node, "ToolTip", m_toolTip);
	addNonEmptyAttribute(node, "HelpString", m_helpString);
	addNonEmptyAttribute(node, "Command", m_command);
	addNonEmptyAttribute(node, "PushVar", m_pushVar);
	addNonEmptyAttribute(node, "GrayVar", m_grayVar);
	node.addAttribute("VisibilityFlag", XMLString().assign("%li", m_lflgVisibility));
	if(m_idToolBar)
		node.addAttribute("IDToolBar", XMLString().assign("%i", m_idToolBar));
	if(m_bAddSpacerBefore)
		node.addAttribute("AddSpacerBefore", XMLString().assign("%i", m_bAddSpacerBefore));
	if(!m_bChgParentBtnImg)
		node.addAttribute("NChangeParentImage", XMLString().assign("%i", m_bChgParentBtnImg));
	if(m_idxFlyOutImage != -1)
		node.addAttribute("IndexFlyOutImage", XMLString().assign("%i", m_idxFlyOutImage));

	if(bSaveFlyOut && (m_pibFlyOut || m_pibCurFlyOut))
	{
		int iFlyOut, iCur = -1;
		CIcadTBbutton* pFlyOut;

		for(iFlyOut = 0, pFlyOut = m_pibFlyOut; pFlyOut; ++iFlyOut, pFlyOut = pFlyOut->m_pibFlyOut)
		{
			pNode = new XMLNode("flyout");
			node.addChild(pNode);
			if(pFlyOut == m_pibCurFlyOut)
				iCur = iFlyOut;
			pFlyOut->saveToXML(*pNode, false);
			if(pFlyOut->m_pibCurFlyOut && pFlyOut->m_pibCurFlyOut != pFlyOut->m_pibFlyOut)
			{
				XMLNode* pNestNode = new XMLNode("flyout");

				pNode->addChild(pNestNode);
				pFlyOut->m_pibCurFlyOut->saveToXML(*pNestNode, true);// first in chain
			}
		}

		if(iCur >= 0)
			node.addAttribute("indexCurFlyOut", XMLString().assign("%i", iCur));
		else if(m_pibCurFlyOut)
		{
			pNode = new XMLNode("flyout");
			node.addChild(pNode);
			pFlyOut->m_pibCurFlyOut->saveToXML(*pNode, true);// first in chain
		}
	}
}

int
CIcadTBbutton::readFromXML(XMLNode& node, CBitmapLoadHook& bmpLoader)
{
	int result, innerResult;
	XMLNode* pNode;

	//if(node.attribute("BinContent")->pickup((char*)&m_data4reg, sizeof(m_data4reg)) != sizeof(m_data4reg))
	//	return -1;
	m_toolTip = node.attribute("ToolTip")->c_str();
	m_helpString = node.attribute("HelpString")->c_str();
	m_command = node.attribute("Command")->c_str();
	m_pushVar = node.attribute("PushVar")->c_str();
	m_grayVar = node.attribute("GrayVar")->c_str();
	m_idToolBar = node.attribute("IDToolBar")->toInt(0);
	m_bAddSpacerBefore = node.attribute("AddSpacerBefore")->toInt(FALSE);
	m_bChgParentBtnImg = node.attribute("UseImageForParent")->toInt(TRUE);
	m_idxFlyOutImage = node.attribute("IndexFlyOutImage")->toInt(-1);

	// load button bitmap data
	pNode = node.childNode("Bitmap");
	if(!pNode)
		return -1;

	m_smColorBmpPath = pNode->attribute("PathSmallColor")->c_str();
	m_lgColorBmpPath = pNode->attribute("PathLargeColor")->c_str();
	m_smBwBmpPath = pNode->attribute("PathSmallBW")->c_str();
	m_lgBwBmpPath = pNode->attribute("PathLargeBW")->c_str();
	m_BMPIdSmColor = pNode->attribute("IDSmallColor")->toInt();
	m_BMPIdLgColor = pNode->attribute("IDLargeColor")->toInt();
	m_BMPIdSmBW = pNode->attribute("IDSmallBW")->toInt();
	m_BMPIdLgBW = pNode->attribute("IDLargeBW")->toInt();

	m_lflgVisibility = node.attribute("VisibilityFlag")->toInt();

	if(!(m_toolTip.IsEmpty() && m_command.IsEmpty()))
		result = bmpLoader.load(this);
	else
		result = -1;

	CIcadTBbutton** ppCurButton;	
	int iCycle = 0, iCurFlyOut = node.attribute("indexCurFlyOut")->toInt(-1 /* default value */);

	if(iCurFlyOut >= 0 || (m_toolTip.IsEmpty() && m_command.IsEmpty()))
		ppCurButton = &m_pibFlyOut;
	else
		ppCurButton = &m_pibCurFlyOut;

for(XMLNode::childIterator it = node.childBegin(); it != node.childEnd(); ++it, ++iCycle)
	{
		if((*it)->name() != "flyout")
			continue;

		CIcadTBbutton* pFlyOut = new CIcadTBbutton;

		if((innerResult = pFlyOut->readFromXML(**it, bmpLoader)) < 0)
		{
			delete pFlyOut;
			continue;
		}

		*ppCurButton = pFlyOut;
		ppCurButton = &pFlyOut->m_pibFlyOut;

		if(iCycle == iCurFlyOut)
		{
			m_pibCurFlyOut = pFlyOut;
			if(result < 0)
				result = innerResult;
		}
	}

	return result;
}
#endif

BOOL
CIcadTBbutton::readFromReg
(
 HKEY	hKey,
 int	idxButton
)
{
	DWORD	dwRetVal, dwSize;
	int		nStrLen;
	CString	strButtonKey;
	char*	pStr;

	strButtonKey.Format("Button#%d"/*DNT*/, idxButton);
	dwSize = sizeof(m_data4reg);

	if(RegQueryValueEx(hKey, strButtonKey, NULL, &dwRetVal, (LPBYTE)&m_data4reg, &dwSize) != ERROR_SUCCESS)
		return FALSE;

	// Get the tooltip string.
	strButtonKey.Format("Button#%dToolTip"/*DNT*/, idxButton);
	if(RegQueryValueEx(hKey, strButtonKey, NULL, &dwRetVal, NULL, (LPDWORD)&nStrLen) == ERROR_SUCCESS)
	{
		pStr = new char[++nStrLen];
		RegQueryValueEx(hKey, strButtonKey, NULL, &dwRetVal, (LPBYTE)pStr, (LPDWORD)&nStrLen);
		m_toolTip = pStr;
		delete [] pStr;
	}

	// Get the help string.
	strButtonKey.Format("Button#%dHelpString"/*DNT*/, idxButton);
	if(RegQueryValueEx(hKey, strButtonKey, NULL, &dwRetVal, NULL, (LPDWORD)&nStrLen) == ERROR_SUCCESS)
	{
		pStr = new char[++nStrLen];
		RegQueryValueEx(hKey, strButtonKey, NULL, &dwRetVal, (LPBYTE)pStr, (LPDWORD)&nStrLen);
		m_helpString = pStr;
		delete [] pStr;
	}

	// Get the command string.
	strButtonKey.Format("Button#%dCommand"/*DNT*/, idxButton);
	if(RegQueryValueEx(hKey, strButtonKey, NULL, &dwRetVal, NULL, (LPDWORD)&nStrLen) == ERROR_SUCCESS)
	{
		char*	pStr = new char[++nStrLen];
		RegQueryValueEx(hKey, strButtonKey, NULL, &dwRetVal, (LPBYTE)pStr, (LPDWORD)&nStrLen);
		m_command = pStr;
		delete [] pStr;
	}

	// Get the PushVar string.
	strButtonKey.Format("Button#%dPushVar"/*DNT*/, idxButton);
	if(RegQueryValueEx(hKey, strButtonKey, NULL, &dwRetVal, NULL, (LPDWORD)&nStrLen) == ERROR_SUCCESS)
	{
		pStr = new char[++nStrLen];
		RegQueryValueEx(hKey, strButtonKey, NULL, &dwRetVal, (LPBYTE)pStr, (LPDWORD)&nStrLen);
		m_pushVar = pStr;
		delete [] pStr;
	}

	// Get the GrayVar string.
	strButtonKey.Format("Button#%dGrayVar"/*DNT*/, idxButton);
	if(RegQueryValueEx(hKey, strButtonKey, NULL, &dwRetVal, NULL, (LPDWORD)&nStrLen) == ERROR_SUCCESS)
	{
		pStr = new char[++nStrLen];
		RegQueryValueEx(hKey, strButtonKey, NULL, &dwRetVal, (LPBYTE)pStr, (LPDWORD)&nStrLen);
		m_grayVar = pStr;
		delete [] pStr;
	}

	strButtonKey.Format("Button#%dBMPSmColorPath"/*DNT*/, idxButton);
	if(RegQueryValueEx(hKey, strButtonKey, NULL, &dwRetVal, NULL, (LPDWORD)&nStrLen) == ERROR_SUCCESS)
	{
		pStr = new char[++nStrLen];
		RegQueryValueEx(hKey, strButtonKey, NULL, &dwRetVal, (LPBYTE)pStr, (LPDWORD)&nStrLen);
		m_smColorBmpPath = pStr;
		delete [] pStr;
	}

	strButtonKey.Format("Button#%dBMPLgColorPath"/*DNT*/, idxButton);
	if(RegQueryValueEx(hKey, strButtonKey, NULL, &dwRetVal, NULL, (LPDWORD)&nStrLen) == ERROR_SUCCESS)
	{
		pStr = new char[++nStrLen];
		RegQueryValueEx(hKey, strButtonKey, NULL, &dwRetVal, (LPBYTE)pStr, (LPDWORD)&nStrLen);
		m_lgColorBmpPath = pStr;
		delete [] pStr;
	}

	strButtonKey.Format("Button#%dBMPSmBWPath"/*DNT*/, idxButton);
	if(RegQueryValueEx(hKey, strButtonKey, NULL, &dwRetVal, NULL, (LPDWORD)&nStrLen) == ERROR_SUCCESS)
	{
		pStr = new char[++nStrLen];
		RegQueryValueEx(hKey, strButtonKey, NULL, &dwRetVal, (LPBYTE)pStr, (LPDWORD)&nStrLen);
		m_smBwBmpPath = pStr;
		delete [] pStr;
	}

	strButtonKey.Format("Button#%dBMPLgBWPath"/*DNT*/, idxButton);
	if(RegQueryValueEx(hKey, strButtonKey, NULL, &dwRetVal, NULL, (LPDWORD)&nStrLen) == ERROR_SUCCESS)
	{
		pStr = new char[++nStrLen];
		RegQueryValueEx(hKey, strButtonKey, NULL, &dwRetVal, (LPBYTE)pStr, (LPDWORD)&nStrLen);
		m_lgBwBmpPath = pStr;
		delete [] pStr;
	}

	return TRUE;
}
