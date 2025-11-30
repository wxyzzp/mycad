#include "dwgcodepage.h"

typedef struct {
	int	nCodePage;
	char* caCodePage;
} CodePageTbl;

static CodePageTbl 	nTbl[] = {
	{  1252,	"undefined"},	// AutoCAD raise error
	{  1252,	"ascii"},		// less than $80
	{  1252,	"iso8859_1"},	// Western Europe(Unix) $20-$FF,%%c,%%d,%%p
	{ 28592,	"iso8859_2"},	// Central Europe(Unix)
	{ 28593,	"iso8859_3"},	// Eastern Europe(Unix)
	{ 28594,	"iso8859_4"},	// Baltic(Unix)
	{ 28595,	"iso8859_5"},	// Cylylic(Unix)
	{ 28596,	"iso8859_6"},	// Arabic(Unix)
	{ 28597,	"iso8859_7"},	// Greek(Unix)
	{  1255,	"iso8859_8"},	// Hebraw(Unix)
	{ 28599,	"iso8859_9"},	// Turkish(Unix)
	{	437,	"dos437"	},	// DOS USA
	{	850,	"dos850"	},	// DOS Western Europe
	{	852,	"dos852"	},	// DOS Eastern Europe
	{	855,	"dos855"	},	// IBM Russian
	{	857,	"dos857"	},	// IBM Turkish
	{	860,	"dos860"	},	// DOS Portuguese
	{	861,	"dos861"	},	// DOS Icelandic
	{	863,	"dos863"	},	// DOS Canadian French
	{	864,	"dos864"	},	// DOS Arabic
	{	865,	"dos865"	},	// DOS Norwegian
	{	869,	"dos869"	},	// DOS Greek
	{	932,	"dos932"	},	// DOS Japanese
	{ 10001,	"mac-roman"},	// Mac
	{	950,	"big5"	},		// DOS Traditional Chinese
	{	949,	"ksc5601"},		// Korean Wansung
	{  1361,	"johab"},		// Korean Johab
	{	866,	"dos866"},		// DOS Russian
	{	936,	"gb2312"},		// DOS Simplified Chinese
	{  1250,	"ansi_1250"},	// Win Eastan Europe
	{  1251,	"ansi_1251"},	// Win Russian
	{  1252,	"ansi_1252"},	// Win Western Europe(ANSI)
	{  1253,	"ansi_1253"},	// Win Greek
	{  1254,	"ansi_1254"},	// Win Turkish
	{  1255,	"ansi_1255"},	// Win Hebraw
	{  1256,	"ansi_1256"},	// Win Arabic
	{  1257,	"ansi_1257"},	// Win Baltic
	{	874,	"ansi_874"},	// Win Thai
	{	932,	"ansi_932"},	// Win Japanese
	{	936,	"ansi_936"},	// Win Simplified Chinese GB
	{	949,	"ansi_949"},	// Win Korean Wansung
	{	950,	"ansi_950"},	// Win Tradissional Chinese big5
	{  1361,	"ansi_1361"},	// Win Korean Johab
	{  1200,	"ansi_1200"},	// Unicode (reserved)
	{  1258,	"ansi_1258"}	// Win Vietnamese (reserved)
};

DB_CLASS
int
get_dwgcodepage(			// R: code page number
	db_drawing* 	pDrw	// I: drawing to get dwgcodepage
)
{
	resbuf	rb;
	UINT	dwg_code_page = 0;

	if(db_getvar(NULL, DB_QDWGCODEPAGE , &rb, pDrw, NULL, NULL) != RTNORM)
		ASSERT(false);

	char *ptr = rb.resval.rstring;
	if( strnicmp( ptr, "ansi_", 5 ) == 0 ){
		dwg_code_page = _ttoi(ptr+5);
	}else if( strnicmp( ptr, "dos", 3 ) == 0 ){
		dwg_code_page = _ttoi(ptr+3);
	}else{
		int i, n;
		n = sizeof(nTbl)/sizeof(CodePageTbl);
		for(i=0;i<n;i++){
			if( stricmp( ptr, nTbl[i].caCodePage) == 0 ){
				dwg_code_page = nTbl[i].nCodePage;
				break;
			}
		}
	}

	delete [] rb.resval.rstring;
	rb.resval.rstring = NULL;

	return dwg_code_page;
} // get_dwgcodepage()

