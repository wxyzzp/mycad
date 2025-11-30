#include "Xmldoc.h"
#include "XmlStream.h"
#include "EncodingPP.h"

static void
setDefaultXMLReferences(XMLMapStrStr& map)
{
	static struct {
		char* ch;
		char* str;
	} symMap[] =
	{	"<",  "lt",
	">",  "gt",
	"'",  "apos",
	"\"", "qout",
	"&",  "amp"
	};
	
	for(int i = sizeof(symMap)/sizeof(symMap[0]); i--;)
		map[XMLString(symMap[i].str)] = XMLString(symMap[i].ch);
}

XMLDocument::XMLDocument(int options):	m_pFile(NULL), m_bRead(true)
{
	m_rootNode.setDocument(this);
	m_pStream = new XMLStream;
}

XMLDocument::~XMLDocument()
{
	if(m_pFile && m_bCloseOnDelete)
		fclose(m_pFile);
	delete m_pStream;
}

XMLNode& XMLDocument::rootNode()
{
	return m_rootNode;
}

int XMLDocument::lastErrorCode() const
{
	return m_pStream->lastErrorCode();
}

const XMLString& XMLDocument::lastErrorText() const
{
	return m_pStream->lastErrorText();
}

bool XMLDocument::setEncoding(const XMLString& encoding)
{
	if(m_pStream->m_pEncoder != NULL)
	{
		delete m_pStream->m_pEncoder;
		m_pStream->m_pEncoder = NULL;
	}

	return beSureEncoder(encoding);
}

bool
XMLDocument::setFile(const XMLString& fileName, bool bRead, bool bCloseOnDelete)
{
	bool bOk = true;

	if(m_pFile)
		fclose(m_pFile);
	m_NodeStack.clear();
	m_bRead = bRead;
	m_FileName = fileName;
	m_bCloseOnDelete = bCloseOnDelete;
	m_pFile = _tfopen(m_FileName.c_str(), bRead ? "r":"w");
	m_SymMap.clear();
	setDefaultXMLReferences(m_SymMap);
	m_pStream->xmlReferences() = &m_SymMap;
	if(m_pStream->m_pEncoder)
	{
		delete m_pStream->m_pEncoder;
		m_pStream->m_pEncoder = NULL;
	}
	if(bRead && m_pFile)
		bOk = read();

	return m_pFile != NULL && bOk;
}

bool
XMLDocument::read()
{
	while(!rootNode().m_bReadCompletely)
	{
		if(!readNext())
		{
			rootNode().m_bReadCompletely = true;
			return false;
		}
	}

	return true;
}

bool
XMLDocument::readNext()
{
#define SET_ERROR1(A,B,C) m_pStream->setError(A, XMLString().assign(B,C))
	if(!m_bRead || !m_pFile)
		return false;

	if(!beSureEncoder())
		return false;

	XMLString arg1;
	int token, sz;

	for(;;)
	{
		token = m_pStream->getToken(arg1);
		sz = m_NodeStack.size();
		switch(token)
		{
		case XMLStream::eTOKEN_OPENING_TAG_B:
			if(!sz)
			{
				m_rootNode.name() = arg1;
				m_NodeStack.push_back(&m_rootNode);
			}
			else
			{
				m_NodeStack.push_back(new XMLNode(arg1));
				m_NodeStack[sz - 1]->addChild(m_NodeStack[sz]);
				m_NodeStack[sz]->setComplete(false);
			}

			m_NodeStack[sz]->setDocument(this);

			while((token = m_pStream->getToken(arg1)) == XMLStream::eTOKEN_ATTRNAME)
			{
				if(!m_NodeStack[sz]->addAttribute(arg1, m_pStream->tokenArg2()))
				{
					SET_ERROR1(eXMLE_SOMETHING_WRONG, "Double attribute %s\n", arg1.c_str());
				}
			}

			switch(token)
			{
			case XMLStream::eTOKEN_OPENING_TAG_E: continue;
			case XMLStream::eTOKEN_EMPTY_TAG_E:
			{
				// here m_NodeStack contain sz elements plus current, so total - sz + 1
				m_NodeStack[sz]->setComplete(true);
				m_NodeStack.resize(sz);

				continue;
			}
			case XMLStream::eTOKEN_BAD: goto tokenBad;
			case XMLStream::eTOKEN_EOF: goto unexpected;
			default: goto unexpected;
			}
			/* NOTREACHED */
		case XMLStream::eTOKEN_BAD:
tokenBad:	SET_ERROR1(eXMLE_SOMETHING_WRONG, "Bad lexical token (lexical error)\n", 0);
			return false;
		case XMLStream::eTOKEN_EOF:
			SET_ERROR1(eXMLE_SOMETHING_WRONG, "Unexpected end of file '%s'\n", m_FileName.c_str());
			return false;
		case XMLStream::eTOKEN_STRING:
			if((m_Options & eXML_OPT_NSKIP_SPACES_IN_BODY) == 0 || sz == 0)
			{
				int i;
				
				for(i = arg1.length(); --i >= 0;)
				{
					if(!_istspace(arg1[i]))
						break;
				}
				if(i < 0)
					break;
			}
			if(sz > 0)
			{
				m_NodeStack[sz - 1]->value() += arg1;
				break;
			}
			/* FALLTHROUGH */
		default:
unexpected:	SET_ERROR1(eXMLE_SOMETHING_WRONG, "unexpected token %i\n", token);
			return false;
 			/* NOTREACHED */
		case XMLStream::eTOKEN_COMMENT_TAG: break;
		case XMLStream::eTOKEN_CLOSING_TAG:
			if(sz <= 0 || !(m_NodeStack[sz - 1]->name() == arg1))
			{
				SET_ERROR1(eXMLE_SOMETHING_WRONG, "Closing of unopen tag: %s\n", arg1.c_str());
				return false;
			}
			m_NodeStack[sz - 1]->setComplete(true);
			m_NodeStack.resize(sz - 1);
			return true;
			/* NOTREACHED */
		}
	}
}

/*--------------------------------------------------------------------------*/
static void StringToXML(XMLString &string, bool bForAttributeValue /*= true*/)
{
	XMLString oldStr;
	int i;
	TCHAR ch;

	oldStr = string;
	string.resize(0);
	for (i = 0; i < oldStr.length(); i++)
	{
		ch = oldStr[i];
		switch ( ch )
		{
		case '&': string += "&amp;"; break;
		case '<': string += "&lt;";  break;
		case '>': string += "&gt;";  break;
		case '\'':
			if ( bForAttributeValue )
				string += "&apos;";
			break;
		case '\"':
			if ( bForAttributeValue )
				string += "&quot;";
			break;
		default:
			string += ch;
			break;
		}
	}
}

static bool
printRawString(XMLStream& out, const XMLString& str)
{
    for (int i = 0; i < str.length(); i++)
	{
		if(out.putch(str[i]) < 0)
			return false;
	}
	return true;
}

static bool
printString(XMLStream& out, const XMLString& str, bool bForAttributeValue = false)
{
	int ch;
	int i;
	XMLString addStr;
#ifdef _MBCS
    XMLString str2 = str;
#endif

    for (i = 0; i < str.length(); )
	{
		// USAGE: MBCS .. ch is splitted!
#ifdef _MBCS
		ch = str2.GetU( i );
#else
		ch = ((1 << sizeof(TCHAR)*8) - 1) & str[i];
		i++;
#endif
		switch ( ch )
		{
		case '&': addStr = "&amp;"; break;
		case '<': addStr = "&lt;";  break;
		case '>': addStr = "&gt;";  break;
		case '\'':
			if ( bForAttributeValue )
				addStr = "&apos;";
			else
			{
				if(out.putch(ch) < 0)
					return false;;
				continue;
			}
			break;
		case '\"':
			if ( bForAttributeValue )
				addStr = "&quot;";
			else
			{
				if(out.putch(ch) < 0)
					return false;;
				continue;
			}
			break;
		default:
			if(out.putch(ch) < 0)
				return false;;
			continue;
		}

		for(int j = 0; j < addStr.length(); ++j)
			if(out.putch(addStr[j]) < 0)
				return false;;
	}

	return true;
}

static bool
printNode(XMLStream& out, const XMLNode& node, bool beautyOutput)
{
	bool result = false;

	if(beautyOutput)
		out.putch('\n');

	// Tag begin ('<') and Tag Name
	out.putch('<');
	if(!printString(out, node.name()))
		return false;

	// Attributes
	{
		XMLNode::attrIterator ai = node.attrBegin();
		XMLNode::attrIterator aiEnd = node.attrEnd();

		for(; ai != aiEnd; ++ai)
		{
			out.putch(' ');
			if(!printString(out, ai->first))
				return false;
			out.putch('=');
			out.putch('"');
			if(!printString(out, ai->second, true))
				return false;
			out.putch('"');
		}
	}

	// Tag end ('>') and Tag value
	out.putch('>');
	if(!printString(out, const_cast<XMLNode&>(node).value()))
		return false;

	if(beautyOutput && const_cast<XMLNode&>(node).value().length() != 0)
		out.putch('\n');

	// Sub Tags
	XMLNode::childIterator ci = node.childBegin();
	XMLNode::childIterator ciEnd = node.childEnd();

	for(; ci != ciEnd; ++ci)
		if(!printNode(out, **ci, beautyOutput))
			return false;
	
	if(beautyOutput)
		out.putch('\n');

	// Closing Tag
	out.putch('<');
	out.putch('/');
	return printString(out, node.name()) && out.putch('>') >= 0;
}

bool
XMLDocument::write(int options)
{
	if(m_pFile == NULL)
		return false;
	if(!beSureEncoder())
		return false;
    printRawString(*m_pStream, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
	return printNode(*m_pStream, rootNode(), (m_Options & eXML_OPT_NBEAUTY_OUTPUT) == 0);
}

inline bool XMLDocument::beSureEncoder(XMLString encoderName)
{
	if(m_pStream->m_pEncoder == NULL && m_pFile != NULL)
	{
		if(encoderName.length() == 0 && m_bRead)
			m_pStream->m_pEncoder = Encoding::getEncoderByXMLFile(m_pFile);
		else // if 'encoderName' is empty fucntion return default encoder
			m_pStream->m_pEncoder = Encoding::getEncoder(encoderName.c_str());
		
		if(m_pStream->m_pEncoder == NULL)
			return false;
		m_pStream->m_pEncoder->setFile(m_pFile);
		if(isNeedInsertPreProcessor())
			m_pStream->m_pEncoder = new EncodingPreProc(m_pStream->m_pEncoder, &m_PreProcPredefinedMacros);
	}

	return m_pStream->m_pEncoder != NULL;
}

void XMLDocument::addPredefinedMacro(const XMLString& name, const XMLString& value)
{
	XML_ASSERT(isNeedInsertPreProcessor());

	m_PreProcPredefinedMacros[name] = value;
}

bool XMLDocument::removePredefinedMacro(const XMLString& name)
{
	XML_ASSERT(isNeedInsertPreProcessor());

	return m_PreProcPredefinedMacros.erase(name) > 0;
}
