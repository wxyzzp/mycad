#ifndef __DbMTextH__
#define __DbMTextH__
/*
	DXF 73, 44 and EMTextLineFeedType is available since Release 14
*/
#include "entitywithextrusion.h"
class db_drawing;

class DB_CLASS CDbMText : public db_entity_with_extrusion
{
    friend int db_CompEntLinks(struct db_BinChange** chglst, db_handitem* oldhip, db_handitem* newhip);
public:
	enum EMTextAttachment
	{
		eTOP_LEFT = 1,
		eTOP_CENTER = 2,
		eTOP_RIGHT = 3,
		eMIDDLE_LEFT = 4,
		eMIDDLE_CENTER = 5,
		eMIDDLE_RIGHT = 6,
		eBOTTOM_LEFT = 7,
		eBOTTOM_CENTER = 8,
		eBOTTOM_RIGHT = 9
	};
	enum EMTextDirection
	{
		eLEFT_RIGHT = 1,
		eTOP_BOTTOM = 3,
		// the flow direction is inherited from the associated text style
		eBYSTYLE = 5
	};
	enum EMTextLineFeedType
	{
		eVARIABLE = 1,
		eFIXED = 2
	};
public:
	CDbMText();
	CDbMText(db_drawing* pDrawing);
	CDbMText(const CDbMText& entity);
	virtual ~CDbMText();

	virtual void* ret_disp() { return m_pDOs; }
	virtual void set_disp(void* p) { m_pDOs = p; }
	virtual void get_extent(sds_point ll, sds_point ur);
	virtual void set_extent(sds_point ll, sds_point ur);

	virtual sds_real* retp_10(void) { return m_insertionPoint; }
	virtual sds_real* retp_11(void) { return m_ox; }

	virtual bool get_1(char* p, int maxlen);
	// DP: FOR COMPATIBILITY ONLY
	virtual bool get_1(char** p) { *p = (char*)m_text.c_str(); return true; }
    virtual bool get_7(char* p, int maxlen);
    virtual bool get_7(char** p);
    virtual bool get_7(db_handitem** p) { *p = m_pTextStyle; return true; }

    virtual bool get_10(sds_point p) { memcpy(p, m_insertionPoint, sizeof(sds_point)); return true; }
    virtual bool get_11(sds_point p) { memcpy(p, m_ox, sizeof(sds_point)); return true; }

    virtual bool get_40(sds_real* p) { *p = m_initialHeight; return true; }
    virtual bool get_41(sds_real* p) { *p = m_initialWidth; return true; }
    // EBATECH(CNBR) -[ Use m_extents member 2002/6/10 get actual width/height from m_extents
	//virtual bool get_42(sds_real* p) { *p = m_actualWidth; return true; }
	//virtual bool get_43(sds_real* p) { *p = m_actualHeight; return true; }
	virtual bool get_42(sds_real* p) { *p = fabs(m_extents[1][0]-m_extents[0][0]); return true; }
	virtual bool get_43(sds_real* p) { *p = fabs(m_extents[1][1]-m_extents[0][1]); return true; }
	// EBATECH(CNBR) ]-
	virtual bool get_44(sds_real* p) { *p = m_LineFeedRatio; return true; }
    virtual bool get_50(sds_real *p, db_drawing* pDrawing);

    virtual bool get_71(int *p) { *p = (int)m_attachTo; return true; }
    virtual bool get_72(int *p) { *p = (int)m_direction; return true; }
    virtual bool get_73(int *p) { *p = (int)m_LineFeedType; return true; }

    virtual bool set_1(char* p) { m_text = (p == NULL) ? db_str_quotequote : p; return true; }
    virtual bool set_7(db_handitem* p) { m_pTextStyle = p; return true; }
	virtual bool set_7(char* p, db_drawing *pDrawing);

    virtual bool set_10(sds_point p) { memcpy(m_insertionPoint, p, sizeof(sds_point)); return true; }
    virtual bool set_11(sds_point p);

    virtual bool set_40(sds_real p) { m_initialHeight = (icadRealEqual(p,0.0)) ? atof(db_oldsysvar[DB_QTEXTSIZE].defval) : fabs(p); return true; }
    virtual bool set_41(sds_real p) { m_initialWidth = (p >= 0.0) ? p : 0.0; return true; }
    // EBATECH(CNBR)-[ 2002/6/10 DXF=42 and 43 is read only.
	//virtual bool set_42(sds_real p) { m_actualWidth = p; return true; }
	//virtual bool set_43(sds_real p) { m_actualHeight = p; return true; }
	// EBATECH(CNBR) ]-
	virtual bool set_44(sds_real p) { m_LineFeedRatio = p; return true; }

    virtual bool set_50(sds_real p, db_drawing *pDrawing);
    virtual bool set_71(int p) { m_attachTo = (p > 0 && p < 10) ? (EMTextAttachment)p : eTOP_LEFT; return true; }
    virtual bool set_72(int p) { m_direction = (p > 0 && p < 6) ? (EMTextDirection)p : eLEFT_RIGHT; return true; }
    virtual bool set_73(int p) { m_LineFeedType = (p > 0 && p < 3) ? (EMTextLineFeedType)p : eVARIABLE; return true; }

    virtual int entgetspecific(struct resbuf** begpp, struct resbuf** endpp, db_drawing* pDrawing);
    virtual int entmod(struct resbuf* modll, db_drawing* pDrawing);

private:
	// 1, 3
	CDbString m_text;
    // 7
	db_handitem* m_pTextStyle;
	// 10
	sds_point m_insertionPoint;
	// 11
	sds_point m_ox;
	// 40
	sds_real m_initialHeight;
	// 41
	sds_real m_initialWidth;
	// 42 EBATECH(CNBR) 2002/6/10 get actual width/height from m_extents
	//sds_real m_actualWidth;
	// 43 EBATECH(CNBR) 2002/6/10 get actual width/height from m_extents
	//sds_real m_actualHeight;
	// 71
	EMTextAttachment m_attachTo;
	// 72
	EMTextDirection m_direction;
	// 44
	sds_real m_LineFeedRatio;
	// 73
	EMTextLineFeedType m_LineFeedType;

	// The extents of the m_pDOs llist; see db_3dface.
	sds_point m_extents[2];
	// Disp obj llist (gr_displayobject *)
	void* m_pDOs;

    int mtextrot(sds_point axisOX, sds_point mtextru, sds_real* mtrotp, int xdir2rot, db_drawing *pDrawing);
};

typedef CDbMText db_mtext;

#endif
