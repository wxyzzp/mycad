/** FILENAME:     ListBox.cpp- Contains implementation of class CListBox.
* Copyright  (C) Copyright 1998-99  Visio Corporation.  All Rights Reserved.
*
*
*
* Abstract
*	This class Maps the basic tile list
*	to windows control listbox
*
*
*Audit Log
*
*
*Audit Trail
*
*
*/


#include "stdafx.h"
#include "ListBox.h"
#include "dialog.h"
#include "sds.h"
#include "dclprivate.h"
#include "constants.h"

#include "ItcLanguage.h" //EBATECH

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/************************************************************************************
/* + ListBox::ListBox -
*
*
*
*
*
*/

ListBox::ListBox()
	{
	}
/************************************************************************************
/* + ListBox::ListBox(ParentAlignmen -
*
*
*
*
*
*/

ListBox::ListBox(int ParentAlignment) // alignment of parent control
	{
	m_ParentAlignment = ParentAlignment;
	}
/************************************************************************************
/* + ListBox::~ListBox -
*
*
*
*
*
*/

ListBox::~ListBox()
	{
	}


BEGIN_MESSAGE_MAP(ListBox, CListBox)
	//{{AFX_MSG_MAP(ListBox)
	ON_WM_LBUTTONDBLCLK()
	ON_CONTROL_REFLECT(LBN_SELCHANGE, OnSelchange)
	ON_CONTROL_REFLECT(LBN_DBLCLK, OnDblclk)
	ON_CONTROL_REFLECT(LBN_SETFOCUS, OnSetfocus)
	ON_CONTROL_REFLECT(LBN_KILLFOCUS, OnKillfocus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/************************************************************************************
/* + int ListBox:: CreateOnScreenObj (CWnd* pParent)
*
*	creates the windows UI control
*
*
*
*
*/

int ListBox:: CreateOnScreenObject (
									CWnd* pParent // i:pointer to the parent window
									)
	{
	int error;
	DWORD dwStyle = WS_CHILD|WS_VISIBLE|WS_BORDER|WS_VSCROLL|WS_HSCROLL|LBS_USETABSTOPS|LBS_NOTIFY|WS_GROUP ; // windows styles for control
	ValueString = m_attriblist->get_ValueString();
	if (!m_attriblist->IsEnabled())
		{
		// set style to enable
		dwStyle = dwStyle|WS_DISABLED;
		}
	if (m_attriblist->IsTabstop())
		{
		// set style to tabstop
		dwStyle = dwStyle|WS_TABSTOP;
		}
	if(m_attriblist->IsMultipleSelect())
		{
		dwStyle = dwStyle|LBS_EXTENDEDSEL   ; //LBS_MULTIPLESEL;
		}

	CRect rectlist, rectstatic ;


	// EBATECH(CNBR) 2001-05-05 Fontface needs localize. -[
	CString wszResourceString;
    // Check for fixed_width_font

	if ( m_attriblist->get_FixedWidthFont() == false )
		{
		// If fixed_font_width is false.
		wszResourceString.LoadString(ID_DCL_DEFAULT_FONTFACE);   // Load the resource string.
		m_ComponentFont->CreatePointFont(DIALOG_FONT_POINTSIZE,wszResourceString,NULL); // create font
		}
    else
        {
        // If fixed_font_width is true.
		wszResourceString.LoadString(ID_DCL_FIXED_PITCH_FONTFACE);   // Load the resource string.
		m_ComponentFont->CreatePointFont(DIALOG_FONT_POINTSIZE,wszResourceString); // create font
		}

	// EBATECH(CNBR) ]-

/*  LOGFONT LogFont;

	LogFont.lfHeight = 80;
	LogFont.lfWidth = 0;
	LogFont.lfEscapement = 10;
	LogFont.lfOrientation = 10;
	LogFont.lfWeight = FW_DONTCARE;
	LogFont.lfItalic  = FALSE;
	LogFont.lfUnderline  = FALSE;
	LogFont.lfStrikeOut  = FALSE;
	LogFont.lfCharSet =  DEFAULT_CHARSET ;
	LogFont.lfOutPrecision = OUT_TT_PRECIS ;
	LogFont.lfClipPrecision  = CLIP_DEFAULT_PRECIS  ;
	LogFont.lfQuality = DEFAULT_QUALITY;
	LogFont.lfPitchAndFamily = FF_DONTCARE | FIXED_PITCH;
    _tcscpy(LogFont.lfFaceName, _T("Fixedsys")); */
	//m_ComponentFont->CreatePointFontIndirect(80,_T("Fixedsys") /*&LogFont*/); // create font


	rectstatic = rectlist = PositionControl();						// get the display position for control
	if(!m_attriblist->get_Label().IsEmpty())		// check for label if found add Stztic control to list
		{
		rectlist.top += HIWORD(GetDialogBaseUnits())-2;
		CString Label =SetMnemonic() ;
		//Modified Cybage SBD 30/03/2001 DD/MM/YYYY
		//mail from Ronald Prepchuk dated 21/03/2001 Sub:FW: Last-minute code submission.
		rectstatic.bottom=rectlist.top;  // CadPLUS fix 9-20-00 PREVENTS
		//REPAINTING OF LIST WITH GRAY
		n_Title.Create(Label,WS_CHILD|WS_VISIBLE,rectstatic,pParent); // create static control
		n_Title.SetFont(m_ComponentFont,TRUE);											  // set font of static control
		}

	error = Create(dwStyle, rectlist, pParent, m_controlID.GetID()); // creates list control
	if (error)
		{
		SetFont(m_ComponentFont,TRUE);						// set font for list control
		CString Value = m_attriblist->get_List();
		CString tempstr ;

		// fill the list with the attribute list
		CString tabstring = m_attriblist->GetTabsList ();
		int index =0, position = 0,count;
		Value += ",";
		while((count =Value.Find(',')) > 0)
			{
			tempstr=Value.Mid(index,count);
			Value.Delete(index,count+1);
			if(position == 0)
				{
				if(m_attriblist->IstabTruncate() != SDS_RTERROR)
					{
					// if tabs truncate is set the the string is truncated , depending upon the tabs value set
					// only the frist string before the '\t' is truncated
					int len1;
					int len2 = tempstr.Find ('\t');
					if(len2>0)
						{
						tabstring += CString(" " );
						len1 = atoi(tabstring.Mid (0,tabstring.Find (' ')));
						if ( len1 <= 0 )
							{
							len1 = tempstr.GetLength() + 1 ;
							}
						if(tempstr.GetLength()> len1 )
							{
							tempstr.Delete((len1-1),(len2+1-len1));
							}
						}
					else
						{
						 len1 = atoi(tabstring.Mid (0,tabstring.Find (' ')));
						 if ( position == 0 && len1 == 0 )
							 {
							 int iPos = tempstr.Find('\t');
							 tempstr = tempstr.Mid(iPos+1);
						     TRACE("%s",tempstr);
							 }
						}
					}

				}

			InsertString(position++,tempstr);
			}


		if ( m_attriblist->IsMultipleSelect() == FALSE )
			{
			if(position>0)
				{
				int nSelPos = m_attriblist->get_ValueInt();
				if ( nSelPos >= 0 )
					{
					SetCurSel(nSelPos);
					}
				else
					{
					CString strSelIndex = m_attriblist->get_ValueString();
					if ( strSelIndex != _T("") )
						{
						int iIndex = atoi((LPCTSTR)strSelIndex);
						SetCurSel(iIndex);
						}
					}
				}
			}
		else
			{
			CString strSelIndex = m_attriblist->get_ValueString();

			if ( !strSelIndex.IsEmpty() )
				{
                int iPos2 = 0;
				for ( int iPos1 = 0; iPos1 < strSelIndex.GetLength() ; )
					{
					CString strIndex ;

					iPos2 = strSelIndex.Find(_T(' '),iPos1);

					if ( iPos2 == -1 )
						{
						strIndex = strSelIndex.Mid(iPos1);
						SetSel(atoi((LPCTSTR)strIndex),TRUE);
						break;
						}

					strIndex = strSelIndex.Mid(iPos1,iPos2);

					SetSel(atoi((LPCTSTR)strIndex),TRUE);

					iPos1 = ++iPos2;



					}

				}
			}
		// Set the Tabs Attributes
		int tabs[20];
		tabstring = m_attriblist->GetTabsList ();
		index =0, position = 0 ;
		tabstring.TrimLeft ();
		tabstring += CString(" ");
		// converts the string from dcl file to integer array
		// which is the parameter for MFC function to set tabs

		if(!tabstring.IsEmpty())
			{
			while((count =tabstring.Find(' ')) > 0)
				{
					tempstr=tabstring.Mid(index,count);
					// {{{ Gurudev - DT: 8/5/99 3:01:05 PM
					if ( atoi(tempstr) < 0 )
						break;
					// Gurudev - Dt:  8/5/99 3:01:08 PM }}}
					tabstring.Delete(index,count+1);
					tabs[position++]= atoi(tempstr)*4;

				}

			BOOL bTabSet = SetTabStops( position, tabs); // MFC function which sets Tabs is the list control

			}


		return SDS_RTNORM;
		}
	else
		return SDS_RSERR;
	}


/************************************************************************************
/* + int ListBox::ModeTile (int nTileState)
*
*	Executes functions like setting focus to a control , enabling or disabling a control etc.
*	nTileState = 0  then enable window
*	nTileState = 1	disable window
*	nTileState = 2  set Focus
*
*
*/

int ListBox::ModeTile ( // Executes functions like setting focus to a control , enabling or disabling a control etc.
		int nTileState	   // i:state of control
		)
	{
	CWnd * pWind;
	pWind = (CWnd *)(this);
    switch(nTileState)
		{
		case SDS_MODE_ENABLE:
//MB 30-12-1999
			if ( n_Title ) n_Title.EnableWindow();

			//pWind->EnableWindow();

			//Modified SAU 31/05/2000			[
			//Reason:List boxes are often not repainted
			pWind->SendMessage(WM_SETREDRAW,FALSE, 0L);
			pWind->EnableWindow();
			pWind->SendMessage(WM_SETREDRAW,TRUE, 0L);
			pWind->InvalidateRect(NULL, TRUE);
			//Modified SAU 31/05/2000			]

			break;
		case SDS_MODE_DISABLE:
//MB 30-12-1999
			if ( n_Title ) n_Title.EnableWindow(FALSE);

			//pWind->EnableWindow(FALSE);

			//Modified SAU 31/05/2000			[
			//Reason:List boxes are often not repainted
			pWind->SendMessage(WM_SETREDRAW,FALSE,0L);
			pWind->EnableWindow(FALSE);
			pWind->SendMessage(WM_SETREDRAW,TRUE,0L);
			pWind->InvalidateRect(NULL, TRUE);
			//Modified SAU 31/05/2000			]

			break;
		case SDS_MODE_SETFOCUS:
			pWind->SetFocus();
			break;
		default:
			TRACE ("WRONG PARAMETER IN MODE TILE");
			break;
		}
	return SDS_RTNORM;
	}
/************************************************************************************
/* + int ListBox:: GetTile (CString & szControlInfo,int lInfoMaxLength)
*
*	Returns the "value" attribute of the tile.
*
*
*
*/

int ListBox:: GetTile (			// Returns the "value" attribute of the tile
		CString & szControlInfo,// o:Date contaned in value attribute
		int lInfoMaxLength		// o: Size of data
		)
	{
	//  if size of data requested is greater then 0
	//  then get the text in the edit control and return
	if(lInfoMaxLength>0)
		{
		/* {{{ Gurudev - DT: 7/19/99 8:39:34 AM
				 szControlInfo = ValueString;
		Gurudev - Dt:  7/19/99 8:39:37 AM }}} */

		 //Modified SAU 31/05/2000  			[
		 //Reason:multiple_select list box do not return the proper string;
		if(m_attriblist->IsMultipleSelect()){
			char  stemp[5];
			int   nTotal	=GetSelCount();
//4M Item:8->
         if (nTotal==0){
            szControlInfo=CString("");
         }
         else{
//<-4M Item:8
            int*  striValue =new int[nTotal];
			char* strValue	=new char[(nTotal)*7];

			GetSelItems(nTotal,striValue);
			strcpy(strValue,"");

			for(int i=0;i<nTotal;i++){
				_itoa(striValue[i],stemp,10);
				strcat(strValue,stemp);
				strcat(strValue," ");
			}
			strValue[strlen(strValue)-1]='\0';
			szControlInfo=strValue;

			// Modified CyberAge PP 01/11/2000 [
			// Reason: sample.lsp/sample.dcl file when loaded & executed, ICAD hangs.
			// sample.lsp/sample.dcl contained in automatelisp.zip.
			// Reference: Email from Ron dated 11 Oct 2000.
			if(strValue  && nTotal > 0 )  delete strValue;
			if(striValue && nTotal > 0 )  delete striValue;
			// Modified CyberAge PP 01/11/2000 ]
//4M Item:8->
         }
//<-4M Item:8
        }
		else {
        //Modified SAU 31/05/2000  			]
	 	int nCurSel = GetCurSel();
				if ( nCurSel != LB_ERR )
					szControlInfo.Format("%d",nCurSel);
				else
					szControlInfo.Format("%s",_T(""));

	 	}
		return SDS_RTNORM ;
		}
	return SDS_RTERROR ;
	}

/************************************************************************************
/* + int ListBox::GetListString(CString& szListString,int* nStringLength,int nWhichItem)
*
*	this function returns the string given the index in the list control
*
*
*
*/

int ListBox::GetListString(
								CString& szListString, // o:Parameter to hold the return string
								int* nStringLength,	   // o:Size of the String
								int nWhichItem		   // i:item index in the listbox whose string is returned
								)
	{

	CString stringatindex;

	(static_cast<CListBox *>(this))-> GetText(nWhichItem,stringatindex); // MFC function to get the string at given index
	szListString = stringatindex;
	return SDS_RTNORM;

	}

/************************************************************************************
/* + int ListBox::AddList(CString szAddToList)
*	Adds a string to the list control.
*	this add list function depends upon the parameters (nChangeAddNew) in start list
*	nChangeAddNew == CHANGE -- removes the current string from the index and add the new string
*	nChangeAddNew == APPEND  -- adds a new string at the bottom of the list
*	nChangeAddNew == REPLACE -- adds the string at the specified index
*	nChangeAddNew == 3 -- the previous list is emptyed.
*
*/

int ListBox::AddList(
						  CString szAddToList // i:String to add
						  )
	{
	if(ListStarted == 1)
		{
		if(nChangeAddNew == 3)
					{
					int count =GetCount();

					// delete all strings from the list box
					for(int index  = 0; index < count ; index++)
						DeleteString(0);
					}
		ListStarted++;
		}
	switch(nChangeAddNew)
		{
	case CHANGE	:
		//removes the current string from the index and add the new string
		DeleteString(nWhichItem);
		InsertString(nWhichItem ,szAddToList);
		break;
	case  APPEND:
		// adds a new string at the bottom of the list
		AddString(szAddToList);
		break;
	case REPLACE:
		// adds the string at the specified index
		AddString(szAddToList);
		break;
		}
	return SDS_RTERROR;
	}

/************************************************************************************
/* + void ListBox::OnLButtonDblClk(UINT nFlags, CPoint point)
*	Handling the callbacks.
*
*
*
*
*/
void ListBox::OnLButtonDblClk(
							  UINT nFlags,
							  CPoint point
							  )
	{
	// TODO: Add your message handler code here and/or call default
	CListBox::OnLButtonDblClk(nFlags, point);
	}

/************************************************************************************
/* + void ListBox::OnSelchange()
*	Throw a callback packet on Selection changed event.
*	reason = SDS_CBR_SELECT.
*
*
*
*/
void ListBox::OnSelchange()
	{
	// TODO: Add your control notification handler code here

	//Modified SAU 31/05/2000  			  			[
	//Reason:multiple_select list box do not return the proper string;
	int*  striValue;
	char*  strValue;
    int      nTotal,nCurSel;
    //Modified SAU 31/05/2000  			  			]

	Dialog * pParent = static_cast<Dialog *>(GetParent());
	try
		{
		// the below code is used to fill the callback packet which is sent in the call back function
		if(pParent->m_IsModal )
			{
			sds_callback_packet *cpkt = NULL;
			cpkt = GetCallBackPacket();
			cpkt->dialog = ((Dialog*)GetParent())->handle;
			cpkt->reason = SDS_CBR_SELECT;

			//Modified SAU 31/05/2000  					[
			//Reason:multiple_select list box do not return the proper string;
			if(m_attriblist->IsMultipleSelect()){
				nTotal = GetSelCount();
				striValue = new int[nTotal];
				GetSelItems(nTotal,striValue);
				strValue=new char[(nTotal)*7];
				char stemp[5];
				strcpy(strValue,"");

				for(int i=0;i<nTotal;i++){
					_itoa(striValue[i],stemp,10);
					strcat(strValue,stemp);
					strcat(strValue," ");
				}
				strValue[strlen(strValue)-1]='\0';
				if(striValue)  delete striValue;
			}
			else{
            //Modified SAU 31/05/2000  			]
				 nCurSel = GetCurSel();
				 nTotal= GetCount();
				 strValue = new char[nTotal+1];
				 _itoa(nCurSel,strValue,10);
			}
			cpkt->value = strValue;

			ScreenObject *pScreen =(ScreenObject *)this;
			CString strAction = pScreen->m_attriblist->get_Action();
			// call the call back function if call back is set
			// If the callback is not set by action_tile() then see whether the
			// "action" attribute has been specified in the dcl file.If not,
			// call the default callback handler.
			if(m_FunctionPtr != NULL)
				m_FunctionPtr(cpkt);
			else if ( strAction.IsEmpty() == FALSE )
				{
				pParent->m_DefCallBack(cpkt);
				}
			else if(pParent->m_DefCallBack != NULL)
				pParent->m_DefCallBack(cpkt);
			// delete the callback packet
			delete cpkt->tile;
			delete cpkt->value;
			delete cpkt;
			}
		}
	catch(...)
		{
		TRACE(_T("OnSelChange..."));
		}

	}
/************************************************************************************
/* + void ListBox::OnDblclk( )
*	Throw the callback packet with reason = SDS_CBR_DOUBLE_CLICK.
*	OnDblClicking first SelChange() gets called and then OnDblClick().
*
*
*
*/
void ListBox::OnDblclk()
	{
	// TODO: Add your control notification handler code here
	Dialog * pParent = static_cast<Dialog *>(GetParent());
	try
		{
		// the below code is used to fill the callback packet which is sent in the call back function
		if(pParent->m_IsModal )
			{
			sds_callback_packet *cpkt = NULL;
			cpkt = GetCallBackPacket();
			cpkt->dialog = ((Dialog*)GetParent())->handle;
			cpkt->reason = SDS_CBR_DOUBLE_CLICK;
			int nCurSel = GetCurSel( );
			int nTotal = GetCount();
			char *strValue = new char[nTotal];
			_itoa(nCurSel,strValue,10);
			cpkt->value = strValue;


			ScreenObject *pScreen =(ScreenObject *)this;
			CString strAction = pScreen->m_attriblist->get_Action();
			// call the call back function if call back is set
			// If the callback is not set by action_tile() then see whether the
			// "action" attribute has been specified in the dcl file.If not,
			// call the default callback handler.
			if(m_FunctionPtr != NULL)
				m_FunctionPtr(cpkt);
			else if ( strAction.IsEmpty() == FALSE )
				{
				pParent->m_DefCallBack(cpkt);
				}
			else if(pParent->m_DefCallBack != NULL)
				pParent->m_DefCallBack(cpkt);
			// delete the callback packet
			delete cpkt->tile;
			delete cpkt;
			}
		}
	catch(...)
		{
		TRACE(_T("OnDblClk failed..."));
		}
	}

/************************************************************************************
/* + int ListBox::InitFocus()
*	Sets the focus on the list.
*
*
*
*
*/

int ListBox::InitFocus()
	{
	SetFocus();
	return SDS_RTNORM;
	}



/************************************************************************************
/* + int ListBox::SetTile(CString szControlKey, CString szControlInfo)
*
*   Sets the current selection in the list box.
*
*
*
*/
int ListBox::SetTile(CString szControlKey, CString szControlInfo)
	{
//4M Item:10->
	CWnd * pWind;
	pWind = (CWnd *)(this);
   pWind->SendMessage(WM_SETREDRAW,FALSE, 0L);
//<-4M Item:10
    //2001-8-5 EBATECH(FUTA) -[
	if ((szControlInfo == "") && (szControlKey == "")){
		szControlInfo = m_attriblist->get_ValueString();
		}
	if (szControlInfo == "") szControlInfo = "-1";
	// ]-
	// MB 22-06-2001 Supporting -1 for mutiple select listbox
	if(m_attriblist->IsMultipleSelect()) {

		int nIndex = atoi((LPCTSTR)szControlInfo );
		if ( SetSel( nIndex, nIndex > -1 ) == LB_ERR )
			{
//4M Bugzilla 77985 Item:10->
         pWind->SendMessage(WM_SETREDRAW,TRUE,0L);
         pWind->InvalidateRect(NULL, TRUE);
//<-4M Bugzilla 77985 Item:10
            return SDS_RTERROR;
			}
		else
			{
			//2001-8-5 EBATECH(FUTA) -[
			char *fs1;
			fs1=strtok((char *)(LPCTSTR)szControlInfo," ");
			while(fs1 != NULL){
				int sIndex=atoi((LPCTSTR)fs1);
				if ( SetSel(sIndex , sIndex > -1) == LB_ERR )
					return SDS_RTERROR;
				fs1=strtok(NULL," ");
			}
			// ]-
//4M Bugzilla 77985 Item:10->
         pWind->SendMessage(WM_SETREDRAW,TRUE,0L);
         pWind->InvalidateRect(NULL, TRUE);
//<-4M Bugzilla 77985 Item:10
            return SDS_RTNORM;
			}
	} else {
		if ( SetCurSel(atoi((LPCTSTR)szControlInfo) ) == LB_ERR )
			{
//4M Item:10->
		pWind->SendMessage(WM_SETREDRAW,TRUE,0L);
      pWind->InvalidateRect(NULL, TRUE);
//<-4M Item:10
            return SDS_RTERROR;
			}
		else
			{
//4M Item:10->
		pWind->SendMessage(WM_SETREDRAW,TRUE,0L);
      pWind->InvalidateRect(NULL, TRUE);
//<-4M Item:10
            return SDS_RTNORM;
			}
	}
	}
/************************************************************************************
/* + void ListBox::OnSetfocus()
*
*
*
*
*
*/

void ListBox::OnSetfocus()
	{
	// TODO: Add your control notification handler code here
	if ( this->m_attriblist->IsAllowAccept() == false )
		{
		Dialog * pParent = static_cast<Dialog *>(GetParent());
		pParent->GetDefID();
		pParent->ToggleDefaultID(FALSE);
		}
	}

/************************************************************************************
/* + void ListBox::OnKillfocus()
*
*
*
*
*
*/

void ListBox::OnKillfocus()
	{
	// TODO: Add your control notification handler code here
		if ( this->m_attriblist->IsAllowAccept() )
		{
		Dialog * pParent = static_cast<Dialog *>(GetParent());
		pParent->ToggleDefaultID(TRUE);
		}

	}
