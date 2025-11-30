/* D:\DEV\PRJ\ICAD\ICADMLEDITDIA.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

//*********************************************************************
//*  IcadMLEditDia.cpp                                                *
//*  Copyright (C) 1997 by Visio, Inc.								  *
//*                                                                   *
//*********************************************************************

//** Includes
#include "Icad.h"/*DNT*/
#include "IcadApi.h"
#include "cmds.h"/*DNT*/
#include "paths.h"
#include "IcadDialog.h"
#include "IcadHelp.h"

class ml_CMLEdit:public IcadDialog {
    public:
        ml_CMLEdit();
        BOOL OnInitDialog();
		int nMLEditType;
    protected:
        afx_msg void OnHelp();
        virtual void DoDataExchange(CDataExchange *pDX);

    DECLARE_MESSAGE_MAP()
};


BEGIN_MESSAGE_MAP(ml_CMLEdit,CDialog)
    ON_BN_CLICKED(IDHELP,OnHelp)
END_MESSAGE_MAP()

void ml_CMLEdit::OnHelp() {
    char fs1[IC_ACADBUF];

    if( !SearchHelpFile( SDS_COMMANDHELP, fs1) ) {
		MessageBox(ResourceString(IDC_ICADMLEDITDIA_UNABLE_TO__0, "Unable to find the requested help file." ),NULL,MB_OK|MB_ICONEXCLAMATION);
        return;
    }
	CMainWindow::IcadHelp( fs1,ICADHELP_CONTENTS,0);
}

ml_CMLEdit::OnInitDialog() {

    BOOL ret=IcadDialog::OnInitDialog();

    return(ret);
}

void ml_CMLEdit::DoDataExchange(CDataExchange *pDX) {
    CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX,ML_CLOSEDCROSS,nMLEditType);
}

ml_CMLEdit::ml_CMLEdit():IcadDialog(ML_MULTILINEEDIT) {
	nMLEditType=0;
}

//***This is the main window
void CMainWindow::IcadMLEditDia(void) {
    int ret;

    ml_CMLEdit m_CMLEdit;
    ret=m_CMLEdit.DoModal();
	if (ret==IDOK) {
		if ((m_CMLEdit.nMLEditType>-1)&&(m_CMLEdit.nMLEditType<12)) {  // 0-11 are valid numbers
			cmd_mledit_prompts(m_CMLEdit.nMLEditType);
		} else {
			// What error handling is needed here?
			// No matter what this returns, the dialog gets destroyed.  Right?
		}
	}
}



