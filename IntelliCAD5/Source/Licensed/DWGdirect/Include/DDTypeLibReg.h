#ifndef _DDTYPELIBREG_INCLUDED_
#define _DDTYPELIBREG_INCLUDED_

#include "DDVersion.h"

#define DD_TYPELIB_REG_KEY "SOFTWARE\\Classes\\TypeLib"


inline long ddTypeLibReg(REFGUID libGuid, LPCTSTR szTLibPathName, LPCTSTR szModuleName)
{
  long nRet;
  CRegKey cReg, cRoot;
  nRet = cRoot.Open(HKEY_LOCAL_MACHINE, DD_TYPELIB_REG_KEY);
  if (ERROR_SUCCESS == nRet)
  {
    CRegKey  cNew;
    TCHAR    caPath[256];
    OLECHAR  oleGuidString[64];
    
    StringFromGUID2(libGuid, oleGuidString, 63);
    _bstr_t bTmp(oleGuidString);
    
    nRet = cNew.Create(cRoot, (LPCTSTR)bTmp);
    if (ERROR_SUCCESS == nRet)
    {
      CRegKey  cSon1;
      CRegKey  cSon2;
      long     l;
      
      l = lstrlen(szTLibPathName);
      while (szTLibPathName[--l] != '\\');
      lstrcpyn(caPath, szTLibPathName, ++l);
      
      OdString sVer;
      sVer.format("%d.%02d", DD_MAJOR_VERSION, DD_MINOR_VERSION);

      nRet = cSon1.Create((HKEY)cNew, sVer);
      if (ERROR_SUCCESS == nRet)
      {
        OdString name;
        name.format("%s Type Library %s", szModuleName, sVer.c_str());
        
        cSon1.SetValue(name);
        
        cSon2.Create((HKEY)cSon1, "0");
        cSon2.SetKeyValue("Win32", szTLibPathName);
        cSon2.Close();
        
        cSon2.Create((HKEY)cSon1, "HELPDIR");
        cSon2.SetValue(caPath);
        cSon2.Close();
        
        cSon2.Create((HKEY)cSon1, "FLAGS");
        cSon2.SetValue("0");
        cSon2.Close();
        
        cSon1.Close();
      }
      cNew.Close();
    }
    cReg.Close();
  }
  cRoot.Close();
  return nRet;
}

inline long ddTypeLibUnreg(REFGUID libGuid)
{
  OLECHAR  oleGuidString[64];
  CRegKey  cReg;
  StringFromGUID2(libGuid, oleGuidString, 63);
  _bstr_t  bTmp(oleGuidString);
  long nRet = cReg.Open(HKEY_LOCAL_MACHINE, DD_TYPELIB_REG_KEY);
  if (ERROR_SUCCESS == nRet)
  {
    CRegKey  cReg2;
    long nRet = cReg2.Open(cReg, (LPCTSTR)bTmp);
    if (ERROR_SUCCESS == nRet)
    {
      OdString sVer;
      sVer.format("%d.%02d", DD_MAJOR_VERSION, DD_MINOR_VERSION);
      cReg2.RecurseDeleteKey(sVer);
    }
    nRet = RegEnumKey(cReg2, 0, 0, 0);
    cReg2.Close();
    if(ERROR_NO_MORE_ITEMS==nRet)
    {
      cReg.RecurseDeleteKey((LPCTSTR)bTmp);
    }
    cReg.Close();
  }
  return nRet;
}

#endif //#ifndef _DDTYPELIBREG_INCLUDED_

