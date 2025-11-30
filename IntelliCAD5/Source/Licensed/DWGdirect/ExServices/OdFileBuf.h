#ifndef ODFILEBUF_DEFINED
#define ODFILEBUF_DEFINED

#include <stdio.h>
#include "DbSystemServices.h"
#include "OdString.h"
#include "RxObjectImpl.h"

////////////////////////////////////////////////////////////
#define ERR_VAL ((OdUInt32)-1)
class OdBaseFileBuf : public OdRxObjectImpl<OdStreamBuf>
{
  OdBaseFileBuf(const OdBaseFileBuf&);
  OdBaseFileBuf& operator = (const OdBaseFileBuf&);

public:
          OdBaseFileBuf();
  virtual ~OdBaseFileBuf()  {}

  virtual void      open(const OdChar* path, Oda::FileShareMode shMode = Oda::kShareDenyNo) = 0;
  virtual void      close();
  virtual OdString  fileName() { return m_FileName; }

protected:
  FILE *              m_fp;
  OdString            m_FileName;
  OdUInt32            m_length;
  Oda::FileShareMode  m_shMode;

  void open(const OdChar *path, const char * mode);
};

class OdWrFileBuf;
typedef OdSmartPtr<OdWrFileBuf> OdWrFileBufPtr;
/////////////////////////////////////////////////////////////
class OdWrFileBuf : public OdBaseFileBuf
{
  OdWrFileBuf(const OdWrFileBuf&);
  OdWrFileBuf& operator = (const OdWrFileBuf&);

  OdUInt32  m_position;

public:
  OdWrFileBuf(const OdChar *path) { open(path); }
  OdWrFileBuf(const OdChar *path, Oda::FileShareMode shMode) { open(path, shMode); }
  OdWrFileBuf();
  ~OdWrFileBuf();
  static OdWrFileBufPtr createObject()
  {
    return OdWrFileBufPtr(new OdWrFileBuf(), kOdRxObjAttach);
  }
  static OdWrFileBufPtr createObject(const OdChar *path, Oda::FileShareMode shMode = Oda::kShareDenyNo)
  {
    return OdWrFileBufPtr(new OdWrFileBuf(path, shMode), kOdRxObjAttach);
  }

  void open(const OdChar *path, Oda::FileShareMode shMode = Oda::kShareDenyNo);
  void close();

  // OdStreamBuf methods
  OdUInt32  length();
  OdUInt32  seek(OdInt32 offset, OdDb::FilerSeekType whence);
  OdUInt32  tell();
  bool      isEof();
  void      putByte(OdUInt8 val);
  void      putBytes(const void* buffer, OdUInt32 nLen);
  OdUInt32  getShareMode();
};


//////////////////////////////////////////////////////////////////////
#define NUM_BUFFERS 8

class OdRdFileBuf;
typedef OdSmartPtr<OdRdFileBuf> OdRdFileBufPtr;

class OdRdFileBuf : public OdBaseFileBuf
{
  OdRdFileBuf(const OdRdFileBuf&);
  OdRdFileBuf& operator = (const OdRdFileBuf&);

public:
  OdRdFileBuf(const OdChar *path) : m_Counter(0L) { init(); open(path); }
  OdRdFileBuf(const OdChar *path, Oda::FileShareMode shMode) : m_Counter(0L)
  { 
    init();
    open(path, shMode); 
  }
  OdRdFileBuf();
  static OdRdFileBufPtr createObject()
  {
    return OdRdFileBufPtr(new OdRdFileBuf(), kOdRxObjAttach);
  }
  static OdRdFileBufPtr createObject(const OdChar *path, Oda::FileShareMode shMode = Oda::kShareDenyNo)
  {
    return OdRdFileBufPtr(new OdRdFileBuf(path, shMode), kOdRxObjAttach);
  }
  ~OdRdFileBuf();

  void open(const OdChar *path, Oda::FileShareMode shMode = Oda::kShareDenyNo);
  void close();

  OdUInt32  length();
  OdUInt32  seek(OdInt32 offset, OdDb::FilerSeekType whence);
  OdUInt32  tell();
  bool      isEof();
  OdUInt8   getByte();
  void      getBytes(void* buffer, OdUInt32 nLen);
  OdUInt32  getShareMode();

protected:
  struct blockstru
  {
    OdUInt8*  buf;        /* this buffer */
    OdUInt32  startaddr;  /* address from which it came in the file */
    int       validbytes; /* number of valid bytes it holds */
    OdInt32   counter;    /* least recently used counter */
  };

  OdUInt32  m_PhysFilePos; /* where the file pointer is */
  OdUInt32  m_BufPos;      /* position from which buf was filled */
  int       m_BytesLeft;   /* bytes left in buf */
  int       m_BufBytes;    /* valid bytes read into buffer */
  OdUInt8*  m_pNextChar;   /* pointer to next char in buffer */
  OdUInt8*  m_pCurBuf;     /* pointer to the buffer currently being used */
  int       m_UsingBlock;  /* which block is currently in use */
  struct blockstru         /* the data being held */
            m_DataBlock[NUM_BUFFERS];

  static const int m_BufSize; /* size of each read buffer */
  static const int  m_PosMask; /* mask to allow position check */
  OdInt32    m_Counter;

  bool filbuf();
  void init();
};


#endif // ODFILEBUF_DEFINED

