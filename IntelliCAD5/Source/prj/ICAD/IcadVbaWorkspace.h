/* INTELLICAD\PRJ\LIB\AUTO\VBAWORKSPACE.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * The CommonProjects object that contains document independent code.
 * Original source is from the Summit APC Sample Library
 * 
 */ 



#ifndef __VBAWORKSPACE_H__
#define __VBAWORKSPACE_H__

//#include "AutoDLL.h"
#include "IcadVbaCommProj.h"
using namespace SUMMIT; //for ActProjectStorage

/////////////////////////////////////////////////////////////////////////////
// CIcadWorkspace
class ATL_NO_VTABLE CIcadWorkspace : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public IExternalConnection,
	public CComCoClass<CIcadWorkspace, &CLSID_Workspace>,
	public ISupportErrorInfoImpl<&IID_IIcadWorkspace>,
	public IDispatchImpl<IIcadWorkspace, &IID_IIcadWorkspace, &LIBID_IntelliCAD>,
	public IProvideClassInfo2Impl<&CLSID_Workspace, &DIID__IcadWorkspaceEvents, &LIBID_IntelliCAD>,
	public IConnectionPointContainerImpl<CIcadWorkspace>,
	public IConnectionPointImpl<CIcadWorkspace, &DIID__IcadWorkspaceEvents, CComDynamicUnkArray>,
	public CActProjectStorage<CIcadWorkspace>,	// ActProjectStorage
	public IActProjectStorageImpl<CIcadWorkspace>,
	public CApcProject<CIcadWorkspace>,
	public CApcProjectItem<CIcadWorkspace>
{
public:
	CIcadWorkspace(): m_dwStrongLocks(0)
		{
		}

	// don't lock the module for an instance of this object,
	// unless something in another process is holding on.
	// this allows you to implement the app object available as a 
	// remote object as well as be alive when you go through MFC's
	// close handler.
	typedef CComCreator< CComObjectNoLock< CIcadWorkspace > > _CreatorClass;

	HRESULT FinalConstruct();
	HRESULT FinalRelease();

	BEGIN_COM_MAP(CIcadWorkspace)
		COM_INTERFACE_ENTRY(IIcadWorkspace)
		COM_INTERFACE_ENTRY(IExternalConnection)
		COM_INTERFACE_ENTRY(ISupportErrorInfo)
		COM_INTERFACE_ENTRY(IProvideClassInfo)
		COM_INTERFACE_ENTRY(IProvideClassInfo2)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadWorkspace)
		COM_INTERFACE_ENTRY(IConnectionPointContainer)
		COM_INTERFACE_ENTRY_IID(__uuidof(IActProjectStorage), IActProjectStorage)
	END_COM_MAP()

	BEGIN_CONNECTION_POINT_MAP(CIcadWorkspace)
		CONNECTION_POINT_ENTRY(DIID__IcadWorkspaceEvents)
	END_CONNECTION_POINT_MAP()


	DWORD m_dwStrongLocks; // count of external strong locks

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

public:
	// IIcadWorkspace
	STDMETHOD(get_Application)(/*[out, retval]*/ IIcadApplication** ppVal);
	STDMETHOD(get_Parent)(/*[out, retval]*/ IIcadApplication** ppVal);
	STDMETHOD(get_ActiveDocument)(/*[out, retval]*/ IIcadDocument** ppVal);
	STDMETHOD(put_Name)(/*[in]*/ BSTR bstr);
	STDMETHOD(get_Name)(/*[out]*/ BSTR* pbstr);

	// ActProjectStorage Overrides
	HRESULT ActProjectStorage_OnClose();
	HRESULT ActProjectStorage_OnDefine();
	HRESULT ActProjectStorage_OnRegister();
	HRESULT ActProjectStorage_OnSave(IStorage* pStg);
	//HRESULT ActProjectStorage_OnOpen();

	// ApcProject Overrides
	HRESULT ApcProject_Save(); // IApcProjectEvents::Save handler
};

#endif //__VBAWORKSPACE_H__


