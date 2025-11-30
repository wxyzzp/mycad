
#pragma once

#include "IcadCusMenu.h"/*DNT*/
#include "IcadCusToolbar.h"/*DNT*/
#include "IcadCusKeyboard.h"/*DNT*/
#include "IcadCusShortcuts.h"/*DNT*/
#include "icadform.h"/*DNT*/
#include "IcadDialog.h"/*DNT*/


#ifdef _DEBUG

class CIcadCusForm : public CPropertyPage
{
    public:
        CListBox		m_FormList;
        CStatic			m_picture;
        FRM_FormEdit*	m_FormWnd;

    public:
             CIcadCusForm();
        void ShowForm(CString String);

    protected:
        virtual void DoDataExchange(CDataExchange* pDX);
        LRESULT      WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
        afx_msg void OnPaint();
        BOOL         OnInitDialog();

    DECLARE_MESSAGE_MAP()
};

class CIcadCusXlink : public CPropertyPage
{
    public:
        CListBox	m_xlinklist;
        CComboBox	m_colcombo;

    public:
        void UpdateFields(short index);
             CIcadCusXlink();

    protected:
        LRESULT      WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
        BOOL         OnInitDialog();
        virtual void DoDataExchange(CDataExchange* pDX);

    DECLARE_MESSAGE_MAP()
};

#endif	// _DEBUG


// Custom forms are not supported currently
//#define	_SUPPORT_CUSTOM_FORMS


class CIcadCustom : public IcadPropertySheet
{
public:
    CIcadCusMenu		m_CusMenu;
    CIcadCusToolbar		m_CusToolbar;
    CIcadCusKeyboard	m_CusKeyboard;
    CIcadCusShortcuts	m_Shortcuts;

#ifdef _SUPPORT_CUSTOM_FORMS
	CIcadCusXlink		m_CusXlink;
	CIcadCusForm		m_CusForm;
#endif

	CIcadCustom();

	CMDIFrameWnd*		m_pMain;
	CButton				m_CloseBtn,
						m_ImportBtn,
						m_ExportBtn,
						m_ResetBtn;
	int					m_nActiveTab;
	bool				m_bQueryUser;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	BOOL		 OnInitDialog();
	BOOL		 PreTranslateMessage(MSG* pMsg);
	//{{AFX_MSG(CIcadCustom)
	afx_msg void OnHelp();
	afx_msg void OnImportButton();
	afx_msg void OnExportButton();
	afx_msg void OnClose();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHel);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG

public:
	afx_msg void OnResetButton();
	afx_msg void OnCloseButton();

	DECLARE_MESSAGE_MAP()
};

HTREEITEM AddOneItem(HTREEITEM hParent, LPCTSTR szText, HTREEITEM hInsAfter, int iImage, int SiImage, short index, UINT mask, CTreeCtrl*);

inline void VarToCStr(CString* c, COleVariant* v);
inline void VarToBinay(BYTE** c, COleVariant* v);
