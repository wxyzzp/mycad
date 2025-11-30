// IcadCntrItem.h : interface of the CIcadCntrItem class
//

class CIcadDoc;
class CIcadView;
class CIcadRectTracker;

class CIcadCntrItem : public COleClientItem
{
	DECLARE_SERIAL(CIcadCntrItem)

// Constructors
public:
	CIcadCntrItem(CIcadDoc* pContainer = NULL, db_handitem* pHandItem = NULL);
		// Note: pContainer is allowed to be NULL to enable IMPLEMENT_SERIALIZE.
		//  IMPLEMENT_SERIALIZE requires the class have a constructor with
		//  zero arguments.  Normally, OLE items are constructed with a
		//  non-NULL document pointer.

// Attributes
public:
	db_handitem* m_pHandItem;
	CIcadRectTracker* m_pTracker;
	BOOL m_bUpdateFailedOnSave;
	double m_HMUperDUx,m_HMUperDUy;
	BOOL m_bInUpdateHandItem;
	int m_nDefaultX,m_nDefaultY;
	struct gr_view* m_pGrView;

	CIcadDoc* GetDocument()
		{ return (CIcadDoc*)COleClientItem::GetDocument(); }
	CIcadView* GetActiveView()
		{ return SDS_CURVIEW; }

	BOOL UpdateLink();
	BOOL UpdateFromServerExtent(CIcadView* pView);
	BOOL InitHandItem(CIcadView* pView);
	BOOL UpdateBounds(CIcadView* pView, LPCRECT lpBounds);
	BOOL UpdateBounds(CIcadView* pView, sds_point ptUL, sds_point ptLR);
	BOOL UpdateHandItem(CIcadView* pView, BOOL bForceSave = FALSE);
	BOOL ResetStorage(db_handitem* pNewHandItem);
	BOOL ReadAcadOLEData(CIcadView* pView);
	BOOL Draw(CIcadView* pView, CDC* pDC, struct gr_view *pGrView = NULL,
		DVASPECT nDrawAspect = (DVASPECT)-1);
	void SetupTracker(CIcadView* pView, LPCRECT lpRect, CIcadRectTracker* pTracker = NULL);
	void SetServerSize(CIcadView* pView, LPCRECT lpRect);

	BOOL GetCurRect(CIcadView* pView, CRect* pRect);
	void RectToPoints(CIcadView* pView, LPCRECT lpRect, sds_point ptLowerRight,
		sds_point ptUpperLeft);
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIcadCntrItem)
	public:
	virtual void OnChange(OLE_NOTIFICATION wNotification, DWORD dwParam);
	virtual void OnActivate();
	virtual BOOL OnChangeItemPosition(const CRect& rectPos);
	virtual void GetItemStorage();
	protected:
	virtual void OnGetItemPosition(CRect& rPosition);
	virtual void OnDeactivateUI(BOOL bUndoable);
	virtual BOOL CanActivate();
	virtual void OnInsertMenus(CMenu* pMenuShared, LPOLEMENUGROUPWIDTHS lpMenuWidths);
	virtual void OnRemoveMenus(CMenu* pMenuShared);
	//}}AFX_VIRTUAL

// Implementation
public:
	~CIcadCntrItem();

	BEGIN_INTERFACE_PART(IcadOleClientSite, IOleClientSite)
		INIT_INTERFACE_PART(CIcadCntrItem, IcadOleClientSite)
		STDMETHOD(SaveObject)();
		STDMETHOD(GetMoniker)(DWORD, DWORD, LPMONIKER*);
		STDMETHOD(GetContainer)(LPOLECONTAINER*);
		STDMETHOD(ShowObject)();
		STDMETHOD(OnShowWindow)(BOOL);
		STDMETHOD(RequestNewObjectLayout)();
	END_INTERFACE_PART(IcadOleClientSite)
	DECLARE_INTERFACE_MAP()

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	virtual void Serialize(CArchive& ar);
};

/////////////////////////////////////////////////////////////////////////////
