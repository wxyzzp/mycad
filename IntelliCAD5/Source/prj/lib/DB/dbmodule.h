/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Thu Nov 06 14:37:21 2025
 */
/* Compiler settings for K:\cad\IntelliCAD5\Source\prj\lib\DB\dbmodule.idl:
    Os (OptLev=s), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __dbmodule_h__
#define __dbmodule_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __IVisioDLLLoadInfo_FWD_DEFINED__
#define __IVisioDLLLoadInfo_FWD_DEFINED__
typedef interface IVisioDLLLoadInfo IVisioDLLLoadInfo;
#endif 	/* __IVisioDLLLoadInfo_FWD_DEFINED__ */


#ifndef __DBDLLLoadInfo_FWD_DEFINED__
#define __DBDLLLoadInfo_FWD_DEFINED__

#ifdef __cplusplus
typedef class DBDLLLoadInfo DBDLLLoadInfo;
#else
typedef struct DBDLLLoadInfo DBDLLLoadInfo;
#endif /* __cplusplus */

#endif 	/* __DBDLLLoadInfo_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

#ifndef __IVisioDLLLoadInfo_INTERFACE_DEFINED__
#define __IVisioDLLLoadInfo_INTERFACE_DEFINED__

/* interface IVisioDLLLoadInfo */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IVisioDLLLoadInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B8094F9D-D014-11D2-B17B-00104B22E914")
    IVisioDLLLoadInfo : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetModule( 
            /* [retval][out] */ long __RPC_FAR *pModuleHandle) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IVisioDLLLoadInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IVisioDLLLoadInfo __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IVisioDLLLoadInfo __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IVisioDLLLoadInfo __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IVisioDLLLoadInfo __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IVisioDLLLoadInfo __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IVisioDLLLoadInfo __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IVisioDLLLoadInfo __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetModule )( 
            IVisioDLLLoadInfo __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pModuleHandle);
        
        END_INTERFACE
    } IVisioDLLLoadInfoVtbl;

    interface IVisioDLLLoadInfo
    {
        CONST_VTBL struct IVisioDLLLoadInfoVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IVisioDLLLoadInfo_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IVisioDLLLoadInfo_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IVisioDLLLoadInfo_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IVisioDLLLoadInfo_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IVisioDLLLoadInfo_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IVisioDLLLoadInfo_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IVisioDLLLoadInfo_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IVisioDLLLoadInfo_GetModule(This,pModuleHandle)	\
    (This)->lpVtbl -> GetModule(This,pModuleHandle)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IVisioDLLLoadInfo_GetModule_Proxy( 
    IVisioDLLLoadInfo __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pModuleHandle);


void __RPC_STUB IVisioDLLLoadInfo_GetModule_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IVisioDLLLoadInfo_INTERFACE_DEFINED__ */



#ifndef __DBMODULELib_LIBRARY_DEFINED__
#define __DBMODULELib_LIBRARY_DEFINED__

/* library DBMODULELib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_DBMODULELib;

EXTERN_C const CLSID CLSID_DBDLLLoadInfo;

#ifdef __cplusplus

class DECLSPEC_UUID("EE82A59E-CDCD-11D2-B17B-00104B22E914")
DBDLLLoadInfo;
#endif
#endif /* __DBMODULELib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
