#include "StdAfx.h"
#include "../include/DwfImport.h"
#include "DwfImportImpl.h"
#include "DwfCallbacks.h"
#include "Dwf/DwfResult.h"
#define STL_USING_MEMORY
#include "OdaSTL.h"

DWFIMPORT_TOOLKIT OdDwfImportPtr createImporter()
{
  return OdRxObjectImpl<DwfImporter>::createObject();
}

class DwfProperties;
typedef OdSmartPtr<DwfProperties> DwfPropertiesPtr;

class DwfProperties : public OdRxDispatchImpl<>
{
  OdString _password;
  OdString _path;
  double _paperWidth, _paperHeight;
  OdDbDatabasePtr _db;
public:
  DwfProperties() : _paperWidth( 297 ), _paperHeight( 210 ) {}
  ODRX_DECLARE_DYNAMIC_PROPERTY_MAP( DwfProperties );
  static DwfPropertiesPtr createObject()
  {
    return OdRxObjectImpl<DwfProperties, OdRxDictionary>::createObject();
  }
  
  OdString get_Password() const { return _password; }
  void put_Password( OdString pass ){ _password = pass; }
  double get_PaperWidth() const { return _paperWidth; }
  void put_PaperWidth( double w ){ _paperWidth = w; }
  double get_PaperHeight() const { return _paperHeight; }
  void put_PaperHeight( double h ){ _paperHeight = h; }
  OdRxObjectPtr get_Database() const { return OdRxObject::cast( _db ); }
  void put_Database( OdRxObject* obj ){ _db = obj; }
  OdString get_DwfPath() const { return _path; }
  void put_DwfPath( OdString path ){ _path = path; }
};

ODRX_DECLARE_PROPERTY(DwfPath)
ODRX_DECLARE_PROPERTY(Password)
ODRX_DECLARE_PROPERTY(PaperWidth)
ODRX_DECLARE_PROPERTY(PaperHeight)
ODRX_DECLARE_PROPERTY(Database)

ODRX_DEFINE_PROPERTY(DwfPath, DwfProperties)
ODRX_DEFINE_PROPERTY(Password, DwfProperties)
ODRX_DEFINE_PROPERTY(PaperWidth, DwfProperties)
ODRX_DEFINE_PROPERTY(PaperHeight, DwfProperties)
ODRX_DEFINE_PROPERTY_OBJECT(Database, DwfProperties,  get_Database, put_Database, OdDbDatabase)

ODRX_BEGIN_DYNAMIC_PROPERTY_MAP( DwfProperties );
  ODRX_GENERATE_PROPERTY( DwfPath )
  ODRX_GENERATE_PROPERTY( Password )
  ODRX_GENERATE_PROPERTY( PaperWidth )
  ODRX_GENERATE_PROPERTY( PaperHeight )
  ODRX_GENERATE_PROPERTY( Database );
ODRX_END_DYNAMIC_PROPERTY_MAP( DwfProperties );

DwfImporter::DwfImporter() 
  : _ignoreMetadata(true),
  _fonts(this),
  _extent(this),
  _blocks(this),
  _images(this),
  _lines(this),
  _properties( DwfProperties::createObject() )
{
  cleanupW2D();
  DwfCallbackManager::_importer = this;
}

OdRxDictionaryPtr DwfImporter::properties() { return _properties; }

namespace dwfImp
{
  // version of DWF (6.00 = 600, 5.5 = 0.55 = 55, etc)
  //
  DWF::DwfType DWFVersion(const char* file)
  {
    FILE *fp = fopen(file, "rb");
    if ( !fp ) return DWF::Unknown;
    BYTE buf[12];
    if ( fread(buf, 1, 12, fp) != 12 )
    {
      fclose( fp ); 
      return DWF::Unknown;
    }
    fclose(fp);
    
    ULONG dwfVer;
    return DWFReader::DwfReader::GetDwfVersion( buf, dwfVer );
  }
}

OdDwfImport::ImportResult DwfImporter::loadW2dExt( const char* path, double wPaper, double hPaper, OdDbPlotSettings::PlotPaperUnits units )
{
  _extent._collectBounds = true;
  ImportResult retval = loadStream( path );
  if (retval) return retval;
  _extent.setPlotSettings( wPaper, hPaper, units);
  _extent.calculateScale();
  _ignoreMetadata = false;
  _extent._collectBounds = false;
  return loadStream( path );
}

OdDwfImport::ImportResult DwfImporter::import()
{
  _db = _properties->get_Database();
  if ( _db.isNull() ) 
  _db->setLWDISPLAY(true);
  _db->closeInput();  // some problems (with viewprt) in case that db was open partially  // MKU 10.06.2004

  _ignoreMetadata = true;
  OdString path = _properties->get_DwfPath();
  if ( path.isEmpty() ) return OdDwfImport::bad_file;
  switch ( dwfImp::DWFVersion( path ) )
	{
	case DWF::DWF_Package:
		return loadPackage( path, _properties->get_Password() );
	case DWF::DWF_Stream:
	case DWF::W2D_Stream:
		{
			// single sheet is imported into the active layout
      _blocks.setCurrentBlock( 
        _db->getTILEMODE() ? _db->getModelSpaceId() : _db->getPaperSpaceId() );
      _blocks.setCurrentLayout( _blocks.currentBlock()->getLayoutId() );
      return loadW2dExt( path, _properties->get_PaperWidth(), _properties->get_PaperHeight() );
		}
		break;
	}
  return OdDwfImport::bad_file;
}

using namespace DWFEPlot;
using namespace DWFReader;
using namespace DWFXmlObjects;
using namespace DWFManifest;

static EPlotDwfReader *createReader( const char* path, const char* password, 
                                    OdDwfImport::ImportResult& result )
{
  try 
  {
    return new EPlotDwfReader( path, password );
  }
  catch ( DWF::DwfResult::Enum e )
  {
    if (e == DWF::DwfResult::File_Open_Error)
      result = OdDwfImport::bad_file;
    else if (e == DWF::DwfResult::Bad_Password_Error)
      result = OdDwfImport::bad_password;
    else result = OdDwfImport::fail;
  }
  return 0;
}

static OdString getLayoutName( EPlotPageRef page )
{
  int n = page->GetProperties()->GetProperties().size();
  for ( int i = 0; i < n; i++ )
  {
    DWFEPlot::EPlotAnyPropertyRef prop = page->GetProperties()->GetProperties()[i];
    if ( prop->GetName() == "Layout Name" )
    {
      return ((SimpleProperty*)( prop.Object() ))->GetValue().ascii();
    }
  }
  return "";
}

static EPlotGraphicResourceRef getW2dResource( EPlotPageRef page )
{
  EPlotResourceVec::const_iterator iter = page->GetResources()->GetResources().begin();
  for (; iter != page->GetResources()->GetResources().end(); ++iter)
  {
    EPlotResourceRef resource = *iter;
    if ( resource->GetRole() == XmlRole::Graphics2d->GetStringValue() )
    {
      return (EPlotGraphicResource*)resource.Object();
    }
  }
  return 0;
}

static void setupLayout( const OdString& name, OdDbDatabase* db, DwfBlockManager& blocks )
{
  OdDbDictionaryPtr pLayoutDict = db->getLayoutDictionaryId().safeOpenObject(OdDb::kForWrite);
  if ( !name.isEmpty() && pLayoutDict->has( name ) )
  {
    OdDbLayoutPtr layout = pLayoutDict->getAt( name ).safeOpenObject();
    blocks.setCurrentLayout( layout->objectId() );
    blocks.setCurrentBlock( layout->getBlockTableRecordId() );
  }
  else
  {
    OdDbObjectId rec;
    blocks.setCurrentLayout( db->createLayout( 
      name.isEmpty() ? dwfImp::generateName( "Layout", pLayoutDict ) : name, &rec ) );
    blocks.setCurrentBlock( rec );
  }
}


OdDwfImport::ImportResult DwfImporter::loadPackage( const char* path, const char* password )
{
  ImportResult retval = OdDwfImport::success;
  std::auto_ptr<EPlotDwfReader> reader( createReader( path, password, retval ) );
  if ( !reader.get() ) return retval;
  
  // flag used to determine whether to read metadata (it is duplicated)
  bool bFirstPass = true;

  // W2D pages -> DWG layouts
  for ( unsigned int i = 0; i < reader->GetPages().size(); i++ )
  {
    EPlotPageRef page = reader->GetPages()[i];
    setupLayout( getLayoutName( page ), _db, _blocks );
    
    EPlotGraphicResourceRef primary_w2d = getW2dResource( page );
    if ( primary_w2d.Object() == 0 ) return OdDwfImport::bad_file;
 
    DWFUtil::String tmpFile = reader->MaterializeFile( primary_w2d->GetHref() );
    EPlotPaperRef paper = page->GetPaper();
    retval = loadW2dExt( tmpFile.ascii(), paper->GetWidth(), paper->GetHeight(), 
      (OdDbPlotSettings::PlotPaperUnits)(UINT)paper->GetUnits() );
    cleanupW2D();
    if ( retval ) return retval;
  }
  return retval;
}

// DWF-wide metadata has no correspondence in DWG ??
//
//void DwfImporter::ImportSumminfo(EPlotDwfReader* reader);
//{
//  OdDbDatabaseSummaryInfoPtr summ = oddbGetSummaryInfo(_db);
//  DWFManifest::ManifestRef manifest = reader->GetManifest();
//  summ->addCustomSummaryInfo(SummValue, OdString(v.string().ascii()));
//}
