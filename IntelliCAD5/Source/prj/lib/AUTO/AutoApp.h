/* INTELLICAD\PRJ\LIB\AUTO\AUTOAPP.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Application Automation Class.
 * This class implements the Automation Interface - IIcadApplication
 * 
 */ 


#ifndef __AUTOAPP_H_
#define __AUTOAPP_H_

#include "AutoDLL.h"
#include "AutoLibrary.h"

class CIcadApp;

/////////////////////////////////////////////////////////////////////////////
// CAutoApp
class AUTO_DECLSPEC ATL_NO_VTABLE CAutoApp : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public IExternalConnection,
	public IClassFactory,
	public CComCoClass<CAutoApp, &CLSID_Application>,
	public ISupportErrorInfo,
	public IProvideClassInfo2Impl<&CLSID_Application, NULL, &LIBID_IntelliCAD>,
	public IDispatchImpl<IIcadApplication, &IID_IIcadApplication, &LIBID_IntelliCAD>
{
	friend class CIcadApp;

protected:
	VARIANT_BOOL m_bVisible;
	CComPtr<IIcadLibrary> m_pLibrary;
	CIcadApp * m_pApp;

public:
	CAutoApp(): m_dwStrongLocks(0)
		{
		m_bVisible = TRUE;
		}
	
	HRESULT Init(CIcadApp *pApp);
	HRESULT FinalRelease();
	HRESULT FinalConstruct();

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoApp)
		COM_INTERFACE_ENTRY(IIcadApplication)
		COM_INTERFACE_ENTRY(ISupportErrorInfo)
		COM_INTERFACE_ENTRY(IExternalConnection)
		COM_INTERFACE_ENTRY(IClassFactory)
		COM_INTERFACE_ENTRY(IProvideClassInfo)
		COM_INTERFACE_ENTRY(IProvideClassInfo2)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadApplication)
	END_COM_MAP()


	typedef CComCreator< CComObjectNoLock< CAutoApp > > _CreatorClass;
	DWORD m_dwStrongLocks; // count of external strong locks

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

public:
// IExternalConnection
	STDMETHOD_(DWORD, AddConnection)(DWORD extconn, DWORD dwreserved)
		{
		if (extconn != EXTCONN_STRONG)
			return 1;
		
		if (m_dwStrongLocks == 0)
			_Module.Lock();
		
		m_dwStrongLocks++;
		return m_dwStrongLocks;
		}
	
	STDMETHOD_(DWORD, ReleaseConnection)(DWORD extconn, DWORD dwreserved, BOOL fLastReleaseCloses) 
		{
		if (extconn != EXTCONN_STRONG)
			return 0;
		
		m_dwStrongLocks--;
		ASSERT(m_dwStrongLocks != ~0UL);
		
		if (m_dwStrongLocks == 0)
			_Module.Unlock();
		
		return m_dwStrongLocks;
		}

	// IClassFactory members.
	STDMETHODIMP CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppv);
	STDMETHODIMP LockServer(BOOL fLock);


	// IIcadApplication
	STDMETHOD(get_ActiveDocument)(IIcadDocument * * ppDoc);
	STDMETHOD(get_ActiveWindow)(IIcadWindow * * ppWindow);
	STDMETHOD(get_Application)(IIcadApplication * * ppApp);
	STDMETHOD(get_Caption)(BSTR * pCaption);
	STDMETHOD(get_DefaultFilePath)(BSTR * pDefFilePath);
	STDMETHOD(put_DefaultFilePath)(BSTR pDefFilePath);
	STDMETHOD(get_Documents)(IIcadDocuments * * ppDocs);
	STDMETHOD(get_FullName)(BSTR * pFullAppName);
	STDMETHOD(get_Height)(long * pHeight);
	STDMETHOD(put_Height)(long pHeight);
	STDMETHOD(get_LocaleID)(long * pLocaleId);
	STDMETHOD(get_Left)(long * pLeft);
	STDMETHOD(put_Left)(long pLeft);
	STDMETHOD(get_Library)(IIcadLibrary * * ppLibrary);
	STDMETHOD(get_Name)(BSTR * pstrAppName);
	STDMETHOD(get_Parent)(IIcadApplication * * ppParent);
	STDMETHOD(get_Path)(BSTR * pstrPath);
	STDMETHOD(get_PlotManager)(IIcadPlotManager * * ppPlotManager);
	STDMETHOD(get_Preferences)(IIcadApplicationPreferences * * ppPreferences);
	STDMETHOD(get_StatusBar)(BSTR * pstrStatusText);
	STDMETHOD(put_StatusBar)(BSTR pstrStatusText);
	STDMETHOD(get_Top)(long * pTop);
	STDMETHOD(put_Top)(long pTop);
	STDMETHOD(get_VBE)(IDispatch * * ppVBE);
	STDMETHOD(get_Version)(BSTR * pstrVersion);
	STDMETHOD(get_Visible)(VARIANT_BOOL * pVisible);
	STDMETHOD(put_Visible)(VARIANT_BOOL pVisible);
	STDMETHOD(get_Width)(long * pWidth);
	STDMETHOD(put_Width)(long pWidth);
	STDMETHOD(get_Windows)(IIcadWindows * * ppWindows);
	STDMETHOD(get_Workspace)(IIcadWorkspace * * ppWorkspace);
	STDMETHOD(DefineFunction)(BSTR FunctionName, VARIANT FunctionPointer);
	STDMETHOD(UndefineFunction)(BSTR FunctionName);
	STDMETHOD(Help)(BSTR HelpFile, BSTR HelpContext, long HelpCommand);
	STDMETHOD(Quit)();
	STDMETHOD(ListSDS)(VARIANT * ListOfSDSApps);
	STDMETHOD(LoadSDS)(BSTR ApplicationName);
	STDMETHOD(UnloadSDS)(BSTR ApplicationName);
	STDMETHOD(LoadLISP)(BSTR ApplicationName);
	STDMETHOD(RunCommand)(BSTR Command);
	STDMETHOD(RunScript)(BSTR ScriptName);
	STDMETHOD(GetInterfaceObject)(BSTR ProgID, IDispatch * * ppObject);
	STDMETHOD(IsFunctionLoaded)(BSTR FunctionName, VARIANT_BOOL *Loaded);
};

#endif //__AUTOAPP_H_


