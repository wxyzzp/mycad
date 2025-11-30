/* File  : <DevDir>\source\prj\icad\ColorComboBox.h
 *
 */

#ifndef	_COLORCOMBOBOX_H
#define	_COLORCOMBOBOX_H


class CColorComboBox : public CComboBox
{
// Construction
public:
  CColorComboBox();
  virtual ~CColorComboBox ();

// Attributes
private:
	bool m_bInitialized;

private:
	void Initialize( void ); // Initialize control

// Overrides
public:
	CToolTipCtrl m_ColorToolTip; // ToolTip Control
	CRect m_ToolTipRect;		   // ToolTip Control Rect
	void AddToolTip();
	void UpdateColorList(); 

	virtual BOOL Create(const RECT& rect, CWnd* pParentWnd);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
  virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
// Implementation
public:

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
 	afx_msg void OnSelchange();
	afx_msg void OnCloseUp();

	DECLARE_MESSAGE_MAP()
};

#endif // _COLORCOMBOBOX_H