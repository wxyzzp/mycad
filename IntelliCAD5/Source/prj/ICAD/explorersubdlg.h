/* icad\ExplorerSubDlg.h
 * Copyright (C) 1997-1999 Visio Corporation. All Rights Reserved.
 *
 * Abstract
 *
 * Header file for the properties tab displayed by IntelliCAD Explorer.
 * IEXP_SubDlg serves as the base class for CExplorerPropsTab, which
 * was created to support TrueType fonts.
 *
 * NOTE:  How IntelliCAD Explorer deals with tabs needs to be redone.
 *        The class/file dependencies also need to be simplified.
 *
 */

#if !defined EXPLORER_SUB_DLG
#define EXPLORER_SUB_DLG


class IEXP_CdetailView;

class IEXP_SubDlg : public CDialog {
    public:
        long                 m_dataptr;
        int                  m_color;
        short                m_oldindex,
                             m_oldDlgIndex,
                             m_DlgIndex,
                             m_Vindex;
        BOOL                 m_Inuse,
                             m_newcolor,
                             m_varies,
                             m_colorVaries;
        IEXP_CdetailView    *m_CdetailView;
        CButton              m_ColorB;
        CComboBox            m_combobox;
        CComboBox		      m_lwcombobox;	// For LineWeight
        CWnd                *m_CItem;
        CString              m_string;

    public:
		inline short GetOldIndex(void);

   	    BOOL OnInitDialog();
        BOOL TableInfoConstruct();
        void ColorButton(BOOL Varies);
        void SaveTabChanges();

    protected:
        virtual BOOL OnCommand(WPARAM wParam,LPARAM lParam);
                void DoDataExchange(CDataExchange *pDX);
        afx_msg void OnPaint();

    DECLARE_MESSAGE_MAP()
};


#endif  // EXPLORER_SUB_DLG
