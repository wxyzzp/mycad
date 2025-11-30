// DWGdirectConverter.h: interface for the DWGdirectProtocolExtension class.
//
//////////////////////////////////////////////////////////////////////

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __DWGdirectConverter__
#define __DWGdirectConverter__

#include "DWGdirectWriter.h"

#include "OdaCommon.h"
#include "DbDatabase.h"
#include "..\\..\\Licensed\\DWGdirect\\include\\DbEntity.h"	// File name clash with ...lib\db\dbgroup.h
#include "CollectionClasses.h"
#include "ModelerDefs.h"

//#define _DEBUG_MHB
class DWGdirectConverter
{
public:
	DWGdirectConverter(db_drawing* drawing, 
						OdDbDatabasePtr& pOdDatabase, 
							OdDb::DwgVersion& saveVersion, 
								OdDb::SaveType& saveType,
									OdRxObjectImpl<DWGdirectWriterServices>& exServices);
	~DWGdirectConverter();	

	OdDbObjectId createOdDbEntity(OdDbObjectId ownerId, db_handitem* pEntity);
	OdDbObjectId createOdDbObject(OdDbObjectId ownerId, db_handitem* pEntity, OdString recordName);

	// Used to update an existing object instead of creating one.
	OdDbObjectId updateOdDbObject(OdDbObjectId ownerId, db_handitem* pEntity, OdDbObjectPtr pExistingObject);

private:
	OdDb::DwgVersion m_DwgSaveVersion;
	OdDb::SaveType   m_DwgSaveType;
	db_drawing* m_pICdwg;
	OdDbDatabasePtr m_pOdDatabase;
	reactorHandleMap m_persistantReactorList;
	delayedConversionMap m_delayedEntities;
	delayedConversionMap m_proxyReferences;
	OdRxObjectImpl<DWGdirectWriterServices>* m_pExServices;
	HandleMapOldStrNewStr m_blockHandleMap;

	OdDbEntityPtr createOdDbLine(db_handitem* pEntity);
	OdDbEntityPtr createOdDbPoint(db_handitem* pEntity);
	OdDbEntityPtr createOdDbCircle(db_handitem* pEntity);
	OdDbEntityPtr createOdDbArc(db_handitem* pEntity);
	OdDbEntityPtr createOdDbTolerance(db_handitem* pEntity, OdDbObjectId ownerId);
	OdDbEntityPtr createOdDbTrace(db_handitem* pEntity);
	OdDbEntityPtr createOdDbShape(db_handitem* pEntity, OdDbObjectId ownerId);
	OdDbEntityPtr createOdDbSolid(db_handitem* pEntity);
	OdDbEntityPtr createOdDbFace(db_handitem* pEntity);
	OdDbEntityPtr createOdDbLWPolyline(db_handitem* pEntity);
	OdDbEntityPtr createOdDbEllipse(db_handitem* pEntity);
	OdDbEntityPtr createOdDbSpline(db_handitem* pEntity);
	OdDbEntityPtr createOdDbVertex(int type, db_handitem* pEntity);
	OdDbEntityPtr createOdDbRay(db_handitem* pEntity);
	OdDbEntityPtr createOdDbXline(db_handitem* pEntity);
	OdDbEntityPtr createOdDbArcAlignedText(db_handitem* pEntity, OdDbObjectId ownerId);
	OdDbEntityPtr createOdDbRText(db_handitem* pEntity);
	OdDbEntityPtr createOdDbText(db_handitem* pEntity);
	OdDbEntityPtr createOdDbMText(db_handitem* pEntity);
	OdDbEntityPtr createOdDbOle2Frame(db_handitem* pEntity);
	OdDbEntityPtr createOdDbHatch(db_handitem* pEntity, OdDbObjectId ownerI);
	OdDbEntityPtr createOdDbAttDef(db_handitem* pEntity);
	OdDbEntityPtr createOdDbViewport(db_handitem* pEntity);
	OdDbEntityPtr createOdDbWipeout(db_handitem* pEntity, OdDbObjectId ownerId);
	OdDbEntityPtr createOdDb3DSolid(db_handitem* pEntity);
	OdDbEntityPtr createOdDbRegion(db_handitem* pEntity);
	OdDbEntityPtr createOdDbBody(db_handitem* pEntity);
	OdDbEntityPtr createOdDbMline(db_handitem* pEntity);
	OdDbEntityPtr createOdDbDimension(db_handitem* pEntity);
	OdDbEntityPtr createOdDbRasterImage(db_handitem* pEntity);
	OdDbEntityPtr createOdDbLeader(db_handitem* pEntity);
	OdDbEntityPtr createOdDbProxyEntity(db_handitem* pEntity, OdDbObjectId ownerId);

	OdDbEntityPtr createOdDbAttribute(db_handitem* pEntity, OdDbEntity* pOwner);

	OdDbEntityPtr createOdDbPolyline(db_handitem* pEntity, OdDbObjectId ownerId);
	OdDbEntityPtr makePolygonMesh(db_handitem* pEntity, OdDbObjectId ownerId);
	OdDbEntityPtr make3dPolyline(db_handitem* pEntity, OdDbObjectId ownerId);
	OdDbEntityPtr makePolyFaceMesh(db_handitem* pEntity, OdDbObjectId ownerId);
	OdDbEntityPtr makeSplineFitPolyline(db_handitem* pEntity, OdDbObjectId ownerId);

	OdDbEntityPtr createOdDbBlockReference(db_handitem* pEntity, OdDbObjectId ownerId);
	OdDbEntityPtr createOdDbMInsertBlock(db_handitem* pEntity, OdDbObjectId ownerId);

	OdDbObjectPtr createOdDbGroup(OdDbObjectId ownerId, db_handitem* pObject);
	OdDbObjectPtr createOdDbRasterImageDef(OdDbObjectId ownerId, db_handitem* pObject);

	OdDbObjectPtr createOdDbLayout(OdDbObjectId blockRecId, db_handitem* pObject);
	OdDbObjectPtr updateOdDbLayout(OdDbObjectId dictionaryId, db_handitem* pObject, OdDbObject* pExistingOdLayout);

	OdDbObjectPtr createOdDbMlStyle(OdDbObjectId mlStyleDictId, db_handitem* pMlStyle);
	OdDbObjectPtr updateOdDbMlStyle(OdDbObjectId dictionaryId, db_handitem* pObject, OdDbObject* pExistingMlStyle);

	OdDbObjectPtr createOdDbDimStyle(OdDbObjectId ownerId, db_handitem* pObject);
	OdDbObjectPtr updateOdDbDimStyle(OdDbObjectId dictionaryId, db_handitem* pObject, OdDbObject* pExistingDimStyle);

	OdDbObjectPtr createOdDbTextStyle(OdDbObjectId ownerId, db_handitem* pObject);
	OdDbObjectPtr updateOdDbTextStyle(OdDbObjectId dictionaryId, db_handitem* pObject, OdDbObject* pExistingDimStyle);

	OdDbObjectPtr createOdDbLinetype(OdDbObjectId ownerId, db_handitem* pObject);
	OdDbObjectPtr updateOdDbLinetype(OdDbObjectId dictionaryId, db_handitem* pObject, OdDbObject* pExistingDimStyle);

	OdDbObjectPtr createOdDbPlotSettings(OdDbObjectId blockRecId, db_handitem* pObject);
	OdDbObjectPtr createOdDbPlaceholder(OdDbObjectId dictionaryId, db_handitem* pObject);
	OdDbObjectPtr createOdDbRasterVariables(OdDbObjectId dictionaryId, db_handitem* pObject);

	OdDbObjectPtr createOdDbDimAssoc(OdDbObjectId dictionaryId, db_handitem* pObject);
	OdDbObjectPtr createOdDbDictionaryVar(OdDbObjectId dictionaryId, db_handitem* pObject);
	OdDbObjectPtr createOdDbXRecord(OdDbObjectId dictionaryId, db_handitem* pObject);
	OdDbObjectPtr createOdDbSpatialFilter(OdDbObjectId dictionaryId, db_handitem* pObject);
	OdDbObjectPtr createOdDbProxyObject(OdDbObjectId dictionaryId, db_handitem* pObject);
//	OdDbObjectPtr createOdDbLayerIndex(OdDbObjectId dictionaryId, db_handitem* pObject, OdString& recordName);

	OdDbObjectPtr createOdDbSortEnts(OdDbObjectId ownerId, db_handitem* pObject);
	OdDbObjectPtr createOdDbVBAproject(OdDbObjectId ownerId, db_handitem* pObject);
	OdDbObjectPtr createOdDbIDBuffer(OdDbObjectId ownerId, db_handitem* pObject);

	OdGeCurve2d* makeHatchLineEdge(db_hatch* hatchEnt, int loop, int edgeIndex);
	OdGeCurve2d* makeHatchArcEdge(db_hatch* hatchEnt, int loop, int edgeIndex);
	OdGeCurve2d* makeHatchEllipticArcEdge(db_hatch* hatchEnt, int loop, int edgeIndex);
	OdGeCurve2d* makeHatchSplineEdge(db_hatch* hatchEnt, int loop, int edgeIndex);
	void makeHatchPolylineEdge(db_hatch* hatchEnt, int loop, OdGePoint2dArray& vertices, OdGeDoubleArray& bulges);
	int makeHatchBoundaryAssociations(db_hatch* hatchEnt, OdDbHatchPtr& pOdDbHatch);
	void makeHatchPatternDefinition(db_hatch* hatchEnt, OdDbHatchPtr& pOdDbHatch);

	OdDbEntityPtr makeRotateDimension(db_dimension* pDimension);
	OdDbEntityPtr makeAlignedDimension(db_dimension* pDimension);
	OdDbEntityPtr makeAngularDimension(db_dimension* pDimension);
	OdDbEntityPtr makeDiametricDimension(db_dimension* pDimension);
	OdDbEntityPtr makeRadialDimension(db_dimension* pDimension);
	OdDbEntityPtr make3PointAngularDimension(db_dimension* pDimension);
	OdDbEntityPtr makeOrdinateDimension(db_dimension* pDimension);

	OdDbObjectId getBlockId(db_handitem* pBlockRec);

	void saveOffDelayedEntities(db_handitem* pEntity, OdDbObjectId ownerId);

	void processDictionaryRecord(OdDbDictionary* pDict, db_hireflink* recordLink, OdString recordName = OdString());

	void finalize(db_handitem* pEntity, OdDbEntity* pEnt);
	void appendData(db_handitem* pEntity, OdDbObject* pEnt);

	OdDb::DwgVersion&		getSaveVersion() { return m_DwgSaveVersion; }
	OdDb::SaveType&			getSaveType()	 { return m_DwgSaveType; }

	AfTypeVer getACISVersion();
public:
	// These functions facilitate so called - delayed - processing of entities.
	// For example an associative hatch needs to have its boundary objects created
	// prior to associating them to the hatch. Or a block must be created before
	// a reference to it can be made. This is accomplished by using saveOffDelayedEntities()
	// to maintain a list of entities and objects that will be processed after all the entities
	// blocks and objects have been converted.
	void processPersistantReactors();
	void processDelayedEntities();
	void processProxyReferences();
	void addBlockHandleMap(OdString& oldHandle, handleName& newHandleName) 
		{ m_blockHandleMap[OdString(oldHandle)] = newHandleName; }
};

#endif	// __DWGdirectConverter__
