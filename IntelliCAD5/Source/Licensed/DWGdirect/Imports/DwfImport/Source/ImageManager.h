#ifndef _DwfImageManager_Included
#define _DwfImageManager_Included

class DwfImporter;
///////////////////////////////////////////////
// Image management
//
class DwfImageManager	
{
  DwfImporter* _importer;
  // mapping: DWF image id -> OdDbRasterImageDef id
  typedef std::map<int, OdDbObjectId> ImageMap;
  ImageMap _imageMap;
public:	
  DwfImageManager(DwfImporter* importer) : _importer(importer){}
    // add new image entity at the given location
    void addImage(const unsigned char* data, int dataSize, int cols, int rows, 
    int format, const WT_Logical_Point& ll, const WT_Logical_Point& ur, int id, WT_File& file);
  void clear();
};
#endif
