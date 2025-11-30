#ifndef _EX_HOSTAPPSERVICES_H_
#define _EX_HOSTAPPSERVICES_H_

#include "DbAudit.h"
#include "DbHostAppServices.h"
#include "HatchPatternManager.h"
#include "StaticRxObject.h"
#include "DbDatabaseReactor.h"
#include <map>

typedef OdArray<OdString> stringArray;

class ExHostAppServices : public OdDbHostAppServices, public OdDbHostAppProgressMeter, public OdDbAuditInfo
{
	class DwgCollection : public OdStaticRxObject<OdDbDatabaseReactor>
	{
		std::map<OdString, OdDbDatabase*> m_loadedDwgs;
		void goodbye(const OdDbDatabase* pDb);
	public:
		OdDbDatabase* lookUp(const OdString& keyFileName);
		void add(OdDbDatabase* pDb);
	}m_dwgCollection;


	OdString  m_Prefix;
	long      m_MeterLimit;
	long      m_MeterCurrent;
	long      m_MeterOld;
	bool      m_disableOutput;
	bool	  m_bSaveRoundTrip;

	OdHatchPatternManagerPtr m_patternManager;
public:
	ExHostAppServices();

	OdDbHostAppProgressMeter* newProgressMeter();

	void releaseProgressMeter(OdDbHostAppProgressMeter* pMeter);

	void warning(const OdString& ) { }

	DD_USING(OdDbHostAppServices::warning); 

	bool getSAVEROUNDTRIP() const 
	{ 
		return (m_bSaveRoundTrip!=false); 
	}
	void setSAVEROUNDTRIP(bool val) { m_bSaveRoundTrip = val; }

	// OdDbHostAppProgressMeter functions
	void start(const char* displayString = NULL);
	void stop();
	void meterProgress();
	void setLimit(int max);
	bool ttfFileNameByDescriptor(const OdTtfDescriptor& descr, OdString& fileName);
	void disableOutput(bool bDisable) { m_disableOutput = bDisable; }
	void setPrefix(const OdString& prefix) { m_Prefix = prefix; }
	OdHatchPatternManager* patternManager();
	OdDbDatabasePtr readFile(const OdChar* fileName,
								bool bAllowCPConversion = false,
									bool bPartial = false,
										Oda::FileShareMode shmode = Oda::kShareDenyNo,
											const OdPassword& password = OdPassword());

  // OdDbAuditInfo overrides
	void printError(const OdChar* strName, 
						const OdChar* strValue, 
							const OdChar* strValidation, 
								const OdChar* strDefaultValue);
	void printInfo (const OdChar* strInfo);
	const OdDbAuditInfo::MsgInfo& getLastInfo();
	void setLastInfo(OdDbAuditInfo::MsgInfo &miLast);
	OdString findFile(const char* pcFilename, OdDbDatabase* pDb = NULL, OdDbHostAppServices::FindFileHint hint = kDefault);
	stringArray summaryList;
	stringArray errorList;

	DD_USING(OdDbHostAppServices::readFile);
};

#endif

