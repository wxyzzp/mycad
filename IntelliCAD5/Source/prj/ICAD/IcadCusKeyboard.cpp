/* D:\DEV\PRJ\ICAD\ICADCUSKEYBOARD.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

// IcadCusKeyboard.cpp : implementation file
//

#include "Icad.h"/*DNT*/
#include "IcadApi.h"
#include "IcadCusKeyboard.h"/*DNT*/
#include "IcadToolBarMain.h"/*DNT*/
#include "IcadMenu.h"/*DNT*/
#include "Preferences.h" /*DNT*/
#include "IcadApp.h"/*DNT*/
#include "LicensedComponents.h"

#ifdef _DEBUG

	#if !defined(USE_SMARTHEAP)
	#define new DEBUG_NEW
	#endif

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern struct IcadCommands g_MnuTbItems[];		// Global command structure
extern int g_nMnuTbItems;						// Global value for number of commands
extern ICACCEL g_IcadAccelKeys[];				//*** Global array of accelerator definitions.
extern int g_nIcadAccelKeys;					//*** Global value for number of accelerators.


/////////////////////////////////////////////////////////////////////////////
// Accelerator functions from the main



void CMainWindow::LoadAcceleratorTable(void)
{
	//*** Load the accelerator list.
	if(!LoadAccelListFromReg())
		{
		LoadDefaultAccelList();
		SaveAcceleratorsToReg();
		}

	//*** Allocate and setup the accelerator array.
	int nAccelKeys = m_pAccelList->GetCount();
	if(nAccelKeys<=0) return;
	LPACCEL lpAccel = new ACCEL[nAccelKeys];
	POSITION pos = m_pAccelList->GetHeadPosition();
	ICACCEL* picAccel;
	int ct;
	for(ct=0; pos!=NULL && ct<nAccelKeys; ct++)
	{
		picAccel = (ICACCEL*)m_pAccelList->GetNext(pos);
		lpAccel[ct].fVirt=picAccel->accel.fVirt;
		lpAccel[ct].key=picAccel->accel.key;
		lpAccel[ct].cmd=ICAD_ACCELID_START+ct;
	}
	//*** Create the accelerator table.
	m_hAccel=CreateAcceleratorTable(lpAccel,nAccelKeys);
	delete [] lpAccel;
}

BOOL CMainWindow::LoadAccelListFromReg(void)
{
	PreferenceKey		acceleratorKey( HKEY_CURRENT_USER, "Accelerators", FALSE);
	//*** If the accelerator list is not empty, empty it.
	DeleteAccelList();
	//*** Load the accelerators from the registry if they are there, else return FALSE.
	int nBadRet=0;
	HKEY hKeyAccelRoot = acceleratorKey;
	if (hKeyAccelRoot == NULL)
		return FALSE;
	//*** Get the number of accelerators.
	int nAccelKeys;
	DWORD dwRetVal,dwSize;
	dwSize=sizeof(int);
	CString strKey;
	if(RegQueryValueEx(hKeyAccelRoot,"nAccelKeys"/*DNT*/,NULL,&dwRetVal,(LPBYTE)&nAccelKeys,
		(LPDWORD)&dwSize)!=ERROR_SUCCESS)
	{
		nBadRet=(-__LINE__);
		goto badout;
	}
	//*** Create the accelerator list.
	int nStrLen,ct;
	HKEY hKeyAccel;
	ACCEL accel;
	dwSize=sizeof(ACCEL);
	ICACCEL* picAccel;
	for(ct=0; ct<nAccelKeys; ct++)
	{
		//*** Get the alias key
		
		strKey.Format("AccelKey-%d"/*DNT*/,ct);
		if(RegOpenKeyEx(hKeyAccelRoot,strKey,0,KEY_READ,&hKeyAccel)!=ERROR_SUCCESS) continue;
		//*** Allocate a new alias structure.
		picAccel = new ICACCEL;
		memset(picAccel,0,sizeof(ICACCEL));
		if(RegQueryValueEx(hKeyAccel,"Command"/*DNT*/,NULL,&dwRetVal,NULL,(LPDWORD)&nStrLen)==ERROR_SUCCESS)
		{
			picAccel->pstrCommand = new char[++nStrLen];
			RegQueryValueEx(hKeyAccel,"Command"/*DNT*/,NULL,&dwRetVal,(LPBYTE)picAccel->pstrCommand,
				(LPDWORD)&nStrLen);
		}
		RegQueryValueEx(hKeyAccel,"accel"/*DNT*/,NULL,&dwRetVal,(LPBYTE)&accel,
			(LPDWORD)&dwSize);
		picAccel->accel.fVirt=accel.fVirt;
		picAccel->accel.key=accel.key;
		picAccel->accel.cmd=accel.cmd;
		m_pAccelList->AddTail((void*)picAccel);
		RegCloseKey(hKeyAccel);
	}
//	RegCloseKey(hKeyAccelRoot);
	return TRUE;

badout:
	CString str;
	str.Format(ResourceString(IDC_ICADCUSKEYBOARD_ERROR_CR_4, "Error creating accelerator table (%d). Default accelerators will be loaded." ),(-nBadRet));
	MessageBox(str);
	//*** Free any accelerators that may have been allocated and return.
	DeleteAccelList();
	return FALSE;
}

void CMainWindow::LoadDefaultAccelList(void)
{
	//*** If the accelerator list is not empty, empty it.
	DeleteAccelList();
	//*** Create the accelerator list with the default values.
	ICACCEL* picAccel;
	int ct;
	for(ct=0; ct<g_nIcadAccelKeys; ct++)
	{
		picAccel = new ICACCEL;
		memset(picAccel,0,sizeof(ICACCEL));
		if(g_IcadAccelKeys[ct].pstrCommand!=NULL && *g_IcadAccelKeys[ct].pstrCommand)
		{
			//*** Allocate the command string.
			picAccel->pstrCommand = new char[strlen(g_IcadAccelKeys[ct].pstrCommand)+1];
			strcpy(picAccel->pstrCommand,g_IcadAccelKeys[ct].pstrCommand);
		}
		picAccel->accel.fVirt=g_IcadAccelKeys[ct].accel.fVirt;
		picAccel->accel.key=g_IcadAccelKeys[ct].accel.key;
		picAccel->accel.cmd=ICAD_ACCELID_START+ct;
		m_pAccelList->AddTail((void*)picAccel);
	}
}

void CMainWindow::SaveAcceleratorsToReg(void)
{
	PreferenceKey		acceleratorKey( HKEY_CURRENT_USER, "Accelerators", FALSE);
	HKEY hKeyAccelRoot = acceleratorKey;
	//*** If the registry exists but the Modified flag has not been set, return;
	if(hKeyAccelRoot != NULL)
	{
//		RegCloseKey(hKeyAccelRoot);
		if(!m_bKeyboardModified) return;
	}
	//*** Get the registry ready to write to.
	acceleratorKey.FlushKey(HKEY_CURRENT_USER, "Accelerators");
	IcadRegDeleteKey(hKeyAccelRoot, "");
	PreferenceKey		AccelKey(HKEY_CURRENT_USER, "Accelerators");
	hKeyAccelRoot = AccelKey;
	DWORD dwRetVal;
	if(hKeyAccelRoot == NULL) return;
	//*** Write the number of accelerators.
	int nAccelKeys=m_pAccelList->GetCount();
	RegSetValueEx(hKeyAccelRoot,"nAccelKeys"/*DNT*/,0,REG_DWORD,(CONST BYTE*)&nAccelKeys,
		sizeof(int));
	//*** Loop through the accelerators and write the accelerator information to the
	//*** registry.
	POSITION pos = m_pAccelList->GetHeadPosition();
	ICACCEL* picAccel;
	HKEY hKeyAccel;
	CString strKey;
	int ct;
	for(ct=0; pos!=NULL && ct<nAccelKeys; ct++)
	{
		//*** Create a key for the accelerator
		strKey.Format("AccelKey-%d"/*DNT*/,ct);
		if(RegCreateKeyEx(hKeyAccelRoot,strKey,0,""/*DNT*/,REG_OPTION_NON_VOLATILE,
			KEY_ALL_ACCESS,NULL,&hKeyAccel,&dwRetVal)!=ERROR_SUCCESS) continue;
		//*** Write the accelerator structure info.
		picAccel = (ICACCEL*)m_pAccelList->GetNext(pos);
		if(picAccel->pstrCommand!=NULL)
		{
			RegSetValueEx(hKeyAccel,"Command"/*DNT*/,0,REG_SZ,(CONST BYTE*)picAccel->pstrCommand,
				strlen(picAccel->pstrCommand));
		}
		RegSetValueEx(hKeyAccel,"accel"/*DNT*/,0,REG_BINARY,(CONST BYTE*)&(picAccel->accel),
			sizeof(ACCEL));
		RegCloseKey(hKeyAccel);
	}
	//*** Close the registry.
//	RegCloseKey(hKeyAccelRoot);
	m_bKeyboardModified=FALSE;
}

void CMainWindow::DeleteAccelList(void)
{
	if(m_pAccelList->IsEmpty()) return;
	ICACCEL* picAccel;
	POSITION pos = m_pAccelList->GetHeadPosition();
	while(pos!=NULL)
	{
		picAccel = (ICACCEL*)m_pAccelList->GetNext(pos);
		if(picAccel->pstrCommand!=NULL) delete picAccel->pstrCommand;
		delete picAccel;
	}
	m_pAccelList->RemoveAll();
}

void CMainWindow::OnAccelKey(UINT nID)
{
	int idxAccelArray = nID-ICAD_ACCELID_START;
	ICACCEL* picAccel = (ICACCEL*)m_pAccelList->GetAt(m_pAccelList->FindIndex(idxAccelArray));
	if(picAccel==NULL || picAccel->pstrCommand==NULL) return;
	SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)picAccel->pstrCommand);
}

/////////////////////////////////////////////////////////////////////////////
// CIcadCusKeyboard property page

IMPLEMENT_DYNCREATE(CIcadCusKeyboard, CPropertyPage)

CIcadCusKeyboard::CIcadCusKeyboard() : CPropertyPage(CIcadCusKeyboard::IDD)
{
	//{{AFX_DATA_INIT(CIcadCusKeyboard)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pCurAccel=NULL;
}

CIcadCusKeyboard::~CIcadCusKeyboard()
{
}

void CIcadCusKeyboard::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIcadCusKeyboard)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CIcadCusKeyboard, CPropertyPage)
	//{{AFX_MSG_MAP(CIcadCusKeyboard)
	ON_LBN_SELCHANGE(CUS_KEYBOARD_LISTDEFKEYS, OnSelChangeDefKeysList)
	ON_BN_CLICKED(CUS_KEYBOARD_BTNADD, OnAddButton)
	ON_BN_CLICKED(CUS_KEYBOARD_BTNDELETE, OnDeleteButton)
	ON_BN_CLICKED(CUS_KEYBOARD_BTNADDCMD, OnAddCmdButton)
	ON_LBN_DBLCLK(CUS_KEYBOARD_LISTCMD, OnAddCmdButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIcadCusKeyboard message handlers

BOOL CIcadCusKeyboard::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	//*** Subclass the accelerator editbox.
	m_AccelEdit.SubclassDlgItem(CUS_KEYBOARD_EDITNEWACCEL,this);
	m_AccelEdit.m_pParentDlg=this;
	//*** Fill the accelerator listbox.
	ResetAccelList();
	//*** Fill the command listbox.
	CListBox* pList = (CListBox*)GetDlgItem(CUS_KEYBOARD_LISTCMD);
	int ct,idxNew,ctStr;
	char* pstr,*ptr1;
	CString localizedToolTip;

	for(ct=0; ct<g_nMnuTbItems; ct++)
	{
		if(*g_MnuTbItems[ct].ToolTip==0 || *g_MnuTbItems[ct].Command==0 || g_MnuTbItems[ct].resourceIndex == 0)
		{
			continue;
		}

		/*D.G.*/// Check for the accessibility of the licensed components commands.
		if(!CIcadApp::m_LicensedComponents.IsCommandAccessible(g_MnuTbItems[ct].Command))
			continue;

		// The localized tooltip is stored one beyond the resource index, if the pAcadTbStr is available.
		int resourceIndex = g_MnuTbItems[ct].resourceIndex + (g_MnuTbItems[ct].pAcadTbStr != NULL ? 1 : 0);
		localizedToolTip.LoadString(resourceIndex);

		if (*localizedToolTip == 0)
			continue;

		int localizedToolTipLength = strlen(localizedToolTip);
		pstr = new char[localizedToolTipLength+1];
		for(ptr1=pstr, ctStr=0; ctStr < localizedToolTipLength; ctStr++)
		{
			//*** Step over any occurance of &
			if(localizedToolTip[ctStr]=='&')
			{
				continue;
			}
			//*** If we hit a \t, terminate the string there.
			if(localizedToolTip[ctStr]=='\t')
			{
				break;
			}
			*ptr1=localizedToolTip[ctStr];
			ptr1++;
		}
		*ptr1=0;
		if(LB_ERR==pList->FindStringExact(-1,pstr))
		{
			idxNew=pList->AddString(pstr);
			if(idxNew!=LB_ERR && idxNew!=LB_ERRSPACE)
			{
				pList->SetItemDataPtr(idxNew,(void*)g_MnuTbItems[ct].Command);
			}
		}
		delete [] pstr;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CIcadCusKeyboard::ResetAccelList()
{
	//*** Fill the accelerator listbox.
	CListBox* pAccelListBox = (CListBox*)GetDlgItem(CUS_KEYBOARD_LISTDEFKEYS);
	pAccelListBox->ResetContent();
	CPtrList* pAccelList = SDS_CMainWindow->m_pAccelList;
	POSITION pos = pAccelList->GetHeadPosition();
	ICACCEL* picAccel;
	CString strAccel;
	int idxNew,ct;
	for(ct=0; pos!=NULL; ct++)
	{
		picAccel = (ICACCEL*)pAccelList->GetNext(pos);

		/*D.G.*/// Check for the accessibility of the licensed components commands.
		if(!CIcadApp::m_LicensedComponents.IsCommandAccessible(picAccel->pstrCommand))
			continue;

		AccelToString(&(picAccel->accel),strAccel);
		if(strAccel.IsEmpty()) continue;
		idxNew=pAccelListBox->AddString(strAccel);
		pAccelListBox->SetItemDataPtr(idxNew,picAccel);
	}
	//*** Disable the accelerator editbox.
	if(m_AccelEdit.IsWindowEnabled())
	{
		CStatic* pAccelTxt = (CStatic*)GetDlgItem(CUS_KEYBOARD_TXTNEWACCEL);
		pAccelTxt->EnableWindow(FALSE);
		m_AccelEdit.EnableWindow(FALSE);
		m_AccelEdit.SetWindowText(""/*DNT*/);
	}
	//*** Disable the Delete and Add Command buttons.
	GetDlgItem(CUS_KEYBOARD_BTNDELETE)->EnableWindow(FALSE);
	GetDlgItem(CUS_KEYBOARD_BTNADDCMD)->EnableWindow(FALSE);
	//*** Disable the Command editbox.
	GetDlgItem(CUS_KEYBOARD_TXTCMD)->EnableWindow(FALSE);
	CEdit* pCmdEdit = (CEdit*)GetDlgItem(CUS_KEYBOARD_EDITCMD);
	pCmdEdit->EnableWindow(FALSE);
	pCmdEdit->SetWindowText(""/*DNT*/);
	//*** Set the pointer to the currently selected item to NULL.
	m_pCurAccel=NULL;
}

void CIcadCusKeyboard::OnSelChangeDefKeysList() 
{
	CListBox* pAccelListBox = (CListBox*)GetDlgItem(CUS_KEYBOARD_LISTDEFKEYS);
	CPtrList* pAccelList = SDS_CMainWindow->m_pAccelList;
	//*** Enable the Command editbox.
	GetDlgItem(CUS_KEYBOARD_TXTCMD)->EnableWindow();
	CEdit* pCmdEdit = (CEdit*)GetDlgItem(CUS_KEYBOARD_EDITCMD);
	pCmdEdit->EnableWindow();
	//*** Enable the Delete button.
	CButton* pBtn = (CButton*)GetDlgItem(CUS_KEYBOARD_BTNDELETE);
	pBtn->EnableWindow();
	//*** Enable the Add Command button.
	pBtn = (CButton*)GetDlgItem(CUS_KEYBOARD_BTNADDCMD);
	pBtn->EnableWindow();
	//*** Disable the accelerator editbox.
	if(m_AccelEdit.IsWindowEnabled())
	{
		CStatic* pAccelTxt = (CStatic*)GetDlgItem(CUS_KEYBOARD_TXTNEWACCEL);
		pAccelTxt->EnableWindow(FALSE);
		m_AccelEdit.EnableWindow(FALSE);
		m_AccelEdit.SetWindowText(""/*DNT*/);
	}
	//*** Save any changes to the command string.
	SaveCommandString();
	//*** Set the new command string in the Command editbox.
	ICACCEL* picAccel = (ICACCEL*)pAccelListBox->GetItemDataPtr(pAccelListBox->GetCurSel());
	if(picAccel==NULL || picAccel->pstrCommand==NULL) pCmdEdit->SetWindowText(ResourceString(IDC_ICADCUSKEYBOARD__5, "" ));
	else pCmdEdit->SetWindowText(picAccel->pstrCommand);
	//*** Set the pointer to the current ICACCEL structure.
	m_pCurAccel=picAccel;
}

void CIcadCusKeyboard::SaveCommandString()
{
	//*** Return if the pointer to the current accelerator structure is NULL.
	if(m_pCurAccel==NULL) return;
	CEdit* pCmdEdit = (CEdit*)GetDlgItem(CUS_KEYBOARD_EDITCMD);
	CString strText;
	pCmdEdit->GetWindowText(strText);
	//*** Return if the command string has not changed
	if((m_pCurAccel->pstrCommand==NULL && strText.IsEmpty()) ||
		(m_pCurAccel->pstrCommand!=NULL && strText.Compare(m_pCurAccel->pstrCommand)==0)) return;
	//*** Free the old command string and set the new value.
	if(m_pCurAccel->pstrCommand!=NULL) delete [] m_pCurAccel->pstrCommand;
	if(strText.IsEmpty()) m_pCurAccel->pstrCommand=NULL;
	else
	{
		m_pCurAccel->pstrCommand = new char[strText.GetLength()+1];
		strcpy(m_pCurAccel->pstrCommand,strText);
	}
	SDS_CMainWindow->m_bKeyboardModified=TRUE;
}

void CIcadCusKeyboard::OnAddButton() 
{
	//*** Save any information for the previous item. 
	SaveCommandString();
	//*** Enable and clear the Command editbox.
	GetDlgItem(CUS_KEYBOARD_TXTCMD)->EnableWindow();
	CEdit* pCmdEdit = (CEdit*)GetDlgItem(CUS_KEYBOARD_EDITCMD);
	pCmdEdit->EnableWindow();
	pCmdEdit->SetWindowText(""/*DNT*/);
	//*** Enable the Add Command button.
	GetDlgItem(CUS_KEYBOARD_BTNADDCMD)->EnableWindow();
	//*** Enable, clear, and set focus to the new accelerator editbox.
	if(!m_AccelEdit.IsWindowEnabled())
	{
		CStatic* pAccelTxt = (CStatic*)GetDlgItem(CUS_KEYBOARD_TXTNEWACCEL);
		pAccelTxt->EnableWindow();
		m_AccelEdit.EnableWindow();
	} 
	else m_AccelEdit.SetWindowText(""/*DNT*/);
	m_AccelEdit.SetFocus();
	SDS_CMainWindow->m_bKeyboardModified=TRUE;
}

void CIcadCusKeyboard::OnDeleteButton() 
{
	//*** Delete the current item in the Defined Keys listbox and the accelerator list.
	CListBox* pAccelListBox = (CListBox*)GetDlgItem(CUS_KEYBOARD_LISTDEFKEYS);
	DeleteAccelListItem(pAccelListBox->GetCurSel());
	//*** Disable the Delete and Add Command buttons.
	GetDlgItem(CUS_KEYBOARD_BTNDELETE)->EnableWindow(FALSE);
	GetDlgItem(CUS_KEYBOARD_BTNADDCMD)->EnableWindow(FALSE);
	//*** Disable the accelerator editbox.
	if(m_AccelEdit.IsWindowEnabled())
	{
		CStatic* pAccelTxt = (CStatic*)GetDlgItem(CUS_KEYBOARD_TXTNEWACCEL);
		pAccelTxt->EnableWindow(FALSE);
		m_AccelEdit.EnableWindow(FALSE);
		m_AccelEdit.SetWindowText(""/*DNT*/);
	}
	//*** Disable the Command editbox.
	GetDlgItem(CUS_KEYBOARD_TXTCMD)->EnableWindow(FALSE);
	CEdit* pCmdEdit = (CEdit*)GetDlgItem(CUS_KEYBOARD_EDITCMD);
	pCmdEdit->EnableWindow(FALSE);
	pCmdEdit->SetWindowText(""/*DNT*/);
	SDS_CMainWindow->m_bKeyboardModified=TRUE;
}

void CIcadCusKeyboard::DeleteAccelListItem(int idxItem)
{
	CListBox* pAccelListBox = (CListBox*)GetDlgItem(CUS_KEYBOARD_LISTDEFKEYS);
	ICACCEL* picAccel = (ICACCEL*)pAccelListBox->GetItemDataPtr(idxItem);
	pAccelListBox->DeleteString(idxItem);
	//*** Delete the item from the accelerator list.
	CPtrList* pAccelList = SDS_CMainWindow->m_pAccelList;
	POSITION pos = pAccelList->GetHeadPosition();
	while(pos!=NULL)
	{
		ICACCEL* piaCur = (ICACCEL*)pAccelList->GetAt(pos);
		if(piaCur==picAccel) break;
		pAccelList->GetNext(pos);
	}
	if(picAccel!=NULL)
	{
		if(picAccel->pstrCommand!=NULL) delete picAccel->pstrCommand;
		delete picAccel;
	}
	if(pos!=NULL) pAccelList->RemoveAt(pos);
	//*** Set the pointer to the current accelerator to NULL.
	m_pCurAccel=NULL;
}

void CIcadCusKeyboard::OnAddCmdButton() 
{
	//*** This function gets called when the "Add Command" button is picked and when
	//*** an item in the "Available Commands" listbox is double-clicked.
	//*** If the Add Command button is not enabled return.
	CButton* pBtnAdd = (CButton*)GetDlgItem(CUS_KEYBOARD_BTNADDCMD);
	if(pBtnAdd->IsWindowEnabled()==FALSE) return;
	//*** Get the command string from the pointer to the IcadCommand struct stored with the
	//*** list item.
	CListBox* pList = (CListBox*)GetDlgItem(CUS_KEYBOARD_LISTCMD);
	CString strCommand((LPCTSTR)pList->GetItemDataPtr(pList->GetCurSel()));
	//*** Add the string to the editbox.
	CEdit* pEdit = (CEdit*)GetDlgItem(CUS_KEYBOARD_EDITCMD);
	if(pEdit->IsWindowEnabled()==FALSE) return;
	CString strEdit;
	pEdit->GetWindowText(strEdit);
	strEdit+=strCommand;
	pEdit->SetWindowText(strEdit);
}

BOOL CIcadCusKeyboard::OnKillActive() 
{
	//*** Save any changes to the command string.
	SaveCommandString();
	return CPropertyPage::OnKillActive();
}

void CIcadCusKeyboard::OnExit() 
{
	CMainWindow* pMain = SDS_CMainWindow;
	//*** Save any changes to the command string.
	SaveCommandString();
	//*** Delete the accelerator table.
	DestroyAcceleratorTable(pMain->m_hAccel);
	//*** Load the new accelerator table.
	CPtrList* pAccelList = pMain->m_pAccelList;
	int nAccelItems = pAccelList->GetCount();
	LPACCEL lpAccel = new ACCEL[nAccelItems];
	POSITION pos = pAccelList->GetHeadPosition();
	ICACCEL* picAccel;
	int ct;
	for(ct=0; pos!=NULL; ct++)
	{
		picAccel = (ICACCEL*)pAccelList->GetNext(pos);
		lpAccel[ct].fVirt=picAccel->accel.fVirt;
		lpAccel[ct].key=picAccel->accel.key;
		lpAccel[ct].cmd=ICAD_ACCELID_START+ct;
	}
	pMain->m_hAccel=CreateAcceleratorTable(lpAccel,nAccelItems);
	delete [] lpAccel;
}

BOOL CIcadCusKeyboard::SaveAcceleratorsToFile(LPCTSTR pszFileName)
{
	if(pszFileName==NULL) return FALSE;
	//*** Make a local copy of the file name.
	char* pszFile = new char[lstrlen(pszFileName)+5]; //*** +5 in case it doesn't include an extension.
	lstrcpy(pszFile,pszFileName);
	//*** Make sure the file name has the correct extension.
	if(strstr(pszFile,".ick"/*DNT*/)==NULL)
	{
		char* pc1;
		if((pc1=strrchr(pszFile,'.'/*DNT*/))!=NULL) *pc1=0;
		ic_setext(pszFile,".ick"/*DNT*/);
	}
	CStdioFile fileSave;
	CFileException fileException;
	CString str;
	if(!fileSave.Open(pszFile,CFile::modeCreate | CFile::modeWrite,&fileException))
	{
		delete [] pszFile;
		return FALSE;
	}
	fileSave.WriteString("[IntelliCAD Custom Keyboard File]\n"/*DNT*/);
	//*** Get the number of accelerators
	CPtrList* pAccelList = SDS_CMainWindow->m_pAccelList;
	int nAccelKeys=pAccelList->GetCount();
	//*** Write the number of accelerators
	str.Format("\tnAccelKeys=%d\n"/*DNT*/,nAccelKeys);
	fileSave.WriteString(str);
	//*** Loop through the accelerator keys and write the accelerator information to
	//*** the file.
	POSITION pos = pAccelList->GetHeadPosition();
	ICACCEL* picAccel;
	int ct;
	for(ct=0; pos!=NULL && ct<nAccelKeys; ct++)
	{
		str.Format("[AccelKey-%d]\n"/*DNT*/,ct);
		fileSave.WriteString(str);
		//*** Write the accelerator structure info.
		picAccel = (ICACCEL*)pAccelList->GetNext(pos);
		if(picAccel->pstrCommand!=NULL)
		{
			str.Format("\tCommand=%s\n"/*DNT*/,picAccel->pstrCommand);
			fileSave.WriteString(str);
		}
		str.Format("\taccel=%d,%d,%d\n"/*DNT*/,picAccel->accel.fVirt,picAccel->accel.key,picAccel->accel.cmd);
		fileSave.WriteString(str);
	}
	//*** Close the file.
	fileSave.Close();
	delete [] pszFile;
	return TRUE;
}

int	SDS_ReadAccelerators(char* Fname)
{
	int rval = RTNORM;
	if(Fname==NULL) return RTERROR;
	
	//*** Make a local copy of the file name.
	char* pszFile = new char[strlen(Fname)+1];
	strcpy(pszFile,Fname);
	//*** Make sure the file name has the correct extension.
	if(strstr(pszFile,ResourceString(DNT_ICADCUSKEYBOARD__ICK_6, ".ick" ))==NULL)
	{
		delete [] pszFile;
		return FALSE;
	}
	CStdioFile fileRead;
	CFileException fileException;
	if(!fileRead.Open(pszFile,CFile::modeRead,&fileException))
	{
		delete [] pszFile;
		return FALSE;
	}
	CMainWindow* pMain = SDS_CMainWindow;
	CPtrList* pAccelList = pMain->m_pAccelList;
	ICACCEL* picAccel=NULL;
	int ct=0,nAccelKeys;
	char* pszTmp=NULL;
	CString str;
	fileRead.ReadString(str);
	if(str.Compare("[IntelliCAD Custom Keyboard File]"/*DNT*/)!=0) {rval = RTERROR; goto out; }
	//*** Get the number of accelerators.
	fileRead.ReadString(str);
	str.TrimLeft();
	pszTmp = new char[str.GetLength()+1];
	lstrcpy(pszTmp,str);
	if(!strnsame("nAccelKeys="/*DNT*/,pszTmp,9)) {rval = RTERROR; goto out; }
	char *pcTmp1,*pcTmp2;
	pcTmp1=(strrchr(pszTmp,'='/*DNT*/)+1);
	nAccelKeys=atoi(pcTmp1);
	if(nAccelKeys<=0) {rval = RTERROR; goto out; }
	delete [] pszTmp;
	pszTmp=NULL;
	//*** Loop through the accelerator keys and read the accelerator information from
	//*** the file.
	pMain->DeleteAccelList();
	while(fileRead.ReadString(str) && ct<=nAccelKeys)
	{
		if(pszTmp!=NULL) delete pszTmp;
		str.TrimLeft();
		pszTmp = new char[str.GetLength()+1];
		lstrcpy(pszTmp,str);
		if(strnsame("[AccelKey-"/*DNT*/,pszTmp,10))
		{
			//*** New accelerator.
			picAccel = new ICACCEL;
			memset(picAccel,0,sizeof(ICACCEL));
			pAccelList->AddTail(picAccel);
			ct++;
			continue;
		}
		else if(strnsame("Command="/*DNT*/,pszTmp,8))
		{
			if(picAccel==NULL || picAccel->pstrCommand!=NULL) continue;
			pcTmp1=(strrchr(pszTmp,'='/*DNT*/)+1);
			if(*pcTmp1==0) continue;
			picAccel->pstrCommand = new char[strlen(pcTmp1)+1];
			strcpy(picAccel->pstrCommand,pcTmp1);
		}
		else if(strnsame("accel="/*DNT*/,pszTmp,6))
		{ 
			if(picAccel==NULL) continue;
			pcTmp1=(strrchr(pszTmp,'='/*DNT*/)+1);
			if(*pcTmp1==0) continue;
			if((pcTmp2=strchr(pszTmp,','/*DNT*/))==NULL) continue;
			*pcTmp2=0;
			picAccel->accel.fVirt=(BYTE)atoi(pcTmp1);
			pcTmp1=(pcTmp2+1);
			if(*pcTmp1==0) continue;
			if((pcTmp2=strchr(pcTmp1,','/*DNT*/))==NULL) continue;
			*pcTmp2=0;
			picAccel->accel.key=(WORD)atoi(pcTmp1);
			pcTmp1=(pcTmp2+1);
			if(*pcTmp1==0) continue;
			picAccel->accel.cmd=(WORD)atoi(pcTmp1);
		}
	}

out:

	//*** Cleanup and bail.
	fileRead.Close();
	if(pszTmp!=NULL) delete [] pszTmp;
	delete [] pszFile;
	return rval;
}

BOOL CIcadCusKeyboard::ReadAcceleratorFile(LPCTSTR pszFileName)
{
	if(pszFileName==NULL) return FALSE;
	
	//*** Make a local copy of the file name.
	char* pszFile = new char[lstrlen(pszFileName)+1];
	lstrcpy(pszFile,pszFileName);

	int rval = SDS_ReadAccelerators(pszFile);

	delete[] pszFile;
	
	return rval == RTNORM;
}

#ifdef LOCALISE_ISSUE
// AccelToString has no support for non US Keyboards..
// Used to build a list of accelerators for the User define dialog.
#endif
void CIcadCusKeyboard::AccelToString(LPACCEL lpAccel, CString& strResult)
{
	strResult.Empty();
	if(lpAccel->fVirt & FCONTROL) strResult+="Ctrl"/*DNT*/;
	if(lpAccel->fVirt & FALT)
	{
		if(!strResult.IsEmpty()) strResult+="+"/*DNT*/;
		strResult+="Alt"/*DNT*/;
	}
	if(lpAccel->fVirt & FSHIFT)
	{
		//*** Don't allow any "Shift+..." strings.
		if(strResult.IsEmpty())
		{
			if(lpAccel->key>=VK_ZERO && lpAccel->key<=VK_Z) return;
		}
		else strResult+="+"/*DNT*/;
		strResult+="Shift"/*DNT*/;
	}
	if(!(lpAccel->fVirt & FVIRTKEY))
	{
		//*** There must be a virtual-key code or the accelerator is invalid.
		strResult.Empty();
		return;
	}
	if(lpAccel->key>=VK_ZERO && lpAccel->key<=VK_Z && strResult.IsEmpty()) return;
	if(!strResult.IsEmpty()) strResult+="+"/*DNT*/;
	switch(lpAccel->key)
	{
		case VK_BACK_SPACE:
			strResult+="Backspace"/*DNT*/;
			break;
		case VK_PAUSE:
			strResult+="Pause"/*DNT*/;
			break;
		case VK_PRIOR:
			strResult+="Page Up"/*DNT*/;
			break;
		case VK_NEXT:
			strResult+="Page Down"/*DNT*/;
			break;
		case VK_END:
			strResult+="End"/*DNT*/;
			break;
		case VK_HOME:
			strResult+="Home"/*DNT*/;
			break;
		case VK_LEFT:
			strResult+="Left Arrow"/*DNT*/;
			break;
		case VK_UP:
			strResult+="Up Arrow"/*DNT*/;
			break;
		case VK_RIGHT:
			strResult+="Right Arrow"/*DNT*/;
			break;
		case VK_DOWN:
			strResult+="Down Arrow"/*DNT*/;
			break;
		case VK_INSERT:
			strResult+="Ins"/*DNT*/;
			break;
		case VK_DELETE:
			strResult+="Del"/*DNT*/;
			break;
		case VK_ZERO:
			strResult+="Zero"/*DNT*/;
			break;
		case VK_1:
			strResult+="1"/*DNT*/;
			break;
		case VK_2:
			strResult+="2"/*DNT*/;
			break;
		case VK_3:
			strResult+="3"/*DNT*/;
			break;
		case VK_4:
			strResult+="4"/*DNT*/;
			break;
		case VK_5:
			strResult+="5"/*DNT*/;
			break;
		case VK_6:
			strResult+="6"/*DNT*/;
			break;
		case VK_7:
			strResult+="7"/*DNT*/;
			break;
		case VK_8:
			strResult+="8"/*DNT*/;
			break;
		case VK_9:
			strResult+="9"/*DNT*/;
			break;
		case VK_A:
			strResult+="A"/*DNT*/;
			break;
		case VK_B:
			strResult+="B"/*DNT*/;
			break;
		case VK_C:
			strResult+="C"/*DNT*/;
			break;
		case VK_D:
			strResult+="D"/*DNT*/;
			break;
		case VK_E:
			strResult+="E"/*DNT*/;
			break;
		case VK_F:
			strResult+="F"/*DNT*/;
			break;
		case VK_G:
			strResult+="G"/*DNT*/;
			break;
		case VK_H:
			strResult+="H"/*DNT*/;
			break;
		case VK_I:
			strResult+="I"/*DNT*/;
			break;
		case VK_J:
			strResult+="J"/*DNT*/;
			break;
		case VK_K:
			strResult+="K"/*DNT*/;
			break;
		case VK_L:
			strResult+="L"/*DNT*/;
			break;
		case VK_M:
			strResult+="M"/*DNT*/;
			break;
		case VK_N:
			strResult+="N"/*DNT*/;
			break;
		case VK_O:
			strResult+="O"/*DNT*/;
			break;
		case VK_P:
			strResult+="P"/*DNT*/;
			break;
		case VK_Q:
			strResult+="Q"/*DNT*/;
			break;
		case VK_R:
			strResult+="R"/*DNT*/;
			break;
		case VK_S:
			strResult+="S"/*DNT*/;
			break;
		case VK_T:
			strResult+="T"/*DNT*/;
			break;
		case VK_U:
			strResult+="U"/*DNT*/;
			break;
		case VK_V:
			strResult+="V"/*DNT*/;
			break;
		case VK_W:
			strResult+="W"/*DNT*/;
			break;
		case VK_X:
			strResult+="X"/*DNT*/;
			break;
		case VK_Y:
			strResult+="Y"/*DNT*/;
			break;
		case VK_Z:
			strResult+="Z"/*DNT*/;
			break;
		case VK_NUMPAD0:
			strResult+="Num 0"/*DNT*/;
			break;
		case VK_NUMPAD1:
			strResult+="Num 1"/*DNT*/;
			break;
		case VK_NUMPAD2:
			strResult+="Num 2"/*DNT*/;
			break;
		case VK_NUMPAD3:
			strResult+="Num 3"/*DNT*/;
			break;
		case VK_NUMPAD4:
			strResult+="Num 4"/*DNT*/;
			break;
		case VK_NUMPAD5:
			strResult+="Num 5"/*DNT*/;
			break;
		case VK_NUMPAD6:
			strResult+="Num 6"/*DNT*/;
			break;
		case VK_NUMPAD7:
			strResult+="Num 7"/*DNT*/;
			break;
		case VK_NUMPAD8:
			strResult+="Num 8"/*DNT*/;
			break;
		case VK_NUMPAD9:
			strResult+="Num 9"/*DNT*/;
			break;
		case VK_MULTIPLY:
			strResult+="Num *"/*DNT*/;
			break;
		case VK_ADD:
			strResult+="Num +"/*DNT*/;
			break;
		case VK_SUBTRACT:
			strResult+="Num -"/*DNT*/;
			break;
		case VK_DECIMAL:
			strResult+="Num Del"/*DNT*/;
			break;
		case VK_DIVIDE:
			strResult+="Num /"/*DNT*/;
			break;
		case VK_F1:
			strResult+="F1"/*DNT*/;
			break;
		case VK_F2:
			strResult+="F2"/*DNT*/;
			break;
		case VK_F3:
			strResult+="F3"/*DNT*/;
			break;
		case VK_F4:
			strResult+="F4"/*DNT*/;
			break;
		case VK_F5:
			strResult+="F5"/*DNT*/;
			break;
		case VK_F6:
			strResult+="F6"/*DNT*/;
			break;
		case VK_F7:
			strResult+="F7"/*DNT*/;
			break;
		case VK_F8:
			strResult+="F8"/*DNT*/;
			break;
		case VK_F9:
			strResult+="F9"/*DNT*/;
			break;
		case VK_F10:
			strResult+="F10"/*DNT*/;
			break;
		case VK_F11:
			strResult+="F11"/*DNT*/;
			break;
		case VK_F12:
			strResult+="F12"/*DNT*/;
			break;
		case VK_F13:
			strResult+="F13"/*DNT*/;
			break;
		case VK_F14:
			strResult+="F14"/*DNT*/;
			break;
		case VK_F15:
			strResult+="F15"/*DNT*/;
			break;
		case VK_F16:
			strResult+="F16"/*DNT*/;
			break;
		default:
			strResult.Empty();
			break;
	}
}



