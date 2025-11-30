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

#ifndef _OD_RXDYNAMICMODULE_H_
#define _OD_RXDYNAMICMODULE_H_

#include "RxModule.h"
#include "DDVersion.h"
#include "RefCountImpl.h"
#include "OdString.h"


/** Description:

    {group:OdRx_Classes} 
*/
template <class TBase>
class OdRxStaticModule : public TBase
{
public:
  void* moduleHandle() { return 0; }
  void setModuleHandle(void* ) {}
  OdString moduleFileName() const { return OdString(); }
};

typedef OdRxModulePtr (*StaticModuleEntryPoint)();

#define ODRX_STATIC_MODULE_ENTRY_POINT(_UserModuleClass) odrxCreateModuleObject_For_##_UserModuleClass

/** Description:
    This macro is used to create an entry point function for a custom application which 
    are linked in statically to a DWGdirect client application.
*/
#define ODRX_DECLARE_STATIC_MODULE_ENTRY_POINT(_UserModuleClass) \
OdRxModulePtr ODRX_STATIC_MODULE_ENTRY_POINT(_UserModuleClass)()

struct STATIC_MODULE_DESC
{
  const OdChar*           szAppName;
  StaticModuleEntryPoint  entryPoint;
};

/** Description: 
    Defines the start of the static module map, which contains entries for all
    custom DWGdirect modules that are linked in statically to a DWGdirect client
    application.
*/
#define ODRX_BEGIN_STATIC_MODULE_MAP() \
STATIC_MODULE_DESC g_ODRX_STATIC_MODULE_MAP[] = {

/** Description: 
    Defines an entry in the static module map, which contains entries for all
    custom DWGdirect modules that are linked in statically to a DWGdirect client
    application.

    Arguments:
    AppName (I) Registered application name, that can be later passed to 
      OdRxDynamicLinker::loadModule to load the static application.
    ModuleClassName (I) Name of the C++ class derived from OdRxModule, that 
      implements support for this module.
*/
#define ODRX_DEFINE_STATIC_APPLICATION(AppName, ModuleClassName) \
{ AppName, ODRX_STATIC_MODULE_ENTRY_POINT(ModuleClassName) },

/** Description: 
    Defines the end of the static module map, which contains entries for all
    custom DWGdirect modules that are linked in statically to a DWGdirect client
    application.
*/
#define ODRX_END_STATIC_MODULE_MAP() \
{ 0, 0 } };

FIRSTDLL_EXPORT void odrxInitStaticModuleMap(STATIC_MODULE_DESC* pMap);

#define ODRX_INIT_STATIC_MODULE_MAP() odrxInitStaticModuleMap(g_ODRX_STATIC_MODULE_MAP)



#define ODRX_DEFINE_STATIC_MODULE(_UserModuleClass)\
ODRX_DECLARE_STATIC_MODULE_ENTRY_POINT(_UserModuleClass)\
{ return OdRefCountImpl<OdRxStaticModule<_UserModuleClass> >::createObject(); }



#if defined(_TOOLKIT_IN_DLL_) && !defined(__MWERKS__) ////////////////////////////////////////////////////////////////////////////////

#define ODRX_STATIC_MODULE_PATH "{5CEAD1EF-4D33-48fe-99E4-E09176BCF088}/"

#define ODRX_STATIC_APP_MODULE_NAME(AppName) (OdString(ODRX_STATIC_MODULE_PATH) + AppName + ".drx")

FIRSTDLL_EXPORT OdString moduleFileName(void* hModule);

/** Description:

    {group:OdRx_Classes} 
*/
template <class TBase>
class OdRxDynamicModule : public TBase
{
  void* m_hModule;
public:
  OdRxDynamicModule()
    : m_hModule(0)
  {
  }
  void* moduleHandle()
  {
    return m_hModule;
  }

  void setModuleHandle(void* handle)
  {
    m_hModule = handle;
  }

  OdString moduleFileName() const { return ::moduleFileName(m_hModule); }
};

#undef DD_BIN_COMP_VAL
#define DD_BIN_COMP_VAL(N) FIRSTDLL_EXPORT void _odrx_check_binary_compatibility_##N();

DD_BINARY_COMPATIBILITY

#undef DD_BIN_COMP_VAL

#define DD_BIN_COMP_VAL(N) ::_odrx_check_binary_compatibility_##N

/** Description:
    Creates the entry point function for a DWGdirect custom application.
*/
#define ODRX_DEFINE_DYNAMIC_MODULE(_UserModuleClass)\
__declspec(dllexport) OdRxModulePtr odrxCreateModuleObject()\
{\
  DD_BINARY_COMPATIBILITY();\
  return OdRefCountImpl<OdRxDynamicModule<_UserModuleClass> >::createObject();\
}


#else //#ifdef _TOOLKIT_IN_DLL_ ////////////////////////////////////////////////////////////////////////////////

#define ODRX_STATIC_MODULE_PATH ""

#define ODRX_STATIC_APP_MODULE_NAME(AppName) AppName

#define ODRX_DEFINE_DYNAMIC_MODULE(_UserModuleClass) ODRX_DEFINE_STATIC_MODULE(_UserModuleClass)


#endif //#ifdef _TOOLKIT_IN_DLL_ ////////////////////////////////////////////////////////////////////////////////




#endif // _OD_RXDYNAMICMODULE_H_


