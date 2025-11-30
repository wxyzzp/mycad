#include "EncodingPP.h"
#include "xml.h"

enum
{	ePPL_IF = 0
,	ePPL_IFNDEF
,	ePPL_IFDEF
,	ePPL_DEFINE
,	ePPL_DEFINED
,	ePPL_ELSE
,	ePPL_ENDIF
,	ePPL_UNDEF
,	ePPL_SPACES
,	ePPL_MARK_OF_EXCLAMATION
,	ePPL_LBRACE
,	ePPL_RBRACE
,	ePPL_OR
,	ePPL_AND
,	ePPL_EQUAL
,	ePPL_LESS
,	ePPL_GREAT
,	ePPL_EOL
,	ePPL_EOF
,	ePPL_STRING
,	ePPL_COMMAND /* all commands (keywords which may follow '#' sign): 'ifdef', 'else', etc. */
,	ePPL_NREALIZED
};

EncodingPreProc::EncodingPreProc(Encoding* pEncoding, XMLMapStrStr* pMap): Encoding(pEncoding), m_NewLine(2),
	m_IfStack(0), m_nIfStackUsed(0), m_pMapDefines(pMap)
{
}

EncodingPreProc::~EncodingPreProc()
{
}

int EncodingPreProc::lexToken(XMLString& string)
{
	int ch = getchNL();
	int i;
	bool allSpaces = true;
	static const TCHAR specSymbols[] = "()<>=!|&";
	static const struct {
		TCHAR* name;
		int id;
	} commands[] =
	{	"if",      ePPL_IF,
		"ifdef",   ePPL_IFDEF,
		"ifndef",  ePPL_IFNDEF,
		"define",  ePPL_DEFINE,
		"defined", ePPL_DEFINED,
		"else",    ePPL_ELSE,
		"endif",   ePPL_ENDIF,
		"undef",   ePPL_UNDEF
	};

	string.resize(0);
	switch(ch)
	{
	case '!':  return ePPL_MARK_OF_EXCLAMATION;
	case '(':  return ePPL_LBRACE;
	case ')':  return ePPL_RBRACE;
	case '<':  return ePPL_LESS;
	case '>':  return ePPL_GREAT;
	case '=':  return ePPL_EQUAL;
	case -1:   return ePPL_EOF;
	case '\n': ungetchSource(ch); return ePPL_EOL;// EOL should not be lost
	case '|':
		if((ch = getchNL()) == '|')
			return ePPL_OR;
		allSpaces = false;
		break;
	case '&':
		if((ch = getchNL()) == '&')
			return ePPL_AND;
		allSpaces = false;
		break;
	}

	if(!_istspace(ch))
		allSpaces = false;

	for(; (_istspace(ch) != 0) == allSpaces; ch = getchNL())
	{
		if(ch == -1)
			break;
		if(!allSpaces)
		{
			for(i = sizeof(specSymbols)/sizeof(specSymbols[0]); i >= 0; --i)
			{
				if(ch == specSymbols[i])
					break;
			}

			if(i >= 0)
				break;
		}
		else if(ch == '\n')
			break;
		string += ch;
	}

	if(ch != -1)
		ungetchSource(ch);

	if(allSpaces == false)
	{
		const TCHAR* pStr = string.c_str();

		for(i = 0; i < sizeof(commands)/sizeof(commands[0]); ++i)
		{
			if(_tcscmp(commands[i].name, pStr) == 0)
				return commands[i].id;
		}
	}

	return allSpaces ? ePPL_SPACES:ePPL_STRING;
}

int EncodingPreProc::putch(int ch)
{
	return putchSource(ch);// stub. Actually preprocessor shouldn't inserted for writing
}

bool EncodingPreProc::getTillNL(XMLString& string)
{
	int ch;

	for(; (ch = getchNL()) != -1;)
	{
		if(ch == '\n')
		{
			ungetchSource(ch);
			return true;
		}
		string += ch;
	}

	return false;
}

bool EncodingPreProc::procDefine()
{
	XMLString name, value;
	int token;

	if(!getConcreteToken(ePPL_STRING, name))
		return false;

	token = lexToken(value);
	if(token == ePPL_SPACES)
		value.resize(0);

	getTillNL(value);
	mapDefines()[name] = value;

	return true;
}

void EncodingPreProc::reportUnexpectedToken(int token, int tokenExpected)
{
	int iLine, iPos;
	
	getFilePos(iLine, iPos);
	setError(XMLString().assign("Unexpected preprocessor token, string %i near or at position %i. ",
		iLine, iPos).c_str());
	setError(XMLString().assign("Expected token %i, on input %i. ", tokenExpected, token).c_str());
}

bool EncodingPreProc::getConcreteToken(int tokenExpected, XMLString& keyWord)
{
	int token = lexToken(keyWord);

	if(token == ePPL_SPACES)
		token = lexToken(keyWord);// two 'spaces' tokens can't be adjoin
	if(token != tokenExpected)
	{
		reportUnexpectedToken(token, tokenExpected);
		return false;
	}

	return true;
}

bool EncodingPreProc::getMacro(const XMLString& name, XMLString& result)
{
	if(!m_pMapDefines)
		return false;

	XMLMapStrStr::const_iterator it = m_pMapDefines->find(name);

	if(it != m_pMapDefines->end())
	{
		result = it->second;
		return true;
	}
	else
		return false;
}

bool EncodingPreProc::syntaxAnalize()
{
	XMLString keyWord, macro;
	int token;
	bool result = false;
	
	keyWord.resize(0);
	token = lexToken(keyWord);
	
	if(token == ePPL_EOF)
	{
		reportUnexpectedToken(token, ePPL_COMMAND);
		goto syntaxError;
	}

	switch(token)
	{
	case ePPL_IFDEF:
	case ePPL_IFNDEF:
		if(!getConcreteToken(ePPL_STRING, keyWord))
			goto syntaxError;
		push(token == ePPL_IFDEF ? getMacro(keyWord, macro):!getMacro(keyWord, macro));
		break;
	case ePPL_ENDIF:
		pop();
		break;
	case ePPL_ELSE:
		toggleLast();
		break;
	case ePPL_DEFINE:
		if(!procDefine())
			goto syntaxError;
		break;
	case ePPL_UNDEF:
		if(!getConcreteToken(ePPL_STRING, keyWord))
			goto syntaxError;
		mapDefines().erase(keyWord);
		break;
	case ePPL_IF:// not yet realized
	default:
		reportUnexpectedToken(token, ePPL_NREALIZED);
		goto syntaxError;
	}

	getTillNL(keyWord);
	return true;

syntaxError:
	getTillNL(keyWord);
	return false;
}

int EncodingPreProc::getchTranslate()
{
	register int ch;
	
	for(;;)
	{
		ch = getchNL();
		
		if(ch < 0)
		{
			ASSERT(m_nIfStackUsed == 0);// temporary
			if(m_nIfStackUsed != 0)
				setError(XMLString().assign("%i levels are not closed", m_nIfStackUsed).c_str());
			return ch;
		}
		
		if(!isNewLine() || ch != '#')
		{
			if(isEnable())
				return ch;
			continue;
		}
		
		syntaxAnalize();
	}
}

void EncodingPreProc::push(bool value)
{
	m_IfStack |= (value ? 1:0) << 2*m_nIfStackUsed++;
}

void EncodingPreProc::pop()
{
	m_IfStack &= (1 << 2*(--m_nIfStackUsed)) - 1;
}

void EncodingPreProc::toggleLast()
{
	ASSERT(m_nIfStackUsed > 0 && (m_IfStack & (1 << (2*m_nIfStackUsed - 1))) == 0);
	m_IfStack |= 1 << (2*m_nIfStackUsed - 1);
}

bool EncodingPreProc::isEnable() const
{
	if(m_nIfStackUsed == 0)
		return true;

	int last = 3 & (m_IfStack >> 2*(m_nIfStackUsed - 1));

	return last == 2 || last == 1;
}

int EncodingPreProc::getchNL()
{
	register int ch = getchSource();

	if(ch == '\n')
	{
		m_NewLine = 2;
		return ch;
	}
	else if(m_NewLine == 1)
		m_NewLine = 0;
	else if(m_NewLine == 2 && !_istspace(ch))
		m_NewLine = 1;

	return ch;
}

void EncodingPreProc::ungetchSource(int ch)
{
	m_NewLine += (m_NewLine == 1) ? 1:0;
	Encoding::ungetchSource(ch);
}
