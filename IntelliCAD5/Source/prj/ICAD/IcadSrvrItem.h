// IcadSrvrItem.h : interface of the CIcadSrvrItem class
//
#pragma once

class CIcadSrvrItem : public COleServerItem
{
	DECLARE_DYNAMIC(CIcadSrvrItem)

// Constructors
public:
	CIcadSrvrItem(CIcadDoc* pContainerDoc);
	CIcadView* m_pView;

// Attributes
	CIcadDoc* GetDocument() const
		{ return (CIcadDoc*)COleServerItem::GetDocument(); }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIcadSrvrItem)
	public:
	virtual BOOL OnDraw(CDC* pDC, CSize& rSize);
	virtual BOOL OnSetExtent(DVASPECT dwDrawAspect, const CSize& rSize);
	virtual BOOL OnGetExtent(DVASPECT dwDrawAspect, CSize& rSize);
	virtual void OnSaveEmbedding(LPSTORAGE lpStorage);
	virtual void OnOpen();
	//}}AFX_VIRTUAL

// Implementation
public:
	~CIcadSrvrItem();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
};

/////////////////////////////////////////////////////////////////////////////
