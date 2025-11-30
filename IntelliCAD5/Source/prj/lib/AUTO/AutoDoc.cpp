/* INTELLICAD\PRJ\LIB\AUTO\AUTODOC.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * Document Automation Peer Class.
 * This is the implementation the Automation Interface - IIcadDocument.
 *
 */

#include "StdAfx.h"
#include "AutoMSpace.h"
#include "AutoPSpace.h"
#include "AutoLayer.h"
#include "AutoLayers.h"
#include "AutoLinetype.h"
#include "AutoLinetypes.h"
#include "AutoDimStyle.h"
#include "AutoDimStyles.h"
#include "AutoRegApps.h"
#include "AutoTextStyle.h"
#include "AutoTextStyles.h"
#include "AutoBlock.h"
#include "AutoBlocks.h"
#include "AutoUCS.h"
#include "AutoUCSs.h"
#include "AutoView.h"
#include "AutoViews.h"
#include "AutoViewport.h"
#include "AutoViewports.h"
#include "AutoPoints.h"
#include "AutoPoint.h"
#include "AutoSelSet.h"
#include "AutoSelSets.h"
#include "AutoUtility.h"
#include "AutoDocPrefs.h"
#include "IcadMain.h"
#include "viewport.h"
#include "IcadView.h"
#include "AutoHelpers.h"
#include "vporttabrec.h"
/////////////////////////////////////////////////////////////////////////////
// CAutoDoc

HRESULT CAutoDoc::Init(CIcadDoc *pDoc)
    {
    m_pDoc = pDoc;
    return NOERROR;
    }

HRESULT CAutoDoc::FinalConstruct()
    {
    m_pMSpace = NULL;
    m_pPSpace = NULL;
    m_pSelSets = NULL;
    return NOERROR;
    }

HRESULT CAutoDoc::FinalRelease()
    {
    return NOERROR;
    }

STDMETHODIMP CAutoDoc::InterfaceSupportsErrorInfo(REFIID riid)
    {
    static const IID* arr[] =
        {
        &IID_IIcadDocument
        };
    for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
        {
        if (IsEqualGUID(*arr[i],riid))  //  MDTJULY1001 - for mssdk january 2001 version
            return S_OK;
        }
    return S_FALSE;
    }

STDMETHODIMP CAutoDoc::get_Active (VARIANT_BOOL * Active)
    {
    if (Active == NULL)
        return E_POINTER;

    CIcadDoc* pDoc = ((CMainWindow*)GetApp()->m_pMainWnd)->m_pCurDoc;

    if (pDoc == m_pDoc)
        *Active = VARIANT_TRUE;
    else
        *Active = VARIANT_FALSE;

    return NOERROR;
    }

STDMETHODIMP CAutoDoc::get_ActiveDimStyle (IIcadDimensionStyle * * ppDimStyle)
    {
    if (ppDimStyle == NULL)
        return E_POINTER;

    db_handitem *pTableRecord = NULL;
    pTableRecord = m_pDoc->m_dbDrawing->ret_currenttabrec (DB_QDIMSTYLE);
    if (!pTableRecord)
        return E_FAIL;

    CComObject<CAutoDimensionStyle> *pDimensionStyle;
    CComObject<CAutoDimensionStyle>::CreateInstance(&pDimensionStyle);
    if (!pDimensionStyle)
        return E_FAIL;

    HRESULT hr;
    hr = pDimensionStyle->Init(m_pDoc, this, pTableRecord);
    if (FAILED(hr))
        return hr;

    return pDimensionStyle->QueryInterface (ppDimStyle);
    }

STDMETHODIMP CAutoDoc::put_ActiveDimStyle (IIcadDimensionStyle * ppDimStyle)
    {
    if (ppDimStyle == NULL)
        return E_POINTER;

    BSTR Name;
    ppDimStyle->get_Name (&Name);
    CString str (Name);

    struct resbuf rb;
    rb.restype = RTSTR;
    rb.resval.rstring = str.GetBuffer(0);

    char sysvar[IC_ACADBUF];
    strcpy (sysvar, "DIMSTYLE");

    return ((CIcadApp *)GetApp())->SDSSetVar (m_pDoc, sysvar, &rb);

    }

STDMETHODIMP CAutoDoc::get_ActiveLayer (IIcadLayer * * ppLayer)
    {
    if (ppLayer == NULL)
        return E_POINTER;

    db_handitem *pTableRecord = NULL;
    pTableRecord = m_pDoc->m_dbDrawing->ret_currenttabrec (DB_QCLAYER);
    if (!pTableRecord)
        return E_FAIL;

    CComObject<CAutoLayer> *pLayer;
    CComObject<CAutoLayer>::CreateInstance(&pLayer);
    if (!pLayer)
        return E_FAIL;

    HRESULT hr;
    hr = pLayer->Init(m_pDoc, this, pTableRecord);
    if (FAILED(hr))
        return hr;

    return pLayer->QueryInterface (ppLayer);
    }

STDMETHODIMP CAutoDoc::put_ActiveLayer (IIcadLayer * ppLayer)
    {
    if (ppLayer == NULL)
        return E_POINTER;

    BSTR Name;
    ppLayer->get_Name (&Name);
    CString str (Name);

    struct resbuf rb;
    rb.restype = RTSTR;
    rb.resval.rstring = str.GetBuffer(0);

    char sysvar[IC_ACADBUF];
    strcpy (sysvar, "CLAYER");

    return ((CIcadApp *)GetApp())->SDSSetVar (m_pDoc, sysvar, &rb);

    }

STDMETHODIMP CAutoDoc::get_ActiveLinetype (IIcadLinetype * * ppLinetype)
    {
    if (ppLinetype == NULL)
        return E_POINTER;

    db_handitem *pTableRecord = NULL;
    pTableRecord = m_pDoc->m_dbDrawing->ret_currenttabrec (DB_QCELTYPE);
    if (!pTableRecord)
        return E_FAIL;

    CComObject<CAutoLinetype> *pLinetype;
    CComObject<CAutoLinetype>::CreateInstance(&pLinetype);
    if (!pLinetype)
        return E_FAIL;

    HRESULT hr;
    hr = pLinetype->Init(m_pDoc, this, pTableRecord);
    if (FAILED(hr))
        return hr;

    return pLinetype->QueryInterface (ppLinetype);
    }

STDMETHODIMP CAutoDoc::put_ActiveLinetype (IIcadLinetype * ppLinetype)
    {
    if (ppLinetype == NULL)
        return E_POINTER;

    BSTR Name;
    ppLinetype->get_Name (&Name);
    CString str (Name);

    struct resbuf rb;
    rb.restype = RTSTR;
    rb.resval.rstring = str.GetBuffer(0);

    char sysvar[IC_ACADBUF];
    strcpy (sysvar, "CELTYPE");

    return ((CIcadApp *)GetApp())->SDSSetVar (m_pDoc, sysvar, &rb);

    }

STDMETHODIMP CAutoDoc::get_ActivePViewport (IIcadPViewport * * ppVPort)
    {
    if (ppVPort == NULL)
        return E_POINTER;

    db_viewport *pViewport = m_pDoc->m_dbDrawing->GetCurrentViewport ();

    CComPtr<IIcadEntity> pEntity;
    HRESULT hr = CreateEntityWrapper (m_pDoc, NULL, pViewport, &pEntity);
    if (FAILED(hr))
        return E_FAIL;

    hr = pEntity->QueryInterface (ppVPort);
    if (FAILED(hr))
        return E_FAIL;

    return NOERROR;
    }

STDMETHODIMP CAutoDoc::put_ActivePViewport (IIcadPViewport * ppVPort)
    {
    if (ppVPort == NULL)
        return E_INVALIDARG;

    int id;
    HRESULT hr = ppVPort->get__ViewportID (&id);
    if (FAILED(hr))
        return E_FAIL;

    db_viewport *pViewport = m_pDoc->m_dbDrawing->GetViewport (id);

    return m_pDoc->MakePViewportActive (pViewport);
    }


STDMETHODIMP CAutoDoc::get_ActiveSelectionSet (IIcadSelectionSet * * ppSelSet)
    {
    if (ppSelSet == NULL)
        return E_POINTER;

    HRESULT hr;

    CComObject<CAutoSelectionSet>* pSelectionSet;
    CComObject<CAutoSelectionSet>::CreateInstance(&pSelectionSet);
    if (!pSelectionSet)
        return E_FAIL;

    CString Name("*ACTIVE");
    pSelectionSet->Init(m_pDoc, Name);

    hr = pSelectionSet->InitializeFromSDSsset (m_pDoc->m_pGripSelection);
    //do not test hr, because the active sel set may be empty

    return pSelectionSet->QueryInterface (ppSelSet);
    }

STDMETHODIMP CAutoDoc::get_ActiveSpace (PMSpace * pSpace)
    {
    if (pSpace == NULL)
        return E_POINTER;

    struct resbuf rb;
    db_getvar (NULL, DB_QTILEMODE, &rb, m_pDoc->m_dbDrawing, NULL, NULL);

    if (rb.resval.rint == 1)
        *pSpace = vicModelSpace;
    else if (rb.resval.rint == 0)
        *pSpace = vicPaperSpace;
    else
        return E_FAIL;

    return NOERROR;
    }

STDMETHODIMP CAutoDoc::put_ActiveSpace (PMSpace pSpace)
    {
    struct resbuf rb;
    rb.restype=RTSHORT;

    if (pSpace == vicModelSpace)
        rb.resval.rint = 1;
    else if (pSpace == vicPaperSpace)
        rb.resval.rint = 0;
    else
        return E_INVALIDARG;

    char sysvar[IC_ACADBUF];
    strcpy (sysvar, "TILEMODE");

    //db_setvar (NULL, DB_QTILEMODE, &rb, m_pDoc->m_dbDrawing, NULL, NULL, 0);
    return ((CIcadApp *)GetApp())->SDSSetVar (m_pDoc, sysvar, &rb);
    }

STDMETHODIMP CAutoDoc::get_ActiveTextStyle (IIcadTextStyle * * ppTextStyle)
    {
    if (ppTextStyle == NULL)
        return E_POINTER;

    db_handitem *pTableRecord = NULL;
    pTableRecord = m_pDoc->m_dbDrawing->ret_currenttabrec (DB_QTEXTSTYLE);
    if (!pTableRecord)
        return E_FAIL;

    CComObject<CAutoTextStyle> *pTextStyle;
    CComObject<CAutoTextStyle>::CreateInstance(&pTextStyle);
    if (!pTextStyle)
        return E_FAIL;

    HRESULT hr;
    hr = pTextStyle->Init(m_pDoc, this, pTableRecord);
    if (FAILED(hr))
        return hr;

    return pTextStyle->QueryInterface (ppTextStyle);
    }

STDMETHODIMP CAutoDoc::put_ActiveTextStyle (IIcadTextStyle * ppTextStyle)
    {
    if (ppTextStyle == NULL)
        return E_POINTER;

    BSTR Name;
    ppTextStyle->get_Name (&Name);
    CString str (Name);

    struct resbuf rb;
    rb.restype = RTSTR;
    rb.resval.rstring = str.GetBuffer(0);

    char sysvar[IC_ACADBUF];
    strcpy (sysvar, "TEXTSTYLE");

    return ((CIcadApp *)GetApp())->SDSSetVar (m_pDoc, sysvar, &rb);
    }

STDMETHODIMP CAutoDoc::get_ActiveUCS (IIcadUserCoordSystem * * ppUCS)
    {
    if (ppUCS == NULL)
        return E_POINTER;

    BOOL bDefault = FALSE;
    db_handitem *pTableRecord = NULL;
    pTableRecord = m_pDoc->m_dbDrawing->ret_currenttabrec (DB_QUCSNAME);

    //if there is no active ucs ,create a default one and call it DEFAULT
    if (!pTableRecord)
        bDefault = TRUE;

    CComObject<CAutoUCS> *pUCS;
    CComObject<CAutoUCS>::CreateInstance(&pUCS);
    if (!pUCS)
        return E_FAIL;

    HRESULT hr;
    hr = pUCS->Init(m_pDoc, this, pTableRecord);
    if (FAILED(hr))
        return hr;

    if (bDefault)
        pUCS->put_Name (CComBSTR("DEFAULT"));

    return pUCS->QueryInterface (ppUCS);
    }

STDMETHODIMP CAutoDoc::put_ActiveUCS (IIcadUserCoordSystem * ppUCS)
    {
    if (ppUCS == NULL)
        return E_POINTER;

    BSTR Name;
    ppUCS->get_Name (&Name);
    CString str (Name);

    struct resbuf rb;
    rb.restype = RTSTR;
    rb.resval.rstring = str.GetBuffer(0);

    char sysvar[IC_ACADBUF];
    strcpy (sysvar, "UCSNAME");

    return ((CIcadApp *)GetApp())->SDSSetVar (m_pDoc, sysvar, &rb);

    }

STDMETHODIMP CAutoDoc::get_ActiveViewport (IIcadViewport * * ppViewport)
    {
    if (ppViewport == NULL)
        return E_POINTER;

    CIcadView *pView = (CIcadView *)((CMDIFrameWnd *)GetApp()->m_pMainWnd)->GetActiveFrame()->GetActiveView();

    db_handitem *pVportHip = pView->m_pVportTabHip;

    //if the m_pVportTabHip is not yet set, activate the window, so that this variable will get set
    if (!pView->m_pVportTabHip)
        {
        CFrameWnd* pFrame;
        pFrame = pView->GetParentFrame();
        if (pFrame)
            pFrame->SetFocus();
        }

    if( pVportHip == NULL)
        return E_FAIL;

    if (pVportHip->ret_type() == DB_VIEWPORT)
        return E_FAIL;

    ASSERT (pVportHip->ret_type() == DB_VPORTTABREC);

    CComObject<CAutoViewport> *pViewport;
    CComObject<CAutoViewport>::CreateInstance(&pViewport);
    if (!pViewport)
        return E_FAIL;

    HRESULT hr;
    hr = pViewport->Init(m_pDoc, NULL, pVportHip);
    if (FAILED(hr))
        return hr;

    return pViewport->QueryInterface(ppViewport);
    }

void GetActiveViewportName (CIcadDoc *pDoc, CString *ActiveVpName);

STDMETHODIMP CAutoDoc::put_ActiveViewport (IIcadViewport * ppViewport)
    {
    if (ppViewport == NULL)
        return E_INVALIDARG;

    BSTR VportHandle;
    HRESULT hr = ppViewport->get_Handle (&VportHandle);
    if (FAILED(hr))
        return E_FAIL;

    BSTR VportName;
    hr = ppViewport->get_Name (&VportName);
    if (FAILED(hr))
        return E_FAIL;
    CString VpName(VportName);

    if (VpName != "*ACTIVE")    //then we probably need to restore a view
        {
        CString ActiveVpName;
        GetActiveViewportName (m_pDoc , &ActiveVpName);
        if (VpName != ActiveVpName)
            m_pDoc->RestoreViewportConfiguration (VpName.GetBuffer(0));
        }

    CString str(VportHandle);
    db_objhandle hand1(str);
    db_objhandle hand2;

    CIcadView* pView = NULL;
    POSITION pos = m_pDoc->GetFirstViewPosition();
    while (pos != NULL)
        {
        pView = (CIcadView*) m_pDoc->GetNextView(pos);
        hand2 = pView->m_pVportTabHip->RetHandle();
        if (hand1 == hand2)
            break;

        //also check this viewports original hip
        db_handitem *orighip = NULL;
        ((db_vporttabrec *)(pView->m_pVportTabHip))->get_original_viewport (&orighip);
        if (orighip)
            {
            hand2 = orighip->RetHandle();
            if (hand1 == hand2)
                break;
            }
        //if we didn't find the view, set it to NULL
        pView = NULL;
        }

    if (pView == NULL)
        return E_FAIL;

    CFrameWnd* pFrame;
    pFrame = pView->GetParentFrame();
    if (pFrame)
        {
        pFrame->ActivateFrame();
        pFrame->SetActiveView (pView, TRUE);
        pFrame->SetFocus();
        }

    m_pDoc->RedrawCurrentView();

    return NOERROR;
    }

STDMETHODIMP CAutoDoc::get_Blocks (IIcadBlocks * * ppBlocks)
    {
    if (ppBlocks == NULL)
        return E_POINTER;

    CComObject<CAutoBlocks>* pBlocks;
    CComObject<CAutoBlocks>::CreateInstance(&pBlocks);
    if (!pBlocks)
        return E_FAIL;
    pBlocks->Init(m_pDoc);

    return pBlocks->QueryInterface(ppBlocks);
    }

STDMETHODIMP CAutoDoc::get_DimensionStyles (IIcadDimensionStyles * * ppDimensionStyles)
    {
    if (ppDimensionStyles == NULL)
        return E_POINTER;

    CComObject<CAutoDimensionStyles>* pDimensionStyles;
    CComObject<CAutoDimensionStyles>::CreateInstance(&pDimensionStyles);
    if (!pDimensionStyles)
        return E_FAIL;
    pDimensionStyles->Init(m_pDoc);

    return pDimensionStyles->QueryInterface(ppDimensionStyles);
    }

STDMETHODIMP CAutoDoc::get_Layers (IIcadLayers * * ppLayers)
    {
    if (ppLayers == NULL)
        return E_POINTER;

    CComObject<CAutoLayers>* pLayers;
    CComObject<CAutoLayers>::CreateInstance(&pLayers);
    if (!pLayers)
        return E_FAIL;
    pLayers->Init(m_pDoc);

    return pLayers->QueryInterface(ppLayers);
    }

STDMETHODIMP CAutoDoc::get_Linetypes (IIcadLinetypes * * ppLinetypes)
    {
    if (ppLinetypes == NULL)
        return E_POINTER;

    CComObject<CAutoLinetypes>* pLinetypes;
    CComObject<CAutoLinetypes>::CreateInstance(&pLinetypes);
    if (!pLinetypes)
        return E_FAIL;
    pLinetypes->Init(m_pDoc);

    return pLinetypes->QueryInterface(ppLinetypes);
    }

STDMETHODIMP CAutoDoc::get_RegisteredApplications (IIcadRegisteredApplications * * ppAppIDs)
    {
    if (ppAppIDs == NULL)
        return E_POINTER;

    CComObject<CAutoRegisteredApplications>* pRegisteredApplications;
    CComObject<CAutoRegisteredApplications>::CreateInstance(&pRegisteredApplications);
    if (!pRegisteredApplications)
        return E_FAIL;
    pRegisteredApplications->Init(m_pDoc);

    return pRegisteredApplications->QueryInterface(ppAppIDs);
    }

STDMETHODIMP CAutoDoc::get_SelectionSets (IIcadSelectionSets * * ppSelSets)
    {
    if (ppSelSets == NULL)
        return E_POINTER;

    if (!m_pSelSets)
        {
        CComObject<CAutoSelectionSets> *pSelectionSets;
        CComObject<CAutoSelectionSets>::CreateInstance(&pSelectionSets);
        if (!pSelectionSets)
            return E_FAIL;

        pSelectionSets->Init(m_pDoc);
        pSelectionSets->QueryInterface (&m_pSelSets);
        }

    return m_pSelSets->QueryInterface(ppSelSets);
    }

STDMETHODIMP CAutoDoc::get_TextStyles (IIcadTextStyles * * ppTextStyles)
    {
    if (ppTextStyles == NULL)
        return E_POINTER;

    CComObject<CAutoTextStyles>* pTextStyles;
    CComObject<CAutoTextStyles>::CreateInstance(&pTextStyles);
    if (!pTextStyles)
        return E_FAIL;
    pTextStyles->Init(m_pDoc);

    return pTextStyles->QueryInterface(ppTextStyles);
    }

STDMETHODIMP CAutoDoc::get_UserCoordinateSystems (IIcadUserCoordSystems * * ppUserCoordSystems)
    {
    if (ppUserCoordSystems == NULL)
        return E_POINTER;

    CComObject<CAutoUCSs>* pUCSs;
    CComObject<CAutoUCSs>::CreateInstance(&pUCSs);
    if (!pUCSs)
        return E_FAIL;
    pUCSs->Init(m_pDoc);

    return pUCSs->QueryInterface(ppUserCoordSystems);
    }

STDMETHODIMP CAutoDoc::get_Viewports (IIcadViewports * * ppViewports)
    {
    if (ppViewports == NULL)
        return E_POINTER;

    CComObject<CAutoViewports>* pViewports;
    CComObject<CAutoViewports>::CreateInstance(&pViewports);
    if (!pViewports)
        return E_FAIL;
    pViewports->Init(m_pDoc);

    return pViewports->QueryInterface(ppViewports);
    }

STDMETHODIMP CAutoDoc::get_Views (IIcadViews * * ppViews)
    {
    if (ppViews == NULL)
        return E_POINTER;

    CComObject<CAutoViews>* pViews;
    CComObject<CAutoViews>::CreateInstance(&pViews);
    if (!pViews)
        return E_FAIL;
    pViews->Init(m_pDoc);

    return pViews->QueryInterface(ppViews);
    }

STDMETHODIMP CAutoDoc::get_ModelSpace (IIcadModelSpace * * ppEntities)
    {
    if (ppEntities == NULL)
        return E_POINTER;

    if (!m_pMSpace)
        {
        CComObject<CAutoMSpace> *pMSpace;
        CComObject<CAutoMSpace>::CreateInstance(&pMSpace);
        if (!pMSpace)
            return E_FAIL;

        pMSpace->Init(m_pDoc);
        pMSpace->QueryInterface (&m_pMSpace);
        }

    return m_pMSpace->QueryInterface(ppEntities);
    }

STDMETHODIMP CAutoDoc::get_PaperSpace (IIcadPaperSpace * * ppEntities)
    {
    if (ppEntities == NULL)
        return E_POINTER;

    if (!m_pPSpace)
        {
        CComObject<CAutoPSpace> *pPSpace;
        CComObject<CAutoPSpace>::CreateInstance(&pPSpace);
        if (!pPSpace)
            return E_FAIL;

        pPSpace->Init(m_pDoc);
        pPSpace->QueryInterface (&m_pPSpace);
        }

        return m_pPSpace->QueryInterface(ppEntities);
    }

STDMETHODIMP CAutoDoc::get_Preferences (IIcadDocumentPreferences * * ppPreferences)
    {
    if (ppPreferences == NULL)
        return E_POINTER;

    CComObject<CAutoDocPreferences> *pPrefs;
    CComObject<CAutoDocPreferences>::CreateInstance(&pPrefs);
    if (!pPrefs)
        return E_FAIL;

    pPrefs->Init(m_pDoc);

    return pPrefs->QueryInterface (ppPreferences);
    }

STDMETHODIMP CAutoDoc::get_Application (IIcadApplication * * ppApp)
    {
    if (ppApp == NULL)
        return E_POINTER;

    return GetApp()->m_pAutoApp->QueryInterface (ppApp);
    }

STDMETHODIMP CAutoDoc::get_Parent (IIcadDocuments * * ppParent)
    {
    if (ppParent == NULL)
        return E_POINTER;

    CComPtr<IIcadApplication> pApp;
    get_Application (&pApp);

    return pApp->get_Documents (ppParent);
    }

STDMETHODIMP CAutoDoc::get_FullName (BSTR * pstrFullAppName)
    {
    if (pstrFullAppName == NULL)
        return E_POINTER;

    CString strResult = m_pDoc->GetPathName();
    *pstrFullAppName = strResult.AllocSysString();

    return NOERROR;
    }

STDMETHODIMP CAutoDoc::get_Name (BSTR * pAppName)
    {
    if (pAppName == NULL)
        return E_POINTER;

    CString strResult = m_pDoc->GetTitle();
    *pAppName = strResult.AllocSysString();

    return NOERROR;
    }

STDMETHODIMP CAutoDoc::get_Path (BSTR * pPath)
    {
    if (pPath == NULL)
        return E_POINTER;

    CString strResult = m_pDoc->GetPathName();
    int idxChar = strResult.ReverseFind('\\');
    if(idxChar!=(-1))
        {
        strResult = strResult.Left (idxChar);
        }
    else
        {
        strResult.Empty();
        }

    *pPath = strResult.AllocSysString();
    return NOERROR;
    }

STDMETHODIMP CAutoDoc::get_ReadOnly (VARIANT_BOOL * pbReadOnly)
    {
    if (pbReadOnly == NULL)
        return E_POINTER;

    *pbReadOnly = ((CIcadDoc*)m_pDoc)->m_bIsReadOnly  ? VARIANT_TRUE : VARIANT_FALSE;

    return NOERROR;
    }

STDMETHODIMP CAutoDoc::get_Saved (VARIANT_BOOL * pbSaved)
    {
    if (pbSaved == NULL)
        return E_POINTER;

    *pbSaved = m_pDoc->IsModified() ? VARIANT_TRUE : VARIANT_FALSE;

    return NOERROR;
    }

STDMETHODIMP CAutoDoc::get_MSpace (VARIANT_BOOL * pSpace)
    {
    if (pSpace == NULL)
        return E_POINTER;

    *pSpace = ((CIcadDoc*)m_pDoc)->m_dbDrawing->inpspace() ? VARIANT_FALSE : VARIANT_TRUE;

    return NOERROR;
    }

STDMETHODIMP CAutoDoc::put_MSpace (VARIANT_BOOL pSpace)
    {
    if (pSpace == VARIANT_TRUE)
        return m_pDoc->MSpaceOrPSpace (TRUE);
    else
        return m_pDoc->MSpaceOrPSpace (FALSE);
    }

STDMETHODIMP CAutoDoc::get_ObjectSnapMode (VARIANT_BOOL * SnapMode)
    {
    if (SnapMode == NULL)
        return E_POINTER;

    struct resbuf rb;
    db_getvar (NULL, DB_QSNAPMODE, &rb, m_pDoc->m_dbDrawing, NULL, NULL);
    *SnapMode = ( rb.resval.rint > 0 ) ? VARIANT_TRUE : VARIANT_FALSE;

    return NOERROR;
    }

STDMETHODIMP CAutoDoc::put_ObjectSnapMode (VARIANT_BOOL SnapMode)
    {
    struct resbuf rb;
    db_getvar (NULL, DB_QSNAPMODE, &rb, m_pDoc->m_dbDrawing, NULL, NULL);

    //Toggle the value - I dont think this works very well
    rb.resval.rint = rb.resval.rint * -1;

    char sysvar[IC_ACADBUF];
    strcpy (sysvar, "OSMODE");

    return ((CIcadApp *)GetApp())->SDSSetVar (m_pDoc, sysvar, &rb);
    //db_setvar (NULL, DB_QSNAPMODE, &rb, m_pDoc->m_dbDrawing, NULL, NULL, 0);
    }

STDMETHODIMP CAutoDoc::get_ElevationModelSpace (double * Elevation)
    {
    if (Elevation == NULL)
        return E_POINTER;

    struct resbuf rb;
    db_getvar (NULL, DB_QELEVATION, &rb, m_pDoc->m_dbDrawing, NULL, NULL);
    *Elevation = rb.resval.rreal;

    return NOERROR;
    }

STDMETHODIMP CAutoDoc::put_ElevationModelSpace (double Elevation)
    {
    struct resbuf rb;
    rb.resval.rreal = Elevation;
    db_setvar (NULL, DB_QELEVATION, &rb, m_pDoc->m_dbDrawing, NULL, NULL, 0);

    return NOERROR;
    }

STDMETHODIMP CAutoDoc::get_ElevationPaperSpace (double * Elevation)
    {
    if (Elevation == NULL)
        return E_POINTER;

    struct resbuf rb;
    db_getvar (NULL, DB_QP_ELEVATION, &rb, m_pDoc->m_dbDrawing, NULL, NULL);
    *Elevation = rb.resval.rreal;

    return NOERROR;
    }

STDMETHODIMP CAutoDoc::put_ElevationPaperSpace (double Elevation)
    {
    struct resbuf rb;
    rb.resval.rreal = Elevation;
    db_setvar (NULL, DB_QP_ELEVATION, &rb, m_pDoc->m_dbDrawing, NULL, NULL, 0);

    return NOERROR;
    }

STDMETHODIMP CAutoDoc::get_Limits (IIcadPoints * * ppLimits)
    {
    if (ppLimits == NULL)
        return E_POINTER;

    struct resbuf rb1;
    db_getvar (NULL, DB_QLIMMIN, &rb1, m_pDoc->m_dbDrawing, NULL, NULL);

    struct resbuf rb2;
    db_getvar (NULL, DB_QLIMMAX, &rb2, m_pDoc->m_dbDrawing, NULL, NULL);

    CComObject<CAutoPoints> *pLimits;
    CComObject<CAutoPoints>::CreateInstance(&pLimits);
    if (!pLimits)
        return E_OUTOFMEMORY;

    CComPtr<IIcadPoint> pIIcadPoint = NULL;
    pLimits->Add (rb1.resval.rpoint[0], rb1.resval.rpoint[1], rb1.resval.rpoint[2], 0, &pIIcadPoint);
    if (pIIcadPoint == NULL)
        return E_OUTOFMEMORY;
    pIIcadPoint = NULL;

    pLimits->Add (rb2.resval.rpoint[0], rb2.resval.rpoint[1], rb2.resval.rpoint[2], 0, &pIIcadPoint);
    if (pIIcadPoint == NULL)
        return E_OUTOFMEMORY;
    pIIcadPoint = NULL;

	//IntelliKorea 2001/6/25
	//return pLimits->QueryInterface (&ppLimits);
	return pLimits->QueryInterface (IID_IIcadPoints, (void **)ppLimits);
	//IntelliKorea 2001/6/25
    }

STDMETHODIMP CAutoDoc::put_Limits (IIcadPoints * ppLimits)
    {
    struct resbuf rb1;
    struct resbuf rb2;
    CComPtr<IIcadPoint> pIIcadPoint;

    rb1.resval.rpoint[0] = 0.0;
    rb1.resval.rpoint[1] = 0.0;
    rb1.resval.rpoint[2] = 0.0;

    rb2.resval.rpoint[0] = 100.0;
    rb2.resval.rpoint[1] = 100.0;
    rb2.resval.rpoint[2] = 100.0;

    HRESULT hr = ppLimits->Item(1, &pIIcadPoint);
    if (SUCCEEDED(hr) && pIIcadPoint)
        {
        pIIcadPoint->get_x (&rb1.resval.rpoint[0]);
        pIIcadPoint->get_y (&rb1.resval.rpoint[1]);
        pIIcadPoint->get_z (&rb1.resval.rpoint[2]);
        pIIcadPoint = NULL;
        }

    hr = ppLimits->Item(2, &pIIcadPoint);
    if (SUCCEEDED(hr) && pIIcadPoint)
        {
        pIIcadPoint->get_x (&rb2.resval.rpoint[0]);
        pIIcadPoint->get_y (&rb2.resval.rpoint[1]);
        pIIcadPoint->get_z (&rb2.resval.rpoint[2]);
        pIIcadPoint = NULL;
        }

	//[----IntelliKorea 2001/6/25
    rb1.restype = RT3DPOINT;
	rb2.restype = RT3DPOINT;
	//     IntelliKorea 2001/6/25 -----]
    db_setvar (NULL, DB_QLIMMIN, &rb1, m_pDoc->m_dbDrawing, NULL, NULL, 0);
    db_setvar (NULL, DB_QLIMMAX, &rb2, m_pDoc->m_dbDrawing, NULL, NULL, 0);

    return NOERROR;
    }

STDMETHODIMP CAutoDoc::get_Utility (IIcadUtility * * ppUtility)
    {
    if (ppUtility == NULL)
        return E_POINTER;

    CComObject<CAutoUtility>* pUtility;
    CComObject<CAutoUtility>::CreateInstance(&pUtility);
    if (!pUtility)
        return E_FAIL;

    pUtility->Init (m_pDoc);

    return pUtility->QueryInterface (ppUtility);
    }

STDMETHODIMP CAutoDoc::Activate ()
    {
    POSITION pos = m_pDoc->GetFirstViewPosition();
    if (pos)
        {
        CIcadView* pView;
        pView = (CIcadView*) m_pDoc->GetNextView(pos);
        if (pView)
            {
            CFrameWnd* pFrame;
            pFrame = pView->GetParentFrame();

            if (pFrame)
                {
                pFrame->ActivateFrame();
                return NOERROR;
                }
            }
        }

    return E_FAIL;
    }

STDMETHODIMP CAutoDoc::Close (VARIANT_BOOL SaveChanges, BSTR File)
    {
    BOOL ret = TRUE;

    if (SaveChanges == VARIANT_TRUE)
        {
        CString strResult = m_pDoc->GetPathName();
        CString strFile(File);

        if (strResult.IsEmpty())    //the doc has never been saved
            {
            ret = m_pDoc->DoSave (strFile, FALSE);  // if File is NULL, the SaveAs dialog comes up
            }
        else
            {
            if (strFile.IsEmpty())
                {
                strFile = strResult;
                }
            else
                {
                //if there is no path for the file, add one
                if (strFile.ReverseFind ('\\') == -1)
                    {
                    char cwd[IC_ACADBUF];
                    GetCurrentDirectory(IC_ACADBUF, cwd);
                    CString strCurDir = cwd;

                    strFile = strCurDir + '\\' + strFile;
                    }

                //if the file has no extension, add one
                if (strFile.ReverseFind('.') == -1)
                    {
                    strFile = strFile + ".dwg";
                    }
                }

            //the doc has been saved - check if we are saving to a different name
            if (strResult.CompareNoCase(strFile))
                ret = m_pDoc->DoSave (strFile, TRUE);
            else
                ret = m_pDoc->DoSave (strFile, FALSE);
            }
        }

    if (ret)
        m_pDoc->OnCloseDocument();

    return (ret ? NOERROR : E_FAIL);
    }

STDMETHODIMP CAutoDoc::Export (BSTR FileName, BSTR Extension, IIcadSelectionSet * SelectionSet)
    {
    CString strFileName(FileName);
    if (strFileName.IsEmpty())
        return E_INVALIDARG;

    if (strFileName.ReverseFind('.') != -1)
        return E_INVALIDARG;

    CString strExt(Extension);
    if (strExt.IsEmpty())
        return E_INVALIDARG;

    int FileType;
    if (strExt.CompareNoCase ("DXF") == 0)
        FileType = 1;
    else if (strExt.CompareNoCase ("BMP") == 0)
        FileType = 24;
    else if (strExt.CompareNoCase ("WMF") == 0)
        FileType = 25;
    else if (strExt.CompareNoCase ("EMF") == 0)
        FileType = 26;
    else
        return E_INVALIDARG;

    CString strFile = strFileName + "." + strExt;

    sds_name sset;
    HRESULT hr;

    if (SelectionSet)
        {
        hr = ConvertAutoSelectionSetToSDSsset (SelectionSet, sset, m_pDoc);
        if (FAILED(hr))
            return E_FAIL;
        }

    if (FileType == 1)
        {
        sds_point origin;
        origin[0] = origin[1] = origin[2] = 0.0;
        return m_pDoc->AUTO_cmd_wblock_entmake(sset, origin, strFile.GetBuffer(0), (char *)&FileType, NULL);
        }
    else
        return m_pDoc->ExportEntities (strFile, FileType, sset);
    }

STDMETHODIMP CAutoDoc::Import (BSTR FileName, IIcadPoint * InsertionPoint, double ScaleFactor)
    {
    CString strFile (FileName);
    if (strFile.IsEmpty())
        return E_INVALIDARG;

    //check for file existence
    if ((-1) == access(strFile, 00))
        return E_INVALIDARG;

    char ext[10];
    ic_getext( strFile.GetBuffer(0), ext);
    CString StrExt(ext);

    if (StrExt.CompareNoCase (".DXF") == 0)
        m_pDoc->m_nFileType = IC_DXF;
	else if (StrExt.CompareNoCase (".DWG") != 0)
        return E_INVALIDARG;

    db_insert *insert = new db_insert;
    if (ScaleFactor)
        {
        insert->set_41(ScaleFactor);
        insert->set_42(ScaleFactor);
        insert->set_43(ScaleFactor);
        }

    sds_point pt;
    pt[0] = pt[1] = pt[2] = 0.0;
    if (InsertionPoint)
        {
        InsertionPoint->get_x (&pt[0]);
        InsertionPoint->get_y (&pt[1]);
        InsertionPoint->get_z (&pt[2]);
        }
    insert->set_10(pt);

    return m_pDoc->InsertBlock(strFile.GetBuffer(0), insert, pt, NULL, TRUE);

    }

STDMETHODIMP CAutoDoc::NewWindow ()
    {
    CIcadApp *pApp = GetApp();
    SendMessage(pApp->m_pIcadMainWnd->GetSafeHwnd(), WM_COMMAND, ICAD_WNDACTION_WOPEN, 0 );
    return NOERROR;
    }

STDMETHODIMP CAutoDoc::PrintDrawing (SHORT nCopies)
    {
    if (nCopies < 1)
        return E_INVALIDARG;

    CIcadView* pView = (CIcadView*)(((CMainWindow*)GetApp()->m_pMainWnd)->m_pCurView);
    if(pView == NULL)
        return E_FAIL;

    int i;
    for (i = 0; i < nCopies; i++)
        pView->FilePrint(FALSE);

    return NOERROR;
    }

STDMETHODIMP CAutoDoc::PrintPreview ()
    {
    CIcadView* pView = (CIcadView*)(((CMainWindow*)GetApp()->m_pMainWnd)->m_pCurView);
    if(pView == NULL)
        return E_FAIL;

    pView->FilePrintPreview();
    return NOERROR;
    }

STDMETHODIMP CAutoDoc::Save ()
    {
    BOOL ret = FALSE;
    CString strResult = m_pDoc->GetPathName();

    if (strResult.IsEmpty())    //the doc has never been saved
        {
        ret = m_pDoc->DoSave (NULL, FALSE); // the SaveAs dialog comes up
        }
    else
        {
        ret = m_pDoc->DoSave (strResult, FALSE);
        }

    return (ret ? NOERROR : E_FAIL);
    }

STDMETHODIMP CAutoDoc::SaveAs (BSTR SaveFile, FileVersions Version)
    {
    CString strFile(SaveFile);
    if (strFile.IsEmpty())
        return E_INVALIDARG;

    //save the original filetype and version
    int OldFileType = m_pDoc->m_nFileType;
    int OldFileVersion = m_pDoc->m_nFileVersion;

    //Set the new file version
    m_pDoc->m_nFileVersion = Version;

    //Set the new file type
    char ext[10];
    ic_getext( strFile.GetBuffer(0), ext);
    CString StrExt(ext);
    if (StrExt.IsEmpty() || (StrExt.Compare (".") == 0) || (StrExt.CompareNoCase (".DWG") == 0)) // EBATECH
        m_pDoc->m_nFileType = IC_DWG;
    else if (StrExt.CompareNoCase (".DXF") == 0)
        m_pDoc->m_nFileType = IC_DXF;
    else
        return E_INVALIDARG;

    //Save the file
    BOOL ret = FALSE;
    ret = m_pDoc->OnSaveDocument (strFile);

    //restore the original values
    m_pDoc->m_nFileType = OldFileType;
    m_pDoc->m_nFileVersion = OldFileVersion;

    return (ret ? NOERROR : E_FAIL);
    }

STDMETHODIMP CAutoDoc::WBlock (BSTR FileName, IIcadSelectionSet * SelectionSet, IIcadPoint * Origin)
    {
    CString name(FileName);
    if (name.IsEmpty())
        return E_INVALIDARG;

    sds_point origin;
    origin[0] = origin[1] = origin[2] = 0.0;
    if (Origin)
        {
        Origin->get_x (&origin[0]);
        Origin->get_y (&origin[1]);
        Origin->get_z (&origin[2]);
        }

    if (SelectionSet == NULL)
        return E_INVALIDARG;

    HRESULT hr;
    sds_name sset;
    hr = ConvertAutoSelectionSetToSDSsset (SelectionSet, sset, m_pDoc);
    if (FAILED(hr))
        return E_FAIL;

    return m_pDoc->AUTO_cmd_wblock_entmake(sset, origin, name.GetBuffer(0), NULL, NULL);
    }

STDMETHODIMP CAutoDoc::GetVariable (BSTR Name, VARIANT * pValue)
    {
    if (pValue == NULL)
        return E_POINTER;

    CString SysVar(Name);
    if (SysVar.IsEmpty())
        return E_INVALIDARG;

    struct sds_resbuf rb;
    HRESULT hr;

    hr = GetApp()->SDSGetVar (m_pDoc, SysVar, &rb);
    if (FAILED(hr))
        return E_INVALIDARG;

    switch (rb.restype)
        {
        case RTREAL:    //A Real - 8 Bytes.
        case RTANG:
        case RTORINT:
            {
            (*pValue).vt = VT_R8;
            (*pValue).dblVal = rb.resval.rreal;
            return NOERROR;
            }
            break;

        case RTPOINT:   //A 2D Point - 3 Reals @ 8 Bytes each.  Last is ignored
        case RT3DPOINT: //A 3D Point
            {
            CComObject<CAutoPoint> *pPoint;
            CComObject<CAutoPoint>::CreateInstance(&pPoint);
            if (!pPoint)
                return E_FAIL;

            pPoint->Init(NULL, NULL, rb.resval.rpoint[0], rb.resval.rpoint[1], rb.resval.rpoint[2]);

            IDispatch * pDisp;
            hr = pPoint->QueryInterface (&pDisp);
            if (FAILED(hr))
                return E_FAIL;

            (*pValue).vt = VT_DISPATCH;
            (*pValue).pdispVal = pDisp;
            return NOERROR;
            }
            break;

        case RTSHORT:   //A Short - 2 Bytes.
            {
            (*pValue).vt = VT_I2;
            (*pValue).iVal = rb.resval.rint;
            return NOERROR;
            }
            break;

        case RTLONG:    //A Long - 4 Bytes.
            {
            (*pValue).vt = VT_I4;
            (*pValue).lVal = rb.resval.rlong;
            return NOERROR;
            }
            break;

        case RTSTR:     //A string
            {
            CString str(rb.resval.rstring);

            (*pValue).vt = VT_BSTR;
            (*pValue).bstrVal = str.AllocSysString();
            return NOERROR;
            }
            break;

        case RTNONE:    //There is no value for this return type.
        case RTENAME:   //An Entity Name - 2 Longs @ 4 Bytes each
        case RTPICKS:   //A Selection Set Name - 2 Longs @ 4 Bytes each.
        case RTVOID:    //Void - ???
        case RTLB:      //A List Begin - ???
        case RTLE:      //A List End - ???
        case RTDOTE:    //Dot, for dotted pairs - ???
        case RTT:       //True, for AutoLISP - ???
        case RTNIL:     //Nil, for AutoLISP - ???
        case RTDXF0:    //The Zero Group Code for DXF Lists - ???
        default:
            ASSERT (0); //shouldn't be here...
            break;
        }

    return E_FAIL;
    }

STDMETHODIMP CAutoDoc::SetVariable (BSTR Name, VARIANT Value)
    {
    CString SysVar(Name);
    if (SysVar.IsEmpty())
        return E_INVALIDARG;

    struct sds_resbuf rb;
    HRESULT hr;

    switch (Value.vt)
        {
        case VT_R4:
            rb.restype = RTREAL;
            rb.resval.rreal = Value.fltVal;
            break;

        case VT_R8:
            rb.restype = RTREAL;
            rb.resval.rreal = Value.dblVal;
            break;

        case VT_DISPATCH:
            {
            CComPtr<IIcadPoint> pPoint;

            hr = (Value.pdispVal)->QueryInterface (&pPoint);
            if (FAILED(hr))
                return E_FAIL;

            sds_point pt;
            pPoint->get_x (&pt[0]);
            pPoint->get_y (&pt[1]);
            pPoint->get_z (&pt[2]);

            rb.restype = RT3DPOINT;
            ic_ptcpy (rb.resval.rpoint, pt);
            }
            break;

        case VT_I2:
            rb.restype = RTSHORT;
            rb.resval.rint = Value.iVal;
            break;

        case VT_I4:
            rb.restype = RTLONG;
            rb.resval.rlong = Value.lVal;
            break;

        case VT_BSTR:
            {
            CString str(Value.bstrVal);
            rb.restype = RTSTR;
            rb.resval.rstring = str.GetBuffer(0);
            }
            break;

        default:
            ASSERT(0);
            return E_INVALIDARG;
            break;
        }

    hr = GetApp()->SDSSetVar (m_pDoc, SysVar, &rb);
    if (FAILED(hr))
        return E_INVALIDARG;

    return NOERROR;
    }
// Modified CyberAge VSB 02/23/2001 [
// Reason: Mail for Sander dated 02/08/2001
/*
STDMETHODIMP CAutoDoc::GetLispVariable (BSTR Name, VARIANT * pValue)
    {
    if (pValue == NULL)
        return E_POINTER;

    return E_NOTIMPL;
    }
*/
// Modified CyberAge VSB 02/23/2001 ]

STDMETHODIMP CAutoDoc::GetLispVariable (BSTR Name, VARIANT * pValue)
    {
        if (pValue == NULL)
            return E_POINTER;

        CString SysVar(Name);
        if (SysVar.IsEmpty())
            return E_INVALIDARG;

        struct sds_resbuf * rb = NULL;
        HRESULT hr;

        hr = GetApp()->SDSGetLispVar (m_pDoc, SysVar, &rb);
        if (FAILED(hr))
            return E_INVALIDARG;

        switch (rb->restype)
        {

        case RTREAL:    //A Real - 8 Bytes.

        case RTANG:
        case RTORINT:
            {
                (*pValue).vt = VT_R8;
                (*pValue).dblVal = rb->resval.rreal;
                return NOERROR;
            }
            break;

        case RTPOINT:   //A 2D Point - 3 Reals @ 8 Bytes each.  Last is ignored
        case RT3DPOINT: //A 3D Point
            {
            /*
            CComObject<CAutoPoint> *pPoint;
            CComObject<CAutoPoint>::CreateInstance(&pPoint);
            if (!pPoint)
            return E_FAIL;

              pPoint->Init(NULL, NULL, rb->resval.rpoint[0], rb->resval.rpoint[1], rb->resval.rpoint[2]);

                IDispatch * pDisp;
                hr = pPoint->QueryInterface (&pDisp);
                if (FAILED(hr))
                return E_FAIL;

                  (*pValue).vt = VT_DISPATCH;
                  (*pValue).pdispVal = pDisp;
                  return NOERROR;
                */
                return E_NOTIMPL;
            }
            break;

        case RTSHORT:   //A Short - 2 Bytes.
            {
                (*pValue).vt = VT_I2;
                (*pValue).iVal = rb->resval.rint;
                return NOERROR;
            }
            break;

        case RTLONG:    //A Long - 4 Bytes.
            {
                (*pValue).vt = VT_I4;
                (*pValue).lVal = rb->resval.rlong;
                return NOERROR;
            }
            break;

        case RTSTR:     //A string
            {
                CString str(rb->resval.rstring);

                (*pValue).vt = VT_BSTR;
                (*pValue).bstrVal = str.AllocSysString();
                return NOERROR;
            }
            break;
			// EBATECH(CNBR) -[ 2002/4/24 Support BOOL
        case RTT:       //True, for AutoLISP - ???
        	{
                (*pValue).vt = VT_BOOL;
                (*pValue).boolVal = VARIANT_TRUE;
                return NOERROR;
			}
			break;
        case RTNIL:     //Nil, for AutoLISP - ???
        	{
                (*pValue).vt = VT_BOOL;
                (*pValue).boolVal = VARIANT_FALSE;
                return NOERROR;
			}
			break;
			// EBATECH(CNBR) ]-

        case RTNONE:    //There is no value for this return type.
        case RTENAME:   //An Entity Name - 2 Longs @ 4 Bytes each
        case RTPICKS:   //A Selection Set Name - 2 Longs @ 4 Bytes each.
        case RTVOID:    //Void - ???
        case RTLB:      //A List Begin - ???
        case RTLE:      //A List End - ???
        case RTDOTE:    //Dot, for dotted pairs - ???
        case RTDXF0:    //The Zero Group Code for DXF Lists - ???
        default:
            ASSERT (0); //shouldn't be here...
            break;
        }

        return E_FAIL;
    }
// Modified CyberAge VSB 02/23/2001 [
// Reason: Mail for Sander dated 02/08/2001
/*
STDMETHODIMP CAutoDoc::SetLispVariable (BSTR Name, VARIANT Value)
    {
    return E_NOTIMPL;
    }
*/
// Modified CyberAge VSB 02/23/2001 ]
STDMETHODIMP CAutoDoc::SetLispVariable (BSTR Name, VARIANT Value)
    {
        CString Var(Name);
        if (Var.IsEmpty())
            return E_INVALIDARG;

        struct sds_resbuf *rb = new struct sds_resbuf;
        if (  rb == NULL )
            return E_FAIL;

        CString cmd;
        HRESULT hr;

        switch (Value.vt)
        {
        case VT_R4:

            rb->restype = RTREAL;
            rb->resval.rreal = Value.fltVal;
            cmd.Format("(Setq %s  %f )",Var,Value.fltVal ) ;
            break;

        case VT_R8:
            rb->restype = RTREAL;
            rb->resval.rreal = Value.dblVal;
            cmd.Format("(Setq %s  %e )",Var,Value.dblVal ) ;
            break;

        case VT_DISPATCH:
            {
                /*
                CComPtr<IIcadPoint> pPoint;

                hr = (Value.pdispVal)->QueryInterface (&pPoint);
                if (FAILED(hr))
                    return E_FAIL;

                sds_point pt;
                pPoint->get_x (&pt[0]);
                pPoint->get_y (&pt[1]);
                pPoint->get_z (&pt[2]);

                rb->restype = RT3DPOINT;
                ic_ptcpy (rb->resval.rpoint, pt);
                cmd.Format("(Setq %s  '( %ld %ld %ld ) )",Var,pt[0],pt[1],pt[2] ) ;
                */
                return E_NOTIMPL;
            }
            break;

        case VT_I2:
            rb->restype = RTSHORT;
            rb->resval.rint = Value.iVal;
            cmd.Format("(Setq %s  %d )",Var,Value.iVal ) ;
            break;

        case VT_I4:
            rb->restype = RTLONG;
            rb->resval.rlong = Value.lVal;
            cmd.Format("(Setq %s  %ld )",Var,Value.lVal) ;
            break;

        case VT_BSTR:
            {
                CString str(Value.bstrVal);
                rb->restype = RTSTR;
                rb->resval.rstring = str.GetBuffer(0);
                cmd.Format("(Setq %s  \"%s\" )",Var,str ) ;
            }
            break;
			// EBATECH(CNBR) -[ 2002/4/24 Support BOOL
		case VT_BOOL:
            {
				if( Value.boolVal ) {
		            rb->restype = RTT;
		            cmd.Format("(Setq %s  T)",Var) ;
		        } else {
		            rb->restype = RTNIL;
		            cmd.Format("(Setq %s  nil)",Var) ;
		        }
            }
            break;
			// EBATECH(CNBR) ]-
        default:
            ASSERT(0);
            return E_INVALIDARG;
            break;
        }

        hr = GetApp()->SDSSetLispVar (m_pDoc, cmd, &rb);

        //if ( rb != NULL )
        //  delete rb;

        if (FAILED(hr))
            return E_INVALIDARG;

        return NOERROR;
    }

STDMETHODIMP CAutoDoc::EvaluateLispExpression (BSTR LispExpression, VARIANT *LispResult)
    {
    if (LispResult == NULL)
        return E_POINTER;

    return E_NOTIMPL;
    }

STDMETHODIMP CAutoDoc::HandleToObject (BSTR Handle, IDispatch * * pObject)
    {
    CString str(Handle);
    if (str.IsEmpty())
        return E_INVALIDARG;

    db_objhandle handle((LPCTSTR)str);

    db_handitem *pDbHandItem = m_pDoc->m_dbDrawing->handent (handle);

    if (pDbHandItem == NULL)
        return E_FAIL;

    CComPtr<IIcadEntity> pEnt;

    HRESULT hr = CreateEntityWrapper (m_pDoc, NULL, pDbHandItem, &pEnt);
    if (FAILED(hr))
        return hr;

    return pEnt->QueryInterface (pObject);
    }

STDMETHODIMP CAutoDoc::PurgeAll ()
    {
    m_pDoc->PurgeDrawing ();

    return NOERROR;
    }

STDMETHODIMP CAutoDoc::Regen (WhichViewport Viewports)
    {
    if (Viewports == vicActiveViewport)
        m_pDoc->RegenCurrentView ();
    else if (Viewports == vicAllViewports)
        m_pDoc->RegenAllViews ();
    else
        return E_INVALIDARG;

    return NOERROR;
    }

// EBATECH(CNBR) -[ 2002/4/24 Support SHAPE
STDMETHODIMP CAutoDoc::LoadShapeFile(BSTR FileName)
    {
    CString str(FileName);
    if (str.IsEmpty())
        return E_INVALIDARG;

    m_pDoc->LoadShapeFile((char*)str.GetBuffer(0));

    return NOERROR;
    }
// EBATECH(CNBR) ]-
