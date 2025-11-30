#ifndef _DWFIMPORT_INCLUDED_
#define _DWFIMPORT_INCLUDED_

#include "RxObject.h"
#include "RxDictionary.h"

#ifdef  _TOOLKIT_IN_DLL_
  #ifdef DWFIMPORT_EXPORTS
    #define DWFIMPORT_TOOLKIT   __declspec(dllexport)
  #else  // DWFIMPORT_EXPORTS
    #define DWFIMPORT_TOOLKIT   __declspec(dllimport)
  #endif // DWFIMPORT_EXPORTS
#else   // _TOOLKIT_IN_DLL_
  #define DWFIMPORT_TOOLKIT
#endif  // _TOOLKIT_IN_DLL_

class OdDbDatabase;

class OdDwfImport : public OdRxObject
{
public:
  enum ImportResult { success, fail, bad_password, bad_file, bad_database };
  virtual ImportResult import() = 0;

  // Documented properties are:
  // "Database"    - OdDbDatabase object, where dwf is imported
  // "DwfPath"     - string, path to the imported dwf file
  // "Password"    - string, (default empty), password to dwf if required
  // "PaperWidth"  - double, (default 297),   paper width in millimeters
  // "PaperHeight" - double, (default 210),   paper height in millimeters
  //
  virtual OdRxDictionaryPtr properties() = 0;
};

typedef OdSmartPtr<OdDwfImport> OdDwfImportPtr;

// export symbol - "?createImporter@@YA?AV?$OdSmartPtr@VOdDwfImport@@@@XZ"
DWFIMPORT_TOOLKIT OdDwfImportPtr createImporter();

#endif // _DWFIMPORT_INCLUDED_
