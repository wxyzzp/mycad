#include "XmlStream.h"
#include "XmlDoc.h"

#ifndef _ENCODING_H_
#include "Encoding.h"
#endif

#if 0
const TCHAR * XMLStream::s_xmlHeader	= "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
const TCHAR * XMLStream::s_indent		= "  ";
#endif

int
XMLStream::getToken(XMLString& str)
{
    int ch = (m_bInsideTag) ? skipSpaces():getch();

	if(ch == -1)
		return eTOKEN_EOF;

	if(!m_bInsideTag)
	{
		ungetch(ch);
		return (ch == '<') ? getTag(str):getString(str);
	}

	// inside tag
	if(ch == '/')
	{
		m_bInsideTag = false;
		return (getch() == '>') ? eTOKEN_EMPTY_TAG_E:eTOKEN_BAD;
	}

	if(ch == '>')
	{
		m_bInsideTag = false;
		return eTOKEN_OPENING_TAG_E;
	}
	
	ungetch(ch);
	return getAttr(str);
}

int
XMLStream::getTag(XMLString& str)
{
    int ch = skipSpaces();// return symbol beyond spaces

    if(ch == -1)
        return eTOKEN_EOF;
    if(ch != '<')
    {
        ungetch(ch);
        return eTOKEN_BAD;
    }
    ch = getch();
    switch(ch)
    {
    case '/': return (getString(str, ">") != eTOKEN_STRING || getch() != '>') ? eTOKEN_BAD:eTOKEN_CLOSING_TAG;
    case '!': if(getch() == '-' && getch() == '-') return getComment(str); break;
    case '?': return (getString(str, ">") != eTOKEN_STRING || getch() != '>') ? eTOKEN_BAD:eTOKEN_COMMENT_TAG;
    default:
        break;
    }

	ungetch(ch);
	m_bInsideTag = true;

    if(getString(str, " \t\n/>") != eTOKEN_STRING)
        return eTOKEN_BAD;
	return eTOKEN_OPENING_TAG_B;
}

int
XMLStream::getComment(XMLString& str)
{
	int i = 0, ch;

	for(;;)
	{
		ch = getch();

		if(ch == -1)
			return eTOKEN_BAD;
		if(ch == '-')
			++i;
		else if(ch == '>' && i > 1)
			return eTOKEN_COMMENT_TAG;
		else if(i > 0)
		{
			while(i)
			{
				str += '-';
				--i;
			}
		}
#ifdef _MBCS
		str.SetU( ch );
#else
		str += ch;
#endif
	}
}

int
XMLStream::getString(XMLString& str, const TCHAR* cp)
{
    int ch = getch(), i, nBrk(0);
    int brkSyms[8];

    str.resize(0);
    for(; cp && *cp && nBrk < sizeof(brkSyms)/sizeof(brkSyms[0] - 1);)
        brkSyms[nBrk++] = *cp++;
    brkSyms[nBrk++] = '<';
    for(;;)
    {
        if(ch == -1)
            return eTOKEN_STRING;
		if(ch == '&')
		{
			XMLString name;

			for(;;)
			{
				ch = getch();
				if(ch == -1)
					return eTOKEN_BAD;
				if(ch == ';')
					break;
				for(i = nBrk; i--;)
				{
					if(ch == brkSyms[i])
						return eTOKEN_BAD;
				}
#ifdef _MBCS
				name.SetU( ch );
#else
				name += ch;
#endif
			}

			if(m_pXMLReferences)
			{
				XMLMapStrStr::iterator it = m_pXMLReferences->find(name);
				
				if(it != m_pXMLReferences->end())
				{
					str += it->second;
				}
			}
			ch = getch();
			continue;// skip ';' 
		}
        for(i = nBrk; i--;)
        {
            if(ch == brkSyms[i])
            {
                ungetch(ch);
                return eTOKEN_STRING;
            }
        }
#ifdef _MBCS
		str.SetU( ch );
#else
		str += ch;
#endif
		ch = getch();
    }
}

int
XMLStream::getQString(XMLString& str)
{
	int ch = getch();
	TCHAR brkSyms[2];

	if(ch != '\'' && ch != '"')
		return eTOKEN_BAD;
	brkSyms[0] = ch;
	brkSyms[1] = 0;
	if(getString(str, brkSyms) != eTOKEN_STRING || getch() != ch)
		return eTOKEN_BAD;

	return eTOKEN_QSTRING;
}

int
XMLStream::skipSpaces()
{
    int ch;

    for(;;)
    {
        ch = getch();
        if(ch == -1 || !isspace(ch))
            return ch;
    }
}

int
XMLStream::getAttr(XMLString& str)
{
    int ch;

    ch = skipSpaces();
	ungetch(ch);
    str.resize(0);
	if(getString(str, "=>'\"") != eTOKEN_STRING || skipSpaces() != '=')
		return eTOKEN_BAD;
    ch = skipSpaces();
	ungetch(ch);
	
	return ((ch != '\'' && ch != '"') || getQString(m_AttrValue) != eTOKEN_QSTRING) ? eTOKEN_BAD:eTOKEN_ATTRNAME;
}

/*-------------------------------------------------------------------------*/
XMLStream::XMLStream()
{
	init();
}

void
XMLStream::init()
{
	m_bInsideTag = false;
	m_pEncoder = NULL;
	m_pXMLReferences = NULL;
	m_ErrorCode = XMLDocument::eXMLE_NONE;
}

XMLStream::~XMLStream()
{
	if(m_pEncoder)
		delete m_pEncoder;
}

/*-------------------------------------------------------------------------*/
bool XMLStream::empty ()
{
	int ch = getch();

	if(ch == -1)
		return true;
	ungetch(ch);
	return false;
}

Encoding* XMLStream::setEncoding(XMLString& coding)
{
	if(m_pEncoder)
		delete m_pEncoder;

	return m_pEncoder = Encoding::getEncoder(coding.c_str());
}

int XMLStream::getch()
{
	int ch;
	
	XML_ASSERT(m_pEncoder != NULL);
	if((ch = m_pEncoder->getch()) < 0)
		return -1;
	
	return ch;
}

int XMLStream::putch(int ch)
{
	return m_pEncoder->putch(ch);
}

void XMLStream::setError(int code, const XMLString& str)
{
	m_ErrorCode = code;
	m_ErrorText += str;
}

void XMLStream::addToError(const XMLString& str)
{
	XML_ASSERT(m_ErrorCode != 0);
	m_ErrorText += str;	
}
