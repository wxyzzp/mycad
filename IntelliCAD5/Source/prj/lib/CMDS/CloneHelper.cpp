#include "cmds.h"/*DNT*/
#include "CloneHelper.h"
#include "cmdsLayout.h"
#include "db.h"/*DNT*/
#include "fixup1005.h"/*DNT*/
#include "styletabrec.h"/*DNT*/
#include "DbLayout.h"
#include "DbAcisEntity.h"/*DNT*/
#include "Modeler.h"/*DNT*/
#include "transf.h"/*DNT*/
using icl::transf;
#include "gvector.h"/*DNT*/
using icl::point;
using icl::gvector;
#include "IcadApi.h"/*DNT*/
#include "IcadDoc.h"/*DNT*/

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static bool s_bKeepSpaceFlag67 = false;

sds_point CCloneHelper::s_zeroPoint = {0.0, 0.0, 0.0};

CCloneHelper::CCloneHelper(db_drawing* pDrawingDest, db_drawing* pDrawingSource): 
m_pDrawingDest(pDrawingDest), m_pDrawingSource(pDrawingSource), 
m_fileType(IC_DWG), m_fileVersion(IC_ACAD2004), m_bReportError(false)
{
	ASSERT(m_pDrawingDest != m_pDrawingSource);

	m_bKeepSpaceFlag67 = false;
	m_sdsLayoutName[0] = 0;
	m_sdsLayoutName[1] = 0;
}

CCloneHelper::CCloneHelper(char fileType, int fileVersion): 
m_pDrawingDest(NULL), m_fileType(fileType), m_fileVersion(fileVersion), m_bReportError(false)
{
	m_bKeepSpaceFlag67 = false;
	m_sdsLayoutName[0] = 0;
	m_sdsLayoutName[1] = 0;
}

int CCloneHelper::copy(sds_name entitiesToCopy)
{
	ASSERT(m_pDrawingDest != NULL);
	ASSERT(m_pDrawingSource != NULL);
	ASSERT(m_pDrawingDest != m_pDrawingSource);

	SDS_CallUserCallbackFunc(SDS_CBBEGINCLONE, (void*)entitiesToCopy, NULL, NULL);
	// track the 1005 handles in eed so that they
	// can be fixed up to be in sync later
	m_pDrawingDest->Make1005FixupTables();

	// Set the flag to copying so new associations will not be made until copy is over.
	m_pDrawingDest->SetCopyingFlag(true);

	// SystemMetrix(Hiro)-[FIX: the copied annotation is associated with invalid leader.
	ReactorFixups reactorFixup;
	// ]-SystemMetrix(Hiro) FIX: the copied annotation is associated with invalid leader.

	s_bKeepSpaceFlag67 = true;

	struct resbuf filter;
	filter.restype = RTSTR; 
	filter.resval.rstring = "*"/*DNT*/; 
	filter.rbnext = NULL;

	struct resbuf* pEntityList = NULL;
	sds_name copiedEntities;
	sds_name sdsEntityName, sdsLastEntityName;
	for (long i = 0; sds_ssname(entitiesToCopy, i, sdsEntityName) == RTNORM; i++) 
	{
		pEntityList = sds_entgetx(sdsEntityName, &filter);
		ASSERT(pEntityList != NULL);
		
		// Deal with complex entities.
		// If we have a polyline or an insert with attributes, process everything BUT the seqend here.
		if (ic_assoc(pEntityList, 66) == 0 && 
			ic_rbassoc->resval.rint == 1 && 
			ic_assoc(pEntityList,0) == 0 && 
			(strisame("INSERT"/*DNT*/, ic_rbassoc->resval.rstring) || 
			strisame("POLYLINE"/*DNT*/, ic_rbassoc->resval.rstring))) 
		{
			while(!strisame("SEQEND"/*DNT*/, ic_rbassoc->resval.rstring)) 
			{
				if(cmd_ItemInsert(m_pDrawingSource, m_pDrawingDest, NULL, pEntityList) == FALSE) 
				{ 
					CMD_INTERNAL_MSG(-__LINE__);
					m_pDrawingDest->Delete1005FixupTables();
					sds_relrb(pEntityList);
					sds_ssfree(copiedEntities);
					s_bKeepSpaceFlag67 = false;
					return 0;
				}

				sds_relrb(pEntityList);
				if(sds_entnext_noxref(sdsEntityName, sdsEntityName) != RTNORM) 
				{ 
					CMD_INTERNAL_MSG(-__LINE__);
					m_pDrawingDest->Delete1005FixupTables();
					sds_relrb(pEntityList);
					sds_ssfree(copiedEntities);
					s_bKeepSpaceFlag67 = false;
					return 0;
				}
				
				pEntityList = sds_entgetx(sdsEntityName, &filter);
				ic_assoc(pEntityList, 0);
			}
		}
		
		if(cmd_ItemInsert(m_pDrawingSource, m_pDrawingDest, NULL, pEntityList) == FALSE) 
		{ 
			CMD_INTERNAL_MSG(-__LINE__);
			m_pDrawingDest->Delete1005FixupTables();
			sds_relrb(pEntityList);
			sds_ssfree(copiedEntities);
			s_bKeepSpaceFlag67 = false;
			return 0;
		}
		
		sds_relrb(pEntityList);
		
		// Setup the table so the reactors can be fixed.
		//sds_entlast(sdsLastEntityName);
		sdsLastEntityName[0] = (long)SDS_LastHandItem;
		sdsLastEntityName[1] = (long)m_pDrawingDest;

		sds_ssadd(sdsLastEntityName, copiedEntities, copiedEntities);

		db_handitem* pSourceEntity = (db_handitem*)sdsEntityName[0];
		// old entity may be a seqend so need to get the polyline
		if(pSourceEntity->ret_type() == DB_SEQEND)
			((db_seqend*)pSourceEntity)->get_mainent(&pSourceEntity);
			
		db_handitem* pDestEntity = (db_handitem*)sdsLastEntityName[0];
		reactorFixup.AddHanditem(pDestEntity);
		reactorFixup.AddHandleMap(pSourceEntity->RetHandle(), pDestEntity->RetHandle());
	}
				
	// SystemMetrix(Hiro)-[FIX: the copied annotation is associated with invalid leader.
	// fix the reactors on the copied entities
	reactorFixup.ApplyFixups(SDS_CURDWG);
	// ]-SystemMetrix(Hiro) FIX: the copied annotation is associated with invalid leader.

	// Set the flag to copying so new associations will not be made until copy is over.
	m_pDrawingDest->SetCopyingFlag(false);
				
	// make sure the 1005 handles are in sync in eed
	if (m_pDrawingDest->FixingUp1005s())
	{
		m_pDrawingDest->Apply1005Fixups();
		m_pDrawingDest->Delete1005FixupTables();
	}

	// Send notification to apps
	SDS_CallUserCallbackFunc(SDS_CBENDCLONE, (void*)entitiesToCopy, copiedEntities, (void*)0);
	sds_ssfree(copiedEntities);
	s_bKeepSpaceFlag67 = false;
	return 1;
}

int CCloneHelper::saveLayout(sds_name sdsLayoutName, sds_name layoutSS, const char* pFileName)
{
	m_bKeepSpaceFlag67 = true;
	m_sdsLayoutName[0] = sdsLayoutName[0];
	m_sdsLayoutName[1] = sdsLayoutName[1];

	if(!wblock(layoutSS, pFileName))
	{
		m_bKeepSpaceFlag67 = false;
		m_sdsLayoutName[0] = 0;
		m_sdsLayoutName[1] = 0;
		return 0;
	}
	m_bKeepSpaceFlag67 = false;
	m_sdsLayoutName[0] = 0;
	m_sdsLayoutName[1] = 0;
	return 1;
}

int CCloneHelper::wblock(sds_name entitiesToClone, const char* pFileName)
{
	ASSERT(m_pDrawingDest == NULL);
	m_pDrawingSource = SDS_CURDWG;

	ReactorFixups reactorFixup;
	//*** Loop through entities
	db_setfontsubstfn(NULL);

	struct resbuf setgetrb;
	int measurement;
	int pstylemode;
	SDS_getvar(NULL, DB_QMEASUREMENT, &setgetrb, m_pDrawingSource, &SDS_CURCFG, &SDS_CURSES);
	measurement = setgetrb.resval.rint;
	SDS_getvar(NULL, DB_QPSTYLEMODE, &setgetrb, m_pDrawingSource, &SDS_CURCFG, &SDS_CURSES);
	pstylemode = setgetrb.resval.rint;
	m_pDrawingDest = new db_drawing(measurement, pstylemode);

	db_setfontsubstfn(SDS_FontSubsMsg);
	if(m_pDrawingDest == NULL) 
	{ 
		CMD_INTERNAL_MSG(-__LINE__);
		return 0;
	}

//	SDS_getvar(NULL,DB_QHANDSEED,&setgetrb,m_pDrawingSource,&SDS_CURCFG,&SDS_CURSES);
//	SDS_setvar(NULL,DB_QHANDSEED,&setgetrb,m_pDrawingDest,&SDS_CURCFG,&SDS_CURSES,0);

	//temp code - don't know why this creates a file acad won't open
	setgetrb.restype = RTSHORT;
	setgetrb.resval.rint = 0;
	SDS_setvar(NULL, DB_QLWPOLYOUT, &setgetrb, m_pDrawingDest, &SDS_CURCFG, &SDS_CURSES, 0);

//	int fi1;
//	for(fi1=0; fi1<34; fi1++) {
		//TODO - find largest stock handle in CURDWG and set handseed
		//	in m_pDrawingDest (new drawing) to the next handle after it
//		m_pDrawingDest->set_stockhand(m_pDrawingSource->ret_stockhand(fi1),fi1);
//	}

	// don't generate new anonymous block names during wblock
	m_pDrawingDest->StopAnonBlkNameGen();

	// track the 1005 handles in eed so that they
	// can be fixed up to be in sync later
	m_pDrawingDest->Make1005FixupTables();

	// set copying to true for reactor tables
	m_pDrawingDest->SetCopyingFlag(true);

	//write the entities out in their current coordinates without translation.
	//	this isn't like writing out a block definition
	sds_name sdsEntityName;
	long nEntitiesCloned = 0;
	for(long i = 0; sds_ssname(entitiesToClone, i, sdsEntityName) == RTNORM; i++)
	{
		if(cloneEntity(sdsEntityName, s_zeroPoint, &reactorFixup) < 0)
			continue;
		else
			nEntitiesCloned++;
	}
	if (nEntitiesCloned == 0)
	{
		sds_sslength(entitiesToClone, &nEntitiesCloned);
		if(nEntitiesCloned != 0)
		{
			CMD_INTERNAL_MSG(-__LINE__);
			m_pDrawingDest->Delete1005FixupTables();
			delete m_pDrawingDest;
			m_pDrawingDest = NULL;
			return 0;
		}
	}

	// Apply reactor fixups
	reactorFixup.ApplyFixups(m_pDrawingDest);
	m_pDrawingDest->SetCopyingFlag(false);

	//now set the INSBASE var in the written drawing
	setgetrb.restype = RT3DPOINT;
	//don't translate pt to wcs
	ic_ptcpy(setgetrb.resval.rpoint, m_basePoint);
	SDS_setvar(NULL, DB_QINSBASE, &setgetrb, m_pDrawingDest, &SDS_CURCFG, &SDS_CURSES, 0);

	if((initVars() != 0) || (addDimstyleBlocks() != 0))
	{
		CMD_INTERNAL_MSG(-__LINE__);
		m_pDrawingDest->Delete1005FixupTables();
		delete m_pDrawingDest;
		m_pDrawingDest = NULL;
		return 0;
	}

	// make sure the 1005 handles are in sync in eed
	if (m_pDrawingDest->FixingUp1005s())
	{
		m_pDrawingDest->Apply1005Fixups();
		m_pDrawingDest->Delete1005FixupTables();
	}

	if(m_sdsLayoutName[0] && m_sdsLayoutName[1])
	{
		CDbLayout* pSourceLayout = (CDbLayout*)m_sdsLayoutName[0];
		db_drawing* pSourceDrawing = (db_drawing*)m_sdsLayoutName[1];

		CLayoutManager lm(m_pDrawingDest);
		lm.deleteLayout("Layout2");
		lm.renameLayout("Layout1", pSourceLayout->name(pSourceDrawing));

		CDbLayout* pDestLayout = lm.findLayoutNamed(pSourceLayout->name(pSourceDrawing));
		ASSERT(pDestLayout != NULL);
		pDestLayout->copyFrom(pSourceLayout);
		// TODO: copy other layout data too
	}

	SDS_getvar(NULL, DB_QMAKEBAK, &setgetrb, m_pDrawingDest, &SDS_CURCFG, &SDS_CURSES);
	/*D.G.*/// Encreased dxf dec. precision from 6 to 12.
	if(SDS_CMainWindow->m_pFileIO.drw_savefile(m_pDrawingDest,
		0L, 
		NULL, 
		const_cast<char*>(pFileName),
		m_fileType, 
		m_fileVersion, 
		0, 
		12, 
		1, 
		1, 
		setgetrb.resval.rint, 
		TRUE) == FALSE)
	{
		CMD_INTERNAL_MSG(-__LINE__);
		delete m_pDrawingDest;
		m_pDrawingDest = NULL;
		return 0;
	}
	delete m_pDrawingDest;
	m_pDrawingDest = NULL;
	return 1;
}

int CCloneHelper::wblock(const char* pBlockName, bool bMask, const char* pFileName)
{
	ASSERT(m_pDrawingDest == NULL);
	m_pDrawingSource = SDS_CURDWG;

	sds_name ename;
	struct resbuf setgetrb;
	if(!bMask)
	{
		struct resbuf* pBlockList = sds_tblsearch("BLOCK"/*DNT*/, pBlockName, 0);
		if(pBlockList == NULL) 
			return 0;

		if(ic_assoc(pBlockList, 10) !=0) 
		{ 
			sds_relrb(pBlockList);
			CMD_INTERNAL_MSG(-__LINE__); 
			return 0; 
		}
		ic_ptcpy(m_basePoint, ic_rbassoc->resval.rpoint);
		//*** Loop through entities
		if(ic_assoc(pBlockList, -2) != 0) 
		{ 
			sds_relrb(pBlockList);
			CMD_INTERNAL_MSG(-__LINE__); 
			return 0; 
		}
		ic_encpy(ename, ic_rbassoc->resval.rlname);
		db_setfontsubstfn(NULL);
		int measurement;
		int pstylemode;
		SDS_getvar(NULL, DB_QMEASUREMENT, &setgetrb, m_pDrawingSource, &SDS_CURCFG, &SDS_CURSES);
		measurement = setgetrb.resval.rint;
		SDS_getvar(NULL, DB_QPSTYLEMODE, &setgetrb, m_pDrawingSource, &SDS_CURCFG, &SDS_CURSES);
		pstylemode = setgetrb.resval.rint;
		m_pDrawingDest = new db_drawing(measurement, pstylemode);
		db_setfontsubstfn(SDS_FontSubsMsg);

		if(m_pDrawingDest == NULL) 
		{ 
			sds_relrb(pBlockList);
			CMD_INTERNAL_MSG(-__LINE__); 
			return 0; 
		}

//		SDS_getvar(NULL,DB_QHANDSEED,&setgetrb,m_pDrawingSource,&SDS_CURCFG,&SDS_CURSES);
//		SDS_setvar(NULL,DB_QHANDSEED,&setgetrb,m_pDrawingDest,&SDS_CURCFG,&SDS_CURSES,0);

//		for(int fi1=0; fi1<34; fi1++) {
			//TODO - find largest stock handle in CURDWG and set handseed
			//	in m_pDrawingDest (new drawing) to the next handle after it
//			m_pDrawingDest->set_stockhand(m_pDrawingSource->ret_stockhand(fi1),fi1);
//		}

		// don't generate new anonymous block names during wblock
		m_pDrawingDest->StopAnonBlkNameGen();

		// track the 1005 handles in eed so that they
		// can be fixed up to be in sync later
		m_pDrawingDest->Make1005FixupTables();

		// set reactor fixup tables
		m_pDrawingDest->SetCopyingFlag(true);
		ReactorFixups reactorFixup;

		do 
		{
			if(cloneEntity(ename, s_zeroPoint, &reactorFixup) < 0) 
			{
				sds_relrb(pBlockList);
				m_pDrawingDest->Delete1005FixupTables();
				delete m_pDrawingDest;
				m_pDrawingDest = NULL;
				CMD_INTERNAL_MSG(-__LINE__); 
				return 0; 
			}
		} while(sds_entnext_noxref(ename, ename) == RTNORM);

		// apply reactor fixups
		reactorFixup.ApplyFixups(m_pDrawingDest);
		m_pDrawingDest->SetCopyingFlag(false);

		if((initVars() != 0) || (addDimstyleBlocks() != 0))
		{
			sds_relrb(pBlockList);
			m_pDrawingDest->Delete1005FixupTables();
			delete m_pDrawingDest;
			m_pDrawingDest = NULL;
			CMD_INTERNAL_MSG(-__LINE__); 
			return 0; 
		}

		//now set the INSBASE var in the written drawing
		setgetrb.restype = RT3DPOINT;
		//don't translate pt to wcs
		ic_ptcpy(setgetrb.resval.rpoint, m_basePoint);
		SDS_setvar(NULL, DB_QINSBASE, &setgetrb, m_pDrawingDest, &SDS_CURCFG, &SDS_CURSES, 0);

		// make sure the 1005 handles are in sync in eed
		if(m_pDrawingDest->FixingUp1005s())
		{
			m_pDrawingDest->Apply1005Fixups();
			m_pDrawingDest->Delete1005FixupTables();
		}

		SDS_getvar(NULL, DB_QMAKEBAK, &setgetrb, m_pDrawingSource, &SDS_CURCFG, &SDS_CURSES);
		/*D.G.*/// Encreased dxf dec. precision from 6 to 12.
		if(SDS_CMainWindow->m_pFileIO.drw_savefile(m_pDrawingDest,
			0L,
			NULL,
			const_cast<char*>(pFileName),
			m_fileType,
			m_fileVersion,
			0, 
			12, 
			1,
			1,
			setgetrb.resval.rint,
			TRUE) == FALSE) 
		{ 
			sds_relrb(pBlockList);
			delete m_pDrawingDest;
			m_pDrawingDest = NULL;
			CMD_INTERNAL_MSG(-__LINE__); 
			return 0; 
		}
		sds_relrb(pBlockList);
		delete m_pDrawingDest;
		m_pDrawingDest = NULL;
		return 1; 
	}
	else
	{
		int nBlocksCloned = 0;
		struct resbuf* pBlockList = sds_tblnext("BLOCK"/*DNT*/, 1);
		while(pBlockList != NULL)
		{
			if(ic_assoc(pBlockList, 2) != 0) 
			{ 
				if(pBlockList)
				{
					sds_relrb(pBlockList);
					pBlockList = NULL;
				}
				if(m_pDrawingDest)
				{
					m_pDrawingDest->Delete1005FixupTables();
					delete m_pDrawingDest;
					m_pDrawingDest = NULL;
				}
				CMD_INTERNAL_MSG(-__LINE__); 
				return 0; 
			}
			if(sds_wcmatch(ic_rbassoc->resval.rstring, pBlockName) == RTNORM)
			{
				if(m_pDrawingDest == NULL)
				{ 
					//if we found something to write, open the file
					db_setfontsubstfn(NULL);
					int measurement;
					int pstylemode;
					SDS_getvar(NULL, DB_QMEASUREMENT, &setgetrb, m_pDrawingSource, &SDS_CURCFG, &SDS_CURSES);
					measurement = setgetrb.resval.rint;
					SDS_getvar(NULL, DB_QPSTYLEMODE, &setgetrb, m_pDrawingSource, &SDS_CURCFG, &SDS_CURSES);
					pstylemode = setgetrb.resval.rint;
					m_pDrawingDest = new db_drawing(measurement, pstylemode);
					db_setfontsubstfn(SDS_FontSubsMsg);
					if(m_pDrawingDest == NULL) 
					{
						if(pBlockList)
						{
							sds_relrb(pBlockList);
							pBlockList = NULL;
						}
						CMD_INTERNAL_MSG(-__LINE__); 
						return 0; 
					}
					
					// don't generate new anonymous block names during wblock
					m_pDrawingDest->StopAnonBlkNameGen();
					
					// track the 1005 handles in eed so that they
					// can be fixed up to be in sync later
					m_pDrawingDest->Make1005FixupTables();
					
					//						for(int fi1=0; fi1<34; fi1++) {
					//							m_pDrawingDest->set_stockhand(m_pDrawingSource->ret_stockhand(fi1),fi1);
					//						}
				}
				if(ic_assoc(pBlockList, 10) != 0) 
				{ 
					if(pBlockList)
					{
						sds_relrb(pBlockList);
						pBlockList = NULL;
					}
					if(m_pDrawingDest)
					{
						m_pDrawingDest->Delete1005FixupTables();
						delete m_pDrawingDest;
						m_pDrawingDest = NULL;
					}
					CMD_INTERNAL_MSG(-__LINE__); 
					return 0; 
				}
				ic_ptcpy(m_basePoint, ic_rbassoc->resval.rpoint);
				
				// setup up table to fix reactors
				ReactorFixups reactorFixup;
				m_pDrawingDest->SetCopyingFlag(true);
				
				//*** Loop through entities
				if(ic_assoc(pBlockList, -2) != 0) 
				{ 
					if(pBlockList)
					{
						sds_relrb(pBlockList);
						pBlockList = NULL;
					}
					if(m_pDrawingDest)
					{
						m_pDrawingDest->Delete1005FixupTables();
						delete m_pDrawingDest;
						m_pDrawingDest = NULL;
					}
					CMD_INTERNAL_MSG(-__LINE__); 
					return 0; 
				}
				ic_encpy(ename, ic_rbassoc->resval.rlname);
				do 
				{
					if(cloneEntity(ename, m_basePoint, &reactorFixup) < 0)
					{
						if(pBlockList)
						{
							sds_relrb(pBlockList);
							pBlockList = NULL;
						}
						if(m_pDrawingDest)
						{
							m_pDrawingDest->Delete1005FixupTables();
							delete m_pDrawingDest;
							m_pDrawingDest = NULL;
						}
						CMD_INTERNAL_MSG(-__LINE__); 
						return 0; 
					}
				} while(sds_entnext_noxref(ename, ename) == RTNORM);
				nBlocksCloned++;
				
				// fixup table and restore drawing copying flag
				m_pDrawingDest->SetCopyingFlag(false);
				reactorFixup.ApplyFixups(m_pDrawingDest);
			}
			if(pBlockList != NULL)
			{
				sds_relrb(pBlockList);
				pBlockList = NULL;
			}
			pBlockList = sds_tblnext("BLOCK"/*DNT*/,0);
		}
		if(m_pDrawingDest != NULL)
		{
			if((initVars() != 0) || (addDimstyleBlocks() != 0))
			{
				m_pDrawingDest->Delete1005FixupTables();
				delete m_pDrawingDest;
				m_pDrawingDest = NULL;
				CMD_INTERNAL_MSG(-__LINE__); 
				return 0; 
			}
		
			// make sure the 1005 handles are in sync in eed
			if (m_pDrawingDest->FixingUp1005s())
			{
				m_pDrawingDest->Apply1005Fixups();
				m_pDrawingDest->Delete1005FixupTables();
			}
		
			SDS_getvar(NULL, DB_QMAKEBAK, &setgetrb, m_pDrawingSource, &SDS_CURCFG, &SDS_CURSES);
			/*D.G.*/// Encreased dxf dec. precision from 6 to 12.
			if(SDS_CMainWindow->m_pFileIO.drw_savefile(m_pDrawingDest,
				0L,
				NULL,
				const_cast<char*>(pFileName),
				m_fileType,
				m_fileVersion,
				0, 
				12, 
				1,
				1,
				setgetrb.resval.rint,
				TRUE) == FALSE) 
			{
				delete m_pDrawingDest;
				m_pDrawingDest = NULL;
				return 0;
			}
		}
		delete m_pDrawingDest;
		m_pDrawingDest = NULL;
		return nBlocksCloned;
	}
	return 0;
}

int CCloneHelper::initVars()
{
	ASSERT(m_pDrawingSource != NULL);
	ASSERT(m_pDrawingDest != NULL);

	//gets all current vars which are saved in dwg and copies their values into db.
	//also copies current Layer 0, and STANDARD style & dimstyle settings
	struct sds_resbuf* pRecordList = NULL;
	int rc = 0;

	ASSERT(m_pDrawingDest != NULL);
	db_setfontsubstfn(NULL);

	// Move LAYER 0 to the new drawing
	pRecordList = sds_tblsearch("LAYER"/*DNT*/,"0"/*DNT*/,0);
	if(pRecordList == NULL)
	{ 
		IC_RELRB(pRecordList);
		rc = (-__LINE__); 
		return rc; 
	}

	// Found out if LAYER 0 has a LINETYPE
	if(ic_assoc(pRecordList, 6) == 0)
	{
		// Get the LINETYPE and add to the drawing
		struct resbuf *prbLineType = sds_tblsearch("LTYPE"/*DNT*/, ic_rbassoc->resval.rstring, 0);
		if ( prbLineType != NULL )
		{
			// what about adding the TEXT STYLE for shapes in the LINETYPE (if any?)
			// NOTE we are ADDING a table entry
			// If the LTYPE is CONT... it will already be in the drawing, and
			// tblmesser will error instead of adding, which is what we want.
			// So we don't check the error
			//
			SDS_tblmesser(prbLineType, IC_TBLMESSER_MAKE, m_pDrawingDest);
			IC_RELRB(prbLineType);
		}
	}

	// Now add the LAYER to the drawing
	rc = SDS_tblmesser(pRecordList, IC_TBLMESSER_MODIFY, m_pDrawingDest);
	if(rc != RTNORM) 
	{ 
		IC_RELRB(pRecordList);
		rc = (-__LINE__); 
		return rc; 
	}
	IC_RELRB(pRecordList);

	// Add STANDARD style to the new drawing
	pRecordList = sds_tblsearch("STYLE"/*DNT*/, "STANDARD"/*DNT*/, 0);
	if(pRecordList == NULL)
	{ 
		IC_RELRB(pRecordList);
		rc = (-__LINE__); 
		return rc; 
	}
	rc = SDS_tblmesser(pRecordList, IC_TBLMESSER_MODIFY, m_pDrawingDest);
	if(rc != RTNORM) 
	{ 
		IC_RELRB(pRecordList);
		rc = (-__LINE__); 
		return rc; 
	}
	IC_RELRB(pRecordList);

	// Add first dimstyle in list (not necessarily "STANDARD"; can be renamed) to the new drawing
	pRecordList = sds_tblnext("DIMSTYLE", 1);
	if(pRecordList == NULL) 
	{ 
		IC_RELRB(pRecordList);
		rc = (-__LINE__); 
		return rc; 
	}
	bool bAdd = true;
	int messerMode = IC_TBLMESSER_MODIFY;
	// see if this has the name "STANDARD" or not. If not, we need to make, not modify
	struct resbuf* pNamePair = ic_ret_assoc(pRecordList, 2);
	if(!strsame(pNamePair->resval.rstring, "STANDARD")) 
	{
		messerMode = IC_TBLMESSER_MAKE;
		// also check to see if it is there already.  If it is, do not add, because the add will fail
		db_handitem* pStyle = m_pDrawingDest->findtabrec(DB_DIMSTYLETAB, pNamePair->resval.rstring, 1);
		if(pStyle != NULL) 
			bAdd = false;
	}

	if(bAdd)
	{
		rc = SDS_tblmesser(pRecordList, messerMode, m_pDrawingDest);
		if(rc != RTNORM)
		{
			IC_RELRB(pRecordList);
			rc = (-__LINE__); 
			return rc; 
		}
	}
	IC_RELRB(pRecordList);
	db_setfontsubstfn(SDS_FontSubsMsg);

	struct resbuf var;
	struct db_sysvarlink* pSysVars = db_Getsysvar();
	int nVars = db_Getnvars();
	for(int i = 0; i < nVars; i++) 
	{
		if(pSysVars[i].loc!=0)
			continue;
		if(i==DB_QHANDSEED ||
		   i==DB_QMAXACTVP ||
		   i==DB_QMENUNAME ||
		   i==DB_QCMDACTIVE ||
		   i==DB_QCVPORT ||
		   i==DB_QTILEMODE ||
		   i==DB_QINSBASE ||
		   i==DB_QSAVENAME ||
		   i==DB_QDWGPREFIX ||
		   i==DB_QTDCREATE ||
		   i==DB_QDBMOD ||
		   i==DB_QTDINDWG ||
		   i==DB_QTDUPDATE
		   || i==DB_QTDUUPDATE
		   )continue;

		SDS_getvar(NULL, i, &var, m_pDrawingSource, &SDS_CURCFG, &SDS_CURSES);
		SDS_setvar(NULL, i, &var, m_pDrawingDest, &SDS_CURCFG, &SDS_CURSES,0);
		if(var.restype == RTSTR) 
			IC_FREE(var.resval.rstring);
	}

	if(rc > 0)
		rc = 0;
	return rc;
}

int CCloneHelper::addDimstyleBlocks()
{
	ASSERT(m_pDrawingSource != NULL);
	ASSERT(m_pDrawingDest != NULL);

	// add the blocks for the dimstyle if necc
	struct resbuf* pBlockName = NULL;
	struct resbuf* pDimstyleList = NULL;
	int rc = 0;
	pDimstyleList = sds_tblnext("DIMSTYLE",1);
	if(pDimstyleList == NULL)
		return RTERROR;
	do
	{
		pBlockName = ic_ret_assoc(pDimstyleList, 5);
		if(pBlockName != NULL && pBlockName->resval.rstring[0] != '\0')
		{
			rc = cloneBlock(pBlockName->resval.rstring);
			if(rc < 0)
			{
				IC_RELRB(pDimstyleList);
				return rc;
			}
		}
		pBlockName = ic_ret_assoc(pDimstyleList, 6);
		if(pBlockName != NULL && pBlockName->resval.rstring[0] != '\0')
		{
			rc = cloneBlock(pBlockName->resval.rstring);
			if(rc < 0)
			{
				IC_RELRB(pDimstyleList);
				return rc;
			}
		}
		pBlockName = ic_ret_assoc(pDimstyleList, 7);
		if(pBlockName!=NULL && pBlockName->resval.rstring[0] != '\0')
		{
			rc = cloneBlock(pBlockName->resval.rstring);
			if(rc < 0)
			{
				IC_RELRB(pDimstyleList);
				return rc;
			}
		}
		IC_RELRB(pDimstyleList);
		pDimstyleList = sds_tblnext("DIMSTYLE", 0);
	}
	while (pDimstyleList);

	return rc;
}

int CCloneHelper::createTableForEntity(const char* pEntityType, struct resbuf* pEntityList)
{
	ASSERT(m_pDrawingSource != NULL);
	ASSERT(m_pDrawingDest != NULL);

	char tableName[IC_ACADNMLEN];
	char recordName[IC_ACADBUF];
	struct resbuf* pCurPair = NULL;
	struct resbuf* pRecordList = NULL;
	//*** Make sure tables are created for this entity
	for(pCurPair = pEntityList; pCurPair != NULL; pCurPair = pCurPair->rbnext)
	{
		if(pCurPair->restype<3 || pCurPair->restype>8)
			continue;
		switch(pCurPair->restype) 
		{
		case 3: 
			if(!strsame(pEntityType, "DIMENSION"/*DNT*/) && 
				!strsame(pEntityType, "LEADER"/*DNT*/) &&
				!strsame(pEntityType, "TOLERANCE"/*DNT*/))
				continue;
			strncpy(tableName, "DIMSTYLE"/*DNT*/, sizeof(tableName) - 1);
			break;
		case 4:
		case 5://there's no table associated w/these groups
			continue;
			break;
		case 6: 
			if(strisame(pCurPair->resval.rstring,"CONTINUOUS"/*DNT*/) ||
				strisame(pCurPair->resval.rstring,"BYBLOCK"/*DNT*/) ||
				strisame(pCurPair->resval.rstring,"BYLAYER"/*DNT*/))
				continue;
			strncpy(tableName, "LTYPE"/*DNT*/, sizeof(tableName) - 1);
			break;
		case 7: 
			strncpy(tableName, "STYLE"/*DNT*/, sizeof(tableName) - 1);
			break;
		case 8:	
			if(strsame(pCurPair->resval.rstring, "0"/*DNT*/))
				continue;
			strncpy(tableName, "LAYER"/*DNT*/, sizeof(tableName) - 1);
			break;
		}
		strncpy(recordName, pCurPair->resval.rstring, sizeof(recordName) - 1);
		pRecordList = SDS_tblgetter(tableName, recordName, (short)0, m_pDrawingDest);
		if(pRecordList == NULL) 
		{
			pRecordList = sds_tblsearch(tableName, recordName, (short)0);
			if(pRecordList == NULL) 
			{
				ASSERT(false);
				CMD_INTERNAL_MSG(__LINE__);
				return 0;
			}
			if(pCurPair->restype == 8)
			{
				//layers can have linetypes.
				ic_assoc(pRecordList, 6);
				if(sds_wcmatch(ic_rbassoc->resval.rstring, "CONTINUOUS,BYBLOCK,BYLAYER,Continuous,ByBlock,ByLayer"/*DNT*/) != RTNORM)
				{
					struct resbuf* pLinetypeList = SDS_tblgetter("LTYPE"/*DNT*/, ic_rbassoc->resval.rstring, (short)0, m_pDrawingDest);
					if(pLinetypeList == NULL)
					{
						pLinetypeList = sds_tblsearch("LTYPE"/*DNT*/, ic_rbassoc->resval.rstring, (short)0);
						if(pLinetypeList == NULL) 
						{
							ASSERT(false);
							sds_relrb(pRecordList);
							CMD_INTERNAL_MSG(__LINE__);
							return 0;
						}
						// if this linetype has associated text styles, we need them.
						struct resbuf* pLinetypePair = pLinetypeList;
						while(pLinetypePair != NULL) 
						{
							
							if (pLinetypePair->restype == 340 && pLinetypePair->resval.rlname[0] != 0L) 
							{
								char* pStyleName;
								db_handitem* pStyle = (db_handitem *)pLinetypePair->resval.rlname[0];
								pStyleName = ((db_styletabrec *)pStyle)->retp_name();
								struct resbuf* pTextStyleList = SDS_tblgetter2("STYLE"/*DNT*/, pStyleName,(short)0, m_pDrawingDest);
								if(pTextStyleList == NULL) 
								{
									pTextStyleList = sds_tblsearch("STYLE"/*DNT*/, pStyleName, (short)0);
									if(pTextStyleList == NULL) 
									{
										ASSERT(false);
										sds_relrb(pLinetypeList);
										sds_relrb(pRecordList);
										CMD_INTERNAL_MSG(__LINE__);
										return 0;
									}
									if (SDS_tblmesser(pTextStyleList, IC_TBLMESSER_MAKE, m_pDrawingDest) != RTNORM) 
									{
										ASSERT(false);
										sds_relrb(pTextStyleList);
										sds_relrb(pLinetypeList);
										sds_relrb(pRecordList);
										CMD_INTERNAL_MSG(__LINE__);
										return 0;
									}
									sds_relrb(pTextStyleList);
									
									// now point the new linetype's style entry to the new style
									pLinetypePair->resval.rlname[0]=(long)m_pDrawingDest->ret_lasttabrec(DB_STYLETAB,1);
									pLinetypePair->resval.rlname[1]=(long)m_pDrawingDest;
								}
								else 
								{ // point to the existing one instead
									struct resbuf* pNamePair = ic_ret_assoc(pTextStyleList, -1);
									if(pNamePair != NULL) 
										ic_encpy(pLinetypePair->resval.rlname, pNamePair->resval.rlname);
									sds_relrb(pTextStyleList);
								}
							}
							pLinetypePair = pLinetypePair->rbnext;
						}

						if(SDS_tblmesser(pLinetypeList, IC_TBLMESSER_MAKE, m_pDrawingDest) != RTNORM) 
						{
							ASSERT(false);
							sds_relrb(pLinetypeList);
							sds_relrb(pRecordList);
							CMD_INTERNAL_MSG(__LINE__);
							return 0;
						}
					}
					
					sds_relrb(pLinetypeList);
				}
			}
			//remove reactor stuff from table entry...
			struct resbuf* pRecordPair = NULL;
			for(pRecordPair = pRecordList; pRecordPair; pRecordPair = pRecordPair->rbnext)
			{
				if(pRecordPair->rbnext && pRecordPair->rbnext->restype == 102)
				{
					//take out all the persistent reactor crap...
					struct resbuf *rb102,*rbsearch;
					rb102 = pRecordPair->rbnext;
					for(rbsearch = rb102->rbnext; rbsearch != NULL; rbsearch = rbsearch->rbnext)
					{
						if(rbsearch->restype == 102)
						{
							pRecordPair->rbnext = rbsearch->rbnext;
							rbsearch->rbnext = NULL;
							sds_relrb(rb102);
							rb102 = rbsearch = NULL;
							break;
						}
					}
				}
			}
			if(SDS_tblmesser(pRecordList, IC_TBLMESSER_MAKE, m_pDrawingDest) != RTNORM) 
			{ 
				ASSERT(false);
				sds_relrb(pRecordList);
				CMD_INTERNAL_MSG(__LINE__);
				return 0;
			}
		}
		sds_relrb(pRecordList);
	}
	return 1;
}

void CCloneHelper::removeSpaceFlag67(struct resbuf* pEntityList)
{
	// Link around the paperspace flag & reactor data.
	struct resbuf* pCurPair = pEntityList;
	while (pCurPair) 
	{
		if(pCurPair->restype == 1001) 
			// register app if necessary
			SDS_regapp(pCurPair->resval.rstring, m_pDrawingDest);
		if(!m_bKeepSpaceFlag67 && pCurPair->rbnext && pCurPair->rbnext->restype == 67) 
		{
			struct resbuf* pSpaceFlag67 = pCurPair->rbnext;
			pCurPair->rbnext = pCurPair->rbnext->rbnext;
			pSpaceFlag67->rbnext = NULL;
			sds_relrb(pSpaceFlag67);
		}
/*		Reactors should now be supported
		else if(pCurPair->rbnext && pCurPair->rbnext->restype==102){
			//take out all the persistent reactor crap...
			if(tmprb!=NULL){sds_relrb(tmprb);tmprb=NULL;}
			struct resbuf *rb102;
			rb102=pCurPair->rbnext;
			for(tmprb=rb102->rbnext;tmprb!=NULL;tmprb=tmprb->rbnext){
				if(tmprb->restype==102){
					pCurPair->rbnext=tmprb->rbnext;
					tmprb->rbnext=NULL;
					sds_relrb(rb102);
					rb102=tmprb=NULL;
					break;
				}
			}
		}
*/
		else 
			pCurPair = pCurPair->rbnext;
	}
}

long CCloneHelper::cloneEntity(sds_name ename, sds_point inspt, ReactorFixups *reactorFixup) 
{
	ASSERT(m_pDrawingSource != NULL);
	ASSERT(m_pDrawingDest != NULL);

	//*** This function is recursive. This function changes the value of the "ename" variable
	//*** by design because it is recursive.
	struct resbuf* tmprb=NULL;
	long rc = 0L;

	struct resbuf app;
	app.restype = RTSTR; 
	app.resval.rstring = "*"/*DNT*/; 
	app.rbnext = NULL;
	struct resbuf* pEntityList = sds_entgetx(ename, &app);
	ASSERT(pEntityList != NULL);
	if(ic_assoc(pEntityList, 0) != 0) 
	{ 
		sds_relrb(pEntityList);
		rc = (-__LINE__); 
		reportError(); 
		return rc;
	}
	tmprb = ic_rbassoc;
	if (strsame(tmprb->resval.rstring, "ENDBLK")) 
	{
		sds_relrb(pEntityList);
		return rc;  // nothing to do, no ents in block
	}

	removeSpaceFlag67(pEntityList);

	//*tmprb**NOTE: DON'T MOVE TMPRB***

#ifdef BUILD_WITH_ACIS_SUPPORT
	if( !cmd_GaveACISMsg 
		&& sds_wcmatch(tmprb->resval.rstring, "BODY,3DSOLID,REGION") == RTNORM 
		&& !CModeler::get()->isStarted()) //EBATECH cause build error.)
	{
		cmd_GaveACISMsg = TRUE;
		cmd_ErrorPrompt(CMD_ERR_WBLOCKACIS, 0);
		return rc;
	}
#endif
	if(!cmd_GaveA2KEntMsg && (strsame("RTEXT"/*DNT*/, tmprb->resval.rstring) ||
		strsame("WIPEOUT"/*DNT*/, tmprb->resval.rstring) ||
		strsame("ARCALIGNEDTEXT"/*DNT*/, tmprb->resval.rstring))) 
	{
		cmd_GaveA2KEntMsg = TRUE;
		cmd_ErrorPrompt(CMD_ERR_UNSUPPORTEDA2KENT, 0);
		return rc;
	}
	if(!cmd_GaveProxyMsg && strsame("ACAD_PROXY_OBJECT"/*DNT*/, tmprb->resval.rstring)) 
	{
		cmd_GaveProxyMsg = TRUE;
		cmd_ErrorPrompt(CMD_ERR_WBLOCKPROXY, 0);
		return rc;
	}

	if(strsame("SEQEND"/*DNT*/, tmprb->resval.rstring))
	{
		//all table entries exist - this is a recursive call
		if(m_pDrawingDest->entmake(pEntityList))
		{
			sds_relrb(pEntityList);
			rc = (-__LINE__); 
			reportError(); 
			return rc;
		}
		return rc;
	}
	if(!icadPointEqual(inspt, s_zeroPoint))
	{
		sds_point pt1, pt2;
		//*** Translate the insertion point to (0,0).
		for(struct resbuf* pCurPair = pEntityList; pCurPair != NULL; pCurPair = pCurPair->rbnext)
		{
			if(pCurPair->restype >= 10 && pCurPair->restype <= 18)
			{
				if(pCurPair->restype == 11 && sds_wcmatch(tmprb->resval.rstring, "RAY,XLINE,MTEXT,ELLIPSE,IMAGE"/*DNT*/) == RTNORM)
					continue;
				else 
					if(pCurPair->restype == 12 && sds_wcmatch(tmprb->resval.rstring,"SPLINE,IMAGE"/*DNT*/) == RTNORM)
						continue;
				else 
					if(pCurPair->restype==13 && sds_wcmatch(tmprb->resval.rstring,"SPLINE,IMAGE"/*DNT*/) == RTNORM)
						continue;
				ic_ptcpy(pt1, pCurPair->resval.rpoint);
				ic_ptcpy(pt2, s_zeroPoint); 
				pt2[2] = pt1[2];
				sds_polar(pt2, sds_angle(inspt, pt1), cmd_dist2d(inspt, pt1), pCurPair->resval.rpoint);
			}
		}
	}
	if(!strsame("VERTEX"/*DNT*/,tmprb->resval.rstring)) 
	{
		if(!createTableForEntity(tmprb->resval.rstring, pEntityList))
		{
			sds_relrb(pEntityList);
			rc = (-__LINE__); 
			reportError(); 
			return rc;
		}
	}

	if(strsame("INSERT"/*DNT*/, tmprb->resval.rstring) || strsame("DIMENSION"/*DNT*/, tmprb->resval.rstring))
	{
		if(ic_assoc(pEntityList, 2) == 0) 
		{
			char blockName[IC_ACADBUF];
			strncpy(blockName, ic_rbassoc->resval.rstring, sizeof(blockName) - 1);
			//*** Call this recursive function to define the block entities for this insert.
			rc = cloneBlock(blockName);
			if(rc < 0L) 
			{
				sds_relrb(pEntityList);
				rc = (-__LINE__); 
				reportError(); 
				return rc;
			}
		}
	}
	else 
		if(strsame("POLYLINE"/*DNT*/, tmprb->resval.rstring)) 
		{
			if(ic_assoc(pEntityList, 66) == 0 && ic_rbassoc->resval.rint == 1) 
			{
				//NOTE: For plines, the recursive call is AFTER we've already made
				//	the main ent. We won't want to do another entmake below
				db_handitem* pNewPline;
				if(m_pDrawingDest->entmake(pEntityList, &pNewPline)) 
				{ 
					sds_relrb(pEntityList);
					rc = (-__LINE__); 
					reportError(); 
					return rc;
				}
				
				// store reactors
				if (reactorFixup)
				{
					db_handitem* pOldPline = (db_handitem*)ename[0];
					
					reactorFixup->AddHanditem(pNewPline);
					reactorFixup->AddHandleMap(pOldPline->RetHandle(), pNewPline->RetHandle());
				}
				
				while(sds_entnext_noxref(ename, ename) == RTNORM) 
				{
					rc = cloneEntity(ename, inspt, reactorFixup);
					if(rc < 0L)
					{
						sds_relrb(pEntityList);
						reportError(); 
						return rc;
					}
					else 
						if(rc == 1L) 
						{
							// it seems there is no such a case now
							ASSERT(false);

							sds_relrb(pEntityList);
							rc = 0L;
							return rc;
						}
					if(IC_TYPE_FROM_ENAME(ename) == DB_SEQEND) 
						break;
				}
				return rc;//we already made the pline itself, so skip entmake below
			}
		}

	db_handitem* pNewEntity;
	if(strsame("IMAGE"/*DNT*/, tmprb->resval.rstring))
		SDS_entmakeImageInsert(pEntityList, m_pDrawingDest);
	else
	{
		if(m_pDrawingDest->entmake(pEntityList, &pNewEntity)) 
		{ 
			// first endblk made here
			SDS_getvar(NULL, DB_QERRNO,&app, m_pDrawingSource, &SDS_CURCFG, &SDS_CURSES);
			sds_relrb(pEntityList);
			rc = (-__LINE__); 
			reportError(); 
			return rc;
		}

		// store reactors
		if (reactorFixup)
		{
			db_handitem* pOldEntity = (db_handitem*)ename[0];

			reactorFixup->AddHanditem(pNewEntity);
			reactorFixup->AddHandleMap(pOldEntity->RetHandle(), pNewEntity->RetHandle());
		}
	}
	if(strsame("INSERT"/*DNT*/, tmprb->resval.rstring))
	{
		if(ic_assoc(pEntityList, 66) == 0 && ic_rbassoc->resval.rint == 1) 
		{
			while(sds_entnext_noxref(ename, ename) == RTNORM) 
			{
				rc = cloneEntity(ename, inspt, reactorFixup);
				if(rc < 0L)
				{
					sds_relrb(pEntityList);
					reportError(); 
					return rc;
				}
				else 
					if(rc == 1L) 
					{
						// it seems there is no such a case now
						ASSERT(false);

						rc = 0L;
						sds_relrb(pEntityList);
						reportError(); 
						return rc;
					}
				if(IC_TYPE_FROM_ENAME(ename) == DB_SEQEND)
					break;
			}
		}
	}
	if(IC_TYPE_FROM_ENAME(ename) == DB_MLINE)
	{
		sds_name newename;
		newename[0] = (long)pNewEntity;
		newename[1] = (long)m_pDrawingDest;;
		SDS_updateMLine(ename, newename);
	}

	return rc;
}

void CCloneHelper::reportError()
{
	if(!m_bReportError)
	{
		cmd_ErrorPrompt(CMD_ERR_CREATEENT, 0);
		m_bReportError = true;
	}
}

void CCloneHelper::removeSpaceFlag67ForBlock(struct resbuf* pBlockList)
{
	// Link around the paperspace flag & reactor data. -- added repair 1/12/99
	struct resbuf* pCurPair = pBlockList;
	while (pCurPair) 
	{
		if(pCurPair->rbnext && pCurPair->rbnext->restype == 67) 
		{
			struct resbuf* pSpaceFlag67 = pCurPair->rbnext;
			pCurPair->rbnext = pCurPair->rbnext->rbnext;
			pSpaceFlag67->rbnext = NULL;
			sds_relrb(pSpaceFlag67);
		}
		else 
			if(pCurPair->rbnext && pCurPair->rbnext->restype == 102)
			{
				//take out all the persistent reactor crap...
				struct resbuf* pReactors102 = pCurPair->rbnext;
				struct resbuf* pReactorPair = NULL;
				for(pReactorPair = pReactors102->rbnext; pReactorPair != NULL; pReactorPair = pReactorPair->rbnext)
				{
					if(pReactorPair->restype == 102)
					{
						pCurPair->rbnext = pReactorPair->rbnext;
						pReactorPair->rbnext = NULL;
						sds_relrb(pReactors102);
						break;
					}
				}
			}
			else 
				pCurPair=pCurPair->rbnext;
	}
}

long CCloneHelper::cloneBlock(const char* pBlockName) 
{
	ASSERT(m_pDrawingSource != NULL);
	ASSERT(m_pDrawingDest != NULL);

	//*** This function is recursive to create block entities for inserts.
	sds_name tmpenm;
	sds_point basept;
	long rc = 0L;
	ReactorFixups reactorFixup;

	////*** Create the BLOCK entity if it is not in the table
	//NOTE: you CANNOT just search and build if the block is anonymous,
	//	because you may have already made a block by that name
	struct resbuf* pBlockRecordList = SDS_tblgetter("BLOCK"/*DNT*/, pBlockName, (short)0, m_pDrawingDest);
	if(pBlockRecordList == NULL) 
	{
		//NOTE:m_pDrawingDest is NOT the current dwg
		pBlockRecordList = sds_tblsearch("BLOCK"/*DNT*/, pBlockName, (short)0);
		if(pBlockRecordList == NULL)
		{ 
			rc = (-__LINE__); 
			return rc;
		}
		if(ic_assoc(pBlockRecordList, -2) != 0) 
		{ 
			sds_relrb(pBlockRecordList);
			rc = (-__LINE__); 
			return rc;
		}
		ic_encpy(tmpenm, ic_rbassoc->resval.rlname);
		if(tmpenm[0] == 0) 
		{ 
			sds_relrb(pBlockRecordList);
			rc = (-__LINE__); 
			return rc; 
		}
		// try to cut off xrefs here
		if(tmpenm[1] != (long)SDS_CURDWG) 
		{	
			// stepping into an xref
			tmpenm[1] = (long)SDS_CURDWG;
			struct resbuf* rb = ic_ret_assoc(pBlockRecordList, -1);
			if (rb != NULL)
			{
				db_handitem* pHanditem;
				((db_blocktabrec *)rb->resval.rlname[0])->get_blockllends(&pHanditem, (db_handitem**)&tmpenm[0]);
			}
		}

		removeSpaceFlag67ForBlock(pBlockRecordList);

		//*** Check for nested blocks before creating this block.
		if(IC_TYPE_FROM_ENAME(tmpenm) == DB_ENDBLK)
		{	// was DB_SEQEND
			//an empty block w/no ents
			//make the block entity & then go to endblk
			if(m_pDrawingDest->entmake(pBlockRecordList)) 
			{ 
				sds_relrb(pBlockRecordList);
				rc = (-__LINE__); 
				return rc; 
			}
		}
		else
		{
			sds_name sdsBlockEntityName;
			ic_encpy(sdsBlockEntityName, tmpenm);
			do
			{
				struct resbuf* pEntityList = sds_entget(sdsBlockEntityName);
				ASSERT(pEntityList != NULL);
				
				if(ic_assoc(pEntityList, 0) != 0) 
				{ 
					sds_relrb(pEntityList);
					sds_relrb(pBlockRecordList);
					rc = (-__LINE__); 
					return rc; 
				}
				if(strsame("INSERT"/*DNT*/, ic_rbassoc->resval.rstring) || 
					strsame("DIMENSION"/*DNT*/, ic_rbassoc->resval.rstring)) 
				{
					if(ic_assoc(pEntityList, 2) != 0) 
					{ 
						sds_relrb(pEntityList);
						sds_relrb(pBlockRecordList);
						rc = (-__LINE__); 
						return rc; 
					}
					char blockName[IC_ACADBUF];
					strncpy(blockName, ic_rbassoc->resval.rstring, sizeof(blockName) - 1);
					
					//*** This is the recursion call.
					rc = cloneBlock(blockName);
					if(rc < 0L)  
					{
						sds_relrb(pEntityList);
						sds_relrb(pBlockRecordList);
						return rc; 
					}
				}
				sds_relrb(pEntityList);
			}while(sds_entnext_noxref(sdsBlockEntityName, sdsBlockEntityName) == RTNORM);
			if(ic_assoc(pBlockRecordList, 10) != 0) 
			{ 
				sds_relrb(pBlockRecordList);
				rc = (-__LINE__); 
				return rc; 
			}
			ic_ptcpy(basept, ic_rbassoc->resval.rpoint);
			//*** Entmake the BLOCK entity
			if(m_pDrawingDest->entmake(pBlockRecordList))
			{
				sds_relrb(pBlockRecordList);
				rc = (-__LINE__); 
				return rc; 
			}
			//*** Entmake the first subentity
			
			// setup copy flag for reactor fixup tables
			m_pDrawingDest->SetCopyingFlag(true);
			
			// bugfix -- while top level entities during wblock ARE shifted by the basepoint of the block being written,
			// subentities within other blocks ARE NOT so shifted.
			rc = cloneEntity(tmpenm,/* basept*/ s_zeroPoint, &reactorFixup);
			if(rc < 0L) 
			{
				sds_relrb(pBlockRecordList);
				return rc; 
			}
			//*** Entmake the remaining subentities
			while(sds_entnext_noxref(tmpenm, tmpenm) == RTNORM) 
			{
				rc = cloneEntity(tmpenm, /*basept*/s_zeroPoint, &reactorFixup);
				if(rc < 0L) 
				{
					sds_relrb(pBlockRecordList);
					return rc; 
				}
				if(rc == 1L) 
				{
					// it seems there is no such a case now
					ASSERT(false);

					rc = 0L; 
					break; 
				}
			}
			
			//fixup any pointers
			reactorFixup.ApplyFixups(m_pDrawingDest);
			m_pDrawingDest->SetCopyingFlag(false);
		}
		//*** Create the ENDBLK subentity
		sds_relrb(pBlockRecordList);
		struct resbuf* pBlockRecordList = sds_buildlist(RTDXF0, "ENDBLK"/*DNT*/, 0);
		ASSERT(pBlockRecordList != NULL);
		if(m_pDrawingDest->entmake(pBlockRecordList)) 
		{ 
			sds_relrb(pBlockRecordList);
			// second endblk made here, no -1 group
			rc = (-__LINE__); 
			return rc;
		}
	}
	sds_relrb(pBlockRecordList);
	return rc;
}

static BOOL cmd_BuildElist(struct resbuf *rbt,db_drawing *To_dbDrawing,db_drawing *Frm_dbDrawing) 
{
    struct resbuf *rbb=NULL, *newrb;
	int isdim=0,isdimsty=0;

    for(newrb=rbt;newrb;newrb=newrb->rbnext) 
	{
        switch(newrb->restype) 
		{
			case    0:
					   if(strisame(newrb->resval.rstring,"DIMENSION"/*DNT*/)) 
						   isdim=1;
					   if(strisame(newrb->resval.rstring,"DIMSTYLE"/*DNT*/)) 
						   isdimsty=1;
					   break;
            case    3: // Search for that DimStyle in the DimStyle table from the drawing that contains that block. (DIMSTYLE)
					   if(!isdim) 
						   break;
                       if ((rbb=SDS_tblgetter("DIMSTYLE"/*DNT*/,newrb->resval.rstring,TRUE,To_dbDrawing))==NULL) 
					   {
                           if ((rbb=SDS_tblgetter("DIMSTYLE"/*DNT*/,newrb->resval.rstring,TRUE,Frm_dbDrawing))!=NULL) 
						   { // Create the enitity.
							   cmd_BuildElist(rbb,To_dbDrawing,Frm_dbDrawing);
                               if (SDS_tblmesser(rbb,IC_TBLMESSER_MAKE,To_dbDrawing)!=RTNORM) 
							   { 
								   sds_relrb(rbb); 
								   return(FALSE); 
							   }
							   sds_relrb(rbb);
                           }
                       } 
					   else 
						   sds_relrb(rbb);
                       break;
            case    2: // Search for that DimStyle in the DimStyle table from the drawing that contains that block. (DIMSTYLE)
					   if(!isdimsty) 
						   break;
                       if ((rbb=SDS_tblgetter("STYLE"/*DNT*/,newrb->resval.rstring,TRUE,To_dbDrawing))==NULL) 
					   {
                           if ((rbb=SDS_tblgetter("STYLE"/*DNT*/,newrb->resval.rstring,TRUE,Frm_dbDrawing))!=NULL) 
						   { // Create the enitity.
                               if (SDS_tblmesser(rbb,IC_TBLMESSER_MAKE,To_dbDrawing)!=RTNORM) 
							   { 
								   sds_relrb(rbb); 
								   return(FALSE); 
							   }
                              sds_relrb(rbb);
                           }
                       } 
					   else 
						   sds_relrb(rbb);
                       break;
            case    6: // Search for the linetype in the linetype table from the drawing that contains the block. (LINETYPE)
                       rbb=NULL;
                       if (stricmp(newrb->resval.rstring,"BYLAYER"/*DNT*/) && stricmp(newrb->resval.rstring,"BYBLOCK"/*DNT*/) && ((rbb=SDS_tblgetter("LTYPE"/*DNT*/,newrb->resval.rstring,TRUE,To_dbDrawing))==NULL)) 
					   {
                           if ((rbb=SDS_tblgetter("LTYPE"/*DNT*/,newrb->resval.rstring,TRUE,Frm_dbDrawing))!=NULL) 
						   {
                               if (SDS_tblmesser(rbb,IC_TBLMESSER_MAKE,To_dbDrawing)!=RTNORM) 
							   { 
								   sds_relrb(rbb); 
								   return(FALSE); 
							   }
                               sds_relrb(rbb);
                           }
                       } 
					   else 
						   if (rbb) 
							   sds_relrb(rbb);
                       break;
            case    7: // Search for that TextStyle in the TextStyle table from the drawing that contains that block. (TEXTSTYLE)
                       if ((rbb=SDS_tblgetter("STYLE"/*DNT*/,newrb->resval.rstring,TRUE,To_dbDrawing))==NULL) 
					   {
                           if ((rbb=SDS_tblgetter("STYLE"/*DNT*/,newrb->resval.rstring,TRUE,Frm_dbDrawing))!=NULL) 
						   { // Create the enitity.
                               if (SDS_tblmesser(rbb,IC_TBLMESSER_MAKE,To_dbDrawing)!=RTNORM) 
							   { 
								   sds_relrb(rbb); 
								   return(FALSE); 
							   }
                              sds_relrb(rbb);
                           }
                       } 
					   else 
						   sds_relrb(rbb);
                       break;
            case    340: // Search for that TextStyle in the TextStyle table from the drawing that contains that block. (TEXTSTYLE)
					   if(!isdimsty) 
						   break;
                       {
                           db_handitem   *handitem;
                           char          *string;
                           handitem=(db_handitem *)((long)newrb->resval.rlname[0]);
                           handitem->get_2(&string);

                           if ((rbb=SDS_tblgetter("STYLE"/*DNT*/,string,TRUE,To_dbDrawing))==NULL) 
						   {
                               if ((rbb=SDS_tblgetter("STYLE"/*DNT*/,string,TRUE,Frm_dbDrawing))!=NULL) 
							   { // Create the enitity.
                                   if (SDS_tblmesser(rbb,IC_TBLMESSER_MAKE,To_dbDrawing)!=RTNORM) 
								   { 
									   sds_relrb(rbb); 
									   return(FALSE); 
								   }
                                   sds_relrb(rbb);
                               }
                           } 
						   else 
							   sds_relrb(rbb);
                       }
                       break;
            case    8: // Search for that layer in the layer table from the drawing that contains that block. (LAYER)
                       if ((rbb=SDS_tblgetter("LAYER"/*DNT*/,newrb->resval.rstring,TRUE,To_dbDrawing))==NULL) 
					   {
                           if ((rbb=SDS_tblgetter("LAYER"/*DNT*/,newrb->resval.rstring,TRUE,Frm_dbDrawing))!=NULL) 
						   { // Create the enitity.
                               cmd_BuildElist(rbb,To_dbDrawing,Frm_dbDrawing);
                               if (SDS_tblmesser(rbb,IC_TBLMESSER_MAKE,To_dbDrawing)!=RTNORM) 
							   { 
								   sds_relrb(rbb); 
								   return(FALSE); 
							   }
                               sds_relrb(rbb);
                           }
                       } 
					   else 
						   sds_relrb(rbb);
                       break;
            case 1001:
					   SDS_regapp(newrb->resval.rstring,To_dbDrawing);
                       break;
        }
    }
    return(TRUE);
}

// Had to make a new entgetx because the past code allows entget of deleted ents.
static struct sds_resbuf *cmd_entgetx(const sds_name nmEntity, const struct sds_resbuf *prbAppList) 
{
	if(nmEntity==NULL) 
		return(NULL);
	if(nmEntity[0]==0L || nmEntity[1]==0L) 
		return(NULL);
	return(((db_handitem *)nmEntity[0])->entgetx((struct resbuf *)prbAppList,(db_drawing *)nmEntity[1]));
}

BOOL cmd_ItemInsert(db_drawing *Frm_dbDrawing,db_drawing *To_dbDrawing,db_handitem *handItem,struct resbuf *rb_elist) 
{
    struct resbuf *rbt,*rbtt,
                  *elist,*elist2,
                  *ebl,
                  appname;
    CString       String;
    int           retcode;
    sds_name      eresult;
    int           type,flag,nesting;
    sds_name      ename;
	BOOL          ret=TRUE;
	char          fs1[IC_ACADBUF];

    appname.restype=RTSTR; 
	appname.resval.rstring="*"/*DNT*/; 
	appname.rbnext=NULL;

	elist=NULL;
    if (!rb_elist) 
	{
        ename[0]=(long)handItem;
        ename[1]=(long)Frm_dbDrawing;
        if ((elist=cmd_entgetx(ename,&appname))==NULL) 
		{ 
			ret=FALSE; 
			goto out; 
		}
        type=handItem->ret_type();
    } 
	else 
	{
        elist=rb_elist;
        if (ic_assoc(elist,0)) 
		{ 
			ret=FALSE; 
			goto out; 
		}
        type=db_str2hitype(ic_rbassoc->resval.rstring);

		if (ic_assoc(elist,-1)) 
		{ 
			ret=FALSE; 
			goto out; 
		}
		ic_encpy(ename,ic_rbassoc->resval.rlname);
    }

	// Link around the paperspace  & other reactor data.
// there is a potential bug here, I believe.  Each of the ifs where something is found, and
// an action is taken, probably should NOT then step to the next item in the list, since that can cause
// us to step OVER something that we actually wanted to remove.  However, due to time constraints this
// would probably not be a good time to change this.
// The change, when implemented, would be to move the rbt=rbt->rbnext to the bottom of the loop,
// with an else in front so that it happens ONLY if nothing was clipped out of the list.

	for(rbt=elist;rbt;rbt=rbt->rbnext) 
	{
        if(!s_bKeepSpaceFlag67 && rbt->rbnext && rbt->rbnext->restype==67) 
		{
			rbtt=rbt->rbnext;
			rbt->rbnext=rbt->rbnext->rbnext;
			rbtt->rbnext=NULL;
			sds_relrb(rbtt);
		}
        if(rbt->rbnext && rbt->rbnext->restype==60) 
		{
			rbtt=rbt->rbnext;
			rbt->rbnext=rbt->rbnext->rbnext;
			rbtt->rbnext=NULL;
			sds_relrb(rbtt);
		}
		if(rbt->rbnext && rbt->rbnext->restype==102)
		{
			rbtt=rbt->rbnext;
			for(ebl=rbtt->rbnext;ebl!=NULL;ebl=ebl->rbnext)
			{
				if(102==ebl->restype)
				{
					rbt->rbnext=ebl->rbnext;
					ebl->rbnext=NULL;
					sds_relrb(rbtt);
					rbtt=ebl=NULL;
					break;
				}
			}
		}
        if(db_is_tabrectype(type) && rbt->restype==70 && rbt->resval.rint&128) 
		{
            rbt->resval.rint-=128;
        }
    }

    switch(type) 
	{
        case DB_INSERT:
            if (ic_assoc(elist,2)) 
			{ 
				ret=FALSE; 
				goto out; 
			}
            ebl=SDS_tblgetter("BLOCK"/*DNT*/,ic_rbassoc->resval.rstring,TRUE,Frm_dbDrawing);
            if (ic_assoc(ebl,-1)) 
			{
				sds_relrb(ebl); 
				ebl=NULL;
				ret=FALSE; 
				goto out;
			}
            if (!cmd_ItemInsert(Frm_dbDrawing,To_dbDrawing,(db_handitem *)ic_rbassoc->resval.rlname[0],NULL)) 
			{ 
				ret=FALSE; 
				goto out; 
			}
			sds_relrb(ebl); 
			ebl=NULL;
            cmd_BuildElist(elist,To_dbDrawing,Frm_dbDrawing);
            for(rbt=elist;rbt;rbt=rbt->rbnext) 
			{
                if(rbt->restype==2 && rbt->resval.rstring && *rbt->resval.rstring=='*'/*DNT*/) 
				{
					sds_getinput(fs1);
					db_astrncpy(&rbt->resval.rstring,fs1,-1);
				}
            }
            if (SDS_entmake(elist,To_dbDrawing)==RTERROR) 
			{ 
				ret=FALSE; 
				goto out; 
			}
            break;
        case DB_BLOCK:
        case DB_BLOCKTABREC:
			flag=1;
			nesting=0;
			while(flag) 
			{
				flag=0;
				for(rbt=elist;rbt->restype!=-2;rbt=rbt->rbnext);
				// Get the first entity in the block definition.
                sds_name_set(rbt->resval.rlname,eresult);
				do 
				{
					if ((elist2=cmd_entgetx(eresult,/*NULL*/&appname))==NULL) 
					{ 
						ret=FALSE; 
						goto out; 
					}
					for(rbt=elist2;rbt;rbt=rbt->rbnext) 
					{
						if (rbt->restype==0) 
						{
							if (strisame(rbt->resval.rstring,"INSERT"/*DNT*/)) 
							{
								if (ic_assoc(rbt,2)) 
								{ 
									ret=FALSE; 
									goto out; 
								}
								ebl=SDS_tblgetter("BLOCK"/*DNT*/,ic_rbassoc->resval.rstring,TRUE,Frm_dbDrawing);
								if (ic_assoc(ebl,-1)) 
								{
									sds_relrb(ebl); 
									ebl=NULL;
									ret=FALSE; 
									goto out;
								}
								if(!cmd_ItemInsert(Frm_dbDrawing,To_dbDrawing,(db_handitem *)ic_rbassoc->resval.rlname[0],NULL)) 
								{
									flag=1;
								}
								sds_relrb(ebl); 
								ebl=NULL;
							}
						}
					}
					IC_RELRB(elist2);
                    if ((retcode=sds_entnext_noxref(eresult,eresult))!=RTNORM) 
						break;
				} while(retcode==RTNORM);
				if(++nesting>100) 
					break;
			}
            if (SDS_entmake(elist,To_dbDrawing)!=RTNORM) 
			{ 
				ret=FALSE; 
				goto out; 
			}
            // Search for the -2 group for the first entity in the block definion.
            for(rbt=elist;rbt->restype!=-2;rbt=rbt->rbnext);
            // Get the first entity in the block definition.
            sds_name_set(rbt->resval.rlname,ename);
			if(!rb_elist) 
				IC_RELRB(elist);
            do 
			{
                if ((elist=cmd_entgetx(ename,/*NULL*/&appname))==NULL) 
				{ 
					ret=FALSE; 
					goto out; 
				}
                // Duplicate the first entity in the current drawing and any (LAYERS,LINETYPES,TEXTSTYLES,DIMENSION STYLES).
                cmd_BuildElist(elist,To_dbDrawing,Frm_dbDrawing);
                // Create the enitity.
                if (SDS_entmake(elist,To_dbDrawing)!=RTNORM) 
				{ 
					ret=FALSE; 
					goto out; 
				}
				IC_RELRB(elist);
                if ((retcode=sds_entnext_noxref(ename,eresult))!=RTNORM) 
					break;
                sds_name_set(eresult,ename);
            } while(retcode==RTNORM);
            // Create the endblock.
            if ((elist=sds_buildlist(RTDXF0,"ENDBLK"/*DNT*/,0))==NULL) 
			{ 
				ret=FALSE; 
				goto out; 
			}
            if (SDS_entmake(elist,To_dbDrawing)==RTERROR) 
			{ 
				ret=FALSE; 
				goto out; 
			}
			IC_RELRB(elist);
            break;
        case DB_DIMENSION:
			// Link around the dimension block name.
            for(rbt=elist;rbt;rbt=rbt->rbnext) 
			{
                if(rbt->rbnext && rbt->rbnext->restype==2) 
				{
					rbtt=rbt->rbnext;
					rbt->rbnext=rbt->rbnext->rbnext;
					rbtt->rbnext=NULL;
					sds_relrb(rbtt);
				}
            }
            cmd_BuildElist(elist,To_dbDrawing,Frm_dbDrawing);
			SDS_AtNodeDrag=1;
            if (SDS_entmake(elist,To_dbDrawing)==RTERROR) 
			{
				SDS_AtNodeDrag=0;
				ret=FALSE; 
				goto out;
			}
			SDS_AtNodeDrag=0;
            break;
        case DB_IMAGE:
			if(SDS_entmakeImageInsert(elist,To_dbDrawing) == RTERROR) 
			{
				ret=FALSE; 
				goto out;
			}
			break;

		case DB_ACAD_PROXY_ENTITY:
			if(cmd_GaveProxyMsg) 
				break;
			cmd_ErrorPrompt(CMD_ERR_PASTEPROXY,1);
			cmd_GaveProxyMsg=1;
			break;
		case DB_3DSOLID:
		case DB_REGION:
		case DB_BODY:
#ifdef BUILD_WITH_ACIS_SUPPORT
			if(cmd_GaveACISMsg) 
				break;
			cmd_ErrorPrompt(CMD_ERR_PASTEACIS,1);
			cmd_GaveACISMsg=1;
#endif
			break;
		case DB_RTEXT:
		case DB_ARCALIGNEDTEXT:
		case DB_WIPEOUT:
			if(cmd_GaveA2KEntMsg) 
				break;
			cmd_ErrorPrompt(CMD_ERR_UNSUPPORTEDA2KENT,1);
			cmd_GaveA2KEntMsg=1;
			break;
        default:
			if(db_is_tabrectype(type) && To_dbDrawing==Frm_dbDrawing) 
				break;
            cmd_BuildElist(elist,To_dbDrawing,Frm_dbDrawing);
            if (SDS_entmake(elist,To_dbDrawing)==RTERROR) 
			{ 
				ret=FALSE; 
				goto out; 
			}

			/*D.G.*/// TO DO: remap association to the pasted boundary.
			if(type == DB_HATCH)
				((db_hatch*)To_dbDrawing->ret_lastent())->BreakAssociation();

            break;
    }

    if(type==DB_MLINE)
	{                
        sds_name newename;
        newename[0] = (long)SDS_LastHandItem;
        newename[1] = (long)To_dbDrawing;
        SDS_updateMLine(ename, newename);
	}

out: ;
	if(SDS_CURDOC) 
	{
		if(sds_entlast(ename)==RTNORM) 
			sds_ssadd(ename,SDS_CURDOC->m_pPrevSelection,SDS_CURDOC->m_pPrevSelection);
	}

	if(!rb_elist) 
		IC_RELRB(elist);
    return(ret);
}
