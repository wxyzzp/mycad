// GRDLLLoadInfo.h : Declaration of the CGRDLLLoadInfo

#ifndef __GRDLLLOADINFO_H_
#define __GRDLLLOADINFO_H_

#include "dbmodule.h"
#include "grrc.h"

/////////////////////////////////////////////////////////////////////////////
// GRDLLLoadInfo
class ATL_NO_VTABLE GRDLLLoadInfo : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<GRDLLLoadInfo, &CLSID_GRDLLLoadInfo>,
	public IDispatchImpl<IVisioDLLLoadInfo, &IID_IVisioDLLLoadInfo, &LIBID_GRMODULELib>
{
public:
	GRDLLLoadInfo()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_GRDLLLOADINFO)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(GRDLLLoadInfo)
	COM_INTERFACE_ENTRY(IVisioDLLLoadInfo)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// IVisioDLLLoadInfo
public:
	STDMETHOD(GetModule)(long *pModuleHandle);
};

#endif //__GRDLLLOADINFO_H_
