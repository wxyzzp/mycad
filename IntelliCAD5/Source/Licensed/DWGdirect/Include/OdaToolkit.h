

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0361 */
/* at Tue Aug 24 16:27:37 2004
 */
/* Compiler settings for \OpenDWG\dd1.12\odatoolkit\OdaToolkit.idl:
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

#ifndef __OdaToolkit_h__
#define __OdaToolkit_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IOdaSvgExporter_FWD_DEFINED__
#define __IOdaSvgExporter_FWD_DEFINED__
typedef interface IOdaSvgExporter IOdaSvgExporter;
#endif 	/* __IOdaSvgExporter_FWD_DEFINED__ */


#ifndef __IAcadApplication_FWD_DEFINED__
#define __IAcadApplication_FWD_DEFINED__
typedef interface IAcadApplication IAcadApplication;
#endif 	/* __IAcadApplication_FWD_DEFINED__ */


#ifndef __IAcadUtility_FWD_DEFINED__
#define __IAcadUtility_FWD_DEFINED__
typedef interface IAcadUtility IAcadUtility;
#endif 	/* __IAcadUtility_FWD_DEFINED__ */


#ifndef __IAcadDocument_FWD_DEFINED__
#define __IAcadDocument_FWD_DEFINED__
typedef interface IAcadDocument IAcadDocument;
#endif 	/* __IAcadDocument_FWD_DEFINED__ */


#ifndef __IAcadDocuments_FWD_DEFINED__
#define __IAcadDocuments_FWD_DEFINED__
typedef interface IAcadDocuments IAcadDocuments;
#endif 	/* __IAcadDocuments_FWD_DEFINED__ */


#ifndef __IAcadSelectionSets_FWD_DEFINED__
#define __IAcadSelectionSets_FWD_DEFINED__
typedef interface IAcadSelectionSets IAcadSelectionSets;
#endif 	/* __IAcadSelectionSets_FWD_DEFINED__ */


#ifndef __IAcadSelectionSet_FWD_DEFINED__
#define __IAcadSelectionSet_FWD_DEFINED__
typedef interface IAcadSelectionSet IAcadSelectionSet;
#endif 	/* __IAcadSelectionSet_FWD_DEFINED__ */


#ifndef __IAcadPreferences_FWD_DEFINED__
#define __IAcadPreferences_FWD_DEFINED__
typedef interface IAcadPreferences IAcadPreferences;
#endif 	/* __IAcadPreferences_FWD_DEFINED__ */


#ifndef __IAcadPreferencesFiles_FWD_DEFINED__
#define __IAcadPreferencesFiles_FWD_DEFINED__
typedef interface IAcadPreferencesFiles IAcadPreferencesFiles;
#endif 	/* __IAcadPreferencesFiles_FWD_DEFINED__ */


#ifndef __IAcadLayerStateManager_FWD_DEFINED__
#define __IAcadLayerStateManager_FWD_DEFINED__
typedef interface IAcadLayerStateManager IAcadLayerStateManager;
#endif 	/* __IAcadLayerStateManager_FWD_DEFINED__ */


#ifndef __IAcadSecurityParams_FWD_DEFINED__
#define __IAcadSecurityParams_FWD_DEFINED__
typedef interface IAcadSecurityParams IAcadSecurityParams;
#endif 	/* __IAcadSecurityParams_FWD_DEFINED__ */


#ifndef __IOdaAuditInfo_FWD_DEFINED__
#define __IOdaAuditInfo_FWD_DEFINED__
typedef interface IOdaAuditInfo IOdaAuditInfo;
#endif 	/* __IOdaAuditInfo_FWD_DEFINED__ */


#ifndef ___DAcadApplicationEvents_FWD_DEFINED__
#define ___DAcadApplicationEvents_FWD_DEFINED__
typedef interface _DAcadApplicationEvents _DAcadApplicationEvents;
#endif 	/* ___DAcadApplicationEvents_FWD_DEFINED__ */


#ifndef ___DAcadDocumentEvents_FWD_DEFINED__
#define ___DAcadDocumentEvents_FWD_DEFINED__
typedef interface _DAcadDocumentEvents _DAcadDocumentEvents;
#endif 	/* ___DAcadDocumentEvents_FWD_DEFINED__ */


#ifndef __IOdaHostApp_FWD_DEFINED__
#define __IOdaHostApp_FWD_DEFINED__
typedef interface IOdaHostApp IOdaHostApp;
#endif 	/* __IOdaHostApp_FWD_DEFINED__ */


#ifndef __IOdaDwfPageData_FWD_DEFINED__
#define __IOdaDwfPageData_FWD_DEFINED__
typedef interface IOdaDwfPageData IOdaDwfPageData;
#endif 	/* __IOdaDwfPageData_FWD_DEFINED__ */


#ifndef __IOdaDwfTemplate_FWD_DEFINED__
#define __IOdaDwfTemplate_FWD_DEFINED__
typedef interface IOdaDwfTemplate IOdaDwfTemplate;
#endif 	/* __IOdaDwfTemplate_FWD_DEFINED__ */


#ifndef __IOdaDwfImporter_FWD_DEFINED__
#define __IOdaDwfImporter_FWD_DEFINED__
typedef interface IOdaDwfImporter IOdaDwfImporter;
#endif 	/* __IOdaDwfImporter_FWD_DEFINED__ */


#ifndef __AcadApplication_FWD_DEFINED__
#define __AcadApplication_FWD_DEFINED__

#ifdef __cplusplus
typedef class AcadApplication AcadApplication;
#else
typedef struct AcadApplication AcadApplication;
#endif /* __cplusplus */

#endif 	/* __AcadApplication_FWD_DEFINED__ */


#ifndef __AcadDocument_FWD_DEFINED__
#define __AcadDocument_FWD_DEFINED__

#ifdef __cplusplus
typedef class AcadDocument AcadDocument;
#else
typedef struct AcadDocument AcadDocument;
#endif /* __cplusplus */

#endif 	/* __AcadDocument_FWD_DEFINED__ */


#ifndef ___IOdaHostAppEvents_FWD_DEFINED__
#define ___IOdaHostAppEvents_FWD_DEFINED__
typedef interface _IOdaHostAppEvents _IOdaHostAppEvents;
#endif 	/* ___IOdaHostAppEvents_FWD_DEFINED__ */


#ifndef __OdaHostApp_FWD_DEFINED__
#define __OdaHostApp_FWD_DEFINED__

#ifdef __cplusplus
typedef class OdaHostApp OdaHostApp;
#else
typedef struct OdaHostApp OdaHostApp;
#endif /* __cplusplus */

#endif 	/* __OdaHostApp_FWD_DEFINED__ */


#ifndef __AcadUtility_FWD_DEFINED__
#define __AcadUtility_FWD_DEFINED__

#ifdef __cplusplus
typedef class AcadUtility AcadUtility;
#else
typedef struct AcadUtility AcadUtility;
#endif /* __cplusplus */

#endif 	/* __AcadUtility_FWD_DEFINED__ */


#ifndef __AcadPreferences_FWD_DEFINED__
#define __AcadPreferences_FWD_DEFINED__

#ifdef __cplusplus
typedef class AcadPreferences AcadPreferences;
#else
typedef struct AcadPreferences AcadPreferences;
#endif /* __cplusplus */

#endif 	/* __AcadPreferences_FWD_DEFINED__ */


#ifndef __AcadSelectionSet_FWD_DEFINED__
#define __AcadSelectionSet_FWD_DEFINED__

#ifdef __cplusplus
typedef class AcadSelectionSet AcadSelectionSet;
#else
typedef struct AcadSelectionSet AcadSelectionSet;
#endif /* __cplusplus */

#endif 	/* __AcadSelectionSet_FWD_DEFINED__ */


#ifndef __OdaDwfTemplate_FWD_DEFINED__
#define __OdaDwfTemplate_FWD_DEFINED__

#ifdef __cplusplus
typedef class OdaDwfTemplate OdaDwfTemplate;
#else
typedef struct OdaDwfTemplate OdaDwfTemplate;
#endif /* __cplusplus */

#endif 	/* __OdaDwfTemplate_FWD_DEFINED__ */


#ifndef __OdaDwfImporter_FWD_DEFINED__
#define __OdaDwfImporter_FWD_DEFINED__

#ifdef __cplusplus
typedef class OdaDwfImporter OdaDwfImporter;
#else
typedef struct OdaDwfImporter OdaDwfImporter;
#endif /* __cplusplus */

#endif 	/* __OdaDwfImporter_FWD_DEFINED__ */


#ifndef __AcadLayerStateManager_FWD_DEFINED__
#define __AcadLayerStateManager_FWD_DEFINED__

#ifdef __cplusplus
typedef class AcadLayerStateManager AcadLayerStateManager;
#else
typedef struct AcadLayerStateManager AcadLayerStateManager;
#endif /* __cplusplus */

#endif 	/* __AcadLayerStateManager_FWD_DEFINED__ */


#ifndef __AcadSecurityParams_FWD_DEFINED__
#define __AcadSecurityParams_FWD_DEFINED__

#ifdef __cplusplus
typedef class AcadSecurityParams AcadSecurityParams;
#else
typedef struct AcadSecurityParams AcadSecurityParams;
#endif /* __cplusplus */

#endif 	/* __AcadSecurityParams_FWD_DEFINED__ */


#ifndef __OdaSvgExporter_FWD_DEFINED__
#define __OdaSvgExporter_FWD_DEFINED__

#ifdef __cplusplus
typedef class OdaSvgExporter OdaSvgExporter;
#else
typedef struct OdaSvgExporter OdaSvgExporter;
#endif /* __cplusplus */

#endif 	/* __OdaSvgExporter_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"
#include "odax.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __IOdaSvgExporter_INTERFACE_DEFINED__
#define __IOdaSvgExporter_INTERFACE_DEFINED__

/* interface IOdaSvgExporter */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IOdaSvgExporter;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("FDAF1115-5A20-4D16-A509-01012B71E4F7")
    IOdaSvgExporter : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Properties( 
            /* [in] */ BSTR Name,
            /* [retval][out] */ VARIANT *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Properties( 
            /* [in] */ BSTR Name,
            /* [in] */ VARIANT NewVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Export( 
            /* [in] */ IAcadDatabase *Database,
            /* [in] */ BSTR FilePath) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IOdaSvgExporterVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IOdaSvgExporter * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IOdaSvgExporter * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IOdaSvgExporter * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IOdaSvgExporter * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IOdaSvgExporter * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IOdaSvgExporter * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IOdaSvgExporter * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Properties )( 
            IOdaSvgExporter * This,
            /* [in] */ BSTR Name,
            /* [retval][out] */ VARIANT *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Properties )( 
            IOdaSvgExporter * This,
            /* [in] */ BSTR Name,
            /* [in] */ VARIANT NewVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Export )( 
            IOdaSvgExporter * This,
            /* [in] */ IAcadDatabase *Database,
            /* [in] */ BSTR FilePath);
        
        END_INTERFACE
    } IOdaSvgExporterVtbl;

    interface IOdaSvgExporter
    {
        CONST_VTBL struct IOdaSvgExporterVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOdaSvgExporter_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IOdaSvgExporter_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IOdaSvgExporter_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IOdaSvgExporter_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IOdaSvgExporter_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IOdaSvgExporter_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IOdaSvgExporter_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IOdaSvgExporter_get_Properties(This,Name,pVal)	\
    (This)->lpVtbl -> get_Properties(This,Name,pVal)

#define IOdaSvgExporter_put_Properties(This,Name,NewVal)	\
    (This)->lpVtbl -> put_Properties(This,Name,NewVal)

#define IOdaSvgExporter_Export(This,Database,FilePath)	\
    (This)->lpVtbl -> Export(This,Database,FilePath)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaSvgExporter_get_Properties_Proxy( 
    IOdaSvgExporter * This,
    /* [in] */ BSTR Name,
    /* [retval][out] */ VARIANT *pVal);


void __RPC_STUB IOdaSvgExporter_get_Properties_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOdaSvgExporter_put_Properties_Proxy( 
    IOdaSvgExporter * This,
    /* [in] */ BSTR Name,
    /* [in] */ VARIANT NewVal);


void __RPC_STUB IOdaSvgExporter_put_Properties_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IOdaSvgExporter_Export_Proxy( 
    IOdaSvgExporter * This,
    /* [in] */ IAcadDatabase *Database,
    /* [in] */ BSTR FilePath);


void __RPC_STUB IOdaSvgExporter_Export_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IOdaSvgExporter_INTERFACE_DEFINED__ */



#ifndef __DWGdirectX_LIBRARY_DEFINED__
#define __DWGdirectX_LIBRARY_DEFINED__

/* library DWGdirectX */
/* [helpstring][version][uuid] */ 


























typedef /* [public][uuid] */  DECLSPEC_UUID("84D7DE3C-6588-48de-8BE2-EFD51A55449F") 
enum __MIDL___MIDL_itf_OdaToolkit_0360_0001
    {	odR09_dxf	= -5,
	odR10_dxf	= -4,
	odR12_dwg	= -3,
	odUnknown	= -1,
	odR12_dxf	= 1,
	odR13_dwg	= 4,
	odR13_dxf	= 5,
	odR14_dwg	= 8,
	odR14_dxf	= 9,
	odR15_dwg	= 12,
	odR15_dxf	= 13,
	odR15_Template	= 14,
	odNative	= 24,
	odR18_dwg	= 24,
	odR18_dxf	= 25,
	odR18_Template	= 26
    } 	OdSaveAsType;

typedef /* [public][uuid] */  DECLSPEC_UUID("86FE9BAD-C72B-49E1-A7AA-18827230CB90") 
enum __MIDL___MIDL_itf_OdaToolkit_0361_0001
    {	ACADSECURITYPARAMS_ENCRYPT_DATA	= 1,
	ACADSECURITYPARAMS_ENCRYPT_PROPS	= 2,
	ACADSECURITYPARAMS_SIGN_DATA	= 16,
	ACADSECURITYPARAMS_ADD_TIMESTAMP	= 32
    } 	AcadSecurityParamsType;

typedef /* [public][uuid] */  DECLSPEC_UUID("FA368541-05CB-4D94-B056-EF48C708EA5F") 
enum __MIDL___MIDL_itf_OdaToolkit_0361_0002
    {	ACADSECURITYPARAMS_ALGID_RC4	= 26625
    } 	AcadSecurityParamsConstants;

typedef /* [public][public][public][uuid] */  DECLSPEC_UUID("F7C088E7-7CA3-4AD2-BCC4-B6F4DFD59B55") 
enum __MIDL___MIDL_itf_OdaToolkit_0373_0001
    {	odDwfCompressedBinary	= 0,
	odDwfUncompressedBinary	= 1,
	odDwfAscii	= 2
    } 	OdDwfFormat;

typedef /* [public][public][public][uuid] */  DECLSPEC_UUID("08A7608E-3AE1-412C-9FF6-6C93EBE9C4C2") 
enum __MIDL___MIDL_itf_OdaToolkit_0373_0002
    {	odDwf_v55	= 55,
	odDwf_v42	= 42,
	odDwf_v60	= 600
    } 	OdDwfVersion;

typedef /* [public][public][public][public][uuid] */  DECLSPEC_UUID("BFF266F3-D2AB-11D3-8D11-00108302FDA5") 
enum __MIDL___MIDL_itf_OdaToolkit_0376_0001
    {	acLsNone	= 0,
	acLsOn	= 1,
	acLsFrozen	= 2,
	acLsLocked	= 4,
	acLsPlot	= 8,
	acLsNewViewport	= 16,
	acLsColor	= 32,
	acLsLineType	= 64,
	acLsLineWeight	= 128,
	acLsPlotStyle	= 256,
	acLsAll	= 65535
    } 	AcLayerStateMask;


EXTERN_C const IID LIBID_DWGdirectX;

#ifndef __IAcadApplication_INTERFACE_DEFINED__
#define __IAcadApplication_INTERFACE_DEFINED__

/* interface IAcadApplication */
/* [oleautomation][dual][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IAcadApplication;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("93BC4E71-AFE7-4AA7-BC07-F80ACDB672D5")
    IAcadApplication : public IDispatch
    {
    public:
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Visible( 
            /* [retval][out] */ VARIANT_BOOL *Visible) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Visible( 
            /* [in] */ VARIANT_BOOL Visible) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR *pAppName) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Caption( 
            /* [retval][out] */ BSTR *bstrCaption) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Application( 
            /* [retval][out] */ IAcadApplication **pAppObj) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_ActiveDocument( 
            /* [retval][out] */ IAcadDocument **pActiveDoc) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_ActiveDocument( 
            /* [in] */ IAcadDocument *pActiveDoc) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_FullName( 
            /* [retval][out] */ BSTR *FullName) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Height( 
            /* [retval][out] */ int *Height) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Height( 
            /* [in] */ int Height) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_WindowLeft( 
            /* [retval][out] */ int *left) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_WindowLeft( 
            /* [in] */ int left) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Path( 
            /* [retval][out] */ BSTR *bstrPath) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_LocaleId( 
            /* [retval][out] */ long *lcid) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_WindowTop( 
            /* [retval][out] */ int *top) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_WindowTop( 
            /* [in] */ int top) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Version( 
            /* [retval][out] */ BSTR *bstrVer) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Width( 
            /* [retval][out] */ int *Width) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Width( 
            /* [in] */ int Width) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Preferences( 
            /* [retval][out] */ IAcadPreferences **pPreferences) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_StatusId( 
            /* [in] */ IDispatch *VportObj,
            /* [retval][out] */ VARIANT_BOOL *bStatus) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ListArx( 
            /* [retval][out] */ VARIANT *pVarListArray) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LoadArx( 
            /* [in] */ BSTR Name) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetInterfaceObject( 
            /* [in] */ BSTR ProgID,
            /* [retval][out] */ IDispatch **pObj) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE UnloadArx( 
            /* [in] */ BSTR Name) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Update( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Quit( void) = 0;
        
        virtual /* [helpstring][hidden][id] */ HRESULT STDMETHODCALLTYPE Zoom( 
            /* [in] */ int Type,
            /* [in] */ VARIANT *vParams) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_VBE( 
            /* [retval][out] */ IDispatch **pDispVBE) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_MenuGroups( 
            /* [retval][out] */ IDispatch **pMenuGroups) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_MenuBar( 
            /* [retval][out] */ IDispatch **pMenuBar) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LoadDVB( 
            /* [in] */ BSTR Name) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE UnloadDVB( 
            /* [in] */ BSTR Name) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Documents( 
            /* [retval][out] */ IAcadDocuments **pDocuments) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Eval( 
            /* [in] */ BSTR Expression) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_WindowState( 
            /* [retval][out] */ AcWindowState *eWinState) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_WindowState( 
            /* [in] */ AcWindowState eWinState) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RunMacro( 
            /* [in] */ BSTR MacroPath) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ZoomExtents( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ZoomAll( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ZoomCenter( 
            /* [in] */ VARIANT Center,
            /* [in] */ double Magnify) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ZoomScaled( 
            /* [in] */ double scale,
            /* [in] */ AcZoomScaleType ScaleType) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ZoomWindow( 
            /* [in] */ VARIANT LowerLeft,
            /* [in] */ VARIANT UpperRight) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ZoomPickWindow( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetAcadState( 
            /* [retval][out] */ IDispatch **pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ZoomPrevious( void) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_HWND( 
            /* [retval][out] */ long *HWND) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAcadApplicationVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAcadApplication * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAcadApplication * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAcadApplication * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IAcadApplication * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IAcadApplication * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IAcadApplication * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IAcadApplication * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Visible )( 
            IAcadApplication * This,
            /* [retval][out] */ VARIANT_BOOL *Visible);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Visible )( 
            IAcadApplication * This,
            /* [in] */ VARIANT_BOOL Visible);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            IAcadApplication * This,
            /* [retval][out] */ BSTR *pAppName);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Caption )( 
            IAcadApplication * This,
            /* [retval][out] */ BSTR *bstrCaption);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Application )( 
            IAcadApplication * This,
            /* [retval][out] */ IAcadApplication **pAppObj);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_ActiveDocument )( 
            IAcadApplication * This,
            /* [retval][out] */ IAcadDocument **pActiveDoc);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_ActiveDocument )( 
            IAcadApplication * This,
            /* [in] */ IAcadDocument *pActiveDoc);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_FullName )( 
            IAcadApplication * This,
            /* [retval][out] */ BSTR *FullName);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Height )( 
            IAcadApplication * This,
            /* [retval][out] */ int *Height);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Height )( 
            IAcadApplication * This,
            /* [in] */ int Height);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_WindowLeft )( 
            IAcadApplication * This,
            /* [retval][out] */ int *left);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_WindowLeft )( 
            IAcadApplication * This,
            /* [in] */ int left);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Path )( 
            IAcadApplication * This,
            /* [retval][out] */ BSTR *bstrPath);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_LocaleId )( 
            IAcadApplication * This,
            /* [retval][out] */ long *lcid);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_WindowTop )( 
            IAcadApplication * This,
            /* [retval][out] */ int *top);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_WindowTop )( 
            IAcadApplication * This,
            /* [in] */ int top);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Version )( 
            IAcadApplication * This,
            /* [retval][out] */ BSTR *bstrVer);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Width )( 
            IAcadApplication * This,
            /* [retval][out] */ int *Width);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Width )( 
            IAcadApplication * This,
            /* [in] */ int Width);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Preferences )( 
            IAcadApplication * This,
            /* [retval][out] */ IAcadPreferences **pPreferences);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_StatusId )( 
            IAcadApplication * This,
            /* [in] */ IDispatch *VportObj,
            /* [retval][out] */ VARIANT_BOOL *bStatus);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ListArx )( 
            IAcadApplication * This,
            /* [retval][out] */ VARIANT *pVarListArray);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LoadArx )( 
            IAcadApplication * This,
            /* [in] */ BSTR Name);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetInterfaceObject )( 
            IAcadApplication * This,
            /* [in] */ BSTR ProgID,
            /* [retval][out] */ IDispatch **pObj);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *UnloadArx )( 
            IAcadApplication * This,
            /* [in] */ BSTR Name);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Update )( 
            IAcadApplication * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Quit )( 
            IAcadApplication * This);
        
        /* [helpstring][hidden][id] */ HRESULT ( STDMETHODCALLTYPE *Zoom )( 
            IAcadApplication * This,
            /* [in] */ int Type,
            /* [in] */ VARIANT *vParams);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_VBE )( 
            IAcadApplication * This,
            /* [retval][out] */ IDispatch **pDispVBE);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_MenuGroups )( 
            IAcadApplication * This,
            /* [retval][out] */ IDispatch **pMenuGroups);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_MenuBar )( 
            IAcadApplication * This,
            /* [retval][out] */ IDispatch **pMenuBar);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LoadDVB )( 
            IAcadApplication * This,
            /* [in] */ BSTR Name);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *UnloadDVB )( 
            IAcadApplication * This,
            /* [in] */ BSTR Name);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Documents )( 
            IAcadApplication * This,
            /* [retval][out] */ IAcadDocuments **pDocuments);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Eval )( 
            IAcadApplication * This,
            /* [in] */ BSTR Expression);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_WindowState )( 
            IAcadApplication * This,
            /* [retval][out] */ AcWindowState *eWinState);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_WindowState )( 
            IAcadApplication * This,
            /* [in] */ AcWindowState eWinState);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RunMacro )( 
            IAcadApplication * This,
            /* [in] */ BSTR MacroPath);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ZoomExtents )( 
            IAcadApplication * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ZoomAll )( 
            IAcadApplication * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ZoomCenter )( 
            IAcadApplication * This,
            /* [in] */ VARIANT Center,
            /* [in] */ double Magnify);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ZoomScaled )( 
            IAcadApplication * This,
            /* [in] */ double scale,
            /* [in] */ AcZoomScaleType ScaleType);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ZoomWindow )( 
            IAcadApplication * This,
            /* [in] */ VARIANT LowerLeft,
            /* [in] */ VARIANT UpperRight);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ZoomPickWindow )( 
            IAcadApplication * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetAcadState )( 
            IAcadApplication * This,
            /* [retval][out] */ IDispatch **pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ZoomPrevious )( 
            IAcadApplication * This);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_HWND )( 
            IAcadApplication * This,
            /* [retval][out] */ long *HWND);
        
        END_INTERFACE
    } IAcadApplicationVtbl;

    interface IAcadApplication
    {
        CONST_VTBL struct IAcadApplicationVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAcadApplication_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAcadApplication_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAcadApplication_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAcadApplication_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IAcadApplication_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IAcadApplication_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IAcadApplication_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IAcadApplication_get_Visible(This,Visible)	\
    (This)->lpVtbl -> get_Visible(This,Visible)

#define IAcadApplication_put_Visible(This,Visible)	\
    (This)->lpVtbl -> put_Visible(This,Visible)

#define IAcadApplication_get_Name(This,pAppName)	\
    (This)->lpVtbl -> get_Name(This,pAppName)

#define IAcadApplication_get_Caption(This,bstrCaption)	\
    (This)->lpVtbl -> get_Caption(This,bstrCaption)

#define IAcadApplication_get_Application(This,pAppObj)	\
    (This)->lpVtbl -> get_Application(This,pAppObj)

#define IAcadApplication_get_ActiveDocument(This,pActiveDoc)	\
    (This)->lpVtbl -> get_ActiveDocument(This,pActiveDoc)

#define IAcadApplication_put_ActiveDocument(This,pActiveDoc)	\
    (This)->lpVtbl -> put_ActiveDocument(This,pActiveDoc)

#define IAcadApplication_get_FullName(This,FullName)	\
    (This)->lpVtbl -> get_FullName(This,FullName)

#define IAcadApplication_get_Height(This,Height)	\
    (This)->lpVtbl -> get_Height(This,Height)

#define IAcadApplication_put_Height(This,Height)	\
    (This)->lpVtbl -> put_Height(This,Height)

#define IAcadApplication_get_WindowLeft(This,left)	\
    (This)->lpVtbl -> get_WindowLeft(This,left)

#define IAcadApplication_put_WindowLeft(This,left)	\
    (This)->lpVtbl -> put_WindowLeft(This,left)

#define IAcadApplication_get_Path(This,bstrPath)	\
    (This)->lpVtbl -> get_Path(This,bstrPath)

#define IAcadApplication_get_LocaleId(This,lcid)	\
    (This)->lpVtbl -> get_LocaleId(This,lcid)

#define IAcadApplication_get_WindowTop(This,top)	\
    (This)->lpVtbl -> get_WindowTop(This,top)

#define IAcadApplication_put_WindowTop(This,top)	\
    (This)->lpVtbl -> put_WindowTop(This,top)

#define IAcadApplication_get_Version(This,bstrVer)	\
    (This)->lpVtbl -> get_Version(This,bstrVer)

#define IAcadApplication_get_Width(This,Width)	\
    (This)->lpVtbl -> get_Width(This,Width)

#define IAcadApplication_put_Width(This,Width)	\
    (This)->lpVtbl -> put_Width(This,Width)

#define IAcadApplication_get_Preferences(This,pPreferences)	\
    (This)->lpVtbl -> get_Preferences(This,pPreferences)

#define IAcadApplication_get_StatusId(This,VportObj,bStatus)	\
    (This)->lpVtbl -> get_StatusId(This,VportObj,bStatus)

#define IAcadApplication_ListArx(This,pVarListArray)	\
    (This)->lpVtbl -> ListArx(This,pVarListArray)

#define IAcadApplication_LoadArx(This,Name)	\
    (This)->lpVtbl -> LoadArx(This,Name)

#define IAcadApplication_GetInterfaceObject(This,ProgID,pObj)	\
    (This)->lpVtbl -> GetInterfaceObject(This,ProgID,pObj)

#define IAcadApplication_UnloadArx(This,Name)	\
    (This)->lpVtbl -> UnloadArx(This,Name)

#define IAcadApplication_Update(This)	\
    (This)->lpVtbl -> Update(This)

#define IAcadApplication_Quit(This)	\
    (This)->lpVtbl -> Quit(This)

#define IAcadApplication_Zoom(This,Type,vParams)	\
    (This)->lpVtbl -> Zoom(This,Type,vParams)

#define IAcadApplication_get_VBE(This,pDispVBE)	\
    (This)->lpVtbl -> get_VBE(This,pDispVBE)

#define IAcadApplication_get_MenuGroups(This,pMenuGroups)	\
    (This)->lpVtbl -> get_MenuGroups(This,pMenuGroups)

#define IAcadApplication_get_MenuBar(This,pMenuBar)	\
    (This)->lpVtbl -> get_MenuBar(This,pMenuBar)

#define IAcadApplication_LoadDVB(This,Name)	\
    (This)->lpVtbl -> LoadDVB(This,Name)

#define IAcadApplication_UnloadDVB(This,Name)	\
    (This)->lpVtbl -> UnloadDVB(This,Name)

#define IAcadApplication_get_Documents(This,pDocuments)	\
    (This)->lpVtbl -> get_Documents(This,pDocuments)

#define IAcadApplication_Eval(This,Expression)	\
    (This)->lpVtbl -> Eval(This,Expression)

#define IAcadApplication_get_WindowState(This,eWinState)	\
    (This)->lpVtbl -> get_WindowState(This,eWinState)

#define IAcadApplication_put_WindowState(This,eWinState)	\
    (This)->lpVtbl -> put_WindowState(This,eWinState)

#define IAcadApplication_RunMacro(This,MacroPath)	\
    (This)->lpVtbl -> RunMacro(This,MacroPath)

#define IAcadApplication_ZoomExtents(This)	\
    (This)->lpVtbl -> ZoomExtents(This)

#define IAcadApplication_ZoomAll(This)	\
    (This)->lpVtbl -> ZoomAll(This)

#define IAcadApplication_ZoomCenter(This,Center,Magnify)	\
    (This)->lpVtbl -> ZoomCenter(This,Center,Magnify)

#define IAcadApplication_ZoomScaled(This,scale,ScaleType)	\
    (This)->lpVtbl -> ZoomScaled(This,scale,ScaleType)

#define IAcadApplication_ZoomWindow(This,LowerLeft,UpperRight)	\
    (This)->lpVtbl -> ZoomWindow(This,LowerLeft,UpperRight)

#define IAcadApplication_ZoomPickWindow(This)	\
    (This)->lpVtbl -> ZoomPickWindow(This)

#define IAcadApplication_GetAcadState(This,pVal)	\
    (This)->lpVtbl -> GetAcadState(This,pVal)

#define IAcadApplication_ZoomPrevious(This)	\
    (This)->lpVtbl -> ZoomPrevious(This)

#define IAcadApplication_get_HWND(This,HWND)	\
    (This)->lpVtbl -> get_HWND(This,HWND)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_get_Visible_Proxy( 
    IAcadApplication * This,
    /* [retval][out] */ VARIANT_BOOL *Visible);


void __RPC_STUB IAcadApplication_get_Visible_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_put_Visible_Proxy( 
    IAcadApplication * This,
    /* [in] */ VARIANT_BOOL Visible);


void __RPC_STUB IAcadApplication_put_Visible_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_get_Name_Proxy( 
    IAcadApplication * This,
    /* [retval][out] */ BSTR *pAppName);


void __RPC_STUB IAcadApplication_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_get_Caption_Proxy( 
    IAcadApplication * This,
    /* [retval][out] */ BSTR *bstrCaption);


void __RPC_STUB IAcadApplication_get_Caption_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_get_Application_Proxy( 
    IAcadApplication * This,
    /* [retval][out] */ IAcadApplication **pAppObj);


void __RPC_STUB IAcadApplication_get_Application_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_get_ActiveDocument_Proxy( 
    IAcadApplication * This,
    /* [retval][out] */ IAcadDocument **pActiveDoc);


void __RPC_STUB IAcadApplication_get_ActiveDocument_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_put_ActiveDocument_Proxy( 
    IAcadApplication * This,
    /* [in] */ IAcadDocument *pActiveDoc);


void __RPC_STUB IAcadApplication_put_ActiveDocument_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_get_FullName_Proxy( 
    IAcadApplication * This,
    /* [retval][out] */ BSTR *FullName);


void __RPC_STUB IAcadApplication_get_FullName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_get_Height_Proxy( 
    IAcadApplication * This,
    /* [retval][out] */ int *Height);


void __RPC_STUB IAcadApplication_get_Height_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_put_Height_Proxy( 
    IAcadApplication * This,
    /* [in] */ int Height);


void __RPC_STUB IAcadApplication_put_Height_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_get_WindowLeft_Proxy( 
    IAcadApplication * This,
    /* [retval][out] */ int *left);


void __RPC_STUB IAcadApplication_get_WindowLeft_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_put_WindowLeft_Proxy( 
    IAcadApplication * This,
    /* [in] */ int left);


void __RPC_STUB IAcadApplication_put_WindowLeft_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_get_Path_Proxy( 
    IAcadApplication * This,
    /* [retval][out] */ BSTR *bstrPath);


void __RPC_STUB IAcadApplication_get_Path_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_get_LocaleId_Proxy( 
    IAcadApplication * This,
    /* [retval][out] */ long *lcid);


void __RPC_STUB IAcadApplication_get_LocaleId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_get_WindowTop_Proxy( 
    IAcadApplication * This,
    /* [retval][out] */ int *top);


void __RPC_STUB IAcadApplication_get_WindowTop_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_put_WindowTop_Proxy( 
    IAcadApplication * This,
    /* [in] */ int top);


void __RPC_STUB IAcadApplication_put_WindowTop_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_get_Version_Proxy( 
    IAcadApplication * This,
    /* [retval][out] */ BSTR *bstrVer);


void __RPC_STUB IAcadApplication_get_Version_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_get_Width_Proxy( 
    IAcadApplication * This,
    /* [retval][out] */ int *Width);


void __RPC_STUB IAcadApplication_get_Width_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_put_Width_Proxy( 
    IAcadApplication * This,
    /* [in] */ int Width);


void __RPC_STUB IAcadApplication_put_Width_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_get_Preferences_Proxy( 
    IAcadApplication * This,
    /* [retval][out] */ IAcadPreferences **pPreferences);


void __RPC_STUB IAcadApplication_get_Preferences_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_get_StatusId_Proxy( 
    IAcadApplication * This,
    /* [in] */ IDispatch *VportObj,
    /* [retval][out] */ VARIANT_BOOL *bStatus);


void __RPC_STUB IAcadApplication_get_StatusId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_ListArx_Proxy( 
    IAcadApplication * This,
    /* [retval][out] */ VARIANT *pVarListArray);


void __RPC_STUB IAcadApplication_ListArx_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_LoadArx_Proxy( 
    IAcadApplication * This,
    /* [in] */ BSTR Name);


void __RPC_STUB IAcadApplication_LoadArx_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_GetInterfaceObject_Proxy( 
    IAcadApplication * This,
    /* [in] */ BSTR ProgID,
    /* [retval][out] */ IDispatch **pObj);


void __RPC_STUB IAcadApplication_GetInterfaceObject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_UnloadArx_Proxy( 
    IAcadApplication * This,
    /* [in] */ BSTR Name);


void __RPC_STUB IAcadApplication_UnloadArx_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_Update_Proxy( 
    IAcadApplication * This);


void __RPC_STUB IAcadApplication_Update_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_Quit_Proxy( 
    IAcadApplication * This);


void __RPC_STUB IAcadApplication_Quit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][hidden][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_Zoom_Proxy( 
    IAcadApplication * This,
    /* [in] */ int Type,
    /* [in] */ VARIANT *vParams);


void __RPC_STUB IAcadApplication_Zoom_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_get_VBE_Proxy( 
    IAcadApplication * This,
    /* [retval][out] */ IDispatch **pDispVBE);


void __RPC_STUB IAcadApplication_get_VBE_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_get_MenuGroups_Proxy( 
    IAcadApplication * This,
    /* [retval][out] */ IDispatch **pMenuGroups);


void __RPC_STUB IAcadApplication_get_MenuGroups_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_get_MenuBar_Proxy( 
    IAcadApplication * This,
    /* [retval][out] */ IDispatch **pMenuBar);


void __RPC_STUB IAcadApplication_get_MenuBar_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_LoadDVB_Proxy( 
    IAcadApplication * This,
    /* [in] */ BSTR Name);


void __RPC_STUB IAcadApplication_LoadDVB_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_UnloadDVB_Proxy( 
    IAcadApplication * This,
    /* [in] */ BSTR Name);


void __RPC_STUB IAcadApplication_UnloadDVB_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_get_Documents_Proxy( 
    IAcadApplication * This,
    /* [retval][out] */ IAcadDocuments **pDocuments);


void __RPC_STUB IAcadApplication_get_Documents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_Eval_Proxy( 
    IAcadApplication * This,
    /* [in] */ BSTR Expression);


void __RPC_STUB IAcadApplication_Eval_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_get_WindowState_Proxy( 
    IAcadApplication * This,
    /* [retval][out] */ AcWindowState *eWinState);


void __RPC_STUB IAcadApplication_get_WindowState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_put_WindowState_Proxy( 
    IAcadApplication * This,
    /* [in] */ AcWindowState eWinState);


void __RPC_STUB IAcadApplication_put_WindowState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_RunMacro_Proxy( 
    IAcadApplication * This,
    /* [in] */ BSTR MacroPath);


void __RPC_STUB IAcadApplication_RunMacro_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_ZoomExtents_Proxy( 
    IAcadApplication * This);


void __RPC_STUB IAcadApplication_ZoomExtents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_ZoomAll_Proxy( 
    IAcadApplication * This);


void __RPC_STUB IAcadApplication_ZoomAll_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_ZoomCenter_Proxy( 
    IAcadApplication * This,
    /* [in] */ VARIANT Center,
    /* [in] */ double Magnify);


void __RPC_STUB IAcadApplication_ZoomCenter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_ZoomScaled_Proxy( 
    IAcadApplication * This,
    /* [in] */ double scale,
    /* [in] */ AcZoomScaleType ScaleType);


void __RPC_STUB IAcadApplication_ZoomScaled_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_ZoomWindow_Proxy( 
    IAcadApplication * This,
    /* [in] */ VARIANT LowerLeft,
    /* [in] */ VARIANT UpperRight);


void __RPC_STUB IAcadApplication_ZoomWindow_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_ZoomPickWindow_Proxy( 
    IAcadApplication * This);


void __RPC_STUB IAcadApplication_ZoomPickWindow_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_GetAcadState_Proxy( 
    IAcadApplication * This,
    /* [retval][out] */ IDispatch **pVal);


void __RPC_STUB IAcadApplication_GetAcadState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_ZoomPrevious_Proxy( 
    IAcadApplication * This);


void __RPC_STUB IAcadApplication_ZoomPrevious_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadApplication_get_HWND_Proxy( 
    IAcadApplication * This,
    /* [retval][out] */ long *HWND);


void __RPC_STUB IAcadApplication_get_HWND_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IAcadApplication_INTERFACE_DEFINED__ */


#ifndef __IAcadUtility_INTERFACE_DEFINED__
#define __IAcadUtility_INTERFACE_DEFINED__

/* interface IAcadUtility */
/* [object][oleautomation][dual][helpstring][uuid] */ 


EXTERN_C const IID IID_IAcadUtility;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A6D2CC96-062C-408F-B879-550CCAA81079")
    IAcadUtility : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AngleToReal( 
            /* [in] */ BSTR Angle,
            /* [in] */ AcAngleUnits Unit,
            /* [retval][out] */ double *Value) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AngleToString( 
            /* [in] */ double Angle,
            /* [in] */ AcAngleUnits Unit,
            /* [in] */ int precision,
            /* [retval][out] */ BSTR *bstrValue) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DistanceToReal( 
            /* [in] */ BSTR Distance,
            /* [in] */ AcUnits Unit,
            /* [retval][out] */ double *Value) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RealToString( 
            /* [in] */ double Value,
            /* [in] */ AcUnits Unit,
            /* [in] */ int precision,
            /* [retval][out] */ BSTR *bstrValue) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE TranslateCoordinates( 
            /* [in] */ VARIANT Point,
            /* [in] */ AcCoordinateSystem FromCoordSystem,
            /* [in] */ AcCoordinateSystem ToCoordSystem,
            /* [in] */ int Displacement,
            /* [optional][in] */ VARIANT OCSNormal,
            /* [retval][out] */ VARIANT *transPt) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE InitializeUserInput( 
            /* [in] */ int Bits,
            /* [optional][in] */ VARIANT KeyWordList) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetInteger( 
            /* [optional][in] */ VARIANT Prompt,
            /* [retval][out] */ int *Value) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetReal( 
            /* [optional][in] */ VARIANT Prompt,
            /* [retval][out] */ double *Value) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetInput( 
            /* [retval][out] */ BSTR *Value) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetKeyword( 
            /* [optional][in] */ VARIANT Prompt,
            /* [retval][out] */ BSTR *bstrKeyword) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetString( 
            /* [in] */ int HasSpaces,
            /* [optional][in] */ VARIANT Prompt,
            /* [retval][out] */ BSTR *bstrValue) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetAngle( 
            /* [optional][in] */ VARIANT Point,
            /* [optional][in] */ VARIANT Prompt,
            /* [retval][out] */ double *Angle) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AngleFromXAxis( 
            /* [in] */ VARIANT StartPoint,
            /* [in] */ VARIANT EndPoint,
            /* [retval][out] */ double *Angle) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetCorner( 
            /* [in] */ VARIANT Point,
            /* [optional][in] */ VARIANT Prompt,
            /* [retval][out] */ VARIANT *corner) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetDistance( 
            /* [optional][in] */ VARIANT Point,
            /* [optional][in] */ VARIANT Prompt,
            /* [retval][out] */ double *dist) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetOrientation( 
            /* [optional][in] */ VARIANT Point,
            /* [optional][in] */ VARIANT Prompt,
            /* [retval][out] */ double *Angle) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPoint( 
            /* [optional][in] */ VARIANT Point,
            /* [optional][in] */ VARIANT Prompt,
            /* [retval][out] */ VARIANT *inputPoint) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE PolarPoint( 
            /* [in] */ VARIANT Point,
            /* [in] */ double Angle,
            /* [in] */ double Distance,
            /* [retval][out] */ VARIANT *inputPoint) = 0;
        
        virtual /* [helpstring][vararg][id] */ HRESULT STDMETHODCALLTYPE CreateTypedArray( 
            /* [out] */ VARIANT *varArr,
            /* [in] */ int Type,
            /* [in] */ SAFEARRAY * inArgs) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetEntity( 
            /* [out] */ IDispatch **Object,
            /* [out] */ VARIANT *PickedPoint,
            /* [optional][in] */ VARIANT Prompt) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Prompt( 
            /* [in] */ BSTR Message) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetSubEntity( 
            /* [out] */ IDispatch **Object,
            /* [out] */ VARIANT *PickedPoint,
            /* [out] */ VARIANT *transMatrix,
            /* [out] */ VARIANT *ContextData,
            /* [optional][in] */ VARIANT Prompt) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IsURL( 
            /* [in] */ BSTR URL,
            /* [retval][out] */ VARIANT_BOOL *IsValidURL) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetRemoteFile( 
            /* [in] */ BSTR URL,
            /* [out] */ BSTR *LocalFile,
            /* [in] */ VARIANT_BOOL IgnoreCache) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE PutRemoteFile( 
            /* [in] */ BSTR URL,
            /* [in] */ BSTR LocalFile) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IsRemoteFile( 
            /* [in] */ BSTR LocalFile,
            /* [out] */ BSTR *URL,
            /* [retval][out] */ VARIANT_BOOL *IsDownloadedFile) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LaunchBrowserDialog( 
            /* [out] */ BSTR *SelectedURL,
            /* [in] */ BSTR DialogTitle,
            /* [in] */ BSTR OpenButtonCaption,
            /* [in] */ BSTR StartPageURL,
            /* [in] */ BSTR RegistryRootKey,
            /* [in] */ VARIANT_BOOL OpenButtonAlwaysEnabled,
            /* [retval][out] */ VARIANT_BOOL *success) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAcadUtilityVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAcadUtility * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAcadUtility * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAcadUtility * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IAcadUtility * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IAcadUtility * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IAcadUtility * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IAcadUtility * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AngleToReal )( 
            IAcadUtility * This,
            /* [in] */ BSTR Angle,
            /* [in] */ AcAngleUnits Unit,
            /* [retval][out] */ double *Value);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AngleToString )( 
            IAcadUtility * This,
            /* [in] */ double Angle,
            /* [in] */ AcAngleUnits Unit,
            /* [in] */ int precision,
            /* [retval][out] */ BSTR *bstrValue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DistanceToReal )( 
            IAcadUtility * This,
            /* [in] */ BSTR Distance,
            /* [in] */ AcUnits Unit,
            /* [retval][out] */ double *Value);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RealToString )( 
            IAcadUtility * This,
            /* [in] */ double Value,
            /* [in] */ AcUnits Unit,
            /* [in] */ int precision,
            /* [retval][out] */ BSTR *bstrValue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *TranslateCoordinates )( 
            IAcadUtility * This,
            /* [in] */ VARIANT Point,
            /* [in] */ AcCoordinateSystem FromCoordSystem,
            /* [in] */ AcCoordinateSystem ToCoordSystem,
            /* [in] */ int Displacement,
            /* [optional][in] */ VARIANT OCSNormal,
            /* [retval][out] */ VARIANT *transPt);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *InitializeUserInput )( 
            IAcadUtility * This,
            /* [in] */ int Bits,
            /* [optional][in] */ VARIANT KeyWordList);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetInteger )( 
            IAcadUtility * This,
            /* [optional][in] */ VARIANT Prompt,
            /* [retval][out] */ int *Value);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetReal )( 
            IAcadUtility * This,
            /* [optional][in] */ VARIANT Prompt,
            /* [retval][out] */ double *Value);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetInput )( 
            IAcadUtility * This,
            /* [retval][out] */ BSTR *Value);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetKeyword )( 
            IAcadUtility * This,
            /* [optional][in] */ VARIANT Prompt,
            /* [retval][out] */ BSTR *bstrKeyword);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetString )( 
            IAcadUtility * This,
            /* [in] */ int HasSpaces,
            /* [optional][in] */ VARIANT Prompt,
            /* [retval][out] */ BSTR *bstrValue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetAngle )( 
            IAcadUtility * This,
            /* [optional][in] */ VARIANT Point,
            /* [optional][in] */ VARIANT Prompt,
            /* [retval][out] */ double *Angle);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AngleFromXAxis )( 
            IAcadUtility * This,
            /* [in] */ VARIANT StartPoint,
            /* [in] */ VARIANT EndPoint,
            /* [retval][out] */ double *Angle);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetCorner )( 
            IAcadUtility * This,
            /* [in] */ VARIANT Point,
            /* [optional][in] */ VARIANT Prompt,
            /* [retval][out] */ VARIANT *corner);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetDistance )( 
            IAcadUtility * This,
            /* [optional][in] */ VARIANT Point,
            /* [optional][in] */ VARIANT Prompt,
            /* [retval][out] */ double *dist);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetOrientation )( 
            IAcadUtility * This,
            /* [optional][in] */ VARIANT Point,
            /* [optional][in] */ VARIANT Prompt,
            /* [retval][out] */ double *Angle);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPoint )( 
            IAcadUtility * This,
            /* [optional][in] */ VARIANT Point,
            /* [optional][in] */ VARIANT Prompt,
            /* [retval][out] */ VARIANT *inputPoint);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *PolarPoint )( 
            IAcadUtility * This,
            /* [in] */ VARIANT Point,
            /* [in] */ double Angle,
            /* [in] */ double Distance,
            /* [retval][out] */ VARIANT *inputPoint);
        
        /* [helpstring][vararg][id] */ HRESULT ( STDMETHODCALLTYPE *CreateTypedArray )( 
            IAcadUtility * This,
            /* [out] */ VARIANT *varArr,
            /* [in] */ int Type,
            /* [in] */ SAFEARRAY * inArgs);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetEntity )( 
            IAcadUtility * This,
            /* [out] */ IDispatch **Object,
            /* [out] */ VARIANT *PickedPoint,
            /* [optional][in] */ VARIANT Prompt);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Prompt )( 
            IAcadUtility * This,
            /* [in] */ BSTR Message);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetSubEntity )( 
            IAcadUtility * This,
            /* [out] */ IDispatch **Object,
            /* [out] */ VARIANT *PickedPoint,
            /* [out] */ VARIANT *transMatrix,
            /* [out] */ VARIANT *ContextData,
            /* [optional][in] */ VARIANT Prompt);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *IsURL )( 
            IAcadUtility * This,
            /* [in] */ BSTR URL,
            /* [retval][out] */ VARIANT_BOOL *IsValidURL);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetRemoteFile )( 
            IAcadUtility * This,
            /* [in] */ BSTR URL,
            /* [out] */ BSTR *LocalFile,
            /* [in] */ VARIANT_BOOL IgnoreCache);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *PutRemoteFile )( 
            IAcadUtility * This,
            /* [in] */ BSTR URL,
            /* [in] */ BSTR LocalFile);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *IsRemoteFile )( 
            IAcadUtility * This,
            /* [in] */ BSTR LocalFile,
            /* [out] */ BSTR *URL,
            /* [retval][out] */ VARIANT_BOOL *IsDownloadedFile);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LaunchBrowserDialog )( 
            IAcadUtility * This,
            /* [out] */ BSTR *SelectedURL,
            /* [in] */ BSTR DialogTitle,
            /* [in] */ BSTR OpenButtonCaption,
            /* [in] */ BSTR StartPageURL,
            /* [in] */ BSTR RegistryRootKey,
            /* [in] */ VARIANT_BOOL OpenButtonAlwaysEnabled,
            /* [retval][out] */ VARIANT_BOOL *success);
        
        END_INTERFACE
    } IAcadUtilityVtbl;

    interface IAcadUtility
    {
        CONST_VTBL struct IAcadUtilityVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAcadUtility_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAcadUtility_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAcadUtility_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAcadUtility_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IAcadUtility_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IAcadUtility_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IAcadUtility_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IAcadUtility_AngleToReal(This,Angle,Unit,Value)	\
    (This)->lpVtbl -> AngleToReal(This,Angle,Unit,Value)

#define IAcadUtility_AngleToString(This,Angle,Unit,precision,bstrValue)	\
    (This)->lpVtbl -> AngleToString(This,Angle,Unit,precision,bstrValue)

#define IAcadUtility_DistanceToReal(This,Distance,Unit,Value)	\
    (This)->lpVtbl -> DistanceToReal(This,Distance,Unit,Value)

#define IAcadUtility_RealToString(This,Value,Unit,precision,bstrValue)	\
    (This)->lpVtbl -> RealToString(This,Value,Unit,precision,bstrValue)

#define IAcadUtility_TranslateCoordinates(This,Point,FromCoordSystem,ToCoordSystem,Displacement,OCSNormal,transPt)	\
    (This)->lpVtbl -> TranslateCoordinates(This,Point,FromCoordSystem,ToCoordSystem,Displacement,OCSNormal,transPt)

#define IAcadUtility_InitializeUserInput(This,Bits,KeyWordList)	\
    (This)->lpVtbl -> InitializeUserInput(This,Bits,KeyWordList)

#define IAcadUtility_GetInteger(This,Prompt,Value)	\
    (This)->lpVtbl -> GetInteger(This,Prompt,Value)

#define IAcadUtility_GetReal(This,Prompt,Value)	\
    (This)->lpVtbl -> GetReal(This,Prompt,Value)

#define IAcadUtility_GetInput(This,Value)	\
    (This)->lpVtbl -> GetInput(This,Value)

#define IAcadUtility_GetKeyword(This,Prompt,bstrKeyword)	\
    (This)->lpVtbl -> GetKeyword(This,Prompt,bstrKeyword)

#define IAcadUtility_GetString(This,HasSpaces,Prompt,bstrValue)	\
    (This)->lpVtbl -> GetString(This,HasSpaces,Prompt,bstrValue)

#define IAcadUtility_GetAngle(This,Point,Prompt,Angle)	\
    (This)->lpVtbl -> GetAngle(This,Point,Prompt,Angle)

#define IAcadUtility_AngleFromXAxis(This,StartPoint,EndPoint,Angle)	\
    (This)->lpVtbl -> AngleFromXAxis(This,StartPoint,EndPoint,Angle)

#define IAcadUtility_GetCorner(This,Point,Prompt,corner)	\
    (This)->lpVtbl -> GetCorner(This,Point,Prompt,corner)

#define IAcadUtility_GetDistance(This,Point,Prompt,dist)	\
    (This)->lpVtbl -> GetDistance(This,Point,Prompt,dist)

#define IAcadUtility_GetOrientation(This,Point,Prompt,Angle)	\
    (This)->lpVtbl -> GetOrientation(This,Point,Prompt,Angle)

#define IAcadUtility_GetPoint(This,Point,Prompt,inputPoint)	\
    (This)->lpVtbl -> GetPoint(This,Point,Prompt,inputPoint)

#define IAcadUtility_PolarPoint(This,Point,Angle,Distance,inputPoint)	\
    (This)->lpVtbl -> PolarPoint(This,Point,Angle,Distance,inputPoint)

#define IAcadUtility_CreateTypedArray(This,varArr,Type,inArgs)	\
    (This)->lpVtbl -> CreateTypedArray(This,varArr,Type,inArgs)

#define IAcadUtility_GetEntity(This,Object,PickedPoint,Prompt)	\
    (This)->lpVtbl -> GetEntity(This,Object,PickedPoint,Prompt)

#define IAcadUtility_Prompt(This,Message)	\
    (This)->lpVtbl -> Prompt(This,Message)

#define IAcadUtility_GetSubEntity(This,Object,PickedPoint,transMatrix,ContextData,Prompt)	\
    (This)->lpVtbl -> GetSubEntity(This,Object,PickedPoint,transMatrix,ContextData,Prompt)

#define IAcadUtility_IsURL(This,URL,IsValidURL)	\
    (This)->lpVtbl -> IsURL(This,URL,IsValidURL)

#define IAcadUtility_GetRemoteFile(This,URL,LocalFile,IgnoreCache)	\
    (This)->lpVtbl -> GetRemoteFile(This,URL,LocalFile,IgnoreCache)

#define IAcadUtility_PutRemoteFile(This,URL,LocalFile)	\
    (This)->lpVtbl -> PutRemoteFile(This,URL,LocalFile)

#define IAcadUtility_IsRemoteFile(This,LocalFile,URL,IsDownloadedFile)	\
    (This)->lpVtbl -> IsRemoteFile(This,LocalFile,URL,IsDownloadedFile)

#define IAcadUtility_LaunchBrowserDialog(This,SelectedURL,DialogTitle,OpenButtonCaption,StartPageURL,RegistryRootKey,OpenButtonAlwaysEnabled,success)	\
    (This)->lpVtbl -> LaunchBrowserDialog(This,SelectedURL,DialogTitle,OpenButtonCaption,StartPageURL,RegistryRootKey,OpenButtonAlwaysEnabled,success)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadUtility_AngleToReal_Proxy( 
    IAcadUtility * This,
    /* [in] */ BSTR Angle,
    /* [in] */ AcAngleUnits Unit,
    /* [retval][out] */ double *Value);


void __RPC_STUB IAcadUtility_AngleToReal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadUtility_AngleToString_Proxy( 
    IAcadUtility * This,
    /* [in] */ double Angle,
    /* [in] */ AcAngleUnits Unit,
    /* [in] */ int precision,
    /* [retval][out] */ BSTR *bstrValue);


void __RPC_STUB IAcadUtility_AngleToString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadUtility_DistanceToReal_Proxy( 
    IAcadUtility * This,
    /* [in] */ BSTR Distance,
    /* [in] */ AcUnits Unit,
    /* [retval][out] */ double *Value);


void __RPC_STUB IAcadUtility_DistanceToReal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadUtility_RealToString_Proxy( 
    IAcadUtility * This,
    /* [in] */ double Value,
    /* [in] */ AcUnits Unit,
    /* [in] */ int precision,
    /* [retval][out] */ BSTR *bstrValue);


void __RPC_STUB IAcadUtility_RealToString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadUtility_TranslateCoordinates_Proxy( 
    IAcadUtility * This,
    /* [in] */ VARIANT Point,
    /* [in] */ AcCoordinateSystem FromCoordSystem,
    /* [in] */ AcCoordinateSystem ToCoordSystem,
    /* [in] */ int Displacement,
    /* [optional][in] */ VARIANT OCSNormal,
    /* [retval][out] */ VARIANT *transPt);


void __RPC_STUB IAcadUtility_TranslateCoordinates_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadUtility_InitializeUserInput_Proxy( 
    IAcadUtility * This,
    /* [in] */ int Bits,
    /* [optional][in] */ VARIANT KeyWordList);


void __RPC_STUB IAcadUtility_InitializeUserInput_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadUtility_GetInteger_Proxy( 
    IAcadUtility * This,
    /* [optional][in] */ VARIANT Prompt,
    /* [retval][out] */ int *Value);


void __RPC_STUB IAcadUtility_GetInteger_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadUtility_GetReal_Proxy( 
    IAcadUtility * This,
    /* [optional][in] */ VARIANT Prompt,
    /* [retval][out] */ double *Value);


void __RPC_STUB IAcadUtility_GetReal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadUtility_GetInput_Proxy( 
    IAcadUtility * This,
    /* [retval][out] */ BSTR *Value);


void __RPC_STUB IAcadUtility_GetInput_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadUtility_GetKeyword_Proxy( 
    IAcadUtility * This,
    /* [optional][in] */ VARIANT Prompt,
    /* [retval][out] */ BSTR *bstrKeyword);


void __RPC_STUB IAcadUtility_GetKeyword_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadUtility_GetString_Proxy( 
    IAcadUtility * This,
    /* [in] */ int HasSpaces,
    /* [optional][in] */ VARIANT Prompt,
    /* [retval][out] */ BSTR *bstrValue);


void __RPC_STUB IAcadUtility_GetString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadUtility_GetAngle_Proxy( 
    IAcadUtility * This,
    /* [optional][in] */ VARIANT Point,
    /* [optional][in] */ VARIANT Prompt,
    /* [retval][out] */ double *Angle);


void __RPC_STUB IAcadUtility_GetAngle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadUtility_AngleFromXAxis_Proxy( 
    IAcadUtility * This,
    /* [in] */ VARIANT StartPoint,
    /* [in] */ VARIANT EndPoint,
    /* [retval][out] */ double *Angle);


void __RPC_STUB IAcadUtility_AngleFromXAxis_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadUtility_GetCorner_Proxy( 
    IAcadUtility * This,
    /* [in] */ VARIANT Point,
    /* [optional][in] */ VARIANT Prompt,
    /* [retval][out] */ VARIANT *corner);


void __RPC_STUB IAcadUtility_GetCorner_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadUtility_GetDistance_Proxy( 
    IAcadUtility * This,
    /* [optional][in] */ VARIANT Point,
    /* [optional][in] */ VARIANT Prompt,
    /* [retval][out] */ double *dist);


void __RPC_STUB IAcadUtility_GetDistance_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadUtility_GetOrientation_Proxy( 
    IAcadUtility * This,
    /* [optional][in] */ VARIANT Point,
    /* [optional][in] */ VARIANT Prompt,
    /* [retval][out] */ double *Angle);


void __RPC_STUB IAcadUtility_GetOrientation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadUtility_GetPoint_Proxy( 
    IAcadUtility * This,
    /* [optional][in] */ VARIANT Point,
    /* [optional][in] */ VARIANT Prompt,
    /* [retval][out] */ VARIANT *inputPoint);


void __RPC_STUB IAcadUtility_GetPoint_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadUtility_PolarPoint_Proxy( 
    IAcadUtility * This,
    /* [in] */ VARIANT Point,
    /* [in] */ double Angle,
    /* [in] */ double Distance,
    /* [retval][out] */ VARIANT *inputPoint);


void __RPC_STUB IAcadUtility_PolarPoint_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][vararg][id] */ HRESULT STDMETHODCALLTYPE IAcadUtility_CreateTypedArray_Proxy( 
    IAcadUtility * This,
    /* [out] */ VARIANT *varArr,
    /* [in] */ int Type,
    /* [in] */ SAFEARRAY * inArgs);


void __RPC_STUB IAcadUtility_CreateTypedArray_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadUtility_GetEntity_Proxy( 
    IAcadUtility * This,
    /* [out] */ IDispatch **Object,
    /* [out] */ VARIANT *PickedPoint,
    /* [optional][in] */ VARIANT Prompt);


void __RPC_STUB IAcadUtility_GetEntity_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadUtility_Prompt_Proxy( 
    IAcadUtility * This,
    /* [in] */ BSTR Message);


void __RPC_STUB IAcadUtility_Prompt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadUtility_GetSubEntity_Proxy( 
    IAcadUtility * This,
    /* [out] */ IDispatch **Object,
    /* [out] */ VARIANT *PickedPoint,
    /* [out] */ VARIANT *transMatrix,
    /* [out] */ VARIANT *ContextData,
    /* [optional][in] */ VARIANT Prompt);


void __RPC_STUB IAcadUtility_GetSubEntity_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadUtility_IsURL_Proxy( 
    IAcadUtility * This,
    /* [in] */ BSTR URL,
    /* [retval][out] */ VARIANT_BOOL *IsValidURL);


void __RPC_STUB IAcadUtility_IsURL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadUtility_GetRemoteFile_Proxy( 
    IAcadUtility * This,
    /* [in] */ BSTR URL,
    /* [out] */ BSTR *LocalFile,
    /* [in] */ VARIANT_BOOL IgnoreCache);


void __RPC_STUB IAcadUtility_GetRemoteFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadUtility_PutRemoteFile_Proxy( 
    IAcadUtility * This,
    /* [in] */ BSTR URL,
    /* [in] */ BSTR LocalFile);


void __RPC_STUB IAcadUtility_PutRemoteFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadUtility_IsRemoteFile_Proxy( 
    IAcadUtility * This,
    /* [in] */ BSTR LocalFile,
    /* [out] */ BSTR *URL,
    /* [retval][out] */ VARIANT_BOOL *IsDownloadedFile);


void __RPC_STUB IAcadUtility_IsRemoteFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadUtility_LaunchBrowserDialog_Proxy( 
    IAcadUtility * This,
    /* [out] */ BSTR *SelectedURL,
    /* [in] */ BSTR DialogTitle,
    /* [in] */ BSTR OpenButtonCaption,
    /* [in] */ BSTR StartPageURL,
    /* [in] */ BSTR RegistryRootKey,
    /* [in] */ VARIANT_BOOL OpenButtonAlwaysEnabled,
    /* [retval][out] */ VARIANT_BOOL *success);


void __RPC_STUB IAcadUtility_LaunchBrowserDialog_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IAcadUtility_INTERFACE_DEFINED__ */


#ifndef __IAcadDocument_INTERFACE_DEFINED__
#define __IAcadDocument_INTERFACE_DEFINED__

/* interface IAcadDocument */
/* [oleautomation][dual][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IAcadDocument;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3C391D99-2886-4E1A-9855-EE30C9FC5B8A")
    IAcadDocument : public IAcadDatabase
    {
    public:
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Plot( 
            /* [retval][out] */ IDispatch **pPlot) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_ActiveLayer( 
            /* [retval][out] */ IAcadLayer **pActLayer) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_ActiveLayer( 
            /* [in] */ IAcadLayer *pActLayer) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_ActiveLinetype( 
            /* [retval][out] */ IAcadLineType **pActLinetype) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_ActiveLinetype( 
            /* [in] */ IAcadLineType *pActLinetype) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_ActiveDimStyle( 
            /* [retval][out] */ IAcadDimStyle **pActDimStyle) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_ActiveDimStyle( 
            /* [in] */ IAcadDimStyle *pActDimStyle) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_ActiveTextStyle( 
            /* [retval][out] */ IAcadTextStyle **pActTextStyle) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_ActiveTextStyle( 
            /* [in] */ IAcadTextStyle *pActTextStyle) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_ActiveUCS( 
            /* [retval][out] */ IAcadUCS **pActUCS) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_ActiveUCS( 
            /* [in] */ IAcadUCS *pActUCS) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_ActiveViewport( 
            /* [retval][out] */ IAcadViewport **pActView) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_ActiveViewport( 
            /* [in] */ IAcadViewport *pActView) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_ActivePViewport( 
            /* [retval][out] */ IAcadPViewport **pActView) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_ActivePViewport( 
            /* [in] */ IAcadPViewport *pActView) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_ActiveSpace( 
            /* [retval][out] */ AcActiveSpace *ActSpace) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_ActiveSpace( 
            /* [in] */ AcActiveSpace ActSpace) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_SelectionSets( 
            /* [retval][out] */ IAcadSelectionSets **pSelSets) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_ActiveSelectionSet( 
            /* [retval][out] */ IAcadSelectionSet **pSelSet) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_FullName( 
            /* [retval][out] */ BSTR *FullName) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR *Name) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Path( 
            /* [retval][out] */ BSTR *Path) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_ObjectSnapMode( 
            /* [retval][out] */ VARIANT_BOOL *fSnapMode) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_ObjectSnapMode( 
            /* [in] */ VARIANT_BOOL fSnapMode) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_ReadOnly( 
            /* [retval][out] */ VARIANT_BOOL *bReadOnly) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Saved( 
            /* [retval][out] */ VARIANT_BOOL *bSaved) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_MSpace( 
            /* [retval][out] */ VARIANT_BOOL *Mode) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_MSpace( 
            /* [in] */ VARIANT_BOOL Mode) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Utility( 
            /* [retval][out] */ IAcadUtility **pUtil) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Open( 
            /* [in] */ BSTR FullName,
            /* [optional][in] */ VARIANT Password,
            /* [retval][out] */ IAcadDocument **pDocObj) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AuditInfo( 
            /* [in] */ VARIANT_BOOL FixErr) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Import( 
            /* [in] */ BSTR FileName,
            /* [in] */ VARIANT InsertionPoint,
            /* [in] */ double ScaleFactor,
            /* [retval][out] */ IDispatch **pObj) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Export( 
            /* [in] */ BSTR FileName,
            /* [in] */ BSTR Extension,
            /* [in] */ IAcadSelectionSet *SelectionSet) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE New( 
            /* [in] */ BSTR TemplateFileName,
            /* [retval][out] */ IAcadDocument **pDocObj) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Save( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SaveAs( 
            /* [in] */ BSTR FullFileName,
            /* [optional][in] */ VARIANT SaveAsType,
            /* [optional][in] */ VARIANT vSecurityParams) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Wblock( 
            /* [in] */ BSTR FileName,
            /* [in] */ IAcadSelectionSet *SelectionSet) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE PurgeAll( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetVariable( 
            /* [in] */ BSTR Name,
            /* [retval][out] */ VARIANT *Value) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetVariable( 
            /* [in] */ BSTR Name,
            /* [in] */ VARIANT Value) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LoadShapeFile( 
            /* [in] */ BSTR FullName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Regen( 
            /* [in] */ AcRegenType WhichViewports) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_PickfirstSelectionSet( 
            /* [retval][out] */ IDispatch **pSelSet) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Active( 
            /* [retval][out] */ VARIANT_BOOL *pvbActive) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Activate( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Close( 
            /* [optional][in] */ VARIANT SaveChanges,
            /* [optional][in] */ VARIANT FileName) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_WindowState( 
            /* [in] */ AcWindowState pWinState) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_WindowState( 
            /* [retval][out] */ AcWindowState *pWinState) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Width( 
            /* [in] */ int pWidth) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Width( 
            /* [retval][out] */ int *pWidth) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Height( 
            /* [in] */ int pHeight) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Height( 
            /* [retval][out] */ int *pHeight) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_ActiveLayout( 
            /* [in] */ IAcadLayout *pLayout) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_ActiveLayout( 
            /* [retval][out] */ IAcadLayout **pLayout) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SendCommand( 
            /* [in] */ BSTR Command) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_HWND( 
            /* [retval][out] */ long *HWND) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_WindowTitle( 
            /* [retval][out] */ BSTR *Title) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Application( 
            /* [retval][out] */ IAcadApplication **pAppObj) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Database( 
            /* [retval][out] */ IAcadDatabase **pDatabase) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE StartUndoMark( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE EndUndoMark( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAcadDocumentVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAcadDocument * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAcadDocument * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAcadDocument * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IAcadDocument * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IAcadDocument * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IAcadDocument * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IAcadDocument * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_ModelSpace )( 
            IAcadDocument * This,
            /* [retval][out] */ IAcadModelSpace **pMSpace);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_PaperSpace )( 
            IAcadDocument * This,
            /* [retval][out] */ IAcadPaperSpace **pPSpace);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Blocks )( 
            IAcadDocument * This,
            /* [retval][out] */ IAcadBlocks **pBlocks);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CopyObjects )( 
            IAcadDocument * This,
            /* [in] */ VARIANT Objects,
            /* [optional][in] */ VARIANT Owner,
            /* [optional][out][in] */ VARIANT *IdPairs,
            /* [retval][out] */ VARIANT *pNewObjects);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Groups )( 
            IAcadDocument * This,
            /* [retval][out] */ IAcadGroups **pGroups);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_DimStyles )( 
            IAcadDocument * This,
            /* [retval][out] */ IAcadDimStyles **pDimStyles);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Layers )( 
            IAcadDocument * This,
            /* [retval][out] */ IAcadLayers **pLayers);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Linetypes )( 
            IAcadDocument * This,
            /* [retval][out] */ IAcadLineTypes **pLinetypes);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Dictionaries )( 
            IAcadDocument * This,
            /* [retval][out] */ IAcadDictionaries **pDictionaries);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_RegisteredApplications )( 
            IAcadDocument * This,
            /* [retval][out] */ IAcadRegisteredApplications **pRegApps);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_TextStyles )( 
            IAcadDocument * This,
            /* [retval][out] */ IAcadTextStyles **pTextStyles);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_UserCoordinateSystems )( 
            IAcadDocument * This,
            /* [retval][out] */ IAcadUCSs **pUCSs);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Views )( 
            IAcadDocument * This,
            /* [retval][out] */ IAcadViews **pViews);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Viewports )( 
            IAcadDocument * This,
            /* [retval][out] */ IAcadViewports **pViewports);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_ElevationModelSpace )( 
            IAcadDocument * This,
            /* [retval][out] */ double *Elevation);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_ElevationModelSpace )( 
            IAcadDocument * This,
            /* [in] */ double Elevation);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_ElevationPaperSpace )( 
            IAcadDocument * This,
            /* [retval][out] */ double *Elevation);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_ElevationPaperSpace )( 
            IAcadDocument * This,
            /* [in] */ double Elevation);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Limits )( 
            IAcadDocument * This,
            /* [retval][out] */ VARIANT *Limits);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Limits )( 
            IAcadDocument * This,
            /* [in] */ VARIANT Limits);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *HandleToObject )( 
            IAcadDocument * This,
            /* [in] */ BSTR Handle,
            /* [retval][out] */ IDispatch **pObj);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ObjectIdToObject )( 
            IAcadDocument * This,
            /* [in] */ long ObjectID,
            /* [retval][out] */ IDispatch **pObj);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Layouts )( 
            IAcadDocument * This,
            /* [retval][out] */ IAcadLayouts **pLayouts);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_PlotConfigurations )( 
            IAcadDocument * This,
            /* [retval][out] */ IAcadPlotConfigurations **pPlotConfigs);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Preferences )( 
            IAcadDocument * This,
            /* [retval][out] */ IAcadDatabasePreferences **pPref);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_FileDependencies )( 
            IAcadDocument * This,
            /* [retval][out] */ IAcadFileDependencies **pFDM);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_SummaryInfo )( 
            IAcadDocument * This,
            /* [retval][out] */ IAcadSummaryInfo **pSummaryInfo);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Plot )( 
            IAcadDocument * This,
            /* [retval][out] */ IDispatch **pPlot);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_ActiveLayer )( 
            IAcadDocument * This,
            /* [retval][out] */ IAcadLayer **pActLayer);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_ActiveLayer )( 
            IAcadDocument * This,
            /* [in] */ IAcadLayer *pActLayer);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_ActiveLinetype )( 
            IAcadDocument * This,
            /* [retval][out] */ IAcadLineType **pActLinetype);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_ActiveLinetype )( 
            IAcadDocument * This,
            /* [in] */ IAcadLineType *pActLinetype);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_ActiveDimStyle )( 
            IAcadDocument * This,
            /* [retval][out] */ IAcadDimStyle **pActDimStyle);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_ActiveDimStyle )( 
            IAcadDocument * This,
            /* [in] */ IAcadDimStyle *pActDimStyle);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_ActiveTextStyle )( 
            IAcadDocument * This,
            /* [retval][out] */ IAcadTextStyle **pActTextStyle);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_ActiveTextStyle )( 
            IAcadDocument * This,
            /* [in] */ IAcadTextStyle *pActTextStyle);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_ActiveUCS )( 
            IAcadDocument * This,
            /* [retval][out] */ IAcadUCS **pActUCS);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_ActiveUCS )( 
            IAcadDocument * This,
            /* [in] */ IAcadUCS *pActUCS);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_ActiveViewport )( 
            IAcadDocument * This,
            /* [retval][out] */ IAcadViewport **pActView);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_ActiveViewport )( 
            IAcadDocument * This,
            /* [in] */ IAcadViewport *pActView);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_ActivePViewport )( 
            IAcadDocument * This,
            /* [retval][out] */ IAcadPViewport **pActView);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_ActivePViewport )( 
            IAcadDocument * This,
            /* [in] */ IAcadPViewport *pActView);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_ActiveSpace )( 
            IAcadDocument * This,
            /* [retval][out] */ AcActiveSpace *ActSpace);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_ActiveSpace )( 
            IAcadDocument * This,
            /* [in] */ AcActiveSpace ActSpace);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_SelectionSets )( 
            IAcadDocument * This,
            /* [retval][out] */ IAcadSelectionSets **pSelSets);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_ActiveSelectionSet )( 
            IAcadDocument * This,
            /* [retval][out] */ IAcadSelectionSet **pSelSet);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_FullName )( 
            IAcadDocument * This,
            /* [retval][out] */ BSTR *FullName);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            IAcadDocument * This,
            /* [retval][out] */ BSTR *Name);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Path )( 
            IAcadDocument * This,
            /* [retval][out] */ BSTR *Path);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_ObjectSnapMode )( 
            IAcadDocument * This,
            /* [retval][out] */ VARIANT_BOOL *fSnapMode);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_ObjectSnapMode )( 
            IAcadDocument * This,
            /* [in] */ VARIANT_BOOL fSnapMode);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_ReadOnly )( 
            IAcadDocument * This,
            /* [retval][out] */ VARIANT_BOOL *bReadOnly);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Saved )( 
            IAcadDocument * This,
            /* [retval][out] */ VARIANT_BOOL *bSaved);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_MSpace )( 
            IAcadDocument * This,
            /* [retval][out] */ VARIANT_BOOL *Mode);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_MSpace )( 
            IAcadDocument * This,
            /* [in] */ VARIANT_BOOL Mode);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Utility )( 
            IAcadDocument * This,
            /* [retval][out] */ IAcadUtility **pUtil);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Open )( 
            IAcadDocument * This,
            /* [in] */ BSTR FullName,
            /* [optional][in] */ VARIANT Password,
            /* [retval][out] */ IAcadDocument **pDocObj);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AuditInfo )( 
            IAcadDocument * This,
            /* [in] */ VARIANT_BOOL FixErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Import )( 
            IAcadDocument * This,
            /* [in] */ BSTR FileName,
            /* [in] */ VARIANT InsertionPoint,
            /* [in] */ double ScaleFactor,
            /* [retval][out] */ IDispatch **pObj);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Export )( 
            IAcadDocument * This,
            /* [in] */ BSTR FileName,
            /* [in] */ BSTR Extension,
            /* [in] */ IAcadSelectionSet *SelectionSet);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *New )( 
            IAcadDocument * This,
            /* [in] */ BSTR TemplateFileName,
            /* [retval][out] */ IAcadDocument **pDocObj);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Save )( 
            IAcadDocument * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SaveAs )( 
            IAcadDocument * This,
            /* [in] */ BSTR FullFileName,
            /* [optional][in] */ VARIANT SaveAsType,
            /* [optional][in] */ VARIANT vSecurityParams);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Wblock )( 
            IAcadDocument * This,
            /* [in] */ BSTR FileName,
            /* [in] */ IAcadSelectionSet *SelectionSet);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *PurgeAll )( 
            IAcadDocument * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetVariable )( 
            IAcadDocument * This,
            /* [in] */ BSTR Name,
            /* [retval][out] */ VARIANT *Value);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetVariable )( 
            IAcadDocument * This,
            /* [in] */ BSTR Name,
            /* [in] */ VARIANT Value);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LoadShapeFile )( 
            IAcadDocument * This,
            /* [in] */ BSTR FullName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Regen )( 
            IAcadDocument * This,
            /* [in] */ AcRegenType WhichViewports);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_PickfirstSelectionSet )( 
            IAcadDocument * This,
            /* [retval][out] */ IDispatch **pSelSet);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Active )( 
            IAcadDocument * This,
            /* [retval][out] */ VARIANT_BOOL *pvbActive);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Activate )( 
            IAcadDocument * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Close )( 
            IAcadDocument * This,
            /* [optional][in] */ VARIANT SaveChanges,
            /* [optional][in] */ VARIANT FileName);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_WindowState )( 
            IAcadDocument * This,
            /* [in] */ AcWindowState pWinState);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_WindowState )( 
            IAcadDocument * This,
            /* [retval][out] */ AcWindowState *pWinState);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Width )( 
            IAcadDocument * This,
            /* [in] */ int pWidth);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Width )( 
            IAcadDocument * This,
            /* [retval][out] */ int *pWidth);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Height )( 
            IAcadDocument * This,
            /* [in] */ int pHeight);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Height )( 
            IAcadDocument * This,
            /* [retval][out] */ int *pHeight);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_ActiveLayout )( 
            IAcadDocument * This,
            /* [in] */ IAcadLayout *pLayout);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_ActiveLayout )( 
            IAcadDocument * This,
            /* [retval][out] */ IAcadLayout **pLayout);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SendCommand )( 
            IAcadDocument * This,
            /* [in] */ BSTR Command);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_HWND )( 
            IAcadDocument * This,
            /* [retval][out] */ long *HWND);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_WindowTitle )( 
            IAcadDocument * This,
            /* [retval][out] */ BSTR *Title);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Application )( 
            IAcadDocument * This,
            /* [retval][out] */ IAcadApplication **pAppObj);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Database )( 
            IAcadDocument * This,
            /* [retval][out] */ IAcadDatabase **pDatabase);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *StartUndoMark )( 
            IAcadDocument * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *EndUndoMark )( 
            IAcadDocument * This);
        
        END_INTERFACE
    } IAcadDocumentVtbl;

    interface IAcadDocument
    {
        CONST_VTBL struct IAcadDocumentVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAcadDocument_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAcadDocument_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAcadDocument_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAcadDocument_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IAcadDocument_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IAcadDocument_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IAcadDocument_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IAcadDocument_get_ModelSpace(This,pMSpace)	\
    (This)->lpVtbl -> get_ModelSpace(This,pMSpace)

#define IAcadDocument_get_PaperSpace(This,pPSpace)	\
    (This)->lpVtbl -> get_PaperSpace(This,pPSpace)

#define IAcadDocument_get_Blocks(This,pBlocks)	\
    (This)->lpVtbl -> get_Blocks(This,pBlocks)

#define IAcadDocument_CopyObjects(This,Objects,Owner,IdPairs,pNewObjects)	\
    (This)->lpVtbl -> CopyObjects(This,Objects,Owner,IdPairs,pNewObjects)

#define IAcadDocument_get_Groups(This,pGroups)	\
    (This)->lpVtbl -> get_Groups(This,pGroups)

#define IAcadDocument_get_DimStyles(This,pDimStyles)	\
    (This)->lpVtbl -> get_DimStyles(This,pDimStyles)

#define IAcadDocument_get_Layers(This,pLayers)	\
    (This)->lpVtbl -> get_Layers(This,pLayers)

#define IAcadDocument_get_Linetypes(This,pLinetypes)	\
    (This)->lpVtbl -> get_Linetypes(This,pLinetypes)

#define IAcadDocument_get_Dictionaries(This,pDictionaries)	\
    (This)->lpVtbl -> get_Dictionaries(This,pDictionaries)

#define IAcadDocument_get_RegisteredApplications(This,pRegApps)	\
    (This)->lpVtbl -> get_RegisteredApplications(This,pRegApps)

#define IAcadDocument_get_TextStyles(This,pTextStyles)	\
    (This)->lpVtbl -> get_TextStyles(This,pTextStyles)

#define IAcadDocument_get_UserCoordinateSystems(This,pUCSs)	\
    (This)->lpVtbl -> get_UserCoordinateSystems(This,pUCSs)

#define IAcadDocument_get_Views(This,pViews)	\
    (This)->lpVtbl -> get_Views(This,pViews)

#define IAcadDocument_get_Viewports(This,pViewports)	\
    (This)->lpVtbl -> get_Viewports(This,pViewports)

#define IAcadDocument_get_ElevationModelSpace(This,Elevation)	\
    (This)->lpVtbl -> get_ElevationModelSpace(This,Elevation)

#define IAcadDocument_put_ElevationModelSpace(This,Elevation)	\
    (This)->lpVtbl -> put_ElevationModelSpace(This,Elevation)

#define IAcadDocument_get_ElevationPaperSpace(This,Elevation)	\
    (This)->lpVtbl -> get_ElevationPaperSpace(This,Elevation)

#define IAcadDocument_put_ElevationPaperSpace(This,Elevation)	\
    (This)->lpVtbl -> put_ElevationPaperSpace(This,Elevation)

#define IAcadDocument_get_Limits(This,Limits)	\
    (This)->lpVtbl -> get_Limits(This,Limits)

#define IAcadDocument_put_Limits(This,Limits)	\
    (This)->lpVtbl -> put_Limits(This,Limits)

#define IAcadDocument_HandleToObject(This,Handle,pObj)	\
    (This)->lpVtbl -> HandleToObject(This,Handle,pObj)

#define IAcadDocument_ObjectIdToObject(This,ObjectID,pObj)	\
    (This)->lpVtbl -> ObjectIdToObject(This,ObjectID,pObj)

#define IAcadDocument_get_Layouts(This,pLayouts)	\
    (This)->lpVtbl -> get_Layouts(This,pLayouts)

#define IAcadDocument_get_PlotConfigurations(This,pPlotConfigs)	\
    (This)->lpVtbl -> get_PlotConfigurations(This,pPlotConfigs)

#define IAcadDocument_get_Preferences(This,pPref)	\
    (This)->lpVtbl -> get_Preferences(This,pPref)

#define IAcadDocument_get_FileDependencies(This,pFDM)	\
    (This)->lpVtbl -> get_FileDependencies(This,pFDM)

#define IAcadDocument_get_SummaryInfo(This,pSummaryInfo)	\
    (This)->lpVtbl -> get_SummaryInfo(This,pSummaryInfo)


#define IAcadDocument_get_Plot(This,pPlot)	\
    (This)->lpVtbl -> get_Plot(This,pPlot)

#define IAcadDocument_get_ActiveLayer(This,pActLayer)	\
    (This)->lpVtbl -> get_ActiveLayer(This,pActLayer)

#define IAcadDocument_put_ActiveLayer(This,pActLayer)	\
    (This)->lpVtbl -> put_ActiveLayer(This,pActLayer)

#define IAcadDocument_get_ActiveLinetype(This,pActLinetype)	\
    (This)->lpVtbl -> get_ActiveLinetype(This,pActLinetype)

#define IAcadDocument_put_ActiveLinetype(This,pActLinetype)	\
    (This)->lpVtbl -> put_ActiveLinetype(This,pActLinetype)

#define IAcadDocument_get_ActiveDimStyle(This,pActDimStyle)	\
    (This)->lpVtbl -> get_ActiveDimStyle(This,pActDimStyle)

#define IAcadDocument_put_ActiveDimStyle(This,pActDimStyle)	\
    (This)->lpVtbl -> put_ActiveDimStyle(This,pActDimStyle)

#define IAcadDocument_get_ActiveTextStyle(This,pActTextStyle)	\
    (This)->lpVtbl -> get_ActiveTextStyle(This,pActTextStyle)

#define IAcadDocument_put_ActiveTextStyle(This,pActTextStyle)	\
    (This)->lpVtbl -> put_ActiveTextStyle(This,pActTextStyle)

#define IAcadDocument_get_ActiveUCS(This,pActUCS)	\
    (This)->lpVtbl -> get_ActiveUCS(This,pActUCS)

#define IAcadDocument_put_ActiveUCS(This,pActUCS)	\
    (This)->lpVtbl -> put_ActiveUCS(This,pActUCS)

#define IAcadDocument_get_ActiveViewport(This,pActView)	\
    (This)->lpVtbl -> get_ActiveViewport(This,pActView)

#define IAcadDocument_put_ActiveViewport(This,pActView)	\
    (This)->lpVtbl -> put_ActiveViewport(This,pActView)

#define IAcadDocument_get_ActivePViewport(This,pActView)	\
    (This)->lpVtbl -> get_ActivePViewport(This,pActView)

#define IAcadDocument_put_ActivePViewport(This,pActView)	\
    (This)->lpVtbl -> put_ActivePViewport(This,pActView)

#define IAcadDocument_get_ActiveSpace(This,ActSpace)	\
    (This)->lpVtbl -> get_ActiveSpace(This,ActSpace)

#define IAcadDocument_put_ActiveSpace(This,ActSpace)	\
    (This)->lpVtbl -> put_ActiveSpace(This,ActSpace)

#define IAcadDocument_get_SelectionSets(This,pSelSets)	\
    (This)->lpVtbl -> get_SelectionSets(This,pSelSets)

#define IAcadDocument_get_ActiveSelectionSet(This,pSelSet)	\
    (This)->lpVtbl -> get_ActiveSelectionSet(This,pSelSet)

#define IAcadDocument_get_FullName(This,FullName)	\
    (This)->lpVtbl -> get_FullName(This,FullName)

#define IAcadDocument_get_Name(This,Name)	\
    (This)->lpVtbl -> get_Name(This,Name)

#define IAcadDocument_get_Path(This,Path)	\
    (This)->lpVtbl -> get_Path(This,Path)

#define IAcadDocument_get_ObjectSnapMode(This,fSnapMode)	\
    (This)->lpVtbl -> get_ObjectSnapMode(This,fSnapMode)

#define IAcadDocument_put_ObjectSnapMode(This,fSnapMode)	\
    (This)->lpVtbl -> put_ObjectSnapMode(This,fSnapMode)

#define IAcadDocument_get_ReadOnly(This,bReadOnly)	\
    (This)->lpVtbl -> get_ReadOnly(This,bReadOnly)

#define IAcadDocument_get_Saved(This,bSaved)	\
    (This)->lpVtbl -> get_Saved(This,bSaved)

#define IAcadDocument_get_MSpace(This,Mode)	\
    (This)->lpVtbl -> get_MSpace(This,Mode)

#define IAcadDocument_put_MSpace(This,Mode)	\
    (This)->lpVtbl -> put_MSpace(This,Mode)

#define IAcadDocument_get_Utility(This,pUtil)	\
    (This)->lpVtbl -> get_Utility(This,pUtil)

#define IAcadDocument_Open(This,FullName,Password,pDocObj)	\
    (This)->lpVtbl -> Open(This,FullName,Password,pDocObj)

#define IAcadDocument_AuditInfo(This,FixErr)	\
    (This)->lpVtbl -> AuditInfo(This,FixErr)

#define IAcadDocument_Import(This,FileName,InsertionPoint,ScaleFactor,pObj)	\
    (This)->lpVtbl -> Import(This,FileName,InsertionPoint,ScaleFactor,pObj)

#define IAcadDocument_Export(This,FileName,Extension,SelectionSet)	\
    (This)->lpVtbl -> Export(This,FileName,Extension,SelectionSet)

#define IAcadDocument_New(This,TemplateFileName,pDocObj)	\
    (This)->lpVtbl -> New(This,TemplateFileName,pDocObj)

#define IAcadDocument_Save(This)	\
    (This)->lpVtbl -> Save(This)

#define IAcadDocument_SaveAs(This,FullFileName,SaveAsType,vSecurityParams)	\
    (This)->lpVtbl -> SaveAs(This,FullFileName,SaveAsType,vSecurityParams)

#define IAcadDocument_Wblock(This,FileName,SelectionSet)	\
    (This)->lpVtbl -> Wblock(This,FileName,SelectionSet)

#define IAcadDocument_PurgeAll(This)	\
    (This)->lpVtbl -> PurgeAll(This)

#define IAcadDocument_GetVariable(This,Name,Value)	\
    (This)->lpVtbl -> GetVariable(This,Name,Value)

#define IAcadDocument_SetVariable(This,Name,Value)	\
    (This)->lpVtbl -> SetVariable(This,Name,Value)

#define IAcadDocument_LoadShapeFile(This,FullName)	\
    (This)->lpVtbl -> LoadShapeFile(This,FullName)

#define IAcadDocument_Regen(This,WhichViewports)	\
    (This)->lpVtbl -> Regen(This,WhichViewports)

#define IAcadDocument_get_PickfirstSelectionSet(This,pSelSet)	\
    (This)->lpVtbl -> get_PickfirstSelectionSet(This,pSelSet)

#define IAcadDocument_get_Active(This,pvbActive)	\
    (This)->lpVtbl -> get_Active(This,pvbActive)

#define IAcadDocument_Activate(This)	\
    (This)->lpVtbl -> Activate(This)

#define IAcadDocument_Close(This,SaveChanges,FileName)	\
    (This)->lpVtbl -> Close(This,SaveChanges,FileName)

#define IAcadDocument_put_WindowState(This,pWinState)	\
    (This)->lpVtbl -> put_WindowState(This,pWinState)

#define IAcadDocument_get_WindowState(This,pWinState)	\
    (This)->lpVtbl -> get_WindowState(This,pWinState)

#define IAcadDocument_put_Width(This,pWidth)	\
    (This)->lpVtbl -> put_Width(This,pWidth)

#define IAcadDocument_get_Width(This,pWidth)	\
    (This)->lpVtbl -> get_Width(This,pWidth)

#define IAcadDocument_put_Height(This,pHeight)	\
    (This)->lpVtbl -> put_Height(This,pHeight)

#define IAcadDocument_get_Height(This,pHeight)	\
    (This)->lpVtbl -> get_Height(This,pHeight)

#define IAcadDocument_put_ActiveLayout(This,pLayout)	\
    (This)->lpVtbl -> put_ActiveLayout(This,pLayout)

#define IAcadDocument_get_ActiveLayout(This,pLayout)	\
    (This)->lpVtbl -> get_ActiveLayout(This,pLayout)

#define IAcadDocument_SendCommand(This,Command)	\
    (This)->lpVtbl -> SendCommand(This,Command)

#define IAcadDocument_get_HWND(This,HWND)	\
    (This)->lpVtbl -> get_HWND(This,HWND)

#define IAcadDocument_get_WindowTitle(This,Title)	\
    (This)->lpVtbl -> get_WindowTitle(This,Title)

#define IAcadDocument_get_Application(This,pAppObj)	\
    (This)->lpVtbl -> get_Application(This,pAppObj)

#define IAcadDocument_get_Database(This,pDatabase)	\
    (This)->lpVtbl -> get_Database(This,pDatabase)

#define IAcadDocument_StartUndoMark(This)	\
    (This)->lpVtbl -> StartUndoMark(This)

#define IAcadDocument_EndUndoMark(This)	\
    (This)->lpVtbl -> EndUndoMark(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_get_Plot_Proxy( 
    IAcadDocument * This,
    /* [retval][out] */ IDispatch **pPlot);


void __RPC_STUB IAcadDocument_get_Plot_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_get_ActiveLayer_Proxy( 
    IAcadDocument * This,
    /* [retval][out] */ IAcadLayer **pActLayer);


void __RPC_STUB IAcadDocument_get_ActiveLayer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_put_ActiveLayer_Proxy( 
    IAcadDocument * This,
    /* [in] */ IAcadLayer *pActLayer);


void __RPC_STUB IAcadDocument_put_ActiveLayer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_get_ActiveLinetype_Proxy( 
    IAcadDocument * This,
    /* [retval][out] */ IAcadLineType **pActLinetype);


void __RPC_STUB IAcadDocument_get_ActiveLinetype_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_put_ActiveLinetype_Proxy( 
    IAcadDocument * This,
    /* [in] */ IAcadLineType *pActLinetype);


void __RPC_STUB IAcadDocument_put_ActiveLinetype_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_get_ActiveDimStyle_Proxy( 
    IAcadDocument * This,
    /* [retval][out] */ IAcadDimStyle **pActDimStyle);


void __RPC_STUB IAcadDocument_get_ActiveDimStyle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_put_ActiveDimStyle_Proxy( 
    IAcadDocument * This,
    /* [in] */ IAcadDimStyle *pActDimStyle);


void __RPC_STUB IAcadDocument_put_ActiveDimStyle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_get_ActiveTextStyle_Proxy( 
    IAcadDocument * This,
    /* [retval][out] */ IAcadTextStyle **pActTextStyle);


void __RPC_STUB IAcadDocument_get_ActiveTextStyle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_put_ActiveTextStyle_Proxy( 
    IAcadDocument * This,
    /* [in] */ IAcadTextStyle *pActTextStyle);


void __RPC_STUB IAcadDocument_put_ActiveTextStyle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_get_ActiveUCS_Proxy( 
    IAcadDocument * This,
    /* [retval][out] */ IAcadUCS **pActUCS);


void __RPC_STUB IAcadDocument_get_ActiveUCS_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_put_ActiveUCS_Proxy( 
    IAcadDocument * This,
    /* [in] */ IAcadUCS *pActUCS);


void __RPC_STUB IAcadDocument_put_ActiveUCS_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_get_ActiveViewport_Proxy( 
    IAcadDocument * This,
    /* [retval][out] */ IAcadViewport **pActView);


void __RPC_STUB IAcadDocument_get_ActiveViewport_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_put_ActiveViewport_Proxy( 
    IAcadDocument * This,
    /* [in] */ IAcadViewport *pActView);


void __RPC_STUB IAcadDocument_put_ActiveViewport_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_get_ActivePViewport_Proxy( 
    IAcadDocument * This,
    /* [retval][out] */ IAcadPViewport **pActView);


void __RPC_STUB IAcadDocument_get_ActivePViewport_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_put_ActivePViewport_Proxy( 
    IAcadDocument * This,
    /* [in] */ IAcadPViewport *pActView);


void __RPC_STUB IAcadDocument_put_ActivePViewport_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_get_ActiveSpace_Proxy( 
    IAcadDocument * This,
    /* [retval][out] */ AcActiveSpace *ActSpace);


void __RPC_STUB IAcadDocument_get_ActiveSpace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_put_ActiveSpace_Proxy( 
    IAcadDocument * This,
    /* [in] */ AcActiveSpace ActSpace);


void __RPC_STUB IAcadDocument_put_ActiveSpace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_get_SelectionSets_Proxy( 
    IAcadDocument * This,
    /* [retval][out] */ IAcadSelectionSets **pSelSets);


void __RPC_STUB IAcadDocument_get_SelectionSets_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_get_ActiveSelectionSet_Proxy( 
    IAcadDocument * This,
    /* [retval][out] */ IAcadSelectionSet **pSelSet);


void __RPC_STUB IAcadDocument_get_ActiveSelectionSet_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_get_FullName_Proxy( 
    IAcadDocument * This,
    /* [retval][out] */ BSTR *FullName);


void __RPC_STUB IAcadDocument_get_FullName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_get_Name_Proxy( 
    IAcadDocument * This,
    /* [retval][out] */ BSTR *Name);


void __RPC_STUB IAcadDocument_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_get_Path_Proxy( 
    IAcadDocument * This,
    /* [retval][out] */ BSTR *Path);


void __RPC_STUB IAcadDocument_get_Path_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_get_ObjectSnapMode_Proxy( 
    IAcadDocument * This,
    /* [retval][out] */ VARIANT_BOOL *fSnapMode);


void __RPC_STUB IAcadDocument_get_ObjectSnapMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_put_ObjectSnapMode_Proxy( 
    IAcadDocument * This,
    /* [in] */ VARIANT_BOOL fSnapMode);


void __RPC_STUB IAcadDocument_put_ObjectSnapMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_get_ReadOnly_Proxy( 
    IAcadDocument * This,
    /* [retval][out] */ VARIANT_BOOL *bReadOnly);


void __RPC_STUB IAcadDocument_get_ReadOnly_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_get_Saved_Proxy( 
    IAcadDocument * This,
    /* [retval][out] */ VARIANT_BOOL *bSaved);


void __RPC_STUB IAcadDocument_get_Saved_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_get_MSpace_Proxy( 
    IAcadDocument * This,
    /* [retval][out] */ VARIANT_BOOL *Mode);


void __RPC_STUB IAcadDocument_get_MSpace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_put_MSpace_Proxy( 
    IAcadDocument * This,
    /* [in] */ VARIANT_BOOL Mode);


void __RPC_STUB IAcadDocument_put_MSpace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_get_Utility_Proxy( 
    IAcadDocument * This,
    /* [retval][out] */ IAcadUtility **pUtil);


void __RPC_STUB IAcadDocument_get_Utility_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_Open_Proxy( 
    IAcadDocument * This,
    /* [in] */ BSTR FullName,
    /* [optional][in] */ VARIANT Password,
    /* [retval][out] */ IAcadDocument **pDocObj);


void __RPC_STUB IAcadDocument_Open_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_AuditInfo_Proxy( 
    IAcadDocument * This,
    /* [in] */ VARIANT_BOOL FixErr);


void __RPC_STUB IAcadDocument_AuditInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_Import_Proxy( 
    IAcadDocument * This,
    /* [in] */ BSTR FileName,
    /* [in] */ VARIANT InsertionPoint,
    /* [in] */ double ScaleFactor,
    /* [retval][out] */ IDispatch **pObj);


void __RPC_STUB IAcadDocument_Import_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_Export_Proxy( 
    IAcadDocument * This,
    /* [in] */ BSTR FileName,
    /* [in] */ BSTR Extension,
    /* [in] */ IAcadSelectionSet *SelectionSet);


void __RPC_STUB IAcadDocument_Export_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_New_Proxy( 
    IAcadDocument * This,
    /* [in] */ BSTR TemplateFileName,
    /* [retval][out] */ IAcadDocument **pDocObj);


void __RPC_STUB IAcadDocument_New_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_Save_Proxy( 
    IAcadDocument * This);


void __RPC_STUB IAcadDocument_Save_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_SaveAs_Proxy( 
    IAcadDocument * This,
    /* [in] */ BSTR FullFileName,
    /* [optional][in] */ VARIANT SaveAsType,
    /* [optional][in] */ VARIANT vSecurityParams);


void __RPC_STUB IAcadDocument_SaveAs_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_Wblock_Proxy( 
    IAcadDocument * This,
    /* [in] */ BSTR FileName,
    /* [in] */ IAcadSelectionSet *SelectionSet);


void __RPC_STUB IAcadDocument_Wblock_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_PurgeAll_Proxy( 
    IAcadDocument * This);


void __RPC_STUB IAcadDocument_PurgeAll_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_GetVariable_Proxy( 
    IAcadDocument * This,
    /* [in] */ BSTR Name,
    /* [retval][out] */ VARIANT *Value);


void __RPC_STUB IAcadDocument_GetVariable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_SetVariable_Proxy( 
    IAcadDocument * This,
    /* [in] */ BSTR Name,
    /* [in] */ VARIANT Value);


void __RPC_STUB IAcadDocument_SetVariable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_LoadShapeFile_Proxy( 
    IAcadDocument * This,
    /* [in] */ BSTR FullName);


void __RPC_STUB IAcadDocument_LoadShapeFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_Regen_Proxy( 
    IAcadDocument * This,
    /* [in] */ AcRegenType WhichViewports);


void __RPC_STUB IAcadDocument_Regen_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_get_PickfirstSelectionSet_Proxy( 
    IAcadDocument * This,
    /* [retval][out] */ IDispatch **pSelSet);


void __RPC_STUB IAcadDocument_get_PickfirstSelectionSet_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_get_Active_Proxy( 
    IAcadDocument * This,
    /* [retval][out] */ VARIANT_BOOL *pvbActive);


void __RPC_STUB IAcadDocument_get_Active_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_Activate_Proxy( 
    IAcadDocument * This);


void __RPC_STUB IAcadDocument_Activate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_Close_Proxy( 
    IAcadDocument * This,
    /* [optional][in] */ VARIANT SaveChanges,
    /* [optional][in] */ VARIANT FileName);


void __RPC_STUB IAcadDocument_Close_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_put_WindowState_Proxy( 
    IAcadDocument * This,
    /* [in] */ AcWindowState pWinState);


void __RPC_STUB IAcadDocument_put_WindowState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_get_WindowState_Proxy( 
    IAcadDocument * This,
    /* [retval][out] */ AcWindowState *pWinState);


void __RPC_STUB IAcadDocument_get_WindowState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_put_Width_Proxy( 
    IAcadDocument * This,
    /* [in] */ int pWidth);


void __RPC_STUB IAcadDocument_put_Width_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_get_Width_Proxy( 
    IAcadDocument * This,
    /* [retval][out] */ int *pWidth);


void __RPC_STUB IAcadDocument_get_Width_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_put_Height_Proxy( 
    IAcadDocument * This,
    /* [in] */ int pHeight);


void __RPC_STUB IAcadDocument_put_Height_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_get_Height_Proxy( 
    IAcadDocument * This,
    /* [retval][out] */ int *pHeight);


void __RPC_STUB IAcadDocument_get_Height_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_put_ActiveLayout_Proxy( 
    IAcadDocument * This,
    /* [in] */ IAcadLayout *pLayout);


void __RPC_STUB IAcadDocument_put_ActiveLayout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_get_ActiveLayout_Proxy( 
    IAcadDocument * This,
    /* [retval][out] */ IAcadLayout **pLayout);


void __RPC_STUB IAcadDocument_get_ActiveLayout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_SendCommand_Proxy( 
    IAcadDocument * This,
    /* [in] */ BSTR Command);


void __RPC_STUB IAcadDocument_SendCommand_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_get_HWND_Proxy( 
    IAcadDocument * This,
    /* [retval][out] */ long *HWND);


void __RPC_STUB IAcadDocument_get_HWND_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_get_WindowTitle_Proxy( 
    IAcadDocument * This,
    /* [retval][out] */ BSTR *Title);


void __RPC_STUB IAcadDocument_get_WindowTitle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_get_Application_Proxy( 
    IAcadDocument * This,
    /* [retval][out] */ IAcadApplication **pAppObj);


void __RPC_STUB IAcadDocument_get_Application_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_get_Database_Proxy( 
    IAcadDocument * This,
    /* [retval][out] */ IAcadDatabase **pDatabase);


void __RPC_STUB IAcadDocument_get_Database_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_StartUndoMark_Proxy( 
    IAcadDocument * This);


void __RPC_STUB IAcadDocument_StartUndoMark_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadDocument_EndUndoMark_Proxy( 
    IAcadDocument * This);


void __RPC_STUB IAcadDocument_EndUndoMark_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IAcadDocument_INTERFACE_DEFINED__ */


#ifndef __IAcadDocuments_INTERFACE_DEFINED__
#define __IAcadDocuments_INTERFACE_DEFINED__

/* interface IAcadDocuments */
/* [oleautomation][dual][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IAcadDocuments;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("CE3683C2-A49B-4AE7-AC30-E4968CC70ED4")
    IAcadDocuments : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Item( 
            /* [in] */ VARIANT Index,
            /* [retval][out] */ IAcadDocument **pItem) = 0;
        
        virtual /* [hidden][restricted][propget][id] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown **pEnumVariant) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long *Count) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Application( 
            /* [retval][out] */ IAcadApplication **pAppObj) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Add( 
            /* [optional][in] */ VARIANT TemplateName,
            /* [retval][out] */ IAcadDocument **pDispDoc) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Open( 
            /* [in] */ BSTR Name,
            /* [optional][in] */ VARIANT ReadOnly,
            /* [optional][in] */ VARIANT Password,
            /* [retval][out] */ IAcadDocument **pDispDoc) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Close( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAcadDocumentsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAcadDocuments * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAcadDocuments * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAcadDocuments * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IAcadDocuments * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IAcadDocuments * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IAcadDocuments * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IAcadDocuments * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Item )( 
            IAcadDocuments * This,
            /* [in] */ VARIANT Index,
            /* [retval][out] */ IAcadDocument **pItem);
        
        /* [hidden][restricted][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get__NewEnum )( 
            IAcadDocuments * This,
            /* [retval][out] */ IUnknown **pEnumVariant);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            IAcadDocuments * This,
            /* [retval][out] */ long *Count);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Application )( 
            IAcadDocuments * This,
            /* [retval][out] */ IAcadApplication **pAppObj);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Add )( 
            IAcadDocuments * This,
            /* [optional][in] */ VARIANT TemplateName,
            /* [retval][out] */ IAcadDocument **pDispDoc);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Open )( 
            IAcadDocuments * This,
            /* [in] */ BSTR Name,
            /* [optional][in] */ VARIANT ReadOnly,
            /* [optional][in] */ VARIANT Password,
            /* [retval][out] */ IAcadDocument **pDispDoc);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Close )( 
            IAcadDocuments * This);
        
        END_INTERFACE
    } IAcadDocumentsVtbl;

    interface IAcadDocuments
    {
        CONST_VTBL struct IAcadDocumentsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAcadDocuments_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAcadDocuments_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAcadDocuments_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAcadDocuments_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IAcadDocuments_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IAcadDocuments_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IAcadDocuments_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IAcadDocuments_Item(This,Index,pItem)	\
    (This)->lpVtbl -> Item(This,Index,pItem)

#define IAcadDocuments_get__NewEnum(This,pEnumVariant)	\
    (This)->lpVtbl -> get__NewEnum(This,pEnumVariant)

#define IAcadDocuments_get_Count(This,Count)	\
    (This)->lpVtbl -> get_Count(This,Count)

#define IAcadDocuments_get_Application(This,pAppObj)	\
    (This)->lpVtbl -> get_Application(This,pAppObj)

#define IAcadDocuments_Add(This,TemplateName,pDispDoc)	\
    (This)->lpVtbl -> Add(This,TemplateName,pDispDoc)

#define IAcadDocuments_Open(This,Name,ReadOnly,Password,pDispDoc)	\
    (This)->lpVtbl -> Open(This,Name,ReadOnly,Password,pDispDoc)

#define IAcadDocuments_Close(This)	\
    (This)->lpVtbl -> Close(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadDocuments_Item_Proxy( 
    IAcadDocuments * This,
    /* [in] */ VARIANT Index,
    /* [retval][out] */ IAcadDocument **pItem);


void __RPC_STUB IAcadDocuments_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden][restricted][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocuments_get__NewEnum_Proxy( 
    IAcadDocuments * This,
    /* [retval][out] */ IUnknown **pEnumVariant);


void __RPC_STUB IAcadDocuments_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocuments_get_Count_Proxy( 
    IAcadDocuments * This,
    /* [retval][out] */ long *Count);


void __RPC_STUB IAcadDocuments_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadDocuments_get_Application_Proxy( 
    IAcadDocuments * This,
    /* [retval][out] */ IAcadApplication **pAppObj);


void __RPC_STUB IAcadDocuments_get_Application_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadDocuments_Add_Proxy( 
    IAcadDocuments * This,
    /* [optional][in] */ VARIANT TemplateName,
    /* [retval][out] */ IAcadDocument **pDispDoc);


void __RPC_STUB IAcadDocuments_Add_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadDocuments_Open_Proxy( 
    IAcadDocuments * This,
    /* [in] */ BSTR Name,
    /* [optional][in] */ VARIANT ReadOnly,
    /* [optional][in] */ VARIANT Password,
    /* [retval][out] */ IAcadDocument **pDispDoc);


void __RPC_STUB IAcadDocuments_Open_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadDocuments_Close_Proxy( 
    IAcadDocuments * This);


void __RPC_STUB IAcadDocuments_Close_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IAcadDocuments_INTERFACE_DEFINED__ */


#ifndef __IAcadSelectionSets_INTERFACE_DEFINED__
#define __IAcadSelectionSets_INTERFACE_DEFINED__

/* interface IAcadSelectionSets */
/* [object][oleautomation][dual][helpstring][uuid] */ 


EXTERN_C const IID IID_IAcadSelectionSets;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6CD414B6-7119-426F-B8A0-000855AF8D60")
    IAcadSelectionSets : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Item( 
            /* [in] */ VARIANT Index,
            /* [retval][out] */ IAcadSelectionSet **pItem) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [hidden][restricted][propget][id] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown **pVal) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Application( 
            /* [retval][out] */ IAcadApplication **pAppObj) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Add( 
            /* [in] */ BSTR Name,
            /* [retval][out] */ IAcadSelectionSet **pSet) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAcadSelectionSetsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAcadSelectionSets * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAcadSelectionSets * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAcadSelectionSets * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IAcadSelectionSets * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IAcadSelectionSets * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IAcadSelectionSets * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IAcadSelectionSets * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Item )( 
            IAcadSelectionSets * This,
            /* [in] */ VARIANT Index,
            /* [retval][out] */ IAcadSelectionSet **pItem);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            IAcadSelectionSets * This,
            /* [retval][out] */ long *pVal);
        
        /* [hidden][restricted][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get__NewEnum )( 
            IAcadSelectionSets * This,
            /* [retval][out] */ IUnknown **pVal);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Application )( 
            IAcadSelectionSets * This,
            /* [retval][out] */ IAcadApplication **pAppObj);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Add )( 
            IAcadSelectionSets * This,
            /* [in] */ BSTR Name,
            /* [retval][out] */ IAcadSelectionSet **pSet);
        
        END_INTERFACE
    } IAcadSelectionSetsVtbl;

    interface IAcadSelectionSets
    {
        CONST_VTBL struct IAcadSelectionSetsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAcadSelectionSets_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAcadSelectionSets_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAcadSelectionSets_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAcadSelectionSets_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IAcadSelectionSets_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IAcadSelectionSets_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IAcadSelectionSets_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IAcadSelectionSets_Item(This,Index,pItem)	\
    (This)->lpVtbl -> Item(This,Index,pItem)

#define IAcadSelectionSets_get_Count(This,pVal)	\
    (This)->lpVtbl -> get_Count(This,pVal)

#define IAcadSelectionSets_get__NewEnum(This,pVal)	\
    (This)->lpVtbl -> get__NewEnum(This,pVal)

#define IAcadSelectionSets_get_Application(This,pAppObj)	\
    (This)->lpVtbl -> get_Application(This,pAppObj)

#define IAcadSelectionSets_Add(This,Name,pSet)	\
    (This)->lpVtbl -> Add(This,Name,pSet)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadSelectionSets_Item_Proxy( 
    IAcadSelectionSets * This,
    /* [in] */ VARIANT Index,
    /* [retval][out] */ IAcadSelectionSet **pItem);


void __RPC_STUB IAcadSelectionSets_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadSelectionSets_get_Count_Proxy( 
    IAcadSelectionSets * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB IAcadSelectionSets_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden][restricted][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadSelectionSets_get__NewEnum_Proxy( 
    IAcadSelectionSets * This,
    /* [retval][out] */ IUnknown **pVal);


void __RPC_STUB IAcadSelectionSets_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadSelectionSets_get_Application_Proxy( 
    IAcadSelectionSets * This,
    /* [retval][out] */ IAcadApplication **pAppObj);


void __RPC_STUB IAcadSelectionSets_get_Application_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadSelectionSets_Add_Proxy( 
    IAcadSelectionSets * This,
    /* [in] */ BSTR Name,
    /* [retval][out] */ IAcadSelectionSet **pSet);


void __RPC_STUB IAcadSelectionSets_Add_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IAcadSelectionSets_INTERFACE_DEFINED__ */


#ifndef __IAcadSelectionSet_INTERFACE_DEFINED__
#define __IAcadSelectionSet_INTERFACE_DEFINED__

/* interface IAcadSelectionSet */
/* [object][oleautomation][dual][helpstring][uuid] */ 


EXTERN_C const IID IID_IAcadSelectionSet;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6729D2C8-E1AF-4248-914D-F208A0E05E84")
    IAcadSelectionSet : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Item( 
            /* [in] */ VARIANT Index,
            /* [retval][out] */ IAcadEntity **pEntity) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [hidden][restricted][propget][id] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown **pVal) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR *bstrName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Highlight( 
            /* [in] */ VARIANT_BOOL bFlag) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Erase( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Update( void) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Application( 
            /* [retval][out] */ IAcadApplication **pAppObj) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddItems( 
            /* [in] */ VARIANT pSelSet) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RemoveItems( 
            /* [in] */ VARIANT Objects) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Clear( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Select( 
            /* [in] */ AcSelect Mode,
            /* [optional][in] */ VARIANT Point1,
            /* [optional][in] */ VARIANT Point2,
            /* [optional][in] */ VARIANT FilterType,
            /* [optional][in] */ VARIANT FilterData) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SelectAtPoint( 
            /* [in] */ VARIANT Point,
            /* [optional][in] */ VARIANT FilterType,
            /* [optional][in] */ VARIANT FilterData) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SelectByPolygon( 
            /* [in] */ AcSelect Mode,
            /* [in] */ VARIANT PointsList,
            /* [optional][in] */ VARIANT FilterType,
            /* [optional][in] */ VARIANT FilterData) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SelectOnScreen( 
            /* [optional][in] */ VARIANT FilterType,
            /* [optional][in] */ VARIANT FilterData) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Delete( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAcadSelectionSetVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAcadSelectionSet * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAcadSelectionSet * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAcadSelectionSet * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IAcadSelectionSet * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IAcadSelectionSet * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IAcadSelectionSet * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IAcadSelectionSet * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Item )( 
            IAcadSelectionSet * This,
            /* [in] */ VARIANT Index,
            /* [retval][out] */ IAcadEntity **pEntity);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            IAcadSelectionSet * This,
            /* [retval][out] */ long *pVal);
        
        /* [hidden][restricted][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get__NewEnum )( 
            IAcadSelectionSet * This,
            /* [retval][out] */ IUnknown **pVal);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            IAcadSelectionSet * This,
            /* [retval][out] */ BSTR *bstrName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Highlight )( 
            IAcadSelectionSet * This,
            /* [in] */ VARIANT_BOOL bFlag);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Erase )( 
            IAcadSelectionSet * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Update )( 
            IAcadSelectionSet * This);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Application )( 
            IAcadSelectionSet * This,
            /* [retval][out] */ IAcadApplication **pAppObj);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AddItems )( 
            IAcadSelectionSet * This,
            /* [in] */ VARIANT pSelSet);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RemoveItems )( 
            IAcadSelectionSet * This,
            /* [in] */ VARIANT Objects);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Clear )( 
            IAcadSelectionSet * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Select )( 
            IAcadSelectionSet * This,
            /* [in] */ AcSelect Mode,
            /* [optional][in] */ VARIANT Point1,
            /* [optional][in] */ VARIANT Point2,
            /* [optional][in] */ VARIANT FilterType,
            /* [optional][in] */ VARIANT FilterData);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SelectAtPoint )( 
            IAcadSelectionSet * This,
            /* [in] */ VARIANT Point,
            /* [optional][in] */ VARIANT FilterType,
            /* [optional][in] */ VARIANT FilterData);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SelectByPolygon )( 
            IAcadSelectionSet * This,
            /* [in] */ AcSelect Mode,
            /* [in] */ VARIANT PointsList,
            /* [optional][in] */ VARIANT FilterType,
            /* [optional][in] */ VARIANT FilterData);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SelectOnScreen )( 
            IAcadSelectionSet * This,
            /* [optional][in] */ VARIANT FilterType,
            /* [optional][in] */ VARIANT FilterData);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Delete )( 
            IAcadSelectionSet * This);
        
        END_INTERFACE
    } IAcadSelectionSetVtbl;

    interface IAcadSelectionSet
    {
        CONST_VTBL struct IAcadSelectionSetVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAcadSelectionSet_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAcadSelectionSet_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAcadSelectionSet_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAcadSelectionSet_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IAcadSelectionSet_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IAcadSelectionSet_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IAcadSelectionSet_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IAcadSelectionSet_Item(This,Index,pEntity)	\
    (This)->lpVtbl -> Item(This,Index,pEntity)

#define IAcadSelectionSet_get_Count(This,pVal)	\
    (This)->lpVtbl -> get_Count(This,pVal)

#define IAcadSelectionSet_get__NewEnum(This,pVal)	\
    (This)->lpVtbl -> get__NewEnum(This,pVal)

#define IAcadSelectionSet_get_Name(This,bstrName)	\
    (This)->lpVtbl -> get_Name(This,bstrName)

#define IAcadSelectionSet_Highlight(This,bFlag)	\
    (This)->lpVtbl -> Highlight(This,bFlag)

#define IAcadSelectionSet_Erase(This)	\
    (This)->lpVtbl -> Erase(This)

#define IAcadSelectionSet_Update(This)	\
    (This)->lpVtbl -> Update(This)

#define IAcadSelectionSet_get_Application(This,pAppObj)	\
    (This)->lpVtbl -> get_Application(This,pAppObj)

#define IAcadSelectionSet_AddItems(This,pSelSet)	\
    (This)->lpVtbl -> AddItems(This,pSelSet)

#define IAcadSelectionSet_RemoveItems(This,Objects)	\
    (This)->lpVtbl -> RemoveItems(This,Objects)

#define IAcadSelectionSet_Clear(This)	\
    (This)->lpVtbl -> Clear(This)

#define IAcadSelectionSet_Select(This,Mode,Point1,Point2,FilterType,FilterData)	\
    (This)->lpVtbl -> Select(This,Mode,Point1,Point2,FilterType,FilterData)

#define IAcadSelectionSet_SelectAtPoint(This,Point,FilterType,FilterData)	\
    (This)->lpVtbl -> SelectAtPoint(This,Point,FilterType,FilterData)

#define IAcadSelectionSet_SelectByPolygon(This,Mode,PointsList,FilterType,FilterData)	\
    (This)->lpVtbl -> SelectByPolygon(This,Mode,PointsList,FilterType,FilterData)

#define IAcadSelectionSet_SelectOnScreen(This,FilterType,FilterData)	\
    (This)->lpVtbl -> SelectOnScreen(This,FilterType,FilterData)

#define IAcadSelectionSet_Delete(This)	\
    (This)->lpVtbl -> Delete(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadSelectionSet_Item_Proxy( 
    IAcadSelectionSet * This,
    /* [in] */ VARIANT Index,
    /* [retval][out] */ IAcadEntity **pEntity);


void __RPC_STUB IAcadSelectionSet_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadSelectionSet_get_Count_Proxy( 
    IAcadSelectionSet * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB IAcadSelectionSet_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden][restricted][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadSelectionSet_get__NewEnum_Proxy( 
    IAcadSelectionSet * This,
    /* [retval][out] */ IUnknown **pVal);


void __RPC_STUB IAcadSelectionSet_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadSelectionSet_get_Name_Proxy( 
    IAcadSelectionSet * This,
    /* [retval][out] */ BSTR *bstrName);


void __RPC_STUB IAcadSelectionSet_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadSelectionSet_Highlight_Proxy( 
    IAcadSelectionSet * This,
    /* [in] */ VARIANT_BOOL bFlag);


void __RPC_STUB IAcadSelectionSet_Highlight_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadSelectionSet_Erase_Proxy( 
    IAcadSelectionSet * This);


void __RPC_STUB IAcadSelectionSet_Erase_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadSelectionSet_Update_Proxy( 
    IAcadSelectionSet * This);


void __RPC_STUB IAcadSelectionSet_Update_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadSelectionSet_get_Application_Proxy( 
    IAcadSelectionSet * This,
    /* [retval][out] */ IAcadApplication **pAppObj);


void __RPC_STUB IAcadSelectionSet_get_Application_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadSelectionSet_AddItems_Proxy( 
    IAcadSelectionSet * This,
    /* [in] */ VARIANT pSelSet);


void __RPC_STUB IAcadSelectionSet_AddItems_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadSelectionSet_RemoveItems_Proxy( 
    IAcadSelectionSet * This,
    /* [in] */ VARIANT Objects);


void __RPC_STUB IAcadSelectionSet_RemoveItems_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadSelectionSet_Clear_Proxy( 
    IAcadSelectionSet * This);


void __RPC_STUB IAcadSelectionSet_Clear_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadSelectionSet_Select_Proxy( 
    IAcadSelectionSet * This,
    /* [in] */ AcSelect Mode,
    /* [optional][in] */ VARIANT Point1,
    /* [optional][in] */ VARIANT Point2,
    /* [optional][in] */ VARIANT FilterType,
    /* [optional][in] */ VARIANT FilterData);


void __RPC_STUB IAcadSelectionSet_Select_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadSelectionSet_SelectAtPoint_Proxy( 
    IAcadSelectionSet * This,
    /* [in] */ VARIANT Point,
    /* [optional][in] */ VARIANT FilterType,
    /* [optional][in] */ VARIANT FilterData);


void __RPC_STUB IAcadSelectionSet_SelectAtPoint_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadSelectionSet_SelectByPolygon_Proxy( 
    IAcadSelectionSet * This,
    /* [in] */ AcSelect Mode,
    /* [in] */ VARIANT PointsList,
    /* [optional][in] */ VARIANT FilterType,
    /* [optional][in] */ VARIANT FilterData);


void __RPC_STUB IAcadSelectionSet_SelectByPolygon_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadSelectionSet_SelectOnScreen_Proxy( 
    IAcadSelectionSet * This,
    /* [optional][in] */ VARIANT FilterType,
    /* [optional][in] */ VARIANT FilterData);


void __RPC_STUB IAcadSelectionSet_SelectOnScreen_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadSelectionSet_Delete_Proxy( 
    IAcadSelectionSet * This);


void __RPC_STUB IAcadSelectionSet_Delete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IAcadSelectionSet_INTERFACE_DEFINED__ */


#ifndef __IAcadPreferences_INTERFACE_DEFINED__
#define __IAcadPreferences_INTERFACE_DEFINED__

/* interface IAcadPreferences */
/* [object][oleautomation][dual][helpstring][uuid] */ 


EXTERN_C const IID IID_IAcadPreferences;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("498902DC-F5F2-4D4C-948C-D19B645F0C7F")
    IAcadPreferences : public IDispatch
    {
    public:
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Application( 
            /* [retval][out] */ IAcadApplication **pAppObj) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Files( 
            /* [retval][out] */ IAcadPreferencesFiles **pObj) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAcadPreferencesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAcadPreferences * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAcadPreferences * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAcadPreferences * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IAcadPreferences * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IAcadPreferences * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IAcadPreferences * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IAcadPreferences * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Application )( 
            IAcadPreferences * This,
            /* [retval][out] */ IAcadApplication **pAppObj);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Files )( 
            IAcadPreferences * This,
            /* [retval][out] */ IAcadPreferencesFiles **pObj);
        
        END_INTERFACE
    } IAcadPreferencesVtbl;

    interface IAcadPreferences
    {
        CONST_VTBL struct IAcadPreferencesVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAcadPreferences_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAcadPreferences_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAcadPreferences_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAcadPreferences_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IAcadPreferences_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IAcadPreferences_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IAcadPreferences_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IAcadPreferences_get_Application(This,pAppObj)	\
    (This)->lpVtbl -> get_Application(This,pAppObj)

#define IAcadPreferences_get_Files(This,pObj)	\
    (This)->lpVtbl -> get_Files(This,pObj)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferences_get_Application_Proxy( 
    IAcadPreferences * This,
    /* [retval][out] */ IAcadApplication **pAppObj);


void __RPC_STUB IAcadPreferences_get_Application_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferences_get_Files_Proxy( 
    IAcadPreferences * This,
    /* [retval][out] */ IAcadPreferencesFiles **pObj);


void __RPC_STUB IAcadPreferences_get_Files_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IAcadPreferences_INTERFACE_DEFINED__ */


#ifndef __IAcadPreferencesFiles_INTERFACE_DEFINED__
#define __IAcadPreferencesFiles_INTERFACE_DEFINED__

/* interface IAcadPreferencesFiles */
/* [object][oleautomation][dual][helpstring][uuid] */ 


EXTERN_C const IID IID_IAcadPreferencesFiles;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2443685E-ED1D-4753-AA91-963CB86AE095")
    IAcadPreferencesFiles : public IDispatch
    {
    public:
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Application( 
            /* [retval][out] */ IAcadApplication **pAppObj) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_SupportPath( 
            /* [in] */ BSTR orient) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_SupportPath( 
            /* [retval][out] */ BSTR *orient) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_DriversPath( 
            /* [in] */ BSTR Path) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_DriversPath( 
            /* [retval][out] */ BSTR *Path) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_MenuFile( 
            /* [in] */ BSTR Path) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_MenuFile( 
            /* [retval][out] */ BSTR *Path) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_HelpFilePath( 
            /* [in] */ BSTR Path) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_HelpFilePath( 
            /* [retval][out] */ BSTR *Path) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_DefaultInternetURL( 
            /* [in] */ BSTR Path) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_DefaultInternetURL( 
            /* [retval][out] */ BSTR *Path) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_ConfigFile( 
            /* [retval][out] */ BSTR *ConfigFile) = 0;
        
        virtual /* [helpstring][hidden][propget][id] */ HRESULT STDMETHODCALLTYPE get_LicenseServer( 
            /* [retval][out] */ BSTR *Path) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_TextEditor( 
            /* [in] */ BSTR Path) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_TextEditor( 
            /* [retval][out] */ BSTR *Path) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_MainDictionary( 
            /* [in] */ BSTR Path) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_MainDictionary( 
            /* [retval][out] */ BSTR *Path) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_CustomDictionary( 
            /* [in] */ BSTR Path) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_CustomDictionary( 
            /* [retval][out] */ BSTR *Path) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_AltFontFile( 
            /* [in] */ BSTR fontFile) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_AltFontFile( 
            /* [retval][out] */ BSTR *fontFile) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_FontFileMap( 
            /* [in] */ BSTR Path) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_FontFileMap( 
            /* [retval][out] */ BSTR *Path) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_PrintFile( 
            /* [in] */ BSTR Path) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_PrintFile( 
            /* [retval][out] */ BSTR *Path) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_PrintSpoolExecutable( 
            /* [in] */ BSTR Path) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_PrintSpoolExecutable( 
            /* [retval][out] */ BSTR *Path) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_PostScriptPrologFile( 
            /* [in] */ BSTR Path) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_PostScriptPrologFile( 
            /* [retval][out] */ BSTR *Path) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_PrintSpoolerPath( 
            /* [in] */ BSTR Path) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_PrintSpoolerPath( 
            /* [retval][out] */ BSTR *Path) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_AutoSavePath( 
            /* [in] */ BSTR AutoSavePath) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_AutoSavePath( 
            /* [retval][out] */ BSTR *AutoSavePath) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_TemplateDwgPath( 
            /* [in] */ BSTR Path) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_TemplateDwgPath( 
            /* [retval][out] */ BSTR *Path) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_LogFilePath( 
            /* [in] */ BSTR Path) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_LogFilePath( 
            /* [retval][out] */ BSTR *Path) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_TempFilePath( 
            /* [in] */ BSTR Path) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_TempFilePath( 
            /* [retval][out] */ BSTR *Path) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_TempXrefPath( 
            /* [in] */ BSTR Path) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_TempXrefPath( 
            /* [retval][out] */ BSTR *Path) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_TextureMapPath( 
            /* [in] */ BSTR Path) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_TextureMapPath( 
            /* [retval][out] */ BSTR *Path) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_AltTabletMenuFile( 
            /* [in] */ BSTR MenuFile) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_AltTabletMenuFile( 
            /* [retval][out] */ BSTR *MenuFile) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetProjectFilePath( 
            /* [in] */ BSTR ProjectName,
            /* [in] */ BSTR ProjectFilePath) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetProjectFilePath( 
            /* [in] */ BSTR ProjectName,
            /* [retval][out] */ BSTR *Path) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_PrinterConfigPath( 
            /* [in] */ BSTR Path) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_PrinterConfigPath( 
            /* [retval][out] */ BSTR *Path) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_PrinterDescPath( 
            /* [in] */ BSTR Path) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_PrinterDescPath( 
            /* [retval][out] */ BSTR *Path) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_PrinterStyleSheetPath( 
            /* [in] */ BSTR Path) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_PrinterStyleSheetPath( 
            /* [retval][out] */ BSTR *Path) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_WorkspacePath( 
            /* [in] */ BSTR Path) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_WorkspacePath( 
            /* [retval][out] */ BSTR *Path) = 0;
        
        virtual /* [helpstring][hidden][propput][id] */ HRESULT STDMETHODCALLTYPE put_ObjectARXPath( 
            /* [in] */ BSTR Path) = 0;
        
        virtual /* [helpstring][hidden][propget][id] */ HRESULT STDMETHODCALLTYPE get_ObjectARXPath( 
            /* [retval][out] */ BSTR *Path) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_ColorBookPath( 
            /* [in] */ BSTR Path) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_ColorBookPath( 
            /* [retval][out] */ BSTR *Path) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_ToolPalettePath( 
            /* [in] */ BSTR Path) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_ToolPalettePath( 
            /* [retval][out] */ BSTR *Path) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAcadPreferencesFilesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAcadPreferencesFiles * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAcadPreferencesFiles * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAcadPreferencesFiles * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IAcadPreferencesFiles * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IAcadPreferencesFiles * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IAcadPreferencesFiles * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IAcadPreferencesFiles * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Application )( 
            IAcadPreferencesFiles * This,
            /* [retval][out] */ IAcadApplication **pAppObj);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_SupportPath )( 
            IAcadPreferencesFiles * This,
            /* [in] */ BSTR orient);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_SupportPath )( 
            IAcadPreferencesFiles * This,
            /* [retval][out] */ BSTR *orient);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_DriversPath )( 
            IAcadPreferencesFiles * This,
            /* [in] */ BSTR Path);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_DriversPath )( 
            IAcadPreferencesFiles * This,
            /* [retval][out] */ BSTR *Path);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_MenuFile )( 
            IAcadPreferencesFiles * This,
            /* [in] */ BSTR Path);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_MenuFile )( 
            IAcadPreferencesFiles * This,
            /* [retval][out] */ BSTR *Path);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_HelpFilePath )( 
            IAcadPreferencesFiles * This,
            /* [in] */ BSTR Path);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_HelpFilePath )( 
            IAcadPreferencesFiles * This,
            /* [retval][out] */ BSTR *Path);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_DefaultInternetURL )( 
            IAcadPreferencesFiles * This,
            /* [in] */ BSTR Path);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_DefaultInternetURL )( 
            IAcadPreferencesFiles * This,
            /* [retval][out] */ BSTR *Path);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_ConfigFile )( 
            IAcadPreferencesFiles * This,
            /* [retval][out] */ BSTR *ConfigFile);
        
        /* [helpstring][hidden][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_LicenseServer )( 
            IAcadPreferencesFiles * This,
            /* [retval][out] */ BSTR *Path);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_TextEditor )( 
            IAcadPreferencesFiles * This,
            /* [in] */ BSTR Path);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_TextEditor )( 
            IAcadPreferencesFiles * This,
            /* [retval][out] */ BSTR *Path);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_MainDictionary )( 
            IAcadPreferencesFiles * This,
            /* [in] */ BSTR Path);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_MainDictionary )( 
            IAcadPreferencesFiles * This,
            /* [retval][out] */ BSTR *Path);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_CustomDictionary )( 
            IAcadPreferencesFiles * This,
            /* [in] */ BSTR Path);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_CustomDictionary )( 
            IAcadPreferencesFiles * This,
            /* [retval][out] */ BSTR *Path);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_AltFontFile )( 
            IAcadPreferencesFiles * This,
            /* [in] */ BSTR fontFile);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_AltFontFile )( 
            IAcadPreferencesFiles * This,
            /* [retval][out] */ BSTR *fontFile);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_FontFileMap )( 
            IAcadPreferencesFiles * This,
            /* [in] */ BSTR Path);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_FontFileMap )( 
            IAcadPreferencesFiles * This,
            /* [retval][out] */ BSTR *Path);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_PrintFile )( 
            IAcadPreferencesFiles * This,
            /* [in] */ BSTR Path);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_PrintFile )( 
            IAcadPreferencesFiles * This,
            /* [retval][out] */ BSTR *Path);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_PrintSpoolExecutable )( 
            IAcadPreferencesFiles * This,
            /* [in] */ BSTR Path);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_PrintSpoolExecutable )( 
            IAcadPreferencesFiles * This,
            /* [retval][out] */ BSTR *Path);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_PostScriptPrologFile )( 
            IAcadPreferencesFiles * This,
            /* [in] */ BSTR Path);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_PostScriptPrologFile )( 
            IAcadPreferencesFiles * This,
            /* [retval][out] */ BSTR *Path);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_PrintSpoolerPath )( 
            IAcadPreferencesFiles * This,
            /* [in] */ BSTR Path);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_PrintSpoolerPath )( 
            IAcadPreferencesFiles * This,
            /* [retval][out] */ BSTR *Path);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_AutoSavePath )( 
            IAcadPreferencesFiles * This,
            /* [in] */ BSTR AutoSavePath);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_AutoSavePath )( 
            IAcadPreferencesFiles * This,
            /* [retval][out] */ BSTR *AutoSavePath);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_TemplateDwgPath )( 
            IAcadPreferencesFiles * This,
            /* [in] */ BSTR Path);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_TemplateDwgPath )( 
            IAcadPreferencesFiles * This,
            /* [retval][out] */ BSTR *Path);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_LogFilePath )( 
            IAcadPreferencesFiles * This,
            /* [in] */ BSTR Path);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_LogFilePath )( 
            IAcadPreferencesFiles * This,
            /* [retval][out] */ BSTR *Path);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_TempFilePath )( 
            IAcadPreferencesFiles * This,
            /* [in] */ BSTR Path);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_TempFilePath )( 
            IAcadPreferencesFiles * This,
            /* [retval][out] */ BSTR *Path);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_TempXrefPath )( 
            IAcadPreferencesFiles * This,
            /* [in] */ BSTR Path);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_TempXrefPath )( 
            IAcadPreferencesFiles * This,
            /* [retval][out] */ BSTR *Path);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_TextureMapPath )( 
            IAcadPreferencesFiles * This,
            /* [in] */ BSTR Path);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_TextureMapPath )( 
            IAcadPreferencesFiles * This,
            /* [retval][out] */ BSTR *Path);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_AltTabletMenuFile )( 
            IAcadPreferencesFiles * This,
            /* [in] */ BSTR MenuFile);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_AltTabletMenuFile )( 
            IAcadPreferencesFiles * This,
            /* [retval][out] */ BSTR *MenuFile);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetProjectFilePath )( 
            IAcadPreferencesFiles * This,
            /* [in] */ BSTR ProjectName,
            /* [in] */ BSTR ProjectFilePath);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetProjectFilePath )( 
            IAcadPreferencesFiles * This,
            /* [in] */ BSTR ProjectName,
            /* [retval][out] */ BSTR *Path);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_PrinterConfigPath )( 
            IAcadPreferencesFiles * This,
            /* [in] */ BSTR Path);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_PrinterConfigPath )( 
            IAcadPreferencesFiles * This,
            /* [retval][out] */ BSTR *Path);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_PrinterDescPath )( 
            IAcadPreferencesFiles * This,
            /* [in] */ BSTR Path);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_PrinterDescPath )( 
            IAcadPreferencesFiles * This,
            /* [retval][out] */ BSTR *Path);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_PrinterStyleSheetPath )( 
            IAcadPreferencesFiles * This,
            /* [in] */ BSTR Path);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_PrinterStyleSheetPath )( 
            IAcadPreferencesFiles * This,
            /* [retval][out] */ BSTR *Path);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_WorkspacePath )( 
            IAcadPreferencesFiles * This,
            /* [in] */ BSTR Path);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_WorkspacePath )( 
            IAcadPreferencesFiles * This,
            /* [retval][out] */ BSTR *Path);
        
        /* [helpstring][hidden][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_ObjectARXPath )( 
            IAcadPreferencesFiles * This,
            /* [in] */ BSTR Path);
        
        /* [helpstring][hidden][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_ObjectARXPath )( 
            IAcadPreferencesFiles * This,
            /* [retval][out] */ BSTR *Path);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_ColorBookPath )( 
            IAcadPreferencesFiles * This,
            /* [in] */ BSTR Path);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_ColorBookPath )( 
            IAcadPreferencesFiles * This,
            /* [retval][out] */ BSTR *Path);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_ToolPalettePath )( 
            IAcadPreferencesFiles * This,
            /* [in] */ BSTR Path);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_ToolPalettePath )( 
            IAcadPreferencesFiles * This,
            /* [retval][out] */ BSTR *Path);
        
        END_INTERFACE
    } IAcadPreferencesFilesVtbl;

    interface IAcadPreferencesFiles
    {
        CONST_VTBL struct IAcadPreferencesFilesVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAcadPreferencesFiles_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAcadPreferencesFiles_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAcadPreferencesFiles_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAcadPreferencesFiles_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IAcadPreferencesFiles_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IAcadPreferencesFiles_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IAcadPreferencesFiles_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IAcadPreferencesFiles_get_Application(This,pAppObj)	\
    (This)->lpVtbl -> get_Application(This,pAppObj)

#define IAcadPreferencesFiles_put_SupportPath(This,orient)	\
    (This)->lpVtbl -> put_SupportPath(This,orient)

#define IAcadPreferencesFiles_get_SupportPath(This,orient)	\
    (This)->lpVtbl -> get_SupportPath(This,orient)

#define IAcadPreferencesFiles_put_DriversPath(This,Path)	\
    (This)->lpVtbl -> put_DriversPath(This,Path)

#define IAcadPreferencesFiles_get_DriversPath(This,Path)	\
    (This)->lpVtbl -> get_DriversPath(This,Path)

#define IAcadPreferencesFiles_put_MenuFile(This,Path)	\
    (This)->lpVtbl -> put_MenuFile(This,Path)

#define IAcadPreferencesFiles_get_MenuFile(This,Path)	\
    (This)->lpVtbl -> get_MenuFile(This,Path)

#define IAcadPreferencesFiles_put_HelpFilePath(This,Path)	\
    (This)->lpVtbl -> put_HelpFilePath(This,Path)

#define IAcadPreferencesFiles_get_HelpFilePath(This,Path)	\
    (This)->lpVtbl -> get_HelpFilePath(This,Path)

#define IAcadPreferencesFiles_put_DefaultInternetURL(This,Path)	\
    (This)->lpVtbl -> put_DefaultInternetURL(This,Path)

#define IAcadPreferencesFiles_get_DefaultInternetURL(This,Path)	\
    (This)->lpVtbl -> get_DefaultInternetURL(This,Path)

#define IAcadPreferencesFiles_get_ConfigFile(This,ConfigFile)	\
    (This)->lpVtbl -> get_ConfigFile(This,ConfigFile)

#define IAcadPreferencesFiles_get_LicenseServer(This,Path)	\
    (This)->lpVtbl -> get_LicenseServer(This,Path)

#define IAcadPreferencesFiles_put_TextEditor(This,Path)	\
    (This)->lpVtbl -> put_TextEditor(This,Path)

#define IAcadPreferencesFiles_get_TextEditor(This,Path)	\
    (This)->lpVtbl -> get_TextEditor(This,Path)

#define IAcadPreferencesFiles_put_MainDictionary(This,Path)	\
    (This)->lpVtbl -> put_MainDictionary(This,Path)

#define IAcadPreferencesFiles_get_MainDictionary(This,Path)	\
    (This)->lpVtbl -> get_MainDictionary(This,Path)

#define IAcadPreferencesFiles_put_CustomDictionary(This,Path)	\
    (This)->lpVtbl -> put_CustomDictionary(This,Path)

#define IAcadPreferencesFiles_get_CustomDictionary(This,Path)	\
    (This)->lpVtbl -> get_CustomDictionary(This,Path)

#define IAcadPreferencesFiles_put_AltFontFile(This,fontFile)	\
    (This)->lpVtbl -> put_AltFontFile(This,fontFile)

#define IAcadPreferencesFiles_get_AltFontFile(This,fontFile)	\
    (This)->lpVtbl -> get_AltFontFile(This,fontFile)

#define IAcadPreferencesFiles_put_FontFileMap(This,Path)	\
    (This)->lpVtbl -> put_FontFileMap(This,Path)

#define IAcadPreferencesFiles_get_FontFileMap(This,Path)	\
    (This)->lpVtbl -> get_FontFileMap(This,Path)

#define IAcadPreferencesFiles_put_PrintFile(This,Path)	\
    (This)->lpVtbl -> put_PrintFile(This,Path)

#define IAcadPreferencesFiles_get_PrintFile(This,Path)	\
    (This)->lpVtbl -> get_PrintFile(This,Path)

#define IAcadPreferencesFiles_put_PrintSpoolExecutable(This,Path)	\
    (This)->lpVtbl -> put_PrintSpoolExecutable(This,Path)

#define IAcadPreferencesFiles_get_PrintSpoolExecutable(This,Path)	\
    (This)->lpVtbl -> get_PrintSpoolExecutable(This,Path)

#define IAcadPreferencesFiles_put_PostScriptPrologFile(This,Path)	\
    (This)->lpVtbl -> put_PostScriptPrologFile(This,Path)

#define IAcadPreferencesFiles_get_PostScriptPrologFile(This,Path)	\
    (This)->lpVtbl -> get_PostScriptPrologFile(This,Path)

#define IAcadPreferencesFiles_put_PrintSpoolerPath(This,Path)	\
    (This)->lpVtbl -> put_PrintSpoolerPath(This,Path)

#define IAcadPreferencesFiles_get_PrintSpoolerPath(This,Path)	\
    (This)->lpVtbl -> get_PrintSpoolerPath(This,Path)

#define IAcadPreferencesFiles_put_AutoSavePath(This,AutoSavePath)	\
    (This)->lpVtbl -> put_AutoSavePath(This,AutoSavePath)

#define IAcadPreferencesFiles_get_AutoSavePath(This,AutoSavePath)	\
    (This)->lpVtbl -> get_AutoSavePath(This,AutoSavePath)

#define IAcadPreferencesFiles_put_TemplateDwgPath(This,Path)	\
    (This)->lpVtbl -> put_TemplateDwgPath(This,Path)

#define IAcadPreferencesFiles_get_TemplateDwgPath(This,Path)	\
    (This)->lpVtbl -> get_TemplateDwgPath(This,Path)

#define IAcadPreferencesFiles_put_LogFilePath(This,Path)	\
    (This)->lpVtbl -> put_LogFilePath(This,Path)

#define IAcadPreferencesFiles_get_LogFilePath(This,Path)	\
    (This)->lpVtbl -> get_LogFilePath(This,Path)

#define IAcadPreferencesFiles_put_TempFilePath(This,Path)	\
    (This)->lpVtbl -> put_TempFilePath(This,Path)

#define IAcadPreferencesFiles_get_TempFilePath(This,Path)	\
    (This)->lpVtbl -> get_TempFilePath(This,Path)

#define IAcadPreferencesFiles_put_TempXrefPath(This,Path)	\
    (This)->lpVtbl -> put_TempXrefPath(This,Path)

#define IAcadPreferencesFiles_get_TempXrefPath(This,Path)	\
    (This)->lpVtbl -> get_TempXrefPath(This,Path)

#define IAcadPreferencesFiles_put_TextureMapPath(This,Path)	\
    (This)->lpVtbl -> put_TextureMapPath(This,Path)

#define IAcadPreferencesFiles_get_TextureMapPath(This,Path)	\
    (This)->lpVtbl -> get_TextureMapPath(This,Path)

#define IAcadPreferencesFiles_put_AltTabletMenuFile(This,MenuFile)	\
    (This)->lpVtbl -> put_AltTabletMenuFile(This,MenuFile)

#define IAcadPreferencesFiles_get_AltTabletMenuFile(This,MenuFile)	\
    (This)->lpVtbl -> get_AltTabletMenuFile(This,MenuFile)

#define IAcadPreferencesFiles_SetProjectFilePath(This,ProjectName,ProjectFilePath)	\
    (This)->lpVtbl -> SetProjectFilePath(This,ProjectName,ProjectFilePath)

#define IAcadPreferencesFiles_GetProjectFilePath(This,ProjectName,Path)	\
    (This)->lpVtbl -> GetProjectFilePath(This,ProjectName,Path)

#define IAcadPreferencesFiles_put_PrinterConfigPath(This,Path)	\
    (This)->lpVtbl -> put_PrinterConfigPath(This,Path)

#define IAcadPreferencesFiles_get_PrinterConfigPath(This,Path)	\
    (This)->lpVtbl -> get_PrinterConfigPath(This,Path)

#define IAcadPreferencesFiles_put_PrinterDescPath(This,Path)	\
    (This)->lpVtbl -> put_PrinterDescPath(This,Path)

#define IAcadPreferencesFiles_get_PrinterDescPath(This,Path)	\
    (This)->lpVtbl -> get_PrinterDescPath(This,Path)

#define IAcadPreferencesFiles_put_PrinterStyleSheetPath(This,Path)	\
    (This)->lpVtbl -> put_PrinterStyleSheetPath(This,Path)

#define IAcadPreferencesFiles_get_PrinterStyleSheetPath(This,Path)	\
    (This)->lpVtbl -> get_PrinterStyleSheetPath(This,Path)

#define IAcadPreferencesFiles_put_WorkspacePath(This,Path)	\
    (This)->lpVtbl -> put_WorkspacePath(This,Path)

#define IAcadPreferencesFiles_get_WorkspacePath(This,Path)	\
    (This)->lpVtbl -> get_WorkspacePath(This,Path)

#define IAcadPreferencesFiles_put_ObjectARXPath(This,Path)	\
    (This)->lpVtbl -> put_ObjectARXPath(This,Path)

#define IAcadPreferencesFiles_get_ObjectARXPath(This,Path)	\
    (This)->lpVtbl -> get_ObjectARXPath(This,Path)

#define IAcadPreferencesFiles_put_ColorBookPath(This,Path)	\
    (This)->lpVtbl -> put_ColorBookPath(This,Path)

#define IAcadPreferencesFiles_get_ColorBookPath(This,Path)	\
    (This)->lpVtbl -> get_ColorBookPath(This,Path)

#define IAcadPreferencesFiles_put_ToolPalettePath(This,Path)	\
    (This)->lpVtbl -> put_ToolPalettePath(This,Path)

#define IAcadPreferencesFiles_get_ToolPalettePath(This,Path)	\
    (This)->lpVtbl -> get_ToolPalettePath(This,Path)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_Application_Proxy( 
    IAcadPreferencesFiles * This,
    /* [retval][out] */ IAcadApplication **pAppObj);


void __RPC_STUB IAcadPreferencesFiles_get_Application_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_put_SupportPath_Proxy( 
    IAcadPreferencesFiles * This,
    /* [in] */ BSTR orient);


void __RPC_STUB IAcadPreferencesFiles_put_SupportPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_SupportPath_Proxy( 
    IAcadPreferencesFiles * This,
    /* [retval][out] */ BSTR *orient);


void __RPC_STUB IAcadPreferencesFiles_get_SupportPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_put_DriversPath_Proxy( 
    IAcadPreferencesFiles * This,
    /* [in] */ BSTR Path);


void __RPC_STUB IAcadPreferencesFiles_put_DriversPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_DriversPath_Proxy( 
    IAcadPreferencesFiles * This,
    /* [retval][out] */ BSTR *Path);


void __RPC_STUB IAcadPreferencesFiles_get_DriversPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_put_MenuFile_Proxy( 
    IAcadPreferencesFiles * This,
    /* [in] */ BSTR Path);


void __RPC_STUB IAcadPreferencesFiles_put_MenuFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_MenuFile_Proxy( 
    IAcadPreferencesFiles * This,
    /* [retval][out] */ BSTR *Path);


void __RPC_STUB IAcadPreferencesFiles_get_MenuFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_put_HelpFilePath_Proxy( 
    IAcadPreferencesFiles * This,
    /* [in] */ BSTR Path);


void __RPC_STUB IAcadPreferencesFiles_put_HelpFilePath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_HelpFilePath_Proxy( 
    IAcadPreferencesFiles * This,
    /* [retval][out] */ BSTR *Path);


void __RPC_STUB IAcadPreferencesFiles_get_HelpFilePath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_put_DefaultInternetURL_Proxy( 
    IAcadPreferencesFiles * This,
    /* [in] */ BSTR Path);


void __RPC_STUB IAcadPreferencesFiles_put_DefaultInternetURL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_DefaultInternetURL_Proxy( 
    IAcadPreferencesFiles * This,
    /* [retval][out] */ BSTR *Path);


void __RPC_STUB IAcadPreferencesFiles_get_DefaultInternetURL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_ConfigFile_Proxy( 
    IAcadPreferencesFiles * This,
    /* [retval][out] */ BSTR *ConfigFile);


void __RPC_STUB IAcadPreferencesFiles_get_ConfigFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][hidden][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_LicenseServer_Proxy( 
    IAcadPreferencesFiles * This,
    /* [retval][out] */ BSTR *Path);


void __RPC_STUB IAcadPreferencesFiles_get_LicenseServer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_put_TextEditor_Proxy( 
    IAcadPreferencesFiles * This,
    /* [in] */ BSTR Path);


void __RPC_STUB IAcadPreferencesFiles_put_TextEditor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_TextEditor_Proxy( 
    IAcadPreferencesFiles * This,
    /* [retval][out] */ BSTR *Path);


void __RPC_STUB IAcadPreferencesFiles_get_TextEditor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_put_MainDictionary_Proxy( 
    IAcadPreferencesFiles * This,
    /* [in] */ BSTR Path);


void __RPC_STUB IAcadPreferencesFiles_put_MainDictionary_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_MainDictionary_Proxy( 
    IAcadPreferencesFiles * This,
    /* [retval][out] */ BSTR *Path);


void __RPC_STUB IAcadPreferencesFiles_get_MainDictionary_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_put_CustomDictionary_Proxy( 
    IAcadPreferencesFiles * This,
    /* [in] */ BSTR Path);


void __RPC_STUB IAcadPreferencesFiles_put_CustomDictionary_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_CustomDictionary_Proxy( 
    IAcadPreferencesFiles * This,
    /* [retval][out] */ BSTR *Path);


void __RPC_STUB IAcadPreferencesFiles_get_CustomDictionary_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_put_AltFontFile_Proxy( 
    IAcadPreferencesFiles * This,
    /* [in] */ BSTR fontFile);


void __RPC_STUB IAcadPreferencesFiles_put_AltFontFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_AltFontFile_Proxy( 
    IAcadPreferencesFiles * This,
    /* [retval][out] */ BSTR *fontFile);


void __RPC_STUB IAcadPreferencesFiles_get_AltFontFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_put_FontFileMap_Proxy( 
    IAcadPreferencesFiles * This,
    /* [in] */ BSTR Path);


void __RPC_STUB IAcadPreferencesFiles_put_FontFileMap_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_FontFileMap_Proxy( 
    IAcadPreferencesFiles * This,
    /* [retval][out] */ BSTR *Path);


void __RPC_STUB IAcadPreferencesFiles_get_FontFileMap_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_put_PrintFile_Proxy( 
    IAcadPreferencesFiles * This,
    /* [in] */ BSTR Path);


void __RPC_STUB IAcadPreferencesFiles_put_PrintFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_PrintFile_Proxy( 
    IAcadPreferencesFiles * This,
    /* [retval][out] */ BSTR *Path);


void __RPC_STUB IAcadPreferencesFiles_get_PrintFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_put_PrintSpoolExecutable_Proxy( 
    IAcadPreferencesFiles * This,
    /* [in] */ BSTR Path);


void __RPC_STUB IAcadPreferencesFiles_put_PrintSpoolExecutable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_PrintSpoolExecutable_Proxy( 
    IAcadPreferencesFiles * This,
    /* [retval][out] */ BSTR *Path);


void __RPC_STUB IAcadPreferencesFiles_get_PrintSpoolExecutable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_put_PostScriptPrologFile_Proxy( 
    IAcadPreferencesFiles * This,
    /* [in] */ BSTR Path);


void __RPC_STUB IAcadPreferencesFiles_put_PostScriptPrologFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_PostScriptPrologFile_Proxy( 
    IAcadPreferencesFiles * This,
    /* [retval][out] */ BSTR *Path);


void __RPC_STUB IAcadPreferencesFiles_get_PostScriptPrologFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_put_PrintSpoolerPath_Proxy( 
    IAcadPreferencesFiles * This,
    /* [in] */ BSTR Path);


void __RPC_STUB IAcadPreferencesFiles_put_PrintSpoolerPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_PrintSpoolerPath_Proxy( 
    IAcadPreferencesFiles * This,
    /* [retval][out] */ BSTR *Path);


void __RPC_STUB IAcadPreferencesFiles_get_PrintSpoolerPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_put_AutoSavePath_Proxy( 
    IAcadPreferencesFiles * This,
    /* [in] */ BSTR AutoSavePath);


void __RPC_STUB IAcadPreferencesFiles_put_AutoSavePath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_AutoSavePath_Proxy( 
    IAcadPreferencesFiles * This,
    /* [retval][out] */ BSTR *AutoSavePath);


void __RPC_STUB IAcadPreferencesFiles_get_AutoSavePath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_put_TemplateDwgPath_Proxy( 
    IAcadPreferencesFiles * This,
    /* [in] */ BSTR Path);


void __RPC_STUB IAcadPreferencesFiles_put_TemplateDwgPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_TemplateDwgPath_Proxy( 
    IAcadPreferencesFiles * This,
    /* [retval][out] */ BSTR *Path);


void __RPC_STUB IAcadPreferencesFiles_get_TemplateDwgPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_put_LogFilePath_Proxy( 
    IAcadPreferencesFiles * This,
    /* [in] */ BSTR Path);


void __RPC_STUB IAcadPreferencesFiles_put_LogFilePath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_LogFilePath_Proxy( 
    IAcadPreferencesFiles * This,
    /* [retval][out] */ BSTR *Path);


void __RPC_STUB IAcadPreferencesFiles_get_LogFilePath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_put_TempFilePath_Proxy( 
    IAcadPreferencesFiles * This,
    /* [in] */ BSTR Path);


void __RPC_STUB IAcadPreferencesFiles_put_TempFilePath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_TempFilePath_Proxy( 
    IAcadPreferencesFiles * This,
    /* [retval][out] */ BSTR *Path);


void __RPC_STUB IAcadPreferencesFiles_get_TempFilePath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_put_TempXrefPath_Proxy( 
    IAcadPreferencesFiles * This,
    /* [in] */ BSTR Path);


void __RPC_STUB IAcadPreferencesFiles_put_TempXrefPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_TempXrefPath_Proxy( 
    IAcadPreferencesFiles * This,
    /* [retval][out] */ BSTR *Path);


void __RPC_STUB IAcadPreferencesFiles_get_TempXrefPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_put_TextureMapPath_Proxy( 
    IAcadPreferencesFiles * This,
    /* [in] */ BSTR Path);


void __RPC_STUB IAcadPreferencesFiles_put_TextureMapPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_TextureMapPath_Proxy( 
    IAcadPreferencesFiles * This,
    /* [retval][out] */ BSTR *Path);


void __RPC_STUB IAcadPreferencesFiles_get_TextureMapPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_put_AltTabletMenuFile_Proxy( 
    IAcadPreferencesFiles * This,
    /* [in] */ BSTR MenuFile);


void __RPC_STUB IAcadPreferencesFiles_put_AltTabletMenuFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_AltTabletMenuFile_Proxy( 
    IAcadPreferencesFiles * This,
    /* [retval][out] */ BSTR *MenuFile);


void __RPC_STUB IAcadPreferencesFiles_get_AltTabletMenuFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_SetProjectFilePath_Proxy( 
    IAcadPreferencesFiles * This,
    /* [in] */ BSTR ProjectName,
    /* [in] */ BSTR ProjectFilePath);


void __RPC_STUB IAcadPreferencesFiles_SetProjectFilePath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_GetProjectFilePath_Proxy( 
    IAcadPreferencesFiles * This,
    /* [in] */ BSTR ProjectName,
    /* [retval][out] */ BSTR *Path);


void __RPC_STUB IAcadPreferencesFiles_GetProjectFilePath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_put_PrinterConfigPath_Proxy( 
    IAcadPreferencesFiles * This,
    /* [in] */ BSTR Path);


void __RPC_STUB IAcadPreferencesFiles_put_PrinterConfigPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_PrinterConfigPath_Proxy( 
    IAcadPreferencesFiles * This,
    /* [retval][out] */ BSTR *Path);


void __RPC_STUB IAcadPreferencesFiles_get_PrinterConfigPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_put_PrinterDescPath_Proxy( 
    IAcadPreferencesFiles * This,
    /* [in] */ BSTR Path);


void __RPC_STUB IAcadPreferencesFiles_put_PrinterDescPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_PrinterDescPath_Proxy( 
    IAcadPreferencesFiles * This,
    /* [retval][out] */ BSTR *Path);


void __RPC_STUB IAcadPreferencesFiles_get_PrinterDescPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_put_PrinterStyleSheetPath_Proxy( 
    IAcadPreferencesFiles * This,
    /* [in] */ BSTR Path);


void __RPC_STUB IAcadPreferencesFiles_put_PrinterStyleSheetPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_PrinterStyleSheetPath_Proxy( 
    IAcadPreferencesFiles * This,
    /* [retval][out] */ BSTR *Path);


void __RPC_STUB IAcadPreferencesFiles_get_PrinterStyleSheetPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_put_WorkspacePath_Proxy( 
    IAcadPreferencesFiles * This,
    /* [in] */ BSTR Path);


void __RPC_STUB IAcadPreferencesFiles_put_WorkspacePath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_WorkspacePath_Proxy( 
    IAcadPreferencesFiles * This,
    /* [retval][out] */ BSTR *Path);


void __RPC_STUB IAcadPreferencesFiles_get_WorkspacePath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][hidden][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_put_ObjectARXPath_Proxy( 
    IAcadPreferencesFiles * This,
    /* [in] */ BSTR Path);


void __RPC_STUB IAcadPreferencesFiles_put_ObjectARXPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][hidden][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_ObjectARXPath_Proxy( 
    IAcadPreferencesFiles * This,
    /* [retval][out] */ BSTR *Path);


void __RPC_STUB IAcadPreferencesFiles_get_ObjectARXPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_put_ColorBookPath_Proxy( 
    IAcadPreferencesFiles * This,
    /* [in] */ BSTR Path);


void __RPC_STUB IAcadPreferencesFiles_put_ColorBookPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_ColorBookPath_Proxy( 
    IAcadPreferencesFiles * This,
    /* [retval][out] */ BSTR *Path);


void __RPC_STUB IAcadPreferencesFiles_get_ColorBookPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_put_ToolPalettePath_Proxy( 
    IAcadPreferencesFiles * This,
    /* [in] */ BSTR Path);


void __RPC_STUB IAcadPreferencesFiles_put_ToolPalettePath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadPreferencesFiles_get_ToolPalettePath_Proxy( 
    IAcadPreferencesFiles * This,
    /* [retval][out] */ BSTR *Path);


void __RPC_STUB IAcadPreferencesFiles_get_ToolPalettePath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IAcadPreferencesFiles_INTERFACE_DEFINED__ */


#ifndef __IAcadLayerStateManager_INTERFACE_DEFINED__
#define __IAcadLayerStateManager_INTERFACE_DEFINED__

/* interface IAcadLayerStateManager */
/* [object][oleautomation][dual][helpstring][uuid] */ 


EXTERN_C const IID IID_IAcadLayerStateManager;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("92999267-E45C-43BA-BAB0-7C408BAFADC2")
    IAcadLayerStateManager : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetDatabase( 
            /* [in] */ IAcadDatabase *iHostDb) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Mask( 
            /* [in] */ BSTR bsName,
            /* [in] */ AcLayerStateMask eMask) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Mask( 
            /* [in] */ BSTR bsName,
            /* [retval][out] */ AcLayerStateMask *eMask) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Save( 
            /* [in] */ BSTR bsName,
            /* [in] */ AcLayerStateMask eMask) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Restore( 
            /* [in] */ BSTR bsName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Delete( 
            /* [in] */ BSTR bsName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Rename( 
            /* [in] */ BSTR bsName,
            /* [in] */ BSTR bsNewName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Import( 
            /* [in] */ BSTR bsFilename) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Export( 
            /* [in] */ BSTR bsName,
            /* [in] */ BSTR bsFilename) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAcadLayerStateManagerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAcadLayerStateManager * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAcadLayerStateManager * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAcadLayerStateManager * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IAcadLayerStateManager * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IAcadLayerStateManager * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IAcadLayerStateManager * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IAcadLayerStateManager * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetDatabase )( 
            IAcadLayerStateManager * This,
            /* [in] */ IAcadDatabase *iHostDb);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Mask )( 
            IAcadLayerStateManager * This,
            /* [in] */ BSTR bsName,
            /* [in] */ AcLayerStateMask eMask);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Mask )( 
            IAcadLayerStateManager * This,
            /* [in] */ BSTR bsName,
            /* [retval][out] */ AcLayerStateMask *eMask);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Save )( 
            IAcadLayerStateManager * This,
            /* [in] */ BSTR bsName,
            /* [in] */ AcLayerStateMask eMask);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Restore )( 
            IAcadLayerStateManager * This,
            /* [in] */ BSTR bsName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Delete )( 
            IAcadLayerStateManager * This,
            /* [in] */ BSTR bsName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Rename )( 
            IAcadLayerStateManager * This,
            /* [in] */ BSTR bsName,
            /* [in] */ BSTR bsNewName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Import )( 
            IAcadLayerStateManager * This,
            /* [in] */ BSTR bsFilename);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Export )( 
            IAcadLayerStateManager * This,
            /* [in] */ BSTR bsName,
            /* [in] */ BSTR bsFilename);
        
        END_INTERFACE
    } IAcadLayerStateManagerVtbl;

    interface IAcadLayerStateManager
    {
        CONST_VTBL struct IAcadLayerStateManagerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAcadLayerStateManager_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAcadLayerStateManager_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAcadLayerStateManager_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAcadLayerStateManager_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IAcadLayerStateManager_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IAcadLayerStateManager_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IAcadLayerStateManager_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IAcadLayerStateManager_SetDatabase(This,iHostDb)	\
    (This)->lpVtbl -> SetDatabase(This,iHostDb)

#define IAcadLayerStateManager_put_Mask(This,bsName,eMask)	\
    (This)->lpVtbl -> put_Mask(This,bsName,eMask)

#define IAcadLayerStateManager_get_Mask(This,bsName,eMask)	\
    (This)->lpVtbl -> get_Mask(This,bsName,eMask)

#define IAcadLayerStateManager_Save(This,bsName,eMask)	\
    (This)->lpVtbl -> Save(This,bsName,eMask)

#define IAcadLayerStateManager_Restore(This,bsName)	\
    (This)->lpVtbl -> Restore(This,bsName)

#define IAcadLayerStateManager_Delete(This,bsName)	\
    (This)->lpVtbl -> Delete(This,bsName)

#define IAcadLayerStateManager_Rename(This,bsName,bsNewName)	\
    (This)->lpVtbl -> Rename(This,bsName,bsNewName)

#define IAcadLayerStateManager_Import(This,bsFilename)	\
    (This)->lpVtbl -> Import(This,bsFilename)

#define IAcadLayerStateManager_Export(This,bsName,bsFilename)	\
    (This)->lpVtbl -> Export(This,bsName,bsFilename)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadLayerStateManager_SetDatabase_Proxy( 
    IAcadLayerStateManager * This,
    /* [in] */ IAcadDatabase *iHostDb);


void __RPC_STUB IAcadLayerStateManager_SetDatabase_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadLayerStateManager_put_Mask_Proxy( 
    IAcadLayerStateManager * This,
    /* [in] */ BSTR bsName,
    /* [in] */ AcLayerStateMask eMask);


void __RPC_STUB IAcadLayerStateManager_put_Mask_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadLayerStateManager_get_Mask_Proxy( 
    IAcadLayerStateManager * This,
    /* [in] */ BSTR bsName,
    /* [retval][out] */ AcLayerStateMask *eMask);


void __RPC_STUB IAcadLayerStateManager_get_Mask_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadLayerStateManager_Save_Proxy( 
    IAcadLayerStateManager * This,
    /* [in] */ BSTR bsName,
    /* [in] */ AcLayerStateMask eMask);


void __RPC_STUB IAcadLayerStateManager_Save_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadLayerStateManager_Restore_Proxy( 
    IAcadLayerStateManager * This,
    /* [in] */ BSTR bsName);


void __RPC_STUB IAcadLayerStateManager_Restore_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadLayerStateManager_Delete_Proxy( 
    IAcadLayerStateManager * This,
    /* [in] */ BSTR bsName);


void __RPC_STUB IAcadLayerStateManager_Delete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadLayerStateManager_Rename_Proxy( 
    IAcadLayerStateManager * This,
    /* [in] */ BSTR bsName,
    /* [in] */ BSTR bsNewName);


void __RPC_STUB IAcadLayerStateManager_Rename_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadLayerStateManager_Import_Proxy( 
    IAcadLayerStateManager * This,
    /* [in] */ BSTR bsFilename);


void __RPC_STUB IAcadLayerStateManager_Import_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IAcadLayerStateManager_Export_Proxy( 
    IAcadLayerStateManager * This,
    /* [in] */ BSTR bsName,
    /* [in] */ BSTR bsFilename);


void __RPC_STUB IAcadLayerStateManager_Export_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IAcadLayerStateManager_INTERFACE_DEFINED__ */


#ifndef __IAcadSecurityParams_INTERFACE_DEFINED__
#define __IAcadSecurityParams_INTERFACE_DEFINED__

/* interface IAcadSecurityParams */
/* [object][oleautomation][dual][helpstring][uuid] */ 


EXTERN_C const IID IID_IAcadSecurityParams;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6917CED6-6655-475C-B62F-370BE1148408")
    IAcadSecurityParams : public IDispatch
    {
    public:
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Action( 
            /* [in] */ long pOperations) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Action( 
            /* [retval][out] */ long *pOperations) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Password( 
            /* [in] */ BSTR pSecret) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Password( 
            /* [retval][out] */ BSTR *pSecret) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_ProviderType( 
            /* [in] */ long pProvType) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_ProviderType( 
            /* [retval][out] */ long *pProvType) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_ProviderName( 
            /* [in] */ BSTR pProvName) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_ProviderName( 
            /* [retval][out] */ BSTR *pProvName) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Algorithm( 
            /* [in] */ long pAlgId) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Algorithm( 
            /* [retval][out] */ long *pAlgId) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_KeyLength( 
            /* [in] */ long pKeyLen) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_KeyLength( 
            /* [retval][out] */ long *pKeyLen) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Subject( 
            /* [in] */ BSTR pCertSubject) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Subject( 
            /* [retval][out] */ BSTR *pCertSubject) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Issuer( 
            /* [in] */ BSTR pCertIssuer) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Issuer( 
            /* [retval][out] */ BSTR *pCertIssuer) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_SerialNumber( 
            /* [in] */ BSTR pSerialNum) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_SerialNumber( 
            /* [retval][out] */ BSTR *pSerialNum) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Comment( 
            /* [in] */ BSTR pText) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Comment( 
            /* [retval][out] */ BSTR *pText) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_TimeServer( 
            /* [in] */ BSTR pTimeServerName) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_TimeServer( 
            /* [retval][out] */ BSTR *pTimeServerName) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAcadSecurityParamsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAcadSecurityParams * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAcadSecurityParams * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAcadSecurityParams * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IAcadSecurityParams * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IAcadSecurityParams * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IAcadSecurityParams * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IAcadSecurityParams * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Action )( 
            IAcadSecurityParams * This,
            /* [in] */ long pOperations);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Action )( 
            IAcadSecurityParams * This,
            /* [retval][out] */ long *pOperations);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Password )( 
            IAcadSecurityParams * This,
            /* [in] */ BSTR pSecret);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Password )( 
            IAcadSecurityParams * This,
            /* [retval][out] */ BSTR *pSecret);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_ProviderType )( 
            IAcadSecurityParams * This,
            /* [in] */ long pProvType);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_ProviderType )( 
            IAcadSecurityParams * This,
            /* [retval][out] */ long *pProvType);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_ProviderName )( 
            IAcadSecurityParams * This,
            /* [in] */ BSTR pProvName);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_ProviderName )( 
            IAcadSecurityParams * This,
            /* [retval][out] */ BSTR *pProvName);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Algorithm )( 
            IAcadSecurityParams * This,
            /* [in] */ long pAlgId);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Algorithm )( 
            IAcadSecurityParams * This,
            /* [retval][out] */ long *pAlgId);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_KeyLength )( 
            IAcadSecurityParams * This,
            /* [in] */ long pKeyLen);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_KeyLength )( 
            IAcadSecurityParams * This,
            /* [retval][out] */ long *pKeyLen);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Subject )( 
            IAcadSecurityParams * This,
            /* [in] */ BSTR pCertSubject);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Subject )( 
            IAcadSecurityParams * This,
            /* [retval][out] */ BSTR *pCertSubject);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Issuer )( 
            IAcadSecurityParams * This,
            /* [in] */ BSTR pCertIssuer);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Issuer )( 
            IAcadSecurityParams * This,
            /* [retval][out] */ BSTR *pCertIssuer);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_SerialNumber )( 
            IAcadSecurityParams * This,
            /* [in] */ BSTR pSerialNum);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_SerialNumber )( 
            IAcadSecurityParams * This,
            /* [retval][out] */ BSTR *pSerialNum);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Comment )( 
            IAcadSecurityParams * This,
            /* [in] */ BSTR pText);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Comment )( 
            IAcadSecurityParams * This,
            /* [retval][out] */ BSTR *pText);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_TimeServer )( 
            IAcadSecurityParams * This,
            /* [in] */ BSTR pTimeServerName);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_TimeServer )( 
            IAcadSecurityParams * This,
            /* [retval][out] */ BSTR *pTimeServerName);
        
        END_INTERFACE
    } IAcadSecurityParamsVtbl;

    interface IAcadSecurityParams
    {
        CONST_VTBL struct IAcadSecurityParamsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAcadSecurityParams_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAcadSecurityParams_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAcadSecurityParams_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAcadSecurityParams_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IAcadSecurityParams_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IAcadSecurityParams_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IAcadSecurityParams_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IAcadSecurityParams_put_Action(This,pOperations)	\
    (This)->lpVtbl -> put_Action(This,pOperations)

#define IAcadSecurityParams_get_Action(This,pOperations)	\
    (This)->lpVtbl -> get_Action(This,pOperations)

#define IAcadSecurityParams_put_Password(This,pSecret)	\
    (This)->lpVtbl -> put_Password(This,pSecret)

#define IAcadSecurityParams_get_Password(This,pSecret)	\
    (This)->lpVtbl -> get_Password(This,pSecret)

#define IAcadSecurityParams_put_ProviderType(This,pProvType)	\
    (This)->lpVtbl -> put_ProviderType(This,pProvType)

#define IAcadSecurityParams_get_ProviderType(This,pProvType)	\
    (This)->lpVtbl -> get_ProviderType(This,pProvType)

#define IAcadSecurityParams_put_ProviderName(This,pProvName)	\
    (This)->lpVtbl -> put_ProviderName(This,pProvName)

#define IAcadSecurityParams_get_ProviderName(This,pProvName)	\
    (This)->lpVtbl -> get_ProviderName(This,pProvName)

#define IAcadSecurityParams_put_Algorithm(This,pAlgId)	\
    (This)->lpVtbl -> put_Algorithm(This,pAlgId)

#define IAcadSecurityParams_get_Algorithm(This,pAlgId)	\
    (This)->lpVtbl -> get_Algorithm(This,pAlgId)

#define IAcadSecurityParams_put_KeyLength(This,pKeyLen)	\
    (This)->lpVtbl -> put_KeyLength(This,pKeyLen)

#define IAcadSecurityParams_get_KeyLength(This,pKeyLen)	\
    (This)->lpVtbl -> get_KeyLength(This,pKeyLen)

#define IAcadSecurityParams_put_Subject(This,pCertSubject)	\
    (This)->lpVtbl -> put_Subject(This,pCertSubject)

#define IAcadSecurityParams_get_Subject(This,pCertSubject)	\
    (This)->lpVtbl -> get_Subject(This,pCertSubject)

#define IAcadSecurityParams_put_Issuer(This,pCertIssuer)	\
    (This)->lpVtbl -> put_Issuer(This,pCertIssuer)

#define IAcadSecurityParams_get_Issuer(This,pCertIssuer)	\
    (This)->lpVtbl -> get_Issuer(This,pCertIssuer)

#define IAcadSecurityParams_put_SerialNumber(This,pSerialNum)	\
    (This)->lpVtbl -> put_SerialNumber(This,pSerialNum)

#define IAcadSecurityParams_get_SerialNumber(This,pSerialNum)	\
    (This)->lpVtbl -> get_SerialNumber(This,pSerialNum)

#define IAcadSecurityParams_put_Comment(This,pText)	\
    (This)->lpVtbl -> put_Comment(This,pText)

#define IAcadSecurityParams_get_Comment(This,pText)	\
    (This)->lpVtbl -> get_Comment(This,pText)

#define IAcadSecurityParams_put_TimeServer(This,pTimeServerName)	\
    (This)->lpVtbl -> put_TimeServer(This,pTimeServerName)

#define IAcadSecurityParams_get_TimeServer(This,pTimeServerName)	\
    (This)->lpVtbl -> get_TimeServer(This,pTimeServerName)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadSecurityParams_put_Action_Proxy( 
    IAcadSecurityParams * This,
    /* [in] */ long pOperations);


void __RPC_STUB IAcadSecurityParams_put_Action_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadSecurityParams_get_Action_Proxy( 
    IAcadSecurityParams * This,
    /* [retval][out] */ long *pOperations);


void __RPC_STUB IAcadSecurityParams_get_Action_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadSecurityParams_put_Password_Proxy( 
    IAcadSecurityParams * This,
    /* [in] */ BSTR pSecret);


void __RPC_STUB IAcadSecurityParams_put_Password_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadSecurityParams_get_Password_Proxy( 
    IAcadSecurityParams * This,
    /* [retval][out] */ BSTR *pSecret);


void __RPC_STUB IAcadSecurityParams_get_Password_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadSecurityParams_put_ProviderType_Proxy( 
    IAcadSecurityParams * This,
    /* [in] */ long pProvType);


void __RPC_STUB IAcadSecurityParams_put_ProviderType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadSecurityParams_get_ProviderType_Proxy( 
    IAcadSecurityParams * This,
    /* [retval][out] */ long *pProvType);


void __RPC_STUB IAcadSecurityParams_get_ProviderType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadSecurityParams_put_ProviderName_Proxy( 
    IAcadSecurityParams * This,
    /* [in] */ BSTR pProvName);


void __RPC_STUB IAcadSecurityParams_put_ProviderName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadSecurityParams_get_ProviderName_Proxy( 
    IAcadSecurityParams * This,
    /* [retval][out] */ BSTR *pProvName);


void __RPC_STUB IAcadSecurityParams_get_ProviderName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadSecurityParams_put_Algorithm_Proxy( 
    IAcadSecurityParams * This,
    /* [in] */ long pAlgId);


void __RPC_STUB IAcadSecurityParams_put_Algorithm_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadSecurityParams_get_Algorithm_Proxy( 
    IAcadSecurityParams * This,
    /* [retval][out] */ long *pAlgId);


void __RPC_STUB IAcadSecurityParams_get_Algorithm_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadSecurityParams_put_KeyLength_Proxy( 
    IAcadSecurityParams * This,
    /* [in] */ long pKeyLen);


void __RPC_STUB IAcadSecurityParams_put_KeyLength_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadSecurityParams_get_KeyLength_Proxy( 
    IAcadSecurityParams * This,
    /* [retval][out] */ long *pKeyLen);


void __RPC_STUB IAcadSecurityParams_get_KeyLength_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadSecurityParams_put_Subject_Proxy( 
    IAcadSecurityParams * This,
    /* [in] */ BSTR pCertSubject);


void __RPC_STUB IAcadSecurityParams_put_Subject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadSecurityParams_get_Subject_Proxy( 
    IAcadSecurityParams * This,
    /* [retval][out] */ BSTR *pCertSubject);


void __RPC_STUB IAcadSecurityParams_get_Subject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadSecurityParams_put_Issuer_Proxy( 
    IAcadSecurityParams * This,
    /* [in] */ BSTR pCertIssuer);


void __RPC_STUB IAcadSecurityParams_put_Issuer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadSecurityParams_get_Issuer_Proxy( 
    IAcadSecurityParams * This,
    /* [retval][out] */ BSTR *pCertIssuer);


void __RPC_STUB IAcadSecurityParams_get_Issuer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadSecurityParams_put_SerialNumber_Proxy( 
    IAcadSecurityParams * This,
    /* [in] */ BSTR pSerialNum);


void __RPC_STUB IAcadSecurityParams_put_SerialNumber_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadSecurityParams_get_SerialNumber_Proxy( 
    IAcadSecurityParams * This,
    /* [retval][out] */ BSTR *pSerialNum);


void __RPC_STUB IAcadSecurityParams_get_SerialNumber_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadSecurityParams_put_Comment_Proxy( 
    IAcadSecurityParams * This,
    /* [in] */ BSTR pText);


void __RPC_STUB IAcadSecurityParams_put_Comment_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadSecurityParams_get_Comment_Proxy( 
    IAcadSecurityParams * This,
    /* [retval][out] */ BSTR *pText);


void __RPC_STUB IAcadSecurityParams_get_Comment_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IAcadSecurityParams_put_TimeServer_Proxy( 
    IAcadSecurityParams * This,
    /* [in] */ BSTR pTimeServerName);


void __RPC_STUB IAcadSecurityParams_put_TimeServer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IAcadSecurityParams_get_TimeServer_Proxy( 
    IAcadSecurityParams * This,
    /* [retval][out] */ BSTR *pTimeServerName);


void __RPC_STUB IAcadSecurityParams_get_TimeServer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IAcadSecurityParams_INTERFACE_DEFINED__ */


#ifndef __IOdaAuditInfo_INTERFACE_DEFINED__
#define __IOdaAuditInfo_INTERFACE_DEFINED__

/* interface IOdaAuditInfo */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IOdaAuditInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("56B202FE-BE71-43b4-B504-1902EDE89644")
    IOdaAuditInfo : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_numFixes( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_numErrors( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_fixErrors( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IOdaAuditInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IOdaAuditInfo * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IOdaAuditInfo * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IOdaAuditInfo * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IOdaAuditInfo * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IOdaAuditInfo * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IOdaAuditInfo * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IOdaAuditInfo * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_numFixes )( 
            IOdaAuditInfo * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_numErrors )( 
            IOdaAuditInfo * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_fixErrors )( 
            IOdaAuditInfo * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        END_INTERFACE
    } IOdaAuditInfoVtbl;

    interface IOdaAuditInfo
    {
        CONST_VTBL struct IOdaAuditInfoVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOdaAuditInfo_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IOdaAuditInfo_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IOdaAuditInfo_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IOdaAuditInfo_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IOdaAuditInfo_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IOdaAuditInfo_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IOdaAuditInfo_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IOdaAuditInfo_get_numFixes(This,pVal)	\
    (This)->lpVtbl -> get_numFixes(This,pVal)

#define IOdaAuditInfo_get_numErrors(This,pVal)	\
    (This)->lpVtbl -> get_numErrors(This,pVal)

#define IOdaAuditInfo_get_fixErrors(This,pVal)	\
    (This)->lpVtbl -> get_fixErrors(This,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaAuditInfo_get_numFixes_Proxy( 
    IOdaAuditInfo * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB IOdaAuditInfo_get_numFixes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaAuditInfo_get_numErrors_Proxy( 
    IOdaAuditInfo * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB IOdaAuditInfo_get_numErrors_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaAuditInfo_get_fixErrors_Proxy( 
    IOdaAuditInfo * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB IOdaAuditInfo_get_fixErrors_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IOdaAuditInfo_INTERFACE_DEFINED__ */


#ifndef ___DAcadApplicationEvents_DISPINTERFACE_DEFINED__
#define ___DAcadApplicationEvents_DISPINTERFACE_DEFINED__

/* dispinterface _DAcadApplicationEvents */
/* [hidden][helpstring][uuid] */ 


EXTERN_C const IID DIID__DAcadApplicationEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("93E291E9-BB72-429b-B4DE-DF76B88603EB")
    _DAcadApplicationEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DAcadApplicationEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DAcadApplicationEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DAcadApplicationEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DAcadApplicationEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DAcadApplicationEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DAcadApplicationEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DAcadApplicationEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DAcadApplicationEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _DAcadApplicationEventsVtbl;

    interface _DAcadApplicationEvents
    {
        CONST_VTBL struct _DAcadApplicationEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DAcadApplicationEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _DAcadApplicationEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _DAcadApplicationEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _DAcadApplicationEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _DAcadApplicationEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _DAcadApplicationEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _DAcadApplicationEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DAcadApplicationEvents_DISPINTERFACE_DEFINED__ */


#ifndef ___DAcadDocumentEvents_DISPINTERFACE_DEFINED__
#define ___DAcadDocumentEvents_DISPINTERFACE_DEFINED__

/* dispinterface _DAcadDocumentEvents */
/* [hidden][helpstring][uuid] */ 


EXTERN_C const IID DIID__DAcadDocumentEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("63A90E32-CB8B-4a3b-ADFF-EF94A35278B2")
    _DAcadDocumentEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DAcadDocumentEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DAcadDocumentEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DAcadDocumentEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DAcadDocumentEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DAcadDocumentEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DAcadDocumentEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DAcadDocumentEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DAcadDocumentEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _DAcadDocumentEventsVtbl;

    interface _DAcadDocumentEvents
    {
        CONST_VTBL struct _DAcadDocumentEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DAcadDocumentEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _DAcadDocumentEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _DAcadDocumentEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _DAcadDocumentEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _DAcadDocumentEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _DAcadDocumentEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _DAcadDocumentEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DAcadDocumentEvents_DISPINTERFACE_DEFINED__ */


#ifndef __IOdaHostApp_INTERFACE_DEFINED__
#define __IOdaHostApp_INTERFACE_DEFINED__

/* interface IOdaHostApp */
/* [oleautomation][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IOdaHostApp;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E789E253-BEDC-44ff-B228-AD3A17D9AB3A")
    IOdaHostApp : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Application( 
            /* [retval][out] */ IAcadApplication **pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE BrowseForOpenFileName( 
            BSTR rootFolder,
            BSTR fileFilter,
            VARIANT_BOOL *bOk,
            BSTR *filename) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AuditDatabase( 
            IAcadDocument *pDoc,
            VARIANT_BOOL bFixErrors) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_AuditInfo( 
            /* [retval][out] */ IOdaAuditInfo **pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OriginalFileType( 
            IAcadDocument *pDoc,
            VARIANT *FileType) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Recover( 
            /* [in] */ BSTR fileName,
            /* [optional][in] */ VARIANT Password,
            /* [retval][out] */ IAcadDocument **pDoc) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_OpenDbPartially( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_OpenDbPartially( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IOdaHostAppVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IOdaHostApp * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IOdaHostApp * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IOdaHostApp * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IOdaHostApp * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IOdaHostApp * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IOdaHostApp * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IOdaHostApp * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Application )( 
            IOdaHostApp * This,
            /* [retval][out] */ IAcadApplication **pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *BrowseForOpenFileName )( 
            IOdaHostApp * This,
            BSTR rootFolder,
            BSTR fileFilter,
            VARIANT_BOOL *bOk,
            BSTR *filename);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AuditDatabase )( 
            IOdaHostApp * This,
            IAcadDocument *pDoc,
            VARIANT_BOOL bFixErrors);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_AuditInfo )( 
            IOdaHostApp * This,
            /* [retval][out] */ IOdaAuditInfo **pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OriginalFileType )( 
            IOdaHostApp * This,
            IAcadDocument *pDoc,
            VARIANT *FileType);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Recover )( 
            IOdaHostApp * This,
            /* [in] */ BSTR fileName,
            /* [optional][in] */ VARIANT Password,
            /* [retval][out] */ IAcadDocument **pDoc);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_OpenDbPartially )( 
            IOdaHostApp * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_OpenDbPartially )( 
            IOdaHostApp * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        END_INTERFACE
    } IOdaHostAppVtbl;

    interface IOdaHostApp
    {
        CONST_VTBL struct IOdaHostAppVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOdaHostApp_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IOdaHostApp_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IOdaHostApp_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IOdaHostApp_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IOdaHostApp_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IOdaHostApp_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IOdaHostApp_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IOdaHostApp_get_Application(This,pVal)	\
    (This)->lpVtbl -> get_Application(This,pVal)

#define IOdaHostApp_BrowseForOpenFileName(This,rootFolder,fileFilter,bOk,filename)	\
    (This)->lpVtbl -> BrowseForOpenFileName(This,rootFolder,fileFilter,bOk,filename)

#define IOdaHostApp_AuditDatabase(This,pDoc,bFixErrors)	\
    (This)->lpVtbl -> AuditDatabase(This,pDoc,bFixErrors)

#define IOdaHostApp_get_AuditInfo(This,pVal)	\
    (This)->lpVtbl -> get_AuditInfo(This,pVal)

#define IOdaHostApp_OriginalFileType(This,pDoc,FileType)	\
    (This)->lpVtbl -> OriginalFileType(This,pDoc,FileType)

#define IOdaHostApp_Recover(This,fileName,Password,pDoc)	\
    (This)->lpVtbl -> Recover(This,fileName,Password,pDoc)

#define IOdaHostApp_get_OpenDbPartially(This,pVal)	\
    (This)->lpVtbl -> get_OpenDbPartially(This,pVal)

#define IOdaHostApp_put_OpenDbPartially(This,newVal)	\
    (This)->lpVtbl -> put_OpenDbPartially(This,newVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaHostApp_get_Application_Proxy( 
    IOdaHostApp * This,
    /* [retval][out] */ IAcadApplication **pVal);


void __RPC_STUB IOdaHostApp_get_Application_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IOdaHostApp_BrowseForOpenFileName_Proxy( 
    IOdaHostApp * This,
    BSTR rootFolder,
    BSTR fileFilter,
    VARIANT_BOOL *bOk,
    BSTR *filename);


void __RPC_STUB IOdaHostApp_BrowseForOpenFileName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IOdaHostApp_AuditDatabase_Proxy( 
    IOdaHostApp * This,
    IAcadDocument *pDoc,
    VARIANT_BOOL bFixErrors);


void __RPC_STUB IOdaHostApp_AuditDatabase_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaHostApp_get_AuditInfo_Proxy( 
    IOdaHostApp * This,
    /* [retval][out] */ IOdaAuditInfo **pVal);


void __RPC_STUB IOdaHostApp_get_AuditInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IOdaHostApp_OriginalFileType_Proxy( 
    IOdaHostApp * This,
    IAcadDocument *pDoc,
    VARIANT *FileType);


void __RPC_STUB IOdaHostApp_OriginalFileType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IOdaHostApp_Recover_Proxy( 
    IOdaHostApp * This,
    /* [in] */ BSTR fileName,
    /* [optional][in] */ VARIANT Password,
    /* [retval][out] */ IAcadDocument **pDoc);


void __RPC_STUB IOdaHostApp_Recover_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaHostApp_get_OpenDbPartially_Proxy( 
    IOdaHostApp * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB IOdaHostApp_get_OpenDbPartially_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOdaHostApp_put_OpenDbPartially_Proxy( 
    IOdaHostApp * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB IOdaHostApp_put_OpenDbPartially_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IOdaHostApp_INTERFACE_DEFINED__ */


#ifndef __IOdaDwfPageData_INTERFACE_DEFINED__
#define __IOdaDwfPageData_INTERFACE_DEFINED__

/* interface IOdaDwfPageData */
/* [object][oleautomation][dual][helpstring][uuid] */ 


EXTERN_C const IID IID_IOdaDwfPageData;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6C2751B5-A21F-46d8-8EBE-59F0A15288D9")
    IOdaDwfPageData : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_LayoutName( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Author( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Author( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Title( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Title( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Subject( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Subject( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Company( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Company( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Comments( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Comments( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Reviewers( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Reviewers( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Keywords( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Keywords( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Description( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Description( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Copyright( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Copyright( 
            /* [in] */ BSTR newVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IOdaDwfPageDataVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IOdaDwfPageData * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IOdaDwfPageData * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IOdaDwfPageData * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IOdaDwfPageData * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IOdaDwfPageData * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IOdaDwfPageData * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IOdaDwfPageData * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_LayoutName )( 
            IOdaDwfPageData * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Author )( 
            IOdaDwfPageData * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Author )( 
            IOdaDwfPageData * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Title )( 
            IOdaDwfPageData * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Title )( 
            IOdaDwfPageData * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Subject )( 
            IOdaDwfPageData * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Subject )( 
            IOdaDwfPageData * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Company )( 
            IOdaDwfPageData * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Company )( 
            IOdaDwfPageData * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Comments )( 
            IOdaDwfPageData * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Comments )( 
            IOdaDwfPageData * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Reviewers )( 
            IOdaDwfPageData * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Reviewers )( 
            IOdaDwfPageData * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Keywords )( 
            IOdaDwfPageData * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Keywords )( 
            IOdaDwfPageData * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Description )( 
            IOdaDwfPageData * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Description )( 
            IOdaDwfPageData * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Copyright )( 
            IOdaDwfPageData * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Copyright )( 
            IOdaDwfPageData * This,
            /* [in] */ BSTR newVal);
        
        END_INTERFACE
    } IOdaDwfPageDataVtbl;

    interface IOdaDwfPageData
    {
        CONST_VTBL struct IOdaDwfPageDataVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOdaDwfPageData_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IOdaDwfPageData_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IOdaDwfPageData_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IOdaDwfPageData_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IOdaDwfPageData_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IOdaDwfPageData_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IOdaDwfPageData_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IOdaDwfPageData_get_LayoutName(This,pVal)	\
    (This)->lpVtbl -> get_LayoutName(This,pVal)

#define IOdaDwfPageData_get_Author(This,pVal)	\
    (This)->lpVtbl -> get_Author(This,pVal)

#define IOdaDwfPageData_put_Author(This,newVal)	\
    (This)->lpVtbl -> put_Author(This,newVal)

#define IOdaDwfPageData_get_Title(This,pVal)	\
    (This)->lpVtbl -> get_Title(This,pVal)

#define IOdaDwfPageData_put_Title(This,newVal)	\
    (This)->lpVtbl -> put_Title(This,newVal)

#define IOdaDwfPageData_get_Subject(This,pVal)	\
    (This)->lpVtbl -> get_Subject(This,pVal)

#define IOdaDwfPageData_put_Subject(This,newVal)	\
    (This)->lpVtbl -> put_Subject(This,newVal)

#define IOdaDwfPageData_get_Company(This,pVal)	\
    (This)->lpVtbl -> get_Company(This,pVal)

#define IOdaDwfPageData_put_Company(This,newVal)	\
    (This)->lpVtbl -> put_Company(This,newVal)

#define IOdaDwfPageData_get_Comments(This,pVal)	\
    (This)->lpVtbl -> get_Comments(This,pVal)

#define IOdaDwfPageData_put_Comments(This,newVal)	\
    (This)->lpVtbl -> put_Comments(This,newVal)

#define IOdaDwfPageData_get_Reviewers(This,pVal)	\
    (This)->lpVtbl -> get_Reviewers(This,pVal)

#define IOdaDwfPageData_put_Reviewers(This,newVal)	\
    (This)->lpVtbl -> put_Reviewers(This,newVal)

#define IOdaDwfPageData_get_Keywords(This,pVal)	\
    (This)->lpVtbl -> get_Keywords(This,pVal)

#define IOdaDwfPageData_put_Keywords(This,newVal)	\
    (This)->lpVtbl -> put_Keywords(This,newVal)

#define IOdaDwfPageData_get_Description(This,pVal)	\
    (This)->lpVtbl -> get_Description(This,pVal)

#define IOdaDwfPageData_put_Description(This,newVal)	\
    (This)->lpVtbl -> put_Description(This,newVal)

#define IOdaDwfPageData_get_Copyright(This,pVal)	\
    (This)->lpVtbl -> get_Copyright(This,pVal)

#define IOdaDwfPageData_put_Copyright(This,newVal)	\
    (This)->lpVtbl -> put_Copyright(This,newVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaDwfPageData_get_LayoutName_Proxy( 
    IOdaDwfPageData * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IOdaDwfPageData_get_LayoutName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaDwfPageData_get_Author_Proxy( 
    IOdaDwfPageData * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IOdaDwfPageData_get_Author_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOdaDwfPageData_put_Author_Proxy( 
    IOdaDwfPageData * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IOdaDwfPageData_put_Author_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaDwfPageData_get_Title_Proxy( 
    IOdaDwfPageData * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IOdaDwfPageData_get_Title_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOdaDwfPageData_put_Title_Proxy( 
    IOdaDwfPageData * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IOdaDwfPageData_put_Title_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaDwfPageData_get_Subject_Proxy( 
    IOdaDwfPageData * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IOdaDwfPageData_get_Subject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOdaDwfPageData_put_Subject_Proxy( 
    IOdaDwfPageData * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IOdaDwfPageData_put_Subject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaDwfPageData_get_Company_Proxy( 
    IOdaDwfPageData * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IOdaDwfPageData_get_Company_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOdaDwfPageData_put_Company_Proxy( 
    IOdaDwfPageData * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IOdaDwfPageData_put_Company_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaDwfPageData_get_Comments_Proxy( 
    IOdaDwfPageData * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IOdaDwfPageData_get_Comments_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOdaDwfPageData_put_Comments_Proxy( 
    IOdaDwfPageData * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IOdaDwfPageData_put_Comments_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaDwfPageData_get_Reviewers_Proxy( 
    IOdaDwfPageData * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IOdaDwfPageData_get_Reviewers_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOdaDwfPageData_put_Reviewers_Proxy( 
    IOdaDwfPageData * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IOdaDwfPageData_put_Reviewers_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaDwfPageData_get_Keywords_Proxy( 
    IOdaDwfPageData * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IOdaDwfPageData_get_Keywords_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOdaDwfPageData_put_Keywords_Proxy( 
    IOdaDwfPageData * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IOdaDwfPageData_put_Keywords_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaDwfPageData_get_Description_Proxy( 
    IOdaDwfPageData * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IOdaDwfPageData_get_Description_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOdaDwfPageData_put_Description_Proxy( 
    IOdaDwfPageData * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IOdaDwfPageData_put_Description_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaDwfPageData_get_Copyright_Proxy( 
    IOdaDwfPageData * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IOdaDwfPageData_get_Copyright_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOdaDwfPageData_put_Copyright_Proxy( 
    IOdaDwfPageData * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IOdaDwfPageData_put_Copyright_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IOdaDwfPageData_INTERFACE_DEFINED__ */


#ifndef __IOdaDwfTemplate_INTERFACE_DEFINED__
#define __IOdaDwfTemplate_INTERFACE_DEFINED__

/* interface IOdaDwfTemplate */
/* [object][oleautomation][dual][helpstring][uuid] */ 


EXTERN_C const IID IID_IOdaDwfTemplate;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("8F2A46C2-4D8A-46D5-8BBE-B06A4027BE10")
    IOdaDwfTemplate : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WriteFile( 
            /* [in] */ IAcadDatabase *Database,
            /* [in] */ BSTR FileName,
            /* [in] */ OdDwfFormat FileType,
            /* [in] */ OdDwfVersion Version) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddPage( 
            /* [in] */ BSTR LayoutName,
            /* [retval][out] */ long *PageIndex) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_PageCount( 
            /* [retval][out] */ long *pCount) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Page( 
            /* [in] */ VARIANT Index,
            /* [retval][out] */ IOdaDwfPageData **PageData) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RemovePage( 
            /* [in] */ VARIANT Index) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RemoveAllPages( void) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Background( 
            /* [retval][out] */ IAcadAcCmColor **pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Background( 
            /* [in] */ IAcadAcCmColor *newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ExportInvisibleLayers( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ExportInvisibleLayers( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ForceInitialViewToExtents( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ForceInitialViewToExtents( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SkipLayerInfo( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_SkipLayerInfo( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SkipNamedViewsInfo( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_SkipNamedViewsInfo( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Password( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Password( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Publisher( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Publisher( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_WideComments( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_WideComments( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SourceProductName( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_SourceProductName( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Palette( 
            /* [retval][out] */ VARIANT *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Palette( 
            /* [in] */ VARIANT pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IOdaDwfTemplateVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IOdaDwfTemplate * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IOdaDwfTemplate * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IOdaDwfTemplate * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IOdaDwfTemplate * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IOdaDwfTemplate * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IOdaDwfTemplate * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IOdaDwfTemplate * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *WriteFile )( 
            IOdaDwfTemplate * This,
            /* [in] */ IAcadDatabase *Database,
            /* [in] */ BSTR FileName,
            /* [in] */ OdDwfFormat FileType,
            /* [in] */ OdDwfVersion Version);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AddPage )( 
            IOdaDwfTemplate * This,
            /* [in] */ BSTR LayoutName,
            /* [retval][out] */ long *PageIndex);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_PageCount )( 
            IOdaDwfTemplate * This,
            /* [retval][out] */ long *pCount);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Page )( 
            IOdaDwfTemplate * This,
            /* [in] */ VARIANT Index,
            /* [retval][out] */ IOdaDwfPageData **PageData);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RemovePage )( 
            IOdaDwfTemplate * This,
            /* [in] */ VARIANT Index);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RemoveAllPages )( 
            IOdaDwfTemplate * This);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Background )( 
            IOdaDwfTemplate * This,
            /* [retval][out] */ IAcadAcCmColor **pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Background )( 
            IOdaDwfTemplate * This,
            /* [in] */ IAcadAcCmColor *newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ExportInvisibleLayers )( 
            IOdaDwfTemplate * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ExportInvisibleLayers )( 
            IOdaDwfTemplate * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ForceInitialViewToExtents )( 
            IOdaDwfTemplate * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ForceInitialViewToExtents )( 
            IOdaDwfTemplate * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SkipLayerInfo )( 
            IOdaDwfTemplate * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_SkipLayerInfo )( 
            IOdaDwfTemplate * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SkipNamedViewsInfo )( 
            IOdaDwfTemplate * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_SkipNamedViewsInfo )( 
            IOdaDwfTemplate * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Password )( 
            IOdaDwfTemplate * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Password )( 
            IOdaDwfTemplate * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Publisher )( 
            IOdaDwfTemplate * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Publisher )( 
            IOdaDwfTemplate * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_WideComments )( 
            IOdaDwfTemplate * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_WideComments )( 
            IOdaDwfTemplate * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SourceProductName )( 
            IOdaDwfTemplate * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_SourceProductName )( 
            IOdaDwfTemplate * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Palette )( 
            IOdaDwfTemplate * This,
            /* [retval][out] */ VARIANT *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Palette )( 
            IOdaDwfTemplate * This,
            /* [in] */ VARIANT pVal);
        
        END_INTERFACE
    } IOdaDwfTemplateVtbl;

    interface IOdaDwfTemplate
    {
        CONST_VTBL struct IOdaDwfTemplateVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOdaDwfTemplate_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IOdaDwfTemplate_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IOdaDwfTemplate_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IOdaDwfTemplate_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IOdaDwfTemplate_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IOdaDwfTemplate_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IOdaDwfTemplate_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IOdaDwfTemplate_WriteFile(This,Database,FileName,FileType,Version)	\
    (This)->lpVtbl -> WriteFile(This,Database,FileName,FileType,Version)

#define IOdaDwfTemplate_AddPage(This,LayoutName,PageIndex)	\
    (This)->lpVtbl -> AddPage(This,LayoutName,PageIndex)

#define IOdaDwfTemplate_get_PageCount(This,pCount)	\
    (This)->lpVtbl -> get_PageCount(This,pCount)

#define IOdaDwfTemplate_get_Page(This,Index,PageData)	\
    (This)->lpVtbl -> get_Page(This,Index,PageData)

#define IOdaDwfTemplate_RemovePage(This,Index)	\
    (This)->lpVtbl -> RemovePage(This,Index)

#define IOdaDwfTemplate_RemoveAllPages(This)	\
    (This)->lpVtbl -> RemoveAllPages(This)

#define IOdaDwfTemplate_get_Background(This,pVal)	\
    (This)->lpVtbl -> get_Background(This,pVal)

#define IOdaDwfTemplate_put_Background(This,newVal)	\
    (This)->lpVtbl -> put_Background(This,newVal)

#define IOdaDwfTemplate_get_ExportInvisibleLayers(This,pVal)	\
    (This)->lpVtbl -> get_ExportInvisibleLayers(This,pVal)

#define IOdaDwfTemplate_put_ExportInvisibleLayers(This,newVal)	\
    (This)->lpVtbl -> put_ExportInvisibleLayers(This,newVal)

#define IOdaDwfTemplate_get_ForceInitialViewToExtents(This,pVal)	\
    (This)->lpVtbl -> get_ForceInitialViewToExtents(This,pVal)

#define IOdaDwfTemplate_put_ForceInitialViewToExtents(This,newVal)	\
    (This)->lpVtbl -> put_ForceInitialViewToExtents(This,newVal)

#define IOdaDwfTemplate_get_SkipLayerInfo(This,pVal)	\
    (This)->lpVtbl -> get_SkipLayerInfo(This,pVal)

#define IOdaDwfTemplate_put_SkipLayerInfo(This,newVal)	\
    (This)->lpVtbl -> put_SkipLayerInfo(This,newVal)

#define IOdaDwfTemplate_get_SkipNamedViewsInfo(This,pVal)	\
    (This)->lpVtbl -> get_SkipNamedViewsInfo(This,pVal)

#define IOdaDwfTemplate_put_SkipNamedViewsInfo(This,newVal)	\
    (This)->lpVtbl -> put_SkipNamedViewsInfo(This,newVal)

#define IOdaDwfTemplate_get_Password(This,pVal)	\
    (This)->lpVtbl -> get_Password(This,pVal)

#define IOdaDwfTemplate_put_Password(This,newVal)	\
    (This)->lpVtbl -> put_Password(This,newVal)

#define IOdaDwfTemplate_get_Publisher(This,pVal)	\
    (This)->lpVtbl -> get_Publisher(This,pVal)

#define IOdaDwfTemplate_put_Publisher(This,newVal)	\
    (This)->lpVtbl -> put_Publisher(This,newVal)

#define IOdaDwfTemplate_get_WideComments(This,pVal)	\
    (This)->lpVtbl -> get_WideComments(This,pVal)

#define IOdaDwfTemplate_put_WideComments(This,newVal)	\
    (This)->lpVtbl -> put_WideComments(This,newVal)

#define IOdaDwfTemplate_get_SourceProductName(This,pVal)	\
    (This)->lpVtbl -> get_SourceProductName(This,pVal)

#define IOdaDwfTemplate_put_SourceProductName(This,newVal)	\
    (This)->lpVtbl -> put_SourceProductName(This,newVal)

#define IOdaDwfTemplate_get_Palette(This,pVal)	\
    (This)->lpVtbl -> get_Palette(This,pVal)

#define IOdaDwfTemplate_put_Palette(This,pVal)	\
    (This)->lpVtbl -> put_Palette(This,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IOdaDwfTemplate_WriteFile_Proxy( 
    IOdaDwfTemplate * This,
    /* [in] */ IAcadDatabase *Database,
    /* [in] */ BSTR FileName,
    /* [in] */ OdDwfFormat FileType,
    /* [in] */ OdDwfVersion Version);


void __RPC_STUB IOdaDwfTemplate_WriteFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IOdaDwfTemplate_AddPage_Proxy( 
    IOdaDwfTemplate * This,
    /* [in] */ BSTR LayoutName,
    /* [retval][out] */ long *PageIndex);


void __RPC_STUB IOdaDwfTemplate_AddPage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaDwfTemplate_get_PageCount_Proxy( 
    IOdaDwfTemplate * This,
    /* [retval][out] */ long *pCount);


void __RPC_STUB IOdaDwfTemplate_get_PageCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaDwfTemplate_get_Page_Proxy( 
    IOdaDwfTemplate * This,
    /* [in] */ VARIANT Index,
    /* [retval][out] */ IOdaDwfPageData **PageData);


void __RPC_STUB IOdaDwfTemplate_get_Page_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IOdaDwfTemplate_RemovePage_Proxy( 
    IOdaDwfTemplate * This,
    /* [in] */ VARIANT Index);


void __RPC_STUB IOdaDwfTemplate_RemovePage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IOdaDwfTemplate_RemoveAllPages_Proxy( 
    IOdaDwfTemplate * This);


void __RPC_STUB IOdaDwfTemplate_RemoveAllPages_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaDwfTemplate_get_Background_Proxy( 
    IOdaDwfTemplate * This,
    /* [retval][out] */ IAcadAcCmColor **pVal);


void __RPC_STUB IOdaDwfTemplate_get_Background_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOdaDwfTemplate_put_Background_Proxy( 
    IOdaDwfTemplate * This,
    /* [in] */ IAcadAcCmColor *newVal);


void __RPC_STUB IOdaDwfTemplate_put_Background_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaDwfTemplate_get_ExportInvisibleLayers_Proxy( 
    IOdaDwfTemplate * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB IOdaDwfTemplate_get_ExportInvisibleLayers_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOdaDwfTemplate_put_ExportInvisibleLayers_Proxy( 
    IOdaDwfTemplate * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB IOdaDwfTemplate_put_ExportInvisibleLayers_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaDwfTemplate_get_ForceInitialViewToExtents_Proxy( 
    IOdaDwfTemplate * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB IOdaDwfTemplate_get_ForceInitialViewToExtents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOdaDwfTemplate_put_ForceInitialViewToExtents_Proxy( 
    IOdaDwfTemplate * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB IOdaDwfTemplate_put_ForceInitialViewToExtents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaDwfTemplate_get_SkipLayerInfo_Proxy( 
    IOdaDwfTemplate * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB IOdaDwfTemplate_get_SkipLayerInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOdaDwfTemplate_put_SkipLayerInfo_Proxy( 
    IOdaDwfTemplate * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB IOdaDwfTemplate_put_SkipLayerInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaDwfTemplate_get_SkipNamedViewsInfo_Proxy( 
    IOdaDwfTemplate * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB IOdaDwfTemplate_get_SkipNamedViewsInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOdaDwfTemplate_put_SkipNamedViewsInfo_Proxy( 
    IOdaDwfTemplate * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB IOdaDwfTemplate_put_SkipNamedViewsInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaDwfTemplate_get_Password_Proxy( 
    IOdaDwfTemplate * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IOdaDwfTemplate_get_Password_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOdaDwfTemplate_put_Password_Proxy( 
    IOdaDwfTemplate * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IOdaDwfTemplate_put_Password_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaDwfTemplate_get_Publisher_Proxy( 
    IOdaDwfTemplate * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IOdaDwfTemplate_get_Publisher_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOdaDwfTemplate_put_Publisher_Proxy( 
    IOdaDwfTemplate * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IOdaDwfTemplate_put_Publisher_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaDwfTemplate_get_WideComments_Proxy( 
    IOdaDwfTemplate * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IOdaDwfTemplate_get_WideComments_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOdaDwfTemplate_put_WideComments_Proxy( 
    IOdaDwfTemplate * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IOdaDwfTemplate_put_WideComments_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaDwfTemplate_get_SourceProductName_Proxy( 
    IOdaDwfTemplate * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IOdaDwfTemplate_get_SourceProductName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOdaDwfTemplate_put_SourceProductName_Proxy( 
    IOdaDwfTemplate * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IOdaDwfTemplate_put_SourceProductName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOdaDwfTemplate_get_Palette_Proxy( 
    IOdaDwfTemplate * This,
    /* [retval][out] */ VARIANT *pVal);


void __RPC_STUB IOdaDwfTemplate_get_Palette_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOdaDwfTemplate_put_Palette_Proxy( 
    IOdaDwfTemplate * This,
    /* [in] */ VARIANT pVal);


void __RPC_STUB IOdaDwfTemplate_put_Palette_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IOdaDwfTemplate_INTERFACE_DEFINED__ */


#ifndef __IOdaDwfImporter_INTERFACE_DEFINED__
#define __IOdaDwfImporter_INTERFACE_DEFINED__

/* interface IOdaDwfImporter */
/* [object][oleautomation][dual][helpstring][uuid] */ 


EXTERN_C const IID IID_IOdaDwfImporter;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C5631279-833D-4fe0-8965-B06E2FEC0E9C")
    IOdaDwfImporter : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Import( 
            /* [in] */ IAcadDatabase *Database,
            /* [in] */ BSTR FileName,
            /* [defaultvalue][optional][in] */ BSTR password = L"",
            /* [defaultvalue][optional][in] */ double wPaper = 297,
            /* [defaultvalue][optional][in] */ double hPaper = 210) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IOdaDwfImporterVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IOdaDwfImporter * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IOdaDwfImporter * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IOdaDwfImporter * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IOdaDwfImporter * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IOdaDwfImporter * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IOdaDwfImporter * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IOdaDwfImporter * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Import )( 
            IOdaDwfImporter * This,
            /* [in] */ IAcadDatabase *Database,
            /* [in] */ BSTR FileName,
            /* [defaultvalue][optional][in] */ BSTR password,
            /* [defaultvalue][optional][in] */ double wPaper,
            /* [defaultvalue][optional][in] */ double hPaper);
        
        END_INTERFACE
    } IOdaDwfImporterVtbl;

    interface IOdaDwfImporter
    {
        CONST_VTBL struct IOdaDwfImporterVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOdaDwfImporter_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IOdaDwfImporter_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IOdaDwfImporter_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IOdaDwfImporter_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IOdaDwfImporter_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IOdaDwfImporter_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IOdaDwfImporter_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IOdaDwfImporter_Import(This,Database,FileName,password,wPaper,hPaper)	\
    (This)->lpVtbl -> Import(This,Database,FileName,password,wPaper,hPaper)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IOdaDwfImporter_Import_Proxy( 
    IOdaDwfImporter * This,
    /* [in] */ IAcadDatabase *Database,
    /* [in] */ BSTR FileName,
    /* [defaultvalue][optional][in] */ BSTR password,
    /* [defaultvalue][optional][in] */ double wPaper,
    /* [defaultvalue][optional][in] */ double hPaper);


void __RPC_STUB IOdaDwfImporter_Import_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IOdaDwfImporter_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_AcadApplication;

#ifdef __cplusplus

class DECLSPEC_UUID("350EA6EC-5E10-4e00-82B9-787F3961EC0A")
AcadApplication;
#endif

EXTERN_C const CLSID CLSID_AcadDocument;

#ifdef __cplusplus

class DECLSPEC_UUID("CB564067-0757-48aa-899E-F614B0D0FD63")
AcadDocument;
#endif

#ifndef ___IOdaHostAppEvents_DISPINTERFACE_DEFINED__
#define ___IOdaHostAppEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IOdaHostAppEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__IOdaHostAppEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("5FA75E80-D112-4186-AD8E-0992D7F13475")
    _IOdaHostAppEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IOdaHostAppEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _IOdaHostAppEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _IOdaHostAppEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _IOdaHostAppEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _IOdaHostAppEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _IOdaHostAppEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _IOdaHostAppEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _IOdaHostAppEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _IOdaHostAppEventsVtbl;

    interface _IOdaHostAppEvents
    {
        CONST_VTBL struct _IOdaHostAppEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IOdaHostAppEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _IOdaHostAppEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _IOdaHostAppEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _IOdaHostAppEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _IOdaHostAppEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _IOdaHostAppEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _IOdaHostAppEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IOdaHostAppEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_OdaHostApp;

#ifdef __cplusplus

class DECLSPEC_UUID("7759508F-BF18-4ae6-8536-336B15B590F4")
OdaHostApp;
#endif

EXTERN_C const CLSID CLSID_AcadUtility;

#ifdef __cplusplus

class DECLSPEC_UUID("CD4D83CA-0EC9-4018-8CFC-5FCF4E759943")
AcadUtility;
#endif

EXTERN_C const CLSID CLSID_AcadPreferences;

#ifdef __cplusplus

class DECLSPEC_UUID("7FF43F58-ED54-47CE-A0D7-7624247F5357")
AcadPreferences;
#endif

EXTERN_C const CLSID CLSID_AcadSelectionSet;

#ifdef __cplusplus

class DECLSPEC_UUID("5AD5645E-0911-4532-9A4C-0DE40E39C090")
AcadSelectionSet;
#endif

EXTERN_C const CLSID CLSID_OdaDwfTemplate;

#ifdef __cplusplus

class DECLSPEC_UUID("0BFB8504-25E6-4104-A5AB-ED5C6CFAD6FA")
OdaDwfTemplate;
#endif

EXTERN_C const CLSID CLSID_OdaDwfImporter;

#ifdef __cplusplus

class DECLSPEC_UUID("A7D35007-AF69-4665-BABC-0C437E61010C")
OdaDwfImporter;
#endif

EXTERN_C const CLSID CLSID_AcadLayerStateManager;

#ifdef __cplusplus

class DECLSPEC_UUID("F0BEA1FE-F7A4-46EE-8C73-DF7E837580F0")
AcadLayerStateManager;
#endif

EXTERN_C const CLSID CLSID_AcadSecurityParams;

#ifdef __cplusplus

class DECLSPEC_UUID("6A897328-EC0C-459D-A2FD-B50807284257")
AcadSecurityParams;
#endif

EXTERN_C const CLSID CLSID_OdaSvgExporter;

#ifdef __cplusplus

class DECLSPEC_UUID("6CF2409D-F945-4912-ACA7-96AF9175AF1F")
OdaSvgExporter;
#endif
#endif /* __DWGdirectX_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

unsigned long             __RPC_USER  VARIANT_UserSize(     unsigned long *, unsigned long            , VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserMarshal(  unsigned long *, unsigned char *, VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserUnmarshal(unsigned long *, unsigned char *, VARIANT * ); 
void                      __RPC_USER  VARIANT_UserFree(     unsigned long *, VARIANT * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif



