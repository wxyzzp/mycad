/* File  : <DevDir>\source\prj\lib\gr\mtextgen.cpp
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 * Mtext specific functions
 */

#include "gr.h"/*DNT*/
#include <stack>
#include <vector>
#include "mtextgen.h"/*DNT*/
#include "DbMText.h"/*DNT*/
#include "gr_view.h"/*DNT*/
#include "textgen.h"/*DNT*/
#include "truetypeutils.h"/*DNT*/
#include "styletabrec.h"/*DNT*/
#include "TTFInfoGetter.h"/*DNT*/
#include "dwgcodepage.h"/*DNT*/
#include "gr_parsetext.h"/*DNT*/
#include "gr_wordwrap.h"/*DNT*/

#ifdef _DEBUG 
#undef DEBUG_NEW  // this is necessary if the file is MFC-dependent  
#define _CRTDBG_MAP_ALLOC 
#include "crtdbg.h"  
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__) 
#define new DEBUG_NEW 
#endif 
 
char	CMtextFragment::s_buffer[MTEXTPARSE_BUFSIZE] = "\0";
int		CMtextFragment::s_bufferLength = 0;

int
rotatePoint
(
 sds_point	destination,
 sds_point	source,
 sds_real	angleSin,
 sds_real	angleCos,
 char		axis /*= 'Z'*/
)
{
	assert(destination != source);

	switch(axis)
	{
	case 'Z' :
	case 'z' :
		destination[0] = angleCos * source[0] - angleSin * source[1];
		destination[1] = angleSin * source[0] + angleCos * source[1];
		destination[2] = source[2];
		break;

	case 'Y' :
	case 'y' :
		destination[0] = angleCos * source[0] + angleSin * source[2];
		destination[1] = source[1];
		destination[2] = - angleSin * source[0] + angleCos * source[2];
		break;

	case 'X' :
	case 'x' :
		destination[0] = source[0];
		destination[1] = angleCos * source[1] - angleSin * source[2];
		destination[2] = angleSin * source[1] + angleCos * source[2];
		break;

	default :
		assert(false);
	}

	return 1;
}

/*-------------------------------------------------------------------------*//**
Gets font name and font modifiers from mtext string.

@author Denis Petrov
@param pString => pointer to char in mtext string
@param length => length of mtext string from pString
@param textStyle <= structure to store common text fragment formatting parameters
@param pDrawing => pointer to db_drawing to find font in
@param hDC => handle of device context to find TTF
@return length of parsed chars for success, 0 for error
*//*--------------------------------------------------------------------------*/
int
getFontName
(
 char*					pString,
 int					length,
 CTextFragmentStyle&	textStyle,
 db_drawing*			pDrawing,
 HDC					hDC
)
{
	int		i = 0;
	char	lastSymbol,
			fontName[IC_ACADBUF],
			fontFileName[IC_ACADBUF];
	long	stylenumber = 0L;

	while(i < length && pString[i] != ';' && pString[i] != '|')
		++i;

	lastSymbol = pString[i];
	pString[i] = 0;
	strcpy(fontName, pString);

	pString[i] = lastSymbol;
	// no font name
	if(fontName[0] == '\0')
		return 0;

	unsigned int	modifier;
	// \f<font family>|b0|i0|c999|p999;
	if(pString[i] == '|')
	{
		++i;
		while(i < length && pString[i] != ';')
		{
			switch(pString[i])
			{
			case 'b' :
				modifier = CTextFragmentStyle::eBOLD;
				stylenumber |= (( (long)atoi(&pString[i+1]) << 25 ) & STYLENUMBER_BOLD);
				break;

			case 'i' :
				modifier = CTextFragmentStyle::eITALIC;
				stylenumber |= (( (long)atoi(&pString[i+1]) << 24 ) & STYLENUMBER_ITALIC);
				break;

			case 'c' :
				modifier = CTextFragmentStyle::eUNKNOWN;
				stylenumber |= (( (long)atoi(&pString[i+1]) << 8 ) & STYLENUMBER_CHAR_MASK);
				break;

			case 'p' :
				modifier = CTextFragmentStyle::eUNKNOWN;
				stylenumber |= ( (long)atoi(&pString[i+1]) & STYLENUMBER_PITCH_MASK );
				break;

			default :
				assert(false);
				return 0;
			}

			++i;
			if(modifier != CTextFragmentStyle::eUNKNOWN)
			{
				if(pString[i] == '1')
					textStyle.m_modifiers |= modifier;
				else
					textStyle.m_modifiers &= ~modifier;

				++i;
				if(pString[i] == '|')
					++i;
				modifier = CTextFragmentStyle::eUNKNOWN;
			}
			else
			{
				while(i < length && pString[i] != '|' && pString[i] != ';')
					++i;
				if(pString[i] == '|')
					++i;
			}
		}

		if(!CTTFInfoGetter::get().getFontFileName(fontFileName, fontName, stylenumber, hDC))
		{
			char globalName[IC_ACADBUF];
			if( CTTFInfoGetter::get().getGlobalStyleByLocalName(fontName, stylenumber, globalName))
				CTTFInfoGetter::makeFontFileName(globalName, fontFileName);
			else
			{
				strcpy(fontName, "Arial"/*DNT*/);
				strcpy(fontFileName, "Arial.ttf"/*DNT*/);
				stylenumber = ACAD_DEFAULTSTYLENUMBER;
			}
		}
	}
	// \F<fontfilename>;
	else 
	{
		char localName[IC_ACADBUF];
		char globalName[IC_ACADBUF];
		// get fontfamilyname from registry
		if(getTTFontNameByFile(localName, fontName) != 0 && 
			CTTFInfoGetter::get().getGlobalStyleByLocalName(localName, stylenumber, globalName) != 0)
		{
			strcpy(fontFileName, fontName);
			strcpy(fontName, localName);
		}
		else
		{
			strcpy(fontFileName, fontName);
			strcpy(fontName, localName);
			stylenumber = DEFAULT_CHARSET << 8;
		}
	}

	//if(db_setfontusingmap(fontFileName, NULL, &textStyle.m_pFont, IC_ALLOW_ICAD_FNT, pDrawing) != 0)
	if(db_setfont(fontFileName, NULL, &textStyle.m_pFont, pDrawing) < 0)
	{
		assert(false);
		return 0;
	}

	// Modified Cybage VSB 30/07/2001 DD/MM/YY [
	// Reason: IntelliCAD crashes on missing font file or if font file
	// path is invalid in Tools->option->Fonts
	// Bug No 77817 from BugZilla
	if ( textStyle.m_pFont == NULL )
	{
		TCHAR fontName[] = "Icad.fnt";
		if(db_setfont(fontName, NULL, &textStyle.m_pFont, pDrawing) < 0)
		{
			assert(false);
			return 0;
		}
	}
	// Modified Cybage VSB 30/07/2001 DD/MM/YY ]

	if(textStyle.m_pFont->format == db_fontlink::TRUETYPE)
	{
		strcpy(textStyle.m_pFont->desc, fontName);
		textStyle.m_stylenumber = stylenumber;
	}
	return i;
}

/*-------------------------------------------------------------------------*//**
Gets numerical parameter value from mtext string.

@author Denis Petrov
@param pString => pointer to char in mtext string
@param length => length of mtext string from pString
@param value <= numerical value of parameter
@return length of parsed chars for success, 0 for error
*//*--------------------------------------------------------------------------*/
int
getValue
(
 char*		pString,
 int		length,
 sds_real&	value
)
{
	int		i = 0;
	char	lastSymbol;

	while(i < length && pString[i] != ';' && pString[i] != 'x' && pString[i] != 'X')
	{	
		if(pString[i] != '-' && pString[i] != '+' && pString[i] != '.' && !isdigit(pString[i]))
			break;
		++i;
	}

	lastSymbol = pString[i];
	pString[i] = 0;
	value = atof(pString);
	pString[i] = lastSymbol;
	return i;
}

/*-------------------------------------------------------------------------*//**
Parses string with control sequences into vector of CMtextFragment structures.

@author Denis Petrov
@param fragments <= vector of CMtextFragment structures
@param pMtext => DB object which string must be parsed
@param pDrawing => pointer to drawing
@return number of lines for success, 0 for error
*//*--------------------------------------------------------------------------*/
int
gr_mtextparse
(
 MTEXTFRAGMENTS_VECTOR&	fragments,
 db_mtext*				pMtext,
 db_drawing*			pDrawing,
 CDC*					pDC
)
{
	// MTEXT string with formatting control codes
	char*	pMtextString;
	int		mtextStringLength;

	pMtext->get_1(&pMtextString);
	if(!pMtextString)
		return 0;

	mtextStringLength = strlen(pMtextString);

	// pointer to MTEXT style object
	db_handitem*	pTextStyle = NULL;
	pMtext->get_7(&pTextStyle);
	if(!pTextStyle)
		return 0;

	// stack to store formatting style
	TEXTFRAGMENTSTYLE_STACK	stack;
	CMtextFragment			currentMtextFragment;

	// initialization of data
	int	mtextDirection;
	pMtext->get_72(&mtextDirection);
	if(mtextDirection == 3)	// top to bottom
		currentMtextFragment.m_style.vert = 1;

	// set initial color, it requires add last space
	pMtext->get_62(&currentMtextFragment.m_style.m_color);
	currentMtextFragment.m_style.m_lastspace = 1;

	// it seems what we don't need to calculate rotation because we obtain ECS already rotated
	//pMtext->get_50(&currentMtextFragment.m_style.m_rotation, pDrawing);
	currentMtextFragment.m_style.m_rotation = 0.0;
	pMtext->get_40(&currentMtextFragment.m_style.m_height);

	currentMtextFragment.m_style.m_pBigfont = pTextStyle->ret_bigfont();
	// m_pFont should set non-zero. Old fasion dwg sometimes p_mfont is zero and p_mBigfont is non-zero.
	currentMtextFragment.m_style.m_pFont = pTextStyle->ret_font()? pTextStyle->ret_font() : pTextStyle->ret_bigfont();
	if(mtextDirection == 5)
	{
		// direction by style
		pTextStyle->get_70(&mtextDirection);
		if(mtextDirection & IC_SHAPEFILE_VERTICAL)
			currentMtextFragment.m_style.vert = 1;
	}

	if(isEqualZeroWithin(currentMtextFragment.m_style.m_height))
		pTextStyle->get_40(&currentMtextFragment.m_style.m_height);

	pTextStyle->get_41(&currentMtextFragment.m_style.m_widthFactor);
	pTextStyle->get_50(&currentMtextFragment.m_style.m_obliqueAngle);

	// main parser declarations
	sds_real	paramValue;
	int			nLine = 1,
				currentMtextFragmentFirstChar = 6;

	CMtextFragment::s_buffer[0] = 0;
	CMtextFragment::s_bufferLength = 6;

	// go till the end of mtext string
	for(int i = 0; i < mtextStringLength; ++i)
	{
		if(CMtextFragment::s_bufferLength >= MTEXTPARSE_BUFSIZE)
		{
			assert(false);
			break;
		}

		switch(pMtextString[i])
		{
		case ' ' :
			CMtextFragment::s_buffer[CMtextFragment::s_bufferLength] = pMtextString[i];
			++CMtextFragment::s_bufferLength;
			++i;
			if(CMtextFragment::s_bufferLength > currentMtextFragmentFirstChar)
			{
				// have Mtext fragment with style, add it to vector
				CMtextFragment::s_buffer[CMtextFragment::s_bufferLength] = 0;
				++CMtextFragment::s_bufferLength;
				currentMtextFragment.m_firstChar = currentMtextFragmentFirstChar;
				currentMtextFragment.m_length = CMtextFragment::s_bufferLength - currentMtextFragmentFirstChar;
				CMtextFragment::s_bufferLength += 6;
				currentMtextFragmentFirstChar = CMtextFragment::s_bufferLength;
				currentMtextFragment.m_nLine = nLine;
				currentMtextFragment.m_fragmentType = CMtextFragment::EFragmentType::eDEFAULT;
				fragments.push_back(currentMtextFragment);
			}
			while(i < mtextStringLength && pMtextString[i] == ' ')
			{
				CMtextFragment::s_buffer[CMtextFragment::s_bufferLength] = pMtextString[i];
				++CMtextFragment::s_bufferLength;
				++i;
			}
			--i;
			break;

		case '\\' :
			// it's begin of control code or one of next symbols: { , }, '\'
			++i;
			if(pMtextString[i] == '{' || pMtextString[i] == '}' || pMtextString[i] == '\\')
			{
				CMtextFragment::s_buffer[CMtextFragment::s_bufferLength] = pMtextString[i];
				++CMtextFragment::s_bufferLength;
			}
			else
			{
				// it's begin of control code
				// don't create a new fragment for the following control codes
				if( CMtextFragment::s_bufferLength > currentMtextFragmentFirstChar &&
					pMtextString[i] != 'U' && pMtextString[i] != 'u' &&
					pMtextString[i] != 'M' && pMtextString[i] != 'm' && pMtextString[i] != '~')
				{
					// have Mtext fragment with style, add it to vector
					CMtextFragment::s_buffer[CMtextFragment::s_bufferLength] = 0;
					++CMtextFragment::s_bufferLength;
					currentMtextFragment.m_firstChar = currentMtextFragmentFirstChar;
					currentMtextFragment.m_length = CMtextFragment::s_bufferLength - currentMtextFragmentFirstChar;
					CMtextFragment::s_bufferLength += 6;
					currentMtextFragmentFirstChar = CMtextFragment::s_bufferLength;
					currentMtextFragment.m_nLine = nLine;
					currentMtextFragment.m_fragmentType = CMtextFragment::EFragmentType::eDEFAULT;
					fragments.push_back(currentMtextFragment);
				}
				switch(pMtextString[i])
				{
				case 'p' :
					/// KLUGE ALERT:
					// AutoCAD 2005 introduced a new mtext format code '\p'. Its a little
					// difficult to get full information on it, but it appears to be paragraph
					// tabs. It is unclear what the numbers are, but the format looks like this;
					// \pl0.899488,t8.38542;
					// We are too late in the release cycle to support this correctly, so we
					// are simply stripping it out. If we don't strip it out, the numeric part
					// including the coma and semicolon are displayed as part of the text and 
					// appears as garbage.
					// This will need to be revisited after we ship. MHB.
                    while((i < mtextStringLength) && pMtextString[i] != ';')
                    {
						i++;
                    }
					break;
				case 'P' :
					// wrap to next line
					++nLine;
					break;

				case 'O' :	// Start Overline
					currentMtextFragment.m_style.m_modifiers |= CTextFragmentStyle::EModifiers::eOVERLINED;
					break;

				case 'o' :	// End Overline
					currentMtextFragment.m_style.m_modifiers &= ~CTextFragmentStyle::EModifiers::eOVERLINED;
					break;

				case 'L' :	// Start Underline
					currentMtextFragment.m_style.m_modifiers |= CTextFragmentStyle::EModifiers::eUNDERLINED;
					break;

				case 'l' :	// End Underline
					currentMtextFragment.m_style.m_modifiers &= ~CTextFragmentStyle::EModifiers::eUNDERLINED;
					break;

				case 'c' :	// Color \c0..256; n=ACI(0 = By Block, 256 = By Layer)
				case 'C' :
					++i;
					i += getValue(pMtextString + i, mtextStringLength - i, paramValue);
					assert(pMtextString[i] == ';');
					if(paramValue >= DB_BBCOLOR && paramValue <= DB_BLCOLOR)
						currentMtextFragment.m_style.m_color = static_cast<int>(paramValue);
					break;

				case 'a' :	// Vertical Alignment \A0;Top \A1;Middle \A2;Bottom
				case 'A' :
					++i;
					i += getValue(pMtextString + i, mtextStringLength - i, paramValue);
					assert(pMtextString[i] == ';');
					if(paramValue >= 0.0 && paramValue <= 2.0)
						currentMtextFragment.m_style.m_alignment = static_cast<int>(paramValue);
					break;

				case 'h' :	// Height \h0.5; (absolute) \h0.5x; (relative)
				case 'H' :
					++i;
					i += getValue(pMtextString + i, mtextStringLength - i, paramValue);
					if(pMtextString[i] != ';' && pMtextString[i] != 'x' && pMtextString[i] != 'X')
						--i;
					if(pMtextString[i] == 'x' || pMtextString[i] == 'X')
					{
						if(paramValue > 0.0)
							currentMtextFragment.m_style.m_height *= paramValue;
						if(pMtextString[i + 1] == ';')
							++i;
					}
					else
						if(paramValue > 0.0)
							currentMtextFragment.m_style.m_height = paramValue;
					break;

				case 't' :	// Text Spacing
				case 'T' :
					++i;
					i += getValue(pMtextString + i, mtextStringLength - i, paramValue);
					assert(pMtextString[i] == ';');
					if(paramValue >= 0.25 && paramValue <= 4.0)
						currentMtextFragment.m_style.m_spaceFactor = paramValue;
					break;

				case 'q' :	// obliQue angle
				case 'Q' :
					++i;
					i += getValue(pMtextString + i, mtextStringLength - i, paramValue);
					assert(pMtextString[i] == ';');
					
					//BugZilla No. 78343; 28-04-2003 [
					paramValue = fmod(paramValue, 360.0);
					if((fabs(paramValue) - 85.0) > IC_ZRO && (fabs(paramValue) - 274.9) < IC_ZRO)
							paramValue = 0;
					//BugZilla No. 78343; 28-04-2003 ]

					// convert to radians
					currentMtextFragment.m_style.m_obliqueAngle = IC_PI * paramValue / 180.0;
					break;

				case 'w' :	// Width factor
				case 'W' :
					++i;
					i += getValue(pMtextString + i, mtextStringLength - i, paramValue);
					if(pMtextString[i] != ';' && pMtextString[i] != 'x' && pMtextString[i] != 'X')
						--i;
					else
						if((pMtextString[i] == 'x' || pMtextString[i] == 'X') && pMtextString[i + 1] == ';')
							++i;
					if(paramValue > 0.0)
						currentMtextFragment.m_style.m_widthFactor = paramValue;
					break;

				case 'U' :	// Unicode encoding
				case 'u' :
				case 'M' :	// Multibyte encoding
				case 'm' :
					// these symbols will be parsed in gr_textgen(), so don't touch
					CMtextFragment::s_buffer[CMtextFragment::s_bufferLength] = pMtextString[i - 1];
					++CMtextFragment::s_bufferLength;
					CMtextFragment::s_buffer[CMtextFragment::s_bufferLength] = pMtextString[i];
					++CMtextFragment::s_bufferLength;
					break;

				case 'f' :	// change Font
				case 'F' :
					++i;
					i += getFontName(pMtextString + i, mtextStringLength - i, currentMtextFragment.m_style, pDrawing, pDC->GetSafeHdc());
					break;

				case 's' :	// Stack text
				case 'S' :
					int	flag;
					flag = currentMtextFragment.m_style.vert;
					currentMtextFragment.m_style.vert = 0;
					++i;
					// EBATECH(CNBR) TODO: stack spliter '#' is added since AutoCAD 2000.
					while(i < mtextStringLength && pMtextString[i] != ';' && pMtextString[i] != '^' && pMtextString[i] != '/' && pMtextString[i] != '#')
					{
						if(pMtextString[i] == '\\')
							++i;
						// test for MBCS char
						if ((_MBC_LEAD == _mbbtype((unsigned char)pMtextString[i], 0)) &&
							(_MBC_TRAIL == _mbbtype((unsigned char)pMtextString[i +1], 1)))
						{
							CMtextFragment::s_buffer[CMtextFragment::s_bufferLength] = pMtextString[i];
							++CMtextFragment::s_bufferLength;
							++i;
							CMtextFragment::s_buffer[CMtextFragment::s_bufferLength] = pMtextString[i];
							++CMtextFragment::s_bufferLength;
						}
						else
						{
							CMtextFragment::s_buffer[CMtextFragment::s_bufferLength] = pMtextString[i];
							++CMtextFragment::s_bufferLength;
						}
						++i;
					}
					
					if(i >= mtextStringLength)
						break;

					// This handles the stacked fractions
					if(pMtextString[i] == '/')
						currentMtextFragment.m_style.m_modifiers |= CTextFragmentStyle::EModifiers::eUNDERLINED;
					// TO DO: This keeps diagonal stacked fractions from crashing IntelliCAD
					// but draws tem as stack and NOT diagonal fractions
					else if(pMtextString[i] == '#')
						currentMtextFragment.m_style.m_modifiers |= CTextFragmentStyle::EModifiers::eUNDERLINED;

					// form stacked fragment on this line
					if(CMtextFragment::s_bufferLength > currentMtextFragmentFirstChar)
					{
						// have Mtext fragment with style, add it to vector
						CMtextFragment::s_buffer[CMtextFragment::s_bufferLength] = 0;
						++CMtextFragment::s_bufferLength;
						currentMtextFragment.m_firstChar = currentMtextFragmentFirstChar;
						currentMtextFragment.m_length = CMtextFragment::s_bufferLength - currentMtextFragmentFirstChar;
						CMtextFragment::s_bufferLength += 6;
						currentMtextFragmentFirstChar = CMtextFragment::s_bufferLength;
						currentMtextFragment.m_nLine = nLine;
						currentMtextFragment.m_fragmentType = CMtextFragment::EFragmentType::eSTACKED;
						fragments.push_back(currentMtextFragment);
					}

					if(pMtextString[i] == '/')
					{
						currentMtextFragment.m_style.m_modifiers &= ~CTextFragmentStyle::EModifiers::eUNDERLINED;
						currentMtextFragment.m_style.m_modifiers |= CTextFragmentStyle::EModifiers::eOVERLINED;
					}
					// TO DO: This keeps diagonal stacked fractions from crashing IntelliCAD
					// but draws tem as stack and NOT diagonal fractions
					else if(pMtextString[i] == '#')
					{
						currentMtextFragment.m_style.m_modifiers &= ~CTextFragmentStyle::EModifiers::eUNDERLINED;
						currentMtextFragment.m_style.m_modifiers |= CTextFragmentStyle::EModifiers::eOVERLINED;
					}
					++i;

					while(i < mtextStringLength && pMtextString[i] != ';')
					{
						if(pMtextString[i] == '\\')
							++i;
						// test for MBCS char
						if ((_MBC_LEAD == _mbbtype((unsigned char)pMtextString[i], 0)) &&
							(_MBC_TRAIL == _mbbtype((unsigned char)pMtextString[i + 1], 1)))
						{
							CMtextFragment::s_buffer[CMtextFragment::s_bufferLength] = pMtextString[i];
							++CMtextFragment::s_bufferLength;
							++i;
							CMtextFragment::s_buffer[CMtextFragment::s_bufferLength] = pMtextString[i];
							++CMtextFragment::s_bufferLength;
						}
						else
						{
							CMtextFragment::s_buffer[CMtextFragment::s_bufferLength] = pMtextString[i];
							++CMtextFragment::s_bufferLength;
						}
						++i;
					}

					if(i >= mtextStringLength)
						break;

					if(CMtextFragment::s_bufferLength > currentMtextFragmentFirstChar)
					{
						// have Mtext fragment with style, add it to vector
						CMtextFragment::s_buffer[CMtextFragment::s_bufferLength] = 0;
						++CMtextFragment::s_bufferLength;
						currentMtextFragment.m_firstChar = currentMtextFragmentFirstChar;
						currentMtextFragment.m_length = CMtextFragment::s_bufferLength - currentMtextFragmentFirstChar;
						CMtextFragment::s_bufferLength += 6;
						currentMtextFragmentFirstChar = CMtextFragment::s_bufferLength;
						currentMtextFragment.m_nLine = nLine;
						currentMtextFragment.m_fragmentType = CMtextFragment::EFragmentType::eWITH_STACKED;
						fragments.push_back(currentMtextFragment);

						currentMtextFragment.m_style.m_modifiers &= ~CTextFragmentStyle::EModifiers::eOVERLINED;
					}

					// restore defult fragment type
					currentMtextFragment.m_fragmentType = CMtextFragment::EFragmentType::eDEFAULT;
					currentMtextFragment.m_style.vert = flag;
					break;

				case '~' :	// Non-breaking space
					CMtextFragment::s_buffer[CMtextFragment::s_bufferLength] = ' ';
					++CMtextFragment::s_bufferLength;
					break;

				default :	// General character
					if ((_MBC_LEAD == _mbbtype((unsigned char)pMtextString[i], 0)) &&
						(_MBC_TRAIL == _mbbtype((unsigned char)pMtextString[i + 1], 1)))
					{
						CMtextFragment::s_buffer[CMtextFragment::s_bufferLength] = pMtextString[i];
						++CMtextFragment::s_bufferLength;
						++i;
						CMtextFragment::s_buffer[CMtextFragment::s_bufferLength] = pMtextString[i];
						++CMtextFragment::s_bufferLength;
					}
					else
					{
						CMtextFragment::s_buffer[CMtextFragment::s_bufferLength] = pMtextString[i];
						++CMtextFragment::s_bufferLength;
					}
				}	// switch
			}	// control code case
			break;

		case '{' :	// Formating stack push
			if(CMtextFragment::s_bufferLength > currentMtextFragmentFirstChar)
			{
				// have Mtext fragment with style, add it to vector
				CMtextFragment::s_buffer[CMtextFragment::s_bufferLength] = 0;
				++CMtextFragment::s_bufferLength;
				currentMtextFragment.m_firstChar = currentMtextFragmentFirstChar;
				currentMtextFragment.m_length = CMtextFragment::s_bufferLength - currentMtextFragmentFirstChar;
				CMtextFragment::s_bufferLength += 6;
				currentMtextFragmentFirstChar = CMtextFragment::s_bufferLength;
				currentMtextFragment.m_nLine = nLine;
				currentMtextFragment.m_fragmentType = CMtextFragment::EFragmentType::eDEFAULT;
				fragments.push_back(currentMtextFragment);
			}
			stack.push(currentMtextFragment.m_style);
			break;

		case '}' :	// Formating stack pop
			if(stack.empty())
				// TODO: raise error
				assert(false);
			else
			{
				if(CMtextFragment::s_bufferLength > currentMtextFragmentFirstChar)
				{
					// have Mtext fragment with style, add it to vector
					CMtextFragment::s_buffer[CMtextFragment::s_bufferLength] = 0;
					++CMtextFragment::s_bufferLength;
					currentMtextFragment.m_firstChar = currentMtextFragmentFirstChar;
					currentMtextFragment.m_length = CMtextFragment::s_bufferLength - currentMtextFragmentFirstChar;
					CMtextFragment::s_bufferLength += 6;
					currentMtextFragmentFirstChar = CMtextFragment::s_bufferLength;
					currentMtextFragment.m_nLine = nLine;
					currentMtextFragment.m_fragmentType = CMtextFragment::EFragmentType::eDEFAULT;
					fragments.push_back(currentMtextFragment);
				}
				currentMtextFragment.m_style = stack.top();
				stack.pop();
			}
			break;

		default :
			// it's simple symbol
			// accumulate fragment
			if ((_MBC_LEAD == _mbbtype((unsigned char)pMtextString[i], 0)) &&
			  (_MBC_TRAIL == _mbbtype((unsigned char)pMtextString[i + 1], 1)))
			{
				CMtextFragment::s_buffer[CMtextFragment::s_bufferLength] = pMtextString[i];
				++CMtextFragment::s_bufferLength;
				++i;
				CMtextFragment::s_buffer[CMtextFragment::s_bufferLength] = pMtextString[i];
				++CMtextFragment::s_bufferLength;
			}
			else
			{
				CMtextFragment::s_buffer[CMtextFragment::s_bufferLength] = pMtextString[i];
				++CMtextFragment::s_bufferLength;
			}
		}	// switch
	}	// for(

	// form rest of string
	if(CMtextFragment::s_bufferLength > currentMtextFragmentFirstChar)
	{
		// have Mtext fragment with style, add it to vector
		CMtextFragment::s_buffer[CMtextFragment::s_bufferLength] = 0;
		++CMtextFragment::s_bufferLength;
		currentMtextFragment.m_firstChar = currentMtextFragmentFirstChar;
		currentMtextFragment.m_length = CMtextFragment::s_bufferLength - currentMtextFragmentFirstChar;
		currentMtextFragmentFirstChar = CMtextFragment::s_bufferLength;
		currentMtextFragment.m_nLine = nLine;
		currentMtextFragment.m_fragmentType = CMtextFragment::EFragmentType::eDEFAULT;
		fragments.push_back(currentMtextFragment);
	}

	if(fragments.empty())
		return 0;

	return nLine;
}

/*-------------------------------------------------------------------------*//**
Gets bounding box of given MTEXT fragment using gr_textgen().

@author Denis Petrov
@param text => temporary DB object to pass to gr_textgen()
@param textStyle => style of fragment
@param bmin <= minimum point of fragment's bounding box
@param bmax <= maximum point of fragment's bounding box
@param pView =>
@param pDrawing =>
@param pDC =>
@return 1 for success, 0 for error or allowed width or height less then real width or height
*//*--------------------------------------------------------------------------*/
int
getMtextFragmentBB
(
 db_text&				text,
 CTextFragmentStyle&	textStyle,
 sds_point&				bmin,
 sds_point&				bmax,
 gr_view*				pView,
 db_drawing*			pDrawing,
 CDC*					pDC
)
{
	int			result = 0;
	db_mtext*	pMtext = (db_mtext*)pView->ment;

	// save original style font
	db_styletabrec*	pStyle;
	pMtext->get_7((db_handitem**)&pStyle);

	// temporary set new font in style object
	db_fontlink*	pFont = pStyle->ret_font();

	long	fontStyle;
	char	fontFamily[IC_ACADBUF];

	if(pFont != textStyle.m_pFont)
	{
		pStyle->set_font(textStyle.m_pFont);
		// new font is TTF
		if(textStyle.m_pFont->format == db_fontlink::TRUETYPE)
		{
			if(pFont->format == db_fontlink::TRUETYPE)
			{
				// old font is also TTF
				// save font style
				pStyle->getFontFamily(fontFamily);
				pStyle->getFontStyleNumber(&fontStyle);
			}

			pStyle->setFontFamily(textStyle.m_pFont->desc, pDrawing);
			// EBATECH(CNBR) -[ use stylenumber member variable.
			// temporary set new font style number if it is TTF
			//if(textStyle.m_modifiers & CTextFragmentStyle::eITALIC)
			//	pStyle->setFontStyleNumber(textStyle.m_pFont->desc, ACAD_ITALIC, pDrawing);
			//
			//if(textStyle.m_modifiers & CTextFragmentStyle::eBOLD)
			//	pStyle->setFontStyleNumber(textStyle.m_pFont->desc, ACAD_BOLD, pDrawing);
			//
			//if( textStyle.m_modifiers & CTextFragmentStyle::eBOLD &&
			//	textStyle.m_modifiers & CTextFragmentStyle::eITALIC )
			//	pStyle->setFontStyleNumber(textStyle.m_pFont->desc, ACAD_ITALIC | ACAD_BOLD, pDrawing);

			// textStyle has true stylenumber, so simply it can set.
			pStyle->setFontStyleNumber(textStyle.m_pFont->desc, textStyle.m_stylenumber, pDrawing);
			// EBATECH ]-
		}
	}
	db_fontlink*	pBigFont = pStyle->ret_bigfont();
	if(pBigFont != textStyle.m_pBigfont)
		pStyle->set_bigfont(textStyle.m_pBigfont);

	sds_real	spacingFactor;
	if(textStyle.m_spaceFactor)
	{
		// save font spacing
		spacingFactor = textStyle.m_pFont->spfact;
		// temporary set new spacing factor
		textStyle.m_pFont->spfact = textStyle.m_spaceFactor;
	}

	if(!gr_textgen(pMtext, &textStyle, 1, pDrawing, pView, pDC, NULL, NULL, bmin, bmax))
		result = 1;

	if(pFont != textStyle.m_pFont)
	{
		// restore original font
		pStyle->set_font(pFont);
		// new font is TTF
		if(textStyle.m_pFont->format == db_fontlink::TRUETYPE)
			if(pFont->format == db_fontlink::TRUETYPE)
			{
				// old font is also TTF
				pStyle->setFontFamily(fontFamily, pDrawing);
				// restore original font style
				pStyle->setFontStyleNumber(fontFamily, fontStyle, pDrawing);
			}
			else
				pStyle->ensureNoTrueTypeEED(pDrawing);
	}
	if(pBigFont != textStyle.m_pBigfont)
		// restore original bigfont
		pStyle->set_bigfont(pBigFont);

	// resore original spacing factor
	if(textStyle.m_spaceFactor)
		textStyle.m_pFont->spfact = spacingFactor;

	return result;
}

/*-------------------------------------------------------------------------*//**
Forms display objects of given MTEXT fragment using gr_textgen().

@author Denis Petrov
@param pDOListBegin <= begin of display objects list for given fragment
@param pDOListBegin <= begin of display objects list for given fragment
@param offset => offset of given fragment from the insertion point of MTEXT
@param text => temporary DB object to pass to gr_textgen()
@param textStyle => style of fragment
@param pView =>
@param pDrawing =>
@param pDC =>
@return 1 for success, 0 for error or allowed width or height less then real width or height
*//*--------------------------------------------------------------------------*/
int
generateMtextFragment
(
 gr_displayobject*&		pDOListBegin,
 gr_displayobject*&		pDOListEnd,
 sds_point&				offset,
 db_text&				text,
 CTextFragmentStyle&	textStyle,
 gr_view*				pView,
 db_drawing*			pDrawing,
 CDC*					pDC
)
{
	int			result = 0;
	short		color;
	db_mtext*	pMtext = (db_mtext*)pView->ment;

	// save original style font
	db_styletabrec*	pStyle;
	pMtext->get_7((db_handitem**)&pStyle);

	// temporary set new font in style object
	db_fontlink* pFont = pStyle->ret_font();

	long	fontStyle;
	char	fontFamily[IC_ACADBUF];
	if(pFont != textStyle.m_pFont)
	{
		pStyle->set_font(textStyle.m_pFont);
		// new font is TTF
		if(textStyle.m_pFont->format == db_fontlink::TRUETYPE)
		{
			if(pFont->format == db_fontlink::TRUETYPE)
			{
				// old font is also TTF
				// save font style
				pStyle->getFontFamily(fontFamily);
				pStyle->getFontStyleNumber(&fontStyle);
			}

			pStyle->setFontFamily(textStyle.m_pFont->desc, pDrawing);
			// EBATECH(CNBR) -[ textStyle has true stylenumber
			//if(textStyle.m_modifiers & CTextFragmentStyle::eITALIC)
			//	pStyle->setFontStyleNumber(textStyle.m_pFont->desc, ACAD_ITALIC, pDrawing);

			//if(textStyle.m_modifiers & CTextFragmentStyle::eBOLD)
			//	pStyle->setFontStyleNumber(textStyle.m_pFont->desc, ACAD_BOLD, pDrawing);

			//if(textStyle.m_modifiers & CTextFragmentStyle::eBOLD &&
			//	textStyle.m_modifiers & CTextFragmentStyle::eITALIC)
			//	pStyle->setFontStyleNumber(textStyle.m_pFont->desc, ACAD_ITALIC | ACAD_BOLD, pDrawing);
			pStyle->setFontStyleNumber(textStyle.m_pFont->desc, textStyle.m_stylenumber, pDrawing);
			// EBATECH(CNBR) ]-
		}
	}

	db_fontlink* pBigFont = pStyle->ret_bigfont();
	if(pBigFont != textStyle.m_pBigfont)
		pStyle->set_bigfont(textStyle.m_pBigfont);

	sds_real	spacingFactor;
	if(textStyle.m_spaceFactor)
	{
		// save font spacing
		spacingFactor = textStyle.m_pFont->spfact;
		// temporary set new spacing factor
		textStyle.m_pFont->spfact = textStyle.m_spaceFactor;
	}

	if(textStyle.m_modifiers & CTextFragmentStyle::EModifiers::eUNDERLINED)
	{
		*(--textStyle.val) = 'U';
		*(--textStyle.val) = '%';
		*(--textStyle.val) = '%';
	}

	if(textStyle.m_modifiers & CTextFragmentStyle::EModifiers::eOVERLINED)
	{
		*(--textStyle.val) = 'O';
		*(--textStyle.val) = '%';
		*(--textStyle.val) = '%';
	}

	textStyle.m_insertionPoint[0] += offset[0];
	textStyle.m_insertionPoint[1] += offset[1];
	textStyle.m_insertionPoint[2] += offset[2];
	text.set_11(textStyle.m_insertionPoint);
	text.set_10(textStyle.m_insertionPoint);

	if(!gr_fixtextjust(&text, pDrawing, pView, pDC, textStyle.vert))
	{
		text.get_10(textStyle.m_insertionPoint);

		// EBATECH(CNBR) -[ 24-03-2003 MTEXT bylayer and byblock is bad color.
		color = textStyle.m_color;
		if(color < DB_BBCOLOR || color > DB_BLCOLOR) 
			color = DB_BLCOLOR;
		if(color == DB_BBCOLOR) 
			color = pView->bbcolor;
		if(color == DB_BLCOLOR)
			color = pView->blcolor;
		pView->color = color;
		// EBATECH(CNBR) ]-

		// generate text fragment
		if(!gr_textgen(pMtext, &textStyle, 0, pDrawing, pView, pDC, &pDOListBegin, &pDOListEnd, NULL, NULL))
			result = 1;
	}

	if(pFont != textStyle.m_pFont)
	{
		// restore original font
		pStyle->set_font(pFont);
		// new font is TTF
		if(textStyle.m_pFont->format == db_fontlink::TRUETYPE)
			if(pFont->format == db_fontlink::TRUETYPE)
			{
				// old font is also TTF
				pStyle->setFontFamily(fontFamily, pDrawing);
				// restore original font style
				pStyle->setFontStyleNumber(fontFamily, fontStyle, pDrawing);
			}
			else
				pStyle->ensureNoTrueTypeEED(pDrawing);
	}
	if(pBigFont != textStyle.m_pBigfont)
		pStyle->set_bigfont(pBigFont);

	// resore original spacing factor
	if(textStyle.m_spaceFactor)
		textStyle.m_pFont->spfact = spacingFactor;

	return result;
}

/*-------------------------------------------------------------------------*//**
Forms display objects of given MTEXT line using generateMtextFragment().

@author Denis Petrov
@param pDOListBegin <= begin of display objects list for given line
@param pDOListBegin <= begin of display objects list for given line
@param insertionPoint => insertion point of MTEXT
@param lineOffset => offset of given line from the insertion point of MTEXT
@param lineSize => sizes of line (width and height) for correct alignment
@param text => temporary DB object to pass to gr_textgen()
@param fragments => vector of all MTEXT fragments
@param startIndex => start index of line in fragments vector
@param pView =>
@param pDrawing =>
@param pDC =>
@return index of fragment following last generated for success, 0 for error or allowed width or height less then real width or height
*//*--------------------------------------------------------------------------*/
int
generateMtextLine
(
 gr_displayobject*&		pDOListBegin,
 gr_displayobject*&		pDOListEnd,
 sds_point&				insertionPoint,
 sds_point&				lineOffset,
 sds_real*				lineSize,
 db_text&				text,
 MTEXTFRAGMENTS_VECTOR&	fragments,
 int					startIndex,
 gr_view*				pView,
 db_drawing*			pDrawing,
 CDC*					pDC
)
{
	// this will be used to store display objects of MTEXT fragments
	gr_displayobject	*pCurrentFragmentListBegin = NULL,
						*pCurrentFragmentListEnd = NULL;
	sds_point			currentFragmentOffset;
	int					i = startIndex;

	fragments.at(i).m_style.m_insertionPoint[0] = insertionPoint[0];
	fragments.at(i).m_style.m_insertionPoint[1] = insertionPoint[1];
	fragments.at(i).m_style.m_insertionPoint[2] = insertionPoint[2];
	fragments.at(i).m_style.val = CMtextFragment::s_buffer + fragments.at(i).m_firstChar;
	currentFragmentOffset[0] = lineOffset[0];
	currentFragmentOffset[1] = lineOffset[1];
	currentFragmentOffset[2] = lineOffset[2];

	if(fragments.at(i).m_style.m_alignment == 1)
	{
		if(fragments.at(i).m_fragmentType == CMtextFragment::EFragmentType::eDEFAULT)
			currentFragmentOffset[1] += 0.5 * (lineSize[1] - fragments.at(i).m_style.m_height);
		else
			currentFragmentOffset[1] += 0.5 * lineSize[1] - fragments.at(i).m_style.m_height;
	}
	else
	{
		if(fragments.at(i).m_style.m_alignment == 2)
		{
			if(fragments.at(i).m_fragmentType == CMtextFragment::EFragmentType::eDEFAULT)
				currentFragmentOffset[1] += lineSize[1] - fragments.at(i).m_style.m_height;
			else
				currentFragmentOffset[1] += lineSize[1] - 2.0 * fragments.at(i).m_style.m_height;
		}
	}

	if(fragments.at(i).m_fragmentType == CMtextFragment::EFragmentType::eSTACKED)
		currentFragmentOffset[1] += fragments.at(i).m_style.m_height * (1.0 +
		static_cast<sds_real>(fragments.at(i).m_style.m_pFont->below) /
		static_cast<sds_real>(fragments.at(i).m_style.m_pFont->above));

	while(fragments.at(startIndex).m_nLine == fragments.at(i).m_nLine)
	{
		if(fragments.at(i).m_fragmentType == CMtextFragment::EFragmentType::eSTACKED &&
			(fragments.at(i).m_style.m_modifiers & CTextFragmentStyle::EModifiers::eUNDERLINED) &&
			i + 1 < fragments.size() &&
			fragments.at(i + 1).m_fragmentType == CMtextFragment::EFragmentType::eWITH_STACKED &&
			(fragments.at(i + 1).m_style.m_modifiers & CTextFragmentStyle::EModifiers::eOVERLINED))
		{
			if(fragments.at(i + 1).m_size[0] > fragments.at(i).m_size[0])
				fragments.at(i).m_style.m_modifiers &= ~CTextFragmentStyle::EModifiers::eUNDERLINED;
			else
				fragments.at(i + 1).m_style.m_modifiers &= ~CTextFragmentStyle::EModifiers::eOVERLINED;
		}

		generateMtextFragment(pCurrentFragmentListBegin, pCurrentFragmentListEnd, currentFragmentOffset,
							  text, fragments.at(i).m_style, pView, pDrawing, pDC);

		if(pCurrentFragmentListBegin && pCurrentFragmentListEnd)
		{
			if(!pDOListBegin)
				pDOListBegin = pCurrentFragmentListBegin;
			else
				pDOListEnd->next = pCurrentFragmentListBegin;

			pDOListEnd = pCurrentFragmentListEnd;
			pCurrentFragmentListBegin = NULL;
			pCurrentFragmentListEnd = NULL;
		}

		++i;

		if(i >= fragments.size())
			break;

		// prepare to next fragment
		fragments.at(i).m_style.m_insertionPoint[0] = insertionPoint[0];
		fragments.at(i).m_style.m_insertionPoint[1] = insertionPoint[1];
		fragments.at(i).m_style.m_insertionPoint[2] = insertionPoint[2];
		fragments.at(i).m_style.val = CMtextFragment::s_buffer + fragments.at(i).m_firstChar;
		currentFragmentOffset[2] = lineOffset[2];

		switch(fragments.at(i).m_fragmentType)
		{
		case CMtextFragment::EFragmentType::eDEFAULT :
			if(fragments.at(i).m_style.vert)
			{
				currentFragmentOffset[1] -= fragments.at(i - 1).m_size[1];
				currentFragmentOffset[0] = lineOffset[0];
			}
			else
			{
				currentFragmentOffset[1] = lineOffset[1];
				currentFragmentOffset[0] +=	fragments.at(i - 1).m_size[0];
				if(i > 1 && fragments.at(i - 2).m_fragmentType == CMtextFragment::EFragmentType::eSTACKED &&
					fragments.at(i - 1).m_fragmentType == CMtextFragment::EFragmentType::eWITH_STACKED)
				{
					currentFragmentOffset[0] -=	0.5 * (fragments.at(i - 2).m_size[0] - fragments.at(i - 1).m_size[0]);
					if(fragments.at(i - 2).m_size[0] > fragments.at(i - 1).m_size[0])
						currentFragmentOffset[0] += fragments.at(i - 2).m_size[0] - fragments.at(i - 1).m_size[0];
				}
			}
			break;

		case CMtextFragment::EFragmentType::eSTACKED :
			if(i > 1 && fragments.at(i - 1).m_style.vert)
			{
				currentFragmentOffset[1] -= fragments.at(i - 1).m_size[1];
				currentFragmentOffset[0] = lineOffset[0];
				currentFragmentOffset[1] -= fragments.at(i).m_style.m_height * (1.0 + static_cast<sds_real>(fragments.at(i).m_style.m_pFont->below) /
					static_cast<sds_real>(fragments.at(i).m_style.m_pFont->above));
			}
			else
			{
				currentFragmentOffset[1] = lineOffset[1];
				currentFragmentOffset[1] += fragments.at(i).m_style.m_height * (1.0 + static_cast<sds_real>(fragments.at(i).m_style.m_pFont->below) /
					static_cast<sds_real>(fragments.at(i).m_style.m_pFont->above));
				currentFragmentOffset[0] +=	fragments.at(i - 1).m_size[0];
			}
			break;

		case CMtextFragment::EFragmentType::eWITH_STACKED:
			if(i > 1 && fragments.at(i - 2).m_style.vert)
			{
				currentFragmentOffset[1] -= fragments.at(i - 1).m_size[1];
				currentFragmentOffset[0] = lineOffset[0];
				currentFragmentOffset[1] -= fragments.at(i).m_style.m_height * (1.0 + static_cast<sds_real>(fragments.at(i).m_style.m_pFont->below) /
					static_cast<sds_real>(fragments.at(i).m_style.m_pFont->above));
			}
			else
			{
				currentFragmentOffset[1] = lineOffset[1];
				currentFragmentOffset[0] +=	0.5 * (fragments.at(i - 1).m_size[0] - fragments.at(i).m_size[0]);
			}
			break;

		default :
			assert(false);
		}	// switch

		if(fragments.at(i).m_style.m_alignment == 1)
		{
			if(fragments.at(i).m_fragmentType == CMtextFragment::EFragmentType::eDEFAULT)
				currentFragmentOffset[1] += 0.5 * (lineSize[1] - fragments.at(i).m_style.m_height);
			else
				currentFragmentOffset[1] += 0.5 * lineSize[1] - fragments.at(i).m_style.m_height;
		}
		else
		{
			if(fragments.at(i).m_style.m_alignment == 2)
			{
				if(fragments.at(i).m_fragmentType == CMtextFragment::EFragmentType::eDEFAULT)
					currentFragmentOffset[1] += lineSize[1] - fragments.at(i).m_style.m_height;
				else
					currentFragmentOffset[1] += lineSize[1] - 2.0 * fragments.at(i).m_style.m_height;
			}
		}
	}
	return i;
}

/*-------------------------------------------------------------------------*//**
Gets width and height of given MTEXT line using generateMtextFragment().

@author Denis Petrov
@param size <= sizes (width and height) of given line
@param fragments => vector of all MTEXT fragments
@param spacing => spacing factor
@param lineSpacing => line spacing factor
@param lineDirection => 0:horizontal 1:vertical
@param nLine => number of line to get width and height
@param index => index in vector of fragments to start find line with
@return index of fragment of next line for success, 0 for error

EBATECH(CNBR) 2001/12/9 Add lineDirection argument and Support Vertical
EBATECH(CNBR) 2002/4/12 Stacked MTEXT size is too wide.
*//*--------------------------------------------------------------------------*/
int
getLineRect
(
 sds_real*				size,
 MTEXTFRAGMENTS_VECTOR&	fragments,
 sds_real&				spacing,
 sds_real&				lineSpacing,
 int					lineDirection,
 int					nLine,
 int					index
)
{
	int	i = index;
	while(i < fragments.size() && fragments.at(i).m_nLine != nLine)
		++i;

	if(i >= fragments.size())
		return 0;

	sds_real	ls, s; // ls means Line Space. s means fragments space
	s = fragments.at(i).m_style.m_height * spacing;
	ls = (nLine == 1 ? 1.0 : fragments.at(i).m_style.m_pFont->lffact * lineSpacing);

	if(fragments.at(i).m_fragmentType == CMtextFragment::EFragmentType::eSTACKED)
	{
		if(i+1 < fragments.size())
		{
			size[0] = ( fragments.at(i).m_size[0] > fragments.at(i+1).m_size[0] ?
				fragments.at(i).m_size[0] : fragments.at(i+1).m_size[0] ) + s;
			size[1] = 2.0 * fragments.at(i).m_style.m_height * ls;
			++i;
		}
		else
		{
			size[0] = fragments.at(i).m_size[0] + s;
			size[1] = 2.0 * fragments.at(i).m_style.m_height * ls;
		}
	}
	else
		if(lineDirection == 0)
		{
			size[0] = fragments.at(i).m_size[0] + s;
			size[1] = fragments.at(i).m_style.m_height * ls;
		}
		else	// Vertical
		{
			size[0] = fragments.at(i).m_style.m_height * ls;
			size[1] = fragments.at(i).m_size[1] + s;
		}

	++i;
	while(i < fragments.size() && fragments.at(i).m_nLine == nLine)
	{
		ls = nLine == 1 ? 1.0 : fragments.at(i).m_style.m_pFont->lffact * lineSpacing;

		if(fragments.at(i).m_fragmentType == CMtextFragment::EFragmentType::eSTACKED)
		{
			if(size[1] < 2.0 * fragments.at(i).m_style.m_height * ls)
				size[1] = 2.0 * fragments.at(i).m_style.m_height * ls;

			s = fragments.at(i).m_style.m_height * spacing;
			if(i + 1 < fragments.size())
			{
				size[0] += ( fragments.at(i).m_size[0] > fragments.at(i+1).m_size[0] ?
					fragments.at(i).m_size[0] : fragments.at(i+1).m_size[0] ) + s;
				++i;
			}
			else
			{
				size[0] += fragments.at(i).m_size[0] + s;
				size[1] = 2.0 * fragments.at(i).m_style.m_height * ls;
			}
		}
		else 
			if(lineDirection == 0)
			{
				if(size[1] < fragments.at(i).m_style.m_height * ls)
					size[1] = fragments.at(i).m_style.m_height * ls;
				s = fragments.at(i).m_style.m_height * spacing;
				size[0] += fragments.at(i).m_size[0] + s;
			}
			else
			{
				if(size[0] < fragments.at(i).m_style.m_height * ls)
					size[0] = fragments.at(i).m_style.m_height * ls;
				s = fragments.at(i).m_style.m_height * spacing;
				size[1] += fragments.at(i).m_size[1] + s;
			}
		++i;
	}

	return i;
}

int
gr_mtextgen
(
 gr_displayobject*&	pDOListBegin,
 gr_displayobject*&	pDOListEnd,
 gr_view*			pView,
 db_drawing*		pDrawing,
 CDC*				pDC
)
{
	db_mtext*	pMtext = (db_mtext*)pView->ment;

	// form transformation matrix
	sds_point	entaxis[3];
	pMtext->get_210(entaxis[2]);
	sds_real	vectorLength =	entaxis[2][0] * entaxis[2][0] +
								entaxis[2][1] * entaxis[2][1] +
								entaxis[2][2] * entaxis[2][2];

	if(isEqualZeroWithin(vectorLength - 1.0))
	{
		vectorLength = sqrt(vectorLength);
		entaxis[2][0] /= vectorLength;
		entaxis[2][1] /= vectorLength;
		entaxis[2][2] /= vectorLength;
	}

	pMtext->get_11(entaxis[0]);
	vectorLength =	entaxis[0][0] * entaxis[0][0] +
					entaxis[0][1] * entaxis[0][1] +
					entaxis[0][2] * entaxis[0][2];

	if(isEqualZeroWithin(vectorLength - 1.0))
	{
		vectorLength = sqrt(vectorLength);
		entaxis[0][0] /= vectorLength;
		entaxis[0][1] /= vectorLength;
		entaxis[0][2] /= vectorLength;
	}

	ic_crossproduct(entaxis[2], entaxis[0], entaxis[1]);

	CMatrix4 entityTransformation;
	entityTransformation(0,0) = entaxis[0][0];
	entityTransformation(1,0) = entaxis[0][1];
	entityTransformation(2,0) = entaxis[0][2];
	entityTransformation(0,1) = entaxis[1][0];
	entityTransformation(1,1) = entaxis[1][1];
	entityTransformation(2,1) = entaxis[1][2];
	entityTransformation(0,2) = entaxis[2][0];
	entityTransformation(1,2) = entaxis[2][1];
	entityTransformation(2,2) = entaxis[2][2];
	entityTransformation(3,3) = 1.0;
	pView->m_transformation = pView->m_transformation * entityTransformation;

	int nLines;
	MTEXTFRAGMENTS_VECTOR	mtextFragments;
	UINT dwg_code_page = get_dwgcodepage(pDrawing);
	// check whether it's necessary before allocate strings
	if(dwg_code_page == 932 || dwg_code_page == 936 || dwg_code_page == 950)
	{
		// Chinese/Japanese word wrap pre-process
		// DP: TODO: check allocation/deallocation of strings
		char*	str_in_base = NULL;
		char*	str_bak = NULL;
		char*	str_with_slashes = NULL;
		
		pMtext->get_1(&str_in_base);
		if(str_in_base == NULL )
			return 0;
		
		str_bak = new char[strlen(str_in_base) + 1];
		if(str_bak == NULL )
			return 0;
		strcpy(str_bak, str_in_base);
		str_with_slashes = cjk_wordwrap((unsigned char*)str_bak, pDrawing);
		pMtext->set_1(str_with_slashes);
		// Parse MTEXT String into fragments
		nLines = gr_mtextparse(mtextFragments, pMtext, pDrawing, pDC);
		// Restore MTEXT String for error termination
		pMtext->set_1(str_bak);
		delete [] str_with_slashes;
		delete [] str_bak;
	}
	else
		nLines = gr_mtextparse(mtextFragments, pMtext, pDrawing, pDC);

	if(!nLines)
		return 0;

	// temporary DB object to pass to gr_textgen()
	db_text			mtextFragment;
	// pointer to MTEXT style object
	db_handitem*	pTextStyle = NULL;
	pMtext->get_7(&pTextStyle);
	// this will be used to store display objects of MTEXT lines
	gr_displayobject	*pMtextLineListBegin = NULL,
						*pMtextLineListEnd = NULL;

	short originalColor = pView->color;
	// initialization of data
	int	attachmentPoint,
		textHAlign, textVAlign;
	pMtext->get_71(&attachmentPoint);
	assert(attachmentPoint > 0 &&  attachmentPoint < 10);

	/*
	**	gr_textgen() needs the llpt (10) of a text item.
	**	We are going to get each line's ATTACHMENT pt (11)
	**	by offsetting insertPoint.	Therefore, we need to call
	**	gr_fixtextjust() to set the llpt for us.
	**	This requires a db_text object.
	**	Let's fill it in with the necessary values --
	**	except for pMtextString, alignmentPoint, and llpt, which will vary as we go,
	**	line by line:
	*/

	textHAlign = (attachmentPoint - 1) % 3;
	textVAlign = 3 - (attachmentPoint - 1) / 3;
	if(textVAlign == 1)
		textVAlign = 0;		// 0 - bottom, 1 - baseline
	mtextFragment.set_72(textHAlign);
	mtextFragment.set_73(textVAlign);
	textVAlign = (attachmentPoint - 1) / 3;
	mtextFragment.set_7(pTextStyle);

	int			lineDirection = mtextFragments.at(0).m_style.vert == 1 ? 1 : 0,
				linesDirection = 1 - lineDirection;
	sds_real	mtextSize[2] = {0.0, 0.0},
				mtextRealSize[2] = {0.0, 0.0},	// EBATECH(CNBR) 2002/4/4 extents
				initialTextHeight = 0.0;

	pMtext->get_41(&mtextSize[lineDirection]);
	if(mtextSize[lineDirection] <= 0.0 )
		mtextSize[lineDirection] = 0.0;
	//pMtext->get_43(&mtextSize[1]);
	pMtext->get_40(&initialTextHeight);

	sds_point	insertPoint, insertPointWCS;
	pMtext->get_10(insertPointWCS);

	// transform insertion point by inverse MTEXT matrix
	insertPoint[0] = entaxis[0][0] * insertPointWCS[0] + entaxis[0][1] * insertPointWCS[1] + entaxis[0][2] * insertPointWCS[2];
	insertPoint[1] = entaxis[1][0] * insertPointWCS[0] + entaxis[1][1] * insertPointWCS[1] + entaxis[1][2] * insertPointWCS[2];
	insertPoint[2] = entaxis[2][0] * insertPointWCS[0] + entaxis[2][1] * insertPointWCS[1] + entaxis[2][2] * insertPointWCS[2];

	sds_real	spacingFactor = 0.0, lineSpacing = 0.0, lineSpacingFactor = 0.0;
	pMtext->get_44(&lineSpacingFactor);
	if(lineSpacingFactor <= 0.0)
		lineSpacingFactor = 1.0;

	int fixedLineSpacing; // 2:Fixed 1:Variable(Default)
	pMtext->get_73(&fixedLineSpacing);

	/* DP: According to commentaries lffact member of db_fontlink must be used for
	calculating of line spacing but really lffact is not correct member. In previous
	realization of MTEXT generation was used next formula: below / above

	if(lineSpacingFactor)
		lineSpacingFactor *= mtextFragments.at(0).m_style.m_pFont->lffact;
	else
		lineSpacingFactor = mtextFragments.at(0).m_style.m_pFont->lffact;
	*/

	// re-order mtext if necessary
	if(mtextFragments.size() > 1 && !mtextReorder(mtextFragments, *pDrawing, *pDC))
		assert(false);

	// compute bounding boxes of mtext fragments
	int			i;
	sds_point	bbmin, bbmax;

	for(i = mtextFragments.size(); i--; )
	{
		mtextFragments.at(i).m_style.val = CMtextFragment::s_buffer + mtextFragments.at(i).m_firstChar;
		if(getMtextFragmentBB(mtextFragment, mtextFragments.at(i).m_style, bbmin, bbmax, pView, pDrawing, pDC))
		{
			mtextFragments.at(i).m_size[0] = bbmax[0] - bbmin[0];
			mtextFragments.at(i).m_size[1] = bbmax[1] - bbmin[1];
		}
	}

	// wrap lines if necessary
	int			nLine = 1;
	sds_real	lineSize[2] = {0.0, 0.0};
	int			j = 0, k = 0;

	if(!isEqualZeroWithin(mtextSize[lineDirection]))
	{
		for(i = 0 ; i < mtextFragments.size(); )
		{
			if(nLine == mtextFragments.at(i).m_nLine)
			{
				lineSize[lineDirection] += mtextFragments.at(i).m_size[lineDirection];
				if( lineSize[lineDirection] > mtextSize[lineDirection] &&
					((i > 0 && mtextFragments.at(i).m_fragmentType != CMtextFragment::EFragmentType::eWITH_STACKED &&
					mtextFragments.at(i - 1).m_nLine == mtextFragments.at(i).m_nLine) ||
					(i > 1 && mtextFragments.at(i).m_fragmentType == CMtextFragment::EFragmentType::eWITH_STACKED &&
					mtextFragments.at(i - 2).m_nLine == mtextFragments.at(i).m_nLine)) )
				{
					j = mtextFragments.at(i).m_fragmentType == CMtextFragment::EFragmentType::eWITH_STACKED ? i - 1 : i;

					for( ; j < mtextFragments.size() /*&& mtextFragments.at(j).m_nLine == nLine*/ ; ++j)
					{
						++(mtextFragments.at(j).m_nLine);
					}

					lineSize[lineDirection] = 0.0;
					i = k;
					continue;
				}
			}
			else
			{
				++nLine;
				k = i;
				lineSize[lineDirection] = mtextFragments.at(i).m_size[lineDirection];
			}
			++i;
		}
	}

	// EBATECH(CNBR) -[ 2002/4/2 Bind fragments for speed
	if( mtextFragments.size() > 1 )
	{
		if( BindMtextFragments( mtextFragments ) == true )
		{
			for(i = mtextFragments.size(); i--; )
			{
				if(getMtextFragmentBB(mtextFragment, mtextFragments.at(i).m_style, bbmin, bbmax, pView, pDrawing, pDC))
				{
					mtextFragments.at(i).m_size[0] = bbmax[0] - bbmin[0];
					mtextFragments.at(i).m_size[1] = bbmax[1] - bbmin[1];
				}
			}
		}
	}
	// EBATECH(CNBR) ]-

	// accumulate height of MTEXT
	i = 0;
	nLine = 1;

	sds_real initialLineSpacing;
	initialLineSpacing = initialTextHeight * mtextFragments.at(0).m_style.m_pFont->lffact * spacingFactor;

	while(i < mtextFragments.size())
	{
		// EBATECH(CNBR) -[ 2001/12/10 Line Rectangle was half size when VAlign is Mid or Bottom.
		//i += getLineRect(lineSize, mtextFragments, spacingFactor, lineSpacingFactor, lineDirection, nLine, i);
		i = getLineRect(lineSize, mtextFragments, spacingFactor, lineSpacingFactor, lineDirection, nLine, i);
		// EBATECH(CNBR) ]-
		++nLine;
		if(fixedLineSpacing != 2)
			mtextSize[linesDirection] += lineSize[linesDirection];
		else
			mtextSize[linesDirection] += initialLineSpacing;

		if(lineSize[lineDirection] > mtextRealSize[lineDirection])
			mtextRealSize[lineDirection] = lineSize[lineDirection];
		if(lineSize[linesDirection] > mtextRealSize[linesDirection])
			mtextRealSize[linesDirection] = lineSize[linesDirection];
	}
	// align center or right even width(DXF41) is zero.
	if(mtextSize[lineDirection] <= 0.0)
		mtextSize[lineDirection] = mtextRealSize[lineDirection];

	sds_point alignOffset = {0.0, 0.0, 0.0};
	if(lineDirection) // Vertical
	{
		if(textVAlign == 1)
			alignOffset[lineDirection] = -0.5 * mtextSize[lineDirection];
		else
			if(textVAlign == 2)
				alignOffset[lineDirection] = - mtextSize[lineDirection];
	}
	else	// Horizontal
	{
		if(textVAlign == 1)
			alignOffset[linesDirection] = 0.5 * mtextSize[linesDirection];
		else
			if(textVAlign == 2)
				alignOffset[linesDirection] = mtextSize[linesDirection];

		if(textHAlign == 1)
			alignOffset[lineDirection] = - 0.5 * mtextSize[lineDirection];
		else
			if(textHAlign == 2)
				alignOffset[lineDirection] = - mtextSize[lineDirection];
	}
	insertPoint[0] += alignOffset[0];
	insertPoint[1] += alignOffset[1];

	sds_point	lineOffset = {0.0, 0.0, 0.0};
	if(lineDirection) // Vertical
		lineOffset[linesDirection] = initialTextHeight;
	else	// Horizontal
		lineOffset[linesDirection] = -initialTextHeight;
	j = i = 0;
	nLine = 1;
	lineSize[linesDirection] = initialTextHeight;
	lineSize[lineDirection] = 0.0;
	while(i < mtextFragments.size())
	{
		if(i == j)
		{
			if( lineDirection ) // Vertical
			{
				// at(i) - begin of new line
				lineOffset[linesDirection] += lineSize[linesDirection] * (lineDirection - linesDirection) * (mtextFragments.at(j).m_nLine - nLine - 1);

				nLine = mtextFragments.at(j).m_nLine;
				j = getLineRect(lineSize, mtextFragments, spacingFactor, lineSpacingFactor, lineDirection, nLine, i);

				lineOffset[linesDirection] += lineSize[linesDirection] * (lineDirection - linesDirection);

				lineOffset[lineDirection] = 0.0;
				if(textVAlign == 1)
					lineOffset[lineDirection] -= 0.5 * (mtextSize[lineDirection] - lineSize[lineDirection]);
				else
					if(textVAlign == 2)
						lineOffset[lineDirection] -= (mtextSize[lineDirection] - lineSize[lineDirection]);
				lineSize[linesDirection] = mtextSize[linesDirection];
			}
			else // Horizontal
			{
				// at(i) - begin of new line
				lineOffset[linesDirection] += lineSize[linesDirection] * (lineDirection - linesDirection) * (mtextFragments.at(j).m_nLine - nLine - 1);

				nLine = mtextFragments.at(j).m_nLine;
				j = getLineRect(lineSize, mtextFragments, spacingFactor, lineSpacingFactor, lineDirection, nLine, i);

				lineOffset[lineDirection] = 0.0;
				lineOffset[linesDirection] += lineSize[linesDirection] * (lineDirection - linesDirection);

				if(textHAlign == 1)
					lineOffset[lineDirection] += 0.5 * (mtextSize[lineDirection] - lineSize[lineDirection]);
				else
					if(textHAlign == 2)
						lineOffset[lineDirection] += (mtextSize[lineDirection] - lineSize[lineDirection]);
				lineSize[lineDirection] = mtextSize[lineDirection];
			}
		}

		// try to generate line 
		k = generateMtextLine(pMtextLineListBegin, pMtextLineListEnd, insertPoint, lineOffset, lineSize,
							  mtextFragment, mtextFragments, i, pView, pDrawing, pDC);
		if(k)
		{
			if(k < j)
				assert(false);
			else
				i = k;
		}
		else
		{
			assert(false);
			pView->color = originalColor;
			return 0;
		}

		if(pMtextLineListBegin && pMtextLineListEnd)
		{
			if(!pDOListBegin)
				pDOListBegin = pMtextLineListBegin;
			else
				pDOListEnd->next = pMtextLineListBegin;

			pDOListEnd = pMtextLineListEnd;
			pMtextLineListBegin = NULL;
			pMtextLineListEnd = NULL;
		}
	}
	pView->color = originalColor;
	return 0;
}

/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Re-order mtext's fragments for bi-directional languages.
 * Returns:	'true' in success and 'false' in failure.
 ********************************************************************************/
bool
mtextReorder
(
 MTEXTFRAGMENTS_VECTOR&	mtextFrags,		// <=>
 db_drawing&			drw,			// =>
 CDC&					dc				// =>
)
{
	bool			bResult = true;
	wchar_t*		wcStr = new wchar_t[CMtextFragment::s_bufferLength];
	char*			mbStr = NULL;
	int				fragsCount = mtextFrags.size(),
					dwgCodePage = get_dwgcodepage(&drw),
					fragN, wcharN,
					*frags1stCharsNums = NULL,
					reoCharset = 0;	// = 0 if we don't need any re-ordering,
									// = HEBREW_CHARSET if we need a Hebrew re-ordering,
									// = ARABIC_CHARSET if we need an Arabic re-ordering
	TCHAR*			pCurChar;
	long			parsedCode;
	CFont			tmpFont;
	GCP_RESULTSA	reoResults;

	memset(&reoResults, 0, sizeof(GCP_RESULTSA));
	frags1stCharsNums = new int[fragsCount];

	// 1. See do we need a re-ordering & prepare a string for it.
	for(fragN = 0, wcharN = 0; fragN < fragsCount; ++fragN)
	{
		frags1stCharsNums[fragN] = wcharN;

		for(pCurChar = CMtextFragment::s_buffer + mtextFrags[fragN].m_firstChar; *pCurChar; )
		{
			// Modified Cybage VSB 30/07/2001 DD/MM/YY [
			// Reason: IntelliCAD crashes on missing font file or if font file
			// path is invalid in Tools->option->Fonts Mail from Sander, testallfonts.dwg file crashes
			// for missing gdt.shx font
			// Bug No 77817 from BugZilla
			if ( mtextFrags[fragN].m_style .m_pFont == NULL )
			{
				TCHAR fontName[] = "Icad.fnt";
				if(db_setfont(fontName, NULL, &mtextFrags[fragN].m_style .m_pFont, &drw) < 0)
				{
					bResult = false;
					goto out;
				}
			}
			// Modified Cybage VSB 30/07/2001 DD/MM/YY ]

			if((parsedCode = gr_parsetext(&pCurChar, dwgCodePage, mtextFrags[fragN].m_style.m_pFont->format)) == -1L)
			{
				bResult = false;
				goto out;
			}

			if(parsedCode > 0)
			{
				wcStr[wcharN] = parsedCode;
				if(!reoCharset)
				{
					if(0x0590 <= wcStr[wcharN] && wcStr[wcharN] <= 0x05FF) 		// Hebrew Unicode subrange
						reoCharset = HEBREW_CHARSET;
					else
						if(0x0600 <= wcStr[wcharN] && wcStr[wcharN] <= 0x06FF)	// Arabic Unicode subrange
							reoCharset = ARABIC_CHARSET;
				}
				++wcharN;
			}
		}	// for( thru fragment's chars
	}	// for( thru fragments

	wcStr[wcharN++] = L'\0';

	if(!reoCharset || wcharN < 3)
		goto out;	// we are happy :-)


	// 2. Get re-ordering map (indices) of the string.
	LOGFONT	logfont;
	CFont	*pOldFont;

	memset(&logfont, 0, sizeof(LOGFONT));
	logfont.lfCharSet = reoCharset;

	if(!tmpFont.CreateFontIndirect(&logfont))
	{
		bResult = false;
		goto out;
	}
	if(!(pOldFont = dc.SelectObject(&tmpFont)))
	{
		tmpFont.DeleteObject();
		bResult = false;
		goto out;
	}

	if(GetTextCharset(dc.GetSafeHdc()) != reoCharset)
	{	// what TO DO ?..
		dc.SelectObject(pOldFont);
		tmpFont.DeleteObject();
		bResult = false;
		goto out;
	}

	mbStr = new char[wcharN];
	WideCharToMultiByte(reoCharset == HEBREW_CHARSET ? 1255 : 1256,
						0,
						wcStr,
						wcharN,
						mbStr,
						wcharN,
						NULL,
						NULL);

	reoResults.lpOrder = new UINT[wcharN];
	reoResults.lStructSize = sizeof(GCP_RESULTSA);
	reoResults.nGlyphs = wcharN;

	if(!GetCharacterPlacementA(dc.GetSafeHdc(), mbStr, wcharN, 0, &reoResults, GCP_REORDER))
	{
		dc.SelectObject(pOldFont);
		tmpFont.DeleteObject();
		bResult = false;
		goto out;
	}

	if(reoResults.lpOrder[0] == reoResults.lpOrder[wcharN-1])
	{	// TO DO: couldn't re-order by the GetCharacterPlacementA.
		dc.SelectObject(pOldFont);
		tmpFont.DeleteObject();
//		bResult = false;
		goto out;
	}


	// 3. Re-order mtext's fragments.
	int	*fragsOrder, maxFragCharN;
	fragsOrder = new int[fragsCount];

	// compute frags order: some kind of sorting of reoResults.lpOrder[frags1stCharsNums[]] array
	// TO DO: optimize.
	for(fragN = fragsCount; fragN--; )
	{
		fragsOrder[fragN] = fragN;
		maxFragCharN = reoResults.lpOrder[frags1stCharsNums[fragN]];

		for(int i = fragsCount; i--; )
		{
			if(((int)reoResults.lpOrder[frags1stCharsNums[i]]) > maxFragCharN)
			{
				maxFragCharN = reoResults.lpOrder[frags1stCharsNums[i]];
				fragsOrder[fragN] = i;
			}
		}
		reoResults.lpOrder[frags1stCharsNums[fragsOrder[fragN]]] = -1;
	}

	// set reordering flags for moving spaces in TTFCreateDisplayObjects
	for(fragN = 0; fragN < fragsCount - 1; ++fragN)
	{
		if(fragsOrder[fragN+1] != fragsOrder[fragN] + 1)
			mtextFrags[fragN].m_style.m_bReorderFlag = true;
	}

	// re-order fragments themselves
	mtextFrags.resize(fragsCount * 2);
	for(fragN = fragsCount; fragN--; )
		mtextFrags[fragsCount + fragN] = mtextFrags[fragsOrder[fragN]];

	MTEXTFRAGMENTS_VECTOR::iterator	pLastItemToErase;
	for(pLastItemToErase = mtextFrags.begin(), fragN = mtextFrags.size() / 2; fragN--; )
		++pLastItemToErase;

	mtextFrags.erase(mtextFrags.begin(), pLastItemToErase);

	delete [] fragsOrder;

out:
	delete [] wcStr;
	delete [] mbStr;
	delete [] frags1stCharsNums;
	delete [] reoResults.lpOrder;

	return bResult;
}

/********************************************************************************
 * Author:	Chikahiro Masami
 * Purpose:	Bind mtext's fragments for performance.
 * Returns:	'true' any fragments are binded and 'false' nothing to change.
 ********************************************************************************/
bool BindMtextFragments(
 MTEXTFRAGMENTS_VECTOR&	mtextFrags
){
	bool bResult = false;
	MTEXTFRAGMENTS_VECTOR::iterator	pFrags;

	for(pFrags = mtextFrags.end() -1 ; pFrags != mtextFrags.begin(); --pFrags )
	{
		if( (*pFrags).m_fragmentType == (*(pFrags-1)).m_fragmentType &&
			(*pFrags).m_nLine == (*(pFrags-1)).m_nLine &&
			(*pFrags).m_style.gen == (*(pFrags-1)).m_style.gen &&
			(*pFrags).m_style.vert == (*(pFrags-1)).m_style.vert &&
			(*pFrags).m_style.m_modifiers == (*(pFrags-1)).m_style.m_modifiers &&
			(*pFrags).m_style.m_color == (*(pFrags-1)).m_style.m_color &&
			(*pFrags).m_style.m_alignment == (*(pFrags-1)).m_style.m_alignment &&
			(*pFrags).m_style.m_spaceFactor == (*(pFrags-1)).m_style.m_spaceFactor &&
			(*pFrags).m_style.m_stylenumber == (*(pFrags-1)).m_style.m_stylenumber &&
			(*pFrags).m_style.m_height == (*(pFrags-1)).m_style.m_height &&
			(*pFrags).m_style.m_obliqueAngle == (*(pFrags-1)).m_style.m_obliqueAngle &&
			(*pFrags).m_style.m_widthFactor == (*(pFrags-1)).m_style.m_widthFactor &&
			(*pFrags).m_style.m_pFont == (*(pFrags-1)).m_style.m_pFont &&
			(*pFrags).m_style.m_pBigfont == (*(pFrags-1)).m_style.m_pBigfont )
		{
			char* pTemp = new char [ (*pFrags).m_length + (*(pFrags-1)).m_length + 1 ];
			strcpy( pTemp, (*(pFrags-1)).m_style.val );
			strcat( pTemp, (*pFrags).m_style.val );
			strcpy( (*(pFrags-1)).m_style.val, pTemp );
			(*(pFrags-1)).m_length += (*pFrags).m_length;
			delete [] (void*)pTemp;
			pFrags = mtextFrags.erase(pFrags);
			bResult = true;
		}

	}
	return bResult;
}
