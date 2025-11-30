// Utilitys.h: 
// Classes used for storing handles for matching and delayed processing
//////////////////////////////////////////////////////////////////////
#include "Db.h"
#include "OdaCommon.h"
#include "DbDatabase.h"
#include "DWGdirectConverter.h"

typedef OdArray<OdString> stringArray;

class ReadWriteUtils
{
public:
	static void         addEntitiesToBlocks(OdDbDatabasePtr& pDDdwg, db_drawing* pDwg, DWGdirectConverter* converter, db_blocktabrec* pBlock, OdDbBlockTableRecord* pOdDbBlock, bool bResolveLayout = true);
	static OdString     getStyleNameByHandle(db_drawing* pDwg, db_objhandle& styleHandle);
	static OdDbObjectId createBlock(OdDbDatabasePtr& pDDdwg, db_drawing* pDwg, DWGdirectConverter* converter, OdString blockName); 
	static OdDbObjectId getTextStyleIDByName(OdDbDatabasePtr& pDDdwg, OdString& name);
	static OdDbObjectId getLinetypeIDByName(OdDbDatabasePtr& pDDdwg, OdString& name);
	static OdDbObjectId getLayerIDByName(OdDbDatabasePtr& pDDdwg, OdString& name);
	static OdDbObjectId setHandle(OdDbDatabasePtr& pDDdwg, db_handitem* pHandItem, OdDbObjectId pOwnerId, OdDbObject* pOdDbObject);
	static OdDbObjectId getBlockIDByName(OdDbDatabasePtr& pDDdwg, db_drawing* pDwg, DWGdirectConverter* converter, OdString& name);
	static OdDbObjectId getBlockIDByHandle(OdDbDatabasePtr& pDDdwg, db_objhandle& styleHandle);
	static OdDbObjectId getDimStyleIDByName(OdDbDatabasePtr& pDDdwg, OdString& name);
	static OdDbObjectId getMLineStyleIDByName(OdDbDatabasePtr& pDDdwg, OdString& name);
	static OdResBufPtr  getOdResBufPtrFromSdsResBufPtr(OdDbDatabasePtr& pDDdwg, struct resbuf* extData);
	static struct sds_resbuf* getSdsResBufPtrFromOdResBuf(OdResBuf* pResChain, OdDbDatabase* pDDdwg);

	static int countLayouts(db_drawing* pDwg);
	// Returns number of errors detected.
	static int reportAuditInfo(db_drawing* pDb, 
								stringArray& summaryList, 
								stringArray& errorList, short flags = 0);

	static void _fixZeroRealDWGdirect(OdGeVector3d& pD);
	static void _fixZeroRealDWGdirect(OdGePoint3d& pD);

private:

	ReadWriteUtils() {}
	~ReadWriteUtils() {}

};