/** FILENAME:     PopUpList.cpp- Contains implementation of class CPopUpList.
* Copyright  (C) Copyright 1998-99  Visio Corporation.  All Rights Reserved.
*
*
*
* Abstract 
*		this class Maps the basic tiles PopUpList	
*	to windows control comboBox
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
#include "PopUpList.h"
#include "dialog.h"
#include "sds.h"
#include "dclprivate.h"

#include "ItcLanguage.h" //EBATECH

#define NOITEM		-1
#define CHANGE		1
#define APPEND		2
#define REPLACE		3

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/************************************************************************************
/* + PopUpList::PopUpList - 
*
*	Construction
*
*
*
*/

PopUpList::PopUpList()
	{
	}
/************************************************************************************
/* + PopUpList::PopUpList(int ParentAlignmen - 
*
*	overloaded constructor
*
*
*
*/

PopUpList::PopUpList(int ParentAlignment) // alignment of parent
	{
	m_ParentAlignment = ParentAlignment;

	}

/************************************************************************************
/* + PopUpList::~PopUpList ()
*
*	Destruction
*
*
*
*/

PopUpList::~PopUpList()
	{
	}


BEGIN_MESSAGE_MAP(PopUpList, CComboBox)
	//{{AFX_MSG_MAP(PopUpList)
	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelchange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/************************************************************************************
/* + int PopUpList:: CreateOnScreenObj (CWnd* pParent)
*
*	creates the windows UI control.
*	
*
*
*
*/

int PopUpList:: CreateOnScreenObject (
									  CWnd* pParent // i:pointer to the parent window
									  )
	{
	int error;
	int editwidth;
	dwStyle = WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST|WS_VSCROLL|WS_HSCROLL|WS_GROUP;
	pParentWnd = pParent;
	DWORD dwStyleStatic = WS_CHILD|WS_VISIBLE;
	ValueString = m_attriblist->get_ValueString();
	if (!m_attriblist->IsEnabled())
		{
		// set style to enable
		dwStyle = dwStyle|WS_DISABLED;
		dwStyleStatic = dwStyleStatic| WS_DISABLED;
		}
	if (m_attriblist->IsTabstop())
		{
		// set style to tabstop
		dwStyle = dwStyle|WS_TABSTOP;
		}

	CRect  rectstatic ; 

	/* {{{ Gurudev - DT: 8/5/99 11:20:09 AM
		int i=m_ComponentFont->CreatePointFont(80,_T("MS Sans Serif"),NULL);
	Gurudev - Dt:  8/5/99 11:20:12 AM }}} */ 

	// EBATECH(CNBR) 2001-05-05 Fontface needs localize. -[
	CString wszResourceString;

	// {{{ Gurudev - DT: 8/5/99 11:20:09 AM
	// Check for fixed_width_font
	if ( m_attriblist->get_FixedWidthFont() == false )
		{
		// If fixed_font_width is false.
		wszResourceString.LoadString(ID_DCL_DEFAULT_FONTFACE);   // Load the resource string.
		m_ComponentFont->CreatePointFont(DIALOG_FONT_POINTSIZE,wszResourceString,NULL); // create font
		//m_ComponentFont->CreatePointFont(80,_T("MS Sans Serif"),NULL); // create font
		}
    else
        {
        // If fixed_font_width is true.
		wszResourceString.LoadString(ID_DCL_FIXED_PITCH_FONTFACE);   // Load the resource string.
		m_ComponentFont->CreatePointFont(DIALOG_FONT_POINTSIZE,wszResourceString,NULL); // create font
		//m_ComponentFont->CreatePointFont(80,_T("Fixedsys")); // create font
		}
	// EBATECH(CNBR) ]-
	// Gurudev - Dt:  8/5/99 11:20:12 AM }}}  


	rectstatic = rectpopup = PositionControl(); //  get the actual display area for control in the dialog
	// if label is given then add a static control
	if(!m_attriblist->get_Label().IsEmpty())
		{
		CSize size;
		CDC dc;
		// get the size of label	
		dc.CreateIC ("DISPLAY",NULL,NULL,NULL);
		size = dc.GetOutputTextExtent(m_attriblist->get_Label());
		
		int static_width = size.cx  + 8;//(m_attriblist->get_Label().GetLength()) * (LOWORD(GetDialogBaseUnits())) -2 ;
		rectstatic.right = rectstatic.left + static_width;
		rectpopup.left = rectstatic.right ;
		CString Label =SetMnemonic() ;
		error = n_Title.Create(Label,dwStyleStatic,rectstatic,pParent); // create  static control
		n_Title.SetFont(m_ComponentFont,TRUE);	// set font of static control
		editwidth=( m_attriblist->get_editWidth () * 6 )+12;
		// if edit width is given the edit control will be aligned from right with width equal to edit width
		if(!m_attriblist->IsFromDefault ())
			{
			rectpopup.left = rectpopup.right - editwidth;
			}
		}
	else
		{
		// label is not given but edit width is given 
		editwidth= m_attriblist->get_editWidth () * 6 + 12;
		// if edit width is given the edit control will be aligned from right with width equal to edit width
		if(!m_attriblist->IsFromDefault ())
			{
			rectpopup.left = rectpopup.right - editwidth;
			}

		}
	// set the drop down width
	if(! (m_attriblist->get_List()).IsEmpty ())
		rectpopup.bottom = rectpopup.top + 100;
	
	
	
	error = Create(dwStyle, rectpopup, pParent, m_controlID.GetID()); // creates the UI control
	if (error)
		{
		SetFont(m_ComponentFont,TRUE);				// set font of combobox
		
		CString Value = m_attriblist->get_List();
		if(!Value.IsEmpty())
			{
			CString tempstr ;
			// fill the listbox of combobox with list attributes
			// index of seperator
			int index =0,count=0;
			Value += ",";
			while((index =Value.Find(',')) > 0)
				{
					tempstr=Value.Mid(0,index);
					Value.Delete(0,index+1);
					AddString(SetTabs(tempstr)); // setTabs function replaces the \t with blank spaces
					count++;
				}
			GetLBText(0,tempstr);
			SelectString(0,tempstr);	// select the frist string in the list so that 
					
			//Commented to make the width of the dropdown list box equal to that of the edit_box.
			// If the height gives problems change the height value to (count*16)
			// 23-Aug-1999
			//SetDroppedWidth(count*14);					
			SetDroppedWidth(rectpopup.right - rectpopup.left);	
			// 23-Aug-1999
				
			return SDS_RTNORM;	
			}
		//Commented to make the width of the dropdown list box equal to that of the edit_box.
		// 23-Aug-1999
		//SetDroppedWidth(16);
		SetDroppedWidth(rectpopup.right - rectpopup.left);	
		// 23-Aug-1999
		}
	else
		return SDS_RSERR;

	return SDS_RTNORM;	
	}


/************************************************************************************
/* + CString PopUpList::SetTabs(CString tabsValue) 
*
*	function to set tabs.
*
*
*
*/

CString PopUpList::SetTabs(						
						   CString tabsValue	// i : tabs string 
						   ) 
	{
	CDC dc;
	dc.CreateIC ("DISPLAY",NULL,NULL,NULL);
	int count=0 ,index=-1, tabsize;
	int down=0;
	CString tabstring,tempstr,tabs;
	tabsValue += CString("\t");
////////////////////////////////
	// gets the tabs string and convert to an integer array 
	tabs = m_attriblist->GetTabsList ();
	tabs += CString(" ");
	if(count =tabs.Find(' ')>0)
		{
		tabsize=atoi(tabs.Mid(0,count));
		}

		int tabsArry[20];
		tabstring = m_attriblist->GetTabsList ();
		index =0 ;
		int position = 0 ;
		tabstring.TrimLeft ();
		tabstring += CString(" ");
		if(!tabstring.IsEmpty())
			{
			while((count =tabstring.Find(' ')) > 0)
				{
				tempstr=tabstring.Mid(index,count);
				// {{{ Gurudev - DT: 8/5/99 2:58:05 PM
				if ( atoi(tempstr) < 0 )
						break;
				// Gurudev - Dt:  8/5/99 2:58:12 PM }}}  
				tabstring.Delete(index,count+1);
				tabsArry[position++]= atoi(tempstr);
				}
			}
	//////////////////////
	// calculate the width of the string with out tabes used using function 
	// GetOutputTextExtent then calculate the width of text with tabs enabled
	// using the function GetOutputTabbedTextExtent
	count =0;
	int oddcount=-1;
	tabsize=0;

	/* {{{ Gurudev - DT: 8/5/99 3:42:47 PM
	while((index =tabsValue.Find('\t')) > 0)
	 Gurudev - Dt:  8/5/99 3:42:49 PM }}} */ 
	while((index =tabsValue.Find('\t')) >= 0)
		{
		CSize size1 , size;
		CString temstring;
		
		
		if((count%2) == 0)
			{
			oddcount++;
			}
		if(oddcount<position)
			{
			if(tabsArry[oddcount]>tabsize)
				tabsize= tabsArry[oddcount];
			}
		tempstr=tabsValue.Mid(0,index);
		tabsValue.Delete(0,index+1);
		temstring = tempstr + CString("\t");
		size =dc.GetOutputTextExtent(tempstr);
		int temptabsize =tabsize*tabsize;
		size1= dc.GetOutputTabbedTextExtent(temstring,1,&temptabsize);
	
		// the code bellow adds blank spaces in place of \t till the width of 
	    // the text width out tabs enabled is equal to width with tabs enabled

		while(size.cx<size1.cx )
			{
			tempstr = tempstr+CString(" ");
			size = dc.GetOutputTextExtent(tempstr);
			}
		if(size.cx<20)
			tempstr = tempstr+CString("    ");
		tabstring += tempstr;
		count++;
		}
	return tabstring;
	}

/************************************************************************************
/* + int PopUpList::ModeTile ( int nTileState)
*
*	Executes functions like setting focus to a control , enabling or disabling a control
*	nTileState = 0  then enable window
*	nTileState = 1	disable window
*	nTileState = 2  set Focus
*
*
*
*/

int PopUpList::ModeTile ( // Executes functions like setting focus to a control , enabling or disabling a control etc.
		int nTileState	   // i:state of control
		)
	{
	CWnd * pWind;
	pWind = (CWnd *)(this);

	switch(nTileState)
		{
		case SDS_MODE_ENABLE:
			//Modifiy SMR for preventing potentially crash. 2001/5/26
			if (n_Title) n_Title.EnableWindow(); 
			pWind->EnableWindow();
			break;
		case SDS_MODE_DISABLE:
			//Modifiy SMR for preventing potentially crash. 2001/5/26
			if (n_Title) n_Title.EnableWindow(FALSE); 
			pWind->EnableWindow(FALSE);
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
/* + int PopUpList:: GetTile (CString & szControlInfo,int lInfoMaxLength)	
*	Returns the "value" attribute of the tile
*
*
*
*
*/

int PopUpList:: GetTile (		// Returns the "value" attribute of the tile
		CString & szControlInfo,// o:Date contaned in value attribute
		int lInfoMaxLength	// o: Size of data
		)
	{
	//  if size of data requested is greater then 0 
	//  then get the text in the edit control and return
	if(lInfoMaxLength>0)
	{
		//Modified SAU 31/05/2000			[
		//popup_list tiles do not work at all.Regardless of the item selected in gui
		int nCurSel = GetCurSel();
		if ( nCurSel != CB_ERR )
			szControlInfo.Format("%d",nCurSel);
		else
			szControlInfo.Format("%s",_T(""));
		//Modified SAU 31/05/2000			]
		//szControlInfo = ValueString;
		return SDS_RTNORM ; 
	}
	return SDS_RTERROR ; 
	}



/************************************************************************************
/* + int PopUpList::GetListStrin (CString& szListString,int* nStringLength,int nWhichItem	) 
*
*	this function returns the string given the index in the list control
*
*
*
*/

int PopUpList::GetListString(
								CString& szListString, // o:Parameter to hold the return string
								int* nStringLength,	   // o:Size of the String
								int nWhichItem		   // i:item index in the listbox whose string is returned
								)
	{
	
	CString stringatindex;
	
	(static_cast<CComboBox *>(this))-> GetLBText(nWhichItem,stringatindex); // MFC function to get the string 
	szListString = stringatindex;
	return SDS_RTNORM;
	
	}
/************************************************************************************
/* + int PopUpList::AddLis (CString szAddToList)
*	this add list function depends upon the parameters (nChangeAddNew) in start list
*	nChangeAddNew == CHANGE -- removes the current string from the index and add the new string
*	nChangeAddNew == APPEND  -- adds a new string at the bottom of the list
*	nChangeAddNew == REPLACE -- adds the string at the specified index
*	nChangeAddNew == 3 -- the previous list is emptyed.
*
*
*
*
*/

int PopUpList::AddList(
						  CString szAddToList // i:String to add
						  )
	{
	// This function will be implemented in the base classes
	if(ListStarted == 1)
		{
		if(nChangeAddNew == 3)
					{
					// delete all strings from the list box
					int count =GetCount();
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
	// Modified CyberAge AP 09/22/2000	
	// Reason: To fix Bug No. BRX 1924.  
	// rectpopup.bottom = 100;
	rectpopup.bottom = rectpopup.top + 100;
	this->MoveWindow (rectpopup,false);
	CString tempstr ;
	GetLBText(0,tempstr);
	SelectString(0,tempstr);
	return SDS_RTNORM;
	}


/************************************************************************************
/* + void PopUpList::OnSelchange( )
*	Throw the callback packet with reason = SDS_CBR_SELECT.
*	Value is a string telling the index of the selected element.
*
*
*
*/

void PopUpList::OnSelchange() 
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
			cpkt->reason = SDS_CBR_SELECT;
			int nCurSel = GetCurSel( );
			int nTotal = GetCount();
			
			char *iValue = new char[nTotal+1];
			cpkt->value = _itoa(nCurSel,iValue,10);
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
            delete [] cpkt->value ;
			delete cpkt->tile;
			delete cpkt;
			}	
		}
	catch(...)
		{
		TRACE(_T("OnSelChange failed..."));
		}

	}

/************************************************************************************
/* + int PopUpList:: InitFocus()
*	Set the initial focus.
*
*
*
*
*/

int PopUpList:: InitFocus()
	{
	// sets the focus to the control
	SetFocus();
	return SDS_RTNORM;
	}


int PopUpList::SetTile(CString szControlKey, CString szControlInfo)
	{

	//2001-8-5 EBATECH(FUTA) -[
	if ((szControlInfo == "") && (szControlKey == "")){
		szControlInfo = m_attriblist->get_ValueString();
		}
	if (szControlInfo == "") szControlInfo = "0";
	// ]-
	if ( SetCurSel(atoi((LPCTSTR)szControlInfo) ) == LB_ERR )
		{
		return SDS_RTERROR;
		}
	else
		{
		return SDS_RTNORM;
		}
	}
