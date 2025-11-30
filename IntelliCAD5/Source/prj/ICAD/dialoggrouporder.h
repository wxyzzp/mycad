#ifndef __DialogGroupOrderH__
#define __DialogGroupOrderH__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class db_drawing;
class db_dictionary;
class CDbGroup;

/*------------------------------------------------------------------------------*//**
@author Denis Petrov

@description Representation of dialog for reordering entities in group.
*//*----------------------------------------------------------------------------*/
class CDialogGroupOrder : public CDialog
{
// Construction
	CDialogGroupOrder(CWnd* pParent = NULL);   // standard constructor
public:
	enum EAction
	{
		eACTION_HIGHLIGHT = 20
	};

	CDialogGroupOrder(CWnd* pParent, db_drawing* pDrawing, sds_name showGroups = NULL);
	virtual BOOL OnInitDialog();
// Dialog Data
	//{{AFX_DATA(CDialogGroupOrder)
	enum { IDD = IDD_GROUP_ORDER };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogGroupOrder)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
	bool isDidReorder() const { return m_bDidReorder; }
	int getCurrentGroup(sds_name sdsGroupName) const;
// Implementation
protected:
	db_drawing* m_pDrawing;
	db_dictionary* m_pDictGroup;
	CDbGroup* m_pCurrentGroup;
	sds_name m_showGroups;
	CListBox* m_pListGroup;
	bool m_bDidReorder;
	static CString s_outString;

	BOOL OnHelpInfo(HELPINFO *pHel);

	// Generated message map functions
	//{{AFX_MSG(CDialogGroupOrder)
	afx_msg void OnHighlight();
	afx_msg void OnReorder();
	afx_msg void OnReverseOrder();
	afx_msg void OnSelchange();
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif
