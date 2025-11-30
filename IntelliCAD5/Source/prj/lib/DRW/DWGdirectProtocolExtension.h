// DWGdirectProtocolExtension.h: interface for the DWGdirectProtocolExtension class.
//
//////////////////////////////////////////////////////////////////////

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __DWGdirectProtocolExtension__
#define __DWGdirectProtocolExtension__

#include "RxObject.h"
#include "..\\..\\Licensed\\DWGdirect\\include\\DbEntity.h" // File name clash with ...lib\db\dbentity.h

class db_drawing;
class db_handitem;
class db_blocktabrec;

#define CPYPT3D(a, b) (a)[0] = (b).x; (a)[1] = (b).y; (a)[2] = (b).z;
#define CPYPT2D(a, b) (a)[0] = (b).x; (a)[1] = (b).y; (a)[2] = 0.0;

#define BLOBINT(pBuf, val) (pBuf[0]) = (char)(val & 0xFF); (pBuffUnk[1]) = (val>>8);
#define BLOBLONG(pBuf, val) (pBuf[0]) = val & 0xFF;(pBuf[1]) = (val>>8);(pBuf[2]) = (val>>16)  & 0xFF;(pBuf[3]) = (val>>24);

class DWGdirectProtocolServices : public ExSystemServices, public ExHostAppServices
{
protected:
          using ExSystemServices::operator new;
          using ExSystemServices::operator delete;
};

class OdDbEntity_Creator : public OdRxObject
{
public:
	ODRX_DECLARE_MEMBERS(OdDbEntity_Creator);
	OdDbEntity_Creator() {}

	friend class DWGdirectReader;

	virtual db_handitem* createHanditem(OdDbEntity* pEnt, db_drawing* pDb, db_blocktabrec* pBlockRecord = NULL) const;

protected:
	// If pBlockRecord == NULL, the entity created is added to the drawing database. If not, the entity is
	// added to the block record.
	static void finalize(OdDbEntity* pEntity, db_handitem* pEnt, db_drawing* pDb, db_blocktabrec* pBlockRecord, bool bAddToDatabase = true); 

}; // end class OdDbEntity_Creator

class Creators;

class DWGdirectProtocolExtension  
{
	Creators* m_pCreators;
public:
	DWGdirectProtocolExtension();
	virtual ~DWGdirectProtocolExtension();
	void initialize();
	void uninitialize();
};

#endif // __DWGdirectProtocolExtension__

