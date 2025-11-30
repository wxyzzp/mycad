/* D:\DEV\PRJ\ICAD\ICADATTEDIA.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

//*********************************************************************
//*  IcadAtteDia.cpp                                                  *
//*  Copyright (C) 1997 by Visio, Inc.							      *
//*                                                                   *
//*********************************************************************

// This dialog gives users a place to see and edit all the attributes
// associated with the selected block.
//
// IDs in the dialog are:
// ATT_BLOCK   -  Static Text displaying the name of the selected block
// ATT_LIST    -  List View Control with attributes.  3 columns: Name, Prompt, Value
// ATT_PROMPT  -  Edit Box for changing the prompt of the selected attribute
// ATT_VALUE   -  Edit Box for changing the value of the selected attribute

#include "Icad.h"/*DNT*/
#include "resource.hm"/*DNT*/
#include "paths.h"
#include "IcadDialog.h"

// This structure contains the information for each attribute
class ATT_ListInfo {
public:
	ATT_ListInfo::ATT_ListInfo(void) { Name=NULL; Prompt=NULL; Value=NULL; prb=NULL; }
	ATT_ListInfo::~ATT_ListInfo(void) {
		delete Name; delete Prompt; delete Value;
		if (prb!=NULL) sds_relrb(prb);
	}
    char  *Name,
		  *Prompt,
          *Value;
	struct resbuf *prb;
};

class att_CAtte : public IcadDialog {
    public:
        sds_name		m_BlockName;			// Entity name for selected block
        BOOL			OnInitDialog();
        static int CALLBACK ListSortProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
        att_CAtte() : IcadDialog(IDD_ATTEDIT) { }	// The dialog itself
		CStatic		   *m_CStaticBlock;			// Static control displaying block name text at top of dialog
        CListCtrl	   *m_plvThisList;			// Pointer to the list view control we are working with
		CEdit		   *m_CEditValue;			// The edit box that contains the Value
		int			    nEditSelection;			// The Item whose value appears in the Edit Box
		BOOL			m_swkilling;

    protected:
        afx_msg BOOL    OnHelpInfo(HELPINFO *pHel);
        afx_msg void    OnHelp();
        afx_msg void	OnListSelect();
        afx_msg void	OnEnKill();
		afx_msg void	OnSetFocus();
        virtual void	OnOK( );
        virtual void	OnCancel( );
        LRESULT	ListNotifyHandler(UINT message, WPARAM wParam, LPARAM lParam);
        LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};


BEGIN_MESSAGE_MAP(att_CAtte,CDialog)
    ON_EN_SETFOCUS(ATTEDIT_VALUE,OnSetFocus)		// When user first clicks in the Value edit box...
    ON_EN_KILLFOCUS(ATTEDIT_VALUE,OnEnKill)			// When user is done in the Value edit box...
	ON_WM_HELPINFO()
    ON_BN_CLICKED(IDHELP,OnHelp)
END_MESSAGE_MAP()

BOOL att_CAtte::OnHelpInfo(HELPINFO *pHel){
	OnHelp();
	return(true);
}

void att_CAtte::OnHelp() {
	CommandRefHelp( m_hWnd, HLP_DDATTE);
}

//***This is the main window
void CMainWindow::IcadAtteDia(sds_name nmEntity) {			// Entity name gets passed to this dialog (from ???)
    struct resbuf	*prbThisGroup=NULL,			// The part (group) of the entity definition we are currently working with.
					*prbStartOfList=NULL;		// The first thing in the list.

    prbThisGroup=prbStartOfList=sds_entget(nmEntity);
    if(prbThisGroup->restype==-1) prbThisGroup=prbThisGroup->rbnext;
    if(prbThisGroup->restype==0) {								// 0 is Entity Type
        if(strsame(prbThisGroup->resval.rstring,"INSERT")) {
            while(prbThisGroup->rbnext) {
                prbThisGroup=prbThisGroup->rbnext;
                if(prbThisGroup->restype==66) {					// 66 is the flag for Variable Attributes Follow This Block In The Entity List.
                    if(prbThisGroup->resval.rint) {
                        att_CAtte m_AttEdit;					// Inline declaration of this instance of the att_CAtte dialog.
                        m_AttEdit.m_swkilling=false;
						ic_encpy(m_AttEdit.m_BlockName,nmEntity);
                        m_AttEdit.DoModal();
                        break;
                    }else{
                        goto bail;
                    }
                }
            }
        }
    }

bail:
    IC_RELRB(prbStartOfList);		// Frees whole list
    return;
}



// When the dialog is initially created...
BOOL att_CAtte::OnInitDialog() {
	int						nIndex=0;			// For stepping throught the items in the list control
    sds_name				nmFirstEnt,			// First entity in the block in question.
							nmEntity,
							nmInsertNext;
	ATT_ListInfo			*pListInfo;
	struct resbuf		   *prbThisAtt,			// For messing with attribute info just within this function.
						   *elist;				// resbuf returned by sds_tblsearch.  *** Don't change "elist" or it won't get freed by IC_FREEIT!!! ***
	LV_ITEM					lvItemInfo;			// ListView Item structure
	CString					csValue;			// Already exists as szValue, but this needs a CString. Better way???
	char					blockname[IC_ACADBUF];

	IcadDialog::OnInitDialog();

	lvItemInfo.mask		=LVIF_TEXT|LVIF_PARAM;
	lvItemInfo.state	=0;
	lvItemInfo.stateMask=0;

    // Get the pointer to the static control
    m_CStaticBlock=(CStatic *)GetDlgItem(ATTEDIT_BLOCK);
	// Get the pointer to the List Control
	m_plvThisList=(CListCtrl *)GetDlgItem(ATTEDIT_LIST);
	// Get the pointer to the Edit Box
	m_CEditValue=(CEdit *)GetDlgItem(ATTEDIT_VALUE);

	// Create the column headings, widths, etc.
	m_plvThisList->InsertColumn(0,ResourceString(IDC_ICADATTEDIA_NAME_2, "Name" ),LVCFMT_LEFT,100);
	m_plvThisList->InsertColumn(1,ResourceString(IDC_ICADATTEDIA_PROMPT_3, "Prompt" ),LVCFMT_LEFT,150);
	m_plvThisList->InsertColumn(2,ResourceString(IDC_ICADATTEDIA_VALUE_4, "Value" ),LVCFMT_LEFT,170);

	// Get the list of information about the selected block
	elist=sds_entget(m_BlockName);

	// Get the name of the block, and write it into the Static text at the top of the dialog.
	// Note:  Using prbThisAtt as a "temp" to get the Block name.  Nothing to do with Attributes.
	if(ic_assoc(elist,2)!=0) {
		IC_FREEIT
		return(FALSE);
	}
	strcpy(blockname,ic_rbassoc->resval.rstring);
	m_CStaticBlock->SetWindowText(blockname);
	IC_FREEIT

		// Deal with attributes, which should be present in the BLOCK to have gotten this far.
    if((elist=sds_tblsearch("BLOCK"/*DNT*/,blockname,0))==NULL) {
		IC_FREEIT
		return(FALSE);
	}
	if(ic_assoc(elist,-2)!=0) {
		IC_FREEIT
		return(FALSE);
	}
	ic_encpy(nmFirstEnt,ic_rbassoc->resval.rlname);
	IC_FREEIT

    // The "entities" immediately following a block with attributes, *are* the attributes.
	sds_entnext_noxref(m_BlockName,nmInsertNext);
	do {
		// Get initial information for all attributes
		prbThisAtt=sds_entget(nmInsertNext);
		if(ic_assoc(prbThisAtt,0)) {
			sds_relrb(prbThisAtt);
			return(NULL);
		}
		if (strsame(ic_rbassoc->resval.rstring,"SEQEND"/*DNT*/)) {
			sds_relrb(prbThisAtt);
			break;
		}
		else if (strsame(ic_rbassoc->resval.rstring,"ATTRIB"/*DNT*/)) {			// As long as we're still going through attribute entities...
			struct resbuf *flagrb=ic_ret_assoc(prbThisAtt,70);
			if (flagrb->resval.rint & IC_ATTRIB_CONST) continue;  // do not process constant attributes

			if (NULL == (pListInfo = new ATT_ListInfo)) 
				return(FALSE);

			pListInfo->prb=prbThisAtt;

			if (!ic_assoc(prbThisAtt,2)) 
			{
				if (NULL == (pListInfo->Name = (char *) new char[strlen(ic_rbassoc->resval.rstring)+1])) 
				{
					delete pListInfo;
					return(FALSE);		// Doesn't szName already account for the +1 ???
				}

				strcpy(pListInfo->Name,ic_rbassoc->resval.rstring);
				ic_encpy(nmEntity,nmFirstEnt);

				do {
                    if((elist=sds_entget(nmEntity))==NULL) break;
					if(ic_assoc(elist,0)!=0) break;
					if(strsame("ENDBLK",ic_rbassoc->resval.rstring)) break;
					else if(strisame("ATTDEF"/*DNT*/,ic_rbassoc->resval.rstring) && ic_assoc(elist,2)==0 && strisame(pListInfo->Name,ic_rbassoc->resval.rstring)) {
						// Get the prompt for the attribute
						if(ic_assoc(elist,3)!=0) break;			// Group 3 in an ATTDEF is the Prompt string (we're not talking ATTRIB here)

						if (NULL == (pListInfo->Prompt = (char *) new char[strlen(ic_rbassoc->resval.rstring)+1])) 
						{
							delete pListInfo;
							return(FALSE);	// If NULL kills strlen, what if user didn't enter
						}

						strcpy(pListInfo->Prompt,ic_rbassoc->resval.rstring);
						break;
					}
					IC_FREEIT
                } while(sds_entnext_noxref(nmEntity,nmEntity)==RTNORM);
				IC_FREEIT
			}
			if(!ic_assoc(prbThisAtt,1)) 
			{
				if (NULL == (pListInfo->Value = (char *) new char[strlen(ic_rbassoc->resval.rstring)+1])) 
				{
					delete pListInfo;
					return(FALSE);	// something?  How should I handle NULL???
				}

				strcpy(pListInfo->Value,ic_rbassoc->resval.rstring);
			}

			// Write the information to the List View Control.
			lvItemInfo.mask       = LVIF_TEXT | LVIF_PARAM | LVIF_STATE;    // Possible states for list items
			lvItemInfo.lParam     =(LPARAM)pListInfo;
			lvItemInfo.iItem      =nIndex;
    		lvItemInfo.iSubItem   =0;
			lvItemInfo.pszText    =pListInfo->Name;
			// Insert the row for this attribute.  lvItemInfo (above) will fill in first column's text.
			m_plvThisList->InsertItem(&lvItemInfo);
			// Write Prompt for Attribute to 2nd column of List Control
			m_plvThisList->SetItemText(nIndex,1,pListInfo->Prompt);
			// Write Value for Atttribute to 3rd column of List Control
			m_plvThisList->SetItemText(nIndex,2,pListInfo->Value);
			if (0==nIndex) {
				//DP: added LVIS_FOCUSED
				m_plvThisList->SetItemState(nIndex,LVIS_SELECTED | LVIS_FOCUSED,LVIS_SELECTED | LVIS_FOCUSED);    // Set 1st item as selected by default.
				// Get text (CString) from the Value column for the selected item.
				csValue=m_plvThisList->GetItemText(nIndex,2);
				// Put text for the selected item in Value Edit Box.
				m_CEditValue->SetWindowText((LPCTSTR)csValue);
			}
			++nIndex;			// Go on to the next attribute
		}
    } while(sds_entnext_noxref(nmInsertNext,nmInsertNext)==RTNORM);

	m_CEditValue->SetFocus();                                      // Edit box should have focus at first.
    return TRUE;
}


//
LRESULT att_CAtte::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) {

	switch(message) {
		case WM_NOTIFY:
            return(ListNotifyHandler(message,wParam,lParam));
	}
	return CDialog::WindowProc(message, wParam, lParam);
}



// When the user clicks a column heading (to change sort order/priority)...
LRESULT att_CAtte::ListNotifyHandler(UINT message, WPARAM wParam, LPARAM lParam) {
	LV_DISPINFO   *pLvdi= (LV_DISPINFO *)lParam;
	NM_LISTVIEW   *pNm  = (NM_LISTVIEW *)lParam;
    LV_KEYDOWN    *pnkd = (LV_KEYDOWN FAR *)lParam;

	// Did they click the listbox?  If no, bail.
	if (wParam!=ATTEDIT_LIST) return 0L;
	// If they clicked a column heading, tell the sort function
	switch (pLvdi->hdr.code) {
        case LVN_ITEMCHANGED:
			OnEnKill();
			m_swkilling=0;
			OnListSelect();
			break;
		case LVN_COLUMNCLICK:
            m_plvThisList[0].SortItems(ListSortProc,(LPARAM)(pNm->iSubItem));
			break;
		default:
			break;
	}
	return 0L;
}


///////////////////////////////////////////////////////////////////////
// This is the sorting function, whenever a column heading is clicked
// then this function is called by the (SORTITEMS) function.
//
// The comparison function must return a negative value if the first
// item should precede the second, a positive value if the first item
// should follow the second, or zero if the two items are equivalent.
// The lParam1 and lParam2 parameters specify the item data for the two
// items being compared. The lParamSort parameter is the same as the
// dwData value.
//
// A return value is the result of the compare.
//
int CALLBACK att_CAtte::ListSortProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) {
    char   *string1,
           *string2;

    switch(lParamSort) {
        case 0:		// Sorting by first column:  Name
            string1=((ATT_ListInfo *)lParam1)->Name;
            string2=((ATT_ListInfo *)lParam2)->Name;
            break;
        case 1:		// Sorting by second column:  Prompt
            string1=((ATT_ListInfo *)lParam1)->Prompt;
            string2=((ATT_ListInfo *)lParam2)->Prompt;
            break;
        case 2:		// Sorting by third column:  Value
            string1=((ATT_ListInfo *)lParam1)->Value;
            string2=((ATT_ListInfo *)lParam2)->Value;
            break;
    }

    return(lstrcmp(string1,string2));
}



// When the user selects an item in the List View, write the info to the edit box...
void att_CAtte::OnListSelect() {
    int						nItemIndex=0;			// The index for items in the ListCtrl
	CString					csValue;				// Already exists as szValue, but this needs a CString. Better way???

	// Set the index to the number of items in the ListCtrl
	for(nItemIndex=(m_plvThisList->GetItemCount()-1); nItemIndex!=-1; --nItemIndex) {		// Go through the ListCtrl (backwards) 'til you find selected item.

       if (m_plvThisList->GetItemState(nItemIndex,LVIS_SELECTED)) {		// Find out which line from the ListCtrl is selected
			csValue=m_plvThisList->GetItemText(nItemIndex,2);				// Get text (CString) from the Value column for the selected item.
			m_CEditValue->EnableWindow(1);									// Allow input in the edit box.
			m_CEditValue->SetWindowText((LPCTSTR)csValue);					// Put text for the selected item in Value Edit Box.
		} else continue;
	}
}

// When the user is done in an edit box and goes to something else... (when Edit Box loses focus)
void att_CAtte::OnEnKill() {
    int					 nItemIndex=0;
	ATT_ListInfo		*pInfo;
	CString              csValue;

	if (m_plvThisList->GetSelectedCount()) {	// If an item in the list box is selected...
		if (m_swkilling) {							// If text in Edit Box has been modified....
			if (m_CEditValue->GetModify()) {			// Get the text from the Edit Box
				m_CEditValue->GetWindowText(csValue);
				pInfo=(ATT_ListInfo *)m_plvThisList->GetItemData(nEditSelection);
				if ( pInfo != NULL )
					{
					delete pInfo->Value;
					if (NULL==(pInfo->Value=(char *)new char[csValue.GetLength()+1])) return;
					strcpy(pInfo->Value,(LPCTSTR)csValue);
					m_plvThisList->SetItemText(nEditSelection,2,(LPCTSTR)csValue);
					}
			}
		} else m_swkilling=1;
	} else {									// If no items are selected in the list box...
		m_CEditValue->SetWindowText( ""/*DNT*/ );			// Blank out the edit box.
		m_CEditValue->EnableWindow(0);				// Do not allow input in the edit box.
		m_swkilling=1;
	}
}

void att_CAtte::OnSetFocus() {
	// This is for finding the List Item that was selected when the user *began* to mess with the Edit Box.
	// It's used when the Edit Box loses focus, to update the right item in the List View control
	// ON_EN_KILLFOCUS is too late, since the focus has already been reset.
    int nItemIndex=-1;

	m_swkilling=1;
	nEditSelection=m_plvThisList->GetNextItem(nItemIndex,LVNI_SELECTED);
}

// When the user clicks OK...
void att_CAtte::OnOK() {
    int			   nItemIndex;		//	Number of attributes in the List View control
    CString        csTmp;
    struct resbuf *pRb=NULL;
	ATT_ListInfo  *pInfo;

    OnEnKill();	  // OnEnKill gets whatever info is in the Value Edit Box and put it in the List View control.  Then...
	if (m_plvThisList->GetItemCount()) {  // if none, don't bother
		for(nItemIndex=(m_plvThisList->GetItemCount()-1); nItemIndex!=-1; --nItemIndex) {		// Go through the ListCtrl (backwards) 'til you find selected item.
			pInfo=(ATT_ListInfo *)m_plvThisList->GetItemData(nItemIndex);
			pRb=pInfo->prb;
			while(pRb->rbnext) {
				pRb=pRb->rbnext;
				if(pRb->restype==1) {
				   pRb->resval.rstring=ic_realloc_char((char*)pRb->resval.rstring,strlen(pInfo->Value)+1);
				   strcpy(pRb->resval.rstring,pInfo->Value);
				   break;
				}
			}
			int ret=sds_entmod(pInfo->prb);
			m_plvThisList->SetItemData(nItemIndex,NULL);
		    delete pInfo;
		}

		sds_entupd(m_BlockName);
	}


	// Get any changed information for all attributes in the list
	// and save it back with the block it came from
	// Make the dialog go away

	CDialog::OnOK();
}


// When the user clicks Cancel...
void att_CAtte::OnCancel() {
    int                  nItemIndex;		//	Number of attributes in the List View control

	// Don't change the block's attributes
	for((nItemIndex=m_plvThisList->GetItemCount()-1); nItemIndex!=-1; --nItemIndex) {		// Go through the ListCtrl (backwards) 'til you find selected item.
        delete (ATT_ListInfo *)m_plvThisList->GetItemData(nItemIndex);
    }

    CDialog::OnCancel();
}



