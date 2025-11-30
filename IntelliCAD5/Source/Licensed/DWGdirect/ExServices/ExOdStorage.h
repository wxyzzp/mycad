#ifndef EXODSTORAGE_DEFINED
#define EXODSTORAGE_DEFINED

#include "OleStorage.h"

#ifdef _MSC_VER
#include "objidl.h"

class ExOdOleStream : public OdOleStream
{
  OdUInt32 m_nCurPos;
public:  
  ExOdOleStream();
 ~ExOdOleStream(); 
  static OdOleStreamPtr createObject()
  {
    return OdOleStreamPtr(new OdRxObjectImpl<ExOdOleStream>, kOdRxObjAttach);
  }
  void setStream(void *pStream);
  void *getStream();

  OdUInt32 seek(OdInt32 offset, OdDb::FilerSeekType whence);
  void     getBytes(void* buffer, OdUInt32 nLen);
  OdUInt32  tell();
  OdUInt8   getByte();
  OdUInt32  length();
  /*
  void      putByte(OdUInt8 val);
  void      putBytes(const void* buffer, OdUInt32 nLen);
  void      copyDataTo(OdStreamBuf* pDest, OdUInt32 nSrcStart, OdUInt32 nSrcEnd);
  void      truncate();


  bool      isEof();*/

protected:  
  LPSTREAM m_lpStream;

};



class ExOdOleStorage;
typedef OdSmartPtr<ExOdOleStorage> ExOdOleStoragePtr;

class ExOdOleStorage : public OdOleStorage
{
public:
  ExOdOleStorage();
 ~ExOdOleStorage();
  static ExOdOleStoragePtr createObject()
  {
    return ExOdOleStoragePtr(new OdRxObjectImpl<ExOdOleStorage>, kOdRxObjAttach);
  }
  void setStorage(void *pStorage);
  OdOleStreamPtr openStream(const OdString &streamName, const OdUInt32 offsetInStream = 0);

  void setLockBytes(void* pLockBytes);

protected:
  LPSTORAGE m_lpStorage;
  LPLOCKBYTES m_pLockBytes;

};



#endif

#endif

