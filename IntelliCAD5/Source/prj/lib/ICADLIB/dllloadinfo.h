/* D:\ICADDEV\PRJ\LIB\ICADLIB\DLLLOADINFO.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * $Revision: 1.1.1.1 $ $Date: 2000/01/19 13:41:38 $ 
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

// ***********************************************
// DLLLOADINFO.H
//
// Duplicates part of automatically generated files.
// Needed to get rid of circular dependency
//

#ifndef __IVisioDLLLoadInfo_INTERFACE_DEFINED__
#define __IVisioDLLLoadInfo_INTERFACE_DEFINED__

/* interface IVisioDLLLoadInfo */
/* [unique][helpstring][dual][uuid][object] */ 


    
MIDL_INTERFACE("B8094F9D-D014-11D2-B17B-00104B22E914")
IVisioDLLLoadInfo : public IDispatch
	{
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetModuleHandle( 
            /* [retval][out] */ long __RPC_FAR *pModuleHandle) = 0;
        
    };

#endif

