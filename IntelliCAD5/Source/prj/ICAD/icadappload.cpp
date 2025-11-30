/* C:\DEV\PRJ\ICAD\ICADAPPLOAD.CPP
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
#include "IcadApi.h"
#include "lisp.h"/*DNT*/
#include "resource.hm"/*DNT*/
#include "paths.h"
#include "IcadApp.h"
#include "IcadDialog.h"
#include "LicensedComponents.h"

#ifdef BUILD_WITH_VBA
namespace Office
	{
#include <vbaext.h>
#include <vbaguids.h>
const IID IID__AddIns =	{0xDA936B62L,0xAC8B,0x11D1,{0xB6,0xE5,0x00,0xA0,0xC9,0x0F,0x27,0x44}};
	}
using namespace Office;
#endif

#define ICAD_APPLOAD "icadload.dfs"/*DNT*/

class APPLOAD_LoadDlg : public IcadDialog 
	{
	
    public:
        BOOL      bupdate;
        short     cnt;
        char    **stringarray;
        CString   cString;
        CListBox *Applistbox;
		
        APPLOAD_LoadDlg() : IcadDialog(APPLOAD_Appload) {}
		
    public:
        void         Makestring(char *string);
        void         DeleteString();
		
    protected:
        afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
        afx_msg void OnHelp();
        void         OnAddFile();
        void         OnLoadfile();
        void         OnRemoveFile();
        void         OnLBSelchange();
        void         OnUnloadfile();
        virtual BOOL OnInitDialog();
        void         OnOK();
		
		DECLARE_MESSAGE_MAP()
	};

BEGIN_MESSAGE_MAP(APPLOAD_LoadDlg,CDialog)
ON_BN_CLICKED(APPLOAD_FILE,OnAddFile)
ON_BN_CLICKED(APPLOAD_REMOVE,OnRemoveFile)
ON_BN_CLICKED(APPLOAD_LOAD,OnLoadfile)
ON_BN_CLICKED(APPLOAD_UNLOAD,OnUnloadfile)
ON_LBN_SELCHANGE(APPLOAD_LIST,OnLBSelchange)
ON_BN_CLICKED(IDHELP,OnHelp)
ON_WM_HELPINFO()
END_MESSAGE_MAP()


CString AddinPrefix;// = "COM Add-In ";
CString AddinColon;// = " : ";
extern bool g_Vba6Installed;


class AddinHelper
	{
	public:
		static HRESULT GetNumberOfAddins (long * Count);
		static HRESULT GetAddinNameAndStatus (long Index, CString *Name, BOOL *Loaded);
		static HRESULT LoadAddin (long Index);
		static HRESULT UnloadAddin (long Index);
	};

#ifdef BUILD_WITH_VBA

HRESULT AddinHelper::GetNumberOfAddins (long * Count)
	{
	
	if(!CIcadApp::m_LicensedComponents.IsAccessible(CLicensedComponents::Components::VBA))
	{
		*Count = 0;
		return NOERROR;
	}

	HRESULT hr = E_FAIL;

	if (!g_Vba6Installed)
		{
		*Count = 0;
		return hr;
		}

	CComPtr<IDispatch> pDisp = NULL;
	pDisp = GetApp()->ApcHostAddIns->GetAddIns ();

	if (pDisp)
		{
		CComPtr<_AddIns> pAddins;
		hr = pDisp->QueryInterface(IID__AddIns, (void **)&pAddins);
		if (SUCCEEDED (hr))
			{
			hr = pAddins->get_Count(Count);
			}
		}
	return hr;
	}

HRESULT AddinHelper::GetAddinNameAndStatus (long Index, CString *Name, BOOL *Loaded)
	{

	if(!CIcadApp::m_LicensedComponents.IsAccessible(CLicensedComponents::Components::VBA))
		return E_NOTIMPL;

	
	HRESULT hr = E_FAIL;

	if (!g_Vba6Installed)
		return hr;

	long nCount;
	CComPtr<IDispatch> pDisp = NULL;
	pDisp = GetApp()->ApcHostAddIns->GetAddIns ();
	CString sIndex;
	sIndex.Format("%d", Index);

	if (pDisp)
		{
		CComPtr<_AddIns> pAddins;
		hr = pDisp->QueryInterface(IID__AddIns, (void **)&pAddins);
		if (SUCCEEDED (hr))
			{
			hr = pAddins->get_Count(&nCount);
			ASSERT (SUCCEEDED (hr) && (Index <= nCount) && (Index > 0));

			VARIANT_BOOL bConnected;
			BSTR bstrName;
			CString str;
			CComPtr<AddIn> pAddin;

			CComVariant varIndex;
			V_VT(&varIndex) = VT_I4;
			V_I4(&varIndex) = Index;	
			hr = pAddins->Item (varIndex, &pAddin);
			if (FAILED(hr))
				return hr;

			hr = pAddin->get_ProgID (&bstrName);
			if (FAILED(hr))
				return hr;

			str = bstrName;
			*Name = AddinPrefix + sIndex + AddinColon + str;

			hr = pAddin->get_Connect (&bConnected);
			if (FAILED(hr))
				return hr;

			if (bConnected == VARIANT_TRUE)
				*Loaded = TRUE;
			else
				*Loaded = FALSE;
			}
		}
	return hr;
	}

HRESULT AddinHelper::LoadAddin (long Index)
	{

	if(!CIcadApp::m_LicensedComponents.IsAccessible(CLicensedComponents::Components::VBA))
		return E_NOTIMPL;

	HRESULT hr = E_FAIL;

	if (!g_Vba6Installed)
		return hr;
	
	long nCount;
	CComPtr<IDispatch> pDisp = NULL;
	pDisp = GetApp()->ApcHostAddIns->GetAddIns ();

	if (pDisp)
		{
		CComPtr<_AddIns> pAddins;
		hr = pDisp->QueryInterface(IID__AddIns, (void **)&pAddins);
		if (SUCCEEDED (hr))
			{
			hr = pAddins->get_Count(&nCount);
			ASSERT (SUCCEEDED (hr) && (Index <= nCount) && (Index > 0));

			VARIANT_BOOL bConnected;
			CComPtr<AddIn> pAddin;

			CComVariant varIndex;
			V_VT(&varIndex) = VT_I4;
			V_I4(&varIndex) = Index;	
			hr = pAddins->Item (varIndex, &pAddin);
			if (FAILED(hr))
				return hr;

			hr = pAddin->get_Connect (&bConnected);
			if (FAILED(hr))
				return hr;

			if (bConnected == VARIANT_FALSE) //Load it only if currently unloaded
				{
				bConnected = VARIANT_TRUE;
				hr = pAddin->put_Connect (bConnected);
				if (FAILED(hr))
					return hr;
				}
			}
		}
	return hr;
	}

HRESULT AddinHelper::UnloadAddin (long Index)
	{

	if(!CIcadApp::m_LicensedComponents.IsAccessible(CLicensedComponents::Components::VBA))
		return E_NOTIMPL;

	HRESULT hr = E_FAIL;

	if (!g_Vba6Installed)
		return hr;

	long nCount;
	CComPtr<IDispatch> pDisp = NULL;
	pDisp = GetApp()->ApcHostAddIns->GetAddIns ();

	if (pDisp)
		{
		CComPtr<_AddIns> pAddins;
		hr = pDisp->QueryInterface(IID__AddIns, (void **)&pAddins);
		if (SUCCEEDED (hr))
			{
			hr = pAddins->get_Count(&nCount);
			ASSERT (SUCCEEDED (hr) && (Index <= nCount) && (Index > 0));

			VARIANT_BOOL bConnected;
			CComPtr<AddIn> pAddin;

			CComVariant varIndex;
			V_VT(&varIndex) = VT_I4;
			V_I4(&varIndex) = Index;	
			hr = pAddins->Item (varIndex, &pAddin);
			if (FAILED(hr))
				return hr;

			hr = pAddin->get_Connect (&bConnected);
			if (FAILED(hr))
				return hr;

			if (bConnected == VARIANT_TRUE) //UnLoad it only if currently loaded
				{
				bConnected = VARIANT_FALSE;
				hr = pAddin->put_Connect (bConnected);
				if (FAILED(hr))
					return hr;
				}
			}
		}
	return hr;
	}
#else
HRESULT AddinHelper::GetNumberOfAddins (long * Count)
	{
	*Count = 0;
	return NOERROR;
	}

HRESULT AddinHelper::GetAddinNameAndStatus (long Index, CString *Name, BOOL *Loaded)
	{
	return E_NOTIMPL;
	}

HRESULT AddinHelper::LoadAddin (long Index)
	{
	return E_NOTIMPL;
	}

HRESULT AddinHelper::UnloadAddin (long Index)
	{
	return E_NOTIMPL;
	}


#endif //BUILD_WITH_VBA


void APPLOAD_LoadDlg::DeleteString() 
	{
    short fi1;
    for(fi1=0;fi1<cnt;++fi1) 
		IC_FREE(stringarray[fi1]);
    delete [] stringarray;
    cnt=0;
    stringarray=NULL;
	}

void APPLOAD_LoadDlg::Makestring(char *string) 
	{
    if (!stringarray) 
		{
        cnt=1;
        stringarray=new char *[1];
		} 
	else 
		{
        ++cnt;
		char **temp;
		temp = new char *[cnt];
		memcpy(temp, stringarray, sizeof(char *) * (cnt - 1));
		delete [] stringarray;
		stringarray = temp;
		}
	stringarray[(cnt-1)]=new char[strlen(string)+1];
	strcpy(stringarray[(cnt-1)],string);
	}

BOOL APPLOAD_LoadDlg::OnHelpInfo(HELPINFO *pHel)
	{
    OnHelp();
	return(true);
	}

void APPLOAD_LoadDlg::OnHelp() 
	{
	CommandRefHelp( m_hWnd, HLP_APPLOAD);
	}

void APPLOAD_LoadDlg::OnUnloadfile() 
	{
    int      iSelItems, fi1, fi2;
    LPINT    ipArray;
    _TCHAR  *string=NULL, *fcp1;
    CButton *cbutton;
	char     fs1[IC_ACADBUF];
	
    if (NULL==(string=(char *)new char[IC_ACADBUF])) 
		goto done;

    iSelItems=Applistbox->GetSelCount();
    if (NULL==(ipArray=(LPINT)new int[iSelItems])) 
		goto done;

    Applistbox->GetSelItems(iSelItems,ipArray);

    for(fi1=iSelItems-1;fi1>-1;--fi1) 
		{
        Applistbox->GetText(ipArray[fi1],string);
 	 	if (!strnisame (string, AddinPrefix, AddinPrefix.GetLength()))	//if it is COM ADD-in - do not save it
			{
			if (NULL==(fcp1=strrchr(string,'.'/*DNT*/))) 
				goto done;
			
			if (strisame(fcp1+1,"dll"/*DNT*/)) 
				{
				sprintf(fs1,"(XUNLOAD \"%s\")"/*DNT*/,string);
				for(fi2=0; fs1[fi2]!=0; fi2++) 
					{ 
					if(fs1[fi2]=='\\'/*DNT*/) 
						fs1[fi2]='/'/*DNT*/; 
					//EBATECH(R.Arayashiki) ]- for MBCS String
					else if ((_MBC_LEAD ==_mbbtype((unsigned char)fs1[fi2],0)) &&
						  (_MBC_TRAIL==_mbbtype((unsigned char)fs1[fi2+1],1)))
						fi2 ++;
					// ]- EBATECH
					}
				sds_printf(ResourceString(IDC_ICADAPPLOAD__NUNLOADING__5, "\nUnloading %s" ),string);
				SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)fs1);
				SDS_CMainWindow->m_IgnoreLastCmd=true;
				Applistbox->SetSel(ipArray[fi1],FALSE);
				}
			}
		else
			{
			int Index, len;
			len = strlen(AddinPrefix);
			char *p = &string[len];
			sscanf (p, "%d", &Index);
			AddinHelper::UnloadAddin (Index);
			}
		}

    cbutton=(CButton *)GetDlgItem(APPLOAD_UNLOAD); cbutton->EnableWindow(FALSE);
	
done:
	if (string) { delete [] string; }
	delete [] ipArray;
	}

void APPLOAD_LoadDlg::OnLBSelchange() 
	{
    int            iSelItems, fi1;
    LPINT          ipArray;
    _TCHAR        *string=NULL, *fcp1;
    struct resbuf *rb, *rbb;
    CButton       *cbutton, *cRemoveButton;
	CString Name;
	
    if (0==(iSelItems=Applistbox->GetSelCount())) 
		{
        cbutton=(CButton *)GetDlgItem(APPLOAD_UNLOAD); cbutton->EnableWindow(FALSE);
        cbutton=(CButton *)GetDlgItem(APPLOAD_REMOVE); cbutton->EnableWindow(FALSE);
        cbutton=(CButton *)GetDlgItem(APPLOAD_LOAD);   cbutton->EnableWindow(FALSE);
		} 
	else 
		{
        cbutton=(CButton *)GetDlgItem(APPLOAD_LOAD);   cbutton->EnableWindow(TRUE);
        cbutton=(CButton *)GetDlgItem(APPLOAD_REMOVE); cbutton->EnableWindow(TRUE);
		}
	
	// Get the class pointer from the appload unload button.
	cbutton=(CButton *)GetDlgItem(APPLOAD_UNLOAD);
    cRemoveButton=(CButton *)GetDlgItem(APPLOAD_REMOVE); 
	
	// Create an array of integers.
	if (NULL==(ipArray=(LPINT)new int[iSelItems])) 
		return;
	
	// Get an array of index's of the selected items.
	Applistbox->GetSelItems(iSelItems,ipArray);
	
	// Create the string buffer.
	if (NULL==(string=(char *)new char[IC_ACADBUF])) 
		goto done;
	
	// returns a link list of resbuf's that are the loaded application names.
	//if (NULL==(rb=sds_loaded())) 
	//	goto done;
	rb=sds_loaded();
	
	// Loop through the list of resbuf's.
	for(rbb=rb;rbb!=NULL;rbb=rbb->rbnext) 
		{
		// Go through the index of items selected.
		for(fi1=iSelItems-1;fi1>-1;--fi1) 
			{
			// Get the text string.
			if (LB_ERR==(Applistbox->GetText(ipArray[fi1],string))) 
				goto done;
			
			// find the last back slash.
			if (NULL==(fcp1=strrchr(string,'\\'/*DNT*/))) 
				continue;
			
			// find the application name in the path string. If the application name is the same enable the unload button.
			if (strstr(rbb->resval.rstring,fcp1+1)) 
				{
				cbutton->EnableWindow(TRUE);
				sds_relrb(rb);
				//                for(rbb=rbt=rb;rbb!=NULL;rbt=rbb) { rbb=rbt->rbnext; sds_relrb(rbt); rbt=NULL; }
				delete [] string;
				delete [] ipArray;
				return;
				}
			}
		}

	//If the app has not yet been found, loop through the Addins
	long nCount, i;
	BOOL bLoaded;
	AddinHelper::GetNumberOfAddins (&nCount);
	for(fi1=iSelItems-1;fi1>-1;--fi1) 
		{
		// Get the text string.
		if (LB_ERR==(Applistbox->GetText(ipArray[fi1],string))) 
			goto done;
		
		for (i = 1; i <= nCount; i++)
			{
			AddinHelper::GetAddinNameAndStatus (i, &Name, &bLoaded);
			if (strsame (string, Name))
				{
				//if it is an Addin - disable the remove button
				cRemoveButton->EnableWindow(FALSE);

				//if the Addin is Loaded, enable the Unload button
				if (bLoaded)
					cbutton->EnableWindow(TRUE);
				sds_relrb(rb);
				delete [] string;
				delete [] ipArray;
				return;
				}
			}
		}
		
done:
	if (string) { delete [] string; }
	delete [] ipArray;
	cbutton->EnableWindow(FALSE);
	sds_relrb(rb);
	}

void APPLOAD_LoadDlg::OnLoadfile() 
	{
    int      iSelItems, icount, fi1;
    LPINT    ipArray;
    _TCHAR  *string=NULL;
    CButton *cbutton;
    FILE    *stream=NULL;
	
    // Get the pointer to the given control.
    cbutton=(CButton *)GetDlgItem(APPLOAD_UPDATE);

    // Get the state of the check button.
    bupdate=(BOOL)cbutton->GetState();

    // If TRUE then save the list of
    if (bupdate) 
		{
        if (NULL==(string=(char *)new char[IC_ACADBUF])) 
			goto done;
        if ((sds_findfile(ICAD_APPLOAD,string))!=RTNORM) 
			goto done;
        if (NULL==(stream=fopen(string,"w+"/*DNT*/))) 
			goto done;
		
        icount=Applistbox->GetCount();
        for(fi1=0;fi1<icount;++fi1) 
			{
            Applistbox->GetText(fi1,string);
	 		if (!strnisame (string, AddinPrefix, AddinPrefix.GetLength()))	//if it is COM ADD-in - do not save it
				{
	            fprintf(stream,"%s"/*DNT*/,string);
		        if ((fi1+1)<icount) 
					fprintf(stream,"\n"/*DNT*/);
				}
			}
        if (string) 
			delete [] string;
        if (stream) 
			fclose(stream);
		}
	
    if (NULL==(string=(char *)new char[IC_ACADBUF])) 
		goto done;
	
    iSelItems=Applistbox->GetSelCount();
    if (NULL==(ipArray=(LPINT)new int[iSelItems])) 
		goto done;
	
    Applistbox->GetSelItems(iSelItems,ipArray);
	
    for(fi1=iSelItems-1;fi1>-1;--fi1) 
		{
        Applistbox->GetText(ipArray[fi1],string);
 		if (!strnisame (string, AddinPrefix, AddinPrefix.GetLength()))	//if it is COM ADD-in - do not save it
			{
	        Makestring(string);
			}
		else
			{
			int Index, len;
			len = strlen(AddinPrefix);
			char *p = &string[len];
			sscanf (p, "%d", &Index);
			AddinHelper::LoadAddin (Index);
			}
		}
	
done:
	if (string) { delete [] string; }
	delete [] ipArray;
	EndDialog(2);
	}

////////////////////////////////////////////////////////////////////////////////////
//
// OnRemoveFile()
//
// This function is called when the remove file button is pushed.
//
// returns: void.
//
void APPLOAD_LoadDlg::OnRemoveFile() 
	{
    int      iSelItems,	fi1;
    LPINT    ipArray;
    CButton *cbutton;
	BOOL	 bRemoved = FALSE;
    _TCHAR  *string=NULL;
	
    // Get the number of rows selected.
    iSelItems=Applistbox->GetSelCount();

    // Get an array of indicies for all of the selected rows.
    if (NULL==(ipArray=(LPINT)new int[iSelItems])) 
		return;

    Applistbox->GetSelItems(iSelItems,ipArray);

    // allocate string for list in file.
    if (NULL==(string=(char *)new char[IC_ACADBUF])) 
		goto done;

    // Delete each selected item from the list.
    for(fi1=iSelItems-1;fi1>-1;--fi1) 
		{
        Applistbox->GetText(ipArray[fi1],string);
		if (!strnisame (string, AddinPrefix, AddinPrefix.GetLength()))	//if it is COM ADD-in - do not remove it
			{
			bRemoved = TRUE;
			Applistbox->DeleteString(ipArray[fi1]);
			}
		}

	if (bRemoved)
		{
		// Do the proper button enablling/disablling stuff.
		cbutton=(CButton *)GetDlgItem(APPLOAD_UNLOAD); cbutton->EnableWindow(FALSE);
		cbutton=(CButton *)GetDlgItem(APPLOAD_REMOVE); cbutton->EnableWindow(FALSE);
		cbutton=(CButton *)GetDlgItem(APPLOAD_LOAD);   cbutton->EnableWindow(FALSE);
		}

done:
	if (string) { delete [] string; }
    delete [] ipArray;
	}

////////////////////////////////////////////////////////////////////////////////////
//
// OnAddFile()
//
// This function is called when the addfile button is pushed, brings up the getfield
// dialog to select an application to add to the list.
//
// returns: void.
//
void APPLOAD_LoadDlg::OnAddFile() 
	{
    struct resbuf  rb, *rbb, *rbt;
    int            cnt;
    CButton       *cbutton;
    char          *fcp1;
	
	// EBATECH(CNBR) 2002/4/30 Disable TYPE button option+2
	if ((sds_getfiled(ResourceString(IDC_ICADAPPLOAD_SELECT_LISP__7, "Select LISP or SDS Files" ),ResourceString(IDC_ICADAPPLOAD__8, "" ),ResourceString(IDC_ICADAPPLOAD_LISP_APPLICA_9, "LISP Application|lsp,SDS Application|dll,Menu LISP Extension|mnl" ),2,&rb))!=RTNORM) 
		{
        rb.resval.rstring=NULL;
        return;
		}

    if ((Applistbox->FindString(-1,rb.resval.rstring))!=LB_ERR) 
		goto out;

    Applistbox->AddString(rb.resval.rstring);
    cnt=(Applistbox->GetCount()-1);
    Applistbox->SetSel(-1,FALSE);
    Applistbox->SetSel(cnt);

    cbutton=(CButton *)GetDlgItem(APPLOAD_REMOVE); 
	cbutton->EnableWindow(TRUE);

    cbutton=(CButton *)GetDlgItem(APPLOAD_LOAD);   
	cbutton->EnableWindow(TRUE);
	
    if (NULL==(fcp1=strrchr(rb.resval.rstring,'.'/*DNT*/))) 
		goto out;

    if (strisame(fcp1+1,"dll"/*DNT*/)) 
		{
        if (NULL==(rbt=sds_loaded())) 
			goto out;

        for(rbb=rbt;rbb!=NULL;rbb=rbb->rbnext) 
			{
            if (NULL==(fcp1=strrchr(rb.resval.rstring,'\\'/*DNT*/))) 
				continue;

            // find the application name in the path string. If the application name is the same enable the unload button.
            if (strstr(rbb->resval.rstring,fcp1+1)) 
				{
                cbutton=(CButton *)GetDlgItem(APPLOAD_UNLOAD); cbutton->EnableWindow(TRUE);
                break;
				}
			}
		}
out:
	if (rb.resval.rstring)
		IC_FREE(rb.resval.rstring);
	}

////////////////////////////////////////////////////////////////////////////////////
//
// OnInitDialog()
//
// This function is called on startup, here all the initialization stuff is done.
//
// returns: (TRUE) if successful
//          (FALSE) if error.
//
BOOL APPLOAD_LoadDlg::OnInitDialog() 
	{
    FILE          *stream;
    _TCHAR        *string = NULL, *fcp1;
    struct resbuf *rbb, *rbt = NULL;
    CButton       *cbutton;
    int            ret;
    BOOL           Enableflag=FALSE;
	long nCount, i;
	CString Name;
	BOOL bLoaded;
	
    IcadDialog::OnInitDialog();

	AddinPrefix = ResourceString(IDC_VBA_ADDIN_PREFIX, "COM Add-In " );
	AddinColon = ResourceString(IDC_VBA_ADDIN_COLON, " : " );

    // Get the pointer to the given control.
    Applistbox=(CListBox *)GetDlgItem(APPLOAD_LIST);
	
    // allocate string for list in file.
    if (NULL==(string=(char *)new char[IC_ACADBUF])) 
		goto done;
	
    // Search for the file.
    if (RTNORM!=(ret=sds_findfile(ICAD_APPLOAD,string)))
		{
        strcpy(string,ICAD_APPLOAD);
		}
	
    // Open the file in read only mode.
    if (NULL==(stream=fopen(string,(ret!=RTNORM)?"w"/*DNT*/:"r" /*DNT*/))) 
		goto done;
	
    if (ret==RTNORM) 
		{
        Enableflag=TRUE;
        do 
			{
            if (NULL==(fgets(string,IC_ACADBUF-1,stream))) break;
            if ((fcp1=strchr(string,'\n'/*DNT*/))!=NULL) *fcp1=0;
            Applistbox->AddString(string);
			} while(!feof(stream));
		}
    fclose(stream);


	//Now that we have added all the Lisp and SDS apps, add the COM Add-Ins.
	AddinHelper::GetNumberOfAddins (&nCount);
	if (nCount)
		Enableflag = TRUE;
	for (i = 1; i <= nCount; i++)
		{
		AddinHelper::GetAddinNameAndStatus (i, &Name, &bLoaded);
		Applistbox->AddString(Name);
		}

    // Set the state of the buttons on startup.
    cbutton=(CButton *)GetDlgItem(APPLOAD_UPDATE);
    cbutton->SetCheck(bupdate);
    if (Enableflag) 
		{
		//        Applistbox->SetSel(0);
		
        Applistbox->GetText(0,string);
        if (NULL==(fcp1=strrchr(string,'.'))) 
			goto done;
		
        if (strisame(fcp1+1,"dll"/*DNT*/)) 
			{
            rbt = sds_loaded();
			
            for(rbb=rbt;rbb!=NULL;rbb=rbb->rbnext) 
				{
                if (NULL==(fcp1=strrchr(string,'\\'/*DNT*/))) 
					continue;
				
                // find the application name in the path string. If the application name is the same enable the unload button.
                if (strstr(rbb->resval.rstring,fcp1+1)) 
					{
                    cbutton=(CButton *)GetDlgItem(APPLOAD_UNLOAD); 
					cbutton->EnableWindow(TRUE);
                    break;
					}
				}
			}
			//deal with the Addins
			{
			if (nCount)
				{
				AddinHelper::GetAddinNameAndStatus (1, &Name, &bLoaded);
				if (bLoaded)
					{
					cbutton=(CButton *)GetDlgItem(APPLOAD_UNLOAD); 
					cbutton->EnableWindow(TRUE);
					}
				}
			}
		
		} 
	else 
		{
        cbutton=(CButton *)GetDlgItem(APPLOAD_UNLOAD); cbutton->EnableWindow(FALSE);
        cbutton=(CButton *)GetDlgItem(APPLOAD_REMOVE); cbutton->EnableWindow(FALSE);
        cbutton=(CButton *)GetDlgItem(APPLOAD_LOAD);   cbutton->EnableWindow(FALSE);
		}
	
done:
	sds_relrb(rbt);
	delete [] string;
	return(TRUE);
	}

////////////////////////////////////////////////////////////////////////////////////
//
// OnOK()
//
// This function is called when the ok button is selected, checks if the save
// button has been checked, and if so then save the list of items.
//
// returns: void.
//
void APPLOAD_LoadDlg::OnOK() 
	{
    FILE    *stream=NULL;
    CButton *cbutton;
    int      icount, fi1;
    _TCHAR  *string=NULL;
	
    // Get the pointer to the given control.
    cbutton=(CButton *)GetDlgItem(APPLOAD_UPDATE);
	
    // Get the state of the check button.
    bupdate=(BOOL)cbutton->GetState();
	
    // If TRUE then save the list of
    if (bupdate) 
		{
        if (NULL==(string=(char *)new char[IC_ACADBUF])) 
			goto done;
		
        if ((sds_findfile(ICAD_APPLOAD,string))!=RTNORM) 
			goto done;
		
        if (NULL==(stream=fopen(string,"w+"/*DNT*/))) 
			goto done;
		
        icount=Applistbox->GetCount();
		
        for(fi1=0;fi1<icount;++fi1) 
			{
            Applistbox->GetText(fi1,string);
	 		if (!strnisame (string, AddinPrefix, AddinPrefix.GetLength()))	//if it is COM ADD-in - do not save it
				{
				fprintf(stream,"%s"/*DNT*/,string);
				if ((fi1+1)<icount) 
					fprintf(stream,"\n"/*DNT*/);
				}
			}
		}
done:
    if (stream) fclose(stream);
    if (string) { delete [] string; }
	
	CDialog::OnOK();
	}

// ======================================================================================================================================================================
// ====================================================== CMAINWINDOW CLASS FUNCTION ====================================================================================
// ======================================================================================================================================================================

////////////////////////////////////////////////////////////////////////////////////
//
// IcadAppLoader()
//
// This function is called by the mainwindow procedure, creates a modal dialog
// and loads the (dll's) or (lisp) files when the modal dialog terminates.
//
// returns: void.
//
void CMainWindow::IcadAppLoader(void) 
	{
    APPLOAD_LoadDlg *AppDlg;
    static BOOL      updatevar=1;
    int              iResult;
    //_TCHAR          *fcp1,
    //                *fcp2,
    _TCHAR          *fcp3;
	char            fs1[IC_ACADBUF];
    short           fi1,
		fi2;
	
    // Create new APPLOAD_LoadDlg class.
    AppDlg = new APPLOAD_LoadDlg();
    AppDlg->bupdate=updatevar;
    AppDlg->stringarray=NULL;
    AppDlg->cnt=0;

    // Call the dialog modal function.
    iResult=AppDlg->DoModal();

    // If loading files iResult will be (2).
    if (iResult==2) 
		{
        // Parse through string and load all (lsp) or (dll's).
        for(fi2=0;fi2<AppDlg->cnt;++fi2) 
			{
            if (NULL==(fcp3=strrchr(AppDlg->stringarray[fi2],'.'/*DNT*/))) 
				continue;

			// EBATECH(CNBR) -[ 2002/4/30 IntelliCAD can load lsp and mnl.
            //if (strisame(fcp3+1,"lsp"/*DNT*/)) 
            if (strisame(fcp3+1,"lsp"/*DNT*/)||strisame(fcp3+1,"mnl"/*DNT*/))
            // EBATECH(CNBR) ]-
				{
				sprintf(fs1,"(LOAD \"%s\")"/*DNT*/,AppDlg->stringarray[fi2]);
				for(fi1=0; fs1[fi1]!=0; fi1++)
					if(fs1[fi1]=='\\'/*DNT*/)
						fs1[fi1]='/';
					// EBATECH(R.Arayashiki) ]- for MBCS String
					else if ((_MBC_LEAD ==_mbbtype((unsigned char)fs1[fi1],0)) &&
						  (_MBC_TRAIL==_mbbtype((unsigned char)fs1[fi1+1],1)))
						fi1 ++;
					// ]- EBATECH
                sds_printf(ResourceString(IDC_ICADAPPLOAD__NLOADING___14, "\nLoading %s" ),AppDlg->stringarray[fi2]);
				SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)fs1);
                SDS_CMainWindow->m_IgnoreLastCmd=true;
				} 
			else if (strisame(fcp3+1,"dll"/*DNT*/)) 
				{
				sprintf(fs1,"(XLOAD \"%s\")"/*DNT*/,AppDlg->stringarray[fi2]);
				for(int fi1=0; fs1[fi1]!=0; fi1++)
					if(fs1[fi1]=='\\'/*DNT*/)
						fs1[fi1]='/'/*DNT*/;
					// EBATECH(R.Arayashiki) ]- for MBCS String
					else if ((_MBC_LEAD ==_mbbtype((unsigned char)fs1[fi1],0)) &&
						  (_MBC_TRAIL==_mbbtype((unsigned char)fs1[fi1+1],1)))
						fi1 ++;
					// ]- EBATECH
                sds_printf(ResourceString(IDC_ICADAPPLOAD__NLOADING___14, "\nLoading %s" ),AppDlg->stringarray[fi2]);
				SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)fs1);
                SDS_CMainWindow->m_IgnoreLastCmd=true;
				}
			}
		}

    // Set the global updatevar to the local updatevar.
    AppDlg->DeleteString();
    updatevar=AppDlg->bupdate;
    delete AppDlg;
	}


