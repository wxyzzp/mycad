/*------------------------------------------------------------------------
$Workfile  gr_parsetext.cpp$
$Archive$
$Revision: 1.3 $
$Date: 2001/07/05 15:26:39 $
$Author    Vitaly Spirin$

$Contents: Parser that is used in TEXT and MTEXT objects$
$Log: gr_parsetext.cpp,v $
Revision 1.3  2001/07/05 15:26:39  alexanderv
impovements and fixes in TT text generation

Revision 1.5  2001/06/29 10:17:57  denisp
impovements and fixes in TT text generation

Revision 1.4  2001/03/27 11:12:18  denisp
changed to display symbol with code 216 for "%%c" sequence

Revision 1.3  2001/03/11 12:01:32  vitalys
dwgcodepage work changed

Revision 1.2  2001/01/10 10:35:52  vitalys
corect work with accent characters (use MB_PRECOMPOSED instead MB_COMPOSED in MultiByteToWideChar)

Revision 1.1  2000/12/27 13:43:33  vitalys
Fixed bugs concerning and lowlights localization.

------------------------------------------------------------------------*/

#include "gr.h"
#include "WinVer.h"
#include "sds.h"
#include "glyph.h"

long 
gr_parsetext(                       // R:  Unicode of character of control code
    _TCHAR**                cpp,    // IO: String of text that should be parsed
    UINT                    dwg_code_page,  // for converting from multibyte to Unicode
    db_fontlink::FontType   type,    // I:  Font type. May be: SHAPE_1_0, SHAPE_1_1, UNIFONT, BIGFONT, TRUETYPE
	TTF_table* pFontTable
) 
{
/*
  This is a parser.  It determines the fist character code (from %%, 
  \U+, or \M+n control sequences) for the string *cpp and assigns *cpp
  TO the next char. Character maybe: glyph and control code. Glyph may
  be present as _TCHAR or with special sequence.

**  Here are the current rules (based on testing ACAD); they aren't
**  perfect yet:
**
**      %% sequences:
**          Recognized following chars (case insensitive):
**              c : Diameter symbol             (0x2205)
**              d : Degree symbol               (0xB0)
**              o : Overscore toggle            (-2L)
**              p : Plus-or-minus symbol        (0xB1)
**              u : Underscore toggle           (-3L)
**            nnn : char with code nnn          (Unicode of this char)
**              ? : Anything else (including %) (That char)
**
**      \U+xxxx sequences (the 'U' is case insensitive):
**          All 4 chars of xxxx must be given (leading 0's, if necessary)
**          and must be valid hex digits.
**
**      \M+nxxxx (the 'M' is case insensitive):
**          Same as \U+xxxx (° n must be 1-4, but we are ignoring
**          its meaning for now). (not implemented yet)
**
**  Returns the UNICODE chr code or one of the following:
**       0L : Something specified chr 0 (%%0, \U+0000, \M+n0000),
**              or, somehow, this function got called with
**              *cpp pointing to the string-terminator.
**      -1L : Error (cpp==NULL)
**      -2L : Overscore  toggle
**      -3L : Underscore toggle
**
**      (The return value is a long so that negative codes can be used
**      for special purposes without conflicting with valid
**      unsign short UNICODE values.)
*/
    _TCHAR*     cp;
    _TCHAR*     hexp;
    _TCHAR      fuc1;
	short fi1;
    long rc;


    rc = 0L;
//
//    (*cpp)++;
//    return 0x9d49;
//
    if (cpp==NULL) 
    { 
        rc = -1L; 
        goto out; 
    }; // if

    cp=(*cpp);

    rc=*cp;  /* Default */

    if ( _tcsnccmp(cp, _T("%"), 1) == 0) {
        if (_tcsnccmp(_tcsinc(cp), _T("%"), 1) == 0 && _tcslen(cp) > 2) 
        {
            cp = _tcsinc(_tcsinc(cp));
            if (_tcsncicmp(cp, _T("C"), 1) == 0 )
            {
				if (type == db_fontlink::SHAPE_1_0)
					rc = 129;
				else if (type == db_fontlink::SHAPE_1_1)
					rc = 258;
					//rc = 0x2205; 
				else
				{
					rc = 0x2205;
					if(!pFontTable || !pFontTable->FindGlyph(rc))
						rc = 216;
				}
            }else

            if (_tcsncicmp(cp, _T("D"), 1) == 0 )
            {
				if (type == db_fontlink::SHAPE_1_0)
					rc = 127;
				else if (type == db_fontlink::SHAPE_1_1)
					rc = 256;
				else
					rc = 0x00B0; 
            }else

            if (_tcsncicmp(cp, _T("O"), 1) == 0 )
            {
                rc = -2L;
            }else

            if (_tcsncicmp(cp, _T("P"), 1) == 0 )
            {
				if (type == db_fontlink::SHAPE_1_0)
					rc = 128;
				else if (type == db_fontlink::SHAPE_1_1)
					rc = 257;
				else
					rc = 0x00B1; 
            }else

            if (_tcsncicmp(cp, _T("U"), 1) == 0 )
            {
                rc = -3L;
            }else
            {
                // Covers %%%, too.
                // Look for up to 3 decimal digits (nnn):
                for (fi1=0; fi1<3 && cp[fi1] != 0 && _istdigit(*cp); fi1++);
                TCHAR       char_code[2];
                wchar_t     wchar_code;

                if (fi1 != 0)
                { 
                    fuc1 = *_tcsninc(cp, fi1);
                    *_tcsninc(cp, fi1) = 0;
                    char_code[0] = atoi((char *)cp);
                    *_tcsninc(cp, fi1) = fuc1;
                    cp = _tcsninc(cp, fi1 - 1);
                } else
                {
                    char_code[0] = *cp;
                    if (strlen(cp) > 1)
                    {
                        char_code[1] = *(cp + 1);
                    };
                }; // if

                if (MultiByteToWideChar(GetACP(), MB_PRECOMPOSED, (_TCHAR*)&char_code, 
                        1, (wchar_t*)&wchar_code, sizeof(rc)) == 0 )
                {
                    assert(false);
                }; // if
                rc = wchar_code;
            }; // if
        }; // if
    } else if (_tcsnicmp(cp, _T("\\"), 1) == 0) {
        hexp=NULL;
        if (_tcsnicmp(_tcsinc(cp), _T("U+"), 2) == 0) {
			_TCHAR* begNum = _tcsninc(cp, 3);

			//Special symbols in SHAPE_1_0 files are stored at pos 127 - 129; in SHAPE_1_1 files they are
			//stored at 256 - 258 therefore I add 129 to the SHAPE_1_0 position to get the SHAPE_1_1 position.
			int addpos = 0;  
			if (type == db_fontlink::SHAPE_1_1)
				addpos += 129;
			
			if ((type == db_fontlink::SHAPE_1_0) || (type == db_fontlink::SHAPE_1_1)) 
            {
				if (_tcsnicmp(begNum, _T("2205"), 4) == 0) 
					rc = 129 + addpos;

				else if (_tcsnicmp(begNum, _T("00B0"), 4) == 0)
					rc = 127 + addpos;

				else if (_tcsnicmp(begNum, _T("00B1"), 4) == 0)
					rc = 128 + addpos;

				else
					hexp=_tcsninc(cp, 3);
			}else
            {
				if (_tcsnicmp(begNum, _T("2205"), 4) == 0)
				{
					if(!pFontTable || !pFontTable->FindGlyph(0x2205))
						rc = 216;
					cp = _tcsninc(cp, 6);
				}
				else
					hexp=_tcsninc(cp, 3);
            }; // if

            if (hexp != NULL) { // if it's odinary (not special) Unicode char
                for (fi1 = 0; fi1 < 4 && _istxdigit(hexp[fi1]); fi1++);
                if (fi1>=4) {
                    fuc1 = hexp[4]; 
                    hexp[4] = 0;
                    rc = _tcstol(hexp, NULL, 16);
                    hexp[4] = fuc1;
                    cp = _tcsninc(hexp, 3);
                }; // if
            }; // if

        } else if (_tcsnicmp(_tcsinc(cp), _T("M+"), 2) == 0) {
		    hexp=_tcsninc(cp, 3);
            if (_istxdigit(*hexp))
            {
                UINT    code_page;
                if (_tcsnicmp(hexp, _T("0"), 1) == 0) {
                    code_page = dwg_code_page;    // Current
                } else if (_tcsnicmp(hexp, _T("1"), 1) == 0) {
                    code_page = 932;    // Japanese
                } else if (_tcsnicmp(hexp, _T("2"), 1) == 0) {
                    code_page = 950;    // Traditional Chinese
                } else if (_tcsnicmp(hexp, _T("3"), 1) == 0) {
                    code_page = 949;    // Wansung (Korean)
                } else if (_tcsnicmp(hexp, _T("4"), 1) == 0) {
                    code_page = 1361;   // Johab
                } else if (_tcsnicmp(hexp, _T("5"), 1) == 0) {
                    code_page = 936;    // Simplified Chinese
                } else {
                    goto out;   // incorrect \M+ sequence.
                }; // if

                hexp = _tcsinc(hexp);

                char    multi_byte[2];


                for (fi1 = 0; fi1 < 2 && _istxdigit(hexp[fi1]); fi1++);
                if (fi1>=2) {
                    fuc1 = hexp[2]; 
                    hexp[2] = 0;
                    multi_byte[0] = _tcstol(hexp, NULL, 16);
                    hexp[2] = fuc1;
                    hexp = _tcsninc(hexp, 2);

                    for (fi1 = 0; fi1 < 2 && _istxdigit(hexp[fi1]); fi1++);
                    if (fi1>=2) {
                        fuc1 = hexp[2]; 
                        hexp[2] = 0;
                        multi_byte[1] = _tcstol(hexp, NULL, 16);
                        hexp[2] = fuc1;

                        wchar_t     wchar_code;

                        // character from "\M+nxxxx" to Unicode
                        if (MultiByteToWideChar(code_page, MB_PRECOMPOSED, 
                        multi_byte, 2, &wchar_code, sizeof(wchar_code)) == 0 )
                        {
                            cp = *cpp;
                            goto out;
                        }; // if
                        rc = wchar_code;
                        cp = _tcsinc(hexp);
                    }; // if
                }; // if

            }; // if

        }; // if (end of "\U+..", "\M+.." proceessing)

    }; // if (end of "%%" , "\\" proceesing)

out:
    if (cp == *cpp) // if the character is not special or control
    {
        wchar_t     wchar_code[2];
        
        // multibyte (or just ordinary ANSI) to Unicode


        if (MultiByteToWideChar(dwg_code_page, MB_PRECOMPOSED, cp, 2, 
                wchar_code, sizeof(wchar_code)) == 0 )
        {
            // if NLS files of dwg_code_page hasn't been installed
            if (MultiByteToWideChar(GetACP(), MB_PRECOMPOSED, cp, 2, 
                wchar_code, sizeof(wchar_code)) == 0 )
            {
//                OutPutError();
                assert(false);
                return -1L;
            }
        }; // if
        rc = wchar_code[0];
    }; // if

    cp = _tcsinc(cp);
    if (cpp!=NULL) *cpp=(char *)cp;
    return rc;
}
