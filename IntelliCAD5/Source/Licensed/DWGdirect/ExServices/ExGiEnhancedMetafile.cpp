// ExGiEnhancedMetafile.cpp : implementation of the GiEnhancedMetafile class
//
// AE 11.04.2003 - Begin 

#include <windows.h>
#include "OdaCommon.h"
#include "RxObjectImpl.h"
#include "OdBinaryData.h"
#include "ExGiEnhancedMetafile.h"


//**************************************************************************************/
// Constructor
//**************************************************************************************/
OdExGiEnhancedMetafile::OdExGiEnhancedMetafile()
  : m_HEMF(NULL)
{

}

//**************************************************************************************/
// Destructor
//**************************************************************************************/
OdExGiEnhancedMetafile::~OdExGiEnhancedMetafile()
{
  deleteMetafile();
}

OdGiEnhancedMetafilePtr OdExGiEnhancedMetafile::createObject()
{
  return OdGiEnhancedMetafilePtr(new OdRxObjectImpl<OdExGiEnhancedMetafile>, kOdRxObjAttach);
}

//**************************************************************************************/
// loadMetafile
//**************************************************************************************/
bool OdExGiEnhancedMetafile::loadMetafile(OdStreamBuf *pBuf)
{
  bool bRes = false;

  deleteMetafile();


  OdInt32 size = pBuf->length();
  OdBinaryData data;
  data.resize(size);
  pBuf->getBytes(data.asArrayPtr(), size);

  
 	// First try to read it as an enhanced metafile
  m_HEMF = ::SetEnhMetaFileBits(size, data.asArrayPtr());
  if (!m_HEMF) // It was apparently not an enhanced metafile, so try 16bit windows metafile
  {
    m_HEMF = ::SetWinMetaFileBits(size, data.asArrayPtr(), NULL, NULL);
  }

  return (bRes = (m_HEMF != NULL));
}

//**************************************************************************************/
// deleteMetafile
//**************************************************************************************/
void OdExGiEnhancedMetafile::deleteMetafile()
{
  if (m_HEMF)
  {
    ::DeleteEnhMetaFile(m_HEMF);
    m_HEMF = NULL;
  }
}

void* OdExGiEnhancedMetafile::getHandle() const
{
  return (void *)m_HEMF;
}

// AE - End 
