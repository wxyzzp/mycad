#ifndef _DWFIMPORTER_INCLUDED_
#define _DWFIMPORTER_INCLUDED_

#include "../include/DwfImport.h"
#include "DbBlockTableRecord.h"
#include "DbLinetypeTableRecord.h"
#include "DbGroup.h"
#include "DbHatch.h"
#include "DbPlotSettings.h"
#include "DbTextStyleTableRecord.h"
#include "DbDatabase.h"
#define STL_USING_MAP
#define STL_USING_VECTOR
#include "OdaSTL.h"
#include "whiptk/whip_toolkit.h"
#include "Dwf/DwfToolkit.h"
#include "Dwf/dwf_core.h"
#include "ExtentManager.h"
#include "FontManager.h"
#include "BlockManager.h"
#include "ImageManager.h"
#include "LineManager.h"

class DwfProperties;
typedef OdSmartPtr<DwfProperties> DwfPropertiesPtr;

// Upper level import manager
//
class DwfImporter : public OdDwfImport
{
	// database to fill
	OdDbDatabasePtr _db;
  // 
  DwfPropertiesPtr _properties;
  
public:
  DwfImporter();
  OdDbDatabase* database(){return _db;}
  OdRxDictionaryPtr properties();
  ImportResult import();
private:
	
	// load DWF (ver >= 600)
  // return values described in "Dwfimport.h"
	ImportResult loadPackage( const char* path, const char* password );

	// loads old style DWF (ver<=55) || new DFW .w2d section 
	// path = path to old DWF || materialized w2d
  // return values described in "Dwfimport.h"
	ImportResult loadStream( const char* path );
  // load w2d stream, calculate size and set paper
  ImportResult loadW2dExt( const char* path, double wPaper, double hPaper, OdDbPlotSettings::PlotPaperUnits units = OdDbPlotSettings::kMillimeters );
  
  // cleanup all page specific stuff (text styles etc.)
  void cleanupW2D();

  /////////////////////////////////////////////////////
  // import helpers, state flags & variables
  /////////////////////////////////////////////////////
private:
  DwfExtentManager _extent;
  DwfBlockManager _blocks;
  DwfFontManager _fonts;
  DwfImageManager	_images;
  DwfLineManager _lines;
public: 
  // Extent, scale & paper management
  DwfExtentManager& extent(){return _extent;}
  // Block, group & layer management
  DwfBlockManager& blocks(){return _blocks;}
  // font & text management
  DwfFontManager& fonts(){return _fonts;}
  // Image management
  DwfImageManager& images(){return _images;}
  // Line management
  DwfLineManager& lines(){return _lines;}
  // flag: ignore metadata while reading w2d if version >= 600
	bool _ignoreMetadata;
};


namespace dwfImp
{
  // generate name, not in dictionary / table
  template<class T> static OdString generateName(const char* begin, const T& table, int start_no = 0)
  {
    char name[20];
    for (;;) {
      sprintf(name, "%s%d", begin, start_no++);
      if (!table->has(name)) break;
    }
    return OdString(name);
  }
}


#endif // _DWFIMPORTER_INCLUDED_
