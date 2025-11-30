/* F:\ICADDEV\PRJ\ICAD\OPTIONPATHLISTCTRL.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

// OptionPathListCtrl.h: interface for the OptionPathListCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPTIONPATHLISTCTRL_H__5B31CB49_F750_11D2_9FBE_00C04F68E33B__INCLUDED_)
#define AFX_OPTIONPATHLISTCTRL_H__5B31CB49_F750_11D2_9FBE_00C04F68E33B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ListCtrlEx.h"

class OptionPathListCtrl  : public CListCtrlEx
{
public:
	enum EditType {FILE, PATH};

	OptionPathListCtrl(EditType type);
	virtual ~OptionPathListCtrl();

	BOOL ValidateData();
	BOOL ValidateFileData();
	BOOL ValidatePathData();
	
	CEdit* EditSubLabel(int row, int column);
	CEdit* EditInvalidSubLabel();

protected:
	//{{AFX_MSG(OptionPathListCtrl)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	int m_editRow;		//Row/column currently being edited.  Used for validate data
	int	m_editColumn;		
	bool m_editing;		//flag indicating whether the user was editing the data in one of the controls

	EditType m_editType;

	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_OPTIONPATHLISTCTRL_H__5B31CB49_F750_11D2_9FBE_00C04F68E33B__INCLUDED_)


