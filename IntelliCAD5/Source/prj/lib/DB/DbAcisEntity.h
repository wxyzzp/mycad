// File  : <DevDir>\source\prj\lib\db\DbAcisEntity.h
// Author: Dmitry Gavrilov

#pragma once

#ifndef _DBACISENTITY_H_
#define _DBACISENTITY_H_

#include "DbExport.h"
#include "entity.h"
#include "gvector.h"
#include "DbAcisEntityData.h"

class DB_CLASS CDbAcisEntity : public db_entity
{
	friend int db_CompEntLinks(db_BinChange** ppChanges, db_handitem* pOldOne, db_handitem* pNewOne);

public:
    // Ctors
	CDbAcisEntity
    (
    int type, 
    db_drawing* pDrw = 0
    );
	CDbAcisEntity
    (
    const CDbAcisEntity& source
    );
    // Dtor
	~CDbAcisEntity();

    // ?
	bool isAcisEntity() { return true; }

    // Set-get DXF group data
    virtual bool set_1(char*);
    virtual bool get_1(char*, int);
    virtual bool get_1(char**);
    virtual char **retp_1();

    // Set-get display-object list
    void* ret_disp();
	void set_disp(void*);
    
    // Get-set extension points
	void get_extent(sds_point, sds_point);
	void set_extent(sds_point, sds_point);
    
    // Get entity specific data
	int	entgetspecific
    (
    resbuf** ppBegRb, 
    resbuf** ppEndRb, 
    db_drawing* pDrw
    );

	// Get-set dragging data
	const icl::point& draggingStart() const;
	icl::point& draggingStart();
	const icl::point& draggingCurr() const;
	icl::point& draggingCurr();
	void setUcsToWcsTransformation(const icl::transf&);
	const icl::transf& getUcs2WcsTransformation() const;
	icl::gvector& axisZ();
	const icl::gvector& axisZ() const;
	icl::gvector& viewDir();
	const icl::gvector& viewDir() const;

    // 
	const char* retImageDataPtr();
	int getImageDataSize();
	void  getPoint(sds_point pt);
    const CDbAcisEntityData& getData() const;

    void setImageData(char* pNewImageData, int newImageDataSize);
    void setData(const CDbAcisEntityData&);
	void setPoint(sds_point newPoint);

protected:
	CDbAcisEntityData m_data; //SAT data + type information

	std::vector<char> m_image; // image data (?)

	icl::point m_draggingStart;
	icl::point m_draggingCurr;
	icl::gvector m_axisZ;
	icl::gvector m_viewDir;

    sds_point m_Point;			// unknown point in MarComp

	void* m_pDispList;		// disp objs linked list (gr_displayobject*)
	sds_point m_Extents[2];		// the extents of the disp objs (see db_3dface)

	static int s_EntsCount;		// number of calls of constructor
};


class DB_CLASS db_3dsolid : public CDbAcisEntity
{
	friend int db_CompEntLinks(db_BinChange** ppChanges, db_handitem* pOldOne, db_handitem* pNewOne);

public:
	db_3dsolid() : CDbAcisEntity(DB_3DSOLID)
	{}
	db_3dsolid(db_drawing* pDrw) : CDbAcisEntity(DB_3DSOLID, pDrw)
	{}
    db_3dsolid(const db_3dsolid& ent): CDbAcisEntity(ent)
    {}
};


class DB_CLASS db_body : public CDbAcisEntity
{
	friend int db_CompEntLinks(db_BinChange** ppChanges, db_handitem* pOldOne, db_handitem* pNewOne);

public:
	db_body() : CDbAcisEntity(DB_BODY)
	{}
	db_body(db_drawing* pDrw) : CDbAcisEntity(DB_BODY, pDrw)
	{}
    db_body(const db_body& ent) : CDbAcisEntity(ent)
    {}
};


class DB_CLASS db_region : public CDbAcisEntity
{
	friend int db_CompEntLinks(db_BinChange** ppChanges, db_handitem* pOldOne, db_handitem* pNewOne);

public:
	db_region() : CDbAcisEntity(DB_REGION)
	{}
	db_region(db_drawing* pDrw) : CDbAcisEntity(DB_REGION, pDrw)
	{}
    db_region(const db_region& ent) : CDbAcisEntity(ent)
    {}
};

#endif // _DBACISENTITY_H_
