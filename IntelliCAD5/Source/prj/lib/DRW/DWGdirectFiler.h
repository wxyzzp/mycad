// CFiler.h: interface of the DWGdirect class.
//
//////////////////////////////////////////////////////////////////////

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DbFiler.h"

class CDxfFiler : public OdRxObjectImpl<OdDbDxfFiler>
{
	OdDbDatabase* m_pDb;

public:

	CDxfFiler(OdDbDatabase* pDb) : m_pDb(pDb) {}
	
	FilerType filerType() const {return OdDbFiler::kCopyFiler;}
	OdDbDatabase* database() const { return m_pDb; }

	OdResBufPtr& getFilerData() { return m_OdResBuf; }

	OdDb::DwgVersion dwgVersion(OdDb::MaintReleaseVer* = NULL) const;

	// Data output functions
	void wrString(int groupCode, const OdChar *string);
	void wrName(int groupCode, const OdChar *string);
	void wrBool(int groupCode, bool val);
	void wrInt8(int groupCode, OdInt8 val);
	void wrUInt8(int groupCode, OdUInt8 val);
	void wrInt16(int groupCode, OdInt16 val);
	void wrUInt16(int groupCode, OdUInt16 val);
	void wrInt32(int groupCode, OdInt32 val);
	void wrUInt32(int groupCode, OdUInt32 val);
	void wrHandle(int groupCode, OdDbHandle val);
	void wrObjectId(int groupCode, OdDbObjectId val);
	void wrAngle(int groupCode, double val, int precision = kDfltPrec);
	void wrDouble(int groupCode, double val, int precision = kDfltPrec);
	void wrPoint2d(int groupCode, const OdGePoint2d& pt, int precision = kDfltPrec);
	void wrPoint3d(int groupCode, const OdGePoint3d& pt, int precision = kDfltPrec);
	void wrVector2d(int groupCode, const OdGeVector2d& pt, int precision = kDfltPrec);
	void wrVector3d(int groupCode, const OdGeVector3d& pt, int precision = kDfltPrec);
	void wrScale3d(int groupCode, const OdGeScale3d& pt, int precision = kDfltPrec);
	void wrBinaryChunk(int groupCode, const OdUInt8* pBuff, OdUInt32 nSize);

	void        rdString(OdString &string);
	bool        rdBool();
	OdInt8      rdInt8();
	OdInt16     rdInt16();
	OdInt32     rdInt32();
	OdUInt8     rdUInt8();
	OdUInt16    rdUInt16();
	OdUInt32    rdUInt32();
	OdDbHandle  rdHandle();
	OdDbObjectId rdObjectId();
	double      rdAngle();
	double      rdDouble();
	void        rdPoint2d(OdGePoint2d& pt);
	void        rdPoint3d(OdGePoint3d& pt);
	void        rdVector2d(OdGeVector2d& pt);
	void        rdVector3d(OdGeVector3d& pt);
	void        rdScale3d(OdGeScale3d& pt);
	void        rdBinaryChunk(OdBinaryData&);

	int  precision() const;

	int  currentGroupCode();
	bool atEOF();
	bool atEndOfObject();
	bool atSubclassData(const char * subClassName);
	int nextItem();
	void initResBuf(OdResBufPtr p);
	bool atExtendedData();
	void pushBackItem();
private:
	bool m_state;
	int m_groupCode;
	OdResBufPtr m_OdResBuf;
	OdResBufPtr m_pResBuf;
};