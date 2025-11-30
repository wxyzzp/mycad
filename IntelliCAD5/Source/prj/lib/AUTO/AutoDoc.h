/* INTELLICAD\PRJ\LIB\AUTO\AUTODOC.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Document Automation Class.
 * This class implements the Automation Interface - IIcadDocument
 * 
 */ 


#ifndef __AUTODOC_H_
#define __AUTODOC_H_

#include "AutoDLL.h"
class CAutoMSpace;
class CAutoPSpace;
class CIcadDoc;

/////////////////////////////////////////////////////////////////////////////
// CAutoDoc
class AUTO_DECLSPEC ATL_NO_VTABLE CAutoDoc : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CAutoDoc, &CLSID_Document>,
	public ISupportErrorInfo,
	public IDispatchImpl<IIcadDocument, &IID_IIcadDocument, &LIBID_IntelliCAD>,
	public IProvideClassInfo2Impl<&CLSID_Document, &DIID__EIcadDocument, &LIBID_IntelliCAD>,
	public IConnectionPointContainerImpl<CAutoDoc>,
	public IConnectionPointImpl<CAutoDoc, &DIID__EIcadDocument, CComDynamicUnkArray>
{
private:
	CComPtr<IIcadModelSpace> m_pMSpace;
	CComPtr<IIcadPaperSpace> m_pPSpace;
	CComPtr<IIcadSelectionSets> m_pSelSets;
	CIcadDoc * m_pDoc;

public:
	CAutoDoc()
		{
		m_pMSpace = NULL;
		m_pPSpace = NULL;
		m_pSelSets = NULL;
		}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoDoc)
		COM_INTERFACE_ENTRY(IIcadDocument)
		COM_INTERFACE_ENTRY(ISupportErrorInfo)
		COM_INTERFACE_ENTRY(IProvideClassInfo)
		COM_INTERFACE_ENTRY(IProvideClassInfo2)
		COM_INTERFACE_ENTRY(IConnectionPointContainer)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadDocument)
	END_COM_MAP()

	BEGIN_CONNECTION_POINT_MAP(CAutoDoc)
		CONNECTION_POINT_ENTRY(DIID__EIcadDocument)
	END_CONNECTION_POINT_MAP()

	HRESULT Init(CIcadDoc *pDoc);
	HRESULT FinalRelease();
	HRESULT FinalConstruct();

	// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

	// IIcadDocument
	STDMETHOD(get_Active)(VARIANT_BOOL * Active);
	STDMETHOD(get_ActiveDimStyle)(IIcadDimensionStyle * * ppDimStyle);
	STDMETHOD(put_ActiveDimStyle)(IIcadDimensionStyle * ppDimStyle);
	STDMETHOD(get_ActiveLayer)(IIcadLayer * * ppLayer);
	STDMETHOD(put_ActiveLayer)(IIcadLayer * ppLayer);
	STDMETHOD(get_ActiveLinetype)(IIcadLinetype * * ppLinetype);
	STDMETHOD(put_ActiveLinetype)(IIcadLinetype * ppLinetype);
	STDMETHOD(get_ActivePViewport)(IIcadPViewport * * ppVPort);
	STDMETHOD(put_ActivePViewport)(IIcadPViewport * ppVPort);
	STDMETHOD(get_ActiveSelectionSet)(IIcadSelectionSet * * ppSelSet);
	STDMETHOD(get_ActiveSpace)(PMSpace * pSpace);
	STDMETHOD(put_ActiveSpace)(PMSpace pSpace);
	STDMETHOD(get_ActiveTextStyle)(IIcadTextStyle * * ppTextStyle);
	STDMETHOD(put_ActiveTextStyle)(IIcadTextStyle * ppTextStyle);
	STDMETHOD(get_ActiveUCS)(IIcadUserCoordSystem * * ppUCS);
	STDMETHOD(put_ActiveUCS)(IIcadUserCoordSystem * ppUCS);
	STDMETHOD(get_ActiveViewport)(IIcadViewport * * ppViewport);
	STDMETHOD(put_ActiveViewport)(IIcadViewport * ppViewport);
	STDMETHOD(get_Blocks)(IIcadBlocks * * ppBlocks);
	STDMETHOD(get_DimensionStyles)(IIcadDimensionStyles * * ppDimensionStyles);
	STDMETHOD(get_Layers)(IIcadLayers * * ppLayers);
	STDMETHOD(get_Linetypes)(IIcadLinetypes * * ppLinetypes);
	STDMETHOD(get_RegisteredApplications)(IIcadRegisteredApplications * * ppAppIDs);
	STDMETHOD(get_SelectionSets)(IIcadSelectionSets * * ppSelSets);
	STDMETHOD(get_TextStyles)(IIcadTextStyles * * ppTextStyles);
	STDMETHOD(get_UserCoordinateSystems)(IIcadUserCoordSystems * * ppUserCoordSystems);
	STDMETHOD(get_Viewports)(IIcadViewports * * ppViewports);
	STDMETHOD(get_Views)(IIcadViews * * ppViews);
	STDMETHOD(get_ModelSpace)(IIcadModelSpace * * ppEntities);
	STDMETHOD(get_PaperSpace)(IIcadPaperSpace * * ppEntities);
	STDMETHOD(get_Preferences)(IIcadDocumentPreferences * * ppPreferences);
	STDMETHOD(get_Application)(IIcadApplication * * ppApp);
	STDMETHOD(get_Parent)(IIcadDocuments * * ppParent);
	STDMETHOD(get_FullName)(BSTR * pstrFullAppName);
	STDMETHOD(get_Name)(BSTR * pAppName);
	STDMETHOD(get_Path)(BSTR * pPath);
	STDMETHOD(get_ReadOnly)(VARIANT_BOOL * pbReadOnly);
	STDMETHOD(get_Saved)(VARIANT_BOOL * pbSaved);
	STDMETHOD(get_MSpace)(VARIANT_BOOL * pSpace);
	STDMETHOD(put_MSpace)(VARIANT_BOOL pSpace);
	STDMETHOD(get_ObjectSnapMode)(VARIANT_BOOL * SnapMode);
	STDMETHOD(put_ObjectSnapMode)(VARIANT_BOOL SnapMode);
	STDMETHOD(get_ElevationModelSpace)(double * Elevation);
	STDMETHOD(put_ElevationModelSpace)(double Elevation);
	STDMETHOD(get_ElevationPaperSpace)(double * Elevation);
	STDMETHOD(put_ElevationPaperSpace)(double Elevation);
	STDMETHOD(get_Limits)(IIcadPoints * * ppLimits);
	STDMETHOD(put_Limits)(IIcadPoints * ppLimits);
	STDMETHOD(get_Utility)(IIcadUtility * * ppUtility);
	STDMETHOD(Activate)();
	STDMETHOD(Close)(VARIANT_BOOL SaveChanges, BSTR File);
	STDMETHOD(Export)(BSTR FileName, BSTR Extension, IIcadSelectionSet * SelectionSet);
	STDMETHOD(Import)(BSTR FileName, IIcadPoint * InsertionPoint, double ScaleFactor);
	STDMETHOD(NewWindow)();
	STDMETHOD(PrintDrawing)(SHORT nCopies);
	STDMETHOD(PrintPreview)();
	STDMETHOD(Save)();
	STDMETHOD(SaveAs)(BSTR SaveFile, FileVersions Version);
	STDMETHOD(WBlock)(BSTR FileName, IIcadSelectionSet * SelectionSet, IIcadPoint * Origin);
	STDMETHOD(GetVariable)(BSTR Name, VARIANT * pValue);
	STDMETHOD(SetVariable)(BSTR Name, VARIANT Value);
	STDMETHOD(GetLispVariable)(BSTR Name, VARIANT * pValue);
	STDMETHOD(SetLispVariable)(BSTR Name, VARIANT Value);
	STDMETHOD(EvaluateLispExpression)(BSTR LispExpression, VARIANT *LispResult) ;
	STDMETHOD(HandleToObject)(BSTR Handle, IDispatch * * pObject);
	STDMETHOD(PurgeAll)();
	STDMETHOD(Regen)(WhichViewport Viewports);
	STDMETHOD(LoadShapeFile)(BSTR FileName);	// EBATECH(CNBR) 2002/4/24 Support SHAPE
};

#endif //__AUTODOC_H_



