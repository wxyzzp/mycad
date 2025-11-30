// File DlgSpoolingSettings.h
//

#pragma once


class CDlgSpoolingSettings : public CDialog
{
public:
	CDlgSpoolingSettings(CWnd* pParent = NULL);

	enum { IDD = PRINT_SPOOLING };
	BOOL	m_bForcePrintToFile;
	BOOL	m_bUseOutputFolder;
	CString	m_outputFolder;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	afx_msg void OnBrowse();
	afx_msg void OnForcePrintToFile();
	afx_msg void OnUsePath();
	afx_msg void OnChangePath();
	DECLARE_MESSAGE_MAP()
};
