// DWGdirectProtocolExtension.cpp: implementation of the DWGdirectProtocolExtension class.
//
//////////////////////////////////////////////////////////////////////
#include "db.h"
#include "viewport.h"
#include "DbMText.h"
#include "DbAcisEntity.h"
#include "objects.h"
#include "CollectionClasses.h"
#include "Utilities.h"

#include "OdaCommon.h"
#include "DWGdirectReader.h"
#include "DWGdirectProtocolExtension.h"
#include "RxObjectImpl.h"

#include "DbSymUtl.h"

#include "DbLine.h"
#include "DbCircle.h"

#include "Db2dPolyline.h"
#include "Db3dPolyline.h"
#include "Db3dPolylineVertex.h"
#include "DbPolyFaceMesh.h"
#include "DbPolyFaceMeshVertex.h"
#include "DbFaceRecord.h"
#include "DbPolygonMesh.h"
#include "DbPolygonMeshVertex.h"
#include "DbBlockReference.h"
#include "DbBlockTableRecord.h"
#include "DbAttribute.h"
#include "DbMInsertBlock.h"
#include "DbSpline.h"
#include "DbEllipse.h"
#include "DbSolid.h"
#include "DbTrace.h"
#include "Db3dSolid.h"
#include "GeKnotVector.h"
#include "DbProxyEntity.h"
#include "StaticRxObject.h"
#include "DbSpatialFilter.h"
#include "DbIndex.h"
#include "DbPolyline.h"
#include "GeCircArc3d.h"

#include "GeCurve2d.h"
#include "GeLine2d.h"
#include "GeCircArc2d.h"
#include "GeEllipArc2d.h"
#include "GeCubicSplineCurve2d.h"
#include "GeLineSeg2d.h"
#include "GeNurbCurve2d.h"

#include "DbSymbolTable.h"
#include "DbLayerTable.h"
#include "DbLayerTableRecord.h"
#include "DbLinetypeTable.h"
#include "DbLinetypeTableRecord.h"
#include "DbPoint.h"
#include "DbArc.h"
#include "DbFace.h"
#include "DbSequenceEnd.h"
#include "DbRay.h"
#include "DbXline.h"
#include "DbShape.h"
#include "DbAttributeDefinition.h"
#include "DbDimension.h"
#include "DbViewport.h"
#include "DbLeader.h"
#include "DbRegion.h"
#include "DbBody.h"
#include "DbMline.h"
#include "DbHatch.h"
#include "DbImage.h"
#include "DbArcAlignedText.h"
#include "RText.h"
#include "DbWipeout.h"
#include "DbFrame.h"
#include "DbBlockBegin.h"
#include "DbBlockEnd.h"
#include "DbFcf.h"
#include "DbOle2Frame.h"

#include "MemoryStream.h"
#include "OdFileBuf.h"
#include "GeScale3d.h"
#include "DbAlignedDimension.h"
#include "DbRotatedDimension.h"
#include "Db3PointAngularDimension.h"
#include "Db2LineAngularDimension.h"
#include "DbDiametricDimension.h"
#include "DbOrdinateDimension.h"
#include "DbRadialDimension.h"
#include "DbPolyFaceMesh.h"
#include "OleStorage.h"

#include "DWGdirectFiler.h"
#include "RxDynamicModule.h"

#include "GiWorldDrawVectors.h" // For displaying proxies.

TOOLKIT_EXPORT OdRxDictionaryIteratorPtr odDbGetDrawingClasses(OdDbDatabase& database);

#ifdef _DEBUG_MHB
OdRxObjectImpl<DWGdirectProtocolServices> g_pProtoExServices;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
ODRX_NO_CONS_DEFINE_MEMBERS(OdDbEntity_Creator, OdRxObject)

DWGdirectProtocolExtension::DWGdirectProtocolExtension()
{

}

DWGdirectProtocolExtension::~DWGdirectProtocolExtension()
{

}

class OdDbLine_Creator : public OdDbEntity_Creator
{
public:
	OdDbLine_Creator() {}

	db_handitem* createHanditem(OdDbEntity* pEntity, db_drawing* pDb, db_blocktabrec* pBlockRecord) const
	{
		ASSERT(pEntity != NULL);
		ASSERT(pDb != NULL);

		OdDbLinePtr pLine= pEntity;

        db_line* pNewLine = new db_line();
		ASSERT(pNewLine != NULL);
		if (pNewLine == NULL)
			return NULL;

		OdGePoint3d point3D;
		pLine->getStartPoint(point3D);

		sds_point startPt;
		CPYPT3D(startPt, point3D);
        pNewLine->set_10(startPt);

		sds_point endPt;
		pLine->getEndPoint(point3D);
		CPYPT3D(endPt, point3D);
        pNewLine->set_11(endPt);

		sds_point extrusion;
		CPYPT3D(extrusion, pLine->normal());
		pNewLine->init_210(extrusion);

		pNewLine->set_39(pLine->thickness());
		pNewLine->set_62(pLine->color().colorIndex());

		finalize(pEntity, pNewLine, pDb, pBlockRecord); 

		return pNewLine;
	}
}; // end class OdDbOdDbLine_Creator

class OdDbPoint_Creator : public OdDbEntity_Creator
{
public:
	OdDbPoint_Creator() {}

	db_handitem* createHanditem(OdDbEntity* pEntity, db_drawing* pDb, db_blocktabrec* pBlockRecord) const
	{
		ASSERT(pEntity != NULL);
		ASSERT(pDb != NULL);

		OdDbPointPtr pPoint= pEntity;

		db_point* pNewPoint = new db_point();
		ASSERT(pNewPoint != NULL);
		if (pNewPoint == NULL)
		   return NULL;

		sds_point point;
		CPYPT3D(point, pPoint->position());
		pNewPoint->set_10(point);

		sds_point extrusion;
		CPYPT3D(extrusion, pPoint->normal());
		pNewPoint->init_210(extrusion);

		pNewPoint->set_50(pPoint->ecsRotation());
		pNewPoint->set_39(pPoint->thickness());
		pNewPoint->set_62(pPoint->color().colorIndex());

		finalize(pEntity, pNewPoint, pDb, pBlockRecord); 

		return pNewPoint;
	}
}; // end class OdDbPoint_Creator

class OdDbCircle_Creator : public OdDbEntity_Creator
{
public:
	OdDbCircle_Creator() {}

	db_handitem* createHanditem(OdDbEntity* pEntity, db_drawing* pDb, db_blocktabrec* pBlockRecord) const
	{
		ASSERT(pEntity != NULL);
		ASSERT(pDb != NULL);

		OdDbCirclePtr pCircle= pEntity;
		db_circle* pNewCircle = new db_circle();
		ASSERT(pNewCircle != NULL);
		if (pNewCircle == NULL)
			return NULL;

		sds_point center;
		OdGeVector3d normal = pCircle->normal();

		OdGePoint3d point3D = pCircle->center();
		if (normal != OdGeVector3d::kZAxis)
		{
			point3D.transformBy(OdGeMatrix3d::worldToPlane(normal));
		}
		CPYPT3D(center, point3D);
		pNewCircle->set_10(center);
		
		pNewCircle->set_40(pCircle->radius());
		pNewCircle->set_39(pCircle->thickness());
		pNewCircle->set_62(pCircle->color().colorIndex());

		sds_point extrusion;
		CPYPT3D(extrusion, normal);
		pNewCircle->init_210(extrusion);

		finalize(pEntity, pNewCircle, pDb, pBlockRecord); 

		return pNewCircle;
	}
}; // end class OdDbCircle_Creator

class OdDbArc_Creator : public OdDbEntity_Creator
{
public:
	OdDbArc_Creator() {}

	db_handitem* createHanditem(OdDbEntity* pEntity, db_drawing* pDb, db_blocktabrec* pBlockRecord) const
	{
		ASSERT(pEntity != NULL);
		ASSERT(pDb != NULL);

		OdDbArcPtr pArc= pEntity;
		db_arc* pNewArc = new db_arc();
		ASSERT(pNewArc != NULL);
		if (pNewArc == NULL)
			return NULL;

		sds_point center;
		OdGeVector3d normal = pArc->normal();

		OdGePoint3d point3D = pArc->center();
		if (normal != OdGeVector3d::kZAxis)
		{
			point3D.transformBy(OdGeMatrix3d::worldToPlane(normal));
		}
		CPYPT3D(center, point3D);
		pNewArc->set_10(center);

		pNewArc->set_40(pArc->radius());
		pNewArc->set_39(pArc->thickness());
		pNewArc->set_62(pArc->color().colorIndex());
		pNewArc->set_50(pArc->startAngle());
		pNewArc->set_51(pArc->endAngle());

		sds_point extrusion;
		CPYPT3D(extrusion, normal);
		pNewArc->init_210(extrusion);

		finalize(pEntity, pNewArc, pDb, pBlockRecord); 

		return pNewArc;
	}
}; // end class OdDbArc_Creator

class OdDbTrace_Creator : public OdDbEntity_Creator
{
public:
	OdDbTrace_Creator() {}

	db_handitem* createHanditem(OdDbEntity* pEntity, db_drawing* pDb, db_blocktabrec* pBlockRecord) const
	{
		ASSERT(pEntity != NULL);
		ASSERT(pDb != NULL);

		OdDbTracePtr pTrace= pEntity;

		db_trace* pNewTrace = new db_trace();
		ASSERT(pNewTrace != NULL);
		if (pNewTrace == NULL)
			return NULL;

		sds_point point;
		OdGePoint3d point3D;
	
		OdGeVector3d normal = pTrace->normal();
		bool bNeedsTransform = (normal != OdGeVector3d::kZAxis) ? true : false;

		pTrace->getPointAt(0, point3D);
		if (bNeedsTransform == true)
			point3D.transformBy(OdGeMatrix3d::worldToPlane(normal));
		CPYPT3D(point, point3D);
		pNewTrace->set_10(point);

		pTrace->getPointAt(1, point3D);
		if (bNeedsTransform == true)
			point3D.transformBy(OdGeMatrix3d::worldToPlane(normal));
		CPYPT3D(point, point3D);
		pNewTrace->set_11(point);

		pTrace->getPointAt(2, point3D);
		if (bNeedsTransform == true)
			point3D.transformBy(OdGeMatrix3d::worldToPlane(normal));
		CPYPT3D(point, point3D);
		pNewTrace->set_12(point);

		pTrace->getPointAt(3, point3D);
		if (bNeedsTransform == true)
			point3D.transformBy(OdGeMatrix3d::worldToPlane(normal));
		CPYPT3D(point, point3D);
		pNewTrace->set_13(point);

		pNewTrace->set_39(pTrace->thickness());
		pNewTrace->set_62(pTrace->color().colorIndex());

		sds_point extrusion;
		CPYPT3D(extrusion, normal);
		pNewTrace->init_210(extrusion);

		finalize(pEntity, pNewTrace, pDb, pBlockRecord); 

		return pNewTrace;
	}
};

class OdDbSolid_Creator : public OdDbEntity_Creator
{
public:
	OdDbSolid_Creator() {}

	db_handitem* createHanditem(OdDbEntity* pEntity, db_drawing* pDb, db_blocktabrec* pBlockRecord) const
	{
		ASSERT(pEntity != NULL);
		ASSERT(pDb != NULL);

		OdDbSolidPtr pSolid= pEntity;

		db_solid* pNewSolid = new db_solid();
		ASSERT(pNewSolid != NULL);
		if (pNewSolid == NULL)
			return NULL;

		sds_point point;
		OdGePoint3d point3D;

		OdGeVector3d normal = pSolid->normal();
		bool bNeedsTransform = (normal != OdGeVector3d::kZAxis) ? true : false;

		pSolid->getPointAt(0, point3D);
		if (bNeedsTransform == true)
			point3D.transformBy(OdGeMatrix3d::worldToPlane(normal));
		CPYPT3D(point, point3D);
		pNewSolid->set_10(point);

		pSolid->getPointAt(1, point3D);
		if (bNeedsTransform == true)
			point3D.transformBy(OdGeMatrix3d::worldToPlane(normal));
		CPYPT3D(point, point3D);
		pNewSolid->set_11(point);

		pSolid->getPointAt(2, point3D);
		if (bNeedsTransform == true)
			point3D.transformBy(OdGeMatrix3d::worldToPlane(normal));
		CPYPT3D(point, point3D);
		pNewSolid->set_12(point);

		pSolid->getPointAt(3, point3D);
		if (bNeedsTransform == true)
			point3D.transformBy(OdGeMatrix3d::worldToPlane(normal));
		CPYPT3D(point, point3D);
		pNewSolid->set_13(point);

		pNewSolid->set_39(pSolid->thickness());
		pNewSolid->set_62(pSolid->color().colorIndex());

		sds_point extrusion;
		CPYPT3D(extrusion, normal);
		pNewSolid->init_210(extrusion);

		finalize(pEntity, pNewSolid, pDb, pBlockRecord); 

		return pNewSolid;
	}
};

class OdDbFace_Creator : public OdDbEntity_Creator
{
public:
	OdDbFace_Creator() {}

	db_handitem* createHanditem(OdDbEntity* pEntity, db_drawing* pDb, db_blocktabrec* pBlockRecord) const
	{
		ASSERT(pEntity != NULL);
		ASSERT(pDb != NULL);

		OdDbFacePtr pFace= pEntity;

		db_3dface* pNewFace = new db_3dface();
		ASSERT(pNewFace != NULL);
		if (pNewFace == NULL)
			return NULL;

		sds_point point;
		OdGePoint3d point3D;

		pFace->getVertexAt(0, point3D);
		CPYPT3D(point, point3D);
		pNewFace->set_10(point);

		pFace->getVertexAt(1, point3D);
		CPYPT3D(point, point3D);
		pNewFace->set_11(point);

		pFace->getVertexAt(2, point3D);
		CPYPT3D(point, point3D);
		pNewFace->set_12(point);

		pFace->getVertexAt(3, point3D);
		CPYPT3D(point, point3D);
		pNewFace->set_13(point);

		int flags = 0;
		flags |= (pFace->isEdgeVisibleAt(0)) ? 0 : BIT1;
		flags |= (pFace->isEdgeVisibleAt(1)) ? 0 : BIT2;
		flags |= (pFace->isEdgeVisibleAt(2)) ? 0 : BIT4;
		flags |= (pFace->isEdgeVisibleAt(3)) ? 0 : BIT8;
		pNewFace->set_70(flags);

		pNewFace->set_39(0.0);
		pNewFace->set_62(pFace->color().colorIndex());

		sds_point extrusion = {0.0,0.0,1.0};
		pNewFace->init_210(extrusion);

		finalize(pEntity, pNewFace, pDb, pBlockRecord); 

		return pNewFace;
	}
}; // end class OdDbArc_Creator

class OdDbSequenceEnd_Creator : public OdDbEntity_Creator
{
public:
	OdDbSequenceEnd_Creator() {}

	db_handitem* createHanditem(OdDbEntity* pEntity, db_drawing* pDb, db_blocktabrec* pBlockRecord) const
	{
		ASSERT(pEntity != NULL);
		ASSERT(pDb != NULL);

		db_seqend* pNewSeqEnd = new db_seqend();
		ASSERT(pNewSeqEnd != NULL);
		if (pNewSeqEnd == NULL)
			return NULL;

		finalize(pEntity, pNewSeqEnd, pDb, pBlockRecord); 

		return pNewSeqEnd;
	}
}; // end class OdDbSequenceEnd_Creator

class OdDbPolyline_Creator : public OdDbEntity_Creator
{
public:
	OdDbPolyline_Creator() {}

	db_handitem* createHanditem(OdDbEntity* pEntity, db_drawing* pDb, db_blocktabrec* pBlockRecord) const
	{
		ASSERT(pEntity != NULL);
		ASSERT(pDb != NULL);

		OdDbPolylinePtr pPolyline= pEntity;
		db_lwpolyline* pNewLWPline = new db_lwpolyline();
		ASSERT(pNewLWPline != NULL);
		if (pNewLWPline == NULL)
			return NULL;

		OdGeVector3d normal = pPolyline->normal();
		bool bNeedsTransform = (normal != OdGeVector3d::kZAxis) ? true : false;

		int numVerts = pPolyline->numVerts();
		ASSERT(numVerts != 0);

		pNewLWPline->set_90(numVerts);
		pNewLWPline->set_38(pPolyline->elevation());

		sds_point* vertices = new sds_point[numVerts * sizeof(sds_point)];

		double* bulges = NULL;
		if (pPolyline->hasBulges() == true)
		{
			bulges = new double[numVerts * sizeof(double)];
		}

		bool bHasWidths = pPolyline->hasWidth();

		double* startWidths = NULL;
		double* endWidths = NULL;
		if (bHasWidths == true)
		{
			startWidths = new double[numVerts * sizeof(double)];
			endWidths   = new double[numVerts * sizeof(double)];
		}
		else
		{
			pNewLWPline->set_43(pPolyline->getConstantWidth());
		}

		OdGePoint3d point3D;
		for(int i = 0; i < numVerts; i++)
		{
			if (vertices != NULL)
			{
				pPolyline->getPointAt(i, point3D);
				if (bNeedsTransform == true)
					point3D.transformBy(OdGeMatrix3d::worldToPlane(normal));

				CPYPT3D(vertices[i], point3D);
			}
			
			if (bulges != NULL)
			{
				bulges[i] = pPolyline->getBulgeAt(i);
			}

			if (bHasWidths == true)
			{
				pPolyline->getWidthsAt(i, startWidths[i], endWidths[i]);
			}
		}

		pNewLWPline->set_10(vertices, numVerts);
		if (startWidths != NULL)
		{
			pNewLWPline->set_40(startWidths, numVerts);
		}
		if (endWidths != NULL)
		{
			pNewLWPline->set_41(endWidths, numVerts);
		}
		if (bulges != NULL)
		{
			pNewLWPline->set_42(bulges, numVerts);
		}


		int flags = 0;
		if (pPolyline->isClosed() == true)
			flags |= BIT1;
		if (pPolyline->hasPlinegen() == true)
			flags |= BIT80;
		pNewLWPline->set_70(flags);


		pNewLWPline->set_39(pPolyline->thickness());
		pNewLWPline->set_62(pPolyline->color().colorIndex());

		sds_point extrusion;
		CPYPT3D(extrusion, normal);
		pNewLWPline->init_210(extrusion);

		delete [] vertices;
		delete [] bulges;
		delete [] startWidths;
		delete [] endWidths;

		finalize(pEntity, pNewLWPline, pDb, pBlockRecord); 

		return pNewLWPline;
	}
}; // end class OdDbPolyline_Creator

class OdDbVertex_Creator : public OdDbEntity_Creator
{
public:
	OdDbVertex_Creator() {}

	db_handitem* createHanditem(OdDbEntity* pEntity, db_drawing* pDb, db_blocktabrec* pBlockRecord) const
	{
		ASSERT(pEntity != NULL);
		ASSERT(pDb != NULL);

		db_vertex* pNewVertex = new db_vertex();
		ASSERT(pNewVertex != NULL);
		if (pNewVertex == NULL)
			return NULL;

		int flags = 0;
		sds_point   point;

		if (pEntity->isKindOf(OdDb2dVertex::desc()))
		{
			OdDb2dVertexPtr pVertex= pEntity;
			bool bIs2D = pVertex->isKindOf(OdDb2dVertex::desc());

			CPYPT3D(point, pVertex->position());
			pNewVertex->set_10(point);

            pNewVertex->set_40(pVertex->startWidth());
            pNewVertex->set_41(pVertex->endWidth());
            pNewVertex->set_42(pVertex->bulge());
            pNewVertex->set_50(pVertex->tangent());	

			if (bIs2D == true && pVertex->isTangentUsed())
				flags |= BIT2;
			
			OdDb::Vertex2dType type = pVertex->vertexType();
			if (type == OdDb::k2dCurveFitVertex)
				flags |= BIT1;
			if (type == OdDb::k2dSplineFitVertex)
				flags |= BIT8;
			if (type == OdDb::k2dSplineCtlVertex)
				flags |= BIT10;

			pNewVertex->set_62(pVertex->color().colorIndex());
		}
		else if (pEntity->isKindOf(OdDb3dPolylineVertex::desc()))
		{
			OdDb3dPolylineVertexPtr pVertex= pEntity;

			CPYPT3D(point, pVertex->position());
			pNewVertex->set_10(point);

			pNewVertex->set_40(0.0);
			pNewVertex->set_41(0.0);
			pNewVertex->set_42(0.0);
			pNewVertex->set_50(0.0);	

			OdDb::Vertex3dType type = pVertex->vertexType();
			if (type == OdDb::k3dFitVertex)
				flags |= BIT1;
			if (type == OdDb::k3dControlVertex)
				flags |= BIT10;
			flags |= BIT20;				// 3D polyline vertex

			pNewVertex->set_62(pVertex->color().colorIndex());
		}
		else if (pEntity->isKindOf(OdDbPolygonMeshVertex::desc()))
		{
			OdDbPolygonMeshVertexPtr pVertex= pEntity;

			CPYPT3D(point, pVertex->position());
			pNewVertex->set_10(point);

			pNewVertex->set_40(0.0);
			pNewVertex->set_41(0.0);
			pNewVertex->set_42(0.0);
			pNewVertex->set_50(0.0);	

			OdDb::Vertex3dType type = pVertex->vertexType();
			if (type == OdDb::k3dFitVertex)
				flags |= BIT1;
			if (type == OdDb::k3dControlVertex)
				flags |= BIT10;
			flags |= BIT40;				// 3D polygon mesh vertex.

			pNewVertex->set_62(pVertex->color().colorIndex());
		}
		else if (pEntity->isKindOf(OdDbPolyFaceMeshVertex::desc()))
		{
			OdDbPolyFaceMeshVertexPtr pVertex= pEntity;

			CPYPT3D(point, pVertex->position());
			pNewVertex->set_10(point);

			pNewVertex->set_40(0.0);
			pNewVertex->set_41(0.0);
			pNewVertex->set_42(0.0);
			pNewVertex->set_50(0.0);	

			flags |= IC_VERTEX_3DMESHVERT;				// 3D poly face mesh vertex.
			flags |= IC_VERTEX_FACELIST;				// 3D poly face mesh vertex.

			pNewVertex->set_62(pVertex->color().colorIndex());
		}
		pNewVertex->set_70(flags);

		finalize(pEntity, pNewVertex, pDb, pBlockRecord); 

		return pNewVertex;
	}
}; // end class OdDbVertex_Creator

class OdDbEllipse_Creator : public OdDbEntity_Creator
{
public:
	OdDbEllipse_Creator() {}

	db_handitem* createHanditem(OdDbEntity* pEntity, db_drawing* pDb, db_blocktabrec* pBlockRecord) const
	{
		ASSERT(pEntity != NULL);
		ASSERT(pDb != NULL);

		OdDbEllipsePtr pEllipse = pEntity;

		db_ellipse* pNewEllipse = new db_ellipse();
		ASSERT(pNewEllipse != NULL);
		if (pNewEllipse == NULL)
			return NULL;

		sds_point   point;
		OdGePoint3d center, endPt;
		
		center = pEllipse->center();
		CPYPT3D(point, center);
		pNewEllipse->set_10(point);

		bool bIsClosed = pEllipse->isClosed();

		if (bIsClosed)
		{
			pEllipse->getEndPoint(endPt);
			OdGeVector3d endRelToCenter = endPt - center;
			CPYPT3D(point, endRelToCenter);
		}
		else
		{
			OdGeVector3d vector3D = pEllipse->majorAxis();
			CPYPT3D(point, vector3D);
		}
		pNewEllipse->set_11(point);

		pNewEllipse->set_40(pEllipse->radiusRatio());

		double param;
		pEllipse->getStartParam(param);
		pNewEllipse->set_41(param);
		pEllipse->getEndParam(param);
		pNewEllipse->set_42(param);

		pNewEllipse->set_62(pEllipse->color().colorIndex());

		sds_point extrusion;
		CPYPT3D(extrusion, pEllipse->normal());
		pNewEllipse->init_210(extrusion);

		finalize(pEntity, pNewEllipse, pDb, pBlockRecord); 

		return pNewEllipse;
	}
};

class OdDbSpline_Creator : public OdDbEntity_Creator
{
public:
	OdDbSpline_Creator() {}

	db_handitem* createHanditem(OdDbEntity* pEntity, db_drawing* pDb, db_blocktabrec* pBlockRecord) const
	{
		ASSERT(pEntity != NULL);
		ASSERT(pDb != NULL);

		OdDbSplinePtr pSpline = pEntity;

		db_spline* pNewSpline = new db_spline();
		ASSERT(pNewSpline != NULL);
		if (pNewSpline == NULL)
			return NULL;

		sds_point point;
		OdGeVector3d startTan, entTan;

		pSpline->getFitTangents(startTan, entTan);

		CPYPT3D(point, startTan);
        pNewSpline->set_12(point);

		CPYPT3D(point, entTan);
        pNewSpline->set_13(point);

		int degree;
		bool rational;
		bool closed;
		bool periodic;
		double knotTol;
		double controlPtTol;
		
		OdGePoint3d point3D;
		OdGePoint3dArray controlPoints;
		OdGeDoubleArray knots;
		OdGeDoubleArray weights;

		pSpline->getNurbsData(degree, rational, closed, periodic,
							  controlPoints, knots, weights, 
							  controlPtTol, knotTol);

        pNewSpline->set_44(pSpline->fitTolerance());
        pNewSpline->set_71(degree);

		int flags = 0;
		if (closed == true)
			flags |= BIT1;
//		if (periodic == true)	// TODO MHB Causes bad visuals ?????
//			flags |= BIT2;
		if (rational == true)
			flags |= BIT4;
		if (pSpline->isPlanar() == true)
			flags |= BIT8;
//		if (pSpline->isLinear() == false) // TODO MHB No access ?????
//			flags |= BIT10;
        pNewSpline->set_70(flags);

		int count;
		if (knots.size())
		{
			count = knots.size();

			double* knotArray = new double [sizeof(double)*count];
			for (int i = 0; i < count; i++)
			{
				knotArray[i] = knots.getAt(i);
			}
			pNewSpline->set_40(knotArray, count);
			delete [] knotArray;
		}

		if (controlPoints.size())
		{
			count = controlPoints.size();

			sds_point* pointArray = new sds_point [sizeof(sds_point)*count];
			for (int i = 0; i < count; i++)
			{
				point3D = controlPoints.getAt(i);
				CPYPT3D(pointArray[i], point3D);
			}

			pNewSpline->set_10(pointArray, count);
			delete [] pointArray;
		}
			
		if (rational == true && weights.size())
		{
			count = weights.size();

			double* weightArray = new double [sizeof(double)*count];

			for (int i = 0; i < count; i++)
			{
				weightArray[i] = weights.getAt(i);
			}

			pNewSpline->set_41(weightArray, count);
			delete [] weightArray;
		}

		if (pSpline->hasFitData() == true)
		{
			bool bTangentsExist;
			double fitTolerance;
			OdGePoint3dArray fitPoints;
			OdGeVector3d startTangent;
			OdGeVector3d entTangent;
			sds_point* pointArray = NULL;;
			pSpline->getFitData(fitPoints, degree, fitTolerance, bTangentsExist, startTangent, entTangent);
			if (fitPoints.size())
			{
				int count = fitPoints.size();

				pointArray = new sds_point [sizeof(sds_point)*count];
				for (int i = 0; i < count; i++)
				{
					pSpline->getFitPointAt(i, point3D);
					CPYPT3D(pointArray[i], point3D);
				}
				pNewSpline->set_11(pointArray,count);
				delete [] pointArray;
			}
			pNewSpline->set_scenario(2);
		}
		else
		{
            pNewSpline->set_42(knotTol);
            pNewSpline->set_43(controlPtTol);
            pNewSpline->set_scenario(1);
		}

		pNewSpline->set_62(pSpline->color().colorIndex());

		finalize(pEntity, pNewSpline, pDb, pBlockRecord); 

		return pNewSpline;
	}
};

class OdDb2dPolyline_Creator : public OdDbEntity_Creator
{
public:
	OdDb2dPolyline_Creator() {}

	db_handitem* createHanditem(OdDbEntity* pEntity, db_drawing* pDb, db_blocktabrec* pBlockRecord) const
	{
		ASSERT(pEntity != NULL);
		ASSERT(pDb != NULL);

		OdDb2dPolylinePtr pPolyline = pEntity;

		db_polyline* pNewPline = new db_polyline();
		ASSERT(pNewPline != NULL);
		if (pNewPline == NULL)
			return NULL;

		pNewPline->set_39(pPolyline->thickness());
		pNewPline->set_40(pPolyline->defaultStartWidth());
		pNewPline->set_41(pPolyline->defaultEndWidth());

		int flags = 0;
		OdDb::Poly2dType type = pPolyline->polyType();
		if (pPolyline->isClosed() == true)
			flags |= BIT1;
		if (type == OdDb::k2dFitCurvePoly)
			flags |= BIT2;
		if (type == OdDb::k2dQuadSplinePoly || type == OdDb::k2dCubicSplinePoly)
			flags |= BIT4;
		if (pPolyline->isLinetypeGenerationOn() == true)
			flags |= BIT8;
		pNewPline->set_70(flags);

		pNewPline->set_71(0);
		pNewPline->set_72(0);
		pNewPline->set_73(0);
		pNewPline->set_74(0);

		flags = 0;
		if (type == OdDb::k2dQuadSplinePoly)
			flags |= 5;
		if (type == OdDb::k3dCubicSplinePoly)
			flags |= 6;
		pNewPline->set_75(flags);
		
		pNewPline->set_62(pPolyline->color().colorIndex());

		// For a pline, we need to add it to the database before we can add vertices to it.
		finalize(pEntity, pNewPline, pDb, pBlockRecord); 

		OdDbObjectIteratorPtr pVertIter = pPolyline->vertexIterator();

		for (; !pVertIter->done(); pVertIter->step())
		{
			OdDbEntityPtr pVertex = pVertIter->entity();

			try
			{
				db_vertex* pNewVertex = NULL;

				// Use protocol extension to create a db version of this entity.
				OdSmartPtr<OdDbEntity_Creator> pVertexCreator = pVertex;
				pNewVertex = (db_vertex*)pVertexCreator->createHanditem(pVertex, pDb, pBlockRecord);
				ASSERT(pNewVertex != NULL);
				
				if (pNewVertex != NULL)
				{
					pNewVertex->setParentPolyline(pNewPline);
				}
			}
			catch (OdError& err)
			{
				OdError err1 = err;	// Shut the compiler up.
#ifdef _DEBUG_MHB
				OdString msg = g_pProtoExServices.getErrorDescription(err.code()).c_str();
				ASSERT(!"DWGdirect OdDb2dPolyline_Creator() vertex creation error: ");
#endif
				delete pNewPline;
				return NULL;
			}
		}

		// Now the sequence end.
		try
		{
			db_seqend* pNewSeqEnd = NULL;
			OdDbSequenceEndPtr pSeqEnd = pPolyline->openSequenceEnd(OdDb::kForRead);
			ASSERT(!pSeqEnd.isNull());

			// Use protocol extension to create a db version of this entity.
			OdSmartPtr<OdDbEntity_Creator> pSeqEndCreator = (OdDbEntity*)pSeqEnd;
			pNewSeqEnd = (db_seqend*)pSeqEndCreator->createHanditem(pSeqEnd, pDb, pBlockRecord);
			ASSERT(pNewSeqEnd != NULL);
			
			if (pNewSeqEnd != NULL)
			{
				pNewSeqEnd->set_mainent(pNewPline);
			}
		}
		catch (OdError& err)
		{
			OdError err1 = err;	// Shut the compiler up.
#ifdef _DEBUG_MHB
			OdString msg = g_pProtoExServices.getErrorDescription(err.code()).c_str();
			ASSERT(!"DWGdirect OdDb2dPolyline_Creator() seqend creation error: ");
#endif
			delete pNewPline;
			return NULL;
		}

		OdGeVector3d normal = pPolyline->normal();
		sds_point extrusion;
		CPYPT3D(extrusion, normal);
		pNewPline->set_210(extrusion);

		// This call sets the Z for all the vertices in this polyline.
		sds_point point = {0.0,0.0,pPolyline->elevation()};
		pNewPline->set_10(point);
		
		return pNewPline;
	}
}; // end class OdDb2dPolyline_Creator

class OdDb3dPolyline_Creator : public OdDbEntity_Creator
{
public:
	OdDb3dPolyline_Creator() {}

	db_handitem* createHanditem(OdDbEntity* pEntity, db_drawing* pDb, db_blocktabrec* pBlockRecord) const
	{
		ASSERT(pEntity != NULL);
		ASSERT(pDb != NULL);

		OdDb3dPolylinePtr pPolyline = pEntity;

		db_polyline* pNewPline = new db_polyline();
		ASSERT(pNewPline != NULL);
		if (pNewPline == NULL)
			return NULL;

		pNewPline->set_39(0.0);
		pNewPline->set_40(0.0);
		pNewPline->set_41(0.0);

		OdDb::Poly3dType type = pPolyline->polyType();
		int flags = 0;
		if (pPolyline->isClosed() == true)
			flags |= BIT1;
		if (type == OdDb::k3dQuadSplinePoly || type == OdDb::k3dCubicSplinePoly)
			flags |= BIT4;
		flags |= BIT8;
		pNewPline->set_70(flags);

		pNewPline->set_71(0);
		pNewPline->set_72(0);
		pNewPline->set_73(0);
		pNewPline->set_74(0);

		flags = 0;
		if (type == OdDb::k3dQuadSplinePoly)
			flags |= 5;
		if (type == OdDb::k3dCubicSplinePoly)
			flags |= 6;
		pNewPline->set_75(flags);
		
		pNewPline->set_62(pPolyline->color().colorIndex());

		// For a pline, we need to add it to the database before we can add vertices to it.
		finalize(pEntity, pNewPline, pDb, pBlockRecord); 

		OdDbObjectIteratorPtr pVertIter = pPolyline->vertexIterator();

		for (; !pVertIter->done(); pVertIter->step())
		{
			OdDbEntityPtr pVertex = pVertIter->entity();

			try
			{
				db_vertex* pNewVertex = NULL;

				// Use protocol extension to create a db version of this entity.
				OdSmartPtr<OdDbEntity_Creator> pVertexCreator = pVertex;
				pNewVertex = (db_vertex*)pVertexCreator->createHanditem(pVertex, pDb, pBlockRecord);
				ASSERT(pNewVertex != NULL);

				if (pNewVertex != NULL)
				{
					pNewVertex->setParentPolyline(pNewPline);
				}
			}
			catch (OdError& err)
			{
				OdError err1 = err;	// Shut the compiler up.
#ifdef _DEBUG_MHB
				OdString msg = g_pProtoExServices.getErrorDescription(err.code()).c_str();
				ASSERT(!"DWGdirect OdDb3dPolyline_Creator() vertex creation error: ");
#endif
				delete pNewPline;
				return NULL;
			}
		}

		// Now the sequence end.
		try
		{
			db_seqend* pNewSeqEnd = NULL;
			OdDbSequenceEndPtr pSeqEnd = pPolyline->openSequenceEnd(OdDb::kForRead);

			// Use protocol extension to create a db version of this entity.
			OdSmartPtr<OdDbEntity_Creator> pSeqEndCreator = (OdDbEntity*)pSeqEnd;
			pNewSeqEnd = (db_seqend*)pSeqEndCreator->createHanditem(pSeqEnd, pDb, pBlockRecord);
			ASSERT(pNewSeqEnd != NULL);

			if (pNewSeqEnd != NULL)
			{
				pNewSeqEnd->set_mainent(pNewPline);
			}
		}
		catch (OdError& err)
		{
			OdError err1 = err;	// Shut the compiler up.
#ifdef _DEBUG_MHB
			OdString msg = g_pProtoExServices.getErrorDescription(err.code()).c_str();
			ASSERT(!"DWGdirect OdDb3dPolyline_Creator() seqend creation error: ");
#endif
			delete pNewPline;
			return NULL;
		}

		return pNewPline;
	}
};

class OdDbPolyFaceMesh_Creator : public OdDbEntity_Creator
{
public:
	OdDbPolyFaceMesh_Creator() {}

	db_handitem* createHanditem(OdDbEntity* pEntity, db_drawing* pDb, db_blocktabrec* pBlockRecord) const
	{
		ASSERT(pEntity != NULL);
		ASSERT(pDb != NULL);

		OdDbPolyFaceMeshPtr pPolyline = pEntity;

		db_polyline* pNewPline = new db_polyline();
		ASSERT(pNewPline != NULL);
		if (pNewPline == NULL)
			return NULL;

		int flags = 0;
		flags |= BIT40;		//is Polyface mesh.
		pNewPline->set_70(flags);

		sds_point point = {0.0, 0.0, 0.0};

		pNewPline->set_10(point);	// Dummy.

		OdInt16 numVerts = pPolyline->numVertices();
		OdInt16 numFaces = pPolyline->numFaces();

		pNewPline->set_71(numVerts);
		pNewPline->set_66((numVerts > 0) ? 1 : 0);
		pNewPline->set_72(numFaces);
		pNewPline->set_62(pPolyline->color().colorIndex());

		finalize(pEntity, pNewPline, pDb, pBlockRecord); 

		OdDbObjectIteratorPtr pVertIter = pPolyline->vertexIterator();

		for (; !pVertIter->done(); pVertIter->step())
		{

			OdDbEntityPtr pEntity = pVertIter->entity();

			OdDbPolyFaceMeshVertexPtr pVertex = pEntity->queryX(OdDbPolyFaceMeshVertex::desc());

			try
			{
				db_vertex* pNewVertex = NULL;
				// Use protocol extension to create a db version of this entity.
				OdSmartPtr<OdDbEntity_Creator> pEntCreator = pEntity;
				pNewVertex = (db_vertex*)pEntCreator->createHanditem(pEntity, pDb, pBlockRecord);
				ASSERT(pNewVertex != NULL);

				if (pVertex.isNull())
				{
					OdDbFaceRecordPtr pFace = pVertIter->entity()->queryX(OdDbFaceRecord::desc());

					if (!pFace.isNull())
					{
						int flags = IC_VERTEX_FACELIST;
						pNewVertex->set_70(flags);

						pNewVertex->set_71(pFace->getVertexAt(0));
						pNewVertex->set_72(pFace->getVertexAt(1));
						pNewVertex->set_73(pFace->getVertexAt(2));
						pNewVertex->set_74(pFace->getVertexAt(3));
					}

				}

				if (pNewVertex != NULL)
				{
					pNewVertex->setParentPolyline(pNewPline);
				}
			}
			catch (OdError& err)
			{
				OdError err1 = err;	// Shut the compiler up.
#ifdef _DEBUG_MHB
				OdString msg = g_pProtoExServices.getErrorDescription(err.code()).c_str();
				ASSERT(!"DWGdirect OdDbPolygonMesh_Creator() vertex creation error: ");
#endif
				delete pNewPline;
				return NULL;
			}

		}

		// Now the sequence end.
		try
		{
			db_seqend* pNewSeqEnd = NULL;
			OdDbSequenceEndPtr pSeqEnd = pPolyline->openSequenceEnd(OdDb::kForRead);
			ASSERT(!pSeqEnd.isNull());

			// Use protocol extension to create a db version of this entity.
			OdSmartPtr<OdDbEntity_Creator> pSeqEndCreator = (OdDbEntity*)pSeqEnd;
			pNewSeqEnd = (db_seqend*)pSeqEndCreator->createHanditem(pSeqEnd, pDb, pBlockRecord);
			ASSERT(pNewSeqEnd != NULL);

			if (pNewSeqEnd != NULL)
			{
				pNewSeqEnd->set_mainent(pNewPline);
			}
		}
		catch (OdError& err)
		{
			OdError err1 = err;	// Shut the compiler up.
#ifdef _DEBUG_MHB
			OdString msg = g_pProtoExServices.getErrorDescription(err.code()).c_str();
			ASSERT(!msg.c_str());
#endif
			delete pNewPline;
			return NULL;
		}

		return pNewPline;
	}
};

class OdDbPolygonMesh_Creator : public OdDbEntity_Creator
{
public:
	OdDbPolygonMesh_Creator() {}

	db_handitem* createHanditem(OdDbEntity* pEntity, db_drawing* pDb, db_blocktabrec* pBlockRecord) const
	{
		ASSERT(pEntity != NULL);
		ASSERT(pDb != NULL);

		OdDbPolygonMeshPtr pPolyline = pEntity;

		db_polyline* pNewPline = new db_polyline();
		ASSERT(pNewPline != NULL);
		if (pNewPline == NULL)
			return NULL;

		int flags = 0;
		flags |= BIT10;		//is Polygon mesh.
		if (pPolyline->isMClosed() == true)
			flags |= BIT1;
		if (pPolyline->isNClosed() == true)
			flags |= BIT20;		
		pNewPline->set_70(flags);

		sds_point point = {0.0, 0.0, 0.0};

		pNewPline->set_10(point);	// Dummy.

		pNewPline->set_71(pPolyline->mSize());
		pNewPline->set_72(pPolyline->nSize());
		pNewPline->set_73(pPolyline->mSurfaceDensity());
		pNewPline->set_74(pPolyline->nSurfaceDensity());
		pNewPline->set_75(pPolyline->polyMeshType());

		pNewPline->set_62(pPolyline->color().colorIndex());

		finalize(pEntity, pNewPline, pDb, pBlockRecord); 

		OdDbObjectIteratorPtr pVertIter = pPolyline->vertexIterator();

		for (; !pVertIter->done(); pVertIter->step())
		{
			OdDbEntityPtr pVertex = pVertIter->entity();

			try
			{
				db_vertex* pNewVertex = NULL;

				// Use protocol extension to create a db version of this entity.
				OdSmartPtr<OdDbEntity_Creator> pEntCreator = pVertex;
				pNewVertex = (db_vertex*)pEntCreator->createHanditem(pVertex, pDb, pBlockRecord);
				ASSERT(pNewVertex != NULL);

				if (pNewVertex != NULL)
				{
					pNewVertex->setParentPolyline(pNewPline);
				}
			}
			catch (OdError& err)
			{
				OdError err1 = err;	// Shut the compiler up.
#ifdef _DEBUG_MHB
				OdString msg = g_pProtoExServices.getErrorDescription(err.code()).c_str();
				ASSERT(!"DWGdirect OdDbPolygonMesh_Creator() vertex creation error: ");
#endif
				delete pNewPline;
				return NULL;
			}

		}

		// Now the sequence end.
		try
		{
			db_seqend* pNewSeqEnd = NULL;
			OdDbSequenceEndPtr pSeqEnd = pPolyline->openSequenceEnd(OdDb::kForRead);
			ASSERT(!pSeqEnd.isNull());

			// Use protocol extension to create a db version of this entity.
			OdSmartPtr<OdDbEntity_Creator> pSeqEndCreator = (OdDbEntity*)pSeqEnd;
			pNewSeqEnd = (db_seqend*)pSeqEndCreator->createHanditem(pSeqEnd, pDb, pBlockRecord);
			ASSERT(pNewSeqEnd != NULL);

			if (pNewSeqEnd != NULL)
			{
				pNewSeqEnd->set_mainent(pNewPline);
			}
		}
		catch (OdError& err)
		{
			OdError err1 = err;	// Shut the compiler up.
#ifdef _DEBUG_MHB
			OdString msg = g_pProtoExServices.getErrorDescription(err.code()).c_str();
			ASSERT(!"DWGdirect OdDbPolygonMesh_Creator() seqend creation error: ");
#endif
			delete pNewPline;
			return NULL;
		}

		return pNewPline;
	}
};

class OdDbBlockReference_Creator : public OdDbEntity_Creator
{
public:
	OdDbBlockReference_Creator() {}

	db_handitem* createHanditem(OdDbEntity* pEntity, db_drawing* pDb, db_blocktabrec* pBlockRecord) const
	{
		ASSERT(pEntity != NULL);
		ASSERT(pDb != NULL);

		OdDbBlockReferencePtr pInsert = pEntity;

		db_insert* pNewInsert = new db_insert();
		ASSERT(pNewInsert != NULL);
		if (pNewInsert == NULL)
			return NULL;

		OdDbObjectId blockId = pInsert->blockTableRecord();
        bool retVal = pNewInsert->set_2((char*)DWGdirectReader::GetBlockNameFromDWGdirectId(blockId).c_str(),pDb);
		if (retVal == false && db_lasterror == OL_ESNVALID)
		{	// Found some drawings with db_inserts but no block for it? 
			// this will try to recover.
			delete pNewInsert;
			return NULL;
		}
		sds_point point;
		OdGePoint3d point3D;

		OdGeVector3d normal = pInsert->normal();
		bool bNeedsTransform = (normal != OdGeVector3d::kZAxis) ? true : false;

		point3D = pInsert->position();

		if (bNeedsTransform == true)
			point3D.transformBy(OdGeMatrix3d::worldToPlane(normal));

		CPYPT3D(point, point3D);
        pNewInsert->set_10(point);

		OdGeScale3d scale = pInsert->scaleFactors();
        pNewInsert->set_41(scale.sx);
        pNewInsert->set_42(scale.sy);
        pNewInsert->set_43(scale.sz);

        pNewInsert->set_44(0);
        pNewInsert->set_45(0);
        pNewInsert->set_50(pInsert->rotation());

        pNewInsert->set_70(1);
        pNewInsert->set_71(1);

		sds_point extrusion;
		CPYPT3D(extrusion, normal);
		pNewInsert->init_210(extrusion);

		pNewInsert->set_62(pInsert->color().colorIndex());

		finalize(pEntity, pNewInsert, pDb, pBlockRecord); 

		OdDbObjectIteratorPtr objectIter = pInsert->attributeIterator();
		objectIter->start();

		OdDbEntityPtr entsFollow = objectIter->entity(); 
		bool bAttributesFollow;
		if (entsFollow.isNull())
			bAttributesFollow = false;
		else
			bAttributesFollow = true;
        pNewInsert->set_66((bAttributesFollow == true) ? 1 : 0);

		if (bAttributesFollow)
		{
			pDb->setcomplexinsertpending();
			for (; !objectIter->done(); objectIter->step())
			{
				OdDbAttributePtr pAttribute = objectIter->entity();

				try
				{
					db_attrib* pAttrib = NULL;
					// Use protocol extension to create a db version of this entity.
					OdSmartPtr<OdDbEntity_Creator> pAttribCreator = pAttribute;
					pAttrib = (db_attrib*)pAttribCreator->createHanditem(pAttribute, pDb, pBlockRecord);
					ASSERT(pAttrib != NULL);
				}
				catch (OdError& err)
				{
					OdError err1 = err;	// Shut the compiler up.
#ifdef _DEBUG_MHB
					OdString msg = g_pProtoExServices.getErrorDescription(err.code()).c_str();
					ASSERT(!"DWGdirect OdDbBlockReference_Creator() attribute creation error: ");
#endif
				}
			}

			// Now the sequence end.
			try
			{
				db_seqend* pNewSeqEnd = NULL;
				OdDbSequenceEndPtr pSeqEnd = pInsert->openSequenceEnd(OdDb::kForRead);
				ASSERT(!pSeqEnd.isNull());

				// Use protocol extension to create a db version of this entity.
				OdSmartPtr<OdDbEntity_Creator> pSeqEndCreator = (OdDbEntity*)pSeqEnd;
				pNewSeqEnd = (db_seqend*)pSeqEndCreator->createHanditem(pSeqEnd, pDb, pBlockRecord);
				ASSERT(pNewSeqEnd != NULL);

				if (pNewSeqEnd != NULL)
				{
					pNewSeqEnd->set_mainent(pNewInsert);
				}
			}
			catch (OdError& err)
			{
			OdError err1 = err;	// Shut the compiler up.
#ifdef _DEBUG_MHB
				OdString msg = g_pProtoExServices.getErrorDescription(err.code()).c_str();
				ASSERT(!"DWGdirect OdDbBlockReference_Creator() seqend creation error: ");
#endif
				delete pNewInsert;
				pNewInsert = NULL;
			}
			pDb->setcomplexinsertnotpending();
		}

		return pNewInsert;
	}
};

class OdDbMInsertBlock_Creator : public OdDbEntity_Creator
{
public:
	OdDbMInsertBlock_Creator() {}

	db_handitem* createHanditem(OdDbEntity* pEntity, db_drawing* pDb, db_blocktabrec* pBlockRecord) const
	{
		ASSERT(pEntity != NULL);
		ASSERT(pDb != NULL);

		OdDbMInsertBlockPtr pInsert = pEntity;

		db_insert* pNewInsert = new db_insert();
		ASSERT(pNewInsert != NULL);
		if (pNewInsert == NULL)
			return NULL;

		OdDbObjectId blockId = pInsert->blockTableRecord();
        pNewInsert->set_2((char*)DWGdirectReader::GetBlockNameFromDWGdirectId(blockId).c_str(),pDb);

		sds_point point;
		OdGePoint3d point3D;

		OdGeVector3d normal = pInsert->normal();
		bool bNeedsTransform = (normal != OdGeVector3d::kZAxis) ? true : false;

		point3D = pInsert->position();

		if (bNeedsTransform == true)
			point3D.transformBy(OdGeMatrix3d::worldToPlane(normal));
		CPYPT3D(point, point3D);
        pNewInsert->set_10(point);

		OdGeScale3d scale = pInsert->scaleFactors();
        pNewInsert->set_41(scale.sx);
        pNewInsert->set_42(scale.sy);
        pNewInsert->set_43(scale.sz);

        pNewInsert->set_44(pInsert->columnSpacing());
        pNewInsert->set_45(pInsert->rowSpacing());
        pNewInsert->set_50(pInsert->rotation());

        pNewInsert->set_70(pInsert->columns());
        pNewInsert->set_71(pInsert->rows());

		sds_point extrusion;
		CPYPT3D(extrusion, pInsert->normal());
		pNewInsert->init_210(extrusion);

		pNewInsert->set_62(pInsert->color().colorIndex());

		finalize(pEntity, pNewInsert, pDb, pBlockRecord); 

		OdDbObjectIteratorPtr objectIter = pInsert->attributeIterator();
		
		bool bAttributesFollow = (objectIter->entity().isNull()) ? false : true;
        pNewInsert->set_66((bAttributesFollow == true) ? 1 : 0);

		if (bAttributesFollow)
		{
			pDb->setcomplexinsertpending();
			for (; !objectIter->done(); objectIter->step())
			{
				OdDbAttributePtr pAttribute = objectIter->entity();

				try
				{
					db_attrib* pAttrib = NULL;
					// Use protocol extension to create a db version of this entity.
					OdSmartPtr<OdDbEntity_Creator> pAttribCreator = pAttribute;
					pAttrib = (db_attrib*)pAttribCreator->createHanditem(pAttribute, pDb, pBlockRecord);
					ASSERT(pAttrib != NULL);
				}
				catch (OdError& err)
				{
					OdError err1 = err;	// Shut the compiler up.
#ifdef _DEBUG_MHB
					OdString msg = g_pProtoExServices.getErrorDescription(err.code()).c_str();
					ASSERT(!"DWGdirect OdDbMInsertBlock_Creator() attribute creation error: ");
#endif
				}
			}

			// Now the sequence end.
			try
			{
				db_seqend* pNewSeqEnd = NULL;
				OdDbSequenceEndPtr pSeqEnd = pInsert->openSequenceEnd(OdDb::kForRead);
				ASSERT(!pSeqEnd.isNull());

				// Use protocol extension to create a db version of this entity.
				OdSmartPtr<OdDbEntity_Creator> pSeqEndCreator = (OdDbEntity*)pSeqEnd;
				pNewSeqEnd = (db_seqend*)pSeqEndCreator->createHanditem(pSeqEnd, pDb, pBlockRecord);
				ASSERT(pNewSeqEnd != NULL);

				if (pNewSeqEnd != NULL)
				{
					pNewSeqEnd->set_mainent(pNewInsert);
				}
			}
			catch (OdError& err)
			{
			OdError err1 = err;	// Shut the compiler up.
#ifdef _DEBUG_MHB
				OdString msg = g_pProtoExServices.getErrorDescription(err.code()).c_str();
				ASSERT(!"DWGdirect OdDbMInsertBlock_Creator() seqend creation error: ");
#endif
				delete pNewInsert;
				pNewInsert = NULL;
			}
			pDb->setcomplexinsertnotpending();
		}

		return pNewInsert;
	}
};

class OdDb3dSolid_Creator : public OdDbEntity_Creator
{
public:
	OdDb3dSolid_Creator() {}

	db_handitem* createHanditem(OdDbEntity* pEntity, db_drawing* pDb, db_blocktabrec* pBlockRecord) const
	{
		ASSERT(pEntity != NULL);
		ASSERT(pDb != NULL);

		OdDb3dSolidPtr pSolid = pEntity;

		db_3dsolid* pNewSolid = new db_3dsolid();
		ASSERT(pNewSolid != NULL);
		if (pNewSolid == NULL)
			return NULL;

		OdMemoryStreamPtr pStream = OdMemoryStream::createNew();
		pSolid->acisOut(pStream, kAf_ASCII_400);
		ASSERT(!pStream.isNull());
		if (pStream.isNull())
			return NULL;

		long dataLen = pStream->length();
		char* data = new char[dataLen + 1];
		memset(data, 0, dataLen);

		pStream->rewind();
		pStream->getBytes(data, dataLen);

		pNewSolid->set_1(data);

		delete [] data;

		pNewSolid->set_62(pSolid->color().colorIndex());

		finalize(pEntity, pNewSolid, pDb, pBlockRecord); 

		return pNewSolid;
	}
};

class OdDbProxyEntity_Creator : public OdDbEntity_Creator
{
public:
	OdDbProxyEntity_Creator() {}

	db_handitem* createHanditem(OdDbEntity* pEntity, db_drawing* pDb, db_blocktabrec* pBlockRecord) const
	{
		ASSERT(pEntity != NULL);
		ASSERT(pDb != NULL);

		OdDbProxyEntityPtr pProxy = pEntity;

		db_acad_proxy_entity* pNewProxy = new db_acad_proxy_entity();
		ASSERT(pNewProxy != NULL);
		if (pNewProxy == NULL)
			return NULL;

		CDxfFiler tmpFiler(pProxy->database());
		pProxy->dxfOutFields(&tmpFiler);
		OdResBufPtr pOdResBuf = tmpFiler.getFilerData();

		pNewProxy->setResbufChain(ReadWriteUtils::getSdsResBufPtrFromOdResBuf(pOdResBuf, pEntity->database()));

		OdRxDictionaryIteratorPtr pIter = odDbGetDrawingClasses(*pEntity->database());
		OdRxClassPtr pRegisteredClass;
		while(!pIter->done())
		{
			if (pIter->getKey() == pProxy->originalClassName())
			{
			  pRegisteredClass = pIter->object();
			  break;
			}
			pIter->next();
		}

		ASSERT(!pRegisteredClass.isNull());
		if (!pRegisteredClass.isNull())
		{
			pNewProxy->setOdDbClass((void*)pRegisteredClass.detach());
		}

		pNewProxy->set_62(pProxy->color().colorIndex());

		// TODO When PROXYSHOW is set to 2, a bounding box is
		// shown, with a vectorized string in it telling the
		// user what the bounding box is. This is the string
		// but vectorizing it is a problem which I am working on.
		// 
		//OdString classDescription = pProxy->originalClassName();
		//classDescription += " (";
		//classDescription += pProxy->applicationDescription();
		//classDescription += ")";
		OdString classDescription = "";
		pNewProxy->set_description((char*)classDescription.c_str(), classDescription.getLength());

		finalize(pEntity, pNewProxy, pDb, pBlockRecord); 

		OdGiEntityContext context(pProxy->database());
		OdGiEntityDraw wDraw((db_handitem*)pNewProxy);
		wDraw.setContext(&context);
		pProxy->worldDraw(&wDraw);
		pNewProxy->set_displayVectors(wDraw.getVectors());

		return pNewProxy;	
	}
}; // end OdDbProxyEntity_Creator

class OdDbRay_Creator : public OdDbEntity_Creator
{
public:
	OdDbRay_Creator() {}

	db_handitem* createHanditem(OdDbEntity* pEntity, db_drawing* pDb, db_blocktabrec* pBlockRecord) const
	{
		ASSERT(pEntity != NULL);
		ASSERT(pDb != NULL);

		OdDbRayPtr pRay = pEntity;

		db_ray* pNewRay = new db_ray();
		ASSERT(pNewRay != NULL);
		if (pNewRay == NULL)
			return NULL;

		sds_point point;
		CPYPT3D(point, pRay->basePoint());
		pNewRay->set_10(point);

		CPYPT3D(point, pRay->unitDir());
		pNewRay->set_11(point);

		pNewRay->set_62(pRay->color().colorIndex());

		finalize(pEntity, pNewRay, pDb, pBlockRecord); 

		return pNewRay;
	}
}; // end class OdDbRay_Creator

class OdDbXline_Creator : public OdDbEntity_Creator
{
public:
	OdDbXline_Creator() {}

	db_handitem* createHanditem(OdDbEntity* pEntity, db_drawing* pDb, db_blocktabrec* pBlockRecord) const
	{
		ASSERT(pEntity != NULL);
		ASSERT(pDb != NULL);

		OdDbXlinePtr pXline = pEntity;

		db_xline* pNewXline = new db_xline();
		ASSERT(pNewXline != NULL);
		if (pNewXline == NULL)
			return NULL;

		sds_point point;
		CPYPT3D(point, pXline->basePoint());
		pNewXline->set_10(point);

		CPYPT3D(point, pXline->unitDir());
		pNewXline->set_11(point);

		pNewXline->set_62(pXline->color().colorIndex());

		finalize(pEntity, pNewXline, pDb, pBlockRecord); 

		return pNewXline;
	}
}; // end class OdDbXline_Creator

class OdDbShape_Creator : public OdDbEntity_Creator
{
public:
	OdDbShape_Creator() {}

	db_handitem* createHanditem(OdDbEntity* pEntity, db_drawing* pDb, db_blocktabrec* pBlockRecord) const
	{
		ASSERT(pEntity != NULL);
		ASSERT(pDb != NULL);

		OdDbShapePtr pShape = pEntity;

		db_shape* pNewShape = new db_shape();
		ASSERT(pNewShape != NULL);
		if (pNewShape == NULL)
			return NULL;

		OdString stringHandle = pShape->styleId().getHandle().ascii();
        pNewShape->set_tshipbyhand(db_objhandle(stringHandle.c_str()), pDb);

		OdString shapeName = pShape->name();
        pNewShape->set_2((char*)shapeName.c_str());

		int shapeNum = pShape->shapeNumber();
        pNewShape->set_2(shapeNum);

		sds_point point;
		OdGePoint3d point3D;

		OdGeVector3d normal = pShape->normal();
		bool bNeedsTransform = (normal != OdGeVector3d::kZAxis) ? true : false;

		point3D = pShape->position();
		if (bNeedsTransform == true)
			point3D.transformBy(OdGeMatrix3d::worldToPlane(normal));
		CPYPT3D(point, point3D);
        pNewShape->set_10(point);

        pNewShape->set_40(pShape->size());
        pNewShape->set_41(pShape->widthFactor());
        pNewShape->set_50(pShape->rotation());
        pNewShape->set_51(pShape->oblique());
		pNewShape->set_39(pShape->thickness());

		sds_point extrusion;
		CPYPT3D(extrusion, normal);
		pNewShape->init_210(extrusion);

		pNewShape->set_62(pShape->color().colorIndex());

		finalize(pEntity, pNewShape, pDb, pBlockRecord); 

		return pNewShape;
	}
}; // end class OdDbShape_Creator

class OdDbText_Creator : public OdDbEntity_Creator
{
public:
	OdDbText_Creator() {}

	db_handitem* createHanditem(OdDbEntity* pEntity, db_drawing* pDb, db_blocktabrec* pBlockRecord) const
	{
		ASSERT(pEntity != NULL);
		ASSERT(pDb != NULL);

		OdDbTextPtr pText = pEntity;

		db_text* pNewText = new db_text();
		ASSERT(pNewText != NULL);
		if (pNewText == NULL)
			return NULL;

		pNewText->set_1((char*)pText->textString().c_str());

		OdDbObjectId styleId = pText->textStyle();
        pNewText->set_7((char*)DWGdirectReader::GetTextStyleNameFromDWGdirectId(styleId).c_str(), pDb);

		sds_point point;
		OdGePoint3d point3D;

		OdGeVector3d normal = pText->normal();
		bool bNeedsTransform = (normal != OdGeVector3d::kZAxis) ? true : false;

		point3D = pText->position();
		if (bNeedsTransform == true)
			point3D.transformBy(OdGeMatrix3d::worldToPlane(normal));
		CPYPT3D(point, point3D);
        pNewText->set_10(point);

		point3D = pText->alignmentPoint();
		if (bNeedsTransform == true)
			point3D.transformBy(OdGeMatrix3d::worldToPlane(normal));
		CPYPT3D(point, point3D);
		pNewText->set_11(point);

		pNewText->set_40(pText->height());
		pNewText->set_41(pText->widthFactor());
		pNewText->set_50(pText->rotation());
		pNewText->set_51(pText->oblique());

		int flags = 0;
		if (pText->isMirroredInX() == true)
			flags |= 2;
		if (pText->isMirroredInY() == true)
			flags |= 4;
		pNewText->set_71(flags);

		pNewText->set_72(pText->horizontalMode());
		pNewText->set_73(pText->verticalMode());

		sds_point extrusion;
		CPYPT3D(extrusion, normal);
		pNewText->init_210(extrusion);

		pNewText->set_62(pText->color().colorIndex());

		finalize(pEntity, pNewText, pDb, pBlockRecord); 

		return pNewText;
	}
}; // end class OdDbText_Creator

class OdDbAttributeDefinition_Creator : public OdDbEntity_Creator
{
public:
	OdDbAttributeDefinition_Creator() {}

	db_handitem* createHanditem(OdDbEntity* pEntity, db_drawing* pDb, db_blocktabrec* pBlockRecord) const
	{
		ASSERT(pEntity != NULL);
		ASSERT(pDb != NULL);

		OdDbAttributeDefinitionPtr pAttDef = pEntity;

		db_attdef* pNewAttDef = new db_attdef();
		ASSERT(pNewAttDef != NULL);
		if (pNewAttDef == NULL)
			return NULL;

        pNewAttDef->set_1((char*)pAttDef->textString().c_str());
        pNewAttDef->set_2((char*)pAttDef->tag().c_str());
        pNewAttDef->set_3((char*)pAttDef->prompt().c_str());

		OdDbObjectId styleId = pAttDef->textStyle();
        pNewAttDef->set_7((char*)DWGdirectReader::GetTextStyleNameFromDWGdirectId(styleId).c_str(), pDb);

		sds_point point;
		OdGePoint3d point3D;

		OdGeVector3d normal = pAttDef->normal();
		bool bNeedsTransform = (normal != OdGeVector3d::kZAxis) ? true : false;

		point3D = pAttDef->position();
		if (bNeedsTransform == true)
			point3D.transformBy(OdGeMatrix3d::worldToPlane(normal));
		CPYPT3D(point, point3D);
        pNewAttDef->set_10(point);

		point3D = pAttDef->alignmentPoint();
		if (bNeedsTransform == true)
			point3D.transformBy(OdGeMatrix3d::worldToPlane(normal));
		CPYPT3D(point, point3D);
        pNewAttDef->set_11(point);

        pNewAttDef->set_40(pAttDef->height());
        pNewAttDef->set_41(pAttDef->widthFactor());
        pNewAttDef->set_50(pAttDef->rotation());
        pNewAttDef->set_51(pAttDef->oblique());

		int flags = 0;
		if (pAttDef->isInvisible())
			flags |= BIT1;
		if (pAttDef->isConstant())
			flags |= BIT2;
		if (pAttDef->isVerifiable())
			flags |= BIT4;
		if (pAttDef->isPreset())
			flags |= BIT8;
        pNewAttDef->set_70(flags);

		flags = 0;
		if (pAttDef->isMirroredInX() == true)
			flags |= 2;
		if (pAttDef->isMirroredInY() == true)
			flags |= 4;
		pNewAttDef->set_71(flags);

        pNewAttDef->set_72(pAttDef->horizontalMode());
        pNewAttDef->set_73(pAttDef->verticalMode());
        pNewAttDef->set_74(pAttDef->verticalMode());

		sds_point extrusion;
		CPYPT3D(extrusion, normal);
		pNewAttDef->init_210(extrusion);

		pNewAttDef->set_62(pAttDef->color().colorIndex());

		finalize(pEntity, pNewAttDef, pDb, pBlockRecord); 

		return pNewAttDef;
	}
}; // end class OdDbAttributeDefinition_Creator

class OdDbAttribute_Creator : public OdDbEntity_Creator
{
public:
	OdDbAttribute_Creator() {}

	db_handitem* createHanditem(OdDbEntity* pEntity, db_drawing* pDb, db_blocktabrec* pBlockRecord) const
	{
		ASSERT(pEntity != NULL);
		ASSERT(pDb != NULL);

		OdDbAttributePtr pAttrib = pEntity;

		db_attrib* pNewAttrib = new db_attrib();
		ASSERT(pNewAttrib != NULL);
		if (pNewAttrib == NULL)
			return NULL;

        pNewAttrib->set_1((char*)pAttrib->textString().c_str());
        pNewAttrib->set_2((char*)pAttrib->tag().c_str());

		OdDbObjectId styleId = pAttrib->textStyle();
        pNewAttrib->set_7((char*)DWGdirectReader::GetTextStyleNameFromDWGdirectId(styleId).c_str(), pDb);

		sds_point point;
		OdGePoint3d point3D;

		OdGeVector3d normal = pAttrib->normal();
		bool bNeedsTransform = (normal != OdGeVector3d::kZAxis) ? true : false;

		point3D = pAttrib->position(); 
		if (bNeedsTransform == true)
			point3D.transformBy(OdGeMatrix3d::worldToPlane(normal));
		CPYPT3D(point, point3D);
        pNewAttrib->set_10(point);

		point3D = pAttrib->alignmentPoint();
		if (bNeedsTransform == true)
			point3D.transformBy(OdGeMatrix3d::worldToPlane(normal));
		CPYPT3D(point, point3D);
        pNewAttrib->set_11(point);

        pNewAttrib->set_40(pAttrib->height());
        pNewAttrib->set_41(pAttrib->widthFactor());
        pNewAttrib->set_50(pAttrib->rotation());
        pNewAttrib->set_51(pAttrib->oblique());

		int flags = 0;
		if (pAttrib->isInvisible())
			flags |= BIT1;
		if (pAttrib->isConstant())
			flags |= BIT2;
		if (pAttrib->isVerifiable())
			flags |= BIT4;
		if (pAttrib->isPreset())
			flags |= BIT8;
        pNewAttrib->set_70(flags);

		flags = 0;
		if (pAttrib->isMirroredInX() == true)
			flags |= 2;
		if (pAttrib->isMirroredInY() == true)
			flags |= 4;
		pNewAttrib->set_71(flags);

        pNewAttrib->set_72(pAttrib->horizontalMode());
        pNewAttrib->set_73(pAttrib->fieldLength());
        pNewAttrib->set_74(pAttrib->verticalMode());

		sds_point extrusion;
		CPYPT3D(extrusion, normal);
		pNewAttrib->init_210(extrusion);

		pNewAttrib->set_62(pAttrib->color().colorIndex());

		finalize(pEntity, pNewAttrib, pDb, pBlockRecord); 

		return pNewAttrib;
	}
}; // end class OdDbAttribute_Creator

class OdDbDimension_Creator : public OdDbEntity_Creator
{
public:
	OdDbDimension_Creator() {}

	db_handitem* createHanditem(OdDbEntity* pEntity, db_drawing* pDb, db_blocktabrec* pBlockRecord) const
	{
		ASSERT(pEntity != NULL);
		ASSERT(pDb != NULL);

		OdDbDimensionPtr pDim = pEntity;

		db_dimension* pNewDim = new db_dimension();
		ASSERT(pNewDim != NULL);
		if (pNewDim == NULL)
			return NULL;

        pNewDim->set_1((char*)pDim->dimensionText().c_str());

		OdDbObjectId blockId = pDim->dimBlockId();
        pNewDim->set_2((char*)DWGdirectReader::GetBlockNameFromDWGdirectId(blockId).c_str(),pDb);

		OdDbObjectId styleId = pDim->dimensionStyle();
        pNewDim->set_3((char*)DWGdirectReader::GetDimStyleNameFromDWGdirectId(styleId).c_str(),pDb);

		sds_point point;
		CPYPT3D(point, pDim->textPosition())
        pNewDim->set_11(point);

		CPYPT3D(point, pDim->dimBlockPosition())
        pNewDim->set_12(point);

		pNewDim->set_41(pDim->textLineSpacingFactor());
        pNewDim->set_42(pDim->measurement());
        pNewDim->set_51(pDim->horizontalRotation());
        pNewDim->set_53(pDim->textRotation());

		int flags = 0;
		if (pDim->isUsingDefaultTextPosition() == false)
			flags |= BIT80;

        pNewDim->set_71(pDim->textAttachment());
        pNewDim->set_72(pDim->textLineSpacingStyle());

		sds_point extrusion;
		CPYPT3D(extrusion, pDim->normal());
		pNewDim->init_210(extrusion);

		if (pEntity->isKindOf( OdDbAlignedDimension::desc()))
		{
			CPYPT3D(point, ((OdDbAlignedDimensionPtr)pEntity)->dimLinePoint());
			pNewDim->set_10(point);

			pNewDim->set_52(((OdDbAlignedDimensionPtr)pEntity)->oblique());

			CPYPT3D(point, ((OdDbAlignedDimensionPtr)pEntity)->xLine1Point());
			pNewDim->set_13(point);
			CPYPT3D(point, ((OdDbAlignedDimensionPtr)pEntity)->xLine2Point());
			pNewDim->set_14(point);

			flags |= BIT1;
		}
		else if (pEntity->isKindOf( OdDbRotatedDimension::desc()))
		{
			CPYPT3D(point, ((OdDbRotatedDimensionPtr)pEntity)->dimLinePoint());
			pNewDim->set_10(point);

			pNewDim->set_50(((OdDbRotatedDimensionPtr)pEntity)->rotation());
			pNewDim->set_52(((OdDbRotatedDimensionPtr)pEntity)->oblique());

			CPYPT3D(point, ((OdDbRotatedDimensionPtr)pEntity)->xLine1Point());
			pNewDim->set_13(point);
			CPYPT3D(point, ((OdDbRotatedDimensionPtr)pEntity)->xLine2Point());
			pNewDim->set_14(point);
		}
		else if (pEntity->isKindOf( OdDb3PointAngularDimension::desc()))
		{
			CPYPT3D(point, ((OdDb3PointAngularDimensionPtr)pEntity)->arcPoint());
			pNewDim->set_10(point);

			CPYPT3D(point, ((OdDb3PointAngularDimensionPtr)pEntity)->centerPoint());
			pNewDim->set_15(point);

			CPYPT3D(point, ((OdDb3PointAngularDimensionPtr)pEntity)->xLine1Point());
			pNewDim->set_13(point);
			CPYPT3D(point, ((OdDb3PointAngularDimensionPtr)pEntity)->xLine2Point());
			pNewDim->set_14(point);

			flags |= 5;
		}
		else if (pEntity->isKindOf( OdDb2LineAngularDimension::desc()))
		{
			CPYPT3D(point, ((OdDb2LineAngularDimensionPtr)pEntity)->xLine2End());
			pNewDim->set_10(point);

			CPYPT3D(point, ((OdDb2LineAngularDimensionPtr)pEntity)->xLine1Start());
			pNewDim->set_13(point);

			CPYPT3D(point, ((OdDb2LineAngularDimensionPtr)pEntity)->xLine1End());
			pNewDim->set_14(point);

			CPYPT3D(point, ((OdDb2LineAngularDimensionPtr)pEntity)->xLine2Start());
			pNewDim->set_15(point);

			CPYPT3D(point, ((OdDb2LineAngularDimensionPtr)pEntity)->arcPoint());
			pNewDim->set_16(point);

			flags |= BIT2;
		}
		else if (pEntity->isKindOf( OdDbDiametricDimension::desc()))
		{
			CPYPT3D(point, ((OdDbDiametricDimensionPtr)pEntity)->farChordPoint());
			pNewDim->set_10(point);

			CPYPT3D(point, ((OdDbDiametricDimensionPtr)pEntity)->chordPoint());
			pNewDim->set_15(point);

			pNewDim->set_40(((OdDbDiametricDimensionPtr)pEntity)->leaderLength());

			flags |= 3;
		}
		else if (pEntity->isKindOf( OdDbOrdinateDimension::desc()))
		{
			CPYPT3D(point, ((OdDbOrdinateDimensionPtr)pEntity)->origin());
			pNewDim->set_10(point);

			CPYPT3D(point, ((OdDbOrdinateDimensionPtr)pEntity)->definingPoint());
			pNewDim->set_13(point);

			CPYPT3D(point, ((OdDbOrdinateDimensionPtr)pEntity)->leaderEndPoint());
			pNewDim->set_14(point);

			if (((OdDbOrdinateDimensionPtr)pEntity)->isUsingXAxis() == true)
				flags |= BIT40;

			flags |= 6;

		}
		else if (pEntity->isKindOf( OdDbRadialDimension::desc()))
		{
			CPYPT3D(point, ((OdDbRadialDimensionPtr)pEntity)->center());
			pNewDim->set_10(point);

			CPYPT3D(point, ((OdDbRadialDimensionPtr)pEntity)->chordPoint());
			pNewDim->set_15(point);

			pNewDim->set_40(((OdDbRadialDimensionPtr)pEntity)->leaderLength());
			
			flags |= BIT4;
		}
		else
		{
			ASSERT(!"Haven't run into one of these: OdDbDimension_Creator()");
		}


        pNewDim->set_70(flags);

		pNewDim->set_62(pEntity->color().colorIndex());

		finalize(pEntity, pNewDim, pDb, pBlockRecord); 

		return pNewDim;

	}
}; // end class OdDbDimension_Creator

class OdDbViewport_Creator : public OdDbEntity_Creator
{
public:
	OdDbViewport_Creator() {}

	db_handitem* createHanditem(OdDbEntity* pEntity, db_drawing* pDb, db_blocktabrec* pBlockRecord) const
	{
		ASSERT(pEntity != NULL);
		ASSERT(pDb != NULL);

		OdDbViewportPtr pViewport = pEntity;

		db_viewport* pNewViewport = new db_viewport();
		ASSERT(pNewViewport != NULL);
		if (pNewViewport == NULL)
			return NULL;

		sds_point point;
		OdGePoint3d point3D;
		OdGePoint2d point2D;

		CPYPT3D(point, pViewport->centerPoint());
		pNewViewport->set_10(point);

		pNewViewport->set_40(pViewport->width());
		pNewViewport->set_41(pViewport->height());
		pNewViewport->set_62(pViewport->color().colorIndex());
		pNewViewport->set_69(pViewport->number());

		CPYPT2D(point, pViewport->viewCenter());
		pNewViewport->set_12(point);

		CPYPT2D(point, pViewport->snapBasePoint());
		pNewViewport->set_13(point);

		CPYPT2D(point, pViewport->snapIncrement());
		pNewViewport->set_14(point);

		CPYPT2D(point, pViewport->gridIncrement());
		pNewViewport->set_15(point);

		CPYPT3D(point, pViewport->viewDirection());
		pNewViewport->set_16(point);

		CPYPT3D(point, pViewport->viewTarget());
		pNewViewport->set_17(point);

		pNewViewport->set_42(pViewport->lensLength());
		pNewViewport->set_43(pViewport->frontClipDistance());
		pNewViewport->set_44(pViewport->backClipDistance());
		pNewViewport->set_45(pViewport->viewHeight());
		pNewViewport->set_50(pViewport->snapAngle());
		pNewViewport->set_51(pViewport->twistAngle());
		pNewViewport->set_72(pViewport->circleSides());

		// Tack on table records of all the frozen layers in this
		// viewport to the viewport itself.
		OdDbObjectIdArray frozenLayerIDs;
		pViewport->getFrozenLayerList(frozenLayerIDs);
		
		int layerCount = frozenLayerIDs.size();
		if (layerCount)
		{
			int layersFound = 0;
			OdDbLayerTablePtr pLayerTable = pEntity->database()->getLayerTableId().safeOpenObject();
			ASSERT(!pLayerTable.isNull());

			OdDbSymbolTableIteratorPtr pTableIter = pLayerTable->newIterator();
			for(pTableIter->start(); !pTableIter->done(); pTableIter->step())
			{
				unsigned int iIndex;
				if (frozenLayerIDs.find(pTableIter->getRecordId(), iIndex, 0))
				{
					OdDbLayerTableRecordPtr pLayerRec = pTableIter->getRecordId().safeOpenObject();
					OdString layerName = pLayerRec->getName();

					db_layertabrec* pLayer = (db_layertabrec*)pDb->findtabrec(DB_LAYERTAB, layerName.c_str(), 0);
					ASSERT(pLayer != NULL);
					if (pLayer != NULL)
					{
						layersFound++;
						pNewViewport->AddLayerToVpLayerData(pLayer);
						if (layersFound == layerCount)
							break;
					}
				}
			}
		}

		int flags = 0;
		if (pViewport->isFrontClipOn() == true)
			flags |= BIT2;
		if (pViewport->isBackClipOn() == true)
			flags |= BIT4;
		if (pViewport->isUcsFollowModeOn() == true)
			flags |= BIT8;
		if (pViewport->isFrontClipAtEyeOn() == true)
			flags |= BIT10;
		if (pViewport->isUcsIconVisible() == true)
			flags |= BIT20;
		if (pViewport->isUcsIconAtOrigin() == false)	// MHB seems odd. but verified with ODT
			flags |= BIT40;
		if (pViewport->isFastZoomOn() == true)
			flags |= BIT80;
		if (pViewport->isSnapOn() == true)
			flags |= BIT100;
		if (pViewport->isGridOn() == true)
			flags |= BIT200;
		if (pViewport->isSnapIsometric() == true)
			flags |= BIT400;
		if (pViewport->hiddenLinesRemoved() == true)
			flags |= BIT800;
		if (pViewport->isLocked() == true)
			flags |= BIT4000;
		if (pViewport->isNonRectClipOn() == true)
			flags |= BIT10000;
		if (pViewport->isOn() == false)					// MHB seems odd. but verified with ODT
			flags |= BIT20000;
		flags |= BIT8000;				// Docs say 'Currently always enabled.
		pNewViewport->set_90(flags);

		// TODO MHB
		// Hard-pointer ID/handle to entity that serves as the viewport's 
		// clipping boundary (only present if viewport is non-rectangular)
		// I'd have to actually make it first ???????
		// pViewport->nonRectClipEntityId();
		//pNewViewport->set_340();

		pNewViewport->set_1((char*)pViewport->plotStyleSheet().c_str());
		pNewViewport->set_281(pViewport->renderMode());
		pNewViewport->set_71((pViewport->isUcsSavedWithViewport() == true) ? 1 : 0);

		// TODO MHB
		// Display UCS icon at UCS origin flag:
		// Controls whether UCS icon represents viewport UCS or current UCS 
		// (these will be different if UCSVP is 1 and viewport is not active). 
		// However, this field is currently being ignored and the icon always 
		// represents the viewport UCS
		 pNewViewport->set_74(1);

		OdGeVector3d tmpVect3D, vector3D;
		pViewport->getUcs(point3D, vector3D, tmpVect3D);

		CPYPT3D(point, point3D);
		pNewViewport->set_110(point);

		CPYPT3D(point, vector3D);
		pNewViewport->set_111(point);

		CPYPT3D(point, tmpVect3D);
		pNewViewport->set_112(point);

		OdDb::OrthographicView viewType;
		bool bIsOrthographic = pViewport->isUcsOrthographic(viewType);
		pNewViewport->set_79((bIsOrthographic == false) ? 0 : viewType);

		// TODO MHB
		//OdDbObjectId ucsId = pViewport->ucsName();

		// ID/handle of AcDbUCSTableRecord if UCS is a named UCS. 
		// If not present, then UCS is unnamed
		// pNewViewport->set_345();

		// ID/handle of AcDbUCSTableRecord of base UCS if UCS 
		// is orthographic (79 code is non-zero). If not present and 
		// 79 code is non-zero, then base UCS is taken to be WORLD
		// pNewViewport->set_346();

		pNewViewport->set_146(pViewport->elevation());

		pNewViewport->set_62(pViewport->color().colorIndex());

		finalize(pEntity, pNewViewport, pDb, pBlockRecord); 

		return pNewViewport;
	}
}; // end class OdDbViewport_Creator

class OdDbMText_Creator : public OdDbEntity_Creator
{
public:
	OdDbMText_Creator() {}

	db_handitem* createHanditem(OdDbEntity* pEntity, db_drawing* pDb, db_blocktabrec* pBlockRecord) const
	{
		ASSERT(pEntity != NULL);
		ASSERT(pDb != NULL);

		OdDbMTextPtr pMText = pEntity;

		db_mtext* pNewMText = new db_mtext();
		ASSERT(pNewMText != NULL);
		if (pNewMText == NULL)
			return NULL;
		
		pNewMText->set_1((char*)pMText->contents().c_str());

		OdDbObjectId styleId = pMText->textStyle();
        pNewMText->set_7((char*)DWGdirectReader::GetTextStyleNameFromDWGdirectId(styleId).c_str(), pDb);

		sds_point point;

		OdGeVector3d normal = pMText->normal();
		bool bNeedsTransform = (normal != OdGeVector3d::kZAxis) ? true : false;

		CPYPT3D(point, pMText->location());
        pNewMText->set_10(point);

		CPYPT3D(point, pMText->direction());
        pNewMText->set_11(point);

        pNewMText->set_40(pMText->textHeight());
        pNewMText->set_41(pMText->width());
        pNewMText->set_71(pMText->attachment());
        pNewMText->set_72(pMText->flowDirection());
        pNewMText->set_44(pMText->lineSpacingFactor());
        pNewMText->set_73(pMText->lineSpacingStyle());

		sds_point extrusion;
		CPYPT3D(extrusion, normal);
		pNewMText->init_210(extrusion);

		pNewMText->set_62(pMText->color().colorIndex());

		finalize(pEntity, pNewMText, pDb, pBlockRecord); 

		return pNewMText;
	}
}; // end class OdDbMText_Creator

class OdDbLeader_Creator : public OdDbEntity_Creator
{
public:
	OdDbLeader_Creator() {}

	db_handitem* createHanditem(OdDbEntity* pEntity, db_drawing* pDb, db_blocktabrec* pBlockRecord) const
	{
		ASSERT(pEntity != NULL);
		ASSERT(pDb != NULL);

		OdDbLeaderPtr pLeader = pEntity;

		db_leader* pNewLeader = new db_leader();
		ASSERT(pNewLeader != NULL);
		if (pNewLeader == NULL)
			return NULL;

		OdDbObjectId styleId = pLeader->dimensionStyle();
		pNewLeader->set_3((char*)DWGdirectReader::GetDimStyleNameFromDWGdirectId(styleId).c_str(),pDb);

		int numPoints = pLeader->numVertices();
		if (numPoints)
		{
			sds_point *points= new sds_point [numPoints * sizeof(sds_point)];

			for (int i = 0; i < numPoints; i++)
			{
				CPYPT3D(points[i], pLeader->vertexAt(i));
			}

			pNewLeader->set_10(points,numPoints);
			delete [] points;
		}

		pNewLeader->set_40(pLeader->annoHeight());
		pNewLeader->set_41(pLeader->annoWidth());
		pNewLeader->set_71((pLeader->hasArrowHead()) ? 1 : 0);
		pNewLeader->set_72((pLeader->isSplined()) ? 1 : 0);
        pNewLeader->set_73(pLeader->annoType());
		pNewLeader->set_74((pLeader->isHookLineOnXDir()) ? 1 : 0);
		pNewLeader->set_75((pLeader->hasHookLine()) ? 1 : 0);

		pNewLeader->set_77(pLeader->dimclrd().colorIndex());

		sds_point point;
		CPYPT3D(point, pLeader->annotationXDir());
		pNewLeader->set_211(point);
                             
		// TODO MHB not accessable ??
        //Bugzilla No. 78044 ; 25-03-2002
        //leader->set_offsettoblockinspt(aden.leader.offsettoblkinspt);
        //pNewLeader->set_212(aden.leader.offsettoblkinspt);

		CPYPT3D(point, pLeader->annotationOffset());
		pNewLeader->set_213(point);

		db_handitem* text = NULL;
		OdString stringHandle = pLeader->annotationObjId().getHandle().ascii();
		if (stringHandle.iCompare("0") != 0)
		{
			text = pDb->handent(db_objhandle(stringHandle.c_str()));
		}
		pNewLeader->set_340(text);

		sds_point extrusion;
		CPYPT3D(extrusion, pLeader->normal());
		pNewLeader->init_210(extrusion);

		pNewLeader->set_62(pLeader->color().colorIndex());

		finalize(pEntity, pNewLeader, pDb, pBlockRecord); 

		return pNewLeader;
	}
}; // end class OdDbLeader_Creator

class OdDbRegion_Creator : public OdDbEntity_Creator
{
public:
	OdDbRegion_Creator() {}

	db_handitem* createHanditem(OdDbEntity* pEntity, db_drawing* pDb, db_blocktabrec* pBlockRecord) const
	{
		ASSERT(pEntity != NULL);
		ASSERT(pDb != NULL);

		OdDbRegionPtr pRegion = pEntity;

		db_region* pNewRegion = new db_region();
		ASSERT(pNewRegion != NULL);
		if (pNewRegion == NULL)
			return NULL;

		OdMemoryStreamPtr pStream = OdMemoryStream::createNew();
		pRegion->acisOut(pStream, kAf_ASCII_400);
		ASSERT(!pStream.isNull());
		if (pStream.isNull())
			return NULL;

		long dataLen = pStream->length();
		char* data = new char[dataLen + 1];
		memset(data, 0, dataLen);

		pStream->rewind();
		pStream->getBytes(data, dataLen);

		pNewRegion->set_1(data);

		delete [] data;

		pNewRegion->set_62(pRegion->color().colorIndex());

		finalize(pEntity, pNewRegion, pDb, pBlockRecord); 

		return pNewRegion;
	}
}; // end class OdDbRegion_Creator

class OdDbBody_Creator : public OdDbEntity_Creator
{
public:
	OdDbBody_Creator() {}

	db_handitem* createHanditem(OdDbEntity* pEntity, db_drawing* pDb, db_blocktabrec* pBlockRecord) const
	{
		ASSERT(pEntity != NULL);
		ASSERT(pDb != NULL);

		OdDbBodyPtr pBody = pEntity;

		db_body* pNewBody = new db_body();
		ASSERT(pNewBody != NULL);
		if (pNewBody == NULL)
			return NULL;

		OdMemoryStreamPtr pStream = OdMemoryStream::createNew();
		pBody->acisOut(pStream, kAf_ASCII_400);
		ASSERT(!pStream.isNull());
		if (pStream.isNull())
			return NULL;

		long dataLen = pStream->length();
		char* data = new char[dataLen + 1];
		memset(data, 0, dataLen);

		pStream->rewind();
		pStream->getBytes(data, dataLen);
		
		pNewBody->set_1(data);

		delete [] data;

		pNewBody->set_62(pBody->color().colorIndex());

		finalize(pEntity, pNewBody, pDb, pBlockRecord); 

		return pNewBody;
	}
}; // end class OdDbBody_Creator

class OdDbMline_Creator : public OdDbEntity_Creator
{
public:
	OdDbMline_Creator() {}

	db_handitem* createHanditem(OdDbEntity* pEntity, db_drawing* pDb, db_blocktabrec* pBlockRecord) const
	{
		ASSERT(pEntity != NULL);
		ASSERT(pDb != NULL);

		OdDbMlinePtr pMline = pEntity;

		db_mline* pNewMline = new db_mline();
		ASSERT(pNewMline != NULL);
		if (pNewMline == NULL)
			return NULL;

		OdString stringHandle = pMline->style().getHandle().ascii();
		pNewMline->set_340byhand(db_objhandle(stringHandle.c_str()), pDb);

		sds_point point;
		CPYPT3D(point, pMline->vertexAt(0));
        pNewMline->set_10(point);

        pNewMline->set_40(pMline->scale());
        pNewMline->set_70(pMline->justification());

		int numVerts = pMline->numVertices();
        pNewMline->set_72(numVerts);

		int flags = 0;
		if (numVerts > 0)
			flags |= BIT1;
		if (pMline->closedMline() == true)
			flags |= BIT2;
		if (pMline->supressStartCaps() == true)
			flags |= BIT4;
		if (pMline->supressEndCaps() == true)
			flags |= BIT8;

        pNewMline->set_71(flags);

        sds_point *parr11= new sds_point[numVerts * sizeof(sds_point)];
        sds_point *parr12= new sds_point[numVerts * sizeof(sds_point)];
        sds_point *parr13= new sds_point[numVerts * sizeof(sds_point)];

		// This needs to be done first to set up the storage buffers
		// for this mline.
		if (numVerts >= 2)
		{
			OdMLSegmentArray segments;
			pMline->getParametersAt(0, segments);
			pNewMline->set_73(segments.size());
		}

		int i = 0;
        for (i = 0; i < numVerts; i++) 
		{
			CPYPT3D(parr11[i], pMline->vertexAt(i));

			CPYPT3D(parr12[i], pMline->axisAt(i));

			CPYPT3D(parr13[i], pMline->miterAt(i));

			OdMLSegmentArray segments;
			pMline->getParametersAt(i, segments);

			int elementCount = segments.size();
			int j = 0;
			for ( j = 0; j < elementCount; j++)
			{
				OdGeDoubleArray areaFillParams = segments.getAt(j).m_AreaFillParams;
				OdGeDoubleArray segmentParams  = segments.getAt(j).m_SegParams;
				int numSegsParams = segmentParams.size();
				int numFillParams = areaFillParams.size();

				
				{
					sds_real *pReals = new sds_real[numSegsParams * sizeof(sds_real)];

					pNewMline->set_74(numSegsParams,i,j);
					
					for (int k = 0; k < numSegsParams; k++)
					{
						pReals[k] = segmentParams.getAt(k);
					}
					if (numSegsParams)
						pNewMline->set_41(pReals, i, j);
					delete [] pReals;
				}

				
				{
					sds_real *pReals = new sds_real[numFillParams * sizeof(sds_real)];

					pNewMline->set_75(numFillParams, i, j);

					for (int k = 0; k < numFillParams; k++)
					{
						pReals[k] = areaFillParams.getAt(k);
					}
					if (numFillParams)
						pNewMline->set_42(pReals,i,j);
					delete [] pReals;
				}
			}
        }
        pNewMline->set_11(parr11);
        pNewMline->set_12(parr12);
        pNewMline->set_13(parr13);
        delete [] parr11; delete [] parr12; delete [] parr13;

		sds_point extrusion;
		CPYPT3D(extrusion, pMline->normal());
		pNewMline->init_210(extrusion);

		pNewMline->set_62(pMline->color().colorIndex());

		finalize(pEntity, pNewMline, pDb, pBlockRecord); 

		return pNewMline;
	}
}; // end class OdDbMline_Creator

class OdDbHatch_Creator : public OdDbEntity_Creator
{
public:
	OdDbHatch_Creator() {}

	db_handitem* createHanditem(OdDbEntity* pEntity, db_drawing* pDb, db_blocktabrec* pBlockRecord) const
	{
		ASSERT(pEntity != NULL);
		ASSERT(pDb != NULL);

		OdDbHatchPtr pHatch = pEntity;

		db_hatch* pNewHatch = new db_hatch();
		ASSERT(pNewHatch != NULL);
		if (pNewHatch == NULL)
			return NULL;

		sds_point point;

		int seedCount = pHatch->numSeedPoints();
		if (seedCount)
		{
			OdGePoint2d point2D = pHatch->getSeedPointAt(0);
			CPYPT2D(point, pHatch->getSeedPointAt(0));
			point[2] = pHatch->elevation();
		}
		else
		{
			CPYPT3D(point, OdGePoint3d(0.0,0.0,0.0));
		}
		pNewHatch->set_10(point);

		pNewHatch->set_2((char*)pHatch->patternName().c_str());
		pNewHatch->set_70((pHatch->isSolidFill() == true) ? 1 : 0);
		pNewHatch->set_71((pHatch->associative() == true) ? 1 : 0);
		pNewHatch->set_47(pHatch->pixelSize());

		pNewHatch->set_98(seedCount);

		int loopCount = pHatch->numLoops();
		pNewHatch->set_91(loopCount);

		pNewHatch->set_75(pHatch->hatchStyle());
		pNewHatch->set_76(pHatch->patternType());
		pNewHatch->set_52(pHatch->patternAngle());
		pNewHatch->set_41(pHatch->patternSpace());
		pNewHatch->set_77((pHatch->patternDouble() == true) ? 1 : 0);
		pNewHatch->set_78(pHatch->numPatternDefinitions());

		EdgeArray edgePtrs;
		OdGePoint2dArray vertices;
		OdGeDoubleArray bulges;
		for (int i = 0; i < loopCount; i++)
		{			
			int loopType = pHatch->loopTypeAt(i);
			pNewHatch->set_92(loopType);

			try
			{
				if (loopType & OdDbHatch::kPolyline)
				{
					pNewHatch->set_path(i, loopType, 1);

					pHatch->getLoopAt(i, vertices, bulges);
					
					int numVerts = vertices.size();
					bool bHasBulges = (numVerts == bulges.size ()) ? true: false;

					pNewHatch->set_pathedgepline(i, 1/*closed*/, (int)bHasBulges, numVerts);
					
					int vertexIndex = 0;
					for (OdGePoint2dArray::const_iterator vertex = vertices.begin(); vertices.end () != vertex; ++vertex)
					{
						pNewHatch->set_pathedgeplinevert(i, vertexIndex, vertex->x, vertex->y, ((bHasBulges == true) ? bulges[vertexIndex] : 0.0));
						
						vertexIndex++;
					}
					vertices.clear();
					bulges.clear();
				}
				else
				{
					pHatch->getLoopAt(i, edgePtrs);

					int edgeCount = edgePtrs.size();

					pNewHatch->set_path(i, loopType, edgeCount);

					for (int j = 0; j < edgeCount; j++)
					{
						OdGeCurve2d* pCurve = edgePtrs.getAt(j);

						OdGe::EntityId curveType = pCurve->type();

						switch(pCurve->type())
						{
						case OdGe::kLineSeg2d:
							{
								OdGeLineSeg2d* pLine = (OdGeLineSeg2d*)pCurve;
								OdGePoint2d startPt = pLine->startPoint();
								OdGePoint2d endPt   = pLine->endPoint();

								pNewHatch->set_pathedgeline(i, j, startPt.x, startPt.y, endPt.x, endPt.y);
							}
						break;
						
						case OdGe::kNurbCurve2d:
							{
								OdGeNurbCurve2d* pSpline = (OdGeNurbCurve2d*)pCurve;
								ASSERT(pSpline != NULL);

								int degree = pSpline->degree();
								int numKnots = pSpline->numKnots();
								int numPoints = pSpline->numControlPoints();
								bool bRational = pSpline->isRational() == true;
								
								pNewHatch->set_pathedgespline(	i, j, 
																bRational, 
																false, // TDO MHB ???pTmpSpline->isPeriodic(), 
																degree, 
																numKnots, 
																numPoints);


								for (long k = 0; k < numKnots; k++) 
								{
									pNewHatch->set_pathedgesplineknot(i, j, k, pSpline->knotAt(k));
								}

								double weight;
								OdGePoint2d point2D;
								for (k = 0; k < numPoints; k++) {
									point2D = pSpline->controlPointAt(k);

									pNewHatch->set_pathedgesplinecontrol(i, j, k, point2D.x, point2D.y);

									
									if (bRational) /* weight */
									{ 
										weight = pSpline->weightAt(k);

										pNewHatch->set_pathedgesplineweight(i, j, k, weight);
									}

								}
							}
						break;
						case OdGe::kCircArc2d:
							{
								OdGeCircArc2d* pTmpArc = (OdGeCircArc2d*)edgePtrs.getAt(j);
								ASSERT(pTmpArc != NULL);

								OdGePoint2d center = pTmpArc->center();

								pNewHatch->set_pathedgearc(	i, j, center.x, center.y, 
															pTmpArc->radius(), 
															pTmpArc->startAngFromXAxis(), 
															pTmpArc->endAngFromXAxis(), 
															!pTmpArc->isClockWise()		// Icad uses counter-clockwise.
														  );
							}
						break;
						case OdGe::kEllipArc2d:
							{
								OdGeEllipArc2d* pTmpArc = (OdGeEllipArc2d*)edgePtrs.getAt(j);
								ASSERT(pTmpArc != NULL);

								OdGePoint2d center = pTmpArc->center();
								OdGeVector2d majorAxis = pTmpArc->majorAxis();
								double minMajRatio = pTmpArc->minorRadius() / pTmpArc->majorRadius();

								pNewHatch->set_pathedgeellarc(	i, j, 
																center.x, center.y, 
																majorAxis.x, majorAxis.y,
																minMajRatio, 
																pTmpArc->startAng(), 
																pTmpArc->endAng(),
																!pTmpArc->isClockWise()	// Icad uses counter-clockwise.
															 );
							}
						break;
						// TODO MHB
						// I'm not sure what these are?
						//				else if (pCurve->isKindOf(OdGeCompositCurve2d::desc()))
						//				{
						//				}
						//				else if (curveType == OdGe::kExternalCurve2d)
						//				{
						//				}
						//				else if (curveType == OdGe::kOffsetCurve2d)
						//				{
						//				}
						}	// switch(pCurve->type())
					}				
				}
			}
			catch (OdError& err)
			{
				OdError err1 = err; // Shut the compiler up.
#ifdef _DEBUG_MHB
				OdString msg = g_pProtoExServices.getErrorDescription(err.code()).c_str();
				ASSERT(!"DWGDirect Hatch createHanditem() error: ");					
#endif
				delete pNewHatch;
				return NULL;
			}
			
		}	// for (int i = 0; i < loopCount; i++)

		OdDbObjectIdArray assocObjects;
		pHatch->getAssocObjIds(assocObjects);

		i = 0;
		int objCount = assocObjects.size();
		if (objCount > 0)
		{
			OdString stringHandle;
			db_hatchboundobjslink *objectsLink = new db_hatchboundobjslink(pDb);

			for (int j = 0; j < objCount; j++)
			{
				stringHandle = assocObjects.getAt(j).getHandle().ascii();	
				objectsLink->addBoundaryFixup(pDb,db_objhandle(stringHandle.c_str()));
			}

			pNewHatch->set_pathBoundaryAndAssociate(i, j, objectsLink);
		}
		assocObjects.clear();                  

		int patternDefs = pHatch->numPatternDefinitions();

		OdGeDoubleArray dashes;
		double angle, baseX, baseY, offsetX, offsetY;

		for (int j = 0; j < patternDefs; j++)
		{
			pHatch->getPatternDefinitionAt( j,
											angle,
											baseX,
											baseY,
											offsetX,
											offsetY,
											dashes
										  );

			int dashCount = dashes.size();

			pNewHatch->set_patline(	j, angle,
									baseX, baseY,
									offsetX,offsetY,
									dashCount);

			for (int k = 0; k < dashCount; k++) 
			{
				pNewHatch->set_patlinedash(j,k,dashes.getAt(k));
			}
		}

		dashes.clear();

		if (seedCount)
		{
			OdGePoint2d point2D;
			double elev = pHatch->elevation();
			sds_point* seeds = new sds_point[seedCount * sizeof(sds_point)];
			for (int i = 0; i < seedCount; i++)
			{
				CPYPT2D(seeds[i], pHatch->getSeedPointAt(i));
			}

			pNewHatch->set_10(seeds,seedCount);
			delete [] seeds;
		}


		sds_point extrusion;
		CPYPT3D(extrusion, pHatch->normal());
		pNewHatch->init_210(extrusion);

		pNewHatch->set_62(pHatch->color().colorIndex());

		finalize(pEntity, pNewHatch, pDb, pBlockRecord); 

		return pNewHatch;

	}		// createHanditem()
};			// end class OdDbHatch_Creator

class OdDbImage_Creator : public OdDbEntity_Creator
{
public:
	OdDbImage_Creator() {}

	db_handitem* createHanditem(OdDbEntity* pEntity, db_drawing* pDb, db_blocktabrec* pBlockRecord) const
	{
		ASSERT(pEntity != NULL);
		ASSERT(pDb != NULL);

		OdDbRasterImagePtr pImage = pEntity;

		db_image* pNewImage = new db_image();
		ASSERT(pNewImage != NULL);
		if (pNewImage == NULL)
			return NULL;

		// TODO MHB
		// pNewImage->set_90(aden.image.classversion);
		
		sds_point point;
		OdGePoint3d origin;
		OdGeVector3d uVector, vVector;
		pImage->getOrientation(origin, uVector, vVector);

		sds_point imageSize;
		CPYPT2D(imageSize, pImage->imageSize());
		pNewImage->set_13(imageSize);

		CPYPT3D(point, origin);
		pNewImage->set_10(point);
		
		uVector.x /= imageSize[0];
		uVector.y /= imageSize[1];
		CPYPT3D(point, uVector);
		pNewImage->set_11(point);

		vVector.x /= imageSize[0];
		vVector.y /= imageSize[1];
		CPYPT3D(point, vVector);
		pNewImage->set_12(point);

		OdString stringHandle = pImage->imageDefId().getHandle().ascii();
        if (pNewImage->set_340byhand(db_objhandle(stringHandle.c_str()),pDb) == FALSE) {
            // Set the rebuild handle list flag, and call it again.
            pNewImage->set_340byhand(db_objhandle(stringHandle.c_str()),pDb);
        }

		int flags = 0;
		if (pImage->isSetDisplayOpt(OdDbRasterImage::kShow) == true)
			flags |= BIT1;
		if (pImage->isSetDisplayOpt(OdDbRasterImage::kShowUnAligned ) == true)
			flags |= BIT2;
		if (pImage->isSetDisplayOpt(OdDbRasterImage::kClip ) == true)
			flags |= BIT4;
		if (pImage->isSetDisplayOpt(OdDbRasterImage::kTransparent ) == true)
			flags |= BIT8;
        pNewImage->set_70(flags);

		pNewImage->set_71(pImage->clipBoundaryType());
        pNewImage->set_280((pImage->isClipped() == true) ? 1 : 0);
        pNewImage->set_281(pImage->brightness());
        pNewImage->set_282(pImage->contrast());
        pNewImage->set_283(pImage->fade());

		// Prior to setting this images reactor handle, we need to add one to the database.
		// Each instance of an image has it's own reactor.
		///////////////////////////////////////////////////////////////////////
		OdDbObjectId pImageReactorId = pImage->reactorId();
		ASSERT(!pImageReactorId.isNull());

		db_imagedef_reactor* pNewImageDefReactor = new db_imagedef_reactor();
		ASSERT(pNewImageDefReactor != NULL);

		OdDbRasterImageDefReactorPtr pImageDefReactor = pImageReactorId.safeOpenObject();
		DWGdirectReader::TransferHandleFromDWGdirect(pNewImageDefReactor, pImageDefReactor);
		DWGdirectReader::TransferEEDataFromDWGdirect(pDb, pNewImageDefReactor, pImageDefReactor);
		DWGdirectReader::TransferReactorsFromDWGdirect(pDb, pNewImageDefReactor, pImageDefReactor, DB_HARD_POINTER);

		// TODO MHB 2 == R14???
		pNewImageDefReactor->set_90(2);
	
		// Now set the soft pointer to the image.
		stringHandle = pImage->objectId().getHandle().ascii();
		db_hireflink* pImageLink = new db_hireflink(DB_SOFT_POINTER, db_objhandle(stringHandle.c_str()), pNewImage);
		pNewImageDefReactor->addhiref(pImageLink);

		ASSERT(pDb->handent(db_objhandle(stringHandle.c_str())) == NULL);
		pDb->addhanditem(pNewImageDefReactor);
		///////////////////////////////////////////////////////////////////////

		stringHandle = pImageReactorId.getHandle().ascii();
        if (pNewImage->set_360byhand(db_objhandle(stringHandle.c_str()),pDb) == FALSE) {
            // Set the rebuild handle list flag, and call it again.
            pNewImage->set_360byhand(db_objhandle(stringHandle.c_str()),pDb);
        }

		OdDbRasterImage::ClipBoundaryType boundaryType = pImage->clipBoundaryType();

		if (boundaryType != OdDbRasterImage::kInvalid)
		{
			OdGePoint2dArray clipPoints = pImage->clipBoundary();

			int count = clipPoints.size();
			ASSERT(count > 0);

			sds_point* pPt= new sds_point [count * sizeof(sds_point)];
			
			for (int i = 0; i < count; i++)
			{
				pPt[i][0] = clipPoints.getAt(i).x;
				pPt[i][1] = clipPoints.getAt(i).y;
				pPt[i][2] = 0.0;
			}

            pNewImage->set_14(pPt,count);
			delete [] pPt;
		}

		pNewImage->set_62(pImage->color().colorIndex());

		finalize(pEntity, pNewImage, pDb, pBlockRecord); 

		return pNewImage;
	}
}; // end class OdDbImage_Creator

class OdDbArcAlignedText_Creator : public OdDbEntity_Creator
{
public:
	OdDbArcAlignedText_Creator() {}

	db_handitem* createHanditem(OdDbEntity* pEntity, db_drawing* pDb, db_blocktabrec* pBlockRecord) const
	{
		ASSERT(pEntity != NULL);
		ASSERT(pDb != NULL);

		OdDbArcAlignedTextPtr pText = pEntity;

		db_arcalignedtext* pNewText = new db_arcalignedtext();
		ASSERT(pNewText != NULL);
		if (pNewText == NULL)
			return NULL;

		pNewText->set_text((char*)pText->textString().c_str());

		OdString typeFace;
		bool bold;
		bool italic;
		int  charSet;
		int  pitchAndFamily;
		pText->font(typeFace, bold, italic, charSet, pitchAndFamily);
		pNewText->set_fontname((char*)typeFace.c_str());
		pNewText->set_bigfontname((char*)pText->bigFontFileName().c_str());

		OdString stringHandle = pText->textStyle().getHandle().ascii();
		pNewText->set_textstyleobjhandle(db_objhandle(stringHandle.c_str()));

		sds_point point;
		CPYPT3D(point, pText->center());
		pNewText->set_pt0(point);

		pNewText->set_radius(pText->radius());
		pNewText->set_widthfactor(pText->xScale());
		pNewText->set_height(pText->textSize());
		pNewText->set_charspacing(pText->charSpacing());
		pNewText->set_offsetfromarc(pText->offsetFromArc());
        pNewText->set_rightoffset(pText->rightOffset());
        pNewText->set_leftoffset(pText->leftOffset());
        pNewText->set_startangle(pText->startAngle());
        pNewText->set_endangle(pText->endAngle());

		// TODO MHB verify this value?
		bool bReversOrder = pText->isReversedCharOrder();
		pNewText->set_charorder((bReversOrder == true) ? 1 : 0);

		pNewText->set_direction(pText->textDirection());
		pNewText->set_alignment(pText->alignment());
		pNewText->set_side(pText->textPosition());
		pNewText->set_bold((bold == true) ? 1 : 0);
		pNewText->set_italic((italic == true) ? 1 : 0);
		pNewText->set_underline((pText->isUnderlined() == true) ? 1 : 0);
		pNewText->set_charset(charSet);
		pNewText->set_pitch(pitchAndFamily);

		// TODO MHB verify this.
		pNewText->set_fonttype((pText->isShxFont()) ? 1 : 0);

		pNewText->set_color(pText->color().colorIndex());

		// TODO MHB verify this.
		pNewText->set_wizardflag((pText->wizardFlag() == true) ? 1 : 0);

		// Used by ODT - can be removed when DWGdirect output works.
		pNewText->set_grblob(NULL);
		pNewText->set_grblobsz(0);

		stringHandle = pText->arcId().getHandle().ascii();
        pNewText->set_arcobjhandle(db_objhandle(stringHandle.c_str()));

		sds_point extrusion;
		CPYPT3D(extrusion, pText->normal());
		pNewText->init_210(extrusion);

		pNewText->set_62(pText->color().colorIndex());

		finalize(pEntity, pNewText, pDb, pBlockRecord); 

		return pNewText;
	}
}; // end class OdDbArcAlignedText_Creator

class RText_Creator : public OdDbEntity_Creator
{
public:
	RText_Creator() {}

	db_handitem* createHanditem(OdDbEntity* pEntity, db_drawing* pDb, db_blocktabrec* pBlockRecord) const
	{
		ASSERT(pEntity != NULL);
		ASSERT(pDb != NULL);
	
		RText* pText = (RText*)pEntity;

		db_rtext* pNewText = new db_rtext();
		ASSERT(pNewText != NULL);
		if (pNewText == NULL)
			return NULL;


		sds_point point;
		CPYPT3D(point, pText->getPoint());
		pNewText->set_pt0(point);

        pNewText->set_rotang(pText->getRotAngle());
        pNewText->set_height(pText->getHeight());

		OdString stringHandle = pText->textStyleId().getHandle().ascii();
		pNewText->set_textstyleobjhandle(db_objhandle(stringHandle.c_str()));

		// TODO MHB verify this value.
        pNewText->set_typeflag((pText->enabledMTextSequences() == true) ? 1 : 0);

        pNewText->set_contents((char*)pText->getStringContents().c_str());

		sds_point extrusion;
		CPYPT3D(extrusion, pText->normal());
		pNewText->init_210(extrusion);

		pNewText->set_62(pText->color().colorIndex());

		finalize(pEntity, pNewText, pDb, pBlockRecord); 

		return pNewText;
	}
}; // end class RText_Creator

class OdDbWipeout_Creator : public OdDbEntity_Creator
{
public:
	OdDbWipeout_Creator() {}

	db_handitem* createHanditem(OdDbEntity* pEntity, db_drawing* pDb, db_blocktabrec* pBlockRecord) const
	{
		ASSERT(pEntity != NULL);
		ASSERT(pDb != NULL);

		OdDbWipeoutPtr pWipeout = pEntity;

		db_wipeout* pNewWipeout = new db_wipeout();
		ASSERT(pNewWipeout != NULL);
		if (pNewWipeout == NULL)
			return NULL;

		CDxfFiler tmpFiler(pWipeout->database());
		pWipeout->dxfOutFields(&tmpFiler);
		OdResBufPtr pOdResBuf = tmpFiler.getFilerData();

		pNewWipeout->setResbufChain(ReadWriteUtils::getSdsResBufPtrFromOdResBuf(pOdResBuf, pEntity->database()));

		finalize(pEntity, pNewWipeout, pDb, pBlockRecord); 

		return pNewWipeout;
	}
}; // end class OdDbWipeout_Creator

class OdDbCurve_Creator : public OdDbEntity_Creator
{
public:
	OdDbCurve_Creator() {}

	db_handitem* createHanditem(OdDbEntity* pEntity, db_drawing* pDb, db_blocktabrec* pBlockRecord) const
	{
		ASSERT(pEntity != NULL);
		ASSERT(pDb != NULL);

		ASSERT(!"Haven't run into one of these: OdDbCurve_Creator()");
		return NULL;
	}
}; // end class OdDbCurve_Creator

class OdDbFaceRecord_Creator : public OdDbEntity_Creator
{
public:
	OdDbFaceRecord_Creator() {}

	db_handitem* createHanditem(OdDbEntity* pEntity, db_drawing* pDb, db_blocktabrec* pBlockRecord) const
	{
		ASSERT(pEntity != NULL);
		ASSERT(pDb != NULL);

		OdDbFaceRecordPtr pFace = pEntity;

		db_vertex* pNewVertex = new db_vertex();
		ASSERT(pNewVertex != NULL);
		if (pNewVertex == NULL)
			return NULL;

		sds_point point = {0.0,0.0,0.0};
		pNewVertex->set_10(point);

		int i = 0;
		int flags = 0;
		
		pNewVertex->set_71(pFace->getVertexAt(i++));
		pNewVertex->set_72(pFace->getVertexAt(i++));
		pNewVertex->set_73(pFace->getVertexAt(i++));
		pNewVertex->set_74(pFace->getVertexAt(i));

		i = 0;
		if (pFace->isEdgeVisibleAt(i++) == false)
			flags |= BIT1;
		if (pFace->isEdgeVisibleAt(i++) == false)
			flags |= BIT2;
		if (pFace->isEdgeVisibleAt(i++) == false)
			flags |= BIT4;
		if (pFace->isEdgeVisibleAt(i) == false)
			flags |= BIT8;
		flags |= BIT80;			// Is Polyface mesh vertex
		pNewVertex->set_70(flags);

		pNewVertex->set_62(pFace->color().colorIndex());

		finalize(pEntity, pNewVertex, pDb, pBlockRecord); 

		return pNewVertex;
	}
}; // end class OdDbFaceRecord_Creator

class OdDbFrame_Creator : public OdDbEntity_Creator
{
public:
	OdDbFrame_Creator() {}

	db_handitem* createHanditem(OdDbEntity* pEntity, db_drawing* pDb, db_blocktabrec* pBlockRecord) const
	{
		ASSERT(pEntity != NULL);
		ASSERT(pDb != NULL);

		OdDbOle2FramePtr pFrame = pEntity;

		db_ole2frame* pNewFrame = new db_ole2frame();
		ASSERT(pNewFrame != NULL);
		if (pNewFrame == NULL)
			return NULL;

		pNewFrame->set_unk1(pFrame->getUnknown1());
		pNewFrame->set_unk2(pFrame->getUnknown2());

		sds_point point;
		OdGePoint3d point3D;

		OdRectangle3d rect;
		pFrame->position(rect);

		pFrame->getLocation(point3D);

		CPYPT3D(point, rect.upLeft);
		pNewFrame->set_10(point);

		CPYPT3D(point, rect.lowRight);
		pNewFrame->set_11(point);

		CPYPT3D(point, rect.upRight);
		pNewFrame->set_ur(point);

		CPYPT3D(point, rect.lowLeft);
		pNewFrame->set_ll(point);

		pNewFrame->set_3((char*)pFrame->getUserType().c_str());

		// TODO MHB what other values could this be?
		pNewFrame->set_70(2);

		pNewFrame->set_71(pFrame->getType());
		pNewFrame->set_72(pFrame->getModelFlag());

		pNewFrame->set_pixelwidth(pFrame->getPixelWidth());
		pNewFrame->set_pixelheight(pFrame->getPixelHeight());
		pNewFrame->set_modelflag(pFrame->getModelFlag());
		pNewFrame->set_colorsnum(pFrame->getColorsNum());
		pNewFrame->set_counter(pFrame->getCounter());
		pNewFrame->set_unknown6(pFrame->getUnknown6());
		pNewFrame->set_unknown7(pFrame->getUnknown7());
		pNewFrame->set_aspect(pFrame->getAspect());
		pNewFrame->set_outputquality(pFrame->outputQuality());

		//---------------------------------------------------------------
		// Now turn these calls into a blob which ODT understands.
		// Once ODT output is converted to DWGdirect, this should be
		// removed. The above 8 calls will replace the calls below.
		//---------------------------------------------------------------
		// Follow up. IntelliCAD uses this blob during display of the
		// frame. That code must be updated to use the new DWGdirect
		// variables. TODO - MHB
		//---------------------------------------------------------------
		char* t;
		char pBuffUnk[26];
		t = pBuffUnk;
		BLOBINT(t, pFrame->getPixelWidth());

		t = pBuffUnk+2;
		BLOBINT(t, pFrame->getPixelHeight());

		t = pBuffUnk+4;
		BLOBLONG(t, pFrame->getModelFlag());

		t = pBuffUnk+8;
		BLOBLONG(t, pFrame->getColorsNum());

		t = pBuffUnk+12;
		BLOBLONG(t, pFrame->getCounter());

		t = pBuffUnk+16;
		BLOBLONG(t, pFrame->getUnknown6());

		t = pBuffUnk+20;
		BLOBINT(t, pFrame->getUnknown7());

		t = pBuffUnk+22;
		BLOBLONG(t, pFrame->getAspect());

		pNewFrame->set_unk3(pBuffUnk);
		//---------------------------------------------------------------

		OdByteDataPtr pFrameData = pFrame->compDocFile(false);

		if (!pFrameData.isNull())
		{
			long dataSize = pFrameData->size();
			unsigned char* dataBuffer = new unsigned char[dataSize];
			try
			{
				memset(dataBuffer, 0, dataSize);
				pFrameData->readAt(0, dataBuffer, dataSize);

				pNewFrame->set_310((char*)dataBuffer,dataSize);

				delete [] dataBuffer;
			}
			catch(...)
			{
				delete pNewFrame;
				if (dataBuffer != NULL)
					delete [] dataBuffer;
				return NULL;
			}
		}

		pNewFrame->set_62(pFrame->color().colorIndex());

		// TODO - MHB
		// Should add a couple of char* variables to the
		// icad db_ole2frame class to hold the Raster image preview
		// if any and the Enhanced metafile preview if any. DWGdirect
		// can get at them as streams which we can make char* s out of.
		//pFrame->getRasterImage();
		//pFrame->getEnhancedMetafile();

		finalize(pEntity, pNewFrame, pDb, pBlockRecord); 

		return pNewFrame;
	}
}; // end class OdDbFrame_Creator

class OdDbBlockBegin_Creator : public OdDbEntity_Creator
{
public:
	OdDbBlockBegin_Creator() {}

	db_handitem* createHanditem(OdDbEntity* pEntity, db_drawing* pDb, db_blocktabrec* pBlockRecord) const
	{
		ASSERT(pEntity != NULL);
		ASSERT(pDb != NULL);

		ASSERT(!"Haven't run into one of these: OdDbBlockBegin_Creator()");
		return NULL;
	}
}; // end class OdDbBlockBegin_Creator

class OdDbBlockEnd_Creator : public OdDbEntity_Creator
{
public:
	OdDbBlockEnd_Creator() {}

	db_handitem* createHanditem(OdDbEntity* pEntity, db_drawing* pDb, db_blocktabrec* pBlockRecord) const
	{
		ASSERT(pEntity != NULL);
		ASSERT(pDb != NULL);

		ASSERT(!"Haven't run into one of these: OdDbBlockEnd_Creator()");
		return NULL;
	}
}; // end class OdDbBlockEnd_Creator

class OdDbFcf_Creator : public OdDbEntity_Creator
{
public:
	OdDbFcf_Creator() {}

	db_handitem* createHanditem(OdDbEntity* pEntity, db_drawing* pDb, db_blocktabrec* pBlockRecord) const
	{
		ASSERT(pEntity != NULL);
		ASSERT(pDb != NULL);

		OdDbFcfPtr pTolerance = pEntity;

		db_tolerance *pNewTolerance = new db_tolerance();
		ASSERT(pNewTolerance != NULL);
		if (pNewTolerance == NULL)
			return NULL;

        pNewTolerance->set_1((char*)pTolerance->text().c_str());

		OdString styleMane = DWGdirectReader::GetDimStyleNameFromDWGdirectId(pTolerance->dimensionStyle());
        pNewTolerance->set_3((char*)styleMane.c_str(),pDb);

		sds_point point;
		CPYPT3D(point, pTolerance->location());
		pNewTolerance->set_10(point);

		CPYPT3D(point, pTolerance->direction());
        pNewTolerance->set_11(point);

        pNewTolerance->set_textht(pTolerance->dimtxt());

		pNewTolerance->set_62(pTolerance->color().colorIndex());

		CPYPT3D(point, pTolerance->normal());
		pNewTolerance->init_210(point);

		pNewTolerance->set_62(pTolerance->color().colorIndex());

		finalize(pEntity, pNewTolerance, pDb, pBlockRecord); 

		return pNewTolerance;
	}
}; // end class OdDbFcf_Creator

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
// This method is called for all entities that have not overridden the createHanditem.
// If we hit this assert, we missed an entity, and need to add a handler for it. 
// DO NOT DELETE THE RETURNED db_handitem IT HAS BEEN ADDED TO THE DATABASE OR BLOCK RECORD!!!!!!!
db_handitem* OdDbEntity_Creator::createHanditem(OdDbEntity* pEnt, db_drawing* pDb, db_blocktabrec* pBlockRecord) const
{
	ASSERT(!"Protocol Extension for this entity doesn't exit yet");
	return NULL;
}

// This function wraps up the common things for a given db_entity, and adds it to the database. 
void OdDbEntity_Creator::finalize(OdDbEntity* pEntity, db_handitem* pEnt, db_drawing* pDb, db_blocktabrec* pBlockRecord, bool bAddToDatabase)
{
	ASSERT(pEnt != NULL);
	ASSERT(pDb != NULL);

	db_objhandle entityHandle = db_objhandle(pEntity->objectId().getHandle().ascii());
	DWGdirectReader::TransferHandleFromDWGdirect(pEnt, pEntity);

	OdDbObjectId layerId = pEntity->layerId();
	ASSERT(!layerId.isNull());
	db_handitem* layer = pDb->handent(db_objhandle(layerId.getHandle().ascii()));
	ASSERT(layer != NULL);
	if (layer == NULL)
	{
		layer = pDb->findtabrec(DB_LAYERTAB, OdDbSymUtil::layerZeroName(), 1);
	}
    ((db_entity*)pEnt)->set_layerhip(layer);

	OdDbObjectId linetypeId = pEntity->linetypeId();
	ASSERT(!linetypeId.isNull());
	db_handitem* linetype = pDb->handent(db_objhandle(linetypeId.getHandle().ascii()));
	ASSERT(linetype != NULL);
	if (linetype == NULL)
	{
		linetype = pDb->findtabrec(DB_LTYPETAB, OdDbSymUtil::linetypeContinuousName(), 1);
	}
    ((db_entity*)pEnt)->set_ltypehip(linetype);

    ((db_entity*)pEnt)->set_48(pEntity->linetypeScale());
    ((db_entity*)pEnt)->set_60(pEntity->visibility());
	((db_entity*)pEnt)->set_370(pEntity->lineWeight());

	OdString plotStyleName = pEntity->plotStyleName();
	if (plotStyleName.iCompare(OdDbSymUtil::linetypeByBlockName()) == 0)
	{
		((db_entity*)pEnt)->set_380(1);		//DB_BB_PLSTYLE
	}
	else if (plotStyleName.iCompare(db_str_bylayer) == 0)
	{
		((db_entity*)pEnt)->set_380(0);		// DB_BL_PLSTYLE
	}
	else
	{
		((db_entity*)pEnt)->set_380(0);		// DB_NM_PLSTYLE

		OdDbObjectId plotNameId;
		pEntity->getPlotStyleNameId(plotNameId);
		if (!plotNameId.isNull())
		{
			((db_entity*)pEnt)->set_380(3); // DB_NM_PLSTYLE

			OdString stringHandle = plotNameId.getHandle().ascii();
			
			ASSERT(pDb->handent(db_objhandle(stringHandle.c_str())) != NULL);
			db_placeholder* plotStyle = (db_placeholder*)pDb->handent(db_objhandle(stringHandle.c_str()));
			ASSERT(plotStyle != NULL);

			if (plotStyle != NULL)
			{
				((db_entity*)pEnt)->set_390((db_handitem*)plotStyle);
			}
		}
	}

	// Extension dictionary.
	DWGdirectReader::TransferExtensionDictionariesFromDWGdirect(pEnt, pEntity, pDb);

	// Extended entity data.
	DWGdirectReader::TransferEEDataFromDWGdirect(pDb, pEnt, pEntity);

	// Reactors that may be on the style record.
	DWGdirectReader::TransferReactorsFromDWGdirect(pDb, pEnt, pEntity, DB_HARD_POINTER);

	if (bAddToDatabase == true)	// Can be false for example for vertices
	{
//		ASSERT(pDb->handent(entityHandle) == NULL);

		// Now add this new entity to the database.
		if (pBlockRecord != NULL)
		{
			pBlockRecord->addent(pEnt, pDb);
		}
		else
		{
			pDb->addhanditem(pEnt);
		}
	}

	return;
}

class Creators
{
	// Create custom Creator instances
	OdStaticRxObject<OdDbEntity_Creator> m_pEntityCreator;

	OdStaticRxObject<OdDbLine_Creator>  m_pLineCreator;
	OdStaticRxObject<OdDbPoint_Creator>  m_pPointCreator;
	OdStaticRxObject<OdDbCircle_Creator>  m_pCircleCreator;
	OdStaticRxObject<OdDbArc_Creator>  m_pArcCreator;
	OdStaticRxObject<OdDbTrace_Creator>  m_pTraceCreator;
	OdStaticRxObject<OdDbSolid_Creator>  m_pSolidCreator;
	OdStaticRxObject<OdDbFace_Creator>  m_pFaceCreator;
	OdStaticRxObject<OdDbSequenceEnd_Creator>  m_pSequenceEndCreator;
	OdStaticRxObject<OdDbPolyline_Creator>  m_pPolylineCreator;
	OdStaticRxObject<OdDbVertex_Creator>  m_pVertexCreator;
	OdStaticRxObject<OdDbEllipse_Creator>  m_pEllipseCreator;
	OdStaticRxObject<OdDbSpline_Creator>  m_pSplineCreator;
	OdStaticRxObject<OdDbRay_Creator>  m_pRayCreator;
	OdStaticRxObject<OdDbXline_Creator>  m_pXlineCreator;
	OdStaticRxObject<OdDbShape_Creator>  m_pShapeCreator;
	OdStaticRxObject<OdDbText_Creator>  m_pTextCreator;
	OdStaticRxObject<OdDbBlockReference_Creator>  m_pBlockReferenceCreator;
	OdStaticRxObject<OdDbAttributeDefinition_Creator>  m_pAttributeDefinitionCreator;
	OdStaticRxObject<OdDbAttribute_Creator>  m_pAttributeCreator;
	OdStaticRxObject<OdDbDimension_Creator>  m_pDimensionCreator;
	OdStaticRxObject<OdDbViewport_Creator>  m_pViewportCreator;
	OdStaticRxObject<OdDbMText_Creator>  m_pMTextCreator;
	OdStaticRxObject<OdDbLeader_Creator>  m_pLeaderCreator;
	OdStaticRxObject<OdDb3dSolid_Creator>  m_p3DSolidCreator;
	OdStaticRxObject<OdDbRegion_Creator>  m_pRegionCreator;
	OdStaticRxObject<OdDbBody_Creator>  m_pBodyCreator;
	OdStaticRxObject<OdDbMline_Creator>  m_pMlineCreator;
	OdStaticRxObject<OdDbProxyEntity_Creator>  m_pProxyEntityCreator;
	OdStaticRxObject<OdDb2dPolyline_Creator>  m_p2dPolylineCreator;
	OdStaticRxObject<OdDbHatch_Creator>  m_pHatchCreator;
	OdStaticRxObject<OdDbImage_Creator>  m_pImageCreator;
	OdStaticRxObject<OdDbArcAlignedText_Creator>  m_pArcAlignedTextCreator;
	OdStaticRxObject<RText_Creator>  m_pRTextCreator;
	OdStaticRxObject<OdDbWipeout_Creator>  m_pWipeoutCreator;
	OdStaticRxObject<OdDbMInsertBlock_Creator>  m_pMInsertBlockCreator;
	OdStaticRxObject<OdDbPolyFaceMesh_Creator>  m_pPolyFaceMeshCreator;
	OdStaticRxObject<OdDb3dPolyline_Creator>  m_p3dPolylineCreator;
	OdStaticRxObject<OdDbPolygonMesh_Creator>  m_pPolygonMeshCreator;

	OdStaticRxObject<OdDbCurve_Creator>  m_pCurveCreator;
	OdStaticRxObject<OdDbFaceRecord_Creator>  m_pFaceRecordCreator;
	OdStaticRxObject<OdDbFrame_Creator>  m_pFrameCreator;
	OdStaticRxObject<OdDbBlockBegin_Creator>  m_pBlockBeginCreator;
	OdStaticRxObject<OdDbBlockEnd_Creator>  m_pBlockEndCreator;
	OdStaticRxObject<OdDbFcf_Creator>  m_pFcfCreator;

public:
void addXs()
{
	// Register the Creator instances
	OdDbEntity::desc()->addX(OdDbEntity_Creator::desc(),		&m_pEntityCreator);
	OdDbLine::desc()->addX(OdDbEntity_Creator::desc(),			&m_pLineCreator);
	OdDbPoint::desc()->addX(OdDbEntity_Creator::desc(),			&m_pPointCreator);
	OdDbCircle::desc()->addX(OdDbEntity_Creator::desc(),		&m_pCircleCreator);
	OdDbArc::desc()->addX(OdDbEntity_Creator::desc(),			&m_pArcCreator);
	OdDbTrace::desc()->addX(OdDbEntity_Creator::desc(),			&m_pTraceCreator);
	OdDbSolid::desc()->addX(OdDbEntity_Creator::desc(),			&m_pSolidCreator);
	OdDbFace::desc()->addX(OdDbEntity_Creator::desc(),			&m_pFaceCreator);
	OdDbSequenceEnd::desc()->addX(OdDbEntity_Creator::desc(),	&m_pSequenceEndCreator);
	OdDbPolyline::desc()->addX(OdDbEntity_Creator::desc(),		&m_pPolylineCreator);
	OdDbVertex::desc()->addX(OdDbEntity_Creator::desc(),		&m_pVertexCreator);
	OdDbEllipse::desc()->addX(OdDbEntity_Creator::desc(),		&m_pEllipseCreator);
	OdDbSpline::desc()->addX(OdDbEntity_Creator::desc(),		&m_pSplineCreator);
	OdDbRay::desc()->addX(OdDbEntity_Creator::desc(),			&m_pRayCreator);
	OdDbXline::desc()->addX(OdDbEntity_Creator::desc(),			&m_pXlineCreator);
	OdDbShape::desc()->addX(OdDbEntity_Creator::desc(),			&m_pShapeCreator);
	OdDbBlockReference::desc()->addX(OdDbEntity_Creator::desc(),&m_pBlockReferenceCreator);
	OdDbAttributeDefinition::desc()->addX(OdDbEntity_Creator::desc(),&m_pAttributeDefinitionCreator);
	OdDbAttribute::desc()->addX(OdDbEntity_Creator::desc(),		&m_pAttributeCreator);
	OdDbDimension::desc()->addX(OdDbEntity_Creator::desc(),		&m_pDimensionCreator);
	OdDbViewport::desc()->addX(OdDbEntity_Creator::desc(),		&m_pViewportCreator);
	OdDbMText::desc()->addX(OdDbEntity_Creator::desc(),			&m_pMTextCreator);
	OdDbText::desc()->addX(OdDbEntity_Creator::desc(),			&m_pTextCreator);
	OdDbLeader::desc()->addX(OdDbEntity_Creator::desc(),		&m_pLeaderCreator);
	OdDb3dSolid::desc()->addX(OdDbEntity_Creator::desc(),		&m_p3DSolidCreator);
	OdDbRegion::desc()->addX(OdDbEntity_Creator::desc(),		&m_pRegionCreator);
	OdDbBody::desc()->addX(OdDbEntity_Creator::desc(),			&m_pBodyCreator);
	OdDbMline::desc()->addX(OdDbEntity_Creator::desc(),			&m_pMlineCreator);
	OdDbMline::desc()->addX(OdDbEntity_Creator::desc(),			&m_pMlineCreator);
	OdDbProxyEntity::desc()->addX(OdDbEntity_Creator::desc(),	&m_pProxyEntityCreator);
	OdDb2dPolyline::desc()->addX(OdDbEntity_Creator::desc(),	&m_p2dPolylineCreator);
	OdDbHatch::desc()->addX(OdDbEntity_Creator::desc(),			&m_pHatchCreator);
	OdDbImage::desc()->addX(OdDbEntity_Creator::desc(),			&m_pImageCreator);
	OdDbArcAlignedText::desc()->addX(OdDbEntity_Creator::desc(),&m_pArcAlignedTextCreator);
	RText::desc()->addX(OdDbEntity_Creator::desc(),				&m_pRTextCreator);
	OdDbWipeout::desc()->addX(OdDbEntity_Creator::desc(),		&m_pWipeoutCreator);
	OdDbMInsertBlock::desc()->addX(OdDbEntity_Creator::desc(),	&m_pMInsertBlockCreator);
	OdDbPolyFaceMesh::desc()->addX(OdDbEntity_Creator::desc(),  &m_pPolyFaceMeshCreator);
	OdDb3dPolyline::desc()->addX(OdDbEntity_Creator::desc(),	&m_p3dPolylineCreator);
	OdDbPolygonMesh::desc()->addX(OdDbEntity_Creator::desc(),	&m_pPolygonMeshCreator);

	OdDbCurve::desc()->addX(OdDbEntity_Creator::desc(),			&m_pCurveCreator);
	OdDbFaceRecord::desc()->addX(OdDbEntity_Creator::desc(),	&m_pFaceRecordCreator);
	OdDbFrame::desc()->addX(OdDbEntity_Creator::desc(),			&m_pFrameCreator);
	OdDbBlockBegin::desc()->addX(OdDbEntity_Creator::desc(),	&m_pBlockBeginCreator);
	OdDbBlockEnd::desc()->addX(OdDbEntity_Creator::desc(),		&m_pBlockEndCreator);
	OdDbFcf::desc()->addX(OdDbEntity_Creator::desc(),			&m_pFcfCreator);
}

void delXs()
{
	OdDbEntity::desc()->delX(OdDbEntity_Creator::desc());	
	OdDbLine::desc()->delX(OdDbEntity_Creator::desc());	
	OdDbPoint::desc()->delX(OdDbEntity_Creator::desc());
	OdDbCircle::desc()->delX(OdDbEntity_Creator::desc());	
	OdDbArc::desc()->delX(OdDbEntity_Creator::desc());	
	OdDbTrace::desc()->delX(OdDbEntity_Creator::desc());
	OdDbSolid::desc()->delX(OdDbEntity_Creator::desc());	
	OdDbFace::desc()->delX(OdDbEntity_Creator::desc());	
	OdDbSequenceEnd::desc()->delX(OdDbEntity_Creator::desc());
	OdDbPolyline::desc()->delX(OdDbEntity_Creator::desc());
	OdDbVertex::desc()->delX(OdDbEntity_Creator::desc());	
	OdDbEllipse::desc()->delX(OdDbEntity_Creator::desc());
	OdDbSpline::desc()->delX(OdDbEntity_Creator::desc());
	OdDbRay::desc()->delX(OdDbEntity_Creator::desc());	
	OdDbXline::desc()->delX(OdDbEntity_Creator::desc());	
	OdDbShape::desc()->delX(OdDbEntity_Creator::desc());	
	OdDbBlockReference::desc()->delX(OdDbEntity_Creator::desc());
	OdDbAttributeDefinition::desc()->delX(OdDbEntity_Creator::desc());
	OdDbAttribute::desc()->delX(OdDbEntity_Creator::desc());
	OdDbDimension::desc()->delX(OdDbEntity_Creator::desc());
	OdDbViewport::desc()->delX(OdDbEntity_Creator::desc());
	OdDbMText::desc()->delX(OdDbEntity_Creator::desc());	
	OdDbText::desc()->delX(OdDbEntity_Creator::desc());	
	OdDbLeader::desc()->delX(OdDbEntity_Creator::desc());
	OdDb3dSolid::desc()->delX(OdDbEntity_Creator::desc());	
	OdDbRegion::desc()->delX(OdDbEntity_Creator::desc());	
	OdDbBody::desc()->delX(OdDbEntity_Creator::desc());		
	OdDbMline::desc()->delX(OdDbEntity_Creator::desc());	
	OdDbMline::desc()->delX(OdDbEntity_Creator::desc());	
	OdDbProxyEntity::desc()->delX(OdDbEntity_Creator::desc());
	OdDb2dPolyline::desc()->delX(OdDbEntity_Creator::desc());
	OdDbHatch::desc()->delX(OdDbEntity_Creator::desc());
	OdDbImage::desc()->delX(OdDbEntity_Creator::desc());	
	OdDbArcAlignedText::desc()->delX(OdDbEntity_Creator::desc());
	RText::desc()->delX(OdDbEntity_Creator::desc());		
	OdDbWipeout::desc()->delX(OdDbEntity_Creator::desc());	
	OdDbMInsertBlock::desc()->delX(OdDbEntity_Creator::desc());
	OdDbPolyFaceMesh::desc()->delX(OdDbEntity_Creator::desc());
	OdDb3dPolyline::desc()->delX(OdDbEntity_Creator::desc());
	OdDbPolygonMesh::desc()->delX(OdDbEntity_Creator::desc());

	OdDbCurve::desc()->delX(OdDbEntity_Creator::desc());	
	OdDbFaceRecord::desc()->delX(OdDbEntity_Creator::desc());
	OdDbFrame::desc()->delX(OdDbEntity_Creator::desc());	
	OdDbBlockBegin::desc()->delX(OdDbEntity_Creator::desc());
	OdDbBlockEnd::desc()->delX(OdDbEntity_Creator::desc());
	OdDbFcf::desc()->delX(OdDbEntity_Creator::desc());
}

};

//////////////////////////////////////////////////////////////////////////////////////////
void DWGdirectProtocolExtension::initialize()
{
	// Register OdDbEntity_Creator with DWGdirect
	OdDbEntity_Creator::rxInit();

	OdRxObjectImpl<DWGdirectProtocolServices> m_pExServices;

	m_pCreators = new Creators;
	m_pCreators->addXs();

	return;
} // end DWGdirectProtocolExtension::initialize()

void DWGdirectProtocolExtension::uninitialize()
{
	m_pCreators->delXs();
	OdDbEntity_Creator::rxUninit();

	delete m_pCreators;
	m_pCreators = 0;

	return;
}



