// IcadCusMenu.h : header file
//

#pragma once

#include "IcadMenu.h"
#include "IcadCusMenuTree.h"

/////////////////////////////////////////////////////////////////////////////
// CIcadCusMenu dialog

class CIcadCusMenu : public CPropertyPage
{
	DECLARE_DYNCREATE(CIcadCusMenu)

//*** Member variables
public:
	CImageList* m_pImageList;
	long m_lflgVisibility;
	CIcadCusMenuTree m_TreeCtrl;
	CImageList* m_pDragImage;
	HTREEITEM m_hCurDragTarget;
	HTREEITEM m_hOrigDragItem;
	BOOL m_bCurDragItemHasKids;
	HCURSOR m_hPrevCursor;
	CIcadMenu* m_pCurMenu;
	CString m_strLoadLisp;
	BOOL m_bDestroyOld;
	BOOL m_bOKtoWarn;

//*** Helper functions
public:
	void AddMenuItemsToTree(int* idxCurArrayItem, HTREEITEM hParent);
	LPICMENUITEM InitNewMenuItem(HTREEITEM hItem);
	void CopyMenuItem(LPICMENUITEM lpDest, LPICMENUITEM lpSrc);
	void CopyMenuItem(LPICMENUITEM lpDest, LPICMNUINFO lpSrc);
	HTREEITEM AddOneItem(HTREEITEM hParent, LPCTSTR szText, int cChildren, LPICMENUITEM lpMenuItem);
	void ContextMenuPick(UINT nId);
	void EnableCommandControls(BOOL bEnable=TRUE);
	void AddCommandToEdit(LPCTSTR strCommand);
	void SaveCommandToItem(HTREEITEM hItem) ;
	void DeleteTreeItem(HTREEITEM hItem);
	void OnExit();
	void CreateCustomMenuArray();
	void CreateCustomSubMenuItems(HTREEITEM hParentItem, int* idxCurArrayItem);
	UINT GetTreeCount();
	UINT GetTreeSubItemCount(HTREEITEM hParentItem);
	HTREEITEM CopyTreeItem(HTREEITEM hInsertAfter, HTREEITEM hOrigItem, HTREEITEM hParentItem);
	void DrawInsertButton();
	void WarnUser();
	void OnReset();

// Construction
public:
	CIcadCusMenu();
	~CIcadCusMenu();

// Dialog Data
	//{{AFX_DATA(CIcadCusMenu)
	enum { IDD = CUS_MENU };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CIcadCusMenu)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CIcadCusMenu)
	virtual BOOL OnInitDialog();
	afx_msg void OnEndLabelEditMenuTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonInsertItem();
	afx_msg void OnButtonDelete();
	afx_msg void OnButtonRename();
	afx_msg void OnSelChangedMenuTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillFocusCommandEdit();
	afx_msg void OnAddCommandToEdit();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnDeleteTreeItem(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginDragMenuTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnButtonAdvanced();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////
