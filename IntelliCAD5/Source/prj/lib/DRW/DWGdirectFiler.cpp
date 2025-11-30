// CFiler.cpp: implementation of the DWGdirect class.
//
//////////////////////////////////////////////////////////////////////
#include "Db.h"

#include "OdaCommon.h"
#include "OdaDefs.h"

#include "DbFiler.h"
#include "RxObjectImpl.h"
#include "GeScale3d.h"
#include "OdString.h"
#include "DbObjectId.h"
#include "GePoint2d.h"
#include "GePoint3d.h"
#include "DbEntity.h"
#include "DbObject.h"
#include "OdBinaryData.h"

#include "DbLayerTableRecord.h"
#include "DbLinetypeTableRecord.h"

#include "DWGdirectFiler.h"

//#define _DEBUG_MHB
#ifdef _DEBUG_MHB
void __floatToString(double value, char *psBuffer, short prec);
#endif

void CDxfFiler::initResBuf(OdResBufPtr p)
{
	m_OdResBuf = p;
	m_pResBuf = p;
}

 int CDxfFiler::currentGroupCode()
{
	if (m_state == false)
	{
		m_groupCode = 0;
		if (!m_pResBuf.isNull())
		{
			m_pResBuf = m_pResBuf->next();
			m_groupCode = (m_pResBuf.isNull() ? 0 : m_pResBuf->restype());
		}
		m_state = true;
	}
	return m_groupCode;
}

int CDxfFiler::nextItem()
{
	if (!m_state)
	{
		m_groupCode = 0;
		if (!m_pResBuf.isNull())
		{
			m_pResBuf = m_pResBuf->next();
			m_groupCode = (m_pResBuf.isNull() ? 0 : m_pResBuf->restype());
		}
	}

	m_state = false;

	return m_groupCode;
}

void CDxfFiler::pushBackItem()
{
	m_state = true;
}

bool CDxfFiler::atExtendedData()
{
	int nextCode = currentGroupCode();
	return (nextCode == 1001);  // XDATA follows
}

bool CDxfFiler::atEOF() 
{ 
	int code = currentGroupCode();
//	return code == 0 || code == 100 || code == 1001 || code == 94; 
	return code == 0 || code == 100 || code == 1001; 
}

bool CDxfFiler::atEndOfObject() 
{ 
	int code = currentGroupCode();
	return code == 0; 
}

bool CDxfFiler::atSubclassData(const char * subClassName)
{
	if (!m_pResBuf.isNull() && m_pResBuf->restype() == 100 && m_pResBuf->getString() == subClassName)
	{
		m_state = false;
		return true;
	}
	else
	{
		return false;
	}
}

OdDb::DwgVersion CDxfFiler::dwgVersion(OdDb::MaintReleaseVer* pMVer) const
{
	if(pMVer) *pMVer = OdDb::kMRelease0;
	return OdDb::kDHL_1800;
}

void CDxfFiler::wrString(int groupCode, const OdChar *string)
{
	if (m_OdResBuf.isNull())
	{
		m_pResBuf = m_OdResBuf = OdResBuf::newRb(groupCode, OdString(string));
	}
	else
	{
		m_pResBuf->setNext(OdResBuf::newRb(groupCode, OdString(string)));
		m_pResBuf = m_pResBuf->next();
	}

#ifdef _DEBUG_MHB
	char psBuffer[100], sBuf[30];
	strcpy(psBuffer, "( ");
	itoa(groupCode, sBuf, 10);
	strcat(psBuffer, sBuf);
	strcat(psBuffer, " . ");
	strcat(psBuffer, "\"");
	strcat(psBuffer, string);
	strcat(psBuffer, "\" )");
	OutputDebugString("\t");
	OutputDebugString(psBuffer);
	OutputDebugString("\twrString()\n");
#endif //_DEBUG_MHB
	
	return;
}

void CDxfFiler::wrName(int groupCode, const OdChar *string)
{
	if (m_OdResBuf.isNull())
	{
		m_pResBuf = m_OdResBuf = OdResBuf::newRb(groupCode, OdString(string));
	}
	else
	{
		m_pResBuf->setNext(OdResBuf::newRb(groupCode, OdString(string)));
		m_pResBuf = m_pResBuf->next();
	}
	
#ifdef _DEBUG_MHB
	char psBuffer[100], sBuf[30];
	strcpy(psBuffer, "( ");
	itoa(groupCode, sBuf, 10);
	strcat(psBuffer, sBuf);
	strcat(psBuffer, " . ");
	strcat(psBuffer, "\"");
	strcat(psBuffer, string);
	strcat(psBuffer, "\" )");
	OutputDebugString("\t");
	OutputDebugString(psBuffer);
	OutputDebugString("\twrName()\n");
#endif //_DEBUG_MHB
	
	return;
}

void CDxfFiler::wrBool(int groupCode, bool val)
{
	if (m_OdResBuf.isNull())
	{
		m_pResBuf = m_OdResBuf = OdResBuf::newRb(groupCode, val);
	}
	else
	{
		m_pResBuf->setNext(OdResBuf::newRb(groupCode, val));
		m_pResBuf = m_pResBuf->next();
	}
	
#ifdef _DEBUG_MHB
	char psBuffer[100], sBuf[30];
	strcpy(psBuffer, "( ");
	itoa(groupCode, sBuf, 10);
	strcat(psBuffer, sBuf);
	strcat(psBuffer, " . ");

	strcat(psBuffer, val == true ? "true" : "false");
	strcat(psBuffer, " )");
	OutputDebugString("\t");
	OutputDebugString(psBuffer);
	OutputDebugString("\twrBool()\n");
#endif //_DEBUG_MHB

	return;
}

void CDxfFiler::wrInt8(int groupCode, OdInt8 val)
{
	if (m_OdResBuf.isNull())
	{
		m_pResBuf = m_OdResBuf = OdResBuf::newRb(groupCode, val);
	}
	else
	{
		m_pResBuf->setNext(OdResBuf::newRb(groupCode, val));
		m_pResBuf = m_pResBuf->next();
	}
	
#ifdef _DEBUG_MHB
	char psBuffer[100], sBuf[30];
	strcpy(psBuffer, "( ");
	itoa(groupCode, sBuf, 10);
	strcat(psBuffer, sBuf);
	strcat(psBuffer, " . ");

	itoa(val, sBuf, 10);

	strcat(psBuffer, sBuf);
	strcat(psBuffer, " )");
	OutputDebugString("\t");
	OutputDebugString(psBuffer);
	OutputDebugString("\twrInt8()\n");
#endif //_DEBUG_MHB
	
	return;
}

void CDxfFiler::wrUInt8(int groupCode, OdUInt8 val)
{
	if (m_OdResBuf.isNull())
	{
		m_pResBuf = m_OdResBuf = OdResBuf::newRb(groupCode, val);
	}
	else
	{
		m_pResBuf->setNext(OdResBuf::newRb(groupCode, val));
		m_pResBuf = m_pResBuf->next();
	}
	
#ifdef _DEBUG_MHB
	char psBuffer[100], sBuf[30];
	strcpy(psBuffer, "( ");
	itoa(groupCode, sBuf, 10);
	strcat(psBuffer, sBuf);
	strcat(psBuffer, " . ");

	itoa(val, sBuf, 10);

	strcat(psBuffer, sBuf);
	strcat(psBuffer, " )");
	OutputDebugString("\t");
	OutputDebugString(psBuffer);
	OutputDebugString("\twrUInt8()\n");
#endif //_DEBUG_MHB
	
	return;
}

void CDxfFiler::wrInt16(int groupCode, OdInt16 val)
{
	if (m_OdResBuf.isNull())
	{
		m_pResBuf = m_OdResBuf = OdResBuf::newRb(groupCode, val);
	}
	else
	{
		m_pResBuf->setNext(OdResBuf::newRb(groupCode, val));
		m_pResBuf = m_pResBuf->next();
	}
	
#ifdef _DEBUG_MHB
	char psBuffer[100], sBuf[30];
	strcpy(psBuffer, "( ");
	itoa(groupCode, sBuf, 10);
	strcat(psBuffer, sBuf);
	strcat(psBuffer, " . ");

	itoa(val, sBuf, 10);

	strcat(psBuffer, sBuf);
	strcat(psBuffer, " )");
	OutputDebugString("\t");
	OutputDebugString(psBuffer);
	OutputDebugString("\twrInt16()\n");
#endif //_DEBUG_MHB
	
	return;
}

void CDxfFiler::wrUInt16(int groupCode, OdUInt16 val)
{
	if (m_OdResBuf.isNull())
	{
		m_pResBuf = m_OdResBuf = OdResBuf::newRb(groupCode, val);
	}
	else
	{
		m_pResBuf->setNext(OdResBuf::newRb(groupCode, val));
		m_pResBuf = m_pResBuf->next();
	}
	
#ifdef _DEBUG_MHB
	char psBuffer[100], sBuf[30];
	strcpy(psBuffer, "( ");
	itoa(groupCode, sBuf, 10);
	strcat(psBuffer, sBuf);
	strcat(psBuffer, " . ");

	itoa(val, sBuf, 10);

	strcat(psBuffer, sBuf);
	strcat(psBuffer, " )");
	OutputDebugString("\t");
	OutputDebugString(psBuffer);
	OutputDebugString("\twrUInt16()\n");
#endif //_DEBUG_MHB
	
	return;
}

void CDxfFiler::wrInt32(int groupCode, OdInt32 val)
{
	if (m_OdResBuf.isNull())
	{
		m_pResBuf = m_OdResBuf = OdResBuf::newRb(groupCode, val);
	}
	else
	{
		m_pResBuf->setNext(OdResBuf::newRb(groupCode, val));
		m_pResBuf = m_pResBuf->next();
	}
	
#ifdef _DEBUG_MHB
	char psBuffer[100], sBuf[30];
	strcpy(psBuffer, "( ");
	itoa(groupCode, sBuf, 10);
	strcat(psBuffer, sBuf);
	strcat(psBuffer, " . ");

	itoa(val, sBuf, 10);

	strcat(psBuffer, sBuf);
	strcat(psBuffer, " )");
	OutputDebugString("\t");
	OutputDebugString(psBuffer);
	OutputDebugString("\twrInt32()\n");
#endif //_DEBUG_MHB
	
	return;
}

void CDxfFiler::wrUInt32(int groupCode, OdUInt32 val)
{
	if (m_OdResBuf.isNull())
	{
		m_pResBuf = m_OdResBuf = OdResBuf::newRb(groupCode, val);
	}
	else
	{
		m_pResBuf->setNext(OdResBuf::newRb(groupCode, val));
		m_pResBuf = m_pResBuf->next();
	}
	
#ifdef _DEBUG_MHB
	char psBuffer[100], sBuf[30];
	strcpy(psBuffer, "( ");
	itoa(groupCode, sBuf, 10);
	strcat(psBuffer, sBuf);
	strcat(psBuffer, " . ");

	itoa(val, sBuf, 10);

	strcat(psBuffer, sBuf);
	strcat(psBuffer, " )");
	OutputDebugString("\t");
	OutputDebugString(psBuffer);
	OutputDebugString("\twrUInt32()\n");
#endif //_DEBUG_MHB
	
	return;
}

void CDxfFiler::wrHandle(int groupCode, OdDbHandle val)
{
	if (m_OdResBuf.isNull())
	{
		m_pResBuf = m_OdResBuf = OdResBuf::newRb(groupCode);
	}
	else
	{
		m_pResBuf->setNext(OdResBuf::newRb(groupCode));
		m_pResBuf = m_pResBuf->next();
	}
	
	m_pResBuf->setHandle(val);

#ifdef _DEBUG_MHB
	char psBuffer[100], sBuf[30];
	strcpy(psBuffer, "( ");
	itoa(groupCode, sBuf, 10);
	strcat(psBuffer, sBuf);
	strcat(psBuffer, " . ");

	val.getIntoAsciiBuffer(sBuf);

	strcat(psBuffer, sBuf);
	strcat(psBuffer, " )");
	
	OutputDebugString("\t");
	OutputDebugString(psBuffer);
	OutputDebugString("\twrHandle()\n");
#endif //_DEBUG_MHB

	return;
}

void CDxfFiler::wrObjectId(int groupCode, OdDbObjectId val)
{
	if (m_OdResBuf.isNull())
	{
		m_pResBuf = m_OdResBuf = OdResBuf::newRb(groupCode);
	}
	else
	{
		m_pResBuf->setNext(OdResBuf::newRb(groupCode));
		m_pResBuf = m_pResBuf->next();
	}
	
	m_pResBuf->setObjectId(val);

#ifdef _DEBUG_MHB
	char psBuffer[100], sBuf[30];
	strcpy(psBuffer, "( ");
	itoa(groupCode, sBuf, 10);
	strcat(psBuffer, sBuf);
	strcat(psBuffer, " . ");

	val.getHandle().getIntoAsciiBuffer(sBuf);

	strcat(psBuffer, sBuf);
	strcat(psBuffer, " )");
	OutputDebugString("\t");
	OutputDebugString(psBuffer);
	OutputDebugString("\twrObjectId()\n");
#endif //_DEBUG_MHB
	
	return;
}

void CDxfFiler::wrAngle(int groupCode, double val, int precision)
{
	if (m_OdResBuf.isNull())
	{
		m_pResBuf = m_OdResBuf = OdResBuf::newRb(groupCode, val);
	}
	else
	{
		m_pResBuf->setNext(OdResBuf::newRb(groupCode, val));
		m_pResBuf = m_pResBuf->next();
	}
	
#ifdef _DEBUG_MHB
	char psBuffer[100], sBuf[30];
	strcpy(psBuffer, "( " );
	itoa(groupCode, sBuf, 10);
	strcat(psBuffer, sBuf);
	strcat(psBuffer, " . ");

	__floatToString(val, sBuf, 4);

	strcat(psBuffer, sBuf);
	strcat(psBuffer, " )");
	OutputDebugString("\t");
	OutputDebugString(psBuffer);
	OutputDebugString("\twrAngle()\n");
#endif //_DEBUG_MHB

	return;
}

void CDxfFiler::wrDouble(int groupCode, double val, int precision)
{
	if (m_OdResBuf.isNull())
	{
		m_pResBuf = m_OdResBuf = OdResBuf::newRb(groupCode, val);
	}
	else
	{
		m_pResBuf->setNext(OdResBuf::newRb(groupCode, val));
		m_pResBuf = m_pResBuf->next();
	}
	
#ifdef _DEBUG_MHB
	char psBuffer[100], sBuf[30];
	strcpy(psBuffer, "( " );
	itoa(groupCode, sBuf, 10);
	strcat(psBuffer, sBuf);
	strcat(psBuffer, " . ");

	__floatToString(val, sBuf, 4);

	strcat(psBuffer, sBuf);
	strcat(psBuffer, " )");
	OutputDebugString("\t");
	OutputDebugString(psBuffer);
	OutputDebugString("\twrDouble()\n");
#endif //_DEBUG_MHB

	return;
}

void CDxfFiler::wrPoint2d(int groupCode, const OdGePoint2d& pt, int precision)
{
	if (m_OdResBuf.isNull())
	{
		m_pResBuf = m_OdResBuf = OdResBuf::newRb(groupCode);
	}
	else
	{
		m_pResBuf->setNext(OdResBuf::newRb(groupCode));
		m_pResBuf = m_pResBuf->next();
	}
	
	OdGePoint3d tmp(pt.x, pt.y, 0.0);
	m_pResBuf->setPoint3d(tmp);

#ifdef _DEBUG_MHB
	char psBuffer[100], sBuf[30];
	strcpy(psBuffer, "( ");
	itoa(groupCode, sBuf, 10);
	strcat(psBuffer, sBuf);
	strcat(psBuffer, "  ");

	__floatToString(pt.x, sBuf, 4);
	strcat(psBuffer, sBuf);
	strcat(psBuffer, "  ");

	__floatToString(pt.y, sBuf, 4);
	strcat(psBuffer, sBuf);
	strcat(psBuffer, "  ");

	strcat(psBuffer, ")");
	OutputDebugString("\t");
	OutputDebugString(psBuffer);
	OutputDebugString("\twrPoint2d()\n");
#endif //_DEBUG_MHB

	return;
}

void CDxfFiler::wrPoint3d(int groupCode, const OdGePoint3d& pt, int precision)
{
	if (m_OdResBuf.isNull())
	{
		m_pResBuf = m_OdResBuf = OdResBuf::newRb(groupCode, pt);
	}
	else
	{
		m_pResBuf->setNext(OdResBuf::newRb(groupCode, pt));
		m_pResBuf = m_pResBuf->next();
	}
	
#ifdef _DEBUG_MHB
	char psBuffer[100], sBuf[30];
	strcpy(psBuffer, "( ");
	itoa(groupCode, sBuf, 10);
	strcat(psBuffer, sBuf);
	strcat(psBuffer, "  ");

	__floatToString(pt.x, sBuf, 4);
	strcat(psBuffer, sBuf);
	strcat(psBuffer, "  ");

	__floatToString(pt.y, sBuf, 4);
	strcat(psBuffer, sBuf);
	strcat(psBuffer, "  ");

	__floatToString(pt.z, sBuf, 4);        
	strcat(psBuffer, sBuf);
	strcat(psBuffer, ")");
	OutputDebugString("\t");
	OutputDebugString(psBuffer);
	OutputDebugString("\twrPoint3d()\n");
#endif //_DEBUG_MHB

	return;
}

void CDxfFiler::wrVector2d(int groupCode, const OdGeVector2d& pt, int precision)
{
	if (m_OdResBuf.isNull())
	{
		m_pResBuf = m_OdResBuf = OdResBuf::newRb(groupCode);
	}
	else
	{
		m_pResBuf->setNext(OdResBuf::newRb(groupCode));
		m_pResBuf = m_pResBuf->next();
	}
	
	OdGePoint3d tmp(pt.x, pt.y, 0.0);
	m_pResBuf->setPoint3d(tmp);

#ifdef _DEBUG_MHB
	char psBuffer[100], sBuf[30];
	strcpy(psBuffer, "( ");
	itoa(groupCode, sBuf, 10);
	strcat(psBuffer, sBuf);
	strcat(psBuffer, "  ");

	__floatToString(pt.x, sBuf, 4);
	strcat(psBuffer, sBuf);
	strcat(psBuffer, "  ");

	__floatToString(pt.y, sBuf, 4);
	strcat(psBuffer, sBuf);
	strcat(psBuffer, "  ");

	strcat(psBuffer, ")");
	OutputDebugString("\t");
	OutputDebugString(psBuffer);
	OutputDebugString("\twrVector2d()\n");
#endif //_DEBUG_MHB

	return;
}

void CDxfFiler::wrVector3d(int groupCode, const OdGeVector3d& pt, int precision)
{
	if (m_OdResBuf.isNull())
	{
		m_pResBuf = m_OdResBuf = OdResBuf::newRb(groupCode);
	}
	else
	{
		m_pResBuf->setNext(OdResBuf::newRb(groupCode));
		m_pResBuf = m_pResBuf->next();
	}
	
	OdGePoint3d tmp(pt.x, pt.y, pt.z);
	m_pResBuf->setPoint3d(tmp);

#ifdef _DEBUG_MHB
	char psBuffer[100], sBuf[30];
	strcpy(psBuffer, "( ");
	itoa(groupCode, sBuf, 10);
	strcat(psBuffer, sBuf);
	strcat(psBuffer, "  ");

	__floatToString(pt.x, sBuf, 4);
	strcat(psBuffer, sBuf);
	strcat(psBuffer, "  ");

	__floatToString(pt.y, sBuf, 4);
	strcat(psBuffer, sBuf);
	strcat(psBuffer, "  ");

	__floatToString(pt.z, sBuf, 4);        
	strcat(psBuffer, sBuf);
	strcat(psBuffer, ")");
	OutputDebugString("\t");
	OutputDebugString(psBuffer);
	OutputDebugString("\twrVector3d()\n");
#endif //_DEBUG_MHB

	return;
}

void CDxfFiler::wrScale3d(int groupCode, const OdGeScale3d& pt, int precision)
{
	if (m_OdResBuf.isNull())
	{
		m_pResBuf = m_OdResBuf = OdResBuf::newRb(groupCode);
	}
	else
	{
		m_pResBuf->setNext(OdResBuf::newRb(groupCode));
		m_pResBuf = m_pResBuf->next();
	}
	
	OdGePoint3d tmp(pt.sx, pt.sy, pt.sz);
	m_pResBuf->setPoint3d(tmp);

#ifdef _DEBUG_MHB
	char psBuffer[100], sBuf[30];
	strcpy(psBuffer, "( ");
	itoa(groupCode, sBuf, 10);
	strcat(psBuffer, sBuf);
	strcat(psBuffer, "  ");

	__floatToString(pt.sx, sBuf, 4);
	strcat(psBuffer, sBuf);
	strcat(psBuffer, "  ");

	__floatToString(pt.sy, sBuf, 4);
	strcat(psBuffer, sBuf);
	strcat(psBuffer, "  ");

	__floatToString(pt.sz, sBuf, 4);        
	strcat(psBuffer, sBuf);
	strcat(psBuffer, ")");
	OutputDebugString("\t");
	OutputDebugString(psBuffer);
	OutputDebugString("\twrScale3d()\n");
#endif //_DEBUG_MHB

	return;
}

void CDxfFiler::wrBinaryChunk(int groupCode, const OdUInt8* pBuff, OdUInt32 nSize)
{
	if (m_OdResBuf.isNull())
	{
		m_pResBuf = m_OdResBuf = OdResBuf::newRb(groupCode);
	}
	else
	{
		m_pResBuf->setNext(OdResBuf::newRb(groupCode));
		m_pResBuf = m_pResBuf->next();
	}

	OdBinaryData binaryData;
	binaryData.resize(nSize);
	memcpy(binaryData.asArrayPtr(), pBuff, nSize);

	m_pResBuf->setBinaryChunk(binaryData);

#ifdef _DEBUG_MHB
	char* tmp = new char[100];

	sprintf(tmp, "\t(%i . %i)  wrBinaryChunk())\n", groupCode, nSize);
	OutputDebugString(tmp);
	delete [] tmp;
#endif //_DEBUG_MHB
}


void CDxfFiler::rdString(OdString& string)
{
	if (m_pResBuf.isNull())
	{
		m_pResBuf = m_OdResBuf;
		ASSERT(!m_pResBuf.isNull());
		ASSERT(m_pResBuf->restype() == OdDxfCode::String);
		string = m_pResBuf->getString();
	}
	else
	{
		string = m_pResBuf->getString();
	}

#ifdef _DEBUG_MHB
	char psBuffer[100];
	strcpy(psBuffer, "( wrString() . \"");
	strcat(psBuffer, string);
	strcat(psBuffer, "\" )\n");
	OutputDebugString(psBuffer);
#endif //_DEBUG_MHB
	
  return;
}

bool CDxfFiler::rdBool()
{
	bool val = false;

	if (m_pResBuf.isNull())
	{
		m_pResBuf = m_OdResBuf;
		ASSERT(!m_pResBuf.isNull());
		ASSERT(m_pResBuf->restype() == OdDxfCode::Bool);
		val = m_pResBuf->getBool();
	}
	else
	{
		val = m_pResBuf->getBool();
	}

#ifdef _DEBUG_MHB
	char psBuffer[100];
	strcpy(psBuffer, "( wrBool() . ");
	strcat(psBuffer, val == true ? "true" : "false");
	strcat(psBuffer, " )\n");
	OutputDebugString(psBuffer);
#endif //_DEBUG_MHB

  return val;
}

OdInt8 CDxfFiler::rdInt8()
{
	OdInt8 ret = 0;

	if (m_pResBuf.isNull())
	{
		m_pResBuf = m_OdResBuf;
		ASSERT(!m_pResBuf.isNull());
		ASSERT(m_pResBuf->restype() == OdDxfCode::Integer8);
		ret = m_pResBuf->getInt8();
	}
	else
	{
		ret = m_pResBuf->getInt8();
	}

  return ret;
}

OdInt16 CDxfFiler::rdInt16()
{
	OdInt16 val = 0;

	if (m_pResBuf.isNull())
	{
		m_pResBuf = m_OdResBuf;
		ASSERT(!m_pResBuf.isNull());
		ASSERT(m_pResBuf->restype() == OdDxfCode::Integer16);
		val = m_pResBuf->getInt16();
	}
	else
	{
		val = m_pResBuf->getInt16();
	}

#ifdef _DEBUG_MHB
	char psBuffer[100], sBuf[30];
	strcpy(psBuffer, "( rdInt16() . ");
	itoa(val, sBuf, 10);
	strcat(psBuffer, sBuf);
	strcat(psBuffer, " )\n");
	OutputDebugString(psBuffer);
#endif //_DEBUG_MHB
	
  return val;
}

OdInt32 CDxfFiler::rdInt32()
{
	OdInt32 val = 0;

	if (m_pResBuf.isNull())
	{
		m_pResBuf = m_OdResBuf;
		ASSERT(!m_pResBuf.isNull());
		ASSERT(m_pResBuf->restype() == OdDxfCode::Integer32);
		val = m_pResBuf->getInt32();
	}
	else
	{
		val = m_pResBuf->getInt32();
	}

#ifdef _DEBUG_MHB
	char psBuffer[100], sBuf[30];
	strcpy(psBuffer, "( rdInt32() . ");
	itoa(val, sBuf, 10);
	strcat(psBuffer, sBuf);
	strcat(psBuffer, " )\n");
	OutputDebugString(psBuffer);
#endif //_DEBUG_MHB
	
  return val;
}

OdUInt8 CDxfFiler::rdUInt8()
{
	OdUInt8 val = 0;

	if (m_pResBuf.isNull())
	{
		m_pResBuf = m_OdResBuf;
		ASSERT(!m_pResBuf.isNull());
		ASSERT(m_pResBuf->restype() == OdDxfCode::Integer8);
		val = m_pResBuf->getInt8();
	}
	else
	{
		val = m_pResBuf->getInt8();
	}

#ifdef _DEBUG_MHB
	char psBuffer[100], sBuf[30];
	strcpy(psBuffer, "( rdUInt8() . ");
	itoa(val, sBuf, 10);
	strcat(psBuffer, sBuf);
	strcat(psBuffer, " )\n");
	OutputDebugString(psBuffer);
#endif //_DEBUG_MHB
	
   return val;
}

OdUInt16 CDxfFiler::rdUInt16()
{
	OdUInt16 val = 0;

	if (m_pResBuf.isNull())
	{
		m_pResBuf = m_OdResBuf;
		ASSERT(!m_pResBuf.isNull());
		ASSERT(m_pResBuf->restype() == OdDxfCode::Integer16);
		val = m_pResBuf->getInt16();
	}
	else
	{
		val = m_pResBuf->getInt16();
	}

#ifdef _DEBUG_MHB
	char psBuffer[100], sBuf[30];
	strcpy(psBuffer, "( rdUInt16() . ");
	itoa(val, sBuf, 10);
	strcat(psBuffer, sBuf);
	strcat(psBuffer, " )\n");
	OutputDebugString(psBuffer);
#endif //_DEBUG_MHB
	
  return val;
}

OdUInt32 CDxfFiler::rdUInt32()
{
	OdUInt32 val = 0;

	if (m_pResBuf.isNull())
	{
		m_pResBuf = m_OdResBuf;
		ASSERT(!m_pResBuf.isNull());
		ASSERT(m_pResBuf->restype() == OdDxfCode::Integer32);
		val = m_pResBuf->getInt32();
	}
	else
	{
		val = m_pResBuf->getInt32();
	}

#ifdef _DEBUG_MHB
	char psBuffer[100], sBuf[30];
	strcpy(psBuffer, "( rdUInt32() . ");
	itoa(val, sBuf, 10);
	strcat(psBuffer, sBuf);
	strcat(psBuffer, " )\n");
	OutputDebugString(psBuffer);
#endif //_DEBUG_MHB
	
  return val;
}

OdDbHandle CDxfFiler::rdHandle()
{
	OdDbHandle val;

	if (m_pResBuf.isNull())
	{
		m_pResBuf = m_OdResBuf;
		ASSERT(!m_pResBuf.isNull());
		ASSERT(m_pResBuf->restype() == OdDxfCode::Handle);
		val = m_pResBuf->getHandle();
	}
	else
	{
		val = m_pResBuf->getHandle();
	}

#ifdef _DEBUG_MHB
	char psBuffer[100], sBuf[30];
	strcpy(psBuffer, "( rdHandle() . ");
	val.getIntoAsciiBuffer(sBuf);
	strcat(psBuffer, sBuf);
	strcat(psBuffer, " )\n");
	OutputDebugString(psBuffer);
#endif //_DEBUG_MHB

  return val;
}

OdDbObjectId CDxfFiler::rdObjectId()
{
	OdDbObjectId val;

	if (m_pResBuf.isNull())
	{
		m_pResBuf = m_OdResBuf;
		ASSERT(!m_pResBuf.isNull());
		ASSERT(m_pResBuf->restype() == OdDxfCode::ObjectId);
		val = m_pResBuf->getObjectId(m_pDb);
	}
	else
	{
		val = m_pResBuf->getObjectId(m_pDb);
	}

#ifdef _DEBUG_MHB
	char psBuffer[100], sBuf[30];
	strcpy(psBuffer, "( rdObjectId() . ");

	val.getHandle().getIntoAsciiBuffer(sBuf);

	strcat(psBuffer, sBuf);
	strcat(psBuffer, " )\n");
	OutputDebugString(psBuffer);
#endif //_DEBUG_MHB
	
  return val;
}

double  CDxfFiler::rdAngle()
{
	double val = 0.0;

	if (m_pResBuf.isNull())
	{
		m_pResBuf = m_OdResBuf;
		ASSERT(!m_pResBuf.isNull());
		ASSERT(m_pResBuf->restype() == OdDxfCode::Angle);
		val = m_pResBuf->getDouble();
	}
	else
	{
		val = m_pResBuf->getDouble();
	}

#ifdef _DEBUG_MHB
	char psBuffer[100], sBuf[30];
	strcpy(psBuffer, "( rdAngle() . " );

	__floatToString(val, sBuf, 4);

	strcat(psBuffer, sBuf);
	strcat(psBuffer, " )\n");
	OutputDebugString(psBuffer);
#endif //_DEBUG_MHB

  return val;
}

double CDxfFiler::rdDouble()
{
	double val = 0;

	if (m_pResBuf.isNull())
	{
		m_pResBuf = m_OdResBuf;
		ASSERT(!m_pResBuf.isNull());
		ASSERT(m_pResBuf->restype() == OdDxfCode::Double);
		val = m_pResBuf->getDouble();
	}
	else
	{
		val = m_pResBuf->getDouble();
	}

#ifdef _DEBUG_MHB
	char psBuffer[100], sBuf[30];
	strcpy(psBuffer, "( rdDouble() . " );

	__floatToString(val, sBuf, 4);

	strcat(psBuffer, sBuf);
	strcat(psBuffer, " )\n");
	OutputDebugString(psBuffer);
#endif //_DEBUG_MHB

  return val;
}

void CDxfFiler::rdPoint2d(OdGePoint2d& pt)
{
	if (m_pResBuf.isNull())
	{
		m_pResBuf = m_OdResBuf;
		ASSERT(!m_pResBuf.isNull());
		ASSERT(m_pResBuf->restype() == OdDxfCode::Point);
		OdGePoint3d point = m_pResBuf->getPoint3d();
		pt.x = point.x;
		pt.y = point.y;
	}
	else
	{
		OdGePoint3d point = m_pResBuf->getPoint3d();
		pt.x = point.x;
		pt.y = point.y;
	}

#ifdef _DEBUG_MHB
	char psBuffer[100], sBuf[30];
	strcpy(psBuffer, "( rdPoint2d() . ");

	__floatToString(pt.x, sBuf, 4);
	strcat(psBuffer, sBuf);
	strcat(psBuffer, "  ");

	__floatToString(pt.y, sBuf, 4);
	strcat(psBuffer, sBuf);
	strcat(psBuffer, "  ");

	strcat(psBuffer, ")\n");
	OutputDebugString(psBuffer);
#endif //_DEBUG_MHB

	return;
}

void CDxfFiler::rdPoint3d(OdGePoint3d& pt)
{
	if (m_pResBuf.isNull())
	{
		m_pResBuf = m_OdResBuf;
		ASSERT(!m_pResBuf.isNull());
		ASSERT(m_pResBuf->restype() == OdDxfCode::Point);
		pt = m_pResBuf->getPoint3d();
	}
	else
	{
		pt = m_pResBuf->getPoint3d();
	}

#ifdef _DEBUG_MHB
	char psBuffer[100], sBuf[30];
	strcpy(psBuffer, "( rdPoint3d() . ");

	__floatToString(pt.x, sBuf, 4);
	strcat(psBuffer, sBuf);
	strcat(psBuffer, "  ");

	__floatToString(pt.y, sBuf, 4);
	strcat(psBuffer, sBuf);
	strcat(psBuffer, "  ");

	__floatToString(pt.z, sBuf, 4);
	strcat(psBuffer, sBuf);
	strcat(psBuffer, "  ");

	strcat(psBuffer, ")\n");
	OutputDebugString(psBuffer);
#endif //_DEBUG_MHB

	return;
}

void CDxfFiler::rdVector2d(OdGeVector2d& pt)
{
	if (m_pResBuf.isNull())
	{
		m_pResBuf = m_OdResBuf;
		ASSERT(!m_pResBuf.isNull());
		ASSERT(m_pResBuf->restype() == OdDxfCode::Point);
		OdGePoint3d point = m_pResBuf->getPoint3d();
		pt.x = point.x;
		pt.y = point.y;
	}
	else
	{
		OdGePoint3d point = m_pResBuf->getPoint3d();
		pt.x = point.x;
		pt.y = point.y;
	}

#ifdef _DEBUG_MHB
	char psBuffer[100], sBuf[30];
	strcpy(psBuffer, "( rdVector2d() . ");

	__floatToString(pt.x, sBuf, 4);
	strcat(psBuffer, sBuf);
	strcat(psBuffer, "  ");

	__floatToString(pt.y, sBuf, 4);
	strcat(psBuffer, sBuf);
	strcat(psBuffer, "  ");

	strcat(psBuffer, ")\n");
#endif //_DEBUG_MHB

	return;
}

void CDxfFiler::rdVector3d(OdGeVector3d& pt)
{
	if (m_pResBuf.isNull())
	{
		m_pResBuf = m_OdResBuf;
		ASSERT(!m_pResBuf.isNull());
		ASSERT(m_pResBuf->restype() == OdDxfCode::Point);
		OdGePoint3d point = m_pResBuf->getPoint3d();
		pt.x = point.x;
		pt.y = point.y;
		pt.z = point.z;
	}
	else
	{
		OdGePoint3d point = m_pResBuf->getPoint3d();
		pt.x = point.x;
		pt.y = point.y;
		pt.z = point.z;
	}

#ifdef _DEBUG_MHB
	char psBuffer[100], sBuf[30];
	strcpy(psBuffer, "( rdVector3d() . ");

	__floatToString(pt.x, sBuf, 4);
	strcat(psBuffer, sBuf);
	strcat(psBuffer, "  ");

	__floatToString(pt.y, sBuf, 4);
	strcat(psBuffer, sBuf);
	strcat(psBuffer, "  ");

	__floatToString(pt.z, sBuf, 4);
	strcat(psBuffer, sBuf);
	strcat(psBuffer, "  ");

	strcat(psBuffer, ")\n");
#endif //_DEBUG_MHB

	return;
}

void CDxfFiler::rdScale3d(OdGeScale3d& pt)
{
	if (m_pResBuf.isNull())
	{
		m_pResBuf = m_OdResBuf;
		ASSERT(!m_pResBuf.isNull());
		ASSERT(m_pResBuf->restype() == OdDxfCode::Point);
		OdGePoint3d point = m_pResBuf->getPoint3d();
		pt.sx = point.x;
		pt.sy = point.y;
		pt.sz = point.z;
	}
	else
	{
		OdGePoint3d point = m_pResBuf->getPoint3d();
		pt.sx = point.x;
		pt.sy = point.y;
		pt.sz = point.z;
	}

#ifdef _DEBUG_MHB
	char psBuffer[100], sBuf[30];
	strcpy(psBuffer, "( rdScale3d() .");

	__floatToString(pt.sx, sBuf, 4);
	strcat(psBuffer, sBuf);
	strcat(psBuffer, "  ");

	__floatToString(pt.sy, sBuf, 4);
	strcat(psBuffer, sBuf);
	strcat(psBuffer, "  ");

	__floatToString(pt.sz, sBuf, 4);        
	strcat(psBuffer, sBuf);
	strcat(psBuffer, ")\n");
	OutputDebugString(psBuffer);
#endif //_DEBUG_MHB

	return;
}

void CDxfFiler::rdBinaryChunk(OdBinaryData& val)
{
	int nSize;
	if (m_pResBuf.isNull())
	{
		m_pResBuf = m_OdResBuf;
		ASSERT(!m_pResBuf.isNull());
		ASSERT(m_pResBuf->restype() == OdDxfCode::BinaryChunk);
		OdBinaryData binaryData = m_pResBuf->getBinaryChunk();

		nSize = binaryData.length();
		val.resize(nSize);
		memcpy((void*)val.asArrayPtr(), (void*)binaryData.asArrayPtr(), nSize);
	}
	else
	{
		OdBinaryData binaryData = m_pResBuf->getBinaryChunk();

		nSize = binaryData.length();
		val.resize(nSize);
		memcpy((void*)val.asArrayPtr(), (void*)binaryData.asArrayPtr(), nSize);
	}

#ifdef _DEBUG_MHB
	char* tmpBuf = new char[50];

	sprintf(tmpBuf, "( rdBinaryChunk() . %i)\n", nSize);
	OutputDebugString(tmpBuf);
	delete [] tmpBuf;
#endif //_DEBUG_MHB

	return;
}

#ifdef _DEBUG_MHB
void __floatToString(double value, char *psBuffer, short prec)
{
	int dec, sign;
	short place = 0;
	char *psTemp = _fcvt(value, prec, &dec, &sign);
	memset(psBuffer, '\0', prec + 4);
	if (sign) {
		strcpy(psBuffer, "-");
		place++;
	}
    dec = (dec > 4) ? 4 : (dec < -4) ? -4 : dec;
	if (dec <= 0) {
		if (prec) {
			strcat(psBuffer, "0.");
			place += 2;
		} else {
			strcat(psBuffer, "0");
			place++;
		}
		memset(psBuffer + place, '0', abs(dec));
		strcat(psBuffer, psTemp);
	} else {
		strncat(psBuffer, psTemp, dec);
		if (prec)
			strcat(psBuffer, ".");
		strcat(psBuffer, psTemp + dec);
	}
}
#endif

int CDxfFiler::precision() const
{
  return 8;
}


