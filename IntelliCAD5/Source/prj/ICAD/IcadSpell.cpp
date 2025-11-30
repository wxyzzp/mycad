/* D:\DEV\PRJ\ICAD\ICADSPELL.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

//*********************************************************************
//*  IcadSpell.cpp													  *
//*  Copyright (C) 1994,1995, 1996, 1997 by Visio, Inc.               *
//*                                                                   *
//*********************************************************************

//** Includes
#include "Icad.h"/*DNT*/
#include "IcadApi.h"
#include "IcadSpell.h"/*DNT*/
#include "IcadPrivateGuids.h"/*DNT*/
#include "resource.hm"/*DNT*/
#include "paths.h"
#include "configure.h"
#include "IcadApp.h"
#include "LicensedComponents.h"
#include "cmds.h"/*DNT*/


void CMainWindow::SpellCheck(sds_name ssSpellCheck)
{
#ifndef BUILD_WITH_WORD_SPELLCHECK
	#ifndef BUILD_WITH_INSO_SPELLCHECK
		return;
	#endif //BUILD_WITH_INSO_SPELLCHECK
#endif //BUILD_WITH_WORD_SPELLCHECK

	if(!CIcadApp::m_LicensedComponents.IsAccessible(CLicensedComponents::Components::SPELLCHECK))
	{
		cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED, 0);
		return;
	}

	HRESULT hr = NOERROR;
	CVSpell vSpell;

	if(m_piVSpell==NULL)
	{
		hr = CoInitialize(NULL);

		if(FAILED(hr))
		{
			MessageBox(	ResourceString(IDC_ICADSPELL_TO_MAKE_SURE_T_1, "The Spell Checker failed to initialize.  Please check the installation\nto make sure the Spell Checker was installed correctly (1)." ),ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD" ),
						MB_OK | MB_ICONEXCLAMATION);
			return;
		}

		hr = E_FAIL;

#ifdef BUILD_WITH_WORD_SPELLCHECK

		//*** Try to load the Microsoft Word spell checker first.
		hr = CoCreateInstance(CLSID_CCSAPISpell,		//CLSID associated with the class object
				   			  NULL,						//controlling unknown
							  CLSCTX_INPROC_SERVER,		//Context for running executable code
							  IID_IVSpell,				//Interface identifier
							  (void **)&m_piVSpell);	//Interface pointer
		if(hr==S_OK)
		{
			vSpell=m_piVSpell;
			hr = vSpell.Init(TRUE,											//Ignore words with numbers
							 MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),	//Language
							 NULL,											//NULL for the Word speller
							 NULL);											//NULL for Word
		}
#endif //BUILD_WITH_WORD_SPELLCHECK

		if(FAILED(hr))
		{
#ifdef BUILD_WITH_INSO_SPELLCHECK
			//*** If the Word speller failed, try INSO's.
			hr = CoCreateInstance(CLSID_CICSpell,			//CLSID associated with the class object
								  NULL,						//controlling unknown
								  CLSCTX_INPROC_SERVER,		//Context for running executable code
								  IID_IVSpell,				//Interface identifier
								  (void **)&m_piVSpell);	//Interface pointer

			if(FAILED(hr))
			{
				MessageBox(	ResourceString(IDC_ICADSPELL_TO_MAKE_SURE_T_3, "The Spell Checker failed to initialize.  Please check the installation\nto make sure the Spell Checker was installed correctly (2)." ),ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD" ),
							MB_OK | MB_ICONEXCLAMATION);
				return;
			}

			//*** Initialize the INSO dictionary.
			vSpell=m_piVSpell;

			DWORD dwSizeEngine=0,dwSizeDict=0;
			if(0!=(dwSizeEngine=SearchPath(NULL,"Vspell32.dll"/*DNT*/,NULL,0,NULL,NULL)) &&
				0!=(dwSizeDict=SearchPath(NULL,ResourceString(IDC_ICADSPELL_VSPL_IE_DAT_4, "Vspl_ie.dat" ),NULL,0,NULL,NULL)))
			{
				_TCHAR* pszEngine = new _TCHAR[dwSizeEngine];
				SearchPath(NULL,"Vspell32.dll"/*DNT*/,NULL,dwSizeEngine,pszEngine,NULL);
				_TCHAR* pszDict = new _TCHAR[dwSizeDict];
				SearchPath(NULL,ResourceString(IDC_ICADSPELL_VSPL_IE_DAT_4, "Vspl_ie.dat" ),NULL,dwSizeDict,pszDict,NULL);

				hr = vSpell.Init(TRUE,											//Ignore words with numbers
								 MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),	//Language
								 pszEngine,										//INSO engine, This will be NULL for the Word speller
								 pszDict);										//INSO English dictionary, This will be NULL for Word

				delete pszEngine;
				delete pszDict;

				if(FAILED(hr))
				{
					//*** If every attempt failed, fail.
					MessageBox(	ResourceString(IDC_ICADSPELL_TO_MAKE_SURE_T_5, "The Spell Checker failed to initialize.  Please check the installation\nto make sure the Spell Checker was installed correctly (3)." ),ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD" ),
								MB_OK | MB_ICONEXCLAMATION);
					m_piVSpell->Release();
					m_piVSpell=NULL;
					return;
				}
			}
			else
			{
				MessageBox(	ResourceString(IDC_ICADSPELL_A_DICTIONARY_W_6, "A dictionary was not found.  Please check the installation\nto make sure a dictionary was installed." ),ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD" ),
							MB_OK | MB_ICONEXCLAMATION);
				m_piVSpell->Release();
				m_piVSpell=NULL;
				return;
			}
#else
			return;
#endif //BUILD_WITH_INSO_SPELLCHECK
		}
	}
	else vSpell=m_piVSpell;

	IcadSpellDlg dlgSpell(this);
	dlgSpell.SpellSelSet(ssSpellCheck,&vSpell);

	return;
}

/////////////////////////////////////////////////////////////////////////////
// IcadSpellDlg dialog

IcadSpellDlg::IcadSpellDlg(CWnd* pParent /*=NULL*/)
	: IcadDialog(IcadSpellDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(IcadSpellDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_nSpellMode=SPL_SELSET; //*** Default spelling mode.
	m_strSentence.Empty();
	m_bSentenceChanged=FALSE;
}


void IcadSpellDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX,SPL_EDITNOTINDICT,m_strWord);
	DDX_Text(pDX,SPL_EDITCHANGETO,m_strChangeTo);
	//{{AFX_DATA_MAP(IcadSpellDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

void IcadSpellDlg::SpellSelSet(sds_name ssSpellCheck, CVSpell* pvSpell)
{
//*** This function spell checks a selection set.
	m_pvSpell=pvSpell;
	ic_sscpy(m_ssSpellCheck,ssSpellCheck);
	m_nSpellMode=SPL_SELSET;
	m_strSentence.Empty();
	m_bSentenceChanged=FALSE;
	//*** See OnInitDialog for the rest of the initialization code.
	if(DoModal()!=IDCANCEL)
	{
		sds_alert(ResourceString(IDC_ICADSPELL_THE_SPELLING_C_8, "The spelling check is complete." ));
	}
	return;
}

void IcadSpellDlg::GetNextFromSelSet()
{
	struct sds_resbuf* elist;
	sds_name ename;
	HRESULT hr = NOERROR;
	WORD wResult = VSPL_NOERR;
	int ret,nLen;
	BOOL bIsMtext = FALSE;
	BOOL bIsValidType = FALSE;
	struct sds_resbuf* pRbTmp = NULL;

	for(;;)
	{
		if(m_strSentence.IsEmpty() ||
			(m_idxFirstChar+=(m_nCount+m_nStep))>=(nLen=m_strSentence.GetLength()))
		{
			if(m_bSentenceChanged==TRUE)
			{
				SDS_SetUndo(IC_UNDO_COMMAND_BEGIN,(void *)"SPELL"/*DNT*/,NULL,NULL,SDS_CURDWG);
				//*** If we are at the end of a sentence, update the entity.
				sds_name ename;
				if(sds_ssname(m_ssSpellCheck,m_ssct,ename)==RTNORM)
				{
					bIsMtext=FALSE;
					bIsValidType=FALSE;
					struct sds_resbuf* elist;
					elist=sds_entget(ename);
					if(ic_assoc(elist,0)==0)
					{
						if(strisame("TEXT"/*DNT*/,ic_rbassoc->resval.rstring))
						{
							bIsValidType=TRUE;
						}
						if(strisame("MTEXT"/*DNT*/,ic_rbassoc->resval.rstring))
						{
							bIsMtext=TRUE;
							bIsValidType=TRUE;
						}
					}
					if(bIsValidType)
					{
						nLen=m_strSentence.GetLength();
						if(bIsMtext)
						{
							//*** Remove the 3 groups and put the entire string in the
							//*** 1 group, sds_entmod() will add the right 3 groups.
							struct sds_resbuf *pRbBeg = NULL, *pRbPrev = NULL;
							for(pRbTmp=elist; pRbTmp!=NULL; pRbTmp=pRbTmp->rbnext)
							{
								if(pRbTmp->restype==3)
								{
									if(NULL==pRbBeg)
									{
										//*** Set the beg pointer to the first 3 group.
										pRbBeg=pRbPrev;
									}
								}
								else
								{
									if(pRbBeg!=NULL)
									{
										pRbPrev->rbnext=NULL;
										IC_RELRB(pRbBeg->rbnext);
										pRbBeg->rbnext=pRbTmp;
										break;
									}
								}
								pRbPrev=pRbTmp;
							}
						}
						if(ic_assoc(elist,1)==0 && ic_rbassoc->resval.rstring!=NULL)
						{
							db_astrncpy(&ic_rbassoc->resval.rstring,
								m_strSentence.GetBuffer(0),nLen+1);
						}
						sds_entmod(elist);
					}
					IC_RELRB(elist);
				}
				m_bSentenceChanged=FALSE;
				SDS_SetUndo(IC_UNDO_COMMAND_END,(void *)"SPELL"/*DNT*/,NULL,NULL,SDS_CURDWG);
			}
			//*** Get the next entity in the selection set to spell check.
			for(m_ssct++; (ret=sds_ssname(m_ssSpellCheck,m_ssct,ename))==RTNORM; m_ssct++)
			{
				bIsMtext=FALSE;
				elist=sds_entget(ename);
				if(ic_assoc(elist,0)==0)
				{
					if(strisame("TEXT"/*DNT*/,ic_rbassoc->resval.rstring))
					{
						break;
					}
					if(strisame("MTEXT"/*DNT*/,ic_rbassoc->resval.rstring))
					{
						bIsMtext=TRUE;
						break;
					}
				}
				IC_RELRB(elist);
			}
			if(ret!=RTNORM)
			{
				EndDialog(IDOK);
				break;
			}
			nLen=0;
			m_strSentence.Empty();
			if(bIsMtext)
			{
				//*** Append the 3 groups onto the string.
				for(pRbTmp=elist; pRbTmp!=NULL; pRbTmp=pRbTmp->rbnext)
				{
					if(pRbTmp->restype==3)
					{
						m_strSentence+=pRbTmp->resval.rstring;
					}
				}
			}
			if(ic_assoc(elist,1)==0 && ic_rbassoc->resval.rstring!=NULL)
			{
				m_strSentence+=ic_rbassoc->resval.rstring;
			}
			nLen=m_strSentence.GetLength();
			//*** Step over any leading spacers.
			for(m_idxFirstChar=0; m_idxFirstChar<nLen && m_strSentence[m_idxFirstChar]==' '; m_idxFirstChar++);
			IC_RELRB(elist);
		}
		for(m_nCount=0; ((m_idxFirstChar+m_nCount)<nLen); m_nCount++)
		{
			if(m_strSentence[m_idxFirstChar+m_nCount]==' ')
			{
				m_nStep=1;
				break;
			}
			if((m_strSentence[m_idxFirstChar+m_nCount]=='\\' &&
				((m_idxFirstChar+m_nCount+1)<nLen) &&
				m_strSentence[m_idxFirstChar+m_nCount+1]=='P'))
			{
				m_nStep=2;
				break;
			}
		}
		m_strWord=m_strSentence.Mid(m_idxFirstChar,m_nCount);

		//*** Check spelling of m_strWord.
		hr = m_pvSpell->Spell(m_strWord.GetBuffer(0),&wResult);
		if(wResult==VSPL_NOTINDIC)
		{
			//*** We have a possible misspelled word
			CHAR szSuggestions[256];
			DWORD dwNumSuggestions = 0;
			//*** Set the Change to string.
			m_strChangeTo=m_strWord;
			//*** Set the suggestion list.
			hr = m_pvSpell->Suggest(m_strWord.GetBuffer(0),szSuggestions,&dwNumSuggestions,sizeof(szSuggestions),&wResult);
			CListBox* pListBox = (CListBox*)GetDlgItem(SPL_LISTSUGGEST);
			if(dwNumSuggestions)
			{
				if(pListBox!=NULL)
				{
					CString str;
					UINT ct;
					int idxChar;
					pListBox->EnableWindow();
					pListBox->ResetContent();
					for(ct=0,idxChar=0; ct<dwNumSuggestions; ct++,idxChar++)
					{
						str=(szSuggestions+idxChar);
						pListBox->AddString(str);
						while(szSuggestions[idxChar]!=0) idxChar++;
					}
					pListBox->SetCurSel(0);
				}
				//*** Set the Change to string to the highlighted list item.
				pListBox->GetText(0,m_strChangeTo);
			}
			else if(pListBox!=NULL)
			{
				pListBox->ResetContent();
				pListBox->AddString(ResourceString(IDC_ICADSPELL__NO_SUGGESTIO_10, "(No Suggestions)" ));
				pListBox->EnableWindow(FALSE);
			}
			UpdateData(FALSE);
			//*** Highlight the suggestion.
			CEdit* pEdit = (CEdit*)GetDlgItem(SPL_EDITCHANGETO);
			if(pEdit!=NULL)
			{
				pEdit->SetFocus();
				pEdit->SetSel(0,-1,TRUE);
			}
			break;
		}
	}

	return;
}

BEGIN_MESSAGE_MAP(IcadSpellDlg, CDialog)
	//{{AFX_MSG_MAP(IcadSpellDlg)
	ON_BN_CLICKED(SPL_BTNIGNORE, OnIgnore)
	ON_BN_CLICKED(SPL_BTNCHANGE, OnChange)
	ON_LBN_SELCHANGE(SPL_LISTSUGGEST, OnSelChangeSuggestList)
	ON_LBN_DBLCLK(SPL_LISTSUGGEST, OnChange)
	ON_BN_CLICKED(SPL_BTNIGNOREALL, OnIgnoreAll)
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDHELP,OnHelp)
END_MESSAGE_MAP()

void IcadSpellDlg::OnHelp() {
	CommandRefHelp( m_hWnd, HLP_SPELL);
}

/////////////////////////////////////////////////////////////////////////////
// IcadSpellDlg message handlers

BOOL IcadSpellDlg::OnInitDialog()
{
	IcadDialog::OnInitDialog();

	if(m_nSpellMode==SPL_SELSET)
	{
		//*** Gray out the buttons that are not hooked up yet.
		CWnd* pWnd = GetDlgItem(SPL_BTNCHANGEALL);
		if(pWnd!=NULL) pWnd->EnableWindow(FALSE);
		pWnd = GetDlgItem(SPL_BTNADD);
		if(pWnd!=NULL) pWnd->EnableWindow(FALSE);
		pWnd = GetDlgItem(SPL_BTNOPTIONS);
		if(pWnd!=NULL) pWnd->EnableWindow(FALSE);
//		pWnd = GetDlgItem(IDHELP);
//		if(pWnd!=NULL) pWnd->EnableWindow(FALSE);

		m_ssct=(-1L);
		GetNextFromSelSet();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void IcadSpellDlg::OnIgnore()
{
	UpdateData(TRUE);
	if(m_nSpellMode==SPL_SELSET)
	{
		//*** Get the next misspelled word.
		GetNextFromSelSet();
	}
}

void IcadSpellDlg::OnChange()
{
	UpdateData(TRUE);
	if(m_nSpellMode==SPL_SELSET)
	{
		//*** Add the misspelled word to the sentence.
		CString str;
		str=m_strSentence.Left(m_idxFirstChar);
		str+=m_strChangeTo;
		str+=m_strSentence.Right(m_strSentence.GetLength()-(m_idxFirstChar+m_nCount));
		m_nCount=m_strChangeTo.GetLength();
		m_strSentence=str;
		m_bSentenceChanged=TRUE;
		//*** Get the next misspelled word.
		GetNextFromSelSet();
	}
}

void IcadSpellDlg::OnSelChangeSuggestList()
{
	//*** Update the change to string.
	CListBox* pListBox = (CListBox*)GetDlgItem(SPL_LISTSUGGEST);
	if(pListBox==NULL) return;
	pListBox->GetText(pListBox->GetCurSel(),m_strChangeTo);
	UpdateData(FALSE);
	//*** Highlight the contents of the Change to editbox.
	CEdit* pEdit = (CEdit*)GetDlgItem(SPL_EDITCHANGETO);
	if(pEdit==NULL) return;
	pEdit->SetFocus();
	pEdit->SetSel(0,-1,TRUE);
}

void IcadSpellDlg::OnIgnoreAll()
{
	WORD wResult = VSPL_NOERR;
	m_pvSpell->IgnoreAll(m_strWord.GetBuffer(0),&wResult);
	OnIgnore();
}


