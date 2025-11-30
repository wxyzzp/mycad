#pragma warning (disable : 4786)
#include "icadlib.h"
#include "TTFInfoGetter.h"
#include <string>
#include <map>

typedef std::multimap<std::string, char**> CFileToLocal;
struct ipair
{
	std::string first;
	char** second;
	ipair(): second(NULL){}
	ipair(const char* _first, char** _second): first(_first), second(_second){}
};
typedef std::vector<ipair> CGtoL;
typedef std::map<long, CGtoL*> CGtoLMap;

class CGToLInfo
{
public:
	CFileToLocal m_fileToLocal;
	CGtoLMap m_globalToLocal;
	void clear()
	{
		m_fileToLocal.clear();
		CGtoLMap::iterator it = m_globalToLocal.begin();
		for(; it != m_globalToLocal.end(); ++it)
			delete it->second;
		m_globalToLocal.clear();
	}
	~CGToLInfo() { clear(); }
};

CTTFInfoGetter CTTFInfoGetter::s_theGetter;

CTTFInfoGetter::CTTFInfoGetter(): m_bResolveLocalNames(false), m_gtol(NULL)
{
	m_fontsInfo.reserve(10);
}

CTTFInfoGetter::~CTTFInfoGetter()
{
	delete m_gtol;
}

// returns the size of m_fontsInfo
int CTTFInfoGetter::fillFontsInfo(LPCTSTR pTypefaceName /*= NULL*/, BYTE charSet /*= DEFAULT_CHARSET*/, HDC hDC /*= NULL*/)
{
	// performance improvement: don't enum fonts if pTypefaceName and charSet are the same as in previous call of the function;
	// NOTE, font structs in m_fontsInfo shouldn't change between fillFontsInfo calls!
	static LOGFONT	lf = {0L, 0L, 0L, 0L, 0L, 0, 0, 0, 0, 0, 0, 0, 0, ""/*DNT*/};
	if(lf.lfCharSet == charSet && ((!pTypefaceName && !*lf.lfFaceName) || (pTypefaceName && !_tcscmp(pTypefaceName, lf.lfFaceName))))
	{
		if(m_fontsInfo.size())
			return m_fontsInfo.size();
	}
	else
	{
		lf.lfCharSet = charSet;
		if(pTypefaceName)
			_tcscpy(lf.lfFaceName, pTypefaceName);
		else
			*lf.lfFaceName = 0;
	}

	m_fontsInfo.clear();

	bool	bGetHere = false;
	if(!hDC)
	{
		hDC = GetDC(NULL);
		bGetHere = true;
	}

	EnumFontFamiliesEx(hDC, &lf, (FONTENUMPROC)enumProc, (LPARAM)this, 0);

	if(bGetHere)
		ReleaseDC(NULL, hDC);

	return m_fontsInfo.size();
}

int CALLBACK CTTFInfoGetter::enumProc
(
 ENUMLOGFONTEX*		pLogFont,
 NEWTEXTMETRICEX*	pTextMetric,
 DWORD				fontType,
 LPARAM				lParam
)
{
	if(fontType & TRUETYPE_FONTTYPE)
		((CTTFInfoGetter*)lParam)->m_fontsInfo.push_back(*pLogFont);
	return 1;
}

// EBATECH(CNBR) Notice: getFontFileName() is ttf file only.
int CTTFInfoGetter::getFontFileName(char* fileName, char* fontName, long styleNumber /*= ACAD_DEFAULTSTYLENUMBER*/, HDC hDC /*= NULL*/)
{
	// find out if the new font is a TrueType font
	HKEY hKey;
	char szValue[IC_ACADBUF];
	DWORD dwType, dwStrSize = IC_ACADBUF;
	char pathName[IC_ACADBUF];

	// EBATECH(CNBR) -[ vertical font
	bool vertical = false;
	if( *fontName == '@' ){
		vertical = true;
		++fontName;
	}
	// EBATECH(CNBR) ]-
	// if we have a TrueType font, get the actual font name from
	// the registry (e.g., arialbd.ttf)
	OSVERSIONINFO osVer;
	osVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osVer);
	strcpy(pathName, osVer.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS ?
					"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Fonts"/*DNT*/ :
					"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts"/*DNT*/);
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, pathName, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
		return 0;

	strcpy(pathName,fontName);
	strcat(pathName," (TrueType)"/*DNT*/);
	if(RegQueryValueEx(hKey, pathName, NULL, &dwType, (LPBYTE)szValue, &dwStrSize) != ERROR_SUCCESS)
	{
		if(!fillFontsInfo(fontName, charSetByStyleNum(styleNumber), hDC))
		{
			fillFontsInfo(fontName, DEFAULT_CHARSET, hDC);
			if(!m_fontsInfo.size())
			{
				RegCloseKey(hKey);
				return 0;
			}
		}
		strcpy(pathName, (char*)m_fontsInfo.at(0).elfFullName);
		strcat(pathName, " (TrueType)"/*DNT*/);
		if(RegQueryValueEx(hKey, pathName, NULL, &dwType, (LPBYTE)szValue, &dwStrSize) != ERROR_SUCCESS)
		{
			RegCloseKey(hKey);
			return 0;
		}
	}
	// EBATECH(CNBR) -[ vertical font
	if( vertical == true ){
		*fileName = '@';
		++fileName;
	}
	// EBATECH(CNBR) ]-
	strcpy(fileName, szValue);
	RegCloseKey(hKey);
	return 1;
}

long CTTFInfoGetter::styleNumByFontParams(LONG weight, BYTE italic, BYTE pitchAndFamily, BYTE charSet )
{
	long	sn = pitchAndFamily | (0x0000FF00 & ((long)charSet << 8));	// EBATECH(CNBR) Add charSet

	if(italic)
		sn |= ACAD_ITALIC;
	if(weight > FW_MEDIUM)
		sn |= ACAD_BOLD;
	return sn;
}

void CTTFInfoGetter::fontParamsByStyleNum(LONG& weight, BYTE& italic, BYTE& pitchAndFamily, BYTE& charSet, long styleNumber)
{
	weight = (styleNumber & ACAD_BOLD) ? FW_BOLD : FW_NORMAL;
	italic = (styleNumber & ACAD_ITALIC) ? 255 : 0;
	pitchAndFamily = styleNumber & 0x000000FF;
	charSet = charSetByStyleNum(styleNumber);	// EBATECH(CNBR) Add charSet
}

static int getLocalNameByStyle(CTTFInfoGetter& ig, long stylenumber, const std::string* aGlobalName, std::string* aLocalName, int nName, HDC hDc = NULL)
{
	int		nElem = nName;
	char	sGlobalName[IC_ACADBUF];
	bool	bGetHere = false;
	if(!hDc)
	{
		hDc = GetDC(NULL);
		bGetHere = true;
	}
	// enum all fonts when nobody gets.
	ig.fillFontsInfo(NULL, CTTFInfoGetter::charSetByStyleNum(stylenumber), hDc);
	// get global name
	for(int j = 0; nElem && j < ig.fontCount() ; ++j)
	{
		// get global name from TrueType font
		HFONT hFont;
		if(( hFont = CreateFontIndirect( &(ig.fontInfo(j).elfLogFont))) != NULL ){
			hFont = (HFONT)SelectObject(hDc, hFont);
			DWORD cbData;
			cbData = GetFontData(hDc, TTF_NAME_TABLE, 0, NULL, 0);
			char *lpData = new char [cbData];
			GetFontData(hDc, TTF_NAME_TABLE, 0, (LPVOID)lpData, cbData);

			ttfnameHeader *pHead;
			ttfnameRecord *pRec;
			LPWSTR pRet;
			LPSTR pRet2;
			int nSize;
			// Swap Byte
			pHead = (ttfnameHeader *)lpData;
			_swab( lpData, lpData, sizeof(ttfnameHeader));
			pRec = (ttfnameRecord *)((ttfnameHeader *)lpData + 1);
			_swab( (char*)pRec, (char*)pRec, sizeof(ttfnameRecord) * pHead->nRec );

			// Windows/Any LCID/Facename (UNICODE)
			for(int i = 0 ; nElem && i < pHead->nRec ; i++ )
			{
				if(!(pRec[i].PlatformID == PID_MICROSOFT && pRec[i].EncodingID == EID_UNICODE &&
					pRec[i].LocaleID == 0x0409 && pRec[i].NameID == NID_FACENAME))
					continue;
				// Find Same IDs
				pRet = (LPWSTR)&(lpData[(DWORD)(pHead->Offset1) + pRec[i].Offset2]);
				// Swap Byte
				_swab( (LPSTR)pRet, (LPSTR)pRet, pRec[i].nSize );
				// Get String Size
				pRet2 = (LPSTR)&(sGlobalName[0]);
				nSize = WideCharToMultiByte(CP_ACP, 0, pRet, pRec[i].nSize/2, pRet2, pRec[i].nSize/2, NULL, NULL );
				if( nSize <= 0 ){ nSize = 0; }
				*(pRet2+nSize) = '\0';
				// Compare Global Name
				for(int iName = 0; nElem && iName < nName; ++iName)
				{
					if(aLocalName[iName].length())
						continue;
					const int bVertical = aGlobalName[iName][0] == '@';
					if(strnicmp(pRet2, aGlobalName[iName].c_str() + bVertical,
						aGlobalName[iName].length() - bVertical))
						continue;
					aLocalName[iName] = ig.fontInfo(j).elfLogFont.lfFaceName;
					if(bVertical)
						aLocalName[iName] = "@" + aLocalName[iName];
					--nElem;
				}
			}

			delete [] lpData;
			hFont = (HFONT)SelectObject(hDc, hFont);
			DeleteObject(hFont);
		}
	}

	if(bGetHere)
		ReleaseDC(NULL, hDc);

	return nElem == 0;
}

int CTTFInfoGetter::getLocalNameByGlobalName(char* globalName, long stylenumber, char* localName )
{
	//CString sGlobalName;
	int success = 0;
	const int bVertical = (*globalName == '@');
	// Assume globalName and localName is same.
	if(fillFontsInfo(globalName + bVertical, charSetByStyleNum(stylenumber)))
	{
		if(bVertical){
			*localName = '@';
			++localName;
		}
		strcpy( localName, fontInfo(0).elfLogFont.lfFaceName );
		success = 1;
	}
	else
	{
		if(!isResolveLocalNames())
		{
			std::string global(globalName), local;
			success = getLocalNameByStyle(*this, stylenumber, &global, &local, 1);
			if(success == 1)
				strcpy(localName, local.c_str());
		}
	}
	m_fontsInfo.clear();
	return success;
}

int CTTFInfoGetter::getGlobalStyleByLocalName(char* localName, long& stylenumber, char* globalName )
{
	char sGlobalName[IC_ACADBUF];
	int i, j;
	int success = 0;
	bool vertical = false;
	HDC hDc;

	if( *localName == '@' ){
		vertical = true;
		++localName;
	}

	hDc = GetDC(NULL);
	if(!fillFontsInfo(localName, GetSystemCharSet(), hDc))
		fillFontsInfo(localName, DEFAULT_CHARSET, hDc );
	// get global name
	for(j = 0; j < m_fontsInfo.size() ; j++ )
	{
		// get global name from TrueType font
		HFONT hFont;
		if(( hFont = CreateFontIndirect( &(m_fontsInfo.at(j).elfLogFont))) != NULL ){
			hFont = (HFONT)SelectObject(hDc, hFont);
			DWORD cbData;
			cbData = GetFontData(hDc, TTF_NAME_TABLE, 0, NULL, 0);
			char *lpData;
			lpData = new char [cbData];
			GetFontData(hDc, TTF_NAME_TABLE, 0, (LPVOID)lpData, cbData);

			ttfnameHeader *pHead;
			ttfnameRecord *pRec;
			LPWSTR pRet;
			LPSTR pRet2;
			int nSize;
			// Swap Byte
			pHead = (ttfnameHeader *)lpData;
			_swab( lpData, lpData, sizeof(ttfnameHeader));
			pRec = (ttfnameRecord *)((ttfnameHeader *)lpData + 1);
			_swab( (char*)pRec, (char*)pRec, sizeof(ttfnameRecord) * pHead->nRec );

			// Windows/Any LCID/Facename (UNICODE)
			for(i = 0 ; i < pHead->nRec ; i++ ){
				if( pRec[i].PlatformID == PID_MICROSOFT &&
					pRec[i].EncodingID == EID_UNICODE &&
					pRec[i].LocaleID == 0x0409 &&
					pRec[i].NameID == NID_FACENAME ){
					// Find Same IDs
					pRet = (LPWSTR)&(lpData[(DWORD)(pHead->Offset1) + pRec[i].Offset2]);
					// Swap Byte
					_swab( (LPSTR)pRet, (LPSTR)pRet, pRec[i].nSize );
					// Get String Size
					pRet2 = (LPSTR)&sGlobalName[0];
					nSize = WideCharToMultiByte(CP_ACP, 0, pRet, pRec[i].nSize/2, pRet2, pRec[i].nSize/2, NULL, NULL );
					if( nSize <= 0 ){ nSize = 0; }
					*(pRet2+nSize) = '\0';
					success = 1;
					break;
				}
			}

			delete [] lpData;
			hFont = (HFONT)SelectObject(hDc, hFont);
			DeleteObject(hFont);
			if( success == 1 ){
				break;
			}
		}
	}
	ReleaseDC(NULL, hDc);
	// get global name and style number
	if( success == 1 ){
		// set vertical flag.
		if( vertical == true ){
			*globalName = '@';
			++globalName;
		}
		strcpy( globalName, sGlobalName );
		LOGFONT *lfp = &(m_fontsInfo.at(j).elfLogFont);
		stylenumber = styleNumByFontParams(lfp->lfWeight, lfp->lfItalic, lfp->lfPitchAndFamily, lfp->lfCharSet );
	}
	// if English name is lost.
	else if( m_fontsInfo.size() > 0 ){
		LOGFONT *lfp = &(m_fontsInfo.at(0).elfLogFont);
		strcpy( globalName, lfp->lfFaceName );
		stylenumber = styleNumByFontParams(lfp->lfWeight, lfp->lfItalic, lfp->lfPitchAndFamily, lfp->lfCharSet );
		success = 2;
	}
	m_fontsInfo.clear();
	return success;
}

int CTTFInfoGetter::makeFontFileName(char* fileName, char* fontName)
{
	strcpy(fontName,fileName);
	strcat(fontName,".ttf"/*DNT*/);
	return TRUE;
}

static bool getRegKey(HKEY& hKey)
{
	char pathName[IC_ACADBUF];
	OSVERSIONINFO osVer;
	osVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osVer);
	if(osVer.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
	{
		strcpy(pathName,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Fonts"/*DNT*/);
	}
	else
	{
		strcpy(pathName,"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts"/*DNT*/);
	}
	return RegOpenKeyEx(HKEY_LOCAL_MACHINE, pathName, 0, KEY_READ, &hKey) == ERROR_SUCCESS;
}

static bool getLocalName(const char* szValue, const TCHAR* valueName, const char* fontFile,
						 char* fontName)
{
	if(strnicmp(fontFile, szValue, IC_ACADBUF) == 0)
	{
		char pathName[IC_ACADBUF];
		strcpy(pathName, valueName);
		char *rptr;
		rptr = strstr( pathName, " (TrueType)"/*DNT*/);
		if( rptr != NULL )
			*rptr = '\0';
		// EBATECH(CNBR) -[ 2001/12/10 for TTC file
		if( rptr = strrchr( fontFile, '.' ))
		{
			if( strnicmp( rptr, ".ttc"/*DNT*/, 4 ) == 0 )
			{
				rptr = strstr( pathName, " &"/*DNT*/);
				if( rptr != NULL )
					*rptr = '\0';
				else
					return false;
			}
		}
		// EBATECH(CNBR) ]-
		strcpy(fontName, pathName);
		return true;
	}
	return false;
}

// EBATECH(CNBR) Notice: getTTFontNameByFile() is ttf file only.
// EBATECH(CNBR) If Extension is TTC, getTTFontNameByFile() choices first font.
int getTTFontNameByFile(char* fontName, char* fontFile)
{
	// find out if the new font is a TrueType font
	HKEY hKey;
	if(!getRegKey(hKey))
		return 0;

	TCHAR valueName[IC_ACADBUF];
	DWORD valueNameSize = IC_ACADBUF;
	DWORD dwIndex = 0;
	char szValue[IC_ACADBUF];
	DWORD dwType, dwStrSize = IC_ACADBUF;
	while(RegEnumValue(hKey, dwIndex, valueName, &valueNameSize, NULL, &dwType, (LPBYTE)szValue, &dwStrSize) == ERROR_SUCCESS)
	{
		if(getLocalName(szValue, valueName, fontFile, fontName))
		{
			RegCloseKey(hKey);
			return 1;
		}
		++dwIndex;
		valueNameSize = dwStrSize = IC_ACADBUF;
	}
	RegCloseKey(hKey);
	return 0;
}

void CTTFInfoGetter::addNameByFile(char* fontName, char** localName)
{
	if(!m_gtol)
		m_gtol = new CGToLInfo();
	m_gtol->m_fileToLocal.insert(CFileToLocal::value_type(strupr(fontName), localName));
}

void CTTFInfoGetter::addGlobalToLocal(long stylenumber, char* globalName, char** localName)
{
	if(!m_gtol)
		m_gtol = new CGToLInfo();
	CGtoLMap::iterator it = m_gtol->m_globalToLocal.find(stylenumber);
	CGtoL* ptr = NULL;
	if(it == m_gtol->m_globalToLocal.end())
	{
		ptr = new CGtoL;
		m_gtol->m_globalToLocal[stylenumber] = ptr;
	}
	else
		ptr = it->second;
	ptr->push_back(ipair(globalName, localName));
}

void CTTFInfoGetter::setResolveLocalNames(bool bOn)
{
	m_bResolveLocalNames = bOn;
	if(bOn)
	{
		if(m_gtol)
			m_gtol->clear();
	}
	else
	{
		delete m_gtol;
		m_gtol = NULL;
	}
}

void CTTFInfoGetter::resolveLocalNames()
{
	if(!m_gtol)
		return;

	HDC	hDc = GetDC(NULL);
	CGtoLMap::iterator itm = m_gtol->m_globalToLocal.begin();
	for(; itm != m_gtol->m_globalToLocal.end(); ++itm)
	{
		std::vector<std::string> aGlobal;
		std::vector<std::string> aLocal;
		for(int i = 0; i < itm->second->size(); ++i)
			aGlobal.push_back((*itm->second)[i].first);
		aLocal.resize(aGlobal.size());
		getLocalNameByStyle(*this, itm->first, &aGlobal[0], &aLocal[0], aLocal.size(), hDc);
		for(i = 0; i < aLocal.size(); ++i)
		{
			if(aLocal[i].length())
			{
				*(*itm->second)[i].second = new char[aLocal[i].length() + 1];
				strcpy(*(*itm->second)[i].second, aLocal[i].c_str());
			}
		}
	}
	ReleaseDC(NULL, hDc);

	for(CFileToLocal::iterator it1 = m_gtol->m_fileToLocal.begin(); it1 != m_gtol->m_fileToLocal.end(); )
	{
		if(*it1->second)
		{
			CFileToLocal::iterator it2 = it1;
			++it2;
			m_gtol->m_fileToLocal.erase(it1);
			it1 = it2;
		}
		else
			++it1;
	}

	// find out if the new font is a TrueType font
	HKEY hKey;
	if(!getRegKey(hKey))
	{
		setResolveLocalNames(false);
		return;
	}

	TCHAR valueName[IC_ACADBUF];
	DWORD valueNameSize = IC_ACADBUF;
	DWORD dwIndex = 0;
	char szValue[IC_ACADBUF];
	DWORD dwType, dwStrSize = IC_ACADBUF;
	while(m_gtol->m_fileToLocal.size() &&
		RegEnumValue(hKey, dwIndex, valueName, &valueNameSize, NULL, &dwType,
					 (LPBYTE)szValue, &dwStrSize) == ERROR_SUCCESS)
	{
		char upper[IC_ACADBUF];
		strcpy(upper, szValue);
		strupr(upper);
		std::pair <CFileToLocal::iterator, CFileToLocal::iterator> rng =
			m_gtol->m_fileToLocal.equal_range(upper);
		CFileToLocal::iterator it1;
		for(CFileToLocal::iterator it = rng.first; it != rng.second; it = it1)
		{
			it1 = it;
			it1++;
			char fontName[IC_ACADBUF];
			if(getLocalName(szValue, valueName, it->first.c_str(), fontName))
			{
				*it->second = new char[strlen(fontName) + 1];
				strcpy(*it->second, fontName);
				m_gtol->m_fileToLocal.erase(it);
			}
		}
		++dwIndex;
		valueNameSize = dwStrSize = IC_ACADBUF;
	}
	setResolveLocalNames(false);
	RegCloseKey(hKey);
	return;
}

/********************************************************************************
 * Author:	Dmitry Gavrilov
 * Purpose:	Obtain the current ANSI code-page identifier for the system
 *			and convert it to the character-set value.
 * Returns:	Current system character set.
 ********************************************************************************/
 /* This code move from GR/TrueTypeUtils.cpp EBATECH(CNBR) 2001-06-13 */
BYTE
GetSystemCharSet()
{
	switch(GetACP())
	{
		case  932 : return SHIFTJIS_CHARSET;
		case  949 :	return HANGUL_CHARSET;
		case  936 :	return GB2312_CHARSET;
		case  950 :	return CHINESEBIG5_CHARSET;
		case  1253:	return GREEK_CHARSET;
		case  1254:	return TURKISH_CHARSET;
		case  1255:	return HEBREW_CHARSET;
		case  1256:	return ARABIC_CHARSET;
		case  1257:	return BALTIC_CHARSET;
		case  1251:	return RUSSIAN_CHARSET;
		case  874 :	return THAI_CHARSET;
		case  1250:	return EASTEUROPE_CHARSET;
		default   :	return ANSI_CHARSET;
	}
}


/*DG - 5.5.2003*/// There are short TTF names, e.g. "Swis721 LtCn BT" (like listed in HKLM\SOFTWARE\Microsoft\Shared Tools\Panose
// and contained in LOGFONT::lfFaceName) and long ones, e.g. "Swiss 721 Light Condensed BT (TrueType)" (listed in
// HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Fonts and contained in ENUMLOGFONTEX::elfFullName).
// The following function is an example how long to short names conversion can be performed.
// Note, its performance is not good if used for several fonts in a loop.
#if 0

// performs in-place conversion from long TTF name to short
// note, clears m_fontsInfo
void CTTFInfoGetter::fullToShortFontName(char* pFontName)	// <=>
{
	fillFontsInfo(NULL, DEFAULT_CHARSET);

	// 1st, try to find the font among all main fonts (i.e. with main style, e.g. "Arial")
	int	i = m_fontsInfo.size();
	for( ; i--; )
		if(strisame(pFontName, (char*)m_fontsInfo[i].elfFullName))
		{
			strcpy(pFontName, m_fontsInfo[i].elfLogFont.lfFaceName);
			m_fontsInfo.clear();
			return;
		}

	// if we are here, then we should look over all styles of all fonts (e.g. "Arial Italic", "Arial Bold")
	LOGFONT lf;
	memset(&lf, 0, sizeof(lf));
	lf.lfCharSet = DEFAULT_CHARSET;
	HDC	hDC = GetDC(NULL);
	for(i = m_fontsInfo.size(); i--; )
	{
		strcpy(lf.lfFaceName, m_fontsInfo[i].elfLogFont.lfFaceName);
		if(!EnumFontFamiliesEx(hDC, &lf, (FONTENUMPROC)enumProc_search, (LPARAM)pFontName, 0))
			break;
	}
	ReleaseDC(NULL, hDC);
	m_fontsInfo.clear();
}

// declare this function in CTTFInfoGetter if necessary...
int CALLBACK CTTFInfoGetter::enumProc_search(ENUMLOGFONTEX *pLogFont,
	NEWTEXTMETRICEX *pTextMetric,
	DWORD fontType,
	LPARAM lParam)	// <=>
{
	if((fontType & TRUETYPE_FONTTYPE) && strisame((char*)pLogFont->elfFullName, (char*)lParam))
	{
		strcpy((char*)lParam, pLogFont->elfLogFont.lfFaceName);
		return 0;
	}
	return 1;
}

#endif
