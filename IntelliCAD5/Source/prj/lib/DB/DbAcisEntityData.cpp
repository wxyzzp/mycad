#include "DbAcisEntityData.h"
#include <string.h>
#include <assert.h>
#include "dbdefines.h"

CDbAcisEntityData::CDbAcisEntityData():
m_type(eUndefined), m_pSat(new icl::string)
{}

CDbAcisEntityData::CDbAcisEntityData(const CDbAcisEntityData& obj):
m_pSat(new icl::string(obj.getSat())),
m_type(obj.getType())
{}

CDbAcisEntityData::CDbAcisEntityData
(
const icl::string& sat, 
CDbAcisEntityData::EType type
):
m_pSat(new icl::string(sat)),
m_type(type)
{}

CDbAcisEntityData::CDbAcisEntityData
(
const icl::string& sat, 
int dbt, 
int
):
m_pSat(new icl::string(sat))
{
	setDbtype(dbt);
}

CDbAcisEntityData::CDbAcisEntityData
(
const icl::string& sat, 
const char* dbt, 
int
):
m_pSat(new icl::string(sat))
{
	setDbtype(dbt);
}

const CDbAcisEntityData& CDbAcisEntityData::operator = (const CDbAcisEntityData& obj)
{
	if (this != &obj) {
		*m_pSat = obj.getSat();
		m_type = obj.getType();
	}
	return *this;
}

void CDbAcisEntityData::clear()
{
	m_pSat->erase();
	m_type = eUndefined;
}

const icl::string& CDbAcisEntityData::getSat() const
{
	return *m_pSat;
}

void CDbAcisEntityData::setSat(const icl::string& s)
{
	*m_pSat = s;
}

CDbAcisEntityData::EType CDbAcisEntityData::getType() const
{
	return m_type;
}
void CDbAcisEntityData::setType(const EType& t)
{
	m_type = t;
}

int CDbAcisEntityData::getDbtype() const
{
	switch (m_type)
	{
	case CDbAcisEntityData::e3DSolid:
		return DB_3DSOLID;
	case CDbAcisEntityData::eRegion:
		return DB_REGION;
	case CDbAcisEntityData::eBody:
		return DB_BODY;
	default:
		assert(false);
		return -1;
	}
}

int CDbAcisEntityData::setDbtype(int t)
{
	switch (t)
	{
	case DB_3DSOLID:
		m_type = e3DSolid;
		break;
	case DB_REGION:
		m_type = eRegion;
		break;
	case DB_BODY:
		m_type = eBody;
		break;
	default:
		m_type = eUndefined;
		return 0;
	}
	return 1;
}

int CDbAcisEntityData::setDbtype(const char* type)
{
	if (!stricmp(type, db_hitype2str(DB_3DSOLID)))
	{
		m_type = e3DSolid;
	}
	else if (!stricmp(type, db_hitype2str(DB_REGION)))
	{
		m_type = eRegion;
	}
	else if (!stricmp(type, db_hitype2str(DB_BODY)))
	{
		m_type = eBody;
	}
	else
	{
		assert(false);
		return 0;
	}
	return 1;
}

CDbAcisEntityData::EType operator * 
(
CDbAcisEntityData::EType t1,
CDbAcisEntityData::EType t2
)
{
	if (t1 == CDbAcisEntityData::eUndefined || t2 == CDbAcisEntityData::eUndefined)
	{
		return CDbAcisEntityData::eUndefined;
	}
	if (t1 == CDbAcisEntityData::eBody || t2 == CDbAcisEntityData::eBody)
	{
		return CDbAcisEntityData::eBody;
	}
	if (t1 == CDbAcisEntityData::eRegion || t2 == CDbAcisEntityData::eRegion)
	{
		return CDbAcisEntityData::eRegion;
	}
	if (t1 == CDbAcisEntityData::e3DSolid && t2 == CDbAcisEntityData::e3DSolid)
	{
		return CDbAcisEntityData::e3DSolid;
	}

	assert(false);
	return CDbAcisEntityData::eUndefined;
}

CDbAcisEntityData::EType operator + 
(
CDbAcisEntityData::EType t1,
CDbAcisEntityData::EType t2
)
{
	if (t1 == CDbAcisEntityData::eUndefined)
	{
		return t2;
	}
	if (t2 == CDbAcisEntityData::eUndefined)
	{
		return t1;
	}
	if (t1 == CDbAcisEntityData::eBody || t2 == CDbAcisEntityData::eBody)
	{
		return CDbAcisEntityData::eBody;
	}
	if ((t1 == CDbAcisEntityData::e3DSolid && t2 == CDbAcisEntityData::eRegion) ||
		(t2 == CDbAcisEntityData::e3DSolid && t1 == CDbAcisEntityData::eRegion))
	{
		return CDbAcisEntityData::eBody;
	}
	if (t1 == CDbAcisEntityData::eRegion && t2 == CDbAcisEntityData::eRegion)
	{
		return CDbAcisEntityData::eRegion;
	}
	if (t2 == CDbAcisEntityData::e3DSolid && t2 == CDbAcisEntityData::e3DSolid)
	{
		return CDbAcisEntityData::e3DSolid;
	}

	assert(false);
	return CDbAcisEntityData::eUndefined;
}

CDbAcisEntityData::EType operator - 
(
CDbAcisEntityData::EType t1,
CDbAcisEntityData::EType t2
)
{
	if (t1 == CDbAcisEntityData::eUndefined)
	{
		return t2;
	}
	if (t2 == CDbAcisEntityData::eUndefined)
	{
		return t1;
	}
	if (t1 == CDbAcisEntityData::eBody || t2 == CDbAcisEntityData::eBody)
	{
		return CDbAcisEntityData::eBody;
	}
	if (t1 == CDbAcisEntityData::e3DSolid && t2 == CDbAcisEntityData::eRegion)
	{
		return CDbAcisEntityData::eBody;
	}
	if (t1 == CDbAcisEntityData::eRegion && t2 == CDbAcisEntityData::e3DSolid)
	{
		return CDbAcisEntityData::eRegion;
	}
	if (t1 == CDbAcisEntityData::eRegion && t2 == CDbAcisEntityData::eRegion)
	{
		return CDbAcisEntityData::eRegion;
	}
	if (t2 == CDbAcisEntityData::e3DSolid && t2 == CDbAcisEntityData::e3DSolid)
	{
		return CDbAcisEntityData::e3DSolid;
	}

	assert(false);
	return CDbAcisEntityData::eUndefined;
}
