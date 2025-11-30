#ifndef _DwfLineManager_Included_
#define _DwfLineManager_Included_

class DwfImporter;
/////////////////////////////////////////////////////
// Line management
//
class DwfLineManager
{
  DwfImporter* _importer;
  // mapping: DWF line style -> DWG line style
  typedef std::map<int, OdDbObjectId> LineStyleMap;
  LineStyleMap _lineStyles;
  static void setLinePattern(OdDbLinetypeTableRecord* pLt, const short* pattern, int length, double scale);
  static void setPredefinedPattern(OdDbLinetypeTableRecord* pLt, int patternId, double scale);
public:	
  DwfLineManager(DwfImporter* importer) : _importer(importer){}
  // get current line type & update _lineStyles mapping
  OdDbObjectId getCurrentLineStyle(WT_File& file);
  // convert DWF lineweight to DWG lineweight
  OdDb::LineWeight getCurrentLineWeight(WT_File& file);
  // create polyline entity from point set
  void addPolyline(WT_Point_Set& pl, WT_File& file);
};
#endif

