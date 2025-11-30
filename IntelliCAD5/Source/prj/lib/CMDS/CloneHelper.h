#pragma once

#ifndef __CloneHelperH__
#define __CloneHelperH__

class ReactorFixups;
BOOL cmd_ItemInsert(db_drawing *Frm_dbDrawing,db_drawing *To_dbDrawing,db_handitem *handItem,struct resbuf *rb_elist);

/*-----------------------------------------------------------------------------------------*//**
@author Denis Petrov

@description Helper class for cloning database objects.
*//*------------------------------------------------------------------------------------------*/
class CCloneHelper
{
public:
	CCloneHelper(db_drawing* pDrawingDest, db_drawing* pDrawingSource);
	CCloneHelper(char fileType, int fileVersion);
	void setBasePoint(sds_point basePoint) { memcpy(m_basePoint, basePoint, sizeof(m_basePoint)); }
	int wblock(sds_name entitiesToClone, const char* pFileName);
	int wblock(const char* pBlockName, bool bMask, const char* pFileName);
	int saveLayout(sds_name sdsLayoutName, sds_name layoutSS, const char* pFileName);
	int copy(sds_name entitiesToCopy);
protected:
	char m_fileType;
	int m_fileVersion;
	sds_point m_basePoint;
	static sds_point s_zeroPoint;

	db_drawing* m_pDrawingDest;
	db_drawing* m_pDrawingSource;

	bool m_bReportError;
	bool m_bKeepSpaceFlag67;
	sds_name m_sdsLayoutName;

	int initVars();
	int addDimstyleBlocks();
	int createTableForEntity(const char* pEntityType, struct resbuf* pEntityList);
	long cloneEntity(sds_name ename, sds_point inspt, ReactorFixups *reactorFixup);
	void removeSpaceFlag67(struct resbuf* pEntityList);
	void reportError();
	void removeSpaceFlag67ForBlock(struct resbuf* pBlockList);
	long cloneBlock(const char* pBlockName);
};

#endif
