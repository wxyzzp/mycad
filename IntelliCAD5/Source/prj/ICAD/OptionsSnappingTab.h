	/* F:\ICADDEV\PRJ\ICAD\OPTIONSSNAPPINGTAB.H
	 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
	 * 
	 * Abstract
	 * 
	 * <TODO : Add Abstract here>
	 * 
	 */ 
	
	
	#if !defined(AFX_OPTIONSSNAPPINGTAB_H__20BA0F08_8F4E_4B62_8E51_A2573DF604F7__INCLUDED_)
	#define AFX_OPTIONSSNAPPINGTAB_H__20BA0F08_8F4E_4B62_8E51_A2573DF604F7__INCLUDED_
	
	#if _MSC_VER > 1000
	#pragma once
	#endif // _MSC_VER > 1000
    // OptionsSnappingTab.h : header file

	#include "OptionsSnapDlgPic.h"
//	// This file added by CyberAge VSB 12/05/2001 
//	// Reason: Implementation of Flyover Snapping.
    
    /////////////////////////////////////////////////////////////////////////////
    // OptionsSnappingTab dialog
    
    class OptionsSnappingTab : public CPropertyPage
    {
    	DECLARE_DYNCREATE(OptionsSnappingTab)
    
    // Construction
    public:
    	OptionsSnappingTab();
    	~OptionsSnappingTab();
    
    // Dialog Data
    	//{{AFX_DATA(OptionsSnappingTab)
    	enum { IDD = OPTIONS_SNAPPING };
		COptionsSnapDlgPic	m_cDlgPicture;
		BOOL	m_bEnableFlyOver;
		BOOL	m_bDisplayMarker;
		BOOL	m_bDisplayTooltips;
		BOOL	m_bDisplayApertureBox;
		BOOL	m_bDrawMarkerInAllViews;
		BOOL	m_bShowOsnapCursorDecor;
		int		m_iMarkerSize;
		int		m_iMarkerThickness;
//		int		m_nMarkerSize;
//		int		m_nMarkerThickness;
//		BOOL	m_bAperture;
//		BOOL	m_bFlyover;
//		BOOL	m_bSnapView;
//		BOOL	m_bTooltips;
//		BOOL	m_bMarker;
    	//}}AFX_DATA
    
    public:
		int m_iMarkerColor;
	
//		BOOL DrawMarkerPreview();
//		int m_nMarkerColor ;
    // Overrides
    	// ClassWizard generate virtual function overrides
    	//{{AFX_VIRTUAL(OptionsSnappingTab)
    	public:
    	virtual void OnOK();
    	protected:
    	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    	//}}AFX_VIRTUAL
    
    // Implementation
    protected:
    	// Generated message map functions
    	//{{AFX_MSG(OptionsSnappingTab)
		virtual BOOL OnInitDialog();
		afx_msg void OnSnapMarkercolor();
    	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
		afx_msg void OnDeltaposSnapSizeUpdn(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnDeltaposSnapThicknessUpdn(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnEnableCheck();
//		afx_msg void OnScreenColor();
//		afx_msg void OnDeltaposSpin1(NMHDR* pNMHDR, LRESULT* pResult);
//		afx_msg void OnDeltaposSpin2(NMHDR* pNMHDR, LRESULT* pResult);
//		afx_msg void OnPaint();
//		afx_msg void OnCheckEnableFlyOverSnap();
//		afx_msg void OnCheckDisplayToolTips();
//		afx_msg void OnCheckDisplayMarker();
//	
//		// Modified CyberAge VSB 11/06/2001 
//		// Aperture Box settings should be independant of Flyover Snap [	
//		//afx_msg void OnCheckAperture();
//		// Modified CyberAge VSB 11/06/2001 ]
//		
//		// Modified CyberAge VSB 20/05/2001 
//		// To Synchronise with Enable Flyover Snap option.[	
//		afx_msg void OnCheckSnapView();	
//		// Modified CyberAge VSB 20/05/2001 ]
    	//}}AFX_MSG
    	DECLARE_MESSAGE_MAP()
	private:
		void enableControls( BOOL bEnable );
    
    };
    
    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
    
#endif // !defined(AFX_OPTIONSSNAPPINGTAB_H__20BA0F08_8F4E_4B62_8E51_A2573DF604F7__INCLUDED_)
