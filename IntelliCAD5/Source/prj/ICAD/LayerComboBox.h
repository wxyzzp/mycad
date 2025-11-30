/* File  : <DevDir>\source\prj\icad\LayerComboBox.h
 *
 */

#ifndef	_LAYERCOMBOBOX_H
#define	_LAYERCOMBOBOX_H

#define IMG_WIDTH 15
#define IMG_HEIGHT 15
#define IMG_OFFSET 2

class CLayerComboListBox : public CListBox
{
// Construction
public:
  CLayerComboListBox();
  virtual ~CLayerComboListBox();
  void DrawItem(RECT rcItem, int nState, int nItem, HDC hDC, LPCTSTR lpszText);

protected:
  int m_nPanelWidth;

  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
  virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
  void DrawBitmap(CDC *pDC, UINT nResourceID, int nLeft, int nTop);

	DECLARE_MESSAGE_MAP()
};

class CLayerComboBox : public CComboBox
{
// Construction
public:
  CLayerComboBox();
  virtual ~CLayerComboBox ();

// Operations
public:
	CToolTipCtrl m_LayerToolTip; // ToolTip Control
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
  void UpdateLayerList();

protected:
  int m_nCurSelItem;
  BOOL m_bSubclassedListBox;
  CLayerComboListBox m_wndListBox;

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
 	afx_msg void OnSelchange();
  afx_msg void OnDropDown();
	afx_msg void OnCloseUp();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()
};


#endif // _LAYERCOMBOBOX_H