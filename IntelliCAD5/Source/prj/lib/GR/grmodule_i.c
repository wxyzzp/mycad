/* this file contains the actual definitions of */
/* the IIDs and CLSIDs */

/* link this file in with the server and any clients */


/* File created by MIDL compiler version 5.01.0164 */
/* at Thu Nov 06 14:37:45 2025
 */
/* Compiler settings for K:\cad\IntelliCAD5\Source\prj\lib\GR\grmodule.idl:
    Os (OptLev=s), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )
#ifdef __cplusplus
extern "C"{
#endif 


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

const IID IID_IVisioDLLLoadInfo = {0xB8094F9D,0xD014,0x11D2,{0xB1,0x7B,0x00,0x10,0x4B,0x22,0xE9,0x14}};


const IID LIBID_GRMODULELib = {0x12AA1391,0xD8DC,0x11D2,{0xB1,0x86,0x00,0x10,0x4B,0x22,0xE9,0x14}};


const CLSID CLSID_GRDLLLoadInfo = {0x12AA139E,0xD8DC,0x11D2,{0xB1,0x86,0x00,0x10,0x4B,0x22,0xE9,0x14}};


#ifdef __cplusplus
}
#endif

