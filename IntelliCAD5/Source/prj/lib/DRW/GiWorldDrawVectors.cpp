// OdGiEntityGeometry.cpp: implementation of the OdGiEntityGeometry class.
//
//////////////////////////////////////////////////////////////////////
#include "db.h"
#include "layertabrec.h"

#include "theuser.h"
#include "elliparc.h"
#include "line.h"
#include "ptarray.h"

#include "OdaCommon.h"

#include "GeMatrix3d.h"
#include "CmColor.h"
#include "GiDrawable.h"
#include "GiWorldDrawVectors.h"

#include "DbLayerTableRecord.h"
//#include "DbLinetypeTableRecord.h"
//#define _DEBUG_MHB

//#include <elliparc.h>

//----------------------------------------------------------
//
// OdGiEntityGeometry
//
//----------------------------------------------------------
OdGiEntityGeometry::~OdGiEntityGeometry()
{
	if (m_pDisplayVectors.next != NULL)
	{
		// Before we let these go, get a bounding box
		// and set the extents for this too.
		double xmin, ymin, xmax, ymax;
		struct gr_displayobject* tmp = m_pDisplayVectors.next;

		bool minmaxInit = false;
		while(tmp)
		{
			for (int i = 0; i < tmp->npts * 3; i++)
			{
				if (false == minmaxInit)
				{
					minmaxInit = TRUE;
					xmin = xmax = tmp->chain[i];
					ymin = ymax = tmp->chain[++i];
					++i; // 3D point walk past Z
				}
				else
				{
					xmin = min(tmp->chain[i], xmin);
					xmax = max(tmp->chain[i], xmax);
					i++;
					ymin = min(tmp->chain[i], ymin);
					ymax = max(tmp->chain[i], ymax);
					++i; // 3D point walk past Z
				}
			}
			tmp = tmp->next;
		}

		if (minmaxInit == true)
		{
			sds_point ll;
			ll[0] = xmin;
			ll[1] = ymin;
			ll[2] = 0.0;
			sds_point ur;
			ur[0] = xmax;
			ur[1] = ymax;
			ur[2] = 0.0;

			m_pHanditem->set_extent(ll, ur);
		}

		// Ok, now we're done with it.
		if (db_grafreedofp!=NULL) 
			db_grafreedofp(m_pDisplayVectors.next);
	}
	m_pDisplayVectors.next = NULL;
}

void OdGiEntityGeometry::setExtents(const OdGePoint3d *pNewExtents)
{
#ifdef _DEBUG_MHB
	OutputDebugString("OdGiEntityGeometry::setExtents() - Not implemented\n");
#endif
	return;
}

void OdGiEntityGeometry::pushModelTransform(const OdGeVector3d& vNormal)
{
#ifdef _DEBUG_MHB
	OutputDebugString("OdGiEntityGeometry::pushModelTransform() - Not implemented\n");
#endif
	return;
}

void OdGiEntityGeometry::pushModelTransform(const OdGeMatrix3d& xMat)
{
#ifdef _DEBUG_MHB
	OutputDebugString("OdGiEntityGeometry::pushModelTransform() - Not implemented\n");
#endif
	return;
}

void OdGiEntityGeometry::popModelTransform()
{
#ifdef _DEBUG_MHB
	OutputDebugString("OdGiEntityGeometry::popModelTransform() - Not implemented\n");
#endif
	return;
}

void OdGiEntityGeometry::circle(const OdGePoint3d& center, double radius, const OdGeVector3d& normal)
{
	long retVal;
	CEllipse tmpCircle(C3Point(center.x, center.y, center.z), radius, retVal);

	CPointArray ptArray(DIVIDESEGMENTS, retVal);
	tmpCircle.GetPointsWithEqualArcLength(DIVIDESEGMENTS, ptArray);

	FillArray(m_pHanditem, ptArray, &m_pDisplayVectors, getColor());
	
	return;
}
  
void OdGiEntityGeometry::circle(const OdGePoint3d& p1, const OdGePoint3d& p2, const OdGePoint3d& p3)
{
	CLine line1(C3Point(p1.x, p1.y, p1.z), C3Point(p2.x, p2.y, p2.z));
	CLine line2(C3Point(p1.x, p1.y, p1.z), C3Point(p2.x, p2.y, p2.z));
	
	// The intersection of the perpendicular bisectors of the two lines above 
	// is the center of this circle. So get it. We'll let geo do the heavy lifting.

	C3Point midPoint1, midPoint2;	// We'll rotate the lines about their midpoints.
	line1.Evaluate(0.5, midPoint1);
	line2.Evaluate(0.5, midPoint2);
	
	CAffine rotMat;
	rotMat.RotateAbout(PIOVER2, midPoint1);
	line1.Transform(rotMat);
	rotMat.InitializeMatrix();

	rotMat.RotateAbout(PIOVER2, midPoint2);
	line2.Transform(rotMat);

	C3Point centerPoint;
	RC ret = IntersectLines(midPoint1, line1.Direction(), midPoint2, line2.Direction(), centerPoint);
	ASSERT(ret == 0);

	long retVal;
	CEllipse tmpCircle(centerPoint, centerPoint.DistanceTo(C3Point(p1.x, p1.y, p1.z)), retVal);

	CPointArray ptArray(DIVIDESEGMENTS, retVal);
	tmpCircle.GetPointsWithEqualArcLength(DIVIDESEGMENTS, ptArray);

	FillArray(m_pHanditem, ptArray, &m_pDisplayVectors, getColor());
	
	return;
}

void OdGiEntityGeometry::circularArc(const OdGePoint3d& center,
    double radius,
    const OdGeVector3d& normal,
    const OdGeVector3d& startVector,
    double sweepAngle,
    OdGiArcType arcType)
{

	long retVal;
	C3Vector tmpVect(C3Vector(startVector.x, startVector.y, startVector.z));
	double startAngle = tmpVect.Magnitude();


	CEllipArc tmpArc(radius, C3Point(center.x, center.y, center.z),
					 startAngle, startAngle + sweepAngle, retVal);


	CPointArray ptArray(DIVIDESEGMENTS, retVal);
	tmpArc.GetPointsWithEqualArcLength(DIVIDESEGMENTS, ptArray);

	FillArray(m_pHanditem, ptArray, &m_pDisplayVectors, getColor());
	
	return;
}

void OdGiEntityGeometry::circularArc(const OdGePoint3d& start,
    const OdGePoint3d& point,
    const OdGePoint3d& end,
    OdGiArcType arcType)
{
	long retVal;
	CEllipArc tmpArc	(C3Point(start.x, start.y, start.z), 
						C3Point(point.x, point.y, point.z), 
						C3Point(end.x, end.y, end.z), 0.0, 1.0, retVal);

	CPointArray ptArray(DIVIDESEGMENTS + 1, retVal);

	tmpArc.GetPointsWithEqualArcLength(DIVIDESEGMENTS, ptArray);
	
	ptArray.Insert(C3Point(start.x, start.y, start.z), 0);
	ptArray.Add(C3Point(end.x, end.y, end.z));

	FillArray(m_pHanditem, ptArray, &m_pDisplayVectors, getColor());
	
	return;
}

void OdGiEntityGeometry::polyline(OdInt32 nbPoints,
    const OdGePoint3d* pVertexList,
    const OdGeVector3d* pNormal,
    OdInt32 lBaseSubEntMarker)
{
	struct gr_displayobject* tmp = &m_pDisplayVectors;
	while(tmp->next != NULL)
	{
		tmp = tmp->next;
	}
	tmp->next = new gr_displayobject();
	memset(tmp->next, 0, sizeof(struct gr_displayobject));

	ASSERT(tmp->next != NULL);
	if (tmp->next != NULL)
	{
		tmp = tmp->next;

		int color;
		m_pHanditem->get_62(&color);
		if (color == 256)
		{
			color = ((db_layertabrec*)m_pHanditem->ret_layerhip())->ret_color();
		}

		tmp->type = DISP_OBJ_PTS_3D;	// 3D (in UCS)
		tmp->color = getColor();
		tmp->lweight = -3;
		tmp->npts = nbPoints;
		tmp->SetSourceEntity((db_entity*)m_pHanditem);
		tmp->chain = new double[nbPoints * 3];

		int index = 0;
		for (int i = 0; i < nbPoints; i++)
		{
			OdGePoint3d pt = pVertexList[i];
			tmp->chain[i + index++] = pt.x;
			tmp->chain[i + index++] = pt.y;
			tmp->chain[i + index  ] = pt.z;
		}
	}

	return;
}

void OdGiEntityGeometry::polygon(OdInt32 nbPoints, const OdGePoint3d* pVertexList)
{
	struct gr_displayobject* tmp = &m_pDisplayVectors;
	while(tmp->next != NULL)
	{
		tmp = tmp->next;
	}
	tmp->next = new gr_displayobject();
	memset(tmp->next, 0, sizeof(struct gr_displayobject));

	ASSERT(tmp->next != NULL);
	if (tmp->next != NULL)
	{
		tmp = tmp->next;

		tmp->type = DISP_OBJ_PTS_3D;	// 3D (in UCS)
		tmp->color = getColor();
		tmp->lweight = -3;
		tmp->npts = nbPoints;
		tmp->SetSourceEntity((db_entity*)m_pHanditem);
		tmp->chain = new double[nbPoints * 3];

		int index = 0;
		for (int i = 0; i < nbPoints; i++)
		{
			OdGePoint3d pt = pVertexList[i];
			tmp->chain[i + index++] = pt.x;
			tmp->chain[i + index++] = pt.y;
			tmp->chain[i + index  ] = pt.z;
		}
	}
	return;
}

void OdGiEntityGeometry::pline(const OdGiPolyline& lwBuf, OdUInt32 fromIndex, OdUInt32 numSegs)
{
#ifdef _DEBUG_MHB
	OutputDebugString("OdGiEntityGeometry::pline() - Not implemented\n");
#endif
	return;
}

void OdGiEntityGeometry::mesh(OdInt32 rows,
    OdInt32 columns,
    const OdGePoint3d* pVertexList,
    const OdGiEdgeData* pEdgeData,
    const OdGiFaceData* pFaceData,
    const OdGiVertexData* pVertexData)
{
#ifdef _DEBUG_MHB
	OutputDebugString("OdGiEntityGeometry::mes() - needs traits handling.\n");
#endif
	struct gr_displayobject* tmp = &m_pDisplayVectors;
	while (tmp->next != NULL)
	{
		tmp = tmp->next;
	}
	tmp->next = new gr_displayobject();
	memset(tmp->next, 0, sizeof(struct gr_displayobject));

	ASSERT(tmp->next != NULL);
	if (tmp->next != NULL)
	{
		tmp = tmp->next;

		tmp->type = DISP_OBJ_PTS_3D + DISP_OBJ_PTS_CLOSED;	// 3D (in UCS) & closed.
		tmp->color = getColor(); 
		tmp->lweight = -3;
		tmp->npts = rows + columns;
		tmp->SetSourceEntity((db_entity*)m_pHanditem);
		tmp->chain = new double[tmp->npts * 3];
	}
	int i, j, k;
	int index = 0;
	for (k = 0, i = 0; i < rows; i++)
	{
		for (j = 0; j < columns; j++, k++)
		{
			OdGePoint3d pt = pVertexList[k];	
			tmp->chain[k + index++] = pt.x;
			tmp->chain[k + index++] = pt.y;
			tmp->chain[k + index  ] = pt.z;
		}
	}
	return;
}

void OdGiEntityGeometry::shell(OdInt32 nbVertex,
    const OdGePoint3d* pVertexList,
    OdInt32 faceListSize,
    const OdInt32* pFaceList,
    const OdGiEdgeData* pEdgeData,
    const OdGiFaceData* pFaceData,
    const OdGiVertexData* pVertexData)
{
	struct gr_displayobject* tmp = &m_pDisplayVectors;
	while(tmp->next != NULL)
	{
		tmp = tmp->next;
	}
	tmp->next = new gr_displayobject();
	memset(tmp->next, 0, sizeof(struct gr_displayobject));

	ASSERT(tmp->next != NULL);
	if (tmp->next != NULL)
	{
		tmp = tmp->next;

		tmp->type = DISP_OBJ_PTS_3D + DISP_OBJ_PTS_CLOSED;	// 3D (in UCS) & closed.
		tmp->color = getColor(); 
		tmp->lweight = -3;
		tmp->npts = nbVertex;
		tmp->SetSourceEntity((db_entity*)m_pHanditem);
		tmp->chain = new double[nbVertex * 3];

		int index = 0;
		for (int i = 0; i < nbVertex; i++)
		{
			OdGePoint3d pt = pVertexList[i];
			tmp->chain[i + index++] = pt.x;
			tmp->chain[i + index++] = pt.y;
			tmp->chain[i + index  ] = pt.z;
		}
	}

	return;
}

void OdGiEntityGeometry::text(const OdGePoint3d& position,
    const OdGeVector3d& normal, const OdGeVector3d& direction,
    double height, double width, double oblique, const OdChar* msg)
{
#ifdef _DEBUG_MHB
	OutputDebugString("OdGiEntityGeometry::tex() - Not implemented\n");
#endif
	return;
}

void OdGiEntityGeometry::text(const OdGePoint3d& position,
    const OdGeVector3d& normal, const OdGeVector3d& direction,
    const OdChar* msg, OdInt32 length, bool raw, const OdGiTextStyle* pTextStyle)
{
#ifdef _DEBUG_MHB
	OutputDebugString("OdGiEntityGeometry::tex() - Not implemented\n");
#endif
	return;
}

void OdGiEntityGeometry::xline(const OdGePoint3d& pt1, const OdGePoint3d& pt2)
{
	struct gr_displayobject* tmp = &m_pDisplayVectors;
	while (tmp->next != NULL)
	{
		tmp = tmp->next;
	}
	tmp->next = new gr_displayobject();
	memset(tmp->next, 0, sizeof(struct gr_displayobject));

	ASSERT(tmp->next != NULL);
	if (tmp->next != NULL)
	{
		tmp = tmp->next;

		tmp->type = DISP_OBJ_PTS_3D;	
		tmp->color = getColor(); 
		tmp->lweight = -3;
		tmp->npts = 2;
		tmp->SetSourceEntity((db_entity*)m_pHanditem);
		tmp->chain = new double[tmp->npts * 3];

		tmp->chain[0] = pt1.x;
		tmp->chain[1] = pt1.y;
		tmp->chain[2] = pt1.z;

		tmp->chain[3] = pt2.x;
		tmp->chain[4] = pt2.y;
		tmp->chain[5] = pt2.z;
	}
	return;
}

void OdGiEntityGeometry::ray(const OdGePoint3d& pt1, const OdGePoint3d& pt2)
{
	struct gr_displayobject* tmp = &m_pDisplayVectors;
	while (tmp->next != NULL)
	{
		tmp = tmp->next;
	}
	tmp->next = new gr_displayobject();
	memset(tmp->next, 0, sizeof(struct gr_displayobject));

	ASSERT(tmp->next != NULL);
	if (tmp->next != NULL)
	{
		tmp = tmp->next;

		tmp->type = DISP_OBJ_PTS_3D;	
		tmp->color = getColor(); 
		tmp->lweight = -3;
		tmp->npts = 2;
		tmp->SetSourceEntity((db_entity*)m_pHanditem);
		tmp->chain = new double[tmp->npts * 3];

		tmp->chain[0] = pt1.x;
		tmp->chain[1] = pt1.y;
		tmp->chain[2] = pt1.z;

		tmp->chain[3] = pt2.x;
		tmp->chain[4] = pt2.y;
		tmp->chain[5] = pt2.z;
	}
	return;
}

void OdGiEntityGeometry::pline(const OdDbPolyline& lwBuf, OdUInt32 fromIndex, OdUInt32 numSegs)
{
#ifdef _DEBUG_MHB
	OutputDebugString("OdGiEntityGeometry::pline() - Not implemented\n");
#endif
	return;
}

void OdGiEntityGeometry::nurbs(const OdGeNurbCurve3d& nurbs)
{
#ifdef _DEBUG_MHB
	OutputDebugString("OdGiEntityGeometry::nurbs() - Not implemented\n");
#endif
	return;
}

void OdGiEntityGeometry::pushClipBoundary(OdGiClipBoundary* pBoundary)
{
#ifdef _DEBUG_MHB
	OutputDebugString("OdGiEntityGeometry::pushClipBoundary() - Not implemented\n");
#endif
	return;
}

void OdGiEntityGeometry::popClipBoundary()
{
#ifdef _DEBUG_MHB
	OutputDebugString("OdGiEntityGeometry::popClipBoundary() - Not implemented\n");
#endif
 	return;
}

int OdGiEntityGeometry::getColor() 
{ 
	int color;
	int traitColor = m_pTraits->color();
	if (traitColor == (OdUInt16)-1 || traitColor == 256)
	{
		m_pHanditem->get_62(&color);
		if (color == 256)
		{
			color = ((db_layertabrec*)m_pHanditem->ret_layerhip())->ret_color();
			if (color != traitColor)
				m_pTraits->setColor((OdUInt16)color);
		}
		else
		{
		if (color != traitColor)
			m_pTraits->setColor((OdUInt16)color);
		}
	}
	else
	{
		color = traitColor;
	}

	return color;
}

//----------------------------------------------------------
//
// OdGiEntityTraits
//
//----------------------------------------------------------
void OdGiEntityTraits::setColor(OdUInt16 color)
{
#ifdef _DEBUG_MHB
	char tmp[100];
	sprintf(tmp, "OdGiEntityTraits::setColor() - Color == %u\n", color);
	OutputDebugString(tmp);
#endif
	m_Color = color;
	return;
}

void OdGiEntityTraits::setTrueColor(const OdCmEntityColor& cmColor)
{
#ifdef _DEBUG_MHB
	char tmp[100];
	sprintf(tmp, "OdGiEntityTraits::setTrueColor() - Color == %u\n", cmColor.colorIndex());
	OutputDebugString(tmp);
#endif
	m_Color = cmColor.colorIndex();
	return;
}

void OdGiEntityTraits::setLayer(OdDbStub* layerId)
{
	m_pLayerStub = layerId;

#ifdef _DEBUG_MHB
	char tmp[100];
	sprintf(tmp, "OdGiEntityTraits::setLayer() - Name == %s\n", ((OdDbLayerTableRecordPtr)((OdDbObjectId)((OdDbStub*)layerId)).safeOpenObject())->getName());
	OutputDebugString(tmp);
#endif
	return;
}

void OdGiEntityTraits::setLineType(OdDbStub* linetypeId)
{
	m_pLinetypeStub = linetypeId;

#ifdef _DEBUG_MHB
	char tmp[100];
	sprintf(tmp, "OdGiEntityTraits::setLineType() - Name == %s\n", ((OdDbLayerTableRecordPtr)((OdDbObjectId)((OdDbStub*)linetypeId)).safeOpenObject())->getName());
	OutputDebugString(tmp);
#endif

	return;
}

void OdGiEntityTraits::setSelectionMarker(OdInt32 markerId)
{
#ifdef _DEBUG_MHB
	 OutputDebugString("OdGiEntityTraits::setSelectionMarker() - Not implemented\n");
#endif
	return;
}

void OdGiEntityTraits::setFillType(OdGiFillType fillType)
{
#ifdef _DEBUG_MHB
	char tmp[100];
	sprintf(tmp, "OdGiEntityTraits::setFillType() - Type == %d\n", fillType);
	OutputDebugString(tmp);
#endif
	m_fillType = fillType;

	return;
}

void OdGiEntityTraits::setLineWeight(OdDb::LineWeight lw)
{
#ifdef _DEBUG_MHB
	char tmp[100];
	sprintf(tmp, "OdGiEntityTraits::setLineWeight() - Weight == %d\n", lw);
	OutputDebugString(tmp);
#endif
	m_lineWeight = lw;
	return;
}

void OdGiEntityTraits::setLineTypeScale(double dScale)
{
#ifdef _DEBUG_MHB
	char tmp[100];
	sprintf(tmp, "OdGiEntityTraits::setLineTypeScale() - Scale == %d\n", dScale);
	OutputDebugString(tmp);
#endif
	m_linetypeScale = dScale;
	return;
}

void OdGiEntityTraits::setThickness(double dThickness)
{
#ifdef _DEBUG_MHB
	char tmp[100];
	sprintf(tmp, "OdGiEntityTraits::thickness() - Thickness == %d\n", dThickness);
	OutputDebugString(tmp);
#endif
	m_thickness = dThickness;
	return;
}

void OdGiEntityTraits::setPlotStyleName(OdDb::PlotStyleNameType nameType, OdDbStub* style)
{
	m_pPlotStyle = style;
	m_plotStyleNameType = nameType;
	return;
}

void OdGiEntityTraits::setMaterial(OdDbStub* materialId)
{
	m_pMaterial = materialId;
	return;
}

OdUInt16 OdGiEntityTraits::color() const
{
#ifdef _DEBUG_MHB
	char tmp[100];
	sprintf(tmp, "OdGiEntityTraits::color() - Color == %u\n", m_Color);
	OutputDebugString(tmp);
#endif
	return m_Color;
}

OdDbStub* OdGiEntityTraits::layer() const
{
#ifdef _DEBUG_MHB
	char tmp[100];
	sprintf(tmp, "OdGiEntityTraits::layer() - Name == %s\n", ((OdDbLayerTableRecordPtr)((OdDbObjectId)((OdDbStub*)m_pLayerStub)).safeOpenObject())->getName());
	OutputDebugString(tmp);
#endif
	return m_pLayerStub;
}

OdDbStub* OdGiEntityTraits::lineType() const
{
#ifdef _DEBUG_MHB
	char tmp[100];
	sprintf(tmp, "OdGiEntityTraits::lineType() - Name == %s\n", ((OdDbLayerTableRecordPtr)((OdDbObjectId)((OdDbStub*)m_pLinetypeStub)).safeOpenObject())->getName());
	OutputDebugString(tmp);
#endif
	return m_pLinetypeStub;
}

OdGiFillType OdGiEntityTraits::fillType() const
{
#ifdef _DEBUG_MHB
	char tmp[100];
	sprintf(tmp, "OdGiEntityTraits::fillType() - Type == %d\n", m_fillType);
	OutputDebugString(tmp);
#endif
	return m_fillType;
}

OdDb::LineWeight OdGiEntityTraits::lineWeight() const
{
#ifdef _DEBUG_MHB
	char tmp[100];
	sprintf(tmp, "OdGiEntityTraits::lineWeight() - Weight == %d\n", m_lineWeight);
	OutputDebugString(tmp);
#endif
	return m_lineWeight;
}

double OdGiEntityTraits::lineTypeScale() const
{
#ifdef _DEBUG_MHB
	char tmp[100];
	sprintf(tmp, "OdGiEntityTraits::lineTypeScale() - Scale == %d\n", m_linetypeScale);
	OutputDebugString(tmp);
#endif
	return m_linetypeScale;
}

double OdGiEntityTraits::thickness() const
{
#ifdef _DEBUG_MHB
	char tmp[100];
	sprintf(tmp, "OdGiEntityTraits::thickness() - Thickness == %d\n", m_thickness);
	OutputDebugString(tmp);
#endif
	return m_thickness;
}

OdDb::PlotStyleNameType OdGiEntityTraits::plotStyleNameType() const 
{
	return m_plotStyleNameType;
}

OdDbStub* OdGiEntityTraits::plotStyleNameId() const
{
	return m_pPlotStyle;
}

OdDbStub* OdGiEntityTraits::material() const
{
	return m_pMaterial;
}

//----------------------------------------------------------
//
// OdGiEntityDraw
//
//----------------------------------------------------------
OdGiEntityDraw::OdGiEntityDraw(db_handitem* pHi)
  : m_WdGeom()
  , m_Traits()
{
	m_WdGeom.setHanditem(pHi);
	m_WdGeom.setTraits(&m_Traits);
}

OdGiWorldGeometry& OdGiEntityDraw::geometry() const
{
	return (OdGiWorldGeometry&)m_WdGeom;
}

OdGiRegenType OdGiEntityDraw::regenType() const
{
  return kOdGiForExplode;
}

bool OdGiEntityDraw::regenAbort() const
{
  return false;
}

OdGiSubEntityTraits& OdGiEntityDraw::subEntityTraits() const
{
  return (OdGiSubEntityTraits&)m_Traits;
}

OdGiGeometry& OdGiEntityDraw::rawGeometry() const
{
  return (OdGiGeometry&)m_WdGeom;
}

bool OdGiEntityDraw::isDragging() const
{
  return false;
}
  
double OdGiEntityDraw::deviation(const OdGiDeviationType, const OdGePoint3d&) const
{
  return 0;
}

double OdGiEntityDraw::modelDeviation(const OdGiDeviationType type, const OdGePoint3d& modelPoint) const
{
  return 0.0;
}

OdUInt32 OdGiEntityDraw::numberOfIsolines() const
{
  return 1;
}
  
void OdGiEntityDraw::setContext(OdGiContext* pUserContext)
{
  pCtx = pUserContext;
}

OdGiContext* OdGiEntityDraw::context() const
{
  return pCtx;
}


///////////////////////////////////////////////////////////////////////////////////
void FillArray(db_handitem* pEnt, CPointArray& ptArray, struct gr_displayobject* pDisplayVectors, int color)
{
	struct gr_displayobject* tmp = pDisplayVectors;
	while(tmp->next != NULL)
	{
		tmp = tmp->next;
	}
	tmp->next = new gr_displayobject();
	memset(tmp->next, 0, sizeof(struct gr_displayobject));

	ASSERT(tmp->next != NULL);
	if (tmp->next != NULL)
	{
		int ptCount = ptArray.Size();
		tmp->type = DISP_OBJ_PTS_3D;		// 3D (in UCS)
		tmp->color = color;
		tmp->lweight = -3;
		tmp->npts = ptCount;
		tmp->SetSourceEntity((db_entity*)pEnt);
		tmp->chain = new double[ptCount * 3];

		C3Point point;
		int index = 0;
		double whichParam = 0.0;
		for (int i = 0; i < ptCount; i++)
		{
			tmp->chain[i + index++] = ptArray[i].X();
			tmp->chain[i + index++] = ptArray[i].Y();
			tmp->chain[i + index  ] = ptArray[i].Z();
		}

	}
	
	return;
}

