#ifndef __icadgridiconsnapH__
#define __icadgridiconsnapH__

int SDS_DrawGrid(CIcadView* pView,struct gr_view* view);
int SDS_SnapPt(const sds_point startpt, sds_point snappt);
int SDS_DrawBlip(const sds_point pt);

class db_drawing;
class CIcadView;
class CDrawDevice;

class CGrid
{
public:
	CGrid(db_drawing* pDrawing);
	int draw(struct gr_view* pViewData, CDrawDevice* pDrawDevice, CIcadView* pView);
protected:
	db_drawing* m_pDrawing;
};

#endif
