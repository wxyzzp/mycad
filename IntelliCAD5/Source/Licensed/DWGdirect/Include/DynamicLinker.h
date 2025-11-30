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



// DynamicLinker.h: interface for the OdaDynamicLinkerI class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_ODADYNAMICLINKERI_H_INCLUDED_)
#define _ODADYNAMICLINKERI_H_INCLUDED_

class OdRxDictionary;
class OdDbSystemServices;
class OdRxDLinkerReactor;

#include "RxDictionary.h"

class OdRxModule;
typedef OdSmartPtr<OdRxModule> OdRxModulePtr;

#include "DD_PackPush.h"

/** Description:

    {group:OdRx_Classes} 
*/
class FIRSTDLL_EXPORT OdRxDynamicLinker : public OdRxObject
{
public:
  ODRX_DECLARE_MEMBERS(OdRxDynamicLinker);

  virtual void        addReactor    (OdRxDLinkerReactor* pReactor) = 0;
  virtual void        removeReactor (OdRxDLinkerReactor* pReactor) = 0;

  // Loads the dynamic extension from the file. The moduleFileName parameter should contain 
  // full path to the library, no assumptions for file path and/or extension are made. 
  virtual OdUInt32    loadModule    (const OdString& moduleFileName, bool bSilent = true) = 0;

  // Loads the named application. The applicationName should contain only the name of the DWGDirect
  // application and (optionally) the file extension to override the ".drx" extension, which will
  // be added by default to the file name if no extension is supplied. 
  // ----------
  // SUBJECT TO CHANGE:
  // The dynamic loader will search for the files, specified in module search path (which can be set
  // by odrxSetModuleSearchPath function). By default the application will search for the extension
  // modules in the current application directory.
  // ----------
  virtual OdUInt32    loadApp       (const OdString& applicationName, bool bSilent = true) = 0;

  // Obtains the ID of the module, specified by full module name (should be used whenever the module
  // was loaded by loadModule method with full file path specification).
  virtual OdUInt32    moduleId      (const OdString& moduleFileName) = 0;

  // The method unloads the module with specified ID
  virtual bool        unloadModule  (OdUInt32 moduleId) = 0;

  // Returns the smart pointer of the module, specified by module ID
  virtual OdRxModulePtr       module(OdUInt32 moduleId) = 0;

  // Qyeries whether the module is locked (used by any other object in system).
  virtual bool        isModuleLocked(OdUInt32 moduleId) = 0;


//---------------------------
// AES 13-Jul-2004 added the following functions
  
  // Queries whether the application is loaded; the name of the application should match the
  // parameter passed to loadApp method. It should be a short module name optionally followed 
  // by file extension; if no extension is specified ".drx" is assumed.
  virtual bool        isAppLoaded   ( const OdString& strApplicationName ) = 0;

  // Returns the identifier of the application, specified by its name. The name must contain only
  // the name of application (optional file extension is possible). If extension is omitted the
  // system will assume that the file has ".drx" extension.
  virtual OdUInt32    appId         ( const OdString& strApplicationName ) = 0;
  
  // The method unloads all loaded application in order, reverse to their loading order. This 
  // verifies that dependant applications are loaded correctly. All applications and modules 
  // (even locked ones) are unloaded forcedly. The method should be called on system shutdown 
  // only.
  virtual bool        unloadAll     ( void ) = 0;
//---------------------------


  /*
  virtual void*       getSymbolAddress (const OdString& serviceName,
                                        const OdString& symbol) const = 0;
  virtual bool        initListedModules(const OdString& fileName) = 0;

  virtual OdRxObject* registerService  (const OdString& serviceName,
                                        OdRxService* serviceObj) = 0;


  virtual bool        isAppMDIAware    (const OdString& modulename) const = 0;
  virtual bool        registerAppMDIAware(void* appId) const =  0;
  virtual bool        registerAppNotMDIAware(void* appId) const = 0;
 
  virtual bool        isAppBusy         (const OdString& modulename) const =  0;
  virtual void        setAppBusy        (const OdString& modulename, bool bBusyFlag) const =  0;
  */
};

/** Description:
    Performs DWGdirect system initialization.  This function registers all classes supported by 
    DWGdirect, so that instances of these classes may be created by client applications. It also 
    allow the user to set the OdDbSystemServices instance that will be used by DWGdirect for 
    file creation and file access.  This function should be called once per process, prior to any
    other DWGdirect calls.
*/
TOOLKIT_EXPORT void odInitialize(OdDbSystemServices* pSystemServices);

/** Description:
    Releases the dynamic class registration data and the OdDbSystemServices pointer 
    associated with the earlier call to odInitialize.  This function should be called once per process,
    as the last operation performed by DWGdirect.
*/
TOOLKIT_EXPORT void odUninitialize();

typedef OdSmartPtr<OdRxDynamicLinker> OdRxDynamicLinkerPtr;


/** Description:
    Returns the DWGdirect global dynamic linker.
*/
FIRSTDLL_EXPORT OdRxDynamicLinkerPtr odrxDynamicLinker();


#include "DD_PackPop.h"

#endif // !defined(_ODADYNAMICLINKERI_H_INCLUDED_)


