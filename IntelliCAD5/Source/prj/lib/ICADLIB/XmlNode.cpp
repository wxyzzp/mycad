#include "Xml.h"
#include "XmlNode.h"
#include "xmldoc.h"

static XMLString s_InvalidString(false);

XMLString* XMLNode::attribute(const XMLString& attrName)
{
	const XMLMapStrStr& map = *m_pAttrList;
	if(map.find(attrName) == map.end())
		return &s_InvalidString;

	XMLMapStrStr::iterator it = m_pAttrList->find(attrName);

	return (it != m_pAttrList->end()) ? &it->second:&s_InvalidString;
}

int XMLNode::nAttribute() const
{
	return m_pAttrList->size();
}

/*-------------------------------------------------------------------------*/
int XMLNode::nChild()
{
	if(!m_bReadCompletely)
		readRemain();
	return m_ChildNodes.size ();
}

void XMLNode::readRemain()
{
	while(!m_bReadCompletely && m_pDoc)
	{
		if(!m_pDoc->read())
			break;
	}
}

/*-------------------------------------------------------------------------*/
XMLNode* XMLNode::childNode(int index)
{
	if(index >= m_ChildNodes.size() && !m_bReadCompletely)
		readRemain();
	return ( index >= 0 && index < m_ChildNodes.size()) ? m_ChildNodes[index]:NULL;
}

/*-------------------------------------------------------------------------*/
XMLNode* XMLNode::childNode(const XMLString &name)
{
	if(!m_bReadCompletely)
		readRemain();
	for ( int i = nChild(); i--;)
	{
		if ( m_ChildNodes[i]->name() == name )
			return m_ChildNodes[i];
	}

	return NULL;
}

void
XMLNode::addChild(XMLNode* pNode)
{
	XML_ASSERT(pNode->m_pParent == NULL);
	m_ChildNodes.push_back(pNode);
	pNode->m_pParent = this;
}

XMLNode::XMLNode(const XMLString &name_): m_name(name_), m_bRoot(false),
	m_bReadCompletely(true), m_pDoc(NULL), m_pParent(NULL)
{
	m_pAttrList = new XMLMapStrStr;
}

XMLNode::XMLNode(): m_bRoot(true), m_bReadCompletely(false), m_pDoc(NULL), m_pParent(NULL)
{
	m_pAttrList = new XMLMapStrStr;
}

XMLNode::~XMLNode()
{
	delete m_pAttrList;
	eraseAllChildren();
}

XMLNode*
XMLNode::clone(bool bRecurse)
{
	XMLNode* pNode = new XMLNode(name());

	pNode->reset(*this, bRecurse);

	return pNode;
}

void
XMLNode::reset(const XMLNode& yaNode, bool bRecurse)
{
	m_name = yaNode.name();
	m_text = yaNode.m_text;

	if(!bRecurse)
		return;

	// Clear and copy attributes
	{
		m_pAttrList->clear();
		for(XMLNode::attrIterator ai = yaNode.attrBegin(), aiEnd = yaNode.attrEnd(); ai != aiEnd; ++ai)
			(*m_pAttrList)[ai->first] = ai->second;
	}

	// Clear and copy child nodes
	{
		eraseAllChildren();
		for(XMLNode::childIterator ci = yaNode.childBegin(), ciEnd = yaNode.childEnd(); ci != ciEnd; ++ci)
			addChild((*ci)->clone(true));
	}
}

bool
XMLNode::addAttribute(const XMLString& attrName, const XMLString& attrValue)
{
	XMLString* pAttr = attribute(attrName);

	if(pAttr->isValid())
		*pAttr = attrValue;
	else
		(*m_pAttrList)[attrName] = attrValue;

	return !pAttr->isValid();
}

void
XMLNode::eraseChild(int iChild)
{
	if(iChild >= m_ChildNodes.size())
		return;

	m_ChildNodes[iChild]->destroy(false);
	m_ChildNodes[iChild] = m_ChildNodes.back();
	m_ChildNodes.pop_back();
}

void XMLNode::eraseAttr(XMLString& attrName)
{
	m_pAttrList->erase(attrName);
}

void XMLNode::destroy(bool bEraseFromParent)
{
	eraseAllChildren();

	if(bEraseFromParent && m_pParent)
	{
		for(int i = m_pParent->m_ChildNodes.size(); i--;)
		{
			if(m_pParent->m_ChildNodes[i] == this)
			{
				m_ChildNodes[i] = m_ChildNodes.back();
				m_ChildNodes.pop_back();
				break;
			}
		}
	}

	delete this;
}

void XMLNode::eraseAllChildren()
{
	for(int i = m_ChildNodes.size(); i--;)
		m_ChildNodes[i]->destroy(false);

	m_ChildNodes.clear();
}
