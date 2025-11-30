#include "db.h"
#include "DbGroup.h"
#include "objects.h"
#include <map>

CDbGroup::CDbGroup(void) : CDbObject(DB_GROUP)
{
    m_deleted = 0;
    m_flags = 0;
	m_pDescription = NULL;
	m_pEntitiesList[0] = m_pEntitiesList[1] = NULL;
}

CDbGroup::CDbGroup(const CDbGroup& group): CDbObject(group)
{
    m_deleted = group.m_deleted;
    m_flags = group.m_flags;
    m_pDescription = NULL; 
	db_astrncpy(&m_pDescription, group.m_pDescription, -1);

    m_pEntitiesList[0] = m_pEntitiesList[1] = NULL;
    db_hireflink *pCurEntity;
	for(pCurEntity = group.m_pEntitiesList[0]; pCurEntity != NULL; pCurEntity = pCurEntity->next)
		addEntity(new db_hireflink(*pCurEntity)); 
}

CDbGroup::~CDbGroup() 
{ 
	delete[] m_pDescription; 
	db_hireflink::delll(m_pEntitiesList[0]); 
}

int CDbGroup::entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp)
{
	assert(begpp != NULL);
	assert(endpp != NULL);

	*begpp = *endpp = db_newrb(300, 'S', m_pDescription);

	int flag = (int)isUnnamed();
	(*endpp)->rbnext = db_newrb(70, 'I', &flag);
	*endpp = (*endpp)->rbnext;

	flag = (int)isSelectable();
	(*endpp)->rbnext = db_newrb(71, 'I', &flag);
	*endpp = (*endpp)->rbnext;

	for(db_hireflink* link = m_pEntitiesList[0]; link; link = link->next)
	{
		db_handitem* hip = link->ret_hip(dp);
		if(hip->ret_deleted())
			continue;
		sds_name entityName;
		entityName[0]=(long)hip;
		entityName[1]=(long)dp;
		struct resbuf* pCurEntity = db_newrb(340, 'N', entityName);
		(*endpp)->rbnext = pCurEntity;
		*endpp = pCurEntity;
	}

	return 0;
}

int CDbGroup::entmod(struct resbuf *modll, db_drawing *dp)
{
	CDbObject::entmod(modll, dp);

	if(ic_assoc(modll, 300) == 0)
		setDescription(ic_rbassoc->resval.rstring);
	else
		setDescription(NULL);

	std::map<db_objhandle, bool> items;
	std::map<db_objhandle, bool>::iterator it;
	for(db_hireflink* link = m_pEntitiesList[0]; link != NULL; link = link->next)
		items[link->hand] = false;

	if(ic_assoc(modll, 340) == 0)
	{
		while(ic_rbassoc != NULL && ic_rbassoc->restype == 340)
		{
			db_handitem* hip = ((db_handitem*)ic_rbassoc->resval.rlname[0]);
			it = items.find(hip->RetHandle());
			if(it != items.end())
			{
				it->second = true;
			}
			else
			{
				addEntity(new db_hireflink(DB_HARD_POINTER, hip->RetHandle(), hip));
				hip->addReactorAllowDups(RetHandle(), this, ReactorItem::SOFT_POINTER);
			}
			ic_rbassoc = ic_rbassoc->rbnext;
		}
	}
	// remove items that are not in the list
	db_hireflink* prev = NULL;
	for(link = m_pEntitiesList[0]; link != NULL; )
	{
		if((it = items.find(link->hand)) != items.end() && it->second == false &&
			!link->ret_hip(dp)->ret_deleted()) //NS: important, do not remove references to elements??
		{
			if(prev == NULL)
			{
				ASSERT(m_pEntitiesList[0] == link);
				m_pEntitiesList[0] = link->next;
			}
			else
			{
				prev->next = link->next;
			}
			if(m_pEntitiesList[1] == link)
				m_pEntitiesList[1] = prev;
			
			db_hireflink* tmp = link->next;
			link->ret_hip(dp)->removeReactor(this->RetHandle());
			delete link;
			link = tmp;
		}
		else
		{
			prev = link;
			link = link->next;
		}
	}
	return 0;
}

void CDbGroup::getEntitiesList(db_hireflink*& pListBegin, db_hireflink*& pListEnd) const
{
	pListBegin = m_pEntitiesList[0];
	pListEnd = m_pEntitiesList[1];
}

void CDbGroup::setEntitiesList(db_hireflink* pListBegin, db_hireflink* pListEnd)
{
	m_pEntitiesList[0] = pListBegin;
	m_pEntitiesList[1] = pListEnd;
}

void CDbGroup::addEntity(db_hireflink* pEntityLink) 
{
	assert(pEntityLink != NULL);
	if(findEntity(&pEntityLink->hand, NULL))
		return;

	if(m_pEntitiesList[0] == NULL) 
		m_pEntitiesList[0] = pEntityLink;
	else 
		m_pEntitiesList[1]->next = pEntityLink;
	m_pEntitiesList[1] = pEntityLink;
}

bool CDbGroup::findEntity(const db_objhandle* pEntity, int* pIndex) const
{
	assert(pEntity != NULL);

	if(pIndex != NULL)
		*pIndex = 0;
	db_hireflink* pCurEntity = m_pEntitiesList[0];
	while(pCurEntity != NULL)
	{
		if(pCurEntity->hand == *pEntity)
			return true;
		if(pIndex != NULL)
			++(*pIndex);
		pCurEntity = pCurEntity->next;
	}
	return false;
}

int CDbGroup::nEntities() const 
{
	int n = 0;
	db_hireflink *pCurEntity;

	for(pCurEntity = m_pEntitiesList[0]; pCurEntity != NULL; pCurEntity = pCurEntity->next)
		++n;
	return n;
}

const char* CDbGroup::name(db_drawing* pDrawing)/*const*/
{
	db_dictionary* pDict = (db_dictionary*)pDrawing->dictsearch(pDrawing->namedobjdict(),
		"ACAD_GROUP" /*DNT*/, 0);
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

void CDbGroup::setName(char* pName, db_drawing* pDrawing)
{
	db_dictionary* pDict = (db_dictionary*)pDrawing->dictsearch(pDrawing->namedobjdict(),
		"ACAD_GROUP" /*DNT*/, 0);
	db_dictionaryrec* pCurRec;
	pDict->get_recllends(&pCurRec, NULL);
	while(pCurRec != NULL)
	{
		if(pCurRec->hiref->ret_hip(pDrawing) == this)
			break;
		pCurRec = pCurRec->next;
	}
	assert(pCurRec != NULL);
	db_astrncpy(&pCurRec->name, pName, -1);
}

void CDbGroup::setUnnamed(bool bUnnamed)
{
	if(bUnnamed)
		m_flags |= eUNNAMED;
	else
		m_flags &= ~eUNNAMED;
}

void CDbGroup::setSelectable(bool bSelectable)
{
	if(bSelectable)
		m_flags |= eSELECTABLE;
	else
		m_flags &= ~eSELECTABLE;
}