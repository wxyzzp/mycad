/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Mon May 24 16:34:01 1999
 */
/* Compiler settings for .\idl\Undo.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __UndoTLB_h__
#define __UndoTLB_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __UndoMgr_FWD_DEFINED__
#define __UndoMgr_FWD_DEFINED__

#ifdef __cplusplus
typedef class UndoMgr UndoMgr;
#else
typedef struct UndoMgr UndoMgr;
#endif /* __cplusplus */

#endif 	/* __UndoMgr_FWD_DEFINED__ */


#ifndef __ParentUndoUnit_FWD_DEFINED__
#define __ParentUndoUnit_FWD_DEFINED__

#ifdef __cplusplus
typedef class ParentUndoUnit ParentUndoUnit;
#else
typedef struct ParentUndoUnit ParentUndoUnit;
#endif /* __cplusplus */

#endif 	/* __ParentUndoUnit_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 


#ifndef __VisioUndoMgr_LIBRARY_DEFINED__
#define __VisioUndoMgr_LIBRARY_DEFINED__

/* library VisioUndoMgr */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_VisioUndoMgr;

EXTERN_C const CLSID CLSID_UndoMgr;

#ifdef __cplusplus

class DECLSPEC_UUID("ED880C90-F1A1-11D1-AD51-0060089FD322")
UndoMgr;
#endif

EXTERN_C const CLSID CLSID_ParentUndoUnit;

#ifdef __cplusplus

class DECLSPEC_UUID("C3494C63-F302-11D1-AD51-0060089FD322")
ParentUndoUnit;
#endif
#endif /* __VisioUndoMgr_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
