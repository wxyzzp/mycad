// DBDLLLoadInfo.h : Declaration of the CDBDLLLoadInfo

#ifndef __DBDLLLOADINFO_H_
#define __DBDLLLOADINFO_H_

#include "dbmodule.h"
#include "dbdll.h"

/////////////////////////////////////////////////////////////////////////////
// DBDLLLoadInfo
class ATL_NO_VTABLE DBDLLLoadInfo : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<DBDLLLoadInfo, &CLSID_DBDLLLoadInfo>,
	public IDispatchImpl<IVisioDLLLoadInfo, &IID_IVisioDLLLoadInfo, &LIBID_DBMODULELib>
{
public:
	DBDLLLoadInfo()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_DBDLLLOADINFO)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(DBDLLLoadInfo)
	COM_INTERFACE_ENTRY(IVisioDLLLoadInfo)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// IVisioDLLLoadInfo
public:
	STDMETHOD(GetModule)(long *pModuleHandle);
};

#endif //__DBDLLLOADINFO_H_
