/* C:\DEV\PRJ\ICAD\ICADABOUTDIA.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 
// THIS FILE HAS BEEN GLOBALIZED!

//** Includes
#include "Icad.h"/*DNT*/
#include "IcadApp.h"
#include "cmds.h"/*DNT*/	
#include "Authenticator.h"
#include "Authrc.h"
#include "Preferences.h"
#include "blddate.h"

//This is the dialog class
class abt_About:public CDialog {
    public:
		CString m_CCopyright;
    public:
        CAnimateCtrl m_Animate;
        abt_About();
        BOOL OnInitDialog();
    protected:
		void OnLButtonDown(UINT nFlags, CPoint point); 
		
    DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(abt_About,CDialog)
	ON_WM_LBUTTONDOWN()
    ON_WM_PAINT()
END_MESSAGE_MAP()


BOOL abt_About::OnInitDialog() {
	HWND hItemWnd;
//    LPCTSTR SDS_RegEntry = HKEY_INTELLICAD_SETUP/*DNT*/;
  	char szTmp[IC_ACADBUF];
	CIntelliCadPrefKey		setupKey(HKEY_CURRENT_USER, "Setup");
    HKEY hKey = setupKey;
	DWORD type;
	ULONG fl2;

    CDialog::OnInitDialog();

    //Insert copyright notice
    hItemWnd=::GetDlgItem(m_hWnd,ABT_COPYRIGHT);
	LOADSTRINGS_2(IDC_ICADABOUTDIA_WARNING__TH_0);
    m_CCopyright = LOADEDSTRING;

    ::SetWindowText(hItemWnd,MLBLDVERS);

    //Insert copyright notice
    hItemWnd=::GetDlgItem(m_hWnd,IDC_ABOUT_VERSION);
    ::SetWindowText(hItemWnd,MLBLDVERS);

	if (hKey != NULL) 
	{
		// Check for user.
		fl2=sizeof(szTmp);
		if(ERROR_SUCCESS==RegQueryValueEx(hKey,"RegisteredOwner"/*DNT*/,NULL,&type,(unsigned char *)szTmp,&fl2))
		{
			hItemWnd=::GetDlgItem(m_hWnd,IDC_ABOUT_USER);
			::SetWindowText(hItemWnd,szTmp);
		}
		// Check for company
		fl2=sizeof(szTmp);
		if(ERROR_SUCCESS==RegQueryValueEx(hKey,"RegisteredOrganization"/*DNT*/,NULL,&type,(unsigned char *)szTmp,&fl2))
		{
			hItemWnd=::GetDlgItem(m_hWnd,IDC_ABOUT_COMPANY);
			::SetWindowText(hItemWnd,szTmp);
		}

		// Check for serial number
		BOOL	licensed = FALSE;
		fl2=sizeof(szTmp);
		licensed = CIcadApp::SerialNumber( szTmp, fl2);

		if ( CIcadApp::IsTestDrive() )
		{
			hItemWnd=::GetDlgItem(m_hWnd,IDC_ABOUT_PRODUCTID);
			::SetWindowText(hItemWnd,ResourceString( IDS_IS_TESTDRIVE, "This product is for evaluation only."));
		}
		else if ( licensed )
		{
			hItemWnd=::GetDlgItem(m_hWnd,IDC_ABOUT_PRODUCTID);
			::SetWindowText(hItemWnd,szTmp);

			// Check for support number
  			char support[IC_ACADBUF];
			fl2=sizeof(support);
			if(ERROR_SUCCESS==RegQueryValueEx(hKey,"SupportNumber"/*DNT*/,NULL,&type,(unsigned char *)support,&fl2))
			{
				hItemWnd=::GetDlgItem(m_hWnd,IDC_ABOUT_SUPPORTID);
				::SetWindowText(hItemWnd,support);
			}
				// since it's registered, just use the serial number
			else
			{
				hItemWnd=::GetDlgItem(m_hWnd,IDC_ABOUT_SUPPORTID);
				::SetWindowText(hItemWnd,szTmp);
			}
		}
		else
		{
		    //Modified Cybage SBD 22/03/2001 DD/MM/YYYY [
			//Reason : Mail from Surya Sarda(suryas@cadopia.com) Dated 22/03/2001 DD/MM/YYYY.
			//Check for serial number.
			fl2=sizeof(szTmp);
			if(ERROR_SUCCESS==RegQueryValueEx(hKey,"SerialNumber"/*DNT*/,NULL,&type,(unsigned char *)szTmp,&fl2))
			{
				hItemWnd=::GetDlgItem(m_hWnd,IDC_ABOUT_PRODUCTID);
				::SetWindowText(hItemWnd,szTmp);
			}
			//Modified Cybage SBD 22/03/2001 DD/MM/YYYY ]
			else 
			{
				hItemWnd=::GetDlgItem(m_hWnd,IDC_ABOUT_PRODUCTID);
				::SetWindowText(hItemWnd,ResourceString( IDS_NOT_LICENSED, "This product is not licensed."));
			}
		}
	}

//	RegCloseKey(hKey);
    return(true);
}

void abt_About::OnLButtonDown(UINT nFlags, CPoint point) 
{	
/*	char *text[]={
	};
	
	CDC *cdc;
	CDC bmcdc;
	CPen pen;
	CBitmap *bm=NULL;
	int rc,fi1,fi2;
    HWND hItemWnd;
	MSG msg;
	char fs1[IC_ACADBUF];
	static int DoTheEgg;
	char *pBuffer=NULL;
	unsigned long bytes;
	CRect lpRectScroll(0,300,650,500);
	CBrush brush(GetSysColor(COLOR_ACTIVEBORDER));

	if(point.x>110 || point.y>110 || point.x<6 || point.y<6	|| 
	    !(nFlags&MK_CONTROL) || !(nFlags&MK_SHIFT)) goto NoEgg;

	cmd_PlaySound(0);
	cdc=this->GetDC();

	bmcdc.CreateCompatibleDC(cdc);
	hItemWnd=::GetDlgItem(m_hWnd,ABT_COPYRIGHT);

	if(RTNORM!=sds_findfile("MfcX32.dll",fs1)) goto out;
	if(FALSE==SDS_ReadFileIntoBuffer(fs1,&pBuffer,&bytes)) goto out;
	bm=SDS_GetBitmapFromBuffer(cdc,pBuffer,bytes);
	cdc->FillRect(lpRectScroll,&brush);
	DeleteObject(bmcdc.SelectObject(bm));
	for(fi1=0; fi1<=150; ++fi1) {
		rc=cdc->BitBlt(0,300-(fi1*2),480,fi1*2,&bmcdc,0,0,SRCCOPY);
		Sleep(15);
		while(PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE)) {
			GetMessage(&msg,NULL,0,0);
			if(msg.message==WM_LBUTTONDOWN || msg.message==WM_PAINT) goto out;
		}
	}
	cdc->SetBkColor(GetSysColor(COLOR_ACTIVEBORDER));
	for(fi1=0; fi1<sizeof(text)/sizeof(text[0]); ++fi1) {
		strcpy(fs1,text[fi1]);
		for(fi2=0; fi2<(int)strlen(fs1); fi2++) {
			if(fs1[fi2]=='~')      fs1[fi2]='W';
			else if(fs1[fi2]=='A') fs1[fi2]='y';
			else                   fs1[fi2]-=5;
		}
		rc=cdc->TextOut(10,405,fs1,strlen(fs1));
		for(fi2=0; fi2<23; ++fi2) {
		    cdc->ScrollDC(0,-1,&lpRectScroll,&lpRectScroll,NULL,NULL);
			Sleep(60);
			while(PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE)) {
				GetMessage(&msg,NULL,0,0);
				if(msg.message==WM_LBUTTONDOWN || msg.message==WM_PAINT) goto out;
			}
		}
	}

	out: ;
	
	if(pBuffer) free(pBuffer);
	if(bm) delete bm;
	this->ReleaseDC(cdc);
	this->InvalidateRect(NULL);

    NoEgg: ;	
*/	CDialog::OnLButtonDown(nFlags, point);
}

abt_About::abt_About()
	{
		// load the dialog template from IcadAuth.DLL
	HINSTANCE	authentication = AuthenticationInstance();
	HRSRC hResource = ::FindResource( authentication, MAKEINTRESOURCE( IDD_ABOUT), RT_DIALOG);
	m_hDialogTemplate = LoadResource( authentication, hResource);
	}

//***This is the main window function
void CMainWindow::IcadAbout() 
	{
    abt_About About;
    About.DoModal();
	}


