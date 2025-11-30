
#ifndef _ODWINREADERCRYPT_H_
#define _ODWINREADERCRYPT_H_

#include <windows.h>
#include <wincrypt.h>

#include "DbSecurity.h"
#include "RxDictionary.h"
#include "OdString.h"

class DWGdirectReaderCrypt : public OdCrypt
{
  HCRYPTPROV m_hCryptProv; // the provider handle
  HCRYPTHASH m_hHash;      // the hash object
  HCRYPTKEY  m_hKey;       // the session key
public:
  ODRX_DECLARE_MEMBERS(DWGdirectReaderCrypt);

  DWGdirectReaderCrypt();
  ~DWGdirectReaderCrypt();
  void clear();

  // OdCrypt
  bool initialize(const OdSecurityParams& secParams);
  bool decryptData(OdUInt8* buffer, OdUInt32 nBufferSize);
  bool encryptData(OdUInt8* buffer, OdUInt32 nBufferSize);
};

inline void odrxInitReaderCrypt()
{
  DWGdirectReaderCrypt::rxInit();
  odrxServiceDictionary()->putAt("OdCrypt", DWGdirectReaderCrypt::desc());
}

inline void odrxUninitReaderCrypt()
{
  odrxServiceDictionary()->remove("OdCrypt");
  DWGdirectReaderCrypt::rxUninit();
}

#endif //#ifndef _ODWINREADERCRYPT_H_
