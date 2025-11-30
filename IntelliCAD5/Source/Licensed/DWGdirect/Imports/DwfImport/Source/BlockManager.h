#ifndef _DwfBlockManager_Included_
#define _DwfBlockManager_Included_

class DwfImporter;

/////////////////////////////////////////////////////
// Block, group & layer management
//
class DwfBlockManager
{
  DwfImporter* _importer;
  // current group (WT_Object_Node block = OdDbGroup)
  OdDbGroupPtr _group;
  // mapping: WT_Object_Node id -> OdDbGroup
  typedef std::map<int, OdDbGroupPtr> GroupMap;
  GroupMap _groups;
  // current block, where data is inserted
  OdDbBlockTableRecordPtr _currentBlock;
  // mapping: DWF layer number -> DWG layer
  typedef std::map<int, OdDbObjectId> LayerMap;
  LayerMap _layerMap;
  OdDbLayoutPtr _layout;
  // see registerColorMap() ...
  bool _preserveColorIndices;
public:
  //
  DwfBlockManager( DwfImporter* importer ) 
    : _importer( importer ), _preserveColorIndices( false ){}
  // set current group (add new if necesary) 
  void setCurrentGroup(int id, const char* name = 0);
  // current block, where data is inserted
  OdDbBlockTableRecordPtr currentBlock(){return _currentBlock;}
  void setCurrentBlock(OdDbObjectId id){_currentBlock = id.safeOpenObject(OdDb::kForWrite);}
  void setCurrentBlock(OdDbBlockTableRecordPtr block){_currentBlock = block;}
  // maintain mapping WT_Layers to DWG layers && set CLAYER (adding new if necessary)
  void setCurrentLayer(WT_Layer& layer);
  // add entity to the current block (and possibly, group)
  void addEntity(OdDbEntity* ent, WT_File& file);
	// create hatch entity from current rendition settings
	OdDbHatchPtr addHatch(WT_File& file);
	// Add viewport to the current layout
	OdDbObjectId addViewport();
  //
  void clear();
  // replace with '_' any character forbidden for named object name
  static OdString normalizeObjectName( const OdString& name );
  void setCurrentLayout( OdDbObjectId layoutId );
  OdDbLayoutPtr currentLayout(){return _layout;}
  // check if the palette coincides with light or dark dwg palette
  // and set _preserveColorIndices variable
  void registerColorMap( WT_Color_Map& cm );
};

#endif
