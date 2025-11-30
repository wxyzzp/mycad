#if !defined(AFX_ICADOLELINKSDIALOG_H__5A11C9C2_9CB6_11D1_9198_0060974FCFCA__INCLUDED_)
#define AFX_ICADOLELINKSDIALOG_H__5A11C9C2_9CB6_11D1_9198_0060974FCFCA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// IcadOleLinksDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CIcadOleUILinkInfo

class CIcadOleUILinkInfo : public IOleUILinkInfo
{
public:
	CIcadOleUILinkInfo(COleDocument* pDocument);

// Implementation
	COleDocument* m_pDocument;          // document being manipulated
	COleClientItem* m_pSelectedItem;    // primary selected item in m_pDocument
	POSITION m_pos;                     // used during link enumeration
	BOOL m_bUpdateLinks;                // update links?
	BOOL m_bUpdateEmbeddings;           // update embeddings?

	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();
	STDMETHOD(QueryInterface)(REFIID, LPVOID*);

	// IOleUILinkContainer
	STDMETHOD_(DWORD,GetNextLink)(DWORD);
	STDMETHOD(SetLinkUpdateOptions)(DWORD, DWORD);
	STDMETHOD(GetLinkUpdateOptions)(DWORD, LPDWORD);
	STDMETHOD(SetLinkSource)(DWORD, LPTSTR, ULONG, ULONG*, BOOL);
	STDMETHOD(GetLinkSource)(DWORD, LPTSTR*, ULONG*, LPTSTR*, LPTSTR*, BOOL*,
		BOOL*);
	STDMETHOD(OpenLinkSource)(DWORD);
	STDMETHOD(UpdateLink)(DWORD, BOOL, BOOL);
	STDMETHOD(CancelLink)(DWORD);
	// IOleUILinkInfo
	STDMETHOD(GetLastUpdate)(DWORD dwLink, FILETIME* lpLastUpdate);
};


/////////////////////////////////////////////////////////////////////////////
// CIcadOleLinksDialog

class CIcadOleLinksDialog : public COleDialog
{
	DECLARE_DYNAMIC(CIcadOleLinksDialog)

// Attributes
public:
	OLEUIEDITLINKS m_el;    // structure for OleUIEditLinks

// Constructors
	CIcadOleLinksDialog(COleDocument* pDoc, CView* pView, DWORD dwFlags = 0,
		CWnd* pParentWnd = NULL);

// Operations
	virtual int DoModal();  // display the dialog and edit links

// Implementation
public:
	virtual ~CIcadOleLinksDialog();
#ifdef _DEBUG
	virtual void Dump(CDumpContext& dc) const;
	virtual void AssertValid() const;
#endif

public:
	CIcadOleUILinkInfo m_xLinkInfo; // implements IOleUILinkContainer
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ICADOLELINKSDIALOG_H__5A11C9C2_9CB6_11D1_9198_0060974FCFCA__INCLUDED_)
