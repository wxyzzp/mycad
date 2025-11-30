/* C:\ICAD\PRJ\LIB\DB\PLOTSETTINGS.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "db.h"
#include "DbPlotSettings.h"
#include "objects.h"

CDbPlotSettings::CDbPlotSettings(int type): db_handitem(type),
m_deleted(0), m_plotLayoutFlag(eClear),	m_plotPaperUnits(eInches), 
m_plotRotation(e0), m_plotType(eDisplay), m_scaleType(eToFit)
{
	m_plotConfig[0] = '\0';
	m_paperSize[0] = '\0';
	m_plotView[0] = '\0';
	m_curStyleSheet[0] = '\0';

	m_leftBottomMargin[0] = m_leftBottomMargin[1] = 0.0;
	m_rightTopMargin[0] = m_rightTopMargin[1] = 0.0;
	m_plotPaperSize[0] = m_plotPaperSize[1] = 0.0;
	m_plotOrigin[0] = m_plotOrigin[1] = 0.0;
	m_winMin[0] = m_winMin[1] = 0.0;
	m_winMax[0] = m_winMax[1] = 0.0;
	m_customScaleNumerator = 1.0;
	m_customScaleDenominator = 1.0;
	m_scaleFactor = 1.0;
	m_paperImageOrigin[0] = m_paperImageOrigin[1] = 0.0;
}

CDbPlotSettings::CDbPlotSettings(const CDbPlotSettings& ps): db_handitem(ps),
m_deleted(ps.m_deleted) 
{
	copyFrom(&ps);
}

CDbPlotSettings::~CDbPlotSettings()
{
}

void CDbPlotSettings::copyFrom(const CDbPlotSettings* pSettings)
{
	strcpy(m_plotConfig, pSettings->m_plotConfig);
	strcpy(m_paperSize, pSettings->m_paperSize);
	strcpy(m_plotView, pSettings->m_plotView);
	strcpy(m_curStyleSheet, pSettings->m_curStyleSheet);

	m_plotLayoutFlag = pSettings->m_plotLayoutFlag;
	m_plotPaperUnits = pSettings->m_plotPaperUnits;
	m_plotRotation = pSettings->m_plotRotation;
	m_plotType = pSettings->m_plotType;
	m_scaleType = pSettings->m_scaleType;

	m_leftBottomMargin[0] = pSettings->m_leftBottomMargin[0]; 
	m_leftBottomMargin[1] = pSettings->m_leftBottomMargin[1];
	m_rightTopMargin[0] = pSettings->m_rightTopMargin[0];
	m_rightTopMargin[1] = pSettings->m_rightTopMargin[1];
	m_plotPaperSize[0] = pSettings->m_plotPaperSize[0];
	m_plotPaperSize[1] = pSettings->m_plotPaperSize[1];
	m_plotOrigin[0] = pSettings->m_plotOrigin[0];
	m_plotOrigin[1] = pSettings->m_plotOrigin[1];
	m_winMin[0] = pSettings->m_winMin[0];
	m_winMin[1] = pSettings->m_winMin[1];
	m_winMax[0] = pSettings->m_winMax[0];
	m_winMax[1] = pSettings->m_winMax[1];
	m_customScaleNumerator = pSettings->m_customScaleNumerator;
	m_customScaleDenominator = pSettings->m_customScaleDenominator;
	m_scaleFactor = pSettings->m_scaleFactor;
	m_paperImageOrigin[0] = pSettings->m_paperImageOrigin[0];
	m_paperImageOrigin[1] = pSettings->m_paperImageOrigin[1];
}

int CDbPlotSettings::entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp)
{
	return CDbObject::entgetspecific(begpp, endpp, dp);
}

int CDbPlotSettings::entmod(struct resbuf *modll, db_drawing *dp)
{
	return CDbObject::entmod(modll, dp);
}

const char* CDbPlotSettings::name(db_drawing* pDrawing)/*const*/
{
	db_dictionary* pDict = (db_dictionary*)pDrawing->dictsearch(pDrawing->namedobjdict(),
		"ACAD_PLOTSETTINGS", 0);
	db_dictionaryrec* pCurRec;
	pDict->get_recllends(&pCurRec, NULL);
	while(pCurRec != NULL)
	{
		if(pCurRec->hiref->ret_hip(pDrawing) == this)
			return pCurRec->name;
		pCurRec = pCurRec->next;
	}
	assert(false);
	return NULL;
}

void CDbPlotSettings::setName(char* pName, db_drawing* pDrawing)
{
	char* pS = const_cast<char*>(name(pDrawing));
	db_astrncpy(&pS, pName, -1);
}
