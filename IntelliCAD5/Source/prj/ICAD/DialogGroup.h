#ifndef __DialogGroupH__
#define __DialogGroupH__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class db_drawing;
class db_dictionary;
class CDbGroup;
/*------------------------------------------------------------------------------*//**
@author Denis Petrov

@description Representation of dialog for manipulating with groups.
*//*----------------------------------------------------------------------------*/
class CDialogGroup : public CDialog
{
	CDialogGroup(CWnd* pParent = NULL);   // standard constructor
// Construction
public:
	enum EAction
	{
		eACTION_HIGHLIGHT = 10,
		eACTION_FINDBYENTITY = 11,
		eACTION_ADD = 12,
		eACTION_REMOVE = 13,
		eACTION_REORDER = 14
	};

	CDialogGroup(CWnd* pParent, db_drawing* pDrawing);
	virtual BOOL OnInitDialog();
// Dialog Data
	//{{AFX_DATA(CDialogGroup)
	enum { IDD = IDD_GROUP };
	BOOL	m_displayUnnamed;
	//}}AFX_DATA
	int getCurrentGroup(sds_name sdsGroupName) const;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogGroup)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	db_drawing* m_pDrawing;
	db_dictionary* m_pDictGroup;
	CDbGroup* m_pCurrentGroup;
	CListCtrl* m_pListGroup;

	BOOL OnHelpInfo(HELPINFO *pHel);

	int fillListGroup();
	// Generated message map functions
	//{{AFX_MSG(CDialogGroup)
	afx_msg void OnCreateNew();
	afx_msg void OnHighlight();
	afx_msg void OnFindByEntity();
	afx_msg void OnListClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAdd();
	afx_msg void OnRemove();
	afx_msg void OnUngroup();
	afx_msg void OnReorder();
	afx_msg void OnDisplayUnnamed();
	afx_msg void OnRename();
	afx_msg void OnChangeDescription();
	afx_msg void OnChangeSelected();
	afx_msg void OnCreateUnnamed();
	afx_msg void OnHelp();
	afx_msg void OnChangeNewName();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif
