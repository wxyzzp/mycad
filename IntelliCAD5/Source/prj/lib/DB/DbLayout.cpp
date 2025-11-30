/* C:\ICAD\PRJ\LIB\DB\LAYOUT.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "db.h"
#include "DbLayout.h"
#include "objects.h"

CDbLayout::CDbLayout(): CDbPlotSettings(DB_LAYOUT),
m_deleted(0), m_tabOrder(0), m_layoutFlag(0)
{
	memset(m_name, 0, sizeof(m_name));
	m_limMin[0] = m_limMin[1] = m_limMax[0] = m_limMax[1] = 0.0;
	m_insPoint[0] = m_insPoint[1] = m_insPoint[2] = 0.0;
	m_extMin[0] = m_extMin[1] = m_extMin[2] = 0.0;
	m_extMax[0] = m_extMax[1] = m_extMax[2] = 0.0;
	m_elevation = 0.0;
	m_ucsO[0] = m_ucsO[1] = m_ucsO[2] = 0.0;
	m_ucsX[0] = m_ucsX[1] = m_ucsX[2] = 0.0;
	m_ucsY[0] = m_ucsY[1] = m_ucsY[2] = 0.0;
	m_ucsViewType = eNotOrtho;
	m_pBlock = NULL;
	m_pLastViewport = NULL;
	m_pUCS = NULL;
	m_pBaseUCS = NULL;
}

CDbLayout::CDbLayout(const CDbLayout &l): CDbPlotSettings(l),
m_deleted(l.m_deleted), m_tabOrder(l.m_tabOrder), m_layoutFlag(l.m_layoutFlag)
{
	memcpy(m_name, l.m_name, sizeof(m_name));
	memcpy(m_limMin, l.m_limMin, 2 * sizeof(double));
	memcpy(m_limMax, l.m_limMax, 2 * sizeof(double));
	memcpy(m_insPoint, l.m_insPoint, sizeof(sds_point));
	memcpy(m_extMin, l.m_extMin, sizeof(sds_point));
	memcpy(m_extMax, l.m_extMax, sizeof(sds_point));
	m_elevation = l.m_elevation;
	memcpy(m_ucsO, l.m_ucsO, sizeof(sds_point));
	memcpy(m_ucsX, l.m_ucsX, sizeof(sds_point));
	memcpy(m_ucsY, l.m_ucsY, sizeof(sds_point));
	m_ucsViewType = l.m_ucsViewType;
	m_pBlock = l.m_pBlock;
	m_pLastViewport = l.m_pLastViewport;
	m_pUCS = l.m_pUCS;
	m_pBaseUCS = l.m_pBaseUCS;
}

CDbLayout::~CDbLayout()
{
}

int CDbLayout::entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp)
{
	CDbPlotSettings::entgetspecific(begpp, endpp, dp);

	(*endpp)->rbnext = db_newrb( 1, 'S', (void*)name(dp));
	*endpp = (*endpp)->rbnext;

	(*endpp)->rbnext = db_newrb(70, 'I', &m_layoutFlag);
	*endpp = (*endpp)->rbnext;

	ASSERT(m_pBlock != NULL);
	sds_name name;
	name[0]=(long)m_pBlock;
	name[1]=(long)dp;
	(*endpp)->rbnext = db_newrb(330, 'N', name);
	*endpp = (*endpp)->rbnext;

	if(m_pLastViewport)
	{
		name[0]=(long)m_pLastViewport;
		name[1]=(long)dp;
		(*endpp)->rbnext = db_newrb(331, 'N', name);
		*endpp = (*endpp)->rbnext;
	}

	if(m_pUCS)
	{
		name[0]=(long)m_pUCS;
		name[1]=(long)dp;
		(*endpp)->rbnext = db_newrb(345, 'N', name);
		*endpp = (*endpp)->rbnext;
	}
	
	if(m_pBaseUCS)
	{
		name[0]=(long)m_pBaseUCS;
		name[1]=(long)dp;
		(*endpp)->rbnext = db_newrb(346, 'N', name);
		*endpp = (*endpp)->rbnext;
	}
	return 0;
}

int CDbLayout::entmod(struct resbuf *modll, db_drawing *dp)
{
	CDbPlotSettings::entmod(modll, dp);

	// TODO: try to remove dependence on order of layout flag in list
	if(ic_assoc(modll, 70) == 0)
		if(ic_assoc(ic_rbassoc->rbnext, 70) == 0)
			m_layoutFlag = ic_rbassoc->resval.rint;

	// there can be several 330 groups in the object list
	// block name should be last
	struct resbuf* pBlockName = modll;
	while(ic_assoc(pBlockName->rbnext, 330) == 0)
		pBlockName = ic_rbassoc;
	m_pBlock = (db_handitem*)pBlockName->resval.rlname[0];

	if(ic_assoc(modll, 331) == 0)
		m_pLastViewport = (db_handitem*)ic_rbassoc->resval.rlname[0];

	if(ic_assoc(modll, 345) == 0)
		m_pUCS = (db_handitem*)ic_rbassoc->resval.rlname[0];
	else
		m_pUCS = NULL;

	if(ic_assoc(modll, 345) == 0)
		m_pBaseUCS = (db_handitem*)ic_rbassoc->resval.rlname[0];
	else
		m_pBaseUCS = NULL;

	return 0;
}

const char* CDbLayout::name(db_drawing* pDrawing)/*const*/
{
	db_dictionary* pDict = (db_dictionary*)pDrawing->dictsearch(pDrawing->namedobjdict(), "ACAD_LAYOUT" /*DNT*/, 0);
	if(pDict)
	{
		db_dictionaryrec* pCurRec;
		pDict->get_recllends(&pCurRec, NULL);
		while(pCurRec != NULL)
		{
			if(pCurRec->hiref->ret_hip(pDrawing) == this)
				return pCurRec->name;
			pCurRec = pCurRec->next;
		}
	}
	// this LAYOUT is not in dictionary (strange)
	assert(false);
	return m_name;
}

void CDbLayout::setName(char* pName, db_drawing* pDrawing)
{
	db_dictionary* pDict = (db_dictionary*)pDrawing->dictsearch(pDrawing->namedobjdict(), "ACAD_LAYOUT" /*DNT*/, 0);
	if(pDict)
	{
		db_dictionaryrec* pCurRec;
		pDict->get_recllends(&pCurRec, NULL);
		while(pCurRec != NULL)
		{
			if(pCurRec->hiref->ret_hip(pDrawing) == this)
				break;
			pCurRec = pCurRec->next;
		}
		if(pCurRec != NULL)
			db_astrncpy(&pCurRec->name, pName, -1);
		else
			// this LAYOUT is not in dictionary (strange)
			assert(false);
	}
	strncpy(m_name, pName, IC_ACADBUF - 1);
}
