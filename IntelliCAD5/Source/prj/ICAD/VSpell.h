/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 3.01.75 */
/* at Tue May 06 17:19:35 1997
 */
/* Compiler settings for VSpell.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: none
*/
//@@MIDL_FILE_HEADING(  )
#include "rpc.h"
#include "rpcndr.h"
#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __VSpell_h__
#define __VSpell_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __IVSpell_FWD_DEFINED__
#define __IVSpell_FWD_DEFINED__
typedef interface IVSpell IVSpell;
#endif 	/* __IVSpell_FWD_DEFINED__ */


#ifndef __CICSpell_FWD_DEFINED__
#define __CICSpell_FWD_DEFINED__

#ifdef __cplusplus
typedef class CICSpell CICSpell;
#else
typedef struct CICSpell CICSpell;
#endif /* __cplusplus */

#endif 	/* __CICSpell_FWD_DEFINED__ */


#ifndef __CCSAPISpell_FWD_DEFINED__
#define __CCSAPISpell_FWD_DEFINED__

#ifdef __cplusplus
typedef class CCSAPISpell CCSAPISpell;
#else
typedef struct CCSAPISpell CCSAPISpell;
#endif /* __cplusplus */

#endif 	/* __CCSAPISpell_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

#ifndef __IVSpell_INTERFACE_DEFINED__
#define __IVSpell_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IVSpell
 * at Tue May 06 17:19:35 1997
 * using MIDL 3.01.75
 ****************************************/
/* [unique][helpstring][dual][uuid][object] */ 



EXTERN_C const IID IID_IVSpell;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("26E39D50-C628-11D0-9067-00609764089F")
    IVSpell : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Init( 
            BOOL bIgnoreNum,
            LCID lcLocale,
            LPSTR lpszEnginePath,
            LPSTR lpszDicPath) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Spell( 
            LPSTR lpszWord,
            LPWORD lpwResult) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Suggest( 
            LPSTR lpszWord,
            LPSTR lpszSuggestions,
            LPDWORD lpdwNumSuggestions,
            DWORD dwSuggestionBufLen,
            LPWORD lpwResult) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IgnoreAll( 
            LPSTR lpszIgnoreThis,
            LPWORD lpwResult) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LoadUserDictionary( 
            LPSTR lpszDicPath,
            LPWORD lpwResult) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE UnloadUserDictionary( 
            LPSTR lpszDicPath) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddToUserDictionary( 
            LPSTR lpszDicPath,
            LPSTR lpszAddThis,
            LPWORD lpwResult) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddUnsupportedLocale( 
            LCID lcLocale) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IsLocaleSupported( 
            LCID lcLocale,
            BOOL __RPC_FAR *lpbSupported) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ChangeLocale( 
            LCID lcLocale,
            LPSTR lpszDicPath) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetLocale( 
            LCID __RPC_FAR *lplcLocale) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IVSpellVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IVSpell __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IVSpell __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IVSpell __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IVSpell __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IVSpell __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IVSpell __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IVSpell __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Init )( 
            IVSpell __RPC_FAR * This,
            BOOL bIgnoreNum,
            LCID lcLocale,
            LPSTR lpszEnginePath,
            LPSTR lpszDicPath);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Spell )( 
            IVSpell __RPC_FAR * This,
            LPSTR lpszWord,
            LPWORD lpwResult);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Suggest )( 
            IVSpell __RPC_FAR * This,
            LPSTR lpszWord,
            LPSTR lpszSuggestions,
            LPDWORD lpdwNumSuggestions,
            DWORD dwSuggestionBufLen,
            LPWORD lpwResult);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IgnoreAll )( 
            IVSpell __RPC_FAR * This,
            LPSTR lpszIgnoreThis,
            LPWORD lpwResult);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *LoadUserDictionary )( 
            IVSpell __RPC_FAR * This,
            LPSTR lpszDicPath,
            LPWORD lpwResult);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *UnloadUserDictionary )( 
            IVSpell __RPC_FAR * This,
            LPSTR lpszDicPath);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AddToUserDictionary )( 
            IVSpell __RPC_FAR * This,
            LPSTR lpszDicPath,
            LPSTR lpszAddThis,
            LPWORD lpwResult);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AddUnsupportedLocale )( 
            IVSpell __RPC_FAR * This,
            LCID lcLocale);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IsLocaleSupported )( 
            IVSpell __RPC_FAR * This,
            LCID lcLocale,
            BOOL __RPC_FAR *lpbSupported);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ChangeLocale )( 
            IVSpell __RPC_FAR * This,
            LCID lcLocale,
            LPSTR lpszDicPath);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetLocale )( 
            IVSpell __RPC_FAR * This,
            LCID __RPC_FAR *lplcLocale);
        
        END_INTERFACE
    } IVSpellVtbl;

    interface IVSpell
    {
        CONST_VTBL struct IVSpellVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IVSpell_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IVSpell_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IVSpell_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IVSpell_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IVSpell_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IVSpell_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IVSpell_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IVSpell_Init(This,bIgnoreNum,lcLocale,lpszEnginePath,lpszDicPath)	\
    (This)->lpVtbl -> Init(This,bIgnoreNum,lcLocale,lpszEnginePath,lpszDicPath)

#define IVSpell_Spell(This,lpszWord,lpwResult)	\
    (This)->lpVtbl -> Spell(This,lpszWord,lpwResult)

#define IVSpell_Suggest(This,lpszWord,lpszSuggestions,lpdwNumSuggestions,dwSuggestionBufLen,lpwResult)	\
    (This)->lpVtbl -> Suggest(This,lpszWord,lpszSuggestions,lpdwNumSuggestions,dwSuggestionBufLen,lpwResult)

#define IVSpell_IgnoreAll(This,lpszIgnoreThis,lpwResult)	\
    (This)->lpVtbl -> IgnoreAll(This,lpszIgnoreThis,lpwResult)

#define IVSpell_LoadUserDictionary(This,lpszDicPath,lpwResult)	\
    (This)->lpVtbl -> LoadUserDictionary(This,lpszDicPath,lpwResult)

#define IVSpell_UnloadUserDictionary(This,lpszDicPath)	\
    (This)->lpVtbl -> UnloadUserDictionary(This,lpszDicPath)

#define IVSpell_AddToUserDictionary(This,lpszDicPath,lpszAddThis,lpwResult)	\
    (This)->lpVtbl -> AddToUserDictionary(This,lpszDicPath,lpszAddThis,lpwResult)

#define IVSpell_AddUnsupportedLocale(This,lcLocale)	\
    (This)->lpVtbl -> AddUnsupportedLocale(This,lcLocale)

#define IVSpell_IsLocaleSupported(This,lcLocale,lpbSupported)	\
    (This)->lpVtbl -> IsLocaleSupported(This,lcLocale,lpbSupported)

#define IVSpell_ChangeLocale(This,lcLocale,lpszDicPath)	\
    (This)->lpVtbl -> ChangeLocale(This,lcLocale,lpszDicPath)

#define IVSpell_GetLocale(This,lplcLocale)	\
    (This)->lpVtbl -> GetLocale(This,lplcLocale)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IVSpell_Init_Proxy( 
    IVSpell __RPC_FAR * This,
    BOOL bIgnoreNum,
    LCID lcLocale,
    LPSTR lpszEnginePath,
    LPSTR lpszDicPath);


void __RPC_STUB IVSpell_Init_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IVSpell_Spell_Proxy( 
    IVSpell __RPC_FAR * This,
    LPSTR lpszWord,
    LPWORD lpwResult);


void __RPC_STUB IVSpell_Spell_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IVSpell_Suggest_Proxy( 
    IVSpell __RPC_FAR * This,
    LPSTR lpszWord,
    LPSTR lpszSuggestions,
    LPDWORD lpdwNumSuggestions,
    DWORD dwSuggestionBufLen,
    LPWORD lpwResult);


void __RPC_STUB IVSpell_Suggest_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IVSpell_IgnoreAll_Proxy( 
    IVSpell __RPC_FAR * This,
    LPSTR lpszIgnoreThis,
    LPWORD lpwResult);


void __RPC_STUB IVSpell_IgnoreAll_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IVSpell_LoadUserDictionary_Proxy( 
    IVSpell __RPC_FAR * This,
    LPSTR lpszDicPath,
    LPWORD lpwResult);


void __RPC_STUB IVSpell_LoadUserDictionary_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IVSpell_UnloadUserDictionary_Proxy( 
    IVSpell __RPC_FAR * This,
    LPSTR lpszDicPath);


void __RPC_STUB IVSpell_UnloadUserDictionary_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IVSpell_AddToUserDictionary_Proxy( 
    IVSpell __RPC_FAR * This,
    LPSTR lpszDicPath,
    LPSTR lpszAddThis,
    LPWORD lpwResult);


void __RPC_STUB IVSpell_AddToUserDictionary_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IVSpell_AddUnsupportedLocale_Proxy( 
    IVSpell __RPC_FAR * This,
    LCID lcLocale);


void __RPC_STUB IVSpell_AddUnsupportedLocale_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IVSpell_IsLocaleSupported_Proxy( 
    IVSpell __RPC_FAR * This,
    LCID lcLocale,
    BOOL __RPC_FAR *lpbSupported);


void __RPC_STUB IVSpell_IsLocaleSupported_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IVSpell_ChangeLocale_Proxy( 
    IVSpell __RPC_FAR * This,
    LCID lcLocale,
    LPSTR lpszDicPath);


void __RPC_STUB IVSpell_ChangeLocale_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IVSpell_GetLocale_Proxy( 
    IVSpell __RPC_FAR * This,
    LCID __RPC_FAR *lplcLocale);


void __RPC_STUB IVSpell_GetLocale_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IVSpell_INTERFACE_DEFINED__ */



#ifndef __VSPELLLib_LIBRARY_DEFINED__
#define __VSPELLLib_LIBRARY_DEFINED__

/****************************************
 * Generated header for library: VSPELLLib
 * at Tue May 06 17:19:35 1997
 * using MIDL 3.01.75
 ****************************************/
/* [helpstring][version][uuid] */ 



EXTERN_C const IID LIBID_VSPELLLib;

#ifdef __cplusplus
EXTERN_C const CLSID CLSID_CICSpell;

class DECLSPEC_UUID("26E39D51-C628-11D0-9067-00609764089F")
CICSpell;
#endif

#ifdef __cplusplus
EXTERN_C const CLSID CLSID_CCSAPISpell;

class DECLSPEC_UUID("26E39D54-C628-11D0-9067-00609764089F")
CCSAPISpell;
#endif
#endif /* __VSPELLLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
