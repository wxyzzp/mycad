/* File  : <DevDir>\source\prj\icad\DimStyleComboBox.h
 *
 */

#ifndef	_DIMSTYLECOMBOBOX_H
#define	_DIMSTYLECOMBOBOX_H


class CDimStyleComboBox : public CComboBox
{

	
	// Construction
public:
  CDimStyleComboBox();
  virtual ~CDimStyleComboBox ();
  CToolTipCtrl m_ToolTipCtrl;
  CRect m_rect;

// Operations
public:

// Overrides
	virtual BOOL PreTranslateMessage(MSG* pMsg);
public:
	virtual BOOL Create(const RECT& rect, CWnd* pParentWnd);
// Implementation
public:
  void UpdateDimStyleList();
  void AddToolTip();

protected:
	afx_msg void OnSelchange();
	afx_msg void OnDropDown();
	afx_msg void OnCloseUp();
	DECLARE_MESSAGE_MAP() 




};




#endif // _DIMSTYLECOMBOBOX_H