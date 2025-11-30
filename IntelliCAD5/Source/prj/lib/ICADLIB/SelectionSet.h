// File  : <DevDir>\source\prj\lib\icadlib\SelectionSet.h
//
// CSelectionSet class
//

#pragma once

#pragma warning (disable : 4786)

#include <list>
#include <map>
#include <vector>
#include "objhandle.h"

using std::list;
using std::map;
using std::vector;

typedef list<long>					SSlist;
typedef SSlist::iterator			SSLiter;
typedef map<db_objhandle, SSLiter>	SSmap;
typedef SSmap::iterator				SSMiter;
typedef vector<long>				SSvector;


class CSelectionSet
{
public:
	CSelectionSet() : m_drawing(0L), m_bOrdered(false)
	{}
	~CSelectionSet()
	{}

	SSlist		m_asList;
	SSmap		m_asMap;
	SSvector	m_asVector;
	long		m_drawing;
	bool		m_bOrdered, m_bAscending;
};
