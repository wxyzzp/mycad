#include "cmds.h"
#include "cmdsLayout.h"
#include "CloneHelper.h"
#include "IcadApi.h"
#include "IcadDoc.h"
#include "IcadView.h"
#include "DbLayout.h"
#include "objects.h"
#include "fixup1005.h"/*DNT*/
#include "CommandQueue.h"
#include "paths.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

short cmd_createBlock(sds_name sdsBlockName, const char* pBlockName, short blockFlag70, sds_point insertPoint, sds_name blockSS, db_drawing* pDrawing);

const char CLayoutManager::s_model[6] = "Model";

char CLayoutManager::s_defaultName[IC_ACADBUF] = "";

CLayoutManager::CLayoutManager(db_drawing* pDrawing /*= SDS_CURDWG*/)
{
	m_pDrawing = pDrawing;
	ASSERT(m_pDrawing != NULL);
	m_pDict = (db_dictionary*)m_pDrawing->dictsearch(pDrawing->namedobjdict(), "ACAD_LAYOUT", 0);
	ASSERT(m_pDict != NULL);

	resetIterator();
}

db_blocktabrec* CLayoutManager::createLayoutBlock()
{
	sds_name layoutBlock;
	sds_point ip = {0.0, 0.0, 0.0};

	char name[IC_ACADBUF];
	sprintf(name, "*Paper_Space%d", m_pDict->ret_nrecs() - 2);
	struct resbuf* pBlock = sds_buildlist(RTDXF0, "BLOCK",
		2, name,
		70, 0,
		10, ip,
		RTNONE);
	int result = sds_entmakex(pBlock, layoutBlock);
	ASSERT(result == RTNORM);
	sds_relrb(pBlock);

	struct resbuf* pBlockEnd = sds_buildlist(RTDXF0, "ENDBLK", RTNONE);
	result = sds_entmake(pBlockEnd);
	ASSERT(result == RTKWORD);
	sds_relrb(pBlockEnd);

	return static_cast<db_blocktabrec*>(((db_block*)layoutBlock[0])->ret_bthip());
}

long CLayoutManager::getLayoutEntities(sds_name layoutSS, sds_name sdsLayoutName)
{
	long nEntities = 0;
	sds_sslength(layoutSS, &nEntities);
	ASSERT(nEntities == 0);
	nEntities = 0;

	CDbLayout* pLayout = (CDbLayout*)sdsLayoutName[0];
	db_drawing* pDrawing = (db_drawing*)sdsLayoutName[1];

	extern bool SDS_InsideSSwithX;
	SDS_InsideSSwithX = true;

	sds_name sdsEntityName;
	sdsEntityName[1] = (long)pDrawing;
	db_handitem* pFirstEntity = NULL;
	db_handitem* pLastEntity = NULL;
	if(pLayout->isCurrent())
	{
		pDrawing->get_entllends(&pFirstEntity, &pLastEntity);
		if(pFirstEntity == NULL && pLastEntity == NULL)
			return 0;
		ASSERT(pFirstEntity != NULL && pLastEntity != NULL);
		while(pFirstEntity != pLastEntity)
		{
			if(!pFirstEntity->ret_pspace() || pFirstEntity->ret_deleted())
			{
				pFirstEntity = pFirstEntity->next;
				continue;
			}
			sdsEntityName[0] = (long)pFirstEntity;
			if(sds_ssadd(sdsEntityName, layoutSS, layoutSS) == RTNORM)
				++nEntities;
			pFirstEntity = pFirstEntity->next;
		}
		if(pLastEntity->ret_pspace() || pLastEntity->ret_deleted())
		{
			sdsEntityName[0] = (long)pLastEntity;
			if(sds_ssadd(sdsEntityName, layoutSS, layoutSS) == RTNORM)
				++nEntities;
		}
	}
	else
	{
		static_cast<db_blocktabrec*>(pLayout->block())->get_blockllends(&pFirstEntity, &pLastEntity);
		ASSERT(pFirstEntity != NULL && pFirstEntity->ret_type() == DB_BLOCK);
		ASSERT(pLastEntity != NULL && pLastEntity->ret_type() == DB_ENDBLK);
		if(pFirstEntity->next == pLastEntity)
			return 0;
		pFirstEntity = pFirstEntity->next;
		while(pFirstEntity != pLastEntity)
		{
			if(!pFirstEntity->ret_pspace() || pFirstEntity->ret_deleted())
			{
				pFirstEntity = pFirstEntity->next;
				continue;
			}
			sdsEntityName[0] = (long)pFirstEntity;
			if(sds_ssadd(sdsEntityName, layoutSS, layoutSS) == RTNORM)
				++nEntities;
			pFirstEntity = pFirstEntity->next;
		}
	}

	SDS_InsideSSwithX = false;
	return nEntities;
}

int CLayoutManager::addToDictionary(const char* pName, CDbLayout* pLayout)
{
	sds_name dictName;
	dictName[0] = (long)m_pDict;
	dictName[1] = (long)m_pDrawing;

	sds_name layoutName;
	layoutName[0] = (long)pLayout;
	layoutName[1] = (long)m_pDrawing;

	struct resbuf* pDictLayout = sds_entget(dictName);
	if(pDictLayout == NULL)
	{
		ASSERT(FALSE);
		sds_relrb(pDictLayout);
		return 0;
	}

	struct resbuf* pLayoutToAdd = sds_buildlist(3, pName, 350, layoutName, RTNONE);
	struct resbuf* pCurPair = pDictLayout;
	while(pCurPair->rbnext != NULL && pCurPair->rbnext->restype != 350)
		pCurPair = pCurPair->rbnext;

	while(pCurPair->rbnext != NULL && pCurPair->rbnext->restype == 350)
	{
		pCurPair = pCurPair->rbnext;
		if(pCurPair->rbnext != NULL)
			pCurPair = pCurPair->rbnext;
		else
			break;
	}

	if(pCurPair->rbnext == NULL)
		pCurPair->rbnext = pLayoutToAdd;
	else
	{
		ASSERT(pCurPair->restype == 3);
		pLayoutToAdd->rbnext->rbnext = pCurPair->rbnext->rbnext;
		pCurPair->rbnext->rbnext = pLayoutToAdd;
	}
	pLayoutToAdd = NULL;
	int result = sds_entmod(pDictLayout);
	if(result != RTNORM)
	{
		ASSERT(FALSE);
		sds_relrb(pDictLayout);
		return 0;
	}
	sds_relrb(pDictLayout);
	return 1;
}

typedef std::list<db_handitem*>	ENTS_LIST;
typedef ENTS_LIST::iterator		LI;

int CLayoutManager::setCurrentLayout(CDbLayout* pOldLayout, CDbLayout* pNewLayout)
{
	// TO DO: code for undo/redo

	// we should:
	// 1) create *paper_spaceX blockRecord which will be pointed by pOldLayout->pspaceblockrec
	// 2) move PS ents (dxf67 == 1) from ENTITIES to this block
	// 3) move ents from pNewLayout->pspaceblockrec to the ENTITIES section
	// 4) delete pNewLayout->pspaceblockrec
	// note, we have to create/delete blockRecords because icad doesn't have *paper_space (and *model_space as well) blocks;
	// however, actually we don't create/delete anything, but just change content of current objects
	db_handitem	*pFirstEnt = NULL, *pLastEnt = NULL;
	// move PS ents from ENTITIES to a temp list
	m_pDrawing->get_entllends(&pFirstEnt, &pLastEnt);
	//if(!pFirstEnt || !pLastEnt)
	//{
	//	ASSERT(FALSE);
	//	return 0;
	//}
	ENTS_LIST		elist;
	db_handitem*	pEntity = pFirstEnt;
	int				viewportID = 0;
	for( ; pEntity; pEntity = pEntity->next)
	{
		if(pEntity->ret_pspace())
		{
			if(pEntity->ret_type() == DB_VIEWPORT)
			{	// de-activate VIEWPORTs (dxf68 := 0) and temporary set IDs to zero (dxf69 := 0);
				// dxf69 == 0 is needed for db_drawing::delhanditem
				pEntity->set_68(0);
				pEntity->get_69(&viewportID);
				pEntity->set_69(0);
			}
			m_pDrawing->delhanditem(NULL, pEntity, 3, false);
			if(pEntity->ret_type() == DB_VIEWPORT)
				pEntity->set_69(viewportID);
			elist.push_back(pEntity);
		}
	}

	// move ents from pNewLayout->pspaceblockrec to ENTITIES
	// (note, excluding BLOCK and ENDBLK)
	static_cast<db_blocktabrec*>(pNewLayout->block())->get_blockllends(&pFirstEnt, &pLastEnt);
	if(!pFirstEnt || !pLastEnt)
	{
		ASSERT(FALSE);
		return 0;
	}
	// need this var because db_drawing::addhanditem nullifies db_handitem::next
	db_handitem* pNextEnt = pFirstEnt->next->next;	
	for(viewportID = 1, pEntity = pFirstEnt->next; pEntity != pLastEnt; pEntity = pNextEnt, pNextEnt = pNextEnt->next)
	{
		pNewLayout->block()->delent(pEntity, m_pDrawing);
		pEntity->set_67(1);
		m_pDrawing->addhanditem(pEntity);
		if(pEntity->ret_type() == DB_VIEWPORT)
		{
			pEntity->set_69(viewportID);	/*DG - 11.6.2002*/// Must set this as well as dxf68.
			pEntity->set_68(viewportID++);
		}
	}

	// move ents from the temp list to pNewLayout->pspaceblockrec
	pNewLayout->block()->delent(pLastEnt, m_pDrawing);	// remove pBlockEnd temporarily because db_blocktabrec::addent appends ents after the last one
	for(LI pItem = elist.begin(); pItem != elist.end(); ++pItem)
	{
		(*pItem)->next = NULL;	// for safety
		pNewLayout->block()->addent(*pItem, m_pDrawing);
	}
	pNewLayout->block()->addent(pLastEnt, m_pDrawing);

	// swap paper space blocks extension dictionaries
	db_objhandle newDictEx, oldDictEx;
	db_handitem* pNewDictEx;
	db_handitem* pOldDictEx;
	if(pNewLayout->block()->ret_xdict())
	{
		pNewLayout->block()->ret_xdict()->get_data(NULL, &newDictEx, &pNewDictEx, m_pDrawing);
		pNewLayout->block()->del_xdict();
	}
	if(m_pDrawing->ret_xdict(1))
	{
		m_pDrawing->ret_xdict(1)->get_data(NULL, &oldDictEx, &pOldDictEx, m_pDrawing);
		m_pDrawing->del_xdict(1);
	}
	if(newDictEx != NULL)
		m_pDrawing->set_xdict(1, DB_SOFT_POINTER, newDictEx, pNewDictEx);
	if(oldDictEx != NULL)
		pNewLayout->block()->set_xdict(DB_SOFT_POINTER, oldDictEx, pOldDictEx);

	// swap block record stock handles
	db_objhandle handle = m_pDrawing->ret_stockhand(DB_SHI_BLKREC_PSPACE);
	m_pDrawing->set_stockhand(pNewLayout->block()->RetHandle(), DB_SHI_BLKREC_PSPACE);
	pOldLayout->setBlock(pNewLayout->block());
	pOldLayout->block()->SetHandle(handle);
	static_cast<db_blocktabrec*>(pOldLayout->block())->set_layoutobjhandle(pOldLayout->RetHandle());

	// swap block stock handles
	static_cast<db_blocktabrec*>(pOldLayout->block())->get_blockllends(&pFirstEnt, &pLastEnt);
	handle = m_pDrawing->ret_stockhand(DB_SHI_BLK_PSPACE);
	m_pDrawing->set_stockhand(pFirstEnt->RetHandle(), DB_SHI_BLK_PSPACE);
	pFirstEnt->SetHandle(handle);
	handle = m_pDrawing->ret_stockhand(DB_SHI_ENDBLK_PSPACE);
	m_pDrawing->set_stockhand(pLastEnt->RetHandle(), DB_SHI_ENDBLK_PSPACE);
	pLastEnt->SetHandle(handle);

	// set paper space layout stock handle
	m_pDrawing->set_stockhand(pNewLayout->RetHandle(), DB_SHI_PSLAYOUT);
	pNewLayout->setCurrent();
	return 1;
}

void CLayoutManager::resetIterator()
{
	m_pDict->get_recllends(&m_pDictIt, NULL);
	ASSERT(m_pDictIt != NULL);
}

bool CLayoutManager::next(const char*& pName, CDbLayout** pLayout, db_blocktabrec** pBlockRec, bool bAllowModel)
{
	if(m_pDictIt == NULL)
		return false;

	CDbLayout* pLayoutIt;
	while(m_pDictIt != NULL)
	{
		pLayoutIt = static_cast<CDbLayout*>(m_pDictIt->hiref->ret_hip(m_pDrawing));
		if(pLayoutIt->ret_deleted())
			m_pDictIt = m_pDictIt->next;
		else
			break;
	}
	if(m_pDictIt == NULL)
		return false;

	if(!bAllowModel && stricmp(m_pDictIt->name, s_model) == 0)
		m_pDictIt = m_pDictIt->next;

	while(m_pDictIt != NULL)
	{
		pLayoutIt = static_cast<CDbLayout*>(m_pDictIt->hiref->ret_hip(m_pDrawing));
		if(pLayoutIt->ret_deleted())
			m_pDictIt = m_pDictIt->next;
		else
			break;
	}
	if(m_pDictIt == NULL)
		return false;

	if(pLayout)
		*pLayout = static_cast<CDbLayout*>(m_pDictIt->hiref->ret_hip(m_pDrawing));
	if(pBlockRec)
		*pBlockRec = static_cast<db_blocktabrec*>((*pLayout)->block());
	pName = m_pDictIt->name;
	m_pDictIt = m_pDictIt->next;
	return true;
}

int CLayoutManager::createLayout(const char* pName, CDbLayout** pLayout)
{
	sds_name dictName;
	dictName[0] = (long)m_pDict;
	dictName[1] = (long)m_pDrawing;

	sds_name layoutBlockName;
	layoutBlockName[0] = (long)createLayoutBlock();
	layoutBlockName[1] = (long)m_pDrawing;
	ASSERT(layoutBlockName[0] != 0);

	struct resbuf* pNewLayout;
	struct resbuf limMin, limMax;
	SDS_getvar(NULL, DB_QLIMMIN, &limMin, m_pDrawing, &SDS_CURCFG, &SDS_CURSES);
	SDS_getvar(NULL, DB_QLIMMAX, &limMax, m_pDrawing, &SDS_CURCFG, &SDS_CURSES);
	sds_point bigPoint = {1.000000000000000E+20, 1.000000000000000E+20, 1.000000000000000E+20};
	sds_point smallPoint = {-1.000000000000000E+20, -1.000000000000000E+20, -1.000000000000000E+20};
	sds_point xAxis = {1.0, 0.0, 0.0};
	sds_point yAxis = {0.0, 1.0, 0.0};
	sds_point ip = {0.0, 0.0, 0.0};
	
	pNewLayout = sds_buildlist(RTDXF0, "LAYOUT",
		102, "{ACAD_REACTORS",
		330, dictName,
		102, "}",
		// plot settings
		2, "None",
		4, "",
		6, "",
		40, 0.0,
		41, 0.0,
		42, 0.0,
		43, 0.0,
		44, 0.0,
		45, 0.0,
		46, 0.0,
		47, 0.0,
		48, 0.0,
		49, 0.0,
		140, 0.0,
		141, 0.0,
		142, 1.0,
		143, 1.0,
		70, 688,
		72, 0,
		73, 0,
		74, 5,
		7, "",
		75, 16,
		147, 1.0,
		148, 0.0,
		149, 0.0,
		// layout
		70, 1,
		71, 1,
		10, limMin.resval.rpoint,
		11, limMax.resval.rpoint,
		12, ip,
		14, bigPoint,
		15, smallPoint,
		146, 0.0,
		13, ip,
		16, xAxis,
		17, yAxis,
		76, 0,
		330,
		layoutBlockName,
		RTNONE);

	sds_name layoutName;
	int result = sds_entmakex(pNewLayout, layoutName);
	if(result != RTNORM)
	{
		ASSERT(FALSE);
		sds_relrb(pNewLayout);
		return 0;
	}

	sds_relrb(pNewLayout);
	addToDictionary(pName, (CDbLayout*)layoutName[0]);
	((db_blocktabrec*)layoutBlockName[0])->set_layoutobjhandle(((CDbLayout*)layoutName[0])->RetHandle());
	if(pLayout)
		*pLayout = (CDbLayout*)layoutName[0];
	return 1;
}

int CLayoutManager::copyLayout(const char* pName, const char* pNameNew)
{
	CDbLayout* pSourceLayout = findLayoutNamed(pName);
	ASSERT(pSourceLayout != NULL);

	sds_name sourceLayoutName;
	sourceLayoutName[0] = (long)pSourceLayout;
	sourceLayoutName[1] = (long)m_pDrawing;

	struct resbuf apps;
	apps.restype = RTSTR; 
	apps.resval.rstring = "*"/*DNT*/; 
	apps.rbnext = NULL;
	struct resbuf* pLayoutList = sds_entgetx(sourceLayoutName, &apps);
	// TODO: remove reactors

	// there can be several 330 groups in the object list
	// block name should be last
	struct resbuf* pBlockName = pLayoutList;
	while(ic_assoc(pBlockName->rbnext, 330) == 0)
		pBlockName = ic_rbassoc;

	sds_name layoutBlockName;
	sds_name layoutSS;
	long nEntities = getLayoutEntities(layoutSS, sourceLayoutName);
	if(nEntities != 0)
	{
		sds_point ip = {0.0, 0.0, 0.0};
		char name[IC_ACADBUF];
		sprintf(name, "*Paper_Space%d", m_pDict->ret_nrecs() - 2);

		if(!cmd_createBlock(layoutBlockName, name, 0, ip, layoutSS, m_pDrawing))
		{
			ASSERT(false);
			sds_ssfree(layoutSS);
			return 0;
		}
		sds_ssfree(layoutSS);
	}
	else
	{
		layoutBlockName[0] = (long)createLayoutBlock();
		layoutBlockName[1] = (long)m_pDrawing;
		ASSERT(layoutBlockName[0] != 0);
	}
	sds_name_set(layoutBlockName, pBlockName->resval.rlname);
	
	sds_name layoutName;
	int result = sds_entmakex(pLayoutList, layoutName);
	if(result != RTNORM)
	{
		ASSERT(FALSE);
		sds_relrb(pLayoutList);
		return 0;
	}

	sds_relrb(pLayoutList);
	CDbLayout* pNewLayout = (CDbLayout*)layoutName[0];
	addToDictionary(pNameNew, pNewLayout);
	((db_blocktabrec*)layoutBlockName[0])->set_layoutobjhandle(pNewLayout->RetHandle());
	return 1;
}

int CLayoutManager::importLayout(sds_name sdsLayoutName)
{
	CDbLayout* pSourceLayout = (CDbLayout*)sdsLayoutName[0];
	db_drawing* pSourceDrawing = (db_drawing*)sdsLayoutName[1];

	ASSERT(pSourceLayout != NULL);
	ASSERT(pSourceDrawing != NULL);

	char name[IC_ACADBUF];
	const char* pSourceLayoutName = pSourceLayout->name(pSourceDrawing);
	if(findLayoutNamed(pSourceLayoutName))
	{
		_snprintf(name, IC_ACADBUF - 1, "%s-%s", defaultName(), pSourceLayoutName);
		name[IC_ACADBUF - 1] = '\0';
	}
	else
		strcpy(name, pSourceLayoutName);

	ASSERT(m_pDrawing == SDS_CURDWG);
	ASSERT(m_pDrawing != pSourceDrawing);

	struct resbuf apps;
	apps.restype = RTSTR; 
	apps.resval.rstring = "*"/*DNT*/; 
	apps.rbnext = NULL;
	struct resbuf* pLayoutList = sds_entgetx(sdsLayoutName, &apps);
	// TODO: remove reactors

	// there can be several 330 groups in the object list
	// block name should be last
	struct resbuf* pBlockName = pLayoutList;
	while(ic_assoc(pBlockName->rbnext, 330) == 0)
		pBlockName = ic_rbassoc;

	// create fake block
	sds_name layoutBlockName;
	layoutBlockName[0] = (long)createLayoutBlock();
	layoutBlockName[1] = (long)m_pDrawing;
	ASSERT(layoutBlockName[0] != 0);
	sds_name_set(layoutBlockName, pBlockName->resval.rlname);
	
	sds_name layoutName;
	int result = sds_entmakex(pLayoutList, layoutName);
	if(result != RTNORM)
	{
		ASSERT(FALSE);
		sds_relrb(pLayoutList);
		return 0;
	}

	sds_relrb(pLayoutList);
	CDbLayout* pNewLayout = (CDbLayout*)layoutName[0];
	addToDictionary(name, pNewLayout);
	((db_blocktabrec*)layoutBlockName[0])->set_layoutobjhandle(pNewLayout->RetHandle());

	// temporary set new layout as current
	CDbLayout* pCurLayout = NULL;
	findActiveLayout(false, &pCurLayout);
	ASSERT(pCurLayout != NULL);
	setCurrentLayout(pCurLayout, pNewLayout);

	// copy source layout entities
	sds_name layoutSS;
	long nEntities = getLayoutEntities(layoutSS, sdsLayoutName);
	if(nEntities != 0)
	{
		CCloneHelper ch(m_pDrawing, pSourceDrawing);
		if(!ch.copy(layoutSS))
		{
			// layout was imported but cloning its entities failed
			ASSERT(false);
			sds_ssfree(layoutSS);
			return 0;
		}
		sds_ssfree(layoutSS);
	}

	// restore current layout
	setCurrentLayout(pNewLayout, pCurLayout);
	return 1;
}

int CLayoutManager::renameLayout(const char* pNameOld, const char* pNameNew)
{
	if(findLayoutNamed(pNameNew))
		return 0;

	CDbLayout* pLayout = findLayoutNamed(pNameOld);
	if(!pLayout)
		return 0;

	sds_name dictName;
	dictName[0] = (long)m_pDict;
	dictName[1] = (long)m_pDrawing;
	
	struct resbuf* pDictLayout = sds_entget(dictName);
	if(pDictLayout == NULL)
	{
		ASSERT(FALSE);
		return 0;
	}
	
	struct resbuf* pCurPair = pDictLayout;
	while(pCurPair->rbnext != NULL && pCurPair->rbnext->restype != 350)
		pCurPair = pCurPair->rbnext;
	ASSERT(pCurPair->rbnext != NULL);
	
	while(pCurPair->rbnext != NULL && pCurPair->rbnext->restype == 350)
	{
		ASSERT(pCurPair->restype == 3);
		if(stricmp(pNameOld, pCurPair->resval.rstring) == 0 &&
			pLayout == (CDbLayout*)pCurPair->rbnext->resval.rlname[0])
		{
			char* pName = pCurPair->resval.rstring;
			pCurPair->resval.rstring = const_cast<char*>(pNameNew);
			int result = sds_entmod(pDictLayout);
			pCurPair->resval.rstring = pName;
			if(result != RTNORM)
			{
				ASSERT(FALSE);
				sds_relrb(pDictLayout);
				return 0;
			}
			break;
		}
		pCurPair = pCurPair->rbnext;
		pCurPair = pCurPair->rbnext;
		ASSERT(pCurPair != NULL);
	}
	sds_relrb(pDictLayout);
	return 1;
}

int CLayoutManager::deleteLayout(const char* pName)
{
	if(stricmp(pName, s_model) == 0)
		return 0;

	CDbLayout* pCurLayout = NULL;
	CDbLayout* pLayoutToDel = NULL;
	CDbLayout* pNewCurLayout = NULL;
	CDbLayout* pLayout;
	db_dictionaryrec* pDictIt;
	m_pDict->get_recllends(&pDictIt, NULL);
	for(; pDictIt != NULL; pDictIt = pDictIt->next)
	{
		pLayout = static_cast<CDbLayout*>(pDictIt->hiref->ret_hip(m_pDrawing));
		if(pLayout->ret_deleted())
			continue;
		if(stricmp(pDictIt->name, s_model) == 0)
			continue;
		if(pLayout->isCurrent())
			pCurLayout = pLayout;
		if(stricmp(pDictIt->name, pName) == 0)
			pLayoutToDel = pLayout;
		if(pNewCurLayout == NULL && pLayout != pLayoutToDel)
			pNewCurLayout = pLayout;
		if(pCurLayout && pNewCurLayout && pLayoutToDel)
			break;
	}
	if(pCurLayout == pLayoutToDel)
	{
		if(pNewCurLayout == NULL)
		{
			// deletion of last layout is requested
			if(!createLayout(PAPERSPACE_LAYOUT1, &pNewCurLayout))
				return 0;
		}
		setCurrentLayout(pLayoutToDel, pNewCurLayout);

		struct resbuf tm;
		SDS_getvar(NULL, DB_QTILEMODE, &tm, m_pDrawing, &SDS_CURCFG, &SDS_CURSES);
		// create paper space viewport in empty layout
		if(tm.resval.rint == 0)
			if(!m_pDrawing->ret_ntabrecs(DB_VXTAB, 0))
				cmd_pspace();
	}

	sds_name sdsName;
	sdsName[1] = (long)m_pDrawing;
	sdsName[0] = (long)pLayoutToDel;
	sds_entdel(sdsName);

	sdsName[0] = (long)pLayoutToDel->block();
	sds_entdel(sdsName);
	return 1;
}

int CLayoutManager::setCurrentLayout(const char* pName)
{
	// model layout is special case
	struct resbuf tm;
	SDS_getvar(NULL, DB_QTILEMODE, &tm, m_pDrawing, &SDS_CURCFG, &SDS_CURSES);
	if(stricmp(pName, s_model) == 0)
	{
		if(tm.resval.rint == 0)
		{
			tm.resval.rint = 1;
			sds_setvar("TILEMODE", &tm);
		}
		return 1;
	}

	CDbLayout* pCurLayout;
	if(stricmp(pName, findActiveLayout(false, &pCurLayout)) == 0)
	{
		if(tm.resval.rint == 1)
		{
			tm.resval.rint = 0;
			sds_setvar("TILEMODE", &tm);
			return 1;
		}
		return 0;
	}
	ASSERT(pCurLayout != NULL);

	CDbLayout* pNewCurLayout = findLayoutNamed(pName);
	if(pNewCurLayout == NULL)
		return 0;

	setCurrentLayout(pCurLayout, pNewCurLayout);

	if(tm.resval.rint == 1)
	{
		tm.resval.rint = 0;
		sds_setvar("TILEMODE", &tm);
	}
	// calliing the folling function when switching between layouts to set active viewport
	// if there is one and create a viewport for empty layout. BUG #78949 - SWH, 9/25/2004
	//
	cmd_pspace();
	return 1;
}

const char* CLayoutManager::findActiveLayout(bool bAllowModel, CDbLayout** pLayout) const
{
	struct resbuf tm;
	if(bAllowModel)
		SDS_getvar(NULL, DB_QTILEMODE, &tm, m_pDrawing, &SDS_CURCFG, &SDS_CURSES);
	else
		tm.resval.rint = 0;

	db_dictionaryrec* pCurRec;
	m_pDict->get_recllends(&pCurRec, NULL);
	while(pCurRec != NULL)
	{
		if(!static_cast<CDbLayout*>(pCurRec->hiref->ret_hip(m_pDrawing))->ret_deleted())
			if(!tm.resval.rint)
			{
				// paper space layout is active
				if(static_cast<CDbLayout*>(pCurRec->hiref->ret_hip(m_pDrawing))->isCurrent())
					break;
			}
			else
			{
				// model space layout is active
				if(static_cast<CDbLayout*>(pCurRec->hiref->ret_hip(m_pDrawing))->block() == static_cast<CDbLayout*>(pCurRec->hiref->ret_hip(m_pDrawing)))
				{
					ASSERT(stricmp(pCurRec->name, s_model) == 0);
					break;
				}

			}
			pCurRec = pCurRec->next;
	}
	ASSERT(pCurRec != NULL);
	if(pLayout)
		*pLayout = static_cast<CDbLayout*>(pCurRec->hiref->ret_hip(m_pDrawing));
	return pCurRec->name;
}

CDbLayout* CLayoutManager::findLayoutNamed(const char* pName) const
{
	CDbLayout* pLayout = static_cast<CDbLayout*>(m_pDrawing->dictsearch(m_pDict, const_cast<char*>(pName), 0));
	ASSERT(pLayout == NULL || pLayout->ret_type() == DB_LAYOUT);

	return pLayout;
}

const char* CLayoutManager::defaultName()
{
	db_dictionaryrec* pFirstRec = NULL;
	m_pDict->get_recllends(&pFirstRec, NULL);
	int freeIndex = 1;
	int nameLen = 0;
	db_dictionaryrec* pCurRec = NULL;
	while(true)
	{
		sprintf(s_defaultName, "Layout%d", freeIndex);
		nameLen = strlen(s_defaultName);
		pCurRec = pFirstRec;
		while(pCurRec != NULL)
		{
			if(!static_cast<CDbLayout*>(pCurRec->hiref->ret_hip(m_pDrawing))->ret_deleted())
				if(strnicmp(s_defaultName, pCurRec->name, nameLen) == 0)
				{
					++freeIndex;
					nameLen = 0;
					break;
				}
			pCurRec = pCurRec->next;
		}
		if(nameLen != 0)
			break;
	}
	return s_defaultName;
}

int cmd_setUpCTAB()
{
	char layoutName[IC_ACADBUF];

	CLayoutManager lm(SDS_CURDWG);
	strncpy(layoutName, lm.findActiveLayout(true), IC_ACADBUF - 1);

	struct resbuf ctab;
	ctab.restype = RTSTR;
	ctab.resval.rstring = layoutName;
	SDS_setvar(NULL, DB_QCTAB, &ctab, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES, 0);
	return 1;
}

int cmd_listLayouts();
void cmd_OnOpenOK(HWND hDlg);
extern char* cmd_DwgSavePath;

short cmd_layout()
{
	if (cmd_istransparent() == RTERROR)
		return RTERROR;

	int result = RTNORM;
	char answer[IC_ACADBUF];
	char name[IC_ACADBUF];
	char nameAuto[IC_ACADBUF];
	sds_name layout = { 0, 0 };

	// Get the path to the template files
	CMainWindow *mainWnd = SDS_CMainWindow;
	CString csTemplatePath = cmd_DwgSavePath;
	if (mainWnd && mainWnd->m_paths && mainWnd->m_paths->m_templatesPath)
	{
		// There may be mulitple template paths so just grab the first one
		FilePath cFirstPath(mainWnd->m_paths->m_templatesPath);
		csTemplatePath.Format("%s%s", cFirstPath.GetFirstPath(), "\\");
	}

	CLayoutManager lm(SDS_CURDWG);

	if(sds_initget(0, ResourceString(IDC_LAYOUT_OPTIONS, "List Set New Copy Delete Rename SAve Template ~_? ~_Set ~_New ~_Copy ~_Delete ~_Rename ~_SAve ~_Template")) != RTNORM)
		return RTERROR;

	result = sds_getkword(ResourceString(IDC_LAYOUT_OPTIONSPROMPT, "\nEnter layout option [Copy/Delete/New/Rename/Set/SAve/Template/? to list] <Set>: "), answer);
	if(result != RTNORM)
	{
		if(result == RTNONE)
			strcpy(answer, "Set"/*DNT*/);
		else
			return RTNORM;
	}

	struct resbuf filedia;
	SDS_getvar(NULL, DB_QFILEDIA, &filedia, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
	if(stricmp(answer, "Template"/*DNT*/) == 0)
	{
		struct resbuf filename; 
		if(filedia.resval.rint && GetActiveCommandQueue()->IsEmpty() && GetMenuQueue()->IsEmpty() && !lsp_cmdhasmore) 
		{
			result = SDS_GetFiled(ResourceString(IDC_LAYOUT_TEMPLATEFILE, "Select Template From File"), 
				csTemplatePath, 
				ResourceString(IDC_LAYOUT_FILEEXTENSIONS, "Template Drawing|dwt,Standard Drawing File|dwg,Drawing Exchange Format|dxf"), 
				4, 
				&filename, 
				MAKEINTRESOURCE(IDD_OPENSAVE_PREVIEW), 
				cmd_OnOpenOK);
			if(result != RTNORM)
				return RTCAN;
			ASSERT(filename.restype == RTSTR);
			strcpy(name, filename.resval.rstring);
			delete filename.resval.rstring;
		}
		else
		{
			result = sds_getstring(1, ResourceString(IDC_LAYOUT_TEMPLATEFILE2, "Enter file name: "), name);
			if(result != RTNORM)
				return RTCAN;
			if(name[0] == '~')
			{
				result = SDS_GetFiled(ResourceString(IDC_LAYOUT_TEMPLATEFILE, "Select Template From File"), 
					csTemplatePath, 
					ResourceString(IDC_LAYOUT_FILEEXTENSIONS, "Template Drawing|dwt,Standard Drawing File|dwg,Drawing Exchange Format|dxf"), 
					4, 
					&filename, 
					MAKEINTRESOURCE(IDD_OPENSAVE_PREVIEW), 
					cmd_OnOpenOK);
				if(result != RTNORM)
					return RTCAN;
				ASSERT(filename.restype == RTSTR);
				strcpy(name, filename.resval.rstring);
				delete filename.resval.rstring;
			}
		}
		ASSERT(name[0] != '\0');
		ChangeSlashToBackslashAndRemovePipe(name);
		// check for existence and read access
		if(access(name, 0x04) == 0)
		{
			SDS_CMainWindow->m_strWndAction = name;
			SDS_CMainWindow->m_pvWndAction = (void*)SDS_CURDWG;
			ExecuteUIAction(ICAD_WNDACTION_LAYOUTTEMPL);
			ExecuteUIAction(ICAD_WNDACTION_LAYOUT);
			cmd_redrawall();
		}
		else
			sds_printf(ResourceString(IDC_LAYOUT_FILENOTFOUND, "\n%s: Can not find file"), name);
		return RTNORM;	
	}
	if(stricmp(answer, "?"/*DNT*/) == 0)
	{
		cmd_listLayouts();
		return RTNORM;
	}

	if(stricmp(answer, "New"/*DNT*/) == 0)
	{
		strcpy(nameAuto, lm.defaultName());
		sprintf(name, "%s<%s>: ", ResourceString(IDC_LAYOUT_ENTERNAME, "\nEnter a layout name"), nameAuto);
		result = sds_getstring(1, name, name);
		if(result != RTNORM)
			return RTNORM;
		if(name[0] == '\0')
			strcpy(name, nameAuto);

		ic_trim(name, "be");
		if(!ic_isvalidname(name))
		{
			sds_printf("\n%s", ResourceString(IDC_LAYOUT_NOTVALIDNAME, "This is not valid name for layout"));
			return RTNORM;
		}
		if(lm.findLayoutNamed(name))
		{
			sds_printf("\n%s", ResourceString(IDC_LAYOUT_ALREADYEXIST, "Layout with such name already exists"));
			return RTNORM;
		}
		if(!lm.createLayout(name))
			sds_printf("\n%s", ResourceString(IDC_LAYOUT_FAILED, "Operation failed."));

		ExecuteUIAction(ICAD_WNDACTION_LAYOUT);
	}
	else
	{
		strcpy(nameAuto, lm.findActiveLayout(false));
		sprintf(name, "%s<%s>: ", ResourceString(IDC_LAYOUT_ENTERNAME, "\nEnter a layout name"), nameAuto);
		result = sds_getstring(1, name, name);
		if(result != RTNORM)
			return RTNORM;
		if(name[0] == '\0')
			strcpy(name, nameAuto);

		ic_trim(name, "be");
		if(!ic_isvalidname(name))
		{
			sds_printf("\n%s", ResourceString(IDC_LAYOUT_NOTVALIDNAME, "This is not valid name for layout"));
			return RTNORM;
		}
		if(!lm.findLayoutNamed(name))
		{
			sds_printf("\n%s", ResourceString(IDC_LAYOUT_NOTFOUND, "Layout not found"));
			return RTNORM;
		}
		if(stricmp(answer, "Delete"/*DNT*/) == 0)
		{
			if(stricmp(name, ResourceString(IDC_LAYOUT_MODEL, "Model")) == 0)
			{
				sds_printf("\n%s", ResourceString(IDC_LAYOUT_CANTDELETE, "Can not delete Model layout"));
				return RTNORM;
			}
			if(!lm.deleteLayout(name))
				sds_printf("\n%s", ResourceString(IDC_LAYOUT_FAILED, "Operation failed."));
			ExecuteUIAction(ICAD_WNDACTION_LAYOUT);
			cmd_redraw();
		}
		else
			if(stricmp(answer, "Set"/*DNT*/) == 0)
			{
				struct resbuf ctab;
				ctab.restype = RTSTR;
				ctab.resval.rstring = name;
				sds_setvar("CTAB", &ctab );
#if 0
				sds_getvar("TILEMODE", &tm);
				if(stricmp(name, ResourceString(IDC_LAYOUT_MODEL, "Model")) == 0)
				{
					if(tm.resval.rint == 0)
					{
						// just switch to model space
						tm.resval.rint = 1;
						sds_setvar("TILEMODE", &tm);
					}
				}
				else
				{
					if(tm.resval.rint == 1)
					{
						// from model space, switch to paper space at first
						tm.resval.rint = 0;
						sds_setvar("TILEMODE", &tm);
					}
					// set required layout as current
					cmd_setLayout(layout);
					// create paper space viewport in empty layout
					if(!pDrawing->ret_ntabrecs(DB_VXTAB, 0))
						cmd_pspace();
				}
				cmd_regenall();
#endif
			}
			else
			{
				if(stricmp(answer, "Rename"/*DNT*/) == 0)
				{
					sprintf(answer, "%s: ", ResourceString(IDC_LAYOUT_ENTERNAME2, "\nEnter new layout name"), nameAuto);
					if(sds_getstring(1, answer, answer) != RTNORM)
						return RTNORM;
					ic_trim(answer,"be");
					if(!ic_isvalidname(answer))
					{
						sds_printf("\n%s", ResourceString(IDC_LAYOUT_NOTVALIDNAME, "This is not valid name for layout"));
						return RTNORM;
					}
					if(lm.findLayoutNamed(answer))
					{
						sds_printf("\n%s", ResourceString(IDC_LAYOUT_ALREADYEXIST, "Layout with such name already exists"));
						return RTNORM;
					}
					if(!lm.renameLayout(name, answer))
						sds_printf("\n%s", ResourceString(IDC_LAYOUT_FAILED, "Operation failed."));
					ExecuteUIAction(ICAD_WNDACTION_LAYOUT);
				}
				else
				{
					if(stricmp(answer, "Copy"/*DNT*/) == 0)
					{
						int freeIndex = 2;
						sprintf(nameAuto, "%s (%d)", name, freeIndex);
						while(lm.findLayoutNamed(nameAuto))
						{
							++freeIndex;
							sprintf(nameAuto, "%s (%d)", name, freeIndex);
						}
						sprintf(answer, "%s<%s>: ", ResourceString(IDC_LAYOUT_ENTERNAME2, "\nEnter new layout name"), nameAuto);
						if(sds_getstring(1, answer, answer) != RTNORM)
							return RTNORM;
						if(answer[0] == '\0')
							strcpy(answer, nameAuto);
						ic_trim(answer, "be");
						if(!ic_isvalidname(answer))
						{
							sds_printf("\n%s", ResourceString(IDC_LAYOUT_NOTVALIDNAME, "This is not valid name for layout"));
							return RTNORM;
						}
						if(lm.findLayoutNamed(answer))
						{
							sds_printf("\n%s", ResourceString(IDC_LAYOUT_ALREADYEXIST, "Layout with such name already exists"));
							return RTNORM;
						}
						if(!lm.copyLayout(name, answer))
							sds_printf("\n%s", ResourceString(IDC_LAYOUT_FAILED, "Operation failed."));
						ExecuteUIAction(ICAD_WNDACTION_LAYOUT);
					}
					else
						if(stricmp(answer, "SAve"/*DNT*/) == 0)
						{
							struct resbuf filename;
							if(filedia.resval.rint && GetActiveCommandQueue()->IsEmpty() && GetMenuQueue()->IsEmpty() && !lsp_cmdhasmore) 
							{
								result = SDS_GetFiled(ResourceString(IDC_LAYOUT_SAVEFILE, "Create drawing file"), 
									csTemplatePath, 
									ResourceString(IDC_LAYOUT_FILEEXTENSIONS, "Template Drawing|dwt,Standard Drawing File|dwg,Drawing Exchange Format|dxf"), 
									513, 
									&filename, 
									MAKEINTRESOURCE(IDD_OPENSAVE_PREVIEW), 
									cmd_OnOpenOK);
								if(result != RTNORM)
									return RTCAN;
								ASSERT(filename.restype == RTSTR);

								strcpy(nameAuto, filename.resval.rstring);
								delete filename.resval.rstring;
								ChangeSlashToBackslashAndRemovePipe(nameAuto);
							}
							else
							{
								result = sds_getstring(1, ResourceString(IDC_LAYOUT_SAVEFILE2, "Enter name of output file: "), nameAuto);
								if(result != RTNORM)
									return RTCAN;
								if(nameAuto[0] == '~')
								{
									result = SDS_GetFiled(ResourceString(IDC_LAYOUT_SAVEFILE, "Create drawing file"), 
										csTemplatePath, 
										ResourceString(IDC_LAYOUT_FILEEXTENSIONS, "Template Drawing|dwt,Standard Drawing File|dwg,Drawing Exchange Format|dxf"), 
										513, 
										&filename, 
										MAKEINTRESOURCE(IDD_OPENSAVE_PREVIEW), 
										cmd_OnOpenOK);
									if(result != RTNORM)
										return RTCAN;
									ASSERT(filename.restype == RTSTR);

									strcpy(nameAuto, filename.resval.rstring);
									delete filename.resval.rstring;
									ChangeSlashToBackslashAndRemovePipe(nameAuto);
								}
								else
								{
									ChangeSlashToBackslashAndRemovePipe(nameAuto);

									// grab the expert system variable
									struct resbuf expert;
									int nExpert = 0;
									if (SDS_getvar(NULL, DB_QEXPERT, &expert, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES) == RTNORM)
										nExpert = expert.resval.rint;

									// Check if the file exists
									if( (access(nameAuto, 0x00) == 0) && (nExpert<2) )
									{
										if(sds_initget(0, ResourceString(IDC_LAYOUT_YESNO, "Yes No ~_Yes ~_No")) != RTNORM) 
											return RTERROR;
										result = sds_getkword(ResourceString(IDC_LAYOUT_REPLACEFILE, "\nA drawing with this name already exists. Replace it? <N> "), answer);
										if(result != RTNORM)
										{
											if(result == RTNONE)
												strcpy(answer, "No");
											else
												return RTERROR;
										}
										if(strisame(answer, "No"/*DNT*/)) 
											return RTNORM;
									}
								}
							}
							CDbLayout* pSourceLayout = lm.findLayoutNamed(name);
							layout[0] = (long)pSourceLayout;
							layout[1] = (long)SDS_CURDWG;
					
							sds_name layoutSS;
							CLayoutManager::getLayoutEntities(layoutSS, layout);
							
							CCloneHelper ch(IC_DWG, IC_ACAD2000);
							if(!ch.saveLayout(layout, layoutSS, nameAuto))
								sds_printf("\n%s", ResourceString(IDC_LAYOUT_FAILED, "Operation failed."));

							sds_ssfree(layoutSS);
						}
						else
							ASSERT(FALSE);
				} // Rename
			} // Set
	} // New
	return RTNORM;
}

int cmd_listLayouts()
{
	sds_textscr();

	int scroll = 0;
	char scrollAnswer[IC_ACADBUF];
	const char* pName;
	CDbLayout* pLayout;
	db_blocktabrec* pBlock;
	CLayoutManager lm(SDS_CURDWG);
	lm.resetIterator();
	while(lm.next(pName, &pLayout, &pBlock, false))
	{
		sds_printf("\n%s: %s", ResourceString(IDC_LAYOUT_NAME, "Layout name:"), pName);
		if(pLayout->isCurrent())
			strncpy(scrollAnswer, "*Paper_Space", IC_ACADBUF-1);
		else
		{
			// This should start the *Paper_space block, not *Paper_Space0. So we'll
			// get the name from the block itself. List displays the wrong info if not.
			char* tmpStr = NULL;
			pBlock->get_2(&tmpStr);

			strncpy(scrollAnswer, tmpStr, IC_ACADBUF - 1);
		}
		sds_printf("\n%s: %s", ResourceString(IDC_LAYOUT_BLOCKNAME, "Block name:"), scrollAnswer);

		if(!scroll)
		{
			sds_initget(0, ResourceString(IDC_LAYOUT_SCROLL, "Scroll ~_Scroll"));
			scroll = sds_getkword(ResourceString(IDC_LAYOUT_SCROLLPROMPT, "\nScroll/<ENTER to show next group>: "), scrollAnswer);
			if(scroll == RTNORM)
				scroll = 1;
			else
				if(scroll == RTNONE)
					scroll = 0;
				else
					break;
		}
	}
	return 1;
}

int cmd_getCurrentLayout(sds_name layout)
{
	CDbLayout* pCurLayout;
	CLayoutManager lm(SDS_CURDWG);
	lm.findActiveLayout(true, &pCurLayout);
	layout[0] = (long)pCurLayout;
	layout[1] = (long)SDS_CURDWG;
	return 1;
}

short cmd_pagesetup()
{
	cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0);
	SDS_FreeEventList(0);
	return(RTNORM);
}

short cmd_layoutwizard()
{
	cmd_ErrorPrompt(CMD_ERR_UNSUPPORTED,0);
	SDS_FreeEventList(0);
	return(RTNORM);
}
