#ifndef _XmlStream_H_
#define _XmlStream_H_

// Developer: VT

#ifndef _XML_H_
#include "xml.h"
#endif

#ifndef _ENCODING_H_
#include "Encoding.h"
#endif

/*
XML grammatic (terminals marked with "_t"):

  XML_FileContent: StreamElement
                 | StreamElement XML_FileContent

  StreamElement: Tag
               | String_t

  Tag: OpeningTag
     | CommentTag_t
	 | ClosingTag_t
	 | Other

  OpeningTag: OpeningTagBegin_t OpeningTagEnd_t
            | OpeningTagBegin_t AttrList OpeningTagEnd_t

  AttrList: AttrName_t QString_t
          | AttrName_t QString_t AttrList

  QString_t: "'" String_t "'"
           | '"' String_t '"'

  AttrName_t: String_t Eq_t

  Eq_t: "="

  OpeningTagEnd_t: ">"

  OpeningTagBegin_t: "<" String_t

  CommentTag_t: "<!--" \.*\ "-->"

  ClosingTag_t: "</" String_t ">"

  So lexical tokens (tokens marked with '*' have associated string):
    String_t          *
	CommentTag_t      *
	ClosingTag_t      *
	OpeningTagBegin_t *
	OpeningTagEnd_t
	AttrName_t        *
*/

class XMLStream
{
public: // DATA
	Encoding* m_pEncoder; // pointer to encoder

public:
	// It might be useful: searching by attribute name, attribute value

	enum EMode
	{
		eRead,
		eWrite,
		eUnknown
	};

    enum EToken
    {   eTOKEN_STRING
	,   eTOKEN_QSTRING
    ,   eTOKEN_COMMENT_TAG
    ,   eTOKEN_CLOSING_TAG
    ,   eTOKEN_OPENING_TAG_B
    ,   eTOKEN_OPENING_TAG_E
	,   eTOKEN_EMPTY_TAG_E
    ,   eTOKEN_ATTRNAME
    ,   eTOKEN_EOF
	,	eTOKEN_BAD
    };

	// Construct empty stream.
	XMLStream ();

	virtual ~XMLStream ();

	// get token and return its id.
	int getToken(XMLString& str);	
	
	// Return true if the xml stream is empty.
	bool empty ();

	XMLString& tokenArg2() { return m_AttrValue; }

	int lastErrorCode() const { return m_ErrorCode; }
	const XMLString& lastErrorText() const { return m_ErrorText; }

	Encoding* setEncoding(XMLString& codeSet);

	XMLMapStrStr*& xmlReferences() { return m_pXMLReferences; }
	void setError(int code, const XMLString& str);
	void addToError(const XMLString& str);

	int putch(int ch);

protected:

	int getTag(XMLString& str);
	int getComment(XMLString& str);
	int getString(XMLString& str, const TCHAR* cp = NULL);
	int getQString(XMLString& str);
	int skipSpaces();
	int getAttr(XMLString& str);

	int getch();

    void ungetch(int ch) { m_pEncoder->ungetch(ch); }

	void init();

private:
    bool m_bInsideTag;     // true if current position inside tag
	XMLString m_AttrValue; // token second argument. Now used only for attribute token
	XMLString m_ErrorText; // text of last error
	int m_ErrorCode;	   // error's code
	XMLMapStrStr* m_pXMLReferences; // registered reference. Filled from XMLDocument::read only
};

#endif // _XmlStream_H_
