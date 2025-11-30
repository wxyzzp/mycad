

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


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

#if !defined(_M_IA64) && !defined(_M_AMD64)


#pragma warning( disable: 4049 )  /* more than 64k source lines */


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_IOdaView,0xDBCBA22B,0xAA6A,0x403f,0xBE,0xA4,0xAC,0xE7,0xF6,0xC0,0xA7,0x99);


MIDL_DEFINE_GUID(IID, IID_IOdaDevice,0xE8170D5B,0x2A69,0x46af,0xB6,0xB7,0x3D,0xED,0xF9,0xEC,0x42,0x5E);


MIDL_DEFINE_GUID(IID, IID_IOdaDeviceManager,0xE045EAD6,0x226D,0x4296,0x8B,0xA3,0x07,0xD4,0x82,0x27,0x9B,0x53);


MIDL_DEFINE_GUID(IID, IID_IOdaViewer,0x8477311A,0x1357,0x462F,0x8B,0x8A,0x81,0xE0,0x22,0xB9,0x87,0xC2);


MIDL_DEFINE_GUID(IID, LIBID_VIEWXLib,0x1246EFC6,0xC050,0x4E25,0x91,0x58,0x51,0x03,0xA3,0xC0,0xEC,0xB5);


MIDL_DEFINE_GUID(CLSID, CLSID_OdaViewer,0x53840EC9,0xB5E8,0x42DB,0xA6,0xA7,0xE9,0xB7,0xBE,0xD3,0xCA,0xB3);


MIDL_DEFINE_GUID(CLSID, CLSID_OdaDevice,0xCF8C6149,0xF86B,0x4ab0,0x97,0x0F,0x40,0xD0,0xCC,0xE1,0x19,0x18);


MIDL_DEFINE_GUID(CLSID, CLSID_OdaView,0x013F2FA5,0xA719,0x470A,0xB9,0x3D,0xA2,0xAB,0x7F,0xFE,0xD6,0x8E);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/


