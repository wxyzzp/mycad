#ifndef __DbGroupH__
#define __DbGroupH__

typedef db_handitem CDbObject;
/*--------------------------------------------------------------------------------*//**
@author Denis Petrov

@description Class for representing GROUP object.
*//*----------------------------------------------------------------------------------*/
class DB_CLASS CDbGroup : public CDbObject 
{
	friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);
	
	enum EFlags
	{
		eUNNAMED = 0x0001,
		eSELECTABLE = 0x0002
	};

public:
    CDbGroup();
	CDbGroup(const CDbGroup& group);
	virtual ~CDbGroup();

	virtual int entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp);
	virtual int entmod(struct resbuf *modll, db_drawing *dp);

	int  ret_deleted(void)  { return m_deleted; }
    void set_deleted(int p) { m_deleted = (char)(p!=0); }

	void getEntitiesList(db_hireflink*& pListBegin, db_hireflink*& pListEnd) const;
	void setEntitiesList(db_hireflink* pListBegin, db_hireflink* pListEnd);
	void addEntity(db_hireflink* pEntityLink);
	bool findEntity(const db_objhandle* pEntity, int* pIndex) const;

	int nEntities() const;

	bool isUnnamed() const { if(m_flags & eUNNAMED) return true; else return false; }
	void setUnnamed(bool bUnnamed);

	bool isSelectable() const { if(m_flags & eSELECTABLE) return true; else return false; }
	void setSelectable(bool bSelectable);

	const char* description() const { return m_pDescription; }
	void setDescription(char* pDesc) { db_astrncpy(&m_pDescription, (pDesc == NULL) ? db_str_quotequote : pDesc, -1); }

	const char* name(db_drawing* pDrawing)/* const*/;
	void setName(char* pName, db_drawing* pDrawing);

    bool get_70(int *p) { *p = int(isUnnamed()); return true; }
    bool get_71(int *p)    { *p = int(isSelectable()); return true; }
    bool get_300(char **p) { *p = m_pDescription; return true; }

    bool set_70(int p) { setUnnamed(p != 0 ? true : false); return true; }
    bool set_71(int p) { setSelectable(p != 0 ? true : false); return true; }
    bool set_300(char *p) { setDescription(p); return true; }

private:
    char m_deleted;
	// 70, 71
    char m_flags;
	// 300
    char* m_pDescription;
	// 340  list of entities in the group
    db_hireflink* m_pEntitiesList[2];
};

#endif
