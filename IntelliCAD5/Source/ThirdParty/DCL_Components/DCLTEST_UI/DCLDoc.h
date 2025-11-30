/** FILENAME:     DCLDoc.h- Contains declarations of class CDCLDoc.
* Copyright  (C) Copyright 1998-99 Visio Corporation.  All Rights Reserved.
*
*
*
* Abstract 
*
*
*
*Audit Log 
*
*
*Audit Trail
*
*
*/
#ifndef _DCLDOC_H
#define _DCLDOC_H

#if _MSC_VER > 1000
#pragma once
#endif 

class CDCLView;
class CDCLDialogList;
class TreeNode;
class Tree;

class CDCLDoc : public CDocument
{
protected: 
	CDCLDoc();
	DECLARE_DYNCREATE(CDCLDoc)
public:


public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDCLDoc)
	public:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	int ShowDialog(								// Show the dialog with the specified name.
					CString strDlgName			// i : Name of the dialog.
					);
	TreeNode * m_ptrCurrentNode;				// Store the Node pointer to show the tilename as the dialog box title. 
	CMapStringToString m_MapName2Value;			// a map to store attribute name and attribute value
	void Init();								// function which cleans up everything before opening the next dcl file
	HTREEITEM m_hParent;						// holds the parent of the treeviewnode that is to be created
	HTREEITEM m_hRoot;							// rootitem of the treeview
	CString m_strSelectedDlg;					// dialog name selected by the user
	CStringArray m_strdlgArray;					// array that holds all the dialog names in the dcl file
	
	TreeNode * m_pTreeRootNode;					// pointer to the root node of the tree 
	CString m_wszf2Open;						// dcl file name that is to be opened
	CDCLDialogList * m_pDialogList;             // a pointer to the listview
	CDCLView * m_pTreeView;                     // a pointer to the treeview
	TreeNode * GetRootNode(                     // retrieve the root node of a particular dialog
							CString& dialogName // i : dialogname whose rootnode is to be obtained
							);
	int ShowTree(								// start showing the tree
				CString& strDialogName						// i : name of the dialog
				);
	int ShowDialogList();						// to fill the listview showing the dialog names in the dcl file
	int CreateTree(								// Build the tree
					CString& strFilename        // i : FileName to be parsed 
					);                   
	int nFileID;
	
	virtual ~CDCLDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:


protected:
	//{{AFX_MSG(CDCLDoc)
	afx_msg void OnFileOpen();
	afx_msg void OnEditCut();
	afx_msg void OnTest();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	Tree * m_pTree;								// Hold the tree pointer.
};

#endif  // _DCLDOC_H
