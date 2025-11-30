///////////////////////////////////////////////////////////////////////////////
// Copyright © 2002, Open Design Alliance Inc. ("Open Design")
//
// This software is owned by Open Design, and may only be incorporated into
// application programs owned by members of Open Design subject to a signed
// Membership Agreement and Supplemental Software License Agreement with
// Open Design. The structure and organization of this Software are the valuable
// trade secrets of Open Design and its suppliers. The Software is also protected
// by copyright law and international treaty provisions. You agree not to
// modify, adapt, translate, reverse engineer, decompile, disassemble or
// otherwise attempt to discover the source code of the Software. Application
// programs incorporating this software must include the following statement
// with their copyright notices:
//
//      DWGdirect © 2002 by Open Design Alliance Inc. All rights reserved.
//
// By use of this software, you acknowledge and accept the terms of this
// agreement.
///////////////////////////////////////////////////////////////////////////////



#ifndef _OD_PLATFORM_SETTINGS_H_
#define _OD_PLATFORM_SETTINGS_H_

//class OdGePoint3d;



#ifdef _MSC_VER

#define DD_USING(a)

#if _MSC_VER > 1200  && !defined(_WIN32_WCE) // 1200 == VC6
#define OD_TYPENAME typename
#define OD_STD_ALLOCATOR_AS_IS
#endif

#endif

#ifdef ODA_SUN_SPARC

#define OD_STRING_FNS
#define ODA_BIGENDIAN
#define OD_STD_ALLOCATOR
#define ODA_UNIXOS

//#define OD_TEMPLATE_INIT(A)  static void odinit_##A##_fn() { OdObjectWithImpl<A, A##Impl> obj; }

#define OD_TYPENAME typename
#define OD_TYPENAME2 typename

#endif


#ifdef ODA_MAC_OSX

#define OD_STRING_FNS
#define ODA_BIGENDIAN
#define OD_TYPENAME typename
#define ODA_UNIXOS

#endif


#ifdef ODA_SGI

#define OD_STRING_FNS
#define ODA_BIGENDIAN
#define OD_STD_ALLOCATOR
#define OD_TYPENAME typename
#define ODA_UNIXOS

#endif


#ifdef _WIN32_WCE

#define OD_STRING_FNS
#define OD_NO_GETENV
//#define OD_DISABLE_ACIS_SUPPORT
#define OD_TYPENAME 

#define DD_USING(a) 

extern void *bsearch(const void *key, const void *base, size_t numeles, size_t widthele, int(__cdecl *cfunc)(const void *elem1, const void *elem2));

#ifdef ARM
#define DD_STRICT_ALIGNMENT
#endif

#endif


// HP RISC
#ifdef ODA_HP

#define OD_STRING_FNS
#define ODA_BIGENDIAN
#define OD_STD_ALLOCATOR
#define ODA_NEED_TEMP_USING
#define ODA_GCC
#define ODA_UNIXOS

// Requires typename keyword in some contexts, which are illegal with other compilers.
#define OD_TYPENAME typename

#define DD_USING(a) 

#endif


// HP-UX Itanium
#ifdef ODA_HP_IT

#define OD_STRING_FNS
#define ODA_BIGENDIAN
#define OD_STD_ALLOCATOR
#define ODA_NEED_TEMP_USING
#define ODA_GCC
#define ODA_UNIXOS

// Requires typename keyword in some contexts, which are illegal with other compilers.
#define OD_TYPENAME typename

#define DD_USING(a) 

#endif


#ifdef ODA_AIX

#define OD_STRING_FNS
#define ODA_BIGENDIAN
#define OD_STD_ALLOCATOR_AS_IS
#define ODA_NEED_TEMP_USING
// Requires typename keyword in some contexts, which are illegal with other compilers.
#define OD_TYPENAME typename
#define OD_TYPENAME3 typename
#define ODA_UNIXOS

#endif


#ifdef __BCPLUSPLUS__

#define OD_STD_ALLOCATOR_AS_IS
#define ODA_GCC_2_95
#define OD_BSEARCH std::bsearch

#endif


#ifdef ODA_LINUX_X86

#define OD_STRING_FNS
#define OD_TYPENAME typename
#define ODA_UNIXOS

#endif


#ifdef ODA_SUN_X86

#define OD_STRING_FNS
#define OD_STD_ALLOCATOR

#define OD_TYPENAME typename
#define OD_TYPENAME2 typename
#define ODA_UNIXOS

#endif


#if defined(__MWERKS__) && !defined(_WIN32_WCE)
#include <TargetConditionals.h>

#if TARGET_OS_MAC
#define ODA_BIGENDIAN
#define OD_STRING_FNS
#define OD_NO_GETENV
#endif

#endif // __MWERKS__




#ifdef OD_STRING_FNS

//DD:EXPORT_ON
extern int Od_stricmp(const char*str, const char* str2);
extern int Od_strnicmp(const char *s1, const char *s2, int len);
extern char* Od_strupr(char* str);
extern char* Od_strlwr(char* str);
extern char* Od_strrev(char* str);
//DD:EXPORT_OFF

#define odStrICmp(str, str2) Od_stricmp(str, str2)
#define odStrnICmp(str, str2, n) Od_strnicmp(str, str2, n)
#define odStrUpr(str) Od_strupr(str)
#define odStrLwr(str) Od_strlwr(str)
#define odStrRev(str) Od_strrev(str)

#endif  /*  OD_STRING_FNS  */

/*   strings functions  */

#ifndef odStrLen
#define odStrLen(str)             strlen(str)
#endif

#ifndef odStrCmp
#define odStrCmp(str, str2)       strcmp(str, str2)
#endif

#ifndef odStrICmp
#define odStrICmp(str, str2) stricmp(str, str2)
#endif

#ifndef odStrnICmp
#define odStrnICmp(str, str2, n) strnicmp(str, str2, n)
#endif

#ifndef odStrUpr
#define odStrUpr(str) strupr(str)
#endif

#ifndef odStrLwr
#define odStrLwr(str) strlwr(str)
#endif

#ifndef odStrRev
#define odStrRev(str) strrev(str)
#endif

#ifndef OD_TYPENAME
#define OD_TYPENAME
#endif

#ifndef OD_TYPENAME2
#define OD_TYPENAME2
#endif

#ifndef OD_TYPENAME3
#define OD_TYPENAME3
#endif

#ifndef OD_BSEARCH
#define OD_BSEARCH ::bsearch
#endif

#ifndef OD_TEMPLATE_INIT
#define OD_TEMPLATE_INIT(A)
#endif


#ifndef DD_USING
#define DD_USING(a) using a 
#endif

#endif //#ifndef _OD_PLATFORM_SETTINGS_H_


