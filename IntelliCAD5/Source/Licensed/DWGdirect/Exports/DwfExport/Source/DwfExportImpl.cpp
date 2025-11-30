///////////////////////////////////////////////////////////////////////////////
// Copyright © 2002, Open Design Alliance Inc. ("Open Design") 
// 
// This software is owned by Open Design, and may only be incorporated into 
// application programs owned by members of Open Design subject to a signed 
// Membership Agreement and Supplemental Software License Agreement with 
// Open Design. The structure and organization of this Software are the valuable 
// trade secrets of Open Design and its suppliers. The Software is also protected 
// by copyright law and international treaty provisions. You agree not to 
// modify, adapt, translate, reverse engineer, decompile, disassemble or 
// otherwise attempt to discover the source code of the Software. Application 
// programs incorporating this software must include the following statment 
// with their copyright notices:
//
//      DWGdirect © 2002 by Open Design Alliance Inc. All rights reserved. 
//
// By use of this software, you acknowledge and accept the terms of this 
// agreement.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
// DwfExportImpl.cpp : implementation of the CDwfExportImpl class
//
///////////////////////////////////////////////////////////////////////////////

//#ifdef _WIN32
#define STL_USING_IOSTREAM
#include "OdaSTL.h"
//#endif


#include "OdaCommon.h"
#include "DbDatabase.h"
#include "DbObjectIterator.h"
#include "DbBlockTableRecord.h"
#include "DbEntity.h"
#include "DbSymbolTable.h"
#include "DbBlockTable.h"
#include "DbViewTable.h"
#include "DbViewTableRecord.h"
#include "DbLayout.h"
#include "DbDictionary.h"
#include "DbExtents.h"
#include "OdTimeStamp.h"

#include "DbHostAppServices.h"
#include "DbGsManager.h"
#include "GiContextForDbDatabase.h"
#include "ColorMapping.h"

#include "2dSupport.h"
#include "DwfDrawObject.h"
#include "DwfExportImpl.h"
#include "DwfSupport.h"

#if defined(_TOOLKIT_IN_DLL_) || !defined(DD_CLIENT_BUILD)
#include "whiptk/palette.h"
#else
extern const WT_RGBA32 WD_Old_Default_Palette[256];
extern const WT_RGBA32 WD_New_Default_Palette[256];
#endif

#ifdef _WIN32
#include <direct.h>
#endif
#include <sys/stat.h>
#include <errno.h>

/*
#include <io.h>    // _access

#include "Dwf\dwf_core.h"
#include "Dwf\Utilities\VectorUtil.h"
#include "Dwf\Utilities\GuidGen.h"
#include "Dwf\Writer\IMP\FileResourceHandler.h"
#include "Dwf\EPlot\Writer\EPlotSectionManager.h"
#include "Dwf\EPlot\EPlotInterface.h"
*/
#define STL_USING_MAP
#define STL_USING_VECTOR
#include "OdaSTL.h"

using DWF::DwfResult;


/*
using namespace DWFManifest;
using namespace DWFXmlObjects;
using namespace DWFEPlot;
using namespace DWFWriter;
*/

//using namespace std;

//#ifdef ODA_MAC
//#include <console.h>
//#endif



//***************************************************************************
/*
OdString g_sDwfFileName;                  // the output filename
OdArray<OdString>     g_arrayW2Dfiles;    // the storage of names of the temporary graphic files
std::vector<OdString> g_temp_files;       // paths to all temp files
OdArray<int>          g_arrayParamIndexes;// the storage of indexes of params corresponding to initialized layouts
OdArray<OdString>     g_arrayLayoutNames;
OdArray<OdDbObjectId> g_arrayLayoutIds;

OdArray<dwfPlotInfo>  g_arrayPlotInfo;
*/

struct DwfLayoutInfo
{
  OdString          m_sW2Dfiles;      // the storage of the names of the temporary graphic files 
                                      //  corresponding to layout
  OdString          m_sLayoutNames;
  OdDbObjectId      m_idLayout;
  DwfLayoutPlotInfo m_PlotInfo;
  int               m_iParamIndex;    // the storage of the indexes of params corresponding to 
                                      //  initialized layouts
};

OdString g_sDwfFileName;                  // the output filename
std::vector<OdString> g_temp_files;       // paths to all temp files
OdArray<DwfLayoutInfo> g_arrayLayoutInfo;


//static const double kMmPerInch = 25.3999994451349;


//---------------------------------------------------------------------------
// cleanUp()
//
//  Cleans up all temporary files & directories
//
static void CleanUp()
{
  int iRet(0);  

  while (!g_temp_files.empty())
  {
    OdString sPath( g_temp_files.back().c_str() );
 
    struct _stat stFileInfo;                        // status information on a file
    if (   _stat(sPath.c_str(), &stFileInfo) == 0   // returns 0 if the file-status information is obtained
        && (stFileInfo.st_mode & _S_IFDIR)
       )
    {
#ifdef _WIN32
        iRet = _rmdir(sPath.c_str());
#else if defined(linux)
        iRet = remove(sPath.c_str());
#endif
    }
    else
    {
        iRet = remove(sPath.c_str());
    }
/*
    if (_access(sPath.c_str(), 0) == 0)             // If it exists up to here
*/
    if (iRet == (-1))
    {
      //ODA_ASSERT(false);                            //  ...error. Unable to delete file
      OdString sErrnoPath(strerror( errno ));
      std::cout << "\nexportDwf: could not delete " << sErrnoPath.c_str() << std::endl;
    }

    g_temp_files.pop_back();
  }
}


//---------------------------------------------------------------------------
// GetTempFile()
//
//  Create a unique filename in the working directory
//
static OdString GetTempFile( const OdString& sSeed)
{
  OdString sFilename( sSeed );

  // Remove the leading slash (if any) and disk befor it (if any)
  int iSlash = sFilename.find('\\');
	if (iSlash != (-1))
  {
    sFilename = sFilename.right(sFilename.getLength() - iSlash - 1);
  }
  else
  {
    iSlash = 0;
  }

  iSlash = sFilename.reverseFind('.');
	if (iSlash == (-1))
  {
    ODA_FAIL();
    throw OdError(eUnknownFileType);
  }

  OdString sPrefix = sFilename.left(iSlash);
  OdString sExtension = sFilename.mid(iSlash);

  // MKU 06.07.2004   #2022
  // GetTempFileName() uses the first three characters of this string as the prefix of the filename.
  //  It should to be sure in this.
  iSlash = sPrefix.reverseFind('\\');
	if (iSlash != (-1))
  {
    sPrefix = sPrefix.right(sPrefix.getLength() - iSlash - 1);
  }
  if (sPrefix.getLength() < 3)
  {
    sPrefix += "xx";
  }

  // Create a temporary filename.
  //
#ifdef _WIN32

  TCHAR bufPath[MAX_PATH];
  TCHAR bufTmpFileName[MAX_PATH]; // /*MAX_FNAME*/

  long nBufferLength = GetTempPath( MAX_PATH, bufPath );

  if (nBufferLength == 0)
  {
    ODA_FAIL();
    throw OdError(eCantOpenFile);     // If the function fails, the return value is zero.
  }

  if (nBufferLength > MAX_PATH)
  {
    ODA_FAIL();
    throw OdError(eCantOpenFile); // If the return value is greater than nBufferLength, 
                                  //  the return value is the size of the buffer required to hold the path.
  }

  if ( GetTempFileName( bufPath,        // directory name
                        sPrefix,        // file name prefix
                        0,              // If uUnique is zero, the function uses a hexadecimal string 
                                        //  derived from the current system time. In this case, the function 
                                        //  uses different values until it finds a unique file name, 
                                        //  and then it creates the file in the lpPathName directory.
                        bufTmpFileName  // file name buffer
                      ) == 0 )
  {
    ODA_FAIL();
    throw OdError(eCantOpenFile);       // If the function fails, the return value is zero.
  }

  OdString sTmpFilename = bufTmpFileName;
  sFilename = sTmpFilename.left( sTmpFilename.find('.') );
  sFilename += sExtension.c_str();

  g_temp_files.push_back( sTmpFilename ); // keep file with the .tmp extension in mind for cleaning at the end
  g_temp_files.push_back( sFilename );    // keep the working file in mind for cleaning at the end

#else if defined(linux)

  // The mkstemp() function generates a unique temporary file name from template. 
  //  The last six characters of template must be XXXXXX and these are replaced with a string 
  //  that makes the filename unique.

  TCHAR bufTmpFileName[L_tmpnam];
  OdString sTmpFilename(sPrefix);
  sTmpFilename += "XXXXXX";
  strcpy( bufTmpFileName, sTmpFilename.c_str() );
/*
  if (tmpnam(bufTmpFileName) == NULL)
  {
    ODA_FAIL();
    throw OdError(eCantOpenFile);       // If the function fails, the return value is zero.
  }
*/
  if (mkstemp(bufTmpFileName) == (-1))
  {
    ODA_FAIL();
    throw OdError(eCantOpenFile);       // If the function fails, the return value is zero.
  }

  sTmpFilename = bufTmpFileName;
  sFilename += sExtension.c_str();
  g_temp_files.push_back( sFilename );    // keep the working file in mind for cleaning at the end

#endif

  return sFilename;
}


//---------------------------------------------------------------------------
// SPecifyImageMimeType()
//
//  Specifies the file resource's MIME-type
//  Returns true for jpg or png files.
//  In other cases there is no appropriate file resource's MIME-type
//
static bool SpecifyImageMimeType(OdString& sMimeType, const char * pImage)
{
  bool bRet(false);

  std::string sImage = pImage;
  if (sImage.rfind(".jpg") != std::string::npos)
  {
    sMimeType = EPlotPageDescriptor::MIME_JPEG;
    bRet = true;
  }
  else if (sImage.rfind(".png") != std::string::npos)
  {
    sMimeType = EPlotPageDescriptor::MIME_PNG;
    bRet = true;
  }
  else
  {
    bRet = false;     // unsupported
  }

  return bRet;
}

static bool isAnythingForExport(OdDbDatabase* pDb, OdDbObjectId layoutId)
{
  bool bRet(true);

  OdDbLayoutPtr pLayout = layoutId.safeOpenObject();

  if (pLayout->getBlockTableRecordId() != pDb->getModelSpaceId()
      && 
      //OdDbObjectId overallVpId = pLayout->overallVportId();
      (pLayout->overallVportId()).isErased()) // no viewport in paper space
  {
    bRet = false;
  }
  return bRet;
}

//***************************************************************************
// 
//***************************************************************************
CDwfExportImpl::CDwfExportImpl(const DwExportParams& Params)
    : m_Params(Params)
    , m_bWtDefaulPalette(false)
    , m_bColorMapOptimize(false)        // MKU 1/21/2004
    , m_nColors(0)
    , m_pColors(NULL)
    , m_pProgressMeter(NULL)
    , m_bInkedArea(false)
    , m_Layout(0)
    , m_bLandscape(true)
{
  g_temp_files.resize(0);
/*
  g_arrayW2Dfiles.resize(0);   
  g_arrayParamIndexes.resize(0);
  g_arrayLayoutNames.resize(0);
  g_arrayLayoutIds.resize(0);
  g_arrayPlotInfo.resize(0);
*/
  g_arrayLayoutInfo.resize(0);
}

CDwfExportImpl::~CDwfExportImpl()
{
  if (m_pColors)
  {
    delete [] m_pColors;
  }
  g_temp_files.resize(0);
/*
  g_arrayW2Dfiles.resize(0);   
  g_arrayParamIndexes.resize(0);
  g_arrayLayoutNames.resize(0);
  g_arrayLayoutIds.resize(0);
  g_arrayPlotInfo.resize(0);
*/
  g_arrayLayoutInfo.resize(0);
}

//
// Progress Meter processing
//
void CDwfExportImpl::pmInit()
{
  if (!m_pProgressMeter)
  {
      m_pProgressMeter = m_Params.pDb->appServices()->newProgressMeter();
  }
}

void CDwfExportImpl::pmRelease()
{
  if (m_pProgressMeter)
  {
    m_Params.pDb->appServices()->releaseProgressMeter(m_pProgressMeter);
  }
}

void CDwfExportImpl::pmStart(const char* displayString)
{
  if (m_pProgressMeter)
  {
    m_pProgressMeter->start(displayString);
  }
}

void CDwfExportImpl::pmStop()
{
  if (m_pProgressMeter)
  {
    m_pProgressMeter->stop();
  }
}

void CDwfExportImpl::pmMeterProgress()
{
  if(m_pProgressMeter)
  {
    m_pProgressMeter->meterProgress();
  }
}

void CDwfExportImpl::pmSetLimit(int max)
{
  if (m_pProgressMeter)
  {
    m_pProgressMeter->setLimit(max);
  }
}


//
// isDwfVersion6()
//
// Checks if the DWF version is 06.00 and greater.
//  Returns true in this case.
//  Otherwise - false.
//
bool CDwfExportImpl::isDwfVersion6()
{
  bool bRet(false);

  switch (m_Params.Version)
  {
  case nDwf_v60:      // the DWF 06.00 (and greater) 
    bRet = true;
    break;

  case nDwf_v55:      // previous DWF versions
  case nDwf_v42:
    bRet = false;
    break;

  default:
    ODA_FAIL();
    throw OdError(eIncorrectInitFileVersion);
  }

  return bRet;
}


bool CDwfExportImpl::run()
{
  try
  {
    g_sDwfFileName = m_Params.sDwfFileName.c_str();

    // To check the extension
    /*
    int iPos = g_sDwfFileName.reverseFind('.');
	  if (iPos != (-1))
    {
      OdString sExtension = g_sDwfFileName.mid(iPos + 1);
      if (sExtension.iCompare("dwf"))
      {
        ODA_FAIL();
        throw OdError(eUnknownFileType);
      }
    }
    else
    {
      ODA_FAIL();
      throw OdError(eUnknownFileType);
    }
    */

    // To set the palette to be used. 
    //  If parmeter is NULL - one of two default palettes will be used depending on background color.
    m_pPalette = (m_Params.pPalette != NULL) ? m_Params.pPalette
                                             : odcmAcadPalette(m_Params.background);

    if (isDwfVersion6())
    {
      // the DWF 06.00 (and greater) 
      //

      //initContext();    // To initialize environment

      // To create DWF device
// MKU 04.12.2003      m_pDeviceDwf = OdDwfDevice::createObject();
        // Create the custom rendering device
      OdGsDevicePtr pDevice = OdDwfDevice::createObject();
      m_pDeviceDwf = (OdDwfDevice*)pDevice.get();

      //
      // To get Layouts
      //

      //OdArray<OdString> arrayLayoutNames;
      //OdArray<OdDbObjectId> arrayLayoutIds;
/*
      g_arrayParamIndexes.resize(0);
      g_arrayLayoutNames.resize(0);
      g_arrayLayoutIds.resize(0);
*/
      g_arrayLayoutInfo.resize(0);

      if ( m_Params.arPageData.size() > 0 )
      {
        for (UINT k = 0; k < m_Params.arPageData.size(); k++)
        {
          OdString sLayout = m_Params.arPageData[k].sLayout;

          if (!sLayout.isEmpty() && m_Params.pDb->findLayoutNamed(sLayout))
          {
            OdDbObjectId idLayout = m_Params.pDb->findLayoutNamed(sLayout);
            
            //if (isAnythingForExport(m_Params.pDb, idLayout))
            //{
/*
              int nItem = g_arrayLayoutNames.size();
              g_arrayLayoutNames.resize(nItem + 1);
              g_arrayLayoutIds.resize(nItem + 1);
              g_arrayParamIndexes.resize(nItem + 1);

              g_arrayLayoutNames[nItem] = sLayout;
              g_arrayLayoutIds[nItem] = idLayout; //m_Params.pDb->findLayoutNamed(sLayout);
              g_arrayParamIndexes[nItem] = k;
*/
              int nItem = g_arrayLayoutInfo.size();
              g_arrayLayoutInfo.resize(nItem + 1);

              g_arrayLayoutInfo[nItem].m_sLayoutNames = sLayout;
              g_arrayLayoutInfo[nItem].m_idLayout = idLayout;   //m_Params.pDb->findLayoutNamed(sLayout);
              g_arrayLayoutInfo[nItem].m_iParamIndex = k;
            //}
          }
          else
          {
            ODA_ASSERT(false);
          }
        }
      }
      else
      {
        /*
        // set the Active Layout to export
        g_arrayLayoutNames.resize(1);
        g_arrayLayoutIds.resize(1);

        OdDbObjectId idLayout = OdDbBlockTableRecordPtr(m_Params.pDb->getActiveLayoutBTRId().safeOpenObject())->getLayoutId();

        g_arrayLayoutIds[0] = idLayout;
        g_arrayLayoutNames[0] = m_Params.pDb->findActiveLayout(true);
        */

        // To export all initialized layouts
        //
        // Model Space is the 1st
/*
        g_arrayLayoutNames.resize(1);
        g_arrayLayoutIds.resize(1);
*/
        g_arrayLayoutInfo.resize(1);

        OdDbObjectId idMsLayout = OdDbBlockTableRecordPtr(m_Params.pDb->getModelSpaceId().safeOpenObject())
                                    ->getLayoutId();
//        g_arrayLayoutIds[0] = idMsLayout;
        g_arrayLayoutInfo[0].m_idLayout = idMsLayout;
        OdDbLayoutPtr pMsLayout = idMsLayout.safeOpenObject();
        //g_arrayLayoutNames[0] = (idMsLayout.safeOpenObject())->getLayoutName();
//        g_arrayLayoutNames[0] = pMsLayout->getLayoutName();
        g_arrayLayoutInfo[0].m_sLayoutNames = pMsLayout->getLayoutName();

        // To get the paper layouts in this database.

        OdDbDictionaryPtr         pLayoutDict = m_Params.pDb->getLayoutDictionaryId().safeOpenObject();
        OdDbDictionaryIteratorPtr pLayoutIter = pLayoutDict->newIterator();

        for (; !pLayoutIter->done(); pLayoutIter->next())
        {
          OdDbObjectId idCur = pLayoutIter->objectId();
          if (idCur == idMsLayout)
          {
            continue;
          }

          if (isAnythingForExport(m_Params.pDb, idCur))
          {
//            int nItem = g_arrayLayoutNames.size();
            int nItem = g_arrayLayoutInfo.size();
/*
            g_arrayLayoutNames.resize(nItem + 1);
            g_arrayLayoutIds.resize(nItem + 1);
*/
            g_arrayLayoutInfo.resize(nItem + 1);
/*
            g_arrayLayoutIds[nItem] = idCur;
            g_arrayLayoutNames[nItem] = pLayoutIter->name();
*/
            g_arrayLayoutInfo[nItem].m_idLayout = idCur;
            g_arrayLayoutInfo[nItem].m_sLayoutNames = pLayoutIter->name();

          }
        }
      }

      if (!g_arrayLayoutInfo.size())
      {
        std::cout << "\nexportDwf: at least one layout must be initialized, exiting\n";
        return false;
      }

      ODA_ASSERT( g_arrayLayoutInfo.size() );    // At least one layout must exist always

      //g_arrayW2Dfiles.resize( g_arrayLayoutNames.size() );
      //g_arrayPlotInfo.resize( g_arrayLayoutNames.size() );

      //
      // Loop for all layouts
      //
      pmInit();
      pmStart("Publishing Dwf6 file...");
//      pmSetLimit(g_arrayLayoutNames.size());
      pmSetLimit(g_arrayLayoutInfo.size());

//      for (UINT i = 0; i < g_arrayLayoutNames.size(); )
      for (UINT i = 0; i < g_arrayLayoutInfo.size(); )
      {
        m_Layout = i;
//        g_arrayW2Dfiles.resize( i + 1 );
//        g_arrayPlotInfo.resize( i + 1 );

//        if (g_arrayLayoutNames[i].isEmpty()
        if (g_arrayLayoutInfo[i].m_sLayoutNames.isEmpty()
            || !initPlotting(i))
        {
          // to delete references into g_arrayXxx
//          g_arrayLayoutNames.removeAt(i);
//          g_arrayLayoutIds.removeAt(i);
          g_arrayLayoutInfo.removeAt(i);
/*
          if (i < m_Params.arPageData.size())
          {
            g_arrayParamIndexes.removeAt(i);
          }
*/
          pmMeterProgress();
          continue;
        }

        std::string sTmpFileName = g_sDwfFileName.c_str();
        int iPos = sTmpFileName.rfind('.');
	      if (iPos != (-1))
        {
          sTmpFileName = sTmpFileName.substr(0, iPos);
        }

        //sTmpFileName = sTmpFileName.substr(0, sTmpFileName.rfind(".dwf"));
        sTmpFileName = sTmpFileName.substr(0, sTmpFileName.rfind(".dwf"));
        sTmpFileName += '-';
//        sTmpFileName += g_arrayLayoutNames[i].c_str();
        sTmpFileName += g_arrayLayoutInfo[i].m_sLayoutNames.c_str();
        sTmpFileName += ".w2d";

        OdString sGraphicFileName(sTmpFileName.c_str());
        sGraphicFileName = GetTempFile( sGraphicFileName );

        //g_temp_files.push_back(sGraphicFileName);             // keep in mind for cleaning at the end

//        ODA_ASSERT( i < g_arrayW2Dfiles.size() );
//        g_arrayW2Dfiles[i] = sGraphicFileName;
        g_arrayLayoutInfo[i].m_sW2Dfiles = sGraphicFileName;

        openWtFile(sGraphicFileName);

        // To initialize environment
        initColors();

        m_pDeviceDwf->eraseAllViews();
        m_pDeviceDwf->setContext(this);       // set special context for dwf device (including WT file)

        // To create & activate own Dwf View        
        OdGiContextForDbDatabasePtr pCtx = OdGiContextForDbDatabase::createObject();
        pCtx->setDatabase(m_Params.pDb);
        pCtx->setPlotGeneration(true);

//        m_pDevice = OdDbGsManager::setupLayoutViews( g_arrayLayoutIds[i],
        m_pDevice = OdDbGsManager::setupLayoutViews( g_arrayLayoutInfo[i].m_idLayout,
                                                     m_pDeviceDwf, 
                                                     pCtx );
        m_pDeviceDwf->setBackgroundColor(m_Params.background);
        m_pDeviceDwf->setLogicalPalette(m_pPalette, 256);

        OdGsDCRect outputRect( OdGsDCPoint( m_rectViewport.left,  m_rectViewport.bottom),
                               OdGsDCPoint( m_rectViewport.right, m_rectViewport.top) );

        m_pDeviceDwf->onSize(outputRect);

        draw();

        // Close WT file
        m_wtFile.close();
        m_pDevice->eraseAllViews();

        pmMeterProgress();
        i++;
      }

      pmStop();
/*
      ODA_ASSERT( g_arrayW2Dfiles.size() == g_arrayPlotInfo.size() );
      ODA_ASSERT( g_arrayPlotInfo.size() == g_arrayParamIndexes.size() );
      ODA_ASSERT( g_arrayParamIndexes.size() == g_arrayLayoutNames.size() );
      ODA_ASSERT( g_arrayLayoutNames.size() == g_arrayLayoutIds.size() );
*/

      writingDwf6Files();

      CleanUp();      // To clean up our temporary

      pmRelease();
    }
    else
    {
      // previous DWF versions
      begin();
      draw();
      end();
    }
  }
  catch (DwfResult::Enum result)
  {
    //std::cout << "\nDwf6 exception thrown during exportDwf: " << (HRESULT)result << std::endl;
    OdString msg("Dwf6 exception is thrown during exportDwf: ");
    char buffer[64];
    ltoa((HRESULT)result, buffer, 10);
    msg += buffer;

    m_Params.pDb->appServices()->warning(msg);

    pmStop();
    pmRelease();
    CleanUp();    // To clean up our temporary

    return false;
  }
  catch (WT_Result::Enum result)
  {
    //std::cout << "\nWhipTk exception thrown during exportDwf: " << result << std::endl;
    OdString msg("WhipTk exception is thrown during exportDwf: ");
    char buffer[64];
    ltoa((HRESULT)result, buffer, 10);
    msg += buffer;
    m_Params.pDb->appServices()->warning(msg);

    pmStop();
    pmRelease();
    CleanUp();    // To clean up our temporary

    return false;
  }
	catch (OdError& err)
	{
    //OdString msg = m_Params.pDb->appServices()->getErrorDescription(err.code());
    //std::cout << "\nOdError thrown during exportDwf: " << msg.c_str() << std::endl;
    OdString msg("OdError is thrown during exportDwf: ");
    msg += m_Params.pDb->appServices()->getErrorDescription(err.code());
    m_Params.pDb->appServices()->warning(msg);

    pmStop();
    pmRelease();
    CleanUp();    // To clean up our temporary

    return false;

  }
  catch (...)
  {
    //std::cout << "\nException thrown during exportDwf, exiting\n";
    OdString msg("Unexpected error is happened during exportDwf.");
    m_Params.pDb->appServices()->warning(msg);

    pmStop();
    pmRelease();
    CleanUp();    // To clean up our temporary

    return false;
  }

  return true;
}



//***************************************************************************
//
// begin()
//
//***************************************************************************
void CDwfExportImpl::begin()
{
  // for previous DWF versions only (earlier then v06.00)

  if (isDwfVersion6())    
  {
    ODA_ASSERT(false);
  }

  ODA_ASSERT(m_Params.pDb);
  if (!m_Params.pDb)
  {
    ODA_FAIL();
    throw OdError(eNoDatabase);
  }

  initContext();    // To initialize environment

  openWtFile(g_sDwfFileName);

  createDevice();   // create OdDwfDevice & OdDwfView

  // The definition of the default color map is in
  // "palette.h". If you don't like the default palette,
  // you can set your own by setting the color map attribute
  // on the file's desired_rendition.

}


//
// initContext()
//
//  To initialize environment
//
void CDwfExportImpl::initContext()
{
  long xSize = (m_Params.xSize > lMaxDwfResolution) ? lMaxDwfResolution : m_Params.xSize;
  long ySize = (m_Params.ySize > lMaxDwfResolution) ? lMaxDwfResolution : m_Params.ySize;

  // DWF coordinates range is 0 - (2**31 - 1)
  // Xmin must not be 0 because if drawing has some extra space at the left
  // (panned to right for example)  Autodesk's viewer does not treat this situation
  // correctly - left margin of visible area has negative X coordinate.
  // There is no such problem with Y coord.
  // AutoCAD's DWFOUT does the same thing
  m_rectViewport.left = 0x40000000 - xSize / 2;           // Xmin
  m_rectViewport.right = m_rectViewport.left + xSize - 1; // Xmax
  //m_rectViewport.top = 0;                                 // Ymin
  //m_rectViewport.bottom = m_rectViewport.top + ySize - 1;  // Ymax
  m_rectViewport.bottom = 0;                                 // Ymin
  m_rectViewport.top = m_rectViewport.bottom + ySize - 1;  // Ymax

  ODA_ASSERT(m_pPalette);
  if (!m_pPalette)    
  {
    ODA_FAIL();
    throw OdError(eNullObjectPointer);
  }

  if (!m_bColorMapOptimize)
  {
    return;
  }
  // Compare user palette with WHIPTK default color map (WD_New_Default_Palette)
  //
  int numberNotOfMatchings = 0;
  int sizePalette = 256;

  for (int i = 0; i < sizePalette; i++)
  {
    ODCOLORREF colorFromUser = m_pPalette[i];

    int j;
    for (j = 0; j < sizePalette; j++)
    {
      WT_RGBA32 wt_color = WD_New_Default_Palette[j];
      ODCOLORREF colorFromDefault(ODRGB(wt_color.m_rgb.r, wt_color.m_rgb.g, wt_color.m_rgb.b));

      if (colorFromUser == colorFromDefault)
      {
        break;
      }
    }

    if (j >= sizePalette)
    {
      numberNotOfMatchings++;
    }
  }

  // To fill from the selected palette (Default or Set by user) by RGB and corresponding index
  if (numberNotOfMatchings)
  {
    m_bWtDefaulPalette = false;
    m_pColors = new WT_RGBA32[256];
    m_nColors = 256;
  
    for (int k = 0; k < sizePalette; k++)
    {
      ODCOLORREF color = m_pPalette[k];
      m_colorIndexMap[color] = k;
      m_pColors[k] = WT_RGBA32(ODGETRED(color), ODGETGREEN(color), ODGETBLUE(color));
    } 
  }
  else
  {
    for (int m = 0; m < sizePalette; m++)
    {
      WT_RGBA32 wt_color = WD_New_Default_Palette[m];
      ODCOLORREF rgb(ODRGB(wt_color.m_rgb.r, wt_color.m_rgb.g, wt_color.m_rgb.b));
      m_colorIndexMap[rgb] = m;
    }

    m_bWtDefaulPalette = true;
  }
}


bool CDwfExportImpl::initPlotting(int tag)
{
  DwfLayoutPlotInfo plotInfo;

  plotInfo.m_Measurement = m_Params.pDb->getMEASUREMENT();
  //bool bMetric = ( m_Params.pDb->getMEASUREMENT() == OdDb::kMetric ) ? true : false;

  //if (_paperInfo._units == OdDbPlotSettings::kMillimeters && _db->getMEASUREMENT() == OdDb::kEnglish)
  //else if (_paperInfo._units == OdDbPlotSettings::kInches && _db->getMEASUREMENT() == OdDb::kMetric)

//  OdDbObjectId layoutId = g_arrayLayoutIds[tag];
  OdDbObjectId layoutId = g_arrayLayoutInfo[tag].m_idLayout;
  OdDbLayoutPtr pLayout = layoutId.safeOpenObject();

  pLayout->getPlotPaperSize( plotInfo.m_dPaperWidth, plotInfo.m_dPaperHeight );

  if (OdZero(plotInfo.m_dPaperWidth) || OdZero(plotInfo.m_dPaperHeight))
  {
    // This is an uninitialized layout. To set default Plot Info
    //
    plotInfo.m_dPaperWidth =  215.89999389648;
    plotInfo.m_dPaperHeight = 279.39999389648;

	  double dTopMargin    =  17.793762207031;
	  double dRightMargin  =   5.7937469482422;
	  double dBottomMargin =  17.793750762939;
	  double dLeftMargin   =   5.7937498092651;

    plotInfo.m_dPrintableXmin = dLeftMargin;
    plotInfo.m_dPrintableYmin = dBottomMargin;
    plotInfo.m_dPrintableXmax = plotInfo.m_dPaperWidth - dRightMargin;
    plotInfo.m_dPrintableYmax = plotInfo.m_dPaperHeight - dTopMargin;

    plotInfo.m_PlotPaperUnit = OdDbPlotSettings::kMillimeters;
  }
  else
  {
    // MKU 06.07.2004   #1763
    // take into account Plot Rotation
    OdDbPlotSettings::PlotRotation rotation = pLayout->plotRotation();
    double dTmp;
    switch (rotation)
    {
      case OdDbPlotSettings::k90degrees:
      case OdDbPlotSettings::k270degrees:
        dTmp = plotInfo.m_dPaperWidth;
        plotInfo.m_dPaperWidth = plotInfo.m_dPaperHeight;
        plotInfo.m_dPaperHeight = dTmp;

        pLayout->getPlotPaperMargins( plotInfo.m_dPrintableYmin, plotInfo.m_dPrintableXmin,
                                      plotInfo.m_dPrintableYmax, plotInfo.m_dPrintableXmax );

      default:
        pLayout->getPlotPaperMargins( plotInfo.m_dPrintableXmin, plotInfo.m_dPrintableYmin, 
                                      plotInfo.m_dPrintableXmax, plotInfo.m_dPrintableYmax );
        break;
    }

    plotInfo.m_PlotPaperUnit = pLayout->plotPaperUnits();
    /*
    if (units == OdDbPlotSettings::kInches)
    {
      plotInfo.m_dScaleUnit = 25.4;
    }
    else if (units == OdDbPlotSettings::kMillimeters)
    {
      plotInfo.m_dScaleUnit = 1.0;
    }
    else      // OdDbPlotSettings::kPixels)
    {
        ODA_FAIL();
      //throw OdError(eNullObjectPointer);
    }
    */
  }

  //double drawingWidth = plotInfo.m_dPrintableXmax - plotInfo.m_dPrintableXmin;
  //double drawingHeight = plotInfo.m_dPrintableYmax - plotInfo.m_dPrintableYmin;
/*
  long xSize;
  long ySize;

  if (pLayout->getBlockTableRecordId() == m_Params.pDb->getModelSpaceId()
      || plotInfo.m_dPaperWidth > plotInfo.m_dPaperHeight)
  {
    // Landscape
    xSize = (m_Params.xSize > lMaxDwfResolution) ? lMaxDwfResolution : m_Params.xSize;
    ySize = (m_Params.ySize > lMaxDwfResolution) ? lMaxDwfResolution : m_Params.ySize;
  }
  else
  {
    // Portrait
    ySize = (m_Params.xSize > lMaxDwfResolution) ? lMaxDwfResolution : m_Params.xSize;
    xSize = (m_Params.ySize > lMaxDwfResolution) ? lMaxDwfResolution : m_Params.ySize;
  }
*/
  long xSize = (m_Params.xSize > lMaxDwfResolution) ? lMaxDwfResolution : m_Params.xSize;
  long ySize = (m_Params.ySize > lMaxDwfResolution) ? lMaxDwfResolution : m_Params.ySize;

  // DWF coordinates range is 0 - (2**31 - 1)
  // Xmin must not be 0 because if drawing has some extra space at the left
  // (panned to right for example)  Autodesk's viewer does not treat this situation
  // correctly - left margin of visible area has negative X coordinate.
  // There is no such problem with Y coord.
  // AutoCAD's DWFOUT does the same thing
  m_rectViewport.left = 0x40000000 - xSize / 2;           // Xmin
  m_rectViewport.right = m_rectViewport.left + xSize - 1; // Xmax
  m_rectViewport.bottom = 0;                                 // Ymin
  m_rectViewport.top = m_rectViewport.bottom + ySize - 1;  // Ymax

//  g_arrayPlotInfo[tag] = plotInfo;
  g_arrayLayoutInfo[tag].m_PlotInfo = plotInfo;

  return true;
}


void CDwfExportImpl::initColors()
{
  if (m_pColors)
  {
    delete [] m_pColors;
  }
/*
  if (!m_bColorMapOptimize)
  {
    return;
  }
*/
  ODA_ASSERT(m_pPalette);
  if (!m_pPalette)    
  {
    ODA_FAIL();
    throw OdError(eNullObjectPointer);
  }

  // Compare user palette with WHIPTK default color map (WD_New_Default_Palette)
  //
  int numberNotOfMatchings = 0;
  int sizePalette = 256;

  for (int i = 0; i < sizePalette; i++)
  {

    ODCOLORREF colorFromUser = m_pPalette[i];

    int j;
    for (j = 0; j < sizePalette; j++)
    {
      WT_RGBA32 wt_color = WD_New_Default_Palette[j];
      ODCOLORREF colorFromDefault(ODRGB(wt_color.m_rgb.r, wt_color.m_rgb.g, wt_color.m_rgb.b));

      if (colorFromUser == colorFromDefault)
      {
        break;
      }
    }

    if (j >= sizePalette)
    {
      numberNotOfMatchings++;
    }
  }

  // To fill from the selected palette (Default or Set by user) by RGB and corresponding index
  if (numberNotOfMatchings)
  {
    m_bWtDefaulPalette = false;
    m_pColors = new WT_RGBA32[256];
    m_nColors = 256;

    for (int k = 0; k < sizePalette; k++)
    {
      ODCOLORREF color = m_pPalette[k];
      m_colorIndexMap[color] = k;
      m_pColors[k] = WT_RGBA32(ODGETRED(color), ODGETGREEN(color), ODGETBLUE(color));
    }
  }
  else
  {
    for (int m = 0; m < sizePalette; m++)
    {
      WT_RGBA32 wt_color = WD_New_Default_Palette[m];
      ODCOLORREF rgb(ODRGB(wt_color.m_rgb.r, wt_color.m_rgb.g, wt_color.m_rgb.b));
      m_colorIndexMap[rgb] = m;
    }

    m_bWtDefaulPalette = true;
  }
}



//
// openWtFile()
//
void CDwfExportImpl::openWtFile(const OdString& sFileName)
{
  m_wtFile.set_filename( sFileName.c_str() );

  m_wtFile.set_file_mode(WT_File::File_Write);
  m_wtFile.heuristics().set_target_version(m_Params.Version);
  // During open file header is written. Compression can be turned on
  // only after open!
  m_wtFile.open();

  switch(m_Params.Format)
  {
  case DW_UNCOMPRESSED_BINARY:
    m_wtFile.heuristics().set_allow_binary_data(WD_True);
    m_wtFile.set_data_decompression(WD_False);
    break;

  case DW_COMPRESSED_BINARY:
    m_wtFile.heuristics().set_allow_binary_data(WD_True);
    m_wtFile.heuristics().set_allow_data_compression(WD_True);
    break;

  case DW_ASCII:
    m_wtFile.heuristics().set_allow_binary_data(WD_False);
    m_wtFile.set_data_decompression(WD_False);
    break;
  }
}



//
// createDevice()
//
//  Create & set own Dwf Device Create; create & activate own Dwf View.
//
void CDwfExportImpl::createDevice()
{
  // for previous DWF versions only (earlier then v06.00)

  if (isDwfVersion6())
  {
    ODA_ASSERT(false);
  }

  // MKU 04.12.2003m_pDeviceDwf = OdDwfDevice::createObject();
  OdGsDevicePtr pDevice = OdDwfDevice::createObject();
   m_pDeviceDwf = (OdDwfDevice*)pDevice.get();

  m_pDeviceDwf->setContext(this);  //  set special context for dwf device
  // Note: to call Own Device methods before ::odgsSetupActiveLayoutViews() not after!
  //       Only Base Interface Methords could be called after ::odgsSetupActiveLayoutViews().

  //  SK  05.02.03
  OdGiContextForDbDatabasePtr pCtx = OdGiContextForDbDatabase::createObject();
  pCtx->setDatabase(m_Params.pDb);
  pCtx->setPlotGeneration(true);

  // if the layaout was set by user to setup this one
  //  otherwise to setup the active layout
  OdString sLayout;
  if (m_Params.arPageData.size())
  {
    sLayout = m_Params.arPageData[0].sLayout;
  }
  m_pDevice = (!sLayout.isEmpty() && m_Params.pDb->findLayoutNamed(sLayout))
      ? OdDbGsManager::setupLayoutViews( m_Params.pDb->findLayoutNamed(sLayout), m_pDeviceDwf, pCtx )
      : OdDbGsManager::setupActiveLayoutViews( m_pDeviceDwf, pCtx );

  //m_Params.pDb->setupActivePalette(m_pDevice, (m_Params.pBackground) ? *m_Params.pBackground 
  //                                                                   : m_Params.pDb->getActiveBackground());
  m_pDeviceDwf->setBackgroundColor(m_Params.background);
  m_pDeviceDwf->setLogicalPalette(m_pPalette, 256);

  //  OdDbHostAppServices* hostServe =  m_Params.pDb->appServices();
  //  const ODCOLORREF* palette = hostServe->getACIPalette();  
  //  m_pDevice->setLogicalPalette(palette, 256);

  OdGsDCRect outputRect(
      OdGsDCPoint(m_rectViewport.left, m_rectViewport.bottom),
      OdGsDCPoint(m_rectViewport.right, m_rectViewport.top)
    );

  m_pDeviceDwf->onSize(outputRect);
}


//***************************************************************************
//
void CDwfExportImpl::end()
{
  // for previous DWF versions only (earlier then v06.00)

  if (isDwfVersion6())
  {
    ODA_ASSERT(false);
  }

  m_wtFile.close();
}


//***************************************************************************
//
// draw()
//
//***************************************************************************
void CDwfExportImpl::draw()
{
//  m_pDeviceDwf->setRun(OdDwfDevice::Nonprocess_run);
  nonprocessRun();

  putInkedArea();

  putDrawingInfo();           // Puts the Drawing Info into the file 

  putDrawingUnits();          // Defines Drawing Units 
  putEmbed();                 // Embed Source File 
  putViewInfo();

  putPlotInfo();  // todo new // Define Plot Info

  putColorMap();              // Puts the list of colors (a map)
  putBackground();            // Defines Drawing Background

  //
  // Draw geometry
  //
//  m_pDeviceDwf->setRun(OdDwfDevice::Main_run);
  m_pDeviceDwf->update(0, OdDwfDevice::Main_run);
}


///////////////////////////////////////////////////////////////////////////////
// Auxiliary methods
///////////////////////////////////////////////////////////////////////////////

//***************************************************************************
//
// nonprocessRun()
//
//  To optimize a presetting by nonprocess run (draw)
//***************************************************************************
void CDwfExportImpl::nonprocessRun()
{
  if (m_bWtDefaulPalette)   return;         // MKU 1/21/2004

  if (m_bColorMapOptimize)
  {
    m_nColors = 1;
    m_pColors[0] = WT_RGBA32(ODGETRED(m_Params.background), 
                             ODGETGREEN(m_Params.background), 
                             ODGETBLUE(m_Params.background));

    m_pDeviceDwf->update(0, OdDwfDevice::Nonprocess_run);

    m_colorIndexMap.clear();
    for (int i = 0; i < m_nColors; i++)
    {
      WT_RGBA32 wt_color = m_pColors[i];
      ODCOLORREF rgb(ODRGB(wt_color.m_rgb.r, wt_color.m_rgb.g, wt_color.m_rgb.b));
      m_colorIndexMap[rgb] = i;
    }
  }
  else
  {
    // To set off the color map optimization and to export whole color map always
    m_colorIndexMap.clear();

    if (m_pColors)
    {
      delete [] m_pColors;
    }

    ODA_ASSERT(m_pPalette);
    if (!m_pPalette)    
    {
      ODA_FAIL();
      throw OdError(eNullObjectPointer);
    }

    m_nColors = 256;
    m_pColors = new WT_RGBA32[256];

    for (int i = 0; i < m_nColors; i++)
    {
      ODCOLORREF colorFromUser = m_pPalette[i];

      WT_RGBA32 wt_color(ODGETRED(colorFromUser), ODGETGREEN(colorFromUser), ODGETBLUE(colorFromUser));

      m_pColors[i] = wt_color;

      //WT_RGBA32 wt_color = m_pColors[i];
      //ODCOLORREF rgb(ODRGB(wt_color.m_rgb.r, wt_color.m_rgb.g, wt_color.m_rgb.b));
      m_colorIndexMap[colorFromUser] = i;
    }
  }
}

void CDwfExportImpl::colorPresetting(ODCOLORREF color)
{
  bool bColorWasFound(false);
  WT_RGBA32 wt_color(ODGETRED(color), ODGETGREEN(color), ODGETBLUE(color));
  for (int i = 0; i < m_nColors; i++)
  {
    if (m_pColors[i] == wt_color)
    {
      bColorWasFound = true;
      break;
    }
  }
  if (!bColorWasFound)
  {
    m_pColors[m_nColors++] = wt_color;
  }
}


//***************************************************************************
//
// getColorIndex()
//
//  To get color indext at active palette
//
//***************************************************************************
bool CDwfExportImpl::getWtColorIndex(const ODCOLORREF rgb, int& color_index) const
{
  //int color_index;
  bool bRet(true);
  
  colorIndexMap::const_iterator it;
  it = m_colorIndexMap.find(rgb);
  if (it != m_colorIndexMap.end())
  {
    color_index = (*it).second;
  }
  else
  {
    //ODA_ASSERT(false);
    color_index = getWtDefaultColorIndex();
    bRet = false;
  }
  
  return bRet;
}

/////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------

//
// putInkedArea()
//
//  The Inked Area opcode specifies a rectangle, in logical coordinates, 
//  in which all of the .dwf file’s geometric primitives fit tightly inside.
//
void CDwfExportImpl::putInkedArea()
{
  if (!m_bInkedArea)
  {
    return;
  }

  // To get extents of active view & transform them
  OdGsView* view = m_pDeviceDwf->viewAt(0);
  OdDbExtents extents;
  extents.addBlockExt(OdDbBlockTableRecordPtr(m_Params.pDb->getActiveLayoutBTRId().safeOpenObject()));
  if (!extents.isValidExtents())
  {
    return;
  }

  extents.transformBy(view->objectToDeviceMatrix());

  OdGePoint3d ptMin, ptMax;
  ptMin = extents.minPoint(); 
  ptMax = extents.maxPoint(); 

  WT_Logical_Point bounds[4];
  boxAnticlockwise(bounds, ptMin, ptMax);   // To set a direction of box detour clockwise
  
  WT_Inked_Area inkedArea;
  inkedArea.set_inked_area(bounds);
	inkedArea.serialize(m_wtFile);

  // to set Landscape / Portrait property for Model space
  OdDbObjectId layoutId = g_arrayLayoutInfo[m_Layout].m_idLayout;
  OdDbLayoutPtr pLayout = layoutId.safeOpenObject();
  if ( pLayout->getBlockTableRecordId() == m_Params.pDb->getModelSpaceId() )
  {
    double dWidth = ptMax.x - ptMin.x;
    double dHeight = ptMax.y - ptMin.y;

    m_bLandscape = (dWidth > dHeight) ? true : false;
  }
}

//
// putDrawingInfo()
//
//  Put into the file Drawing Info
//
void CDwfExportImpl::putDrawingInfo()
{
  // Set Drawing Info
  //

  bool bPageDataExists = (m_Params.arPageData.size() == 0) ? false : true;
  DwfPageData pageData;
  if (bPageDataExists)
  {
    pageData = m_Params.arPageData[ 0 ];
  }

  if (bPageDataExists && !pageData.sPageDescription.isEmpty())
  {
    m_wtFile.desired_rendition().drawing_info().description().set(pageData.sPageDescription.c_str());
  }

  if (bPageDataExists && !pageData.sPageAuthor.isEmpty())
  {
    m_wtFile.desired_rendition().drawing_info().author().set(pageData.sPageAuthor.c_str());
  }

  if (bPageDataExists && !pageData.sPageComments.isEmpty())
  {
    m_wtFile.desired_rendition().drawing_info().comments().set(pageData.sPageComments.c_str());
  }

  if (bPageDataExists && !pageData.sPageCopyright.isEmpty())
  {
    m_wtFile.desired_rendition().drawing_info().copyright().set(pageData.sPageCopyright.c_str());
  }
/*
  if (!pageData.sCreator.isEmpty())
  {
    m_wtFile.desired_rendition().drawing_info().creator().set(m_Params.sCreator.c_str());
  }
  else
  {
*/
    m_wtFile.desired_rendition().drawing_info().creator().set(
        "Open Design DWF Export"
    #ifdef WIN32
        " - WIN32"
    #else if defined(linux)
        " - Linux"
    #endif
    );
//  }

  if (bPageDataExists && !pageData.sPageKeywords.isEmpty())
  {
    m_wtFile.desired_rendition().drawing_info().keywords().set(pageData.sPageKeywords.c_str());
  }

  if (bPageDataExists && !pageData.sPageTitle.isEmpty())
  {
    m_wtFile.desired_rendition().drawing_info().title().set(pageData.sPageTitle.c_str());
  }

  if (bPageDataExists && !pageData.sPageSubject.isEmpty())
  {
    m_wtFile.desired_rendition().drawing_info().subject().set(pageData.sPageSubject.c_str());
  }
/*
  if (!m_Params.sSourceFileName.isEmpty())
  {
    m_wtFile.desired_rendition().drawing_info().source_filename().set(m_Params.sSourceFileName.c_str());
  }
*/
  m_wtFile.desired_rendition().drawing_info().source_filename().set(m_Params.pDb->getFilename().c_str());

  // Creation and Modification time
  OdString locTimeStr;
  long ltime;

  OdDbDate locTime(OdTimeStamp::kInitLocalTime);
  locTime.strftime("%c", locTimeStr);
  ltime = locTime.packedValue();

  m_wtFile.desired_rendition().drawing_info().creation_time().set(ltime, locTimeStr);
  m_wtFile.desired_rendition().drawing_info().modification_time().set(ltime, locTimeStr);

  // Drawing Creation time
  locTime = m_Params.pDb->getTDCREATE(); locTime.universalToLocal();
  ltime = locTime.packedValue();
  locTime.strftime("%c", locTimeStr);

  m_wtFile.desired_rendition().drawing_info().source_creation_time().set(ltime, locTimeStr);
  m_wtFile.desired_rendition().drawing_info().source_creation_time().
                                                    set_guid(m_Params.pDb->getFINGERPRINTGUID().c_str());

  // Drawing Modification time
  locTime = m_Params.pDb->getTDUPDATE(); locTime.universalToLocal();
  ltime = locTime.packedValue();
  locTime.strftime("%c", locTimeStr);

  m_wtFile.desired_rendition().drawing_info().source_modification_time().set(ltime, locTimeStr);
  m_wtFile.desired_rendition().drawing_info().source_modification_time().
                                                    set_guid(m_Params.pDb->getVERSIONGUID().c_str());

  // Put Drawing Info
  m_wtFile.desired_rendition().drawing_info().sync(m_wtFile);
}


//
// putDrawingUnits()
//
//  The Define Drawing Units function describes the real-world scale of the DWF’s logical coordinates.
//
void CDwfExportImpl::putDrawingUnits()
{
  double xform[16];

  ((OdDwfView*)m_pDeviceDwf->viewAt(0))->convDeviceMatrixToWt(/*m_pDeviceDwf->viewAt(0), */xform);

  WT_Units units;
  units.set_application_to_dwf_transform( WT_Matrix(xform) );
  units.set_units( getUnitName( m_Params.pDb->getINSUNITS() ) );

  units.serialize(m_wtFile);
}

//
// putEmbed()
//
//  The Embed Source File function allows the source file from which the DWF was created 
//  to be embedded (or referenced) inside the DWF, thus allowing smart drag-and-drop operations.
//
void CDwfExportImpl::putEmbed()
{
  WT_Embed  Embed;
  Embed.set_whole_MIME("image/vnd.dwg", m_wtFile);
  if (m_wtFile.desired_rendition().drawing_info().description().string().length())
  {
    Embed.set_description(m_wtFile.desired_rendition().drawing_info().description().string(), m_wtFile);
  }
  else
  {
    Embed.set_description(" ", m_wtFile);  // AutoDesk's viewer do not like it empty
  }
/*
  if (!m_Params.sSourceFileName.isEmpty())
  {
    Embed.set_filename(m_Params.sSourceFileName.c_str(), m_wtFile);
  }
*/
  Embed.set_filename(m_Params.pDb->getFilename().c_str(), m_wtFile);

  Embed.serialize(m_wtFile);
}

//
// putViewInfo()
//
void CDwfExportImpl::putViewInfo()
{
  // Named View
  if (!m_Params.bSkipNamedViewsInfo)
  { 
    // The Define Initial View function indicates to the DWF reading application 
    //  what portion of the logical coordinate space should initially be viewed
    putView(m_pDeviceDwf->viewAt(0), "INITIAL");   //  to get active view as initial

    putView(m_pDeviceDwf->viewAt(0));
  }
}

//
// putView() 
//
void CDwfExportImpl::putView(OdGsView* gsView, const char * Name)
{
  WT_Logical_Point wtMin;
  WT_Logical_Point wtMax;

  ((OdDwfView*)gsView)->convViewportToWt(/*gsView, */wtMin, wtMax);

  WT_Logical_Box wtBox( wtMin, wtMax );
  if (Name != NULL)
  {
    WT_Named_View View(wtBox, Name);
    View.serialize(m_wtFile);
  } 
  else
  {
    WT_View View(wtBox);
    View.serialize(m_wtFile);
  }
}

//
// putPlotInfo()
//
//  The Define Plot Info function describes the real-world plotting of the DWF’s logical coordinates.
//
void CDwfExportImpl::putPlotInfo()
{
}

//
// putBackground()
//
//  The Define Drawing Background function specifies how the DWF reading application should render 
//  the background behind the geometric primitives to be drawn.
//
void CDwfExportImpl::putBackground()
{
  // Setup color map by the colors used in drawing
//todo  m_wtFile.desired_rendition().color_map().set(m_nColorsUsed, m_pColors, m_wtFile.);

  // Background color
  if (m_Params.Version == nDwf_v60)
  {
    // since version 6.0 Background opcode was deprecated
    return;
  }

  if (m_Params.Version == nDwf_v42)
  {
    if ( ODGETRED(m_Params.background) == 0 
      && ODGETGREEN(m_Params.background) == 0 
      && ODGETBLUE(m_Params.background) == 0
      )
    { 
      // For some reason zero background is not accepted by AutoDesk's viewer
      m_wtFile.desired_rendition().rendering_options().background().set(WT_Color(1, 0, 0));
    } 
    else 
    {
      m_wtFile.desired_rendition().rendering_options().background().set(WT_Color(
              ODGETRED(m_Params.background),
              ODGETGREEN(m_Params.background),
              ODGETBLUE(m_Params.background)));
    }  
    m_wtFile.desired_rendition().rendering_options().sync(m_wtFile);
  }
  else if (m_Params.Version == nDwf_v55)
  {
    /*  D.Novikov 5 Feb 03
    ODCOLORREF colorBackground((m_Params.pBackground) ? *m_Params.pBackground
                                                     : m_Params.pDb->getActiveBackground()
                             );
                             */
    int color_index;
    WT_Background background;
    if (getWtColorIndex(m_Params.background, color_index))
    {
      background.set(WT_Color (color_index, m_wtFile.desired_rendition().color_map()));
    }
    else
    {
      //ODCOLORREF rgb(ODRGB(wt_color.m_rgb.r, wt_color.m_rgb.g, wt_color.m_rgb.b));
      background.set(WT_Color (ODGETRED(m_Params.background), 
                                          ODGETGREEN(m_Params.background), 
                                          ODGETBLUE(m_Params.background)));
    }
    background.serialize(m_wtFile);
  }
/*
  // since version 6.0 Background opcode was deprecated
  else if (m_Params.Version == nDwf_v60)
  {
    // MKU 30.05.2003 v60.0tmp
    int color_index;
    WT_Background background;
    if (getWtColorIndex(m_Params.background, color_index))
    {
      background.set(WT_Color (color_index, m_wtFile.desired_rendition().color_map()));
    }
    else
    {
      //ODCOLORREF rgb(ODRGB(wt_color.m_rgb.r, wt_color.m_rgb.g, wt_color.m_rgb.b));
      background.set(WT_Color (ODGETRED(m_Params.background), 
                                          ODGETGREEN(m_Params.background), 
                                          ODGETBLUE(m_Params.background)));
    }
    background.serialize(m_wtFile);
  }
*/
  else
  {
    ODA_ASSERT(false);
    throw OdError(eIncorrectInitFileVersion);
  }
}


//
// putColorMap()
//
//  A list of colors (a map) to be used when displaying images
//
void CDwfExportImpl::putColorMap()
{
  if (m_bWtDefaulPalette || !m_pColors)   return;

  m_wtFile.desired_rendition().color_map().set(m_nColors, m_pColors, m_wtFile);
}



//***************************************************************************
//
// writingDwf6Files()
//
//  A few steps to creating a basic DWF 6 file: 
//    - Create an DWFEPlot::EPlotSectionManager;
//    - Create a DWFWriter::DwfWriter object;
//    - Create a DWFEPlot::EPlotPageDescriptor which will be used to define a page in the DWF 6 file;
//    - Add resources to the page;
//    - Destroy the page descriptor, dwf writer, and section manager, which causes the file to be written 
//      with the manifest and descriptor files in it;
//    - Optimize the file.
//
//***************************************************************************
void CDwfExportImpl::writingDwf6Files()
{
  if (!g_arrayLayoutInfo.size())  
  {
    // message-warning
    return;
  }

  // Create manager. To add file globals to the manifest as dwf metadata.
  EPlotSectionManager* manager 
    = new EPlotSectionManager( "Open Design Alliance Inc.", // the vendor of the authoring tool
                               "Open Design DWF Export"     // the product creating the design data
#ifdef WIN32
                                            " - WIN32"
#else if defined(linux)
                                            " - Linux"
#endif
                                                      ,
                               "2003",                  // the version of the product creating the dwf
                               "Open Design Alliance Inc.", // the vendor of the dwf writer
                               "1.0"                    // the version of the dwf writer
                              );

  DwfWriter* dwfWriter 
    = new DwfWriter( g_sDwfFileName.c_str(),            // the DWF package file we're creating, 
                     manager, 
                     "",                                // EPLOT_TYPEINFO
                     NULL,                              // ePlotTypeInfoHandler.Object(), 
                     (!m_Params.sPassword.isEmpty()) ? m_Params.sPassword.c_str() : "",
                     DEFAULT_DWF_PACKAGE_REVISION       // the first 12 bytes of the dwf file
                    );                                  //  as a DWF file header, e.g. "(DWF V06.00)"

  // Let the dwfWriter know which document interfaces we intend to implement.
  Interface iFace = DWFEPlot::EPLOT_INTERFACE;
  dwfWriter->AddInterface( iFace.GetObjId(), iFace.GetName(), iFace.GetHref() );

  // Add some more dwf-wide metadata

  if (!m_Params.sPublisher.isEmpty())
  {
    dwfWriter->AddProperty("publisher", m_Params.sPublisher.c_str());
  }
  if (!m_Params.sWideComments.isEmpty())
  {
    dwfWriter->AddProperty("comments", m_Params.sWideComments.c_str());
  }

  // The optional global section can contain properties common to all pages in a DWF EPlot package.  
  EPlotGlobalDescriptor *global_descriptor 
    = manager->CreateGlobalDescriptor( dwfWriter,
                                       (!m_Params.sSourceProductName.isEmpty())
                                          ? m_Params.sSourceProductName.c_str()
                                          : "Open Design DWF Export using the DWF Toolkit v6.0" );

  // Add a table of contents to the Dwf package
  EPlotBookmarkRef rootBookmark = new EPlotBookmark();
  global_descriptor->GetGlobal()->SetBookmark(rootBookmark);

  // It should provide the actual source Filename from which all of the resources 
  //  on this page are generated or in which they are somehow contained.
//DWFUtil::String sourceFilename = m_Params.sSourceFileName.c_str();
  DWFUtil::String sourceFilename = m_Params.pDb->getFilename().c_str();

  // The creation and modification times for the source design data
  DWFUtil::String usCreationTime;
  DWFUtil::String usModificationTime;

  OdDbDate createTime(m_Params.pDb->getTDCREATE());
  createTime.universalToLocal();

  OdString strLocalTime;
  createTime.strftime("%c", strLocalTime);

  usCreationTime = strLocalTime.c_str();
  usModificationTime = strLocalTime.c_str();

  //
  // To publish all prepared W2D files
  //

  pmStart("Writing Dwf6 file...");
//  pmSetLimit(g_arrayW2Dfiles.size());
  pmSetLimit(g_arrayLayoutInfo.size());

//  for (UINT i = 0; i < g_arrayW2Dfiles.size(); i++)
  for (UINT i = 0; i < g_arrayLayoutInfo.size(); i++)
  {
    bool        bPageDataExists(false);
    DwfPageData pageData;

    // If the page data exists to get them
    if (i < m_Params.arPageData.size())
    {
      bPageDataExists = true;
      pageData = m_Params.arPageData[ g_arrayLayoutInfo[i].m_iParamIndex ];
    }
    else
    {
      bPageDataExists = false;
    }

    DWFUtil::String uuidStr = DWFUtil::GuidGen(false);  // a GUID

    DWFUtil::String sSectionTitle;                      // Section title
    if (bPageDataExists && !pageData.sPageTitle.isEmpty())
    {
      sSectionTitle = pageData.sPageTitle.c_str();
    }
    else if (bPageDataExists && !pageData.sLayout.isEmpty())
    {
      sSectionTitle = pageData.sLayout.c_str();
    }
    else
    {
//      sSectionTitle = g_arrayLayoutNames[i].c_str();
      sSectionTitle = g_arrayLayoutInfo[i].m_sLayoutNames.c_str();
      /*
      // as default to get Section Title from w2d file name
      OdString sTmp( g_arrayW2Dfiles[i].c_str() );
      int iSlash = sTmp.reverseFind('\\');
	    if (iSlash != (-1) )
	    {
        sTmp = sTmp.mid(iSlash + 1);
      }
	    if ( (iSlash = sTmp.reverseFind('.')) != (-1) )
	    {
        sTmp = sTmp.left(iSlash);
      }
      sSectionTitle = sTmp.c_str();
      */
    }

    // Create the required descriptor object (one per page)
    EPlotPageDescriptor *page_descriptor 
      = manager->CreatePageDescriptor( dwfWriter,
                                       sSectionTitle,   // Section title
                                       sourceFilename,  // Path to the native-data source file 
                                                        //  used to generate this section
                                       uuidStr          // a GUID
                                      );
    // Get the <Page> object from the page descriptor.
    EPlotPageRef page = page_descriptor->GetPage();

    page->SetPlotOrder(i + 1);    // as default to use publishing number

    // Set some page metadata (all of these are optional)

    if (bPageDataExists)
    {
      if (!pageData.sLayout.isEmpty())
      {
        // Save layout name for roundtrip
        page->GetProperties()->AddProperty( "Layout Name", pageData.sLayout.c_str() );
      }
      if (!pageData.sPageAuthor.isEmpty())
      {
        page->GetProperties()->AddProperty(EPlotProperty::S_PROPERTY_AUTHOR,  pageData.sPageAuthor.c_str());
      }
      if (!pageData.sPageSubject.isEmpty())
      {
        page->GetProperties()->AddProperty(EPlotProperty::S_PROPERTY_SUBJECT, pageData.sPageSubject.c_str());
      }
      if (!pageData.sPageCompany.isEmpty())
      {
        page->GetProperties()->AddProperty(EPlotProperty::S_PROPERTY_COMPANY, pageData.sPageCompany.c_str());
      }
      if (!pageData.sPageComments.isEmpty())
      {
        page->GetProperties()->AddProperty(EPlotProperty::S_PROPERTY_COMMENTS,  pageData.sPageComments.c_str());
      }
      if (!pageData.sPageReviewers.isEmpty())
      {
        page->GetProperties()->AddProperty(EPlotProperty::S_PROPERTY_REVIEWERS, pageData.sPageReviewers.c_str());
      } 
      if (!pageData.sPageKeywords.isEmpty())
      {
        page->GetProperties()->AddProperty(EPlotProperty::S_PROPERTY_KEYWORDS,  pageData.sPageKeywords.c_str());
      }
      if (!pageData.sPageDescription.isEmpty())
      {
        page->GetProperties()->AddProperty(EPlotProperty::S_PROPERTY_DESCRIPTION, pageData.sPageDescription.c_str());
      }
      if (!pageData.sPageCopyright.isEmpty())
      {
        page->GetProperties()->AddProperty(EPlotProperty::S_PROPERTY_COPYRIGHT, pageData.sPageCopyright.c_str());
      }
    }

    // Creation and Modification time
    DWFUtil::String usPageTime(strLocalTime.c_str());

    // TODO: The form of Manifest schema-valid time strings. For instance, CCYY-MM-DDThh:mm:ss.sss

    page->GetProperties()->AddProperty(EPlotProperty::S_PROPERTY_CREATION_TIME, usPageTime);
    page->GetProperties()->AddProperty(EPlotProperty::S_PROPERTY_MODIFICATION_TIME, usPageTime);

    if (!usCreationTime.empty())
    {
      page->GetProperties()->AddProperty(EPlotProperty::S_PROPERTY_SOURCE_CREATION_TIME, usCreationTime);
    }
    if (!usModificationTime.empty())
    {
      page->GetProperties()->AddProperty(EPlotProperty::S_PROPERTY_SOURCE_MODIFICATION_TIME, usModificationTime);
    }

    
    EPlotGraphicResourceRef w2d = page_descriptor->CreateGraphicResource();

    // ADD THE W2D TO THE PAGE
    //  NOTE: for each page, we can only have ONE resource with the role "Graphics2d".

    FileResourceHandlerRef w2dHandler 
//      = new FileResourceHandler( g_arrayW2Dfiles[i].c_str(),            // The internal W2D stream name
      = new FileResourceHandler( g_arrayLayoutInfo[i].m_sW2Dfiles.c_str(),// The internal W2D stream name
                                 XmlRole::Graphics2d->GetStringValue(), 
                                 EPlotPageDescriptor::MIME_W2D
                                );

    // Assign a unique GUID for this resource.
    w2d->SetObjId( DWFUtil::GuidGen(false) );

    // Let's cook up some paper & the transform of the resource
    setPlottingSpace(i, page, w2d);

    // Write the resource
    page_descriptor->CommitResource( w2d.Object(), w2dHandler.Object() );


    if (bPageDataExists)
    {
      // ADD A THUMBNAIL

      if (!pageData.thumbnail.sFileName.isEmpty())
      {
        OdString sMimeType;

        // To specify the image file resource's MIME-type
        if (SpecifyImageMimeType(sMimeType, pageData.thumbnail.sFileName))
        {
          EPlotImageResourceRef ires = page_descriptor->CreateImageResource(w2d.Object());
                                                              // the W2D file is the parent of the thumbnail
          FileResourceHandlerRef iresHandler = 
              new FileResourceHandler( pageData.thumbnail.sFileName.c_str(),
                                       XmlRole::Thumbnail->GetStringValue(), 
                                       sMimeType.c_str() );

          ires->SetColorDepth(pageData.thumbnail.ColorDepth);   // the number of colors used in the image

          DWFUtil::Vector<double> extents;                      // left bottom right top
          extents.push_back(0); 
          extents.push_back(0); 
          extents.push_back(pageData.thumbnail.Width); 
          extents.push_back(pageData.thumbnail.Height);
          ires->SetExtents(extents);                            // the extents of the image, in pixels.

          page_descriptor->CommitResource(ires.Object(), iresHandler.Object());   // Write the resource
        }
      }

      // ADD A PREVIEW

      if (!pageData.preview.sFileName.isEmpty())
      {
        OdString sMimeType;

        // To specify the image file resource's MIME-type
        if (SpecifyImageMimeType(sMimeType, pageData.preview.sFileName))
        {
          EPlotImageResourceRef ires = page_descriptor->CreateImageResource(w2d.Object());
                                                              // the W2D file is the parent of the thumbnail
          FileResourceHandlerRef iresHandler = 
              new FileResourceHandler( pageData.preview.sFileName.c_str(),
                                       XmlRole::Preview->GetStringValue(), 
                                       sMimeType.c_str() );

          ires->SetColorDepth(pageData.preview.ColorDepth);     // the number of colors used in the image

          DWFUtil::Vector<double> extents;                      // left bottom right top
          extents.push_back(0); 
          extents.push_back(0); 
          extents.push_back(pageData.preview.Width); 
          extents.push_back(pageData.preview.Height);
          ires->SetExtents(extents);                            // the extents of the image, in pixels.

          page_descriptor->CommitResource(ires.Object(), iresHandler.Object());   // Write the resource
        }
      }

      // ADD A FONT

      for (UINT i = 0; i < pageData.arFonts.size(); i++)
      {
        DwfPageData::FontResource fontResource = pageData.arFonts[i];

        // add a embedded font resource to
        EPlotFontResourceRef fres = page_descriptor->CreateFontResource();
        FileResourceHandlerRef fresHandler = 
            new FileResourceHandler( fontResource.sFileName.c_str(),
                                     XmlRole::Font->GetStringValue(), 
                                     EPlotPageDescriptor::MIME_FONT );

        // The required font attributes.
        fres->SetRequest(       fontResource.request        );
        fres->SetPrivilege(     fontResource.privilege      );
        fres->SetCharacterCode( fontResource.characterCode  );
        fres->SetCanonicalName( fontResource.sCanonicalName.c_str() );
        fres->SetLogfontName(   fontResource.sLogfontName.c_str()   );

        page_descriptor->CommitResource(fres.Object(), fresHandler.Object());     // Write the resource
      }
    }

    DWFUtil::String pageName = page_descriptor->GetName();

    delete page_descriptor; 

    // Add the bookmark to the table of contents for the DWF EPlot pages
    DWFUtil::String pageBookmarkName = sSectionTitle + " Page";
    DWFUtil::String pageHRef("?docType=ePlot&page=" + pageName );
    EPlotBookmarkRef pageBookmark = new EPlotBookmark( pageBookmarkName, pageHRef );
    rootBookmark->AddBookmark(pageBookmark);

    pmMeterProgress();
  }

  pmStop();

  delete global_descriptor;
  delete dwfWriter;
  delete manager;
}


//-----------------------------------------------------------------

//
// setPlottingSpace()
//
//  Function describes the real-world plotting. 
//  Sets information relating the WHIP! drawing coordinate space to plotting (paper) space.
//  Sets the transform of the resource.
//
void CDwfExportImpl::setPlottingSpace(int tag, EPlotPageRef& page, EPlotGraphicResourceRef& w2d)
{
//  DwfLayoutPlotInfo plotInfo = g_arrayPlotInfo[tag];
  DwfLayoutPlotInfo plotInfo = g_arrayLayoutInfo[tag].m_PlotInfo;

  // To set plotting space for model space in depend on extents
  OdDbObjectId layoutId = g_arrayLayoutInfo[tag].m_idLayout;
  OdDbLayoutPtr pLayout = layoutId.safeOpenObject();
  if ( pLayout->getBlockTableRecordId() == m_Params.pDb->getModelSpaceId() )
  {
    if (m_bLandscape)
    {
      if (plotInfo.m_dPaperHeight > plotInfo.m_dPaperWidth)
      {
        double dTmp = plotInfo.m_dPaperHeight;
        plotInfo.m_dPaperHeight = plotInfo.m_dPaperWidth;
        plotInfo.m_dPaperWidth = dTmp;
      }
    }
    else
    {
      if (plotInfo.m_dPaperHeight < plotInfo.m_dPaperWidth)
      {
        double dTmp = plotInfo.m_dPaperHeight;
        plotInfo.m_dPaperHeight = plotInfo.m_dPaperWidth;
        plotInfo.m_dPaperWidth = dTmp;
      }
    }
  }


  EPlotPaperRef paper = page->GetPaper();

  // To set the desired control color, behind any "paper" graphic that is shown. 
  //  Viewers may optionally support this color.
  DWFUtil::Vector<long> color;
  color.push_back(ODGETRED(m_Params.background));
  color.push_back(ODGETGREEN(m_Params.background));
  color.push_back(ODGETBLUE(m_Params.background));
  paper->SetColor(color);

  // To define the paper onto which the graphics will be placed.

  double trueWidth  = abs( m_rectViewport.right - m_rectViewport.left );
  double trueHeight = abs( m_rectViewport.top - m_rectViewport.bottom );

  double scaleX, scaleY;

  DWFUtil::Vector<double> paperClip;

  paper->SetShow(true/*false*/);

  if (plotInfo.m_PlotPaperUnit == OdDbPlotSettings::kMillimeters)
  {
    paper->SetUnits( EPlotPaper::Millimeters );
    paper->SetHeight( plotInfo.m_dPaperHeight );
    paper->SetWidth( plotInfo.m_dPaperWidth );
  }
  else
  {
    paper->SetUnits( EPlotPaper::Inches );
    paper->SetHeight( plotInfo.m_dPaperHeight / kMmPerInch);
    paper->SetWidth( plotInfo.m_dPaperWidth / kMmPerInch);
  }

  //bool landscape = (trueWidth > trueHeight);
  //paper->SetHeight(landscape ? 210 : 297);
  //paper->SetWidth(landscape ? 297 : 210);
  //paper->SetHeight(landscape ? 8.5 : 11);
  //paper->SetWidth(landscape ? 11 : 8.5);

  double stretchH = trueHeight; //(trueHeight * 1.10);
  double stretchW = trueWidth;  //(trueWidth * 1.10);

  scaleX = paper->GetWidth() / (stretchW ? stretchW : paper->GetWidth());
  scaleY = paper->GetHeight() / (stretchH ? stretchH : paper->GetHeight());

  paperClip.push_back(0);
  paperClip.push_back(0);
  paperClip.push_back(paper->GetWidth());
  paperClip.push_back(paper->GetHeight());

  paper->SetClip(paperClip);

  // To specify a clipping rect for any graphic resource, if applicable.  
  //  This rectangle is in paper units and is relative to the paper "origin", 
  //  which is the bottom left corner of the paper.

  DWFUtil::Vector<double> clipRect;
/*
  clipRect.push_back(0.5);
  clipRect.push_back(0.60492008963916233);
  clipRect.push_back(10.5);
  clipRect.push_back(7.8950799103608365);

  w2d->SetClip(clipRect);
*/

  // To set the transform of the resource.  This is a transform mapping
  //  the drawing vector-space (W2D) to the paper.

	double centerDrawingX =  ( m_rectViewport.right + m_rectViewport.left ) / 2;
	double centerDrawingY =  ( m_rectViewport.top + m_rectViewport.bottom ) / 2;

	double centerPaperX =  (paperClip[2] + paperClip[0] ) / 2;
	double centerPaperY =  (paperClip[3] + paperClip[1] ) / 2;

  double scale = max (scaleX, scaleY);

	double transX = centerPaperX - scale * centerDrawingX;
	double transY = centerPaperY - scale * centerDrawingY;

  // To set the W2D resource's transform (identity).

  DWFUtil::Vector<double> dwfToPaper;

  dwfToPaper.push_back(scale); dwfToPaper.push_back(0); dwfToPaper.push_back(0); dwfToPaper.push_back(0);
  dwfToPaper.push_back(0); dwfToPaper.push_back(scale); dwfToPaper.push_back(0); dwfToPaper.push_back(0);
  dwfToPaper.push_back(0); dwfToPaper.push_back(0); dwfToPaper.push_back(1); dwfToPaper.push_back(0);
  dwfToPaper.push_back(transX); dwfToPaper.push_back(transY); dwfToPaper.push_back(0); dwfToPaper.push_back(1);

  w2d->SetTransform(dwfToPaper);

  // Set the W2D's extents
  DWFUtil::Vector<double> extents;
  extents.push_back(m_rectViewport.left   * scale + transX);  // left
  extents.push_back(m_rectViewport.bottom * scale + transY);  // bottom
  extents.push_back(m_rectViewport.right  * scale + transX);  // right
  extents.push_back(m_rectViewport.top    * scale + transY);  // top
  w2d->SetExtents(extents);
}


//
// getPlotPaperSize()
//
//  get paper size for current view
void CDwfExportImpl::getPlotPaperSize( double& plotWidth, double& plotHeight )
{
  ODA_ASSERT(m_Layout >= 0);

  if (isDwfVersion6())
  {
    DwfLayoutPlotInfo plotInfo = g_arrayLayoutInfo[m_Layout].m_PlotInfo;

    plotWidth = plotInfo.m_dPaperWidth;
    plotHeight = plotInfo.m_dPaperHeight;

  }
  else
  {
      plotWidth  = 215.89999389648;
      plotHeight = 279.39999389648;
  }
}

