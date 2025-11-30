/* D:\DEV\PRJ\ICAD\ICADCNTRITEM.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

// IcadCntrItem.cpp : implementation of the CIcadCntrItem class
//

#include "Icad.h"/*DNT*/
#include "IcadDoc.h"
#include "IcadCntrItem.h"/*DNT*/
#include "IcadToolBarMain.h"/*DNT*/
#include "IcadMenu.h"/*DNT*/
#include "IcadMenuMain.h"/*DNT*/
#include "IcadRectTracker.h"/*DNT*/
#include "IcadView.h"

#ifdef _DEBUG

	#if !defined(USE_SMARTHEAP)
	#define new DEBUG_NEW
	#endif

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIcadCntrItem implementation

IMPLEMENT_SERIAL(CIcadCntrItem, COleClientItem, 0)

CIcadCntrItem::CIcadCntrItem(CIcadDoc* pContainer,
	db_handitem* pHandItem) : COleClientItem(pContainer)
{
	m_pHandItem=pHandItem;
	m_bUpdateFailedOnSave=FALSE;
	m_pTracker = new CIcadRectTracker();
	m_HMUperDUx = 0.0;
	m_HMUperDUy = 0.0;
	m_bInUpdateHandItem=FALSE;
	m_nDefaultX=30;
	m_nDefaultY=30;
	m_pGrView = NULL;
}

CIcadCntrItem::~CIcadCntrItem()
{
	if(NULL!=m_pTracker)
	{
		delete m_pTracker;
		m_pTracker=NULL;
	}
}

void CIcadCntrItem::OnChange(OLE_NOTIFICATION nCode, DWORD dwParam)
{
	ASSERT_VALID(this);

	COleClientItem::OnChange(nCode, dwParam);

	switch(nCode)
	{
		case OLE_CHANGED:
			{
				CSize sizeExtent;
				OLE_OBJTYPE oleType = GetType();
				if(oleType==OT_LINK)
				{
					if(GetCachedExtent(&sizeExtent))
					{
						//*** Convert sizeExtent to pixels.
						CClientDC dcClient(NULL);
						dcClient.HIMETRICtoDP(&sizeExtent);
						CRect rect;
						GetCurRect(NULL,&rect);
						if(sizeExtent!=rect.Size())
						{
							if(rect.Width()==0)
							{
								rect.right=rect.left+sizeExtent.cx;
								rect.bottom=rect.top+sizeExtent.cy;
							}
							else
							{
								int cy = ((rect.Width()*sizeExtent.cy)/sizeExtent.cx);
								rect.bottom=rect.top+cy;
								SetServerSize(NULL,rect);
							}
							UpdateBounds(NULL,rect);
						}
						else
						{
							UpdateHandItem(NULL);
						}
						sds_redraw(NULL,IC_REDRAW_DONTCARE);
					}
				}
			}
			break;
		case OLE_CLOSED:
			{
				UpdateHandItem(NULL);
				UpdateFromServerExtent(NULL);
			}
			break;
		case OLE_CHANGED_STATE:
			if(dwParam==loadedState)
			{
				int i=0;
			}
			else if(dwParam==activeState)
			{
				int i=0;
			}
			else if(dwParam==activeUIState)
			{
				int i=0;
			}
			else if(dwParam==emptyState)
			{
				int i=0;
			}
			else if(dwParam==openState)
			{
				int i=0;
			}
			break;
	}
}

BOOL CIcadCntrItem::OnChangeItemPosition(const CRect& rectPos)
{
	ASSERT_VALID(this);
	if (!COleClientItem::OnChangeItemPosition(rectPos))
		return FALSE;
	UpdateBounds(NULL,rectPos);

	return TRUE;
}

void CIcadCntrItem::OnGetItemPosition(CRect& rPosition)
{
	ASSERT_VALID(this);
	if(NULL!=m_pHandItem)
	{
		CRect rect;
		GetCurRect(NULL,&rect);
		rPosition = rect;
	}
}

void CIcadCntrItem::OnActivate()
{
    // Allow only one inplace activate item per frame
    CIcadView* pView = (CIcadView*)GetActiveView();
    ASSERT_VALID(pView);
    COleClientItem* pItem = GetDocument()->GetInPlaceActiveItem(pView);
    if (pItem != NULL && pItem != this)
        pItem->Close();
    
	if(NULL!=pView)
	{
		pView->m_bSizeInPlace=TRUE;
		pView->m_bInPlaceDeactivated=FALSE;
	}

	//This section of code below used to be in CIcadCntrItem::OnInsertMenus()
	//The problem was that when inserting a Powerpoint Presentation, OnInsertMenus
	//would be called twice, messing up the menus, resulting in a crash.  By moving
	//this code up into OnActivate, we ensure it gets called only once.  What this
	//code appears to be doing is replacing the default MFC Client InPlace menu with
	//one that is created and managed by ICAD.
	CDocTemplate* pTemplate = GetDocument()->GetDocTemplate();
	if(pTemplate->m_hMenuInPlace!=NULL)
	{
		::DestroyMenu(pTemplate->m_hMenuInPlace);
	}
	CMainWindow* pMain = SDS_CMainWindow;
	if(NULL==pMain || NULL==pMain->m_pMenuMain)
	{
		return;
	}
	CIcadMenu* pMenu = pMain->m_pMenuMain->GetDefaultMenu();
	if(NULL!=pMenu)
	{
		pTemplate->m_hMenuInPlace = pMenu->MenuCreator(ICAD_MN_CLI);
	}
	//end of code from CIcadCntrItem::OnInsertMenus()

    COleClientItem::OnActivate();
}

void CIcadCntrItem::OnDeactivateUI(BOOL bUndoable)
{
	COleClientItem::OnDeactivateUI(bUndoable);

    CIcadView* pView = (CIcadView*)GetActiveView();
	if(NULL!=pView)
	{
		pView->m_bInPlaceDeactivated=TRUE;
	}
    // Hide the object if it is not an outside-in object
    DWORD dwMisc = 0;
    m_lpObject->GetMiscStatus(GetDrawAspect(), &dwMisc);
    if (dwMisc & OLEMISC_INSIDEOUT)
        DoVerb(OLEIVERB_HIDE, NULL);
}

void CIcadCntrItem::Serialize(CArchive& ar)
{
	ASSERT_VALID(this);

	// Call base class first to read in COleClientItem data.
	// Since this sets up the m_pDocument pointer returned from
	//  CIcadCntrItem::GetDocument, it is a good idea to call
	//  the base class Serialize first.
	COleClientItem::Serialize(ar);

	// now store/retrieve data specific to CIcadCntrItem
	if (ar.IsStoring())
	{
		// TODO: add storing code here
//		ar << m_rect;
	}
	else
	{
		// TODO: add loading code here
//		ar << m_rect;
	}
}

BOOL CIcadCntrItem::CanActivate()
{
	// Editing in-place while the server itself is being edited in-place
	//  does not work and is not supported.  So, disable in-place
	//  activation in this case.
	CIcadDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	ASSERT(pDoc->IsKindOf(RUNTIME_CLASS(COleServerDoc)));
	if (pDoc->IsInPlaceActive())
		return FALSE;

	// otherwise, rely on default behavior
	return COleClientItem::CanActivate();
}

void CIcadCntrItem::OnInsertMenus(CMenu* pMenuShared, LPOLEMENUGROUPWIDTHS lpMenuWidths)
{
	//code that was here moved up to OnActivate()
	COleClientItem::OnInsertMenus(pMenuShared,lpMenuWidths);
}

void CIcadCntrItem::OnRemoveMenus(CMenu* pMenuShared)
{
	COleClientItem::OnRemoveMenus(pMenuShared);
}

BOOL CIcadCntrItem::UpdateLink()
{
	ASSERT_VALID(this);
	ASSERT(m_lpObject != NULL);

	m_scLast = S_OK;
//	if (!IsLinkUpToDate())
	{
		m_scLast = m_lpObject->Update();
		ASSERT_VALID(m_pDocument);
		m_pDocument->SetModifiedFlag();
	}
	return m_scLast == S_OK;
}

BOOL CIcadCntrItem::UpdateFromServerExtent(CIcadView* pView)
{
	CSize sizeExtent;
	OLE_OBJTYPE oleType = GetType();
	if(oleType==OT_LINK)
	{
		//*** Check for Automatic or Manual update, and don't if set to Manual.
		if(OLEUPDATE_ALWAYS==GetLinkUpdateOptions())
		{
			//*** Return after this call because this function will send an ON_CHANGED
			//*** notification which will update the item.
			UpdateLink();
		}
		return TRUE;
	}
	if(!GetCachedExtent(&sizeExtent) || (NULL==pView && NULL==(pView=GetActiveView())))
	{
		return FALSE;
	}

	//*** Convert sizeExtent to points.
	sds_point ptLR,ptUL;
	if(NULL!=m_pHandItem && !icadRealEqual(m_HMUperDUx,0.0) && !icadRealEqual(m_HMUperDUy,0.0))
	{
		double dCx, dCy;
		dCx = sizeExtent.cx/m_HMUperDUx;
		dCy = sizeExtent.cy/m_HMUperDUy;

		((db_ole2frame*)m_pHandItem)->get_10(ptUL);
		((db_ole2frame*)m_pHandItem)->get_11(ptLR);

		if((ptLR[0]-ptUL[0])==dCx && (ptUL[1]-ptLR[1])==dCy)
		{
			return FALSE;
		}
		ptLR[0]=ptUL[0]+dCx;
		ptLR[1]=ptUL[1]-dCy;
	}
	else
	{
		//*** This is a new item, initialize it.
		CClientDC dcClient(NULL);
		CSize sizeExtPix(sizeExtent);
		dcClient.HIMETRICtoDP(&sizeExtPix);

		CRect rect(m_nDefaultX,m_nDefaultY,m_nDefaultX+sizeExtPix.cx,
			m_nDefaultY+sizeExtPix.cy);
		RectToPoints(pView,rect,ptLR,ptUL);

		m_HMUperDUx = ((double)sizeExtent.cx)/(ptLR[0]-ptUL[0]);
		m_HMUperDUy = ((double)sizeExtent.cy)/(ptUL[1]-ptLR[1]);
	}
	return UpdateBounds(pView,ptUL,ptLR);
}

BOOL CIcadCntrItem::UpdateBounds(CIcadView* pView, LPCRECT lpBounds)
{
	if(NULL==lpBounds || (NULL==pView && NULL==(pView=GetActiveView())))
		return FALSE;

	sds_point ptUL,ptLR;
	RectToPoints(pView,lpBounds,ptLR,ptUL);
	return UpdateBounds(pView,ptUL,ptLR);
}

void CIcadCntrItem::RectToPoints(CIcadView* pView, LPCRECT lpRect,
	sds_point ptLowerRight, sds_point ptUpperLeft)
{
	if(NULL==m_pGrView)
	{
		//*** Just in case.
		m_pGrView = pView->m_pCurDwgView;
	}
	gr_pix2rp(m_pGrView,lpRect->left,lpRect->top,&ptUpperLeft[0],&ptUpperLeft[1]);
	ptUpperLeft[2]=0.0;
	gr_pix2rp(m_pGrView,lpRect->right,lpRect->bottom,&ptLowerRight[0],&ptLowerRight[1]);
	ptLowerRight[2]=0.0;
}

BOOL CIcadCntrItem::UpdateBounds(CIcadView* pView, sds_point ptUL, sds_point ptLR)
{
	//*** Update the 10 and 11 in the handitem
	if(NULL==pView && NULL==(pView=GetActiveView()))
	{
		return FALSE;
	}
	if(NULL==m_pHandItem && !InitHandItem(pView))
	{
		return FALSE;
	}

	struct db_BinChange *chglst=NULL;
	db_handitem *savelp=NULL;
	SDS_CopyEntLink(&savelp,m_pHandItem);

	m_pHandItem->set_10(ptUL);
	m_pHandItem->set_11(ptLR);

	//*** Update the other 2 semi-unknown points
	sds_point ptLL,ptUR;
	ptLL[0]=ptUL[0];
	ptLL[1]=ptLR[1];
	ptLL[2]=0.0;
	((db_ole2frame*)m_pHandItem)->set_ll(ptLL);
	ptUR[0]=ptLR[0];
	ptUR[1]=ptUL[1];
	ptUR[2]=0.0;
	((db_ole2frame*)m_pHandItem)->set_ur(ptUR);
	//*** Update the extents of the object.
	((db_ole2frame*)m_pHandItem)->set_extent(ptLL,ptUR);
	//*** Code for undo/redo
	CIcadDoc* pDoc = GetDocument();
	if(NULL!=pDoc)
	{
		if(db_CompEntLinks(&chglst,savelp,m_pHandItem)==RTNORM && chglst)
		{
			SDS_SetUndo(IC_UNDO_COMMAND_BEGIN,(void *)ResourceString(IDC_ICADCNTRITEM_OLE_MOVE_0, "OLE Move" ),NULL,NULL,pDoc->m_dbDrawing); 			
			SDS_SetUndo(IC_UNDO_CHG_ENT_TAB_CLS,(void *)m_pHandItem,(void *)chglst,NULL,pDoc->m_dbDrawing);
			SDS_SetUndo(IC_UNDO_COMMAND_END,(void *)ResourceString(IDC_ICADCNTRITEM_OLE_MOVE_0, "OLE Move" ),NULL,NULL,pDoc->m_dbDrawing); 			
		}
	}
	if(NULL!=savelp)
	{
		delete savelp;
		savelp=NULL;
	}
	GetDocument()->SetModifiedFlag();
	return TRUE;
}

BOOL CIcadCntrItem::InitHandItem(CIcadView* pView)
{
	if(NULL!=m_pHandItem || (NULL==pView && NULL==(pView=GetActiveView())))
		return TRUE;

	struct sds_resbuf* newent=NULL;
	CIcadDoc* pDoc = GetDocument();
	if(NULL==pDoc)
		return FALSE;
	//*** Create a new handitem and add it to the drawing.
	CRect rect;
	GetCurRect(pView,&rect); //*** Get default rect position.
	sds_point ptUL,ptLR;
	RectToPoints(pView,rect,ptLR,ptUL);

	sds_name ename;
	//*** Figure out whether we are in paperspace or not (72)
	int nPsFlag=0;
	struct sds_resbuf rb;
	SDS_getvar(NULL,DB_QTILEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	int nTileMode=rb.resval.rint;
	SDS_getvar(NULL,DB_QCVPORT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if(!nTileMode && rb.resval.rint==1)
	{
		nPsFlag=1;
	}
	//*** Get the type (71)
	int nObjType;
	OLE_OBJTYPE oleType = GetType();
	if(oleType==OT_LINK)
	{
		nObjType=1;
	}
	if(oleType==OT_EMBEDDED)
	{
		nObjType=2;
	}
	else //*** Static
	{
		nObjType=3;
	}
	//*** Get the name of the application.
	CString strUserName;
	GetUserType(USERCLASSTYPE_FULL,strUserName);
	//*** Create the entity.
    if(NULL==(newent=sds_buildlist(RTDXF0,"OLE2FRAME"/*DNT*/,3,strUserName.GetBuffer(0),
		10,ptUL,11,ptLR,70,2,71,nObjType,72,nPsFlag,0)) ||
		RTNORM!=SDS_entmake(newent,pDoc->m_dbDrawing) ||
		RTNORM!=sds_entlast(ename))
	{
		IC_RELRB(newent);
		return FALSE;
	}
	m_pHandItem=(db_handitem*)ename[0];
	//*** Fill in the first 2 "unknown" bytes
	//*** This char value always changes, but 0 works.
    ((db_ole2frame *)m_pHandItem)->set_unk1(0);
	//*** char - Consistently 85 ??
    ((db_ole2frame *)m_pHandItem)->set_unk2(85);
	//*** Fill in the rest of the "unknown" bytes
	char* pOffset;
	char buff[26];
	memset(&buff,0,sizeof(buff));
	pOffset=buff;
	//*** Step over the first long, it is fine set to 0.
	pOffset+=sizeof(long);
	//*** long - 1 for modelspace
	//***		 0 for paperspace
	if(!nPsFlag)
	{
		*pOffset=1;
	}
	pOffset+=sizeof(long);
	//*** long - Consistently 256, probably number of colors in metafile.
	(*(long*)pOffset)=256;
	pOffset+=sizeof(long);
	//*** long - Some kind of counter, 1 works.
	(*(long*)pOffset)=1;
	pOffset+=sizeof(long);
	//*** long - Consistently 1.
	(*(long*)pOffset)=1;
	pOffset+=sizeof(long);
	//*** short - Somewhat consistently 0.
	pOffset+=sizeof(short);
	//*** long - from DVASPECT enumeration 
	//***		 1 DVASPECT_CONTENT
	//***		 2 DVASPECT_THUMBNAIL
	//***		 4 DVASPECT_ICON
	//***		 8 DVASPECT_DOCPRINT
	DVASPECT aspect = GetDrawAspect();
	(*(long*)pOffset)=(long)aspect;
    ((db_ole2frame *)m_pHandItem)->set_unk3(buff);

	IC_RELRB(newent);
	return TRUE;
}

BOOL CIcadCntrItem::UpdateHandItem(CIcadView* pView, BOOL bForceSave)
{
	if(m_bInUpdateHandItem)
	{
		//*** Prevent re-entry
		return FALSE;
	}
	if(NULL==pView && NULL==(pView=GetActiveView()))
	{
		return FALSE;
	}
	m_bInUpdateHandItem=TRUE;
	int nBadRet=0;
	HRESULT hResult;

//	struct db_BinChange *chglst=NULL;
//	db_handitem *savelp=NULL;
//	BOOL bDoUndo = TRUE;

	//*** Make sure the storage has been created.
	if(NULL==m_lpObject || NULL==m_lpLockBytes || NULL==m_lpStorage)
	{
		nBadRet=(-__LINE__);
		m_bInUpdateHandItem=FALSE;
		return FALSE;
	}
	//*** Make sure the entity is created already.
	if(!InitHandItem(pView))
	{
		nBadRet=(-__LINE__);
		goto out;
	}
	//*** bForceSave should be TRUE when we first create an item from existing data
	//*** (Create from existing document, or Paste from clipboard)
	if(bForceSave && S_OK!=m_xOleClientSite.SaveObject())
	{
		nBadRet=(-__LINE__);
		goto out;
	}
	//*** Get the hGlobal
	HGLOBAL hGlobal;
	hResult=GetHGlobalFromILockBytes(m_lpLockBytes,&hGlobal);
	if(S_OK!=hResult)
	{
		nBadRet=(-__LINE__);
		goto out;
	}
	else
	{
/*
		if(bDoUndo)
		{
			SDS_CopyEntLink(&savelp,m_pHandItem);
		}
*/
		//*** Get the size from ILockBytes
		STATSTG statstg;
		hResult=m_lpLockBytes->Stat(&statstg, STATFLAG_NONAME);
		if(hResult!=S_OK)
		{
			nBadRet=(-__LINE__);
			goto out;
		}
		DWORD dwBytes = statstg.cbSize.LowPart;
		//*** Finally, update the handitem from the hGlobal data.
		char* pGlobalData = (char*)GlobalLock(hGlobal);
		((db_ole2frame*)m_pHandItem)->set_310(pGlobalData,dwBytes);
		GlobalUnlock(hGlobal);
		GetDocument()->SetModifiedFlag();
		//*** Code for undo/redo
/*
		if(bDoUndo && NULL!=pDoc)
		{
			if(db_CompEntLinks(&chglst,savelp,m_pHandItem)==RTNORM && chglst)
			{
				SDS_SetUndo(IC_UNDO_COMMAND_BEGIN,(void *)ResourceString(IDC_ICADCNTRITEM_OLE_EDIT_2, "OLE Edit" ),NULL,NULL,pDoc->m_dbDrawing); 			
				SDS_SetUndo(IC_UNDO_CHG_ENT_TAB_CLS,(void *)m_pHandItem,(void *)chglst,NULL,pDoc->m_dbDrawing);
				SDS_SetUndo(IC_UNDO_COMMAND_END,(void *)ResourceString(IDC_ICADCNTRITEM_OLE_EDIT_2, "OLE Edit" ),NULL,NULL,pDoc->m_dbDrawing); 			
			}
		}
*/
	}

out:

/*
	if(NULL!=savelp)
	{
		delete savelp;
		savelp=NULL;
	}
*/
	m_bInUpdateHandItem=FALSE;
	if(nBadRet<0)
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CIcadCntrItem::ResetStorage(db_handitem* pNewHandItem)
{
/*
	//*** Free all the old OLE data and use the new.
	char* pOleData;
	pNewHandItem->get_310(&pOleData);
	int nBytes;
	pNewHandItem->get_90(&nBytes);
	if(NULL==m_lpLockBytes)
	{
		return FALSE;
	}
	ULARGE_INTEGER ulBytes;
	ulBytes.LowPart=nBytes;
	ulBytes.HighPart=0L;
	HRESULT hResult = m_lpLockBytes->SetSize(ulBytes);
	if(S_OK!=hResult)
	{
		return FALSE;
	}
	ulBytes.LowPart=0L;
	ULONG ulActualBytes;
	hResult=m_lpLockBytes->WriteAt(ulBytes,pOleData,nBytes,&ulActualBytes);
	if(S_OK!=hResult)
	{
		return FALSE;
	}

	//*** Release the old object
	if(NULL!=m_lpObject)
	{
		m_lpObject->Release();
		m_lpObject=NULL;
	}
	//*** Load the object with the new storage
	hResult=(::OleLoad(m_lpStorage,IID_IUnknown,GetClientSite(),
		(LPVOID FAR*)&m_lpObject));
	if(hResult!=S_OK)
	{
		return FALSE;
	}
	FinishCreate(hResult);
	SetSize(m_rect);
*/
	return TRUE;
}

BOOL CIcadCntrItem::ReadAcadOLEData(CIcadView* pView)
{
	if((NULL==pView && NULL==(pView=GetActiveView())) || NULL==m_pHandItem)
		return FALSE;

	int nBadRet=0;
	char* pBuff;
	DVASPECT aspect;
	CRect rect;
	//*** Get the OLE data from the handitem
	HRESULT hResult;
	db_ole2frame* pDbOleFrame = (db_ole2frame*)m_pHandItem;
	int nDataSize;
	pDbOleFrame->get_90(&nDataSize);
	if(nDataSize<=0) return FALSE;
	char* pOleData;
	pDbOleFrame->get_310(&pOleData);
	//*** Setup the bounding rectangle for the object
	CSize sizeExtent;
	sds_point ptUL,ptLR;
	pDbOleFrame->get_10(ptUL);
	pDbOleFrame->get_11(ptLR);
	//*** Create the HGLOBAL
	HGLOBAL hOleData = GlobalAlloc(GHND | GMEM_DDESHARE,nDataSize);
	LPVOID pData = GlobalLock(hOleData);
	memcpy(pData,pOleData,nDataSize);
	GlobalUnlock(hOleData);
	//*** Create the ILockBytes
	hResult=CreateILockBytesOnHGlobal(hOleData,TRUE,&m_lpLockBytes);
	if(S_OK!=hResult)
	{
		nBadRet=(-__LINE__);
		goto badout;
	}
	hResult=::StgIsStorageILockBytes(m_lpLockBytes);
	if(S_OK!=hResult)
	{
		nBadRet=(-__LINE__);
		goto badout;
	}
	//*** Create the IStorage
	hResult=StgOpenStorageOnILockBytes(m_lpLockBytes,NULL
		,STGM_SHARE_EXCLUSIVE | STGM_READWRITE
		,NULL,0,&m_lpStorage);
	if(hResult!=S_OK)
	{
		nBadRet=(-__LINE__);
		goto badout;
	}
	//*** Load the object
	hResult=(::OleLoad(m_lpStorage,IID_IUnknown,GetClientSite(),
		(LPVOID FAR*)&m_lpObject));
	if(hResult!=S_OK)
	{
		nBadRet=(-__LINE__);
		goto badout;
	}
	if (!FinishCreate(hResult))
		goto badout;
	//*** Set the draw aspect.
	pBuff=pDbOleFrame->ret_unk3();
	aspect=(DVASPECT)pBuff[22];

	//*** Instead of calling SetDrawAspect(), which marks the document as dirty.
	// prime iconic cache (in case object has never displayed iconic)
	if (aspect == DVASPECT_ICON)
	{
		SetIconicMetafile(NULL);    // allow object to provide own icon
		HGLOBAL hMetaPict = GetIconicMetafile();
		if (hMetaPict != NULL)
		{
			STGMEDIUM stgMedium;
			stgMedium.hGlobal = hMetaPict;
			stgMedium.tymed = TYMED_MFPICT;
			stgMedium.pUnkForRelease = NULL;
			ReleaseStgMedium(&stgMedium);
		}
	}

	// Note: the aspect you are setting may be uncached and therefore blank.
	//  To make sure it is cached use SetIconPicture or use IOleCache to
	//  set the cache yourself.
	m_nDrawAspect = aspect;
	//***

	GetCachedExtent(&sizeExtent);

	m_HMUperDUx = ((double)sizeExtent.cx)/(ptLR[0]-ptUL[0]);
	m_HMUperDUy = ((double)sizeExtent.cy)/(ptUL[1]-ptLR[1]);

	return TRUE;

badout:

	if(NULL!=m_lpLockBytes)
	{
		m_lpLockBytes->Release();
		m_lpLockBytes=NULL;
	}
	if(NULL!=m_lpStorage)
	{
		m_lpStorage->Release();
		m_lpStorage=NULL;
	}
	return FALSE;
}

BOOL CIcadCntrItem::GetCurRect(CIcadView* pView, CRect* pRect)
{
	sds_point ptUL,ptLR;

	if(NULL==pView && NULL==(pView=GetActiveView()))
	{
		return FALSE;
	}
	if(NULL==m_pHandItem)
	{
		struct sds_resbuf rb;
		SDS_getvar(NULL,DB_QTILEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		int nTileMode=rb.resval.rint;
		SDS_getvar(NULL,DB_QCVPORT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		if(nTileMode==0 && rb.resval.rint>1)
		{
			//*** If the item is being inserted into a modelspace viewport, figure
			//*** out the default position inside it.
			int x1,x2,y1,y2;
			SDS_GetRectForVP( pView, pView->m_pVportTabHip,&x1,&x2,&y1,&y2);
			m_nDefaultX+=x1;
			m_nDefaultY+=y1;
		}
		pRect->SetRect(m_nDefaultX,m_nDefaultY,m_nDefaultX,m_nDefaultY);
		return TRUE;
	}
	try
	{
		((db_ole2frame*)m_pHandItem)->get_10(ptUL);
		((db_ole2frame*)m_pHandItem)->get_11(ptLR);
		gr_rp2pix(m_pGrView,ptUL[0],ptUL[1],(int*)&pRect->left,(int*)&pRect->top);
		gr_rp2pix(m_pGrView,ptLR[0],ptLR[1],(int*)&pRect->right,(int*)&pRect->bottom);
	}
	catch(...)
	{
	}
	return TRUE;
}

void CIcadCntrItem::SetServerSize(CIcadView* pView, LPCRECT lpRect)
{
	if(NULL==pView && NULL==(pView=GetActiveView()))
	{
		return;
	}
	CSize sizeExtent;
	CRect rect(lpRect);
	sizeExtent=rect.Size();
	DVASPECT aspect = GetDrawAspect();
	DWORD dwStatus;
	BOOL bRun = FALSE;
	if(NULL==m_lpObject)
	{
		return;
	}
	if(S_OK==m_lpObject->GetMiscStatus(aspect,&dwStatus) &&
		(dwStatus & OLEMISC_RECOMPOSEONRESIZE))
	{
		OLE_OBJTYPE oleType = GetType();
		if(oleType!=OT_LINK)
		{
			//*** Don't do this for links because it sends OLE_CHANGED notifications.
			bRun=TRUE;
		}
	}
	BeginWaitCursor();

	sds_point ptUL,ptLR;

	RectToPoints(pView,rect,ptLR,ptUL);
	double cxDU = ptLR[0]-ptUL[0];
	double cyDU = ptUL[1]-ptLR[1];

	((db_ole2frame*)m_pHandItem)->get_10(ptUL);
	((db_ole2frame*)m_pHandItem)->get_11(ptLR);

	CSize sizeOrigExtent;
	GetCachedExtent(&sizeOrigExtent);

	m_HMUperDUx = ((double)sizeOrigExtent.cx)/(ptLR[0]-ptUL[0]);
	m_HMUperDUy = ((double)sizeOrigExtent.cy)/(ptUL[1]-ptLR[1]);

	sizeExtent.cx = (long)(m_HMUperDUx*cxDU);
	sizeExtent.cy = (long)(m_HMUperDUy*cyDU);

//	CClientDC dcClient(NULL);
//	dcClient.DPtoHIMETRIC(&sizeExtent);

	if(bRun)
	{
		::OleRun(m_lpObject);
	}
	m_lpObject->SetExtent(aspect,&sizeExtent);
	if(bRun)
	{
		m_lpObject->Update();
		m_lpObject->Close(OLECLOSE_SAVEIFDIRTY);
	}
	EndWaitCursor();
}

/* OleStdFree
** ---------- 
** free memory using the currently active IMalloc* allocator
*/
/*
STDAPI_(void) OleStdFree(LPVOID pmem)
{
	IMalloc* pIMalloc;  
	if(pmem==NULL)
		return;  
	if(CoGetMalloc(MEMCTX_TASK,&pIMalloc)!=NOERROR)
	{ 
		return;
	}   
	if(NULL!=pIMalloc)
	{
		ULONG refs;
		pIMalloc->Free(pmem); 
		refs = pIMalloc->Release();
	}
}  
*/
BOOL CIcadCntrItem::Draw(CIcadView* pView, CDC* pDC, struct gr_view *pGrView, DVASPECT nDrawAspect)
{
	BOOL bRet;
	if((NULL==pView && NULL==(pView=GetActiveView())) || NULL==m_pHandItem)
	{
		return FALSE;
	}
	m_pGrView=pGrView;
	if(NULL==m_pGrView)
	{
		//*** Just in case.
		m_pGrView = pView->m_pCurDwgView;
	}
	//*** Setup the bounding rectangle for the object
	CRect pBounds;
	GetCurRect(pView,&pBounds);
	//*** If the item is active, just tell the server to update it's size.
	if(IsInPlaceActive())
	{
		SetItemRects();
		//this return causes problems - deactivation via ESC will not draw the object
		//Deactivation via ESC only DeactivatesUI, does not fully deactivate the object
		//If we need this return to be put back in, call getItemState, and if UIActive, return
		if(activeUIState==GetItemState())
		{
			return TRUE;
		}
	}
/*  Didn't work, not sure why
	//*** Select a different palette for drawing the item
	LPVIEWOBJECT lpViewObject = NULL;
	// Get a pointer to IViewObject.
	if(NULL!=m_lpObject)
		m_lpObject->QueryInterface(IID_IViewObject,(LPVOID FAR*)&lpViewObject);
	// If the QI succeeds, get the LOGPALETTE for the object.
	LPLOGPALETTE lpColorSet = NULL;
    if(NULL!=lpViewObject)
	{
		HRESULT hr = lpViewObject->GetColorSet(DVASPECT_CONTENT,-1,NULL,NULL,NULL,
			&lpColorSet);
		if(hr!=S_OK)
		{
			int i=0;
		}
	}
	HPALETTE hPal = NULL;
	HPALETTE hOldPal = NULL;
	// If a LOGPALETTE was returned (not guaranteed), create the
	// palette and realize it. Note: A smarter application
	// would want to get the LOGPALETTE for each of its visible
	// objects, and try to create a palette that satisfies all of the
	// visible objects. Also, OleStdFree() is used to free the
	// returned LOGPALETTE.
	if(NULL!=lpColorSet)
	{
		hPal = CreatePalette((const LPLOGPALETTE)lpColorSet);
		hOldPal = SelectPalette(pDC->GetSafeHdc(),hPal,FALSE);
		RealizePalette(pDC->GetSafeHdc());
		OleStdFree(lpColorSet);
	}
*/
	//*** Draw the background
	CBrush brBkgnd(GetSysColor(COLOR_WINDOW));
	pDC->FillRect(pBounds,&brBkgnd);
	//*** Draw the item.
	bRet=COleClientItem::Draw(pDC,pBounds,nDrawAspect);
/*
	// If we created a palette, restore the old one and destroy
	// the object.
	if(NULL!=hPal)
	{
		SelectPalette(pDC->GetSafeHdc(),hOldPal,FALSE);
		DeleteObject(hPal);
	}
	// If a view pointer was successfully returned, it needs to be
	// released.
	if(lpViewObject)
		lpViewObject->Release();
*/
	//*** Draw the tracking rectangle
	SetupTracker(pView,pBounds);
	m_pTracker->Draw(pDC);
	//*** Make sure the image is updated.
	//pView->InvalidateRect(pBounds);

	return bRet;
}

void CIcadCntrItem::SetupTracker(CIcadView* pView, LPCRECT lpRect, CIcadRectTracker* pTracker)
{
	if(NULL==pView && NULL==(pView=GetActiveView()))
		return;

	if(NULL==pTracker)
	{
		pTracker=m_pTracker;
	}
	pTracker->m_rect = lpRect;

	struct sds_resbuf rb;
	SDS_getvar(NULL,DB_QGRIPSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	pTracker->m_nHandleSize=(rb.resval.rint*2);

	pTracker->m_nStyle=0;
	if(NULL!=pView && this==pView->m_pSelection)
	{
		pTracker->m_nStyle |= CRectTracker::resizeInside;
	}

	if(GetType()==OT_LINK)
	{
		pTracker->m_nStyle |= CRectTracker::dottedLine;
	}
	else
	{
		pTracker->m_nStyle |= CRectTracker::solidLine;
	}

	if(GetItemState()==COleClientItem::openState ||
		GetItemState()==COleClientItem::activeUIState)
	{
		pTracker->m_nStyle |= CRectTracker::hatchInside;
	}
}

void CIcadCntrItem::GetItemStorage()
{
//*** This function creates the new entity and the storage for the item.
	int nBadRet=0;
	HRESULT hResult;

	if(NULL==m_lpLockBytes)
	{
		//*** Create the ILockBytes
		if(S_OK!=CreateILockBytesOnHGlobal(NULL,TRUE,&m_lpLockBytes))
		{
			nBadRet=(-__LINE__);
			goto out;
		}
		//*** Create the IStorage
		hResult=StgCreateDocfileOnILockBytes(m_lpLockBytes
			,STGM_SHARE_EXCLUSIVE|STGM_CREATE|STGM_READWRITE
			,0,&m_lpStorage);
		if(S_OK!=hResult)
		{
			nBadRet=(-__LINE__);
			goto out;
		}
	}

out:

	if(nBadRet<0)
	{
		if(NULL!=m_lpLockBytes)
		{
			m_lpLockBytes->Release();
			m_lpLockBytes=NULL;
		}
		if(NULL!=m_lpStorage)
		{
			m_lpStorage->Release();
			m_lpStorage=NULL;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////

BEGIN_INTERFACE_MAP(CIcadCntrItem, COleClientItem)
	INTERFACE_PART(CIcadCntrItem, IID_IOleClientSite, IcadOleClientSite)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Implementation of IOleClientSite

STDMETHODIMP_(ULONG) CIcadCntrItem::XIcadOleClientSite::AddRef()
{
	METHOD_MANAGE_STATE(CIcadCntrItem, IcadOleClientSite)
	ASSERT_VALID(pThis);

	return pThis->m_xOleClientSite.AddRef();
}

STDMETHODIMP_(ULONG) CIcadCntrItem::XIcadOleClientSite::Release()
{
	METHOD_MANAGE_STATE(CIcadCntrItem, IcadOleClientSite)
	ASSERT_VALID(pThis);

	return pThis->m_xOleClientSite.Release();
}

STDMETHODIMP CIcadCntrItem::XIcadOleClientSite::QueryInterface(
	REFIID iid, LPVOID* ppvObj)
{
	METHOD_MANAGE_STATE(CIcadCntrItem, IcadOleClientSite)
	ASSERT_VALID(pThis);

	return pThis->m_xOleClientSite.QueryInterface(iid,ppvObj);
}

STDMETHODIMP CIcadCntrItem::XIcadOleClientSite::SaveObject()
{
	METHOD_MANAGE_STATE(CIcadCntrItem, IcadOleClientSite)
	ASSERT_VALID(pThis);
	SCODE sc =	pThis->m_xOleClientSite.SaveObject();
	if(sc==S_OK)
	{
		pThis->UpdateHandItem(NULL);
		pThis->UpdateFromServerExtent(NULL);
	}
	return sc;
}

STDMETHODIMP CIcadCntrItem::XIcadOleClientSite::GetMoniker(
	DWORD dwAssign, DWORD dwWhichMoniker, LPMONIKER* ppMoniker)
{
	METHOD_MANAGE_STATE(CIcadCntrItem, IcadOleClientSite)
	ASSERT_VALID(pThis);
	return pThis->m_xOleClientSite.GetMoniker(dwAssign,dwWhichMoniker,ppMoniker);
}

STDMETHODIMP CIcadCntrItem::XIcadOleClientSite::GetContainer(
	LPOLECONTAINER* ppContainer)
{
	METHOD_MANAGE_STATE(CIcadCntrItem, IcadOleClientSite)
	ASSERT_VALID(pThis);
	return pThis->m_xOleClientSite.GetContainer(ppContainer);
}

STDMETHODIMP CIcadCntrItem::XIcadOleClientSite::ShowObject()
{
	METHOD_MANAGE_STATE(CIcadCntrItem, IcadOleClientSite)
	ASSERT_VALID(pThis);
	return pThis->m_xOleClientSite.ShowObject();
}

STDMETHODIMP CIcadCntrItem::XIcadOleClientSite::OnShowWindow(BOOL fShow)
{
	METHOD_MANAGE_STATE(CIcadCntrItem, IcadOleClientSite)
	ASSERT_VALID(pThis);
	return pThis->m_xOleClientSite.OnShowWindow(fShow);
}

STDMETHODIMP CIcadCntrItem::XIcadOleClientSite::RequestNewObjectLayout()
{
	METHOD_MANAGE_STATE(CIcadCntrItem, IcadOleClientSite)
	ASSERT_VALID(pThis);
	return pThis->m_xOleClientSite.RequestNewObjectLayout();
}

/////////////////////////////////////////////////////////////////////////////
// CIcadCntrItem diagnostics

#ifdef _DEBUG
void CIcadCntrItem::AssertValid() const
{
	COleClientItem::AssertValid();
}

void CIcadCntrItem::Dump(CDumpContext& dc) const
{
	COleClientItem::Dump(dc);
}
#endif

/////////////////////////////////////////////////////////////////////////////


