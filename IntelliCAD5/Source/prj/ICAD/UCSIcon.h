#ifndef __UCSIconH__
#define __UCSIconH__

int SDS_DrawUcsIcon(sds_point vdir, const sds_real *sclfact,CIcadView *pView,int unpaint);

class db_drawing;
class CIcadView;
class CDrawDevice;

class CUCSIcon
{
public:
	CUCSIcon(db_drawing* pDrawing);
	int draw(sds_point vdir, const sds_real* sclfact, int unpaint, CDrawDevice* pDrawDevice, CIcadView* pView);

protected:
	db_drawing* m_pDrawing;
	CIcadView* m_pView;

	int drawAlternate(const sds_real* scale, const sds_point rpadd, const sds_point viewdir, int unpaint, CDrawDevice* pDrawDevice);
	int drawElement(sds_point frompt, sds_point topt, int color, sds_point basept, sds_point tempxdir, sds_point tempydir, CDrawDevice* pDrawDevice);
};

#endif
