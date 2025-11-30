
#if defined(_MSC_VER) && defined(_WIN32)
#include <windows.h>
#endif

#include "db.h"

#define STL_USING_IOSTREAM
#include "OdaSTL.h"
#define  STD(a)  std:: a


#include "OdaCommon.h"
#include "OdToolKit.h"
#include "OdString.h"
#include "ExHostAppServices.h"
#include "Gs/Gs.h"
#include "DbAudit.h"
#include "Gi/TtfDescriptor.h"

ExHostAppServices::ExHostAppServices() 
                 : m_disableOutput(false)
{
}

OdHatchPatternManager* ExHostAppServices::patternManager()
{
	if(m_patternManager.isNull())
	{
		m_patternManager = OdHatchPatternManager::createObject();
		m_patternManager->setApplicationService(this);
	}
	return m_patternManager.get();
}

OdDbHostAppProgressMeter* ExHostAppServices::newProgressMeter()
{
	return this;
}

void ExHostAppServices::releaseProgressMeter(OdDbHostAppProgressMeter* /*pMeter*/)
{
}

void ExHostAppServices::DwgCollection::goodbye(const OdDbDatabase* pDb)
{
	std::map<OdString, OdDbDatabase*>::iterator iter = m_loadedDwgs.begin(), end = m_loadedDwgs.end();
	while(iter!=end)
	{
		if(iter->second==pDb)
		{
			m_loadedDwgs.erase(iter);
			break;
		}
    ++iter;
	}
}

OdDbDatabase* ExHostAppServices::DwgCollection::lookUp(const OdString& keyFileName)
{
	std::map<OdString, OdDbDatabase*>::iterator iter = m_loadedDwgs.find(keyFileName);
	if(iter!=m_loadedDwgs.end())
	{
		return iter->second;
	}
	return 0;
}

void ExHostAppServices::DwgCollection::add(OdDbDatabase* pDb)
{
	ODA_ASSERT(lookUp(pDb->getFilename())==0);
	m_loadedDwgs.insert(std::map<OdString, OdDbDatabase*>::value_type(pDb->getFilename(), pDb));
	pDb->addReactor(this);
}

OdDbDatabasePtr ExHostAppServices::readFile(const OdChar* fileName,
    bool bAllowCPConversion, bool bPartial, Oda::FileShareMode shmode, const OdPassword& password)
{
	OdDbDatabasePtr pRes = m_dwgCollection.lookUp(fileName);
	if(pRes.isNull())
	{
		pRes = OdDbHostAppServices::readFile(fileName, bAllowCPConversion, bPartial, shmode, password);
		m_dwgCollection.add(pRes.get());
	}
	return pRes;
}


void ExHostAppServices::start(const char* /*displayString*/)
{
	if (!m_disableOutput)
	{
#ifndef _WIN32_WCE
		STD(cout) << m_Prefix.c_str() << "Load Started ------------------------" << STD(endl);
#endif
	}
}

void ExHostAppServices::stop()
{
	if (!m_disableOutput)
	{
#ifndef _WIN32_WCE
		STD(cout) << m_Prefix.c_str() << "Load Completed------------------------" << STD(endl);
#endif
	}
}

void ExHostAppServices::meterProgress()
{
	m_MeterCurrent++;
	char temp[100];
	if (!m_disableOutput)
	{
		double  f1, f2;

		f1 = (double)m_MeterCurrent / m_MeterLimit * 100;
		f2 = (double)m_MeterOld / m_MeterLimit * 100;
		if ((f1 - f2) > 0.7)
		{
			sprintf(temp, "%2.2lf%%", f1);
#ifndef _WIN32_WCE
			STD(cout) << m_Prefix.c_str() << "Progress: " << temp << STD(endl);
#endif
			m_MeterOld = m_MeterCurrent;
		}
	}
}

void ExHostAppServices::setLimit(int max)
{
	m_MeterLimit = max;
	m_MeterCurrent = 0;
	m_MeterOld = 0;
	if (!m_disableOutput)
	{
#ifndef _WIN32_WCE
		STD(cout) << m_Prefix.c_str() << "Meter Limit: " << max << STD(endl);
#endif
	}
}


bool ExHostAppServices::ttfFileNameByDescriptor(const OdTtfDescriptor& descr, OdString& fileName)
{
#if defined(_MSC_VER) && defined(_WIN32) && !defined(_WIN32_WCE)
	OdString sNonExactMatched;

	OdString sFaceName = descr.typeface();

	OSVERSIONINFO os;
	os.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
	::GetVersionEx(&os);
	OdString sName;
	sName.format("Software\\Microsoft\\Windows%s\\CurrentVersion\\",
					(os.dwPlatformId & VER_PLATFORM_WIN32_NT)!=0 ? " nt" : "");

	LONG nRes;
	OdString sValueName;
	HKEY hFontSubstitutes;
	OdChar* lpData;
	DWORD nDataSize;
	nRes = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, sName + "FontSubstitutes\\", 0, KEY_READ, &hFontSubstitutes);
	if (hFontSubstitutes)
	{
		sValueName.format("%s,%d", descr.typeface(), descr.charSet());
		nRes = ::RegQueryValueEx(hFontSubstitutes, sValueName, NULL, NULL, NULL, &nDataSize);
		if(nRes == ERROR_SUCCESS)
		{
			lpData = sFaceName.getBuffer(nDataSize);
			nRes = ::RegQueryValueEx(hFontSubstitutes, sValueName, NULL, NULL, (LPBYTE)lpData, &nDataSize);
			sFaceName.releaseBuffer();
			int n = sFaceName.find(',');
			if(n>0)
			{
				sFaceName = sFaceName.left(n);
			}
		}
		nRes = ::RegCloseKey(hFontSubstitutes);
	}

	HKEY hFonts;
	::RegOpenKeyEx(HKEY_LOCAL_MACHINE, sName + "Fonts\\", 0, KEY_READ, &hFonts);
	if (hFonts)
	{
		DWORD nIndex = 0;
		DWORD nValNameSize = 20;
		nDataSize = max(fileName.getAllocLength(), 20);
		int n_BT = sFaceName.replace(" BT", " ");
		for(;;)
		{
			do
			{
				OdChar* lpValName = sValueName.getBuffer(nValNameSize);
				lpData = fileName.getBuffer(nDataSize);
				nRes = ::RegEnumValue(hFonts, nIndex, lpValName, &nValNameSize, NULL, NULL, (LPBYTE)lpData, &nDataSize);
				sValueName.releaseBuffer();
				fileName.releaseBuffer();
				if(nRes == ERROR_MORE_DATA)
				{
					nValNameSize += 20;
				}
			}

			while(nRes == ERROR_MORE_DATA);

			if(nRes==ERROR_SUCCESS)
			{
				++nIndex;
				nValNameSize = sValueName.getAllocLength();
				nDataSize = fileName.getAllocLength();
				if(sValueName.replace("(TrueType)", "")) // is TrueType font?
				{
					if(sValueName.replace(" BT", "")==n_BT)
					{
						bool bBold = false;
						if(n_BT)
						{
							sValueName.replace(" Extra Bold ", " XBd ");
						}
						else
						{
							if(sValueName.find(" Extra Bold ")==-1)
								bBold = (sValueName.replace(" Bold ", " ")!=0);
						}
            
						bool bItalic = (sValueName.replace(" Italic ", " ")!=0);
						sValueName.remove(' ');
						sFaceName.remove(' ');
						if(sValueName==sFaceName)
						{
							sNonExactMatched = fileName;
							if(descr.isBold()==bBold && descr.isItalic()==bItalic)
							break;
						}
						if ( sValueName.find(sFaceName+"&") != -1 ||
									sValueName.find("&"+sFaceName) != -1 )
						{
							sNonExactMatched = fileName;
							break;
						}
					}
				}
			}
			else
			{
				fileName = sNonExactMatched;
				break;
			}
		}
		::RegCloseKey(hFonts);
	}

	return (!fileName.isEmpty());
#else
	return false;
#endif
}

void ExHostAppServices::setLastInfo(OdDbAuditInfo::MsgInfo &miLast)
{
	 OdDbAuditInfo::setLastInfo(miLast);
	return;
}

void ExHostAppServices::printInfo(const char* strInfo)
{
	  OdDbAuditInfo::MsgInfo  miTmp;

	  miTmp.bIsError = false;
	  miTmp.strName = strInfo;
	  miTmp.strDefaultValue = "";
	  miTmp.strValue = "";
	  miTmp.strValidation = "";
	  setLastInfo(miTmp);
  
	  summaryList.insert(summaryList.end(), strInfo);
	  return;
}

void ExHostAppServices::printError(const OdChar* strName,
                               const OdChar* strValue, 
                               const OdChar* strValidation,
                               const OdChar* strDefaultValue)
{
	OdDbAuditInfo::MsgInfo  miTmp;

	miTmp.bIsError = true;
	miTmp.strName = strName;
	miTmp.strDefaultValue = strDefaultValue;
	miTmp.strValue = strValue;
	miTmp.strValidation = strValidation;
	setLastInfo(miTmp);

	OdString strLine;
	if (strName)
	{
		strLine += strName;
		strLine += ", ";
	}

	if (strValue)
	{
		strLine += strValue;
		strLine += ", ";
	}

	if (strValidation)
	{
		strLine += "Validation: <";
		strLine += strValidation;
		strLine += ">, ";
	}

	if (strDefaultValue)
	{
		if (fixErrors())
			strLine += "Replaced by: <";
		else
			strLine += "Devault value is: < ";
		strLine += strDefaultValue;
		strLine += ">.";
	}

	  errorList.insert(errorList.end(), strLine);
}

const ExHostAppServices::MsgInfo& ExHostAppServices::getLastInfo()
{
	return OdDbAuditInfo::getLastInfo(); 
}

OdString ExHostAppServices::findFile(const char* pcFilename, OdDbDatabase* pDb, OdDbHostAppServices::FindFileHint hint)
{
	char qualifiedName[DB_PATHNAMESZ];

	// Use icad.fmp to map the file for us.
	char* tmp = db_usefontmap((char*)pcFilename);
	OdString tmpName(tmp);
	delete [] tmp;

	if (tmpName.isEmpty())
	{
		if(OdString(pcFilename).iCompare("acad.lin") == 0)
			tmpName = "icad.lin";
		else if (OdString(pcFilename).iCompare("acadiso.lin") == 0)
			tmpName = "icadiso.lin";
		else if (OdString(pcFilename).iCompare("acad.pat") == 0)
			tmpName = "icad.pat";
		else if (OdString(pcFilename).iCompare("acadiso.pat") == 0)
			tmpName = "icadiso.pat";
		else if (OdString(pcFilename).iCompare("acad.unt") == 0)
			tmpName = "icad.unt";
		else
		{
			return OdString();
		}
	}

	
	if(ic_findinpath(NULL, tmpName, qualifiedName, DB_PATHNAMESZ) == RTNORM)
	{			
		return OdString(qualifiedName);
	}

	return OdString();
}
