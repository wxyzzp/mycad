#ifndef _XMLDOC_H_
#define _XMLDOC_H_

// Developer: VT

#ifndef _XML_H_
#include "xml.h"
#endif

#pragma warning( disable: 4786 )

#ifndef _XMLNODE_H_
#include "XMLNode.h"
#endif

// FORWARDS
class XMLStream;

class XMLDocument
{
public:// TYPES
	typedef enum
	{	eXMLE_NONE = 0
	,	eXMLE_SOMETHING_WRONG
	} EXMLError;

	typedef enum
	{	eXML_OPT_NPREPROC_STREAM = 1
	,	eXML_OPT_NBEAUTY_OUTPUT  = 2
	,	eXML_OPT_NSKIP_SPACES_IN_BODY = 4
	} EXMLOption;

public:// FUNCTIONS
	XMLDocument(int options = 0);
	~XMLDocument();

	// associate file with this document (for reading or writing)
	bool setFile(const XMLString& fileName, bool bRead = true, bool bCloseOnDelete = true);

	// write full document to file
	bool write(int options = 0);

	// read file completely (or till first unrecoverable error)
	bool read();

	// return root node of dicument
	XMLNode& rootNode();

	// return last error code
	int lastErrorCode() const;

	// return last error text
	const XMLString& lastErrorText() const;

	// return symbolic name of encoding (value may not cincide with arg to setEncoding())
	const XMLString& encoding() const;

	// set encoding
	bool setEncoding(const XMLString& encoding);

	void addPredefinedMacro(const XMLString& name, const XMLString& value);
	bool removePredefinedMacro(const XMLString& name);

protected: // FUNCTIONS
	// read till next closed tag
	bool readNext();

	// return true if need for inserting preprocessor
	bool isNeedInsertPreProcessor() const { return m_bRead && ((m_Options & eXML_OPT_NPREPROC_STREAM) == 0); }

private: // FUNCTIONS
	bool beSureEncoder(XMLString encoderName = XMLString());

private: // DATA
	XMLNode m_rootNode;   // root node of document
	XMLString m_Version;  // version (not filled)
	XMLString m_FileName; // name of file
	FILE* m_pFile;        // pointer to file
	XMLNodes m_NodeStack; // stack of nodes (for internal use only - read())
	XMLStream* m_pStream; // pointer to stream
	bool m_bRead;         // open for reading?
	bool m_bCloseOnDelete;// close on delete
	XMLMapStrStr m_SymMap;// registered aliases (now only special symbols here)
	XMLString m_Encoding; // encoding name
	int m_Options;		  // bitwise options
	XMLMapStrStr m_PreProcPredefinedMacros;// map of predefined macro for preprocessor
};

#endif //_XMLDOC_H_
