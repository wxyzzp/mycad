#include "OdaCommon.h"
#include "RxObjectImpl.h"
#include "ExOdStorage.h"
#include "OdString.h"

#ifdef WIN32
#define LPWCHAR  WCHAR *
#else
#define LPWCHAR LPSTR
#endif

#ifdef _MSC_VER

ExOdOleStream::ExOdOleStream()
  : m_lpStream(NULL)
  , m_nCurPos(0)
{
}

ExOdOleStream::~ExOdOleStream()
{
  if (m_lpStream)
  {
    m_lpStream->Release();
    m_lpStream = NULL;
  }
}

void ExOdOleStream::setStream(void *pStream)
{
  m_lpStream = (LPSTREAM) pStream;
}

void* ExOdOleStream::getStream()
{
  return m_lpStream;
}

OdUInt32 ExOdOleStream::seek(OdInt32 offset, OdDb::FilerSeekType whence)
{
  if (!m_lpStream)
  {
    return 0;
  }

  ULARGE_INTEGER newpos;
  LARGE_INTEGER togo;
  togo.LowPart  = offset;
  togo.HighPart = 0L;
  m_lpStream->Seek(togo, whence, &newpos);
  m_nCurPos = newpos.LowPart;
  return newpos.LowPart;
}

void ExOdOleStream::getBytes(void* dest, OdUInt32 size)
{
  if (!m_lpStream)
  {
    return;
  }

  ULONG nByteReads;
  m_lpStream->Read(dest, size, &nByteReads);
  m_nCurPos += size;
}

OdUInt8 ExOdOleStream::getByte()
{
  if (!m_lpStream)
  {
    return 0;
  }

  OdUInt8 data;
  m_lpStream->Read(&data, 1, NULL);
  m_nCurPos++;
  return data;
}

OdUInt32 ExOdOleStream::tell()
{
  return m_nCurPos;
}


OdUInt32 ExOdOleStream::length()
{
  if (!m_lpStream)
  {
    return 0;
  }

  STATSTG stat;
  m_lpStream->Stat(&stat, STATFLAG_NONAME);

  ULARGE_INTEGER newpos;
  LARGE_INTEGER togo;
  togo.LowPart  = 0;
  togo.HighPart = 0L;
  m_lpStream->Seek(togo, STREAM_SEEK_CUR, &newpos);

  return stat.cbSize.LowPart - newpos.LowPart;
}


ExOdOleStorage::ExOdOleStorage()
  : m_lpStorage(NULL)
{
}


ExOdOleStorage::~ExOdOleStorage()
{
  if (m_pLockBytes)
  {
    m_pLockBytes->Release();
    m_pLockBytes = 0;
  }
  if (m_lpStorage)
  {
    m_lpStorage->Release();
    m_lpStorage = NULL;
  }
}

void ExOdOleStorage::setStorage(void *pStorage)
{
  m_lpStorage = (LPSTORAGE) pStorage;
}

void ExOdOleStorage::setLockBytes(void* pLockBytes)
{
  m_pLockBytes = (LPLOCKBYTES) pLockBytes;
}


#ifdef WIN32
//===================================================
//Convert a byte string to widechar string. mbtowcs 
//should have worked but doesn't.
void ToWideChar( LPWCHAR pTo, LPCSTR pFrom )
{
   int i;
   for(i=0;i<=lstrlen( pFrom ); i++)  pTo[i] = pFrom[i];
}
#endif


OdOleStreamPtr ExOdOleStorage::openStream(const OdString &streamName, const OdUInt32 offsetInStream)
{
  OdOleStreamPtr pStreamPtr = ExOdOleStream::createObject();

  LPSTREAM lpStream;
  HRESULT  result;
  if(!m_lpStorage)
  {
    return pStreamPtr;
  }

  wchar_t swc[MAX_PATH];
  ToWideChar(swc, streamName.c_str());

  result = m_lpStorage->OpenStream(swc, NULL, STGM_READ | STGM_SHARE_EXCLUSIVE,
                                    0, &lpStream);  
  if (FAILED(result))
  {
    lpStream = NULL;
    return pStreamPtr;
  }
  
  LARGE_INTEGER togo;
  togo.LowPart = offsetInStream;
  togo.HighPart = 0L;

  lpStream->Seek(togo, STREAM_SEEK_SET, NULL);
  pStreamPtr->setStream(lpStream);
  

  return pStreamPtr;
}

#endif // _MSC_VER
