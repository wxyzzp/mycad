/* D:\DEV\PRJ\ICAD\ICADINSNAME.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

//*********************************************************************
//*  IcadInsName.cpp                                                  *
//*  Copyright (C) 1994, 1995, 1996, 1997, 1998 by Visio, Inc.        *
//*                                                                   *
//*********************************************************************

//** Includes
#include "Icad.h"/*DNT*/


class ins_Name:public CDialog{
    public:
        CString csTitle,csMessage;
        ins_Name();
        CString csName;
        BOOL OnInitDialog();
    protected:
        virtual void DoDataExchange(CDataExchange *pDX);

    DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(ins_Name,CDialog)
END_MESSAGE_MAP()    

BOOL ins_Name::OnInitDialog() {
    CDialog::OnInitDialog();
    ::SetWindowText(m_hWnd,csTitle);
    ::SetWindowText(::GetDlgItem(m_hWnd,INS_NAME_MESS),csMessage);
    return(TRUE);
}

void ins_Name::DoDataExchange(CDataExchange *pDX) {
    struct resbuf *tmprb=NULL;
    
    DDX_Text(pDX,INS_NAME,csName);
    if(pDX->m_bSaveAndValidate){
        if(!ic_isvalidname(csName.GetBuffer(csName.GetLength()+1))){    
			CString	str;	/*D.G.*/
			str.FormatMessage(ResourceString(IDC_WRONG_NAME_MESSAGE, "The name can use up to %1!d! characters, including letters, numbers,\nhyphens, underscores, and dollar signs ($), but not spaces.\nPlease try again."), IC_ACADNMLEN - 1);
            MessageBox((LPCTSTR)str, ResourceString(CMD_ERR_INVALIDNAME, "Invalid Name Entered"), MB_ICONERROR | MB_APPLMODAL | MB_OK);
            pDX->Fail();
        }else if((tmprb=sds_tblsearch("BLOCK"/*DNT*/,csName.GetBuffer(IC_ACADNMLEN+1),0))!=NULL){
            IC_RELRB(tmprb);
            MessageBox(ResourceString(IDC_ICADINSNAME_YOU_HAVE_ENT_2, "You have entered a block name that already exists in the drawing. Please enter a unique name for this block." ),ResourceString(IDC_ICADINSNAME_DUPLICATE_BL_3, "Duplicate Block Name" ),MB_ICONERROR|MB_APPLMODAL|MB_OK);
            pDX->Fail();
        }
    }
}

ins_Name::ins_Name():CDialog(INS_NAME_ERR){
}




//***This is the main 
void CMainWindow::IcadNameDia(int mode) {
    //mode 
    //    0 = not a valid block
    //    1 = a block already defined with that name
    
    
    ins_Name m_Name;
    switch(mode){
        case 0:
            m_Name.csTitle=  ResourceString(IDC_ICADINSNAME_BLOCK_NAME_E_4, "Block Name Error" );
            m_Name.csMessage.FormatMessage(ResourceString(IDC_ICADINSNAME_THE_BLOCK_NA_5, "The block name can use up to %1!d! characters, including letters, numbers,\nhyphens, underscores, and dollar signs ($), but not spaces.  Please enter a new name for the block."), IC_ACADNMLEN - 1);
            break;
        case 1:
            m_Name.csTitle=  ResourceString(IDC_ICADINSNAME_DUPLICATE_BL_3, "Duplicate Block Name" );
            m_Name.csMessage=ResourceString(IDC_ICADINSNAME_YOU_HAVE_ENT_6, "You have entered an xref name that already exists in the drawing.  Please enter a unique name for this xref." );
            break;
        case 2:
            m_Name.csTitle=  ResourceString(IDC_ICADINSNAME_DUPLICATE_BL_3, "Duplicate Block Name" );
            m_Name.csMessage=ResourceString(IDC_ICADINSNAME_YOU_HAVE_ENT_7, "You have entered a block name that already exists in the drawing.  Please enter a unique name for this block." );
            break;
        case 3:
            m_Name.csTitle=  ResourceString(IDC_ICADINSNAME_BLOCK_NAME_E_4, "Block Name Error" );
            m_Name.csMessage=ResourceString(IDC_ICADINSNAME_THE_BLOCK_NA_8, "The block name entered is the same as the name of the current drawing.  Please enter a new name for the block." );
            break;
    }
    m_Name.DoModal();
    SDS_CMainWindow->m_strWndAction=m_Name.csName;
}

