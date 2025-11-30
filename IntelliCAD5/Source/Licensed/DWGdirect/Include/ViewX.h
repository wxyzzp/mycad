

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0361 */
/* at Tue Aug 24 16:28:35 2004
 */
/* Compiler settings for \OpenDWG\dd1.12\ViewX\ViewX.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
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

#ifndef __ViewX_h__
#define __ViewX_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IOdaView_FWD_DEFINED__
#define __IOdaView_FWD_DEFINED__
typedef interface IOdaView IOdaView;
#endif 	/* __IOdaView_FWD_DEFINED__ */


#ifndef __IOdaDevice_FWD_DEFINED__
#define __IOdaDevice_FWD_DEFINED__
typedef interface IOdaDevice IOdaDevice;
#endif 	/* __IOdaDevice_FWD_DEFINED__ */


#ifndef __IOdaDeviceManager_FWD_DEFINED__
#define __IOdaDeviceManager_FWD_DEFINED__
typedef interface IOdaDeviceManager IOdaDeviceManager;
#endif 	/* __IOdaDeviceManager_FWD_DEFINED__ */


#ifndef __IOdaViewer_FWD_DEFINED__
#define __IOdaViewer_FWD_DEFINED__
typedef interface IOdaViewer IOdaViewer;
#endif 	/* __IOdaViewer_FWD_DEFINED__ */


#ifndef __OdaViewer_FWD_DEFINED__
#define __OdaViewer_FWD_DEFINED__

#ifdef __cplusplus
typedef class OdaViewer OdaViewer;
#else
typedef struct OdaViewer OdaViewer;
#endif /* __cplusplus */

#endif 	/* __OdaViewer_FWD_DEFINED__ */


#ifndef __OdaDevice_FWD_DEFINED__
#define __OdaDevice_FWD_DEFINED__

#ifdef __cplusplus
typedef class OdaDevice OdaDevice;
#else
typedef struct OdaDevice OdaDevice;
#endif /* __cplusplus */

#endif 	/* __OdaDevice_FWD_DEFINED__ */


#ifndef __OdaView_FWD_DEFINED__
#define __OdaView_FWD_DEFINED__

#ifdef __cplusplus
typedef class OdaView OdaView;
#else
typedef struct OdaView OdaView;
#endif /* __cplusplus */

#endif 	/* __OdaView_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"
#include "odax.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

/* interface __MIDL_itf_ViewX_0000 */
/* [local] */ 

typedef /* [uuid] */  DECLSPEC_UUID("EF1CDECA-115E-4f9c-AB0B-B670D2817161") 
enum Projection
    {	kParallel	= 0,
	kPerspective	= 1
    } 	OdaProjection;



extern RPC_IF_HANDLE __MIDL_itf_ViewX_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_ViewX_0000_v0_0_s_ifspec;

#ifndef __IOdaView_INTERFACE_DEFINED__
#define __IOdaView_INTERFACE_DEFINED__

/* interface IOdaView */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IOdaView;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("DBCBA22B-AA6A-403f-BEA4-ACE7F6C0A799")
    IOdaView : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_LensLength( 
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_LensLength( 
            /* [in] */ double newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ViewportBorderVisible( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ViewportBorderVisible( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Viewport( 
            /* [retval][out] */ VARIANT *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Viewport( 
            /* [in] */ VARIANT newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_IsPerspective( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Update( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Zoom( 
            double Factor) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Pan( 
            double X,
            double Y) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Orbit( 
            double X,
            double Y) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Roll( 
            double Angle) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Dolly( 
            double X,
            double Y,
            double Z) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ZoomExtents( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ZoomAll( void) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ViewportBorderWeight( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ViewportBorderWeight( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ViewportBorderColor( 
            /* [retval][out] */ VARIANT *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ViewportBorderColor( 
            /* [in] */ VARIANT newVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IOdaViewVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IOdaView * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IOdaView * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IOdaView * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IOdaView * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IOdaView * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IOdaView * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IOdaView * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_LensLength )( 
            IOdaView * This,
            /* [retval][out] */ double *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_LensLength )( 
            IOdaView * This,
            /* [in] */ double newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ViewportBorderVisible )( 
            IOdaView * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ViewportBorderVisible )( 
            IOdaView * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Viewport )( 
            IOdaView * This,
            /* [retval][out] */ VARIANT *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Viewport )( 
            IOdaView * This,
            /* [in] */ VARIANT newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsPerspective )( 
            IOdaView * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Update )( 
            IOdaView * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Zoom )( 
            IOdaView * This,
            double Factor);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Pan )( 
            IOdaView * This,
            double X,
            double Y);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Orbit )( 
            IOdaView * This,
            double X,
            double Y);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Roll )( 
            IOdaView * This,
            double Angle);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Dolly )( 
            IOdaView * This,
            double X,
            double Y,
            double Z);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ZoomExtents )( 
            IOdaView * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ZoomAll )( 
            IOdaView * This);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ViewportBorderWeight )( 
            IOdaView * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ViewportBorderWeight )( 
            IOdaView * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ViewportBorderColor )( 
            IOdaView * This,
            /* [retval][out] */ VARIANT *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ViewportBorderColor )( 
            IOdaView * This,
            /* [in] */ VARIANT newVal);
        
        END_INTERFACE
    } IOdaViewVtbl;

    interface IOdaView
    {
        CONST_VTBL struct IOdaViewVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOdaView_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IOdaView_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IOdaView_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IOdaView_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IOdaView_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IOdaView_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IOdaView_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IOdaView_get_LensLength(This,pVal)	\
    (This)->lpVtbl -> get_LensLength(This,pVal)

#define IOdaView_put_LensLength(This,newVal)	\
    (This)->lpVtbl -> put_LensLength(This,newVal)

#define IOdaView_get_ViewportBorderVisible(This,pVal)	\
    (This)->lpVtbl -> get_ViewportBorderVisible(This,pVal)

#define IOdaView_put_ViewportBorderVisible(This,newVal)	\
    (This)->lpVtbl -> put_ViewportBorderVisible(This,newVal)

#define IOdaView_get_Viewport(This,pVal)	\
    (This)->lpVtbl -> get_Viewport(This,pVal)

#define IOdaView_put_Viewport(This,newVal)	\
    (This)->lpVtbl -> put_Viewport(This,newVal)

#define IOdaView_get_IsPerspective(This,pVal)	\
    (This)->lpVtbl -> get_IsPerspective(This,pVal)

#define IOdaView_Update(This)	\
    (This)->lpVtbl -> Update(This)

#define IOdaView_Zoom(This,Factor)	\
    (This)->lpVtbl -> Zoom(This,Factor)

#define IOdaView_Pan(This,X,Y)	\
    (This)->lpVtbl -> Pan(This,X,Y)

#define IOdaView_Orbit(This,X,Y)	\
    (This)->lpVtbl -> Orbit(This,X,Y)

#define IOdaView_Roll(This,Angle)	\
    (This)->lpVtbl -> Roll(This,Angle)

#define IOdaView_Dolly(This,X,Y,Z)	\
    (This)->lpVtbl -> Dolly(This,X,Y,Z)

#define IOdaView_ZoomExtents(This)	\
    (This)->lpVtbl -> ZoomExtents(This)

#define IOdaView_ZoomAll(This)	\
    (This)->lpVtbl -> ZoomAll(This)

#define IOdaView_get_ViewportBorderWeight(This,pVal)	\
    (This)->lpVtbl -> get_ViewportBorderWeight(This,pVal)

#define IOdaView_put_ViewportBorderWeight(This,newVal)	\
    (This)->lpVtbl -> put_ViewportBorderWeight(This,newVal)

#define IOdaView_get_ViewportBorderColor(This,pVal)	\
    (This)->lpVtbl -> get_ViewportBorderColor(This,pVal)

#define IOdaView_put_ViewportBorderColor(This,newVal)	\
    (This)->lpVtbl -> put_ViewportBorderColor(This,newVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaView_get_LensLength_Proxy( 
    IOdaView * This,
    /* [retval][out] */ double *pVal);


void __RPC_STUB IOdaView_get_LensLength_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOdaView_put_LensLength_Proxy( 
    IOdaView * This,
    /* [in] */ double newVal);


void __RPC_STUB IOdaView_put_LensLength_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaView_get_ViewportBorderVisible_Proxy( 
    IOdaView * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB IOdaView_get_ViewportBorderVisible_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOdaView_put_ViewportBorderVisible_Proxy( 
    IOdaView * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB IOdaView_put_ViewportBorderVisible_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaView_get_Viewport_Proxy( 
    IOdaView * This,
    /* [retval][out] */ VARIANT *pVal);


void __RPC_STUB IOdaView_get_Viewport_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOdaView_put_Viewport_Proxy( 
    IOdaView * This,
    /* [in] */ VARIANT newVal);


void __RPC_STUB IOdaView_put_Viewport_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaView_get_IsPerspective_Proxy( 
    IOdaView * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB IOdaView_get_IsPerspective_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IOdaView_Update_Proxy( 
    IOdaView * This);


void __RPC_STUB IOdaView_Update_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IOdaView_Zoom_Proxy( 
    IOdaView * This,
    double Factor);


void __RPC_STUB IOdaView_Zoom_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IOdaView_Pan_Proxy( 
    IOdaView * This,
    double X,
    double Y);


void __RPC_STUB IOdaView_Pan_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IOdaView_Orbit_Proxy( 
    IOdaView * This,
    double X,
    double Y);


void __RPC_STUB IOdaView_Orbit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IOdaView_Roll_Proxy( 
    IOdaView * This,
    double Angle);


void __RPC_STUB IOdaView_Roll_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IOdaView_Dolly_Proxy( 
    IOdaView * This,
    double X,
    double Y,
    double Z);


void __RPC_STUB IOdaView_Dolly_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IOdaView_ZoomExtents_Proxy( 
    IOdaView * This);


void __RPC_STUB IOdaView_ZoomExtents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IOdaView_ZoomAll_Proxy( 
    IOdaView * This);


void __RPC_STUB IOdaView_ZoomAll_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaView_get_ViewportBorderWeight_Proxy( 
    IOdaView * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB IOdaView_get_ViewportBorderWeight_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOdaView_put_ViewportBorderWeight_Proxy( 
    IOdaView * This,
    /* [in] */ long newVal);


void __RPC_STUB IOdaView_put_ViewportBorderWeight_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaView_get_ViewportBorderColor_Proxy( 
    IOdaView * This,
    /* [retval][out] */ VARIANT *pVal);


void __RPC_STUB IOdaView_get_ViewportBorderColor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOdaView_put_ViewportBorderColor_Proxy( 
    IOdaView * This,
    /* [in] */ VARIANT newVal);


void __RPC_STUB IOdaView_put_ViewportBorderColor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IOdaView_INTERFACE_DEFINED__ */


#ifndef __IOdaDevice_INTERFACE_DEFINED__
#define __IOdaDevice_INTERFACE_DEFINED__

/* interface IOdaDevice */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IOdaDevice;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E8170D5B-2A69-46af-B6B7-3DEDF9EC425E")
    IOdaDevice : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Item( 
            /* [in] */ VARIANT index,
            /* [retval][out] */ IOdaView **pView) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ViewCount( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetupActiveLayoutViews( 
            /* [in] */ IAcadDatabase *pBase,
            /* [retval][out] */ IOdaView **pView) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetupLayoutViews( 
            /* [in] */ IAcadLayout *pLayout,
            /* [retval][out] */ IOdaView **pView) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Update( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Close( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OnSize( 
            long Width,
            long Height) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_BackgroundColor( 
            /* [retval][out] */ VARIANT *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_BackgroundColor( 
            /* [in] */ VARIANT newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ActiveView( 
            /* [retval][out] */ IOdaView **pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IOdaDeviceVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IOdaDevice * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IOdaDevice * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IOdaDevice * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IOdaDevice * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IOdaDevice * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IOdaDevice * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IOdaDevice * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Item )( 
            IOdaDevice * This,
            /* [in] */ VARIANT index,
            /* [retval][out] */ IOdaView **pView);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ViewCount )( 
            IOdaDevice * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetupActiveLayoutViews )( 
            IOdaDevice * This,
            /* [in] */ IAcadDatabase *pBase,
            /* [retval][out] */ IOdaView **pView);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetupLayoutViews )( 
            IOdaDevice * This,
            /* [in] */ IAcadLayout *pLayout,
            /* [retval][out] */ IOdaView **pView);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Update )( 
            IOdaDevice * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Close )( 
            IOdaDevice * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OnSize )( 
            IOdaDevice * This,
            long Width,
            long Height);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_BackgroundColor )( 
            IOdaDevice * This,
            /* [retval][out] */ VARIANT *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_BackgroundColor )( 
            IOdaDevice * This,
            /* [in] */ VARIANT newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ActiveView )( 
            IOdaDevice * This,
            /* [retval][out] */ IOdaView **pVal);
        
        END_INTERFACE
    } IOdaDeviceVtbl;

    interface IOdaDevice
    {
        CONST_VTBL struct IOdaDeviceVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOdaDevice_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IOdaDevice_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IOdaDevice_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IOdaDevice_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IOdaDevice_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IOdaDevice_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IOdaDevice_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IOdaDevice_Item(This,index,pView)	\
    (This)->lpVtbl -> Item(This,index,pView)

#define IOdaDevice_get_ViewCount(This,pVal)	\
    (This)->lpVtbl -> get_ViewCount(This,pVal)

#define IOdaDevice_SetupActiveLayoutViews(This,pBase,pView)	\
    (This)->lpVtbl -> SetupActiveLayoutViews(This,pBase,pView)

#define IOdaDevice_SetupLayoutViews(This,pLayout,pView)	\
    (This)->lpVtbl -> SetupLayoutViews(This,pLayout,pView)

#define IOdaDevice_Update(This)	\
    (This)->lpVtbl -> Update(This)

#define IOdaDevice_Close(This)	\
    (This)->lpVtbl -> Close(This)

#define IOdaDevice_OnSize(This,Width,Height)	\
    (This)->lpVtbl -> OnSize(This,Width,Height)

#define IOdaDevice_get_BackgroundColor(This,pVal)	\
    (This)->lpVtbl -> get_BackgroundColor(This,pVal)

#define IOdaDevice_put_BackgroundColor(This,newVal)	\
    (This)->lpVtbl -> put_BackgroundColor(This,newVal)

#define IOdaDevice_get_ActiveView(This,pVal)	\
    (This)->lpVtbl -> get_ActiveView(This,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IOdaDevice_Item_Proxy( 
    IOdaDevice * This,
    /* [in] */ VARIANT index,
    /* [retval][out] */ IOdaView **pView);


void __RPC_STUB IOdaDevice_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaDevice_get_ViewCount_Proxy( 
    IOdaDevice * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB IOdaDevice_get_ViewCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IOdaDevice_SetupActiveLayoutViews_Proxy( 
    IOdaDevice * This,
    /* [in] */ IAcadDatabase *pBase,
    /* [retval][out] */ IOdaView **pView);


void __RPC_STUB IOdaDevice_SetupActiveLayoutViews_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IOdaDevice_SetupLayoutViews_Proxy( 
    IOdaDevice * This,
    /* [in] */ IAcadLayout *pLayout,
    /* [retval][out] */ IOdaView **pView);


void __RPC_STUB IOdaDevice_SetupLayoutViews_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IOdaDevice_Update_Proxy( 
    IOdaDevice * This);


void __RPC_STUB IOdaDevice_Update_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IOdaDevice_Close_Proxy( 
    IOdaDevice * This);


void __RPC_STUB IOdaDevice_Close_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IOdaDevice_OnSize_Proxy( 
    IOdaDevice * This,
    long Width,
    long Height);


void __RPC_STUB IOdaDevice_OnSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaDevice_get_BackgroundColor_Proxy( 
    IOdaDevice * This,
    /* [retval][out] */ VARIANT *pVal);


void __RPC_STUB IOdaDevice_get_BackgroundColor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOdaDevice_put_BackgroundColor_Proxy( 
    IOdaDevice * This,
    /* [in] */ VARIANT newVal);


void __RPC_STUB IOdaDevice_put_BackgroundColor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaDevice_get_ActiveView_Proxy( 
    IOdaDevice * This,
    /* [retval][out] */ IOdaView **pVal);


void __RPC_STUB IOdaDevice_get_ActiveView_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IOdaDevice_INTERFACE_DEFINED__ */


#ifndef __IOdaDeviceManager_INTERFACE_DEFINED__
#define __IOdaDeviceManager_INTERFACE_DEFINED__

/* interface IOdaDeviceManager */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IOdaDeviceManager;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E045EAD6-226D-4296-8BA3-07D482279B53")
    IOdaDeviceManager : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Item( 
            /* [in] */ VARIANT Index,
            /* [retval][out] */ IOdaDevice **pDev) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE NewDevice( 
            /* [in] */ VARIANT DevicePath,
            /* [retval][out] */ IOdaDevice **pDev) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IOdaDeviceManagerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IOdaDeviceManager * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IOdaDeviceManager * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IOdaDeviceManager * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IOdaDeviceManager * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IOdaDeviceManager * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IOdaDeviceManager * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IOdaDeviceManager * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Item )( 
            IOdaDeviceManager * This,
            /* [in] */ VARIANT Index,
            /* [retval][out] */ IOdaDevice **pDev);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            IOdaDeviceManager * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *NewDevice )( 
            IOdaDeviceManager * This,
            /* [in] */ VARIANT DevicePath,
            /* [retval][out] */ IOdaDevice **pDev);
        
        END_INTERFACE
    } IOdaDeviceManagerVtbl;

    interface IOdaDeviceManager
    {
        CONST_VTBL struct IOdaDeviceManagerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOdaDeviceManager_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IOdaDeviceManager_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IOdaDeviceManager_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IOdaDeviceManager_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IOdaDeviceManager_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IOdaDeviceManager_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IOdaDeviceManager_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IOdaDeviceManager_Item(This,Index,pDev)	\
    (This)->lpVtbl -> Item(This,Index,pDev)

#define IOdaDeviceManager_get_Count(This,pVal)	\
    (This)->lpVtbl -> get_Count(This,pVal)

#define IOdaDeviceManager_NewDevice(This,DevicePath,pDev)	\
    (This)->lpVtbl -> NewDevice(This,DevicePath,pDev)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IOdaDeviceManager_Item_Proxy( 
    IOdaDeviceManager * This,
    /* [in] */ VARIANT Index,
    /* [retval][out] */ IOdaDevice **pDev);


void __RPC_STUB IOdaDeviceManager_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaDeviceManager_get_Count_Proxy( 
    IOdaDeviceManager * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB IOdaDeviceManager_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IOdaDeviceManager_NewDevice_Proxy( 
    IOdaDeviceManager * This,
    /* [in] */ VARIANT DevicePath,
    /* [retval][out] */ IOdaDevice **pDev);


void __RPC_STUB IOdaDeviceManager_NewDevice_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IOdaDeviceManager_INTERFACE_DEFINED__ */


#ifndef __IOdaViewer_INTERFACE_DEFINED__
#define __IOdaViewer_INTERFACE_DEFINED__

/* interface IOdaViewer */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IOdaViewer;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("8477311A-1357-462F-8B8A-81E022B987C2")
    IOdaViewer : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DeviceManager( 
            /* [retval][out] */ IOdaDeviceManager **pDevices) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IOdaViewerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IOdaViewer * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IOdaViewer * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IOdaViewer * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IOdaViewer * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IOdaViewer * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IOdaViewer * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IOdaViewer * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DeviceManager )( 
            IOdaViewer * This,
            /* [retval][out] */ IOdaDeviceManager **pDevices);
        
        END_INTERFACE
    } IOdaViewerVtbl;

    interface IOdaViewer
    {
        CONST_VTBL struct IOdaViewerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOdaViewer_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IOdaViewer_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IOdaViewer_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IOdaViewer_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IOdaViewer_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IOdaViewer_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IOdaViewer_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IOdaViewer_DeviceManager(This,pDevices)	\
    (This)->lpVtbl -> DeviceManager(This,pDevices)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IOdaViewer_DeviceManager_Proxy( 
    IOdaViewer * This,
    /* [retval][out] */ IOdaDeviceManager **pDevices);


void __RPC_STUB IOdaViewer_DeviceManager_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IOdaViewer_INTERFACE_DEFINED__ */



#ifndef __VIEWXLib_LIBRARY_DEFINED__
#define __VIEWXLib_LIBRARY_DEFINED__

/* library VIEWXLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_VIEWXLib;

EXTERN_C const CLSID CLSID_OdaViewer;

#ifdef __cplusplus

class DECLSPEC_UUID("53840EC9-B5E8-42DB-A6A7-E9B7BED3CAB3")
OdaViewer;
#endif

EXTERN_C const CLSID CLSID_OdaDevice;

#ifdef __cplusplus

class DECLSPEC_UUID("CF8C6149-F86B-4ab0-970F-40D0CCE11918")
OdaDevice;
#endif

EXTERN_C const CLSID CLSID_OdaView;

#ifdef __cplusplus

class DECLSPEC_UUID("013F2FA5-A719-470A-B93D-A2AB7FFED68E")
OdaView;
#endif
#endif /* __VIEWXLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  VARIANT_UserSize(     unsigned long *, unsigned long            , VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserMarshal(  unsigned long *, unsigned char *, VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserUnmarshal(unsigned long *, unsigned char *, VARIANT * ); 
void                      __RPC_USER  VARIANT_UserFree(     unsigned long *, VARIANT * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif



