/* D:\DEV\PRJ\ICAD\ICADCUSSHORTCUTS.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

// IcadCusShortcuts.cpp : implementation file
//

#include "Icad.h"/*DNT*/
#include "IcadApi.h"
#include "IcadCusShortcuts.h"/*DNT*/
#include "IcadToolbar.h"/*DNT*/
#include "IcadMenu.h"/*DNT*/
#include "IcadPrivate.h"
#include "Preferences.h"
#include "IcadApp.h"
#include "LicensedComponents.h"
#include "IntCmds.h"

#ifdef _DEBUG

	#if !defined(USE_SMARTHEAP)
	#define new DEBUG_NEW
	#endif

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern struct SafeAliasList  *SDS_AliasListBeg;
extern int SDS_nInternalCmdList;
struct SDS_AliasList SDS_pAliasCmdList[];
extern int SDS_nAliasCmdList;

/////////////////////////////////////////////////////////////////////////////
// Alias functions from the main

	// search the list by alias for a particular alias
SafeAliasList *
SearchAliasList( SDS_AliasList *key)
	{
	for ( SafeAliasList *next = SDS_AliasListBeg; next; next = next->next )
		{
		if ( next->alias && key->alias && strisame( next->alias, key->alias) )
			return next;
		}
	
	return NULL;
	}


	// search the default Table by alias for a particular alias
SafeAliasList *
SearchAliasTable( SDS_AliasList *key)
	{
	for ( int i=0; i < SDS_nAliasCmdList; ++i )
		{
		SDS_AliasList	*next = SDS_pAliasCmdList + i;
		if ( next->alias && key->alias && strisame( next->alias, key->alias) )
			return (SafeAliasList *)next;
		}
	
	return NULL;
	}


void CMainWindow::LoadAliases()
	{
	PreferenceKey		aliasKey( HKEY_CURRENT_USER, "Aliases", FALSE);
	int nBadRet=0;
								// reset aliases to defaults
	if(SDS_AliasListBeg!=NULL)
		{
		SDS_FreeAlias();
		SDS_AliasListBeg=NULL;
		}

	//*** Load the Aliases from the registry if it exists, if not load the default
	//*** aliases.
	HKEY hKeyAliasRoot = aliasKey;

	if(hKeyAliasRoot == NULL)
		{
		//*** Save the aliases to the registry.
		SDS_DefaultAlias();
		SaveAliasesToReg();
		return;
		}

	//*** Get the number of aliases.
	int nAliases;
	DWORD dwRetVal,dwSize;
	dwSize=sizeof(int);
	CString strKey;
	if(RegQueryValueEx(hKeyAliasRoot,"nAliases"/*DNT*/,NULL,&dwRetVal,(LPBYTE)&nAliases,
		(LPDWORD)&dwSize)!=ERROR_SUCCESS)
		{
		nBadRet=(-__LINE__);
		goto badout;
		}
	//*** Create the Aliases.
	struct SafeAliasList *pAliasList, *pEndList;
	DWORD	nStrLen;
	int		ct;
	HKEY hKeyAlias;
	for(ct=0; ct<nAliases; ct++)
		{
		//*** Get the alias key
		strKey.Format("Alias-%d"/*DNT*/,ct);
		if(RegOpenKeyEx(hKeyAliasRoot,strKey,0,KEY_READ,&hKeyAlias)!=ERROR_SUCCESS) continue;

		//*** Allocate a new alias structure.
		pAliasList = new SafeAliasList;
		
		if(RegQueryValueEx(hKeyAlias,"Alias"/*DNT*/,NULL,&dwRetVal,NULL,(LPDWORD)&nStrLen)==ERROR_SUCCESS)
			{
			pAliasList->alias = new char[++nStrLen];
			memset(pAliasList->alias,0,nStrLen);
			RegQueryValueEx(hKeyAlias,"Alias"/*DNT*/,NULL,&dwRetVal,(LPBYTE)pAliasList->alias,
				(LPDWORD)&nStrLen);
			}
		if(RegQueryValueEx(hKeyAlias,"Command"/*DNT*/,NULL,&dwRetVal,NULL,(LPDWORD)&nStrLen)==ERROR_SUCCESS)
			{
			DWORD	length = ++nStrLen;

			pAliasList->LName = new char[length];
			memset(pAliasList->LName,0,length);
			RegQueryValueEx(hKeyAlias,"Command"/*DNT*/,NULL,&dwRetVal,(LPBYTE)pAliasList->LName,
				&nStrLen);
            }
		if(RegQueryValueEx(hKeyAlias,"GCommand"/*DNT*/,NULL,&dwRetVal,NULL,(LPDWORD)&nStrLen)==ERROR_SUCCESS)
			{
			DWORD	length = ++nStrLen;

			pAliasList->GName = new char[length];
			memset(pAliasList->GName,0,length);
			RegQueryValueEx(hKeyAlias,"GCommand"/*DNT*/,NULL,&dwRetVal,(LPBYTE)pAliasList->GName,
				&nStrLen);
            }
		if(RegQueryValueEx(hKeyAlias,"ShellPrompt"/*DNT*/,NULL,&dwRetVal,NULL,(LPDWORD)&nStrLen)==ERROR_SUCCESS)
			{
			pAliasList->prompt = new char[++nStrLen];
			RegQueryValueEx(hKeyAlias,"ShellPrompt"/*DNT*/,NULL,&dwRetVal,(LPBYTE)pAliasList->prompt,
				(LPDWORD)&nStrLen);
			RegQueryValueEx(hKeyAlias,"RetVal"/*DNT*/,NULL,&dwRetVal,(LPBYTE)&(pAliasList->retval),
				(LPDWORD)&dwSize);
			RegQueryValueEx(hKeyAlias,"CrMode"/*DNT*/,NULL,&dwRetVal,(LPBYTE)&(pAliasList->crmode),
				(LPDWORD)&dwSize);
			}

		if ( pAliasList != NULL )
			{
			SafeAliasList	*currentAlias = SearchAliasList( pAliasList);

			// if alias found, override from registry
			if ( currentAlias )
				{
                // If for some reason pAliasList->LName == NULL, let currentAlias stand
                // as it is found in the table.
                if (pAliasList->LName != NULL)
                    {
				    *currentAlias = *pAliasList;
                    }
				delete pAliasList; 
                pAliasList = NULL;
				}				
			else	// otherwise add to registry
				{
				if(SDS_AliasListBeg==NULL) 
					SDS_AliasListBeg=pEndList=pAliasList;
				else 
					pEndList=pEndList->next=pAliasList;
				}
            } 

		RegCloseKey(hKeyAlias);
		}
//	RegCloseKey(hKeyAliasRoot);
	return;

badout:
	CString str;
	str.Format(ResourceString(IDC_ICADCUSSHORTCUTS_ERROR_C_7, "Error creating aliases (%d). Default aliases will be loaded." ),(-nBadRet));
	MessageBox(str);
	//*** Free any aliases that may have been allocated and load the defaults.
	SDS_FreeAlias();
	SDS_DefaultAlias();
	}

void CMainWindow::SaveAliasesToReg()
{
	PreferenceKey		aliasKey( HKEY_CURRENT_USER, "Aliases", FALSE);
	HKEY hKeyAliasRoot = aliasKey;
	//*** Return if the registry exists but the Modified flag has not been set;
	if(hKeyAliasRoot != NULL)
	{
//		RegCloseKey(hKeyAliasRoot);
		if(!m_bAliasModified) return;
	}
	//*** Get the registry ready to write to.
	aliasKey.FlushKey(HKEY_CURRENT_USER, "Aliases");
	IcadRegDeleteKey(hKeyAliasRoot, "");
	PreferenceKey		AlKey(HKEY_CURRENT_USER, "Aliases");
	hKeyAliasRoot = AlKey;
	DWORD dwRetVal;
	if(hKeyAliasRoot == NULL) return;
	//*** Get the number of aliases
	struct SDS_AliasList* pAliasList;
	int nAliases=0;
	for(pAliasList=SDS_AliasListBeg; pAliasList!=NULL; pAliasList=pAliasList->next) nAliases++;
	//*** Write the number of aliases
	RegSetValueEx(hKeyAliasRoot,"nAliases"/*DNT*/,0,REG_DWORD,(CONST BYTE*)&nAliases,
		sizeof(int));
	//*** Loop through the aliases and write the alias information to the registry.
	HKEY hKeyAlias;
	CString strKey;
	int ct;
	for(ct=0,pAliasList=SDS_AliasListBeg; pAliasList!=NULL && ct<nAliases; pAliasList=pAliasList->next,ct++)
	{
		SDS_AliasList	*defaultAlias = SearchAliasTable( pAliasList );

								// don't add registry if it matches default
		if ( defaultAlias 
				&& pAliasList->alias && strisame( defaultAlias->alias, pAliasList->alias)
				&& pAliasList->GName && strisame( defaultAlias->GName, pAliasList->GName)
				&& pAliasList->prompt && strisame( defaultAlias->prompt, pAliasList->prompt)
				)
			{}; //continue;	// good idea, but not compatible if 98C reinstalled

		//*** Create a key for the alias
		strKey.Format("Alias-%d"/*DNT*/,ct);
		if(RegCreateKeyEx(hKeyAliasRoot,strKey,0,""/*DNT*/,REG_OPTION_NON_VOLATILE,
			KEY_ALL_ACCESS,NULL,&hKeyAlias,&dwRetVal)!=ERROR_SUCCESS) continue;
		//*** Write the alias structure info.
		if(pAliasList->alias!=NULL)
		{
			RegSetValueEx(hKeyAlias,"Alias"/*DNT*/,0,REG_SZ,(CONST BYTE*)pAliasList->alias,
				strlen(pAliasList->alias));
		}
		if(pAliasList->LName!=NULL)
		{
			RegSetValueEx(hKeyAlias,"Command"/*DNT*/,0,REG_SZ,(CONST BYTE*)pAliasList->LName,
				strlen(pAliasList->LName));
        }
		if (pAliasList->GName != NULL) 
		{
			RegSetValueEx(hKeyAlias,"GCommand"/*DNT*/,0,REG_SZ,(CONST BYTE*)pAliasList->GName,
				strlen(pAliasList->GName));
        }


		if(pAliasList->prompt!=NULL)
		{
			RegSetValueEx(hKeyAlias,"ShellPrompt"/*DNT*/,0,REG_SZ,(CONST BYTE*)pAliasList->prompt,
				strlen(pAliasList->prompt));
			RegSetValueEx(hKeyAlias,"RetVal"/*DNT*/,0,REG_DWORD,(CONST BYTE*)&(pAliasList->retval),
				sizeof(int));
			RegSetValueEx(hKeyAlias,"CrMode"/*DNT*/,0,REG_DWORD,(CONST BYTE*)&(pAliasList->crmode),
				sizeof(int));
		}
		RegCloseKey(hKeyAlias);
	}
	//*** Close the registry.
//	RegCloseKey(hKeyAliasRoot);
	m_bAliasModified=FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CIcadCusShortcuts property page

IMPLEMENT_DYNCREATE(CIcadCusShortcuts, CPropertyPage)

CIcadCusShortcuts::CIcadCusShortcuts() : CPropertyPage(CIcadCusShortcuts::IDD)
{
	//{{AFX_DATA_INIT(CIcadCusShortcuts)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_idxCurAlias=(-1);
}

CIcadCusShortcuts::~CIcadCusShortcuts()
{
}

void CIcadCusShortcuts::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIcadCusShortcuts)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CIcadCusShortcuts, CPropertyPage)
	//{{AFX_MSG_MAP(CIcadCusShortcuts)
	ON_LBN_SELCHANGE(CUS_ALIAS_LISTALIASES, OnSelChangeAliasesList)
	ON_BN_CLICKED(CUS_ALIAS_BTNNEW, OnNewButton)
	ON_BN_CLICKED(CUS_SHORTCUTS_BTNASSIGN, OnAssignButton)
	ON_BN_CLICKED(CUS_ALIAS_BTNDELETE, OnDeleteButton)
	ON_LBN_DBLCLK(CUS_ALIAS_LISTCMD, OnAssignButton)
	ON_EN_KILLFOCUS(CUS_ALIAS_EDITSHORTCUT, OnKillFocusAliasEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIcadCusShortcuts message handlers

BOOL CIcadCusShortcuts::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	//*** Fill the command listbox.
	CListBox
        * pCmdList = (CListBox*)GetDlgItem(CUS_ALIAS_LISTCMD);
	int idxNew,ct;
	char* pstr;
	for(ct=0; ct<SDS_nInternalCmdList; ct++)
	{
		if(SDS_pInternalCmdList[ct].cmdFlags & 1) continue;
		pstr=(char*)LPCSTR(SDS_pInternalCmdList[ct].localName);

		if (NULL != pstr)
		{
			/*D.G.*/// Check for the accessibility of the licensed components commands.
			if(!CIcadApp::m_LicensedComponents.IsCommandAccessible(pstr))
				continue;

			idxNew=pCmdList->AddString(pstr);
			pCmdList->SetItemDataPtr(idxNew,&SDS_pInternalCmdList[ct]);
		}
	}
	//*** Fill the alias listbox.
	ResetAliasList();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CIcadCusShortcuts::OnSelChangeAliasesList() 
{
	CListBox* pAliasListBox = (CListBox*)GetDlgItem(CUS_ALIAS_LISTALIASES);
	//*** Save any changes to the alias string.
	if(!SaveAliasString())
	{
		return;
	}
	//*** Enable the Delete and Assign buttons.
	GetDlgItem(CUS_ALIAS_BTNDELETE)->EnableWindow();
	GetDlgItem(CUS_SHORTCUTS_BTNASSIGN)->EnableWindow();
	//*** Enable and set the Alias editbox.
	CEdit* pAliasEdit = (CEdit*)GetDlgItem(CUS_ALIAS_EDITSHORTCUT);
	pAliasEdit->EnableWindow();
	GetDlgItem(CUS_ALIAS_TXTSHORTCUT)->EnableWindow();
	m_idxCurAlias = pAliasListBox->GetCurSel();
	CString strText;
	pAliasListBox->GetText(m_idxCurAlias,strText);
	pAliasEdit->SetWindowText(strText);
	//*** Set the Alias's command in the Command editbox.
    struct SDS_AliasList* Atmp = (struct SDS_AliasList*)pAliasListBox->GetItemDataPtr(m_idxCurAlias);
	CEdit* pCmdEdit = (CEdit*)GetDlgItem(CUS_SHORTCUTS_EDITCMD);
	pCmdEdit->SetWindowText(Atmp->LName);
	//*** Highlight the command in the Available commands listbox.
	CListBox* pCmdList = (CListBox*)GetDlgItem(CUS_ALIAS_LISTCMD);
	pCmdList->SelectString(-1,Atmp->LName);
}

BOOL CIcadCusShortcuts::SaveAliasString() 
{
	if(m_hWnd==NULL)
	{
		return TRUE;
	}
	//*** Return if there hasn't been a command assigned to this alias.
	CString strCmd;
	GetDlgItem(CUS_SHORTCUTS_EDITCMD)->GetWindowText(strCmd);
	if(strCmd.IsEmpty())
	{
		return TRUE;
	}
	//*** Get the string from the Alias editbox.
	CString strAlias;
	CEdit* pAliasEdit = (CEdit*)GetDlgItem(CUS_ALIAS_EDITSHORTCUT);
	pAliasEdit->GetWindowText(strAlias);
	if(strAlias.IsEmpty())
	{
		return TRUE;
	}
	strAlias.TrimLeft();
	strAlias.TrimRight();
	strAlias.MakeUpper();
	//*** Check for space characters.
	if(strAlias.Find(' ')!=(-1))
	{
		CString str;
		str.Format(ResourceString(IDC_ICADCUSSHORTCUTS__CANNO_10, "The alias \"%s\" cannot contain spaces.  Please replace all\nspaces with underscores." ),strAlias);
		MessageBox(str,NULL,MB_OK | MB_ICONERROR);
		pAliasEdit->SetFocus();
		return FALSE;
	}
	//*** Check for non-alpha numeric characters
	int ct=0;
	int nLen = strAlias.GetLength();
	for(ct=0; ct<nLen; ct++)
	{
		if(!__iscsym(strAlias.GetAt(ct)) && strAlias.GetAt(ct)!='-'/*DNT*/)
		{
			CString str;
			str.Format(ResourceString(IDC_ICADCUSSHORTCUTS_A_Z__0_13, "The alias \"%s\" contains invalid characters.  Only the characters\nA-Z, 0-9, underscores ( _ ), and dashes ( - ) are valid." ),strAlias);
			MessageBox(str,NULL,MB_OK | MB_ICONERROR);
			pAliasEdit->SetFocus();
			pAliasEdit->SetSel(0,-1);
			return FALSE;
		}
	}
	//*** Check to see if the alias already exists.
	CListBox* pAliasListBox = (CListBox*)GetDlgItem(CUS_ALIAS_LISTALIASES);
	BOOL bAliasChanged = TRUE;
	BOOL bCmdChanged = TRUE;
	int idxStr;
	if((idxStr=pAliasListBox->FindStringExact(-1,strAlias))!=(-1))
	{
		if(pAliasListBox->GetCurSel()==LB_ERR)
		{
			CString str;
			str.Format(ResourceString(IDC_ICADCUSSHORTCUTS__ALREA_14, "The alias \"%s\" already exists in the alias list.  Do you wish\nto overwrite it?" ),strAlias);
			if(MessageBox(str,NULL,MB_YESNO | MB_ICONQUESTION)==IDNO)
			{
				//*** Set the command editbox to the existing command
				struct SDS_AliasList* pCurAlias = (struct SDS_AliasList*)pAliasListBox->GetItemDataPtr(idxStr);
				GetDlgItem(CUS_SHORTCUTS_EDITCMD)->SetWindowText(pCurAlias->LName);
				//*** Highlight the command in the Available commands listbox.
				((CListBox*)GetDlgItem(CUS_ALIAS_LISTCMD))->SelectString(-1,pCurAlias->LName);
				return TRUE;
			}
		}
		m_idxCurAlias=idxStr;
		bAliasChanged=FALSE;
	}
	//*** Get the pointer to the alias structure.
	struct SafeAliasList* pCurAlias;
	if(m_idxCurAlias==(-1))
	{
		//*** Add a new link.
		struct SDS_AliasList* Atmp;
		for(Atmp=SDS_AliasListBeg; Atmp!=NULL && Atmp->next!=NULL; Atmp=Atmp->next);
		pCurAlias = new SafeAliasList;
		if(Atmp==NULL) SDS_AliasListBeg=pCurAlias;
		else Atmp->next=pCurAlias;
	}
    else
	{
		pCurAlias = (SafeAliasList*)pAliasListBox->GetItemDataPtr(m_idxCurAlias);
	}
	if(pCurAlias->LName!=NULL && strCmd.Compare(pCurAlias->LName)==0)
	{
		bCmdChanged=FALSE;
	}
	//*** Return if the alias string has not changed and the command string has not
	//*** changed.
	if(!bAliasChanged && !bCmdChanged)
	{
		return TRUE;
	}
	//*** Free the old alias string and set the new value.
	if(bAliasChanged)
	{
		if(pCurAlias->alias!=NULL) delete pCurAlias->alias;
		pCurAlias->alias = new char[strAlias.GetLength()+1];
		strcpy(pCurAlias->alias,strAlias);
	}
	//*** Free the old command string and set the new value.
	if(bCmdChanged)
	{
		if(pCurAlias->LName!=NULL) delete pCurAlias->LName;
		pCurAlias->LName = new char[strCmd.GetLength()+1];
		strcpy(pCurAlias->LName,strCmd);

		for ( SafeAliasList *next = SDS_AliasListBeg; next ; next = next->next )
		    {
		    if (next == pCurAlias) continue;
		    if (next->LName != NULL && strisame(next->LName, strCmd) )
			    break;
		    }
				
		if(pCurAlias->GName!=NULL) delete pCurAlias->GName;
		
		if (next == NULL || next->GName == NULL || *(next->GName) == 0)
			pCurAlias->GName=NULL;
		else
			{
			pCurAlias->GName = new char[strlen(next->GName)+1];
			strcpy(pCurAlias->GName,next->GName);
			}

	}
	//*** Update the alias listbox string.
	if(bAliasChanged)
	{
		if(m_idxCurAlias!=(-1)) pAliasListBox->DeleteString(m_idxCurAlias);
		int idxCurItem=pAliasListBox->AddString(pCurAlias->alias);
		pAliasListBox->SetItemDataPtr(idxCurItem,pCurAlias);
	}
	SDS_CMainWindow->m_bAliasModified=TRUE;
	return TRUE;
}

void CIcadCusShortcuts::OnNewButton() 
{
	//*** Save any changes in the alias's editbox.
	if(!SaveAliasString())
	{
		return;
	}
	//*** Enable the Assign button.
	GetDlgItem(CUS_SHORTCUTS_BTNASSIGN)->EnableWindow();
	//*** Enable, clear, and set focus to the Alias editbox.
	CEdit* pAliasEdit = (CEdit*)GetDlgItem(CUS_ALIAS_EDITSHORTCUT);
	pAliasEdit->EnableWindow();
	GetDlgItem(CUS_ALIAS_TXTSHORTCUT)->EnableWindow();
	pAliasEdit->SetWindowText(""/*DNT*/);
	pAliasEdit->SetFocus();
	//*** Clear any text in the command editbox.
	GetDlgItem(CUS_SHORTCUTS_EDITCMD)->SetWindowText(""/*DNT*/);
	//*** Clear the current selection in the listboxes.
	((CListBox*)GetDlgItem(CUS_ALIAS_LISTALIASES))->SetCurSel(-1);
	m_idxCurAlias=(-1);
	//*** Disable the Delete button.
	GetDlgItem(CUS_ALIAS_BTNDELETE)->EnableWindow(FALSE);
}

void CIcadCusShortcuts::OnAssignButton() 
{
	//*** Return if there isn't an alias in the alias editbox.
	CString strAlias;
	GetDlgItem(CUS_ALIAS_EDITSHORTCUT)->GetWindowText(strAlias);
	if(strAlias.IsEmpty())
	{
		return;
	}
	//*** Set the command string in the Command editbox.
	CListBox* pCmdList = (CListBox*)GetDlgItem(CUS_ALIAS_LISTCMD);
	CString strText;
	int result;
	// Before this check, icad would crash in debug versions on the following
	// GetText call. This way it just sits there waiting on a command to be
	// selected.
	if ((result = pCmdList->GetCurSel()) == LB_ERR)
		return;
	pCmdList->GetText(result,strText);
	CEdit* pEdit = (CEdit*)GetDlgItem(CUS_SHORTCUTS_EDITCMD);
	pEdit->SetWindowText(strText);
	//*** Add the alias to the alias list and highlight it.
	if(!SaveAliasString())
	{
		pEdit->SetWindowText(""/*DNT*/);
		return;
	}
	((CListBox*)GetDlgItem(CUS_ALIAS_LISTALIASES))->SelectString(-1,strAlias);
}

void CIcadCusShortcuts::OnDeleteButton() 
{
	//*** Delete the item from the Alias linked list.
	CListBox* pAliasListBox = (CListBox*)GetDlgItem(CUS_ALIAS_LISTALIASES);
	struct SDS_AliasList *pCurAlias,*paTmp,*paPrev;
	pCurAlias = (struct SDS_AliasList*)pAliasListBox->GetItemDataPtr(m_idxCurAlias);
	for(paPrev=paTmp=SDS_AliasListBeg; paTmp!=NULL && paTmp!=pCurAlias; paTmp=paTmp->next)
	{
		paPrev=paTmp;
	}
	if(paTmp==NULL) return;
	if(paTmp==SDS_AliasListBeg) SDS_AliasListBeg=paTmp->next;
	else paPrev->next=paTmp->next;
	paTmp->next=NULL;
	if(paTmp->alias!=NULL) delete paTmp->alias;
	if(paTmp->GName!=NULL) delete paTmp->GName;
	if(paTmp->prompt!=NULL) delete paTmp->prompt;
	delete paTmp;
	//*** Delete the item from the Alias listbox.
	pAliasListBox->DeleteString(m_idxCurAlias);
	m_idxCurAlias=(-1);
	//*** Disable the Delete and Assign button.
	GetDlgItem(CUS_ALIAS_BTNDELETE)->EnableWindow(FALSE);
	GetDlgItem(CUS_SHORTCUTS_BTNASSIGN)->EnableWindow(FALSE);
	//*** Disable and clear the Alias editbox.
	CEdit* pAliasEdit = (CEdit*)GetDlgItem(CUS_ALIAS_EDITSHORTCUT);
	pAliasEdit->EnableWindow(FALSE);
	GetDlgItem(CUS_ALIAS_TXTSHORTCUT)->EnableWindow(FALSE);
	pAliasEdit->SetWindowText(""/*DNT*/);
	//*** Clear any text in the command editbox.
	GetDlgItem(CUS_SHORTCUTS_EDITCMD)->SetWindowText(""/*DNT*/);
	SDS_CMainWindow->m_bAliasModified=TRUE;
}

BOOL CIcadCusShortcuts::OnExit()
{
	//*** Save any changes in the alias's editbox.
	return SaveAliasString();
}

void CIcadCusShortcuts::OnKillFocusAliasEdit() 
{
	
}

void CIcadCusShortcuts::ResetAliasList() 
{
	//*** Fill the alias listbox.
	CListBox* pAliasListBox = (CListBox*)GetDlgItem(CUS_ALIAS_LISTALIASES);
	pAliasListBox->ResetContent();
    struct SDS_AliasList *Atmp;
	int idxNew;
    for(Atmp=SDS_AliasListBeg; Atmp!=NULL; Atmp=Atmp->next) {
		if(Atmp->prompt!=NULL || Atmp->alias==NULL) continue;

		/*D.G.*/// Check for the accessibility of the licensed components commands.
		if(!CIcadApp::m_LicensedComponents.IsCommandAccessible(Atmp->LName))
			continue;

		idxNew=pAliasListBox->AddString(Atmp->alias);
		pAliasListBox->SetItemDataPtr(idxNew,Atmp);
	}
	//*** Disable the Delete and Assign buttons.
	GetDlgItem(CUS_ALIAS_BTNDELETE)->EnableWindow(FALSE);
	GetDlgItem(CUS_SHORTCUTS_BTNASSIGN)->EnableWindow(FALSE);
	//*** Disable and clear the Alias editbox.
	CEdit* pAliasEdit = (CEdit*)GetDlgItem(CUS_ALIAS_EDITSHORTCUT);
	pAliasEdit->EnableWindow(FALSE);
	GetDlgItem(CUS_ALIAS_TXTSHORTCUT)->EnableWindow(FALSE);
	pAliasEdit->SetWindowText(""/*DNT*/);
	//*** Clear any text in the command editbox.
	GetDlgItem(CUS_SHORTCUTS_EDITCMD)->SetWindowText(ResourceString(IDC_ICADCUSSHORTCUTS__8, "" ));
	SDS_CMainWindow->m_bAliasModified=TRUE;
}

BOOL CIcadCusShortcuts::SaveAliasesToFile(LPCTSTR pszFileName)
{
	if(pszFileName==NULL) return FALSE;
	//*** Make a local copy of the file name.
	char* pszFile = new char[lstrlen(pszFileName)+5]; //*** +5 in case it doesn't include an extension.
	lstrcpy(pszFile,pszFileName);
	//*** Make sure the file name has the correct extension.
	if(strstr(pszFile,".ica"/*DNT*/)==NULL)
	{
		char* pc1;
		if((pc1=strrchr(pszFile,'.'))!=NULL) *pc1=0;
		ic_setext(pszFile,".ica"/*DNT*/);
	}
	CStdioFile fileSave;
	CFileException fileException;
	CString str;
	if(!fileSave.Open(pszFile,CFile::modeCreate | CFile::modeWrite,&fileException))
	{
		delete [] pszFile;
		return FALSE;
	}
	fileSave.WriteString("[IntelliCAD Custom Alias File]\n"/*DNT*/);
	//*** Get the number of aliases
	struct SDS_AliasList* pAliasList;
	int nAliases=0;
	for(pAliasList=SDS_AliasListBeg; pAliasList!=NULL; pAliasList=pAliasList->next) nAliases++;
	//*** Write the number of aliases
	str.Format("\tnAliases=%d\n"/*DNT*/,nAliases);
	fileSave.WriteString(str);
	//*** Loop through the aliases and write the alias information to the registry.
	int ct;
	for(ct=0,pAliasList=SDS_AliasListBeg; pAliasList!=NULL && ct<nAliases; pAliasList=pAliasList->next,ct++)
	{
		str.Format("[Alias-%d]\n"/*DNT*/,ct);
		fileSave.WriteString(str);
		//*** Write the alias structure info.
		if(pAliasList->alias!=NULL)
		{
			str.Format("\tAlias=%s\n"/*DNT*/,pAliasList->alias);
			fileSave.WriteString(str);
		}
		if(pAliasList->LName!=NULL)
		{
			str.Format("\tLocalCommand=%s\n"/*DNT*/,pAliasList->LName);
			fileSave.WriteString(str);
		}
		if(pAliasList->GName!=NULL)
		{
			str.Format("\tGlobalCommand=%s\n"/*DNT*/,pAliasList->GName);
			fileSave.WriteString(str);
		}
		if(pAliasList->prompt!=NULL)
		{
			str.Format("\tShellPrompt=%s\n"/*DNT*/,pAliasList->prompt);
			fileSave.WriteString(str);
			str.Format("\tRetVal=%d\n"/*DNT*/,pAliasList->retval);
			fileSave.WriteString(str);
			str.Format("\tCrMode=%d\n"/*DNT*/,pAliasList->crmode);
			fileSave.WriteString(str);
		}
	}
	//*** Close the file.
	fileSave.Close();
	delete [] pszFile;
	return TRUE;
}

BOOL CIcadCusShortcuts::ReadAliasFile(LPCTSTR pszFileName, BOOL bAppend)
{
	BOOL bRet=TRUE;
	if(pszFileName==NULL) return FALSE;
	//*** Make a local copy of the file name.
	char* pszFile = new char[lstrlen(pszFileName)+1];
	lstrcpy(pszFile,pszFileName);
	//*** Make sure the file name has the correct extension.
	if(strstr(pszFile,".ica"/*DNT*/)==NULL)
	{
		delete [] pszFile;
		return FALSE;
	}
	CStdioFile fileSave;
	CFileException fileException;
	if(!fileSave.Open(pszFile,CFile::modeRead,&fileException))
	{
		delete [] pszFile;
		return FALSE;
	}
    struct SafeAliasList *palCur=NULL,*palTmp;
	int ct=0,nAliases;
	char* pszTmp=NULL;
	CString str;
	fileSave.ReadString(str);
	if(str.Compare("[IntelliCAD Custom Alias File]"/*DNT*/)!=0) { bRet=FALSE; goto out; }
	//*** Get the number of aliases
	fileSave.ReadString(str);
	str.TrimLeft();
	pszTmp = new char[str.GetLength()+1];
	lstrcpy(pszTmp,str);
	if(!strnsame("nAliases="/*DNT*/,pszTmp,9)) { bRet=FALSE; goto out; }
	char* pcTmp1;
	pcTmp1=(strrchr(pszTmp,'='/*DNT*/)+1);
	nAliases=atoi(pcTmp1);
	if(nAliases<=0) { bRet=FALSE; goto out; }
	delete [] pszTmp;
	pszTmp=NULL;
	//*** Free the list if we are not appending, otherwise set palCur to the end.
	if(!bAppend) SDS_FreeAlias();
	else
	{
		for(palCur=SDS_AliasListBeg; palCur!=NULL && palCur->next!=NULL; palCur=palCur->next);
	}
	//*** Loop through the aliases and write the alias information to the registry.
	while(fileSave.ReadString(str) && ct<=nAliases)
	{
		if(pszTmp!=NULL) delete pszTmp;
		str.TrimLeft();
		pszTmp = new char[str.GetLength()+1];
		lstrcpy(pszTmp,str);
		if(strnsame("[Alias-"/*DNT*/,pszTmp,7))
		{
			//*** New alias.
			palTmp = new SafeAliasList;
			if(SDS_AliasListBeg==NULL) SDS_AliasListBeg=palCur=palTmp;
			else palCur=palCur->next=palTmp;
			ct++;
			continue;
		}
		else if(strnsame("Alias="/*DNT*/,pszTmp,6))
		{
			if(palCur==NULL || palCur->alias!=NULL) continue;
			pcTmp1=(strrchr(pszTmp,'=')+1);
			if(*pcTmp1==0) continue;
			palCur->alias = new char[strlen(pcTmp1)+1];
			strcpy(palCur->alias,pcTmp1);
		}
		else if(strnsame("LocalCommand="/*DNT*/,pszTmp,13))
		{ 
			if(palCur==NULL || palCur->LName!=NULL) continue;
			pcTmp1=(strrchr(pszTmp,'=')+1);
			if(*pcTmp1==0) continue;
			palCur->LName = new char[strlen(pcTmp1)+1];
			strcpy(palCur->LName,pcTmp1);
		}
		else if(strnsame("GlobalCommand="/*DNT*/,pszTmp,14))
		{ 
			if(palCur==NULL || palCur->GName!=NULL) continue;
			pcTmp1=(strrchr(pszTmp,'=')+1);
			if(*pcTmp1==0) continue;
			palCur->GName = new char[strlen(pcTmp1)+1];
			strcpy(palCur->GName,pcTmp1);
		}
		else if(strnsame("ShellPrompt="/*DNT*/,pszTmp,12))
		{
			if(palCur==NULL || palCur->prompt!=NULL) continue;
			pcTmp1=(strrchr(pszTmp,'=')+1);
			if(*pcTmp1==0) continue;
			palCur->prompt = new char[strlen(pcTmp1)+1];
			strcpy(palCur->prompt,pcTmp1);
		}
		else if(strnsame("RetVal="/*DNT*/,pszTmp,7))
		{
			if(palCur==NULL || palCur->retval!=0) continue;
			pcTmp1=(strrchr(pszTmp,'=')+1);
			if(*pcTmp1==0) continue;
			palCur->retval = atoi(pcTmp1);
		}
		else if(strnsame("CrMode="/*DNT*/,pszTmp,7))
		{
			if(palCur==NULL || palCur->crmode!=0) continue;
			pcTmp1=(strrchr(pszTmp,'='/*DNT*/)+1);
			if(*pcTmp1==0) continue;
			palCur->crmode = atoi(pcTmp1);
		}
	}

out:

	//*** Cleanup and bail.
	fileSave.Close();
	if(pszTmp!=NULL) delete [] pszTmp;
	delete [] pszFile;
	return bRet;
}


