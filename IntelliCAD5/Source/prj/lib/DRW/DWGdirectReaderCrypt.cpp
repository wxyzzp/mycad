#define _WIN32_WINNT 0x0400 // to enable Windows Cryptographic API

#include "OdaCommon.h"
#include "DWGdirectReaderCrypt.h"
#include "RxObjectImpl.h"

ODRX_CONS_DEFINE_MEMBERS(DWGdirectReaderCrypt, OdCrypt, RXIMPL_CONSTR);

DWGdirectReaderCrypt::DWGdirectReaderCrypt()
  : m_hCryptProv(0)
  , m_hHash(0)
  , m_hKey(0)
{}

DWGdirectReaderCrypt::~DWGdirectReaderCrypt()
{
  clear();
}

void DWGdirectReaderCrypt::clear()
{
  if(m_hHash)
    CryptDestroyHash(m_hHash);
  m_hHash = 0;
  if(m_hKey)
    CryptDestroyKey(m_hKey);
  m_hKey = 0;
  if(m_hCryptProv)
    CryptReleaseContext(m_hCryptProv, 0);
  m_hCryptProv = 0;
}

bool DWGdirectReaderCrypt::initialize(const OdSecurityParams& secParams)
{
  clear();
  
  // Get a handle to the default provider. 
  if(!CryptAcquireContextW(
    &m_hCryptProv,
    NULL,
    (LPCWSTR)secParams.provName.c_str(),
    secParams.nProvType,
    CRYPT_VERIFYCONTEXT))
  {
    ODA_TRACE0("Error during CryptAcquireContext!");
    return false;
  }
  
  // Create a hash object.
  if(!CryptCreateHash(
    m_hCryptProv,
    CALG_MD5,
    0,
    0,
    &m_hHash))
  {
    ODA_TRACE0("Error during CryptCreateHash!");
    return false;
  }
  
  // Hash in the password data. 
  if(!CryptHashData(
    m_hHash,
    (const BYTE*)secParams.password.c_str(), 
    secParams.password.getLength() * 2,
    0))
  {
    ODA_TRACE0("Error during CryptHashData!");
    return false;
  }
  
  // Derive a session key from the hash object. 
  DWORD dwFlags = secParams.nKeyLength << 16;
  dwFlags |= CRYPT_NO_SALT;
  if(!CryptDeriveKey(
    m_hCryptProv,
    secParams.nAlgId,// CALG_RC4
    m_hHash,
    dwFlags,
    &m_hKey))
  {
    ODA_TRACE0("Error during CryptDeriveKey!\n");
    return false;
  }
  
  return true;
}

// Decrypt data. 
bool DWGdirectReaderCrypt::decryptData(OdUInt8* buffer, OdUInt32 nBufferSize)
{
  DWORD dwBufferSize = nBufferSize; // Needed for win64
  if(!CryptDecrypt(m_hKey, 0, TRUE, 0, buffer, &dwBufferSize))
  {
    ODA_TRACE0("Error during CryptDecrypt!");
    return false;
  }
  return true;
}

// Encrypt data. 
bool DWGdirectReaderCrypt::encryptData(OdUInt8* buffer, OdUInt32 nBufferSize)
{
  DWORD dwBufferSize = nBufferSize; // Needed for win64
  if(!CryptEncrypt(m_hKey, 0, TRUE, 0, buffer, &dwBufferSize, dwBufferSize))
  {
    ODA_TRACE0("Error during CryptDecrypt!");
    return false;
  }
  return true;
}
