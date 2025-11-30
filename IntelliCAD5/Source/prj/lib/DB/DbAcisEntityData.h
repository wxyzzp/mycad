#ifndef _DBACISENITYDATA_H_
#define _DBACISENITYDATA_H_

#include "db.h"
#include "xstd.h"

// structure for passing typed sat data, to differ body's sat data from 3dsolid's sat-data
class DB_CLASS CDbAcisEntityData
{
public: //data types
	// object type
	enum EType
	{
		eUndefined,
		e3DSolid,
		eRegion,
		eBody
	};

protected:
	// sat-data
	icl::string* m_pSat;
	// type
	EType m_type;

public:
	// ctors
	CDbAcisEntityData();

	CDbAcisEntityData(const CDbAcisEntityData& obj);

	CDbAcisEntityData(const icl::string& sat, EType type);

	enum
	{
		eDbType
	};

	CDbAcisEntityData(const icl::string& sat, int dbt, int);

	CDbAcisEntityData(const icl::string& sat, const char* dbt, int);

	virtual ~CDbAcisEntityData() { if (m_pSat) delete m_pSat; }

	// assignment
	const CDbAcisEntityData& operator = (const CDbAcisEntityData& obj);

	void clear();

	// accessors
	const icl::string* getAcisData() const { return m_pSat; }
	const icl::string& getSat() const;
	void setSat(const icl::string& s);

	EType getType() const;
	void setType(const EType& t);

	int getDbtype() const;

	int setDbtype(int t);

	int setDbtype(const char* type);
};

// operators
DB_API CDbAcisEntityData::EType operator * 
(
CDbAcisEntityData::EType,
CDbAcisEntityData::EType
);

DB_API CDbAcisEntityData::EType operator + 
(
CDbAcisEntityData::EType,
CDbAcisEntityData::EType
);

DB_API CDbAcisEntityData::EType operator - 
(
CDbAcisEntityData::EType,
CDbAcisEntityData::EType
);

inline
bool operator < 
(
const CDbAcisEntityData& o1, 
const CDbAcisEntityData& o2
)
{
	return o1.getSat() < o2.getSat();
}

#endif
