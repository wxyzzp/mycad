/* File  : <DevDir>\source\prj\icad\LTypeComboBox.h
 *
 */

#ifndef	_LTYPECOMBOBOX_H
#define	_LTYPECOMBOBOX_H


class CLTypeComboBox : public CComboBox
{
// Construction
public:
  CLTypeComboBox();
  virtual ~CLTypeComboBox ();

// Operations
public:

	CToolTipCtrl m_LTypeToolTip; // ToolTip Control
	CRect m_ToolTipRect;		   // ToolTip Control Rect
	void AddToolTip();
// Overrides
public:
	virtual BOOL Create(const RECT& rect, CWnd* pParentWnd);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
  virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// Implementation
public:
  void UpdateLTypeList();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
 	afx_msg void OnSelchange();
  afx_msg void OnDropDown();
	afx_msg void OnCloseUp();

	DECLARE_MESSAGE_MAP()
};

#endif // _LTYPECOMBOBOX_H