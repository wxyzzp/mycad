// DWGdirectWriter.h: interface of the DWGdirect class.
//
//////////////////////////////////////////////////////////////////////

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __DWGdirectWriter__
#define __DWGdirectWriter__

#include "OdaCommon.h"
#include "OdToolKit.h"
#include "OdString.h"
#include "DbDatabase.h"
#include "DbLayerTableRecord.h"

#include "ExSystemServices.h"
#include "ExHostAppServices.h"
#include "ModelerGeometryImpl.h"

//#define _DEBUG_MHB
#define ICAD_WNDACTION_GETPASSWORD   4200

const double TIMEFACTOR = 86400000.0;

const OdString EXTTMP(".$$$");
const OdString EXTBAK(".bak");
const OdString LDWG(".dwg");
const OdString LDXF(".dxf");
const OdString LDXB(".dxb");
const OdString ACAD("ACAD");	// appname
const OdString PSPACE("*Paper_Space");	
const OdString MSPACE("*Model_Space");	
const OdString MODEL("*Model");	
const OdString PAPER("*Paper");	
const OdString LAYOUT1("Layout1");	
const OdString LAYOUT2("Layout2");	

class DWGdirectConverter;
class DwExportParams;
class db_ltypetabrec;
class db_layertabrec;

class DWGdirectWriterServices : public ExSystemServices, public ExHostAppServices
{
protected:
          using ExSystemServices::operator new;
          using ExSystemServices::operator delete;

public:
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
			char retStr[100] = {'1'};
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

class DWGdirectWriter
{
public:
	DWGdirectWriter(OdRxObjectImpl<DWGdirectWriterServices>& exServices);
	~DWGdirectWriter();	

	bool writeDiskFile(char* fileName, db_drawing* drawing, short fileType, int version, bool bUsePasswd, bool bSavePreview);
	db_drawing* auditFile(db_drawing* drawing, bool fixErrors, short writeLogFile);

	enum SaveType
	{
		kSvg = 3,
		kDwf
	};

private:
	db_drawing* m_pICdwg;
	DWGdirectConverter* m_Converter;
	OdRxObjectImpl<DWGdirectWriterServices>* m_pExServices;

	bool populateDatabase(OdDbDatabasePtr& pDDdwg);

	void TransferTextStylesToDWGdirect(OdDbDatabasePtr& pDDdwg);
	void TransferRegAppsToDWGdirect(OdDbDatabasePtr& pDDdwg);
	void TransferLineTypesToDWGdirect(OdDbDatabasePtr& pDDdwg);
	void TransferLayersToDWGdirect(OdDbDatabasePtr& pDDdwg);
	void TransferDimStylesToDWGdirect(OdDbDatabasePtr& pDDdwg);
	void TransferViewportsToDWGdirect(OdDbDatabasePtr& pDDdwg);
	void TransferUCSsToDWGdirect(OdDbDatabasePtr& pDDdwg);
	void TransferViewsToDWGdirect(OdDbDatabasePtr& pDDdwg);
	void TransferObjectsToDWGdirect(OdDbDatabasePtr& pDDdwg);
	void TransferBlocksToDWGdirect(OdDbDatabasePtr& pDDdwg);
	void TransferSysVarsToDWGdirect(OdDbDatabasePtr& pDDdwg);
	void TransferEntitiesToDWGdirect(OdDbDatabasePtr& pDDdwg);

	void ProcessBlockWithLayout(OdDbDatabasePtr& pDDdwg, db_blocktabrec* pBlock, OdString blockName);
	void ProcessBlockWithoutLayout(OdDbDatabasePtr& pDDdwg, db_blocktabrec* pBlock, OdString blockName);

	OdDbObjectId TransferXRefToDWGdirect(OdDbDatabasePtr& pDDdwg, db_blocktabrec* pBlock);

	OdDbObjectId getUcsIDByName(OdDbDatabasePtr& pDDdwg, OdString& name);
	OdDbObjectId getUCSIdFromHandItem(OdDbDatabasePtr& pDDdwg, db_handitem* pUcs);
	OdDbObjectId createDictionary(OdDbDatabasePtr& pDDdwg, OdDbObjectId& ownerDictionary, db_handitem* pDictionary, OdString& name);

	void processDictionary(OdDbDatabasePtr& pDDdwg, OdDbObjectId& ownerDictId, db_handitem* pRefHanditem, OdString& recordName);
	void processDictionaryRecord(OdDbDatabasePtr& pDDdwg, OdString& recordName, db_hireflink* recordReference, OdDbDictionaryPtr& ownerDictionary);
	void processGroupDictionary(OdDbDatabasePtr& pDDdwg);
	void processMlineStylesDictionary(OdDbDatabasePtr& pDDdwg, db_handitem* pRefHanditem, OdString& recordName);
	void processUnknownDictionary(OdDbDatabasePtr& pDDdwg, OdDbObjectId& ownerDictId, db_handitem* pRefHanditem, OdString& recordName);

	void processSortentsTable(OdDbDatabasePtr& pDDdwg, db_hireflink *msRecordReference, OdDbDictionaryPtr& pOdExtensionDictionary);

	void processImageDictionary(OdDbDatabasePtr& pDDdwg, db_handitem* pRefHanditem, OdString& recordName);
	void processPlotSettingsDictionary(OdDbDatabasePtr& pDDdwg, db_handitem* pRefHanditem, OdString& recordName);
	void processPlotStyleNameDictionary(OdDbDatabasePtr& pDDdwg, db_handitem* pRefHanditem, OdString& recordName);
	void processAcdbVariableDictionary(OdDbDatabasePtr& pDDdwg, db_handitem* pRefHanditem, OdString& recordName);
	void processAseIndexDictionary(OdDbDatabasePtr& pDDdwg, db_handitem* pRefHanditem, OdString& recordName);
	void processColorDictionary(OdDbDatabasePtr& pDDdwg, db_handitem* pRefHanditem, OdString& recordName);
	void processMaterialDictionary(OdDbDatabasePtr& pDDdwg, db_handitem* pRefHanditem, OdString& recordName);
	
	bool getCurrentPSLayout(db_handitem** ppLayout);	// returns false if modelspace is active.

	bool saveAsSVG(OdDbDatabasePtr& pDDdwg, char* fileName);
	bool saveAsDWF(OdDbDatabasePtr& pDDdwg, char* fileName, int version);
	void initPageData(OdDbDatabasePtr& pDDdwg, char* fileName, DwExportParams& params);

	void setLineTypeProps(OdDbDatabasePtr& pDDdwg, OdDbLinetypeTableRecordPtr& pLt, db_ltypetabrec* pLineTypeRec);
	void setLayerProps(OdDbDatabasePtr& pDDdwg, OdDbLayerTableRecordPtr& pLayer, db_layertabrec* pLineTypeRec);

	void wrapUp(OdDbDatabasePtr& pDDdwg);
};

#endif	// __DWGdirectWriter__
