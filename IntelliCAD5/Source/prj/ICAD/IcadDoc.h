//////////////////////////////////////////////////////////////////////
// Icad Document class
#pragma once

#include "IcadATL.h"
namespace ICAD_AUTOMATION
{
#include "IcadTLB.h"
}
using namespace ICAD_AUTOMATION;
#include "IcadAPC.h"
#include "AutoDoc.h"
#ifdef BUILD_WITH_VBA
#include "IcadVbaMacro.h"
#endif
#include "db.h"
#include "icadapi.h"
#include <map>

class db_viewport;
class db_vporttabrec;
class db_viewtabrec;

class CIcadSrvrItem;
class CIcadMenu;
class CIcadCntrItem;
class SDSThreadJob;

class CDrawHelper
{
public:
	CDrawHelper(): m_bEnable(false){}
	void reset() { m_data.clear(); }
	void add(CIcadView* pView, bool bRegen);
	void process(CIcadDoc* pDoc);
	void setEnable(bool bOn) { m_bEnable = bOn; }
	bool isEnable() const { return m_bEnable; }

protected:
	typedef std::map<CIcadView*, std::pair<int, int> > MMap;
	MMap m_data;
	bool m_bEnable;
};

class CCmdHelper
{
public:
	CCmdHelper(): m_bCheckCallback(false), m_bOopsAvailable(false){}
	~CCmdHelper() { clearOops(); }
	void oopsAddErased(db_handitem* hip);
	void oopsAddModified(db_handitem* hip, db_BinChange* change,
		const std::vector<db_handitem*>& initiators);
	void oopsProcess(CIcadDoc* pDoc);
	bool isCheckCallback() const { return m_bCheckCallback; }
	void callbackChanged(db_handitem* hip, bool bUndo);
	bool isOopsAvailable() const { return m_bOopsAvailable; }
	void setOopsAvailable(bool bOn);

protected:
	void clearOops();
	void setCheckCallback(bool bOn) { m_bCheckCallback = bOn; }

protected:
	class CModifiedData
	{
	public:
		CModifiedData(): m_change(NULL){}
		~CModifiedData();
		std::vector<db_handitem*> m_initiators;
		db_BinChange* m_change;
	};
	typedef std::map<db_handitem*, int> MErased;
	typedef std::map<db_handitem*, std::pair<int, CModifiedData*> > MModified;
	MErased m_erased;
	MModified m_modified;
	bool m_bCheckCallback;
	bool m_bOopsAvailable;
};

class CIcadDoc : public CApcDocument<CIcadDoc,COleServerDoc>,
				 public CApcProjectItem<CIcadDoc>,
				 public CIcadRecorder
{
protected: // create from serialization only
	CIcadDoc();
	DECLARE_DYNCREATE(CIcadDoc)

	BOOL m_bDesignMode;
	BOOL m_bVbaProjectSaved;
	BOOL LockDocument(LPCTSTR lpszPathName);
	void UnlockDocument();

public:
	BOOL m_bInIPResize;
	CSize m_ZoomNum,m_ZoomDenom;
	CSize m_sizeDoc;
	CDrawHelper m_drawHelper;
	CCmdHelper m_cmdHelper;

// Attributes
public:
	CIcadSrvrItem* GetEmbeddedItem()
		{ return (CIcadSrvrItem*)COleServerDoc::GetEmbeddedItem(); }

	virtual db_viewport *GetCurrentViewport( void );
	bool	m_bCallSSchangeCB;	// 'true' if we should call notification callbacks when the current SS (m_pGripSelection) was changed

//4M Item:25->
// Replacement of SDS_NodeList with SDS_NodeList_new
/*
    SDS_NodeList m_nodeList;
*/
    SDS_NodeList_New m_nodeList;
//<-4M Item:25
//4M Item:28->
   CGripNodesArray m_gripsToDraw,m_gripsToUndraw;
//<-4M Item:28

    //BugZilla No. 78155; 27-05-2002
    struct cmd_dimlastpt lastdimpts;
	
    sds_name m_pGripSelection;
	sds_name m_pPrevSelection;
	sds_name m_pOopsSelection;				  // *** Stores the last selection set of entities that were erased.
	struct SDS_ucs_llist *m_pucs_llbeg,*m_pucs_llcur;

	COleClientItem* CreateOleItem(CView* pView, db_handitem* pHandItem);
	BOOL DeleteOleItem(db_handitem* pHandItem);
	BOOL DrawOleItem();
	CIcadCntrItem* GetOleItem(db_handitem* pHandItem);
	CView* m_pViewForRedraw;
	db_handitem* m_pHandItemForRedraw;
	CDC* m_pDCForRedraw;
	struct gr_view *m_pGviewForRedraw;

	db_drawing *m_dbDrawing;
	bool m_bDocIsClosing;
	bool m_bDidFirstRegen;
	class CIcadView *m_RegenListView;
	struct SDS_textdtext_globalpacket *m_pGpaktxt,*m_pGpakatt;
	int m_nOpenViews;
	int m_nCvportOnOpen;

	CIcadMenu* m_pIcadMenu;

	CComObject<CAutoDoc>* m_pAutoDoc;

	HRESULT ApcProjectItem_View();
	HRESULT ApcProject_Save();
	HRESULT DefineProjectItem();
	HRESULT ApcProjectItem_Activate();
	BOOL CheckDesignMode();
	void FireEvent(int id);
	HANDLE m_hVbaSdsEvent;

	HANDLE m_hFile;
	BOOL m_bOpenReadOnly;	//*** The file is not read only, but the user wishes to open it read only.
	BOOL m_bIsReadOnly;		//*** The file's read-only attribute is set.
	BOOL m_bAlreadyOpen;	//*** The file is open in another application.

	BOOL m_bSaveToStorage;

	int m_nFileVersion;
	int m_nFileType;
	int m_nLastMsVport;

	double m_rRectang_ang;
	double m_rRectang_thick;
	double m_rRectang_width;
	double m_rRectang_elev;
	double m_rRectang_fill;
	double m_rRectang_chama;
	double m_rRectang_chamb;

	int m_rRectang_mode;	//Bit-coded modes for cmd_rectang_mode

	RGBQUAD	*m_DocPalette;	// EBATECH(CNBR) 2001.06.24 for OEM App.
	BOOL	createDocPalette();	// EBATECH(CNBR) 2001.06.24 for OEM App.

	//Automation Helpers - these are functions that Automation calls so that Automation does not directly
	//depend on cmds, sds etc.
	HANDLE GetVbaSdsEvent();
	void PostJobToMainEngineThread (SDSThreadJob *pJob);
	virtual HRESULT PurgeDrawing ();
	virtual HRESULT RegenAllViews ();
	virtual HRESULT RegenCurrentView ();
	virtual HRESULT RedrawAllViews ();
	virtual HRESULT RedrawCurrentView ();
	virtual HRESULT SplitView (CIcadView *pView, int nHowMany, int nOrientation);
	virtual HRESULT SetView (db_vporttabrec *pVPort, db_viewtabrec *pView);
	virtual HRESULT SaveViewportConfiguration (char *vp_name);
	virtual HRESULT RestoreViewportConfiguration (char *vp_name);
	virtual HRESULT InsertBlock (char *blkname, db_insert *ins, sds_point inspt, db_insert **newins, bool explode = FALSE);
	virtual HRESULT DeleteEntity (db_handitem *hip);
	virtual HRESULT RedrawEntity (db_handitem *hip, redraw_mode mode);
	virtual HRESULT UpdateEntity (db_handitem *hip);
	virtual HRESULT XformEntity (db_handitem *hip, sds_matrix matrix);
	virtual HRESULT GetActiveSelectionSet(long* pLength, sds_name ss);
	virtual HRESULT LoadorSaveLinetype (char *Linetype, char *Filename, BOOL bSave);
	virtual HRESULT MakePViewportActive (db_viewport * pViewport);
	virtual HRESULT MSpaceOrPSpace (BOOL mspace);
	virtual HRESULT CreateImage (CString strFile, CString strDictName, sds_point ptIn, db_handitem **hip);
	virtual HRESULT AttachXRef (CString& FileName, CString& BlockName, sds_point ptOrigin, double xscale, double yscale, double zscale, double rotation, bool bOverlay, db_handitem **inserthip);
	virtual HRESULT XRefOperations (int Operation, CString FileName, db_handitem *blockhip);
	virtual HRESULT ExportEntities (CString FileName, int FileType, sds_name sset);
	virtual HRESULT ApplyHatch (sds_name ssetin, db_hatch *hip);
	virtual HRESULT FindHatchPattern (CString PatternName);
	virtual HRESULT AUTO_cmd_zoom(CIcadView *pView, sds_point zoomcenter, double zoomfactor, int ScaleType=-1);
	virtual HRESULT AUTO_cmd_zoom_extents(CIcadView *pView, int CheckLimits);
	virtual HRESULT AUTO_cmd_wblock_entmake(sds_name sset, sds_point insbase, char *fname, char *FileType, int* pnFileVersion);
	virtual HRESULT AUTO_cmd_box_create (sds_point pt1,sds_point pt2, sds_real high, db_handitem **hip);
	virtual HRESULT AUTO_cmd_cone_create (sds_point pt0,sds_real bdia,sds_real tdia,sds_point pthigh,int nseg, db_handitem **hip);
	virtual HRESULT AUTO_cmd_cyl_create (sds_point pt0,sds_real dia,sds_point pthigh,int nseg, db_handitem **hip);
	virtual HRESULT AUTO_cmd_sphere_create (sds_point pt0,sds_real rad,int longseg,int latseg, int domemode, db_handitem **hip);
	virtual HRESULT AUTO_cmd_torus_create (sds_point pt0,sds_real tordia,sds_real tubedia,int torseg,int tubseg, db_handitem **hip);
	virtual HRESULT AUTO_cmd_wedge_create (sds_point pt1,sds_point pt2, sds_real high,double ang, db_handitem **hip);
	virtual HRESULT AUTO_cmd_array_polar (sds_name ssetin, int nObjects, double fillangle, double anglebetween, sds_point basept, sds_point ptonaxis, bool rotate, sds_name ssetout);
	virtual HRESULT AUTO_cmd_array_rectangular (sds_name ssetin, int nRows, int nCols, int nLevs, double dRows, double dCols, double dLevs, sds_name ssetout);
	virtual HRESULT AUTO_cmd_explode_objects (sds_name ssetin, sds_name ssetout);
	virtual HRESULT AUTO_cmd_offset_func(sds_name ename, double offset, int bothmode, int plmode, sds_point dirIndicator, sds_name ssetout);
	virtual HRESULT AUTO_angtof (const char *szAngle, int nUnitType, double *pdAngle);
	virtual HRESULT AUTO_distof (const char *szDistance, int nUnitType, double *pdDistance);
	virtual HRESULT AUTO_rtos (double dNumber,int nUnitType,int nPrecision,char *szNumber, int iDimType);
	virtual HRESULT AUTO_angtos (double dAngle, int nUnitType, int nPrecision, char *szAngle) ;
	virtual HRESULT AUTO_cvunit (double dUnits, const char *szOldUnit, const char *szNewUnit, double *pdNewNum);
	virtual HRESULT AUTO_osnap (const sds_point ptAperCtr, const char *szSnapModes, sds_point ptPoint);
	virtual HRESULT AUTO_findfile (const char *szLookFor, char *szPathFound);
	virtual HRESULT AUTO_GetAngDistOrient (const sds_point ptStart, const char *szMsg, double *pdResult, getangdist_mode nMode);
	virtual HRESULT AUTO_getcorner(const sds_point ptStart, const char *szCornerMsg, sds_point ptOpposite, bool bApplySnapping);
	virtual HRESULT AUTO_getinput(char *szEntry);
	virtual HRESULT AUTO_getint(const char *szIntMsg, int *pnInteger) ;
	virtual HRESULT AUTO_getpoint(const sds_point ptReference, const char *szPointMsg, sds_point ptPoint) ;
	virtual HRESULT AUTO_getreal(const char *szRealMsg, double *pdReal) ;
	virtual HRESULT AUTO_getstring(int swSpaces, const char *szStringMsg, char *szString) ;
	virtual HRESULT AUTO_initget(int bsAllowed, const char *szKeyWordList) ;
	virtual HRESULT AUTO_getkword(const char *szKWordMsg, char *szKWord);
	virtual HRESULT AUTO_prompt(const char *szPromptMsg);
	virtual HRESULT AUTO_polar(const sds_point ptPolarCtr, double dAngle, double dDistance, sds_point ptPoint);
	virtual HRESULT AUTO_graphscr(void) ;
	virtual HRESULT AUTO_textscr(void) ;
	virtual HRESULT AUTO_wcmatch(const char *szCompareThis, const char *szToThis);
	virtual HRESULT AUTO_entsel(const char *szSelectMsg, sds_name nmEntity, sds_point ptSelected);
	virtual HRESULT AUTO_nentsel(const char *szNEntMsg, sds_name nmEntity, sds_point ptEntPoint, sds_point ptECStoWCS[4], struct sds_resbuf **pprbNestBlkList);
	virtual HRESULT AUTO_trans(const sds_point From, const struct sds_resbuf *CoordFrom, const struct sds_resbuf *CoordTo, int swVectOrDisp, sds_point To);
	virtual HRESULT AUTO_grdraw(const sds_point ptFrom, const sds_point ptTo, int nColor, int swHighlight);
	virtual HRESULT AUTO_ssadd(const sds_name nmEntToAdd, const sds_name nmSelSet, sds_name nmNewSet);
	virtual HRESULT AUTO_ssname(const sds_name nmSelSet, long lSetIndex, sds_name nmEntity);
	virtual HRESULT AUTO_ssfree(sds_name nmSetToFree);
	virtual HRESULT AUTO_sslength(sds_name nmSet, long * Length);
	virtual HRESULT AUTO_pmtssget(const char *szSelMsg,const char *szSelMethod, void *pFirstPoint, const void *pSecondPoint, const struct sds_resbuf *prbFilter,
							sds_name nmNewSet,const int OmitFromPrevSS, bool  bIncludeInvisible = true, bool bIncludeLockedLayers = false );
	virtual HRESULT AUTO_ssxss (sds_name ss0, sds_name ss1, sds_real fuzz, struct resbuf **ptlistp, int mode3d, long *nskipped0p, long *nskipped1p, IntersectOptions interOption);
	virtual HRESULT setUndo( int mode,void *data0,void *data1,void *data2 );
	// EBATECH(CNBR) 2002/4/24 Support SHAPE
	virtual HRESULT LoadShapeFile(const char *Filename);

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIcadDoc)
	public:
	virtual void OnCloseDocument(void);
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual HMENU GetDefaultMenu();
	virtual void DeleteContents();
	virtual BOOL DoFileSave();
	virtual void OnOpenEmbedding(LPSTORAGE lpStorage);
	virtual void OnSaveEmbedding(LPSTORAGE lpStorage);
	virtual void OnDeactivateUI(BOOL bUndoable);
	virtual void OnSetItemRects(LPCRECT lpPosRect, LPCRECT lpClipRect);
	protected:
	virtual BOOL OnNewDocument(void);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual COleServerItem* OnGetEmbeddedItem();
	virtual LPMONIKER GetMoniker(OLEGETMONIKER nAssign);
	//}}AFX_VIRTUAL


// Implementation
public:
	virtual ~CIcadDoc();
	virtual BOOL SaveModified();
	BOOL SaveModified( BOOL bAskForSave );
	BOOL IsModified();
	//Bugzilla No 78255 05-08-2002
	void StoreOleClientItem(CObList*& );
	void ReStoreOleClientItem(CObList*&);

// Generated message map functions
protected:
	//{{AFX_MSG(CIcadDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//	  DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
