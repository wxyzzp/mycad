/* D:\ICADDEV\PRJ\ICAD\ICADVIEW.H
* Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
*
* Abstract
*
* CIcadView class-- drawings, viewports
*
*/

#pragma once

#include "IcadApi.h"
#include "gr.h"
#include <afxole.h>
#include <afxcmn.h>

#include "IcadToolTip.h"

extern "C" short cmd_vmlout(void);
class CRealTime;
class CDrawDevice;
class CMainWindow;
class CIcadCntrItem;
class CFrameBuffer;

const UINT RealTimeTimer_id = 1974;		// some ID
const UINT RealTimeTimer_time = 26;		// elapse time

enum RealTimeMotions
{
	NO,
		ZOOM,
		ZOOM_WHEEL,
		PAN,
		ROT_CYL,
		ROT_CYL_X,
		ROT_CYL_Y,
		ROT_CYL_Z,
		ROT_SPH
};

// // Modified CyberAge VSB 12/05/2001
// // Reason: Implementation of Flyover Snapping.
// typedef struct tagsnapstruct {
// 	sds_point pt;
// 	int nSnappedType;
// 	bool bValidSnap;
// }SNAPSTRUCT;
//
// class	SnapInfo {
//
// public :
//
// 	int snapsize;
// 	int thickness;
// 	int bkgcolor;
// 	int snapcolor;
// 	int osmode;
// 	int showtooltips;
// 	int autosnap;
// 	int marker;
// 	int nNextSnap;
// 	int m_nSnapDrawn;
// 	int m_LastSnapPtPixX;
// 	int m_LastSnapPtPixY;
//
// 	CDC *pDC;
// 	struct gr_view *m_gr_view;
// 	sds_point m_lastsnappoint;
// 	SNAPSTRUCT TempSnapStruct;
//
// 	SnapInfo(){
//
// 				nNextSnap= 0;
// 				snapsize =0;
// 				thickness=0;
// 				bkgcolor=0;
// 				snapcolor=0;
// 				osmode=0;
// 				showtooltips=0;
// 				autosnap=0;
// 				marker=0;
// 				nNextSnap=0;
// 				m_nSnapDrawn=0;
// 				pDC = NULL;
// 				m_gr_view = NULL;
// 				m_lastsnappoint[0] = 0;m_lastsnappoint[1] = 0;m_lastsnappoint[2] = 0;
//
// 				ic_ptcpy(TempSnapStruct.pt,m_lastsnappoint);
// 				TempSnapStruct.bValidSnap = FALSE;
// 				TempSnapStruct.nSnappedType = 0;
// 	}
//
// 	SNAPSTRUCT TryEntitySnap(CPoint &Point);
// 	SNAPSTRUCT GetNextSnapData();
// 	void GetLatestValues();
// 	void GetSnapString(char *snapstr,int *osnapmode);
// 	void GetToolTipText(char * szText, int nSnapDrawn);
//
// 	void DeleteSnap();
// 	// Modified CyberAge VSB 20/05/2001
// 	// To differentiate from where this function is called and differ the behaviour.
// 	// void DrawSnap(SNAPSTRUCT &SnapData);
// 	void DrawSnap(SNAPSTRUCT &SnapData, int Drag = 0);
//
// 	void DrawSnapForEND(int ptx, int pty, CPen &pen);
// 	void DrawSnapForMID(int ptx, int pty, CPen &pen);
// 	void DrawSnapForINT(int ptx, int pty, CPen &pen);
// 	void DrawSnapForNEA(int ptx, int pty, CPen &pen);
// 	void DrawSnapForCEN(int ptx, int pty, CPen &pen);
// 	void DrawSnapForQUA(int ptx, int pty, CPen &pen);
// 	void DrawSnapForTAN(int ptx, int pty, CPen &pen);
// 	void DrawSnapForPER(int ptx, int pty, CPen &pen);
// 	void DrawSnapForINS(int ptx, int pty, CPen &pen);
// 	void DrawSnapForNOD(int ptx, int pty, CPen &pen);
// };
//
// // Modified CyberAge VSB 12/05/2001 ]
/*DG - 2.12.2002*/// Comment out the following defining if you don't want to use icad's spooling features
// (e.g. print to PRN file directly w/o interaction with user, store output printing files to a particular folder).
// NOTE: Keep in sync USE_ICAD_SPOOLING definings in dlgspoolingsettings.cpp, icadview.h and preferences.h files!
#define USE_ICAD_SPOOLING

class CIcadView : public CView
{
	DECLARE_DYNCREATE(CIcadView)
		
public:
	
	enum ViewMode
	{
		kNormal = 0,
			kHiddenLine,
			kShaded
	};
	
	CIcadView();
	virtual ~CIcadView();
	CIcadDoc* GetDocument();
	db_drawing *GetDrawing();
	void SetNewView(bool bSetFocus, bool callback=TRUE);
	void UpdateCoords(sds_point pt,int coords);
	void CalcViewPoints(int UpdateTab);
	
	CMainWindow* m_pMain;
	CIcadCntrItem* m_pSelection;
	COleDropTarget m_dropTarget;
	
	//*** Cursor member variables
public:
	void CursorDown( CPoint);
	bool CursorOn( bool);
	UINT CursorID( UINT);
	
	bool	m_CursorOn;
	CPoint  m_LastCursorPos;
	short   m_LastCursorType;
	
protected:
	HCURSOR m_prevCursor;
	UINT    m_idcCursor; //*** ID for the cursor to be displayed in the view window.
	short   m_CursorType;
	CPoint  m_CursorDownPos;
	
	CIcadToolTip m_toolTip;
	mutable CString m_toolTipText;
	
public:
	CRect   m_rectMspace;
	BOOL    m_bEatNextMouse;
	int     m_iViewMode;  // 0=Normal 1=Hidden line 2=Shaded.
	
	int     m_iWinX;
	int     m_iWinY;
	
	short   m_fUseRegenList;
	int     m_LastCol,m_LastHl,m_LastRop,m_LastWid;
	int     m_FillLastCol,m_FillLastHl,m_FillLastRop;
	BOOL    m_bMakingSld;
	BOOL    m_bNeedRedraw;
	BOOL    m_bReScaleOnce;
	struct SDS_prevview *m_pZoomPrevB,*m_pZoomPrevC;
	struct SDS_prevview *m_pZoomPsPrevB,*m_pZoomPsPrevC;
	int     m_iPrevViews;
	int     m_iPrevPsViews;
	CRect  *m_pClipRectDC;
	CRect  *m_pClipRectMemDC;
	struct resbuf *m_pDragVects;
	double m_dViewSizeAtLastRegen;
	//*** Save varaibles for printing
	BOOL	  m_bAltView;
	sds_real  m_rPrintViewSize,m_rPrintLensLength,m_rPrintFrontZ,m_rPrintBackZ,
		m_rPrintViewTwist;
	sds_point m_ptPrintViewCtr,m_ptPrintViewDir,m_ptPrintTarget,m_ptPrintScreenSize;
	int		  m_nPrintViewMode;
	sds_point m_ptLwrLeft,m_ptUprRight;
	CRect	  m_PrintClipRect;
	CSize	  m_sizePrintView;
	BOOL	  m_bNeedRestoreView;
	bool	  m_showScrollBars;
	
	//4M Bugzilla 77987 19/02/02->
private:
	//BugZilla No. 78266; 19-08-2002
	bool m_bLastInPlaceActiveItem;
	//<-4M 19/02/02
	CRealTime*      m_pRealTime;
	RealTimeMotions	m_RTMotion;
		
	//4M Bugzilla 77987 19/02/02->
public:
	CRealTime * GetRealTime(){return m_pRealTime;}
	RealTimeMotions GetRTMotion()const {return m_RTMotion;}
	void SetRealTime(CRealTime * rt);
	void DeleteRealTime(UINT id=RealTimeTimer_id);
	void SetRTMotion(RealTimeMotions mt);
	//<-4M 19/02/02
	//4M Item:96
	db_drawing * PrevDrawing;
	//<-4M Item:96
	//*** Entity drawing variables
	POINT  *m_pPolyPts;
	int     m_nPolyPts;
	//*** Pointer to this view's dwg struct
	CIcadDoc* m_pViewsDoc;
	struct gr_view *m_pCurDwgView;
	db_handitem *m_pVportTabHip;
	short m_RectBitBlt;
	POINT m_RectBitBltPt[2];
	//*** Printing member functions
	virtual void FilePrint(BOOL bShowPrintDlg);
	virtual void FilePrintPreview(void);
		
#ifdef USE_ICAD_SPOOLING
	void customOnFilePrint();
#endif
		
	//Added Cybage AW 15/11/2001[
	//Bug no : 77870 from BugZilla (Crash during print preview )
	virtual void OnEndPrintPreview( CDC* pDC, CPrintInfo* pInfo, POINT point, CPreviewView* pView );
	//Added Cybage AW 15/11/2001]
		
	void PrintHeaderOrFooter(CDC* pDC, BOOL bPrintHeader);
	void SavePrintViewInfo(struct sds_resbuf* pRb);
	void RestorePrintViewInfo();
	void OnInsertObject(LPCTSTR pszFileName);
	void OnInsertObject();
	CIcadCntrItem* HitTestItems(CPoint point);
	void SetSelection(CIcadCntrItem* pItem, BOOL bForceRedrawAll = FALSE);
	virtual void OnDraw(CDC* pDC);
		
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
		
	void ShowScrollBar(UINT nBar, BOOL bShow = TRUE);
	void UpdateScrollBars(void);
	void CopyToClipboard();
	BOOL CreatePictureFromSS(sds_name ssEnts, HBITMAP* phBmp,
			HENHMETAFILE* phEnhMetaFile, ICADWMFINFO* pWmfInfo, HANDLE* phIcadData,
			BOOL bPaintBkgnd, BOOL bHIMETRIC = FALSE);
	void EsnapContextMenu(CPoint point);
	void OleContextMenu(CPoint point);
	void OnOleContextMenuPick(UINT nId);
	
	void processOsnapChar( UINT nChar );
	void updateToolTip(){ m_toolTip.Update(); }
	
	int SaveBitmapToBuffer( char **ppBuffer, long *bytes, bool ChgBackgnd );
	
	// 	// Modified CyberAge VSB 12/05/2001
	// 	// Reason: Implementation of Flyover Snapping.[
	// 	SnapInfo SnapObject;			//Main Flyover Snap object.
	// 	CToolTipCtrl m_ctlToolTip;		//Tooltip ctrl for flyover snap
	// 	CPoint m_ptPrevMouseMovePt;		//Previous mouse move point stored as they should not be repeated once.
	// 	// Modified CyberAge VSB 12/05/2001 ]
	//
	// 	// Modified CyberAge VSB 20/05/2001
	// 	// Reason: Implementation of Flyover Snapping.[
	// 	int m_iFlyOverSnapX;			//Stores  the snap points, used to drawsnap while dragging.
	// 	int m_iFlyOverSnapY;			//Stores  the snap points, used to drawsnap while dragging.
	// 	int m_nFlyOverSnapDrawn;		//Stores  if snap is drawn,used while dragging.
	// 	// Modified CyberAge VSB 20/05/2001 ]
	// This is an optimization.  Ideally we wouldn't keep a draw device
	// around all the time, but new and delete are slow and we sometimes
	// use these a lot
	//
	// DP: temporarily give access to m_pFrameBufferDrawDevice for these functions
	friend void SDS_DrawEntity(db_handitem* pEntity, CDC* pDC, db_drawing* pDrawing, CIcadView* pView);
	friend short cmd_vmlout(void);
	
	// DP: These functions exists for compatibility only. Do not use. Use instead GetDrawDevice() and ReleaseDrawDevice()
	CDrawDevice* GetFrameBufferDrawDevice() { return m_pFrameBufferDrawDevice; }
	HDC	GetFrameBufferDC();
	CDC* GetFrameBufferCDC();
	
	CDC *GetTargetDeviceCDC( void );
	bool IsPrinting( void );
	
	CDrawDevice* GetDrawDevice();
	bool ReleaseDrawDevice(CDrawDevice* pDevice);
	
	
	BOOL m_bIsTempView;
	BOOL m_bDontDelInTab;
	
	BOOL m_bSizeInPlace;
	BOOL m_bInPlaceDeactivated;
	double m_dPreInPlaceViewSize;
	
	bool		m_bHaveMagneticGrip;	/*D.G.*/// 'true' when cursor is attracted by some grip point
	sds_point	m_magneticGrip;			// exact value of the magnetic grip point
	
// PRIVATE OPERATIONS
//
private:
	void	setPrinterDC( CDC *pCDC );
	void	clearPrinterDC( void );
	CDC		*getPrinterDC( void );
		
	// AG: custom tooltip processing
	void FilterToolTipMessage( MSG *pMsg );
	void RelayToolTipMessage( const MSG *pMsg );
		
private:
		
	// This is an optimization.  Ideally we wouldn't keep a draw device
	// around all the time, but new and delete are slow and we sometimes
	// use these a lot
	//
	CDrawDevice			*m_pFrameBufferDrawDevice;
		
	// This is only set if we are currently printing
	// otherwise it is NULL
	//
	CDC						*m_pCurrentPrinterDC;
	CDrawDevice			*m_printerDrawDevice;
	CPalette				*m_oldPrinterPalette;
		
	// EBATECH(shiba)-[ 2001-06-12 Clear afterimage by command interrupt RTZOOMPAN etc.
	CPoint					m_lastMBDownPoint;	// Store the last mouse button down point
	BOOL					m_bMButtonClick;	// Click judge flag OnMButtonUp()
	// ]-EBATECH(shiba) 2001-06-12
		
protected:
	
	//{{AFX_MSG(CIcadView)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint(void);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point); // SMC(Maeda)
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	afx_msg BOOL OnQueryNewPalette();
	afx_msg void OnPaletteChanged(CWnd* pFocusWnd);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	afx_msg virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	afx_msg virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	afx_msg virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	afx_msg virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	// Ole messages
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg virtual BOOL IsSelected(const CObject* pDocItem) const;// Container support
	afx_msg void OnCancelEditCntr();
	afx_msg void OnCancelEditSrvr();
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState,
		CPoint point);
	virtual DROPEFFECT OnDropEx(COleDataObject* pDataObject, DROPEFFECT dropDefault,
		DROPEFFECT dropList, CPoint point);
	//*** Cursor member functions
	afx_msg void OnCaptureChanged(CWnd* pWnd);
	virtual void OnActivateView( BOOL bActivate, CView* pActivateView, CView* pDeactiveView );
	
	//*** ToolTip member functions
	afx_msg BOOL OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);
	virtual int OnToolHitTest( CPoint pt, TOOLINFO* pTI ) const;
	
	DECLARE_MESSAGE_MAP()
};
