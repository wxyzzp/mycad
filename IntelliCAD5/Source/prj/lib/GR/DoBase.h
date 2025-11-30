#ifndef __DoBaseH__
#define __DoBaseH__

class CDC;
class db_entity;
class db_drawing;
struct gr_view;
struct gr_displayobject;
struct gr_pclink;

class GR_CLASS CDoBase
{
public:
	CDoBase();

	void setDC(CDC* pDC) { m_pDC = pDC; }
	void setDrawing(db_drawing* pDrawing) { m_pDrawing = pDrawing; }
	void setViewData(gr_view* pView) { m_pViewData = pView; }
	void setEntity(db_entity* pEntity) { m_pEntity = pEntity; }
	void setPointsData(gr_pclink* pInitialPoint, gr_pclink* pLastPoint, gr_pclink* pInternalPoint, long nInternalPoints);
	void setFlags(short closed, short capped);
	void setDotDir(sds_point dotDir);

	void setPointsData(gr_displayobject* pListBegin, gr_displayobject* pListEnd = NULL);

	virtual int getDisplayObjects(gr_displayobject*& pListBegin, gr_displayobject*& pListEnd);
	virtual int getBoundingBox(CD3& minPoint, CD3& maxPoint);

	static int getBoundingBox(gr_displayobject* pListBegin, CD3& minPoint, CD3& maxPoint);
	static int getPlanarBoundingBox(gr_view* pViewData, gr_displayobject* pListBegin, CD3& minPoint, CD3& maxPoint);
	static int getPlanarBoundingBox(gr_view* pViewData, db_entity* pEntity, CD3& minPoint, CD3& maxPoint);

	static CDoBase& get() { return s_theDoBase; }

protected:
	/********************************************************************************
	* Author:	Dmitry Gavrilov.
	* Purpose:	An extension of gr_gensolid. Clip if necessary given polyline/polygon
	*			set by points array and create appropriate disp objs.
	* Returns:	gr_gensolid return codes.
	********************************************************************************/
	int clipAndCreateDO(CPolyLine& points, short closed, short fillable, short dimension, 
		gr_displayobject** ppBegNewDO, gr_displayobject**	ppEndNewDO);

	int formDOs();
protected:
	db_entity* m_pEntity;
	gr_view* m_pViewData;
	db_drawing* m_pDrawing;
	CDC* m_pDC;
	
	gr_pclink* m_pInitialPoint;
	gr_pclink* m_pLastPoint;
	gr_pclink* m_pInternalPoint;
	long m_nInternalPoints;

	short m_closed;
	short m_capped;
	CD3 m_dotDir;
	
	gr_displayobject* m_pListBegin;
	gr_displayobject* m_pListEnd;

	static RC s_geoRC;
	static CPolyLine s_pointsArray2;
	static CPolyLine s_pointsArray4;
	static CPolyLine s_pointsArray10;
	static CPolyLine s_pointsArray;
private:
	static CDoBase s_theDoBase;
};

#endif
