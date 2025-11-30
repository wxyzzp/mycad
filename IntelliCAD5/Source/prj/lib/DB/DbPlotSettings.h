#ifndef __DbPlotSettingsH__
#define __DbPlotSettingsH__

#include "handitem.h"
typedef db_handitem CDbObject;
/*--------------------------------------------------------------------------------*//**
@author Denis Petrov

@description Class for representing PLOTSETTINGS object.
*//*----------------------------------------------------------------------------------*/
class DB_CLASS CDbPlotSettings: public CDbObject 
{
    friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

public:
	enum EPlotLayoutFlags
	{
		eClear = 0,
		ePlotViewportBorders = 1, 
		eShowPlotStyles = 2,  
		ePlotCentered = 4,  
		ePlotHidden = 8,  
		eUseStandardScale = 16,  
		ePlotPlotStyles = 32,  
		eScaleLineweights = 64,  
		ePrintLineweights = 128, 
		eDrawViewportsFirst = 512,  
		eModelType = 1024,  
		eUpdatePaper = 2048, 
		eZoomToPaperOnUpdate = 4096,  
		eInitializing = 8192,  
		ePrevPlotInit = 16384, 
	};
	enum EPlotPaperUnits
	{
		eInches = 0,
		eMillimeters = 1,
		ePixels = 2,
	};
	enum EPlotRotation
	{
		e0 = 0,
		e90 = 1,
		e180 = 2,
		e270 = 3,
	};
	enum EPlotType
	{
		eDisplay = 0, // Last screen display
		eExtents = 1, // Drawing extents
		eLimits = 2, //	Drawing limits
		eView = 3, // Last view
		eWindow	= 4, // Last window
		eLayout	= 5, // Layout information
	};
	enum EPlotScaleType
	{
		eToFit = 0, // Scaled to Fit
		e1_128i = 1, // 1/128"=1'
		e1_64i = 2,  // 1/64"=1'
		e1_32i = 3,  // 1/32"=1'
		e1_16i = 4,  // 1/16"=1'
		e3_32i = 5,  // 3/32"=1'
		e1_8i = 6,   // 1/8"=1'
		e3_16i = 7,  // 3/16"=1'
		e1_4i = 8,   // 1/4"=1'
		e3_8i = 9,   // 3/8"=1'
		e1_2i = 10,  // 1/2"=1'
		e3_4i = 11,  // 3/4"=1'
		e1_1i = 12,  // 1"=1'
		e3_1i = 13,  // 3"=1'
		e6_1i = 14,  // 6"=1'
		e1i_1i = 15, // 1'=1'
		e1_1 = 16,   // 1:1
		e1_2 = 17,   // 1:2
		e1_4 = 18,   // 1:4
		e1_8 = 19,   // 1:8
		e1_10 = 20,  // 1:10
		e1_16 = 21,  // 1:16
        e1_20 = 22,  // 1:20
		e1_30 = 23,  // 1:30
		e1_40 = 24,  // 1:40
		e1_50 = 25,  // 1:50
		e1_100 = 26, // 1:100
		e2_1 = 27,   // 2:1
		e4_1 = 28,   // 4:1
		e8_1 = 29,   // 8:1
		e10_1 = 30,  // 10:1
		e100_1 = 31, // 100:1
		e1000_1 = 32,// 1000:1,
		eCustom = 33,
	};

	CDbPlotSettings(int type = DB_PLOTSETTINGS);
	CDbPlotSettings(const CDbPlotSettings &ps);
	virtual ~CDbPlotSettings();

	virtual int entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp);
	virtual int entmod(struct resbuf *modll, db_drawing *dp);

    int   ret_deleted(void)  { return m_deleted; }
    void  set_deleted(int p) { m_deleted= (char)(p != 0); }

	const char* name(db_drawing* pDrawing)/*const*/;
	void setName(char* pName, db_drawing* pDrawing);
	void copyFrom(const CDbPlotSettings* pSettings);

	const char* plotConfig() const { return m_plotConfig; }
	const char* paperSize() const { return m_paperSize; }
	const char* plotViewName() const { return m_plotView; }
	const char* currentStyleSheet() const { return m_curStyleSheet; }

	void getPlotPaperMargins(double& printXmin, double& printYmin, double& printXmax, double& printYmax) const;
	void getPlotPaperSize(double& width, double& height) const;
	void getPlotOrigin(double& x, double& y) const;
	void getPlotWindowArea(double& xmin, double& ymin, double& xmax, double& ymax) const;
	CDbPlotSettings::EPlotPaperUnits plotPaperUnits() const { return m_plotPaperUnits; }
	CDbPlotSettings::EPlotRotation plotRotation() const { return m_plotRotation; }
	CDbPlotSettings::EPlotType plotType() const { return m_plotType; }
	CDbPlotSettings::EPlotScaleType scaleType() const { return m_scaleType; }

    virtual bool set_2(char* p) { strncpy(m_plotConfig, (p == NULL) ? db_str_quotequote : p, IC_ACADBUF - 1); return true; }
    virtual bool set_4(char* p) { strncpy(m_paperSize, (p == NULL) ? db_str_quotequote : p, IC_ACADBUF - 1); return true; }
    virtual bool set_6(char* p) { strncpy(m_plotView, (p == NULL) ? db_str_quotequote : p, IC_ACADBUF - 1); return true; }
    virtual bool set_7(char* p) { strncpy(m_curStyleSheet, (p == NULL) ? db_str_quotequote : p, IC_ACADBUF - 1); return true; }
    virtual bool set_70(int p) { m_plotLayoutFlag = p; return true; }
    virtual bool set_72(int p) { m_plotPaperUnits = (EPlotPaperUnits)p; return true; }
    virtual bool set_73(int p) { m_plotRotation  = (EPlotRotation)p; return true; }
    virtual bool set_74(int p) { m_plotType = (EPlotType)p; return true; }
    virtual bool set_75(int p) { m_scaleType = (EPlotScaleType)p; return true; }

    virtual bool set_40(sds_real p) { m_leftBottomMargin[0] = p; return true; }
    virtual bool set_41(sds_real p) { m_leftBottomMargin[1] = p; return true; }
    virtual bool set_42(sds_real p) { m_rightTopMargin[0] = p; return true; }
    virtual bool set_43(sds_real p) { m_rightTopMargin[1] = p; return true; }
    virtual bool set_44(sds_real p) { m_plotPaperSize[0] = p; return true; }
    virtual bool set_45(sds_real p) { m_plotPaperSize[1] = p; return true; }
    virtual bool set_46(sds_real p) { m_plotOrigin[0] = p; return true; }
    virtual bool set_47(sds_real p) { m_plotOrigin[1] = p; return true; }
    virtual bool set_48(sds_real p) { m_winMin[0] = p; return true; }
    virtual bool set_49(sds_real p) { m_winMin[1] = p; return true; }
    virtual bool set_140(sds_real p) { m_winMax[0] = p; return true; }
    virtual bool set_141(sds_real p) { m_winMax[1] = p; return true; }

    virtual bool set_142(sds_real p) { m_customScaleNumerator = p; return true; }
    virtual bool set_143(sds_real p) { m_customScaleDenominator = p; return true; }
    virtual bool set_147(sds_real p) { m_scaleFactor = p; return true; }
    virtual bool set_148(sds_real p) { m_paperImageOrigin[0] = p; return true; }
    virtual bool set_149(sds_real p) { m_paperImageOrigin[1] = p; return true; }

    virtual bool get_2(char** p) { *p = m_plotConfig; return true; }
    virtual bool get_4(char** p) { *p = m_paperSize; return true; }
    virtual bool get_6(char** p) { *p = m_plotView; return true; }
    virtual bool get_7(char** p) { *p = m_curStyleSheet; return true; }
    virtual bool get_70(int *p) { *p = m_plotLayoutFlag; return true; }
    virtual bool get_72(int *p) { *p = m_plotPaperUnits; return true; }
    virtual bool get_73(int *p) { *p = m_plotRotation; return true; }
    virtual bool get_74(int *p) { *p = m_plotType; return true; }
    virtual bool get_75(int *p) { *p = m_scaleType; return true; }

    virtual bool get_40(sds_real *p) { *p = m_leftBottomMargin[0]; return true; }
    virtual bool get_41(sds_real *p) { *p = m_leftBottomMargin[1]; return true; }
    virtual bool get_42(sds_real *p) { *p = m_rightTopMargin[0]; return true; }
    virtual bool get_43(sds_real *p) { *p = m_rightTopMargin[1]; return true; }
    virtual bool get_44(sds_real *p) { *p = m_plotPaperSize[0]; return true; }
    virtual bool get_45(sds_real *p) { *p = m_plotPaperSize[1]; return true; }
    virtual bool get_46(sds_real *p) { *p = m_plotOrigin[0]; return true; }
    virtual bool get_47(sds_real *p) { *p = m_plotOrigin[1]; return true; }
    virtual bool get_48(sds_real *p) { *p = m_winMin[0]; return true; }
    virtual bool get_49(sds_real *p) { *p = m_winMin[1]; return true; }
    virtual bool get_140(sds_real *p) { *p = m_winMax[0]; return true; }
    virtual bool get_141(sds_real *p) { *p = m_winMax[1]; return true; }

    virtual bool get_142(sds_real *p) { *p = m_customScaleNumerator; return true; }
    virtual bool get_143(sds_real *p) { *p = m_customScaleDenominator; return true; }
    virtual bool get_147(sds_real *p) { *p = m_scaleFactor; return true; }
    virtual bool get_148(sds_real *p) { *p = m_paperImageOrigin[0]; return true; }
    virtual bool get_149(sds_real *p) { *p = m_paperImageOrigin[1]; return true; }
private:
    char  m_deleted;

	// 1
	//char m_name[IC_ACADBUF];
	// 2
	char m_plotConfig[IC_ACADBUF];
	// 4
	char m_paperSize[IC_ACADBUF];
	// 6
	char m_plotView[IC_ACADBUF];
	// 7
	char m_curStyleSheet[IC_ACADBUF];

	// 40, 41, 42, 43
	double m_leftBottomMargin[2];
	double m_rightTopMargin[2];
	// 44, 45
	double m_plotPaperSize[2];
	// 46, 47
	double m_plotOrigin[2];
	// 48, 49, 140, 141
	double m_winMin[2];
	double m_winMax[2];
	// 142 real world paper units
	double m_customScaleNumerator;
	// 143 drawing units
	double m_customScaleDenominator;
	// 70
	short  m_plotLayoutFlag;
	// 72
	EPlotPaperUnits m_plotPaperUnits;
	// 73
	EPlotRotation m_plotRotation;
	// 74
	EPlotType m_plotType;
	// 75
	EPlotScaleType m_scaleType;
	// 147
	double m_scaleFactor;
	// 148, 149
	double m_paperImageOrigin[2];
};

inline void CDbPlotSettings::getPlotPaperMargins(double& printXmin, double& printYmin, double& printXmax, double& printYmax) const
{
	printXmin = m_leftBottomMargin[0];
	printYmin = m_leftBottomMargin[1];
	printXmax = m_rightTopMargin[0];
	printYmax = m_rightTopMargin[1];
}

inline void CDbPlotSettings::getPlotPaperSize(double& width, double& height) const
{
	width = m_plotPaperSize[0];
	height = m_plotPaperSize[1];
}

inline void CDbPlotSettings::getPlotOrigin(double& x, double& y) const
{
	x = m_plotOrigin[0];
	y = m_plotOrigin[1];
}

inline void CDbPlotSettings::getPlotWindowArea(double& xmin, double& ymin, double& xmax, double& ymax) const
{
	xmin = m_winMin[0];
	ymin = m_winMin[1];
	xmax = m_winMax[0];
	ymax = m_winMax[1];
}

#endif
