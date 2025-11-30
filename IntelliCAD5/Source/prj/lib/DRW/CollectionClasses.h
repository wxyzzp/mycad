// CollectionClasses.h: 
// Classes used for storing handles for matching and delayed processing
//////////////////////////////////////////////////////////////////////


#include "OdaCommon.h"
#include "DbDatabase.h"
#include <map>

#ifndef _COLLECTIONCLASSES_H_
#define _COLLECTIONCLASSES_H_

const int BIT1	= 0x01;
const int BIT2	= 0x02;
const int BIT4	= 0x04;
const int BIT8	= 0x08;
const int BIT10 = 0x10;
const int BIT20 = 0x20;
const int BIT40 = 0x40;
const int BIT80 = 0x80;
const int BIT100 = 0x100;
const int BIT200 = 0x200;
const int BIT400 = 0x400;
const int BIT800 = 0x800;
const int BIT4000 = 0x4000;
const int BIT8000 = 0x8000;
const int BIT10000 = 0x10000;
const int BIT20000 = 0x20000;

class handlePairs;
typedef OdArray<handlePairs> reactorHandleMap;
class handItemObjectIDPair;
typedef OdArray<handItemObjectIDPair> delayedConversionMap;
class proxyRef;
class refHandle;
typedef OdArray<refHandle> proxyRefs;
typedef OdArray<proxyRef> proxyRefHandles;
class handleName;
typedef std::map<OdString, handleName> HandleMapOldStrNewStr;

class db_acad_proxy_object;

class handleName
{
public:
	handleName() {};
	handleName(OdString& newHandle, OdString& newName) {m_handle = newHandle; m_name = newName;}
	OdString& getHandle() { return m_handle; }
	OdString& getName() { return m_name; }

private:
	OdString m_handle;
	OdString m_name;
};

class handlePairs
{
public:
	handlePairs() {};
	handlePairs(OdDbHandle& e1, OdDbHandle& e2) {ent = e1; associatedEnt = e2;}
	OdDbHandle getEnt() { return ent; }
	OdDbHandle getAssoc() { return associatedEnt; }
private:
	OdDbHandle ent;
	OdDbHandle associatedEnt;
};

// Because it is possible for an instance of a db_insert to be converted
// prior to the block that it is made of, we will save off db_insert pointers,
// and process them in the DWGdirectConverter::~DWGdirectConverter(). Same
// for dimensions, leaders, hatches
class handItemObjectIDPair
{
public:
	handItemObjectIDPair() {m_pEntity = NULL;};
	handItemObjectIDPair(db_handitem* pE, OdDbObjectId id, int sp) { m_pEntity = pE; m_ObjectId = id; space = sp;}
	db_handitem* getHandItem() { return m_pEntity; }
	OdDbObjectId getObjectId() { return m_ObjectId; }
	int getSpace() { return space; }
private:
	int space;
	db_handitem* m_pEntity;
	OdDbObjectId m_ObjectId;
};

class refHandle
{
public:
	refHandle() { ASSERT(!"Don't Use this"); };
	refHandle (int type, OdString handle) { m_Type = type; m_handle = handle; }
	int m_Type;
	OdString m_handle;
};

class proxyRef
{
public:
	proxyRef() { ASSERT(!"Don't Use this"); };
	proxyRef(db_acad_proxy_object* proxy) { m_pProxy = proxy; }

	db_acad_proxy_object* getProxy() { return m_pProxy; }
	int getProxyType() { return m_Type; }
	proxyRefs& getRefHandles() { return m_RefHandles; }

void addReference(int type, OdString handle)
		{
			const refHandle refToAdd(type, handle);
			m_RefHandles.insert(m_RefHandles.end(), refToAdd);
		}
private:
	int m_Type; 
	proxyRefs m_RefHandles;
	db_acad_proxy_object* m_pProxy;
};

#endif //_COLLECTIONCLASSES_H_