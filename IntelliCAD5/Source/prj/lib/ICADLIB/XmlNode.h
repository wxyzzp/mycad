#ifndef _XMLNODE_H_
#define _XMLNODE_H_

// Developer: VT

/*
	XMLNode contains an xml node. Notations and conventions :
	- xml node = <element( attList)*> content </element> OR <element( attList)* />
            content = is empty OR xmlnode content OR text content
*/

#ifndef _XML_H_
#include "xml.h"
#endif
#include <cstdarg>

// FORWARDS
class XMLDocument;

class XMLNode
{
public: // TYPES
	// attributes iterator
	typedef XMLMapStrStr::const_iterator attrIterator;

	// child nodes iterator
	typedef XMLNodes::const_iterator childIterator;

public:
	/*-------------------------------------------------------------------------*//**
	@return the name of the xml element ( = node)
	*//*--------------------------------------------------------------------------*/
	const XMLString& name() const { return m_name; }
	XMLString& name() { return m_name; }

	/*-------------------------------------------------------------------------*//**
	Get the text of the node
	*//*--------------------------------------------------------------------------*/
	XMLString& value() { return m_text; }

	/*-------------------------------------------------------------------------*//**
    Return pointer to attribute value
	*//*--------------------------------------------------------------------------*/
	XMLString* attribute(const XMLString& attrName);

	// add attribute
	// Return true if attribute successfully added.
	// If return false - attribute exists, value is overwriten
	bool addAttribute(const XMLString& attrName, const XMLString& attrValue);

	// return number of attributes
	int nAttribute() const;

	// function to iteration through attributes
	attrIterator attrBegin() const { return m_pAttrList->begin(); }
	attrIterator attrEnd() const { return m_pAttrList->end(); }

	// function to iteration through child nodes
	childIterator childBegin() const { return m_ChildNodes.begin(); }
	childIterator childEnd() const { return m_ChildNodes.end(); }

	/*-------------------------------------------------------------------------*//**
	@return the number of child nodes
	*//*--------------------------------------------------------------------------*/
	int nChild();

	/*-------------------------------------------------------------------------*//**
	Get a child node at an index in the array
	@param index		[in]
    @return pointer to node if index within range of the childnodes array, else NULL
	*//*--------------------------------------------------------------------------*/
	XMLNode* childNode(int index);

	/*-------------------------------------------------------------------------*//**
	Get a child node with this name. If there are more, the first one is returned.
	@param name			[in]
    @return pointer to node if a child node with that name was found, else NULL
	*//*--------------------------------------------------------------------------*/
	XMLNode* childNode(const XMLString &name);

	/*-------------------------------------------------------------------------*//**
    Append node to the ChildNode array.
    @param node [in] Pointer to node
    @return nothing
	*//*--------------------------------------------------------------------------*/
	void addChild(XMLNode* node);

	// constructor
	XMLNode (const XMLString &name_);

	// return clone of node
	XMLNode* clone(bool bRecurse);

	// assign this to yaNode
	void reset(const XMLNode& yaNode, bool bRecurse);

	// destroy node (destructor). bEraseFromParent - for internal use only
	void destroy(bool bEraseFromParent = true);

private:
	//construction, destruction
	~XMLNode ();
	XMLNode();

	void setComplete(bool bComplete) { m_bReadCompletely = bComplete; }
	void setDocument(XMLDocument* pDoc) { m_pDoc = pDoc; }
	void setParent(XMLNode* pParent) { m_pParent = pParent; }
	void readRemain();
	void eraseChild(int iChild);
	void eraseAttr(XMLString& attrName);
	void eraseAllChildren();

	friend class XMLDocument;

	//members
	XMLString m_name; // Tag name
	XMLString m_text; // Tag text
	XMLMapStrStr* m_pAttrList; // map of attributes
                          // (attribute name is key and attribute value is value)
	XMLNodes m_ChildNodes;// array of sub-nodes (or child-nodes)
	bool m_bRoot;
	bool m_bReadCompletely;// is node read completely from file?
	XMLDocument* m_pDoc;   // pointer to document
	XMLNode* m_pParent;    // pointer to parent node
};

#endif // _XMLNODE_H_
