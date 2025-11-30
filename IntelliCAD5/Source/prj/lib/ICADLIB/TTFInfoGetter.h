#ifndef __TTFInfoGetterH__
#define __TTFInfoGetterH__

#include <vector>
typedef std::vector<ENUMLOGFONTEX> CTTFInfoSet;


const long ACAD_DEFAULTSTYLENUMBER = 34L;
const long ACAD_BOLD = 0x02000000L;
const long ACAD_ITALIC = 0x01000000L;

class CGToLInfo;

/*-------------------------------------------------------------------------------------*//**
@author Denis Petrov

@description Helper class for getting information about TTFs
*//*--------------------------------------------------------------------------------------*/
class CTTFInfoGetter
{
public:
	CTTFInfoGetter();
	~CTTFInfoGetter();

	int fillFontsInfo(LPCTSTR pTypefaceName = NULL, BYTE charSet = DEFAULT_CHARSET, HDC hDC = NULL);

	int fontCount() const { return m_fontsInfo.size(); }
	const ENUMLOGFONTEX& fontInfo(int index) const { return m_fontsInfo.at(index); }

	int getFontFileName(char* fileName, char* fontName, long styleNumber = ACAD_DEFAULTSTYLENUMBER, HDC hDC = NULL);

	static CTTFInfoGetter& get() { return s_theGetter; }

	static long styleNumByFontParams(LONG weight, BYTE italic, BYTE pitchAndFamily, BYTE charSet);
	static void fontParamsByStyleNum(LONG& weight, BYTE& italic, BYTE& pitchAndFamily, BYTE& charSet, long styleNumber);
	static BYTE charSetByStyleNum(long styleNumber) { return (styleNumber >> 8) & 0x000000FF; }

	int getLocalNameByGlobalName(char* globalName, long stylenumber, char* localName );
	int getGlobalStyleByLocalName(char* localName, long& stylenumber, char* globalName );

	void setResolveLocalNames(bool bOn);
	bool isResolveLocalNames() { return m_bResolveLocalNames; }
	void resolveLocalNames();
	void addNameByFile(char* fontName, char** localName);
	void addGlobalToLocal(long stylenumber, char* globalName, char** localName);
	
	static int makeFontFileName(char* fileName, char* fontName);

protected:
	static CTTFInfoGetter s_theGetter;

	CTTFInfoSet	m_fontsInfo;
	bool		m_bResolveLocalNames;
	CGToLInfo*	m_gtol;

	static int CALLBACK enumProc(ENUMLOGFONTEX* pLogFont, NEWTEXTMETRICEX* pTextMetric, DWORD fontType, LPARAM lParam);
};

/*-------------------------------------------------------------------------------------*//**
@author Denis Petrov

@description Returns 1 if font name was found by font file succesefully.
*//*--------------------------------------------------------------------------------------*/
int getTTFontNameByFile(char* fontName, char* fontFile);

// EBATECH(CNBR) 2001.06.13 move from GR/TrueTypeUtils.h
BYTE	GetSystemCharSet();

/*-------------------------------------------------------------------------------------*//**
@author MASAMI, Chikahiro @ Ebatech

@description Convert Local Name to Global Name
*//*--------------------------------------------------------------------------------------*/
#define TTF_NAME_TABLE	0x656d616eL // 'name' Little Endian
#define PID_MICROSOFT	3			// Vender code = Microsoft
#define EID_UNICODE 	1			// Encoding ID = Unicode
#define NID_FACENAME	1			// Data TypeID = Facename

#pragma pack(push, 1)
struct ttfnameHeader{	// 'name' table header record data.
	USHORT	fontID;		// All fields order big endian.
	USHORT	nRec;		// Loop counter(Use)
	USHORT	Offset1;	// Data Range Offset byte(Use)
};

struct ttfnameRecord {	// 'name' table record
	USHORT	PlatformID;	// All fields order big endian.
	USHORT	EncodingID;
	USHORT	LocaleID;	// ex. 0x0409 = ENU, 0x0411 = JPN
	USHORT	NameID;		// Data type ex. 0=Copyright notice
	USHORT	nSize;		// Byte Size of Data
	USHORT	Offset2;	// Data Offset from Offset1
};
#pragma pack(pop)
#endif
