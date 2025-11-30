
#ifndef _ODWINWRITERCRYPT_H_
#define _ODWINWRITERCRYPT_H_

#include <windows.h>
#include <wincrypt.h>

#include "DbSecurity.h"
#include "RxDictionary.h"
#include "OdString.h"

class DWGdirectWriterCrypt : public OdCrypt
{
  HCRYPTPROV m_hCryptProv; // the provider handle
  HCRYPTHASH m_hHash;      // the hash object
  HCRYPTKEY  m_hKey;       // the session key
public:
  ODRX_DECLARE_MEMBERS(DWGdirectWriterCrypt);

  DWGdirectWriterCrypt();
  ~DWGdirectWriterCrypt();
  void clear();

  // OdCrypt
  bool initialize(const OdSecurityParams& secParams);
  bool decryptData(OdUInt8* buffer, OdUInt32 nBufferSize);
  bool encryptData(OdUInt8* buffer, OdUInt32 nBufferSize);
};

inline void odrxInitWriterCrypt()
{
  DWGdirectWriterCrypt::rxInit();
  odrxServiceDictionary()->putAt("OdCrypt", DWGdirectWriterCrypt::desc());
}

inline void odrxUninitWriterCrypt()
{
  odrxServiceDictionary()->remove("OdCrypt");
  DWGdirectWriterCrypt::rxUninit();
}

#endif //#ifndef _ODWINWRITERCRYPT_H_
