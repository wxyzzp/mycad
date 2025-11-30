// DWGdirectReader.h: interface of the DWGdirect class.
//
//////////////////////////////////////////////////////////////////////

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __DWGdirectReader__
#define __DWGdirectReader__

#include "DWGdirectProtocolExtension.h"

//#define _DEBUG_MHB

#define ICAD_WNDACTION_GETPASSWORD   4200

// From drwfns.cpp
extern BOOL ChangeEedAfterLoading(db_drawing* dp,db_handitem* hip,sds_resbuf* rbb);

class db_drawing;
class db_handitem;
class db_dictionary;
class db_dictionaryrec;
class db_rastervariables;
class db_idbuffer;
class db_spatial_filter;
class db_spatial_index;
class db_layer_index;
class CDbSortentsTable;
class db_vbaproject;
class db_acad_proxy_object;
class db_acad_proxy_entity;
class db_object_ptr;
class db_placeholder;
class db_dictionaryvar;

class DWGdirectReaderServices : public ExSystemServices, public ExHostAppServices
{
protected:
          using ExSystemServices::operator new;
          using ExSystemServices::operator delete;
private:

bool getPassword(const OdString& dwgName, bool bIsXRef, OdPassword& password)
{
	HMODULE   hmodule = GetModuleHandle(NULL);
	ASSERT(hmodule);
	if(hmodule)
	{
		typedef HWND (* getviewhwnd)() ;
		getviewhwnd pFun ;
		pFun = (getviewhwnd)GetProcAddress(hmodule,"sds_getmainhwnd");
		ASSERT(pFun);
		if(pFun)
		{
			char retStr[100] = {'0'};
			LRESULT res = SendMessage(pFun(),ICAD_WNDACTION_GETPASSWORD,
									  (WPARAM)retStr,(LPARAM)dwgName.c_str());

			if (res == S_OK)
			{
				wchar_t pwdBuff[_MAX_PATH] = {0};

				::OemToCharW(retStr, pwdBuff);
				::CharUpperW(pwdBuff);
				password = pwdBuff;

				return !password.isEmpty();
			}
			else
				return false;
      }
   }

  return false;
}

};

class DWGdirectReader
{
public:
	DWGdirectReader(OdRxObjectImpl<DWGdirectReaderServices>& exServices);
	~DWGdirectReader();	

	bool readDiskFile(char* fileName, db_drawing* drawing, short bIsRecovering);
	bool readMemoryFile(db_drawing* drawing, OdDbDatabasePtr& pOdDwg);

private:
	db_drawing* m_pICdwg;
	OdRxObjectImpl<DWGdirectReaderServices>* m_pExServices;
	delayedConversionMap m_delayedEntities;

	// Used to make icad entities out of DWGdirect entities.
	DWGdirectProtocolExtension m_TheProtocolExtensions;

	void TransferThumbnailBmpFromDWGdirect(OdDbDatabasePtr& pDDdw, OdString& fileReadName);
	void TransferStockHandlesFromDWGdirect(OdDbDatabasePtr& pDDdw);
	void FixPreR14dimblkDWGdirect(OdDbDatabasePtr& pDDdw);
	void TransferStartHandseedFromDWGdirect(OdDbDatabasePtr& pDDdw);
	void TransferBlocksFromDWGdirect(OdDbDatabasePtr& pDDdw);
	void TransferRegAppsFromDWGdirect(OdDbDatabasePtr& pDDdw);
	void TransferStylesFromDWGdirect(OdDbDatabasePtr& pDDdw);
	void TransferLineTypesFromDWGdirect(OdDbDatabasePtr& pDDdw);
	void TransferLayersFromDWGdirect(OdDbDatabasePtr& pDDdw);
	void TransferDimStylesFromDWGdirect(OdDbDatabasePtr& pDDdw);
	void TransferViewportsFromDWGdirect(OdDbDatabasePtr& pDDdw);
	void TransferViewsFromDWGdirect(OdDbDatabasePtr& pDDdw);
	void TransferUCSsFromDWGdirect(OdDbDatabasePtr& pDDdw);
	void TransferObjectsFromDWGdirect(OdDbDatabasePtr& pDDdw);
	void TransferEntitiesInBlocksFromDWGdirect(OdDbDatabasePtr& pDDdw);
	void TransferSystemVariablesFromDWGdirect(OdDbDatabasePtr& pDDdw);
	void TransferModelspaceEntsFromDWGdirect(OdDbDatabasePtr& pDDdw);
	void TransferPaperspaceEntsFromDWGdirect(OdDbDatabasePtr& pDDdw);
	void TransferWorldUCSFromDWGdirect(OdDbDatabasePtr& pDDdwg);

	OdString GetLayerNameFromDWGdirectId(OdDbObjectId& id);
	OdString GetLinetypeNameFromDWGdirectId(OdDbObjectId& id);
	OdString GetMlineStyleNameFromDWGdirectId(OdDbObjectId& id);

	void CreateAllGroups(db_dictionaryrec* pGroupDictionary, OdDbDictionary* pObject);
	void CreateAllMlineStyles(db_dictionaryrec* pMLineStyleDictionary, OdDbDictionary* pObject);
	void CreateAllLayouts(db_dictionaryrec* pLayoutDictionary, OdDbDictionary* pObject);
	void CreateAllImageDefs(db_dictionaryrec* pImageDictionary, OdDbDictionary* pObject);
	void CreateAllPlotSettings(db_dictionaryrec* pNewDictionary, OdDbDictionary* pObject);
	void CreateWipeoutVariables(OdDbObject* pObject);
	void CreateAllPlaceHolders(OdDbDictionary* pObject);
	void CreateRasterVariables(OdDbObject* pObject);
	void CreateSpatialIndex(db_drawing* pDwg, OdDbObject* pObject);
	void CreateObject(db_drawing* pDwg, OdDbObject* pObject);

	db_dictionary* GetNameObjectDictionaryFromDWGdirect(OdDbDatabasePtr& pDDdwg);

	bool populateIcadDWG(OdDbDatabasePtr& pDDdwg, OdString& fileReadName);
	void populateBlock(db_blocktabrec* pBlockRec, OdDbBlockTableRecordPtr pOdDbBlockRec, int isPaperSpaceBlock);

	void processDelayedEntities(delayedConversionMap* ents = NULL);
	void ProcessProxyObjectReferences();

	void setActiveLayoutTab(OdDbDatabasePtr& pDDdwg);

	void importDwf(OdDbDatabasePtr& pDDdwg, char* fileName);

	// Used by protocol extension functions too.
public:
	static OdString GetBlockNameFromDWGdirectId(OdDbObjectId& id);
	static OdString GetTextStyleNameFromDWGdirectId(OdDbObjectId& id);
	static OdString GetDimStyleNameFromDWGdirectId(OdDbObjectId& id);
	static void TransferHandleFromDWGdirect(db_handitem* pItem, OdDbObject* DWGdirectObject);
	static void TransferExtensionDictionariesFromDWGdirect(db_handitem* pItem, OdDbObject* DWGdirectObject, db_drawing* pDb);
	static void TransferEEDataFromDWGdirect(db_drawing* pDwg, db_handitem* pItem, OdDbObject* DWGdirectObject);
	static void TransferReactorsFromDWGdirect(db_drawing* pDwg, db_handitem* pItem, OdDbObject* DWGdirectObject, int type);
	static void TransferExtendedEntityDataFromDWGdirect(OdResBuf* eed, db_handitem* ent, db_drawing* pDwg, OdDbDatabase* pDDdwg);
	static void CreateXRecord(db_drawing* pDwg, OdDbObject* pObject);
	static void CreateDLPNTableRecord(db_drawing* pDwg, OdDbObject* pObject);
	static void CreateDictionaryVar(db_drawing* pDb, OdDbObject* pObject);
	static void CreateSortEntTable(OdDbDictionary* pDict, db_drawing* pDwg, OdDbObject* pObject);
	static void CreateIDBuffer(db_drawing* pDb, OdDbObject* pObject);
	static void CreateDimAssoc(db_drawing* pDb, db_handitem* pDim, OdDbObject* pObject);
	static void CreateSpatialFilter(db_drawing* pDwg, OdDbObject* pObject);
	static void CreateVBAproject(db_drawing* pDwg, OdDbObject* pObject);
	static db_dictionary* CreateDictionary(db_drawing* pDwg, OdDbObject* DWGdirectObject);
	static void AddDictionaryRecords(db_dictionary* pDictionary, OdDbDictionary* pObject, int ptrOwnerType);
	static db_handitem* CreateProxyObject(db_drawing* pDwg, OdDbObject* pObject);
	static void CreatePlaceHolder(db_drawing* pDwg, OdDbObject* pObject);
//	static void CreateLayerIndex(db_drawing* pDwg, OdDbObject* pObject);

	static void ProcessDictionaryRecords(OdDbDictionary* pDict, db_drawing* pDb, db_handitem* pItem);
};


#endif	// __DWGdirectReader__
