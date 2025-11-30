/* File  : <DevDir>\source\prj\icad\LWeightComboBox.h
 *
 */

#ifndef	_LWEIGHTCOMBOBOX_H
#define	_LWEIGHTCOMBOBOX_H


class CLWeightComboBox : public CComboBox
{
	// inner Structure Definition to store item related data.
	struct SItemData
	{
		int width;				// Actual width stored in hundreths of mm. for example: 0, 5, 10, 50 etc.
		char strDisplay[32];	// String to be shown in MM or in Inches. for example: 0.25mm or 0.083in. 
	};
	
private:
	void AddLineWeight(void* pItemData);
	
	// Construction
public:
           CLWeightComboBox();
  virtual ~CLWeightComboBox();

// Operations
public:
	CToolTipCtrl m_LWeightToolTop; // ToolTip Control
	CRect m_ToolTipRect;		   // ToolTip Control Rect

	int m_DisplayScaleFactor;	// Display Scale from Lineweight Dialog

	void AddToolTip();

// Overrides
public:
	virtual BOOL Create(const RECT& rect, CWnd* pParentWnd);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual void DeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct);

// Implementation
public:
	void UpdateLWeightList();
	int FindLWeight (int nLineweight);
	

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
 	afx_msg void OnSelchange();
	afx_msg void OnCloseUp();
//
	DECLARE_MESSAGE_MAP()
};

#endif // _LWEIGHTCOMBOBOX_H