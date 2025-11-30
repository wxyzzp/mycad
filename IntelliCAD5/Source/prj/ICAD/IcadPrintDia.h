/////////////////////////////////////////////////////////////////////////////
//  Copyright (C) 1994,1995, 1996, 1997 by Visio, Inc.
//  prn_Print class
//
#pragma once

#ifndef _ICADPRINTDIA_H
#define _ICADPRINTDIA_H

#define PRN_OPT_CURRENTVIEW	0
#define PRN_OPT_SAVEDVIEW	1
#define PRN_OPT_EXTENTS		2
#define PRN_OPT_LIMITS		3
#define PRN_OPT_WINDOW		4

#define PRN_OPT_ALLENTS		0
#define PRN_OPT_SELENTS		1

#define PRN_OPT_INCHES		0
#define PRN_OPT_MM			1

class CIcadView;

typedef struct tagICPRINTINFO
{
	CIcadView* pView;
	sds_point ptLwrLeft;
	sds_point ptUprRight;
	CDC* pDC;
	double dPrinterWidth;
	double dPrinterHeight;
	double dHorzRes;
	double dVertRes;
	double dAdjustedWidth;
	double dAdjustedHeight;
	double dHeaderAdjust;
	double dFooterAdjust;
	double dViewSize;
	double dPixPerDU;
} ICPRINTINFO, FAR* LPICPRINTINFO;

class prn_Print : public CDialog
{

//*** Public Methods
public:
	prn_Print();
	virtual ~prn_Print();
	void ResetViewList();

//*** Public Properties
public:
	BOOL m_bPreview,m_bPrinting,m_bPrintDirect;
	int m_iView,m_iScale,m_iSelMode,m_iFitMode,m_iClipMode,m_iMapToBlack,m_iCenter;
	int m_nLineWidth;
	double m_dDrawingUnits,m_dPrintedUnits;
	CString m_strDrawingUnits,m_strPrintedUnits;
	CString m_strFromX,m_strFromY,m_strToX,m_strToY;
	CString m_strOriginX,m_strOriginY;
	CString m_strViewName;
	CString m_strHeader,m_strFooter;
	sds_name m_ssPrintEnts;
	CPtrList* m_pViewList;

//*** Public Methods
public:
	BOOL GetCurrentPrintInfo(LPICPRINTINFO pPrintInfo);
	BOOL GetPrintWindow(CIcadView* pView, sds_point ptLwrLeft, sds_point ptUprRight);
	struct sds_resbuf* GetCurViewRb();

//*** Protected Methods
protected:
    afx_msg BOOL OnHelpInfo(HELPINFO *pHel);
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange *pDX);
	void SetDefaults(BOOL bDisplayWarning);
	void SetPaperSize();
	void DoEndDialog(int nResult);
	BOOL UpdateFitSize(CDC* pDC = NULL);

	afx_msg void OnHelp();
	afx_msg void OnClick();
	afx_msg void OnSelectArea();
	afx_msg void OnCenter();
	afx_msg void OnSelectOrigin();
	afx_msg void OnPrintPreview();
	afx_msg void OnOk();
	afx_msg void OnCancel();
	afx_msg void OnPrintSetup();
	afx_msg void OnFitToPage();
	afx_msg void OnUnitsChanged();
	afx_msg void OnSetDefaults();
	afx_msg void OnViewSel();
	afx_msg void OnUpdateHeader();
	afx_msg void OnUpdateFooter();
	afx_msg void OnSpinLineWidth(NMHDR *pNotifyStruct,LRESULT *result);

	DECLARE_MESSAGE_MAP()
};


#endif
