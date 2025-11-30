// DWGdirectConverter.cpp: implementation of the DWGdirect class.
//
//////////////////////////////////////////////////////////////////////

#include "Db.h"
#include "dimstyletabrec.h"
#include "styletabrec.h"
#include "DWGdirectConverter.h"
#include "DwgdirectWriter.h"
#include "DbAcisEntity.h"
#include "..\\db\\DbMText.h" // File name clash with ...Licensed\\DWGdirect\\include\\DbMText.h
#include "viewport.h"
#include "..\\..\\prj\\lib\\db\\dblayout.h" // File name clash with ..\\..\\Licensed\\DWGdirect\\include\\DbGroup.h" 
#include "..\\..\\prj\\lib\\db\\dbgroup.h" // File name clash with ..\\..\\Licensed\\DWGdirect\\include\\DbGroup.h" 
#include "objects.h"
#include "Utilities.h"
#include "point.h"
#include "..\\..\\prj\\lib\\db\\DbSortentsTable.h"

#include "DWGdirectFiler.h"

#include "modeler.h"

// OdDb includes
#include "DbBlockTableRecord.h"
#include "DbDictionary.h"
#include "DbCircle.h"
#include "DbLine.h"
#include "DbPoint.h"
#include "DbArc.h"
#include "DbTrace.h"
#include "DbSolid.h"
#include "DbFace.h"
#include "DbPolyline.h"
#include "DbEllipse.h"
#include "DbSpline.h"
#include "DbPolyFaceMeshVertex.h"
#include "DbPolygonMeshVertex.h"
#include "Db3dPolylineVertex.h"
#include "Db2dVertex.h"
#include "DbPolygonMesh.h"
#include "Db3dPolyline.h"
#include "DbPolyFaceMesh.h"
#include "DbRay.h"
#include "DbXline.h"
#include "Db3dSolid.h"
#include "DbRegion.h"
#include "DbBody.h"
#include "MemoryStream.h"
#include "DbText.h"
#include "..\\..\\Licensed\\DWGdirect\\include\\DbMText.h" // File name clash with ...lib\db\DbMtext.h
#include "DbOle2Frame.h"
#include "DbAttributeDefinition.h"
#include "..\\..\\Licensed\\DWGdirect\\include\\DbGroup.h"
#include "DbViewport.h"
#include "..\\..\\Licensed\\DWGdirect\\include\\DbLayout.h"
#include "DbMlineStyle.h"
#include "DbMline.h"
#include "DbBlockReference.h"
#include "DbAttribute.h"
#include "DbRasterImage.h"
#include "DbLeader.h"
#include "DbPlotSettingsValidator.h"
#include "DbProxyObject.h"
#include "DbXrecord.h"
#include "DbShape.h"
#include "DbWipeout.h"
#include "DbFcf.h"
#include "DbMInsertBlock.h"
#include "DbPlaceHolder.h"
#include "OleStorage.h"
#include "DbRasterVariables.h"
#include "DbFaceRecord.h"
#include "DbArcAlignedText.h"
#include "..\\..\\Licensed\\DWGdirect\\include\\DbSortentsTable.h"
#include "DbVbaProject.h"
#include "DbSpatialFilter.h"
#include "DbIdBuffer.h"
#include "RText.h"

// Hatch loops.
#include "GeCircArc2d.h"	
#include "GeEllipArc2d.h"
#include "GeKnotVector.h"
#include "GeNurbCurve2d.h"
#include "GeLineSeg2d.h"
#include "GeScale3d.h"

// Dimensions
#include "DbDimension.h"
#include "Db2LineAngularDimension.h"
#include "Db3PointAngularDimension.h"
#include "DbAlignedDimension.h"
#include "DbDiametricDimension.h"
#include "DbOrdinateDimension.h"
#include "DbRadialDimension.h"
#include "DbRotatedDimension.h"
#include "DbDimAssoc.h"

#include "DbWipeoutVariables.h"
#include "DbDictionaryVar.h"
#include "DbTextStyleTableRecord.h"

extern TOOLKIT_EXPORT void odDbSetRawHatchPattern(OdDbHatch& hatch,
                                           OdDbHatch::HatchPatternType patType, 
                                           const OdString& name,
                                           const OdHatchPattern& pat);

DWGdirectConverter::DWGdirectConverter(db_drawing* drawing, 
									   OdDbDatabasePtr& pOdDatabase, 
									   OdDb::DwgVersion& version, 
									   OdDb::SaveType& type,
									   OdRxObjectImpl<DWGdirectWriterServices>& exServices)
{
	ASSERT(drawing != NULL);
	ASSERT(!pOdDatabase.isNull());

	m_pICdwg = drawing; 
	m_pOdDatabase = pOdDatabase;
	m_DwgSaveVersion = version;
	m_DwgSaveType    = type;

	// Just in case
	m_persistantReactorList.clear();
	m_blockHandleMap.clear();

	m_pExServices = &exServices;
}

DWGdirectConverter::~DWGdirectConverter()
{
}

OdDbObjectId DWGdirectConverter::createOdDbEntity(OdDbObjectId ownerId, db_handitem* pEntity)
{
	ASSERT(pEntity != NULL);
	ASSERT(!ownerId.isNull());

	OdDbEntityPtr pEnt;
	switch(pEntity->ret_type())
	{
		case DB_3DFACE			 :
			pEnt = createOdDbFace(pEntity);
			break;
		case DB_3DSOLID 		 :
			pEnt = createOdDb3DSolid(pEntity);
			break;
		case DB_ACAD_PROXY_ENTITY:
			pEnt = createOdDbProxyEntity(pEntity, ownerId);
			break;
		case DB_ARC 			 :
			pEnt = createOdDbArc(pEntity);
			break;
		case DB_ATTDEF			 :
			pEnt = createOdDbAttDef(pEntity);
			break;
		case DB_ATTRIB			 :
			// Handled during DB_INSERT creation.
			break;
		case DB_BLOCK			 :
			// see DWGdirectWriter::TransferBlocksToDWGdirect()
			break;
		case DB_BODY			 :
			pEnt = createOdDbBody(pEntity);
			break;
		case DB_CIRCLE			 :
			pEnt = createOdDbCircle(pEntity);
			break;
		case DB_DIMENSION		 :
			pEnt = createOdDbDimension(pEntity);
			break;
		case DB_ELLIPSE 		 :
			pEnt = createOdDbEllipse(pEntity);
			break;
		case DB_ENDBLK			 :
			// NoOp.
			break;
		case DB_HATCH			 :
			saveOffDelayedEntities(pEntity, ownerId);
			break;
		case DB_IMAGE			 :
			pEnt = createOdDbRasterImage(pEntity);
			break;
		case DB_INSERT			 :
			pEnt = createOdDbBlockReference(pEntity, ownerId);
			break;
		case DB_LEADER			 :
			saveOffDelayedEntities(pEntity, ownerId);
			break;
		case DB_LINE			 :
			pEnt = createOdDbLine(pEntity);
			break;
		case DB_LWPOLYLINE		 :
			pEnt = createOdDbLWPolyline(pEntity);
			break;
		case DB_MLINE			 :
			pEnt = createOdDbMline(pEntity);
			break;
		case DB_MTEXT			 :
			pEnt = createOdDbMText(pEntity);
			break;
		case DB_OLE2FRAME		 :
			pEnt = createOdDbOle2Frame(pEntity);
			break;
		case DB_POINT			 :
			pEnt = createOdDbPoint(pEntity);
			break;
		case DB_POLYLINE		 :
			pEnt = createOdDbPolyline(pEntity, ownerId);
			break;
		case DB_RAY 			 :
			pEnt = createOdDbRay(pEntity);
			break;
		case DB_REGION			 :
			pEnt = createOdDbRegion(pEntity);
			break;
		case DB_SEQEND			 :
			// NoOp.
			break;
		case DB_SHAPE			 :
			pEnt = createOdDbShape(pEntity, ownerId);
			break;
		case DB_SOLID			 :
			pEnt = createOdDbSolid(pEntity);
			break;
		case DB_SPLINE			 :
			pEnt = createOdDbSpline(pEntity);
			break;
		case DB_TEXT			 :
			pEnt = createOdDbText(pEntity);
			break;
		case DB_TOLERANCE		 :
			pEnt = createOdDbTolerance(pEntity, ownerId);
			break;
		case DB_TRACE			 :
			pEnt = createOdDbTrace(pEntity);
			break;
		case DB_VERTEX			 :
			// Handled in polyline creation.
			//pEnt = createOdDbVertex(pEntity);
			break;
		case DB_VIEWPORT		 :
			pEnt = createOdDbViewport(pEntity);
			break;
		case DB_WIPEOUT			 :
			pEnt = createOdDbWipeout(pEntity, ownerId);
			break;
		case DB_XLINE			 :
			pEnt = createOdDbXline(pEntity);
			break;
		case DB_ARCALIGNEDTEXT	 :
			pEnt = createOdDbArcAlignedText(pEntity, ownerId);
			break;
		case DB_RTEXT	 :
			pEnt = createOdDbRText(pEntity);
			break;
//		case DB_ACTABLE			 :				// New to AutoCAD 2005. Need to add to Icad database.
//			pEnt = createOdDbTable(pEntity);	// Note that there is already a DB_TABLE in icad.
//			break;
		default:
			ASSERT(!"Unknown entity");
			break;
	}

	// Make sure the ouput handle is the same as the pEntity's handle.
	if (!pEnt.isNull())
	{
		OdDbObjectId id;
		try
		{
			id = ReadWriteUtils::setHandle(m_pOdDatabase, pEntity, ownerId, pEnt);

		OdDbBlockTableRecordPtr ownerPtr = ownerId.safeOpenObject(OdDb::kForWrite);
		OdDbObjectId idAfter = ownerPtr->appendOdDbEntity(pEnt);
		ASSERT(id == idAfter);

		finalize(pEntity, pEnt);
		pEnt->recordGraphicsModified(false);
		}
		catch (OdError& err)
		{
			OdError err1 = err; // Shut the compiler up.
#ifdef _DEBUG_MHB
			OdString msg = m_pExServices->getErrorDescription(err.code()).c_str();
			ASSERT(!"DWGdirectWriter create file error: ");
#endif
			if (!id.isNull())
				id.safeOpenObject(OdDb::kForWrite)->erase();
		}
		return id;
	}

	return OdDbObjectId();
}

OdDbObjectId DWGdirectConverter::createOdDbObject(OdDbObjectId ownerId, db_handitem* pHanditem, OdString recordName)
{
	ASSERT(pHanditem != NULL);
	ASSERT(!ownerId.isNull());

	OdDbObjectPtr pOdDbObject;
	switch(pHanditem->ret_type())
	{
		case DB_GROUP				:
			pOdDbObject = createOdDbGroup(ownerId, pHanditem);
			break;
		case DB_LAYOUT			:
			pOdDbObject = createOdDbLayout(ownerId, pHanditem);
			break;
		case DB_MLINESTYLE		:
			pOdDbObject = createOdDbMlStyle(ownerId, pHanditem);
			break;
		case DB_IMAGEDEF		:
			pOdDbObject = createOdDbRasterImageDef(ownerId, pHanditem);
			break;
		case DB_PLOTSETTINGS	:
			pOdDbObject = createOdDbPlotSettings(ownerId, pHanditem);
			break;
		case DB_DIMASSOC		:
			pOdDbObject = createOdDbDimAssoc(ownerId, pHanditem);
			break;
		case DB_DICTIONARYVAR		:
			pOdDbObject = createOdDbDictionaryVar(ownerId, pHanditem);
			break;
		case DB_ACAD_PROXY_OBJECT		 :
			pOdDbObject = createOdDbProxyObject(ownerId, pHanditem);
			break;
		case DB_XRECORD			:
			pOdDbObject = createOdDbXRecord(ownerId, pHanditem);
			break;
		case DB_SPATIAL_FILTER	:
			pOdDbObject = createOdDbSpatialFilter(ownerId, pHanditem);
			break;
		//case DB_LAYER_FILTER	:	// Doesn't exist yet. Need to update db_table to take a
		//							//   an extension dictionary.
		//	OutputDebugString("Export of DB_LAYER_FILTER need work.\n");
		//	//pOdDbObject = createOdDbLayerfilter(ownerId, pHanditem);
		//	break;
		case DB_WIPEOUTVARIABLES:
			{
				OdDbWipeoutVariablesPtr pWipeVars = OdDbWipeoutVariables::openWipeoutVariables(m_pOdDatabase, OdDb::kForWrite);
				int showFrame;
				pHanditem->get_70(&showFrame);
				pWipeVars->setShowFrame((showFrame == 1) ? true : false);
			}
			break;
		case DB_VBAPROJECT:
			pOdDbObject = createOdDbVBAproject(ownerId, pHanditem);
			break;
		case DB_DICTIONARYWDFLT:
				OutputDebugString("Export of DB_DICTIONARYWDFLT need work.\n");
			break;
		case DB_IDBUFFER:
				OutputDebugString("Export of DB_IDBUFFER need work.\n");
				pOdDbObject = createOdDbIDBuffer(ownerId, pHanditem);
			break;
		case DB_IMAGEDEF_REACTOR:
				// Not processed. This is handled by createOdDbRasterImage()
			break;
		case DB_RASTERVARIABLES:
				pOdDbObject = createOdDbRasterVariables(ownerId, pHanditem);
			break;
		case DB_SPATIAL_INDEX:
				OutputDebugString("Export of DB_SPATIAL_INDEX need work.\n");
			break;
		case DB_SORTENTSTABLE:
				pOdDbObject = createOdDbSortEnts(ownerId, pHanditem);
			break;
		case DB_LAYER_INDEX:
				//OutputDebugString("Export of DB_LAYER_INDEX need work.\n");
				//pOdDbObject = createOdDbLayerIndex(ownerId, pHanditem, recordName);
			break;
		case DB_OBJECT_PTR:
			break;
		case DB_PLACEHOLDER			:
				pOdDbObject = createOdDbPlaceholder(ownerId, pHanditem);
			break;
		case DB_DIMSTYLETABREC    :
			pOdDbObject = createOdDbDimStyle(ownerId, pHanditem);
			break;
		case DB_STYLETABREC    :
			pOdDbObject = createOdDbTextStyle(ownerId, pHanditem);
			break;
		default:
			ASSERT(!"Unknown object");
			// We'll treat it like a proxy ???
			pOdDbObject = createOdDbProxyObject(ownerId, pHanditem);
			break;
	}

	if (pOdDbObject.isNull())
	{
		return OdDbObjectId();
	}

	return pOdDbObject->id();
}

OdDbObjectId DWGdirectConverter::updateOdDbObject(OdDbObjectId ownerId, db_handitem* pObject, OdDbObjectPtr pExistingObject)
{
	ASSERT(pObject != NULL);

	OdDbObjectPtr pOdDbObject;
	switch(pObject->ret_type())
	{
		case DB_LAYOUT			 :
			ASSERT(!ownerId.isNull());
			pOdDbObject = updateOdDbLayout(ownerId, pObject, pExistingObject);
			break;
		case DB_MLINESTYLE			 :
			ASSERT(!ownerId.isNull());
			pOdDbObject = updateOdDbMlStyle(ownerId, pObject, pExistingObject);
			break;
		case DB_DIMSTYLETABREC    :
			ASSERT(!ownerId.isNull());
			pOdDbObject = updateOdDbDimStyle(ownerId, pObject, pExistingObject);
			break;
		case DB_STYLETABREC    :
			ASSERT(!ownerId.isNull());
			pOdDbObject = updateOdDbTextStyle(ownerId, pObject, pExistingObject);
			break;
		default:
			ASSERT(!"Unknown object");
			break;
	}

	if (!pOdDbObject.isNull())
		return pOdDbObject->id();
	else
		return OdDbObjectId();
}


OdDbEntityPtr DWGdirectConverter::createOdDbLine(db_handitem* pEntity)
{
	ASSERT(pEntity != NULL && pEntity->ret_type() == DB_LINE);

	db_line* pline = (db_line*)pEntity;
	ASSERT(pline != NULL);

	OdDbLinePtr pOdDbLine = OdDbLine::createObject();
	ASSERT(!pOdDbLine.isNull());
	((OdDbEntityPtr)pOdDbLine)->setDatabaseDefaults(m_pOdDatabase);

	sds_point point;
	pline->get_10(point);
	pOdDbLine->setStartPoint(OdGePoint3d(point[0], point[1], point[2]));

	pline->get_11(point);
	pOdDbLine->setEndPoint(OdGePoint3d(point[0], point[1], point[2]));

	pline->get_210(point);
	pOdDbLine->setNormal(OdGeVector3d(point[0], point[1], point[2]));


	double doubleVal;
	pline->get_39(&doubleVal);
	pOdDbLine->setThickness(doubleVal);

	int intVal;
	pline->get_62(&intVal);
	pOdDbLine->setColorIndex(intVal);

	return pOdDbLine;
}

OdDbEntityPtr DWGdirectConverter::createOdDbPoint(db_handitem* pEntity)
{
	ASSERT(pEntity != NULL && pEntity->ret_type() == DB_POINT);

	db_point* pPoint = (db_point*)pEntity;
	ASSERT(pPoint != NULL);

	OdDbPointPtr pOdDbPoint = OdDbPoint::createObject();
	ASSERT(!pOdDbPoint.isNull());
	((OdDbEntityPtr)pOdDbPoint)->setDatabaseDefaults(m_pOdDatabase);

	sds_point point;
	pPoint->get_10(point);
	pOdDbPoint->setPosition(OdGePoint3d(point[0], point[1], point[2]));

	pPoint->get_210(point);
	pOdDbPoint->setNormal(OdGeVector3d(point[0], point[1], point[2]));

	double doubleVal;
	pPoint->get_50(&doubleVal);
	pOdDbPoint->setEcsRotation(doubleVal);

	pPoint->get_39(&doubleVal);
	pOdDbPoint->setThickness(doubleVal);

	int intVal;
	pPoint->get_62(&intVal);
	pOdDbPoint->setColorIndex(intVal);

	return pOdDbPoint;
}

OdDbEntityPtr DWGdirectConverter::createOdDbCircle(db_handitem* pEntity)
{
	ASSERT(pEntity != NULL && pEntity->ret_type() == DB_CIRCLE);

	db_circle* pCircle = (db_circle*)pEntity;
	OdDbCirclePtr pOdDbCircle = OdDbCircle::createObject();
	ASSERT(!pOdDbCircle.isNull());
	((OdDbEntityPtr)pOdDbCircle)->setDatabaseDefaults(m_pOdDatabase);

	sds_point point;

	pCircle->get_210(point);
	OdGeVector3d normal(point[0], point[1], point[2]);
	pOdDbCircle->setNormal(OdGeVector3d(point[0], point[1], point[2]));

	pCircle->get_10(point);
	if (normal != OdGeVector3d::kZAxis)
	{
		OdGePoint3d point3d = OdGePoint3d(point[0], point[1], point[2]);
		point3d.transformBy(OdGeMatrix3d::planeToWorld(normal));
		point[0] = point3d.x;
		point[1] = point3d.y;
		point[2] = point3d.z;
	}
	pOdDbCircle->setCenter(OdGePoint3d(point[0], point[1], point[2]));

	double doubleVal;
	pCircle->get_39(&doubleVal);
	pOdDbCircle->setThickness(doubleVal);

	pCircle->get_40(&doubleVal);
	pOdDbCircle->setRadius(doubleVal);

	
	int intVal;
	pCircle->get_62(&intVal);
	pOdDbCircle->setColorIndex((OdUInt16)intVal);

	return pOdDbCircle;
}

OdDbEntityPtr DWGdirectConverter::createOdDbProxyEntity(db_handitem* pEntity, OdDbObjectId ownerId)
{
	ASSERT(pEntity != NULL && pEntity->ret_type() == DB_ACAD_PROXY_ENTITY);

	db_acad_proxy_entity* pProxy = (db_acad_proxy_entity*)pEntity;
	void* pOdClass = pProxy->getOdDbClass();
	if (pOdClass == NULL)
		return OdDbProxyEntityPtr();

	OdRxClassPtr pRegClass;
	pRegClass.attach((OdRxClass*)pOdClass);

	OdDbProxyEntityPtr pOdDbProxy = pRegClass->create(); 
	ASSERT(!pOdDbProxy.isNull());
	((OdDbEntityPtr)pOdDbProxy)->setDatabaseDefaults(m_pOdDatabase);

	CDxfFiler tmpFiler(m_pOdDatabase);
	tmpFiler.initResBuf(ReadWriteUtils::getOdResBufPtrFromSdsResBufPtr(m_pOdDatabase, pProxy->getResbufChain()));

	ASSERT(!ownerId.isNull());
	if (!ownerId.isNull())
	{
		OdDbObjectId id = ReadWriteUtils::setHandle(m_pOdDatabase, pEntity, ownerId, pOdDbProxy);

		OdDbBlockTableRecordPtr ownerPtr = ownerId.safeOpenObject(OdDb::kForWrite);
		ownerPtr->appendOdDbEntity(pOdDbProxy);

		tmpFiler.nextItem(); // Skipp DXF 5 handle value;

		pOdDbProxy->dxfInFields(&tmpFiler);
	}

	pRegClass.detach();

	appendData(pProxy, pOdDbProxy);

	return OdDbProxyEntityPtr();
}

OdDbEntityPtr DWGdirectConverter::createOdDbArc(db_handitem* pEntity)
{
	ASSERT(pEntity != NULL && pEntity->ret_type() == DB_ARC);

	db_arc* pArc = (db_arc*)pEntity;
	OdDbArcPtr pOdDbArc = OdDbArc::createObject();
	ASSERT(!pOdDbArc.isNull());
	((OdDbEntityPtr)pOdDbArc)->setDatabaseDefaults(m_pOdDatabase);

	sds_point point;
	pArc->get_210(point);
	OdGeVector3d normal(point[0], point[1], point[2]);
	pOdDbArc->setNormal(OdGeVector3d(point[0], point[1], point[2]));

	pArc->get_10(point);
	if (normal != OdGeVector3d::kZAxis)
	{
		OdGePoint3d point3d = OdGePoint3d(point[0], point[1], point[2]);
		point3d.transformBy(OdGeMatrix3d::planeToWorld(normal));
		point[0] = point3d.x;
		point[1] = point3d.y;
		point[2] = point3d.z;
	}
	pOdDbArc->setCenter(OdGePoint3d(point[0], point[1], point[2]));

	double doubleVal;
	pArc->get_40(&doubleVal);
	pOdDbArc->setRadius(doubleVal);

	pArc->get_39(&doubleVal);
	pOdDbArc->setThickness(doubleVal);

	int intVal;
	pArc->get_62(&intVal);
	pOdDbArc->setColorIndex((OdUInt16)intVal);

	pArc->get_50(&doubleVal);
	pOdDbArc->setStartAngle(doubleVal);

	pArc->get_51(&doubleVal);
	pOdDbArc->setEndAngle(doubleVal);

	return pOdDbArc;
}

OdDbEntityPtr DWGdirectConverter::createOdDbTolerance(db_handitem* pEntity, OdDbObjectId ownerId)
{
	ASSERT(pEntity != NULL && pEntity->ret_type() == DB_TOLERANCE);

	db_tolerance* pTolerance = (db_tolerance*)pEntity;
	OdDbFcfPtr pOdDbTolerance = OdDbFcf::createObject();
	ASSERT(!pOdDbTolerance.isNull());
	((OdDbEntityPtr)pOdDbTolerance)->setDatabaseDefaults(m_pOdDatabase);

	char* str;
	pTolerance->get_1(&str);
	pOdDbTolerance->setText(str);

	pTolerance->get_3(&str);
	OdDbObjectId dimstyleId = ReadWriteUtils::getDimStyleIDByName(m_pOdDatabase, OdString(str));
	if (!dimstyleId.isNull())
	{
		// TODO MHB
//		pOdDbTolerance->setDimstyleData(dimstyleId);
	}

	sds_point point, normal;
	pTolerance->get_10(point);
	pOdDbTolerance->setLocation(OdGePoint3d(point[0], point[1], point[2]));

	pTolerance->get_11(point);
	pTolerance->get_210(normal);
	pOdDbTolerance->setOrientation(OdGeVector3d(normal[0], normal[1], normal[2]),
								   OdGeVector3d(point[0], point[1], point[2]));
	return pOdDbTolerance;
}

OdDbEntityPtr DWGdirectConverter::createOdDbTrace(db_handitem* pEntity)
{
	ASSERT(pEntity != NULL && pEntity->ret_type() == DB_TRACE);

	db_trace* pTrace = (db_trace*)pEntity;
	OdDbTracePtr pOdDbTrace = OdDbTrace::createObject();
	ASSERT(!pOdDbTrace.isNull());
	((OdDbEntityPtr)pOdDbTrace)->setDatabaseDefaults(m_pOdDatabase);

	sds_point point;
	pTrace->get_10(point);
	pOdDbTrace->setPointAt(0, OdGePoint3d(point[0], point[1], point[2]));

	pTrace->get_11(point);
	pOdDbTrace->setPointAt(1, OdGePoint3d(point[0], point[1], point[2]));

	pTrace->get_12(point);
	pOdDbTrace->setPointAt(2, OdGePoint3d(point[0], point[1], point[2]));

	pTrace->get_13(point);
	pOdDbTrace->setPointAt(3, OdGePoint3d(point[0], point[1], point[2]));

	pTrace->get_210(point);
	pOdDbTrace->setNormal(OdGeVector3d(point[0], point[1], point[2]));

	double doubleVal;
	pTrace->get_39(&doubleVal);
	pOdDbTrace->setThickness(doubleVal);

	int intVal;
	pTrace->get_62(&intVal);
	pOdDbTrace->setColorIndex((OdUInt16)intVal);

	return pOdDbTrace;
}

OdDbEntityPtr DWGdirectConverter::createOdDbShape(db_handitem* pEntity, OdDbObjectId ownerId)
{
	ASSERT(pEntity != NULL && pEntity->ret_type() == DB_SHAPE);

	db_shape* pShape= (db_shape*)pEntity;
	OdDbShapePtr pOdDbShape = OdDbShape::createObject();
	ASSERT(!pOdDbShape.isNull());
	((OdDbEntityPtr)pOdDbShape)->setDatabaseDefaults(m_pOdDatabase);

	char hand[17];
	db_handitem *textStyle = pShape->ret_tship();
	if (textStyle != NULL)
	{
		textStyle->RetHandle().ToAscii(hand);
		OdDbObjectId styleId = ownerId.database()->getOdDbObjectId(OdDbHandle(hand));
		ASSERT(!styleId.isNull());
		if (!styleId.isNull())
		{
			pOdDbShape->setStyleId(styleId);
		}
	}
	int shapeNum;		
	pShape->get_2(&shapeNum);
	pOdDbShape->setShapeNumber(shapeNum);

	sds_point point;
	pShape->get_10(point);
	pOdDbShape->setPosition(OdGePoint3d(point[0], point[1], point[2]));

	pShape->get_210(point);
	pOdDbShape->setNormal(OdGeVector3d(point[0], point[1], point[2]));

	double doubleVal;
	pShape->get_39(&doubleVal);
	pOdDbShape->setThickness(doubleVal);

	pShape->get_40(&doubleVal);
	pOdDbShape->setSize(doubleVal);

	pShape->get_50(&doubleVal);
	pOdDbShape->setRotation(doubleVal);

	pShape->get_51(&doubleVal);
	pOdDbShape->setOblique(doubleVal);

	int intVal;
	pShape->get_62(&intVal);
	pOdDbShape->setColorIndex((OdUInt16)intVal);

	return pOdDbShape;
}
OdDbEntityPtr DWGdirectConverter::createOdDbSolid(db_handitem* pEntity)
{
	ASSERT(pEntity != NULL && pEntity->ret_type() == DB_SOLID);

	db_solid* pSolid= (db_solid*)pEntity;
	OdDbSolidPtr pOdDbSolid = OdDbSolid::createObject();
	ASSERT(!pOdDbSolid.isNull());
	((OdDbEntityPtr)pOdDbSolid)->setDatabaseDefaults(m_pOdDatabase);

	sds_point point;
	pSolid->get_10(point);
	pOdDbSolid->setPointAt(0, OdGePoint3d(point[0], point[1], point[2]));

	pSolid->get_11(point);
	pOdDbSolid->setPointAt(1, OdGePoint3d(point[0], point[1], point[2]));

	pSolid->get_12(point);
	pOdDbSolid->setPointAt(2, OdGePoint3d(point[0], point[1], point[2]));

	pSolid->get_13(point);
	pOdDbSolid->setPointAt(3, OdGePoint3d(point[0], point[1], point[2]));

	pSolid->get_210(point);
	pOdDbSolid->setNormal(OdGeVector3d(point[0], point[1], point[2]));

	double doubleVal;
	pSolid->get_39(&doubleVal);
	pOdDbSolid->setThickness(doubleVal);

	int intVal;
	pSolid->get_62(&intVal);
	pOdDbSolid->setColorIndex((OdUInt16)intVal);

	return pOdDbSolid;
}

OdDbEntityPtr DWGdirectConverter::createOdDb3DSolid(db_handitem* pEntity)
{
	ASSERT(pEntity != NULL && pEntity->ret_type() == DB_3DSOLID);

	db_3dsolid* pSolid= (db_3dsolid*)pEntity;
	OdDb3dSolidPtr pOdDbSolid = OdDb3dSolid::createObject();
	ASSERT(!pOdDbSolid.isNull());
	((OdDbEntityPtr)pOdDbSolid)->setDatabaseDefaults(m_pOdDatabase);

	char* data;
	pSolid->get_1(&data);

	OdUInt32 dataLen = strlen(data);
	OdMemoryStreamPtr pStream = OdMemoryStream::createNew(dataLen);
	pStream->putBytes(data, dataLen);
	pStream->rewind();

	AfTypeVer acisVersion = getACISVersion();
	pOdDbSolid->acisIn(pStream, &acisVersion);

	int intVal;
	pSolid->get_62(&intVal);
	pOdDbSolid->setColorIndex((OdUInt16)intVal);

	return pOdDbSolid;
}

OdDbEntityPtr DWGdirectConverter::createOdDbRegion(db_handitem* pEntity)
{
	ASSERT(pEntity != NULL && pEntity->ret_type() == DB_REGION);

	db_region* pSolid= (db_region*)pEntity;
	OdDbRegionPtr pOdDbSolid = OdDbRegion::createObject();
	ASSERT(!pOdDbSolid.isNull());
	((OdDbEntityPtr)pOdDbSolid)->setDatabaseDefaults(m_pOdDatabase);

	char* data;
	pSolid->get_1(&data);

	OdUInt32 dataLen = strlen(data);
	OdMemoryStreamPtr pStream = OdMemoryStream::createNew(dataLen);
	pStream->putBytes(data, dataLen);
	pStream->rewind();

	AfTypeVer acisVersion = getACISVersion();
	pOdDbSolid->acisIn(pStream, &acisVersion);

	int intVal;
	pSolid->get_62(&intVal);
	pOdDbSolid->setColorIndex((OdUInt16)intVal);

	return pOdDbSolid;
}

OdDbEntityPtr DWGdirectConverter::createOdDbBody(db_handitem* pEntity)
{
	ASSERT(pEntity != NULL && pEntity->ret_type() == DB_BODY);

	db_body* pSolid= (db_body*)pEntity;
	OdDbBodyPtr pOdDbSolid = OdDbBody::createObject();
	ASSERT(!pOdDbSolid.isNull());
	((OdDbEntityPtr)pOdDbSolid)->setDatabaseDefaults(m_pOdDatabase);

	char* data;
	pSolid->get_1(&data);

	OdUInt32 dataLen = strlen(data);
	OdMemoryStreamPtr pStream = OdMemoryStream::createNew(dataLen);
	pStream->putBytes(data, dataLen);
	pStream->rewind();

	AfTypeVer acisVersion = getACISVersion();
	pOdDbSolid->acisIn(pStream, &acisVersion);

	int intVal;
	pSolid->get_62(&intVal);
	pOdDbSolid->setColorIndex((OdUInt16)intVal);

	return pOdDbSolid;
}

OdDbEntityPtr DWGdirectConverter::createOdDbFace(db_handitem* pEntity)
{
	ASSERT(pEntity != NULL && pEntity->ret_type() == DB_3DFACE);

	db_3dface* pFace = (db_3dface*)pEntity;
	OdDbFacePtr pOdDbFace = OdDbFace::createObject();
	ASSERT(!pOdDbFace.isNull());
	((OdDbEntityPtr)pOdDbFace)->setDatabaseDefaults(m_pOdDatabase);

	sds_point point;
	pFace->get_10(point);
	pOdDbFace->setVertexAt(0, OdGePoint3d(point[0], point[1], point[2]));

	pFace->get_11(point);
	pOdDbFace->setVertexAt(1, OdGePoint3d(point[0], point[1], point[2]));

	pFace->get_12(point);
	pOdDbFace->setVertexAt(2, OdGePoint3d(point[0], point[1], point[2]));

	pFace->get_13(point);
	pOdDbFace->setVertexAt(3, OdGePoint3d(point[0], point[1], point[2]));

	int flags;
	pFace->get_70(&flags);
	if ((flags & BIT1) == BIT1)
		pOdDbFace->makeEdgeInvisibleAt(0);
	if ((flags & BIT2) == BIT2)
		pOdDbFace->makeEdgeInvisibleAt(1);
	if ((flags & BIT4) == BIT4)
		pOdDbFace->makeEdgeInvisibleAt(2);
	if ((flags & BIT8) == BIT8)
		pOdDbFace->makeEdgeInvisibleAt(3);

	int intVal;
	pFace->get_62(&intVal);
	pOdDbFace->setColorIndex((OdUInt16)intVal);

	return pOdDbFace;
}

OdDbEntityPtr DWGdirectConverter::createOdDbLWPolyline(db_handitem* pEntity)
{
	ASSERT(pEntity != NULL && pEntity->ret_type() == DB_LWPOLYLINE);

	db_lwpolyline* pPline = (db_lwpolyline*)pEntity;
	OdDbPolylinePtr pOdDbPolyline = OdDbPolyline::createObject();
	ASSERT(!pOdDbPolyline.isNull());
	((OdDbEntityPtr)pOdDbPolyline)->setDatabaseDefaults(m_pOdDatabase);

	int numVerts;
	pPline->get_90(&numVerts);

	sds_point* vertices = new sds_point[numVerts * sizeof(sds_point)];
	pPline->get_10(vertices, numVerts);

	double*bulges = new double[numVerts * sizeof(double)];
	pPline->get_42array(bulges, numVerts);

	double doubleVal;
	double* startWidths = NULL;
	double* endWidths = NULL;
	bool bHasWidths = pPline->haswidths();
	if (bHasWidths == true)
	{
		startWidths = new double[numVerts * sizeof(double)];
		endWidths   = new double[numVerts * sizeof(double)];

		pPline->get_40array(startWidths, numVerts);
		pPline->get_41array(endWidths, numVerts);
	}
	else
	{
		pPline->get_43(&doubleVal);
		pOdDbPolyline->setConstantWidth(doubleVal);
	}

	for (int i = 0; i < numVerts; i++)
	{
		pOdDbPolyline->addVertexAt(i,
								   OdGePoint2d(vertices[i][0], vertices[i][1]),
								   bulges[i],
								   (bHasWidths) ? startWidths[i] : 0.0,
								   (bHasWidths) ? endWidths[i] : 0.0);
	}

	delete [] vertices;
	delete [] bulges;
	delete [] startWidths;
	delete [] endWidths;

	int flags = 0;
	pPline->get_70(&flags);
	if ((flags & BIT1) == BIT1)
		pOdDbPolyline->setClosed(true);
	if ((flags & BIT80) == BIT80)
		pOdDbPolyline->setPlinegen(true);

	sds_point point;
	pPline->get_210(point);
	pOdDbPolyline->setNormal(OdGeVector3d(point[0], point[1], point[2]));

	pPline->get_39(&doubleVal);
	pOdDbPolyline->setThickness(doubleVal);

	pPline->get_38(&doubleVal);
	pOdDbPolyline->setElevation(doubleVal);

	int intVal;
	pPline->get_62(&intVal);
	pOdDbPolyline->setColorIndex((OdUInt16)intVal);

	return pOdDbPolyline;
}

OdDbEntityPtr DWGdirectConverter::createOdDbMline(db_handitem* pEntity)
{
	ASSERT(pEntity != NULL && pEntity->ret_type() == DB_MLINE);

	db_mline* pMline = (db_mline*)pEntity;
	OdDbMlinePtr pOdDbMline = OdDbMline::createObject();
	ASSERT(!pOdDbMline.isNull());
	((OdDbEntityPtr)pOdDbMline)->setDatabaseDefaults(m_pOdDatabase);
		
	char* str;
	pMline->get_2(&str);
	pOdDbMline->setStyle(ReadWriteUtils::getMLineStyleIDByName(m_pOdDatabase, OdString(str)));

	double doubleVal;
	pMline->get_40(&doubleVal);
	pOdDbMline->setScale(doubleVal);

	int flags;
	pMline->get_71(&flags);
	
	if ((flags & BIT2) == BIT2)
		pOdDbMline->setClosedMline(true);
	if ((flags & BIT8) == BIT8)
		pOdDbMline->setSupressEndCaps(true);
	if ((flags & BIT4) == BIT4)
		pOdDbMline->setSupressStartCaps(true);

	int intVal;
	pMline->get_70(&intVal);
	pOdDbMline->setJustification(intVal);

	sds_point point;
	pMline->get_210(point);
	pOdDbMline->setNormal(OdGeVector3d(point[0], point[1], point[2]));

	int numVers, numElems;
	pMline->get_72(&numVers);	// numVerts
	pMline->get_73(&numElems);	// numElems

	sds_point* verts = new sds_point[numVers * sizeof(sds_point)]; 
	pMline->get_11(verts);

	double *segParams, *fillParams;
	OdMLSegment param;
	OdMLSegmentArray params;
	for (int i = 0; i < numVers; i++)
	{
		pOdDbMline->appendSeg(OdGePoint3d(verts[i][0], verts[i][1], verts[i][2]));
		for (int j = 0; j < numElems; j++)
		{
			int numSegParams;
			pMline->get_74(&numSegParams, i, j);
			segParams = new double[numSegParams * sizeof(double)];

			int numFillParams;
			pMline->get_75(&numFillParams, i, j);
			fillParams = new double[numFillParams * sizeof(double)];
			
			pMline->get_41(segParams, i, j);
			pMline->get_42(fillParams, i, j);

			for (int k = 0; k < numSegParams; k++)
			{
				param.m_SegParams.insert(param.m_SegParams.end(), segParams[k]);
			}

			for (k = 0; k < numFillParams; k++)
			{
				param.m_AreaFillParams.insert(param.m_AreaFillParams.end(), fillParams[k]);
			}

			params.insert(params.end(), param);

			param.m_AreaFillParams.clear();
			param.m_SegParams.clear();

			delete [] segParams;
			delete [] fillParams;
		}
		pOdDbMline->setParametersAt(i, params);

		param.m_AreaFillParams.clear();
		param.m_SegParams.clear();
		params.clear();
	}

	delete [] verts;

	return pOdDbMline;
}

OdDbEntityPtr DWGdirectConverter::createOdDbEllipse(db_handitem* pEntity)
{
	ASSERT(pEntity != NULL && pEntity->ret_type() == DB_ELLIPSE);

	db_ellipse* pEllipse= (db_ellipse*)pEntity;
	OdDbEllipsePtr pOdDbEllipse = OdDbEllipse::createObject();
	ASSERT(!pOdDbEllipse.isNull());
	((OdDbEntityPtr)pOdDbEllipse)->setDatabaseDefaults(m_pOdDatabase);

	sds_point centerPoint;
	pEllipse->get_10(centerPoint);

	sds_point majAxis;
	pEllipse->get_11(majAxis);

	double axisRatio;
	pEllipse->get_40(&axisRatio);

	double startParam, endParam;
	pEllipse->get_41(&startParam);
	pEllipse->get_42(&endParam);

	sds_point extrusion;
	pEllipse->get_210(extrusion);

	// DWGdirect throws an exception if the axisRatio is > 1.0.
	// There is a Bugzilla bug written against this, and when
	// fixed, this if should never be entered.
	if(axisRatio < 1.e-6 || axisRatio > 1.)
	{
		ASSERT(!"ERROR - AxisRatio");
		axisRatio = (1 / axisRatio);
	}

	pOdDbEllipse->set(OdGePoint3d(centerPoint[0], centerPoint[1], centerPoint[2]),
					  OdGeVector3d(extrusion[0], extrusion[1], extrusion[2]),
					  OdGeVector3d(majAxis[0], majAxis[1], majAxis[2]),
					  axisRatio, startParam, endParam);

	// The calls below are necessary, because the start and end angles of off. Seems 
	// set() has a bug in this that this works around. Need set() to get the majAxis right.
	pOdDbEllipse->setCenter(OdGePoint3d(centerPoint[0], centerPoint[1], centerPoint[2]));
	pOdDbEllipse->setRadiusRatio(axisRatio);
	pOdDbEllipse->setStartParam(startParam);
	pOdDbEllipse->setEndParam(endParam);

	int intVal;		
	pEllipse->get_62(&intVal);
	pOdDbEllipse->setColorIndex((OdUInt16)intVal);

	return pOdDbEllipse;
}

OdDbEntityPtr DWGdirectConverter::createOdDbDimension(db_handitem* pEntity)
{
	ASSERT(pEntity != NULL && pEntity->ret_type() == DB_DIMENSION);

	OdDbDimensionPtr pDim;
	db_dimension* pDimension = (db_dimension*)pEntity;

	int flags;
	pDimension->get_70(&flags);

	switch(flags & 0xF)	// Chop off high bits the rest are values.
	{
		case 0:			// Rotates, Horizontal or Vertical
			pDim = makeRotateDimension(pDimension);
		break;
		case BIT1:	// Aligned.
			pDim = makeAlignedDimension(pDimension);
		break;
		case BIT2:	// Angular
			pDim = makeAngularDimension(pDimension);
		break;
		case 3:		// Diameter
			pDim = makeDiametricDimension(pDimension);
		break;
		case BIT4:	// Radius
			pDim = makeRadialDimension(pDimension);
		break;
		case 5:		// Angular 3 point
			pDim = make3PointAngularDimension(pDimension);
		break;
		case 6:		// Ordinal
			pDim = makeOrdinateDimension(pDimension);
		break;
		default:
		{
			ASSERT(!"Unknown dimension type");
		}
	}

	if (!pDim.isNull())
	{
		db_handitem* pBlockRec = NULL;
		pDimension->get_2(&pBlockRec);
		if (pBlockRec == NULL)
		{
			return OdDbDimensionPtr();
		}
		ASSERT(pBlockRec != NULL);
		OdDbObjectId dimBlockId = getBlockId(pBlockRec);
		if (dimBlockId.isNull())
			dimBlockId = ReadWriteUtils::getBlockIDByHandle(m_pOdDatabase, pBlockRec->RetHandle());

		ASSERT(!dimBlockId.isNull());

		pDim->setDimBlockId(dimBlockId);

		char* str;
		pDimension->get_3(&str);
		OdDbObjectId dimStyleId = ReadWriteUtils::getDimStyleIDByName(m_pOdDatabase, OdString(str));
		pDim->setDimensionStyle(dimStyleId);
		pDim->setDimstyleData(dimStyleId);

		sds_point normal, textLoc;
		pDimension->get_11(textLoc);
		pDim->setTextPosition(OdGePoint3d(textLoc[0], textLoc[1], textLoc[2]));

		pDimension->get_210(normal);
		pDim->setNormal(OdGeVector3d(normal[0], normal[1], normal[2]));
		
		char* dimText;
		pDimension->get_1(&dimText);
		pDim->setDimensionText(dimText);

		int intVal;
		pDimension->get_62(&intVal);
		pDim->setColorIndex(intVal);

		pDimension->get_71(&intVal);
		pDim->setTextAttachment((OdDbMText::AttachmentPoint)intVal);

		pDimension->get_72(&intVal);
		pDim->setTextLineSpacingStyle((OdDb::LineSpacingStyle)intVal);

		double doubleVal; 
		pDimension->get_38(&doubleVal);
		pDim->setElevation(doubleVal);

		pDimension->get_41(&doubleVal);
		pDim->setTextLineSpacingFactor(doubleVal);

		pDimension->get_53(&doubleVal);
		pDim->setTextRotation(doubleVal);

		pDimension->get_51(&doubleVal);	
		pDim->setHorizontalRotation(doubleVal);		
	}

	return pDim;
}

OdDbEntityPtr DWGdirectConverter::createOdDbLeader(db_handitem* pEntity)
{
	ASSERT(pEntity != NULL && pEntity->ret_type() == DB_LEADER);

	db_leader* pLeader = (db_leader*)pEntity;
	OdDbLeaderPtr pOdDbLeader = OdDbLeader::createObject();
	ASSERT(!pOdDbLeader.isNull());
	((OdDbEntityPtr)pOdDbLeader)->setDatabaseDefaults(m_pOdDatabase);

	sds_point point;
	pLeader->get_213(point);
	pOdDbLeader->setAnnotationOffset(OdGeVector3d(point[0], point[1], point[2]));

	char* str;
	pLeader->get_3(&str);
	pOdDbLeader->setDimensionStyle(ReadWriteUtils::getDimStyleIDByName(m_pOdDatabase, OdString(str)));

	int dimTad;
	double dimGap, dimScale;
	pLeader->GetDimvars(dimGap, dimScale, dimTad);
	pOdDbLeader->setDimgap(dimGap);
	pOdDbLeader->setDimscale(dimScale);
	pOdDbLeader->setDimtad(dimTad);

	int intVal;
	pLeader->get_62(&intVal);
	pOdDbLeader->setColorIndex(intVal);

	pLeader->get_72(&intVal);

	if (intVal == 0)
		pOdDbLeader->setToStraightLeader();
	else
		pOdDbLeader->setToSplineLeader();

	int numVerts;
	pLeader->get_76(&numVerts);

	sds_point* vertices = new sds_point[numVerts * sizeof(sds_point)];
	pLeader->get_10(vertices, numVerts);

	for (int i = 0; i < numVerts; i++)
	{
		pOdDbLeader->appendVertex(OdGePoint3d(vertices[i][0], vertices[i][1], vertices[i][2]));	
	}
	delete [] vertices;

	return pOdDbLeader;
}

OdDbEntityPtr DWGdirectConverter::createOdDbSpline(db_handitem* pEntity)
{
	ASSERT(pEntity != NULL && pEntity->ret_type() == DB_SPLINE);

	db_spline* pSpline = (db_spline*)pEntity;
	OdDbSplinePtr pOdDbSpline = OdDbSpline::createObject();
	ASSERT(!pOdDbSpline.isNull());
	((OdDbEntityPtr)pOdDbSpline)->setDatabaseDefaults(m_pOdDatabase);

	int degree;
	pSpline->get_71(&degree);

	int flags;
	pSpline->get_70(&flags);
	bool rational = (flags & BIT4) == BIT4;
	bool closed   = (flags & BIT1) == BIT1;
	bool periodic = (flags & BIT2) == BIT2;

	double controlPtTol, knotTol;
	pSpline->get_42(&knotTol);
	pSpline->get_43(&controlPtTol);

	int numCtrlPts;
	pSpline->get_73(&numCtrlPts);
	sds_point* pointArray = new sds_point [sizeof(sds_point)*numCtrlPts];
	pSpline->get_10(pointArray, numCtrlPts);
	OdGePoint3dArray controlPoints(numCtrlPts);

	int i = 0;
	for (i = 0; i < numCtrlPts; i++)
	{
		controlPoints.insert(controlPoints.end(), OdGePoint3d(pointArray[i][0], pointArray[i][1], pointArray[i][2]));
	}
	delete [] pointArray;

	int numKnots;
	pSpline->get_72(&numKnots);
	double* knotArray = new double [sizeof(double)*numKnots];
	pSpline->get_40array(knotArray, numKnots);
	OdGeDoubleArray knots(numKnots);

	for (i = 0; i < numKnots; i++)
	{
		knots.insert(knots.end(), knotArray[i]);
	}
	delete [] knotArray;

	double* weightArray = new double [sizeof(double)*numCtrlPts];
	pSpline->get_41array(weightArray, numCtrlPts);
	OdGeDoubleArray weights(numCtrlPts);

	for (i = 0; i < numKnots; i++)
	{
		weights.insert(weights.end(), weightArray[i]);
	}
	delete [] weightArray;

	pOdDbSpline->setNurbsData(degree, rational, closed, periodic,
								controlPoints,
								knots, 
								weights,
								controlPtTol, knotTol);

	int numFitPts;
	pSpline->get_74(&numFitPts);
	if (numFitPts)
	{
		sds_point* pointArray = new sds_point [sizeof(sds_point)*numFitPts];
		pSpline->get_10(pointArray, numFitPts);
		OdGePoint3dArray fitPoints(numFitPts);

		pSpline->get_11(pointArray, numFitPts);

		for (i = 0; i < numFitPts; i++)
		{
			fitPoints.insert(fitPoints.end(), OdGePoint3d(pointArray[i][0], pointArray[i][1], pointArray[i][2]));
		}
		double fitTol;
		pSpline->get_44(&fitTol);

		sds_point startTan, endTan;
		pSpline->get_12(startTan);
		pSpline->get_13(endTan);

		pOdDbSpline->setFitData(fitPoints, degree, fitTol,
								OdGeVector3d(startTan[0], startTan[1], startTan[2]),
								OdGeVector3d(endTan[0], endTan[1], endTan[2])
								);
		delete [] pointArray;
	}

	int intVal;		
	pSpline->get_62(&intVal);
	pOdDbSpline->setColorIndex((OdUInt16)intVal);

	return pOdDbSpline;
}

OdDbEntityPtr DWGdirectConverter::createOdDbVertex(int type, db_handitem* pEntity)
{
	ASSERT(pEntity != NULL && pEntity->ret_type() == DB_VERTEX);

	db_vertex* pVertex = (db_vertex*)pEntity;

	int flags, color;
	pVertex->get_70(&flags);
	pVertex->get_62(&color);

	sds_point point;
	pVertex->get_10(point);

	if (type == 1)		// 3D polyface mesh vertex.
	{
		OdDbPolyFaceMeshVertexPtr pOdDbVertex = OdDbPolyFaceMeshVertex::createObject();
		ASSERT(!pOdDbVertex.isNull());
		pOdDbVertex->setDatabaseDefaults(m_pOdDatabase);

		pOdDbVertex->setPosition(OdGePoint3d(point[0], point[1], point[2]));
		pOdDbVertex->setColorIndex(color);

		return pOdDbVertex;
	}
	else if (type == 0)	// 3D polygon mesh vertex.
	{
		OdDbPolygonMeshVertexPtr pOdDbVertex = OdDbPolygonMeshVertex::createObject();
		ASSERT(!pOdDbVertex.isNull());
		pOdDbVertex->setDatabaseDefaults(m_pOdDatabase);

		pOdDbVertex->setPosition(OdGePoint3d(point[0], point[1], point[2]));
		pOdDbVertex->setColorIndex(color);

		int type = 0;
		if ((flags & BIT1) == BIT1)
			type = 2;
		if ((flags & BIT10) == BIT10)
			type = 1;

		return pOdDbVertex;
	}	
	else if (type == 2)	// 3D polyline vertex
	{
		OdDb3dPolylineVertexPtr pOdDbVertex = OdDb3dPolylineVertex::createObject(); 
		ASSERT(!pOdDbVertex.isNull());
		pOdDbVertex->setDatabaseDefaults(m_pOdDatabase);

		pOdDbVertex->setPosition(OdGePoint3d(point[0], point[1], point[2]));
		pOdDbVertex->setColorIndex(color);

		int type = 0;
		if ((flags & BIT1) == BIT1)
			type = 2;
		if ((flags & BIT10) == BIT10)
			type = 1;

		pOdDbVertex->setVertexType((OdDb::Vertex3dType)type);

		return pOdDbVertex;
	}
	else								// 2d vertex
	{
		OdDb2dVertexPtr pOdDbVertex = OdDb2dVertex::createObject(); 
		ASSERT(!pOdDbVertex.isNull());
		pOdDbVertex->setDatabaseDefaults(m_pOdDatabase);

		pOdDbVertex->setPosition(OdGePoint3d(point[0], point[1], point[2]));
		pOdDbVertex->setColorIndex(color);

		double doubleVal;
		pVertex->get_40(&doubleVal);
		pOdDbVertex->setStartWidth(doubleVal);

		pVertex->get_41(&doubleVal);
		pOdDbVertex->setEndWidth(doubleVal);

		pVertex->get_42(&doubleVal);
		pOdDbVertex->setBulge(doubleVal);

		pVertex->get_50(&doubleVal);
		pOdDbVertex->setTangent(doubleVal);

		int type = 0;
		if ((flags & BIT1) == BIT1)
			type = 3;
		if ((flags & BIT8) == BIT8)
			type = 2;
		if ((flags & BIT10) == BIT10)
			type = 1;

		pOdDbVertex->setVertexType((OdDb::Vertex2dType)type);

		if ((flags & BIT2) == BIT2)
			pOdDbVertex->useTangent();

		return pOdDbVertex;
	}
	
	ASSERT(!"Should never reach here");
	OdDbVertexPtr emptyVert;
	return emptyVert;
}

OdDbEntityPtr DWGdirectConverter::createOdDbPolyline(db_handitem* pEntity, OdDbObjectId ownerId)
{
	ASSERT(pEntity != NULL && pEntity->ret_type() == DB_POLYLINE);

	db_polyline* pPline = (db_polyline*)pEntity;

	int flags;
	pPline->get_70(&flags);
	bool bSplinefitVertsAdded = ((flags & BIT4) == BIT4);
	bool bPlineIsPolyfaceMesh = ((flags & BIT40) == BIT40);
	bool bPlineIs3D = ((flags & BIT8) == BIT8);
	bool bPlineIs3DPolygonMesh = ((flags & BIT10) == BIT10);

	if (bPlineIs3DPolygonMesh == true)
	{
		makePolygonMesh(pEntity, ownerId);
	}
	else if (bPlineIsPolyfaceMesh == true)
	{
		makePolyFaceMesh(pEntity, ownerId);
	}
	else if (bPlineIs3D == true)
	{
		make3dPolyline(pEntity, ownerId);
	}
	else if (bSplinefitVertsAdded = true)
	{
		makeSplineFitPolyline(pEntity, ownerId);
	}
	else // 2dPolyline handled in createOdDbLWPolyline()
	{
		ASSERT(!"Should never reach here");
	}

	return OdDbPolylinePtr();
}

OdDbEntityPtr DWGdirectConverter::createOdDbRay(db_handitem* pEntity)
{
	ASSERT(pEntity != NULL && pEntity->ret_type() == DB_RAY);

	db_ray* pRay = (db_ray*)pEntity;
	OdDbRayPtr pOdDbRay = OdDbRay::createObject();
	ASSERT(!pOdDbRay.isNull());
	((OdDbEntityPtr)pOdDbRay)->setDatabaseDefaults(m_pOdDatabase);

	sds_point point;

	pRay->get_10(point);
	pOdDbRay->setBasePoint(OdGePoint3d(point[0], point[1], point[2]));

	pRay->get_11(point);
	pOdDbRay->setUnitDir(OdGeVector3d(point[0], point[1], point[2]));

	int color;
	pRay->get_62(&color);
	pOdDbRay->setColorIndex((OdUInt16)color);

	return pOdDbRay;
}

OdDbEntityPtr DWGdirectConverter::createOdDbXline(db_handitem* pEntity)
{
	ASSERT(pEntity != NULL && pEntity->ret_type() == DB_XLINE);

	db_xline* pXline = (db_xline*)pEntity;
	OdDbXlinePtr pOdDbXline = OdDbXline::createObject();
	ASSERT(!pOdDbXline.isNull());
	((OdDbEntityPtr)pOdDbXline)->setDatabaseDefaults(m_pOdDatabase);

	sds_point point;

	pXline->get_10(point);
	pOdDbXline->setBasePoint(OdGePoint3d(point[0], point[1], point[2]));

	pXline->get_11(point);
	pOdDbXline->setUnitDir(OdGeVector3d(point[0], point[1], point[2]));

	int color;
	pXline->get_62(&color);
	pOdDbXline->setColorIndex((OdUInt16)color);

	return pOdDbXline;
}

OdDbEntityPtr DWGdirectConverter::createOdDbArcAlignedText(db_handitem* pEntity, OdDbObjectId ownerId)
{
	ASSERT(pEntity != NULL && pEntity->ret_type() == DB_ARCALIGNEDTEXT);

	db_arcalignedtext* pText = (db_arcalignedtext*)pEntity;
	OdDbArcAlignedTextPtr pOdDbText = OdDbArcAlignedText::createObject();
	ASSERT(!pOdDbText.isNull());
	((OdDbEntityPtr)pOdDbText)->setDatabaseDefaults(m_pOdDatabase);

	pOdDbText->setAlignment((OdArcTextAlignment)pText->ret_alignment());

	char hand[17];
	pText->ret_arcobjhandle().ToAscii(hand);
	OdDbObjectId arcId = m_pOdDatabase->getOdDbObjectId(OdDbHandle(hand));
	OdDbObjectPtr x = arcId.safeOpenObject();
	ASSERT(!arcId.isNull());
	if (!arcId.isNull())
	{
		OdDbObjectPtr pArc = arcId.safeOpenObject(OdDb::kForWrite);
		pOdDbText->setArcId(pArc->objectId());
	}

	char fontName[512];
	pText->get_bigfontname(fontName);
	pOdDbText->setBigFontFileName(fontName);
	pText->get_fontname(fontName);
	pOdDbText->setFileName(fontName);

	sds_point point;
	
	pText->get_pt0(point);
	pOdDbText->setCenter(OdGePoint3d(point[0], point[1], point[2]));

	double doubleVal;
	doubleVal = pText->ret_charspacing();
	pOdDbText->setCharSpacing(doubleVal);

	doubleVal = pText->ret_startangle();
	pOdDbText->setEndAngle(doubleVal);

	pOdDbText->setLeftOffset(pText->ret_leftoffset());
	pOdDbText->setRightOffset(pText->ret_rightoffset());
	pOdDbText->setOffsetFromArc(pText->ret_offsetfromarc());
	pOdDbText->setRadius(pText->ret_radius());
	pOdDbText->setStartAngle(pText->ret_startangle());
	pOdDbText->setTextDirection((OdArcTextDirection)pText->ret_direction());
	pOdDbText->setTextPosition((OdArcTextPosition)pText->ret_side());
	pOdDbText->setTextSize(pText->ret_height());
	pOdDbText->setUnderlined((pText->ret_underline() == 1) ? true : false);
	pOdDbText->setWizardFlag((pText->ret_wizardflag() == 1) ? true : false);
	pOdDbText->setXScale(pText->ret_widthfactor());

	pOdDbText->setFont(  fontName,
						(pText->ret_bold() == 1) ? true : false,
						(pText->ret_italic() == 1) ? true : false,
						 pText->ret_charset(),
						 pText->ret_pitch()
						);

	char text[1024];
	pText->get_text(text);
	pOdDbText->setTextString(text);

	pText->ret_textstyleobjhandle().ToAscii(hand);
	OdDbObjectId styleId = m_pOdDatabase->getOdDbObjectId(OdDbHandle(hand));
	ASSERT(!styleId.isNull());
	if (!styleId.isNull())
	{
		pOdDbText->setTextStyle(styleId);
	}

	pText->get_210(point);
	pOdDbText->setNormal(OdGeVector3d(point[0], point[1], point[2]));
	pOdDbText->setColorIndex((OdUInt16)pText->ret_color());

	return pOdDbText;
}

OdDbEntityPtr DWGdirectConverter::createOdDbRText(db_handitem* pEntity)
{
	ASSERT(pEntity != NULL && pEntity->ret_type() == DB_RTEXT);

	db_rtext* pText = (db_rtext*)pEntity;
	OdRTextPtr pOdDbRText = RText::createObject();
	ASSERT(!pOdDbRText.isNull());
	((OdDbEntityPtr)pOdDbRText)->setDatabaseDefaults(m_pOdDatabase);

	sds_point point;
	pText->get_pt0(point);
	pOdDbRText->setPoint(OdGePoint3d(point[0], point[1], point[2]));

	char content[1024];
	pText->get_contents(content);
	pOdDbRText->setStringContents(OdString(content));

	pText->get_210(point);
	pOdDbRText->setNormal(OdGeVector3d(point[0], point[1], point[2]));

	pOdDbRText->setHeight(pText->ret_height());
	pOdDbRText->setRotAngle(pText->ret_rotang());
	pOdDbRText->enableMTextSequences((pText->ret_typeflag() == 1) ? true : false);

	db_objhandle styleHandle = pText->ret_textstyleobjhandle();
	if (styleHandle != NULL)
	{
		OdDbObjectId styleId = ReadWriteUtils::getTextStyleIDByName(m_pOdDatabase, ReadWriteUtils::getStyleNameByHandle(m_pICdwg, styleHandle));
		pOdDbRText->setTextStyle(styleId);
	}
	return pOdDbRText;
}

OdDbEntityPtr DWGdirectConverter::createOdDbText(db_handitem* pEntity)
{
	ASSERT(pEntity != NULL && pEntity->ret_type() == DB_TEXT);

	db_text* pText = (db_text*)pEntity;
	OdDbTextPtr pOdDbText = OdDbText::createObject();
	ASSERT(!pOdDbText.isNull());
	((OdDbEntityPtr)pOdDbText)->setDatabaseDefaults(m_pOdDatabase);

	sds_point point;
	pText->get_10(point);
	pOdDbText->setPosition(OdGePoint3d(point[0], point[1], point[2]));

	pText->get_11(point);
	pOdDbText->setAlignmentPoint(OdGePoint3d(point[0], point[1], point[2]));

	pText->get_210(point);
	pOdDbText->setNormal(OdGeVector3d(point[0], point[1], point[2]));

	double doubleVal;
	pText->get_39(&doubleVal);
	pOdDbText->setThickness(doubleVal);

	pText->get_40(&doubleVal);
	pOdDbText->setHeight(doubleVal);

	pText->get_41(&doubleVal);
	pOdDbText->setWidthFactor(doubleVal);

	pText->get_50(&doubleVal);
	pOdDbText->setRotation(doubleVal);

	pText->get_51(&doubleVal);
	pOdDbText->setOblique(doubleVal);

	int color;
	pText->get_62(&color);
	pOdDbText->setColorIndex((OdUInt16)color);

	int intVal;
	pText->get_72(&intVal);
	pOdDbText->setHorizontalMode((OdDb::TextHorzMode)intVal);

	pText->get_73(&intVal);
	pOdDbText->setVerticalMode((OdDb::TextVertMode)intVal);

	char* textString;
	pText->get_1(&textString);
	pOdDbText->setTextString(textString);

	db_handitem* style;
	pText->get_7(&style);
	ASSERT(style);
	if (style != NULL)
	{
		OdDbObjectId styleId = ReadWriteUtils::getTextStyleIDByName(m_pOdDatabase, ReadWriteUtils::getStyleNameByHandle(m_pICdwg, style->RetHandle()));
		pOdDbText->setTextStyle(styleId);
	}
	return pOdDbText;
}

OdDbEntityPtr DWGdirectConverter::createOdDbMText(db_handitem* pEntity)
{
	ASSERT(pEntity != NULL && pEntity->ret_type() == DB_MTEXT);

	db_mtext* pText = (db_mtext*)pEntity;
	OdDbMTextPtr pOdDbText = OdDbMText::createObject();
	ASSERT(!pOdDbText.isNull());
	((OdDbEntityPtr)pOdDbText)->setDatabaseDefaults(m_pOdDatabase);

	sds_point point;
	pText->get_10(point);
	pOdDbText->setLocation(OdGePoint3d(point[0], point[1], point[2]));

	pText->get_11(point);
	pOdDbText->setDirection(OdGeVector3d(point[0], point[1], point[2]));

	pText->get_210(point);
	pOdDbText->setNormal(OdGeVector3d(point[0], point[1], point[2]));

	double doubleVal;

	pText->get_40(&doubleVal);
	pOdDbText->setTextHeight(doubleVal);

	pText->get_41(&doubleVal);
	pOdDbText->setWidth(doubleVal);

	pText->get_50(&doubleVal, m_pICdwg);
	pOdDbText->setRotation(doubleVal);

	pText->get_44(&doubleVal);
	pOdDbText->setLineSpacingFactor(doubleVal);

	int color;
	pText->get_62(&color);
	pOdDbText->setColorIndex((OdUInt16)color);

	int intVal;
	pText->get_71(&intVal);
	pOdDbText->setAttachment((OdDbMText::AttachmentPoint)intVal);
	
	pText->get_72(&intVal);
	pOdDbText->setFlowDirection((OdDbMText::FlowDirection)intVal);

	pText->get_73(&intVal);
	pOdDbText->setLineSpacingStyle((OdDb::LineSpacingStyle)intVal);

	// TODO - MHB ??
	//pOdDbText->setVerticalMode((OdDb::TextVertMode));

	char* textString;
	pText->get_1(&textString);
	pOdDbText->setContents(textString);

	db_handitem* style;
	pText->get_7(&style);
	OdDbObjectId styleId = ReadWriteUtils::getTextStyleIDByName(m_pOdDatabase, ReadWriteUtils::getStyleNameByHandle(m_pICdwg, style->RetHandle()));
	pOdDbText->setTextStyle(styleId);

	return pOdDbText;
}

OdDbEntityPtr DWGdirectConverter::createOdDbOle2Frame(db_handitem* pEntity)
{
	ASSERT(pEntity != NULL && pEntity->ret_type() == DB_OLE2FRAME);

	db_ole2frame* pFrame = (db_ole2frame*)pEntity;
	OdDbOle2FramePtr pOdDbFrame = OdDbOle2Frame::createObject();
	ASSERT(!pOdDbFrame.isNull());
	((OdDbEntityPtr)pOdDbFrame)->setDatabaseDefaults(m_pOdDatabase);

	int unk1 = (char)pFrame->ret_unk1();
	pOdDbFrame->setUnknown1((OdUInt8)unk1);

	int unk2 = (char)pFrame->ret_unk2();
	pOdDbFrame->setUnknown2((OdUInt8)unk2);

	pOdDbFrame->setOutputQuality((OdUInt8)pFrame->get_outputquality());
	pOdDbFrame->setAspect(pFrame->get_aspect());
	pOdDbFrame->setColorsNum(pFrame->get_colorsnum());
	pOdDbFrame->setCounter(pFrame->get_counter());
	pOdDbFrame->setModelFlag(pFrame->get_modelflag());
	pOdDbFrame->setPixelHeight(pFrame->get_pixelheight());
	pOdDbFrame->setPixelWidth(pFrame->get_pixelwidth());
	pOdDbFrame->setUnknown6(pFrame->get_unknown6());
	pOdDbFrame->setUnknown7(pFrame->get_unknown7());

	sds_point ul, lr, ur, ll;
	pFrame->get_10(ul);
	pFrame->get_11(lr);
	pFrame->get_ur(ur);
	pFrame->get_ll(ll);

	OdRectangle3d positionRect;
	positionRect.upLeft	  = OdGePoint3d(ul[0], ul[1], ul[2]);
	positionRect.upRight  = OdGePoint3d(ur[0], ur[1], ur[2]);
	positionRect.lowLeft  = OdGePoint3d(ll[0], ll[1], ll[2]);
	positionRect.lowRight = OdGePoint3d(lr[0], lr[1], lr[2]);

	pOdDbFrame->setPosition(positionRect);

	int datasz;
	pFrame->get_90(&datasz);

	char* data;
	pFrame->get_310(&data);

	OdByteDataPtr pFrameData = pOdDbFrame->compDocFile(true);
	pFrameData->writeAt(0, (const OdUInt8*)data, (OdUInt32)datasz);

	int color;
	pFrame->get_62(&color);
	pOdDbFrame->setColorIndex((OdUInt16)color);

	// TODO - MHB
	// Should add a couple of char* variables to the
	// icad db_ole2frame class to hold the Raster image preview
	// if any and the Enhanced metafile preview if any. DWGdirect
	// can get at them as streams which we can make char* s out of.
	//pFrame->getRasterImage();
	//pFrame->getEnhancedMetafile();

	return pOdDbFrame;
}

OdDbEntityPtr DWGdirectConverter::createOdDbHatch(db_handitem* pEntity, OdDbObjectId ownerId)
{
	ASSERT(pEntity != NULL && pEntity->ret_type() == DB_HATCH);

	db_hatch* pHatch = (db_hatch*)pEntity;
	OdDbHatchPtr pOdDbHatch = OdDbHatch::createObject();
	ASSERT(!pOdDbHatch.isNull());
	((OdDbEntityPtr)pOdDbHatch)->setDatabaseDefaults(m_pOdDatabase);

	// Because of the way hatches can be dependancies, like the boundary
	// object being associative to it, we need to add the hatch to the data
	// base here. 
	OdDbObjectId idBefore = ReadWriteUtils::setHandle(m_pOdDatabase, pEntity, ownerId, pOdDbHatch);

	OdDbBlockTableRecordPtr ownerPtr = ownerId.safeOpenObject(OdDb::kForWrite);
	OdDbObjectId idAfter = ownerPtr->appendOdDbEntity(pOdDbHatch);
	ASSERT(idBefore == idAfter);

	finalize(pEntity, pOdDbHatch);

	int intVal;
	pHatch->get_62(&intVal);
	pOdDbHatch->setColorIndex(intVal);
		
	pHatch->get_75(&intVal);
	pOdDbHatch->setHatchStyle((OdDbHatch::HatchStyle)intVal);

	sds_point point;
	pHatch->get_10(point);
	pOdDbHatch->setElevation(point[2]);

	pHatch->get_210(point);
	pOdDbHatch->setNormal(OdGeVector3d(point[0], point[1], point[2]));

	int hatchType;
	pHatch->get_76(&hatchType);

	double angle;
	pHatch->get_52(&angle);
	int doubleHatch;
	pHatch->get_77(&doubleHatch);
	double scale;
	pHatch->get_41(&scale);
	int solidFill;
	pHatch->get_70(&solidFill);

	if (!solidFill && hatchType == OdDbHatch::kUserDefined)
		pOdDbHatch->setPatternDouble((doubleHatch == 1) ? true : false);

	if (!solidFill && hatchType != OdDbHatch::kUserDefined)
	{
		try
		{
			pOdDbHatch->setPatternScale(scale);
			pOdDbHatch->setPatternAngle(angle);
		}
		catch (OdError& err)
		{
			OdError err1 = err; // Shut the compiler up.
#ifdef _DEBUG_MHB
			OdString msg = m_pExServices->getErrorDescription(err.code()).c_str();
			ASSERT(!"DWGdirectConverter::createOdDbHatch() error: ");
#endif
		}

	}

	// TODO MHB ? 
	// pOdDbHatch->setPatternSpace(double space);

	int seedCount;
	pHatch->get_98(&seedCount);
	pHatch->get_10(point);
	pOdDbHatch->appendSeedPoint(OdGePoint2d(point[0], point[1]));

	if (seedCount > 1)
	{
		sds_point* seeds = new sds_point [seedCount * sizeof(sds_point)];
		pHatch->get_10(seeds, seedCount);

		for (int i = 0; i < seedCount; i++)
		{
			pOdDbHatch->appendSeedPoint(OdGePoint2d(seeds[i][0], seeds[i][1]));
		}

		delete [] seeds;
	}

	// Append the hatches boundary loops. 
	int loopCount;
	int flags, numEdges, numBoundarObjs;
	pHatch->get_91(&loopCount);

	for (int i = 0; i < loopCount; i++)
	{
		// flags == 2 - polyline.
		pHatch->get_path(i, &flags, &numEdges, &numBoundarObjs);

		if (!(flags & 2))
		{
			EdgeArray edgePtrs;
			for (int j = 0; j < numEdges; j++)
			{
				OdGeCurve2d* pEdge;
				switch(pHatch->ret_pathedgetype(i, j))
				{
					case DB_HATCHEDGELINE:
						pEdge = makeHatchLineEdge(pHatch, i, j);
						break;
					case DB_HATCHEDGEARC:
						pEdge = makeHatchArcEdge(pHatch, i, j);
						break;
					case DB_HATCHEDGEELLARC:
						pEdge = makeHatchEllipticArcEdge(pHatch, i, j);
						break;
					case DB_HATCHEDGESPLINE:
						pEdge = makeHatchSplineEdge(pHatch, i, j);
						break;
					default:
						ASSERT(!"UnKnown hatch edge type");
						break;
				}
				edgePtrs.insertAt(j, pEdge);
			}

			pOdDbHatch->appendLoop((OdInt32)flags, edgePtrs); 
		}
		else
		{
			OdGePoint2dArray vertices;
			OdGeDoubleArray bulges;

			makeHatchPolylineEdge(pHatch, i, vertices, bulges);

			pOdDbHatch->appendLoop((OdInt32)flags, vertices, bulges);

			vertices.clear();
			bulges.clear();
		}
	}

	// Now handle the associated entities of its boundary.
	pHatch->get_71(&intVal);
	if (intVal == 1)
	{
		int retVal = makeHatchBoundaryAssociations(pHatch, pOdDbHatch);
		pOdDbHatch->setAssociative((retVal == 1) ? true : false);
	}

	makeHatchPatternDefinition(pHatch, pOdDbHatch);

	return OdDbHatchPtr();
}

OdDbEntityPtr DWGdirectConverter::createOdDbAttDef(db_handitem* pEntity)
{
	ASSERT(pEntity != NULL && pEntity->ret_type() == DB_ATTDEF);

	db_attdef* pAttdef = (db_attdef*)pEntity;
	OdDbAttributeDefinitionPtr pOdDbAttdef = OdDbAttributeDefinition::createObject();
	ASSERT(!pOdDbAttdef.isNull());
	((OdDbEntityPtr)pOdDbAttdef)->setDatabaseDefaults(m_pOdDatabase);

	int flags;
	pAttdef->get_70(&flags);
	pOdDbAttdef->setConstant(((flags & BIT2) == BIT2) ? true : false);
	pOdDbAttdef->setInvisible(((flags & BIT1) == BIT1) ? true : false);
	pOdDbAttdef->setPreset(((flags & BIT8) == BIT8) ? true : false);
	pOdDbAttdef->setVerifiable(((flags & BIT4) == BIT4) ? true : false);

	int intVal;
	pAttdef->get_73(&intVal);
	pOdDbAttdef->setFieldLength((OdUInt16)intVal);

	char* prompt;
	pAttdef->get_3(&prompt);
	pOdDbAttdef->setPrompt(OdString(prompt));

	char* tag;
	pAttdef->get_2(&tag);
	pOdDbAttdef->setTag(OdString(tag));

	sds_point point;
	pAttdef->get_10(point);
	pOdDbAttdef->setPosition(OdGePoint3d(point[0], point[1], point[2]));

	pAttdef->get_11(point);
	pOdDbAttdef->setAlignmentPoint(OdGePoint3d(point[0], point[1], point[2]));

	pAttdef->get_210(point);
	pOdDbAttdef->setNormal(OdGeVector3d(point[0], point[1], point[2]));

	double doubleVal;
	pAttdef->get_39(&doubleVal);
	pOdDbAttdef->setThickness(doubleVal);

	pAttdef->get_40(&doubleVal);
	pOdDbAttdef->setHeight(doubleVal);

	pAttdef->get_41(&doubleVal);
	pOdDbAttdef->setWidthFactor(doubleVal);

	pAttdef->get_50(&doubleVal);
	pOdDbAttdef->setRotation(doubleVal);

	pAttdef->get_51(&doubleVal);
	pOdDbAttdef->setOblique(doubleVal);

	pAttdef->get_62(&intVal);
	pOdDbAttdef->setColorIndex(intVal);
		
	pAttdef->get_72(&intVal);
	pOdDbAttdef->setHorizontalMode((OdDb::TextHorzMode)intVal);

	pAttdef->get_73(&intVal);
	pOdDbAttdef->setVerticalMode((OdDb::TextVertMode)intVal);

	char* textString;
	pAttdef->get_1(&textString);
	pOdDbAttdef->setTextString(textString);

	db_handitem* style;
	pAttdef->get_7(&style);
	OdDbObjectId styleId = ReadWriteUtils::getTextStyleIDByName(m_pOdDatabase, ReadWriteUtils::getStyleNameByHandle(m_pICdwg, style->RetHandle()));
	pOdDbAttdef->setTextStyle(styleId);

	return pOdDbAttdef;
}

OdDbEntityPtr DWGdirectConverter::createOdDbAttribute(db_handitem* pEntity, OdDbEntity* pOwner)
{
	ASSERT(pEntity != NULL && pEntity->ret_type() == DB_ATTRIB);

	db_attrib* pAttribute = (db_attrib*)pEntity;
	OdDbAttributePtr pOdDbAttribute = OdDbAttribute::createObject();
	ASSERT(!pOdDbAttribute.isNull());
	((OdDbEntityPtr)pOdDbAttribute)->setDatabaseDefaults(m_pOdDatabase);

	OdDbObjectId idBefore = ReadWriteUtils::setHandle(m_pOdDatabase, pEntity, pOwner->id(), pOdDbAttribute);

	sds_point point;

	pAttribute->get_10(point);
	pOdDbAttribute->setPosition(OdGePoint3d(point[0], point[1], point[2]));

	pAttribute->get_11(point);
	pOdDbAttribute->setAlignmentPoint(OdGePoint3d(point[0], point[1], point[2]));

	pAttribute->get_210(point);
	pOdDbAttribute->setNormal(OdGeVector3d(point[0], point[1], point[2]));

	db_handitem* style;
	pAttribute->get_7(&style);
	OdDbObjectId styleId = ReadWriteUtils::getTextStyleIDByName(m_pOdDatabase, ReadWriteUtils::getStyleNameByHandle(m_pICdwg, style->RetHandle()));
	pOdDbAttribute->setTextStyle(styleId);

	char* str;
	pAttribute->get_1(&str);
	pOdDbAttribute->setTextString(str);

	pAttribute->get_2(&str);
	pOdDbAttribute->setTag(str);

	double doubleVal;
	pAttribute->get_39(&doubleVal);
	pOdDbAttribute->setThickness(doubleVal);

	pAttribute->get_40(&doubleVal);
	pOdDbAttribute->setHeight(doubleVal);

	pAttribute->get_41(&doubleVal);
	pOdDbAttribute->setWidthFactor(doubleVal);

	pAttribute->get_50(&doubleVal);
	pOdDbAttribute->setRotation(doubleVal);

	pAttribute->get_51(&doubleVal);
	pOdDbAttribute->setOblique(doubleVal);

	int intVal;
	pAttribute->get_62(&intVal);
	pOdDbAttribute->setColorIndex(intVal);
		
	pAttribute->get_70(&intVal);
	pOdDbAttribute->setFieldLength(((intVal & BIT1) == BIT1) ? true : false);

	pAttribute->get_72(&intVal);
	pOdDbAttribute->setHorizontalMode((OdDb::TextHorzMode)intVal);

	pAttribute->get_73(&intVal);
	pOdDbAttribute->setFieldLength(intVal);

	pAttribute->get_74(&intVal);
	pOdDbAttribute->setVerticalMode((OdDb::TextVertMode)intVal);

	finalize(pEntity, pOdDbAttribute);

	return pOdDbAttribute;
}

OdDbEntityPtr DWGdirectConverter::createOdDbRasterImage(db_handitem* pEntity)
{
	ASSERT(pEntity != NULL && pEntity->ret_type() == DB_IMAGE);

	db_image* pImage = (db_image*)pEntity;
	OdDbRasterImagePtr pOdDbImage = OdDbRasterImage::createObject();
	ASSERT(!pOdDbImage.isNull());
	((OdDbEntityPtr)pOdDbImage)->setDatabaseDefaults(m_pOdDatabase);

	int intVal;
	pImage->get_281(&intVal);
	pOdDbImage->setBrightness(intVal);

	int numClipVerts;
	pImage->get_91(&numClipVerts);
	
	OdGePoint2dArray clipPoints;
	sds_point* vertices = new sds_point[numClipVerts * sizeof(sds_point)];
	pImage->get_14(vertices, numClipVerts);

	for (int i = 0; i < numClipVerts; i++)
	{
		clipPoints.insert(clipPoints.end(), OdGePoint2d(vertices[i][0], vertices[i][1]));
	}
	pOdDbImage->setClipBoundary(clipPoints);
	delete [] vertices;

	pImage->get_280(&intVal);
	pOdDbImage->setClipped((intVal == 1) ? true : false);

	pImage->get_282(&intVal);
	pOdDbImage->setContrast(intVal);

	pImage->get_283(&intVal);
	pOdDbImage->setFade(intVal);

	int flags;
	pImage->get_70(&flags);
	pOdDbImage->setDisplayOpt((OdDbRasterImage::ImageDisplayOpt)1, ((flags & BIT1) ? true : false));
	pOdDbImage->setDisplayOpt((OdDbRasterImage::ImageDisplayOpt)2, ((flags & BIT2) ? true : false));
	pOdDbImage->setDisplayOpt((OdDbRasterImage::ImageDisplayOpt)4, ((flags & BIT4) ? true : false));
	pOdDbImage->setDisplayOpt((OdDbRasterImage::ImageDisplayOpt)8, ((flags & BIT8) ? true : false));

	char hand[17];
	db_objhandle imageDefHandle;
	pImage->get_340hand(&imageDefHandle);
	imageDefHandle.ToAscii(hand);
	OdDbObjectId imageDefId = m_pOdDatabase->getOdDbObjectId(OdDbHandle(hand));
	ASSERT(!imageDefId.isNull());

	if (!imageDefId.isNull())
		pOdDbImage->setImageDefId(imageDefId);

	sds_point size, origin, uVect, vVect;
	pImage->get_13(size);
	pImage->get_10(origin);
	pImage->get_11(uVect);
	pImage->get_12(vVect);

	pOdDbImage->setOrientation(OdGePoint3d(origin[0], origin[1], origin[2]), 
							   OdGeVector3d(uVect[0], uVect[1], uVect[2]),
                               OdGeVector3d(vVect[0], vVect[1], vVect[2]));

	return pOdDbImage;
}

OdDbEntityPtr DWGdirectConverter::createOdDbViewport(db_handitem* pEntity)
{
	ASSERT(pEntity != NULL && pEntity->ret_type() == DB_VIEWPORT);

	db_viewport* pViewport = (db_viewport*)pEntity;
	OdDbViewportPtr pOdDbViewport = OdDbViewport::createObject();
	ASSERT(!pOdDbViewport.isNull());
	((OdDbEntityPtr)pOdDbViewport)->setDatabaseDefaults(m_pOdDatabase);
	
	sds_point point;

	pViewport->get_10(point);
	pOdDbViewport->setCenterPoint(OdGePoint3d(point[0], point[1], point[2]));

	pViewport->get_12(point);
	pOdDbViewport->setViewCenter(OdGePoint2d(point[0], point[1]));

	pViewport->get_13(point);
	pOdDbViewport->setSnapBasePoint(OdGePoint2d(point[0], point[1]));

	pViewport->get_14(point);
	pOdDbViewport->setSnapIncrement(OdGeVector2d(point[0], point[1]));

	pViewport->get_15(point);
	pOdDbViewport->setGridIncrement(OdGeVector2d(point[0], point[1]));

	pViewport->get_16(point);
	pOdDbViewport->setViewDirection(OdGeVector3d(point[0], point[1], point[2]));

	pViewport->get_17(point);
	pOdDbViewport->setViewTarget(OdGePoint3d(point[0], point[1], point[2]));

	sds_point origin, xAxis, yAxis;
	pViewport->get_110(origin);
	pViewport->get_111(xAxis);
	pViewport->get_112(yAxis);
	pOdDbViewport->setUcs(OdGePoint3d(origin[0], origin[1], origin[2]),
						  OdGeVector3d(xAxis[0], xAxis[1], xAxis[2]),
						  OdGeVector3d(yAxis[0], yAxis[1], yAxis[2])
						 );

	double doubleVal;
	pViewport->get_40(&doubleVal);
	pOdDbViewport->setWidth(doubleVal);

	pViewport->get_41(&doubleVal);
	pOdDbViewport->setHeight(doubleVal);

	pViewport->get_42(&doubleVal);
	pOdDbViewport->setLensLength(doubleVal);

	pViewport->get_43(&doubleVal);
	pOdDbViewport->setFrontClipDistance(doubleVal);

	pViewport->get_44(&doubleVal);
	pOdDbViewport->setBackClipDistance(doubleVal);

	pViewport->get_45(&doubleVal);
	pOdDbViewport->setViewHeight(doubleVal);

	pViewport->get_50(&doubleVal);
	pOdDbViewport->setSnapAngle(doubleVal);

	pViewport->get_51(&doubleVal);
	pOdDbViewport->setTwistAngle(doubleVal);

	pViewport->get_146(&doubleVal);
	pOdDbViewport->setElevation(doubleVal);

	char str[512];
	pViewport->get_1(&(str[0]));
	pOdDbViewport->setPlotStyleSheet(str);

	int intVal;
	pViewport->get_62(&intVal);
	pOdDbViewport->setColorIndex(intVal);
		
	pViewport->get_72(&intVal);
	pOdDbViewport->setCircleSides(intVal);

	short shortVal;
	pViewport->get_281(&shortVal);
	pOdDbViewport->setRenderMode((OdDb::RenderMode )shortVal);

	pViewport->get_71(&shortVal);
	pOdDbViewport->setUcsPerViewport((shortVal == 1) ? true : false);

	int flags;
	pViewport->get_90(&flags);

	pOdDbViewport->setBackClipOff();
	if (flags & BIT4)
		pOdDbViewport->setBackClipOn();

	pOdDbViewport->setFastZoomOff();
	if (flags & BIT80)
		pOdDbViewport->setFastZoomOn();

	pOdDbViewport->setFrontClipAtEyeOff();
	if (flags & BIT10)
		pOdDbViewport->setFrontClipAtEyeOn();

	pOdDbViewport->setFrontClipOff();
	if (flags & BIT2)
		pOdDbViewport->setFrontClipOn();

	pOdDbViewport->setGridOff();
	if (flags & BIT200)
		pOdDbViewport->setGridOn();

	if (flags & BIT4000)
		pOdDbViewport->setLocked();

	pOdDbViewport->setNonRectClipOff();
	if (flags & BIT10000)
		pOdDbViewport->setNonRectClipOn();

	pOdDbViewport->setOn();
	if (flags & BIT20000)
		pOdDbViewport->setOff();

	pOdDbViewport->setPerspectiveOff();
	if (flags & BIT1)
		pOdDbViewport->setPerspectiveOn();

	pOdDbViewport->setSnapStandard();
	if (flags & BIT400)
		pOdDbViewport->setSnapIsometric();

	pOdDbViewport->setSnapOff();
	if (flags & BIT100)
		pOdDbViewport->setSnapOn();

	pOdDbViewport->setUcsFollowModeOff();
	if (flags & BIT8)
		pOdDbViewport->setUcsFollowModeOn();

	pOdDbViewport->setUcsIconAtOrigin();
	if (flags & BIT40)
		pOdDbViewport->setUcsIconAtCorner();

	pOdDbViewport->setUcsIconInvisible();
	if (flags & BIT20)
		pOdDbViewport->setUcsIconVisible();

	if (flags & BIT800)
		pOdDbViewport->showHiddenLines();

	// TODO MHB ICAD has a bug that prevents this from 
	// working anyway. This should be tested once it is fixed.
	// Frozen layers in viewports are not honored by icad.
	// Set viewports frozen layer ID's.
	int layerCount = pViewport->GetVpLayerFrozenLayerCount();
	if (layerCount > 0)
	{
		OdDbObjectIdArray frozenIds;
		char* layerName = NULL;
		for (int i = 0; i < layerCount; i++)
		{
			layerName = pViewport->GetVpLayerFrozenLayerName(i);
			OdDbObjectId layerId = ReadWriteUtils::getLayerIDByName(m_pOdDatabase, OdString(layerName));
			if (!layerId.isNull())
			{
				frozenIds.insert(frozenIds.end(), layerId);
			}
		}
		ASSERT(frozenIds.size());
		
		pOdDbViewport->freezeLayersInViewport(frozenIds);
	}
	// TODO MHB
	//OdDbViewport::setNonRectClipEntityId(OdDbObjectId)
	//Sets the Object ID of the entity that serves as this viewport's clipping boundary (DXF 340).

	return pOdDbViewport;
}

OdDbEntityPtr DWGdirectConverter::createOdDbWipeout(db_handitem* pEntity, OdDbObjectId ownerId)
{
	ASSERT(pEntity != NULL && pEntity->ret_type() == DB_WIPEOUT);

	db_wipeout* pWipeout = (db_wipeout*)pEntity;
	OdDbWipeoutPtr pOdDbWipeout = OdDbWipeout::createObject();
	ASSERT(!pOdDbWipeout.isNull());
	((OdDbEntityPtr)pOdDbWipeout)->setDatabaseDefaults(m_pOdDatabase);
	
	CDxfFiler tmpFiler(m_pOdDatabase);
	tmpFiler.initResBuf(ReadWriteUtils::getOdResBufPtrFromSdsResBufPtr(m_pOdDatabase, pWipeout->getResbufChain()));

	ASSERT(!ownerId.isNull());
	if (!ownerId.isNull())
	{
		OdDbBlockTableRecordPtr ownerPtr = ownerId.safeOpenObject(OdDb::kForWrite);
		ownerPtr->appendOdDbEntity(pOdDbWipeout);

		tmpFiler.nextItem(); // Skipp DXF 5 handle value;

		pOdDbWipeout->dxfInFields(&tmpFiler);

		OdDbWipeoutVariables::openWipeoutVariables(m_pOdDatabase);
	}

	return OdDbWipeoutPtr();
}

void DWGdirectConverter::saveOffDelayedEntities(db_handitem* pEntity, OdDbObjectId ownerId)
{
	ASSERT(pEntity != NULL);

	m_delayedEntities.insert(m_delayedEntities.end(), handItemObjectIDPair(pEntity, ownerId, 0));

	return;
}

OdDbEntityPtr DWGdirectConverter::createOdDbBlockReference(db_handitem* pEntity, OdDbObjectId ownerId)
{
	ASSERT(pEntity != NULL && pEntity->ret_type() == DB_INSERT);

	db_insert* pInsert = (db_insert*)pEntity;

	int rows = 1;
	int columns = 1;
	pInsert->get_70(&columns);
	pInsert->get_71(&rows);
	
	if (columns != 1 || rows != 1)
		return createOdDbMInsertBlock(pEntity, ownerId);

	OdDbBlockReferencePtr pOdDbInsert = OdDbBlockReference::createObject();
	ASSERT(!pOdDbInsert.isNull());
	((OdDbEntityPtr)pOdDbInsert)->setDatabaseDefaults(m_pOdDatabase);
	
	OdDbObjectId blockId = getBlockId(pInsert->ret_bthip());

	if (blockId.isNull())
	{
		char* str;
		pInsert->get_2(&str);

		blockId = ReadWriteUtils::getBlockIDByName(m_pOdDatabase, m_pICdwg, this, OdString(str));
	}

	ASSERT(blockId.safeOpenObject()->isKindOf(OdDbBlockTableRecord::desc()));
	pOdDbInsert->setBlockTableRecord(blockId);

	sds_point point;
	pInsert->get_210(point);
	OdGeVector3d insertNormal(point[0], point[1], point[2]);
	pOdDbInsert->setNormal(insertNormal);
		
	pInsert->get_10(point);
	OdGePoint3d insertPoint(point[0], point[1], point[2]);
	insertPoint.transformBy(OdGeMatrix3d::planeToWorld(insertNormal));
	pOdDbInsert->setPosition(insertPoint);
		
	double doubleVal;
	pInsert->get_50(&doubleVal);
	pOdDbInsert->setRotation(doubleVal);
		
	int intVal;
	pInsert->get_62(&intVal);
	pOdDbInsert->setColorIndex(intVal);
		
	OdGeScale3d scale(*pInsert->retp_41(), *pInsert->retp_42(), *pInsert->retp_43());
	pOdDbInsert->setScaleFactors(scale);
	
	if (pInsert->retAttribsFollow())
	{
		// Before adding attributes, the insert has to be database resident.
		OdDbObjectId idBefore = ReadWriteUtils::setHandle(m_pOdDatabase, pInsert, ownerId, pOdDbInsert);
		OdDbBlockTableRecordPtr ownerPtr = ownerId.safeOpenObject(OdDb::kForWrite);
		OdDbObjectId idAfter = ownerPtr->appendOdDbEntity(pOdDbInsert);
		ASSERT(idBefore == idAfter);
		finalize(pInsert, pOdDbInsert);

		db_handitem* pEnt = pEntity->next;
		while(pEnt->ret_type() != DB_SEQEND)
		{
			if (pEnt->ret_type() == DB_ATTRIB)
			{
				OdDbAttributePtr pOdDbAttribute = createOdDbAttribute(pEnt, pOdDbInsert);
				ASSERT(!pOdDbAttribute.isNull());

				idAfter = pOdDbInsert->appendAttribute(pOdDbAttribute);
				ASSERT(pOdDbAttribute->id() == idAfter);
			}
			pEnt = pEnt->next;
		}

		// Since we had to add attributes which means the insert itself had
		// to be database resident prior to adding them, we return a null
		// object prointer to prevent further processing.
		return OdDbObjectPtr();
	}

	return pOdDbInsert;
}

OdDbEntityPtr DWGdirectConverter::createOdDbMInsertBlock(db_handitem* pEntity, OdDbObjectId ownerId)
{
	ASSERT(pEntity != NULL && pEntity->ret_type() == DB_INSERT);

	db_insert* pInsert = (db_insert*)pEntity;

	int rows = 1;
	int columns = 1;
	pInsert->get_70(&columns);
	pInsert->get_71(&rows);
	
	ASSERT(columns != 1 || rows != 1);

	OdDbMInsertBlockPtr pOdDbInsert = OdDbMInsertBlock::createObject();
	ASSERT(!pOdDbInsert.isNull());
	((OdDbEntityPtr)pOdDbInsert)->setDatabaseDefaults(m_pOdDatabase);
	
	pOdDbInsert->setColumns(columns);
	pOdDbInsert->setRows(rows);

	double columnSpacing, rowSpacing;
	pInsert->get_44(&columnSpacing);
	pInsert->get_45(&rowSpacing);
	
	pOdDbInsert->setColumns(columns);
	pOdDbInsert->setRows(rows);

	pOdDbInsert->setColumnSpacing(columnSpacing);
	pOdDbInsert->setRowSpacing(rowSpacing);

	char* str;
	pInsert->get_2(&str);
	OdDbObjectId blockId = ReadWriteUtils::getBlockIDByName(m_pOdDatabase, m_pICdwg, this, OdString(str));
	pOdDbInsert->setBlockTableRecord(blockId);

	sds_point point;
	pInsert->get_10(point);
	pOdDbInsert->setPosition(OdGePoint3d(point[0], point[1], point[2]));
		
	pInsert->get_210(point);
	pOdDbInsert->setNormal(OdGeVector3d(point[0], point[1], point[2]));
		
	double doubleVal;
	pInsert->get_50(&doubleVal);
	pOdDbInsert->setRotation(doubleVal);
		
	int intVal;
	pInsert->get_62(&intVal);
	pOdDbInsert->setColorIndex(intVal);
		

	pOdDbInsert->setScaleFactors(OdGeScale3d(*pInsert->retp_41(), *pInsert->retp_42(), *pInsert->retp_43()));
	
	if (pInsert->retAttribsFollow())
	{
		// Before adding attributes, the insert has to be database resident.
		OdDbObjectId idBefore = ReadWriteUtils::setHandle(m_pOdDatabase, pInsert, ownerId, pOdDbInsert);
		OdDbBlockTableRecordPtr ownerPtr = ownerId.safeOpenObject(OdDb::kForWrite);
		OdDbObjectId idAfter = ownerPtr->appendOdDbEntity(pOdDbInsert);
		ASSERT(idBefore == idAfter);
		finalize(pInsert, pOdDbInsert);

		db_handitem* pEnt = pEntity->next;
		while(pEnt->ret_type() != DB_SEQEND)
		{
			if (pEnt->ret_type() == DB_ATTRIB)
			{
				OdDbAttributePtr pOdDbAttribute = createOdDbAttribute(pEnt, pOdDbInsert);
				ASSERT(!pOdDbAttribute.isNull());

				idAfter = pOdDbInsert->appendAttribute(pOdDbAttribute);
				ASSERT(pOdDbAttribute->id() == idAfter);
			}
			pEnt = pEnt->next;
		}

		// Since we had to add attributes which means the insert itself had
		// to be database resident prior to adding them, we return a null
		// object prointer to prevent further processing.
		return OdDbObjectPtr();
	}

	return pOdDbInsert;
}

OdDbObjectPtr DWGdirectConverter::createOdDbRasterImageDef(OdDbObjectId ownerId, db_handitem* pHanditem)
{
	ASSERT(pHanditem != NULL && pHanditem->ret_type() == DB_IMAGEDEF);

	db_imagedef* pImageDef = (db_imagedef*)pHanditem;
	OdDbRasterImageDefPtr pOdDbImageDef = OdDbRasterImageDef::createObject();
	ASSERT(!pOdDbImageDef.isNull());
	OdDbObjectId odImageDefID = ReadWriteUtils::setHandle(m_pOdDatabase, pImageDef, ownerId, pOdDbImageDef);
	
	char* imageFileName;
	pImageDef->get_1(&imageFileName);
	pOdDbImageDef->setSourceFileName(imageFileName);

	return pOdDbImageDef;
}

OdDbObjectPtr DWGdirectConverter::createOdDbGroup(OdDbObjectId ownerId, db_handitem* pHanditem)
{
	OdDbObjectId retId;

	ASSERT(!m_pOdDatabase.isNull());
	ASSERT(!ownerId.isNull());
	ASSERT(pHanditem != NULL);
	ASSERT(pHanditem->ret_type() == DB_GROUP);

	OdDbGroupPtr pOdGroup;
	CDbGroup* pGroup = (CDbGroup*)pHanditem;

	db_hireflink* pListBegin = NULL;
	db_hireflink* pListEnd = NULL;

	pGroup->getEntitiesList(pListBegin, pListEnd);
	if(pListBegin == NULL)
		return pOdGroup;

	// Create the group here. Need its ID for entity reactor.
	pOdGroup = OdDbGroup::createObject();
	OdDbObjectId odGroupID = ReadWriteUtils::setHandle(m_pOdDatabase, pHanditem, ownerId, pOdGroup);

	// Anonymous flag
	int intVal;
	pGroup->get_70(&intVal);
	if (intVal == 1)
		pOdGroup->setAnonymous();
	// Color
	pGroup->get_62(&intVal);
	pOdGroup->setColorIndex(intVal);
	// Description
	pOdGroup->setDescription(OdString(pGroup->description()));
	// Name
	pOdGroup->setName(OdString(pGroup->name(m_pICdwg)));
	// layer
	char* layerName;
	pGroup->get_8(&layerName);
	pOdGroup->setLayer(OdString(layerName));
	// linetype
	char* lineType;
	pGroup->get_6(&lineType);
	pOdGroup->setLinetype(OdString(lineType));
	// linetype scale
	double doubleVal;
	pGroup->get_48(&doubleVal);
	pOdGroup->setLinetypeScale(doubleVal);
	// linetype weight
	pGroup->get_370(&intVal);
	pOdGroup->setLineweight((OdDb::LineWeight)intVal);
	// Selectable
	pGroup->get_71(&intVal);
	pOdGroup->setSelectable((intVal == 1) ? true : false);

	pGroup->get_60(&intVal);
	pOdGroup->setVisibility((OdDb::Visibility)intVal);

	pGroup->get_380(&intVal);
	if (intVal > 2)
	{
		db_handitem* plotStyle;
		pGroup->get_390(&plotStyle);

		char* styleName;
		plotStyle->get_1(&styleName);
		pOdGroup->setPlotStyleName(OdString(styleName));
	}

	while (pListBegin != NULL)
	{
		pHanditem = pListBegin->ret_hip(m_pICdwg);
		
		char hand[17];
		pListBegin->hand.ToAscii(hand);
		OdDbObjectId entityId = m_pOdDatabase->getOdDbObjectId(OdDbHandle(hand));

		if (!entityId.isNull())
		{
			// Now add the entity to the group.
			pOdGroup->append(entityId);
		}

		pListBegin = pListBegin->next;
	}

	return pOdGroup;
}

OdDbObjectPtr DWGdirectConverter::createOdDbLayout(OdDbObjectId layoutDictId, db_handitem* pHanditem)
{
	OdDbObjectId retId;

	ASSERT(!m_pOdDatabase.isNull());
	ASSERT(pHanditem != NULL);
	ASSERT(pHanditem->ret_type() == DB_LAYOUT);

	CDbLayout* pLayout = (CDbLayout*)pHanditem;

	OdDbLayoutPtr pOdLayout  = OdDbLayout::createObject();
	OdDbObjectId odLayoutID = ReadWriteUtils::setHandle(m_pOdDatabase, pHanditem, layoutDictId, pOdLayout);

	OdString layoutName = pLayout->name(m_pICdwg);
	pOdLayout->setLayoutName(layoutName);

	OdDbPlotSettingsValidator* pPlotSettingsValidator = m_pExServices->plotSettingsValidator();

	char* tmpStr;

	pLayout->get_2(&tmpStr);
	pPlotSettingsValidator->setPlotCfgName((OdDbPlotSettings*)pOdLayout, tmpStr);

    pLayout->get_6(&tmpStr);
	pPlotSettingsValidator->setPlotViewName((OdDbPlotSettings*)pOdLayout, tmpStr);
	
    pLayout->get_7(&tmpStr);
	pPlotSettingsValidator->setCurrentStyleSheet((OdDbPlotSettings*)pOdLayout, tmpStr);

    pLayout->get_4(&tmpStr);
	pPlotSettingsValidator->setCanonicalMediaName((OdDbPlotSettings*)pOdLayout, tmpStr);

	int plotUnits;
	double paperWidth, paperHeight;
	double lMargin, tMargin, rMargin, bMargin;
    pLayout->get_40(&lMargin);
    pLayout->get_41(&bMargin);
    pLayout->get_42(&rMargin);
    pLayout->get_43(&tMargin);
    pLayout->get_44(&paperWidth);
    pLayout->get_45(&paperHeight);
    pLayout->get_72(&plotUnits);

	pPlotSettingsValidator->setMediaSize((OdDbPlotSettings*)pOdLayout,
										 tmpStr,
										 paperWidth,
										 paperHeight,
										 (PlotPaperUnits)plotUnits,
										 lMargin,
										 tMargin,
										 rMargin,
										 bMargin);
	double x, y, xmax, ymax;

    pLayout->get_46(&x);
    pLayout->get_47(&y);
	pPlotSettingsValidator->setPlotOrigin((OdDbPlotSettings*)pOdLayout,
										  x, y);

    pLayout->get_48(&x);
    pLayout->get_49(&y);
    pLayout->get_140(&xmax);
    pLayout->get_141(&ymax);
	pPlotSettingsValidator->setPlotWindowArea((OdDbPlotSettings*)pOdLayout,
		                                      x, y, xmax, ymax);

    pLayout->get_142(&x);
    pLayout->get_143(&y);
	pPlotSettingsValidator->setCustomPrintScale((OdDbPlotSettings*)pOdLayout,
												x, y);

	int intVal;
	pLayout->get_73(&intVal);
	pPlotSettingsValidator->setPlotRotation((OdDbPlotSettings*)pOdLayout, 
										    (OdDbPlotSettings::PlotRotation)intVal);

	pLayout->get_74(&intVal);
	pPlotSettingsValidator->setPlotType((OdDbPlotSettings*)pOdLayout, 
										(OdDbPlotSettings::PlotType)intVal);

	pLayout->get_75(&intVal);
	pPlotSettingsValidator->setStdScaleType((OdDbPlotSettings*)pOdLayout, 
										    (OdDbPlotSettings::StdScaleType)intVal);

	double doubleVal;
	pLayout->get_147(&doubleVal);
	pPlotSettingsValidator->setStdScale((OdDbPlotSettings*)pOdLayout, 
										doubleVal);
	
	pLayout->get_148(&x);
	pLayout->get_149(&y);
	pOdLayout->setPaperImageOrigin(OdGePoint2d(x, y));

	pLayout->get_71(&intVal);
	pOdLayout->setTabOrder(intVal);

	sds_point point;
	pLayout->get_10(point);
	pOdLayout->setLIMMIN(OdGePoint2d(point[0], point[1]));

	pLayout->get_11(point);
	pOdLayout->setLIMMAX(OdGePoint2d(point[0], point[1]));

	pLayout->get_12(point);
	pOdLayout->setINSBASE(OdGePoint3d(point[0], point[1], point[2]));

	pLayout->get_14(point);
	pOdLayout->setEXTMIN(OdGePoint3d(point[0], point[1], point[2]));

	pLayout->get_15(point);
	pOdLayout->setEXTMAX(OdGePoint3d(point[0], point[1], point[2]));

	pOdLayout->setLIMCHECK(pLayout->isLIMCHECK());
	pOdLayout->setPSLTSCALE(pLayout->isPSLTSCALE());
	pOdLayout->setTabOrder(pLayout->tabOrder());

	return pOdLayout;
}

OdDbObjectPtr DWGdirectConverter::updateOdDbLayout(OdDbObjectId dictionaryId, db_handitem* pObject, OdDbObject* pExistingOdLayout)
{
	ASSERT(!m_pOdDatabase.isNull());
	ASSERT(pObject != NULL);
	ASSERT(pObject->ret_type() == DB_LAYOUT);
	ASSERT(pExistingOdLayout != NULL);
	ASSERT(pExistingOdLayout->isKindOf(OdDbLayout::desc()));

	CDbLayout* pLayout = (CDbLayout*)pObject;
	OdDbLayoutPtr pOdLayout = (OdDbLayoutPtr)pExistingOdLayout;

	OdString layoutName = pLayout->name(m_pICdwg);
	pOdLayout->setLayoutName(layoutName);

	OdDbPlotSettingsValidator* pPlotSettingsValidator = m_pExServices->plotSettingsValidator();

	char* tmpStr;

	pLayout->get_2(&tmpStr);
	pPlotSettingsValidator->setPlotCfgName((OdDbPlotSettings*)pOdLayout, tmpStr);

    pLayout->get_6(&tmpStr);
	pPlotSettingsValidator->setPlotViewName((OdDbPlotSettings*)pOdLayout, tmpStr);
	
    pLayout->get_7(&tmpStr);
	pPlotSettingsValidator->setCurrentStyleSheet((OdDbPlotSettings*)pOdLayout, tmpStr);

    pLayout->get_4(&tmpStr);
	pPlotSettingsValidator->setCanonicalMediaName((OdDbPlotSettings*)pOdLayout, tmpStr);

	int plotUnits;
	double paperWidth, paperHeight;
	double lMargin, tMargin, rMargin, bMargin;
    pLayout->get_40(&lMargin);
    pLayout->get_41(&bMargin);
    pLayout->get_42(&rMargin);
    pLayout->get_43(&tMargin);
    pLayout->get_44(&paperWidth);
    pLayout->get_45(&paperHeight);
    pLayout->get_72(&plotUnits);

	pPlotSettingsValidator->setMediaSize((OdDbPlotSettings*)pOdLayout,
										 tmpStr,
										 paperWidth,
										 paperHeight,
										 (PlotPaperUnits)plotUnits,
										 lMargin,
										 tMargin,
										 rMargin,
										 bMargin);
	double x, y, xmax, ymax;

    pLayout->get_46(&x);
    pLayout->get_47(&y);
	pPlotSettingsValidator->setPlotOrigin((OdDbPlotSettings*)pOdLayout,
										  x, y);

    pLayout->get_48(&x);
    pLayout->get_49(&y);
    pLayout->get_140(&xmax);
    pLayout->get_141(&ymax);
	pPlotSettingsValidator->setPlotWindowArea((OdDbPlotSettings*)pOdLayout,
		                                      x, y, xmax, ymax);

    pLayout->get_142(&x);
    pLayout->get_143(&y);
	pPlotSettingsValidator->setCustomPrintScale((OdDbPlotSettings*)pOdLayout,
												x, y);

	int intVal;
	pLayout->get_73(&intVal);
	pPlotSettingsValidator->setPlotRotation((OdDbPlotSettings*)pOdLayout, 
										    (OdDbPlotSettings::PlotRotation)intVal);

	pLayout->get_74(&intVal);
	pPlotSettingsValidator->setPlotType((OdDbPlotSettings*)pOdLayout, 
										(OdDbPlotSettings::PlotType)intVal);

	pLayout->get_75(&intVal);
	pPlotSettingsValidator->setStdScaleType((OdDbPlotSettings*)pOdLayout, 
										    (OdDbPlotSettings::StdScaleType)intVal);

	double doubleVal;
	pLayout->get_147(&doubleVal);
	pPlotSettingsValidator->setStdScale((OdDbPlotSettings*)pOdLayout, 
										doubleVal);
	
	pLayout->get_148(&x);
	pLayout->get_149(&y);
	pOdLayout->setPaperImageOrigin(OdGePoint2d(x, y));

	pLayout->get_71(&intVal);
	pOdLayout->setTabOrder(intVal);

	sds_point point;
	pLayout->get_10(point);
	pOdLayout->setLIMMIN(OdGePoint2d(point[0], point[1]));

	pLayout->get_11(point);
	pOdLayout->setLIMMAX(OdGePoint2d(point[0], point[1]));

	pLayout->get_12(point);
	pOdLayout->setINSBASE(OdGePoint3d(point[0], point[1], point[2]));

	pLayout->get_14(point);
	pOdLayout->setEXTMIN(OdGePoint3d(point[0], point[1], point[2]));

	pLayout->get_15(point);
	pOdLayout->setEXTMAX(OdGePoint3d(point[0], point[1], point[2]));

	int flags;
	pLayout->get_Layout70(&flags);

	pOdLayout->setLIMCHECK(((flags & BIT2) == BIT2) ? true : false);
	pOdLayout->setPSLTSCALE(((flags & BIT1) == BIT1) ? true : false);
	pOdLayout->setTabOrder(pLayout->tabOrder());

	return pOdLayout;
}

OdDbObjectPtr DWGdirectConverter::createOdDbMlStyle(OdDbObjectId dictionaryId, db_handitem* pHanditem)
{
	ASSERT(!m_pOdDatabase.isNull());
	ASSERT(pHanditem != NULL);
	ASSERT(pHanditem->ret_type() == DB_MLINESTYLE);

	db_mlinestyle* pMlStyle = (db_mlinestyle*)pHanditem;

	OdDbMlineStylePtr pOdMlStyle  = OdDbMlineStyle::createObject();
	OdDbObjectId odMlStyleID = ReadWriteUtils::setHandle(m_pOdDatabase, pHanditem, dictionaryId, pOdMlStyle);

	char* str;
    pMlStyle->get_2(&str);
	pOdMlStyle->setName(str);

    pMlStyle->get_3(&str);
	pOdMlStyle->setDescription(str);

	double doubleVal;
    pMlStyle->get_51(&doubleVal);
	pOdMlStyle->setStartAngle(doubleVal);

    pMlStyle->get_52(&doubleVal);
	pOdMlStyle->setEndAngle(doubleVal);

	int intVal;
    pMlStyle->get_62(&intVal);
	OdCmColor color;
	color.setColorMethod(OdCmEntityColor::kByACI);
	color.setColorIndex(intVal);
	pOdMlStyle->setFillColor(color);

	int flags;
	pMlStyle->get_70(&flags);

	if (flags & BIT1)
		pOdMlStyle->setFilled(true);
	if (flags & BIT2)
		pOdMlStyle->setShowMiters(true);
	if (flags & BIT10)
		pOdMlStyle->setStartSquareCap(true);
	if (flags & BIT20)
		pOdMlStyle->setStartInnerArcs(true);
	if (flags & BIT40)
		pOdMlStyle->setStartRoundCap(true);
	if (flags & BIT100)
		pOdMlStyle->setEndSquareCap(true);
	if (flags & BIT200)
		pOdMlStyle->setEndInnerArcs(true);
	if (flags & BIT400)
		pOdMlStyle->setEndRoundCap(true);

	pMlStyle->get_71(&intVal);

	int* colorVals = new int[intVal];
	memset(colorVals, 0,sizeof(int)*intVal);
	pMlStyle->get_62(colorVals, intVal);

	char** linetypes = new char *[intVal];
	memset(linetypes, 0,sizeof(char *) * intVal);
	pMlStyle->get_6(linetypes, intVal);

	double* offsets = new double[intVal];
	memset(offsets, 0,sizeof(double) * intVal);
	pMlStyle->get_49(offsets, intVal);

	for (int i = 0; i < intVal; i ++)
	{
		color.setColorIndex(colorVals[i]);

		pOdMlStyle->addElement(offsets[i], color, ReadWriteUtils::getLinetypeIDByName(m_pOdDatabase, OdString(linetypes[i])));
	}

	delete [] colorVals;
	delete [] linetypes;
	delete [] offsets;

	return pOdMlStyle;
}

OdDbObjectPtr DWGdirectConverter::updateOdDbMlStyle(OdDbObjectId dictionaryId, db_handitem* pObject, OdDbObject* pExistingMlStyle)
{
	ASSERT(!m_pOdDatabase.isNull());
	ASSERT(pObject != NULL);
	ASSERT(pObject->ret_type() == DB_MLINESTYLE);
	ASSERT(pExistingMlStyle != NULL);
	ASSERT(pExistingMlStyle->isKindOf(OdDbMlineStyle::desc()));

	db_mlinestyle* pMlStyle = (db_mlinestyle*)pObject;
	OdDbMlineStylePtr pOdMlStyle = (OdDbMlineStylePtr)pExistingMlStyle;

	char* str;
    pMlStyle->get_2(&str);
	pOdMlStyle->setName(str);

    pMlStyle->get_3(&str);
	pOdMlStyle->setDescription(str);

	double doubleVal;
    pMlStyle->get_51(&doubleVal);
	pOdMlStyle->setStartAngle(doubleVal);

    pMlStyle->get_52(&doubleVal);
	pOdMlStyle->setEndAngle(doubleVal);

	int intVal;
    pMlStyle->get_62(&intVal);
	OdCmColor color;
	color.setColorMethod(OdCmEntityColor::kByACI);
	color.setColorIndex(intVal);
	pOdMlStyle->setFillColor(color);

	int flags;
	pMlStyle->get_70(&flags);

	if (flags & BIT1)
		pOdMlStyle->setFilled(true);
	if (flags & BIT2)
		pOdMlStyle->setShowMiters(true);
	if (flags & BIT10)
		pOdMlStyle->setStartSquareCap(true);
	if (flags & BIT20)
		pOdMlStyle->setStartInnerArcs(true);
	if (flags & BIT40)
		pOdMlStyle->setStartRoundCap(true);
	if (flags & BIT80)
		pOdMlStyle->setEndSquareCap(true);
	if (flags & BIT100)
		pOdMlStyle->setEndInnerArcs(true);
	if (flags & BIT200)
		pOdMlStyle->setEndRoundCap(true);


	pMlStyle->get_71(&intVal);

	int* colorVals = new int[intVal];
	memset(colorVals, 0,sizeof(int)*intVal);
	pMlStyle->get_62(colorVals, intVal);

	char** linetypes = new char *[intVal];
	memset(linetypes, 0,sizeof(char *) * intVal);
	pMlStyle->get_6(linetypes, intVal);

	double* offsets = new double[intVal];
	memset(offsets, 0,sizeof(double) * intVal);
	pMlStyle->get_49(offsets, intVal);

	// First remove all the elements.
	for (int i = 0; i < intVal; i ++)
		pOdMlStyle->removeElementAt(0/* Yep 0 */);

	for (i = 0; i < intVal; i ++)
	{
		color.setColorIndex(colorVals[i]);

		pOdMlStyle->addElement(offsets[i], color, ReadWriteUtils::getLinetypeIDByName(m_pOdDatabase, OdString(linetypes[i])));
	}
	
	delete [] colorVals;
	delete [] linetypes;
	delete [] offsets;

	return pOdMlStyle;
}

OdDbObjectPtr DWGdirectConverter::createOdDbPlotSettings(OdDbObjectId dictionaryId, db_handitem* pObject)
{
	ASSERT(!m_pOdDatabase.isNull());
	ASSERT(pObject != NULL);
	ASSERT(pObject->ret_type() == DB_PLOTSETTINGS);

	CDbPlotSettings* pSettings = (CDbPlotSettings*)pObject;

	OdDbPlotSettingsPtr pOdSettings   = OdDbPlotSettings::createObject();
	OdDbObjectId odSettingsID = ReadWriteUtils::setHandle(m_pOdDatabase, pObject, dictionaryId, pOdSettings);

	OdRxObjectImpl<DWGdirectWriterServices> tmpExServices;
	OdDbPlotSettingsValidatorPtr tmpPlotValidator = tmpExServices.plotSettingsValidator();

	char* device;
	pSettings->get_2(&device);
	tmpPlotValidator->setPlotCfgName(pOdSettings, device, pSettings->name(m_pICdwg));

	char* str;
	pSettings->get_6(&str);
	tmpPlotValidator->setPlotViewName(pOdSettings, str);
	pSettings->get_7(&str);
	tmpPlotValidator->setCurrentStyleSheet(pOdSettings, str);

	double width, height;
	double printXmin, printYmin, printXmax, printYmax;
	pSettings->get_4(&str);
	pSettings->getPlotPaperSize(width, height);
	pSettings->getPlotPaperMargins(printXmin, printYmin, printXmax, printYmax);
	tmpPlotValidator->setMediaSize(pOdSettings, str,
									width, height,
									(OdDbPlotSettings::PlotPaperUnits)pSettings->plotPaperUnits(),
									printXmin, printYmin, printXmax, printYmax);

	tmpPlotValidator->setPlotPaperUnits(pOdSettings, (OdDbPlotSettings::PlotPaperUnits)pSettings->plotPaperUnits());

	tmpPlotValidator->setPlotRotation(pOdSettings, (OdDbPlotSettings::PlotRotation)pSettings->plotRotation());
	tmpPlotValidator->setPlotType(pOdSettings, (OdDbPlotSettings::PlotType)pSettings->plotType());
	tmpPlotValidator->setStdScaleType(pOdSettings, (OdDbPlotSettings::StdScaleType)pSettings->scaleType());

	double x, y;
	pSettings->getPlotOrigin(x, y);
	tmpPlotValidator->setPlotOrigin(pOdSettings, x, y);

	pSettings->getPlotWindowArea(printXmin, printYmin, printXmax, printYmax);
	tmpPlotValidator->setPlotWindowArea(pOdSettings, printXmin, printYmin, printXmax, printYmax);

	pSettings->get_142(&x);
	pSettings->get_143(&y);
	tmpPlotValidator->setCustomPrintScale(pOdSettings, x, y);

	pSettings->get_147(&x);
	tmpPlotValidator->setStdScale(pOdSettings, x);

	pSettings->get_148(&x);
	pSettings->get_149(&y);
	pOdSettings->setPaperImageOrigin(OdGePoint2d(x, y));

	int flags = 0;
	pSettings->get_70(&flags);

	pOdSettings->setPlotViewportBorders(false);
	if (flags & BIT1)
		pOdSettings->setPlotViewportBorders(true);
	pOdSettings->setShowPlotStyles(false);
	if (flags & BIT2)
		pOdSettings->setShowPlotStyles(true);
	pOdSettings->setPlotHidden(false);
	if (flags & BIT8)
		pOdSettings->setPlotHidden(true);
	pOdSettings->setPlotPlotStyles(false);
	tmpPlotValidator->setUseStandardScale(pOdSettings, false);
	if (flags & BIT10)
		tmpPlotValidator->setUseStandardScale(pOdSettings, true);
	if (flags & BIT20)
		pOdSettings->setPlotPlotStyles(true);
	pOdSettings->setScaleLineweights(false);
	if (flags & BIT40)
		pOdSettings->setScaleLineweights(true);
	pOdSettings->setPrintLineweights(false);
	if (flags & BIT80)
		pOdSettings->setPrintLineweights(true);
	pOdSettings->setDrawViewportsFirst(false);
	if (flags & BIT200)
		pOdSettings->setDrawViewportsFirst(true);
	pOdSettings->setModelType(false);
	if (flags & BIT400)
		pOdSettings->setModelType(true);

	return pOdSettings;
}

OdDbObjectPtr DWGdirectConverter::createOdDbProxyObject(OdDbObjectId ownerId, db_handitem* pObject)
{	
	ASSERT(!m_pOdDatabase.isNull());
	ASSERT(pObject != NULL);
	ASSERT(pObject->ret_type() == DB_ACAD_PROXY_OBJECT);

	db_acad_proxy_object* pProxyObject = (db_acad_proxy_object*)pObject;
	void* pOdClass = pProxyObject->getOdDbClass();
	ASSERT(pOdClass != NULL);

	if (pOdClass == NULL)
		return OdDbObjectPtr();

	OdRxClassPtr pRegClass;
	pRegClass.attach((OdRxClass*)pOdClass);

	OdDbObjectPtr pOdObject = pRegClass->create(); 
	ASSERT(!pOdObject.isNull());

	pProxyObject->setOdDbClass((void*)pRegClass.detach());

//  This will be done in processProxyReferences() after
//  all proxy objects have been created.
//	CDxfFiler tmpFiler(m_pOdDatabase);
//	tmpFiler.initResBuf(ReadWriteUtils::getOdResBufPtrFromSdsResBufPtr(m_pOdDatabase, pProxyObject->getResbufChain()));

	ASSERT(!ownerId.isNull());
	if (!ownerId.isNull())
	{
		char hand[17];
		pProxyObject->RetHandlePtr()->ToAscii(hand);

		m_pOdDatabase->addOdDbObject(pOdObject, ownerId, OdDbHandle(hand));

//		tmpFiler.nextItem(); // Skipp DXF 5 handle value;
		// This will be done in processProxyReferences() after
		// all proxy objects have been created.
//		pOdObject->dxfInFields(&tmpFiler);
	}

	appendData(pProxyObject, pOdObject);

	m_proxyReferences.insert(m_proxyReferences.end(), handItemObjectIDPair(pProxyObject, pOdObject->objectId(), 0));

	db_hireflink *firstpp, *lastpp;
	pProxyObject->get_hirefllends(&firstpp, &lastpp);
	if (firstpp != NULL)
	{
		while(firstpp != NULL)
		{
			int type;
			db_objhandle hand;
			db_handitem* handItem;

			((db_hireflink*)firstpp)->get_data(&type, &hand, &handItem, m_pICdwg);
			if (handItem != NULL)
			{
				char hand[17];
				handItem->RetHandlePtr()->ToAscii(hand);
				OdDbObjectId objId = m_pOdDatabase->getOdDbObjectId(OdDbHandle(hand));

				if (objId.isNull())
				{
					int entType = handItem->ret_type();

					if (db_is_objecttype(entType))
					{
						//objId = createOdDbObject(dictionaryId, handItem, OdString());
						objId = createOdDbObject(pOdObject->objectId(), handItem, OdString());
						//if (!objId.isNull())
						//{
						//	{
						//		m_proxyReferences.insert(m_proxyReferences.end(), handItemObjectIDPair(handItem, objId, 0));
						//	}
						//}
						bool bIsInDB = ((OdDbObjectPtr)objId.safeOpenObject(OdDb::kForRead))->isDBRO();
						ASSERT(bIsInDB);

						ASSERT(!objId.isNull());
					}
				}
			}
			firstpp = firstpp->next;
		}
	}

	return pOdObject;
}

//OdDbObjectPtr DWGdirectConverter::createOdDbLayerIndex(OdDbObjectId dictionaryId, db_handitem* pObject, OdString& recordName)
//{
//	ASSERT(!m_pOdDatabase.isNull());
//	ASSERT(pObject != NULL);
//	ASSERT(pObject->ret_type() == DB_LAYER_INDEX);
//
//	db_layer_index* pIndex = (db_layer_index*)pObject;
//	void* pOdClass = pIndex->getOdDbClass();
//	ASSERT(pOdClass != NULL);
//
//	if (pOdClass == NULL)
//		return OdDbObjectPtr();
//
//	OdRxClassPtr pRegClass;
//	pRegClass.attach((OdRxClass*)pOdClass);
//
//	OdDbObjectPtr pOdObject = pRegClass->create(); 
//	ASSERT(!pOdObject.isNull());
//
//	CDxfFiler tmpFiler(m_pOdDatabase);
//	tmpFiler.initResBuf(ReadWriteUtils::getOdResBufPtrFromSdsResBufPtr(m_pOdDatabase, pIndex->getResbufChain()));
//
//	ASSERT(!dictionaryId.isNull());
//	if (!dictionaryId.isNull())
//	{
//		char hand[8];
//		pIndex->RetHandlePtr()->ToAscii(hand);
//
//		m_pOdDatabase->addOdDbObject(pOdObject, dictionaryId, OdDbHandle(hand));
//
//		tmpFiler.nextItem(); // Skipp DXF 5 handle value;
//
//		pOdObject->dxfInFields(&tmpFiler);
//
//		OdDbDictionaryPtr pDict = dictionaryId.safeOpenObject(OdDb::kForWrite);
//		pDict->setAt(recordName, pOdObject);
//	}
//
//	pRegClass.detach();
//
//	return pOdObject;
//}


OdDbObjectPtr DWGdirectConverter::createOdDbDimAssoc(OdDbObjectId dictionaryId, db_handitem* pObject)
{
	ASSERT(!m_pOdDatabase.isNull());
	ASSERT(pObject != NULL);
	ASSERT(pObject->ret_type() == DB_DIMASSOC);

	db_dimassoc* pDimAssoc = (db_dimassoc*)pObject;

	OdDbDimAssocPtr pOdDimAssoc   = OdDbDimAssoc::createObject();
	OdDbObjectId odDimAssocID = ReadWriteUtils::setHandle(m_pOdDatabase, pDimAssoc, dictionaryId, pOdDimAssoc);

	m_pOdDatabase->addOdDbObject(pOdDimAssoc, dictionaryId);

	appendData(pDimAssoc, pOdDimAssoc);
	
	char hand[17];
	db_handitem* pDim;
	pDimAssoc->get_330(&pDim);
	pDim->RetHandlePtr()->ToAscii(hand);
	OdDbObjectId dimId = m_pOdDatabase->getOdDbObjectId(OdDbHandle(hand));
	ASSERT(!dimId.isNull());
	pOdDimAssoc->setDimension(dimId);

	int transSpaceFlag;
	pDimAssoc->get_70(&transSpaceFlag);
	pOdDimAssoc->setTransSpace((transSpaceFlag == 1) ? true : false);

	int rotatedDimType;
	pDimAssoc->get_71(&rotatedDimType);
	pOdDimAssoc->setRotatedDimType((OdDbDimAssoc::RotatedType)rotatedDimType);

	int intVal;
	double doubleVal;
	char* str;
	int index = 0;
	for (int i = 0; i < 4; i++) 
	{
		pDimAssoc->get_1(&str, i);
		if (str == NULL)
			continue;

		OdDbOsnapPointRefPtr osnapRef = OdDbOsnapPointRef::createObject();

		pDimAssoc->get_72(&intVal, i);
		osnapRef->setOsnapMode((OdDb::OsnapMode)intVal);

		pDimAssoc->get_40(&doubleVal, i);
		osnapRef->setNearOsnap(doubleVal);

		db_handitem* mainEnt;
		pDimAssoc->get_331(&mainEnt, i);
		if (mainEnt != NULL)
		{
			mainEnt->RetHandlePtr()->ToAscii(hand);

			OdDbXrefFullSubentPath& mainEntity = osnapRef->mainEntity();
			OdDbObjectIdArray& mainObjectIds = mainEntity.objectIds();

			OdDbObjectId mainEntId = m_pOdDatabase->getOdDbObjectId(OdDbHandle(hand));
			if (!mainEntId.isNull())
				mainObjectIds.insert(mainObjectIds.end(), mainEntId);

			int intVal;
			pDimAssoc->get_73(&intVal, i);
			OdDbSubentId& subEntityId = mainEntity.subentId();
			subEntityId.setIndex(index);
			subEntityId.setType((OdDb::SubentType)intVal);

			unsigned char odhandle[17];
			OdHandleArray& handles = mainEntity.xrefObjHandles();
			long handleCount = pDimAssoc->ret_xrefHandleCount(i);
			for (int j = 0; j < handleCount; j++)
			{
				pDimAssoc->get_xrefObjHandle(j, odhandle);
				handles.insert(handles.end(), OdDbHandle((OdChar *)odhandle));
			}
		}

		db_handitem* intEnt;
		pDimAssoc->get_332(&intEnt, i);
		if (intEnt != NULL)
		{
			intEnt->RetHandlePtr()->ToAscii(hand);

			OdDbXrefFullSubentPath& intersectEnts = osnapRef->intersectEntity();
			OdDbObjectIdArray& intersectObjectIds = intersectEnts.objectIds();

			OdDbObjectId intEntId = m_pOdDatabase->getOdDbObjectId(OdDbHandle(hand));
			ASSERT(!intEntId.isNull());

			intersectObjectIds.insert(intersectObjectIds.end(), intEntId);

			int intVal;
			pDimAssoc->get_74(&intVal, i);
			OdDbSubentId& subEntityId = intersectEnts.subentId();
			subEntityId.setIndex(index);
			subEntityId.setType((OdDb::SubentType)intVal);

			unsigned char odhandle[8];
			OdHandleArray& handles = intersectEnts.xrefObjHandles();
			pDimAssoc->get_intXrefObjHandle(0, odhandle);
			if (odhandle[0] != '\0')
			{
				handles.insert(handles.end(), OdDbHandle((OdChar *)odhandle));
			}

		}

		pOdDimAssoc->setOsnapPointRef((OdDbDimAssoc::Associativity)index, osnapRef);

		// TODO - MHB no way to set it ???
		//pDimAssoc->get_75(&intVal, i);
		//if (intVal == 1)
		//{
		//}
		index++;

	}

	return pOdDimAssoc;
}

OdDbObjectPtr DWGdirectConverter::createOdDbDictionaryVar(OdDbObjectId dictionaryId, db_handitem* pObject)
{
	ASSERT(!m_pOdDatabase.isNull());
	ASSERT(pObject != NULL);
	ASSERT(pObject->ret_type() == DB_DICTIONARYVAR);

	db_dictionaryvar* pDictVar = (db_dictionaryvar*)pObject;

	OdDbDictionaryVarPtr pOdDictVar   = OdDbDictionaryVar::createObject();
	OdDbObjectId odDictVarID = ReadWriteUtils::setHandle(m_pOdDatabase, pDictVar, dictionaryId, pOdDictVar);

	m_pOdDatabase->addOdDbObject(pOdDictVar, dictionaryId);

	appendData(pDictVar, pOdDictVar);
	
	int schema;
	pDictVar->get_280(&schema);
	pOdDictVar->setSchema(schema);

	char* str;
	pDictVar->get_1(&str);
	pOdDictVar->setValue(OdString(str));

	return pOdDictVar;
}

OdDbObjectPtr DWGdirectConverter::createOdDbXRecord(OdDbObjectId dictionaryId, db_handitem* pObject)
{
	ASSERT(!m_pOdDatabase.isNull());
	ASSERT(pObject != NULL);
	ASSERT(pObject->ret_type() == DB_XRECORD);

	db_xrecord* pXRecord = (db_xrecord*)pObject;

	sds_resbuf* resBufChain = (sds_resbuf*)pXRecord->getResbufChain();
	if (resBufChain == NULL)
		return OdDbXrecordPtr();

	OdDbXrecordPtr pOdXrecord = OdDbXrecord::createObject();
	OdDbObjectId odXrecordID = ReadWriteUtils::setHandle(m_pOdDatabase, pXRecord, dictionaryId, pOdXrecord);

	pOdXrecord->setFromRbChain(ReadWriteUtils::getOdResBufPtrFromSdsResBufPtr(m_pOdDatabase, resBufChain), m_pOdDatabase);

	return pOdXrecord;

}

OdDbObjectPtr DWGdirectConverter::createOdDbSpatialFilter(OdDbObjectId dictionaryId, db_handitem* pObject)
{
	ASSERT(!m_pOdDatabase.isNull());
	ASSERT(pObject != NULL);
	ASSERT(pObject->ret_type() == DB_SPATIAL_FILTER);

	db_spatial_filter* pFilter = (db_spatial_filter*)pObject;

	OdDbSpatialFilterPtr pOdFilter = OdDbSpatialFilter::createObject();
	OdDbObjectId odFilterID = ReadWriteUtils::setHandle(m_pOdDatabase, pFilter, dictionaryId, pOdFilter);

	int numPts = pFilter->ret_npts();
	sds_point* pts = new sds_point[numPts];
	ASSERT(pts != NULL);

	pFilter->get_ptarray(pts, numPts);
	OdGePoint2dArray ptArray;
	for (int i = 0; i < numPts; i++)
		ptArray.insert(ptArray.end(), OdGePoint2d(pts[i][0], pts[i][1]));

	sds_point matrix[4];
	OdGeMatrix3d xToClipSpace;
	pFilter->get_clipboundmat(matrix);
	int j = 0;
 	for(i = 0; i < 4; ++i)
	{
		for(j = 0; j < 4; ++j)
		{
			xToClipSpace[i][j] = matrix[j][i];
		}
	}

	int iIsEnabled;
	pFilter->get_71(&iIsEnabled);
	pOdFilter->setDefinition(ptArray,
                     xToClipSpace, 
                     pFilter->ret_frontclip(),
                     pFilter->ret_backclip(),
                     (bool)(iIsEnabled == 1));

	ptArray.clear();

	// TODO ?? MHB
	// void OdDbSpatialFilter::setPerspectiveCamera(const OdGePoint3d& fromPt);

	return pOdFilter;

}

OdDbObjectPtr DWGdirectConverter::createOdDbIDBuffer(OdDbObjectId dictionaryId, db_handitem* pObject)
{
	OutputDebugString("Export of DB_IDBUFFER need verification.\n");

	ASSERT(!m_pOdDatabase.isNull());
	ASSERT(pObject != NULL);
	ASSERT(pObject->ret_type() == DB_IDBUFFER);

	db_idbuffer* pBuffer = (db_idbuffer*)pObject;
	OdDbIdBufferPtr pOdIDBuffer = OdDbIdBuffer::createObject();
	OdDbObjectId odBufferID = ReadWriteUtils::setHandle(m_pOdDatabase, pBuffer, dictionaryId, pOdIDBuffer);

	db_hireflink *firstpp, *lastpp, *tmp;
	pBuffer->get_hirefllends(&firstpp, &lastpp);

	int type;
	char hand[17];
	db_handitem  *pEnt = NULL;
	db_objhandle *pHandle = NULL;
	for (tmp=firstpp; tmp!=NULL; tmp=tmp->next) 
	{
		tmp->get_data(&type, pHandle, &pEnt, m_pICdwg);
		pHandle->ToAscii(hand);
		OdDbObjectId objId = dictionaryId.database()->getOdDbObjectId(OdDbHandle(hand));
		if (!objId.isNull())
		{
			pOdIDBuffer->addId(objId);
		}
	}

	ASSERT(!dictionaryId.isNull());
	if (!dictionaryId.isNull())
	{
		char hand[17];
		pBuffer->RetHandlePtr()->ToAscii(hand);

		m_pOdDatabase->addOdDbObject(pOdIDBuffer, dictionaryId, OdDbHandle(hand));
	}

	appendData(pObject, pOdIDBuffer);
	
	return pOdIDBuffer;
}

OdDbObjectPtr DWGdirectConverter::createOdDbRasterVariables(OdDbObjectId dictionaryId, db_handitem* pObject)
{
	ASSERT(!m_pOdDatabase.isNull());
	ASSERT(pObject != NULL);
	ASSERT(pObject->ret_type() == DB_RASTERVARIABLES);

	db_rastervariables* pRasterVariables = (db_rastervariables*)pObject;

	OdDbRasterVariablesPtr pOdRasterVariables = OdDbRasterVariables::createObject();
	ASSERT(!pOdRasterVariables.isNull());

	OdDbObjectId odOdDbRasterVariablesID = ReadWriteUtils::setHandle(m_pOdDatabase, pRasterVariables, dictionaryId, pOdRasterVariables);

	m_pOdDatabase->addOdDbObject(pOdRasterVariables, dictionaryId);

	appendData(pRasterVariables, pOdRasterVariables);

	int frameFlag;
	pRasterVariables->get_70(&frameFlag);
	pOdRasterVariables->setImageFrame((OdDbRasterVariables::FrameSettings)frameFlag);

	int imageQuality;
	pRasterVariables->get_71(&imageQuality);
	pOdRasterVariables->setImageQuality((OdDbRasterVariables::ImageQuality)imageQuality);

	int scale;
	pRasterVariables->get_72(&scale);
	pOdRasterVariables->setUserScale((OdDbRasterImageDef::Units)scale);

	return pOdRasterVariables;

}

OdDbObjectPtr DWGdirectConverter::createOdDbPlaceholder(OdDbObjectId dictionaryId, db_handitem* pObject)
{
#ifdef _DEBUG_MHB
	OutputDebugString("DWGdirectConverter::createOdDbPlaceholder() needs completion & testing.");
#endif
	ASSERT(!m_pOdDatabase.isNull());
	ASSERT(pObject != NULL);
	ASSERT(pObject->ret_type() == DB_PLACEHOLDER);

	db_placeholder* pPlaceholder = (db_placeholder*)pObject;

	OdDbPlaceHolderPtr pOdPlaceholder = OdDbPlaceHolder::createObject();
	OdDbObjectId odPlaceholderID = ReadWriteUtils::setHandle(m_pOdDatabase, pPlaceholder, dictionaryId, pOdPlaceholder);

	return pOdPlaceholder;
}

OdDbObjectPtr DWGdirectConverter::createOdDbVBAproject(OdDbObjectId ownerId, db_handitem* pObject)
{
	ASSERT(!m_pOdDatabase.isNull());
	ASSERT(pObject != NULL);
	ASSERT(pObject->ret_type() == DB_VBAPROJECT);

	db_vbaproject* pVBAproject = (db_vbaproject*)pObject;

	OdDbVbaProjectPtr pOdVbaProject = OdDbVbaProject::createObject();
	OdDbObjectId odVbaProjectID = ReadWriteUtils::setHandle(m_pOdDatabase, pVBAproject, ownerId, pOdVbaProject);

	long dataLen = pVBAproject->ret_datasize();
	char *data = pVBAproject->ret_data();

	OdBinaryData odData;
	odData.resize(dataLen);
	memcpy((void*)odData.asArrayPtr(), data, dataLen);

	pOdVbaProject->setVbaProject(odData);

	return pOdVbaProject;
}

OdDbObjectPtr DWGdirectConverter::createOdDbSortEnts(OdDbObjectId ownerDictId, db_handitem* pObject)
{
	ASSERT(!m_pOdDatabase.isNull());
	ASSERT(pObject != NULL);
	ASSERT(pObject->ret_type() == DB_SORTENTSTABLE);

	CDbSortentsTable* pSortentsTable = (CDbSortentsTable*)pObject;

	// Use the block's getSortentsTable call to create the actuall table.
	OdDbObjectId ownerBlockId = ownerDictId.safeOpenObject()->ownerId();
	OdDbBlockTableRecordPtr pOdBlock = ownerBlockId.safeOpenObject(OdDb::kForWrite);

	OdDbSortentsTablePtr pOdSortentsTable = pOdBlock->getSortentsTable();	// Creates it if not found.

	OdDbObjectId odSortentsTableID = m_pOdDatabase->addOdDbObject(pOdSortentsTable, ownerDictId);

	pSortentsTable->resetIterator();

	db_objhandle origHandle;
	db_objhandle sortHandle;
	bool bHasEnts = pSortentsTable->next(origHandle, sortHandle);

	OdDbObjectIdArray relArray;
	while(bHasEnts == true)
	{
		char hand[17];

		origHandle.ToAscii(hand);
		OdDbObjectId entIdO = m_pOdDatabase->getOdDbObjectId(OdDbHandle(hand));
		ASSERT(origHandle != NULL && !entIdO.isNull());
		if (origHandle != NULL && !entIdO.isNull())
		{
			relArray.insert(relArray.end(), entIdO);
		}

		sortHandle.ToAscii(hand);
		OdDbObjectId entIdN = m_pOdDatabase->getOdDbObjectId(OdDbHandle(hand));
		ASSERT(sortHandle != NULL && !entIdN.isNull());
		if (sortHandle != NULL && !entIdN.isNull())
		{
			relArray.insert(relArray.end(), entIdN);
		}

		bHasEnts = pSortentsTable->next(origHandle, sortHandle);
	}

	pOdSortentsTable->setRelativeDrawOrder(relArray);

	// TODO The owner of the ownerDictId is the space block that owns the 
	// dictionary. That ID (blockId) needs to be set in this SortentsTable.
	// Currently there doesn't seem to be a way.
	//pOdSortentsTable->setBlockId(ownerDictId.safeOpenObject()->ownerId());

	return pOdSortentsTable;
}

OdDbObjectPtr DWGdirectConverter::createOdDbDimStyle(OdDbObjectId ownerId, db_handitem* pObject)
{
	ASSERT(!m_pOdDatabase.isNull());
	ASSERT(pObject != NULL);
	ASSERT(pObject->ret_type() == DB_DIMSTYLETABREC);

	db_dimstyletabrec* pDimStyleRec = (db_dimstyletabrec*)pObject;

	OdDbDimStyleTableRecordPtr pOdDimStyle = OdDbDimStyleTableRecord::createObject();
	OdDbObjectId odDimStyleID = ReadWriteUtils::setHandle(m_pOdDatabase, pDimStyleRec, ownerId, pOdDimStyle);

	char dimBlkName[DB_ACADBUF];

	pDimStyleRec->get_3(dimBlkName, DB_ACADBUF - 1);
	pOdDimStyle->setDimpost(OdString(dimBlkName));

	pDimStyleRec->get_4(dimBlkName, DB_ACADBUF - 1);
	pOdDimStyle->setDimapost(OdString(dimBlkName));

	pDimStyleRec->get_5(dimBlkName, DB_ACADBUF - 1);
	if (dimBlkName[0] != '\0')
		pOdDimStyle->setDimblk(OdString(dimBlkName));

	pDimStyleRec->get_6(dimBlkName, DB_ACADBUF - 1);
	if (dimBlkName[0] != '\0')
		pOdDimStyle->setDimblk1(OdString(dimBlkName));

	pDimStyleRec->get_7(dimBlkName, DB_ACADBUF - 1);
	if (dimBlkName[0] != '\0')
		pOdDimStyle->setDimblk2(OdString(dimBlkName));

	char dimTxSty[DB_ACADBUF];
	pDimStyleRec->get_340name(dimTxSty, DB_ACADBUF - 1);
	if (dimTxSty[0] != '\0')
		pOdDimStyle->setDimtxsty(ReadWriteUtils::getTextStyleIDByName(m_pOdDatabase, OdString(dimTxSty)));

	double doubleVal;
    pDimStyleRec->get_40(&doubleVal);
	pOdDimStyle->setDimscale(doubleVal);
    pDimStyleRec->get_41(&doubleVal);
	pOdDimStyle->setDimasz(doubleVal);
    pDimStyleRec->get_42(&doubleVal);
	pOdDimStyle->setDimexo(doubleVal);
    pDimStyleRec->get_43(&doubleVal);
	pOdDimStyle->setDimdli(doubleVal);
    pDimStyleRec->get_44(&doubleVal);
	pOdDimStyle->setDimexe(doubleVal);
    pDimStyleRec->get_45(&doubleVal);
	pOdDimStyle->setDimrnd(doubleVal);
    pDimStyleRec->get_46(&doubleVal);
	pOdDimStyle->setDimdle(doubleVal);
    pDimStyleRec->get_47(&doubleVal);
	pOdDimStyle->setDimtp(doubleVal);
    pDimStyleRec->get_48(&doubleVal);
	pOdDimStyle->setDimtm(doubleVal);

	int intVal;
    pDimStyleRec->get_71(&intVal);
	pOdDimStyle->setDimtol((intVal == 1) ? true : false);
    pDimStyleRec->get_72(&intVal);
	pOdDimStyle->setDimlim((intVal == 1) ? true : false);
    pDimStyleRec->get_73(&intVal);
	pOdDimStyle->setDimtih((intVal == 1) ? true : false);
    pDimStyleRec->get_74(&intVal);
	pOdDimStyle->setDimtoh((intVal == 1) ? true : false);
    pDimStyleRec->get_75(&intVal);
	pOdDimStyle->setDimse1((intVal == 1) ? true : false);
    pDimStyleRec->get_76(&intVal);
	pOdDimStyle->setDimse2((intVal == 1) ? true : false);
    pDimStyleRec->get_77(&intVal);
	pOdDimStyle->setDimtad(intVal);
    pDimStyleRec->get_78(&intVal);
	pOdDimStyle->setDimzin(intVal);
	pDimStyleRec->get_79(&intVal);
	pOdDimStyle->setDimazin(intVal);
    pDimStyleRec->get_140(&doubleVal);
	pOdDimStyle->setDimtxt(doubleVal);
    pDimStyleRec->get_141(&doubleVal);
	pOdDimStyle->setDimcen(doubleVal);
    pDimStyleRec->get_142(&doubleVal);
	pOdDimStyle->setDimtsz(doubleVal);
    pDimStyleRec->get_143(&doubleVal);
	pOdDimStyle->setDimaltf(doubleVal);
    pDimStyleRec->get_144(&doubleVal);
	pOdDimStyle->setDimlfac(doubleVal);
    pDimStyleRec->get_145(&doubleVal);
	pOdDimStyle->setDimtvp(doubleVal);
    pDimStyleRec->get_146(&doubleVal);
	pOdDimStyle->setDimtfac(doubleVal);
    pDimStyleRec->get_147(&doubleVal);
	pOdDimStyle->setDimgap(doubleVal);
	pDimStyleRec->get_148(&doubleVal);
	pOdDimStyle->setDimaltrnd(doubleVal);
    pDimStyleRec->get_170(&intVal);
	pOdDimStyle->setDimalt((intVal == 1) ? true : false);
    pDimStyleRec->get_171(&intVal);
	pOdDimStyle->setDimaltd(intVal);
    pDimStyleRec->get_172(&intVal);
	pOdDimStyle->setDimtofl((intVal == 1) ? true : false);
    pDimStyleRec->get_173(&intVal);
	pOdDimStyle->setDimsah((intVal == 1) ? true : false);
    pDimStyleRec->get_174(&intVal);
	pOdDimStyle->setDimtix((intVal == 1) ? true : false);
    pDimStyleRec->get_175(&intVal);
	pOdDimStyle->setDimsoxd((intVal == 1) ? true : false);

	OdCmColor color;
    pDimStyleRec->get_176(&intVal);
	color.setColorIndex(intVal);
	pOdDimStyle->setDimclrd(color);
    pDimStyleRec->get_177(&intVal);
	color.setColorIndex(intVal);
	pOdDimStyle->setDimclre(color);
    pDimStyleRec->get_178(&intVal);
	color.setColorIndex(intVal);
	pOdDimStyle->setDimclrt(color);

	pDimStyleRec->get_179(&intVal);
	pOdDimStyle->setDimadec((intVal < 0) ? 0 : intVal);

	// Obsolete using dimlunits.
	pDimStyleRec->get_270(&intVal);
	pOdDimStyle->setDimlunit(intVal);
    pDimStyleRec->get_271(&intVal);
	pOdDimStyle->setDimdec(intVal);
    pDimStyleRec->get_272(&intVal);
	pOdDimStyle->setDimtdec(intVal);
    pDimStyleRec->get_273(&intVal);
	pOdDimStyle->setDimaltu(intVal);
    pDimStyleRec->get_274(&intVal);
	pOdDimStyle->setDimalttd(intVal);
    pDimStyleRec->get_275(&intVal);
	pOdDimStyle->setDimaunit(intVal);
	pDimStyleRec->get_276(&intVal);
	pOdDimStyle->setDimfrac(intVal);
	pDimStyleRec->get_277(&intVal);
	pOdDimStyle->setDimlunit(intVal);
	pDimStyleRec->get_278(&intVal);
	pOdDimStyle->setDimdsep(intVal);
    pDimStyleRec->get_280(&intVal);
	pOdDimStyle->setDimjust(intVal);
    pDimStyleRec->get_281(&intVal);
	pOdDimStyle->setDimsd1((intVal == 1) ? true : false);
    pDimStyleRec->get_282(&intVal);
	pOdDimStyle->setDimsd2((intVal == 1) ? true : false);
    pDimStyleRec->get_283(&intVal);
	pOdDimStyle->setDimtolj(intVal);
    pDimStyleRec->get_284(&intVal);
	pOdDimStyle->setDimtzin(doubleVal);
    pDimStyleRec->get_285(&intVal);
	pOdDimStyle->setDimaltz(intVal);
    pDimStyleRec->get_286(&intVal);
	pOdDimStyle->setDimalttz(intVal);
	// TODO MHB VERIFY
	pDimStyleRec->get_287(&intVal);
	pOdDimStyle->setDimatfit(intVal);

    pDimStyleRec->get_288(&intVal);
	pOdDimStyle->setDimupt((intVal == 1) ? true : false);
	// TODO MHB VERIFY
	pDimStyleRec->get_289(&intVal);
	pOdDimStyle->setDimatfit(intVal);

	pDimStyleRec->get_279(&intVal);
	pOdDimStyle->setDimtmove(intVal);
    pDimStyleRec->get_371(&intVal);
	pOdDimStyle->setDimlwd((OdDb::LineWeight)intVal);
    pDimStyleRec->get_372(&intVal);
	pOdDimStyle->setDimlwe((OdDb::LineWeight)intVal);

	appendData(pDimStyleRec, pOdDimStyle);

	return pOdDimStyle;
}

OdDbObjectPtr DWGdirectConverter::updateOdDbDimStyle(OdDbObjectId ownerId, db_handitem* pObject, OdDbObject* pExistingDimStyle)
{
	ASSERT(!m_pOdDatabase.isNull());
	ASSERT(pObject != NULL);
	ASSERT(pObject->ret_type() == DB_DIMSTYLETABREC);
	ASSERT(pExistingDimStyle != NULL);
	ASSERT(pExistingDimStyle->isKindOf(OdDbDimStyleTableRecord::desc()));

	db_dimstyletabrec* pDimStyleRec = (db_dimstyletabrec*)pObject;
	OdDbDimStyleTableRecordPtr pOdDimStyle = (OdDbDimStyleTableRecordPtr)pExistingDimStyle;

	char dimBlkName[DB_ACADBUF];

	pDimStyleRec->get_3(dimBlkName, DB_ACADBUF - 1);
	pOdDimStyle->setDimpost(OdString(dimBlkName));

	pDimStyleRec->get_4(dimBlkName, DB_ACADBUF - 1);
	pOdDimStyle->setDimapost(OdString(dimBlkName));

	pDimStyleRec->get_5(dimBlkName, DB_ACADBUF - 1);
	if (dimBlkName[0] != '\0')
		pOdDimStyle->setDimblk(OdString(dimBlkName));

	pDimStyleRec->get_6(dimBlkName, DB_ACADBUF - 1);
	if (dimBlkName[0] != '\0')
		pOdDimStyle->setDimblk1(OdString(dimBlkName));

	pDimStyleRec->get_7(dimBlkName, DB_ACADBUF - 1);
	if (dimBlkName[0] != '\0')
		pOdDimStyle->setDimblk2(OdString(dimBlkName));

	double doubleVal;
    pDimStyleRec->get_40(&doubleVal);
	pOdDimStyle->setDimscale(doubleVal);
    pDimStyleRec->get_41(&doubleVal);
	pOdDimStyle->setDimasz(doubleVal);
    pDimStyleRec->get_42(&doubleVal);
	pOdDimStyle->setDimexo(doubleVal);
    pDimStyleRec->get_43(&doubleVal);
	pOdDimStyle->setDimdli(doubleVal);
    pDimStyleRec->get_44(&doubleVal);
	pOdDimStyle->setDimexe(doubleVal);
    pDimStyleRec->get_45(&doubleVal);
	pOdDimStyle->setDimrnd(doubleVal);
    pDimStyleRec->get_46(&doubleVal);
	pOdDimStyle->setDimdle(doubleVal);
    pDimStyleRec->get_47(&doubleVal);
	pOdDimStyle->setDimtp(doubleVal);
    pDimStyleRec->get_48(&doubleVal);
	pOdDimStyle->setDimtm(doubleVal);

	int intVal;
    pDimStyleRec->get_71(&intVal);
	pOdDimStyle->setDimtol((intVal == 1) ? true : false);
    pDimStyleRec->get_72(&intVal);
	pOdDimStyle->setDimlim((intVal == 1) ? true : false);
    pDimStyleRec->get_73(&intVal);
	pOdDimStyle->setDimtih((intVal == 1) ? true : false);
    pDimStyleRec->get_74(&intVal);
	pOdDimStyle->setDimtoh((intVal == 1) ? true : false);
    pDimStyleRec->get_75(&intVal);
	pOdDimStyle->setDimse1((intVal == 1) ? true : false);
    pDimStyleRec->get_76(&intVal);
	pOdDimStyle->setDimse2((intVal == 1) ? true : false);
    pDimStyleRec->get_77(&intVal);
	pOdDimStyle->setDimtad(intVal);
    pDimStyleRec->get_78(&intVal);
	pOdDimStyle->setDimzin(intVal);
	pDimStyleRec->get_79(&intVal);
	pOdDimStyle->setDimazin(intVal);
    pDimStyleRec->get_140(&doubleVal);
	pOdDimStyle->setDimtxt(doubleVal);
    pDimStyleRec->get_141(&doubleVal);
	pOdDimStyle->setDimcen(doubleVal);
    pDimStyleRec->get_142(&doubleVal);
	pOdDimStyle->setDimtsz(doubleVal);
    pDimStyleRec->get_143(&doubleVal);
	pOdDimStyle->setDimaltf(doubleVal);
    pDimStyleRec->get_144(&doubleVal);
	pOdDimStyle->setDimlfac(doubleVal);
    pDimStyleRec->get_145(&doubleVal);
	pOdDimStyle->setDimtvp(doubleVal);
    pDimStyleRec->get_146(&doubleVal);
	pOdDimStyle->setDimtfac(doubleVal);
    pDimStyleRec->get_147(&doubleVal);
	pOdDimStyle->setDimgap(doubleVal);
	pDimStyleRec->get_148(&doubleVal);
	pOdDimStyle->setDimaltrnd(doubleVal);
    pDimStyleRec->get_170(&intVal);
	pOdDimStyle->setDimalt((intVal == 1) ? true : false);
    pDimStyleRec->get_171(&intVal);
	pOdDimStyle->setDimaltd(intVal);
    pDimStyleRec->get_172(&intVal);
	pOdDimStyle->setDimtofl((intVal == 1) ? true : false);
    pDimStyleRec->get_173(&intVal);
	pOdDimStyle->setDimsah((intVal == 1) ? true : false);
    pDimStyleRec->get_174(&intVal);
	pOdDimStyle->setDimtix((intVal == 1) ? true : false);
    pDimStyleRec->get_175(&intVal);
	pOdDimStyle->setDimsoxd((intVal == 1) ? true : false);

	OdCmColor color;
    pDimStyleRec->get_176(&intVal);
	color.setColorIndex(intVal);
	pOdDimStyle->setDimclrd(color);
    pDimStyleRec->get_177(&intVal);
	color.setColorIndex(intVal);
	pOdDimStyle->setDimclre(color);
    pDimStyleRec->get_178(&intVal);
	color.setColorIndex(intVal);
	pOdDimStyle->setDimclrt(color);

	pDimStyleRec->get_179(&intVal);
	pOdDimStyle->setDimadec((intVal < 0) ? 0 : intVal);

	// Obsolete using dimlunits.
	pDimStyleRec->get_270(&intVal);
	pOdDimStyle->setDimlunit(intVal);
    pDimStyleRec->get_271(&intVal);
	pOdDimStyle->setDimdec(intVal);
    pDimStyleRec->get_272(&intVal);
	pOdDimStyle->setDimtdec(intVal);
    pDimStyleRec->get_273(&intVal);
	pOdDimStyle->setDimaltu(intVal);
    pDimStyleRec->get_274(&intVal);
	pOdDimStyle->setDimalttd(intVal);
    pDimStyleRec->get_275(&intVal);
	pOdDimStyle->setDimaunit(intVal);
	pDimStyleRec->get_276(&intVal);
	pOdDimStyle->setDimfrac(intVal);
	pDimStyleRec->get_277(&intVal);
	pOdDimStyle->setDimlunit(intVal);
	pDimStyleRec->get_278(&intVal);
	pOdDimStyle->setDimdsep(intVal);
    pDimStyleRec->get_280(&intVal);
	pOdDimStyle->setDimjust(intVal);
    pDimStyleRec->get_281(&intVal);
	pOdDimStyle->setDimsd1((intVal == 1) ? true : false);
    pDimStyleRec->get_282(&intVal);
	pOdDimStyle->setDimsd2((intVal == 1) ? true : false);
    pDimStyleRec->get_283(&intVal);
	pOdDimStyle->setDimtolj(intVal);
    pDimStyleRec->get_284(&intVal);
	pOdDimStyle->setDimtzin(doubleVal);
    pDimStyleRec->get_285(&intVal);
	pOdDimStyle->setDimaltz(intVal);
    pDimStyleRec->get_286(&intVal);
	pOdDimStyle->setDimalttz(intVal);
	// TODO MHB VERIFY
	pDimStyleRec->get_287(&intVal);
	pOdDimStyle->setDimatfit(intVal);

    pDimStyleRec->get_288(&intVal);
	pOdDimStyle->setDimupt((intVal == 1) ? true : false);
	// TODO MHB VERIFY
	pDimStyleRec->get_289(&intVal);
	pOdDimStyle->setDimatfit(intVal);

	pDimStyleRec->get_279(&intVal);
	pOdDimStyle->setDimtmove(intVal);
    pDimStyleRec->get_371(&intVal);
	pOdDimStyle->setDimlwd((OdDb::LineWeight)intVal);
    pDimStyleRec->get_372(&intVal);
	pOdDimStyle->setDimlwe((OdDb::LineWeight)intVal);

	appendData(pDimStyleRec, pOdDimStyle);

	return pOdDimStyle;
}

OdDbObjectPtr DWGdirectConverter::createOdDbTextStyle(OdDbObjectId ownerId, db_handitem* pObject)
{
	ASSERT(!m_pOdDatabase.isNull());
	ASSERT(pObject != NULL);
	ASSERT(pObject->ret_type() == DB_STYLETABREC);

	db_styletabrec* pStyleRec = (db_styletabrec*)pObject;

	OdDbTextStyleTableRecordPtr pOdStyle = OdDbTextStyleTableRecord::createObject();
	OdDbObjectId odStyleID = ReadWriteUtils::setHandle(m_pOdDatabase, pStyleRec, ownerId, pOdStyle);

	int flags, textGen;
	double fixHeight, lastHeight, widthFactor, obliqueAngle;
	char fontName[DB_ACADBUF];
	char bigFontName[DB_ACADBUF];

	pStyleRec->get_70(&flags);
	pStyleRec->get_40(&fixHeight);
	pStyleRec->get_41(&widthFactor);
	pStyleRec->get_42(&lastHeight);
	pStyleRec->get_50(&obliqueAngle);
	pStyleRec->get_71(&textGen);
	pStyleRec->get_3(fontName, DB_ACADBUF-1);
	pStyleRec->get_4(bigFontName, DB_ACADBUF-1);

	pOdStyle->setIsBackwards((textGen & BIT2) == BIT2);
	pOdStyle->setIsUpsideDown((textGen & BIT4) == BIT4);
	pOdStyle->setIsShapeFile((flags & BIT1) == BIT1);
	pOdStyle->setIsVertical((flags & BIT4) == BIT4);
	pOdStyle->setObliquingAngle(obliqueAngle);
	pOdStyle->setPriorSize(lastHeight);
	pOdStyle->setTextSize(fixHeight);
	pOdStyle->setXScale(widthFactor);
	pOdStyle->setFileName(OdString(fontName));
	pOdStyle->setBigFontFileName(OdString(bigFontName));

	appendData(pStyleRec, pOdStyle);

	return pOdStyle;
}

OdDbObjectPtr DWGdirectConverter::updateOdDbTextStyle(OdDbObjectId ownerId, db_handitem* pObject, OdDbObject* pExistingStyle)
{
	ASSERT(!m_pOdDatabase.isNull());
	ASSERT(pObject != NULL);
	ASSERT(pObject->ret_type() == DB_STYLETABREC);
	ASSERT(pExistingStyle != NULL);
	ASSERT(pExistingStyle->isKindOf(OdDbTextStyleTableRecord::desc()));

	db_styletabrec* pStyleRec = (db_styletabrec*)pObject;
	OdDbTextStyleTableRecordPtr pOdStyle = (OdDbTextStyleTableRecordPtr)pExistingStyle;

	int flags, textGen;
	double fixHeight, lastHeight, widthFactor, obliqueAngle;
	char fontName[DB_ACADBUF];
	char bigFontName[DB_ACADBUF];

	pStyleRec->get_70(&flags);
	pStyleRec->get_40(&fixHeight);
	pStyleRec->get_41(&widthFactor);
	pStyleRec->get_42(&lastHeight);
	pStyleRec->get_50(&obliqueAngle);
	pStyleRec->get_71(&textGen);
	pStyleRec->get_3(fontName, DB_ACADBUF-1);
	pStyleRec->get_4(bigFontName, DB_ACADBUF-1);

	pOdStyle->setIsBackwards((textGen & BIT2) == BIT2);
	pOdStyle->setIsUpsideDown((textGen & BIT4) == BIT4);
	pOdStyle->setIsShapeFile((flags & BIT1) == BIT1);
	pOdStyle->setIsVertical((flags & BIT4) == BIT4);
	pOdStyle->setObliquingAngle(obliqueAngle);
	pOdStyle->setPriorSize(lastHeight);
	pOdStyle->setTextSize(fixHeight);
	pOdStyle->setXScale(widthFactor);
	pOdStyle->setFileName(OdString(fontName));
	pOdStyle->setBigFontFileName(OdString(bigFontName));

	appendData(pStyleRec, pOdStyle);

	return pOdStyle;
}

OdDbEntityPtr DWGdirectConverter::makePolyFaceMesh(db_handitem* pEntity, OdDbObjectId ownerId)
{
	ASSERT(pEntity != NULL && pEntity->ret_type() == DB_POLYLINE);

	db_polyline* pPline = (db_polyline*)pEntity;

	OdDbBlockTableRecordPtr ownerPtr = ownerId.safeOpenObject(OdDb::kForWrite);

	OdDbPolyFaceMeshPtr pOdDbPolyline = OdDbPolyFaceMesh::createObject();
	OdDbObjectId idBefore = ReadWriteUtils::setHandle(m_pOdDatabase, pPline, ownerId, pOdDbPolyline);
	OdDbObjectId idAfter = ownerPtr->appendOdDbEntity(pOdDbPolyline);
	ASSERT(idBefore == idAfter);

	finalize(pPline, pOdDbPolyline);

	db_handitem* pVertex = pEntity->next;
	while (pVertex != NULL && pVertex->ret_type() != DB_SEQEND)
	{
		int flags;
		pVertex->get_70(&flags);
		if ((flags & IC_VERTEX_FACELIST) && (flags & IC_VERTEX_3DMESHVERT))
		{
			OdDbPolyFaceMeshVertexPtr pOdDbVertex = (OdDbPolyFaceMeshVertexPtr)createOdDbVertex(1, pVertex);
			ASSERT(!pOdDbVertex.isNull());

			OdDbObjectId before = ReadWriteUtils::setHandle(m_pOdDatabase, pVertex, pOdDbPolyline->id(), pOdDbVertex);

			finalize(pVertex, pOdDbVertex);
			OdDbObjectId after = pOdDbPolyline->appendVertex(pOdDbVertex);
			ASSERT(before == after);
		}

		else if((flags & IC_VERTEX_FACELIST) && !(flags & IC_VERTEX_3DMESHVERT))
		{
			// face record.
			OdDbFaceRecordPtr pFr = OdDbFaceRecord::createObject();
			ASSERT(!pFr.isNull());

			OdDbObjectId before = ReadWriteUtils::setHandle(m_pOdDatabase, pVertex, pOdDbPolyline->id(), pFr);

			finalize(pVertex, pFr);
			OdDbObjectId after = pOdDbPolyline->appendFaceRecord(pFr);
			ASSERT(before == after);

			int val;
			pVertex->get_71(&val);
			pFr->setVertexAt(0, val);
			pVertex->get_72(&val);
			pFr->setVertexAt(1, val);
			pVertex->get_73(&val);
			pFr->setVertexAt(2, val);
			pVertex->get_74(&val);
			pFr->setVertexAt(3, val);

			int color;
			pVertex->get_62(&color);
			pFr->setColorIndex(color);

		}
		else
		{
			ASSERT(!"Unknown vertex type");
		}

		pVertex = pVertex->next;
	}

	int color;
	pPline->get_62(&color);
	pOdDbPolyline->setColorIndex((OdUInt16)color);

	return pOdDbPolyline;
}

OdDbEntityPtr DWGdirectConverter::make3dPolyline(db_handitem* pEntity, OdDbObjectId ownerId)
{
	ASSERT(pEntity != NULL && pEntity->ret_type() == DB_POLYLINE);

	db_polyline* pPline = (db_polyline*)pEntity;

	OdDbBlockTableRecordPtr ownerPtr = ownerId.safeOpenObject(OdDb::kForWrite);

	OdDb3dPolylinePtr pOdDbPolyline = OdDb3dPolyline::createObject();
	OdDbObjectId idBefore = ReadWriteUtils::setHandle(m_pOdDatabase, pPline, ownerId, pOdDbPolyline);
	OdDbObjectId idAfter = ownerPtr->appendOdDbEntity(pOdDbPolyline);
	ASSERT(idBefore == idAfter);

	finalize(pPline, pOdDbPolyline);

	db_handitem* pVertex = pEntity->next;
	while (pVertex != NULL && pVertex->ret_type() != DB_SEQEND)
	{
		OdDb3dPolylineVertexPtr pOdDbVertex = (OdDb3dPolylineVertexPtr)createOdDbVertex(2, pVertex);
		ASSERT(!pOdDbVertex.isNull());
		OdDbObjectId before = ReadWriteUtils::setHandle(m_pOdDatabase, pVertex, pOdDbPolyline->id(), pOdDbVertex);

		finalize(pVertex, pOdDbVertex);
		OdDbObjectId after = pOdDbPolyline->appendVertex(pOdDbVertex);
		ASSERT(before == after);

		pVertex = pVertex->next;
	}

	int flags;
	pPline->get_70(&flags);

	if ((flags & BIT1) == BIT1)
		pOdDbPolyline->makeClosed();
	else
		pOdDbPolyline->makeOpen();

	pPline->get_75(&flags);
	pOdDbPolyline->setPolyType((OdDb::Poly3dType)flags);

	int color;
	pPline->get_62(&color);
	pOdDbPolyline->setColorIndex((OdUInt16)color);

	return pOdDbPolyline;
}

OdDbEntityPtr DWGdirectConverter::makeSplineFitPolyline(db_handitem* pEntity, OdDbObjectId ownerId)
{
	ASSERT(pEntity != NULL && pEntity->ret_type() == DB_POLYLINE);

	db_polyline* pPline = (db_polyline*)pEntity;

	OdDbBlockTableRecordPtr ownerPtr = ownerId.safeOpenObject(OdDb::kForWrite);

	OdDb2dPolylinePtr pOdDbPolyline = OdDb2dPolyline::createObject();
	OdDbObjectId idBefore = ReadWriteUtils::setHandle(m_pOdDatabase, pPline, ownerId, pOdDbPolyline);
	OdDbObjectId idAfter = ownerPtr->appendOdDbEntity(pOdDbPolyline);
	ASSERT(idBefore == idAfter);

	finalize(pPline, pOdDbPolyline);

	int flags;
	db_handitem* pVertex = pEntity->next;
	while (pVertex != NULL && pVertex->ret_type() != DB_SEQEND)
	{
		OdDb2dVertexPtr pOdDbVertex = (OdDb2dVertexPtr)createOdDbVertex(3, pVertex);
		ASSERT(!pOdDbVertex.isNull());
		OdDbObjectId before = ReadWriteUtils::setHandle(m_pOdDatabase, pVertex, pOdDbPolyline->id(), pOdDbVertex);

		finalize(pVertex, pOdDbVertex);
		OdDbObjectId after = pOdDbPolyline->appendVertex(pOdDbVertex);
		ASSERT(before == after);

		pVertex = pVertex->next;
	}

	pPline->get_70(&flags);

	if ((flags & BIT1) == BIT1)
		pOdDbPolyline->makeClosed();
	else
		pOdDbPolyline->makeOpen();

	pPline->get_75(&flags);
	pOdDbPolyline->setPolyType((OdDb::Poly2dType)flags);

	int color;
	pPline->get_62(&color);
	pOdDbPolyline->setColorIndex((OdUInt16)color);

	double doubleVal;
	pPline->get_39(&doubleVal);
	pOdDbPolyline->setThickness(doubleVal);

	pOdDbPolyline->setElevation(pPline->ret_elev());

	sds_point point;
	pPline->get_210(point);
	OdGeVector3d normal(point[0], point[1], point[2]);
	pOdDbPolyline->transformBy(OdGeMatrix3d::planeToWorld(normal));

	return pOdDbPolyline;
}

OdDbEntityPtr DWGdirectConverter::makePolygonMesh(db_handitem* pEntity, OdDbObjectId ownerId)
{
	ASSERT(pEntity != NULL && pEntity->ret_type() == DB_POLYLINE);

	db_polyline* pPline = (db_polyline*)pEntity;

	OdDbBlockTableRecordPtr ownerPtr = ownerId.safeOpenObject(OdDb::kForWrite);

	OdDbPolygonMeshPtr pOdDbPolyline = OdDbPolygonMesh::createObject();
	OdDbObjectId idBefore = ReadWriteUtils::setHandle(m_pOdDatabase, pPline, ownerId, pOdDbPolyline);
	OdDbObjectId idAfter = ownerPtr->appendOdDbEntity(pOdDbPolyline);
	ASSERT(idBefore == idAfter);

	finalize(pPline, pOdDbPolyline);

	db_handitem* pVertex = pEntity->next;
	while (pVertex != NULL && pVertex->ret_type() != DB_SEQEND)
	{
		OdDbPolygonMeshVertexPtr pOdDbVertex = (OdDbPolygonMeshVertexPtr)createOdDbVertex(0, pVertex);
		ASSERT(!pOdDbVertex.isNull());
		OdDbObjectId before = ReadWriteUtils::setHandle(m_pOdDatabase, pVertex, pOdDbPolyline->id(), pOdDbVertex);

		finalize(pVertex, pOdDbVertex);
		OdDbObjectId after = pOdDbPolyline->appendVertex(pOdDbVertex);
		ASSERT(before == after);

		pVertex = pVertex->next;
	}

	int intVal;
	pPline->get_71(&intVal);
	pOdDbPolyline->setMSize(intVal);

	pPline->get_72(&intVal);
	pOdDbPolyline->setNSize(intVal);

	pPline->get_73(&intVal);
	pOdDbPolyline->setMSurfaceDensity(intVal);

	pPline->get_74(&intVal);
	pOdDbPolyline->setNSurfaceDensity(intVal);

	pPline->get_75(&intVal);
	pOdDbPolyline->setPolyMeshType((OdDb::PolyMeshType)intVal);

	pPline->get_70(&intVal);
	bool bIsClosedInM = ((intVal & BIT1) == BIT1);
	bool bisClosedInN = ((intVal & BIT20) == BIT20);

	if (bIsClosedInM)
		pOdDbPolyline->makeMClosed();
	else
		pOdDbPolyline->makeMOpen();

	if (bisClosedInN)
		pOdDbPolyline->makeNClosed();
	else
		pOdDbPolyline->makeNOpen();

	int color;
	pPline->get_62(&color);
	pOdDbPolyline->setColorIndex((OdUInt16)color);

	return pOdDbPolyline;

}

OdGeCurve2d* DWGdirectConverter::makeHatchLineEdge(db_hatch* hatchEnt, int loop, int edgeIndex)
{
	ASSERT(hatchEnt != NULL);

	double vert1[2], vert2[2];
	hatchEnt->get_pathedgeline(loop, edgeIndex, &vert1[0], &vert1[1], &vert2[0], &vert2[1]);

	OdGeLineSeg2d * pLine = new OdGeLineSeg2d(OdGePoint2d(vert1[0], vert1[1]), OdGePoint2d(vert2[0], vert2[1]));
	return (OdGeCurve2d*)pLine;
}

OdGeCurve2d* DWGdirectConverter::makeHatchArcEdge(db_hatch* hatchEnt, int loop, int edgeIndex)
{
	ASSERT(hatchEnt != NULL);


	int isccw;
	double center[2];
	double radius, startAngle, endAngle;

	hatchEnt->get_pathedgearc(loop, edgeIndex, &center[0], &center[1], &radius, &startAngle,
									  &endAngle, &isccw);

	OdGeCircArc2d* pArc = new OdGeCircArc2d(OdGePoint2d(center[0], center[1]), 
											radius, startAngle, endAngle, OdGeVector2d::kXAxis, 
											(isccw == 1) ? false : true);
	return (OdGeCurve2d*)pArc;
}

OdGeCurve2d* DWGdirectConverter::makeHatchEllipticArcEdge(db_hatch* hatchEnt, int loop, int edgeIndex)
{
	ASSERT(hatchEnt != NULL);


	double center[2], axisendpt[2];
	double minmajratio, startangle, endangle;
	int isccw;

	hatchEnt->get_pathedgeellarc(loop, edgeIndex, &center[0], &center[1], &axisendpt[0], &axisendpt[1],
										 &minmajratio, &startangle, &endangle, &isccw);

	OdGePoint2d centerPt(center[0], center[1]);

	OdGeVector2d majAxis(axisendpt[0], axisendpt[1]);
	double majorRadius = majAxis.length();
	double minorRadius = majorRadius * minmajratio;

	// Get the minor axis vector Use GEO to do the heavy lifting
	C3Point ptMinor = C3Point(axisendpt[0], axisendpt[1]);
	ptMinor.TurnLeft();
	ptMinor *= minmajratio;

	OdGeVector2d minAxis(ptMinor.X(), ptMinor.Y());

	while (endangle < startangle)
		endangle += Oda2PI;

	OdGeEllipArc2d* pArc = new OdGeEllipArc2d(OdGePoint2d(center[0], center[1]), 
											  majAxis, minAxis,
											  majorRadius, minorRadius,
											  startangle, endangle);
	return (OdGeCurve2d*)pArc;
}

OdGeCurve2d* DWGdirectConverter::makeHatchSplineEdge(db_hatch* hatchEnt, int loop, int edgeIndex)
{
	ASSERT(hatchEnt != NULL);

	int degree, numknots, numctrlpts, rational, periodic;
	double *knots;

	hatchEnt->get_pathedgespline(loop, edgeIndex, &rational, &periodic, &degree, &numknots,
										 &numctrlpts, &knots);

	OdGeKnotVector odKnots(numknots, knots);

	OdGeDoubleArray  weights;
	OdGePoint2dArray ctrlPts;
	for (int i = 0; i < numctrlpts; i++)
	{
		double ctrlpt[2];
		hatchEnt->get_pathedgesplinecontrol(loop, edgeIndex, i, &ctrlpt[0], &ctrlpt[1]);

		ctrlPts.insert(ctrlPts.end(), OdGePoint2d(ctrlpt[0], ctrlpt[1]));

		double weight = 0.0;
		if (rational) 
		{ 
			double weight;

			hatchEnt->get_pathedgesplineweight(loop, edgeIndex, i, &weight);			
			weights.insert(weights.end(), weight);
		}
	}

	OdGeNurbCurve2d* pNurb = new OdGeNurbCurve2d(degree, odKnots, ctrlPts, weights, (periodic == 1) ? true : false);
	
	ctrlPts.clear();
	weights.clear();

	return (OdGeCurve2d*)pNurb;
}

void DWGdirectConverter::makeHatchPolylineEdge(db_hatch* hatchEnt, int loop, OdGePoint2dArray& vertices, OdGeDoubleArray& bulges)
{
	ASSERT(hatchEnt != NULL);

	int hasBulges, closed, numVerts;
	hatchEnt->get_pathedgepline(loop, &closed, &hasBulges, &numVerts);

	for (int i = 0; i < numVerts; i++)
	{	
		double vertX, vertY, bulge;
		hatchEnt->get_pathedgeplinevert(loop, i, &vertX, &vertY, &bulge);
		vertices.insert(vertices.end(), OdGePoint2d(vertX, vertY));
		bulges.insert(bulges.end(), bulge);
	}

	return;
}

int DWGdirectConverter::makeHatchBoundaryAssociations(db_hatch* hatchEnt, OdDbHatchPtr& pOdDbHatch)
{
	int retVal = 1;
	ASSERT(hatchEnt != NULL);
	ASSERT(!pOdDbHatch.isNull());

	char hand[17];
	int pathCount;
	db_objhandle objHand;
	hatchEnt->get_91(&pathCount);

	OdDbObjectId boundaryId;
	OdDbObjectIdArray dbObjIds;

	db_hatchboundobjs* objects = NULL;
	db_hatchboundobjslink* boundObjs;

	int assocObjs = 0;
	for (int i = 0; i < pathCount; i++)
	{
		hatchEnt->get_pathboundobj(i, &boundObjs);
		if (boundObjs == NULL)
			continue;

		// Get the handle from each entity in boundObjs
		boundObjs->getNext(&objects, &objHand);

		// Insert the ID of the entity that has this handle
		// into the list of bounary objects array dbObjIds.
		objHand.ToAscii(hand);
		boundaryId = m_pOdDatabase->getOdDbObjectId(OdDbHandle(hand));
		if (boundaryId.isValid() && !boundaryId.isErased() && !boundaryId.isNull())
			dbObjIds.insert(dbObjIds.end(), boundaryId);

		// Do the rest too.
		while (boundObjs->getNext(&objects, &objHand) == true)
		{
			objHand.ToAscii(hand);
			boundaryId = m_pOdDatabase->getOdDbObjectId(hand);
			if (boundaryId.isNull())	// Bail at the first sign of problems.
				return 0;

			ASSERT(!boundaryId.isNull() && boundaryId.safeOpenObject()->isKindOf(OdDbEntity::desc()));
			dbObjIds.insert(dbObjIds.end(), boundaryId);
		}

		if (dbObjIds.size())
		{
			pOdDbHatch->setAssocObjIdsAt(assocObjs, dbObjIds);
			assocObjs++;
			dbObjIds.clear();
		}
	}
		
	return retVal;
}

void DWGdirectConverter::makeHatchPatternDefinition(db_hatch* pHatch, OdDbHatchPtr& pOdDbHatch)
{
	ASSERT(pHatch != NULL);
	ASSERT(!pOdDbHatch.isNull());

	int type;
	pHatch->get_76(&type);

	int patternLineCount;
	pHatch->get_78(&patternLineCount);
	
	char* patName;
	pHatch->get_2(&patName);

	OdHatchPatternManagerPtr patternManager = m_pExServices->patternManager();

	int numDashes;
	double angle,patternthrough[2],patternoffset[2],*dashs;
	OdHatchPattern pattern;
	for (int i = 0; i < patternLineCount; i++)
	{
		pHatch->get_patline(i,&angle,
				 &patternthrough[0],&patternthrough[1],
				 &patternoffset[0],&patternoffset[1],
				 &numDashes,&dashs);

#ifdef _DEBUG_MHB
		char tmpbuf[512];
		sprintf(tmpbuf, "(53 %f),(43 %f),(44 %f),(45 %f),(46 %f),(70 %d)\n", 
			angle,patternthrough[0],patternthrough[1],
			patternoffset[0],patternoffset[1],numDashes);
		OutputDebugString(tmpbuf);
#endif
		OdHatchPatternLine pat;
		pat.m_basePoint = OdGePoint2d(patternthrough[0], patternthrough[1]);
		OdGeVector2d offset = OdGeVector2d(patternoffset[0], patternoffset[1]);
		offset;
		pat.m_patternOffset = offset;
		for (int j = 0; j < numDashes; j++)
		{
			pat.m_dashes.insert(pat.m_dashes.end(),dashs[j]);
#ifdef _DEBUG_MHB
		tmpbuf[512];
		sprintf(tmpbuf,"(49 %f)\n", dashs[j]);
		OutputDebugString(tmpbuf);
#endif
		}
		pat.m_dLineAngle = angle;

		pattern.insert(pattern.end(), pat);
	}

	odDbSetRawHatchPattern(*pOdDbHatch, (OdDbHatch::HatchPatternType) type,
						   OdString(patName), pattern);

	return;
}

OdDbEntityPtr DWGdirectConverter::makeRotateDimension(db_dimension* pDimension)
{
	ASSERT(pDimension != NULL);

	OdDbRotatedDimensionPtr pDim = OdDbRotatedDimension::createObject();
	ASSERT(!pDim.isNull());

	sds_point point;
	pDimension->get_10(point);
	pDim->setDimLinePoint(OdGePoint3d(point[0], point[1], point[2]));

	pDimension->get_11(point);
	pDim->setTextPosition(OdGePoint3d(point[0], point[1], point[2]));

	pDimension->get_13(point);
	pDim->setXLine1Point(OdGePoint3d(point[0], point[1], point[2]));

	pDimension->get_14(point);
	pDim->setXLine2Point(OdGePoint3d(point[0], point[1], point[2]));

	double doubleVal;
	pDimension->get_50(&doubleVal);
	pDim->setRotation(doubleVal);

	pDimension->get_52(&doubleVal);
	pDim->setOblique(doubleVal);

	return pDim;
}

OdDbEntityPtr DWGdirectConverter::makeAlignedDimension(db_dimension* pDimension)
{
	ASSERT(pDimension != NULL);

	OdDbAlignedDimensionPtr pDim = OdDbAlignedDimension::createObject();
	ASSERT(!pDim.isNull());

	sds_point point;
	pDimension->get_10(point);
	pDim->setDimLinePoint(OdGePoint3d(point[0], point[1], point[2]));

	pDimension->get_11(point);
	pDim->setTextPosition(OdGePoint3d(point[0], point[1], point[2]));

	pDimension->get_13(point);
	pDim->setXLine1Point(OdGePoint3d(point[0], point[1], point[2]));

	pDimension->get_14(point);
	pDim->setXLine2Point(OdGePoint3d(point[0], point[1], point[2]));

	double doubleVal;
	pDimension->get_52(&doubleVal);
	pDim->setOblique(doubleVal);

	return pDim;
}

OdDbEntityPtr DWGdirectConverter::makeAngularDimension(db_dimension* pDimension)
{
	ASSERT(pDimension != NULL);

	OdDb2LineAngularDimensionPtr pDim = OdDb2LineAngularDimension::createObject();
	ASSERT(!pDim.isNull());

	sds_point point;
	pDimension->get_10(point);
	pDim->setXLine2End(OdGePoint3d(point[0], point[1], point[2]));

	pDimension->get_13(point);
	pDim->setXLine1Start(OdGePoint3d(point[0], point[1], point[2]));

	pDimension->get_14(point);
	pDim->setXLine1End(OdGePoint3d(point[0], point[1], point[2]));

	pDimension->get_15(point);
	pDim->setXLine2Start(OdGePoint3d(point[0], point[1], point[2]));

	pDimension->get_16(point);
	pDim->setArcPoint(OdGePoint3d(point[0], point[1], point[2]));

	return pDim;
}

OdDbEntityPtr DWGdirectConverter::makeDiametricDimension(db_dimension* pDimension)
{
	ASSERT(pDimension != NULL);

	OdDbDiametricDimensionPtr pDim = OdDbDiametricDimension::createObject();
	ASSERT(!pDim.isNull());

	sds_point point;
	pDimension->get_10(point);
	pDim->setFarChordPoint(OdGePoint3d(point[0], point[1], point[2]));

	pDimension->get_15(point);
	pDim->setChordPoint(OdGePoint3d(point[0], point[1], point[2]));

	double doubleVal;
	pDimension->get_40(&doubleVal);
	pDim->setLeaderLength(doubleVal);

	return pDim;
}

OdDbEntityPtr DWGdirectConverter::makeRadialDimension(db_dimension* pDimension)
{
	ASSERT(pDimension != NULL);

	OdDbRadialDimensionPtr pDim = OdDbRadialDimension::createObject();
	ASSERT(!pDim.isNull());

	sds_point point;
	pDimension->get_10(point);
	pDim->setCenter(OdGePoint3d(point[0], point[1], point[2]));

	pDimension->get_15(point);
	pDim->setChordPoint(OdGePoint3d(point[0], point[1], point[2]));

	double doubleVal;
	pDimension->get_40(&doubleVal);
	pDim->setLeaderLength(doubleVal);

	return pDim;
}

OdDbEntityPtr DWGdirectConverter::make3PointAngularDimension(db_dimension* pDimension)
{
	ASSERT(pDimension != NULL);

	OdDb3PointAngularDimensionPtr pDim = OdDb3PointAngularDimension::createObject();
	ASSERT(!pDim.isNull());

	sds_point point;
	pDimension->get_10(point);
	pDim->setArcPoint(OdGePoint3d(point[0], point[1], point[2]));

	pDimension->get_13(point);
	pDim->setXLine1Point(OdGePoint3d(point[0], point[1], point[2]));

	pDimension->get_14(point);
	pDim->setXLine2Point(OdGePoint3d(point[0], point[1], point[2]));

	pDimension->get_15(point);
	pDim->setCenterPoint(OdGePoint3d(point[0], point[1], point[2]));

	return pDim;
}

OdDbEntityPtr DWGdirectConverter::makeOrdinateDimension(db_dimension* pDimension)
{
	ASSERT(pDimension != NULL);

	OdDbOrdinateDimensionPtr pDim = OdDbOrdinateDimension::createObject();
	ASSERT(!pDim.isNull());

	int flags = 0;
	pDimension->get_70(&flags);

	sds_point point;
	pDimension->get_10(point);
	pDim->setOrigin(OdGePoint3d(point[0], point[1], point[2]));

	pDimension->get_13(point);
	pDim->setDefiningPoint(OdGePoint3d(point[0], point[1], point[2]));

	pDimension->get_14(point);
	pDim->setLeaderEndPoint(OdGePoint3d(point[0], point[1], point[2]));

	if (flags & BIT40)
		pDim->useXAxis();
	else
		pDim->useYAxis();

	return pDim;
}

void DWGdirectConverter::finalize(db_handitem* pEntity, OdDbEntity* pEnt)
{
	ASSERT(pEntity != NULL);
	ASSERT(pEnt != NULL);
	
	if (pEntity == NULL || pEnt == NULL)
		return;

	// layer
	char* layerName;
	pEntity->get_8(&layerName);
	pEnt->setLayer(OdString(layerName));

	// linetype
	char* lineType;
	pEntity->get_6(&lineType);
	pEnt->setLinetype(OdString(lineType));

	// linetype scale
	double doubleVal;
	pEntity->get_48(&doubleVal);
	pEnt->setLinetypeScale(doubleVal);

	int intVal;
	pEntity->get_60(&intVal);
	pEnt->setVisibility((OdDb::Visibility)intVal);

	// linetype weight
	pEntity->get_370(&intVal);
	pEnt->setLineWeight((OdDb::LineWeight)intVal);

	pEntity->get_380(&intVal);
	if (intVal > 2)
	{
		db_handitem* plotStyle;
		pEntity->get_390(&plotStyle);

		try
		{
			if (plotStyle != NULL)
			{
				char* styleName;
				plotStyle->get_1(&styleName);
				if (styleName != NULL)
				{
					pEntity->set_380(0);
					pEnt->setPlotStyleName(OdString(styleName));
				}
			}
		}
		catch(...)
		{
			pEnt->setPlotStyleName(OdString());
		}
	}

	appendData(pEntity, pEnt);

	return;
}

void DWGdirectConverter::appendData(db_handitem* pEntity, OdDbObject* pEnt)
{
	ASSERT(pEntity != NULL);
	ASSERT(pEnt != NULL);
	
	if (pEntity == NULL || pEnt == NULL)
		return;
	
	// Extension Dictionaries
	db_hireflink* extDict = pEntity->ret_xdict();
	if (extDict != NULL)
	{
		pEnt->createExtensionDictionary();
		OdDbObjectId extDictID = pEnt->extensionDictionary();
		OdDbDictionaryPtr extDictPtr = pEnt->extensionDictionary().safeOpenObject(OdDb::kForWrite);

		int type;
		db_handitem* ent;
		db_objhandle handle;

		db_handitem	*pFirstObject, *pLastObject, *pObject = NULL;
		m_pICdwg->get_objllends(&pFirstObject, &pLastObject);

		extDict->get_data(&type, &handle, &ent, m_pICdwg);
		// Looking for the dictionary with its handle == this extension dictionaries handle.
		for(pObject = pFirstObject; pObject != NULL; pObject = pObject->next)
		{
			if(pObject->ret_type() != DB_DICTIONARY)
				continue;
			if(pObject->RetHandle() == handle)
				break;
		}

		ASSERT(pObject != NULL && pObject->ret_type() == DB_DICTIONARY);
		if (pObject != NULL)
		{
			db_dictionary* pDict = (db_dictionary*)pObject;

			db_dictionaryrec *beginDictRecord= NULL;
			db_dictionaryrec *endDictRecord = NULL;

			pDict->get_recllends(&beginDictRecord, &endDictRecord);

			db_dictionaryrec *walkingDict = beginDictRecord;
			while (walkingDict != NULL)
			{
				OdString recordName = OdString(walkingDict->name);

				db_hireflink* recordLink = walkingDict->hiref;
				while (recordLink != NULL)
				{
					processDictionaryRecord(extDictPtr, recordLink, recordName);
					recordLink = recordLink->next;
				}
				walkingDict = walkingDict->next;
			}
		}
	}

	// Extended Entity data (All Apps)
	struct resbuf* extData = pEntity->get_eed(NULL, NULL);
	if (extData != NULL)
	{
		pEnt->setXData(ReadWriteUtils::getOdResBufPtrFromSdsResBufPtr(m_pOdDatabase, extData));
		sds_relrb(extData);
	}

	// Reactors
	ReactorSubject* reactors = pEntity->getReactor();
	int reactorCount = reactors->GetCountNotDeleted(m_pICdwg);

	if (reactorCount)
	{
		reactors->ResetIterator();

		char entHand[17];
		pEntity->RetHandlePtr()->ToAscii(entHand);

		char assocHand[17];
		db_objhandle entHandle;
		// For persistant reactors, we save off the handle of the entity receiving the reactor,
		// and the handle of the entity to react to in the persistantReactorList collection.
		// When this class's destructor is called then all the reactors are hooked up. This is
		// done in the destructor to ensure all the entities have been converted and added to the
		// OdDbDatabase being exported so the handles can be converted to valid OdDbEntity IDs.
		while (reactors->GetNextHandle(entHandle))
		{
			entHandle.ToAscii(assocHand);
			m_persistantReactorList.insert(m_persistantReactorList.end(), handlePairs(OdDbHandle(entHand), OdDbHandle(assocHand)));
		}
	}
	return;
}

void DWGdirectConverter::processDictionaryRecord(OdDbDictionary* pOwnerDict, db_hireflink* recordLink, OdString recordName)
{
	ASSERT(pOwnerDict != NULL);
	ASSERT(recordLink != NULL);

	if (recordLink == NULL)
		return;

	db_handitem* linkEntity = recordLink->ret_hip(m_pICdwg);
	ASSERT(linkEntity != NULL);
	if (linkEntity == NULL)
		return;

	switch (linkEntity->ret_type())
	{
	case DB_SORTENTSTABLE:
		{
		OdDbObjectId id = createOdDbObject(pOwnerDict->objectId(), linkEntity, recordName);

		if (id.isNull())
			return;
		OdDbObjectPtr pSortEnts = id.safeOpenObject(OdDb::kForWrite);
		OdDbObjectId odSortentsAfterId = pOwnerDict->setAt("ACAD_SORTENTS", pSortEnts);
		}
	break;
	case DB_DIMASSOC:
		{
		OdDbObjectId id = createOdDbObject(pOwnerDict->objectId(), linkEntity, recordName);

		if (id.isNull())
			return;
		OdDbObjectPtr pDimAssoc = id.safeOpenObject(OdDb::kForWrite);
		OdDbObjectId odDimAssocAfterId = pOwnerDict->setAt("ACAD_DIMASSOC", pDimAssoc);
		}
	break;
	case DB_XRECORD:
		{
		OdDbObjectId id = createOdDbObject(pOwnerDict->objectId(), linkEntity, recordName);

		if (id.isNull())
			return;
		OdDbObjectPtr pXRecord = id.safeOpenObject(OdDb::kForWrite);
		OdDbObjectId odDimAssocAfterId = pOwnerDict->setAt(recordName, pXRecord);
		}
	break;
	case DB_ACAD_PROXY_OBJECT:
		{
		OdDbObjectId id = createOdDbObject(pOwnerDict->objectId(), linkEntity, recordName);

		if (id.isNull())
			return;
		OdDbObjectPtr pProxyObject = id.safeOpenObject(OdDb::kForWrite);
		OdDbObjectId odProxyObjectAfterId = pOwnerDict->setAt(recordName, pProxyObject);
		}
		break;
	case DB_DICTIONARY:
		{
		OdDbDictionaryPtr pNewDictionary = OdDbDictionary::createObject();
		ASSERT(!pNewDictionary.isNull());

		OdDbObjectId idBefore = ReadWriteUtils::setHandle(m_pOdDatabase, linkEntity, pOwnerDict->objectId(), pNewDictionary);

		int ownerFlag;
		((db_dictionary*)linkEntity)->get_280(&ownerFlag);
		((OdDbDictionaryPtr)pNewDictionary)->setTreatElementsAsHard((ownerFlag == 1) ? true : false);

		int mergeStyle;
		((db_dictionary*)linkEntity)->get_281(&mergeStyle);
		((OdDbDictionaryPtr)pNewDictionary)->setMergeStyle((OdDb::DuplicateRecordCloning)mergeStyle);

		OdDbObjectId odProxyObjectAfterId = pOwnerDict->setAt(recordName, pNewDictionary);

		db_dictionary* pDict = (db_dictionary*)linkEntity;

			db_dictionaryrec *beginDictRecord= NULL;
			db_dictionaryrec *endDictRecord = NULL;

			pDict->get_recllends(&beginDictRecord, &endDictRecord);

			db_dictionaryrec *walkingDict = beginDictRecord;
			while (walkingDict != NULL)
			{
				OdString recordName = OdString(walkingDict->name);

				db_hireflink* recordLink = walkingDict->hiref;
				while (recordLink != NULL)
				{
					processDictionaryRecord(pNewDictionary, recordLink, recordName);
					recordLink = recordLink->next;
				}
				walkingDict = walkingDict->next;
			}
		}
		break;
	case DB_SPATIAL_FILTER:
		{
		OdDbObjectId id = createOdDbObject(pOwnerDict->objectId(), linkEntity, recordName);

		if (id.isNull())
			return;
		OdDbObjectPtr pFilter = id.safeOpenObject(OdDb::kForWrite);
		OdDbObjectId odFilterId = pOwnerDict->setAt(recordName, pFilter);
		}
	break;

	default:
		ASSERT(!"Unhandled dicrionary record encountered. DWGdirectConverter::processDictionaryRecord()");
		break;
	}

	return;
}

void DWGdirectConverter::processPersistantReactors()
{
	int count = m_persistantReactorList.size();

	OdDbObjectIdArray ids;
	for (int i = 0; i < count; i++)
	{
		try
		{
			handlePairs tmpHandles = m_persistantReactorList.getAt(i);

			OdDbObjectId reactorId = m_pOdDatabase->getOdDbObjectId(tmpHandles.getAssoc());
			OdDbObjectId entityId = m_pOdDatabase->getOdDbObjectId(tmpHandles.getEnt());

			// Group reactor hookup is done in object processing in DWGdirectWriter.
			bool bObjectIsAGroup = false;
			
			if (!reactorId.isNull())
			{
				bObjectIsAGroup = reactorId.safeOpenObject()->isKindOf(OdDbGroup::desc());
			}

			if ((bObjectIsAGroup == false) && (!entityId.isNull() && !reactorId.isNull()))
			{
				OdDbObjectPtr pEntity = entityId.safeOpenObject(OdDb::kForWrite);
				pEntity->getPersistentReactors(ids);

				int size = ids.length();
				for (int xx = 0; xx < size; xx++)
				{
					OdDbObjectId tmpId = ids[xx];
					if (!tmpId.isNull())
					{
						OdDbObjectPtr tmp = tmpId.safeOpenObject();
					}
				}
				if (!ids.empty() && !ids.contains(reactorId, 0))
					pEntity->addPersistentReactor(reactorId);
			}
		}
		catch(...)
		{
		}
	}

	m_persistantReactorList.clear();

	return;
}

void DWGdirectConverter::processProxyReferences()
{
	int count = m_proxyReferences.size();

	for (int i = 0; i < count; i++)
	{
		handItemObjectIDPair tmpPtrs = m_proxyReferences.getAt(i);

		db_handitem* pEntity = tmpPtrs.getHandItem();
		OdDbObjectId objectId = tmpPtrs.getObjectId();

		OdTypedIdsArray proxyReferences;
		OdDbProxyObjectPtr pOdProxyObject = objectId.safeOpenObject(OdDb::kForWrite);

		db_acad_proxy_object* pProxyObject = (db_acad_proxy_object*)pEntity;

		CDxfFiler tmpFiler(m_pOdDatabase);
		tmpFiler.initResBuf(ReadWriteUtils::getOdResBufPtrFromSdsResBufPtr(m_pOdDatabase, pProxyObject->getResbufChain()));

		tmpFiler.nextItem(); // Skipp DXF 5 handle value;

		pOdProxyObject->dxfInFields(&tmpFiler);
	}

	m_proxyReferences.clear();

	return;
}

void DWGdirectConverter::processDelayedEntities()
{
	int count = m_delayedEntities.size();

	for (int i = 0; i < count; i++)
	{
		try
		{
			handItemObjectIDPair tmpPtrs = m_delayedEntities.getAt(i);

			db_handitem* pEntity = tmpPtrs.getHandItem();
			OdDbObjectId ownerId = tmpPtrs.getObjectId();

			ASSERT(pEntity != NULL);
			ASSERT(!ownerId.isNull());

			OdDbEntityPtr pEnt;
			switch(pEntity->ret_type())
			{
			case DB_INSERT:
				pEnt = createOdDbBlockReference(pEntity, ownerId);
				break;
			case DB_DIMENSION:
				pEnt = createOdDbDimension(pEntity);
				break;
			case DB_LEADER:
				pEnt = createOdDbLeader(pEntity);
				break;
			case DB_HATCH:
				pEnt = createOdDbHatch(pEntity, ownerId);
				break;
			default:
				ASSERT(!"Unknown entity type");
				break;

			}

			if (!pEnt.isNull())
			{
				OdDbObjectId idBefore = ReadWriteUtils::setHandle(m_pOdDatabase, pEntity, ownerId, pEnt);

				OdDbBlockTableRecordPtr ownerPtr = ownerId.safeOpenObject(OdDb::kForWrite);
				OdDbObjectId idAfter = ownerPtr->appendOdDbEntity(pEnt);

				ASSERT(idBefore == idAfter);

				finalize(pEntity, pEnt);
			}
		}
		catch(...)
		{
			ASSERT(!"Conversion failed - exception thrown");
		}
	}

	m_delayedEntities.clear();

	return;
}

AfTypeVer DWGdirectConverter::getACISVersion()
{
	AfTypeVer acisType;

	if (getSaveType() == OdDb::kDxf)
		acisType = kAfTypeASCII;
	else
		acisType = kAfTypeBinary;

	OdDb::DwgVersion ver = getSaveVersion();
	if (ver == OdDb::vAC14)
		acisType |= kAfVer106;
	else if (ver == OdDb::vAC18)
	{
		acisType |= kAfVer700;
	}
		else
	{
		acisType |= kAfVer400;
	}

	return acisType;
}

OdDbObjectId DWGdirectConverter::getBlockId(db_handitem* pBlockRec)
{
	OdDbObjectId blockId;
	db_objhandle handle = pBlockRec->RetHandle();
	if (m_blockHandleMap.size() && handle != NULL)
	{
		HandleMapOldStrNewStr::iterator iter;

		char hand[17] = {0};
		handle.ToAscii(hand);
		iter = m_blockHandleMap.find(OdString(hand));

		if (iter != m_blockHandleMap.end())
		{
			handleName hn = (*iter).second;
			OdString h = hn.getHandle();
			OdString n = hn.getName();

			blockId = ReadWriteUtils::getBlockIDByName(m_pOdDatabase, m_pICdwg, this, n);
			if (blockId.isNull())
				blockId = ReadWriteUtils::getBlockIDByHandle(m_pOdDatabase, db_objhandle(h.c_str()));

			ASSERT( n == ((OdDbBlockTableRecordPtr)blockId.safeOpenObject())->getName());
		}
	}

	return blockId;
}
