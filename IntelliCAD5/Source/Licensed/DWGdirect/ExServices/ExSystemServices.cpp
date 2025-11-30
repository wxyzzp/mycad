
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4018) // signed/unsigned mismatch
#pragma warning(disable : 4146) // unary minus operator applied to unsigned type, result still unsigned

#ifndef _WIN32_WCE
#include <io.h>
#endif

#pragma warning(pop)
#endif

#include "OdaCommon.h"
#include "ExSystemServices.h"
#include "OdFileBuf.h"
#include "RxObjectImpl.h"
#include "RxDefs.h"
#include "RxDictionary.h"
#include "RxModule.h"
#include "OdToolKit.h"
#include "ExGiRasterImage.h"
#include "FlatMemStream.h"
#include "ColorMapping.h"
#include "Gs/GsBMPDevice.h"
#include "Gs/Gs.h"
#include "OdTimeStamp.h"

#define STL_USING_ALGORITHM
#include "OdaSTL.h"
bool IsCompound(OdStreamBuf *pBuf);

ExSystemServices::ExSystemServices()
  : m_CodePageId(CP_UNDEFINED)
{
#ifdef _WIN32
  switch(::GetACP())
  {
  case 874:
    m_CodePageId = CP_ANSI_874;
    break;
  case 932:
    m_CodePageId = CP_ANSI_932;
    break;
  case 936:
    m_CodePageId = CP_ANSI_936;
    break;
  case 949:
    m_CodePageId = CP_ANSI_949;
    break;
  case 950:
    m_CodePageId = CP_ANSI_950;
    break;
  case 1200:
    m_CodePageId = CP_ANSI_1200;
    break;
  case 1250:
    m_CodePageId = CP_ANSI_1250;
    break;
  case 1251:
    m_CodePageId = CP_ANSI_1251;
    break;
  case 1252:
    m_CodePageId = CP_ANSI_1252;
    break;
  case 1253:
    m_CodePageId = CP_ANSI_1253;
    break;
  case 1254:
    m_CodePageId = CP_ANSI_1254;
    break;
  case 1255:
    m_CodePageId = CP_ANSI_1255;
    break;
  case 1256:
    m_CodePageId = CP_ANSI_1256;
    break;
  case 1257:
    m_CodePageId = CP_ANSI_1257;
    break;
  }
#endif
}

OdStreamBufPtr ExSystemServices::createFile(
    const OdChar* pcFilename,                   // file name
    Oda::FileAccessMode nDesiredAccess,
    Oda::FileShareMode nShareMode,
    Oda::FileCreationDisposition /*nCreationDisposition*/)

{
	OdSmartPtr<OdBaseFileBuf> pFile;

  // MKU 19.02.2004   It was corrected after debugging with BORLANDC. It hangs on open() statement 
  //                  in case of using conditional operator: ? :.
  //
  // typedef OdSmartPtr<OdBaseFileBuf> OdBaseFileBufPtr;
	// OdBaseFileBufPtr pFile;
  // pFile = (nDesiredAccess == Oda::kFileRead ?
  //   OdBaseFileBufPtr(OdRdFileBuf::createObject()) :
  //   OdBaseFileBufPtr(OdWrFileBuf::createObject()));

  if(pcFilename && pcFilename[0])
  {
    if (nDesiredAccess == Oda::kFileRead)
    {
      pFile = OdRdFileBuf::createObject();
    }
    else
    {
      pFile = OdWrFileBuf::createObject();
    }

    pFile->open(pcFilename, nShareMode);
  }
  else
  {
    throw OdError(eNoFileName);
  }
	return OdStreamBufPtr(pFile);
}


bool ExSystemServices::accessFile(const OdChar* pcFilename, int mode)
{
#if defined(_MSC_VER) && !defined(_WIN32_WCE)
  return (_access(pcFilename,
    (GETBIT(mode, Oda::kFileRead)  ? 0x04 : 0x00) | 
    (GETBIT(mode, Oda::kFileWrite) ? 0x02 : 0x00)) == 0);
#else
  try
  {
    createFile(pcFilename, (Oda::FileAccessMode)mode);
    return true;
  }
  catch(...) 
	{
		return false; // Do NOT remove this, or else some compilers (e.g. cw7 mac) will optimize out the catch! 
	}
  return false;
#endif
}

#if defined _WIN32 & !defined(_WIN32_WCE)

#include <time.h>
#include <sys/stat.h>

long ExSystemServices::packedTime(const OdTimeStamp &Time)
{
  struct tm Tm;
  short msec, tm_mon, tm_mday, tm_year, tm_hour, tm_min, tm_sec;

  Time.getDate( tm_mon, tm_mday, tm_year);
  Time.getTime( tm_hour, tm_min, tm_sec, msec );

  Tm.tm_isdst = 0;
  Tm.tm_mon  = tm_mon - 1;
  Tm.tm_mday = tm_mday;
  Tm.tm_year = tm_year - 1900;
  Tm.tm_hour = tm_hour;
  Tm.tm_min  = tm_min;
  Tm.tm_sec  = tm_sec;

  return mktime( &Tm );
}

bool ExSystemServices::getstrftime(const OdTimeStamp &Time, const char *pFormat, OdString &str)
{
  if (!pFormat)
    return false;

  struct tm Tm;
  short msec, tm_mon, tm_mday, tm_year, tm_hour, tm_min, tm_sec;

  Time.getDate( tm_mon, tm_mday, tm_year);
  Time.getTime( tm_hour, tm_min, tm_sec, msec );

  Tm.tm_mon  = tm_mon - 1;
  Tm.tm_mday = tm_mday;
  Tm.tm_year = tm_year - 1900;
  Tm.tm_hour = tm_hour;
  Tm.tm_min  = tm_min;
  Tm.tm_sec  = tm_sec;

  char buf[256];
  if (!strftime(buf, 256, pFormat/*"%c"*/, &Tm)) // strftime(strDest, maxsize, pFormat/*"%c"*/, &Tm)
    return false;

  str = buf;
  
  return true;
}

bool ExSystemServices::getctimestr(const OdTimeStamp &, OdString &)
{
  return false;
}

bool ExSystemServices::localToUniversal(OdTimeStamp &)
{
  return false;
}

bool ExSystemServices::UniversalTolocal(OdTimeStamp &)
{
  return false;
}

bool ExSystemServices::getUniversalTime(OdTimeStamp &)
{
  return false;
}

bool ExSystemServices::getLocalTime (OdTimeStamp &)
{
  return false;
}

OdUInt32 ExSystemServices::getSystemTime(OdChar* strDest, OdUInt32 maxsize, OdInt32* seconds)
{
  // Returns the number of characters placed in strDest if the total number of 
  //  resulting characters, including the terminating null, is not more than maxsize. 
  //  Otherwise, the functions return 0, and the contents of strDest is indeterminate.
  // seconds returns the time in elapsed seconds.
  time_t ltime;
  time(&ltime);
  struct tm * pTm = localtime(&ltime);
  if (seconds)
  {
    *seconds = ltime;
  }
  return strftime(strDest, maxsize, "%c", pTm);
}

long ExSystemServices::getFileCTime(const OdChar* name)
{
  struct _stat st;
  if (_stat(name, &st)) return -1;
  return st.st_ctime;
}

long ExSystemServices::getFileMTime(const OdChar* name)
{
  struct _stat st;
  if (_stat(name, &st)) return -1;
  return st.st_mtime;
}

OdInt64 ExSystemServices::getFileSize(const OdChar* name)
{
#if defined(WIN32) && !defined(_WIN32_WCE)
  WIN32_FIND_DATA fd;
  if (HANDLE hf = ::FindFirstFile(name, &fd))
  {
    FindClose(hf);
    return (OdInt64(fd.nFileSizeLow) | (OdInt64(fd.nFileSizeHigh) << 32));
  }
  return OdInt64(-1);
#else
  struct _stat st;
  if (_stat(fname, &st)) return OdInt64(-1);
  return OdInt64(st.st_size);
#endif
}

#else // _WIN32

OdUInt32 ExSystemServices::getSystemTime(OdChar* strDest, OdUInt32 maxsize, OdInt32* seconds)
{
  return 0;
}

long ExSystemServices::getFileCTime(const OdChar* name)
{
  return (-1);
}

long ExSystemServices::getFileMTime(const OdChar* name)
{
  return (-1);
}
OdInt64 ExSystemServices::getFileSize(const OdChar* name)
{
  return OdInt64(-1);
}
#endif  // _WIN32

#ifdef _WIN32
#include <windows.h>

#endif

bool IsCompound(OdStreamBuf *pBuf)
{
  static OdUInt8 docID[8]={ 0xd0, 0xcf, 0x11, 0xe0, 0xa1, 0xb1, 0x1a, 0xe1 };  
  OdUInt8  b8[8];
  int   i;

  pBuf->seek(128, OdDb::kSeekFromStart);
  pBuf->getBytes(b8, 8);
  pBuf->seek(0, OdDb::kSeekFromStart);
  for (i=0; i<8; i++)
  {
    if (b8[i] != docID[i]) 
    {
      return false;
    }
  }
  return true;
}

OdGsDevicePtr ExSystemServices::gsBitmapDevice()
{
    OdUInt32 gsModuleId = ::odrxDynamicLinker()->loadModule( "WinGDI");
    if(gsModuleId)
    {
      OdGsModulePtr pGsModule = ::odrxDynamicLinker()->module(gsModuleId);
      return pGsModule->createBitmapDevice();
    }

	return OdGsDevicePtr();
}

OdString ExSystemServices::formatMessage(unsigned int code, va_list* argList)
{
  static const OdChar* message[] =
  {
#define OD_ERROR_DEF(cod, desc)  desc,
#include "ErrorDefs.h"
#undef OD_ERROR_DEF

#define OD_MESSAGE_DEF(cod, desc) desc,
#include "MessageDefs.h"
#undef OD_MESSAGE_DEF
    ""// DummyLastMassage
  };

  OdString msg;
  if (code >= sidDummyLastMassage)
  {
    msg.format("Unknown message (code: %d)", code);
    return msg;
  }

  if (argList)
  {
    msg.formatV(message[code], *argList);
  }
  else msg = message[code];

  return msg;
}

OdCodePageId ExSystemServices::systemCodePage() const
{
  return m_CodePageId;
}

void ExSystemServices::setSystemCodePage(OdCodePageId id)
{
  m_CodePageId = id;
}


