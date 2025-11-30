#ifndef __DbLayoutH__
#define __DbLayoutH__

#include "DbPlotSettings.h"
/*--------------------------------------------------------------------------------*//**
@author Denis Petrov

@description Class for representing LAYOUT object.
*//*----------------------------------------------------------------------------------*/
class DB_CLASS CDbLayout: public CDbPlotSettings
{
	friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);
	friend class drw_readwrite;
public:
	enum EViewType
	{
		eNotOrtho = 0,
		eTop = 1,
		eBottom = 2,
		eFront = 3,
		eBack = 4,
		eLeft = 5,
		eRight = 6
	};
	enum EFlags
	{
		ePSLTSCALE = 0x01,
		eLIMCHECK = 0x02,
	};

    CDbLayout();
    CDbLayout(const CDbLayout &l);
	virtual ~CDbLayout();

	virtual int entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp);
	virtual int entmod(struct resbuf *modll, db_drawing *dp);

    int   ret_deleted()  { return m_deleted; }
    void  set_deleted(int p) { m_deleted = (char)(p !=0); }

	const char* name(db_drawing* pDrawing)/*const*/;
	void setName(char* pName, db_drawing* pDrawing);

	db_handitem* block() { return m_pBlock; }
	void* blockAddress() { return &m_pBlock; }
	void setBlock(db_handitem* pBlock) { m_pBlock = pBlock; }

	db_handitem* lastViewport(void) { return m_pLastViewport; }
	void* viewportAddress() { return &m_pLastViewport; }
	void setLastViewport(db_handitem *pViewport) { m_pLastViewport = pViewport; }

	void setCurrent() { m_pBlock = (db_handitem*)&m_pBlock; }
	bool isCurrent() const { return m_pBlock == (db_handitem*)&m_pBlock; }
	
	bool isPSLTSCALE() const { return m_layoutFlag & ePSLTSCALE ? true : false; }
	bool isLIMCHECK() const { return m_layoutFlag & eLIMCHECK ? true : false; }
	int tabOrder() const { return m_tabOrder; }
	CDbLayout::EViewType ucsViewType() const { return m_ucsViewType; }

    virtual bool get_10(sds_point p) { memcpy(p, m_limMin, 2 * sizeof(double)); return false; }
    virtual bool get_11(sds_point p) { memcpy(p, m_limMax, 2 * sizeof(double)); return false; }
    virtual bool get_12(sds_point p) { memcpy(p, m_insPoint, sizeof(sds_point)); return false; }
    virtual bool get_13(sds_point p) { memcpy(p, m_ucsO, sizeof(sds_point)); return false; }
    virtual bool get_14(sds_point p) { memcpy(p, m_extMin, sizeof(sds_point)); return false; }
    virtual bool get_15(sds_point p) { memcpy(p, m_extMax, sizeof(sds_point)); return false; }
    virtual bool get_16(sds_point p) { memcpy(p, m_ucsX, sizeof(sds_point)); return false; }
    virtual bool get_17(sds_point p) { memcpy(p, m_ucsY, sizeof(sds_point)); return false; }

	// parent class has that group too - MHB but it belongs to the plotsettings.
	// This flag belongs to layout as opposed to the plotsettings.
    virtual bool get_Layout70(int *p) { *p = m_layoutFlag; return true; }
    virtual bool get_71(int *p) { *p = m_tabOrder; return true; }
    virtual bool get_76(int *p) { *p = m_ucsViewType; return true; }
    virtual bool get_146(sds_real *p) { *p = m_elevation; return true; }

    virtual bool set_10(sds_point p) { memcpy(m_limMin, p, 2 * sizeof(double)); return true; }
    virtual bool set_11(sds_point p) { memcpy(m_limMax, p, 2 * sizeof(double)); return true; }
    virtual bool set_12(sds_point p) { memcpy(m_insPoint, p, sizeof(sds_point)); return true; }
    virtual bool set_13(sds_point p) { memcpy(m_ucsO, p, sizeof(sds_point)); return true; }
    virtual bool set_14(sds_point p) { memcpy(m_extMin, p, sizeof(sds_point)); return true; }
    virtual bool set_15(sds_point p) { memcpy(m_extMax, p, sizeof(sds_point)); return true; }
    virtual bool set_16(sds_point p) { memcpy(m_ucsX, p, sizeof(sds_point)); return true; }
    virtual bool set_17(sds_point p) { memcpy(m_ucsY, p, sizeof(sds_point)); return true; }

	// parent class has that group too - MHB but it belongs to the plotsettings.
	// This flag belongs to layout as opposed to the plotsettings.
    virtual bool set_Layout70(int p) { m_layoutFlag = p; return true; }
    virtual bool set_71(int p) { m_tabOrder = p; return true; }
    virtual bool set_76(int p) { m_ucsViewType = (EViewType)p; return true; }
    virtual bool set_146(sds_real p) { m_elevation = p; return true; }
private:
    char m_deleted;

	// 1
	char m_name[IC_ACADBUF];
	// 70
	short m_layoutFlag;
	// 71
	short m_tabOrder;
	// 10, 20
	double m_limMin[2];
	// 11, 21
	double m_limMax[2];
	// 12, 22, 32
	double m_insPoint[3];
	// 14, 24, 34
	double m_extMin[3];
	// 15, 25, 35
	double m_extMax[3];
	// 146
	double m_elevation;
	// 13, 23, 33
	double m_ucsO[3];
	// 16, 26, 36
	double m_ucsX[3];
	// 17, 27, 37
	double m_ucsY[3];
	// 76
	EViewType m_ucsViewType;
	// 330
	db_handitem* m_pBlock;
	// 331
	db_handitem* m_pLastViewport;
	// 345
	db_handitem* m_pUCS;
	// 346
	db_handitem* m_pBaseUCS;
};

#endif
